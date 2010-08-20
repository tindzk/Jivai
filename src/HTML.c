#import "HTML.h"

void HTML_Unescape(String *html) {
	if (html->len == 0) {
		return;
	}

	if (html->buf[0] != '"' && html->buf[0] != '\'') {
		return;
	}

	int idx = 0;

	char prev  = '\0';
	char quote = html->buf[0];

	for (size_t i = 1; i < html->len; i++) {
		if (prev == '\\') {
			if (html->buf[i] == quote) {
				html->buf[idx] = html->buf[i];
				idx++;
				i++;
			}
		} else if (prev != '\0') {
			html->buf[idx] = prev;
			idx++;
		}

		prev = html->buf[i];
	}

	html->len = idx;
}
