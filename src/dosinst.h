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

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <sys/stat.h>
#ଅନ୍ତର୍ଭାଵ <fcntl.h>

#ifndef UNIX_LINT
# include <io.h>
# include <ctype.h>

# include <direct.h>

# include <windows.h>
# include <shlobj.h>
#endif

#ifdef UNIX_LINT
// Running lint on Unix: Some things are missing.
ଅକ୍ଷର *searchpath(ଅକ୍ଷର *name);
#endif

#ଯଦି defined(UNIX_LINT)
# include <unistd.h>
# include <errno.h>
#endif

#ଅନ୍ତର୍ଭାଵ "version.h"

#ଯଦି defined(UNIX_LINT)
# define vim_mkdir(x, y) mkdir((ଅକ୍ଷର *)(x), y)
#ଅନ୍ୟଥା
# define vim_mkdir(x, y) _mkdir((ଅକ୍ଷର *)(x))
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
#ifndef ଚାବି_WOW64_64_ଚାବି
# define ଚାବି_WOW64_64_ଚାବି 0x0100
#endif
#ifndef ଚାବି_WOW64_32_ଚାବି
# define ଚାବି_WOW64_32_ଚାବି 0x0200
#endif

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#ଅନ୍ୟଥା
# define UNUSED
#endif

#define VIM_STARTMENU "Programs\\Vim " VIM_VERSION_SHORT

ପୂର୍ଣ୍ଣସଂଖ୍ୟା	interactive;		// non-zero when running interactively

/*
 * Call malloc() and exit when out of memory.
 */
    ସ୍ଥିର ଶୂନ୍ୟ *
alloc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    ଶୂନ୍ୟ *p;

    p = malloc(len);
    ଯଦି (p == ଖାଲି)
    {
	printf("ERROR: out of memory\n");
	exit(1);
    }
    ଫେରନ୍ତୁ p;
}

/*
 * The toupper() in Bcc 5.5 doesn't work, use our own implementation.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mytoupper(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଯଦି (c >= 'a' && c <= 'z')
	ଫେରନ୍ତୁ c - 'a' + 'A';
    ଫେରନ୍ତୁ c;
}

    ସ୍ଥିର ଶୂନ୍ୟ
myexit(ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
    ଯଦି (!interactive)
    {
	// Present a prompt, otherwise error messages can't be read.
	printf("Press Enter to continue\n");
	rewind(stdin);
	(ଶୂନ୍ୟ)getchar();
    }
    exit(n);
}


ପ୍ରକାରବ୍ୟାଖ୍ୟା BOOL (WINAPI *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
/*
 * Check if this is a 64-bit OS.
 */
    ସ୍ଥିର BOOL
is_64bit_os(ଶୂନ୍ୟ)
{
#ifdef _WIN64
    ଫେରନ୍ତୁ TRUE;
#ଅନ୍ୟଥା
    BOOL bIsWow64 = FALSE;
    LPFN_ISWOW64PROCESS pIsWow64Process;

    pIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
	    GetModuleHandle("kernel32"), "IsWow64Process");
    ଯଦି (pIsWow64Process != ଖାଲି)
	pIsWow64Process(GetCurrentProcess(), &bIsWow64);
    ଫେରନ୍ତୁ bIsWow64;
#endif
}

    ସ୍ଥିର ଅକ୍ଷର *
