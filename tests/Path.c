#import <Path.h>

#import "TestSuite.h"

#define self tsPath

class {

};

tsRegister("Path") {
	return true;
}

tsCase(Acute, "Validation") {
	Assert($("Folder path"),  Path_isFolderPath($("/Users/")));
	Assert($("Folder path"), !Path_isFolderPath($("/usr/bin/test")));
	Assert($("Folder path"), !Path_isFolderPath($("/usr/bin/test.exe")));

	Assert($("Creatable folder path"),  Path_isCreatableFolderPath($("/Users/")));
	Assert($("Creatable folder path"),  Path_isCreatableFolderPath($("/Users/./")));
	Assert($("Creatable folder path"), !Path_isCreatableFolderPath($(".")));
	Assert($("Creatable folder path"), !Path_isCreatableFolderPath($("..")));

	/* Legal even though after resolving these will point to "/". */
	Assert($("Creatable folder path"), Path_isCreatableFolderPath($("/./")));
	Assert($("Creatable folder path"), Path_isCreatableFolderPath($("/../")));
	Assert($("Creatable folder path"), Path_isCreatableFolderPath($("/Users/../")));

	Assert($("File path"),  Path_isFilePath($("test")));
	Assert($("File path"),  Path_isFilePath($("test.exe")));
	Assert($("File path"),  Path_isFilePath($("/usr/bin/test.exe")));
	Assert($("File path"), !Path_isFilePath($(".")));
	Assert($("File path"), !Path_isFilePath($("..")));

	Assert($("Creatable file path"),  Path_isCreatableFilePath($("/usr/bin/test")));
	Assert($("Creatable file path"),  Path_isCreatableFilePath($("/usr/bin/test.exe")));
	Assert($("Creatable file path"), !Path_isCreatableFilePath($(".")));
	Assert($("Creatable file path"), !Path_isCreatableFilePath($("..")));
	Assert($("Creatable file path"), !Path_isCreatableFilePath($("/Users/")));
	Assert($("Creatable file path"), !Path_isCreatableFilePath($("/Users/.")));
	Assert($("Creatable file path"), !Path_isCreatableFilePath($("/Users/..")));
}

tsCase(Acute, "Extracting extension") {
	RdString ext = Path_getFileExtension($("/usr/bin/test"));
	Assert($("Equals"), String_Equals(ext, $("")));

	ext = Path_getFileExtension($("/usr/bin/test.exe"));
	Assert($("Equals"), String_Equals(ext, $("exe")));

	ext = Path_getFileExtension($("/usr/bin.2/test"));
	Assert($("Equals"), String_Equals(ext, $("")));
}

tsCase(Acute, "Extracting file name") {
	RdString name = Path_getFileName($("/usr/bin/test.exe"));
	Assert($("Equals"), String_Equals(name, $("test.exe")));

	name = Path_getFileName($("../test.exe"));
	Assert($("Equals"), String_Equals(name, $("test.exe")));

	name = Path_getFileName($("test.exe"));
	Assert($("Equals"), String_Equals(name, $("test.exe")));
}

tsCase(Acute, "Extracting folder name") {
	RdString name = Path_getFolderName($("./"));
	Assert($("Equals"), String_Equals(name, $(".")));

	name = Path_getFolderName($("/usr/bin/"));
	Assert($("Equals"), String_Equals(name, $("bin")));
}

tsCase(Acute, "Extracting folder path") {
	RdString path = Path_getFolderPath($("./"));
	Assert($("Equals"), String_Equals(path, $("./")));

	path = Path_getFolderPath($("test.exe"));
	Assert($("Equals"), String_Equals(path, $("./")));

	path = Path_getFolderPath($("/tmp/"));
	Assert($("Equals"), String_Equals(path, $("/tmp/")));

	path = Path_getFolderPath($("/usr/bin/test.exe"));
	Assert($("Equals"), String_Equals(path, $("/usr/bin/")));
}

tsCase(Acute, "Current path") {
	String path = Path_getCurrent();
	Assert($("Exists"),    Path_exists(path.rd));
	Assert($("Is folder"), Path_isFolder(path.rd));
	String_Destroy(&path);
}

tsCase(Acute, "Meta information") {
	size_t size = 1024 * 250;

	Stat64 meta = Path_getMeta($("./TestSuite.exe"), false);
	Assert($("Size approximation"), meta.size > size);

	Path_createLink($("./TestSuite.exe"), $("./link.exe"));
	meta = Path_getMeta($("./link.exe"), true);
	Assert($("Size approximation"), meta.size > size);

	meta = Path_getMeta($("./link.exe"), false);
	Assert($("Size approximation"), meta.size < size);

	Path_deleteLink($("link.exe"));
}

