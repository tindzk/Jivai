#import "URL.h"

#define self URL

set(ref(State)) {
	ref(State_Scheme),
	ref(State_Host),
	ref(State_Port),
	ref(State_Path),
	ref(State_Fragment)
};

sdef(ref(Parts), Parse, RdString url) {
	ref(Parts) res;

	res.scheme   = String_New(0);
	res.host     = String_New(0);
	res.port     = 0;
	res.path     = String_New(128);
	res.fragment = String_New(32);

	String_Append(&res.path, '/');

	ref(State) state = ref(State_Scheme);

	String buf = String_New(256);

	for (size_t i = 0; i < url.len; i++) {
		switch (state) {
			case ref(State_Scheme):
				if (String_EndsWith(buf.rd, $("://"))) {
					String_Copy(&res.scheme,
						String_Slice(buf.rd, 0, -3));

					buf.len = 0;

					state = ref(State_Host);
				}

				break;

			case ref(State_Host):
				if (String_EndsWith(buf.rd, $(":"))) {
					state = ref(State_Port);
				} else if (String_EndsWith(buf.rd, $("/"))) {
					state = ref(State_Path);
				}

				if (state != ref(State_Host)) {
					String_Copy(&res.host,
						String_Slice(buf.rd, 0, -1));

					buf.len = 0;
				}

				break;

			case ref(State_Port):
				if (String_EndsWith(buf.rd, $("/"))) {
					res.port = Int16_Parse(buf.rd);
					buf.len = 0;

					state = ref(State_Path);
				}

				break;

			case ref(State_Path):
				if (String_EndsWith(buf.rd, $("#"))) {
					String_Append(&res.path,
						String_Slice(buf.rd, 0, -1));
					state = ref(State_Fragment);
					buf.len = 0;
				}

				break;

			case ref(State_Fragment):
				break;
		}

		String_Append(&buf, url.buf[i]);
	}

	if (buf.len > 0) {
		if (state == ref(State_Path)) {
			String_Append(&res.path, buf.rd);
		} else if (state == ref(State_Fragment)) {
			String_Append(&res.fragment, buf.rd);
		}
	}

	String_Destroy(&buf);

	return res;
}

sdef(void, Parts_Destroy, ref(Parts) *parts) {
	String_Destroy(&parts->scheme);
	String_Destroy(&parts->host);
	String_Destroy(&parts->path);
	String_Destroy(&parts->fragment);
}
