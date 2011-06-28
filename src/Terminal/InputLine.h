#import "../String.h"
#import "../Unicode.h"
#import "../Terminal.h"
#import "../Exception.h"

#define self Terminal_InputLine

exc(CommandExceedsAllowedLength)

Callback(ref(OnKeyUp),    void);
Callback(ref(OnKeyDown),  void);
Callback(ref(OnKeyLeft),  void);
Callback(ref(OnKeyRight), void);
Callback(ref(OnKeyBack),  void);
Callback(ref(OnKeyEnter), void, RdString);
Callback(ref(OnKeyPress), bool, RdString);

class {
	size_t pos;
	String line;

	Terminal *term;

	ref(OnKeyUp)    onKeyUp;
	ref(OnKeyDown)  onKeyDown;
	ref(OnKeyLeft)  onKeyLeft;
	ref(OnKeyRight) onKeyRight;
	ref(OnKeyBack)  onKeyBack;
	ref(OnKeyEnter) onKeyEnter;
	ref(OnKeyPress) onKeyPress;
};

rsdef(self, New, Terminal *term);
def(void, Destroy);
overload def(void, ClearLine, bool update);
overload def(void, ClearLine);
def(void, Print, RdString s);
def(void, SetValue, RdString s);
def(void, DeletePreceding);
def(void, DeleteSucceeding);
def(void, MoveRight, size_t n);
def(void, MoveLeft, size_t n);
def(void, Process);

#undef self
