#import <Terminal.h>
#import <FileStream.h>
#import <BufferedStream.h>

int main(void) {
	File file;
	File_Open(&file, $("YAML.yml"), FileStatus_ReadOnly);

	BufferedStream stream = BufferedStream_New(File_AsStream(&file));
	BufferedStream_SetInputBuffer(&stream, 1024, 128);

	String s = String_New(5);

	do {
		s.len = BufferedStreamImpl.read(&stream, s.buf, String_GetSize(s));
		String_Print(s.prot);
	} while (s.len > 0);

	String_Destroy(&s);

	BufferedStream_Close(&stream);
	BufferedStream_Destroy(&stream);

	return ExitStatus_Success;
}
