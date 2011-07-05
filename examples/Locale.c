#import <Main.h>
#import <String.h>
#import <Locale.h>

#define self Application

def(void, print) {
	System_out(t("Hello!\n"));
	System_out(t("Hello \\n World!\n"));
}

def(bool, Run) {
	System_out($("Default:\n"));
	call(print);

	System_out($("\nGerman:\n"));
	Locale_load(Locale_GetInstance(), $("locale/de.lng"));
	call(print);

	/* Messages without translation are returned in its original form. */
	System_out(t("This is an untranslated message.\n"));

	System_out($("\nFrench:\n"));
	Locale_load(Locale_GetInstance(), $("locale/fr.lng"));
	call(print);

	/* Multiple occurrences of the same string are allowed. These are
	 * merged in the ELF binary (only occur once in the rodata section).
	 * Duplicates are automatically omitted by the Locale module, i.e.
	 * it is not necessary to translate such messages twice.
	 */
	System_out(t("This is an untranslated message.\n"));

	return true;
}
