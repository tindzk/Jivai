#include <YAML.h>
#include <FileStream.h>
#include <BufferedStream.h>

ExceptionManager exc;

void PrintTree(YAML_Node *node, int depth) {
	if (node->type == YAML_NodeType_Section
	 || node->type == YAML_NodeType_Item) {
		String_Print(String("\n"));

		for (ssize_t i = depth - 1; i > 0; i--) {
			String_Print(String("  "));
		}
	}

	if (node->type == YAML_NodeType_Section) {
		String_Print(String("section: "));
		String_Print(YAML_Section(node)->name);
	} else if (node->type == YAML_NodeType_Item) {
		String_Print(String("key: "));

		if (YAML_Item(node)->key.len == 0) {
			String_Print(String("(empty)"));
		} else {
			String_Print(YAML_Item(node)->key);
		}

		String_Print(String(" value: "));
		String_Print(YAML_Item(node)->value);
	}

	for (size_t i = 0; i < node->len; i++) {
		PrintTree(node->nodes[i], depth + 1);
	}
}

int main(void) {
	ExceptionManager_Init(&exc);

	File0(&exc);
	YAML0(&exc);
	String0(&exc);
	Memory0(&exc);

	YAML yml;
	File file;
	BufferedStream stream;

	FileStream_Open(&file, String("YAML.yml"), FileStatus_ReadOnly);

	BufferedStream_Init(&stream, &FileStream_Methods, &file);
	BufferedStream_SetInputBuffer(&stream, 1024, 128);

	YAML_Init(&yml, 4, &BufferedStream_Methods, &stream);
	YAML_Parse(&yml);

	/* There shouldn't be any characters left in the buffer. */
	if (BufferedStream_Flush(&stream).len != 0) {
		String_Print(String("The buffer is not empty.\n"));
	}

	/* Seek to the beginning and reset the stream. */
	File_Seek(&file, 0, SEEK_SET);
	BufferedStream_Reset(&stream);

	/* Parse the file again. No memory should get leaked. (Verify
	 * with Valgrind.) */
	YAML_Parse(&yml);

	/* Print the final tree. */
	PrintTree(YAML_GetRoot(&yml), 0);

	YAML_Destroy(&yml);

	BufferedStream_Close(&stream);
	BufferedStream_Destroy(&stream);

	String_Print(String("\n"));

	return EXIT_SUCCESS;
}
