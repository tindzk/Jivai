#include <YAML.h>
#include <FileStream.h>
#include <BufferedStream.h>

ExceptionManager exc;

void PrintTree(YAML_Node *node, int depth) {
	if (node->p != NULL) {
		String_Print(String("\n"));

		int i = depth;

		while (i--) {
			String_Print(String("    "));
		}

		if (node->type == YAML_NodeType_Section) {
			YAML_Section *section = node->p;

			String_Print(String("section: "));
			String_Print(section->name);
		} else if (node->type == YAML_NodeType_Item) {
			YAML_Item *item = node->p;

			String_Print(String("key: "));

			if (item->key.len == 0) {
				String_Print(String("(empty)"));
			} else {
				String_Print(item->key);
			}

			String_Print(String(" value: "));
			String_Print(item->value);
		}
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

	FileStream_Open(&file, String("YAML.yml"), O_RDONLY);

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
	PrintTree((YAML_Node *) &yml.tree.root, 0);

	YAML_Destroy(&yml);

	BufferedStream_Close(&stream);
	BufferedStream_Destroy(&stream);

	String_Print(String("\n"));

	return EXIT_SUCCESS;
}