tsCase(Acute, "File") {
	File file = File_new($("test.dat"), FileStatus_Create | FileStatus_WriteOnly);
	rpt(1024) {
		File_write(&file, '\0');
	}
	File_destroy(&file);

	Assert($("Size"), Path_getSize($("test.dat")) == 1024);

	Path_truncate($("test.dat"), 512);
	Assert($("Size"), Path_getSize($("test.dat")) == 512);

	Path_deleteFile($("test.dat"));
}

tsCase(Acute, "Files") {
	Assert($("Is file"), Path_isFile($("TestSuite.exe"), false));
	Assert($("Is file"), Path_isFile($("TestSuite.exe"), true));
	Assert($("Is file"), Path_isFile($("./TestSuite.exe"), false));
	Assert($("Is file"), Path_isFile($("./TestSuite.exe"), true));

	Assert($("Exists"),  Path_exists($("TestSuite.exe"), false));
	Assert($("Exists"),  Path_exists($("TestSuite.exe"), true));
	Assert($("Exists"), !Path_exists($("temp.exe"), true));
	Assert($("Exists"), !Path_exists($("temp.exe"), true));
	Assert($("Exists"),  Path_exists($("./TestSuite.exe"), false));
	Assert($("Exists"),  Path_exists($("./TestSuite.exe"), true));
	Assert($("Exists"), !Path_exists($("./temp.exe"), true));
	Assert($("Exists"), !Path_exists($("./temp.exe"), true));
}

tsCase(Acute, "Folders") {
	Assert($("Is folder"), Path_isFolder($("/tmp/"), false));
	Assert($("Is folder"), Path_isFolder($("/tmp/"), true));

	Assert($("Exists"),  Path_exists($("/tmp/"), false));
	Assert($("Exists"),  Path_exists($("/tmp/"), true));
	Assert($("Exists"), !Path_exists($("/tmp"),  false));
	Assert($("Exists"), !Path_exists($("/tmp"),  true));
	Assert($("Exists"), !Path_exists($("/invalid/path"), false));
	Assert($("Exists"), !Path_exists($("/invalid/path"), true));
	Assert($("Exists"), !Path_exists($("/invalid/path/"), false));
	Assert($("Exists"), !Path_exists($("/invalid/path/"), true));
}

tsCase(Acute, "Links") {
	Path_createLink($("/tmp/"), $("temp/"));

	Assert($("Exists"), !Path_exists($("temp"),  false));
	Assert($("Exists"), !Path_exists($("temp"),  true));
	Assert($("Exists"),  Path_exists($("temp/"), false));
	Assert($("Exists"),  Path_exists($("temp/"), true));

	Assert($("Is link"), Path_isLink($("temp/")));

	Path_deleteLink($("temp/")); /* The trailing slash is obligatory. */
}

tsCase(Acute, "Links") {
	Path_createLink($("TestSuite.exe"), $("temp.exe"));

	Assert($("Exists"), Path_exists($("temp.exe"), false));
	Assert($("Exists"), Path_exists($("temp.exe"), true));

	Assert($("Is link"), Path_isLink($("temp.exe")));

	Path_deleteLink($("temp.exe"));
}

tsCase(Acute, "Links (invalid)") {
	Path_createLink($("/invalid/path/"), $("temp/"));

	Assert($("Exists"), !Path_exists($("temp"),  false));
	Assert($("Exists"), !Path_exists($("temp"),  true));
	Assert($("Exists"),  Path_exists($("temp/"), false));
	Assert($("Exists"), !Path_exists($("temp/"), true));

	String resolved = Path_followLink($("temp/"));
	Assert($("Equals"), String_Equals(resolved.rd, $("/invalid/path/")));
	String_Destroy(&resolved);

	Path_deleteLink($("temp/"));
}

tsCase(Acute, "Links (invalid)") {
	Path_createLink($("/invalid/path"), $("temp"));

	Assert($("Exists"),  Path_exists($("temp"),  false));
	Assert($("Exists"), !Path_exists($("temp"),  true));
	Assert($("Exists"), !Path_exists($("temp/"), false));
	Assert($("Exists"), !Path_exists($("temp/"), true));

	String resolved = Path_followLink($("temp"));
	Assert($("Equals"), String_Equals(resolved.rd, $("/invalid/path")));
	String_Destroy(&resolved);

	Path_deleteLink($("temp"));
}

tsCase(Acute, "File") {
	File file = File_new($("test.dat"), FileStatus_Create);
	File_destroy(&file);

	Path_createLink($("test.dat"), $("temp"));

	/* This will delete the file the link is pointing to! */
	Path_deleteFile($("temp"));
	Assert($("Exists"), Path_exists($("temp"), false));

	/* Links can only be deleted using deleteLink(). deleteFile() with
	 * `follow' set to false doesn't work on links.
	 */
	Path_deleteLink($("temp"));
}

