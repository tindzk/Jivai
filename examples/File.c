#import <File.h>
#import <Integer.h>

int main(void) {
	File file;

	try {
		File_Open(&file, String("File.txt"), FileStatus_ReadOnly);
	} clean catch (File, excNotFound) {
		String_Print(String("File not found.\n"));
		excReturn ExitStatus_Failure;
	} finally {

	} tryEnd;

	String s = StackString(5);

	do {
		s.len = File_Read(File_FromObject(&file), s.buf, s.size);
		String_Print(s);
	} while (s.len > 0);

	String_Print(String("size: "));
	String_Print(Integer64_ToString(File_GetSize(&file)));
	String_Print(String("\n"));

	File_Close(&file);

	return ExitStatus_Success;
}
