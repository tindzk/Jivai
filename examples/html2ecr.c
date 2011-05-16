#import <Main.h>
#import <File.h>
#import <Path.h>
#import <HTML/Tree.h>
#import <HTML/Parser.h>
#import <HTML/Quirks.h>
#import <StringStream.h>
#import <Ecriture/Tree.h>
#import <Ecriture/Builder.h>

#define self Application

def(bool, Run) {
	if (this->args->len == 0) {
		Logger_Error(&this->logger, $("No path given."));
		return false;
	}

	RdString path = this->args->buf[0];

	String s = String_New((size_t) Path_GetSize(path));
	File_GetContents(path, &s);

	HTML_Tree tree = HTML_Tree_New();

	HTML_Quirks quirks = HTML_Quirks_New(
		HTML_OnToken_For(&tree, HTML_Tree_ProcessToken));
	HTML_Parser html = HTML_Parser_New(
		HTML_OnToken_For(&quirks, HTML_Quirks_ProcessToken));

	HTML_Parser_Process(&html, s.rd);

	String ecr = String_New(0);
	StringStream stream = StringStream_New(&ecr);

	Ecriture_Builder builder =
		Ecriture_Builder_New(StringStream_AsStream(&stream));
	Ecriture_Tree_BuildTokens(
		Ecriture_OnBuildToken_For(&builder, Ecriture_Builder_ProcessToken),
		HTML_Tree_GetRoot(&tree));

	String_Print(ecr.rd);
	String_Destroy(&ecr);

	HTML_Parser_Destroy(&html);
	HTML_Tree_Destroy(&tree);
	String_Destroy(&s);

	return true;
}
