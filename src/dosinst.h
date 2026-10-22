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

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <sys/stat.h>
#അന്തര്ഭാവ <fcntl.h>

#ifndef UNIX_LINT
# include <io.h>
# include <ctype.h>

# include <direct.h>

# include <windows.h>
# include <shlobj.h>
#endif

#ifdef UNIX_LINT
// Running lint on Unix: Some things are missing.
അക്ഷരം *searchpath(അക്ഷരം *name);
#endif

#എങ്കിൽ defined(UNIX_LINT)
# include <unistd.h>
# include <errno.h>
#endif

#അന്തര്ഭാവ "version.h"

#എങ്കിൽ defined(UNIX_LINT)
# define vim_mkdir(x, y) mkdir((അക്ഷരം *)(x), y)
#അല്ലെങ്കിൽ
# define vim_mkdir(x, y) _mkdir((അക്ഷരം *)(x))
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
#ifndef താക്കോൽ_WOW64_64_താക്കോൽ
# define താക്കോൽ_WOW64_64_താക്കോൽ 0x0100
#endif
#ifndef താക്കോൽ_WOW64_32_താക്കോൽ
# define താക്കോൽ_WOW64_32_താക്കോൽ 0x0200
#endif

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#അല്ലെങ്കിൽ
# define UNUSED
#endif

#define VIM_STARTMENU "Programs\\Vim " VIM_VERSION_SHORT

പൂർണ്ണസംഖ്യ	interactive;		// non-zero when running interactively

/*
 * Call malloc() and exit when out of memory.
 */
    സ്ഥിതം ശൂന്യം *
alloc(പൂർണ്ണസംഖ്യ len)
{
    ശൂന്യം *p;

    p = malloc(len);
    എങ്കിൽ (p == ശൂന്യ)
    {
	printf("ERROR: out of memory\n");
	exit(1);
    }
    തിരികെ p;
}

/*
 * The toupper() in Bcc 5.5 doesn't work, use our own implementation.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
mytoupper(പൂർണ്ണസംഖ്യ c)
{
    എങ്കിൽ (c >= 'a' && c <= 'z')
	തിരികെ c - 'a' + 'A';
    തിരികെ c;
}

    സ്ഥിതം ശൂന്യം
myexit(പൂർണ്ണസംഖ്യ n)
{
    എങ്കിൽ (!interactive)
    {
	// Present a prompt, otherwise error messages can't be read.
	printf("Press Enter to continue\n");
	rewind(stdin);
	(ശൂന്യം)getchar();
    }
    exit(n);
}


തരനിർവചനം BOOL (WINAPI *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
/*
 * Check if this is a 64-bit OS.
 */
    സ്ഥിതം BOOL
is_64bit_os(ശൂന്യം)
{
#ifdef _WIN64
    തിരികെ TRUE;
#അല്ലെങ്കിൽ
    BOOL bIsWow64 = FALSE;
    LPFN_ISWOW64PROCESS pIsWow64Process;

    pIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
	    GetModuleHandle("kernel32"), "IsWow64Process");
    എങ്കിൽ (pIsWow64Process != ശൂന്യ)
	pIsWow64Process(GetCurrentProcess(), &bIsWow64);
    തിരികെ bIsWow64;
#endif
}

    സ്ഥിതം അക്ഷരം *
searchpath(അക്ഷരം *name)
{
    സ്ഥിതം അക്ഷരം widename[2 * BUFSIZE];
    സ്ഥിതം അക്ഷരം location[2 * BUFSIZE + 2];

    // There appears to be a bug in FindExecutableA() on Windows NT.
    // Use FindExecutableW() instead...
    MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)name, -1,
	    (LPWSTR)widename, BUFSIZE);
    എങ്കിൽ (FindExecutableW((LPCWSTR)widename, (LPCWSTR)"",
		(LPWSTR)location) > (HINSTANCE)32)
    {
	WideCharToMultiByte(CP_ACP, 0, (LPWSTR)location, -1,
		(LPSTR)widename, 2 * BUFSIZE, ശൂന്യ, ശൂന്യ);
	തിരികെ widename;
    }
    തിരികെ ശൂന്യ;
}

