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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ UNIX_LINT
# include <io.h>
# include <ctype.h>

# include <direct.h>

# include <windows.h>
# include <shlobj.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX_LINT
// Running lint on Unix: Some things are missing.
ಅಕ್ಷರ *searchpath(ಅಕ್ಷರ *name);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(UNIX_LINT)
# include <unistd.h>
# include <errno.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "version.h"

#ಆದರೆ defined(UNIX_LINT)
# define vim_mkdir(x, y) mkdir((ಅಕ್ಷರ *)(x), y)
#ಇಲ್ಲದಿದ್ದರೆ
# define vim_mkdir(x, y) _mkdir((ಅಕ್ಷರ *)(x))
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು sleep(n) Sleep((n) * 1000)

// ----------------------------------------


#ವ್ಯಾಖ್ಯಾನಿಸು BUFSIZE (MAX_ಮಾರ್ಗ*2)		// long enough to hold a file name path
#ವ್ಯಾಖ್ಯಾನಿಸು NUL 0

#ವ್ಯಾಖ್ಯಾನಿಸು FAIL 0
#ವ್ಯಾಖ್ಯಾನಿಸು OK 1

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FALSE
# define FALSE 0
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ TRUE
# define TRUE 1
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Modern way of creating registry entries, also works on 64 bit windows when
 * compiled as a 32 bit program.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಕೀಲಿ_WOW64_64_ಕೀಲಿ
# define ಕೀಲಿ_WOW64_64_ಕೀಲಿ 0x0100
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಕೀಲಿ_WOW64_32_ಕೀಲಿ
# define ಕೀಲಿ_WOW64_32_ಕೀಲಿ 0x0200
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __MINGW32__
# define ಬಳಸದ __attribute__((ಬಳಸದ_2))
#ಇಲ್ಲದಿದ್ದರೆ
# define ಬಳಸದ
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು VIM_STARTMENU "Programs\\Vim " VIM_ಆವೃತ್ತಿ_SHORT

ಪೂರ್ಣಾಂಕ	interactive;		// non-zero when running interactively

/*
 * Call malloc() and exit when out of memory.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ *
alloc(ಪೂರ್ಣಾಂಕ len)
{
    ಶೂನ್ಯ *p;

    p = ಸ್ಮರಣೆ_ಕೊಡು(len);
    ಆದರೆ (p == ಖಾಲಿ)
    {
	ಬರೆ("ERROR: out of memory\n");
	ನಿರ್ಗಮಿಸು(1);
    }
    ಹಿಂತಿರುಗಿ p;
}

/*
 * The toupper() in Bcc 5.5 doesn't work, use our own implementation.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
mytoupper(ಪೂರ್ಣಾಂಕ c)
{
    ಆದರೆ (c >= 'a' && c <= 'z')
	ಹಿಂತಿರುಗಿ c - 'a' + 'A';
    ಹಿಂತಿರುಗಿ c;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
myexit(ಪೂರ್ಣಾಂಕ n)
{
    ಆದರೆ (!interactive)
    {
	// Present a prompt, otherwise error messages can't be read.
	ಬರೆ("Press Enter to continue\n");
	rewind(stdin);
	(ಶೂನ್ಯ)getchar();
    }
    ನಿರ್ಗಮಿಸು(n);
}


ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ BOOL (WINAPI *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
/*
 * Check if this is a 64-bit OS.
 */
    ಸ್ಥಿರ BOOL
is_64bit_os(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN64
    ಹಿಂತಿರುಗಿ TRUE;
#ಇಲ್ಲದಿದ್ದರೆ
    BOOL bIsWow64 = FALSE;
    LPFN_ISWOW64PROCESS pIsWow64Process;

    pIsWow64Process = (LPFN_ISWOW64PROCESS)Get_Proc_ವಿಳಾಸ(
	    GetModuleHandle("kernel32"), "IsWow64Process");
    ಆದರೆ (pIsWow64Process != ಖಾಲಿ)
	pIsWow64Process(GetCurrentProcess(), &bIsWow64);
    ಹಿಂತಿರುಗಿ bIsWow64;
#ಆದರೆ_ಅಂತ್ಯ
}

    ಸ್ಥಿರ ಅಕ್ಷರ *
