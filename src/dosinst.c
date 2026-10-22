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
#ضمن "dosinst.h"
#ضمن <io.h>

#define GVIMEXT64_PATH	    "GvimExt64\\gvimext.dll"
#define GVIMEXT32_PATH	    "GvimExt32\\gvimext.dll"

// Macro to do an error check I was typing over and over
#define CHECK_REG_ERROR(code) \
    افعل { \
	إذا (code != ERROR_SUCCESS) \
	{ \
	    printf("%ld error number:  %ld\n", (long)__LINE__, (long)code); \
	    أرجع 1; \
	} \
    } طالما (0)

صحيح	has_vim = 0;		// installable vim.exe exists
صحيح	has_gvim = 0;		// installable gvim.exe exists

حرف	oldvimrc[BUFSIZE];	// name of existing vimrc file
حرف	vimrc[BUFSIZE];		// name of vimrc file to create

حرف	*default_bat_dir = عدم;  // when not NULL, use this as the default
				  // directory to write .bat files in
حرف	*default_vim_dir = عدم;  // when not NULL, use this as the default
				  // install dir for NSIS

/*
 * Structure used for each choice the user can make.
 */
بنية choice
{
    صحيح	    active;			// non-zero when choice is active
    حرف    *text;			// text displayed for this choice
    فراغ    (*changefunc)(صحيح idx);	// function to change this choice
    صحيح	    arg;			// argument for function
    فراغ    (*installfunc)(صحيح idx);	// function to install this choice
};

بنية choice	choices[30];		// choices the user can make
صحيح		choice_ژماردن = 0;	// number of choices available

#define TABLE_SIZE(s)	(صحيح)ARRAYSIZE(s)

تعداد
{
    compat_vi = 1,
    compat_vim,
    compat_some_enhancements,
    compat_all_enhancements
};
حرف	*(compat_choices[]) =
{
    "\nChoose the default way to run Vim:",
    "Vi compatible",
    "Vim default",
    "with some Vim enhancements",
    "with syntax highlighting and other features switched on",
};
صحيح	compat_choice = (صحيح)compat_all_enhancements;
حرف	*compat_text = "- run Vim %s";

تعداد
{
    remap_no = 1,
    remap_win
};
حرف	*(remap_choices[]) =
{
    "\nChoose:",
    "Do not remap keys for Windows behavior",
    "Remap a few keys for Windows behavior (CTRL-V, CTRL-C, CTRL-F, etc)",
};
صحيح	remap_choice = (صحيح)remap_no;
حرف	*remap_text = "- %s";

تعداد
{
    mouse_xterm = 1,
    mouse_mswin,
    mouse_default
};
حرف	*(mouse_choices[]) =
{
    "\nChoose the way how Vim uses the mouse:",
    "right button extends selection (the Unix way)",
    "right button has a popup menu, left button starts select mode (the Windows way)",
    "right button has a popup menu, left button starts visual mode",
};
صحيح	mouse_choice = (صحيح)mouse_default;
حرف	*mouse_text = "- The mouse %s";

تعداد
{
    vimfiles_dir_none = 1,
    vimfiles_dir_vim,
    vimfiles_dir_home
};
ساكن حرف *(vimfiles_dir_choices[]) =
{
    "\nCreate plugin directories:",
    "No",
    "In the VIM directory",
    "In your HOME directory",
};

// non-zero when selected to install the popup menu entry.
ساكن صحيح	install_popup = 0;

// non-zero when selected to install the "Open with" entry.
ساكن صحيح	install_openwith = 0;

// non-zero when need to add an uninstall entry in the registry
ساكن صحيح	need_uninstall_entry = 0;

/*
 * Definitions of the directory name (under $VIM) of the vimfiles directory
 * and its subdirectories:
 */
ساكن حرف	*(vimfiles_subdirs[]) =
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
    ساكن صحيح
get_choice(حرف **table, صحيح entries)
{
    صحيح		answer;
    صحيح		idx;
    حرف	dummy[100];

    افعل
    {
	لكل (idx = 0; idx < entries; ++idx)
	{
	    إذا (idx)
		printf("%2d  ", idx);
	    puts(table[idx]);
	}
	printf("Choice: ");
	إذا (scanf("%d", &answer) != 1)
	{
	    scanf("%99s", dummy);
	    answer = 0;
	}
    }
    طالما (answer < 1 || answer >= entries);

    أرجع answer;
}

/*
 * Check if the user unpacked the archives properly.
 * Sets "runtimeidx".
 */
    ساكن فراغ
