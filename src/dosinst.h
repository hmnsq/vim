/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */
/*
 * dosinst.h: Common code for dosinst.c and uninstall.c
 */

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <sys/stat.h>
#অন্তর্ভাব <fcntl.h>

#ifndef UNIX_LINT
# include <io.h>
# include <ctype.h>

# include <direct.h>

# include <windows.h>
# include <shlobj.h>
#endif

#ifdef UNIX_LINT
// Running lint on Unix: Some things are missing.
বর্ণ *searchpath(বর্ণ *name);
#endif

#যদি defined(UNIX_LINT)
# include <unistd.h>
# include <errno.h>
#endif

#অন্তর্ভাব "version.h"

#যদি defined(UNIX_LINT)
# define vim_mkdir(x, y) mkdir((বর্ণ *)(x), y)
#অন্যথা
# define vim_mkdir(x, y) _mkdir((বর্ণ *)(x))
#endif

#define sleep(n) Sleep((n) * 1000)

// ----------------------------------------


#define BUFSIZE (MAX_PATH*2)		// long enough to hold a file name path
#define NUL 0

#define FAIL 0
#define OK 1

#ifndef FALSE
# define FALSE 0
#endif
#ifndef TRUE
# define TRUE 1
#endif

/*
 * Modern way of creating registry entries, also works on 64 bit windows when
 * compiled as a 32 bit program.
 */
#ifndef চাবি_WOW64_64_চাবি
# define চাবি_WOW64_64_চাবি 0x0100
#endif
#ifndef চাবি_WOW64_32_চাবি
# define চাবি_WOW64_32_চাবি 0x0200
#endif

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#অন্যথা
# define UNUSED
#endif

#define VIM_STARTMENU "Programs\\Vim " VIM_VERSION_SHORT

পূর্ণাঙ্ক	interactive;		// non-zero when running interactively

/*
 * Call malloc() and exit when out of memory.
 */
    স্থির শূন্য *
alloc(পূর্ণাঙ্ক len)
{
    শূন্য *p;

    p = malloc(len);
    যদি (p == রিক্ত)
    {
	printf("ERROR: out of memory\n");
	exit(1);
    }
    প্রত্যাগম p;
}

/*
 * The toupper() in Bcc 5.5 doesn't work, use our own implementation.
 */
    স্থির পূর্ণাঙ্ক
mytoupper(পূর্ণাঙ্ক c)
{
    যদি (c >= 'a' && c <= 'z')
	প্রত্যাগম c - 'a' + 'A';
    প্রত্যাগম c;
}

    স্থির শূন্য
myexit(পূর্ণাঙ্ক n)
{
    যদি (!interactive)
    {
	// Present a prompt, otherwise error messages can't be read.
	printf("Press Enter to continue\n");
	rewind(stdin);
	(শূন্য)getchar();
    }
    exit(n);
}


প্রকারনির্দেশ BOOL (WINAPI *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
/*
 * Check if this is a 64-bit OS.
 */
    স্থির BOOL
is_64bit_os(শূন্য)
{
#ifdef _WIN64
    প্রত্যাগম TRUE;
#অন্যথা
    BOOL bIsWow64 = FALSE;
    LPFN_ISWOW64PROCESS pIsWow64Process;

    pIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
	    GetModuleHandle("kernel32"), "IsWow64Process");
    যদি (pIsWow64Process != রিক্ত)
	pIsWow64Process(GetCurrentProcess(), &bIsWow64);
    প্রত্যাগম bIsWow64;
#endif
}

    স্থির বর্ণ *
searchpath(বর্ণ *name)
{
    স্থির বর্ণ widename[2 * BUFSIZE];
    স্থির বর্ণ location[2 * BUFSIZE + 2];

    // There appears to be a bug in FindExecutableA() on Windows NT.
    // Use FindExecutableW() instead...
    MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)name, -1,
	    (LPWSTR)widename, BUFSIZE);
    যদি (FindExecutableW((LPCWSTR)widename, (LPCWSTR)"",
		(LPWSTR)location) > (HINSTANCE)32)
    {
	WideCharToMultiByte(CP_ACP, 0, (LPWSTR)location, -1,
		(LPSTR)widename, 2 * BUFSIZE, রিক্ত, রিক্ত);
	প্রত্যাগম widename;
    }
    প্রত্যাগম রিক্ত;
}

