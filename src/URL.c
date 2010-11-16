#import "URL.h"

set(ref(State)) {
	ref(State_Scheme),
	ref(State_Host),
	ref(State_Port),
	ref(State_Path),
	ref(State_Fragment)
};

sdef(ref(Parts), Parse, String url) {
	ref(Parts) res;

	res.scheme   = HeapString(0);
	res.host     = HeapString(0);
	res.port     = 0;
	res.path     = HeapString(128);
	res.fragment = HeapString(32);

	String_Append(&res.path, '/');

	ref(State) state = ref(State_Scheme);

	String buf = HeapString(256);

	for (size_t i = 0; i < url.len; i++) {
		switch (state) {
			case ref(State_Scheme):
				if (String_EndsWith(buf, String("://"))) {
					String_Copy(&res.scheme, buf, 0, -3);
					buf.len = 0;

					state = ref(State_Host);
				}

				break;

			case ref(State_Host):
				if (String_EndsWith(buf, String(":"))) {
					state = ref(State_Port);
				} else if (String_EndsWith(buf, String("/"))) {
					state = ref(State_Path);
				}

				if (state != ref(State_Host)) {
					String_Copy(&res.host, buf, 0, -1);
					buf.len = 0;
				}

				break;

			case ref(State_Port):
				if (String_EndsWith(buf, String("/"))) {
					res.port = Int16_Parse(buf);
					buf.len = 0;

					state = ref(State_Path);
				}

				break;

			case ref(State_Path):
				if (String_EndsWith(buf, String("#"))) {
					String_Append(&res.path,
						String_Slice(buf, 0, -1));
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
			String_Append(&res.path, buf);
		} else if (state == ref(State_Fragment)) {
			String_Append(&res.fragment, buf);
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
