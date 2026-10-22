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

#ضمن <stdio.h>
#ضمن <stdlib.h>
#ضمن <string.h>
#ضمن <sys/stat.h>
#ضمن <fcntl.h>

#جيڪڏهن_وصف_نه_ٿيل UNIX_LINT
# include <io.h>
# include <ctype.h>

# include <direct.h>

# include <windows.h>
# include <shlobj.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل UNIX_LINT
// Running lint on Unix: Some things are missing.
حرف *searchpath(حرف *name);
#جيڪڏهن_پورو

#إذا defined(UNIX_LINT)
# include <unistd.h>
# include <errno.h>
#جيڪڏهن_پورو

#ضمن "version.h"

#إذا defined(UNIX_LINT)
# define vim_mkdir(x, y) mkdir((حرف *)(x), y)
#وإلا
# define vim_mkdir(x, y) _mkdir((حرف *)(x))
#جيڪڏهن_پورو

#وصف_ڪريو sleep(n) Sleep((n) * 1000)

// ----------------------------------------


#وصف_ڪريو BUFSIZE (MAX_رستو*2)		// long enough to hold a file name path
#وصف_ڪريو NUL 0

#وصف_ڪريو FAIL 0
#وصف_ڪريو OK 1

#جيڪڏهن_وصف_نه_ٿيل FALSE
# define FALSE 0
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل TRUE
# define TRUE 1
#جيڪڏهن_پورو

/*
 * Modern way of creating registry entries, also works on 64 bit windows when
 * compiled as a 32 bit program.
 */
#جيڪڏهن_وصف_نه_ٿيل ڪنجي_WOW64_64_ڪنجي
# define ڪنجي_WOW64_64_ڪنجي 0x0100
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل ڪنجي_WOW64_32_ڪنجي
# define ڪنجي_WOW64_32_ڪنجي 0x0200
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل __MINGW32__
# define اڻ_استعمال_ٿيل __attribute__((اڻ_استعمال_ٿيل_2))
#وإلا
# define اڻ_استعمال_ٿيل
#جيڪڏهن_پورو

#وصف_ڪريو VIM_STARTMENU "Programs\\Vim " VIM_نسخو_SHORT

صحيح	interactive;		// non-zero when running interactively

/*
 * Call malloc() and exit when out of memory.
 */
    ساكن فراغ *
alloc(صحيح len)
{
    فراغ *p;

    p = يادگيري_ڏيو(len);
    إذا (p == عدم)
    {
	لکو("ERROR: out of memory\n");
	ٻاهر_نڪرو(1);
    }
    أرجع p;
}

/*
 * The toupper() in Bcc 5.5 doesn't work, use our own implementation.
 */
    ساكن صحيح
mytoupper(صحيح c)
{
    إذا (c >= 'a' && c <= 'z')
	أرجع c - 'a' + 'A';
    أرجع c;
}

    ساكن فراغ
myexit(صحيح n)
{
    إذا (!interactive)
    {
	// Present a prompt, otherwise error messages can't be read.
	لکو("Press Enter to continue\n");
	rewind(stdin);
	(فراغ)getchar();
    }
    ٻاهر_نڪرو(n);
}


