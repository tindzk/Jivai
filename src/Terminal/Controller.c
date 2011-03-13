#import "Controller.h"

#define self Terminal_Controller

rsdef(self, New, Terminal *term) {
	return (self) {
		.term = term
	};
}

static def(void, Print, Typography_Node *node, VarArg *argptr) {
	fwd(i, node->len) {
		Typography_Node *child = node->buf[i];

		if (child->type == Typography_NodeType_Text) {
			RdString value = Typography_Text(child)->value.rd;
			Terminal_FmtArgPrint(this->term, value, argptr);
		} else if (child->type == Typography_NodeType_Item) {
			RdString name = Typography_Item(child)->name.rd;

			Terminal_Style style = Terminal_GetStyle(this->term);

			if (String_Equals(name, $("fg")) ||
				String_Equals(name, $("bg")))
			{
				RdString strColor = Typography_Item(child)->options.rd;

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
	Typography tyo = Typography_New(&tyo);
	Typography_Parse(&tyo, String_AsStream(&s));

	VarArg argptr;
	VarArg_Start(argptr, s);
	call(Print, Typography_GetRoot(&tyo), &argptr);
	VarArg_End(argptr);

	Typography_Destroy(&tyo);
}
