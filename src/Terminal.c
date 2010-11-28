#import "Terminal.h"

#undef Terminal_Print

sdef(bool, IsTTY, File *file) {
	struct termios term;
	return ioctl(file->fd, TCGETS, &term) == 0;
}

def(void, Init, File *in, File *out, bool assumeVT100) {
	this->in  = in;
	this->out = out;

	this->isVT100 = assumeVT100 && scall(IsTTY, out);

	tcgetattr(FileNo_StdIn, &this->oldTermios);

	this->curTermios = this->oldTermios;

	this->style = (ref(Style)) {0, 0};
}

def(void, Configure, bool echo, bool signal) {
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

def(void, Destroy) {
	tcsetattr(FileNo_StdIn, 0, &this->oldTermios);
}

def(void, Write, String s) {
	File_Write(this->out, s);
}

def(void, ResetVT100) {
	call(Write, ref(VT100_Normal));

	this->style = (ref(Style)) {0, 0};
}

/* Write VT100 escape sequences to the stream for the given color. */
def(void, SetVT100Color, int color) {
	switch (color & ref(Color_ForegroundMask)) {
		case ref(Color_ForegroundBlack):
			call(Write, ref(VT100_Foreground_Black));
			break;

		case ref(Color_ForegroundRed):
			call(Write, ref(VT100_Foreground_Red));
			break;

		case ref(Color_ForegroundGreen):
			call(Write, ref(VT100_Foreground_Green));
			break;

		case ref(Color_ForegroundYellow):
			call(Write, ref(VT100_Foreground_Yellow));
			break;

		case ref(Color_ForegroundBlue):
			call(Write, ref(VT100_Foreground_Blue));
			break;

		case ref(Color_ForegroundMagenta):
			call(Write, ref(VT100_Foreground_Magenta));
			break;

		case ref(Color_ForegroundCyan):
			call(Write, ref(VT100_Foreground_Cyan));
			break;

		case ref(Color_ForegroundWhite):
			call(Write, ref(VT100_Foreground_White));
			break;
	}

	switch (color & ref(Color_BackgroundMask)) {
		case ref(Color_BackgroundBlack):
			call(Write, ref(VT100_Background_Black));
			break;

		case ref(Color_BackgroundRed):
			call(Write, ref(VT100_Background_Red));
			break;

		case ref(Color_BackgroundGreen):
			call(Write, ref(VT100_Background_Green));
			break;

		case ref(Color_BackgroundYellow):
			call(Write, ref(VT100_Background_Yellow));
			break;

		case ref(Color_BackgroundBlue):
			call(Write, ref(VT100_Background_Blue));
			break;

		case ref(Color_BackgroundMagenta):
			call(Write, ref(VT100_Background_Magenta));
			break;

		case ref(Color_BackgroundCyan):
			call(Write, ref(VT100_Background_Cyan));
			break;

		case ref(Color_BackgroundWhite):
			call(Write, ref(VT100_Background_White));
			break;
	}

	this->style.color = color;
}

def(void, SetVT100Font, int font) {
	if (BitMask_Has(font, ref(Font_Bold))) {
		call(Write, ref(VT100_Bold));
	}

	if (BitMask_Has(font, ref(Font_Italics))) {
		call(Write, ref(VT100_Italics));
	}

	if (BitMask_Has(font, ref(Font_Underline))) {
		call(Write, ref(VT100_Underline));
	}

	if (BitMask_Has(font, ref(Font_Blink))) {
		call(Write, ref(VT100_Blink));
	}

	this->style.font = font;
}

def(ref(Style), GetStyle) {
	return this->style;
}

def(void, Restore, ref(Style) style) {
	if (this->style.color != style.color
	 || this->style.font  != style.font) {
		call(ResetVT100);

		call(SetVT100Font,  style.font);
		call(SetVT100Color, style.color);
	}
}

sdef(size_t, ResolveColorName, String name, bool bg) {
	int color = 0;

	if (String_Equals(name, String("black"))) {
		color = ref(Color_ForegroundBlack);
	} else if (String_Equals(name, String("red"))) {
		color = ref(Color_ForegroundRed);
	} else if (String_Equals(name, String("green"))) {
		color = ref(Color_ForegroundGreen);
	} else if (String_Equals(name, String("yellow"))) {
		color = ref(Color_ForegroundYellow);
	} else if (String_Equals(name, String("blue"))) {
		color = ref(Color_ForegroundBlue);
	} else if (String_Equals(name, String("magenta"))) {
		color = ref(Color_ForegroundMagenta);
	} else if (String_Equals(name, String("cyan"))) {
		color = ref(Color_ForegroundCyan);
	} else if (String_Equals(name, String("white"))) {
		color = ref(Color_ForegroundWhite);
	}

	if (bg) {
		color *= 1 << 4;
	}

	return color;
}

sdef(ref(Size), GetSize) {
	struct winsize size;

	ref(Size) res;

	if (ioctl(FileNo_StdIn, TIOCGWINSZ, &size) == 0) {
		res.cols = size.ws_col;
		res.rows = size.ws_row;
	} else {
		throw(excIoctlFailed);
	}

	return res;
}

overload def(void, Print, int color, int font, String s) {
	/* Setup the stream with the given color if possible. */
	if (this->isVT100) {
		call(SetVT100Font,  font);
		call(SetVT100Color, color);
	}

	/* Write the text into the stream. */
	call(Write, s);

	/* Restore the normal color state for the stream. */
	if (this->isVT100) {
		call(ResetVT100);
	}
}

overload def(void, Print, String s) {
	call(Write, s);
}

overload def(void, Print, char c) {
	File_Write(this->out, &c, 1);
}

def(void, DeleteLine, size_t n) {
	if (n == 1) {
		call(Write, ref(VT100_Delete_Line));
	} else {
		call(Write, String("\33["));
		call(Write, Int32_ToString(n));
		call(Write, String("M"));
	}
}

def(void, DeleteUntilEol) {
	call(Write, ref(VT100_Delete_UntilEol));
}

def(void, MoveHome) {
	call(Write, ref(VT100_Cursor_Home));
}

def(void, MoveUp, size_t n) {
	if (n == 1) {
		call(Write, ref(VT100_Cursor_Up));
	} else {
		call(Write, String("\33["));
		call(Write, Int32_ToString(n));
		call(Write, String("A"));
	}
}

def(void, MoveDown, size_t n) {
	if (n == 1) {
		call(Write, ref(VT100_Cursor_Down));
	} else {
		call(Write, String("\33["));
		call(Write, Int32_ToString(n));
		call(Write, String("B"));
	}
}

def(void, MoveLeft, size_t n) {
	if (n == 1) {
		call(Write, ref(VT100_Cursor_Left));
	} else {
		call(Write, String("\33["));
		call(Write, Int32_ToString(n));
		call(Write, String("D"));
	}
}

def(void, MoveRight, size_t n) {
	if (n > 0) {
		if (n == 1) {
			call(Write, ref(VT100_Cursor_Right));
		} else {
			call(Write, String("\33["));
			call(Write, Int32_ToString(n));
			call(Write, String("C"));
		}
	}
}

def(void, HideCursor) {
	call(Write, ref(VT100_Cursor_Hide));
}

def(void, ShowCursor) {
	call(Write, ref(VT100_Cursor_Show));
}

def(void, SaveCursor) {
	call(Write, ref(VT100_Cursor_Save));
}

def(void, RestoreCursor) {
	call(Write, ref(VT100_Cursor_Restore));
}

def(char, ReadChar) {
	char c = '\0';
	File_Read(this->in, &c, 1);
	return c;
}

def(ref(Key), ReadKey) {
	ref(Key) key;

	key.c = call(ReadChar);
	key.t = ref(KeyType_Unknown);

	if (key.c == '\177') {
		key.t = ref(KeyType_Backspace);
	} else if (key.c == '\033') {
		char k = call(ReadChar);

		if (k == '[') {
			char k2 = call(ReadChar);

			if (k2 == 'A') {
				key.t = ref(KeyType_Down);
			} else if (k2 == 'B') {
				key.t = ref(KeyType_Up);
			} else if (k2 == 'C') {
				key.t = ref(KeyType_Right);
			} else if (k2 == 'D') {
				key.t = ref(KeyType_Left);
			} else if (k2 == '3') {
				call(ReadChar);
				key.t = ref(KeyType_Delete);
			} else if (k2 == 'H' || k2 == '7') {
				if (k2 == '7') {
					call(ReadChar);
				}

				key.t = ref(KeyType_Home);
			} else if (k2 == 'F' || k2 == '8') {
				if (k2 == '8') {
					call(ReadChar);
				}

				key.t = ref(KeyType_End);
			}
		}
	}

	return key;
}
