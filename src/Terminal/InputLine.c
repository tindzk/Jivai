#include "InputLine.h"

Exception_Define(Terminal_InputLine_CommandExceedsAllowedLengthException);

static ExceptionManager *exc;

void Terminal_InputLine0(ExceptionManager *e) {
	exc = e;
}

void Terminal_InputLine_Init(Terminal_InputLine *this, Terminal *term) {
	this->term = term;

	this->context    = NULL;
	this->onKeyUp    = NULL;
	this->onKeyDown  = NULL;
	this->onKeyLeft  = NULL;
	this->onKeyRight = NULL;
	this->onKeyBack  = NULL;
	this->onKeyPress = NULL;
	this->onKeyEnter = NULL;

	this->pos = 0;
	this->line = HeapString(150);
}

void Terminal_InputLine_Destroy(Terminal_InputLine *this) {
	String_Destroy(&this->line);
}

void Terminal_InputLine_ClearLine(Terminal_InputLine *this) {
	Terminal_MoveLeft(this->term, Unicode_Count(this->line));
	Terminal_DeleteUntilEol(this->term);

	this->line.len = 0;
	this->pos = 0;
}

void Terminal_InputLine_Print(Terminal_InputLine *this, String s) {
	Terminal_Write(this->term, s);

	String_Append(&this->line, s);
	this->pos += s.len;
}

void Terminal_InputLine_SetValue(Terminal_InputLine *this, String s) {
	Terminal_InputLine_ClearLine(this);
	Terminal_InputLine_Print(this, s);
}

void Terminal_InputLine_DeletePreceding(Terminal_InputLine *this) {
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

			String rest = String_Slice(&this->line, this->pos, String_End);
			this->line.len = this->pos - width;

			Terminal_InputLine_Print(this, rest);
			Terminal_InputLine_MoveLeft(this, rest.len);

			String_Destroy(&rest);

			this->pos -= width;
		}
	}
}

void Terminal_InputLine_DeleteSucceeding(Terminal_InputLine *this) {
	if (this->pos < this->line.len) {
		Terminal_DeleteUntilEol(this->term);

		String rest = String_Slice(&this->line, this->pos + 1, String_End);
		this->line.len = this->pos;

		Terminal_InputLine_Print(this, rest);
		Terminal_InputLine_MoveLeft(this, rest.len);

		String_Destroy(&rest);
	}
}

void Terminal_InputLine_MoveRight(Terminal_InputLine *this, size_t n) {
	if (n > 0) {
		size_t i = 0;
		size_t bytes = 0;

		while (i < n && this->pos + bytes < this->line.len) {
			size_t width = Unicode_Next(this->line, this->pos + bytes);

			if (width == 0) {
				break;
			}

			bytes += width;
			i++;
		}

		if (bytes == 0) {
			return;
		}

		Terminal_MoveRight(this->term, n);

		this->pos += bytes;
	}
}

void Terminal_InputLine_MoveLeft(Terminal_InputLine *this, size_t n) {
	if ((int) this->pos - (int) n < 0) {
		n = this->pos;
	}

	if (n > 0) {
		size_t i = 0;
		size_t bytes = 0;

		while (i < n && this->pos - bytes - 1 >= 0) {
			size_t width = Unicode_Prev(this->line, this->pos - bytes);

			if (width == 0) {
				break;
			}

			bytes += width;
			i++;
		}

		if (bytes == 0) {
			return;
		}

		Terminal_MoveLeft(this->term, n);

		this->pos -= bytes;
	}
}

void Terminal_InputLine_Process(Terminal_InputLine *this) {
	Terminal_Key key = Terminal_ReadKey(this->term);

	if (key.t == Terminal_KeyType_Backspace) {
		Terminal_InputLine_DeletePreceding(this);

		if (this->onKeyBack != NULL) {
			this->onKeyBack(this->context);
		}
	} else if (key.t == Terminal_KeyType_Up) {
		if (this->onKeyUp != NULL) {
			this->onKeyUp(this->context);
		}
	} else if (key.t == Terminal_KeyType_Down) {
		if (this->onKeyDown != NULL) {
			this->onKeyDown(this->context);
		}
	} else if (key.t == Terminal_KeyType_Right) {
		Terminal_InputLine_MoveRight(this, 1);

		if (this->onKeyRight != NULL) {
			this->onKeyRight(this->context);
		}
	} else if (key.t == Terminal_KeyType_Left) {
		Terminal_InputLine_MoveLeft(this, 1);

		if (this->onKeyLeft != NULL) {
			this->onKeyLeft(this->context);
		}
	} else if (key.t == Terminal_KeyType_Delete) {
		Terminal_InputLine_DeleteSucceeding(this);
	} else if (key.t == Terminal_KeyType_Home) {
		Terminal_InputLine_MoveLeft(this, this->pos);
	} else if (key.t == Terminal_KeyType_End) {
		Terminal_InputLine_MoveRight(this, this->line.len - this->pos);
	} else if (key.c == '\n') {
		Terminal_Write(this->term, String("\n"));

		if (this->onKeyEnter != NULL) {
			this->onKeyEnter(this->context, this->line);
		}

		this->pos = 0;
		this->line.len = 0;
	} else {
		String ch;
		size_t len = Unicode_CalcWidth(&key.c);

		if (len == 0) {
			ch = StackString(1);
			String_AppendChar(&ch, key.c);
		} else {
			ch = StackString(len);
			String_AppendChar(&ch, key.c);

			while (--len) {
				String_AppendChar(&ch, Terminal_ReadChar(this->term));
			}
		}

		bool handled = false;

		if (this->onKeyPress != NULL) {
			handled = this->onKeyPress(this->context, ch);
		}

		if (!handled) {
			if (this->pos == this->line.len) { /* EOL */
				Terminal_InputLine_Print(this, ch);
			} else {
				String rest = String_Slice(&this->line, this->pos, String_End);
				this->line.len = this->pos;

				Terminal_InputLine_Print(this, ch);
				Terminal_InputLine_Print(this, rest);

				Terminal_InputLine_MoveLeft(this, rest.len);

				String_Destroy(&rest);
			}

			if (this->line.len == this->line.size) {
				this->line.len = 0;
				this->pos = 0;

				throw(exc, &Terminal_InputLine_CommandExceedsAllowedLengthException);
			}
		}
	}
}
