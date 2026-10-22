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
वर्ण *searchpath(वर्ण *name);
#endif

#यदि defined(UNIX_LINT)
# include <unistd.h>
# include <errno.h>
#endif

#अन्तर्भाव "version.h"

#यदि defined(UNIX_LINT)
# define vim_mkdir(x, y) mkdir((वर्ण *)(x), y)
#अन्यथा
# define vim_mkdir(x, y) _mkdir((वर्ण *)(x))
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

पूर्णाङ्क	interactive;		// non-zero when running interactively

/*
 * Call malloc() and exit when out of memory.
 */
    स्थिर शून्य *
alloc(पूर्णाङ्क len)
{
    शून्य *p;

    p = malloc(len);
    यदि (p == रिक्त)
    {
	printf("ERROR: out of memory\n");
	exit(1);
    }
    प्रत्यागम p;
}

/*
 * The toupper() in Bcc 5.5 doesn't work, use our own implementation.
 */
    स्थिर पूर्णाङ्क
mytoupper(पूर्णाङ्क c)
{
    यदि (c >= 'a' && c <= 'z')
	प्रत्यागम c - 'a' + 'A';
    प्रत्यागम c;
}

    स्थिर शून्य
myexit(पूर्णाङ्क n)
{
    यदि (!interactive)
    {
	// Present a prompt, otherwise error messages can't be read.
	printf("Press Enter to continue\n");
	rewind(stdin);
	(शून्य)getchar();
    }
    exit(n);
}


प्रकारनिर्देश BOOL (WINAPI *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
/*
 * Check if this is a 64-bit OS.
 */
    स्थिर BOOL
is_64bit_os(शून्य)
{
#ifdef _WIN64
    प्रत्यागम TRUE;
#अन्यथा
    BOOL bIsWow64 = FALSE;
    LPFN_ISWOW64PROCESS pIsWow64Process;

    pIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
	    GetModuleHandle("kernel32"), "IsWow64Process");
    यदि (pIsWow64Process != रिक्त)
	pIsWow64Process(GetCurrentProcess(), &bIsWow64);
    प्रत्यागम bIsWow64;
#endif
}

    स्थिर वर्ण *
searchpath(वर्ण *name)
{
    स्थिर वर्ण widename[2 * BUFSIZE];
    स्थिर वर्ण location[2 * BUFSIZE + 2];

    // There appears to be a bug in FindExecutableA() on Windows NT.
    // Use FindExecutableW() instead...
    MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)name, -1,
	    (LPWSTR)widename, BUFSIZE);
    यदि (FindExecutableW((LPCWSTR)widename, (LPCWSTR)"",
		(LPWSTR)location) > (HINSTANCE)32)
    {
	WideCharToMultiByte(CP_ACP, 0, (LPWSTR)location, -1,
		(LPSTR)widename, 2 * BUFSIZE, रिक्त, रिक्त);
	प्रत्यागम widename;
    }
    प्रत्यागम रिक्त;
}

/*
 * Call searchpath() and save the result in allocated memory, or return NULL.
 */
    स्थिर वर्ण *
