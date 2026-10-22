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
#અન્તર્ભાવ "dosinst.h"
#અન્તર્ભાવ <io.h>

#define GVIMEXT64_PATH	    "GvimExt64\\gvimext.dll"
#define GVIMEXT32_PATH	    "GvimExt32\\gvimext.dll"

// Macro to do an error check I was typing over and over
#define CHECK_REG_ERROR(code) \
    કરો { \
	જો (code != ERROR_SUCCESS) \
	{ \
	    printf("%ld error number:  %ld\n", (long)__LINE__, (long)code); \
	    પાછા 1; \
	} \
    } જ્યારે (0)

પૂર્ણાંક	has_vim = 0;		// installable vim.exe exists
પૂર્ણાંક	has_gvim = 0;		// installable gvim.exe exists

અક્ષર	oldvimrc[BUFSIZE];	// name of existing vimrc file
અક્ષર	vimrc[BUFSIZE];		// name of vimrc file to create

અક્ષર	*default_bat_dir = ખાલી;  // when not NULL, use this as the default
				  // directory to write .bat files in
અક્ષર	*default_vim_dir = ખાલી;  // when not NULL, use this as the default
				  // install dir for NSIS

/*
 * Structure used for each choice the user can make.
 */
સંરચના choice
{
    પૂર્ણાંક	    active;			// non-zero when choice is active
    અક્ષર    *text;			// text displayed for this choice
    શૂન્ય    (*changefunc)(પૂર્ણાંક idx);	// function to change this choice
    પૂર્ણાંક	    arg;			// argument for function
    શૂન્ય    (*installfunc)(પૂર્ણાંક idx);	// function to install this choice
};

સંરચના choice	choices[30];		// choices the user can make
પૂર્ણાંક		choice_ગણતરી = 0;	// number of choices available

#define TABLE_SIZE(s)	(પૂર્ણાંક)ARRAYSIZE(s)

ગણના
{
    compat_vi = 1,
    compat_vim,
    compat_some_enhancements,
    compat_all_enhancements
};
અક્ષર	*(compat_choices[]) =
{
    "\nChoose the default way to run Vim:",
    "Vi compatible",
    "Vim default",
    "with some Vim enhancements",
    "with syntax highlighting and other features switched on",
};
પૂર્ણાંક	compat_choice = (પૂર્ણાંક)compat_all_enhancements;
અક્ષર	*compat_text = "- run Vim %s";

ગણના
{
    remap_no = 1,
    remap_win
};
અક્ષર	*(remap_choices[]) =
{
    "\nChoose:",
    "Do not remap keys for Windows behavior",
    "Remap a few keys for Windows behavior (CTRL-V, CTRL-C, CTRL-F, etc)",
};
પૂર્ણાંક	remap_choice = (પૂર્ણાંક)remap_no;
અક્ષર	*remap_text = "- %s";

ગણના
{
    mouse_xterm = 1,
    mouse_mswin,
    mouse_default
};
અક્ષર	*(mouse_choices[]) =
{
    "\nChoose the way how Vim uses the mouse:",
    "right button extends selection (the Unix way)",
    "right button has a popup menu, left button starts select mode (the Windows way)",
    "right button has a popup menu, left button starts visual mode",
};
પૂર્ણાંક	mouse_choice = (પૂર્ણાંક)mouse_default;
અક્ષર	*mouse_text = "- The mouse %s";

ગણના
{
    vimfiles_dir_none = 1,
    vimfiles_dir_vim,
    vimfiles_dir_home
};
સ્થિર અક્ષર *(vimfiles_dir_choices[]) =
{
    "\nCreate plugin directories:",
    "No",
    "In the VIM directory",
    "In your HOME directory",
};

// non-zero when selected to install the popup menu entry.
સ્થિર પૂર્ણાંક	install_popup = 0;

// non-zero when selected to install the "Open with" entry.
સ્થિર પૂર્ણાંક	install_openwith = 0;

// non-zero when need to add an uninstall entry in the registry
સ્થિર પૂર્ણાંક	need_uninstall_entry = 0;

/*
 * Definitions of the directory name (under $VIM) of the vimfiles directory
 * and its subdirectories:
 */
સ્થિર અક્ષર	*(vimfiles_subdirs[]) =
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
    સ્થિર પૂર્ણાંક
get_choice(અક્ષર **table, પૂર્ણાંક entries)
{
    પૂર્ણાંક		answer;
    પૂર્ણાંક		idx;
    અક્ષર	dummy[100];

    કરો
    {
	માટે (idx = 0; idx < entries; ++idx)
	{
	    જો (idx)
		printf("%2d  ", idx);
	    puts(table[idx]);
	}
	printf("Choice: ");
	જો (scanf("%d", &answer) != 1)
	{
	    scanf("%99s", dummy);
	    answer = 0;
	}
    }
    જ્યારે (answer < 1 || answer >= entries);

    પાછા answer;
}

/*
 * Check if the user unpacked the archives properly.
 * Sets "runtimeidx".
 */
    સ્થિર શૂન્ય
