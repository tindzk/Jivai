#import "String.h"

/*
 * This is a regular expression library that implements a subset of
 * Perl regular expressions.
 *
 * Please refer to http://slre.sourceforge.net/ for a detailed
 * description.
 *
 * Supported syntax:
 *  ^		Match beginning of a buffer
 *  $		Match end of a buffer
 *  ()		Grouping and substring capturing
 *  [...]	Match any character from set
 *  [^...]	Match any character but ones from set
 *  \s		Match whitespace
 *  \S		Match non-whitespace
 *  \d		Match decimal digit
 *  \r		Match carriage return
 *  \n		Match newline
 *  +		Match one or more times (greedy)
 *  +?		Match one or more times (non-greedy)
 *  *		Match zero or more times (greedy)
 *  *?		Match zero or more times (non-greedy)
 *  ?		Match zero or once
 *  \xDD	Match byte with hex value 0xDD
 *  \meta	Match one of the meta character: ^$().[*+?\
 *
 * Commands and operands are all unsigned chars (1 byte long). All
 * code offsets are relative to the current address, and positive
 * (always point forward). Data offsets are absolute. Commands with
 * operands:
 *
 * Token_Branch offset1 offset2
 *   Try to match the code block that follows the Token_Branch
 *   instruction (code block ends with Token_End). If there is no
 *   match, try to match code block that starts at offset1. If
 *   either of these match, jump to offset2.
 *
 * Token_Exact dataOffset dataLength
 *   Try to match the exact string. The string is recorded in the
 *   data section by storing its offset and length.
 *
 * Token_Open captureNumber
 * Token_Close captureNumber
 *   If the `caps' array Pattern_Match() is passed a non-NULL value,
 *   all matching substrings are stored within it.
 *   Token_Open records the beginning of the substring, whereas
 *   Token_Close sets the length for the respective match.
 *
 * Token_Star         codeOffset
 * Token_Plus         codeOffset
 * Token_QuestionMark codeOffset
 *   *, +, ?, respectively. Try to extract as much as possible from
 *   the given string, until a code block following these
 *   instructions matches. When the longest possible string is
 *   matched, jump to codeOffset.
 *
 * Token_StarQ, Token_PlusQ are non-greedy versions of Token_Star
 * and Token_Plus.
 */

#undef self
#define self Pattern

enum {
	excEmptyPattern = excOffset,
	excNoClosingBracket,
	excUnbalancedBrackets,
	excJumpOffsetTooBig,
	excOffsetOverflow,
	excOffsetUnderflow,
	excUnknownCommand
};

typedef enum {
	Token_End = 0,
	Token_Branch,
	Token_Any,
	Token_Exact,
	Token_AnyOf,
	Token_AnyBut,
	Token_Open,
	Token_Close,
	Token_BOL,
	Token_EOL,
	Token_Star,
	Token_Plus,
	Token_StarQ,
	Token_PlusQ,
	Token_QuestionMark,
	Token_Space,
	Token_NonSpace,
	Token_Digit
} Token;

/*
 * Compiled regular expression
 */
typedef struct {
	Array(unsigned char, *code);
	Array(unsigned char, *data);

	size_t ofs;
	size_t numCaps; /* Number of bracket pairs. */
} Pattern;

void Pattern_Init(Pattern *this);
void Pattern_Destroy(Pattern *this);
void Pattern_Compile(Pattern *this, String pattern);

/*
 * Returns true if there is a match, otherwise false.
 *
 * If the `caps' array is not NULL, then it is filled with the
 * values of the captured substrings. The first element is always
 * the fully matched substring.
 *
 * It is assumed that `caps' contains enough elements to hold all
 * captures.
 */

#define Pattern_Result(...) (String*[]) { __VA_ARGS__ }

bool Pattern_Match(Pattern *this, String s, String **caps);
