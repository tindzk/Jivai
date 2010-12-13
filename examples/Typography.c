/* Typography is a lightweight markup language. */

#import <Path.h>
#import <File.h>
#import <Integer.h>
#import <Typography.h>
#import <FileStream.h>
#import <BufferedStream.h>

void PrintTree(Typography_Node *node, size_t depth) {
	String_FmtPrint($("depth=%"),
		Int16_ToString(depth));

	for (size_t i = 0; i < depth; i++) {
		String_Print($("    "));
	}

	if (node->type == Typography_NodeType_Text) {
		String_Print($("value: "));

		String_Print(Typography_Text(node)->value);
	} else if (node->type == Typography_NodeType_Item) {
		String_Print($("name: "));

		String_Print(Typography_Item(node)->name);

		String_Print($(" options: "));

		if (Typography_Item(node)->options.len > 0) {
			String_Print(Typography_Item(node)->options);
		} else {
			String_Print($("(empty)"));
		}
	}

	String_Print($("\n"));

	for (size_t i = 0; i < node->len; i++) {
		PrintTree(node->buf[i], depth + 1);
	}
}

int main(__unused int argc, __unused char *argv[]) {
	File file;
	FileStream_Open(&file,
		$("Typography.tyo"),
		FileStatus_ReadOnly);

	BufferedStream stream;
	BufferedStream_Init(&stream, File_AsStream(&file));
	BufferedStream_SetInputBuffer(&stream, 1024, 128);

	Typography tyo;

	try {
		Typography_Init(&tyo);
		Typography_Parse(&tyo, BufferedStream_AsStream(&stream));

		PrintTree(Typography_GetRoot(&tyo), 0);

		Typography_Destroy(&tyo);
	} clean catchAny {
		Exception_Print(e);

#if Exception_SaveTrace
		Backtrace_PrintTrace(__exc_mgr.e.trace, __exc_mgr.e.traceItems);
#endif

		excReturn ExitStatus_Failure;
	} finally {

	} tryEnd;

	return ExitStatus_Success;
}
