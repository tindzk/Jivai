#import <Path.h>
#import <Main.h>
#import <HTML/Tree.h>
#import <HTML/Tokenizer.h>
#import <FileStream.h>
#import <BufferedStream.h>

#define self Application

def(void, IndentTree, int level) {
	rpt(level) {
		String_Print($("  "));
	}
}

def(void, PrintTree, HTML_Tree_Node *nodes, int level) {
	each(_node, nodes) {
		HTML_Tree_Node *node = *_node;

		String tmp;

		if (node->type == HTML_Tree_NodeType_Tag) {
			call(IndentTree, level);

			tmp = String_Format($("tag=%\n"), node->value.rd);
			String_Print(tmp.rd);
			String_Destroy(&tmp);

			for (size_t i = 0; i < node->attrs->len; i++) {
				call(IndentTree, level + 1);

				tmp = String_Format(
					$("name=\"%\" value=\"%\"\n"),
					node->attrs->buf[i].name.rd,
					node->attrs->buf[i].value.rd);
				String_Print(tmp.rd);
				String_Destroy(&tmp);
			}

			call(PrintTree, node, level + 1);
		} else if (node->type == HTML_Tree_NodeType_Value) {
			call(IndentTree, level);

			/* HTML_Tree also creates nodes for values only consisting
			 * of whitespaces. In some cases this is useful, but not for
			 * printing trees.
			 */
			RdString value = String_Trim(node->value.rd);

			if (value.len > 0) {
				tmp = String_Format($("value=\"%\"\n"), value);
				String_Print(tmp.rd);
				String_Destroy(&tmp);
			} else {
				String_Print($("value={empty}\n"));
			}
		}
	}
}

def(bool, Run) {
	RdString path =
		(this->args->len == 0)
			? $("HTMLTree.html")
			: this->args->buf[0];

	String s = String_New((size_t) Path_GetSize(path));
	File_GetContents(path, &s);

	HTML_Tree tree = HTML_Tree_New();
	HTML_Tokenizer html = HTML_Tokenizer_New(
		HTML_OnToken_For(&tree, HTML_Tree_ProcessToken));

	HTML_Tokenizer_Process(&html, s.rd);
	call(PrintTree, HTML_Tree_GetRoot(&tree), 0);

	HTML_Tokenizer_Destroy(&html);
	HTML_Tree_Destroy(&tree);
	String_Destroy(&s);

	return true;
}
