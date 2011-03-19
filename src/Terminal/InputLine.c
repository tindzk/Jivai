#import "InputLine.h"

#define self Terminal_InputLine

rsdef(self, New, Terminal *term) {
	return (self) {
		.term = term,

		.onKeyUp    = (ref(OnKeyUp))    EmptyCallback(),
		.onKeyDown  = (ref(OnKeyDown))  EmptyCallback(),
		.onKeyLeft  = (ref(OnKeyLeft))  EmptyCallback(),
		.onKeyRight = (ref(OnKeyRight)) EmptyCallback(),
		.onKeyBack  = (ref(OnKeyBack))  EmptyCallback(),
		.onKeyPress = (ref(OnKeyPress)) EmptyCallback(),
		.onKeyEnter = (ref(OnKeyEnter)) EmptyCallback(),

		.pos  = 0,
		.line = String_New(150),
	};
}

def(void, Destroy) {
	String_Destroy(&this->line);
}

overload def(void, ClearLine, bool update) {
	if (update) {
		size_t n = Unicode_Count(this->line.rd, 0, this->pos);

		call(MoveLeft, n);
		Terminal_DeleteUntilEol(this->term);
	}

	this->line.len = 0;
	this->pos = 0;
}

overload def(void, ClearLine) {
	call(ClearLine, true);
}

def(void, Print, RdString s) {
	Terminal_Print(this->term, s);

	String_Append(&this->line, s);
	this->pos += s.len;
}

def(void, SetValue, RdString s) {
	if (!String_Equals(this->line.rd, s)) {
		call(ClearLine);
		call(Print, s);
	}
}

def(void, DeletePreceding) {
	if (this->pos > 0) {
		Terminal_MoveLeft(this->term, 1);
		Terminal_DeleteUntilEol(this->term);

		if (this->line.len == this->pos) {
			size_t width = Unicode_Prev(this->line.rd, this->pos);

			if (width == 0) {
				return;
			}

			this->line.len = this->pos - width;
			this->pos -= width;
		} else {
			size_t width = Unicode_Prev(this->line.rd, this->pos);

			if (width == 0) {
				return;
			}

			RdString rest = String_Slice(this->line.rd, this->pos);
			this->line.len = this->pos - width;

			call(Print, rest);
			call(MoveLeft, rest.len);

			this->pos -= width;
		}
	}
}

def(void, DeleteSucceeding) {
	if (this->pos < this->line.len) {
		Terminal_DeleteUntilEol(this->term);

		RdString rest = String_Slice(this->line.rd, this->pos + 1);
		this->line.len = this->pos;

		call(Print, rest);
		call(MoveLeft, rest.len);
	}
}

def(void, MoveRight, size_t n) {
	if (n > 0) {
		size_t bytes = 0;

		for (size_t i = 0; i < n; i++) {
			size_t width = Unicode_Next(this->line.rd, this->pos + bytes);

			if (width == 0) {
				n = i;
				break;
			}

			bytes += width;
		}

		if (bytes == 0) {
			return;
		}

		Terminal_MoveRight(this->term, n);

		this->pos += bytes;
	}
}

def(void, MoveLeft, size_t n) {
	if (n > 0) {
		size_t bytes = 0;

		for (size_t i = 0; i < n; i++) {
			size_t width = Unicode_Prev(this->line.rd, this->pos - bytes);

			if (width == 0) {
				n = i;
				break;
			}

			bytes += width;
		}

		if (bytes == 0) {
			return;
		}

		Terminal_MoveLeft(this->term, n);

		this->pos -= bytes;
	}
}

def(void, Process) {
	Terminal_Key key = Terminal_ReadKey(this->term);

	if (key.t == Terminal_KeyType_Backspace) {
		call(DeletePreceding);
		callback(this->onKeyBack);
	} else if (key.t == Terminal_KeyType_Up) {
		callback(this->onKeyUp);
	} else if (key.t == Terminal_KeyType_Down) {
		callback(this->onKeyDown);
	} else if (key.t == Terminal_KeyType_Right) {
		call(MoveRight, 1);
		callback(this->onKeyRight);
	} else if (key.t == Terminal_KeyType_Left) {
		call(MoveLeft, 1);
		callback(this->onKeyLeft);
	} else if (key.t == Terminal_KeyType_Delete) {
		call(DeleteSucceeding);
	} else if (key.t == Terminal_KeyType_Home) {
		call(MoveLeft, this->pos);
	} else if (key.t == Terminal_KeyType_End) {
		call(MoveRight, this->line.len - this->pos);
	} else if (key.c == '\n') {
		Terminal_Print(this->term, $("\n"));

		if (hasCallback(this->onKeyEnter)) {
			RdString line = this->line.rd;

			this->pos      = 0;
			this->line.len = 0;

			callback(this->onKeyEnter, line);
		}
	} else {
		String ch;
		ssize_t len = Unicode_CalcWidth(&key.c);

		if (len == 0) {
			ch = String_New(1);
			String_Append(&ch, key.c);
		} else {
			ch = String_New(len);
			String_Append(&ch, key.c);

			while (--len) {
				String_Append(&ch, Terminal_ReadChar(this->term));
			}
		}

		bool handled = callbackRet(this->onKeyPress, false, ch.rd);

		if (!handled) {
			if (this->pos == this->line.len) { /* EOL */
				call(Print, ch.rd);
			} else {
				String rest =
					String_Clone(
						String_Slice(
							this->line.rd, this->pos));

				String_Crop(&this->line, 0, this->pos);

				call(Print, ch.rd);
				call(Print, rest.rd);

				size_t n = Unicode_Count(this->line.rd,
					this->line.len - rest.len,
					rest.len);

				call(MoveLeft, n);

				String_Destroy(&rest);
			}

			if (this->line.len == String_GetSize(this->line)) {
				this->line.len = 0;
				this->pos      = 0;

				throw(CommandExceedsAllowedLength);
			}
		}

		String_Destroy(&ch);
	}
}
