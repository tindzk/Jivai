#import <YAML.h>
#import <FileStream.h>
#import <BufferedStream.h>

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
		PrintTree(node->buf[i], depth + 1);
	}
}

int main(void) {
	YAML yml;
	File file;
	BufferedStream stream;

	FileStream_Open(&file, String("YAML.yml"), FileStatus_ReadOnly);

	BufferedStream_Init(&stream, &FileStreamImpl, &file);
	BufferedStream_SetInputBuffer(&stream, 1024, 128);

	YAML_Init(&yml, 4, &BufferedStreamImpl, &stream);
	YAML_Parse(&yml);

	/* Print the final tree. */
	PrintTree(YAML_GetRoot(&yml), 0);

	YAML_Destroy(&yml);

	BufferedStream_Close(&stream);
	BufferedStream_Destroy(&stream);

	String_Print(String("\n"));

	return ExitStatus_Success;
}
