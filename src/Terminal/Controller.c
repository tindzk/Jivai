#import "Controller.h"

#define self Terminal_Controller

def(void, Init, Terminal *term) {
	this->term = term;
}

static def(void, Print, Typography_Node *node, VarArg *argptr) {
	forward (i, node->len) {
		Typography_Node *child = node->buf[i];

		if (child->type == Typography_NodeType_Text) {
			Terminal_FmtArgPrint(this->term,
				Typography_Text(child)->value, argptr);
		} else if (child->type == Typography_NodeType_Item) {
			String name = Typography_Item(child)->name;

			Terminal_Style style = Terminal_GetStyle(this->term);

			if (String_Equals(name, $("fg")) ||
				String_Equals(name, $("bg")))
			{
				String strColor = Typography_Item(child)->options;

				if (String_Equals(strColor, $("%"))) {
					strColor = VarArg_Get(*argptr, String);
				}

				int color = Terminal_ResolveColorName(
					strColor,
					String_Equals(name, $("bg")));

				Terminal_SetVT100Color(this->term, color);
			} else if (String_Equals(name, $("b"))) {
				Terminal_SetVT100Font(this->term, Terminal_Font_Bold);
			} else if (String_Equals(name, $("i"))) {
				Terminal_SetVT100Font(this->term, Terminal_Font_Italics);
			} else if (String_Equals(name, $("u"))) {
				Terminal_SetVT100Font(this->term, Terminal_Font_Underline);
			} else if (String_Equals(name, $("bl"))) {
				Terminal_SetVT100Font(this->term, Terminal_Font_Blink);
			}

			call(Print, child, argptr);

			Terminal_Restore(this->term, style);
		}
	}
}

def(void, Render, String s, ...) {
	Typography tyo;
	Typography_Init(&tyo);
	Typography_Parse(&tyo, String_AsStream(&s));

	VarArg argptr;
	VarArg_Start(argptr, s);
	call(Print, Typography_GetRoot(&tyo), &argptr);
	VarArg_End(argptr);

	Typography_Destroy(&tyo);
}
