#import "HTML.h"

#define self HTML

rsdef(CarrierString, Unescape, RdString str) {
	if (str.buf[0] != '"' && str.buf[0] != '\'') {
		return String_ToCarrier(RdString_Exalt(str));
	}

	char quote = str.buf[0];

	str = String_Slice(str, 1, -1);

	CarrierString res = String_ToCarrier(RdString_Exalt(str));

	fwd(i, str.len) {
		if (str.buf[i] == '\\') {
			if (res.omni) {
				res = String_ToCarrier(String_New(str.len - 1));
				Memory_Copy(res.buf, str.buf, i);
				res.len = i;
			}

			if (i + 1 < str.len && str.buf[i + 1] == quote) {
				i++;
			}
		}

		if (!res.omni) {
			res.buf[res.len] = str.buf[i];
			res.len++;
		}
	}

	return res;
}

rsdef(String, Escape, RdString str) {
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
