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
    करा { \
	जर (code != ERROR_SUCCESS) \
	{ \
	    म_लिखो("%ld error number:  %ld\n", (दीर्घ)__LINE__, (दीर्घ)code); \
	    परत 1; \
	} \
    } पर्यंत (0)

पूर्णांक	has_vim = 0;		// installable vim.exe exists
पूर्णांक	has_gvim = 0;		// installable gvim.exe exists

अक्षर	oldvimrc[BUFSIZE];	// name of existing vimrc file
अक्षर	vimrc[BUFSIZE];		// name of vimrc file to create

अक्षर	*default_bat_dir = रिक्त;  // when not NULL, use this as the default
				  // directory to write .bat files in
अक्षर	*default_vim_dir = रिक्त;  // when not NULL, use this as the default
				  // install dir for NSIS

/*
 * Structure used for each choice the user can make.
 */
संरचना choice
{
    पूर्णांक	    active;			// non-zero when choice is active
    अक्षर    *text;			// text displayed for this choice
    शून्य    (*changefunc)(पूर्णांक idx);	// function to change this choice
    पूर्णांक	    arg;			// argument for function
    शून्य    (*installfunc)(पूर्णांक idx);	// function to install this choice
};

संरचना choice	choices[30];		// choices the user can make
पूर्णांक		choice_मोजणी = 0;	// number of choices available

#define TABLE_SIZE(s)	(पूर्णांक)ARRAYSIZE(s)

गणना
{
    compat_vi = 1,
    compat_vim,
    compat_some_enhancements,
    compat_all_enhancements
};
अक्षर	*(compat_choices[]) =
{
    "\nChoose the default way to run Vim:",
    "Vi compatible",
    "Vim default",
    "with some Vim enhancements",
    "with syntax highlighting and other features switched on",
};
पूर्णांक	compat_choice = (पूर्णांक)compat_all_enhancements;
अक्षर	*compat_text = "- run Vim %s";

गणना
{
    remap_no = 1,
    remap_win
};
अक्षर	*(remap_choices[]) =
{
    "\nChoose:",
    "Do not remap keys for Windows behavior",
    "Remap a few keys for Windows behavior (CTRL-V, CTRL-C, CTRL-F, etc)",
};
पूर्णांक	remap_choice = (पूर्णांक)remap_no;
अक्षर	*remap_text = "- %s";

गणना
{
    mouse_xterm = 1,
    mouse_mswin,
    mouse_default
};
अक्षर	*(mouse_choices[]) =
{
    "\nChoose the way how Vim uses the mouse:",
    "right button extends selection (the Unix way)",
    "right button has a popup menu, left button starts select mode (the Windows way)",
    "right button has a popup menu, left button starts visual mode",
};
पूर्णांक	mouse_choice = (पूर्णांक)mouse_default;
अक्षर	*mouse_text = "- The mouse %s";

गणना
{
    vimfiles_dir_none = 1,
    vimfiles_dir_vim,
    vimfiles_dir_home
};
स्थिर अक्षर *(vimfiles_dir_choices[]) =
{
    "\nCreate plugin directories:",
    "No",
    "In the VIM directory",
    "In your HOME directory",
};

// non-zero when selected to install the popup menu entry.
स्थिर पूर्णांक	install_popup = 0;

// non-zero when selected to install the "Open with" entry.
स्थिर पूर्णांक	install_openwith = 0;

// non-zero when need to add an uninstall entry in the registry
स्थिर पूर्णांक	need_uninstall_entry = 0;

/*
 * Definitions of the directory name (under $VIM) of the vimfiles directory
 * and its subdirectories:
 */
स्थिर अक्षर	*(vimfiles_subdirs[]) =
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
    स्थिर पूर्णांक
get_choice(अक्षर **table, पूर्णांक entries)
{
    पूर्णांक		answer;
    पूर्णांक		idx;
    अक्षर	dummy[100];

    करा
    {
	साठी (idx = 0; idx < entries; ++idx)
	{
	    जर (idx)
		म_लिखो("%2d  ", idx);
	    माला_दो(table[idx]);
	}
	म_लिखो("Choice: ");
	जर (म_पूछो("%d", &answer) != 1)
	{
	    म_पूछो("%99s", dummy);
	    answer = 0;
	}
    }
    पर्यंत (answer < 1 || answer >= entries);

    परत answer;
}

/*
 * Check if the user unpacked the archives properly.
 * Sets "runtimeidx".
 */
    स्थिर शून्य
check_unpack(शून्य)
{
    अक्षर	buf[BUFSIZE];
    खाता	*fd;
    संरचना stat	st;

    // check for presence of the correct version number in installdir[]
    runtimeidx = म_माप(installdir) - म_माप(VIM_VERSION_NODOT);
    जर (runtimeidx <= 0
	    || stricmp(installdir + runtimeidx, VIM_VERSION_NODOT) != 0
	    || (installdir[runtimeidx - 1] != '/'
		&& installdir[runtimeidx - 1] != '\\'))
    {
	म_लिखो("ERROR: Install program not in directory \"%s\"\n",
		VIM_VERSION_NODOT);
	म_लिखो("This program can only work when it is located in its original directory\n");
	myexit(1);
    }

    // check if filetype.vim is present, which means the runtime archive has
    // been unpacked
    प्र_लिखो(buf, "%s\\filetype.vim", installdir);
    जर (stat(buf, &st) < 0)
    {
	म_लिखो("ERROR: Cannot find filetype.vim in \"%s\"\n", installdir);
	म_लिखो("It looks like you did not unpack the runtime archive.\n");
	म_लिखो("You must unpack the runtime archive \"%srt.zip\" before installing.\n",
		VIM_VERSION_NODOT);
	myexit(1);
    }

    // Check if vim.exe or gvim.exe is in the current directory.
    जर ((fd = ख_खोलो("gvim.exe", "r")) != रिक्त)
    {
	ख_बंद(fd);
	has_gvim = 1;
    }
    जर ((fd = ख_खोलो("vim.exe", "r")) != रिक्त)
    {
	ख_बंद(fd);
	has_vim = 1;
    }
    जर (!has_gvim && !has_vim)
    {
	म_लिखो("ERROR: Cannot find any Vim executables in \"%s\"\n\n",
								  installdir);
	myexit(1);
    }
}

/*
 * Compare paths "p[plen]" to "q[qlen]".  Return 0 if they match.
 * Ignores case and differences between '/' and '\'.
 * "plen" and "qlen" can be negative, strlen() is used then.
 */
    स्थिर पूर्णांक
