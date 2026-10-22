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

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>
#ಅನ್ತರ್ಭಾವ <fcntl.h>

#ifndef UNIX_LINT
# include <io.h>
# include <ctype.h>

# include <direct.h>

# include <windows.h>
# include <shlobj.h>
#endif

#ifdef UNIX_LINT
// Running lint on Unix: Some things are missing.
ವರ್ಣ *searchpath(ವರ್ಣ *name);
#endif

#ಯದಿ defined(UNIX_LINT)
# include <unistd.h>
# include <errno.h>
#endif

#ಅನ್ತರ್ಭಾವ "version.h"

#ಯದಿ defined(UNIX_LINT)
# define vim_mkdir(x, y) mkdir((ವರ್ಣ *)(x), y)
#ಅನ್ಯಥಾ
# define vim_mkdir(x, y) _mkdir((ವರ್ಣ *)(x))
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
#ifndef ಕೀಲಿ_WOW64_64_ಕೀಲಿ
# define ಕೀಲಿ_WOW64_64_ಕೀಲಿ 0x0100
#endif
#ifndef ಕೀಲಿ_WOW64_32_ಕೀಲಿ
# define ಕೀಲಿ_WOW64_32_ಕೀಲಿ 0x0200
#endif

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#ಅನ್ಯಥಾ
# define UNUSED
#endif

#define VIM_STARTMENU "Programs\\Vim " VIM_VERSION_SHORT

ಪೂರ್ಣಾಙ್ಕ	interactive;		// non-zero when running interactively

/*
 * Call malloc() and exit when out of memory.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ *
alloc(ಪೂರ್ಣಾಙ್ಕ len)
{
    ಶೂನ್ಯ *p;

    p = malloc(len);
    ಯದಿ (p == ರಿಕ್ತ)
    {
	printf("ERROR: out of memory\n");
	exit(1);
    }
    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * The toupper() in Bcc 5.5 doesn't work, use our own implementation.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
mytoupper(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಯದಿ (c >= 'a' && c <= 'z')
	ಪ್ರತ್ಯಾಗಮ c - 'a' + 'A';
    ಪ್ರತ್ಯಾಗಮ c;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
myexit(ಪೂರ್ಣಾಙ್ಕ n)
{
    ಯದಿ (!interactive)
    {
	// Present a prompt, otherwise error messages can't be read.
	printf("Press Enter to continue\n");
	rewind(stdin);
	(ಶೂನ್ಯ)getchar();
    }
    exit(n);
}


ಪ್ರಕಾರನಿರ್ದೇಶ BOOL (WINAPI *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
/*
 * Check if this is a 64-bit OS.
 */
    ಸ್ಥಿರ BOOL
is_64bit_os(ಶೂನ್ಯ)
{
#ifdef _WIN64
    ಪ್ರತ್ಯಾಗಮ TRUE;
#ಅನ್ಯಥಾ
    BOOL bIsWow64 = FALSE;
    LPFN_ISWOW64PROCESS pIsWow64Process;

    pIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
	    GetModuleHandle("kernel32"), "IsWow64Process");
    ಯದಿ (pIsWow64Process != ರಿಕ್ತ)
	pIsWow64Process(GetCurrentProcess(), &bIsWow64);
    ಪ್ರತ್ಯಾಗಮ bIsWow64;
#endif
}

    ಸ್ಥಿರ ವರ್ಣ *
