/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * dosinst.c: Install program for Vim on MS-DOS and MS-Windows
 *
 * Compile with Make_mvc.mak, Make_cyg.mak or Make_ming.mak.
 */

/*
 * Include common code for dosinst.c and uninstall.c.
 */
#define DOSINST
#ಅನ್ತರ್ಭಾವ "dosinst.h"
#ಅನ್ತರ್ಭಾವ <io.h>

#define GVIMEXT64_PATH	    "GvimExt64\\gvimext.dll"
#define GVIMEXT32_PATH	    "GvimExt32\\gvimext.dll"

// Macro to do an error check I was typing over and over
#define CHECK_REG_ERROR(code) \
    ಕುರು { \
	ಯದಿ (code != ERROR_SUCCESS) \
	{ \
	    printf("%ld error number:  %ld\n", (long)__LINE__, (long)code); \
	    ಪ್ರತ್ಯಾಗಮ 1; \
	} \
    } ಯಾವತ್ (0)

ಪೂರ್ಣಾಙ್ಕ	has_vim = 0;		// installable vim.exe exists
ಪೂರ್ಣಾಙ್ಕ	has_gvim = 0;		// installable gvim.exe exists

ವರ್ಣ	oldvimrc[BUFSIZE];	// name of existing vimrc file
ವರ್ಣ	vimrc[BUFSIZE];		// name of vimrc file to create

ವರ್ಣ	*default_bat_dir = ರಿಕ್ತ;  // when not NULL, use this as the default
				  // directory to write .bat files in
ವರ್ಣ	*default_vim_dir = ರಿಕ್ತ;  // when not NULL, use this as the default
				  // install dir for NSIS

/*
 * Structure used for each choice the user can make.
 */
ಸಂರಚನಾ choice
{
    ಪೂರ್ಣಾಙ್ಕ	    active;			// non-zero when choice is active
    ವರ್ಣ    *text;			// text displayed for this choice
    ಶೂನ್ಯ    (*changefunc)(ಪೂರ್ಣಾಙ್ಕ idx);	// function to change this choice
    ಪೂರ್ಣಾಙ್ಕ	    arg;			// argument for function
    ಶೂನ್ಯ    (*installfunc)(ಪೂರ್ಣಾಙ್ಕ idx);	// function to install this choice
};

ಸಂರಚನಾ choice	choices[30];		// choices the user can make
ಪೂರ್ಣಾಙ್ಕ		choice_ಎಣಿಕೆ = 0;	// number of choices available

#define TABLE_SIZE(s)	(ಪೂರ್ಣಾಙ್ಕ)ARRAYSIZE(s)

ಗಣನಾ
{
    compat_vi = 1,
    compat_vim,
    compat_some_enhancements,
    compat_all_enhancements
};
ವರ್ಣ	*(compat_choices[]) =
{
    "\nChoose the default way to run Vim:",
    "Vi compatible",
    "Vim default",
    "with some Vim enhancements",
    "with syntax highlighting and other features switched on",
};
ಪೂರ್ಣಾಙ್ಕ	compat_choice = (ಪೂರ್ಣಾಙ್ಕ)compat_all_enhancements;
ವರ್ಣ	*compat_text = "- run Vim %s";

ಗಣನಾ
{
    remap_no = 1,
    remap_win
};
ವರ್ಣ	*(remap_choices[]) =
{
    "\nChoose:",
    "Do not remap keys for Windows behavior",
    "Remap a few keys for Windows behavior (CTRL-V, CTRL-C, CTRL-F, etc)",
};
ಪೂರ್ಣಾಙ್ಕ	remap_choice = (ಪೂರ್ಣಾಙ್ಕ)remap_no;
ವರ್ಣ	*remap_text = "- %s";

ಗಣನಾ
{
    mouse_xterm = 1,
    mouse_mswin,
    mouse_default
};
ವರ್ಣ	*(mouse_choices[]) =
{
    "\nChoose the way how Vim uses the mouse:",
    "right button extends selection (the Unix way)",
    "right button has a popup menu, left button starts select mode (the Windows way)",
    "right button has a popup menu, left button starts visual mode",
};
ಪೂರ್ಣಾಙ್ಕ	mouse_choice = (ಪೂರ್ಣಾಙ್ಕ)mouse_default;
ವರ್ಣ	*mouse_text = "- The mouse %s";

ಗಣನಾ
{
    vimfiles_dir_none = 1,
    vimfiles_dir_vim,
    vimfiles_dir_home
};
ಸ್ಥಿರ ವರ್ಣ *(vimfiles_dir_choices[]) =
{
    "\nCreate plugin directories:",
    "No",
    "In the VIM directory",
    "In your HOME directory",
};

// non-zero when selected to install the popup menu entry.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	install_popup = 0;

// non-zero when selected to install the "Open with" entry.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	install_openwith = 0;

// non-zero when need to add an uninstall entry in the registry
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	need_uninstall_entry = 0;

/*
 * Definitions of the directory name (under $VIM) of the vimfiles directory
 * and its subdirectories:
 */
ಸ್ಥಿರ ವರ್ಣ	*(vimfiles_subdirs[]) =
{
    "colors",
    "compiler",
    "doc",
    "ftdetect",
    "ftplugin",
    "indent",
    "keymap",
    "plugin",
    "syntax",
};

/*
 * Obtain a choice from a table.
 * First entry is a question, others are choices.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_choice(ವರ್ಣ **table, ಪೂರ್ಣಾಙ್ಕ entries)
{
    ಪೂರ್ಣಾಙ್ಕ		answer;
    ಪೂರ್ಣಾಙ್ಕ		idx;
    ವರ್ಣ	dummy[100];

    ಕುರು
    {
	ಪ್ರತಿ (idx = 0; idx < entries; ++idx)
	{
	    ಯದಿ (idx)
		printf("%2d  ", idx);
	    puts(table[idx]);
	}
	printf("Choice: ");
	ಯದಿ (scanf("%d", &answer) != 1)
	{
	    scanf("%99s", dummy);
	    answer = 0;
	}
    }
    ಯಾವತ್ (answer < 1 || answer >= entries);

    ಪ್ರತ್ಯಾಗಮ answer;
}

/*
 * Check if the user unpacked the archives properly.
 * Sets "runtimeidx".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
check_unpack(ಶೂನ್ಯ)
{
    ವರ್ಣ	buf[BUFSIZE];
    FILE	*fd;
    ಸಂರಚನಾ stat	st;

    // check for presence of the correct version number in installdir[]
    runtimeidx = strlen(installdir) - strlen(VIM_VERSION_NODOT);
    ಯದಿ (runtimeidx <= 0
	    || stricmp(installdir + runtimeidx, VIM_VERSION_NODOT) != 0
	    || (installdir[runtimeidx - 1] != '/'
		&& installdir[runtimeidx - 1] != '\\'))
    {
	printf("ERROR: Install program not in directory \"%s\"\n",
		VIM_VERSION_NODOT);
	printf("This program can only work when it is located in its original directory\n");
	myexit(1);
    }

    // check if filetype.vim is present, which means the runtime archive has
    // been unpacked
    sprintf(buf, "%s\\filetype.vim", installdir);
    ಯದಿ (stat(buf, &st) < 0)
    {
	printf("ERROR: Cannot find filetype.vim in \"%s\"\n", installdir);
	printf("It looks like you did not unpack the runtime archive.\n");
	printf("You must unpack the runtime archive \"%srt.zip\" before installing.\n",
		VIM_VERSION_NODOT);
	myexit(1);
    }

    // Check if vim.exe or gvim.exe is in the current directory.
    ಯದಿ ((fd = fopen("gvim.exe", "r")) != ರಿಕ್ತ)
    {
	fclose(fd);
	has_gvim = 1;
    }
    ಯದಿ ((fd = fopen("vim.exe", "r")) != ರಿಕ್ತ)
    {
	fclose(fd);
	has_vim = 1;
    }
    ಯದಿ (!has_gvim && !has_vim)
    {
	printf("ERROR: Cannot find any Vim executables in \"%s\"\n\n",
								  installdir);
	myexit(1);
    }
}

/*
 * Compare paths "p[plen]" to "q[qlen]".  Return 0 if they match.
 * Ignores case and differences between '/' and '\'.
 * "plen" and "qlen" can be negative, strlen() is used then.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
pathcmp(ವರ್ಣ *p, ಪೂರ್ಣಾಙ್ಕ plen, ವರ್ಣ *q, ಪೂರ್ಣಾಙ್ಕ qlen)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (plen < 0)
	plen = strlen(p);
    ಯದಿ (qlen < 0)
	qlen = strlen(q);
    ಪ್ರತಿ (i = 0; ; ++i)
    {
	// End of "p": check if "q" also ends or just has a slash.
	ಯದಿ (i == plen)
	{
	    ಯದಿ (i == qlen)  // match
		ಪ್ರತ್ಯಾಗಮ 0;
	    ಯದಿ (i == qlen - 1 && (q[i] == '\\' || q[i] == '/'))
		ಪ್ರತ್ಯಾಗಮ 0;   // match with trailing slash
	    ಪ್ರತ್ಯಾಗಮ 1;	    // no match
	}

	// End of "q": check if "p" also ends or just has a slash.
	ಯದಿ (i == qlen)
	{
	    ಯದಿ (i == plen)  // match
		ಪ್ರತ್ಯಾಗಮ 0;
	    ಯದಿ (i == plen - 1 && (p[i] == '\\' || p[i] == '/'))
		ಪ್ರತ್ಯಾಗಮ 0;   // match with trailing slash
	    ಪ್ರತ್ಯಾಗಮ 1;	    // no match
	}

	ಯದಿ (!(mytoupper(p[i]) == mytoupper(q[i])
		|| ((p[i] == '/' || p[i] == '\\')
		    && (q[i] == '/' || q[i] == '\\'))))
	    ಪ್ರತ್ಯಾಗಮ 1;	    // no match
    }
    //NOTREACHED
}

/*
 * If the executable "**destination" is in the install directory, find another
 * one in $PATH.
 * On input "**destination" is the path of an executable in allocated memory
 * (or NULL).
 * "*destination" is set to NULL or the location of the file.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
findoldfile(ವರ್ಣ **destination)
{
    ವರ್ಣ	*bp = *destination;
    size_t	indir_l = strlen(installdir);
    ವರ್ಣ	*cp;
    ವರ್ಣ	*tmpname;
    ವರ್ಣ	*farname;

    /*
     * No action needed if exe not found or not in this directory.
     */
    ಯದಿ (bp == ರಿಕ್ತ || strnicmp(bp, installdir, indir_l) != 0)
	ಪ್ರತ್ಯಾಗಮ;
    cp = bp + indir_l;
    ಯದಿ (strchr("/\\", *cp++) == ರಿಕ್ತ
	    || strchr(cp, '\\') != ರಿಕ್ತ
	    || strchr(cp, '/') != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    tmpname = alloc(strlen(cp) + 1);
    strcpy(tmpname, cp);
    tmpname[strlen(tmpname) - 1] = 'x';	// .exe -> .exx

    ಯದಿ (access(tmpname, 0) == 0)
    {
	printf("\nERROR: %s and %s clash.  Remove or rename %s.\n",
	    tmpname, cp, tmpname);
	myexit(1);
    }

    ಯದಿ (rename(cp, tmpname) != 0)
    {
	printf("\nERROR: failed to rename %s to %s: %s\n",
	    cp, tmpname, strerror(0));
	myexit(1);
    }

    farname = searchpath_save(cp);

    ಯದಿ (rename(tmpname, cp) != 0)
    {
	printf("\nERROR: failed to rename %s back to %s: %s\n",
	    tmpname, cp, strerror(0));
	myexit(1);
    }

    ಮುಕ್ತ(*destination);
    ಮುಕ್ತ(tmpname);
    *destination = farname;
}

/*
 * Check if there is a vim.[exe|bat|, gvim.[exe|bat|, etc. in the path.
 * When "check_bat_only" is TRUE, only find "default_bat_dir".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
find_bat_exe(ಪೂರ್ಣಾಙ್ಕ check_bat_only)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    ಪ್ರತಿ (i = 1; i < TARGET_ಎಣಿಕೆ; ++i)
    {
	targets[i].oldbat = searchpath_save(targets[i].batname);
	ಯದಿ (!check_bat_only)
	    targets[i].oldexe = searchpath_save(targets[i].exename);

	ಯದಿ (default_bat_dir == ರಿಕ್ತ && targets[i].oldbat != ರಿಕ್ತ)
	{
	    default_bat_dir = alloc(strlen(targets[i].oldbat) + 1);
	    strcpy(default_bat_dir, targets[i].oldbat);
	    remove_tail(default_bat_dir);
	}
	ಯದಿ (check_bat_only && targets[i].oldbat != ರಿಕ್ತ)
	{
	    ಮುಕ್ತ(targets[i].oldbat);
	    targets[i].oldbat = ರಿಕ್ತ;
	}
    }

    mch_chdir(installdir);
}

/*
 * Get the value of $VIMRUNTIME or $VIM and write it in $TEMP/vimini.ini, so
 * that NSIS can read it.
 * When not set, use the directory of a previously installed Vim.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_vim_env(ಶೂನ್ಯ)
{
    ವರ್ಣ	*vim;
    ವರ್ಣ	buf[BUFSIZE];
    FILE	*fd;
    ವರ್ಣ	fname[BUFSIZE];

    // First get $VIMRUNTIME.  If it's set, remove the tail.
    vim = getenv("VIMRUNTIME");
    ಯದಿ (vim != ರಿಕ್ತ && *vim != 0 && strlen(vim) < ಪರಿಮಾಣ(buf))
    {
	strcpy(buf, vim);
	remove_tail(buf);
	vim = buf;
    }
    ಅನ್ಯಥಾ
    {
	vim = getenv("VIM");
	ಯದಿ (vim == ರಿಕ್ತ || *vim == 0)
	{
	    // Use the directory from an old uninstall entry.
	    ಯದಿ (default_vim_dir != ರಿಕ್ತ)
		vim = default_vim_dir;
	    ಅನ್ಯಥಾ
		// Let NSIS know there is no default, it should use
		// $PROGRAMFILES.
		vim = "";
	}
    }

    // NSIS also uses GetTempPath(), thus we should get the same directory
    // name as where NSIS will look for vimini.ini.
    GetTempPath(ಪರಿಮಾಣ(fname) - 12, fname);
    add_pathsep(fname);
    strcat(fname, "vimini.ini");

    fd = fopen(fname, "w");
    ಯದಿ (fd != ರಿಕ್ತ)
    {
	// Make it look like an .ini file, so that NSIS can read it with a
	// ReadINIStr command.
	fprintf(fd, "[vimini]\n");
	fprintf(fd, "dir=\"%s\"\n", vim);
	fclose(fd);
    }
    ಅನ್ಯಥಾ
    {
	printf("Failed to open %s\n", fname);
	sleep(2);
    }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_windows;

/*
 * Callback used for EnumWindows():
 * Count the window if the title looks like it is for the uninstaller.
 */
