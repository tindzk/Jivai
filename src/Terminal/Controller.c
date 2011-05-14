#import "Controller.h"

#define self Terminal_Controller

rsdef(self, New, Terminal *term) {
	return (self) {
		.term = term
	};
}

static def(void, OnToken, Ecriture_TokenType type, String value, __unused size_t line) {
	if (type == Ecriture_TokenType_TagStart) {
		assert(this->depth + 1 <= Terminal_Controller_Depth);

		this->items[this->depth].name  = value;
		this->items[this->depth].style = Terminal_GetStyle(this->term);

		this->depth++;

		if (String_Equals(value.rd, $("b"))) {
			Terminal_SetVT100Font(this->term, Terminal_Font_Bold);
		} else if (String_Equals(value.rd, $("i"))) {
			Terminal_SetVT100Font(this->term, Terminal_Font_Italics);
		} else if (String_Equals(value.rd, $("u"))) {
			Terminal_SetVT100Font(this->term, Terminal_Font_Underline);
		} else if (String_Equals(value.rd, $("bl"))) {
			Terminal_SetVT100Font(this->term, Terminal_Font_Blink);
		}
	} else if (type == Ecriture_TokenType_Option) {
		assert(this->depth > 0);

		RdString name = this->items[this->depth - 1].name.rd;

		if (String_Equals(name, $("fg")) ||
			String_Equals(name, $("bg")))
		{
			RdString strColor = value.rd;

			if (String_Equals(strColor, $("%"))) {
				strColor = VarArg_Get(this->argptr, RdString);
			}

			int color = Terminal_ResolveColorName(
				strColor,
				String_Equals(name, $("bg")));

			Terminal_SetVT100Color(this->term, color);
		}

		String_Destroy(&value);
	} else if (type == Ecriture_TokenType_Value) {
		Terminal_FmtArgPrint(this->term, value.rd, &this->argptr);
		String_Destroy(&value);
	} else if (type == Ecriture_TokenType_TagEnd) {
		assert(this->depth > 0);

		Terminal_Restore(this->term, this->items[this->depth - 1].style);
		String_Destroy(&this->items[this->depth - 1].name);
		this->depth--;
	}
}

def(void, Render, RdString s, ...) {
	Ecriture_Parser ecr =
		Ecriture_Parser_New(Ecriture_OnToken_For(this, ref(OnToken)));

	VarArg_Start(this->argptr, s);

	Ecriture_Parser_Process(&ecr, String_AsStream(RdString_Exalt(s)));

	VarArg_End(this->argptr);

	Ecriture_Parser_Destroy(&ecr);
}
