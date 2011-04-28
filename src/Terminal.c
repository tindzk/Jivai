#import "Terminal.h"

#define self Terminal

sdef(bool, IsTTY, Channel *ch) {
	struct termios term;
	return ioctl(Channel_GetId(ch), TCGETS, &term) == 0;
}

rsdef(self, New, bool assumeVT100) {
	self res;

	res.in  = Channel_StdIn;
	res.out = Channel_StdOut;

	res.isVT100 = assumeVT100 && scall(IsTTY, res.out);

	tcgetattr(ChannelId_StdIn, &res.oldTermios);

	res.curTermios = res.oldTermios;

	res.style = (ref(Style)) {0, 0};

	return res;
}

def(void, SetInput, Channel *ch) {
	this->in = ch;
}

def(void, SetOutput, Channel *ch) {
	this->out = ch;
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

	tcsetattr(ChannelId_StdIn, TCSAFLUSH, &this->curTermios);
}

def(void, Destroy) {
	tcsetattr(ChannelId_StdIn, 0, &this->oldTermios);
}

def(void, ResetVT100) {
	Channel_Write(this->out, ref(VT100_Normal));

	this->style = (ref(Style)) {0, 0};
}

/* Write VT100 escape sequences to the stream for the given color. */
def(void, SetVT100Color, int color) {
	switch (color & ref(Color_ForegroundMask)) {
		case ref(Color_ForegroundBlack):
			Channel_Write(this->out, ref(VT100_Foreground_Black));
			break;

		case ref(Color_ForegroundRed):
			Channel_Write(this->out, ref(VT100_Foreground_Red));
			break;

		case ref(Color_ForegroundGreen):
			Channel_Write(this->out, ref(VT100_Foreground_Green));
			break;

		case ref(Color_ForegroundYellow):
			Channel_Write(this->out, ref(VT100_Foreground_Yellow));
			break;

		case ref(Color_ForegroundBlue):
			Channel_Write(this->out, ref(VT100_Foreground_Blue));
			break;

		case ref(Color_ForegroundMagenta):
			Channel_Write(this->out, ref(VT100_Foreground_Magenta));
			break;

		case ref(Color_ForegroundCyan):
			Channel_Write(this->out, ref(VT100_Foreground_Cyan));
			break;

		case ref(Color_ForegroundWhite):
			Channel_Write(this->out, ref(VT100_Foreground_White));
			break;
	}

	switch (color & ref(Color_BackgroundMask)) {
		case ref(Color_BackgroundBlack):
			Channel_Write(this->out, ref(VT100_Background_Black));
			break;

		case ref(Color_BackgroundRed):
			Channel_Write(this->out, ref(VT100_Background_Red));
			break;

		case ref(Color_BackgroundGreen):
			Channel_Write(this->out, ref(VT100_Background_Green));
			break;

		case ref(Color_BackgroundYellow):
			Channel_Write(this->out, ref(VT100_Background_Yellow));
			break;

		case ref(Color_BackgroundBlue):
			Channel_Write(this->out, ref(VT100_Background_Blue));
			break;

		case ref(Color_BackgroundMagenta):
			Channel_Write(this->out, ref(VT100_Background_Magenta));
			break;

		case ref(Color_BackgroundCyan):
			Channel_Write(this->out, ref(VT100_Background_Cyan));
			break;

		case ref(Color_BackgroundWhite):
			Channel_Write(this->out, ref(VT100_Background_White));
			break;
	}

	this->style.color = color;
}

