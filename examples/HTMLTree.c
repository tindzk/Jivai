#import <Main.h>
#import <HTML/Tree.h>
#import <FileStream.h>
#import <BufferedStream.h>

void IndentTree(int level) {
	while (level--) {
		String_Print($("  "));
	}
}

void PrintTree(HTML_Tree_Node *nodes, int level) {
	each(_node, nodes) {
		HTML_Tree_Node *node = *_node;

		String tmp;

		if (node->type == HTML_Tree_NodeType_Tag) {
			IndentTree(level);

			tmp = String_Format($("tag=%\n"), node->value.rd);
			String_Print(tmp.rd);
			String_Destroy(&tmp);

			for (size_t i = 0; i < node->attrs->len; i++) {
				IndentTree(level + 1);

				tmp = String_Format(
					$("name=\"%\" value=\"%\"\n"),
					node->attrs->buf[i].name.rd,
					node->attrs->buf[i].value.rd);
				String_Print(tmp.rd);
				String_Destroy(&tmp);
			}

			PrintTree(node, level + 1);
		} else if (node->type == HTML_Tree_NodeType_Value) {
			IndentTree(level);

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

#define self Application

def(bool, Run) {
	RdString name =
		(this->args->len == 0)
			? $("HTMLTree.html")
			: this->args->buf[0];

	File file;

	try {
		File_Open(&file, name, FileStatus_ReadOnly);
	} catch (File, NotFound) {
		String_Print($("File not found.\n"));
		return false;
	} finally {

	} tryEnd;

	BufferedStream stream = BufferedStream_New(File_AsStream(&file));
	BufferedStream_SetInputBuffer(&stream, 1024, 128);

	HTML_Tree tree = HTML_Tree_New();

	HTML_Tokenizer html;
	HTML_Tokenizer_Init(&html, Callback(&tree, &HTML_Tree_ProcessToken));
	HTML_Tokenizer_Process(&html, &BufferedStreamImpl, &stream);
	HTML_Tokenizer_Destroy(&html);

	HTML_Tree_Node *node = HTML_Tree_GetRoot(&tree);
	PrintTree(node, 0);

	HTML_Tree_Destroy(&tree);

	BufferedStream_Close(&stream);
	BufferedStream_Destroy(&stream);

	String_Print($("\n"));

	return true;
}
