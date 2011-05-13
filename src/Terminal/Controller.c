#import "Controller.h"

#define self Terminal_Controller

rsdef(self, New, Terminal *term) {
	return (self) {
		.term = term
	};
}

static def(void, Print, Ecriture_Node *node, VarArg *argptr) {
	fwd(i, node->len) {
		Ecriture_Node *child = node->buf[i];

		if (child->type == Ecriture_NodeType_Text) {
			RdString value = Ecriture_Text_GetValue(child);
			Terminal_FmtArgPrint(this->term, value, argptr);
		} else if (child->type == Ecriture_NodeType_Item) {
			RdString name = Ecriture_Item_GetName(child);

			Terminal_Style style = Terminal_GetStyle(this->term);

			if (String_Equals(name, $("fg")) ||
				String_Equals(name, $("bg")))
			{
				RdString strColor = Ecriture_Item_GetOptions(child);

				if (String_Equals(strColor, $("%"))) {
					strColor = VarArg_Get(*argptr, RdString);
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

def(void, Render, RdString s, ...) {
	Ecriture ecr = Ecriture_New(&ecr);
	Ecriture_Parse(&ecr, String_AsStream(RdString_Exalt(s)));

	VarArg argptr;
	VarArg_Start(argptr, s);
	call(Print, Ecriture_GetRoot(&ecr), &argptr);
	VarArg_End(argptr);

	Ecriture_Destroy(&ecr);
}