searchpath_save(वर्ण *name)
{
    वर्ण	*p;
    वर्ण	*s;

    p = searchpath(name);
    यदि (p == रिक्त)
	प्रत्यागम रिक्त;
    s = alloc(strlen(p) + 1);
    strcpy(s, p);
    प्रत्यागम s;
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
    पूर्णाङ्क
get_shell_folder_path(
	वर्ण *shell_folder_path,
	ध्रुव वर्ण *shell_folder_name)
{
    /*
     * The following code was successfully built with make_mvc.mak.
     * The resulting executable worked on Windows 95, Millennium Edition, and
     * 2000 Professional.  But it was changed after testing...
     */
    LPITEMIDLIST    pidl = 0; // Pointer to an Item ID list allocated below
    LPMALLOC	    pMalloc;  // Pointer to an IMalloc interface
    पूर्णाङ्क		    csidl;
    पूर्णाङ्क		    alt_csidl = -1;
    स्थिर पूर्णाङ्क	    desktop_csidl = -1;
    स्थिर पूर्णाङ्क	    programs_csidl = -1;
    पूर्णाङ्क		    *pcsidl;
    पूर्णाङ्क		    r;

    यदि (strcmp(shell_folder_name, "desktop") == 0)
    {
	pcsidl = &desktop_csidl;
	csidl = CSIDL_COMMON_DESKTOPDIRECTORY;
	alt_csidl = CSIDL_DESKTOP;
    }
    अन्यथा यदि (strncmp(shell_folder_name, "Programs", 8) == 0)
    {
	pcsidl = &programs_csidl;
	csidl = CSIDL_COMMON_PROGRAMS;
	alt_csidl = CSIDL_PROGRAMS;
    }
    अन्यथा
    {
	printf("\nERROR (internal) unrecognised shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	प्रत्यागम FAIL;
    }

    // Did this stuff before, use the same ID again.
    यदि (*pcsidl >= 0)
    {
	csidl = *pcsidl;
	alt_csidl = -1;
    }

retry:
    // Initialize pointer to IMalloc interface
    यदि (NOERROR != SHGetMalloc(&pMalloc))
    {
	printf("\nERROR getting interface for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	प्रत्यागम FAIL;
    }

    // Get an ITEMIDLIST corresponding to the folder code
    यदि (NOERROR != SHGetSpecialFolderLocation(0, csidl, &pidl))
    {
	यदि (alt_csidl < 0 || NOERROR != SHGetSpecialFolderLocation(0,
							    alt_csidl, &pidl))
	{
	    printf("\nERROR getting ITEMIDLIST for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	    प्रत्यागम FAIL;
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

    यदि (!r)
    {
	यदि (alt_csidl >= 0)
	{
	    // We probably get here for Windows 95: the "all users"
	    // desktop/start menu entry doesn't exist.
	    csidl = alt_csidl;
	    alt_csidl = -1;
	    गच्छ retry;
	}
	printf("\nERROR translating ITEMIDLIST for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	प्रत्यागम FAIL;
    }

    // If there is an alternative: verify we can write in this directory.
    // This should cause a retry when the "all users" directory exists but we
    // are a normal user and can't write there.
    यदि (alt_csidl >= 0)
    {
	वर्ण tbuf[BUFSIZE];
	FILE *fd;

	strcpy(tbuf, shell_folder_path);
	strcat(tbuf, "\\vim write test");
	fd = fopen(tbuf, "w");
	यदि (fd == रिक्त)
	{
	    csidl = alt_csidl;
	    alt_csidl = -1;
	    गच्छ retry;
	}
	fclose(fd);
	unlink(tbuf);
    }

    /*
     * Keep the found csidl for next time, so that we don't have to do the
     * write test every time.
     */
    यदि (*pcsidl < 0)
	*pcsidl = csidl;

    यदि (strncmp(shell_folder_name, "Programs\\", 9) == 0)
	strcat(shell_folder_path, shell_folder_name + 8);

    प्रत्यागम OK;
}

/*
 * List of targets.  The first one (index zero) is used for the default path
 * for the batch files.
 */
#define TARGET_मोजणी  9

संरचना
{
    वर्ण	*name;		// Vim exe name (without .exe)
    वर्ण	*batname;	// batch file name
    वर्ण	*lnkname;	// shortcut file name
    वर्ण	*exename;	// exe file name
    वर्ण	*exenamearg;	// exe file name when using exearg
    वर्ण	*exearg;	// argument for vim.exe or gvim.exe
    वर्ण	*oldbat;	// path to existing xxx.bat or NULL
    वर्ण	*oldexe;	// path to existing xxx.exe or NULL
    वर्ण	batpath[BUFSIZE];  // path of batch file to create; not
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
वर्ण *(icon_names[ICON_मोजणी]) =
	{"gVim " VIM_VERSION_SHORT,
	 "gVim Easy " VIM_VERSION_SHORT,
	 "gVim Read only " VIM_VERSION_SHORT};
वर्ण *(icon_link_names[ICON_मोजणी]) =
	{"gVim " VIM_VERSION_SHORT ".lnk",
	 "gVim Easy " VIM_VERSION_SHORT ".lnk",
	 "gVim Read only " VIM_VERSION_SHORT ".lnk"};

/* This is only used for dosinst.c. */
#यदि defined(DOSINST)
/*
 * Run an external command and wait for it to finish.
 */
    स्थिर शून्य
run_command(वर्ण *cmd)
{
    वर्ण	*cmd_path;
    वर्ण	cmd_buf[BUFSIZE * 2 + 35];
    वर्ण	*p;

    // On WinNT, 'start' is a shell built-in for cmd.exe rather than an
    // executable (start.exe) like in Win9x.
    cmd_path = searchpath_save("cmd.exe");
    यदि (cmd_path != रिक्त)
    {
	// There is a cmd.exe, so this might be Windows NT.  If it is,
	// we need to call cmd.exe explicitly.  If it is a later OS,
	// calling cmd.exe won't hurt if it is present.
	// Also, "start" on NT expects a window title argument.
	// Replace the slashes with backslashes.
	यावत् ((p = strchr(cmd_path, '/')) != रिक्त)
	    *p = '\\';
	sprintf(cmd_buf, "%s /c start \"vimcmd\" /wait %s", cmd_path, cmd);
	मुक्त(cmd_path);
    }
    अन्यथा
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
    शून्य
add_pathsep(वर्ण *name)
{
    पूर्णाङ्क		len = strlen(name);

    यदि (len > 0 && name[len - 1] != '\\' && name[len - 1] != '/')
	strcat(name, "\\");
}

/*
 * The normal chdir() does not change the default drive.  This one does.
 */
    पूर्णाङ्क
change_drive(पूर्णाङ्क drive)
{
    वर्ण temp[3] = "-:";
    temp[0] = (वर्ण)(drive + 'A' - 1);
    प्रत्यागम !SetCurrentDirectory(temp);
}

/*
 * Change directory to "path".
 * Return 0 for success, -1 for failure.
 */
    पूर्णाङ्क
mch_chdir(वर्ण *path)
{
    यदि (path[0] == NUL)		// just checking...
	प्रत्यागम 0;
    यदि (path[1] == ':')		// has a drive name
    {
	यदि (change_drive(mytoupper(path[0]) - 'A' + 1))
	    प्रत्यागम -1;		// invalid drive name
	path += 2;
    }
    यदि (*path == NUL)		// drive name only
	प्रत्यागम 0;
    प्रत्यागम chdir(path);		// let the normal chdir() do the rest
}

/*
 * Expand the executable name into a full path name.
 */
    स्थिर वर्ण *
my_fullpath(वर्ण *buf, वर्ण *fname UNUSED, पूर्णाङ्क len)
{
    // Only GetModuleFileName() will get the long file name path.
    // GetFullPathName() may still use the short (FAT) name.
    DWORD len_read = GetModuleFileName(रिक्त, buf, (size_t)len);

    प्रत्यागम (len_read > 0 && len_read < (DWORD)len) ? buf : रिक्त;
}

/*
 * Remove the tail from a file or directory name.
 * Puts a NUL on the last '/' or '\'.
 */
    स्थिर शून्य
remove_tail(वर्ण *path)
{
    पूर्णाङ्क		i;

    प्रति (i = strlen(path) - 1; i > 0; --i)
	यदि (path[i] == '/' || path[i] == '\\')
	{
	    path[i] = NUL;
	    भङ्ग;
	}
}


वर्ण	installdir[MAX_PATH-9];	// top of the installation dir, where the
				// install.exe is located, E.g.:
				// "c:\vim\vim60"
पूर्णाङ्क	runtimeidx;		// index in installdir[] where "vim60" starts
वर्ण	*sysdrive;		// system drive or "c:\"

/*
 * Setup for using this program.
 * Sets "installdir[]".
 */
    स्थिर शून्य
do_inits(वर्ण **argv)
{
    // Find out the full path of our executable.
    यदि (my_fullpath(installdir, argv[0], परिमाण(installdir)) == रिक्त)
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
    यदि (sysdrive == रिक्त || *sysdrive == NUL)
	sysdrive = "C:\\";
}
