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

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <sys/stat.h>
#અન્તર્ભાવ <fcntl.h>

#ifndef UNIX_LINT
# include <io.h>
# include <ctype.h>

# include <direct.h>

# include <windows.h>
# include <shlobj.h>
#endif

#ifdef UNIX_LINT
// Running lint on Unix: Some things are missing.
અક્ષર *searchpath(અક્ષર *name);
#endif

#જો defined(UNIX_LINT)
# include <unistd.h>
# include <errno.h>
#endif

#અન્તર્ભાવ "version.h"

#જો defined(UNIX_LINT)
# define vim_mkdir(x, y) mkdir((અક્ષર *)(x), y)
#નહીંતર
# define vim_mkdir(x, y) _mkdir((અક્ષર *)(x))
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
#ifndef ચાવી_WOW64_64_ચાવી
# define ચાવી_WOW64_64_ચાવી 0x0100
#endif
#ifndef ચાવી_WOW64_32_ચાવી
# define ચાવી_WOW64_32_ચાવી 0x0200
#endif

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#નહીંતર
# define UNUSED
#endif

#define VIM_STARTMENU "Programs\\Vim " VIM_VERSION_SHORT

પૂર્ણાંક	interactive;		// non-zero when running interactively

/*
 * Call malloc() and exit when out of memory.
 */
    સ્થિર શૂન્ય *
alloc(પૂર્ણાંક len)
{
    શૂન્ય *p;

    p = malloc(len);
    જો (p == ખાલી)
    {
	printf("ERROR: out of memory\n");
	exit(1);
    }
    પાછા p;
}

/*
 * The toupper() in Bcc 5.5 doesn't work, use our own implementation.
 */
    સ્થિર પૂર્ણાંક
mytoupper(પૂર્ણાંક c)
{
    જો (c >= 'a' && c <= 'z')
	પાછા c - 'a' + 'A';
    પાછા c;
}

    સ્થિર શૂન્ય
myexit(પૂર્ણાંક n)
{
    જો (!interactive)
    {
	// Present a prompt, otherwise error messages can't be read.
	printf("Press Enter to continue\n");
	rewind(stdin);
	(શૂન્ય)getchar();
    }
    exit(n);
}


પ્રકારવ્યાખ્યા BOOL (WINAPI *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
/*
 * Check if this is a 64-bit OS.
 */
    સ્થિર BOOL
is_64bit_os(શૂન્ય)
{
#ifdef _WIN64
    પાછા TRUE;
#નહીંતર
    BOOL bIsWow64 = FALSE;
    LPFN_ISWOW64PROCESS pIsWow64Process;

    pIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
	    GetModuleHandle("kernel32"), "IsWow64Process");
    જો (pIsWow64Process != ખાલી)
	pIsWow64Process(GetCurrentProcess(), &bIsWow64);
    પાછા bIsWow64;
#endif
}

    સ્થિર અક્ષર *
searchpath(અક્ષર *name)
{
    સ્થિર અક્ષર widename[2 * BUFSIZE];
    સ્થિર અક્ષર location[2 * BUFSIZE + 2];

    // There appears to be a bug in FindExecutableA() on Windows NT.
    // Use FindExecutableW() instead...
    MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)name, -1,
	    (LPWSTR)widename, BUFSIZE);
    જો (FindExecutableW((LPCWSTR)widename, (LPCWSTR)"",
		(LPWSTR)location) > (HINSTANCE)32)
    {
	WideCharToMultiByte(CP_ACP, 0, (LPWSTR)location, -1,
		(LPSTR)widename, 2 * BUFSIZE, ખાલી, ખાલી);
	પાછા widename;
    }
    પાછા ખાલી;
}

/*
 * Call searchpath() and save the result in allocated memory, or return NULL.
 */
    સ્થિર અક્ષર *