check_unpack(શૂન્ય)
{
    અક્ષર	buf[BUFSIZE];
    FILE	*fd;
    સંરચના stat	st;

    // check for presence of the correct version number in installdir[]
    runtimeidx = strlen(installdir) - strlen(VIM_VERSION_NODOT);
    જો (runtimeidx <= 0
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
    જો (stat(buf, &st) < 0)
    {
	printf("ERROR: Cannot find filetype.vim in \"%s\"\n", installdir);
	printf("It looks like you did not unpack the runtime archive.\n");
	printf("You must unpack the runtime archive \"%srt.zip\" before installing.\n",
		VIM_VERSION_NODOT);
	myexit(1);
    }

    // Check if vim.exe or gvim.exe is in the current directory.
    જો ((fd = fopen("gvim.exe", "r")) != ખાલી)
    {
	fclose(fd);
	has_gvim = 1;
    }
    જો ((fd = fopen("vim.exe", "r")) != ખાલી)
    {
	fclose(fd);
	has_vim = 1;
    }
    જો (!has_gvim && !has_vim)
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
    સ્થિર પૂર્ણાંક
pathcmp(અક્ષર *p, પૂર્ણાંક plen, અક્ષર *q, પૂર્ણાંક qlen)
{
    પૂર્ણાંક		i;

    જો (plen < 0)
	plen = strlen(p);
    જો (qlen < 0)
	qlen = strlen(q);
    માટે (i = 0; ; ++i)
    {
	// End of "p": check if "q" also ends or just has a slash.
	જો (i == plen)
	{
	    જો (i == qlen)  // match
		પાછા 0;
	    જો (i == qlen - 1 && (q[i] == '\\' || q[i] == '/'))
		પાછા 0;   // match with trailing slash
	    પાછા 1;	    // no match
	}

	// End of "q": check if "p" also ends or just has a slash.
	જો (i == qlen)
	{
	    જો (i == plen)  // match
		પાછા 0;
	    જો (i == plen - 1 && (p[i] == '\\' || p[i] == '/'))
		પાછા 0;   // match with trailing slash
	    પાછા 1;	    // no match
	}

	જો (!(mytoupper(p[i]) == mytoupper(q[i])
		|| ((p[i] == '/' || p[i] == '\\')
		    && (q[i] == '/' || q[i] == '\\'))))
	    પાછા 1;	    // no match
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
    સ્થિર શૂન્ય
findoldfile(અક્ષર **destination)
{
    અક્ષર	*bp = *destination;
    size_t	indir_l = strlen(installdir);
    અક્ષર	*cp;
    અક્ષર	*tmpname;
    અક્ષર	*farname;

    /*
     * No action needed if exe not found or not in this directory.
     */
    જો (bp == ખાલી || strnicmp(bp, installdir, indir_l) != 0)
	પાછા;
    cp = bp + indir_l;
    જો (strchr("/\\", *cp++) == ખાલી
	    || strchr(cp, '\\') != ખાલી
	    || strchr(cp, '/') != ખાલી)
	પાછા;

    tmpname = alloc(strlen(cp) + 1);
    strcpy(tmpname, cp);
    tmpname[strlen(tmpname) - 1] = 'x';	// .exe -> .exx

    જો (access(tmpname, 0) == 0)
    {
	printf("\nERROR: %s and %s clash.  Remove or rename %s.\n",
	    tmpname, cp, tmpname);
	myexit(1);
    }

    જો (rename(cp, tmpname) != 0)
    {
	printf("\nERROR: failed to rename %s to %s: %s\n",
	    cp, tmpname, strerror(0));
	myexit(1);
    }

    farname = searchpath_save(cp);

    જો (rename(tmpname, cp) != 0)
    {
	printf("\nERROR: failed to rename %s back to %s: %s\n",
	    tmpname, cp, strerror(0));
	myexit(1);
    }

    મુક્ત(*destination);
    મુક્ત(tmpname);
    *destination = farname;
}

/*
 * Check if there is a vim.[exe|bat|, gvim.[exe|bat|, etc. in the path.
 * When "check_bat_only" is TRUE, only find "default_bat_dir".
 */
    સ્થિર શૂન્ય
find_bat_exe(પૂર્ણાંક check_bat_only)
{
    પૂર્ણાંક		i;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    માટે (i = 1; i < TARGET_ગણતરી; ++i)
    {
	targets[i].oldbat = searchpath_save(targets[i].batname);
	જો (!check_bat_only)
	    targets[i].oldexe = searchpath_save(targets[i].exename);

	જો (default_bat_dir == ખાલી && targets[i].oldbat != ખાલી)
	{
	    default_bat_dir = alloc(strlen(targets[i].oldbat) + 1);
	    strcpy(default_bat_dir, targets[i].oldbat);
	    remove_tail(default_bat_dir);
	}
	જો (check_bat_only && targets[i].oldbat != ખાલી)
	{
	    મુક્ત(targets[i].oldbat);
	    targets[i].oldbat = ખાલી;
	}
    }

    mch_chdir(installdir);
}

/*
 * Get the value of $VIMRUNTIME or $VIM and write it in $TEMP/vimini.ini, so
 * that NSIS can read it.
 * When not set, use the directory of a previously installed Vim.
 */
    સ્થિર શૂન્ય
get_vim_env(શૂન્ય)
{
    અક્ષર	*vim;
    અક્ષર	buf[BUFSIZE];
    FILE	*fd;
    અક્ષર	fname[BUFSIZE];

    // First get $VIMRUNTIME.  If it's set, remove the tail.
    vim = getenv("VIMRUNTIME");
    જો (vim != ખાલી && *vim != 0 && strlen(vim) < કદ(buf))
    {
	strcpy(buf, vim);
	remove_tail(buf);
	vim = buf;
    }
    નહીંતર
    {
	vim = getenv("VIM");
	જો (vim == ખાલી || *vim == 0)
	{
	    // Use the directory from an old uninstall entry.
	    જો (default_vim_dir != ખાલી)
		vim = default_vim_dir;
	    નહીંતર
		// Let NSIS know there is no default, it should use
		// $PROGRAMFILES.
		vim = "";
	}
    }

    // NSIS also uses GetTempPath(), thus we should get the same directory
    // name as where NSIS will look for vimini.ini.
    GetTempPath(કદ(fname) - 12, fname);
    add_pathsep(fname);
    strcat(fname, "vimini.ini");

    fd = fopen(fname, "w");
    જો (fd != ખાલી)
    {
	// Make it look like an .ini file, so that NSIS can read it with a
	// ReadINIStr command.
	fprintf(fd, "[vimini]\n");
	fprintf(fd, "dir=\"%s\"\n", vim);
	fclose(fd);
    }
    નહીંતર
    {
	printf("Failed to open %s\n", fname);
	sleep(2);
    }
}

સ્થિર પૂર્ણાંક અંક_windows;

/*
 * Callback used for EnumWindows():
 * Count the window if the title looks like it is for the uninstaller.
 */
//ARGSUSED
    સ્થિર BOOL CALLBACK
window_cb(HWND hwnd, LPARAM lparam UNUSED)
{
    અક્ષર title[256];

    title[0] = 0;
    GetWindowText(hwnd, title, 256);
    જો (strstr(title, "Vim ") != ખાલી && strstr(title, " Uninstall") != ખાલી)
	++અંક_windows;
    પાછા TRUE;
}

/*
 * Run the uninstaller silently.
 */
    સ્થિર પૂર્ણાંક
run_silent_uninstall(અક્ષર *uninst_exe)
{
    અક્ષર    vimrt_dir[BUFSIZE];
    અક્ષર    temp_uninst[BUFSIZE];
    અક્ષર    temp_dir[MAX_PATH];
    અક્ષર    buf[BUFSIZE * 2 + 10];
    પૂર્ણાંક	    i;
    DWORD   tick;

    strcpy(vimrt_dir, uninst_exe);
    remove_tail(vimrt_dir);

    જો (!GetTempPath(કદ(temp_dir), temp_dir))
	પાછા FAIL;

    // Copy the uninstaller to a temporary exe.
    tick = Get_Tick_ગણતરી();
    માટે (i = 0; ; i++)
    {
	sprintf(temp_uninst, "%s\\vimun%04X.exe", temp_dir,
					  (unsigned પૂર્ણાંક)((i + tick) & 0xFFFF));
	જો (CopyFile(uninst_exe, temp_uninst, TRUE))
	    તોડો;
	જો (GetLastError() != ERROR_FILE_EXISTS)
	    પાછા FAIL;
	જો (i == 65535)
	    પાછા FAIL;
    }

    // Run the copied uninstaller silently.
    જો (strchr(temp_uninst, ' ') != ખાલી)
	sprintf(buf, "\"%s\" /S _?=%s", temp_uninst, vimrt_dir);
    નહીંતર
	sprintf(buf, "%s /S _?=%s", temp_uninst, vimrt_dir);
    run_command(buf);

    DeleteFile(temp_uninst);
    પાછા OK;
}

/*
 * Check for already installed Vims.
 * Return non-zero when found one.
 */
    સ્થિર પૂર્ણાંક
uninstall_check(પૂર્ણાંક skip_question)
{
    HKEY	ચાવી_handle;
    HKEY	uninstall_ચાવી_handle;
    અક્ષર	*uninstall_ચાવી = "software\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
    અક્ષર	subkey_name_buff[BUFSIZE];
    અક્ષર	temp_string_buffer[BUFSIZE-2];
    DWORD	local_bufsize;
    FILETIME	temp_pfiletime;
    DWORD	ચાવી_index;
    અક્ષર	આગત;
    long	code;
    DWORD	કિંમત_type;
    DWORD	orig_અંક_ચાવીઓ;
    DWORD	new_અંક_ચાવીઓ;
    DWORD	allow_silent;
    પૂર્ણાંક		foundone = 0;

    code = Reg_Open_ચાવી_Ex(HKEY_LOCAL_MACHINE, uninstall_ચાવી, 0,
				     ચાવી_WOW64_64_ચાવી | ચાવી_READ, &ચાવી_handle);
    CHECK_REG_ERROR(code);

    ચાવી_index = 0;
    જ્યારે (TRUE)
    {
	local_bufsize = કદ(subkey_name_buff);
	જો (Reg_Enum_ચાવી_Ex(ચાવી_handle, ચાવી_index, subkey_name_buff, &local_bufsize,
		ખાલી, ખાલી, ખાલી, &temp_pfiletime) == ERROR_NO_MORE_વસ્તુઓ)
	    તોડો;

	જો (strncmp("Vim", subkey_name_buff, 3) == 0)
	{
	    // Open the key named Vim*
	    code = Reg_Open_ચાવી_Ex(ચાવી_handle, subkey_name_buff, 0,
			   ચાવી_WOW64_64_ચાવી | ચાવી_READ, &uninstall_ચાવી_handle);
	    CHECK_REG_ERROR(code);

	    // get the DisplayName out of it to show the user
	    local_bufsize = કદ(temp_string_buffer);
	    code = Reg_Query_કિંમત_Ex(uninstall_ચાવી_handle, "displayname", 0,
		    &કિંમત_type, (LPBYTE)temp_string_buffer,
		    &local_bufsize);
	    CHECK_REG_ERROR(code);

	    allow_silent = 0;
	    જો (skip_question)
	    {
		DWORD varsize = કદ(DWORD);

		Reg_Query_કિંમત_Ex(uninstall_ચાવી_handle, "AllowSilent", 0,
			&કિંમત_type, (LPBYTE)&allow_silent,
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

	    જો (skip_question)
		printf("\nRunning uninstall program for \"%s\"\n", temp_string_buffer);
	    નહીંતર
		printf("\nDo you want to uninstall \"%s\" now?\n(y)es/(n)o)  ", temp_string_buffer);
	    fflush(stdout);

	    // get the UninstallString
	    local_bufsize = કદ(temp_string_buffer);
	    code = Reg_Query_કિંમત_Ex(uninstall_ચાવી_handle, "uninstallstring", 0,
		    &કિંમત_type, (LPBYTE)temp_string_buffer, &local_bufsize);
	    CHECK_REG_ERROR(code);

	    // Remember the directory, it is used as the default for NSIS.
	    default_vim_dir = alloc(strlen(temp_string_buffer) + 1);
	    strcpy(default_vim_dir, temp_string_buffer);
	    remove_tail(default_vim_dir);
	    remove_tail(default_vim_dir);

	    આગત = 'n';
	    કરો
	    {
		જો (આગત != 'n')
		    printf("%c is an invalid reply.  Please enter either 'y' or 'n'\n", આગત);

		જો (skip_question)
		    આગત = 'y';
		નહીંતર
		{
		    rewind(stdin);
		    scanf("%c", &આગત);
		}
		પસંદગી (આગત)
		{
		    સ્થિતિ 'y':
		    સ્થિતિ 'Y':
			// save the number of uninstall keys so we can know if
			// it changed
			Reg_Query_Info_ચાવી(ચાવી_handle, ખાલી, ખાલી, ખાલી,
					     &orig_અંક_ચાવીઓ, ખાલી, ખાલી, ખાલી,
						      ખાલી, ખાલી, ખાલી, ખાલી);

			// Find existing .bat files before deleting them.
			find_bat_exe(TRUE);

			જો (allow_silent)
			{
			    જો (run_silent_uninstall(temp_string_buffer)
								    == FAIL)
				allow_silent = 0; // Retry with non silent.
			}
			જો (!allow_silent)
			{
			    // Execute the uninstall program.  Put it in double
			    // quotes if there is an embedded space.
			    {
				અક્ષર buf[BUFSIZE];

				જો (strchr(temp_string_buffer, ' ') != ખાલી)
				    sprintf(buf, "\"%s\"", temp_string_buffer);
				નહીંતર
				    strcpy(buf, temp_string_buffer);
				run_command(buf);
			    }

			    // Count the number of windows with a title that
			    // match the installer, so that we can check when
			    // it's done.  The uninstaller copies itself,
			    // executes the copy and exits, thus we can't wait
			    // for the process to finish.
			    sleep(1);  // wait for uninstaller to start up
			    અંક_windows = 0;
			    EnumWindows(window_cb, 0);
			    જો (અંક_windows == 0)
			    {
				// Did not find the uninstaller, ask user to
				// press Enter when done. Just in case.
				printf("Press Enter when the uninstaller is finished\n");
				rewind(stdin);
				(શૂન્ય)getchar();
			    }
			    નહીંતર
			    {
				printf("Waiting for the uninstaller to finish (press CTRL-C to abort).");
				કરો
				{
				    printf(".");
				    fflush(stdout);
				    sleep(1);	// wait for the uninstaller to
						// finish
				    અંક_windows = 0;
				    EnumWindows(window_cb, 0);
				} જ્યારે (અંક_windows > 0);
			    }
			}
			printf("\nDone!\n");

			// Check if an uninstall reg key was deleted.
			// if it was, we want to decrement key_index.
			// if we don't do this, we will skip the key
			// immediately after any key that we delete.
			Reg_Query_Info_ચાવી(ચાવી_handle, ખાલી, ખાલી, ખાલી,
					      &new_અંક_ચાવીઓ, ખાલી, ખાલી, ખાલી,
						      ખાલી, ખાલી, ખાલી, ખાલી);
			જો (new_અંક_ચાવીઓ < orig_અંક_ચાવીઓ)
			    ચાવી_index--;

			આગત = 'y';
			તોડો;

		    સ્થિતિ 'n':
		    સ્થિતિ 'N':
			// Do not uninstall
			આગત = 'n';
			તોડો;

		    મૂળભૂત: // just drop through and redo the loop
			તોડો;
		}

	    } જ્યારે (આગત != 'n' && આગત != 'y');

	    Reg_Close_ચાવી(uninstall_ચાવી_handle);
	}

	ચાવી_index++;
    }
    Reg_Close_ચાવી(ચાવી_handle);

    પાછા foundone;
}

/*
 * Find out information about the system.
 */
    સ્થિર શૂન્ય
inspect_system(શૂન્ય)
{
    અક્ષર	*p;
    અક્ષર	buf[BUFSIZE];
    FILE	*fd;
    પૂર્ણાંક		i;
    પૂર્ણાંક		foundone;

    // This may take a little while, let the user know what we're doing.
    printf("Inspecting system...\n");

    /*
     * If $VIM is set, check that it's pointing to our directory.
     */
    p = getenv("VIM");
    જો (p != ખાલી && pathcmp(p, -1, installdir, runtimeidx - 1) != 0)
    {
	printf("------------------------------------------------------\n");
	printf("$VIM is set to \"%s\".\n", p);
	printf("This is different from where this version of Vim is:\n");
	strcpy(buf, installdir);
	*(buf + runtimeidx - 1) = NUL;
	printf("\"%s\"\n", buf);
	printf("You must adjust or remove the setting of $VIM,\n");
	જો (interactive)
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
    જો (p != ખાલી && pathcmp(p, -1, installdir, -1) != 0)
    {
	printf("------------------------------------------------------\n");
	printf("$VIMRUNTIME is set to \"%s\".\n", p);
	printf("This is different from where this version of Vim is:\n");
	printf("\"%s\"\n", installdir);
	printf("You must adjust or remove the setting of $VIMRUNTIME,\n");
	જો (interactive)
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
    માટે (i = 1; i < TARGET_ગણતરી; ++i)
    {
	findoldfile(&(targets[i].oldexe));
	જો (targets[i].oldexe != ખાલી)
	    foundone = 1;
    }

    જો (foundone)
    {
	printf("Warning: Found Vim executable(s) in your $PATH:\n");
	માટે (i = 1; i < TARGET_ગણતરી; ++i)
	    જો (targets[i].oldexe != ખાલી)
		printf("%s\n", targets[i].oldexe);
	printf("It will be used instead of the version you are installing.\n");
	printf("Please delete or rename it, or adjust your $PATH setting.\n");
    }

    /*
     * Check if there is an existing ../_vimrc or ../.vimrc file.
     */
    strcpy(oldvimrc, installdir);
    strcpy(oldvimrc + runtimeidx, "_vimrc");
    જો ((fd = fopen(oldvimrc, "r")) == ખાલી)
    {
	strcpy(oldvimrc + runtimeidx, "vimrc~1"); // short version of .vimrc
	જો ((fd = fopen(oldvimrc, "r")) == ખાલી)
	{
	    strcpy(oldvimrc + runtimeidx, ".vimrc");
	    fd = fopen(oldvimrc, "r");
	}
    }
    જો (fd != ખાલી)
	fclose(fd);
    નહીંતર
	*oldvimrc = NUL;
}

/*
 * Add a dummy choice to avoid that the numbering changes depending on items
 * in the environment.  The user may type a number he remembered without
 * looking.
 */
    સ્થિર શૂન્ય
add_dummy_choice(શૂન્ય)
{
    choices[choice_ગણતરી].installfunc = ખાલી;
    choices[choice_ગણતરી].active = 0;
    choices[choice_ગણતરી].changefunc = ખાલી;
    choices[choice_ગણતરી].text = ખાલી;
    choices[choice_ગણતરી].arg = 0;
    ++choice_ગણતરી;
}

////////////////////////////////////////////////
// stuff for creating the batch files.

/*
 * Install the vim.bat, gvim.bat, etc. files.
 */
    સ્થિર શૂન્ય
install_bat_choice(પૂર્ણાંક idx)
{
    અક્ષર	*batpath = targets[choices[idx].arg].batpath;
    અક્ષર	*oldname = targets[choices[idx].arg].oldbat;
    અક્ષર	*exename = targets[choices[idx].arg].exenamearg;
    અક્ષર	*vimarg = targets[choices[idx].arg].exearg;
    FILE	*fd;

    જો (*batpath == NUL)
	પાછા;

    fd = fopen(batpath, "w");
    જો (fd == ખાલી)
    {
	printf("\nERROR: Cannot open \"%s\" for writing.\n", batpath);
	પાછા;
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

    જો (*exename == 'g')
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

    જો (*exename == 'g')
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
    નહીંતર
    {
	// Always use quotes, $VIM or $VIMRUNTIME might have a space.
	fprintf(fd, "\"%%VIM_EXE_DIR%%\\%s\" %s %%*\n",
		exename, vimarg);
    }

    fclose(fd);
    printf("%s has been %s\n", batpath,
	    oldname == ખાલી ? "created" : "overwritten");
}

/*
 * Make the text string for choice "idx".
 * The format "fmt" is must have one %s item, which "arg" is used for.
 */
    સ્થિર શૂન્ય
alloc_text(પૂર્ણાંક idx, અક્ષર *fmt, અક્ષર *arg)
{
    જો (choices[idx].text != ખાલી)
	મુક્ત(choices[idx].text);

    choices[idx].text = alloc(strlen(fmt) + strlen(arg) - 1);
    sprintf(choices[idx].text, fmt, arg);
}

/*
 * Toggle the "Overwrite .../vim.bat" to "Don't overwrite".
 */
    સ્થિર શૂન્ય
toggle_bat_choice(પૂર્ણાંક idx)
{
    અક્ષર	*batname = targets[choices[idx].arg].batpath;
    અક્ષર	*oldname = targets[choices[idx].arg].oldbat;

    જો (*batname == NUL)
    {
	alloc_text(idx, "    Overwrite %s", oldname);
	strcpy(batname, oldname);
    }
    નહીંતર
    {
	alloc_text(idx, "    Do NOT overwrite %s", oldname);
	*batname = NUL;
    }
}

/*
 * Do some work for a batch file entry: Append the batch file name to the path
 * and set the text for the choice.
 */
    સ્થિર શૂન્ય
set_bat_text(પૂર્ણાંક idx, અક્ષર *batpath, અક્ષર *name)
{
    strcat(batpath, name);

    alloc_text(idx, "    Create %s", batpath);
}

/*
 * Select a directory to write the batch file line.
 */
    સ્થિર શૂન્ય
change_bat_choice(પૂર્ણાંક idx)
{
    અક્ષર	*path;
    અક્ષર	*batpath;
    અક્ષર	*name;
    પૂર્ણાંક		n;
    અક્ષર	*s;
    અક્ષર	*p;
    પૂર્ણાંક		ગણતરી;
    અક્ષર	**names = ખાલી;
    પૂર્ણાંક		i;
    પૂર્ણાંક		target = choices[idx].arg;

    name = targets[target].batname;
    batpath = targets[target].batpath;

    path = getenv("PATH");
    જો (path == ખાલી)
    {
	printf("\nERROR: The variable $PATH is not set\n");
	પાછા;
    }

    /*
     * first round: count number of names in path;
     * second round: save names to names[].
     */
    માટે (;;)
    {
	ગણતરી = 1;
	માટે (p = path; *p; )
	{
	    s = strchr(p, ';');
	    જો (s == ખાલી)
		s = p + strlen(p);
	    જો (names != ખાલી)
	    {
		names[ગણતરી] = alloc(s - p + 1);
		strncpy(names[ગણતરી], p, s - p);
		names[ગણતરી][s - p] = NUL;
	    }
	    ++ગણતરી;
	    p = s;
	    જો (*p != NUL)
		++p;
	}
	જો (names != ખાલી)
	    તોડો;
	names = alloc((ગણતરી + 1) * કદ(અક્ષર *));
    }
    names[0] = alloc(50);
    sprintf(names[0], "Select directory to create %s in:", name);
    names[ગણતરી] = alloc(50);
    જો (choices[idx].arg == 0)
	sprintf(names[ગણતરી], "Do not create any .bat file.");
    નહીંતર
	sprintf(names[ગણતરી], "Do not create a %s file.", name);
    n = get_choice(names, ગણતરી + 1);

    જો (n == ગણતરી)
    {
	// Selected last item, don't create bat file.
	*batpath = NUL;
	જો (choices[idx].arg != 0)
	    alloc_text(idx, "    Do NOT create %s", name);
    }
    નહીંતર
    {
	// Selected one of the paths.  For the first item only keep the path,
	// for the others append the batch file name.
	strcpy(batpath, names[n]);
	add_pathsep(batpath);
	જો (choices[idx].arg != 0)
	    set_bat_text(idx, batpath, name);
    }

    માટે (i = 0; i <= ગણતરી; ++i)
	મુક્ત(names[i]);
    મુક્ત(names);
}

અક્ષર *bat_text_yes = "Install .bat files to use Vim at the command line:";
અક્ષર *bat_text_no = "do NOT install .bat files to use Vim at the command line";

    સ્થિર શૂન્ય
change_main_bat_choice(પૂર્ણાંક idx)
{
    પૂર્ણાંક		i;

    // let the user select a default directory or NONE
    change_bat_choice(idx);

    જો (targets[0].batpath[0] != NUL)
	choices[idx].text = bat_text_yes;
    નહીંતર
	choices[idx].text = bat_text_no;

    // update the individual batch file selections
    માટે (i = 1; i < TARGET_ગણતરી; ++i)
    {
	// Only make it active when the first item has a path and the vim.exe
	// or gvim.exe exists (there is a changefunc then).
	જો (targets[0].batpath[0] != NUL
		&& choices[idx + i].changefunc != ખાલી)
	{
	    choices[idx + i].active = 1;
	    જો (choices[idx + i].changefunc == change_bat_choice
		    && targets[i].batpath[0] != NUL)
	    {
		strcpy(targets[i].batpath, targets[0].batpath);
		set_bat_text(idx + i, targets[i].batpath, targets[i].batname);
	    }
	}
	નહીંતર
	    choices[idx + i].active = 0;
    }
}

/*
 * Initialize a choice for creating a batch file.
 */
    સ્થિર શૂન્ય
init_bat_choice(પૂર્ણાંક target)
{
    અક્ષર	*batpath = targets[target].batpath;
    અક્ષર	*oldbat = targets[target].oldbat;
    અક્ષર	*p;
    પૂર્ણાંક		i;

    choices[choice_ગણતરી].arg = target;
    choices[choice_ગણતરી].installfunc = install_bat_choice;
    choices[choice_ગણતરી].active = 1;
    choices[choice_ગણતરી].text = ખાલી;	// will be set below
    જો (oldbat != ખાલી)
    {
	// A [g]vim.bat exists: Only choice is to overwrite it or not.
	choices[choice_ગણતરી].changefunc = toggle_bat_choice;
	*batpath = NUL;
	toggle_bat_choice(choice_ગણતરી);
    }
    નહીંતર
    {
	જો (default_bat_dir != ખાલી)
	    // Prefer using the same path as an existing .bat file.
	    strcpy(batpath, default_bat_dir);
	નહીંતર
	{
	    // No [g]vim.bat exists: Write it to a directory in $PATH.  Use
	    // $WINDIR by default, if it's empty the first item in $PATH.
	    p = getenv("WINDIR");
	    જો (p != ખાલી && *p != NUL)
		strcpy(batpath, p);
	    નહીંતર
	    {
		p = getenv("PATH");
		જો (p == ખાલી || *p == NUL)		// "cannot happen"
		    strcpy(batpath, "C:/Windows");
		નહીંતર
		{
		    i = 0;
		    જ્યારે (*p != NUL && *p != ';')
			batpath[i++] = *p++;
		    batpath[i] = NUL;
		}
	    }
	}
	add_pathsep(batpath);
	set_bat_text(choice_ગણતરી, batpath, targets[target].batname);

	choices[choice_ગણતરી].changefunc = change_bat_choice;
    }
    ++choice_ગણતરી;
}

/*
 * Set up the choices for installing .bat files.
 * For these items "arg" is the index in targets[].
 */
    સ્થિર શૂન્ય
init_bat_choices(શૂન્ય)
{
    પૂર્ણાંક		i;

    // The first item is used to switch installing batch files on/off and
    // setting the default path.
    choices[choice_ગણતરી].text = bat_text_yes;
    choices[choice_ગણતરી].changefunc = change_main_bat_choice;
    choices[choice_ગણતરી].installfunc = ખાલી;
    choices[choice_ગણતરી].active = 1;
    choices[choice_ગણતરી].arg = 0;
    ++choice_ગણતરી;

    // Add items for each batch file target.  Only used when not disabled by
    // the first item.  When a .exe exists, don't offer to create a .bat.
    માટે (i = 1; i < TARGET_ગણતરી; ++i)
	જો (targets[i].oldexe == ખાલી
		&& (targets[i].exenamearg[0] == 'g' ? has_gvim : has_vim))
	    init_bat_choice(i);
	નહીંતર
	    add_dummy_choice();
}

/*
 * Install the vimrc file.
 */
    સ્થિર શૂન્ય
install_vimrc(પૂર્ણાંક idx UNUSED)
{
    FILE	*fd, *tfd;
    અક્ષર	*fname;

    // If an old vimrc file exists, overwrite it.
    // Otherwise create a new one.
    જો (*oldvimrc != NUL)
	fname = oldvimrc;
    નહીંતર
	fname = vimrc;

    fd = fopen(fname, "w");
    જો (fd == ખાલી)
    {
	printf("\nERROR: Cannot open \"%s\" for writing.\n", fname);
	પાછા;
    }
    પસંદગી (compat_choice)
    {
	સ્થિતિ compat_vi:
		    fprintf(fd, "\" Vi compatible\n");
		    fprintf(fd, "set compatible\n");
		    તોડો;
	સ્થિતિ compat_vim:
		    fprintf(fd, "\" Vim's default behavior\n");
		    fprintf(fd, "if &compatible\n");
		    fprintf(fd, "  set nocompatible\n");
		    fprintf(fd, "endif\n");
		    તોડો;
	સ્થિતિ compat_some_enhancements:
		    fprintf(fd, "\" Vim with some enhancements\n");
		    fprintf(fd, "source $VIMRUNTIME/defaults.vim\n");
		    તોડો;
	સ્થિતિ compat_all_enhancements:
		    fprintf(fd, "\" Vim with all enhancements\n");
		    fprintf(fd, "source $VIMRUNTIME/vimrc_example.vim\n");
		    તોડો;
    }
    પસંદગી (remap_choice)
    {
	સ્થિતિ remap_no:
		    તોડો;
	સ્થિતિ remap_win:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Remap a few keys for Windows behavior\n");
		    fprintf(fd, "source $VIMRUNTIME/mswin.vim\n");
		    તોડો;
    }
    પસંદગી (mouse_choice)
    {
	સ્થિતિ mouse_xterm:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Mouse behavior (the Unix way)\n");
		    fprintf(fd, "behave xterm\n");
		    તોડો;
	સ્થિતિ mouse_mswin:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Mouse behavior (the Windows way)\n");
		    fprintf(fd, "behave mswin\n");
		    તોડો;
	સ્થિતિ mouse_default:
		    તોડો;
    }
    જો ((tfd = fopen("diff.exe", "r")) != ખાલી)
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

    સ્થિર શૂન્ય
change_vimrc_choice(પૂર્ણાંક idx)
{
    જો (choices[idx].installfunc != ખાલી)
    {
	// Switch to NOT change or create a vimrc file.
	જો (*oldvimrc != NUL)
	    alloc_text(idx, "Do NOT change startup file %s", oldvimrc);
	નહીંતર
	    alloc_text(idx, "Do NOT create startup file %s", vimrc);
	choices[idx].installfunc = ખાલી;
	choices[idx + 1].active = 0;
	choices[idx + 2].active = 0;
	choices[idx + 3].active = 0;
    }
    નહીંતર
    {
	// Switch to change or create a vimrc file.
	જો (*oldvimrc != NUL)
	    alloc_text(idx, "Overwrite startup file %s with:", oldvimrc);
	નહીંતર
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
    સ્થિર શૂન્ય
change_run_choice(પૂર્ણાંક idx)
{
    compat_choice = get_choice(compat_choices, TABLE_SIZE(compat_choices));
    alloc_text(idx, compat_text, compat_choices[compat_choice]);
}

/*
 * Change the choice if keys are to be remapped.
 */
    સ્થિર શૂન્ય
change_remap_choice(પૂર્ણાંક idx)
{
    remap_choice = get_choice(remap_choices, TABLE_SIZE(remap_choices));
    alloc_text(idx, remap_text, remap_choices[remap_choice]);
}

/*
 * Change the choice how to select text.
 */
    સ્થિર શૂન્ય
change_mouse_choice(પૂર્ણાંક idx)
{
    mouse_choice = get_choice(mouse_choices, TABLE_SIZE(mouse_choices));
    alloc_text(idx, mouse_text, mouse_choices[mouse_choice]);
}

    સ્થિર શૂન્ય
init_vimrc_choices(શૂન્ય)
{
    // set path for a new _vimrc file (also when not used)
    strcpy(vimrc, installdir);
    strcpy(vimrc + runtimeidx, "_vimrc");

    // Set opposite value and then toggle it by calling change_vimrc_choice()
    જો (*oldvimrc == NUL)
	choices[choice_ગણતરી].installfunc = ખાલી;
    નહીંતર
	choices[choice_ગણતરી].installfunc = install_vimrc;
    choices[choice_ગણતરી].text = ખાલી;
    change_vimrc_choice(choice_ગણતરી);
    choices[choice_ગણતરી].changefunc = change_vimrc_choice;
    choices[choice_ગણતરી].active = 1;
    ++choice_ગણતરી;

    // default way to run Vim
    alloc_text(choice_ગણતરી, compat_text, compat_choices[compat_choice]);
    choices[choice_ગણતરી].changefunc = change_run_choice;
    choices[choice_ગણતરી].installfunc = ખાલી;
    choices[choice_ગણતરી].active = (*oldvimrc == NUL);
    ++choice_ગણતરી;

    // Whether to remap keys
    alloc_text(choice_ગણતરી, remap_text , remap_choices[remap_choice]);
    choices[choice_ગણતરી].changefunc = change_remap_choice;
    choices[choice_ગણતરી].installfunc = ખાલી;
    choices[choice_ગણતરી].active = (*oldvimrc == NUL);
    ++choice_ગણતરી;

    // default way to use the mouse
    alloc_text(choice_ગણતરી, mouse_text, mouse_choices[mouse_choice]);
    choices[choice_ગણતરી].changefunc = change_mouse_choice;
    choices[choice_ગણતરી].installfunc = ખાલી;
    choices[choice_ગણતરી].active = (*oldvimrc == NUL);
    ++choice_ગણતરી;
}

    સ્થિર LONG
reg_create_ચાવી(
    HKEY મૂળ,
    અચલ અક્ષર *subkey,
    PHKEY ph_ચાવી,
    DWORD flag)
{
    DWORD disp;

    *ph_ચાવી = ખાલી;
    પાછા Reg_Create_ચાવી_Ex(
		મૂળ, subkey,
		0, ખાલી, REG_OPTION_NON_VOLATILE,
		flag | ચાવી_WRITE,
		ખાલી, ph_ચાવી, &disp);
}

    સ્થિર LONG
reg_set_string_કિંમત(
    HKEY h_ચાવી,
    અચલ અક્ષર *કિંમત_name,
    અચલ અક્ષર *data)
{
    પાછા Reg_Set_કિંમત_Ex(h_ચાવી, કિંમત_name, 0, REG_SZ,
				     (LPBYTE)data, (DWORD)(1 + strlen(data)));
}

    સ્થિર LONG
reg_create_ચાવી_and_કિંમત(
    HKEY h_મૂળ_ચાવી,
    અચલ અક્ષર *subkey,
    અચલ અક્ષર *કિંમત_name,
    અચલ અક્ષર *data,
    DWORD flag)
{
    HKEY h_ચાવી;
    LONG lRet = reg_create_ચાવી(h_મૂળ_ચાવી, subkey, &h_ચાવી, flag);

    જો (ERROR_SUCCESS == lRet)
    {
	lRet = reg_set_string_કિંમત(h_ચાવી, કિંમત_name, data);
	Reg_Close_ચાવી(h_ચાવી);
    }
    પાછા lRet;
}

    સ્થિર LONG
register_inproc_server(
    HKEY h_મૂળ_ચાવી,
    અચલ અક્ષર *clsid,
    અચલ અક્ષર *extname,
    અચલ અક્ષર *module,
    અચલ અક્ષર *threading_model,
    DWORD flag)
{
    CHAR subkey[BUFSIZE];
    LONG lRet;

    sprintf(subkey, "CLSID\\%s", clsid);
    lRet = reg_create_ચાવી_and_કિંમત(h_મૂળ_ચાવી, subkey, ખાલી, extname, flag);
    જો (ERROR_SUCCESS == lRet)
    {
	sprintf(subkey, "CLSID\\%s\\InProcServer32", clsid);
	lRet = reg_create_ચાવી_and_કિંમત(h_મૂળ_ચાવી, subkey, ખાલી, module, flag);
	જો (ERROR_SUCCESS == lRet)
	{
	    lRet = reg_create_ચાવી_and_કિંમત(h_મૂળ_ચાવી, subkey,
					   "ThreadingModel", threading_model, flag);
	}
    }
    પાછા lRet;
}

    સ્થિર LONG
register_shellex(
    HKEY h_મૂળ_ચાવી,
    અચલ અક્ષર *clsid,
    અચલ અક્ષર *name,
    અચલ અક્ષર *exe_path,
    DWORD flag)
{
    LONG lRet = reg_create_ચાવી_and_કિંમત(
	    h_મૂળ_ચાવી,
	    "*\\shellex\\ContextMenuHandlers\\gvim",
	    ખાલી,
	    clsid,
	    flag);

    જો (ERROR_SUCCESS == lRet)
    {
	lRet = reg_create_ચાવી_and_કિંમત(
		HKEY_LOCAL_MACHINE,
		"Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved",
		clsid,
		name,
		flag);

	જો (ERROR_SUCCESS == lRet)
	{
	    lRet = reg_create_ચાવી_and_કિંમત(
		    HKEY_LOCAL_MACHINE,
		    "Software\\Vim\\Gvim",
		    "path",
		    exe_path,
		    flag);
	}
    }
    પાછા lRet;
}

    સ્થિર LONG
register_openwith(
    HKEY h_મૂળ_ચાવી,
    અચલ અક્ષર *exe_path,
    DWORD flag)
{
    અક્ષર	exe_cmd[BUFSIZE];
    LONG	lRet;

    sprintf(exe_cmd, "\"%s\" \"%%1\"", exe_path);
    lRet = reg_create_ચાવી_and_કિંમત(
	    h_મૂળ_ચાવી,
	    "Applications\\gvim.exe\\shell\\edit\\command",
	    ખાલી,
	    exe_cmd,
	    flag);

    જો (ERROR_SUCCESS == lRet)
    {
	પૂર્ણાંક i;
	સ્થિર અચલ અક્ષર *openwith[] = {
		".htm\\OpenWithList\\gvim.exe",
		".vim\\OpenWithList\\gvim.exe",
		"*\\OpenWithList\\gvim.exe",
	};

	માટે (i = 0; ERROR_SUCCESS == lRet && i < (પૂર્ણાંક)ARRAYSIZE(openwith); i++)
	    lRet = reg_create_ચાવી_and_કિંમત(h_મૂળ_ચાવી, openwith[i], ખાલી, "", flag);
    }

    પાછા lRet;
}

    સ્થિર LONG
register_uninstall(
    HKEY h_મૂળ_ચાવી,
    અચલ અક્ષર *appname,
    અચલ અક્ષર *display_name,
    અચલ અક્ષર *uninstall_string,
    અચલ અક્ષર *display_icon,
    અચલ અક્ષર *display_version,
    અચલ અક્ષર *publisher)
{
    LONG lRet = reg_create_ચાવી_and_કિંમત(h_મૂળ_ચાવી, appname,
			     "DisplayName", display_name, ચાવી_WOW64_64_ચાવી);

    જો (ERROR_SUCCESS == lRet)
	lRet = reg_create_ચાવી_and_કિંમત(h_મૂળ_ચાવી, appname,
		     "UninstallString", uninstall_string, ચાવી_WOW64_64_ચાવી);
    જો (ERROR_SUCCESS == lRet)
	lRet = reg_create_ચાવી_and_કિંમત(h_મૂળ_ચાવી, appname,
		     "DisplayIcon", display_icon, ચાવી_WOW64_64_ચાવી);
    જો (ERROR_SUCCESS == lRet)
	lRet = reg_create_ચાવી_and_કિંમત(h_મૂળ_ચાવી, appname,
		     "DisplayVersion", display_version, ચાવી_WOW64_64_ચાવી);
    જો (ERROR_SUCCESS == lRet)
	lRet = reg_create_ચાવી_and_કિંમત(h_મૂળ_ચાવી, appname,
		     "Publisher", publisher, ચાવી_WOW64_64_ચાવી);
    પાછા lRet;
}

/*
 * Add some entries to the registry:
 * - to add "Edit with Vim" to the context * menu
 * - to add Vim to the "Open with..." list
 * - to uninstall Vim
 */
//ARGSUSED
    સ્થિર પૂર્ણાંક
install_registry(શૂન્ય)
{
    LONG	lRet = ERROR_SUCCESS;
    અચલ અક્ષર	*vim_ext_ThreadingModel = "Apartment";
    અચલ અક્ષર	*vim_ext_name = "Vim Shell Extension";
    અચલ અક્ષર	*vim_ext_clsid = "{51EEE242-AD87-11d3-9C1E-0090278BBD99}";
    અક્ષર	vim_exe_path[MAX_PATH];
    અક્ષર	display_name[BUFSIZE];
    અક્ષર	uninstall_string[BUFSIZE];
    અક્ષર	icon_string[BUFSIZE];
    અક્ષર	version_string[BUFSIZE];
    પૂર્ણાંક		i;
    પૂર્ણાંક		loop_ગણતરી = is_64bit_os() ? 2 : 1;
    DWORD	flag;

    sprintf(vim_exe_path, "%s\\gvim.exe", installdir);

    જો (install_popup)
    {
	અક્ષર	    bufg[BUFSIZE];

	printf("Creating \"Edit with Vim\" popup menu entry\n");

	માટે (i = 0; i < loop_ગણતરી; i++)
	{
	    જો (i == 0)
	    {
		sprintf(bufg, "%s\\" GVIMEXT32_PATH, installdir);
		flag = ચાવી_WOW64_32_ચાવી;
	    }
	    નહીંતર
	    {
		sprintf(bufg, "%s\\" GVIMEXT64_PATH, installdir);
		flag = ચાવી_WOW64_64_ચાવી;
	    }

	    lRet = register_inproc_server(
		    HKEY_CLASSES_મૂળ, vim_ext_clsid, vim_ext_name,
		    bufg, vim_ext_ThreadingModel, flag);
	    જો (ERROR_SUCCESS != lRet)
		પાછા FAIL;
	    lRet = register_shellex(
		    HKEY_CLASSES_મૂળ, vim_ext_clsid, vim_ext_name,
		    vim_exe_path, flag);
	    જો (ERROR_SUCCESS != lRet)
		પાછા FAIL;
	}
    }

    જો (install_openwith)
    {
	printf("Creating \"Open with ...\" list entry\n");

	માટે (i = 0; i < loop_ગણતરી; i++)
	{
	    જો (i == 0)
		flag = ચાવી_WOW64_32_ચાવી;
	    નહીંતર
		flag = ચાવી_WOW64_64_ચાવી;

	    lRet = register_openwith(HKEY_CLASSES_મૂળ, vim_exe_path, flag);
	    જો (ERROR_SUCCESS != lRet)
		પાછા FAIL;
	}
    }

    printf("Creating an uninstall entry\n");
    sprintf(display_name, "Vim " VIM_VERSION_SHORT
#ifdef _M_ARM64
	    " (arm64)"
#અથવાજો _M_X64
	    " (x64)"
#endif
	    );

    // For the NSIS installer use the generated uninstaller.
    જો (interactive)
	sprintf(uninstall_string, "%s\\uninstall.exe", installdir);
    નહીંતર
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
    જો (ERROR_SUCCESS != lRet)
	પાછા FAIL;

    પાછા OK;
}

    સ્થિર શૂન્ય
change_popup_choice(પૂર્ણાંક idx)
{
    જો (install_popup == 0)
    {
	choices[idx].text = "Install an entry for Vim in the popup menu for the right\n    mouse button so that you can edit any file with Vim";
	install_popup = 1;
    }
    નહીંતર
    {
	choices[idx].text = "Do NOT install an entry for Vim in the popup menu for the\n    right mouse button to edit any file with Vim";
	install_popup = 0;
    }
}

/*
 * Only add the choice for the popup menu entry when gvim.exe was found and
 * both gvimext.dll and regedit.exe exist.
 */
    સ્થિર શૂન્ય
init_popup_choice(શૂન્ય)
{
    સંરચના stat	st;

    જો (has_gvim
	    && (stat(GVIMEXT32_PATH, &st) >= 0
		|| stat(GVIMEXT64_PATH, &st) >= 0))
    {
	choices[choice_ગણતરી].changefunc = change_popup_choice;
	choices[choice_ગણતરી].installfunc = ખાલી;
	choices[choice_ગણતરી].active = 1;
	change_popup_choice(choice_ગણતરી);  // set the text
	++choice_ગણતરી;
    }
    નહીંતર
	add_dummy_choice();
}

    સ્થિર શૂન્ય
change_openwith_choice(પૂર્ણાંક idx)
{
    જો (install_openwith == 0)
    {
	choices[idx].text = "Add Vim to the \"Open With...\" list in the popup menu for the right\n    mouse button so that you can edit any file with Vim";
	install_openwith = 1;
    }
    નહીંતર
    {
	choices[idx].text = "Do NOT add Vim to the \"Open With...\" list in the popup menu for the\n    right mouse button to edit any file with Vim";
	install_openwith = 0;
    }
}

/*
 * Only add the choice for the open-with menu entry when gvim.exe was found
 * and regedit.exe exist.
 */
    સ્થિર શૂન્ય
init_openwith_choice(શૂન્ય)
{
    જો (has_gvim)
    {
	choices[choice_ગણતરી].changefunc = change_openwith_choice;
	choices[choice_ગણતરી].installfunc = ખાલી;
	choices[choice_ગણતરી].active = 1;
	change_openwith_choice(choice_ગણતરી);  // set the text
	++choice_ગણતરી;
    }
    નહીંતર
	add_dummy_choice();
}

/*
 * Create a shell link.
 *
 * returns 0 on failure, non-zero on successful completion.
 *
 * NOTE:  Currently untested with mingw.
 */
    પૂર્ણાંક
create_shortcut(
	અચલ અક્ષર *shortcut_name,
	અચલ અક્ષર *iconfile_path,
	પૂર્ણાંક	    iconindex,
	અચલ અક્ષર *shortcut_target,
	અચલ અક્ષર *shortcut_args,
	અચલ અક્ષર *workingdir
	)
{
    IShellLink	    *shelllink_ptr;
    HRESULT	    hres;
    IPersistFile	    *persistfile_ptr;

    // Initialize COM library
    hres = CoInitialize(ખાલી);
    જો (!SUCCEEDED(hres))
    {
	printf("Error:  Could not open the COM library.  Not creating shortcut.\n");
	પાછા FAIL;
    }

    // Instantiate a COM object for the ShellLink, store a pointer to it
    // in shelllink_ptr.
    hres = CoCreateInstance(&CLSID_ShellLink,
			   ખાલી,
			   CLSCTX_INPROC_SERVER,
			   &IID_IShellLink,
			   (શૂન્ય **) &shelllink_ptr);

    જો (SUCCEEDED(hres)) // If the instantiation was successful...
    {
	// ...Then build a PersistFile interface for the ShellLink so we can
	// save it as a file after we build it.
	hres = shelllink_ptr->lpVtbl->QueryInterface(shelllink_ptr,
		&IID_IPersistFile, (શૂન્ય **) &persistfile_ptr);

	જો (SUCCEEDED(hres))
	{
	    wchar_t wsz[BUFSIZE];

	    // translate the (possibly) multibyte shortcut filename to windows
	    // Unicode so it can be used as a file name.
	    MultiByteToWideChar(CP_ACP, 0, shortcut_name, -1, wsz, કદ(wsz)/કદ(wsz[0]));

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
	નહીંતર
	{
	    printf("QueryInterface Error\n");
	    પાછા FAIL;
	}

	// Return the ShellLink object
	shelllink_ptr->lpVtbl->Release(shelllink_ptr);
    }
    નહીંતર
    {
	printf("CoCreateInstance Error - hres = %08x\n", (પૂર્ણાંક)hres);
	પાછા FAIL;
    }

    પાછા OK;
}

/*
 * Build a path to where we will put a specified link.
 *
 * Return 0 on error, non-zero on success
 */
   પૂર્ણાંક
build_link_name(
	અક્ષર	   *link_path,
	અચલ અક્ષર *link_name,
	અચલ અક્ષર *shell_folder_name)
{
    અક્ષર	shell_folder_path[MAX_PATH];

    જો (get_shell_folder_path(shell_folder_path, shell_folder_name) == FAIL)
    {
	printf("An error occurred while attempting to find the path to %s.\n",
							   shell_folder_name);
	પાછા FAIL;
    }

    // Make sure the directory exists (create Start Menu\Programs\Vim).
    // Ignore errors if it already exists.
    vim_mkdir(shell_folder_path, 0755);

    // build the path to the shortcut and the path to gvim.exe
    sprintf(link_path, "%s\\%s.lnk", shell_folder_path, link_name);

    પાછા OK;
}

    સ્થિર પૂર્ણાંક
build_shortcut(
	અચલ અક્ષર *name,	// Name of the shortcut
	અચલ અક્ષર *exename,	// Name of the executable (e.g., vim.exe)
	અચલ અક્ષર *args,
	અચલ અક્ષર *shell_folder,
	અચલ અક્ષર *workingdir)
{
    અક્ષર	executable_path[BUFSIZE];
    અક્ષર	link_name[BUFSIZE];

    sprintf(executable_path, "%s\\%s", installdir, exename);

    જો (build_link_name(link_name, name, shell_folder) == FAIL)
    {
	printf("An error has occurred.  A shortcut to %s will not be created %s.\n",
		name,
		*shell_folder == 'd' ? "on the desktop" : "in the Start menu");
	પાછા FAIL;
    }

    // Create the shortcut:
    પાછા create_shortcut(link_name, executable_path, 0,
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
    સ્થિર શૂન્ય
install_start_menu(પૂર્ણાંક idx UNUSED)
{
    need_uninstall_entry = 1;
    printf("Creating start menu\n");
    જો (has_vim)
    {
	જો (build_shortcut("Vim", "vim.exe", "",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    પાછા;
	જો (build_shortcut("Vim Read-only", "vim.exe", "-R",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    પાછા;
	જો (build_shortcut("Vim Diff", "vim.exe", "-d",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    પાછા;
    }
    જો (has_gvim)
    {
	જો (build_shortcut("gVim", "gvim.exe", "",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    પાછા;
	જો (build_shortcut("gVim Easy", "gvim.exe", "-y",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    પાછા;
	જો (build_shortcut("gVim Read-only", "gvim.exe", "-R",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    પાછા;
	જો (build_shortcut("gVim Diff", "gvim.exe", "-d",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    પાછા;
    }
    જો (build_shortcut("Uninstall",
		interactive ? "uninstall.exe" : "uninstall-gui.exe", "",
					   VIM_STARTMENU, installdir) == FAIL)
	પાછા;
    // For Windows NT the working dir of the vimtutor.bat must be right,
    // otherwise gvim.exe won't be found and using gvimbat doesn't work.
    જો (build_shortcut("Vim tutor", "vimtutor.bat", "",
					   VIM_STARTMENU, installdir) == FAIL)
	પાછા;
    જો (build_shortcut("Help", has_gvim ? "gvim.exe" : "vim.exe", "-c h",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	પાછા;
    {
	અક્ષર	shell_folder_path[BUFSIZE];

	// Creating the URL shortcut works a bit differently...
	જો (get_shell_folder_path(shell_folder_path, VIM_STARTMENU) == FAIL)
	{
	    printf("Finding the path of the Start menu failed\n");
	    પાછા ;
	}
	add_pathsep(shell_folder_path);
	strcat(shell_folder_path, "Vim Online.url");
	જો (!WritePrivateProfileString("InternetShortcut", "URL",
				    "https://www.vim.org/", shell_folder_path))
	{
	    printf("Creating the Vim online URL failed\n");
	    પાછા;
	}
    }
}

    સ્થિર શૂન્ય
toggle_startmenu_choice(પૂર્ણાંક idx)
{
    જો (choices[idx].installfunc == ખાલી)
    {
	choices[idx].installfunc = install_start_menu;
	choices[idx].text = "Add Vim to the Start menu";
    }
    નહીંતર
    {
	choices[idx].installfunc = ખાલી;
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
    શૂન્ય
install_shortcut_gvim(પૂર્ણાંક idx)
{
    // Create shortcut(s) on the desktop
    જો (choices[idx].arg)
    {
	(શૂન્ય)build_shortcut(icon_names[0], "gvim.exe",
						      "", "desktop", WORKDIR);
	need_uninstall_entry = 1;
    }
}

    શૂન્ય
install_shortcut_evim(પૂર્ણાંક idx)
{
    જો (choices[idx].arg)
    {
	(શૂન્ય)build_shortcut(icon_names[1], "gvim.exe",
						    "-y", "desktop", WORKDIR);
	need_uninstall_entry = 1;
    }
}

    શૂન્ય
install_shortcut_gview(પૂર્ણાંક idx)
{
    જો (choices[idx].arg)
    {
	(શૂન્ય)build_shortcut(icon_names[2], "gvim.exe",
						    "-R", "desktop", WORKDIR);
	need_uninstall_entry = 1;
    }
}

    શૂન્ય
toggle_shortcut_choice(પૂર્ણાંક idx)
{
    અક્ષર	*arg;

    જો (choices[idx].installfunc == install_shortcut_gvim)
	arg = "gVim";
    નહીંતર જો (choices[idx].installfunc == install_shortcut_evim)
	arg = "gVim Easy";
    નહીંતર
	arg = "gVim Read-only";
    જો (choices[idx].arg)
    {
	choices[idx].arg = 0;
	alloc_text(idx, "Do NOT create a desktop icon for %s", arg);
    }
    નહીંતર
    {
	choices[idx].arg = 1;
	alloc_text(idx, "Create a desktop icon for %s", arg);
    }
}

    સ્થિર શૂન્ય
init_startmenu_choice(શૂન્ય)
{
    // Start menu
    choices[choice_ગણતરી].changefunc = toggle_startmenu_choice;
    choices[choice_ગણતરી].installfunc = ખાલી;
    choices[choice_ગણતરી].active = 1;
    toggle_startmenu_choice(choice_ગણતરી);	// set the text
    ++choice_ગણતરી;
}

/*
 * Add the choice for the desktop shortcuts.
 */
    સ્થિર શૂન્ય
init_shortcut_choices(શૂન્ય)
{
    // Shortcut to gvim
    choices[choice_ગણતરી].text = ખાલી;
    choices[choice_ગણતરી].arg = 0;
    choices[choice_ગણતરી].active = has_gvim;
    choices[choice_ગણતરી].changefunc = toggle_shortcut_choice;
    choices[choice_ગણતરી].installfunc = install_shortcut_gvim;
    toggle_shortcut_choice(choice_ગણતરી);
    ++choice_ગણતરી;

    // Shortcut to evim
    choices[choice_ગણતરી].text = ખાલી;
    choices[choice_ગણતરી].arg = 0;
    choices[choice_ગણતરી].active = has_gvim;
    choices[choice_ગણતરી].changefunc = toggle_shortcut_choice;
    choices[choice_ગણતરી].installfunc = install_shortcut_evim;
    toggle_shortcut_choice(choice_ગણતરી);
    ++choice_ગણતરી;

    // Shortcut to gview
    choices[choice_ગણતરી].text = ખાલી;
    choices[choice_ગણતરી].arg = 0;
    choices[choice_ગણતરી].active = has_gvim;
    choices[choice_ગણતરી].changefunc = toggle_shortcut_choice;
    choices[choice_ગણતરી].installfunc = install_shortcut_gview;
    toggle_shortcut_choice(choice_ગણતરી);
    ++choice_ગણતરી;
}

/*
 * Attempt to register OLE for Vim.
 */
   સ્થિર શૂન્ય
install_OLE_register(શૂન્ય)
{
    અક્ષર register_command_string[BUFSIZE + 30];

    printf("\n--- Attempting to register Vim with OLE ---\n");
    printf("(There is no message whether this works or not.)\n");

    sprintf(register_command_string, "\"%s\\gvim.exe\" -silent -register", installdir);
    system(register_command_string);
}

/*
 * Remove the last part of directory "path[]" to get its parent, and put the
 * result in "to[]".
 */
    સ્થિર શૂન્ય
dir_remove_last(અચલ અક્ષર *path, અક્ષર to[MAX_PATH])
{
    અક્ષર c;
    long last_char_to_copy;
    long path_length = strlen(path);

    // skip the last character just in case it is a '\\'
    last_char_to_copy = path_length - 2;
    c = path[last_char_to_copy];

    જ્યારે (c != '\\')
    {
	last_char_to_copy--;
	c = path[last_char_to_copy];
    }

    strncpy(to, path, (size_t)last_char_to_copy);
    to[last_char_to_copy] = NUL;
}

    સ્થિર શૂન્ય
set_directories_text(પૂર્ણાંક idx)
{
    પૂર્ણાંક vimfiles_dir_choice = choices[idx].arg;

    જો (vimfiles_dir_choice == (પૂર્ણાંક)vimfiles_dir_none)
	alloc_text(idx, "Do NOT create plugin directories%s", "");
    નહીંતર
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
સ્થિર અક્ષર *homedir = ખાલી;

    શૂન્ય
init_homedir(શૂન્ય)
{
    અક્ષર    *var;
    અક્ષર    buf[MAX_PATH];

    જો (homedir != ખાલી)
    {
	મુક્ત(homedir);
	homedir = ખાલી;
    }

    var = getenv("HOME");

    /*
     * Typically, $HOME is not defined on Windows, unless the user has
     * specifically defined it for Vim's sake.  However, on Windows NT
     * platforms, $HOMEDRIVE and $HOMEPATH are automatically defined for
     * each user.  Try constructing $HOME from these.
     */
    જો (var == ખાલી || *var == NUL)
    {
	અક્ષર	*homedrive, *homepath;

	homedrive = getenv("HOMEDRIVE");
	homepath = getenv("HOMEPATH");
	જો (homepath == ખાલી || *homepath == NUL)
	    homepath = "\\";
	જો (homedrive != ખાલી
		   && strlen(homedrive) + strlen(homepath) < કદ(buf))
	{
	    જો (snprintf(buf, કદ(buf), "%s%s", homedrive, homepath) > 0
		&& buf[0] != NUL)
		var = buf;
	}
    }

    જો (var == ખાલી)
	var = getenv("USERPROFILE");

    /*
     * Weird but true: $HOME may contain an indirect reference to another
     * variable, esp. "%USERPROFILE%".  Happens when $USERPROFILE isn't set
     * when $HOME is being set.
     */
    જો (var != ખાલી && *var == '%')
    {
	અક્ષર	*p;
	અક્ષર	*exp;

	p = strchr(var + 1, '%');
	જો (p != ખાલી)
	{
	    strncpy(buf, var + 1, p - (var + 1));
	    buf[p - (var + 1)] = NUL;
	    exp = getenv(buf);
	    જો (exp != ખાલી && *exp != NUL
				&& strlen(exp) + strlen(p) < કદ(buf))
	    {
		snprintf(buf, કદ(buf), "%s%s", exp, p + 1);
		var = buf;
	    }
	}
    }

    જો (var != ખાલી && *var == NUL)	// empty is same as not set
	var = ખાલી;

    જો (var == ખાલી)
	homedir = ખાલી;
    નહીંતર
	homedir = _strdup(var);
}

/*
 * Change the directory that the vim plugin directories will be created in:
 * $HOME, $VIM or nowhere.
 */
    સ્થિર શૂન્ય
change_directories_choice(પૂર્ણાંક idx)
{
    પૂર્ણાંક	    choice_ગણતરી = TABLE_SIZE(vimfiles_dir_choices);

    // Don't offer the $HOME choice if $HOME isn't set.
    જો (homedir == ખાલી)
	--choice_ગણતરી;
    choices[idx].arg = get_choice(vimfiles_dir_choices, choice_ગણતરી);
    set_directories_text(idx);
}

/*
 * Create the plugin directories...
 */
//ARGSUSED
    સ્થિર શૂન્ય
install_vimfilesdir(પૂર્ણાંક idx)
{
    પૂર્ણાંક i;
    પૂર્ણાંક vimfiles_dir_choice = choices[idx].arg;
    અક્ષર *p;
    અક્ષર vimdir_path[MAX_PATH];
    અક્ષર vimfiles_path[MAX_PATH + 9];
    અક્ષર tmp_dirname[BUFSIZE];

    // switch on the location that the user wants the plugin directories
    // built in
    પસંદગી (vimfiles_dir_choice)
    {
	સ્થિતિ vimfiles_dir_vim:
	{
	    // Go to the %VIM% directory - check env first, then go one dir
	    //	   below installdir if there is no %VIM% environment variable.
	    //	   The accuracy of $VIM is checked in inspect_system(), so we
	    //	   can be sure it is ok to use here.
	    p = getenv("VIM");
	    જો (p == ખાલી) // No $VIM in path
		dir_remove_last(installdir, vimdir_path);
	    નહીંતર
		strcpy(vimdir_path, p);
	    તોડો;
	}
	સ્થિતિ vimfiles_dir_home:
	{
	    // Find the $HOME directory.  Its existence was already checked.
	    p = homedir;
	    જો (p == ખાલી)
	    {
		printf("Internal error: $HOME is NULL\n");
		p = "c:\\";
	    }
	    strcpy(vimdir_path, p);
	    તોડો;
	}
	સ્થિતિ vimfiles_dir_none:
	{
	    // Do not create vim plugin directory.
	    પાછા;
	}
    }

    // Now, just create the directory.	If it already exists, it will fail
    // silently.
    sprintf(vimfiles_path, "%s\\vimfiles", vimdir_path);
    vim_mkdir(vimfiles_path, 0755);

    printf("Creating the following directories in \"%s\":\n", vimfiles_path);
    માટે (i = 0; i < TABLE_SIZE(vimfiles_subdirs); i++)
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
    સ્થિર શૂન્ય
init_directories_choice(શૂન્ય)
{
    સંરચના stat	st;
    અક્ષર	tmp_dirname[BUFSIZE];
    અક્ષર	*p;
    પૂર્ણાંક		vimfiles_dir_choice;

    choices[choice_ગણતરી].text = alloc(150);
    choices[choice_ગણતરી].changefunc = change_directories_choice;
    choices[choice_ગણતરી].installfunc = install_vimfilesdir;
    choices[choice_ગણતરી].active = 1;

    // Check if the "compiler" directory already exists.  That's a good
    // indication that the plugin directories were already created.
    p = getenv("HOME");
    જો (p != ખાલી)
    {
	vimfiles_dir_choice = (પૂર્ણાંક)vimfiles_dir_home;
	sprintf(tmp_dirname, "%s\\vimfiles\\compiler", p);
	જો (stat(tmp_dirname, &st) == 0)
	    vimfiles_dir_choice = (પૂર્ણાંક)vimfiles_dir_none;
    }
    નહીંતર
    {
	vimfiles_dir_choice = (પૂર્ણાંક)vimfiles_dir_vim;
	p = getenv("VIM");
	જો (p == ખાલી)  // No $VIM in path, use the install dir.
	    dir_remove_last(installdir, tmp_dirname);
	નહીંતર
	    strcpy(tmp_dirname, p);
	strcat(tmp_dirname, "\\vimfiles\\compiler");
	જો (stat(tmp_dirname, &st) == 0)
	    vimfiles_dir_choice = (પૂર્ણાંક)vimfiles_dir_none;
    }

    choices[choice_ગણતરી].arg = vimfiles_dir_choice;
    set_directories_text(choice_ગણતરી);
    ++choice_ગણતરી;
}

/*
 * Setup the choices and the default values.
 */
    સ્થિર શૂન્ય
setup_choices(શૂન્ય)
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

    સ્થિર શૂન્ય
print_cmd_લીટી_help(શૂન્ય)
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
    સ્થિર શૂન્ય
command_લીટી_setup_choices(પૂર્ણાંક argc, અક્ષર **argv)
{
    પૂર્ણાંક i, j;

    માટે (i = 1; i < argc; i++)
    {
	જો (strcmp(argv[i], "-create-batfiles") == 0)
	{
	    જો (i + 1 == argc)
		ચાલુ;
	    જ્યારે (argv[i + 1][0] != '-' && i < argc)
	    {
		i++;
		માટે (j = 1; j < TARGET_ગણતરી; ++j)
		    જો ((targets[j].exenamearg[0] == 'g' ? has_gvim : has_vim)
			    && strcmp(argv[i], targets[j].name) == 0)
		    {
			init_bat_choice(j);
			તોડો;
		    }
		જો (j == TARGET_ગણતરી)
		    printf("%s is not a valid choice for -create-batfiles\n",
								     argv[i]);

		જો (i + 1 == argc)
		    તોડો;
	    }
	}
	નહીંતર જો (strcmp(argv[i], "-create-vimrc") == 0)
	{
	    // Setup default vimrc choices.  If there is already a _vimrc file,
	    // it will NOT be overwritten.
	    init_vimrc_choices();
	}
	નહીંતર જો (strcmp(argv[i], "-vimrc-remap") == 0)
	{
	    જો (i + 1 == argc)
		તોડો;
	    i++;
	    જો (strcmp(argv[i], "no") == 0)
		remap_choice = remap_no;
	    નહીંતર જો (strcmp(argv[i], "win") == 0)
		remap_choice = remap_win;
	}
	નહીંતર જો (strcmp(argv[i], "-vimrc-behave") == 0)
	{
	    જો (i + 1 == argc)
		તોડો;
	    i++;
	    જો (strcmp(argv[i], "unix") == 0)
		mouse_choice = mouse_xterm;
	    નહીંતર જો (strcmp(argv[i], "mswin") == 0)
		mouse_choice = mouse_mswin;
	    નહીંતર જો (strcmp(argv[i], "default") == 0)
		mouse_choice = mouse_default;
	}
	નહીંતર જો (strcmp(argv[i], "-vimrc-compat") == 0)
	{
	    જો (i + 1 == argc)
		તોડો;
	    i++;
	    જો (strcmp(argv[i], "vi") == 0)
		compat_choice = compat_vi;
	    નહીંતર જો (strcmp(argv[i], "vim") == 0)
		compat_choice = compat_vim;
	    નહીંતર જો (strcmp(argv[i], "defaults") == 0)
		compat_choice = compat_some_enhancements;
	    નહીંતર જો (strcmp(argv[i], "all") == 0)
		compat_choice = compat_all_enhancements;
	}
	નહીંતર જો (strcmp(argv[i], "-install-popup") == 0)
	{
	    init_popup_choice();
	}
	નહીંતર જો (strcmp(argv[i], "-install-openwith") == 0)
	{
	    init_openwith_choice();
	}
	નહીંતર જો (strcmp(argv[i], "-add-start-menu") == 0)
	{
	    init_startmenu_choice();
	}
	નહીંતર જો (strcmp(argv[i], "-install-icons") == 0)
	{
	    init_shortcut_choices();
	}
	નહીંતર જો (strcmp(argv[i], "-create-directories") == 0)
	{
	    પૂર્ણાંક vimfiles_dir_choice = (પૂર્ણાંક)vimfiles_dir_none;

	    init_directories_choice();
	    જો (i + 1 < argc && argv[i + 1][0] != '-')
	    {
		i++;
		જો (strcmp(argv[i], "vim") == 0)
		    vimfiles_dir_choice = (પૂર્ણાંક)vimfiles_dir_vim;
		નહીંતર જો (strcmp(argv[i], "home") == 0)
		{
		    જો (homedir == ખાલી)  // No $HOME in environment
			vimfiles_dir_choice = (પૂર્ણાંક)vimfiles_dir_none;
		    નહીંતર
			vimfiles_dir_choice = (પૂર્ણાંક)vimfiles_dir_home;
		}
		નહીંતર
		{
		    printf("Unknown argument for -create-directories: %s\n",
								     argv[i]);
		    print_cmd_લીટી_help();
		}
	    }
	    નહીંતર // No choice specified, default to vim directory
		vimfiles_dir_choice = (પૂર્ણાંક)vimfiles_dir_vim;
	    choices[choice_ગણતરી - 1].arg = vimfiles_dir_choice;
	}
	નહીંતર જો (strcmp(argv[i], "-register-OLE") == 0)
	{
	    // This is always done when gvim is found
	}
	નહીંતર // Unknown switch
	{
	    printf("Got unknown argument argv[%d] = %s\n", i, argv[i]);
	    print_cmd_લીટી_help();
	}
    }
}


/*
 * Show a few screens full of helpful information.
 */
    સ્થિર શૂન્ય
show_help(શૂન્ય)
{
    સ્થિર અક્ષર *(વસ્તુઓ[]) =
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
ખાલી
    };
    પૂર્ણાંક		i;
    પૂર્ણાંક		c;

    rewind(stdin);
    printf("\n");
    માટે (i = 0; વસ્તુઓ[i] != ખાલી; ++i)
    {
	puts(વસ્તુઓ[i]);
	printf("Hit Enter to continue, b (back) or q (quit help): ");
	c = getchar();
	rewind(stdin);
	જો (c == 'b' || c == 'B')
	{
	    જો (i == 0)
		--i;
	    નહીંતર
		i -= 2;
	}
	જો (c == 'q' || c == 'Q')
	    તોડો;
	printf("\n");
    }
}

/*
 * Install the choices.
 */
    સ્થિર શૂન્ય
install(શૂન્ય)
{
    પૂર્ણાંક		i;

    // Install the selected choices.
    માટે (i = 0; i < choice_ગણતરી; ++i)
	જો (choices[i].installfunc != ખાલી && choices[i].active)
	    (choices[i].installfunc)(i);

    // Add some entries to the registry, if needed.
    જો (install_popup
	    || install_openwith
	    || (need_uninstall_entry && interactive)
	    || !interactive)
	install_registry();

    // Register gvim with OLE.
    જો (has_gvim)
	install_OLE_register();
}

/*
 * request_choice
 */
    સ્થિર શૂન્ય
request_choice(શૂન્ય)
{
    પૂર્ણાંક		      i;

    printf("\n\nInstall will do for you:\n");
    માટે (i = 0; i < choice_ગણતરી; ++i)
	જો (choices[i].active)
	    printf("%2d  %s\n", i + 1, choices[i].text);
    printf("To change an item, enter its number\n\n");
    printf("Enter item number, h (help), d (do it) or q (quit): ");
}

    પૂર્ણાંક
મુખ્ય(પૂર્ણાંક argc, અક્ષર **argv)
{
    પૂર્ણાંક		i;
    અક્ષર	buf[BUFSIZE];

    /*
     * Run interactively if there are no command line arguments.
     */
    જો (argc > 1)
	interactive = 0;
    નહીંતર
	interactive = 1;

    // Initialize this program.
    do_inits(argv);
    init_homedir();

    જો (argc > 1 && strcmp(argv[1], "-uninstall-check") == 0)
    {
	// Only check for already installed Vims.  Used by NSIS installer.
	i = uninstall_check(1);

	// Find the value of $VIM, because NSIS isn't able to do this by
	// itself.
	get_vim_env();

	// When nothing found exit quietly.  If something found wait for
	// a little while, so that the user can read the messages.
	જો (i && _isatty(1))
	    sleep(3);
	exit(0);
    }

    printf("This program sets up the installation of Vim "
						   VIM_VERSION_MEDIUM "\n\n");

    // Check if the user unpacked the archives properly.
    check_unpack();

    // Check for already installed Vims.
    જો (interactive)
	uninstall_check(0);

    // Find out information about the system.
    inspect_system();

    જો (interactive)
    {
	// Setup all the choices.
	setup_choices();

	// Let the user change choices and finally install (or quit).
	માટે (;;)
	{
	    request_choice();
	    rewind(stdin);
	    જો (scanf("%99s", buf) == 1)
	    {
		જો (isdigit((unsigned અક્ષર)buf[0]))
		{
		    // Change a choice.
		    i = atoi(buf);
		    જો (i > 0 && i <= choice_ગણતરી && choices[i - 1].active)
			(choices[i - 1].changefunc)(i - 1);
		    નહીંતર
			printf("\nIllegal choice\n");
		}
		નહીંતર જો (buf[0] == 'h' || buf[0] == 'H')
		{
		    // Help
		    show_help();
		}
		નહીંતર જો (buf[0] == 'd' || buf[0] == 'D')
		{
		    // Install!
		    install();
		    printf("\nThat finishes the installation.  Happy Vimming!\n");
		    તોડો;
		}
		નહીંતર જો (buf[0] == 'q' || buf[0] == 'Q')
		{
		    // Quit
		    printf("\nExiting without anything done\n");
		    તોડો;
		}
		નહીંતર
		    printf("\nIllegal choice\n");
	    }
	}
	printf("\n");
	myexit(0);
    }
    નહીંતર
    {
	/*
	 * Run non-interactive - setup according to the command line switches
	 */
	command_લીટી_setup_choices(argc, argv);
	install();

	// Avoid that the user has to hit Enter, just wait a little bit to
	// allow reading the messages.
	sleep(2);
    }

    પાછા 0;
}
