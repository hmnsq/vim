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
#شامل "dosinst.h"
#شامل <io.h>

#define GVIMEXT64_PATH	    "GvimExt64\\gvimext.dll"
#define GVIMEXT32_PATH	    "GvimExt32\\gvimext.dll"

// Macro to do an error check I was typing over and over
#define CHECK_REG_ERROR(code) \
    انجامبده { \
	اگر (code != ERROR_SUCCESS) \
	{ \
	    printf("%ld error number:  %ld\n", (long)__LINE__, (long)code); \
	    بازگشت 1; \
	} \
    } وقتی (0)

صحیح	has_vim = 0;		// installable vim.exe exists
صحیح	has_gvim = 0;		// installable gvim.exe exists

نویسه	oldvimrc[BUFSIZE];	// name of existing vimrc file
نویسه	vimrc[BUFSIZE];		// name of vimrc file to create

نویسه	*default_bat_dir = پوچ;  // when not NULL, use this as the default
				  // directory to write .bat files in
نویسه	*default_vim_dir = پوچ;  // when not NULL, use this as the default
				  // install dir for NSIS

/*
 * Structure used for each choice the user can make.
 */
ساختار choice
{
    صحیح	    active;			// non-zero when choice is active
    نویسه    *text;			// text displayed for this choice
    تهی    (*changefunc)(صحیح idx);	// function to change this choice
    صحیح	    arg;			// argument for function
    تهی    (*installfunc)(صحیح idx);	// function to install this choice
};

ساختار choice	choices[30];		// choices the user can make
صحیح		choice_شمار = 0;	// number of choices available

#define TABLE_SIZE(s)	(صحیح)ARRAYSIZE(s)

شمارش
{
    compat_vi = 1,
    compat_vim,
    compat_some_enhancements,
    compat_all_enhancements
};
نویسه	*(compat_choices[]) =
{
    "\nChoose the default way to run Vim:",
    "Vi compatible",
    "Vim default",
    "with some Vim enhancements",
    "with syntax highlighting and other features switched on",
};
صحیح	compat_choice = (صحیح)compat_all_enhancements;
نویسه	*compat_text = "- run Vim %s";

شمارش
{
    remap_no = 1,
    remap_win
};
نویسه	*(remap_choices[]) =
{
    "\nChoose:",
    "Do not remap keys for Windows behavior",
    "Remap a few keys for Windows behavior (CTRL-V, CTRL-C, CTRL-F, etc)",
};
صحیح	remap_choice = (صحیح)remap_no;
نویسه	*remap_text = "- %s";

شمارش
{
    mouse_xterm = 1,
    mouse_mswin,
    mouse_default
};
نویسه	*(mouse_choices[]) =
{
    "\nChoose the way how Vim uses the mouse:",
    "right button extends selection (the Unix way)",
    "right button has a popup menu, left button starts select mode (the Windows way)",
    "right button has a popup menu, left button starts visual mode",
};
صحیح	mouse_choice = (صحیح)mouse_default;
نویسه	*mouse_text = "- The mouse %s";

شمارش
{
    vimfiles_dir_none = 1,
    vimfiles_dir_vim,
    vimfiles_dir_home
};
ایستا نویسه *(vimfiles_dir_choices[]) =
{
    "\nCreate plugin directories:",
    "No",
    "In the VIM directory",
    "In your HOME directory",
};

// non-zero when selected to install the popup menu entry.
ایستا صحیح	install_popup = 0;

// non-zero when selected to install the "Open with" entry.
ایستا صحیح	install_openwith = 0;

// non-zero when need to add an uninstall entry in the registry
ایستا صحیح	need_uninstall_entry = 0;

/*
 * Definitions of the directory name (under $VIM) of the vimfiles directory
 * and its subdirectories:
 */
ایستا نویسه	*(vimfiles_subdirs[]) =
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
    ایستا صحیح
get_choice(نویسه **table, صحیح entries)
{
    صحیح		answer;
    صحیح		idx;
    نویسه	dummy[100];

    انجامبده
    {
	برای (idx = 0; idx < entries; ++idx)
	{
	    اگر (idx)
		printf("%2d  ", idx);
	    puts(table[idx]);
	}
	printf("Choice: ");
	اگر (scanf("%d", &answer) != 1)
	{
	    scanf("%99s", dummy);
	    answer = 0;
	}
    }
    وقتی (answer < 1 || answer >= entries);

    بازگشت answer;
}

/*
 * Check if the user unpacked the archives properly.
 * Sets "runtimeidx".
 */
    ایستا تهی
