#import <FileStream.h>
#import <BufferedStream.h>

ExceptionManager exc;

int main(void) {
	ExceptionManager_Init(&exc);

	File0(&exc);
	String0(&exc);

	File file;
	BufferedStream stream;

	FileStream_Open(&file, String("YAML.yml"), FileStatus_ReadOnly);

	BufferedStream_Init(&stream, &FileStreamImpl, &file);
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
