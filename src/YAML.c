#include "YAML.h"

static ExceptionManager *exc;

Exception_Define(YAML_IllegalNestingException);

void YAML0(ExceptionManager *e) {
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
		YAML_Section *section = node->p;

		String_Destroy(&section->name);
		Memory_Free(section);
	} else if (node->type == YAML_NodeType_Item) {
		YAML_Item *item = node->p;

		String_Destroy(&item->key);
		String_Destroy(&item->value);

		Memory_Free(item);
	}
}

void YAML_AddSection(YAML *this, size_t depth, String s) {
	if (depth > this->depth) {
		this->node = Tree_AddNode(YAML_Node, (Tree_Node *) this->node);
		this->node->p    = NULL;
		this->node->type = YAML_NodeType_Node;

		this->depth++;
	} else {
		while (this->depth - depth > 0) {
			if (this->node->parent == NULL) {
				throw(exc, &YAML_IllegalNestingException);
			}

			this->node = this->node->parent;
			this->depth--;
		}
	}

	YAML_Section *section = New(YAML_Section);
	section->name = String_Clone(s);

	YAML_Node *node = Tree_AddNode(YAML_Node, (Tree_Node *) this->node);
	node->type = YAML_NodeType_Section;
	node->p    = section;
}

void YAML_AddItem(YAML *this, size_t depth, String key, String value) {
	if (depth > this->depth) {
		this->node = Tree_AddNode(YAML_Node, (Tree_Node *) this->node);
		this->node->p    = NULL;
		this->node->type = YAML_NodeType_Node;

		this->depth++;
	} else {
		while (this->depth - depth > 0) {
			if (this->node->parent == NULL) {
				throw(exc, &YAML_IllegalNestingException);
			}

			this->node = this->node->parent;
			this->depth--;
		}
	}

	YAML_Item *item = New(YAML_Item);
	item->key   = String_Clone(key);
	item->value = String_Clone(value);

	YAML_Node *node = Tree_AddNode(YAML_Node, (Tree_Node *) this->node);
	node->type = YAML_NodeType_Item;
	node->p    = item;
}

void YAML_Parse(YAML *this) {
	char c;

	enum state_t { COMMENT, KEY, VALUE, DEPTH };
	enum state_t state = KEY;
	enum state_t prevstate = state;

	Tree_Reset(&this->tree);

	this->node = (YAML_Node *) &this->tree.root;
	this->depth = 0;

	String buf = HeapString(512);
	String key = HeapString(0);

	size_t whitespaces = 0;

	if (this->stream->read(this->context, &c, 1) == 0) {
		goto out;
	}

	while (true) {
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
				} else if (c != '\n') {
					String_Append(&buf, c);
					state = KEY;
				}

				break;

			case KEY:
				if (c == '#') {
					state = COMMENT;
					prevstate = KEY;
				} else if (buf.len > 0 && buf.buf[buf.len - 1] == ':') {
					/* Skip all whitespaces. */
					if (c == ' ' || c == '\t') {
						break;
					}

					if (c == '\n') {
						buf.len--; /* Remove the colon. */
						YAML_AddSection(this, whitespaces / this->depthWidth, buf);

						buf.len = 0;
						whitespaces = 0;

						state = DEPTH;
					} else if (c == '#') {
						state = COMMENT;
						prevstate = KEY;
					} else {
						buf.len--; /* Remove the colon. */
						String_Copy(&key, buf);

						buf.len = 0;
						state = VALUE;

						popChar = true;
					}
				} else if (c == '\n' && buf.len > 0) {
					YAML_AddItem(this, whitespaces / this->depthWidth, String(""), buf);
					buf.len = 0;
				} else if (c != ' ' && c != '\t' && c != '\n') {
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

		if (this->stream->read(this->context, &c, 1) == 0) {
			break;
		}
	}

out:
	String_Destroy(&buf);
	String_Destroy(&key);
}
