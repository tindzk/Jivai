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

	tcgetattr(STDIN_FILENO, &this->oldTermios);

	this->curTermios = this->oldTermios;
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

	tcsetattr(STDIN_FILENO, TCSAFLUSH, &this->curTermios);
}

void Terminal_Destroy(Terminal *this) {
	tcsetattr(STDIN_FILENO, 0, &this->oldTermios);
}

/* Write VT100 escape sequences to the stream for the given color. */
void Terminal_SetVT100Color(Terminal *this, int color) {
	if (color == Terminal_Color_Normal) {
		File_Write(this->out, Terminal_VT100_Normal);
		return;
	}

	switch (color & Terminal_Color_ForegroundMask) {
		case Terminal_Color_Normal:
			File_Write(this->out, Terminal_VT100_Normal);
			break;

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
}

Terminal_Size Terminal_GetSize(void) {
	struct winsize size;

	Terminal_Size res;

	if (ioctl(STDIN_FILENO, TIOCGWINSZ, &size) == 0) {
		res.cols = (int) size.ws_col;
		res.rows = (int) size.ws_row;
	} else {
		throw(exc, excIoctlFailed);
	}

	return res;
}

void Terminal_Write(Terminal *this, String s) {
	File_Write(this->out, s);
}

overload void Terminal_Print(Terminal *this, int color, int font, String s) {
	/* Setup the stream with the given color if possible. */
	if (this->isVT100) {
		Terminal_SetVT100Color(this, color);
		Terminal_SetVT100Font(this, font);
	}

	/* Write the text into the stream. */
	File_Write(this->out, s);

	/* Restore the normal color state for the stream. */
	if (this->isVT100) {
		Terminal_SetVT100Color(this, Terminal_Color_Normal);
	}
}

overload void Terminal_Print(Terminal *this, String s) {
	File_Write(this->out, s);
}

void Terminal_DeleteLine(Terminal *this) {
	File_Write(this->out, Terminal_VT100_Delete_Line);
}

void Terminal_DeleteUntilEol(Terminal *this) {
	File_Write(this->out, Terminal_VT100_Delete_UntilEol);
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
