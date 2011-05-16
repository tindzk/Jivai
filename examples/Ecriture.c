#import <Main.h>
#import <File.h>
#import <Path.h>
#import <Ecriture/Tree.h>
#import <Ecriture/Parser.h>

#import "_DocumentTree.h"

#define self Application

def(bool, Run) {
	RdString path =
		(this->args->len == 0)
			? $("Ecriture.ecr")
			: this->args->buf[0];

	String s = String_New((size_t) Path_GetSize(path));
	File_GetContents(path, &s);

	Ecriture_Tree tree = Ecriture_Tree_New();

	Ecriture_Parser ecr = Ecriture_Parser_New(
		Ecriture_OnToken_For(&tree, Ecriture_Tree_ProcessToken));

	Ecriture_Parser_Process(&ecr, s.rd);

	DocumentTree_Node *root = Ecriture_Tree_GetRoot(&tree);
	fwd(i, root->len) {
		call(PrintTree, root->buf[i], 0);
	}

	Ecriture_Parser_Destroy(&ecr);
	Ecriture_Tree_Destroy(&tree);
	String_Destroy(&s);

	return true;
}
