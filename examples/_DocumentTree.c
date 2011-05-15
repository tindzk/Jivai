#import "_DocumentTree.h"

#define self Application

static def(void, IndentTree, int level) {
	rpt(level) {
		String_Print($("  "));
	}
}

def(void, PrintTree, DocumentTree_Node *nodes, int level) {
	each(_node, nodes) {
		DocumentTree_Node *node = *_node;

		String tmp;

		if (node->type == DocumentTree_NodeType_Tag) {
			call(IndentTree, level);

			tmp = String_Format($("tag=%\n"), node->value.rd);
			String_Print(tmp.rd);
			String_Destroy(&tmp);

			fwd(i, node->attrs->len) {
				call(IndentTree, level + 1);

				tmp = String_Format(
					$("name=\"%\" value=\"%\"\n"),
					node->attrs->buf[i].name.rd,
					node->attrs->buf[i].value.rd);
				String_Print(tmp.rd);
				String_Destroy(&tmp);
			}

			call(PrintTree, node, level + 1);
		} else if (node->type == DocumentTree_NodeType_Value) {
			call(IndentTree, level);

			/* HTML_Tree/Ecriture_Tree create nodes for values only consisting
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

#undef self
