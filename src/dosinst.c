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
#অন্তর্ভাব "dosinst.h"
#অন্তর্ভাব <io.h>

#define GVIMEXT64_PATH	    "GvimExt64\\gvimext.dll"
#define GVIMEXT32_PATH	    "GvimExt32\\gvimext.dll"

// Macro to do an error check I was typing over and over
#define CHECK_REG_ERROR(code) \
    করো { \
	যদি (code != ERROR_SUCCESS) \
	{ \
	    printf("%ld error number:  %ld\n", (long)__LINE__, (long)code); \
	    ফেরত 1; \
	} \
    } যখন (0)

পূর্ণসংখ্যা	has_vim = 0;		// installable vim.exe exists
পূর্ণসংখ্যা	has_gvim = 0;		// installable gvim.exe exists

অক্ষর	oldvimrc[BUFSIZE];	// name of existing vimrc file
অক্ষর	vimrc[BUFSIZE];		// name of vimrc file to create

অক্ষর	*default_bat_dir = ফাঁকা;  // when not NULL, use this as the default
				  // directory to write .bat files in
অক্ষর	*default_vim_dir = ফাঁকা;  // when not NULL, use this as the default
				  // install dir for NSIS

/*
 * Structure used for each choice the user can make.
 */
কাঠামো choice
{
    পূর্ণসংখ্যা	    active;			// non-zero when choice is active
    অক্ষর    *text;			// text displayed for this choice
    শূন্য    (*changefunc)(পূর্ণসংখ্যা idx);	// function to change this choice
    পূর্ণসংখ্যা	    arg;			// argument for function
    শূন্য    (*installfunc)(পূর্ণসংখ্যা idx);	// function to install this choice
};

কাঠামো choice	choices[30];		// choices the user can make
পূর্ণসংখ্যা		choice_গুনতি = 0;	// number of choices available

#define TABLE_আকার(s)	(পূর্ণসংখ্যা)ARRAYSIZE(s)

গণনা
{
    compat_vi = 1,
    compat_vim,
    compat_some_enhancements,
    compat_all_enhancements
};
অক্ষর	*(compat_choices[]) =
{
    "\nChoose the default way to run Vim:",
    "Vi compatible",
    "Vim default",
    "with some Vim enhancements",
    "with syntax highlighting and other features switched on",
};
পূর্ণসংখ্যা	compat_choice = (পূর্ণসংখ্যা)compat_all_enhancements;
অক্ষর	*compat_text = "- run Vim %s";

গণনা
{
    remap_no = 1,
    remap_win
};
অক্ষর	*(remap_choices[]) =
{
    "\nChoose:",
    "Do not remap keys for Windows behavior",
    "Remap a few keys for Windows behavior (CTRL-V, CTRL-C, CTRL-F, etc)",
};
পূর্ণসংখ্যা	remap_choice = (পূর্ণসংখ্যা)remap_no;
অক্ষর	*remap_text = "- %s";

গণনা
{
    mouse_xterm = 1,
    mouse_mswin,
    mouse_default
};
অক্ষর	*(mouse_choices[]) =
{
    "\nChoose the way how Vim uses the mouse:",
    "right button extends selection (the Unix way)",
    "right button has a popup menu, left button starts select mode (the Windows way)",
    "right button has a popup menu, left button starts visual mode",
};
পূর্ণসংখ্যা	mouse_choice = (পূর্ণসংখ্যা)mouse_default;
অক্ষর	*mouse_text = "- The mouse %s";

গণনা
{
    vimfiles_dir_none = 1,
    vimfiles_dir_vim,
    vimfiles_dir_home
};
স্থির অক্ষর *(vimfiles_dir_choices[]) =
{
    "\nCreate plugin directories:",
    "No",
    "In the VIM directory",
    "In your HOME directory",
};

// non-zero when selected to install the popup menu entry.
স্থির পূর্ণসংখ্যা	install_popup = 0;

// non-zero when selected to install the "Open with" entry.
স্থির পূর্ণসংখ্যা	install_openwith = 0;

// non-zero when need to add an uninstall entry in the registry
স্থির পূর্ণসংখ্যা	need_uninstall_entry = 0;

/*
 * Definitions of the directory name (under $VIM) of the vimfiles directory
 * and its subdirectories:
 */
স্থির অক্ষর	*(vimfiles_subdirs[]) =
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
    স্থির পূর্ণসংখ্যা
get_choice(অক্ষর **table, পূর্ণসংখ্যা entries)
{
    পূর্ণসংখ্যা		answer;
    পূর্ণসংখ্যা		idx;
    অক্ষর	dummy[100];

    করো
    {
	জন্য (idx = 0; idx < entries; ++idx)
	{
	    যদি (idx)
		printf("%2d  ", idx);
	    puts(table[idx]);
	}
	printf("Choice: ");
	যদি (scanf("%d", &answer) != 1)
	{
	    scanf("%99s", dummy);
	    answer = 0;
	}
    }
    যখন (answer < 1 || answer >= entries);

    ফেরত answer;
}

/*
 * Check if the user unpacked the archives properly.
 * Sets "runtimeidx".
 */
    স্থির শূন্য
