#import "../File.h"
#import "../String.h"
#import "../Unicode.h"
#import "../Terminal.h"
#import "../Exception.h"

#define self Terminal_InputLine

// @exc CommandExceedsAllowedLength

DefineCallback(ref(OnKeyUp),    void);
DefineCallback(ref(OnKeyDown),  void);
DefineCallback(ref(OnKeyLeft),  void);
DefineCallback(ref(OnKeyRight), void);
DefineCallback(ref(OnKeyBack),  void);
DefineCallback(ref(OnKeyEnter), void, ProtString);
DefineCallback(ref(OnKeyPress), bool, ProtString);

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
def(void, Print, ProtString s);
def(void, SetValue, ProtString s);
def(void, DeletePreceding);
def(void, DeleteSucceeding);
def(void, MoveRight, size_t n);
def(void, MoveLeft, size_t n);
def(void, Process);

#undef self