def(void, SetVT100Font, int font) {
	if (BitMask_Has(font, ref(Font_Bold))) {
		Channel_Write(this->out, ref(VT100_Bold));
	}

	if (BitMask_Has(font, ref(Font_Italics))) {
		Channel_Write(this->out, ref(VT100_Italics));
	}

	if (BitMask_Has(font, ref(Font_Underline))) {
		Channel_Write(this->out, ref(VT100_Underline));
	}

	if (BitMask_Has(font, ref(Font_Blink))) {
		Channel_Write(this->out, ref(VT100_Blink));
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

sdef(size_t, ResolveColorName, RdString name, bool bg) {
	int color = 0;

	if (String_Equals(name, $("black"))) {
		color = ref(Color_ForegroundBlack);
	} else if (String_Equals(name, $("red"))) {
		color = ref(Color_ForegroundRed);
	} else if (String_Equals(name, $("green"))) {
		color = ref(Color_ForegroundGreen);
	} else if (String_Equals(name, $("yellow"))) {
		color = ref(Color_ForegroundYellow);
	} else if (String_Equals(name, $("blue"))) {
		color = ref(Color_ForegroundBlue);
	} else if (String_Equals(name, $("magenta"))) {
		color = ref(Color_ForegroundMagenta);
	} else if (String_Equals(name, $("cyan"))) {
		color = ref(Color_ForegroundCyan);
	} else if (String_Equals(name, $("white"))) {
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

	if (ioctl(ChannelId_StdIn, TIOCGWINSZ, &size) == 0) {
		res.cols = size.ws_col;
		res.rows = size.ws_row;
	} else {
		throw(IoctlFailed);
	}

	return res;
}

overload def(void, Print, int color, int font, RdString s) {
	/* Setup the stream with the given color if possible. */
	if (this->isVT100) {
		call(SetVT100Font,  font);
		call(SetVT100Color, color);
	}

	/* Write the text into the stream. */
	Channel_Write(this->out, s);

	/* Restore the normal color state for the stream. */
	if (this->isVT100) {
		call(ResetVT100);
	}
}

overload def(void, Print, RdString s) {
	Channel_Write(this->out, s);
}

overload def(void, Print, char c) {
	Channel_Write(this->out, c);
}

def(void, PrintFmt, FmtString s) {
#if String_FmtChecks
	s.val++;
#endif

	fwd(i, s.fmt.len) {
		if (i + 1 < s.fmt.len && s.fmt.buf[i] == '!' && s.fmt.buf[i + 1] == '%') {
			Channel_Write(this->out, '%');
			i++;
		} else if (s.fmt.buf[i] == '%') {
#if String_FmtChecks
			if (s.val->len == (size_t) -1) {
				throw(ElementMismatch);
			}
#endif

			Channel_Write(this->out, *s.val);
			s.val++;
		} else {
			Channel_Write(this->out, s.fmt.buf[i]);
		}
	}
}

def(void, FmtArgPrint, RdString fmt, VarArg *argptr) {
	fwd(i, fmt.len) {
		if (i + 1 < fmt.len && fmt.buf[i] == '!' &&
			(fmt.buf[i + 1] == '$' || fmt.buf[i + 1] == '%'))
		{
			Channel_Write(this->out, fmt.buf[i + 1]);
			i++;
		} else if (fmt.buf[i] == '$') {
			call(PrintFmt, VarArg_Get(*argptr, FmtString));
		} else if (fmt.buf[i] == '%') {
			Channel_Write(this->out, VarArg_Get(*argptr, RdString));
		} else {
			Channel_Write(this->out, fmt.buf[i]);
		}
	}
}

def(void, FmtPrint, RdString fmt, ...) {
	VarArg argptr;
	VarArg_Start(argptr, fmt);
	call(FmtArgPrint, fmt, &argptr);
	VarArg_End(argptr);
}

def(void, DeleteLine, size_t n) {
	if (n == 1) {
		Channel_Write(this->out, ref(VT100_Delete_Line));
	} else {
		String s = Integer_ToString(n);

		Channel_Write(this->out, $("\33["));
		Channel_Write(this->out, s.rd);
		Channel_Write(this->out, $("M"));

		String_Destroy(&s);
	}
}

def(void, DeleteUntilEol) {
	Channel_Write(this->out, ref(VT100_Delete_UntilEol));
}

def(void, MoveHome) {
	Channel_Write(this->out, ref(VT100_Cursor_Home));
}

def(void, MoveUp, size_t n) {
	if (n == 1) {
		Channel_Write(this->out, ref(VT100_Cursor_Up));
	} else {
		String s = Integer_ToString(n);

		Channel_Write(this->out, $("\33["));
		Channel_Write(this->out, s.rd);
		Channel_Write(this->out, $("A"));

		String_Destroy(&s);
	}
}

def(void, MoveDown, size_t n) {
	if (n == 1) {
		Channel_Write(this->out, ref(VT100_Cursor_Down));
	} else {
		String s = Integer_ToString(n);

		Channel_Write(this->out, $("\33["));
		Channel_Write(this->out, s.rd);
		Channel_Write(this->out, $("B"));

		String_Destroy(&s);
	}
}

def(void, MoveLeft, size_t n) {
	if (n == 1) {
		Channel_Write(this->out, ref(VT100_Cursor_Left));
	} else {
		String s = Integer_ToString(n);

		Channel_Write(this->out, $("\33["));
		Channel_Write(this->out, s.rd);
		Channel_Write(this->out, $("D"));

		String_Destroy(&s);
	}
}

def(void, MoveRight, size_t n) {
	if (n > 0) {
		if (n == 1) {
			Channel_Write(this->out, ref(VT100_Cursor_Right));
		} else {
			String s = Integer_ToString(n);

			Channel_Write(this->out, $("\33["));
			Channel_Write(this->out, s.rd);
			Channel_Write(this->out, $("C"));

			String_Destroy(&s);
		}
	}
}

def(void, HideCursor) {
	Channel_Write(this->out, ref(VT100_Cursor_Hide));
}

def(void, ShowCursor) {
	Channel_Write(this->out, ref(VT100_Cursor_Show));
}

def(void, SaveCursor) {
	Channel_Write(this->out, ref(VT100_Cursor_Save));
}

def(void, RestoreCursor) {
	Channel_Write(this->out, ref(VT100_Cursor_Restore));
}

def(char, ReadChar) {
	char c = '\0';
	Channel_Read(this->in, &c, 1);
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
