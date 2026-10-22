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

#అన్తర్భావ <stdio.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <sys/stat.h>
#అన్తర్భావ <fcntl.h>

#నిర్వచితం_కాకపోతే UNIX_LINT
# include <io.h>
# include <ctype.h>

# include <direct.h>

# include <windows.h>
# include <shlobj.h>
#అయితే_ముగింపు

#నిర్వచితమైతే UNIX_LINT
// Running lint on Unix: Some things are missing.
అక్షరం *searchpath(అక్షరం *name);
#అయితే_ముగింపు

#ఒకవేళ defined(UNIX_LINT)
# include <unistd.h>
# include <errno.h>
#అయితే_ముగింపు

#అన్తర్భావ "version.h"

#ఒకవేళ defined(UNIX_LINT)
# define vim_mkdir(x, y) mkdir((అక్షరం *)(x), y)
#లేకపోతే
# define vim_mkdir(x, y) _mkdir((అక్షరం *)(x))
#అయితే_ముగింపు

#నిర్వచించు sleep(n) Sleep((n) * 1000)

// ----------------------------------------


#నిర్వచించు BUFSIZE (MAX_మార్గం*2)		// long enough to hold a file name path
#నిర్వచించు NUL 0

#నిర్వచించు FAIL 0
#నిర్వచించు OK 1

#నిర్వచితం_కాకపోతే FALSE
# define FALSE 0
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే TRUE
# define TRUE 1
#అయితే_ముగింపు

/*
 * Modern way of creating registry entries, also works on 64 bit windows when
 * compiled as a 32 bit program.
 */
#నిర్వచితం_కాకపోతే కీలకం_WOW64_64_కీలకం
# define కీలకం_WOW64_64_కీలకం 0x0100
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే కీలకం_WOW64_32_కీలకం
# define కీలకం_WOW64_32_కీలకం 0x0200
#అయితే_ముగింపు

#నిర్వచితమైతే __MINGW32__
# define వాడని __attribute__((వాడని_2))
#లేకపోతే
# define వాడని
#అయితే_ముగింపు

#నిర్వచించు VIM_STARTMENU "Programs\\Vim " VIM_సంస్కరణ_SHORT

పూర్ణాంకం	interactive;		// non-zero when running interactively

/*
 * Call malloc() and exit when out of memory.
 */
    స్థిర శూన్య *
alloc(పూర్ణాంకం len)
{
    శూన్య *p;

    p = జ్ఞాపకం_ఇవ్వు(len);
    ఒకవేళ (p == ఖాళీ)
    {
	రాయి("ERROR: out of memory\n");
	నిష్క్రమించు(1);
    }
    తిరిగి p;
}

/*
 * The toupper() in Bcc 5.5 doesn't work, use our own implementation.
 */
    స్థిర పూర్ణాంకం
mytoupper(పూర్ణాంకం c)
{
    ఒకవేళ (c >= 'a' && c <= 'z')
	తిరిగి c - 'a' + 'A';
    తిరిగి c;
}

    స్థిర శూన్య
myexit(పూర్ణాంకం n)
{
    ఒకవేళ (!interactive)
    {
	// Present a prompt, otherwise error messages can't be read.
	రాయి("Press Enter to continue\n");
	rewind(stdin);
	(శూన్య)getchar();
    }
    నిష్క్రమించు(n);
}


