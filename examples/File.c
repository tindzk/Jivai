#import <File.h>
#import <Integer.h>

int main(void) {
	File file;

	try {
		File_Open(&file, $("File.txt"), FileStatus_ReadOnly);
	} clean catch (File, NotFound) {
		String_Print($("File not found.\n"));
		excReturn ExitStatus_Failure;
	} finally {

	} tryEnd;

	String s = StackString(5);

	do {
		s.len = File_Read(&file, s.buf, s.size);
		String_Print(s);
	} while (s.len > 0);

	String_Print($("size: "));
	String_Print(Integer_ToString(File_GetSize(&file)));
	String_Print($("\n"));

	File_Close(&file);

	return ExitStatus_Success;
}
