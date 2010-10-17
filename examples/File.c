#import <File.h>

ExceptionManager exc;

int main(void) {
	ExceptionManager_Init(&exc);

	File0(&exc);
	String0(&exc);

	File file;

	try (&exc) {
		File_Open(&file, String("File.txt"), FileStatus_ReadOnly);
	} clean catch (Modules_File, excNotFound, e) {
		String_Print(String("File not found.\n"));
		return ExitStatus_Failure;
	} finally {

	} tryEnd;

	String s = StackString(5);

	do {
		s.len = File_Read(&file, s.buf, s.size);
		String_Print(s);
	} while (s.len > 0);

	String_Print(String("size: "));
	String_Print(Integer64_ToString(File_GetSize(&file)));
	String_Print(String("\n"));

	File_Close(&file);

	return ExitStatus_Success;
}
