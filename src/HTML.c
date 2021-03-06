#import "HTML.h"

#define self HTML

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

/* Non-nestable tags. */
static RdString tags[] = {
	$("area"),
	$("base"),
	$("br"),
	$("hr"),
	$("img"),
	$("input"),
	$("link"),
	$("meta"),
	$("param")
};

rsdef(bool, Equals, RdString a, RdString b) {
	if (a.len != b.len) {
		return false;
	}

	fwd(i, a.len) {
		if (Char_ToLower(a.buf[i]) != Char_ToLower(b.buf[i])) {
			return false;
		}
	}

	return true;
}

rsdef(bool, IsNestable, RdString tagName) {
	fwd(i, nElems(tags)) {
		if (scall(Equals, tags[i], tagName)) {
			return false;
		}
	}

	return true;
}