//ARGSUSED
    ಸ್ಥಿರ BOOL CALLBACK
window_cb(HWND hwnd, LPARAM lparam UNUSED)
{
    ವರ್ಣ title[256];

    title[0] = 0;
    GetWindowText(hwnd, title, 256);
    ಯದಿ (strstr(title, "Vim ") != ರಿಕ್ತ && strstr(title, " Uninstall") != ರಿಕ್ತ)
	++ಅಂಕೆ_windows;
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Run the uninstaller silently.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
run_silent_uninstall(ವರ್ಣ *uninst_exe)
{
    ವರ್ಣ    vimrt_dir[BUFSIZE];
    ವರ್ಣ    temp_uninst[BUFSIZE];
    ವರ್ಣ    temp_dir[MAX_PATH];
    ವರ್ಣ    buf[BUFSIZE * 2 + 10];
    ಪೂರ್ಣಾಙ್ಕ	    i;
    DWORD   tick;

    strcpy(vimrt_dir, uninst_exe);
    remove_tail(vimrt_dir);

    ಯದಿ (!GetTempPath(ಪರಿಮಾಣ(temp_dir), temp_dir))
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // Copy the uninstaller to a temporary exe.
    tick = Get_Tick_ಎಣಿಕೆ();
    ಪ್ರತಿ (i = 0; ; i++)
    {
	sprintf(temp_uninst, "%s\\vimun%04X.exe", temp_dir,
					  (unsigned ಪೂರ್ಣಾಙ್ಕ)((i + tick) & 0xFFFF));
	ಯದಿ (CopyFile(uninst_exe, temp_uninst, TRUE))
	    ಭಙ್ಗ;
	ಯದಿ (GetLastError() != ERROR_FILE_EXISTS)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಯದಿ (i == 65535)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // Run the copied uninstaller silently.
    ಯದಿ (strchr(temp_uninst, ' ') != ರಿಕ್ತ)
	sprintf(buf, "\"%s\" /S _?=%s", temp_uninst, vimrt_dir);
    ಅನ್ಯಥಾ
	sprintf(buf, "%s /S _?=%s", temp_uninst, vimrt_dir);
    run_command(buf);

    DeleteFile(temp_uninst);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Check for already installed Vims.
 * Return non-zero when found one.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
uninstall_check(ಪೂರ್ಣಾಙ್ಕ skip_question)
{
    HKEY	ಕೀಲಿ_handle;
    HKEY	uninstall_ಕೀಲಿ_handle;
    ವರ್ಣ	*uninstall_ಕೀಲಿ = "software\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
    ವರ್ಣ	subkey_name_buff[BUFSIZE];
    ವರ್ಣ	temp_string_buffer[BUFSIZE-2];
    DWORD	local_bufsize;
    FILETIME	temp_pfiletime;
    DWORD	ಕೀಲಿ_index;
    ವರ್ಣ	ಒಳಹರಿವು;
    long	code;
    DWORD	ಮೌಲ್ಯ_type;
    DWORD	orig_ಅಂಕೆ_ಕೀಲಿಗಳು;
    DWORD	new_ಅಂಕೆ_ಕೀಲಿಗಳು;
    DWORD	allow_silent;
    ಪೂರ್ಣಾಙ್ಕ		foundone = 0;

    code = Reg_Open_ಕೀಲಿ_Ex(HKEY_LOCAL_MACHINE, uninstall_ಕೀಲಿ, 0,
				     ಕೀಲಿ_WOW64_64_ಕೀಲಿ | ಕೀಲಿ_READ, &ಕೀಲಿ_handle);
    CHECK_REG_ERROR(code);

    ಕೀಲಿ_index = 0;
    ಯಾವತ್ (TRUE)
    {
	local_bufsize = ಪರಿಮಾಣ(subkey_name_buff);
	ಯದಿ (Reg_Enum_ಕೀಲಿ_Ex(ಕೀಲಿ_handle, ಕೀಲಿ_index, subkey_name_buff, &local_bufsize,
		ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, &temp_pfiletime) == ERROR_NO_MORE_ವಸ್ತುಗಳು)
	    ಭಙ್ಗ;

	ಯದಿ (strncmp("Vim", subkey_name_buff, 3) == 0)
	{
	    // Open the key named Vim*
	    code = Reg_Open_ಕೀಲಿ_Ex(ಕೀಲಿ_handle, subkey_name_buff, 0,
			   ಕೀಲಿ_WOW64_64_ಕೀಲಿ | ಕೀಲಿ_READ, &uninstall_ಕೀಲಿ_handle);
	    CHECK_REG_ERROR(code);

	    // get the DisplayName out of it to show the user
	    local_bufsize = ಪರಿಮಾಣ(temp_string_buffer);
	    code = Reg_Query_ಮೌಲ್ಯ_Ex(uninstall_ಕೀಲಿ_handle, "displayname", 0,
		    &ಮೌಲ್ಯ_type, (LPBYTE)temp_string_buffer,
		    &local_bufsize);
	    CHECK_REG_ERROR(code);

	    allow_silent = 0;
	    ಯದಿ (skip_question)
	    {
		DWORD varsize = ಪರಿಮಾಣ(DWORD);

		Reg_Query_ಮೌಲ್ಯ_Ex(uninstall_ಕೀಲಿ_handle, "AllowSilent", 0,
			&ಮೌಲ್ಯ_type, (LPBYTE)&allow_silent,
			&varsize);
	    }

	    foundone = 1;
	    printf("\n*********************************************************\n");
	    printf("Vim Install found what looks like an existing Vim version.\n");
	    printf("The name of the entry is:\n");
	    printf("\n        \"%s\"\n\n", temp_string_buffer);

	    printf("Installing the new version will disable part of the existing version.\n");
	    printf("(The batch files used in a console and the \"Edit with Vim\" entry in\n");
	    printf("the popup menu will use the new version)\n");

	    ಯದಿ (skip_question)
		printf("\nRunning uninstall program for \"%s\"\n", temp_string_buffer);
	    ಅನ್ಯಥಾ
		printf("\nDo you want to uninstall \"%s\" now?\n(y)es/(n)o)  ", temp_string_buffer);
	    fflush(stdout);

	    // get the UninstallString
	    local_bufsize = ಪರಿಮಾಣ(temp_string_buffer);
	    code = Reg_Query_ಮೌಲ್ಯ_Ex(uninstall_ಕೀಲಿ_handle, "uninstallstring", 0,
		    &ಮೌಲ್ಯ_type, (LPBYTE)temp_string_buffer, &local_bufsize);
	    CHECK_REG_ERROR(code);

	    // Remember the directory, it is used as the default for NSIS.
	    default_vim_dir = alloc(strlen(temp_string_buffer) + 1);
	    strcpy(default_vim_dir, temp_string_buffer);
	    remove_tail(default_vim_dir);
	    remove_tail(default_vim_dir);

	    ಒಳಹರಿವು = 'n';
	    ಕುರು
	    {
		ಯದಿ (ಒಳಹರಿವು != 'n')
		    printf("%c is an invalid reply.  Please enter either 'y' or 'n'\n", ಒಳಹರಿವು);

		ಯದಿ (skip_question)
		    ಒಳಹರಿವು = 'y';
		ಅನ್ಯಥಾ
		{
		    rewind(stdin);
		    scanf("%c", &ಒಳಹರಿವು);
		}
		ವಿಕಲ್ಪ (ಒಳಹರಿವು)
		{
		    ಪಕ್ಷ 'y':
		    ಪಕ್ಷ 'Y':
			// save the number of uninstall keys so we can know if
			// it changed
			Reg_Query_Info_ಕೀಲಿ(ಕೀಲಿ_handle, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ,
					     &orig_ಅಂಕೆ_ಕೀಲಿಗಳು, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ,
						      ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);

			// Find existing .bat files before deleting them.
			find_bat_exe(TRUE);

			ಯದಿ (allow_silent)
			{
			    ಯದಿ (run_silent_uninstall(temp_string_buffer)
								    == FAIL)
				allow_silent = 0; // Retry with non silent.
			}
			ಯದಿ (!allow_silent)
			{
			    // Execute the uninstall program.  Put it in double
			    // quotes if there is an embedded space.
			    {
				ವರ್ಣ buf[BUFSIZE];

				ಯದಿ (strchr(temp_string_buffer, ' ') != ರಿಕ್ತ)
				    sprintf(buf, "\"%s\"", temp_string_buffer);
				ಅನ್ಯಥಾ
				    strcpy(buf, temp_string_buffer);
				run_command(buf);
			    }

			    // Count the number of windows with a title that
			    // match the installer, so that we can check when
			    // it's done.  The uninstaller copies itself,
			    // executes the copy and exits, thus we can't wait
			    // for the process to finish.
			    sleep(1);  // wait for uninstaller to start up
			    ಅಂಕೆ_windows = 0;
			    EnumWindows(window_cb, 0);
			    ಯದಿ (ಅಂಕೆ_windows == 0)
			    {
				// Did not find the uninstaller, ask user to
				// press Enter when done. Just in case.
				printf("Press Enter when the uninstaller is finished\n");
				rewind(stdin);
				(ಶೂನ್ಯ)getchar();
			    }
			    ಅನ್ಯಥಾ
			    {
				printf("Waiting for the uninstaller to finish (press CTRL-C to abort).");
				ಕುರು
				{
				    printf(".");
				    fflush(stdout);
				    sleep(1);	// wait for the uninstaller to
						// finish
				    ಅಂಕೆ_windows = 0;
				    EnumWindows(window_cb, 0);
				} ಯಾವತ್ (ಅಂಕೆ_windows > 0);
			    }
			}
			printf("\nDone!\n");

			// Check if an uninstall reg key was deleted.
			// if it was, we want to decrement key_index.
			// if we don't do this, we will skip the key
			// immediately after any key that we delete.
			Reg_Query_Info_ಕೀಲಿ(ಕೀಲಿ_handle, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ,
					      &new_ಅಂಕೆ_ಕೀಲಿಗಳು, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ,
						      ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
			ಯದಿ (new_ಅಂಕೆ_ಕೀಲಿಗಳು < orig_ಅಂಕೆ_ಕೀಲಿಗಳು)
			    ಕೀಲಿ_index--;

			ಒಳಹರಿವು = 'y';
			ಭಙ್ಗ;

		    ಪಕ್ಷ 'n':
		    ಪಕ್ಷ 'N':
			// Do not uninstall
			ಒಳಹರಿವು = 'n';
			ಭಙ್ಗ;

		    ಸ್ವಭಾವ: // just drop through and redo the loop
			ಭಙ್ಗ;
		}

	    } ಯಾವತ್ (ಒಳಹರಿವು != 'n' && ಒಳಹರಿವು != 'y');

	    Reg_Close_ಕೀಲಿ(uninstall_ಕೀಲಿ_handle);
	}

	ಕೀಲಿ_index++;
    }
    Reg_Close_ಕೀಲಿ(ಕೀಲಿ_handle);

    ಪ್ರತ್ಯಾಗಮ foundone;
}

/*
 * Find out information about the system.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
inspect_system(ಶೂನ್ಯ)
{
    ವರ್ಣ	*p;
    ವರ್ಣ	buf[BUFSIZE];
    FILE	*fd;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		foundone;

    // This may take a little while, let the user know what we're doing.
    printf("Inspecting system...\n");

    /*
     * If $VIM is set, check that it's pointing to our directory.
     */
    p = getenv("VIM");
    ಯದಿ (p != ರಿಕ್ತ && pathcmp(p, -1, installdir, runtimeidx - 1) != 0)
    {
	printf("------------------------------------------------------\n");
	printf("$VIM is set to \"%s\".\n", p);
	printf("This is different from where this version of Vim is:\n");
	strcpy(buf, installdir);
	*(buf + runtimeidx - 1) = NUL;
	printf("\"%s\"\n", buf);
	printf("You must adjust or remove the setting of $VIM,\n");
	ಯದಿ (interactive)
	{
	    printf("to be able to use this install program.\n");
	    myexit(1);
	}
	printf("otherwise Vim WILL NOT WORK properly!\n");
	printf("------------------------------------------------------\n");
    }

    /*
     * If $VIMRUNTIME is set, check that it's pointing to our runtime directory.
     */
    p = getenv("VIMRUNTIME");
    ಯದಿ (p != ರಿಕ್ತ && pathcmp(p, -1, installdir, -1) != 0)
    {
	printf("------------------------------------------------------\n");
	printf("$VIMRUNTIME is set to \"%s\".\n", p);
	printf("This is different from where this version of Vim is:\n");
	printf("\"%s\"\n", installdir);
	printf("You must adjust or remove the setting of $VIMRUNTIME,\n");
	ಯದಿ (interactive)
	{
	    printf("to be able to use this install program.\n");
	    myexit(1);
	}
	printf("otherwise Vim WILL NOT WORK properly!\n");
	printf("------------------------------------------------------\n");
    }

    /*
     * Check if there is a vim.[exe|bat|, gvim.[exe|bat|, etc. in the path.
     */
    find_bat_exe(FALSE);

    /*
     * A .exe in the install directory may be found anyway on Windows 2000.
     * Check for this situation and find another executable if necessary.
     * w.briscoe@ponl.com 2001-01-20
     */
    foundone = 0;
    ಪ್ರತಿ (i = 1; i < TARGET_ಎಣಿಕೆ; ++i)
    {
	findoldfile(&(targets[i].oldexe));
	ಯದಿ (targets[i].oldexe != ರಿಕ್ತ)
	    foundone = 1;
    }

    ಯದಿ (foundone)
    {
	printf("Warning: Found Vim executable(s) in your $PATH:\n");
	ಪ್ರತಿ (i = 1; i < TARGET_ಎಣಿಕೆ; ++i)
	    ಯದಿ (targets[i].oldexe != ರಿಕ್ತ)
		printf("%s\n", targets[i].oldexe);
	printf("It will be used instead of the version you are installing.\n");
	printf("Please delete or rename it, or adjust your $PATH setting.\n");
    }

    /*
     * Check if there is an existing ../_vimrc or ../.vimrc file.
     */
    strcpy(oldvimrc, installdir);
    strcpy(oldvimrc + runtimeidx, "_vimrc");
    ಯದಿ ((fd = fopen(oldvimrc, "r")) == ರಿಕ್ತ)
    {
	strcpy(oldvimrc + runtimeidx, "vimrc~1"); // short version of .vimrc
	ಯದಿ ((fd = fopen(oldvimrc, "r")) == ರಿಕ್ತ)
	{
	    strcpy(oldvimrc + runtimeidx, ".vimrc");
	    fd = fopen(oldvimrc, "r");
	}
    }
    ಯದಿ (fd != ರಿಕ್ತ)
	fclose(fd);
    ಅನ್ಯಥಾ
	*oldvimrc = NUL;
}

/*
 * Add a dummy choice to avoid that the numbering changes depending on items
 * in the environment.  The user may type a number he remembered without
 * looking.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
add_dummy_choice(ಶೂನ್ಯ)
{
    choices[choice_ಎಣಿಕೆ].installfunc = ರಿಕ್ತ;
    choices[choice_ಎಣಿಕೆ].active = 0;
    choices[choice_ಎಣಿಕೆ].changefunc = ರಿಕ್ತ;
    choices[choice_ಎಣಿಕೆ].text = ರಿಕ್ತ;
    choices[choice_ಎಣಿಕೆ].arg = 0;
    ++choice_ಎಣಿಕೆ;
}

////////////////////////////////////////////////
// stuff for creating the batch files.

/*
 * Install the vim.bat, gvim.bat, etc. files.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
install_bat_choice(ಪೂರ್ಣಾಙ್ಕ idx)
{
    ವರ್ಣ	*batpath = targets[choices[idx].arg].batpath;
    ವರ್ಣ	*oldname = targets[choices[idx].arg].oldbat;
    ವರ್ಣ	*exename = targets[choices[idx].arg].exenamearg;
    ವರ್ಣ	*vimarg = targets[choices[idx].arg].exearg;
    FILE	*fd;

    ಯದಿ (*batpath == NUL)
	ಪ್ರತ್ಯಾಗಮ;

    fd = fopen(batpath, "w");
    ಯದಿ (fd == ರಿಕ್ತ)
    {
	printf("\nERROR: Cannot open \"%s\" for writing.\n", batpath);
	ಪ್ರತ್ಯಾಗಮ;
    }

    need_uninstall_entry = 1;

    fprintf(fd, "@echo off\n");
    fprintf(fd, "rem -- Run Vim --\n");
    fprintf(fd, VIMBAT_UNINSTKEY "\n");
    fprintf(fd, "\n");
    fprintf(fd, "setlocal\n");

    /*
     * Don't use double quotes for the "set" argument, also when it
     * contains a space.  The quotes would be included in the value.
     * The order of preference is:
     * 1. $VIMRUNTIME/vim.exe	    (user preference)
     * 2. $VIM/vim81/vim.exe	    (hard coded version)
     * 3. installdir/vim.exe	    (hard coded install directory)
     */
    fprintf(fd, "set VIM_EXE_DIR=%s\n", installdir);
    fprintf(fd, "if exist \"%%VIM%%\\%s\\%s\" set VIM_EXE_DIR=%%VIM%%\\%s\n",
	    VIM_VERSION_NODOT, exename, VIM_VERSION_NODOT);
    fprintf(fd, "if exist \"%%VIMRUNTIME%%\\%s\" set VIM_EXE_DIR=%%VIMRUNTIME%%\n", exename);
    fprintf(fd, "\n");

    // Give an error message when the executable could not be found.
    fprintf(fd, "if not exist \"%%VIM_EXE_DIR%%\\%s\" (\n", exename);
    fprintf(fd, "    echo \"%%VIM_EXE_DIR%%\\%s\" not found\n", exename);
    fprintf(fd, "    goto :eof\n");
    fprintf(fd, ")\n");
    fprintf(fd, "\n");

    ಯದಿ (*exename == 'g')
    {
	fprintf(fd, "rem check --nofork argument\n");
	fprintf(fd, "set VIMNOFORK=\n");
	fprintf(fd, ":loopstart\n");
	fprintf(fd, "if .%%1==. goto loopend\n");
	fprintf(fd, "if .%%1==.--nofork (\n");
	fprintf(fd, "    set VIMNOFORK=1\n");
	fprintf(fd, ") else if .%%1==.-f (\n");
	fprintf(fd, "    set VIMNOFORK=1\n");
	fprintf(fd, ")\n");
	fprintf(fd, "shift\n");
	fprintf(fd, "goto loopstart\n");
	fprintf(fd, ":loopend\n");
	fprintf(fd, "\n");
    }

    ಯದಿ (*exename == 'g')
    {
	// For gvim.exe use "start /b" to avoid that the console window
	// stays open.
	fprintf(fd, "if .%%VIMNOFORK%%==.1 (\n");
	fprintf(fd, "    start \"dummy\" /b /wait ");
	// Always use quotes, $VIM or $VIMRUNTIME might have a space.
	fprintf(fd, "\"%%VIM_EXE_DIR%%\\%s\" %s %%*\n",
		exename, vimarg);
	fprintf(fd, ") else (\n");
	fprintf(fd, "    start \"dummy\" /b ");
	// Always use quotes, $VIM or $VIMRUNTIME might have a space.
	fprintf(fd, "\"%%VIM_EXE_DIR%%\\%s\" %s %%*\n",
		exename, vimarg);
	fprintf(fd, ")\n");
    }
    ಅನ್ಯಥಾ
    {
	// Always use quotes, $VIM or $VIMRUNTIME might have a space.
	fprintf(fd, "\"%%VIM_EXE_DIR%%\\%s\" %s %%*\n",
		exename, vimarg);
    }

    fclose(fd);
    printf("%s has been %s\n", batpath,
	    oldname == ರಿಕ್ತ ? "created" : "overwritten");
}

/*
 * Make the text string for choice "idx".
 * The format "fmt" is must have one %s item, which "arg" is used for.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
alloc_text(ಪೂರ್ಣಾಙ್ಕ idx, ವರ್ಣ *fmt, ವರ್ಣ *arg)
{
    ಯದಿ (choices[idx].text != ರಿಕ್ತ)
	ಮುಕ್ತ(choices[idx].text);

    choices[idx].text = alloc(strlen(fmt) + strlen(arg) - 1);
    sprintf(choices[idx].text, fmt, arg);
}

/*
 * Toggle the "Overwrite .../vim.bat" to "Don't overwrite".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
toggle_bat_choice(ಪೂರ್ಣಾಙ್ಕ idx)
{
    ವರ್ಣ	*batname = targets[choices[idx].arg].batpath;
    ವರ್ಣ	*oldname = targets[choices[idx].arg].oldbat;

    ಯದಿ (*batname == NUL)
    {
	alloc_text(idx, "    Overwrite %s", oldname);
	strcpy(batname, oldname);
    }
    ಅನ್ಯಥಾ
    {
	alloc_text(idx, "    Do NOT overwrite %s", oldname);
	*batname = NUL;
    }
}

/*
 * Do some work for a batch file entry: Append the batch file name to the path
 * and set the text for the choice.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_bat_text(ಪೂರ್ಣಾಙ್ಕ idx, ವರ್ಣ *batpath, ವರ್ಣ *name)
{
    strcat(batpath, name);

    alloc_text(idx, "    Create %s", batpath);
}

/*
 * Select a directory to write the batch file line.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
change_bat_choice(ಪೂರ್ಣಾಙ್ಕ idx)
{
    ವರ್ಣ	*path;
    ವರ್ಣ	*batpath;
    ವರ್ಣ	*name;
    ಪೂರ್ಣಾಙ್ಕ		n;
    ವರ್ಣ	*s;
    ವರ್ಣ	*p;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ;
    ವರ್ಣ	**names = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		target = choices[idx].arg;

    name = targets[target].batname;
    batpath = targets[target].batpath;

    path = getenv("PATH");
    ಯದಿ (path == ರಿಕ್ತ)
    {
	printf("\nERROR: The variable $PATH is not set\n");
	ಪ್ರತ್ಯಾಗಮ;
    }

    /*
     * first round: count number of names in path;
     * second round: save names to names[].
     */
    ಪ್ರತಿ (;;)
    {
	ಎಣಿಕೆ = 1;
	ಪ್ರತಿ (p = path; *p; )
	{
	    s = strchr(p, ';');
	    ಯದಿ (s == ರಿಕ್ತ)
		s = p + strlen(p);
	    ಯದಿ (names != ರಿಕ್ತ)
	    {
		names[ಎಣಿಕೆ] = alloc(s - p + 1);
		strncpy(names[ಎಣಿಕೆ], p, s - p);
		names[ಎಣಿಕೆ][s - p] = NUL;
	    }
	    ++ಎಣಿಕೆ;
	    p = s;
	    ಯದಿ (*p != NUL)
		++p;
	}
	ಯದಿ (names != ರಿಕ್ತ)
	    ಭಙ್ಗ;
	names = alloc((ಎಣಿಕೆ + 1) * ಪರಿಮಾಣ(ವರ್ಣ *));
    }
    names[0] = alloc(50);
    sprintf(names[0], "Select directory to create %s in:", name);
    names[ಎಣಿಕೆ] = alloc(50);
    ಯದಿ (choices[idx].arg == 0)
	sprintf(names[ಎಣಿಕೆ], "Do not create any .bat file.");
    ಅನ್ಯಥಾ
	sprintf(names[ಎಣಿಕೆ], "Do not create a %s file.", name);
    n = get_choice(names, ಎಣಿಕೆ + 1);

    ಯದಿ (n == ಎಣಿಕೆ)
    {
	// Selected last item, don't create bat file.
	*batpath = NUL;
	ಯದಿ (choices[idx].arg != 0)
	    alloc_text(idx, "    Do NOT create %s", name);
    }
    ಅನ್ಯಥಾ
    {
	// Selected one of the paths.  For the first item only keep the path,
	// for the others append the batch file name.
	strcpy(batpath, names[n]);
	add_pathsep(batpath);
	ಯದಿ (choices[idx].arg != 0)
	    set_bat_text(idx, batpath, name);
    }

    ಪ್ರತಿ (i = 0; i <= ಎಣಿಕೆ; ++i)
	ಮುಕ್ತ(names[i]);
    ಮುಕ್ತ(names);
}

ವರ್ಣ *bat_text_yes = "Install .bat files to use Vim at the command line:";
ವರ್ಣ *bat_text_no = "do NOT install .bat files to use Vim at the command line";

    ಸ್ಥಿರ ಶೂನ್ಯ
change_main_bat_choice(ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    // let the user select a default directory or NONE
    change_bat_choice(idx);

    ಯದಿ (targets[0].batpath[0] != NUL)
	choices[idx].text = bat_text_yes;
    ಅನ್ಯಥಾ
	choices[idx].text = bat_text_no;

    // update the individual batch file selections
    ಪ್ರತಿ (i = 1; i < TARGET_ಎಣಿಕೆ; ++i)
    {
	// Only make it active when the first item has a path and the vim.exe
	// or gvim.exe exists (there is a changefunc then).
	ಯದಿ (targets[0].batpath[0] != NUL
		&& choices[idx + i].changefunc != ರಿಕ್ತ)
	{
	    choices[idx + i].active = 1;
	    ಯದಿ (choices[idx + i].changefunc == change_bat_choice
		    && targets[i].batpath[0] != NUL)
	    {
		strcpy(targets[i].batpath, targets[0].batpath);
		set_bat_text(idx + i, targets[i].batpath, targets[i].batname);
	    }
	}
	ಅನ್ಯಥಾ
	    choices[idx + i].active = 0;
    }
}

/*
 * Initialize a choice for creating a batch file.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
init_bat_choice(ಪೂರ್ಣಾಙ್ಕ target)
{
    ವರ್ಣ	*batpath = targets[target].batpath;
    ವರ್ಣ	*oldbat = targets[target].oldbat;
    ವರ್ಣ	*p;
    ಪೂರ್ಣಾಙ್ಕ		i;

    choices[choice_ಎಣಿಕೆ].arg = target;
    choices[choice_ಎಣಿಕೆ].installfunc = install_bat_choice;
    choices[choice_ಎಣಿಕೆ].active = 1;
    choices[choice_ಎಣಿಕೆ].text = ರಿಕ್ತ;	// will be set below
    ಯದಿ (oldbat != ರಿಕ್ತ)
    {
	// A [g]vim.bat exists: Only choice is to overwrite it or not.
	choices[choice_ಎಣಿಕೆ].changefunc = toggle_bat_choice;
	*batpath = NUL;
	toggle_bat_choice(choice_ಎಣಿಕೆ);
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (default_bat_dir != ರಿಕ್ತ)
	    // Prefer using the same path as an existing .bat file.
	    strcpy(batpath, default_bat_dir);
	ಅನ್ಯಥಾ
	{
	    // No [g]vim.bat exists: Write it to a directory in $PATH.  Use
	    // $WINDIR by default, if it's empty the first item in $PATH.
	    p = getenv("WINDIR");
	    ಯದಿ (p != ರಿಕ್ತ && *p != NUL)
		strcpy(batpath, p);
	    ಅನ್ಯಥಾ
	    {
		p = getenv("PATH");
		ಯದಿ (p == ರಿಕ್ತ || *p == NUL)		// "cannot happen"
		    strcpy(batpath, "C:/Windows");
		ಅನ್ಯಥಾ
		{
		    i = 0;
		    ಯಾವತ್ (*p != NUL && *p != ';')
			batpath[i++] = *p++;
		    batpath[i] = NUL;
		}
	    }
	}
	add_pathsep(batpath);
	set_bat_text(choice_ಎಣಿಕೆ, batpath, targets[target].batname);

	choices[choice_ಎಣಿಕೆ].changefunc = change_bat_choice;
    }
    ++choice_ಎಣಿಕೆ;
}

/*
 * Set up the choices for installing .bat files.
 * For these items "arg" is the index in targets[].
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
init_bat_choices(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    // The first item is used to switch installing batch files on/off and
    // setting the default path.
    choices[choice_ಎಣಿಕೆ].text = bat_text_yes;
    choices[choice_ಎಣಿಕೆ].changefunc = change_main_bat_choice;
    choices[choice_ಎಣಿಕೆ].installfunc = ರಿಕ್ತ;
    choices[choice_ಎಣಿಕೆ].active = 1;
    choices[choice_ಎಣಿಕೆ].arg = 0;
    ++choice_ಎಣಿಕೆ;

    // Add items for each batch file target.  Only used when not disabled by
    // the first item.  When a .exe exists, don't offer to create a .bat.
    ಪ್ರತಿ (i = 1; i < TARGET_ಎಣಿಕೆ; ++i)
	ಯದಿ (targets[i].oldexe == ರಿಕ್ತ
		&& (targets[i].exenamearg[0] == 'g' ? has_gvim : has_vim))
	    init_bat_choice(i);
	ಅನ್ಯಥಾ
	    add_dummy_choice();
}

/*
 * Install the vimrc file.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
install_vimrc(ಪೂರ್ಣಾಙ್ಕ idx UNUSED)
{
    FILE	*fd, *tfd;
    ವರ್ಣ	*fname;

    // If an old vimrc file exists, overwrite it.
    // Otherwise create a new one.
    ಯದಿ (*oldvimrc != NUL)
	fname = oldvimrc;
    ಅನ್ಯಥಾ
	fname = vimrc;

    fd = fopen(fname, "w");
    ಯದಿ (fd == ರಿಕ್ತ)
    {
	printf("\nERROR: Cannot open \"%s\" for writing.\n", fname);
	ಪ್ರತ್ಯಾಗಮ;
    }
    ವಿಕಲ್ಪ (compat_choice)
    {
	ಪಕ್ಷ compat_vi:
		    fprintf(fd, "\" Vi compatible\n");
		    fprintf(fd, "set compatible\n");
		    ಭಙ್ಗ;
	ಪಕ್ಷ compat_vim:
		    fprintf(fd, "\" Vim's default behavior\n");
		    fprintf(fd, "if &compatible\n");
		    fprintf(fd, "  set nocompatible\n");
		    fprintf(fd, "endif\n");
		    ಭಙ್ಗ;
	ಪಕ್ಷ compat_some_enhancements:
		    fprintf(fd, "\" Vim with some enhancements\n");
		    fprintf(fd, "source $VIMRUNTIME/defaults.vim\n");
		    ಭಙ್ಗ;
	ಪಕ್ಷ compat_all_enhancements:
		    fprintf(fd, "\" Vim with all enhancements\n");
		    fprintf(fd, "source $VIMRUNTIME/vimrc_example.vim\n");
		    ಭಙ್ಗ;
    }
    ವಿಕಲ್ಪ (remap_choice)
    {
	ಪಕ್ಷ remap_no:
		    ಭಙ್ಗ;
	ಪಕ್ಷ remap_win:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Remap a few keys for Windows behavior\n");
		    fprintf(fd, "source $VIMRUNTIME/mswin.vim\n");
		    ಭಙ್ಗ;
    }
    ವಿಕಲ್ಪ (mouse_choice)
    {
	ಪಕ್ಷ mouse_xterm:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Mouse behavior (the Unix way)\n");
		    fprintf(fd, "behave xterm\n");
		    ಭಙ್ಗ;
	ಪಕ್ಷ mouse_mswin:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Mouse behavior (the Windows way)\n");
		    fprintf(fd, "behave mswin\n");
		    ಭಙ್ಗ;
	ಪಕ್ಷ mouse_default:
		    ಭಙ್ಗ;
    }
    ಯದಿ ((tfd = fopen("diff.exe", "r")) != ರಿಕ್ತ)
    {
	// Use the diff.exe that comes with the self-extracting gvim.exe.
	fclose(tfd);
	fprintf(fd, "\n");
	fprintf(fd, "\" Use the internal diff if available.\n");
	fprintf(fd, "\" Otherwise use the special 'diffexpr' for Windows.\n");
	fprintf(fd, "if &diffopt !~# 'internal'\n");
	fprintf(fd, "  set diffexpr=MyDiff()\n");
	fprintf(fd, "endif\n");
	fprintf(fd, "function MyDiff()\n");
	fprintf(fd, "  let opt = '-a --binary '\n");
	fprintf(fd, "  if &diffopt =~ 'icase' | let opt = opt . '-i ' | endif\n");
	fprintf(fd, "  if &diffopt =~ 'iwhite' | let opt = opt . '-b ' | endif\n");
	// Use quotes only when needed, they may cause trouble.
	// Always escape "!".
	fprintf(fd, "  let arg1 = v:fname_in\n");
	fprintf(fd, "  if arg1 =~ ' ' | let arg1 = '\"' . arg1 . '\"' | endif\n");
	fprintf(fd, "  let arg1 = substitute(arg1, '!', '\\!', 'g')\n");
	fprintf(fd, "  let arg2 = v:fname_new\n");
	fprintf(fd, "  if arg2 =~ ' ' | let arg2 = '\"' . arg2 . '\"' | endif\n");
	fprintf(fd, "  let arg2 = substitute(arg2, '!', '\\!', 'g')\n");
	fprintf(fd, "  let arg3 = v:fname_out\n");
	fprintf(fd, "  if arg3 =~ ' ' | let arg3 = '\"' . arg3 . '\"' | endif\n");
	fprintf(fd, "  let arg3 = substitute(arg3, '!', '\\!', 'g')\n");

	// If the path has a space:  When using cmd.exe (Win NT/2000/XP) put
	// quotes around the diff command and rely on the default value of
	// shellxquote to solve the quoting problem for the whole command.
	//
	// Otherwise put a double quote just before the space and at the
	// end of the command.  Putting quotes around the whole thing
	// doesn't work on Win 95/98/ME.  This is mostly guessed!
	fprintf(fd, "  if $VIMRUNTIME =~ ' '\n");
	fprintf(fd, "    if &sh =~ '\\<cmd'\n");
	fprintf(fd, "      if empty(&shellxquote)\n");
	fprintf(fd, "        let l:shxq_sav = ''\n");
	fprintf(fd, "        set shellxquote&\n");
	fprintf(fd, "      endif\n");
	fprintf(fd, "      let cmd = '\"' . $VIMRUNTIME . '\\diff\"'\n");
	fprintf(fd, "    else\n");
	fprintf(fd, "      let cmd = substitute($VIMRUNTIME, ' ', '\" ', '') . '\\diff\"'\n");
	fprintf(fd, "    endif\n");
	fprintf(fd, "  else\n");
	fprintf(fd, "    let cmd = $VIMRUNTIME . '\\diff'\n");
	fprintf(fd, "  endif\n");
	fprintf(fd, "  let cmd = substitute(cmd, '!', '\\!', 'g')\n");
	fprintf(fd, "  silent execute '!' . cmd . ' ' . opt . arg1 . ' ' . arg2 . ' > ' . arg3\n");
	fprintf(fd, "  if exists('l:shxq_sav')\n");
	fprintf(fd, "    let &shellxquote=l:shxq_sav\n");
	fprintf(fd, "  endif\n");
	fprintf(fd, "endfunction\n");
	fprintf(fd, "\n");
    }
    fclose(fd);
    printf("%s has been written\n", fname);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
change_vimrc_choice(ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (choices[idx].installfunc != ರಿಕ್ತ)
    {
	// Switch to NOT change or create a vimrc file.
	ಯದಿ (*oldvimrc != NUL)
	    alloc_text(idx, "Do NOT change startup file %s", oldvimrc);
	ಅನ್ಯಥಾ
	    alloc_text(idx, "Do NOT create startup file %s", vimrc);
	choices[idx].installfunc = ರಿಕ್ತ;
	choices[idx + 1].active = 0;
	choices[idx + 2].active = 0;
	choices[idx + 3].active = 0;
    }
    ಅನ್ಯಥಾ
    {
	// Switch to change or create a vimrc file.
	ಯದಿ (*oldvimrc != NUL)
	    alloc_text(idx, "Overwrite startup file %s with:", oldvimrc);
	ಅನ್ಯಥಾ
	    alloc_text(idx, "Create startup file %s with:", vimrc);
	choices[idx].installfunc = install_vimrc;
	choices[idx + 1].active = 1;
	choices[idx + 2].active = 1;
	choices[idx + 3].active = 1;
    }
}

/*
 * Change the choice how to run Vim.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
change_run_choice(ಪೂರ್ಣಾಙ್ಕ idx)
{
    compat_choice = get_choice(compat_choices, TABLE_SIZE(compat_choices));
    alloc_text(idx, compat_text, compat_choices[compat_choice]);
}

/*
 * Change the choice if keys are to be remapped.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
change_remap_choice(ಪೂರ್ಣಾಙ್ಕ idx)
{
    remap_choice = get_choice(remap_choices, TABLE_SIZE(remap_choices));
    alloc_text(idx, remap_text, remap_choices[remap_choice]);
}

/*
 * Change the choice how to select text.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
change_mouse_choice(ಪೂರ್ಣಾಙ್ಕ idx)
{
    mouse_choice = get_choice(mouse_choices, TABLE_SIZE(mouse_choices));
    alloc_text(idx, mouse_text, mouse_choices[mouse_choice]);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
init_vimrc_choices(ಶೂನ್ಯ)
{
    // set path for a new _vimrc file (also when not used)
    strcpy(vimrc, installdir);
    strcpy(vimrc + runtimeidx, "_vimrc");

    // Set opposite value and then toggle it by calling change_vimrc_choice()
    ಯದಿ (*oldvimrc == NUL)
	choices[choice_ಎಣಿಕೆ].installfunc = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
	choices[choice_ಎಣಿಕೆ].installfunc = install_vimrc;
    choices[choice_ಎಣಿಕೆ].text = ರಿಕ್ತ;
    change_vimrc_choice(choice_ಎಣಿಕೆ);
    choices[choice_ಎಣಿಕೆ].changefunc = change_vimrc_choice;
    choices[choice_ಎಣಿಕೆ].active = 1;
    ++choice_ಎಣಿಕೆ;

    // default way to run Vim
    alloc_text(choice_ಎಣಿಕೆ, compat_text, compat_choices[compat_choice]);
    choices[choice_ಎಣಿಕೆ].changefunc = change_run_choice;
    choices[choice_ಎಣಿಕೆ].installfunc = ರಿಕ್ತ;
    choices[choice_ಎಣಿಕೆ].active = (*oldvimrc == NUL);
    ++choice_ಎಣಿಕೆ;

    // Whether to remap keys
    alloc_text(choice_ಎಣಿಕೆ, remap_text , remap_choices[remap_choice]);
    choices[choice_ಎಣಿಕೆ].changefunc = change_remap_choice;
    choices[choice_ಎಣಿಕೆ].installfunc = ರಿಕ್ತ;
    choices[choice_ಎಣಿಕೆ].active = (*oldvimrc == NUL);
    ++choice_ಎಣಿಕೆ;

    // default way to use the mouse
    alloc_text(choice_ಎಣಿಕೆ, mouse_text, mouse_choices[mouse_choice]);
    choices[choice_ಎಣಿಕೆ].changefunc = change_mouse_choice;
    choices[choice_ಎಣಿಕೆ].installfunc = ರಿಕ್ತ;
    choices[choice_ಎಣಿಕೆ].active = (*oldvimrc == NUL);
    ++choice_ಎಣಿಕೆ;
}

    ಸ್ಥಿರ LONG
reg_create_ಕೀಲಿ(
    HKEY ಬೇರು,
    ಧ್ರುವ ವರ್ಣ *subkey,
    PHKEY ph_ಕೀಲಿ,
    DWORD flag)
{
    DWORD disp;

    *ph_ಕೀಲಿ = ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ Reg_Create_ಕೀಲಿ_Ex(
		ಬೇರು, subkey,
		0, ರಿಕ್ತ, REG_OPTION_NON_VOLATILE,
		flag | ಕೀಲಿ_WRITE,
		ರಿಕ್ತ, ph_ಕೀಲಿ, &disp);
}

    ಸ್ಥಿರ LONG
reg_set_string_ಮೌಲ್ಯ(
    HKEY h_ಕೀಲಿ,
    ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ_name,
    ಧ್ರುವ ವರ್ಣ *data)
{
    ಪ್ರತ್ಯಾಗಮ Reg_Set_ಮೌಲ್ಯ_Ex(h_ಕೀಲಿ, ಮೌಲ್ಯ_name, 0, REG_SZ,
				     (LPBYTE)data, (DWORD)(1 + strlen(data)));
}

    ಸ್ಥಿರ LONG
reg_create_ಕೀಲಿ_and_ಮೌಲ್ಯ(
    HKEY h_ಬೇರು_ಕೀಲಿ,
    ಧ್ರುವ ವರ್ಣ *subkey,
    ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ_name,
    ಧ್ರುವ ವರ್ಣ *data,
    DWORD flag)
{
    HKEY h_ಕೀಲಿ;
    LONG lRet = reg_create_ಕೀಲಿ(h_ಬೇರು_ಕೀಲಿ, subkey, &h_ಕೀಲಿ, flag);

    ಯದಿ (ERROR_SUCCESS == lRet)
    {
	lRet = reg_set_string_ಮೌಲ್ಯ(h_ಕೀಲಿ, ಮೌಲ್ಯ_name, data);
	Reg_Close_ಕೀಲಿ(h_ಕೀಲಿ);
    }
    ಪ್ರತ್ಯಾಗಮ lRet;
}

    ಸ್ಥಿರ LONG
register_inproc_server(
    HKEY h_ಬೇರು_ಕೀಲಿ,
    ಧ್ರುವ ವರ್ಣ *clsid,
    ಧ್ರುವ ವರ್ಣ *extname,
    ಧ್ರುವ ವರ್ಣ *module,
    ಧ್ರುವ ವರ್ಣ *threading_model,
    DWORD flag)
{
    CHAR subkey[BUFSIZE];
    LONG lRet;

    sprintf(subkey, "CLSID\\%s", clsid);
    lRet = reg_create_ಕೀಲಿ_and_ಮೌಲ್ಯ(h_ಬೇರು_ಕೀಲಿ, subkey, ರಿಕ್ತ, extname, flag);
    ಯದಿ (ERROR_SUCCESS == lRet)
    {
	sprintf(subkey, "CLSID\\%s\\InProcServer32", clsid);
	lRet = reg_create_ಕೀಲಿ_and_ಮೌಲ್ಯ(h_ಬೇರು_ಕೀಲಿ, subkey, ರಿಕ್ತ, module, flag);
	ಯದಿ (ERROR_SUCCESS == lRet)
	{
	    lRet = reg_create_ಕೀಲಿ_and_ಮೌಲ್ಯ(h_ಬೇರು_ಕೀಲಿ, subkey,
					   "ThreadingModel", threading_model, flag);
	}
    }
    ಪ್ರತ್ಯಾಗಮ lRet;
}

    ಸ್ಥಿರ LONG
register_shellex(
    HKEY h_ಬೇರು_ಕೀಲಿ,
    ಧ್ರುವ ವರ್ಣ *clsid,
    ಧ್ರುವ ವರ್ಣ *name,
    ಧ್ರುವ ವರ್ಣ *exe_path,
    DWORD flag)
{
    LONG lRet = reg_create_ಕೀಲಿ_and_ಮೌಲ್ಯ(
	    h_ಬೇರು_ಕೀಲಿ,
	    "*\\shellex\\ContextMenuHandlers\\gvim",
	    ರಿಕ್ತ,
	    clsid,
	    flag);

    ಯದಿ (ERROR_SUCCESS == lRet)
    {
	lRet = reg_create_ಕೀಲಿ_and_ಮೌಲ್ಯ(
		HKEY_LOCAL_MACHINE,
		"Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved",
		clsid,
		name,
		flag);

	ಯದಿ (ERROR_SUCCESS == lRet)
	{
	    lRet = reg_create_ಕೀಲಿ_and_ಮೌಲ್ಯ(
		    HKEY_LOCAL_MACHINE,
		    "Software\\Vim\\Gvim",
		    "path",
		    exe_path,
		    flag);
	}
    }
    ಪ್ರತ್ಯಾಗಮ lRet;
}

    ಸ್ಥಿರ LONG
register_openwith(
    HKEY h_ಬೇರು_ಕೀಲಿ,
    ಧ್ರುವ ವರ್ಣ *exe_path,
    DWORD flag)
{
    ವರ್ಣ	exe_cmd[BUFSIZE];
    LONG	lRet;

    sprintf(exe_cmd, "\"%s\" \"%%1\"", exe_path);
    lRet = reg_create_ಕೀಲಿ_and_ಮೌಲ್ಯ(
	    h_ಬೇರು_ಕೀಲಿ,
	    "Applications\\gvim.exe\\shell\\edit\\command",
	    ರಿಕ್ತ,
	    exe_cmd,
	    flag);

    ಯದಿ (ERROR_SUCCESS == lRet)
    {
	ಪೂರ್ಣಾಙ್ಕ i;
	ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *openwith[] = {
		".htm\\OpenWithList\\gvim.exe",
		".vim\\OpenWithList\\gvim.exe",
		"*\\OpenWithList\\gvim.exe",
	};

	ಪ್ರತಿ (i = 0; ERROR_SUCCESS == lRet && i < (ಪೂರ್ಣಾಙ್ಕ)ARRAYSIZE(openwith); i++)
	    lRet = reg_create_ಕೀಲಿ_and_ಮೌಲ್ಯ(h_ಬೇರು_ಕೀಲಿ, openwith[i], ರಿಕ್ತ, "", flag);
    }

    ಪ್ರತ್ಯಾಗಮ lRet;
}

    ಸ್ಥಿರ LONG
register_uninstall(
    HKEY h_ಬೇರು_ಕೀಲಿ,
    ಧ್ರುವ ವರ್ಣ *appname,
    ಧ್ರುವ ವರ್ಣ *display_name,
    ಧ್ರುವ ವರ್ಣ *uninstall_string,
    ಧ್ರುವ ವರ್ಣ *display_icon,
    ಧ್ರುವ ವರ್ಣ *display_version,
    ಧ್ರುವ ವರ್ಣ *publisher)
{
    LONG lRet = reg_create_ಕೀಲಿ_and_ಮೌಲ್ಯ(h_ಬೇರು_ಕೀಲಿ, appname,
			     "DisplayName", display_name, ಕೀಲಿ_WOW64_64_ಕೀಲಿ);

    ಯದಿ (ERROR_SUCCESS == lRet)
	lRet = reg_create_ಕೀಲಿ_and_ಮೌಲ್ಯ(h_ಬೇರು_ಕೀಲಿ, appname,
		     "UninstallString", uninstall_string, ಕೀಲಿ_WOW64_64_ಕೀಲಿ);
    ಯದಿ (ERROR_SUCCESS == lRet)
	lRet = reg_create_ಕೀಲಿ_and_ಮೌಲ್ಯ(h_ಬೇರು_ಕೀಲಿ, appname,
		     "DisplayIcon", display_icon, ಕೀಲಿ_WOW64_64_ಕೀಲಿ);
    ಯದಿ (ERROR_SUCCESS == lRet)
	lRet = reg_create_ಕೀಲಿ_and_ಮೌಲ್ಯ(h_ಬೇರು_ಕೀಲಿ, appname,
		     "DisplayVersion", display_version, ಕೀಲಿ_WOW64_64_ಕೀಲಿ);
    ಯದಿ (ERROR_SUCCESS == lRet)
	lRet = reg_create_ಕೀಲಿ_and_ಮೌಲ್ಯ(h_ಬೇರು_ಕೀಲಿ, appname,
		     "Publisher", publisher, ಕೀಲಿ_WOW64_64_ಕೀಲಿ);
    ಪ್ರತ್ಯಾಗಮ lRet;
}

/*
 * Add some entries to the registry:
 * - to add "Edit with Vim" to the context * menu
 * - to add Vim to the "Open with..." list
 * - to uninstall Vim
 */
//ARGSUSED
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
install_registry(ಶೂನ್ಯ)
{
    LONG	lRet = ERROR_SUCCESS;
    ಧ್ರುವ ವರ್ಣ	*vim_ext_ThreadingModel = "Apartment";
    ಧ್ರುವ ವರ್ಣ	*vim_ext_name = "Vim Shell Extension";
    ಧ್ರುವ ವರ್ಣ	*vim_ext_clsid = "{51EEE242-AD87-11d3-9C1E-0090278BBD99}";
    ವರ್ಣ	vim_exe_path[MAX_PATH];
    ವರ್ಣ	display_name[BUFSIZE];
    ವರ್ಣ	uninstall_string[BUFSIZE];
    ವರ್ಣ	icon_string[BUFSIZE];
    ವರ್ಣ	version_string[BUFSIZE];
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		loop_ಎಣಿಕೆ = is_64bit_os() ? 2 : 1;
    DWORD	flag;

    sprintf(vim_exe_path, "%s\\gvim.exe", installdir);

    ಯದಿ (install_popup)
    {
	ವರ್ಣ	    bufg[BUFSIZE];

	printf("Creating \"Edit with Vim\" popup menu entry\n");

	ಪ್ರತಿ (i = 0; i < loop_ಎಣಿಕೆ; i++)
	{
	    ಯದಿ (i == 0)
	    {
		sprintf(bufg, "%s\\" GVIMEXT32_PATH, installdir);
		flag = ಕೀಲಿ_WOW64_32_ಕೀಲಿ;
	    }
	    ಅನ್ಯಥಾ
	    {
		sprintf(bufg, "%s\\" GVIMEXT64_PATH, installdir);
		flag = ಕೀಲಿ_WOW64_64_ಕೀಲಿ;
	    }

	    lRet = register_inproc_server(
		    HKEY_CLASSES_ಬೇರು, vim_ext_clsid, vim_ext_name,
		    bufg, vim_ext_ThreadingModel, flag);
	    ಯದಿ (ERROR_SUCCESS != lRet)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    lRet = register_shellex(
		    HKEY_CLASSES_ಬೇರು, vim_ext_clsid, vim_ext_name,
		    vim_exe_path, flag);
	    ಯದಿ (ERROR_SUCCESS != lRet)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }

    ಯದಿ (install_openwith)
    {
	printf("Creating \"Open with ...\" list entry\n");

	ಪ್ರತಿ (i = 0; i < loop_ಎಣಿಕೆ; i++)
	{
	    ಯದಿ (i == 0)
		flag = ಕೀಲಿ_WOW64_32_ಕೀಲಿ;
	    ಅನ್ಯಥಾ
		flag = ಕೀಲಿ_WOW64_64_ಕೀಲಿ;

	    lRet = register_openwith(HKEY_CLASSES_ಬೇರು, vim_exe_path, flag);
	    ಯದಿ (ERROR_SUCCESS != lRet)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }

    printf("Creating an uninstall entry\n");
    sprintf(display_name, "Vim " VIM_VERSION_SHORT
#ifdef _M_ARM64
	    " (arm64)"
#ನೋಚೇದ್ಯದಿ _M_X64
	    " (x64)"
#endif
	    );

    // For the NSIS installer use the generated uninstaller.
    ಯದಿ (interactive)
	sprintf(uninstall_string, "%s\\uninstall.exe", installdir);
    ಅನ್ಯಥಾ
	sprintf(uninstall_string, "%s\\uninstall-gui.exe", installdir);

    sprintf(icon_string, "%s\\gvim.exe,0", installdir);

    sprintf(version_string, VIM_VERSION_SHORT "." VIM_VERSION_PATCHLEVEL_STR);

    lRet = register_uninstall(
	HKEY_LOCAL_MACHINE,
	"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Vim " VIM_VERSION_SHORT,
	display_name,
	uninstall_string,
	icon_string,
	version_string,
	"The Vim Project");
    ಯದಿ (ERROR_SUCCESS != lRet)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
change_popup_choice(ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (install_popup == 0)
    {
	choices[idx].text = "Install an entry for Vim in the popup menu for the right\n    mouse button so that you can edit any file with Vim";
	install_popup = 1;
    }
    ಅನ್ಯಥಾ
    {
	choices[idx].text = "Do NOT install an entry for Vim in the popup menu for the\n    right mouse button to edit any file with Vim";
	install_popup = 0;
    }
}

/*
 * Only add the choice for the popup menu entry when gvim.exe was found and
 * both gvimext.dll and regedit.exe exist.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
init_popup_choice(ಶೂನ್ಯ)
{
    ಸಂರಚನಾ stat	st;

    ಯದಿ (has_gvim
	    && (stat(GVIMEXT32_PATH, &st) >= 0
		|| stat(GVIMEXT64_PATH, &st) >= 0))
    {
	choices[choice_ಎಣಿಕೆ].changefunc = change_popup_choice;
	choices[choice_ಎಣಿಕೆ].installfunc = ರಿಕ್ತ;
	choices[choice_ಎಣಿಕೆ].active = 1;
	change_popup_choice(choice_ಎಣಿಕೆ);  // set the text
	++choice_ಎಣಿಕೆ;
    }
    ಅನ್ಯಥಾ
	add_dummy_choice();
}

    ಸ್ಥಿರ ಶೂನ್ಯ
change_openwith_choice(ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (install_openwith == 0)
    {
	choices[idx].text = "Add Vim to the \"Open With...\" list in the popup menu for the right\n    mouse button so that you can edit any file with Vim";
	install_openwith = 1;
    }
    ಅನ್ಯಥಾ
    {
	choices[idx].text = "Do NOT add Vim to the \"Open With...\" list in the popup menu for the\n    right mouse button to edit any file with Vim";
	install_openwith = 0;
    }
}

/*
 * Only add the choice for the open-with menu entry when gvim.exe was found
 * and regedit.exe exist.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
init_openwith_choice(ಶೂನ್ಯ)
{
    ಯದಿ (has_gvim)
    {
	choices[choice_ಎಣಿಕೆ].changefunc = change_openwith_choice;
	choices[choice_ಎಣಿಕೆ].installfunc = ರಿಕ್ತ;
	choices[choice_ಎಣಿಕೆ].active = 1;
	change_openwith_choice(choice_ಎಣಿಕೆ);  // set the text
	++choice_ಎಣಿಕೆ;
    }
    ಅನ್ಯಥಾ
	add_dummy_choice();
}

/*
 * Create a shell link.
 *
 * returns 0 on failure, non-zero on successful completion.
 *
 * NOTE:  Currently untested with mingw.
 */
    ಪೂರ್ಣಾಙ್ಕ
create_shortcut(
	ಧ್ರುವ ವರ್ಣ *shortcut_name,
	ಧ್ರುವ ವರ್ಣ *iconfile_path,
	ಪೂರ್ಣಾಙ್ಕ	    iconindex,
	ಧ್ರುವ ವರ್ಣ *shortcut_target,
	ಧ್ರುವ ವರ್ಣ *shortcut_args,
	ಧ್ರುವ ವರ್ಣ *workingdir
	)
{
    IShellLink	    *shelllink_ptr;
    HRESULT	    hres;
    IPersistFile	    *persistfile_ptr;

    // Initialize COM library
    hres = CoInitialize(ರಿಕ್ತ);
    ಯದಿ (!SUCCEEDED(hres))
    {
	printf("Error:  Could not open the COM library.  Not creating shortcut.\n");
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // Instantiate a COM object for the ShellLink, store a pointer to it
    // in shelllink_ptr.
    hres = CoCreateInstance(&CLSID_ShellLink,
			   ರಿಕ್ತ,
			   CLSCTX_INPROC_SERVER,
			   &IID_IShellLink,
			   (ಶೂನ್ಯ **) &shelllink_ptr);

    ಯದಿ (SUCCEEDED(hres)) // If the instantiation was successful...
    {
	// ...Then build a PersistFile interface for the ShellLink so we can
	// save it as a file after we build it.
	hres = shelllink_ptr->lpVtbl->QueryInterface(shelllink_ptr,
		&IID_IPersistFile, (ಶೂನ್ಯ **) &persistfile_ptr);

	ಯದಿ (SUCCEEDED(hres))
	{
	    wchar_t wsz[BUFSIZE];

	    // translate the (possibly) multibyte shortcut filename to windows
	    // Unicode so it can be used as a file name.
	    MultiByteToWideChar(CP_ACP, 0, shortcut_name, -1, wsz, ಪರಿಮಾಣ(wsz)/ಪರಿಮಾಣ(wsz[0]));

	    // set the attributes
	    shelllink_ptr->lpVtbl->SetPath(shelllink_ptr, shortcut_target);
	    shelllink_ptr->lpVtbl->SetWorkingDirectory(shelllink_ptr,
								  workingdir);
	    shelllink_ptr->lpVtbl->SetIconLocation(shelllink_ptr,
						    iconfile_path, iconindex);
	    shelllink_ptr->lpVtbl->SetArguments(shelllink_ptr, shortcut_args);

	    // save the shortcut to a file and return the PersistFile object
	    persistfile_ptr->lpVtbl->Save(persistfile_ptr, wsz, 1);
	    persistfile_ptr->lpVtbl->Release(persistfile_ptr);
	}
	ಅನ್ಯಥಾ
	{
	    printf("QueryInterface Error\n");
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	// Return the ShellLink object
	shelllink_ptr->lpVtbl->Release(shelllink_ptr);
    }
    ಅನ್ಯಥಾ
    {
	printf("CoCreateInstance Error - hres = %08x\n", (ಪೂರ್ಣಾಙ್ಕ)hres);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Build a path to where we will put a specified link.
 *
 * Return 0 on error, non-zero on success
 */
   ಪೂರ್ಣಾಙ್ಕ
build_link_name(
	ವರ್ಣ	   *link_path,
	ಧ್ರುವ ವರ್ಣ *link_name,
	ಧ್ರುವ ವರ್ಣ *shell_folder_name)
{
    ವರ್ಣ	shell_folder_path[MAX_PATH];

    ಯದಿ (get_shell_folder_path(shell_folder_path, shell_folder_name) == FAIL)
    {
	printf("An error occurred while attempting to find the path to %s.\n",
							   shell_folder_name);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // Make sure the directory exists (create Start Menu\Programs\Vim).
    // Ignore errors if it already exists.
    vim_mkdir(shell_folder_path, 0755);

    // build the path to the shortcut and the path to gvim.exe
    sprintf(link_path, "%s\\%s.lnk", shell_folder_path, link_name);

    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
build_shortcut(
	ಧ್ರುವ ವರ್ಣ *name,	// Name of the shortcut
	ಧ್ರುವ ವರ್ಣ *exename,	// Name of the executable (e.g., vim.exe)
	ಧ್ರುವ ವರ್ಣ *args,
	ಧ್ರುವ ವರ್ಣ *shell_folder,
	ಧ್ರುವ ವರ್ಣ *workingdir)
{
    ವರ್ಣ	executable_path[BUFSIZE];
    ವರ್ಣ	link_name[BUFSIZE];

    sprintf(executable_path, "%s\\%s", installdir, exename);

    ಯದಿ (build_link_name(link_name, name, shell_folder) == FAIL)
    {
	printf("An error has occurred.  A shortcut to %s will not be created %s.\n",
		name,
		*shell_folder == 'd' ? "on the desktop" : "in the Start menu");
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // Create the shortcut:
    ಪ್ರತ್ಯಾಗಮ create_shortcut(link_name, executable_path, 0,
					   executable_path, args, workingdir);
}

/*
 * We used to use "homedir" as the working directory, but that is a bad choice
 * on multi-user systems.  However, not specifying a directory results in the
 * current directory to be c:\Windows\system32 on Windows 7. Use environment
 * variables instead.
 */
#define WORKDIR "%HOMEDRIVE%%HOMEPATH%"

/*
 * Create shortcut(s) in the Start Menu\Programs\Vim folder.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
install_start_menu(ಪೂರ್ಣಾಙ್ಕ idx UNUSED)
{
    need_uninstall_entry = 1;
    printf("Creating start menu\n");
    ಯದಿ (has_vim)
    {
	ಯದಿ (build_shortcut("Vim", "vim.exe", "",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ (build_shortcut("Vim Read-only", "vim.exe", "-R",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ (build_shortcut("Vim Diff", "vim.exe", "-d",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (has_gvim)
    {
	ಯದಿ (build_shortcut("gVim", "gvim.exe", "",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ (build_shortcut("gVim Easy", "gvim.exe", "-y",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ (build_shortcut("gVim Read-only", "gvim.exe", "-R",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ (build_shortcut("gVim Diff", "gvim.exe", "-d",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (build_shortcut("Uninstall",
		interactive ? "uninstall.exe" : "uninstall-gui.exe", "",
					   VIM_STARTMENU, installdir) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    // For Windows NT the working dir of the vimtutor.bat must be right,
    // otherwise gvim.exe won't be found and using gvimbat doesn't work.
    ಯದಿ (build_shortcut("Vim tutor", "vimtutor.bat", "",
					   VIM_STARTMENU, installdir) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (build_shortcut("Help", has_gvim ? "gvim.exe" : "vim.exe", "-c h",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    {
	ವರ್ಣ	shell_folder_path[BUFSIZE];

	// Creating the URL shortcut works a bit differently...
	ಯದಿ (get_shell_folder_path(shell_folder_path, VIM_STARTMENU) == FAIL)
	{
	    printf("Finding the path of the Start menu failed\n");
	    ಪ್ರತ್ಯಾಗಮ ;
	}
	add_pathsep(shell_folder_path);
	strcat(shell_folder_path, "Vim Online.url");
	ಯದಿ (!WritePrivateProfileString("InternetShortcut", "URL",
				    "https://www.vim.org/", shell_folder_path))
	{
	    printf("Creating the Vim online URL failed\n");
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
toggle_startmenu_choice(ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (choices[idx].installfunc == ರಿಕ್ತ)
    {
	choices[idx].installfunc = install_start_menu;
	choices[idx].text = "Add Vim to the Start menu";
    }
    ಅನ್ಯಥಾ
    {
	choices[idx].installfunc = ರಿಕ್ತ;
	choices[idx].text = "Do NOT add Vim to the Start menu";
    }
}

/*
 * Function to actually create the shortcuts
 *
 * Currently I am supplying no working directory to the shortcut.  This
 *    means that the initial working dir will be:
 *    - the location of the shortcut if no file is supplied
 *    - the location of the file being edited if a file is supplied (ie via
 *      drag and drop onto the shortcut).
 */
    ಶೂನ್ಯ
install_shortcut_gvim(ಪೂರ್ಣಾಙ್ಕ idx)
{
    // Create shortcut(s) on the desktop
    ಯದಿ (choices[idx].arg)
    {
	(ಶೂನ್ಯ)build_shortcut(icon_names[0], "gvim.exe",
						      "", "desktop", WORKDIR);
	need_uninstall_entry = 1;
    }
}

    ಶೂನ್ಯ
install_shortcut_evim(ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (choices[idx].arg)
    {
	(ಶೂನ್ಯ)build_shortcut(icon_names[1], "gvim.exe",
						    "-y", "desktop", WORKDIR);
	need_uninstall_entry = 1;
    }
}

    ಶೂನ್ಯ
install_shortcut_gview(ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (choices[idx].arg)
    {
	(ಶೂನ್ಯ)build_shortcut(icon_names[2], "gvim.exe",
						    "-R", "desktop", WORKDIR);
	need_uninstall_entry = 1;
    }
}

    ಶೂನ್ಯ
toggle_shortcut_choice(ಪೂರ್ಣಾಙ್ಕ idx)
{
    ವರ್ಣ	*arg;

    ಯದಿ (choices[idx].installfunc == install_shortcut_gvim)
	arg = "gVim";
    ಅನ್ಯಥಾ ಯದಿ (choices[idx].installfunc == install_shortcut_evim)
	arg = "gVim Easy";
    ಅನ್ಯಥಾ
	arg = "gVim Read-only";
    ಯದಿ (choices[idx].arg)
    {
	choices[idx].arg = 0;
	alloc_text(idx, "Do NOT create a desktop icon for %s", arg);
    }
    ಅನ್ಯಥಾ
    {
	choices[idx].arg = 1;
	alloc_text(idx, "Create a desktop icon for %s", arg);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
init_startmenu_choice(ಶೂನ್ಯ)
{
    // Start menu
    choices[choice_ಎಣಿಕೆ].changefunc = toggle_startmenu_choice;
    choices[choice_ಎಣಿಕೆ].installfunc = ರಿಕ್ತ;
    choices[choice_ಎಣಿಕೆ].active = 1;
    toggle_startmenu_choice(choice_ಎಣಿಕೆ);	// set the text
    ++choice_ಎಣಿಕೆ;
}

/*
 * Add the choice for the desktop shortcuts.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
init_shortcut_choices(ಶೂನ್ಯ)
{
    // Shortcut to gvim
    choices[choice_ಎಣಿಕೆ].text = ರಿಕ್ತ;
    choices[choice_ಎಣಿಕೆ].arg = 0;
    choices[choice_ಎಣಿಕೆ].active = has_gvim;
    choices[choice_ಎಣಿಕೆ].changefunc = toggle_shortcut_choice;
    choices[choice_ಎಣಿಕೆ].installfunc = install_shortcut_gvim;
    toggle_shortcut_choice(choice_ಎಣಿಕೆ);
    ++choice_ಎಣಿಕೆ;

    // Shortcut to evim
    choices[choice_ಎಣಿಕೆ].text = ರಿಕ್ತ;
    choices[choice_ಎಣಿಕೆ].arg = 0;
    choices[choice_ಎಣಿಕೆ].active = has_gvim;
    choices[choice_ಎಣಿಕೆ].changefunc = toggle_shortcut_choice;
    choices[choice_ಎಣಿಕೆ].installfunc = install_shortcut_evim;
    toggle_shortcut_choice(choice_ಎಣಿಕೆ);
    ++choice_ಎಣಿಕೆ;

    // Shortcut to gview
    choices[choice_ಎಣಿಕೆ].text = ರಿಕ್ತ;
    choices[choice_ಎಣಿಕೆ].arg = 0;
    choices[choice_ಎಣಿಕೆ].active = has_gvim;
    choices[choice_ಎಣಿಕೆ].changefunc = toggle_shortcut_choice;
    choices[choice_ಎಣಿಕೆ].installfunc = install_shortcut_gview;
    toggle_shortcut_choice(choice_ಎಣಿಕೆ);
    ++choice_ಎಣಿಕೆ;
}

/*
 * Attempt to register OLE for Vim.
 */
   ಸ್ಥಿರ ಶೂನ್ಯ
install_OLE_register(ಶೂನ್ಯ)
{
    ವರ್ಣ register_command_string[BUFSIZE + 30];

    printf("\n--- Attempting to register Vim with OLE ---\n");
    printf("(There is no message whether this works or not.)\n");

    sprintf(register_command_string, "\"%s\\gvim.exe\" -silent -register", installdir);
    system(register_command_string);
}

/*
 * Remove the last part of directory "path[]" to get its parent, and put the
 * result in "to[]".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
dir_remove_last(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ to[MAX_PATH])
{
    ವರ್ಣ c;
    long last_char_to_copy;
    long path_length = strlen(path);

    // skip the last character just in case it is a '\\'
    last_char_to_copy = path_length - 2;
    c = path[last_char_to_copy];

    ಯಾವತ್ (c != '\\')
    {
	last_char_to_copy--;
	c = path[last_char_to_copy];
    }

    strncpy(to, path, (size_t)last_char_to_copy);
    to[last_char_to_copy] = NUL;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
set_directories_text(ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪೂರ್ಣಾಙ್ಕ vimfiles_dir_choice = choices[idx].arg;

    ಯದಿ (vimfiles_dir_choice == (ಪೂರ್ಣಾಙ್ಕ)vimfiles_dir_none)
	alloc_text(idx, "Do NOT create plugin directories%s", "");
    ಅನ್ಯಥಾ
	alloc_text(idx, "Create plugin directories: %s",
				   vimfiles_dir_choices[vimfiles_dir_choice]);
}

/*
 * To get the "real" home directory:
 * - get value of $HOME
 * - if not found, get value of $HOMEDRIVE$HOMEPATH
 * - if not found, get value of $USERPROFILE
 *
 * This code is based on init_homedir() in misc1.c, keep in sync!
 */
ಸ್ಥಿರ ವರ್ಣ *homedir = ರಿಕ್ತ;

    ಶೂನ್ಯ
init_homedir(ಶೂನ್ಯ)
{
    ವರ್ಣ    *var;
    ವರ್ಣ    buf[MAX_PATH];

    ಯದಿ (homedir != ರಿಕ್ತ)
    {
	ಮುಕ್ತ(homedir);
	homedir = ರಿಕ್ತ;
    }

    var = getenv("HOME");

    /*
     * Typically, $HOME is not defined on Windows, unless the user has
     * specifically defined it for Vim's sake.  However, on Windows NT
     * platforms, $HOMEDRIVE and $HOMEPATH are automatically defined for
     * each user.  Try constructing $HOME from these.
     */
    ಯದಿ (var == ರಿಕ್ತ || *var == NUL)
    {
	ವರ್ಣ	*homedrive, *homepath;

	homedrive = getenv("HOMEDRIVE");
	homepath = getenv("HOMEPATH");
	ಯದಿ (homepath == ರಿಕ್ತ || *homepath == NUL)
	    homepath = "\\";
	ಯದಿ (homedrive != ರಿಕ್ತ
		   && strlen(homedrive) + strlen(homepath) < ಪರಿಮಾಣ(buf))
	{
	    ಯದಿ (snprintf(buf, ಪರಿಮಾಣ(buf), "%s%s", homedrive, homepath) > 0
		&& buf[0] != NUL)
		var = buf;
	}
    }

    ಯದಿ (var == ರಿಕ್ತ)
	var = getenv("USERPROFILE");

    /*
     * Weird but true: $HOME may contain an indirect reference to another
     * variable, esp. "%USERPROFILE%".  Happens when $USERPROFILE isn't set
     * when $HOME is being set.
     */
    ಯದಿ (var != ರಿಕ್ತ && *var == '%')
    {
	ವರ್ಣ	*p;
	ವರ್ಣ	*exp;

	p = strchr(var + 1, '%');
	ಯದಿ (p != ರಿಕ್ತ)
	{
	    strncpy(buf, var + 1, p - (var + 1));
	    buf[p - (var + 1)] = NUL;
	    exp = getenv(buf);
	    ಯದಿ (exp != ರಿಕ್ತ && *exp != NUL
				&& strlen(exp) + strlen(p) < ಪರಿಮಾಣ(buf))
	    {
		snprintf(buf, ಪರಿಮಾಣ(buf), "%s%s", exp, p + 1);
		var = buf;
	    }
	}
    }

    ಯದಿ (var != ರಿಕ್ತ && *var == NUL)	// empty is same as not set
	var = ರಿಕ್ತ;

    ಯದಿ (var == ರಿಕ್ತ)
	homedir = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
	homedir = _strdup(var);
}

/*
 * Change the directory that the vim plugin directories will be created in:
 * $HOME, $VIM or nowhere.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
change_directories_choice(ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪೂರ್ಣಾಙ್ಕ	    choice_ಎಣಿಕೆ = TABLE_SIZE(vimfiles_dir_choices);

    // Don't offer the $HOME choice if $HOME isn't set.
    ಯದಿ (homedir == ರಿಕ್ತ)
	--choice_ಎಣಿಕೆ;
    choices[idx].arg = get_choice(vimfiles_dir_choices, choice_ಎಣಿಕೆ);
    set_directories_text(idx);
}

/*
 * Create the plugin directories...
 */
//ARGSUSED
    ಸ್ಥಿರ ಶೂನ್ಯ
install_vimfilesdir(ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪೂರ್ಣಾಙ್ಕ i;
    ಪೂರ್ಣಾಙ್ಕ vimfiles_dir_choice = choices[idx].arg;
    ವರ್ಣ *p;
    ವರ್ಣ vimdir_path[MAX_PATH];
    ವರ್ಣ vimfiles_path[MAX_PATH + 9];
    ವರ್ಣ tmp_dirname[BUFSIZE];

    // switch on the location that the user wants the plugin directories
    // built in
    ವಿಕಲ್ಪ (vimfiles_dir_choice)
    {
	ಪಕ್ಷ vimfiles_dir_vim:
	{
	    // Go to the %VIM% directory - check env first, then go one dir
	    //	   below installdir if there is no %VIM% environment variable.
	    //	   The accuracy of $VIM is checked in inspect_system(), so we
	    //	   can be sure it is ok to use here.
	    p = getenv("VIM");
	    ಯದಿ (p == ರಿಕ್ತ) // No $VIM in path
		dir_remove_last(installdir, vimdir_path);
	    ಅನ್ಯಥಾ
		strcpy(vimdir_path, p);
	    ಭಙ್ಗ;
	}
	ಪಕ್ಷ vimfiles_dir_home:
	{
	    // Find the $HOME directory.  Its existence was already checked.
	    p = homedir;
	    ಯದಿ (p == ರಿಕ್ತ)
	    {
		printf("Internal error: $HOME is NULL\n");
		p = "c:\\";
	    }
	    strcpy(vimdir_path, p);
	    ಭಙ್ಗ;
	}
	ಪಕ್ಷ vimfiles_dir_none:
	{
	    // Do not create vim plugin directory.
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }

    // Now, just create the directory.	If it already exists, it will fail
    // silently.
    sprintf(vimfiles_path, "%s\\vimfiles", vimdir_path);
    vim_mkdir(vimfiles_path, 0755);

    printf("Creating the following directories in \"%s\":\n", vimfiles_path);
    ಪ್ರತಿ (i = 0; i < TABLE_SIZE(vimfiles_subdirs); i++)
    {
	sprintf(tmp_dirname, "%s\\%s", vimfiles_path, vimfiles_subdirs[i]);
	printf("  %s", vimfiles_subdirs[i]);
	vim_mkdir(tmp_dirname, 0755);
    }
    printf("\n");
}

/*
 * Add the creation of runtime files to the setup sequence.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
init_directories_choice(ಶೂನ್ಯ)
{
    ಸಂರಚನಾ stat	st;
    ವರ್ಣ	tmp_dirname[BUFSIZE];
    ವರ್ಣ	*p;
    ಪೂರ್ಣಾಙ್ಕ		vimfiles_dir_choice;

    choices[choice_ಎಣಿಕೆ].text = alloc(150);
    choices[choice_ಎಣಿಕೆ].changefunc = change_directories_choice;
    choices[choice_ಎಣಿಕೆ].installfunc = install_vimfilesdir;
    choices[choice_ಎಣಿಕೆ].active = 1;

    // Check if the "compiler" directory already exists.  That's a good
    // indication that the plugin directories were already created.
    p = getenv("HOME");
    ಯದಿ (p != ರಿಕ್ತ)
    {
	vimfiles_dir_choice = (ಪೂರ್ಣಾಙ್ಕ)vimfiles_dir_home;
	sprintf(tmp_dirname, "%s\\vimfiles\\compiler", p);
	ಯದಿ (stat(tmp_dirname, &st) == 0)
	    vimfiles_dir_choice = (ಪೂರ್ಣಾಙ್ಕ)vimfiles_dir_none;
    }
    ಅನ್ಯಥಾ
    {
	vimfiles_dir_choice = (ಪೂರ್ಣಾಙ್ಕ)vimfiles_dir_vim;
	p = getenv("VIM");
	ಯದಿ (p == ರಿಕ್ತ)  // No $VIM in path, use the install dir.
	    dir_remove_last(installdir, tmp_dirname);
	ಅನ್ಯಥಾ
	    strcpy(tmp_dirname, p);
	strcat(tmp_dirname, "\\vimfiles\\compiler");
	ಯದಿ (stat(tmp_dirname, &st) == 0)
	    vimfiles_dir_choice = (ಪೂರ್ಣಾಙ್ಕ)vimfiles_dir_none;
    }

    choices[choice_ಎಣಿಕೆ].arg = vimfiles_dir_choice;
    set_directories_text(choice_ಎಣಿಕೆ);
    ++choice_ಎಣಿಕೆ;
}

/*
 * Setup the choices and the default values.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
setup_choices(ಶೂನ್ಯ)
{
    // install the batch files
    init_bat_choices();

    // (over) write _vimrc file
    init_vimrc_choices();

    // Whether to add Vim to the popup menu
    init_popup_choice();

    // Whether to add Vim to the "Open With..." menu
    init_openwith_choice();

    // Whether to add Vim to the Start Menu.
    init_startmenu_choice();

    // Whether to add shortcuts to the Desktop.
    init_shortcut_choices();

    // Whether to create the runtime directories.
    init_directories_choice();
}

    ಸ್ಥಿರ ಶೂನ್ಯ
print_cmd_ಸಾಲು_help(ಶೂನ್ಯ)
{
    printf("Vim installer non-interactive command line arguments:\n");
    printf("\n");
    printf("-create-batfiles  [vim gvim evim view gview vimdiff gvimdiff]\n");
    printf("    Create .bat files for Vim variants in the Windows directory.\n");
    printf("-create-vimrc\n");
    printf("    Create a default _vimrc file if one does not already exist.\n");
    printf("-vimrc-remap [no|win]\n");
    printf("    Remap keys when creating a default _vimrc file.\n");
    printf("-vimrc-behave [unix|mswin|default]\n");
    printf("    Set mouse behavior when creating a default _vimrc file.\n");
    printf("-vimrc-compat [vi|vim|defaults|all]\n");
    printf("    Set Vi compatibility when creating a default _vimrc file.\n");
    printf("-install-popup\n");
    printf("    Install the Edit-with-Vim context menu entry\n");
    printf("-install-openwith\n");
    printf("    Add Vim to the \"Open With...\" context menu list\n");
    printf("-add-start-menu");
    printf("    Add Vim to the start menu\n");
    printf("-install-icons");
    printf("    Create icons for gVim executables on the desktop\n");
    printf("-create-directories [vim|home]\n");
    printf("    Create runtime directories to drop plugins into; in the $VIM\n");
    printf("    or $HOME directory\n");
    printf("-register-OLE");
    printf("    Ignored\n");
    printf("\n");
}

/*
 * Setup installation choices based on command line switches
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
command_ಸಾಲು_setup_choices(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ **argv)
{
    ಪೂರ್ಣಾಙ್ಕ i, j;

    ಪ್ರತಿ (i = 1; i < argc; i++)
    {
	ಯದಿ (strcmp(argv[i], "-create-batfiles") == 0)
	{
	    ಯದಿ (i + 1 == argc)
		ಅನುವರ್ತ;
	    ಯಾವತ್ (argv[i + 1][0] != '-' && i < argc)
	    {
		i++;
		ಪ್ರತಿ (j = 1; j < TARGET_ಎಣಿಕೆ; ++j)
		    ಯದಿ ((targets[j].exenamearg[0] == 'g' ? has_gvim : has_vim)
			    && strcmp(argv[i], targets[j].name) == 0)
		    {
			init_bat_choice(j);
			ಭಙ್ಗ;
		    }
		ಯದಿ (j == TARGET_ಎಣಿಕೆ)
		    printf("%s is not a valid choice for -create-batfiles\n",
								     argv[i]);

		ಯದಿ (i + 1 == argc)
		    ಭಙ್ಗ;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (strcmp(argv[i], "-create-vimrc") == 0)
	{
	    // Setup default vimrc choices.  If there is already a _vimrc file,
	    // it will NOT be overwritten.
	    init_vimrc_choices();
	}
	ಅನ್ಯಥಾ ಯದಿ (strcmp(argv[i], "-vimrc-remap") == 0)
	{
	    ಯದಿ (i + 1 == argc)
		ಭಙ್ಗ;
	    i++;
	    ಯದಿ (strcmp(argv[i], "no") == 0)
		remap_choice = remap_no;
	    ಅನ್ಯಥಾ ಯದಿ (strcmp(argv[i], "win") == 0)
		remap_choice = remap_win;
	}
	ಅನ್ಯಥಾ ಯದಿ (strcmp(argv[i], "-vimrc-behave") == 0)
	{
	    ಯದಿ (i + 1 == argc)
		ಭಙ್ಗ;
	    i++;
	    ಯದಿ (strcmp(argv[i], "unix") == 0)
		mouse_choice = mouse_xterm;
	    ಅನ್ಯಥಾ ಯದಿ (strcmp(argv[i], "mswin") == 0)
		mouse_choice = mouse_mswin;
	    ಅನ್ಯಥಾ ಯದಿ (strcmp(argv[i], "default") == 0)
		mouse_choice = mouse_default;
	}
	ಅನ್ಯಥಾ ಯದಿ (strcmp(argv[i], "-vimrc-compat") == 0)
	{
	    ಯದಿ (i + 1 == argc)
		ಭಙ್ಗ;
	    i++;
	    ಯದಿ (strcmp(argv[i], "vi") == 0)
		compat_choice = compat_vi;
	    ಅನ್ಯಥಾ ಯದಿ (strcmp(argv[i], "vim") == 0)
		compat_choice = compat_vim;
	    ಅನ್ಯಥಾ ಯದಿ (strcmp(argv[i], "defaults") == 0)
		compat_choice = compat_some_enhancements;
	    ಅನ್ಯಥಾ ಯದಿ (strcmp(argv[i], "all") == 0)
		compat_choice = compat_all_enhancements;
	}
	ಅನ್ಯಥಾ ಯದಿ (strcmp(argv[i], "-install-popup") == 0)
	{
	    init_popup_choice();
	}
	ಅನ್ಯಥಾ ಯದಿ (strcmp(argv[i], "-install-openwith") == 0)
	{
	    init_openwith_choice();
	}
	ಅನ್ಯಥಾ ಯದಿ (strcmp(argv[i], "-add-start-menu") == 0)
	{
	    init_startmenu_choice();
	}
	ಅನ್ಯಥಾ ಯದಿ (strcmp(argv[i], "-install-icons") == 0)
	{
	    init_shortcut_choices();
	}
	ಅನ್ಯಥಾ ಯದಿ (strcmp(argv[i], "-create-directories") == 0)
	{
	    ಪೂರ್ಣಾಙ್ಕ vimfiles_dir_choice = (ಪೂರ್ಣಾಙ್ಕ)vimfiles_dir_none;

	    init_directories_choice();
	    ಯದಿ (i + 1 < argc && argv[i + 1][0] != '-')
	    {
		i++;
		ಯದಿ (strcmp(argv[i], "vim") == 0)
		    vimfiles_dir_choice = (ಪೂರ್ಣಾಙ್ಕ)vimfiles_dir_vim;
		ಅನ್ಯಥಾ ಯದಿ (strcmp(argv[i], "home") == 0)
		{
		    ಯದಿ (homedir == ರಿಕ್ತ)  // No $HOME in environment
			vimfiles_dir_choice = (ಪೂರ್ಣಾಙ್ಕ)vimfiles_dir_none;
		    ಅನ್ಯಥಾ
			vimfiles_dir_choice = (ಪೂರ್ಣಾಙ್ಕ)vimfiles_dir_home;
		}
		ಅನ್ಯಥಾ
		{
		    printf("Unknown argument for -create-directories: %s\n",
								     argv[i]);
		    print_cmd_ಸಾಲು_help();
		}
	    }
	    ಅನ್ಯಥಾ // No choice specified, default to vim directory
		vimfiles_dir_choice = (ಪೂರ್ಣಾಙ್ಕ)vimfiles_dir_vim;
	    choices[choice_ಎಣಿಕೆ - 1].arg = vimfiles_dir_choice;
	}
	ಅನ್ಯಥಾ ಯದಿ (strcmp(argv[i], "-register-OLE") == 0)
	{
	    // This is always done when gvim is found
	}
	ಅನ್ಯಥಾ // Unknown switch
	{
	    printf("Got unknown argument argv[%d] = %s\n", i, argv[i]);
	    print_cmd_ಸಾಲು_help();
	}
    }
}


/*
 * Show a few screens full of helpful information.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
show_help(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ ವರ್ಣ *(ವಸ್ತುಗಳು[]) =
    {
"Installing .bat files\n"
"---------------------\n"
"The vim.bat file is written in one of the directories in $PATH.\n"
"This makes it possible to start Vim from the command line.\n"
"If vim.exe can be found in $PATH, the choice for vim.bat will not be\n"
"present.  It is assumed you will use the existing vim.exe.\n"
"If vim.bat can already be found in $PATH this is probably for an old\n"
"version of Vim (but this is not checked!).  You can overwrite it.\n"
"If no vim.bat already exists, you can select one of the directories in\n"
"$PATH for creating the batch file, or disable creating a vim.bat file.\n"
"\n"
"If you choose not to create the vim.bat file, Vim can still be executed\n"
"in other ways, but not from the command line.\n"
"\n"
"The same applies to choices for gvim, evim, (g)view, and (g)vimdiff.\n"
"The first item can be used to change the path for all of them.\n"
,
"Creating a _vimrc file\n"
"----------------------\n"
"The _vimrc file is used to set options for how Vim behaves.\n"
"The install program can create a _vimrc file with a few basic choices.\n"
"You can edit this file later to tune your preferences.\n"
"If you already have a _vimrc or .vimrc file it can be overwritten.\n"
"Don't do that if you have made changes to it.\n"
,
"Vim features\n"
"------------\n"
"(this choice is only available when creating a _vimrc file)\n"
"1. Vim can run in Vi-compatible mode.  Many nice Vim features are then\n"
"   disabled.  Only choose Vi-compatible if you really need full Vi\n"
"   compatibility.\n"
"2. Vim runs in not-Vi-compatible mode.  Vim is still mostly Vi compatible,\n"
"   but adds nice features like multi-level undo.\n"
"3. Running Vim with some enhancements is useful when you want some of\n"
"   the nice Vim features, but have a slow computer and want to keep it\n"
"   really fast.\n"
"4. Syntax highlighting shows many files in color.  Not only does this look\n"
"   nice, it also makes it easier to spot errors and you can work faster.\n"
"   The other features include editing compressed files.\n"
,
"Windows key mapping\n"
"-------------------\n"
"(this choice is only available when creating a _vimrc file)\n"
"Under MS-Windows the CTRL-C key copies text to the clipboard and CTRL-V\n"
"pastes text from the clipboard.  There are a few more keys like these.\n"
"Unfortunately, in Vim these keys normally have another meaning.\n"
"1. Choose to have the keys like they normally are in Vim (useful if you\n"
"   also use Vim on other systems).\n"
"2. Choose to have the keys work like they are used on MS-Windows (useful\n"
"   if you mostly work on MS-Windows).\n"
,
"Mouse use\n"
"---------\n"
"(this choice is only available when creating a _vimrc file)\n"
"The right mouse button can be used in two ways:\n"
"1. The Unix way is to extend an existing selection.  The popup menu is\n"
"   not available.\n"
"2. The MS-Windows way is to show a popup menu, which allows you to\n"
"   copy/paste text, undo/redo, etc.  Extending the selection can still be\n"
"   done by keeping SHIFT pressed while using the left mouse button\n"
,
"Edit-with-Vim context menu entry\n"
"--------------------------------\n"
"(this choice is only available when gvim.exe and gvimext.dll are present)\n"
"You can associate different file types with Vim, so that you can (double)\n"
"click on a file to edit it with Vim.  This means you have to individually\n"
"select each file type.\n"
"An alternative is the option offered here: Install an \"Edit with Vim\"\n"
"entry in the popup menu for the right mouse button.  This means you can\n"
"edit any file with Vim.\n"
,
"\"Open With...\" context menu entry\n"
"--------------------------------\n"
"(this choice is only available when gvim.exe is present)\n"
"This option adds Vim to the \"Open With...\" entry in the popup menu for\n"
"the right mouse button.  This also makes it possible to edit HTML files\n"
"directly from Internet Explorer.\n"
,
"Add Vim to the Start menu\n"
"-------------------------\n"
"In Windows 95 and later, Vim can be added to the Start menu.  This will\n"
"create a submenu with an entry for vim, gvim, evim, vimdiff, etc..\n"
,
"Icons on the desktop\n"
"--------------------\n"
"(these choices are only available when installing gvim)\n"
"In Windows 95 and later, shortcuts (icons) can be created on the Desktop.\n"
,
"Create plugin directories\n"
"-------------------------\n"
"Plugin directories allow extending Vim by dropping a file into a directory.\n"
"This choice allows creating them in $HOME (if you have a home directory) or\n"
"$VIM (used for everybody on the system).\n"
,
ರಿಕ್ತ
    };
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		c;

    rewind(stdin);
    printf("\n");
    ಪ್ರತಿ (i = 0; ವಸ್ತುಗಳು[i] != ರಿಕ್ತ; ++i)
    {
	puts(ವಸ್ತುಗಳು[i]);
	printf("Hit Enter to continue, b (back) or q (quit help): ");
	c = getchar();
	rewind(stdin);
	ಯದಿ (c == 'b' || c == 'B')
	{
	    ಯದಿ (i == 0)
		--i;
	    ಅನ್ಯಥಾ
		i -= 2;
	}
	ಯದಿ (c == 'q' || c == 'Q')
	    ಭಙ್ಗ;
	printf("\n");
    }
}

/*
 * Install the choices.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
install(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    // Install the selected choices.
    ಪ್ರತಿ (i = 0; i < choice_ಎಣಿಕೆ; ++i)
	ಯದಿ (choices[i].installfunc != ರಿಕ್ತ && choices[i].active)
	    (choices[i].installfunc)(i);

    // Add some entries to the registry, if needed.
    ಯದಿ (install_popup
	    || install_openwith
	    || (need_uninstall_entry && interactive)
	    || !interactive)
	install_registry();

    // Register gvim with OLE.
    ಯದಿ (has_gvim)
	install_OLE_register();
}

/*
 * request_choice
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
request_choice(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		      i;

    printf("\n\nInstall will do for you:\n");
    ಪ್ರತಿ (i = 0; i < choice_ಎಣಿಕೆ; ++i)
	ಯದಿ (choices[i].active)
	    printf("%2d  %s\n", i + 1, choices[i].text);
    printf("To change an item, enter its number\n\n");
    printf("Enter item number, h (help), d (do it) or q (quit): ");
}

    ಪೂರ್ಣಾಙ್ಕ
main(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ **argv)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ವರ್ಣ	buf[BUFSIZE];

    /*
     * Run interactively if there are no command line arguments.
     */
    ಯದಿ (argc > 1)
	interactive = 0;
    ಅನ್ಯಥಾ
	interactive = 1;

    // Initialize this program.
    do_inits(argv);
    init_homedir();

    ಯದಿ (argc > 1 && strcmp(argv[1], "-uninstall-check") == 0)
    {
	// Only check for already installed Vims.  Used by NSIS installer.
	i = uninstall_check(1);

	// Find the value of $VIM, because NSIS isn't able to do this by
	// itself.
	get_vim_env();

	// When nothing found exit quietly.  If something found wait for
	// a little while, so that the user can read the messages.
	ಯದಿ (i && _isatty(1))
	    sleep(3);
	exit(0);
    }

    printf("This program sets up the installation of Vim "
						   VIM_VERSION_MEDIUM "\n\n");

    // Check if the user unpacked the archives properly.
    check_unpack();

    // Check for already installed Vims.
    ಯದಿ (interactive)
	uninstall_check(0);

    // Find out information about the system.
    inspect_system();

    ಯದಿ (interactive)
    {
	// Setup all the choices.
	setup_choices();

	// Let the user change choices and finally install (or quit).
	ಪ್ರತಿ (;;)
	{
	    request_choice();
	    rewind(stdin);
	    ಯದಿ (scanf("%99s", buf) == 1)
	    {
		ಯದಿ (isdigit((unsigned ವರ್ಣ)buf[0]))
		{
		    // Change a choice.
		    i = atoi(buf);
		    ಯದಿ (i > 0 && i <= choice_ಎಣಿಕೆ && choices[i - 1].active)
			(choices[i - 1].changefunc)(i - 1);
		    ಅನ್ಯಥಾ
			printf("\nIllegal choice\n");
		}
		ಅನ್ಯಥಾ ಯದಿ (buf[0] == 'h' || buf[0] == 'H')
		{
		    // Help
		    show_help();
		}
		ಅನ್ಯಥಾ ಯದಿ (buf[0] == 'd' || buf[0] == 'D')
		{
		    // Install!
		    install();
		    printf("\nThat finishes the installation.  Happy Vimming!\n");
		    ಭಙ್ಗ;
		}
		ಅನ್ಯಥಾ ಯದಿ (buf[0] == 'q' || buf[0] == 'Q')
		{
		    // Quit
		    printf("\nExiting without anything done\n");
		    ಭಙ್ಗ;
		}
		ಅನ್ಯಥಾ
		    printf("\nIllegal choice\n");
	    }
	}
	printf("\n");
	myexit(0);
    }
    ಅನ್ಯಥಾ
    {
	/*
	 * Run non-interactive - setup according to the command line switches
	 */
	command_ಸಾಲು_setup_choices(argc, argv);
	install();

	// Avoid that the user has to hit Enter, just wait a little bit to
	// allow reading the messages.
	sleep(2);
    }

    ಪ್ರತ್ಯಾಗಮ 0;
}
