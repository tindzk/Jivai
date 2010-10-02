#import <pty.h>

#import "Bit.h"
#import "File.h"
#import "Macros.h"
#import "Module.h"
#import "UniStd.h" /* isatty */
#import "BitMask.h"
#import "Integer.h"
#import "Exception.h"

#undef self
#define self Terminal

size_t Modules_Terminal;

enum {
	excIoctlFailed = excOffset
};

/* VT100 escape sequence strings. */
#define Terminal_VT100_Normal              String("\33[0m")
#define Terminal_VT100_Bold                String("\33[1m")
#define Terminal_VT100_Italics             String("\33[3m")
#define Terminal_VT100_Underline           String("\33[4m")
#define Terminal_VT100_Blink               String("\33[5m")

#define Terminal_VT100_Foreground_Black    String("\33[30m")
#define Terminal_VT100_Foreground_Red      String("\33[31m")
#define Terminal_VT100_Foreground_Green    String("\33[32m")
#define Terminal_VT100_Foreground_Yellow   String("\33[33m")
#define Terminal_VT100_Foreground_Blue     String("\33[34m")
#define Terminal_VT100_Foreground_Magenta  String("\33[35m")
#define Terminal_VT100_Foreground_Cyan     String("\33[36m")
#define Terminal_VT100_Foreground_White    String("\33[37m")

#define Terminal_VT100_Background_Black    String("\33[40m")
#define Terminal_VT100_Background_Red      String("\33[41m")
#define Terminal_VT100_Background_Green    String("\33[42m")
#define Terminal_VT100_Background_Yellow   String("\33[43m")
#define Terminal_VT100_Background_Blue     String("\33[44m")
#define Terminal_VT100_Background_Magenta  String("\33[45m")
#define Terminal_VT100_Background_Cyan     String("\33[46m")
#define Terminal_VT100_Background_White    String("\33[47m")

#define Terminal_VT100_Cursor_Hide         String("\33[?25l")
#define Terminal_VT100_Cursor_Show         String("\33[?25h")
#define Terminal_VT100_Cursor_Left         String("\10")
#define Terminal_VT100_Cursor_Right        String("\33[C")

#define Terminal_VT100_Delete_Line         String("\33[2K")
#define Terminal_VT100_Delete_UntilEol     String("\33[K")

typedef enum {
	/* Normal text. */
	Terminal_Color_Normal = 0,

	/* Foreground colors. */
	Terminal_Color_ForegroundBlack   = 0x1,
	Terminal_Color_ForegroundRed     = 0x2,
	Terminal_Color_ForegroundGreen   = 0x3,
	Terminal_Color_ForegroundYellow  = 0x4,
	Terminal_Color_ForegroundBlue    = 0x5,
	Terminal_Color_ForegroundMagenta = 0x6,
	Terminal_Color_ForegroundCyan    = 0x7,
	Terminal_Color_ForegroundWhite   = 0x8,
	Terminal_Color_ForegroundMask    = 0xF,

	/* Background colors. */
	Terminal_Color_BackgroundBlack   = 0x10,
	Terminal_Color_BackgroundRed     = 0x20,
	Terminal_Color_BackgroundGreen   = 0x30,
	Terminal_Color_BackgroundYellow  = 0x40,
	Terminal_Color_BackgroundBlue    = 0x50,
	Terminal_Color_BackgroundMagenta = 0x60,
	Terminal_Color_BackgroundCyan    = 0x70,
	Terminal_Color_BackgroundWhite   = 0x80,
	Terminal_Color_BackgroundMask    = 0xF0
} Terminal_Color;

typedef enum {
	/* Font attributes. */
	Terminal_Font_Normal    = 0,
	Terminal_Font_Bold      = Bit(0),
	Terminal_Font_Italics   = Bit(1),
	Terminal_Font_Underline = Bit(2),
	Terminal_Font_Blink     = Bit(3)
} Terminal_Font;

typedef enum {
	Terminal_KeyType_Up,
	Terminal_KeyType_Down,
	Terminal_KeyType_Left,
	Terminal_KeyType_Right,

	Terminal_KeyType_Backspace,
	Terminal_KeyType_Delete,

	Terminal_KeyType_Home,
	Terminal_KeyType_End,

	Terminal_KeyType_Unknown
} Terminal_KeyType;

typedef struct {
	char c;
	Terminal_KeyType t;
} Terminal_Key;

typedef struct {
	int cols;
	int rows;
} Terminal_Size;

typedef struct {
	int color;
	int font;
} Terminal_Style;

typedef struct {
	File *in;
	File *out;

	bool isVT100;

	struct termios oldTermios;
	struct termios curTermios;

	Terminal_Style style;
} Terminal;

#define CTRLKEY(x) \
	((x) - 96)

void Terminal0(ExceptionManager *e);

void Terminal_Init(Terminal *this, File *in, File *out, bool assumeVT100);
void Terminal_Configure(Terminal *this, bool echo, bool signal);
void Terminal_Destroy(Terminal *this);
void Terminal_ResetVT100(Terminal *this);
void Terminal_SetVT100Color(Terminal *this, int color);
void Terminal_SetVT100Font(Terminal *this, int font);
Terminal_Style Terminal_GetStyle(Terminal *this);
void Terminal_Restore(Terminal *this, Terminal_Style style);
size_t Terminal_ResolveColorName(String name, bool bg);
Terminal_Size Terminal_GetSize(void);
void Terminal_Write(Terminal *this, String s);
overload void Terminal_Print(Terminal *this, int color, int font, String s);
overload void Terminal_Print(Terminal *this, String s);
void Terminal_DeleteLine(Terminal *this);
void Terminal_DeleteUntilEol(Terminal *this);
void Terminal_MoveLeft(Terminal *this, size_t n);
void Terminal_MoveRight(Terminal *this, size_t n);
char Terminal_ReadChar(Terminal *this);
void Terminal_HideCursor(Terminal *this);
void Terminal_ShowCursor(Terminal *this);
Terminal_Key Terminal_ReadKey(Terminal *this);
