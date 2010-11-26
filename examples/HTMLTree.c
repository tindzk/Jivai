#import <HTML/Tree.h>
#import <FileStream.h>
#import <BufferedStream.h>

void IndentTree(int level) {
	while (level--) {
		String_Print(String("  "));
	}
}

void PrintTree(HTML_Tree_Node *node, int level) {
	HTML_Tree_Foreach(node, ^(HTML_Tree_Node *node) {
		String tmp;

		if (node->type == HTML_Tree_NodeType_Tag) {
			IndentTree(level);

			String_Print(tmp = String_Format(String("tag=%\n"), node->value));
			String_Destroy(&tmp);

			for (size_t i = 0; i < node->attrs->len; i++) {
				IndentTree(level + 1);

				String_Print(tmp = String_Format(
					String("name=\"%\" value=\"%\"\n"),
					node->attrs->buf[i].name, node->attrs->buf[i].value));

				String_Destroy(&tmp);
			}

			PrintTree(node, level + 1);
		} else if (node->type == HTML_Tree_NodeType_Value) {
			IndentTree(level);

			/* HTML_Tree also creates nodes for values only consisting
			 * of whitespaces. In some cases this is useful, but not for
			 * printing trees.
			 */
			String_Trim(&node->value);

			if (node->value.len > 0) {
				String_Print(tmp = String_Format(
					String("value=\"%\"\n"),
					node->value));

				String_Destroy(&tmp);
			} else {
				String_Print(String("value={empty}\n"));
			}
		}
	});
}

int main(void) {
	File file;
	BufferedStream stream;

	try {
		FileStream_Open(&file, String("HTMLTree.html"), FileStatus_ReadOnly);
	} clean catch (File, excNotFound) {
		String_Print(String("File not found.\n"));
		return ExitStatus_Failure;
	} finally {

	} tryEnd;

	BufferedStream_Init(&stream, &FileStreamImpl, &file);
	BufferedStream_SetInputBuffer(&stream, 1024, 128);

	HTML_Tree tree;
	HTML_Tree_Init(&tree);

	HTML_Tokenizer html;
	HTML_Tokenizer_Init(&html, Callback(&tree, &HTML_Tree_ProcessToken));
	HTML_Tokenizer_Process(HTML_Tokenizer_FromObject(&html),
		&BufferedStreamImpl, &stream);
	HTML_Tokenizer_Destroy(&html);

	HTML_Tree_Node *node = HTML_Tree_GetRoot(&tree);
	PrintTree(node, 0);

	HTML_Tree_Destroy(&tree);

	BufferedStream_Close(&stream);
	BufferedStream_Destroy(&stream);

	String_Print(String("\n"));

	return ExitStatus_Success;
}