check_unpack(শূন্য)
{
    অক্ষর	buf[BUFSIZE];
    FILE	*fd;
    কাঠামো stat	st;

    // check for presence of the correct version number in installdir[]
    runtimeidx = strlen(installdir) - strlen(VIM_VERSION_NODOT);
    যদি (runtimeidx <= 0
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
    যদি (stat(buf, &st) < 0)
    {
	printf("ERROR: Cannot find filetype.vim in \"%s\"\n", installdir);
	printf("It looks like you did not unpack the runtime archive.\n");
	printf("You must unpack the runtime archive \"%srt.zip\" before installing.\n",
		VIM_VERSION_NODOT);
	myexit(1);
    }

    // Check if vim.exe or gvim.exe is in the current directory.
    যদি ((fd = fopen("gvim.exe", "r")) != ফাঁকা)
    {
	fclose(fd);
	has_gvim = 1;
    }
    যদি ((fd = fopen("vim.exe", "r")) != ফাঁকা)
    {
	fclose(fd);
	has_vim = 1;
    }
    যদি (!has_gvim && !has_vim)
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
    স্থির পূর্ণসংখ্যা
pathcmp(অক্ষর *p, পূর্ণসংখ্যা plen, অক্ষর *q, পূর্ণসংখ্যা qlen)
{
    পূর্ণসংখ্যা		i;

    যদি (plen < 0)
	plen = strlen(p);
    যদি (qlen < 0)
	qlen = strlen(q);
    জন্য (i = 0; ; ++i)
    {
	// End of "p": check if "q" also ends or just has a slash.
	যদি (i == plen)
	{
	    যদি (i == qlen)  // match
		ফেরত 0;
	    যদি (i == qlen - 1 && (q[i] == '\\' || q[i] == '/'))
		ফেরত 0;   // match with trailing slash
	    ফেরত 1;	    // no match
	}

	// End of "q": check if "p" also ends or just has a slash.
	যদি (i == qlen)
	{
	    যদি (i == plen)  // match
		ফেরত 0;
	    যদি (i == plen - 1 && (p[i] == '\\' || p[i] == '/'))
		ফেরত 0;   // match with trailing slash
	    ফেরত 1;	    // no match
	}

	যদি (!(mytoupper(p[i]) == mytoupper(q[i])
		|| ((p[i] == '/' || p[i] == '\\')
		    && (q[i] == '/' || q[i] == '\\'))))
	    ফেরত 1;	    // no match
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
    স্থির শূন্য
findoldfile(অক্ষর **destination)
{
    অক্ষর	*bp = *destination;
    size_t	indir_l = strlen(installdir);
    অক্ষর	*cp;
    অক্ষর	*tmpname;
    অক্ষর	*farname;

    /*
     * No action needed if exe not found or not in this directory.
     */
    যদি (bp == ফাঁকা || strnicmp(bp, installdir, indir_l) != 0)
	ফেরত;
    cp = bp + indir_l;
    যদি (strchr("/\\", *cp++) == ফাঁকা
	    || strchr(cp, '\\') != ফাঁকা
	    || strchr(cp, '/') != ফাঁকা)
	ফেরত;

    tmpname = alloc(strlen(cp) + 1);
    strcpy(tmpname, cp);
    tmpname[strlen(tmpname) - 1] = 'x';	// .exe -> .exx

    যদি (access(tmpname, 0) == 0)
    {
	printf("\nERROR: %s and %s clash.  Remove or rename %s.\n",
	    tmpname, cp, tmpname);
	myexit(1);
    }

    যদি (rename(cp, tmpname) != 0)
    {
	printf("\nERROR: failed to rename %s to %s: %s\n",
	    cp, tmpname, strerror(0));
	myexit(1);
    }

    farname = searchpath_save(cp);

    যদি (rename(tmpname, cp) != 0)
    {
	printf("\nERROR: failed to rename %s back to %s: %s\n",
	    tmpname, cp, strerror(0));
	myexit(1);
    }

    মুক্ত(*destination);
    মুক্ত(tmpname);
    *destination = farname;
}

/*
 * Check if there is a vim.[exe|bat|, gvim.[exe|bat|, etc. in the path.
 * When "check_bat_only" is TRUE, only find "default_bat_dir".
 */
    স্থির শূন্য
find_bat_exe(পূর্ণসংখ্যা check_bat_only)
{
    পূর্ণসংখ্যা		i;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    জন্য (i = 1; i < TARGET_গুনতি; ++i)
    {
	targets[i].oldbat = searchpath_save(targets[i].batname);
	যদি (!check_bat_only)
	    targets[i].oldexe = searchpath_save(targets[i].exename);

	যদি (default_bat_dir == ফাঁকা && targets[i].oldbat != ফাঁকা)
	{
	    default_bat_dir = alloc(strlen(targets[i].oldbat) + 1);
	    strcpy(default_bat_dir, targets[i].oldbat);
	    remove_লেজ(default_bat_dir);
	}
	যদি (check_bat_only && targets[i].oldbat != ফাঁকা)
	{
	    মুক্ত(targets[i].oldbat);
	    targets[i].oldbat = ফাঁকা;
	}
    }

    mch_chdir(installdir);
}

/*
 * Get the value of $VIMRUNTIME or $VIM and write it in $TEMP/vimini.ini, so
 * that NSIS can read it.
 * When not set, use the directory of a previously installed Vim.
 */
    স্থির শূন্য
get_vim_env(শূন্য)
{
    অক্ষর	*vim;
    অক্ষর	buf[BUFSIZE];
    FILE	*fd;
    অক্ষর	fname[BUFSIZE];

    // First get $VIMRUNTIME.  If it's set, remove the tail.
    vim = getenv("VIMRUNTIME");
    যদি (vim != ফাঁকা && *vim != 0 && strlen(vim) < আকার(buf))
    {
	strcpy(buf, vim);
	remove_লেজ(buf);
	vim = buf;
    }
    অন্যথা
    {
	vim = getenv("VIM");
	যদি (vim == ফাঁকা || *vim == 0)
	{
	    // Use the directory from an old uninstall entry.
	    যদি (default_vim_dir != ফাঁকা)
		vim = default_vim_dir;
	    অন্যথা
		// Let NSIS know there is no default, it should use
		// $PROGRAMFILES.
		vim = "";
	}
    }

    // NSIS also uses GetTempPath(), thus we should get the same directory
    // name as where NSIS will look for vimini.ini.
    Get_অস্থায়ী_Path(আকার(fname) - 12, fname);
    add_pathsep(fname);
    strcat(fname, "vimini.ini");

    fd = fopen(fname, "w");
    যদি (fd != ফাঁকা)
    {
	// Make it look like an .ini file, so that NSIS can read it with a
	// ReadINIStr command.
	fprintf(fd, "[vimini]\n");
	fprintf(fd, "dir=\"%s\"\n", vim);
	fclose(fd);
    }
    অন্যথা
    {
	printf("Failed to open %s\n", fname);
	sleep(2);
    }
}

স্থির পূর্ণসংখ্যা অঙ্ক_windows;

/*
 * Callback used for EnumWindows():
 * Count the window if the title looks like it is for the uninstaller.
 */
//ARGSUSED
    স্থির BOOL CALLBACK
window_cb(HWND hwnd, LPARAM lparam UNUSED)
{
    অক্ষর title[256];

    title[0] = 0;
    GetWindowText(hwnd, title, 256);
    যদি (strstr(title, "Vim ") != ফাঁকা && strstr(title, " Uninstall") != ফাঁকা)
	++অঙ্ক_windows;
    ফেরত TRUE;
}

/*
 * Run the uninstaller silently.
 */
    স্থির পূর্ণসংখ্যা
run_silent_uninstall(অক্ষর *uninst_exe)
{
    অক্ষর    vimrt_dir[BUFSIZE];
    অক্ষর    অস্থায়ী_uninst[BUFSIZE];
    অক্ষর    অস্থায়ী_dir[MAX_PATH];
    অক্ষর    buf[BUFSIZE * 2 + 10];
    পূর্ণসংখ্যা	    i;
    DWORD   tick;

    strcpy(vimrt_dir, uninst_exe);
    remove_লেজ(vimrt_dir);

    যদি (!Get_অস্থায়ী_Path(আকার(অস্থায়ী_dir), অস্থায়ী_dir))
	ফেরত FAIL;

    // Copy the uninstaller to a temporary exe.
    tick = Get_Tick_গুনতি();
    জন্য (i = 0; ; i++)
    {
	sprintf(অস্থায়ী_uninst, "%s\\vimun%04X.exe", অস্থায়ী_dir,
					  (unsigned পূর্ণসংখ্যা)((i + tick) & 0xFFFF));
	যদি (CopyFile(uninst_exe, অস্থায়ী_uninst, TRUE))
	    বিরতি;
	যদি (GetLastError() != ERROR_FILE_EXISTS)
	    ফেরত FAIL;
	যদি (i == 65535)
	    ফেরত FAIL;
    }

    // Run the copied uninstaller silently.
    যদি (strchr(অস্থায়ী_uninst, ' ') != ফাঁকা)
	sprintf(buf, "\"%s\" /S _?=%s", অস্থায়ী_uninst, vimrt_dir);
    অন্যথা
	sprintf(buf, "%s /S _?=%s", অস্থায়ী_uninst, vimrt_dir);
    run_command(buf);

    DeleteFile(অস্থায়ী_uninst);
    ফেরত OK;
}

/*
 * Check for already installed Vims.
 * Return non-zero when found one.
 */
    স্থির পূর্ণসংখ্যা
uninstall_check(পূর্ণসংখ্যা skip_question)
{
    HKEY	চাবি_handle;
    HKEY	uninstall_চাবি_handle;
    অক্ষর	*uninstall_চাবি = "software\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
    অক্ষর	subkey_name_buff[BUFSIZE];
    অক্ষর	অস্থায়ী_string_বাফার[BUFSIZE-2];
    DWORD	local_bufsize;
    FILETIME	অস্থায়ী_pfiletime;
    DWORD	চাবি_সূচক;
    অক্ষর	প্রবেশ;
    long	code;
    DWORD	মান_type;
    DWORD	orig_অঙ্ক_চাবিগুলি;
    DWORD	new_অঙ্ক_চাবিগুলি;
    DWORD	allow_silent;
    পূর্ণসংখ্যা		foundone = 0;

    code = Reg_Open_চাবি_Ex(HKEY_LOCAL_MACHINE, uninstall_চাবি, 0,
				     চাবি_WOW64_64_চাবি | চাবি_READ, &চাবি_handle);
    CHECK_REG_ERROR(code);

    চাবি_সূচক = 0;
    যখন (TRUE)
    {
	local_bufsize = আকার(subkey_name_buff);
	যদি (Reg_Enum_চাবি_Ex(চাবি_handle, চাবি_সূচক, subkey_name_buff, &local_bufsize,
		ফাঁকা, ফাঁকা, ফাঁকা, &অস্থায়ী_pfiletime) == ERROR_NO_MORE_বস্তুসমূহ)
	    বিরতি;

	যদি (strncmp("Vim", subkey_name_buff, 3) == 0)
	{
	    // Open the key named Vim*
	    code = Reg_Open_চাবি_Ex(চাবি_handle, subkey_name_buff, 0,
			   চাবি_WOW64_64_চাবি | চাবি_READ, &uninstall_চাবি_handle);
	    CHECK_REG_ERROR(code);

	    // get the DisplayName out of it to show the user
	    local_bufsize = আকার(অস্থায়ী_string_বাফার);
	    code = Reg_Query_মান_Ex(uninstall_চাবি_handle, "displayname", 0,
		    &মান_type, (LPBYTE)অস্থায়ী_string_বাফার,
		    &local_bufsize);
	    CHECK_REG_ERROR(code);

	    allow_silent = 0;
	    যদি (skip_question)
	    {
		DWORD varsize = আকার(DWORD);

		Reg_Query_মান_Ex(uninstall_চাবি_handle, "AllowSilent", 0,
			&মান_type, (LPBYTE)&allow_silent,
			&varsize);
	    }

	    foundone = 1;
	    printf("\n*********************************************************\n");
	    printf("Vim Install found what looks like an existing Vim version.\n");
	    printf("The name of the entry is:\n");
	    printf("\n        \"%s\"\n\n", অস্থায়ী_string_বাফার);

	    printf("Installing the new version will disable part of the existing version.\n");
	    printf("(The batch files used in a console and the \"Edit with Vim\" entry in\n");
	    printf("the popup menu will use the new version)\n");

	    যদি (skip_question)
		printf("\nRunning uninstall program for \"%s\"\n", অস্থায়ী_string_বাফার);
	    অন্যথা
		printf("\nDo you want to uninstall \"%s\" now?\n(y)es/(n)o)  ", অস্থায়ী_string_বাফার);
	    fflush(stdout);

	    // get the UninstallString
	    local_bufsize = আকার(অস্থায়ী_string_বাফার);
	    code = Reg_Query_মান_Ex(uninstall_চাবি_handle, "uninstallstring", 0,
		    &মান_type, (LPBYTE)অস্থায়ী_string_বাফার, &local_bufsize);
	    CHECK_REG_ERROR(code);

	    // Remember the directory, it is used as the default for NSIS.
	    default_vim_dir = alloc(strlen(অস্থায়ী_string_বাফার) + 1);
	    strcpy(default_vim_dir, অস্থায়ী_string_বাফার);
	    remove_লেজ(default_vim_dir);
	    remove_লেজ(default_vim_dir);

	    প্রবেশ = 'n';
	    করো
	    {
		যদি (প্রবেশ != 'n')
		    printf("%c is an invalid reply.  Please enter either 'y' or 'n'\n", প্রবেশ);

		যদি (skip_question)
		    প্রবেশ = 'y';
		অন্যথা
		{
		    rewind(stdin);
		    scanf("%c", &প্রবেশ);
		}
		নির্বাচন (প্রবেশ)
		{
		    ক্ষেত্র 'y':
		    ক্ষেত্র 'Y':
			// save the number of uninstall keys so we can know if
			// it changed
			Reg_Query_Info_চাবি(চাবি_handle, ফাঁকা, ফাঁকা, ফাঁকা,
					     &orig_অঙ্ক_চাবিগুলি, ফাঁকা, ফাঁকা, ফাঁকা,
						      ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা);

			// Find existing .bat files before deleting them.
			find_bat_exe(TRUE);

			যদি (allow_silent)
			{
			    যদি (run_silent_uninstall(অস্থায়ী_string_বাফার)
								    == FAIL)
				allow_silent = 0; // Retry with non silent.
			}
			যদি (!allow_silent)
			{
			    // Execute the uninstall program.  Put it in double
			    // quotes if there is an embedded space.
			    {
				অক্ষর buf[BUFSIZE];

				যদি (strchr(অস্থায়ী_string_বাফার, ' ') != ফাঁকা)
				    sprintf(buf, "\"%s\"", অস্থায়ী_string_বাফার);
				অন্যথা
				    strcpy(buf, অস্থায়ী_string_বাফার);
				run_command(buf);
			    }

			    // Count the number of windows with a title that
			    // match the installer, so that we can check when
			    // it's done.  The uninstaller copies itself,
			    // executes the copy and exits, thus we can't wait
			    // for the process to finish.
			    sleep(1);  // wait for uninstaller to start up
			    অঙ্ক_windows = 0;
			    EnumWindows(window_cb, 0);
			    যদি (অঙ্ক_windows == 0)
			    {
				// Did not find the uninstaller, ask user to
				// press Enter when done. Just in case.
				printf("Press Enter when the uninstaller is finished\n");
				rewind(stdin);
				(শূন্য)getchar();
			    }
			    অন্যথা
			    {
				printf("Waiting for the uninstaller to finish (press CTRL-C to abort).");
				করো
				{
				    printf(".");
				    fflush(stdout);
				    sleep(1);	// wait for the uninstaller to
						// finish
				    অঙ্ক_windows = 0;
				    EnumWindows(window_cb, 0);
				} যখন (অঙ্ক_windows > 0);
			    }
			}
			printf("\nDone!\n");

			// Check if an uninstall reg key was deleted.
			// if it was, we want to decrement key_index.
			// if we don't do this, we will skip the key
			// immediately after any key that we delete.
			Reg_Query_Info_চাবি(চাবি_handle, ফাঁকা, ফাঁকা, ফাঁকা,
					      &new_অঙ্ক_চাবিগুলি, ফাঁকা, ফাঁকা, ফাঁকা,
						      ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা);
			যদি (new_অঙ্ক_চাবিগুলি < orig_অঙ্ক_চাবিগুলি)
			    চাবি_সূচক--;

			প্রবেশ = 'y';
			বিরতি;

		    ক্ষেত্র 'n':
		    ক্ষেত্র 'N':
			// Do not uninstall
			প্রবেশ = 'n';
			বিরতি;

		    পূর্বনির্ধারিত: // just drop through and redo the loop
			বিরতি;
		}

	    } যখন (প্রবেশ != 'n' && প্রবেশ != 'y');

	    Reg_Close_চাবি(uninstall_চাবি_handle);
	}

	চাবি_সূচক++;
    }
    Reg_Close_চাবি(চাবি_handle);

    ফেরত foundone;
}

/*
 * Find out information about the system.
 */
    স্থির শূন্য
inspect_system(শূন্য)
{
    অক্ষর	*p;
    অক্ষর	buf[BUFSIZE];
    FILE	*fd;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		foundone;

    // This may take a little while, let the user know what we're doing.
    printf("Inspecting system...\n");

    /*
     * If $VIM is set, check that it's pointing to our directory.
     */
    p = getenv("VIM");
    যদি (p != ফাঁকা && pathcmp(p, -1, installdir, runtimeidx - 1) != 0)
    {
	printf("------------------------------------------------------\n");
	printf("$VIM is set to \"%s\".\n", p);
	printf("This is different from where this version of Vim is:\n");
	strcpy(buf, installdir);
	*(buf + runtimeidx - 1) = NUL;
	printf("\"%s\"\n", buf);
	printf("You must adjust or remove the setting of $VIM,\n");
	যদি (interactive)
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
    যদি (p != ফাঁকা && pathcmp(p, -1, installdir, -1) != 0)
    {
	printf("------------------------------------------------------\n");
	printf("$VIMRUNTIME is set to \"%s\".\n", p);
	printf("This is different from where this version of Vim is:\n");
	printf("\"%s\"\n", installdir);
	printf("You must adjust or remove the setting of $VIMRUNTIME,\n");
	যদি (interactive)
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
    জন্য (i = 1; i < TARGET_গুনতি; ++i)
    {
	findoldfile(&(targets[i].oldexe));
	যদি (targets[i].oldexe != ফাঁকা)
	    foundone = 1;
    }

    যদি (foundone)
    {
	printf("Warning: Found Vim executable(s) in your $PATH:\n");
	জন্য (i = 1; i < TARGET_গুনতি; ++i)
	    যদি (targets[i].oldexe != ফাঁকা)
		printf("%s\n", targets[i].oldexe);
	printf("It will be used instead of the version you are installing.\n");
	printf("Please delete or rename it, or adjust your $PATH setting.\n");
    }

    /*
     * Check if there is an existing ../_vimrc or ../.vimrc file.
     */
    strcpy(oldvimrc, installdir);
    strcpy(oldvimrc + runtimeidx, "_vimrc");
    যদি ((fd = fopen(oldvimrc, "r")) == ফাঁকা)
    {
	strcpy(oldvimrc + runtimeidx, "vimrc~1"); // short version of .vimrc
	যদি ((fd = fopen(oldvimrc, "r")) == ফাঁকা)
	{
	    strcpy(oldvimrc + runtimeidx, ".vimrc");
	    fd = fopen(oldvimrc, "r");
	}
    }
    যদি (fd != ফাঁকা)
	fclose(fd);
    অন্যথা
	*oldvimrc = NUL;
}

/*
 * Add a dummy choice to avoid that the numbering changes depending on items
 * in the environment.  The user may type a number he remembered without
 * looking.
 */
    স্থির শূন্য
add_dummy_choice(শূন্য)
{
    choices[choice_গুনতি].installfunc = ফাঁকা;
    choices[choice_গুনতি].active = 0;
    choices[choice_গুনতি].changefunc = ফাঁকা;
    choices[choice_গুনতি].text = ফাঁকা;
    choices[choice_গুনতি].arg = 0;
    ++choice_গুনতি;
}

////////////////////////////////////////////////
// stuff for creating the batch files.

/*
 * Install the vim.bat, gvim.bat, etc. files.
 */
    স্থির শূন্য
install_bat_choice(পূর্ণসংখ্যা idx)
{
    অক্ষর	*batpath = targets[choices[idx].arg].batpath;
    অক্ষর	*oldname = targets[choices[idx].arg].oldbat;
    অক্ষর	*exename = targets[choices[idx].arg].exenamearg;
    অক্ষর	*vimarg = targets[choices[idx].arg].exearg;
    FILE	*fd;

    যদি (*batpath == NUL)
	ফেরত;

    fd = fopen(batpath, "w");
    যদি (fd == ফাঁকা)
    {
	printf("\nERROR: Cannot open \"%s\" for writing.\n", batpath);
	ফেরত;
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

    যদি (*exename == 'g')
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

    যদি (*exename == 'g')
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
    অন্যথা
    {
	// Always use quotes, $VIM or $VIMRUNTIME might have a space.
	fprintf(fd, "\"%%VIM_EXE_DIR%%\\%s\" %s %%*\n",
		exename, vimarg);
    }

    fclose(fd);
    printf("%s has been %s\n", batpath,
	    oldname == ফাঁকা ? "created" : "overwritten");
}

/*
 * Make the text string for choice "idx".
 * The format "fmt" is must have one %s item, which "arg" is used for.
 */
    স্থির শূন্য
alloc_text(পূর্ণসংখ্যা idx, অক্ষর *fmt, অক্ষর *arg)
{
    যদি (choices[idx].text != ফাঁকা)
	মুক্ত(choices[idx].text);

    choices[idx].text = alloc(strlen(fmt) + strlen(arg) - 1);
    sprintf(choices[idx].text, fmt, arg);
}

/*
 * Toggle the "Overwrite .../vim.bat" to "Don't overwrite".
 */
    স্থির শূন্য
toggle_bat_choice(পূর্ণসংখ্যা idx)
{
    অক্ষর	*batname = targets[choices[idx].arg].batpath;
    অক্ষর	*oldname = targets[choices[idx].arg].oldbat;

    যদি (*batname == NUL)
    {
	alloc_text(idx, "    Overwrite %s", oldname);
	strcpy(batname, oldname);
    }
    অন্যথা
    {
	alloc_text(idx, "    Do NOT overwrite %s", oldname);
	*batname = NUL;
    }
}

/*
 * Do some work for a batch file entry: Append the batch file name to the path
 * and set the text for the choice.
 */
    স্থির শূন্য
set_bat_text(পূর্ণসংখ্যা idx, অক্ষর *batpath, অক্ষর *name)
{
    strcat(batpath, name);

    alloc_text(idx, "    Create %s", batpath);
}

/*
 * Select a directory to write the batch file line.
 */
    স্থির শূন্য
change_bat_choice(পূর্ণসংখ্যা idx)
{
    অক্ষর	*path;
    অক্ষর	*batpath;
    অক্ষর	*name;
    পূর্ণসংখ্যা		n;
    অক্ষর	*s;
    অক্ষর	*p;
    পূর্ণসংখ্যা		গুনতি;
    অক্ষর	**names = ফাঁকা;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		target = choices[idx].arg;

    name = targets[target].batname;
    batpath = targets[target].batpath;

    path = getenv("PATH");
    যদি (path == ফাঁকা)
    {
	printf("\nERROR: The variable $PATH is not set\n");
	ফেরত;
    }

    /*
     * first round: count number of names in path;
     * second round: save names to names[].
     */
    জন্য (;;)
    {
	গুনতি = 1;
	জন্য (p = path; *p; )
	{
	    s = strchr(p, ';');
	    যদি (s == ফাঁকা)
		s = p + strlen(p);
	    যদি (names != ফাঁকা)
	    {
		names[গুনতি] = alloc(s - p + 1);
		strncpy(names[গুনতি], p, s - p);
		names[গুনতি][s - p] = NUL;
	    }
	    ++গুনতি;
	    p = s;
	    যদি (*p != NUL)
		++p;
	}
	যদি (names != ফাঁকা)
	    বিরতি;
	names = alloc((গুনতি + 1) * আকার(অক্ষর *));
    }
    names[0] = alloc(50);
    sprintf(names[0], "Select directory to create %s in:", name);
    names[গুনতি] = alloc(50);
    যদি (choices[idx].arg == 0)
	sprintf(names[গুনতি], "Do not create any .bat file.");
    অন্যথা
	sprintf(names[গুনতি], "Do not create a %s file.", name);
    n = get_choice(names, গুনতি + 1);

    যদি (n == গুনতি)
    {
	// Selected last item, don't create bat file.
	*batpath = NUL;
	যদি (choices[idx].arg != 0)
	    alloc_text(idx, "    Do NOT create %s", name);
    }
    অন্যথা
    {
	// Selected one of the paths.  For the first item only keep the path,
	// for the others append the batch file name.
	strcpy(batpath, names[n]);
	add_pathsep(batpath);
	যদি (choices[idx].arg != 0)
	    set_bat_text(idx, batpath, name);
    }

    জন্য (i = 0; i <= গুনতি; ++i)
	মুক্ত(names[i]);
    মুক্ত(names);
}

অক্ষর *bat_text_yes = "Install .bat files to use Vim at the command line:";
অক্ষর *bat_text_no = "do NOT install .bat files to use Vim at the command line";

    স্থির শূন্য
change_main_bat_choice(পূর্ণসংখ্যা idx)
{
    পূর্ণসংখ্যা		i;

    // let the user select a default directory or NONE
    change_bat_choice(idx);

    যদি (targets[0].batpath[0] != NUL)
	choices[idx].text = bat_text_yes;
    অন্যথা
	choices[idx].text = bat_text_no;

    // update the individual batch file selections
    জন্য (i = 1; i < TARGET_গুনতি; ++i)
    {
	// Only make it active when the first item has a path and the vim.exe
	// or gvim.exe exists (there is a changefunc then).
	যদি (targets[0].batpath[0] != NUL
		&& choices[idx + i].changefunc != ফাঁকা)
	{
	    choices[idx + i].active = 1;
	    যদি (choices[idx + i].changefunc == change_bat_choice
		    && targets[i].batpath[0] != NUL)
	    {
		strcpy(targets[i].batpath, targets[0].batpath);
		set_bat_text(idx + i, targets[i].batpath, targets[i].batname);
	    }
	}
	অন্যথা
	    choices[idx + i].active = 0;
    }
}

/*
 * Initialize a choice for creating a batch file.
 */
    স্থির শূন্য
init_bat_choice(পূর্ণসংখ্যা target)
{
    অক্ষর	*batpath = targets[target].batpath;
    অক্ষর	*oldbat = targets[target].oldbat;
    অক্ষর	*p;
    পূর্ণসংখ্যা		i;

    choices[choice_গুনতি].arg = target;
    choices[choice_গুনতি].installfunc = install_bat_choice;
    choices[choice_গুনতি].active = 1;
    choices[choice_গুনতি].text = ফাঁকা;	// will be set below
    যদি (oldbat != ফাঁকা)
    {
	// A [g]vim.bat exists: Only choice is to overwrite it or not.
	choices[choice_গুনতি].changefunc = toggle_bat_choice;
	*batpath = NUL;
	toggle_bat_choice(choice_গুনতি);
    }
    অন্যথা
    {
	যদি (default_bat_dir != ফাঁকা)
	    // Prefer using the same path as an existing .bat file.
	    strcpy(batpath, default_bat_dir);
	অন্যথা
	{
	    // No [g]vim.bat exists: Write it to a directory in $PATH.  Use
	    // $WINDIR by default, if it's empty the first item in $PATH.
	    p = getenv("WINDIR");
	    যদি (p != ফাঁকা && *p != NUL)
		strcpy(batpath, p);
	    অন্যথা
	    {
		p = getenv("PATH");
		যদি (p == ফাঁকা || *p == NUL)		// "cannot happen"
		    strcpy(batpath, "C:/Windows");
		অন্যথা
		{
		    i = 0;
		    যখন (*p != NUL && *p != ';')
			batpath[i++] = *p++;
		    batpath[i] = NUL;
		}
	    }
	}
	add_pathsep(batpath);
	set_bat_text(choice_গুনতি, batpath, targets[target].batname);

	choices[choice_গুনতি].changefunc = change_bat_choice;
    }
    ++choice_গুনতি;
}

/*
 * Set up the choices for installing .bat files.
 * For these items "arg" is the index in targets[].
 */
    স্থির শূন্য
init_bat_choices(শূন্য)
{
    পূর্ণসংখ্যা		i;

    // The first item is used to switch installing batch files on/off and
    // setting the default path.
    choices[choice_গুনতি].text = bat_text_yes;
    choices[choice_গুনতি].changefunc = change_main_bat_choice;
    choices[choice_গুনতি].installfunc = ফাঁকা;
    choices[choice_গুনতি].active = 1;
    choices[choice_গুনতি].arg = 0;
    ++choice_গুনতি;

    // Add items for each batch file target.  Only used when not disabled by
    // the first item.  When a .exe exists, don't offer to create a .bat.
    জন্য (i = 1; i < TARGET_গুনতি; ++i)
	যদি (targets[i].oldexe == ফাঁকা
		&& (targets[i].exenamearg[0] == 'g' ? has_gvim : has_vim))
	    init_bat_choice(i);
	অন্যথা
	    add_dummy_choice();
}

/*
 * Install the vimrc file.
 */
    স্থির শূন্য
install_vimrc(পূর্ণসংখ্যা idx UNUSED)
{
    FILE	*fd, *tfd;
    অক্ষর	*fname;

    // If an old vimrc file exists, overwrite it.
    // Otherwise create a new one.
    যদি (*oldvimrc != NUL)
	fname = oldvimrc;
    অন্যথা
	fname = vimrc;

    fd = fopen(fname, "w");
    যদি (fd == ফাঁকা)
    {
	printf("\nERROR: Cannot open \"%s\" for writing.\n", fname);
	ফেরত;
    }
    নির্বাচন (compat_choice)
    {
	ক্ষেত্র compat_vi:
		    fprintf(fd, "\" Vi compatible\n");
		    fprintf(fd, "set compatible\n");
		    বিরতি;
	ক্ষেত্র compat_vim:
		    fprintf(fd, "\" Vim's default behavior\n");
		    fprintf(fd, "if &compatible\n");
		    fprintf(fd, "  set nocompatible\n");
		    fprintf(fd, "endif\n");
		    বিরতি;
	ক্ষেত্র compat_some_enhancements:
		    fprintf(fd, "\" Vim with some enhancements\n");
		    fprintf(fd, "source $VIMRUNTIME/defaults.vim\n");
		    বিরতি;
	ক্ষেত্র compat_all_enhancements:
		    fprintf(fd, "\" Vim with all enhancements\n");
		    fprintf(fd, "source $VIMRUNTIME/vimrc_example.vim\n");
		    বিরতি;
    }
    নির্বাচন (remap_choice)
    {
	ক্ষেত্র remap_no:
		    বিরতি;
	ক্ষেত্র remap_win:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Remap a few keys for Windows behavior\n");
		    fprintf(fd, "source $VIMRUNTIME/mswin.vim\n");
		    বিরতি;
    }
    নির্বাচন (mouse_choice)
    {
	ক্ষেত্র mouse_xterm:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Mouse behavior (the Unix way)\n");
		    fprintf(fd, "behave xterm\n");
		    বিরতি;
	ক্ষেত্র mouse_mswin:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Mouse behavior (the Windows way)\n");
		    fprintf(fd, "behave mswin\n");
		    বিরতি;
	ক্ষেত্র mouse_default:
		    বিরতি;
    }
    যদি ((tfd = fopen("diff.exe", "r")) != ফাঁকা)
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

    স্থির শূন্য
change_vimrc_choice(পূর্ণসংখ্যা idx)
{
    যদি (choices[idx].installfunc != ফাঁকা)
    {
	// Switch to NOT change or create a vimrc file.
	যদি (*oldvimrc != NUL)
	    alloc_text(idx, "Do NOT change startup file %s", oldvimrc);
	অন্যথা
	    alloc_text(idx, "Do NOT create startup file %s", vimrc);
	choices[idx].installfunc = ফাঁকা;
	choices[idx + 1].active = 0;
	choices[idx + 2].active = 0;
	choices[idx + 3].active = 0;
    }
    অন্যথা
    {
	// Switch to change or create a vimrc file.
	যদি (*oldvimrc != NUL)
	    alloc_text(idx, "Overwrite startup file %s with:", oldvimrc);
	অন্যথা
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
    স্থির শূন্য
change_run_choice(পূর্ণসংখ্যা idx)
{
    compat_choice = get_choice(compat_choices, TABLE_আকার(compat_choices));
    alloc_text(idx, compat_text, compat_choices[compat_choice]);
}

/*
 * Change the choice if keys are to be remapped.
 */
    স্থির শূন্য
change_remap_choice(পূর্ণসংখ্যা idx)
{
    remap_choice = get_choice(remap_choices, TABLE_আকার(remap_choices));
    alloc_text(idx, remap_text, remap_choices[remap_choice]);
}

/*
 * Change the choice how to select text.
 */
    স্থির শূন্য
change_mouse_choice(পূর্ণসংখ্যা idx)
{
    mouse_choice = get_choice(mouse_choices, TABLE_আকার(mouse_choices));
    alloc_text(idx, mouse_text, mouse_choices[mouse_choice]);
}

    স্থির শূন্য
init_vimrc_choices(শূন্য)
{
    // set path for a new _vimrc file (also when not used)
    strcpy(vimrc, installdir);
    strcpy(vimrc + runtimeidx, "_vimrc");

    // Set opposite value and then toggle it by calling change_vimrc_choice()
    যদি (*oldvimrc == NUL)
	choices[choice_গুনতি].installfunc = ফাঁকা;
    অন্যথা
	choices[choice_গুনতি].installfunc = install_vimrc;
    choices[choice_গুনতি].text = ফাঁকা;
    change_vimrc_choice(choice_গুনতি);
    choices[choice_গুনতি].changefunc = change_vimrc_choice;
    choices[choice_গুনতি].active = 1;
    ++choice_গুনতি;

    // default way to run Vim
    alloc_text(choice_গুনতি, compat_text, compat_choices[compat_choice]);
    choices[choice_গুনতি].changefunc = change_run_choice;
    choices[choice_গুনতি].installfunc = ফাঁকা;
    choices[choice_গুনতি].active = (*oldvimrc == NUL);
    ++choice_গুনতি;

    // Whether to remap keys
    alloc_text(choice_গুনতি, remap_text , remap_choices[remap_choice]);
    choices[choice_গুনতি].changefunc = change_remap_choice;
    choices[choice_গুনতি].installfunc = ফাঁকা;
    choices[choice_গুনতি].active = (*oldvimrc == NUL);
    ++choice_গুনতি;

    // default way to use the mouse
    alloc_text(choice_গুনতি, mouse_text, mouse_choices[mouse_choice]);
    choices[choice_গুনতি].changefunc = change_mouse_choice;
    choices[choice_গুনতি].installfunc = ফাঁকা;
    choices[choice_গুনতি].active = (*oldvimrc == NUL);
    ++choice_গুনতি;
}

    স্থির LONG
reg_create_চাবি(
    HKEY মূল,
    ধ্রুবক অক্ষর *subkey,
    PHKEY ph_চাবি,
    DWORD flag)
{
    DWORD disp;

    *ph_চাবি = ফাঁকা;
    ফেরত Reg_Create_চাবি_Ex(
		মূল, subkey,
		0, ফাঁকা, REG_OPTION_NON_VOLATILE,
		flag | চাবি_WRITE,
		ফাঁকা, ph_চাবি, &disp);
}

    স্থির LONG
reg_set_string_মান(
    HKEY h_চাবি,
    ধ্রুবক অক্ষর *মান_name,
    ধ্রুবক অক্ষর *তথ্য)
{
    ফেরত Reg_Set_মান_Ex(h_চাবি, মান_name, 0, REG_SZ,
				     (LPBYTE)তথ্য, (DWORD)(1 + strlen(তথ্য)));
}

    স্থির LONG
reg_create_চাবি_and_মান(
    HKEY h_মূল_চাবি,
    ধ্রুবক অক্ষর *subkey,
    ধ্রুবক অক্ষর *মান_name,
    ধ্রুবক অক্ষর *তথ্য,
    DWORD flag)
{
    HKEY h_চাবি;
    LONG lRet = reg_create_চাবি(h_মূল_চাবি, subkey, &h_চাবি, flag);

    যদি (ERROR_SUCCESS == lRet)
    {
	lRet = reg_set_string_মান(h_চাবি, মান_name, তথ্য);
	Reg_Close_চাবি(h_চাবি);
    }
    ফেরত lRet;
}

    স্থির LONG
register_inproc_server(
    HKEY h_মূল_চাবি,
    ধ্রুবক অক্ষর *clsid,
    ধ্রুবক অক্ষর *extname,
    ধ্রুবক অক্ষর *module,
    ধ্রুবক অক্ষর *threading_model,
    DWORD flag)
{
    CHAR subkey[BUFSIZE];
    LONG lRet;

    sprintf(subkey, "CLSID\\%s", clsid);
    lRet = reg_create_চাবি_and_মান(h_মূল_চাবি, subkey, ফাঁকা, extname, flag);
    যদি (ERROR_SUCCESS == lRet)
    {
	sprintf(subkey, "CLSID\\%s\\InProcServer32", clsid);
	lRet = reg_create_চাবি_and_মান(h_মূল_চাবি, subkey, ফাঁকা, module, flag);
	যদি (ERROR_SUCCESS == lRet)
	{
	    lRet = reg_create_চাবি_and_মান(h_মূল_চাবি, subkey,
					   "ThreadingModel", threading_model, flag);
	}
    }
    ফেরত lRet;
}

    স্থির LONG
register_shellex(
    HKEY h_মূল_চাবি,
    ধ্রুবক অক্ষর *clsid,
    ধ্রুবক অক্ষর *name,
    ধ্রুবক অক্ষর *exe_path,
    DWORD flag)
{
    LONG lRet = reg_create_চাবি_and_মান(
	    h_মূল_চাবি,
	    "*\\shellex\\ContextMenuHandlers\\gvim",
	    ফাঁকা,
	    clsid,
	    flag);

    যদি (ERROR_SUCCESS == lRet)
    {
	lRet = reg_create_চাবি_and_মান(
		HKEY_LOCAL_MACHINE,
		"Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved",
		clsid,
		name,
		flag);

	যদি (ERROR_SUCCESS == lRet)
	{
	    lRet = reg_create_চাবি_and_মান(
		    HKEY_LOCAL_MACHINE,
		    "Software\\Vim\\Gvim",
		    "path",
		    exe_path,
		    flag);
	}
    }
    ফেরত lRet;
}

    স্থির LONG
register_openwith(
    HKEY h_মূল_চাবি,
    ধ্রুবক অক্ষর *exe_path,
    DWORD flag)
{
    অক্ষর	exe_cmd[BUFSIZE];
    LONG	lRet;

    sprintf(exe_cmd, "\"%s\" \"%%1\"", exe_path);
    lRet = reg_create_চাবি_and_মান(
	    h_মূল_চাবি,
	    "Applications\\gvim.exe\\shell\\edit\\command",
	    ফাঁকা,
	    exe_cmd,
	    flag);

    যদি (ERROR_SUCCESS == lRet)
    {
	পূর্ণসংখ্যা i;
	স্থির ধ্রুবক অক্ষর *openwith[] = {
		".htm\\OpenWithList\\gvim.exe",
		".vim\\OpenWithList\\gvim.exe",
		"*\\OpenWithList\\gvim.exe",
	};

	জন্য (i = 0; ERROR_SUCCESS == lRet && i < (পূর্ণসংখ্যা)ARRAYSIZE(openwith); i++)
	    lRet = reg_create_চাবি_and_মান(h_মূল_চাবি, openwith[i], ফাঁকা, "", flag);
    }

    ফেরত lRet;
}

    স্থির LONG
register_uninstall(
    HKEY h_মূল_চাবি,
    ধ্রুবক অক্ষর *appname,
    ধ্রুবক অক্ষর *display_name,
    ধ্রুবক অক্ষর *uninstall_string,
    ধ্রুবক অক্ষর *display_icon,
    ধ্রুবক অক্ষর *display_version,
    ধ্রুবক অক্ষর *publisher)
{
    LONG lRet = reg_create_চাবি_and_মান(h_মূল_চাবি, appname,
			     "DisplayName", display_name, চাবি_WOW64_64_চাবি);

    যদি (ERROR_SUCCESS == lRet)
	lRet = reg_create_চাবি_and_মান(h_মূল_চাবি, appname,
		     "UninstallString", uninstall_string, চাবি_WOW64_64_চাবি);
    যদি (ERROR_SUCCESS == lRet)
	lRet = reg_create_চাবি_and_মান(h_মূল_চাবি, appname,
		     "DisplayIcon", display_icon, চাবি_WOW64_64_চাবি);
    যদি (ERROR_SUCCESS == lRet)
	lRet = reg_create_চাবি_and_মান(h_মূল_চাবি, appname,
		     "DisplayVersion", display_version, চাবি_WOW64_64_চাবি);
    যদি (ERROR_SUCCESS == lRet)
	lRet = reg_create_চাবি_and_মান(h_মূল_চাবি, appname,
		     "Publisher", publisher, চাবি_WOW64_64_চাবি);
    ফেরত lRet;
}

/*
 * Add some entries to the registry:
 * - to add "Edit with Vim" to the context * menu
 * - to add Vim to the "Open with..." list
 * - to uninstall Vim
 */
//ARGSUSED
    স্থির পূর্ণসংখ্যা
install_registry(শূন্য)
{
    LONG	lRet = ERROR_SUCCESS;
    ধ্রুবক অক্ষর	*vim_ext_ThreadingModel = "Apartment";
    ধ্রুবক অক্ষর	*vim_ext_name = "Vim Shell Extension";
    ধ্রুবক অক্ষর	*vim_ext_clsid = "{51EEE242-AD87-11d3-9C1E-0090278BBD99}";
    অক্ষর	vim_exe_path[MAX_PATH];
    অক্ষর	display_name[BUFSIZE];
    অক্ষর	uninstall_string[BUFSIZE];
    অক্ষর	icon_string[BUFSIZE];
    অক্ষর	version_string[BUFSIZE];
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		loop_গুনতি = is_64bit_os() ? 2 : 1;
    DWORD	flag;

    sprintf(vim_exe_path, "%s\\gvim.exe", installdir);

    যদি (install_popup)
    {
	অক্ষর	    bufg[BUFSIZE];

	printf("Creating \"Edit with Vim\" popup menu entry\n");

	জন্য (i = 0; i < loop_গুনতি; i++)
	{
	    যদি (i == 0)
	    {
		sprintf(bufg, "%s\\" GVIMEXT32_PATH, installdir);
		flag = চাবি_WOW64_32_চাবি;
	    }
	    অন্যথা
	    {
		sprintf(bufg, "%s\\" GVIMEXT64_PATH, installdir);
		flag = চাবি_WOW64_64_চাবি;
	    }

	    lRet = register_inproc_server(
		    HKEY_CLASSES_মূল, vim_ext_clsid, vim_ext_name,
		    bufg, vim_ext_ThreadingModel, flag);
	    যদি (ERROR_SUCCESS != lRet)
		ফেরত FAIL;
	    lRet = register_shellex(
		    HKEY_CLASSES_মূল, vim_ext_clsid, vim_ext_name,
		    vim_exe_path, flag);
	    যদি (ERROR_SUCCESS != lRet)
		ফেরত FAIL;
	}
    }

    যদি (install_openwith)
    {
	printf("Creating \"Open with ...\" list entry\n");

	জন্য (i = 0; i < loop_গুনতি; i++)
	{
	    যদি (i == 0)
		flag = চাবি_WOW64_32_চাবি;
	    অন্যথা
		flag = চাবি_WOW64_64_চাবি;

	    lRet = register_openwith(HKEY_CLASSES_মূল, vim_exe_path, flag);
	    যদি (ERROR_SUCCESS != lRet)
		ফেরত FAIL;
	}
    }

    printf("Creating an uninstall entry\n");
    sprintf(display_name, "Vim " VIM_VERSION_SHORT
#ifdef _M_ARM64
	    " (arm64)"
#নতুবা_যদি _M_X64
	    " (x64)"
#endif
	    );

    // For the NSIS installer use the generated uninstaller.
    যদি (interactive)
	sprintf(uninstall_string, "%s\\uninstall.exe", installdir);
    অন্যথা
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
    যদি (ERROR_SUCCESS != lRet)
	ফেরত FAIL;

    ফেরত OK;
}

    স্থির শূন্য
change_popup_choice(পূর্ণসংখ্যা idx)
{
    যদি (install_popup == 0)
    {
	choices[idx].text = "Install an entry for Vim in the popup menu for the right\n    mouse button so that you can edit any file with Vim";
	install_popup = 1;
    }
    অন্যথা
    {
	choices[idx].text = "Do NOT install an entry for Vim in the popup menu for the\n    right mouse button to edit any file with Vim";
	install_popup = 0;
    }
}

/*
 * Only add the choice for the popup menu entry when gvim.exe was found and
 * both gvimext.dll and regedit.exe exist.
 */
    স্থির শূন্য
init_popup_choice(শূন্য)
{
    কাঠামো stat	st;

    যদি (has_gvim
	    && (stat(GVIMEXT32_PATH, &st) >= 0
		|| stat(GVIMEXT64_PATH, &st) >= 0))
    {
	choices[choice_গুনতি].changefunc = change_popup_choice;
	choices[choice_গুনতি].installfunc = ফাঁকা;
	choices[choice_গুনতি].active = 1;
	change_popup_choice(choice_গুনতি);  // set the text
	++choice_গুনতি;
    }
    অন্যথা
	add_dummy_choice();
}

    স্থির শূন্য
change_openwith_choice(পূর্ণসংখ্যা idx)
{
    যদি (install_openwith == 0)
    {
	choices[idx].text = "Add Vim to the \"Open With...\" list in the popup menu for the right\n    mouse button so that you can edit any file with Vim";
	install_openwith = 1;
    }
    অন্যথা
    {
	choices[idx].text = "Do NOT add Vim to the \"Open With...\" list in the popup menu for the\n    right mouse button to edit any file with Vim";
	install_openwith = 0;
    }
}

/*
 * Only add the choice for the open-with menu entry when gvim.exe was found
 * and regedit.exe exist.
 */
    স্থির শূন্য
init_openwith_choice(শূন্য)
{
    যদি (has_gvim)
    {
	choices[choice_গুনতি].changefunc = change_openwith_choice;
	choices[choice_গুনতি].installfunc = ফাঁকা;
	choices[choice_গুনতি].active = 1;
	change_openwith_choice(choice_গুনতি);  // set the text
	++choice_গুনতি;
    }
    অন্যথা
	add_dummy_choice();
}

/*
 * Create a shell link.
 *
 * returns 0 on failure, non-zero on successful completion.
 *
 * NOTE:  Currently untested with mingw.
 */
    পূর্ণসংখ্যা
create_shortcut(
	ধ্রুবক অক্ষর *shortcut_name,
	ধ্রুবক অক্ষর *iconfile_path,
	পূর্ণসংখ্যা	    iconindex,
	ধ্রুবক অক্ষর *shortcut_target,
	ধ্রুবক অক্ষর *shortcut_args,
	ধ্রুবক অক্ষর *workingdir
	)
{
    IShellLink	    *shelllink_ptr;
    HRESULT	    hres;
    IPersistFile	    *persistfile_ptr;

    // Initialize COM library
    hres = CoInitialize(ফাঁকা);
    যদি (!SUCCEEDED(hres))
    {
	printf("Error:  Could not open the COM library.  Not creating shortcut.\n");
	ফেরত FAIL;
    }

    // Instantiate a COM object for the ShellLink, store a pointer to it
    // in shelllink_ptr.
    hres = CoCreateInstance(&CLSID_ShellLink,
			   ফাঁকা,
			   CLSCTX_INPROC_SERVER,
			   &IID_IShellLink,
			   (শূন্য **) &shelllink_ptr);

    যদি (SUCCEEDED(hres)) // If the instantiation was successful...
    {
	// ...Then build a PersistFile interface for the ShellLink so we can
	// save it as a file after we build it.
	hres = shelllink_ptr->lpVtbl->QueryInterface(shelllink_ptr,
		&IID_IPersistFile, (শূন্য **) &persistfile_ptr);

	যদি (SUCCEEDED(hres))
	{
	    wchar_t wsz[BUFSIZE];

	    // translate the (possibly) multibyte shortcut filename to windows
	    // Unicode so it can be used as a file name.
	    MultiByteToWideChar(CP_ACP, 0, shortcut_name, -1, wsz, আকার(wsz)/আকার(wsz[0]));

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
	অন্যথা
	{
	    printf("QueryInterface Error\n");
	    ফেরত FAIL;
	}

	// Return the ShellLink object
	shelllink_ptr->lpVtbl->Release(shelllink_ptr);
    }
    অন্যথা
    {
	printf("CoCreateInstance Error - hres = %08x\n", (পূর্ণসংখ্যা)hres);
	ফেরত FAIL;
    }

    ফেরত OK;
}

/*
 * Build a path to where we will put a specified link.
 *
 * Return 0 on error, non-zero on success
 */
   পূর্ণসংখ্যা
build_link_name(
	অক্ষর	   *link_path,
	ধ্রুবক অক্ষর *link_name,
	ধ্রুবক অক্ষর *shell_folder_name)
{
    অক্ষর	shell_folder_path[MAX_PATH];

    যদি (get_shell_folder_path(shell_folder_path, shell_folder_name) == FAIL)
    {
	printf("An error occurred while attempting to find the path to %s.\n",
							   shell_folder_name);
	ফেরত FAIL;
    }

    // Make sure the directory exists (create Start Menu\Programs\Vim).
    // Ignore errors if it already exists.
    vim_mkdir(shell_folder_path, 0755);

    // build the path to the shortcut and the path to gvim.exe
    sprintf(link_path, "%s\\%s.lnk", shell_folder_path, link_name);

    ফেরত OK;
}

    স্থির পূর্ণসংখ্যা
build_shortcut(
	ধ্রুবক অক্ষর *name,	// Name of the shortcut
	ধ্রুবক অক্ষর *exename,	// Name of the executable (e.g., vim.exe)
	ধ্রুবক অক্ষর *args,
	ধ্রুবক অক্ষর *shell_folder,
	ধ্রুবক অক্ষর *workingdir)
{
    অক্ষর	executable_path[BUFSIZE];
    অক্ষর	link_name[BUFSIZE];

    sprintf(executable_path, "%s\\%s", installdir, exename);

    যদি (build_link_name(link_name, name, shell_folder) == FAIL)
    {
	printf("An error has occurred.  A shortcut to %s will not be created %s.\n",
		name,
		*shell_folder == 'd' ? "on the desktop" : "in the Start menu");
	ফেরত FAIL;
    }

    // Create the shortcut:
    ফেরত create_shortcut(link_name, executable_path, 0,
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
    স্থির শূন্য
install_start_menu(পূর্ণসংখ্যা idx UNUSED)
{
    need_uninstall_entry = 1;
    printf("Creating start menu\n");
    যদি (has_vim)
    {
	যদি (build_shortcut("Vim", "vim.exe", "",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ফেরত;
	যদি (build_shortcut("Vim Read-only", "vim.exe", "-R",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ফেরত;
	যদি (build_shortcut("Vim Diff", "vim.exe", "-d",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ফেরত;
    }
    যদি (has_gvim)
    {
	যদি (build_shortcut("gVim", "gvim.exe", "",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ফেরত;
	যদি (build_shortcut("gVim Easy", "gvim.exe", "-y",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ফেরত;
	যদি (build_shortcut("gVim Read-only", "gvim.exe", "-R",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ফেরত;
	যদি (build_shortcut("gVim Diff", "gvim.exe", "-d",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ফেরত;
    }
    যদি (build_shortcut("Uninstall",
		interactive ? "uninstall.exe" : "uninstall-gui.exe", "",
					   VIM_STARTMENU, installdir) == FAIL)
	ফেরত;
    // For Windows NT the working dir of the vimtutor.bat must be right,
    // otherwise gvim.exe won't be found and using gvimbat doesn't work.
    যদি (build_shortcut("Vim tutor", "vimtutor.bat", "",
					   VIM_STARTMENU, installdir) == FAIL)
	ফেরত;
    যদি (build_shortcut("Help", has_gvim ? "gvim.exe" : "vim.exe", "-c h",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	ফেরত;
    {
	অক্ষর	shell_folder_path[BUFSIZE];

	// Creating the URL shortcut works a bit differently...
	যদি (get_shell_folder_path(shell_folder_path, VIM_STARTMENU) == FAIL)
	{
	    printf("Finding the path of the Start menu failed\n");
	    ফেরত ;
	}
	add_pathsep(shell_folder_path);
	strcat(shell_folder_path, "Vim Online.url");
	যদি (!WritePrivateProfileString("InternetShortcut", "URL",
				    "https://www.vim.org/", shell_folder_path))
	{
	    printf("Creating the Vim online URL failed\n");
	    ফেরত;
	}
    }
}

    স্থির শূন্য
toggle_startmenu_choice(পূর্ণসংখ্যা idx)
{
    যদি (choices[idx].installfunc == ফাঁকা)
    {
	choices[idx].installfunc = install_start_menu;
	choices[idx].text = "Add Vim to the Start menu";
    }
    অন্যথা
    {
	choices[idx].installfunc = ফাঁকা;
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
    শূন্য
install_shortcut_gvim(পূর্ণসংখ্যা idx)
{
    // Create shortcut(s) on the desktop
    যদি (choices[idx].arg)
    {
	(শূন্য)build_shortcut(icon_names[0], "gvim.exe",
						      "", "desktop", WORKDIR);
	need_uninstall_entry = 1;
    }
}

    শূন্য
install_shortcut_evim(পূর্ণসংখ্যা idx)
{
    যদি (choices[idx].arg)
    {
	(শূন্য)build_shortcut(icon_names[1], "gvim.exe",
						    "-y", "desktop", WORKDIR);
	need_uninstall_entry = 1;
    }
}

    শূন্য
install_shortcut_gview(পূর্ণসংখ্যা idx)
{
    যদি (choices[idx].arg)
    {
	(শূন্য)build_shortcut(icon_names[2], "gvim.exe",
						    "-R", "desktop", WORKDIR);
	need_uninstall_entry = 1;
    }
}

    শূন্য
toggle_shortcut_choice(পূর্ণসংখ্যা idx)
{
    অক্ষর	*arg;

    যদি (choices[idx].installfunc == install_shortcut_gvim)
	arg = "gVim";
    অন্যথা যদি (choices[idx].installfunc == install_shortcut_evim)
	arg = "gVim Easy";
    অন্যথা
	arg = "gVim Read-only";
    যদি (choices[idx].arg)
    {
	choices[idx].arg = 0;
	alloc_text(idx, "Do NOT create a desktop icon for %s", arg);
    }
    অন্যথা
    {
	choices[idx].arg = 1;
	alloc_text(idx, "Create a desktop icon for %s", arg);
    }
}

    স্থির শূন্য
init_startmenu_choice(শূন্য)
{
    // Start menu
    choices[choice_গুনতি].changefunc = toggle_startmenu_choice;
    choices[choice_গুনতি].installfunc = ফাঁকা;
    choices[choice_গুনতি].active = 1;
    toggle_startmenu_choice(choice_গুনতি);	// set the text
    ++choice_গুনতি;
}

/*
 * Add the choice for the desktop shortcuts.
 */
    স্থির শূন্য
init_shortcut_choices(শূন্য)
{
    // Shortcut to gvim
    choices[choice_গুনতি].text = ফাঁকা;
    choices[choice_গুনতি].arg = 0;
    choices[choice_গুনতি].active = has_gvim;
    choices[choice_গুনতি].changefunc = toggle_shortcut_choice;
    choices[choice_গুনতি].installfunc = install_shortcut_gvim;
    toggle_shortcut_choice(choice_গুনতি);
    ++choice_গুনতি;

    // Shortcut to evim
    choices[choice_গুনতি].text = ফাঁকা;
    choices[choice_গুনতি].arg = 0;
    choices[choice_গুনতি].active = has_gvim;
    choices[choice_গুনতি].changefunc = toggle_shortcut_choice;
    choices[choice_গুনতি].installfunc = install_shortcut_evim;
    toggle_shortcut_choice(choice_গুনতি);
    ++choice_গুনতি;

    // Shortcut to gview
    choices[choice_গুনতি].text = ফাঁকা;
    choices[choice_গুনতি].arg = 0;
    choices[choice_গুনতি].active = has_gvim;
    choices[choice_গুনতি].changefunc = toggle_shortcut_choice;
    choices[choice_গুনতি].installfunc = install_shortcut_gview;
    toggle_shortcut_choice(choice_গুনতি);
    ++choice_গুনতি;
}

/*
 * Attempt to register OLE for Vim.
 */
   স্থির শূন্য
install_OLE_register(শূন্য)
{
    অক্ষর register_command_string[BUFSIZE + 30];

    printf("\n--- Attempting to register Vim with OLE ---\n");
    printf("(There is no message whether this works or not.)\n");

    sprintf(register_command_string, "\"%s\\gvim.exe\" -silent -register", installdir);
    system(register_command_string);
}

/*
 * Remove the last part of directory "path[]" to get its parent, and put the
 * result in "to[]".
 */
    স্থির শূন্য
dir_remove_last(ধ্রুবক অক্ষর *path, অক্ষর to[MAX_PATH])
{
    অক্ষর c;
    long last_char_to_copy;
    long path_দৈর্ঘ্য = strlen(path);

    // skip the last character just in case it is a '\\'
    last_char_to_copy = path_দৈর্ঘ্য - 2;
    c = path[last_char_to_copy];

    যখন (c != '\\')
    {
	last_char_to_copy--;
	c = path[last_char_to_copy];
    }

    strncpy(to, path, (size_t)last_char_to_copy);
    to[last_char_to_copy] = NUL;
}

    স্থির শূন্য
set_directories_text(পূর্ণসংখ্যা idx)
{
    পূর্ণসংখ্যা vimfiles_dir_choice = choices[idx].arg;

    যদি (vimfiles_dir_choice == (পূর্ণসংখ্যা)vimfiles_dir_none)
	alloc_text(idx, "Do NOT create plugin directories%s", "");
    অন্যথা
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
স্থির অক্ষর *homedir = ফাঁকা;

    শূন্য
init_homedir(শূন্য)
{
    অক্ষর    *var;
    অক্ষর    buf[MAX_PATH];

    যদি (homedir != ফাঁকা)
    {
	মুক্ত(homedir);
	homedir = ফাঁকা;
    }

    var = getenv("HOME");

    /*
     * Typically, $HOME is not defined on Windows, unless the user has
     * specifically defined it for Vim's sake.  However, on Windows NT
     * platforms, $HOMEDRIVE and $HOMEPATH are automatically defined for
     * each user.  Try constructing $HOME from these.
     */
    যদি (var == ফাঁকা || *var == NUL)
    {
	অক্ষর	*homedrive, *homepath;

	homedrive = getenv("HOMEDRIVE");
	homepath = getenv("HOMEPATH");
	যদি (homepath == ফাঁকা || *homepath == NUL)
	    homepath = "\\";
	যদি (homedrive != ফাঁকা
		   && strlen(homedrive) + strlen(homepath) < আকার(buf))
	{
	    যদি (snprintf(buf, আকার(buf), "%s%s", homedrive, homepath) > 0
		&& buf[0] != NUL)
		var = buf;
	}
    }

    যদি (var == ফাঁকা)
	var = getenv("USERPROFILE");

    /*
     * Weird but true: $HOME may contain an indirect reference to another
     * variable, esp. "%USERPROFILE%".  Happens when $USERPROFILE isn't set
     * when $HOME is being set.
     */
    যদি (var != ফাঁকা && *var == '%')
    {
	অক্ষর	*p;
	অক্ষর	*exp;

	p = strchr(var + 1, '%');
	যদি (p != ফাঁকা)
	{
	    strncpy(buf, var + 1, p - (var + 1));
	    buf[p - (var + 1)] = NUL;
	    exp = getenv(buf);
	    যদি (exp != ফাঁকা && *exp != NUL
				&& strlen(exp) + strlen(p) < আকার(buf))
	    {
		snprintf(buf, আকার(buf), "%s%s", exp, p + 1);
		var = buf;
	    }
	}
    }

    যদি (var != ফাঁকা && *var == NUL)	// empty is same as not set
	var = ফাঁকা;

    যদি (var == ফাঁকা)
	homedir = ফাঁকা;
    অন্যথা
	homedir = _strdup(var);
}

/*
 * Change the directory that the vim plugin directories will be created in:
 * $HOME, $VIM or nowhere.
 */
    স্থির শূন্য
change_directories_choice(পূর্ণসংখ্যা idx)
{
    পূর্ণসংখ্যা	    choice_গুনতি = TABLE_আকার(vimfiles_dir_choices);

    // Don't offer the $HOME choice if $HOME isn't set.
    যদি (homedir == ফাঁকা)
	--choice_গুনতি;
    choices[idx].arg = get_choice(vimfiles_dir_choices, choice_গুনতি);
    set_directories_text(idx);
}

/*
 * Create the plugin directories...
 */
//ARGSUSED
    স্থির শূন্য
install_vimfilesdir(পূর্ণসংখ্যা idx)
{
    পূর্ণসংখ্যা i;
    পূর্ণসংখ্যা vimfiles_dir_choice = choices[idx].arg;
    অক্ষর *p;
    অক্ষর vimdir_path[MAX_PATH];
    অক্ষর vimfiles_path[MAX_PATH + 9];
    অক্ষর tmp_dirname[BUFSIZE];

    // switch on the location that the user wants the plugin directories
    // built in
    নির্বাচন (vimfiles_dir_choice)
    {
	ক্ষেত্র vimfiles_dir_vim:
	{
	    // Go to the %VIM% directory - check env first, then go one dir
	    //	   below installdir if there is no %VIM% environment variable.
	    //	   The accuracy of $VIM is checked in inspect_system(), so we
	    //	   can be sure it is ok to use here.
	    p = getenv("VIM");
	    যদি (p == ফাঁকা) // No $VIM in path
		dir_remove_last(installdir, vimdir_path);
	    অন্যথা
		strcpy(vimdir_path, p);
	    বিরতি;
	}
	ক্ষেত্র vimfiles_dir_home:
	{
	    // Find the $HOME directory.  Its existence was already checked.
	    p = homedir;
	    যদি (p == ফাঁকা)
	    {
		printf("Internal error: $HOME is NULL\n");
		p = "c:\\";
	    }
	    strcpy(vimdir_path, p);
	    বিরতি;
	}
	ক্ষেত্র vimfiles_dir_none:
	{
	    // Do not create vim plugin directory.
	    ফেরত;
	}
    }

    // Now, just create the directory.	If it already exists, it will fail
    // silently.
    sprintf(vimfiles_path, "%s\\vimfiles", vimdir_path);
    vim_mkdir(vimfiles_path, 0755);

    printf("Creating the following directories in \"%s\":\n", vimfiles_path);
    জন্য (i = 0; i < TABLE_আকার(vimfiles_subdirs); i++)
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
    স্থির শূন্য
init_directories_choice(শূন্য)
{
    কাঠামো stat	st;
    অক্ষর	tmp_dirname[BUFSIZE];
    অক্ষর	*p;
    পূর্ণসংখ্যা		vimfiles_dir_choice;

    choices[choice_গুনতি].text = alloc(150);
    choices[choice_গুনতি].changefunc = change_directories_choice;
    choices[choice_গুনতি].installfunc = install_vimfilesdir;
    choices[choice_গুনতি].active = 1;

    // Check if the "compiler" directory already exists.  That's a good
    // indication that the plugin directories were already created.
    p = getenv("HOME");
    যদি (p != ফাঁকা)
    {
	vimfiles_dir_choice = (পূর্ণসংখ্যা)vimfiles_dir_home;
	sprintf(tmp_dirname, "%s\\vimfiles\\compiler", p);
	যদি (stat(tmp_dirname, &st) == 0)
	    vimfiles_dir_choice = (পূর্ণসংখ্যা)vimfiles_dir_none;
    }
    অন্যথা
    {
	vimfiles_dir_choice = (পূর্ণসংখ্যা)vimfiles_dir_vim;
	p = getenv("VIM");
	যদি (p == ফাঁকা)  // No $VIM in path, use the install dir.
	    dir_remove_last(installdir, tmp_dirname);
	অন্যথা
	    strcpy(tmp_dirname, p);
	strcat(tmp_dirname, "\\vimfiles\\compiler");
	যদি (stat(tmp_dirname, &st) == 0)
	    vimfiles_dir_choice = (পূর্ণসংখ্যা)vimfiles_dir_none;
    }

    choices[choice_গুনতি].arg = vimfiles_dir_choice;
    set_directories_text(choice_গুনতি);
    ++choice_গুনতি;
}

/*
 * Setup the choices and the default values.
 */
    স্থির শূন্য
setup_choices(শূন্য)
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

    স্থির শূন্য
print_cmd_সারি_help(শূন্য)
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
    স্থির শূন্য
command_সারি_setup_choices(পূর্ণসংখ্যা argc, অক্ষর **argv)
{
    পূর্ণসংখ্যা i, j;

    জন্য (i = 1; i < argc; i++)
    {
	যদি (strcmp(argv[i], "-create-batfiles") == 0)
	{
	    যদি (i + 1 == argc)
		চালু;
	    যখন (argv[i + 1][0] != '-' && i < argc)
	    {
		i++;
		জন্য (j = 1; j < TARGET_গুনতি; ++j)
		    যদি ((targets[j].exenamearg[0] == 'g' ? has_gvim : has_vim)
			    && strcmp(argv[i], targets[j].name) == 0)
		    {
			init_bat_choice(j);
			বিরতি;
		    }
		যদি (j == TARGET_গুনতি)
		    printf("%s is not a valid choice for -create-batfiles\n",
								     argv[i]);

		যদি (i + 1 == argc)
		    বিরতি;
	    }
	}
	অন্যথা যদি (strcmp(argv[i], "-create-vimrc") == 0)
	{
	    // Setup default vimrc choices.  If there is already a _vimrc file,
	    // it will NOT be overwritten.
	    init_vimrc_choices();
	}
	অন্যথা যদি (strcmp(argv[i], "-vimrc-remap") == 0)
	{
	    যদি (i + 1 == argc)
		বিরতি;
	    i++;
	    যদি (strcmp(argv[i], "no") == 0)
		remap_choice = remap_no;
	    অন্যথা যদি (strcmp(argv[i], "win") == 0)
		remap_choice = remap_win;
	}
	অন্যথা যদি (strcmp(argv[i], "-vimrc-behave") == 0)
	{
	    যদি (i + 1 == argc)
		বিরতি;
	    i++;
	    যদি (strcmp(argv[i], "unix") == 0)
		mouse_choice = mouse_xterm;
	    অন্যথা যদি (strcmp(argv[i], "mswin") == 0)
		mouse_choice = mouse_mswin;
	    অন্যথা যদি (strcmp(argv[i], "default") == 0)
		mouse_choice = mouse_default;
	}
	অন্যথা যদি (strcmp(argv[i], "-vimrc-compat") == 0)
	{
	    যদি (i + 1 == argc)
		বিরতি;
	    i++;
	    যদি (strcmp(argv[i], "vi") == 0)
		compat_choice = compat_vi;
	    অন্যথা যদি (strcmp(argv[i], "vim") == 0)
		compat_choice = compat_vim;
	    অন্যথা যদি (strcmp(argv[i], "defaults") == 0)
		compat_choice = compat_some_enhancements;
	    অন্যথা যদি (strcmp(argv[i], "all") == 0)
		compat_choice = compat_all_enhancements;
	}
	অন্যথা যদি (strcmp(argv[i], "-install-popup") == 0)
	{
	    init_popup_choice();
	}
	অন্যথা যদি (strcmp(argv[i], "-install-openwith") == 0)
	{
	    init_openwith_choice();
	}
	অন্যথা যদি (strcmp(argv[i], "-add-start-menu") == 0)
	{
	    init_startmenu_choice();
	}
	অন্যথা যদি (strcmp(argv[i], "-install-icons") == 0)
	{
	    init_shortcut_choices();
	}
	অন্যথা যদি (strcmp(argv[i], "-create-directories") == 0)
	{
	    পূর্ণসংখ্যা vimfiles_dir_choice = (পূর্ণসংখ্যা)vimfiles_dir_none;

	    init_directories_choice();
	    যদি (i + 1 < argc && argv[i + 1][0] != '-')
	    {
		i++;
		যদি (strcmp(argv[i], "vim") == 0)
		    vimfiles_dir_choice = (পূর্ণসংখ্যা)vimfiles_dir_vim;
		অন্যথা যদি (strcmp(argv[i], "home") == 0)
		{
		    যদি (homedir == ফাঁকা)  // No $HOME in environment
			vimfiles_dir_choice = (পূর্ণসংখ্যা)vimfiles_dir_none;
		    অন্যথা
			vimfiles_dir_choice = (পূর্ণসংখ্যা)vimfiles_dir_home;
		}
		অন্যথা
		{
		    printf("Unknown argument for -create-directories: %s\n",
								     argv[i]);
		    print_cmd_সারি_help();
		}
	    }
	    অন্যথা // No choice specified, default to vim directory
		vimfiles_dir_choice = (পূর্ণসংখ্যা)vimfiles_dir_vim;
	    choices[choice_গুনতি - 1].arg = vimfiles_dir_choice;
	}
	অন্যথা যদি (strcmp(argv[i], "-register-OLE") == 0)
	{
	    // This is always done when gvim is found
	}
	অন্যথা // Unknown switch
	{
	    printf("Got unknown argument argv[%d] = %s\n", i, argv[i]);
	    print_cmd_সারি_help();
	}
    }
}


/*
 * Show a few screens full of helpful information.
 */
    স্থির শূন্য
show_help(শূন্য)
{
    স্থির অক্ষর *(বস্তুসমূহ[]) =
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
ফাঁকা
    };
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		c;

    rewind(stdin);
    printf("\n");
    জন্য (i = 0; বস্তুসমূহ[i] != ফাঁকা; ++i)
    {
	puts(বস্তুসমূহ[i]);
	printf("Hit Enter to continue, b (back) or q (quit help): ");
	c = getchar();
	rewind(stdin);
	যদি (c == 'b' || c == 'B')
	{
	    যদি (i == 0)
		--i;
	    অন্যথা
		i -= 2;
	}
	যদি (c == 'q' || c == 'Q')
	    বিরতি;
	printf("\n");
    }
}

/*
 * Install the choices.
 */
    স্থির শূন্য
install(শূন্য)
{
    পূর্ণসংখ্যা		i;

    // Install the selected choices.
    জন্য (i = 0; i < choice_গুনতি; ++i)
	যদি (choices[i].installfunc != ফাঁকা && choices[i].active)
	    (choices[i].installfunc)(i);

    // Add some entries to the registry, if needed.
    যদি (install_popup
	    || install_openwith
	    || (need_uninstall_entry && interactive)
	    || !interactive)
	install_registry();

    // Register gvim with OLE.
    যদি (has_gvim)
	install_OLE_register();
}

/*
 * request_choice
 */
    স্থির শূন্য
request_choice(শূন্য)
{
    পূর্ণসংখ্যা		      i;

    printf("\n\nInstall will do for you:\n");
    জন্য (i = 0; i < choice_গুনতি; ++i)
	যদি (choices[i].active)
	    printf("%2d  %s\n", i + 1, choices[i].text);
    printf("To change an item, enter its number\n\n");
    printf("Enter item number, h (help), d (do it) or q (quit): ");
}

    পূর্ণসংখ্যা
প্রধান(পূর্ণসংখ্যা argc, অক্ষর **argv)
{
    পূর্ণসংখ্যা		i;
    অক্ষর	buf[BUFSIZE];

    /*
     * Run interactively if there are no command line arguments.
     */
    যদি (argc > 1)
	interactive = 0;
    অন্যথা
	interactive = 1;

    // Initialize this program.
    do_inits(argv);
    init_homedir();

    যদি (argc > 1 && strcmp(argv[1], "-uninstall-check") == 0)
    {
	// Only check for already installed Vims.  Used by NSIS installer.
	i = uninstall_check(1);

	// Find the value of $VIM, because NSIS isn't able to do this by
	// itself.
	get_vim_env();

	// When nothing found exit quietly.  If something found wait for
	// a little while, so that the user can read the messages.
	যদি (i && _isatty(1))
	    sleep(3);
	exit(0);
    }

    printf("This program sets up the installation of Vim "
						   VIM_VERSION_MEDIUM "\n\n");

    // Check if the user unpacked the archives properly.
    check_unpack();

    // Check for already installed Vims.
    যদি (interactive)
	uninstall_check(0);

    // Find out information about the system.
    inspect_system();

    যদি (interactive)
    {
	// Setup all the choices.
	setup_choices();

	// Let the user change choices and finally install (or quit).
	জন্য (;;)
	{
	    request_choice();
	    rewind(stdin);
	    যদি (scanf("%99s", buf) == 1)
	    {
		যদি (isdigit((unsigned অক্ষর)buf[0]))
		{
		    // Change a choice.
		    i = atoi(buf);
		    যদি (i > 0 && i <= choice_গুনতি && choices[i - 1].active)
			(choices[i - 1].changefunc)(i - 1);
		    অন্যথা
			printf("\nIllegal choice\n");
		}
		অন্যথা যদি (buf[0] == 'h' || buf[0] == 'H')
		{
		    // Help
		    show_help();
		}
		অন্যথা যদি (buf[0] == 'd' || buf[0] == 'D')
		{
		    // Install!
		    install();
		    printf("\nThat finishes the installation.  Happy Vimming!\n");
		    বিরতি;
		}
		অন্যথা যদি (buf[0] == 'q' || buf[0] == 'Q')
		{
		    // Quit
		    printf("\nExiting without anything done\n");
		    বিরতি;
		}
		অন্যথা
		    printf("\nIllegal choice\n");
	    }
	}
	printf("\n");
	myexit(0);
    }
    অন্যথা
    {
	/*
	 * Run non-interactive - setup according to the command line switches
	 */
	command_সারি_setup_choices(argc, argv);
	install();

	// Avoid that the user has to hit Enter, just wait a little bit to
	// allow reading the messages.
	sleep(2);
    }

    ফেরত 0;
}
