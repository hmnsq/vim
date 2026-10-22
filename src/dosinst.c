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
#تعریف_کرو DOSINST
#شامل "dosinst.h"
#شامل <io.h>

#تعریف_کرو GVIMEXT64_راستہ	    "GvimExt64\\gvimext.dll"
#تعریف_کرو GVIMEXT32_راستہ	    "GvimExt32\\gvimext.dll"

// Macro to do an error check I was typing over and over
#تعریف_کرو CHECK_REG_ERROR(رمز) \
    کرو { \
	اگر (رمز != ERROR_SUCCESS) \
	{ \
	    لکھو("%ld error number:  %ld\n", (طویل)__LINE__, (طویل)رمز); \
	    واپس 1; \
	} \
    } جب (0)

صحیح	has_vim = 0;		// installable vim.exe exists
صحیح	has_gvim = 0;		// installable gvim.exe exists

حرف	oldvimrc[BUFSIZE];	// name of existing vimrc file
حرف	vimrc[BUFSIZE];		// name of vimrc file to create

حرف	*default_bat_dir = تہی;  // when not NULL, use this as the default
				  // directory to write .bat files in
حرف	*default_vim_dir = تہی;  // when not NULL, use this as the default
				  // install dir for NSIS

/*
 * Structure used for each choice the user can make.
 */
ساخت choice
{
    صحیح	    فعال;			// non-zero when choice is active
    حرف    *متن;			// text displayed for this choice
    خالی    (*changefunc)(صحیح idx);	// function to change this choice
    صحیح	    arg;			// argument for function
    خالی    (*installfunc)(صحیح idx);	// function to install this choice
};

ساخت choice	choices[30];		// choices the user can make
صحیح		choice_گنتی = 0;	// number of choices available

#تعریف_کرو جدول_سائز(s)	(صحیح)ARRAYSIZE(s)

شمار
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
صحیح	compat_choice = (صحیح)compat_all_enhancements;
حرف	*compat_متن = "- run Vim %s";

شمار
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
صحیح	remap_choice = (صحیح)remap_no;
حرف	*remap_متن = "- %s";

شمار
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
صحیح	mouse_choice = (صحیح)mouse_default;
حرف	*mouse_متن = "- The mouse %s";

شمار
{
    vimfiles_dir_none = 1,
    vimfiles_dir_vim,
    vimfiles_dir_home
};
ساکن حرف *(vimfiles_dir_choices[]) =
{
    "\nCreate plugin directories:",
    "No",
    "In the VIM directory",
    "In your HOME directory",
};

// non-zero when selected to install the popup menu entry.
ساکن صحیح	install_popup = 0;

// non-zero when selected to install the "Open with" entry.
ساکن صحیح	install_openwith = 0;

// non-zero when need to add an uninstall entry in the registry
ساکن صحیح	need_uninstall_اندراج = 0;

/*
 * Definitions of the directory name (under $VIM) of the vimfiles directory
 * and its subdirectories:
 */
ساکن حرف	*(vimfiles_subdirs[]) =
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
    ساکن صحیح
get_choice(حرف **جدول, صحیح entries)
{
    صحیح		answer;
    صحیح		idx;
    حرف	dummy[100];

    کرو
    {
	برائے (idx = 0; idx < entries; ++idx)
	{
	    اگر (idx)
		لکھو("%2d  ", idx);
	    puts(جدول[idx]);
	}
	لکھو("Choice: ");
	اگر (scanf("%d", &answer) != 1)
	{
	    scanf("%99s", dummy);
	    answer = 0;
	}
    }
    جب (answer < 1 || answer >= entries);

    واپس answer;
}

/*
 * Check if the user unpacked the archives properly.
 * Sets "runtimeidx".
 */
    ساکن خالی
check_unpack(خالی)
{
    حرف	buf[BUFSIZE];
    FILE	*fd;
    ساخت stat	st;

    // check for presence of the correct version number in installdir[]
    runtimeidx = لڑی_لمبائی(installdir) - لڑی_لمبائی(VIM_نسخہ_NODOT);
    اگر (runtimeidx <= 0
	    || stricmp(installdir + runtimeidx, VIM_نسخہ_NODOT) != 0
	    || (installdir[runtimeidx - 1] != '/'
		&& installdir[runtimeidx - 1] != '\\'))
    {
	لکھو("ERROR: Install program not in directory \"%s\"\n",
		VIM_نسخہ_NODOT);
	لکھو("This program can only work when it is located in its original directory\n");
	myexit(1);
    }

    // check if filetype.vim is present, which means the runtime archive has
    // been unpacked
    sprintf(buf, "%s\\filetype.vim", installdir);
    اگر (stat(buf, &st) < 0)
    {
	لکھو("ERROR: Cannot find filetype.vim in \"%s\"\n", installdir);
	لکھو("It looks like you did not unpack the runtime archive.\n");
	لکھو("You must unpack the runtime archive \"%srt.zip\" before installing.\n",
		VIM_نسخہ_NODOT);
	myexit(1);
    }

    // Check if vim.exe or gvim.exe is in the current directory.
    اگر ((fd = fopen("gvim.exe", "r")) != تہی)
    {
	fclose(fd);
	has_gvim = 1;
    }
    اگر ((fd = fopen("vim.exe", "r")) != تہی)
    {
	fclose(fd);
	has_vim = 1;
    }
    اگر (!has_gvim && !has_vim)
    {
	لکھو("ERROR: Cannot find any Vim executables in \"%s\"\n\n",
								  installdir);
	myexit(1);
    }
}

/*
 * Compare paths "p[plen]" to "q[qlen]".  Return 0 if they match.
 * Ignores case and differences between '/' and '\'.
 * "plen" and "qlen" can be negative, strlen() is used then.
 */
    ساکن صحیح
