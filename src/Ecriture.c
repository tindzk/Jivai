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

rsdef(CarrierString, Escape, RdString str) {
	CarrierString res = String_ToCarrier(RdString_Exalt(str));

	fwd(i, str.len) {
		if (str.buf[i] == '`' && res.omni) {
			res = String_ToCarrier(String_New(str.len + 1));
			Memory_Copy(res.buf, str.buf, i + 1);
			res.len = i + 1;
		}

		if (!res.omni) {
			res.buf[res.len] = str.buf[i];
			res.len++;
		}
	}

	return res;
}
