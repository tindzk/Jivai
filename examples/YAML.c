#import <YAML.h>
#import <FileStream.h>
#import <BufferedStream.h>

void PrintTree(YAML_Node *node, int depth) {
	if (node->type == YAML_NodeType_Section ||
		node->type == YAML_NodeType_Item)
	{
		String_Print($("\n"));

		rpt(depth) {
			String_Print($("  "));
		}
	}

	if (node->type == YAML_NodeType_Section) {
		String_Print($("section: "));
		String_Print(YAML_Section_GetName(node));
	} else if (node->type == YAML_NodeType_Item) {
		String_Print($("key: "));

		if (YAML_Item_GetKey(node).len == 0) {
			String_Print($("(empty)"));
		} else {
			String_Print(YAML_Item_GetKey(node));
		}

		String_Print($(" value: "));
		String_Print(YAML_Item_GetValue(node));
	}

	fwd(i, node->len) {
		PrintTree(node->buf[i], depth + 1);
	}
}

int main(void) {
	File file;
	FileStream_Open(&file, $("YAML.yml"), FileStatus_ReadOnly);

	BufferedStream stream = BufferedStream_New(File_AsStream(&file));
	BufferedStream_SetInputBuffer(&stream, 1024, 128);

	YAML yml = YAML_New(4, &BufferedStreamImpl, &stream);
	YAML_Parse(&yml);

	/* Print the final tree. */
	PrintTree(YAML_GetRoot(&yml), 0);

	YAML_Destroy(&yml);

	BufferedStream_Close(&stream);
	BufferedStream_Destroy(&stream);

	String_Print($("\n"));

	return ExitStatus_Success;
}
