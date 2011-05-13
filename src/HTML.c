#import "HTML.h"

#define self HTML

overload sdef(void, Unescape, String *str) {
	if (str->len == 0) {
		return;
	}

	if (str->buf[0] != '"' && str->buf[0] != '\'') {
		return;
	}

	int idx = 0;

	char prev  = '\0';
	char quote = str->buf[0];

	for (size_t i = 1; i < str->len; i++) {
		if (prev == '\\') {
			if (str->buf[i] == quote) {
				str->buf[idx] = str->buf[i];
				idx++;
				i++;
			}
		} else if (prev != '\0') {
			str->buf[idx] = prev;
			idx++;
		}

		prev = str->buf[i];
	}

	str->len = idx;
}

overload sdef(String, Unescape, RdString str) {
	String res = String_Clone(str);
	scall(Unescape, &res);
	return res;
}

sdef(String, Escape, RdString str) {
	String res = String_New(str.len + 128);

	res.buf[0] = '"';
	res.len++;

	fwd(i, str.len) {
		switch (str.buf[i]) {
			case '"':
				String_Append(&res, $("\\"));

			default:
				String_Append(&res, str.buf[i]);
		}
	}

	String_Append(&res, $("\""));

	return res;
}
