#import "Ecriture.h"

#define self Ecriture

rsdef(CarrierString, Unescape, RdString str) {
	CarrierString res = String_ToCarrier(RdString_Exalt(str));

	fwd(i, str.len) {
		if (str.buf[i] == '`') {
			if (i + 1 < str.len && str.buf[i + 1] == '`') {
				i++;
			}

			if (res.omni) {
				res = String_ToCarrier(String_New(str.len - 1));
				Memory_Copy(res.buf, str.buf, i);
				res.len = i;
				continue;
			}
		}

		if (!res.omni) {
			res.buf[res.len] = str.buf[i];
			res.len++;
		}
	}

	return res;
}

rsdef(CarrierString, Escape, RdString str, ref(TokenType) type) {
	assert(type == ref(TokenType_Option)    /* Escape "`", "=" and "]". */
		|| type == ref(TokenType_AttrName)  /* Escape "`" and "]". */
		|| type == ref(TokenType_AttrValue) /* Escape "`" and "]". */
		|| type == ref(TokenType_Value));   /* Escape "`". */

	CarrierString res = String_ToCarrier(RdString_Exalt(str));

	fwd(i, str.len) {
		if (str.buf[i] == '`') {
			goto escape;
		} else if (type == ref(TokenType_Option)) {
			if (str.buf[i] == '=' || str.buf[i] == ']') {
				goto escape;
			}
		} else if (type == ref(TokenType_AttrName)
				|| type == ref(TokenType_AttrValue))
		{
			if (str.buf[i] == ']') {
				goto escape;
			}
		}

		when (escape) {
			if (res.omni) {
				res = String_ToCarrier(String_New(str.len + 1));
				Memory_Copy(res.buf, str.buf, i);
				res.len = i;
			}

			String_Append((String *) &res, '`');
		}

		if (!res.omni) {
			String_Append((String *) &res, str.buf[i]);
		}
	}

	return res;
}
