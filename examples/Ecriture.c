/* Ecriture is a lightweight markup language. */

#import <Main.h>
#import <File.h>
#import <Integer.h>
#import <Ecriture.h>
#import <FileStream.h>
#import <BufferedStream.h>

#define self Application

def(void, PrintTree, Ecriture_Node *node, size_t depth) {
	String strDepth = Integer_ToString(depth);

	String_Print($("depth="));
	String_Print(strDepth.rd);

	String_Destroy(&strDepth);

	rpt(depth) {
		String_Print($("    "));
	}

	if (node->type == Ecriture_NodeType_Text) {
		String_Print($("value: "));

		String_Print(Ecriture_Text_GetValue(node));
	} else if (node->type == Ecriture_NodeType_Item) {
		String_Print($("name: "));

		String_Print(Ecriture_Item_GetName(node));

		String_Print($(" options: "));

		if (Ecriture_Item_GetOptions(node).len > 0) {
			String_Print(Ecriture_Item_GetOptions(node));
		} else {
			String_Print($("(empty)"));
		}
	}

	String_Print($("\n"));

	fwd(i, node->len) {
		call(PrintTree, node->buf[i], depth + 1);
	}
}

def(bool, Run) {
	File file = File_New($("Ecriture.ecr"), FileStatus_ReadOnly);

	BufferedStream stream = BufferedStream_New(File_AsStream(&file));
	BufferedStream_SetInputBuffer(&stream, 1024, 128);

	Ecriture ecr = Ecriture_New();

	try {
		Ecriture_Parse(&ecr, BufferedStream_AsStream(&stream));
		call(PrintTree, Ecriture_GetRoot(&ecr), 0);
	} finally {
		BufferedStream_Destroy(&stream);
		Ecriture_Destroy(&ecr);
	} tryEnd;

	return true;
}
