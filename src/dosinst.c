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
#నిర్వచించు DOSINST
#అన్తర్భావ "dosinst.h"
#అన్తర్భావ <io.h>

#నిర్వచించు GVIMEXT64_మార్గం	    "GvimExt64\\gvimext.dll"
#నిర్వచించు GVIMEXT32_మార్గం	    "GvimExt32\\gvimext.dll"

// Macro to do an error check I was typing over and over
#నిర్వచించు CHECK_REG_ERROR(సంకేతం_2) \
    చేయి { \
	ఒకవేళ (సంకేతం_2 != ERROR_SUCCESS) \
	{ \
	    రాయి("%ld error number:  %ld\n", (దీర్ఘ)__LINE__, (దీర్ఘ)సంకేతం_2); \
	    తిరిగి 1; \
	} \
    } అయితే (0)

పూర్ణాంకం	has_vim = 0;		// installable vim.exe exists
పూర్ణాంకం	has_gvim = 0;		// installable gvim.exe exists

అక్షరం	oldvimrc[BUFSIZE];	// name of existing vimrc file
అక్షరం	vimrc[BUFSIZE];		// name of vimrc file to create

అక్షరం	*default_bat_dir = ఖాళీ;  // when not NULL, use this as the default
				  // directory to write .bat files in
అక్షరం	*default_vim_dir = ఖాళీ;  // when not NULL, use this as the default
				  // install dir for NSIS

/*
 * Structure used for each choice the user can make.
 */
నిర్మాణం choice
{
    పూర్ణాంకం	    చురుకైన;			// non-zero when choice is active
    అక్షరం    *పాఠ్యం;			// text displayed for this choice
    శూన్య    (*changefunc)(పూర్ణాంకం idx);	// function to change this choice
    పూర్ణాంకం	    arg;			// argument for function
    శూన్య    (*installfunc)(పూర్ణాంకం idx);	// function to install this choice
};

నిర్మాణం choice	choices[30];		// choices the user can make
పూర్ణాంకం		choice_లెక్క = 0;	// number of choices available

#నిర్వచించు పట్టిక_కొలత(s)	(పూర్ణాంకం)ARRAYSIZE(s)

గణన
{
    compat_vi = 1,
    compat_vim,
    compat_some_enhancements,
    compat_all_enhancements
};
అక్షరం	*(compat_choices[]) =
{
    "\nChoose the default way to run Vim:",
    "Vi compatible",
    "Vim default",
    "with some Vim enhancements",
    "with syntax highlighting and other features switched on",
};
పూర్ణాంకం	compat_choice = (పూర్ణాంకం)compat_all_enhancements;
అక్షరం	*compat_పాఠ్యం = "- run Vim %s";

గణన
{
    remap_no = 1,
    remap_win
};
అక్షరం	*(remap_choices[]) =
{
    "\nChoose:",
    "Do not remap keys for Windows behavior",
    "Remap a few keys for Windows behavior (CTRL-V, CTRL-C, CTRL-F, etc)",
};
పూర్ణాంకం	remap_choice = (పూర్ణాంకం)remap_no;
అక్షరం	*remap_పాఠ్యం = "- %s";

గణన
{
    mouse_xterm = 1,
    mouse_mswin,
    mouse_default
};
అక్షరం	*(mouse_choices[]) =
{
    "\nChoose the way how Vim uses the mouse:",
    "right button extends selection (the Unix way)",
    "right button has a popup menu, left button starts select mode (the Windows way)",
    "right button has a popup menu, left button starts visual mode",
};
పూర్ణాంకం	mouse_choice = (పూర్ణాంకం)mouse_default;
అక్షరం	*mouse_పాఠ్యం = "- The mouse %s";

గణన
{
    vimfiles_dir_none = 1,
    vimfiles_dir_vim,
    vimfiles_dir_home
};
స్థిర అక్షరం *(vimfiles_dir_choices[]) =
{
    "\nCreate plugin directories:",
    "No",
    "In the VIM directory",
    "In your HOME directory",
};

// non-zero when selected to install the popup menu entry.
స్థిర పూర్ణాంకం	install_popup = 0;

// non-zero when selected to install the "Open with" entry.
స్థిర పూర్ణాంకం	install_openwith = 0;

// non-zero when need to add an uninstall entry in the registry
స్థిర పూర్ణాంకం	need_uninstall_నమోదు = 0;

/*
 * Definitions of the directory name (under $VIM) of the vimfiles directory
 * and its subdirectories:
 */
స్థిర అక్షరం	*(vimfiles_subdirs[]) =
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
    స్థిర పూర్ణాంకం
get_choice(అక్షరం **పట్టిక, పూర్ణాంకం entries)
{
    పూర్ణాంకం		answer;
    పూర్ణాంకం		idx;
    అక్షరం	dummy[100];

    చేయి
    {
	కొరకు (idx = 0; idx < entries; ++idx)
	{
	    ఒకవేళ (idx)
		రాయి("%2d  ", idx);
	    puts(పట్టిక[idx]);
	}
	రాయి("Choice: ");
	ఒకవేళ (scanf("%d", &answer) != 1)
	{
	    scanf("%99s", dummy);
	    answer = 0;
	}
    }
    అయితే (answer < 1 || answer >= entries);

    తిరిగి answer;
}

/*
 * Check if the user unpacked the archives properly.
 * Sets "runtimeidx".
 */
    స్థిర శూన్య
check_unpack(శూన్య)
{
    అక్షరం	buf[BUFSIZE];
    FILE	*fd;
    నిర్మాణం stat	st;

    // check for presence of the correct version number in installdir[]
    runtimeidx = అక్షరశ్రేణి_పొడవు(installdir) - అక్షరశ్రేణి_పొడవు(VIM_సంస్కరణ_NODOT);
    ఒకవేళ (runtimeidx <= 0
	    || stricmp(installdir + runtimeidx, VIM_సంస్కరణ_NODOT) != 0
	    || (installdir[runtimeidx - 1] != '/'
		&& installdir[runtimeidx - 1] != '\\'))
    {
	రాయి("ERROR: Install program not in directory \"%s\"\n",
		VIM_సంస్కరణ_NODOT);
	రాయి("This program can only work when it is located in its original directory\n");
	myexit(1);
    }

    // check if filetype.vim is present, which means the runtime archive has
    // been unpacked
    sprintf(buf, "%s\\filetype.vim", installdir);
    ఒకవేళ (stat(buf, &st) < 0)
    {
	రాయి("ERROR: Cannot find filetype.vim in \"%s\"\n", installdir);
	రాయి("It looks like you did not unpack the runtime archive.\n");
	రాయి("You must unpack the runtime archive \"%srt.zip\" before installing.\n",
		VIM_సంస్కరణ_NODOT);
	myexit(1);
    }

    // Check if vim.exe or gvim.exe is in the current directory.
    ఒకవేళ ((fd = fopen("gvim.exe", "r")) != ఖాళీ)
    {
	fclose(fd);
	has_gvim = 1;
    }
    ఒకవేళ ((fd = fopen("vim.exe", "r")) != ఖాళీ)
    {
	fclose(fd);
	has_vim = 1;
    }
    ఒకవేళ (!has_gvim && !has_vim)
    {
	రాయి("ERROR: Cannot find any Vim executables in \"%s\"\n\n",
								  installdir);
	myexit(1);
    }
}

/*
 * Compare paths "p[plen]" to "q[qlen]".  Return 0 if they match.
 * Ignores case and differences between '/' and '\'.
 * "plen" and "qlen" can be negative, strlen() is used then.
 */
    స్థిర పూర్ణాంకం
