#import "Locale.h"

#define self Locale

Singleton(self);
SingletonDestructor(self);

rsdef(self, New) {
	return (self) {
		.tbl = HashTable_new(1024, sizeof(ref(Item)))
	};
}

static sdef(void, destroyItems, HashTable *tbl) {
	for (HashTable_Entry *cur = HashTable_getFirst(tbl);
							cur != null;
							cur = HashTable_getNext(tbl, cur))
	{
		ref(Item) *data = (void *) cur->data;
		String_Destroy(&data->dest);
	}
}

def(void, Destroy) {
	scall(destroyItems, &this->tbl);
	HashTable_destroy(&this->tbl);
}

sdef(String, decode, RdString s) {
	String res = String_New(s.len);

	fwd(i, s.len) {
		if (i + 1 < s.len && s.buf[i] == '\\') {
			if (s.buf[i + 1] == 'n') {
				String_Append(&res, '\n');
			} else {
				String_Append(&res, s.buf[i + 1]);
			}

			i++;
		} else {
			String_Append(&res, s.buf[i]);
		}
	}

	return res;
}

sdef(String, encode, RdString s) {
	String res = String_New(s.len * 1.15);

	fwd(i, s.len) {
		if (s.buf[i] == '\n') {
			String_Append(&res, $("\\n"));
		} else if (s.buf[i] == '\\') {
			String_Append(&res, $("\\\\"));
		} else {
			String_Append(&res, s.buf[i]);
		}
	}

	return res;
}

sdef(void, createInitial, RdString binary, RdString path) {
	ELF elf = ELF_New(binary);

	File file = File_New(path,
		FileStatus_WriteOnly |
		FileStatus_Truncate  |
		FileStatus_Create);

	RdBuffer sect = ELF_GetSection(&elf, $(".locale"));

	/* Use a hash table to skip duplicates. */
	HashTable tbl = HashTable_new(sect.len / sizeof(RdString), 0);

	for (RdString *cur = sect.ptr; (void *) cur < sect.ptr + sect.len; cur++) {
		if (HashTable_lookup(&tbl, *cur) != null) {
			continue;
		}

		String encoded = scall(encode, *cur);

		File_Write(&file, encoded.rd);
		File_Write(&file, $("\n\n\n"));

		String_Destroy(&encoded);

		HashTable_insert(&tbl, *cur, null);
	}

	HashTable_destroy(&tbl);

	File_Destroy(&file);

	ELF_Destroy(&elf);
}

sdef(void, check, RdString binary, RdString path, ref(OnCheckError) cb) {
	ELF elf = ELF_New(binary);
	RdBuffer sect = ELF_GetSection(&elf, $(".locale"));

	HashTable bin = HashTable_new(sect.len / sizeof(RdString), 0);
	HashTable lng = HashTable_new(sect.len / sizeof(RdString), sizeof(ref(Item)));

	scall(load, path, &lng);

	/* Find messages that aren't defined in the language file. */
	for (RdString *cur = sect.ptr; (void *) cur < sect.ptr + sect.len; cur++) {
		if (HashTable_lookup(&bin, *cur) != null) {
			/* Ignore duplicates. */
			continue;
		}

		HashTable_insert(&bin, *cur, null);

		if (HashTable_lookup(&lng, *cur) == null) {
			callback(cb, ref(CheckError_Missing), *cur);
		}
	}

	for (HashTable_Entry *cur = HashTable_getFirst(&lng);
							cur != null;
							cur = HashTable_getNext(&lng, cur))
	{
		ref(Item) *item = (void *) cur->data;

		if (item->dest.len == 0) {
			/* Message wasn't translated yet. */
			callback(cb, ref(CheckError_Empty), cur->key.rd);
		} else if (HashTable_lookup(&bin, cur->key.rd) == null) {
			/* Message doen't exist in the binary. */
			callback(cb, ref(CheckError_Invalid), cur->key.rd);
		}
	}

	scall(destroyItems, &lng);

	HashTable_destroy(&lng);
	HashTable_destroy(&bin);

	ELF_Destroy(&elf);
}

sdef(void, dump, RdString path, ref(OnMessage) cb) {
	ELF elf = ELF_New(path);
	RdBuffer sect = ELF_GetSection(&elf, $(".locale"));

	for (RdString *cur = sect.ptr; (void *) cur < sect.ptr + sect.len; cur++) {
		callback(cb, *cur);
	}

	ELF_Destroy(&elf);
}

overload sdef(void, load, RdString path, HashTable *tbl) {
	String file = File_GetContents(path);

	RdString iter = $("");
	while (String_Split(file.rd, $("\n\n"), &iter)) {
		RdString item = String_Trim(iter);

		if (item.len == 0) {
			continue;
		}

		/* Skip comments. */
		while (item.buf[0] == '#') {
			ssize_t ofs = String_Find(item, '\n');

			if (ofs == String_NotFound) {
				goto skip;
			}

			item = String_Trim(String_Slice(item, ofs + 1));
		}

		when (skip) {
			continue;
		}

		RdString src, dest;
		if (!String_Parse($("%\n%"), item, &src, &dest)) {
			src  = item;
			dest = $("");
		} else {
			/* Only process the first and second line. */
			ssize_t ofs = String_Find(dest, '\n');
			if (ofs != String_NotFound) {
				dest = String_Slice(dest, 0, ofs);
			}
		}

		String dsrc = scall(decode, src);

		if (HashTable_lookup(tbl, dsrc.rd) != null) {
			String_Destroy(&dsrc);
			String_Destroy(&file);
			throw(Duplicate);
		}

		ref(Item) entry = {
			.dest = scall(decode, dest)
		};

		HashTable_insert(tbl, dsrc.rd, &entry);

		String_Destroy(&dsrc);
	}

	String_Destroy(&file);
}

overload def(void, load, RdString path) {
	scall(destroyItems, &this->tbl);
	HashTable_clear(&this->tbl);
	scall(load, path, &this->tbl);
}

def(RdString, getTranslation, RdString value) {
	ref(Item) *item = HashTable_lookup(&this->tbl, value);

	if (item == null) {
		return value;
	}

	return item->dest.rd;
}
