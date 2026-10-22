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

#شامل <stdio.h>
#شامل <stdlib.h>
#شامل <string.h>
#شامل <sys/stat.h>
#شامل <fcntl.h>

#ifndef UNIX_LINT
# include <io.h>
# include <ctype.h>

# include <direct.h>

# include <windows.h>
# include <shlobj.h>
#endif

#ifdef UNIX_LINT
// Running lint on Unix: Some things are missing.
نویسه *searchpath(نویسه *name);
#endif

#اگر defined(UNIX_LINT)
# include <unistd.h>
# include <errno.h>
#endif

#شامل "version.h"

#اگر defined(UNIX_LINT)
# define vim_mkdir(x, y) mkdir((نویسه *)(x), y)
#وگرنه
# define vim_mkdir(x, y) _mkdir((نویسه *)(x))
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
#ifndef کلید_WOW64_64_کلید
# define کلید_WOW64_64_کلید 0x0100
#endif
#ifndef کلید_WOW64_32_کلید
# define کلید_WOW64_32_کلید 0x0200
#endif

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#وگرنه
# define UNUSED
#endif

#define VIM_STARTMENU "Programs\\Vim " VIM_VERSION_SHORT

صحیح	interactive;		// non-zero when running interactively

/*
 * Call malloc() and exit when out of memory.
 */
    ایستا تهی *
alloc(صحیح len)
{
    تهی *p;

    p = malloc(len);
    اگر (p == پوچ)
    {
	printf("ERROR: out of memory\n");
	exit(1);
    }
    بازگشت p;
}

/*
 * The toupper() in Bcc 5.5 doesn't work, use our own implementation.
 */
    ایستا صحیح
mytoupper(صحیح c)
{
    اگر (c >= 'a' && c <= 'z')
	بازگشت c - 'a' + 'A';
    بازگشت c;
}

    ایستا تهی
myexit(صحیح n)
{
    اگر (!interactive)
    {
	// Present a prompt, otherwise error messages can't be read.
	printf("Press Enter to continue\n");
	rewind(stdin);
	(تهی)getchar();
    }
    exit(n);
}


تعریفنوع BOOL (WINAPI *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
/*
 * Check if this is a 64-bit OS.
 */
    ایستا BOOL
is_64bit_os(تهی)
{
#ifdef _WIN64
    بازگشت TRUE;
#وگرنه
    BOOL bIsWow64 = FALSE;
    LPFN_ISWOW64PROCESS pIsWow64Process;

    pIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
	    GetModuleHandle("kernel32"), "IsWow64Process");
    اگر (pIsWow64Process != پوچ)
	pIsWow64Process(GetCurrentProcess(), &bIsWow64);
    بازگشت bIsWow64;
#endif
}

    ایستا نویسه *
searchpath(نویسه *name)
{
    ایستا نویسه widename[2 * BUFSIZE];
    ایستا نویسه location[2 * BUFSIZE + 2];

    // There appears to be a bug in FindExecutableA() on Windows NT.
    // Use FindExecutableW() instead...
    MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)name, -1,
	    (LPWSTR)widename, BUFSIZE);
    اگر (FindExecutableW((LPCWSTR)widename, (LPCWSTR)"",
		(LPWSTR)location) > (HINSTANCE)32)
    {
	WideCharToMultiByte(CP_ACP, 0, (LPWSTR)location, -1,
		(LPSTR)widename, 2 * BUFSIZE, پوچ, پوچ);
	بازگشت widename;
    }
    بازگشت پوچ;
}

/*
 * Call searchpath() and save the result in allocated memory, or return NULL.
 */
    ایستا نویسه *
