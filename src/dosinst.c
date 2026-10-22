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
#ଅନ୍ତର୍ଭାଵ "dosinst.h"
#ଅନ୍ତର୍ଭାଵ <io.h>

#define GVIMEXT64_PATH	    "GvimExt64\\gvimext.dll"
#define GVIMEXT32_PATH	    "GvimExt32\\gvimext.dll"

// Macro to do an error check I was typing over and over
#define CHECK_REG_ERROR(code) \
    କର { \
	ଯଦି (code != ERROR_SUCCESS) \
	{ \
	    printf("%ld error number:  %ld\n", (long)__LINE__, (long)code); \
	    ଫେରନ୍ତୁ 1; \
	} \
    } ଯେତେବେଳେ (0)

ପୂର୍ଣ୍ଣସଂଖ୍ୟା	has_vim = 0;		// installable vim.exe exists
ପୂର୍ଣ୍ଣସଂଖ୍ୟା	has_gvim = 0;		// installable gvim.exe exists

ଅକ୍ଷର	oldvimrc[BUFSIZE];	// name of existing vimrc file
ଅକ୍ଷର	vimrc[BUFSIZE];		// name of vimrc file to create

ଅକ୍ଷର	*default_bat_dir = ଖାଲି;  // when not NULL, use this as the default
				  // directory to write .bat files in
ଅକ୍ଷର	*default_vim_dir = ଖାଲି;  // when not NULL, use this as the default
				  // install dir for NSIS

/*
 * Structure used for each choice the user can make.
 */
ସଂରଚନା choice
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    active;			// non-zero when choice is active
    ଅକ୍ଷର    *text;			// text displayed for this choice
    ଶୂନ୍ୟ    (*changefunc)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);	// function to change this choice
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    arg;			// argument for function
    ଶୂନ୍ୟ    (*installfunc)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);	// function to install this choice
};

ସଂରଚନା choice	choices[30];		// choices the user can make
ପୂର୍ଣ୍ଣସଂଖ୍ୟା		choice_ଗଣନା = 0;	// number of choices available

#define TABLE_SIZE(s)	(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAYSIZE(s)

ଗଣନା
{
    compat_vi = 1,
    compat_vim,
    compat_some_enhancements,
    compat_all_enhancements
};
ଅକ୍ଷର	*(compat_choices[]) =
{
    "\nChoose the default way to run Vim:",
    "Vi compatible",
    "Vim default",
    "with some Vim enhancements",
    "with syntax highlighting and other features switched on",
};
ପୂର୍ଣ୍ଣସଂଖ୍ୟା	compat_choice = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)compat_all_enhancements;
ଅକ୍ଷର	*compat_text = "- run Vim %s";

ଗଣନା
{
    remap_no = 1,
    remap_win
};
ଅକ୍ଷର	*(remap_choices[]) =
{
    "\nChoose:",
    "Do not remap keys for Windows behavior",
    "Remap a few keys for Windows behavior (CTRL-V, CTRL-C, CTRL-F, etc)",
};
ପୂର୍ଣ୍ଣସଂଖ୍ୟା	remap_choice = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)remap_no;
ଅକ୍ଷର	*remap_text = "- %s";

ଗଣନା
{
    mouse_xterm = 1,
    mouse_mswin,
    mouse_default
};
ଅକ୍ଷର	*(mouse_choices[]) =
{
    "\nChoose the way how Vim uses the mouse:",
    "right button extends selection (the Unix way)",
    "right button has a popup menu, left button starts select mode (the Windows way)",
    "right button has a popup menu, left button starts visual mode",
};
ପୂର୍ଣ୍ଣସଂଖ୍ୟା	mouse_choice = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)mouse_default;
ଅକ୍ଷର	*mouse_text = "- The mouse %s";

ଗଣନା
{
    vimfiles_dir_none = 1,
    vimfiles_dir_vim,
    vimfiles_dir_home
};
ସ୍ଥିର ଅକ୍ଷର *(vimfiles_dir_choices[]) =
{
    "\nCreate plugin directories:",
    "No",
    "In the VIM directory",
    "In your HOME directory",
};

// non-zero when selected to install the popup menu entry.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	install_popup = 0;

// non-zero when selected to install the "Open with" entry.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	install_openwith = 0;

// non-zero when need to add an uninstall entry in the registry
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	need_uninstall_entry = 0;

/*
 * Definitions of the directory name (under $VIM) of the vimfiles directory
 * and its subdirectories:
 */
ସ୍ଥିର ଅକ୍ଷର	*(vimfiles_subdirs[]) =
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_choice(ଅକ୍ଷର **table, ପୂର୍ଣ୍ଣସଂଖ୍ୟା entries)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		answer;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    ଅକ୍ଷର	dummy[100];

    କର
    {
	ପାଇଁ (idx = 0; idx < entries; ++idx)
	{
	    ଯଦି (idx)
		printf("%2d  ", idx);
	    puts(table[idx]);
	}
	printf("Choice: ");
	ଯଦି (scanf("%d", &answer) != 1)
	{
	    scanf("%99s", dummy);
	    answer = 0;
	}
    }
    ଯେତେବେଳେ (answer < 1 || answer >= entries);

    ଫେରନ୍ତୁ answer;
}

/*
 * Check if the user unpacked the archives properly.
 * Sets "runtimeidx".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
check_unpack(ଶୂନ୍ୟ)
{
    ଅକ୍ଷର	buf[BUFSIZE];
    FILE	*fd;
    ସଂରଚନା stat	st;

    // check for presence of the correct version number in installdir[]
    runtimeidx = strlen(installdir) - strlen(VIM_VERSION_NODOT);
    ଯଦି (runtimeidx <= 0
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
    ଯଦି (stat(buf, &st) < 0)
    {
	printf("ERROR: Cannot find filetype.vim in \"%s\"\n", installdir);
	printf("It looks like you did not unpack the runtime archive.\n");
	printf("You must unpack the runtime archive \"%srt.zip\" before installing.\n",
		VIM_VERSION_NODOT);
	myexit(1);
    }

    // Check if vim.exe or gvim.exe is in the current directory.
    ଯଦି ((fd = fopen("gvim.exe", "r")) != ଖାଲି)
    {
	fclose(fd);
	has_gvim = 1;
    }
    ଯଦି ((fd = fopen("vim.exe", "r")) != ଖାଲି)
    {
	fclose(fd);
	has_vim = 1;
    }
    ଯଦି (!has_gvim && !has_vim)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
pathcmp(ଅକ୍ଷର *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା plen, ଅକ୍ଷର *q, ପୂର୍ଣ୍ଣସଂଖ୍ୟା qlen)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (plen < 0)
	plen = strlen(p);
    ଯଦି (qlen < 0)
	qlen = strlen(q);
    ପାଇଁ (i = 0; ; ++i)
    {
	// End of "p": check if "q" also ends or just has a slash.
	ଯଦି (i == plen)
	{
	    ଯଦି (i == qlen)  // match
		ଫେରନ୍ତୁ 0;
	    ଯଦି (i == qlen - 1 && (q[i] == '\\' || q[i] == '/'))
		ଫେରନ୍ତୁ 0;   // match with trailing slash
	    ଫେରନ୍ତୁ 1;	    // no match
	}

	// End of "q": check if "p" also ends or just has a slash.
	ଯଦି (i == qlen)
	{
	    ଯଦି (i == plen)  // match
		ଫେରନ୍ତୁ 0;
	    ଯଦି (i == plen - 1 && (p[i] == '\\' || p[i] == '/'))
		ଫେରନ୍ତୁ 0;   // match with trailing slash
	    ଫେରନ୍ତୁ 1;	    // no match
	}

	ଯଦି (!(mytoupper(p[i]) == mytoupper(q[i])
		|| ((p[i] == '/' || p[i] == '\\')
		    && (q[i] == '/' || q[i] == '\\'))))
	    ଫେରନ୍ତୁ 1;	    // no match
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
    ସ୍ଥିର ଶୂନ୍ୟ
findoldfile(ଅକ୍ଷର **destination)
{
    ଅକ୍ଷର	*bp = *destination;
    size_t	indir_l = strlen(installdir);
    ଅକ୍ଷର	*cp;
    ଅକ୍ଷର	*tmpname;
    ଅକ୍ଷର	*farname;

    /*
     * No action needed if exe not found or not in this directory.
     */
    ଯଦି (bp == ଖାଲି || strnicmp(bp, installdir, indir_l) != 0)
	ଫେରନ୍ତୁ;
    cp = bp + indir_l;
    ଯଦି (strchr("/\\", *cp++) == ଖାଲି
	    || strchr(cp, '\\') != ଖାଲି
	    || strchr(cp, '/') != ଖାଲି)
	ଫେରନ୍ତୁ;

    tmpname = alloc(strlen(cp) + 1);
    strcpy(tmpname, cp);
    tmpname[strlen(tmpname) - 1] = 'x';	// .exe -> .exx

    ଯଦି (access(tmpname, 0) == 0)
    {
	printf("\nERROR: %s and %s clash.  Remove or rename %s.\n",
	    tmpname, cp, tmpname);
	myexit(1);
    }

    ଯଦି (rename(cp, tmpname) != 0)
    {
	printf("\nERROR: failed to rename %s to %s: %s\n",
	    cp, tmpname, strerror(0));
	myexit(1);
    }

    farname = searchpath_save(cp);

    ଯଦି (rename(tmpname, cp) != 0)
    {
	printf("\nERROR: failed to rename %s back to %s: %s\n",
	    tmpname, cp, strerror(0));
	myexit(1);
    }

    ମୁକ୍ତ(*destination);
    ମୁକ୍ତ(tmpname);
    *destination = farname;
}

