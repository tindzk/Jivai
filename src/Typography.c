#import "Typography.h"

#define self Typography

def(void, Init) {
	Tree_Init(&this->tree, (void *) ref(DestroyNode));
}

def(void, Destroy) {
	Tree_Destroy(&this->tree);
}

void ref(DestroyNode)(ref(Node) *node) {
	if (node->type == ref(NodeType_Item)) {
		ref(Item) *item = (ref(Item) *) &node->data;

		String_Destroy(&item->name);
		String_Destroy(&item->options);
	} else if (node->type == ref(NodeType_Text)) {
		ref(Text) *text = (ref(Text) *) &node->data;

		String_Destroy(&text->value);
	}
}

def(ref(Node) *, GetRoot) {
	return (ref(Node) *) &this->tree.root;
}

static def(void, Flush, String *value) {
	if (value->len > 0) {
		ref(Node) *node = Tree_AddCustomNode(this->node,
			sizeof(ref(Node))
		  + sizeof(ref(Text)));

		node->type = ref(NodeType_Text);
		node->line = this->line;

		ref(Text) *text = (ref(Text) *) &node->data;
		text->value = String_Clone(*value);

		value->len  = 0;
	}
}

static def(void, Read, size_t st) {
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

					call(Flush, &value);
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
					call(Flush, &value);

					this->node = Tree_AddCustomNode(this->node,
						sizeof(ref(Node))
					  + sizeof(ref(Item)));

					this->node->type = ref(NodeType_Item);
					this->node->line = this->line;

					ref(Item) *item = (ref(Item) *) &this->node->data;

					item->name    = String_Clone(String_Trim(name));
					item->options = String_Clone(options);

					options.len = 0;
					name.len    = 0;

					call(Read, BLOCK);

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
					call(Flush, &value);

					if (this->node->parent == NULL) {
						throw(excIllegalNesting);
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

	call(Flush, &value);

out:
	String_Destroy(&options);
	String_Destroy(&value);
	String_Destroy(&name);
}

def(void, Parse, StreamInterface *stream, void *context) {
	this->line = 0;

	this->stream  = stream;
	this->context = context;

	Tree_Reset(&this->tree);
	this->node = (ref(Node) *) &this->tree.root;

	call(Read, 0);
}
