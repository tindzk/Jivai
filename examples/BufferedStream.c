#import <FileStream.h>
#import <BufferedStream.h>

int main(void) {
	File file;
	File_Open(&file, $("YAML.yml"), FileStatus_ReadOnly);

	BufferedStream stream;
	BufferedStream_Init(&stream, File_AsStream(&file));
	BufferedStream_SetInputBuffer(&stream, 1024, 128);

	String s = StackString(5);

	do {
		s.len = BufferedStreamImpl.read(&stream, s.buf, s.size);
		String_Print(s);
	} while (s.len > 0);

	BufferedStream_Close(&stream);
	BufferedStream_Destroy(&stream);

	return ExitStatus_Success;
}
