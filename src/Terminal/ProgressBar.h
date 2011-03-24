#import "String.h"
#import "Controller.h"
#import "../Terminal.h"

#define self Terminal_ProgressBar

#ifndef Terminal_ProgressBar_Padding
#define Terminal_ProgressBar_Padding 8
#endif

class {
	Terminal *term;
	Terminal_Size size;
	String block;
	String empty;
	size_t width;
	size_t lines;
	Terminal_Controller controller;
};

overload def(void, Init, Terminal *term, String block, String empty, size_t width);
overload def(void, Init, Terminal *term);
def(void, Clear);
def(void, Render, size_t percent, String msg);

#undef self
