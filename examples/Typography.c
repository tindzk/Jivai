/* Typography is a lightweight markup language. */

#import <File.h>
#import <Integer.h>
#import <Typography.h>
#import <FileStream.h>
#import <BufferedStream.h>

void PrintTree(Typography_Node *node, size_t depth) {
	String strDepth = Integer_ToString(depth);

	String_Print($("depth="));
	String_Print(strDepth.rd);

	String_Destroy(&strDepth);

	rpt(depth) {
		String_Print($("    "));
	}

	if (node->type == Typography_NodeType_Text) {
		String_Print($("value: "));

		String_Print(Typography_Text(node)->value.rd);
	} else if (node->type == Typography_NodeType_Item) {
		String_Print($("name: "));

		String_Print(Typography_Item(node)->name.rd);

		String_Print($(" options: "));

		if (Typography_Item(node)->options.len > 0) {
			String_Print(Typography_Item(node)->options.rd);
		} else {
			String_Print($("(empty)"));
		}
	}

	String_Print($("\n"));

	fwd(i, node->len) {
		PrintTree(node->buf[i], depth + 1);
	}
}

int main(void) {
	File file;
	File_Open(&file, $("Typography.tyo"), FileStatus_ReadOnly);

	BufferedStream stream = BufferedStream_New(File_AsStream(&file));
	BufferedStream_SetInputBuffer(&stream, 1024, 128);

	Typography tyo = Typography_New();

	try {
		Typography_Parse(&tyo, BufferedStream_AsStream(&stream));

		PrintTree(Typography_GetRoot(&tyo), 0);

		Typography_Destroy(&tyo);
	} catchAny {
		Exception_Print(e);
		excReturn ExitStatus_Failure;
	} finally {
		BufferedStream_Destroy(&stream);
	} tryEnd;

	return ExitStatus_Success;
}
