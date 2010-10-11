#import "Buffer.h"

typedef struct {
	Terminal *term;
	Terminal_Buffer termbuf;
	size_t cur;
} Terminal_Selection;

void Terminal_Selection_Init(Terminal_Selection *this, Terminal *term);
void Terminal_Selection_Destroy(Terminal_Selection *this);
void Terminal_Selection_Add(Terminal_Selection *this, String caption, bool selected);
ssize_t Terminal_Selection_GetSel(Terminal_Selection *this);
