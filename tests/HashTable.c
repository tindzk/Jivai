#import <Path.h>
#import <String.h>
#import <HashTable.h>

#import "TestSuite.h"

#define self tsHashTable

class {

};

tsRegister("HashTable") {
	return true;
}

record(ref(Entry)) {
	RdString key;
};

tsCase(Acute, "Initialization") {
	HashTable table = HashTable_new(0, sizeof(ref(Entry)));
	HashTable_destroy(&table);
}

tsCase(Acute, "Inserting") {
	RdString keys[] = {
		$("H"),
		$("e"),
		$("l"),
		$("o"),
		$("W"),
		$("r"),
		$("d"),
		$("Hello"),
		$("World")
	};

	HashTable table = HashTable_new(0, sizeof(ref(Entry)));

	fwd(i, nElems(keys)) {
		ref(Entry) entry = {
			.key = keys[i]
		};

		HashTable_insert(&table, keys[i], &entry);
	}

	fwd(i, nElems(keys)) {
		ref(Entry) *entry = HashTable_lookup(&table, keys[i]);

		Assert($("Found entry"), entry != null);

		if (entry != null) {
			Assert($("Correct key"), String_Equals(entry->key, keys[i]));
		}
	}

	size_t cnt = 0;

	for (HashTable_Entry *cur = HashTable_getFirst(&table);
							cur != null;
							cur = HashTable_getNext(&table, cur))
	{
		cnt++;
	}

	Assert($("Count"), cnt == nElems(keys));

	HashTable_destroy(&table);
}
