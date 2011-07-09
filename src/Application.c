#import "Application.h"

#define self Application

/* Adds all locales from the program's external libraries. */
static sdef(int, onLibrary, struct dl_phdr_info *info, __unused size_t size, void *data) {
	DynInstName(self) $this = (DynInstName(self)) (self *) data;

	RdString path = String_FromNul((char *) info->dlpi_name);
	path.len != 0 || ret(0);

	RdString ext      = Path_getFileExtension(path);
	RdString fileName = Path_getFileName(path);

	Logger_Debug(&this->logger, $("Found external library %."), fileName);

	if (!String_Equals(ext, $("dll"))) {
		Logger_Debug(&this->logger, $("Incompatible library extension."));
		return 0;
	}

	String realPath;
	if (Path_isLink(path)) {
		realPath = Path_followLink(path);
	} else {
		realPath = String_Clone(path);
	}

	RdString folderPath = Path_getFolderPath(realPath.rd);
	RdString context    = String_Slice(fileName, 0, -4);

	String lngpath = String_Format($("%Locale/"), folderPath);

	if (!Path_exists(lngpath.rd)) {
		Logger_Debug(&this->logger, $("External library % has no locales."),
			context);
		String_Destroy(&lngpath);
	} else {
		if (Locale_hasContext(Locale_GetInstance(), context)) {
			Logger_Error(&this->logger, $(
				"The locale context % was already registered. Possibly "
				"the program maintains a local copy of its external locales."),
				context);
		} else {
			Logger_Debug(&this->logger,
				$("Adding locale directory % (for external library %)."),
				lngpath.rd, context);

			Locale_addContext(Locale_GetInstance(), context, lngpath);
		}
	}

	String_Destroy(&realPath);

	return 0;
}

def(void, Init, int argc, char *argv[], char *envp[]) {
	FPU_setExceptions(FPU_Exception_All & ~FPU_Exception_InexactResult);

	size_t envItems = 0;
	for (char **cur = envp; *cur != null; cur++) {
		envItems++;
	}

	this->base   = String_FromNul(argv[0]);
	this->env    = RdStringArray_New(envItems);
	this->args   = RdStringArray_New(argc - 1);
	this->logger = Logger_New(Logger_Printer_For(this, ref(OnLogMessage)));
	this->term   = Terminal_New(false);
	Terminal_SetOutput(&this->term, Channel_StdErr);

	for (int i = 1; i < argc; i++) {
		RdStringArray_Push(&this->args, String_FromNul(argv[i]));
	}

	fwd(i, envItems) {
		RdStringArray_Push(&this->env, String_FromNul(envp[i]));
	}

	Locale_setLanguage(Locale_GetInstance(), System_getLanguage());

	String path = Path_expand(this->base);
	String lngpath = String_Format($("%Locale/"), Path_getFolderPath(path.rd));

	if (!Path_exists(lngpath.rd)) {
		/* Perhaps some external libraries are localised but the
		 * application itself is not.
		 */
		Logger_Debug(&this->logger, $("Program has no language support."));
		String_Destroy(&lngpath);
	} else {
		/* The program's "Locale" sub-folder can include the language
		 * files of its libraries. This can be useful if the application
		 * is statically linked. That way, a self-contained package
		 * can be created while still supporting localisations of its
		 * external libraries.
		 * Just copy the respective locales to Locale/<LibraryName>/.
		 */
		Directory dir = Directory_New(lngpath.rd);

		Directory_Entry entry;
		while (Directory_Read(&dir, &entry)) {
			if (entry.type == Directory_ItemType_Directory) {
				String contextPath = String_Format($("%%/"), lngpath.rd, entry.name);

				Logger_Debug(&this->logger,
					$("Adding internal locale context %."),
					entry.name);

				Locale_addContext(Locale_GetInstance(), entry.name, contextPath);
			}
		}

		Directory_Destroy(&dir);

		Logger_Debug(&this->logger, $("Adding default context."));
		Locale_addContext(Locale_GetInstance(), $("main"), lngpath);
	}

	dl_iterate_phdr(ref(onLibrary), this);

	String_Destroy(&path);
}

def(void, Destroy) {
	RdStringArray_Free(this->args);
	RdStringArray_Free(this->env);
	Terminal_Destroy(&this->term);
}

override def(void, OnLogMessage, FmtString msg, Logger_Level level, RdString file, int line) {
	RdString slevel = Logger_ResolveLevel(level);
	String sline = Integer_ToString(line);

	Terminal_FmtPrint(&this->term,
		$("[%] $ (%:%)\n"),
		slevel, msg, file, sline.rd);

	String_Destroy(&sline);
}

static Memory_Map    map;
static Memory_Libc   libc;
static Memory_Logger logger;
static bool configured = false; /* TODO earlyConstructor doesn't work yet. */

override earlyConstructor void configureMemory(void) {
	if (configured) {
		return;
	}

	if (1 || System_IsRunningOnValgrind()) {
		String_Print($("[Info] Running in Valgrind. Using traditional allocator functions.\n"));
		libc = Memory_Libc_New();
		Memory0(Memory_Libc_AsMemory(&libc));
	} else if (System_IsDebugging()) {
		map    = Memory_Map_New();
		logger = Memory_Logger_New(Memory_Map_AsMemory(&map));

		Memory0(Memory_Logger_AsMemory(&logger));
	} else {
		map = Memory_Map_New();
		Memory0(Memory_Map_AsMemory(&map));
	}

	configured = true;
}
