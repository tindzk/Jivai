#import "Parser.h"

#define self Ecriture_Parser

rsdef(self, New, Ecriture_OnToken onToken) {
	return (self) {
		.onToken = onToken
	};
}

def(void, Destroy) { }

static def(char, Read, size_t st, char next) {
	char cur    = '\0';
	char prev   = '\0';
	char inject = '\0';

	enum state_t { NONE, POINT, OPTIONS, BLOCK, LITERAL };

	enum state_t state     = st;
	enum state_t prevstate = NONE;

	String name    = String_New(0);
	String value   = String_New(0);
	String options = String_New(0);

	while (next != '\0') {
		if (inject != '\0') {
			cur    = inject;
			inject = '\0';
		} else if (next != '\0') {
			cur = next;

			if (delegate(this->stream, read, Buffer_ForChar(&next)) == 0) {
				next = '\0';
			}

			if (cur == '\n') {
				this->line++;
			}
		}

		switch (state) {
			case NONE:
				if (cur == '.') {
					prevstate = NONE;
					state     = POINT;

					if (value.len != 0) {
						callback(this->onToken, Ecriture_TokenType_Value, String_Clone(value.rd), this->line);
						value.len = 0;
					}
				} else {
					String_Append(&value, cur);
				}

				break;

			case POINT:
				if (prev == '\\') {
					if (cur != '[' && cur != ']' &&
						cur != '{' && cur != '}')
					{
						String_Append(&name, '\\');
					}

					String_Append(&name, cur);
				} else if (Char_IsAlpha(cur) || Char_IsDigit(cur) || (cur == ' ' && name.len != 0)) {
					String_Append(&name, cur);
				} else if (cur == '[') {
					callback(this->onToken, Ecriture_TokenType_TagStart, String_Clone(name.rd), this->line);
					name.len = 0;

					state = OPTIONS;
				} else if (cur == '{') {
					if (value.len != 0) {
						callback(this->onToken, Ecriture_TokenType_Value, String_Clone(value.rd), this->line);
						value.len = 0;
					}

					if (name.len != 0) {
						callback(this->onToken, Ecriture_TokenType_TagStart, String_Clone(name.rd), this->line);
						name.len = 0;
					}

					if (next == '{') {
						if (delegate(this->stream, read, Buffer_ForChar(&next)) == 0) {
							next = '\0';
						}

						next = call(Read, LITERAL, next);
					} else {
						next = call(Read, BLOCK, next);
					}

					state = prevstate;
				} else if (cur != '\\') {
					String_Append(&value, '.');
					String_Append(&value, name.rd);

					name.len = 0;

					inject = cur;
					state  = prevstate;
				}

				break;

			case BLOCK:
				if (prev == '\\') {
					if (cur == '{' || cur == '}') {
						value.len--;
					}

					String_Append(&value, cur);
				} else if (cur == '}') {
					if (value.len != 0) {
						callback(this->onToken, Ecriture_TokenType_Value, String_Clone(value.rd), this->line);
						value.len = 0;
					}

					callback(this->onToken, Ecriture_TokenType_TagEnd, String_New(0), this->line);

					goto out;
				} else if (cur == '.') {
					prevstate = BLOCK;
					state     = POINT;
				} else {
					String_Append(&value, cur);
				}

				break;

			case OPTIONS:
				if (prev == '\\') {
					String_Append(&options, cur);
				} else if (cur == ']') {
					callback(this->onToken, Ecriture_TokenType_Option, String_Clone(options.rd), this->line);
					options.len = 0;
					state = POINT;
				} else if (cur != '\\') {
					String_Append(&options, cur);
				}

				break;

			case LITERAL:
				if (prev == '\\') {
					if (cur != '}' && next != '}') {
						String_Append(&value, '\\');
					}

					String_Append(&value, cur);
				} else if (cur == '}' && next == '}') {
					if (value.len != 0) {
						callback(this->onToken, Ecriture_TokenType_Value, String_Clone(value.rd), this->line);
						value.len = 0;
					}

					callback(this->onToken, Ecriture_TokenType_TagEnd, String_New(0), this->line);

					if (delegate(this->stream, read, Buffer_ForChar(&next)) == 0) {
						next = '\0';
					}

					goto out;
				} else if (cur != '\\') {
					String_Append(&value, cur);
				}

				break;
		}

		prev = cur;
	}

	if (value.len != 0) {
		callback(this->onToken, Ecriture_TokenType_Value, String_Clone(value.rd), this->line);
	}

out:
	String_Destroy(&options);
	String_Destroy(&value);
	String_Destroy(&name);

	return next;
}

def(void, Process, Stream stream) {
	this->line   = 1;
	this->stream = stream;

	char c = '\0';
	delegate(this->stream, read, Buffer_ForChar(&c));

	call(Read, 0, c);

	callback(this->onToken, Ecriture_TokenType_Done, String_New(0), this->line);
}
