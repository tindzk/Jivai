#import <File.h>
#import <Integer.h>

int main(void) {
	File file;

	try {
		File_Open(&file, $("File.txt"), FileStatus_ReadOnly);
	} catch (File, NotFound) {
		String_Print($("File not found.\n"));
		excReturn ExitStatus_Failure;
	} finally {

	} tryEnd;

	String s = String_New(5);

	do {
		s.len = File_Read(&file, s.buf, String_GetSize(s));
		String_Print(s.rd);
	} while (s.len > 0);

	String_Destroy(&s);

	String strSize = Integer_ToString(File_GetSize(&file));

	String_Print($("size: "));
	String_Print(strSize.rd);
	String_Print($("\n"));

	String_Destroy(&strSize);

	File_Close(&file);

	return ExitStatus_Success;
}
