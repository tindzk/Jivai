#import "Expander.h"

/* Very simple glob() implementation. Only one placeholder is
 * allowed. Escaping and expanding (e.g. {a, b, c}) might be
 * implemented in the future.
 */

#define self Folder_Expander

rsdef(self, new, ref(OnMatch) onMatch) {
	return (self) {
		.onMatch = onMatch
	};
}

/* Returns true if the pattern is a valid expression. */
def(bool, scan, RdString pattern) {
	ssize_t star = String_Find(pattern, '*');

	if (star == String_NotFound) {
		return false;
	}

	ssize_t slash = String_ReverseFind(String_Slice(pattern, star), '/');

	RdString path, left;

	if (slash == String_NotFound) {
		path = $("./");
		left = String_Slice(pattern, 0, star);
	} else {
		slash += star;

		path = String_Slice(pattern, 0, slash + 1);
		left = String_Slice(pattern, slash + 1, star - slash - 1);
	}

	RdString right = String_Slice(pattern, star + 1);

	Folder_Entry item;

	Folder folder = Folder_new(path);

	while (Folder_read(&folder, &item)) {
		if (item.type != Folder_ItemType_Symlink &&
			item.type != Folder_ItemType_Regular)
		{
			continue;
		}

		if (String_BeginsWith(item.name, left) &&
			String_EndsWith(item.name, right))
		{
			callback(this->onMatch, path, item.name);
		}
	}

	Folder_destroy(&folder);

	return true;
}
