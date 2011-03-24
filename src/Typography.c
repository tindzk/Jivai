#import "Typography.h"

#define self Typography

rsdef(self, New) {
	return (self) {
		.tree = Tree_New(Callback(NULL, ref(DestroyNode)))
	};
}

def(void, Destroy) {
	Tree_Destroy(&this->tree);
}

def(void, DestroyNode, ref(Node) *node) {
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
		ref(Node) *node =
			(ref(Node) *) Tree_AddCustomNode(&this->tree,
				(Tree_Node *) this->node,
				sizeof(ref(Node)) +
				sizeof(ref(Text)));

		node->type = ref(NodeType_Text);
		node->line = this->line;

		ref(Text) *text = (ref(Text) *) &node->data;
		text->value = String_Clone(value->rd);

		value->len = 0;
	}
}

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

			if (delegate(this->stream, read, &next, 1) == 0) {
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

					call(Flush, &value);
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
					state = OPTIONS;
				} else if (cur == '{') {
					call(Flush, &value);

					this->node =
						(ref(Node) *) Tree_AddCustomNode(&this->tree,
							(Tree_Node *) this->node,
							sizeof(ref(Node)) +
							sizeof(ref(Item)));

					this->node->type = ref(NodeType_Item);
					this->node->line = this->line;

					ref(Item) *item = (ref(Item) *) &this->node->data;

					item->name    = String_Clone(String_Trim(name.rd));
					item->options = String_Clone(options.rd);

					options.len = 0;
					name.len    = 0;

					if (next == '{') {
						if (delegate(this->stream, read, &next, 1) == 0) {
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
					call(Flush, &value);

					if (this->node->parent == NULL) {
						throw(IllegalNesting);
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
				} else if (cur == ']') {
					state = POINT;
				} else if (cur != '\\') {
					String_Append(&options, cur);
				}

				break;

			case LITERAL:
				if (prev == '\\') {
					String_Append(&value, cur);
				} else if (cur == '}' && next == '}') {
					call(Flush, &value);

					if (this->node->parent == NULL) {
						throw(IllegalNesting);
					}

					this->node = this->node->parent;

					if (delegate(this->stream, read, &next, 1) == 0) {
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

	call(Flush, &value);

out:
	String_Destroy(&options);
	String_Destroy(&value);
	String_Destroy(&name);

	return next;
}

def(void, Parse, Stream stream) {
	this->line   = 1;
	this->node   = NULL;
	this->stream = stream;

	Tree_Reset(&this->tree);

	char c = '\0';
	delegate(this->stream, read, &c, 1);

	call(Read, 0, c);
}