searchpath(ವರ್ಣ *name)
{
    ಸ್ಥಿರ ವರ್ಣ widename[2 * BUFSIZE];
    ಸ್ಥಿರ ವರ್ಣ location[2 * BUFSIZE + 2];

    // There appears to be a bug in FindExecutableA() on Windows NT.
    // Use FindExecutableW() instead...
    MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)name, -1,
	    (LPWSTR)widename, BUFSIZE);
    ಯದಿ (FindExecutableW((LPCWSTR)widename, (LPCWSTR)"",
		(LPWSTR)location) > (HINSTANCE)32)
    {
	WideCharToMultiByte(CP_ACP, 0, (LPWSTR)location, -1,
		(LPSTR)widename, 2 * BUFSIZE, ರಿಕ್ತ, ರಿಕ್ತ);
	ಪ್ರತ್ಯಾಗಮ widename;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Call searchpath() and save the result in allocated memory, or return NULL.
 */
    ಸ್ಥಿರ ವರ್ಣ *
searchpath_save(ವರ್ಣ *name)
{
    ವರ್ಣ	*p;
    ವರ್ಣ	*s;

    p = searchpath(name);
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    s = alloc(strlen(p) + 1);
    strcpy(s, p);
    ಪ್ರತ್ಯಾಗಮ s;
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
    ಪೂರ್ಣಾಙ್ಕ
get_shell_folder_path(
	ವರ್ಣ *shell_folder_path,
	ಧ್ರುವ ವರ್ಣ *shell_folder_name)
{
    /*
     * The following code was successfully built with make_mvc.mak.
     * The resulting executable worked on Windows 95, Millennium Edition, and
     * 2000 Professional.  But it was changed after testing...
     */
    LPITEMIDLIST    pidl = 0; // Pointer to an Item ID list allocated below
    LPMALLOC	    pMalloc;  // Pointer to an IMalloc interface
    ಪೂರ್ಣಾಙ್ಕ		    csidl;
    ಪೂರ್ಣಾಙ್ಕ		    alt_csidl = -1;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    desktop_csidl = -1;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    programs_csidl = -1;
    ಪೂರ್ಣಾಙ್ಕ		    *pcsidl;
    ಪೂರ್ಣಾಙ್ಕ		    r;

    ಯದಿ (strcmp(shell_folder_name, "desktop") == 0)
    {
	pcsidl = &desktop_csidl;
	csidl = CSIDL_COMMON_DESKTOPDIRECTORY;
	alt_csidl = CSIDL_DESKTOP;
    }
    ಅನ್ಯಥಾ ಯದಿ (strncmp(shell_folder_name, "Programs", 8) == 0)
    {
	pcsidl = &programs_csidl;
	csidl = CSIDL_COMMON_PROGRAMS;
	alt_csidl = CSIDL_PROGRAMS;
    }
    ಅನ್ಯಥಾ
    {
	printf("\nERROR (internal) unrecognised shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // Did this stuff before, use the same ID again.
    ಯದಿ (*pcsidl >= 0)
    {
	csidl = *pcsidl;
	alt_csidl = -1;
    }

retry:
    // Initialize pointer to IMalloc interface
    ಯದಿ (NOERROR != SHGetMalloc(&pMalloc))
    {
	printf("\nERROR getting interface for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // Get an ITEMIDLIST corresponding to the folder code
    ಯದಿ (NOERROR != SHGetSpecialFolderLocation(0, csidl, &pidl))
    {
	ಯದಿ (alt_csidl < 0 || NOERROR != SHGetSpecialFolderLocation(0,
							    alt_csidl, &pidl))
	{
	    printf("\nERROR getting ITEMIDLIST for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
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

    ಯದಿ (!r)
    {
	ಯದಿ (alt_csidl >= 0)
	{
	    // We probably get here for Windows 95: the "all users"
	    // desktop/start menu entry doesn't exist.
	    csidl = alt_csidl;
	    alt_csidl = -1;
	    ಗಚ್ಛ retry;
	}
	printf("\nERROR translating ITEMIDLIST for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // If there is an alternative: verify we can write in this directory.
    // This should cause a retry when the "all users" directory exists but we
    // are a normal user and can't write there.
    ಯದಿ (alt_csidl >= 0)
    {
	ವರ್ಣ tbuf[BUFSIZE];
	FILE *fd;

	strcpy(tbuf, shell_folder_path);
	strcat(tbuf, "\\vim write test");
	fd = fopen(tbuf, "w");
	ಯದಿ (fd == ರಿಕ್ತ)
	{
	    csidl = alt_csidl;
	    alt_csidl = -1;
	    ಗಚ್ಛ retry;
	}
	fclose(fd);
	unlink(tbuf);
    }

    /*
     * Keep the found csidl for next time, so that we don't have to do the
     * write test every time.
     */
    ಯದಿ (*pcsidl < 0)
	*pcsidl = csidl;

    ಯದಿ (strncmp(shell_folder_name, "Programs\\", 9) == 0)
	strcat(shell_folder_path, shell_folder_name + 8);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * List of targets.  The first one (index zero) is used for the default path
 * for the batch files.
 */
#define TARGET_ಎಣಿಕೆ  9

ಸಂರಚನಾ
{
    ವರ್ಣ	*name;		// Vim exe name (without .exe)
    ವರ್ಣ	*batname;	// batch file name
    ವರ್ಣ	*lnkname;	// shortcut file name
    ವರ್ಣ	*exename;	// exe file name
    ವರ್ಣ	*exenamearg;	// exe file name when using exearg
    ವರ್ಣ	*exearg;	// argument for vim.exe or gvim.exe
    ವರ್ಣ	*oldbat;	// path to existing xxx.bat or NULL
    ವರ್ಣ	*oldexe;	// path to existing xxx.exe or NULL
    ವರ್ಣ	batpath[BUFSIZE];  // path of batch file to create; not
				   // created when it's empty
} targets[TARGET_ಎಣಿಕೆ] =
{
    {"all",	"batch files", ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ""},
    {"vim",	"vim.bat",	"Vim.lnk",
			"vim.exe",    "vim.exe",  "", ರಿಕ್ತ, ರಿಕ್ತ, ""},
    {"gvim",	"gvim.bat",	"gVim.lnk",
			"gvim.exe",   "gvim.exe", "", ರಿಕ್ತ, ರಿಕ್ತ, ""},
    {"evim",	"evim.bat",	"gVim Easy.lnk",
			"evim.exe",   "gvim.exe", "-y", ರಿಕ್ತ, ರಿಕ್ತ, ""},
    {"view",	"view.bat",	"Vim Read-only.lnk",
			"view.exe",   "vim.exe",  "-R", ರಿಕ್ತ, ರಿಕ್ತ, ""},
    {"gview",	"gview.bat",	"gVim Read-only.lnk",
			"gview.exe",  "gvim.exe", "-R", ರಿಕ್ತ, ರಿಕ್ತ, ""},
    {"vimdiff", "vimdiff.bat",	"Vim Diff.lnk",
			"vimdiff.exe","vim.exe",  "-d", ರಿಕ್ತ, ರಿಕ್ತ, ""},
    {"gvimdiff","gvimdiff.bat",	"gVim Diff.lnk",
			"gvimdiff.exe","gvim.exe", "-d", ರಿಕ್ತ, ರಿಕ್ತ, ""},
    {"vimtutor","vimtutor.bat", "Vim tutor.lnk",
			"vimtutor.bat",  "vimtutor.bat", "", ರಿಕ್ತ, ರಿಕ್ತ, ""},
};

/* Uninstall key for vim.bat, etc. */
#define VIMBAT_UNINSTKEY    "rem # uninstall key: " VIM_VERSION_NODOT " #"

#define ICON_ಎಣಿಕೆ 3
ವರ್ಣ *(icon_names[ICON_ಎಣಿಕೆ]) =
	{"gVim " VIM_VERSION_SHORT,
	 "gVim Easy " VIM_VERSION_SHORT,
	 "gVim Read only " VIM_VERSION_SHORT};
ವರ್ಣ *(icon_link_names[ICON_ಎಣಿಕೆ]) =
	{"gVim " VIM_VERSION_SHORT ".lnk",
	 "gVim Easy " VIM_VERSION_SHORT ".lnk",
	 "gVim Read only " VIM_VERSION_SHORT ".lnk"};

/* This is only used for dosinst.c. */
#ಯದಿ defined(DOSINST)
/*
 * Run an external command and wait for it to finish.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
run_command(ವರ್ಣ *cmd)
{
    ವರ್ಣ	*cmd_path;
    ವರ್ಣ	cmd_buf[BUFSIZE * 2 + 35];
    ವರ್ಣ	*p;

    // On WinNT, 'start' is a shell built-in for cmd.exe rather than an
    // executable (start.exe) like in Win9x.
    cmd_path = searchpath_save("cmd.exe");
    ಯದಿ (cmd_path != ರಿಕ್ತ)
    {
	// There is a cmd.exe, so this might be Windows NT.  If it is,
	// we need to call cmd.exe explicitly.  If it is a later OS,
	// calling cmd.exe won't hurt if it is present.
	// Also, "start" on NT expects a window title argument.
	// Replace the slashes with backslashes.
	ಯಾವತ್ ((p = strchr(cmd_path, '/')) != ರಿಕ್ತ)
	    *p = '\\';
	sprintf(cmd_buf, "%s /c start \"vimcmd\" /wait %s", cmd_path, cmd);
	ಮುಕ್ತ(cmd_path);
    }
    ಅನ್ಯಥಾ
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
    ಶೂನ್ಯ
add_pathsep(ವರ್ಣ *name)
{
    ಪೂರ್ಣಾಙ್ಕ		len = strlen(name);

    ಯದಿ (len > 0 && name[len - 1] != '\\' && name[len - 1] != '/')
	strcat(name, "\\");
}

/*
 * The normal chdir() does not change the default drive.  This one does.
 */
    ಪೂರ್ಣಾಙ್ಕ
change_drive(ಪೂರ್ಣಾಙ್ಕ drive)
{
    ವರ್ಣ temp[3] = "-:";
    temp[0] = (ವರ್ಣ)(drive + 'A' - 1);
    ಪ್ರತ್ಯಾಗಮ !SetCurrentDirectory(temp);
}

/*
 * Change directory to "path".
 * Return 0 for success, -1 for failure.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_chdir(ವರ್ಣ *path)
{
    ಯದಿ (path[0] == NUL)		// just checking...
	ಪ್ರತ್ಯಾಗಮ 0;
    ಯದಿ (path[1] == ':')		// has a drive name
    {
	ಯದಿ (change_drive(mytoupper(path[0]) - 'A' + 1))
	    ಪ್ರತ್ಯಾಗಮ -1;		// invalid drive name
	path += 2;
    }
    ಯದಿ (*path == NUL)		// drive name only
	ಪ್ರತ್ಯಾಗಮ 0;
    ಪ್ರತ್ಯಾಗಮ chdir(path);		// let the normal chdir() do the rest
}

/*
 * Expand the executable name into a full path name.
 */
    ಸ್ಥಿರ ವರ್ಣ *
my_fullpath(ವರ್ಣ *buf, ವರ್ಣ *fname UNUSED, ಪೂರ್ಣಾಙ್ಕ len)
{
    // Only GetModuleFileName() will get the long file name path.
    // GetFullPathName() may still use the short (FAT) name.
    DWORD len_read = GetModuleFileName(ರಿಕ್ತ, buf, (size_t)len);

    ಪ್ರತ್ಯಾಗಮ (len_read > 0 && len_read < (DWORD)len) ? buf : ರಿಕ್ತ;
}

/*
 * Remove the tail from a file or directory name.
 * Puts a NUL on the last '/' or '\'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
remove_tail(ವರ್ಣ *path)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಪ್ರತಿ (i = strlen(path) - 1; i > 0; --i)
	ಯದಿ (path[i] == '/' || path[i] == '\\')
	{
	    path[i] = NUL;
	    ಭಙ್ಗ;
	}
}


ವರ್ಣ	installdir[MAX_PATH-9];	// top of the installation dir, where the
				// install.exe is located, E.g.:
				// "c:\vim\vim60"
ಪೂರ್ಣಾಙ್ಕ	runtimeidx;		// index in installdir[] where "vim60" starts
ವರ್ಣ	*sysdrive;		// system drive or "c:\"

/*
 * Setup for using this program.
 * Sets "installdir[]".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
do_inits(ವರ್ಣ **argv)
{
    // Find out the full path of our executable.
    ಯದಿ (my_fullpath(installdir, argv[0], ಪರಿಮಾಣ(installdir)) == ರಿಕ್ತ)
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
    ಯದಿ (sysdrive == ರಿಕ್ತ || *sysdrive == NUL)
	sysdrive = "C:\\";
}
