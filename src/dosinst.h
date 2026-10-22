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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifndef UNIX_LINT
# include <io.h>
# include <ctype.h>

# include <direct.h>

# include <windows.h>
# include <shlobj.h>
#endif

#ifdef UNIX_LINT
// Running lint on Unix: Some things are missing.
var_na *searchpath(var_na *name);
#endif

#ถ้า defined(UNIX_LINT)
# include <unistd.h>
# include <errno.h>
#endif

#include "version.h"

#ถ้า defined(UNIX_LINT)
# define vim_mkdir(x, y) mkdir((var_na *)(x), y)
#มิฉะนั้น
# define vim_mkdir(x, y) _mkdir((var_na *)(x))
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
#ifndef KEY_WOW64_64KEY
# define KEY_WOW64_64KEY 0x0100
#endif
#ifndef KEY_WOW64_32KEY
# define KEY_WOW64_32KEY 0x0200
#endif

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#มิฉะนั้น
# define UNUSED
#endif

#define VIM_STARTMENU "Programs\\Vim " VIM_VERSION_SHORT

จำนวนเต็ม	interactive;		// non-zero when running interactively

/*
 * Call malloc() and exit when out of memory.
 */
    static void *
alloc(จำนวนเต็ม len)
{
    void *p;

    p = malloc(len);
    ถ้า (p == NULL)
    {
	printf("ERROR: out of memory\n");
	exit(1);
    }
    คืน p;
}

/*
 * The toupper() in Bcc 5.5 doesn't work, use our own implementation.
 */
    static จำนวนเต็ม
mytoupper(จำนวนเต็ม c)
{
    ถ้า (c >= 'a' && c <= 'z')
	คืน c - 'a' + 'A';
    คืน c;
}

    static void
myexit(จำนวนเต็ม n)
{
    ถ้า (!interactive)
    {
	// Present a prompt, otherwise error messages can't be read.
	printf("Press Enter to continue\n");
	rewind(stdin);
	(void)getchar();
    }
    exit(n);
}


typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
/*
 * Check if this is a 64-bit OS.
 */
    static BOOL
is_64bit_os(void)
{
#ifdef _WIN64
    คืน TRUE;
#มิฉะนั้น
    BOOL bIsWow64 = FALSE;
    LPFN_ISWOW64PROCESS pIsWow64Process;

    pIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
	    GetModuleHandle("kernel32"), "IsWow64Process");
    ถ้า (pIsWow64Process != NULL)
	pIsWow64Process(GetCurrentProcess(), &bIsWow64);
    คืน bIsWow64;
#endif
}

    static var_na *
searchpath(var_na *name)
{
    static var_na widename[2 * BUFSIZE];
    static var_na location[2 * BUFSIZE + 2];

    // There appears to be a bug in FindExecutableA() on Windows NT.
    // Use FindExecutableW() instead...
    MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)name, -1,
	    (LPWSTR)widename, BUFSIZE);
    ถ้า (FindExecutableW((LPCWSTR)widename, (LPCWSTR)"",
		(LPWSTR)location) > (HINSTANCE)32)
    {
	WideCharToMultiByte(CP_ACP, 0, (LPWSTR)location, -1,
		(LPSTR)widename, 2 * BUFSIZE, NULL, NULL);
	คืน widename;
    }
    คืน NULL;
}

/*
 * Call searchpath() and save the result in allocated memory, or return NULL.
 */
    static var_na *