searchpath(ಅಕ್ಷರ *name)
{
    ಸ್ಥಿರ ಅಕ್ಷರ widename[2 * BUFSIZE];
    ಸ್ಥಿರ ಅಕ್ಷರ location[2 * BUFSIZE + 2];

    // There appears to be a bug in FindExecutableA() on Windows NT.
    // Use FindExecutableW() instead...
    Multi_Byte_ಗಮ್ಯ_Wide_Char(CP_ACP, 0, (LPCTSTR)name, -1,
	    (LPWSTR)widename, BUFSIZE);
    ಆದರೆ (ಕಂಡುಹಿಡಿ_Executable_W((LPCWSTR)widename, (LPCWSTR)"",
		(LPWSTR)location) > (HINSTANCE)32)
    {
	Wide_Char_ಗಮ್ಯ_Multi_Byte(CP_ACP, 0, (LPWSTR)location, -1,
		(LPSTR)widename, 2 * BUFSIZE, ಖಾಲಿ, ಖಾಲಿ);
	ಹಿಂತಿರುಗಿ widename;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Call searchpath() and save the result in allocated memory, or return NULL.
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
searchpath_save(ಅಕ್ಷರ *name)
{
    ಅಕ್ಷರ	*p;
    ಅಕ್ಷರ	*s;

    p = searchpath(name);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    s = alloc(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(p) + 1);
    strcpy(s, p);
    ಹಿಂತಿರುಗಿ s;
}


#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CSIDL_COMMON_PROGRAMS
# define CSIDL_COMMON_PROGRAMS 0x0017
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CSIDL_COMMON_DESKTOPDIRECTORY
# define CSIDL_COMMON_DESKTOPDIRECTORY 0x0019
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Get the path to a requested Windows shell folder.
 *
 * Return FAIL on error, OK on success
 */
    ಪೂರ್ಣಾಂಕ
get_shell_folder_ಮಾರ್ಗ(
	ಅಕ್ಷರ *shell_folder_ಮಾರ್ಗ,
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *shell_folder_name)
{
    /*
     * The following code was successfully built with make_mvc.mak.
     * The resulting executable worked on Windows 95, Millennium Edition, and
     * 2000 Professional.  But it was changed after testing...
     */
    LPITEMIDLIST    pidl = 0; // Pointer to an Item ID list allocated below
    LPMALLOC	    pMalloc;  // Pointer to an IMalloc interface
    ಪೂರ್ಣಾಂಕ		    csidl;
    ಪೂರ್ಣಾಂಕ		    alt_csidl = -1;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    desktop_csidl = -1;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    programs_csidl = -1;
    ಪೂರ್ಣಾಂಕ		    *pcsidl;
    ಪೂರ್ಣಾಂಕ		    r;

    ಆದರೆ (strcmp(shell_folder_name, "desktop") == 0)
    {
	pcsidl = &desktop_csidl;
	csidl = CSIDL_COMMON_DESKTOPDIRECTORY;
	alt_csidl = CSIDL_DESKTOP;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(shell_folder_name, "Programs", 8) == 0)
    {
	pcsidl = &programs_csidl;
	csidl = CSIDL_COMMON_PROGRAMS;
	alt_csidl = CSIDL_PROGRAMS;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಬರೆ("\nERROR (internal) unrecognised shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // Did this stuff before, use the same ID again.
    ಆದರೆ (*pcsidl >= 0)
    {
	csidl = *pcsidl;
	alt_csidl = -1;
    }

retry:
    // Initialize pointer to IMalloc interface
    ಆದರೆ (NOERROR != SHGetMalloc(&pMalloc))
    {
	ಬರೆ("\nERROR getting interface for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // Get an ITEMIDLIST corresponding to the folder code
    ಆದರೆ (NOERROR != SHGetSpecialFolderLocation(0, csidl, &pidl))
    {
	ಆದರೆ (alt_csidl < 0 || NOERROR != SHGetSpecialFolderLocation(0,
							    alt_csidl, &pidl))
	{
	    ಬರೆ("\nERROR getting ITEMIDLIST for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	csidl = alt_csidl;
	alt_csidl = -1;
    }

    // Translate that ITEMIDLIST to a string
    r = SHGet_ಮಾರ್ಗ_From_IDList(pidl, shell_folder_ಮಾರ್ಗ);

    // Free the data associated with pidl
    pMalloc->lpVtbl->Free(pMalloc, pidl);
    // Release the IMalloc interface
    pMalloc->lpVtbl->Release(pMalloc);

    ಆದರೆ (!r)
    {
	ಆದರೆ (alt_csidl >= 0)
	{
	    // We probably get here for Windows 95: the "all users"
	    // desktop/start menu entry doesn't exist.
	    csidl = alt_csidl;
	    alt_csidl = -1;
	    ಹೋಗು retry;
	}
	ಬರೆ("\nERROR translating ITEMIDLIST for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // If there is an alternative: verify we can write in this directory.
    // This should cause a retry when the "all users" directory exists but we
    // are a normal user and can't write there.
    ಆದರೆ (alt_csidl >= 0)
    {
	ಅಕ್ಷರ tbuf[BUFSIZE];
	FILE *fd;

	strcpy(tbuf, shell_folder_ಮಾರ್ಗ);
	strcat(tbuf, "\\vim write test");
	fd = fopen(tbuf, "w");
	ಆದರೆ (fd == ಖಾಲಿ)
	{
	    csidl = alt_csidl;
	    alt_csidl = -1;
	    ಹೋಗು retry;
	}
	fclose(fd);
	unlink(tbuf);
    }

    /*
     * Keep the found csidl for next time, so that we don't have to do the
     * write test every time.
     */
    ಆದರೆ (*pcsidl < 0)
	*pcsidl = csidl;

    ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(shell_folder_name, "Programs\\", 9) == 0)
	strcat(shell_folder_ಮಾರ್ಗ, shell_folder_name + 8);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * List of targets.  The first one (index zero) is used for the default path
 * for the batch files.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು ಗುರಿ_ಎಣಿಕೆ  9

ರಚನೆ
{
    ಅಕ್ಷರ	*name;		// Vim exe name (without .exe)
    ಅಕ್ಷರ	*batname;	// batch file name
    ಅಕ್ಷರ	*lnkname;	// shortcut file name
    ಅಕ್ಷರ	*exename;	// exe file name
    ಅಕ್ಷರ	*exenamearg;	// exe file name when using exearg
    ಅಕ್ಷರ	*exearg;	// argument for vim.exe or gvim.exe
    ಅಕ್ಷರ	*oldbat;	// path to existing xxx.bat or NULL
    ಅಕ್ಷರ	*oldexe;	// path to existing xxx.exe or NULL
    ಅಕ್ಷರ	batpath[BUFSIZE];  // path of batch file to create; not
				   // created when it's empty
} targets[ಗುರಿ_ಎಣಿಕೆ] =
{
    {"all",	"batch files", ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ""},
    {"vim",	"vim.bat",	"Vim.lnk",
			"vim.exe",    "vim.exe",  "", ಖಾಲಿ, ಖಾಲಿ, ""},
    {"gvim",	"gvim.bat",	"gVim.lnk",
			"gvim.exe",   "gvim.exe", "", ಖಾಲಿ, ಖಾಲಿ, ""},
    {"evim",	"evim.bat",	"gVim Easy.lnk",
			"evim.exe",   "gvim.exe", "-y", ಖಾಲಿ, ಖಾಲಿ, ""},
    {"view",	"view.bat",	"Vim Read-only.lnk",
			"view.exe",   "vim.exe",  "-R", ಖಾಲಿ, ಖಾಲಿ, ""},
    {"gview",	"gview.bat",	"gVim Read-only.lnk",
			"gview.exe",  "gvim.exe", "-R", ಖಾಲಿ, ಖಾಲಿ, ""},
    {"vimdiff", "vimdiff.bat",	"Vim Diff.lnk",
			"vimdiff.exe","vim.exe",  "-d", ಖಾಲಿ, ಖಾಲಿ, ""},
    {"gvimdiff","gvimdiff.bat",	"gVim Diff.lnk",
			"gvimdiff.exe","gvim.exe", "-d", ಖಾಲಿ, ಖಾಲಿ, ""},
    {"vimtutor","vimtutor.bat", "Vim tutor.lnk",
			"vimtutor.bat",  "vimtutor.bat", "", ಖಾಲಿ, ಖಾಲಿ, ""},
};

/* Uninstall key for vim.bat, etc. */
#ವ್ಯಾಖ್ಯಾನಿಸು VIMBAT_UNINSTKEY    "rem # uninstall key: " VIM_ಆವೃತ್ತಿ_NODOT " #"

#ವ್ಯಾಖ್ಯಾನಿಸು ICON_ಎಣಿಕೆ 3
ಅಕ್ಷರ *(icon_ಹೆಸರುಗಳು[ICON_ಎಣಿಕೆ]) =
	{"gVim " VIM_ಆವೃತ್ತಿ_SHORT,
	 "gVim Easy " VIM_ಆವೃತ್ತಿ_SHORT,
	 "gVim Read only " VIM_ಆವೃತ್ತಿ_SHORT};
ಅಕ್ಷರ *(icon_link_ಹೆಸರುಗಳು[ICON_ಎಣಿಕೆ]) =
	{"gVim " VIM_ಆವೃತ್ತಿ_SHORT ".lnk",
	 "gVim Easy " VIM_ಆವೃತ್ತಿ_SHORT ".lnk",
	 "gVim Read only " VIM_ಆವೃತ್ತಿ_SHORT ".lnk"};

/* This is only used for dosinst.c. */
#ಆದರೆ defined(DOSINST)
/*
 * Run an external command and wait for it to finish.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸತತ_command(ಅಕ್ಷರ *cmd)
{
    ಅಕ್ಷರ	*cmd_ಮಾರ್ಗ;
    ಅಕ್ಷರ	cmd_buf[BUFSIZE * 2 + 35];
    ಅಕ್ಷರ	*p;

    // On WinNT, 'start' is a shell built-in for cmd.exe rather than an
    // executable (start.exe) like in Win9x.
    cmd_ಮಾರ್ಗ = searchpath_save("cmd.exe");
    ಆದರೆ (cmd_ಮಾರ್ಗ != ಖಾಲಿ)
    {
	// There is a cmd.exe, so this might be Windows NT.  If it is,
	// we need to call cmd.exe explicitly.  If it is a later OS,
	// calling cmd.exe won't hurt if it is present.
	// Also, "start" on NT expects a window title argument.
	// Replace the slashes with backslashes.
	ಆಗ ((p = strchr(cmd_ಮಾರ್ಗ, '/')) != ಖಾಲಿ)
	    *p = '\\';
	sprintf(cmd_buf, "%s /c start \"vimcmd\" /wait %s", cmd_ಮಾರ್ಗ, cmd);
	ಬಿಡುಗಡೆ(cmd_ಮಾರ್ಗ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// No cmd.exe, just make the call and let the system handle it.
	sprintf(cmd_buf, "start /w %s", cmd);
    }
    system(cmd_buf);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Append a backslash to "name" if there isn't one yet.
 */
    ಶೂನ್ಯ
ಕೂಡಿಸು_pathsep(ಅಕ್ಷರ *name)
{
    ಪೂರ್ಣಾಂಕ		len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name);

    ಆದರೆ (len > 0 && name[len - 1] != '\\' && name[len - 1] != '/')
	strcat(name, "\\");
}

/*
 * The normal chdir() does not change the default drive.  This one does.
 */
    ಪೂರ್ಣಾಂಕ
change_drive(ಪೂರ್ಣಾಂಕ drive)
{
    ಅಕ್ಷರ ತಾತ್ಕಾಲಿಕ[3] = "-:";
    ತಾತ್ಕಾಲಿಕ[0] = (ಅಕ್ಷರ)(drive + 'A' - 1);
    ಹಿಂತಿರುಗಿ !ಗಣ_Current_Directory(ತಾತ್ಕಾಲಿಕ);
}

/*
 * Change directory to "path".
 * Return 0 for success, -1 for failure.
 */
    ಪೂರ್ಣಾಂಕ
mch_chdir(ಅಕ್ಷರ *ಮಾರ್ಗ)
{
    ಆದರೆ (ಮಾರ್ಗ[0] == NUL)		// just checking...
	ಹಿಂತಿರುಗಿ 0;
    ಆದರೆ (ಮಾರ್ಗ[1] == ':')		// has a drive name
    {
	ಆದರೆ (change_drive(mytoupper(ಮಾರ್ಗ[0]) - 'A' + 1))
	    ಹಿಂತಿರುಗಿ -1;		// invalid drive name
	ಮಾರ್ಗ += 2;
    }
    ಆದರೆ (*ಮಾರ್ಗ == NUL)		// drive name only
	ಹಿಂತಿರುಗಿ 0;
    ಹಿಂತಿರುಗಿ chdir(ಮಾರ್ಗ);		// let the normal chdir() do the rest
}

/*
 * Expand the executable name into a full path name.
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
my_fullpath(ಅಕ್ಷರ *buf, ಅಕ್ಷರ *fname ಬಳಸದ, ಪೂರ್ಣಾಂಕ len)
{
    // Only GetModuleFileName() will get the long file name path.
    // GetFullPathName() may still use the short (FAT) name.
    DWORD len_read = GetModuleFileName(ಖಾಲಿ, buf, (size_t)len);

    ಹಿಂತಿರುಗಿ (len_read > 0 && len_read < (DWORD)len) ? buf : ಖಾಲಿ;
}

/*
 * Remove the tail from a file or directory name.
 * Puts a NUL on the last '/' or '\'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
remove_ಬಾಲ(ಅಕ್ಷರ *ಮಾರ್ಗ)
{
    ಪೂರ್ಣಾಂಕ		i;

    ಗಾಗಿ (i = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮಾರ್ಗ) - 1; i > 0; --i)
	ಆದರೆ (ಮಾರ್ಗ[i] == '/' || ಮಾರ್ಗ[i] == '\\')
	{
	    ಮಾರ್ಗ[i] = NUL;
	    ನಿಲ್ಲಿಸು;
	}
}


ಅಕ್ಷರ	installdir[MAX_ಮಾರ್ಗ-9];	// top of the installation dir, where the
				// install.exe is located, E.g.:
				// "c:\vim\vim60"
ಪೂರ್ಣಾಂಕ	runtimeidx;		// index in installdir[] where "vim60" starts
ಅಕ್ಷರ	*sysdrive;		// system drive or "c:\"

/*
 * Setup for using this program.
 * Sets "installdir[]".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
do_inits(ಅಕ್ಷರ **argv)
{
    // Find out the full path of our executable.
    ಆದರೆ (my_fullpath(installdir, argv[0], ಗಾತ್ರ(installdir)) == ಖಾಲಿ)
    {
	ಬರೆ("ERROR: Cannot get name of executable\n");
	myexit(1);
    }
    // remove the tail, the executable name "install.exe"
    remove_ಬಾಲ(installdir);

    // change to the installdir
    mch_chdir(installdir);

    // Find the system drive.  Only used for searching the Vim executable, not
    // very important.
    sysdrive = getenv("SYSTEMDRIVE");
    ಆದರೆ (sysdrive == ಖಾಲಿ || *sysdrive == NUL)
	sysdrive = "C:\\";
}
