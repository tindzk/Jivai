#import "Terminal.h"

size_t Modules_Terminal;

static ExceptionManager *exc;

void Terminal0(ExceptionManager *e) {
	Modules_Terminal = Module_Register(String("Terminal"));

	exc = e;
}

void Terminal_Init(Terminal *this, File *in, File *out, bool assumeVT100) {
	this->in  = in;
	this->out = out;

	this->isVT100 = assumeVT100 && isatty(out->fd);

	tcgetattr(FileNo_StdIn, &this->oldTermios);

	this->curTermios = this->oldTermios;

	this->style = (Terminal_Style) {0, 0};
}

void Terminal_Configure(Terminal *this, bool echo, bool signal) {
	if (echo) {
		BitMask_Set(this->curTermios.c_lflag, ECHO);
	} else {
		BitMask_Clear(this->curTermios.c_lflag, ECHO);
	}

	if (signal) {
		BitMask_Set(this->curTermios.c_lflag, ISIG);
	} else {
		BitMask_Clear(this->curTermios.c_lflag, ISIG);
	}

	this->curTermios.c_iflag &= ~IXON;
	this->curTermios.c_lflag &= ~ICANON;

	tcsetattr(FileNo_StdIn, TCSAFLUSH, &this->curTermios);
}

void Terminal_Destroy(Terminal *this) {
	tcsetattr(FileNo_StdIn, 0, &this->oldTermios);
}

void Terminal_ResetVT100(Terminal *this) {
	File_Write(this->out, Terminal_VT100_Normal);

	this->style = (Terminal_Style) {0, 0};
}

/* Write VT100 escape sequences to the stream for the given color. */
void Terminal_SetVT100Color(Terminal *this, int color) {
	switch (color & Terminal_Color_ForegroundMask) {
		case Terminal_Color_ForegroundBlack:
			File_Write(this->out, Terminal_VT100_Foreground_Black);
			break;

		case Terminal_Color_ForegroundRed:
			File_Write(this->out, Terminal_VT100_Foreground_Red);
			break;

		case Terminal_Color_ForegroundGreen:
			File_Write(this->out, Terminal_VT100_Foreground_Green);
			break;

		case Terminal_Color_ForegroundYellow:
			File_Write(this->out, Terminal_VT100_Foreground_Yellow);
			break;

		case Terminal_Color_ForegroundBlue:
			File_Write(this->out, Terminal_VT100_Foreground_Blue);
			break;

		case Terminal_Color_ForegroundMagenta:
			File_Write(this->out, Terminal_VT100_Foreground_Magenta);
			break;

		case Terminal_Color_ForegroundCyan:
			File_Write(this->out, Terminal_VT100_Foreground_Cyan);
			break;

		case Terminal_Color_ForegroundWhite:
			File_Write(this->out, Terminal_VT100_Foreground_White);
			break;
	}

	switch (color & Terminal_Color_BackgroundMask) {
		case Terminal_Color_BackgroundBlack:
			File_Write(this->out, Terminal_VT100_Background_Black);
			break;

		case Terminal_Color_BackgroundRed:
			File_Write(this->out, Terminal_VT100_Background_Red);
			break;

		case Terminal_Color_BackgroundGreen:
			File_Write(this->out, Terminal_VT100_Background_Green);
			break;

		case Terminal_Color_BackgroundYellow:
			File_Write(this->out, Terminal_VT100_Background_Yellow);
			break;

		case Terminal_Color_BackgroundBlue:
			File_Write(this->out, Terminal_VT100_Background_Blue);
			break;

		case Terminal_Color_BackgroundMagenta:
			File_Write(this->out, Terminal_VT100_Background_Magenta);
			break;

		case Terminal_Color_BackgroundCyan:
			File_Write(this->out, Terminal_VT100_Background_Cyan);
			break;

		case Terminal_Color_BackgroundWhite:
			File_Write(this->out, Terminal_VT100_Background_White);
			break;
	}

	this->style.color = color;
}

void Terminal_SetVT100Font(Terminal *this, int font) {
	if (BitMask_Has(font, Terminal_Font_Bold)) {
		File_Write(this->out, Terminal_VT100_Bold);
	}

	if (BitMask_Has(font, Terminal_Font_Italics)) {
		File_Write(this->out, Terminal_VT100_Italics);
	}

	if (BitMask_Has(font, Terminal_Font_Underline)) {
		File_Write(this->out, Terminal_VT100_Underline);
	}

	if (BitMask_Has(font, Terminal_Font_Blink)) {
		File_Write(this->out, Terminal_VT100_Blink);
	}

	this->style.font = font;
}

Terminal_Style Terminal_GetStyle(Terminal *this) {
	return this->style;
}

void Terminal_Restore(Terminal *this, Terminal_Style style) {
	if (this->style.color != style.color
	 || this->style.font  != style.font) {
		Terminal_ResetVT100(this);

		Terminal_SetVT100Font (this, style.font);
		Terminal_SetVT100Color(this, style.color);
	}
}

size_t Terminal_ResolveColorName(String name, bool bg) {
	int color = 0;

	if (String_Equals(name, String("black"))) {
		color = Terminal_Color_ForegroundBlack;
	} else if (String_Equals(name, String("red"))) {
		color = Terminal_Color_ForegroundRed;
	} else if (String_Equals(name, String("green"))) {
		color = Terminal_Color_ForegroundGreen;
	} else if (String_Equals(name, String("yellow"))) {
		color = Terminal_Color_ForegroundYellow;
	} else if (String_Equals(name, String("blue"))) {
		color = Terminal_Color_ForegroundBlue;
	} else if (String_Equals(name, String("magenta"))) {
		color = Terminal_Color_ForegroundMagenta;
	} else if (String_Equals(name, String("cyan"))) {
		color = Terminal_Color_ForegroundCyan;
	} else if (String_Equals(name, String("white"))) {
		color = Terminal_Color_ForegroundWhite;
	}

	if (bg) {
		color *= 1 << 4;
	}

	return color;
}

