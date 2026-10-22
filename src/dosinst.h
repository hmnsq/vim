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

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ UNIX_LINT
# include <io.h>
# include <ctype.h>

# include <direct.h>

# include <windows.h>
# include <shlobj.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX_LINT
// Running lint on Unix: Some things are missing.
ਅੱਖਰ *searchpath(ਅੱਖਰ *name);
#ਜੇ_ਅੰਤ

#ਜੇ defined(UNIX_LINT)
# include <unistd.h>
# include <errno.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "version.h"

#ਜੇ defined(UNIX_LINT)
# define vim_mkdir(x, y) mkdir((ਅੱਖਰ *)(x), y)
#ਨਹੀਂਤਾਂ
# define vim_mkdir(x, y) _mkdir((ਅੱਖਰ *)(x))
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ sleep(n) Sleep((n) * 1000)

// ----------------------------------------


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BUFSIZE (MAX_ਰਸਤਾ*2)		// long enough to hold a file name path
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NUL 0

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FAIL 0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OK 1

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FALSE
# define FALSE 0
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ TRUE
# define TRUE 1
#ਜੇ_ਅੰਤ

/*
 * Modern way of creating registry entries, also works on 64 bit windows when
 * compiled as a 32 bit program.
 */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਕੁੰਜੀ_WOW64_64_ਕੁੰਜੀ
# define ਕੁੰਜੀ_WOW64_64_ਕੁੰਜੀ 0x0100
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਕੁੰਜੀ_WOW64_32_ਕੁੰਜੀ
# define ਕੁੰਜੀ_WOW64_32_ਕੁੰਜੀ 0x0200
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __MINGW32__
# define ਅਣਵਰਤਿਆ __attribute__((ਅਣਵਰਤਿਆ_2))
#ਨਹੀਂਤਾਂ
# define ਅਣਵਰਤਿਆ
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_STARTMENU "Programs\\Vim " VIM_ਸੰਸਕਰਣ_SHORT

ਪੂਰਨ	interactive;		// non-zero when running interactively

/*
 * Call malloc() and exit when out of memory.
 */
    ਸਥਿਰ ਖਾਲੀ *
alloc(ਪੂਰਨ len)
{
    ਖਾਲੀ *p;

    p = ਸਮ੍ਰਿਤੀ_ਦਿਓ(len);
    ਜੇ (p == ਨਿਰਾ)
    {
	ਲਿਖੋ("ERROR: out of memory\n");
	ਬਾਹਰ_ਨਿਕਲੋ(1);
    }
    ਵਾਪਸ p;
}

/*
 * The toupper() in Bcc 5.5 doesn't work, use our own implementation.
 */
    ਸਥਿਰ ਪੂਰਨ
mytoupper(ਪੂਰਨ c)
{
    ਜੇ (c >= 'a' && c <= 'z')
	ਵਾਪਸ c - 'a' + 'A';
    ਵਾਪਸ c;
}

    ਸਥਿਰ ਖਾਲੀ
myexit(ਪੂਰਨ n)
{
    ਜੇ (!interactive)
    {
	// Present a prompt, otherwise error messages can't be read.
	ਲਿਖੋ("Press Enter to continue\n");
	rewind(stdin);
	(ਖਾਲੀ)getchar();
    }
    ਬਾਹਰ_ਨਿਕਲੋ(n);
}


