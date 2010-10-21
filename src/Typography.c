#import "Typography.h"

static ExceptionManager *exc;

void Typography0(ExceptionManager *e) {
	exc = e;
}

void Typography_Init(Typography *this, StreamInterface *stream, void *context) {
	this->line = 0;

	this->stream  = stream;
	this->context = context;

	Tree_Init(&this->tree, (void *) &Typography_DestroyNode);
}

void Typography_Destroy(Typography *this) {
	Tree_Destroy(&this->tree);
}

void Typography_DestroyNode(Typography_Node *node) {
	if (node->type == Typography_NodeType_Item) {
		Typography_Item *item = (Typography_Item *) &node->data;

		String_Destroy(&item->name);
		String_Destroy(&item->options);
	} else if (node->type == Typography_NodeType_Text) {
		Typography_Text *text = (Typography_Text *) &node->data;

		String_Destroy(&text->value);
	}
}

Typography_Node* Typography_GetRoot(Typography *this) {
	return (Typography_Node *) &this->tree.root;
}

static void Typography_Flush(Typography *this, String *value) {
	if (value->len > 0) {
		Typography_Node *node = Tree_AddCustomNode(this->node,
			sizeof(Typography_Node)
		  + sizeof(Typography_Text));

		node->type = Typography_NodeType_Text;
		node->line = this->line;

		Typography_Text *text = (Typography_Text *) &node->data;
		text->value = String_Clone(*value);

		value->len  = 0;
	}
}

static void Typography_Read(Typography *this, size_t st) {
	char cur  = '\0';
	char prev = '\0';
	char next = '\0';

	enum state_t { NONE = 0, POINT, OPTIONS, BLOCK };
	enum state_t state = st;

	bool prevstate = NONE;

	String name    = HeapString(0);
	String value   = HeapString(0);
	String options = HeapString(0);

	while (!this->stream->isEof(this->context)) {
		if (next == '\0') {
			this->stream->read(this->context, &cur, 1);

			if (cur == '\n') {
				this->line++;
			}
		} else {
			cur  = next;
			next = '\0';
		}

		switch (state) {
			case NONE:
				if (cur == '.') {
					prevstate = NONE;
					state     = POINT;

					Typography_Flush(this, &value);
				} else {
					String_Append(&value, cur);
				}

				break;

			case POINT:
				if (prev == '\\') {
					if (cur != '[' && cur != ']'
					 && cur != '{' && cur != '}') {
						String_Append(&name, '\\');
					}

					String_Append(&name, cur);
				} else if (Char_IsAlpha(cur) || Char_IsDigit(cur) || (cur == ' ' && name.len != 0)) {
					String_Append(&name, cur);
				} else if (cur == '[') {
					state = OPTIONS;
				} else if (cur == '{') {
					Typography_Flush(this, &value);

					this->node = Tree_AddCustomNode(this->node,
						sizeof(Typography_Node)
					  + sizeof(Typography_Item));

					this->node->type = Typography_NodeType_Item;
					this->node->line = this->line;

					Typography_Item *item = (Typography_Item *) &this->node->data;

					item->name    = String_Clone(String_Trim(name));
					item->options = String_Clone(options);

					options.len = 0;
					name.len    = 0;

					Typography_Read(this, BLOCK);

					state = prevstate;
				} else if (cur != '\\') {
					String_Append(&value, String("."));
					String_Append(&value, name);

					name.len = 0;

					next  = cur;
					state = prevstate;
				}

				break;

			case BLOCK:
				if (prev == '\\') {
					if (cur == '{' || cur == '}') {
						value.len--;
					}

					String_Append(&value, cur);
				} else if (cur == '}') {
					Typography_Flush(this, &value);

					if (this->node->parent == NULL) {
						throw(exc, excIllegalNesting);
					}

					this->node = this->node->parent;

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

					if (cur == '\\') {
						cur = '\0';
					}
				} else if (cur == ']') {
					state = POINT;
				} else if (cur != '\\') {
					String_Append(&options, cur);
				}

				break;
		}

		prev = cur;
	}

	Typography_Flush(this, &value);

out:
	String_Destroy(&options);
	String_Destroy(&value);
	String_Destroy(&name);
}

void Typography_Parse(Typography *this) {
	Tree_Reset(&this->tree);
	this->node = (Typography_Node *) &this->tree.root;

	Typography_Read(this, 0);
}
