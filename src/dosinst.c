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
#अन्तर्भाव "dosinst.h"
#अन्तर्भाव <io.h>

#define GVIMEXT64_PATH	    "GvimExt64\\gvimext.dll"
#define GVIMEXT32_PATH	    "GvimExt32\\gvimext.dll"

// Macro to do an error check I was typing over and over
#define CHECK_REG_ERROR(code) \
    करेहि { \
	जइ (code != ERROR_SUCCESS) \
	{ \
	    printf("%ld error number:  %ld\n", (long)__LINE__, (long)code); \
	    प्रत्यागम 1; \
	} \
    } जाव (0)

पूर्णाङ्क	has_vim = 0;		// installable vim.exe exists
पूर्णाङ्क	has_gvim = 0;		// installable gvim.exe exists

वर्ण	oldvimrc[BUFSIZE];	// name of existing vimrc file
वर्ण	vimrc[BUFSIZE];		// name of vimrc file to create

वर्ण	*default_bat_dir = रिक्त;  // when not NULL, use this as the default
				  // directory to write .bat files in
वर्ण	*default_vim_dir = रिक्त;  // when not NULL, use this as the default
				  // install dir for NSIS

/*
 * Structure used for each choice the user can make.
 */
संरचना choice
{
    पूर्णाङ्क	    active;			// non-zero when choice is active
    वर्ण    *text;			// text displayed for this choice
    शून्य    (*changefunc)(पूर्णाङ्क idx);	// function to change this choice
    पूर्णाङ्क	    arg;			// argument for function
    शून्य    (*installfunc)(पूर्णाङ्क idx);	// function to install this choice
};

संरचना choice	choices[30];		// choices the user can make
पूर्णाङ्क		choice_गणनम् = 0;	// number of choices available

#define TABLE_SIZE(s)	(पूर्णाङ्क)ARRAYSIZE(s)

गणना
{
    compat_vi = 1,
    compat_vim,
    compat_some_enhancements,
    compat_all_enhancements
};
वर्ण	*(compat_choices[]) =
{
    "\nChoose the default way to run Vim:",
    "Vi compatible",
    "Vim default",
    "with some Vim enhancements",
    "with syntax highlighting and other features switched on",
};
पूर्णाङ्क	compat_choice = (पूर्णाङ्क)compat_all_enhancements;
वर्ण	*compat_text = "- run Vim %s";

गणना
{
    remap_no = 1,
    remap_win
};
वर्ण	*(remap_choices[]) =
{
    "\nChoose:",
    "Do not remap keys for Windows behavior",
    "Remap a few keys for Windows behavior (CTRL-V, CTRL-C, CTRL-F, etc)",
};
पूर्णाङ्क	remap_choice = (पूर्णाङ्क)remap_no;
वर्ण	*remap_text = "- %s";

गणना
{
    mouse_xterm = 1,
    mouse_mswin,
    mouse_default
};
वर्ण	*(mouse_choices[]) =
{
    "\nChoose the way how Vim uses the mouse:",
    "right button extends selection (the Unix way)",
    "right button has a popup menu, left button starts select mode (the Windows way)",
    "right button has a popup menu, left button starts visual mode",
};
पूर्णाङ्क	mouse_choice = (पूर्णाङ्क)mouse_default;
वर्ण	*mouse_text = "- The mouse %s";

गणना
{
    vimfiles_dir_none = 1,
    vimfiles_dir_vim,
    vimfiles_dir_home
};
स्थिर वर्ण *(vimfiles_dir_choices[]) =
{
    "\nCreate plugin directories:",
    "No",
    "In the VIM directory",
    "In your HOME directory",
};

// non-zero when selected to install the popup menu entry.
स्थिर पूर्णाङ्क	install_popup = 0;

// non-zero when selected to install the "Open with" entry.
स्थिर पूर्णाङ्क	install_openwith = 0;

// non-zero when need to add an uninstall entry in the registry
स्थिर पूर्णाङ्क	need_uninstall_entry = 0;

/*
 * Definitions of the directory name (under $VIM) of the vimfiles directory
 * and its subdirectories:
 */
स्थिर वर्ण	*(vimfiles_subdirs[]) =
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
    स्थिर पूर्णाङ्क
get_choice(वर्ण **table, पूर्णाङ्क entries)
{
    पूर्णाङ्क		answer;
    पूर्णाङ्क		idx;
    वर्ण	dummy[100];

    करेहि
    {
	प्रति (idx = 0; idx < entries; ++idx)
	{
	    जइ (idx)
		printf("%2d  ", idx);
	    puts(table[idx]);
	}
	printf("Choice: ");
	जइ (scanf("%d", &answer) != 1)
	{
	    scanf("%99s", dummy);
	    answer = 0;
	}
    }
    जाव (answer < 1 || answer >= entries);

    प्रत्यागम answer;
}

/*
 * Check if the user unpacked the archives properly.
 * Sets "runtimeidx".
 */
    स्थिर शून्य
