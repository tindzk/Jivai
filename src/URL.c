#import "URL.h"

#define self URL

sdef(ref(Parts), Parse, RdString url) {
	ref(Parts) res = {
		.port = 0
	};

	ssize_t pos = String_Find(url, $("://"));

	if (pos == String_NotFound) {
		throw(SchemeMissing);
	}

	res.scheme = String_Slice(url, 0, pos);

	pos += $("://").len;

	ssize_t pos2 = String_Find(url, pos, '/');

	if (pos2 == String_NotFound) {
		res.host = String_Slice(url, pos);
	} else {
		res.host = String_Slice(url, pos, pos2 - pos);
		res.path = String_Slice(url, pos2);

		ssize_t pos3 = String_ReverseFind(res.host, ':');

		if (pos3 != String_NotFound) {
			res.port = UInt16_Parse(String_Slice(res.host, pos3 + 1));
			res.host = String_Slice(res.host, 0, pos3);
		}

		ssize_t pos4 = String_ReverseFind(res.path, '#');

		if (pos4 != String_NotFound) {
			res.fragment = String_Slice(res.path, pos4 + 1);
			res.path     = String_Slice(res.path, 0, pos4);
		}
	}

	return res;
}
