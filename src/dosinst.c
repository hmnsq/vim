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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DOSINST
#ਅਨ੍ਤਰ੍ਭਾਵ "dosinst.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <io.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GVIMEXT64_ਰਸਤਾ	    "GvimExt64\\gvimext.dll"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GVIMEXT32_ਰਸਤਾ	    "GvimExt32\\gvimext.dll"

// Macro to do an error check I was typing over and over
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CHECK_REG_ERROR(ਕੋਡ) \
    ਕਰੋ { \
	ਜੇ (ਕੋਡ != ERROR_SUCCESS) \
	{ \
	    ਲਿਖੋ("%ld error number:  %ld\n", (ਲੰਮਾ)__LINE__, (ਲੰਮਾ)ਕੋਡ); \
	    ਵਾਪਸ 1; \
	} \
    } ਜਦੋਂ (0)

ਪੂਰਨ	has_vim = 0;		// installable vim.exe exists
ਪੂਰਨ	has_gvim = 0;		// installable gvim.exe exists

ਅੱਖਰ	oldvimrc[BUFSIZE];	// name of existing vimrc file
ਅੱਖਰ	vimrc[BUFSIZE];		// name of vimrc file to create

ਅੱਖਰ	*default_bat_dir = ਨਿਰਾ;  // when not NULL, use this as the default
				  // directory to write .bat files in
ਅੱਖਰ	*default_vim_dir = ਨਿਰਾ;  // when not NULL, use this as the default
				  // install dir for NSIS

/*
 * Structure used for each choice the user can make.
 */
ਬਣਤਰ choice
{
    ਪੂਰਨ	    ਸਰਗਰਮ;			// non-zero when choice is active
    ਅੱਖਰ    *ਪਾਠ;			// text displayed for this choice
    ਖਾਲੀ    (*changefunc)(ਪੂਰਨ idx);	// function to change this choice
    ਪੂਰਨ	    arg;			// argument for function
    ਖਾਲੀ    (*installfunc)(ਪੂਰਨ idx);	// function to install this choice
};

ਬਣਤਰ choice	choices[30];		// choices the user can make
ਪੂਰਨ		choice_ਗਿਣਤੀ = 0;	// number of choices available

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਾਰਣੀ_ਮਾਪ(s)	(ਪੂਰਨ)ARRAYSIZE(s)

ਗਿਣਤੀ
{
    compat_vi = 1,
    compat_vim,
    compat_some_enhancements,
    compat_all_enhancements
};
ਅੱਖਰ	*(compat_choices[]) =
{
    "\nChoose the default way to run Vim:",
    "Vi compatible",
    "Vim default",
    "with some Vim enhancements",
    "with syntax highlighting and other features switched on",
};
ਪੂਰਨ	compat_choice = (ਪੂਰਨ)compat_all_enhancements;
ਅੱਖਰ	*compat_ਪਾਠ = "- run Vim %s";

ਗਿਣਤੀ
{
    remap_no = 1,
    remap_win
};
ਅੱਖਰ	*(remap_choices[]) =
{
    "\nChoose:",
    "Do not remap keys for Windows behavior",
    "Remap a few keys for Windows behavior (CTRL-V, CTRL-C, CTRL-F, etc)",
};
ਪੂਰਨ	remap_choice = (ਪੂਰਨ)remap_no;
ਅੱਖਰ	*remap_ਪਾਠ = "- %s";

ਗਿਣਤੀ
{
    mouse_xterm = 1,
    mouse_mswin,
    mouse_default
};
ਅੱਖਰ	*(mouse_choices[]) =
{
    "\nChoose the way how Vim uses the mouse:",
    "right button extends selection (the Unix way)",
    "right button has a popup menu, left button starts select mode (the Windows way)",
    "right button has a popup menu, left button starts visual mode",
};
ਪੂਰਨ	mouse_choice = (ਪੂਰਨ)mouse_default;
ਅੱਖਰ	*mouse_ਪਾਠ = "- The mouse %s";

ਗਿਣਤੀ
{
    vimfiles_dir_none = 1,
    vimfiles_dir_vim,
    vimfiles_dir_home
};
ਸਥਿਰ ਅੱਖਰ *(vimfiles_dir_choices[]) =
{
    "\nCreate plugin directories:",
    "No",
    "In the VIM directory",
    "In your HOME directory",
};

// non-zero when selected to install the popup menu entry.
ਸਥਿਰ ਪੂਰਨ	install_popup = 0;

// non-zero when selected to install the "Open with" entry.
ਸਥਿਰ ਪੂਰਨ	install_openwith = 0;

// non-zero when need to add an uninstall entry in the registry
ਸਥਿਰ ਪੂਰਨ	need_uninstall_ਇੰਦਰਾਜ = 0;

/*
 * Definitions of the directory name (under $VIM) of the vimfiles directory
 * and its subdirectories:
 */
ਸਥਿਰ ਅੱਖਰ	*(vimfiles_subdirs[]) =
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
    ਸਥਿਰ ਪੂਰਨ
get_choice(ਅੱਖਰ **ਸਾਰਣੀ, ਪੂਰਨ entries)
{
    ਪੂਰਨ		answer;
    ਪੂਰਨ		idx;
    ਅੱਖਰ	dummy[100];

    ਕਰੋ
    {
	ਲਈ (idx = 0; idx < entries; ++idx)
	{
	    ਜੇ (idx)
		ਲਿਖੋ("%2d  ", idx);
	    puts(ਸਾਰਣੀ[idx]);
	}
	ਲਿਖੋ("Choice: ");
	ਜੇ (scanf("%d", &answer) != 1)
	{
	    scanf("%99s", dummy);
	    answer = 0;
	}
    }
    ਜਦੋਂ (answer < 1 || answer >= entries);

    ਵਾਪਸ answer;
}

/*
 * Check if the user unpacked the archives properly.
 * Sets "runtimeidx".
 */
    ਸਥਿਰ ਖਾਲੀ
check_unpack(ਖਾਲੀ)
{
    ਅੱਖਰ	buf[BUFSIZE];
    FILE	*fd;
    ਬਣਤਰ stat	st;

    // check for presence of the correct version number in installdir[]
    runtimeidx = ਲੜੀ_ਲੰਬਾਈ(installdir) - ਲੜੀ_ਲੰਬਾਈ(VIM_ਸੰਸਕਰਣ_NODOT);
    ਜੇ (runtimeidx <= 0
	    || stricmp(installdir + runtimeidx, VIM_ਸੰਸਕਰਣ_NODOT) != 0
	    || (installdir[runtimeidx - 1] != '/'
		&& installdir[runtimeidx - 1] != '\\'))
    {
	ਲਿਖੋ("ERROR: Install program not in directory \"%s\"\n",
		VIM_ਸੰਸਕਰਣ_NODOT);
	ਲਿਖੋ("This program can only work when it is located in its original directory\n");
	myexit(1);
    }

    // check if filetype.vim is present, which means the runtime archive has
    // been unpacked
    sprintf(buf, "%s\\filetype.vim", installdir);
    ਜੇ (stat(buf, &st) < 0)
    {
	ਲਿਖੋ("ERROR: Cannot find filetype.vim in \"%s\"\n", installdir);
	ਲਿਖੋ("It looks like you did not unpack the runtime archive.\n");
	ਲਿਖੋ("You must unpack the runtime archive \"%srt.zip\" before installing.\n",
		VIM_ਸੰਸਕਰਣ_NODOT);
	myexit(1);
    }

    // Check if vim.exe or gvim.exe is in the current directory.
    ਜੇ ((fd = fopen("gvim.exe", "r")) != ਨਿਰਾ)
    {
	fclose(fd);
	has_gvim = 1;
    }
    ਜੇ ((fd = fopen("vim.exe", "r")) != ਨਿਰਾ)
    {
	fclose(fd);
	has_vim = 1;
    }
    ਜੇ (!has_gvim && !has_vim)
    {
	ਲਿਖੋ("ERROR: Cannot find any Vim executables in \"%s\"\n\n",
								  installdir);
	myexit(1);
    }
}

/*
 * Compare paths "p[plen]" to "q[qlen]".  Return 0 if they match.
 * Ignores case and differences between '/' and '\'.
 * "plen" and "qlen" can be negative, strlen() is used then.
 */
    ਸਥਿਰ ਪੂਰਨ