searchpath_save(var_na *name)
{
    var_na	*p;
    var_na	*s;

    p = searchpath(name);
    ถ้า (p == NULL)
	คืน NULL;
    s = alloc(strlen(p) + 1);
    strcpy(s, p);
    คืน s;
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
    จำนวนเต็ม
get_shell_folder_path(
	var_na *shell_folder_path,
	const var_na *shell_folder_name)
{
    /*
     * The following code was successfully built with make_mvc.mak.
     * The resulting executable worked on Windows 95, Millennium Edition, and
     * 2000 Professional.  But it was changed after testing...
     */
    LPITEMIDLIST    pidl = 0; // Pointer to an Item ID list allocated below
    LPMALLOC	    pMalloc;  // Pointer to an IMalloc interface
    จำนวนเต็ม		    csidl;
    จำนวนเต็ม		    alt_csidl = -1;
    static จำนวนเต็ม	    desktop_csidl = -1;
    static จำนวนเต็ม	    programs_csidl = -1;
    จำนวนเต็ม		    *pcsidl;
    จำนวนเต็ม		    r;

    ถ้า (strcmp(shell_folder_name, "desktop") == 0)
    {
	pcsidl = &desktop_csidl;
	csidl = CSIDL_COMMON_DESKTOPDIRECTORY;
	alt_csidl = CSIDL_DESKTOP;
    }
    มิฉะนั้น ถ้า (strncmp(shell_folder_name, "Programs", 8) == 0)
    {
	pcsidl = &programs_csidl;
	csidl = CSIDL_COMMON_PROGRAMS;
	alt_csidl = CSIDL_PROGRAMS;
    }
    มิฉะนั้น
    {
	printf("\nERROR (internal) unrecognised shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	คืน FAIL;
    }

    // Did this stuff before, use the same ID again.
    ถ้า (*pcsidl >= 0)
    {
	csidl = *pcsidl;
	alt_csidl = -1;
    }

retry:
    // Initialize pointer to IMalloc interface
    ถ้า (NOERROR != SHGetMalloc(&pMalloc))
    {
	printf("\nERROR getting interface for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	คืน FAIL;
    }

    // Get an ITEMIDLIST corresponding to the folder code
    ถ้า (NOERROR != SHGetSpecialFolderLocation(0, csidl, &pidl))
    {
	ถ้า (alt_csidl < 0 || NOERROR != SHGetSpecialFolderLocation(0,
							    alt_csidl, &pidl))
	{
	    printf("\nERROR getting ITEMIDLIST for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	    คืน FAIL;
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

    ถ้า (!r)
    {
	ถ้า (alt_csidl >= 0)
	{
	    // We probably get here for Windows 95: the "all users"
	    // desktop/start menu entry doesn't exist.
	    csidl = alt_csidl;
	    alt_csidl = -1;
	    goto retry;
	}
	printf("\nERROR translating ITEMIDLIST for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	คืน FAIL;
    }

    // If there is an alternative: verify we can write in this directory.
    // This should cause a retry when the "all users" directory exists but we
    // are a normal user and can't write there.
    ถ้า (alt_csidl >= 0)
    {
	var_na tbuf[BUFSIZE];
	FILE *fd;

	strcpy(tbuf, shell_folder_path);
	strcat(tbuf, "\\vim write test");
	fd = fopen(tbuf, "w");
	ถ้า (fd == NULL)
	{
	    csidl = alt_csidl;
	    alt_csidl = -1;
	    goto retry;
	}
	fclose(fd);
	unlink(tbuf);
    }

    /*
     * Keep the found csidl for next time, so that we don't have to do the
     * write test every time.
     */
    ถ้า (*pcsidl < 0)
	*pcsidl = csidl;

    ถ้า (strncmp(shell_folder_name, "Programs\\", 9) == 0)
	strcat(shell_folder_path, shell_folder_name + 8);

    คืน OK;
}

/*
 * List of targets.  The first one (index zero) is used for the default path
 * for the batch files.
 */
#define TARGET_COUNT  9

sa_nracanaa
{
    var_na	*name;		// Vim exe name (without .exe)
    var_na	*batname;	// batch file name
    var_na	*lnkname;	// shortcut file name
    var_na	*exename;	// exe file name
    var_na	*exenamearg;	// exe file name when using exearg
    var_na	*exearg;	// argument for vim.exe or gvim.exe
    var_na	*oldbat;	// path to existing xxx.bat or NULL
    var_na	*oldexe;	// path to existing xxx.exe or NULL
    var_na	batpath[BUFSIZE];  // path of batch file to create; not
				   // created when it's empty
} targets[TARGET_COUNT] =
{
    {"all",	"batch files", NULL, NULL, NULL, NULL, NULL, NULL, ""},
    {"vim",	"vim.bat",	"Vim.lnk",
			"vim.exe",    "vim.exe",  "", NULL, NULL, ""},
    {"gvim",	"gvim.bat",	"gVim.lnk",
			"gvim.exe",   "gvim.exe", "", NULL, NULL, ""},
    {"evim",	"evim.bat",	"gVim Easy.lnk",
			"evim.exe",   "gvim.exe", "-y", NULL, NULL, ""},
    {"view",	"view.bat",	"Vim Read-only.lnk",
			"view.exe",   "vim.exe",  "-R", NULL, NULL, ""},
    {"gview",	"gview.bat",	"gVim Read-only.lnk",
			"gview.exe",  "gvim.exe", "-R", NULL, NULL, ""},
    {"vimdiff", "vimdiff.bat",	"Vim Diff.lnk",
			"vimdiff.exe","vim.exe",  "-d", NULL, NULL, ""},
    {"gvimdiff","gvimdiff.bat",	"gVim Diff.lnk",
			"gvimdiff.exe","gvim.exe", "-d", NULL, NULL, ""},
    {"vimtutor","vimtutor.bat", "Vim tutor.lnk",
			"vimtutor.bat",  "vimtutor.bat", "", NULL, NULL, ""},
};

/* Uninstall key for vim.bat, etc. */
#define VIMBAT_UNINSTKEY    "rem # uninstall key: " VIM_VERSION_NODOT " #"

#define ICON_COUNT 3
var_na *(icon_names[ICON_COUNT]) =
	{"gVim " VIM_VERSION_SHORT,
	 "gVim Easy " VIM_VERSION_SHORT,
	 "gVim Read only " VIM_VERSION_SHORT};
var_na *(icon_link_names[ICON_COUNT]) =
	{"gVim " VIM_VERSION_SHORT ".lnk",
	 "gVim Easy " VIM_VERSION_SHORT ".lnk",
	 "gVim Read only " VIM_VERSION_SHORT ".lnk"};

/* This is only used for dosinst.c. */
#ถ้า defined(DOSINST)
/*
 * Run an external command and wait for it to finish.
 */
    static void
run_command(var_na *cmd)
{
    var_na	*cmd_path;
    var_na	cmd_buf[BUFSIZE * 2 + 35];
    var_na	*p;

    // On WinNT, 'start' is a shell built-in for cmd.exe rather than an
    // executable (start.exe) like in Win9x.
    cmd_path = searchpath_save("cmd.exe");
    ถ้า (cmd_path != NULL)
    {
	// There is a cmd.exe, so this might be Windows NT.  If it is,
	// we need to call cmd.exe explicitly.  If it is a later OS,
	// calling cmd.exe won't hurt if it is present.
	// Also, "start" on NT expects a window title argument.
	// Replace the slashes with backslashes.
	ขณะ ((p = strchr(cmd_path, '/')) != NULL)
	    *p = '\\';
	sprintf(cmd_buf, "%s /c start \"vimcmd\" /wait %s", cmd_path, cmd);
	free(cmd_path);
    }
    มิฉะนั้น
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
    void
add_pathsep(var_na *name)
{
    จำนวนเต็ม		len = strlen(name);

    ถ้า (len > 0 && name[len - 1] != '\\' && name[len - 1] != '/')
	strcat(name, "\\");
}

/*
 * The normal chdir() does not change the default drive.  This one does.
 */
    จำนวนเต็ม
change_drive(จำนวนเต็ม drive)
{
    var_na temp[3] = "-:";
    temp[0] = (var_na)(drive + 'A' - 1);
    คืน !SetCurrentDirectory(temp);
}

/*
 * Change directory to "path".
 * Return 0 for success, -1 for failure.
 */
    จำนวนเต็ม
mch_chdir(var_na *path)
{
    ถ้า (path[0] == NUL)		// just checking...
	คืน 0;
    ถ้า (path[1] == ':')		// has a drive name
    {
	ถ้า (change_drive(mytoupper(path[0]) - 'A' + 1))
	    คืน -1;		// invalid drive name
	path += 2;
    }
    ถ้า (*path == NUL)		// drive name only
	คืน 0;
    คืน chdir(path);		// let the normal chdir() do the rest
}

/*
 * Expand the executable name into a full path name.
 */
    static var_na *
my_fullpath(var_na *buf, var_na *fname UNUSED, จำนวนเต็ม len)
{
    // Only GetModuleFileName() will get the long file name path.
    // GetFullPathName() may still use the short (FAT) name.
    DWORD len_read = GetModuleFileName(NULL, buf, (size_t)len);

    คืน (len_read > 0 && len_read < (DWORD)len) ? buf : NULL;
}

/*
 * Remove the tail from a file or directory name.
 * Puts a NUL on the last '/' or '\'.
 */
    static void
remove_tail(var_na *path)
{
    จำนวนเต็ม		i;

    สำหรับ (i = strlen(path) - 1; i > 0; --i)
	ถ้า (path[i] == '/' || path[i] == '\\')
	{
	    path[i] = NUL;
	    bha_nga;
	}
}


var_na	installdir[MAX_PATH-9];	// top of the installation dir, where the
				// install.exe is located, E.g.:
				// "c:\vim\vim60"
จำนวนเต็ม	runtimeidx;		// index in installdir[] where "vim60" starts
var_na	*sysdrive;		// system drive or "c:\"

/*
 * Setup for using this program.
 * Sets "installdir[]".
 */
    static void
do_inits(var_na **argv)
{
    // Find out the full path of our executable.
    ถ้า (my_fullpath(installdir, argv[0], parimaa_na(installdir)) == NULL)
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
    ถ้า (sysdrive == NULL || *sysdrive == NUL)
	sysdrive = "C:\\";
}
