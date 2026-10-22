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

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <sys/stat.h>
#अन्तर्भाव <fcntl.h>

#ifndef UNIX_LINT
# include <io.h>
# include <ctype.h>

# include <direct.h>

# include <windows.h>
# include <shlobj.h>
#endif

#ifdef UNIX_LINT
// Running lint on Unix: Some things are missing.
अक्षर *searchpath(अक्षर *name);
#endif

#जर defined(UNIX_LINT)
# include <unistd.h>
# include <errno.h>
#endif

#अन्तर्भाव "version.h"

#जर defined(UNIX_LINT)
# define vim_mkdir(x, y) सूची_गढ़ो((अक्षर *)(x), y)
#अन्यथा
# define vim_mkdir(x, y) _mkdir((अक्षर *)(x))
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
#ifndef किल्ली_WOW64_64_किल्ली
# define किल्ली_WOW64_64_किल्ली 0x0100
#endif
#ifndef किल्ली_WOW64_32_किल्ली
# define किल्ली_WOW64_32_किल्ली 0x0200
#endif

#ifdef __MINGW32__
# define UNUSED __attribute__((unused))
#अन्यथा
# define UNUSED
#endif

#define VIM_STARTMENU "Programs\\Vim " VIM_VERSION_SHORT

पूर्णांक	interactive;		// non-zero when running interactively

/*
 * Call malloc() and exit when out of memory.
 */
    स्थिर शून्य *
alloc(पूर्णांक len)
{
    शून्य *p;

    p = दो_स्मृति(len);
    जर (p == रिक्त)
    {
	म_लिखो("ERROR: out of memory\n");
	निकास(1);
    }
    परत p;
}

/*
 * The toupper() in Bcc 5.5 doesn't work, use our own implementation.
 */
    स्थिर पूर्णांक
mytoupper(पूर्णांक c)
{
    जर (c >= 'a' && c <= 'z')
	परत c - 'a' + 'A';
    परत c;
}

    स्थिर शून्य
myexit(पूर्णांक n)
{
    जर (!interactive)
    {
	// Present a prompt, otherwise error messages can't be read.
	म_लिखो("Press Enter to continue\n");
	शुरुआत(मानक_निवेश);
	(शून्य)अक्षर_लो();
    }
    निकास(n);
}


प्रकारव्याख्या BOOL (WINAPI *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
/*
 * Check if this is a 64-bit OS.
 */
    स्थिर BOOL
is_64bit_os(शून्य)
{
#ifdef _WIN64
    परत TRUE;
#अन्यथा
    BOOL bIsWow64 = FALSE;
    LPFN_ISWOW64PROCESS pIsWow64Process;

    pIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
	    GetModuleHandle("kernel32"), "IsWow64Process");
    जर (pIsWow64Process != रिक्त)
	pIsWow64Process(GetCurrentProcess(), &bIsWow64);
    परत bIsWow64;
#endif
}

    स्थिर अक्षर *
searchpath(अक्षर *name)
{
    स्थिर अक्षर widename[2 * BUFSIZE];
    स्थिर अक्षर location[2 * BUFSIZE + 2];

    // There appears to be a bug in FindExecutableA() on Windows NT.
    // Use FindExecutableW() instead...
    MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)name, -1,
	    (LPWSTR)widename, BUFSIZE);
    जर (FindExecutableW((LPCWSTR)widename, (LPCWSTR)"",
		(LPWSTR)location) > (HINSTANCE)32)
    {
	WideCharToMultiByte(CP_ACP, 0, (LPWSTR)location, -1,
		(LPSTR)widename, 2 * BUFSIZE, रिक्त, रिक्त);
	परत widename;
    }
    परत रिक्त;
}

/*
 * Call searchpath() and save the result in allocated memory, or return NULL.
 */
    स्थिर अक्षर *
