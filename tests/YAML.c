#import <YAML.h>
#import <FileStream.h>
#import <BufferedStream.h>

#import "TestSuite.h"
#import "App.h"

extern ExceptionManager exc;

#undef self
#define self tsYAML

class(self) {

};

tsRegister("YAML") {
	File0(&exc);
	YAML0(&exc);
	String0(&exc);
	Memory0(&exc);

	return true;
}

tsCase(Acute, "Memory") {
	File file;
	FileStream_Open(&file, String("../examples/YAML.yml"), FileStatus_ReadOnly);

	BufferedStream stream;
	BufferedStream_Init(&stream, &FileStreamImpl, &file);
	BufferedStream_SetInputBuffer(&stream, 1024, 128);

	YAML yml;
	YAML_Init(&yml, 4, &BufferedStreamImpl, &stream);

	YAML_Parse(&yml);

	Assert($("Empty buffer"), BufferedStream_Flush(&stream).len == 0);

	size_t oldNodes = YAML_GetRoot(&yml)->len;

	/* Seek to the beginning and reset the stream. */
	File_Seek(&file, 0, File_SeekType_Set);
	BufferedStream_Reset(&stream);

	/* Parse the file again. No memory should get leaked. Verify
	 * with Valgrind.
	 */
	YAML_Parse(&yml);

	Assert($("Same number of top-level nodes"), oldNodes == YAML_GetRoot(&yml)->len);

	YAML_Destroy(&yml);

	BufferedStream_Close(&stream);
	BufferedStream_Destroy(&stream);
}

tsFinalize;