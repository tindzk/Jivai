#import "YAML.h"

size_t Modules_YAML;

static ExceptionManager *exc;

void YAML0(ExceptionManager *e) {
	Modules_YAML = Module_Register(String("YAML"));

	exc = e;
}

void YAML_Init(YAML *this, size_t depthWidth, StreamInterface *stream, void *context) {
	this->depthWidth = depthWidth;

	this->stream  = stream;
	this->context = context;

	Tree_Init(&this->tree, (void *) &YAML_DestroyNode);
}

void YAML_Destroy(YAML *this) {
	Tree_Destroy(&this->tree);
}

void YAML_DestroyNode(YAML_Node *node) {
	if (node->type == YAML_NodeType_Section) {
		YAML_Section *section = (YAML_Section *) &node->data;

		String_Destroy(&section->name);
	} else if (node->type == YAML_NodeType_Item) {
		YAML_Item *item = (YAML_Item *) &node->data;

		String_Destroy(&item->key);
		String_Destroy(&item->value);
	}
}

YAML_Node* YAML_GetRoot(YAML *this) {
	return (YAML_Node *) &this->tree.root;
}

size_t YAML_GetLine(YAML *this) {
	return this->line;
}

void* YAML_Store(YAML *this, size_t depth, YAML_NodeType type, size_t size) {
	bool storeInSubNode = false;

	if (depth > this->depth) {
		if (this->node->len == 0) {
			this->node = Tree_AddCustomNode(this->node,
				sizeof(YAML_Node) + size);
		} else {
			this->node = Tree_AddCustomNode(
				this->node->nodes[this->node->len - 1],
				sizeof(YAML_Node) + size);
		}

		this->node->type = YAML_NodeType_Node;

		this->depth++;

		storeInSubNode = true;
	} else if (depth < this->depth) {
		while (this->depth - depth > 0) {
			if (this->node->parent == NULL) {
				throw(exc, excIllegalNesting);
			}

			if (this->node->type == YAML_NodeType_Node) {
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
			if (this->node->parent->type == YAML_NodeType_Node) {
				this->node = this->node->parent;
			}
		}
	}

	YAML_Node *node = Tree_AddCustomNode(this->node,
		sizeof(YAML_Node) + size);

	node->type = type;

	if (storeInSubNode) {
		this->node = node;
	}

	return &node->data;
}

void YAML_AddSection(YAML *this, size_t depth, String s) {
	YAML_Section *section = YAML_Store(this, depth, YAML_NodeType_Section, sizeof(YAML_Section));
	section->name = String_Clone(s);
}

void YAML_AddItem(YAML *this, size_t depth, String key, String value) {
	YAML_Item *item = YAML_Store(this, depth, YAML_NodeType_Item, sizeof(YAML_Item));

	item->key   = String_Clone(key);
	item->value = String_Clone(value);
}

void YAML_Parse(YAML *this) {
	char c;

	enum state_t { COMMENT, KEY, VALUE, DEPTH };
	enum state_t state = KEY;
	enum state_t prevstate = state;

	Tree_Reset(&this->tree);

	this->node = (YAML_Node *) &this->tree.root;
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
						YAML_AddSection(this, whitespaces / this->depthWidth, buf);

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
						YAML_AddItem(this, whitespaces / this->depthWidth, String(""), buf);
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
					YAML_AddItem(this, whitespaces / this->depthWidth, key, buf);

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
