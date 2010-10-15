#import "String.h"
#import "Controller.h"
#import "../Terminal.h"

#ifndef Terminal_ProgressBar_Padding
#define Terminal_ProgressBar_Padding 8
#endif

typedef struct {
	Terminal *term;
	Terminal_Size size;
	String block;
	String empty;
	size_t width;
	size_t lines;
	Terminal_Controller controller;
} Terminal_ProgressBar;

overload void Terminal_ProgressBar_Init(Terminal_ProgressBar *this, Terminal *term, String block, String empty, size_t width);
overload void Terminal_ProgressBar_Init(Terminal_ProgressBar *this, Terminal *term);
void Terminal_ProgressBar_Clear(Terminal_ProgressBar *this);
void Terminal_ProgressBar_Render(Terminal_ProgressBar *this, size_t percent, String msg);
