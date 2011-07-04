#import "Math.h"
#import "String.h"
#import "Exception.h"
#import "MurmurHash3.h"

#define self HashTable

/* HashTable implements a fast string/structure mapping employing
 * MurmurHash3 hashes.
 *
 * Hint: For better performance, disable Exception_Assert.
 *
 * The term "bucket" stands for an entry in this->entries. It is
 * always an integer value, i.e. the entry's absolute offset.
 *
 * "bucket" shall not be confused with "entry" which is the bucket's
 * pointer pendant.
 */

exc(TableIsFull);

/* Set to `0' when no resizes are desired, e.g. when the final size is
 * already known. Consider that if the table exceeds its allowed size,
 * a `TableIsFull' exception is thrown.
 *
 * Set to `1' when to resize only if the hash table is full.
 *
 * Resizes are time-consuming and in most cases it's more appropriate
 * to use a higher value for the initial size.
 */
#ifndef HashTable_MaximumLoad
#define HashTable_MaximumLoad 0.85
#endif

/* These settings will only have an impact on the insertion speed.
 * An additional bitmap can drastically speed up insertion but comes
 * at the cost of a slightly higher memory overhead. For a total of
 * 1 million entries, the bitmap will have a size of 122 KiB.
 *
 * The bitmap will be used to find an empty entry. The number of
 * iterations and comparisons is reduced by factor 32. Another
 * optimization includes the use of the `clz' instruction to find
 * the free bucket within a bitmap line without branching.
 *
 * OptimizedBitmap does further optimizations. First, it splits the
 * loop into two going into both directions relative from the bucket
 * that collided. Second, the range of the first loop (which goes
 * upward) is restricted when previous calls found that a certain range
 * only contains occupied buckets, i.e. when the first loop fell through
 * the whole range will be marked unusable and will be ignored the next
 * time. These optimizations are useful the higher the load of the table
 * because free buckets are getting rarer then.
 */
#define HashTable_Type_LowMemory       0
#define HashTable_Type_Bitmap          1
#define HashTable_Type_OptimizedBitmap 2

#ifndef HashTable_Type
#define HashTable_Type HashTable_Type_OptimizedBitmap
#endif

/* As for uint, only 1/32 of the normally needed iterations
 * (HashTable_Type_LowMemory) are required.
 */
#define HashTable_BitmapType uint
#define HashTable_BitmapBits (8 * sizeof(HashTable_BitmapType))

/* Entries are chained; `next' refers to the next bucket. */
record(ref(Entry)) {
	String key;
	u32 next;
	byte data[];
};

class {
	void *entries; /* Hash table buffer.               */
	u32 mask;      /* Mask to obtain bucket from hash. */
	u32 size;      /* Number of allocated entries.     */
	u32 count;     /* Number of occupied entries.      */
	u16 valueSize; /* Size of each value.              */

#if HashTable_Type == HashTable_Type_Bitmap || \
	HashTable_Type == HashTable_Type_OptimizedBitmap

	/* `bitmap' maps each bucket in `entries' while stating whether the
	 * corresponding entry is free or occupied.
	 */
	ref(BitmapType) *bitmap;
	u32 cntBitmap;

#endif

#if HashTable_Type == HashTable_Type_OptimizedBitmap
	u32 bitmapUpper;
#endif
};

static alwaysInline def(double, getLoad) {
	return (double) this->count / this->size;
}

rsdef(self, new, u32 len, u16 valueSize);
def(void, destroy);
def(ref(Entry) *, getFirst);
def(ref(Entry) *, getNext, ref(Entry) *cur);
def(void *, lookup, RdString key);
def(void, insert, RdString key, void *value);

#undef self
