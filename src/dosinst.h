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

#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <sys/stat.h>
#அந்தர்பாவ <fcntl.h>

#ifndef UNIX_LINT
# include <io.h>
# include <ctype.h>

# include <direct.h>

# include <windows.h>
# include <shlobj.h>
#endif

#ifdef UNIX_LINT
// Running lint on Unix: Some things are missing.
எழுத்து *searchpath(எழுத்து *name);
#endif

#எனில் defined(UNIX_LINT)
# include <unistd.h>
# include <errno.h>
#endif

#அந்தர்பாவ "version.h"

#எனில் defined(UNIX_LINT)
# define vim_mkdir(x, y) mkdir((எழுத்து *)(x), y)
#இல்லையெனில்
# define vim_mkdir(x, y) _mkdir((எழுத்து *)(x))
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
#ifndef சாவி_WOW64_64_சாவி
# define சாவி_WOW64_64_சாவி 0x0100
#endif
#ifndef சாவி_WOW64_32_சாவி
# define சாவி_WOW64_32_சாவி 0x0200
#endif

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#இல்லையெனில்
# define UNUSED
#endif

#define VIM_STARTMENU "Programs\\Vim " VIM_VERSION_SHORT

முழு	interactive;		// non-zero when running interactively

/*
 * Call malloc() and exit when out of memory.
 */
    நிலையான வெறுமை *
alloc(முழு len)
{
    வெறுமை *p;

    p = malloc(len);
    எனில் (p == வெற்று)
    {
	printf("ERROR: out of memory\n");
	exit(1);
    }
    திரும்பு p;
}

/*
 * The toupper() in Bcc 5.5 doesn't work, use our own implementation.
 */
    நிலையான முழு
mytoupper(முழு c)
{
    எனில் (c >= 'a' && c <= 'z')
	திரும்பு c - 'a' + 'A';
    திரும்பு c;
}

    நிலையான வெறுமை
myexit(முழு n)
{
    எனில் (!interactive)
    {
	// Present a prompt, otherwise error messages can't be read.
	printf("Press Enter to continue\n");
	rewind(stdin);
	(வெறுமை)getchar();
    }
    exit(n);
}


வகைவரை BOOL (WINAPI *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
/*
 * Check if this is a 64-bit OS.
 */
    நிலையான BOOL
is_64bit_os(வெறுமை)
{
#ifdef _WIN64
    திரும்பு TRUE;
#இல்லையெனில்
    BOOL bIsWow64 = FALSE;
    LPFN_ISWOW64PROCESS pIsWow64Process;

    pIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
	    GetModuleHandle("kernel32"), "IsWow64Process");
    எனில் (pIsWow64Process != வெற்று)
	pIsWow64Process(GetCurrentProcess(), &bIsWow64);
    திரும்பு bIsWow64;
#endif
}

    நிலையான எழுத்து *
searchpath(எழுத்து *name)
{
    நிலையான எழுத்து widename[2 * BUFSIZE];
    நிலையான எழுத்து location[2 * BUFSIZE + 2];

    // There appears to be a bug in FindExecutableA() on Windows NT.
    // Use FindExecutableW() instead...
    MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)name, -1,
	    (LPWSTR)widename, BUFSIZE);
    எனில் (FindExecutableW((LPCWSTR)widename, (LPCWSTR)"",
		(LPWSTR)location) > (HINSTANCE)32)
    {
	WideCharToMultiByte(CP_ACP, 0, (LPWSTR)location, -1,
		(LPSTR)widename, 2 * BUFSIZE, வெற்று, வெற்று);
	திரும்பு widename;
    }
    திரும்பு வெற்று;
}

/*
 * Call searchpath() and save the result in allocated memory, or return NULL.
 */
    நிலையான எழுத்து *
