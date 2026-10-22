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

#اگر_غیر_تعریف_شدہ UNIX_LINT
# include <io.h>
# include <ctype.h>

# include <direct.h>

# include <windows.h>
# include <shlobj.h>
#اگر_ختم

#اگر_تعریف_شدہ UNIX_LINT
// Running lint on Unix: Some things are missing.
حرف *searchpath(حرف *name);
#اگر_ختم

#اگر defined(UNIX_LINT)
# include <unistd.h>
# include <errno.h>
#اگر_ختم

#شامل "version.h"

#اگر defined(UNIX_LINT)
# define vim_mkdir(x, y) mkdir((حرف *)(x), y)
#ورنہ
# define vim_mkdir(x, y) _mkdir((حرف *)(x))
#اگر_ختم

#تعریف_کرو sleep(n) Sleep((n) * 1000)

// ----------------------------------------


#تعریف_کرو BUFSIZE (MAX_راستہ*2)		// long enough to hold a file name path
#تعریف_کرو NUL 0

#تعریف_کرو FAIL 0
#تعریف_کرو OK 1

#اگر_غیر_تعریف_شدہ FALSE
# define FALSE 0
#اگر_ختم
#اگر_غیر_تعریف_شدہ TRUE
# define TRUE 1
#اگر_ختم

/*
 * Modern way of creating registry entries, also works on 64 bit windows when
 * compiled as a 32 bit program.
 */
#اگر_غیر_تعریف_شدہ کلید_WOW64_64_کلید
# define کلید_WOW64_64_کلید 0x0100
#اگر_ختم
#اگر_غیر_تعریف_شدہ کلید_WOW64_32_کلید
# define کلید_WOW64_32_کلید 0x0200
#اگر_ختم

#اگر_تعریف_شدہ __MINGW32__
# define غیر_مستعمل __attribute__((غیر_مستعمل_2))
#ورنہ
# define غیر_مستعمل
#اگر_ختم

#تعریف_کرو VIM_STARTMENU "Programs\\Vim " VIM_نسخہ_SHORT

صحیح	interactive;		// non-zero when running interactively

/*
 * Call malloc() and exit when out of memory.
 */
    ساکن خالی *
alloc(صحیح len)
{
    خالی *p;

    p = حافظہ_دو(len);
    اگر (p == تہی)
    {
	لکھو("ERROR: out of memory\n");
	باہر_نکلو(1);
    }
    واپس p;
}

/*
 * The toupper() in Bcc 5.5 doesn't work, use our own implementation.
 */
    ساکن صحیح
mytoupper(صحیح c)
{
    اگر (c >= 'a' && c <= 'z')
	واپس c - 'a' + 'A';
    واپس c;
}

    ساکن خالی
myexit(صحیح n)
{
    اگر (!interactive)
    {
	// Present a prompt, otherwise error messages can't be read.
	لکھو("Press Enter to continue\n");
	rewind(stdin);
	(خالی)getchar();
    }
    باہر_نکلو(n);
}


