#import "ProgressBar.h"

#define self Terminal_ProgressBar

#undef Terminal_ProgressBar_Init

/* Based upon
 * http://nadiana.com/animated-terminal-progress-bar-in-python */

overload def(void, Init, Terminal *term, String block, String empty, size_t width) {
	this->term = term;

	this->size = Terminal_GetSize();

	if (width > 0 && width < this->size.cols - ref(Padding)) {
		this->width = width;
	} else {
		this->width = this->size.cols - ref(Padding);
	}

	this->block = block;
	this->empty = empty;
	this->lines = 0;

	Terminal_Controller_Init(&this->controller, this->term);
}

inline overload def(void, Init, Terminal *term) {
	call(Init, term, $("▊"), $(" "), 0);
}

/* Clear all printed lines. */
def(void, Clear) {
	if (this->lines > 0) {
		Terminal_MoveUp(this->term, this->lines);
		Terminal_DeleteLine(this->term, this->lines);

		this->lines = 0;
	}
}

def(void, Render, size_t percent, String msg) {
	size_t inlineMsgLen = 0;

	if (msg.len > 0) {
		ssize_t offset = String_Find(msg, '\n');
		inlineMsgLen = (offset == String_NotFound) ? msg.len : (size_t) offset;
	}

	size_t barWidth;

	if (inlineMsgLen + this->width + ref(Padding) > this->size.cols) {
		/* The message is too long to fit in one line. Adjust the
		 * bar width to fit.
		 */
		barWidth = this->size.cols - inlineMsgLen - ref(Padding);
	} else {
		barWidth = this->width;
	}

	if (this->lines > 0) {
		call(Clear);
	}

	size_t width     = (barWidth * percent) / 100;
	size_t remaining =  barWidth - width;

	String strPercent = Integer_ToString(percent);

	String progress = String_New(this->block.len * width);
	fwd(i, width) {
		String_Append(&progress, this->block);
	}

	String empty = String_New(this->empty.len * remaining);
	fwd(i, remaining) {
		String_Append(&empty, this->empty);
	}

	Terminal_Controller_Render(&this->controller,
		$("%!% [.b{%%}] %\n"),
		strPercent, progress, empty, msg);

	String_Destroy(&strPercent);
	String_Destroy(&progress);
	String_Destroy(&empty);

	this->lines = 1;
	fwd(i, msg.len) {
		if (msg.buf[i] == '\n') {
			this->lines++;
		}
	}
}