pathcmp(अक्षर *p, पूर्णांक plen, अक्षर *q, पूर्णांक qlen)
{
    पूर्णांक		i;

    जर (plen < 0)
	plen = म_माप(p);
    जर (qlen < 0)
	qlen = म_माप(q);
    साठी (i = 0; ; ++i)
    {
	// End of "p": check if "q" also ends or just has a slash.
	जर (i == plen)
	{
	    जर (i == qlen)  // match
		परत 0;
	    जर (i == qlen - 1 && (q[i] == '\\' || q[i] == '/'))
		परत 0;   // match with trailing slash
	    परत 1;	    // no match
	}

	// End of "q": check if "p" also ends or just has a slash.
	जर (i == qlen)
	{
	    जर (i == plen)  // match
		परत 0;
	    जर (i == plen - 1 && (p[i] == '\\' || p[i] == '/'))
		परत 0;   // match with trailing slash
	    परत 1;	    // no match
	}

	जर (!(mytoupper(p[i]) == mytoupper(q[i])
		|| ((p[i] == '/' || p[i] == '\\')
		    && (q[i] == '/' || q[i] == '\\'))))
	    परत 1;	    // no match
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
findoldfile(अक्षर **destination)
{
    अक्षर	*bp = *destination;
    माप_प्रकार	indir_l = म_माप(installdir);
    अक्षर	*cp;
    अक्षर	*tmpname;
    अक्षर	*farname;

    /*
     * No action needed if exe not found or not in this directory.
     */
    जर (bp == रिक्त || strnicmp(bp, installdir, indir_l) != 0)
	परत;
    cp = bp + indir_l;
    जर (म_अक्षर("/\\", *cp++) == रिक्त
	    || म_अक्षर(cp, '\\') != रिक्त
	    || म_अक्षर(cp, '/') != रिक्त)
	परत;

    tmpname = alloc(म_माप(cp) + 1);
    म_नकल(tmpname, cp);
    tmpname[म_माप(tmpname) - 1] = 'x';	// .exe -> .exx

    जर (access(tmpname, 0) == 0)
    {
	म_लिखो("\nERROR: %s and %s clash.  Remove or rename %s.\n",
	    tmpname, cp, tmpname);
	myexit(1);
    }

    जर (नाम(cp, tmpname) != 0)
    {
	म_लिखो("\nERROR: failed to rename %s to %s: %s\n",
	    cp, tmpname, म_त्रुटि(0));
	myexit(1);
    }

    farname = searchpath_save(cp);

    जर (नाम(tmpname, cp) != 0)
    {
	म_लिखो("\nERROR: failed to rename %s back to %s: %s\n",
	    tmpname, cp, म_त्रुटि(0));
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
find_bat_exe(पूर्णांक check_bat_only)
{
    पूर्णांक		i;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    साठी (i = 1; i < TARGET_मोजणी; ++i)
    {
	targets[i].oldbat = searchpath_save(targets[i].batname);
	जर (!check_bat_only)
	    targets[i].oldexe = searchpath_save(targets[i].exename);

	जर (default_bat_dir == रिक्त && targets[i].oldbat != रिक्त)
	{
	    default_bat_dir = alloc(म_माप(targets[i].oldbat) + 1);
	    म_नकल(default_bat_dir, targets[i].oldbat);
	    remove_tail(default_bat_dir);
	}
	जर (check_bat_only && targets[i].oldbat != रिक्त)
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
    अक्षर	*vim;
    अक्षर	buf[BUFSIZE];
    खाता	*fd;
    अक्षर	fname[BUFSIZE];

    // First get $VIMRUNTIME.  If it's set, remove the tail.
    vim = दो_पर्या("VIMRUNTIME");
    जर (vim != रिक्त && *vim != 0 && म_माप(vim) < आकार(buf))
    {
	म_नकल(buf, vim);
	remove_tail(buf);
	vim = buf;
    }
    अन्यथा
    {
	vim = दो_पर्या("VIM");
	जर (vim == रिक्त || *vim == 0)
	{
	    // Use the directory from an old uninstall entry.
	    जर (default_vim_dir != रिक्त)
		vim = default_vim_dir;
	    अन्यथा
		// Let NSIS know there is no default, it should use
		// $PROGRAMFILES.
		vim = "";
	}
    }

    // NSIS also uses GetTempPath(), thus we should get the same directory
    // name as where NSIS will look for vimini.ini.
    GetTempPath(आकार(fname) - 12, fname);
    add_pathsep(fname);
    म_जोड़ो(fname, "vimini.ini");

    fd = ख_खोलो(fname, "w");
    जर (fd != रिक्त)
    {
	// Make it look like an .ini file, so that NSIS can read it with a
	// ReadINIStr command.
	ख_लिखो(fd, "[vimini]\n");
	ख_लिखो(fd, "dir=\"%s\"\n", vim);
	ख_बंद(fd);
    }
    अन्यथा
    {
	म_लिखो("Failed to open %s\n", fname);
	sleep(2);
    }
}

स्थिर पूर्णांक अंक_windows;

/*
 * Callback used for EnumWindows():
 * Count the window if the title looks like it is for the uninstaller.
 */
//ARGSUSED
    स्थिर BOOL CALLBACK
window_cb(HWND hwnd, LPARAM lparam UNUSED)
{
    अक्षर title[256];

    title[0] = 0;
    GetWindowText(hwnd, title, 256);
    जर (म_माला(title, "Vim ") != रिक्त && म_माला(title, " Uninstall") != रिक्त)
	++अंक_windows;
    परत TRUE;
}

/*
 * Run the uninstaller silently.
 */
    स्थिर पूर्णांक
run_silent_uninstall(अक्षर *uninst_exe)
{
    अक्षर    vimrt_dir[BUFSIZE];
    अक्षर    temp_uninst[BUFSIZE];
    अक्षर    temp_dir[MAX_PATH];
    अक्षर    buf[BUFSIZE * 2 + 10];
    पूर्णांक	    i;
    DWORD   tick;

    म_नकल(vimrt_dir, uninst_exe);
    remove_tail(vimrt_dir);

    जर (!GetTempPath(आकार(temp_dir), temp_dir))
	परत FAIL;

    // Copy the uninstaller to a temporary exe.
    tick = Get_Tick_मोजणी();
    साठी (i = 0; ; i++)
    {
	प्र_लिखो(temp_uninst, "%s\\vimun%04X.exe", temp_dir,
					  (अचिन्हित पूर्णांक)((i + tick) & 0xFFFF));
	जर (CopyFile(uninst_exe, temp_uninst, TRUE))
	    थांबा;
	जर (GetLastError() != ERROR_FILE_EXISTS)
	    परत FAIL;
	जर (i == 65535)
	    परत FAIL;
    }

    // Run the copied uninstaller silently.
    जर (म_अक्षर(temp_uninst, ' ') != रिक्त)
	प्र_लिखो(buf, "\"%s\" /S _?=%s", temp_uninst, vimrt_dir);
    अन्यथा
	प्र_लिखो(buf, "%s /S _?=%s", temp_uninst, vimrt_dir);
    run_command(buf);

    DeleteFile(temp_uninst);
    परत OK;
}

/*
 * Check for already installed Vims.
 * Return non-zero when found one.
 */
    स्थिर पूर्णांक
uninstall_check(पूर्णांक skip_question)
{
    HKEY	किल्ली_handle;
    HKEY	uninstall_किल्ली_handle;
    अक्षर	*uninstall_किल्ली = "software\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
    अक्षर	subkey_name_buff[BUFSIZE];
    अक्षर	temp_string_buffer[BUFSIZE-2];
    DWORD	local_bufsize;
    FILETIME	temp_pfiletime;
    DWORD	किल्ली_index;
    अक्षर	आदान;
    दीर्घ	code;
    DWORD	मूल्य_type;
    DWORD	orig_अंक_किल्ल्या;
    DWORD	new_अंक_किल्ल्या;
    DWORD	allow_silent;
    पूर्णांक		foundone = 0;

    code = Reg_Open_किल्ली_Ex(HKEY_LOCAL_MACHINE, uninstall_किल्ली, 0,
				     किल्ली_WOW64_64_किल्ली | किल्ली_READ, &किल्ली_handle);
    CHECK_REG_ERROR(code);

    किल्ली_index = 0;
    पर्यंत (TRUE)
    {
	local_bufsize = आकार(subkey_name_buff);
	जर (Reg_Enum_किल्ली_Ex(किल्ली_handle, किल्ली_index, subkey_name_buff, &local_bufsize,
		रिक्त, रिक्त, रिक्त, &temp_pfiletime) == ERROR_NO_MORE_वस्तुसंच)
	    थांबा;

	जर (म_भेदन("Vim", subkey_name_buff, 3) == 0)
	{
	    // Open the key named Vim*
	    code = Reg_Open_किल्ली_Ex(किल्ली_handle, subkey_name_buff, 0,
			   किल्ली_WOW64_64_किल्ली | किल्ली_READ, &uninstall_किल्ली_handle);
	    CHECK_REG_ERROR(code);

	    // get the DisplayName out of it to show the user
	    local_bufsize = आकार(temp_string_buffer);
	    code = Reg_Query_मूल्य_Ex(uninstall_किल्ली_handle, "displayname", 0,
		    &मूल्य_type, (LPBYTE)temp_string_buffer,
		    &local_bufsize);
	    CHECK_REG_ERROR(code);

	    allow_silent = 0;
	    जर (skip_question)
	    {
		DWORD varsize = आकार(DWORD);

		Reg_Query_मूल्य_Ex(uninstall_किल्ली_handle, "AllowSilent", 0,
			&मूल्य_type, (LPBYTE)&allow_silent,
			&varsize);
	    }

	    foundone = 1;
	    म_लिखो("\n*********************************************************\n");
	    म_लिखो("Vim Install found what looks like an existing Vim version.\n");
	    म_लिखो("The name of the entry is:\n");
	    म_लिखो("\n        \"%s\"\n\n", temp_string_buffer);

	    म_लिखो("Installing the new version will disable part of the existing version.\n");
	    म_लिखो("(The batch files used in a console and the \"Edit with Vim\" entry in\n");
	    म_लिखो("the popup menu will use the new version)\n");

	    जर (skip_question)
		म_लिखो("\nRunning uninstall program for \"%s\"\n", temp_string_buffer);
	    अन्यथा
		म_लिखो("\nDo you want to uninstall \"%s\" now?\n(y)es/(n)o)  ", temp_string_buffer);
	    ख_साफ(मानक_निकास);

	    // get the UninstallString
	    local_bufsize = आकार(temp_string_buffer);
	    code = Reg_Query_मूल्य_Ex(uninstall_किल्ली_handle, "uninstallstring", 0,
		    &मूल्य_type, (LPBYTE)temp_string_buffer, &local_bufsize);
	    CHECK_REG_ERROR(code);

	    // Remember the directory, it is used as the default for NSIS.
	    default_vim_dir = alloc(म_माप(temp_string_buffer) + 1);
	    म_नकल(default_vim_dir, temp_string_buffer);
	    remove_tail(default_vim_dir);
	    remove_tail(default_vim_dir);

	    आदान = 'n';
	    करा
	    {
		जर (आदान != 'n')
		    म_लिखो("%c is an invalid reply.  Please enter either 'y' or 'n'\n", आदान);

		जर (skip_question)
		    आदान = 'y';
		अन्यथा
		{
		    शुरुआत(मानक_निवेश);
		    म_पूछो("%c", &आदान);
		}
		निवड (आदान)
		{
		    स्थिती 'y':
		    स्थिती 'Y':
			// save the number of uninstall keys so we can know if
			// it changed
			Reg_Query_Info_किल्ली(किल्ली_handle, रिक्त, रिक्त, रिक्त,
					     &orig_अंक_किल्ल्या, रिक्त, रिक्त, रिक्त,
						      रिक्त, रिक्त, रिक्त, रिक्त);

			// Find existing .bat files before deleting them.
			find_bat_exe(TRUE);

			जर (allow_silent)
			{
			    जर (run_silent_uninstall(temp_string_buffer)
								    == FAIL)
				allow_silent = 0; // Retry with non silent.
			}
			जर (!allow_silent)
			{
			    // Execute the uninstall program.  Put it in double
			    // quotes if there is an embedded space.
			    {
				अक्षर buf[BUFSIZE];

				जर (म_अक्षर(temp_string_buffer, ' ') != रिक्त)
				    प्र_लिखो(buf, "\"%s\"", temp_string_buffer);
				अन्यथा
				    म_नकल(buf, temp_string_buffer);
				run_command(buf);
			    }

			    // Count the number of windows with a title that
			    // match the installer, so that we can check when
			    // it's done.  The uninstaller copies itself,
			    // executes the copy and exits, thus we can't wait
			    // for the process to finish.
			    sleep(1);  // wait for uninstaller to start up
			    अंक_windows = 0;
			    EnumWindows(window_cb, 0);
			    जर (अंक_windows == 0)
			    {
				// Did not find the uninstaller, ask user to
				// press Enter when done. Just in case.
				म_लिखो("Press Enter when the uninstaller is finished\n");
				शुरुआत(मानक_निवेश);
				(शून्य)अक्षर_लो();
			    }
			    अन्यथा
			    {
				म_लिखो("Waiting for the uninstaller to finish (press CTRL-C to abort).");
				करा
				{
				    म_लिखो(".");
				    ख_साफ(मानक_निकास);
				    sleep(1);	// wait for the uninstaller to
						// finish
				    अंक_windows = 0;
				    EnumWindows(window_cb, 0);
				} पर्यंत (अंक_windows > 0);
			    }
			}
			म_लिखो("\nDone!\n");

			// Check if an uninstall reg key was deleted.
			// if it was, we want to decrement key_index.
			// if we don't do this, we will skip the key
			// immediately after any key that we delete.
			Reg_Query_Info_किल्ली(किल्ली_handle, रिक्त, रिक्त, रिक्त,
					      &new_अंक_किल्ल्या, रिक्त, रिक्त, रिक्त,
						      रिक्त, रिक्त, रिक्त, रिक्त);
			जर (new_अंक_किल्ल्या < orig_अंक_किल्ल्या)
			    किल्ली_index--;

			आदान = 'y';
			थांबा;

		    स्थिती 'n':
		    स्थिती 'N':
			// Do not uninstall
			आदान = 'n';
			थांबा;

		    पूर्वनिर्धारित: // just drop through and redo the loop
			थांबा;
		}

	    } पर्यंत (आदान != 'n' && आदान != 'y');

	    Reg_Close_किल्ली(uninstall_किल्ली_handle);
	}

	किल्ली_index++;
    }
    Reg_Close_किल्ली(किल्ली_handle);

    परत foundone;
}

/*
 * Find out information about the system.
 */
    स्थिर शून्य
inspect_system(शून्य)
{
    अक्षर	*p;
    अक्षर	buf[BUFSIZE];
    खाता	*fd;
    पूर्णांक		i;
    पूर्णांक		foundone;

    // This may take a little while, let the user know what we're doing.
    म_लिखो("Inspecting system...\n");

    /*
     * If $VIM is set, check that it's pointing to our directory.
     */
    p = दो_पर्या("VIM");
    जर (p != रिक्त && pathcmp(p, -1, installdir, runtimeidx - 1) != 0)
    {
	म_लिखो("------------------------------------------------------\n");
	म_लिखो("$VIM is set to \"%s\".\n", p);
	म_लिखो("This is different from where this version of Vim is:\n");
	म_नकल(buf, installdir);
	*(buf + runtimeidx - 1) = NUL;
	म_लिखो("\"%s\"\n", buf);
	म_लिखो("You must adjust or remove the setting of $VIM,\n");
	जर (interactive)
	{
	    म_लिखो("to be able to use this install program.\n");
	    myexit(1);
	}
	म_लिखो("otherwise Vim WILL NOT WORK properly!\n");
	म_लिखो("------------------------------------------------------\n");
    }

    /*
     * If $VIMRUNTIME is set, check that it's pointing to our runtime directory.
     */
    p = दो_पर्या("VIMRUNTIME");
    जर (p != रिक्त && pathcmp(p, -1, installdir, -1) != 0)
    {
	म_लिखो("------------------------------------------------------\n");
	म_लिखो("$VIMRUNTIME is set to \"%s\".\n", p);
	म_लिखो("This is different from where this version of Vim is:\n");
	म_लिखो("\"%s\"\n", installdir);
	म_लिखो("You must adjust or remove the setting of $VIMRUNTIME,\n");
	जर (interactive)
	{
	    म_लिखो("to be able to use this install program.\n");
	    myexit(1);
	}
	म_लिखो("otherwise Vim WILL NOT WORK properly!\n");
	म_लिखो("------------------------------------------------------\n");
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
    साठी (i = 1; i < TARGET_मोजणी; ++i)
    {
	findoldfile(&(targets[i].oldexe));
	जर (targets[i].oldexe != रिक्त)
	    foundone = 1;
    }

    जर (foundone)
    {
	म_लिखो("Warning: Found Vim executable(s) in your $PATH:\n");
	साठी (i = 1; i < TARGET_मोजणी; ++i)
	    जर (targets[i].oldexe != रिक्त)
		म_लिखो("%s\n", targets[i].oldexe);
	म_लिखो("It will be used instead of the version you are installing.\n");
	म_लिखो("Please delete or rename it, or adjust your $PATH setting.\n");
    }

    /*
     * Check if there is an existing ../_vimrc or ../.vimrc file.
     */
    म_नकल(oldvimrc, installdir);
    म_नकल(oldvimrc + runtimeidx, "_vimrc");
    जर ((fd = ख_खोलो(oldvimrc, "r")) == रिक्त)
    {
	म_नकल(oldvimrc + runtimeidx, "vimrc~1"); // short version of .vimrc
	जर ((fd = ख_खोलो(oldvimrc, "r")) == रिक्त)
	{
	    म_नकल(oldvimrc + runtimeidx, ".vimrc");
	    fd = ख_खोलो(oldvimrc, "r");
	}
    }
    जर (fd != रिक्त)
	ख_बंद(fd);
    अन्यथा
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
    choices[choice_मोजणी].installfunc = रिक्त;
    choices[choice_मोजणी].active = 0;
    choices[choice_मोजणी].changefunc = रिक्त;
    choices[choice_मोजणी].text = रिक्त;
    choices[choice_मोजणी].arg = 0;
    ++choice_मोजणी;
}

////////////////////////////////////////////////
// stuff for creating the batch files.

/*
 * Install the vim.bat, gvim.bat, etc. files.
 */
    स्थिर शून्य
install_bat_choice(पूर्णांक idx)
{
    अक्षर	*batpath = targets[choices[idx].arg].batpath;
    अक्षर	*oldname = targets[choices[idx].arg].oldbat;
    अक्षर	*exename = targets[choices[idx].arg].exenamearg;
    अक्षर	*vimarg = targets[choices[idx].arg].exearg;
    खाता	*fd;

    जर (*batpath == NUL)
	परत;

    fd = ख_खोलो(batpath, "w");
    जर (fd == रिक्त)
    {
	म_लिखो("\nERROR: Cannot open \"%s\" for writing.\n", batpath);
	परत;
    }

    need_uninstall_entry = 1;

    ख_लिखो(fd, "@echo off\n");
    ख_लिखो(fd, "rem -- Run Vim --\n");
    ख_लिखो(fd, VIMBAT_UNINSTKEY "\n");
    ख_लिखो(fd, "\n");
    ख_लिखो(fd, "setlocal\n");

    /*
     * Don't use double quotes for the "set" argument, also when it
     * contains a space.  The quotes would be included in the value.
     * The order of preference is:
     * 1. $VIMRUNTIME/vim.exe	    (user preference)
     * 2. $VIM/vim81/vim.exe	    (hard coded version)
     * 3. installdir/vim.exe	    (hard coded install directory)
     */
    ख_लिखो(fd, "set VIM_EXE_DIR=%s\n", installdir);
    ख_लिखो(fd, "if exist \"%%VIM%%\\%s\\%s\" set VIM_EXE_DIR=%%VIM%%\\%s\n",
	    VIM_VERSION_NODOT, exename, VIM_VERSION_NODOT);
    ख_लिखो(fd, "if exist \"%%VIMRUNTIME%%\\%s\" set VIM_EXE_DIR=%%VIMRUNTIME%%\n", exename);
    ख_लिखो(fd, "\n");

    // Give an error message when the executable could not be found.
    ख_लिखो(fd, "if not exist \"%%VIM_EXE_DIR%%\\%s\" (\n", exename);
    ख_लिखो(fd, "    echo \"%%VIM_EXE_DIR%%\\%s\" not found\n", exename);
    ख_लिखो(fd, "    goto :eof\n");
    ख_लिखो(fd, ")\n");
    ख_लिखो(fd, "\n");

    जर (*exename == 'g')
    {
	ख_लिखो(fd, "rem check --nofork argument\n");
	ख_लिखो(fd, "set VIMNOFORK=\n");
	ख_लिखो(fd, ":loopstart\n");
	ख_लिखो(fd, "if .%%1==. goto loopend\n");
	ख_लिखो(fd, "if .%%1==.--nofork (\n");
	ख_लिखो(fd, "    set VIMNOFORK=1\n");
	ख_लिखो(fd, ") else if .%%1==.-f (\n");
	ख_लिखो(fd, "    set VIMNOFORK=1\n");
	ख_लिखो(fd, ")\n");
	ख_लिखो(fd, "shift\n");
	ख_लिखो(fd, "goto loopstart\n");
	ख_लिखो(fd, ":loopend\n");
	ख_लिखो(fd, "\n");
    }

    जर (*exename == 'g')
    {
	// For gvim.exe use "start /b" to avoid that the console window
	// stays open.
	ख_लिखो(fd, "if .%%VIMNOFORK%%==.1 (\n");
	ख_लिखो(fd, "    start \"dummy\" /b /wait ");
	// Always use quotes, $VIM or $VIMRUNTIME might have a space.
	ख_लिखो(fd, "\"%%VIM_EXE_DIR%%\\%s\" %s %%*\n",
		exename, vimarg);
	ख_लिखो(fd, ") else (\n");
	ख_लिखो(fd, "    start \"dummy\" /b ");
	// Always use quotes, $VIM or $VIMRUNTIME might have a space.
	ख_लिखो(fd, "\"%%VIM_EXE_DIR%%\\%s\" %s %%*\n",
		exename, vimarg);
	ख_लिखो(fd, ")\n");
    }
    अन्यथा
    {
	// Always use quotes, $VIM or $VIMRUNTIME might have a space.
	ख_लिखो(fd, "\"%%VIM_EXE_DIR%%\\%s\" %s %%*\n",
		exename, vimarg);
    }

    ख_बंद(fd);
    म_लिखो("%s has been %s\n", batpath,
	    oldname == रिक्त ? "created" : "overwritten");
}

/*
 * Make the text string for choice "idx".
 * The format "fmt" is must have one %s item, which "arg" is used for.
 */
    स्थिर शून्य
alloc_text(पूर्णांक idx, अक्षर *fmt, अक्षर *arg)
{
    जर (choices[idx].text != रिक्त)
	मुक्त(choices[idx].text);

    choices[idx].text = alloc(म_माप(fmt) + म_माप(arg) - 1);
    प्र_लिखो(choices[idx].text, fmt, arg);
}

/*
 * Toggle the "Overwrite .../vim.bat" to "Don't overwrite".
 */
    स्थिर शून्य
toggle_bat_choice(पूर्णांक idx)
{
    अक्षर	*batname = targets[choices[idx].arg].batpath;
    अक्षर	*oldname = targets[choices[idx].arg].oldbat;

    जर (*batname == NUL)
    {
	alloc_text(idx, "    Overwrite %s", oldname);
	म_नकल(batname, oldname);
    }
    अन्यथा
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
set_bat_text(पूर्णांक idx, अक्षर *batpath, अक्षर *name)
{
    म_जोड़ो(batpath, name);

    alloc_text(idx, "    Create %s", batpath);
}

/*
 * Select a directory to write the batch file line.
 */
    स्थिर शून्य
change_bat_choice(पूर्णांक idx)
{
    अक्षर	*path;
    अक्षर	*batpath;
    अक्षर	*name;
    पूर्णांक		n;
    अक्षर	*s;
    अक्षर	*p;
    पूर्णांक		मोजणी;
    अक्षर	**names = रिक्त;
    पूर्णांक		i;
    पूर्णांक		target = choices[idx].arg;

    name = targets[target].batname;
    batpath = targets[target].batpath;

    path = दो_पर्या("PATH");
    जर (path == रिक्त)
    {
	म_लिखो("\nERROR: The variable $PATH is not set\n");
	परत;
    }

    /*
     * first round: count number of names in path;
     * second round: save names to names[].
     */
    साठी (;;)
    {
	मोजणी = 1;
	साठी (p = path; *p; )
	{
	    s = म_अक्षर(p, ';');
	    जर (s == रिक्त)
		s = p + म_माप(p);
	    जर (names != रिक्त)
	    {
		names[मोजणी] = alloc(s - p + 1);
		म_नकलन(names[मोजणी], p, s - p);
		names[मोजणी][s - p] = NUL;
	    }
	    ++मोजणी;
	    p = s;
	    जर (*p != NUL)
		++p;
	}
	जर (names != रिक्त)
	    थांबा;
	names = alloc((मोजणी + 1) * आकार(अक्षर *));
    }
    names[0] = alloc(50);
    प्र_लिखो(names[0], "Select directory to create %s in:", name);
    names[मोजणी] = alloc(50);
    जर (choices[idx].arg == 0)
	प्र_लिखो(names[मोजणी], "Do not create any .bat file.");
    अन्यथा
	प्र_लिखो(names[मोजणी], "Do not create a %s file.", name);
    n = get_choice(names, मोजणी + 1);

    जर (n == मोजणी)
    {
	// Selected last item, don't create bat file.
	*batpath = NUL;
	जर (choices[idx].arg != 0)
	    alloc_text(idx, "    Do NOT create %s", name);
    }
    अन्यथा
    {
	// Selected one of the paths.  For the first item only keep the path,
	// for the others append the batch file name.
	म_नकल(batpath, names[n]);
	add_pathsep(batpath);
	जर (choices[idx].arg != 0)
	    set_bat_text(idx, batpath, name);
    }

    साठी (i = 0; i <= मोजणी; ++i)
	मुक्त(names[i]);
    मुक्त(names);
}

अक्षर *bat_text_yes = "Install .bat files to use Vim at the command line:";
अक्षर *bat_text_no = "do NOT install .bat files to use Vim at the command line";

    स्थिर शून्य
change_main_bat_choice(पूर्णांक idx)
{
    पूर्णांक		i;

    // let the user select a default directory or NONE
    change_bat_choice(idx);

    जर (targets[0].batpath[0] != NUL)
	choices[idx].text = bat_text_yes;
    अन्यथा
	choices[idx].text = bat_text_no;

    // update the individual batch file selections
    साठी (i = 1; i < TARGET_मोजणी; ++i)
    {
	// Only make it active when the first item has a path and the vim.exe
	// or gvim.exe exists (there is a changefunc then).
	जर (targets[0].batpath[0] != NUL
		&& choices[idx + i].changefunc != रिक्त)
	{
	    choices[idx + i].active = 1;
	    जर (choices[idx + i].changefunc == change_bat_choice
		    && targets[i].batpath[0] != NUL)
	    {
		म_नकल(targets[i].batpath, targets[0].batpath);
		set_bat_text(idx + i, targets[i].batpath, targets[i].batname);
	    }
	}
	अन्यथा
	    choices[idx + i].active = 0;
    }
}

/*
 * Initialize a choice for creating a batch file.
 */
    स्थिर शून्य
init_bat_choice(पूर्णांक target)
{
    अक्षर	*batpath = targets[target].batpath;
    अक्षर	*oldbat = targets[target].oldbat;
    अक्षर	*p;
    पूर्णांक		i;

    choices[choice_मोजणी].arg = target;
    choices[choice_मोजणी].installfunc = install_bat_choice;
    choices[choice_मोजणी].active = 1;
    choices[choice_मोजणी].text = रिक्त;	// will be set below
    जर (oldbat != रिक्त)
    {
	// A [g]vim.bat exists: Only choice is to overwrite it or not.
	choices[choice_मोजणी].changefunc = toggle_bat_choice;
	*batpath = NUL;
	toggle_bat_choice(choice_मोजणी);
    }
    अन्यथा
    {
	जर (default_bat_dir != रिक्त)
	    // Prefer using the same path as an existing .bat file.
	    म_नकल(batpath, default_bat_dir);
	अन्यथा
	{
	    // No [g]vim.bat exists: Write it to a directory in $PATH.  Use
	    // $WINDIR by default, if it's empty the first item in $PATH.
	    p = दो_पर्या("WINDIR");
	    जर (p != रिक्त && *p != NUL)
		म_नकल(batpath, p);
	    अन्यथा
	    {
		p = दो_पर्या("PATH");
		जर (p == रिक्त || *p == NUL)		// "cannot happen"
		    म_नकल(batpath, "C:/Windows");
		अन्यथा
		{
		    i = 0;
		    पर्यंत (*p != NUL && *p != ';')
			batpath[i++] = *p++;
		    batpath[i] = NUL;
		}
	    }
	}
	add_pathsep(batpath);
	set_bat_text(choice_मोजणी, batpath, targets[target].batname);

	choices[choice_मोजणी].changefunc = change_bat_choice;
    }
    ++choice_मोजणी;
}

/*
 * Set up the choices for installing .bat files.
 * For these items "arg" is the index in targets[].
 */
    स्थिर शून्य
init_bat_choices(शून्य)
{
    पूर्णांक		i;

    // The first item is used to switch installing batch files on/off and
    // setting the default path.
    choices[choice_मोजणी].text = bat_text_yes;
    choices[choice_मोजणी].changefunc = change_main_bat_choice;
    choices[choice_मोजणी].installfunc = रिक्त;
    choices[choice_मोजणी].active = 1;
    choices[choice_मोजणी].arg = 0;
    ++choice_मोजणी;

    // Add items for each batch file target.  Only used when not disabled by
    // the first item.  When a .exe exists, don't offer to create a .bat.
    साठी (i = 1; i < TARGET_मोजणी; ++i)
	जर (targets[i].oldexe == रिक्त
		&& (targets[i].exenamearg[0] == 'g' ? has_gvim : has_vim))
	    init_bat_choice(i);
	अन्यथा
	    add_dummy_choice();
}

/*
 * Install the vimrc file.
 */
    स्थिर शून्य
install_vimrc(पूर्णांक idx UNUSED)
{
    खाता	*fd, *tfd;
    अक्षर	*fname;

    // If an old vimrc file exists, overwrite it.
    // Otherwise create a new one.
    जर (*oldvimrc != NUL)
	fname = oldvimrc;
    अन्यथा
	fname = vimrc;

    fd = ख_खोलो(fname, "w");
    जर (fd == रिक्त)
    {
	म_लिखो("\nERROR: Cannot open \"%s\" for writing.\n", fname);
	परत;
    }
    निवड (compat_choice)
    {
	स्थिती compat_vi:
		    ख_लिखो(fd, "\" Vi compatible\n");
		    ख_लिखो(fd, "set compatible\n");
		    थांबा;
	स्थिती compat_vim:
		    ख_लिखो(fd, "\" Vim's default behavior\n");
		    ख_लिखो(fd, "if &compatible\n");
		    ख_लिखो(fd, "  set nocompatible\n");
		    ख_लिखो(fd, "endif\n");
		    थांबा;
	स्थिती compat_some_enhancements:
		    ख_लिखो(fd, "\" Vim with some enhancements\n");
		    ख_लिखो(fd, "source $VIMRUNTIME/defaults.vim\n");
		    थांबा;
	स्थिती compat_all_enhancements:
		    ख_लिखो(fd, "\" Vim with all enhancements\n");
		    ख_लिखो(fd, "source $VIMRUNTIME/vimrc_example.vim\n");
		    थांबा;
    }
    निवड (remap_choice)
    {
	स्थिती remap_no:
		    थांबा;
	स्थिती remap_win:
		    ख_लिखो(fd, "\n");
		    ख_लिखो(fd, "\" Remap a few keys for Windows behavior\n");
		    ख_लिखो(fd, "source $VIMRUNTIME/mswin.vim\n");
		    थांबा;
    }
    निवड (mouse_choice)
    {
	स्थिती mouse_xterm:
		    ख_लिखो(fd, "\n");
		    ख_लिखो(fd, "\" Mouse behavior (the Unix way)\n");
		    ख_लिखो(fd, "behave xterm\n");
		    थांबा;
	स्थिती mouse_mswin:
		    ख_लिखो(fd, "\n");
		    ख_लिखो(fd, "\" Mouse behavior (the Windows way)\n");
		    ख_लिखो(fd, "behave mswin\n");
		    थांबा;
	स्थिती mouse_default:
		    थांबा;
    }
    जर ((tfd = ख_खोलो("diff.exe", "r")) != रिक्त)
    {
	// Use the diff.exe that comes with the self-extracting gvim.exe.
	ख_बंद(tfd);
	ख_लिखो(fd, "\n");
	ख_लिखो(fd, "\" Use the internal diff if available.\n");
	ख_लिखो(fd, "\" Otherwise use the special 'diffexpr' for Windows.\n");
	ख_लिखो(fd, "if &diffopt !~# 'internal'\n");
	ख_लिखो(fd, "  set diffexpr=MyDiff()\n");
	ख_लिखो(fd, "endif\n");
	ख_लिखो(fd, "function MyDiff()\n");
	ख_लिखो(fd, "  let opt = '-a --binary '\n");
	ख_लिखो(fd, "  if &diffopt =~ 'icase' | let opt = opt . '-i ' | endif\n");
	ख_लिखो(fd, "  if &diffopt =~ 'iwhite' | let opt = opt . '-b ' | endif\n");
	// Use quotes only when needed, they may cause trouble.
	// Always escape "!".
	ख_लिखो(fd, "  let arg1 = v:fname_in\n");
	ख_लिखो(fd, "  if arg1 =~ ' ' | let arg1 = '\"' . arg1 . '\"' | endif\n");
	ख_लिखो(fd, "  let arg1 = substitute(arg1, '!', '\\!', 'g')\n");
	ख_लिखो(fd, "  let arg2 = v:fname_new\n");
	ख_लिखो(fd, "  if arg2 =~ ' ' | let arg2 = '\"' . arg2 . '\"' | endif\n");
	ख_लिखो(fd, "  let arg2 = substitute(arg2, '!', '\\!', 'g')\n");
	ख_लिखो(fd, "  let arg3 = v:fname_out\n");
	ख_लिखो(fd, "  if arg3 =~ ' ' | let arg3 = '\"' . arg3 . '\"' | endif\n");
	ख_लिखो(fd, "  let arg3 = substitute(arg3, '!', '\\!', 'g')\n");

	// If the path has a space:  When using cmd.exe (Win NT/2000/XP) put
	// quotes around the diff command and rely on the default value of
	// shellxquote to solve the quoting problem for the whole command.
	//
	// Otherwise put a double quote just before the space and at the
	// end of the command.  Putting quotes around the whole thing
	// doesn't work on Win 95/98/ME.  This is mostly guessed!
	ख_लिखो(fd, "  if $VIMRUNTIME =~ ' '\n");
	ख_लिखो(fd, "    if &sh =~ '\\<cmd'\n");
	ख_लिखो(fd, "      if empty(&shellxquote)\n");
	ख_लिखो(fd, "        let l:shxq_sav = ''\n");
	ख_लिखो(fd, "        set shellxquote&\n");
	ख_लिखो(fd, "      endif\n");
	ख_लिखो(fd, "      let cmd = '\"' . $VIMRUNTIME . '\\diff\"'\n");
	ख_लिखो(fd, "    else\n");
	ख_लिखो(fd, "      let cmd = substitute($VIMRUNTIME, ' ', '\" ', '') . '\\diff\"'\n");
	ख_लिखो(fd, "    endif\n");
	ख_लिखो(fd, "  else\n");
	ख_लिखो(fd, "    let cmd = $VIMRUNTIME . '\\diff'\n");
	ख_लिखो(fd, "  endif\n");
	ख_लिखो(fd, "  let cmd = substitute(cmd, '!', '\\!', 'g')\n");
	ख_लिखो(fd, "  silent execute '!' . cmd . ' ' . opt . arg1 . ' ' . arg2 . ' > ' . arg3\n");
	ख_लिखो(fd, "  if exists('l:shxq_sav')\n");
	ख_लिखो(fd, "    let &shellxquote=l:shxq_sav\n");
	ख_लिखो(fd, "  endif\n");
	ख_लिखो(fd, "endfunction\n");
	ख_लिखो(fd, "\n");
    }
    ख_बंद(fd);
    म_लिखो("%s has been written\n", fname);
}

    स्थिर शून्य
change_vimrc_choice(पूर्णांक idx)
{
    जर (choices[idx].installfunc != रिक्त)
    {
	// Switch to NOT change or create a vimrc file.
	जर (*oldvimrc != NUL)
	    alloc_text(idx, "Do NOT change startup file %s", oldvimrc);
	अन्यथा
	    alloc_text(idx, "Do NOT create startup file %s", vimrc);
	choices[idx].installfunc = रिक्त;
	choices[idx + 1].active = 0;
	choices[idx + 2].active = 0;
	choices[idx + 3].active = 0;
    }
    अन्यथा
    {
	// Switch to change or create a vimrc file.
	जर (*oldvimrc != NUL)
	    alloc_text(idx, "Overwrite startup file %s with:", oldvimrc);
	अन्यथा
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
change_run_choice(पूर्णांक idx)
{
    compat_choice = get_choice(compat_choices, TABLE_SIZE(compat_choices));
    alloc_text(idx, compat_text, compat_choices[compat_choice]);
}

/*
 * Change the choice if keys are to be remapped.
 */
    स्थिर शून्य
change_remap_choice(पूर्णांक idx)
{
    remap_choice = get_choice(remap_choices, TABLE_SIZE(remap_choices));
    alloc_text(idx, remap_text, remap_choices[remap_choice]);
}

/*
 * Change the choice how to select text.
 */
    स्थिर शून्य
change_mouse_choice(पूर्णांक idx)
{
    mouse_choice = get_choice(mouse_choices, TABLE_SIZE(mouse_choices));
    alloc_text(idx, mouse_text, mouse_choices[mouse_choice]);
}

    स्थिर शून्य
init_vimrc_choices(शून्य)
{
    // set path for a new _vimrc file (also when not used)
    म_नकल(vimrc, installdir);
    म_नकल(vimrc + runtimeidx, "_vimrc");

    // Set opposite value and then toggle it by calling change_vimrc_choice()
    जर (*oldvimrc == NUL)
	choices[choice_मोजणी].installfunc = रिक्त;
    अन्यथा
	choices[choice_मोजणी].installfunc = install_vimrc;
    choices[choice_मोजणी].text = रिक्त;
    change_vimrc_choice(choice_मोजणी);
    choices[choice_मोजणी].changefunc = change_vimrc_choice;
    choices[choice_मोजणी].active = 1;
    ++choice_मोजणी;

    // default way to run Vim
    alloc_text(choice_मोजणी, compat_text, compat_choices[compat_choice]);
    choices[choice_मोजणी].changefunc = change_run_choice;
    choices[choice_मोजणी].installfunc = रिक्त;
    choices[choice_मोजणी].active = (*oldvimrc == NUL);
    ++choice_मोजणी;

    // Whether to remap keys
    alloc_text(choice_मोजणी, remap_text , remap_choices[remap_choice]);
    choices[choice_मोजणी].changefunc = change_remap_choice;
    choices[choice_मोजणी].installfunc = रिक्त;
    choices[choice_मोजणी].active = (*oldvimrc == NUL);
    ++choice_मोजणी;

    // default way to use the mouse
    alloc_text(choice_मोजणी, mouse_text, mouse_choices[mouse_choice]);
    choices[choice_मोजणी].changefunc = change_mouse_choice;
    choices[choice_मोजणी].installfunc = रिक्त;
    choices[choice_मोजणी].active = (*oldvimrc == NUL);
    ++choice_मोजणी;
}

    स्थिर LONG
reg_create_किल्ली(
    HKEY मूळ,
    स्थिरांक अक्षर *subkey,
    PHKEY ph_किल्ली,
    DWORD flag)
{
    DWORD disp;

    *ph_किल्ली = रिक्त;
    परत Reg_Create_किल्ली_Ex(
		मूळ, subkey,
		0, रिक्त, REG_OPTION_NON_VOLATILE,
		flag | किल्ली_WRITE,
		रिक्त, ph_किल्ली, &disp);
}

    स्थिर LONG
reg_set_string_मूल्य(
    HKEY h_किल्ली,
    स्थिरांक अक्षर *मूल्य_name,
    स्थिरांक अक्षर *data)
{
    परत Reg_Set_मूल्य_Ex(h_किल्ली, मूल्य_name, 0, REG_SZ,
				     (LPBYTE)data, (DWORD)(1 + म_माप(data)));
}

    स्थिर LONG
reg_create_किल्ली_and_मूल्य(
    HKEY h_मूळ_किल्ली,
    स्थिरांक अक्षर *subkey,
    स्थिरांक अक्षर *मूल्य_name,
    स्थिरांक अक्षर *data,
    DWORD flag)
{
    HKEY h_किल्ली;
    LONG lRet = reg_create_किल्ली(h_मूळ_किल्ली, subkey, &h_किल्ली, flag);

    जर (ERROR_SUCCESS == lRet)
    {
	lRet = reg_set_string_मूल्य(h_किल्ली, मूल्य_name, data);
	Reg_Close_किल्ली(h_किल्ली);
    }
    परत lRet;
}

    स्थिर LONG
register_inproc_server(
    HKEY h_मूळ_किल्ली,
    स्थिरांक अक्षर *clsid,
    स्थिरांक अक्षर *extname,
    स्थिरांक अक्षर *module,
    स्थिरांक अक्षर *threading_model,
    DWORD flag)
{
    CHAR subkey[BUFSIZE];
    LONG lRet;

    प्र_लिखो(subkey, "CLSID\\%s", clsid);
    lRet = reg_create_किल्ली_and_मूल्य(h_मूळ_किल्ली, subkey, रिक्त, extname, flag);
    जर (ERROR_SUCCESS == lRet)
    {
	प्र_लिखो(subkey, "CLSID\\%s\\InProcServer32", clsid);
	lRet = reg_create_किल्ली_and_मूल्य(h_मूळ_किल्ली, subkey, रिक्त, module, flag);
	जर (ERROR_SUCCESS == lRet)
	{
	    lRet = reg_create_किल्ली_and_मूल्य(h_मूळ_किल्ली, subkey,
					   "ThreadingModel", threading_model, flag);
	}
    }
    परत lRet;
}

    स्थिर LONG
register_shellex(
    HKEY h_मूळ_किल्ली,
    स्थिरांक अक्षर *clsid,
    स्थिरांक अक्षर *name,
    स्थिरांक अक्षर *exe_path,
    DWORD flag)
{
    LONG lRet = reg_create_किल्ली_and_मूल्य(
	    h_मूळ_किल्ली,
	    "*\\shellex\\ContextMenuHandlers\\gvim",
	    रिक्त,
	    clsid,
	    flag);

    जर (ERROR_SUCCESS == lRet)
    {
	lRet = reg_create_किल्ली_and_मूल्य(
		HKEY_LOCAL_MACHINE,
		"Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved",
		clsid,
		name,
		flag);

	जर (ERROR_SUCCESS == lRet)
	{
	    lRet = reg_create_किल्ली_and_मूल्य(
		    HKEY_LOCAL_MACHINE,
		    "Software\\Vim\\Gvim",
		    "path",
		    exe_path,
		    flag);
	}
    }
    परत lRet;
}

    स्थिर LONG
register_openwith(
    HKEY h_मूळ_किल्ली,
    स्थिरांक अक्षर *exe_path,
    DWORD flag)
{
    अक्षर	exe_cmd[BUFSIZE];
    LONG	lRet;

    प्र_लिखो(exe_cmd, "\"%s\" \"%%1\"", exe_path);
    lRet = reg_create_किल्ली_and_मूल्य(
	    h_मूळ_किल्ली,
	    "Applications\\gvim.exe\\shell\\edit\\command",
	    रिक्त,
	    exe_cmd,
	    flag);

    जर (ERROR_SUCCESS == lRet)
    {
	पूर्णांक i;
	स्थिर स्थिरांक अक्षर *openwith[] = {
		".htm\\OpenWithList\\gvim.exe",
		".vim\\OpenWithList\\gvim.exe",
		"*\\OpenWithList\\gvim.exe",
	};

	साठी (i = 0; ERROR_SUCCESS == lRet && i < (पूर्णांक)ARRAYSIZE(openwith); i++)
	    lRet = reg_create_किल्ली_and_मूल्य(h_मूळ_किल्ली, openwith[i], रिक्त, "", flag);
    }

    परत lRet;
}

    स्थिर LONG
register_uninstall(
    HKEY h_मूळ_किल्ली,
    स्थिरांक अक्षर *appname,
    स्थिरांक अक्षर *display_name,
    स्थिरांक अक्षर *uninstall_string,
    स्थिरांक अक्षर *display_icon,
    स्थिरांक अक्षर *display_version,
    स्थिरांक अक्षर *publisher)
{
    LONG lRet = reg_create_किल्ली_and_मूल्य(h_मूळ_किल्ली, appname,
			     "DisplayName", display_name, किल्ली_WOW64_64_किल्ली);

    जर (ERROR_SUCCESS == lRet)
	lRet = reg_create_किल्ली_and_मूल्य(h_मूळ_किल्ली, appname,
		     "UninstallString", uninstall_string, किल्ली_WOW64_64_किल्ली);
    जर (ERROR_SUCCESS == lRet)
	lRet = reg_create_किल्ली_and_मूल्य(h_मूळ_किल्ली, appname,
		     "DisplayIcon", display_icon, किल्ली_WOW64_64_किल्ली);
    जर (ERROR_SUCCESS == lRet)
	lRet = reg_create_किल्ली_and_मूल्य(h_मूळ_किल्ली, appname,
		     "DisplayVersion", display_version, किल्ली_WOW64_64_किल्ली);
    जर (ERROR_SUCCESS == lRet)
	lRet = reg_create_किल्ली_and_मूल्य(h_मूळ_किल्ली, appname,
		     "Publisher", publisher, किल्ली_WOW64_64_किल्ली);
    परत lRet;
}

/*
 * Add some entries to the registry:
 * - to add "Edit with Vim" to the context * menu
 * - to add Vim to the "Open with..." list
 * - to uninstall Vim
 */
//ARGSUSED
    स्थिर पूर्णांक
install_registry(शून्य)
{
    LONG	lRet = ERROR_SUCCESS;
    स्थिरांक अक्षर	*vim_ext_ThreadingModel = "Apartment";
    स्थिरांक अक्षर	*vim_ext_name = "Vim Shell Extension";
    स्थिरांक अक्षर	*vim_ext_clsid = "{51EEE242-AD87-11d3-9C1E-0090278BBD99}";
    अक्षर	vim_exe_path[MAX_PATH];
    अक्षर	display_name[BUFSIZE];
    अक्षर	uninstall_string[BUFSIZE];
    अक्षर	icon_string[BUFSIZE];
    अक्षर	version_string[BUFSIZE];
    पूर्णांक		i;
    पूर्णांक		loop_मोजणी = is_64bit_os() ? 2 : 1;
    DWORD	flag;

    प्र_लिखो(vim_exe_path, "%s\\gvim.exe", installdir);

    जर (install_popup)
    {
	अक्षर	    bufg[BUFSIZE];

	म_लिखो("Creating \"Edit with Vim\" popup menu entry\n");

	साठी (i = 0; i < loop_मोजणी; i++)
	{
	    जर (i == 0)
	    {
		प्र_लिखो(bufg, "%s\\" GVIMEXT32_PATH, installdir);
		flag = किल्ली_WOW64_32_किल्ली;
	    }
	    अन्यथा
	    {
		प्र_लिखो(bufg, "%s\\" GVIMEXT64_PATH, installdir);
		flag = किल्ली_WOW64_64_किल्ली;
	    }

	    lRet = register_inproc_server(
		    HKEY_CLASSES_मूळ, vim_ext_clsid, vim_ext_name,
		    bufg, vim_ext_ThreadingModel, flag);
	    जर (ERROR_SUCCESS != lRet)
		परत FAIL;
	    lRet = register_shellex(
		    HKEY_CLASSES_मूळ, vim_ext_clsid, vim_ext_name,
		    vim_exe_path, flag);
	    जर (ERROR_SUCCESS != lRet)
		परत FAIL;
	}
    }

    जर (install_openwith)
    {
	म_लिखो("Creating \"Open with ...\" list entry\n");

	साठी (i = 0; i < loop_मोजणी; i++)
	{
	    जर (i == 0)
		flag = किल्ली_WOW64_32_किल्ली;
	    अन्यथा
		flag = किल्ली_WOW64_64_किल्ली;

	    lRet = register_openwith(HKEY_CLASSES_मूळ, vim_exe_path, flag);
	    जर (ERROR_SUCCESS != lRet)
		परत FAIL;
	}
    }

    म_लिखो("Creating an uninstall entry\n");
    प्र_लिखो(display_name, "Vim " VIM_VERSION_SHORT
#ifdef _M_ARM64
	    " (arm64)"
#किंवाजर _M_X64
	    " (x64)"
#endif
	    );

    // For the NSIS installer use the generated uninstaller.
    जर (interactive)
	प्र_लिखो(uninstall_string, "%s\\uninstall.exe", installdir);
    अन्यथा
	प्र_लिखो(uninstall_string, "%s\\uninstall-gui.exe", installdir);

    प्र_लिखो(icon_string, "%s\\gvim.exe,0", installdir);

    प्र_लिखो(version_string, VIM_VERSION_SHORT "." VIM_VERSION_PATCHLEVEL_STR);

    lRet = register_uninstall(
	HKEY_LOCAL_MACHINE,
	"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Vim " VIM_VERSION_SHORT,
	display_name,
	uninstall_string,
	icon_string,
	version_string,
	"The Vim Project");
    जर (ERROR_SUCCESS != lRet)
	परत FAIL;

    परत OK;
}

    स्थिर शून्य
change_popup_choice(पूर्णांक idx)
{
    जर (install_popup == 0)
    {
	choices[idx].text = "Install an entry for Vim in the popup menu for the right\n    mouse button so that you can edit any file with Vim";
	install_popup = 1;
    }
    अन्यथा
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

    जर (has_gvim
	    && (stat(GVIMEXT32_PATH, &st) >= 0
		|| stat(GVIMEXT64_PATH, &st) >= 0))
    {
	choices[choice_मोजणी].changefunc = change_popup_choice;
	choices[choice_मोजणी].installfunc = रिक्त;
	choices[choice_मोजणी].active = 1;
	change_popup_choice(choice_मोजणी);  // set the text
	++choice_मोजणी;
    }
    अन्यथा
	add_dummy_choice();
}

    स्थिर शून्य
change_openwith_choice(पूर्णांक idx)
{
    जर (install_openwith == 0)
    {
	choices[idx].text = "Add Vim to the \"Open With...\" list in the popup menu for the right\n    mouse button so that you can edit any file with Vim";
	install_openwith = 1;
    }
    अन्यथा
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
    जर (has_gvim)
    {
	choices[choice_मोजणी].changefunc = change_openwith_choice;
	choices[choice_मोजणी].installfunc = रिक्त;
	choices[choice_मोजणी].active = 1;
	change_openwith_choice(choice_मोजणी);  // set the text
	++choice_मोजणी;
    }
    अन्यथा
	add_dummy_choice();
}

/*
 * Create a shell link.
 *
 * returns 0 on failure, non-zero on successful completion.
 *
 * NOTE:  Currently untested with mingw.
 */
    पूर्णांक
create_shortcut(
	स्थिरांक अक्षर *shortcut_name,
	स्थिरांक अक्षर *iconfile_path,
	पूर्णांक	    iconindex,
	स्थिरांक अक्षर *shortcut_target,
	स्थिरांक अक्षर *shortcut_args,
	स्थिरांक अक्षर *workingdir
	)
{
    IShellLink	    *shelllink_ptr;
    HRESULT	    hres;
    IPersistFile	    *persistfile_ptr;

    // Initialize COM library
    hres = CoInitialize(रिक्त);
    जर (!SUCCEEDED(hres))
    {
	म_लिखो("Error:  Could not open the COM library.  Not creating shortcut.\n");
	परत FAIL;
    }

    // Instantiate a COM object for the ShellLink, store a pointer to it
    // in shelllink_ptr.
    hres = CoCreateInstance(&CLSID_ShellLink,
			   रिक्त,
			   CLSCTX_INPROC_SERVER,
			   &IID_IShellLink,
			   (शून्य **) &shelllink_ptr);

    जर (SUCCEEDED(hres)) // If the instantiation was successful...
    {
	// ...Then build a PersistFile interface for the ShellLink so we can
	// save it as a file after we build it.
	hres = shelllink_ptr->lpVtbl->QueryInterface(shelllink_ptr,
		&IID_IPersistFile, (शून्य **) &persistfile_ptr);

	जर (SUCCEEDED(hres))
	{
	    ब_अक्षर_प्रकार wsz[BUFSIZE];

	    // translate the (possibly) multibyte shortcut filename to windows
	    // Unicode so it can be used as a file name.
	    MultiByteToWideChar(CP_ACP, 0, shortcut_name, -1, wsz, आकार(wsz)/आकार(wsz[0]));

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
	अन्यथा
	{
	    म_लिखो("QueryInterface Error\n");
	    परत FAIL;
	}

	// Return the ShellLink object
	shelllink_ptr->lpVtbl->Release(shelllink_ptr);
    }
    अन्यथा
    {
	म_लिखो("CoCreateInstance Error - hres = %08x\n", (पूर्णांक)hres);
	परत FAIL;
    }

    परत OK;
}

/*
 * Build a path to where we will put a specified link.
 *
 * Return 0 on error, non-zero on success
 */
   पूर्णांक
build_link_name(
	अक्षर	   *link_path,
	स्थिरांक अक्षर *link_name,
	स्थिरांक अक्षर *shell_folder_name)
{
    अक्षर	shell_folder_path[MAX_PATH];

    जर (get_shell_folder_path(shell_folder_path, shell_folder_name) == FAIL)
    {
	म_लिखो("An error occurred while attempting to find the path to %s.\n",
							   shell_folder_name);
	परत FAIL;
    }

    // Make sure the directory exists (create Start Menu\Programs\Vim).
    // Ignore errors if it already exists.
    vim_mkdir(shell_folder_path, 0755);

    // build the path to the shortcut and the path to gvim.exe
    प्र_लिखो(link_path, "%s\\%s.lnk", shell_folder_path, link_name);

    परत OK;
}

    स्थिर पूर्णांक
build_shortcut(
	स्थिरांक अक्षर *name,	// Name of the shortcut
	स्थिरांक अक्षर *exename,	// Name of the executable (e.g., vim.exe)
	स्थिरांक अक्षर *args,
	स्थिरांक अक्षर *shell_folder,
	स्थिरांक अक्षर *workingdir)
{
    अक्षर	executable_path[BUFSIZE];
    अक्षर	link_name[BUFSIZE];

    प्र_लिखो(executable_path, "%s\\%s", installdir, exename);

    जर (build_link_name(link_name, name, shell_folder) == FAIL)
    {
	म_लिखो("An error has occurred.  A shortcut to %s will not be created %s.\n",
		name,
		*shell_folder == 'd' ? "on the desktop" : "in the Start menu");
	परत FAIL;
    }

    // Create the shortcut:
    परत create_shortcut(link_name, executable_path, 0,
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
install_start_menu(पूर्णांक idx UNUSED)
{
    need_uninstall_entry = 1;
    म_लिखो("Creating start menu\n");
    जर (has_vim)
    {
	जर (build_shortcut("Vim", "vim.exe", "",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    परत;
	जर (build_shortcut("Vim Read-only", "vim.exe", "-R",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    परत;
	जर (build_shortcut("Vim Diff", "vim.exe", "-d",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    परत;
    }
    जर (has_gvim)
    {
	जर (build_shortcut("gVim", "gvim.exe", "",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    परत;
	जर (build_shortcut("gVim Easy", "gvim.exe", "-y",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    परत;
	जर (build_shortcut("gVim Read-only", "gvim.exe", "-R",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    परत;
	जर (build_shortcut("gVim Diff", "gvim.exe", "-d",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    परत;
    }
    जर (build_shortcut("Uninstall",
		interactive ? "uninstall.exe" : "uninstall-gui.exe", "",
					   VIM_STARTMENU, installdir) == FAIL)
	परत;
    // For Windows NT the working dir of the vimtutor.bat must be right,
    // otherwise gvim.exe won't be found and using gvimbat doesn't work.
    जर (build_shortcut("Vim tutor", "vimtutor.bat", "",
					   VIM_STARTMENU, installdir) == FAIL)
	परत;
    जर (build_shortcut("Help", has_gvim ? "gvim.exe" : "vim.exe", "-c h",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	परत;
    {
	अक्षर	shell_folder_path[BUFSIZE];

	// Creating the URL shortcut works a bit differently...
	जर (get_shell_folder_path(shell_folder_path, VIM_STARTMENU) == FAIL)
	{
	    म_लिखो("Finding the path of the Start menu failed\n");
	    परत ;
	}
	add_pathsep(shell_folder_path);
	म_जोड़ो(shell_folder_path, "Vim Online.url");
	जर (!WritePrivateProfileString("InternetShortcut", "URL",
				    "https://www.vim.org/", shell_folder_path))
	{
	    म_लिखो("Creating the Vim online URL failed\n");
	    परत;
	}
    }
}

    स्थिर शून्य
toggle_startmenu_choice(पूर्णांक idx)
{
    जर (choices[idx].installfunc == रिक्त)
    {
	choices[idx].installfunc = install_start_menu;
	choices[idx].text = "Add Vim to the Start menu";
    }
    अन्यथा
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
install_shortcut_gvim(पूर्णांक idx)
{
    // Create shortcut(s) on the desktop
    जर (choices[idx].arg)
    {
	(शून्य)build_shortcut(icon_names[0], "gvim.exe",
						      "", "desktop", WORKDIR);
	need_uninstall_entry = 1;
    }
}

    शून्य
install_shortcut_evim(पूर्णांक idx)
{
    जर (choices[idx].arg)
    {
	(शून्य)build_shortcut(icon_names[1], "gvim.exe",
						    "-y", "desktop", WORKDIR);
	need_uninstall_entry = 1;
    }
}

    शून्य
install_shortcut_gview(पूर्णांक idx)
{
    जर (choices[idx].arg)
    {
	(शून्य)build_shortcut(icon_names[2], "gvim.exe",
						    "-R", "desktop", WORKDIR);
	need_uninstall_entry = 1;
    }
}

    शून्य
toggle_shortcut_choice(पूर्णांक idx)
{
    अक्षर	*arg;

    जर (choices[idx].installfunc == install_shortcut_gvim)
	arg = "gVim";
    अन्यथा जर (choices[idx].installfunc == install_shortcut_evim)
	arg = "gVim Easy";
    अन्यथा
	arg = "gVim Read-only";
    जर (choices[idx].arg)
    {
	choices[idx].arg = 0;
	alloc_text(idx, "Do NOT create a desktop icon for %s", arg);
    }
    अन्यथा
    {
	choices[idx].arg = 1;
	alloc_text(idx, "Create a desktop icon for %s", arg);
    }
}

    स्थिर शून्य
init_startmenu_choice(शून्य)
{
    // Start menu
    choices[choice_मोजणी].changefunc = toggle_startmenu_choice;
    choices[choice_मोजणी].installfunc = रिक्त;
    choices[choice_मोजणी].active = 1;
    toggle_startmenu_choice(choice_मोजणी);	// set the text
    ++choice_मोजणी;
}

/*
 * Add the choice for the desktop shortcuts.
 */
    स्थिर शून्य
init_shortcut_choices(शून्य)
{
    // Shortcut to gvim
    choices[choice_मोजणी].text = रिक्त;
    choices[choice_मोजणी].arg = 0;
    choices[choice_मोजणी].active = has_gvim;
    choices[choice_मोजणी].changefunc = toggle_shortcut_choice;
    choices[choice_मोजणी].installfunc = install_shortcut_gvim;
    toggle_shortcut_choice(choice_मोजणी);
    ++choice_मोजणी;

    // Shortcut to evim
    choices[choice_मोजणी].text = रिक्त;
    choices[choice_मोजणी].arg = 0;
    choices[choice_मोजणी].active = has_gvim;
    choices[choice_मोजणी].changefunc = toggle_shortcut_choice;
    choices[choice_मोजणी].installfunc = install_shortcut_evim;
    toggle_shortcut_choice(choice_मोजणी);
    ++choice_मोजणी;

    // Shortcut to gview
    choices[choice_मोजणी].text = रिक्त;
    choices[choice_मोजणी].arg = 0;
    choices[choice_मोजणी].active = has_gvim;
    choices[choice_मोजणी].changefunc = toggle_shortcut_choice;
    choices[choice_मोजणी].installfunc = install_shortcut_gview;
    toggle_shortcut_choice(choice_मोजणी);
    ++choice_मोजणी;
}

/*
 * Attempt to register OLE for Vim.
 */
   स्थिर शून्य
install_OLE_register(शून्य)
{
    अक्षर register_command_string[BUFSIZE + 30];

    म_लिखो("\n--- Attempting to register Vim with OLE ---\n");
    म_लिखो("(There is no message whether this works or not.)\n");

    प्र_लिखो(register_command_string, "\"%s\\gvim.exe\" -silent -register", installdir);
    प्रणाली(register_command_string);
}

/*
 * Remove the last part of directory "path[]" to get its parent, and put the
 * result in "to[]".
 */
    स्थिर शून्य
dir_remove_last(स्थिरांक अक्षर *path, अक्षर to[MAX_PATH])
{
    अक्षर c;
    दीर्घ last_char_to_copy;
    दीर्घ path_length = म_माप(path);

    // skip the last character just in case it is a '\\'
    last_char_to_copy = path_length - 2;
    c = path[last_char_to_copy];

    पर्यंत (c != '\\')
    {
	last_char_to_copy--;
	c = path[last_char_to_copy];
    }

    म_नकलन(to, path, (माप_प्रकार)last_char_to_copy);
    to[last_char_to_copy] = NUL;
}

    स्थिर शून्य
set_directories_text(पूर्णांक idx)
{
    पूर्णांक vimfiles_dir_choice = choices[idx].arg;

    जर (vimfiles_dir_choice == (पूर्णांक)vimfiles_dir_none)
	alloc_text(idx, "Do NOT create plugin directories%s", "");
    अन्यथा
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
स्थिर अक्षर *homedir = रिक्त;

    शून्य
init_homedir(शून्य)
{
    अक्षर    *var;
    अक्षर    buf[MAX_PATH];

    जर (homedir != रिक्त)
    {
	मुक्त(homedir);
	homedir = रिक्त;
    }

    var = दो_पर्या("HOME");

    /*
     * Typically, $HOME is not defined on Windows, unless the user has
     * specifically defined it for Vim's sake.  However, on Windows NT
     * platforms, $HOMEDRIVE and $HOMEPATH are automatically defined for
     * each user.  Try constructing $HOME from these.
     */
    जर (var == रिक्त || *var == NUL)
    {
	अक्षर	*homedrive, *homepath;

	homedrive = दो_पर्या("HOMEDRIVE");
	homepath = दो_पर्या("HOMEPATH");
	जर (homepath == रिक्त || *homepath == NUL)
	    homepath = "\\";
	जर (homedrive != रिक्त
		   && म_माप(homedrive) + म_माप(homepath) < आकार(buf))
	{
	    जर (snprintf(buf, आकार(buf), "%s%s", homedrive, homepath) > 0
		&& buf[0] != NUL)
		var = buf;
	}
    }

    जर (var == रिक्त)
	var = दो_पर्या("USERPROFILE");

    /*
     * Weird but true: $HOME may contain an indirect reference to another
     * variable, esp. "%USERPROFILE%".  Happens when $USERPROFILE isn't set
     * when $HOME is being set.
     */
    जर (var != रिक्त && *var == '%')
    {
	अक्षर	*p;
	अक्षर	*exp;

	p = म_अक्षर(var + 1, '%');
	जर (p != रिक्त)
	{
	    म_नकलन(buf, var + 1, p - (var + 1));
	    buf[p - (var + 1)] = NUL;
	    exp = दो_पर्या(buf);
	    जर (exp != रिक्त && *exp != NUL
				&& म_माप(exp) + म_माप(p) < आकार(buf))
	    {
		snprintf(buf, आकार(buf), "%s%s", exp, p + 1);
		var = buf;
	    }
	}
    }

    जर (var != रिक्त && *var == NUL)	// empty is same as not set
	var = रिक्त;

    जर (var == रिक्त)
	homedir = रिक्त;
    अन्यथा
	homedir = _strdup(var);
}

/*
 * Change the directory that the vim plugin directories will be created in:
 * $HOME, $VIM or nowhere.
 */
    स्थिर शून्य
change_directories_choice(पूर्णांक idx)
{
    पूर्णांक	    choice_मोजणी = TABLE_SIZE(vimfiles_dir_choices);

    // Don't offer the $HOME choice if $HOME isn't set.
    जर (homedir == रिक्त)
	--choice_मोजणी;
    choices[idx].arg = get_choice(vimfiles_dir_choices, choice_मोजणी);
    set_directories_text(idx);
}

/*
 * Create the plugin directories...
 */
//ARGSUSED
    स्थिर शून्य
install_vimfilesdir(पूर्णांक idx)
{
    पूर्णांक i;
    पूर्णांक vimfiles_dir_choice = choices[idx].arg;
    अक्षर *p;
    अक्षर vimdir_path[MAX_PATH];
    अक्षर vimfiles_path[MAX_PATH + 9];
    अक्षर tmp_dirname[BUFSIZE];

    // switch on the location that the user wants the plugin directories
    // built in
    निवड (vimfiles_dir_choice)
    {
	स्थिती vimfiles_dir_vim:
	{
	    // Go to the %VIM% directory - check env first, then go one dir
	    //	   below installdir if there is no %VIM% environment variable.
	    //	   The accuracy of $VIM is checked in inspect_system(), so we
	    //	   can be sure it is ok to use here.
	    p = दो_पर्या("VIM");
	    जर (p == रिक्त) // No $VIM in path
		dir_remove_last(installdir, vimdir_path);
	    अन्यथा
		म_नकल(vimdir_path, p);
	    थांबा;
	}
	स्थिती vimfiles_dir_home:
	{
	    // Find the $HOME directory.  Its existence was already checked.
	    p = homedir;
	    जर (p == रिक्त)
	    {
		म_लिखो("Internal error: $HOME is NULL\n");
		p = "c:\\";
	    }
	    म_नकल(vimdir_path, p);
	    थांबा;
	}
	स्थिती vimfiles_dir_none:
	{
	    // Do not create vim plugin directory.
	    परत;
	}
    }

    // Now, just create the directory.	If it already exists, it will fail
    // silently.
    प्र_लिखो(vimfiles_path, "%s\\vimfiles", vimdir_path);
    vim_mkdir(vimfiles_path, 0755);

    म_लिखो("Creating the following directories in \"%s\":\n", vimfiles_path);
    साठी (i = 0; i < TABLE_SIZE(vimfiles_subdirs); i++)
    {
	प्र_लिखो(tmp_dirname, "%s\\%s", vimfiles_path, vimfiles_subdirs[i]);
	म_लिखो("  %s", vimfiles_subdirs[i]);
	vim_mkdir(tmp_dirname, 0755);
    }
    म_लिखो("\n");
}

/*
 * Add the creation of runtime files to the setup sequence.
 */
    स्थिर शून्य
init_directories_choice(शून्य)
{
    संरचना stat	st;
    अक्षर	tmp_dirname[BUFSIZE];
    अक्षर	*p;
    पूर्णांक		vimfiles_dir_choice;

    choices[choice_मोजणी].text = alloc(150);
    choices[choice_मोजणी].changefunc = change_directories_choice;
    choices[choice_मोजणी].installfunc = install_vimfilesdir;
    choices[choice_मोजणी].active = 1;

    // Check if the "compiler" directory already exists.  That's a good
    // indication that the plugin directories were already created.
    p = दो_पर्या("HOME");
    जर (p != रिक्त)
    {
	vimfiles_dir_choice = (पूर्णांक)vimfiles_dir_home;
	प्र_लिखो(tmp_dirname, "%s\\vimfiles\\compiler", p);
	जर (stat(tmp_dirname, &st) == 0)
	    vimfiles_dir_choice = (पूर्णांक)vimfiles_dir_none;
    }
    अन्यथा
    {
	vimfiles_dir_choice = (पूर्णांक)vimfiles_dir_vim;
	p = दो_पर्या("VIM");
	जर (p == रिक्त)  // No $VIM in path, use the install dir.
	    dir_remove_last(installdir, tmp_dirname);
	अन्यथा
	    म_नकल(tmp_dirname, p);
	म_जोड़ो(tmp_dirname, "\\vimfiles\\compiler");
	जर (stat(tmp_dirname, &st) == 0)
	    vimfiles_dir_choice = (पूर्णांक)vimfiles_dir_none;
    }

    choices[choice_मोजणी].arg = vimfiles_dir_choice;
    set_directories_text(choice_मोजणी);
    ++choice_मोजणी;
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
print_cmd_ओळ_help(शून्य)
{
    म_लिखो("Vim installer non-interactive command line arguments:\n");
    म_लिखो("\n");
    म_लिखो("-create-batfiles  [vim gvim evim view gview vimdiff gvimdiff]\n");
    म_लिखो("    Create .bat files for Vim variants in the Windows directory.\n");
    म_लिखो("-create-vimrc\n");
    म_लिखो("    Create a default _vimrc file if one does not already exist.\n");
    म_लिखो("-vimrc-remap [no|win]\n");
    म_लिखो("    Remap keys when creating a default _vimrc file.\n");
    म_लिखो("-vimrc-behave [unix|mswin|default]\n");
    म_लिखो("    Set mouse behavior when creating a default _vimrc file.\n");
    म_लिखो("-vimrc-compat [vi|vim|defaults|all]\n");
    म_लिखो("    Set Vi compatibility when creating a default _vimrc file.\n");
    म_लिखो("-install-popup\n");
    म_लिखो("    Install the Edit-with-Vim context menu entry\n");
    म_लिखो("-install-openwith\n");
    म_लिखो("    Add Vim to the \"Open With...\" context menu list\n");
    म_लिखो("-add-start-menu");
    म_लिखो("    Add Vim to the start menu\n");
    म_लिखो("-install-icons");
    म_लिखो("    Create icons for gVim executables on the desktop\n");
    म_लिखो("-create-directories [vim|home]\n");
    म_लिखो("    Create runtime directories to drop plugins into; in the $VIM\n");
    म_लिखो("    or $HOME directory\n");
    म_लिखो("-register-OLE");
    म_लिखो("    Ignored\n");
    म_लिखो("\n");
}

/*
 * Setup installation choices based on command line switches
 */
    स्थिर शून्य
command_ओळ_setup_choices(पूर्णांक argc, अक्षर **argv)
{
    पूर्णांक i, j;

    साठी (i = 1; i < argc; i++)
    {
	जर (म_भेद(argv[i], "-create-batfiles") == 0)
	{
	    जर (i + 1 == argc)
		सुरूठेवा;
	    पर्यंत (argv[i + 1][0] != '-' && i < argc)
	    {
		i++;
		साठी (j = 1; j < TARGET_मोजणी; ++j)
		    जर ((targets[j].exenamearg[0] == 'g' ? has_gvim : has_vim)
			    && म_भेद(argv[i], targets[j].name) == 0)
		    {
			init_bat_choice(j);
			थांबा;
		    }
		जर (j == TARGET_मोजणी)
		    म_लिखो("%s is not a valid choice for -create-batfiles\n",
								     argv[i]);

		जर (i + 1 == argc)
		    थांबा;
	    }
	}
	अन्यथा जर (म_भेद(argv[i], "-create-vimrc") == 0)
	{
	    // Setup default vimrc choices.  If there is already a _vimrc file,
	    // it will NOT be overwritten.
	    init_vimrc_choices();
	}
	अन्यथा जर (म_भेद(argv[i], "-vimrc-remap") == 0)
	{
	    जर (i + 1 == argc)
		थांबा;
	    i++;
	    जर (म_भेद(argv[i], "no") == 0)
		remap_choice = remap_no;
	    अन्यथा जर (म_भेद(argv[i], "win") == 0)
		remap_choice = remap_win;
	}
	अन्यथा जर (म_भेद(argv[i], "-vimrc-behave") == 0)
	{
	    जर (i + 1 == argc)
		थांबा;
	    i++;
	    जर (म_भेद(argv[i], "unix") == 0)
		mouse_choice = mouse_xterm;
	    अन्यथा जर (म_भेद(argv[i], "mswin") == 0)
		mouse_choice = mouse_mswin;
	    अन्यथा जर (म_भेद(argv[i], "default") == 0)
		mouse_choice = mouse_default;
	}
	अन्यथा जर (म_भेद(argv[i], "-vimrc-compat") == 0)
	{
	    जर (i + 1 == argc)
		थांबा;
	    i++;
	    जर (म_भेद(argv[i], "vi") == 0)
		compat_choice = compat_vi;
	    अन्यथा जर (म_भेद(argv[i], "vim") == 0)
		compat_choice = compat_vim;
	    अन्यथा जर (म_भेद(argv[i], "defaults") == 0)
		compat_choice = compat_some_enhancements;
	    अन्यथा जर (म_भेद(argv[i], "all") == 0)
		compat_choice = compat_all_enhancements;
	}
	अन्यथा जर (म_भेद(argv[i], "-install-popup") == 0)
	{
	    init_popup_choice();
	}
	अन्यथा जर (म_भेद(argv[i], "-install-openwith") == 0)
	{
	    init_openwith_choice();
	}
	अन्यथा जर (म_भेद(argv[i], "-add-start-menu") == 0)
	{
	    init_startmenu_choice();
	}
	अन्यथा जर (म_भेद(argv[i], "-install-icons") == 0)
	{
	    init_shortcut_choices();
	}
	अन्यथा जर (म_भेद(argv[i], "-create-directories") == 0)
	{
	    पूर्णांक vimfiles_dir_choice = (पूर्णांक)vimfiles_dir_none;

	    init_directories_choice();
	    जर (i + 1 < argc && argv[i + 1][0] != '-')
	    {
		i++;
		जर (म_भेद(argv[i], "vim") == 0)
		    vimfiles_dir_choice = (पूर्णांक)vimfiles_dir_vim;
		अन्यथा जर (म_भेद(argv[i], "home") == 0)
		{
		    जर (homedir == रिक्त)  // No $HOME in environment
			vimfiles_dir_choice = (पूर्णांक)vimfiles_dir_none;
		    अन्यथा
			vimfiles_dir_choice = (पूर्णांक)vimfiles_dir_home;
		}
		अन्यथा
		{
		    म_लिखो("Unknown argument for -create-directories: %s\n",
								     argv[i]);
		    print_cmd_ओळ_help();
		}
	    }
	    अन्यथा // No choice specified, default to vim directory
		vimfiles_dir_choice = (पूर्णांक)vimfiles_dir_vim;
	    choices[choice_मोजणी - 1].arg = vimfiles_dir_choice;
	}
	अन्यथा जर (म_भेद(argv[i], "-register-OLE") == 0)
	{
	    // This is always done when gvim is found
	}
	अन्यथा // Unknown switch
	{
	    म_लिखो("Got unknown argument argv[%d] = %s\n", i, argv[i]);
	    print_cmd_ओळ_help();
	}
    }
}


/*
 * Show a few screens full of helpful information.
 */
    स्थिर शून्य
show_help(शून्य)
{
    स्थिर अक्षर *(वस्तुसंच[]) =
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
    पूर्णांक		i;
    पूर्णांक		c;

    शुरुआत(मानक_निवेश);
    म_लिखो("\n");
    साठी (i = 0; वस्तुसंच[i] != रिक्त; ++i)
    {
	माला_दो(वस्तुसंच[i]);
	म_लिखो("Hit Enter to continue, b (back) or q (quit help): ");
	c = अक्षर_लो();
	शुरुआत(मानक_निवेश);
	जर (c == 'b' || c == 'B')
	{
	    जर (i == 0)
		--i;
	    अन्यथा
		i -= 2;
	}
	जर (c == 'q' || c == 'Q')
	    थांबा;
	म_लिखो("\n");
    }
}

/*
 * Install the choices.
 */
    स्थिर शून्य
install(शून्य)
{
    पूर्णांक		i;

    // Install the selected choices.
    साठी (i = 0; i < choice_मोजणी; ++i)
	जर (choices[i].installfunc != रिक्त && choices[i].active)
	    (choices[i].installfunc)(i);

    // Add some entries to the registry, if needed.
    जर (install_popup
	    || install_openwith
	    || (need_uninstall_entry && interactive)
	    || !interactive)
	install_registry();

    // Register gvim with OLE.
    जर (has_gvim)
	install_OLE_register();
}

/*
 * request_choice
 */
    स्थिर शून्य
request_choice(शून्य)
{
    पूर्णांक		      i;

    म_लिखो("\n\nInstall will do for you:\n");
    साठी (i = 0; i < choice_मोजणी; ++i)
	जर (choices[i].active)
	    म_लिखो("%2d  %s\n", i + 1, choices[i].text);
    म_लिखो("To change an item, enter its number\n\n");
    म_लिखो("Enter item number, h (help), d (do it) or q (quit): ");
}

    पूर्णांक
main(पूर्णांक argc, अक्षर **argv)
{
    पूर्णांक		i;
    अक्षर	buf[BUFSIZE];

    /*
     * Run interactively if there are no command line arguments.
     */
    जर (argc > 1)
	interactive = 0;
    अन्यथा
	interactive = 1;

    // Initialize this program.
    do_inits(argv);
    init_homedir();

    जर (argc > 1 && म_भेद(argv[1], "-uninstall-check") == 0)
    {
	// Only check for already installed Vims.  Used by NSIS installer.
	i = uninstall_check(1);

	// Find the value of $VIM, because NSIS isn't able to do this by
	// itself.
	get_vim_env();

	// When nothing found exit quietly.  If something found wait for
	// a little while, so that the user can read the messages.
	जर (i && _isatty(1))
	    sleep(3);
	निकास(0);
    }

    म_लिखो("This program sets up the installation of Vim "
						   VIM_VERSION_MEDIUM "\n\n");

    // Check if the user unpacked the archives properly.
    check_unpack();

    // Check for already installed Vims.
    जर (interactive)
	uninstall_check(0);

    // Find out information about the system.
    inspect_system();

    जर (interactive)
    {
	// Setup all the choices.
	setup_choices();

	// Let the user change choices and finally install (or quit).
	साठी (;;)
	{
	    request_choice();
	    शुरुआत(मानक_निवेश);
	    जर (म_पूछो("%99s", buf) == 1)
	    {
		जर (है_अंक((अचिन्हित अक्षर)buf[0]))
		{
		    // Change a choice.
		    i = म_से_प(buf);
		    जर (i > 0 && i <= choice_मोजणी && choices[i - 1].active)
			(choices[i - 1].changefunc)(i - 1);
		    अन्यथा
			म_लिखो("\nIllegal choice\n");
		}
		अन्यथा जर (buf[0] == 'h' || buf[0] == 'H')
		{
		    // Help
		    show_help();
		}
		अन्यथा जर (buf[0] == 'd' || buf[0] == 'D')
		{
		    // Install!
		    install();
		    म_लिखो("\nThat finishes the installation.  Happy Vimming!\n");
		    थांबा;
		}
		अन्यथा जर (buf[0] == 'q' || buf[0] == 'Q')
		{
		    // Quit
		    म_लिखो("\nExiting without anything done\n");
		    थांबा;
		}
		अन्यथा
		    म_लिखो("\nIllegal choice\n");
	    }
	}
	म_लिखो("\n");
	myexit(0);
    }
    अन्यथा
    {
	/*
	 * Run non-interactive - setup according to the command line switches
	 */
	command_ओळ_setup_choices(argc, argv);
	install();

	// Avoid that the user has to hit Enter, just wait a little bit to
	// allow reading the messages.
	sleep(2);
    }

    परत 0;
}
