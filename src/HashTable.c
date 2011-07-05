#import "HashTable.h"

/* Influenced by Joshua Haberman's hash table implementation for upb. */

#define self HashTable

static alwaysInline sdef(u32, getBitmapMask, u32 n) {
	return 1 << (HashTable_BitmapBits - 1 - n);
}

static alwaysInline def(u32, getEntrySize) {
	return sizeof(ref(Entry)) + this->valueSize;
}

/* Resolves a bucket to its pointer without prior validation. */
static alwaysInline def(ref(Entry) *, getEntry, s32 bucket) {
	return this->entries + bucket * call(getEntrySize);
}

rsdef(self, new, u32 len, u16 valueSize) {
	self res;

	/* `len' must be a multiple of two. `size' is set initially to one
	 * because `len' may be zero which would result in skipping the
	 * loop.
	 */
	res.size = 1;
	for (size_t i = 1; res.size < len; i++) {
		res.size = (1 << i);
	}

	res.count     = 0;
	res.mask      = res.size - 1;
	res.valueSize = valueSize;
	res.entries   = Memory_New(res.size * scall(getEntrySize, &res));

	fwd(i, res.size) {
		ref(Entry) *entry = scall(getEntry, &res, i);

		entry->key  = String_New(0);
		entry->next = (u32) -1;
	}

#if HashTable_Type == HashTable_Type_Bitmap || \
	HashTable_Type == HashTable_Type_OptimizedBitmap

	res.cntBitmap = res.size / ref(BitmapBits) + 1;
	res.bitmap    = Memory_New(res.cntBitmap * sizeof(ref(BitmapType)));

	/* Mark all buckets as available. */
	fwd(i, res.cntBitmap) {
		res.bitmap[i] = ~0;

		/* Items for `i' are ranging from `lower' to `upper'. Ensure
		 * that the bitmap doesn't have more items than can be possibly
		 * addressed in `entries'.
		 */
		size_t lower = i * ref(BitmapBits);
		size_t upper = lower + ref(BitmapBits) - 1;

		/* Disable all bits pointing to unadressable entries. */
		if (upper >= res.size) {
			uint disable = upper - res.size + 1;

			fwd(j, disable) {
				/* Refer to the most significant bit. */
				uint bit = HashTable_BitmapBits - j - 1;
				res.bitmap[i] &= ~scall(getBitmapMask, bit);
			}

			for (i++; i < res.cntBitmap; i++) {
				res.bitmap[i] = 0;
			}
		}
	}

#endif

#if HashTable_Type == HashTable_Type_OptimizedBitmap
	res.bitmapUpper = res.cntBitmap - 1;
#endif

	return res;
}

def(void, destroy) {
#if HashTable_Type == HashTable_Type_Bitmap || \
	HashTable_Type == HashTable_Type_OptimizedBitmap
	Memory_Destroy(this->bitmap);
#endif

	for (ref(Entry) *cur = call(getFirst); cur != null; cur = call(getNext, cur)) {
		String_Destroy(&cur->key);
	}

	Memory_Destroy(this->entries);
}

def(void, clear) {
	self table = scall(new, this->size, this->valueSize);
	scall(destroy, this);
	*this = table;
}

static def(u32, getBucket, RdString key) {
	u32 hash;
	MurmurHash3_x86_32(key.buf, key.len, 0, &hash);

	return (hash & this->mask);
}

def(ref(Entry) *, getFirst) {
	ref(Entry) *entry = call(getEntry, -1);
	return call(getNext, entry);
}

def(ref(Entry) *, getNext, ref(Entry) *cur) {
	ref(Entry) *end = call(getEntry, this->size);

	for (;;) {
		cur = (void *) cur + call(getEntrySize);

		if (cur == end) {
			return null;
		} else if (cur->key.len != 0) {
			return cur;
		}
	}

	return null;
}

def(void *, lookup, RdString key) {
	u32 cur = call(getBucket, key);

	for (;;) {
		ref(Entry) *entry = call(getEntry, cur);

		if (entry->key.len != 0 && String_Equals(entry->key.rd, key)) {
			return entry->data;
		} else if (entry->next == (u32) -1) {
			break;
		}

		cur = entry->next;
	}

	return null;
}

/* Finds a free bucket and marks it as "occupied". */
static def(u32, allocBucket, u32 cur) {
#if HashTable_Type == HashTable_Type_LowMemory
	fwd(i, this->size) {
		ref(Entry) *entry = call(getEntry, i);

		if (entry->key.len == 0) {
			return i;
		}
	}
#elif HashTable_Type == HashTable_Type_Bitmap
	fwd(i, this->cntBitmap) {
		if (this->bitmap[i] != 0) {
			int j = __builtin_clz(this->bitmap[i]);

			assert((this->bitmap[i] & scall(getBitmapMask, j)) != 0);
			assert(call(getEntry, i * ref(BitmapBits) + j)->key.len == 0);

			this->bitmap[i] &= ~scall(getBitmapMask, j);

			return i * ref(BitmapBits) + j;
		}
	}
#elif HashTable_Type == HashTable_Type_OptimizedBitmap
	size_t ofs = cur / ref(BitmapBits);

	for (size_t i = ofs; i <= this->bitmapUpper; i++) {
		if (this->bitmap[i] != 0) {
			int j = __builtin_clz(this->bitmap[i]);
			this->bitmap[i] &= ~scall(getBitmapMask, j);
			return i * ref(BitmapBits) + j;
		}
	}

	/* Limit the upper range to save some iterations next time. */
	if (this->bitmapUpper > ofs - 1) {
		this->bitmapUpper = ofs - 1;
	}

	for (ssize_t i = ofs - 1; i >= 0; i--) {
		if (this->bitmap[i] != 0) {
			int j = __builtin_clz(this->bitmap[i]);
			this->bitmap[i] &= ~scall(getBitmapMask, j);
			return i * ref(BitmapBits) + j;
		}
	}
#endif

	return (u32) -1;
}

