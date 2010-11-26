#import "InputLine.h"
#import "../App.h"

def(void, Init, Terminal *term) {
	this->term = term;

	this->onKeyUp    = (ref(OnKeyUp))    EmptyCallback();
	this->onKeyDown  = (ref(OnKeyDown))  EmptyCallback();
	this->onKeyLeft  = (ref(OnKeyLeft))  EmptyCallback();
	this->onKeyRight = (ref(OnKeyRight)) EmptyCallback();
	this->onKeyBack  = (ref(OnKeyBack))  EmptyCallback();
	this->onKeyPress = (ref(OnKeyPress)) EmptyCallback();
	this->onKeyEnter = (ref(OnKeyEnter)) EmptyCallback();

	this->pos  = 0;
	this->line = HeapString(150);
}

def(void, Destroy) {
	String_Destroy(&this->line);
}

overload def(void, ClearLine, bool update) {
	if (update) {
		size_t n = Unicode_Count(this->line, 0, this->pos);

		call(MoveLeft, n);
		Terminal_DeleteUntilEol(this->term);
	}

	this->line.len = 0;
	this->pos = 0;
}

overload def(void, ClearLine) {
	call(ClearLine, true);
}

def(void, Print, String s) {
	Terminal_Print(this->term, s);

	String_Append(&this->line, s);
	this->pos += s.len;
}

def(void, SetValue, String s) {
	if (!String_Equals(this->line, s)) {
		call(ClearLine);
		call(Print, s);
	}
}

def(void, DeletePreceding) {
	if (this->pos > 0) {
		Terminal_MoveLeft(this->term, 1);
		Terminal_DeleteUntilEol(this->term);

		if (this->line.len == this->pos) {
			size_t width = Unicode_Prev(this->line, this->pos);

			if (width == 0) {
				return;
			}

			this->line.len = this->pos - width;
			this->pos -= width;
		} else {
			size_t width = Unicode_Prev(this->line, this->pos);

			if (width == 0) {
				return;
			}

			String rest = String_Slice(this->line, this->pos);
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

		String rest = String_Slice(this->line, this->pos + 1);
		this->line.len = this->pos;

		call(Print, rest);
		call(MoveLeft, rest.len);
	}
}

def(void, MoveRight, size_t n) {
	if (n > 0) {
		size_t bytes = 0;

		for (size_t i = 0; i < n; i++) {
			size_t width = Unicode_Next(this->line, this->pos + bytes);

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
			size_t width = Unicode_Prev(this->line, this->pos - bytes);

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
		Terminal_Print(this->term, String("\n"));

		if (hasCallback(this->onKeyEnter)) {
			String line = String_Disown(this->line);

			this->pos      = 0;
			this->line.len = 0;

			callback(this->onKeyEnter, line);
		}
	} else {
		String ch;
		ssize_t len = Unicode_CalcWidth(&key.c);

		if (len == 0) {
			ch = StackString(1);
			String_Append(&ch, key.c);
		} else {
			ch = StackString(len);
			String_Append(&ch, key.c);

			while (--len) {
				String_Append(&ch, Terminal_ReadChar(this->term));
			}
		}

		bool handled = callbackRet(this->onKeyPress, false, ch);

		if (!handled) {
			if (this->pos == this->line.len) { /* EOL */
				call(Print, ch);
			} else {
				String rest =
					String_Clone(
						String_Slice(
							this->line, this->pos));

				String_Crop(&this->line, 0, this->pos);

				call(Print, ch);
				call(Print, rest);

				size_t n = Unicode_Count(this->line,
					this->line.len - rest.len,
					rest.len);

				call(MoveLeft, n);

				String_Destroy(&rest);
			}

			if (this->line.len == this->line.size) {
				this->line.len = 0;
				this->pos      = 0;

				throw(excCommandExceedsAllowedLength);
			}
		}
	}
}
