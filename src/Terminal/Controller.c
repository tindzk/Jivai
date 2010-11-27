#import "Controller.h"

def(void, Init, Terminal *term) {
	this->term = term;
}

static def(void, PrintFmt, String s, VarArg *argptr) {
	for (size_t i = 0; i < s.len; i++) {
		if (i + 1 != s.len && s.buf[i] == '!' && s.buf[i + 1] == '%') {
			Terminal_Print(this->term, '%');
			i++;
		} else if (s.buf[i] == '%') {
			Terminal_Print(this->term, VarArg_Get(*argptr, String));
		} else {
			Terminal_Print(this->term, s.buf[i]);
		}
	}
}

static def(void, Print, Typography_Node *node, VarArg *argptr) {
	for (size_t i = 0; i < node->len; i++) {
		Typography_Node *child = node->buf[i];

		if (child->type == Typography_NodeType_Text) {
			call(PrintFmt, Typography_Text(child)->value, argptr);
		} else if (child->type == Typography_NodeType_Item) {
			String name = Typography_Item(child)->name;

			Terminal_Style style = Terminal_GetStyle(this->term);

			if (String_Equals(name, String("fg"))
			 || String_Equals(name, String("bg"))) {
				String strColor = Typography_Item(child)->options;

				if (String_Equals(strColor, String("%"))) {
					strColor = VarArg_Get(*argptr, String);
				}

				int color = Terminal_ResolveColorName(
					strColor,
					String_Equals(name, String("bg")));

				Terminal_SetVT100Color(this->term, color);
			} else if (String_Equals(name, String("b"))) {
				Terminal_SetVT100Font(this->term, Terminal_Font_Bold);
			} else if (String_Equals(name, String("i"))) {
				Terminal_SetVT100Font(this->term, Terminal_Font_Italics);
			} else if (String_Equals(name, String("u"))) {
				Terminal_SetVT100Font(this->term, Terminal_Font_Underline);
			} else if (String_Equals(name, String("bl"))) {
				Terminal_SetVT100Font(this->term, Terminal_Font_Blink);
			}

			call(Print, child, argptr);

			Terminal_Restore(this->term, style);
		}
	}
}

def(void, Render, String s, ...) {
	StringStream stream;
	StringStream_Init(&stream, &s);

	Typography tyo;
	Typography_Init(&tyo);
	Typography_Parse(&tyo, &StringStreamImpl, &stream);

	VarArg argptr;
	VarArg_Start(argptr, s);
	call(Print, Typography_GetRoot(&tyo), &argptr);
	VarArg_End(argptr);

	Typography_Destroy(&tyo);
}
