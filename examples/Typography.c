/* Typography is a lightweight markup language. */

#import <Path.h>
#import <File.h>
#import <Signal.h>
#import <Integer.h>
#import <Typography.h>
#import <FileStream.h>
#import <BufferedStream.h>

ExceptionManager exc;

void PrintTree(Typography_Node *node, size_t depth) {
	String_FmtPrint(String("depth=%"),
		Integer_ToString(depth));

	for (size_t i = 0; i < depth; i++) {
		String_Print(String("    "));
	}

	if (node->type == Typography_NodeType_Text) {
		String_Print(String("value: "));

		String_Print(Typography_Text(node)->value);
	} else if (node->type == Typography_NodeType_Item) {
		String_Print(String("name: "));

		String_Print(Typography_Item(node)->name);

		String_Print(String(" options: "));

		if (Typography_Item(node)->options.len > 0) {
			String_Print(Typography_Item(node)->options);
		} else {
			String_Print(String("(empty)"));
		}
	}

	String_Print(String("\n"));

	for (size_t i = 0; i < node->len; i++) {
		PrintTree(node->nodes[i], depth + 1);
	}
}

int main(__unused int argc, __unused char *argv[]) {
	ExceptionManager_Init(&exc);

	Path0(&exc);
	File0(&exc);
	Memory0(&exc);
	String0(&exc);
	Signal0(&exc);
	Integer0(&exc);
	Typography0(&exc);

	Typography tyo;

	File file;
	BufferedStream stream;

	FileStream_Open(&file,
		String("Typography.tyo"),
		FileStatus_ReadOnly);

	BufferedStream_Init(&stream, &FileStream_Methods, &file);
	BufferedStream_SetInputBuffer(&stream, 1024, 128);

	try (&exc) {
		Typography_Init(&tyo, &BufferedStream_Methods, &stream);
		Typography_Parse(&tyo);

		PrintTree(Typography_GetRoot(&tyo), 0);

		Typography_Destroy(&tyo);
	} clean catchAny {
		ExceptionManager_Print(&exc, e);

#if Exception_SaveTrace
		Backtrace_PrintTrace(e->trace, e->traceItems);
#endif

		excReturn ExitStatus_Failure;
	} finally {

	} tryEnd;

	return ExitStatus_Success;
}
