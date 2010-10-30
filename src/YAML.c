#import "YAML.h"
#import "App.h"

static ExceptionManager *exc;

void YAML0(ExceptionManager *e) {
	exc = e;
}

def(void, Init, size_t depthWidth, StreamInterface *stream, void *context) {
	this->depthWidth = depthWidth;

	this->stream  = stream;
	this->context = context;

	Tree_Init(&this->tree, (void *) ref(DestroyNode));
}

def(void, Destroy) {
	Tree_Destroy(&this->tree);
}

void ref(DestroyNode)(ref(Node) *node) {
	if (node->type == ref(NodeType_Section)) {
		ref(Section) *section = (ref(Section) *) &node->data;

		String_Destroy(&section->name);
	} else if (node->type == ref(NodeType_Item)) {
		ref(Item) *item = (ref(Item) *) &node->data;

		String_Destroy(&item->key);
		String_Destroy(&item->value);
	}
}

def(ref(Node) *, GetRoot) {
	return (ref(Node) *) &this->tree.root;
}

def(size_t, GetLine) {
	return this->line;
}

def(void *, Store, size_t depth, ref(NodeType) type, size_t size) {
	bool storeInSubNode = false;

	if (depth > this->depth) {
		if (this->node->len == 0) {
			this->node = Tree_AddCustomNode(this->node,
				sizeof(ref(Node)) + size);
		} else {
			this->node = Tree_AddCustomNode(
				this->node->nodes[this->node->len - 1],
				sizeof(ref(Node)) + size);
		}

		this->node->type = ref(NodeType_Node);

		this->depth++;

		storeInSubNode = true;
	} else if (depth < this->depth) {
		while (this->depth - depth > 0) {
			if (this->node->parent == NULL) {
				throw(exc, excIllegalNesting);
			}

			if (this->node->type == ref(NodeType_Node)) {
				this->depth--;
			}

			this->node = this->node->parent;
		}

		if (this->node->parent == NULL) {
			throw(exc, excIllegalNesting);
		}

		if (this->node->parent != NULL) {
			this->node = this->node->parent;
		}
	} else {
		if (this->node->parent != NULL) {
			if (this->node->parent->type == ref(NodeType_Node)) {
				this->node = this->node->parent;
			}
		}
	}

	ref(Node) *node = Tree_AddCustomNode(this->node,
		sizeof(ref(Node)) + size);

	node->type = type;

	if (storeInSubNode) {
		this->node = node;
	}

	return &node->data;
}

def(void, AddSection, size_t depth, String s) {
	ref(Section) *section = call(Store, depth, ref(NodeType_Section), sizeof(ref(Section)));
	section->name = String_Clone(s);
}

def(void, AddItem, size_t depth, String key, String value) {
	ref(Item) *item = call(Store, depth, ref(NodeType_Item), sizeof(ref(Item)));

	item->key   = String_Clone(key);
	item->value = String_Clone(value);
}

def(void, Parse) {
	char c;

	enum state_t { COMMENT, KEY, VALUE, DEPTH };
	enum state_t state = KEY;
	enum state_t prevstate = state;

	Tree_Reset(&this->tree);

	this->node = (ref(Node) *) &this->tree.root;
	this->depth = 0;
	this->line  = 0;

	String buf = HeapString(512);
	String key = HeapString(0);

	size_t whitespaces = 0;

	goto next;

	for (;;) {
		bool popChar = false;

		switch (state) {
			case DEPTH:
				if (c == '#') {
					state = COMMENT;
					prevstate = DEPTH;
					whitespaces = 0;
				} else if (c == ' ') {
					whitespaces++;
				} else if (c == '\t') {
					whitespaces += this->depthWidth;
				} else if (c == '\n') {
					whitespaces = 0;
				} else {
					String_Append(&buf, c);
					state = KEY;
				}

				break;

			case KEY:
				if (c == '#') {
					state = COMMENT;
					prevstate = KEY;
				} else if (buf.len > 0 && buf.buf[buf.len - 1] == ':') {
					if (c == '\n') {
						buf.len--; /* Remove the colon. */
						call(AddSection, whitespaces / this->depthWidth, buf);

						buf.len = 0;
						whitespaces = 0;

						state = DEPTH;
					} else if (c == '#') {
						state = COMMENT;
						prevstate = KEY;
					} else if (c != ' ' && c != '\t') {
						buf.len--; /* Remove the colon. */
						String_Copy(&key, buf);

						buf.len = 0;
						state = VALUE;

						popChar = true;
					}
				} else if (c == '\n') {
					if (buf.len > 0) {
						call(AddItem, whitespaces / this->depthWidth, String(""), buf);
						buf.len = 0;
					}

					whitespaces = 0;
					state = DEPTH;
				} else if (c != ' ' && c != '\t') {
					String_Append(&buf, c);
				}

				break;

			case VALUE:
				if (c == '#') {
					state = COMMENT;
					prevstate = VALUE;
				} else if (c == '\n') {
					String_Trim(&buf);
					call(AddItem, whitespaces / this->depthWidth, key, buf);

					buf.len = 0;
					whitespaces = 0;
					state = DEPTH;
				} else {
					String_Append(&buf, c);
				}

				break;

			case COMMENT:
				if (c == '\n') {
					state = prevstate;
					popChar = true;
				}

				break;
		}

		if (popChar) {
			continue;
		}

		if (this->stream->isEof(this->context)) {
			break;
		}

	next:
		this->stream->read(this->context, &c, 1);

		if (c == '\n') {
			this->line++;
		}
	}

	String_Destroy(&buf);
	String_Destroy(&key);
}