/*
 * Call searchpath() and save the result in allocated memory, or return NULL.
 */
    স্থির বর্ণ *
searchpath_save(বর্ণ *name)
{
    বর্ণ	*p;
    বর্ণ	*s;

    p = searchpath(name);
    যদি (p == রিক্ত)
	প্রত্যাগম রিক্ত;
    s = alloc(strlen(p) + 1);
    strcpy(s, p);
    প্রত্যাগম s;
}


#ifndef CSIDL_COMMON_PROGRAMS
# define CSIDL_COMMON_PROGRAMS 0x0017
#endif
#ifndef CSIDL_COMMON_DESKTOPDIRECTORY
# define CSIDL_COMMON_DESKTOPDIRECTORY 0x0019
#endif

/*
 * Get the path to a requested Windows shell folder.
 *
 * Return FAIL on error, OK on success
 */
    পূর্ণাঙ্ক
get_shell_folder_path(
	বর্ণ *shell_folder_path,
	ধ্রুব বর্ণ *shell_folder_name)
{
    /*
     * The following code was successfully built with make_mvc.mak.
     * The resulting executable worked on Windows 95, Millennium Edition, and
     * 2000 Professional.  But it was changed after testing...
     */
    LPITEMIDLIST    pidl = 0; // Pointer to an Item ID list allocated below
    LPMALLOC	    pMalloc;  // Pointer to an IMalloc interface
    পূর্ণাঙ্ক		    csidl;
    পূর্ণাঙ্ক		    alt_csidl = -1;
    স্থির পূর্ণাঙ্ক	    desktop_csidl = -1;
    স্থির পূর্ণাঙ্ক	    programs_csidl = -1;
    পূর্ণাঙ্ক		    *pcsidl;
    পূর্ণাঙ্ক		    r;

    যদি (strcmp(shell_folder_name, "desktop") == 0)
    {
	pcsidl = &desktop_csidl;
	csidl = CSIDL_COMMON_DESKTOPDIRECTORY;
	alt_csidl = CSIDL_DESKTOP;
    }
    অন্যথা যদি (strncmp(shell_folder_name, "Programs", 8) == 0)
    {
	pcsidl = &programs_csidl;
	csidl = CSIDL_COMMON_PROGRAMS;
	alt_csidl = CSIDL_PROGRAMS;
    }
    অন্যথা
    {
	printf("\nERROR (internal) unrecognised shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	প্রত্যাগম FAIL;
    }

    // Did this stuff before, use the same ID again.
    যদি (*pcsidl >= 0)
    {
	csidl = *pcsidl;
	alt_csidl = -1;
    }

retry:
    // Initialize pointer to IMalloc interface
    যদি (NOERROR != SHGetMalloc(&pMalloc))
    {
	printf("\nERROR getting interface for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	প্রত্যাগম FAIL;
    }

    // Get an ITEMIDLIST corresponding to the folder code
    যদি (NOERROR != SHGetSpecialFolderLocation(0, csidl, &pidl))
    {
	যদি (alt_csidl < 0 || NOERROR != SHGetSpecialFolderLocation(0,
							    alt_csidl, &pidl))
	{
	    printf("\nERROR getting ITEMIDLIST for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	    প্রত্যাগম FAIL;
	}
	csidl = alt_csidl;
	alt_csidl = -1;
    }

    // Translate that ITEMIDLIST to a string
    r = SHGetPathFromIDList(pidl, shell_folder_path);

    // Free the data associated with pidl
    pMalloc->lpVtbl->Free(pMalloc, pidl);
    // Release the IMalloc interface
    pMalloc->lpVtbl->Release(pMalloc);

    যদি (!r)
    {
	যদি (alt_csidl >= 0)
	{
	    // We probably get here for Windows 95: the "all users"
	    // desktop/start menu entry doesn't exist.
	    csidl = alt_csidl;
	    alt_csidl = -1;
	    গচ্ছ retry;
	}
	printf("\nERROR translating ITEMIDLIST for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	প্রত্যাগম FAIL;
    }

    // If there is an alternative: verify we can write in this directory.
    // This should cause a retry when the "all users" directory exists but we
    // are a normal user and can't write there.
    যদি (alt_csidl >= 0)
    {
	বর্ণ tbuf[BUFSIZE];
	FILE *fd;

	strcpy(tbuf, shell_folder_path);
	strcat(tbuf, "\\vim write test");
	fd = fopen(tbuf, "w");
	যদি (fd == রিক্ত)
	{
	    csidl = alt_csidl;
	    alt_csidl = -1;
	    গচ্ছ retry;
	}
	fclose(fd);
	unlink(tbuf);
    }

    /*
     * Keep the found csidl for next time, so that we don't have to do the
     * write test every time.
     */
    যদি (*pcsidl < 0)
	*pcsidl = csidl;

    যদি (strncmp(shell_folder_name, "Programs\\", 9) == 0)
	strcat(shell_folder_path, shell_folder_name + 8);

    প্রত্যাগম OK;
}

/*
 * List of targets.  The first one (index zero) is used for the default path
 * for the batch files.
 */
#define TARGET_গুনতি  9

সংরচনা
{
    বর্ণ	*name;		// Vim exe name (without .exe)
    বর্ণ	*batname;	// batch file name
    বর্ণ	*lnkname;	// shortcut file name
    বর্ণ	*exename;	// exe file name
    বর্ণ	*exenamearg;	// exe file name when using exearg
    বর্ণ	*exearg;	// argument for vim.exe or gvim.exe
    বর্ণ	*oldbat;	// path to existing xxx.bat or NULL
    বর্ণ	*oldexe;	// path to existing xxx.exe or NULL
    বর্ণ	batpath[BUFSIZE];  // path of batch file to create; not
				   // created when it's empty
} targets[TARGET_গুনতি] =
{
    {"all",	"batch files", রিক্ত, রিক্ত, রিক্ত, রিক্ত, রিক্ত, রিক্ত, ""},
    {"vim",	"vim.bat",	"Vim.lnk",
			"vim.exe",    "vim.exe",  "", রিক্ত, রিক্ত, ""},
    {"gvim",	"gvim.bat",	"gVim.lnk",
			"gvim.exe",   "gvim.exe", "", রিক্ত, রিক্ত, ""},
    {"evim",	"evim.bat",	"gVim Easy.lnk",
			"evim.exe",   "gvim.exe", "-y", রিক্ত, রিক্ত, ""},
    {"view",	"view.bat",	"Vim Read-only.lnk",
			"view.exe",   "vim.exe",  "-R", রিক্ত, রিক্ত, ""},
    {"gview",	"gview.bat",	"gVim Read-only.lnk",
			"gview.exe",  "gvim.exe", "-R", রিক্ত, রিক্ত, ""},
    {"vimdiff", "vimdiff.bat",	"Vim Diff.lnk",
			"vimdiff.exe","vim.exe",  "-d", রিক্ত, রিক্ত, ""},
    {"gvimdiff","gvimdiff.bat",	"gVim Diff.lnk",
			"gvimdiff.exe","gvim.exe", "-d", রিক্ত, রিক্ত, ""},
    {"vimtutor","vimtutor.bat", "Vim tutor.lnk",
			"vimtutor.bat",  "vimtutor.bat", "", রিক্ত, রিক্ত, ""},
};

/* Uninstall key for vim.bat, etc. */
#define VIMBAT_UNINSTKEY    "rem # uninstall key: " VIM_VERSION_NODOT " #"

#define ICON_গুনতি 3
বর্ণ *(icon_names[ICON_গুনতি]) =
	{"gVim " VIM_VERSION_SHORT,
	 "gVim Easy " VIM_VERSION_SHORT,
	 "gVim Read only " VIM_VERSION_SHORT};
বর্ণ *(icon_link_names[ICON_গুনতি]) =
	{"gVim " VIM_VERSION_SHORT ".lnk",
	 "gVim Easy " VIM_VERSION_SHORT ".lnk",
	 "gVim Read only " VIM_VERSION_SHORT ".lnk"};

/* This is only used for dosinst.c. */
#যদি defined(DOSINST)
/*
 * Run an external command and wait for it to finish.
 */
    স্থির শূন্য
run_command(বর্ণ *cmd)
{
    বর্ণ	*cmd_path;
    বর্ণ	cmd_buf[BUFSIZE * 2 + 35];
    বর্ণ	*p;

    // On WinNT, 'start' is a shell built-in for cmd.exe rather than an
    // executable (start.exe) like in Win9x.
    cmd_path = searchpath_save("cmd.exe");
    যদি (cmd_path != রিক্ত)
    {
	// There is a cmd.exe, so this might be Windows NT.  If it is,
	// we need to call cmd.exe explicitly.  If it is a later OS,
	// calling cmd.exe won't hurt if it is present.
	// Also, "start" on NT expects a window title argument.
	// Replace the slashes with backslashes.
	যাবত্ ((p = strchr(cmd_path, '/')) != রিক্ত)
	    *p = '\\';
	sprintf(cmd_buf, "%s /c start \"vimcmd\" /wait %s", cmd_path, cmd);
	মুক্ত(cmd_path);
    }
    অন্যথা
    {
	// No cmd.exe, just make the call and let the system handle it.
	sprintf(cmd_buf, "start /w %s", cmd);
    }
    system(cmd_buf);
}
#endif

/*
 * Append a backslash to "name" if there isn't one yet.
 */
    শূন্য
add_pathsep(বর্ণ *name)
{
    পূর্ণাঙ্ক		len = strlen(name);

    যদি (len > 0 && name[len - 1] != '\\' && name[len - 1] != '/')
	strcat(name, "\\");
}

/*
 * The normal chdir() does not change the default drive.  This one does.
 */
    পূর্ণাঙ্ক
change_drive(পূর্ণাঙ্ক drive)
{
    বর্ণ temp[3] = "-:";
    temp[0] = (বর্ণ)(drive + 'A' - 1);
    প্রত্যাগম !SetCurrentDirectory(temp);
}

/*
 * Change directory to "path".
 * Return 0 for success, -1 for failure.
 */
    পূর্ণাঙ্ক
mch_chdir(বর্ণ *path)
{
    যদি (path[0] == NUL)		// just checking...
	প্রত্যাগম 0;
    যদি (path[1] == ':')		// has a drive name
    {
	যদি (change_drive(mytoupper(path[0]) - 'A' + 1))
	    প্রত্যাগম -1;		// invalid drive name
	path += 2;
    }
    যদি (*path == NUL)		// drive name only
	প্রত্যাগম 0;
    প্রত্যাগম chdir(path);		// let the normal chdir() do the rest
}

/*
 * Expand the executable name into a full path name.
 */
    স্থির বর্ণ *
my_fullpath(বর্ণ *buf, বর্ণ *fname UNUSED, পূর্ণাঙ্ক len)
{
    // Only GetModuleFileName() will get the long file name path.
    // GetFullPathName() may still use the short (FAT) name.
    DWORD len_read = GetModuleFileName(রিক্ত, buf, (size_t)len);

    প্রত্যাগম (len_read > 0 && len_read < (DWORD)len) ? buf : রিক্ত;
}

/*
 * Remove the tail from a file or directory name.
 * Puts a NUL on the last '/' or '\'.
 */
    স্থির শূন্য
remove_tail(বর্ণ *path)
{
    পূর্ণাঙ্ক		i;

    প্রতি (i = strlen(path) - 1; i > 0; --i)
	যদি (path[i] == '/' || path[i] == '\\')
	{
	    path[i] = NUL;
	    ভঙ্গ;
	}
}


বর্ণ	installdir[MAX_PATH-9];	// top of the installation dir, where the
				// install.exe is located, E.g.:
				// "c:\vim\vim60"
পূর্ণাঙ্ক	runtimeidx;		// index in installdir[] where "vim60" starts
বর্ণ	*sysdrive;		// system drive or "c:\"

/*
 * Setup for using this program.
 * Sets "installdir[]".
 */
    স্থির শূন্য
do_inits(বর্ণ **argv)
{
    // Find out the full path of our executable.
    যদি (my_fullpath(installdir, argv[0], পরিমাণ(installdir)) == রিক্ত)
    {
	printf("ERROR: Cannot get name of executable\n");
	myexit(1);
    }
    // remove the tail, the executable name "install.exe"
    remove_tail(installdir);

    // change to the installdir
    mch_chdir(installdir);

    // Find the system drive.  Only used for searching the Vim executable, not
    // very important.
    sysdrive = getenv("SYSTEMDRIVE");
    যদি (sysdrive == রিক্ত || *sysdrive == NUL)
	sysdrive = "C:\\";
}