tsCase(Acute, "Folder creation") {
	Path_createFolder($("a/b/c/"), true);

	Assert($("Exists"),    Path_exists($("a/b/c/")));
	Assert($("Is folder"), Path_isFolder($("a/b/c/")));

	Path_deleteFolder($("a/b/c/"));
	Path_deleteFolder($("a/b/"));
	Path_deleteFolder($("a/"));
}

tsCase(Acute, "Moving folders") {
	Path_createFolder($("a/"), false);
	Path_createFolder($("b/"), false);
	Path_createFolder($("c/"), false);

	Path_moveFolder($("a/"), $("c/a/"));
	Path_moveFolder($("b/"), $("c/b/"));

	Assert($("Exists"), Path_exists($("c/a/")));
	Assert($("Exists"), Path_exists($("c/b/")));

	Path_deleteFolder($("c/a/"));
	Path_deleteFolder($("c/b/"));
	Path_deleteFolder($("c/"));
}

tsCase(Acute, "Moving files") {
	File file = File_new($("test.dat"), FileStatus_Create | FileStatus_WriteOnly);
	File_destroy(&file);

	Path_moveFile($("test.dat"), $("test2.dat"));
	Assert($("Exists"), Path_exists($("test2.dat")));

	Path_deleteFile($("test2.dat"));
}

tsCase(Acute, "Time") {
	Path_createFolder($("test/"));

	Time_UnixEpoch time = {
		.sec  = 1262304000,
		.nsec = 0
	};

	Path_setTime($("test/"), time);

	Time_UnixEpoch time2 = Path_getTime($("test/"));

	Assert($("Equals"),
		time.sec  == time2.sec &&
		time.nsec == time2.nsec);

	Path_deleteFolder($("test/"));
}

tsCase(Acute, "Expanding relative paths") {
	String file = Path_expandFile($("./TestSuite.exe"));
	Assert($("Is absolute"), file.len != 0 && Path_isAbsolutePath(file.rd));
	Assert($("Ending"), String_EndsWith(file.rd, $("/TestSuite.exe")));
	Assert($("Ending"), !String_Contains(file.rd, $("//")));
	Assert($("Exists"), Path_exists(file.rd));
	String_Destroy(&file);

	String folder = Path_expandFolder($("./"));
	Assert($("Is absolute"), folder.len != 0 && Path_isAbsolutePath(folder.rd));
	Assert($("Exists"), Path_exists(folder.rd));
	String_Destroy(&folder);

	folder = Path_expandFolder($("../"));
	Assert($("Is absolute"), folder.len != 0 && Path_isAbsolutePath(folder.rd));
	Assert($("Exists"), Path_exists(folder.rd));
	String_Destroy(&folder);

	folder = Path_expandFolder($("/tmp/"));
	Assert($("Equals"), String_Equals(folder.rd, $("/tmp/")));
	Assert($("Exists"), Path_exists(folder.rd));
	String_Destroy(&folder);

	folder = Path_expandFolder($("/tmp/../"));
	Assert($("Equals"), String_Equals(folder.rd, $("/")));
	Assert($("Exists"), Path_exists(folder.rd));
	String_Destroy(&folder);

	String path = Path_expand($("/tmp/../"));
	Assert($("Equals"), String_Equals(path.rd, $("/")));
	Assert($("Exists"), Path_exists(path.rd));
	String_Destroy(&path);

	path = Path_expand($("./TestSuite.exe"));
	Assert($("Is absolute"), path.len != 0 && Path_isAbsolutePath(path.rd));
	Assert($("Ending"), String_EndsWith(path.rd, $("/TestSuite.exe")));
	Assert($("Exists"), Path_exists(path.rd));
	String_Destroy(&path);
}

tsCase(Acute, "Extended attributes") {
	Path_setExtendedAttribute($("./TestSuite.exe"), $("user.key"), $("value"));

	String value = Path_getExtendedAttribute($("./TestSuite.exe"), $("user.key"));
	Assert($("Equals"), String_Equals(value.rd, $("value")));
	String_Destroy(&value);

	value = String_New(128);
	Path_getExtendedAttribute($("./TestSuite.exe"), $("user.key"), &value);
	Assert($("Equals"), String_Equals(value.rd, $("value")));
	String_Destroy(&value);
}

tsCase(Acute, "Permissions") {
	Assert($("Readable"),   Path_isReadable($("./TestSuite.exe")));
	Assert($("Writable"),   Path_isWritable($("./TestSuite.exe")));
	Assert($("Executable"), Path_isExecutable($("./TestSuite.exe")));

	Assert($("Readable"),   Path_isReadable($("./")));
	Assert($("Writable"),   Path_isWritable($("./")));
	Assert($("Executable"), Path_isExecutable($("./")));

	Assert($("Readable"),    Path_isReadable($("/dev/")));
	Assert($("Writable"),   !Path_isWritable($("/dev/")));
	Assert($("Executable"),  Path_isExecutable($("/dev/")));
}
