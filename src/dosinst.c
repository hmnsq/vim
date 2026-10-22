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
#ವ್ಯಾಖ್ಯಾನಿಸು DOSINST
#ಅನ್ತರ್ಭಾವ "dosinst.h"
#ಅನ್ತರ್ಭಾವ <io.h>

#ವ್ಯಾಖ್ಯಾನಿಸು GVIMEXT64_ಮಾರ್ಗ	    "GvimExt64\\gvimext.dll"
#ವ್ಯಾಖ್ಯಾನಿಸು GVIMEXT32_ಮಾರ್ಗ	    "GvimExt32\\gvimext.dll"

// Macro to do an error check I was typing over and over
#ವ್ಯಾಖ್ಯಾನಿಸು CHECK_REG_ERROR(ಸಂಕೇತ_2) \
    ಮಾಡು { \
	ಆದರೆ (ಸಂಕೇತ_2 != ERROR_SUCCESS) \
	{ \
	    ಬರೆ("%ld error number:  %ld\n", (ದೀರ್ಘ)__LINE__, (ದೀರ್ಘ)ಸಂಕೇತ_2); \
	    ಹಿಂತಿರುಗಿ 1; \
	} \
    } ಆಗ (0)

ಪೂರ್ಣಾಂಕ	has_vim = 0;		// installable vim.exe exists
ಪೂರ್ಣಾಂಕ	has_gvim = 0;		// installable gvim.exe exists

ಅಕ್ಷರ	oldvimrc[BUFSIZE];	// name of existing vimrc file
ಅಕ್ಷರ	vimrc[BUFSIZE];		// name of vimrc file to create

ಅಕ್ಷರ	*default_bat_dir = ಖಾಲಿ;  // when not NULL, use this as the default
				  // directory to write .bat files in
ಅಕ್ಷರ	*default_vim_dir = ಖಾಲಿ;  // when not NULL, use this as the default
				  // install dir for NSIS

/*
 * Structure used for each choice the user can make.
 */
ರಚನೆ choice
{
    ಪೂರ್ಣಾಂಕ	    ಸಕ್ರಿಯ;			// non-zero when choice is active
    ಅಕ್ಷರ    *ಪಠ್ಯ;			// text displayed for this choice
    ಶೂನ್ಯ    (*changefunc)(ಪೂರ್ಣಾಂಕ idx);	// function to change this choice
    ಪೂರ್ಣಾಂಕ	    arg;			// argument for function
    ಶೂನ್ಯ    (*installfunc)(ಪೂರ್ಣಾಂಕ idx);	// function to install this choice
};

ರಚನೆ choice	choices[30];		// choices the user can make
ಪೂರ್ಣಾಂಕ		choice_ಎಣಿಕೆ = 0;	// number of choices available

#ವ್ಯಾಖ್ಯಾನಿಸು ಕೋಷ್ಟಕ_ಗಾತ್ರ(s)	(ಪೂರ್ಣಾಂಕ)ARRAYSIZE(s)

ಎಣಿಕೆ
{
    compat_vi = 1,
    compat_vim,
    compat_some_enhancements,
    compat_all_enhancements
};
ಅಕ್ಷರ	*(compat_choices[]) =
{
    "\nChoose the default way to run Vim:",
    "Vi compatible",
    "Vim default",
    "with some Vim enhancements",
    "with syntax highlighting and other features switched on",
};
ಪೂರ್ಣಾಂಕ	compat_choice = (ಪೂರ್ಣಾಂಕ)compat_all_enhancements;
ಅಕ್ಷರ	*compat_ಪಠ್ಯ = "- run Vim %s";

ಎಣಿಕೆ
{
    remap_no = 1,
    remap_win
};
ಅಕ್ಷರ	*(remap_choices[]) =
{
    "\nChoose:",
    "Do not remap keys for Windows behavior",
    "Remap a few keys for Windows behavior (CTRL-V, CTRL-C, CTRL-F, etc)",
};
ಪೂರ್ಣಾಂಕ	remap_choice = (ಪೂರ್ಣಾಂಕ)remap_no;
ಅಕ್ಷರ	*remap_ಪಠ್ಯ = "- %s";

ಎಣಿಕೆ
{
    mouse_xterm = 1,
    mouse_mswin,
    mouse_default
};
ಅಕ್ಷರ	*(mouse_choices[]) =
{
    "\nChoose the way how Vim uses the mouse:",
    "right button extends selection (the Unix way)",
    "right button has a popup menu, left button starts select mode (the Windows way)",
    "right button has a popup menu, left button starts visual mode",
};
ಪೂರ್ಣಾಂಕ	mouse_choice = (ಪೂರ್ಣಾಂಕ)mouse_default;
ಅಕ್ಷರ	*mouse_ಪಠ್ಯ = "- The mouse %s";

ಎಣಿಕೆ
{
    vimfiles_dir_none = 1,
    vimfiles_dir_vim,
    vimfiles_dir_home
};
ಸ್ಥಿರ ಅಕ್ಷರ *(vimfiles_dir_choices[]) =
{
    "\nCreate plugin directories:",
    "No",
    "In the VIM directory",
    "In your HOME directory",
};

// non-zero when selected to install the popup menu entry.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	install_popup = 0;

// non-zero when selected to install the "Open with" entry.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	install_openwith = 0;

// non-zero when need to add an uninstall entry in the registry
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	need_uninstall_ನಮೂದು = 0;

/*
 * Definitions of the directory name (under $VIM) of the vimfiles directory
 * and its subdirectories:
 */
ಸ್ಥಿರ ಅಕ್ಷರ	*(vimfiles_subdirs[]) =
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_choice(ಅಕ್ಷರ **ಕೋಷ್ಟಕ, ಪೂರ್ಣಾಂಕ entries)
{
    ಪೂರ್ಣಾಂಕ		answer;
    ಪೂರ್ಣಾಂಕ		idx;
    ಅಕ್ಷರ	dummy[100];

    ಮಾಡು
    {
	ಗಾಗಿ (idx = 0; idx < entries; ++idx)
	{
	    ಆದರೆ (idx)
		ಬರೆ("%2d  ", idx);
	    puts(ಕೋಷ್ಟಕ[idx]);
	}
	ಬರೆ("Choice: ");
	ಆದರೆ (scanf("%d", &answer) != 1)
	{
	    scanf("%99s", dummy);
	    answer = 0;
	}
    }
    ಆಗ (answer < 1 || answer >= entries);

    ಹಿಂತಿರುಗಿ answer;
}

/*
 * Check if the user unpacked the archives properly.
 * Sets "runtimeidx".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
check_unpack(ಶೂನ್ಯ)
{
    ಅಕ್ಷರ	buf[BUFSIZE];
    FILE	*fd;
    ರಚನೆ stat	st;

    // check for presence of the correct version number in installdir[]
    runtimeidx = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(installdir) - ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(VIM_ಆವೃತ್ತಿ_NODOT);
    ಆದರೆ (runtimeidx <= 0
	    || stricmp(installdir + runtimeidx, VIM_ಆವೃತ್ತಿ_NODOT) != 0
	    || (installdir[runtimeidx - 1] != '/'
		&& installdir[runtimeidx - 1] != '\\'))
    {
	ಬರೆ("ERROR: Install program not in directory \"%s\"\n",
		VIM_ಆವೃತ್ತಿ_NODOT);
	ಬರೆ("This program can only work when it is located in its original directory\n");
	myexit(1);
    }

    // check if filetype.vim is present, which means the runtime archive has
    // been unpacked
    sprintf(buf, "%s\\filetype.vim", installdir);
    ಆದರೆ (stat(buf, &st) < 0)
    {
	ಬರೆ("ERROR: Cannot find filetype.vim in \"%s\"\n", installdir);
	ಬರೆ("It looks like you did not unpack the runtime archive.\n");
	ಬರೆ("You must unpack the runtime archive \"%srt.zip\" before installing.\n",
		VIM_ಆವೃತ್ತಿ_NODOT);
	myexit(1);
    }

    // Check if vim.exe or gvim.exe is in the current directory.
    ಆದರೆ ((fd = fopen("gvim.exe", "r")) != ಖಾಲಿ)
    {
	fclose(fd);
	has_gvim = 1;
    }
    ಆದರೆ ((fd = fopen("vim.exe", "r")) != ಖಾಲಿ)
    {
	fclose(fd);
	has_vim = 1;
    }
    ಆದರೆ (!has_gvim && !has_vim)
    {
	ಬರೆ("ERROR: Cannot find any Vim executables in \"%s\"\n\n",
								  installdir);
	myexit(1);
    }
}

/*
 * Compare paths "p[plen]" to "q[qlen]".  Return 0 if they match.
 * Ignores case and differences between '/' and '\'.
 * "plen" and "qlen" can be negative, strlen() is used then.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
pathcmp(ಅಕ್ಷರ *p, ಪೂರ್ಣಾಂಕ plen, ಅಕ್ಷರ *q, ಪೂರ್ಣಾಂಕ qlen)
{
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (plen < 0)
	plen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(p);
    ಆದರೆ (qlen < 0)
	qlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(q);
    ಗಾಗಿ (i = 0; ; ++i)
    {
	// End of "p": check if "q" also ends or just has a slash.
	ಆದರೆ (i == plen)
	{
	    ಆದರೆ (i == qlen)  //ಹೊಂದಿಕೆ
		ಹಿಂತಿರುಗಿ 0;
	    ಆದರೆ (i == qlen - 1 && (q[i] == '\\' || q[i] == '/'))
		ಹಿಂತಿರುಗಿ 0;   // match with trailing slash
	    ಹಿಂತಿರುಗಿ 1;	    // no match
	}

	// End of "q": check if "p" also ends or just has a slash.
	ಆದರೆ (i == qlen)
	{
	    ಆದರೆ (i == plen)  //ಹೊಂದಿಕೆ
		ಹಿಂತಿರುಗಿ 0;
	    ಆದರೆ (i == plen - 1 && (p[i] == '\\' || p[i] == '/'))
		ಹಿಂತಿರುಗಿ 0;   // match with trailing slash
	    ಹಿಂತಿರುಗಿ 1;	    // no match
	}

	ಆದರೆ (!(mytoupper(p[i]) == mytoupper(q[i])
		|| ((p[i] == '/' || p[i] == '\\')
		    && (q[i] == '/' || q[i] == '\\'))))
	    ಹಿಂತಿರುಗಿ 1;	    // no match
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
    ಸ್ಥಿರ ಶೂನ್ಯ
findoldfile(ಅಕ್ಷರ **destination)
{
    ಅಕ್ಷರ	*bp = *destination;
    size_t	indir_l = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(installdir);
    ಅಕ್ಷರ	*cp;
    ಅಕ್ಷರ	*tmpname;
    ಅಕ್ಷರ	*farname;

    /*
     * No action needed if exe not found or not in this directory.
     */
    ಆದರೆ (bp == ಖಾಲಿ || strnicmp(bp, installdir, indir_l) != 0)
	ಹಿಂತಿರುಗಿ;
    cp = bp + indir_l;
    ಆದರೆ (strchr("/\\", *cp++) == ಖಾಲಿ
	    || strchr(cp, '\\') != ಖಾಲಿ
	    || strchr(cp, '/') != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    tmpname = alloc(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(cp) + 1);
    strcpy(tmpname, cp);
    tmpname[ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(tmpname) - 1] = 'x';	// .exe -> .exx

    ಆದರೆ (access(tmpname, 0) == 0)
    {
	ಬರೆ("\nERROR: %s and %s clash.  Remove or rename %s.\n",
	    tmpname, cp, tmpname);
	myexit(1);
    }

    ಆದರೆ (rename(cp, tmpname) != 0)
    {
	ಬರೆ("\nERROR: failed to rename %s to %s: %s\n",
	    cp, tmpname, strerror(0));
	myexit(1);
    }

    farname = searchpath_save(cp);

    ಆದರೆ (rename(tmpname, cp) != 0)
    {
	ಬರೆ("\nERROR: failed to rename %s back to %s: %s\n",
	    tmpname, cp, strerror(0));
	myexit(1);
    }

    ಬಿಡುಗಡೆ(*destination);
    ಬಿಡುಗಡೆ(tmpname);
    *destination = farname;
}