searchpath_save(अक्षर *name)
{
    अक्षर	*p;
    अक्षर	*s;

    p = searchpath(name);
    जर (p == रिक्त)
	परत रिक्त;
    s = alloc(म_माप(p) + 1);
    म_नकल(s, p);
    परत s;
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
    पूर्णांक
get_shell_folder_path(
	अक्षर *shell_folder_path,
	स्थिरांक अक्षर *shell_folder_name)
{
    /*
     * The following code was successfully built with make_mvc.mak.
     * The resulting executable worked on Windows 95, Millennium Edition, and
     * 2000 Professional.  But it was changed after testing...
     */
    LPITEMIDLIST    pidl = 0; // Pointer to an Item ID list allocated below
    LPMALLOC	    pMalloc;  // Pointer to an IMalloc interface
    पूर्णांक		    csidl;
    पूर्णांक		    alt_csidl = -1;
    स्थिर पूर्णांक	    desktop_csidl = -1;
    स्थिर पूर्णांक	    programs_csidl = -1;
    पूर्णांक		    *pcsidl;
    पूर्णांक		    r;

    जर (म_भेद(shell_folder_name, "desktop") == 0)
    {
	pcsidl = &desktop_csidl;
	csidl = CSIDL_COMMON_DESKTOPDIRECTORY;
	alt_csidl = CSIDL_DESKTOP;
    }
    अन्यथा जर (म_भेदन(shell_folder_name, "Programs", 8) == 0)
    {
	pcsidl = &programs_csidl;
	csidl = CSIDL_COMMON_PROGRAMS;
	alt_csidl = CSIDL_PROGRAMS;
    }
    अन्यथा
    {
	म_लिखो("\nERROR (internal) unrecognised shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	परत FAIL;
    }

    // Did this stuff before, use the same ID again.
    जर (*pcsidl >= 0)
    {
	csidl = *pcsidl;
	alt_csidl = -1;
    }

retry:
    // Initialize pointer to IMalloc interface
    जर (NOERROR != SHGetMalloc(&pMalloc))
    {
	म_लिखो("\nERROR getting interface for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	परत FAIL;
    }

    // Get an ITEMIDLIST corresponding to the folder code
    जर (NOERROR != SHGetSpecialFolderLocation(0, csidl, &pidl))
    {
	जर (alt_csidl < 0 || NOERROR != SHGetSpecialFolderLocation(0,
							    alt_csidl, &pidl))
	{
	    म_लिखो("\nERROR getting ITEMIDLIST for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	    परत FAIL;
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

    जर (!r)
    {
	जर (alt_csidl >= 0)
	{
	    // We probably get here for Windows 95: the "all users"
	    // desktop/start menu entry doesn't exist.
	    csidl = alt_csidl;
	    alt_csidl = -1;
	    जा retry;
	}
	म_लिखो("\nERROR translating ITEMIDLIST for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	परत FAIL;
    }

    // If there is an alternative: verify we can write in this directory.
    // This should cause a retry when the "all users" directory exists but we
    // are a normal user and can't write there.
    जर (alt_csidl >= 0)
    {
	अक्षर tbuf[BUFSIZE];
	खाता *fd;

	म_नकल(tbuf, shell_folder_path);
	म_जोड़ो(tbuf, "\\vim write test");
	fd = ख_खोलो(tbuf, "w");
	जर (fd == रिक्त)
	{
	    csidl = alt_csidl;
	    alt_csidl = -1;
	    जा retry;
	}
	ख_बंद(fd);
	unlink(tbuf);
    }

    /*
     * Keep the found csidl for next time, so that we don't have to do the
     * write test every time.
     */
    जर (*pcsidl < 0)
	*pcsidl = csidl;

    जर (म_भेदन(shell_folder_name, "Programs\\", 9) == 0)
	म_जोड़ो(shell_folder_path, shell_folder_name + 8);

    परत OK;
}

/*
 * List of targets.  The first one (index zero) is used for the default path
 * for the batch files.
 */
#define TARGET_मोजणी  9

संरचना
{
    अक्षर	*name;		// Vim exe name (without .exe)
    अक्षर	*batname;	// batch file name
    अक्षर	*lnkname;	// shortcut file name
    अक्षर	*exename;	// exe file name
    अक्षर	*exenamearg;	// exe file name when using exearg
    अक्षर	*exearg;	// argument for vim.exe or gvim.exe
    अक्षर	*oldbat;	// path to existing xxx.bat or NULL
    अक्षर	*oldexe;	// path to existing xxx.exe or NULL
    अक्षर	batpath[BUFSIZE];  // path of batch file to create; not
				   // created when it's empty
} targets[TARGET_मोजणी] =
{
    {"all",	"batch files", रिक्त, रिक्त, रिक्त, रिक्त, रिक्त, रिक्त, ""},
    {"vim",	"vim.bat",	"Vim.lnk",
			"vim.exe",    "vim.exe",  "", रिक्त, रिक्त, ""},
    {"gvim",	"gvim.bat",	"gVim.lnk",
			"gvim.exe",   "gvim.exe", "", रिक्त, रिक्त, ""},
    {"evim",	"evim.bat",	"gVim Easy.lnk",
			"evim.exe",   "gvim.exe", "-y", रिक्त, रिक्त, ""},
    {"view",	"view.bat",	"Vim Read-only.lnk",
			"view.exe",   "vim.exe",  "-R", रिक्त, रिक्त, ""},
    {"gview",	"gview.bat",	"gVim Read-only.lnk",
			"gview.exe",  "gvim.exe", "-R", रिक्त, रिक्त, ""},
    {"vimdiff", "vimdiff.bat",	"Vim Diff.lnk",
			"vimdiff.exe","vim.exe",  "-d", रिक्त, रिक्त, ""},
    {"gvimdiff","gvimdiff.bat",	"gVim Diff.lnk",
			"gvimdiff.exe","gvim.exe", "-d", रिक्त, रिक्त, ""},
    {"vimtutor","vimtutor.bat", "Vim tutor.lnk",
			"vimtutor.bat",  "vimtutor.bat", "", रिक्त, रिक्त, ""},
};

/* Uninstall key for vim.bat, etc. */
#define VIMBAT_UNINSTKEY    "rem # uninstall key: " VIM_VERSION_NODOT " #"

#define ICON_मोजणी 3
अक्षर *(icon_names[ICON_मोजणी]) =
	{"gVim " VIM_VERSION_SHORT,
	 "gVim Easy " VIM_VERSION_SHORT,
	 "gVim Read only " VIM_VERSION_SHORT};
अक्षर *(icon_link_names[ICON_मोजणी]) =
	{"gVim " VIM_VERSION_SHORT ".lnk",
	 "gVim Easy " VIM_VERSION_SHORT ".lnk",
	 "gVim Read only " VIM_VERSION_SHORT ".lnk"};

/* This is only used for dosinst.c. */
#जर defined(DOSINST)
/*
 * Run an external command and wait for it to finish.
 */
    स्थिर शून्य
run_command(अक्षर *cmd)
{
    अक्षर	*cmd_path;
    अक्षर	cmd_buf[BUFSIZE * 2 + 35];
    अक्षर	*p;

    // On WinNT, 'start' is a shell built-in for cmd.exe rather than an
    // executable (start.exe) like in Win9x.
    cmd_path = searchpath_save("cmd.exe");
    जर (cmd_path != रिक्त)
    {
	// There is a cmd.exe, so this might be Windows NT.  If it is,
	// we need to call cmd.exe explicitly.  If it is a later OS,
	// calling cmd.exe won't hurt if it is present.
	// Also, "start" on NT expects a window title argument.
	// Replace the slashes with backslashes.
	पर्यंत ((p = म_अक्षर(cmd_path, '/')) != रिक्त)
	    *p = '\\';
	प्र_लिखो(cmd_buf, "%s /c start \"vimcmd\" /wait %s", cmd_path, cmd);
	मुक्त(cmd_path);
    }
    अन्यथा
    {
	// No cmd.exe, just make the call and let the system handle it.
	प्र_लिखो(cmd_buf, "start /w %s", cmd);
    }
    प्रणाली(cmd_buf);
}
#endif

/*
 * Append a backslash to "name" if there isn't one yet.
 */
    शून्य
add_pathsep(अक्षर *name)
{
    पूर्णांक		len = म_माप(name);

    जर (len > 0 && name[len - 1] != '\\' && name[len - 1] != '/')
	म_जोड़ो(name, "\\");
}

/*
 * The normal chdir() does not change the default drive.  This one does.
 */
    पूर्णांक
change_drive(पूर्णांक drive)
{
    अक्षर temp[3] = "-:";
    temp[0] = (अक्षर)(drive + 'A' - 1);
    परत !SetCurrentDirectory(temp);
}

/*
 * Change directory to "path".
 * Return 0 for success, -1 for failure.
 */
    पूर्णांक
mch_chdir(अक्षर *path)
{
    जर (path[0] == NUL)		// just checking...
	परत 0;
    जर (path[1] == ':')		// has a drive name
    {
	जर (change_drive(mytoupper(path[0]) - 'A' + 1))
	    परत -1;		// invalid drive name
	path += 2;
    }
    जर (*path == NUL)		// drive name only
	परत 0;
    परत स_बदलो(path);		// let the normal chdir() do the rest
}

/*
 * Expand the executable name into a full path name.
 */
    स्थिर अक्षर *
my_fullpath(अक्षर *buf, अक्षर *fname UNUSED, पूर्णांक len)
{
    // Only GetModuleFileName() will get the long file name path.
    // GetFullPathName() may still use the short (FAT) name.
    DWORD len_read = GetModuleFileName(रिक्त, buf, (माप_प्रकार)len);

    परत (len_read > 0 && len_read < (DWORD)len) ? buf : रिक्त;
}

/*
 * Remove the tail from a file or directory name.
 * Puts a NUL on the last '/' or '\'.
 */
    स्थिर शून्य
remove_tail(अक्षर *path)
{
    पूर्णांक		i;

    साठी (i = म_माप(path) - 1; i > 0; --i)
	जर (path[i] == '/' || path[i] == '\\')
	{
	    path[i] = NUL;
	    थांबा;
	}
}


अक्षर	installdir[MAX_PATH-9];	// top of the installation dir, where the
				// install.exe is located, E.g.:
				// "c:\vim\vim60"
पूर्णांक	runtimeidx;		// index in installdir[] where "vim60" starts
अक्षर	*sysdrive;		// system drive or "c:\"

/*
 * Setup for using this program.
 * Sets "installdir[]".
 */
    स्थिर शून्य
do_inits(अक्षर **argv)
{
    // Find out the full path of our executable.
    जर (my_fullpath(installdir, argv[0], आकार(installdir)) == रिक्त)
    {
	म_लिखो("ERROR: Cannot get name of executable\n");
	myexit(1);
    }
    // remove the tail, the executable name "install.exe"
    remove_tail(installdir);

    // change to the installdir
    mch_chdir(installdir);

    // Find the system drive.  Only used for searching the Vim executable, not
    // very important.
    sysdrive = दो_पर्या("SYSTEMDRIVE");
    जर (sysdrive == रिक्त || *sysdrive == NUL)
	sysdrive = "C:\\";
}