pathcmp(حرف *p, صحیح plen, حرف *q, صحیح qlen)
{
    صحیح		i;

    اگر (plen < 0)
	plen = لڑی_لمبائی(p);
    اگر (qlen < 0)
	qlen = لڑی_لمبائی(q);
    برائے (i = 0; ; ++i)
    {
	// End of "p": check if "q" also ends or just has a slash.
	اگر (i == plen)
	{
	    اگر (i == qlen)  //مطابقت
		واپس 0;
	    اگر (i == qlen - 1 && (q[i] == '\\' || q[i] == '/'))
		واپس 0;   // match with trailing slash
	    واپس 1;	    // no match
	}

	// End of "q": check if "p" also ends or just has a slash.
	اگر (i == qlen)
	{
	    اگر (i == plen)  //مطابقت
		واپس 0;
	    اگر (i == plen - 1 && (p[i] == '\\' || p[i] == '/'))
		واپس 0;   // match with trailing slash
	    واپس 1;	    // no match
	}

	اگر (!(mytoupper(p[i]) == mytoupper(q[i])
		|| ((p[i] == '/' || p[i] == '\\')
		    && (q[i] == '/' || q[i] == '\\'))))
	    واپس 1;	    // no match
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
    ساکن خالی
findoldfile(حرف **destination)
{
    حرف	*bp = *destination;
    size_t	indir_l = لڑی_لمبائی(installdir);
    حرف	*cp;
    حرف	*tmpname;
    حرف	*farname;

    /*
     * No action needed if exe not found or not in this directory.
     */
    اگر (bp == تہی || strnicmp(bp, installdir, indir_l) != 0)
	واپس;
    cp = bp + indir_l;
    اگر (strchr("/\\", *cp++) == تہی
	    || strchr(cp, '\\') != تہی
	    || strchr(cp, '/') != تہی)
	واپس;

    tmpname = alloc(لڑی_لمبائی(cp) + 1);
    strcpy(tmpname, cp);
    tmpname[لڑی_لمبائی(tmpname) - 1] = 'x';	// .exe -> .exx

    اگر (access(tmpname, 0) == 0)
    {
	لکھو("\nERROR: %s and %s clash.  Remove or rename %s.\n",
	    tmpname, cp, tmpname);
	myexit(1);
    }

    اگر (rename(cp, tmpname) != 0)
    {
	لکھو("\nERROR: failed to rename %s to %s: %s\n",
	    cp, tmpname, strerror(0));
	myexit(1);
    }

    farname = searchpath_save(cp);

    اگر (rename(tmpname, cp) != 0)
    {
	لکھو("\nERROR: failed to rename %s back to %s: %s\n",
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
    ساکن خالی
ڈھونڈو_bat_exe(صحیح check_bat_only)
{
    صحیح		i;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    برائے (i = 1; i < ہدف_گنتی; ++i)
    {
	targets[i].oldbat = searchpath_save(targets[i].batname);
	اگر (!check_bat_only)
	    targets[i].oldexe = searchpath_save(targets[i].exename);

	اگر (default_bat_dir == تہی && targets[i].oldbat != تہی)
	{
	    default_bat_dir = alloc(لڑی_لمبائی(targets[i].oldbat) + 1);
	    strcpy(default_bat_dir, targets[i].oldbat);
	    remove_دم(default_bat_dir);
	}
	اگر (check_bat_only && targets[i].oldbat != تہی)
	{
	    آزاد(targets[i].oldbat);
	    targets[i].oldbat = تہی;
	}
    }

    mch_chdir(installdir);
}

/*
 * Get the value of $VIMRUNTIME or $VIM and write it in $TEMP/vimini.ini, so
 * that NSIS can read it.
 * When not set, use the directory of a previously installed Vim.
 */
    ساکن خالی
get_vim_env(خالی)
{
    حرف	*vim;
    حرف	buf[BUFSIZE];
    FILE	*fd;
    حرف	fname[BUFSIZE];

    // First get $VIMRUNTIME.  If it's set, remove the tail.
    vim = getenv("VIMRUNTIME");
    اگر (vim != تہی && *vim != 0 && لڑی_لمبائی(vim) < حجم(buf))
    {
	strcpy(buf, vim);
	remove_دم(buf);
	vim = buf;
    }
    ورنہ
    {
	vim = getenv("VIM");
	اگر (vim == تہی || *vim == 0)
	{
	    // Use the directory from an old uninstall entry.
	    اگر (default_vim_dir != تہی)
		vim = default_vim_dir;
	    ورنہ
		// Let NSIS know there is no default, it should use
		// $PROGRAMFILES.
		vim = "";
	}
    }

    // NSIS also uses GetTempPath(), thus we should get the same directory
    // name as where NSIS will look for vimini.ini.
    Get_عارضی_راستہ(حجم(fname) - 12, fname);
    جوڑو_pathsep(fname);
    strcat(fname, "vimini.ini");

    fd = fopen(fname, "w");
    اگر (fd != تہی)
    {
	// Make it look like an .ini file, so that NSIS can read it with a
	// ReadINIStr command.
	fprintf(fd, "[vimini]\n");
	fprintf(fd, "dir=\"%s\"\n", vim);
	fclose(fd);
    }
    ورنہ
    {
	لکھو("Failed to open %s\n", fname);
	sleep(2);
    }
}

ساکن صحیح عدد_windows;

/*
 * Callback used for EnumWindows():
 * Count the window if the title looks like it is for the uninstaller.
 */
//ARGSUSED
    ساکن BOOL CALLBACK
کھڑکی_cb(HWND hwnd, LPARAM lparam غیر_مستعمل)
{
    حرف title[256];

    title[0] = 0;
    Get_کھڑکی_متن(hwnd, title, 256);
    اگر (لڑی_تلاش(title, "Vim ") != تہی && لڑی_تلاش(title, " Uninstall") != تہی)
	++عدد_windows;
    واپس TRUE;
}

/*
 * Run the uninstaller silently.
 */
    ساکن صحیح
تسلسل_silent_uninstall(حرف *uninst_exe)
{
    حرف    vimrt_dir[BUFSIZE];
    حرف    عارضی_uninst[BUFSIZE];
    حرف    عارضی_dir[MAX_راستہ];
    حرف    buf[BUFSIZE * 2 + 10];
    صحیح	    i;
    DWORD   tick;

    strcpy(vimrt_dir, uninst_exe);
    remove_دم(vimrt_dir);

    اگر (!Get_عارضی_راستہ(حجم(عارضی_dir), عارضی_dir))
	واپس FAIL;

    // Copy the uninstaller to a temporary exe.
    tick = Get_Tick_گنتی();
    برائے (i = 0; ; i++)
    {
	sprintf(عارضی_uninst, "%s\\vimun%04X.exe", عارضی_dir,
					  (غیرعلامتی صحیح)((i + tick) & 0xFFFF));
	اگر (CopyFile(uninst_exe, عارضی_uninst, TRUE))
	    توڑو;
	اگر (Get_آخری_Error() != ERROR_FILE_EXISTS)
	    واپس FAIL;
	اگر (i == 65535)
	    واپس FAIL;
    }

    // Run the copied uninstaller silently.
    اگر (strchr(عارضی_uninst, ' ') != تہی)
	sprintf(buf, "\"%s\" /S _?=%s", عارضی_uninst, vimrt_dir);
    ورنہ
	sprintf(buf, "%s /S _?=%s", عارضی_uninst, vimrt_dir);
    تسلسل_command(buf);

    DeleteFile(عارضی_uninst);
    واپس OK;
}

/*
 * Check for already installed Vims.
 * Return non-zero when found one.
 */
    ساکن صحیح
uninstall_check(صحیح skip_question)
{
    HKEY	کلید_handle;
    HKEY	uninstall_کلید_handle;
    حرف	*uninstall_کلید = "software\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
    حرف	subkey_name_buff[BUFSIZE];
    حرف	عارضی_string_بفر[BUFSIZE-2];
    DWORD	local_bufsize;
    FILETIME	عارضی_pfiletime;
    DWORD	کلید_اشاریہ;
    حرف	داخلہ;
    طویل	رمز;
    DWORD	قدر_type;
    DWORD	orig_عدد_کلیدیں;
    DWORD	new_عدد_کلیدیں;
    DWORD	allow_silent;
    صحیح		foundone = 0;

    رمز = Reg_Open_کلید_Ex(HKEY_LOCAL_MACHINE, uninstall_کلید, 0,
				     کلید_WOW64_64_کلید | کلید_READ, &کلید_handle);
    CHECK_REG_ERROR(رمز);

    کلید_اشاریہ = 0;
    جب (TRUE)
    {
	local_bufsize = حجم(subkey_name_buff);
	اگر (Reg_Enum_کلید_Ex(کلید_handle, کلید_اشاریہ, subkey_name_buff, &local_bufsize,
		تہی, تہی, تہی, &عارضی_pfiletime) == ERROR_NO_مزید_اشیاء)
	    توڑو;

	اگر (لڑی_موازنہ("Vim", subkey_name_buff, 3) == 0)
	{
	    // Open the key named Vim*
	    رمز = Reg_Open_کلید_Ex(کلید_handle, subkey_name_buff, 0,
			   کلید_WOW64_64_کلید | کلید_READ, &uninstall_کلید_handle);
	    CHECK_REG_ERROR(رمز);

	    // get the DisplayName out of it to show the user
	    local_bufsize = حجم(عارضی_string_بفر);
	    رمز = Reg_Query_قدر_Ex(uninstall_کلید_handle, "displayname", 0,
		    &قدر_type, (LPBYTE)عارضی_string_بفر,
		    &local_bufsize);
	    CHECK_REG_ERROR(رمز);

	    allow_silent = 0;
	    اگر (skip_question)
	    {
		DWORD varsize = حجم(DWORD);

		Reg_Query_قدر_Ex(uninstall_کلید_handle, "AllowSilent", 0,
			&قدر_type, (LPBYTE)&allow_silent,
			&varsize);
	    }

	    foundone = 1;
	    لکھو("\n*********************************************************\n");
	    لکھو("Vim Install found what looks like an existing Vim version.\n");
	    لکھو("The name of the entry is:\n");
	    لکھو("\n        \"%s\"\n\n", عارضی_string_بفر);

	    لکھو("Installing the new version will disable part of the existing version.\n");
	    لکھو("(The batch files used in a console and the \"Edit with Vim\" entry in\n");
	    لکھو("the popup menu will use the new version)\n");

	    اگر (skip_question)
		لکھو("\nRunning uninstall program for \"%s\"\n", عارضی_string_بفر);
	    ورنہ
		لکھو("\nDo you want to uninstall \"%s\" now?\n(y)es/(n)o)  ", عارضی_string_بفر);
	    fflush(stdout);

	    // get the UninstallString
	    local_bufsize = حجم(عارضی_string_بفر);
	    رمز = Reg_Query_قدر_Ex(uninstall_کلید_handle, "uninstallstring", 0,
		    &قدر_type, (LPBYTE)عارضی_string_بفر, &local_bufsize);
	    CHECK_REG_ERROR(رمز);

	    // Remember the directory, it is used as the default for NSIS.
	    default_vim_dir = alloc(لڑی_لمبائی(عارضی_string_بفر) + 1);
	    strcpy(default_vim_dir, عارضی_string_بفر);
	    remove_دم(default_vim_dir);
	    remove_دم(default_vim_dir);

	    داخلہ = 'n';
	    کرو
	    {
		اگر (داخلہ != 'n')
		    لکھو("%c is an invalid reply.  Please enter either 'y' or 'n'\n", داخلہ);

		اگر (skip_question)
		    داخلہ = 'y';
		ورنہ
		{
		    rewind(stdin);
		    scanf("%c", &داخلہ);
		}
		انتخاب (داخلہ)
		{
		    حالت 'y':
		    حالت 'Y':
			// save the number of uninstall keys so we can know if
			// it changed
			Reg_Query_Info_کلید(کلید_handle, تہی, تہی, تہی,
					     &orig_عدد_کلیدیں, تہی, تہی, تہی,
						      تہی, تہی, تہی, تہی);

			// Find existing .bat files before deleting them.
			ڈھونڈو_bat_exe(TRUE);

			اگر (allow_silent)
			{
			    اگر (تسلسل_silent_uninstall(عارضی_string_بفر)
								    == FAIL)
				allow_silent = 0; // Retry with non silent.
			}
			اگر (!allow_silent)
			{
			    // Execute the uninstall program.  Put it in double
			    // quotes if there is an embedded space.
			    {
				حرف buf[BUFSIZE];

				اگر (strchr(عارضی_string_بفر, ' ') != تہی)
				    sprintf(buf, "\"%s\"", عارضی_string_بفر);
				ورنہ
				    strcpy(buf, عارضی_string_بفر);
				تسلسل_command(buf);
			    }

			    // Count the number of windows with a title that
			    // match the installer, so that we can check when
			    // it's done.  The uninstaller copies itself,
			    // executes the copy and exits, thus we can't wait
			    // for the process to finish.
			    sleep(1);  // wait for uninstaller to start up
			    عدد_windows = 0;
			    EnumWindows(کھڑکی_cb, 0);
			    اگر (عدد_windows == 0)
			    {
				// Did not find the uninstaller, ask user to
				// press Enter when done. Just in case.
				لکھو("Press Enter when the uninstaller is finished\n");
				rewind(stdin);
				(خالی)getchar();
			    }
			    ورنہ
			    {
				لکھو("Waiting for the uninstaller to finish (press CTRL-C to abort).");
				کرو
				{
				    لکھو(".");
				    fflush(stdout);
				    sleep(1);	// wait for the uninstaller to
						//اختتام
				    عدد_windows = 0;
				    EnumWindows(کھڑکی_cb, 0);
				} جب (عدد_windows > 0);
			    }
			}
			لکھو("\nDone!\n");

			// Check if an uninstall reg key was deleted.
			// if it was, we want to decrement key_index.
			// if we don't do this, we will skip the key
			// immediately after any key that we delete.
			Reg_Query_Info_کلید(کلید_handle, تہی, تہی, تہی,
					      &new_عدد_کلیدیں, تہی, تہی, تہی,
						      تہی, تہی, تہی, تہی);
			اگر (new_عدد_کلیدیں < orig_عدد_کلیدیں)
			    کلید_اشاریہ--;

			داخلہ = 'y';
			توڑو;

		    حالت 'n':
		    حالت 'N':
			// Do not uninstall
			داخلہ = 'n';
			توڑو;

		    طےشدہ: // just drop through and redo the loop
			توڑو;
		}

	    } جب (داخلہ != 'n' && داخلہ != 'y');

	    Reg_Close_کلید(uninstall_کلید_handle);
	}

	کلید_اشاریہ++;
    }
    Reg_Close_کلید(کلید_handle);

    واپس foundone;
}

/*
 * Find out information about the system.
 */
    ساکن خالی
inspect_system(خالی)
{
    حرف	*p;
    حرف	buf[BUFSIZE];
    FILE	*fd;
    صحیح		i;
    صحیح		foundone;

    // This may take a little while, let the user know what we're doing.
    لکھو("Inspecting system...\n");

    /*
     * If $VIM is set, check that it's pointing to our directory.
     */
    p = getenv("VIM");
    اگر (p != تہی && pathcmp(p, -1, installdir, runtimeidx - 1) != 0)
    {
	لکھو("------------------------------------------------------\n");
	لکھو("$VIM is set to \"%s\".\n", p);
	لکھو("This is different from where this version of Vim is:\n");
	strcpy(buf, installdir);
	*(buf + runtimeidx - 1) = NUL;
	لکھو("\"%s\"\n", buf);
	لکھو("You must adjust or remove the setting of $VIM,\n");
	اگر (interactive)
	{
	    لکھو("to be able to use this install program.\n");
	    myexit(1);
	}
	لکھو("otherwise Vim WILL NOT WORK properly!\n");
	لکھو("------------------------------------------------------\n");
    }

    /*
     * If $VIMRUNTIME is set, check that it's pointing to our runtime directory.
     */
    p = getenv("VIMRUNTIME");
    اگر (p != تہی && pathcmp(p, -1, installdir, -1) != 0)
    {
	لکھو("------------------------------------------------------\n");
	لکھو("$VIMRUNTIME is set to \"%s\".\n", p);
	لکھو("This is different from where this version of Vim is:\n");
	لکھو("\"%s\"\n", installdir);
	لکھو("You must adjust or remove the setting of $VIMRUNTIME,\n");
	اگر (interactive)
	{
	    لکھو("to be able to use this install program.\n");
	    myexit(1);
	}
	لکھو("otherwise Vim WILL NOT WORK properly!\n");
	لکھو("------------------------------------------------------\n");
    }

    /*
     * Check if there is a vim.[exe|bat|, gvim.[exe|bat|, etc. in the path.
     */
    ڈھونڈو_bat_exe(FALSE);

    /*
     * A .exe in the install directory may be found anyway on Windows 2000.
     * Check for this situation and find another executable if necessary.
     * w.briscoe@ponl.com 2001-01-20
     */
    foundone = 0;
    برائے (i = 1; i < ہدف_گنتی; ++i)
    {
	findoldfile(&(targets[i].oldexe));
	اگر (targets[i].oldexe != تہی)
	    foundone = 1;
    }

    اگر (foundone)
    {
	لکھو("Warning: Found Vim executable(s) in your $PATH:\n");
	برائے (i = 1; i < ہدف_گنتی; ++i)
	    اگر (targets[i].oldexe != تہی)
		لکھو("%s\n", targets[i].oldexe);
	لکھو("It will be used instead of the version you are installing.\n");
	لکھو("Please delete or rename it, or adjust your $PATH setting.\n");
    }

    /*
     * Check if there is an existing ../_vimrc or ../.vimrc file.
     */
    strcpy(oldvimrc, installdir);
    strcpy(oldvimrc + runtimeidx, "_vimrc");
    اگر ((fd = fopen(oldvimrc, "r")) == تہی)
    {
	strcpy(oldvimrc + runtimeidx, "vimrc~1"); // short version of .vimrc
	اگر ((fd = fopen(oldvimrc, "r")) == تہی)
	{
	    strcpy(oldvimrc + runtimeidx, ".vimrc");
	    fd = fopen(oldvimrc, "r");
	}
    }
    اگر (fd != تہی)
	fclose(fd);
    ورنہ
	*oldvimrc = NUL;
}

/*
 * Add a dummy choice to avoid that the numbering changes depending on items
 * in the environment.  The user may type a number he remembered without
 * looking.
 */
    ساکن خالی
جوڑو_dummy_choice(خالی)
{
    choices[choice_گنتی].installfunc = تہی;
    choices[choice_گنتی].فعال = 0;
    choices[choice_گنتی].changefunc = تہی;
    choices[choice_گنتی].متن = تہی;
    choices[choice_گنتی].arg = 0;
    ++choice_گنتی;
}

////////////////////////////////////////////////
// stuff for creating the batch files.

/*
 * Install the vim.bat, gvim.bat, etc. files.
 */
    ساکن خالی
install_bat_choice(صحیح idx)
{
    حرف	*batpath = targets[choices[idx].arg].batpath;
    حرف	*oldname = targets[choices[idx].arg].oldbat;
    حرف	*exename = targets[choices[idx].arg].exenamearg;
    حرف	*vimarg = targets[choices[idx].arg].exearg;
    FILE	*fd;

    اگر (*batpath == NUL)
	واپس;

    fd = fopen(batpath, "w");
    اگر (fd == تہی)
    {
	لکھو("\nERROR: Cannot open \"%s\" for writing.\n", batpath);
	واپس;
    }

    need_uninstall_اندراج = 1;

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
	    VIM_نسخہ_NODOT, exename, VIM_نسخہ_NODOT);
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
    ورنہ
    {
	// Always use quotes, $VIM or $VIMRUNTIME might have a space.
	fprintf(fd, "\"%%VIM_EXE_DIR%%\\%s\" %s %%*\n",
		exename, vimarg);
    }

    fclose(fd);
    لکھو("%s has been %s\n", batpath,
	    oldname == تہی ? "created" : "overwritten");
}

/*
 * Make the text string for choice "idx".
 * The format "fmt" is must have one %s item, which "arg" is used for.
 */
    ساکن خالی
alloc_متن(صحیح idx, حرف *fmt, حرف *arg)
{
    اگر (choices[idx].متن != تہی)
	آزاد(choices[idx].متن);

    choices[idx].متن = alloc(لڑی_لمبائی(fmt) + لڑی_لمبائی(arg) - 1);
    sprintf(choices[idx].متن, fmt, arg);
}

/*
 * Toggle the "Overwrite .../vim.bat" to "Don't overwrite".
 */
    ساکن خالی
toggle_bat_choice(صحیح idx)
{
    حرف	*batname = targets[choices[idx].arg].batpath;
    حرف	*oldname = targets[choices[idx].arg].oldbat;

    اگر (*batname == NUL)
    {
	alloc_متن(idx, "    Overwrite %s", oldname);
	strcpy(batname, oldname);
    }
    ورنہ
    {
	alloc_متن(idx, "    Do NOT overwrite %s", oldname);
	*batname = NUL;
    }
}

/*
 * Do some work for a batch file entry: Append the batch file name to the path
 * and set the text for the choice.
 */
    ساکن خالی
سیٹ_bat_متن(صحیح idx, حرف *batpath, حرف *name)
{
    strcat(batpath, name);

    alloc_متن(idx, "    Create %s", batpath);
}

/*
 * Select a directory to write the batch file line.
 */
    ساکن خالی
change_bat_choice(صحیح idx)
{
    حرف	*راستہ;
    حرف	*batpath;
    حرف	*name;
    صحیح		n;
    حرف	*s;
    حرف	*p;
    صحیح		گنتی;
    حرف	**نام = تہی;
    صحیح		i;
    صحیح		ہدف = choices[idx].arg;

    name = targets[ہدف].batname;
    batpath = targets[ہدف].batpath;

    راستہ = getenv("PATH");
    اگر (راستہ == تہی)
    {
	لکھو("\nERROR: The variable $PATH is not set\n");
	واپس;
    }

    /*
     * first round: count number of names in path;
     * second round: save names to names[].
     */
    برائے (;;)
    {
	گنتی = 1;
	برائے (p = راستہ; *p; )
	{
	    s = strchr(p, ';');
	    اگر (s == تہی)
		s = p + لڑی_لمبائی(p);
	    اگر (نام != تہی)
	    {
		نام[گنتی] = alloc(s - p + 1);
		strncpy(نام[گنتی], p, s - p);
		نام[گنتی][s - p] = NUL;
	    }
	    ++گنتی;
	    p = s;
	    اگر (*p != NUL)
		++p;
	}
	اگر (نام != تہی)
	    توڑو;
	نام = alloc((گنتی + 1) * حجم(حرف *));
    }
    نام[0] = alloc(50);
    sprintf(نام[0], "Select directory to create %s in:", name);
    نام[گنتی] = alloc(50);
    اگر (choices[idx].arg == 0)
	sprintf(نام[گنتی], "Do not create any .bat file.");
    ورنہ
	sprintf(نام[گنتی], "Do not create a %s file.", name);
    n = get_choice(نام, گنتی + 1);

    اگر (n == گنتی)
    {
	// Selected last item, don't create bat file.
	*batpath = NUL;
	اگر (choices[idx].arg != 0)
	    alloc_متن(idx, "    Do NOT create %s", name);
    }
    ورنہ
    {
	// Selected one of the paths.  For the first item only keep the path,
	// for the others append the batch file name.
	strcpy(batpath, نام[n]);
	جوڑو_pathsep(batpath);
	اگر (choices[idx].arg != 0)
	    سیٹ_bat_متن(idx, batpath, name);
    }

    برائے (i = 0; i <= گنتی; ++i)
	آزاد(نام[i]);
    آزاد(نام);
}

حرف *bat_متن_yes = "Install .bat files to use Vim at the command line:";
حرف *bat_متن_no = "do NOT install .bat files to use Vim at the command line";

    ساکن خالی
change_main_bat_choice(صحیح idx)
{
    صحیح		i;

    // let the user select a default directory or NONE
    change_bat_choice(idx);

    اگر (targets[0].batpath[0] != NUL)
	choices[idx].متن = bat_متن_yes;
    ورنہ
	choices[idx].متن = bat_متن_no;

    // update the individual batch file selections
    برائے (i = 1; i < ہدف_گنتی; ++i)
    {
	// Only make it active when the first item has a path and the vim.exe
	// or gvim.exe exists (there is a changefunc then).
	اگر (targets[0].batpath[0] != NUL
		&& choices[idx + i].changefunc != تہی)
	{
	    choices[idx + i].فعال = 1;
	    اگر (choices[idx + i].changefunc == change_bat_choice
		    && targets[i].batpath[0] != NUL)
	    {
		strcpy(targets[i].batpath, targets[0].batpath);
		سیٹ_bat_متن(idx + i, targets[i].batpath, targets[i].batname);
	    }
	}
	ورنہ
	    choices[idx + i].فعال = 0;
    }
}

/*
 * Initialize a choice for creating a batch file.
 */
    ساکن خالی
init_bat_choice(صحیح ہدف)
{
    حرف	*batpath = targets[ہدف].batpath;
    حرف	*oldbat = targets[ہدف].oldbat;
    حرف	*p;
    صحیح		i;

    choices[choice_گنتی].arg = ہدف;
    choices[choice_گنتی].installfunc = install_bat_choice;
    choices[choice_گنتی].فعال = 1;
    choices[choice_گنتی].متن = تہی;	// will be set below
    اگر (oldbat != تہی)
    {
	// A [g]vim.bat exists: Only choice is to overwrite it or not.
	choices[choice_گنتی].changefunc = toggle_bat_choice;
	*batpath = NUL;
	toggle_bat_choice(choice_گنتی);
    }
    ورنہ
    {
	اگر (default_bat_dir != تہی)
	    // Prefer using the same path as an existing .bat file.
	    strcpy(batpath, default_bat_dir);
	ورنہ
	{
	    // No [g]vim.bat exists: Write it to a directory in $PATH.  Use
	    // $WINDIR by default, if it's empty the first item in $PATH.
	    p = getenv("WINDIR");
	    اگر (p != تہی && *p != NUL)
		strcpy(batpath, p);
	    ورنہ
	    {
		p = getenv("PATH");
		اگر (p == تہی || *p == NUL)		// "cannot happen"
		    strcpy(batpath, "C:/Windows");
		ورنہ
		{
		    i = 0;
		    جب (*p != NUL && *p != ';')
			batpath[i++] = *p++;
		    batpath[i] = NUL;
		}
	    }
	}
	جوڑو_pathsep(batpath);
	سیٹ_bat_متن(choice_گنتی, batpath, targets[ہدف].batname);

	choices[choice_گنتی].changefunc = change_bat_choice;
    }
    ++choice_گنتی;
}

/*
 * Set up the choices for installing .bat files.
 * For these items "arg" is the index in targets[].
 */
    ساکن خالی
init_bat_choices(خالی)
{
    صحیح		i;

    // The first item is used to switch installing batch files on/off and
    // setting the default path.
    choices[choice_گنتی].متن = bat_متن_yes;
    choices[choice_گنتی].changefunc = change_main_bat_choice;
    choices[choice_گنتی].installfunc = تہی;
    choices[choice_گنتی].فعال = 1;
    choices[choice_گنتی].arg = 0;
    ++choice_گنتی;

    // Add items for each batch file target.  Only used when not disabled by
    // the first item.  When a .exe exists, don't offer to create a .bat.
    برائے (i = 1; i < ہدف_گنتی; ++i)
	اگر (targets[i].oldexe == تہی
		&& (targets[i].exenamearg[0] == 'g' ? has_gvim : has_vim))
	    init_bat_choice(i);
	ورنہ
	    جوڑو_dummy_choice();
}

/*
 * Install the vimrc file.
 */
    ساکن خالی
install_vimrc(صحیح idx غیر_مستعمل)
{
    FILE	*fd, *tfd;
    حرف	*fname;

    // If an old vimrc file exists, overwrite it.
    // Otherwise create a new one.
    اگر (*oldvimrc != NUL)
	fname = oldvimrc;
    ورنہ
	fname = vimrc;

    fd = fopen(fname, "w");
    اگر (fd == تہی)
    {
	لکھو("\nERROR: Cannot open \"%s\" for writing.\n", fname);
	واپس;
    }
    انتخاب (compat_choice)
    {
	حالت compat_vi:
		    fprintf(fd, "\" Vi compatible\n");
		    fprintf(fd, "set compatible\n");
		    توڑو;
	حالت compat_vim:
		    fprintf(fd, "\" Vim's default behavior\n");
		    fprintf(fd, "if &compatible\n");
		    fprintf(fd, "  set nocompatible\n");
		    fprintf(fd, "endif\n");
		    توڑو;
	حالت compat_some_enhancements:
		    fprintf(fd, "\" Vim with some enhancements\n");
		    fprintf(fd, "source $VIMRUNTIME/defaults.vim\n");
		    توڑو;
	حالت compat_all_enhancements:
		    fprintf(fd, "\" Vim with all enhancements\n");
		    fprintf(fd, "source $VIMRUNTIME/vimrc_example.vim\n");
		    توڑو;
    }
    انتخاب (remap_choice)
    {
	حالت remap_no:
		    توڑو;
	حالت remap_win:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Remap a few keys for Windows behavior\n");
		    fprintf(fd, "source $VIMRUNTIME/mswin.vim\n");
		    توڑو;
    }
    انتخاب (mouse_choice)
    {
	حالت mouse_xterm:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Mouse behavior (the Unix way)\n");
		    fprintf(fd, "behave xterm\n");
		    توڑو;
	حالت mouse_mswin:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Mouse behavior (the Windows way)\n");
		    fprintf(fd, "behave mswin\n");
		    توڑو;
	حالت mouse_default:
		    توڑو;
    }
    اگر ((tfd = fopen("diff.exe", "r")) != تہی)
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
    لکھو("%s has been written\n", fname);
}

    ساکن خالی
change_vimrc_choice(صحیح idx)
{
    اگر (choices[idx].installfunc != تہی)
    {
	// Switch to NOT change or create a vimrc file.
	اگر (*oldvimrc != NUL)
	    alloc_متن(idx, "Do NOT change startup file %s", oldvimrc);
	ورنہ
	    alloc_متن(idx, "Do NOT create startup file %s", vimrc);
	choices[idx].installfunc = تہی;
	choices[idx + 1].فعال = 0;
	choices[idx + 2].فعال = 0;
	choices[idx + 3].فعال = 0;
    }
    ورنہ
    {
	// Switch to change or create a vimrc file.
	اگر (*oldvimrc != NUL)
	    alloc_متن(idx, "Overwrite startup file %s with:", oldvimrc);
	ورنہ
	    alloc_متن(idx, "Create startup file %s with:", vimrc);
	choices[idx].installfunc = install_vimrc;
	choices[idx + 1].فعال = 1;
	choices[idx + 2].فعال = 1;
	choices[idx + 3].فعال = 1;
    }
}

/*
 * Change the choice how to run Vim.
 */
    ساکن خالی
change_تسلسل_choice(صحیح idx)
{
    compat_choice = get_choice(compat_choices, جدول_سائز(compat_choices));
    alloc_متن(idx, compat_متن, compat_choices[compat_choice]);
}

/*
 * Change the choice if keys are to be remapped.
 */
    ساکن خالی
change_remap_choice(صحیح idx)
{
    remap_choice = get_choice(remap_choices, جدول_سائز(remap_choices));
    alloc_متن(idx, remap_متن, remap_choices[remap_choice]);
}

/*
 * Change the choice how to select text.
 */
    ساکن خالی
change_mouse_choice(صحیح idx)
{
    mouse_choice = get_choice(mouse_choices, جدول_سائز(mouse_choices));
    alloc_متن(idx, mouse_متن, mouse_choices[mouse_choice]);
}

    ساکن خالی
init_vimrc_choices(خالی)
{
    // set path for a new _vimrc file (also when not used)
    strcpy(vimrc, installdir);
    strcpy(vimrc + runtimeidx, "_vimrc");

    // Set opposite value and then toggle it by calling change_vimrc_choice()
    اگر (*oldvimrc == NUL)
	choices[choice_گنتی].installfunc = تہی;
    ورنہ
	choices[choice_گنتی].installfunc = install_vimrc;
    choices[choice_گنتی].متن = تہی;
    change_vimrc_choice(choice_گنتی);
    choices[choice_گنتی].changefunc = change_vimrc_choice;
    choices[choice_گنتی].فعال = 1;
    ++choice_گنتی;

    // default way to run Vim
    alloc_متن(choice_گنتی, compat_متن, compat_choices[compat_choice]);
    choices[choice_گنتی].changefunc = change_تسلسل_choice;
    choices[choice_گنتی].installfunc = تہی;
    choices[choice_گنتی].فعال = (*oldvimrc == NUL);
    ++choice_گنتی;

    // Whether to remap keys
    alloc_متن(choice_گنتی, remap_متن , remap_choices[remap_choice]);
    choices[choice_گنتی].changefunc = change_remap_choice;
    choices[choice_گنتی].installfunc = تہی;
    choices[choice_گنتی].فعال = (*oldvimrc == NUL);
    ++choice_گنتی;

    // default way to use the mouse
    alloc_متن(choice_گنتی, mouse_متن, mouse_choices[mouse_choice]);
    choices[choice_گنتی].changefunc = change_mouse_choice;
    choices[choice_گنتی].installfunc = تہی;
    choices[choice_گنتی].فعال = (*oldvimrc == NUL);
    ++choice_گنتی;
}

    ساکن LONG
reg_create_کلید(
    HKEY جڑ,
    ثابت حرف *subkey,
    PHKEY ph_کلید,
    DWORD flag)
{
    DWORD disp;

    *ph_کلید = تہی;
    واپس Reg_Create_کلید_Ex(
		جڑ, subkey,
		0, تہی, REG_OPTION_NON_VOLATILE,
		flag | کلید_WRITE,
		تہی, ph_کلید, &disp);
}

    ساکن LONG
reg_سیٹ_string_قدر(
    HKEY h_کلید,
    ثابت حرف *قدر_name,
    ثابت حرف *ڈیٹا)
{
    واپس Reg_سیٹ_قدر_Ex(h_کلید, قدر_name, 0, REG_SZ,
				     (LPBYTE)ڈیٹا, (DWORD)(1 + لڑی_لمبائی(ڈیٹا)));
}

    ساکن LONG
reg_create_کلید_and_قدر(
    HKEY h_جڑ_کلید,
    ثابت حرف *subkey,
    ثابت حرف *قدر_name,
    ثابت حرف *ڈیٹا,
    DWORD flag)
{
    HKEY h_کلید;
    LONG lRet = reg_create_کلید(h_جڑ_کلید, subkey, &h_کلید, flag);

    اگر (ERROR_SUCCESS == lRet)
    {
	lRet = reg_سیٹ_string_قدر(h_کلید, قدر_name, ڈیٹا);
	Reg_Close_کلید(h_کلید);
    }
    واپس lRet;
}

    ساکن LONG
رجسٹر_inproc_server(
    HKEY h_جڑ_کلید,
    ثابت حرف *clsid,
    ثابت حرف *extname,
    ثابت حرف *module,
    ثابت حرف *threading_model,
    DWORD flag)
{
    CHAR subkey[BUFSIZE];
    LONG lRet;

    sprintf(subkey, "CLSID\\%s", clsid);
    lRet = reg_create_کلید_and_قدر(h_جڑ_کلید, subkey, تہی, extname, flag);
    اگر (ERROR_SUCCESS == lRet)
    {
	sprintf(subkey, "CLSID\\%s\\InProcServer32", clsid);
	lRet = reg_create_کلید_and_قدر(h_جڑ_کلید, subkey, تہی, module, flag);
	اگر (ERROR_SUCCESS == lRet)
	{
	    lRet = reg_create_کلید_and_قدر(h_جڑ_کلید, subkey,
					   "ThreadingModel", threading_model, flag);
	}
    }
    واپس lRet;
}

    ساکن LONG
رجسٹر_shellex(
    HKEY h_جڑ_کلید,
    ثابت حرف *clsid,
    ثابت حرف *name,
    ثابت حرف *exe_راستہ,
    DWORD flag)
{
    LONG lRet = reg_create_کلید_and_قدر(
	    h_جڑ_کلید,
	    "*\\shellex\\ContextMenuHandlers\\gvim",
	    تہی,
	    clsid,
	    flag);

    اگر (ERROR_SUCCESS == lRet)
    {
	lRet = reg_create_کلید_and_قدر(
		HKEY_LOCAL_MACHINE,
		"Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved",
		clsid,
		name,
		flag);

	اگر (ERROR_SUCCESS == lRet)
	{
	    lRet = reg_create_کلید_and_قدر(
		    HKEY_LOCAL_MACHINE,
		    "Software\\Vim\\Gvim",
		    "path",
		    exe_راستہ,
		    flag);
	}
    }
    واپس lRet;
}

    ساکن LONG
رجسٹر_openwith(
    HKEY h_جڑ_کلید,
    ثابت حرف *exe_راستہ,
    DWORD flag)
{
    حرف	exe_cmd[BUFSIZE];
    LONG	lRet;

    sprintf(exe_cmd, "\"%s\" \"%%1\"", exe_راستہ);
    lRet = reg_create_کلید_and_قدر(
	    h_جڑ_کلید,
	    "Applications\\gvim.exe\\shell\\edit\\command",
	    تہی,
	    exe_cmd,
	    flag);

    اگر (ERROR_SUCCESS == lRet)
    {
	صحیح i;
	ساکن ثابت حرف *openwith[] = {
		".htm\\OpenWithList\\gvim.exe",
		".vim\\OpenWithList\\gvim.exe",
		"*\\OpenWithList\\gvim.exe",
	};

	برائے (i = 0; ERROR_SUCCESS == lRet && i < (صحیح)ARRAYSIZE(openwith); i++)
	    lRet = reg_create_کلید_and_قدر(h_جڑ_کلید, openwith[i], تہی, "", flag);
    }

    واپس lRet;
}

    ساکن LONG
رجسٹر_uninstall(
    HKEY h_جڑ_کلید,
    ثابت حرف *appname,
    ثابت حرف *display_name,
    ثابت حرف *uninstall_string,
    ثابت حرف *display_icon,
    ثابت حرف *display_نسخہ,
    ثابت حرف *publisher)
{
    LONG lRet = reg_create_کلید_and_قدر(h_جڑ_کلید, appname,
			     "DisplayName", display_name, کلید_WOW64_64_کلید);

    اگر (ERROR_SUCCESS == lRet)
	lRet = reg_create_کلید_and_قدر(h_جڑ_کلید, appname,
		     "UninstallString", uninstall_string, کلید_WOW64_64_کلید);
    اگر (ERROR_SUCCESS == lRet)
	lRet = reg_create_کلید_and_قدر(h_جڑ_کلید, appname,
		     "DisplayIcon", display_icon, کلید_WOW64_64_کلید);
    اگر (ERROR_SUCCESS == lRet)
	lRet = reg_create_کلید_and_قدر(h_جڑ_کلید, appname,
		     "DisplayVersion", display_نسخہ, کلید_WOW64_64_کلید);
    اگر (ERROR_SUCCESS == lRet)
	lRet = reg_create_کلید_and_قدر(h_جڑ_کلید, appname,
		     "Publisher", publisher, کلید_WOW64_64_کلید);
    واپس lRet;
}

/*
 * Add some entries to the registry:
 * - to add "Edit with Vim" to the context * menu
 * - to add Vim to the "Open with..." list
 * - to uninstall Vim
 */
//ARGSUSED
    ساکن صحیح
install_registry(خالی)
{
    LONG	lRet = ERROR_SUCCESS;
    ثابت حرف	*vim_ext_ThreadingModel = "Apartment";
    ثابت حرف	*vim_ext_name = "Vim Shell Extension";
    ثابت حرف	*vim_ext_clsid = "{51EEE242-AD87-11d3-9C1E-0090278BBD99}";
    حرف	vim_exe_راستہ[MAX_راستہ];
    حرف	display_name[BUFSIZE];
    حرف	uninstall_string[BUFSIZE];
    حرف	icon_string[BUFSIZE];
    حرف	نسخہ_string[BUFSIZE];
    صحیح		i;
    صحیح		loop_گنتی = is_64bit_os() ? 2 : 1;
    DWORD	flag;

    sprintf(vim_exe_راستہ, "%s\\gvim.exe", installdir);

    اگر (install_popup)
    {
	حرف	    bufg[BUFSIZE];

	لکھو("Creating \"Edit with Vim\" popup menu entry\n");

	برائے (i = 0; i < loop_گنتی; i++)
	{
	    اگر (i == 0)
	    {
		sprintf(bufg, "%s\\" GVIMEXT32_راستہ, installdir);
		flag = کلید_WOW64_32_کلید;
	    }
	    ورنہ
	    {
		sprintf(bufg, "%s\\" GVIMEXT64_راستہ, installdir);
		flag = کلید_WOW64_64_کلید;
	    }

	    lRet = رجسٹر_inproc_server(
		    HKEY_CLASSES_جڑ, vim_ext_clsid, vim_ext_name,
		    bufg, vim_ext_ThreadingModel, flag);
	    اگر (ERROR_SUCCESS != lRet)
		واپس FAIL;
	    lRet = رجسٹر_shellex(
		    HKEY_CLASSES_جڑ, vim_ext_clsid, vim_ext_name,
		    vim_exe_راستہ, flag);
	    اگر (ERROR_SUCCESS != lRet)
		واپس FAIL;
	}
    }

    اگر (install_openwith)
    {
	لکھو("Creating \"Open with ...\" list entry\n");

	برائے (i = 0; i < loop_گنتی; i++)
	{
	    اگر (i == 0)
		flag = کلید_WOW64_32_کلید;
	    ورنہ
		flag = کلید_WOW64_64_کلید;

	    lRet = رجسٹر_openwith(HKEY_CLASSES_جڑ, vim_exe_راستہ, flag);
	    اگر (ERROR_SUCCESS != lRet)
		واپس FAIL;
	}
    }

    لکھو("Creating an uninstall entry\n");
    sprintf(display_name, "Vim " VIM_نسخہ_SHORT
#اگر_تعریف_شدہ _M_ARM64
	    " (arm64)"
#وگرنہ_اگر _M_X64
	    " (x64)"
#اگر_ختم
	    );

    // For the NSIS installer use the generated uninstaller.
    اگر (interactive)
	sprintf(uninstall_string, "%s\\uninstall.exe", installdir);
    ورنہ
	sprintf(uninstall_string, "%s\\uninstall-gui.exe", installdir);

    sprintf(icon_string, "%s\\gvim.exe,0", installdir);

    sprintf(نسخہ_string, VIM_نسخہ_SHORT "." VIM_نسخہ_PATCHLEVEL_STR);

    lRet = رجسٹر_uninstall(
	HKEY_LOCAL_MACHINE,
	"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Vim " VIM_نسخہ_SHORT,
	display_name,
	uninstall_string,
	icon_string,
	نسخہ_string,
	"The Vim Project");
    اگر (ERROR_SUCCESS != lRet)
	واپس FAIL;

    واپس OK;
}

    ساکن خالی
change_popup_choice(صحیح idx)
{
    اگر (install_popup == 0)
    {
	choices[idx].متن = "Install an entry for Vim in the popup menu for the right\n    mouse button so that you can edit any file with Vim";
	install_popup = 1;
    }
    ورنہ
    {
	choices[idx].متن = "Do NOT install an entry for Vim in the popup menu for the\n    right mouse button to edit any file with Vim";
	install_popup = 0;
    }
}

/*
 * Only add the choice for the popup menu entry when gvim.exe was found and
 * both gvimext.dll and regedit.exe exist.
 */
    ساکن خالی
init_popup_choice(خالی)
{
    ساخت stat	st;

    اگر (has_gvim
	    && (stat(GVIMEXT32_راستہ, &st) >= 0
		|| stat(GVIMEXT64_راستہ, &st) >= 0))
    {
	choices[choice_گنتی].changefunc = change_popup_choice;
	choices[choice_گنتی].installfunc = تہی;
	choices[choice_گنتی].فعال = 1;
	change_popup_choice(choice_گنتی);  // set the text
	++choice_گنتی;
    }
    ورنہ
	جوڑو_dummy_choice();
}

    ساکن خالی
change_openwith_choice(صحیح idx)
{
    اگر (install_openwith == 0)
    {
	choices[idx].متن = "Add Vim to the \"Open With...\" list in the popup menu for the right\n    mouse button so that you can edit any file with Vim";
	install_openwith = 1;
    }
    ورنہ
    {
	choices[idx].متن = "Do NOT add Vim to the \"Open With...\" list in the popup menu for the\n    right mouse button to edit any file with Vim";
	install_openwith = 0;
    }
}

/*
 * Only add the choice for the open-with menu entry when gvim.exe was found
 * and regedit.exe exist.
 */
    ساکن خالی
init_openwith_choice(خالی)
{
    اگر (has_gvim)
    {
	choices[choice_گنتی].changefunc = change_openwith_choice;
	choices[choice_گنتی].installfunc = تہی;
	choices[choice_گنتی].فعال = 1;
	change_openwith_choice(choice_گنتی);  // set the text
	++choice_گنتی;
    }
    ورنہ
	جوڑو_dummy_choice();
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
	ثابت حرف *shortcut_name,
	ثابت حرف *iconfile_راستہ,
	صحیح	    iconindex,
	ثابت حرف *shortcut_ہدف,
	ثابت حرف *shortcut_دلائل,
	ثابت حرف *workingdir
	)
{
    IShellLink	    *shelllink_ptr;
    HRESULT	    hres;
    IPersistFile	    *persistfile_ptr;

    // Initialize COM library
    hres = CoInitialize(تہی);
    اگر (!SUCCEEDED(hres))
    {
	لکھو("Error:  Could not open the COM library.  Not creating shortcut.\n");
	واپس FAIL;
    }

    // Instantiate a COM object for the ShellLink, store a pointer to it
    // in shelllink_ptr.
    hres = CoCreateInstance(&CLSID_ShellLink,
			   تہی,
			   CLSCTX_INPROC_SERVER,
			   &IID_IShellLink,
			   (خالی **) &shelllink_ptr);

    اگر (SUCCEEDED(hres)) // If the instantiation was successful...
    {
	// ...Then build a PersistFile interface for the ShellLink so we can
	// save it as a file after we build it.
	hres = shelllink_ptr->lpVtbl->QueryInterface(shelllink_ptr,
		&IID_IPersistFile, (خالی **) &persistfile_ptr);

	اگر (SUCCEEDED(hres))
	{
	    wchar_t wsz[BUFSIZE];

	    // translate the (possibly) multibyte shortcut filename to windows
	    // Unicode so it can be used as a file name.
	    Multi_Byte_مقصد_Wide_Char(CP_ACP, 0, shortcut_name, -1, wsz, حجم(wsz)/حجم(wsz[0]));

	    // set the attributes
	    shelllink_ptr->lpVtbl->سیٹ_راستہ(shelllink_ptr, shortcut_ہدف);
	    shelllink_ptr->lpVtbl->سیٹ_Working_Directory(shelllink_ptr,
								  workingdir);
	    shelllink_ptr->lpVtbl->سیٹ_Icon_Location(shelllink_ptr,
						    iconfile_راستہ, iconindex);
	    shelllink_ptr->lpVtbl->سیٹ_Arguments(shelllink_ptr, shortcut_دلائل);

	    // save the shortcut to a file and return the PersistFile object
	    persistfile_ptr->lpVtbl->Save(persistfile_ptr, wsz, 1);
	    persistfile_ptr->lpVtbl->Release(persistfile_ptr);
	}
	ورنہ
	{
	    لکھو("QueryInterface Error\n");
	    واپس FAIL;
	}

	// Return the ShellLink object
	shelllink_ptr->lpVtbl->Release(shelllink_ptr);
    }
    ورنہ
    {
	لکھو("CoCreateInstance Error - hres = %08x\n", (صحیح)hres);
	واپس FAIL;
    }

    واپس OK;
}

/*
 * Build a path to where we will put a specified link.
 *
 * Return 0 on error, non-zero on success
 */
   صحیح
build_link_name(
	حرف	   *link_راستہ,
	ثابت حرف *link_name,
	ثابت حرف *shell_folder_name)
{
    حرف	shell_folder_راستہ[MAX_راستہ];

    اگر (get_shell_folder_راستہ(shell_folder_راستہ, shell_folder_name) == FAIL)
    {
	لکھو("An error occurred while attempting to find the path to %s.\n",
							   shell_folder_name);
	واپس FAIL;
    }

    // Make sure the directory exists (create Start Menu\Programs\Vim).
    // Ignore errors if it already exists.
    vim_mkdir(shell_folder_راستہ, 0755);

    // build the path to the shortcut and the path to gvim.exe
    sprintf(link_راستہ, "%s\\%s.lnk", shell_folder_راستہ, link_name);

    واپس OK;
}

    ساکن صحیح
build_shortcut(
	ثابت حرف *name,	// Name of the shortcut
	ثابت حرف *exename,	// Name of the executable (e.g., vim.exe)
	ثابت حرف *دلائل,
	ثابت حرف *shell_folder,
	ثابت حرف *workingdir)
{
    حرف	executable_راستہ[BUFSIZE];
    حرف	link_name[BUFSIZE];

    sprintf(executable_راستہ, "%s\\%s", installdir, exename);

    اگر (build_link_name(link_name, name, shell_folder) == FAIL)
    {
	لکھو("An error has occurred.  A shortcut to %s will not be created %s.\n",
		name,
		*shell_folder == 'd' ? "on the desktop" : "in the Start menu");
	واپس FAIL;
    }

    // Create the shortcut:
    واپس create_shortcut(link_name, executable_راستہ, 0,
					   executable_راستہ, دلائل, workingdir);
}

/*
 * We used to use "homedir" as the working directory, but that is a bad choice
 * on multi-user systems.  However, not specifying a directory results in the
 * current directory to be c:\Windows\system32 on Windows 7. Use environment
 * variables instead.
 */
#تعریف_کرو WORKDIR "%HOMEDRIVE%%HOMEPATH%"

/*
 * Create shortcut(s) in the Start Menu\Programs\Vim folder.
 */
    ساکن خالی
install_آغاز_menu(صحیح idx غیر_مستعمل)
{
    need_uninstall_اندراج = 1;
    لکھو("Creating start menu\n");
    اگر (has_vim)
    {
	اگر (build_shortcut("Vim", "vim.exe", "",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    واپس;
	اگر (build_shortcut("Vim Read-only", "vim.exe", "-R",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    واپس;
	اگر (build_shortcut("Vim Diff", "vim.exe", "-d",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    واپس;
    }
    اگر (has_gvim)
    {
	اگر (build_shortcut("gVim", "gvim.exe", "",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    واپس;
	اگر (build_shortcut("gVim Easy", "gvim.exe", "-y",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    واپس;
	اگر (build_shortcut("gVim Read-only", "gvim.exe", "-R",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    واپس;
	اگر (build_shortcut("gVim Diff", "gvim.exe", "-d",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    واپس;
    }
    اگر (build_shortcut("Uninstall",
		interactive ? "uninstall.exe" : "uninstall-gui.exe", "",
					   VIM_STARTMENU, installdir) == FAIL)
	واپس;
    // For Windows NT the working dir of the vimtutor.bat must be right,
    // otherwise gvim.exe won't be found and using gvimbat doesn't work.
    اگر (build_shortcut("Vim tutor", "vimtutor.bat", "",
					   VIM_STARTMENU, installdir) == FAIL)
	واپس;
    اگر (build_shortcut("Help", has_gvim ? "gvim.exe" : "vim.exe", "-c h",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	واپس;
    {
	حرف	shell_folder_راستہ[BUFSIZE];

	// Creating the URL shortcut works a bit differently...
	اگر (get_shell_folder_راستہ(shell_folder_راستہ, VIM_STARTMENU) == FAIL)
	{
	    لکھو("Finding the path of the Start menu failed\n");
	    واپس ;
	}
	جوڑو_pathsep(shell_folder_راستہ);
	strcat(shell_folder_راستہ, "Vim Online.url");
	اگر (!Write_نجی_Profile_String("InternetShortcut", "URL",
				    "https://www.vim.org/", shell_folder_راستہ))
	{
	    لکھو("Creating the Vim online URL failed\n");
	    واپس;
	}
    }
}

    ساکن خالی
toggle_startmenu_choice(صحیح idx)
{
    اگر (choices[idx].installfunc == تہی)
    {
	choices[idx].installfunc = install_آغاز_menu;
	choices[idx].متن = "Add Vim to the Start menu";
    }
    ورنہ
    {
	choices[idx].installfunc = تہی;
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
    خالی
install_shortcut_gvim(صحیح idx)
{
    // Create shortcut(s) on the desktop
    اگر (choices[idx].arg)
    {
	(خالی)build_shortcut(icon_نام[0], "gvim.exe",
						      "", "desktop", WORKDIR);
	need_uninstall_اندراج = 1;
    }
}

    خالی
install_shortcut_evim(صحیح idx)
{
    اگر (choices[idx].arg)
    {
	(خالی)build_shortcut(icon_نام[1], "gvim.exe",
						    "-y", "desktop", WORKDIR);
	need_uninstall_اندراج = 1;
    }
}

    خالی
install_shortcut_gview(صحیح idx)
{
    اگر (choices[idx].arg)
    {
	(خالی)build_shortcut(icon_نام[2], "gvim.exe",
						    "-R", "desktop", WORKDIR);
	need_uninstall_اندراج = 1;
    }
}

    خالی
toggle_shortcut_choice(صحیح idx)
{
    حرف	*arg;

    اگر (choices[idx].installfunc == install_shortcut_gvim)
	arg = "gVim";
    ورنہ اگر (choices[idx].installfunc == install_shortcut_evim)
	arg = "gVim Easy";
    ورنہ
	arg = "gVim Read-only";
    اگر (choices[idx].arg)
    {
	choices[idx].arg = 0;
	alloc_متن(idx, "Do NOT create a desktop icon for %s", arg);
    }
    ورنہ
    {
	choices[idx].arg = 1;
	alloc_متن(idx, "Create a desktop icon for %s", arg);
    }
}

    ساکن خالی
init_startmenu_choice(خالی)
{
    // Start menu
    choices[choice_گنتی].changefunc = toggle_startmenu_choice;
    choices[choice_گنتی].installfunc = تہی;
    choices[choice_گنتی].فعال = 1;
    toggle_startmenu_choice(choice_گنتی);	// set the text
    ++choice_گنتی;
}

/*
 * Add the choice for the desktop shortcuts.
 */
    ساکن خالی
init_shortcut_choices(خالی)
{
    // Shortcut to gvim
    choices[choice_گنتی].متن = تہی;
    choices[choice_گنتی].arg = 0;
    choices[choice_گنتی].فعال = has_gvim;
    choices[choice_گنتی].changefunc = toggle_shortcut_choice;
    choices[choice_گنتی].installfunc = install_shortcut_gvim;
    toggle_shortcut_choice(choice_گنتی);
    ++choice_گنتی;

    // Shortcut to evim
    choices[choice_گنتی].متن = تہی;
    choices[choice_گنتی].arg = 0;
    choices[choice_گنتی].فعال = has_gvim;
    choices[choice_گنتی].changefunc = toggle_shortcut_choice;
    choices[choice_گنتی].installfunc = install_shortcut_evim;
    toggle_shortcut_choice(choice_گنتی);
    ++choice_گنتی;

    // Shortcut to gview
    choices[choice_گنتی].متن = تہی;
    choices[choice_گنتی].arg = 0;
    choices[choice_گنتی].فعال = has_gvim;
    choices[choice_گنتی].changefunc = toggle_shortcut_choice;
    choices[choice_گنتی].installfunc = install_shortcut_gview;
    toggle_shortcut_choice(choice_گنتی);
    ++choice_گنتی;
}

/*
 * Attempt to register OLE for Vim.
 */
   ساکن خالی
install_OLE_رجسٹر(خالی)
{
    حرف رجسٹر_command_string[BUFSIZE + 30];

    لکھو("\n--- Attempting to register Vim with OLE ---\n");
    لکھو("(There is no message whether this works or not.)\n");

    sprintf(رجسٹر_command_string, "\"%s\\gvim.exe\" -silent -register", installdir);
    system(رجسٹر_command_string);
}

/*
 * Remove the last part of directory "path[]" to get its parent, and put the
 * result in "to[]".
 */
    ساکن خالی
dir_remove_آخری(ثابت حرف *راستہ, حرف مقصد[MAX_راستہ])
{
    حرف c;
    طویل آخری_char_مقصد_copy;
    طویل راستہ_لمبائی = لڑی_لمبائی(راستہ);

    // skip the last character just in case it is a '\\'
    آخری_char_مقصد_copy = راستہ_لمبائی - 2;
    c = راستہ[آخری_char_مقصد_copy];

    جب (c != '\\')
    {
	آخری_char_مقصد_copy--;
	c = راستہ[آخری_char_مقصد_copy];
    }

    strncpy(مقصد, راستہ, (size_t)آخری_char_مقصد_copy);
    مقصد[آخری_char_مقصد_copy] = NUL;
}

    ساکن خالی
سیٹ_directories_متن(صحیح idx)
{
    صحیح vimfiles_dir_choice = choices[idx].arg;

    اگر (vimfiles_dir_choice == (صحیح)vimfiles_dir_none)
	alloc_متن(idx, "Do NOT create plugin directories%s", "");
    ورنہ
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
ساکن حرف *homedir = تہی;

    خالی
init_homedir(خالی)
{
    حرف    *var;
    حرف    buf[MAX_راستہ];

    اگر (homedir != تہی)
    {
	آزاد(homedir);
	homedir = تہی;
    }

    var = getenv("HOME");

    /*
     * Typically, $HOME is not defined on Windows, unless the user has
     * specifically defined it for Vim's sake.  However, on Windows NT
     * platforms, $HOMEDRIVE and $HOMEPATH are automatically defined for
     * each user.  Try constructing $HOME from these.
     */
    اگر (var == تہی || *var == NUL)
    {
	حرف	*homedrive, *homepath;

	homedrive = getenv("HOMEDRIVE");
	homepath = getenv("HOMEPATH");
	اگر (homepath == تہی || *homepath == NUL)
	    homepath = "\\";
	اگر (homedrive != تہی
		   && لڑی_لمبائی(homedrive) + لڑی_لمبائی(homepath) < حجم(buf))
	{
	    اگر (snprintf(buf, حجم(buf), "%s%s", homedrive, homepath) > 0
		&& buf[0] != NUL)
		var = buf;
	}
    }

    اگر (var == تہی)
	var = getenv("USERPROFILE");

    /*
     * Weird but true: $HOME may contain an indirect reference to another
     * variable, esp. "%USERPROFILE%".  Happens when $USERPROFILE isn't set
     * when $HOME is being set.
     */
    اگر (var != تہی && *var == '%')
    {
	حرف	*p;
	حرف	*قوت_نما;

	p = strchr(var + 1, '%');
	اگر (p != تہی)
	{
	    strncpy(buf, var + 1, p - (var + 1));
	    buf[p - (var + 1)] = NUL;
	    قوت_نما = getenv(buf);
	    اگر (قوت_نما != تہی && *قوت_نما != NUL
				&& لڑی_لمبائی(قوت_نما) + لڑی_لمبائی(p) < حجم(buf))
	    {
		snprintf(buf, حجم(buf), "%s%s", قوت_نما, p + 1);
		var = buf;
	    }
	}
    }

    اگر (var != تہی && *var == NUL)	// empty is same as not set
	var = تہی;

    اگر (var == تہی)
	homedir = تہی;
    ورنہ
	homedir = _strdup(var);
}

/*
 * Change the directory that the vim plugin directories will be created in:
 * $HOME, $VIM or nowhere.
 */
    ساکن خالی
change_directories_choice(صحیح idx)
{
    صحیح	    choice_گنتی = جدول_سائز(vimfiles_dir_choices);

    // Don't offer the $HOME choice if $HOME isn't set.
    اگر (homedir == تہی)
	--choice_گنتی;
    choices[idx].arg = get_choice(vimfiles_dir_choices, choice_گنتی);
    سیٹ_directories_متن(idx);
}

/*
 * Create the plugin directories...
 */
//ARGSUSED
    ساکن خالی
install_vimfilesdir(صحیح idx)
{
    صحیح i;
    صحیح vimfiles_dir_choice = choices[idx].arg;
    حرف *p;
    حرف vimdir_راستہ[MAX_راستہ];
    حرف vimfiles_راستہ[MAX_راستہ + 9];
    حرف tmp_dirname[BUFSIZE];

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
	    اگر (p == تہی) // No $VIM in path
		dir_remove_آخری(installdir, vimdir_راستہ);
	    ورنہ
		strcpy(vimdir_راستہ, p);
	    توڑو;
	}
	حالت vimfiles_dir_home:
	{
	    // Find the $HOME directory.  Its existence was already checked.
	    p = homedir;
	    اگر (p == تہی)
	    {
		لکھو("Internal error: $HOME is NULL\n");
		p = "c:\\";
	    }
	    strcpy(vimdir_راستہ, p);
	    توڑو;
	}
	حالت vimfiles_dir_none:
	{
	    // Do not create vim plugin directory.
	    واپس;
	}
    }

    // Now, just create the directory.	If it already exists, it will fail
    // silently.
    sprintf(vimfiles_راستہ, "%s\\vimfiles", vimdir_راستہ);
    vim_mkdir(vimfiles_راستہ, 0755);

    لکھو("Creating the following directories in \"%s\":\n", vimfiles_راستہ);
    برائے (i = 0; i < جدول_سائز(vimfiles_subdirs); i++)
    {
	sprintf(tmp_dirname, "%s\\%s", vimfiles_راستہ, vimfiles_subdirs[i]);
	لکھو("  %s", vimfiles_subdirs[i]);
	vim_mkdir(tmp_dirname, 0755);
    }
    لکھو("\n");
}

/*
 * Add the creation of runtime files to the setup sequence.
 */
    ساکن خالی
init_directories_choice(خالی)
{
    ساخت stat	st;
    حرف	tmp_dirname[BUFSIZE];
    حرف	*p;
    صحیح		vimfiles_dir_choice;

    choices[choice_گنتی].متن = alloc(150);
    choices[choice_گنتی].changefunc = change_directories_choice;
    choices[choice_گنتی].installfunc = install_vimfilesdir;
    choices[choice_گنتی].فعال = 1;

    // Check if the "compiler" directory already exists.  That's a good
    // indication that the plugin directories were already created.
    p = getenv("HOME");
    اگر (p != تہی)
    {
	vimfiles_dir_choice = (صحیح)vimfiles_dir_home;
	sprintf(tmp_dirname, "%s\\vimfiles\\compiler", p);
	اگر (stat(tmp_dirname, &st) == 0)
	    vimfiles_dir_choice = (صحیح)vimfiles_dir_none;
    }
    ورنہ
    {
	vimfiles_dir_choice = (صحیح)vimfiles_dir_vim;
	p = getenv("VIM");
	اگر (p == تہی)  // No $VIM in path, use the install dir.
	    dir_remove_آخری(installdir, tmp_dirname);
	ورنہ
	    strcpy(tmp_dirname, p);
	strcat(tmp_dirname, "\\vimfiles\\compiler");
	اگر (stat(tmp_dirname, &st) == 0)
	    vimfiles_dir_choice = (صحیح)vimfiles_dir_none;
    }

    choices[choice_گنتی].arg = vimfiles_dir_choice;
    سیٹ_directories_متن(choice_گنتی);
    ++choice_گنتی;
}

/*
 * Setup the choices and the default values.
 */
    ساکن خالی
تیاری_choices(خالی)
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

    ساکن خالی
print_cmd_سطر_help(خالی)
{
    لکھو("Vim installer non-interactive command line arguments:\n");
    لکھو("\n");
    لکھو("-create-batfiles  [vim gvim evim view gview vimdiff gvimdiff]\n");
    لکھو("    Create .bat files for Vim variants in the Windows directory.\n");
    لکھو("-create-vimrc\n");
    لکھو("    Create a default _vimrc file if one does not already exist.\n");
    لکھو("-vimrc-remap [no|win]\n");
    لکھو("    Remap keys when creating a default _vimrc file.\n");
    لکھو("-vimrc-behave [unix|mswin|default]\n");
    لکھو("    Set mouse behavior when creating a default _vimrc file.\n");
    لکھو("-vimrc-compat [vi|vim|defaults|all]\n");
    لکھو("    Set Vi compatibility when creating a default _vimrc file.\n");
    لکھو("-install-popup\n");
    لکھو("    Install the Edit-with-Vim context menu entry\n");
    لکھو("-install-openwith\n");
    لکھو("    Add Vim to the \"Open With...\" context menu list\n");
    لکھو("-add-start-menu");
    لکھو("    Add Vim to the start menu\n");
    لکھو("-install-icons");
    لکھو("    Create icons for gVim executables on the desktop\n");
    لکھو("-create-directories [vim|home]\n");
    لکھو("    Create runtime directories to drop plugins into; in the $VIM\n");
    لکھو("    or $HOME directory\n");
    لکھو("-register-OLE");
    لکھو("    Ignored\n");
    لکھو("\n");
}

/*
 * Setup installation choices based on command line switches
 */
    ساکن خالی
command_سطر_تیاری_choices(صحیح argc, حرف **argv)
{
    صحیح i, j;

    برائے (i = 1; i < argc; i++)
    {
	اگر (strcmp(argv[i], "-create-batfiles") == 0)
	{
	    اگر (i + 1 == argc)
		جاری;
	    جب (argv[i + 1][0] != '-' && i < argc)
	    {
		i++;
		برائے (j = 1; j < ہدف_گنتی; ++j)
		    اگر ((targets[j].exenamearg[0] == 'g' ? has_gvim : has_vim)
			    && strcmp(argv[i], targets[j].name) == 0)
		    {
			init_bat_choice(j);
			توڑو;
		    }
		اگر (j == ہدف_گنتی)
		    لکھو("%s is not a valid choice for -create-batfiles\n",
								     argv[i]);

		اگر (i + 1 == argc)
		    توڑو;
	    }
	}
	ورنہ اگر (strcmp(argv[i], "-create-vimrc") == 0)
	{
	    // Setup default vimrc choices.  If there is already a _vimrc file,
	    // it will NOT be overwritten.
	    init_vimrc_choices();
	}
	ورنہ اگر (strcmp(argv[i], "-vimrc-remap") == 0)
	{
	    اگر (i + 1 == argc)
		توڑو;
	    i++;
	    اگر (strcmp(argv[i], "no") == 0)
		remap_choice = remap_no;
	    ورنہ اگر (strcmp(argv[i], "win") == 0)
		remap_choice = remap_win;
	}
	ورنہ اگر (strcmp(argv[i], "-vimrc-behave") == 0)
	{
	    اگر (i + 1 == argc)
		توڑو;
	    i++;
	    اگر (strcmp(argv[i], "unix") == 0)
		mouse_choice = mouse_xterm;
	    ورنہ اگر (strcmp(argv[i], "mswin") == 0)
		mouse_choice = mouse_mswin;
	    ورنہ اگر (strcmp(argv[i], "default") == 0)
		mouse_choice = mouse_default;
	}
	ورنہ اگر (strcmp(argv[i], "-vimrc-compat") == 0)
	{
	    اگر (i + 1 == argc)
		توڑو;
	    i++;
	    اگر (strcmp(argv[i], "vi") == 0)
		compat_choice = compat_vi;
	    ورنہ اگر (strcmp(argv[i], "vim") == 0)
		compat_choice = compat_vim;
	    ورنہ اگر (strcmp(argv[i], "defaults") == 0)
		compat_choice = compat_some_enhancements;
	    ورنہ اگر (strcmp(argv[i], "all") == 0)
		compat_choice = compat_all_enhancements;
	}
	ورنہ اگر (strcmp(argv[i], "-install-popup") == 0)
	{
	    init_popup_choice();
	}
	ورنہ اگر (strcmp(argv[i], "-install-openwith") == 0)
	{
	    init_openwith_choice();
	}
	ورنہ اگر (strcmp(argv[i], "-add-start-menu") == 0)
	{
	    init_startmenu_choice();
	}
	ورنہ اگر (strcmp(argv[i], "-install-icons") == 0)
	{
	    init_shortcut_choices();
	}
	ورنہ اگر (strcmp(argv[i], "-create-directories") == 0)
	{
	    صحیح vimfiles_dir_choice = (صحیح)vimfiles_dir_none;

	    init_directories_choice();
	    اگر (i + 1 < argc && argv[i + 1][0] != '-')
	    {
		i++;
		اگر (strcmp(argv[i], "vim") == 0)
		    vimfiles_dir_choice = (صحیح)vimfiles_dir_vim;
		ورنہ اگر (strcmp(argv[i], "home") == 0)
		{
		    اگر (homedir == تہی)  // No $HOME in environment
			vimfiles_dir_choice = (صحیح)vimfiles_dir_none;
		    ورنہ
			vimfiles_dir_choice = (صحیح)vimfiles_dir_home;
		}
		ورنہ
		{
		    لکھو("Unknown argument for -create-directories: %s\n",
								     argv[i]);
		    print_cmd_سطر_help();
		}
	    }
	    ورنہ // No choice specified, default to vim directory
		vimfiles_dir_choice = (صحیح)vimfiles_dir_vim;
	    choices[choice_گنتی - 1].arg = vimfiles_dir_choice;
	}
	ورنہ اگر (strcmp(argv[i], "-register-OLE") == 0)
	{
	    // This is always done when gvim is found
	}
	ورنہ // Unknown switch
	{
	    لکھو("Got unknown argument argv[%d] = %s\n", i, argv[i]);
	    print_cmd_سطر_help();
	}
    }
}


/*
 * Show a few screens full of helpful information.
 */
    ساکن خالی
show_help(خالی)
{
    ساکن حرف *(اشیاء[]) =
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
تہی
    };
    صحیح		i;
    صحیح		c;

    rewind(stdin);
    لکھو("\n");
    برائے (i = 0; اشیاء[i] != تہی; ++i)
    {
	puts(اشیاء[i]);
	لکھو("Hit Enter to continue, b (back) or q (quit help): ");
	c = getchar();
	rewind(stdin);
	اگر (c == 'b' || c == 'B')
	{
	    اگر (i == 0)
		--i;
	    ورنہ
		i -= 2;
	}
	اگر (c == 'q' || c == 'Q')
	    توڑو;
	لکھو("\n");
    }
}

/*
 * Install the choices.
 */
    ساکن خالی
install(خالی)
{
    صحیح		i;

    // Install the selected choices.
    برائے (i = 0; i < choice_گنتی; ++i)
	اگر (choices[i].installfunc != تہی && choices[i].فعال)
	    (choices[i].installfunc)(i);

    // Add some entries to the registry, if needed.
    اگر (install_popup
	    || install_openwith
	    || (need_uninstall_اندراج && interactive)
	    || !interactive)
	install_registry();

    // Register gvim with OLE.
    اگر (has_gvim)
	install_OLE_رجسٹر();
}

/*
 * request_choice
 */
    ساکن خالی
request_choice(خالی)
{
    صحیح		      i;

    لکھو("\n\nInstall will do for you:\n");
    برائے (i = 0; i < choice_گنتی; ++i)
	اگر (choices[i].فعال)
	    لکھو("%2d  %s\n", i + 1, choices[i].متن);
    لکھو("To change an item, enter its number\n\n");
    لکھو("Enter item number, h (help), d (do it) or q (quit): ");
}

    صحیح
مین(صحیح argc, حرف **argv)
{
    صحیح		i;
    حرف	buf[BUFSIZE];

    /*
     * Run interactively if there are no command line arguments.
     */
    اگر (argc > 1)
	interactive = 0;
    ورنہ
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
	باہر_نکلو(0);
    }

    لکھو("This program sets up the installation of Vim "
						   VIM_نسخہ_MEDIUM "\n\n");

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
	تیاری_choices();

	// Let the user change choices and finally install (or quit).
	برائے (;;)
	{
	    request_choice();
	    rewind(stdin);
	    اگر (scanf("%99s", buf) == 1)
	    {
		اگر (ہندسہ_ہے((غیرعلامتی حرف)buf[0]))
		{
		    // Change a choice.
		    i = atoi(buf);
		    اگر (i > 0 && i <= choice_گنتی && choices[i - 1].فعال)
			(choices[i - 1].changefunc)(i - 1);
		    ورنہ
			لکھو("\nIllegal choice\n");
		}
		ورنہ اگر (buf[0] == 'h' || buf[0] == 'H')
		{
		    // Help
		    show_help();
		}
		ورنہ اگر (buf[0] == 'd' || buf[0] == 'D')
		{
		    // Install!
		    install();
		    لکھو("\nThat finishes the installation.  Happy Vimming!\n");
		    توڑو;
		}
		ورنہ اگر (buf[0] == 'q' || buf[0] == 'Q')
		{
		    // Quit
		    لکھو("\nExiting without anything done\n");
		    توڑو;
		}
		ورنہ
		    لکھو("\nIllegal choice\n");
	    }
	}
	لکھو("\n");
	myexit(0);
    }
    ورنہ
    {
	/*
	 * Run non-interactive - setup according to the command line switches
	 */
	command_سطر_تیاری_choices(argc, argv);
	install();

	// Avoid that the user has to hit Enter, just wait a little bit to
	// allow reading the messages.
	sleep(2);
    }

    واپس 0;
}