/*
 * Check if there is a vim.[exe|bat|, gvim.[exe|bat|, etc. in the path.
 * When "check_bat_only" is TRUE, only find "default_bat_dir".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕಂಡುಹಿಡಿ_bat_exe(ಪೂರ್ಣಾಂಕ check_bat_only)
{
    ಪೂರ್ಣಾಂಕ		i;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    ಗಾಗಿ (i = 1; i < ಗುರಿ_ಎಣಿಕೆ; ++i)
    {
	targets[i].oldbat = searchpath_save(targets[i].batname);
	ಆದರೆ (!check_bat_only)
	    targets[i].oldexe = searchpath_save(targets[i].exename);

	ಆದರೆ (default_bat_dir == ಖಾಲಿ && targets[i].oldbat != ಖಾಲಿ)
	{
	    default_bat_dir = alloc(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(targets[i].oldbat) + 1);
	    strcpy(default_bat_dir, targets[i].oldbat);
	    remove_ಬಾಲ(default_bat_dir);
	}
	ಆದರೆ (check_bat_only && targets[i].oldbat != ಖಾಲಿ)
	{
	    ಬಿಡುಗಡೆ(targets[i].oldbat);
	    targets[i].oldbat = ಖಾಲಿ;
	}
    }

    mch_chdir(installdir);
}

/*
 * Get the value of $VIMRUNTIME or $VIM and write it in $TEMP/vimini.ini, so
 * that NSIS can read it.
 * When not set, use the directory of a previously installed Vim.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_vim_env(ಶೂನ್ಯ)
{
    ಅಕ್ಷರ	*vim;
    ಅಕ್ಷರ	buf[BUFSIZE];
    FILE	*fd;
    ಅಕ್ಷರ	fname[BUFSIZE];

    // First get $VIMRUNTIME.  If it's set, remove the tail.
    vim = getenv("VIMRUNTIME");
    ಆದರೆ (vim != ಖಾಲಿ && *vim != 0 && ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(vim) < ಗಾತ್ರ(buf))
    {
	strcpy(buf, vim);
	remove_ಬಾಲ(buf);
	vim = buf;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	vim = getenv("VIM");
	ಆದರೆ (vim == ಖಾಲಿ || *vim == 0)
	{
	    // Use the directory from an old uninstall entry.
	    ಆದರೆ (default_vim_dir != ಖಾಲಿ)
		vim = default_vim_dir;
	    ಇಲ್ಲದಿದ್ದರೆ
		// Let NSIS know there is no default, it should use
		// $PROGRAMFILES.
		vim = "";
	}
    }

    // NSIS also uses GetTempPath(), thus we should get the same directory
    // name as where NSIS will look for vimini.ini.
    Get_ತಾತ್ಕಾಲಿಕ_ಮಾರ್ಗ(ಗಾತ್ರ(fname) - 12, fname);
    ಕೂಡಿಸು_pathsep(fname);
    strcat(fname, "vimini.ini");

    fd = fopen(fname, "w");
    ಆದರೆ (fd != ಖಾಲಿ)
    {
	// Make it look like an .ini file, so that NSIS can read it with a
	// ReadINIStr command.
	fprintf(fd, "[vimini]\n");
	fprintf(fd, "dir=\"%s\"\n", vim);
	fclose(fd);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಬರೆ("Failed to open %s\n", fname);
	sleep(2);
    }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಅಂಕೆ_windows;

/*
 * Callback used for EnumWindows():
 * Count the window if the title looks like it is for the uninstaller.
 */
//ARGSUSED
    ಸ್ಥಿರ BOOL CALLBACK
ಕಿಟಕಿ_cb(HWND hwnd, LPARAM lparam ಬಳಸದ)
{
    ಅಕ್ಷರ title[256];

    title[0] = 0;
    Get_ಕಿಟಕಿ_ಪಠ್ಯ(hwnd, title, 256);
    ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(title, "Vim ") != ಖಾಲಿ && ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(title, " Uninstall") != ಖಾಲಿ)
	++ಅಂಕೆ_windows;
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Run the uninstaller silently.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಸತತ_silent_uninstall(ಅಕ್ಷರ *uninst_exe)
{
    ಅಕ್ಷರ    vimrt_dir[BUFSIZE];
    ಅಕ್ಷರ    ತಾತ್ಕಾಲಿಕ_uninst[BUFSIZE];
    ಅಕ್ಷರ    ತಾತ್ಕಾಲಿಕ_dir[MAX_ಮಾರ್ಗ];
    ಅಕ್ಷರ    buf[BUFSIZE * 2 + 10];
    ಪೂರ್ಣಾಂಕ	    i;
    DWORD   tick;

    strcpy(vimrt_dir, uninst_exe);
    remove_ಬಾಲ(vimrt_dir);

    ಆದರೆ (!Get_ತಾತ್ಕಾಲಿಕ_ಮಾರ್ಗ(ಗಾತ್ರ(ತಾತ್ಕಾಲಿಕ_dir), ತಾತ್ಕಾಲಿಕ_dir))
	ಹಿಂತಿರುಗಿ FAIL;

    // Copy the uninstaller to a temporary exe.
    tick = Get_Tick_ಎಣಿಕೆ();
    ಗಾಗಿ (i = 0; ; i++)
    {
	sprintf(ತಾತ್ಕಾಲಿಕ_uninst, "%s\\vimun%04X.exe", ತಾತ್ಕಾಲಿಕ_dir,
					  (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)((i + tick) & 0xFFFF));
	ಆದರೆ (CopyFile(uninst_exe, ತಾತ್ಕಾಲಿಕ_uninst, TRUE))
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (Get_ಕೊನೆಯ_Error() != ERROR_FILE_EXISTS)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆದರೆ (i == 65535)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    // Run the copied uninstaller silently.
    ಆದರೆ (strchr(ತಾತ್ಕಾಲಿಕ_uninst, ' ') != ಖಾಲಿ)
	sprintf(buf, "\"%s\" /S _?=%s", ತಾತ್ಕಾಲಿಕ_uninst, vimrt_dir);
    ಇಲ್ಲದಿದ್ದರೆ
	sprintf(buf, "%s /S _?=%s", ತಾತ್ಕಾಲಿಕ_uninst, vimrt_dir);
    ಸತತ_command(buf);

    DeleteFile(ತಾತ್ಕಾಲಿಕ_uninst);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Check for already installed Vims.
 * Return non-zero when found one.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
uninstall_check(ಪೂರ್ಣಾಂಕ skip_question)
{
    HKEY	ಕೀಲಿ_handle;
    HKEY	uninstall_ಕೀಲಿ_handle;
    ಅಕ್ಷರ	*uninstall_ಕೀಲಿ = "software\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
    ಅಕ್ಷರ	subkey_name_buff[BUFSIZE];
    ಅಕ್ಷರ	ತಾತ್ಕಾಲಿಕ_string_ಬಫರ್[BUFSIZE-2];
    DWORD	local_bufsize;
    FILETIME	ತಾತ್ಕಾಲಿಕ_pfiletime;
    DWORD	ಕೀಲಿ_ಸೂಚ್ಯಂಕ;
    ಅಕ್ಷರ	ಒಳಹರಿವು;
    ದೀರ್ಘ	ಸಂಕೇತ_2;
    DWORD	ಮೌಲ್ಯ_type;
    DWORD	orig_ಅಂಕೆ_ಕೀಲಿಗಳು;
    DWORD	new_ಅಂಕೆ_ಕೀಲಿಗಳು;
    DWORD	allow_silent;
    ಪೂರ್ಣಾಂಕ		foundone = 0;

    ಸಂಕೇತ_2 = Reg_Open_ಕೀಲಿ_Ex(HKEY_LOCAL_MACHINE, uninstall_ಕೀಲಿ, 0,
				     ಕೀಲಿ_WOW64_64_ಕೀಲಿ | ಕೀಲಿ_READ, &ಕೀಲಿ_handle);
    CHECK_REG_ERROR(ಸಂಕೇತ_2);

    ಕೀಲಿ_ಸೂಚ್ಯಂಕ = 0;
    ಆಗ (TRUE)
    {
	local_bufsize = ಗಾತ್ರ(subkey_name_buff);
	ಆದರೆ (Reg_Enum_ಕೀಲಿ_Ex(ಕೀಲಿ_handle, ಕೀಲಿ_ಸೂಚ್ಯಂಕ, subkey_name_buff, &local_bufsize,
		ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, &ತಾತ್ಕಾಲಿಕ_pfiletime) == ERROR_NO_ಇನ್ನಷ್ಟು_ವಸ್ತುಗಳು)
	    ನಿಲ್ಲಿಸು;

	ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ("Vim", subkey_name_buff, 3) == 0)
	{
	    // Open the key named Vim*
	    ಸಂಕೇತ_2 = Reg_Open_ಕೀಲಿ_Ex(ಕೀಲಿ_handle, subkey_name_buff, 0,
			   ಕೀಲಿ_WOW64_64_ಕೀಲಿ | ಕೀಲಿ_READ, &uninstall_ಕೀಲಿ_handle);
	    CHECK_REG_ERROR(ಸಂಕೇತ_2);

	    // get the DisplayName out of it to show the user
	    local_bufsize = ಗಾತ್ರ(ತಾತ್ಕಾಲಿಕ_string_ಬಫರ್);
	    ಸಂಕೇತ_2 = Reg_Query_ಮೌಲ್ಯ_Ex(uninstall_ಕೀಲಿ_handle, "displayname", 0,
		    &ಮೌಲ್ಯ_type, (LPBYTE)ತಾತ್ಕಾಲಿಕ_string_ಬಫರ್,
		    &local_bufsize);
	    CHECK_REG_ERROR(ಸಂಕೇತ_2);

	    allow_silent = 0;
	    ಆದರೆ (skip_question)
	    {
		DWORD varsize = ಗಾತ್ರ(DWORD);

		Reg_Query_ಮೌಲ್ಯ_Ex(uninstall_ಕೀಲಿ_handle, "AllowSilent", 0,
			&ಮೌಲ್ಯ_type, (LPBYTE)&allow_silent,
			&varsize);
	    }

	    foundone = 1;
	    ಬರೆ("\n*********************************************************\n");
	    ಬರೆ("Vim Install found what looks like an existing Vim version.\n");
	    ಬರೆ("The name of the entry is:\n");
	    ಬರೆ("\n        \"%s\"\n\n", ತಾತ್ಕಾಲಿಕ_string_ಬಫರ್);

	    ಬರೆ("Installing the new version will disable part of the existing version.\n");
	    ಬರೆ("(The batch files used in a console and the \"Edit with Vim\" entry in\n");
	    ಬರೆ("the popup menu will use the new version)\n");

	    ಆದರೆ (skip_question)
		ಬರೆ("\nRunning uninstall program for \"%s\"\n", ತಾತ್ಕಾಲಿಕ_string_ಬಫರ್);
	    ಇಲ್ಲದಿದ್ದರೆ
		ಬರೆ("\nDo you want to uninstall \"%s\" now?\n(y)es/(n)o)  ", ತಾತ್ಕಾಲಿಕ_string_ಬಫರ್);
	    fflush(stdout);

	    // get the UninstallString
	    local_bufsize = ಗಾತ್ರ(ತಾತ್ಕಾಲಿಕ_string_ಬಫರ್);
	    ಸಂಕೇತ_2 = Reg_Query_ಮೌಲ್ಯ_Ex(uninstall_ಕೀಲಿ_handle, "uninstallstring", 0,
		    &ಮೌಲ್ಯ_type, (LPBYTE)ತಾತ್ಕಾಲಿಕ_string_ಬಫರ್, &local_bufsize);
	    CHECK_REG_ERROR(ಸಂಕೇತ_2);

	    // Remember the directory, it is used as the default for NSIS.
	    default_vim_dir = alloc(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ತಾತ್ಕಾಲಿಕ_string_ಬಫರ್) + 1);
	    strcpy(default_vim_dir, ತಾತ್ಕಾಲಿಕ_string_ಬಫರ್);
	    remove_ಬಾಲ(default_vim_dir);
	    remove_ಬಾಲ(default_vim_dir);

	    ಒಳಹರಿವು = 'n';
	    ಮಾಡು
	    {
		ಆದರೆ (ಒಳಹರಿವು != 'n')
		    ಬರೆ("%c is an invalid reply.  Please enter either 'y' or 'n'\n", ಒಳಹರಿವು);

		ಆದರೆ (skip_question)
		    ಒಳಹರಿವು = 'y';
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    rewind(stdin);
		    scanf("%c", &ಒಳಹರಿವು);
		}
		ಆಯ್ಕೆ (ಒಳಹರಿವು)
		{
		    ಸ್ಥಿತಿ 'y':
		    ಸ್ಥಿತಿ 'Y':
			// save the number of uninstall keys so we can know if
			// it changed
			Reg_Query_Info_ಕೀಲಿ(ಕೀಲಿ_handle, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ,
					     &orig_ಅಂಕೆ_ಕೀಲಿಗಳು, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ,
						      ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);

			// Find existing .bat files before deleting them.
			ಕಂಡುಹಿಡಿ_bat_exe(TRUE);

			ಆದರೆ (allow_silent)
			{
			    ಆದರೆ (ಸತತ_silent_uninstall(ತಾತ್ಕಾಲಿಕ_string_ಬಫರ್)
								    == FAIL)
				allow_silent = 0; // Retry with non silent.
			}
			ಆದರೆ (!allow_silent)
			{
			    // Execute the uninstall program.  Put it in double
			    // quotes if there is an embedded space.
			    {
				ಅಕ್ಷರ buf[BUFSIZE];

				ಆದರೆ (strchr(ತಾತ್ಕಾಲಿಕ_string_ಬಫರ್, ' ') != ಖಾಲಿ)
				    sprintf(buf, "\"%s\"", ತಾತ್ಕಾಲಿಕ_string_ಬಫರ್);
				ಇಲ್ಲದಿದ್ದರೆ
				    strcpy(buf, ತಾತ್ಕಾಲಿಕ_string_ಬಫರ್);
				ಸತತ_command(buf);
			    }

			    // Count the number of windows with a title that
			    // match the installer, so that we can check when
			    // it's done.  The uninstaller copies itself,
			    // executes the copy and exits, thus we can't wait
			    // for the process to finish.
			    sleep(1);  // wait for uninstaller to start up
			    ಅಂಕೆ_windows = 0;
			    EnumWindows(ಕಿಟಕಿ_cb, 0);
			    ಆದರೆ (ಅಂಕೆ_windows == 0)
			    {
				// Did not find the uninstaller, ask user to
				// press Enter when done. Just in case.
				ಬರೆ("Press Enter when the uninstaller is finished\n");
				rewind(stdin);
				(ಶೂನ್ಯ)getchar();
			    }
			    ಇಲ್ಲದಿದ್ದರೆ
			    {
				ಬರೆ("Waiting for the uninstaller to finish (press CTRL-C to abort).");
				ಮಾಡು
				{
				    ಬರೆ(".");
				    fflush(stdout);
				    sleep(1);	// wait for the uninstaller to
						//ಮುಕ್ತಾಯ
				    ಅಂಕೆ_windows = 0;
				    EnumWindows(ಕಿಟಕಿ_cb, 0);
				} ಆಗ (ಅಂಕೆ_windows > 0);
			    }
			}
			ಬರೆ("\nDone!\n");

			// Check if an uninstall reg key was deleted.
			// if it was, we want to decrement key_index.
			// if we don't do this, we will skip the key
			// immediately after any key that we delete.
			Reg_Query_Info_ಕೀಲಿ(ಕೀಲಿ_handle, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ,
					      &new_ಅಂಕೆ_ಕೀಲಿಗಳು, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ,
						      ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
			ಆದರೆ (new_ಅಂಕೆ_ಕೀಲಿಗಳು < orig_ಅಂಕೆ_ಕೀಲಿಗಳು)
			    ಕೀಲಿ_ಸೂಚ್ಯಂಕ--;

			ಒಳಹರಿವು = 'y';
			ನಿಲ್ಲಿಸು;

		    ಸ್ಥಿತಿ 'n':
		    ಸ್ಥಿತಿ 'N':
			// Do not uninstall
			ಒಳಹರಿವು = 'n';
			ನಿಲ್ಲಿಸು;

		    ಪೂರ್ವನಿಯೋಜಿತ: // just drop through and redo the loop
			ನಿಲ್ಲಿಸು;
		}

	    } ಆಗ (ಒಳಹರಿವು != 'n' && ಒಳಹರಿವು != 'y');

	    Reg_Close_ಕೀಲಿ(uninstall_ಕೀಲಿ_handle);
	}

	ಕೀಲಿ_ಸೂಚ್ಯಂಕ++;
    }
    Reg_Close_ಕೀಲಿ(ಕೀಲಿ_handle);

    ಹಿಂತಿರುಗಿ foundone;
}

