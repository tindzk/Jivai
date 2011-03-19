#import <pty.h>

#import "Bit.h"
#import "File.h"
#import "Macros.h"
#import "BitMask.h"
#import "Integer.h"
#import "Exception.h"

#define self Terminal

// @exc IoctlFailed
// @exc ElementMismatch

/* VT100 escape sequence strings. */
#define Terminal_VT100_Normal              $("\33[0m")
#define Terminal_VT100_Bold                $("\33[1m")
#define Terminal_VT100_Italics             $("\33[3m")
#define Terminal_VT100_Underline           $("\33[4m")
#define Terminal_VT100_Blink               $("\33[5m")

#define Terminal_VT100_Foreground_Black    $("\33[30m")
#define Terminal_VT100_Foreground_Red      $("\33[31m")
#define Terminal_VT100_Foreground_Green    $("\33[32m")
#define Terminal_VT100_Foreground_Yellow   $("\33[33m")
#define Terminal_VT100_Foreground_Blue     $("\33[34m")
#define Terminal_VT100_Foreground_Magenta  $("\33[35m")
#define Terminal_VT100_Foreground_Cyan     $("\33[36m")
#define Terminal_VT100_Foreground_White    $("\33[37m")

#define Terminal_VT100_Background_Black    $("\33[40m")
#define Terminal_VT100_Background_Red      $("\33[41m")
#define Terminal_VT100_Background_Green    $("\33[42m")
#define Terminal_VT100_Background_Yellow   $("\33[43m")
#define Terminal_VT100_Background_Blue     $("\33[44m")
#define Terminal_VT100_Background_Magenta  $("\33[45m")
#define Terminal_VT100_Background_Cyan     $("\33[46m")
#define Terminal_VT100_Background_White    $("\33[47m")

#define Terminal_VT100_Cursor_Hide         $("\33[?25l")
#define Terminal_VT100_Cursor_Show         $("\33[?25h")
#define Terminal_VT100_Cursor_Up           $("\33[A")
#define Terminal_VT100_Cursor_Down         $("\33[B")
#define Terminal_VT100_Cursor_Left         $("\10")
#define Terminal_VT100_Cursor_Right        $("\33[C")
#define Terminal_VT100_Cursor_Save         $("\33[s")
#define Terminal_VT100_Cursor_Restore      $("\33[u")
#define Terminal_VT100_Cursor_Home         $("\33[H")

#define Terminal_VT100_Delete_Line         $("\33[2K")
#define Terminal_VT100_Delete_UntilEol     $("\33[K")

set(ref(Color)) {
	/* Normal text. */
	ref(Color_Normal) = 0,

	/* Foreground colors. */
	ref(Color_ForegroundBlack)   = 0x1,
	ref(Color_ForegroundRed)     = 0x2,
	ref(Color_ForegroundGreen)   = 0x3,
	ref(Color_ForegroundYellow)  = 0x4,
	ref(Color_ForegroundBlue)    = 0x5,
	ref(Color_ForegroundMagenta) = 0x6,
	ref(Color_ForegroundCyan)    = 0x7,
	ref(Color_ForegroundWhite)   = 0x8,
	ref(Color_ForegroundMask)    = 0xF,

	/* Background colors. */
	ref(Color_BackgroundBlack)   = 0x10,
	ref(Color_BackgroundRed)     = 0x20,
	ref(Color_BackgroundGreen)   = 0x30,
	ref(Color_BackgroundYellow)  = 0x40,
	ref(Color_BackgroundBlue)    = 0x50,
	ref(Color_BackgroundMagenta) = 0x60,
	ref(Color_BackgroundCyan)    = 0x70,
	ref(Color_BackgroundWhite)   = 0x80,
	ref(Color_BackgroundMask)    = 0xF0
};

set(ref(Font)) {
	/* Font attributes. */
	ref(Font_Normal)    = 0,
	ref(Font_Bold)      = Bit(0),
	ref(Font_Italics)   = Bit(1),
	ref(Font_Underline) = Bit(2),
	ref(Font_Blink)     = Bit(3)
};

set(ref(KeyType)) {
	ref(KeyType_Up),
	ref(KeyType_Down),
	ref(KeyType_Left),
	ref(KeyType_Right),

	ref(KeyType_Backspace),
	ref(KeyType_Delete),

	ref(KeyType_Home),
	ref(KeyType_End),

	ref(KeyType_Unknown)
};

record(ref(Key)) {
	char c;
	Terminal_KeyType t;
};

record(ref(Size)) {
	unsigned short cols;
	unsigned short rows;
};

record(ref(Style)) {
	int color;
	int font;
};

class {
	File *in;
	File *out;

	bool isVT100;

	struct termios oldTermios;
	struct termios curTermios;

	ref(Style) style;
};

#define CTRLKEY(x) \
	((x) - 96)

rsdef(self, New, bool assumeVT100);
def(void, SetInput, File *file);
def(void, SetOutput, File *file);
def(void, Configure, bool echo, bool signal);
def(void, Destroy);
def(void, ResetVT100);
def(void, SetVT100Color, int color);
def(void, SetVT100Font, int font);
def(ref(Style), GetStyle);
def(void, Restore, ref(Style) style);
sdef(size_t, ResolveColorName, RdString name, bool bg);
sdef(ref(Size), GetSize);
overload def(void, Print, int color, int font, RdString s);
overload def(void, Print, RdString s);
overload def(void, Print, char c);
def(void, PrintFmt, FmtString s);
def(void, FmtArgPrint, RdString fmt, VarArg *argptr);
def(void, FmtPrint, RdString fmt, ...);
def(void, DeleteLine, size_t n);
def(void, DeleteUntilEol);
def(void, MoveHome);
def(void, MoveUp, size_t n);
def(void, MoveDown, size_t n);
def(void, MoveLeft, size_t n);
def(void, MoveRight, size_t n);
def(void, HideCursor);
def(void, ShowCursor);
def(void, SaveCursor);
def(void, RestoreCursor);
def(char, ReadChar);
def(ref(Key), ReadKey);

#define Terminal_Print(obj, ...) \
	Terminal_Print(Terminal_FromObject(obj), ## __VA_ARGS__)

#undef self