searchpath_save(எழுத்து *name)
{
    எழுத்து	*p;
    எழுத்து	*s;

    p = searchpath(name);
    எனில் (p == வெற்று)
	திரும்பு வெற்று;
    s = alloc(strlen(p) + 1);
    strcpy(s, p);
    திரும்பு s;
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
    முழு
get_shell_folder_path(
	எழுத்து *shell_folder_path,
	மாறிலி எழுத்து *shell_folder_name)
{
    /*
     * The following code was successfully built with make_mvc.mak.
     * The resulting executable worked on Windows 95, Millennium Edition, and
     * 2000 Professional.  But it was changed after testing...
     */
    LPITEMIDLIST    pidl = 0; // Pointer to an Item ID list allocated below
    LPMALLOC	    pMalloc;  // Pointer to an IMalloc interface
    முழு		    csidl;
    முழு		    alt_csidl = -1;
    நிலையான முழு	    desktop_csidl = -1;
    நிலையான முழு	    programs_csidl = -1;
    முழு		    *pcsidl;
    முழு		    r;

    எனில் (strcmp(shell_folder_name, "desktop") == 0)
    {
	pcsidl = &desktop_csidl;
	csidl = CSIDL_COMMON_DESKTOPDIRECTORY;
	alt_csidl = CSIDL_DESKTOP;
    }
    இல்லையெனில் எனில் (strncmp(shell_folder_name, "Programs", 8) == 0)
    {
	pcsidl = &programs_csidl;
	csidl = CSIDL_COMMON_PROGRAMS;
	alt_csidl = CSIDL_PROGRAMS;
    }
    இல்லையெனில்
    {
	printf("\nERROR (internal) unrecognised shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	திரும்பு FAIL;
    }

    // Did this stuff before, use the same ID again.
    எனில் (*pcsidl >= 0)
    {
	csidl = *pcsidl;
	alt_csidl = -1;
    }

retry:
    // Initialize pointer to IMalloc interface
    எனில் (NOERROR != SHGetMalloc(&pMalloc))
    {
	printf("\nERROR getting interface for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	திரும்பு FAIL;
    }

    // Get an ITEMIDLIST corresponding to the folder code
    எனில் (NOERROR != SHGetSpecialFolderLocation(0, csidl, &pidl))
    {
	எனில் (alt_csidl < 0 || NOERROR != SHGetSpecialFolderLocation(0,
							    alt_csidl, &pidl))
	{
	    printf("\nERROR getting ITEMIDLIST for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	    திரும்பு FAIL;
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

    எனில் (!r)
    {
	எனில் (alt_csidl >= 0)
	{
	    // We probably get here for Windows 95: the "all users"
	    // desktop/start menu entry doesn't exist.
	    csidl = alt_csidl;
	    alt_csidl = -1;
	    செல் retry;
	}
	printf("\nERROR translating ITEMIDLIST for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	திரும்பு FAIL;
    }

    // If there is an alternative: verify we can write in this directory.
    // This should cause a retry when the "all users" directory exists but we
    // are a normal user and can't write there.
    எனில் (alt_csidl >= 0)
    {
	எழுத்து tbuf[BUFSIZE];
	FILE *fd;

	strcpy(tbuf, shell_folder_path);
	strcat(tbuf, "\\vim write test");
	fd = fopen(tbuf, "w");
	எனில் (fd == வெற்று)
	{
	    csidl = alt_csidl;
	    alt_csidl = -1;
	    செல் retry;
	}
	fclose(fd);
	unlink(tbuf);
    }

    /*
     * Keep the found csidl for next time, so that we don't have to do the
     * write test every time.
     */
    எனில் (*pcsidl < 0)
	*pcsidl = csidl;

    எனில் (strncmp(shell_folder_name, "Programs\\", 9) == 0)
	strcat(shell_folder_path, shell_folder_name + 8);

    திரும்பு OK;
}

/*
 * List of targets.  The first one (index zero) is used for the default path
 * for the batch files.
 */
#define TARGET_மொத்தம்  9

கட்டமைப்பு
{
    எழுத்து	*name;		// Vim exe name (without .exe)
    எழுத்து	*batname;	// batch file name
    எழுத்து	*lnkname;	// shortcut file name
    எழுத்து	*exename;	// exe file name
    எழுத்து	*exenamearg;	// exe file name when using exearg
    எழுத்து	*exearg;	// argument for vim.exe or gvim.exe
    எழுத்து	*oldbat;	// path to existing xxx.bat or NULL
    எழுத்து	*oldexe;	// path to existing xxx.exe or NULL
    எழுத்து	batpath[BUFSIZE];  // path of batch file to create; not
				   // created when it's empty
} targets[TARGET_மொத்தம்] =
{
    {"all",	"batch files", வெற்று, வெற்று, வெற்று, வெற்று, வெற்று, வெற்று, ""},
    {"vim",	"vim.bat",	"Vim.lnk",
			"vim.exe",    "vim.exe",  "", வெற்று, வெற்று, ""},
    {"gvim",	"gvim.bat",	"gVim.lnk",
			"gvim.exe",   "gvim.exe", "", வெற்று, வெற்று, ""},
    {"evim",	"evim.bat",	"gVim Easy.lnk",
			"evim.exe",   "gvim.exe", "-y", வெற்று, வெற்று, ""},
    {"view",	"view.bat",	"Vim Read-only.lnk",
			"view.exe",   "vim.exe",  "-R", வெற்று, வெற்று, ""},
    {"gview",	"gview.bat",	"gVim Read-only.lnk",
			"gview.exe",  "gvim.exe", "-R", வெற்று, வெற்று, ""},
    {"vimdiff", "vimdiff.bat",	"Vim Diff.lnk",
			"vimdiff.exe","vim.exe",  "-d", வெற்று, வெற்று, ""},
    {"gvimdiff","gvimdiff.bat",	"gVim Diff.lnk",
			"gvimdiff.exe","gvim.exe", "-d", வெற்று, வெற்று, ""},
    {"vimtutor","vimtutor.bat", "Vim tutor.lnk",
			"vimtutor.bat",  "vimtutor.bat", "", வெற்று, வெற்று, ""},
};

/* Uninstall key for vim.bat, etc. */
#define VIMBAT_UNINSTKEY    "rem # uninstall key: " VIM_VERSION_NODOT " #"

#define ICON_மொத்தம் 3
எழுத்து *(icon_names[ICON_மொத்தம்]) =
	{"gVim " VIM_VERSION_SHORT,
	 "gVim Easy " VIM_VERSION_SHORT,
	 "gVim Read only " VIM_VERSION_SHORT};
எழுத்து *(icon_link_names[ICON_மொத்தம்]) =
	{"gVim " VIM_VERSION_SHORT ".lnk",
	 "gVim Easy " VIM_VERSION_SHORT ".lnk",
	 "gVim Read only " VIM_VERSION_SHORT ".lnk"};

/* This is only used for dosinst.c. */
#எனில் defined(DOSINST)
/*
 * Run an external command and wait for it to finish.
 */
    நிலையான வெறுமை
run_command(எழுத்து *cmd)
{
    எழுத்து	*cmd_path;
    எழுத்து	cmd_buf[BUFSIZE * 2 + 35];
    எழுத்து	*p;

    // On WinNT, 'start' is a shell built-in for cmd.exe rather than an
    // executable (start.exe) like in Win9x.
    cmd_path = searchpath_save("cmd.exe");
    எனில் (cmd_path != வெற்று)
    {
	// There is a cmd.exe, so this might be Windows NT.  If it is,
	// we need to call cmd.exe explicitly.  If it is a later OS,
	// calling cmd.exe won't hurt if it is present.
	// Also, "start" on NT expects a window title argument.
	// Replace the slashes with backslashes.
	போது ((p = strchr(cmd_path, '/')) != வெற்று)
	    *p = '\\';
	sprintf(cmd_buf, "%s /c start \"vimcmd\" /wait %s", cmd_path, cmd);
	விடுவி(cmd_path);
    }
    இல்லையெனில்
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
    வெறுமை
add_pathsep(எழுத்து *name)
{
    முழு		len = strlen(name);

    எனில் (len > 0 && name[len - 1] != '\\' && name[len - 1] != '/')
	strcat(name, "\\");
}

/*
 * The normal chdir() does not change the default drive.  This one does.
 */
    முழு
change_drive(முழு drive)
{
    எழுத்து temp[3] = "-:";
    temp[0] = (எழுத்து)(drive + 'A' - 1);
    திரும்பு !SetCurrentDirectory(temp);
}

/*
 * Change directory to "path".
 * Return 0 for success, -1 for failure.
 */
    முழு
mch_chdir(எழுத்து *path)
{
    எனில் (path[0] == NUL)		// just checking...
	திரும்பு 0;
    எனில் (path[1] == ':')		// has a drive name
    {
	எனில் (change_drive(mytoupper(path[0]) - 'A' + 1))
	    திரும்பு -1;		// invalid drive name
	path += 2;
    }
    எனில் (*path == NUL)		// drive name only
	திரும்பு 0;
    திரும்பு chdir(path);		// let the normal chdir() do the rest
}

/*
 * Expand the executable name into a full path name.
 */
    நிலையான எழுத்து *
my_fullpath(எழுத்து *buf, எழுத்து *fname UNUSED, முழு len)
{
    // Only GetModuleFileName() will get the long file name path.
    // GetFullPathName() may still use the short (FAT) name.
    DWORD len_read = GetModuleFileName(வெற்று, buf, (size_t)len);

    திரும்பு (len_read > 0 && len_read < (DWORD)len) ? buf : வெற்று;
}

/*
 * Remove the tail from a file or directory name.
 * Puts a NUL on the last '/' or '\'.
 */
    நிலையான வெறுமை
remove_tail(எழுத்து *path)
{
    முழு		i;

    க்கு (i = strlen(path) - 1; i > 0; --i)
	எனில் (path[i] == '/' || path[i] == '\\')
	{
	    path[i] = NUL;
	    நிறுத்து;
	}
}


எழுத்து	installdir[MAX_PATH-9];	// top of the installation dir, where the
				// install.exe is located, E.g.:
				// "c:\vim\vim60"
முழு	runtimeidx;		// index in installdir[] where "vim60" starts
எழுத்து	*sysdrive;		// system drive or "c:\"

/*
 * Setup for using this program.
 * Sets "installdir[]".
 */
    நிலையான வெறுமை
do_inits(எழுத்து **argv)
{
    // Find out the full path of our executable.
    எனில் (my_fullpath(installdir, argv[0], அளவு(installdir)) == வெற்று)
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
    எனில் (sysdrive == வெற்று || *sysdrive == NUL)
	sysdrive = "C:\\";
}