/*
 * Find out information about the system.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
inspect_system(ಶೂನ್ಯ)
{
    ಅಕ್ಷರ	*p;
    ಅಕ್ಷರ	buf[BUFSIZE];
    FILE	*fd;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		foundone;

    // This may take a little while, let the user know what we're doing.
    ಬರೆ("Inspecting system...\n");

    /*
     * If $VIM is set, check that it's pointing to our directory.
     */
    p = getenv("VIM");
    ಆದರೆ (p != ಖಾಲಿ && pathcmp(p, -1, installdir, runtimeidx - 1) != 0)
    {
	ಬರೆ("------------------------------------------------------\n");
	ಬರೆ("$VIM is set to \"%s\".\n", p);
	ಬರೆ("This is different from where this version of Vim is:\n");
	strcpy(buf, installdir);
	*(buf + runtimeidx - 1) = NUL;
	ಬರೆ("\"%s\"\n", buf);
	ಬರೆ("You must adjust or remove the setting of $VIM,\n");
	ಆದರೆ (interactive)
	{
	    ಬರೆ("to be able to use this install program.\n");
	    myexit(1);
	}
	ಬರೆ("otherwise Vim WILL NOT WORK properly!\n");
	ಬರೆ("------------------------------------------------------\n");
    }

    /*
     * If $VIMRUNTIME is set, check that it's pointing to our runtime directory.
     */
    p = getenv("VIMRUNTIME");
    ಆದರೆ (p != ಖಾಲಿ && pathcmp(p, -1, installdir, -1) != 0)
    {
	ಬರೆ("------------------------------------------------------\n");
	ಬರೆ("$VIMRUNTIME is set to \"%s\".\n", p);
	ಬರೆ("This is different from where this version of Vim is:\n");
	ಬರೆ("\"%s\"\n", installdir);
	ಬರೆ("You must adjust or remove the setting of $VIMRUNTIME,\n");
	ಆದರೆ (interactive)
	{
	    ಬರೆ("to be able to use this install program.\n");
	    myexit(1);
	}
	ಬರೆ("otherwise Vim WILL NOT WORK properly!\n");
	ಬರೆ("------------------------------------------------------\n");
    }

    /*
     * Check if there is a vim.[exe|bat|, gvim.[exe|bat|, etc. in the path.
     */
    ಕಂಡುಹಿಡಿ_bat_exe(FALSE);

    /*
     * A .exe in the install directory may be found anyway on Windows 2000.
     * Check for this situation and find another executable if necessary.
     * w.briscoe@ponl.com 2001-01-20
     */
    foundone = 0;
    ಗಾಗಿ (i = 1; i < ಗುರಿ_ಎಣಿಕೆ; ++i)
    {
	findoldfile(&(targets[i].oldexe));
	ಆದರೆ (targets[i].oldexe != ಖಾಲಿ)
	    foundone = 1;
    }

    ಆದರೆ (foundone)
    {
	ಬರೆ("Warning: Found Vim executable(s) in your $PATH:\n");
	ಗಾಗಿ (i = 1; i < ಗುರಿ_ಎಣಿಕೆ; ++i)
	    ಆದರೆ (targets[i].oldexe != ಖಾಲಿ)
		ಬರೆ("%s\n", targets[i].oldexe);
	ಬರೆ("It will be used instead of the version you are installing.\n");
	ಬರೆ("Please delete or rename it, or adjust your $PATH setting.\n");
    }

    /*
     * Check if there is an existing ../_vimrc or ../.vimrc file.
     */
    strcpy(oldvimrc, installdir);
    strcpy(oldvimrc + runtimeidx, "_vimrc");
    ಆದರೆ ((fd = fopen(oldvimrc, "r")) == ಖಾಲಿ)
    {
	strcpy(oldvimrc + runtimeidx, "vimrc~1"); // short version of .vimrc
	ಆದರೆ ((fd = fopen(oldvimrc, "r")) == ಖಾಲಿ)
	{
	    strcpy(oldvimrc + runtimeidx, ".vimrc");
	    fd = fopen(oldvimrc, "r");
	}
    }
    ಆದರೆ (fd != ಖಾಲಿ)
	fclose(fd);
    ಇಲ್ಲದಿದ್ದರೆ
	*oldvimrc = NUL;
}

/*
 * Add a dummy choice to avoid that the numbering changes depending on items
 * in the environment.  The user may type a number he remembered without
 * looking.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_dummy_choice(ಶೂನ್ಯ)
{
    choices[choice_ಎಣಿಕೆ].installfunc = ಖಾಲಿ;
    choices[choice_ಎಣಿಕೆ].ಸಕ್ರಿಯ = 0;
    choices[choice_ಎಣಿಕೆ].changefunc = ಖಾಲಿ;
    choices[choice_ಎಣಿಕೆ].ಪಠ್ಯ = ಖಾಲಿ;
    choices[choice_ಎಣಿಕೆ].arg = 0;
    ++choice_ಎಣಿಕೆ;
}

////////////////////////////////////////////////
// stuff for creating the batch files.

/*
 * Install the vim.bat, gvim.bat, etc. files.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
install_bat_choice(ಪೂರ್ಣಾಂಕ idx)
{
    ಅಕ್ಷರ	*batpath = targets[choices[idx].arg].batpath;
    ಅಕ್ಷರ	*oldname = targets[choices[idx].arg].oldbat;
    ಅಕ್ಷರ	*exename = targets[choices[idx].arg].exenamearg;
    ಅಕ್ಷರ	*vimarg = targets[choices[idx].arg].exearg;
    FILE	*fd;

    ಆದರೆ (*batpath == NUL)
	ಹಿಂತಿರುಗಿ;

    fd = fopen(batpath, "w");
    ಆದರೆ (fd == ಖಾಲಿ)
    {
	ಬರೆ("\nERROR: Cannot open \"%s\" for writing.\n", batpath);
	ಹಿಂತಿರುಗಿ;
    }

    need_uninstall_ನಮೂದು = 1;

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
	    VIM_ಆವೃತ್ತಿ_NODOT, exename, VIM_ಆವೃತ್ತಿ_NODOT);
    fprintf(fd, "if exist \"%%VIMRUNTIME%%\\%s\" set VIM_EXE_DIR=%%VIMRUNTIME%%\n", exename);
    fprintf(fd, "\n");

    // Give an error message when the executable could not be found.
    fprintf(fd, "if not exist \"%%VIM_EXE_DIR%%\\%s\" (\n", exename);
    fprintf(fd, "    echo \"%%VIM_EXE_DIR%%\\%s\" not found\n", exename);
    fprintf(fd, "    goto :eof\n");
    fprintf(fd, ")\n");
    fprintf(fd, "\n");

    ಆದರೆ (*exename == 'g')
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

    ಆದರೆ (*exename == 'g')
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
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Always use quotes, $VIM or $VIMRUNTIME might have a space.
	fprintf(fd, "\"%%VIM_EXE_DIR%%\\%s\" %s %%*\n",
		exename, vimarg);
    }

    fclose(fd);
    ಬರೆ("%s has been %s\n", batpath,
	    oldname == ಖಾಲಿ ? "created" : "overwritten");
}

/*
 * Make the text string for choice "idx".
 * The format "fmt" is must have one %s item, which "arg" is used for.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
alloc_ಪಠ್ಯ(ಪೂರ್ಣಾಂಕ idx, ಅಕ್ಷರ *fmt, ಅಕ್ಷರ *arg)
{
    ಆದರೆ (choices[idx].ಪಠ್ಯ != ಖಾಲಿ)
	ಬಿಡುಗಡೆ(choices[idx].ಪಠ್ಯ);

    choices[idx].ಪಠ್ಯ = alloc(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(fmt) + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(arg) - 1);
    sprintf(choices[idx].ಪಠ್ಯ, fmt, arg);
}

/*
 * Toggle the "Overwrite .../vim.bat" to "Don't overwrite".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
toggle_bat_choice(ಪೂರ್ಣಾಂಕ idx)
{
    ಅಕ್ಷರ	*batname = targets[choices[idx].arg].batpath;
    ಅಕ್ಷರ	*oldname = targets[choices[idx].arg].oldbat;

    ಆದರೆ (*batname == NUL)
    {
	alloc_ಪಠ್ಯ(idx, "    Overwrite %s", oldname);
	strcpy(batname, oldname);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	alloc_ಪಠ್ಯ(idx, "    Do NOT overwrite %s", oldname);
	*batname = NUL;
    }
}

/*
 * Do some work for a batch file entry: Append the batch file name to the path
 * and set the text for the choice.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_bat_ಪಠ್ಯ(ಪೂರ್ಣಾಂಕ idx, ಅಕ್ಷರ *batpath, ಅಕ್ಷರ *name)
{
    strcat(batpath, name);

    alloc_ಪಠ್ಯ(idx, "    Create %s", batpath);
}

/*
 * Select a directory to write the batch file line.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
change_bat_choice(ಪೂರ್ಣಾಂಕ idx)
{
    ಅಕ್ಷರ	*ಮಾರ್ಗ;
    ಅಕ್ಷರ	*batpath;
    ಅಕ್ಷರ	*name;
    ಪೂರ್ಣಾಂಕ		n;
    ಅಕ್ಷರ	*s;
    ಅಕ್ಷರ	*p;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2;
    ಅಕ್ಷರ	**ಹೆಸರುಗಳು = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		ಗುರಿ = choices[idx].arg;

    name = targets[ಗುರಿ].batname;
    batpath = targets[ಗುರಿ].batpath;

    ಮಾರ್ಗ = getenv("PATH");
    ಆದರೆ (ಮಾರ್ಗ == ಖಾಲಿ)
    {
	ಬರೆ("\nERROR: The variable $PATH is not set\n");
	ಹಿಂತಿರುಗಿ;
    }

    /*
     * first round: count number of names in path;
     * second round: save names to names[].
     */
    ಗಾಗಿ (;;)
    {
	ಎಣಿಕೆ_2 = 1;
	ಗಾಗಿ (p = ಮಾರ್ಗ; *p; )
	{
	    s = strchr(p, ';');
	    ಆದರೆ (s == ಖಾಲಿ)
		s = p + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(p);
	    ಆದರೆ (ಹೆಸರುಗಳು != ಖಾಲಿ)
	    {
		ಹೆಸರುಗಳು[ಎಣಿಕೆ_2] = alloc(s - p + 1);
		strncpy(ಹೆಸರುಗಳು[ಎಣಿಕೆ_2], p, s - p);
		ಹೆಸರುಗಳು[ಎಣಿಕೆ_2][s - p] = NUL;
	    }
	    ++ಎಣಿಕೆ_2;
	    p = s;
	    ಆದರೆ (*p != NUL)
		++p;
	}
	ಆದರೆ (ಹೆಸರುಗಳು != ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
	ಹೆಸರುಗಳು = alloc((ಎಣಿಕೆ_2 + 1) * ಗಾತ್ರ(ಅಕ್ಷರ *));
    }
    ಹೆಸರುಗಳು[0] = alloc(50);
    sprintf(ಹೆಸರುಗಳು[0], "Select directory to create %s in:", name);
    ಹೆಸರುಗಳು[ಎಣಿಕೆ_2] = alloc(50);
    ಆದರೆ (choices[idx].arg == 0)
	sprintf(ಹೆಸರುಗಳು[ಎಣಿಕೆ_2], "Do not create any .bat file.");
    ಇಲ್ಲದಿದ್ದರೆ
	sprintf(ಹೆಸರುಗಳು[ಎಣಿಕೆ_2], "Do not create a %s file.", name);
    n = get_choice(ಹೆಸರುಗಳು, ಎಣಿಕೆ_2 + 1);

    ಆದರೆ (n == ಎಣಿಕೆ_2)
    {
	// Selected last item, don't create bat file.
	*batpath = NUL;
	ಆದರೆ (choices[idx].arg != 0)
	    alloc_ಪಠ್ಯ(idx, "    Do NOT create %s", name);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Selected one of the paths.  For the first item only keep the path,
	// for the others append the batch file name.
	strcpy(batpath, ಹೆಸರುಗಳು[n]);
	ಕೂಡಿಸು_pathsep(batpath);
	ಆದರೆ (choices[idx].arg != 0)
	    ಗಣ_bat_ಪಠ್ಯ(idx, batpath, name);
    }

    ಗಾಗಿ (i = 0; i <= ಎಣಿಕೆ_2; ++i)
	ಬಿಡುಗಡೆ(ಹೆಸರುಗಳು[i]);
    ಬಿಡುಗಡೆ(ಹೆಸರುಗಳು);
}