ਕਿਸਮਪਰਿਭਾਸ਼ਾ BOOL (WINAPI *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
/*
 * Check if this is a 64-bit OS.
 */
    ਸਥਿਰ BOOL
is_64bit_os(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN64
    ਵਾਪਸ TRUE;
#ਨਹੀਂਤਾਂ
    BOOL bIsWow64 = FALSE;
    LPFN_ISWOW64PROCESS pIsWow64Process;

    pIsWow64Process = (LPFN_ISWOW64PROCESS)Get_Proc_ਪਤਾ(
	    GetModuleHandle("kernel32"), "IsWow64Process");
    ਜੇ (pIsWow64Process != ਨਿਰਾ)
	pIsWow64Process(GetCurrentProcess(), &bIsWow64);
    ਵਾਪਸ bIsWow64;
#ਜੇ_ਅੰਤ
}

    ਸਥਿਰ ਅੱਖਰ *
searchpath(ਅੱਖਰ *name)
{
    ਸਥਿਰ ਅੱਖਰ widename[2 * BUFSIZE];
    ਸਥਿਰ ਅੱਖਰ location[2 * BUFSIZE + 2];

    // There appears to be a bug in FindExecutableA() on Windows NT.
    // Use FindExecutableW() instead...
    Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char(CP_ACP, 0, (LPCTSTR)name, -1,
	    (LPWSTR)widename, BUFSIZE);
    ਜੇ (ਲੱਭੋ_Executable_W((LPCWSTR)widename, (LPCWSTR)"",
		(LPWSTR)location) > (HINSTANCE)32)
    {
	Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte(CP_ACP, 0, (LPWSTR)location, -1,
		(LPSTR)widename, 2 * BUFSIZE, ਨਿਰਾ, ਨਿਰਾ);
	ਵਾਪਸ widename;
    }
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Call searchpath() and save the result in allocated memory, or return NULL.
 */
    ਸਥਿਰ ਅੱਖਰ *
searchpath_save(ਅੱਖਰ *name)
{
    ਅੱਖਰ	*p;
    ਅੱਖਰ	*s;

    p = searchpath(name);
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    s = alloc(ਲੜੀ_ਲੰਬਾਈ(p) + 1);
    strcpy(s, p);
    ਵਾਪਸ s;
}


#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CSIDL_COMMON_PROGRAMS
# define CSIDL_COMMON_PROGRAMS 0x0017
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CSIDL_COMMON_DESKTOPDIRECTORY
# define CSIDL_COMMON_DESKTOPDIRECTORY 0x0019
#ਜੇ_ਅੰਤ

/*
 * Get the path to a requested Windows shell folder.
 *
 * Return FAIL on error, OK on success
 */
    ਪੂਰਨ
get_shell_folder_ਰਸਤਾ(
	ਅੱਖਰ *shell_folder_ਰਸਤਾ,
	ਅਚਲ ਅੱਖਰ *shell_folder_name)
{
    /*
     * The following code was successfully built with make_mvc.mak.
     * The resulting executable worked on Windows 95, Millennium Edition, and
     * 2000 Professional.  But it was changed after testing...
     */
    LPITEMIDLIST    pidl = 0; // Pointer to an Item ID list allocated below
    LPMALLOC	    pMalloc;  // Pointer to an IMalloc interface
    ਪੂਰਨ		    csidl;
    ਪੂਰਨ		    alt_csidl = -1;
    ਸਥਿਰ ਪੂਰਨ	    desktop_csidl = -1;
    ਸਥਿਰ ਪੂਰਨ	    programs_csidl = -1;
    ਪੂਰਨ		    *pcsidl;
    ਪੂਰਨ		    r;

    ਜੇ (strcmp(shell_folder_name, "desktop") == 0)
    {
	pcsidl = &desktop_csidl;
	csidl = CSIDL_COMMON_DESKTOPDIRECTORY;
	alt_csidl = CSIDL_DESKTOP;
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਲੜੀ_ਤੁਲਨਾ(shell_folder_name, "Programs", 8) == 0)
    {
	pcsidl = &programs_csidl;
	csidl = CSIDL_COMMON_PROGRAMS;
	alt_csidl = CSIDL_PROGRAMS;
    }
    ਨਹੀਂਤਾਂ
    {
	ਲਿਖੋ("\nERROR (internal) unrecognised shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	ਵਾਪਸ FAIL;
    }

    // Did this stuff before, use the same ID again.
    ਜੇ (*pcsidl >= 0)
    {
	csidl = *pcsidl;
	alt_csidl = -1;
    }

retry:
    // Initialize pointer to IMalloc interface
    ਜੇ (NOERROR != SHGetMalloc(&pMalloc))
    {
	ਲਿਖੋ("\nERROR getting interface for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	ਵਾਪਸ FAIL;
    }

    // Get an ITEMIDLIST corresponding to the folder code
    ਜੇ (NOERROR != SHGetSpecialFolderLocation(0, csidl, &pidl))
    {
	ਜੇ (alt_csidl < 0 || NOERROR != SHGetSpecialFolderLocation(0,
							    alt_csidl, &pidl))
	{
	    ਲਿਖੋ("\nERROR getting ITEMIDLIST for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	    ਵਾਪਸ FAIL;
	}
	csidl = alt_csidl;
	alt_csidl = -1;
    }

    // Translate that ITEMIDLIST to a string
    r = SHGet_ਰਸਤਾ_From_IDList(pidl, shell_folder_ਰਸਤਾ);

    // Free the data associated with pidl
    pMalloc->lpVtbl->Free(pMalloc, pidl);
    // Release the IMalloc interface
    pMalloc->lpVtbl->Release(pMalloc);

    ਜੇ (!r)
    {
	ਜੇ (alt_csidl >= 0)
	{
	    // We probably get here for Windows 95: the "all users"
	    // desktop/start menu entry doesn't exist.
	    csidl = alt_csidl;
	    alt_csidl = -1;
	    ਜਾਓ retry;
	}
	ਲਿਖੋ("\nERROR translating ITEMIDLIST for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	ਵਾਪਸ FAIL;
    }

    // If there is an alternative: verify we can write in this directory.
    // This should cause a retry when the "all users" directory exists but we
    // are a normal user and can't write there.
    ਜੇ (alt_csidl >= 0)
    {
	ਅੱਖਰ tbuf[BUFSIZE];
	FILE *fd;

	strcpy(tbuf, shell_folder_ਰਸਤਾ);
	strcat(tbuf, "\\vim write test");
	fd = fopen(tbuf, "w");
	ਜੇ (fd == ਨਿਰਾ)
	{
	    csidl = alt_csidl;
	    alt_csidl = -1;
	    ਜਾਓ retry;
	}
	fclose(fd);
	unlink(tbuf);
    }

    /*
     * Keep the found csidl for next time, so that we don't have to do the
     * write test every time.
     */
    ਜੇ (*pcsidl < 0)
	*pcsidl = csidl;

    ਜੇ (ਲੜੀ_ਤੁਲਨਾ(shell_folder_name, "Programs\\", 9) == 0)
	strcat(shell_folder_ਰਸਤਾ, shell_folder_name + 8);

    ਵਾਪਸ OK;
}

/*
 * List of targets.  The first one (index zero) is used for the default path
 * for the batch files.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਨਿਸ਼ਾਨਾ_ਗਿਣਤੀ  9

ਬਣਤਰ
{
    ਅੱਖਰ	*name;		// Vim exe name (without .exe)
    ਅੱਖਰ	*batname;	// batch file name
    ਅੱਖਰ	*lnkname;	// shortcut file name
    ਅੱਖਰ	*exename;	// exe file name
    ਅੱਖਰ	*exenamearg;	// exe file name when using exearg
    ਅੱਖਰ	*exearg;	// argument for vim.exe or gvim.exe
    ਅੱਖਰ	*oldbat;	// path to existing xxx.bat or NULL
    ਅੱਖਰ	*oldexe;	// path to existing xxx.exe or NULL
    ਅੱਖਰ	batpath[BUFSIZE];  // path of batch file to create; not
				   // created when it's empty
} targets[ਨਿਸ਼ਾਨਾ_ਗਿਣਤੀ] =
{
    {"all",	"batch files", ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ""},
    {"vim",	"vim.bat",	"Vim.lnk",
			"vim.exe",    "vim.exe",  "", ਨਿਰਾ, ਨਿਰਾ, ""},
    {"gvim",	"gvim.bat",	"gVim.lnk",
			"gvim.exe",   "gvim.exe", "", ਨਿਰਾ, ਨਿਰਾ, ""},
    {"evim",	"evim.bat",	"gVim Easy.lnk",
			"evim.exe",   "gvim.exe", "-y", ਨਿਰਾ, ਨਿਰਾ, ""},
    {"view",	"view.bat",	"Vim Read-only.lnk",
			"view.exe",   "vim.exe",  "-R", ਨਿਰਾ, ਨਿਰਾ, ""},
    {"gview",	"gview.bat",	"gVim Read-only.lnk",
			"gview.exe",  "gvim.exe", "-R", ਨਿਰਾ, ਨਿਰਾ, ""},
    {"vimdiff", "vimdiff.bat",	"Vim Diff.lnk",
			"vimdiff.exe","vim.exe",  "-d", ਨਿਰਾ, ਨਿਰਾ, ""},
    {"gvimdiff","gvimdiff.bat",	"gVim Diff.lnk",
			"gvimdiff.exe","gvim.exe", "-d", ਨਿਰਾ, ਨਿਰਾ, ""},
    {"vimtutor","vimtutor.bat", "Vim tutor.lnk",
			"vimtutor.bat",  "vimtutor.bat", "", ਨਿਰਾ, ਨਿਰਾ, ""},
};

/* Uninstall key for vim.bat, etc. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIMBAT_UNINSTKEY    "rem # uninstall key: " VIM_ਸੰਸਕਰਣ_NODOT " #"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ICON_ਗਿਣਤੀ 3
ਅੱਖਰ *(icon_ਨਾਂ[ICON_ਗਿਣਤੀ]) =
	{"gVim " VIM_ਸੰਸਕਰਣ_SHORT,
	 "gVim Easy " VIM_ਸੰਸਕਰਣ_SHORT,
	 "gVim Read only " VIM_ਸੰਸਕਰਣ_SHORT};
ਅੱਖਰ *(icon_link_ਨਾਂ[ICON_ਗਿਣਤੀ]) =
	{"gVim " VIM_ਸੰਸਕਰਣ_SHORT ".lnk",
	 "gVim Easy " VIM_ਸੰਸਕਰਣ_SHORT ".lnk",
	 "gVim Read only " VIM_ਸੰਸਕਰਣ_SHORT ".lnk"};

/* This is only used for dosinst.c. */
#ਜੇ defined(DOSINST)
/*
 * Run an external command and wait for it to finish.
 */
    ਸਥਿਰ ਖਾਲੀ
ਲੜੀਵਾਰ_command(ਅੱਖਰ *cmd)
{
    ਅੱਖਰ	*cmd_ਰਸਤਾ;
    ਅੱਖਰ	cmd_buf[BUFSIZE * 2 + 35];
    ਅੱਖਰ	*p;

    // On WinNT, 'start' is a shell built-in for cmd.exe rather than an
    // executable (start.exe) like in Win9x.
    cmd_ਰਸਤਾ = searchpath_save("cmd.exe");
    ਜੇ (cmd_ਰਸਤਾ != ਨਿਰਾ)
    {
	// There is a cmd.exe, so this might be Windows NT.  If it is,
	// we need to call cmd.exe explicitly.  If it is a later OS,
	// calling cmd.exe won't hurt if it is present.
	// Also, "start" on NT expects a window title argument.
	// Replace the slashes with backslashes.
	ਜਦੋਂ ((p = strchr(cmd_ਰਸਤਾ, '/')) != ਨਿਰਾ)
	    *p = '\\';
	sprintf(cmd_buf, "%s /c start \"vimcmd\" /wait %s", cmd_ਰਸਤਾ, cmd);
	ਮੁਕਤ(cmd_ਰਸਤਾ);
    }
    ਨਹੀਂਤਾਂ
    {
	// No cmd.exe, just make the call and let the system handle it.
	sprintf(cmd_buf, "start /w %s", cmd);
    }
    system(cmd_buf);
}
#ਜੇ_ਅੰਤ

/*
 * Append a backslash to "name" if there isn't one yet.
 */
    ਖਾਲੀ
ਜੋੜੋ_pathsep(ਅੱਖਰ *name)
{
    ਪੂਰਨ		len = ਲੜੀ_ਲੰਬਾਈ(name);

    ਜੇ (len > 0 && name[len - 1] != '\\' && name[len - 1] != '/')
	strcat(name, "\\");
}

/*
 * The normal chdir() does not change the default drive.  This one does.
 */
    ਪੂਰਨ
change_drive(ਪੂਰਨ drive)
{
    ਅੱਖਰ ਆਰਜ਼ੀ[3] = "-:";
    ਆਰਜ਼ੀ[0] = (ਅੱਖਰ)(drive + 'A' - 1);
    ਵਾਪਸ !ਸੈੱਟ_Current_Directory(ਆਰਜ਼ੀ);
}

/*
 * Change directory to "path".
 * Return 0 for success, -1 for failure.
 */
    ਪੂਰਨ
mch_chdir(ਅੱਖਰ *ਰਸਤਾ)
{
    ਜੇ (ਰਸਤਾ[0] == NUL)		// just checking...
	ਵਾਪਸ 0;
    ਜੇ (ਰਸਤਾ[1] == ':')		// has a drive name
    {
	ਜੇ (change_drive(mytoupper(ਰਸਤਾ[0]) - 'A' + 1))
	    ਵਾਪਸ -1;		// invalid drive name
	ਰਸਤਾ += 2;
    }
    ਜੇ (*ਰਸਤਾ == NUL)		// drive name only
	ਵਾਪਸ 0;
    ਵਾਪਸ chdir(ਰਸਤਾ);		// let the normal chdir() do the rest
}

/*
 * Expand the executable name into a full path name.
 */
    ਸਥਿਰ ਅੱਖਰ *
my_fullpath(ਅੱਖਰ *buf, ਅੱਖਰ *fname ਅਣਵਰਤਿਆ, ਪੂਰਨ len)
{
    // Only GetModuleFileName() will get the long file name path.
    // GetFullPathName() may still use the short (FAT) name.
    DWORD len_read = GetModuleFileName(ਨਿਰਾ, buf, (size_t)len);

    ਵਾਪਸ (len_read > 0 && len_read < (DWORD)len) ? buf : ਨਿਰਾ;
}

/*
 * Remove the tail from a file or directory name.
 * Puts a NUL on the last '/' or '\'.
 */
    ਸਥਿਰ ਖਾਲੀ
remove_ਪੂਛ(ਅੱਖਰ *ਰਸਤਾ)
{
    ਪੂਰਨ		i;

    ਲਈ (i = ਲੜੀ_ਲੰਬਾਈ(ਰਸਤਾ) - 1; i > 0; --i)
	ਜੇ (ਰਸਤਾ[i] == '/' || ਰਸਤਾ[i] == '\\')
	{
	    ਰਸਤਾ[i] = NUL;
	    ਰੋਕੋ;
	}
}


ਅੱਖਰ	installdir[MAX_ਰਸਤਾ-9];	// top of the installation dir, where the
				// install.exe is located, E.g.:
				// "c:\vim\vim60"
ਪੂਰਨ	runtimeidx;		// index in installdir[] where "vim60" starts
ਅੱਖਰ	*sysdrive;		// system drive or "c:\"

/*
 * Setup for using this program.
 * Sets "installdir[]".
 */
    ਸਥਿਰ ਖਾਲੀ
do_inits(ਅੱਖਰ **argv)
{
    // Find out the full path of our executable.
    ਜੇ (my_fullpath(installdir, argv[0], ਆਕਾਰ(installdir)) == ਨਿਰਾ)
    {
	ਲਿਖੋ("ERROR: Cannot get name of executable\n");
	myexit(1);
    }
    // remove the tail, the executable name "install.exe"
    remove_ਪੂਛ(installdir);

    // change to the installdir
    mch_chdir(installdir);

    // Find the system drive.  Only used for searching the Vim executable, not
    // very important.
    sysdrive = getenv("SYSTEMDRIVE");
    ਜੇ (sysdrive == ਨਿਰਾ || *sysdrive == NUL)
	sysdrive = "C:\\";
}
