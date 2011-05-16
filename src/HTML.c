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

rsdef(CarrierString, Escape, RdString str) {
	if (str.len == 0) {
		return String_ToCarrier($$("\"\""));
	}

	CarrierString res = String_ToCarrier(RdString_Exalt(str));

	fwd(i, str.len) {
		if (str.buf[i] != '.' &&
			str.buf[i] != ':' &&
			str.buf[i] != '-' &&
			str.buf[i] != '_' &&
			!Char_IsAlpha(str.buf[i]) &&
			!Char_IsDigit(str.buf[i]))
		{
			if (res.omni) {
				res = String_ToCarrier(String_New(str.len + 3));

				res.buf[0] = '"';
				Memory_Copy(res.buf + 1, str.buf, i);

				res.len = i + 1;
			}
		}

		if (!res.omni) {
			if (str.buf[i] == '"') {
				String_Append((String *) &res, '\\');
			}

			String_Append((String *) &res, str.buf[i]);
		}
	}

	if (!res.omni) {
		String_Append((String *) &res, $("\""));
	}

	return res;
}