/*
 * Check if there is a vim.[exe|bat|, gvim.[exe|bat|, etc. in the path.
 * When "check_bat_only" is TRUE, only find "default_bat_dir".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
find_bat_exe(ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_bat_only)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    ପାଇଁ (i = 1; i < TARGET_ଗଣନା; ++i)
    {
	targets[i].oldbat = searchpath_save(targets[i].batname);
	ଯଦି (!check_bat_only)
	    targets[i].oldexe = searchpath_save(targets[i].exename);

	ଯଦି (default_bat_dir == ଖାଲି && targets[i].oldbat != ଖାଲି)
	{
	    default_bat_dir = alloc(strlen(targets[i].oldbat) + 1);
	    strcpy(default_bat_dir, targets[i].oldbat);
	    remove_tail(default_bat_dir);
	}
	ଯଦି (check_bat_only && targets[i].oldbat != ଖାଲି)
	{
	    ମୁକ୍ତ(targets[i].oldbat);
	    targets[i].oldbat = ଖାଲି;
	}
    }

    mch_chdir(installdir);
}

/*
 * Get the value of $VIMRUNTIME or $VIM and write it in $TEMP/vimini.ini, so
 * that NSIS can read it.
 * When not set, use the directory of a previously installed Vim.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
get_vim_env(ଶୂନ୍ୟ)
{
    ଅକ୍ଷର	*vim;
    ଅକ୍ଷର	buf[BUFSIZE];
    FILE	*fd;
    ଅକ୍ଷର	fname[BUFSIZE];

    // First get $VIMRUNTIME.  If it's set, remove the tail.
    vim = getenv("VIMRUNTIME");
    ଯଦି (vim != ଖାଲି && *vim != 0 && strlen(vim) < ଆକାର(buf))
    {
	strcpy(buf, vim);
	remove_tail(buf);
	vim = buf;
    }
    ଅନ୍ୟଥା
    {
	vim = getenv("VIM");
	ଯଦି (vim == ଖାଲି || *vim == 0)
	{
	    // Use the directory from an old uninstall entry.
	    ଯଦି (default_vim_dir != ଖାଲି)
		vim = default_vim_dir;
	    ଅନ୍ୟଥା
		// Let NSIS know there is no default, it should use
		// $PROGRAMFILES.
		vim = "";
	}
    }

    // NSIS also uses GetTempPath(), thus we should get the same directory
    // name as where NSIS will look for vimini.ini.
    GetTempPath(ଆକାର(fname) - 12, fname);
    add_pathsep(fname);
    strcat(fname, "vimini.ini");

    fd = fopen(fname, "w");
    ଯଦି (fd != ଖାଲି)
    {
	// Make it look like an .ini file, so that NSIS can read it with a
	// ReadINIStr command.
	fprintf(fd, "[vimini]\n");
	fprintf(fd, "dir=\"%s\"\n", vim);
	fclose(fd);
    }
    ଅନ୍ୟଥା
    {
	printf("Failed to open %s\n", fname);
	sleep(2);
    }
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_windows;

/*
 * Callback used for EnumWindows():
 * Count the window if the title looks like it is for the uninstaller.
 */
//ARGSUSED
    ସ୍ଥିର BOOL CALLBACK
