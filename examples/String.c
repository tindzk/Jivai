#import <Main.h>
#import <String.h>

#define self Application

def(bool, Run) {
	/* Define a string on the stack, i.e. there is no need to
	 * call String_Destroy() afterwards (although it is allowed).
	 *
	 * Contrary to a heap-allocated string which requires to be
	 * destroyed:
	 *   String s = String_Clone($("Allocated on heap."));
	 *   String_Destroy(&s);
	 */
	RdString s = $("Hello world!");

	/* This loop only serves the purpose of illustrating String's
	 * capabilities. In practice, you'd rather do something like:
	 *   String_Print(s);
	 *   String_Print($("\n"));
	 */
	fwd(i, s.len) {
		/* Put the current character into a string. */
		String c = String_New(1);
		String_Append(&c, s.buf[i]);

		String fmt = String_Format(
			/* % is a placeholder. */
			$("%%"),

			/* Will be put in place of the first '%'. */
			c.rd,

			/* The last character has the position s.len-1. */
			(i == s.len - 1) ? $("\n") : $(""));

		String_Destroy(&c);

		/* String_Print() and String_Format() do not demand a
		 * pointer because the `String()' macro expands to
		 * a compound literal like: (String) { ... }.
		 */
		String_Print(fmt.rd);

		/* String_Format() returns a heap-allocated string! */
		String_Destroy(&fmt);
	}

	/* Find the needle in the string. */
	ssize_t pos = String_Find(s, $(" world!"));

	/* Always check for String_NotFound (-1), otherwise String_Slice()
	 * will interpret it as the penultimate character of the string!
	 */
	if (pos == String_NotFound) {
		String_Print($("Needle not found!\n"));
		return false;
	}

	/* Extract and print both parts: */

	/* 0..pos-1 */
	String_Print(String_Slice(s, 0, pos));
	String_Print($("\n"));

	/* pos..end */
	String_Print(String_Slice(s, pos));
	String_Print($("\n"));

	return true;
}
