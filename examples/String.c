#import <String.h>

int main(void) {
	/* Define a string on the stack, i.e. there is no need to
	 * call String_Destroy() afterwards (although it is allowed).
	 *
	 * Contrary to a heap-allocated string which requires to be
	 * destroyed:
	 *   String s = String_Clone(String("Allocated on heap."));
	 *   String_Destroy(&s);
	 */
	String s = String("Hello world!");

	/* This loop only serves the purpose of illustrating String's
	 * capabilities. In practice, you'd rather do something like:
	 *   String_Print(s);
	 *   String_Print(String("\n"));
	 */
	for (size_t i = 0; i < s.len; i++) {
		/* Put the current character into a string. */
		String c = StackString(1);
		String_Append(&c, s.buf[i]);

		String fmt = String_Format(
			/* % is a placeholder. */
			String("%%"),

			/* Will be put in place of the first '%'. */
			c,

			/* The last character has the position s.len-1. */
			(i == s.len - 1) ? String("\n") : String(""));

		/* String_Print() and String_Format() do not demand a
		 * pointer because the `String()' macro expands to
		 * a compound literal like: (String) { ... }.
		 */
		String_Print(fmt);

		/* String_Format() returns a heap-allocated string! */
		String_Destroy(&fmt);
	}

	/* Find the needle in the string. */
	ssize_t pos = String_Find(s, String(" world!"));

	/* Always check for String_NotFound (-1), otherwise String_Slice()
	 * will interpret it as the penultimate character of the string!
	 */
	if (pos == String_NotFound) {
		String_Print(String("Needle not found!\n"));
		return ExitStatus_Failure;
	}

	/* Extract and print both parts: */

	/* 0..pos-1 */
	String_Print(String_Slice(s, 0, pos));
	String_Print(String("\n"));

	/* pos..end */
	String_Print(String_Slice(s, pos));
	String_Print(String("\n"));

	return ExitStatus_Success;
}