/*
 * Call searchpath() and save the result in allocated memory, or return NULL.
 */
    സ്ഥിതം അക്ഷരം *
searchpath_save(അക്ഷരം *name)
{
    അക്ഷരം	*p;
    അക്ഷരം	*s;

    p = searchpath(name);
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ ശൂന്യ;
    s = alloc(strlen(p) + 1);
    strcpy(s, p);
    തിരികെ s;
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
    പൂർണ്ണസംഖ്യ
get_shell_folder_path(
	അക്ഷരം *shell_folder_path,
	സ്ഥിരം അക്ഷരം *shell_folder_name)
{
    /*
     * The following code was successfully built with make_mvc.mak.
     * The resulting executable worked on Windows 95, Millennium Edition, and
     * 2000 Professional.  But it was changed after testing...
     */
    LPITEMIDLIST    pidl = 0; // Pointer to an Item ID list allocated below
    LPMALLOC	    pMalloc;  // Pointer to an IMalloc interface
    പൂർണ്ണസംഖ്യ		    csidl;
    പൂർണ്ണസംഖ്യ		    alt_csidl = -1;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	    desktop_csidl = -1;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	    programs_csidl = -1;
    പൂർണ്ണസംഖ്യ		    *pcsidl;
    പൂർണ്ണസംഖ്യ		    r;

    എങ്കിൽ (strcmp(shell_folder_name, "desktop") == 0)
    {
	pcsidl = &desktop_csidl;
	csidl = CSIDL_COMMON_DESKTOPDIRECTORY;
	alt_csidl = CSIDL_DESKTOP;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (strncmp(shell_folder_name, "Programs", 8) == 0)
    {
	pcsidl = &programs_csidl;
	csidl = CSIDL_COMMON_PROGRAMS;
	alt_csidl = CSIDL_PROGRAMS;
    }
    അല്ലെങ്കിൽ
    {
	printf("\nERROR (internal) unrecognised shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	തിരികെ FAIL;
    }

    // Did this stuff before, use the same ID again.
    എങ്കിൽ (*pcsidl >= 0)
    {
	csidl = *pcsidl;
	alt_csidl = -1;
    }

retry:
    // Initialize pointer to IMalloc interface
    എങ്കിൽ (NOERROR != SHGetMalloc(&pMalloc))
    {
	printf("\nERROR getting interface for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	തിരികെ FAIL;
    }

    // Get an ITEMIDLIST corresponding to the folder code
    എങ്കിൽ (NOERROR != SHGetSpecialFolderLocation(0, csidl, &pidl))
    {
	എങ്കിൽ (alt_csidl < 0 || NOERROR != SHGetSpecialFolderLocation(0,
							    alt_csidl, &pidl))
	{
	    printf("\nERROR getting ITEMIDLIST for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	    തിരികെ FAIL;
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

    എങ്കിൽ (!r)
    {
	എങ്കിൽ (alt_csidl >= 0)
	{
	    // We probably get here for Windows 95: the "all users"
	    // desktop/start menu entry doesn't exist.
	    csidl = alt_csidl;
	    alt_csidl = -1;
	    പോകുക retry;
	}
	printf("\nERROR translating ITEMIDLIST for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	തിരികെ FAIL;
    }

    // If there is an alternative: verify we can write in this directory.
    // This should cause a retry when the "all users" directory exists but we
    // are a normal user and can't write there.
    എങ്കിൽ (alt_csidl >= 0)
    {
	അക്ഷരം tbuf[BUFSIZE];
	FILE *fd;

	strcpy(tbuf, shell_folder_path);
	strcat(tbuf, "\\vim write test");
	fd = fopen(tbuf, "w");
	എങ്കിൽ (fd == ശൂന്യ)
	{
	    csidl = alt_csidl;
	    alt_csidl = -1;
	    പോകുക retry;
	}
	fclose(fd);
	unlink(tbuf);
    }

    /*
     * Keep the found csidl for next time, so that we don't have to do the
     * write test every time.
     */
    എങ്കിൽ (*pcsidl < 0)
	*pcsidl = csidl;

    എങ്കിൽ (strncmp(shell_folder_name, "Programs\\", 9) == 0)
	strcat(shell_folder_path, shell_folder_name + 8);

    തിരികെ OK;
}

/*
 * List of targets.  The first one (index zero) is used for the default path
 * for the batch files.
 */
#define TARGET_എണ്ണം  9

ഘടന
{
    അക്ഷരം	*name;		// Vim exe name (without .exe)
    അക്ഷരം	*batname;	// batch file name
    അക്ഷരം	*lnkname;	// shortcut file name
    അക്ഷരം	*exename;	// exe file name
    അക്ഷരം	*exenamearg;	// exe file name when using exearg
    അക്ഷരം	*exearg;	// argument for vim.exe or gvim.exe
    അക്ഷരം	*oldbat;	// path to existing xxx.bat or NULL
    അക്ഷരം	*oldexe;	// path to existing xxx.exe or NULL
    അക്ഷരം	batpath[BUFSIZE];  // path of batch file to create; not
				   // created when it's empty
} targets[TARGET_എണ്ണം] =
{
    {"all",	"batch files", ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ""},
    {"vim",	"vim.bat",	"Vim.lnk",
			"vim.exe",    "vim.exe",  "", ശൂന്യ, ശൂന്യ, ""},
    {"gvim",	"gvim.bat",	"gVim.lnk",
			"gvim.exe",   "gvim.exe", "", ശൂന്യ, ശൂന്യ, ""},
    {"evim",	"evim.bat",	"gVim Easy.lnk",
			"evim.exe",   "gvim.exe", "-y", ശൂന്യ, ശൂന്യ, ""},
    {"view",	"view.bat",	"Vim Read-only.lnk",
			"view.exe",   "vim.exe",  "-R", ശൂന്യ, ശൂന്യ, ""},
    {"gview",	"gview.bat",	"gVim Read-only.lnk",
			"gview.exe",  "gvim.exe", "-R", ശൂന്യ, ശൂന്യ, ""},
    {"vimdiff", "vimdiff.bat",	"Vim Diff.lnk",
			"vimdiff.exe","vim.exe",  "-d", ശൂന്യ, ശൂന്യ, ""},
    {"gvimdiff","gvimdiff.bat",	"gVim Diff.lnk",
			"gvimdiff.exe","gvim.exe", "-d", ശൂന്യ, ശൂന്യ, ""},
    {"vimtutor","vimtutor.bat", "Vim tutor.lnk",
			"vimtutor.bat",  "vimtutor.bat", "", ശൂന്യ, ശൂന്യ, ""},
};

/* Uninstall key for vim.bat, etc. */
#define VIMBAT_UNINSTKEY    "rem # uninstall key: " VIM_VERSION_NODOT " #"

#define ICON_എണ്ണം 3
അക്ഷരം *(icon_names[ICON_എണ്ണം]) =
	{"gVim " VIM_VERSION_SHORT,
	 "gVim Easy " VIM_VERSION_SHORT,
	 "gVim Read only " VIM_VERSION_SHORT};
അക്ഷരം *(icon_link_names[ICON_എണ്ണം]) =
	{"gVim " VIM_VERSION_SHORT ".lnk",
	 "gVim Easy " VIM_VERSION_SHORT ".lnk",
	 "gVim Read only " VIM_VERSION_SHORT ".lnk"};

/* This is only used for dosinst.c. */
#എങ്കിൽ defined(DOSINST)
/*
 * Run an external command and wait for it to finish.
 */
    സ്ഥിതം ശൂന്യം
run_command(അക്ഷരം *cmd)
{
    അക്ഷരം	*cmd_path;
    അക്ഷരം	cmd_buf[BUFSIZE * 2 + 35];
    അക്ഷരം	*p;

    // On WinNT, 'start' is a shell built-in for cmd.exe rather than an
    // executable (start.exe) like in Win9x.
    cmd_path = searchpath_save("cmd.exe");
    എങ്കിൽ (cmd_path != ശൂന്യ)
    {
	// There is a cmd.exe, so this might be Windows NT.  If it is,
	// we need to call cmd.exe explicitly.  If it is a later OS,
	// calling cmd.exe won't hurt if it is present.
	// Also, "start" on NT expects a window title argument.
	// Replace the slashes with backslashes.
	ആകുമ്പോൾ ((p = strchr(cmd_path, '/')) != ശൂന്യ)
	    *p = '\\';
	sprintf(cmd_buf, "%s /c start \"vimcmd\" /wait %s", cmd_path, cmd);
	സ്വതന്ത്രം(cmd_path);
    }
    അല്ലെങ്കിൽ
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
    ശൂന്യം
add_pathsep(അക്ഷരം *name)
{
    പൂർണ്ണസംഖ്യ		len = strlen(name);

    എങ്കിൽ (len > 0 && name[len - 1] != '\\' && name[len - 1] != '/')
	strcat(name, "\\");
}

/*
 * The normal chdir() does not change the default drive.  This one does.
 */
    പൂർണ്ണസംഖ്യ
change_drive(പൂർണ്ണസംഖ്യ drive)
{
    അക്ഷരം temp[3] = "-:";
    temp[0] = (അക്ഷരം)(drive + 'A' - 1);
    തിരികെ !SetCurrentDirectory(temp);
}

/*
 * Change directory to "path".
 * Return 0 for success, -1 for failure.
 */
    പൂർണ്ണസംഖ്യ
mch_chdir(അക്ഷരം *path)
{
    എങ്കിൽ (path[0] == NUL)		// just checking...
	തിരികെ 0;
    എങ്കിൽ (path[1] == ':')		// has a drive name
    {
	എങ്കിൽ (change_drive(mytoupper(path[0]) - 'A' + 1))
	    തിരികെ -1;		// invalid drive name
	path += 2;
    }
    എങ്കിൽ (*path == NUL)		// drive name only
	തിരികെ 0;
    തിരികെ chdir(path);		// let the normal chdir() do the rest
}

/*
 * Expand the executable name into a full path name.
 */
    സ്ഥിതം അക്ഷരം *
my_fullpath(അക്ഷരം *buf, അക്ഷരം *fname UNUSED, പൂർണ്ണസംഖ്യ len)
{
    // Only GetModuleFileName() will get the long file name path.
    // GetFullPathName() may still use the short (FAT) name.
    DWORD len_read = GetModuleFileName(ശൂന്യ, buf, (size_t)len);

    തിരികെ (len_read > 0 && len_read < (DWORD)len) ? buf : ശൂന്യ;
}

/*
 * Remove the tail from a file or directory name.
 * Puts a NUL on the last '/' or '\'.
 */
    സ്ഥിതം ശൂന്യം
remove_tail(അക്ഷരം *path)
{
    പൂർണ്ണസംഖ്യ		i;

    വേണ്ടി (i = strlen(path) - 1; i > 0; --i)
	എങ്കിൽ (path[i] == '/' || path[i] == '\\')
	{
	    path[i] = NUL;
	    നിർത്തുക;
	}
}


അക്ഷരം	installdir[MAX_PATH-9];	// top of the installation dir, where the
				// install.exe is located, E.g.:
				// "c:\vim\vim60"
പൂർണ്ണസംഖ്യ	runtimeidx;		// index in installdir[] where "vim60" starts
അക്ഷരം	*sysdrive;		// system drive or "c:\"

/*
 * Setup for using this program.
 * Sets "installdir[]".
 */
    സ്ഥിതം ശൂന്യം
do_inits(അക്ഷരം **argv)
{
    // Find out the full path of our executable.
    എങ്കിൽ (my_fullpath(installdir, argv[0], വലുപ്പം(installdir)) == ശൂന്യ)
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
    എങ്കിൽ (sysdrive == ശൂന്യ || *sysdrive == NUL)
	sysdrive = "C:\\";
}
