#import "Controller.h"

#define self Terminal_Controller

rsdef(self, New, Terminal *term) {
	return (self) {
		.term = term
	};
}

static def(void, Print, Typography_Node *node, VarArg *argptr) {
	forward (i, node->len) {
		Typography_Node *child = node->buf[i];

		if (child->type == Typography_NodeType_Text) {
			ProtString value = Typography_Text(child)->value.prot;
			Terminal_FmtArgPrint(this->term, value, argptr);
		} else if (child->type == Typography_NodeType_Item) {
			ProtString name = Typography_Item(child)->name.prot;

			Terminal_Style style = Terminal_GetStyle(this->term);

			if (String_Equals(name, $("fg")) ||
				String_Equals(name, $("bg")))
			{
				ProtString strColor = Typography_Item(child)->options.prot;

				if (String_Equals(strColor, $("%"))) {
					strColor = VarArg_Get(*argptr, ProtString);
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

def(void, Render, ProtString s, ...) {
	Typography tyo = Typography_New(&tyo);
	Typography_Parse(&tyo, String_AsStream(&s));

	VarArg argptr;
	VarArg_Start(argptr, s);
	call(Print, Typography_GetRoot(&tyo), &argptr);
	VarArg_End(argptr);

	Typography_Destroy(&tyo);
}