قسمتعریف BOOL (WINAPI *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
/*
 * Check if this is a 64-bit OS.
 */
    ساکن BOOL
is_64bit_os(خالی)
{
#اگر_تعریف_شدہ _WIN64
    واپس TRUE;
#ورنہ
    BOOL bIsWow64 = FALSE;
    LPFN_ISWOW64PROCESS pIsWow64Process;

    pIsWow64Process = (LPFN_ISWOW64PROCESS)Get_Proc_پتہ(
	    GetModuleHandle("kernel32"), "IsWow64Process");
    اگر (pIsWow64Process != تہی)
	pIsWow64Process(GetCurrentProcess(), &bIsWow64);
    واپس bIsWow64;
#اگر_ختم
}

    ساکن حرف *
searchpath(حرف *name)
{
    ساکن حرف widename[2 * BUFSIZE];
    ساکن حرف location[2 * BUFSIZE + 2];

    // There appears to be a bug in FindExecutableA() on Windows NT.
    // Use FindExecutableW() instead...
    Multi_Byte_مقصد_Wide_Char(CP_ACP, 0, (LPCTSTR)name, -1,
	    (LPWSTR)widename, BUFSIZE);
    اگر (ڈھونڈو_Executable_W((LPCWSTR)widename, (LPCWSTR)"",
		(LPWSTR)location) > (HINSTANCE)32)
    {
	Wide_Char_مقصد_Multi_Byte(CP_ACP, 0, (LPWSTR)location, -1,
		(LPSTR)widename, 2 * BUFSIZE, تہی, تہی);
	واپس widename;
    }
    واپس تہی;
}

/*
 * Call searchpath() and save the result in allocated memory, or return NULL.
 */
    ساکن حرف *
searchpath_save(حرف *name)
{
    حرف	*p;
    حرف	*s;

    p = searchpath(name);
    اگر (p == تہی)
	واپس تہی;
    s = alloc(لڑی_لمبائی(p) + 1);
    strcpy(s, p);
    واپس s;
}


#اگر_غیر_تعریف_شدہ CSIDL_COMMON_PROGRAMS
# define CSIDL_COMMON_PROGRAMS 0x0017
#اگر_ختم
#اگر_غیر_تعریف_شدہ CSIDL_COMMON_DESKTOPDIRECTORY
# define CSIDL_COMMON_DESKTOPDIRECTORY 0x0019
#اگر_ختم

/*
 * Get the path to a requested Windows shell folder.
 *
 * Return FAIL on error, OK on success
 */
    صحیح
get_shell_folder_راستہ(
	حرف *shell_folder_راستہ,
	ثابت حرف *shell_folder_name)
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
    ساکن صحیح	    desktop_csidl = -1;
    ساکن صحیح	    programs_csidl = -1;
    صحیح		    *pcsidl;
    صحیح		    r;

    اگر (strcmp(shell_folder_name, "desktop") == 0)
    {
	pcsidl = &desktop_csidl;
	csidl = CSIDL_COMMON_DESKTOPDIRECTORY;
	alt_csidl = CSIDL_DESKTOP;
    }
    ورنہ اگر (لڑی_موازنہ(shell_folder_name, "Programs", 8) == 0)
    {
	pcsidl = &programs_csidl;
	csidl = CSIDL_COMMON_PROGRAMS;
	alt_csidl = CSIDL_PROGRAMS;
    }
    ورنہ
    {
	لکھو("\nERROR (internal) unrecognised shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	واپس FAIL;
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
	لکھو("\nERROR getting interface for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	واپس FAIL;
    }

    // Get an ITEMIDLIST corresponding to the folder code
    اگر (NOERROR != SHGetSpecialFolderLocation(0, csidl, &pidl))
    {
	اگر (alt_csidl < 0 || NOERROR != SHGetSpecialFolderLocation(0,
							    alt_csidl, &pidl))
	{
	    لکھو("\nERROR getting ITEMIDLIST for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	    واپس FAIL;
	}
	csidl = alt_csidl;
	alt_csidl = -1;
    }

    // Translate that ITEMIDLIST to a string
    r = SHGet_راستہ_From_IDList(pidl, shell_folder_راستہ);

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
	    جاؤ retry;
	}
	لکھو("\nERROR translating ITEMIDLIST for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	واپس FAIL;
    }

    // If there is an alternative: verify we can write in this directory.
    // This should cause a retry when the "all users" directory exists but we
    // are a normal user and can't write there.
    اگر (alt_csidl >= 0)
    {
	حرف tbuf[BUFSIZE];
	FILE *fd;

	strcpy(tbuf, shell_folder_راستہ);
	strcat(tbuf, "\\vim write test");
	fd = fopen(tbuf, "w");
	اگر (fd == تہی)
	{
	    csidl = alt_csidl;
	    alt_csidl = -1;
	    جاؤ retry;
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

    اگر (لڑی_موازنہ(shell_folder_name, "Programs\\", 9) == 0)
	strcat(shell_folder_راستہ, shell_folder_name + 8);

    واپس OK;
}

/*
 * List of targets.  The first one (index zero) is used for the default path
 * for the batch files.
 */
#تعریف_کرو ہدف_گنتی  9

ساخت
{
    حرف	*name;		// Vim exe name (without .exe)
    حرف	*batname;	// batch file name
    حرف	*lnkname;	// shortcut file name
    حرف	*exename;	// exe file name
    حرف	*exenamearg;	// exe file name when using exearg
    حرف	*exearg;	// argument for vim.exe or gvim.exe
    حرف	*oldbat;	// path to existing xxx.bat or NULL
    حرف	*oldexe;	// path to existing xxx.exe or NULL
    حرف	batpath[BUFSIZE];  // path of batch file to create; not
				   // created when it's empty
} targets[ہدف_گنتی] =
{
    {"all",	"batch files", تہی, تہی, تہی, تہی, تہی, تہی, ""},
    {"vim",	"vim.bat",	"Vim.lnk",
			"vim.exe",    "vim.exe",  "", تہی, تہی, ""},
    {"gvim",	"gvim.bat",	"gVim.lnk",
			"gvim.exe",   "gvim.exe", "", تہی, تہی, ""},
    {"evim",	"evim.bat",	"gVim Easy.lnk",
			"evim.exe",   "gvim.exe", "-y", تہی, تہی, ""},
    {"view",	"view.bat",	"Vim Read-only.lnk",
			"view.exe",   "vim.exe",  "-R", تہی, تہی, ""},
    {"gview",	"gview.bat",	"gVim Read-only.lnk",
			"gview.exe",  "gvim.exe", "-R", تہی, تہی, ""},
    {"vimdiff", "vimdiff.bat",	"Vim Diff.lnk",
			"vimdiff.exe","vim.exe",  "-d", تہی, تہی, ""},
    {"gvimdiff","gvimdiff.bat",	"gVim Diff.lnk",
			"gvimdiff.exe","gvim.exe", "-d", تہی, تہی, ""},
    {"vimtutor","vimtutor.bat", "Vim tutor.lnk",
			"vimtutor.bat",  "vimtutor.bat", "", تہی, تہی, ""},
};

/* Uninstall key for vim.bat, etc. */
#تعریف_کرو VIMBAT_UNINSTKEY    "rem # uninstall key: " VIM_نسخہ_NODOT " #"

#تعریف_کرو ICON_گنتی 3
حرف *(icon_نام[ICON_گنتی]) =
	{"gVim " VIM_نسخہ_SHORT,
	 "gVim Easy " VIM_نسخہ_SHORT,
	 "gVim Read only " VIM_نسخہ_SHORT};
حرف *(icon_link_نام[ICON_گنتی]) =
	{"gVim " VIM_نسخہ_SHORT ".lnk",
	 "gVim Easy " VIM_نسخہ_SHORT ".lnk",
	 "gVim Read only " VIM_نسخہ_SHORT ".lnk"};

/* This is only used for dosinst.c. */
#اگر defined(DOSINST)
/*
 * Run an external command and wait for it to finish.
 */
    ساکن خالی
تسلسل_command(حرف *cmd)
{
    حرف	*cmd_راستہ;
    حرف	cmd_buf[BUFSIZE * 2 + 35];
    حرف	*p;

    // On WinNT, 'start' is a shell built-in for cmd.exe rather than an
    // executable (start.exe) like in Win9x.
    cmd_راستہ = searchpath_save("cmd.exe");
    اگر (cmd_راستہ != تہی)
    {
	// There is a cmd.exe, so this might be Windows NT.  If it is,
	// we need to call cmd.exe explicitly.  If it is a later OS,
	// calling cmd.exe won't hurt if it is present.
	// Also, "start" on NT expects a window title argument.
	// Replace the slashes with backslashes.
	جب ((p = strchr(cmd_راستہ, '/')) != تہی)
	    *p = '\\';
	sprintf(cmd_buf, "%s /c start \"vimcmd\" /wait %s", cmd_راستہ, cmd);
	آزاد(cmd_راستہ);
    }
    ورنہ
    {
	// No cmd.exe, just make the call and let the system handle it.
	sprintf(cmd_buf, "start /w %s", cmd);
    }
    system(cmd_buf);
}
#اگر_ختم

/*
 * Append a backslash to "name" if there isn't one yet.
 */
    خالی
جوڑو_pathsep(حرف *name)
{
    صحیح		len = لڑی_لمبائی(name);

    اگر (len > 0 && name[len - 1] != '\\' && name[len - 1] != '/')
	strcat(name, "\\");
}

/*
 * The normal chdir() does not change the default drive.  This one does.
 */
    صحیح
change_drive(صحیح drive)
{
    حرف عارضی[3] = "-:";
    عارضی[0] = (حرف)(drive + 'A' - 1);
    واپس !سیٹ_Current_Directory(عارضی);
}

/*
 * Change directory to "path".
 * Return 0 for success, -1 for failure.
 */
    صحیح
mch_chdir(حرف *راستہ)
{
    اگر (راستہ[0] == NUL)		// just checking...
	واپس 0;
    اگر (راستہ[1] == ':')		// has a drive name
    {
	اگر (change_drive(mytoupper(راستہ[0]) - 'A' + 1))
	    واپس -1;		// invalid drive name
	راستہ += 2;
    }
    اگر (*راستہ == NUL)		// drive name only
	واپس 0;
    واپس chdir(راستہ);		// let the normal chdir() do the rest
}

/*
 * Expand the executable name into a full path name.
 */
    ساکن حرف *
my_fullpath(حرف *buf, حرف *fname غیر_مستعمل, صحیح len)
{
    // Only GetModuleFileName() will get the long file name path.
    // GetFullPathName() may still use the short (FAT) name.
    DWORD len_read = GetModuleFileName(تہی, buf, (size_t)len);

    واپس (len_read > 0 && len_read < (DWORD)len) ? buf : تہی;
}

/*
 * Remove the tail from a file or directory name.
 * Puts a NUL on the last '/' or '\'.
 */
    ساکن خالی
remove_دم(حرف *راستہ)
{
    صحیح		i;

    برائے (i = لڑی_لمبائی(راستہ) - 1; i > 0; --i)
	اگر (راستہ[i] == '/' || راستہ[i] == '\\')
	{
	    راستہ[i] = NUL;
	    توڑو;
	}
}


حرف	installdir[MAX_راستہ-9];	// top of the installation dir, where the
				// install.exe is located, E.g.:
				// "c:\vim\vim60"
صحیح	runtimeidx;		// index in installdir[] where "vim60" starts
حرف	*sysdrive;		// system drive or "c:\"

/*
 * Setup for using this program.
 * Sets "installdir[]".
 */
    ساکن خالی
do_inits(حرف **argv)
{
    // Find out the full path of our executable.
    اگر (my_fullpath(installdir, argv[0], حجم(installdir)) == تہی)
    {
	لکھو("ERROR: Cannot get name of executable\n");
	myexit(1);
    }
    // remove the tail, the executable name "install.exe"
    remove_دم(installdir);

    // change to the installdir
    mch_chdir(installdir);

    // Find the system drive.  Only used for searching the Vim executable, not
    // very important.
    sysdrive = getenv("SYSTEMDRIVE");
    اگر (sysdrive == تہی || *sysdrive == NUL)
	sysdrive = "C:\\";
}
