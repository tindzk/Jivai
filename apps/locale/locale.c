#import <Main.h>
#import <String.h>
#import <Locale.h>

#define self Application

set(ref(Action)) {
	ref(Action_Help),
	ref(Action_Dump),
	ref(Action_Check),
	ref(Action_Create)
};

record(ref(Option)) {
	RdString name;
	ref(Action) action;
	size_t params;
};

static ref(Option) options[] = {
	{ $("dump"),   ref(Action_Dump),   1 },
	{ $("check"),  ref(Action_Check),  2 },
	{ $("create"), ref(Action_Create), 2 }
};

override def(void, OnLogMessage, FmtString msg, Logger_Level level, __unused RdString file, __unused int line) {
	RdString slevel = Logger_ResolveLevel(level);
	Terminal_FmtPrint(&this->term, $("[%] $\n"), slevel, msg);
}

def(void, onDump, RdString msg) {
	String encoded = Locale_encode(msg);
	System_out(encoded.rd);
	String_Destroy(&encoded);

	System_out($("\n"));
}

def(void, onError, Locale_CheckError type, RdString msg) {
	if (type == Locale_CheckError_Missing) {
		System_out($("[Missing] "));
	} else if (type == Locale_CheckError_Invalid) {
		System_out($("[Invalid] "));
	} else if (type == Locale_CheckError_Empty) {
		System_out($("[Empty] "));
	}

	call(onDump, msg);
}

def(bool, Run) {
	ref(Action) action = ref(Action_Help);

	if (this->args->len == 0) {
		Logger_Error(&this->logger, t("No action given."));
	} else {
		RdString strAction = this->args->buf[0];

		fwd(i, nElems(options)) {
			if (String_Equals(strAction, options[i].name)) {
				if (this->args->len - 1 != options[i].params) {
					Logger_Error(&this->logger, t("Parameter mismatch."));
				} else {
					action = options[i].action;
				}

				break;
			}
		}
	}

	if (action == ref(Action_Help)) {
		String fmt = String_Format($(
			"% dump|check|create\n"
			"\n"
			"  dump file.exe|file.dll\n"
			"    Lists all messages occurring in the binary or library. (Duplicates won't be filtered out.)\n"
			"\n"
			"  create file.exe|file.dll locale.lng\n"
			"    Creates the initial language file.\n"
			"\n"
			"  check file.exe|file.dll locale.lng\n"
			"    Validates the language file."),
			this->base);
		System_out(fmt.rd);
		String_Destroy(&fmt);

		System_out($("\n"));
	} else if (action == ref(Action_Dump)) {
		RdString binary = this->args->buf[1];
		Locale_dump(binary, Locale_OnMessage_For(this, ref(onDump)));
	} else if (action == ref(Action_Check)) {
		RdString binary   = this->args->buf[1];
		RdString language = this->args->buf[2];

		Locale_check(binary, language,
			Locale_OnCheckError_For(this, ref(onError)));
	} else if (action == ref(Action_Create)) {
		RdString binary   = this->args->buf[1];
		RdString language = this->args->buf[2];

		Locale_createInitial(binary, language);
	} else {
		assert(false);
	}

	return true;
}
