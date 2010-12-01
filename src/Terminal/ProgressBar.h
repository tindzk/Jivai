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

#define Terminal_ProgressBar_Init(obj, ...) \
	Terminal_ProgressBar_Init(Terminal_ProgressBar_FromObject(obj), ## __VA_ARGS__)

#undef self