searchpath(ଅକ୍ଷର *name)
{
    ସ୍ଥିର ଅକ୍ଷର widename[2 * BUFSIZE];
    ସ୍ଥିର ଅକ୍ଷର location[2 * BUFSIZE + 2];

    // There appears to be a bug in FindExecutableA() on Windows NT.
    // Use FindExecutableW() instead...
    MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)name, -1,
	    (LPWSTR)widename, BUFSIZE);
    ଯଦି (FindExecutableW((LPCWSTR)widename, (LPCWSTR)"",
		(LPWSTR)location) > (HINSTANCE)32)
    {
	WideCharToMultiByte(CP_ACP, 0, (LPWSTR)location, -1,
		(LPSTR)widename, 2 * BUFSIZE, ଖାଲି, ଖାଲି);
	ଫେରନ୍ତୁ widename;
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Call searchpath() and save the result in allocated memory, or return NULL.
 */
    ସ୍ଥିର ଅକ୍ଷର *
searchpath_save(ଅକ୍ଷର *name)
{
    ଅକ୍ଷର	*p;
    ଅକ୍ଷର	*s;

    p = searchpath(name);
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    s = alloc(strlen(p) + 1);
    strcpy(s, p);
    ଫେରନ୍ତୁ s;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_shell_folder_path(
	ଅକ୍ଷର *shell_folder_path,
	ଅଚଳ ଅକ୍ଷର *shell_folder_name)
{
    /*
     * The following code was successfully built with make_mvc.mak.
     * The resulting executable worked on Windows 95, Millennium Edition, and
     * 2000 Professional.  But it was changed after testing...
     */
    LPITEMIDLIST    pidl = 0; // Pointer to an Item ID list allocated below
    LPMALLOC	    pMalloc;  // Pointer to an IMalloc interface
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    csidl;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    alt_csidl = -1;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    desktop_csidl = -1;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    programs_csidl = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    *pcsidl;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    r;

    ଯଦି (strcmp(shell_folder_name, "desktop") == 0)
    {
	pcsidl = &desktop_csidl;
	csidl = CSIDL_COMMON_DESKTOPDIRECTORY;
	alt_csidl = CSIDL_DESKTOP;
    }
    ଅନ୍ୟଥା ଯଦି (strncmp(shell_folder_name, "Programs", 8) == 0)
    {
	pcsidl = &programs_csidl;
	csidl = CSIDL_COMMON_PROGRAMS;
	alt_csidl = CSIDL_PROGRAMS;
    }
    ଅନ୍ୟଥା
    {
	printf("\nERROR (internal) unrecognised shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	ଫେରନ୍ତୁ FAIL;
    }

    // Did this stuff before, use the same ID again.
    ଯଦି (*pcsidl >= 0)
    {
	csidl = *pcsidl;
	alt_csidl = -1;
    }

retry:
    // Initialize pointer to IMalloc interface
    ଯଦି (NOERROR != SHGetMalloc(&pMalloc))
    {
	printf("\nERROR getting interface for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	ଫେରନ୍ତୁ FAIL;
    }

    // Get an ITEMIDLIST corresponding to the folder code
    ଯଦି (NOERROR != SHGetSpecialFolderLocation(0, csidl, &pidl))
    {
	ଯଦି (alt_csidl < 0 || NOERROR != SHGetSpecialFolderLocation(0,
							    alt_csidl, &pidl))
	{
	    printf("\nERROR getting ITEMIDLIST for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	    ଫେରନ୍ତୁ FAIL;
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

    ଯଦି (!r)
    {
	ଯଦି (alt_csidl >= 0)
	{
	    // We probably get here for Windows 95: the "all users"
	    // desktop/start menu entry doesn't exist.
	    csidl = alt_csidl;
	    alt_csidl = -1;
	    ଯାଅ retry;
	}
	printf("\nERROR translating ITEMIDLIST for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	ଫେରନ୍ତୁ FAIL;
    }

    // If there is an alternative: verify we can write in this directory.
    // This should cause a retry when the "all users" directory exists but we
    // are a normal user and can't write there.
    ଯଦି (alt_csidl >= 0)
    {
	ଅକ୍ଷର tbuf[BUFSIZE];
	FILE *fd;

	strcpy(tbuf, shell_folder_path);
	strcat(tbuf, "\\vim write test");
	fd = fopen(tbuf, "w");
	ଯଦି (fd == ଖାଲି)
	{
	    csidl = alt_csidl;
	    alt_csidl = -1;
	    ଯାଅ retry;
	}
	fclose(fd);
	unlink(tbuf);
    }

    /*
     * Keep the found csidl for next time, so that we don't have to do the
     * write test every time.
     */
    ଯଦି (*pcsidl < 0)
	*pcsidl = csidl;

    ଯଦି (strncmp(shell_folder_name, "Programs\\", 9) == 0)
	strcat(shell_folder_path, shell_folder_name + 8);

    ଫେରନ୍ତୁ OK;
}

/*
 * List of targets.  The first one (index zero) is used for the default path
 * for the batch files.
 */
#define TARGET_ଗଣନା  9

ସଂରଚନା
{
    ଅକ୍ଷର	*name;		// Vim exe name (without .exe)
    ଅକ୍ଷର	*batname;	// batch file name
    ଅକ୍ଷର	*lnkname;	// shortcut file name
    ଅକ୍ଷର	*exename;	// exe file name
    ଅକ୍ଷର	*exenamearg;	// exe file name when using exearg
    ଅକ୍ଷର	*exearg;	// argument for vim.exe or gvim.exe
    ଅକ୍ଷର	*oldbat;	// path to existing xxx.bat or NULL
    ଅକ୍ଷର	*oldexe;	// path to existing xxx.exe or NULL
    ଅକ୍ଷର	batpath[BUFSIZE];  // path of batch file to create; not
				   // created when it's empty
} targets[TARGET_ଗଣନା] =
{
    {"all",	"batch files", ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, ""},
    {"vim",	"vim.bat",	"Vim.lnk",
			"vim.exe",    "vim.exe",  "", ଖାଲି, ଖାଲି, ""},
    {"gvim",	"gvim.bat",	"gVim.lnk",
			"gvim.exe",   "gvim.exe", "", ଖାଲି, ଖାଲି, ""},
    {"evim",	"evim.bat",	"gVim Easy.lnk",
			"evim.exe",   "gvim.exe", "-y", ଖାଲି, ଖାଲି, ""},
    {"view",	"view.bat",	"Vim Read-only.lnk",
			"view.exe",   "vim.exe",  "-R", ଖାଲି, ଖାଲି, ""},
    {"gview",	"gview.bat",	"gVim Read-only.lnk",
			"gview.exe",  "gvim.exe", "-R", ଖାଲି, ଖାଲି, ""},
    {"vimdiff", "vimdiff.bat",	"Vim Diff.lnk",
			"vimdiff.exe","vim.exe",  "-d", ଖାଲି, ଖାଲି, ""},
    {"gvimdiff","gvimdiff.bat",	"gVim Diff.lnk",
			"gvimdiff.exe","gvim.exe", "-d", ଖାଲି, ଖାଲି, ""},
    {"vimtutor","vimtutor.bat", "Vim tutor.lnk",
			"vimtutor.bat",  "vimtutor.bat", "", ଖାଲି, ଖାଲି, ""},
};

/* Uninstall key for vim.bat, etc. */
#define VIMBAT_UNINSTKEY    "rem # uninstall key: " VIM_VERSION_NODOT " #"

#define ICON_ଗଣନା 3
ଅକ୍ଷର *(icon_names[ICON_ଗଣନା]) =
	{"gVim " VIM_VERSION_SHORT,
	 "gVim Easy " VIM_VERSION_SHORT,
	 "gVim Read only " VIM_VERSION_SHORT};
ଅକ୍ଷର *(icon_link_names[ICON_ଗଣନା]) =
	{"gVim " VIM_VERSION_SHORT ".lnk",
	 "gVim Easy " VIM_VERSION_SHORT ".lnk",
	 "gVim Read only " VIM_VERSION_SHORT ".lnk"};

/* This is only used for dosinst.c. */
#ଯଦି defined(DOSINST)
/*
 * Run an external command and wait for it to finish.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
run_command(ଅକ୍ଷର *cmd)
{
    ଅକ୍ଷର	*cmd_path;
    ଅକ୍ଷର	cmd_buf[BUFSIZE * 2 + 35];
    ଅକ୍ଷର	*p;

    // On WinNT, 'start' is a shell built-in for cmd.exe rather than an
    // executable (start.exe) like in Win9x.
    cmd_path = searchpath_save("cmd.exe");
    ଯଦି (cmd_path != ଖାଲି)
    {
	// There is a cmd.exe, so this might be Windows NT.  If it is,
	// we need to call cmd.exe explicitly.  If it is a later OS,
	// calling cmd.exe won't hurt if it is present.
	// Also, "start" on NT expects a window title argument.
	// Replace the slashes with backslashes.
	ଯେତେବେଳେ ((p = strchr(cmd_path, '/')) != ଖାଲି)
	    *p = '\\';
	sprintf(cmd_buf, "%s /c start \"vimcmd\" /wait %s", cmd_path, cmd);
	ମୁକ୍ତ(cmd_path);
    }
    ଅନ୍ୟଥା
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
    ଶୂନ୍ୟ
add_pathsep(ଅକ୍ଷର *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = strlen(name);

    ଯଦି (len > 0 && name[len - 1] != '\\' && name[len - 1] != '/')
	strcat(name, "\\");
}

/*
 * The normal chdir() does not change the default drive.  This one does.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
change_drive(ପୂର୍ଣ୍ଣସଂଖ୍ୟା drive)
{
    ଅକ୍ଷର temp[3] = "-:";
    temp[0] = (ଅକ୍ଷର)(drive + 'A' - 1);
    ଫେରନ୍ତୁ !SetCurrentDirectory(temp);
}

/*
 * Change directory to "path".
 * Return 0 for success, -1 for failure.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_chdir(ଅକ୍ଷର *path)
{
    ଯଦି (path[0] == NUL)		// just checking...
	ଫେରନ୍ତୁ 0;
    ଯଦି (path[1] == ':')		// has a drive name
    {
	ଯଦି (change_drive(mytoupper(path[0]) - 'A' + 1))
	    ଫେରନ୍ତୁ -1;		// invalid drive name
	path += 2;
    }
    ଯଦି (*path == NUL)		// drive name only
	ଫେରନ୍ତୁ 0;
    ଫେରନ୍ତୁ chdir(path);		// let the normal chdir() do the rest
}

/*
 * Expand the executable name into a full path name.
 */
    ସ୍ଥିର ଅକ୍ଷର *
my_fullpath(ଅକ୍ଷର *buf, ଅକ୍ଷର *fname UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    // Only GetModuleFileName() will get the long file name path.
    // GetFullPathName() may still use the short (FAT) name.
    DWORD len_read = GetModuleFileName(ଖାଲି, buf, (size_t)len);

    ଫେରନ୍ତୁ (len_read > 0 && len_read < (DWORD)len) ? buf : ଖାଲି;
}

/*
 * Remove the tail from a file or directory name.
 * Puts a NUL on the last '/' or '\'.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
remove_tail(ଅକ୍ଷର *path)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ପାଇଁ (i = strlen(path) - 1; i > 0; --i)
	ଯଦି (path[i] == '/' || path[i] == '\\')
	{
	    path[i] = NUL;
	    ଭାଙ୍ଗ;
	}
}


ଅକ୍ଷର	installdir[MAX_PATH-9];	// top of the installation dir, where the
				// install.exe is located, E.g.:
				// "c:\vim\vim60"
ପୂର୍ଣ୍ଣସଂଖ୍ୟା	runtimeidx;		// index in installdir[] where "vim60" starts
ଅକ୍ଷର	*sysdrive;		// system drive or "c:\"

/*
 * Setup for using this program.
 * Sets "installdir[]".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
do_inits(ଅକ୍ଷର **argv)
{
    // Find out the full path of our executable.
    ଯଦି (my_fullpath(installdir, argv[0], ଆକାର(installdir)) == ଖାଲି)
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
    ଯଦି (sysdrive == ଖାଲି || *sysdrive == NUL)
	sysdrive = "C:\\";
}