Terminal_Size Terminal_GetSize(void) {
	struct winsize size;

	Terminal_Size res;

	if (ioctl(FileNo_StdIn, TIOCGWINSZ, &size) == 0) {
		res.cols = size.ws_col;
		res.rows = size.ws_row;
	} else {
		throw(exc, excIoctlFailed);
	}

	return res;
}

overload void Terminal_Print(Terminal *this, int color, int font, String s) {
	/* Setup the stream with the given color if possible. */
	if (this->isVT100) {
		Terminal_SetVT100Font (this, font);
		Terminal_SetVT100Color(this, color);
	}

	/* Write the text into the stream. */
	File_Write(this->out, s);

	/* Restore the normal color state for the stream. */
	if (this->isVT100) {
		Terminal_ResetVT100(this);
	}
}

overload void Terminal_Print(Terminal *this, String s) {
	File_Write(this->out, s);
}

overload void Terminal_Print(Terminal *this, char c) {
	File_Write(this->out, &c, 1);
}

overload void Terminal_DeleteLine(Terminal *this, size_t n) {
	if (n == 1) {
		File_Write(this->out, Terminal_VT100_Delete_Line);
	} else {
		File_Write(this->out, String("\33["));
		File_Write(this->out, Integer_ToString(n));
		File_Write(this->out, String("M"));
	}
}

inline overload void Terminal_DeleteLine(Terminal *this) {
	Terminal_DeleteLine(this, 1);
}

void Terminal_DeleteUntilEol(Terminal *this) {
	File_Write(this->out, Terminal_VT100_Delete_UntilEol);
}

void Terminal_MoveHome(Terminal *this) {
	File_Write(this->out, Terminal_VT100_Cursor_Home);
}

void Terminal_MoveUp(Terminal *this, size_t n) {
	if (n == 1) {
		File_Write(this->out, Terminal_VT100_Cursor_Up);
	} else {
		File_Write(this->out, String("\33["));
		File_Write(this->out, Integer_ToString(n));
		File_Write(this->out, String("A"));
	}
}

void Terminal_MoveDown(Terminal *this, size_t n) {
	if (n == 1) {
		File_Write(this->out, Terminal_VT100_Cursor_Down);
	} else {
		File_Write(this->out, String("\33["));
		File_Write(this->out, Integer_ToString(n));
		File_Write(this->out, String("B"));
	}
}

void Terminal_MoveLeft(Terminal *this, size_t n) {
	if (n == 1) {
		File_Write(this->out, Terminal_VT100_Cursor_Left);
	} else {
		File_Write(this->out, String("\33["));
		File_Write(this->out, Integer_ToString(n));
		File_Write(this->out, String("D"));
	}
}

void Terminal_MoveRight(Terminal *this, size_t n) {
	if (n > 0) {
		if (n == 1) {
			File_Write(this->out, Terminal_VT100_Cursor_Right);
		} else {
			File_Write(this->out, String("\33["));
			File_Write(this->out, Integer_ToString(n));
			File_Write(this->out, String("C"));
		}
	}
}

void Terminal_HideCursor(Terminal *this) {
	File_Write(this->out, Terminal_VT100_Cursor_Hide);
}

void Terminal_ShowCursor(Terminal *this) {
	File_Write(this->out, Terminal_VT100_Cursor_Show);
}

void Terminal_SaveCursor(Terminal *this) {
	File_Write(this->out, Terminal_VT100_Cursor_Save);
}

void Terminal_RestoreCursor(Terminal *this) {
	File_Write(this->out, Terminal_VT100_Cursor_Restore);
}

char Terminal_ReadChar(Terminal *this) {
	char c = '\0';
	File_Read(this->in, &c, 1);
	return c;
}

Terminal_Key Terminal_ReadKey(Terminal *this) {
	Terminal_Key key;

	key.c = Terminal_ReadChar(this);
	key.t = Terminal_KeyType_Unknown;

	if (key.c == '\177') {
		key.t = Terminal_KeyType_Backspace;
	} else if (key.c == '\033') {
		char k = Terminal_ReadChar(this);

		if (k == '[') {
			char k2 = Terminal_ReadChar(this);

			if (k2 == 'A') {
				key.t = Terminal_KeyType_Down;
			} else if (k2 == 'B') {
				key.t = Terminal_KeyType_Up;
			} else if (k2 == 'C') {
				key.t = Terminal_KeyType_Right;
			} else if (k2 == 'D') {
				key.t = Terminal_KeyType_Left;
			} else if (k2 == '3') {
				Terminal_ReadChar(this);
				key.t = Terminal_KeyType_Delete;
			} else if (k2 == 'H' || k2 == '7') {
				if (k2 == '7') {
					Terminal_ReadChar(this);
				}

				key.t = Terminal_KeyType_Home;
			} else if (k2 == 'F' || k2 == '8') {
				if (k2 == '8') {
					Terminal_ReadChar(this);
				}

				key.t = Terminal_KeyType_End;
			}
		}
	}

	return key;
}
