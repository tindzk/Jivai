#import <Main.h>
#import <File.h>
#import <Path.h>
#import <HTML/Tree.h>
#import <HTML/Builder.h>
#import <StringStream.h>
#import <Ecriture/Tree.h>
#import <Ecriture/Parser.h>

#define self Application

def(bool, Run) {
	if (this->args->len == 0) {
		Logger_Error(&this->logger, $("No path given."));
		return false;
	}

	RdString path = this->args->buf[0];

	String s = String_New((size_t) Path_GetSize(path));
	File_GetContents(path, &s);

	Ecriture_Tree tree = Ecriture_Tree_New();
	Ecriture_Tree_Initialize(&tree);

	Ecriture_Parser ecr = Ecriture_Parser_New(
		Ecriture_OnToken_For(&tree, Ecriture_Tree_ProcessToken));

	String html = String_New(0);

	Ecriture_Parser_Process(&ecr, s.rd);

	StringStream stream = StringStream_New(&html);

	HTML_Builder builder = HTML_Builder_New(StringStream_AsStream(&stream));

	HTML_Tree_BuildTokens(
		HTML_OnToken_For(&builder, HTML_Builder_ProcessToken),
		Ecriture_Tree_GetRoot(&tree));

	String_Print(html.rd);
	String_Destroy(&html);

	Ecriture_Parser_Destroy(&ecr);
	Ecriture_Tree_Destroy(&tree);
	String_Destroy(&s);

	return true;
}
