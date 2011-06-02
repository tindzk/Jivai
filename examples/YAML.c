#import <Main.h>
#import <File.h>
#import <YAML/Tree.h>
#import <YAML/Parser.h>

#define self Application

def(void, PrintTree, YAML_Tree_Node *node, int depth) {
	if (node->type == YAML_Tree_NodeType_Section ||
		node->type == YAML_Tree_NodeType_Item)
	{
		String_Print($("\n"));

		rpt(depth) {
			String_Print($("  "));
		}
	}

	if (node->type == YAML_Tree_NodeType_Section) {
		String_Print($("section: "));
		String_Print(node->name);
	} else if (node->type == YAML_Tree_NodeType_Item) {
		String_Print($("key: "));

		if (node->name.len == 0) {
			String_Print($("(empty)"));
		} else {
			String_Print(node->name);
		}

		String_Print($(" value: "));
		String_Print(node->value);
	}

	fwd(i, node->len) {
		call(PrintTree, node->buf[i], depth + 1);
	}
}

def(bool, Run) {
	String s = String_New(1024);
	File_GetContents($("YAML.yml"), &s);

	YAML_Tree tree = YAML_Tree_New();
	YAML_Tree_Initialize(&tree);

	YAML_Parser yml = YAML_Parser_New(
		YAML_OnToken_For(&tree, YAML_Tree_ProcessToken));
	YAML_Parser_Process(&yml, s.rd);

	call(PrintTree, YAML_Tree_GetRoot(&tree), 0);

	YAML_Parser_Destroy(&yml);
	YAML_Tree_Destroy(&tree);

	String_Destroy(&s);

	return true;
}
