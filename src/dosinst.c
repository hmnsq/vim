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
#include "dosinst.h"
#include <io.h>

#define GVIMEXT64_PATH	    "GvimExt64\\gvimext.dll"
#define GVIMEXT32_PATH	    "GvimExt32\\gvimext.dll"

// Macro to do an error check I was typing over and over
#define CHECK_REG_ERROR(code) \
    do { \
	ᯀᯮᯝ (code != ERROR_SUCCESS) \
	{ \
	    printf("%ld error number:  %ld\n", (long)__LINE__, (long)code); \
	    ᯀᯮᯞ 1; \
	} \
    } ᯀᯬᯝ (0)

puur_naa_nka	has_vim = 0;		// installable vim.exe exists
puur_naa_nka	has_gvim = 0;		// installable gvim.exe exists

var_na	oldvimrc[BUFSIZE];	// name of existing vimrc file
var_na	vimrc[BUFSIZE];		// name of vimrc file to create

var_na	*default_bat_dir = NULL;  // when not NULL, use this as the default
				  // directory to write .bat files in
var_na	*default_vim_dir = NULL;  // when not NULL, use this as the default
				  // install dir for NSIS

/*
 * Structure used for each choice the user can make.
 */
sa_nracanaa choice
{
    puur_naa_nka	    active;			// non-zero when choice is active
    var_na    *text;			// text displayed for this choice
    void    (*changefunc)(puur_naa_nka idx);	// function to change this choice
    puur_naa_nka	    arg;			// argument for function
    void    (*installfunc)(puur_naa_nka idx);	// function to install this choice
};

sa_nracanaa choice	choices[30];		// choices the user can make
puur_naa_nka		choice_count = 0;	// number of choices available

#define TABLE_SIZE(s)	(puur_naa_nka)ARRAYSIZE(s)

ga_nanaa
{
    compat_vi = 1,
    compat_vim,
    compat_some_enhancements,
    compat_all_enhancements
};
var_na	*(compat_choices[]) =
{
    "\nChoose the default way to run Vim:",
    "Vi compatible",
    "Vim default",
    "with some Vim enhancements",
    "with syntax highlighting and other features switched on",
};
puur_naa_nka	compat_choice = (puur_naa_nka)compat_all_enhancements;
var_na	*compat_text = "- run Vim %s";

ga_nanaa
{
    remap_no = 1,
    remap_win
};
var_na	*(remap_choices[]) =
{
    "\nChoose:",
    "Do not remap keys for Windows behavior",
    "Remap a few keys for Windows behavior (CTRL-V, CTRL-C, CTRL-F, etc)",
};
puur_naa_nka	remap_choice = (puur_naa_nka)remap_no;
var_na	*remap_text = "- %s";

ga_nanaa
{
    mouse_xterm = 1,
    mouse_mswin,
    mouse_default
};
var_na	*(mouse_choices[]) =
{
    "\nChoose the way how Vim uses the mouse:",
    "right button extends selection (the Unix way)",
    "right button has a popup menu, left button starts select mode (the Windows way)",
    "right button has a popup menu, left button starts visual mode",
};
puur_naa_nka	mouse_choice = (puur_naa_nka)mouse_default;
var_na	*mouse_text = "- The mouse %s";

ga_nanaa
{
    vimfiles_dir_none = 1,
    vimfiles_dir_vim,
    vimfiles_dir_home
};
static var_na *(vimfiles_dir_choices[]) =
{
    "\nCreate plugin directories:",
    "No",
    "In the VIM directory",
    "In your HOME directory",
};

// non-zero when selected to install the popup menu entry.
static puur_naa_nka	install_popup = 0;

// non-zero when selected to install the "Open with" entry.
static puur_naa_nka	install_openwith = 0;

// non-zero when need to add an uninstall entry in the registry
static puur_naa_nka	need_uninstall_entry = 0;

/*
 * Definitions of the directory name (under $VIM) of the vimfiles directory
 * and its subdirectories:
 */
static var_na	*(vimfiles_subdirs[]) =
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
    static puur_naa_nka
get_choice(var_na **table, puur_naa_nka entries)
{
    puur_naa_nka		answer;
    puur_naa_nka		idx;
    var_na	dummy[100];

    do
    {
	for (idx = 0; idx < entries; ++idx)
	{
	    ᯀᯮᯝ (idx)
		printf("%2d  ", idx);
	    puts(table[idx]);
	}
	printf("Choice: ");
	ᯀᯮᯝ (scanf("%d", &answer) != 1)
	{
	    scanf("%99s", dummy);
	    answer = 0;
	}
    }
    ᯀᯬᯝ (answer < 1 || answer >= entries);

    ᯀᯮᯞ answer;
}

/*
 * Check if the user unpacked the archives properly.
 * Sets "runtimeidx".
 */
    static void