searchpath_save(અક્ષર *name)
{
    અક્ષર	*p;
    અક્ષર	*s;

    p = searchpath(name);
    જો (p == ખાલી)
	પાછા ખાલી;
    s = alloc(strlen(p) + 1);
    strcpy(s, p);
    પાછા s;
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
    પૂર્ણાંક
get_shell_folder_path(
	અક્ષર *shell_folder_path,
	અચલ અક્ષર *shell_folder_name)
{
    /*
     * The following code was successfully built with make_mvc.mak.
     * The resulting executable worked on Windows 95, Millennium Edition, and
     * 2000 Professional.  But it was changed after testing...
     */
    LPITEMIDLIST    pidl = 0; // Pointer to an Item ID list allocated below
    LPMALLOC	    pMalloc;  // Pointer to an IMalloc interface
    પૂર્ણાંક		    csidl;
    પૂર્ણાંક		    alt_csidl = -1;
    સ્થિર પૂર્ણાંક	    desktop_csidl = -1;
    સ્થિર પૂર્ણાંક	    programs_csidl = -1;
    પૂર્ણાંક		    *pcsidl;
    પૂર્ણાંક		    r;

    જો (strcmp(shell_folder_name, "desktop") == 0)
    {
	pcsidl = &desktop_csidl;
	csidl = CSIDL_COMMON_DESKTOPDIRECTORY;
	alt_csidl = CSIDL_DESKTOP;
    }
    નહીંતર જો (strncmp(shell_folder_name, "Programs", 8) == 0)
    {
	pcsidl = &programs_csidl;
	csidl = CSIDL_COMMON_PROGRAMS;
	alt_csidl = CSIDL_PROGRAMS;
    }
    નહીંતર
    {
	printf("\nERROR (internal) unrecognised shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	પાછા FAIL;
    }

    // Did this stuff before, use the same ID again.
    જો (*pcsidl >= 0)
    {
	csidl = *pcsidl;
	alt_csidl = -1;
    }

retry:
    // Initialize pointer to IMalloc interface
    જો (NOERROR != SHGetMalloc(&pMalloc))
    {
	printf("\nERROR getting interface for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	પાછા FAIL;
    }

    // Get an ITEMIDLIST corresponding to the folder code
    જો (NOERROR != SHGetSpecialFolderLocation(0, csidl, &pidl))
    {
	જો (alt_csidl < 0 || NOERROR != SHGetSpecialFolderLocation(0,
							    alt_csidl, &pidl))
	{
	    printf("\nERROR getting ITEMIDLIST for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	    પાછા FAIL;
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

    જો (!r)
    {
	જો (alt_csidl >= 0)
	{
	    // We probably get here for Windows 95: the "all users"
	    // desktop/start menu entry doesn't exist.
	    csidl = alt_csidl;
	    alt_csidl = -1;
	    જાઓ retry;
	}
	printf("\nERROR translating ITEMIDLIST for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	પાછા FAIL;
    }

    // If there is an alternative: verify we can write in this directory.
    // This should cause a retry when the "all users" directory exists but we
    // are a normal user and can't write there.
    જો (alt_csidl >= 0)
    {
	અક્ષર tbuf[BUFSIZE];
	FILE *fd;

	strcpy(tbuf, shell_folder_path);
	strcat(tbuf, "\\vim write test");
	fd = fopen(tbuf, "w");
	જો (fd == ખાલી)
	{
	    csidl = alt_csidl;
	    alt_csidl = -1;
	    જાઓ retry;
	}
	fclose(fd);
	unlink(tbuf);
    }

    /*
     * Keep the found csidl for next time, so that we don't have to do the
     * write test every time.
     */
    જો (*pcsidl < 0)
	*pcsidl = csidl;

    જો (strncmp(shell_folder_name, "Programs\\", 9) == 0)
	strcat(shell_folder_path, shell_folder_name + 8);

    પાછા OK;
}

/*
 * List of targets.  The first one (index zero) is used for the default path
 * for the batch files.
 */
#define TARGET_ગણતરી  9

સંરચના
{
    અક્ષર	*name;		// Vim exe name (without .exe)
    અક્ષર	*batname;	// batch file name
    અક્ષર	*lnkname;	// shortcut file name
    અક્ષર	*exename;	// exe file name
    અક્ષર	*exenamearg;	// exe file name when using exearg
    અક્ષર	*exearg;	// argument for vim.exe or gvim.exe
    અક્ષર	*oldbat;	// path to existing xxx.bat or NULL
    અક્ષર	*oldexe;	// path to existing xxx.exe or NULL
    અક્ષર	batpath[BUFSIZE];  // path of batch file to create; not
				   // created when it's empty
} targets[TARGET_ગણતરી] =
{
    {"all",	"batch files", ખાલી, ખાલી, ખાલી, ખાલી, ખાલી, ખાલી, ""},
    {"vim",	"vim.bat",	"Vim.lnk",
			"vim.exe",    "vim.exe",  "", ખાલી, ખાલી, ""},
    {"gvim",	"gvim.bat",	"gVim.lnk",
			"gvim.exe",   "gvim.exe", "", ખાલી, ખાલી, ""},
    {"evim",	"evim.bat",	"gVim Easy.lnk",
			"evim.exe",   "gvim.exe", "-y", ખાલી, ખાલી, ""},
    {"view",	"view.bat",	"Vim Read-only.lnk",
			"view.exe",   "vim.exe",  "-R", ખાલી, ખાલી, ""},
    {"gview",	"gview.bat",	"gVim Read-only.lnk",
			"gview.exe",  "gvim.exe", "-R", ખાલી, ખાલી, ""},
    {"vimdiff", "vimdiff.bat",	"Vim Diff.lnk",
			"vimdiff.exe","vim.exe",  "-d", ખાલી, ખાલી, ""},
    {"gvimdiff","gvimdiff.bat",	"gVim Diff.lnk",
			"gvimdiff.exe","gvim.exe", "-d", ખાલી, ખાલી, ""},
    {"vimtutor","vimtutor.bat", "Vim tutor.lnk",
			"vimtutor.bat",  "vimtutor.bat", "", ખાલી, ખાલી, ""},
};

/* Uninstall key for vim.bat, etc. */
#define VIMBAT_UNINSTKEY    "rem # uninstall key: " VIM_VERSION_NODOT " #"

#define ICON_ગણતરી 3
અક્ષર *(icon_names[ICON_ગણતરી]) =
	{"gVim " VIM_VERSION_SHORT,
	 "gVim Easy " VIM_VERSION_SHORT,
	 "gVim Read only " VIM_VERSION_SHORT};
અક્ષર *(icon_link_names[ICON_ગણતરી]) =
	{"gVim " VIM_VERSION_SHORT ".lnk",
	 "gVim Easy " VIM_VERSION_SHORT ".lnk",
	 "gVim Read only " VIM_VERSION_SHORT ".lnk"};

/* This is only used for dosinst.c. */
#જો defined(DOSINST)
/*
 * Run an external command and wait for it to finish.
 */
    સ્થિર શૂન્ય
run_command(અક્ષર *cmd)
{
    અક્ષર	*cmd_path;
    અક્ષર	cmd_buf[BUFSIZE * 2 + 35];
    અક્ષર	*p;

    // On WinNT, 'start' is a shell built-in for cmd.exe rather than an
    // executable (start.exe) like in Win9x.
    cmd_path = searchpath_save("cmd.exe");
    જો (cmd_path != ખાલી)
    {
	// There is a cmd.exe, so this might be Windows NT.  If it is,
	// we need to call cmd.exe explicitly.  If it is a later OS,
	// calling cmd.exe won't hurt if it is present.
	// Also, "start" on NT expects a window title argument.
	// Replace the slashes with backslashes.
	જ્યારે ((p = strchr(cmd_path, '/')) != ખાલી)
	    *p = '\\';
	sprintf(cmd_buf, "%s /c start \"vimcmd\" /wait %s", cmd_path, cmd);
	મુક્ત(cmd_path);
    }
    નહીંતર
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
    શૂન્ય
add_pathsep(અક્ષર *name)
{
    પૂર્ણાંક		len = strlen(name);

    જો (len > 0 && name[len - 1] != '\\' && name[len - 1] != '/')
	strcat(name, "\\");
}

/*
 * The normal chdir() does not change the default drive.  This one does.
 */
    પૂર્ણાંક
change_drive(પૂર્ણાંક drive)
{
    અક્ષર temp[3] = "-:";
    temp[0] = (અક્ષર)(drive + 'A' - 1);
    પાછા !SetCurrentDirectory(temp);
}

/*
 * Change directory to "path".
 * Return 0 for success, -1 for failure.
 */
    પૂર્ણાંક
mch_chdir(અક્ષર *path)
{
    જો (path[0] == NUL)		// just checking...
	પાછા 0;
    જો (path[1] == ':')		// has a drive name
    {
	જો (change_drive(mytoupper(path[0]) - 'A' + 1))
	    પાછા -1;		// invalid drive name
	path += 2;
    }
    જો (*path == NUL)		// drive name only
	પાછા 0;
    પાછા chdir(path);		// let the normal chdir() do the rest
}

/*
 * Expand the executable name into a full path name.
 */
    સ્થિર અક્ષર *
my_fullpath(અક્ષર *buf, અક્ષર *fname UNUSED, પૂર્ણાંક len)
{
    // Only GetModuleFileName() will get the long file name path.
    // GetFullPathName() may still use the short (FAT) name.
    DWORD len_read = GetModuleFileName(ખાલી, buf, (size_t)len);

    પાછા (len_read > 0 && len_read < (DWORD)len) ? buf : ખાલી;
}

/*
 * Remove the tail from a file or directory name.
 * Puts a NUL on the last '/' or '\'.
 */
    સ્થિર શૂન્ય
remove_tail(અક્ષર *path)
{
    પૂર્ણાંક		i;

    માટે (i = strlen(path) - 1; i > 0; --i)
	જો (path[i] == '/' || path[i] == '\\')
	{
	    path[i] = NUL;
	    તોડો;
	}
}


અક્ષર	installdir[MAX_PATH-9];	// top of the installation dir, where the
				// install.exe is located, E.g.:
				// "c:\vim\vim60"
પૂર્ણાંક	runtimeidx;		// index in installdir[] where "vim60" starts
અક્ષર	*sysdrive;		// system drive or "c:\"

/*
 * Setup for using this program.
 * Sets "installdir[]".
 */
    સ્થિર શૂન્ય
do_inits(અક્ષર **argv)
{
    // Find out the full path of our executable.
    જો (my_fullpath(installdir, argv[0], કદ(installdir)) == ખાલી)
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
    જો (sysdrive == ખાલી || *sysdrive == NUL)
	sysdrive = "C:\\";
}
