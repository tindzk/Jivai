#import <Main.h>
#import <File.h>
#import <Path.h>
#import <HTML/Tree.h>
#import <HTML/Parser.h>
#import <HTML/Quirks.h>

#import "_DocumentTree.h"

#define self Application

def(bool, Run) {
	RdString path =
		(this->args->len == 0)
			? $("HTMLTree.html")
			: this->args->buf[0];

	String s = String_New((size_t) Path_GetSize(path));
	File_GetContents(path, &s);

	HTML_Tree tree = HTML_Tree_New();
	HTML_Tree_Initialize(&tree);

	HTML_Quirks quirks = HTML_Quirks_New(
		HTML_OnToken_For(&tree, HTML_Tree_ProcessToken));
	HTML_Parser html = HTML_Parser_New(
		HTML_OnToken_For(&quirks, HTML_Quirks_ProcessToken));

	HTML_Parser_Process(&html, s.rd);

	DocumentTree_Node *root = HTML_Tree_GetRoot(&tree);
	fwd(i, root->len) {
		call(PrintTree, root->buf[i], 0);
	}

	HTML_Parser_Destroy(&html);
	HTML_Tree_Destroy(&tree);
	String_Destroy(&s);

	return true;
}