ಅಕ್ಷರ *bat_ಪಠ್ಯ_yes = "Install .bat files to use Vim at the command line:";
ಅಕ್ಷರ *bat_ಪಠ್ಯ_no = "do NOT install .bat files to use Vim at the command line";

    ಸ್ಥಿರ ಶೂನ್ಯ
change_main_bat_choice(ಪೂರ್ಣಾಂಕ idx)
{
    ಪೂರ್ಣಾಂಕ		i;

    // let the user select a default directory or NONE
    change_bat_choice(idx);

    ಆದರೆ (targets[0].batpath[0] != NUL)
	choices[idx].ಪಠ್ಯ = bat_ಪಠ್ಯ_yes;
    ಇಲ್ಲದಿದ್ದರೆ
	choices[idx].ಪಠ್ಯ = bat_ಪಠ್ಯ_no;

    // update the individual batch file selections
    ಗಾಗಿ (i = 1; i < ಗುರಿ_ಎಣಿಕೆ; ++i)
    {
	// Only make it active when the first item has a path and the vim.exe
	// or gvim.exe exists (there is a changefunc then).
	ಆದರೆ (targets[0].batpath[0] != NUL
		&& choices[idx + i].changefunc != ಖಾಲಿ)
	{
	    choices[idx + i].ಸಕ್ರಿಯ = 1;
	    ಆದರೆ (choices[idx + i].changefunc == change_bat_choice
		    && targets[i].batpath[0] != NUL)
	    {
		strcpy(targets[i].batpath, targets[0].batpath);
		ಗಣ_bat_ಪಠ್ಯ(idx + i, targets[i].batpath, targets[i].batname);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    choices[idx + i].ಸಕ್ರಿಯ = 0;
    }
}

/*
 * Initialize a choice for creating a batch file.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
init_bat_choice(ಪೂರ್ಣಾಂಕ ಗುರಿ)
{
    ಅಕ್ಷರ	*batpath = targets[ಗುರಿ].batpath;
    ಅಕ್ಷರ	*oldbat = targets[ಗುರಿ].oldbat;
    ಅಕ್ಷರ	*p;
    ಪೂರ್ಣಾಂಕ		i;

    choices[choice_ಎಣಿಕೆ].arg = ಗುರಿ;
    choices[choice_ಎಣಿಕೆ].installfunc = install_bat_choice;
    choices[choice_ಎಣಿಕೆ].ಸಕ್ರಿಯ = 1;
    choices[choice_ಎಣಿಕೆ].ಪಠ್ಯ = ಖಾಲಿ;	// will be set below
    ಆದರೆ (oldbat != ಖಾಲಿ)
    {
	// A [g]vim.bat exists: Only choice is to overwrite it or not.
	choices[choice_ಎಣಿಕೆ].changefunc = toggle_bat_choice;
	*batpath = NUL;
	toggle_bat_choice(choice_ಎಣಿಕೆ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (default_bat_dir != ಖಾಲಿ)
	    // Prefer using the same path as an existing .bat file.
	    strcpy(batpath, default_bat_dir);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // No [g]vim.bat exists: Write it to a directory in $PATH.  Use
	    // $WINDIR by default, if it's empty the first item in $PATH.
	    p = getenv("WINDIR");
	    ಆದರೆ (p != ಖಾಲಿ && *p != NUL)
		strcpy(batpath, p);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		p = getenv("PATH");
		ಆದರೆ (p == ಖಾಲಿ || *p == NUL)		// "cannot happen"
		    strcpy(batpath, "C:/Windows");
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    i = 0;
		    ಆಗ (*p != NUL && *p != ';')
			batpath[i++] = *p++;
		    batpath[i] = NUL;
		}
	    }
	}
	ಕೂಡಿಸು_pathsep(batpath);
	ಗಣ_bat_ಪಠ್ಯ(choice_ಎಣಿಕೆ, batpath, targets[ಗುರಿ].batname);

	choices[choice_ಎಣಿಕೆ].changefunc = change_bat_choice;
    }
    ++choice_ಎಣಿಕೆ;
}

/*
 * Set up the choices for installing .bat files.
 * For these items "arg" is the index in targets[].
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
init_bat_choices(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		i;

    // The first item is used to switch installing batch files on/off and
    // setting the default path.
    choices[choice_ಎಣಿಕೆ].ಪಠ್ಯ = bat_ಪಠ್ಯ_yes;
    choices[choice_ಎಣಿಕೆ].changefunc = change_main_bat_choice;
    choices[choice_ಎಣಿಕೆ].installfunc = ಖಾಲಿ;
    choices[choice_ಎಣಿಕೆ].ಸಕ್ರಿಯ = 1;
    choices[choice_ಎಣಿಕೆ].arg = 0;
    ++choice_ಎಣಿಕೆ;

    // Add items for each batch file target.  Only used when not disabled by
    // the first item.  When a .exe exists, don't offer to create a .bat.
    ಗಾಗಿ (i = 1; i < ಗುರಿ_ಎಣಿಕೆ; ++i)
	ಆದರೆ (targets[i].oldexe == ಖಾಲಿ
		&& (targets[i].exenamearg[0] == 'g' ? has_gvim : has_vim))
	    init_bat_choice(i);
	ಇಲ್ಲದಿದ್ದರೆ
	    ಕೂಡಿಸು_dummy_choice();
}

/*
 * Install the vimrc file.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
install_vimrc(ಪೂರ್ಣಾಂಕ idx ಬಳಸದ)
{
    FILE	*fd, *tfd;
    ಅಕ್ಷರ	*fname;

    // If an old vimrc file exists, overwrite it.
    // Otherwise create a new one.
    ಆದರೆ (*oldvimrc != NUL)
	fname = oldvimrc;
    ಇಲ್ಲದಿದ್ದರೆ
	fname = vimrc;

    fd = fopen(fname, "w");
    ಆದರೆ (fd == ಖಾಲಿ)
    {
	ಬರೆ("\nERROR: Cannot open \"%s\" for writing.\n", fname);
	ಹಿಂತಿರುಗಿ;
    }
    ಆಯ್ಕೆ (compat_choice)
    {
	ಸ್ಥಿತಿ compat_vi:
		    fprintf(fd, "\" Vi compatible\n");
		    fprintf(fd, "set compatible\n");
		    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ compat_vim:
		    fprintf(fd, "\" Vim's default behavior\n");
		    fprintf(fd, "if &compatible\n");
		    fprintf(fd, "  set nocompatible\n");
		    fprintf(fd, "endif\n");
		    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ compat_some_enhancements:
		    fprintf(fd, "\" Vim with some enhancements\n");
		    fprintf(fd, "source $VIMRUNTIME/defaults.vim\n");
		    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ compat_all_enhancements:
		    fprintf(fd, "\" Vim with all enhancements\n");
		    fprintf(fd, "source $VIMRUNTIME/vimrc_example.vim\n");
		    ನಿಲ್ಲಿಸು;
    }
    ಆಯ್ಕೆ (remap_choice)
    {
	ಸ್ಥಿತಿ remap_no:
		    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ remap_win:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Remap a few keys for Windows behavior\n");
		    fprintf(fd, "source $VIMRUNTIME/mswin.vim\n");
		    ನಿಲ್ಲಿಸು;
    }
    ಆಯ್ಕೆ (mouse_choice)
    {
	ಸ್ಥಿತಿ mouse_xterm:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Mouse behavior (the Unix way)\n");
		    fprintf(fd, "behave xterm\n");
		    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ mouse_mswin:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Mouse behavior (the Windows way)\n");
		    fprintf(fd, "behave mswin\n");
		    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ mouse_default:
		    ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ ((tfd = fopen("diff.exe", "r")) != ಖಾಲಿ)
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
    ಬರೆ("%s has been written\n", fname);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
change_vimrc_choice(ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (choices[idx].installfunc != ಖಾಲಿ)
    {
	// Switch to NOT change or create a vimrc file.
	ಆದರೆ (*oldvimrc != NUL)
	    alloc_ಪಠ್ಯ(idx, "Do NOT change startup file %s", oldvimrc);
	ಇಲ್ಲದಿದ್ದರೆ
	    alloc_ಪಠ್ಯ(idx, "Do NOT create startup file %s", vimrc);
	choices[idx].installfunc = ಖಾಲಿ;
	choices[idx + 1].ಸಕ್ರಿಯ = 0;
	choices[idx + 2].ಸಕ್ರಿಯ = 0;
	choices[idx + 3].ಸಕ್ರಿಯ = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Switch to change or create a vimrc file.
	ಆದರೆ (*oldvimrc != NUL)
	    alloc_ಪಠ್ಯ(idx, "Overwrite startup file %s with:", oldvimrc);
	ಇಲ್ಲದಿದ್ದರೆ
	    alloc_ಪಠ್ಯ(idx, "Create startup file %s with:", vimrc);
	choices[idx].installfunc = install_vimrc;
	choices[idx + 1].ಸಕ್ರಿಯ = 1;
	choices[idx + 2].ಸಕ್ರಿಯ = 1;
	choices[idx + 3].ಸಕ್ರಿಯ = 1;
    }
}

/*
 * Change the choice how to run Vim.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
change_ಸತತ_choice(ಪೂರ್ಣಾಂಕ idx)
{
    compat_choice = get_choice(compat_choices, ಕೋಷ್ಟಕ_ಗಾತ್ರ(compat_choices));
    alloc_ಪಠ್ಯ(idx, compat_ಪಠ್ಯ, compat_choices[compat_choice]);
}

/*
 * Change the choice if keys are to be remapped.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
change_remap_choice(ಪೂರ್ಣಾಂಕ idx)
{
    remap_choice = get_choice(remap_choices, ಕೋಷ್ಟಕ_ಗಾತ್ರ(remap_choices));
    alloc_ಪಠ್ಯ(idx, remap_ಪಠ್ಯ, remap_choices[remap_choice]);
}

/*
 * Change the choice how to select text.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
change_mouse_choice(ಪೂರ್ಣಾಂಕ idx)
{
    mouse_choice = get_choice(mouse_choices, ಕೋಷ್ಟಕ_ಗಾತ್ರ(mouse_choices));
    alloc_ಪಠ್ಯ(idx, mouse_ಪಠ್ಯ, mouse_choices[mouse_choice]);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
init_vimrc_choices(ಶೂನ್ಯ)
{
    // set path for a new _vimrc file (also when not used)
    strcpy(vimrc, installdir);
    strcpy(vimrc + runtimeidx, "_vimrc");

    // Set opposite value and then toggle it by calling change_vimrc_choice()
    ಆದರೆ (*oldvimrc == NUL)
	choices[choice_ಎಣಿಕೆ].installfunc = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
	choices[choice_ಎಣಿಕೆ].installfunc = install_vimrc;
    choices[choice_ಎಣಿಕೆ].ಪಠ್ಯ = ಖಾಲಿ;
    change_vimrc_choice(choice_ಎಣಿಕೆ);
    choices[choice_ಎಣಿಕೆ].changefunc = change_vimrc_choice;
    choices[choice_ಎಣಿಕೆ].ಸಕ್ರಿಯ = 1;
    ++choice_ಎಣಿಕೆ;

    // default way to run Vim
    alloc_ಪಠ್ಯ(choice_ಎಣಿಕೆ, compat_ಪಠ್ಯ, compat_choices[compat_choice]);
    choices[choice_ಎಣಿಕೆ].changefunc = change_ಸತತ_choice;
    choices[choice_ಎಣಿಕೆ].installfunc = ಖಾಲಿ;
    choices[choice_ಎಣಿಕೆ].ಸಕ್ರಿಯ = (*oldvimrc == NUL);
    ++choice_ಎಣಿಕೆ;

    // Whether to remap keys
    alloc_ಪಠ್ಯ(choice_ಎಣಿಕೆ, remap_ಪಠ್ಯ , remap_choices[remap_choice]);
    choices[choice_ಎಣಿಕೆ].changefunc = change_remap_choice;
    choices[choice_ಎಣಿಕೆ].installfunc = ಖಾಲಿ;
    choices[choice_ಎಣಿಕೆ].ಸಕ್ರಿಯ = (*oldvimrc == NUL);
    ++choice_ಎಣಿಕೆ;

    // default way to use the mouse
    alloc_ಪಠ್ಯ(choice_ಎಣಿಕೆ, mouse_ಪಠ್ಯ, mouse_choices[mouse_choice]);
    choices[choice_ಎಣಿಕೆ].changefunc = change_mouse_choice;
    choices[choice_ಎಣಿಕೆ].installfunc = ಖಾಲಿ;
    choices[choice_ಎಣಿಕೆ].ಸಕ್ರಿಯ = (*oldvimrc == NUL);
    ++choice_ಎಣಿಕೆ;
}

    ಸ್ಥಿರ LONG
reg_create_ಕೀಲಿ(
    HKEY ಬೇರು,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *subkey,
    PHKEY ph_ಕೀಲಿ,
    DWORD flag)
{
    DWORD disp;

    *ph_ಕೀಲಿ = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ Reg_Create_ಕೀಲಿ_Ex(
		ಬೇರು, subkey,
		0, ಖಾಲಿ, REG_OPTION_NON_VOLATILE,
		flag | ಕೀಲಿ_WRITE,
		ಖಾಲಿ, ph_ಕೀಲಿ, &disp);
}

    ಸ್ಥಿರ LONG
reg_ಗಣ_string_ಮೌಲ್ಯ(
    HKEY h_ಕೀಲಿ,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ_name,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ದತ್ತಾಂಶ)
{
    ಹಿಂತಿರುಗಿ Reg_ಗಣ_ಮೌಲ್ಯ_Ex(h_ಕೀಲಿ, ಮೌಲ್ಯ_name, 0, REG_SZ,
				     (LPBYTE)ದತ್ತಾಂಶ, (DWORD)(1 + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ದತ್ತಾಂಶ)));
}

    ಸ್ಥಿರ LONG
reg_create_ಕೀಲಿ_and_ಮೌಲ್ಯ(
    HKEY h_ಬೇರು_ಕೀಲಿ,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *subkey,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ_name,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ದತ್ತಾಂಶ,
    DWORD flag)
{
    HKEY h_ಕೀಲಿ;
    LONG lRet = reg_create_ಕೀಲಿ(h_ಬೇರು_ಕೀಲಿ, subkey, &h_ಕೀಲಿ, flag);

    ಆದರೆ (ERROR_SUCCESS == lRet)
    {
	lRet = reg_ಗಣ_string_ಮೌಲ್ಯ(h_ಕೀಲಿ, ಮೌಲ್ಯ_name, ದತ್ತಾಂಶ);
	Reg_Close_ಕೀಲಿ(h_ಕೀಲಿ);
    }
    ಹಿಂತಿರುಗಿ lRet;
}

    ಸ್ಥಿರ LONG
ರಿಜಿಸ್ಟರ್_inproc_server(
    HKEY h_ಬೇರು_ಕೀಲಿ,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *clsid,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extname,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *module,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *threading_model,
    DWORD flag)
{
    CHAR subkey[BUFSIZE];
    LONG lRet;

    sprintf(subkey, "CLSID\\%s", clsid);
    lRet = reg_create_ಕೀಲಿ_and_ಮೌಲ್ಯ(h_ಬೇರು_ಕೀಲಿ, subkey, ಖಾಲಿ, extname, flag);
    ಆದರೆ (ERROR_SUCCESS == lRet)
    {
	sprintf(subkey, "CLSID\\%s\\InProcServer32", clsid);
	lRet = reg_create_ಕೀಲಿ_and_ಮೌಲ್ಯ(h_ಬೇರು_ಕೀಲಿ, subkey, ಖಾಲಿ, module, flag);
	ಆದರೆ (ERROR_SUCCESS == lRet)
	{
	    lRet = reg_create_ಕೀಲಿ_and_ಮೌಲ್ಯ(h_ಬೇರು_ಕೀಲಿ, subkey,
					   "ThreadingModel", threading_model, flag);
	}
    }
    ಹಿಂತಿರುಗಿ lRet;
}

    ಸ್ಥಿರ LONG
ರಿಜಿಸ್ಟರ್_shellex(
    HKEY h_ಬೇರು_ಕೀಲಿ,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *clsid,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *exe_ಮಾರ್ಗ,
    DWORD flag)
{
    LONG lRet = reg_create_ಕೀಲಿ_and_ಮೌಲ್ಯ(
	    h_ಬೇರು_ಕೀಲಿ,
	    "*\\shellex\\ContextMenuHandlers\\gvim",
	    ಖಾಲಿ,
	    clsid,
	    flag);

    ಆದರೆ (ERROR_SUCCESS == lRet)
    {
	lRet = reg_create_ಕೀಲಿ_and_ಮೌಲ್ಯ(
		HKEY_LOCAL_MACHINE,
		"Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved",
		clsid,
		name,
		flag);

	ಆದರೆ (ERROR_SUCCESS == lRet)
	{
	    lRet = reg_create_ಕೀಲಿ_and_ಮೌಲ್ಯ(
		    HKEY_LOCAL_MACHINE,
		    "Software\\Vim\\Gvim",
		    "path",
		    exe_ಮಾರ್ಗ,
		    flag);
	}
    }
    ಹಿಂತಿರುಗಿ lRet;
}

    ಸ್ಥಿರ LONG
ರಿಜಿಸ್ಟರ್_openwith(
    HKEY h_ಬೇರು_ಕೀಲಿ,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *exe_ಮಾರ್ಗ,
    DWORD flag)
{
    ಅಕ್ಷರ	exe_cmd[BUFSIZE];
    LONG	lRet;

    sprintf(exe_cmd, "\"%s\" \"%%1\"", exe_ಮಾರ್ಗ);
    lRet = reg_create_ಕೀಲಿ_and_ಮೌಲ್ಯ(
	    h_ಬೇರು_ಕೀಲಿ,
	    "Applications\\gvim.exe\\shell\\edit\\command",
	    ಖಾಲಿ,
	    exe_cmd,
	    flag);

    ಆದರೆ (ERROR_SUCCESS == lRet)
    {
	ಪೂರ್ಣಾಂಕ i;
	ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *openwith[] = {
		".htm\\OpenWithList\\gvim.exe",
		".vim\\OpenWithList\\gvim.exe",
		"*\\OpenWithList\\gvim.exe",
	};

	ಗಾಗಿ (i = 0; ERROR_SUCCESS == lRet && i < (ಪೂರ್ಣಾಂಕ)ARRAYSIZE(openwith); i++)
	    lRet = reg_create_ಕೀಲಿ_and_ಮೌಲ್ಯ(h_ಬೇರು_ಕೀಲಿ, openwith[i], ಖಾಲಿ, "", flag);
    }

    ಹಿಂತಿರುಗಿ lRet;
}

    ಸ್ಥಿರ LONG
ರಿಜಿಸ್ಟರ್_uninstall(
    HKEY h_ಬೇರು_ಕೀಲಿ,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *appname,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *display_name,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *uninstall_string,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *display_icon,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *display_ಆವೃತ್ತಿ,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *publisher)
{
    LONG lRet = reg_create_ಕೀಲಿ_and_ಮೌಲ್ಯ(h_ಬೇರು_ಕೀಲಿ, appname,
			     "DisplayName", display_name, ಕೀಲಿ_WOW64_64_ಕೀಲಿ);

    ಆದರೆ (ERROR_SUCCESS == lRet)
	lRet = reg_create_ಕೀಲಿ_and_ಮೌಲ್ಯ(h_ಬೇರು_ಕೀಲಿ, appname,
		     "UninstallString", uninstall_string, ಕೀಲಿ_WOW64_64_ಕೀಲಿ);
    ಆದರೆ (ERROR_SUCCESS == lRet)
	lRet = reg_create_ಕೀಲಿ_and_ಮೌಲ್ಯ(h_ಬೇರು_ಕೀಲಿ, appname,
		     "DisplayIcon", display_icon, ಕೀಲಿ_WOW64_64_ಕೀಲಿ);
    ಆದರೆ (ERROR_SUCCESS == lRet)
	lRet = reg_create_ಕೀಲಿ_and_ಮೌಲ್ಯ(h_ಬೇರು_ಕೀಲಿ, appname,
		     "DisplayVersion", display_ಆವೃತ್ತಿ, ಕೀಲಿ_WOW64_64_ಕೀಲಿ);
    ಆದರೆ (ERROR_SUCCESS == lRet)
	lRet = reg_create_ಕೀಲಿ_and_ಮೌಲ್ಯ(h_ಬೇರು_ಕೀಲಿ, appname,
		     "Publisher", publisher, ಕೀಲಿ_WOW64_64_ಕೀಲಿ);
    ಹಿಂತಿರುಗಿ lRet;
}

/*
 * Add some entries to the registry:
 * - to add "Edit with Vim" to the context * menu
 * - to add Vim to the "Open with..." list
 * - to uninstall Vim
 */
//ARGSUSED
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
install_registry(ಶೂನ್ಯ)
{
    LONG	lRet = ERROR_SUCCESS;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	*vim_ext_ThreadingModel = "Apartment";
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	*vim_ext_name = "Vim Shell Extension";
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	*vim_ext_clsid = "{51EEE242-AD87-11d3-9C1E-0090278BBD99}";
    ಅಕ್ಷರ	vim_exe_ಮಾರ್ಗ[MAX_ಮಾರ್ಗ];
    ಅಕ್ಷರ	display_name[BUFSIZE];
    ಅಕ್ಷರ	uninstall_string[BUFSIZE];
    ಅಕ್ಷರ	icon_string[BUFSIZE];
    ಅಕ್ಷರ	ಆವೃತ್ತಿ_string[BUFSIZE];
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		loop_ಎಣಿಕೆ = is_64bit_os() ? 2 : 1;
    DWORD	flag;

    sprintf(vim_exe_ಮಾರ್ಗ, "%s\\gvim.exe", installdir);

    ಆದರೆ (install_popup)
    {
	ಅಕ್ಷರ	    bufg[BUFSIZE];

	ಬರೆ("Creating \"Edit with Vim\" popup menu entry\n");

	ಗಾಗಿ (i = 0; i < loop_ಎಣಿಕೆ; i++)
	{
	    ಆದರೆ (i == 0)
	    {
		sprintf(bufg, "%s\\" GVIMEXT32_ಮಾರ್ಗ, installdir);
		flag = ಕೀಲಿ_WOW64_32_ಕೀಲಿ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		sprintf(bufg, "%s\\" GVIMEXT64_ಮಾರ್ಗ, installdir);
		flag = ಕೀಲಿ_WOW64_64_ಕೀಲಿ;
	    }

	    lRet = ರಿಜಿಸ್ಟರ್_inproc_server(
		    HKEY_CLASSES_ಬೇರು, vim_ext_clsid, vim_ext_name,
		    bufg, vim_ext_ThreadingModel, flag);
	    ಆದರೆ (ERROR_SUCCESS != lRet)
		ಹಿಂತಿರುಗಿ FAIL;
	    lRet = ರಿಜಿಸ್ಟರ್_shellex(
		    HKEY_CLASSES_ಬೇರು, vim_ext_clsid, vim_ext_name,
		    vim_exe_ಮಾರ್ಗ, flag);
	    ಆದರೆ (ERROR_SUCCESS != lRet)
		ಹಿಂತಿರುಗಿ FAIL;
	}
    }

    ಆದರೆ (install_openwith)
    {
	ಬರೆ("Creating \"Open with ...\" list entry\n");

	ಗಾಗಿ (i = 0; i < loop_ಎಣಿಕೆ; i++)
	{
	    ಆದರೆ (i == 0)
		flag = ಕೀಲಿ_WOW64_32_ಕೀಲಿ;
	    ಇಲ್ಲದಿದ್ದರೆ
		flag = ಕೀಲಿ_WOW64_64_ಕೀಲಿ;

	    lRet = ರಿಜಿಸ್ಟರ್_openwith(HKEY_CLASSES_ಬೇರು, vim_exe_ಮಾರ್ಗ, flag);
	    ಆದರೆ (ERROR_SUCCESS != lRet)
		ಹಿಂತಿರುಗಿ FAIL;
	}
    }

    ಬರೆ("Creating an uninstall entry\n");
    sprintf(display_name, "Vim " VIM_ಆವೃತ್ತಿ_SHORT
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _M_ARM64
	    " (arm64)"
#ಅಥವಾಆದರೆ _M_X64
	    " (x64)"
#ಆದರೆ_ಅಂತ್ಯ
	    );

    // For the NSIS installer use the generated uninstaller.
    ಆದರೆ (interactive)
	sprintf(uninstall_string, "%s\\uninstall.exe", installdir);
    ಇಲ್ಲದಿದ್ದರೆ
	sprintf(uninstall_string, "%s\\uninstall-gui.exe", installdir);

    sprintf(icon_string, "%s\\gvim.exe,0", installdir);

    sprintf(ಆವೃತ್ತಿ_string, VIM_ಆವೃತ್ತಿ_SHORT "." VIM_ಆವೃತ್ತಿ_PATCHLEVEL_STR);

    lRet = ರಿಜಿಸ್ಟರ್_uninstall(
	HKEY_LOCAL_MACHINE,
	"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Vim " VIM_ಆವೃತ್ತಿ_SHORT,
	display_name,
	uninstall_string,
	icon_string,
	ಆವೃತ್ತಿ_string,
	"The Vim Project");
    ಆದರೆ (ERROR_SUCCESS != lRet)
	ಹಿಂತಿರುಗಿ FAIL;

    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
change_popup_choice(ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (install_popup == 0)
    {
	choices[idx].ಪಠ್ಯ = "Install an entry for Vim in the popup menu for the right\n    mouse button so that you can edit any file with Vim";
	install_popup = 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	choices[idx].ಪಠ್ಯ = "Do NOT install an entry for Vim in the popup menu for the\n    right mouse button to edit any file with Vim";
	install_popup = 0;
    }
}

/*
 * Only add the choice for the popup menu entry when gvim.exe was found and
 * both gvimext.dll and regedit.exe exist.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
init_popup_choice(ಶೂನ್ಯ)
{
    ರಚನೆ stat	st;

    ಆದರೆ (has_gvim
	    && (stat(GVIMEXT32_ಮಾರ್ಗ, &st) >= 0
		|| stat(GVIMEXT64_ಮಾರ್ಗ, &st) >= 0))
    {
	choices[choice_ಎಣಿಕೆ].changefunc = change_popup_choice;
	choices[choice_ಎಣಿಕೆ].installfunc = ಖಾಲಿ;
	choices[choice_ಎಣಿಕೆ].ಸಕ್ರಿಯ = 1;
	change_popup_choice(choice_ಎಣಿಕೆ);  // set the text
	++choice_ಎಣಿಕೆ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಕೂಡಿಸು_dummy_choice();
}

    ಸ್ಥಿರ ಶೂನ್ಯ
change_openwith_choice(ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (install_openwith == 0)
    {
	choices[idx].ಪಠ್ಯ = "Add Vim to the \"Open With...\" list in the popup menu for the right\n    mouse button so that you can edit any file with Vim";
	install_openwith = 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	choices[idx].ಪಠ್ಯ = "Do NOT add Vim to the \"Open With...\" list in the popup menu for the\n    right mouse button to edit any file with Vim";
	install_openwith = 0;
    }
}

/*
 * Only add the choice for the open-with menu entry when gvim.exe was found
 * and regedit.exe exist.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
init_openwith_choice(ಶೂನ್ಯ)
{
    ಆದರೆ (has_gvim)
    {
	choices[choice_ಎಣಿಕೆ].changefunc = change_openwith_choice;
	choices[choice_ಎಣಿಕೆ].installfunc = ಖಾಲಿ;
	choices[choice_ಎಣಿಕೆ].ಸಕ್ರಿಯ = 1;
	change_openwith_choice(choice_ಎಣಿಕೆ);  // set the text
	++choice_ಎಣಿಕೆ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಕೂಡಿಸು_dummy_choice();
}

/*
 * Create a shell link.
 *
 * returns 0 on failure, non-zero on successful completion.
 *
 * NOTE:  Currently untested with mingw.
 */
    ಪೂರ್ಣಾಂಕ
create_shortcut(
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *shortcut_name,
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *iconfile_ಮಾರ್ಗ,
	ಪೂರ್ಣಾಂಕ	    iconindex,
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *shortcut_ಗುರಿ,
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *shortcut_ವಾದಗಳು,
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *workingdir
	)
{
    IShellLink	    *shelllink_ptr;
    HRESULT	    hres;
    IPersistFile	    *persistfile_ptr;

    // Initialize COM library
    hres = CoInitialize(ಖಾಲಿ);
    ಆದರೆ (!SUCCEEDED(hres))
    {
	ಬರೆ("Error:  Could not open the COM library.  Not creating shortcut.\n");
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // Instantiate a COM object for the ShellLink, store a pointer to it
    // in shelllink_ptr.
    hres = CoCreateInstance(&CLSID_ShellLink,
			   ಖಾಲಿ,
			   CLSCTX_INPROC_SERVER,
			   &IID_IShellLink,
			   (ಶೂನ್ಯ **) &shelllink_ptr);

    ಆದರೆ (SUCCEEDED(hres)) // If the instantiation was successful...
    {
	// ...Then build a PersistFile interface for the ShellLink so we can
	// save it as a file after we build it.
	hres = shelllink_ptr->lpVtbl->QueryInterface(shelllink_ptr,
		&IID_IPersistFile, (ಶೂನ್ಯ **) &persistfile_ptr);

	ಆದರೆ (SUCCEEDED(hres))
	{
	    wchar_t wsz[BUFSIZE];

	    // translate the (possibly) multibyte shortcut filename to windows
	    // Unicode so it can be used as a file name.
	    Multi_Byte_ಗಮ್ಯ_Wide_Char(CP_ACP, 0, shortcut_name, -1, wsz, ಗಾತ್ರ(wsz)/ಗಾತ್ರ(wsz[0]));

	    // set the attributes
	    shelllink_ptr->lpVtbl->ಗಣ_ಮಾರ್ಗ(shelllink_ptr, shortcut_ಗುರಿ);
	    shelllink_ptr->lpVtbl->ಗಣ_Working_Directory(shelllink_ptr,
								  workingdir);
	    shelllink_ptr->lpVtbl->ಗಣ_Icon_Location(shelllink_ptr,
						    iconfile_ಮಾರ್ಗ, iconindex);
	    shelllink_ptr->lpVtbl->ಗಣ_Arguments(shelllink_ptr, shortcut_ವಾದಗಳು);

	    // save the shortcut to a file and return the PersistFile object
	    persistfile_ptr->lpVtbl->Save(persistfile_ptr, wsz, 1);
	    persistfile_ptr->lpVtbl->Release(persistfile_ptr);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಬರೆ("QueryInterface Error\n");
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	// Return the ShellLink object
	shelllink_ptr->lpVtbl->Release(shelllink_ptr);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಬರೆ("CoCreateInstance Error - hres = %08x\n", (ಪೂರ್ಣಾಂಕ)hres);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Build a path to where we will put a specified link.
 *
 * Return 0 on error, non-zero on success
 */
   ಪೂರ್ಣಾಂಕ
build_link_name(
	ಅಕ್ಷರ	   *link_ಮಾರ್ಗ,
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *link_name,
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *shell_folder_name)
{
    ಅಕ್ಷರ	shell_folder_ಮಾರ್ಗ[MAX_ಮಾರ್ಗ];

    ಆದರೆ (get_shell_folder_ಮಾರ್ಗ(shell_folder_ಮಾರ್ಗ, shell_folder_name) == FAIL)
    {
	ಬರೆ("An error occurred while attempting to find the path to %s.\n",
							   shell_folder_name);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // Make sure the directory exists (create Start Menu\Programs\Vim).
    // Ignore errors if it already exists.
    vim_mkdir(shell_folder_ಮಾರ್ಗ, 0755);

    // build the path to the shortcut and the path to gvim.exe
    sprintf(link_ಮಾರ್ಗ, "%s\\%s.lnk", shell_folder_ಮಾರ್ಗ, link_name);

    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
build_shortcut(
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,	// Name of the shortcut
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *exename,	// Name of the executable (e.g., vim.exe)
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ವಾದಗಳು,
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *shell_folder,
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *workingdir)
{
    ಅಕ್ಷರ	executable_ಮಾರ್ಗ[BUFSIZE];
    ಅಕ್ಷರ	link_name[BUFSIZE];

    sprintf(executable_ಮಾರ್ಗ, "%s\\%s", installdir, exename);

    ಆದರೆ (build_link_name(link_name, name, shell_folder) == FAIL)
    {
	ಬರೆ("An error has occurred.  A shortcut to %s will not be created %s.\n",
		name,
		*shell_folder == 'd' ? "on the desktop" : "in the Start menu");
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // Create the shortcut:
    ಹಿಂತಿರುಗಿ create_shortcut(link_name, executable_ಮಾರ್ಗ, 0,
					   executable_ಮಾರ್ಗ, ವಾದಗಳು, workingdir);
}

/*
 * We used to use "homedir" as the working directory, but that is a bad choice
 * on multi-user systems.  However, not specifying a directory results in the
 * current directory to be c:\Windows\system32 on Windows 7. Use environment
 * variables instead.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು WORKDIR "%HOMEDRIVE%%HOMEPATH%"

/*
 * Create shortcut(s) in the Start Menu\Programs\Vim folder.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
install_ಆರಂಭ_menu(ಪೂರ್ಣಾಂಕ idx ಬಳಸದ)
{
    need_uninstall_ನಮೂದು = 1;
    ಬರೆ("Creating start menu\n");
    ಆದರೆ (has_vim)
    {
	ಆದರೆ (build_shortcut("Vim", "vim.exe", "",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ಹಿಂತಿರುಗಿ;
	ಆದರೆ (build_shortcut("Vim Read-only", "vim.exe", "-R",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ಹಿಂತಿರುಗಿ;
	ಆದರೆ (build_shortcut("Vim Diff", "vim.exe", "-d",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (has_gvim)
    {
	ಆದರೆ (build_shortcut("gVim", "gvim.exe", "",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ಹಿಂತಿರುಗಿ;
	ಆದರೆ (build_shortcut("gVim Easy", "gvim.exe", "-y",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ಹಿಂತಿರುಗಿ;
	ಆದರೆ (build_shortcut("gVim Read-only", "gvim.exe", "-R",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ಹಿಂತಿರುಗಿ;
	ಆದರೆ (build_shortcut("gVim Diff", "gvim.exe", "-d",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (build_shortcut("Uninstall",
		interactive ? "uninstall.exe" : "uninstall-gui.exe", "",
					   VIM_STARTMENU, installdir) == FAIL)
	ಹಿಂತಿರುಗಿ;
    // For Windows NT the working dir of the vimtutor.bat must be right,
    // otherwise gvim.exe won't be found and using gvimbat doesn't work.
    ಆದರೆ (build_shortcut("Vim tutor", "vimtutor.bat", "",
					   VIM_STARTMENU, installdir) == FAIL)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (build_shortcut("Help", has_gvim ? "gvim.exe" : "vim.exe", "-c h",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	ಹಿಂತಿರುಗಿ;
    {
	ಅಕ್ಷರ	shell_folder_ಮಾರ್ಗ[BUFSIZE];

	// Creating the URL shortcut works a bit differently...
	ಆದರೆ (get_shell_folder_ಮಾರ್ಗ(shell_folder_ಮಾರ್ಗ, VIM_STARTMENU) == FAIL)
	{
	    ಬರೆ("Finding the path of the Start menu failed\n");
	    ಹಿಂತಿರುಗಿ ;
	}
	ಕೂಡಿಸು_pathsep(shell_folder_ಮಾರ್ಗ);
	strcat(shell_folder_ಮಾರ್ಗ, "Vim Online.url");
	ಆದರೆ (!Write_ಖಾಸಗಿ_Profile_String("InternetShortcut", "URL",
				    "https://www.vim.org/", shell_folder_ಮಾರ್ಗ))
	{
	    ಬರೆ("Creating the Vim online URL failed\n");
	    ಹಿಂತಿರುಗಿ;
	}
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
toggle_startmenu_choice(ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (choices[idx].installfunc == ಖಾಲಿ)
    {
	choices[idx].installfunc = install_ಆರಂಭ_menu;
	choices[idx].ಪಠ್ಯ = "Add Vim to the Start menu";
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	choices[idx].installfunc = ಖಾಲಿ;
	choices[idx].ಪಠ್ಯ = "Do NOT add Vim to the Start menu";
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
    ಶೂನ್ಯ
install_shortcut_gvim(ಪೂರ್ಣಾಂಕ idx)
{
    // Create shortcut(s) on the desktop
    ಆದರೆ (choices[idx].arg)
    {
	(ಶೂನ್ಯ)build_shortcut(icon_ಹೆಸರುಗಳು[0], "gvim.exe",
						      "", "desktop", WORKDIR);
	need_uninstall_ನಮೂದು = 1;
    }
}

    ಶೂನ್ಯ
install_shortcut_evim(ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (choices[idx].arg)
    {
	(ಶೂನ್ಯ)build_shortcut(icon_ಹೆಸರುಗಳು[1], "gvim.exe",
						    "-y", "desktop", WORKDIR);
	need_uninstall_ನಮೂದು = 1;
    }
}

    ಶೂನ್ಯ
install_shortcut_gview(ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (choices[idx].arg)
    {
	(ಶೂನ್ಯ)build_shortcut(icon_ಹೆಸರುಗಳು[2], "gvim.exe",
						    "-R", "desktop", WORKDIR);
	need_uninstall_ನಮೂದು = 1;
    }
}

    ಶೂನ್ಯ
toggle_shortcut_choice(ಪೂರ್ಣಾಂಕ idx)
{
    ಅಕ್ಷರ	*arg;

    ಆದರೆ (choices[idx].installfunc == install_shortcut_gvim)
	arg = "gVim";
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (choices[idx].installfunc == install_shortcut_evim)
	arg = "gVim Easy";
    ಇಲ್ಲದಿದ್ದರೆ
	arg = "gVim Read-only";
    ಆದರೆ (choices[idx].arg)
    {
	choices[idx].arg = 0;
	alloc_ಪಠ್ಯ(idx, "Do NOT create a desktop icon for %s", arg);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	choices[idx].arg = 1;
	alloc_ಪಠ್ಯ(idx, "Create a desktop icon for %s", arg);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
init_startmenu_choice(ಶೂನ್ಯ)
{
    // Start menu
    choices[choice_ಎಣಿಕೆ].changefunc = toggle_startmenu_choice;
    choices[choice_ಎಣಿಕೆ].installfunc = ಖಾಲಿ;
    choices[choice_ಎಣಿಕೆ].ಸಕ್ರಿಯ = 1;
    toggle_startmenu_choice(choice_ಎಣಿಕೆ);	// set the text
    ++choice_ಎಣಿಕೆ;
}

/*
 * Add the choice for the desktop shortcuts.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
init_shortcut_choices(ಶೂನ್ಯ)
{
    // Shortcut to gvim
    choices[choice_ಎಣಿಕೆ].ಪಠ್ಯ = ಖಾಲಿ;
    choices[choice_ಎಣಿಕೆ].arg = 0;
    choices[choice_ಎಣಿಕೆ].ಸಕ್ರಿಯ = has_gvim;
    choices[choice_ಎಣಿಕೆ].changefunc = toggle_shortcut_choice;
    choices[choice_ಎಣಿಕೆ].installfunc = install_shortcut_gvim;
    toggle_shortcut_choice(choice_ಎಣಿಕೆ);
    ++choice_ಎಣಿಕೆ;

    // Shortcut to evim
    choices[choice_ಎಣಿಕೆ].ಪಠ್ಯ = ಖಾಲಿ;
    choices[choice_ಎಣಿಕೆ].arg = 0;
    choices[choice_ಎಣಿಕೆ].ಸಕ್ರಿಯ = has_gvim;
    choices[choice_ಎಣಿಕೆ].changefunc = toggle_shortcut_choice;
    choices[choice_ಎಣಿಕೆ].installfunc = install_shortcut_evim;
    toggle_shortcut_choice(choice_ಎಣಿಕೆ);
    ++choice_ಎಣಿಕೆ;

    // Shortcut to gview
    choices[choice_ಎಣಿಕೆ].ಪಠ್ಯ = ಖಾಲಿ;
    choices[choice_ಎಣಿಕೆ].arg = 0;
    choices[choice_ಎಣಿಕೆ].ಸಕ್ರಿಯ = has_gvim;
    choices[choice_ಎಣಿಕೆ].changefunc = toggle_shortcut_choice;
    choices[choice_ಎಣಿಕೆ].installfunc = install_shortcut_gview;
    toggle_shortcut_choice(choice_ಎಣಿಕೆ);
    ++choice_ಎಣಿಕೆ;
}

/*
 * Attempt to register OLE for Vim.
 */
   ಸ್ಥಿರ ಶೂನ್ಯ
install_OLE_ರಿಜಿಸ್ಟರ್(ಶೂನ್ಯ)
{
    ಅಕ್ಷರ ರಿಜಿಸ್ಟರ್_command_string[BUFSIZE + 30];

    ಬರೆ("\n--- Attempting to register Vim with OLE ---\n");
    ಬರೆ("(There is no message whether this works or not.)\n");

    sprintf(ರಿಜಿಸ್ಟರ್_command_string, "\"%s\\gvim.exe\" -silent -register", installdir);
    system(ರಿಜಿಸ್ಟರ್_command_string);
}

/*
 * Remove the last part of directory "path[]" to get its parent, and put the
 * result in "to[]".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
dir_remove_ಕೊನೆಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ ಗಮ್ಯ[MAX_ಮಾರ್ಗ])
{
    ಅಕ್ಷರ c;
    ದೀರ್ಘ ಕೊನೆಯ_char_ಗಮ್ಯ_copy;
    ದೀರ್ಘ ಮಾರ್ಗ_ಉದ್ದಳತೆ = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮಾರ್ಗ);

    // skip the last character just in case it is a '\\'
    ಕೊನೆಯ_char_ಗಮ್ಯ_copy = ಮಾರ್ಗ_ಉದ್ದಳತೆ - 2;
    c = ಮಾರ್ಗ[ಕೊನೆಯ_char_ಗಮ್ಯ_copy];

    ಆಗ (c != '\\')
    {
	ಕೊನೆಯ_char_ಗಮ್ಯ_copy--;
	c = ಮಾರ್ಗ[ಕೊನೆಯ_char_ಗಮ್ಯ_copy];
    }

    strncpy(ಗಮ್ಯ, ಮಾರ್ಗ, (size_t)ಕೊನೆಯ_char_ಗಮ್ಯ_copy);
    ಗಮ್ಯ[ಕೊನೆಯ_char_ಗಮ್ಯ_copy] = NUL;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_directories_ಪಠ್ಯ(ಪೂರ್ಣಾಂಕ idx)
{
    ಪೂರ್ಣಾಂಕ vimfiles_dir_choice = choices[idx].arg;

    ಆದರೆ (vimfiles_dir_choice == (ಪೂರ್ಣಾಂಕ)vimfiles_dir_none)
	alloc_ಪಠ್ಯ(idx, "Do NOT create plugin directories%s", "");
    ಇಲ್ಲದಿದ್ದರೆ
	alloc_ಪಠ್ಯ(idx, "Create plugin directories: %s",
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
ಸ್ಥಿರ ಅಕ್ಷರ *homedir = ಖಾಲಿ;

    ಶೂನ್ಯ
init_homedir(ಶೂನ್ಯ)
{
    ಅಕ್ಷರ    *var;
    ಅಕ್ಷರ    buf[MAX_ಮಾರ್ಗ];

    ಆದರೆ (homedir != ಖಾಲಿ)
    {
	ಬಿಡುಗಡೆ(homedir);
	homedir = ಖಾಲಿ;
    }

    var = getenv("HOME");

    /*
     * Typically, $HOME is not defined on Windows, unless the user has
     * specifically defined it for Vim's sake.  However, on Windows NT
     * platforms, $HOMEDRIVE and $HOMEPATH are automatically defined for
     * each user.  Try constructing $HOME from these.
     */
    ಆದರೆ (var == ಖಾಲಿ || *var == NUL)
    {
	ಅಕ್ಷರ	*homedrive, *homepath;

	homedrive = getenv("HOMEDRIVE");
	homepath = getenv("HOMEPATH");
	ಆದರೆ (homepath == ಖಾಲಿ || *homepath == NUL)
	    homepath = "\\";
	ಆದರೆ (homedrive != ಖಾಲಿ
		   && ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(homedrive) + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(homepath) < ಗಾತ್ರ(buf))
	{
	    ಆದರೆ (snprintf(buf, ಗಾತ್ರ(buf), "%s%s", homedrive, homepath) > 0
		&& buf[0] != NUL)
		var = buf;
	}
    }

    ಆದರೆ (var == ಖಾಲಿ)
	var = getenv("USERPROFILE");

    /*
     * Weird but true: $HOME may contain an indirect reference to another
     * variable, esp. "%USERPROFILE%".  Happens when $USERPROFILE isn't set
     * when $HOME is being set.
     */
    ಆದರೆ (var != ಖಾಲಿ && *var == '%')
    {
	ಅಕ್ಷರ	*p;
	ಅಕ್ಷರ	*ಘಾತಾಂಕ;

	p = strchr(var + 1, '%');
	ಆದರೆ (p != ಖಾಲಿ)
	{
	    strncpy(buf, var + 1, p - (var + 1));
	    buf[p - (var + 1)] = NUL;
	    ಘಾತಾಂಕ = getenv(buf);
	    ಆದರೆ (ಘಾತಾಂಕ != ಖಾಲಿ && *ಘಾತಾಂಕ != NUL
				&& ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಘಾತಾಂಕ) + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(p) < ಗಾತ್ರ(buf))
	    {
		snprintf(buf, ಗಾತ್ರ(buf), "%s%s", ಘಾತಾಂಕ, p + 1);
		var = buf;
	    }
	}
    }

    ಆದರೆ (var != ಖಾಲಿ && *var == NUL)	// empty is same as not set
	var = ಖಾಲಿ;

    ಆದರೆ (var == ಖಾಲಿ)
	homedir = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
	homedir = _strdup(var);
}

/*
 * Change the directory that the vim plugin directories will be created in:
 * $HOME, $VIM or nowhere.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
change_directories_choice(ಪೂರ್ಣಾಂಕ idx)
{
    ಪೂರ್ಣಾಂಕ	    choice_ಎಣಿಕೆ = ಕೋಷ್ಟಕ_ಗಾತ್ರ(vimfiles_dir_choices);

    // Don't offer the $HOME choice if $HOME isn't set.
    ಆದರೆ (homedir == ಖಾಲಿ)
	--choice_ಎಣಿಕೆ;
    choices[idx].arg = get_choice(vimfiles_dir_choices, choice_ಎಣಿಕೆ);
    ಗಣ_directories_ಪಠ್ಯ(idx);
}

/*
 * Create the plugin directories...
 */
//ARGSUSED
    ಸ್ಥಿರ ಶೂನ್ಯ
install_vimfilesdir(ಪೂರ್ಣಾಂಕ idx)
{
    ಪೂರ್ಣಾಂಕ i;
    ಪೂರ್ಣಾಂಕ vimfiles_dir_choice = choices[idx].arg;
    ಅಕ್ಷರ *p;
    ಅಕ್ಷರ vimdir_ಮಾರ್ಗ[MAX_ಮಾರ್ಗ];
    ಅಕ್ಷರ vimfiles_ಮಾರ್ಗ[MAX_ಮಾರ್ಗ + 9];
    ಅಕ್ಷರ tmp_dirname[BUFSIZE];

    // switch on the location that the user wants the plugin directories
    // built in
    ಆಯ್ಕೆ (vimfiles_dir_choice)
    {
	ಸ್ಥಿತಿ vimfiles_dir_vim:
	{
	    // Go to the %VIM% directory - check env first, then go one dir
	    //	   below installdir if there is no %VIM% environment variable.
	    //	   The accuracy of $VIM is checked in inspect_system(), so we
	    //	   can be sure it is ok to use here.
	    p = getenv("VIM");
	    ಆದರೆ (p == ಖಾಲಿ) // No $VIM in path
		dir_remove_ಕೊನೆಯ(installdir, vimdir_ಮಾರ್ಗ);
	    ಇಲ್ಲದಿದ್ದರೆ
		strcpy(vimdir_ಮಾರ್ಗ, p);
	    ನಿಲ್ಲಿಸು;
	}
	ಸ್ಥಿತಿ vimfiles_dir_home:
	{
	    // Find the $HOME directory.  Its existence was already checked.
	    p = homedir;
	    ಆದರೆ (p == ಖಾಲಿ)
	    {
		ಬರೆ("Internal error: $HOME is NULL\n");
		p = "c:\\";
	    }
	    strcpy(vimdir_ಮಾರ್ಗ, p);
	    ನಿಲ್ಲಿಸು;
	}
	ಸ್ಥಿತಿ vimfiles_dir_none:
	{
	    // Do not create vim plugin directory.
	    ಹಿಂತಿರುಗಿ;
	}
    }

    // Now, just create the directory.	If it already exists, it will fail
    // silently.
    sprintf(vimfiles_ಮಾರ್ಗ, "%s\\vimfiles", vimdir_ಮಾರ್ಗ);
    vim_mkdir(vimfiles_ಮಾರ್ಗ, 0755);

    ಬರೆ("Creating the following directories in \"%s\":\n", vimfiles_ಮಾರ್ಗ);
    ಗಾಗಿ (i = 0; i < ಕೋಷ್ಟಕ_ಗಾತ್ರ(vimfiles_subdirs); i++)
    {
	sprintf(tmp_dirname, "%s\\%s", vimfiles_ಮಾರ್ಗ, vimfiles_subdirs[i]);
	ಬರೆ("  %s", vimfiles_subdirs[i]);
	vim_mkdir(tmp_dirname, 0755);
    }
    ಬರೆ("\n");
}

/*
 * Add the creation of runtime files to the setup sequence.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
init_directories_choice(ಶೂನ್ಯ)
{
    ರಚನೆ stat	st;
    ಅಕ್ಷರ	tmp_dirname[BUFSIZE];
    ಅಕ್ಷರ	*p;
    ಪೂರ್ಣಾಂಕ		vimfiles_dir_choice;

    choices[choice_ಎಣಿಕೆ].ಪಠ್ಯ = alloc(150);
    choices[choice_ಎಣಿಕೆ].changefunc = change_directories_choice;
    choices[choice_ಎಣಿಕೆ].installfunc = install_vimfilesdir;
    choices[choice_ಎಣಿಕೆ].ಸಕ್ರಿಯ = 1;

    // Check if the "compiler" directory already exists.  That's a good
    // indication that the plugin directories were already created.
    p = getenv("HOME");
    ಆದರೆ (p != ಖಾಲಿ)
    {
	vimfiles_dir_choice = (ಪೂರ್ಣಾಂಕ)vimfiles_dir_home;
	sprintf(tmp_dirname, "%s\\vimfiles\\compiler", p);
	ಆದರೆ (stat(tmp_dirname, &st) == 0)
	    vimfiles_dir_choice = (ಪೂರ್ಣಾಂಕ)vimfiles_dir_none;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	vimfiles_dir_choice = (ಪೂರ್ಣಾಂಕ)vimfiles_dir_vim;
	p = getenv("VIM");
	ಆದರೆ (p == ಖಾಲಿ)  // No $VIM in path, use the install dir.
	    dir_remove_ಕೊನೆಯ(installdir, tmp_dirname);
	ಇಲ್ಲದಿದ್ದರೆ
	    strcpy(tmp_dirname, p);
	strcat(tmp_dirname, "\\vimfiles\\compiler");
	ಆದರೆ (stat(tmp_dirname, &st) == 0)
	    vimfiles_dir_choice = (ಪೂರ್ಣಾಂಕ)vimfiles_dir_none;
    }

    choices[choice_ಎಣಿಕೆ].arg = vimfiles_dir_choice;
    ಗಣ_directories_ಪಠ್ಯ(choice_ಎಣಿಕೆ);
    ++choice_ಎಣಿಕೆ;
}

/*
 * Setup the choices and the default values.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸಿದ್ಧತೆ_choices(ಶೂನ್ಯ)
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

    ಸ್ಥಿರ ಶೂನ್ಯ
print_cmd_ಸಾಲು_help(ಶೂನ್ಯ)
{
    ಬರೆ("Vim installer non-interactive command line arguments:\n");
    ಬರೆ("\n");
    ಬರೆ("-create-batfiles  [vim gvim evim view gview vimdiff gvimdiff]\n");
    ಬರೆ("    Create .bat files for Vim variants in the Windows directory.\n");
    ಬರೆ("-create-vimrc\n");
    ಬರೆ("    Create a default _vimrc file if one does not already exist.\n");
    ಬರೆ("-vimrc-remap [no|win]\n");
    ಬರೆ("    Remap keys when creating a default _vimrc file.\n");
    ಬರೆ("-vimrc-behave [unix|mswin|default]\n");
    ಬರೆ("    Set mouse behavior when creating a default _vimrc file.\n");
    ಬರೆ("-vimrc-compat [vi|vim|defaults|all]\n");
    ಬರೆ("    Set Vi compatibility when creating a default _vimrc file.\n");
    ಬರೆ("-install-popup\n");
    ಬರೆ("    Install the Edit-with-Vim context menu entry\n");
    ಬರೆ("-install-openwith\n");
    ಬರೆ("    Add Vim to the \"Open With...\" context menu list\n");
    ಬರೆ("-add-start-menu");
    ಬರೆ("    Add Vim to the start menu\n");
    ಬರೆ("-install-icons");
    ಬರೆ("    Create icons for gVim executables on the desktop\n");
    ಬರೆ("-create-directories [vim|home]\n");
    ಬರೆ("    Create runtime directories to drop plugins into; in the $VIM\n");
    ಬರೆ("    or $HOME directory\n");
    ಬರೆ("-register-OLE");
    ಬರೆ("    Ignored\n");
    ಬರೆ("\n");
}

/*
 * Setup installation choices based on command line switches
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
command_ಸಾಲು_ಸಿದ್ಧತೆ_choices(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ **argv)
{
    ಪೂರ್ಣಾಂಕ i, j;

    ಗಾಗಿ (i = 1; i < argc; i++)
    {
	ಆದರೆ (strcmp(argv[i], "-create-batfiles") == 0)
	{
	    ಆದರೆ (i + 1 == argc)
		ಮುಂದುವರಿ;
	    ಆಗ (argv[i + 1][0] != '-' && i < argc)
	    {
		i++;
		ಗಾಗಿ (j = 1; j < ಗುರಿ_ಎಣಿಕೆ; ++j)
		    ಆದರೆ ((targets[j].exenamearg[0] == 'g' ? has_gvim : has_vim)
			    && strcmp(argv[i], targets[j].name) == 0)
		    {
			init_bat_choice(j);
			ನಿಲ್ಲಿಸು;
		    }
		ಆದರೆ (j == ಗುರಿ_ಎಣಿಕೆ)
		    ಬರೆ("%s is not a valid choice for -create-batfiles\n",
								     argv[i]);

		ಆದರೆ (i + 1 == argc)
		    ನಿಲ್ಲಿಸು;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(argv[i], "-create-vimrc") == 0)
	{
	    // Setup default vimrc choices.  If there is already a _vimrc file,
	    // it will NOT be overwritten.
	    init_vimrc_choices();
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(argv[i], "-vimrc-remap") == 0)
	{
	    ಆದರೆ (i + 1 == argc)
		ನಿಲ್ಲಿಸು;
	    i++;
	    ಆದರೆ (strcmp(argv[i], "no") == 0)
		remap_choice = remap_no;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(argv[i], "win") == 0)
		remap_choice = remap_win;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(argv[i], "-vimrc-behave") == 0)
	{
	    ಆದರೆ (i + 1 == argc)
		ನಿಲ್ಲಿಸು;
	    i++;
	    ಆದರೆ (strcmp(argv[i], "unix") == 0)
		mouse_choice = mouse_xterm;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(argv[i], "mswin") == 0)
		mouse_choice = mouse_mswin;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(argv[i], "default") == 0)
		mouse_choice = mouse_default;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(argv[i], "-vimrc-compat") == 0)
	{
	    ಆದರೆ (i + 1 == argc)
		ನಿಲ್ಲಿಸು;
	    i++;
	    ಆದರೆ (strcmp(argv[i], "vi") == 0)
		compat_choice = compat_vi;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(argv[i], "vim") == 0)
		compat_choice = compat_vim;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(argv[i], "defaults") == 0)
		compat_choice = compat_some_enhancements;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(argv[i], "all") == 0)
		compat_choice = compat_all_enhancements;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(argv[i], "-install-popup") == 0)
	{
	    init_popup_choice();
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(argv[i], "-install-openwith") == 0)
	{
	    init_openwith_choice();
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(argv[i], "-add-start-menu") == 0)
	{
	    init_startmenu_choice();
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(argv[i], "-install-icons") == 0)
	{
	    init_shortcut_choices();
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(argv[i], "-create-directories") == 0)
	{
	    ಪೂರ್ಣಾಂಕ vimfiles_dir_choice = (ಪೂರ್ಣಾಂಕ)vimfiles_dir_none;

	    init_directories_choice();
	    ಆದರೆ (i + 1 < argc && argv[i + 1][0] != '-')
	    {
		i++;
		ಆದರೆ (strcmp(argv[i], "vim") == 0)
		    vimfiles_dir_choice = (ಪೂರ್ಣಾಂಕ)vimfiles_dir_vim;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(argv[i], "home") == 0)
		{
		    ಆದರೆ (homedir == ಖಾಲಿ)  // No $HOME in environment
			vimfiles_dir_choice = (ಪೂರ್ಣಾಂಕ)vimfiles_dir_none;
		    ಇಲ್ಲದಿದ್ದರೆ
			vimfiles_dir_choice = (ಪೂರ್ಣಾಂಕ)vimfiles_dir_home;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಬರೆ("Unknown argument for -create-directories: %s\n",
								     argv[i]);
		    print_cmd_ಸಾಲು_help();
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ // No choice specified, default to vim directory
		vimfiles_dir_choice = (ಪೂರ್ಣಾಂಕ)vimfiles_dir_vim;
	    choices[choice_ಎಣಿಕೆ - 1].arg = vimfiles_dir_choice;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(argv[i], "-register-OLE") == 0)
	{
	    // This is always done when gvim is found
	}
	ಇಲ್ಲದಿದ್ದರೆ // Unknown switch
	{
	    ಬರೆ("Got unknown argument argv[%d] = %s\n", i, argv[i]);
	    print_cmd_ಸಾಲು_help();
	}
    }
}


/*
 * Show a few screens full of helpful information.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
show_help(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ ಅಕ್ಷರ *(ವಸ್ತುಗಳು[]) =
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
ಖಾಲಿ
    };
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		c;

    rewind(stdin);
    ಬರೆ("\n");
    ಗಾಗಿ (i = 0; ವಸ್ತುಗಳು[i] != ಖಾಲಿ; ++i)
    {
	puts(ವಸ್ತುಗಳು[i]);
	ಬರೆ("Hit Enter to continue, b (back) or q (quit help): ");
	c = getchar();
	rewind(stdin);
	ಆದರೆ (c == 'b' || c == 'B')
	{
	    ಆದರೆ (i == 0)
		--i;
	    ಇಲ್ಲದಿದ್ದರೆ
		i -= 2;
	}
	ಆದರೆ (c == 'q' || c == 'Q')
	    ನಿಲ್ಲಿಸು;
	ಬರೆ("\n");
    }
}

/*
 * Install the choices.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
install(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		i;

    // Install the selected choices.
    ಗಾಗಿ (i = 0; i < choice_ಎಣಿಕೆ; ++i)
	ಆದರೆ (choices[i].installfunc != ಖಾಲಿ && choices[i].ಸಕ್ರಿಯ)
	    (choices[i].installfunc)(i);

    // Add some entries to the registry, if needed.
    ಆದರೆ (install_popup
	    || install_openwith
	    || (need_uninstall_ನಮೂದು && interactive)
	    || !interactive)
	install_registry();

    // Register gvim with OLE.
    ಆದರೆ (has_gvim)
	install_OLE_ರಿಜಿಸ್ಟರ್();
}

/*
 * request_choice
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
request_choice(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		      i;

    ಬರೆ("\n\nInstall will do for you:\n");
    ಗಾಗಿ (i = 0; i < choice_ಎಣಿಕೆ; ++i)
	ಆದರೆ (choices[i].ಸಕ್ರಿಯ)
	    ಬರೆ("%2d  %s\n", i + 1, choices[i].ಪಠ್ಯ);
    ಬರೆ("To change an item, enter its number\n\n");
    ಬರೆ("Enter item number, h (help), d (do it) or q (quit): ");
}

    ಪೂರ್ಣಾಂಕ
ಮುಖ್ಯ(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ **argv)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಅಕ್ಷರ	buf[BUFSIZE];

    /*
     * Run interactively if there are no command line arguments.
     */
    ಆದರೆ (argc > 1)
	interactive = 0;
    ಇಲ್ಲದಿದ್ದರೆ
	interactive = 1;

    // Initialize this program.
    do_inits(argv);
    init_homedir();

    ಆದರೆ (argc > 1 && strcmp(argv[1], "-uninstall-check") == 0)
    {
	// Only check for already installed Vims.  Used by NSIS installer.
	i = uninstall_check(1);

	// Find the value of $VIM, because NSIS isn't able to do this by
	// itself.
	get_vim_env();

	// When nothing found exit quietly.  If something found wait for
	// a little while, so that the user can read the messages.
	ಆದರೆ (i && _isatty(1))
	    sleep(3);
	ನಿರ್ಗಮಿಸು(0);
    }

    ಬರೆ("This program sets up the installation of Vim "
						   VIM_ಆವೃತ್ತಿ_MEDIUM "\n\n");

    // Check if the user unpacked the archives properly.
    check_unpack();

    // Check for already installed Vims.
    ಆದರೆ (interactive)
	uninstall_check(0);

    // Find out information about the system.
    inspect_system();

    ಆದರೆ (interactive)
    {
	// Setup all the choices.
	ಸಿದ್ಧತೆ_choices();

	// Let the user change choices and finally install (or quit).
	ಗಾಗಿ (;;)
	{
	    request_choice();
	    rewind(stdin);
	    ಆದರೆ (scanf("%99s", buf) == 1)
	    {
		ಆದರೆ (ಅಂಕಿಯೇ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)buf[0]))
		{
		    // Change a choice.
		    i = atoi(buf);
		    ಆದರೆ (i > 0 && i <= choice_ಎಣಿಕೆ && choices[i - 1].ಸಕ್ರಿಯ)
			(choices[i - 1].changefunc)(i - 1);
		    ಇಲ್ಲದಿದ್ದರೆ
			ಬರೆ("\nIllegal choice\n");
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (buf[0] == 'h' || buf[0] == 'H')
		{
		    // Help
		    show_help();
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (buf[0] == 'd' || buf[0] == 'D')
		{
		    // Install!
		    install();
		    ಬರೆ("\nThat finishes the installation.  Happy Vimming!\n");
		    ನಿಲ್ಲಿಸು;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (buf[0] == 'q' || buf[0] == 'Q')
		{
		    // Quit
		    ಬರೆ("\nExiting without anything done\n");
		    ನಿಲ್ಲಿಸು;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    ಬರೆ("\nIllegal choice\n");
	    }
	}
	ಬರೆ("\n");
	myexit(0);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	/*
	 * Run non-interactive - setup according to the command line switches
	 */
	command_ಸಾಲು_ಸಿದ್ಧತೆ_choices(argc, argv);
	install();

	// Avoid that the user has to hit Enter, just wait a little bit to
	// allow reading the messages.
	sleep(2);
    }

    ಹಿಂತಿರುಗಿ 0;
}
