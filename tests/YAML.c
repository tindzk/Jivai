#import <YAML.h>
#import <FileStream.h>
#import <BufferedStream.h>

#import "TestSuite.h"

#define self tsYAML

class {

};

tsRegister("YAML") {
	return true;
}

tsCase(Acute, "Memory") {
	File file = File_New($("../examples/YAML.yml"), FileStatus_ReadOnly);

	BufferedStream stream = BufferedStream_New(File_AsStream(&file));
	BufferedStream_SetInputBuffer(&stream, 1024, 128);

	YAML yml = YAML_New(4);
	YAML_Parse(&yml, BufferedStream_AsStream(&stream));

	Assert($("Empty buffer"), BufferedStream_Flush(&stream).len == 0);

	size_t oldNodes = YAML_GetRoot(&yml)->len;

	/* Seek to the beginning and reset the stream. */
	File_Seek(&file, 0, File_SeekType_Set);
	BufferedStream_Reset(&stream);

	/* Parse the file again. No memory should get leaked. Verify
	 * with Valgrind.
	 */
	YAML_Parse(&yml, BufferedStream_AsStream(&stream));

	Assert($("Same number of top-level nodes"), oldNodes == YAML_GetRoot(&yml)->len);

	YAML_Destroy(&yml);

	BufferedStream_Close(&stream);
	BufferedStream_Destroy(&stream);
}

tsFinalize;