تعريفنوع BOOL (WINAPI *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
/*
 * Check if this is a 64-bit OS.
 */
    ساكن BOOL
is_64bit_os(فراغ)
{
#جيڪڏهن_وصف_ٿيل _WIN64
    أرجع TRUE;
#وإلا
    BOOL bIsWow64 = FALSE;
    LPFN_ISWOW64PROCESS pIsWow64Process;

    pIsWow64Process = (LPFN_ISWOW64PROCESS)Get_Proc_پتو(
	    GetModuleHandle("kernel32"), "IsWow64Process");
    إذا (pIsWow64Process != عدم)
	pIsWow64Process(GetCurrentProcess(), &bIsWow64);
    أرجع bIsWow64;
#جيڪڏهن_پورو
}

    ساكن حرف *
searchpath(حرف *name)
{
    ساكن حرف widename[2 * BUFSIZE];
    ساكن حرف location[2 * BUFSIZE + 2];

    // There appears to be a bug in FindExecutableA() on Windows NT.
    // Use FindExecutableW() instead...
    Multi_Byte_منزل_Wide_Char(CP_ACP, 0, (LPCTSTR)name, -1,
	    (LPWSTR)widename, BUFSIZE);
    إذا (ڳوليو_Executable_W((LPCWSTR)widename, (LPCWSTR)"",
		(LPWSTR)location) > (HINSTANCE)32)
    {
	Wide_Char_منزل_Multi_Byte(CP_ACP, 0, (LPWSTR)location, -1,
		(LPSTR)widename, 2 * BUFSIZE, عدم, عدم);
	أرجع widename;
    }
    أرجع عدم;
}

/*
 * Call searchpath() and save the result in allocated memory, or return NULL.
 */
    ساكن حرف *
searchpath_save(حرف *name)
{
    حرف	*p;
    حرف	*s;

    p = searchpath(name);
    إذا (p == عدم)
	أرجع عدم;
    s = alloc(لڙي_ڊيگهه(p) + 1);
    strcpy(s, p);
    أرجع s;
}


#جيڪڏهن_وصف_نه_ٿيل CSIDL_COMMON_PROGRAMS
# define CSIDL_COMMON_PROGRAMS 0x0017
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CSIDL_COMMON_DESKTOPDIRECTORY
# define CSIDL_COMMON_DESKTOPDIRECTORY 0x0019
#جيڪڏهن_پورو

/*
 * Get the path to a requested Windows shell folder.
 *
 * Return FAIL on error, OK on success
 */
    صحيح
get_shell_folder_رستو(
	حرف *shell_folder_رستو,
	ثابت حرف *shell_folder_name)
{
    /*
     * The following code was successfully built with make_mvc.mak.
     * The resulting executable worked on Windows 95, Millennium Edition, and
     * 2000 Professional.  But it was changed after testing...
     */
    LPITEMIDLIST    pidl = 0; // Pointer to an Item ID list allocated below
    LPMALLOC	    pMalloc;  // Pointer to an IMalloc interface
    صحيح		    csidl;
    صحيح		    alt_csidl = -1;
    ساكن صحيح	    desktop_csidl = -1;
    ساكن صحيح	    programs_csidl = -1;
    صحيح		    *pcsidl;
    صحيح		    r;

    إذا (strcmp(shell_folder_name, "desktop") == 0)
    {
	pcsidl = &desktop_csidl;
	csidl = CSIDL_COMMON_DESKTOPDIRECTORY;
	alt_csidl = CSIDL_DESKTOP;
    }
    وإلا إذا (لڙي_ڀيٽ(shell_folder_name, "Programs", 8) == 0)
    {
	pcsidl = &programs_csidl;
	csidl = CSIDL_COMMON_PROGRAMS;
	alt_csidl = CSIDL_PROGRAMS;
    }
    وإلا
    {
	لکو("\nERROR (internal) unrecognised shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	أرجع FAIL;
    }

    // Did this stuff before, use the same ID again.
    إذا (*pcsidl >= 0)
    {
	csidl = *pcsidl;
	alt_csidl = -1;
    }

retry:
    // Initialize pointer to IMalloc interface
    إذا (NOERROR != SHGetMalloc(&pMalloc))
    {
	لکو("\nERROR getting interface for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	أرجع FAIL;
    }

    // Get an ITEMIDLIST corresponding to the folder code
    إذا (NOERROR != SHGetSpecialFolderLocation(0, csidl, &pidl))
    {
	إذا (alt_csidl < 0 || NOERROR != SHGetSpecialFolderLocation(0,
							    alt_csidl, &pidl))
	{
	    لکو("\nERROR getting ITEMIDLIST for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	    أرجع FAIL;
	}
	csidl = alt_csidl;
	alt_csidl = -1;
    }

    // Translate that ITEMIDLIST to a string
    r = SHGet_رستو_From_IDList(pidl, shell_folder_رستو);

    // Free the data associated with pidl
    pMalloc->lpVtbl->Free(pMalloc, pidl);
    // Release the IMalloc interface
    pMalloc->lpVtbl->Release(pMalloc);

    إذا (!r)
    {
	إذا (alt_csidl >= 0)
	{
	    // We probably get here for Windows 95: the "all users"
	    // desktop/start menu entry doesn't exist.
	    csidl = alt_csidl;
	    alt_csidl = -1;
	    اذهب retry;
	}
	لکو("\nERROR translating ITEMIDLIST for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	أرجع FAIL;
    }

    // If there is an alternative: verify we can write in this directory.
    // This should cause a retry when the "all users" directory exists but we
    // are a normal user and can't write there.
    إذا (alt_csidl >= 0)
    {
	حرف tbuf[BUFSIZE];
	FILE *fd;

	strcpy(tbuf, shell_folder_رستو);
	strcat(tbuf, "\\vim write test");
	fd = fopen(tbuf, "w");
	إذا (fd == عدم)
	{
	    csidl = alt_csidl;
	    alt_csidl = -1;
	    اذهب retry;
	}
	fclose(fd);
	unlink(tbuf);
    }

    /*
     * Keep the found csidl for next time, so that we don't have to do the
     * write test every time.
     */
    إذا (*pcsidl < 0)
	*pcsidl = csidl;

    إذا (لڙي_ڀيٽ(shell_folder_name, "Programs\\", 9) == 0)
	strcat(shell_folder_رستو, shell_folder_name + 8);

    أرجع OK;
}

/*
 * List of targets.  The first one (index zero) is used for the default path
 * for the batch files.
 */
#وصف_ڪريو هدف_ڳڻپ  9

بنية
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
} targets[هدف_ڳڻپ] =
{
    {"all",	"batch files", عدم, عدم, عدم, عدم, عدم, عدم, ""},
    {"vim",	"vim.bat",	"Vim.lnk",
			"vim.exe",    "vim.exe",  "", عدم, عدم, ""},
    {"gvim",	"gvim.bat",	"gVim.lnk",
			"gvim.exe",   "gvim.exe", "", عدم, عدم, ""},
    {"evim",	"evim.bat",	"gVim Easy.lnk",
			"evim.exe",   "gvim.exe", "-y", عدم, عدم, ""},
    {"view",	"view.bat",	"Vim Read-only.lnk",
			"view.exe",   "vim.exe",  "-R", عدم, عدم, ""},
    {"gview",	"gview.bat",	"gVim Read-only.lnk",
			"gview.exe",  "gvim.exe", "-R", عدم, عدم, ""},
    {"vimdiff", "vimdiff.bat",	"Vim Diff.lnk",
			"vimdiff.exe","vim.exe",  "-d", عدم, عدم, ""},
    {"gvimdiff","gvimdiff.bat",	"gVim Diff.lnk",
			"gvimdiff.exe","gvim.exe", "-d", عدم, عدم, ""},
    {"vimtutor","vimtutor.bat", "Vim tutor.lnk",
			"vimtutor.bat",  "vimtutor.bat", "", عدم, عدم, ""},
};

/* Uninstall key for vim.bat, etc. */
#وصف_ڪريو VIMBAT_UNINSTKEY    "rem # uninstall key: " VIM_نسخو_NODOT " #"

#وصف_ڪريو ICON_ڳڻپ 3
حرف *(icon_نالا[ICON_ڳڻپ]) =
	{"gVim " VIM_نسخو_SHORT,
	 "gVim Easy " VIM_نسخو_SHORT,
	 "gVim Read only " VIM_نسخو_SHORT};
حرف *(icon_link_نالا[ICON_ڳڻپ]) =
	{"gVim " VIM_نسخو_SHORT ".lnk",
	 "gVim Easy " VIM_نسخو_SHORT ".lnk",
	 "gVim Read only " VIM_نسخو_SHORT ".lnk"};

/* This is only used for dosinst.c. */
#إذا defined(DOSINST)
/*
 * Run an external command and wait for it to finish.
 */
    ساكن فراغ
لاڳيتو_command(حرف *cmd)
{
    حرف	*cmd_رستو;
    حرف	cmd_buf[BUFSIZE * 2 + 35];
    حرف	*p;

    // On WinNT, 'start' is a shell built-in for cmd.exe rather than an
    // executable (start.exe) like in Win9x.
    cmd_رستو = searchpath_save("cmd.exe");
    إذا (cmd_رستو != عدم)
    {
	// There is a cmd.exe, so this might be Windows NT.  If it is,
	// we need to call cmd.exe explicitly.  If it is a later OS,
	// calling cmd.exe won't hurt if it is present.
	// Also, "start" on NT expects a window title argument.
	// Replace the slashes with backslashes.
	طالما ((p = strchr(cmd_رستو, '/')) != عدم)
	    *p = '\\';
	sprintf(cmd_buf, "%s /c start \"vimcmd\" /wait %s", cmd_رستو, cmd);
	حرر(cmd_رستو);
    }
    وإلا
    {
	// No cmd.exe, just make the call and let the system handle it.
	sprintf(cmd_buf, "start /w %s", cmd);
    }
    system(cmd_buf);
}
#جيڪڏهن_پورو

/*
 * Append a backslash to "name" if there isn't one yet.
 */
    فراغ
جوڙيو_pathsep(حرف *name)
{
    صحيح		len = لڙي_ڊيگهه(name);

    إذا (len > 0 && name[len - 1] != '\\' && name[len - 1] != '/')
	strcat(name, "\\");
}

/*
 * The normal chdir() does not change the default drive.  This one does.
 */
    صحيح
change_drive(صحيح drive)
{
    حرف عارضي[3] = "-:";
    عارضي[0] = (حرف)(drive + 'A' - 1);
    أرجع !سيٽ_Current_Directory(عارضي);
}

/*
 * Change directory to "path".
 * Return 0 for success, -1 for failure.
 */
    صحيح
mch_chdir(حرف *رستو)
{
    إذا (رستو[0] == NUL)		// just checking...
	أرجع 0;
    إذا (رستو[1] == ':')		// has a drive name
    {
	إذا (change_drive(mytoupper(رستو[0]) - 'A' + 1))
	    أرجع -1;		// invalid drive name
	رستو += 2;
    }
    إذا (*رستو == NUL)		// drive name only
	أرجع 0;
    أرجع chdir(رستو);		// let the normal chdir() do the rest
}

/*
 * Expand the executable name into a full path name.
 */
    ساكن حرف *
my_fullpath(حرف *buf, حرف *fname اڻ_استعمال_ٿيل, صحيح len)
{
    // Only GetModuleFileName() will get the long file name path.
    // GetFullPathName() may still use the short (FAT) name.
    DWORD len_read = GetModuleFileName(عدم, buf, (size_t)len);

    أرجع (len_read > 0 && len_read < (DWORD)len) ? buf : عدم;
}

/*
 * Remove the tail from a file or directory name.
 * Puts a NUL on the last '/' or '\'.
 */
    ساكن فراغ
remove_پڇ(حرف *رستو)
{
    صحيح		i;

    لكل (i = لڙي_ڊيگهه(رستو) - 1; i > 0; --i)
	إذا (رستو[i] == '/' || رستو[i] == '\\')
	{
	    رستو[i] = NUL;
	    اكسر;
	}
}


حرف	installdir[MAX_رستو-9];	// top of the installation dir, where the
				// install.exe is located, E.g.:
				// "c:\vim\vim60"
صحيح	runtimeidx;		// index in installdir[] where "vim60" starts
حرف	*sysdrive;		// system drive or "c:\"

/*
 * Setup for using this program.
 * Sets "installdir[]".
 */
    ساكن فراغ
do_inits(حرف **argv)
{
    // Find out the full path of our executable.
    إذا (my_fullpath(installdir, argv[0], حجم(installdir)) == عدم)
    {
	لکو("ERROR: Cannot get name of executable\n");
	myexit(1);
    }
    // remove the tail, the executable name "install.exe"
    remove_پڇ(installdir);

    // change to the installdir
    mch_chdir(installdir);

    // Find the system drive.  Only used for searching the Vim executable, not
    // very important.
    sysdrive = getenv("SYSTEMDRIVE");
    إذا (sysdrive == عدم || *sysdrive == NUL)
	sysdrive = "C:\\";
}