రకనిర్వచనం BOOL (WINAPI *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
/*
 * Check if this is a 64-bit OS.
 */
    స్థిర BOOL
is_64bit_os(శూన్య)
{
#నిర్వచితమైతే _WIN64
    తిరిగి TRUE;
#లేకపోతే
    BOOL bIsWow64 = FALSE;
    LPFN_ISWOW64PROCESS pIsWow64Process;

    pIsWow64Process = (LPFN_ISWOW64PROCESS)Get_Proc_చిరునామా(
	    GetModuleHandle("kernel32"), "IsWow64Process");
    ఒకవేళ (pIsWow64Process != ఖాళీ)
	pIsWow64Process(GetCurrentProcess(), &bIsWow64);
    తిరిగి bIsWow64;
#అయితే_ముగింపు
}

    స్థిర అక్షరం *
searchpath(అక్షరం *name)
{
    స్థిర అక్షరం widename[2 * BUFSIZE];
    స్థిర అక్షరం location[2 * BUFSIZE + 2];

    // There appears to be a bug in FindExecutableA() on Windows NT.
    // Use FindExecutableW() instead...
    Multi_Byte_గమ్యం_Wide_Char(CP_ACP, 0, (LPCTSTR)name, -1,
	    (LPWSTR)widename, BUFSIZE);
    ఒకవేళ (కనుగొను_Executable_W((LPCWSTR)widename, (LPCWSTR)"",
		(LPWSTR)location) > (HINSTANCE)32)
    {
	Wide_Char_గమ్యం_Multi_Byte(CP_ACP, 0, (LPWSTR)location, -1,
		(LPSTR)widename, 2 * BUFSIZE, ఖాళీ, ఖాళీ);
	తిరిగి widename;
    }
    తిరిగి ఖాళీ;
}

/*
 * Call searchpath() and save the result in allocated memory, or return NULL.
 */
    స్థిర అక్షరం *
searchpath_save(అక్షరం *name)
{
    అక్షరం	*p;
    అక్షరం	*s;

    p = searchpath(name);
    ఒకవేళ (p == ఖాళీ)
	తిరిగి ఖాళీ;
    s = alloc(అక్షరశ్రేణి_పొడవు(p) + 1);
    strcpy(s, p);
    తిరిగి s;
}


#నిర్వచితం_కాకపోతే CSIDL_COMMON_PROGRAMS
# define CSIDL_COMMON_PROGRAMS 0x0017
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CSIDL_COMMON_DESKTOPDIRECTORY
# define CSIDL_COMMON_DESKTOPDIRECTORY 0x0019
#అయితే_ముగింపు

/*
 * Get the path to a requested Windows shell folder.
 *
 * Return FAIL on error, OK on success
 */
    పూర్ణాంకం
get_shell_folder_మార్గం(
	అక్షరం *shell_folder_మార్గం,
	స్థిరరాశి అక్షరం *shell_folder_name)
{
    /*
     * The following code was successfully built with make_mvc.mak.
     * The resulting executable worked on Windows 95, Millennium Edition, and
     * 2000 Professional.  But it was changed after testing...
     */
    LPITEMIDLIST    pidl = 0; // Pointer to an Item ID list allocated below
    LPMALLOC	    pMalloc;  // Pointer to an IMalloc interface
    పూర్ణాంకం		    csidl;
    పూర్ణాంకం		    alt_csidl = -1;
    స్థిర పూర్ణాంకం	    desktop_csidl = -1;
    స్థిర పూర్ణాంకం	    programs_csidl = -1;
    పూర్ణాంకం		    *pcsidl;
    పూర్ణాంకం		    r;

    ఒకవేళ (strcmp(shell_folder_name, "desktop") == 0)
    {
	pcsidl = &desktop_csidl;
	csidl = CSIDL_COMMON_DESKTOPDIRECTORY;
	alt_csidl = CSIDL_DESKTOP;
    }
    లేకపోతే ఒకవేళ (అక్షరశ్రేణి_పోలిక(shell_folder_name, "Programs", 8) == 0)
    {
	pcsidl = &programs_csidl;
	csidl = CSIDL_COMMON_PROGRAMS;
	alt_csidl = CSIDL_PROGRAMS;
    }
    లేకపోతే
    {
	రాయి("\nERROR (internal) unrecognised shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	తిరిగి FAIL;
    }

    // Did this stuff before, use the same ID again.
    ఒకవేళ (*pcsidl >= 0)
    {
	csidl = *pcsidl;
	alt_csidl = -1;
    }

retry:
    // Initialize pointer to IMalloc interface
    ఒకవేళ (NOERROR != SHGetMalloc(&pMalloc))
    {
	రాయి("\nERROR getting interface for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	తిరిగి FAIL;
    }

    // Get an ITEMIDLIST corresponding to the folder code
    ఒకవేళ (NOERROR != SHGetSpecialFolderLocation(0, csidl, &pidl))
    {
	ఒకవేళ (alt_csidl < 0 || NOERROR != SHGetSpecialFolderLocation(0,
							    alt_csidl, &pidl))
	{
	    రాయి("\nERROR getting ITEMIDLIST for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	    తిరిగి FAIL;
	}
	csidl = alt_csidl;
	alt_csidl = -1;
    }

    // Translate that ITEMIDLIST to a string
    r = SHGet_మార్గం_From_IDList(pidl, shell_folder_మార్గం);

    // Free the data associated with pidl
    pMalloc->lpVtbl->Free(pMalloc, pidl);
    // Release the IMalloc interface
    pMalloc->lpVtbl->Release(pMalloc);

    ఒకవేళ (!r)
    {
	ఒకవేళ (alt_csidl >= 0)
	{
	    // We probably get here for Windows 95: the "all users"
	    // desktop/start menu entry doesn't exist.
	    csidl = alt_csidl;
	    alt_csidl = -1;
	    వెళ్ళు retry;
	}
	రాయి("\nERROR translating ITEMIDLIST for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	తిరిగి FAIL;
    }

    // If there is an alternative: verify we can write in this directory.
    // This should cause a retry when the "all users" directory exists but we
    // are a normal user and can't write there.
    ఒకవేళ (alt_csidl >= 0)
    {
	అక్షరం tbuf[BUFSIZE];
	FILE *fd;

	strcpy(tbuf, shell_folder_మార్గం);
	strcat(tbuf, "\\vim write test");
	fd = fopen(tbuf, "w");
	ఒకవేళ (fd == ఖాళీ)
	{
	    csidl = alt_csidl;
	    alt_csidl = -1;
	    వెళ్ళు retry;
	}
	fclose(fd);
	unlink(tbuf);
    }

    /*
     * Keep the found csidl for next time, so that we don't have to do the
     * write test every time.
     */
    ఒకవేళ (*pcsidl < 0)
	*pcsidl = csidl;

    ఒకవేళ (అక్షరశ్రేణి_పోలిక(shell_folder_name, "Programs\\", 9) == 0)
	strcat(shell_folder_మార్గం, shell_folder_name + 8);

    తిరిగి OK;
}

/*
 * List of targets.  The first one (index zero) is used for the default path
 * for the batch files.
 */
#నిర్వచించు లక్ష్యం_లెక్క  9

నిర్మాణం
{
    అక్షరం	*name;		// Vim exe name (without .exe)
    అక్షరం	*batname;	// batch file name
    అక్షరం	*lnkname;	// shortcut file name
    అక్షరం	*exename;	// exe file name
    అక్షరం	*exenamearg;	// exe file name when using exearg
    అక్షరం	*exearg;	// argument for vim.exe or gvim.exe
    అక్షరం	*oldbat;	// path to existing xxx.bat or NULL
    అక్షరం	*oldexe;	// path to existing xxx.exe or NULL
    అక్షరం	batpath[BUFSIZE];  // path of batch file to create; not
				   // created when it's empty
} targets[లక్ష్యం_లెక్క] =
{
    {"all",	"batch files", ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ""},
    {"vim",	"vim.bat",	"Vim.lnk",
			"vim.exe",    "vim.exe",  "", ఖాళీ, ఖాళీ, ""},
    {"gvim",	"gvim.bat",	"gVim.lnk",
			"gvim.exe",   "gvim.exe", "", ఖాళీ, ఖాళీ, ""},
    {"evim",	"evim.bat",	"gVim Easy.lnk",
			"evim.exe",   "gvim.exe", "-y", ఖాళీ, ఖాళీ, ""},
    {"view",	"view.bat",	"Vim Read-only.lnk",
			"view.exe",   "vim.exe",  "-R", ఖాళీ, ఖాళీ, ""},
    {"gview",	"gview.bat",	"gVim Read-only.lnk",
			"gview.exe",  "gvim.exe", "-R", ఖాళీ, ఖాళీ, ""},
    {"vimdiff", "vimdiff.bat",	"Vim Diff.lnk",
			"vimdiff.exe","vim.exe",  "-d", ఖాళీ, ఖాళీ, ""},
    {"gvimdiff","gvimdiff.bat",	"gVim Diff.lnk",
			"gvimdiff.exe","gvim.exe", "-d", ఖాళీ, ఖాళీ, ""},
    {"vimtutor","vimtutor.bat", "Vim tutor.lnk",
			"vimtutor.bat",  "vimtutor.bat", "", ఖాళీ, ఖాళీ, ""},
};

/* Uninstall key for vim.bat, etc. */
#నిర్వచించు VIMBAT_UNINSTKEY    "rem # uninstall key: " VIM_సంస్కరణ_NODOT " #"

#నిర్వచించు ICON_లెక్క 3
అక్షరం *(icon_పేర్లు[ICON_లెక్క]) =
	{"gVim " VIM_సంస్కరణ_SHORT,
	 "gVim Easy " VIM_సంస్కరణ_SHORT,
	 "gVim Read only " VIM_సంస్కరణ_SHORT};
అక్షరం *(icon_link_పేర్లు[ICON_లెక్క]) =
	{"gVim " VIM_సంస్కరణ_SHORT ".lnk",
	 "gVim Easy " VIM_సంస్కరణ_SHORT ".lnk",
	 "gVim Read only " VIM_సంస్కరణ_SHORT ".lnk"};

/* This is only used for dosinst.c. */
#ఒకవేళ defined(DOSINST)
/*
 * Run an external command and wait for it to finish.
 */
    స్థిర శూన్య
నిరంతరం_command(అక్షరం *cmd)
{
    అక్షరం	*cmd_మార్గం;
    అక్షరం	cmd_buf[BUFSIZE * 2 + 35];
    అక్షరం	*p;

    // On WinNT, 'start' is a shell built-in for cmd.exe rather than an
    // executable (start.exe) like in Win9x.
    cmd_మార్గం = searchpath_save("cmd.exe");
    ఒకవేళ (cmd_మార్గం != ఖాళీ)
    {
	// There is a cmd.exe, so this might be Windows NT.  If it is,
	// we need to call cmd.exe explicitly.  If it is a later OS,
	// calling cmd.exe won't hurt if it is present.
	// Also, "start" on NT expects a window title argument.
	// Replace the slashes with backslashes.
	అయితే ((p = strchr(cmd_మార్గం, '/')) != ఖాళీ)
	    *p = '\\';
	sprintf(cmd_buf, "%s /c start \"vimcmd\" /wait %s", cmd_మార్గం, cmd);
	విడుదల(cmd_మార్గం);
    }
    లేకపోతే
    {
	// No cmd.exe, just make the call and let the system handle it.
	sprintf(cmd_buf, "start /w %s", cmd);
    }
    system(cmd_buf);
}
#అయితే_ముగింపు

/*
 * Append a backslash to "name" if there isn't one yet.
 */
    శూన్య
కూడు_pathsep(అక్షరం *name)
{
    పూర్ణాంకం		len = అక్షరశ్రేణి_పొడవు(name);

    ఒకవేళ (len > 0 && name[len - 1] != '\\' && name[len - 1] != '/')
	strcat(name, "\\");
}

/*
 * The normal chdir() does not change the default drive.  This one does.
 */
    పూర్ణాంకం
change_drive(పూర్ణాంకం drive)
{
    అక్షరం తాత్కాలిక[3] = "-:";
    తాత్కాలిక[0] = (అక్షరం)(drive + 'A' - 1);
    తిరిగి !సమితి_Current_Directory(తాత్కాలిక);
}

/*
 * Change directory to "path".
 * Return 0 for success, -1 for failure.
 */
    పూర్ణాంకం
mch_chdir(అక్షరం *మార్గం)
{
    ఒకవేళ (మార్గం[0] == NUL)		// just checking...
	తిరిగి 0;
    ఒకవేళ (మార్గం[1] == ':')		// has a drive name
    {
	ఒకవేళ (change_drive(mytoupper(మార్గం[0]) - 'A' + 1))
	    తిరిగి -1;		// invalid drive name
	మార్గం += 2;
    }
    ఒకవేళ (*మార్గం == NUL)		// drive name only
	తిరిగి 0;
    తిరిగి chdir(మార్గం);		// let the normal chdir() do the rest
}

/*
 * Expand the executable name into a full path name.
 */
    స్థిర అక్షరం *
my_fullpath(అక్షరం *buf, అక్షరం *fname వాడని, పూర్ణాంకం len)
{
    // Only GetModuleFileName() will get the long file name path.
    // GetFullPathName() may still use the short (FAT) name.
    DWORD len_read = GetModuleFileName(ఖాళీ, buf, (size_t)len);

    తిరిగి (len_read > 0 && len_read < (DWORD)len) ? buf : ఖాళీ;
}

/*
 * Remove the tail from a file or directory name.
 * Puts a NUL on the last '/' or '\'.
 */
    స్థిర శూన్య
remove_తోక(అక్షరం *మార్గం)
{
    పూర్ణాంకం		i;

    కొరకు (i = అక్షరశ్రేణి_పొడవు(మార్గం) - 1; i > 0; --i)
	ఒకవేళ (మార్గం[i] == '/' || మార్గం[i] == '\\')
	{
	    మార్గం[i] = NUL;
	    ఆపు;
	}
}


అక్షరం	installdir[MAX_మార్గం-9];	// top of the installation dir, where the
				// install.exe is located, E.g.:
				// "c:\vim\vim60"
పూర్ణాంకం	runtimeidx;		// index in installdir[] where "vim60" starts
అక్షరం	*sysdrive;		// system drive or "c:\"

/*
 * Setup for using this program.
 * Sets "installdir[]".
 */
    స్థిర శూన్య
do_inits(అక్షరం **argv)
{
    // Find out the full path of our executable.
    ఒకవేళ (my_fullpath(installdir, argv[0], పరిమాణం(installdir)) == ఖాళీ)
    {
	రాయి("ERROR: Cannot get name of executable\n");
	myexit(1);
    }
    // remove the tail, the executable name "install.exe"
    remove_తోక(installdir);

    // change to the installdir
    mch_chdir(installdir);

    // Find the system drive.  Only used for searching the Vim executable, not
    // very important.
    sysdrive = getenv("SYSTEMDRIVE");
    ఒకవేళ (sysdrive == ఖాళీ || *sysdrive == NUL)
	sysdrive = "C:\\";
}
