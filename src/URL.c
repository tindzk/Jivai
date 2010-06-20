#include "URL.h"

URL_Parts URL_Parse(String url) {
	URL_Parts res;

	res.scheme = HeapString(0);
	res.host   = HeapString(0);
	res.port   = 0;
	res.path   = HeapString(128);

	String_Append(&res.path, '/');

	URL_State state = URL_State_Scheme;

	String buf = HeapString(256);

	for (size_t i = 0; i < url.len; i++) {
		switch (state) {
			case URL_State_Scheme:
				if (String_EndsWith(&buf, String("://"))) {
					String_Copy(&res.scheme, buf, 0, -3);
					buf.len = 0;

					state = URL_State_Host;
				}

				break;

			case URL_State_Host:
				if (String_EndsWith(&buf, String(":"))) {
					state = URL_State_Port;
				} else if (String_EndsWith(&buf, String("/"))) {
					state = URL_State_Path;
				}

				if (state != URL_State_Host) {
					String_Copy(&res.host, buf, 0, -1);
					buf.len = 0;
				}

				break;

			case URL_State_Port:
				if (String_EndsWith(&buf, String("/"))) {
					res.port = Integer_ParseString(buf);
					buf.len = 0;

					state = URL_State_Path;
				}

				break;

			case URL_State_Path:
				break;
		}

		String_Append(&buf, url.buf[i]);
	}

	if (state == URL_State_Path) {
		String_Append(&res.path, buf);
	}

	return res;
}
