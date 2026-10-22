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
#அந்தர்பாவ "dosinst.h"
#அந்தர்பாவ <io.h>

#define GVIMEXT64_PATH	    "GvimExt64\\gvimext.dll"
#define GVIMEXT32_PATH	    "GvimExt32\\gvimext.dll"

// Macro to do an error check I was typing over and over
#define CHECK_REG_ERROR(code) \
    செய் { \
	எனில் (code != ERROR_SUCCESS) \
	{ \
	    printf("%ld error number:  %ld\n", (long)__LINE__, (long)code); \
	    திரும்பு 1; \
	} \
    } போது (0)

முழு	has_vim = 0;		// installable vim.exe exists
முழு	has_gvim = 0;		// installable gvim.exe exists

எழுத்து	oldvimrc[BUFSIZE];	// name of existing vimrc file
எழுத்து	vimrc[BUFSIZE];		// name of vimrc file to create

எழுத்து	*default_bat_dir = வெற்று;  // when not NULL, use this as the default
				  // directory to write .bat files in
எழுத்து	*default_vim_dir = வெற்று;  // when not NULL, use this as the default
				  // install dir for NSIS

/*
 * Structure used for each choice the user can make.
 */
கட்டமைப்பு choice
{
    முழு	    active;			// non-zero when choice is active
    எழுத்து    *text;			// text displayed for this choice
    வெறுமை    (*changefunc)(முழு idx);	// function to change this choice
    முழு	    arg;			// argument for function
    வெறுமை    (*installfunc)(முழு idx);	// function to install this choice
};

கட்டமைப்பு choice	choices[30];		// choices the user can make
முழு		choice_மொத்தம் = 0;	// number of choices available

#define TABLE_SIZE(s)	(முழு)ARRAYSIZE(s)

எண்ணிக்கை
{
    compat_vi = 1,
    compat_vim,
    compat_some_enhancements,
    compat_all_enhancements
};
எழுத்து	*(compat_choices[]) =
{
    "\nChoose the default way to run Vim:",
    "Vi compatible",
    "Vim default",
    "with some Vim enhancements",
    "with syntax highlighting and other features switched on",
};
முழு	compat_choice = (முழு)compat_all_enhancements;
எழுத்து	*compat_text = "- run Vim %s";

எண்ணிக்கை
{
    remap_no = 1,
    remap_win
};
எழுத்து	*(remap_choices[]) =
{
    "\nChoose:",
    "Do not remap keys for Windows behavior",
    "Remap a few keys for Windows behavior (CTRL-V, CTRL-C, CTRL-F, etc)",
};
முழு	remap_choice = (முழு)remap_no;
எழுத்து	*remap_text = "- %s";

எண்ணிக்கை
{
    mouse_xterm = 1,
    mouse_mswin,
    mouse_default
};
எழுத்து	*(mouse_choices[]) =
{
    "\nChoose the way how Vim uses the mouse:",
    "right button extends selection (the Unix way)",
    "right button has a popup menu, left button starts select mode (the Windows way)",
    "right button has a popup menu, left button starts visual mode",
};
முழு	mouse_choice = (முழு)mouse_default;
எழுத்து	*mouse_text = "- The mouse %s";

எண்ணிக்கை
{
    vimfiles_dir_none = 1,
    vimfiles_dir_vim,
    vimfiles_dir_home
};
நிலையான எழுத்து *(vimfiles_dir_choices[]) =
{
    "\nCreate plugin directories:",
    "No",
    "In the VIM directory",
    "In your HOME directory",
};

// non-zero when selected to install the popup menu entry.
நிலையான முழு	install_popup = 0;

// non-zero when selected to install the "Open with" entry.
நிலையான முழு	install_openwith = 0;

// non-zero when need to add an uninstall entry in the registry
நிலையான முழு	need_uninstall_entry = 0;

/*
 * Definitions of the directory name (under $VIM) of the vimfiles directory
 * and its subdirectories:
 */
நிலையான எழுத்து	*(vimfiles_subdirs[]) =
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
    நிலையான முழு
get_choice(எழுத்து **table, முழு entries)
{
    முழு		answer;
    முழு		idx;
    எழுத்து	dummy[100];

    செய்
    {
	க்கு (idx = 0; idx < entries; ++idx)
	{
	    எனில் (idx)
		printf("%2d  ", idx);
	    puts(table[idx]);
	}
	printf("Choice: ");
	எனில் (scanf("%d", &answer) != 1)
	{
	    scanf("%99s", dummy);
	    answer = 0;
	}
    }
    போது (answer < 1 || answer >= entries);

    திரும்பு answer;
}

/*
 * Check if the user unpacked the archives properly.
 * Sets "runtimeidx".
 */
    நிலையான வெறுமை
