#import <Main.h>
#import <File.h>
#import <String.h>

#define self Application

def(bool, Run) {
	if (this->args->len == 0) {
		Logger_Error(&this->logger, $("No path given."));
		return false;
	}

	String s = File_GetContents(this->args->buf[0]);

	RdString iter = $("");
	while (String_Split(s.rd, '\n', &iter)) {
		if (iter.len < 5) {
			continue;
		}

		if (iter.buf[0] == '\t' || iter.buf[0] == ' ') {
			continue;
		}

		RdString line = String_Trim(iter);

		if (String_BeginsWith(line, $("/*")) ||
			String_BeginsWith(line, $("//")) ||
			String_BeginsWith(line, $("#")))
		{
			continue;
		}

		if (String_EndsWith(line, $("}"))) {
			line = String_Slice(line, 0, -1);
			line = String_Trim(line);
		}

		if (String_EndsWith(line, $("{"))) {
			line = String_Slice(line, 0, -1);
			line = String_Trim(line);

			String_Print(line);
			String_Print($(";\n"));
		}
	}

	String_Destroy(&s);

	return true;
}
