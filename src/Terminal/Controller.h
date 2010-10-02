#import "../Terminal.h"
#import "../Typography.h"
#import "../StringStream.h"

typedef struct {
	Terminal *term;
} Terminal_Controller;

void Terminal_Controller_Init(Terminal_Controller *this, Terminal *term);
void Terminal_Controller_Render(Terminal_Controller *this, String s, ...);