searchpath_save(نویسه *name)
{
    نویسه	*p;
    نویسه	*s;

    p = searchpath(name);
    اگر (p == پوچ)
	بازگشت پوچ;
    s = alloc(strlen(p) + 1);
    strcpy(s, p);
    بازگشت s;
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
    صحیح
get_shell_folder_path(
	نویسه *shell_folder_path,
	ثابت نویسه *shell_folder_name)
{
    /*
     * The following code was successfully built with make_mvc.mak.
     * The resulting executable worked on Windows 95, Millennium Edition, and
     * 2000 Professional.  But it was changed after testing...
     */
    LPITEMIDLIST    pidl = 0; // Pointer to an Item ID list allocated below
    LPMALLOC	    pMalloc;  // Pointer to an IMalloc interface
    صحیح		    csidl;
    صحیح		    alt_csidl = -1;
    ایستا صحیح	    desktop_csidl = -1;
    ایستا صحیح	    programs_csidl = -1;
    صحیح		    *pcsidl;
    صحیح		    r;

    اگر (strcmp(shell_folder_name, "desktop") == 0)
    {
	pcsidl = &desktop_csidl;
	csidl = CSIDL_COMMON_DESKTOPDIRECTORY;
	alt_csidl = CSIDL_DESKTOP;
    }
    وگرنه اگر (strncmp(shell_folder_name, "Programs", 8) == 0)
    {
	pcsidl = &programs_csidl;
	csidl = CSIDL_COMMON_PROGRAMS;
	alt_csidl = CSIDL_PROGRAMS;
    }
    وگرنه
    {
	printf("\nERROR (internal) unrecognised shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	بازگشت FAIL;
    }

    // Did this stuff before, use the same ID again.
    اگر (*pcsidl >= 0)
    {
	csidl = *pcsidl;
	alt_csidl = -1;
    }

retry:
    // Initialize pointer to IMalloc interface
    اگر (NOERROR != SHGetMalloc(&pMalloc))
    {
	printf("\nERROR getting interface for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	بازگشت FAIL;
    }

    // Get an ITEMIDLIST corresponding to the folder code
    اگر (NOERROR != SHGetSpecialFolderLocation(0, csidl, &pidl))
    {
	اگر (alt_csidl < 0 || NOERROR != SHGetSpecialFolderLocation(0,
							    alt_csidl, &pidl))
	{
	    printf("\nERROR getting ITEMIDLIST for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	    بازگشت FAIL;
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

    اگر (!r)
    {
	اگر (alt_csidl >= 0)
	{
	    // We probably get here for Windows 95: the "all users"
	    // desktop/start menu entry doesn't exist.
	    csidl = alt_csidl;
	    alt_csidl = -1;
	    برو retry;
	}
	printf("\nERROR translating ITEMIDLIST for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	بازگشت FAIL;
    }

    // If there is an alternative: verify we can write in this directory.
    // This should cause a retry when the "all users" directory exists but we
    // are a normal user and can't write there.
    اگر (alt_csidl >= 0)
    {
	نویسه tbuf[BUFSIZE];
	FILE *fd;

	strcpy(tbuf, shell_folder_path);
	strcat(tbuf, "\\vim write test");
	fd = fopen(tbuf, "w");
	اگر (fd == پوچ)
	{
	    csidl = alt_csidl;
	    alt_csidl = -1;
	    برو retry;
	}
	fclose(fd);
	unlink(tbuf);
    }

    /*
     * Keep the found csidl for next time, so that we don't have to do the
     * write test every time.
     */
    اگر (*pcsidl < 0)
	*pcsidl = csidl;

    اگر (strncmp(shell_folder_name, "Programs\\", 9) == 0)
	strcat(shell_folder_path, shell_folder_name + 8);

    بازگشت OK;
}

/*
 * List of targets.  The first one (index zero) is used for the default path
 * for the batch files.
 */
#define TARGET_شمار  9

ساختار
{
    نویسه	*name;		// Vim exe name (without .exe)
    نویسه	*batname;	// batch file name
    نویسه	*lnkname;	// shortcut file name
    نویسه	*exename;	// exe file name
    نویسه	*exenamearg;	// exe file name when using exearg
    نویسه	*exearg;	// argument for vim.exe or gvim.exe
    نویسه	*oldbat;	// path to existing xxx.bat or NULL
    نویسه	*oldexe;	// path to existing xxx.exe or NULL
    نویسه	batpath[BUFSIZE];  // path of batch file to create; not
				   // created when it's empty
} targets[TARGET_شمار] =
{
    {"all",	"batch files", پوچ, پوچ, پوچ, پوچ, پوچ, پوچ, ""},
    {"vim",	"vim.bat",	"Vim.lnk",
			"vim.exe",    "vim.exe",  "", پوچ, پوچ, ""},
    {"gvim",	"gvim.bat",	"gVim.lnk",
			"gvim.exe",   "gvim.exe", "", پوچ, پوچ, ""},
    {"evim",	"evim.bat",	"gVim Easy.lnk",
			"evim.exe",   "gvim.exe", "-y", پوچ, پوچ, ""},
    {"view",	"view.bat",	"Vim Read-only.lnk",
			"view.exe",   "vim.exe",  "-R", پوچ, پوچ, ""},
    {"gview",	"gview.bat",	"gVim Read-only.lnk",
			"gview.exe",  "gvim.exe", "-R", پوچ, پوچ, ""},
    {"vimdiff", "vimdiff.bat",	"Vim Diff.lnk",
			"vimdiff.exe","vim.exe",  "-d", پوچ, پوچ, ""},
    {"gvimdiff","gvimdiff.bat",	"gVim Diff.lnk",
			"gvimdiff.exe","gvim.exe", "-d", پوچ, پوچ, ""},
    {"vimtutor","vimtutor.bat", "Vim tutor.lnk",
			"vimtutor.bat",  "vimtutor.bat", "", پوچ, پوچ, ""},
};

/* Uninstall key for vim.bat, etc. */
#define VIMBAT_UNINSTKEY    "rem # uninstall key: " VIM_VERSION_NODOT " #"

#define ICON_شمار 3
نویسه *(icon_names[ICON_شمار]) =
	{"gVim " VIM_VERSION_SHORT,
	 "gVim Easy " VIM_VERSION_SHORT,
	 "gVim Read only " VIM_VERSION_SHORT};
نویسه *(icon_link_names[ICON_شمار]) =
	{"gVim " VIM_VERSION_SHORT ".lnk",
	 "gVim Easy " VIM_VERSION_SHORT ".lnk",
	 "gVim Read only " VIM_VERSION_SHORT ".lnk"};

/* This is only used for dosinst.c. */
#اگر defined(DOSINST)
/*
 * Run an external command and wait for it to finish.
 */
    ایستا تهی
run_command(نویسه *cmd)
{
    نویسه	*cmd_path;
    نویسه	cmd_buf[BUFSIZE * 2 + 35];
    نویسه	*p;

    // On WinNT, 'start' is a shell built-in for cmd.exe rather than an
    // executable (start.exe) like in Win9x.
    cmd_path = searchpath_save("cmd.exe");
    اگر (cmd_path != پوچ)
    {
	// There is a cmd.exe, so this might be Windows NT.  If it is,
	// we need to call cmd.exe explicitly.  If it is a later OS,
	// calling cmd.exe won't hurt if it is present.
	// Also, "start" on NT expects a window title argument.
	// Replace the slashes with backslashes.
	وقتی ((p = strchr(cmd_path, '/')) != پوچ)
	    *p = '\\';
	sprintf(cmd_buf, "%s /c start \"vimcmd\" /wait %s", cmd_path, cmd);
	آزاد(cmd_path);
    }
    وگرنه
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
    تهی
add_pathsep(نویسه *name)
{
    صحیح		len = strlen(name);

    اگر (len > 0 && name[len - 1] != '\\' && name[len - 1] != '/')
	strcat(name, "\\");
}

/*
 * The normal chdir() does not change the default drive.  This one does.
 */
    صحیح
change_drive(صحیح drive)
{
    نویسه temp[3] = "-:";
    temp[0] = (نویسه)(drive + 'A' - 1);
    بازگشت !SetCurrentDirectory(temp);
}

/*
 * Change directory to "path".
 * Return 0 for success, -1 for failure.
 */
    صحیح
mch_chdir(نویسه *path)
{
    اگر (path[0] == NUL)		// just checking...
	بازگشت 0;
    اگر (path[1] == ':')		// has a drive name
    {
	اگر (change_drive(mytoupper(path[0]) - 'A' + 1))
	    بازگشت -1;		// invalid drive name
	path += 2;
    }
    اگر (*path == NUL)		// drive name only
	بازگشت 0;
    بازگشت chdir(path);		// let the normal chdir() do the rest
}

/*
 * Expand the executable name into a full path name.
 */
    ایستا نویسه *
my_fullpath(نویسه *buf, نویسه *fname UNUSED, صحیح len)
{
    // Only GetModuleFileName() will get the long file name path.
    // GetFullPathName() may still use the short (FAT) name.
    DWORD len_read = GetModuleFileName(پوچ, buf, (size_t)len);

    بازگشت (len_read > 0 && len_read < (DWORD)len) ? buf : پوچ;
}

/*
 * Remove the tail from a file or directory name.
 * Puts a NUL on the last '/' or '\'.
 */
    ایستا تهی
remove_tail(نویسه *path)
{
    صحیح		i;

    برای (i = strlen(path) - 1; i > 0; --i)
	اگر (path[i] == '/' || path[i] == '\\')
	{
	    path[i] = NUL;
	    بشکن;
	}
}


نویسه	installdir[MAX_PATH-9];	// top of the installation dir, where the
				// install.exe is located, E.g.:
				// "c:\vim\vim60"
صحیح	runtimeidx;		// index in installdir[] where "vim60" starts
نویسه	*sysdrive;		// system drive or "c:\"

/*
 * Setup for using this program.
 * Sets "installdir[]".
 */
    ایستا تهی
do_inits(نویسه **argv)
{
    // Find out the full path of our executable.
    اگر (my_fullpath(installdir, argv[0], اندازه(installdir)) == پوچ)
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
    اگر (sysdrive == پوچ || *sysdrive == NUL)
	sysdrive = "C:\\";
}
