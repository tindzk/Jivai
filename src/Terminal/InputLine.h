#import "../File.h"
#import "../String.h"
#import "../Unicode.h"
#import "../Terminal.h"
#import "../Exception.h"

#undef self
#define self Terminal_InputLine

enum {
	excCommandExceedsAllowedLength = excOffset
};

extern size_t Modules_Terminal_InputLine;

typedef void (* Terminal_InputLine_OnKeyUp)(void *);
typedef void (* Terminal_InputLine_OnKeyDown)(void *);
typedef void (* Terminal_InputLine_OnKeyLeft)(void *);
typedef void (* Terminal_InputLine_OnKeyRight)(void *);
typedef void (* Terminal_InputLine_OnKeyBack)(void *);
typedef void (* Terminal_InputLine_OnKeyEnter)(void *, String);
typedef bool (* Terminal_InputLine_OnKeyPress)(void *, String);

typedef struct {
	size_t pos;
	String line;

	Terminal *term;

	void *context;

	Terminal_InputLine_OnKeyUp onKeyUp;
	Terminal_InputLine_OnKeyDown onKeyDown;
	Terminal_InputLine_OnKeyLeft onKeyLeft;
	Terminal_InputLine_OnKeyRight onKeyRight;
	Terminal_InputLine_OnKeyBack onKeyBack;
	Terminal_InputLine_OnKeyEnter onKeyEnter;
	Terminal_InputLine_OnKeyPress onKeyPress;
} Terminal_InputLine;

void Terminal_InputLine0(ExceptionManager *e);

void Terminal_InputLine_Init(Terminal_InputLine *this, Terminal *term);
void Terminal_InputLine_Destroy(Terminal_InputLine *this);
overload void Terminal_InputLine_ClearLine(Terminal_InputLine *this, bool update);
overload void Terminal_InputLine_ClearLine(Terminal_InputLine *this);
void Terminal_InputLine_Print(Terminal_InputLine *this, String s);
void Terminal_InputLine_SetValue(Terminal_InputLine *this, String s);
void Terminal_InputLine_DeletePreceding(Terminal_InputLine *this);
void Terminal_InputLine_DeleteSucceeding(Terminal_InputLine *this);
void Terminal_InputLine_MoveRight(Terminal_InputLine *this, size_t n);
void Terminal_InputLine_MoveLeft(Terminal_InputLine *this, size_t n);
void Terminal_InputLine_Process(Terminal_InputLine *this);