pathcmp(ਅੱਖਰ *p, ਪੂਰਨ plen, ਅੱਖਰ *q, ਪੂਰਨ qlen)
{
    ਪੂਰਨ		i;

    ਜੇ (plen < 0)
	plen = ਲੜੀ_ਲੰਬਾਈ(p);
    ਜੇ (qlen < 0)
	qlen = ਲੜੀ_ਲੰਬਾਈ(q);
    ਲਈ (i = 0; ; ++i)
    {
	// End of "p": check if "q" also ends or just has a slash.
	ਜੇ (i == plen)
	{
	    ਜੇ (i == qlen)  //ਮੇਲ
		ਵਾਪਸ 0;
	    ਜੇ (i == qlen - 1 && (q[i] == '\\' || q[i] == '/'))
		ਵਾਪਸ 0;   // match with trailing slash
	    ਵਾਪਸ 1;	    // no match
	}

	// End of "q": check if "p" also ends or just has a slash.
	ਜੇ (i == qlen)
	{
	    ਜੇ (i == plen)  //ਮੇਲ
		ਵਾਪਸ 0;
	    ਜੇ (i == plen - 1 && (p[i] == '\\' || p[i] == '/'))
		ਵਾਪਸ 0;   // match with trailing slash
	    ਵਾਪਸ 1;	    // no match
	}

	ਜੇ (!(mytoupper(p[i]) == mytoupper(q[i])
		|| ((p[i] == '/' || p[i] == '\\')
		    && (q[i] == '/' || q[i] == '\\'))))
	    ਵਾਪਸ 1;	    // no match
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
    ਸਥਿਰ ਖਾਲੀ
findoldfile(ਅੱਖਰ **destination)
{
    ਅੱਖਰ	*bp = *destination;
    size_t	indir_l = ਲੜੀ_ਲੰਬਾਈ(installdir);
    ਅੱਖਰ	*cp;
    ਅੱਖਰ	*tmpname;
    ਅੱਖਰ	*farname;

    /*
     * No action needed if exe not found or not in this directory.
     */
    ਜੇ (bp == ਨਿਰਾ || strnicmp(bp, installdir, indir_l) != 0)
	ਵਾਪਸ;
    cp = bp + indir_l;
    ਜੇ (strchr("/\\", *cp++) == ਨਿਰਾ
	    || strchr(cp, '\\') != ਨਿਰਾ
	    || strchr(cp, '/') != ਨਿਰਾ)
	ਵਾਪਸ;

    tmpname = alloc(ਲੜੀ_ਲੰਬਾਈ(cp) + 1);
    strcpy(tmpname, cp);
    tmpname[ਲੜੀ_ਲੰਬਾਈ(tmpname) - 1] = 'x';	// .exe -> .exx

    ਜੇ (access(tmpname, 0) == 0)
    {
	ਲਿਖੋ("\nERROR: %s and %s clash.  Remove or rename %s.\n",
	    tmpname, cp, tmpname);
	myexit(1);
    }

    ਜੇ (rename(cp, tmpname) != 0)
    {
	ਲਿਖੋ("\nERROR: failed to rename %s to %s: %s\n",
	    cp, tmpname, strerror(0));
	myexit(1);
    }

    farname = searchpath_save(cp);

    ਜੇ (rename(tmpname, cp) != 0)
    {
	ਲਿਖੋ("\nERROR: failed to rename %s back to %s: %s\n",
	    tmpname, cp, strerror(0));
	myexit(1);
    }

    ਮੁਕਤ(*destination);
    ਮੁਕਤ(tmpname);
    *destination = farname;
}

/*
 * Check if there is a vim.[exe|bat|, gvim.[exe|bat|, etc. in the path.
 * When "check_bat_only" is TRUE, only find "default_bat_dir".
 */
    ਸਥਿਰ ਖਾਲੀ
ਲੱਭੋ_bat_exe(ਪੂਰਨ check_bat_only)
{
    ਪੂਰਨ		i;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    ਲਈ (i = 1; i < ਨਿਸ਼ਾਨਾ_ਗਿਣਤੀ; ++i)
    {
	targets[i].oldbat = searchpath_save(targets[i].batname);
	ਜੇ (!check_bat_only)
	    targets[i].oldexe = searchpath_save(targets[i].exename);

	ਜੇ (default_bat_dir == ਨਿਰਾ && targets[i].oldbat != ਨਿਰਾ)
	{
	    default_bat_dir = alloc(ਲੜੀ_ਲੰਬਾਈ(targets[i].oldbat) + 1);
	    strcpy(default_bat_dir, targets[i].oldbat);
	    remove_ਪੂਛ(default_bat_dir);
	}
	ਜੇ (check_bat_only && targets[i].oldbat != ਨਿਰਾ)
	{
	    ਮੁਕਤ(targets[i].oldbat);
	    targets[i].oldbat = ਨਿਰਾ;
	}
    }

    mch_chdir(installdir);
}

/*
 * Get the value of $VIMRUNTIME or $VIM and write it in $TEMP/vimini.ini, so
 * that NSIS can read it.
 * When not set, use the directory of a previously installed Vim.
 */
    ਸਥਿਰ ਖਾਲੀ
get_vim_env(ਖਾਲੀ)
{
    ਅੱਖਰ	*vim;
    ਅੱਖਰ	buf[BUFSIZE];
    FILE	*fd;
    ਅੱਖਰ	fname[BUFSIZE];

    // First get $VIMRUNTIME.  If it's set, remove the tail.
    vim = getenv("VIMRUNTIME");
    ਜੇ (vim != ਨਿਰਾ && *vim != 0 && ਲੜੀ_ਲੰਬਾਈ(vim) < ਆਕਾਰ(buf))
    {
	strcpy(buf, vim);
	remove_ਪੂਛ(buf);
	vim = buf;
    }
    ਨਹੀਂਤਾਂ
    {
	vim = getenv("VIM");
	ਜੇ (vim == ਨਿਰਾ || *vim == 0)
	{
	    // Use the directory from an old uninstall entry.
	    ਜੇ (default_vim_dir != ਨਿਰਾ)
		vim = default_vim_dir;
	    ਨਹੀਂਤਾਂ
		// Let NSIS know there is no default, it should use
		// $PROGRAMFILES.
		vim = "";
	}
    }

    // NSIS also uses GetTempPath(), thus we should get the same directory
    // name as where NSIS will look for vimini.ini.
    Get_ਆਰਜ਼ੀ_ਰਸਤਾ(ਆਕਾਰ(fname) - 12, fname);
    ਜੋੜੋ_pathsep(fname);
    strcat(fname, "vimini.ini");

    fd = fopen(fname, "w");
    ਜੇ (fd != ਨਿਰਾ)
    {
	// Make it look like an .ini file, so that NSIS can read it with a
	// ReadINIStr command.
	fprintf(fd, "[vimini]\n");
	fprintf(fd, "dir=\"%s\"\n", vim);
	fclose(fd);
    }
    ਨਹੀਂਤਾਂ
    {
	ਲਿਖੋ("Failed to open %s\n", fname);
	sleep(2);
    }
}

ਸਥਿਰ ਪੂਰਨ ਅੰਕ_windows;

/*
 * Callback used for EnumWindows():
 * Count the window if the title looks like it is for the uninstaller.
 */
//ARGSUSED
    ਸਥਿਰ BOOL CALLBACK
ਖਿੜਕੀ_cb(HWND hwnd, LPARAM lparam ਅਣਵਰਤਿਆ)
{
    ਅੱਖਰ title[256];

    title[0] = 0;
    Get_ਖਿੜਕੀ_ਪਾਠ(hwnd, title, 256);
    ਜੇ (ਲੜੀ_ਖੋਜ(title, "Vim ") != ਨਿਰਾ && ਲੜੀ_ਖੋਜ(title, " Uninstall") != ਨਿਰਾ)
	++ਅੰਕ_windows;
    ਵਾਪਸ TRUE;
}

/*
 * Run the uninstaller silently.
 */
    ਸਥਿਰ ਪੂਰਨ
ਲੜੀਵਾਰ_silent_uninstall(ਅੱਖਰ *uninst_exe)
{
    ਅੱਖਰ    vimrt_dir[BUFSIZE];
    ਅੱਖਰ    ਆਰਜ਼ੀ_uninst[BUFSIZE];
    ਅੱਖਰ    ਆਰਜ਼ੀ_dir[MAX_ਰਸਤਾ];
    ਅੱਖਰ    buf[BUFSIZE * 2 + 10];
    ਪੂਰਨ	    i;
    DWORD   tick;

    strcpy(vimrt_dir, uninst_exe);
    remove_ਪੂਛ(vimrt_dir);

    ਜੇ (!Get_ਆਰਜ਼ੀ_ਰਸਤਾ(ਆਕਾਰ(ਆਰਜ਼ੀ_dir), ਆਰਜ਼ੀ_dir))
	ਵਾਪਸ FAIL;

    // Copy the uninstaller to a temporary exe.
    tick = Get_Tick_ਗਿਣਤੀ();
    ਲਈ (i = 0; ; i++)
    {
	sprintf(ਆਰਜ਼ੀ_uninst, "%s\\vimun%04X.exe", ਆਰਜ਼ੀ_dir,
					  (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)((i + tick) & 0xFFFF));
	ਜੇ (CopyFile(uninst_exe, ਆਰਜ਼ੀ_uninst, TRUE))
	    ਰੋਕੋ;
	ਜੇ (Get_ਆਖਰੀ_Error() != ERROR_FILE_EXISTS)
	    ਵਾਪਸ FAIL;
	ਜੇ (i == 65535)
	    ਵਾਪਸ FAIL;
    }

    // Run the copied uninstaller silently.
    ਜੇ (strchr(ਆਰਜ਼ੀ_uninst, ' ') != ਨਿਰਾ)
	sprintf(buf, "\"%s\" /S _?=%s", ਆਰਜ਼ੀ_uninst, vimrt_dir);
    ਨਹੀਂਤਾਂ
	sprintf(buf, "%s /S _?=%s", ਆਰਜ਼ੀ_uninst, vimrt_dir);
    ਲੜੀਵਾਰ_command(buf);

    DeleteFile(ਆਰਜ਼ੀ_uninst);
    ਵਾਪਸ OK;
}

/*
 * Check for already installed Vims.
 * Return non-zero when found one.
 */
    ਸਥਿਰ ਪੂਰਨ
uninstall_check(ਪੂਰਨ skip_question)
{
    HKEY	ਕੁੰਜੀ_handle;
    HKEY	uninstall_ਕੁੰਜੀ_handle;
    ਅੱਖਰ	*uninstall_ਕੁੰਜੀ = "software\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
    ਅੱਖਰ	subkey_name_buff[BUFSIZE];
    ਅੱਖਰ	ਆਰਜ਼ੀ_string_ਬਫਰ[BUFSIZE-2];
    DWORD	local_bufsize;
    FILETIME	ਆਰਜ਼ੀ_pfiletime;
    DWORD	ਕੁੰਜੀ_ਸੂਚਕਾਂਕ;
    ਅੱਖਰ	ਨਿਵੇਸ਼;
    ਲੰਮਾ	ਕੋਡ;
    DWORD	ਮੁੱਲ_type;
    DWORD	orig_ਅੰਕ_ਕੁੰਜੀਆਂ;
    DWORD	new_ਅੰਕ_ਕੁੰਜੀਆਂ;
    DWORD	allow_silent;
    ਪੂਰਨ		foundone = 0;

    ਕੋਡ = Reg_Open_ਕੁੰਜੀ_Ex(HKEY_LOCAL_MACHINE, uninstall_ਕੁੰਜੀ, 0,
				     ਕੁੰਜੀ_WOW64_64_ਕੁੰਜੀ | ਕੁੰਜੀ_READ, &ਕੁੰਜੀ_handle);
    CHECK_REG_ERROR(ਕੋਡ);

    ਕੁੰਜੀ_ਸੂਚਕਾਂਕ = 0;
    ਜਦੋਂ (TRUE)
    {
	local_bufsize = ਆਕਾਰ(subkey_name_buff);
	ਜੇ (Reg_Enum_ਕੁੰਜੀ_Ex(ਕੁੰਜੀ_handle, ਕੁੰਜੀ_ਸੂਚਕਾਂਕ, subkey_name_buff, &local_bufsize,
		ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, &ਆਰਜ਼ੀ_pfiletime) == ERROR_NO_ਹੋਰ_ਚੀਜ਼ਾਂ)
	    ਰੋਕੋ;

	ਜੇ (ਲੜੀ_ਤੁਲਨਾ("Vim", subkey_name_buff, 3) == 0)
	{
	    // Open the key named Vim*
	    ਕੋਡ = Reg_Open_ਕੁੰਜੀ_Ex(ਕੁੰਜੀ_handle, subkey_name_buff, 0,
			   ਕੁੰਜੀ_WOW64_64_ਕੁੰਜੀ | ਕੁੰਜੀ_READ, &uninstall_ਕੁੰਜੀ_handle);
	    CHECK_REG_ERROR(ਕੋਡ);

	    // get the DisplayName out of it to show the user
	    local_bufsize = ਆਕਾਰ(ਆਰਜ਼ੀ_string_ਬਫਰ);
	    ਕੋਡ = Reg_Query_ਮੁੱਲ_Ex(uninstall_ਕੁੰਜੀ_handle, "displayname", 0,
		    &ਮੁੱਲ_type, (LPBYTE)ਆਰਜ਼ੀ_string_ਬਫਰ,
		    &local_bufsize);
	    CHECK_REG_ERROR(ਕੋਡ);

	    allow_silent = 0;
	    ਜੇ (skip_question)
	    {
		DWORD varsize = ਆਕਾਰ(DWORD);

		Reg_Query_ਮੁੱਲ_Ex(uninstall_ਕੁੰਜੀ_handle, "AllowSilent", 0,
			&ਮੁੱਲ_type, (LPBYTE)&allow_silent,
			&varsize);
	    }

	    foundone = 1;
	    ਲਿਖੋ("\n*********************************************************\n");
	    ਲਿਖੋ("Vim Install found what looks like an existing Vim version.\n");
	    ਲਿਖੋ("The name of the entry is:\n");
	    ਲਿਖੋ("\n        \"%s\"\n\n", ਆਰਜ਼ੀ_string_ਬਫਰ);

	    ਲਿਖੋ("Installing the new version will disable part of the existing version.\n");
	    ਲਿਖੋ("(The batch files used in a console and the \"Edit with Vim\" entry in\n");
	    ਲਿਖੋ("the popup menu will use the new version)\n");

	    ਜੇ (skip_question)
		ਲਿਖੋ("\nRunning uninstall program for \"%s\"\n", ਆਰਜ਼ੀ_string_ਬਫਰ);
	    ਨਹੀਂਤਾਂ
		ਲਿਖੋ("\nDo you want to uninstall \"%s\" now?\n(y)es/(n)o)  ", ਆਰਜ਼ੀ_string_ਬਫਰ);
	    fflush(stdout);

	    // get the UninstallString
	    local_bufsize = ਆਕਾਰ(ਆਰਜ਼ੀ_string_ਬਫਰ);
	    ਕੋਡ = Reg_Query_ਮੁੱਲ_Ex(uninstall_ਕੁੰਜੀ_handle, "uninstallstring", 0,
		    &ਮੁੱਲ_type, (LPBYTE)ਆਰਜ਼ੀ_string_ਬਫਰ, &local_bufsize);
	    CHECK_REG_ERROR(ਕੋਡ);

	    // Remember the directory, it is used as the default for NSIS.
	    default_vim_dir = alloc(ਲੜੀ_ਲੰਬਾਈ(ਆਰਜ਼ੀ_string_ਬਫਰ) + 1);
	    strcpy(default_vim_dir, ਆਰਜ਼ੀ_string_ਬਫਰ);
	    remove_ਪੂਛ(default_vim_dir);
	    remove_ਪੂਛ(default_vim_dir);

	    ਨਿਵੇਸ਼ = 'n';
	    ਕਰੋ
	    {
		ਜੇ (ਨਿਵੇਸ਼ != 'n')
		    ਲਿਖੋ("%c is an invalid reply.  Please enter either 'y' or 'n'\n", ਨਿਵੇਸ਼);

		ਜੇ (skip_question)
		    ਨਿਵੇਸ਼ = 'y';
		ਨਹੀਂਤਾਂ
		{
		    rewind(stdin);
		    scanf("%c", &ਨਿਵੇਸ਼);
		}
		ਚੋਣ (ਨਿਵੇਸ਼)
		{
		    ਹਾਲਤ 'y':
		    ਹਾਲਤ 'Y':
			// save the number of uninstall keys so we can know if
			// it changed
			Reg_Query_Info_ਕੁੰਜੀ(ਕੁੰਜੀ_handle, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ,
					     &orig_ਅੰਕ_ਕੁੰਜੀਆਂ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ,
						      ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);

			// Find existing .bat files before deleting them.
			ਲੱਭੋ_bat_exe(TRUE);

			ਜੇ (allow_silent)
			{
			    ਜੇ (ਲੜੀਵਾਰ_silent_uninstall(ਆਰਜ਼ੀ_string_ਬਫਰ)
								    == FAIL)
				allow_silent = 0; // Retry with non silent.
			}
			ਜੇ (!allow_silent)
			{
			    // Execute the uninstall program.  Put it in double
			    // quotes if there is an embedded space.
			    {
				ਅੱਖਰ buf[BUFSIZE];

				ਜੇ (strchr(ਆਰਜ਼ੀ_string_ਬਫਰ, ' ') != ਨਿਰਾ)
				    sprintf(buf, "\"%s\"", ਆਰਜ਼ੀ_string_ਬਫਰ);
				ਨਹੀਂਤਾਂ
				    strcpy(buf, ਆਰਜ਼ੀ_string_ਬਫਰ);
				ਲੜੀਵਾਰ_command(buf);
			    }

			    // Count the number of windows with a title that
			    // match the installer, so that we can check when
			    // it's done.  The uninstaller copies itself,
			    // executes the copy and exits, thus we can't wait
			    // for the process to finish.
			    sleep(1);  // wait for uninstaller to start up
			    ਅੰਕ_windows = 0;
			    EnumWindows(ਖਿੜਕੀ_cb, 0);
			    ਜੇ (ਅੰਕ_windows == 0)
			    {
				// Did not find the uninstaller, ask user to
				// press Enter when done. Just in case.
				ਲਿਖੋ("Press Enter when the uninstaller is finished\n");
				rewind(stdin);
				(ਖਾਲੀ)getchar();
			    }
			    ਨਹੀਂਤਾਂ
			    {
				ਲਿਖੋ("Waiting for the uninstaller to finish (press CTRL-C to abort).");
				ਕਰੋ
				{
				    ਲਿਖੋ(".");
				    fflush(stdout);
				    sleep(1);	// wait for the uninstaller to
						//ਅੰਤ
				    ਅੰਕ_windows = 0;
				    EnumWindows(ਖਿੜਕੀ_cb, 0);
				} ਜਦੋਂ (ਅੰਕ_windows > 0);
			    }
			}
			ਲਿਖੋ("\nDone!\n");

			// Check if an uninstall reg key was deleted.
			// if it was, we want to decrement key_index.
			// if we don't do this, we will skip the key
			// immediately after any key that we delete.
			Reg_Query_Info_ਕੁੰਜੀ(ਕੁੰਜੀ_handle, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ,
					      &new_ਅੰਕ_ਕੁੰਜੀਆਂ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ,
						      ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
			ਜੇ (new_ਅੰਕ_ਕੁੰਜੀਆਂ < orig_ਅੰਕ_ਕੁੰਜੀਆਂ)
			    ਕੁੰਜੀ_ਸੂਚਕਾਂਕ--;

			ਨਿਵੇਸ਼ = 'y';
			ਰੋਕੋ;

		    ਹਾਲਤ 'n':
		    ਹਾਲਤ 'N':
			// Do not uninstall
			ਨਿਵੇਸ਼ = 'n';
			ਰੋਕੋ;

		    ਮੂਲ: // just drop through and redo the loop
			ਰੋਕੋ;
		}

	    } ਜਦੋਂ (ਨਿਵੇਸ਼ != 'n' && ਨਿਵੇਸ਼ != 'y');

	    Reg_Close_ਕੁੰਜੀ(uninstall_ਕੁੰਜੀ_handle);
	}

	ਕੁੰਜੀ_ਸੂਚਕਾਂਕ++;
    }
    Reg_Close_ਕੁੰਜੀ(ਕੁੰਜੀ_handle);

    ਵਾਪਸ foundone;
}

/*
 * Find out information about the system.
 */
    ਸਥਿਰ ਖਾਲੀ
inspect_system(ਖਾਲੀ)
{
    ਅੱਖਰ	*p;
    ਅੱਖਰ	buf[BUFSIZE];
    FILE	*fd;
    ਪੂਰਨ		i;
    ਪੂਰਨ		foundone;

    // This may take a little while, let the user know what we're doing.
    ਲਿਖੋ("Inspecting system...\n");

    /*
     * If $VIM is set, check that it's pointing to our directory.
     */
    p = getenv("VIM");
    ਜੇ (p != ਨਿਰਾ && pathcmp(p, -1, installdir, runtimeidx - 1) != 0)
    {
	ਲਿਖੋ("------------------------------------------------------\n");
	ਲਿਖੋ("$VIM is set to \"%s\".\n", p);
	ਲਿਖੋ("This is different from where this version of Vim is:\n");
	strcpy(buf, installdir);
	*(buf + runtimeidx - 1) = NUL;
	ਲਿਖੋ("\"%s\"\n", buf);
	ਲਿਖੋ("You must adjust or remove the setting of $VIM,\n");
	ਜੇ (interactive)
	{
	    ਲਿਖੋ("to be able to use this install program.\n");
	    myexit(1);
	}
	ਲਿਖੋ("otherwise Vim WILL NOT WORK properly!\n");
	ਲਿਖੋ("------------------------------------------------------\n");
    }

    /*
     * If $VIMRUNTIME is set, check that it's pointing to our runtime directory.
     */
    p = getenv("VIMRUNTIME");
    ਜੇ (p != ਨਿਰਾ && pathcmp(p, -1, installdir, -1) != 0)
    {
	ਲਿਖੋ("------------------------------------------------------\n");
	ਲਿਖੋ("$VIMRUNTIME is set to \"%s\".\n", p);
	ਲਿਖੋ("This is different from where this version of Vim is:\n");
	ਲਿਖੋ("\"%s\"\n", installdir);
	ਲਿਖੋ("You must adjust or remove the setting of $VIMRUNTIME,\n");
	ਜੇ (interactive)
	{
	    ਲਿਖੋ("to be able to use this install program.\n");
	    myexit(1);
	}
	ਲਿਖੋ("otherwise Vim WILL NOT WORK properly!\n");
	ਲਿਖੋ("------------------------------------------------------\n");
    }

    /*
     * Check if there is a vim.[exe|bat|, gvim.[exe|bat|, etc. in the path.
     */
    ਲੱਭੋ_bat_exe(FALSE);

    /*
     * A .exe in the install directory may be found anyway on Windows 2000.
     * Check for this situation and find another executable if necessary.
     * w.briscoe@ponl.com 2001-01-20
     */
    foundone = 0;
    ਲਈ (i = 1; i < ਨਿਸ਼ਾਨਾ_ਗਿਣਤੀ; ++i)
    {
	findoldfile(&(targets[i].oldexe));
	ਜੇ (targets[i].oldexe != ਨਿਰਾ)
	    foundone = 1;
    }

    ਜੇ (foundone)
    {
	ਲਿਖੋ("Warning: Found Vim executable(s) in your $PATH:\n");
	ਲਈ (i = 1; i < ਨਿਸ਼ਾਨਾ_ਗਿਣਤੀ; ++i)
	    ਜੇ (targets[i].oldexe != ਨਿਰਾ)
		ਲਿਖੋ("%s\n", targets[i].oldexe);
	ਲਿਖੋ("It will be used instead of the version you are installing.\n");
	ਲਿਖੋ("Please delete or rename it, or adjust your $PATH setting.\n");
    }

    /*
     * Check if there is an existing ../_vimrc or ../.vimrc file.
     */
    strcpy(oldvimrc, installdir);
    strcpy(oldvimrc + runtimeidx, "_vimrc");
    ਜੇ ((fd = fopen(oldvimrc, "r")) == ਨਿਰਾ)
    {
	strcpy(oldvimrc + runtimeidx, "vimrc~1"); // short version of .vimrc
	ਜੇ ((fd = fopen(oldvimrc, "r")) == ਨਿਰਾ)
	{
	    strcpy(oldvimrc + runtimeidx, ".vimrc");
	    fd = fopen(oldvimrc, "r");
	}
    }
    ਜੇ (fd != ਨਿਰਾ)
	fclose(fd);
    ਨਹੀਂਤਾਂ
	*oldvimrc = NUL;
}

/*
 * Add a dummy choice to avoid that the numbering changes depending on items
 * in the environment.  The user may type a number he remembered without
 * looking.
 */
    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_dummy_choice(ਖਾਲੀ)
{
    choices[choice_ਗਿਣਤੀ].installfunc = ਨਿਰਾ;
    choices[choice_ਗਿਣਤੀ].ਸਰਗਰਮ = 0;
    choices[choice_ਗਿਣਤੀ].changefunc = ਨਿਰਾ;
    choices[choice_ਗਿਣਤੀ].ਪਾਠ = ਨਿਰਾ;
    choices[choice_ਗਿਣਤੀ].arg = 0;
    ++choice_ਗਿਣਤੀ;
}

////////////////////////////////////////////////
// stuff for creating the batch files.

/*
 * Install the vim.bat, gvim.bat, etc. files.
 */
    ਸਥਿਰ ਖਾਲੀ
install_bat_choice(ਪੂਰਨ idx)
{
    ਅੱਖਰ	*batpath = targets[choices[idx].arg].batpath;
    ਅੱਖਰ	*oldname = targets[choices[idx].arg].oldbat;
    ਅੱਖਰ	*exename = targets[choices[idx].arg].exenamearg;
    ਅੱਖਰ	*vimarg = targets[choices[idx].arg].exearg;
    FILE	*fd;

    ਜੇ (*batpath == NUL)
	ਵਾਪਸ;

    fd = fopen(batpath, "w");
    ਜੇ (fd == ਨਿਰਾ)
    {
	ਲਿਖੋ("\nERROR: Cannot open \"%s\" for writing.\n", batpath);
	ਵਾਪਸ;
    }

    need_uninstall_ਇੰਦਰਾਜ = 1;

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
	    VIM_ਸੰਸਕਰਣ_NODOT, exename, VIM_ਸੰਸਕਰਣ_NODOT);
    fprintf(fd, "if exist \"%%VIMRUNTIME%%\\%s\" set VIM_EXE_DIR=%%VIMRUNTIME%%\n", exename);
    fprintf(fd, "\n");

    // Give an error message when the executable could not be found.
    fprintf(fd, "if not exist \"%%VIM_EXE_DIR%%\\%s\" (\n", exename);
    fprintf(fd, "    echo \"%%VIM_EXE_DIR%%\\%s\" not found\n", exename);
    fprintf(fd, "    goto :eof\n");
    fprintf(fd, ")\n");
    fprintf(fd, "\n");

    ਜੇ (*exename == 'g')
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

    ਜੇ (*exename == 'g')
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
    ਨਹੀਂਤਾਂ
    {
	// Always use quotes, $VIM or $VIMRUNTIME might have a space.
	fprintf(fd, "\"%%VIM_EXE_DIR%%\\%s\" %s %%*\n",
		exename, vimarg);
    }

    fclose(fd);
    ਲਿਖੋ("%s has been %s\n", batpath,
	    oldname == ਨਿਰਾ ? "created" : "overwritten");
}

/*
 * Make the text string for choice "idx".
 * The format "fmt" is must have one %s item, which "arg" is used for.
 */
    ਸਥਿਰ ਖਾਲੀ
alloc_ਪਾਠ(ਪੂਰਨ idx, ਅੱਖਰ *fmt, ਅੱਖਰ *arg)
{
    ਜੇ (choices[idx].ਪਾਠ != ਨਿਰਾ)
	ਮੁਕਤ(choices[idx].ਪਾਠ);

    choices[idx].ਪਾਠ = alloc(ਲੜੀ_ਲੰਬਾਈ(fmt) + ਲੜੀ_ਲੰਬਾਈ(arg) - 1);
    sprintf(choices[idx].ਪਾਠ, fmt, arg);
}

/*
 * Toggle the "Overwrite .../vim.bat" to "Don't overwrite".
 */
    ਸਥਿਰ ਖਾਲੀ
toggle_bat_choice(ਪੂਰਨ idx)
{
    ਅੱਖਰ	*batname = targets[choices[idx].arg].batpath;
    ਅੱਖਰ	*oldname = targets[choices[idx].arg].oldbat;

    ਜੇ (*batname == NUL)
    {
	alloc_ਪਾਠ(idx, "    Overwrite %s", oldname);
	strcpy(batname, oldname);
    }
    ਨਹੀਂਤਾਂ
    {
	alloc_ਪਾਠ(idx, "    Do NOT overwrite %s", oldname);
	*batname = NUL;
    }
}

/*
 * Do some work for a batch file entry: Append the batch file name to the path
 * and set the text for the choice.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_bat_ਪਾਠ(ਪੂਰਨ idx, ਅੱਖਰ *batpath, ਅੱਖਰ *name)
{
    strcat(batpath, name);

    alloc_ਪਾਠ(idx, "    Create %s", batpath);
}

/*
 * Select a directory to write the batch file line.
 */
    ਸਥਿਰ ਖਾਲੀ
change_bat_choice(ਪੂਰਨ idx)
{
    ਅੱਖਰ	*ਰਸਤਾ;
    ਅੱਖਰ	*batpath;
    ਅੱਖਰ	*name;
    ਪੂਰਨ		n;
    ਅੱਖਰ	*s;
    ਅੱਖਰ	*p;
    ਪੂਰਨ		ਗਿਣਤੀ_2;
    ਅੱਖਰ	**ਨਾਂ = ਨਿਰਾ;
    ਪੂਰਨ		i;
    ਪੂਰਨ		ਨਿਸ਼ਾਨਾ = choices[idx].arg;

    name = targets[ਨਿਸ਼ਾਨਾ].batname;
    batpath = targets[ਨਿਸ਼ਾਨਾ].batpath;

    ਰਸਤਾ = getenv("PATH");
    ਜੇ (ਰਸਤਾ == ਨਿਰਾ)
    {
	ਲਿਖੋ("\nERROR: The variable $PATH is not set\n");
	ਵਾਪਸ;
    }

    /*
     * first round: count number of names in path;
     * second round: save names to names[].
     */
    ਲਈ (;;)
    {
	ਗਿਣਤੀ_2 = 1;
	ਲਈ (p = ਰਸਤਾ; *p; )
	{
	    s = strchr(p, ';');
	    ਜੇ (s == ਨਿਰਾ)
		s = p + ਲੜੀ_ਲੰਬਾਈ(p);
	    ਜੇ (ਨਾਂ != ਨਿਰਾ)
	    {
		ਨਾਂ[ਗਿਣਤੀ_2] = alloc(s - p + 1);
		strncpy(ਨਾਂ[ਗਿਣਤੀ_2], p, s - p);
		ਨਾਂ[ਗਿਣਤੀ_2][s - p] = NUL;
	    }
	    ++ਗਿਣਤੀ_2;
	    p = s;
	    ਜੇ (*p != NUL)
		++p;
	}
	ਜੇ (ਨਾਂ != ਨਿਰਾ)
	    ਰੋਕੋ;
	ਨਾਂ = alloc((ਗਿਣਤੀ_2 + 1) * ਆਕਾਰ(ਅੱਖਰ *));
    }
    ਨਾਂ[0] = alloc(50);
    sprintf(ਨਾਂ[0], "Select directory to create %s in:", name);
    ਨਾਂ[ਗਿਣਤੀ_2] = alloc(50);
    ਜੇ (choices[idx].arg == 0)
	sprintf(ਨਾਂ[ਗਿਣਤੀ_2], "Do not create any .bat file.");
    ਨਹੀਂਤਾਂ
	sprintf(ਨਾਂ[ਗਿਣਤੀ_2], "Do not create a %s file.", name);
    n = get_choice(ਨਾਂ, ਗਿਣਤੀ_2 + 1);

    ਜੇ (n == ਗਿਣਤੀ_2)
    {
	// Selected last item, don't create bat file.
	*batpath = NUL;
	ਜੇ (choices[idx].arg != 0)
	    alloc_ਪਾਠ(idx, "    Do NOT create %s", name);
    }
    ਨਹੀਂਤਾਂ
    {
	// Selected one of the paths.  For the first item only keep the path,
	// for the others append the batch file name.
	strcpy(batpath, ਨਾਂ[n]);
	ਜੋੜੋ_pathsep(batpath);
	ਜੇ (choices[idx].arg != 0)
	    ਸੈੱਟ_bat_ਪਾਠ(idx, batpath, name);
    }

    ਲਈ (i = 0; i <= ਗਿਣਤੀ_2; ++i)
	ਮੁਕਤ(ਨਾਂ[i]);
    ਮੁਕਤ(ਨਾਂ);
}

ਅੱਖਰ *bat_ਪਾਠ_yes = "Install .bat files to use Vim at the command line:";
ਅੱਖਰ *bat_ਪਾਠ_no = "do NOT install .bat files to use Vim at the command line";

    ਸਥਿਰ ਖਾਲੀ
change_main_bat_choice(ਪੂਰਨ idx)
{
    ਪੂਰਨ		i;

    // let the user select a default directory or NONE
    change_bat_choice(idx);

    ਜੇ (targets[0].batpath[0] != NUL)
	choices[idx].ਪਾਠ = bat_ਪਾਠ_yes;
    ਨਹੀਂਤਾਂ
	choices[idx].ਪਾਠ = bat_ਪਾਠ_no;

    // update the individual batch file selections
    ਲਈ (i = 1; i < ਨਿਸ਼ਾਨਾ_ਗਿਣਤੀ; ++i)
    {
	// Only make it active when the first item has a path and the vim.exe
	// or gvim.exe exists (there is a changefunc then).
	ਜੇ (targets[0].batpath[0] != NUL
		&& choices[idx + i].changefunc != ਨਿਰਾ)
	{
	    choices[idx + i].ਸਰਗਰਮ = 1;
	    ਜੇ (choices[idx + i].changefunc == change_bat_choice
		    && targets[i].batpath[0] != NUL)
	    {
		strcpy(targets[i].batpath, targets[0].batpath);
		ਸੈੱਟ_bat_ਪਾਠ(idx + i, targets[i].batpath, targets[i].batname);
	    }
	}
	ਨਹੀਂਤਾਂ
	    choices[idx + i].ਸਰਗਰਮ = 0;
    }
}

/*
 * Initialize a choice for creating a batch file.
 */
    ਸਥਿਰ ਖਾਲੀ
init_bat_choice(ਪੂਰਨ ਨਿਸ਼ਾਨਾ)
{
    ਅੱਖਰ	*batpath = targets[ਨਿਸ਼ਾਨਾ].batpath;
    ਅੱਖਰ	*oldbat = targets[ਨਿਸ਼ਾਨਾ].oldbat;
    ਅੱਖਰ	*p;
    ਪੂਰਨ		i;

    choices[choice_ਗਿਣਤੀ].arg = ਨਿਸ਼ਾਨਾ;
    choices[choice_ਗਿਣਤੀ].installfunc = install_bat_choice;
    choices[choice_ਗਿਣਤੀ].ਸਰਗਰਮ = 1;
    choices[choice_ਗਿਣਤੀ].ਪਾਠ = ਨਿਰਾ;	// will be set below
    ਜੇ (oldbat != ਨਿਰਾ)
    {
	// A [g]vim.bat exists: Only choice is to overwrite it or not.
	choices[choice_ਗਿਣਤੀ].changefunc = toggle_bat_choice;
	*batpath = NUL;
	toggle_bat_choice(choice_ਗਿਣਤੀ);
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (default_bat_dir != ਨਿਰਾ)
	    // Prefer using the same path as an existing .bat file.
	    strcpy(batpath, default_bat_dir);
	ਨਹੀਂਤਾਂ
	{
	    // No [g]vim.bat exists: Write it to a directory in $PATH.  Use
	    // $WINDIR by default, if it's empty the first item in $PATH.
	    p = getenv("WINDIR");
	    ਜੇ (p != ਨਿਰਾ && *p != NUL)
		strcpy(batpath, p);
	    ਨਹੀਂਤਾਂ
	    {
		p = getenv("PATH");
		ਜੇ (p == ਨਿਰਾ || *p == NUL)		// "cannot happen"
		    strcpy(batpath, "C:/Windows");
		ਨਹੀਂਤਾਂ
		{
		    i = 0;
		    ਜਦੋਂ (*p != NUL && *p != ';')
			batpath[i++] = *p++;
		    batpath[i] = NUL;
		}
	    }
	}
	ਜੋੜੋ_pathsep(batpath);
	ਸੈੱਟ_bat_ਪਾਠ(choice_ਗਿਣਤੀ, batpath, targets[ਨਿਸ਼ਾਨਾ].batname);

	choices[choice_ਗਿਣਤੀ].changefunc = change_bat_choice;
    }
    ++choice_ਗਿਣਤੀ;
}

/*
 * Set up the choices for installing .bat files.
 * For these items "arg" is the index in targets[].
 */
    ਸਥਿਰ ਖਾਲੀ
init_bat_choices(ਖਾਲੀ)
{
    ਪੂਰਨ		i;

    // The first item is used to switch installing batch files on/off and
    // setting the default path.
    choices[choice_ਗਿਣਤੀ].ਪਾਠ = bat_ਪਾਠ_yes;
    choices[choice_ਗਿਣਤੀ].changefunc = change_main_bat_choice;
    choices[choice_ਗਿਣਤੀ].installfunc = ਨਿਰਾ;
    choices[choice_ਗਿਣਤੀ].ਸਰਗਰਮ = 1;
    choices[choice_ਗਿਣਤੀ].arg = 0;
    ++choice_ਗਿਣਤੀ;

    // Add items for each batch file target.  Only used when not disabled by
    // the first item.  When a .exe exists, don't offer to create a .bat.
    ਲਈ (i = 1; i < ਨਿਸ਼ਾਨਾ_ਗਿਣਤੀ; ++i)
	ਜੇ (targets[i].oldexe == ਨਿਰਾ
		&& (targets[i].exenamearg[0] == 'g' ? has_gvim : has_vim))
	    init_bat_choice(i);
	ਨਹੀਂਤਾਂ
	    ਜੋੜੋ_dummy_choice();
}

/*
 * Install the vimrc file.
 */
    ਸਥਿਰ ਖਾਲੀ
install_vimrc(ਪੂਰਨ idx ਅਣਵਰਤਿਆ)
{
    FILE	*fd, *tfd;
    ਅੱਖਰ	*fname;

    // If an old vimrc file exists, overwrite it.
    // Otherwise create a new one.
    ਜੇ (*oldvimrc != NUL)
	fname = oldvimrc;
    ਨਹੀਂਤਾਂ
	fname = vimrc;

    fd = fopen(fname, "w");
    ਜੇ (fd == ਨਿਰਾ)
    {
	ਲਿਖੋ("\nERROR: Cannot open \"%s\" for writing.\n", fname);
	ਵਾਪਸ;
    }
    ਚੋਣ (compat_choice)
    {
	ਹਾਲਤ compat_vi:
		    fprintf(fd, "\" Vi compatible\n");
		    fprintf(fd, "set compatible\n");
		    ਰੋਕੋ;
	ਹਾਲਤ compat_vim:
		    fprintf(fd, "\" Vim's default behavior\n");
		    fprintf(fd, "if &compatible\n");
		    fprintf(fd, "  set nocompatible\n");
		    fprintf(fd, "endif\n");
		    ਰੋਕੋ;
	ਹਾਲਤ compat_some_enhancements:
		    fprintf(fd, "\" Vim with some enhancements\n");
		    fprintf(fd, "source $VIMRUNTIME/defaults.vim\n");
		    ਰੋਕੋ;
	ਹਾਲਤ compat_all_enhancements:
		    fprintf(fd, "\" Vim with all enhancements\n");
		    fprintf(fd, "source $VIMRUNTIME/vimrc_example.vim\n");
		    ਰੋਕੋ;
    }
    ਚੋਣ (remap_choice)
    {
	ਹਾਲਤ remap_no:
		    ਰੋਕੋ;
	ਹਾਲਤ remap_win:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Remap a few keys for Windows behavior\n");
		    fprintf(fd, "source $VIMRUNTIME/mswin.vim\n");
		    ਰੋਕੋ;
    }
    ਚੋਣ (mouse_choice)
    {
	ਹਾਲਤ mouse_xterm:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Mouse behavior (the Unix way)\n");
		    fprintf(fd, "behave xterm\n");
		    ਰੋਕੋ;
	ਹਾਲਤ mouse_mswin:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Mouse behavior (the Windows way)\n");
		    fprintf(fd, "behave mswin\n");
		    ਰੋਕੋ;
	ਹਾਲਤ mouse_default:
		    ਰੋਕੋ;
    }
    ਜੇ ((tfd = fopen("diff.exe", "r")) != ਨਿਰਾ)
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
    ਲਿਖੋ("%s has been written\n", fname);
}

    ਸਥਿਰ ਖਾਲੀ
change_vimrc_choice(ਪੂਰਨ idx)
{
    ਜੇ (choices[idx].installfunc != ਨਿਰਾ)
    {
	// Switch to NOT change or create a vimrc file.
	ਜੇ (*oldvimrc != NUL)
	    alloc_ਪਾਠ(idx, "Do NOT change startup file %s", oldvimrc);
	ਨਹੀਂਤਾਂ
	    alloc_ਪਾਠ(idx, "Do NOT create startup file %s", vimrc);
	choices[idx].installfunc = ਨਿਰਾ;
	choices[idx + 1].ਸਰਗਰਮ = 0;
	choices[idx + 2].ਸਰਗਰਮ = 0;
	choices[idx + 3].ਸਰਗਰਮ = 0;
    }
    ਨਹੀਂਤਾਂ
    {
	// Switch to change or create a vimrc file.
	ਜੇ (*oldvimrc != NUL)
	    alloc_ਪਾਠ(idx, "Overwrite startup file %s with:", oldvimrc);
	ਨਹੀਂਤਾਂ
	    alloc_ਪਾਠ(idx, "Create startup file %s with:", vimrc);
	choices[idx].installfunc = install_vimrc;
	choices[idx + 1].ਸਰਗਰਮ = 1;
	choices[idx + 2].ਸਰਗਰਮ = 1;
	choices[idx + 3].ਸਰਗਰਮ = 1;
    }
}

/*
 * Change the choice how to run Vim.
 */
    ਸਥਿਰ ਖਾਲੀ
change_ਲੜੀਵਾਰ_choice(ਪੂਰਨ idx)
{
    compat_choice = get_choice(compat_choices, ਸਾਰਣੀ_ਮਾਪ(compat_choices));
    alloc_ਪਾਠ(idx, compat_ਪਾਠ, compat_choices[compat_choice]);
}

/*
 * Change the choice if keys are to be remapped.
 */
    ਸਥਿਰ ਖਾਲੀ
change_remap_choice(ਪੂਰਨ idx)
{
    remap_choice = get_choice(remap_choices, ਸਾਰਣੀ_ਮਾਪ(remap_choices));
    alloc_ਪਾਠ(idx, remap_ਪਾਠ, remap_choices[remap_choice]);
}

/*
 * Change the choice how to select text.
 */
    ਸਥਿਰ ਖਾਲੀ
change_mouse_choice(ਪੂਰਨ idx)
{
    mouse_choice = get_choice(mouse_choices, ਸਾਰਣੀ_ਮਾਪ(mouse_choices));
    alloc_ਪਾਠ(idx, mouse_ਪਾਠ, mouse_choices[mouse_choice]);
}

    ਸਥਿਰ ਖਾਲੀ
init_vimrc_choices(ਖਾਲੀ)
{
    // set path for a new _vimrc file (also when not used)
    strcpy(vimrc, installdir);
    strcpy(vimrc + runtimeidx, "_vimrc");

    // Set opposite value and then toggle it by calling change_vimrc_choice()
    ਜੇ (*oldvimrc == NUL)
	choices[choice_ਗਿਣਤੀ].installfunc = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
	choices[choice_ਗਿਣਤੀ].installfunc = install_vimrc;
    choices[choice_ਗਿਣਤੀ].ਪਾਠ = ਨਿਰਾ;
    change_vimrc_choice(choice_ਗਿਣਤੀ);
    choices[choice_ਗਿਣਤੀ].changefunc = change_vimrc_choice;
    choices[choice_ਗਿਣਤੀ].ਸਰਗਰਮ = 1;
    ++choice_ਗਿਣਤੀ;

    // default way to run Vim
    alloc_ਪਾਠ(choice_ਗਿਣਤੀ, compat_ਪਾਠ, compat_choices[compat_choice]);
    choices[choice_ਗਿਣਤੀ].changefunc = change_ਲੜੀਵਾਰ_choice;
    choices[choice_ਗਿਣਤੀ].installfunc = ਨਿਰਾ;
    choices[choice_ਗਿਣਤੀ].ਸਰਗਰਮ = (*oldvimrc == NUL);
    ++choice_ਗਿਣਤੀ;

    // Whether to remap keys
    alloc_ਪਾਠ(choice_ਗਿਣਤੀ, remap_ਪਾਠ , remap_choices[remap_choice]);
    choices[choice_ਗਿਣਤੀ].changefunc = change_remap_choice;
    choices[choice_ਗਿਣਤੀ].installfunc = ਨਿਰਾ;
    choices[choice_ਗਿਣਤੀ].ਸਰਗਰਮ = (*oldvimrc == NUL);
    ++choice_ਗਿਣਤੀ;

    // default way to use the mouse
    alloc_ਪਾਠ(choice_ਗਿਣਤੀ, mouse_ਪਾਠ, mouse_choices[mouse_choice]);
    choices[choice_ਗਿਣਤੀ].changefunc = change_mouse_choice;
    choices[choice_ਗਿਣਤੀ].installfunc = ਨਿਰਾ;
    choices[choice_ਗਿਣਤੀ].ਸਰਗਰਮ = (*oldvimrc == NUL);
    ++choice_ਗਿਣਤੀ;
}

    ਸਥਿਰ LONG
reg_create_ਕੁੰਜੀ(
    HKEY ਜੜ੍ਹ,
    ਅਚਲ ਅੱਖਰ *subkey,
    PHKEY ph_ਕੁੰਜੀ,
    DWORD flag)
{
    DWORD disp;

    *ph_ਕੁੰਜੀ = ਨਿਰਾ;
    ਵਾਪਸ Reg_Create_ਕੁੰਜੀ_Ex(
		ਜੜ੍ਹ, subkey,
		0, ਨਿਰਾ, REG_OPTION_NON_VOLATILE,
		flag | ਕੁੰਜੀ_WRITE,
		ਨਿਰਾ, ph_ਕੁੰਜੀ, &disp);
}

    ਸਥਿਰ LONG
reg_ਸੈੱਟ_string_ਮੁੱਲ(
    HKEY h_ਕੁੰਜੀ,
    ਅਚਲ ਅੱਖਰ *ਮੁੱਲ_name,
    ਅਚਲ ਅੱਖਰ *ਅੰਕੜੇ)
{
    ਵਾਪਸ Reg_ਸੈੱਟ_ਮੁੱਲ_Ex(h_ਕੁੰਜੀ, ਮੁੱਲ_name, 0, REG_SZ,
				     (LPBYTE)ਅੰਕੜੇ, (DWORD)(1 + ਲੜੀ_ਲੰਬਾਈ(ਅੰਕੜੇ)));
}

    ਸਥਿਰ LONG
reg_create_ਕੁੰਜੀ_and_ਮੁੱਲ(
    HKEY h_ਜੜ੍ਹ_ਕੁੰਜੀ,
    ਅਚਲ ਅੱਖਰ *subkey,
    ਅਚਲ ਅੱਖਰ *ਮੁੱਲ_name,
    ਅਚਲ ਅੱਖਰ *ਅੰਕੜੇ,
    DWORD flag)
{
    HKEY h_ਕੁੰਜੀ;
    LONG lRet = reg_create_ਕੁੰਜੀ(h_ਜੜ੍ਹ_ਕੁੰਜੀ, subkey, &h_ਕੁੰਜੀ, flag);

    ਜੇ (ERROR_SUCCESS == lRet)
    {
	lRet = reg_ਸੈੱਟ_string_ਮੁੱਲ(h_ਕੁੰਜੀ, ਮੁੱਲ_name, ਅੰਕੜੇ);
	Reg_Close_ਕੁੰਜੀ(h_ਕੁੰਜੀ);
    }
    ਵਾਪਸ lRet;
}

    ਸਥਿਰ LONG
ਰਜਿਸਟਰ_inproc_server(
    HKEY h_ਜੜ੍ਹ_ਕੁੰਜੀ,
    ਅਚਲ ਅੱਖਰ *clsid,
    ਅਚਲ ਅੱਖਰ *extname,
    ਅਚਲ ਅੱਖਰ *module,
    ਅਚਲ ਅੱਖਰ *threading_model,
    DWORD flag)
{
    CHAR subkey[BUFSIZE];
    LONG lRet;

    sprintf(subkey, "CLSID\\%s", clsid);
    lRet = reg_create_ਕੁੰਜੀ_and_ਮੁੱਲ(h_ਜੜ੍ਹ_ਕੁੰਜੀ, subkey, ਨਿਰਾ, extname, flag);
    ਜੇ (ERROR_SUCCESS == lRet)
    {
	sprintf(subkey, "CLSID\\%s\\InProcServer32", clsid);
	lRet = reg_create_ਕੁੰਜੀ_and_ਮੁੱਲ(h_ਜੜ੍ਹ_ਕੁੰਜੀ, subkey, ਨਿਰਾ, module, flag);
	ਜੇ (ERROR_SUCCESS == lRet)
	{
	    lRet = reg_create_ਕੁੰਜੀ_and_ਮੁੱਲ(h_ਜੜ੍ਹ_ਕੁੰਜੀ, subkey,
					   "ThreadingModel", threading_model, flag);
	}
    }
    ਵਾਪਸ lRet;
}

    ਸਥਿਰ LONG
ਰਜਿਸਟਰ_shellex(
    HKEY h_ਜੜ੍ਹ_ਕੁੰਜੀ,
    ਅਚਲ ਅੱਖਰ *clsid,
    ਅਚਲ ਅੱਖਰ *name,
    ਅਚਲ ਅੱਖਰ *exe_ਰਸਤਾ,
    DWORD flag)
{
    LONG lRet = reg_create_ਕੁੰਜੀ_and_ਮੁੱਲ(
	    h_ਜੜ੍ਹ_ਕੁੰਜੀ,
	    "*\\shellex\\ContextMenuHandlers\\gvim",
	    ਨਿਰਾ,
	    clsid,
	    flag);

    ਜੇ (ERROR_SUCCESS == lRet)
    {
	lRet = reg_create_ਕੁੰਜੀ_and_ਮੁੱਲ(
		HKEY_LOCAL_MACHINE,
		"Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved",
		clsid,
		name,
		flag);

	ਜੇ (ERROR_SUCCESS == lRet)
	{
	    lRet = reg_create_ਕੁੰਜੀ_and_ਮੁੱਲ(
		    HKEY_LOCAL_MACHINE,
		    "Software\\Vim\\Gvim",
		    "path",
		    exe_ਰਸਤਾ,
		    flag);
	}
    }
    ਵਾਪਸ lRet;
}

    ਸਥਿਰ LONG
ਰਜਿਸਟਰ_openwith(
    HKEY h_ਜੜ੍ਹ_ਕੁੰਜੀ,
    ਅਚਲ ਅੱਖਰ *exe_ਰਸਤਾ,
    DWORD flag)
{
    ਅੱਖਰ	exe_cmd[BUFSIZE];
    LONG	lRet;

    sprintf(exe_cmd, "\"%s\" \"%%1\"", exe_ਰਸਤਾ);
    lRet = reg_create_ਕੁੰਜੀ_and_ਮੁੱਲ(
	    h_ਜੜ੍ਹ_ਕੁੰਜੀ,
	    "Applications\\gvim.exe\\shell\\edit\\command",
	    ਨਿਰਾ,
	    exe_cmd,
	    flag);

    ਜੇ (ERROR_SUCCESS == lRet)
    {
	ਪੂਰਨ i;
	ਸਥਿਰ ਅਚਲ ਅੱਖਰ *openwith[] = {
		".htm\\OpenWithList\\gvim.exe",
		".vim\\OpenWithList\\gvim.exe",
		"*\\OpenWithList\\gvim.exe",
	};

	ਲਈ (i = 0; ERROR_SUCCESS == lRet && i < (ਪੂਰਨ)ARRAYSIZE(openwith); i++)
	    lRet = reg_create_ਕੁੰਜੀ_and_ਮੁੱਲ(h_ਜੜ੍ਹ_ਕੁੰਜੀ, openwith[i], ਨਿਰਾ, "", flag);
    }

    ਵਾਪਸ lRet;
}

    ਸਥਿਰ LONG
ਰਜਿਸਟਰ_uninstall(
    HKEY h_ਜੜ੍ਹ_ਕੁੰਜੀ,
    ਅਚਲ ਅੱਖਰ *appname,
    ਅਚਲ ਅੱਖਰ *display_name,
    ਅਚਲ ਅੱਖਰ *uninstall_string,
    ਅਚਲ ਅੱਖਰ *display_icon,
    ਅਚਲ ਅੱਖਰ *display_ਸੰਸਕਰਣ,
    ਅਚਲ ਅੱਖਰ *publisher)
{
    LONG lRet = reg_create_ਕੁੰਜੀ_and_ਮੁੱਲ(h_ਜੜ੍ਹ_ਕੁੰਜੀ, appname,
			     "DisplayName", display_name, ਕੁੰਜੀ_WOW64_64_ਕੁੰਜੀ);

    ਜੇ (ERROR_SUCCESS == lRet)
	lRet = reg_create_ਕੁੰਜੀ_and_ਮੁੱਲ(h_ਜੜ੍ਹ_ਕੁੰਜੀ, appname,
		     "UninstallString", uninstall_string, ਕੁੰਜੀ_WOW64_64_ਕੁੰਜੀ);
    ਜੇ (ERROR_SUCCESS == lRet)
	lRet = reg_create_ਕੁੰਜੀ_and_ਮੁੱਲ(h_ਜੜ੍ਹ_ਕੁੰਜੀ, appname,
		     "DisplayIcon", display_icon, ਕੁੰਜੀ_WOW64_64_ਕੁੰਜੀ);
    ਜੇ (ERROR_SUCCESS == lRet)
	lRet = reg_create_ਕੁੰਜੀ_and_ਮੁੱਲ(h_ਜੜ੍ਹ_ਕੁੰਜੀ, appname,
		     "DisplayVersion", display_ਸੰਸਕਰਣ, ਕੁੰਜੀ_WOW64_64_ਕੁੰਜੀ);
    ਜੇ (ERROR_SUCCESS == lRet)
	lRet = reg_create_ਕੁੰਜੀ_and_ਮੁੱਲ(h_ਜੜ੍ਹ_ਕੁੰਜੀ, appname,
		     "Publisher", publisher, ਕੁੰਜੀ_WOW64_64_ਕੁੰਜੀ);
    ਵਾਪਸ lRet;
}

/*
 * Add some entries to the registry:
 * - to add "Edit with Vim" to the context * menu
 * - to add Vim to the "Open with..." list
 * - to uninstall Vim
 */
//ARGSUSED
    ਸਥਿਰ ਪੂਰਨ
install_registry(ਖਾਲੀ)
{
    LONG	lRet = ERROR_SUCCESS;
    ਅਚਲ ਅੱਖਰ	*vim_ext_ThreadingModel = "Apartment";
    ਅਚਲ ਅੱਖਰ	*vim_ext_name = "Vim Shell Extension";
    ਅਚਲ ਅੱਖਰ	*vim_ext_clsid = "{51EEE242-AD87-11d3-9C1E-0090278BBD99}";
    ਅੱਖਰ	vim_exe_ਰਸਤਾ[MAX_ਰਸਤਾ];
    ਅੱਖਰ	display_name[BUFSIZE];
    ਅੱਖਰ	uninstall_string[BUFSIZE];
    ਅੱਖਰ	icon_string[BUFSIZE];
    ਅੱਖਰ	ਸੰਸਕਰਣ_string[BUFSIZE];
    ਪੂਰਨ		i;
    ਪੂਰਨ		loop_ਗਿਣਤੀ = is_64bit_os() ? 2 : 1;
    DWORD	flag;

    sprintf(vim_exe_ਰਸਤਾ, "%s\\gvim.exe", installdir);

    ਜੇ (install_popup)
    {
	ਅੱਖਰ	    bufg[BUFSIZE];

	ਲਿਖੋ("Creating \"Edit with Vim\" popup menu entry\n");

	ਲਈ (i = 0; i < loop_ਗਿਣਤੀ; i++)
	{
	    ਜੇ (i == 0)
	    {
		sprintf(bufg, "%s\\" GVIMEXT32_ਰਸਤਾ, installdir);
		flag = ਕੁੰਜੀ_WOW64_32_ਕੁੰਜੀ;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		sprintf(bufg, "%s\\" GVIMEXT64_ਰਸਤਾ, installdir);
		flag = ਕੁੰਜੀ_WOW64_64_ਕੁੰਜੀ;
	    }

	    lRet = ਰਜਿਸਟਰ_inproc_server(
		    HKEY_CLASSES_ਜੜ੍ਹ, vim_ext_clsid, vim_ext_name,
		    bufg, vim_ext_ThreadingModel, flag);
	    ਜੇ (ERROR_SUCCESS != lRet)
		ਵਾਪਸ FAIL;
	    lRet = ਰਜਿਸਟਰ_shellex(
		    HKEY_CLASSES_ਜੜ੍ਹ, vim_ext_clsid, vim_ext_name,
		    vim_exe_ਰਸਤਾ, flag);
	    ਜੇ (ERROR_SUCCESS != lRet)
		ਵਾਪਸ FAIL;
	}
    }

    ਜੇ (install_openwith)
    {
	ਲਿਖੋ("Creating \"Open with ...\" list entry\n");

	ਲਈ (i = 0; i < loop_ਗਿਣਤੀ; i++)
	{
	    ਜੇ (i == 0)
		flag = ਕੁੰਜੀ_WOW64_32_ਕੁੰਜੀ;
	    ਨਹੀਂਤਾਂ
		flag = ਕੁੰਜੀ_WOW64_64_ਕੁੰਜੀ;

	    lRet = ਰਜਿਸਟਰ_openwith(HKEY_CLASSES_ਜੜ੍ਹ, vim_exe_ਰਸਤਾ, flag);
	    ਜੇ (ERROR_SUCCESS != lRet)
		ਵਾਪਸ FAIL;
	}
    }

    ਲਿਖੋ("Creating an uninstall entry\n");
    sprintf(display_name, "Vim " VIM_ਸੰਸਕਰਣ_SHORT
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _M_ARM64
	    " (arm64)"
#ਜਾਂਜੇ _M_X64
	    " (x64)"
#ਜੇ_ਅੰਤ
	    );

    // For the NSIS installer use the generated uninstaller.
    ਜੇ (interactive)
	sprintf(uninstall_string, "%s\\uninstall.exe", installdir);
    ਨਹੀਂਤਾਂ
	sprintf(uninstall_string, "%s\\uninstall-gui.exe", installdir);

    sprintf(icon_string, "%s\\gvim.exe,0", installdir);

    sprintf(ਸੰਸਕਰਣ_string, VIM_ਸੰਸਕਰਣ_SHORT "." VIM_ਸੰਸਕਰਣ_PATCHLEVEL_STR);

    lRet = ਰਜਿਸਟਰ_uninstall(
	HKEY_LOCAL_MACHINE,
	"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Vim " VIM_ਸੰਸਕਰਣ_SHORT,
	display_name,
	uninstall_string,
	icon_string,
	ਸੰਸਕਰਣ_string,
	"The Vim Project");
    ਜੇ (ERROR_SUCCESS != lRet)
	ਵਾਪਸ FAIL;

    ਵਾਪਸ OK;
}

    ਸਥਿਰ ਖਾਲੀ
change_popup_choice(ਪੂਰਨ idx)
{
    ਜੇ (install_popup == 0)
    {
	choices[idx].ਪਾਠ = "Install an entry for Vim in the popup menu for the right\n    mouse button so that you can edit any file with Vim";
	install_popup = 1;
    }
    ਨਹੀਂਤਾਂ
    {
	choices[idx].ਪਾਠ = "Do NOT install an entry for Vim in the popup menu for the\n    right mouse button to edit any file with Vim";
	install_popup = 0;
    }
}

/*
 * Only add the choice for the popup menu entry when gvim.exe was found and
 * both gvimext.dll and regedit.exe exist.
 */
    ਸਥਿਰ ਖਾਲੀ
init_popup_choice(ਖਾਲੀ)
{
    ਬਣਤਰ stat	st;

    ਜੇ (has_gvim
	    && (stat(GVIMEXT32_ਰਸਤਾ, &st) >= 0
		|| stat(GVIMEXT64_ਰਸਤਾ, &st) >= 0))
    {
	choices[choice_ਗਿਣਤੀ].changefunc = change_popup_choice;
	choices[choice_ਗਿਣਤੀ].installfunc = ਨਿਰਾ;
	choices[choice_ਗਿਣਤੀ].ਸਰਗਰਮ = 1;
	change_popup_choice(choice_ਗਿਣਤੀ);  // set the text
	++choice_ਗਿਣਤੀ;
    }
    ਨਹੀਂਤਾਂ
	ਜੋੜੋ_dummy_choice();
}

    ਸਥਿਰ ਖਾਲੀ
change_openwith_choice(ਪੂਰਨ idx)
{
    ਜੇ (install_openwith == 0)
    {
	choices[idx].ਪਾਠ = "Add Vim to the \"Open With...\" list in the popup menu for the right\n    mouse button so that you can edit any file with Vim";
	install_openwith = 1;
    }
    ਨਹੀਂਤਾਂ
    {
	choices[idx].ਪਾਠ = "Do NOT add Vim to the \"Open With...\" list in the popup menu for the\n    right mouse button to edit any file with Vim";
	install_openwith = 0;
    }
}

/*
 * Only add the choice for the open-with menu entry when gvim.exe was found
 * and regedit.exe exist.
 */
    ਸਥਿਰ ਖਾਲੀ
init_openwith_choice(ਖਾਲੀ)
{
    ਜੇ (has_gvim)
    {
	choices[choice_ਗਿਣਤੀ].changefunc = change_openwith_choice;
	choices[choice_ਗਿਣਤੀ].installfunc = ਨਿਰਾ;
	choices[choice_ਗਿਣਤੀ].ਸਰਗਰਮ = 1;
	change_openwith_choice(choice_ਗਿਣਤੀ);  // set the text
	++choice_ਗਿਣਤੀ;
    }
    ਨਹੀਂਤਾਂ
	ਜੋੜੋ_dummy_choice();
}

/*
 * Create a shell link.
 *
 * returns 0 on failure, non-zero on successful completion.
 *
 * NOTE:  Currently untested with mingw.
 */
    ਪੂਰਨ
create_shortcut(
	ਅਚਲ ਅੱਖਰ *shortcut_name,
	ਅਚਲ ਅੱਖਰ *iconfile_ਰਸਤਾ,
	ਪੂਰਨ	    iconindex,
	ਅਚਲ ਅੱਖਰ *shortcut_ਨਿਸ਼ਾਨਾ,
	ਅਚਲ ਅੱਖਰ *shortcut_ਦਲੀਲਾਂ,
	ਅਚਲ ਅੱਖਰ *workingdir
	)
{
    IShellLink	    *shelllink_ptr;
    HRESULT	    hres;
    IPersistFile	    *persistfile_ptr;

    // Initialize COM library
    hres = CoInitialize(ਨਿਰਾ);
    ਜੇ (!SUCCEEDED(hres))
    {
	ਲਿਖੋ("Error:  Could not open the COM library.  Not creating shortcut.\n");
	ਵਾਪਸ FAIL;
    }

    // Instantiate a COM object for the ShellLink, store a pointer to it
    // in shelllink_ptr.
    hres = CoCreateInstance(&CLSID_ShellLink,
			   ਨਿਰਾ,
			   CLSCTX_INPROC_SERVER,
			   &IID_IShellLink,
			   (ਖਾਲੀ **) &shelllink_ptr);

    ਜੇ (SUCCEEDED(hres)) // If the instantiation was successful...
    {
	// ...Then build a PersistFile interface for the ShellLink so we can
	// save it as a file after we build it.
	hres = shelllink_ptr->lpVtbl->QueryInterface(shelllink_ptr,
		&IID_IPersistFile, (ਖਾਲੀ **) &persistfile_ptr);

	ਜੇ (SUCCEEDED(hres))
	{
	    wchar_t wsz[BUFSIZE];

	    // translate the (possibly) multibyte shortcut filename to windows
	    // Unicode so it can be used as a file name.
	    Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char(CP_ACP, 0, shortcut_name, -1, wsz, ਆਕਾਰ(wsz)/ਆਕਾਰ(wsz[0]));

	    // set the attributes
	    shelllink_ptr->lpVtbl->ਸੈੱਟ_ਰਸਤਾ(shelllink_ptr, shortcut_ਨਿਸ਼ਾਨਾ);
	    shelllink_ptr->lpVtbl->ਸੈੱਟ_Working_Directory(shelllink_ptr,
								  workingdir);
	    shelllink_ptr->lpVtbl->ਸੈੱਟ_Icon_Location(shelllink_ptr,
						    iconfile_ਰਸਤਾ, iconindex);
	    shelllink_ptr->lpVtbl->ਸੈੱਟ_Arguments(shelllink_ptr, shortcut_ਦਲੀਲਾਂ);

	    // save the shortcut to a file and return the PersistFile object
	    persistfile_ptr->lpVtbl->Save(persistfile_ptr, wsz, 1);
	    persistfile_ptr->lpVtbl->Release(persistfile_ptr);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਲਿਖੋ("QueryInterface Error\n");
	    ਵਾਪਸ FAIL;
	}

	// Return the ShellLink object
	shelllink_ptr->lpVtbl->Release(shelllink_ptr);
    }
    ਨਹੀਂਤਾਂ
    {
	ਲਿਖੋ("CoCreateInstance Error - hres = %08x\n", (ਪੂਰਨ)hres);
	ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
}

/*
 * Build a path to where we will put a specified link.
 *
 * Return 0 on error, non-zero on success
 */
   ਪੂਰਨ
build_link_name(
	ਅੱਖਰ	   *link_ਰਸਤਾ,
	ਅਚਲ ਅੱਖਰ *link_name,
	ਅਚਲ ਅੱਖਰ *shell_folder_name)
{
    ਅੱਖਰ	shell_folder_ਰਸਤਾ[MAX_ਰਸਤਾ];

    ਜੇ (get_shell_folder_ਰਸਤਾ(shell_folder_ਰਸਤਾ, shell_folder_name) == FAIL)
    {
	ਲਿਖੋ("An error occurred while attempting to find the path to %s.\n",
							   shell_folder_name);
	ਵਾਪਸ FAIL;
    }

    // Make sure the directory exists (create Start Menu\Programs\Vim).
    // Ignore errors if it already exists.
    vim_mkdir(shell_folder_ਰਸਤਾ, 0755);

    // build the path to the shortcut and the path to gvim.exe
    sprintf(link_ਰਸਤਾ, "%s\\%s.lnk", shell_folder_ਰਸਤਾ, link_name);

    ਵਾਪਸ OK;
}

    ਸਥਿਰ ਪੂਰਨ
build_shortcut(
	ਅਚਲ ਅੱਖਰ *name,	// Name of the shortcut
	ਅਚਲ ਅੱਖਰ *exename,	// Name of the executable (e.g., vim.exe)
	ਅਚਲ ਅੱਖਰ *ਦਲੀਲਾਂ,
	ਅਚਲ ਅੱਖਰ *shell_folder,
	ਅਚਲ ਅੱਖਰ *workingdir)
{
    ਅੱਖਰ	executable_ਰਸਤਾ[BUFSIZE];
    ਅੱਖਰ	link_name[BUFSIZE];

    sprintf(executable_ਰਸਤਾ, "%s\\%s", installdir, exename);

    ਜੇ (build_link_name(link_name, name, shell_folder) == FAIL)
    {
	ਲਿਖੋ("An error has occurred.  A shortcut to %s will not be created %s.\n",
		name,
		*shell_folder == 'd' ? "on the desktop" : "in the Start menu");
	ਵਾਪਸ FAIL;
    }

    // Create the shortcut:
    ਵਾਪਸ create_shortcut(link_name, executable_ਰਸਤਾ, 0,
					   executable_ਰਸਤਾ, ਦਲੀਲਾਂ, workingdir);
}

/*
 * We used to use "homedir" as the working directory, but that is a bad choice
 * on multi-user systems.  However, not specifying a directory results in the
 * current directory to be c:\Windows\system32 on Windows 7. Use environment
 * variables instead.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WORKDIR "%HOMEDRIVE%%HOMEPATH%"

/*
 * Create shortcut(s) in the Start Menu\Programs\Vim folder.
 */
    ਸਥਿਰ ਖਾਲੀ
install_ਸ਼ੁਰੂ_menu(ਪੂਰਨ idx ਅਣਵਰਤਿਆ)
{
    need_uninstall_ਇੰਦਰਾਜ = 1;
    ਲਿਖੋ("Creating start menu\n");
    ਜੇ (has_vim)
    {
	ਜੇ (build_shortcut("Vim", "vim.exe", "",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ਵਾਪਸ;
	ਜੇ (build_shortcut("Vim Read-only", "vim.exe", "-R",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ਵਾਪਸ;
	ਜੇ (build_shortcut("Vim Diff", "vim.exe", "-d",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ਵਾਪਸ;
    }
    ਜੇ (has_gvim)
    {
	ਜੇ (build_shortcut("gVim", "gvim.exe", "",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ਵਾਪਸ;
	ਜੇ (build_shortcut("gVim Easy", "gvim.exe", "-y",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ਵਾਪਸ;
	ਜੇ (build_shortcut("gVim Read-only", "gvim.exe", "-R",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ਵਾਪਸ;
	ਜੇ (build_shortcut("gVim Diff", "gvim.exe", "-d",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ਵਾਪਸ;
    }
    ਜੇ (build_shortcut("Uninstall",
		interactive ? "uninstall.exe" : "uninstall-gui.exe", "",
					   VIM_STARTMENU, installdir) == FAIL)
	ਵਾਪਸ;
    // For Windows NT the working dir of the vimtutor.bat must be right,
    // otherwise gvim.exe won't be found and using gvimbat doesn't work.
    ਜੇ (build_shortcut("Vim tutor", "vimtutor.bat", "",
					   VIM_STARTMENU, installdir) == FAIL)
	ਵਾਪਸ;
    ਜੇ (build_shortcut("Help", has_gvim ? "gvim.exe" : "vim.exe", "-c h",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	ਵਾਪਸ;
    {
	ਅੱਖਰ	shell_folder_ਰਸਤਾ[BUFSIZE];

	// Creating the URL shortcut works a bit differently...
	ਜੇ (get_shell_folder_ਰਸਤਾ(shell_folder_ਰਸਤਾ, VIM_STARTMENU) == FAIL)
	{
	    ਲਿਖੋ("Finding the path of the Start menu failed\n");
	    ਵਾਪਸ ;
	}
	ਜੋੜੋ_pathsep(shell_folder_ਰਸਤਾ);
	strcat(shell_folder_ਰਸਤਾ, "Vim Online.url");
	ਜੇ (!Write_ਨਿੱਜੀ_Profile_String("InternetShortcut", "URL",
				    "https://www.vim.org/", shell_folder_ਰਸਤਾ))
	{
	    ਲਿਖੋ("Creating the Vim online URL failed\n");
	    ਵਾਪਸ;
	}
    }
}

    ਸਥਿਰ ਖਾਲੀ
toggle_startmenu_choice(ਪੂਰਨ idx)
{
    ਜੇ (choices[idx].installfunc == ਨਿਰਾ)
    {
	choices[idx].installfunc = install_ਸ਼ੁਰੂ_menu;
	choices[idx].ਪਾਠ = "Add Vim to the Start menu";
    }
    ਨਹੀਂਤਾਂ
    {
	choices[idx].installfunc = ਨਿਰਾ;
	choices[idx].ਪਾਠ = "Do NOT add Vim to the Start menu";
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
    ਖਾਲੀ
install_shortcut_gvim(ਪੂਰਨ idx)
{
    // Create shortcut(s) on the desktop
    ਜੇ (choices[idx].arg)
    {
	(ਖਾਲੀ)build_shortcut(icon_ਨਾਂ[0], "gvim.exe",
						      "", "desktop", WORKDIR);
	need_uninstall_ਇੰਦਰਾਜ = 1;
    }
}

    ਖਾਲੀ
install_shortcut_evim(ਪੂਰਨ idx)
{
    ਜੇ (choices[idx].arg)
    {
	(ਖਾਲੀ)build_shortcut(icon_ਨਾਂ[1], "gvim.exe",
						    "-y", "desktop", WORKDIR);
	need_uninstall_ਇੰਦਰਾਜ = 1;
    }
}

    ਖਾਲੀ
install_shortcut_gview(ਪੂਰਨ idx)
{
    ਜੇ (choices[idx].arg)
    {
	(ਖਾਲੀ)build_shortcut(icon_ਨਾਂ[2], "gvim.exe",
						    "-R", "desktop", WORKDIR);
	need_uninstall_ਇੰਦਰਾਜ = 1;
    }
}

    ਖਾਲੀ
toggle_shortcut_choice(ਪੂਰਨ idx)
{
    ਅੱਖਰ	*arg;

    ਜੇ (choices[idx].installfunc == install_shortcut_gvim)
	arg = "gVim";
    ਨਹੀਂਤਾਂ ਜੇ (choices[idx].installfunc == install_shortcut_evim)
	arg = "gVim Easy";
    ਨਹੀਂਤਾਂ
	arg = "gVim Read-only";
    ਜੇ (choices[idx].arg)
    {
	choices[idx].arg = 0;
	alloc_ਪਾਠ(idx, "Do NOT create a desktop icon for %s", arg);
    }
    ਨਹੀਂਤਾਂ
    {
	choices[idx].arg = 1;
	alloc_ਪਾਠ(idx, "Create a desktop icon for %s", arg);
    }
}

    ਸਥਿਰ ਖਾਲੀ
init_startmenu_choice(ਖਾਲੀ)
{
    // Start menu
    choices[choice_ਗਿਣਤੀ].changefunc = toggle_startmenu_choice;
    choices[choice_ਗਿਣਤੀ].installfunc = ਨਿਰਾ;
    choices[choice_ਗਿਣਤੀ].ਸਰਗਰਮ = 1;
    toggle_startmenu_choice(choice_ਗਿਣਤੀ);	// set the text
    ++choice_ਗਿਣਤੀ;
}

/*
 * Add the choice for the desktop shortcuts.
 */
    ਸਥਿਰ ਖਾਲੀ
init_shortcut_choices(ਖਾਲੀ)
{
    // Shortcut to gvim
    choices[choice_ਗਿਣਤੀ].ਪਾਠ = ਨਿਰਾ;
    choices[choice_ਗਿਣਤੀ].arg = 0;
    choices[choice_ਗਿਣਤੀ].ਸਰਗਰਮ = has_gvim;
    choices[choice_ਗਿਣਤੀ].changefunc = toggle_shortcut_choice;
    choices[choice_ਗਿਣਤੀ].installfunc = install_shortcut_gvim;
    toggle_shortcut_choice(choice_ਗਿਣਤੀ);
    ++choice_ਗਿਣਤੀ;

    // Shortcut to evim
    choices[choice_ਗਿਣਤੀ].ਪਾਠ = ਨਿਰਾ;
    choices[choice_ਗਿਣਤੀ].arg = 0;
    choices[choice_ਗਿਣਤੀ].ਸਰਗਰਮ = has_gvim;
    choices[choice_ਗਿਣਤੀ].changefunc = toggle_shortcut_choice;
    choices[choice_ਗਿਣਤੀ].installfunc = install_shortcut_evim;
    toggle_shortcut_choice(choice_ਗਿਣਤੀ);
    ++choice_ਗਿਣਤੀ;

    // Shortcut to gview
    choices[choice_ਗਿਣਤੀ].ਪਾਠ = ਨਿਰਾ;
    choices[choice_ਗਿਣਤੀ].arg = 0;
    choices[choice_ਗਿਣਤੀ].ਸਰਗਰਮ = has_gvim;
    choices[choice_ਗਿਣਤੀ].changefunc = toggle_shortcut_choice;
    choices[choice_ਗਿਣਤੀ].installfunc = install_shortcut_gview;
    toggle_shortcut_choice(choice_ਗਿਣਤੀ);
    ++choice_ਗਿਣਤੀ;
}

/*
 * Attempt to register OLE for Vim.
 */
   ਸਥਿਰ ਖਾਲੀ
install_OLE_ਰਜਿਸਟਰ(ਖਾਲੀ)
{
    ਅੱਖਰ ਰਜਿਸਟਰ_command_string[BUFSIZE + 30];

    ਲਿਖੋ("\n--- Attempting to register Vim with OLE ---\n");
    ਲਿਖੋ("(There is no message whether this works or not.)\n");

    sprintf(ਰਜਿਸਟਰ_command_string, "\"%s\\gvim.exe\" -silent -register", installdir);
    system(ਰਜਿਸਟਰ_command_string);
}

/*
 * Remove the last part of directory "path[]" to get its parent, and put the
 * result in "to[]".
 */
    ਸਥਿਰ ਖਾਲੀ
dir_remove_ਆਖਰੀ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ ਮੰਜ਼ਿਲ[MAX_ਰਸਤਾ])
{
    ਅੱਖਰ c;
    ਲੰਮਾ ਆਖਰੀ_char_ਮੰਜ਼ਿਲ_copy;
    ਲੰਮਾ ਰਸਤਾ_ਲੰਮਾਈ = ਲੜੀ_ਲੰਬਾਈ(ਰਸਤਾ);

    // skip the last character just in case it is a '\\'
    ਆਖਰੀ_char_ਮੰਜ਼ਿਲ_copy = ਰਸਤਾ_ਲੰਮਾਈ - 2;
    c = ਰਸਤਾ[ਆਖਰੀ_char_ਮੰਜ਼ਿਲ_copy];

    ਜਦੋਂ (c != '\\')
    {
	ਆਖਰੀ_char_ਮੰਜ਼ਿਲ_copy--;
	c = ਰਸਤਾ[ਆਖਰੀ_char_ਮੰਜ਼ਿਲ_copy];
    }

    strncpy(ਮੰਜ਼ਿਲ, ਰਸਤਾ, (size_t)ਆਖਰੀ_char_ਮੰਜ਼ਿਲ_copy);
    ਮੰਜ਼ਿਲ[ਆਖਰੀ_char_ਮੰਜ਼ਿਲ_copy] = NUL;
}

    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_directories_ਪਾਠ(ਪੂਰਨ idx)
{
    ਪੂਰਨ vimfiles_dir_choice = choices[idx].arg;

    ਜੇ (vimfiles_dir_choice == (ਪੂਰਨ)vimfiles_dir_none)
	alloc_ਪਾਠ(idx, "Do NOT create plugin directories%s", "");
    ਨਹੀਂਤਾਂ
	alloc_ਪਾਠ(idx, "Create plugin directories: %s",
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
ਸਥਿਰ ਅੱਖਰ *homedir = ਨਿਰਾ;

    ਖਾਲੀ
init_homedir(ਖਾਲੀ)
{
    ਅੱਖਰ    *var;
    ਅੱਖਰ    buf[MAX_ਰਸਤਾ];

    ਜੇ (homedir != ਨਿਰਾ)
    {
	ਮੁਕਤ(homedir);
	homedir = ਨਿਰਾ;
    }

    var = getenv("HOME");

    /*
     * Typically, $HOME is not defined on Windows, unless the user has
     * specifically defined it for Vim's sake.  However, on Windows NT
     * platforms, $HOMEDRIVE and $HOMEPATH are automatically defined for
     * each user.  Try constructing $HOME from these.
     */
    ਜੇ (var == ਨਿਰਾ || *var == NUL)
    {
	ਅੱਖਰ	*homedrive, *homepath;

	homedrive = getenv("HOMEDRIVE");
	homepath = getenv("HOMEPATH");
	ਜੇ (homepath == ਨਿਰਾ || *homepath == NUL)
	    homepath = "\\";
	ਜੇ (homedrive != ਨਿਰਾ
		   && ਲੜੀ_ਲੰਬਾਈ(homedrive) + ਲੜੀ_ਲੰਬਾਈ(homepath) < ਆਕਾਰ(buf))
	{
	    ਜੇ (snprintf(buf, ਆਕਾਰ(buf), "%s%s", homedrive, homepath) > 0
		&& buf[0] != NUL)
		var = buf;
	}
    }

    ਜੇ (var == ਨਿਰਾ)
	var = getenv("USERPROFILE");

    /*
     * Weird but true: $HOME may contain an indirect reference to another
     * variable, esp. "%USERPROFILE%".  Happens when $USERPROFILE isn't set
     * when $HOME is being set.
     */
    ਜੇ (var != ਨਿਰਾ && *var == '%')
    {
	ਅੱਖਰ	*p;
	ਅੱਖਰ	*ਘਾਤਾਂਕ;

	p = strchr(var + 1, '%');
	ਜੇ (p != ਨਿਰਾ)
	{
	    strncpy(buf, var + 1, p - (var + 1));
	    buf[p - (var + 1)] = NUL;
	    ਘਾਤਾਂਕ = getenv(buf);
	    ਜੇ (ਘਾਤਾਂਕ != ਨਿਰਾ && *ਘਾਤਾਂਕ != NUL
				&& ਲੜੀ_ਲੰਬਾਈ(ਘਾਤਾਂਕ) + ਲੜੀ_ਲੰਬਾਈ(p) < ਆਕਾਰ(buf))
	    {
		snprintf(buf, ਆਕਾਰ(buf), "%s%s", ਘਾਤਾਂਕ, p + 1);
		var = buf;
	    }
	}
    }

    ਜੇ (var != ਨਿਰਾ && *var == NUL)	// empty is same as not set
	var = ਨਿਰਾ;

    ਜੇ (var == ਨਿਰਾ)
	homedir = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
	homedir = _strdup(var);
}

/*
 * Change the directory that the vim plugin directories will be created in:
 * $HOME, $VIM or nowhere.
 */
    ਸਥਿਰ ਖਾਲੀ
change_directories_choice(ਪੂਰਨ idx)
{
    ਪੂਰਨ	    choice_ਗਿਣਤੀ = ਸਾਰਣੀ_ਮਾਪ(vimfiles_dir_choices);

    // Don't offer the $HOME choice if $HOME isn't set.
    ਜੇ (homedir == ਨਿਰਾ)
	--choice_ਗਿਣਤੀ;
    choices[idx].arg = get_choice(vimfiles_dir_choices, choice_ਗਿਣਤੀ);
    ਸੈੱਟ_directories_ਪਾਠ(idx);
}

/*
 * Create the plugin directories...
 */
//ARGSUSED
    ਸਥਿਰ ਖਾਲੀ
install_vimfilesdir(ਪੂਰਨ idx)
{
    ਪੂਰਨ i;
    ਪੂਰਨ vimfiles_dir_choice = choices[idx].arg;
    ਅੱਖਰ *p;
    ਅੱਖਰ vimdir_ਰਸਤਾ[MAX_ਰਸਤਾ];
    ਅੱਖਰ vimfiles_ਰਸਤਾ[MAX_ਰਸਤਾ + 9];
    ਅੱਖਰ tmp_dirname[BUFSIZE];

    // switch on the location that the user wants the plugin directories
    // built in
    ਚੋਣ (vimfiles_dir_choice)
    {
	ਹਾਲਤ vimfiles_dir_vim:
	{
	    // Go to the %VIM% directory - check env first, then go one dir
	    //	   below installdir if there is no %VIM% environment variable.
	    //	   The accuracy of $VIM is checked in inspect_system(), so we
	    //	   can be sure it is ok to use here.
	    p = getenv("VIM");
	    ਜੇ (p == ਨਿਰਾ) // No $VIM in path
		dir_remove_ਆਖਰੀ(installdir, vimdir_ਰਸਤਾ);
	    ਨਹੀਂਤਾਂ
		strcpy(vimdir_ਰਸਤਾ, p);
	    ਰੋਕੋ;
	}
	ਹਾਲਤ vimfiles_dir_home:
	{
	    // Find the $HOME directory.  Its existence was already checked.
	    p = homedir;
	    ਜੇ (p == ਨਿਰਾ)
	    {
		ਲਿਖੋ("Internal error: $HOME is NULL\n");
		p = "c:\\";
	    }
	    strcpy(vimdir_ਰਸਤਾ, p);
	    ਰੋਕੋ;
	}
	ਹਾਲਤ vimfiles_dir_none:
	{
	    // Do not create vim plugin directory.
	    ਵਾਪਸ;
	}
    }

    // Now, just create the directory.	If it already exists, it will fail
    // silently.
    sprintf(vimfiles_ਰਸਤਾ, "%s\\vimfiles", vimdir_ਰਸਤਾ);
    vim_mkdir(vimfiles_ਰਸਤਾ, 0755);

    ਲਿਖੋ("Creating the following directories in \"%s\":\n", vimfiles_ਰਸਤਾ);
    ਲਈ (i = 0; i < ਸਾਰਣੀ_ਮਾਪ(vimfiles_subdirs); i++)
    {
	sprintf(tmp_dirname, "%s\\%s", vimfiles_ਰਸਤਾ, vimfiles_subdirs[i]);
	ਲਿਖੋ("  %s", vimfiles_subdirs[i]);
	vim_mkdir(tmp_dirname, 0755);
    }
    ਲਿਖੋ("\n");
}

/*
 * Add the creation of runtime files to the setup sequence.
 */
    ਸਥਿਰ ਖਾਲੀ
init_directories_choice(ਖਾਲੀ)
{
    ਬਣਤਰ stat	st;
    ਅੱਖਰ	tmp_dirname[BUFSIZE];
    ਅੱਖਰ	*p;
    ਪੂਰਨ		vimfiles_dir_choice;

    choices[choice_ਗਿਣਤੀ].ਪਾਠ = alloc(150);
    choices[choice_ਗਿਣਤੀ].changefunc = change_directories_choice;
    choices[choice_ਗਿਣਤੀ].installfunc = install_vimfilesdir;
    choices[choice_ਗਿਣਤੀ].ਸਰਗਰਮ = 1;

    // Check if the "compiler" directory already exists.  That's a good
    // indication that the plugin directories were already created.
    p = getenv("HOME");
    ਜੇ (p != ਨਿਰਾ)
    {
	vimfiles_dir_choice = (ਪੂਰਨ)vimfiles_dir_home;
	sprintf(tmp_dirname, "%s\\vimfiles\\compiler", p);
	ਜੇ (stat(tmp_dirname, &st) == 0)
	    vimfiles_dir_choice = (ਪੂਰਨ)vimfiles_dir_none;
    }
    ਨਹੀਂਤਾਂ
    {
	vimfiles_dir_choice = (ਪੂਰਨ)vimfiles_dir_vim;
	p = getenv("VIM");
	ਜੇ (p == ਨਿਰਾ)  // No $VIM in path, use the install dir.
	    dir_remove_ਆਖਰੀ(installdir, tmp_dirname);
	ਨਹੀਂਤਾਂ
	    strcpy(tmp_dirname, p);
	strcat(tmp_dirname, "\\vimfiles\\compiler");
	ਜੇ (stat(tmp_dirname, &st) == 0)
	    vimfiles_dir_choice = (ਪੂਰਨ)vimfiles_dir_none;
    }

    choices[choice_ਗਿਣਤੀ].arg = vimfiles_dir_choice;
    ਸੈੱਟ_directories_ਪਾਠ(choice_ਗਿਣਤੀ);
    ++choice_ਗਿਣਤੀ;
}

/*
 * Setup the choices and the default values.
 */
    ਸਥਿਰ ਖਾਲੀ
ਤਿਆਰੀ_choices(ਖਾਲੀ)
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

    ਸਥਿਰ ਖਾਲੀ
print_cmd_ਸਤਰ_help(ਖਾਲੀ)
{
    ਲਿਖੋ("Vim installer non-interactive command line arguments:\n");
    ਲਿਖੋ("\n");
    ਲਿਖੋ("-create-batfiles  [vim gvim evim view gview vimdiff gvimdiff]\n");
    ਲਿਖੋ("    Create .bat files for Vim variants in the Windows directory.\n");
    ਲਿਖੋ("-create-vimrc\n");
    ਲਿਖੋ("    Create a default _vimrc file if one does not already exist.\n");
    ਲਿਖੋ("-vimrc-remap [no|win]\n");
    ਲਿਖੋ("    Remap keys when creating a default _vimrc file.\n");
    ਲਿਖੋ("-vimrc-behave [unix|mswin|default]\n");
    ਲਿਖੋ("    Set mouse behavior when creating a default _vimrc file.\n");
    ਲਿਖੋ("-vimrc-compat [vi|vim|defaults|all]\n");
    ਲਿਖੋ("    Set Vi compatibility when creating a default _vimrc file.\n");
    ਲਿਖੋ("-install-popup\n");
    ਲਿਖੋ("    Install the Edit-with-Vim context menu entry\n");
    ਲਿਖੋ("-install-openwith\n");
    ਲਿਖੋ("    Add Vim to the \"Open With...\" context menu list\n");
    ਲਿਖੋ("-add-start-menu");
    ਲਿਖੋ("    Add Vim to the start menu\n");
    ਲਿਖੋ("-install-icons");
    ਲਿਖੋ("    Create icons for gVim executables on the desktop\n");
    ਲਿਖੋ("-create-directories [vim|home]\n");
    ਲਿਖੋ("    Create runtime directories to drop plugins into; in the $VIM\n");
    ਲਿਖੋ("    or $HOME directory\n");
    ਲਿਖੋ("-register-OLE");
    ਲਿਖੋ("    Ignored\n");
    ਲਿਖੋ("\n");
}

/*
 * Setup installation choices based on command line switches
 */
    ਸਥਿਰ ਖਾਲੀ
command_ਸਤਰ_ਤਿਆਰੀ_choices(ਪੂਰਨ argc, ਅੱਖਰ **argv)
{
    ਪੂਰਨ i, j;

    ਲਈ (i = 1; i < argc; i++)
    {
	ਜੇ (strcmp(argv[i], "-create-batfiles") == 0)
	{
	    ਜੇ (i + 1 == argc)
		ਜਾਰੀ;
	    ਜਦੋਂ (argv[i + 1][0] != '-' && i < argc)
	    {
		i++;
		ਲਈ (j = 1; j < ਨਿਸ਼ਾਨਾ_ਗਿਣਤੀ; ++j)
		    ਜੇ ((targets[j].exenamearg[0] == 'g' ? has_gvim : has_vim)
			    && strcmp(argv[i], targets[j].name) == 0)
		    {
			init_bat_choice(j);
			ਰੋਕੋ;
		    }
		ਜੇ (j == ਨਿਸ਼ਾਨਾ_ਗਿਣਤੀ)
		    ਲਿਖੋ("%s is not a valid choice for -create-batfiles\n",
								     argv[i]);

		ਜੇ (i + 1 == argc)
		    ਰੋਕੋ;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (strcmp(argv[i], "-create-vimrc") == 0)
	{
	    // Setup default vimrc choices.  If there is already a _vimrc file,
	    // it will NOT be overwritten.
	    init_vimrc_choices();
	}
	ਨਹੀਂਤਾਂ ਜੇ (strcmp(argv[i], "-vimrc-remap") == 0)
	{
	    ਜੇ (i + 1 == argc)
		ਰੋਕੋ;
	    i++;
	    ਜੇ (strcmp(argv[i], "no") == 0)
		remap_choice = remap_no;
	    ਨਹੀਂਤਾਂ ਜੇ (strcmp(argv[i], "win") == 0)
		remap_choice = remap_win;
	}
	ਨਹੀਂਤਾਂ ਜੇ (strcmp(argv[i], "-vimrc-behave") == 0)
	{
	    ਜੇ (i + 1 == argc)
		ਰੋਕੋ;
	    i++;
	    ਜੇ (strcmp(argv[i], "unix") == 0)
		mouse_choice = mouse_xterm;
	    ਨਹੀਂਤਾਂ ਜੇ (strcmp(argv[i], "mswin") == 0)
		mouse_choice = mouse_mswin;
	    ਨਹੀਂਤਾਂ ਜੇ (strcmp(argv[i], "default") == 0)
		mouse_choice = mouse_default;
	}
	ਨਹੀਂਤਾਂ ਜੇ (strcmp(argv[i], "-vimrc-compat") == 0)
	{
	    ਜੇ (i + 1 == argc)
		ਰੋਕੋ;
	    i++;
	    ਜੇ (strcmp(argv[i], "vi") == 0)
		compat_choice = compat_vi;
	    ਨਹੀਂਤਾਂ ਜੇ (strcmp(argv[i], "vim") == 0)
		compat_choice = compat_vim;
	    ਨਹੀਂਤਾਂ ਜੇ (strcmp(argv[i], "defaults") == 0)
		compat_choice = compat_some_enhancements;
	    ਨਹੀਂਤਾਂ ਜੇ (strcmp(argv[i], "all") == 0)
		compat_choice = compat_all_enhancements;
	}
	ਨਹੀਂਤਾਂ ਜੇ (strcmp(argv[i], "-install-popup") == 0)
	{
	    init_popup_choice();
	}
	ਨਹੀਂਤਾਂ ਜੇ (strcmp(argv[i], "-install-openwith") == 0)
	{
	    init_openwith_choice();
	}
	ਨਹੀਂਤਾਂ ਜੇ (strcmp(argv[i], "-add-start-menu") == 0)
	{
	    init_startmenu_choice();
	}
	ਨਹੀਂਤਾਂ ਜੇ (strcmp(argv[i], "-install-icons") == 0)
	{
	    init_shortcut_choices();
	}
	ਨਹੀਂਤਾਂ ਜੇ (strcmp(argv[i], "-create-directories") == 0)
	{
	    ਪੂਰਨ vimfiles_dir_choice = (ਪੂਰਨ)vimfiles_dir_none;

	    init_directories_choice();
	    ਜੇ (i + 1 < argc && argv[i + 1][0] != '-')
	    {
		i++;
		ਜੇ (strcmp(argv[i], "vim") == 0)
		    vimfiles_dir_choice = (ਪੂਰਨ)vimfiles_dir_vim;
		ਨਹੀਂਤਾਂ ਜੇ (strcmp(argv[i], "home") == 0)
		{
		    ਜੇ (homedir == ਨਿਰਾ)  // No $HOME in environment
			vimfiles_dir_choice = (ਪੂਰਨ)vimfiles_dir_none;
		    ਨਹੀਂਤਾਂ
			vimfiles_dir_choice = (ਪੂਰਨ)vimfiles_dir_home;
		}
		ਨਹੀਂਤਾਂ
		{
		    ਲਿਖੋ("Unknown argument for -create-directories: %s\n",
								     argv[i]);
		    print_cmd_ਸਤਰ_help();
		}
	    }
	    ਨਹੀਂਤਾਂ // No choice specified, default to vim directory
		vimfiles_dir_choice = (ਪੂਰਨ)vimfiles_dir_vim;
	    choices[choice_ਗਿਣਤੀ - 1].arg = vimfiles_dir_choice;
	}
	ਨਹੀਂਤਾਂ ਜੇ (strcmp(argv[i], "-register-OLE") == 0)
	{
	    // This is always done when gvim is found
	}
	ਨਹੀਂਤਾਂ // Unknown switch
	{
	    ਲਿਖੋ("Got unknown argument argv[%d] = %s\n", i, argv[i]);
	    print_cmd_ਸਤਰ_help();
	}
    }
}


/*
 * Show a few screens full of helpful information.
 */
    ਸਥਿਰ ਖਾਲੀ
show_help(ਖਾਲੀ)
{
    ਸਥਿਰ ਅੱਖਰ *(ਚੀਜ਼ਾਂ[]) =
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
ਨਿਰਾ
    };
    ਪੂਰਨ		i;
    ਪੂਰਨ		c;

    rewind(stdin);
    ਲਿਖੋ("\n");
    ਲਈ (i = 0; ਚੀਜ਼ਾਂ[i] != ਨਿਰਾ; ++i)
    {
	puts(ਚੀਜ਼ਾਂ[i]);
	ਲਿਖੋ("Hit Enter to continue, b (back) or q (quit help): ");
	c = getchar();
	rewind(stdin);
	ਜੇ (c == 'b' || c == 'B')
	{
	    ਜੇ (i == 0)
		--i;
	    ਨਹੀਂਤਾਂ
		i -= 2;
	}
	ਜੇ (c == 'q' || c == 'Q')
	    ਰੋਕੋ;
	ਲਿਖੋ("\n");
    }
}

/*
 * Install the choices.
 */
    ਸਥਿਰ ਖਾਲੀ
install(ਖਾਲੀ)
{
    ਪੂਰਨ		i;

    // Install the selected choices.
    ਲਈ (i = 0; i < choice_ਗਿਣਤੀ; ++i)
	ਜੇ (choices[i].installfunc != ਨਿਰਾ && choices[i].ਸਰਗਰਮ)
	    (choices[i].installfunc)(i);

    // Add some entries to the registry, if needed.
    ਜੇ (install_popup
	    || install_openwith
	    || (need_uninstall_ਇੰਦਰਾਜ && interactive)
	    || !interactive)
	install_registry();

    // Register gvim with OLE.
    ਜੇ (has_gvim)
	install_OLE_ਰਜਿਸਟਰ();
}

/*
 * request_choice
 */
    ਸਥਿਰ ਖਾਲੀ
request_choice(ਖਾਲੀ)
{
    ਪੂਰਨ		      i;

    ਲਿਖੋ("\n\nInstall will do for you:\n");
    ਲਈ (i = 0; i < choice_ਗਿਣਤੀ; ++i)
	ਜੇ (choices[i].ਸਰਗਰਮ)
	    ਲਿਖੋ("%2d  %s\n", i + 1, choices[i].ਪਾਠ);
    ਲਿਖੋ("To change an item, enter its number\n\n");
    ਲਿਖੋ("Enter item number, h (help), d (do it) or q (quit): ");
}

    ਪੂਰਨ
ਮੁੱਖ(ਪੂਰਨ argc, ਅੱਖਰ **argv)
{
    ਪੂਰਨ		i;
    ਅੱਖਰ	buf[BUFSIZE];

    /*
     * Run interactively if there are no command line arguments.
     */
    ਜੇ (argc > 1)
	interactive = 0;
    ਨਹੀਂਤਾਂ
	interactive = 1;

    // Initialize this program.
    do_inits(argv);
    init_homedir();

    ਜੇ (argc > 1 && strcmp(argv[1], "-uninstall-check") == 0)
    {
	// Only check for already installed Vims.  Used by NSIS installer.
	i = uninstall_check(1);

	// Find the value of $VIM, because NSIS isn't able to do this by
	// itself.
	get_vim_env();

	// When nothing found exit quietly.  If something found wait for
	// a little while, so that the user can read the messages.
	ਜੇ (i && _isatty(1))
	    sleep(3);
	ਬਾਹਰ_ਨਿਕਲੋ(0);
    }

    ਲਿਖੋ("This program sets up the installation of Vim "
						   VIM_ਸੰਸਕਰਣ_MEDIUM "\n\n");

    // Check if the user unpacked the archives properly.
    check_unpack();

    // Check for already installed Vims.
    ਜੇ (interactive)
	uninstall_check(0);

    // Find out information about the system.
    inspect_system();

    ਜੇ (interactive)
    {
	// Setup all the choices.
	ਤਿਆਰੀ_choices();

	// Let the user change choices and finally install (or quit).
	ਲਈ (;;)
	{
	    request_choice();
	    rewind(stdin);
	    ਜੇ (scanf("%99s", buf) == 1)
	    {
		ਜੇ (ਅੰਕ_ਹੈ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)buf[0]))
		{
		    // Change a choice.
		    i = atoi(buf);
		    ਜੇ (i > 0 && i <= choice_ਗਿਣਤੀ && choices[i - 1].ਸਰਗਰਮ)
			(choices[i - 1].changefunc)(i - 1);
		    ਨਹੀਂਤਾਂ
			ਲਿਖੋ("\nIllegal choice\n");
		}
		ਨਹੀਂਤਾਂ ਜੇ (buf[0] == 'h' || buf[0] == 'H')
		{
		    // Help
		    show_help();
		}
		ਨਹੀਂਤਾਂ ਜੇ (buf[0] == 'd' || buf[0] == 'D')
		{
		    // Install!
		    install();
		    ਲਿਖੋ("\nThat finishes the installation.  Happy Vimming!\n");
		    ਰੋਕੋ;
		}
		ਨਹੀਂਤਾਂ ਜੇ (buf[0] == 'q' || buf[0] == 'Q')
		{
		    // Quit
		    ਲਿਖੋ("\nExiting without anything done\n");
		    ਰੋਕੋ;
		}
		ਨਹੀਂਤਾਂ
		    ਲਿਖੋ("\nIllegal choice\n");
	    }
	}
	ਲਿਖੋ("\n");
	myexit(0);
    }
    ਨਹੀਂਤਾਂ
    {
	/*
	 * Run non-interactive - setup according to the command line switches
	 */
	command_ਸਤਰ_ਤਿਆਰੀ_choices(argc, argv);
	install();

	// Avoid that the user has to hit Enter, just wait a little bit to
	// allow reading the messages.
	sleep(2);
    }

    ਵਾਪਸ 0;
}