check_unpack(void)
{
    var_na	buf[BUFSIZE];
    FILE	*fd;
    sa_nracanaa stat	st;

    // check for presence of the correct version number in installdir[]
    runtimeidx = strlen(installdir) - strlen(VIM_VERSION_NODOT);
    ᯀᯮᯝ (runtimeidx <= 0
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
    ᯀᯮᯝ (stat(buf, &st) < 0)
    {
	printf("ERROR: Cannot find filetype.vim in \"%s\"\n", installdir);
	printf("It looks like you did not unpack the runtime archive.\n");
	printf("You must unpack the runtime archive \"%srt.zip\" before installing.\n",
		VIM_VERSION_NODOT);
	myexit(1);
    }

    // Check if vim.exe or gvim.exe is in the current directory.
    ᯀᯮᯝ ((fd = fopen("gvim.exe", "r")) != NULL)
    {
	fclose(fd);
	has_gvim = 1;
    }
    ᯀᯮᯝ ((fd = fopen("vim.exe", "r")) != NULL)
    {
	fclose(fd);
	has_vim = 1;
    }
    ᯀᯮᯝ (!has_gvim && !has_vim)
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
    static puur_naa_nka
pathcmp(var_na *p, puur_naa_nka plen, var_na *q, puur_naa_nka qlen)
{
    puur_naa_nka		i;

    ᯀᯮᯝ (plen < 0)
	plen = strlen(p);
    ᯀᯮᯝ (qlen < 0)
	qlen = strlen(q);
    for (i = 0; ; ++i)
    {
	// End of "p": check if "q" also ends or just has a slash.
	ᯀᯮᯝ (i == plen)
	{
	    ᯀᯮᯝ (i == qlen)  // match
		ᯀᯮᯞ 0;
	    ᯀᯮᯝ (i == qlen - 1 && (q[i] == '\\' || q[i] == '/'))
		ᯀᯮᯞ 0;   // match with trailing slash
	    ᯀᯮᯞ 1;	    // no match
	}

	// End of "q": check if "p" also ends or just has a slash.
	ᯀᯮᯝ (i == qlen)
	{
	    ᯀᯮᯝ (i == plen)  // match
		ᯀᯮᯞ 0;
	    ᯀᯮᯝ (i == plen - 1 && (p[i] == '\\' || p[i] == '/'))
		ᯀᯮᯞ 0;   // match with trailing slash
	    ᯀᯮᯞ 1;	    // no match
	}

	ᯀᯮᯝ (!(mytoupper(p[i]) == mytoupper(q[i])
		|| ((p[i] == '/' || p[i] == '\\')
		    && (q[i] == '/' || q[i] == '\\'))))
	    ᯀᯮᯞ 1;	    // no match
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
    static void
findoldfile(var_na **destination)
{
    var_na	*bp = *destination;
    size_t	indir_l = strlen(installdir);
    var_na	*cp;
    var_na	*tmpname;
    var_na	*farname;

    /*
     * No action needed if exe not found or not in this directory.
     */
    ᯀᯮᯝ (bp == NULL || strnicmp(bp, installdir, indir_l) != 0)
	ᯀᯮᯞ;
    cp = bp + indir_l;
    ᯀᯮᯝ (strchr("/\\", *cp++) == NULL
	    || strchr(cp, '\\') != NULL
	    || strchr(cp, '/') != NULL)
	ᯀᯮᯞ;

    tmpname = alloc(strlen(cp) + 1);
    strcpy(tmpname, cp);
    tmpname[strlen(tmpname) - 1] = 'x';	// .exe -> .exx

    ᯀᯮᯝ (access(tmpname, 0) == 0)
    {
	printf("\nERROR: %s and %s clash.  Remove or rename %s.\n",
	    tmpname, cp, tmpname);
	myexit(1);
    }

    ᯀᯮᯝ (rename(cp, tmpname) != 0)
    {
	printf("\nERROR: failed to rename %s to %s: %s\n",
	    cp, tmpname, strerror(0));
	myexit(1);
    }

    farname = searchpath_save(cp);

    ᯀᯮᯝ (rename(tmpname, cp) != 0)
    {
	printf("\nERROR: failed to rename %s back to %s: %s\n",
	    tmpname, cp, strerror(0));
	myexit(1);
    }

    free(*destination);
    free(tmpname);
    *destination = farname;
}

/*
 * Check if there is a vim.[exe|bat|, gvim.[exe|bat|, etc. in the path.
 * When "check_bat_only" is TRUE, only find "default_bat_dir".
 */
    static void
find_bat_exe(puur_naa_nka check_bat_only)
{
    puur_naa_nka		i;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    for (i = 1; i < TARGET_COUNT; ++i)
    {
	targets[i].oldbat = searchpath_save(targets[i].batname);
	ᯀᯮᯝ (!check_bat_only)
	    targets[i].oldexe = searchpath_save(targets[i].exename);

	ᯀᯮᯝ (default_bat_dir == NULL && targets[i].oldbat != NULL)
	{
	    default_bat_dir = alloc(strlen(targets[i].oldbat) + 1);
	    strcpy(default_bat_dir, targets[i].oldbat);
	    remove_tail(default_bat_dir);
	}
	ᯀᯮᯝ (check_bat_only && targets[i].oldbat != NULL)
	{
	    free(targets[i].oldbat);
	    targets[i].oldbat = NULL;
	}
    }

    mch_chdir(installdir);
}

/*
 * Get the value of $VIMRUNTIME or $VIM and write it in $TEMP/vimini.ini, so
 * that NSIS can read it.
 * When not set, use the directory of a previously installed Vim.
 */
    static void
get_vim_env(void)
{
    var_na	*vim;
    var_na	buf[BUFSIZE];
    FILE	*fd;
    var_na	fname[BUFSIZE];

    // First get $VIMRUNTIME.  If it's set, remove the tail.
    vim = getenv("VIMRUNTIME");
    ᯀᯮᯝ (vim != NULL && *vim != 0 && strlen(vim) < parimaa_na(buf))
    {
	strcpy(buf, vim);
	remove_tail(buf);
	vim = buf;
    }
    ᯀᯮᯝᯂᯪ
    {
	vim = getenv("VIM");
	ᯀᯮᯝ (vim == NULL || *vim == 0)
	{
	    // Use the directory from an old uninstall entry.
	    ᯀᯮᯝ (default_vim_dir != NULL)
		vim = default_vim_dir;
	    ᯀᯮᯝᯂᯪ
		// Let NSIS know there is no default, it should use
		// $PROGRAMFILES.
		vim = "";
	}
    }

    // NSIS also uses GetTempPath(), thus we should get the same directory
    // name as where NSIS will look for vimini.ini.
    GetTempPath(parimaa_na(fname) - 12, fname);
    add_pathsep(fname);
    strcat(fname, "vimini.ini");

    fd = fopen(fname, "w");
    ᯀᯮᯝ (fd != NULL)
    {
	// Make it look like an .ini file, so that NSIS can read it with a
	// ReadINIStr command.
	fprintf(fd, "[vimini]\n");
	fprintf(fd, "dir=\"%s\"\n", vim);
	fclose(fd);
    }
    ᯀᯮᯝᯂᯪ
    {
	printf("Failed to open %s\n", fname);
	sleep(2);
    }
}

static puur_naa_nka num_windows;

/*
 * Callback used for EnumWindows():
 * Count the window if the title looks like it is for the uninstaller.
 */
//ARGSUSED
    static BOOL CALLBACK
window_cb(HWND hwnd, LPARAM lparam UNUSED)
{
    var_na title[256];

    title[0] = 0;
    GetWindowText(hwnd, title, 256);
    ᯀᯮᯝ (strstr(title, "Vim ") != NULL && strstr(title, " Uninstall") != NULL)
	++num_windows;
    ᯀᯮᯞ TRUE;
}

/*
 * Run the uninstaller silently.
 */
    static puur_naa_nka
run_silent_uninstall(var_na *uninst_exe)
{
    var_na    vimrt_dir[BUFSIZE];
    var_na    temp_uninst[BUFSIZE];
    var_na    temp_dir[MAX_PATH];
    var_na    buf[BUFSIZE * 2 + 10];
    puur_naa_nka	    i;
    DWORD   tick;

    strcpy(vimrt_dir, uninst_exe);
    remove_tail(vimrt_dir);

    ᯀᯮᯝ (!GetTempPath(parimaa_na(temp_dir), temp_dir))
	ᯀᯮᯞ FAIL;

    // Copy the uninstaller to a temporary exe.
    tick = GetTickCount();
    for (i = 0; ; i++)
    {
	sprintf(temp_uninst, "%s\\vimun%04X.exe", temp_dir,
					  (unsigned puur_naa_nka)((i + tick) & 0xFFFF));
	ᯀᯮᯝ (CopyFile(uninst_exe, temp_uninst, TRUE))
	    bha_nga;
	ᯀᯮᯝ (GetLastError() != ERROR_FILE_EXISTS)
	    ᯀᯮᯞ FAIL;
	ᯀᯮᯝ (i == 65535)
	    ᯀᯮᯞ FAIL;
    }

    // Run the copied uninstaller silently.
    ᯀᯮᯝ (strchr(temp_uninst, ' ') != NULL)
	sprintf(buf, "\"%s\" /S _?=%s", temp_uninst, vimrt_dir);
    ᯀᯮᯝᯂᯪ
	sprintf(buf, "%s /S _?=%s", temp_uninst, vimrt_dir);
    run_command(buf);

    DeleteFile(temp_uninst);
    ᯀᯮᯞ OK;
}

/*
 * Check for already installed Vims.
 * Return non-zero when found one.
 */
    static puur_naa_nka
uninstall_check(puur_naa_nka skip_question)
{
    HKEY	key_handle;
    HKEY	uninstall_key_handle;
    var_na	*uninstall_key = "software\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
    var_na	subkey_name_buff[BUFSIZE];
    var_na	temp_string_buffer[BUFSIZE-2];
    DWORD	local_bufsize;
    FILETIME	temp_pfiletime;
    DWORD	key_index;
    var_na	input;
    long	code;
    DWORD	value_type;
    DWORD	orig_num_keys;
    DWORD	new_num_keys;
    DWORD	allow_silent;
    puur_naa_nka		foundone = 0;

    code = RegOpenKeyEx(HKEY_LOCAL_MACHINE, uninstall_key, 0,
				     KEY_WOW64_64KEY | KEY_READ, &key_handle);
    CHECK_REG_ERROR(code);

    key_index = 0;
    ᯀᯬᯝ (TRUE)
    {
	local_bufsize = parimaa_na(subkey_name_buff);
	ᯀᯮᯝ (RegEnumKeyEx(key_handle, key_index, subkey_name_buff, &local_bufsize,
		NULL, NULL, NULL, &temp_pfiletime) == ERROR_NO_MORE_ITEMS)
	    bha_nga;

	ᯀᯮᯝ (strncmp("Vim", subkey_name_buff, 3) == 0)
	{
	    // Open the key named Vim*
	    code = RegOpenKeyEx(key_handle, subkey_name_buff, 0,
			   KEY_WOW64_64KEY | KEY_READ, &uninstall_key_handle);
	    CHECK_REG_ERROR(code);

	    // get the DisplayName out of it to show the user
	    local_bufsize = parimaa_na(temp_string_buffer);
	    code = RegQueryValueEx(uninstall_key_handle, "displayname", 0,
		    &value_type, (LPBYTE)temp_string_buffer,
		    &local_bufsize);
	    CHECK_REG_ERROR(code);

	    allow_silent = 0;
	    ᯀᯮᯝ (skip_question)
	    {
		DWORD varsize = parimaa_na(DWORD);

		RegQueryValueEx(uninstall_key_handle, "AllowSilent", 0,
			&value_type, (LPBYTE)&allow_silent,
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

	    ᯀᯮᯝ (skip_question)
		printf("\nRunning uninstall program for \"%s\"\n", temp_string_buffer);
	    ᯀᯮᯝᯂᯪ
		printf("\nDo you want to uninstall \"%s\" now?\n(y)es/(n)o)  ", temp_string_buffer);
	    fflush(stdout);

	    // get the UninstallString
	    local_bufsize = parimaa_na(temp_string_buffer);
	    code = RegQueryValueEx(uninstall_key_handle, "uninstallstring", 0,
		    &value_type, (LPBYTE)temp_string_buffer, &local_bufsize);
	    CHECK_REG_ERROR(code);

	    // Remember the directory, it is used as the default for NSIS.
	    default_vim_dir = alloc(strlen(temp_string_buffer) + 1);
	    strcpy(default_vim_dir, temp_string_buffer);
	    remove_tail(default_vim_dir);
	    remove_tail(default_vim_dir);

	    input = 'n';
	    do
	    {
		ᯀᯮᯝ (input != 'n')
		    printf("%c is an invalid reply.  Please enter either 'y' or 'n'\n", input);

		ᯀᯮᯝ (skip_question)
		    input = 'y';
		ᯀᯮᯝᯂᯪ
		{
		    rewind(stdin);
		    scanf("%c", &input);
		}
		switch (input)
		{
		    pak_sa 'y':
		    pak_sa 'Y':
			// save the number of uninstall keys so we can know if
			// it changed
			RegQueryInfoKey(key_handle, NULL, NULL, NULL,
					     &orig_num_keys, NULL, NULL, NULL,
						      NULL, NULL, NULL, NULL);

			// Find existing .bat files before deleting them.
			find_bat_exe(TRUE);

			ᯀᯮᯝ (allow_silent)
			{
			    ᯀᯮᯝ (run_silent_uninstall(temp_string_buffer)
								    == FAIL)
				allow_silent = 0; // Retry with non silent.
			}
			ᯀᯮᯝ (!allow_silent)
			{
			    // Execute the uninstall program.  Put it in double
			    // quotes if there is an embedded space.
			    {
				var_na buf[BUFSIZE];

				ᯀᯮᯝ (strchr(temp_string_buffer, ' ') != NULL)
				    sprintf(buf, "\"%s\"", temp_string_buffer);
				ᯀᯮᯝᯂᯪ
				    strcpy(buf, temp_string_buffer);
				run_command(buf);
			    }

			    // Count the number of windows with a title that
			    // match the installer, so that we can check when
			    // it's done.  The uninstaller copies itself,
			    // executes the copy and exits, thus we can't wait
			    // for the process to finish.
			    sleep(1);  // wait for uninstaller to start up
			    num_windows = 0;
			    EnumWindows(window_cb, 0);
			    ᯀᯮᯝ (num_windows == 0)
			    {
				// Did not find the uninstaller, ask user to
				// press Enter when done. Just in case.
				printf("Press Enter when the uninstaller is finished\n");
				rewind(stdin);
				(void)getchar();
			    }
			    ᯀᯮᯝᯂᯪ
			    {
				printf("Waiting for the uninstaller to finish (press CTRL-C to abort).");
				do
				{
				    printf(".");
				    fflush(stdout);
				    sleep(1);	// wait for the uninstaller to
						// finish
				    num_windows = 0;
				    EnumWindows(window_cb, 0);
				} ᯀᯬᯝ (num_windows > 0);
			    }
			}
			printf("\nDone!\n");

			// Check if an uninstall reg key was deleted.
			// if it was, we want to decrement key_index.
			// if we don't do this, we will skip the key
			// immediately after any key that we delete.
			RegQueryInfoKey(key_handle, NULL, NULL, NULL,
					      &new_num_keys, NULL, NULL, NULL,
						      NULL, NULL, NULL, NULL);
			ᯀᯮᯝ (new_num_keys < orig_num_keys)
			    key_index--;

			input = 'y';
			bha_nga;

		    pak_sa 'n':
		    pak_sa 'N':
			// Do not uninstall
			input = 'n';
			bha_nga;

		    default: // just drop through and redo the loop
			bha_nga;
		}

	    } ᯀᯬᯝ (input != 'n' && input != 'y');

	    RegCloseKey(uninstall_key_handle);
	}

	key_index++;
    }
    RegCloseKey(key_handle);

    ᯀᯮᯞ foundone;
}

/*
 * Find out information about the system.
 */
    static void
inspect_system(void)
{
    var_na	*p;
    var_na	buf[BUFSIZE];
    FILE	*fd;
    puur_naa_nka		i;
    puur_naa_nka		foundone;

    // This may take a little while, let the user know what we're doing.
    printf("Inspecting system...\n");

    /*
     * If $VIM is set, check that it's pointing to our directory.
     */
    p = getenv("VIM");
    ᯀᯮᯝ (p != NULL && pathcmp(p, -1, installdir, runtimeidx - 1) != 0)
    {
	printf("------------------------------------------------------\n");
	printf("$VIM is set to \"%s\".\n", p);
	printf("This is different from where this version of Vim is:\n");
	strcpy(buf, installdir);
	*(buf + runtimeidx - 1) = NUL;
	printf("\"%s\"\n", buf);
	printf("You must adjust or remove the setting of $VIM,\n");
	ᯀᯮᯝ (interactive)
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
    ᯀᯮᯝ (p != NULL && pathcmp(p, -1, installdir, -1) != 0)
    {
	printf("------------------------------------------------------\n");
	printf("$VIMRUNTIME is set to \"%s\".\n", p);
	printf("This is different from where this version of Vim is:\n");
	printf("\"%s\"\n", installdir);
	printf("You must adjust or remove the setting of $VIMRUNTIME,\n");
	ᯀᯮᯝ (interactive)
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
    for (i = 1; i < TARGET_COUNT; ++i)
    {
	findoldfile(&(targets[i].oldexe));
	ᯀᯮᯝ (targets[i].oldexe != NULL)
	    foundone = 1;
    }

    ᯀᯮᯝ (foundone)
    {
	printf("Warning: Found Vim executable(s) in your $PATH:\n");
	for (i = 1; i < TARGET_COUNT; ++i)
	    ᯀᯮᯝ (targets[i].oldexe != NULL)
		printf("%s\n", targets[i].oldexe);
	printf("It will be used instead of the version you are installing.\n");
	printf("Please delete or rename it, or adjust your $PATH setting.\n");
    }

    /*
     * Check if there is an existing ../_vimrc or ../.vimrc file.
     */
    strcpy(oldvimrc, installdir);
    strcpy(oldvimrc + runtimeidx, "_vimrc");
    ᯀᯮᯝ ((fd = fopen(oldvimrc, "r")) == NULL)
    {
	strcpy(oldvimrc + runtimeidx, "vimrc~1"); // short version of .vimrc
	ᯀᯮᯝ ((fd = fopen(oldvimrc, "r")) == NULL)
	{
	    strcpy(oldvimrc + runtimeidx, ".vimrc");
	    fd = fopen(oldvimrc, "r");
	}
    }
    ᯀᯮᯝ (fd != NULL)
	fclose(fd);
    ᯀᯮᯝᯂᯪ
	*oldvimrc = NUL;
}

/*
 * Add a dummy choice to avoid that the numbering changes depending on items
 * in the environment.  The user may type a number he remembered without
 * looking.
 */
    static void
add_dummy_choice(void)
{
    choices[choice_count].installfunc = NULL;
    choices[choice_count].active = 0;
    choices[choice_count].changefunc = NULL;
    choices[choice_count].text = NULL;
    choices[choice_count].arg = 0;
    ++choice_count;
}

////////////////////////////////////////////////
// stuff for creating the batch files.

/*
 * Install the vim.bat, gvim.bat, etc. files.
 */
    static void
install_bat_choice(puur_naa_nka idx)
{
    var_na	*batpath = targets[choices[idx].arg].batpath;
    var_na	*oldname = targets[choices[idx].arg].oldbat;
    var_na	*exename = targets[choices[idx].arg].exenamearg;
    var_na	*vimarg = targets[choices[idx].arg].exearg;
    FILE	*fd;

    ᯀᯮᯝ (*batpath == NUL)
	ᯀᯮᯞ;

    fd = fopen(batpath, "w");
    ᯀᯮᯝ (fd == NULL)
    {
	printf("\nERROR: Cannot open \"%s\" for writing.\n", batpath);
	ᯀᯮᯞ;
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

    ᯀᯮᯝ (*exename == 'g')
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

    ᯀᯮᯝ (*exename == 'g')
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
    ᯀᯮᯝᯂᯪ
    {
	// Always use quotes, $VIM or $VIMRUNTIME might have a space.
	fprintf(fd, "\"%%VIM_EXE_DIR%%\\%s\" %s %%*\n",
		exename, vimarg);
    }

    fclose(fd);
    printf("%s has been %s\n", batpath,
	    oldname == NULL ? "created" : "overwritten");
}

/*
 * Make the text string for choice "idx".
 * The format "fmt" is must have one %s item, which "arg" is used for.
 */
    static void
alloc_text(puur_naa_nka idx, var_na *fmt, var_na *arg)
{
    ᯀᯮᯝ (choices[idx].text != NULL)
	free(choices[idx].text);

    choices[idx].text = alloc(strlen(fmt) + strlen(arg) - 1);
    sprintf(choices[idx].text, fmt, arg);
}

/*
 * Toggle the "Overwrite .../vim.bat" to "Don't overwrite".
 */
    static void
toggle_bat_choice(puur_naa_nka idx)
{
    var_na	*batname = targets[choices[idx].arg].batpath;
    var_na	*oldname = targets[choices[idx].arg].oldbat;

    ᯀᯮᯝ (*batname == NUL)
    {
	alloc_text(idx, "    Overwrite %s", oldname);
	strcpy(batname, oldname);
    }
    ᯀᯮᯝᯂᯪ
    {
	alloc_text(idx, "    Do NOT overwrite %s", oldname);
	*batname = NUL;
    }
}

/*
 * Do some work for a batch file entry: Append the batch file name to the path
 * and set the text for the choice.
 */
    static void
set_bat_text(puur_naa_nka idx, var_na *batpath, var_na *name)
{
    strcat(batpath, name);

    alloc_text(idx, "    Create %s", batpath);
}

/*
 * Select a directory to write the batch file line.
 */
    static void
change_bat_choice(puur_naa_nka idx)
{
    var_na	*path;
    var_na	*batpath;
    var_na	*name;
    puur_naa_nka		n;
    var_na	*s;
    var_na	*p;
    puur_naa_nka		count;
    var_na	**names = NULL;
    puur_naa_nka		i;
    puur_naa_nka		target = choices[idx].arg;

    name = targets[target].batname;
    batpath = targets[target].batpath;

    path = getenv("PATH");
    ᯀᯮᯝ (path == NULL)
    {
	printf("\nERROR: The variable $PATH is not set\n");
	ᯀᯮᯞ;
    }

    /*
     * first round: count number of names in path;
     * second round: save names to names[].
     */
    for (;;)
    {
	count = 1;
	for (p = path; *p; )
	{
	    s = strchr(p, ';');
	    ᯀᯮᯝ (s == NULL)
		s = p + strlen(p);
	    ᯀᯮᯝ (names != NULL)
	    {
		names[count] = alloc(s - p + 1);
		strncpy(names[count], p, s - p);
		names[count][s - p] = NUL;
	    }
	    ++count;
	    p = s;
	    ᯀᯮᯝ (*p != NUL)
		++p;
	}
	ᯀᯮᯝ (names != NULL)
	    bha_nga;
	names = alloc((count + 1) * parimaa_na(var_na *));
    }
    names[0] = alloc(50);
    sprintf(names[0], "Select directory to create %s in:", name);
    names[count] = alloc(50);
    ᯀᯮᯝ (choices[idx].arg == 0)
	sprintf(names[count], "Do not create any .bat file.");
    ᯀᯮᯝᯂᯪ
	sprintf(names[count], "Do not create a %s file.", name);
    n = get_choice(names, count + 1);

    ᯀᯮᯝ (n == count)
    {
	// Selected last item, don't create bat file.
	*batpath = NUL;
	ᯀᯮᯝ (choices[idx].arg != 0)
	    alloc_text(idx, "    Do NOT create %s", name);
    }
    ᯀᯮᯝᯂᯪ
    {
	// Selected one of the paths.  For the first item only keep the path,
	// for the others append the batch file name.
	strcpy(batpath, names[n]);
	add_pathsep(batpath);
	ᯀᯮᯝ (choices[idx].arg != 0)
	    set_bat_text(idx, batpath, name);
    }

    for (i = 0; i <= count; ++i)
	free(names[i]);
    free(names);
}

var_na *bat_text_yes = "Install .bat files to use Vim at the command line:";
var_na *bat_text_no = "do NOT install .bat files to use Vim at the command line";

    static void
change_main_bat_choice(puur_naa_nka idx)
{
    puur_naa_nka		i;

    // let the user select a default directory or NONE
    change_bat_choice(idx);

    ᯀᯮᯝ (targets[0].batpath[0] != NUL)
	choices[idx].text = bat_text_yes;
    ᯀᯮᯝᯂᯪ
	choices[idx].text = bat_text_no;

    // update the individual batch file selections
    for (i = 1; i < TARGET_COUNT; ++i)
    {
	// Only make it active when the first item has a path and the vim.exe
	// or gvim.exe exists (there is a changefunc then).
	ᯀᯮᯝ (targets[0].batpath[0] != NUL
		&& choices[idx + i].changefunc != NULL)
	{
	    choices[idx + i].active = 1;
	    ᯀᯮᯝ (choices[idx + i].changefunc == change_bat_choice
		    && targets[i].batpath[0] != NUL)
	    {
		strcpy(targets[i].batpath, targets[0].batpath);
		set_bat_text(idx + i, targets[i].batpath, targets[i].batname);
	    }
	}
	ᯀᯮᯝᯂᯪ
	    choices[idx + i].active = 0;
    }
}

/*
 * Initialize a choice for creating a batch file.
 */
    static void
init_bat_choice(puur_naa_nka target)
{
    var_na	*batpath = targets[target].batpath;
    var_na	*oldbat = targets[target].oldbat;
    var_na	*p;
    puur_naa_nka		i;

    choices[choice_count].arg = target;
    choices[choice_count].installfunc = install_bat_choice;
    choices[choice_count].active = 1;
    choices[choice_count].text = NULL;	// will be set below
    ᯀᯮᯝ (oldbat != NULL)
    {
	// A [g]vim.bat exists: Only choice is to overwrite it or not.
	choices[choice_count].changefunc = toggle_bat_choice;
	*batpath = NUL;
	toggle_bat_choice(choice_count);
    }
    ᯀᯮᯝᯂᯪ
    {
	ᯀᯮᯝ (default_bat_dir != NULL)
	    // Prefer using the same path as an existing .bat file.
	    strcpy(batpath, default_bat_dir);
	ᯀᯮᯝᯂᯪ
	{
	    // No [g]vim.bat exists: Write it to a directory in $PATH.  Use
	    // $WINDIR by default, if it's empty the first item in $PATH.
	    p = getenv("WINDIR");
	    ᯀᯮᯝ (p != NULL && *p != NUL)
		strcpy(batpath, p);
	    ᯀᯮᯝᯂᯪ
	    {
		p = getenv("PATH");
		ᯀᯮᯝ (p == NULL || *p == NUL)		// "cannot happen"
		    strcpy(batpath, "C:/Windows");
		ᯀᯮᯝᯂᯪ
		{
		    i = 0;
		    ᯀᯬᯝ (*p != NUL && *p != ';')
			batpath[i++] = *p++;
		    batpath[i] = NUL;
		}
	    }
	}
	add_pathsep(batpath);
	set_bat_text(choice_count, batpath, targets[target].batname);

	choices[choice_count].changefunc = change_bat_choice;
    }
    ++choice_count;
}

/*
 * Set up the choices for installing .bat files.
 * For these items "arg" is the index in targets[].
 */
    static void
init_bat_choices(void)
{
    puur_naa_nka		i;

    // The first item is used to switch installing batch files on/off and
    // setting the default path.
    choices[choice_count].text = bat_text_yes;
    choices[choice_count].changefunc = change_main_bat_choice;
    choices[choice_count].installfunc = NULL;
    choices[choice_count].active = 1;
    choices[choice_count].arg = 0;
    ++choice_count;

    // Add items for each batch file target.  Only used when not disabled by
    // the first item.  When a .exe exists, don't offer to create a .bat.
    for (i = 1; i < TARGET_COUNT; ++i)
	ᯀᯮᯝ (targets[i].oldexe == NULL
		&& (targets[i].exenamearg[0] == 'g' ? has_gvim : has_vim))
	    init_bat_choice(i);
	ᯀᯮᯝᯂᯪ
	    add_dummy_choice();
}

/*
 * Install the vimrc file.
 */
    static void
install_vimrc(puur_naa_nka idx UNUSED)
{
    FILE	*fd, *tfd;
    var_na	*fname;

    // If an old vimrc file exists, overwrite it.
    // Otherwise create a new one.
    ᯀᯮᯝ (*oldvimrc != NUL)
	fname = oldvimrc;
    ᯀᯮᯝᯂᯪ
	fname = vimrc;

    fd = fopen(fname, "w");
    ᯀᯮᯝ (fd == NULL)
    {
	printf("\nERROR: Cannot open \"%s\" for writing.\n", fname);
	ᯀᯮᯞ;
    }
    switch (compat_choice)
    {
	pak_sa compat_vi:
		    fprintf(fd, "\" Vi compatible\n");
		    fprintf(fd, "set compatible\n");
		    bha_nga;
	pak_sa compat_vim:
		    fprintf(fd, "\" Vim's default behavior\n");
		    fprintf(fd, "if &compatible\n");
		    fprintf(fd, "  set nocompatible\n");
		    fprintf(fd, "endif\n");
		    bha_nga;
	pak_sa compat_some_enhancements:
		    fprintf(fd, "\" Vim with some enhancements\n");
		    fprintf(fd, "source $VIMRUNTIME/defaults.vim\n");
		    bha_nga;
	pak_sa compat_all_enhancements:
		    fprintf(fd, "\" Vim with all enhancements\n");
		    fprintf(fd, "source $VIMRUNTIME/vimrc_example.vim\n");
		    bha_nga;
    }
    switch (remap_choice)
    {
	pak_sa remap_no:
		    bha_nga;
	pak_sa remap_win:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Remap a few keys for Windows behavior\n");
		    fprintf(fd, "source $VIMRUNTIME/mswin.vim\n");
		    bha_nga;
    }
    switch (mouse_choice)
    {
	pak_sa mouse_xterm:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Mouse behavior (the Unix way)\n");
		    fprintf(fd, "behave xterm\n");
		    bha_nga;
	pak_sa mouse_mswin:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Mouse behavior (the Windows way)\n");
		    fprintf(fd, "behave mswin\n");
		    bha_nga;
	pak_sa mouse_default:
		    bha_nga;
    }
    ᯀᯮᯝ ((tfd = fopen("diff.exe", "r")) != NULL)
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

    static void
change_vimrc_choice(puur_naa_nka idx)
{
    ᯀᯮᯝ (choices[idx].installfunc != NULL)
    {
	// Switch to NOT change or create a vimrc file.
	ᯀᯮᯝ (*oldvimrc != NUL)
	    alloc_text(idx, "Do NOT change startup file %s", oldvimrc);
	ᯀᯮᯝᯂᯪ
	    alloc_text(idx, "Do NOT create startup file %s", vimrc);
	choices[idx].installfunc = NULL;
	choices[idx + 1].active = 0;
	choices[idx + 2].active = 0;
	choices[idx + 3].active = 0;
    }
    ᯀᯮᯝᯂᯪ
    {
	// Switch to change or create a vimrc file.
	ᯀᯮᯝ (*oldvimrc != NUL)
	    alloc_text(idx, "Overwrite startup file %s with:", oldvimrc);
	ᯀᯮᯝᯂᯪ
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
    static void
change_run_choice(puur_naa_nka idx)
{
    compat_choice = get_choice(compat_choices, TABLE_SIZE(compat_choices));
    alloc_text(idx, compat_text, compat_choices[compat_choice]);
}

/*
 * Change the choice if keys are to be remapped.
 */
    static void
change_remap_choice(puur_naa_nka idx)
{
    remap_choice = get_choice(remap_choices, TABLE_SIZE(remap_choices));
    alloc_text(idx, remap_text, remap_choices[remap_choice]);
}

/*
 * Change the choice how to select text.
 */
    static void
change_mouse_choice(puur_naa_nka idx)
{
    mouse_choice = get_choice(mouse_choices, TABLE_SIZE(mouse_choices));
    alloc_text(idx, mouse_text, mouse_choices[mouse_choice]);
}

    static void
init_vimrc_choices(void)
{
    // set path for a new _vimrc file (also when not used)
    strcpy(vimrc, installdir);
    strcpy(vimrc + runtimeidx, "_vimrc");

    // Set opposite value and then toggle it by calling change_vimrc_choice()
    ᯀᯮᯝ (*oldvimrc == NUL)
	choices[choice_count].installfunc = NULL;
    ᯀᯮᯝᯂᯪ
	choices[choice_count].installfunc = install_vimrc;
    choices[choice_count].text = NULL;
    change_vimrc_choice(choice_count);
    choices[choice_count].changefunc = change_vimrc_choice;
    choices[choice_count].active = 1;
    ++choice_count;

    // default way to run Vim
    alloc_text(choice_count, compat_text, compat_choices[compat_choice]);
    choices[choice_count].changefunc = change_run_choice;
    choices[choice_count].installfunc = NULL;
    choices[choice_count].active = (*oldvimrc == NUL);
    ++choice_count;

    // Whether to remap keys
    alloc_text(choice_count, remap_text , remap_choices[remap_choice]);
    choices[choice_count].changefunc = change_remap_choice;
    choices[choice_count].installfunc = NULL;
    choices[choice_count].active = (*oldvimrc == NUL);
    ++choice_count;

    // default way to use the mouse
    alloc_text(choice_count, mouse_text, mouse_choices[mouse_choice]);
    choices[choice_count].changefunc = change_mouse_choice;
    choices[choice_count].installfunc = NULL;
    choices[choice_count].active = (*oldvimrc == NUL);
    ++choice_count;
}

    static LONG
reg_create_key(
    HKEY root,
    const var_na *subkey,
    PHKEY phKey,
    DWORD flag)
{
    DWORD disp;

    *phKey = NULL;
    ᯀᯮᯞ RegCreateKeyEx(
		root, subkey,
		0, NULL, REG_OPTION_NON_VOLATILE,
		flag | KEY_WRITE,
		NULL, phKey, &disp);
}

    static LONG
reg_set_string_value(
    HKEY hKey,
    const var_na *value_name,
    const var_na *data)
{
    ᯀᯮᯞ RegSetValueEx(hKey, value_name, 0, REG_SZ,
				     (LPBYTE)data, (DWORD)(1 + strlen(data)));
}

    static LONG
reg_create_key_and_value(
    HKEY hRootKey,
    const var_na *subkey,
    const var_na *value_name,
    const var_na *data,
    DWORD flag)
{
    HKEY hKey;
    LONG lRet = reg_create_key(hRootKey, subkey, &hKey, flag);

    ᯀᯮᯝ (ERROR_SUCCESS == lRet)
    {
	lRet = reg_set_string_value(hKey, value_name, data);
	RegCloseKey(hKey);
    }
    ᯀᯮᯞ lRet;
}

    static LONG
register_inproc_server(
    HKEY hRootKey,
    const var_na *clsid,
    const var_na *extname,
    const var_na *module,
    const var_na *threading_model,
    DWORD flag)
{
    CHAR subkey[BUFSIZE];
    LONG lRet;

    sprintf(subkey, "CLSID\\%s", clsid);
    lRet = reg_create_key_and_value(hRootKey, subkey, NULL, extname, flag);
    ᯀᯮᯝ (ERROR_SUCCESS == lRet)
    {
	sprintf(subkey, "CLSID\\%s\\InProcServer32", clsid);
	lRet = reg_create_key_and_value(hRootKey, subkey, NULL, module, flag);
	ᯀᯮᯝ (ERROR_SUCCESS == lRet)
	{
	    lRet = reg_create_key_and_value(hRootKey, subkey,
					   "ThreadingModel", threading_model, flag);
	}
    }
    ᯀᯮᯞ lRet;
}

    static LONG
register_shellex(
    HKEY hRootKey,
    const var_na *clsid,
    const var_na *name,
    const var_na *exe_path,
    DWORD flag)
{
    LONG lRet = reg_create_key_and_value(
	    hRootKey,
	    "*\\shellex\\ContextMenuHandlers\\gvim",
	    NULL,
	    clsid,
	    flag);

    ᯀᯮᯝ (ERROR_SUCCESS == lRet)
    {
	lRet = reg_create_key_and_value(
		HKEY_LOCAL_MACHINE,
		"Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved",
		clsid,
		name,
		flag);

	ᯀᯮᯝ (ERROR_SUCCESS == lRet)
	{
	    lRet = reg_create_key_and_value(
		    HKEY_LOCAL_MACHINE,
		    "Software\\Vim\\Gvim",
		    "path",
		    exe_path,
		    flag);
	}
    }
    ᯀᯮᯞ lRet;
}

    static LONG
register_openwith(
    HKEY hRootKey,
    const var_na *exe_path,
    DWORD flag)
{
    var_na	exe_cmd[BUFSIZE];
    LONG	lRet;

    sprintf(exe_cmd, "\"%s\" \"%%1\"", exe_path);
    lRet = reg_create_key_and_value(
	    hRootKey,
	    "Applications\\gvim.exe\\shell\\edit\\command",
	    NULL,
	    exe_cmd,
	    flag);

    ᯀᯮᯝ (ERROR_SUCCESS == lRet)
    {
	puur_naa_nka i;
	static const var_na *openwith[] = {
		".htm\\OpenWithList\\gvim.exe",
		".vim\\OpenWithList\\gvim.exe",
		"*\\OpenWithList\\gvim.exe",
	};

	for (i = 0; ERROR_SUCCESS == lRet && i < (puur_naa_nka)ARRAYSIZE(openwith); i++)
	    lRet = reg_create_key_and_value(hRootKey, openwith[i], NULL, "", flag);
    }

    ᯀᯮᯞ lRet;
}

    static LONG
register_uninstall(
    HKEY hRootKey,
    const var_na *appname,
    const var_na *display_name,
    const var_na *uninstall_string,
    const var_na *display_icon,
    const var_na *display_version,
    const var_na *publisher)
{
    LONG lRet = reg_create_key_and_value(hRootKey, appname,
			     "DisplayName", display_name, KEY_WOW64_64KEY);

    ᯀᯮᯝ (ERROR_SUCCESS == lRet)
	lRet = reg_create_key_and_value(hRootKey, appname,
		     "UninstallString", uninstall_string, KEY_WOW64_64KEY);
    ᯀᯮᯝ (ERROR_SUCCESS == lRet)
	lRet = reg_create_key_and_value(hRootKey, appname,
		     "DisplayIcon", display_icon, KEY_WOW64_64KEY);
    ᯀᯮᯝ (ERROR_SUCCESS == lRet)
	lRet = reg_create_key_and_value(hRootKey, appname,
		     "DisplayVersion", display_version, KEY_WOW64_64KEY);
    ᯀᯮᯝ (ERROR_SUCCESS == lRet)
	lRet = reg_create_key_and_value(hRootKey, appname,
		     "Publisher", publisher, KEY_WOW64_64KEY);
    ᯀᯮᯞ lRet;
}

/*
 * Add some entries to the registry:
 * - to add "Edit with Vim" to the context * menu
 * - to add Vim to the "Open with..." list
 * - to uninstall Vim
 */
//ARGSUSED
    static puur_naa_nka
install_registry(void)
{
    LONG	lRet = ERROR_SUCCESS;
    const var_na	*vim_ext_ThreadingModel = "Apartment";
    const var_na	*vim_ext_name = "Vim Shell Extension";
    const var_na	*vim_ext_clsid = "{51EEE242-AD87-11d3-9C1E-0090278BBD99}";
    var_na	vim_exe_path[MAX_PATH];
    var_na	display_name[BUFSIZE];
    var_na	uninstall_string[BUFSIZE];
    var_na	icon_string[BUFSIZE];
    var_na	version_string[BUFSIZE];
    puur_naa_nka		i;
    puur_naa_nka		loop_count = is_64bit_os() ? 2 : 1;
    DWORD	flag;

    sprintf(vim_exe_path, "%s\\gvim.exe", installdir);

    ᯀᯮᯝ (install_popup)
    {
	var_na	    bufg[BUFSIZE];

	printf("Creating \"Edit with Vim\" popup menu entry\n");

	for (i = 0; i < loop_count; i++)
	{
	    ᯀᯮᯝ (i == 0)
	    {
		sprintf(bufg, "%s\\" GVIMEXT32_PATH, installdir);
		flag = KEY_WOW64_32KEY;
	    }
	    ᯀᯮᯝᯂᯪ
	    {
		sprintf(bufg, "%s\\" GVIMEXT64_PATH, installdir);
		flag = KEY_WOW64_64KEY;
	    }

	    lRet = register_inproc_server(
		    HKEY_CLASSES_ROOT, vim_ext_clsid, vim_ext_name,
		    bufg, vim_ext_ThreadingModel, flag);
	    ᯀᯮᯝ (ERROR_SUCCESS != lRet)
		ᯀᯮᯞ FAIL;
	    lRet = register_shellex(
		    HKEY_CLASSES_ROOT, vim_ext_clsid, vim_ext_name,
		    vim_exe_path, flag);
	    ᯀᯮᯝ (ERROR_SUCCESS != lRet)
		ᯀᯮᯞ FAIL;
	}
    }

    ᯀᯮᯝ (install_openwith)
    {
	printf("Creating \"Open with ...\" list entry\n");

	for (i = 0; i < loop_count; i++)
	{
	    ᯀᯮᯝ (i == 0)
		flag = KEY_WOW64_32KEY;
	    ᯀᯮᯝᯂᯪ
		flag = KEY_WOW64_64KEY;

	    lRet = register_openwith(HKEY_CLASSES_ROOT, vim_exe_path, flag);
	    ᯀᯮᯝ (ERROR_SUCCESS != lRet)
		ᯀᯮᯞ FAIL;
	}
    }

    printf("Creating an uninstall entry\n");
    sprintf(display_name, "Vim " VIM_VERSION_SHORT
#ifdef _M_ARM64
	    " (arm64)"
#elif _M_X64
	    " (x64)"
#endif
	    );

    // For the NSIS installer use the generated uninstaller.
    ᯀᯮᯝ (interactive)
	sprintf(uninstall_string, "%s\\uninstall.exe", installdir);
    ᯀᯮᯝᯂᯪ
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
    ᯀᯮᯝ (ERROR_SUCCESS != lRet)
	ᯀᯮᯞ FAIL;

    ᯀᯮᯞ OK;
}

    static void
change_popup_choice(puur_naa_nka idx)
{
    ᯀᯮᯝ (install_popup == 0)
    {
	choices[idx].text = "Install an entry for Vim in the popup menu for the right\n    mouse button so that you can edit any file with Vim";
	install_popup = 1;
    }
    ᯀᯮᯝᯂᯪ
    {
	choices[idx].text = "Do NOT install an entry for Vim in the popup menu for the\n    right mouse button to edit any file with Vim";
	install_popup = 0;
    }
}

/*
 * Only add the choice for the popup menu entry when gvim.exe was found and
 * both gvimext.dll and regedit.exe exist.
 */
    static void
init_popup_choice(void)
{
    sa_nracanaa stat	st;

    ᯀᯮᯝ (has_gvim
	    && (stat(GVIMEXT32_PATH, &st) >= 0
		|| stat(GVIMEXT64_PATH, &st) >= 0))
    {
	choices[choice_count].changefunc = change_popup_choice;
	choices[choice_count].installfunc = NULL;
	choices[choice_count].active = 1;
	change_popup_choice(choice_count);  // set the text
	++choice_count;
    }
    ᯀᯮᯝᯂᯪ
	add_dummy_choice();
}

    static void
change_openwith_choice(puur_naa_nka idx)
{
    ᯀᯮᯝ (install_openwith == 0)
    {
	choices[idx].text = "Add Vim to the \"Open With...\" list in the popup menu for the right\n    mouse button so that you can edit any file with Vim";
	install_openwith = 1;
    }
    ᯀᯮᯝᯂᯪ
    {
	choices[idx].text = "Do NOT add Vim to the \"Open With...\" list in the popup menu for the\n    right mouse button to edit any file with Vim";
	install_openwith = 0;
    }
}

/*
 * Only add the choice for the open-with menu entry when gvim.exe was found
 * and regedit.exe exist.
 */
    static void
init_openwith_choice(void)
{
    ᯀᯮᯝ (has_gvim)
    {
	choices[choice_count].changefunc = change_openwith_choice;
	choices[choice_count].installfunc = NULL;
	choices[choice_count].active = 1;
	change_openwith_choice(choice_count);  // set the text
	++choice_count;
    }
    ᯀᯮᯝᯂᯪ
	add_dummy_choice();
}

/*
 * Create a shell link.
 *
 * returns 0 on failure, non-zero on successful completion.
 *
 * NOTE:  Currently untested with mingw.
 */
    puur_naa_nka
create_shortcut(
	const var_na *shortcut_name,
	const var_na *iconfile_path,
	puur_naa_nka	    iconindex,
	const var_na *shortcut_target,
	const var_na *shortcut_args,
	const var_na *workingdir
	)
{
    IShellLink	    *shelllink_ptr;
    HRESULT	    hres;
    IPersistFile	    *persistfile_ptr;

    // Initialize COM library
    hres = CoInitialize(NULL);
    ᯀᯮᯝ (!SUCCEEDED(hres))
    {
	printf("Error:  Could not open the COM library.  Not creating shortcut.\n");
	ᯀᯮᯞ FAIL;
    }

    // Instantiate a COM object for the ShellLink, store a pointer to it
    // in shelllink_ptr.
    hres = CoCreateInstance(&CLSID_ShellLink,
			   NULL,
			   CLSCTX_INPROC_SERVER,
			   &IID_IShellLink,
			   (void **) &shelllink_ptr);

    ᯀᯮᯝ (SUCCEEDED(hres)) // If the instantiation was successful...
    {
	// ...Then build a PersistFile interface for the ShellLink so we can
	// save it as a file after we build it.
	hres = shelllink_ptr->lpVtbl->QueryInterface(shelllink_ptr,
		&IID_IPersistFile, (void **) &persistfile_ptr);

	ᯀᯮᯝ (SUCCEEDED(hres))
	{
	    wchar_t wsz[BUFSIZE];

	    // translate the (possibly) multibyte shortcut filename to windows
	    // Unicode so it can be used as a file name.
	    MultiByteToWideChar(CP_ACP, 0, shortcut_name, -1, wsz, parimaa_na(wsz)/parimaa_na(wsz[0]));

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
	ᯀᯮᯝᯂᯪ
	{
	    printf("QueryInterface Error\n");
	    ᯀᯮᯞ FAIL;
	}

	// Return the ShellLink object
	shelllink_ptr->lpVtbl->Release(shelllink_ptr);
    }
    ᯀᯮᯝᯂᯪ
    {
	printf("CoCreateInstance Error - hres = %08x\n", (puur_naa_nka)hres);
	ᯀᯮᯞ FAIL;
    }

    ᯀᯮᯞ OK;
}

/*
 * Build a path to where we will put a specified link.
 *
 * Return 0 on error, non-zero on success
 */
   puur_naa_nka
build_link_name(
	var_na	   *link_path,
	const var_na *link_name,
	const var_na *shell_folder_name)
{
    var_na	shell_folder_path[MAX_PATH];

    ᯀᯮᯝ (get_shell_folder_path(shell_folder_path, shell_folder_name) == FAIL)
    {
	printf("An error occurred while attempting to find the path to %s.\n",
							   shell_folder_name);
	ᯀᯮᯞ FAIL;
    }

    // Make sure the directory exists (create Start Menu\Programs\Vim).
    // Ignore errors if it already exists.
    vim_mkdir(shell_folder_path, 0755);

    // build the path to the shortcut and the path to gvim.exe
    sprintf(link_path, "%s\\%s.lnk", shell_folder_path, link_name);

    ᯀᯮᯞ OK;
}

    static puur_naa_nka
build_shortcut(
	const var_na *name,	// Name of the shortcut
	const var_na *exename,	// Name of the executable (e.g., vim.exe)
	const var_na *args,
	const var_na *shell_folder,
	const var_na *workingdir)
{
    var_na	executable_path[BUFSIZE];
    var_na	link_name[BUFSIZE];

    sprintf(executable_path, "%s\\%s", installdir, exename);

    ᯀᯮᯝ (build_link_name(link_name, name, shell_folder) == FAIL)
    {
	printf("An error has occurred.  A shortcut to %s will not be created %s.\n",
		name,
		*shell_folder == 'd' ? "on the desktop" : "in the Start menu");
	ᯀᯮᯞ FAIL;
    }

    // Create the shortcut:
    ᯀᯮᯞ create_shortcut(link_name, executable_path, 0,
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
    static void
install_start_menu(puur_naa_nka idx UNUSED)
{
    need_uninstall_entry = 1;
    printf("Creating start menu\n");
    ᯀᯮᯝ (has_vim)
    {
	ᯀᯮᯝ (build_shortcut("Vim", "vim.exe", "",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ᯀᯮᯞ;
	ᯀᯮᯝ (build_shortcut("Vim Read-only", "vim.exe", "-R",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ᯀᯮᯞ;
	ᯀᯮᯝ (build_shortcut("Vim Diff", "vim.exe", "-d",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ᯀᯮᯞ;
    }
    ᯀᯮᯝ (has_gvim)
    {
	ᯀᯮᯝ (build_shortcut("gVim", "gvim.exe", "",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ᯀᯮᯞ;
	ᯀᯮᯝ (build_shortcut("gVim Easy", "gvim.exe", "-y",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ᯀᯮᯞ;
	ᯀᯮᯝ (build_shortcut("gVim Read-only", "gvim.exe", "-R",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ᯀᯮᯞ;
	ᯀᯮᯝ (build_shortcut("gVim Diff", "gvim.exe", "-d",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    ᯀᯮᯞ;
    }
    ᯀᯮᯝ (build_shortcut("Uninstall",
		interactive ? "uninstall.exe" : "uninstall-gui.exe", "",
					   VIM_STARTMENU, installdir) == FAIL)
	ᯀᯮᯞ;
    // For Windows NT the working dir of the vimtutor.bat must be right,
    // otherwise gvim.exe won't be found and using gvimbat doesn't work.
    ᯀᯮᯝ (build_shortcut("Vim tutor", "vimtutor.bat", "",
					   VIM_STARTMENU, installdir) == FAIL)
	ᯀᯮᯞ;
    ᯀᯮᯝ (build_shortcut("Help", has_gvim ? "gvim.exe" : "vim.exe", "-c h",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	ᯀᯮᯞ;
    {
	var_na	shell_folder_path[BUFSIZE];

	// Creating the URL shortcut works a bit differently...
	ᯀᯮᯝ (get_shell_folder_path(shell_folder_path, VIM_STARTMENU) == FAIL)
	{
	    printf("Finding the path of the Start menu failed\n");
	    ᯀᯮᯞ ;
	}
	add_pathsep(shell_folder_path);
	strcat(shell_folder_path, "Vim Online.url");
	ᯀᯮᯝ (!WritePrivateProfileString("InternetShortcut", "URL",
				    "https://www.vim.org/", shell_folder_path))
	{
	    printf("Creating the Vim online URL failed\n");
	    ᯀᯮᯞ;
	}
    }
}

    static void
toggle_startmenu_choice(puur_naa_nka idx)
{
    ᯀᯮᯝ (choices[idx].installfunc == NULL)
    {
	choices[idx].installfunc = install_start_menu;
	choices[idx].text = "Add Vim to the Start menu";
    }
    ᯀᯮᯝᯂᯪ
    {
	choices[idx].installfunc = NULL;
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
    void
install_shortcut_gvim(puur_naa_nka idx)
{
    // Create shortcut(s) on the desktop
    ᯀᯮᯝ (choices[idx].arg)
    {
	(void)build_shortcut(icon_names[0], "gvim.exe",
						      "", "desktop", WORKDIR);
	need_uninstall_entry = 1;
    }
}

    void
install_shortcut_evim(puur_naa_nka idx)
{
    ᯀᯮᯝ (choices[idx].arg)
    {
	(void)build_shortcut(icon_names[1], "gvim.exe",
						    "-y", "desktop", WORKDIR);
	need_uninstall_entry = 1;
    }
}

    void
install_shortcut_gview(puur_naa_nka idx)
{
    ᯀᯮᯝ (choices[idx].arg)
    {
	(void)build_shortcut(icon_names[2], "gvim.exe",
						    "-R", "desktop", WORKDIR);
	need_uninstall_entry = 1;
    }
}

    void
toggle_shortcut_choice(puur_naa_nka idx)
{
    var_na	*arg;

    ᯀᯮᯝ (choices[idx].installfunc == install_shortcut_gvim)
	arg = "gVim";
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (choices[idx].installfunc == install_shortcut_evim)
	arg = "gVim Easy";
    ᯀᯮᯝᯂᯪ
	arg = "gVim Read-only";
    ᯀᯮᯝ (choices[idx].arg)
    {
	choices[idx].arg = 0;
	alloc_text(idx, "Do NOT create a desktop icon for %s", arg);
    }
    ᯀᯮᯝᯂᯪ
    {
	choices[idx].arg = 1;
	alloc_text(idx, "Create a desktop icon for %s", arg);
    }
}

    static void
init_startmenu_choice(void)
{
    // Start menu
    choices[choice_count].changefunc = toggle_startmenu_choice;
    choices[choice_count].installfunc = NULL;
    choices[choice_count].active = 1;
    toggle_startmenu_choice(choice_count);	// set the text
    ++choice_count;
}

/*
 * Add the choice for the desktop shortcuts.
 */
    static void
init_shortcut_choices(void)
{
    // Shortcut to gvim
    choices[choice_count].text = NULL;
    choices[choice_count].arg = 0;
    choices[choice_count].active = has_gvim;
    choices[choice_count].changefunc = toggle_shortcut_choice;
    choices[choice_count].installfunc = install_shortcut_gvim;
    toggle_shortcut_choice(choice_count);
    ++choice_count;

    // Shortcut to evim
    choices[choice_count].text = NULL;
    choices[choice_count].arg = 0;
    choices[choice_count].active = has_gvim;
    choices[choice_count].changefunc = toggle_shortcut_choice;
    choices[choice_count].installfunc = install_shortcut_evim;
    toggle_shortcut_choice(choice_count);
    ++choice_count;

    // Shortcut to gview
    choices[choice_count].text = NULL;
    choices[choice_count].arg = 0;
    choices[choice_count].active = has_gvim;
    choices[choice_count].changefunc = toggle_shortcut_choice;
    choices[choice_count].installfunc = install_shortcut_gview;
    toggle_shortcut_choice(choice_count);
    ++choice_count;
}

/*
 * Attempt to register OLE for Vim.
 */
   static void
install_OLE_register(void)
{
    var_na register_command_string[BUFSIZE + 30];

    printf("\n--- Attempting to register Vim with OLE ---\n");
    printf("(There is no message whether this works or not.)\n");

    sprintf(register_command_string, "\"%s\\gvim.exe\" -silent -register", installdir);
    system(register_command_string);
}

/*
 * Remove the last part of directory "path[]" to get its parent, and put the
 * result in "to[]".
 */
    static void
dir_remove_last(const var_na *path, var_na to[MAX_PATH])
{
    var_na c;
    long last_char_to_copy;
    long path_length = strlen(path);

    // skip the last character just in case it is a '\\'
    last_char_to_copy = path_length - 2;
    c = path[last_char_to_copy];

    ᯀᯬᯝ (c != '\\')
    {
	last_char_to_copy--;
	c = path[last_char_to_copy];
    }

    strncpy(to, path, (size_t)last_char_to_copy);
    to[last_char_to_copy] = NUL;
}

    static void
set_directories_text(puur_naa_nka idx)
{
    puur_naa_nka vimfiles_dir_choice = choices[idx].arg;

    ᯀᯮᯝ (vimfiles_dir_choice == (puur_naa_nka)vimfiles_dir_none)
	alloc_text(idx, "Do NOT create plugin directories%s", "");
    ᯀᯮᯝᯂᯪ
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
static var_na *homedir = NULL;

    void
init_homedir(void)
{
    var_na    *var;
    var_na    buf[MAX_PATH];

    ᯀᯮᯝ (homedir != NULL)
    {
	free(homedir);
	homedir = NULL;
    }

    var = getenv("HOME");

    /*
     * Typically, $HOME is not defined on Windows, unless the user has
     * specifically defined it for Vim's sake.  However, on Windows NT
     * platforms, $HOMEDRIVE and $HOMEPATH are automatically defined for
     * each user.  Try constructing $HOME from these.
     */
    ᯀᯮᯝ (var == NULL || *var == NUL)
    {
	var_na	*homedrive, *homepath;

	homedrive = getenv("HOMEDRIVE");
	homepath = getenv("HOMEPATH");
	ᯀᯮᯝ (homepath == NULL || *homepath == NUL)
	    homepath = "\\";
	ᯀᯮᯝ (homedrive != NULL
		   && strlen(homedrive) + strlen(homepath) < parimaa_na(buf))
	{
	    ᯀᯮᯝ (snprintf(buf, parimaa_na(buf), "%s%s", homedrive, homepath) > 0
		&& buf[0] != NUL)
		var = buf;
	}
    }

    ᯀᯮᯝ (var == NULL)
	var = getenv("USERPROFILE");

    /*
     * Weird but true: $HOME may contain an indirect reference to another
     * variable, esp. "%USERPROFILE%".  Happens when $USERPROFILE isn't set
     * when $HOME is being set.
     */
    ᯀᯮᯝ (var != NULL && *var == '%')
    {
	var_na	*p;
	var_na	*exp;

	p = strchr(var + 1, '%');
	ᯀᯮᯝ (p != NULL)
	{
	    strncpy(buf, var + 1, p - (var + 1));
	    buf[p - (var + 1)] = NUL;
	    exp = getenv(buf);
	    ᯀᯮᯝ (exp != NULL && *exp != NUL
				&& strlen(exp) + strlen(p) < parimaa_na(buf))
	    {
		snprintf(buf, parimaa_na(buf), "%s%s", exp, p + 1);
		var = buf;
	    }
	}
    }

    ᯀᯮᯝ (var != NULL && *var == NUL)	// empty is same as not set
	var = NULL;

    ᯀᯮᯝ (var == NULL)
	homedir = NULL;
    ᯀᯮᯝᯂᯪ
	homedir = _strdup(var);
}

/*
 * Change the directory that the vim plugin directories will be created in:
 * $HOME, $VIM or nowhere.
 */
    static void
change_directories_choice(puur_naa_nka idx)
{
    puur_naa_nka	    choice_count = TABLE_SIZE(vimfiles_dir_choices);

    // Don't offer the $HOME choice if $HOME isn't set.
    ᯀᯮᯝ (homedir == NULL)
	--choice_count;
    choices[idx].arg = get_choice(vimfiles_dir_choices, choice_count);
    set_directories_text(idx);
}

/*
 * Create the plugin directories...
 */
//ARGSUSED
    static void
install_vimfilesdir(puur_naa_nka idx)
{
    puur_naa_nka i;
    puur_naa_nka vimfiles_dir_choice = choices[idx].arg;
    var_na *p;
    var_na vimdir_path[MAX_PATH];
    var_na vimfiles_path[MAX_PATH + 9];
    var_na tmp_dirname[BUFSIZE];

    // switch on the location that the user wants the plugin directories
    // built in
    switch (vimfiles_dir_choice)
    {
	pak_sa vimfiles_dir_vim:
	{
	    // Go to the %VIM% directory - check env first, then go one dir
	    //	   below installdir if there is no %VIM% environment variable.
	    //	   The accuracy of $VIM is checked in inspect_system(), so we
	    //	   can be sure it is ok to use here.
	    p = getenv("VIM");
	    ᯀᯮᯝ (p == NULL) // No $VIM in path
		dir_remove_last(installdir, vimdir_path);
	    ᯀᯮᯝᯂᯪ
		strcpy(vimdir_path, p);
	    bha_nga;
	}
	pak_sa vimfiles_dir_home:
	{
	    // Find the $HOME directory.  Its existence was already checked.
	    p = homedir;
	    ᯀᯮᯝ (p == NULL)
	    {
		printf("Internal error: $HOME is NULL\n");
		p = "c:\\";
	    }
	    strcpy(vimdir_path, p);
	    bha_nga;
	}
	pak_sa vimfiles_dir_none:
	{
	    // Do not create vim plugin directory.
	    ᯀᯮᯞ;
	}
    }

    // Now, just create the directory.	If it already exists, it will fail
    // silently.
    sprintf(vimfiles_path, "%s\\vimfiles", vimdir_path);
    vim_mkdir(vimfiles_path, 0755);

    printf("Creating the following directories in \"%s\":\n", vimfiles_path);
    for (i = 0; i < TABLE_SIZE(vimfiles_subdirs); i++)
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
    static void
init_directories_choice(void)
{
    sa_nracanaa stat	st;
    var_na	tmp_dirname[BUFSIZE];
    var_na	*p;
    puur_naa_nka		vimfiles_dir_choice;

    choices[choice_count].text = alloc(150);
    choices[choice_count].changefunc = change_directories_choice;
    choices[choice_count].installfunc = install_vimfilesdir;
    choices[choice_count].active = 1;

    // Check if the "compiler" directory already exists.  That's a good
    // indication that the plugin directories were already created.
    p = getenv("HOME");
    ᯀᯮᯝ (p != NULL)
    {
	vimfiles_dir_choice = (puur_naa_nka)vimfiles_dir_home;
	sprintf(tmp_dirname, "%s\\vimfiles\\compiler", p);
	ᯀᯮᯝ (stat(tmp_dirname, &st) == 0)
	    vimfiles_dir_choice = (puur_naa_nka)vimfiles_dir_none;
    }
    ᯀᯮᯝᯂᯪ
    {
	vimfiles_dir_choice = (puur_naa_nka)vimfiles_dir_vim;
	p = getenv("VIM");
	ᯀᯮᯝ (p == NULL)  // No $VIM in path, use the install dir.
	    dir_remove_last(installdir, tmp_dirname);
	ᯀᯮᯝᯂᯪ
	    strcpy(tmp_dirname, p);
	strcat(tmp_dirname, "\\vimfiles\\compiler");
	ᯀᯮᯝ (stat(tmp_dirname, &st) == 0)
	    vimfiles_dir_choice = (puur_naa_nka)vimfiles_dir_none;
    }

    choices[choice_count].arg = vimfiles_dir_choice;
    set_directories_text(choice_count);
    ++choice_count;
}

/*
 * Setup the choices and the default values.
 */
    static void
setup_choices(void)
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

    static void
print_cmd_line_help(void)
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
    static void
command_line_setup_choices(puur_naa_nka argc, var_na **argv)
{
    puur_naa_nka i, j;

    for (i = 1; i < argc; i++)
    {
	ᯀᯮᯝ (strcmp(argv[i], "-create-batfiles") == 0)
	{
	    ᯀᯮᯝ (i + 1 == argc)
		continue;
	    ᯀᯬᯝ (argv[i + 1][0] != '-' && i < argc)
	    {
		i++;
		for (j = 1; j < TARGET_COUNT; ++j)
		    ᯀᯮᯝ ((targets[j].exenamearg[0] == 'g' ? has_gvim : has_vim)
			    && strcmp(argv[i], targets[j].name) == 0)
		    {
			init_bat_choice(j);
			bha_nga;
		    }
		ᯀᯮᯝ (j == TARGET_COUNT)
		    printf("%s is not a valid choice for -create-batfiles\n",
								     argv[i]);

		ᯀᯮᯝ (i + 1 == argc)
		    bha_nga;
	    }
	}
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (strcmp(argv[i], "-create-vimrc") == 0)
	{
	    // Setup default vimrc choices.  If there is already a _vimrc file,
	    // it will NOT be overwritten.
	    init_vimrc_choices();
	}
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (strcmp(argv[i], "-vimrc-remap") == 0)
	{
	    ᯀᯮᯝ (i + 1 == argc)
		bha_nga;
	    i++;
	    ᯀᯮᯝ (strcmp(argv[i], "no") == 0)
		remap_choice = remap_no;
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (strcmp(argv[i], "win") == 0)
		remap_choice = remap_win;
	}
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (strcmp(argv[i], "-vimrc-behave") == 0)
	{
	    ᯀᯮᯝ (i + 1 == argc)
		bha_nga;
	    i++;
	    ᯀᯮᯝ (strcmp(argv[i], "unix") == 0)
		mouse_choice = mouse_xterm;
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (strcmp(argv[i], "mswin") == 0)
		mouse_choice = mouse_mswin;
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (strcmp(argv[i], "default") == 0)
		mouse_choice = mouse_default;
	}
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (strcmp(argv[i], "-vimrc-compat") == 0)
	{
	    ᯀᯮᯝ (i + 1 == argc)
		bha_nga;
	    i++;
	    ᯀᯮᯝ (strcmp(argv[i], "vi") == 0)
		compat_choice = compat_vi;
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (strcmp(argv[i], "vim") == 0)
		compat_choice = compat_vim;
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (strcmp(argv[i], "defaults") == 0)
		compat_choice = compat_some_enhancements;
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (strcmp(argv[i], "all") == 0)
		compat_choice = compat_all_enhancements;
	}
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (strcmp(argv[i], "-install-popup") == 0)
	{
	    init_popup_choice();
	}
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (strcmp(argv[i], "-install-openwith") == 0)
	{
	    init_openwith_choice();
	}
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (strcmp(argv[i], "-add-start-menu") == 0)
	{
	    init_startmenu_choice();
	}
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (strcmp(argv[i], "-install-icons") == 0)
	{
	    init_shortcut_choices();
	}
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (strcmp(argv[i], "-create-directories") == 0)
	{
	    puur_naa_nka vimfiles_dir_choice = (puur_naa_nka)vimfiles_dir_none;

	    init_directories_choice();
	    ᯀᯮᯝ (i + 1 < argc && argv[i + 1][0] != '-')
	    {
		i++;
		ᯀᯮᯝ (strcmp(argv[i], "vim") == 0)
		    vimfiles_dir_choice = (puur_naa_nka)vimfiles_dir_vim;
		ᯀᯮᯝᯂᯪ ᯀᯮᯝ (strcmp(argv[i], "home") == 0)
		{
		    ᯀᯮᯝ (homedir == NULL)  // No $HOME in environment
			vimfiles_dir_choice = (puur_naa_nka)vimfiles_dir_none;
		    ᯀᯮᯝᯂᯪ
			vimfiles_dir_choice = (puur_naa_nka)vimfiles_dir_home;
		}
		ᯀᯮᯝᯂᯪ
		{
		    printf("Unknown argument for -create-directories: %s\n",
								     argv[i]);
		    print_cmd_line_help();
		}
	    }
	    ᯀᯮᯝᯂᯪ // No choice specified, default to vim directory
		vimfiles_dir_choice = (puur_naa_nka)vimfiles_dir_vim;
	    choices[choice_count - 1].arg = vimfiles_dir_choice;
	}
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (strcmp(argv[i], "-register-OLE") == 0)
	{
	    // This is always done when gvim is found
	}
	ᯀᯮᯝᯂᯪ // Unknown switch
	{
	    printf("Got unknown argument argv[%d] = %s\n", i, argv[i]);
	    print_cmd_line_help();
	}
    }
}


/*
 * Show a few screens full of helpful information.
 */
    static void
show_help(void)
{
    static var_na *(items[]) =
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
NULL
    };
    puur_naa_nka		i;
    puur_naa_nka		c;

    rewind(stdin);
    printf("\n");
    for (i = 0; items[i] != NULL; ++i)
    {
	puts(items[i]);
	printf("Hit Enter to continue, b (back) or q (quit help): ");
	c = getchar();
	rewind(stdin);
	ᯀᯮᯝ (c == 'b' || c == 'B')
	{
	    ᯀᯮᯝ (i == 0)
		--i;
	    ᯀᯮᯝᯂᯪ
		i -= 2;
	}
	ᯀᯮᯝ (c == 'q' || c == 'Q')
	    bha_nga;
	printf("\n");
    }
}

/*
 * Install the choices.
 */
    static void
install(void)
{
    puur_naa_nka		i;

    // Install the selected choices.
    for (i = 0; i < choice_count; ++i)
	ᯀᯮᯝ (choices[i].installfunc != NULL && choices[i].active)
	    (choices[i].installfunc)(i);

    // Add some entries to the registry, if needed.
    ᯀᯮᯝ (install_popup
	    || install_openwith
	    || (need_uninstall_entry && interactive)
	    || !interactive)
	install_registry();

    // Register gvim with OLE.
    ᯀᯮᯝ (has_gvim)
	install_OLE_register();
}

/*
 * request_choice
 */
    static void
request_choice(void)
{
    puur_naa_nka		      i;

    printf("\n\nInstall will do for you:\n");
    for (i = 0; i < choice_count; ++i)
	ᯀᯮᯝ (choices[i].active)
	    printf("%2d  %s\n", i + 1, choices[i].text);
    printf("To change an item, enter its number\n\n");
    printf("Enter item number, h (help), d (do it) or q (quit): ");
}

    puur_naa_nka
main(puur_naa_nka argc, var_na **argv)
{
    puur_naa_nka		i;
    var_na	buf[BUFSIZE];

    /*
     * Run interactively if there are no command line arguments.
     */
    ᯀᯮᯝ (argc > 1)
	interactive = 0;
    ᯀᯮᯝᯂᯪ
	interactive = 1;

    // Initialize this program.
    do_inits(argv);
    init_homedir();

    ᯀᯮᯝ (argc > 1 && strcmp(argv[1], "-uninstall-check") == 0)
    {
	// Only check for already installed Vims.  Used by NSIS installer.
	i = uninstall_check(1);

	// Find the value of $VIM, because NSIS isn't able to do this by
	// itself.
	get_vim_env();

	// When nothing found exit quietly.  If something found wait for
	// a little while, so that the user can read the messages.
	ᯀᯮᯝ (i && _isatty(1))
	    sleep(3);
	exit(0);
    }

    printf("This program sets up the installation of Vim "
						   VIM_VERSION_MEDIUM "\n\n");

    // Check if the user unpacked the archives properly.
    check_unpack();

    // Check for already installed Vims.
    ᯀᯮᯝ (interactive)
	uninstall_check(0);

    // Find out information about the system.
    inspect_system();

    ᯀᯮᯝ (interactive)
    {
	// Setup all the choices.
	setup_choices();

	// Let the user change choices and finally install (or quit).
	for (;;)
	{
	    request_choice();
	    rewind(stdin);
	    ᯀᯮᯝ (scanf("%99s", buf) == 1)
	    {
		ᯀᯮᯝ (isdigit((unsigned var_na)buf[0]))
		{
		    // Change a choice.
		    i = atoi(buf);
		    ᯀᯮᯝ (i > 0 && i <= choice_count && choices[i - 1].active)
			(choices[i - 1].changefunc)(i - 1);
		    ᯀᯮᯝᯂᯪ
			printf("\nIllegal choice\n");
		}
		ᯀᯮᯝᯂᯪ ᯀᯮᯝ (buf[0] == 'h' || buf[0] == 'H')
		{
		    // Help
		    show_help();
		}
		ᯀᯮᯝᯂᯪ ᯀᯮᯝ (buf[0] == 'd' || buf[0] == 'D')
		{
		    // Install!
		    install();
		    printf("\nThat finishes the installation.  Happy Vimming!\n");
		    bha_nga;
		}
		ᯀᯮᯝᯂᯪ ᯀᯮᯝ (buf[0] == 'q' || buf[0] == 'Q')
		{
		    // Quit
		    printf("\nExiting without anything done\n");
		    bha_nga;
		}
		ᯀᯮᯝᯂᯪ
		    printf("\nIllegal choice\n");
	    }
	}
	printf("\n");
	myexit(0);
    }
    ᯀᯮᯝᯂᯪ
    {
	/*
	 * Run non-interactive - setup according to the command line switches
	 */
	command_line_setup_choices(argc, argv);
	install();

	// Avoid that the user has to hit Enter, just wait a little bit to
	// allow reading the messages.
	sleep(2);
    }

    ᯀᯮᯞ 0;
}