pathcmp(అక్షరం *p, పూర్ణాంకం plen, అక్షరం *q, పూర్ణాంకం qlen)
{
    పూర్ణాంకం		i;

    ఒకవేళ (plen < 0)
	plen = అక్షరశ్రేణి_పొడవు(p);
    ఒకవేళ (qlen < 0)
	qlen = అక్షరశ్రేణి_పొడవు(q);
    కొరకు (i = 0; ; ++i)
    {
	// End of "p": check if "q" also ends or just has a slash.
	ఒకవేళ (i == plen)
	{
	    ఒకవేళ (i == qlen)  //సరిపోలిక
		తిరిగి 0;
	    ఒకవేళ (i == qlen - 1 && (q[i] == '\\' || q[i] == '/'))
		తిరిగి 0;   // match with trailing slash
	    తిరిగి 1;	    // no match
	}

	// End of "q": check if "p" also ends or just has a slash.
	ఒకవేళ (i == qlen)
	{
	    ఒకవేళ (i == plen)  //సరిపోలిక
		తిరిగి 0;
	    ఒకవేళ (i == plen - 1 && (p[i] == '\\' || p[i] == '/'))
		తిరిగి 0;   // match with trailing slash
	    తిరిగి 1;	    // no match
	}

	ఒకవేళ (!(mytoupper(p[i]) == mytoupper(q[i])
		|| ((p[i] == '/' || p[i] == '\\')
		    && (q[i] == '/' || q[i] == '\\'))))
	    తిరిగి 1;	    // no match
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
    స్థిర శూన్య
findoldfile(అక్షరం **destination)
{
    అక్షరం	*bp = *destination;
    size_t	indir_l = అక్షరశ్రేణి_పొడవు(installdir);
    అక్షరం	*cp;
    అక్షరం	*tmpname;
    అక్షరం	*farname;

    /*
     * No action needed if exe not found or not in this directory.
     */
    ఒకవేళ (bp == ఖాళీ || strnicmp(bp, installdir, indir_l) != 0)
	తిరిగి;
    cp = bp + indir_l;
    ఒకవేళ (strchr("/\\", *cp++) == ఖాళీ
	    || strchr(cp, '\\') != ఖాళీ
	    || strchr(cp, '/') != ఖాళీ)
	తిరిగి;

    tmpname = alloc(అక్షరశ్రేణి_పొడవు(cp) + 1);
    strcpy(tmpname, cp);
    tmpname[అక్షరశ్రేణి_పొడవు(tmpname) - 1] = 'x';	// .exe -> .exx

    ఒకవేళ (access(tmpname, 0) == 0)
    {
	రాయి("\nERROR: %s and %s clash.  Remove or rename %s.\n",
	    tmpname, cp, tmpname);
	myexit(1);
    }

    ఒకవేళ (rename(cp, tmpname) != 0)
    {
	రాయి("\nERROR: failed to rename %s to %s: %s\n",
	    cp, tmpname, strerror(0));
	myexit(1);
    }

    farname = searchpath_save(cp);

    ఒకవేళ (rename(tmpname, cp) != 0)
    {
	రాయి("\nERROR: failed to rename %s back to %s: %s\n",
	    tmpname, cp, strerror(0));
	myexit(1);
    }

    విడుదల(*destination);
    విడుదల(tmpname);
    *destination = farname;
}

/*
 * Check if there is a vim.[exe|bat|, gvim.[exe|bat|, etc. in the path.
 * When "check_bat_only" is TRUE, only find "default_bat_dir".
 */
    స్థిర శూన్య
కనుగొను_bat_exe(పూర్ణాంకం check_bat_only)
{
    పూర్ణాంకం		i;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    కొరకు (i = 1; i < లక్ష్యం_లెక్క; ++i)
    {
	targets[i].oldbat = searchpath_save(targets[i].batname);
	ఒకవేళ (!check_bat_only)
	    targets[i].oldexe = searchpath_save(targets[i].exename);

	ఒకవేళ (default_bat_dir == ఖాళీ && targets[i].oldbat != ఖాళీ)
	{
	    default_bat_dir = alloc(అక్షరశ్రేణి_పొడవు(targets[i].oldbat) + 1);
	    strcpy(default_bat_dir, targets[i].oldbat);
	    remove_తోక(default_bat_dir);
	}
	ఒకవేళ (check_bat_only && targets[i].oldbat != ఖాళీ)
	{
	    విడుదల(targets[i].oldbat);
	    targets[i].oldbat = ఖాళీ;
	}
    }

    mch_chdir(installdir);
}

/*
 * Get the value of $VIMRUNTIME or $VIM and write it in $TEMP/vimini.ini, so
 * that NSIS can read it.
 * When not set, use the directory of a previously installed Vim.
 */
    స్థిర శూన్య
get_vim_env(శూన్య)
{
    అక్షరం	*vim;
    అక్షరం	buf[BUFSIZE];
    FILE	*fd;
    అక్షరం	fname[BUFSIZE];

    // First get $VIMRUNTIME.  If it's set, remove the tail.
    vim = getenv("VIMRUNTIME");
    ఒకవేళ (vim != ఖాళీ && *vim != 0 && అక్షరశ్రేణి_పొడవు(vim) < పరిమాణం(buf))
    {
	strcpy(buf, vim);
	remove_తోక(buf);
	vim = buf;
    }
    లేకపోతే
    {
	vim = getenv("VIM");
	ఒకవేళ (vim == ఖాళీ || *vim == 0)
	{
	    // Use the directory from an old uninstall entry.
	    ఒకవేళ (default_vim_dir != ఖాళీ)
		vim = default_vim_dir;
	    లేకపోతే
		// Let NSIS know there is no default, it should use
		// $PROGRAMFILES.
		vim = "";
	}
    }

    // NSIS also uses GetTempPath(), thus we should get the same directory
    // name as where NSIS will look for vimini.ini.
    Get_తాత్కాలిక_మార్గం(పరిమాణం(fname) - 12, fname);
    కూడు_pathsep(fname);
    strcat(fname, "vimini.ini");

    fd = fopen(fname, "w");
    ఒకవేళ (fd != ఖాళీ)
    {
	// Make it look like an .ini file, so that NSIS can read it with a
	// ReadINIStr command.
	fprintf(fd, "[vimini]\n");
	fprintf(fd, "dir=\"%s\"\n", vim);
	fclose(fd);
    }
    లేకపోతే
    {
	రాయి("Failed to open %s\n", fname);
	sleep(2);
    }
}

స్థిర పూర్ణాంకం అంకె_windows;

/*
 * Callback used for EnumWindows():
 * Count the window if the title looks like it is for the uninstaller.
 */
//ARGSUSED
    స్థిర BOOL CALLBACK
కిటికీ_cb(HWND hwnd, LPARAM lparam వాడని)
{
    అక్షరం title[256];

    title[0] = 0;
    Get_కిటికీ_పాఠ్యం(hwnd, title, 256);
    ఒకవేళ (అక్షరశ్రేణి_శోధన(title, "Vim ") != ఖాళీ && అక్షరశ్రేణి_శోధన(title, " Uninstall") != ఖాళీ)
	++అంకె_windows;
    తిరిగి TRUE;
}

/*
 * Run the uninstaller silently.
 */
    స్థిర పూర్ణాంకం
నిరంతరం_silent_uninstall(అక్షరం *uninst_exe)
{
    అక్షరం    vimrt_dir[BUFSIZE];
    అక్షరం    తాత్కాలిక_uninst[BUFSIZE];
    అక్షరం    తాత్కాలిక_dir[MAX_మార్గం];
    అక్షరం    buf[BUFSIZE * 2 + 10];
    పూర్ణాంకం	    i;
    DWORD   tick;

    strcpy(vimrt_dir, uninst_exe);
    remove_తోక(vimrt_dir);

    ఒకవేళ (!Get_తాత్కాలిక_మార్గం(పరిమాణం(తాత్కాలిక_dir), తాత్కాలిక_dir))
	తిరిగి FAIL;

    // Copy the uninstaller to a temporary exe.
    tick = Get_Tick_లెక్క();
    కొరకు (i = 0; ; i++)
    {
	sprintf(తాత్కాలిక_uninst, "%s\\vimun%04X.exe", తాత్కాలిక_dir,
					  (గుర్తులేని పూర్ణాంకం)((i + tick) & 0xFFFF));
	ఒకవేళ (CopyFile(uninst_exe, తాత్కాలిక_uninst, TRUE))
	    ఆపు;
	ఒకవేళ (Get_చివరి_Error() != ERROR_FILE_EXISTS)
	    తిరిగి FAIL;
	ఒకవేళ (i == 65535)
	    తిరిగి FAIL;
    }

    // Run the copied uninstaller silently.
    ఒకవేళ (strchr(తాత్కాలిక_uninst, ' ') != ఖాళీ)
	sprintf(buf, "\"%s\" /S _?=%s", తాత్కాలిక_uninst, vimrt_dir);
    లేకపోతే
	sprintf(buf, "%s /S _?=%s", తాత్కాలిక_uninst, vimrt_dir);
    నిరంతరం_command(buf);

    DeleteFile(తాత్కాలిక_uninst);
    తిరిగి OK;
}

/*
 * Check for already installed Vims.
 * Return non-zero when found one.
 */
    స్థిర పూర్ణాంకం
uninstall_check(పూర్ణాంకం skip_question)
{
    HKEY	కీలకం_handle;
    HKEY	uninstall_కీలకం_handle;
    అక్షరం	*uninstall_కీలకం = "software\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
    అక్షరం	subkey_name_buff[BUFSIZE];
    అక్షరం	తాత్కాలిక_string_బఫరు[BUFSIZE-2];
    DWORD	local_bufsize;
    FILETIME	తాత్కాలిక_pfiletime;
    DWORD	కీలకం_సూచీ;
    అక్షరం	ఆదానం;
    దీర్ఘ	సంకేతం_2;
    DWORD	విలువ_type;
    DWORD	orig_అంకె_కీలకాలు;
    DWORD	new_అంకె_కీలకాలు;
    DWORD	allow_silent;
    పూర్ణాంకం		foundone = 0;

    సంకేతం_2 = Reg_Open_కీలకం_Ex(HKEY_LOCAL_MACHINE, uninstall_కీలకం, 0,
				     కీలకం_WOW64_64_కీలకం | కీలకం_READ, &కీలకం_handle);
    CHECK_REG_ERROR(సంకేతం_2);

    కీలకం_సూచీ = 0;
    అయితే (TRUE)
    {
	local_bufsize = పరిమాణం(subkey_name_buff);
	ఒకవేళ (Reg_Enum_కీలకం_Ex(కీలకం_handle, కీలకం_సూచీ, subkey_name_buff, &local_bufsize,
		ఖాళీ, ఖాళీ, ఖాళీ, &తాత్కాలిక_pfiletime) == ERROR_NO_మరిన్ని_వస్తువులు)
	    ఆపు;

	ఒకవేళ (అక్షరశ్రేణి_పోలిక("Vim", subkey_name_buff, 3) == 0)
	{
	    // Open the key named Vim*
	    సంకేతం_2 = Reg_Open_కీలకం_Ex(కీలకం_handle, subkey_name_buff, 0,
			   కీలకం_WOW64_64_కీలకం | కీలకం_READ, &uninstall_కీలకం_handle);
	    CHECK_REG_ERROR(సంకేతం_2);

	    // get the DisplayName out of it to show the user
	    local_bufsize = పరిమాణం(తాత్కాలిక_string_బఫరు);
	    సంకేతం_2 = Reg_Query_విలువ_Ex(uninstall_కీలకం_handle, "displayname", 0,
		    &విలువ_type, (LPBYTE)తాత్కాలిక_string_బఫరు,
		    &local_bufsize);
	    CHECK_REG_ERROR(సంకేతం_2);

	    allow_silent = 0;
	    ఒకవేళ (skip_question)
	    {
		DWORD varsize = పరిమాణం(DWORD);

		Reg_Query_విలువ_Ex(uninstall_కీలకం_handle, "AllowSilent", 0,
			&విలువ_type, (LPBYTE)&allow_silent,
			&varsize);
	    }

	    foundone = 1;
	    రాయి("\n*********************************************************\n");
	    రాయి("Vim Install found what looks like an existing Vim version.\n");
	    రాయి("The name of the entry is:\n");
	    రాయి("\n        \"%s\"\n\n", తాత్కాలిక_string_బఫరు);

	    రాయి("Installing the new version will disable part of the existing version.\n");
	    రాయి("(The batch files used in a console and the \"Edit with Vim\" entry in\n");
	    రాయి("the popup menu will use the new version)\n");

	    ఒకవేళ (skip_question)
		రాయి("\nRunning uninstall program for \"%s\"\n", తాత్కాలిక_string_బఫరు);
	    లేకపోతే
		రాయి("\nDo you want to uninstall \"%s\" now?\n(y)es/(n)o)  ", తాత్కాలిక_string_బఫరు);
	    fflush(stdout);

	    // get the UninstallString
	    local_bufsize = పరిమాణం(తాత్కాలిక_string_బఫరు);
	    సంకేతం_2 = Reg_Query_విలువ_Ex(uninstall_కీలకం_handle, "uninstallstring", 0,
		    &విలువ_type, (LPBYTE)తాత్కాలిక_string_బఫరు, &local_bufsize);
	    CHECK_REG_ERROR(సంకేతం_2);

	    // Remember the directory, it is used as the default for NSIS.
	    default_vim_dir = alloc(అక్షరశ్రేణి_పొడవు(తాత్కాలిక_string_బఫరు) + 1);
	    strcpy(default_vim_dir, తాత్కాలిక_string_బఫరు);
	    remove_తోక(default_vim_dir);
	    remove_తోక(default_vim_dir);

	    ఆదానం = 'n';
	    చేయి
	    {
		ఒకవేళ (ఆదానం != 'n')
		    రాయి("%c is an invalid reply.  Please enter either 'y' or 'n'\n", ఆదానం);

		ఒకవేళ (skip_question)
		    ఆదానం = 'y';
		లేకపోతే
		{
		    rewind(stdin);
		    scanf("%c", &ఆదానం);
		}
		ఎంపిక (ఆదానం)
		{
		    స్థితి 'y':
		    స్థితి 'Y':
			// save the number of uninstall keys so we can know if
			// it changed
			Reg_Query_Info_కీలకం(కీలకం_handle, ఖాళీ, ఖాళీ, ఖాళీ,
					     &orig_అంకె_కీలకాలు, ఖాళీ, ఖాళీ, ఖాళీ,
						      ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ);

			// Find existing .bat files before deleting them.
			కనుగొను_bat_exe(TRUE);

			ఒకవేళ (allow_silent)
			{
			    ఒకవేళ (నిరంతరం_silent_uninstall(తాత్కాలిక_string_బఫరు)
								    == FAIL)
				allow_silent = 0; // Retry with non silent.
			}
			ఒకవేళ (!allow_silent)
			{
			    // Execute the uninstall program.  Put it in double
			    // quotes if there is an embedded space.
			    {
				అక్షరం buf[BUFSIZE];

				ఒకవేళ (strchr(తాత్కాలిక_string_బఫరు, ' ') != ఖాళీ)
				    sprintf(buf, "\"%s\"", తాత్కాలిక_string_బఫరు);
				లేకపోతే
				    strcpy(buf, తాత్కాలిక_string_బఫరు);
				నిరంతరం_command(buf);
			    }

			    // Count the number of windows with a title that
			    // match the installer, so that we can check when
			    // it's done.  The uninstaller copies itself,
			    // executes the copy and exits, thus we can't wait
			    // for the process to finish.
			    sleep(1);  // wait for uninstaller to start up
			    అంకె_windows = 0;
			    EnumWindows(కిటికీ_cb, 0);
			    ఒకవేళ (అంకె_windows == 0)
			    {
				// Did not find the uninstaller, ask user to
				// press Enter when done. Just in case.
				రాయి("Press Enter when the uninstaller is finished\n");
				rewind(stdin);
				(శూన్య)getchar();
			    }
			    లేకపోతే
			    {
				రాయి("Waiting for the uninstaller to finish (press CTRL-C to abort).");
				చేయి
				{
				    రాయి(".");
				    fflush(stdout);
				    sleep(1);	// wait for the uninstaller to
						//ముగింపు
				    అంకె_windows = 0;
				    EnumWindows(కిటికీ_cb, 0);
				} అయితే (అంకె_windows > 0);
			    }
			}
			రాయి("\nDone!\n");

			// Check if an uninstall reg key was deleted.
			// if it was, we want to decrement key_index.
			// if we don't do this, we will skip the key
			// immediately after any key that we delete.
			Reg_Query_Info_కీలకం(కీలకం_handle, ఖాళీ, ఖాళీ, ఖాళీ,
					      &new_అంకె_కీలకాలు, ఖాళీ, ఖాళీ, ఖాళీ,
						      ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ);
			ఒకవేళ (new_అంకె_కీలకాలు < orig_అంకె_కీలకాలు)
			    కీలకం_సూచీ--;

			ఆదానం = 'y';
			ఆపు;

		    స్థితి 'n':
		    స్థితి 'N':
			// Do not uninstall
			ఆదానం = 'n';
			ఆపు;

		    అప్రమేయ: // just drop through and redo the loop
			ఆపు;
		}

	    } అయితే (ఆదానం != 'n' && ఆదానం != 'y');

	    Reg_Close_కీలకం(uninstall_కీలకం_handle);
	}

	కీలకం_సూచీ++;
    }
    Reg_Close_కీలకం(కీలకం_handle);

    తిరిగి foundone;
}

/*
 * Find out information about the system.
 */
    స్థిర శూన్య
inspect_system(శూన్య)
{
    అక్షరం	*p;
    అక్షరం	buf[BUFSIZE];
    FILE	*fd;
    పూర్ణాంకం		i;
    పూర్ణాంకం		foundone;

    // This may take a little while, let the user know what we're doing.
    రాయి("Inspecting system...\n");

    /*
     * If $VIM is set, check that it's pointing to our directory.
     */
    p = getenv("VIM");
    ఒకవేళ (p != ఖాళీ && pathcmp(p, -1, installdir, runtimeidx - 1) != 0)
    {
	రాయి("------------------------------------------------------\n");
	రాయి("$VIM is set to \"%s\".\n", p);
	రాయి("This is different from where this version of Vim is:\n");
	strcpy(buf, installdir);
	*(buf + runtimeidx - 1) = NUL;
	రాయి("\"%s\"\n", buf);
	రాయి("You must adjust or remove the setting of $VIM,\n");
	ఒకవేళ (interactive)
	{
	    రాయి("to be able to use this install program.\n");
	    myexit(1);
	}
	రాయి("otherwise Vim WILL NOT WORK properly!\n");
	రాయి("------------------------------------------------------\n");
    }

    /*
     * If $VIMRUNTIME is set, check that it's pointing to our runtime directory.
     */
    p = getenv("VIMRUNTIME");
    ఒకవేళ (p != ఖాళీ && pathcmp(p, -1, installdir, -1) != 0)
    {
	రాయి("------------------------------------------------------\n");
	రాయి("$VIMRUNTIME is set to \"%s\".\n", p);
	రాయి("This is different from where this version of Vim is:\n");
	రాయి("\"%s\"\n", installdir);
	రాయి("You must adjust or remove the setting of $VIMRUNTIME,\n");
	ఒకవేళ (interactive)
	{
	    రాయి("to be able to use this install program.\n");
	    myexit(1);
	}
	రాయి("otherwise Vim WILL NOT WORK properly!\n");
	రాయి("------------------------------------------------------\n");
    }

    /*
     * Check if there is a vim.[exe|bat|, gvim.[exe|bat|, etc. in the path.
     */
    కనుగొను_bat_exe(FALSE);

    /*
     * A .exe in the install directory may be found anyway on Windows 2000.
     * Check for this situation and find another executable if necessary.
     * w.briscoe@ponl.com 2001-01-20
     */
    foundone = 0;
    కొరకు (i = 1; i < లక్ష్యం_లెక్క; ++i)
    {
	findoldfile(&(targets[i].oldexe));
	ఒకవేళ (targets[i].oldexe != ఖాళీ)
	    foundone = 1;
    }

    ఒకవేళ (foundone)
    {
	రాయి("Warning: Found Vim executable(s) in your $PATH:\n");
	కొరకు (i = 1; i < లక్ష్యం_లెక్క; ++i)
	    ఒకవేళ (targets[i].oldexe != ఖాళీ)
		రాయి("%s\n", targets[i].oldexe);
	రాయి("It will be used instead of the version you are installing.\n");
	రాయి("Please delete or rename it, or adjust your $PATH setting.\n");
    }

    /*
     * Check if there is an existing ../_vimrc or ../.vimrc file.
     */
    strcpy(oldvimrc, installdir);
    strcpy(oldvimrc + runtimeidx, "_vimrc");
    ఒకవేళ ((fd = fopen(oldvimrc, "r")) == ఖాళీ)
    {
	strcpy(oldvimrc + runtimeidx, "vimrc~1"); // short version of .vimrc
	ఒకవేళ ((fd = fopen(oldvimrc, "r")) == ఖాళీ)
	{
	    strcpy(oldvimrc + runtimeidx, ".vimrc");
	    fd = fopen(oldvimrc, "r");
	}
    }
    ఒకవేళ (fd != ఖాళీ)
	fclose(fd);
    లేకపోతే
	*oldvimrc = NUL;
}

/*
 * Add a dummy choice to avoid that the numbering changes depending on items
 * in the environment.  The user may type a number he remembered without
 * looking.
 */
    స్థిర శూన్య
కూడు_dummy_choice(శూన్య)
{
    choices[choice_లెక్క].installfunc = ఖాళీ;
    choices[choice_లెక్క].చురుకైన = 0;
    choices[choice_లెక్క].changefunc = ఖాళీ;
    choices[choice_లెక్క].పాఠ్యం = ఖాళీ;
    choices[choice_లెక్క].arg = 0;
    ++choice_లెక్క;
}

////////////////////////////////////////////////
// stuff for creating the batch files.

/*
 * Install the vim.bat, gvim.bat, etc. files.
 */
    స్థిర శూన్య
install_bat_choice(పూర్ణాంకం idx)
{
    అక్షరం	*batpath = targets[choices[idx].arg].batpath;
    అక్షరం	*oldname = targets[choices[idx].arg].oldbat;
    అక్షరం	*exename = targets[choices[idx].arg].exenamearg;
    అక్షరం	*vimarg = targets[choices[idx].arg].exearg;
    FILE	*fd;

    ఒకవేళ (*batpath == NUL)
	తిరిగి;

    fd = fopen(batpath, "w");
    ఒకవేళ (fd == ఖాళీ)
    {
	రాయి("\nERROR: Cannot open \"%s\" for writing.\n", batpath);
	తిరిగి;
    }

    need_uninstall_నమోదు = 1;

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
	    VIM_సంస్కరణ_NODOT, exename, VIM_సంస్కరణ_NODOT);
    fprintf(fd, "if exist \"%%VIMRUNTIME%%\\%s\" set VIM_EXE_DIR=%%VIMRUNTIME%%\n", exename);
    fprintf(fd, "\n");

    // Give an error message when the executable could not be found.
    fprintf(fd, "if not exist \"%%VIM_EXE_DIR%%\\%s\" (\n", exename);
    fprintf(fd, "    echo \"%%VIM_EXE_DIR%%\\%s\" not found\n", exename);
    fprintf(fd, "    goto :eof\n");
    fprintf(fd, ")\n");
    fprintf(fd, "\n");

    ఒకవేళ (*exename == 'g')
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

    ఒకవేళ (*exename == 'g')
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
    లేకపోతే
    {
	// Always use quotes, $VIM or $VIMRUNTIME might have a space.
	fprintf(fd, "\"%%VIM_EXE_DIR%%\\%s\" %s %%*\n",
		exename, vimarg);
    }

    fclose(fd);
    రాయి("%s has been %s\n", batpath,
	    oldname == ఖాళీ ? "created" : "overwritten");
}

/*
 * Make the text string for choice "idx".
 * The format "fmt" is must have one %s item, which "arg" is used for.
 */
    స్థిర శూన్య
alloc_పాఠ్యం(పూర్ణాంకం idx, అక్షరం *fmt, అక్షరం *arg)
{
    ఒకవేళ (choices[idx].పాఠ్యం != ఖాళీ)
	విడుదల(choices[idx].పాఠ్యం);

    choices[idx].పాఠ్యం = alloc(అక్షరశ్రేణి_పొడవు(fmt) + అక్షరశ్రేణి_పొడవు(arg) - 1);
    sprintf(choices[idx].పాఠ్యం, fmt, arg);
}

/*
 * Toggle the "Overwrite .../vim.bat" to "Don't overwrite".
 */
    స్థిర శూన్య
toggle_bat_choice(పూర్ణాంకం idx)
{
    అక్షరం	*batname = targets[choices[idx].arg].batpath;
    అక్షరం	*oldname = targets[choices[idx].arg].oldbat;

    ఒకవేళ (*batname == NUL)
    {
	alloc_పాఠ్యం(idx, "    Overwrite %s", oldname);
	strcpy(batname, oldname);
    }
    లేకపోతే
    {
	alloc_పాఠ్యం(idx, "    Do NOT overwrite %s", oldname);
	*batname = NUL;
    }
}

/*
 * Do some work for a batch file entry: Append the batch file name to the path
 * and set the text for the choice.
 */
    స్థిర శూన్య
సమితి_bat_పాఠ్యం(పూర్ణాంకం idx, అక్షరం *batpath, అక్షరం *name)
{
    strcat(batpath, name);

    alloc_పాఠ్యం(idx, "    Create %s", batpath);
}

/*
 * Select a directory to write the batch file line.
 */
    స్థిర శూన్య
change_bat_choice(పూర్ణాంకం idx)
{
    అక్షరం	*మార్గం;
    అక్షరం	*batpath;
    అక్షరం	*name;
    పూర్ణాంకం		n;
    అక్షరం	*s;
    అక్షరం	*p;
    పూర్ణాంకం		లెక్క;
    అక్షరం	**పేర్లు = ఖాళీ;
    పూర్ణాంకం		i;
    పూర్ణాంకం		లక్ష్యం = choices[idx].arg;

    name = targets[లక్ష్యం].batname;
    batpath = targets[లక్ష్యం].batpath;

    మార్గం = getenv("PATH");
    ఒకవేళ (మార్గం == ఖాళీ)
    {
	రాయి("\nERROR: The variable $PATH is not set\n");
	తిరిగి;
    }

    /*
     * first round: count number of names in path;
     * second round: save names to names[].
     */
    కొరకు (;;)
    {
	లెక్క = 1;
	కొరకు (p = మార్గం; *p; )
	{
	    s = strchr(p, ';');
	    ఒకవేళ (s == ఖాళీ)
		s = p + అక్షరశ్రేణి_పొడవు(p);
	    ఒకవేళ (పేర్లు != ఖాళీ)
	    {
		పేర్లు[లెక్క] = alloc(s - p + 1);
		strncpy(పేర్లు[లెక్క], p, s - p);
		పేర్లు[లెక్క][s - p] = NUL;
	    }
	    ++లెక్క;
	    p = s;
	    ఒకవేళ (*p != NUL)
		++p;
	}
	ఒకవేళ (పేర్లు != ఖాళీ)
	    ఆపు;
	పేర్లు = alloc((లెక్క + 1) * పరిమాణం(అక్షరం *));
    }
    పేర్లు[0] = alloc(50);
    sprintf(పేర్లు[0], "Select directory to create %s in:", name);
    పేర్లు[లెక్క] = alloc(50);
    ఒకవేళ (choices[idx].arg == 0)
	sprintf(పేర్లు[లెక్క], "Do not create any .bat file.");
    లేకపోతే
	sprintf(పేర్లు[లెక్క], "Do not create a %s file.", name);
    n = get_choice(పేర్లు, లెక్క + 1);

    ఒకవేళ (n == లెక్క)
    {
	// Selected last item, don't create bat file.
	*batpath = NUL;
	ఒకవేళ (choices[idx].arg != 0)
	    alloc_పాఠ్యం(idx, "    Do NOT create %s", name);
    }
    లేకపోతే
    {
	// Selected one of the paths.  For the first item only keep the path,
	// for the others append the batch file name.
	strcpy(batpath, పేర్లు[n]);
	కూడు_pathsep(batpath);
	ఒకవేళ (choices[idx].arg != 0)
	    సమితి_bat_పాఠ్యం(idx, batpath, name);
    }

    కొరకు (i = 0; i <= లెక్క; ++i)
	విడుదల(పేర్లు[i]);
    విడుదల(పేర్లు);
}

అక్షరం *bat_పాఠ్యం_yes = "Install .bat files to use Vim at the command line:";
అక్షరం *bat_పాఠ్యం_no = "do NOT install .bat files to use Vim at the command line";

    స్థిర శూన్య
change_main_bat_choice(పూర్ణాంకం idx)
{
    పూర్ణాంకం		i;

    // let the user select a default directory or NONE
    change_bat_choice(idx);

    ఒకవేళ (targets[0].batpath[0] != NUL)
	choices[idx].పాఠ్యం = bat_పాఠ్యం_yes;
    లేకపోతే
	choices[idx].పాఠ్యం = bat_పాఠ్యం_no;

    // update the individual batch file selections
    కొరకు (i = 1; i < లక్ష్యం_లెక్క; ++i)
    {
	// Only make it active when the first item has a path and the vim.exe
	// or gvim.exe exists (there is a changefunc then).
	ఒకవేళ (targets[0].batpath[0] != NUL
		&& choices[idx + i].changefunc != ఖాళీ)
	{
	    choices[idx + i].చురుకైన = 1;
	    ఒకవేళ (choices[idx + i].changefunc == change_bat_choice
		    && targets[i].batpath[0] != NUL)
	    {
		strcpy(targets[i].batpath, targets[0].batpath);
		సమితి_bat_పాఠ్యం(idx + i, targets[i].batpath, targets[i].batname);
	    }
	}
	లేకపోతే
	    choices[idx + i].చురుకైన = 0;
    }
}

/*
 * Initialize a choice for creating a batch file.
 */
    స్థిర శూన్య
init_bat_choice(పూర్ణాంకం లక్ష్యం)
{
    అక్షరం	*batpath = targets[లక్ష్యం].batpath;
    అక్షరం	*oldbat = targets[లక్ష్యం].oldbat;
    అక్షరం	*p;
    పూర్ణాంకం		i;

    choices[choice_లెక్క].arg = లక్ష్యం;
    choices[choice_లెక్క].installfunc = install_bat_choice;
    choices[choice_లెక్క].చురుకైన = 1;
    choices[choice_లెక్క].పాఠ్యం = ఖాళీ;	// will be set below
    ఒకవేళ (oldbat != ఖాళీ)
    {
	// A [g]vim.bat exists: Only choice is to overwrite it or not.
	choices[choice_లెక్క].changefunc = toggle_bat_choice;
	*batpath = NUL;
	toggle_bat_choice(choice_లెక్క);
    }
    లేకపోతే
    {
	ఒకవేళ (default_bat_dir != ఖాళీ)
	    // Prefer using the same path as an existing .bat file.
	    strcpy(batpath, default_bat_dir);
	లేకపోతే
	{
	    // No [g]vim.bat exists: Write it to a directory in $PATH.  Use
	    // $WINDIR by default, if it's empty the first item in $PATH.
	    p = getenv("WINDIR");
	    ఒకవేళ (p != ఖాళీ && *p != NUL)
		strcpy(batpath, p);
	    లేకపోతే
	    {
		p = getenv("PATH");
		ఒకవేళ (p == ఖాళీ || *p == NUL)		// "cannot happen"
		    strcpy(batpath, "C:/Windows");
		లేకపోతే
		{
		    i = 0;
		    అయితే (*p != NUL && *p != ';')
			batpath[i++] = *p++;
		    batpath[i] = NUL;
		}
	    }
	}
	కూడు_pathsep(batpath);
	సమితి_bat_పాఠ్యం(choice_లెక్క, batpath, targets[లక్ష్యం].batname);

	choices[choice_లెక్క].changefunc = change_bat_choice;
    }
    ++choice_లెక్క;
}

/*
 * Set up the choices for installing .bat files.
 * For these items "arg" is the index in targets[].
 */
    స్థిర శూన్య
init_bat_choices(శూన్య)
{
    పూర్ణాంకం		i;

    // The first item is used to switch installing batch files on/off and
    // setting the default path.
    choices[choice_లెక్క].పాఠ్యం = bat_పాఠ్యం_yes;
    choices[choice_లెక్క].changefunc = change_main_bat_choice;
    choices[choice_లెక్క].installfunc = ఖాళీ;
    choices[choice_లెక్క].చురుకైన = 1;
    choices[choice_లెక్క].arg = 0;
    ++choice_లెక్క;

    // Add items for each batch file target.  Only used when not disabled by
    // the first item.  When a .exe exists, don't offer to create a .bat.
    కొరకు (i = 1; i < లక్ష్యం_లెక్క; ++i)
	ఒకవేళ (targets[i].oldexe == ఖాళీ
		&& (targets[i].exenamearg[0] == 'g' ? has_gvim : has_vim))
	    init_bat_choice(i);
	లేకపోతే
	    కూడు_dummy_choice();
}

/*
 * Install the vimrc file.
 */
    స్థిర శూన్య
install_vimrc(పూర్ణాంకం idx వాడని)
{
    FILE	*fd, *tfd;
    అక్షరం	*fname;

    // If an old vimrc file exists, overwrite it.
    // Otherwise create a new one.
    ఒకవేళ (*oldvimrc != NUL)
	fname = oldvimrc;
    లేకపోతే
	fname = vimrc;

    fd = fopen(fname, "w");
    ఒకవేళ (fd == ఖాళీ)
    {
	రాయి("\nERROR: Cannot open \"%s\" for writing.\n", fname);
	తిరిగి;
    }
    ఎంపిక (compat_choice)
    {
	స్థితి compat_vi:
		    fprintf(fd, "\" Vi compatible\n");
		    fprintf(fd, "set compatible\n");
		    ఆపు;
	స్థితి compat_vim:
		    fprintf(fd, "\" Vim's default behavior\n");
		    fprintf(fd, "if &compatible\n");
		    fprintf(fd, "  set nocompatible\n");
		    fprintf(fd, "endif\n");
		    ఆపు;
	స్థితి compat_some_enhancements:
		    fprintf(fd, "\" Vim with some enhancements\n");
		    fprintf(fd, "source $VIMRUNTIME/defaults.vim\n");
		    ఆపు;
	స్థితి compat_all_enhancements:
		    fprintf(fd, "\" Vim with all enhancements\n");
		    fprintf(fd, "source $VIMRUNTIME/vimrc_example.vim\n");
		    ఆపు;
    }
    ఎంపిక (remap_choice)
    {
	స్థితి remap_no:
		    ఆపు;
	స్థితి remap_win:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Remap a few keys for Windows behavior\n");
		    fprintf(fd, "source $VIMRUNTIME/mswin.vim\n");
		    ఆపు;
    }
    ఎంపిక (mouse_choice)
    {
	స్థితి mouse_xterm:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Mouse behavior (the Unix way)\n");
		    fprintf(fd, "behave xterm\n");
		    ఆపు;
	స్థితి mouse_mswin:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Mouse behavior (the Windows way)\n");
		    fprintf(fd, "behave mswin\n");
		    ఆపు;
	స్థితి mouse_default:
		    ఆపు;
    }
    ఒకవేళ ((tfd = fopen("diff.exe", "r")) != ఖాళీ)
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
    రాయి("%s has been written\n", fname);
}

    స్థిర శూన్య
change_vimrc_choice(పూర్ణాంకం idx)
{
    ఒకవేళ (choices[idx].installfunc != ఖాళీ)
    {
	// Switch to NOT change or create a vimrc file.
	ఒకవేళ (*oldvimrc != NUL)
	    alloc_పాఠ్యం(idx, "Do NOT change startup file %s", oldvimrc);
	లేకపోతే
	    alloc_పాఠ్యం(idx, "Do NOT create startup file %s", vimrc);
	choices[idx].installfunc = ఖాళీ;
	choices[idx + 1].చురుకైన = 0;
	choices[idx + 2].చురుకైన = 0;
	choices[idx + 3].చురుకైన = 0;
    }
    లేకపోతే
    {
	// Switch to change or create a vimrc file.
	ఒకవేళ (*oldvimrc != NUL)
	    alloc_పాఠ్యం(idx, "Overwrite startup file %s with:", oldvimrc);
	లేకపోతే
	    alloc_పాఠ్యం(idx, "Create startup file %s with:", vimrc);
	choices[idx].installfunc = install_vimrc;
	choices[idx + 1].చురుకైన = 1;
	choices[idx + 2].చురుకైన = 1;
	choices[idx + 3].చురుకైన = 1;
    }
}

/*
 * Change the choice how to run Vim.
 */
    స్థిర శూన్య
change_నిరంతరం_choice(పూర్ణాంకం idx)
{
    compat_choice = get_choice(compat_choices, పట్టిక_కొలత(compat_choices));
    alloc_పాఠ్యం(idx, compat_పాఠ్యం, compat_choices[compat_choice]);
}

/*
 * Change the choice if keys are to be remapped.
 */
    స్థిర శూన్య
change_remap_choice(పూర్ణాంకం idx)
{
    remap_choice = get_choice(remap_choices, పట్టిక_కొలత(remap_choices));
    alloc_పాఠ్యం(idx, remap_పాఠ్యం, remap_choices[remap_choice]);
}

/*
 * Change the choice how to select text.
 */
    స్థిర శూన్య
change_mouse_choice(పూర్ణాంకం idx)
{
    mouse_choice = get_choice(mouse_choices, పట్టిక_కొలత(mouse_choices));
    alloc_పాఠ్యం(idx, mouse_పాఠ్యం, mouse_choices[mouse_choice]);
}

    స్థిర శూన్య
init_vimrc_choices(శూన్య)
{
    // set path for a new _vimrc file (also when not used)
    strcpy(vimrc, installdir);
    strcpy(vimrc + runtimeidx, "_vimrc");

    // Set opposite value and then toggle it by calling change_vimrc_choice()
    ఒకవేళ (*oldvimrc == NUL)
	choices[choice_లెక్క].installfunc = ఖాళీ;
    లేకపోతే
	choices[choice_లెక్క].installfunc = install_vimrc;
    choices[choice_లెక్క].పాఠ్యం = ఖాళీ;
    change_vimrc_choice(choice_లెక్క);
    choices[choice_లెక్క].changefunc = change_vimrc_choice;
    choices[choice_లెక్క].చురుకైన = 1;
    ++choice_లెక్క;

    // default way to run Vim
    alloc_పాఠ్యం(choice_లెక్క, compat_పాఠ్యం, compat_choices[compat_choice]);
    choices[choice_లెక్క].changefunc = change_నిరంతరం_choice;
    choices[choice_లెక్క].installfunc = ఖాళీ;
    choices[choice_లెక్క].చురుకైన = (*oldvimrc == NUL);
    ++choice_లెక్క;

    // Whether to remap keys
    alloc_పాఠ్యం(choice_లెక్క, remap_పాఠ్యం , remap_choices[remap_choice]);
    choices[choice_లెక్క].changefunc = change_remap_choice;
    choices[choice_లెక్క].installfunc = ఖాళీ;
    choices[choice_లెక్క].చురుకైన = (*oldvimrc == NUL);
    ++choice_లెక్క;

    // default way to use the mouse
    alloc_పాఠ్యం(choice_లెక్క, mouse_పాఠ్యం, mouse_choices[mouse_choice]);
    choices[choice_లెక్క].changefunc = change_mouse_choice;
    choices[choice_లెక్క].installfunc = ఖాళీ;
    choices[choice_లెక్క].చురుకైన = (*oldvimrc == NUL);
    ++choice_లెక్క;
}

    స్థిర LONG
reg_create_కీలకం(
    HKEY మూలం,
    స్థిరరాశి అక్షరం *subkey,
    PHKEY ph_కీలకం,
    DWORD flag)
{
    DWORD disp;

    *ph_కీలకం = ఖాళీ;
    తిరిగి Reg_Create_కీలకం_Ex(
		మూలం, subkey,
		0, ఖాళీ, REG_OPTION_NON_VOLATILE,
		flag | కీలకం_WRITE,
		ఖాళీ, ph_కీలకం, &disp);
}

    స్థిర LONG
reg_సమితి_string_విలువ(
    HKEY h_కీలకం,
    స్థిరరాశి అక్షరం *విలువ_name,
    స్థిరరాశి అక్షరం *దత్తాంశం)
{
    తిరిగి Reg_సమితి_విలువ_Ex(h_కీలకం, విలువ_name, 0, REG_SZ,
				     (LPBYTE)దత్తాంశం, (DWORD)(1 + అక్షరశ్రేణి_పొడవు(దత్తాంశం)));
}

    స్థిర LONG
reg_create_కీలకం_and_విలువ(
    HKEY h_మూలం_కీలకం,
    స్థిరరాశి అక్షరం *subkey,
    స్థిరరాశి అక్షరం *విలువ_name,
    స్థిరరాశి అక్షరం *దత్తాంశం,
    DWORD flag)
{
    HKEY h_కీలకం;
    LONG lRet = reg_create_కీలకం(h_మూలం_కీలకం, subkey, &h_కీలకం, flag);

    ఒకవేళ (ERROR_SUCCESS == lRet)
    {
	lRet = reg_సమితి_string_విలువ(h_కీలకం, విలువ_name, దత్తాంశం);
	Reg_Close_కీలకం(h_కీలకం);
    }
    తిరిగి lRet;
}

    స్థిర LONG
రిజిస్టరు_inproc_server(
    HKEY h_మూలం_కీలకం,
    స్థిరరాశి అక్షరం *clsid,
    స్థిరరాశి అక్షరం *extname,
    స్థిరరాశి అక్షరం *module,
    స్థిరరాశి అక్షరం *threading_model,
    DWORD flag)
{
    CHAR subkey[BUFSIZE];
    LONG lRet;

    sprintf(subkey, "CLSID\\%s", clsid);
    lRet = reg_create_కీలకం_and_విలువ(h_మూలం_కీలకం, subkey, ఖాళీ, extname, flag);
    ఒకవేళ (ERROR_SUCCESS == lRet)
    {
	sprintf(subkey, "CLSID\\%s\\InProcServer32", clsid);
	lRet = reg_create_కీలకం_and_విలువ(h_మూలం_కీలకం, subkey, ఖాళీ, module, flag);
	ఒకవేళ (ERROR_SUCCESS == lRet)
	{
	    lRet = reg_create_కీలకం_and_విలువ(h_మూలం_కీలకం, subkey,
					   "ThreadingModel", threading_model, flag);
	}
    }
    తిరిగి lRet;
}

    స్థిర LONG
రిజిస్టరు_shellex(
    HKEY h_మూలం_కీలకం,
    స్థిరరాశి అక్షరం *clsid,
    స్థిరరాశి అక్షరం *name,
    స్థిరరాశి అక్షరం *exe_మార్గం,
    DWORD flag)
{
    LONG lRet = reg_create_కీలకం_and_విలువ(
	    h_మూలం_కీలకం,
	    "*\\shellex\\ContextMenuHandlers\\gvim",
	    ఖాళీ,
	    clsid,
	    flag);

    ఒకవేళ (ERROR_SUCCESS == lRet)
    {
	lRet = reg_create_కీలకం_and_విలువ(
		HKEY_LOCAL_MACHINE,
		"Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved",
		clsid,
		name,
		flag);

	ఒకవేళ (ERROR_SUCCESS == lRet)
	{
	    lRet = reg_create_కీలకం_and_విలువ(
		    HKEY_LOCAL_MACHINE,
		    "Software\\Vim\\Gvim",
		    "path",
		    exe_మార్గం,
		    flag);
	}
    }
    తిరిగి lRet;
}

    స్థిర LONG
రిజిస్టరు_openwith(
    HKEY h_మూలం_కీలకం,
    స్థిరరాశి అక్షరం *exe_మార్గం,
    DWORD flag)
{
    అక్షరం	exe_cmd[BUFSIZE];
    LONG	lRet;

    sprintf(exe_cmd, "\"%s\" \"%%1\"", exe_మార్గం);
    lRet = reg_create_కీలకం_and_విలువ(
	    h_మూలం_కీలకం,
	    "Applications\\gvim.exe\\shell\\edit\\command",
	    ఖాళీ,
	    exe_cmd,
	    flag);

    ఒకవేళ (ERROR_SUCCESS == lRet)
    {
	పూర్ణాంకం i;
	స్థిర స్థిరరాశి అక్షరం *openwith[] = {
		".htm\\OpenWithList\\gvim.exe",
		".vim\\OpenWithList\\gvim.exe",
		"*\\OpenWithList\\gvim.exe",
	};

	కొరకు (i = 0; ERROR_SUCCESS == lRet && i < (పూర్ణాంకం)ARRAYSIZE(openwith); i++)
	    lRet = reg_create_కీలకం_and_విలువ(h_మూలం_కీలకం, openwith[i], ఖాళీ, "", flag);
    }

    తిరిగి lRet;
}

    స్థిర LONG
రిజిస్టరు_uninstall(
    HKEY h_మూలం_కీలకం,
    స్థిరరాశి అక్షరం *appname,
    స్థిరరాశి అక్షరం *display_name,
    స్థిరరాశి అక్షరం *uninstall_string,
    స్థిరరాశి అక్షరం *display_icon,
    స్థిరరాశి అక్షరం *display_సంస్కరణ,
    స్థిరరాశి అక్షరం *publisher)
{
    LONG lRet = reg_create_కీలకం_and_విలువ(h_మూలం_కీలకం, appname,
			     "DisplayName", display_name, కీలకం_WOW64_64_కీలకం);

    ఒకవేళ (ERROR_SUCCESS == lRet)
	lRet = reg_create_కీలకం_and_విలువ(h_మూలం_కీలకం, appname,
		     "UninstallString", uninstall_string, కీలకం_WOW64_64_కీలకం);
    ఒకవేళ (ERROR_SUCCESS == lRet)
	lRet = reg_create_కీలకం_and_విలువ(h_మూలం_కీలకం, appname,
		     "DisplayIcon", display_icon, కీలకం_WOW64_64_కీలకం);
    ఒకవేళ (ERROR_SUCCESS == lRet)
	lRet = reg_create_కీలకం_and_విలువ(h_మూలం_కీలకం, appname,
		     "DisplayVersion", display_సంస్కరణ, కీలకం_WOW64_64_కీలకం);
    ఒకవేళ (ERROR_SUCCESS == lRet)
	lRet = reg_create_కీలకం_and_విలువ(h_మూలం_కీలకం, appname,
		     "Publisher", publisher, కీలకం_WOW64_64_కీలకం);
    తిరిగి lRet;
}

/*
 * Add some entries to the registry:
 * - to add "Edit with Vim" to the context * menu
 * - to add Vim to the "Open with..." list
 * - to uninstall Vim
 */
//ARGSUSED
    స్థిర పూర్ణాంకం
install_registry(శూన్య)
{
    LONG	lRet = ERROR_SUCCESS;
    స్థిరరాశి అక్షరం	*vim_ext_ThreadingModel = "Apartment";
    స్థిరరాశి అక్షరం	*vim_ext_name = "Vim Shell Extension";
    స్థిరరాశి అక్షరం	*vim_ext_clsid = "{51EEE242-AD87-11d3-9C1E-0090278BBD99}";
    అక్షరం	vim_exe_మార్గం[MAX_మార్గం];
    అక్షరం	display_name[BUFSIZE];
    అక్షరం	uninstall_string[BUFSIZE];
    అక్షరం	icon_string[BUFSIZE];
    అక్షరం	సంస్కరణ_string[BUFSIZE];
    పూర్ణాంకం		i;
    పూర్ణాంకం		loop_లెక్క = is_64bit_os() ? 2 : 1;
    DWORD	flag;

    sprintf(vim_exe_మార్గం, "%s\\gvim.exe", installdir);

    ఒకవేళ (install_popup)
    {
	అక్షరం	    bufg[BUFSIZE];

	రాయి("Creating \"Edit with Vim\" popup menu entry\n");

	కొరకు (i = 0; i < loop_లెక్క; i++)
	{
	    ఒకవేళ (i == 0)
	    {
		sprintf(bufg, "%s\\" GVIMEXT32_మార్గం, installdir);
		flag = కీలకం_WOW64_32_కీలకం;
	    }
	    లేకపోతే
	    {
		sprintf(bufg, "%s\\" GVIMEXT64_మార్గం, installdir);
		flag = కీలకం_WOW64_64_కీలకం;
	    }

	    lRet = రిజిస్టరు_inproc_server(
		    HKEY_CLASSES_మూలం, vim_ext_clsid, vim_ext_name,
		    bufg, vim_ext_ThreadingModel, flag);
	    ఒకవేళ (ERROR_SUCCESS != lRet)
		తిరిగి FAIL;
	    lRet = రిజిస్టరు_shellex(
		    HKEY_CLASSES_మూలం, vim_ext_clsid, vim_ext_name,
		    vim_exe_మార్గం, flag);
	    ఒకవేళ (ERROR_SUCCESS != lRet)
		తిరిగి FAIL;
	}
    }

    ఒకవేళ (install_openwith)
    {
	రాయి("Creating \"Open with ...\" list entry\n");

	కొరకు (i = 0; i < loop_లెక్క; i++)
	{
	    ఒకవేళ (i == 0)
		flag = కీలకం_WOW64_32_కీలకం;
	    లేకపోతే
		flag = కీలకం_WOW64_64_కీలకం;

	    lRet = రిజిస్టరు_openwith(HKEY_CLASSES_మూలం, vim_exe_మార్గం, flag);
	    ఒకవేళ (ERROR_SUCCESS != lRet)
		తిరిగి FAIL;
	}
    }

    రాయి("Creating an uninstall entry\n");
    sprintf(display_name, "Vim " VIM_సంస్కరణ_SHORT
#నిర్వచితమైతే _M_ARM64
	    " (arm64)"
#లేదావేళ _M_X64
	    " (x64)"
#అయితే_ముగింపు
	    );

    // For the NSIS installer use the generated uninstaller.
    ఒకవేళ (interactive)
	sprintf(uninstall_string, "%s\\uninstall.exe", installdir);
    లేకపోతే
	sprintf(uninstall_string, "%s\\uninstall-gui.exe", installdir);

    sprintf(icon_string, "%s\\gvim.exe,0", installdir);

    sprintf(సంస్కరణ_string, VIM_సంస్కరణ_SHORT "." VIM_సంస్కరణ_PATCHLEVEL_STR);

    lRet = రిజిస్టరు_uninstall(
	HKEY_LOCAL_MACHINE,
	"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Vim " VIM_సంస్కరణ_SHORT,
	display_name,
	uninstall_string,
	icon_string,
	సంస్కరణ_string,
	"The Vim Project");
    ఒకవేళ (ERROR_SUCCESS != lRet)
	తిరిగి FAIL;

    తిరిగి OK;
}

    స్థిర శూన్య
change_popup_choice(పూర్ణాంకం idx)
{
    ఒకవేళ (install_popup == 0)
    {
	choices[idx].పాఠ్యం = "Install an entry for Vim in the popup menu for the right\n    mouse button so that you can edit any file with Vim";
	install_popup = 1;
    }
    లేకపోతే
    {
	choices[idx].పాఠ్యం = "Do NOT install an entry for Vim in the popup menu for the\n    right mouse button to edit any file with Vim";
	install_popup = 0;
    }
}

/*
 * Only add the choice for the popup menu entry when gvim.exe was found and
 * both gvimext.dll and regedit.exe exist.
 */
    స్థిర శూన్య
init_popup_choice(శూన్య)
{
    నిర్మాణం stat	st;

    ఒకవేళ (has_gvim
	    && (stat(GVIMEXT32_మార్గం, &st) >= 0
		|| stat(GVIMEXT64_మార్గం, &st) >= 0))
    {
	choices[choice_లెక్క].changefunc = change_popup_choice;
	choices[choice_లెక్క].installfunc = ఖాళీ;
	choices[choice_లెక్క].చురుకైన = 1;
	change_popup_choice(choice_లెక్క);  // set the text
	++choice_లెక్క;
    }
    లేకపోతే
	కూడు_dummy_choice();
}

    స్థిర శూన్య
change_openwith_choice(పూర్ణాంకం idx)
{
    ఒకవేళ (install_openwith == 0)
    {
	choices[idx].పాఠ్యం = "Add Vim to the \"Open With...\" list in the popup menu for the right\n    mouse button so that you can edit any file with Vim";
	install_openwith = 1;
    }
    లేకపోతే
    {
	choices[idx].పాఠ్యం = "Do NOT add Vim to the \"Open With...\" list in the popup menu for the\n    right mouse button to edit any file with Vim";
	install_openwith = 0;
    }
}

/*
 * Only add the choice for the open-with menu entry when gvim.exe was found
 * and regedit.exe exist.
 */
    స్థిర శూన్య
init_openwith_choice(శూన్య)
{
    ఒకవేళ (has_gvim)
    {
	choices[choice_లెక్క].changefunc = change_openwith_choice;
	choices[choice_లెక్క].installfunc = ఖాళీ;
	choices[choice_లెక్క].చురుకైన = 1;
	change_openwith_choice(choice_లెక్క);  // set the text
	++choice_లెక్క;
    }
    లేకపోతే
	కూడు_dummy_choice();
}

/*
 * Create a shell link.
 *
 * returns 0 on failure, non-zero on successful completion.
 *
 * NOTE:  Currently untested with mingw.
 */
    పూర్ణాంకం
create_shortcut(
	స్థిరరాశి అక్షరం *shortcut_name,
	స్థిరరాశి అక్షరం *iconfile_మార్గం,
	పూర్ణాంకం	    iconindex,
	స్థిరరాశి అక్షరం *shortcut_లక్ష్యం,
	స్థిరరాశి అక్షరం *shortcut_వాదనలు,
	స్థిరరాశి అక్షరం *workingdir
	)
{
    IShellLink	    *shelllink_ptr;
    HRESULT	    hres;
    IPersistFile	    *persistfile_ptr;

    // Initialize COM library
    hres = CoInitialize(ఖాళీ);
    ఒకవేళ (!SUCCEEDED(hres))
    {
	రాయి("Error:  Could not open the COM library.  Not creating shortcut.\n");
	తిరిగి FAIL;
    }

    // Instantiate a COM object for the ShellLink, store a pointer to it
    // in shelllink_ptr.
    hres = CoCreateInstance(&CLSID_ShellLink,
			   ఖాళీ,
			   CLSCTX_INPROC_SERVER,
			   &IID_IShellLink,
			   (శూన్య **) &shelllink_ptr);

    ఒకవేళ (SUCCEEDED(hres)) // If the instantiation was successful...
    {
	// ...Then build a PersistFile interface for the ShellLink so we can
	// save it as a file after we build it.
	hres = shelllink_ptr->lpVtbl->QueryInterface(shelllink_ptr,
		&IID_IPersistFile, (శూన్య **) &persistfile_ptr);

	ఒకవేళ (SUCCEEDED(hres))
	{
	    wchar_t wsz[BUFSIZE];

	    // translate the (possibly) multibyte shortcut filename to windows
	    // Unicode so it can be used as a file name.
	    Multi_Byte_గమ్యం_Wide_Char(CP_ACP, 0, shortcut_name, -1, wsz, పరిమాణం(wsz)/పరిమాణం(wsz[0]));

	    // set the attributes
	    shelllink_ptr->lpVtbl->సమితి_మార్గం(shelllink_ptr, shortcut_లక్ష్యం);
	    shelllink_ptr->lpVtbl->సమితి_Working_Directory(shelllink_ptr,
								  workingdir);
	    shelllink_ptr->lpVtbl->సమితి_Icon_Location(shelllink_ptr,
						    iconfile_మార్గం, iconindex);
	    shelllink_ptr->lpVtbl->సమితి_Arguments(shelllink_ptr, shortcut_వాదనలు);

	    // save the shortcut to a file and return the PersistFile object
	    persistfile_ptr->lpVtbl->Save(persistfile_ptr, wsz, 1);
	    persistfile_ptr->lpVtbl->Release(persistfile_ptr);
	}
	లేకపోతే
	{
	    రాయి("QueryInterface Error\n");
	    తిరిగి FAIL;
	}

	// Return the ShellLink object
	shelllink_ptr->lpVtbl->Release(shelllink_ptr);
    }
    లేకపోతే
    {
	రాయి("CoCreateInstance Error - hres = %08x\n", (పూర్ణాంకం)hres);
	తిరిగి FAIL;
    }

    తిరిగి OK;
}

/*
 * Build a path to where we will put a specified link.
 *
 * Return 0 on error, non-zero on success
 */
   పూర్ణాంకం
build_link_name(
	అక్షరం	   *link_మార్గం,
	స్థిరరాశి అక్షరం *link_name,
	స్థిరరాశి అక్షరం *shell_folder_name)
{
    అక్షరం	shell_folder_మార్గం[MAX_మార్గం];

    ఒకవేళ (get_shell_folder_మార్గం(shell_folder_మార్గం, shell_folder_name) == FAIL)
    {
	రాయి("An error occurred while attempting to find the path to %s.\n",
							   shell_folder_name);
	తిరిగి FAIL;
    }

    // Make sure the directory exists (create Start Menu\Programs\Vim).
    // Ignore errors if it already exists.
    vim_mkdir(shell_folder_మార్గం, 0755);

    // build the path to the shortcut and the path to gvim.exe
    sprintf(link_మార్గం, "%s\\%s.lnk", shell_folder_మార్గం, link_name);

    తిరిగి OK;
}

    స్థిర పూర్ణాంకం
build_shortcut(
	స్థిరరాశి అక్షరం *name,	// Name of the shortcut
	స్థిరరాశి అక్షరం *exename,	// Name of the executable (e.g., vim.exe)
	స్థిరరాశి అక్షరం *వాదనలు,
	స్థిరరాశి అక్షరం *shell_folder,
	స్థిరరాశి అక్షరం *workingdir)
{
    అక్షరం	executable_మార్గం[BUFSIZE];
    అక్షరం	link_name[BUFSIZE];

    sprintf(executable_మార్గం, "%s\\%s", installdir, exename);

    ఒకవేళ (build_link_name(link_name, name, shell_folder) == FAIL)
    {
	రాయి("An error has occurred.  A shortcut to %s will not be created %s.\n",
		name,
		*shell_folder == 'd' ? "on the desktop" : "in the Start menu");
	తిరిగి FAIL;
    }

    // Create the shortcut:
    తిరిగి create_shortcut(link_name, executable_మార్గం, 0,
					   executable_మార్గం, వాదనలు, workingdir);
}

/*
 * We used to use "homedir" as the working directory, but that is a bad choice
 * on multi-user systems.  However, not specifying a directory results in the
 * current directory to be c:\Windows\system32 on Windows 7. Use environment
 * variables instead.
 */
#నిర్వచించు WORKDIR "%HOMEDRIVE%%HOMEPATH%"

/*
 * Create shortcut(s) in the Start Menu\Programs\Vim folder.
 */
    స్థిర శూన్య
install_ప్రారంభం_menu(పూర్ణాంకం idx వాడని)
{
    need_uninstall_నమోదు = 1;
    రాయి("Creating start menu\n");
    ఒకవేళ (has_vim)
    {
	ఒకవేళ (build_shortcut("Vim", "vim.exe", "",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    తిరిగి;
	ఒకవేళ (build_shortcut("Vim Read-only", "vim.exe", "-R",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    తిరిగి;
	ఒకవేళ (build_shortcut("Vim Diff", "vim.exe", "-d",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    తిరిగి;
    }
    ఒకవేళ (has_gvim)
    {
	ఒకవేళ (build_shortcut("gVim", "gvim.exe", "",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    తిరిగి;
	ఒకవేళ (build_shortcut("gVim Easy", "gvim.exe", "-y",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    తిరిగి;
	ఒకవేళ (build_shortcut("gVim Read-only", "gvim.exe", "-R",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    తిరిగి;
	ఒకవేళ (build_shortcut("gVim Diff", "gvim.exe", "-d",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    తిరిగి;
    }
    ఒకవేళ (build_shortcut("Uninstall",
		interactive ? "uninstall.exe" : "uninstall-gui.exe", "",
					   VIM_STARTMENU, installdir) == FAIL)
	తిరిగి;
    // For Windows NT the working dir of the vimtutor.bat must be right,
    // otherwise gvim.exe won't be found and using gvimbat doesn't work.
    ఒకవేళ (build_shortcut("Vim tutor", "vimtutor.bat", "",
					   VIM_STARTMENU, installdir) == FAIL)
	తిరిగి;
    ఒకవేళ (build_shortcut("Help", has_gvim ? "gvim.exe" : "vim.exe", "-c h",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	తిరిగి;
    {
	అక్షరం	shell_folder_మార్గం[BUFSIZE];

	// Creating the URL shortcut works a bit differently...
	ఒకవేళ (get_shell_folder_మార్గం(shell_folder_మార్గం, VIM_STARTMENU) == FAIL)
	{
	    రాయి("Finding the path of the Start menu failed\n");
	    తిరిగి ;
	}
	కూడు_pathsep(shell_folder_మార్గం);
	strcat(shell_folder_మార్గం, "Vim Online.url");
	ఒకవేళ (!Write_వ్యక్తిగత_Profile_String("InternetShortcut", "URL",
				    "https://www.vim.org/", shell_folder_మార్గం))
	{
	    రాయి("Creating the Vim online URL failed\n");
	    తిరిగి;
	}
    }
}

    స్థిర శూన్య
toggle_startmenu_choice(పూర్ణాంకం idx)
{
    ఒకవేళ (choices[idx].installfunc == ఖాళీ)
    {
	choices[idx].installfunc = install_ప్రారంభం_menu;
	choices[idx].పాఠ్యం = "Add Vim to the Start menu";
    }
    లేకపోతే
    {
	choices[idx].installfunc = ఖాళీ;
	choices[idx].పాఠ్యం = "Do NOT add Vim to the Start menu";
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
    శూన్య
install_shortcut_gvim(పూర్ణాంకం idx)
{
    // Create shortcut(s) on the desktop
    ఒకవేళ (choices[idx].arg)
    {
	(శూన్య)build_shortcut(icon_పేర్లు[0], "gvim.exe",
						      "", "desktop", WORKDIR);
	need_uninstall_నమోదు = 1;
    }
}

    శూన్య
install_shortcut_evim(పూర్ణాంకం idx)
{
    ఒకవేళ (choices[idx].arg)
    {
	(శూన్య)build_shortcut(icon_పేర్లు[1], "gvim.exe",
						    "-y", "desktop", WORKDIR);
	need_uninstall_నమోదు = 1;
    }
}

    శూన్య
install_shortcut_gview(పూర్ణాంకం idx)
{
    ఒకవేళ (choices[idx].arg)
    {
	(శూన్య)build_shortcut(icon_పేర్లు[2], "gvim.exe",
						    "-R", "desktop", WORKDIR);
	need_uninstall_నమోదు = 1;
    }
}

    శూన్య
toggle_shortcut_choice(పూర్ణాంకం idx)
{
    అక్షరం	*arg;

    ఒకవేళ (choices[idx].installfunc == install_shortcut_gvim)
	arg = "gVim";
    లేకపోతే ఒకవేళ (choices[idx].installfunc == install_shortcut_evim)
	arg = "gVim Easy";
    లేకపోతే
	arg = "gVim Read-only";
    ఒకవేళ (choices[idx].arg)
    {
	choices[idx].arg = 0;
	alloc_పాఠ్యం(idx, "Do NOT create a desktop icon for %s", arg);
    }
    లేకపోతే
    {
	choices[idx].arg = 1;
	alloc_పాఠ్యం(idx, "Create a desktop icon for %s", arg);
    }
}

    స్థిర శూన్య
init_startmenu_choice(శూన్య)
{
    // Start menu
    choices[choice_లెక్క].changefunc = toggle_startmenu_choice;
    choices[choice_లెక్క].installfunc = ఖాళీ;
    choices[choice_లెక్క].చురుకైన = 1;
    toggle_startmenu_choice(choice_లెక్క);	// set the text
    ++choice_లెక్క;
}

/*
 * Add the choice for the desktop shortcuts.
 */
    స్థిర శూన్య
init_shortcut_choices(శూన్య)
{
    // Shortcut to gvim
    choices[choice_లెక్క].పాఠ్యం = ఖాళీ;
    choices[choice_లెక్క].arg = 0;
    choices[choice_లెక్క].చురుకైన = has_gvim;
    choices[choice_లెక్క].changefunc = toggle_shortcut_choice;
    choices[choice_లెక్క].installfunc = install_shortcut_gvim;
    toggle_shortcut_choice(choice_లెక్క);
    ++choice_లెక్క;

    // Shortcut to evim
    choices[choice_లెక్క].పాఠ్యం = ఖాళీ;
    choices[choice_లెక్క].arg = 0;
    choices[choice_లెక్క].చురుకైన = has_gvim;
    choices[choice_లెక్క].changefunc = toggle_shortcut_choice;
    choices[choice_లెక్క].installfunc = install_shortcut_evim;
    toggle_shortcut_choice(choice_లెక్క);
    ++choice_లెక్క;

    // Shortcut to gview
    choices[choice_లెక్క].పాఠ్యం = ఖాళీ;
    choices[choice_లెక్క].arg = 0;
    choices[choice_లెక్క].చురుకైన = has_gvim;
    choices[choice_లెక్క].changefunc = toggle_shortcut_choice;
    choices[choice_లెక్క].installfunc = install_shortcut_gview;
    toggle_shortcut_choice(choice_లెక్క);
    ++choice_లెక్క;
}

/*
 * Attempt to register OLE for Vim.
 */
   స్థిర శూన్య
install_OLE_రిజిస్టరు(శూన్య)
{
    అక్షరం రిజిస్టరు_command_string[BUFSIZE + 30];

    రాయి("\n--- Attempting to register Vim with OLE ---\n");
    రాయి("(There is no message whether this works or not.)\n");

    sprintf(రిజిస్టరు_command_string, "\"%s\\gvim.exe\" -silent -register", installdir);
    system(రిజిస్టరు_command_string);
}

/*
 * Remove the last part of directory "path[]" to get its parent, and put the
 * result in "to[]".
 */
    స్థిర శూన్య
dir_remove_చివరి(స్థిరరాశి అక్షరం *మార్గం, అక్షరం గమ్యం[MAX_మార్గం])
{
    అక్షరం c;
    దీర్ఘ చివరి_char_గమ్యం_copy;
    దీర్ఘ మార్గం_నిడివి = అక్షరశ్రేణి_పొడవు(మార్గం);

    // skip the last character just in case it is a '\\'
    చివరి_char_గమ్యం_copy = మార్గం_నిడివి - 2;
    c = మార్గం[చివరి_char_గమ్యం_copy];

    అయితే (c != '\\')
    {
	చివరి_char_గమ్యం_copy--;
	c = మార్గం[చివరి_char_గమ్యం_copy];
    }

    strncpy(గమ్యం, మార్గం, (size_t)చివరి_char_గమ్యం_copy);
    గమ్యం[చివరి_char_గమ్యం_copy] = NUL;
}

    స్థిర శూన్య
సమితి_directories_పాఠ్యం(పూర్ణాంకం idx)
{
    పూర్ణాంకం vimfiles_dir_choice = choices[idx].arg;

    ఒకవేళ (vimfiles_dir_choice == (పూర్ణాంకం)vimfiles_dir_none)
	alloc_పాఠ్యం(idx, "Do NOT create plugin directories%s", "");
    లేకపోతే
	alloc_పాఠ్యం(idx, "Create plugin directories: %s",
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
స్థిర అక్షరం *homedir = ఖాళీ;

    శూన్య
init_homedir(శూన్య)
{
    అక్షరం    *var;
    అక్షరం    buf[MAX_మార్గం];

    ఒకవేళ (homedir != ఖాళీ)
    {
	విడుదల(homedir);
	homedir = ఖాళీ;
    }

    var = getenv("HOME");

    /*
     * Typically, $HOME is not defined on Windows, unless the user has
     * specifically defined it for Vim's sake.  However, on Windows NT
     * platforms, $HOMEDRIVE and $HOMEPATH are automatically defined for
     * each user.  Try constructing $HOME from these.
     */
    ఒకవేళ (var == ఖాళీ || *var == NUL)
    {
	అక్షరం	*homedrive, *homepath;

	homedrive = getenv("HOMEDRIVE");
	homepath = getenv("HOMEPATH");
	ఒకవేళ (homepath == ఖాళీ || *homepath == NUL)
	    homepath = "\\";
	ఒకవేళ (homedrive != ఖాళీ
		   && అక్షరశ్రేణి_పొడవు(homedrive) + అక్షరశ్రేణి_పొడవు(homepath) < పరిమాణం(buf))
	{
	    ఒకవేళ (snprintf(buf, పరిమాణం(buf), "%s%s", homedrive, homepath) > 0
		&& buf[0] != NUL)
		var = buf;
	}
    }

    ఒకవేళ (var == ఖాళీ)
	var = getenv("USERPROFILE");

    /*
     * Weird but true: $HOME may contain an indirect reference to another
     * variable, esp. "%USERPROFILE%".  Happens when $USERPROFILE isn't set
     * when $HOME is being set.
     */
    ఒకవేళ (var != ఖాళీ && *var == '%')
    {
	అక్షరం	*p;
	అక్షరం	*ఘాతాంకం;

	p = strchr(var + 1, '%');
	ఒకవేళ (p != ఖాళీ)
	{
	    strncpy(buf, var + 1, p - (var + 1));
	    buf[p - (var + 1)] = NUL;
	    ఘాతాంకం = getenv(buf);
	    ఒకవేళ (ఘాతాంకం != ఖాళీ && *ఘాతాంకం != NUL
				&& అక్షరశ్రేణి_పొడవు(ఘాతాంకం) + అక్షరశ్రేణి_పొడవు(p) < పరిమాణం(buf))
	    {
		snprintf(buf, పరిమాణం(buf), "%s%s", ఘాతాంకం, p + 1);
		var = buf;
	    }
	}
    }

    ఒకవేళ (var != ఖాళీ && *var == NUL)	// empty is same as not set
	var = ఖాళీ;

    ఒకవేళ (var == ఖాళీ)
	homedir = ఖాళీ;
    లేకపోతే
	homedir = _strdup(var);
}

/*
 * Change the directory that the vim plugin directories will be created in:
 * $HOME, $VIM or nowhere.
 */
    స్థిర శూన్య
change_directories_choice(పూర్ణాంకం idx)
{
    పూర్ణాంకం	    choice_లెక్క = పట్టిక_కొలత(vimfiles_dir_choices);

    // Don't offer the $HOME choice if $HOME isn't set.
    ఒకవేళ (homedir == ఖాళీ)
	--choice_లెక్క;
    choices[idx].arg = get_choice(vimfiles_dir_choices, choice_లెక్క);
    సమితి_directories_పాఠ్యం(idx);
}

/*
 * Create the plugin directories...
 */
//ARGSUSED
    స్థిర శూన్య
install_vimfilesdir(పూర్ణాంకం idx)
{
    పూర్ణాంకం i;
    పూర్ణాంకం vimfiles_dir_choice = choices[idx].arg;
    అక్షరం *p;
    అక్షరం vimdir_మార్గం[MAX_మార్గం];
    అక్షరం vimfiles_మార్గం[MAX_మార్గం + 9];
    అక్షరం tmp_dirname[BUFSIZE];

    // switch on the location that the user wants the plugin directories
    // built in
    ఎంపిక (vimfiles_dir_choice)
    {
	స్థితి vimfiles_dir_vim:
	{
	    // Go to the %VIM% directory - check env first, then go one dir
	    //	   below installdir if there is no %VIM% environment variable.
	    //	   The accuracy of $VIM is checked in inspect_system(), so we
	    //	   can be sure it is ok to use here.
	    p = getenv("VIM");
	    ఒకవేళ (p == ఖాళీ) // No $VIM in path
		dir_remove_చివరి(installdir, vimdir_మార్గం);
	    లేకపోతే
		strcpy(vimdir_మార్గం, p);
	    ఆపు;
	}
	స్థితి vimfiles_dir_home:
	{
	    // Find the $HOME directory.  Its existence was already checked.
	    p = homedir;
	    ఒకవేళ (p == ఖాళీ)
	    {
		రాయి("Internal error: $HOME is NULL\n");
		p = "c:\\";
	    }
	    strcpy(vimdir_మార్గం, p);
	    ఆపు;
	}
	స్థితి vimfiles_dir_none:
	{
	    // Do not create vim plugin directory.
	    తిరిగి;
	}
    }

    // Now, just create the directory.	If it already exists, it will fail
    // silently.
    sprintf(vimfiles_మార్గం, "%s\\vimfiles", vimdir_మార్గం);
    vim_mkdir(vimfiles_మార్గం, 0755);

    రాయి("Creating the following directories in \"%s\":\n", vimfiles_మార్గం);
    కొరకు (i = 0; i < పట్టిక_కొలత(vimfiles_subdirs); i++)
    {
	sprintf(tmp_dirname, "%s\\%s", vimfiles_మార్గం, vimfiles_subdirs[i]);
	రాయి("  %s", vimfiles_subdirs[i]);
	vim_mkdir(tmp_dirname, 0755);
    }
    రాయి("\n");
}

/*
 * Add the creation of runtime files to the setup sequence.
 */
    స్థిర శూన్య
init_directories_choice(శూన్య)
{
    నిర్మాణం stat	st;
    అక్షరం	tmp_dirname[BUFSIZE];
    అక్షరం	*p;
    పూర్ణాంకం		vimfiles_dir_choice;

    choices[choice_లెక్క].పాఠ్యం = alloc(150);
    choices[choice_లెక్క].changefunc = change_directories_choice;
    choices[choice_లెక్క].installfunc = install_vimfilesdir;
    choices[choice_లెక్క].చురుకైన = 1;

    // Check if the "compiler" directory already exists.  That's a good
    // indication that the plugin directories were already created.
    p = getenv("HOME");
    ఒకవేళ (p != ఖాళీ)
    {
	vimfiles_dir_choice = (పూర్ణాంకం)vimfiles_dir_home;
	sprintf(tmp_dirname, "%s\\vimfiles\\compiler", p);
	ఒకవేళ (stat(tmp_dirname, &st) == 0)
	    vimfiles_dir_choice = (పూర్ణాంకం)vimfiles_dir_none;
    }
    లేకపోతే
    {
	vimfiles_dir_choice = (పూర్ణాంకం)vimfiles_dir_vim;
	p = getenv("VIM");
	ఒకవేళ (p == ఖాళీ)  // No $VIM in path, use the install dir.
	    dir_remove_చివరి(installdir, tmp_dirname);
	లేకపోతే
	    strcpy(tmp_dirname, p);
	strcat(tmp_dirname, "\\vimfiles\\compiler");
	ఒకవేళ (stat(tmp_dirname, &st) == 0)
	    vimfiles_dir_choice = (పూర్ణాంకం)vimfiles_dir_none;
    }

    choices[choice_లెక్క].arg = vimfiles_dir_choice;
    సమితి_directories_పాఠ్యం(choice_లెక్క);
    ++choice_లెక్క;
}

/*
 * Setup the choices and the default values.
 */
    స్థిర శూన్య
అమరిక_choices(శూన్య)
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

    స్థిర శూన్య
print_cmd_పంక్తి_help(శూన్య)
{
    రాయి("Vim installer non-interactive command line arguments:\n");
    రాయి("\n");
    రాయి("-create-batfiles  [vim gvim evim view gview vimdiff gvimdiff]\n");
    రాయి("    Create .bat files for Vim variants in the Windows directory.\n");
    రాయి("-create-vimrc\n");
    రాయి("    Create a default _vimrc file if one does not already exist.\n");
    రాయి("-vimrc-remap [no|win]\n");
    రాయి("    Remap keys when creating a default _vimrc file.\n");
    రాయి("-vimrc-behave [unix|mswin|default]\n");
    రాయి("    Set mouse behavior when creating a default _vimrc file.\n");
    రాయి("-vimrc-compat [vi|vim|defaults|all]\n");
    రాయి("    Set Vi compatibility when creating a default _vimrc file.\n");
    రాయి("-install-popup\n");
    రాయి("    Install the Edit-with-Vim context menu entry\n");
    రాయి("-install-openwith\n");
    రాయి("    Add Vim to the \"Open With...\" context menu list\n");
    రాయి("-add-start-menu");
    రాయి("    Add Vim to the start menu\n");
    రాయి("-install-icons");
    రాయి("    Create icons for gVim executables on the desktop\n");
    రాయి("-create-directories [vim|home]\n");
    రాయి("    Create runtime directories to drop plugins into; in the $VIM\n");
    రాయి("    or $HOME directory\n");
    రాయి("-register-OLE");
    రాయి("    Ignored\n");
    రాయి("\n");
}

/*
 * Setup installation choices based on command line switches
 */
    స్థిర శూన్య
command_పంక్తి_అమరిక_choices(పూర్ణాంకం argc, అక్షరం **argv)
{
    పూర్ణాంకం i, j;

    కొరకు (i = 1; i < argc; i++)
    {
	ఒకవేళ (strcmp(argv[i], "-create-batfiles") == 0)
	{
	    ఒకవేళ (i + 1 == argc)
		కొనసాగు;
	    అయితే (argv[i + 1][0] != '-' && i < argc)
	    {
		i++;
		కొరకు (j = 1; j < లక్ష్యం_లెక్క; ++j)
		    ఒకవేళ ((targets[j].exenamearg[0] == 'g' ? has_gvim : has_vim)
			    && strcmp(argv[i], targets[j].name) == 0)
		    {
			init_bat_choice(j);
			ఆపు;
		    }
		ఒకవేళ (j == లక్ష్యం_లెక్క)
		    రాయి("%s is not a valid choice for -create-batfiles\n",
								     argv[i]);

		ఒకవేళ (i + 1 == argc)
		    ఆపు;
	    }
	}
	లేకపోతే ఒకవేళ (strcmp(argv[i], "-create-vimrc") == 0)
	{
	    // Setup default vimrc choices.  If there is already a _vimrc file,
	    // it will NOT be overwritten.
	    init_vimrc_choices();
	}
	లేకపోతే ఒకవేళ (strcmp(argv[i], "-vimrc-remap") == 0)
	{
	    ఒకవేళ (i + 1 == argc)
		ఆపు;
	    i++;
	    ఒకవేళ (strcmp(argv[i], "no") == 0)
		remap_choice = remap_no;
	    లేకపోతే ఒకవేళ (strcmp(argv[i], "win") == 0)
		remap_choice = remap_win;
	}
	లేకపోతే ఒకవేళ (strcmp(argv[i], "-vimrc-behave") == 0)
	{
	    ఒకవేళ (i + 1 == argc)
		ఆపు;
	    i++;
	    ఒకవేళ (strcmp(argv[i], "unix") == 0)
		mouse_choice = mouse_xterm;
	    లేకపోతే ఒకవేళ (strcmp(argv[i], "mswin") == 0)
		mouse_choice = mouse_mswin;
	    లేకపోతే ఒకవేళ (strcmp(argv[i], "default") == 0)
		mouse_choice = mouse_default;
	}
	లేకపోతే ఒకవేళ (strcmp(argv[i], "-vimrc-compat") == 0)
	{
	    ఒకవేళ (i + 1 == argc)
		ఆపు;
	    i++;
	    ఒకవేళ (strcmp(argv[i], "vi") == 0)
		compat_choice = compat_vi;
	    లేకపోతే ఒకవేళ (strcmp(argv[i], "vim") == 0)
		compat_choice = compat_vim;
	    లేకపోతే ఒకవేళ (strcmp(argv[i], "defaults") == 0)
		compat_choice = compat_some_enhancements;
	    లేకపోతే ఒకవేళ (strcmp(argv[i], "all") == 0)
		compat_choice = compat_all_enhancements;
	}
	లేకపోతే ఒకవేళ (strcmp(argv[i], "-install-popup") == 0)
	{
	    init_popup_choice();
	}
	లేకపోతే ఒకవేళ (strcmp(argv[i], "-install-openwith") == 0)
	{
	    init_openwith_choice();
	}
	లేకపోతే ఒకవేళ (strcmp(argv[i], "-add-start-menu") == 0)
	{
	    init_startmenu_choice();
	}
	లేకపోతే ఒకవేళ (strcmp(argv[i], "-install-icons") == 0)
	{
	    init_shortcut_choices();
	}
	లేకపోతే ఒకవేళ (strcmp(argv[i], "-create-directories") == 0)
	{
	    పూర్ణాంకం vimfiles_dir_choice = (పూర్ణాంకం)vimfiles_dir_none;

	    init_directories_choice();
	    ఒకవేళ (i + 1 < argc && argv[i + 1][0] != '-')
	    {
		i++;
		ఒకవేళ (strcmp(argv[i], "vim") == 0)
		    vimfiles_dir_choice = (పూర్ణాంకం)vimfiles_dir_vim;
		లేకపోతే ఒకవేళ (strcmp(argv[i], "home") == 0)
		{
		    ఒకవేళ (homedir == ఖాళీ)  // No $HOME in environment
			vimfiles_dir_choice = (పూర్ణాంకం)vimfiles_dir_none;
		    లేకపోతే
			vimfiles_dir_choice = (పూర్ణాంకం)vimfiles_dir_home;
		}
		లేకపోతే
		{
		    రాయి("Unknown argument for -create-directories: %s\n",
								     argv[i]);
		    print_cmd_పంక్తి_help();
		}
	    }
	    లేకపోతే // No choice specified, default to vim directory
		vimfiles_dir_choice = (పూర్ణాంకం)vimfiles_dir_vim;
	    choices[choice_లెక్క - 1].arg = vimfiles_dir_choice;
	}
	లేకపోతే ఒకవేళ (strcmp(argv[i], "-register-OLE") == 0)
	{
	    // This is always done when gvim is found
	}
	లేకపోతే // Unknown switch
	{
	    రాయి("Got unknown argument argv[%d] = %s\n", i, argv[i]);
	    print_cmd_పంక్తి_help();
	}
    }
}


/*
 * Show a few screens full of helpful information.
 */
    స్థిర శూన్య
show_help(శూన్య)
{
    స్థిర అక్షరం *(వస్తువులు[]) =
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
ఖాళీ
    };
    పూర్ణాంకం		i;
    పూర్ణాంకం		c;

    rewind(stdin);
    రాయి("\n");
    కొరకు (i = 0; వస్తువులు[i] != ఖాళీ; ++i)
    {
	puts(వస్తువులు[i]);
	రాయి("Hit Enter to continue, b (back) or q (quit help): ");
	c = getchar();
	rewind(stdin);
	ఒకవేళ (c == 'b' || c == 'B')
	{
	    ఒకవేళ (i == 0)
		--i;
	    లేకపోతే
		i -= 2;
	}
	ఒకవేళ (c == 'q' || c == 'Q')
	    ఆపు;
	రాయి("\n");
    }
}

/*
 * Install the choices.
 */
    స్థిర శూన్య
install(శూన్య)
{
    పూర్ణాంకం		i;

    // Install the selected choices.
    కొరకు (i = 0; i < choice_లెక్క; ++i)
	ఒకవేళ (choices[i].installfunc != ఖాళీ && choices[i].చురుకైన)
	    (choices[i].installfunc)(i);

    // Add some entries to the registry, if needed.
    ఒకవేళ (install_popup
	    || install_openwith
	    || (need_uninstall_నమోదు && interactive)
	    || !interactive)
	install_registry();

    // Register gvim with OLE.
    ఒకవేళ (has_gvim)
	install_OLE_రిజిస్టరు();
}

/*
 * request_choice
 */
    స్థిర శూన్య
request_choice(శూన్య)
{
    పూర్ణాంకం		      i;

    రాయి("\n\nInstall will do for you:\n");
    కొరకు (i = 0; i < choice_లెక్క; ++i)
	ఒకవేళ (choices[i].చురుకైన)
	    రాయి("%2d  %s\n", i + 1, choices[i].పాఠ్యం);
    రాయి("To change an item, enter its number\n\n");
    రాయి("Enter item number, h (help), d (do it) or q (quit): ");
}

    పూర్ణాంకం
ప్రధాన(పూర్ణాంకం argc, అక్షరం **argv)
{
    పూర్ణాంకం		i;
    అక్షరం	buf[BUFSIZE];

    /*
     * Run interactively if there are no command line arguments.
     */
    ఒకవేళ (argc > 1)
	interactive = 0;
    లేకపోతే
	interactive = 1;

    // Initialize this program.
    do_inits(argv);
    init_homedir();

    ఒకవేళ (argc > 1 && strcmp(argv[1], "-uninstall-check") == 0)
    {
	// Only check for already installed Vims.  Used by NSIS installer.
	i = uninstall_check(1);

	// Find the value of $VIM, because NSIS isn't able to do this by
	// itself.
	get_vim_env();

	// When nothing found exit quietly.  If something found wait for
	// a little while, so that the user can read the messages.
	ఒకవేళ (i && _isatty(1))
	    sleep(3);
	నిష్క్రమించు(0);
    }

    రాయి("This program sets up the installation of Vim "
						   VIM_సంస్కరణ_MEDIUM "\n\n");

    // Check if the user unpacked the archives properly.
    check_unpack();

    // Check for already installed Vims.
    ఒకవేళ (interactive)
	uninstall_check(0);

    // Find out information about the system.
    inspect_system();

    ఒకవేళ (interactive)
    {
	// Setup all the choices.
	అమరిక_choices();

	// Let the user change choices and finally install (or quit).
	కొరకు (;;)
	{
	    request_choice();
	    rewind(stdin);
	    ఒకవేళ (scanf("%99s", buf) == 1)
	    {
		ఒకవేళ (అంకెనా((గుర్తులేని అక్షరం)buf[0]))
		{
		    // Change a choice.
		    i = atoi(buf);
		    ఒకవేళ (i > 0 && i <= choice_లెక్క && choices[i - 1].చురుకైన)
			(choices[i - 1].changefunc)(i - 1);
		    లేకపోతే
			రాయి("\nIllegal choice\n");
		}
		లేకపోతే ఒకవేళ (buf[0] == 'h' || buf[0] == 'H')
		{
		    // Help
		    show_help();
		}
		లేకపోతే ఒకవేళ (buf[0] == 'd' || buf[0] == 'D')
		{
		    // Install!
		    install();
		    రాయి("\nThat finishes the installation.  Happy Vimming!\n");
		    ఆపు;
		}
		లేకపోతే ఒకవేళ (buf[0] == 'q' || buf[0] == 'Q')
		{
		    // Quit
		    రాయి("\nExiting without anything done\n");
		    ఆపు;
		}
		లేకపోతే
		    రాయి("\nIllegal choice\n");
	    }
	}
	రాయి("\n");
	myexit(0);
    }
    లేకపోతే
    {
	/*
	 * Run non-interactive - setup according to the command line switches
	 */
	command_పంక్తి_అమరిక_choices(argc, argv);
	install();

	// Avoid that the user has to hit Enter, just wait a little bit to
	// allow reading the messages.
	sleep(2);
    }

    తిరిగి 0;
}