check_unpack(تهی)
{
    نویسه	buf[BUFSIZE];
    FILE	*fd;
    ساختار stat	st;

    // check for presence of the correct version number in installdir[]
    runtimeidx = strlen(installdir) - strlen(VIM_VERSION_NODOT);
    اگر (runtimeidx <= 0
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
    اگر (stat(buf, &st) < 0)
    {
	printf("ERROR: Cannot find filetype.vim in \"%s\"\n", installdir);
	printf("It looks like you did not unpack the runtime archive.\n");
	printf("You must unpack the runtime archive \"%srt.zip\" before installing.\n",
		VIM_VERSION_NODOT);
	myexit(1);
    }

    // Check if vim.exe or gvim.exe is in the current directory.
    اگر ((fd = fopen("gvim.exe", "r")) != پوچ)
    {
	fclose(fd);
	has_gvim = 1;
    }
    اگر ((fd = fopen("vim.exe", "r")) != پوچ)
    {
	fclose(fd);
	has_vim = 1;
    }
    اگر (!has_gvim && !has_vim)
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
    ایستا صحیح
pathcmp(نویسه *p, صحیح plen, نویسه *q, صحیح qlen)
{
    صحیح		i;

    اگر (plen < 0)
	plen = strlen(p);
    اگر (qlen < 0)
	qlen = strlen(q);
    برای (i = 0; ; ++i)
    {
	// End of "p": check if "q" also ends or just has a slash.
	اگر (i == plen)
	{
	    اگر (i == qlen)  // match
		بازگشت 0;
	    اگر (i == qlen - 1 && (q[i] == '\\' || q[i] == '/'))
		بازگشت 0;   // match with trailing slash
	    بازگشت 1;	    // no match
	}

	// End of "q": check if "p" also ends or just has a slash.
	اگر (i == qlen)
	{
	    اگر (i == plen)  // match
		بازگشت 0;
	    اگر (i == plen - 1 && (p[i] == '\\' || p[i] == '/'))
		بازگشت 0;   // match with trailing slash
	    بازگشت 1;	    // no match
	}

	اگر (!(mytoupper(p[i]) == mytoupper(q[i])
		|| ((p[i] == '/' || p[i] == '\\')
		    && (q[i] == '/' || q[i] == '\\'))))
	    بازگشت 1;	    // no match
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
    ایستا تهی
findoldfile(نویسه **destination)
{
    نویسه	*bp = *destination;
    size_t	indir_l = strlen(installdir);
    نویسه	*cp;
    نویسه	*tmpname;
    نویسه	*farname;

    /*
     * No action needed if exe not found or not in this directory.
     */
    اگر (bp == پوچ || strnicmp(bp, installdir, indir_l) != 0)
	بازگشت;
    cp = bp + indir_l;
    اگر (strchr("/\\", *cp++) == پوچ
	    || strchr(cp, '\\') != پوچ
	    || strchr(cp, '/') != پوچ)
	بازگشت;

    tmpname = alloc(strlen(cp) + 1);
    strcpy(tmpname, cp);
    tmpname[strlen(tmpname) - 1] = 'x';	// .exe -> .exx

    اگر (access(tmpname, 0) == 0)
    {
	printf("\nERROR: %s and %s clash.  Remove or rename %s.\n",
	    tmpname, cp, tmpname);
	myexit(1);
    }

    اگر (rename(cp, tmpname) != 0)
    {
	printf("\nERROR: failed to rename %s to %s: %s\n",
	    cp, tmpname, strerror(0));
	myexit(1);
    }

    farname = searchpath_save(cp);

    اگر (rename(tmpname, cp) != 0)
    {
	printf("\nERROR: failed to rename %s back to %s: %s\n",
	    tmpname, cp, strerror(0));
	myexit(1);
    }

    آزاد(*destination);
    آزاد(tmpname);
    *destination = farname;
}

/*
 * Check if there is a vim.[exe|bat|, gvim.[exe|bat|, etc. in the path.
 * When "check_bat_only" is TRUE, only find "default_bat_dir".
 */
    ایستا تهی
find_bat_exe(صحیح check_bat_only)
{
    صحیح		i;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    برای (i = 1; i < TARGET_شمار; ++i)
    {
	targets[i].oldbat = searchpath_save(targets[i].batname);
	اگر (!check_bat_only)
	    targets[i].oldexe = searchpath_save(targets[i].exename);

	اگر (default_bat_dir == پوچ && targets[i].oldbat != پوچ)
	{
	    default_bat_dir = alloc(strlen(targets[i].oldbat) + 1);
	    strcpy(default_bat_dir, targets[i].oldbat);
	    remove_tail(default_bat_dir);
	}
	اگر (check_bat_only && targets[i].oldbat != پوچ)
	{
	    آزاد(targets[i].oldbat);
	    targets[i].oldbat = پوچ;
	}
    }

    mch_chdir(installdir);
}

/*
 * Get the value of $VIMRUNTIME or $VIM and write it in $TEMP/vimini.ini, so
 * that NSIS can read it.
 * When not set, use the directory of a previously installed Vim.
 */
    ایستا تهی
get_vim_env(تهی)
{
    نویسه	*vim;
    نویسه	buf[BUFSIZE];
    FILE	*fd;
    نویسه	fname[BUFSIZE];

    // First get $VIMRUNTIME.  If it's set, remove the tail.
    vim = getenv("VIMRUNTIME");
    اگر (vim != پوچ && *vim != 0 && strlen(vim) < اندازه(buf))
    {
	strcpy(buf, vim);
	remove_tail(buf);
	vim = buf;
    }
    وگرنه
    {
	vim = getenv("VIM");
	اگر (vim == پوچ || *vim == 0)
	{
	    // Use the directory from an old uninstall entry.
	    اگر (default_vim_dir != پوچ)
		vim = default_vim_dir;
	    وگرنه
		// Let NSIS know there is no default, it should use
		// $PROGRAMFILES.
		vim = "";
	}
    }

    // NSIS also uses GetTempPath(), thus we should get the same directory
    // name as where NSIS will look for vimini.ini.
    GetTempPath(اندازه(fname) - 12, fname);
    add_pathsep(fname);
    strcat(fname, "vimini.ini");

    fd = fopen(fname, "w");
    اگر (fd != پوچ)
    {
	// Make it look like an .ini file, so that NSIS can read it with a
	// ReadINIStr command.
	fprintf(fd, "[vimini]\n");
	fprintf(fd, "dir=\"%s\"\n", vim);
	fclose(fd);
    }
    وگرنه
    {
	printf("Failed to open %s\n", fname);
	sleep(2);
    }
}

ایستا صحیح رقم_windows;

/*
 * Callback used for EnumWindows():
 * Count the window if the title looks like it is for the uninstaller.
 */
//ARGSUSED
    ایستا BOOL CALLBACK
window_cb(HWND hwnd, LPARAM lparam UNUSED)
{
    نویسه title[256];

    title[0] = 0;
    GetWindowText(hwnd, title, 256);
    اگر (strstr(title, "Vim ") != پوچ && strstr(title, " Uninstall") != پوچ)
	++رقم_windows;
    بازگشت TRUE;
}

/*
 * Run the uninstaller silently.
 */
    ایستا صحیح
run_silent_uninstall(نویسه *uninst_exe)
{
    نویسه    vimrt_dir[BUFSIZE];
    نویسه    temp_uninst[BUFSIZE];
    نویسه    temp_dir[MAX_PATH];
    نویسه    buf[BUFSIZE * 2 + 10];
    صحیح	    i;
    DWORD   tick;

    strcpy(vimrt_dir, uninst_exe);
    remove_tail(vimrt_dir);

    اگر (!GetTempPath(اندازه(temp_dir), temp_dir))
	بازگشت FAIL;

    // Copy the uninstaller to a temporary exe.
    tick = Get_Tick_شمار();
    برای (i = 0; ; i++)
    {
	sprintf(temp_uninst, "%s\\vimun%04X.exe", temp_dir,
					  (unsigned صحیح)((i + tick) & 0xFFFF));
	اگر (CopyFile(uninst_exe, temp_uninst, TRUE))
	    بشکن;
	اگر (GetLastError() != ERROR_FILE_EXISTS)
	    بازگشت FAIL;
	اگر (i == 65535)
	    بازگشت FAIL;
    }

    // Run the copied uninstaller silently.
    اگر (strchr(temp_uninst, ' ') != پوچ)
	sprintf(buf, "\"%s\" /S _?=%s", temp_uninst, vimrt_dir);
    وگرنه
	sprintf(buf, "%s /S _?=%s", temp_uninst, vimrt_dir);
    run_command(buf);

    DeleteFile(temp_uninst);
    بازگشت OK;
}

/*
 * Check for already installed Vims.
 * Return non-zero when found one.
 */
    ایستا صحیح
uninstall_check(صحیح skip_question)
{
    HKEY	کلید_handle;
    HKEY	uninstall_کلید_handle;
    نویسه	*uninstall_کلید = "software\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
    نویسه	subkey_name_buff[BUFSIZE];
    نویسه	temp_string_buffer[BUFSIZE-2];
    DWORD	local_bufsize;
    FILETIME	temp_pfiletime;
    DWORD	کلید_index;
    نویسه	درون_داد;
    long	code;
    DWORD	مقدار_type;
    DWORD	orig_رقم_کلیدها;
    DWORD	new_رقم_کلیدها;
    DWORD	allow_silent;
    صحیح		foundone = 0;

    code = Reg_Open_کلید_Ex(HKEY_LOCAL_MACHINE, uninstall_کلید, 0,
				     کلید_WOW64_64_کلید | کلید_READ, &کلید_handle);
    CHECK_REG_ERROR(code);

    کلید_index = 0;
    وقتی (TRUE)
    {
	local_bufsize = اندازه(subkey_name_buff);
	اگر (Reg_Enum_کلید_Ex(کلید_handle, کلید_index, subkey_name_buff, &local_bufsize,
		پوچ, پوچ, پوچ, &temp_pfiletime) == ERROR_NO_MORE_اقلام)
	    بشکن;

	اگر (strncmp("Vim", subkey_name_buff, 3) == 0)
	{
	    // Open the key named Vim*
	    code = Reg_Open_کلید_Ex(کلید_handle, subkey_name_buff, 0,
			   کلید_WOW64_64_کلید | کلید_READ, &uninstall_کلید_handle);
	    CHECK_REG_ERROR(code);

	    // get the DisplayName out of it to show the user
	    local_bufsize = اندازه(temp_string_buffer);
	    code = Reg_Query_مقدار_Ex(uninstall_کلید_handle, "displayname", 0,
		    &مقدار_type, (LPBYTE)temp_string_buffer,
		    &local_bufsize);
	    CHECK_REG_ERROR(code);

	    allow_silent = 0;
	    اگر (skip_question)
	    {
		DWORD varsize = اندازه(DWORD);

		Reg_Query_مقدار_Ex(uninstall_کلید_handle, "AllowSilent", 0,
			&مقدار_type, (LPBYTE)&allow_silent,
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

	    اگر (skip_question)
		printf("\nRunning uninstall program for \"%s\"\n", temp_string_buffer);
	    وگرنه
		printf("\nDo you want to uninstall \"%s\" now?\n(y)es/(n)o)  ", temp_string_buffer);
	    fflush(stdout);

	    // get the UninstallString
	    local_bufsize = اندازه(temp_string_buffer);
	    code = Reg_Query_مقدار_Ex(uninstall_کلید_handle, "uninstallstring", 0,
		    &مقدار_type, (LPBYTE)temp_string_buffer, &local_bufsize);
	    CHECK_REG_ERROR(code);

	    // Remember the directory, it is used as the default for NSIS.
	    default_vim_dir = alloc(strlen(temp_string_buffer) + 1);
	    strcpy(default_vim_dir, temp_string_buffer);
	    remove_tail(default_vim_dir);
	    remove_tail(default_vim_dir);

	    درون_داد = 'n';
	    انجامبده
	    {
		اگر (درون_داد != 'n')
		    printf("%c is an invalid reply.  Please enter either 'y' or 'n'\n", درون_داد);

		اگر (skip_question)
		    درون_داد = 'y';
		وگرنه
		{
		    rewind(stdin);
		    scanf("%c", &درون_داد);
		}
		انتخاب (درون_داد)
		{
		    حالت 'y':
		    حالت 'Y':
			// save the number of uninstall keys so we can know if
			// it changed
			Reg_Query_Info_کلید(کلید_handle, پوچ, پوچ, پوچ,
					     &orig_رقم_کلیدها, پوچ, پوچ, پوچ,
						      پوچ, پوچ, پوچ, پوچ);

			// Find existing .bat files before deleting them.
			find_bat_exe(TRUE);

			اگر (allow_silent)
			{
			    اگر (run_silent_uninstall(temp_string_buffer)
								    == FAIL)
				allow_silent = 0; // Retry with non silent.
			}
			اگر (!allow_silent)
			{
			    // Execute the uninstall program.  Put it in double
			    // quotes if there is an embedded space.
			    {
				نویسه buf[BUFSIZE];

				اگر (strchr(temp_string_buffer, ' ') != پوچ)
				    sprintf(buf, "\"%s\"", temp_string_buffer);
				وگرنه
				    strcpy(buf, temp_string_buffer);
				run_command(buf);
			    }

			    // Count the number of windows with a title that
			    // match the installer, so that we can check when
			    // it's done.  The uninstaller copies itself,
			    // executes the copy and exits, thus we can't wait
			    // for the process to finish.
			    sleep(1);  // wait for uninstaller to start up
			    رقم_windows = 0;
			    EnumWindows(window_cb, 0);
			    اگر (رقم_windows == 0)
			    {
				// Did not find the uninstaller, ask user to
				// press Enter when done. Just in case.
				printf("Press Enter when the uninstaller is finished\n");
				rewind(stdin);
				(تهی)getchar();
			    }
			    وگرنه
			    {
				printf("Waiting for the uninstaller to finish (press CTRL-C to abort).");
				انجامبده
				{
				    printf(".");
				    fflush(stdout);
				    sleep(1);	// wait for the uninstaller to
						// finish
				    رقم_windows = 0;
				    EnumWindows(window_cb, 0);
				} وقتی (رقم_windows > 0);
			    }
			}
			printf("\nDone!\n");

			// Check if an uninstall reg key was deleted.
			// if it was, we want to decrement key_index.
			// if we don't do this, we will skip the key
			// immediately after any key that we delete.
			Reg_Query_Info_کلید(کلید_handle, پوچ, پوچ, پوچ,
					      &new_رقم_کلیدها, پوچ, پوچ, پوچ,
						      پوچ, پوچ, پوچ, پوچ);
			اگر (new_رقم_کلیدها < orig_رقم_کلیدها)
			    کلید_index--;

			درون_داد = 'y';
			بشکن;

		    حالت 'n':
		    حالت 'N':
			// Do not uninstall
			درون_داد = 'n';
			بشکن;

		    پیشفرض: // just drop through and redo the loop
			بشکن;
		}

	    } وقتی (درون_داد != 'n' && درون_داد != 'y');

	    Reg_Close_کلید(uninstall_کلید_handle);
	}

	کلید_index++;
    }
    Reg_Close_کلید(کلید_handle);

    بازگشت foundone;
}

/*
 * Find out information about the system.
 */
    ایستا تهی
inspect_system(تهی)
{
    نویسه	*p;
    نویسه	buf[BUFSIZE];
    FILE	*fd;
    صحیح		i;
    صحیح		foundone;

    // This may take a little while, let the user know what we're doing.
    printf("Inspecting system...\n");

    /*
     * If $VIM is set, check that it's pointing to our directory.
     */
    p = getenv("VIM");
    اگر (p != پوچ && pathcmp(p, -1, installdir, runtimeidx - 1) != 0)
    {
	printf("------------------------------------------------------\n");
	printf("$VIM is set to \"%s\".\n", p);
	printf("This is different from where this version of Vim is:\n");
	strcpy(buf, installdir);
	*(buf + runtimeidx - 1) = NUL;
	printf("\"%s\"\n", buf);
	printf("You must adjust or remove the setting of $VIM,\n");
	اگر (interactive)
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
    اگر (p != پوچ && pathcmp(p, -1, installdir, -1) != 0)
    {
	printf("------------------------------------------------------\n");
	printf("$VIMRUNTIME is set to \"%s\".\n", p);
	printf("This is different from where this version of Vim is:\n");
	printf("\"%s\"\n", installdir);
	printf("You must adjust or remove the setting of $VIMRUNTIME,\n");
	اگر (interactive)
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
    برای (i = 1; i < TARGET_شمار; ++i)
    {
	findoldfile(&(targets[i].oldexe));
	اگر (targets[i].oldexe != پوچ)
	    foundone = 1;
    }

    اگر (foundone)
    {
	printf("Warning: Found Vim executable(s) in your $PATH:\n");
	برای (i = 1; i < TARGET_شمار; ++i)
	    اگر (targets[i].oldexe != پوچ)
		printf("%s\n", targets[i].oldexe);
	printf("It will be used instead of the version you are installing.\n");
	printf("Please delete or rename it, or adjust your $PATH setting.\n");
    }

    /*
     * Check if there is an existing ../_vimrc or ../.vimrc file.
     */
    strcpy(oldvimrc, installdir);
    strcpy(oldvimrc + runtimeidx, "_vimrc");
    اگر ((fd = fopen(oldvimrc, "r")) == پوچ)
    {
	strcpy(oldvimrc + runtimeidx, "vimrc~1"); // short version of .vimrc
	اگر ((fd = fopen(oldvimrc, "r")) == پوچ)
	{
	    strcpy(oldvimrc + runtimeidx, ".vimrc");
	    fd = fopen(oldvimrc, "r");
	}
    }
    اگر (fd != پوچ)
	fclose(fd);
    وگرنه
	*oldvimrc = NUL;
}

/*
 * Add a dummy choice to avoid that the numbering changes depending on items
 * in the environment.  The user may type a number he remembered without
 * looking.
 */
    ایستا تهی
add_dummy_choice(تهی)
{
    choices[choice_شمار].installfunc = پوچ;
    choices[choice_شمار].active = 0;
    choices[choice_شمار].changefunc = پوچ;
    choices[choice_شمار].text = پوچ;
    choices[choice_شمار].arg = 0;
    ++choice_شمار;
}

////////////////////////////////////////////////
// stuff for creating the batch files.

/*
 * Install the vim.bat, gvim.bat, etc. files.
 */
    ایستا تهی
install_bat_choice(صحیح idx)
{
    نویسه	*batpath = targets[choices[idx].arg].batpath;
    نویسه	*oldname = targets[choices[idx].arg].oldbat;
    نویسه	*exename = targets[choices[idx].arg].exenamearg;
    نویسه	*vimarg = targets[choices[idx].arg].exearg;
    FILE	*fd;

    اگر (*batpath == NUL)
	بازگشت;

    fd = fopen(batpath, "w");
    اگر (fd == پوچ)
    {
	printf("\nERROR: Cannot open \"%s\" for writing.\n", batpath);
	بازگشت;
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

    اگر (*exename == 'g')
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

    اگر (*exename == 'g')
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
    وگرنه
    {
	// Always use quotes, $VIM or $VIMRUNTIME might have a space.
	fprintf(fd, "\"%%VIM_EXE_DIR%%\\%s\" %s %%*\n",
		exename, vimarg);
    }

    fclose(fd);
    printf("%s has been %s\n", batpath,
	    oldname == پوچ ? "created" : "overwritten");
}

/*
 * Make the text string for choice "idx".
 * The format "fmt" is must have one %s item, which "arg" is used for.
 */
    ایستا تهی
alloc_text(صحیح idx, نویسه *fmt, نویسه *arg)
{
    اگر (choices[idx].text != پوچ)
	آزاد(choices[idx].text);

    choices[idx].text = alloc(strlen(fmt) + strlen(arg) - 1);
    sprintf(choices[idx].text, fmt, arg);
}

/*
 * Toggle the "Overwrite .../vim.bat" to "Don't overwrite".
 */
    ایستا تهی
toggle_bat_choice(صحیح idx)
{
    نویسه	*batname = targets[choices[idx].arg].batpath;
    نویسه	*oldname = targets[choices[idx].arg].oldbat;

    اگر (*batname == NUL)
    {
	alloc_text(idx, "    Overwrite %s", oldname);
	strcpy(batname, oldname);
    }
    وگرنه
    {
	alloc_text(idx, "    Do NOT overwrite %s", oldname);
	*batname = NUL;
    }
}

/*
 * Do some work for a batch file entry: Append the batch file name to the path
 * and set the text for the choice.
 */
    ایستا تهی
set_bat_text(صحیح idx, نویسه *batpath, نویسه *name)
{
    strcat(batpath, name);

    alloc_text(idx, "    Create %s", batpath);
}

/*
 * Select a directory to write the batch file line.
 */
    ایستا تهی
change_bat_choice(صحیح idx)
{
    نویسه	*path;
    نویسه	*batpath;
    نویسه	*name;
    صحیح		n;
    نویسه	*s;
    نویسه	*p;
    صحیح		شمار;
    نویسه	**names = پوچ;
    صحیح		i;
    صحیح		target = choices[idx].arg;

    name = targets[target].batname;
    batpath = targets[target].batpath;

    path = getenv("PATH");
    اگر (path == پوچ)
    {
	printf("\nERROR: The variable $PATH is not set\n");
	بازگشت;
    }

    /*
     * first round: count number of names in path;
     * second round: save names to names[].
     */
    برای (;;)
    {
	شمار = 1;
	برای (p = path; *p; )
	{
	    s = strchr(p, ';');
	    اگر (s == پوچ)
		s = p + strlen(p);
	    اگر (names != پوچ)
	    {
		names[شمار] = alloc(s - p + 1);
		strncpy(names[شمار], p, s - p);
		names[شمار][s - p] = NUL;
	    }
	    ++شمار;
	    p = s;
	    اگر (*p != NUL)
		++p;
	}
	اگر (names != پوچ)
	    بشکن;
	names = alloc((شمار + 1) * اندازه(نویسه *));
    }
    names[0] = alloc(50);
    sprintf(names[0], "Select directory to create %s in:", name);
    names[شمار] = alloc(50);
    اگر (choices[idx].arg == 0)
	sprintf(names[شمار], "Do not create any .bat file.");
    وگرنه
	sprintf(names[شمار], "Do not create a %s file.", name);
    n = get_choice(names, شمار + 1);

    اگر (n == شمار)
    {
	// Selected last item, don't create bat file.
	*batpath = NUL;
	اگر (choices[idx].arg != 0)
	    alloc_text(idx, "    Do NOT create %s", name);
    }
    وگرنه
    {
	// Selected one of the paths.  For the first item only keep the path,
	// for the others append the batch file name.
	strcpy(batpath, names[n]);
	add_pathsep(batpath);
	اگر (choices[idx].arg != 0)
	    set_bat_text(idx, batpath, name);
    }

    برای (i = 0; i <= شمار; ++i)
	آزاد(names[i]);
    آزاد(names);
}

نویسه *bat_text_yes = "Install .bat files to use Vim at the command line:";
نویسه *bat_text_no = "do NOT install .bat files to use Vim at the command line";

    ایستا تهی
change_main_bat_choice(صحیح idx)
{
    صحیح		i;

    // let the user select a default directory or NONE
    change_bat_choice(idx);

    اگر (targets[0].batpath[0] != NUL)
	choices[idx].text = bat_text_yes;
    وگرنه
	choices[idx].text = bat_text_no;

    // update the individual batch file selections
    برای (i = 1; i < TARGET_شمار; ++i)
    {
	// Only make it active when the first item has a path and the vim.exe
	// or gvim.exe exists (there is a changefunc then).
	اگر (targets[0].batpath[0] != NUL
		&& choices[idx + i].changefunc != پوچ)
	{
	    choices[idx + i].active = 1;
	    اگر (choices[idx + i].changefunc == change_bat_choice
		    && targets[i].batpath[0] != NUL)
	    {
		strcpy(targets[i].batpath, targets[0].batpath);
		set_bat_text(idx + i, targets[i].batpath, targets[i].batname);
	    }
	}
	وگرنه
	    choices[idx + i].active = 0;
    }
}

/*
 * Initialize a choice for creating a batch file.
 */
    ایستا تهی
init_bat_choice(صحیح target)
{
    نویسه	*batpath = targets[target].batpath;
    نویسه	*oldbat = targets[target].oldbat;
    نویسه	*p;
    صحیح		i;

    choices[choice_شمار].arg = target;
    choices[choice_شمار].installfunc = install_bat_choice;
    choices[choice_شمار].active = 1;
    choices[choice_شمار].text = پوچ;	// will be set below
    اگر (oldbat != پوچ)
    {
	// A [g]vim.bat exists: Only choice is to overwrite it or not.
	choices[choice_شمار].changefunc = toggle_bat_choice;
	*batpath = NUL;
	toggle_bat_choice(choice_شمار);
    }
    وگرنه
    {
	اگر (default_bat_dir != پوچ)
	    // Prefer using the same path as an existing .bat file.
	    strcpy(batpath, default_bat_dir);
	وگرنه
	{
	    // No [g]vim.bat exists: Write it to a directory in $PATH.  Use
	    // $WINDIR by default, if it's empty the first item in $PATH.
	    p = getenv("WINDIR");
	    اگر (p != پوچ && *p != NUL)
		strcpy(batpath, p);
	    وگرنه
	    {
		p = getenv("PATH");
		اگر (p == پوچ || *p == NUL)		// "cannot happen"
		    strcpy(batpath, "C:/Windows");
		وگرنه
		{
		    i = 0;
		    وقتی (*p != NUL && *p != ';')
			batpath[i++] = *p++;
		    batpath[i] = NUL;
		}
	    }
	}
	add_pathsep(batpath);
	set_bat_text(choice_شمار, batpath, targets[target].batname);

	choices[choice_شمار].changefunc = change_bat_choice;
    }
    ++choice_شمار;
}

/*
 * Set up the choices for installing .bat files.
 * For these items "arg" is the index in targets[].
 */
    ایستا تهی
init_bat_choices(تهی)
{
    صحیح		i;

    // The first item is used to switch installing batch files on/off and
    // setting the default path.
    choices[choice_شمار].text = bat_text_yes;
    choices[choice_شمار].changefunc = change_main_bat_choice;
    choices[choice_شمار].installfunc = پوچ;
    choices[choice_شمار].active = 1;
    choices[choice_شمار].arg = 0;
    ++choice_شمار;

    // Add items for each batch file target.  Only used when not disabled by
    // the first item.  When a .exe exists, don't offer to create a .bat.
    برای (i = 1; i < TARGET_شمار; ++i)
	اگر (targets[i].oldexe == پوچ
		&& (targets[i].exenamearg[0] == 'g' ? has_gvim : has_vim))
	    init_bat_choice(i);
	وگرنه
	    add_dummy_choice();
}

/*
 * Install the vimrc file.
 */
    ایستا تهی
install_vimrc(صحیح idx UNUSED)
{
    FILE	*fd, *tfd;
    نویسه	*fname;

    // If an old vimrc file exists, overwrite it.
    // Otherwise create a new one.
    اگر (*oldvimrc != NUL)
	fname = oldvimrc;
    وگرنه
	fname = vimrc;

    fd = fopen(fname, "w");
    اگر (fd == پوچ)
    {
	printf("\nERROR: Cannot open \"%s\" for writing.\n", fname);
	بازگشت;
    }
    انتخاب (compat_choice)
    {
	حالت compat_vi:
		    fprintf(fd, "\" Vi compatible\n");
		    fprintf(fd, "set compatible\n");
		    بشکن;
	حالت compat_vim:
		    fprintf(fd, "\" Vim's default behavior\n");
		    fprintf(fd, "if &compatible\n");
		    fprintf(fd, "  set nocompatible\n");
		    fprintf(fd, "endif\n");
		    بشکن;
	حالت compat_some_enhancements:
		    fprintf(fd, "\" Vim with some enhancements\n");
		    fprintf(fd, "source $VIMRUNTIME/defaults.vim\n");
		    بشکن;
	حالت compat_all_enhancements:
		    fprintf(fd, "\" Vim with all enhancements\n");
		    fprintf(fd, "source $VIMRUNTIME/vimrc_example.vim\n");
		    بشکن;
    }
    انتخاب (remap_choice)
    {
	حالت remap_no:
		    بشکن;
	حالت remap_win:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Remap a few keys for Windows behavior\n");
		    fprintf(fd, "source $VIMRUNTIME/mswin.vim\n");
		    بشکن;
    }
    انتخاب (mouse_choice)
    {
	حالت mouse_xterm:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Mouse behavior (the Unix way)\n");
		    fprintf(fd, "behave xterm\n");
		    بشکن;
	حالت mouse_mswin:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Mouse behavior (the Windows way)\n");
		    fprintf(fd, "behave mswin\n");
		    بشکن;
	حالت mouse_default:
		    بشکن;
    }
    اگر ((tfd = fopen("diff.exe", "r")) != پوچ)
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

    ایستا تهی
change_vimrc_choice(صحیح idx)
{
    اگر (choices[idx].installfunc != پوچ)
    {
	// Switch to NOT change or create a vimrc file.
	اگر (*oldvimrc != NUL)
	    alloc_text(idx, "Do NOT change startup file %s", oldvimrc);
	وگرنه
	    alloc_text(idx, "Do NOT create startup file %s", vimrc);
	choices[idx].installfunc = پوچ;
	choices[idx + 1].active = 0;
	choices[idx + 2].active = 0;
	choices[idx + 3].active = 0;
    }
    وگرنه
    {
	// Switch to change or create a vimrc file.
	اگر (*oldvimrc != NUL)
	    alloc_text(idx, "Overwrite startup file %s with:", oldvimrc);
	وگرنه
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
    ایستا تهی
change_run_choice(صحیح idx)
{
    compat_choice = get_choice(compat_choices, TABLE_SIZE(compat_choices));
    alloc_text(idx, compat_text, compat_choices[compat_choice]);
}

/*
 * Change the choice if keys are to be remapped.
 */
    ایستا تهی
change_remap_choice(صحیح idx)
{
    remap_choice = get_choice(remap_choices, TABLE_SIZE(remap_choices));
    alloc_text(idx, remap_text, remap_choices[remap_choice]);
}

/*
 * Change the choice how to select text.
 */
    ایستا تهی
change_mouse_choice(صحیح idx)
{
    mouse_choice = get_choice(mouse_choices, TABLE_SIZE(mouse_choices));
    alloc_text(idx, mouse_text, mouse_choices[mouse_choice]);
}

    ایستا تهی
init_vimrc_choices(تهی)
{
    // set path for a new _vimrc file (also when not used)
    strcpy(vimrc, installdir);
    strcpy(vimrc + runtimeidx, "_vimrc");

    // Set opposite value and then toggle it by calling change_vimrc_choice()
    اگر (*oldvimrc == NUL)
	choices[choice_شمار].installfunc = پوچ;
    وگرنه
	choices[choice_شمار].installfunc = install_vimrc;
    choices[choice_شمار].text = پوچ;
    change_vimrc_choice(choice_شمار);
    choices[choice_شمار].changefunc = change_vimrc_choice;
    choices[choice_شمار].active = 1;
    ++choice_شمار;

    // default way to run Vim
    alloc_text(choice_شمار, compat_text, compat_choices[compat_choice]);
    choices[choice_شمار].changefunc = change_run_choice;
    choices[choice_شمار].installfunc = پوچ;
    choices[choice_شمار].active = (*oldvimrc == NUL);
    ++choice_شمار;

    // Whether to remap keys
    alloc_text(choice_شمار, remap_text , remap_choices[remap_choice]);
    choices[choice_شمار].changefunc = change_remap_choice;
    choices[choice_شمار].installfunc = پوچ;
    choices[choice_شمار].active = (*oldvimrc == NUL);
    ++choice_شمار;

    // default way to use the mouse
    alloc_text(choice_شمار, mouse_text, mouse_choices[mouse_choice]);
    choices[choice_شمار].changefunc = change_mouse_choice;
    choices[choice_شمار].installfunc = پوچ;
    choices[choice_شمار].active = (*oldvimrc == NUL);
    ++choice_شمار;
}

    ایستا LONG
reg_create_کلید(
    HKEY ریشه,
    ثابت نویسه *subkey,
    PHKEY ph_کلید,
    DWORD flag)
{
    DWORD disp;

    *ph_کلید = پوچ;
    بازگشت Reg_Create_کلید_Ex(
		ریشه, subkey,
		0, پوچ, REG_OPTION_NON_VOLATILE,
		flag | کلید_WRITE,
		پوچ, ph_کلید, &disp);
}

    ایستا LONG
reg_set_string_مقدار(
    HKEY h_کلید,
    ثابت نویسه *مقدار_name,
    ثابت نویسه *data)
{
    بازگشت Reg_Set_مقدار_Ex(h_کلید, مقدار_name, 0, REG_SZ,
				     (LPBYTE)data, (DWORD)(1 + strlen(data)));
}

    ایستا LONG
reg_create_کلید_and_مقدار(
    HKEY h_ریشه_کلید,
    ثابت نویسه *subkey,
    ثابت نویسه *مقدار_name,
    ثابت نویسه *data,
    DWORD flag)
{
    HKEY h_کلید;
    LONG lRet = reg_create_کلید(h_ریشه_کلید, subkey, &h_کلید, flag);

    اگر (ERROR_SUCCESS == lRet)
    {
	lRet = reg_set_string_مقدار(h_کلید, مقدار_name, data);
	Reg_Close_کلید(h_کلید);
    }
    بازگشت lRet;
}

    ایستا LONG
register_inproc_server(
    HKEY h_ریشه_کلید,
    ثابت نویسه *clsid,
    ثابت نویسه *extname,
    ثابت نویسه *module,
    ثابت نویسه *threading_model,
    DWORD flag)
{
    CHAR subkey[BUFSIZE];
    LONG lRet;

    sprintf(subkey, "CLSID\\%s", clsid);
    lRet = reg_create_کلید_and_مقدار(h_ریشه_کلید, subkey, پوچ, extname, flag);
    اگر (ERROR_SUCCESS == lRet)
    {
	sprintf(subkey, "CLSID\\%s\\InProcServer32", clsid);
	lRet = reg_create_کلید_and_مقدار(h_ریشه_کلید, subkey, پوچ, module, flag);
	اگر (ERROR_SUCCESS == lRet)
	{
	    lRet = reg_create_کلید_and_مقدار(h_ریشه_کلید, subkey,
					   "ThreadingModel", threading_model, flag);
	}
    }
    بازگشت lRet;
}

    ایستا LONG
register_shellex(
    HKEY h_ریشه_کلید,
    ثابت نویسه *clsid,
    ثابت نویسه *name,
    ثابت نویسه *exe_path,
    DWORD flag)
{
    LONG lRet = reg_create_کلید_and_مقدار(
	    h_ریشه_کلید,
	    "*\\shellex\\ContextMenuHandlers\\gvim",
	    پوچ,
	    clsid,
	    flag);

    اگر (ERROR_SUCCESS == lRet)
    {
	lRet = reg_create_کلید_and_مقدار(
		HKEY_LOCAL_MACHINE,
		"Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved",
		clsid,
		name,
		flag);

	اگر (ERROR_SUCCESS == lRet)
	{
	    lRet = reg_create_کلید_and_مقدار(
		    HKEY_LOCAL_MACHINE,
		    "Software\\Vim\\Gvim",
		    "path",
		    exe_path,
		    flag);
	}
    }
    بازگشت lRet;
}

    ایستا LONG
register_openwith(
    HKEY h_ریشه_کلید,
    ثابت نویسه *exe_path,
    DWORD flag)
{
    نویسه	exe_cmd[BUFSIZE];
    LONG	lRet;

    sprintf(exe_cmd, "\"%s\" \"%%1\"", exe_path);
    lRet = reg_create_کلید_and_مقدار(
	    h_ریشه_کلید,
	    "Applications\\gvim.exe\\shell\\edit\\command",
	    پوچ,
	    exe_cmd,
	    flag);

    اگر (ERROR_SUCCESS == lRet)
    {
	صحیح i;
	ایستا ثابت نویسه *openwith[] = {
		".htm\\OpenWithList\\gvim.exe",
		".vim\\OpenWithList\\gvim.exe",
		"*\\OpenWithList\\gvim.exe",
	};

	برای (i = 0; ERROR_SUCCESS == lRet && i < (صحیح)ARRAYSIZE(openwith); i++)
	    lRet = reg_create_کلید_and_مقدار(h_ریشه_کلید, openwith[i], پوچ, "", flag);
    }

    بازگشت lRet;
}

    ایستا LONG
register_uninstall(
    HKEY h_ریشه_کلید,
    ثابت نویسه *appname,
    ثابت نویسه *display_name,
    ثابت نویسه *uninstall_string,
    ثابت نویسه *display_icon,
    ثابت نویسه *display_version,
    ثابت نویسه *publisher)
{
    LONG lRet = reg_create_کلید_and_مقدار(h_ریشه_کلید, appname,
			     "DisplayName", display_name, کلید_WOW64_64_کلید);

    اگر (ERROR_SUCCESS == lRet)
	lRet = reg_create_کلید_and_مقدار(h_ریشه_کلید, appname,
		     "UninstallString", uninstall_string, کلید_WOW64_64_کلید);
    اگر (ERROR_SUCCESS == lRet)
	lRet = reg_create_کلید_and_مقدار(h_ریشه_کلید, appname,
		     "DisplayIcon", display_icon, کلید_WOW64_64_کلید);
    اگر (ERROR_SUCCESS == lRet)
	lRet = reg_create_کلید_and_مقدار(h_ریشه_کلید, appname,
		     "DisplayVersion", display_version, کلید_WOW64_64_کلید);
    اگر (ERROR_SUCCESS == lRet)
	lRet = reg_create_کلید_and_مقدار(h_ریشه_کلید, appname,
		     "Publisher", publisher, کلید_WOW64_64_کلید);
    بازگشت lRet;
}

/*
 * Add some entries to the registry:
 * - to add "Edit with Vim" to the context * menu
 * - to add Vim to the "Open with..." list
 * - to uninstall Vim
 */
//ARGSUSED
    ایستا صحیح
install_registry(تهی)
{
    LONG	lRet = ERROR_SUCCESS;
    ثابت نویسه	*vim_ext_ThreadingModel = "Apartment";
    ثابت نویسه	*vim_ext_name = "Vim Shell Extension";
    ثابت نویسه	*vim_ext_clsid = "{51EEE242-AD87-11d3-9C1E-0090278BBD99}";
    نویسه	vim_exe_path[MAX_PATH];
    نویسه	display_name[BUFSIZE];
    نویسه	uninstall_string[BUFSIZE];
    نویسه	icon_string[BUFSIZE];
    نویسه	version_string[BUFSIZE];
    صحیح		i;
    صحیح		loop_شمار = is_64bit_os() ? 2 : 1;
    DWORD	flag;

    sprintf(vim_exe_path, "%s\\gvim.exe", installdir);

    اگر (install_popup)
    {
	نویسه	    bufg[BUFSIZE];

	printf("Creating \"Edit with Vim\" popup menu entry\n");

	برای (i = 0; i < loop_شمار; i++)
	{
	    اگر (i == 0)
	    {
		sprintf(bufg, "%s\\" GVIMEXT32_PATH, installdir);
		flag = کلید_WOW64_32_کلید;
	    }
	    وگرنه
	    {
		sprintf(bufg, "%s\\" GVIMEXT64_PATH, installdir);
		flag = کلید_WOW64_64_کلید;
	    }

	    lRet = register_inproc_server(
		    HKEY_CLASSES_ریشه, vim_ext_clsid, vim_ext_name,
		    bufg, vim_ext_ThreadingModel, flag);
	    اگر (ERROR_SUCCESS != lRet)
		بازگشت FAIL;
	    lRet = register_shellex(
		    HKEY_CLASSES_ریشه, vim_ext_clsid, vim_ext_name,
		    vim_exe_path, flag);
	    اگر (ERROR_SUCCESS != lRet)
		بازگشت FAIL;
	}
    }

    اگر (install_openwith)
    {
	printf("Creating \"Open with ...\" list entry\n");

	برای (i = 0; i < loop_شمار; i++)
	{
	    اگر (i == 0)
		flag = کلید_WOW64_32_کلید;
	    وگرنه
		flag = کلید_WOW64_64_کلید;

	    lRet = register_openwith(HKEY_CLASSES_ریشه, vim_exe_path, flag);
	    اگر (ERROR_SUCCESS != lRet)
		بازگشت FAIL;
	}
    }

    printf("Creating an uninstall entry\n");
    sprintf(display_name, "Vim " VIM_VERSION_SHORT
#ifdef _M_ARM64
	    " (arm64)"
#وگرنهاگر _M_X64
	    " (x64)"
#endif
	    );

    // For the NSIS installer use the generated uninstaller.
    اگر (interactive)
	sprintf(uninstall_string, "%s\\uninstall.exe", installdir);
    وگرنه
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
    اگر (ERROR_SUCCESS != lRet)
	بازگشت FAIL;

    بازگشت OK;
}

    ایستا تهی
change_popup_choice(صحیح idx)
{
    اگر (install_popup == 0)
    {
	choices[idx].text = "Install an entry for Vim in the popup menu for the right\n    mouse button so that you can edit any file with Vim";
	install_popup = 1;
    }
    وگرنه
    {
	choices[idx].text = "Do NOT install an entry for Vim in the popup menu for the\n    right mouse button to edit any file with Vim";
	install_popup = 0;
    }
}

/*
 * Only add the choice for the popup menu entry when gvim.exe was found and
 * both gvimext.dll and regedit.exe exist.
 */
    ایستا تهی
init_popup_choice(تهی)
{
    ساختار stat	st;

    اگر (has_gvim
	    && (stat(GVIMEXT32_PATH, &st) >= 0
		|| stat(GVIMEXT64_PATH, &st) >= 0))
    {
	choices[choice_شمار].changefunc = change_popup_choice;
	choices[choice_شمار].installfunc = پوچ;
	choices[choice_شمار].active = 1;
	change_popup_choice(choice_شمار);  // set the text
	++choice_شمار;
    }
    وگرنه
	add_dummy_choice();
}

    ایستا تهی
change_openwith_choice(صحیح idx)
{
    اگر (install_openwith == 0)
    {
	choices[idx].text = "Add Vim to the \"Open With...\" list in the popup menu for the right\n    mouse button so that you can edit any file with Vim";
	install_openwith = 1;
    }
    وگرنه
    {
	choices[idx].text = "Do NOT add Vim to the \"Open With...\" list in the popup menu for the\n    right mouse button to edit any file with Vim";
	install_openwith = 0;
    }
}

/*
 * Only add the choice for the open-with menu entry when gvim.exe was found
 * and regedit.exe exist.
 */
    ایستا تهی
init_openwith_choice(تهی)
{
    اگر (has_gvim)
    {
	choices[choice_شمار].changefunc = change_openwith_choice;
	choices[choice_شمار].installfunc = پوچ;
	choices[choice_شمار].active = 1;
	change_openwith_choice(choice_شمار);  // set the text
	++choice_شمار;
    }
    وگرنه
	add_dummy_choice();
}

/*
 * Create a shell link.
 *
 * returns 0 on failure, non-zero on successful completion.
 *
 * NOTE:  Currently untested with mingw.
 */
    صحیح
create_shortcut(
	ثابت نویسه *shortcut_name,
	ثابت نویسه *iconfile_path,
	صحیح	    iconindex,
	ثابت نویسه *shortcut_target,
	ثابت نویسه *shortcut_args,
	ثابت نویسه *workingdir
	)
{
    IShellLink	    *shelllink_ptr;
    HRESULT	    hres;
    IPersistFile	    *persistfile_ptr;

    // Initialize COM library
    hres = CoInitialize(پوچ);
    اگر (!SUCCEEDED(hres))
    {
	printf("Error:  Could not open the COM library.  Not creating shortcut.\n");
	بازگشت FAIL;
    }

    // Instantiate a COM object for the ShellLink, store a pointer to it
    // in shelllink_ptr.
    hres = CoCreateInstance(&CLSID_ShellLink,
			   پوچ,
			   CLSCTX_INPROC_SERVER,
			   &IID_IShellLink,
			   (تهی **) &shelllink_ptr);

    اگر (SUCCEEDED(hres)) // If the instantiation was successful...
    {
	// ...Then build a PersistFile interface for the ShellLink so we can
	// save it as a file after we build it.
	hres = shelllink_ptr->lpVtbl->QueryInterface(shelllink_ptr,
		&IID_IPersistFile, (تهی **) &persistfile_ptr);

	اگر (SUCCEEDED(hres))
	{
	    wchar_t wsz[BUFSIZE];

	    // translate the (possibly) multibyte shortcut filename to windows
	    // Unicode so it can be used as a file name.
	    MultiByteToWideChar(CP_ACP, 0, shortcut_name, -1, wsz, اندازه(wsz)/اندازه(wsz[0]));

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
	وگرنه
	{
	    printf("QueryInterface Error\n");
	    بازگشت FAIL;
	}

	// Return the ShellLink object
	shelllink_ptr->lpVtbl->Release(shelllink_ptr);
    }
    وگرنه
    {
	printf("CoCreateInstance Error - hres = %08x\n", (صحیح)hres);
	بازگشت FAIL;
    }

    بازگشت OK;
}

/*
 * Build a path to where we will put a specified link.
 *
 * Return 0 on error, non-zero on success
 */
   صحیح
build_link_name(
	نویسه	   *link_path,
	ثابت نویسه *link_name,
	ثابت نویسه *shell_folder_name)
{
    نویسه	shell_folder_path[MAX_PATH];

    اگر (get_shell_folder_path(shell_folder_path, shell_folder_name) == FAIL)
    {
	printf("An error occurred while attempting to find the path to %s.\n",
							   shell_folder_name);
	بازگشت FAIL;
    }

    // Make sure the directory exists (create Start Menu\Programs\Vim).
    // Ignore errors if it already exists.
    vim_mkdir(shell_folder_path, 0755);

    // build the path to the shortcut and the path to gvim.exe
    sprintf(link_path, "%s\\%s.lnk", shell_folder_path, link_name);

    بازگشت OK;
}

    ایستا صحیح
build_shortcut(
	ثابت نویسه *name,	// Name of the shortcut
	ثابت نویسه *exename,	// Name of the executable (e.g., vim.exe)
	ثابت نویسه *args,
	ثابت نویسه *shell_folder,
	ثابت نویسه *workingdir)
{
    نویسه	executable_path[BUFSIZE];
    نویسه	link_name[BUFSIZE];

    sprintf(executable_path, "%s\\%s", installdir, exename);

    اگر (build_link_name(link_name, name, shell_folder) == FAIL)
    {
	printf("An error has occurred.  A shortcut to %s will not be created %s.\n",
		name,
		*shell_folder == 'd' ? "on the desktop" : "in the Start menu");
	بازگشت FAIL;
    }

    // Create the shortcut:
    بازگشت create_shortcut(link_name, executable_path, 0,
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
    ایستا تهی
install_start_menu(صحیح idx UNUSED)
{
    need_uninstall_entry = 1;
    printf("Creating start menu\n");
    اگر (has_vim)
    {
	اگر (build_shortcut("Vim", "vim.exe", "",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    بازگشت;
	اگر (build_shortcut("Vim Read-only", "vim.exe", "-R",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    بازگشت;
	اگر (build_shortcut("Vim Diff", "vim.exe", "-d",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    بازگشت;
    }
    اگر (has_gvim)
    {
	اگر (build_shortcut("gVim", "gvim.exe", "",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    بازگشت;
	اگر (build_shortcut("gVim Easy", "gvim.exe", "-y",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    بازگشت;
	اگر (build_shortcut("gVim Read-only", "gvim.exe", "-R",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    بازگشت;
	اگر (build_shortcut("gVim Diff", "gvim.exe", "-d",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    بازگشت;
    }
    اگر (build_shortcut("Uninstall",
		interactive ? "uninstall.exe" : "uninstall-gui.exe", "",
					   VIM_STARTMENU, installdir) == FAIL)
	بازگشت;
    // For Windows NT the working dir of the vimtutor.bat must be right,
    // otherwise gvim.exe won't be found and using gvimbat doesn't work.
    اگر (build_shortcut("Vim tutor", "vimtutor.bat", "",
					   VIM_STARTMENU, installdir) == FAIL)
	بازگشت;
    اگر (build_shortcut("Help", has_gvim ? "gvim.exe" : "vim.exe", "-c h",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	بازگشت;
    {
	نویسه	shell_folder_path[BUFSIZE];

	// Creating the URL shortcut works a bit differently...
	اگر (get_shell_folder_path(shell_folder_path, VIM_STARTMENU) == FAIL)
	{
	    printf("Finding the path of the Start menu failed\n");
	    بازگشت ;
	}
	add_pathsep(shell_folder_path);
	strcat(shell_folder_path, "Vim Online.url");
	اگر (!WritePrivateProfileString("InternetShortcut", "URL",
				    "https://www.vim.org/", shell_folder_path))
	{
	    printf("Creating the Vim online URL failed\n");
	    بازگشت;
	}
    }
}

    ایستا تهی
toggle_startmenu_choice(صحیح idx)
{
    اگر (choices[idx].installfunc == پوچ)
    {
	choices[idx].installfunc = install_start_menu;
	choices[idx].text = "Add Vim to the Start menu";
    }
    وگرنه
    {
	choices[idx].installfunc = پوچ;
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
    تهی
install_shortcut_gvim(صحیح idx)
{
    // Create shortcut(s) on the desktop
    اگر (choices[idx].arg)
    {
	(تهی)build_shortcut(icon_names[0], "gvim.exe",
						      "", "desktop", WORKDIR);
	need_uninstall_entry = 1;
    }
}

    تهی
install_shortcut_evim(صحیح idx)
{
    اگر (choices[idx].arg)
    {
	(تهی)build_shortcut(icon_names[1], "gvim.exe",
						    "-y", "desktop", WORKDIR);
	need_uninstall_entry = 1;
    }
}

    تهی
install_shortcut_gview(صحیح idx)
{
    اگر (choices[idx].arg)
    {
	(تهی)build_shortcut(icon_names[2], "gvim.exe",
						    "-R", "desktop", WORKDIR);
	need_uninstall_entry = 1;
    }
}

    تهی
toggle_shortcut_choice(صحیح idx)
{
    نویسه	*arg;

    اگر (choices[idx].installfunc == install_shortcut_gvim)
	arg = "gVim";
    وگرنه اگر (choices[idx].installfunc == install_shortcut_evim)
	arg = "gVim Easy";
    وگرنه
	arg = "gVim Read-only";
    اگر (choices[idx].arg)
    {
	choices[idx].arg = 0;
	alloc_text(idx, "Do NOT create a desktop icon for %s", arg);
    }
    وگرنه
    {
	choices[idx].arg = 1;
	alloc_text(idx, "Create a desktop icon for %s", arg);
    }
}

    ایستا تهی
init_startmenu_choice(تهی)
{
    // Start menu
    choices[choice_شمار].changefunc = toggle_startmenu_choice;
    choices[choice_شمار].installfunc = پوچ;
    choices[choice_شمار].active = 1;
    toggle_startmenu_choice(choice_شمار);	// set the text
    ++choice_شمار;
}

/*
 * Add the choice for the desktop shortcuts.
 */
    ایستا تهی
init_shortcut_choices(تهی)
{
    // Shortcut to gvim
    choices[choice_شمار].text = پوچ;
    choices[choice_شمار].arg = 0;
    choices[choice_شمار].active = has_gvim;
    choices[choice_شمار].changefunc = toggle_shortcut_choice;
    choices[choice_شمار].installfunc = install_shortcut_gvim;
    toggle_shortcut_choice(choice_شمار);
    ++choice_شمار;

    // Shortcut to evim
    choices[choice_شمار].text = پوچ;
    choices[choice_شمار].arg = 0;
    choices[choice_شمار].active = has_gvim;
    choices[choice_شمار].changefunc = toggle_shortcut_choice;
    choices[choice_شمار].installfunc = install_shortcut_evim;
    toggle_shortcut_choice(choice_شمار);
    ++choice_شمار;

    // Shortcut to gview
    choices[choice_شمار].text = پوچ;
    choices[choice_شمار].arg = 0;
    choices[choice_شمار].active = has_gvim;
    choices[choice_شمار].changefunc = toggle_shortcut_choice;
    choices[choice_شمار].installfunc = install_shortcut_gview;
    toggle_shortcut_choice(choice_شمار);
    ++choice_شمار;
}

/*
 * Attempt to register OLE for Vim.
 */
   ایستا تهی
install_OLE_register(تهی)
{
    نویسه register_command_string[BUFSIZE + 30];

    printf("\n--- Attempting to register Vim with OLE ---\n");
    printf("(There is no message whether this works or not.)\n");

    sprintf(register_command_string, "\"%s\\gvim.exe\" -silent -register", installdir);
    system(register_command_string);
}

/*
 * Remove the last part of directory "path[]" to get its parent, and put the
 * result in "to[]".
 */
    ایستا تهی
dir_remove_last(ثابت نویسه *path, نویسه to[MAX_PATH])
{
    نویسه c;
    long last_char_to_copy;
    long path_length = strlen(path);

    // skip the last character just in case it is a '\\'
    last_char_to_copy = path_length - 2;
    c = path[last_char_to_copy];

    وقتی (c != '\\')
    {
	last_char_to_copy--;
	c = path[last_char_to_copy];
    }

    strncpy(to, path, (size_t)last_char_to_copy);
    to[last_char_to_copy] = NUL;
}

    ایستا تهی
set_directories_text(صحیح idx)
{
    صحیح vimfiles_dir_choice = choices[idx].arg;

    اگر (vimfiles_dir_choice == (صحیح)vimfiles_dir_none)
	alloc_text(idx, "Do NOT create plugin directories%s", "");
    وگرنه
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
ایستا نویسه *homedir = پوچ;

    تهی
init_homedir(تهی)
{
    نویسه    *var;
    نویسه    buf[MAX_PATH];

    اگر (homedir != پوچ)
    {
	آزاد(homedir);
	homedir = پوچ;
    }

    var = getenv("HOME");

    /*
     * Typically, $HOME is not defined on Windows, unless the user has
     * specifically defined it for Vim's sake.  However, on Windows NT
     * platforms, $HOMEDRIVE and $HOMEPATH are automatically defined for
     * each user.  Try constructing $HOME from these.
     */
    اگر (var == پوچ || *var == NUL)
    {
	نویسه	*homedrive, *homepath;

	homedrive = getenv("HOMEDRIVE");
	homepath = getenv("HOMEPATH");
	اگر (homepath == پوچ || *homepath == NUL)
	    homepath = "\\";
	اگر (homedrive != پوچ
		   && strlen(homedrive) + strlen(homepath) < اندازه(buf))
	{
	    اگر (snprintf(buf, اندازه(buf), "%s%s", homedrive, homepath) > 0
		&& buf[0] != NUL)
		var = buf;
	}
    }

    اگر (var == پوچ)
	var = getenv("USERPROFILE");

    /*
     * Weird but true: $HOME may contain an indirect reference to another
     * variable, esp. "%USERPROFILE%".  Happens when $USERPROFILE isn't set
     * when $HOME is being set.
     */
    اگر (var != پوچ && *var == '%')
    {
	نویسه	*p;
	نویسه	*exp;

	p = strchr(var + 1, '%');
	اگر (p != پوچ)
	{
	    strncpy(buf, var + 1, p - (var + 1));
	    buf[p - (var + 1)] = NUL;
	    exp = getenv(buf);
	    اگر (exp != پوچ && *exp != NUL
				&& strlen(exp) + strlen(p) < اندازه(buf))
	    {
		snprintf(buf, اندازه(buf), "%s%s", exp, p + 1);
		var = buf;
	    }
	}
    }

    اگر (var != پوچ && *var == NUL)	// empty is same as not set
	var = پوچ;

    اگر (var == پوچ)
	homedir = پوچ;
    وگرنه
	homedir = _strdup(var);
}

/*
 * Change the directory that the vim plugin directories will be created in:
 * $HOME, $VIM or nowhere.
 */
    ایستا تهی
change_directories_choice(صحیح idx)
{
    صحیح	    choice_شمار = TABLE_SIZE(vimfiles_dir_choices);

    // Don't offer the $HOME choice if $HOME isn't set.
    اگر (homedir == پوچ)
	--choice_شمار;
    choices[idx].arg = get_choice(vimfiles_dir_choices, choice_شمار);
    set_directories_text(idx);
}

/*
 * Create the plugin directories...
 */
//ARGSUSED
    ایستا تهی
install_vimfilesdir(صحیح idx)
{
    صحیح i;
    صحیح vimfiles_dir_choice = choices[idx].arg;
    نویسه *p;
    نویسه vimdir_path[MAX_PATH];
    نویسه vimfiles_path[MAX_PATH + 9];
    نویسه tmp_dirname[BUFSIZE];

    // switch on the location that the user wants the plugin directories
    // built in
    انتخاب (vimfiles_dir_choice)
    {
	حالت vimfiles_dir_vim:
	{
	    // Go to the %VIM% directory - check env first, then go one dir
	    //	   below installdir if there is no %VIM% environment variable.
	    //	   The accuracy of $VIM is checked in inspect_system(), so we
	    //	   can be sure it is ok to use here.
	    p = getenv("VIM");
	    اگر (p == پوچ) // No $VIM in path
		dir_remove_last(installdir, vimdir_path);
	    وگرنه
		strcpy(vimdir_path, p);
	    بشکن;
	}
	حالت vimfiles_dir_home:
	{
	    // Find the $HOME directory.  Its existence was already checked.
	    p = homedir;
	    اگر (p == پوچ)
	    {
		printf("Internal error: $HOME is NULL\n");
		p = "c:\\";
	    }
	    strcpy(vimdir_path, p);
	    بشکن;
	}
	حالت vimfiles_dir_none:
	{
	    // Do not create vim plugin directory.
	    بازگشت;
	}
    }

    // Now, just create the directory.	If it already exists, it will fail
    // silently.
    sprintf(vimfiles_path, "%s\\vimfiles", vimdir_path);
    vim_mkdir(vimfiles_path, 0755);

    printf("Creating the following directories in \"%s\":\n", vimfiles_path);
    برای (i = 0; i < TABLE_SIZE(vimfiles_subdirs); i++)
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
    ایستا تهی
init_directories_choice(تهی)
{
    ساختار stat	st;
    نویسه	tmp_dirname[BUFSIZE];
    نویسه	*p;
    صحیح		vimfiles_dir_choice;

    choices[choice_شمار].text = alloc(150);
    choices[choice_شمار].changefunc = change_directories_choice;
    choices[choice_شمار].installfunc = install_vimfilesdir;
    choices[choice_شمار].active = 1;

    // Check if the "compiler" directory already exists.  That's a good
    // indication that the plugin directories were already created.
    p = getenv("HOME");
    اگر (p != پوچ)
    {
	vimfiles_dir_choice = (صحیح)vimfiles_dir_home;
	sprintf(tmp_dirname, "%s\\vimfiles\\compiler", p);
	اگر (stat(tmp_dirname, &st) == 0)
	    vimfiles_dir_choice = (صحیح)vimfiles_dir_none;
    }
    وگرنه
    {
	vimfiles_dir_choice = (صحیح)vimfiles_dir_vim;
	p = getenv("VIM");
	اگر (p == پوچ)  // No $VIM in path, use the install dir.
	    dir_remove_last(installdir, tmp_dirname);
	وگرنه
	    strcpy(tmp_dirname, p);
	strcat(tmp_dirname, "\\vimfiles\\compiler");
	اگر (stat(tmp_dirname, &st) == 0)
	    vimfiles_dir_choice = (صحیح)vimfiles_dir_none;
    }

    choices[choice_شمار].arg = vimfiles_dir_choice;
    set_directories_text(choice_شمار);
    ++choice_شمار;
}

/*
 * Setup the choices and the default values.
 */
    ایستا تهی
setup_choices(تهی)
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

    ایستا تهی
print_cmd_سطر_help(تهی)
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
    ایستا تهی
command_سطر_setup_choices(صحیح argc, نویسه **argv)
{
    صحیح i, j;

    برای (i = 1; i < argc; i++)
    {
	اگر (strcmp(argv[i], "-create-batfiles") == 0)
	{
	    اگر (i + 1 == argc)
		ادامه;
	    وقتی (argv[i + 1][0] != '-' && i < argc)
	    {
		i++;
		برای (j = 1; j < TARGET_شمار; ++j)
		    اگر ((targets[j].exenamearg[0] == 'g' ? has_gvim : has_vim)
			    && strcmp(argv[i], targets[j].name) == 0)
		    {
			init_bat_choice(j);
			بشکن;
		    }
		اگر (j == TARGET_شمار)
		    printf("%s is not a valid choice for -create-batfiles\n",
								     argv[i]);

		اگر (i + 1 == argc)
		    بشکن;
	    }
	}
	وگرنه اگر (strcmp(argv[i], "-create-vimrc") == 0)
	{
	    // Setup default vimrc choices.  If there is already a _vimrc file,
	    // it will NOT be overwritten.
	    init_vimrc_choices();
	}
	وگرنه اگر (strcmp(argv[i], "-vimrc-remap") == 0)
	{
	    اگر (i + 1 == argc)
		بشکن;
	    i++;
	    اگر (strcmp(argv[i], "no") == 0)
		remap_choice = remap_no;
	    وگرنه اگر (strcmp(argv[i], "win") == 0)
		remap_choice = remap_win;
	}
	وگرنه اگر (strcmp(argv[i], "-vimrc-behave") == 0)
	{
	    اگر (i + 1 == argc)
		بشکن;
	    i++;
	    اگر (strcmp(argv[i], "unix") == 0)
		mouse_choice = mouse_xterm;
	    وگرنه اگر (strcmp(argv[i], "mswin") == 0)
		mouse_choice = mouse_mswin;
	    وگرنه اگر (strcmp(argv[i], "default") == 0)
		mouse_choice = mouse_default;
	}
	وگرنه اگر (strcmp(argv[i], "-vimrc-compat") == 0)
	{
	    اگر (i + 1 == argc)
		بشکن;
	    i++;
	    اگر (strcmp(argv[i], "vi") == 0)
		compat_choice = compat_vi;
	    وگرنه اگر (strcmp(argv[i], "vim") == 0)
		compat_choice = compat_vim;
	    وگرنه اگر (strcmp(argv[i], "defaults") == 0)
		compat_choice = compat_some_enhancements;
	    وگرنه اگر (strcmp(argv[i], "all") == 0)
		compat_choice = compat_all_enhancements;
	}
	وگرنه اگر (strcmp(argv[i], "-install-popup") == 0)
	{
	    init_popup_choice();
	}
	وگرنه اگر (strcmp(argv[i], "-install-openwith") == 0)
	{
	    init_openwith_choice();
	}
	وگرنه اگر (strcmp(argv[i], "-add-start-menu") == 0)
	{
	    init_startmenu_choice();
	}
	وگرنه اگر (strcmp(argv[i], "-install-icons") == 0)
	{
	    init_shortcut_choices();
	}
	وگرنه اگر (strcmp(argv[i], "-create-directories") == 0)
	{
	    صحیح vimfiles_dir_choice = (صحیح)vimfiles_dir_none;

	    init_directories_choice();
	    اگر (i + 1 < argc && argv[i + 1][0] != '-')
	    {
		i++;
		اگر (strcmp(argv[i], "vim") == 0)
		    vimfiles_dir_choice = (صحیح)vimfiles_dir_vim;
		وگرنه اگر (strcmp(argv[i], "home") == 0)
		{
		    اگر (homedir == پوچ)  // No $HOME in environment
			vimfiles_dir_choice = (صحیح)vimfiles_dir_none;
		    وگرنه
			vimfiles_dir_choice = (صحیح)vimfiles_dir_home;
		}
		وگرنه
		{
		    printf("Unknown argument for -create-directories: %s\n",
								     argv[i]);
		    print_cmd_سطر_help();
		}
	    }
	    وگرنه // No choice specified, default to vim directory
		vimfiles_dir_choice = (صحیح)vimfiles_dir_vim;
	    choices[choice_شمار - 1].arg = vimfiles_dir_choice;
	}
	وگرنه اگر (strcmp(argv[i], "-register-OLE") == 0)
	{
	    // This is always done when gvim is found
	}
	وگرنه // Unknown switch
	{
	    printf("Got unknown argument argv[%d] = %s\n", i, argv[i]);
	    print_cmd_سطر_help();
	}
    }
}


/*
 * Show a few screens full of helpful information.
 */
    ایستا تهی
show_help(تهی)
{
    ایستا نویسه *(اقلام[]) =
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
پوچ
    };
    صحیح		i;
    صحیح		c;

    rewind(stdin);
    printf("\n");
    برای (i = 0; اقلام[i] != پوچ; ++i)
    {
	puts(اقلام[i]);
	printf("Hit Enter to continue, b (back) or q (quit help): ");
	c = getchar();
	rewind(stdin);
	اگر (c == 'b' || c == 'B')
	{
	    اگر (i == 0)
		--i;
	    وگرنه
		i -= 2;
	}
	اگر (c == 'q' || c == 'Q')
	    بشکن;
	printf("\n");
    }
}

/*
 * Install the choices.
 */
    ایستا تهی
install(تهی)
{
    صحیح		i;

    // Install the selected choices.
    برای (i = 0; i < choice_شمار; ++i)
	اگر (choices[i].installfunc != پوچ && choices[i].active)
	    (choices[i].installfunc)(i);

    // Add some entries to the registry, if needed.
    اگر (install_popup
	    || install_openwith
	    || (need_uninstall_entry && interactive)
	    || !interactive)
	install_registry();

    // Register gvim with OLE.
    اگر (has_gvim)
	install_OLE_register();
}

/*
 * request_choice
 */
    ایستا تهی
request_choice(تهی)
{
    صحیح		      i;

    printf("\n\nInstall will do for you:\n");
    برای (i = 0; i < choice_شمار; ++i)
	اگر (choices[i].active)
	    printf("%2d  %s\n", i + 1, choices[i].text);
    printf("To change an item, enter its number\n\n");
    printf("Enter item number, h (help), d (do it) or q (quit): ");
}

    صحیح
اصلی(صحیح argc, نویسه **argv)
{
    صحیح		i;
    نویسه	buf[BUFSIZE];

    /*
     * Run interactively if there are no command line arguments.
     */
    اگر (argc > 1)
	interactive = 0;
    وگرنه
	interactive = 1;

    // Initialize this program.
    do_inits(argv);
    init_homedir();

    اگر (argc > 1 && strcmp(argv[1], "-uninstall-check") == 0)
    {
	// Only check for already installed Vims.  Used by NSIS installer.
	i = uninstall_check(1);

	// Find the value of $VIM, because NSIS isn't able to do this by
	// itself.
	get_vim_env();

	// When nothing found exit quietly.  If something found wait for
	// a little while, so that the user can read the messages.
	اگر (i && _isatty(1))
	    sleep(3);
	exit(0);
    }

    printf("This program sets up the installation of Vim "
						   VIM_VERSION_MEDIUM "\n\n");

    // Check if the user unpacked the archives properly.
    check_unpack();

    // Check for already installed Vims.
    اگر (interactive)
	uninstall_check(0);

    // Find out information about the system.
    inspect_system();

    اگر (interactive)
    {
	// Setup all the choices.
	setup_choices();

	// Let the user change choices and finally install (or quit).
	برای (;;)
	{
	    request_choice();
	    rewind(stdin);
	    اگر (scanf("%99s", buf) == 1)
	    {
		اگر (isdigit((unsigned نویسه)buf[0]))
		{
		    // Change a choice.
		    i = atoi(buf);
		    اگر (i > 0 && i <= choice_شمار && choices[i - 1].active)
			(choices[i - 1].changefunc)(i - 1);
		    وگرنه
			printf("\nIllegal choice\n");
		}
		وگرنه اگر (buf[0] == 'h' || buf[0] == 'H')
		{
		    // Help
		    show_help();
		}
		وگرنه اگر (buf[0] == 'd' || buf[0] == 'D')
		{
		    // Install!
		    install();
		    printf("\nThat finishes the installation.  Happy Vimming!\n");
		    بشکن;
		}
		وگرنه اگر (buf[0] == 'q' || buf[0] == 'Q')
		{
		    // Quit
		    printf("\nExiting without anything done\n");
		    بشکن;
		}
		وگرنه
		    printf("\nIllegal choice\n");
	    }
	}
	printf("\n");
	myexit(0);
    }
    وگرنه
    {
	/*
	 * Run non-interactive - setup according to the command line switches
	 */
	command_سطر_setup_choices(argc, argv);
	install();

	// Avoid that the user has to hit Enter, just wait a little bit to
	// allow reading the messages.
	sleep(2);
    }

    بازگشت 0;
}