check_unpack(शून्य)
{
    वर्ण	buf[BUFSIZE];
    FILE	*fd;
    संरचना stat	st;

    // check for presence of the correct version number in installdir[]
    runtimeidx = strlen(installdir) - strlen(VIM_VERSION_NODOT);
    जइ (runtimeidx <= 0
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
    जइ (stat(buf, &st) < 0)
    {
	printf("ERROR: Cannot find filetype.vim in \"%s\"\n", installdir);
	printf("It looks like you did not unpack the runtime archive.\n");
	printf("You must unpack the runtime archive \"%srt.zip\" before installing.\n",
		VIM_VERSION_NODOT);
	myexit(1);
    }

    // Check if vim.exe or gvim.exe is in the current directory.
    जइ ((fd = fopen("gvim.exe", "r")) != रिक्त)
    {
	fclose(fd);
	has_gvim = 1;
    }
    जइ ((fd = fopen("vim.exe", "r")) != रिक्त)
    {
	fclose(fd);
	has_vim = 1;
    }
    जइ (!has_gvim && !has_vim)
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
    स्थिर पूर्णाङ्क
pathcmp(वर्ण *p, पूर्णाङ्क plen, वर्ण *q, पूर्णाङ्क qlen)
{
    पूर्णाङ्क		i;

    जइ (plen < 0)
	plen = strlen(p);
    जइ (qlen < 0)
	qlen = strlen(q);
    प्रति (i = 0; ; ++i)
    {
	// End of "p": check if "q" also ends or just has a slash.
	जइ (i == plen)
	{
	    जइ (i == qlen)  // match
		प्रत्यागम 0;
	    जइ (i == qlen - 1 && (q[i] == '\\' || q[i] == '/'))
		प्रत्यागम 0;   // match with trailing slash
	    प्रत्यागम 1;	    // no match
	}

	// End of "q": check if "p" also ends or just has a slash.
	जइ (i == qlen)
	{
	    जइ (i == plen)  // match
		प्रत्यागम 0;
	    जइ (i == plen - 1 && (p[i] == '\\' || p[i] == '/'))
		प्रत्यागम 0;   // match with trailing slash
	    प्रत्यागम 1;	    // no match
	}

	जइ (!(mytoupper(p[i]) == mytoupper(q[i])
		|| ((p[i] == '/' || p[i] == '\\')
		    && (q[i] == '/' || q[i] == '\\'))))
	    प्रत्यागम 1;	    // no match
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
    स्थिर शून्य
findoldfile(वर्ण **destination)
{
    वर्ण	*bp = *destination;
    size_t	indir_l = strlen(installdir);
    वर्ण	*cp;
    वर्ण	*tmpname;
    वर्ण	*farname;

    /*
     * No action needed if exe not found or not in this directory.
     */
    जइ (bp == रिक्त || strnicmp(bp, installdir, indir_l) != 0)
	प्रत्यागम;
    cp = bp + indir_l;
    जइ (strchr("/\\", *cp++) == रिक्त
	    || strchr(cp, '\\') != रिक्त
	    || strchr(cp, '/') != रिक्त)
	प्रत्यागम;

    tmpname = alloc(strlen(cp) + 1);
    strcpy(tmpname, cp);
    tmpname[strlen(tmpname) - 1] = 'x';	// .exe -> .exx

    जइ (access(tmpname, 0) == 0)
    {
	printf("\nERROR: %s and %s clash.  Remove or rename %s.\n",
	    tmpname, cp, tmpname);
	myexit(1);
    }

    जइ (rename(cp, tmpname) != 0)
    {
	printf("\nERROR: failed to rename %s to %s: %s\n",
	    cp, tmpname, strerror(0));
	myexit(1);
    }

    farname = searchpath_save(cp);

    जइ (rename(tmpname, cp) != 0)
    {
	printf("\nERROR: failed to rename %s back to %s: %s\n",
	    tmpname, cp, strerror(0));
	myexit(1);
    }

    मुक्त(*destination);
    मुक्त(tmpname);
    *destination = farname;
}

/*
 * Check if there is a vim.[exe|bat|, gvim.[exe|bat|, etc. in the path.
 * When "check_bat_only" is TRUE, only find "default_bat_dir".
 */
    स्थिर शून्य
find_bat_exe(पूर्णाङ्क check_bat_only)
{
    पूर्णाङ्क		i;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    प्रति (i = 1; i < TARGET_गणनम्; ++i)
    {
	targets[i].oldbat = searchpath_save(targets[i].batname);
	जइ (!check_bat_only)
	    targets[i].oldexe = searchpath_save(targets[i].exename);

	जइ (default_bat_dir == रिक्त && targets[i].oldbat != रिक्त)
	{
	    default_bat_dir = alloc(strlen(targets[i].oldbat) + 1);
	    strcpy(default_bat_dir, targets[i].oldbat);
	    remove_tail(default_bat_dir);
	}
	जइ (check_bat_only && targets[i].oldbat != रिक्त)
	{
	    मुक्त(targets[i].oldbat);
	    targets[i].oldbat = रिक्त;
	}
    }

    mch_chdir(installdir);
}

/*
 * Get the value of $VIMRUNTIME or $VIM and write it in $TEMP/vimini.ini, so
 * that NSIS can read it.
 * When not set, use the directory of a previously installed Vim.
 */
    स्थिर शून्य
get_vim_env(शून्य)
{
    वर्ण	*vim;
    वर्ण	buf[BUFSIZE];
    FILE	*fd;
    वर्ण	fname[BUFSIZE];

    // First get $VIMRUNTIME.  If it's set, remove the tail.
    vim = getenv("VIMRUNTIME");
    जइ (vim != रिक्त && *vim != 0 && strlen(vim) < परिमाण(buf))
    {
	strcpy(buf, vim);
	remove_tail(buf);
	vim = buf;
    }
    अण्णहा
    {
	vim = getenv("VIM");
	जइ (vim == रिक्त || *vim == 0)
	{
	    // Use the directory from an old uninstall entry.
	    जइ (default_vim_dir != रिक्त)
		vim = default_vim_dir;
	    अण्णहा
		// Let NSIS know there is no default, it should use
		// $PROGRAMFILES.
		vim = "";
	}
    }

    // NSIS also uses GetTempPath(), thus we should get the same directory
    // name as where NSIS will look for vimini.ini.
    GetTempPath(परिमाण(fname) - 12, fname);
    add_pathsep(fname);
    strcat(fname, "vimini.ini");

    fd = fopen(fname, "w");
    जइ (fd != रिक्त)
    {
	// Make it look like an .ini file, so that NSIS can read it with a
	// ReadINIStr command.
	fprintf(fd, "[vimini]\n");
	fprintf(fd, "dir=\"%s\"\n", vim);
	fclose(fd);
    }
    अण्णहा
    {
	printf("Failed to open %s\n", fname);
	sleep(2);
    }
}

स्थिर पूर्णाङ्क अङ्कः_windows;

/*
 * Callback used for EnumWindows():
 * Count the window if the title looks like it is for the uninstaller.
 */
//ARGSUSED
    स्थिर BOOL CALLBACK
window_cb(HWND hwnd, LPARAM lparam UNUSED)
{
    वर्ण title[256];

    title[0] = 0;
    GetWindowText(hwnd, title, 256);
    जइ (strstr(title, "Vim ") != रिक्त && strstr(title, " Uninstall") != रिक्त)
	++अङ्कः_windows;
    प्रत्यागम TRUE;
}

/*
 * Run the uninstaller silently.
 */
    स्थिर पूर्णाङ्क
run_silent_uninstall(वर्ण *uninst_exe)
{
    वर्ण    vimrt_dir[BUFSIZE];
    वर्ण    temp_uninst[BUFSIZE];
    वर्ण    temp_dir[MAX_PATH];
    वर्ण    buf[BUFSIZE * 2 + 10];
    पूर्णाङ्क	    i;
    DWORD   tick;

    strcpy(vimrt_dir, uninst_exe);
    remove_tail(vimrt_dir);

    जइ (!GetTempPath(परिमाण(temp_dir), temp_dir))
	प्रत्यागम FAIL;

    // Copy the uninstaller to a temporary exe.
    tick = Get_Tick_गणनम्();
    प्रति (i = 0; ; i++)
    {
	sprintf(temp_uninst, "%s\\vimun%04X.exe", temp_dir,
					  (unsigned पूर्णाङ्क)((i + tick) & 0xFFFF));
	जइ (CopyFile(uninst_exe, temp_uninst, TRUE))
	    भङ्ग;
	जइ (GetLastError() != ERROR_FILE_EXISTS)
	    प्रत्यागम FAIL;
	जइ (i == 65535)
	    प्रत्यागम FAIL;
    }

    // Run the copied uninstaller silently.
    जइ (strchr(temp_uninst, ' ') != रिक्त)
	sprintf(buf, "\"%s\" /S _?=%s", temp_uninst, vimrt_dir);
    अण्णहा
	sprintf(buf, "%s /S _?=%s", temp_uninst, vimrt_dir);
    run_command(buf);

    DeleteFile(temp_uninst);
    प्रत्यागम OK;
}

/*
 * Check for already installed Vims.
 * Return non-zero when found one.
 */
    स्थिर पूर्णाङ्क
uninstall_check(पूर्णाङ्क skip_question)
{
    HKEY	कुञ्चिका_handle;
    HKEY	uninstall_कुञ्चिका_handle;
    वर्ण	*uninstall_कुञ्चिका = "software\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
    वर्ण	subkey_name_buff[BUFSIZE];
    वर्ण	temp_string_buffer[BUFSIZE-2];
    DWORD	local_bufsize;
    FILETIME	temp_pfiletime;
    DWORD	कुञ्चिका_index;
    वर्ण	निवेशः;
    long	code;
    DWORD	मानम्_type;
    DWORD	orig_अङ्कः_कुञ्चिकाः;
    DWORD	new_अङ्कः_कुञ्चिकाः;
    DWORD	allow_silent;
    पूर्णाङ्क		foundone = 0;

    code = Reg_Open_कुञ्चिका_Ex(HKEY_LOCAL_MACHINE, uninstall_कुञ्चिका, 0,
				     कुञ्चिका_WOW64_64_कुञ्चिका | कुञ्चिका_READ, &कुञ्चिका_handle);
    CHECK_REG_ERROR(code);

    कुञ्चिका_index = 0;
    जाव (TRUE)
    {
	local_bufsize = परिमाण(subkey_name_buff);
	जइ (Reg_Enum_कुञ्चिका_Ex(कुञ्चिका_handle, कुञ्चिका_index, subkey_name_buff, &local_bufsize,
		रिक्त, रिक्त, रिक्त, &temp_pfiletime) == ERROR_NO_MORE_वस्तूनि)
	    भङ्ग;

	जइ (strncmp("Vim", subkey_name_buff, 3) == 0)
	{
	    // Open the key named Vim*
	    code = Reg_Open_कुञ्चिका_Ex(कुञ्चिका_handle, subkey_name_buff, 0,
			   कुञ्चिका_WOW64_64_कुञ्चिका | कुञ्चिका_READ, &uninstall_कुञ्चिका_handle);
	    CHECK_REG_ERROR(code);

	    // get the DisplayName out of it to show the user
	    local_bufsize = परिमाण(temp_string_buffer);
	    code = Reg_Query_मानम्_Ex(uninstall_कुञ्चिका_handle, "displayname", 0,
		    &मानम्_type, (LPBYTE)temp_string_buffer,
		    &local_bufsize);
	    CHECK_REG_ERROR(code);

	    allow_silent = 0;
	    जइ (skip_question)
	    {
		DWORD varsize = परिमाण(DWORD);

		Reg_Query_मानम्_Ex(uninstall_कुञ्चिका_handle, "AllowSilent", 0,
			&मानम्_type, (LPBYTE)&allow_silent,
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

	    जइ (skip_question)
		printf("\nRunning uninstall program for \"%s\"\n", temp_string_buffer);
	    अण्णहा
		printf("\nDo you want to uninstall \"%s\" now?\n(y)es/(n)o)  ", temp_string_buffer);
	    fflush(stdout);

	    // get the UninstallString
	    local_bufsize = परिमाण(temp_string_buffer);
	    code = Reg_Query_मानम्_Ex(uninstall_कुञ्चिका_handle, "uninstallstring", 0,
		    &मानम्_type, (LPBYTE)temp_string_buffer, &local_bufsize);
	    CHECK_REG_ERROR(code);

	    // Remember the directory, it is used as the default for NSIS.
	    default_vim_dir = alloc(strlen(temp_string_buffer) + 1);
	    strcpy(default_vim_dir, temp_string_buffer);
	    remove_tail(default_vim_dir);
	    remove_tail(default_vim_dir);

	    निवेशः = 'n';
	    करेहि
	    {
		जइ (निवेशः != 'n')
		    printf("%c is an invalid reply.  Please enter either 'y' or 'n'\n", निवेशः);

		जइ (skip_question)
		    निवेशः = 'y';
		अण्णहा
		{
		    rewind(stdin);
		    scanf("%c", &निवेशः);
		}
		विकल्प (निवेशः)
		{
		    पक्ष 'y':
		    पक्ष 'Y':
			// save the number of uninstall keys so we can know if
			// it changed
			Reg_Query_Info_कुञ्चिका(कुञ्चिका_handle, रिक्त, रिक्त, रिक्त,
					     &orig_अङ्कः_कुञ्चिकाः, रिक्त, रिक्त, रिक्त,
						      रिक्त, रिक्त, रिक्त, रिक्त);

			// Find existing .bat files before deleting them.
			find_bat_exe(TRUE);

			जइ (allow_silent)
			{
			    जइ (run_silent_uninstall(temp_string_buffer)
								    == FAIL)
				allow_silent = 0; // Retry with non silent.
			}
			जइ (!allow_silent)
			{
			    // Execute the uninstall program.  Put it in double
			    // quotes if there is an embedded space.
			    {
				वर्ण buf[BUFSIZE];

				जइ (strchr(temp_string_buffer, ' ') != रिक्त)
				    sprintf(buf, "\"%s\"", temp_string_buffer);
				अण्णहा
				    strcpy(buf, temp_string_buffer);
				run_command(buf);
			    }

			    // Count the number of windows with a title that
			    // match the installer, so that we can check when
			    // it's done.  The uninstaller copies itself,
			    // executes the copy and exits, thus we can't wait
			    // for the process to finish.
			    sleep(1);  // wait for uninstaller to start up
			    अङ्कः_windows = 0;
			    EnumWindows(window_cb, 0);
			    जइ (अङ्कः_windows == 0)
			    {
				// Did not find the uninstaller, ask user to
				// press Enter when done. Just in case.
				printf("Press Enter when the uninstaller is finished\n");
				rewind(stdin);
				(शून्य)getchar();
			    }
			    अण्णहा
			    {
				printf("Waiting for the uninstaller to finish (press CTRL-C to abort).");
				करेहि
				{
				    printf(".");
				    fflush(stdout);
				    sleep(1);	// wait for the uninstaller to
						// finish
				    अङ्कः_windows = 0;
				    EnumWindows(window_cb, 0);
				} जाव (अङ्कः_windows > 0);
			    }
			}
			printf("\nDone!\n");

			// Check if an uninstall reg key was deleted.
			// if it was, we want to decrement key_index.
			// if we don't do this, we will skip the key
			// immediately after any key that we delete.
			Reg_Query_Info_कुञ्चिका(कुञ्चिका_handle, रिक्त, रिक्त, रिक्त,
					      &new_अङ्कः_कुञ्चिकाः, रिक्त, रिक्त, रिक्त,
						      रिक्त, रिक्त, रिक्त, रिक्त);
			जइ (new_अङ्कः_कुञ्चिकाः < orig_अङ्कः_कुञ्चिकाः)
			    कुञ्चिका_index--;

			निवेशः = 'y';
			भङ्ग;

		    पक्ष 'n':
		    पक्ष 'N':
			// Do not uninstall
			निवेशः = 'n';
			भङ्ग;

		    स्वभाव: // just drop through and redo the loop
			भङ्ग;
		}

	    } जाव (निवेशः != 'n' && निवेशः != 'y');

	    Reg_Close_कुञ्चिका(uninstall_कुञ्चिका_handle);
	}

	कुञ्चिका_index++;
    }
    Reg_Close_कुञ्चिका(कुञ्चिका_handle);

    प्रत्यागम foundone;
}

/*
 * Find out information about the system.
 */
    स्थिर शून्य
inspect_system(शून्य)
{
    वर्ण	*p;
    वर्ण	buf[BUFSIZE];
    FILE	*fd;
    पूर्णाङ्क		i;
    पूर्णाङ्क		foundone;

    // This may take a little while, let the user know what we're doing.
    printf("Inspecting system...\n");

    /*
     * If $VIM is set, check that it's pointing to our directory.
     */
    p = getenv("VIM");
    जइ (p != रिक्त && pathcmp(p, -1, installdir, runtimeidx - 1) != 0)
    {
	printf("------------------------------------------------------\n");
	printf("$VIM is set to \"%s\".\n", p);
	printf("This is different from where this version of Vim is:\n");
	strcpy(buf, installdir);
	*(buf + runtimeidx - 1) = NUL;
	printf("\"%s\"\n", buf);
	printf("You must adjust or remove the setting of $VIM,\n");
	जइ (interactive)
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
    जइ (p != रिक्त && pathcmp(p, -1, installdir, -1) != 0)
    {
	printf("------------------------------------------------------\n");
	printf("$VIMRUNTIME is set to \"%s\".\n", p);
	printf("This is different from where this version of Vim is:\n");
	printf("\"%s\"\n", installdir);
	printf("You must adjust or remove the setting of $VIMRUNTIME,\n");
	जइ (interactive)
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
    प्रति (i = 1; i < TARGET_गणनम्; ++i)
    {
	findoldfile(&(targets[i].oldexe));
	जइ (targets[i].oldexe != रिक्त)
	    foundone = 1;
    }

    जइ (foundone)
    {
	printf("Warning: Found Vim executable(s) in your $PATH:\n");
	प्रति (i = 1; i < TARGET_गणनम्; ++i)
	    जइ (targets[i].oldexe != रिक्त)
		printf("%s\n", targets[i].oldexe);
	printf("It will be used instead of the version you are installing.\n");
	printf("Please delete or rename it, or adjust your $PATH setting.\n");
    }

    /*
     * Check if there is an existing ../_vimrc or ../.vimrc file.
     */
    strcpy(oldvimrc, installdir);
    strcpy(oldvimrc + runtimeidx, "_vimrc");
    जइ ((fd = fopen(oldvimrc, "r")) == रिक्त)
    {
	strcpy(oldvimrc + runtimeidx, "vimrc~1"); // short version of .vimrc
	जइ ((fd = fopen(oldvimrc, "r")) == रिक्त)
	{
	    strcpy(oldvimrc + runtimeidx, ".vimrc");
	    fd = fopen(oldvimrc, "r");
	}
    }
    जइ (fd != रिक्त)
	fclose(fd);
    अण्णहा
	*oldvimrc = NUL;
}

/*
 * Add a dummy choice to avoid that the numbering changes depending on items
 * in the environment.  The user may type a number he remembered without
 * looking.
 */
    स्थिर शून्य
add_dummy_choice(शून्य)
{
    choices[choice_गणनम्].installfunc = रिक्त;
    choices[choice_गणनम्].active = 0;
    choices[choice_गणनम्].changefunc = रिक्त;
    choices[choice_गणनम्].text = रिक्त;
    choices[choice_गणनम्].arg = 0;
    ++choice_गणनम्;
}

////////////////////////////////////////////////
// stuff for creating the batch files.

/*
 * Install the vim.bat, gvim.bat, etc. files.
 */
    स्थिर शून्य
install_bat_choice(पूर्णाङ्क idx)
{
    वर्ण	*batpath = targets[choices[idx].arg].batpath;
    वर्ण	*oldname = targets[choices[idx].arg].oldbat;
    वर्ण	*exename = targets[choices[idx].arg].exenamearg;
    वर्ण	*vimarg = targets[choices[idx].arg].exearg;
    FILE	*fd;

    जइ (*batpath == NUL)
	प्रत्यागम;

    fd = fopen(batpath, "w");
    जइ (fd == रिक्त)
    {
	printf("\nERROR: Cannot open \"%s\" for writing.\n", batpath);
	प्रत्यागम;
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

    जइ (*exename == 'g')
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

    जइ (*exename == 'g')
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
    अण्णहा
    {
	// Always use quotes, $VIM or $VIMRUNTIME might have a space.
	fprintf(fd, "\"%%VIM_EXE_DIR%%\\%s\" %s %%*\n",
		exename, vimarg);
    }

    fclose(fd);
    printf("%s has been %s\n", batpath,
	    oldname == रिक्त ? "created" : "overwritten");
}

/*
 * Make the text string for choice "idx".
 * The format "fmt" is must have one %s item, which "arg" is used for.
 */
    स्थिर शून्य
alloc_text(पूर्णाङ्क idx, वर्ण *fmt, वर्ण *arg)
{
    जइ (choices[idx].text != रिक्त)
	मुक्त(choices[idx].text);

    choices[idx].text = alloc(strlen(fmt) + strlen(arg) - 1);
    sprintf(choices[idx].text, fmt, arg);
}

/*
 * Toggle the "Overwrite .../vim.bat" to "Don't overwrite".
 */
    स्थिर शून्य
toggle_bat_choice(पूर्णाङ्क idx)
{
    वर्ण	*batname = targets[choices[idx].arg].batpath;
    वर्ण	*oldname = targets[choices[idx].arg].oldbat;

    जइ (*batname == NUL)
    {
	alloc_text(idx, "    Overwrite %s", oldname);
	strcpy(batname, oldname);
    }
    अण्णहा
    {
	alloc_text(idx, "    Do NOT overwrite %s", oldname);
	*batname = NUL;
    }
}

/*
 * Do some work for a batch file entry: Append the batch file name to the path
 * and set the text for the choice.
 */
    स्थिर शून्य
set_bat_text(पूर्णाङ्क idx, वर्ण *batpath, वर्ण *name)
{
    strcat(batpath, name);

    alloc_text(idx, "    Create %s", batpath);
}

/*
 * Select a directory to write the batch file line.
 */
    स्थिर शून्य
change_bat_choice(पूर्णाङ्क idx)
{
    वर्ण	*path;
    वर्ण	*batpath;
    वर्ण	*name;
    पूर्णाङ्क		n;
    वर्ण	*s;
    वर्ण	*p;
    पूर्णाङ्क		गणनम्;
    वर्ण	**names = रिक्त;
    पूर्णाङ्क		i;
    पूर्णाङ्क		target = choices[idx].arg;

    name = targets[target].batname;
    batpath = targets[target].batpath;

    path = getenv("PATH");
    जइ (path == रिक्त)
    {
	printf("\nERROR: The variable $PATH is not set\n");
	प्रत्यागम;
    }

    /*
     * first round: count number of names in path;
     * second round: save names to names[].
     */
    प्रति (;;)
    {
	गणनम् = 1;
	प्रति (p = path; *p; )
	{
	    s = strchr(p, ';');
	    जइ (s == रिक्त)
		s = p + strlen(p);
	    जइ (names != रिक्त)
	    {
		names[गणनम्] = alloc(s - p + 1);
		strncpy(names[गणनम्], p, s - p);
		names[गणनम्][s - p] = NUL;
	    }
	    ++गणनम्;
	    p = s;
	    जइ (*p != NUL)
		++p;
	}
	जइ (names != रिक्त)
	    भङ्ग;
	names = alloc((गणनम् + 1) * परिमाण(वर्ण *));
    }
    names[0] = alloc(50);
    sprintf(names[0], "Select directory to create %s in:", name);
    names[गणनम्] = alloc(50);
    जइ (choices[idx].arg == 0)
	sprintf(names[गणनम्], "Do not create any .bat file.");
    अण्णहा
	sprintf(names[गणनम्], "Do not create a %s file.", name);
    n = get_choice(names, गणनम् + 1);

    जइ (n == गणनम्)
    {
	// Selected last item, don't create bat file.
	*batpath = NUL;
	जइ (choices[idx].arg != 0)
	    alloc_text(idx, "    Do NOT create %s", name);
    }
    अण्णहा
    {
	// Selected one of the paths.  For the first item only keep the path,
	// for the others append the batch file name.
	strcpy(batpath, names[n]);
	add_pathsep(batpath);
	जइ (choices[idx].arg != 0)
	    set_bat_text(idx, batpath, name);
    }

    प्रति (i = 0; i <= गणनम्; ++i)
	मुक्त(names[i]);
    मुक्त(names);
}

वर्ण *bat_text_yes = "Install .bat files to use Vim at the command line:";
वर्ण *bat_text_no = "do NOT install .bat files to use Vim at the command line";

    स्थिर शून्य
change_main_bat_choice(पूर्णाङ्क idx)
{
    पूर्णाङ्क		i;

    // let the user select a default directory or NONE
    change_bat_choice(idx);

    जइ (targets[0].batpath[0] != NUL)
	choices[idx].text = bat_text_yes;
    अण्णहा
	choices[idx].text = bat_text_no;

    // update the individual batch file selections
    प्रति (i = 1; i < TARGET_गणनम्; ++i)
    {
	// Only make it active when the first item has a path and the vim.exe
	// or gvim.exe exists (there is a changefunc then).
	जइ (targets[0].batpath[0] != NUL
		&& choices[idx + i].changefunc != रिक्त)
	{
	    choices[idx + i].active = 1;
	    जइ (choices[idx + i].changefunc == change_bat_choice
		    && targets[i].batpath[0] != NUL)
	    {
		strcpy(targets[i].batpath, targets[0].batpath);
		set_bat_text(idx + i, targets[i].batpath, targets[i].batname);
	    }
	}
	अण्णहा
	    choices[idx + i].active = 0;
    }
}

/*
 * Initialize a choice for creating a batch file.
 */
    स्थिर शून्य
init_bat_choice(पूर्णाङ्क target)
{
    वर्ण	*batpath = targets[target].batpath;
    वर्ण	*oldbat = targets[target].oldbat;
    वर्ण	*p;
    पूर्णाङ्क		i;

    choices[choice_गणनम्].arg = target;
    choices[choice_गणनम्].installfunc = install_bat_choice;
    choices[choice_गणनम्].active = 1;
    choices[choice_गणनम्].text = रिक्त;	// will be set below
    जइ (oldbat != रिक्त)
    {
	// A [g]vim.bat exists: Only choice is to overwrite it or not.
	choices[choice_गणनम्].changefunc = toggle_bat_choice;
	*batpath = NUL;
	toggle_bat_choice(choice_गणनम्);
    }
    अण्णहा
    {
	जइ (default_bat_dir != रिक्त)
	    // Prefer using the same path as an existing .bat file.
	    strcpy(batpath, default_bat_dir);
	अण्णहा
	{
	    // No [g]vim.bat exists: Write it to a directory in $PATH.  Use
	    // $WINDIR by default, if it's empty the first item in $PATH.
	    p = getenv("WINDIR");
	    जइ (p != रिक्त && *p != NUL)
		strcpy(batpath, p);
	    अण्णहा
	    {
		p = getenv("PATH");
		जइ (p == रिक्त || *p == NUL)		// "cannot happen"
		    strcpy(batpath, "C:/Windows");
		अण्णहा
		{
		    i = 0;
		    जाव (*p != NUL && *p != ';')
			batpath[i++] = *p++;
		    batpath[i] = NUL;
		}
	    }
	}
	add_pathsep(batpath);
	set_bat_text(choice_गणनम्, batpath, targets[target].batname);

	choices[choice_गणनम्].changefunc = change_bat_choice;
    }
    ++choice_गणनम्;
}

/*
 * Set up the choices for installing .bat files.
 * For these items "arg" is the index in targets[].
 */
    स्थिर शून्य
init_bat_choices(शून्य)
{
    पूर्णाङ्क		i;

    // The first item is used to switch installing batch files on/off and
    // setting the default path.
    choices[choice_गणनम्].text = bat_text_yes;
    choices[choice_गणनम्].changefunc = change_main_bat_choice;
    choices[choice_गणनम्].installfunc = रिक्त;
    choices[choice_गणनम्].active = 1;
    choices[choice_गणनम्].arg = 0;
    ++choice_गणनम्;

    // Add items for each batch file target.  Only used when not disabled by
    // the first item.  When a .exe exists, don't offer to create a .bat.
    प्रति (i = 1; i < TARGET_गणनम्; ++i)
	जइ (targets[i].oldexe == रिक्त
		&& (targets[i].exenamearg[0] == 'g' ? has_gvim : has_vim))
	    init_bat_choice(i);
	अण्णहा
	    add_dummy_choice();
}

/*
 * Install the vimrc file.
 */
    स्थिर शून्य
install_vimrc(पूर्णाङ्क idx UNUSED)
{
    FILE	*fd, *tfd;
    वर्ण	*fname;

    // If an old vimrc file exists, overwrite it.
    // Otherwise create a new one.
    जइ (*oldvimrc != NUL)
	fname = oldvimrc;
    अण्णहा
	fname = vimrc;

    fd = fopen(fname, "w");
    जइ (fd == रिक्त)
    {
	printf("\nERROR: Cannot open \"%s\" for writing.\n", fname);
	प्रत्यागम;
    }
    विकल्प (compat_choice)
    {
	पक्ष compat_vi:
		    fprintf(fd, "\" Vi compatible\n");
		    fprintf(fd, "set compatible\n");
		    भङ्ग;
	पक्ष compat_vim:
		    fprintf(fd, "\" Vim's default behavior\n");
		    fprintf(fd, "if &compatible\n");
		    fprintf(fd, "  set nocompatible\n");
		    fprintf(fd, "endif\n");
		    भङ्ग;
	पक्ष compat_some_enhancements:
		    fprintf(fd, "\" Vim with some enhancements\n");
		    fprintf(fd, "source $VIMRUNTIME/defaults.vim\n");
		    भङ्ग;
	पक्ष compat_all_enhancements:
		    fprintf(fd, "\" Vim with all enhancements\n");
		    fprintf(fd, "source $VIMRUNTIME/vimrc_example.vim\n");
		    भङ्ग;
    }
    विकल्प (remap_choice)
    {
	पक्ष remap_no:
		    भङ्ग;
	पक्ष remap_win:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Remap a few keys for Windows behavior\n");
		    fprintf(fd, "source $VIMRUNTIME/mswin.vim\n");
		    भङ्ग;
    }
    विकल्प (mouse_choice)
    {
	पक्ष mouse_xterm:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Mouse behavior (the Unix way)\n");
		    fprintf(fd, "behave xterm\n");
		    भङ्ग;
	पक्ष mouse_mswin:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Mouse behavior (the Windows way)\n");
		    fprintf(fd, "behave mswin\n");
		    भङ्ग;
	पक्ष mouse_default:
		    भङ्ग;
    }
    जइ ((tfd = fopen("diff.exe", "r")) != रिक्त)
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

    स्थिर शून्य
change_vimrc_choice(पूर्णाङ्क idx)
{
    जइ (choices[idx].installfunc != रिक्त)
    {
	// Switch to NOT change or create a vimrc file.
	जइ (*oldvimrc != NUL)
	    alloc_text(idx, "Do NOT change startup file %s", oldvimrc);
	अण्णहा
	    alloc_text(idx, "Do NOT create startup file %s", vimrc);
	choices[idx].installfunc = रिक्त;
	choices[idx + 1].active = 0;
	choices[idx + 2].active = 0;
	choices[idx + 3].active = 0;
    }
    अण्णहा
    {
	// Switch to change or create a vimrc file.
	जइ (*oldvimrc != NUL)
	    alloc_text(idx, "Overwrite startup file %s with:", oldvimrc);
	अण्णहा
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
    स्थिर शून्य
change_run_choice(पूर्णाङ्क idx)
{
    compat_choice = get_choice(compat_choices, TABLE_SIZE(compat_choices));
    alloc_text(idx, compat_text, compat_choices[compat_choice]);
}

/*
 * Change the choice if keys are to be remapped.
 */
    स्थिर शून्य
change_remap_choice(पूर्णाङ्क idx)
{
    remap_choice = get_choice(remap_choices, TABLE_SIZE(remap_choices));
    alloc_text(idx, remap_text, remap_choices[remap_choice]);
}

/*
 * Change the choice how to select text.
 */
    स्थिर शून्य
change_mouse_choice(पूर्णाङ्क idx)
{
    mouse_choice = get_choice(mouse_choices, TABLE_SIZE(mouse_choices));
    alloc_text(idx, mouse_text, mouse_choices[mouse_choice]);
}

    स्थिर शून्य
init_vimrc_choices(शून्य)
{
    // set path for a new _vimrc file (also when not used)
    strcpy(vimrc, installdir);
    strcpy(vimrc + runtimeidx, "_vimrc");

    // Set opposite value and then toggle it by calling change_vimrc_choice()
    जइ (*oldvimrc == NUL)
	choices[choice_गणनम्].installfunc = रिक्त;
    अण्णहा
	choices[choice_गणनम्].installfunc = install_vimrc;
    choices[choice_गणनम्].text = रिक्त;
    change_vimrc_choice(choice_गणनम्);
    choices[choice_गणनम्].changefunc = change_vimrc_choice;
    choices[choice_गणनम्].active = 1;
    ++choice_गणनम्;

    // default way to run Vim
    alloc_text(choice_गणनम्, compat_text, compat_choices[compat_choice]);
    choices[choice_गणनम्].changefunc = change_run_choice;
    choices[choice_गणनम्].installfunc = रिक्त;
    choices[choice_गणनम्].active = (*oldvimrc == NUL);
    ++choice_गणनम्;

    // Whether to remap keys
    alloc_text(choice_गणनम्, remap_text , remap_choices[remap_choice]);
    choices[choice_गणनम्].changefunc = change_remap_choice;
    choices[choice_गणनम्].installfunc = रिक्त;
    choices[choice_गणनम्].active = (*oldvimrc == NUL);
    ++choice_गणनम्;

    // default way to use the mouse
    alloc_text(choice_गणनम्, mouse_text, mouse_choices[mouse_choice]);
    choices[choice_गणनम्].changefunc = change_mouse_choice;
    choices[choice_गणनम्].installfunc = रिक्त;
    choices[choice_गणनम्].active = (*oldvimrc == NUL);
    ++choice_गणनम्;
}

    स्थिर LONG
reg_create_कुञ्चिका(
    HKEY मूलम्,
    ध्रुव वर्ण *subkey,
    PHKEY ph_कुञ्चिका,
    DWORD flag)
{
    DWORD disp;

    *ph_कुञ्चिका = रिक्त;
    प्रत्यागम Reg_Create_कुञ्चिका_Ex(
		मूलम्, subkey,
		0, रिक्त, REG_OPTION_NON_VOLATILE,
		flag | कुञ्चिका_WRITE,
		रिक्त, ph_कुञ्चिका, &disp);
}

    स्थिर LONG
reg_set_string_मानम्(
    HKEY h_कुञ्चिका,
    ध्रुव वर्ण *मानम्_name,
    ध्रुव वर्ण *data)
{
    प्रत्यागम Reg_Set_मानम्_Ex(h_कुञ्चिका, मानम्_name, 0, REG_SZ,
				     (LPBYTE)data, (DWORD)(1 + strlen(data)));
}

    स्थिर LONG
reg_create_कुञ्चिका_and_मानम्(
    HKEY h_मूलम्_कुञ्चिका,
    ध्रुव वर्ण *subkey,
    ध्रुव वर्ण *मानम्_name,
    ध्रुव वर्ण *data,
    DWORD flag)
{
    HKEY h_कुञ्चिका;
    LONG lRet = reg_create_कुञ्चिका(h_मूलम्_कुञ्चिका, subkey, &h_कुञ्चिका, flag);

    जइ (ERROR_SUCCESS == lRet)
    {
	lRet = reg_set_string_मानम्(h_कुञ्चिका, मानम्_name, data);
	Reg_Close_कुञ्चिका(h_कुञ्चिका);
    }
    प्रत्यागम lRet;
}

    स्थिर LONG
register_inproc_server(
    HKEY h_मूलम्_कुञ्चिका,
    ध्रुव वर्ण *clsid,
    ध्रुव वर्ण *extname,
    ध्रुव वर्ण *module,
    ध्रुव वर्ण *threading_model,
    DWORD flag)
{
    CHAR subkey[BUFSIZE];
    LONG lRet;

    sprintf(subkey, "CLSID\\%s", clsid);
    lRet = reg_create_कुञ्चिका_and_मानम्(h_मूलम्_कुञ्चिका, subkey, रिक्त, extname, flag);
    जइ (ERROR_SUCCESS == lRet)
    {
	sprintf(subkey, "CLSID\\%s\\InProcServer32", clsid);
	lRet = reg_create_कुञ्चिका_and_मानम्(h_मूलम्_कुञ्चिका, subkey, रिक्त, module, flag);
	जइ (ERROR_SUCCESS == lRet)
	{
	    lRet = reg_create_कुञ्चिका_and_मानम्(h_मूलम्_कुञ्चिका, subkey,
					   "ThreadingModel", threading_model, flag);
	}
    }
    प्रत्यागम lRet;
}

    स्थिर LONG
register_shellex(
    HKEY h_मूलम्_कुञ्चिका,
    ध्रुव वर्ण *clsid,
    ध्रुव वर्ण *name,
    ध्रुव वर्ण *exe_path,
    DWORD flag)
{
    LONG lRet = reg_create_कुञ्चिका_and_मानम्(
	    h_मूलम्_कुञ्चिका,
	    "*\\shellex\\ContextMenuHandlers\\gvim",
	    रिक्त,
	    clsid,
	    flag);

    जइ (ERROR_SUCCESS == lRet)
    {
	lRet = reg_create_कुञ्चिका_and_मानम्(
		HKEY_LOCAL_MACHINE,
		"Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved",
		clsid,
		name,
		flag);

	जइ (ERROR_SUCCESS == lRet)
	{
	    lRet = reg_create_कुञ्चिका_and_मानम्(
		    HKEY_LOCAL_MACHINE,
		    "Software\\Vim\\Gvim",
		    "path",
		    exe_path,
		    flag);
	}
    }
    प्रत्यागम lRet;
}

    स्थिर LONG
register_openwith(
    HKEY h_मूलम्_कुञ्चिका,
    ध्रुव वर्ण *exe_path,
    DWORD flag)
{
    वर्ण	exe_cmd[BUFSIZE];
    LONG	lRet;

    sprintf(exe_cmd, "\"%s\" \"%%1\"", exe_path);
    lRet = reg_create_कुञ्चिका_and_मानम्(
	    h_मूलम्_कुञ्चिका,
	    "Applications\\gvim.exe\\shell\\edit\\command",
	    रिक्त,
	    exe_cmd,
	    flag);

    जइ (ERROR_SUCCESS == lRet)
    {
	पूर्णाङ्क i;
	स्थिर ध्रुव वर्ण *openwith[] = {
		".htm\\OpenWithList\\gvim.exe",
		".vim\\OpenWithList\\gvim.exe",
		"*\\OpenWithList\\gvim.exe",
	};

	प्रति (i = 0; ERROR_SUCCESS == lRet && i < (पूर्णाङ्क)ARRAYSIZE(openwith); i++)
	    lRet = reg_create_कुञ्चिका_and_मानम्(h_मूलम्_कुञ्चिका, openwith[i], रिक्त, "", flag);
    }

    प्रत्यागम lRet;
}

    स्थिर LONG
register_uninstall(
    HKEY h_मूलम्_कुञ्चिका,
    ध्रुव वर्ण *appname,
    ध्रुव वर्ण *display_name,
    ध्रुव वर्ण *uninstall_string,
    ध्रुव वर्ण *display_icon,
    ध्रुव वर्ण *display_version,
    ध्रुव वर्ण *publisher)
{
    LONG lRet = reg_create_कुञ्चिका_and_मानम्(h_मूलम्_कुञ्चिका, appname,
			     "DisplayName", display_name, कुञ्चिका_WOW64_64_कुञ्चिका);

    जइ (ERROR_SUCCESS == lRet)
	lRet = reg_create_कुञ्चिका_and_मानम्(h_मूलम्_कुञ्चिका, appname,
		     "UninstallString", uninstall_string, कुञ्चिका_WOW64_64_कुञ्चिका);
    जइ (ERROR_SUCCESS == lRet)
	lRet = reg_create_कुञ्चिका_and_मानम्(h_मूलम्_कुञ्चिका, appname,
		     "DisplayIcon", display_icon, कुञ्चिका_WOW64_64_कुञ्चिका);
    जइ (ERROR_SUCCESS == lRet)
	lRet = reg_create_कुञ्चिका_and_मानम्(h_मूलम्_कुञ्चिका, appname,
		     "DisplayVersion", display_version, कुञ्चिका_WOW64_64_कुञ्चिका);
    जइ (ERROR_SUCCESS == lRet)
	lRet = reg_create_कुञ्चिका_and_मानम्(h_मूलम्_कुञ्चिका, appname,
		     "Publisher", publisher, कुञ्चिका_WOW64_64_कुञ्चिका);
    प्रत्यागम lRet;
}

/*
 * Add some entries to the registry:
 * - to add "Edit with Vim" to the context * menu
 * - to add Vim to the "Open with..." list
 * - to uninstall Vim
 */
//ARGSUSED
    स्थिर पूर्णाङ्क
install_registry(शून्य)
{
    LONG	lRet = ERROR_SUCCESS;
    ध्रुव वर्ण	*vim_ext_ThreadingModel = "Apartment";
    ध्रुव वर्ण	*vim_ext_name = "Vim Shell Extension";
    ध्रुव वर्ण	*vim_ext_clsid = "{51EEE242-AD87-11d3-9C1E-0090278BBD99}";
    वर्ण	vim_exe_path[MAX_PATH];
    वर्ण	display_name[BUFSIZE];
    वर्ण	uninstall_string[BUFSIZE];
    वर्ण	icon_string[BUFSIZE];
    वर्ण	version_string[BUFSIZE];
    पूर्णाङ्क		i;
    पूर्णाङ्क		loop_गणनम् = is_64bit_os() ? 2 : 1;
    DWORD	flag;

    sprintf(vim_exe_path, "%s\\gvim.exe", installdir);

    जइ (install_popup)
    {
	वर्ण	    bufg[BUFSIZE];

	printf("Creating \"Edit with Vim\" popup menu entry\n");

	प्रति (i = 0; i < loop_गणनम्; i++)
	{
	    जइ (i == 0)
	    {
		sprintf(bufg, "%s\\" GVIMEXT32_PATH, installdir);
		flag = कुञ्चिका_WOW64_32_कुञ्चिका;
	    }
	    अण्णहा
	    {
		sprintf(bufg, "%s\\" GVIMEXT64_PATH, installdir);
		flag = कुञ्चिका_WOW64_64_कुञ्चिका;
	    }

	    lRet = register_inproc_server(
		    HKEY_CLASSES_मूलम्, vim_ext_clsid, vim_ext_name,
		    bufg, vim_ext_ThreadingModel, flag);
	    जइ (ERROR_SUCCESS != lRet)
		प्रत्यागम FAIL;
	    lRet = register_shellex(
		    HKEY_CLASSES_मूलम्, vim_ext_clsid, vim_ext_name,
		    vim_exe_path, flag);
	    जइ (ERROR_SUCCESS != lRet)
		प्रत्यागम FAIL;
	}
    }

    जइ (install_openwith)
    {
	printf("Creating \"Open with ...\" list entry\n");

	प्रति (i = 0; i < loop_गणनम्; i++)
	{
	    जइ (i == 0)
		flag = कुञ्चिका_WOW64_32_कुञ्चिका;
	    अण्णहा
		flag = कुञ्चिका_WOW64_64_कुञ्चिका;

	    lRet = register_openwith(HKEY_CLASSES_मूलम्, vim_exe_path, flag);
	    जइ (ERROR_SUCCESS != lRet)
		प्रत्यागम FAIL;
	}
    }

    printf("Creating an uninstall entry\n");
    sprintf(display_name, "Vim " VIM_VERSION_SHORT
#ifdef _M_ARM64
	    " (arm64)"
#नोचेद्यदि _M_X64
	    " (x64)"
#endif
	    );

    // For the NSIS installer use the generated uninstaller.
    जइ (interactive)
	sprintf(uninstall_string, "%s\\uninstall.exe", installdir);
    अण्णहा
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
    जइ (ERROR_SUCCESS != lRet)
	प्रत्यागम FAIL;

    प्रत्यागम OK;
}

    स्थिर शून्य
change_popup_choice(पूर्णाङ्क idx)
{
    जइ (install_popup == 0)
    {
	choices[idx].text = "Install an entry for Vim in the popup menu for the right\n    mouse button so that you can edit any file with Vim";
	install_popup = 1;
    }
    अण्णहा
    {
	choices[idx].text = "Do NOT install an entry for Vim in the popup menu for the\n    right mouse button to edit any file with Vim";
	install_popup = 0;
    }
}

/*
 * Only add the choice for the popup menu entry when gvim.exe was found and
 * both gvimext.dll and regedit.exe exist.
 */
    स्थिर शून्य
init_popup_choice(शून्य)
{
    संरचना stat	st;

    जइ (has_gvim
	    && (stat(GVIMEXT32_PATH, &st) >= 0
		|| stat(GVIMEXT64_PATH, &st) >= 0))
    {
	choices[choice_गणनम्].changefunc = change_popup_choice;
	choices[choice_गणनम्].installfunc = रिक्त;
	choices[choice_गणनम्].active = 1;
	change_popup_choice(choice_गणनम्);  // set the text
	++choice_गणनम्;
    }
    अण्णहा
	add_dummy_choice();
}

    स्थिर शून्य
change_openwith_choice(पूर्णाङ्क idx)
{
    जइ (install_openwith == 0)
    {
	choices[idx].text = "Add Vim to the \"Open With...\" list in the popup menu for the right\n    mouse button so that you can edit any file with Vim";
	install_openwith = 1;
    }
    अण्णहा
    {
	choices[idx].text = "Do NOT add Vim to the \"Open With...\" list in the popup menu for the\n    right mouse button to edit any file with Vim";
	install_openwith = 0;
    }
}

/*
 * Only add the choice for the open-with menu entry when gvim.exe was found
 * and regedit.exe exist.
 */
    स्थिर शून्य
init_openwith_choice(शून्य)
{
    जइ (has_gvim)
    {
	choices[choice_गणनम्].changefunc = change_openwith_choice;
	choices[choice_गणनम्].installfunc = रिक्त;
	choices[choice_गणनम्].active = 1;
	change_openwith_choice(choice_गणनम्);  // set the text
	++choice_गणनम्;
    }
    अण्णहा
	add_dummy_choice();
}

/*
 * Create a shell link.
 *
 * returns 0 on failure, non-zero on successful completion.
 *
 * NOTE:  Currently untested with mingw.
 */
    पूर्णाङ्क
create_shortcut(
	ध्रुव वर्ण *shortcut_name,
	ध्रुव वर्ण *iconfile_path,
	पूर्णाङ्क	    iconindex,
	ध्रुव वर्ण *shortcut_target,
	ध्रुव वर्ण *shortcut_args,
	ध्रुव वर्ण *workingdir
	)
{
    IShellLink	    *shelllink_ptr;
    HRESULT	    hres;
    IPersistFile	    *persistfile_ptr;

    // Initialize COM library
    hres = CoInitialize(रिक्त);
    जइ (!SUCCEEDED(hres))
    {
	printf("Error:  Could not open the COM library.  Not creating shortcut.\n");
	प्रत्यागम FAIL;
    }

    // Instantiate a COM object for the ShellLink, store a pointer to it
    // in shelllink_ptr.
    hres = CoCreateInstance(&CLSID_ShellLink,
			   रिक्त,
			   CLSCTX_INPROC_SERVER,
			   &IID_IShellLink,
			   (शून्य **) &shelllink_ptr);

    जइ (SUCCEEDED(hres)) // If the instantiation was successful...
    {
	// ...Then build a PersistFile interface for the ShellLink so we can
	// save it as a file after we build it.
	hres = shelllink_ptr->lpVtbl->QueryInterface(shelllink_ptr,
		&IID_IPersistFile, (शून्य **) &persistfile_ptr);

	जइ (SUCCEEDED(hres))
	{
	    wchar_t wsz[BUFSIZE];

	    // translate the (possibly) multibyte shortcut filename to windows
	    // Unicode so it can be used as a file name.
	    MultiByteToWideChar(CP_ACP, 0, shortcut_name, -1, wsz, परिमाण(wsz)/परिमाण(wsz[0]));

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
	अण्णहा
	{
	    printf("QueryInterface Error\n");
	    प्रत्यागम FAIL;
	}

	// Return the ShellLink object
	shelllink_ptr->lpVtbl->Release(shelllink_ptr);
    }
    अण्णहा
    {
	printf("CoCreateInstance Error - hres = %08x\n", (पूर्णाङ्क)hres);
	प्रत्यागम FAIL;
    }

    प्रत्यागम OK;
}

/*
 * Build a path to where we will put a specified link.
 *
 * Return 0 on error, non-zero on success
 */
   पूर्णाङ्क
build_link_name(
	वर्ण	   *link_path,
	ध्रुव वर्ण *link_name,
	ध्रुव वर्ण *shell_folder_name)
{
    वर्ण	shell_folder_path[MAX_PATH];

    जइ (get_shell_folder_path(shell_folder_path, shell_folder_name) == FAIL)
    {
	printf("An error occurred while attempting to find the path to %s.\n",
							   shell_folder_name);
	प्रत्यागम FAIL;
    }

    // Make sure the directory exists (create Start Menu\Programs\Vim).
    // Ignore errors if it already exists.
    vim_mkdir(shell_folder_path, 0755);

    // build the path to the shortcut and the path to gvim.exe
    sprintf(link_path, "%s\\%s.lnk", shell_folder_path, link_name);

    प्रत्यागम OK;
}

    स्थिर पूर्णाङ्क
build_shortcut(
	ध्रुव वर्ण *name,	// Name of the shortcut
	ध्रुव वर्ण *exename,	// Name of the executable (e.g., vim.exe)
	ध्रुव वर्ण *args,
	ध्रुव वर्ण *shell_folder,
	ध्रुव वर्ण *workingdir)
{
    वर्ण	executable_path[BUFSIZE];
    वर्ण	link_name[BUFSIZE];

    sprintf(executable_path, "%s\\%s", installdir, exename);

    जइ (build_link_name(link_name, name, shell_folder) == FAIL)
    {
	printf("An error has occurred.  A shortcut to %s will not be created %s.\n",
		name,
		*shell_folder == 'd' ? "on the desktop" : "in the Start menu");
	प्रत्यागम FAIL;
    }

    // Create the shortcut:
    प्रत्यागम create_shortcut(link_name, executable_path, 0,
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
    स्थिर शून्य
install_start_menu(पूर्णाङ्क idx UNUSED)
{
    need_uninstall_entry = 1;
    printf("Creating start menu\n");
    जइ (has_vim)
    {
	जइ (build_shortcut("Vim", "vim.exe", "",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    प्रत्यागम;
	जइ (build_shortcut("Vim Read-only", "vim.exe", "-R",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    प्रत्यागम;
	जइ (build_shortcut("Vim Diff", "vim.exe", "-d",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    प्रत्यागम;
    }
    जइ (has_gvim)
    {
	जइ (build_shortcut("gVim", "gvim.exe", "",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    प्रत्यागम;
	जइ (build_shortcut("gVim Easy", "gvim.exe", "-y",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    प्रत्यागम;
	जइ (build_shortcut("gVim Read-only", "gvim.exe", "-R",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    प्रत्यागम;
	जइ (build_shortcut("gVim Diff", "gvim.exe", "-d",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    प्रत्यागम;
    }
    जइ (build_shortcut("Uninstall",
		interactive ? "uninstall.exe" : "uninstall-gui.exe", "",
					   VIM_STARTMENU, installdir) == FAIL)
	प्रत्यागम;
    // For Windows NT the working dir of the vimtutor.bat must be right,
    // otherwise gvim.exe won't be found and using gvimbat doesn't work.
    जइ (build_shortcut("Vim tutor", "vimtutor.bat", "",
					   VIM_STARTMENU, installdir) == FAIL)
	प्रत्यागम;
    जइ (build_shortcut("Help", has_gvim ? "gvim.exe" : "vim.exe", "-c h",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	प्रत्यागम;
    {
	वर्ण	shell_folder_path[BUFSIZE];

	// Creating the URL shortcut works a bit differently...
	जइ (get_shell_folder_path(shell_folder_path, VIM_STARTMENU) == FAIL)
	{
	    printf("Finding the path of the Start menu failed\n");
	    प्रत्यागम ;
	}
	add_pathsep(shell_folder_path);
	strcat(shell_folder_path, "Vim Online.url");
	जइ (!WritePrivateProfileString("InternetShortcut", "URL",
				    "https://www.vim.org/", shell_folder_path))
	{
	    printf("Creating the Vim online URL failed\n");
	    प्रत्यागम;
	}
    }
}

    स्थिर शून्य
toggle_startmenu_choice(पूर्णाङ्क idx)
{
    जइ (choices[idx].installfunc == रिक्त)
    {
	choices[idx].installfunc = install_start_menu;
	choices[idx].text = "Add Vim to the Start menu";
    }
    अण्णहा
    {
	choices[idx].installfunc = रिक्त;
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
    शून्य
install_shortcut_gvim(पूर्णाङ्क idx)
{
    // Create shortcut(s) on the desktop
    जइ (choices[idx].arg)
    {
	(शून्य)build_shortcut(icon_names[0], "gvim.exe",
						      "", "desktop", WORKDIR);
	need_uninstall_entry = 1;
    }
}

    शून्य
install_shortcut_evim(पूर्णाङ्क idx)
{
    जइ (choices[idx].arg)
    {
	(शून्य)build_shortcut(icon_names[1], "gvim.exe",
						    "-y", "desktop", WORKDIR);
	need_uninstall_entry = 1;
    }
}

    शून्य
install_shortcut_gview(पूर्णाङ्क idx)
{
    जइ (choices[idx].arg)
    {
	(शून्य)build_shortcut(icon_names[2], "gvim.exe",
						    "-R", "desktop", WORKDIR);
	need_uninstall_entry = 1;
    }
}

    शून्य
toggle_shortcut_choice(पूर्णाङ्क idx)
{
    वर्ण	*arg;

    जइ (choices[idx].installfunc == install_shortcut_gvim)
	arg = "gVim";
    अण्णहा जइ (choices[idx].installfunc == install_shortcut_evim)
	arg = "gVim Easy";
    अण्णहा
	arg = "gVim Read-only";
    जइ (choices[idx].arg)
    {
	choices[idx].arg = 0;
	alloc_text(idx, "Do NOT create a desktop icon for %s", arg);
    }
    अण्णहा
    {
	choices[idx].arg = 1;
	alloc_text(idx, "Create a desktop icon for %s", arg);
    }
}

    स्थिर शून्य
init_startmenu_choice(शून्य)
{
    // Start menu
    choices[choice_गणनम्].changefunc = toggle_startmenu_choice;
    choices[choice_गणनम्].installfunc = रिक्त;
    choices[choice_गणनम्].active = 1;
    toggle_startmenu_choice(choice_गणनम्);	// set the text
    ++choice_गणनम्;
}

/*
 * Add the choice for the desktop shortcuts.
 */
    स्थिर शून्य
init_shortcut_choices(शून्य)
{
    // Shortcut to gvim
    choices[choice_गणनम्].text = रिक्त;
    choices[choice_गणनम्].arg = 0;
    choices[choice_गणनम्].active = has_gvim;
    choices[choice_गणनम्].changefunc = toggle_shortcut_choice;
    choices[choice_गणनम्].installfunc = install_shortcut_gvim;
    toggle_shortcut_choice(choice_गणनम्);
    ++choice_गणनम्;

    // Shortcut to evim
    choices[choice_गणनम्].text = रिक्त;
    choices[choice_गणनम्].arg = 0;
    choices[choice_गणनम्].active = has_gvim;
    choices[choice_गणनम्].changefunc = toggle_shortcut_choice;
    choices[choice_गणनम्].installfunc = install_shortcut_evim;
    toggle_shortcut_choice(choice_गणनम्);
    ++choice_गणनम्;

    // Shortcut to gview
    choices[choice_गणनम्].text = रिक्त;
    choices[choice_गणनम्].arg = 0;
    choices[choice_गणनम्].active = has_gvim;
    choices[choice_गणनम्].changefunc = toggle_shortcut_choice;
    choices[choice_गणनम्].installfunc = install_shortcut_gview;
    toggle_shortcut_choice(choice_गणनम्);
    ++choice_गणनम्;
}

/*
 * Attempt to register OLE for Vim.
 */
   स्थिर शून्य
install_OLE_register(शून्य)
{
    वर्ण register_command_string[BUFSIZE + 30];

    printf("\n--- Attempting to register Vim with OLE ---\n");
    printf("(There is no message whether this works or not.)\n");

    sprintf(register_command_string, "\"%s\\gvim.exe\" -silent -register", installdir);
    system(register_command_string);
}

/*
 * Remove the last part of directory "path[]" to get its parent, and put the
 * result in "to[]".
 */
    स्थिर शून्य
dir_remove_last(ध्रुव वर्ण *path, वर्ण to[MAX_PATH])
{
    वर्ण c;
    long last_char_to_copy;
    long path_length = strlen(path);

    // skip the last character just in case it is a '\\'
    last_char_to_copy = path_length - 2;
    c = path[last_char_to_copy];

    जाव (c != '\\')
    {
	last_char_to_copy--;
	c = path[last_char_to_copy];
    }

    strncpy(to, path, (size_t)last_char_to_copy);
    to[last_char_to_copy] = NUL;
}

    स्थिर शून्य
set_directories_text(पूर्णाङ्क idx)
{
    पूर्णाङ्क vimfiles_dir_choice = choices[idx].arg;

    जइ (vimfiles_dir_choice == (पूर्णाङ्क)vimfiles_dir_none)
	alloc_text(idx, "Do NOT create plugin directories%s", "");
    अण्णहा
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
स्थिर वर्ण *homedir = रिक्त;

    शून्य
init_homedir(शून्य)
{
    वर्ण    *var;
    वर्ण    buf[MAX_PATH];

    जइ (homedir != रिक्त)
    {
	मुक्त(homedir);
	homedir = रिक्त;
    }

    var = getenv("HOME");

    /*
     * Typically, $HOME is not defined on Windows, unless the user has
     * specifically defined it for Vim's sake.  However, on Windows NT
     * platforms, $HOMEDRIVE and $HOMEPATH are automatically defined for
     * each user.  Try constructing $HOME from these.
     */
    जइ (var == रिक्त || *var == NUL)
    {
	वर्ण	*homedrive, *homepath;

	homedrive = getenv("HOMEDRIVE");
	homepath = getenv("HOMEPATH");
	जइ (homepath == रिक्त || *homepath == NUL)
	    homepath = "\\";
	जइ (homedrive != रिक्त
		   && strlen(homedrive) + strlen(homepath) < परिमाण(buf))
	{
	    जइ (snprintf(buf, परिमाण(buf), "%s%s", homedrive, homepath) > 0
		&& buf[0] != NUL)
		var = buf;
	}
    }

    जइ (var == रिक्त)
	var = getenv("USERPROFILE");

    /*
     * Weird but true: $HOME may contain an indirect reference to another
     * variable, esp. "%USERPROFILE%".  Happens when $USERPROFILE isn't set
     * when $HOME is being set.
     */
    जइ (var != रिक्त && *var == '%')
    {
	वर्ण	*p;
	वर्ण	*exp;

	p = strchr(var + 1, '%');
	जइ (p != रिक्त)
	{
	    strncpy(buf, var + 1, p - (var + 1));
	    buf[p - (var + 1)] = NUL;
	    exp = getenv(buf);
	    जइ (exp != रिक्त && *exp != NUL
				&& strlen(exp) + strlen(p) < परिमाण(buf))
	    {
		snprintf(buf, परिमाण(buf), "%s%s", exp, p + 1);
		var = buf;
	    }
	}
    }

    जइ (var != रिक्त && *var == NUL)	// empty is same as not set
	var = रिक्त;

    जइ (var == रिक्त)
	homedir = रिक्त;
    अण्णहा
	homedir = _strdup(var);
}

/*
 * Change the directory that the vim plugin directories will be created in:
 * $HOME, $VIM or nowhere.
 */
    स्थिर शून्य
change_directories_choice(पूर्णाङ्क idx)
{
    पूर्णाङ्क	    choice_गणनम् = TABLE_SIZE(vimfiles_dir_choices);

    // Don't offer the $HOME choice if $HOME isn't set.
    जइ (homedir == रिक्त)
	--choice_गणनम्;
    choices[idx].arg = get_choice(vimfiles_dir_choices, choice_गणनम्);
    set_directories_text(idx);
}

/*
 * Create the plugin directories...
 */
//ARGSUSED
    स्थिर शून्य
install_vimfilesdir(पूर्णाङ्क idx)
{
    पूर्णाङ्क i;
    पूर्णाङ्क vimfiles_dir_choice = choices[idx].arg;
    वर्ण *p;
    वर्ण vimdir_path[MAX_PATH];
    वर्ण vimfiles_path[MAX_PATH + 9];
    वर्ण tmp_dirname[BUFSIZE];

    // switch on the location that the user wants the plugin directories
    // built in
    विकल्प (vimfiles_dir_choice)
    {
	पक्ष vimfiles_dir_vim:
	{
	    // Go to the %VIM% directory - check env first, then go one dir
	    //	   below installdir if there is no %VIM% environment variable.
	    //	   The accuracy of $VIM is checked in inspect_system(), so we
	    //	   can be sure it is ok to use here.
	    p = getenv("VIM");
	    जइ (p == रिक्त) // No $VIM in path
		dir_remove_last(installdir, vimdir_path);
	    अण्णहा
		strcpy(vimdir_path, p);
	    भङ्ग;
	}
	पक्ष vimfiles_dir_home:
	{
	    // Find the $HOME directory.  Its existence was already checked.
	    p = homedir;
	    जइ (p == रिक्त)
	    {
		printf("Internal error: $HOME is NULL\n");
		p = "c:\\";
	    }
	    strcpy(vimdir_path, p);
	    भङ्ग;
	}
	पक्ष vimfiles_dir_none:
	{
	    // Do not create vim plugin directory.
	    प्रत्यागम;
	}
    }

    // Now, just create the directory.	If it already exists, it will fail
    // silently.
    sprintf(vimfiles_path, "%s\\vimfiles", vimdir_path);
    vim_mkdir(vimfiles_path, 0755);

    printf("Creating the following directories in \"%s\":\n", vimfiles_path);
    प्रति (i = 0; i < TABLE_SIZE(vimfiles_subdirs); i++)
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
    स्थिर शून्य
init_directories_choice(शून्य)
{
    संरचना stat	st;
    वर्ण	tmp_dirname[BUFSIZE];
    वर्ण	*p;
    पूर्णाङ्क		vimfiles_dir_choice;

    choices[choice_गणनम्].text = alloc(150);
    choices[choice_गणनम्].changefunc = change_directories_choice;
    choices[choice_गणनम्].installfunc = install_vimfilesdir;
    choices[choice_गणनम्].active = 1;

    // Check if the "compiler" directory already exists.  That's a good
    // indication that the plugin directories were already created.
    p = getenv("HOME");
    जइ (p != रिक्त)
    {
	vimfiles_dir_choice = (पूर्णाङ्क)vimfiles_dir_home;
	sprintf(tmp_dirname, "%s\\vimfiles\\compiler", p);
	जइ (stat(tmp_dirname, &st) == 0)
	    vimfiles_dir_choice = (पूर्णाङ्क)vimfiles_dir_none;
    }
    अण्णहा
    {
	vimfiles_dir_choice = (पूर्णाङ्क)vimfiles_dir_vim;
	p = getenv("VIM");
	जइ (p == रिक्त)  // No $VIM in path, use the install dir.
	    dir_remove_last(installdir, tmp_dirname);
	अण्णहा
	    strcpy(tmp_dirname, p);
	strcat(tmp_dirname, "\\vimfiles\\compiler");
	जइ (stat(tmp_dirname, &st) == 0)
	    vimfiles_dir_choice = (पूर्णाङ्क)vimfiles_dir_none;
    }

    choices[choice_गणनम्].arg = vimfiles_dir_choice;
    set_directories_text(choice_गणनम्);
    ++choice_गणनम्;
}

/*
 * Setup the choices and the default values.
 */
    स्थिर शून्य
setup_choices(शून्य)
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

    स्थिर शून्य
print_cmd_पङ्क्तिः_help(शून्य)
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
    स्थिर शून्य
command_पङ्क्तिः_setup_choices(पूर्णाङ्क argc, वर्ण **argv)
{
    पूर्णाङ्क i, j;

    प्रति (i = 1; i < argc; i++)
    {
	जइ (strcmp(argv[i], "-create-batfiles") == 0)
	{
	    जइ (i + 1 == argc)
		अनुवर्त;
	    जाव (argv[i + 1][0] != '-' && i < argc)
	    {
		i++;
		प्रति (j = 1; j < TARGET_गणनम्; ++j)
		    जइ ((targets[j].exenamearg[0] == 'g' ? has_gvim : has_vim)
			    && strcmp(argv[i], targets[j].name) == 0)
		    {
			init_bat_choice(j);
			भङ्ग;
		    }
		जइ (j == TARGET_गणनम्)
		    printf("%s is not a valid choice for -create-batfiles\n",
								     argv[i]);

		जइ (i + 1 == argc)
		    भङ्ग;
	    }
	}
	अण्णहा जइ (strcmp(argv[i], "-create-vimrc") == 0)
	{
	    // Setup default vimrc choices.  If there is already a _vimrc file,
	    // it will NOT be overwritten.
	    init_vimrc_choices();
	}
	अण्णहा जइ (strcmp(argv[i], "-vimrc-remap") == 0)
	{
	    जइ (i + 1 == argc)
		भङ्ग;
	    i++;
	    जइ (strcmp(argv[i], "no") == 0)
		remap_choice = remap_no;
	    अण्णहा जइ (strcmp(argv[i], "win") == 0)
		remap_choice = remap_win;
	}
	अण्णहा जइ (strcmp(argv[i], "-vimrc-behave") == 0)
	{
	    जइ (i + 1 == argc)
		भङ्ग;
	    i++;
	    जइ (strcmp(argv[i], "unix") == 0)
		mouse_choice = mouse_xterm;
	    अण्णहा जइ (strcmp(argv[i], "mswin") == 0)
		mouse_choice = mouse_mswin;
	    अण्णहा जइ (strcmp(argv[i], "default") == 0)
		mouse_choice = mouse_default;
	}
	अण्णहा जइ (strcmp(argv[i], "-vimrc-compat") == 0)
	{
	    जइ (i + 1 == argc)
		भङ्ग;
	    i++;
	    जइ (strcmp(argv[i], "vi") == 0)
		compat_choice = compat_vi;
	    अण्णहा जइ (strcmp(argv[i], "vim") == 0)
		compat_choice = compat_vim;
	    अण्णहा जइ (strcmp(argv[i], "defaults") == 0)
		compat_choice = compat_some_enhancements;
	    अण्णहा जइ (strcmp(argv[i], "all") == 0)
		compat_choice = compat_all_enhancements;
	}
	अण्णहा जइ (strcmp(argv[i], "-install-popup") == 0)
	{
	    init_popup_choice();
	}
	अण्णहा जइ (strcmp(argv[i], "-install-openwith") == 0)
	{
	    init_openwith_choice();
	}
	अण्णहा जइ (strcmp(argv[i], "-add-start-menu") == 0)
	{
	    init_startmenu_choice();
	}
	अण्णहा जइ (strcmp(argv[i], "-install-icons") == 0)
	{
	    init_shortcut_choices();
	}
	अण्णहा जइ (strcmp(argv[i], "-create-directories") == 0)
	{
	    पूर्णाङ्क vimfiles_dir_choice = (पूर्णाङ्क)vimfiles_dir_none;

	    init_directories_choice();
	    जइ (i + 1 < argc && argv[i + 1][0] != '-')
	    {
		i++;
		जइ (strcmp(argv[i], "vim") == 0)
		    vimfiles_dir_choice = (पूर्णाङ्क)vimfiles_dir_vim;
		अण्णहा जइ (strcmp(argv[i], "home") == 0)
		{
		    जइ (homedir == रिक्त)  // No $HOME in environment
			vimfiles_dir_choice = (पूर्णाङ्क)vimfiles_dir_none;
		    अण्णहा
			vimfiles_dir_choice = (पूर्णाङ्क)vimfiles_dir_home;
		}
		अण्णहा
		{
		    printf("Unknown argument for -create-directories: %s\n",
								     argv[i]);
		    print_cmd_पङ्क्तिः_help();
		}
	    }
	    अण्णहा // No choice specified, default to vim directory
		vimfiles_dir_choice = (पूर्णाङ्क)vimfiles_dir_vim;
	    choices[choice_गणनम् - 1].arg = vimfiles_dir_choice;
	}
	अण्णहा जइ (strcmp(argv[i], "-register-OLE") == 0)
	{
	    // This is always done when gvim is found
	}
	अण्णहा // Unknown switch
	{
	    printf("Got unknown argument argv[%d] = %s\n", i, argv[i]);
	    print_cmd_पङ्क्तिः_help();
	}
    }
}


/*
 * Show a few screens full of helpful information.
 */
    स्थिर शून्य
show_help(शून्य)
{
    स्थिर वर्ण *(वस्तूनि[]) =
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
रिक्त
    };
    पूर्णाङ्क		i;
    पूर्णाङ्क		c;

    rewind(stdin);
    printf("\n");
    प्रति (i = 0; वस्तूनि[i] != रिक्त; ++i)
    {
	puts(वस्तूनि[i]);
	printf("Hit Enter to continue, b (back) or q (quit help): ");
	c = getchar();
	rewind(stdin);
	जइ (c == 'b' || c == 'B')
	{
	    जइ (i == 0)
		--i;
	    अण्णहा
		i -= 2;
	}
	जइ (c == 'q' || c == 'Q')
	    भङ्ग;
	printf("\n");
    }
}

/*
 * Install the choices.
 */
    स्थिर शून्य
install(शून्य)
{
    पूर्णाङ्क		i;

    // Install the selected choices.
    प्रति (i = 0; i < choice_गणनम्; ++i)
	जइ (choices[i].installfunc != रिक्त && choices[i].active)
	    (choices[i].installfunc)(i);

    // Add some entries to the registry, if needed.
    जइ (install_popup
	    || install_openwith
	    || (need_uninstall_entry && interactive)
	    || !interactive)
	install_registry();

    // Register gvim with OLE.
    जइ (has_gvim)
	install_OLE_register();
}

/*
 * request_choice
 */
    स्थिर शून्य
request_choice(शून्य)
{
    पूर्णाङ्क		      i;

    printf("\n\nInstall will do for you:\n");
    प्रति (i = 0; i < choice_गणनम्; ++i)
	जइ (choices[i].active)
	    printf("%2d  %s\n", i + 1, choices[i].text);
    printf("To change an item, enter its number\n\n");
    printf("Enter item number, h (help), d (do it) or q (quit): ");
}

    पूर्णाङ्क
main(पूर्णाङ्क argc, वर्ण **argv)
{
    पूर्णाङ्क		i;
    वर्ण	buf[BUFSIZE];

    /*
     * Run interactively if there are no command line arguments.
     */
    जइ (argc > 1)
	interactive = 0;
    अण्णहा
	interactive = 1;

    // Initialize this program.
    do_inits(argv);
    init_homedir();

    जइ (argc > 1 && strcmp(argv[1], "-uninstall-check") == 0)
    {
	// Only check for already installed Vims.  Used by NSIS installer.
	i = uninstall_check(1);

	// Find the value of $VIM, because NSIS isn't able to do this by
	// itself.
	get_vim_env();

	// When nothing found exit quietly.  If something found wait for
	// a little while, so that the user can read the messages.
	जइ (i && _isatty(1))
	    sleep(3);
	exit(0);
    }

    printf("This program sets up the installation of Vim "
						   VIM_VERSION_MEDIUM "\n\n");

    // Check if the user unpacked the archives properly.
    check_unpack();

    // Check for already installed Vims.
    जइ (interactive)
	uninstall_check(0);

    // Find out information about the system.
    inspect_system();

    जइ (interactive)
    {
	// Setup all the choices.
	setup_choices();

	// Let the user change choices and finally install (or quit).
	प्रति (;;)
	{
	    request_choice();
	    rewind(stdin);
	    जइ (scanf("%99s", buf) == 1)
	    {
		जइ (isdigit((unsigned वर्ण)buf[0]))
		{
		    // Change a choice.
		    i = atoi(buf);
		    जइ (i > 0 && i <= choice_गणनम् && choices[i - 1].active)
			(choices[i - 1].changefunc)(i - 1);
		    अण्णहा
			printf("\nIllegal choice\n");
		}
		अण्णहा जइ (buf[0] == 'h' || buf[0] == 'H')
		{
		    // Help
		    show_help();
		}
		अण्णहा जइ (buf[0] == 'd' || buf[0] == 'D')
		{
		    // Install!
		    install();
		    printf("\nThat finishes the installation.  Happy Vimming!\n");
		    भङ्ग;
		}
		अण्णहा जइ (buf[0] == 'q' || buf[0] == 'Q')
		{
		    // Quit
		    printf("\nExiting without anything done\n");
		    भङ्ग;
		}
		अण्णहा
		    printf("\nIllegal choice\n");
	    }
	}
	printf("\n");
	myexit(0);
    }
    अण्णहा
    {
	/*
	 * Run non-interactive - setup according to the command line switches
	 */
	command_पङ्क्तिः_setup_choices(argc, argv);
	install();

	// Avoid that the user has to hit Enter, just wait a little bit to
	// allow reading the messages.
	sleep(2);
    }

    प्रत्यागम 0;
}