/* There are three situations this function has to deal with:
 *
 *  1) The entry is still available, i.e. up until now there hasn't
 *     been another key yielding the same bucket and the entry hasn't
 *     been used as a result of a allocBucket() call yet. Consequently,
 *     it is safe to just use the place.
 *
 *  2) The entry is occupied but its key was the first one that mapped
 *     to this bucket. Therefore, we assume that the key is more likely
 *     to be looked-up than the to-be-inserted one limiting the needed
 *     iterations to one. As our key yields the same bucket, we just
 *     need to find an empty entry using allocBucket() and append it to
 *     the chain.
 *
 *  3) The entry is occupied but it is not the first inserted entry
 *     yielding this bucket, i.e. this entry was returned by allocBucket().
 *     As our key directly maps to this bucket, we can improve look-up
 *     speed by taking its place. We need to move the existing contents
 *     somewhere else, find the entry which points to it, and adjust the
 *     its bucket accordingly.
 *
 * In summary, an entry E with the key K will never change its place
 * given that the bucket of K was unused during insertion. A new entry F
 * with a key deviating from K but the same bucket as E will be stored
 * in a free entry G which is then appended to E's chain. A new entry H
 * which has the same bucket as the former free entry G will use G's
 * place and G will be moved to another free entry.
 */
static def(ref(Entry) *, allocateEntry, RdString key) {
	assert(call(lookup, key) == null);

	this->count++;

	u32 bucket = call(getBucket, key);

	ref(Entry) *entry = call(getEntry, bucket);

	if (entry->key.len == 0) {
#if HashTable_Type == HashTable_Type_Bitmap || \
	HashTable_Type == HashTable_Type_OptimizedBitmap
		/* Mark as occupied. */
		this->bitmap[bucket / ref(BitmapBits)] &=
			~scall(getBitmapMask, bucket % ref(BitmapBits));
#endif

		return entry;
	}

	/* A collision occurred; there's already an entry for this bucket. */

	u32 realBucket = call(getBucket, entry->key.rd);

	if (bucket == realBucket) {
		/* It is the first inserted entry that mapped to this
		 * bucket. As a lookup for its key will directly yield
		 * this bucket, it's best to keep it in its position.
		 */

		/* Find an empty bucket. */
		u32 empty = call(allocBucket, bucket);
		assert(empty != (u32) -1);

		/* Follow chain and append our empty bucket. */
		for (;;) {
			if (entry->next == (u32) -1) {
				entry->next = empty;

				/* Resolve the new entry. */
				return call(getEntry, empty);
			}

			entry = call(getEntry, entry->next);
		}
	} else {
		/* Find an empty entry and move the existing contents there. */
		u32 idxNew = call(allocBucket, bucket);
		assert(idxNew != (u32) -1);
		ref(Entry) *new = call(getEntry, idxNew);
		Memory_Copy(new, entry, call(getEntrySize));

		/* As the entry is not the first inserted entry that mapped
		 * to this bucket, we will find `bucket' in the `realBucket'
		 * chain and update the reference to `bucket' to its new
		 * destination, i.e. idxNew.
		 */
		for (ref(Entry) *evictee = call(getEntry, realBucket); true; evictee = call(getEntry, evictee->next)) {
			assert(evictee->key.len != 0);
			assert(evictee->next != (u32) -1);

			if (evictee->next == bucket) {
				evictee->next = idxNew;

				/* `entry' is now safe to be used. */
				return entry;
			}
		}
	}

	return null;
}

def(void, insert, RdString key, void *value) {
	assert((value == null) ^ (this->valueSize != 0));

	double load = call(getLoad);

	if (ref(MaximumLoad) == 0 && (this->count + 1) == this->size) {
		throw(TableIsFull);
	} else if (load >= ref(MaximumLoad)) {
		/* Adding one element would exceed the allowed load. Create a
		 * new hash table doubling its size and add all elements to it.
		 */
		self table = scall(new, this->size * 2, this->valueSize);

		for (ref(Entry) *cur = call(getFirst); cur != null; cur = call(getNext, cur)) {
			ref(Entry) *new = scall(allocateEntry, &table, cur->key.rd);
			assert(new != null);

			/* To improve performance, we don't clone the string here. */
			new->key = cur->key;
			cur->key = String_New(0);

			new->next = (u32) -1;
			Memory_Move(new->data, cur->data, this->valueSize);
		}

		scall(destroy, this);
		*this = table;
	}

	ref(Entry) *entry = call(allocateEntry, key);
	assert(entry != null);

	entry->key  = String_Clone(key);
	entry->next = (u32) -1;

	Memory_Copy(entry->data, value, this->valueSize);

	assert(call(lookup, key) == entry->data);
}