window_cb(HWND hwnd, LPARAM lparam UNUSED)
{
    ଅକ୍ଷର title[256];

    title[0] = 0;
    GetWindowText(hwnd, title, 256);
    ଯଦି (strstr(title, "Vim ") != ଖାଲି && strstr(title, " Uninstall") != ଖାଲି)
	++ଅଙ୍କ_windows;
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Run the uninstaller silently.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
run_silent_uninstall(ଅକ୍ଷର *uninst_exe)
{
    ଅକ୍ଷର    vimrt_dir[BUFSIZE];
    ଅକ୍ଷର    temp_uninst[BUFSIZE];
    ଅକ୍ଷର    temp_dir[MAX_PATH];
    ଅକ୍ଷର    buf[BUFSIZE * 2 + 10];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;
    DWORD   tick;

    strcpy(vimrt_dir, uninst_exe);
    remove_tail(vimrt_dir);

    ଯଦି (!GetTempPath(ଆକାର(temp_dir), temp_dir))
	ଫେରନ୍ତୁ FAIL;

    // Copy the uninstaller to a temporary exe.
    tick = Get_Tick_ଗଣନା();
    ପାଇଁ (i = 0; ; i++)
    {
	sprintf(temp_uninst, "%s\\vimun%04X.exe", temp_dir,
					  (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((i + tick) & 0xFFFF));
	ଯଦି (CopyFile(uninst_exe, temp_uninst, TRUE))
	    ଭାଙ୍ଗ;
	ଯଦି (GetLastError() != ERROR_FILE_EXISTS)
	    ଫେରନ୍ତୁ FAIL;
	ଯଦି (i == 65535)
	    ଫେରନ୍ତୁ FAIL;
    }

    // Run the copied uninstaller silently.
    ଯଦି (strchr(temp_uninst, ' ') != ଖାଲି)
	sprintf(buf, "\"%s\" /S _?=%s", temp_uninst, vimrt_dir);
    ଅନ୍ୟଥା
	sprintf(buf, "%s /S _?=%s", temp_uninst, vimrt_dir);
    run_command(buf);

    DeleteFile(temp_uninst);
    ଫେରନ୍ତୁ OK;
}

/*
 * Check for already installed Vims.
 * Return non-zero when found one.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
uninstall_check(ପୂର୍ଣ୍ଣସଂଖ୍ୟା skip_question)
{
    HKEY	ଚାବି_handle;
    HKEY	uninstall_ଚାବି_handle;
    ଅକ୍ଷର	*uninstall_ଚାବି = "software\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
    ଅକ୍ଷର	subkey_name_buff[BUFSIZE];
    ଅକ୍ଷର	temp_string_buffer[BUFSIZE-2];
    DWORD	local_bufsize;
    FILETIME	temp_pfiletime;
    DWORD	ଚାବି_index;
    ଅକ୍ଷର	ନିବେଶ;
    long	code;
    DWORD	ମୂଲ୍ୟ_type;
    DWORD	orig_ଅଙ୍କ_ଚାବିଗୁଡ଼ିକ;
    DWORD	new_ଅଙ୍କ_ଚାବିଗୁଡ଼ିକ;
    DWORD	allow_silent;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		foundone = 0;

    code = Reg_Open_ଚାବି_Ex(HKEY_LOCAL_MACHINE, uninstall_ଚାବି, 0,
				     ଚାବି_WOW64_64_ଚାବି | ଚାବି_READ, &ଚାବି_handle);
    CHECK_REG_ERROR(code);

    ଚାବି_index = 0;
    ଯେତେବେଳେ (TRUE)
    {
	local_bufsize = ଆକାର(subkey_name_buff);
	ଯଦି (Reg_Enum_ଚାବି_Ex(ଚାବି_handle, ଚାବି_index, subkey_name_buff, &local_bufsize,
		ଖାଲି, ଖାଲି, ଖାଲି, &temp_pfiletime) == ERROR_NO_MORE_ବସ୍ତୁଗୁଡ଼ିକ)
	    ଭାଙ୍ଗ;

	ଯଦି (strncmp("Vim", subkey_name_buff, 3) == 0)
	{
	    // Open the key named Vim*
	    code = Reg_Open_ଚାବି_Ex(ଚାବି_handle, subkey_name_buff, 0,
			   ଚାବି_WOW64_64_ଚାବି | ଚାବି_READ, &uninstall_ଚାବି_handle);
	    CHECK_REG_ERROR(code);

	    // get the DisplayName out of it to show the user
	    local_bufsize = ଆକାର(temp_string_buffer);
	    code = Reg_Query_ମୂଲ୍ୟ_Ex(uninstall_ଚାବି_handle, "displayname", 0,
		    &ମୂଲ୍ୟ_type, (LPBYTE)temp_string_buffer,
		    &local_bufsize);
	    CHECK_REG_ERROR(code);

	    allow_silent = 0;
	    ଯଦି (skip_question)
	    {
		DWORD varsize = ଆକାର(DWORD);

		Reg_Query_ମୂଲ୍ୟ_Ex(uninstall_ଚାବି_handle, "AllowSilent", 0,
			&ମୂଲ୍ୟ_type, (LPBYTE)&allow_silent,
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

	    ଯଦି (skip_question)
		printf("\nRunning uninstall program for \"%s\"\n", temp_string_buffer);
	    ଅନ୍ୟଥା
		printf("\nDo you want to uninstall \"%s\" now?\n(y)es/(n)o)  ", temp_string_buffer);
	    fflush(stdout);

	    // get the UninstallString
	    local_bufsize = ଆକାର(temp_string_buffer);
	    code = Reg_Query_ମୂଲ୍ୟ_Ex(uninstall_ଚାବି_handle, "uninstallstring", 0,
		    &ମୂଲ୍ୟ_type, (LPBYTE)temp_string_buffer, &local_bufsize);
	    CHECK_REG_ERROR(code);

	    // Remember the directory, it is used as the default for NSIS.
	    default_vim_dir = alloc(strlen(temp_string_buffer) + 1);
	    strcpy(default_vim_dir, temp_string_buffer);
	    remove_tail(default_vim_dir);
	    remove_tail(default_vim_dir);

	    ନିବେଶ = 'n';
	    କର
	    {
		ଯଦି (ନିବେଶ != 'n')
		    printf("%c is an invalid reply.  Please enter either 'y' or 'n'\n", ନିବେଶ);

		ଯଦି (skip_question)
		    ନିବେଶ = 'y';
		ଅନ୍ୟଥା
		{
		    rewind(stdin);
		    scanf("%c", &ନିବେଶ);
		}
		ବାଛ (ନିବେଶ)
		{
		    ସ୍ଥିତି 'y':
		    ସ୍ଥିତି 'Y':
			// save the number of uninstall keys so we can know if
			// it changed
			Reg_Query_Info_ଚାବି(ଚାବି_handle, ଖାଲି, ଖାଲି, ଖାଲି,
					     &orig_ଅଙ୍କ_ଚାବିଗୁଡ଼ିକ, ଖାଲି, ଖାଲି, ଖାଲି,
						      ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି);

			// Find existing .bat files before deleting them.
			find_bat_exe(TRUE);

			ଯଦି (allow_silent)
			{
			    ଯଦି (run_silent_uninstall(temp_string_buffer)
								    == FAIL)
				allow_silent = 0; // Retry with non silent.
			}
			ଯଦି (!allow_silent)
			{
			    // Execute the uninstall program.  Put it in double
			    // quotes if there is an embedded space.
			    {
				ଅକ୍ଷର buf[BUFSIZE];

				ଯଦି (strchr(temp_string_buffer, ' ') != ଖାଲି)
				    sprintf(buf, "\"%s\"", temp_string_buffer);
				ଅନ୍ୟଥା
				    strcpy(buf, temp_string_buffer);
				run_command(buf);
			    }

			    // Count the number of windows with a title that
			    // match the installer, so that we can check when
			    // it's done.  The uninstaller copies itself,
			    // executes the copy and exits, thus we can't wait
			    // for the process to finish.
			    sleep(1);  // wait for uninstaller to start up
			    ଅଙ୍କ_windows = 0;
			    EnumWindows(window_cb, 0);
			    ଯଦି (ଅଙ୍କ_windows == 0)
			    {
				// Did not find the uninstaller, ask user to
				// press Enter when done. Just in case.
				printf("Press Enter when the uninstaller is finished\n");
				rewind(stdin);
				(ଶୂନ୍ୟ)getchar();
			    }
			    ଅନ୍ୟଥା
			    {
				printf("Waiting for the uninstaller to finish (press CTRL-C to abort).");
				କର
				{
				    printf(".");
				    fflush(stdout);
				    sleep(1);	// wait for the uninstaller to
						// finish
				    ଅଙ୍କ_windows = 0;
				    EnumWindows(window_cb, 0);
				} ଯେତେବେଳେ (ଅଙ୍କ_windows > 0);
			    }
			}
			printf("\nDone!\n");

			// Check if an uninstall reg key was deleted.
			// if it was, we want to decrement key_index.
			// if we don't do this, we will skip the key
			// immediately after any key that we delete.
			Reg_Query_Info_ଚାବି(ଚାବି_handle, ଖାଲି, ଖାଲି, ଖାଲି,
					      &new_ଅଙ୍କ_ଚାବିଗୁଡ଼ିକ, ଖାଲି, ଖାଲି, ଖାଲି,
						      ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି);
			ଯଦି (new_ଅଙ୍କ_ଚାବିଗୁଡ଼ିକ < orig_ଅଙ୍କ_ଚାବିଗୁଡ଼ିକ)
			    ଚାବି_index--;

			ନିବେଶ = 'y';
			ଭାଙ୍ଗ;

		    ସ୍ଥିତି 'n':
		    ସ୍ଥିତି 'N':
			// Do not uninstall
			ନିବେଶ = 'n';
			ଭାଙ୍ଗ;

		    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: // just drop through and redo the loop
			ଭାଙ୍ଗ;
		}

	    } ଯେତେବେଳେ (ନିବେଶ != 'n' && ନିବେଶ != 'y');

	    Reg_Close_ଚାବି(uninstall_ଚାବି_handle);
	}

	ଚାବି_index++;
    }
    Reg_Close_ଚାବି(ଚାବି_handle);

    ଫେରନ୍ତୁ foundone;
}

/*
 * Find out information about the system.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
inspect_system(ଶୂନ୍ୟ)
{
    ଅକ୍ଷର	*p;
    ଅକ୍ଷର	buf[BUFSIZE];
    FILE	*fd;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		foundone;

    // This may take a little while, let the user know what we're doing.
    printf("Inspecting system...\n");

    /*
     * If $VIM is set, check that it's pointing to our directory.
     */
    p = getenv("VIM");
    ଯଦି (p != ଖାଲି && pathcmp(p, -1, installdir, runtimeidx - 1) != 0)
    {
	printf("------------------------------------------------------\n");
	printf("$VIM is set to \"%s\".\n", p);
	printf("This is different from where this version of Vim is:\n");
	strcpy(buf, installdir);
	*(buf + runtimeidx - 1) = NUL;
	printf("\"%s\"\n", buf);
	printf("You must adjust or remove the setting of $VIM,\n");
	ଯଦି (interactive)
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
    ଯଦି (p != ଖାଲି && pathcmp(p, -1, installdir, -1) != 0)
    {
	printf("------------------------------------------------------\n");
	printf("$VIMRUNTIME is set to \"%s\".\n", p);
	printf("This is different from where this version of Vim is:\n");
	printf("\"%s\"\n", installdir);
	printf("You must adjust or remove the setting of $VIMRUNTIME,\n");
	ଯଦି (interactive)
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
    ପାଇଁ (i = 1; i < TARGET_ଗଣନା; ++i)
    {
	findoldfile(&(targets[i].oldexe));
	ଯଦି (targets[i].oldexe != ଖାଲି)
	    foundone = 1;
    }

    ଯଦି (foundone)
    {
	printf("Warning: Found Vim executable(s) in your $PATH:\n");
	ପାଇଁ (i = 1; i < TARGET_ଗଣନା; ++i)
	    ଯଦି (targets[i].oldexe != ଖାଲି)
		printf("%s\n", targets[i].oldexe);
	printf("It will be used instead of the version you are installing.\n");
	printf("Please delete or rename it, or adjust your $PATH setting.\n");
    }

    /*
     * Check if there is an existing ../_vimrc or ../.vimrc file.
     */
    strcpy(oldvimrc, installdir);
    strcpy(oldvimrc + runtimeidx, "_vimrc");
    ଯଦି ((fd = fopen(oldvimrc, "r")) == ଖାଲି)
    {
	strcpy(oldvimrc + runtimeidx, "vimrc~1"); // short version of .vimrc
	ଯଦି ((fd = fopen(oldvimrc, "r")) == ଖାଲି)
	{
	    strcpy(oldvimrc + runtimeidx, ".vimrc");
	    fd = fopen(oldvimrc, "r");
	}
    }
    ଯଦି (fd != ଖାଲି)
	fclose(fd);
    ଅନ୍ୟଥା
	*oldvimrc = NUL;
}

/*
 * Add a dummy choice to avoid that the numbering changes depending on items
 * in the environment.  The user may type a number he remembered without
 * looking.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
add_dummy_choice(ଶୂନ୍ୟ)
{
    choices[choice_ଗଣନା].installfunc = ଖାଲି;
    choices[choice_ଗଣନା].active = 0;
    choices[choice_ଗଣନା].changefunc = ଖାଲି;
    choices[choice_ଗଣନା].text = ଖାଲି;
    choices[choice_ଗଣନା].arg = 0;
    ++choice_ଗଣନା;
}

////////////////////////////////////////////////
// stuff for creating the batch files.

/*
 * Install the vim.bat, gvim.bat, etc. files.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
install_bat_choice(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଅକ୍ଷର	*batpath = targets[choices[idx].arg].batpath;
    ଅକ୍ଷର	*oldname = targets[choices[idx].arg].oldbat;
    ଅକ୍ଷର	*exename = targets[choices[idx].arg].exenamearg;
    ଅକ୍ଷର	*vimarg = targets[choices[idx].arg].exearg;
    FILE	*fd;

    ଯଦି (*batpath == NUL)
	ଫେରନ୍ତୁ;

    fd = fopen(batpath, "w");
    ଯଦି (fd == ଖାଲି)
    {
	printf("\nERROR: Cannot open \"%s\" for writing.\n", batpath);
	ଫେରନ୍ତୁ;
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

    ଯଦି (*exename == 'g')
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

    ଯଦି (*exename == 'g')
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
    ଅନ୍ୟଥା
    {
	// Always use quotes, $VIM or $VIMRUNTIME might have a space.
	fprintf(fd, "\"%%VIM_EXE_DIR%%\\%s\" %s %%*\n",
		exename, vimarg);
    }

    fclose(fd);
    printf("%s has been %s\n", batpath,
	    oldname == ଖାଲି ? "created" : "overwritten");
}

/*
 * Make the text string for choice "idx".
 * The format "fmt" is must have one %s item, which "arg" is used for.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
alloc_text(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ଅକ୍ଷର *fmt, ଅକ୍ଷର *arg)
{
    ଯଦି (choices[idx].text != ଖାଲି)
	ମୁକ୍ତ(choices[idx].text);

    choices[idx].text = alloc(strlen(fmt) + strlen(arg) - 1);
    sprintf(choices[idx].text, fmt, arg);
}

/*
 * Toggle the "Overwrite .../vim.bat" to "Don't overwrite".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
toggle_bat_choice(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଅକ୍ଷର	*batname = targets[choices[idx].arg].batpath;
    ଅକ୍ଷର	*oldname = targets[choices[idx].arg].oldbat;

    ଯଦି (*batname == NUL)
    {
	alloc_text(idx, "    Overwrite %s", oldname);
	strcpy(batname, oldname);
    }
    ଅନ୍ୟଥା
    {
	alloc_text(idx, "    Do NOT overwrite %s", oldname);
	*batname = NUL;
    }
}

/*
 * Do some work for a batch file entry: Append the batch file name to the path
 * and set the text for the choice.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_bat_text(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ଅକ୍ଷର *batpath, ଅକ୍ଷର *name)
{
    strcat(batpath, name);

    alloc_text(idx, "    Create %s", batpath);
}

/*
 * Select a directory to write the batch file line.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
change_bat_choice(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଅକ୍ଷର	*path;
    ଅକ୍ଷର	*batpath;
    ଅକ୍ଷର	*name;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    ଅକ୍ଷର	*s;
    ଅକ୍ଷର	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2;
    ଅକ୍ଷର	**names = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		target = choices[idx].arg;

    name = targets[target].batname;
    batpath = targets[target].batpath;

    path = getenv("PATH");
    ଯଦି (path == ଖାଲି)
    {
	printf("\nERROR: The variable $PATH is not set\n");
	ଫେରନ୍ତୁ;
    }

    /*
     * first round: count number of names in path;
     * second round: save names to names[].
     */
    ପାଇଁ (;;)
    {
	ଗଣନା_2 = 1;
	ପାଇଁ (p = path; *p; )
	{
	    s = strchr(p, ';');
	    ଯଦି (s == ଖାଲି)
		s = p + strlen(p);
	    ଯଦି (names != ଖାଲି)
	    {
		names[ଗଣନା_2] = alloc(s - p + 1);
		strncpy(names[ଗଣନା_2], p, s - p);
		names[ଗଣନା_2][s - p] = NUL;
	    }
	    ++ଗଣନା_2;
	    p = s;
	    ଯଦି (*p != NUL)
		++p;
	}
	ଯଦି (names != ଖାଲି)
	    ଭାଙ୍ଗ;
	names = alloc((ଗଣନା_2 + 1) * ଆକାର(ଅକ୍ଷର *));
    }
    names[0] = alloc(50);
    sprintf(names[0], "Select directory to create %s in:", name);
    names[ଗଣନା_2] = alloc(50);
    ଯଦି (choices[idx].arg == 0)
	sprintf(names[ଗଣନା_2], "Do not create any .bat file.");
    ଅନ୍ୟଥା
	sprintf(names[ଗଣନା_2], "Do not create a %s file.", name);
    n = get_choice(names, ଗଣନା_2 + 1);

    ଯଦି (n == ଗଣନା_2)
    {
	// Selected last item, don't create bat file.
	*batpath = NUL;
	ଯଦି (choices[idx].arg != 0)
	    alloc_text(idx, "    Do NOT create %s", name);
    }
    ଅନ୍ୟଥା
    {
	// Selected one of the paths.  For the first item only keep the path,
	// for the others append the batch file name.
	strcpy(batpath, names[n]);
	add_pathsep(batpath);
	ଯଦି (choices[idx].arg != 0)
	    set_bat_text(idx, batpath, name);
    }

    ପାଇଁ (i = 0; i <= ଗଣନା_2; ++i)
	ମୁକ୍ତ(names[i]);
    ମୁକ୍ତ(names);
}

ଅକ୍ଷର *bat_text_yes = "Install .bat files to use Vim at the command line:";
ଅକ୍ଷର *bat_text_no = "do NOT install .bat files to use Vim at the command line";

    ସ୍ଥିର ଶୂନ୍ୟ
change_main_bat_choice(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    // let the user select a default directory or NONE
    change_bat_choice(idx);

    ଯଦି (targets[0].batpath[0] != NUL)
	choices[idx].text = bat_text_yes;
    ଅନ୍ୟଥା
	choices[idx].text = bat_text_no;

    // update the individual batch file selections
    ପାଇଁ (i = 1; i < TARGET_ଗଣନା; ++i)
    {
	// Only make it active when the first item has a path and the vim.exe
	// or gvim.exe exists (there is a changefunc then).
	ଯଦି (targets[0].batpath[0] != NUL
		&& choices[idx + i].changefunc != ଖାଲି)
	{
	    choices[idx + i].active = 1;
	    ଯଦି (choices[idx + i].changefunc == change_bat_choice
		    && targets[i].batpath[0] != NUL)
	    {
		strcpy(targets[i].batpath, targets[0].batpath);
		set_bat_text(idx + i, targets[i].batpath, targets[i].batname);
	    }
	}
	ଅନ୍ୟଥା
	    choices[idx + i].active = 0;
    }
}

/*
 * Initialize a choice for creating a batch file.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
init_bat_choice(ପୂର୍ଣ୍ଣସଂଖ୍ୟା target)
{
    ଅକ୍ଷର	*batpath = targets[target].batpath;
    ଅକ୍ଷର	*oldbat = targets[target].oldbat;
    ଅକ୍ଷର	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    choices[choice_ଗଣନା].arg = target;
    choices[choice_ଗଣନା].installfunc = install_bat_choice;
    choices[choice_ଗଣନା].active = 1;
    choices[choice_ଗଣନା].text = ଖାଲି;	// will be set below
    ଯଦି (oldbat != ଖାଲି)
    {
	// A [g]vim.bat exists: Only choice is to overwrite it or not.
	choices[choice_ଗଣନା].changefunc = toggle_bat_choice;
	*batpath = NUL;
	toggle_bat_choice(choice_ଗଣନା);
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (default_bat_dir != ଖାଲି)
	    // Prefer using the same path as an existing .bat file.
	    strcpy(batpath, default_bat_dir);
	ଅନ୍ୟଥା
	{
	    // No [g]vim.bat exists: Write it to a directory in $PATH.  Use
	    // $WINDIR by default, if it's empty the first item in $PATH.
	    p = getenv("WINDIR");
	    ଯଦି (p != ଖାଲି && *p != NUL)
		strcpy(batpath, p);
	    ଅନ୍ୟଥା
	    {
		p = getenv("PATH");
		ଯଦି (p == ଖାଲି || *p == NUL)		// "cannot happen"
		    strcpy(batpath, "C:/Windows");
		ଅନ୍ୟଥା
		{
		    i = 0;
		    ଯେତେବେଳେ (*p != NUL && *p != ';')
			batpath[i++] = *p++;
		    batpath[i] = NUL;
		}
	    }
	}
	add_pathsep(batpath);
	set_bat_text(choice_ଗଣନା, batpath, targets[target].batname);

	choices[choice_ଗଣନା].changefunc = change_bat_choice;
    }
    ++choice_ଗଣନା;
}

/*
 * Set up the choices for installing .bat files.
 * For these items "arg" is the index in targets[].
 */
    ସ୍ଥିର ଶୂନ୍ୟ
init_bat_choices(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    // The first item is used to switch installing batch files on/off and
    // setting the default path.
    choices[choice_ଗଣନା].text = bat_text_yes;
    choices[choice_ଗଣନା].changefunc = change_main_bat_choice;
    choices[choice_ଗଣନା].installfunc = ଖାଲି;
    choices[choice_ଗଣନା].active = 1;
    choices[choice_ଗଣନା].arg = 0;
    ++choice_ଗଣନା;

    // Add items for each batch file target.  Only used when not disabled by
    // the first item.  When a .exe exists, don't offer to create a .bat.
    ପାଇଁ (i = 1; i < TARGET_ଗଣନା; ++i)
	ଯଦି (targets[i].oldexe == ଖାଲି
		&& (targets[i].exenamearg[0] == 'g' ? has_gvim : has_vim))
	    init_bat_choice(i);
	ଅନ୍ୟଥା
	    add_dummy_choice();
}

/*
 * Install the vimrc file.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
install_vimrc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx UNUSED)
{
    FILE	*fd, *tfd;
    ଅକ୍ଷର	*fname;

    // If an old vimrc file exists, overwrite it.
    // Otherwise create a new one.
    ଯଦି (*oldvimrc != NUL)
	fname = oldvimrc;
    ଅନ୍ୟଥା
	fname = vimrc;

    fd = fopen(fname, "w");
    ଯଦି (fd == ଖାଲି)
    {
	printf("\nERROR: Cannot open \"%s\" for writing.\n", fname);
	ଫେରନ୍ତୁ;
    }
    ବାଛ (compat_choice)
    {
	ସ୍ଥିତି compat_vi:
		    fprintf(fd, "\" Vi compatible\n");
		    fprintf(fd, "set compatible\n");
		    ଭାଙ୍ଗ;
	ସ୍ଥିତି compat_vim:
		    fprintf(fd, "\" Vim's default behavior\n");
		    fprintf(fd, "if &compatible\n");
		    fprintf(fd, "  set nocompatible\n");
		    fprintf(fd, "endif\n");
		    ଭାଙ୍ଗ;
	ସ୍ଥିତି compat_some_enhancements:
		    fprintf(fd, "\" Vim with some enhancements\n");
		    fprintf(fd, "source $VIMRUNTIME/defaults.vim\n");
		    ଭାଙ୍ଗ;
	ସ୍ଥିତି compat_all_enhancements:
		    fprintf(fd, "\" Vim with all enhancements\n");
		    fprintf(fd, "source $VIMRUNTIME/vimrc_example.vim\n");
		    ଭାଙ୍ଗ;
    }
    ବାଛ (remap_choice)
    {
	ସ୍ଥିତି remap_no:
		    ଭାଙ୍ଗ;
	ସ୍ଥିତି remap_win:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Remap a few keys for Windows behavior\n");
		    fprintf(fd, "source $VIMRUNTIME/mswin.vim\n");
		    ଭାଙ୍ଗ;
    }
    ବାଛ (mouse_choice)
    {
	ସ୍ଥିତି mouse_xterm:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Mouse behavior (the Unix way)\n");
		    fprintf(fd, "behave xterm\n");
		    ଭାଙ୍ଗ;
	ସ୍ଥିତି mouse_mswin:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Mouse behavior (the Windows way)\n");
		    fprintf(fd, "behave mswin\n");
		    ଭାଙ୍ଗ;
	ସ୍ଥିତି mouse_default:
		    ଭାଙ୍ଗ;
    }
    ଯଦି ((tfd = fopen("diff.exe", "r")) != ଖାଲି)
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

    ସ୍ଥିର ଶୂନ୍ୟ
change_vimrc_choice(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (choices[idx].installfunc != ଖାଲି)
    {
	// Switch to NOT change or create a vimrc file.
	ଯଦି (*oldvimrc != NUL)
	    alloc_text(idx, "Do NOT change startup file %s", oldvimrc);
	ଅନ୍ୟଥା
	    alloc_text(idx, "Do NOT create startup file %s", vimrc);
	choices[idx].installfunc = ଖାଲି;
	choices[idx + 1].active = 0;
	choices[idx + 2].active = 0;
	choices[idx + 3].active = 0;
    }
    ଅନ୍ୟଥା
    {
	// Switch to change or create a vimrc file.
	ଯଦି (*oldvimrc != NUL)
	    alloc_text(idx, "Overwrite startup file %s with:", oldvimrc);
	ଅନ୍ୟଥା
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
    ସ୍ଥିର ଶୂନ୍ୟ
change_run_choice(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    compat_choice = get_choice(compat_choices, TABLE_SIZE(compat_choices));
    alloc_text(idx, compat_text, compat_choices[compat_choice]);
}

/*
 * Change the choice if keys are to be remapped.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
change_remap_choice(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    remap_choice = get_choice(remap_choices, TABLE_SIZE(remap_choices));
    alloc_text(idx, remap_text, remap_choices[remap_choice]);
}

/*
 * Change the choice how to select text.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
change_mouse_choice(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    mouse_choice = get_choice(mouse_choices, TABLE_SIZE(mouse_choices));
    alloc_text(idx, mouse_text, mouse_choices[mouse_choice]);
}

    ସ୍ଥିର ଶୂନ୍ୟ
init_vimrc_choices(ଶୂନ୍ୟ)
{
    // set path for a new _vimrc file (also when not used)
    strcpy(vimrc, installdir);
    strcpy(vimrc + runtimeidx, "_vimrc");

    // Set opposite value and then toggle it by calling change_vimrc_choice()
    ଯଦି (*oldvimrc == NUL)
	choices[choice_ଗଣନା].installfunc = ଖାଲି;
    ଅନ୍ୟଥା
	choices[choice_ଗଣନା].installfunc = install_vimrc;
    choices[choice_ଗଣନା].text = ଖାଲି;
    change_vimrc_choice(choice_ଗଣନା);
    choices[choice_ଗଣନା].changefunc = change_vimrc_choice;
    choices[choice_ଗଣନା].active = 1;
    ++choice_ଗଣନା;

    // default way to run Vim
    alloc_text(choice_ଗଣନା, compat_text, compat_choices[compat_choice]);
    choices[choice_ଗଣନା].changefunc = change_run_choice;
    choices[choice_ଗଣନା].installfunc = ଖାଲି;
    choices[choice_ଗଣନା].active = (*oldvimrc == NUL);
    ++choice_ଗଣନା;

    // Whether to remap keys
    alloc_text(choice_ଗଣନା, remap_text , remap_choices[remap_choice]);
    choices[choice_ଗଣନା].changefunc = change_remap_choice;
    choices[choice_ଗଣନା].installfunc = ଖାଲି;
    choices[choice_ଗଣନା].active = (*oldvimrc == NUL);
    ++choice_ଗଣନା;

    // default way to use the mouse
    alloc_text(choice_ଗଣନା, mouse_text, mouse_choices[mouse_choice]);
    choices[choice_ଗଣନା].changefunc = change_mouse_choice;
    choices[choice_ଗଣନା].installfunc = ଖାଲି;
    choices[choice_ଗଣନା].active = (*oldvimrc == NUL);
    ++choice_ଗଣନା;
}

    ସ୍ଥିର LONG
reg_create_ଚାବି(
    HKEY ମୂଳ,
    ଅଚଳ ଅକ୍ଷର *subkey,
    PHKEY ph_ଚାବି,
    DWORD flag)
{
    DWORD disp;

    *ph_ଚାବି = ଖାଲି;
    ଫେରନ୍ତୁ Reg_Create_ଚାବି_Ex(
		ମୂଳ, subkey,
		0, ଖାଲି, REG_OPTION_NON_VOLATILE,
		flag | ଚାବି_WRITE,
		ଖାଲି, ph_ଚାବି, &disp);
}

    ସ୍ଥିର LONG
reg_set_string_ମୂଲ୍ୟ(
    HKEY h_ଚାବି,
    ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ_name,
    ଅଚଳ ଅକ୍ଷର *data)
{
    ଫେରନ୍ତୁ Reg_Set_ମୂଲ୍ୟ_Ex(h_ଚାବି, ମୂଲ୍ୟ_name, 0, REG_SZ,
				     (LPBYTE)data, (DWORD)(1 + strlen(data)));
}

    ସ୍ଥିର LONG
reg_create_ଚାବି_and_ମୂଲ୍ୟ(
    HKEY h_ମୂଳ_ଚାବି,
    ଅଚଳ ଅକ୍ଷର *subkey,
    ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ_name,
    ଅଚଳ ଅକ୍ଷର *data,
    DWORD flag)
{
    HKEY h_ଚାବି;
    LONG lRet = reg_create_ଚାବି(h_ମୂଳ_ଚାବି, subkey, &h_ଚାବି, flag);

    ଯଦି (ERROR_SUCCESS == lRet)
    {
	lRet = reg_set_string_ମୂଲ୍ୟ(h_ଚାବି, ମୂଲ୍ୟ_name, data);
	Reg_Close_ଚାବି(h_ଚାବି);
    }
    ଫେରନ୍ତୁ lRet;
}

    ସ୍ଥିର LONG
register_inproc_server(
    HKEY h_ମୂଳ_ଚାବି,
    ଅଚଳ ଅକ୍ଷର *clsid,
    ଅଚଳ ଅକ୍ଷର *extname,
    ଅଚଳ ଅକ୍ଷର *module,
    ଅଚଳ ଅକ୍ଷର *threading_model,
    DWORD flag)
{
    CHAR subkey[BUFSIZE];
    LONG lRet;

    sprintf(subkey, "CLSID\\%s", clsid);
    lRet = reg_create_ଚାବି_and_ମୂଲ୍ୟ(h_ମୂଳ_ଚାବି, subkey, ଖାଲି, extname, flag);
    ଯଦି (ERROR_SUCCESS == lRet)
    {
	sprintf(subkey, "CLSID\\%s\\InProcServer32", clsid);
	lRet = reg_create_ଚାବି_and_ମୂଲ୍ୟ(h_ମୂଳ_ଚାବି, subkey, ଖାଲି, module, flag);
	ଯଦି (ERROR_SUCCESS == lRet)
	{
	    lRet = reg_create_ଚାବି_and_ମୂଲ୍ୟ(h_ମୂଳ_ଚାବି, subkey,
					   "ThreadingModel", threading_model, flag);
	}
    }
    ଫେରନ୍ତୁ lRet;
}

    ସ୍ଥିର LONG
register_shellex(
    HKEY h_ମୂଳ_ଚାବି,
    ଅଚଳ ଅକ୍ଷର *clsid,
    ଅଚଳ ଅକ୍ଷର *name,
    ଅଚଳ ଅକ୍ଷର *exe_path,
    DWORD flag)
{
    LONG lRet = reg_create_ଚାବି_and_ମୂଲ୍ୟ(
	    h_ମୂଳ_ଚାବି,
	    "*\\shellex\\ContextMenuHandlers\\gvim",
	    ଖାଲି,
	    clsid,
	    flag);

    ଯଦି (ERROR_SUCCESS == lRet)
    {
	lRet = reg_create_ଚାବି_and_ମୂଲ୍ୟ(
		HKEY_LOCAL_MACHINE,
		"Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved",
		clsid,
		name,
		flag);

	ଯଦି (ERROR_SUCCESS == lRet)
	{
	    lRet = reg_create_ଚାବି_and_ମୂଲ୍ୟ(
		    HKEY_LOCAL_MACHINE,
		    "Software\\Vim\\Gvim",
		    "path",
		    exe_path,
		    flag);
	}
    }
    ଫେରନ୍ତୁ lRet;
}

    ସ୍ଥିର LONG
register_openwith(
    HKEY h_ମୂଳ_ଚାବି,
    ଅଚଳ ଅକ୍ଷର *exe_path,
    DWORD flag)
{
    ଅକ୍ଷର	exe_cmd[BUFSIZE];
    LONG	lRet;

    sprintf(exe_cmd, "\"%s\" \"%%1\"", exe_path);
    lRet = reg_create_ଚାବି_and_ମୂଲ୍ୟ(
	    h_ମୂଳ_ଚାବି,
	    "Applications\\gvim.exe\\shell\\edit\\command",
	    ଖାଲି,
	    exe_cmd,
	    flag);

    ଯଦି (ERROR_SUCCESS == lRet)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
	ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *openwith[] = {
		".htm\\OpenWithList\\gvim.exe",
		".vim\\OpenWithList\\gvim.exe",
		"*\\OpenWithList\\gvim.exe",
	};

	ପାଇଁ (i = 0; ERROR_SUCCESS == lRet && i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAYSIZE(openwith); i++)
	    lRet = reg_create_ଚାବି_and_ମୂଲ୍ୟ(h_ମୂଳ_ଚାବି, openwith[i], ଖାଲି, "", flag);
    }

    ଫେରନ୍ତୁ lRet;
}

    ସ୍ଥିର LONG
register_uninstall(
    HKEY h_ମୂଳ_ଚାବି,
    ଅଚଳ ଅକ୍ଷର *appname,
    ଅଚଳ ଅକ୍ଷର *display_name,
    ଅଚଳ ଅକ୍ଷର *uninstall_string,
    ଅଚଳ ଅକ୍ଷର *display_icon,
    ଅଚଳ ଅକ୍ଷର *display_version,
    ଅଚଳ ଅକ୍ଷର *publisher)
{
    LONG lRet = reg_create_ଚାବି_and_ମୂଲ୍ୟ(h_ମୂଳ_ଚାବି, appname,
			     "DisplayName", display_name, ଚାବି_WOW64_64_ଚାବି);

    ଯଦି (ERROR_SUCCESS == lRet)
	lRet = reg_create_ଚାବି_and_ମୂଲ୍ୟ(h_ମୂଳ_ଚାବି, appname,
		     "UninstallString", uninstall_string, ଚାବି_WOW64_64_ଚାବି);
    ଯଦି (ERROR_SUCCESS == lRet)
	lRet = reg_create_ଚାବି_and_ମୂଲ୍ୟ(h_ମୂଳ_ଚାବି, appname,
		     "DisplayIcon", display_icon, ଚାବି_WOW64_64_ଚାବି);
    ଯଦି (ERROR_SUCCESS == lRet)
	lRet = reg_create_ଚାବି_and_ମୂଲ୍ୟ(h_ମୂଳ_ଚାବି, appname,
		     "DisplayVersion", display_version, ଚାବି_WOW64_64_ଚାବି);
    ଯଦି (ERROR_SUCCESS == lRet)
	lRet = reg_create_ଚାବି_and_ମୂଲ୍ୟ(h_ମୂଳ_ଚାବି, appname,
		     "Publisher", publisher, ଚାବି_WOW64_64_ଚାବି);
    ଫେରନ୍ତୁ lRet;
}

/*
 * Add some entries to the registry:
 * - to add "Edit with Vim" to the context * menu
 * - to add Vim to the "Open with..." list
 * - to uninstall Vim
 */
//ARGSUSED
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
install_registry(ଶୂନ୍ୟ)
{
    LONG	lRet = ERROR_SUCCESS;
    ଅଚଳ ଅକ୍ଷର	*vim_ext_ThreadingModel = "Apartment";
    ଅଚଳ ଅକ୍ଷର	*vim_ext_name = "Vim Shell Extension";
    ଅଚଳ ଅକ୍ଷର	*vim_ext_clsid = "{51EEE242-AD87-11d3-9C1E-0090278BBD99}";
    ଅକ୍ଷର	vim_exe_path[MAX_PATH];
    ଅକ୍ଷର	display_name[BUFSIZE];
    ଅକ୍ଷର	uninstall_string[BUFSIZE];
    ଅକ୍ଷର	icon_string[BUFSIZE];
    ଅକ୍ଷର	version_string[BUFSIZE];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		loop_ଗଣନା = is_64bit_os() ? 2 : 1;
    DWORD	flag;

    sprintf(vim_exe_path, "%s\\gvim.exe", installdir);

    ଯଦି (install_popup)
    {
	ଅକ୍ଷର	    bufg[BUFSIZE];

	printf("Creating \"Edit with Vim\" popup menu entry\n");

	ପାଇଁ (i = 0; i < loop_ଗଣନା; i++)
	{
	    ଯଦି (i == 0)
	    {
		sprintf(bufg, "%s\\" GVIMEXT32_PATH, installdir);
		flag = ଚାବି_WOW64_32_ଚାବି;
	    }
	    ଅନ୍ୟଥା
	    {
		sprintf(bufg, "%s\\" GVIMEXT64_PATH, installdir);
		flag = ଚାବି_WOW64_64_ଚାବି;
	    }

	    lRet = register_inproc_server(
		    HKEY_CLASSES_ମୂଳ, vim_ext_clsid, vim_ext_name,
		    bufg, vim_ext_ThreadingModel, flag);
	    ଯଦି (ERROR_SUCCESS != lRet)
		ଫେରନ୍ତୁ FAIL;
	    lRet = register_shellex(
		    HKEY_CLASSES_ମୂଳ, vim_ext_clsid, vim_ext_name,
		    vim_exe_path, flag);
	    ଯଦି (ERROR_SUCCESS != lRet)
		ଫେରନ୍ତୁ FAIL;
	}
    }

    ଯଦି (install_openwith)
    {
	printf("Creating \"Open with ...\" list entry\n");

	ପାଇଁ (i = 0; i < loop_ଗଣନା; i++)
	{
	    ଯଦି (i == 0)
		flag = ଚାବି_WOW64_32_ଚାବି;
	    ଅନ୍ୟଥା
		flag = ଚାବି_WOW64_64_ଚାବି;

	    lRet = register_openwith(HKEY_CLASSES_ମୂଳ, vim_exe_path, flag);
	    ଯଦି (ERROR_SUCCESS != lRet)
		ଫେରନ୍ତୁ FAIL;
	}
    }

    printf("Creating an uninstall entry\n");
    sprintf(display_name, "Vim " VIM_VERSION_SHORT
#ifdef _M_ARM64
	    " (arm64)"
#ନଚେତଯଦି _M_X64
	    " (x64)"
#endif
	    );

    // For the NSIS installer use the generated uninstaller.
    ଯଦି (interactive)
	sprintf(uninstall_string, "%s\\uninstall.exe", installdir);
    ଅନ୍ୟଥା
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
    ଯଦି (ERROR_SUCCESS != lRet)
	ଫେରନ୍ତୁ FAIL;

    ଫେରନ୍ତୁ OK;
}

    ସ୍ଥିର ଶୂନ୍ୟ
change_popup_choice(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (install_popup == 0)
    {
	choices[idx].text = "Install an entry for Vim in the popup menu for the right\n    mouse button so that you can edit any file with Vim";
	install_popup = 1;
    }
    ଅନ୍ୟଥା
    {
	choices[idx].text = "Do NOT install an entry for Vim in the popup menu for the\n    right mouse button to edit any file with Vim";
	install_popup = 0;
    }
}

/*
 * Only add the choice for the popup menu entry when gvim.exe was found and
 * both gvimext.dll and regedit.exe exist.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
init_popup_choice(ଶୂନ୍ୟ)
{
    ସଂରଚନା stat	st;

    ଯଦି (has_gvim
	    && (stat(GVIMEXT32_PATH, &st) >= 0
		|| stat(GVIMEXT64_PATH, &st) >= 0))
    {
	choices[choice_ଗଣନା].changefunc = change_popup_choice;
	choices[choice_ଗଣନା].installfunc = ଖାଲି;
	choices[choice_ଗଣନା].active = 1;
	change_popup_choice(choice_ଗଣନା);  // set the text
	++choice_ଗଣନା;
    }
    ଅନ୍ୟଥା
	add_dummy_choice();
}

    ସ୍ଥିର ଶୂନ୍ୟ
change_openwith_choice(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (install_openwith == 0)
    {
	choices[idx].text = "Add Vim to the \"Open With...\" list in the popup menu for the right\n    mouse button so that you can edit any file with Vim";
	install_openwith = 1;
    }
    ଅନ୍ୟଥା
    {
	choices[idx].text = "Do NOT add Vim to the \"Open With...\" list in the popup menu for the\n    right mouse button to edit any file with Vim";
	install_openwith = 0;
    }
}

/*
 * Only add the choice for the open-with menu entry when gvim.exe was found
 * and regedit.exe exist.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
init_openwith_choice(ଶୂନ୍ୟ)
{
    ଯଦି (has_gvim)
    {
	choices[choice_ଗଣନା].changefunc = change_openwith_choice;
	choices[choice_ଗଣନା].installfunc = ଖାଲି;
	choices[choice_ଗଣନା].active = 1;
	change_openwith_choice(choice_ଗଣନା);  // set the text
	++choice_ଗଣନା;
    }
    ଅନ୍ୟଥା
	add_dummy_choice();
}

/*
 * Create a shell link.
 *
 * returns 0 on failure, non-zero on successful completion.
 *
 * NOTE:  Currently untested with mingw.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
create_shortcut(
	ଅଚଳ ଅକ୍ଷର *shortcut_name,
	ଅଚଳ ଅକ୍ଷର *iconfile_path,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    iconindex,
	ଅଚଳ ଅକ୍ଷର *shortcut_target,
	ଅଚଳ ଅକ୍ଷର *shortcut_args,
	ଅଚଳ ଅକ୍ଷର *workingdir
	)
{
    IShellLink	    *shelllink_ptr;
    HRESULT	    hres;
    IPersistFile	    *persistfile_ptr;

    // Initialize COM library
    hres = CoInitialize(ଖାଲି);
    ଯଦି (!SUCCEEDED(hres))
    {
	printf("Error:  Could not open the COM library.  Not creating shortcut.\n");
	ଫେରନ୍ତୁ FAIL;
    }

    // Instantiate a COM object for the ShellLink, store a pointer to it
    // in shelllink_ptr.
    hres = CoCreateInstance(&CLSID_ShellLink,
			   ଖାଲି,
			   CLSCTX_INPROC_SERVER,
			   &IID_IShellLink,
			   (ଶୂନ୍ୟ **) &shelllink_ptr);

    ଯଦି (SUCCEEDED(hres)) // If the instantiation was successful...
    {
	// ...Then build a PersistFile interface for the ShellLink so we can
	// save it as a file after we build it.
	hres = shelllink_ptr->lpVtbl->QueryInterface(shelllink_ptr,
		&IID_IPersistFile, (ଶୂନ୍ୟ **) &persistfile_ptr);

	ଯଦି (SUCCEEDED(hres))
	{
	    wchar_t wsz[BUFSIZE];

	    // translate the (possibly) multibyte shortcut filename to windows
	    // Unicode so it can be used as a file name.
	    MultiByteToWideChar(CP_ACP, 0, shortcut_name, -1, wsz, ଆକାର(wsz)/ଆକାର(wsz[0]));

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
	ଅନ୍ୟଥା
	{
	    printf("QueryInterface Error\n");
	    ଫେରନ୍ତୁ FAIL;
	}

	// Return the ShellLink object
	shelllink_ptr->lpVtbl->Release(shelllink_ptr);
    }
    ଅନ୍ୟଥା
    {
	printf("CoCreateInstance Error - hres = %08x\n", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)hres);
	ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Build a path to where we will put a specified link.
 *
 * Return 0 on error, non-zero on success
 */
   ପୂର୍ଣ୍ଣସଂଖ୍ୟା
build_link_name(
	ଅକ୍ଷର	   *link_path,
	ଅଚଳ ଅକ୍ଷର *link_name,
	ଅଚଳ ଅକ୍ଷର *shell_folder_name)
{
    ଅକ୍ଷର	shell_folder_path[MAX_PATH];

    ଯଦି (get_shell_folder_path(shell_folder_path, shell_folder_name) == FAIL)
    {
	printf("An error occurred while attempting to find the path to %s.\n",
							   shell_folder_name);
	ଫେରନ୍ତୁ FAIL;
    }

    // Make sure the directory exists (create Start Menu\Programs\Vim).
    // Ignore errors if it already exists.
    vim_mkdir(shell_folder_path, 0755);

    // build the path to the shortcut and the path to gvim.exe
    sprintf(link_path, "%s\\%s.lnk", shell_folder_path, link_name);

    ଫେରନ୍ତୁ OK;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
build_shortcut(
	ଅଚଳ ଅକ୍ଷର *name,	// Name of the shortcut
	ଅଚଳ ଅକ୍ଷର *exename,	// Name of the executable (e.g., vim.exe)
	ଅଚଳ ଅକ୍ଷର *args,
	ଅଚଳ ଅକ୍ଷର *shell_folder,
	ଅଚଳ ଅକ୍ଷର *workingdir)
{
    ଅକ୍ଷର	executable_path[BUFSIZE];
    ଅକ୍ଷର	link_name[BUFSIZE];

    sprintf(executable_path, "%s\\%s", installdir, exename);

    ଯଦି (build_link_name(link_name, name, shell_folder) == FAIL)
    {
	printf("An error has occurred.  A shortcut to %s will not be created %s.\n",
		name,
		*shell_folder == 'd' ? "on the desktop" : "in the Start menu");
	ଫେରନ୍ତୁ FAIL;
    }

    // Create the shortcut:
    ଫେରନ୍ତୁ create_shortcut(link_name, executable_path, 0,
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
    ସ୍ଥିର ଶୂନ୍ୟ
install_start_menu(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx UNUSED)
{
    need_uninstall_entry = 1;
    printf("Creating start menu\n");
    ଯଦି (has_vim)
    {
	ଯଦି (build_shortcut("Vim", "vim.exe", "",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ଫେରନ୍ତୁ;
	ଯଦି (build_shortcut("Vim Read-only", "vim.exe", "-R",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ଫେରନ୍ତୁ;
	ଯଦି (build_shortcut("Vim Diff", "vim.exe", "-d",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ଫେରନ୍ତୁ;
    }
    ଯଦି (has_gvim)
    {
	ଯଦି (build_shortcut("gVim", "gvim.exe", "",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ଫେରନ୍ତୁ;
	ଯଦି (build_shortcut("gVim Easy", "gvim.exe", "-y",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ଫେରନ୍ତୁ;
	ଯଦି (build_shortcut("gVim Read-only", "gvim.exe", "-R",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ଫେରନ୍ତୁ;
	ଯଦି (build_shortcut("gVim Diff", "gvim.exe", "-d",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ଫେରନ୍ତୁ;
    }
    ଯଦି (build_shortcut("Uninstall",
		interactive ? "uninstall.exe" : "uninstall-gui.exe", "",
					   VIM_STARTMENU, installdir) == FAIL)
	ଫେରନ୍ତୁ;
    // For Windows NT the working dir of the vimtutor.bat must be right,
    // otherwise gvim.exe won't be found and using gvimbat doesn't work.
    ଯଦି (build_shortcut("Vim tutor", "vimtutor.bat", "",
					   VIM_STARTMENU, installdir) == FAIL)
	ଫେରନ୍ତୁ;
    ଯଦି (build_shortcut("Help", has_gvim ? "gvim.exe" : "vim.exe", "-c h",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	ଫେରନ୍ତୁ;
    {
	ଅକ୍ଷର	shell_folder_path[BUFSIZE];

	// Creating the URL shortcut works a bit differently...
	ଯଦି (get_shell_folder_path(shell_folder_path, VIM_STARTMENU) == FAIL)
	{
	    printf("Finding the path of the Start menu failed\n");
	    ଫେରନ୍ତୁ ;
	}
	add_pathsep(shell_folder_path);
	strcat(shell_folder_path, "Vim Online.url");
	ଯଦି (!WritePrivateProfileString("InternetShortcut", "URL",
				    "https://www.vim.org/", shell_folder_path))
	{
	    printf("Creating the Vim online URL failed\n");
	    ଫେରନ୍ତୁ;
	}
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
toggle_startmenu_choice(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (choices[idx].installfunc == ଖାଲି)
    {
	choices[idx].installfunc = install_start_menu;
	choices[idx].text = "Add Vim to the Start menu";
    }
    ଅନ୍ୟଥା
    {
	choices[idx].installfunc = ଖାଲି;
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
    ଶୂନ୍ୟ
install_shortcut_gvim(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    // Create shortcut(s) on the desktop
    ଯଦି (choices[idx].arg)
    {
	(ଶୂନ୍ୟ)build_shortcut(icon_names[0], "gvim.exe",
						      "", "desktop", WORKDIR);
	need_uninstall_entry = 1;
    }
}

    ଶୂନ୍ୟ
install_shortcut_evim(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (choices[idx].arg)
    {
	(ଶୂନ୍ୟ)build_shortcut(icon_names[1], "gvim.exe",
						    "-y", "desktop", WORKDIR);
	need_uninstall_entry = 1;
    }
}

    ଶୂନ୍ୟ
install_shortcut_gview(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (choices[idx].arg)
    {
	(ଶୂନ୍ୟ)build_shortcut(icon_names[2], "gvim.exe",
						    "-R", "desktop", WORKDIR);
	need_uninstall_entry = 1;
    }
}

    ଶୂନ୍ୟ
toggle_shortcut_choice(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଅକ୍ଷର	*arg;

    ଯଦି (choices[idx].installfunc == install_shortcut_gvim)
	arg = "gVim";
    ଅନ୍ୟଥା ଯଦି (choices[idx].installfunc == install_shortcut_evim)
	arg = "gVim Easy";
    ଅନ୍ୟଥା
	arg = "gVim Read-only";
    ଯଦି (choices[idx].arg)
    {
	choices[idx].arg = 0;
	alloc_text(idx, "Do NOT create a desktop icon for %s", arg);
    }
    ଅନ୍ୟଥା
    {
	choices[idx].arg = 1;
	alloc_text(idx, "Create a desktop icon for %s", arg);
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
init_startmenu_choice(ଶୂନ୍ୟ)
{
    // Start menu
    choices[choice_ଗଣନା].changefunc = toggle_startmenu_choice;
    choices[choice_ଗଣନା].installfunc = ଖାଲି;
    choices[choice_ଗଣନା].active = 1;
    toggle_startmenu_choice(choice_ଗଣନା);	// set the text
    ++choice_ଗଣନା;
}

/*
 * Add the choice for the desktop shortcuts.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
init_shortcut_choices(ଶୂନ୍ୟ)
{
    // Shortcut to gvim
    choices[choice_ଗଣନା].text = ଖାଲି;
    choices[choice_ଗଣନା].arg = 0;
    choices[choice_ଗଣନା].active = has_gvim;
    choices[choice_ଗଣନା].changefunc = toggle_shortcut_choice;
    choices[choice_ଗଣନା].installfunc = install_shortcut_gvim;
    toggle_shortcut_choice(choice_ଗଣନା);
    ++choice_ଗଣନା;

    // Shortcut to evim
    choices[choice_ଗଣନା].text = ଖାଲି;
    choices[choice_ଗଣନା].arg = 0;
    choices[choice_ଗଣନା].active = has_gvim;
    choices[choice_ଗଣନା].changefunc = toggle_shortcut_choice;
    choices[choice_ଗଣନା].installfunc = install_shortcut_evim;
    toggle_shortcut_choice(choice_ଗଣନା);
    ++choice_ଗଣନା;

    // Shortcut to gview
    choices[choice_ଗଣନା].text = ଖାଲି;
    choices[choice_ଗଣନା].arg = 0;
    choices[choice_ଗଣନା].active = has_gvim;
    choices[choice_ଗଣନା].changefunc = toggle_shortcut_choice;
    choices[choice_ଗଣନା].installfunc = install_shortcut_gview;
    toggle_shortcut_choice(choice_ଗଣନା);
    ++choice_ଗଣନା;
}

/*
 * Attempt to register OLE for Vim.
 */
   ସ୍ଥିର ଶୂନ୍ୟ
install_OLE_register(ଶୂନ୍ୟ)
{
    ଅକ୍ଷର register_command_string[BUFSIZE + 30];

    printf("\n--- Attempting to register Vim with OLE ---\n");
    printf("(There is no message whether this works or not.)\n");

    sprintf(register_command_string, "\"%s\\gvim.exe\" -silent -register", installdir);
    system(register_command_string);
}

/*
 * Remove the last part of directory "path[]" to get its parent, and put the
 * result in "to[]".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
dir_remove_last(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର to[MAX_PATH])
{
    ଅକ୍ଷର c;
    long last_char_to_copy;
    long path_length = strlen(path);

    // skip the last character just in case it is a '\\'
    last_char_to_copy = path_length - 2;
    c = path[last_char_to_copy];

    ଯେତେବେଳେ (c != '\\')
    {
	last_char_to_copy--;
	c = path[last_char_to_copy];
    }

    strncpy(to, path, (size_t)last_char_to_copy);
    to[last_char_to_copy] = NUL;
}

    ସ୍ଥିର ଶୂନ୍ୟ
set_directories_text(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା vimfiles_dir_choice = choices[idx].arg;

    ଯଦି (vimfiles_dir_choice == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vimfiles_dir_none)
	alloc_text(idx, "Do NOT create plugin directories%s", "");
    ଅନ୍ୟଥା
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
ସ୍ଥିର ଅକ୍ଷର *homedir = ଖାଲି;

    ଶୂନ୍ୟ
init_homedir(ଶୂନ୍ୟ)
{
    ଅକ୍ଷର    *var;
    ଅକ୍ଷର    buf[MAX_PATH];

    ଯଦି (homedir != ଖାଲି)
    {
	ମୁକ୍ତ(homedir);
	homedir = ଖାଲି;
    }

    var = getenv("HOME");

    /*
     * Typically, $HOME is not defined on Windows, unless the user has
     * specifically defined it for Vim's sake.  However, on Windows NT
     * platforms, $HOMEDRIVE and $HOMEPATH are automatically defined for
     * each user.  Try constructing $HOME from these.
     */
    ଯଦି (var == ଖାଲି || *var == NUL)
    {
	ଅକ୍ଷର	*homedrive, *homepath;

	homedrive = getenv("HOMEDRIVE");
	homepath = getenv("HOMEPATH");
	ଯଦି (homepath == ଖାଲି || *homepath == NUL)
	    homepath = "\\";
	ଯଦି (homedrive != ଖାଲି
		   && strlen(homedrive) + strlen(homepath) < ଆକାର(buf))
	{
	    ଯଦି (snprintf(buf, ଆକାର(buf), "%s%s", homedrive, homepath) > 0
		&& buf[0] != NUL)
		var = buf;
	}
    }

    ଯଦି (var == ଖାଲି)
	var = getenv("USERPROFILE");

    /*
     * Weird but true: $HOME may contain an indirect reference to another
     * variable, esp. "%USERPROFILE%".  Happens when $USERPROFILE isn't set
     * when $HOME is being set.
     */
    ଯଦି (var != ଖାଲି && *var == '%')
    {
	ଅକ୍ଷର	*p;
	ଅକ୍ଷର	*exp;

	p = strchr(var + 1, '%');
	ଯଦି (p != ଖାଲି)
	{
	    strncpy(buf, var + 1, p - (var + 1));
	    buf[p - (var + 1)] = NUL;
	    exp = getenv(buf);
	    ଯଦି (exp != ଖାଲି && *exp != NUL
				&& strlen(exp) + strlen(p) < ଆକାର(buf))
	    {
		snprintf(buf, ଆକାର(buf), "%s%s", exp, p + 1);
		var = buf;
	    }
	}
    }

    ଯଦି (var != ଖାଲି && *var == NUL)	// empty is same as not set
	var = ଖାଲି;

    ଯଦି (var == ଖାଲି)
	homedir = ଖାଲି;
    ଅନ୍ୟଥା
	homedir = _strdup(var);
}

/*
 * Change the directory that the vim plugin directories will be created in:
 * $HOME, $VIM or nowhere.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
change_directories_choice(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    choice_ଗଣନା = TABLE_SIZE(vimfiles_dir_choices);

    // Don't offer the $HOME choice if $HOME isn't set.
    ଯଦି (homedir == ଖାଲି)
	--choice_ଗଣନା;
    choices[idx].arg = get_choice(vimfiles_dir_choices, choice_ଗଣନା);
    set_directories_text(idx);
}

/*
 * Create the plugin directories...
 */
//ARGSUSED
    ସ୍ଥିର ଶୂନ୍ୟ
install_vimfilesdir(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା vimfiles_dir_choice = choices[idx].arg;
    ଅକ୍ଷର *p;
    ଅକ୍ଷର vimdir_path[MAX_PATH];
    ଅକ୍ଷର vimfiles_path[MAX_PATH + 9];
    ଅକ୍ଷର tmp_dirname[BUFSIZE];

    // switch on the location that the user wants the plugin directories
    // built in
    ବାଛ (vimfiles_dir_choice)
    {
	ସ୍ଥିତି vimfiles_dir_vim:
	{
	    // Go to the %VIM% directory - check env first, then go one dir
	    //	   below installdir if there is no %VIM% environment variable.
	    //	   The accuracy of $VIM is checked in inspect_system(), so we
	    //	   can be sure it is ok to use here.
	    p = getenv("VIM");
	    ଯଦି (p == ଖାଲି) // No $VIM in path
		dir_remove_last(installdir, vimdir_path);
	    ଅନ୍ୟଥା
		strcpy(vimdir_path, p);
	    ଭାଙ୍ଗ;
	}
	ସ୍ଥିତି vimfiles_dir_home:
	{
	    // Find the $HOME directory.  Its existence was already checked.
	    p = homedir;
	    ଯଦି (p == ଖାଲି)
	    {
		printf("Internal error: $HOME is NULL\n");
		p = "c:\\";
	    }
	    strcpy(vimdir_path, p);
	    ଭାଙ୍ଗ;
	}
	ସ୍ଥିତି vimfiles_dir_none:
	{
	    // Do not create vim plugin directory.
	    ଫେରନ୍ତୁ;
	}
    }

    // Now, just create the directory.	If it already exists, it will fail
    // silently.
    sprintf(vimfiles_path, "%s\\vimfiles", vimdir_path);
    vim_mkdir(vimfiles_path, 0755);

    printf("Creating the following directories in \"%s\":\n", vimfiles_path);
    ପାଇଁ (i = 0; i < TABLE_SIZE(vimfiles_subdirs); i++)
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
    ସ୍ଥିର ଶୂନ୍ୟ
init_directories_choice(ଶୂନ୍ୟ)
{
    ସଂରଚନା stat	st;
    ଅକ୍ଷର	tmp_dirname[BUFSIZE];
    ଅକ୍ଷର	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vimfiles_dir_choice;

    choices[choice_ଗଣନା].text = alloc(150);
    choices[choice_ଗଣନା].changefunc = change_directories_choice;
    choices[choice_ଗଣନା].installfunc = install_vimfilesdir;
    choices[choice_ଗଣନା].active = 1;

    // Check if the "compiler" directory already exists.  That's a good
    // indication that the plugin directories were already created.
    p = getenv("HOME");
    ଯଦି (p != ଖାଲି)
    {
	vimfiles_dir_choice = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vimfiles_dir_home;
	sprintf(tmp_dirname, "%s\\vimfiles\\compiler", p);
	ଯଦି (stat(tmp_dirname, &st) == 0)
	    vimfiles_dir_choice = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vimfiles_dir_none;
    }
    ଅନ୍ୟଥା
    {
	vimfiles_dir_choice = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vimfiles_dir_vim;
	p = getenv("VIM");
	ଯଦି (p == ଖାଲି)  // No $VIM in path, use the install dir.
	    dir_remove_last(installdir, tmp_dirname);
	ଅନ୍ୟଥା
	    strcpy(tmp_dirname, p);
	strcat(tmp_dirname, "\\vimfiles\\compiler");
	ଯଦି (stat(tmp_dirname, &st) == 0)
	    vimfiles_dir_choice = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vimfiles_dir_none;
    }

    choices[choice_ଗଣନା].arg = vimfiles_dir_choice;
    set_directories_text(choice_ଗଣନା);
    ++choice_ଗଣନା;
}

/*
 * Setup the choices and the default values.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
setup_choices(ଶୂନ୍ୟ)
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

    ସ୍ଥିର ଶୂନ୍ୟ
print_cmd_ଧାଡ଼ି_help(ଶୂନ୍ୟ)
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
    ସ୍ଥିର ଶୂନ୍ୟ
command_ଧାଡ଼ି_setup_choices(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର **argv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, j;

    ପାଇଁ (i = 1; i < argc; i++)
    {
	ଯଦି (strcmp(argv[i], "-create-batfiles") == 0)
	{
	    ଯଦି (i + 1 == argc)
		ଜାରି;
	    ଯେତେବେଳେ (argv[i + 1][0] != '-' && i < argc)
	    {
		i++;
		ପାଇଁ (j = 1; j < TARGET_ଗଣନା; ++j)
		    ଯଦି ((targets[j].exenamearg[0] == 'g' ? has_gvim : has_vim)
			    && strcmp(argv[i], targets[j].name) == 0)
		    {
			init_bat_choice(j);
			ଭାଙ୍ଗ;
		    }
		ଯଦି (j == TARGET_ଗଣନା)
		    printf("%s is not a valid choice for -create-batfiles\n",
								     argv[i]);

		ଯଦି (i + 1 == argc)
		    ଭାଙ୍ଗ;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (strcmp(argv[i], "-create-vimrc") == 0)
	{
	    // Setup default vimrc choices.  If there is already a _vimrc file,
	    // it will NOT be overwritten.
	    init_vimrc_choices();
	}
	ଅନ୍ୟଥା ଯଦି (strcmp(argv[i], "-vimrc-remap") == 0)
	{
	    ଯଦି (i + 1 == argc)
		ଭାଙ୍ଗ;
	    i++;
	    ଯଦି (strcmp(argv[i], "no") == 0)
		remap_choice = remap_no;
	    ଅନ୍ୟଥା ଯଦି (strcmp(argv[i], "win") == 0)
		remap_choice = remap_win;
	}
	ଅନ୍ୟଥା ଯଦି (strcmp(argv[i], "-vimrc-behave") == 0)
	{
	    ଯଦି (i + 1 == argc)
		ଭାଙ୍ଗ;
	    i++;
	    ଯଦି (strcmp(argv[i], "unix") == 0)
		mouse_choice = mouse_xterm;
	    ଅନ୍ୟଥା ଯଦି (strcmp(argv[i], "mswin") == 0)
		mouse_choice = mouse_mswin;
	    ଅନ୍ୟଥା ଯଦି (strcmp(argv[i], "default") == 0)
		mouse_choice = mouse_default;
	}
	ଅନ୍ୟଥା ଯଦି (strcmp(argv[i], "-vimrc-compat") == 0)
	{
	    ଯଦି (i + 1 == argc)
		ଭାଙ୍ଗ;
	    i++;
	    ଯଦି (strcmp(argv[i], "vi") == 0)
		compat_choice = compat_vi;
	    ଅନ୍ୟଥା ଯଦି (strcmp(argv[i], "vim") == 0)
		compat_choice = compat_vim;
	    ଅନ୍ୟଥା ଯଦି (strcmp(argv[i], "defaults") == 0)
		compat_choice = compat_some_enhancements;
	    ଅନ୍ୟଥା ଯଦି (strcmp(argv[i], "all") == 0)
		compat_choice = compat_all_enhancements;
	}
	ଅନ୍ୟଥା ଯଦି (strcmp(argv[i], "-install-popup") == 0)
	{
	    init_popup_choice();
	}
	ଅନ୍ୟଥା ଯଦି (strcmp(argv[i], "-install-openwith") == 0)
	{
	    init_openwith_choice();
	}
	ଅନ୍ୟଥା ଯଦି (strcmp(argv[i], "-add-start-menu") == 0)
	{
	    init_startmenu_choice();
	}
	ଅନ୍ୟଥା ଯଦି (strcmp(argv[i], "-install-icons") == 0)
	{
	    init_shortcut_choices();
	}
	ଅନ୍ୟଥା ଯଦି (strcmp(argv[i], "-create-directories") == 0)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା vimfiles_dir_choice = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vimfiles_dir_none;

	    init_directories_choice();
	    ଯଦି (i + 1 < argc && argv[i + 1][0] != '-')
	    {
		i++;
		ଯଦି (strcmp(argv[i], "vim") == 0)
		    vimfiles_dir_choice = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vimfiles_dir_vim;
		ଅନ୍ୟଥା ଯଦି (strcmp(argv[i], "home") == 0)
		{
		    ଯଦି (homedir == ଖାଲି)  // No $HOME in environment
			vimfiles_dir_choice = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vimfiles_dir_none;
		    ଅନ୍ୟଥା
			vimfiles_dir_choice = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vimfiles_dir_home;
		}
		ଅନ୍ୟଥା
		{
		    printf("Unknown argument for -create-directories: %s\n",
								     argv[i]);
		    print_cmd_ଧାଡ଼ି_help();
		}
	    }
	    ଅନ୍ୟଥା // No choice specified, default to vim directory
		vimfiles_dir_choice = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vimfiles_dir_vim;
	    choices[choice_ଗଣନା - 1].arg = vimfiles_dir_choice;
	}
	ଅନ୍ୟଥା ଯଦି (strcmp(argv[i], "-register-OLE") == 0)
	{
	    // This is always done when gvim is found
	}
	ଅନ୍ୟଥା // Unknown switch
	{
	    printf("Got unknown argument argv[%d] = %s\n", i, argv[i]);
	    print_cmd_ଧାଡ଼ି_help();
	}
    }
}


/*
 * Show a few screens full of helpful information.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
show_help(ଶୂନ୍ୟ)
{
    ସ୍ଥିର ଅକ୍ଷର *(ବସ୍ତୁଗୁଡ଼ିକ[]) =
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
ଖାଲି
    };
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;

    rewind(stdin);
    printf("\n");
    ପାଇଁ (i = 0; ବସ୍ତୁଗୁଡ଼ିକ[i] != ଖାଲି; ++i)
    {
	puts(ବସ୍ତୁଗୁଡ଼ିକ[i]);
	printf("Hit Enter to continue, b (back) or q (quit help): ");
	c = getchar();
	rewind(stdin);
	ଯଦି (c == 'b' || c == 'B')
	{
	    ଯଦି (i == 0)
		--i;
	    ଅନ୍ୟଥା
		i -= 2;
	}
	ଯଦି (c == 'q' || c == 'Q')
	    ଭାଙ୍ଗ;
	printf("\n");
    }
}

/*
 * Install the choices.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
install(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    // Install the selected choices.
    ପାଇଁ (i = 0; i < choice_ଗଣନା; ++i)
	ଯଦି (choices[i].installfunc != ଖାଲି && choices[i].active)
	    (choices[i].installfunc)(i);

    // Add some entries to the registry, if needed.
    ଯଦି (install_popup
	    || install_openwith
	    || (need_uninstall_entry && interactive)
	    || !interactive)
	install_registry();

    // Register gvim with OLE.
    ଯଦି (has_gvim)
	install_OLE_register();
}

/*
 * request_choice
 */
    ସ୍ଥିର ଶୂନ୍ୟ
request_choice(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		      i;

    printf("\n\nInstall will do for you:\n");
    ପାଇଁ (i = 0; i < choice_ଗଣନା; ++i)
	ଯଦି (choices[i].active)
	    printf("%2d  %s\n", i + 1, choices[i].text);
    printf("To change an item, enter its number\n\n");
    printf("Enter item number, h (help), d (do it) or q (quit): ");
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ମୁଖ୍ୟ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର **argv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ଅକ୍ଷର	buf[BUFSIZE];

    /*
     * Run interactively if there are no command line arguments.
     */
    ଯଦି (argc > 1)
	interactive = 0;
    ଅନ୍ୟଥା
	interactive = 1;

    // Initialize this program.
    do_inits(argv);
    init_homedir();

    ଯଦି (argc > 1 && strcmp(argv[1], "-uninstall-check") == 0)
    {
	// Only check for already installed Vims.  Used by NSIS installer.
	i = uninstall_check(1);

	// Find the value of $VIM, because NSIS isn't able to do this by
	// itself.
	get_vim_env();

	// When nothing found exit quietly.  If something found wait for
	// a little while, so that the user can read the messages.
	ଯଦି (i && _isatty(1))
	    sleep(3);
	exit(0);
    }

    printf("This program sets up the installation of Vim "
						   VIM_VERSION_MEDIUM "\n\n");

    // Check if the user unpacked the archives properly.
    check_unpack();

    // Check for already installed Vims.
    ଯଦି (interactive)
	uninstall_check(0);

    // Find out information about the system.
    inspect_system();

    ଯଦି (interactive)
    {
	// Setup all the choices.
	setup_choices();

	// Let the user change choices and finally install (or quit).
	ପାଇଁ (;;)
	{
	    request_choice();
	    rewind(stdin);
	    ଯଦି (scanf("%99s", buf) == 1)
	    {
		ଯଦି (isdigit((unsigned ଅକ୍ଷର)buf[0]))
		{
		    // Change a choice.
		    i = atoi(buf);
		    ଯଦି (i > 0 && i <= choice_ଗଣନା && choices[i - 1].active)
			(choices[i - 1].changefunc)(i - 1);
		    ଅନ୍ୟଥା
			printf("\nIllegal choice\n");
		}
		ଅନ୍ୟଥା ଯଦି (buf[0] == 'h' || buf[0] == 'H')
		{
		    // Help
		    show_help();
		}
		ଅନ୍ୟଥା ଯଦି (buf[0] == 'd' || buf[0] == 'D')
		{
		    // Install!
		    install();
		    printf("\nThat finishes the installation.  Happy Vimming!\n");
		    ଭାଙ୍ଗ;
		}
		ଅନ୍ୟଥା ଯଦି (buf[0] == 'q' || buf[0] == 'Q')
		{
		    // Quit
		    printf("\nExiting without anything done\n");
		    ଭାଙ୍ଗ;
		}
		ଅନ୍ୟଥା
		    printf("\nIllegal choice\n");
	    }
	}
	printf("\n");
	myexit(0);
    }
    ଅନ୍ୟଥା
    {
	/*
	 * Run non-interactive - setup according to the command line switches
	 */
	command_ଧାଡ଼ି_setup_choices(argc, argv);
	install();

	// Avoid that the user has to hit Enter, just wait a little bit to
	// allow reading the messages.
	sleep(2);
    }

    ଫେରନ୍ତୁ 0;
}
