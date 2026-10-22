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
#وصف_ڪريو DOSINST
#ضمن "dosinst.h"
#ضمن <io.h>

#وصف_ڪريو GVIMEXT64_رستو	    "GvimExt64\\gvimext.dll"
#وصف_ڪريو GVIMEXT32_رستو	    "GvimExt32\\gvimext.dll"

// Macro to do an error check I was typing over and over
#وصف_ڪريو CHECK_REG_ERROR(ڪوڊ) \
    افعل { \
	إذا (ڪوڊ != ERROR_SUCCESS) \
	{ \
	    لکو("%ld error number:  %ld\n", (ڊگهو)__LINE__, (ڊگهو)ڪوڊ); \
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
    صحيح	    سرگرم;			// non-zero when choice is active
    حرف    *متن;			// text displayed for this choice
    فراغ    (*changefunc)(صحيح idx);	// function to change this choice
    صحيح	    arg;			// argument for function
    فراغ    (*installfunc)(صحيح idx);	// function to install this choice
};

بنية choice	choices[30];		// choices the user can make
صحيح		choice_ڳڻپ = 0;	// number of choices available

#وصف_ڪريو جدول_سائيز(s)	(صحيح)ARRAYSIZE(s)

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
حرف	*compat_متن = "- run Vim %s";

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
حرف	*remap_متن = "- %s";

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
حرف	*mouse_متن = "- The mouse %s";

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
ساكن صحيح	need_uninstall_داخلا = 0;

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
get_choice(حرف **جدول, صحيح entries)
{
    صحيح		answer;
    صحيح		idx;
    حرف	dummy[100];

    افعل
    {
	لكل (idx = 0; idx < entries; ++idx)
	{
	    إذا (idx)
		لکو("%2d  ", idx);
	    puts(جدول[idx]);
	}
	لکو("Choice: ");
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
    runtimeidx = لڙي_ڊيگهه(installdir) - لڙي_ڊيگهه(VIM_نسخو_NODOT);
    إذا (runtimeidx <= 0
	    || stricmp(installdir + runtimeidx, VIM_نسخو_NODOT) != 0
	    || (installdir[runtimeidx - 1] != '/'
		&& installdir[runtimeidx - 1] != '\\'))
    {
	لکو("ERROR: Install program not in directory \"%s\"\n",
		VIM_نسخو_NODOT);
	لکو("This program can only work when it is located in its original directory\n");
	myexit(1);
    }

    // check if filetype.vim is present, which means the runtime archive has
    // been unpacked
    sprintf(buf, "%s\\filetype.vim", installdir);
    إذا (stat(buf, &st) < 0)
    {
	لکو("ERROR: Cannot find filetype.vim in \"%s\"\n", installdir);
	لکو("It looks like you did not unpack the runtime archive.\n");
	لکو("You must unpack the runtime archive \"%srt.zip\" before installing.\n",
		VIM_نسخو_NODOT);
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
	لکو("ERROR: Cannot find any Vim executables in \"%s\"\n\n",
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
	plen = لڙي_ڊيگهه(p);
    إذا (qlen < 0)
	qlen = لڙي_ڊيگهه(q);
    لكل (i = 0; ; ++i)
    {
	// End of "p": check if "q" also ends or just has a slash.
	إذا (i == plen)
	{
	    إذا (i == qlen)  //ميلاپ
		أرجع 0;
	    إذا (i == qlen - 1 && (q[i] == '\\' || q[i] == '/'))
		أرجع 0;   // match with trailing slash
	    أرجع 1;	    // no match
	}

	// End of "q": check if "p" also ends or just has a slash.
	إذا (i == qlen)
	{
	    إذا (i == plen)  //ميلاپ
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
    size_t	indir_l = لڙي_ڊيگهه(installdir);
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

    tmpname = alloc(لڙي_ڊيگهه(cp) + 1);
    strcpy(tmpname, cp);
    tmpname[لڙي_ڊيگهه(tmpname) - 1] = 'x';	// .exe -> .exx

    إذا (access(tmpname, 0) == 0)
    {
	لکو("\nERROR: %s and %s clash.  Remove or rename %s.\n",
	    tmpname, cp, tmpname);
	myexit(1);
    }

    إذا (rename(cp, tmpname) != 0)
    {
	لکو("\nERROR: failed to rename %s to %s: %s\n",
	    cp, tmpname, strerror(0));
	myexit(1);
    }

    farname = searchpath_save(cp);

    إذا (rename(tmpname, cp) != 0)
    {
	لکو("\nERROR: failed to rename %s back to %s: %s\n",
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
ڳوليو_bat_exe(صحيح check_bat_only)
{
    صحيح		i;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    لكل (i = 1; i < هدف_ڳڻپ; ++i)
    {
	targets[i].oldbat = searchpath_save(targets[i].batname);
	إذا (!check_bat_only)
	    targets[i].oldexe = searchpath_save(targets[i].exename);

	إذا (default_bat_dir == عدم && targets[i].oldbat != عدم)
	{
	    default_bat_dir = alloc(لڙي_ڊيگهه(targets[i].oldbat) + 1);
	    strcpy(default_bat_dir, targets[i].oldbat);
	    remove_پڇ(default_bat_dir);
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
    إذا (vim != عدم && *vim != 0 && لڙي_ڊيگهه(vim) < حجم(buf))
    {
	strcpy(buf, vim);
	remove_پڇ(buf);
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
    Get_عارضي_رستو(حجم(fname) - 12, fname);
    جوڙيو_pathsep(fname);
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
	لکو("Failed to open %s\n", fname);
	sleep(2);
    }
}

ساكن صحيح انگ_windows;

/*
 * Callback used for EnumWindows():
 * Count the window if the title looks like it is for the uninstaller.
 */
//ARGSUSED
    ساكن BOOL CALLBACK
دري_cb(HWND hwnd, LPARAM lparam اڻ_استعمال_ٿيل)
{
    حرف title[256];

    title[0] = 0;
    Get_دري_متن(hwnd, title, 256);
    إذا (لڙي_ڳولا(title, "Vim ") != عدم && لڙي_ڳولا(title, " Uninstall") != عدم)
	++انگ_windows;
    أرجع TRUE;
}

/*
 * Run the uninstaller silently.
 */
    ساكن صحيح
لاڳيتو_silent_uninstall(حرف *uninst_exe)
{
    حرف    vimrt_dir[BUFSIZE];
    حرف    عارضي_uninst[BUFSIZE];
    حرف    عارضي_dir[MAX_رستو];
    حرف    buf[BUFSIZE * 2 + 10];
    صحيح	    i;
    DWORD   tick;

    strcpy(vimrt_dir, uninst_exe);
    remove_پڇ(vimrt_dir);

    إذا (!Get_عارضي_رستو(حجم(عارضي_dir), عارضي_dir))
	أرجع FAIL;

    // Copy the uninstaller to a temporary exe.
    tick = Get_Tick_ڳڻپ();
    لكل (i = 0; ; i++)
    {
	sprintf(عارضي_uninst, "%s\\vimun%04X.exe", عارضي_dir,
					  (بنانشان صحيح)((i + tick) & 0xFFFF));
	إذا (CopyFile(uninst_exe, عارضي_uninst, TRUE))
	    اكسر;
	إذا (Get_آخري_Error() != ERROR_FILE_EXISTS)
	    أرجع FAIL;
	إذا (i == 65535)
	    أرجع FAIL;
    }

    // Run the copied uninstaller silently.
    إذا (strchr(عارضي_uninst, ' ') != عدم)
	sprintf(buf, "\"%s\" /S _?=%s", عارضي_uninst, vimrt_dir);
    وإلا
	sprintf(buf, "%s /S _?=%s", عارضي_uninst, vimrt_dir);
    لاڳيتو_command(buf);

    DeleteFile(عارضي_uninst);
    أرجع OK;
}

/*
 * Check for already installed Vims.
 * Return non-zero when found one.
 */
    ساكن صحيح
uninstall_check(صحيح skip_question)
{
    HKEY	ڪنجي_handle;
    HKEY	uninstall_ڪنجي_handle;
    حرف	*uninstall_ڪنجي = "software\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
    حرف	subkey_name_buff[BUFSIZE];
    حرف	عارضي_string_بفر[BUFSIZE-2];
    DWORD	local_bufsize;
    FILETIME	عارضي_pfiletime;
    DWORD	ڪنجي_اشاريو;
    حرف	داخلا_2;
    ڊگهو	ڪوڊ;
    DWORD	قدر_type;
    DWORD	orig_انگ_ڪنجيون;
    DWORD	new_انگ_ڪنجيون;
    DWORD	allow_silent;
    صحيح		foundone = 0;

    ڪوڊ = Reg_Open_ڪنجي_Ex(HKEY_LOCAL_MACHINE, uninstall_ڪنجي, 0,
				     ڪنجي_WOW64_64_ڪنجي | ڪنجي_READ, &ڪنجي_handle);
    CHECK_REG_ERROR(ڪوڊ);

    ڪنجي_اشاريو = 0;
    طالما (TRUE)
    {
	local_bufsize = حجم(subkey_name_buff);
	إذا (Reg_Enum_ڪنجي_Ex(ڪنجي_handle, ڪنجي_اشاريو, subkey_name_buff, &local_bufsize,
		عدم, عدم, عدم, &عارضي_pfiletime) == ERROR_NO_وڌيڪ_شيون)
	    اكسر;

	إذا (لڙي_ڀيٽ("Vim", subkey_name_buff, 3) == 0)
	{
	    // Open the key named Vim*
	    ڪوڊ = Reg_Open_ڪنجي_Ex(ڪنجي_handle, subkey_name_buff, 0,
			   ڪنجي_WOW64_64_ڪنجي | ڪنجي_READ, &uninstall_ڪنجي_handle);
	    CHECK_REG_ERROR(ڪوڊ);

	    // get the DisplayName out of it to show the user
	    local_bufsize = حجم(عارضي_string_بفر);
	    ڪوڊ = Reg_Query_قدر_Ex(uninstall_ڪنجي_handle, "displayname", 0,
		    &قدر_type, (LPBYTE)عارضي_string_بفر,
		    &local_bufsize);
	    CHECK_REG_ERROR(ڪوڊ);

	    allow_silent = 0;
	    إذا (skip_question)
	    {
		DWORD varsize = حجم(DWORD);

		Reg_Query_قدر_Ex(uninstall_ڪنجي_handle, "AllowSilent", 0,
			&قدر_type, (LPBYTE)&allow_silent,
			&varsize);
	    }

	    foundone = 1;
	    لکو("\n*********************************************************\n");
	    لکو("Vim Install found what looks like an existing Vim version.\n");
	    لکو("The name of the entry is:\n");
	    لکو("\n        \"%s\"\n\n", عارضي_string_بفر);

	    لکو("Installing the new version will disable part of the existing version.\n");
	    لکو("(The batch files used in a console and the \"Edit with Vim\" entry in\n");
	    لکو("the popup menu will use the new version)\n");

	    إذا (skip_question)
		لکو("\nRunning uninstall program for \"%s\"\n", عارضي_string_بفر);
	    وإلا
		لکو("\nDo you want to uninstall \"%s\" now?\n(y)es/(n)o)  ", عارضي_string_بفر);
	    fflush(stdout);

	    // get the UninstallString
	    local_bufsize = حجم(عارضي_string_بفر);
	    ڪوڊ = Reg_Query_قدر_Ex(uninstall_ڪنجي_handle, "uninstallstring", 0,
		    &قدر_type, (LPBYTE)عارضي_string_بفر, &local_bufsize);
	    CHECK_REG_ERROR(ڪوڊ);

	    // Remember the directory, it is used as the default for NSIS.
	    default_vim_dir = alloc(لڙي_ڊيگهه(عارضي_string_بفر) + 1);
	    strcpy(default_vim_dir, عارضي_string_بفر);
	    remove_پڇ(default_vim_dir);
	    remove_پڇ(default_vim_dir);

	    داخلا_2 = 'n';
	    افعل
	    {
		إذا (داخلا_2 != 'n')
		    لکو("%c is an invalid reply.  Please enter either 'y' or 'n'\n", داخلا_2);

		إذا (skip_question)
		    داخلا_2 = 'y';
		وإلا
		{
		    rewind(stdin);
		    scanf("%c", &داخلا_2);
		}
		بدل (داخلا_2)
		{
		    حالة 'y':
		    حالة 'Y':
			// save the number of uninstall keys so we can know if
			// it changed
			Reg_Query_Info_ڪنجي(ڪنجي_handle, عدم, عدم, عدم,
					     &orig_انگ_ڪنجيون, عدم, عدم, عدم,
						      عدم, عدم, عدم, عدم);

			// Find existing .bat files before deleting them.
			ڳوليو_bat_exe(TRUE);

			إذا (allow_silent)
			{
			    إذا (لاڳيتو_silent_uninstall(عارضي_string_بفر)
								    == FAIL)
				allow_silent = 0; // Retry with non silent.
			}
			إذا (!allow_silent)
			{
			    // Execute the uninstall program.  Put it in double
			    // quotes if there is an embedded space.
			    {
				حرف buf[BUFSIZE];

				إذا (strchr(عارضي_string_بفر, ' ') != عدم)
				    sprintf(buf, "\"%s\"", عارضي_string_بفر);
				وإلا
				    strcpy(buf, عارضي_string_بفر);
				لاڳيتو_command(buf);
			    }

			    // Count the number of windows with a title that
			    // match the installer, so that we can check when
			    // it's done.  The uninstaller copies itself,
			    // executes the copy and exits, thus we can't wait
			    // for the process to finish.
			    sleep(1);  // wait for uninstaller to start up
			    انگ_windows = 0;
			    EnumWindows(دري_cb, 0);
			    إذا (انگ_windows == 0)
			    {
				// Did not find the uninstaller, ask user to
				// press Enter when done. Just in case.
				لکو("Press Enter when the uninstaller is finished\n");
				rewind(stdin);
				(فراغ)getchar();
			    }
			    وإلا
			    {
				لکو("Waiting for the uninstaller to finish (press CTRL-C to abort).");
				افعل
				{
				    لکو(".");
				    fflush(stdout);
				    sleep(1);	// wait for the uninstaller to
						//پڄاڻي
				    انگ_windows = 0;
				    EnumWindows(دري_cb, 0);
				} طالما (انگ_windows > 0);
			    }
			}
			لکو("\nDone!\n");

			// Check if an uninstall reg key was deleted.
			// if it was, we want to decrement key_index.
			// if we don't do this, we will skip the key
			// immediately after any key that we delete.
			Reg_Query_Info_ڪنجي(ڪنجي_handle, عدم, عدم, عدم,
					      &new_انگ_ڪنجيون, عدم, عدم, عدم,
						      عدم, عدم, عدم, عدم);
			إذا (new_انگ_ڪنجيون < orig_انگ_ڪنجيون)
			    ڪنجي_اشاريو--;

			داخلا_2 = 'y';
			اكسر;

		    حالة 'n':
		    حالة 'N':
			// Do not uninstall
			داخلا_2 = 'n';
			اكسر;

		    افتراضي: // just drop through and redo the loop
			اكسر;
		}

	    } طالما (داخلا_2 != 'n' && داخلا_2 != 'y');

	    Reg_Close_ڪنجي(uninstall_ڪنجي_handle);
	}

	ڪنجي_اشاريو++;
    }
    Reg_Close_ڪنجي(ڪنجي_handle);

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
    لکو("Inspecting system...\n");

    /*
     * If $VIM is set, check that it's pointing to our directory.
     */
    p = getenv("VIM");
    إذا (p != عدم && pathcmp(p, -1, installdir, runtimeidx - 1) != 0)
    {
	لکو("------------------------------------------------------\n");
	لکو("$VIM is set to \"%s\".\n", p);
	لکو("This is different from where this version of Vim is:\n");
	strcpy(buf, installdir);
	*(buf + runtimeidx - 1) = NUL;
	لکو("\"%s\"\n", buf);
	لکو("You must adjust or remove the setting of $VIM,\n");
	إذا (interactive)
	{
	    لکو("to be able to use this install program.\n");
	    myexit(1);
	}
	لکو("otherwise Vim WILL NOT WORK properly!\n");
	لکو("------------------------------------------------------\n");
    }

    /*
     * If $VIMRUNTIME is set, check that it's pointing to our runtime directory.
     */
    p = getenv("VIMRUNTIME");
    إذا (p != عدم && pathcmp(p, -1, installdir, -1) != 0)
    {
	لکو("------------------------------------------------------\n");
	لکو("$VIMRUNTIME is set to \"%s\".\n", p);
	لکو("This is different from where this version of Vim is:\n");
	لکو("\"%s\"\n", installdir);
	لکو("You must adjust or remove the setting of $VIMRUNTIME,\n");
	إذا (interactive)
	{
	    لکو("to be able to use this install program.\n");
	    myexit(1);
	}
	لکو("otherwise Vim WILL NOT WORK properly!\n");
	لکو("------------------------------------------------------\n");
    }

    /*
     * Check if there is a vim.[exe|bat|, gvim.[exe|bat|, etc. in the path.
     */
    ڳوليو_bat_exe(FALSE);

    /*
     * A .exe in the install directory may be found anyway on Windows 2000.
     * Check for this situation and find another executable if necessary.
     * w.briscoe@ponl.com 2001-01-20
     */
    foundone = 0;
    لكل (i = 1; i < هدف_ڳڻپ; ++i)
    {
	findoldfile(&(targets[i].oldexe));
	إذا (targets[i].oldexe != عدم)
	    foundone = 1;
    }

    إذا (foundone)
    {
	لکو("Warning: Found Vim executable(s) in your $PATH:\n");
	لكل (i = 1; i < هدف_ڳڻپ; ++i)
	    إذا (targets[i].oldexe != عدم)
		لکو("%s\n", targets[i].oldexe);
	لکو("It will be used instead of the version you are installing.\n");
	لکو("Please delete or rename it, or adjust your $PATH setting.\n");
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
جوڙيو_dummy_choice(فراغ)
{
    choices[choice_ڳڻپ].installfunc = عدم;
    choices[choice_ڳڻپ].سرگرم = 0;
    choices[choice_ڳڻپ].changefunc = عدم;
    choices[choice_ڳڻپ].متن = عدم;
    choices[choice_ڳڻپ].arg = 0;
    ++choice_ڳڻپ;
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
	لکو("\nERROR: Cannot open \"%s\" for writing.\n", batpath);
	أرجع;
    }

    need_uninstall_داخلا = 1;

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
	    VIM_نسخو_NODOT, exename, VIM_نسخو_NODOT);
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
    لکو("%s has been %s\n", batpath,
	    oldname == عدم ? "created" : "overwritten");
}

/*
 * Make the text string for choice "idx".
 * The format "fmt" is must have one %s item, which "arg" is used for.
 */
    ساكن فراغ
alloc_متن(صحيح idx, حرف *fmt, حرف *arg)
{
    إذا (choices[idx].متن != عدم)
	حرر(choices[idx].متن);

    choices[idx].متن = alloc(لڙي_ڊيگهه(fmt) + لڙي_ڊيگهه(arg) - 1);
    sprintf(choices[idx].متن, fmt, arg);
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
	alloc_متن(idx, "    Overwrite %s", oldname);
	strcpy(batname, oldname);
    }
    وإلا
    {
	alloc_متن(idx, "    Do NOT overwrite %s", oldname);
	*batname = NUL;
    }
}

/*
 * Do some work for a batch file entry: Append the batch file name to the path
 * and set the text for the choice.
 */
    ساكن فراغ
سيٽ_bat_متن(صحيح idx, حرف *batpath, حرف *name)
{
    strcat(batpath, name);

    alloc_متن(idx, "    Create %s", batpath);
}

/*
 * Select a directory to write the batch file line.
 */
    ساكن فراغ
change_bat_choice(صحيح idx)
{
    حرف	*رستو;
    حرف	*batpath;
    حرف	*name;
    صحيح		n;
    حرف	*s;
    حرف	*p;
    صحيح		ڳڻپ;
    حرف	**نالا = عدم;
    صحيح		i;
    صحيح		هدف = choices[idx].arg;

    name = targets[هدف].batname;
    batpath = targets[هدف].batpath;

    رستو = getenv("PATH");
    إذا (رستو == عدم)
    {
	لکو("\nERROR: The variable $PATH is not set\n");
	أرجع;
    }

    /*
     * first round: count number of names in path;
     * second round: save names to names[].
     */
    لكل (;;)
    {
	ڳڻپ = 1;
	لكل (p = رستو; *p; )
	{
	    s = strchr(p, ';');
	    إذا (s == عدم)
		s = p + لڙي_ڊيگهه(p);
	    إذا (نالا != عدم)
	    {
		نالا[ڳڻپ] = alloc(s - p + 1);
		strncpy(نالا[ڳڻپ], p, s - p);
		نالا[ڳڻپ][s - p] = NUL;
	    }
	    ++ڳڻپ;
	    p = s;
	    إذا (*p != NUL)
		++p;
	}
	إذا (نالا != عدم)
	    اكسر;
	نالا = alloc((ڳڻپ + 1) * حجم(حرف *));
    }
    نالا[0] = alloc(50);
    sprintf(نالا[0], "Select directory to create %s in:", name);
    نالا[ڳڻپ] = alloc(50);
    إذا (choices[idx].arg == 0)
	sprintf(نالا[ڳڻپ], "Do not create any .bat file.");
    وإلا
	sprintf(نالا[ڳڻپ], "Do not create a %s file.", name);
    n = get_choice(نالا, ڳڻپ + 1);

    إذا (n == ڳڻپ)
    {
	// Selected last item, don't create bat file.
	*batpath = NUL;
	إذا (choices[idx].arg != 0)
	    alloc_متن(idx, "    Do NOT create %s", name);
    }
    وإلا
    {
	// Selected one of the paths.  For the first item only keep the path,
	// for the others append the batch file name.
	strcpy(batpath, نالا[n]);
	جوڙيو_pathsep(batpath);
	إذا (choices[idx].arg != 0)
	    سيٽ_bat_متن(idx, batpath, name);
    }

    لكل (i = 0; i <= ڳڻپ; ++i)
	حرر(نالا[i]);
    حرر(نالا);
}

حرف *bat_متن_yes = "Install .bat files to use Vim at the command line:";
حرف *bat_متن_no = "do NOT install .bat files to use Vim at the command line";

    ساكن فراغ
change_main_bat_choice(صحيح idx)
{
    صحيح		i;

    // let the user select a default directory or NONE
    change_bat_choice(idx);

    إذا (targets[0].batpath[0] != NUL)
	choices[idx].متن = bat_متن_yes;
    وإلا
	choices[idx].متن = bat_متن_no;

    // update the individual batch file selections
    لكل (i = 1; i < هدف_ڳڻپ; ++i)
    {
	// Only make it active when the first item has a path and the vim.exe
	// or gvim.exe exists (there is a changefunc then).
	إذا (targets[0].batpath[0] != NUL
		&& choices[idx + i].changefunc != عدم)
	{
	    choices[idx + i].سرگرم = 1;
	    إذا (choices[idx + i].changefunc == change_bat_choice
		    && targets[i].batpath[0] != NUL)
	    {
		strcpy(targets[i].batpath, targets[0].batpath);
		سيٽ_bat_متن(idx + i, targets[i].batpath, targets[i].batname);
	    }
	}
	وإلا
	    choices[idx + i].سرگرم = 0;
    }
}

/*
 * Initialize a choice for creating a batch file.
 */
    ساكن فراغ
init_bat_choice(صحيح هدف)
{
    حرف	*batpath = targets[هدف].batpath;
    حرف	*oldbat = targets[هدف].oldbat;
    حرف	*p;
    صحيح		i;

    choices[choice_ڳڻپ].arg = هدف;
    choices[choice_ڳڻپ].installfunc = install_bat_choice;
    choices[choice_ڳڻپ].سرگرم = 1;
    choices[choice_ڳڻپ].متن = عدم;	// will be set below
    إذا (oldbat != عدم)
    {
	// A [g]vim.bat exists: Only choice is to overwrite it or not.
	choices[choice_ڳڻپ].changefunc = toggle_bat_choice;
	*batpath = NUL;
	toggle_bat_choice(choice_ڳڻپ);
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
	جوڙيو_pathsep(batpath);
	سيٽ_bat_متن(choice_ڳڻپ, batpath, targets[هدف].batname);

	choices[choice_ڳڻپ].changefunc = change_bat_choice;
    }
    ++choice_ڳڻپ;
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
    choices[choice_ڳڻپ].متن = bat_متن_yes;
    choices[choice_ڳڻپ].changefunc = change_main_bat_choice;
    choices[choice_ڳڻپ].installfunc = عدم;
    choices[choice_ڳڻپ].سرگرم = 1;
    choices[choice_ڳڻپ].arg = 0;
    ++choice_ڳڻپ;

    // Add items for each batch file target.  Only used when not disabled by
    // the first item.  When a .exe exists, don't offer to create a .bat.
    لكل (i = 1; i < هدف_ڳڻپ; ++i)
	إذا (targets[i].oldexe == عدم
		&& (targets[i].exenamearg[0] == 'g' ? has_gvim : has_vim))
	    init_bat_choice(i);
	وإلا
	    جوڙيو_dummy_choice();
}

/*
 * Install the vimrc file.
 */
    ساكن فراغ
install_vimrc(صحيح idx اڻ_استعمال_ٿيل)
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
	لکو("\nERROR: Cannot open \"%s\" for writing.\n", fname);
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
    لکو("%s has been written\n", fname);
}

    ساكن فراغ
change_vimrc_choice(صحيح idx)
{
    إذا (choices[idx].installfunc != عدم)
    {
	// Switch to NOT change or create a vimrc file.
	إذا (*oldvimrc != NUL)
	    alloc_متن(idx, "Do NOT change startup file %s", oldvimrc);
	وإلا
	    alloc_متن(idx, "Do NOT create startup file %s", vimrc);
	choices[idx].installfunc = عدم;
	choices[idx + 1].سرگرم = 0;
	choices[idx + 2].سرگرم = 0;
	choices[idx + 3].سرگرم = 0;
    }
    وإلا
    {
	// Switch to change or create a vimrc file.
	إذا (*oldvimrc != NUL)
	    alloc_متن(idx, "Overwrite startup file %s with:", oldvimrc);
	وإلا
	    alloc_متن(idx, "Create startup file %s with:", vimrc);
	choices[idx].installfunc = install_vimrc;
	choices[idx + 1].سرگرم = 1;
	choices[idx + 2].سرگرم = 1;
	choices[idx + 3].سرگرم = 1;
    }
}

/*
 * Change the choice how to run Vim.
 */
    ساكن فراغ
change_لاڳيتو_choice(صحيح idx)
{
    compat_choice = get_choice(compat_choices, جدول_سائيز(compat_choices));
    alloc_متن(idx, compat_متن, compat_choices[compat_choice]);
}

/*
 * Change the choice if keys are to be remapped.
 */
    ساكن فراغ
change_remap_choice(صحيح idx)
{
    remap_choice = get_choice(remap_choices, جدول_سائيز(remap_choices));
    alloc_متن(idx, remap_متن, remap_choices[remap_choice]);
}

/*
 * Change the choice how to select text.
 */
    ساكن فراغ
change_mouse_choice(صحيح idx)
{
    mouse_choice = get_choice(mouse_choices, جدول_سائيز(mouse_choices));
    alloc_متن(idx, mouse_متن, mouse_choices[mouse_choice]);
}

    ساكن فراغ
init_vimrc_choices(فراغ)
{
    // set path for a new _vimrc file (also when not used)
    strcpy(vimrc, installdir);
    strcpy(vimrc + runtimeidx, "_vimrc");

    // Set opposite value and then toggle it by calling change_vimrc_choice()
    إذا (*oldvimrc == NUL)
	choices[choice_ڳڻپ].installfunc = عدم;
    وإلا
	choices[choice_ڳڻپ].installfunc = install_vimrc;
    choices[choice_ڳڻپ].متن = عدم;
    change_vimrc_choice(choice_ڳڻپ);
    choices[choice_ڳڻپ].changefunc = change_vimrc_choice;
    choices[choice_ڳڻپ].سرگرم = 1;
    ++choice_ڳڻپ;

    // default way to run Vim
    alloc_متن(choice_ڳڻپ, compat_متن, compat_choices[compat_choice]);
    choices[choice_ڳڻپ].changefunc = change_لاڳيتو_choice;
    choices[choice_ڳڻپ].installfunc = عدم;
    choices[choice_ڳڻپ].سرگرم = (*oldvimrc == NUL);
    ++choice_ڳڻپ;

    // Whether to remap keys
    alloc_متن(choice_ڳڻپ, remap_متن , remap_choices[remap_choice]);
    choices[choice_ڳڻپ].changefunc = change_remap_choice;
    choices[choice_ڳڻپ].installfunc = عدم;
    choices[choice_ڳڻپ].سرگرم = (*oldvimrc == NUL);
    ++choice_ڳڻپ;

    // default way to use the mouse
    alloc_متن(choice_ڳڻپ, mouse_متن, mouse_choices[mouse_choice]);
    choices[choice_ڳڻپ].changefunc = change_mouse_choice;
    choices[choice_ڳڻپ].installfunc = عدم;
    choices[choice_ڳڻپ].سرگرم = (*oldvimrc == NUL);
    ++choice_ڳڻپ;
}

    ساكن LONG
reg_create_ڪنجي(
    HKEY پاڙ,
    ثابت حرف *subkey,
    PHKEY ph_ڪنجي,
    DWORD flag)
{
    DWORD disp;

    *ph_ڪنجي = عدم;
    أرجع Reg_Create_ڪنجي_Ex(
		پاڙ, subkey,
		0, عدم, REG_OPTION_NON_VOLATILE,
		flag | ڪنجي_WRITE,
		عدم, ph_ڪنجي, &disp);
}

    ساكن LONG
reg_سيٽ_string_قدر(
    HKEY h_ڪنجي,
    ثابت حرف *قدر_name,
    ثابت حرف *معلومات)
{
    أرجع Reg_سيٽ_قدر_Ex(h_ڪنجي, قدر_name, 0, REG_SZ,
				     (LPBYTE)معلومات, (DWORD)(1 + لڙي_ڊيگهه(معلومات)));
}

    ساكن LONG
reg_create_ڪنجي_and_قدر(
    HKEY h_پاڙ_ڪنجي,
    ثابت حرف *subkey,
    ثابت حرف *قدر_name,
    ثابت حرف *معلومات,
    DWORD flag)
{
    HKEY h_ڪنجي;
    LONG lRet = reg_create_ڪنجي(h_پاڙ_ڪنجي, subkey, &h_ڪنجي, flag);

    إذا (ERROR_SUCCESS == lRet)
    {
	lRet = reg_سيٽ_string_قدر(h_ڪنجي, قدر_name, معلومات);
	Reg_Close_ڪنجي(h_ڪنجي);
    }
    أرجع lRet;
}

    ساكن LONG
رجسٽر_inproc_server(
    HKEY h_پاڙ_ڪنجي,
    ثابت حرف *clsid,
    ثابت حرف *extname,
    ثابت حرف *module,
    ثابت حرف *threading_model,
    DWORD flag)
{
    CHAR subkey[BUFSIZE];
    LONG lRet;

    sprintf(subkey, "CLSID\\%s", clsid);
    lRet = reg_create_ڪنجي_and_قدر(h_پاڙ_ڪنجي, subkey, عدم, extname, flag);
    إذا (ERROR_SUCCESS == lRet)
    {
	sprintf(subkey, "CLSID\\%s\\InProcServer32", clsid);
	lRet = reg_create_ڪنجي_and_قدر(h_پاڙ_ڪنجي, subkey, عدم, module, flag);
	إذا (ERROR_SUCCESS == lRet)
	{
	    lRet = reg_create_ڪنجي_and_قدر(h_پاڙ_ڪنجي, subkey,
					   "ThreadingModel", threading_model, flag);
	}
    }
    أرجع lRet;
}

    ساكن LONG
رجسٽر_shellex(
    HKEY h_پاڙ_ڪنجي,
    ثابت حرف *clsid,
    ثابت حرف *name,
    ثابت حرف *exe_رستو,
    DWORD flag)
{
    LONG lRet = reg_create_ڪنجي_and_قدر(
	    h_پاڙ_ڪنجي,
	    "*\\shellex\\ContextMenuHandlers\\gvim",
	    عدم,
	    clsid,
	    flag);

    إذا (ERROR_SUCCESS == lRet)
    {
	lRet = reg_create_ڪنجي_and_قدر(
		HKEY_LOCAL_MACHINE,
		"Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved",
		clsid,
		name,
		flag);

	إذا (ERROR_SUCCESS == lRet)
	{
	    lRet = reg_create_ڪنجي_and_قدر(
		    HKEY_LOCAL_MACHINE,
		    "Software\\Vim\\Gvim",
		    "path",
		    exe_رستو,
		    flag);
	}
    }
    أرجع lRet;
}

    ساكن LONG
رجسٽر_openwith(
    HKEY h_پاڙ_ڪنجي,
    ثابت حرف *exe_رستو,
    DWORD flag)
{
    حرف	exe_cmd[BUFSIZE];
    LONG	lRet;

    sprintf(exe_cmd, "\"%s\" \"%%1\"", exe_رستو);
    lRet = reg_create_ڪنجي_and_قدر(
	    h_پاڙ_ڪنجي,
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
	    lRet = reg_create_ڪنجي_and_قدر(h_پاڙ_ڪنجي, openwith[i], عدم, "", flag);
    }

    أرجع lRet;
}

    ساكن LONG
رجسٽر_uninstall(
    HKEY h_پاڙ_ڪنجي,
    ثابت حرف *appname,
    ثابت حرف *display_name,
    ثابت حرف *uninstall_string,
    ثابت حرف *display_icon,
    ثابت حرف *display_نسخو,
    ثابت حرف *publisher)
{
    LONG lRet = reg_create_ڪنجي_and_قدر(h_پاڙ_ڪنجي, appname,
			     "DisplayName", display_name, ڪنجي_WOW64_64_ڪنجي);

    إذا (ERROR_SUCCESS == lRet)
	lRet = reg_create_ڪنجي_and_قدر(h_پاڙ_ڪنجي, appname,
		     "UninstallString", uninstall_string, ڪنجي_WOW64_64_ڪنجي);
    إذا (ERROR_SUCCESS == lRet)
	lRet = reg_create_ڪنجي_and_قدر(h_پاڙ_ڪنجي, appname,
		     "DisplayIcon", display_icon, ڪنجي_WOW64_64_ڪنجي);
    إذا (ERROR_SUCCESS == lRet)
	lRet = reg_create_ڪنجي_and_قدر(h_پاڙ_ڪنجي, appname,
		     "DisplayVersion", display_نسخو, ڪنجي_WOW64_64_ڪنجي);
    إذا (ERROR_SUCCESS == lRet)
	lRet = reg_create_ڪنجي_and_قدر(h_پاڙ_ڪنجي, appname,
		     "Publisher", publisher, ڪنجي_WOW64_64_ڪنجي);
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
    حرف	vim_exe_رستو[MAX_رستو];
    حرف	display_name[BUFSIZE];
    حرف	uninstall_string[BUFSIZE];
    حرف	icon_string[BUFSIZE];
    حرف	نسخو_string[BUFSIZE];
    صحيح		i;
    صحيح		loop_ڳڻپ = is_64bit_os() ? 2 : 1;
    DWORD	flag;

    sprintf(vim_exe_رستو, "%s\\gvim.exe", installdir);

    إذا (install_popup)
    {
	حرف	    bufg[BUFSIZE];

	لکو("Creating \"Edit with Vim\" popup menu entry\n");

	لكل (i = 0; i < loop_ڳڻپ; i++)
	{
	    إذا (i == 0)
	    {
		sprintf(bufg, "%s\\" GVIMEXT32_رستو, installdir);
		flag = ڪنجي_WOW64_32_ڪنجي;
	    }
	    وإلا
	    {
		sprintf(bufg, "%s\\" GVIMEXT64_رستو, installdir);
		flag = ڪنجي_WOW64_64_ڪنجي;
	    }

	    lRet = رجسٽر_inproc_server(
		    HKEY_CLASSES_پاڙ, vim_ext_clsid, vim_ext_name,
		    bufg, vim_ext_ThreadingModel, flag);
	    إذا (ERROR_SUCCESS != lRet)
		أرجع FAIL;
	    lRet = رجسٽر_shellex(
		    HKEY_CLASSES_پاڙ, vim_ext_clsid, vim_ext_name,
		    vim_exe_رستو, flag);
	    إذا (ERROR_SUCCESS != lRet)
		أرجع FAIL;
	}
    }

    إذا (install_openwith)
    {
	لکو("Creating \"Open with ...\" list entry\n");

	لكل (i = 0; i < loop_ڳڻپ; i++)
	{
	    إذا (i == 0)
		flag = ڪنجي_WOW64_32_ڪنجي;
	    وإلا
		flag = ڪنجي_WOW64_64_ڪنجي;

	    lRet = رجسٽر_openwith(HKEY_CLASSES_پاڙ, vim_exe_رستو, flag);
	    إذا (ERROR_SUCCESS != lRet)
		أرجع FAIL;
	}
    }

    لکو("Creating an uninstall entry\n");
    sprintf(display_name, "Vim " VIM_نسخو_SHORT
#جيڪڏهن_وصف_ٿيل _M_ARM64
	    " (arm64)"
#وإلاإذا _M_X64
	    " (x64)"
#جيڪڏهن_پورو
	    );

    // For the NSIS installer use the generated uninstaller.
    إذا (interactive)
	sprintf(uninstall_string, "%s\\uninstall.exe", installdir);
    وإلا
	sprintf(uninstall_string, "%s\\uninstall-gui.exe", installdir);

    sprintf(icon_string, "%s\\gvim.exe,0", installdir);

    sprintf(نسخو_string, VIM_نسخو_SHORT "." VIM_نسخو_PATCHLEVEL_STR);

    lRet = رجسٽر_uninstall(
	HKEY_LOCAL_MACHINE,
	"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Vim " VIM_نسخو_SHORT,
	display_name,
	uninstall_string,
	icon_string,
	نسخو_string,
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
	choices[idx].متن = "Install an entry for Vim in the popup menu for the right\n    mouse button so that you can edit any file with Vim";
	install_popup = 1;
    }
    وإلا
    {
	choices[idx].متن = "Do NOT install an entry for Vim in the popup menu for the\n    right mouse button to edit any file with Vim";
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
	    && (stat(GVIMEXT32_رستو, &st) >= 0
		|| stat(GVIMEXT64_رستو, &st) >= 0))
    {
	choices[choice_ڳڻپ].changefunc = change_popup_choice;
	choices[choice_ڳڻپ].installfunc = عدم;
	choices[choice_ڳڻپ].سرگرم = 1;
	change_popup_choice(choice_ڳڻپ);  // set the text
	++choice_ڳڻپ;
    }
    وإلا
	جوڙيو_dummy_choice();
}

    ساكن فراغ
change_openwith_choice(صحيح idx)
{
    إذا (install_openwith == 0)
    {
	choices[idx].متن = "Add Vim to the \"Open With...\" list in the popup menu for the right\n    mouse button so that you can edit any file with Vim";
	install_openwith = 1;
    }
    وإلا
    {
	choices[idx].متن = "Do NOT add Vim to the \"Open With...\" list in the popup menu for the\n    right mouse button to edit any file with Vim";
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
	choices[choice_ڳڻپ].changefunc = change_openwith_choice;
	choices[choice_ڳڻپ].installfunc = عدم;
	choices[choice_ڳڻپ].سرگرم = 1;
	change_openwith_choice(choice_ڳڻپ);  // set the text
	++choice_ڳڻپ;
    }
    وإلا
	جوڙيو_dummy_choice();
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
	ثابت حرف *iconfile_رستو,
	صحيح	    iconindex,
	ثابت حرف *shortcut_هدف,
	ثابت حرف *shortcut_دليلون,
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
	لکو("Error:  Could not open the COM library.  Not creating shortcut.\n");
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
	    Multi_Byte_منزل_Wide_Char(CP_ACP, 0, shortcut_name, -1, wsz, حجم(wsz)/حجم(wsz[0]));

	    // set the attributes
	    shelllink_ptr->lpVtbl->سيٽ_رستو(shelllink_ptr, shortcut_هدف);
	    shelllink_ptr->lpVtbl->سيٽ_Working_Directory(shelllink_ptr,
								  workingdir);
	    shelllink_ptr->lpVtbl->سيٽ_Icon_Location(shelllink_ptr,
						    iconfile_رستو, iconindex);
	    shelllink_ptr->lpVtbl->سيٽ_Arguments(shelllink_ptr, shortcut_دليلون);

	    // save the shortcut to a file and return the PersistFile object
	    persistfile_ptr->lpVtbl->Save(persistfile_ptr, wsz, 1);
	    persistfile_ptr->lpVtbl->Release(persistfile_ptr);
	}
	وإلا
	{
	    لکو("QueryInterface Error\n");
	    أرجع FAIL;
	}

	// Return the ShellLink object
	shelllink_ptr->lpVtbl->Release(shelllink_ptr);
    }
    وإلا
    {
	لکو("CoCreateInstance Error - hres = %08x\n", (صحيح)hres);
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
	حرف	   *link_رستو,
	ثابت حرف *link_name,
	ثابت حرف *shell_folder_name)
{
    حرف	shell_folder_رستو[MAX_رستو];

    إذا (get_shell_folder_رستو(shell_folder_رستو, shell_folder_name) == FAIL)
    {
	لکو("An error occurred while attempting to find the path to %s.\n",
							   shell_folder_name);
	أرجع FAIL;
    }

    // Make sure the directory exists (create Start Menu\Programs\Vim).
    // Ignore errors if it already exists.
    vim_mkdir(shell_folder_رستو, 0755);

    // build the path to the shortcut and the path to gvim.exe
    sprintf(link_رستو, "%s\\%s.lnk", shell_folder_رستو, link_name);

    أرجع OK;
}

    ساكن صحيح
build_shortcut(
	ثابت حرف *name,	// Name of the shortcut
	ثابت حرف *exename,	// Name of the executable (e.g., vim.exe)
	ثابت حرف *دليلون,
	ثابت حرف *shell_folder,
	ثابت حرف *workingdir)
{
    حرف	executable_رستو[BUFSIZE];
    حرف	link_name[BUFSIZE];

    sprintf(executable_رستو, "%s\\%s", installdir, exename);

    إذا (build_link_name(link_name, name, shell_folder) == FAIL)
    {
	لکو("An error has occurred.  A shortcut to %s will not be created %s.\n",
		name,
		*shell_folder == 'd' ? "on the desktop" : "in the Start menu");
	أرجع FAIL;
    }

    // Create the shortcut:
    أرجع create_shortcut(link_name, executable_رستو, 0,
					   executable_رستو, دليلون, workingdir);
}

/*
 * We used to use "homedir" as the working directory, but that is a bad choice
 * on multi-user systems.  However, not specifying a directory results in the
 * current directory to be c:\Windows\system32 on Windows 7. Use environment
 * variables instead.
 */
#وصف_ڪريو WORKDIR "%HOMEDRIVE%%HOMEPATH%"

/*
 * Create shortcut(s) in the Start Menu\Programs\Vim folder.
 */
    ساكن فراغ
install_شروعات_menu(صحيح idx اڻ_استعمال_ٿيل)
{
    need_uninstall_داخلا = 1;
    لکو("Creating start menu\n");
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
	حرف	shell_folder_رستو[BUFSIZE];

	// Creating the URL shortcut works a bit differently...
	إذا (get_shell_folder_رستو(shell_folder_رستو, VIM_STARTMENU) == FAIL)
	{
	    لکو("Finding the path of the Start menu failed\n");
	    أرجع ;
	}
	جوڙيو_pathsep(shell_folder_رستو);
	strcat(shell_folder_رستو, "Vim Online.url");
	إذا (!Write_ذاتي_Profile_String("InternetShortcut", "URL",
				    "https://www.vim.org/", shell_folder_رستو))
	{
	    لکو("Creating the Vim online URL failed\n");
	    أرجع;
	}
    }
}

    ساكن فراغ
toggle_startmenu_choice(صحيح idx)
{
    إذا (choices[idx].installfunc == عدم)
    {
	choices[idx].installfunc = install_شروعات_menu;
	choices[idx].متن = "Add Vim to the Start menu";
    }
    وإلا
    {
	choices[idx].installfunc = عدم;
	choices[idx].متن = "Do NOT add Vim to the Start menu";
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
	(فراغ)build_shortcut(icon_نالا[0], "gvim.exe",
						      "", "desktop", WORKDIR);
	need_uninstall_داخلا = 1;
    }
}

    فراغ
install_shortcut_evim(صحيح idx)
{
    إذا (choices[idx].arg)
    {
	(فراغ)build_shortcut(icon_نالا[1], "gvim.exe",
						    "-y", "desktop", WORKDIR);
	need_uninstall_داخلا = 1;
    }
}

    فراغ
install_shortcut_gview(صحيح idx)
{
    إذا (choices[idx].arg)
    {
	(فراغ)build_shortcut(icon_نالا[2], "gvim.exe",
						    "-R", "desktop", WORKDIR);
	need_uninstall_داخلا = 1;
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
	alloc_متن(idx, "Do NOT create a desktop icon for %s", arg);
    }
    وإلا
    {
	choices[idx].arg = 1;
	alloc_متن(idx, "Create a desktop icon for %s", arg);
    }
}

    ساكن فراغ
init_startmenu_choice(فراغ)
{
    // Start menu
    choices[choice_ڳڻپ].changefunc = toggle_startmenu_choice;
    choices[choice_ڳڻپ].installfunc = عدم;
    choices[choice_ڳڻپ].سرگرم = 1;
    toggle_startmenu_choice(choice_ڳڻپ);	// set the text
    ++choice_ڳڻپ;
}

/*
 * Add the choice for the desktop shortcuts.
 */
    ساكن فراغ
init_shortcut_choices(فراغ)
{
    // Shortcut to gvim
    choices[choice_ڳڻپ].متن = عدم;
    choices[choice_ڳڻپ].arg = 0;
    choices[choice_ڳڻپ].سرگرم = has_gvim;
    choices[choice_ڳڻپ].changefunc = toggle_shortcut_choice;
    choices[choice_ڳڻپ].installfunc = install_shortcut_gvim;
    toggle_shortcut_choice(choice_ڳڻپ);
    ++choice_ڳڻپ;

    // Shortcut to evim
    choices[choice_ڳڻپ].متن = عدم;
    choices[choice_ڳڻپ].arg = 0;
    choices[choice_ڳڻپ].سرگرم = has_gvim;
    choices[choice_ڳڻپ].changefunc = toggle_shortcut_choice;
    choices[choice_ڳڻپ].installfunc = install_shortcut_evim;
    toggle_shortcut_choice(choice_ڳڻپ);
    ++choice_ڳڻپ;

    // Shortcut to gview
    choices[choice_ڳڻپ].متن = عدم;
    choices[choice_ڳڻپ].arg = 0;
    choices[choice_ڳڻپ].سرگرم = has_gvim;
    choices[choice_ڳڻپ].changefunc = toggle_shortcut_choice;
    choices[choice_ڳڻپ].installfunc = install_shortcut_gview;
    toggle_shortcut_choice(choice_ڳڻپ);
    ++choice_ڳڻپ;
}

/*
 * Attempt to register OLE for Vim.
 */
   ساكن فراغ
install_OLE_رجسٽر(فراغ)
{
    حرف رجسٽر_command_string[BUFSIZE + 30];

    لکو("\n--- Attempting to register Vim with OLE ---\n");
    لکو("(There is no message whether this works or not.)\n");

    sprintf(رجسٽر_command_string, "\"%s\\gvim.exe\" -silent -register", installdir);
    system(رجسٽر_command_string);
}

/*
 * Remove the last part of directory "path[]" to get its parent, and put the
 * result in "to[]".
 */
    ساكن فراغ
dir_remove_آخري(ثابت حرف *رستو, حرف منزل[MAX_رستو])
{
    حرف c;
    ڊگهو آخري_char_منزل_copy;
    ڊگهو رستو_ڊيگهائي = لڙي_ڊيگهه(رستو);

    // skip the last character just in case it is a '\\'
    آخري_char_منزل_copy = رستو_ڊيگهائي - 2;
    c = رستو[آخري_char_منزل_copy];

    طالما (c != '\\')
    {
	آخري_char_منزل_copy--;
	c = رستو[آخري_char_منزل_copy];
    }

    strncpy(منزل, رستو, (size_t)آخري_char_منزل_copy);
    منزل[آخري_char_منزل_copy] = NUL;
}

    ساكن فراغ
سيٽ_directories_متن(صحيح idx)
{
    صحيح vimfiles_dir_choice = choices[idx].arg;

    إذا (vimfiles_dir_choice == (صحيح)vimfiles_dir_none)
	alloc_متن(idx, "Do NOT create plugin directories%s", "");
    وإلا
	alloc_متن(idx, "Create plugin directories: %s",
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
    حرف    buf[MAX_رستو];

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
		   && لڙي_ڊيگهه(homedrive) + لڙي_ڊيگهه(homepath) < حجم(buf))
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
	حرف	*قوت_نما;

	p = strchr(var + 1, '%');
	إذا (p != عدم)
	{
	    strncpy(buf, var + 1, p - (var + 1));
	    buf[p - (var + 1)] = NUL;
	    قوت_نما = getenv(buf);
	    إذا (قوت_نما != عدم && *قوت_نما != NUL
				&& لڙي_ڊيگهه(قوت_نما) + لڙي_ڊيگهه(p) < حجم(buf))
	    {
		snprintf(buf, حجم(buf), "%s%s", قوت_نما, p + 1);
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
    صحيح	    choice_ڳڻپ = جدول_سائيز(vimfiles_dir_choices);

    // Don't offer the $HOME choice if $HOME isn't set.
    إذا (homedir == عدم)
	--choice_ڳڻپ;
    choices[idx].arg = get_choice(vimfiles_dir_choices, choice_ڳڻپ);
    سيٽ_directories_متن(idx);
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
    حرف vimdir_رستو[MAX_رستو];
    حرف vimfiles_رستو[MAX_رستو + 9];
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
		dir_remove_آخري(installdir, vimdir_رستو);
	    وإلا
		strcpy(vimdir_رستو, p);
	    اكسر;
	}
	حالة vimfiles_dir_home:
	{
	    // Find the $HOME directory.  Its existence was already checked.
	    p = homedir;
	    إذا (p == عدم)
	    {
		لکو("Internal error: $HOME is NULL\n");
		p = "c:\\";
	    }
	    strcpy(vimdir_رستو, p);
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
    sprintf(vimfiles_رستو, "%s\\vimfiles", vimdir_رستو);
    vim_mkdir(vimfiles_رستو, 0755);

    لکو("Creating the following directories in \"%s\":\n", vimfiles_رستو);
    لكل (i = 0; i < جدول_سائيز(vimfiles_subdirs); i++)
    {
	sprintf(tmp_dirname, "%s\\%s", vimfiles_رستو, vimfiles_subdirs[i]);
	لکو("  %s", vimfiles_subdirs[i]);
	vim_mkdir(tmp_dirname, 0755);
    }
    لکو("\n");
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

    choices[choice_ڳڻپ].متن = alloc(150);
    choices[choice_ڳڻپ].changefunc = change_directories_choice;
    choices[choice_ڳڻپ].installfunc = install_vimfilesdir;
    choices[choice_ڳڻپ].سرگرم = 1;

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
	    dir_remove_آخري(installdir, tmp_dirname);
	وإلا
	    strcpy(tmp_dirname, p);
	strcat(tmp_dirname, "\\vimfiles\\compiler");
	إذا (stat(tmp_dirname, &st) == 0)
	    vimfiles_dir_choice = (صحيح)vimfiles_dir_none;
    }

    choices[choice_ڳڻپ].arg = vimfiles_dir_choice;
    سيٽ_directories_متن(choice_ڳڻپ);
    ++choice_ڳڻپ;
}

/*
 * Setup the choices and the default values.
 */
    ساكن فراغ
تياري_choices(فراغ)
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
print_cmd_سٽ_help(فراغ)
{
    لکو("Vim installer non-interactive command line arguments:\n");
    لکو("\n");
    لکو("-create-batfiles  [vim gvim evim view gview vimdiff gvimdiff]\n");
    لکو("    Create .bat files for Vim variants in the Windows directory.\n");
    لکو("-create-vimrc\n");
    لکو("    Create a default _vimrc file if one does not already exist.\n");
    لکو("-vimrc-remap [no|win]\n");
    لکو("    Remap keys when creating a default _vimrc file.\n");
    لکو("-vimrc-behave [unix|mswin|default]\n");
    لکو("    Set mouse behavior when creating a default _vimrc file.\n");
    لکو("-vimrc-compat [vi|vim|defaults|all]\n");
    لکو("    Set Vi compatibility when creating a default _vimrc file.\n");
    لکو("-install-popup\n");
    لکو("    Install the Edit-with-Vim context menu entry\n");
    لکو("-install-openwith\n");
    لکو("    Add Vim to the \"Open With...\" context menu list\n");
    لکو("-add-start-menu");
    لکو("    Add Vim to the start menu\n");
    لکو("-install-icons");
    لکو("    Create icons for gVim executables on the desktop\n");
    لکو("-create-directories [vim|home]\n");
    لکو("    Create runtime directories to drop plugins into; in the $VIM\n");
    لکو("    or $HOME directory\n");
    لکو("-register-OLE");
    لکو("    Ignored\n");
    لکو("\n");
}

/*
 * Setup installation choices based on command line switches
 */
    ساكن فراغ
command_سٽ_تياري_choices(صحيح argc, حرف **argv)
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
		لكل (j = 1; j < هدف_ڳڻپ; ++j)
		    إذا ((targets[j].exenamearg[0] == 'g' ? has_gvim : has_vim)
			    && strcmp(argv[i], targets[j].name) == 0)
		    {
			init_bat_choice(j);
			اكسر;
		    }
		إذا (j == هدف_ڳڻپ)
		    لکو("%s is not a valid choice for -create-batfiles\n",
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
		    لکو("Unknown argument for -create-directories: %s\n",
								     argv[i]);
		    print_cmd_سٽ_help();
		}
	    }
	    وإلا // No choice specified, default to vim directory
		vimfiles_dir_choice = (صحيح)vimfiles_dir_vim;
	    choices[choice_ڳڻپ - 1].arg = vimfiles_dir_choice;
	}
	وإلا إذا (strcmp(argv[i], "-register-OLE") == 0)
	{
	    // This is always done when gvim is found
	}
	وإلا // Unknown switch
	{
	    لکو("Got unknown argument argv[%d] = %s\n", i, argv[i]);
	    print_cmd_سٽ_help();
	}
    }
}


/*
 * Show a few screens full of helpful information.
 */
    ساكن فراغ
show_help(فراغ)
{
    ساكن حرف *(شيون[]) =
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
    لکو("\n");
    لكل (i = 0; شيون[i] != عدم; ++i)
    {
	puts(شيون[i]);
	لکو("Hit Enter to continue, b (back) or q (quit help): ");
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
	لکو("\n");
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
    لكل (i = 0; i < choice_ڳڻپ; ++i)
	إذا (choices[i].installfunc != عدم && choices[i].سرگرم)
	    (choices[i].installfunc)(i);

    // Add some entries to the registry, if needed.
    إذا (install_popup
	    || install_openwith
	    || (need_uninstall_داخلا && interactive)
	    || !interactive)
	install_registry();

    // Register gvim with OLE.
    إذا (has_gvim)
	install_OLE_رجسٽر();
}

/*
 * request_choice
 */
    ساكن فراغ
request_choice(فراغ)
{
    صحيح		      i;

    لکو("\n\nInstall will do for you:\n");
    لكل (i = 0; i < choice_ڳڻپ; ++i)
	إذا (choices[i].سرگرم)
	    لکو("%2d  %s\n", i + 1, choices[i].متن);
    لکو("To change an item, enter its number\n\n");
    لکو("Enter item number, h (help), d (do it) or q (quit): ");
}

    صحيح
مکيه(صحيح argc, حرف **argv)
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
	ٻاهر_نڪرو(0);
    }

    لکو("This program sets up the installation of Vim "
						   VIM_نسخو_MEDIUM "\n\n");

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
	تياري_choices();

	// Let the user change choices and finally install (or quit).
	لكل (;;)
	{
	    request_choice();
	    rewind(stdin);
	    إذا (scanf("%99s", buf) == 1)
	    {
		إذا (انگ_آهي((بنانشان حرف)buf[0]))
		{
		    // Change a choice.
		    i = atoi(buf);
		    إذا (i > 0 && i <= choice_ڳڻپ && choices[i - 1].سرگرم)
			(choices[i - 1].changefunc)(i - 1);
		    وإلا
			لکو("\nIllegal choice\n");
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
		    لکو("\nThat finishes the installation.  Happy Vimming!\n");
		    اكسر;
		}
		وإلا إذا (buf[0] == 'q' || buf[0] == 'Q')
		{
		    // Quit
		    لکو("\nExiting without anything done\n");
		    اكسر;
		}
		وإلا
		    لکو("\nIllegal choice\n");
	    }
	}
	لکو("\n");
	myexit(0);
    }
    وإلا
    {
	/*
	 * Run non-interactive - setup according to the command line switches
	 */
	command_سٽ_تياري_choices(argc, argv);
	install();

	// Avoid that the user has to hit Enter, just wait a little bit to
	// allow reading the messages.
	sleep(2);
    }

    أرجع 0;
}