check_unpack(வெறுமை)
{
    எழுத்து	buf[BUFSIZE];
    FILE	*fd;
    கட்டமைப்பு stat	st;

    // check for presence of the correct version number in installdir[]
    runtimeidx = strlen(installdir) - strlen(VIM_VERSION_NODOT);
    எனில் (runtimeidx <= 0
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
    எனில் (stat(buf, &st) < 0)
    {
	printf("ERROR: Cannot find filetype.vim in \"%s\"\n", installdir);
	printf("It looks like you did not unpack the runtime archive.\n");
	printf("You must unpack the runtime archive \"%srt.zip\" before installing.\n",
		VIM_VERSION_NODOT);
	myexit(1);
    }

    // Check if vim.exe or gvim.exe is in the current directory.
    எனில் ((fd = fopen("gvim.exe", "r")) != வெற்று)
    {
	fclose(fd);
	has_gvim = 1;
    }
    எனில் ((fd = fopen("vim.exe", "r")) != வெற்று)
    {
	fclose(fd);
	has_vim = 1;
    }
    எனில் (!has_gvim && !has_vim)
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
    நிலையான முழு
pathcmp(எழுத்து *p, முழு plen, எழுத்து *q, முழு qlen)
{
    முழு		i;

    எனில் (plen < 0)
	plen = strlen(p);
    எனில் (qlen < 0)
	qlen = strlen(q);
    க்கு (i = 0; ; ++i)
    {
	// End of "p": check if "q" also ends or just has a slash.
	எனில் (i == plen)
	{
	    எனில் (i == qlen)  // match
		திரும்பு 0;
	    எனில் (i == qlen - 1 && (q[i] == '\\' || q[i] == '/'))
		திரும்பு 0;   // match with trailing slash
	    திரும்பு 1;	    // no match
	}

	// End of "q": check if "p" also ends or just has a slash.
	எனில் (i == qlen)
	{
	    எனில் (i == plen)  // match
		திரும்பு 0;
	    எனில் (i == plen - 1 && (p[i] == '\\' || p[i] == '/'))
		திரும்பு 0;   // match with trailing slash
	    திரும்பு 1;	    // no match
	}

	எனில் (!(mytoupper(p[i]) == mytoupper(q[i])
		|| ((p[i] == '/' || p[i] == '\\')
		    && (q[i] == '/' || q[i] == '\\'))))
	    திரும்பு 1;	    // no match
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
    நிலையான வெறுமை
findoldfile(எழுத்து **destination)
{
    எழுத்து	*bp = *destination;
    size_t	indir_l = strlen(installdir);
    எழுத்து	*cp;
    எழுத்து	*tmpname;
    எழுத்து	*farname;

    /*
     * No action needed if exe not found or not in this directory.
     */
    எனில் (bp == வெற்று || strnicmp(bp, installdir, indir_l) != 0)
	திரும்பு;
    cp = bp + indir_l;
    எனில் (strchr("/\\", *cp++) == வெற்று
	    || strchr(cp, '\\') != வெற்று
	    || strchr(cp, '/') != வெற்று)
	திரும்பு;

    tmpname = alloc(strlen(cp) + 1);
    strcpy(tmpname, cp);
    tmpname[strlen(tmpname) - 1] = 'x';	// .exe -> .exx

    எனில் (access(tmpname, 0) == 0)
    {
	printf("\nERROR: %s and %s clash.  Remove or rename %s.\n",
	    tmpname, cp, tmpname);
	myexit(1);
    }

    எனில் (rename(cp, tmpname) != 0)
    {
	printf("\nERROR: failed to rename %s to %s: %s\n",
	    cp, tmpname, strerror(0));
	myexit(1);
    }

    farname = searchpath_save(cp);

    எனில் (rename(tmpname, cp) != 0)
    {
	printf("\nERROR: failed to rename %s back to %s: %s\n",
	    tmpname, cp, strerror(0));
	myexit(1);
    }

    விடுவி(*destination);
    விடுவி(tmpname);
    *destination = farname;
}

/*
 * Check if there is a vim.[exe|bat|, gvim.[exe|bat|, etc. in the path.
 * When "check_bat_only" is TRUE, only find "default_bat_dir".
 */
    நிலையான வெறுமை
find_bat_exe(முழு check_bat_only)
{
    முழு		i;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    க்கு (i = 1; i < TARGET_மொத்தம்; ++i)
    {
	targets[i].oldbat = searchpath_save(targets[i].batname);
	எனில் (!check_bat_only)
	    targets[i].oldexe = searchpath_save(targets[i].exename);

	எனில் (default_bat_dir == வெற்று && targets[i].oldbat != வெற்று)
	{
	    default_bat_dir = alloc(strlen(targets[i].oldbat) + 1);
	    strcpy(default_bat_dir, targets[i].oldbat);
	    remove_tail(default_bat_dir);
	}
	எனில் (check_bat_only && targets[i].oldbat != வெற்று)
	{
	    விடுவி(targets[i].oldbat);
	    targets[i].oldbat = வெற்று;
	}
    }

    mch_chdir(installdir);
}

/*
 * Get the value of $VIMRUNTIME or $VIM and write it in $TEMP/vimini.ini, so
 * that NSIS can read it.
 * When not set, use the directory of a previously installed Vim.
 */
    நிலையான வெறுமை
get_vim_env(வெறுமை)
{
    எழுத்து	*vim;
    எழுத்து	buf[BUFSIZE];
    FILE	*fd;
    எழுத்து	fname[BUFSIZE];

    // First get $VIMRUNTIME.  If it's set, remove the tail.
    vim = getenv("VIMRUNTIME");
    எனில் (vim != வெற்று && *vim != 0 && strlen(vim) < அளவு(buf))
    {
	strcpy(buf, vim);
	remove_tail(buf);
	vim = buf;
    }
    இல்லையெனில்
    {
	vim = getenv("VIM");
	எனில் (vim == வெற்று || *vim == 0)
	{
	    // Use the directory from an old uninstall entry.
	    எனில் (default_vim_dir != வெற்று)
		vim = default_vim_dir;
	    இல்லையெனில்
		// Let NSIS know there is no default, it should use
		// $PROGRAMFILES.
		vim = "";
	}
    }

    // NSIS also uses GetTempPath(), thus we should get the same directory
    // name as where NSIS will look for vimini.ini.
    GetTempPath(அளவு(fname) - 12, fname);
    add_pathsep(fname);
    strcat(fname, "vimini.ini");

    fd = fopen(fname, "w");
    எனில் (fd != வெற்று)
    {
	// Make it look like an .ini file, so that NSIS can read it with a
	// ReadINIStr command.
	fprintf(fd, "[vimini]\n");
	fprintf(fd, "dir=\"%s\"\n", vim);
	fclose(fd);
    }
    இல்லையெனில்
    {
	printf("Failed to open %s\n", fname);
	sleep(2);
    }
}

நிலையான முழு இலக்கம்_windows;

/*
 * Callback used for EnumWindows():
 * Count the window if the title looks like it is for the uninstaller.
 */
//ARGSUSED
    நிலையான BOOL CALLBACK
window_cb(HWND hwnd, LPARAM lparam UNUSED)
{
    எழுத்து title[256];

    title[0] = 0;
    GetWindowText(hwnd, title, 256);
    எனில் (strstr(title, "Vim ") != வெற்று && strstr(title, " Uninstall") != வெற்று)
	++இலக்கம்_windows;
    திரும்பு TRUE;
}

/*
 * Run the uninstaller silently.
 */
    நிலையான முழு
run_silent_uninstall(எழுத்து *uninst_exe)
{
    எழுத்து    vimrt_dir[BUFSIZE];
    எழுத்து    temp_uninst[BUFSIZE];
    எழுத்து    temp_dir[MAX_PATH];
    எழுத்து    buf[BUFSIZE * 2 + 10];
    முழு	    i;
    DWORD   tick;

    strcpy(vimrt_dir, uninst_exe);
    remove_tail(vimrt_dir);

    எனில் (!GetTempPath(அளவு(temp_dir), temp_dir))
	திரும்பு FAIL;

    // Copy the uninstaller to a temporary exe.
    tick = Get_Tick_மொத்தம்();
    க்கு (i = 0; ; i++)
    {
	sprintf(temp_uninst, "%s\\vimun%04X.exe", temp_dir,
					  (unsigned முழு)((i + tick) & 0xFFFF));
	எனில் (CopyFile(uninst_exe, temp_uninst, TRUE))
	    நிறுத்து;
	எனில் (GetLastError() != ERROR_FILE_EXISTS)
	    திரும்பு FAIL;
	எனில் (i == 65535)
	    திரும்பு FAIL;
    }

    // Run the copied uninstaller silently.
    எனில் (strchr(temp_uninst, ' ') != வெற்று)
	sprintf(buf, "\"%s\" /S _?=%s", temp_uninst, vimrt_dir);
    இல்லையெனில்
	sprintf(buf, "%s /S _?=%s", temp_uninst, vimrt_dir);
    run_command(buf);

    DeleteFile(temp_uninst);
    திரும்பு OK;
}

/*
 * Check for already installed Vims.
 * Return non-zero when found one.
 */
    நிலையான முழு
uninstall_check(முழு skip_question)
{
    HKEY	சாவி_handle;
    HKEY	uninstall_சாவி_handle;
    எழுத்து	*uninstall_சாவி = "software\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
    எழுத்து	subkey_name_buff[BUFSIZE];
    எழுத்து	temp_string_buffer[BUFSIZE-2];
    DWORD	local_bufsize;
    FILETIME	temp_pfiletime;
    DWORD	சாவி_index;
    எழுத்து	உள்ளீடு;
    long	code;
    DWORD	மதிப்பு_type;
    DWORD	orig_இலக்கம்_சாவிகள்;
    DWORD	new_இலக்கம்_சாவிகள்;
    DWORD	allow_silent;
    முழு		foundone = 0;

    code = Reg_Open_சாவி_Ex(HKEY_LOCAL_MACHINE, uninstall_சாவி, 0,
				     சாவி_WOW64_64_சாவி | சாவி_READ, &சாவி_handle);
    CHECK_REG_ERROR(code);

    சாவி_index = 0;
    போது (TRUE)
    {
	local_bufsize = அளவு(subkey_name_buff);
	எனில் (Reg_Enum_சாவி_Ex(சாவி_handle, சாவி_index, subkey_name_buff, &local_bufsize,
		வெற்று, வெற்று, வெற்று, &temp_pfiletime) == ERROR_NO_MORE_பொருட்கள்)
	    நிறுத்து;

	எனில் (strncmp("Vim", subkey_name_buff, 3) == 0)
	{
	    // Open the key named Vim*
	    code = Reg_Open_சாவி_Ex(சாவி_handle, subkey_name_buff, 0,
			   சாவி_WOW64_64_சாவி | சாவி_READ, &uninstall_சாவி_handle);
	    CHECK_REG_ERROR(code);

	    // get the DisplayName out of it to show the user
	    local_bufsize = அளவு(temp_string_buffer);
	    code = Reg_Query_மதிப்பு_Ex(uninstall_சாவி_handle, "displayname", 0,
		    &மதிப்பு_type, (LPBYTE)temp_string_buffer,
		    &local_bufsize);
	    CHECK_REG_ERROR(code);

	    allow_silent = 0;
	    எனில் (skip_question)
	    {
		DWORD varsize = அளவு(DWORD);

		Reg_Query_மதிப்பு_Ex(uninstall_சாவி_handle, "AllowSilent", 0,
			&மதிப்பு_type, (LPBYTE)&allow_silent,
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

	    எனில் (skip_question)
		printf("\nRunning uninstall program for \"%s\"\n", temp_string_buffer);
	    இல்லையெனில்
		printf("\nDo you want to uninstall \"%s\" now?\n(y)es/(n)o)  ", temp_string_buffer);
	    fflush(stdout);

	    // get the UninstallString
	    local_bufsize = அளவு(temp_string_buffer);
	    code = Reg_Query_மதிப்பு_Ex(uninstall_சாவி_handle, "uninstallstring", 0,
		    &மதிப்பு_type, (LPBYTE)temp_string_buffer, &local_bufsize);
	    CHECK_REG_ERROR(code);

	    // Remember the directory, it is used as the default for NSIS.
	    default_vim_dir = alloc(strlen(temp_string_buffer) + 1);
	    strcpy(default_vim_dir, temp_string_buffer);
	    remove_tail(default_vim_dir);
	    remove_tail(default_vim_dir);

	    உள்ளீடு = 'n';
	    செய்
	    {
		எனில் (உள்ளீடு != 'n')
		    printf("%c is an invalid reply.  Please enter either 'y' or 'n'\n", உள்ளீடு);

		எனில் (skip_question)
		    உள்ளீடு = 'y';
		இல்லையெனில்
		{
		    rewind(stdin);
		    scanf("%c", &உள்ளீடு);
		}
		தேர்வு (உள்ளீடு)
		{
		    நிலை 'y':
		    நிலை 'Y':
			// save the number of uninstall keys so we can know if
			// it changed
			Reg_Query_Info_சாவி(சாவி_handle, வெற்று, வெற்று, வெற்று,
					     &orig_இலக்கம்_சாவிகள், வெற்று, வெற்று, வெற்று,
						      வெற்று, வெற்று, வெற்று, வெற்று);

			// Find existing .bat files before deleting them.
			find_bat_exe(TRUE);

			எனில் (allow_silent)
			{
			    எனில் (run_silent_uninstall(temp_string_buffer)
								    == FAIL)
				allow_silent = 0; // Retry with non silent.
			}
			எனில் (!allow_silent)
			{
			    // Execute the uninstall program.  Put it in double
			    // quotes if there is an embedded space.
			    {
				எழுத்து buf[BUFSIZE];

				எனில் (strchr(temp_string_buffer, ' ') != வெற்று)
				    sprintf(buf, "\"%s\"", temp_string_buffer);
				இல்லையெனில்
				    strcpy(buf, temp_string_buffer);
				run_command(buf);
			    }

			    // Count the number of windows with a title that
			    // match the installer, so that we can check when
			    // it's done.  The uninstaller copies itself,
			    // executes the copy and exits, thus we can't wait
			    // for the process to finish.
			    sleep(1);  // wait for uninstaller to start up
			    இலக்கம்_windows = 0;
			    EnumWindows(window_cb, 0);
			    எனில் (இலக்கம்_windows == 0)
			    {
				// Did not find the uninstaller, ask user to
				// press Enter when done. Just in case.
				printf("Press Enter when the uninstaller is finished\n");
				rewind(stdin);
				(வெறுமை)getchar();
			    }
			    இல்லையெனில்
			    {
				printf("Waiting for the uninstaller to finish (press CTRL-C to abort).");
				செய்
				{
				    printf(".");
				    fflush(stdout);
				    sleep(1);	// wait for the uninstaller to
						// finish
				    இலக்கம்_windows = 0;
				    EnumWindows(window_cb, 0);
				} போது (இலக்கம்_windows > 0);
			    }
			}
			printf("\nDone!\n");

			// Check if an uninstall reg key was deleted.
			// if it was, we want to decrement key_index.
			// if we don't do this, we will skip the key
			// immediately after any key that we delete.
			Reg_Query_Info_சாவி(சாவி_handle, வெற்று, வெற்று, வெற்று,
					      &new_இலக்கம்_சாவிகள், வெற்று, வெற்று, வெற்று,
						      வெற்று, வெற்று, வெற்று, வெற்று);
			எனில் (new_இலக்கம்_சாவிகள் < orig_இலக்கம்_சாவிகள்)
			    சாவி_index--;

			உள்ளீடு = 'y';
			நிறுத்து;

		    நிலை 'n':
		    நிலை 'N':
			// Do not uninstall
			உள்ளீடு = 'n';
			நிறுத்து;

		    இயல்பு: // just drop through and redo the loop
			நிறுத்து;
		}

	    } போது (உள்ளீடு != 'n' && உள்ளீடு != 'y');

	    Reg_Close_சாவி(uninstall_சாவி_handle);
	}

	சாவி_index++;
    }
    Reg_Close_சாவி(சாவி_handle);

    திரும்பு foundone;
}

/*
 * Find out information about the system.
 */
    நிலையான வெறுமை
inspect_system(வெறுமை)
{
    எழுத்து	*p;
    எழுத்து	buf[BUFSIZE];
    FILE	*fd;
    முழு		i;
    முழு		foundone;

    // This may take a little while, let the user know what we're doing.
    printf("Inspecting system...\n");

    /*
     * If $VIM is set, check that it's pointing to our directory.
     */
    p = getenv("VIM");
    எனில் (p != வெற்று && pathcmp(p, -1, installdir, runtimeidx - 1) != 0)
    {
	printf("------------------------------------------------------\n");
	printf("$VIM is set to \"%s\".\n", p);
	printf("This is different from where this version of Vim is:\n");
	strcpy(buf, installdir);
	*(buf + runtimeidx - 1) = NUL;
	printf("\"%s\"\n", buf);
	printf("You must adjust or remove the setting of $VIM,\n");
	எனில் (interactive)
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
    எனில் (p != வெற்று && pathcmp(p, -1, installdir, -1) != 0)
    {
	printf("------------------------------------------------------\n");
	printf("$VIMRUNTIME is set to \"%s\".\n", p);
	printf("This is different from where this version of Vim is:\n");
	printf("\"%s\"\n", installdir);
	printf("You must adjust or remove the setting of $VIMRUNTIME,\n");
	எனில் (interactive)
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
    க்கு (i = 1; i < TARGET_மொத்தம்; ++i)
    {
	findoldfile(&(targets[i].oldexe));
	எனில் (targets[i].oldexe != வெற்று)
	    foundone = 1;
    }

    எனில் (foundone)
    {
	printf("Warning: Found Vim executable(s) in your $PATH:\n");
	க்கு (i = 1; i < TARGET_மொத்தம்; ++i)
	    எனில் (targets[i].oldexe != வெற்று)
		printf("%s\n", targets[i].oldexe);
	printf("It will be used instead of the version you are installing.\n");
	printf("Please delete or rename it, or adjust your $PATH setting.\n");
    }

    /*
     * Check if there is an existing ../_vimrc or ../.vimrc file.
     */
    strcpy(oldvimrc, installdir);
    strcpy(oldvimrc + runtimeidx, "_vimrc");
    எனில் ((fd = fopen(oldvimrc, "r")) == வெற்று)
    {
	strcpy(oldvimrc + runtimeidx, "vimrc~1"); // short version of .vimrc
	எனில் ((fd = fopen(oldvimrc, "r")) == வெற்று)
	{
	    strcpy(oldvimrc + runtimeidx, ".vimrc");
	    fd = fopen(oldvimrc, "r");
	}
    }
    எனில் (fd != வெற்று)
	fclose(fd);
    இல்லையெனில்
	*oldvimrc = NUL;
}

/*
 * Add a dummy choice to avoid that the numbering changes depending on items
 * in the environment.  The user may type a number he remembered without
 * looking.
 */
    நிலையான வெறுமை
add_dummy_choice(வெறுமை)
{
    choices[choice_மொத்தம்].installfunc = வெற்று;
    choices[choice_மொத்தம்].active = 0;
    choices[choice_மொத்தம்].changefunc = வெற்று;
    choices[choice_மொத்தம்].text = வெற்று;
    choices[choice_மொத்தம்].arg = 0;
    ++choice_மொத்தம்;
}

////////////////////////////////////////////////
// stuff for creating the batch files.

/*
 * Install the vim.bat, gvim.bat, etc. files.
 */
    நிலையான வெறுமை
install_bat_choice(முழு idx)
{
    எழுத்து	*batpath = targets[choices[idx].arg].batpath;
    எழுத்து	*oldname = targets[choices[idx].arg].oldbat;
    எழுத்து	*exename = targets[choices[idx].arg].exenamearg;
    எழுத்து	*vimarg = targets[choices[idx].arg].exearg;
    FILE	*fd;

    எனில் (*batpath == NUL)
	திரும்பு;

    fd = fopen(batpath, "w");
    எனில் (fd == வெற்று)
    {
	printf("\nERROR: Cannot open \"%s\" for writing.\n", batpath);
	திரும்பு;
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

    எனில் (*exename == 'g')
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

    எனில் (*exename == 'g')
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
    இல்லையெனில்
    {
	// Always use quotes, $VIM or $VIMRUNTIME might have a space.
	fprintf(fd, "\"%%VIM_EXE_DIR%%\\%s\" %s %%*\n",
		exename, vimarg);
    }

    fclose(fd);
    printf("%s has been %s\n", batpath,
	    oldname == வெற்று ? "created" : "overwritten");
}

/*
 * Make the text string for choice "idx".
 * The format "fmt" is must have one %s item, which "arg" is used for.
 */
    நிலையான வெறுமை
alloc_text(முழு idx, எழுத்து *fmt, எழுத்து *arg)
{
    எனில் (choices[idx].text != வெற்று)
	விடுவி(choices[idx].text);

    choices[idx].text = alloc(strlen(fmt) + strlen(arg) - 1);
    sprintf(choices[idx].text, fmt, arg);
}

/*
 * Toggle the "Overwrite .../vim.bat" to "Don't overwrite".
 */
    நிலையான வெறுமை
toggle_bat_choice(முழு idx)
{
    எழுத்து	*batname = targets[choices[idx].arg].batpath;
    எழுத்து	*oldname = targets[choices[idx].arg].oldbat;

    எனில் (*batname == NUL)
    {
	alloc_text(idx, "    Overwrite %s", oldname);
	strcpy(batname, oldname);
    }
    இல்லையெனில்
    {
	alloc_text(idx, "    Do NOT overwrite %s", oldname);
	*batname = NUL;
    }
}

/*
 * Do some work for a batch file entry: Append the batch file name to the path
 * and set the text for the choice.
 */
    நிலையான வெறுமை
set_bat_text(முழு idx, எழுத்து *batpath, எழுத்து *name)
{
    strcat(batpath, name);

    alloc_text(idx, "    Create %s", batpath);
}

/*
 * Select a directory to write the batch file line.
 */
    நிலையான வெறுமை
change_bat_choice(முழு idx)
{
    எழுத்து	*path;
    எழுத்து	*batpath;
    எழுத்து	*name;
    முழு		n;
    எழுத்து	*s;
    எழுத்து	*p;
    முழு		மொத்தம்;
    எழுத்து	**names = வெற்று;
    முழு		i;
    முழு		target = choices[idx].arg;

    name = targets[target].batname;
    batpath = targets[target].batpath;

    path = getenv("PATH");
    எனில் (path == வெற்று)
    {
	printf("\nERROR: The variable $PATH is not set\n");
	திரும்பு;
    }

    /*
     * first round: count number of names in path;
     * second round: save names to names[].
     */
    க்கு (;;)
    {
	மொத்தம் = 1;
	க்கு (p = path; *p; )
	{
	    s = strchr(p, ';');
	    எனில் (s == வெற்று)
		s = p + strlen(p);
	    எனில் (names != வெற்று)
	    {
		names[மொத்தம்] = alloc(s - p + 1);
		strncpy(names[மொத்தம்], p, s - p);
		names[மொத்தம்][s - p] = NUL;
	    }
	    ++மொத்தம்;
	    p = s;
	    எனில் (*p != NUL)
		++p;
	}
	எனில் (names != வெற்று)
	    நிறுத்து;
	names = alloc((மொத்தம் + 1) * அளவு(எழுத்து *));
    }
    names[0] = alloc(50);
    sprintf(names[0], "Select directory to create %s in:", name);
    names[மொத்தம்] = alloc(50);
    எனில் (choices[idx].arg == 0)
	sprintf(names[மொத்தம்], "Do not create any .bat file.");
    இல்லையெனில்
	sprintf(names[மொத்தம்], "Do not create a %s file.", name);
    n = get_choice(names, மொத்தம் + 1);

    எனில் (n == மொத்தம்)
    {
	// Selected last item, don't create bat file.
	*batpath = NUL;
	எனில் (choices[idx].arg != 0)
	    alloc_text(idx, "    Do NOT create %s", name);
    }
    இல்லையெனில்
    {
	// Selected one of the paths.  For the first item only keep the path,
	// for the others append the batch file name.
	strcpy(batpath, names[n]);
	add_pathsep(batpath);
	எனில் (choices[idx].arg != 0)
	    set_bat_text(idx, batpath, name);
    }

    க்கு (i = 0; i <= மொத்தம்; ++i)
	விடுவி(names[i]);
    விடுவி(names);
}

எழுத்து *bat_text_yes = "Install .bat files to use Vim at the command line:";
எழுத்து *bat_text_no = "do NOT install .bat files to use Vim at the command line";

    நிலையான வெறுமை
change_main_bat_choice(முழு idx)
{
    முழு		i;

    // let the user select a default directory or NONE
    change_bat_choice(idx);

    எனில் (targets[0].batpath[0] != NUL)
	choices[idx].text = bat_text_yes;
    இல்லையெனில்
	choices[idx].text = bat_text_no;

    // update the individual batch file selections
    க்கு (i = 1; i < TARGET_மொத்தம்; ++i)
    {
	// Only make it active when the first item has a path and the vim.exe
	// or gvim.exe exists (there is a changefunc then).
	எனில் (targets[0].batpath[0] != NUL
		&& choices[idx + i].changefunc != வெற்று)
	{
	    choices[idx + i].active = 1;
	    எனில் (choices[idx + i].changefunc == change_bat_choice
		    && targets[i].batpath[0] != NUL)
	    {
		strcpy(targets[i].batpath, targets[0].batpath);
		set_bat_text(idx + i, targets[i].batpath, targets[i].batname);
	    }
	}
	இல்லையெனில்
	    choices[idx + i].active = 0;
    }
}

/*
 * Initialize a choice for creating a batch file.
 */
    நிலையான வெறுமை
init_bat_choice(முழு target)
{
    எழுத்து	*batpath = targets[target].batpath;
    எழுத்து	*oldbat = targets[target].oldbat;
    எழுத்து	*p;
    முழு		i;

    choices[choice_மொத்தம்].arg = target;
    choices[choice_மொத்தம்].installfunc = install_bat_choice;
    choices[choice_மொத்தம்].active = 1;
    choices[choice_மொத்தம்].text = வெற்று;	// will be set below
    எனில் (oldbat != வெற்று)
    {
	// A [g]vim.bat exists: Only choice is to overwrite it or not.
	choices[choice_மொத்தம்].changefunc = toggle_bat_choice;
	*batpath = NUL;
	toggle_bat_choice(choice_மொத்தம்);
    }
    இல்லையெனில்
    {
	எனில் (default_bat_dir != வெற்று)
	    // Prefer using the same path as an existing .bat file.
	    strcpy(batpath, default_bat_dir);
	இல்லையெனில்
	{
	    // No [g]vim.bat exists: Write it to a directory in $PATH.  Use
	    // $WINDIR by default, if it's empty the first item in $PATH.
	    p = getenv("WINDIR");
	    எனில் (p != வெற்று && *p != NUL)
		strcpy(batpath, p);
	    இல்லையெனில்
	    {
		p = getenv("PATH");
		எனில் (p == வெற்று || *p == NUL)		// "cannot happen"
		    strcpy(batpath, "C:/Windows");
		இல்லையெனில்
		{
		    i = 0;
		    போது (*p != NUL && *p != ';')
			batpath[i++] = *p++;
		    batpath[i] = NUL;
		}
	    }
	}
	add_pathsep(batpath);
	set_bat_text(choice_மொத்தம், batpath, targets[target].batname);

	choices[choice_மொத்தம்].changefunc = change_bat_choice;
    }
    ++choice_மொத்தம்;
}

/*
 * Set up the choices for installing .bat files.
 * For these items "arg" is the index in targets[].
 */
    நிலையான வெறுமை
init_bat_choices(வெறுமை)
{
    முழு		i;

    // The first item is used to switch installing batch files on/off and
    // setting the default path.
    choices[choice_மொத்தம்].text = bat_text_yes;
    choices[choice_மொத்தம்].changefunc = change_main_bat_choice;
    choices[choice_மொத்தம்].installfunc = வெற்று;
    choices[choice_மொத்தம்].active = 1;
    choices[choice_மொத்தம்].arg = 0;
    ++choice_மொத்தம்;

    // Add items for each batch file target.  Only used when not disabled by
    // the first item.  When a .exe exists, don't offer to create a .bat.
    க்கு (i = 1; i < TARGET_மொத்தம்; ++i)
	எனில் (targets[i].oldexe == வெற்று
		&& (targets[i].exenamearg[0] == 'g' ? has_gvim : has_vim))
	    init_bat_choice(i);
	இல்லையெனில்
	    add_dummy_choice();
}

/*
 * Install the vimrc file.
 */
    நிலையான வெறுமை
install_vimrc(முழு idx UNUSED)
{
    FILE	*fd, *tfd;
    எழுத்து	*fname;

    // If an old vimrc file exists, overwrite it.
    // Otherwise create a new one.
    எனில் (*oldvimrc != NUL)
	fname = oldvimrc;
    இல்லையெனில்
	fname = vimrc;

    fd = fopen(fname, "w");
    எனில் (fd == வெற்று)
    {
	printf("\nERROR: Cannot open \"%s\" for writing.\n", fname);
	திரும்பு;
    }
    தேர்வு (compat_choice)
    {
	நிலை compat_vi:
		    fprintf(fd, "\" Vi compatible\n");
		    fprintf(fd, "set compatible\n");
		    நிறுத்து;
	நிலை compat_vim:
		    fprintf(fd, "\" Vim's default behavior\n");
		    fprintf(fd, "if &compatible\n");
		    fprintf(fd, "  set nocompatible\n");
		    fprintf(fd, "endif\n");
		    நிறுத்து;
	நிலை compat_some_enhancements:
		    fprintf(fd, "\" Vim with some enhancements\n");
		    fprintf(fd, "source $VIMRUNTIME/defaults.vim\n");
		    நிறுத்து;
	நிலை compat_all_enhancements:
		    fprintf(fd, "\" Vim with all enhancements\n");
		    fprintf(fd, "source $VIMRUNTIME/vimrc_example.vim\n");
		    நிறுத்து;
    }
    தேர்வு (remap_choice)
    {
	நிலை remap_no:
		    நிறுத்து;
	நிலை remap_win:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Remap a few keys for Windows behavior\n");
		    fprintf(fd, "source $VIMRUNTIME/mswin.vim\n");
		    நிறுத்து;
    }
    தேர்வு (mouse_choice)
    {
	நிலை mouse_xterm:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Mouse behavior (the Unix way)\n");
		    fprintf(fd, "behave xterm\n");
		    நிறுத்து;
	நிலை mouse_mswin:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Mouse behavior (the Windows way)\n");
		    fprintf(fd, "behave mswin\n");
		    நிறுத்து;
	நிலை mouse_default:
		    நிறுத்து;
    }
    எனில் ((tfd = fopen("diff.exe", "r")) != வெற்று)
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

    நிலையான வெறுமை
change_vimrc_choice(முழு idx)
{
    எனில் (choices[idx].installfunc != வெற்று)
    {
	// Switch to NOT change or create a vimrc file.
	எனில் (*oldvimrc != NUL)
	    alloc_text(idx, "Do NOT change startup file %s", oldvimrc);
	இல்லையெனில்
	    alloc_text(idx, "Do NOT create startup file %s", vimrc);
	choices[idx].installfunc = வெற்று;
	choices[idx + 1].active = 0;
	choices[idx + 2].active = 0;
	choices[idx + 3].active = 0;
    }
    இல்லையெனில்
    {
	// Switch to change or create a vimrc file.
	எனில் (*oldvimrc != NUL)
	    alloc_text(idx, "Overwrite startup file %s with:", oldvimrc);
	இல்லையெனில்
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
    நிலையான வெறுமை
change_run_choice(முழு idx)
{
    compat_choice = get_choice(compat_choices, TABLE_SIZE(compat_choices));
    alloc_text(idx, compat_text, compat_choices[compat_choice]);
}

/*
 * Change the choice if keys are to be remapped.
 */
    நிலையான வெறுமை
change_remap_choice(முழு idx)
{
    remap_choice = get_choice(remap_choices, TABLE_SIZE(remap_choices));
    alloc_text(idx, remap_text, remap_choices[remap_choice]);
}

/*
 * Change the choice how to select text.
 */
    நிலையான வெறுமை
change_mouse_choice(முழு idx)
{
    mouse_choice = get_choice(mouse_choices, TABLE_SIZE(mouse_choices));
    alloc_text(idx, mouse_text, mouse_choices[mouse_choice]);
}

    நிலையான வெறுமை
init_vimrc_choices(வெறுமை)
{
    // set path for a new _vimrc file (also when not used)
    strcpy(vimrc, installdir);
    strcpy(vimrc + runtimeidx, "_vimrc");

    // Set opposite value and then toggle it by calling change_vimrc_choice()
    எனில் (*oldvimrc == NUL)
	choices[choice_மொத்தம்].installfunc = வெற்று;
    இல்லையெனில்
	choices[choice_மொத்தம்].installfunc = install_vimrc;
    choices[choice_மொத்தம்].text = வெற்று;
    change_vimrc_choice(choice_மொத்தம்);
    choices[choice_மொத்தம்].changefunc = change_vimrc_choice;
    choices[choice_மொத்தம்].active = 1;
    ++choice_மொத்தம்;

    // default way to run Vim
    alloc_text(choice_மொத்தம், compat_text, compat_choices[compat_choice]);
    choices[choice_மொத்தம்].changefunc = change_run_choice;
    choices[choice_மொத்தம்].installfunc = வெற்று;
    choices[choice_மொத்தம்].active = (*oldvimrc == NUL);
    ++choice_மொத்தம்;

    // Whether to remap keys
    alloc_text(choice_மொத்தம், remap_text , remap_choices[remap_choice]);
    choices[choice_மொத்தம்].changefunc = change_remap_choice;
    choices[choice_மொத்தம்].installfunc = வெற்று;
    choices[choice_மொத்தம்].active = (*oldvimrc == NUL);
    ++choice_மொத்தம்;

    // default way to use the mouse
    alloc_text(choice_மொத்தம், mouse_text, mouse_choices[mouse_choice]);
    choices[choice_மொத்தம்].changefunc = change_mouse_choice;
    choices[choice_மொத்தம்].installfunc = வெற்று;
    choices[choice_மொத்தம்].active = (*oldvimrc == NUL);
    ++choice_மொத்தம்;
}

    நிலையான LONG
reg_create_சாவி(
    HKEY வேர்,
    மாறிலி எழுத்து *subkey,
    PHKEY ph_சாவி,
    DWORD flag)
{
    DWORD disp;

    *ph_சாவி = வெற்று;
    திரும்பு Reg_Create_சாவி_Ex(
		வேர், subkey,
		0, வெற்று, REG_OPTION_NON_VOLATILE,
		flag | சாவி_WRITE,
		வெற்று, ph_சாவி, &disp);
}

    நிலையான LONG
reg_set_string_மதிப்பு(
    HKEY h_சாவி,
    மாறிலி எழுத்து *மதிப்பு_name,
    மாறிலி எழுத்து *data)
{
    திரும்பு Reg_Set_மதிப்பு_Ex(h_சாவி, மதிப்பு_name, 0, REG_SZ,
				     (LPBYTE)data, (DWORD)(1 + strlen(data)));
}

    நிலையான LONG
reg_create_சாவி_and_மதிப்பு(
    HKEY h_வேர்_சாவி,
    மாறிலி எழுத்து *subkey,
    மாறிலி எழுத்து *மதிப்பு_name,
    மாறிலி எழுத்து *data,
    DWORD flag)
{
    HKEY h_சாவி;
    LONG lRet = reg_create_சாவி(h_வேர்_சாவி, subkey, &h_சாவி, flag);

    எனில் (ERROR_SUCCESS == lRet)
    {
	lRet = reg_set_string_மதிப்பு(h_சாவி, மதிப்பு_name, data);
	Reg_Close_சாவி(h_சாவி);
    }
    திரும்பு lRet;
}

    நிலையான LONG
register_inproc_server(
    HKEY h_வேர்_சாவி,
    மாறிலி எழுத்து *clsid,
    மாறிலி எழுத்து *extname,
    மாறிலி எழுத்து *module,
    மாறிலி எழுத்து *threading_model,
    DWORD flag)
{
    CHAR subkey[BUFSIZE];
    LONG lRet;

    sprintf(subkey, "CLSID\\%s", clsid);
    lRet = reg_create_சாவி_and_மதிப்பு(h_வேர்_சாவி, subkey, வெற்று, extname, flag);
    எனில் (ERROR_SUCCESS == lRet)
    {
	sprintf(subkey, "CLSID\\%s\\InProcServer32", clsid);
	lRet = reg_create_சாவி_and_மதிப்பு(h_வேர்_சாவி, subkey, வெற்று, module, flag);
	எனில் (ERROR_SUCCESS == lRet)
	{
	    lRet = reg_create_சாவி_and_மதிப்பு(h_வேர்_சாவி, subkey,
					   "ThreadingModel", threading_model, flag);
	}
    }
    திரும்பு lRet;
}

    நிலையான LONG
register_shellex(
    HKEY h_வேர்_சாவி,
    மாறிலி எழுத்து *clsid,
    மாறிலி எழுத்து *name,
    மாறிலி எழுத்து *exe_path,
    DWORD flag)
{
    LONG lRet = reg_create_சாவி_and_மதிப்பு(
	    h_வேர்_சாவி,
	    "*\\shellex\\ContextMenuHandlers\\gvim",
	    வெற்று,
	    clsid,
	    flag);

    எனில் (ERROR_SUCCESS == lRet)
    {
	lRet = reg_create_சாவி_and_மதிப்பு(
		HKEY_LOCAL_MACHINE,
		"Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved",
		clsid,
		name,
		flag);

	எனில் (ERROR_SUCCESS == lRet)
	{
	    lRet = reg_create_சாவி_and_மதிப்பு(
		    HKEY_LOCAL_MACHINE,
		    "Software\\Vim\\Gvim",
		    "path",
		    exe_path,
		    flag);
	}
    }
    திரும்பு lRet;
}

    நிலையான LONG
register_openwith(
    HKEY h_வேர்_சாவி,
    மாறிலி எழுத்து *exe_path,
    DWORD flag)
{
    எழுத்து	exe_cmd[BUFSIZE];
    LONG	lRet;

    sprintf(exe_cmd, "\"%s\" \"%%1\"", exe_path);
    lRet = reg_create_சாவி_and_மதிப்பு(
	    h_வேர்_சாவி,
	    "Applications\\gvim.exe\\shell\\edit\\command",
	    வெற்று,
	    exe_cmd,
	    flag);

    எனில் (ERROR_SUCCESS == lRet)
    {
	முழு i;
	நிலையான மாறிலி எழுத்து *openwith[] = {
		".htm\\OpenWithList\\gvim.exe",
		".vim\\OpenWithList\\gvim.exe",
		"*\\OpenWithList\\gvim.exe",
	};

	க்கு (i = 0; ERROR_SUCCESS == lRet && i < (முழு)ARRAYSIZE(openwith); i++)
	    lRet = reg_create_சாவி_and_மதிப்பு(h_வேர்_சாவி, openwith[i], வெற்று, "", flag);
    }

    திரும்பு lRet;
}

    நிலையான LONG
register_uninstall(
    HKEY h_வேர்_சாவி,
    மாறிலி எழுத்து *appname,
    மாறிலி எழுத்து *display_name,
    மாறிலி எழுத்து *uninstall_string,
    மாறிலி எழுத்து *display_icon,
    மாறிலி எழுத்து *display_version,
    மாறிலி எழுத்து *publisher)
{
    LONG lRet = reg_create_சாவி_and_மதிப்பு(h_வேர்_சாவி, appname,
			     "DisplayName", display_name, சாவி_WOW64_64_சாவி);

    எனில் (ERROR_SUCCESS == lRet)
	lRet = reg_create_சாவி_and_மதிப்பு(h_வேர்_சாவி, appname,
		     "UninstallString", uninstall_string, சாவி_WOW64_64_சாவி);
    எனில் (ERROR_SUCCESS == lRet)
	lRet = reg_create_சாவி_and_மதிப்பு(h_வேர்_சாவி, appname,
		     "DisplayIcon", display_icon, சாவி_WOW64_64_சாவி);
    எனில் (ERROR_SUCCESS == lRet)
	lRet = reg_create_சாவி_and_மதிப்பு(h_வேர்_சாவி, appname,
		     "DisplayVersion", display_version, சாவி_WOW64_64_சாவி);
    எனில் (ERROR_SUCCESS == lRet)
	lRet = reg_create_சாவி_and_மதிப்பு(h_வேர்_சாவி, appname,
		     "Publisher", publisher, சாவி_WOW64_64_சாவி);
    திரும்பு lRet;
}

/*
 * Add some entries to the registry:
 * - to add "Edit with Vim" to the context * menu
 * - to add Vim to the "Open with..." list
 * - to uninstall Vim
 */
//ARGSUSED
    நிலையான முழு
install_registry(வெறுமை)
{
    LONG	lRet = ERROR_SUCCESS;
    மாறிலி எழுத்து	*vim_ext_ThreadingModel = "Apartment";
    மாறிலி எழுத்து	*vim_ext_name = "Vim Shell Extension";
    மாறிலி எழுத்து	*vim_ext_clsid = "{51EEE242-AD87-11d3-9C1E-0090278BBD99}";
    எழுத்து	vim_exe_path[MAX_PATH];
    எழுத்து	display_name[BUFSIZE];
    எழுத்து	uninstall_string[BUFSIZE];
    எழுத்து	icon_string[BUFSIZE];
    எழுத்து	version_string[BUFSIZE];
    முழு		i;
    முழு		loop_மொத்தம் = is_64bit_os() ? 2 : 1;
    DWORD	flag;

    sprintf(vim_exe_path, "%s\\gvim.exe", installdir);

    எனில் (install_popup)
    {
	எழுத்து	    bufg[BUFSIZE];

	printf("Creating \"Edit with Vim\" popup menu entry\n");

	க்கு (i = 0; i < loop_மொத்தம்; i++)
	{
	    எனில் (i == 0)
	    {
		sprintf(bufg, "%s\\" GVIMEXT32_PATH, installdir);
		flag = சாவி_WOW64_32_சாவி;
	    }
	    இல்லையெனில்
	    {
		sprintf(bufg, "%s\\" GVIMEXT64_PATH, installdir);
		flag = சாவி_WOW64_64_சாவி;
	    }

	    lRet = register_inproc_server(
		    HKEY_CLASSES_வேர், vim_ext_clsid, vim_ext_name,
		    bufg, vim_ext_ThreadingModel, flag);
	    எனில் (ERROR_SUCCESS != lRet)
		திரும்பு FAIL;
	    lRet = register_shellex(
		    HKEY_CLASSES_வேர், vim_ext_clsid, vim_ext_name,
		    vim_exe_path, flag);
	    எனில் (ERROR_SUCCESS != lRet)
		திரும்பு FAIL;
	}
    }

    எனில் (install_openwith)
    {
	printf("Creating \"Open with ...\" list entry\n");

	க்கு (i = 0; i < loop_மொத்தம்; i++)
	{
	    எனில் (i == 0)
		flag = சாவி_WOW64_32_சாவி;
	    இல்லையெனில்
		flag = சாவி_WOW64_64_சாவி;

	    lRet = register_openwith(HKEY_CLASSES_வேர், vim_exe_path, flag);
	    எனில் (ERROR_SUCCESS != lRet)
		திரும்பு FAIL;
	}
    }

    printf("Creating an uninstall entry\n");
    sprintf(display_name, "Vim " VIM_VERSION_SHORT
#ifdef _M_ARM64
	    " (arm64)"
#அல்லதெனில் _M_X64
	    " (x64)"
#endif
	    );

    // For the NSIS installer use the generated uninstaller.
    எனில் (interactive)
	sprintf(uninstall_string, "%s\\uninstall.exe", installdir);
    இல்லையெனில்
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
    எனில் (ERROR_SUCCESS != lRet)
	திரும்பு FAIL;

    திரும்பு OK;
}

    நிலையான வெறுமை
change_popup_choice(முழு idx)
{
    எனில் (install_popup == 0)
    {
	choices[idx].text = "Install an entry for Vim in the popup menu for the right\n    mouse button so that you can edit any file with Vim";
	install_popup = 1;
    }
    இல்லையெனில்
    {
	choices[idx].text = "Do NOT install an entry for Vim in the popup menu for the\n    right mouse button to edit any file with Vim";
	install_popup = 0;
    }
}

/*
 * Only add the choice for the popup menu entry when gvim.exe was found and
 * both gvimext.dll and regedit.exe exist.
 */
    நிலையான வெறுமை
init_popup_choice(வெறுமை)
{
    கட்டமைப்பு stat	st;

    எனில் (has_gvim
	    && (stat(GVIMEXT32_PATH, &st) >= 0
		|| stat(GVIMEXT64_PATH, &st) >= 0))
    {
	choices[choice_மொத்தம்].changefunc = change_popup_choice;
	choices[choice_மொத்தம்].installfunc = வெற்று;
	choices[choice_மொத்தம்].active = 1;
	change_popup_choice(choice_மொத்தம்);  // set the text
	++choice_மொத்தம்;
    }
    இல்லையெனில்
	add_dummy_choice();
}

    நிலையான வெறுமை
change_openwith_choice(முழு idx)
{
    எனில் (install_openwith == 0)
    {
	choices[idx].text = "Add Vim to the \"Open With...\" list in the popup menu for the right\n    mouse button so that you can edit any file with Vim";
	install_openwith = 1;
    }
    இல்லையெனில்
    {
	choices[idx].text = "Do NOT add Vim to the \"Open With...\" list in the popup menu for the\n    right mouse button to edit any file with Vim";
	install_openwith = 0;
    }
}

/*
 * Only add the choice for the open-with menu entry when gvim.exe was found
 * and regedit.exe exist.
 */
    நிலையான வெறுமை
init_openwith_choice(வெறுமை)
{
    எனில் (has_gvim)
    {
	choices[choice_மொத்தம்].changefunc = change_openwith_choice;
	choices[choice_மொத்தம்].installfunc = வெற்று;
	choices[choice_மொத்தம்].active = 1;
	change_openwith_choice(choice_மொத்தம்);  // set the text
	++choice_மொத்தம்;
    }
    இல்லையெனில்
	add_dummy_choice();
}

/*
 * Create a shell link.
 *
 * returns 0 on failure, non-zero on successful completion.
 *
 * NOTE:  Currently untested with mingw.
 */
    முழு
create_shortcut(
	மாறிலி எழுத்து *shortcut_name,
	மாறிலி எழுத்து *iconfile_path,
	முழு	    iconindex,
	மாறிலி எழுத்து *shortcut_target,
	மாறிலி எழுத்து *shortcut_args,
	மாறிலி எழுத்து *workingdir
	)
{
    IShellLink	    *shelllink_ptr;
    HRESULT	    hres;
    IPersistFile	    *persistfile_ptr;

    // Initialize COM library
    hres = CoInitialize(வெற்று);
    எனில் (!SUCCEEDED(hres))
    {
	printf("Error:  Could not open the COM library.  Not creating shortcut.\n");
	திரும்பு FAIL;
    }

    // Instantiate a COM object for the ShellLink, store a pointer to it
    // in shelllink_ptr.
    hres = CoCreateInstance(&CLSID_ShellLink,
			   வெற்று,
			   CLSCTX_INPROC_SERVER,
			   &IID_IShellLink,
			   (வெறுமை **) &shelllink_ptr);

    எனில் (SUCCEEDED(hres)) // If the instantiation was successful...
    {
	// ...Then build a PersistFile interface for the ShellLink so we can
	// save it as a file after we build it.
	hres = shelllink_ptr->lpVtbl->QueryInterface(shelllink_ptr,
		&IID_IPersistFile, (வெறுமை **) &persistfile_ptr);

	எனில் (SUCCEEDED(hres))
	{
	    wchar_t wsz[BUFSIZE];

	    // translate the (possibly) multibyte shortcut filename to windows
	    // Unicode so it can be used as a file name.
	    MultiByteToWideChar(CP_ACP, 0, shortcut_name, -1, wsz, அளவு(wsz)/அளவு(wsz[0]));

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
	இல்லையெனில்
	{
	    printf("QueryInterface Error\n");
	    திரும்பு FAIL;
	}

	// Return the ShellLink object
	shelllink_ptr->lpVtbl->Release(shelllink_ptr);
    }
    இல்லையெனில்
    {
	printf("CoCreateInstance Error - hres = %08x\n", (முழு)hres);
	திரும்பு FAIL;
    }

    திரும்பு OK;
}

/*
 * Build a path to where we will put a specified link.
 *
 * Return 0 on error, non-zero on success
 */
   முழு
build_link_name(
	எழுத்து	   *link_path,
	மாறிலி எழுத்து *link_name,
	மாறிலி எழுத்து *shell_folder_name)
{
    எழுத்து	shell_folder_path[MAX_PATH];

    எனில் (get_shell_folder_path(shell_folder_path, shell_folder_name) == FAIL)
    {
	printf("An error occurred while attempting to find the path to %s.\n",
							   shell_folder_name);
	திரும்பு FAIL;
    }

    // Make sure the directory exists (create Start Menu\Programs\Vim).
    // Ignore errors if it already exists.
    vim_mkdir(shell_folder_path, 0755);

    // build the path to the shortcut and the path to gvim.exe
    sprintf(link_path, "%s\\%s.lnk", shell_folder_path, link_name);

    திரும்பு OK;
}

    நிலையான முழு
build_shortcut(
	மாறிலி எழுத்து *name,	// Name of the shortcut
	மாறிலி எழுத்து *exename,	// Name of the executable (e.g., vim.exe)
	மாறிலி எழுத்து *args,
	மாறிலி எழுத்து *shell_folder,
	மாறிலி எழுத்து *workingdir)
{
    எழுத்து	executable_path[BUFSIZE];
    எழுத்து	link_name[BUFSIZE];

    sprintf(executable_path, "%s\\%s", installdir, exename);

    எனில் (build_link_name(link_name, name, shell_folder) == FAIL)
    {
	printf("An error has occurred.  A shortcut to %s will not be created %s.\n",
		name,
		*shell_folder == 'd' ? "on the desktop" : "in the Start menu");
	திரும்பு FAIL;
    }

    // Create the shortcut:
    திரும்பு create_shortcut(link_name, executable_path, 0,
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
    நிலையான வெறுமை
install_start_menu(முழு idx UNUSED)
{
    need_uninstall_entry = 1;
    printf("Creating start menu\n");
    எனில் (has_vim)
    {
	எனில் (build_shortcut("Vim", "vim.exe", "",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    திரும்பு;
	எனில் (build_shortcut("Vim Read-only", "vim.exe", "-R",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    திரும்பு;
	எனில் (build_shortcut("Vim Diff", "vim.exe", "-d",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    திரும்பு;
    }
    எனில் (has_gvim)
    {
	எனில் (build_shortcut("gVim", "gvim.exe", "",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    திரும்பு;
	எனில் (build_shortcut("gVim Easy", "gvim.exe", "-y",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    திரும்பு;
	எனில் (build_shortcut("gVim Read-only", "gvim.exe", "-R",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    திரும்பு;
	எனில் (build_shortcut("gVim Diff", "gvim.exe", "-d",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    திரும்பு;
    }
    எனில் (build_shortcut("Uninstall",
		interactive ? "uninstall.exe" : "uninstall-gui.exe", "",
					   VIM_STARTMENU, installdir) == FAIL)
	திரும்பு;
    // For Windows NT the working dir of the vimtutor.bat must be right,
    // otherwise gvim.exe won't be found and using gvimbat doesn't work.
    எனில் (build_shortcut("Vim tutor", "vimtutor.bat", "",
					   VIM_STARTMENU, installdir) == FAIL)
	திரும்பு;
    எனில் (build_shortcut("Help", has_gvim ? "gvim.exe" : "vim.exe", "-c h",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	திரும்பு;
    {
	எழுத்து	shell_folder_path[BUFSIZE];

	// Creating the URL shortcut works a bit differently...
	எனில் (get_shell_folder_path(shell_folder_path, VIM_STARTMENU) == FAIL)
	{
	    printf("Finding the path of the Start menu failed\n");
	    திரும்பு ;
	}
	add_pathsep(shell_folder_path);
	strcat(shell_folder_path, "Vim Online.url");
	எனில் (!WritePrivateProfileString("InternetShortcut", "URL",
				    "https://www.vim.org/", shell_folder_path))
	{
	    printf("Creating the Vim online URL failed\n");
	    திரும்பு;
	}
    }
}

    நிலையான வெறுமை
toggle_startmenu_choice(முழு idx)
{
    எனில் (choices[idx].installfunc == வெற்று)
    {
	choices[idx].installfunc = install_start_menu;
	choices[idx].text = "Add Vim to the Start menu";
    }
    இல்லையெனில்
    {
	choices[idx].installfunc = வெற்று;
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
    வெறுமை
install_shortcut_gvim(முழு idx)
{
    // Create shortcut(s) on the desktop
    எனில் (choices[idx].arg)
    {
	(வெறுமை)build_shortcut(icon_names[0], "gvim.exe",
						      "", "desktop", WORKDIR);
	need_uninstall_entry = 1;
    }
}

    வெறுமை
install_shortcut_evim(முழு idx)
{
    எனில் (choices[idx].arg)
    {
	(வெறுமை)build_shortcut(icon_names[1], "gvim.exe",
						    "-y", "desktop", WORKDIR);
	need_uninstall_entry = 1;
    }
}

    வெறுமை
install_shortcut_gview(முழு idx)
{
    எனில் (choices[idx].arg)
    {
	(வெறுமை)build_shortcut(icon_names[2], "gvim.exe",
						    "-R", "desktop", WORKDIR);
	need_uninstall_entry = 1;
    }
}

    வெறுமை
toggle_shortcut_choice(முழு idx)
{
    எழுத்து	*arg;

    எனில் (choices[idx].installfunc == install_shortcut_gvim)
	arg = "gVim";
    இல்லையெனில் எனில் (choices[idx].installfunc == install_shortcut_evim)
	arg = "gVim Easy";
    இல்லையெனில்
	arg = "gVim Read-only";
    எனில் (choices[idx].arg)
    {
	choices[idx].arg = 0;
	alloc_text(idx, "Do NOT create a desktop icon for %s", arg);
    }
    இல்லையெனில்
    {
	choices[idx].arg = 1;
	alloc_text(idx, "Create a desktop icon for %s", arg);
    }
}

    நிலையான வெறுமை
init_startmenu_choice(வெறுமை)
{
    // Start menu
    choices[choice_மொத்தம்].changefunc = toggle_startmenu_choice;
    choices[choice_மொத்தம்].installfunc = வெற்று;
    choices[choice_மொத்தம்].active = 1;
    toggle_startmenu_choice(choice_மொத்தம்);	// set the text
    ++choice_மொத்தம்;
}

/*
 * Add the choice for the desktop shortcuts.
 */
    நிலையான வெறுமை
init_shortcut_choices(வெறுமை)
{
    // Shortcut to gvim
    choices[choice_மொத்தம்].text = வெற்று;
    choices[choice_மொத்தம்].arg = 0;
    choices[choice_மொத்தம்].active = has_gvim;
    choices[choice_மொத்தம்].changefunc = toggle_shortcut_choice;
    choices[choice_மொத்தம்].installfunc = install_shortcut_gvim;
    toggle_shortcut_choice(choice_மொத்தம்);
    ++choice_மொத்தம்;

    // Shortcut to evim
    choices[choice_மொத்தம்].text = வெற்று;
    choices[choice_மொத்தம்].arg = 0;
    choices[choice_மொத்தம்].active = has_gvim;
    choices[choice_மொத்தம்].changefunc = toggle_shortcut_choice;
    choices[choice_மொத்தம்].installfunc = install_shortcut_evim;
    toggle_shortcut_choice(choice_மொத்தம்);
    ++choice_மொத்தம்;

    // Shortcut to gview
    choices[choice_மொத்தம்].text = வெற்று;
    choices[choice_மொத்தம்].arg = 0;
    choices[choice_மொத்தம்].active = has_gvim;
    choices[choice_மொத்தம்].changefunc = toggle_shortcut_choice;
    choices[choice_மொத்தம்].installfunc = install_shortcut_gview;
    toggle_shortcut_choice(choice_மொத்தம்);
    ++choice_மொத்தம்;
}

/*
 * Attempt to register OLE for Vim.
 */
   நிலையான வெறுமை
install_OLE_register(வெறுமை)
{
    எழுத்து register_command_string[BUFSIZE + 30];

    printf("\n--- Attempting to register Vim with OLE ---\n");
    printf("(There is no message whether this works or not.)\n");

    sprintf(register_command_string, "\"%s\\gvim.exe\" -silent -register", installdir);
    system(register_command_string);
}

/*
 * Remove the last part of directory "path[]" to get its parent, and put the
 * result in "to[]".
 */
    நிலையான வெறுமை
dir_remove_last(மாறிலி எழுத்து *path, எழுத்து to[MAX_PATH])
{
    எழுத்து c;
    long last_char_to_copy;
    long path_length = strlen(path);

    // skip the last character just in case it is a '\\'
    last_char_to_copy = path_length - 2;
    c = path[last_char_to_copy];

    போது (c != '\\')
    {
	last_char_to_copy--;
	c = path[last_char_to_copy];
    }

    strncpy(to, path, (size_t)last_char_to_copy);
    to[last_char_to_copy] = NUL;
}

    நிலையான வெறுமை
set_directories_text(முழு idx)
{
    முழு vimfiles_dir_choice = choices[idx].arg;

    எனில் (vimfiles_dir_choice == (முழு)vimfiles_dir_none)
	alloc_text(idx, "Do NOT create plugin directories%s", "");
    இல்லையெனில்
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
நிலையான எழுத்து *homedir = வெற்று;

    வெறுமை
init_homedir(வெறுமை)
{
    எழுத்து    *var;
    எழுத்து    buf[MAX_PATH];

    எனில் (homedir != வெற்று)
    {
	விடுவி(homedir);
	homedir = வெற்று;
    }

    var = getenv("HOME");

    /*
     * Typically, $HOME is not defined on Windows, unless the user has
     * specifically defined it for Vim's sake.  However, on Windows NT
     * platforms, $HOMEDRIVE and $HOMEPATH are automatically defined for
     * each user.  Try constructing $HOME from these.
     */
    எனில் (var == வெற்று || *var == NUL)
    {
	எழுத்து	*homedrive, *homepath;

	homedrive = getenv("HOMEDRIVE");
	homepath = getenv("HOMEPATH");
	எனில் (homepath == வெற்று || *homepath == NUL)
	    homepath = "\\";
	எனில் (homedrive != வெற்று
		   && strlen(homedrive) + strlen(homepath) < அளவு(buf))
	{
	    எனில் (snprintf(buf, அளவு(buf), "%s%s", homedrive, homepath) > 0
		&& buf[0] != NUL)
		var = buf;
	}
    }

    எனில் (var == வெற்று)
	var = getenv("USERPROFILE");

    /*
     * Weird but true: $HOME may contain an indirect reference to another
     * variable, esp. "%USERPROFILE%".  Happens when $USERPROFILE isn't set
     * when $HOME is being set.
     */
    எனில் (var != வெற்று && *var == '%')
    {
	எழுத்து	*p;
	எழுத்து	*exp;

	p = strchr(var + 1, '%');
	எனில் (p != வெற்று)
	{
	    strncpy(buf, var + 1, p - (var + 1));
	    buf[p - (var + 1)] = NUL;
	    exp = getenv(buf);
	    எனில் (exp != வெற்று && *exp != NUL
				&& strlen(exp) + strlen(p) < அளவு(buf))
	    {
		snprintf(buf, அளவு(buf), "%s%s", exp, p + 1);
		var = buf;
	    }
	}
    }

    எனில் (var != வெற்று && *var == NUL)	// empty is same as not set
	var = வெற்று;

    எனில் (var == வெற்று)
	homedir = வெற்று;
    இல்லையெனில்
	homedir = _strdup(var);
}

/*
 * Change the directory that the vim plugin directories will be created in:
 * $HOME, $VIM or nowhere.
 */
    நிலையான வெறுமை
change_directories_choice(முழு idx)
{
    முழு	    choice_மொத்தம் = TABLE_SIZE(vimfiles_dir_choices);

    // Don't offer the $HOME choice if $HOME isn't set.
    எனில் (homedir == வெற்று)
	--choice_மொத்தம்;
    choices[idx].arg = get_choice(vimfiles_dir_choices, choice_மொத்தம்);
    set_directories_text(idx);
}

/*
 * Create the plugin directories...
 */
//ARGSUSED
    நிலையான வெறுமை
install_vimfilesdir(முழு idx)
{
    முழு i;
    முழு vimfiles_dir_choice = choices[idx].arg;
    எழுத்து *p;
    எழுத்து vimdir_path[MAX_PATH];
    எழுத்து vimfiles_path[MAX_PATH + 9];
    எழுத்து tmp_dirname[BUFSIZE];

    // switch on the location that the user wants the plugin directories
    // built in
    தேர்வு (vimfiles_dir_choice)
    {
	நிலை vimfiles_dir_vim:
	{
	    // Go to the %VIM% directory - check env first, then go one dir
	    //	   below installdir if there is no %VIM% environment variable.
	    //	   The accuracy of $VIM is checked in inspect_system(), so we
	    //	   can be sure it is ok to use here.
	    p = getenv("VIM");
	    எனில் (p == வெற்று) // No $VIM in path
		dir_remove_last(installdir, vimdir_path);
	    இல்லையெனில்
		strcpy(vimdir_path, p);
	    நிறுத்து;
	}
	நிலை vimfiles_dir_home:
	{
	    // Find the $HOME directory.  Its existence was already checked.
	    p = homedir;
	    எனில் (p == வெற்று)
	    {
		printf("Internal error: $HOME is NULL\n");
		p = "c:\\";
	    }
	    strcpy(vimdir_path, p);
	    நிறுத்து;
	}
	நிலை vimfiles_dir_none:
	{
	    // Do not create vim plugin directory.
	    திரும்பு;
	}
    }

    // Now, just create the directory.	If it already exists, it will fail
    // silently.
    sprintf(vimfiles_path, "%s\\vimfiles", vimdir_path);
    vim_mkdir(vimfiles_path, 0755);

    printf("Creating the following directories in \"%s\":\n", vimfiles_path);
    க்கு (i = 0; i < TABLE_SIZE(vimfiles_subdirs); i++)
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
    நிலையான வெறுமை
init_directories_choice(வெறுமை)
{
    கட்டமைப்பு stat	st;
    எழுத்து	tmp_dirname[BUFSIZE];
    எழுத்து	*p;
    முழு		vimfiles_dir_choice;

    choices[choice_மொத்தம்].text = alloc(150);
    choices[choice_மொத்தம்].changefunc = change_directories_choice;
    choices[choice_மொத்தம்].installfunc = install_vimfilesdir;
    choices[choice_மொத்தம்].active = 1;

    // Check if the "compiler" directory already exists.  That's a good
    // indication that the plugin directories were already created.
    p = getenv("HOME");
    எனில் (p != வெற்று)
    {
	vimfiles_dir_choice = (முழு)vimfiles_dir_home;
	sprintf(tmp_dirname, "%s\\vimfiles\\compiler", p);
	எனில் (stat(tmp_dirname, &st) == 0)
	    vimfiles_dir_choice = (முழு)vimfiles_dir_none;
    }
    இல்லையெனில்
    {
	vimfiles_dir_choice = (முழு)vimfiles_dir_vim;
	p = getenv("VIM");
	எனில் (p == வெற்று)  // No $VIM in path, use the install dir.
	    dir_remove_last(installdir, tmp_dirname);
	இல்லையெனில்
	    strcpy(tmp_dirname, p);
	strcat(tmp_dirname, "\\vimfiles\\compiler");
	எனில் (stat(tmp_dirname, &st) == 0)
	    vimfiles_dir_choice = (முழு)vimfiles_dir_none;
    }

    choices[choice_மொத்தம்].arg = vimfiles_dir_choice;
    set_directories_text(choice_மொத்தம்);
    ++choice_மொத்தம்;
}

/*
 * Setup the choices and the default values.
 */
    நிலையான வெறுமை
setup_choices(வெறுமை)
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

    நிலையான வெறுமை
print_cmd_வரி_help(வெறுமை)
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
    நிலையான வெறுமை
command_வரி_setup_choices(முழு argc, எழுத்து **argv)
{
    முழு i, j;

    க்கு (i = 1; i < argc; i++)
    {
	எனில் (strcmp(argv[i], "-create-batfiles") == 0)
	{
	    எனில் (i + 1 == argc)
		தொடர்;
	    போது (argv[i + 1][0] != '-' && i < argc)
	    {
		i++;
		க்கு (j = 1; j < TARGET_மொத்தம்; ++j)
		    எனில் ((targets[j].exenamearg[0] == 'g' ? has_gvim : has_vim)
			    && strcmp(argv[i], targets[j].name) == 0)
		    {
			init_bat_choice(j);
			நிறுத்து;
		    }
		எனில் (j == TARGET_மொத்தம்)
		    printf("%s is not a valid choice for -create-batfiles\n",
								     argv[i]);

		எனில் (i + 1 == argc)
		    நிறுத்து;
	    }
	}
	இல்லையெனில் எனில் (strcmp(argv[i], "-create-vimrc") == 0)
	{
	    // Setup default vimrc choices.  If there is already a _vimrc file,
	    // it will NOT be overwritten.
	    init_vimrc_choices();
	}
	இல்லையெனில் எனில் (strcmp(argv[i], "-vimrc-remap") == 0)
	{
	    எனில் (i + 1 == argc)
		நிறுத்து;
	    i++;
	    எனில் (strcmp(argv[i], "no") == 0)
		remap_choice = remap_no;
	    இல்லையெனில் எனில் (strcmp(argv[i], "win") == 0)
		remap_choice = remap_win;
	}
	இல்லையெனில் எனில் (strcmp(argv[i], "-vimrc-behave") == 0)
	{
	    எனில் (i + 1 == argc)
		நிறுத்து;
	    i++;
	    எனில் (strcmp(argv[i], "unix") == 0)
		mouse_choice = mouse_xterm;
	    இல்லையெனில் எனில் (strcmp(argv[i], "mswin") == 0)
		mouse_choice = mouse_mswin;
	    இல்லையெனில் எனில் (strcmp(argv[i], "default") == 0)
		mouse_choice = mouse_default;
	}
	இல்லையெனில் எனில் (strcmp(argv[i], "-vimrc-compat") == 0)
	{
	    எனில் (i + 1 == argc)
		நிறுத்து;
	    i++;
	    எனில் (strcmp(argv[i], "vi") == 0)
		compat_choice = compat_vi;
	    இல்லையெனில் எனில் (strcmp(argv[i], "vim") == 0)
		compat_choice = compat_vim;
	    இல்லையெனில் எனில் (strcmp(argv[i], "defaults") == 0)
		compat_choice = compat_some_enhancements;
	    இல்லையெனில் எனில் (strcmp(argv[i], "all") == 0)
		compat_choice = compat_all_enhancements;
	}
	இல்லையெனில் எனில் (strcmp(argv[i], "-install-popup") == 0)
	{
	    init_popup_choice();
	}
	இல்லையெனில் எனில் (strcmp(argv[i], "-install-openwith") == 0)
	{
	    init_openwith_choice();
	}
	இல்லையெனில் எனில் (strcmp(argv[i], "-add-start-menu") == 0)
	{
	    init_startmenu_choice();
	}
	இல்லையெனில் எனில் (strcmp(argv[i], "-install-icons") == 0)
	{
	    init_shortcut_choices();
	}
	இல்லையெனில் எனில் (strcmp(argv[i], "-create-directories") == 0)
	{
	    முழு vimfiles_dir_choice = (முழு)vimfiles_dir_none;

	    init_directories_choice();
	    எனில் (i + 1 < argc && argv[i + 1][0] != '-')
	    {
		i++;
		எனில் (strcmp(argv[i], "vim") == 0)
		    vimfiles_dir_choice = (முழு)vimfiles_dir_vim;
		இல்லையெனில் எனில் (strcmp(argv[i], "home") == 0)
		{
		    எனில் (homedir == வெற்று)  // No $HOME in environment
			vimfiles_dir_choice = (முழு)vimfiles_dir_none;
		    இல்லையெனில்
			vimfiles_dir_choice = (முழு)vimfiles_dir_home;
		}
		இல்லையெனில்
		{
		    printf("Unknown argument for -create-directories: %s\n",
								     argv[i]);
		    print_cmd_வரி_help();
		}
	    }
	    இல்லையெனில் // No choice specified, default to vim directory
		vimfiles_dir_choice = (முழு)vimfiles_dir_vim;
	    choices[choice_மொத்தம் - 1].arg = vimfiles_dir_choice;
	}
	இல்லையெனில் எனில் (strcmp(argv[i], "-register-OLE") == 0)
	{
	    // This is always done when gvim is found
	}
	இல்லையெனில் // Unknown switch
	{
	    printf("Got unknown argument argv[%d] = %s\n", i, argv[i]);
	    print_cmd_வரி_help();
	}
    }
}


/*
 * Show a few screens full of helpful information.
 */
    நிலையான வெறுமை
show_help(வெறுமை)
{
    நிலையான எழுத்து *(பொருட்கள்[]) =
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
வெற்று
    };
    முழு		i;
    முழு		c;

    rewind(stdin);
    printf("\n");
    க்கு (i = 0; பொருட்கள்[i] != வெற்று; ++i)
    {
	puts(பொருட்கள்[i]);
	printf("Hit Enter to continue, b (back) or q (quit help): ");
	c = getchar();
	rewind(stdin);
	எனில் (c == 'b' || c == 'B')
	{
	    எனில் (i == 0)
		--i;
	    இல்லையெனில்
		i -= 2;
	}
	எனில் (c == 'q' || c == 'Q')
	    நிறுத்து;
	printf("\n");
    }
}

/*
 * Install the choices.
 */
    நிலையான வெறுமை
install(வெறுமை)
{
    முழு		i;

    // Install the selected choices.
    க்கு (i = 0; i < choice_மொத்தம்; ++i)
	எனில் (choices[i].installfunc != வெற்று && choices[i].active)
	    (choices[i].installfunc)(i);

    // Add some entries to the registry, if needed.
    எனில் (install_popup
	    || install_openwith
	    || (need_uninstall_entry && interactive)
	    || !interactive)
	install_registry();

    // Register gvim with OLE.
    எனில் (has_gvim)
	install_OLE_register();
}

/*
 * request_choice
 */
    நிலையான வெறுமை
request_choice(வெறுமை)
{
    முழு		      i;

    printf("\n\nInstall will do for you:\n");
    க்கு (i = 0; i < choice_மொத்தம்; ++i)
	எனில் (choices[i].active)
	    printf("%2d  %s\n", i + 1, choices[i].text);
    printf("To change an item, enter its number\n\n");
    printf("Enter item number, h (help), d (do it) or q (quit): ");
}

    முழு
முதன்மை(முழு argc, எழுத்து **argv)
{
    முழு		i;
    எழுத்து	buf[BUFSIZE];

    /*
     * Run interactively if there are no command line arguments.
     */
    எனில் (argc > 1)
	interactive = 0;
    இல்லையெனில்
	interactive = 1;

    // Initialize this program.
    do_inits(argv);
    init_homedir();

    எனில் (argc > 1 && strcmp(argv[1], "-uninstall-check") == 0)
    {
	// Only check for already installed Vims.  Used by NSIS installer.
	i = uninstall_check(1);

	// Find the value of $VIM, because NSIS isn't able to do this by
	// itself.
	get_vim_env();

	// When nothing found exit quietly.  If something found wait for
	// a little while, so that the user can read the messages.
	எனில் (i && _isatty(1))
	    sleep(3);
	exit(0);
    }

    printf("This program sets up the installation of Vim "
						   VIM_VERSION_MEDIUM "\n\n");

    // Check if the user unpacked the archives properly.
    check_unpack();

    // Check for already installed Vims.
    எனில் (interactive)
	uninstall_check(0);

    // Find out information about the system.
    inspect_system();

    எனில் (interactive)
    {
	// Setup all the choices.
	setup_choices();

	// Let the user change choices and finally install (or quit).
	க்கு (;;)
	{
	    request_choice();
	    rewind(stdin);
	    எனில் (scanf("%99s", buf) == 1)
	    {
		எனில் (isdigit((unsigned எழுத்து)buf[0]))
		{
		    // Change a choice.
		    i = atoi(buf);
		    எனில் (i > 0 && i <= choice_மொத்தம் && choices[i - 1].active)
			(choices[i - 1].changefunc)(i - 1);
		    இல்லையெனில்
			printf("\nIllegal choice\n");
		}
		இல்லையெனில் எனில் (buf[0] == 'h' || buf[0] == 'H')
		{
		    // Help
		    show_help();
		}
		இல்லையெனில் எனில் (buf[0] == 'd' || buf[0] == 'D')
		{
		    // Install!
		    install();
		    printf("\nThat finishes the installation.  Happy Vimming!\n");
		    நிறுத்து;
		}
		இல்லையெனில் எனில் (buf[0] == 'q' || buf[0] == 'Q')
		{
		    // Quit
		    printf("\nExiting without anything done\n");
		    நிறுத்து;
		}
		இல்லையெனில்
		    printf("\nIllegal choice\n");
	    }
	}
	printf("\n");
	myexit(0);
    }
    இல்லையெனில்
    {
	/*
	 * Run non-interactive - setup according to the command line switches
	 */
	command_வரி_setup_choices(argc, argv);
	install();

	// Avoid that the user has to hit Enter, just wait a little bit to
	// allow reading the messages.
	sleep(2);
    }

    திரும்பு 0;
}