check_unpack(فراغ)
{
    حرف	buf[BUFSIZE];
    FILE	*fd;
    بنية stat	st;

    // check for presence of the correct version number in installdir[]
    runtimeidx = strlen(installdir) - strlen(VIM_VERSION_NODOT);
    إذا (runtimeidx <= 0
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
    إذا (stat(buf, &st) < 0)
    {
	printf("ERROR: Cannot find filetype.vim in \"%s\"\n", installdir);
	printf("It looks like you did not unpack the runtime archive.\n");
	printf("You must unpack the runtime archive \"%srt.zip\" before installing.\n",
		VIM_VERSION_NODOT);
	myexit(1);
    }

    // Check if vim.exe or gvim.exe is in the current directory.
    إذا ((fd = fopen("gvim.exe", "r")) != عدم)
    {
	fclose(fd);
	has_gvim = 1;
    }
    إذا ((fd = fopen("vim.exe", "r")) != عدم)
    {
	fclose(fd);
	has_vim = 1;
    }
    إذا (!has_gvim && !has_vim)
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
    ساكن صحيح
pathcmp(حرف *p, صحيح plen, حرف *q, صحيح qlen)
{
    صحيح		i;

    إذا (plen < 0)
	plen = strlen(p);
    إذا (qlen < 0)
	qlen = strlen(q);
    لكل (i = 0; ; ++i)
    {
	// End of "p": check if "q" also ends or just has a slash.
	إذا (i == plen)
	{
	    إذا (i == qlen)  // match
		أرجع 0;
	    إذا (i == qlen - 1 && (q[i] == '\\' || q[i] == '/'))
		أرجع 0;   // match with trailing slash
	    أرجع 1;	    // no match
	}

	// End of "q": check if "p" also ends or just has a slash.
	إذا (i == qlen)
	{
	    إذا (i == plen)  // match
		أرجع 0;
	    إذا (i == plen - 1 && (p[i] == '\\' || p[i] == '/'))
		أرجع 0;   // match with trailing slash
	    أرجع 1;	    // no match
	}

	إذا (!(mytoupper(p[i]) == mytoupper(q[i])
		|| ((p[i] == '/' || p[i] == '\\')
		    && (q[i] == '/' || q[i] == '\\'))))
	    أرجع 1;	    // no match
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
    ساكن فراغ
findoldfile(حرف **destination)
{
    حرف	*bp = *destination;
    size_t	indir_l = strlen(installdir);
    حرف	*cp;
    حرف	*tmpname;
    حرف	*farname;

    /*
     * No action needed if exe not found or not in this directory.
     */
    إذا (bp == عدم || strnicmp(bp, installdir, indir_l) != 0)
	أرجع;
    cp = bp + indir_l;
    إذا (strchr("/\\", *cp++) == عدم
	    || strchr(cp, '\\') != عدم
	    || strchr(cp, '/') != عدم)
	أرجع;

    tmpname = alloc(strlen(cp) + 1);
    strcpy(tmpname, cp);
    tmpname[strlen(tmpname) - 1] = 'x';	// .exe -> .exx

    إذا (access(tmpname, 0) == 0)
    {
	printf("\nERROR: %s and %s clash.  Remove or rename %s.\n",
	    tmpname, cp, tmpname);
	myexit(1);
    }

    إذا (rename(cp, tmpname) != 0)
    {
	printf("\nERROR: failed to rename %s to %s: %s\n",
	    cp, tmpname, strerror(0));
	myexit(1);
    }

    farname = searchpath_save(cp);

    إذا (rename(tmpname, cp) != 0)
    {
	printf("\nERROR: failed to rename %s back to %s: %s\n",
	    tmpname, cp, strerror(0));
	myexit(1);
    }

    حرر(*destination);
    حرر(tmpname);
    *destination = farname;
}

/*
 * Check if there is a vim.[exe|bat|, gvim.[exe|bat|, etc. in the path.
 * When "check_bat_only" is TRUE, only find "default_bat_dir".
 */
    ساكن فراغ
find_bat_exe(صحيح check_bat_only)
{
    صحيح		i;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    لكل (i = 1; i < TARGET_ژماردن; ++i)
    {
	targets[i].oldbat = searchpath_save(targets[i].batname);
	إذا (!check_bat_only)
	    targets[i].oldexe = searchpath_save(targets[i].exename);

	إذا (default_bat_dir == عدم && targets[i].oldbat != عدم)
	{
	    default_bat_dir = alloc(strlen(targets[i].oldbat) + 1);
	    strcpy(default_bat_dir, targets[i].oldbat);
	    remove_tail(default_bat_dir);
	}
	إذا (check_bat_only && targets[i].oldbat != عدم)
	{
	    حرر(targets[i].oldbat);
	    targets[i].oldbat = عدم;
	}
    }

    mch_chdir(installdir);
}

/*
 * Get the value of $VIMRUNTIME or $VIM and write it in $TEMP/vimini.ini, so
 * that NSIS can read it.
 * When not set, use the directory of a previously installed Vim.
 */
    ساكن فراغ
get_vim_env(فراغ)
{
    حرف	*vim;
    حرف	buf[BUFSIZE];
    FILE	*fd;
    حرف	fname[BUFSIZE];

    // First get $VIMRUNTIME.  If it's set, remove the tail.
    vim = getenv("VIMRUNTIME");
    إذا (vim != عدم && *vim != 0 && strlen(vim) < حجم(buf))
    {
	strcpy(buf, vim);
	remove_tail(buf);
	vim = buf;
    }
    وإلا
    {
	vim = getenv("VIM");
	إذا (vim == عدم || *vim == 0)
	{
	    // Use the directory from an old uninstall entry.
	    إذا (default_vim_dir != عدم)
		vim = default_vim_dir;
	    وإلا
		// Let NSIS know there is no default, it should use
		// $PROGRAMFILES.
		vim = "";
	}
    }

    // NSIS also uses GetTempPath(), thus we should get the same directory
    // name as where NSIS will look for vimini.ini.
    GetTempPath(حجم(fname) - 12, fname);
    add_pathsep(fname);
    strcat(fname, "vimini.ini");

    fd = fopen(fname, "w");
    إذا (fd != عدم)
    {
	// Make it look like an .ini file, so that NSIS can read it with a
	// ReadINIStr command.
	fprintf(fd, "[vimini]\n");
	fprintf(fd, "dir=\"%s\"\n", vim);
	fclose(fd);
    }
    وإلا
    {
	printf("Failed to open %s\n", fname);
	sleep(2);
    }
}

ساكن صحيح ژمارە_windows;

/*
 * Callback used for EnumWindows():
 * Count the window if the title looks like it is for the uninstaller.
 */
//ARGSUSED
    ساكن BOOL CALLBACK
window_cb(HWND hwnd, LPARAM lparam UNUSED)
{
    حرف title[256];

    title[0] = 0;
    GetWindowText(hwnd, title, 256);
    إذا (strstr(title, "Vim ") != عدم && strstr(title, " Uninstall") != عدم)
	++ژمارە_windows;
    أرجع TRUE;
}

/*
 * Run the uninstaller silently.
 */
    ساكن صحيح
run_silent_uninstall(حرف *uninst_exe)
{
    حرف    vimrt_dir[BUFSIZE];
    حرف    temp_uninst[BUFSIZE];
    حرف    temp_dir[MAX_PATH];
    حرف    buf[BUFSIZE * 2 + 10];
    صحيح	    i;
    DWORD   tick;

    strcpy(vimrt_dir, uninst_exe);
    remove_tail(vimrt_dir);

    إذا (!GetTempPath(حجم(temp_dir), temp_dir))
	أرجع FAIL;

    // Copy the uninstaller to a temporary exe.
    tick = Get_Tick_ژماردن();
    لكل (i = 0; ; i++)
    {
	sprintf(temp_uninst, "%s\\vimun%04X.exe", temp_dir,
					  (unsigned صحيح)((i + tick) & 0xFFFF));
	إذا (CopyFile(uninst_exe, temp_uninst, TRUE))
	    اكسر;
	إذا (GetLastError() != ERROR_FILE_EXISTS)
	    أرجع FAIL;
	إذا (i == 65535)
	    أرجع FAIL;
    }

    // Run the copied uninstaller silently.
    إذا (strchr(temp_uninst, ' ') != عدم)
	sprintf(buf, "\"%s\" /S _?=%s", temp_uninst, vimrt_dir);
    وإلا
	sprintf(buf, "%s /S _?=%s", temp_uninst, vimrt_dir);
    run_command(buf);

    DeleteFile(temp_uninst);
    أرجع OK;
}

/*
 * Check for already installed Vims.
 * Return non-zero when found one.
 */
    ساكن صحيح
uninstall_check(صحيح skip_question)
{
    HKEY	کلیل_handle;
    HKEY	uninstall_کلیل_handle;
    حرف	*uninstall_کلیل = "software\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
    حرف	subkey_name_buff[BUFSIZE];
    حرف	temp_string_buffer[BUFSIZE-2];
    DWORD	local_bufsize;
    FILETIME	temp_pfiletime;
    DWORD	کلیل_index;
    حرف	تێخستن;
    long	code;
    DWORD	بەها_type;
    DWORD	orig_ژمارە_کلیلەکان;
    DWORD	new_ژمارە_کلیلەکان;
    DWORD	allow_silent;
    صحيح		foundone = 0;

    code = Reg_Open_کلیل_Ex(HKEY_LOCAL_MACHINE, uninstall_کلیل, 0,
				     کلیل_WOW64_64_کلیل | کلیل_READ, &کلیل_handle);
    CHECK_REG_ERROR(code);

    کلیل_index = 0;
    طالما (TRUE)
    {
	local_bufsize = حجم(subkey_name_buff);
	إذا (Reg_Enum_کلیل_Ex(کلیل_handle, کلیل_index, subkey_name_buff, &local_bufsize,
		عدم, عدم, عدم, &temp_pfiletime) == ERROR_NO_MORE_کاڵاکان)
	    اكسر;

	إذا (strncmp("Vim", subkey_name_buff, 3) == 0)
	{
	    // Open the key named Vim*
	    code = Reg_Open_کلیل_Ex(کلیل_handle, subkey_name_buff, 0,
			   کلیل_WOW64_64_کلیل | کلیل_READ, &uninstall_کلیل_handle);
	    CHECK_REG_ERROR(code);

	    // get the DisplayName out of it to show the user
	    local_bufsize = حجم(temp_string_buffer);
	    code = Reg_Query_بەها_Ex(uninstall_کلیل_handle, "displayname", 0,
		    &بەها_type, (LPBYTE)temp_string_buffer,
		    &local_bufsize);
	    CHECK_REG_ERROR(code);

	    allow_silent = 0;
	    إذا (skip_question)
	    {
		DWORD varsize = حجم(DWORD);

		Reg_Query_بەها_Ex(uninstall_کلیل_handle, "AllowSilent", 0,
			&بەها_type, (LPBYTE)&allow_silent,
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

	    إذا (skip_question)
		printf("\nRunning uninstall program for \"%s\"\n", temp_string_buffer);
	    وإلا
		printf("\nDo you want to uninstall \"%s\" now?\n(y)es/(n)o)  ", temp_string_buffer);
	    fflush(stdout);

	    // get the UninstallString
	    local_bufsize = حجم(temp_string_buffer);
	    code = Reg_Query_بەها_Ex(uninstall_کلیل_handle, "uninstallstring", 0,
		    &بەها_type, (LPBYTE)temp_string_buffer, &local_bufsize);
	    CHECK_REG_ERROR(code);

	    // Remember the directory, it is used as the default for NSIS.
	    default_vim_dir = alloc(strlen(temp_string_buffer) + 1);
	    strcpy(default_vim_dir, temp_string_buffer);
	    remove_tail(default_vim_dir);
	    remove_tail(default_vim_dir);

	    تێخستن = 'n';
	    افعل
	    {
		إذا (تێخستن != 'n')
		    printf("%c is an invalid reply.  Please enter either 'y' or 'n'\n", تێخستن);

		إذا (skip_question)
		    تێخستن = 'y';
		وإلا
		{
		    rewind(stdin);
		    scanf("%c", &تێخستن);
		}
		بدل (تێخستن)
		{
		    حالة 'y':
		    حالة 'Y':
			// save the number of uninstall keys so we can know if
			// it changed
			Reg_Query_Info_کلیل(کلیل_handle, عدم, عدم, عدم,
					     &orig_ژمارە_کلیلەکان, عدم, عدم, عدم,
						      عدم, عدم, عدم, عدم);

			// Find existing .bat files before deleting them.
			find_bat_exe(TRUE);

			إذا (allow_silent)
			{
			    إذا (run_silent_uninstall(temp_string_buffer)
								    == FAIL)
				allow_silent = 0; // Retry with non silent.
			}
			إذا (!allow_silent)
			{
			    // Execute the uninstall program.  Put it in double
			    // quotes if there is an embedded space.
			    {
				حرف buf[BUFSIZE];

				إذا (strchr(temp_string_buffer, ' ') != عدم)
				    sprintf(buf, "\"%s\"", temp_string_buffer);
				وإلا
				    strcpy(buf, temp_string_buffer);
				run_command(buf);
			    }

			    // Count the number of windows with a title that
			    // match the installer, so that we can check when
			    // it's done.  The uninstaller copies itself,
			    // executes the copy and exits, thus we can't wait
			    // for the process to finish.
			    sleep(1);  // wait for uninstaller to start up
			    ژمارە_windows = 0;
			    EnumWindows(window_cb, 0);
			    إذا (ژمارە_windows == 0)
			    {
				// Did not find the uninstaller, ask user to
				// press Enter when done. Just in case.
				printf("Press Enter when the uninstaller is finished\n");
				rewind(stdin);
				(فراغ)getchar();
			    }
			    وإلا
			    {
				printf("Waiting for the uninstaller to finish (press CTRL-C to abort).");
				افعل
				{
				    printf(".");
				    fflush(stdout);
				    sleep(1);	// wait for the uninstaller to
						// finish
				    ژمارە_windows = 0;
				    EnumWindows(window_cb, 0);
				} طالما (ژمارە_windows > 0);
			    }
			}
			printf("\nDone!\n");

			// Check if an uninstall reg key was deleted.
			// if it was, we want to decrement key_index.
			// if we don't do this, we will skip the key
			// immediately after any key that we delete.
			Reg_Query_Info_کلیل(کلیل_handle, عدم, عدم, عدم,
					      &new_ژمارە_کلیلەکان, عدم, عدم, عدم,
						      عدم, عدم, عدم, عدم);
			إذا (new_ژمارە_کلیلەکان < orig_ژمارە_کلیلەکان)
			    کلیل_index--;

			تێخستن = 'y';
			اكسر;

		    حالة 'n':
		    حالة 'N':
			// Do not uninstall
			تێخستن = 'n';
			اكسر;

		    افتراضي: // just drop through and redo the loop
			اكسر;
		}

	    } طالما (تێخستن != 'n' && تێخستن != 'y');

	    Reg_Close_کلیل(uninstall_کلیل_handle);
	}

	کلیل_index++;
    }
    Reg_Close_کلیل(کلیل_handle);

    أرجع foundone;
}

/*
 * Find out information about the system.
 */
    ساكن فراغ
inspect_system(فراغ)
{
    حرف	*p;
    حرف	buf[BUFSIZE];
    FILE	*fd;
    صحيح		i;
    صحيح		foundone;

    // This may take a little while, let the user know what we're doing.
    printf("Inspecting system...\n");

    /*
     * If $VIM is set, check that it's pointing to our directory.
     */
    p = getenv("VIM");
    إذا (p != عدم && pathcmp(p, -1, installdir, runtimeidx - 1) != 0)
    {
	printf("------------------------------------------------------\n");
	printf("$VIM is set to \"%s\".\n", p);
	printf("This is different from where this version of Vim is:\n");
	strcpy(buf, installdir);
	*(buf + runtimeidx - 1) = NUL;
	printf("\"%s\"\n", buf);
	printf("You must adjust or remove the setting of $VIM,\n");
	إذا (interactive)
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
    إذا (p != عدم && pathcmp(p, -1, installdir, -1) != 0)
    {
	printf("------------------------------------------------------\n");
	printf("$VIMRUNTIME is set to \"%s\".\n", p);
	printf("This is different from where this version of Vim is:\n");
	printf("\"%s\"\n", installdir);
	printf("You must adjust or remove the setting of $VIMRUNTIME,\n");
	إذا (interactive)
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
    لكل (i = 1; i < TARGET_ژماردن; ++i)
    {
	findoldfile(&(targets[i].oldexe));
	إذا (targets[i].oldexe != عدم)
	    foundone = 1;
    }

    إذا (foundone)
    {
	printf("Warning: Found Vim executable(s) in your $PATH:\n");
	لكل (i = 1; i < TARGET_ژماردن; ++i)
	    إذا (targets[i].oldexe != عدم)
		printf("%s\n", targets[i].oldexe);
	printf("It will be used instead of the version you are installing.\n");
	printf("Please delete or rename it, or adjust your $PATH setting.\n");
    }

    /*
     * Check if there is an existing ../_vimrc or ../.vimrc file.
     */
    strcpy(oldvimrc, installdir);
    strcpy(oldvimrc + runtimeidx, "_vimrc");
    إذا ((fd = fopen(oldvimrc, "r")) == عدم)
    {
	strcpy(oldvimrc + runtimeidx, "vimrc~1"); // short version of .vimrc
	إذا ((fd = fopen(oldvimrc, "r")) == عدم)
	{
	    strcpy(oldvimrc + runtimeidx, ".vimrc");
	    fd = fopen(oldvimrc, "r");
	}
    }
    إذا (fd != عدم)
	fclose(fd);
    وإلا
	*oldvimrc = NUL;
}

/*
 * Add a dummy choice to avoid that the numbering changes depending on items
 * in the environment.  The user may type a number he remembered without
 * looking.
 */
    ساكن فراغ
add_dummy_choice(فراغ)
{
    choices[choice_ژماردن].installfunc = عدم;
    choices[choice_ژماردن].active = 0;
    choices[choice_ژماردن].changefunc = عدم;
    choices[choice_ژماردن].text = عدم;
    choices[choice_ژماردن].arg = 0;
    ++choice_ژماردن;
}

////////////////////////////////////////////////
// stuff for creating the batch files.

/*
 * Install the vim.bat, gvim.bat, etc. files.
 */
    ساكن فراغ
install_bat_choice(صحيح idx)
{
    حرف	*batpath = targets[choices[idx].arg].batpath;
    حرف	*oldname = targets[choices[idx].arg].oldbat;
    حرف	*exename = targets[choices[idx].arg].exenamearg;
    حرف	*vimarg = targets[choices[idx].arg].exearg;
    FILE	*fd;

    إذا (*batpath == NUL)
	أرجع;

    fd = fopen(batpath, "w");
    إذا (fd == عدم)
    {
	printf("\nERROR: Cannot open \"%s\" for writing.\n", batpath);
	أرجع;
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

    إذا (*exename == 'g')
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

    إذا (*exename == 'g')
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
    وإلا
    {
	// Always use quotes, $VIM or $VIMRUNTIME might have a space.
	fprintf(fd, "\"%%VIM_EXE_DIR%%\\%s\" %s %%*\n",
		exename, vimarg);
    }

    fclose(fd);
    printf("%s has been %s\n", batpath,
	    oldname == عدم ? "created" : "overwritten");
}

/*
 * Make the text string for choice "idx".
 * The format "fmt" is must have one %s item, which "arg" is used for.
 */
    ساكن فراغ
alloc_text(صحيح idx, حرف *fmt, حرف *arg)
{
    إذا (choices[idx].text != عدم)
	حرر(choices[idx].text);

    choices[idx].text = alloc(strlen(fmt) + strlen(arg) - 1);
    sprintf(choices[idx].text, fmt, arg);
}

/*
 * Toggle the "Overwrite .../vim.bat" to "Don't overwrite".
 */
    ساكن فراغ
toggle_bat_choice(صحيح idx)
{
    حرف	*batname = targets[choices[idx].arg].batpath;
    حرف	*oldname = targets[choices[idx].arg].oldbat;

    إذا (*batname == NUL)
    {
	alloc_text(idx, "    Overwrite %s", oldname);
	strcpy(batname, oldname);
    }
    وإلا
    {
	alloc_text(idx, "    Do NOT overwrite %s", oldname);
	*batname = NUL;
    }
}

/*
 * Do some work for a batch file entry: Append the batch file name to the path
 * and set the text for the choice.
 */
    ساكن فراغ
set_bat_text(صحيح idx, حرف *batpath, حرف *name)
{
    strcat(batpath, name);

    alloc_text(idx, "    Create %s", batpath);
}

/*
 * Select a directory to write the batch file line.
 */
    ساكن فراغ
change_bat_choice(صحيح idx)
{
    حرف	*path;
    حرف	*batpath;
    حرف	*name;
    صحيح		n;
    حرف	*s;
    حرف	*p;
    صحيح		ژماردن;
    حرف	**names = عدم;
    صحيح		i;
    صحيح		target = choices[idx].arg;

    name = targets[target].batname;
    batpath = targets[target].batpath;

    path = getenv("PATH");
    إذا (path == عدم)
    {
	printf("\nERROR: The variable $PATH is not set\n");
	أرجع;
    }

    /*
     * first round: count number of names in path;
     * second round: save names to names[].
     */
    لكل (;;)
    {
	ژماردن = 1;
	لكل (p = path; *p; )
	{
	    s = strchr(p, ';');
	    إذا (s == عدم)
		s = p + strlen(p);
	    إذا (names != عدم)
	    {
		names[ژماردن] = alloc(s - p + 1);
		strncpy(names[ژماردن], p, s - p);
		names[ژماردن][s - p] = NUL;
	    }
	    ++ژماردن;
	    p = s;
	    إذا (*p != NUL)
		++p;
	}
	إذا (names != عدم)
	    اكسر;
	names = alloc((ژماردن + 1) * حجم(حرف *));
    }
    names[0] = alloc(50);
    sprintf(names[0], "Select directory to create %s in:", name);
    names[ژماردن] = alloc(50);
    إذا (choices[idx].arg == 0)
	sprintf(names[ژماردن], "Do not create any .bat file.");
    وإلا
	sprintf(names[ژماردن], "Do not create a %s file.", name);
    n = get_choice(names, ژماردن + 1);

    إذا (n == ژماردن)
    {
	// Selected last item, don't create bat file.
	*batpath = NUL;
	إذا (choices[idx].arg != 0)
	    alloc_text(idx, "    Do NOT create %s", name);
    }
    وإلا
    {
	// Selected one of the paths.  For the first item only keep the path,
	// for the others append the batch file name.
	strcpy(batpath, names[n]);
	add_pathsep(batpath);
	إذا (choices[idx].arg != 0)
	    set_bat_text(idx, batpath, name);
    }

    لكل (i = 0; i <= ژماردن; ++i)
	حرر(names[i]);
    حرر(names);
}

حرف *bat_text_yes = "Install .bat files to use Vim at the command line:";
حرف *bat_text_no = "do NOT install .bat files to use Vim at the command line";

    ساكن فراغ
change_main_bat_choice(صحيح idx)
{
    صحيح		i;

    // let the user select a default directory or NONE
    change_bat_choice(idx);

    إذا (targets[0].batpath[0] != NUL)
	choices[idx].text = bat_text_yes;
    وإلا
	choices[idx].text = bat_text_no;

    // update the individual batch file selections
    لكل (i = 1; i < TARGET_ژماردن; ++i)
    {
	// Only make it active when the first item has a path and the vim.exe
	// or gvim.exe exists (there is a changefunc then).
	إذا (targets[0].batpath[0] != NUL
		&& choices[idx + i].changefunc != عدم)
	{
	    choices[idx + i].active = 1;
	    إذا (choices[idx + i].changefunc == change_bat_choice
		    && targets[i].batpath[0] != NUL)
	    {
		strcpy(targets[i].batpath, targets[0].batpath);
		set_bat_text(idx + i, targets[i].batpath, targets[i].batname);
	    }
	}
	وإلا
	    choices[idx + i].active = 0;
    }
}

/*
 * Initialize a choice for creating a batch file.
 */
    ساكن فراغ
init_bat_choice(صحيح target)
{
    حرف	*batpath = targets[target].batpath;
    حرف	*oldbat = targets[target].oldbat;
    حرف	*p;
    صحيح		i;

    choices[choice_ژماردن].arg = target;
    choices[choice_ژماردن].installfunc = install_bat_choice;
    choices[choice_ژماردن].active = 1;
    choices[choice_ژماردن].text = عدم;	// will be set below
    إذا (oldbat != عدم)
    {
	// A [g]vim.bat exists: Only choice is to overwrite it or not.
	choices[choice_ژماردن].changefunc = toggle_bat_choice;
	*batpath = NUL;
	toggle_bat_choice(choice_ژماردن);
    }
    وإلا
    {
	إذا (default_bat_dir != عدم)
	    // Prefer using the same path as an existing .bat file.
	    strcpy(batpath, default_bat_dir);
	وإلا
	{
	    // No [g]vim.bat exists: Write it to a directory in $PATH.  Use
	    // $WINDIR by default, if it's empty the first item in $PATH.
	    p = getenv("WINDIR");
	    إذا (p != عدم && *p != NUL)
		strcpy(batpath, p);
	    وإلا
	    {
		p = getenv("PATH");
		إذا (p == عدم || *p == NUL)		// "cannot happen"
		    strcpy(batpath, "C:/Windows");
		وإلا
		{
		    i = 0;
		    طالما (*p != NUL && *p != ';')
			batpath[i++] = *p++;
		    batpath[i] = NUL;
		}
	    }
	}
	add_pathsep(batpath);
	set_bat_text(choice_ژماردن, batpath, targets[target].batname);

	choices[choice_ژماردن].changefunc = change_bat_choice;
    }
    ++choice_ژماردن;
}

/*
 * Set up the choices for installing .bat files.
 * For these items "arg" is the index in targets[].
 */
    ساكن فراغ
init_bat_choices(فراغ)
{
    صحيح		i;

    // The first item is used to switch installing batch files on/off and
    // setting the default path.
    choices[choice_ژماردن].text = bat_text_yes;
    choices[choice_ژماردن].changefunc = change_main_bat_choice;
    choices[choice_ژماردن].installfunc = عدم;
    choices[choice_ژماردن].active = 1;
    choices[choice_ژماردن].arg = 0;
    ++choice_ژماردن;

    // Add items for each batch file target.  Only used when not disabled by
    // the first item.  When a .exe exists, don't offer to create a .bat.
    لكل (i = 1; i < TARGET_ژماردن; ++i)
	إذا (targets[i].oldexe == عدم
		&& (targets[i].exenamearg[0] == 'g' ? has_gvim : has_vim))
	    init_bat_choice(i);
	وإلا
	    add_dummy_choice();
}

/*
 * Install the vimrc file.
 */
    ساكن فراغ
install_vimrc(صحيح idx UNUSED)
{
    FILE	*fd, *tfd;
    حرف	*fname;

    // If an old vimrc file exists, overwrite it.
    // Otherwise create a new one.
    إذا (*oldvimrc != NUL)
	fname = oldvimrc;
    وإلا
	fname = vimrc;

    fd = fopen(fname, "w");
    إذا (fd == عدم)
    {
	printf("\nERROR: Cannot open \"%s\" for writing.\n", fname);
	أرجع;
    }
    بدل (compat_choice)
    {
	حالة compat_vi:
		    fprintf(fd, "\" Vi compatible\n");
		    fprintf(fd, "set compatible\n");
		    اكسر;
	حالة compat_vim:
		    fprintf(fd, "\" Vim's default behavior\n");
		    fprintf(fd, "if &compatible\n");
		    fprintf(fd, "  set nocompatible\n");
		    fprintf(fd, "endif\n");
		    اكسر;
	حالة compat_some_enhancements:
		    fprintf(fd, "\" Vim with some enhancements\n");
		    fprintf(fd, "source $VIMRUNTIME/defaults.vim\n");
		    اكسر;
	حالة compat_all_enhancements:
		    fprintf(fd, "\" Vim with all enhancements\n");
		    fprintf(fd, "source $VIMRUNTIME/vimrc_example.vim\n");
		    اكسر;
    }
    بدل (remap_choice)
    {
	حالة remap_no:
		    اكسر;
	حالة remap_win:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Remap a few keys for Windows behavior\n");
		    fprintf(fd, "source $VIMRUNTIME/mswin.vim\n");
		    اكسر;
    }
    بدل (mouse_choice)
    {
	حالة mouse_xterm:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Mouse behavior (the Unix way)\n");
		    fprintf(fd, "behave xterm\n");
		    اكسر;
	حالة mouse_mswin:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Mouse behavior (the Windows way)\n");
		    fprintf(fd, "behave mswin\n");
		    اكسر;
	حالة mouse_default:
		    اكسر;
    }
    إذا ((tfd = fopen("diff.exe", "r")) != عدم)
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

    ساكن فراغ
change_vimrc_choice(صحيح idx)
{
    إذا (choices[idx].installfunc != عدم)
    {
	// Switch to NOT change or create a vimrc file.
	إذا (*oldvimrc != NUL)
	    alloc_text(idx, "Do NOT change startup file %s", oldvimrc);
	وإلا
	    alloc_text(idx, "Do NOT create startup file %s", vimrc);
	choices[idx].installfunc = عدم;
	choices[idx + 1].active = 0;
	choices[idx + 2].active = 0;
	choices[idx + 3].active = 0;
    }
    وإلا
    {
	// Switch to change or create a vimrc file.
	إذا (*oldvimrc != NUL)
	    alloc_text(idx, "Overwrite startup file %s with:", oldvimrc);
	وإلا
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
    ساكن فراغ
change_run_choice(صحيح idx)
{
    compat_choice = get_choice(compat_choices, TABLE_SIZE(compat_choices));
    alloc_text(idx, compat_text, compat_choices[compat_choice]);
}

/*
 * Change the choice if keys are to be remapped.
 */
    ساكن فراغ
change_remap_choice(صحيح idx)
{
    remap_choice = get_choice(remap_choices, TABLE_SIZE(remap_choices));
    alloc_text(idx, remap_text, remap_choices[remap_choice]);
}

/*
 * Change the choice how to select text.
 */
    ساكن فراغ
change_mouse_choice(صحيح idx)
{
    mouse_choice = get_choice(mouse_choices, TABLE_SIZE(mouse_choices));
    alloc_text(idx, mouse_text, mouse_choices[mouse_choice]);
}

    ساكن فراغ
init_vimrc_choices(فراغ)
{
    // set path for a new _vimrc file (also when not used)
    strcpy(vimrc, installdir);
    strcpy(vimrc + runtimeidx, "_vimrc");

    // Set opposite value and then toggle it by calling change_vimrc_choice()
    إذا (*oldvimrc == NUL)
	choices[choice_ژماردن].installfunc = عدم;
    وإلا
	choices[choice_ژماردن].installfunc = install_vimrc;
    choices[choice_ژماردن].text = عدم;
    change_vimrc_choice(choice_ژماردن);
    choices[choice_ژماردن].changefunc = change_vimrc_choice;
    choices[choice_ژماردن].active = 1;
    ++choice_ژماردن;

    // default way to run Vim
    alloc_text(choice_ژماردن, compat_text, compat_choices[compat_choice]);
    choices[choice_ژماردن].changefunc = change_run_choice;
    choices[choice_ژماردن].installfunc = عدم;
    choices[choice_ژماردن].active = (*oldvimrc == NUL);
    ++choice_ژماردن;

    // Whether to remap keys
    alloc_text(choice_ژماردن, remap_text , remap_choices[remap_choice]);
    choices[choice_ژماردن].changefunc = change_remap_choice;
    choices[choice_ژماردن].installfunc = عدم;
    choices[choice_ژماردن].active = (*oldvimrc == NUL);
    ++choice_ژماردن;

    // default way to use the mouse
    alloc_text(choice_ژماردن, mouse_text, mouse_choices[mouse_choice]);
    choices[choice_ژماردن].changefunc = change_mouse_choice;
    choices[choice_ژماردن].installfunc = عدم;
    choices[choice_ژماردن].active = (*oldvimrc == NUL);
    ++choice_ژماردن;
}

    ساكن LONG
reg_create_کلیل(
    HKEY ڕەگ,
    ثابت حرف *subkey,
    PHKEY ph_کلیل,
    DWORD flag)
{
    DWORD disp;

    *ph_کلیل = عدم;
    أرجع Reg_Create_کلیل_Ex(
		ڕەگ, subkey,
		0, عدم, REG_OPTION_NON_VOLATILE,
		flag | کلیل_WRITE,
		عدم, ph_کلیل, &disp);
}

    ساكن LONG
reg_set_string_بەها(
    HKEY h_کلیل,
    ثابت حرف *بەها_name,
    ثابت حرف *data)
{
    أرجع Reg_Set_بەها_Ex(h_کلیل, بەها_name, 0, REG_SZ,
				     (LPBYTE)data, (DWORD)(1 + strlen(data)));
}

    ساكن LONG
reg_create_کلیل_and_بەها(
    HKEY h_ڕەگ_کلیل,
    ثابت حرف *subkey,
    ثابت حرف *بەها_name,
    ثابت حرف *data,
    DWORD flag)
{
    HKEY h_کلیل;
    LONG lRet = reg_create_کلیل(h_ڕەگ_کلیل, subkey, &h_کلیل, flag);

    إذا (ERROR_SUCCESS == lRet)
    {
	lRet = reg_set_string_بەها(h_کلیل, بەها_name, data);
	Reg_Close_کلیل(h_کلیل);
    }
    أرجع lRet;
}

    ساكن LONG
register_inproc_server(
    HKEY h_ڕەگ_کلیل,
    ثابت حرف *clsid,
    ثابت حرف *extname,
    ثابت حرف *module,
    ثابت حرف *threading_model,
    DWORD flag)
{
    CHAR subkey[BUFSIZE];
    LONG lRet;

    sprintf(subkey, "CLSID\\%s", clsid);
    lRet = reg_create_کلیل_and_بەها(h_ڕەگ_کلیل, subkey, عدم, extname, flag);
    إذا (ERROR_SUCCESS == lRet)
    {
	sprintf(subkey, "CLSID\\%s\\InProcServer32", clsid);
	lRet = reg_create_کلیل_and_بەها(h_ڕەگ_کلیل, subkey, عدم, module, flag);
	إذا (ERROR_SUCCESS == lRet)
	{
	    lRet = reg_create_کلیل_and_بەها(h_ڕەگ_کلیل, subkey,
					   "ThreadingModel", threading_model, flag);
	}
    }
    أرجع lRet;
}

    ساكن LONG
register_shellex(
    HKEY h_ڕەگ_کلیل,
    ثابت حرف *clsid,
    ثابت حرف *name,
    ثابت حرف *exe_path,
    DWORD flag)
{
    LONG lRet = reg_create_کلیل_and_بەها(
	    h_ڕەگ_کلیل,
	    "*\\shellex\\ContextMenuHandlers\\gvim",
	    عدم,
	    clsid,
	    flag);

    إذا (ERROR_SUCCESS == lRet)
    {
	lRet = reg_create_کلیل_and_بەها(
		HKEY_LOCAL_MACHINE,
		"Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved",
		clsid,
		name,
		flag);

	إذا (ERROR_SUCCESS == lRet)
	{
	    lRet = reg_create_کلیل_and_بەها(
		    HKEY_LOCAL_MACHINE,
		    "Software\\Vim\\Gvim",
		    "path",
		    exe_path,
		    flag);
	}
    }
    أرجع lRet;
}

    ساكن LONG
register_openwith(
    HKEY h_ڕەگ_کلیل,
    ثابت حرف *exe_path,
    DWORD flag)
{
    حرف	exe_cmd[BUFSIZE];
    LONG	lRet;

    sprintf(exe_cmd, "\"%s\" \"%%1\"", exe_path);
    lRet = reg_create_کلیل_and_بەها(
	    h_ڕەگ_کلیل,
	    "Applications\\gvim.exe\\shell\\edit\\command",
	    عدم,
	    exe_cmd,
	    flag);

    إذا (ERROR_SUCCESS == lRet)
    {
	صحيح i;
	ساكن ثابت حرف *openwith[] = {
		".htm\\OpenWithList\\gvim.exe",
		".vim\\OpenWithList\\gvim.exe",
		"*\\OpenWithList\\gvim.exe",
	};

	لكل (i = 0; ERROR_SUCCESS == lRet && i < (صحيح)ARRAYSIZE(openwith); i++)
	    lRet = reg_create_کلیل_and_بەها(h_ڕەگ_کلیل, openwith[i], عدم, "", flag);
    }

    أرجع lRet;
}

    ساكن LONG
register_uninstall(
    HKEY h_ڕەگ_کلیل,
    ثابت حرف *appname,
    ثابت حرف *display_name,
    ثابت حرف *uninstall_string,
    ثابت حرف *display_icon,
    ثابت حرف *display_version,
    ثابت حرف *publisher)
{
    LONG lRet = reg_create_کلیل_and_بەها(h_ڕەگ_کلیل, appname,
			     "DisplayName", display_name, کلیل_WOW64_64_کلیل);

    إذا (ERROR_SUCCESS == lRet)
	lRet = reg_create_کلیل_and_بەها(h_ڕەگ_کلیل, appname,
		     "UninstallString", uninstall_string, کلیل_WOW64_64_کلیل);
    إذا (ERROR_SUCCESS == lRet)
	lRet = reg_create_کلیل_and_بەها(h_ڕەگ_کلیل, appname,
		     "DisplayIcon", display_icon, کلیل_WOW64_64_کلیل);
    إذا (ERROR_SUCCESS == lRet)
	lRet = reg_create_کلیل_and_بەها(h_ڕەگ_کلیل, appname,
		     "DisplayVersion", display_version, کلیل_WOW64_64_کلیل);
    إذا (ERROR_SUCCESS == lRet)
	lRet = reg_create_کلیل_and_بەها(h_ڕەگ_کلیل, appname,
		     "Publisher", publisher, کلیل_WOW64_64_کلیل);
    أرجع lRet;
}

/*
 * Add some entries to the registry:
 * - to add "Edit with Vim" to the context * menu
 * - to add Vim to the "Open with..." list
 * - to uninstall Vim
 */
//ARGSUSED
    ساكن صحيح
install_registry(فراغ)
{
    LONG	lRet = ERROR_SUCCESS;
    ثابت حرف	*vim_ext_ThreadingModel = "Apartment";
    ثابت حرف	*vim_ext_name = "Vim Shell Extension";
    ثابت حرف	*vim_ext_clsid = "{51EEE242-AD87-11d3-9C1E-0090278BBD99}";
    حرف	vim_exe_path[MAX_PATH];
    حرف	display_name[BUFSIZE];
    حرف	uninstall_string[BUFSIZE];
    حرف	icon_string[BUFSIZE];
    حرف	version_string[BUFSIZE];
    صحيح		i;
    صحيح		loop_ژماردن = is_64bit_os() ? 2 : 1;
    DWORD	flag;

    sprintf(vim_exe_path, "%s\\gvim.exe", installdir);

    إذا (install_popup)
    {
	حرف	    bufg[BUFSIZE];

	printf("Creating \"Edit with Vim\" popup menu entry\n");

	لكل (i = 0; i < loop_ژماردن; i++)
	{
	    إذا (i == 0)
	    {
		sprintf(bufg, "%s\\" GVIMEXT32_PATH, installdir);
		flag = کلیل_WOW64_32_کلیل;
	    }
	    وإلا
	    {
		sprintf(bufg, "%s\\" GVIMEXT64_PATH, installdir);
		flag = کلیل_WOW64_64_کلیل;
	    }

	    lRet = register_inproc_server(
		    HKEY_CLASSES_ڕەگ, vim_ext_clsid, vim_ext_name,
		    bufg, vim_ext_ThreadingModel, flag);
	    إذا (ERROR_SUCCESS != lRet)
		أرجع FAIL;
	    lRet = register_shellex(
		    HKEY_CLASSES_ڕەگ, vim_ext_clsid, vim_ext_name,
		    vim_exe_path, flag);
	    إذا (ERROR_SUCCESS != lRet)
		أرجع FAIL;
	}
    }

    إذا (install_openwith)
    {
	printf("Creating \"Open with ...\" list entry\n");

	لكل (i = 0; i < loop_ژماردن; i++)
	{
	    إذا (i == 0)
		flag = کلیل_WOW64_32_کلیل;
	    وإلا
		flag = کلیل_WOW64_64_کلیل;

	    lRet = register_openwith(HKEY_CLASSES_ڕەگ, vim_exe_path, flag);
	    إذا (ERROR_SUCCESS != lRet)
		أرجع FAIL;
	}
    }

    printf("Creating an uninstall entry\n");
    sprintf(display_name, "Vim " VIM_VERSION_SHORT
#ifdef _M_ARM64
	    " (arm64)"
#وإلاإذا _M_X64
	    " (x64)"
#endif
	    );

    // For the NSIS installer use the generated uninstaller.
    إذا (interactive)
	sprintf(uninstall_string, "%s\\uninstall.exe", installdir);
    وإلا
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
    إذا (ERROR_SUCCESS != lRet)
	أرجع FAIL;

    أرجع OK;
}

    ساكن فراغ
change_popup_choice(صحيح idx)
{
    إذا (install_popup == 0)
    {
	choices[idx].text = "Install an entry for Vim in the popup menu for the right\n    mouse button so that you can edit any file with Vim";
	install_popup = 1;
    }
    وإلا
    {
	choices[idx].text = "Do NOT install an entry for Vim in the popup menu for the\n    right mouse button to edit any file with Vim";
	install_popup = 0;
    }
}

/*
 * Only add the choice for the popup menu entry when gvim.exe was found and
 * both gvimext.dll and regedit.exe exist.
 */
    ساكن فراغ
init_popup_choice(فراغ)
{
    بنية stat	st;

    إذا (has_gvim
	    && (stat(GVIMEXT32_PATH, &st) >= 0
		|| stat(GVIMEXT64_PATH, &st) >= 0))
    {
	choices[choice_ژماردن].changefunc = change_popup_choice;
	choices[choice_ژماردن].installfunc = عدم;
	choices[choice_ژماردن].active = 1;
	change_popup_choice(choice_ژماردن);  // set the text
	++choice_ژماردن;
    }
    وإلا
	add_dummy_choice();
}

    ساكن فراغ
change_openwith_choice(صحيح idx)
{
    إذا (install_openwith == 0)
    {
	choices[idx].text = "Add Vim to the \"Open With...\" list in the popup menu for the right\n    mouse button so that you can edit any file with Vim";
	install_openwith = 1;
    }
    وإلا
    {
	choices[idx].text = "Do NOT add Vim to the \"Open With...\" list in the popup menu for the\n    right mouse button to edit any file with Vim";
	install_openwith = 0;
    }
}

/*
 * Only add the choice for the open-with menu entry when gvim.exe was found
 * and regedit.exe exist.
 */
    ساكن فراغ
init_openwith_choice(فراغ)
{
    إذا (has_gvim)
    {
	choices[choice_ژماردن].changefunc = change_openwith_choice;
	choices[choice_ژماردن].installfunc = عدم;
	choices[choice_ژماردن].active = 1;
	change_openwith_choice(choice_ژماردن);  // set the text
	++choice_ژماردن;
    }
    وإلا
	add_dummy_choice();
}

/*
 * Create a shell link.
 *
 * returns 0 on failure, non-zero on successful completion.
 *
 * NOTE:  Currently untested with mingw.
 */
    صحيح
create_shortcut(
	ثابت حرف *shortcut_name,
	ثابت حرف *iconfile_path,
	صحيح	    iconindex,
	ثابت حرف *shortcut_target,
	ثابت حرف *shortcut_args,
	ثابت حرف *workingdir
	)
{
    IShellLink	    *shelllink_ptr;
    HRESULT	    hres;
    IPersistFile	    *persistfile_ptr;

    // Initialize COM library
    hres = CoInitialize(عدم);
    إذا (!SUCCEEDED(hres))
    {
	printf("Error:  Could not open the COM library.  Not creating shortcut.\n");
	أرجع FAIL;
    }

    // Instantiate a COM object for the ShellLink, store a pointer to it
    // in shelllink_ptr.
    hres = CoCreateInstance(&CLSID_ShellLink,
			   عدم,
			   CLSCTX_INPROC_SERVER,
			   &IID_IShellLink,
			   (فراغ **) &shelllink_ptr);

    إذا (SUCCEEDED(hres)) // If the instantiation was successful...
    {
	// ...Then build a PersistFile interface for the ShellLink so we can
	// save it as a file after we build it.
	hres = shelllink_ptr->lpVtbl->QueryInterface(shelllink_ptr,
		&IID_IPersistFile, (فراغ **) &persistfile_ptr);

	إذا (SUCCEEDED(hres))
	{
	    wchar_t wsz[BUFSIZE];

	    // translate the (possibly) multibyte shortcut filename to windows
	    // Unicode so it can be used as a file name.
	    MultiByteToWideChar(CP_ACP, 0, shortcut_name, -1, wsz, حجم(wsz)/حجم(wsz[0]));

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
	وإلا
	{
	    printf("QueryInterface Error\n");
	    أرجع FAIL;
	}

	// Return the ShellLink object
	shelllink_ptr->lpVtbl->Release(shelllink_ptr);
    }
    وإلا
    {
	printf("CoCreateInstance Error - hres = %08x\n", (صحيح)hres);
	أرجع FAIL;
    }

    أرجع OK;
}

/*
 * Build a path to where we will put a specified link.
 *
 * Return 0 on error, non-zero on success
 */
   صحيح
build_link_name(
	حرف	   *link_path,
	ثابت حرف *link_name,
	ثابت حرف *shell_folder_name)
{
    حرف	shell_folder_path[MAX_PATH];

    إذا (get_shell_folder_path(shell_folder_path, shell_folder_name) == FAIL)
    {
	printf("An error occurred while attempting to find the path to %s.\n",
							   shell_folder_name);
	أرجع FAIL;
    }

    // Make sure the directory exists (create Start Menu\Programs\Vim).
    // Ignore errors if it already exists.
    vim_mkdir(shell_folder_path, 0755);

    // build the path to the shortcut and the path to gvim.exe
    sprintf(link_path, "%s\\%s.lnk", shell_folder_path, link_name);

    أرجع OK;
}

    ساكن صحيح
build_shortcut(
	ثابت حرف *name,	// Name of the shortcut
	ثابت حرف *exename,	// Name of the executable (e.g., vim.exe)
	ثابت حرف *args,
	ثابت حرف *shell_folder,
	ثابت حرف *workingdir)
{
    حرف	executable_path[BUFSIZE];
    حرف	link_name[BUFSIZE];

    sprintf(executable_path, "%s\\%s", installdir, exename);

    إذا (build_link_name(link_name, name, shell_folder) == FAIL)
    {
	printf("An error has occurred.  A shortcut to %s will not be created %s.\n",
		name,
		*shell_folder == 'd' ? "on the desktop" : "in the Start menu");
	أرجع FAIL;
    }

    // Create the shortcut:
    أرجع create_shortcut(link_name, executable_path, 0,
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
    ساكن فراغ
install_start_menu(صحيح idx UNUSED)
{
    need_uninstall_entry = 1;
    printf("Creating start menu\n");
    إذا (has_vim)
    {
	إذا (build_shortcut("Vim", "vim.exe", "",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    أرجع;
	إذا (build_shortcut("Vim Read-only", "vim.exe", "-R",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    أرجع;
	إذا (build_shortcut("Vim Diff", "vim.exe", "-d",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    أرجع;
    }
    إذا (has_gvim)
    {
	إذا (build_shortcut("gVim", "gvim.exe", "",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    أرجع;
	إذا (build_shortcut("gVim Easy", "gvim.exe", "-y",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    أرجع;
	إذا (build_shortcut("gVim Read-only", "gvim.exe", "-R",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    أرجع;
	إذا (build_shortcut("gVim Diff", "gvim.exe", "-d",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    أرجع;
    }
    إذا (build_shortcut("Uninstall",
		interactive ? "uninstall.exe" : "uninstall-gui.exe", "",
					   VIM_STARTMENU, installdir) == FAIL)
	أرجع;
    // For Windows NT the working dir of the vimtutor.bat must be right,
    // otherwise gvim.exe won't be found and using gvimbat doesn't work.
    إذا (build_shortcut("Vim tutor", "vimtutor.bat", "",
					   VIM_STARTMENU, installdir) == FAIL)
	أرجع;
    إذا (build_shortcut("Help", has_gvim ? "gvim.exe" : "vim.exe", "-c h",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	أرجع;
    {
	حرف	shell_folder_path[BUFSIZE];

	// Creating the URL shortcut works a bit differently...
	إذا (get_shell_folder_path(shell_folder_path, VIM_STARTMENU) == FAIL)
	{
	    printf("Finding the path of the Start menu failed\n");
	    أرجع ;
	}
	add_pathsep(shell_folder_path);
	strcat(shell_folder_path, "Vim Online.url");
	إذا (!WritePrivateProfileString("InternetShortcut", "URL",
				    "https://www.vim.org/", shell_folder_path))
	{
	    printf("Creating the Vim online URL failed\n");
	    أرجع;
	}
    }
}

    ساكن فراغ
toggle_startmenu_choice(صحيح idx)
{
    إذا (choices[idx].installfunc == عدم)
    {
	choices[idx].installfunc = install_start_menu;
	choices[idx].text = "Add Vim to the Start menu";
    }
    وإلا
    {
	choices[idx].installfunc = عدم;
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
    فراغ
install_shortcut_gvim(صحيح idx)
{
    // Create shortcut(s) on the desktop
    إذا (choices[idx].arg)
    {
	(فراغ)build_shortcut(icon_names[0], "gvim.exe",
						      "", "desktop", WORKDIR);
	need_uninstall_entry = 1;
    }
}

    فراغ
install_shortcut_evim(صحيح idx)
{
    إذا (choices[idx].arg)
    {
	(فراغ)build_shortcut(icon_names[1], "gvim.exe",
						    "-y", "desktop", WORKDIR);
	need_uninstall_entry = 1;
    }
}

    فراغ
install_shortcut_gview(صحيح idx)
{
    إذا (choices[idx].arg)
    {
	(فراغ)build_shortcut(icon_names[2], "gvim.exe",
						    "-R", "desktop", WORKDIR);
	need_uninstall_entry = 1;
    }
}

    فراغ
toggle_shortcut_choice(صحيح idx)
{
    حرف	*arg;

    إذا (choices[idx].installfunc == install_shortcut_gvim)
	arg = "gVim";
    وإلا إذا (choices[idx].installfunc == install_shortcut_evim)
	arg = "gVim Easy";
    وإلا
	arg = "gVim Read-only";
    إذا (choices[idx].arg)
    {
	choices[idx].arg = 0;
	alloc_text(idx, "Do NOT create a desktop icon for %s", arg);
    }
    وإلا
    {
	choices[idx].arg = 1;
	alloc_text(idx, "Create a desktop icon for %s", arg);
    }
}

    ساكن فراغ
init_startmenu_choice(فراغ)
{
    // Start menu
    choices[choice_ژماردن].changefunc = toggle_startmenu_choice;
    choices[choice_ژماردن].installfunc = عدم;
    choices[choice_ژماردن].active = 1;
    toggle_startmenu_choice(choice_ژماردن);	// set the text
    ++choice_ژماردن;
}

/*
 * Add the choice for the desktop shortcuts.
 */
    ساكن فراغ
init_shortcut_choices(فراغ)
{
    // Shortcut to gvim
    choices[choice_ژماردن].text = عدم;
    choices[choice_ژماردن].arg = 0;
    choices[choice_ژماردن].active = has_gvim;
    choices[choice_ژماردن].changefunc = toggle_shortcut_choice;
    choices[choice_ژماردن].installfunc = install_shortcut_gvim;
    toggle_shortcut_choice(choice_ژماردن);
    ++choice_ژماردن;

    // Shortcut to evim
    choices[choice_ژماردن].text = عدم;
    choices[choice_ژماردن].arg = 0;
    choices[choice_ژماردن].active = has_gvim;
    choices[choice_ژماردن].changefunc = toggle_shortcut_choice;
    choices[choice_ژماردن].installfunc = install_shortcut_evim;
    toggle_shortcut_choice(choice_ژماردن);
    ++choice_ژماردن;

    // Shortcut to gview
    choices[choice_ژماردن].text = عدم;
    choices[choice_ژماردن].arg = 0;
    choices[choice_ژماردن].active = has_gvim;
    choices[choice_ژماردن].changefunc = toggle_shortcut_choice;
    choices[choice_ژماردن].installfunc = install_shortcut_gview;
    toggle_shortcut_choice(choice_ژماردن);
    ++choice_ژماردن;
}

/*
 * Attempt to register OLE for Vim.
 */
   ساكن فراغ
install_OLE_register(فراغ)
{
    حرف register_command_string[BUFSIZE + 30];

    printf("\n--- Attempting to register Vim with OLE ---\n");
    printf("(There is no message whether this works or not.)\n");

    sprintf(register_command_string, "\"%s\\gvim.exe\" -silent -register", installdir);
    system(register_command_string);
}

/*
 * Remove the last part of directory "path[]" to get its parent, and put the
 * result in "to[]".
 */
    ساكن فراغ
dir_remove_last(ثابت حرف *path, حرف to[MAX_PATH])
{
    حرف c;
    long last_char_to_copy;
    long path_length = strlen(path);

    // skip the last character just in case it is a '\\'
    last_char_to_copy = path_length - 2;
    c = path[last_char_to_copy];

    طالما (c != '\\')
    {
	last_char_to_copy--;
	c = path[last_char_to_copy];
    }

    strncpy(to, path, (size_t)last_char_to_copy);
    to[last_char_to_copy] = NUL;
}

    ساكن فراغ
set_directories_text(صحيح idx)
{
    صحيح vimfiles_dir_choice = choices[idx].arg;

    إذا (vimfiles_dir_choice == (صحيح)vimfiles_dir_none)
	alloc_text(idx, "Do NOT create plugin directories%s", "");
    وإلا
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
ساكن حرف *homedir = عدم;

    فراغ
init_homedir(فراغ)
{
    حرف    *var;
    حرف    buf[MAX_PATH];

    إذا (homedir != عدم)
    {
	حرر(homedir);
	homedir = عدم;
    }

    var = getenv("HOME");

    /*
     * Typically, $HOME is not defined on Windows, unless the user has
     * specifically defined it for Vim's sake.  However, on Windows NT
     * platforms, $HOMEDRIVE and $HOMEPATH are automatically defined for
     * each user.  Try constructing $HOME from these.
     */
    إذا (var == عدم || *var == NUL)
    {
	حرف	*homedrive, *homepath;

	homedrive = getenv("HOMEDRIVE");
	homepath = getenv("HOMEPATH");
	إذا (homepath == عدم || *homepath == NUL)
	    homepath = "\\";
	إذا (homedrive != عدم
		   && strlen(homedrive) + strlen(homepath) < حجم(buf))
	{
	    إذا (snprintf(buf, حجم(buf), "%s%s", homedrive, homepath) > 0
		&& buf[0] != NUL)
		var = buf;
	}
    }

    إذا (var == عدم)
	var = getenv("USERPROFILE");

    /*
     * Weird but true: $HOME may contain an indirect reference to another
     * variable, esp. "%USERPROFILE%".  Happens when $USERPROFILE isn't set
     * when $HOME is being set.
     */
    إذا (var != عدم && *var == '%')
    {
	حرف	*p;
	حرف	*exp;

	p = strchr(var + 1, '%');
	إذا (p != عدم)
	{
	    strncpy(buf, var + 1, p - (var + 1));
	    buf[p - (var + 1)] = NUL;
	    exp = getenv(buf);
	    إذا (exp != عدم && *exp != NUL
				&& strlen(exp) + strlen(p) < حجم(buf))
	    {
		snprintf(buf, حجم(buf), "%s%s", exp, p + 1);
		var = buf;
	    }
	}
    }

    إذا (var != عدم && *var == NUL)	// empty is same as not set
	var = عدم;

    إذا (var == عدم)
	homedir = عدم;
    وإلا
	homedir = _strdup(var);
}

/*
 * Change the directory that the vim plugin directories will be created in:
 * $HOME, $VIM or nowhere.
 */
    ساكن فراغ
change_directories_choice(صحيح idx)
{
    صحيح	    choice_ژماردن = TABLE_SIZE(vimfiles_dir_choices);

    // Don't offer the $HOME choice if $HOME isn't set.
    إذا (homedir == عدم)
	--choice_ژماردن;
    choices[idx].arg = get_choice(vimfiles_dir_choices, choice_ژماردن);
    set_directories_text(idx);
}

/*
 * Create the plugin directories...
 */
//ARGSUSED
    ساكن فراغ
install_vimfilesdir(صحيح idx)
{
    صحيح i;
    صحيح vimfiles_dir_choice = choices[idx].arg;
    حرف *p;
    حرف vimdir_path[MAX_PATH];
    حرف vimfiles_path[MAX_PATH + 9];
    حرف tmp_dirname[BUFSIZE];

    // switch on the location that the user wants the plugin directories
    // built in
    بدل (vimfiles_dir_choice)
    {
	حالة vimfiles_dir_vim:
	{
	    // Go to the %VIM% directory - check env first, then go one dir
	    //	   below installdir if there is no %VIM% environment variable.
	    //	   The accuracy of $VIM is checked in inspect_system(), so we
	    //	   can be sure it is ok to use here.
	    p = getenv("VIM");
	    إذا (p == عدم) // No $VIM in path
		dir_remove_last(installdir, vimdir_path);
	    وإلا
		strcpy(vimdir_path, p);
	    اكسر;
	}
	حالة vimfiles_dir_home:
	{
	    // Find the $HOME directory.  Its existence was already checked.
	    p = homedir;
	    إذا (p == عدم)
	    {
		printf("Internal error: $HOME is NULL\n");
		p = "c:\\";
	    }
	    strcpy(vimdir_path, p);
	    اكسر;
	}
	حالة vimfiles_dir_none:
	{
	    // Do not create vim plugin directory.
	    أرجع;
	}
    }

    // Now, just create the directory.	If it already exists, it will fail
    // silently.
    sprintf(vimfiles_path, "%s\\vimfiles", vimdir_path);
    vim_mkdir(vimfiles_path, 0755);

    printf("Creating the following directories in \"%s\":\n", vimfiles_path);
    لكل (i = 0; i < TABLE_SIZE(vimfiles_subdirs); i++)
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
    ساكن فراغ
init_directories_choice(فراغ)
{
    بنية stat	st;
    حرف	tmp_dirname[BUFSIZE];
    حرف	*p;
    صحيح		vimfiles_dir_choice;

    choices[choice_ژماردن].text = alloc(150);
    choices[choice_ژماردن].changefunc = change_directories_choice;
    choices[choice_ژماردن].installfunc = install_vimfilesdir;
    choices[choice_ژماردن].active = 1;

    // Check if the "compiler" directory already exists.  That's a good
    // indication that the plugin directories were already created.
    p = getenv("HOME");
    إذا (p != عدم)
    {
	vimfiles_dir_choice = (صحيح)vimfiles_dir_home;
	sprintf(tmp_dirname, "%s\\vimfiles\\compiler", p);
	إذا (stat(tmp_dirname, &st) == 0)
	    vimfiles_dir_choice = (صحيح)vimfiles_dir_none;
    }
    وإلا
    {
	vimfiles_dir_choice = (صحيح)vimfiles_dir_vim;
	p = getenv("VIM");
	إذا (p == عدم)  // No $VIM in path, use the install dir.
	    dir_remove_last(installdir, tmp_dirname);
	وإلا
	    strcpy(tmp_dirname, p);
	strcat(tmp_dirname, "\\vimfiles\\compiler");
	إذا (stat(tmp_dirname, &st) == 0)
	    vimfiles_dir_choice = (صحيح)vimfiles_dir_none;
    }

    choices[choice_ژماردن].arg = vimfiles_dir_choice;
    set_directories_text(choice_ژماردن);
    ++choice_ژماردن;
}

/*
 * Setup the choices and the default values.
 */
    ساكن فراغ
setup_choices(فراغ)
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

    ساكن فراغ
print_cmd_هێڵ_help(فراغ)
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
    ساكن فراغ
command_هێڵ_setup_choices(صحيح argc, حرف **argv)
{
    صحيح i, j;

    لكل (i = 1; i < argc; i++)
    {
	إذا (strcmp(argv[i], "-create-batfiles") == 0)
	{
	    إذا (i + 1 == argc)
		استمر;
	    طالما (argv[i + 1][0] != '-' && i < argc)
	    {
		i++;
		لكل (j = 1; j < TARGET_ژماردن; ++j)
		    إذا ((targets[j].exenamearg[0] == 'g' ? has_gvim : has_vim)
			    && strcmp(argv[i], targets[j].name) == 0)
		    {
			init_bat_choice(j);
			اكسر;
		    }
		إذا (j == TARGET_ژماردن)
		    printf("%s is not a valid choice for -create-batfiles\n",
								     argv[i]);

		إذا (i + 1 == argc)
		    اكسر;
	    }
	}
	وإلا إذا (strcmp(argv[i], "-create-vimrc") == 0)
	{
	    // Setup default vimrc choices.  If there is already a _vimrc file,
	    // it will NOT be overwritten.
	    init_vimrc_choices();
	}
	وإلا إذا (strcmp(argv[i], "-vimrc-remap") == 0)
	{
	    إذا (i + 1 == argc)
		اكسر;
	    i++;
	    إذا (strcmp(argv[i], "no") == 0)
		remap_choice = remap_no;
	    وإلا إذا (strcmp(argv[i], "win") == 0)
		remap_choice = remap_win;
	}
	وإلا إذا (strcmp(argv[i], "-vimrc-behave") == 0)
	{
	    إذا (i + 1 == argc)
		اكسر;
	    i++;
	    إذا (strcmp(argv[i], "unix") == 0)
		mouse_choice = mouse_xterm;
	    وإلا إذا (strcmp(argv[i], "mswin") == 0)
		mouse_choice = mouse_mswin;
	    وإلا إذا (strcmp(argv[i], "default") == 0)
		mouse_choice = mouse_default;
	}
	وإلا إذا (strcmp(argv[i], "-vimrc-compat") == 0)
	{
	    إذا (i + 1 == argc)
		اكسر;
	    i++;
	    إذا (strcmp(argv[i], "vi") == 0)
		compat_choice = compat_vi;
	    وإلا إذا (strcmp(argv[i], "vim") == 0)
		compat_choice = compat_vim;
	    وإلا إذا (strcmp(argv[i], "defaults") == 0)
		compat_choice = compat_some_enhancements;
	    وإلا إذا (strcmp(argv[i], "all") == 0)
		compat_choice = compat_all_enhancements;
	}
	وإلا إذا (strcmp(argv[i], "-install-popup") == 0)
	{
	    init_popup_choice();
	}
	وإلا إذا (strcmp(argv[i], "-install-openwith") == 0)
	{
	    init_openwith_choice();
	}
	وإلا إذا (strcmp(argv[i], "-add-start-menu") == 0)
	{
	    init_startmenu_choice();
	}
	وإلا إذا (strcmp(argv[i], "-install-icons") == 0)
	{
	    init_shortcut_choices();
	}
	وإلا إذا (strcmp(argv[i], "-create-directories") == 0)
	{
	    صحيح vimfiles_dir_choice = (صحيح)vimfiles_dir_none;

	    init_directories_choice();
	    إذا (i + 1 < argc && argv[i + 1][0] != '-')
	    {
		i++;
		إذا (strcmp(argv[i], "vim") == 0)
		    vimfiles_dir_choice = (صحيح)vimfiles_dir_vim;
		وإلا إذا (strcmp(argv[i], "home") == 0)
		{
		    إذا (homedir == عدم)  // No $HOME in environment
			vimfiles_dir_choice = (صحيح)vimfiles_dir_none;
		    وإلا
			vimfiles_dir_choice = (صحيح)vimfiles_dir_home;
		}
		وإلا
		{
		    printf("Unknown argument for -create-directories: %s\n",
								     argv[i]);
		    print_cmd_هێڵ_help();
		}
	    }
	    وإلا // No choice specified, default to vim directory
		vimfiles_dir_choice = (صحيح)vimfiles_dir_vim;
	    choices[choice_ژماردن - 1].arg = vimfiles_dir_choice;
	}
	وإلا إذا (strcmp(argv[i], "-register-OLE") == 0)
	{
	    // This is always done when gvim is found
	}
	وإلا // Unknown switch
	{
	    printf("Got unknown argument argv[%d] = %s\n", i, argv[i]);
	    print_cmd_هێڵ_help();
	}
    }
}


/*
 * Show a few screens full of helpful information.
 */
    ساكن فراغ
show_help(فراغ)
{
    ساكن حرف *(کاڵاکان[]) =
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
عدم
    };
    صحيح		i;
    صحيح		c;

    rewind(stdin);
    printf("\n");
    لكل (i = 0; کاڵاکان[i] != عدم; ++i)
    {
	puts(کاڵاکان[i]);
	printf("Hit Enter to continue, b (back) or q (quit help): ");
	c = getchar();
	rewind(stdin);
	إذا (c == 'b' || c == 'B')
	{
	    إذا (i == 0)
		--i;
	    وإلا
		i -= 2;
	}
	إذا (c == 'q' || c == 'Q')
	    اكسر;
	printf("\n");
    }
}

/*
 * Install the choices.
 */
    ساكن فراغ
install(فراغ)
{
    صحيح		i;

    // Install the selected choices.
    لكل (i = 0; i < choice_ژماردن; ++i)
	إذا (choices[i].installfunc != عدم && choices[i].active)
	    (choices[i].installfunc)(i);

    // Add some entries to the registry, if needed.
    إذا (install_popup
	    || install_openwith
	    || (need_uninstall_entry && interactive)
	    || !interactive)
	install_registry();

    // Register gvim with OLE.
    إذا (has_gvim)
	install_OLE_register();
}

/*
 * request_choice
 */
    ساكن فراغ
request_choice(فراغ)
{
    صحيح		      i;

    printf("\n\nInstall will do for you:\n");
    لكل (i = 0; i < choice_ژماردن; ++i)
	إذا (choices[i].active)
	    printf("%2d  %s\n", i + 1, choices[i].text);
    printf("To change an item, enter its number\n\n");
    printf("Enter item number, h (help), d (do it) or q (quit): ");
}

    صحيح
main(صحيح argc, حرف **argv)
{
    صحيح		i;
    حرف	buf[BUFSIZE];

    /*
     * Run interactively if there are no command line arguments.
     */
    إذا (argc > 1)
	interactive = 0;
    وإلا
	interactive = 1;

    // Initialize this program.
    do_inits(argv);
    init_homedir();

    إذا (argc > 1 && strcmp(argv[1], "-uninstall-check") == 0)
    {
	// Only check for already installed Vims.  Used by NSIS installer.
	i = uninstall_check(1);

	// Find the value of $VIM, because NSIS isn't able to do this by
	// itself.
	get_vim_env();

	// When nothing found exit quietly.  If something found wait for
	// a little while, so that the user can read the messages.
	إذا (i && _isatty(1))
	    sleep(3);
	exit(0);
    }

    printf("This program sets up the installation of Vim "
						   VIM_VERSION_MEDIUM "\n\n");

    // Check if the user unpacked the archives properly.
    check_unpack();

    // Check for already installed Vims.
    إذا (interactive)
	uninstall_check(0);

    // Find out information about the system.
    inspect_system();

    إذا (interactive)
    {
	// Setup all the choices.
	setup_choices();

	// Let the user change choices and finally install (or quit).
	لكل (;;)
	{
	    request_choice();
	    rewind(stdin);
	    إذا (scanf("%99s", buf) == 1)
	    {
		إذا (isdigit((unsigned حرف)buf[0]))
		{
		    // Change a choice.
		    i = atoi(buf);
		    إذا (i > 0 && i <= choice_ژماردن && choices[i - 1].active)
			(choices[i - 1].changefunc)(i - 1);
		    وإلا
			printf("\nIllegal choice\n");
		}
		وإلا إذا (buf[0] == 'h' || buf[0] == 'H')
		{
		    // Help
		    show_help();
		}
		وإلا إذا (buf[0] == 'd' || buf[0] == 'D')
		{
		    // Install!
		    install();
		    printf("\nThat finishes the installation.  Happy Vimming!\n");
		    اكسر;
		}
		وإلا إذا (buf[0] == 'q' || buf[0] == 'Q')
		{
		    // Quit
		    printf("\nExiting without anything done\n");
		    اكسر;
		}
		وإلا
		    printf("\nIllegal choice\n");
	    }
	}
	printf("\n");
	myexit(0);
    }
    وإلا
    {
	/*
	 * Run non-interactive - setup according to the command line switches
	 */
	command_هێڵ_setup_choices(argc, argv);
	install();

	// Avoid that the user has to hit Enter, just wait a little bit to
	// allow reading the messages.
	sleep(2);
    }

    أرجع 0;
}
