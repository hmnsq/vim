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
	ако (code != ERROR_SUCCESS) \
	{ \
	    printf("%ld error number:  %ld\n", (long)__LINE__, (long)code); \
	    врати 1; \
	} \
    } док (0)

цео	has_vim = 0;		// installable vim.exe exists
цео	has_gvim = 0;		// installable gvim.exe exists

char	oldvimrc[BUFSIZE];	// name of existing vimrc file
char	vimrc[BUFSIZE];		// name of vimrc file to create

char	*default_bat_dir = NULL;  // when not NULL, use this as the default
				  // directory to write .bat files in
char	*default_vim_dir = NULL;  // when not NULL, use this as the default
				  // install dir for NSIS

/*
 * Structure used for each choice the user can make.
 */
struct choice
{
    цео	    active;			// non-zero when choice is active
    char    *text;			// text displayed for this choice
    void    (*changefunc)(цео idx);	// function to change this choice
    цео	    arg;			// argument for function
    void    (*installfunc)(цео idx);	// function to install this choice
};

struct choice	choices[30];		// choices the user can make
цео		choice_count = 0;	// number of choices available

#define TABLE_SIZE(s)	(цео)ARRAYSIZE(s)

enum
{
    compat_vi = 1,
    compat_vim,
    compat_some_enhancements,
    compat_all_enhancements
};
char	*(compat_choices[]) =
{
    "\nChoose the default way to run Vim:",
    "Vi compatible",
    "Vim default",
    "with some Vim enhancements",
    "with syntax highlighting and other features switched on",
};
цео	compat_choice = (цео)compat_all_enhancements;
char	*compat_text = "- run Vim %s";

enum
{
    remap_no = 1,
    remap_win
};
char	*(remap_choices[]) =
{
    "\nChoose:",
    "Do not remap keys for Windows behavior",
    "Remap a few keys for Windows behavior (CTRL-V, CTRL-C, CTRL-F, etc)",
};
цео	remap_choice = (цео)remap_no;
char	*remap_text = "- %s";

enum
{
    mouse_xterm = 1,
    mouse_mswin,
    mouse_default
};
char	*(mouse_choices[]) =
{
    "\nChoose the way how Vim uses the mouse:",
    "right button extends selection (the Unix way)",
    "right button has a popup menu, left button starts select mode (the Windows way)",
    "right button has a popup menu, left button starts visual mode",
};
цео	mouse_choice = (цео)mouse_default;
char	*mouse_text = "- The mouse %s";

enum
{
    vimfiles_dir_none = 1,
    vimfiles_dir_vim,
    vimfiles_dir_home
};
static char *(vimfiles_dir_choices[]) =
{
    "\nCreate plugin directories:",
    "No",
    "In the VIM directory",
    "In your HOME directory",
};

// non-zero when selected to install the popup menu entry.
static цео	install_popup = 0;

// non-zero when selected to install the "Open with" entry.
static цео	install_openwith = 0;

// non-zero when need to add an uninstall entry in the registry
static цео	need_uninstall_entry = 0;

/*
 * Definitions of the directory name (under $VIM) of the vimfiles directory
 * and its subdirectories:
 */
static char	*(vimfiles_subdirs[]) =
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
    static цео
get_choice(char **table, цео entries)
{
    цео		answer;
    цео		idx;
    char	dummy[100];

    do
    {
	за (idx = 0; idx < entries; ++idx)
	{
	    ако (idx)
		printf("%2d  ", idx);
	    puts(table[idx]);
	}
	printf("Choice: ");
	ако (scanf("%d", &answer) != 1)
	{
	    scanf("%99s", dummy);
	    answer = 0;
	}
    }
    док (answer < 1 || answer >= entries);

    врати answer;
}

/*
 * Check if the user unpacked the archives properly.
 * Sets "runtimeidx".
 */
    static void
check_unpack(void)
{
    char	buf[BUFSIZE];
    FILE	*fd;
    struct stat	st;

    // check for presence of the correct version number in installdir[]
    runtimeidx = strlen(installdir) - strlen(VIM_VERSION_NODOT);
    ако (runtimeidx <= 0
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
    ако (stat(buf, &st) < 0)
    {
	printf("ERROR: Cannot find filetype.vim in \"%s\"\n", installdir);
	printf("It looks like you did not unpack the runtime archive.\n");
	printf("You must unpack the runtime archive \"%srt.zip\" before installing.\n",
		VIM_VERSION_NODOT);
	myexit(1);
    }

    // Check if vim.exe or gvim.exe is in the current directory.
    ако ((fd = fopen("gvim.exe", "r")) != NULL)
    {
	fclose(fd);
	has_gvim = 1;
    }
    ако ((fd = fopen("vim.exe", "r")) != NULL)
    {
	fclose(fd);
	has_vim = 1;
    }
    ако (!has_gvim && !has_vim)
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
    static цео
pathcmp(char *p, цео plen, char *q, цео qlen)
{
    цео		i;

    ако (plen < 0)
	plen = strlen(p);
    ако (qlen < 0)
	qlen = strlen(q);
    за (i = 0; ; ++i)
    {
	// End of "p": check if "q" also ends or just has a slash.
	ако (i == plen)
	{
	    ако (i == qlen)  // match
		врати 0;
	    ако (i == qlen - 1 && (q[i] == '\\' || q[i] == '/'))
		врати 0;   // match with trailing slash
	    врати 1;	    // no match
	}

	// End of "q": check if "p" also ends or just has a slash.
	ако (i == qlen)
	{
	    ако (i == plen)  // match
		врати 0;
	    ако (i == plen - 1 && (p[i] == '\\' || p[i] == '/'))
		врати 0;   // match with trailing slash
	    врати 1;	    // no match
	}

	ако (!(mytoupper(p[i]) == mytoupper(q[i])
		|| ((p[i] == '/' || p[i] == '\\')
		    && (q[i] == '/' || q[i] == '\\'))))
	    врати 1;	    // no match
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
findoldfile(char **destination)
{
    char	*bp = *destination;
    size_t	indir_l = strlen(installdir);
    char	*cp;
    char	*tmpname;
    char	*farname;

    /*
     * No action needed if exe not found or not in this directory.
     */
    ако (bp == NULL || strnicmp(bp, installdir, indir_l) != 0)
	врати;
    cp = bp + indir_l;
    ако (strchr("/\\", *cp++) == NULL
	    || strchr(cp, '\\') != NULL
	    || strchr(cp, '/') != NULL)
	врати;

    tmpname = alloc(strlen(cp) + 1);
    strcpy(tmpname, cp);
    tmpname[strlen(tmpname) - 1] = 'x';	// .exe -> .exx

    ако (access(tmpname, 0) == 0)
    {
	printf("\nERROR: %s and %s clash.  Remove or rename %s.\n",
	    tmpname, cp, tmpname);
	myexit(1);
    }

    ако (rename(cp, tmpname) != 0)
    {
	printf("\nERROR: failed to rename %s to %s: %s\n",
	    cp, tmpname, strerror(0));
	myexit(1);
    }

    farname = searchpath_save(cp);

    ако (rename(tmpname, cp) != 0)
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
find_bat_exe(цео check_bat_only)
{
    цео		i;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    за (i = 1; i < TARGET_COUNT; ++i)
    {
	targets[i].oldbat = searchpath_save(targets[i].batname);
	ако (!check_bat_only)
	    targets[i].oldexe = searchpath_save(targets[i].exename);

	ако (default_bat_dir == NULL && targets[i].oldbat != NULL)
	{
	    default_bat_dir = alloc(strlen(targets[i].oldbat) + 1);
	    strcpy(default_bat_dir, targets[i].oldbat);
	    remove_tail(default_bat_dir);
	}
	ако (check_bat_only && targets[i].oldbat != NULL)
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
    char	*vim;
    char	buf[BUFSIZE];
    FILE	*fd;
    char	fname[BUFSIZE];

    // First get $VIMRUNTIME.  If it's set, remove the tail.
    vim = getenv("VIMRUNTIME");
    ако (vim != NULL && *vim != 0 && strlen(vim) < sizeof(buf))
    {
	strcpy(buf, vim);
	remove_tail(buf);
	vim = buf;
    }
    инако
    {
	vim = getenv("VIM");
	ако (vim == NULL || *vim == 0)
	{
	    // Use the directory from an old uninstall entry.
	    ако (default_vim_dir != NULL)
		vim = default_vim_dir;
	    инако
		// Let NSIS know there is no default, it should use
		// $PROGRAMFILES.
		vim = "";
	}
    }

    // NSIS also uses GetTempPath(), thus we should get the same directory
    // name as where NSIS will look for vimini.ini.
    GetTempPath(sizeof(fname) - 12, fname);
    add_pathsep(fname);
    strcat(fname, "vimini.ini");

    fd = fopen(fname, "w");
    ако (fd != NULL)
    {
	// Make it look like an .ini file, so that NSIS can read it with a
	// ReadINIStr command.
	fprintf(fd, "[vimini]\n");
	fprintf(fd, "dir=\"%s\"\n", vim);
	fclose(fd);
    }
    инако
    {
	printf("Failed to open %s\n", fname);
	sleep(2);
    }
}

static цео num_windows;

/*
 * Callback used for EnumWindows():
 * Count the window if the title looks like it is for the uninstaller.
 */
//ARGSUSED
    static BOOL CALLBACK
window_cb(HWND hwnd, LPARAM lparam UNUSED)
{
    char title[256];

    title[0] = 0;
    GetWindowText(hwnd, title, 256);
    ако (strstr(title, "Vim ") != NULL && strstr(title, " Uninstall") != NULL)
	++num_windows;
    врати TRUE;
}

/*
 * Run the uninstaller silently.
 */
    static цео
run_silent_uninstall(char *uninst_exe)
{
    char    vimrt_dir[BUFSIZE];
    char    temp_uninst[BUFSIZE];
    char    temp_dir[MAX_PATH];
    char    buf[BUFSIZE * 2 + 10];
    цео	    i;
    DWORD   tick;

    strcpy(vimrt_dir, uninst_exe);
    remove_tail(vimrt_dir);

    ако (!GetTempPath(sizeof(temp_dir), temp_dir))
	врати FAIL;

    // Copy the uninstaller to a temporary exe.
    tick = GetTickCount();
    за (i = 0; ; i++)
    {
	sprintf(temp_uninst, "%s\\vimun%04X.exe", temp_dir,
					  (unsigned цео)((i + tick) & 0xFFFF));
	ако (CopyFile(uninst_exe, temp_uninst, TRUE))
	    break;
	ако (GetLastError() != ERROR_FILE_EXISTS)
	    врати FAIL;
	ако (i == 65535)
	    врати FAIL;
    }

    // Run the copied uninstaller silently.
    ако (strchr(temp_uninst, ' ') != NULL)
	sprintf(buf, "\"%s\" /S _?=%s", temp_uninst, vimrt_dir);
    инако
	sprintf(buf, "%s /S _?=%s", temp_uninst, vimrt_dir);
    run_command(buf);

    DeleteFile(temp_uninst);
    врати OK;
}

/*
 * Check for already installed Vims.
 * Return non-zero when found one.
 */
    static цео
uninstall_check(цео skip_question)
{
    HKEY	key_handle;
    HKEY	uninstall_key_handle;
    char	*uninstall_key = "software\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
    char	subkey_name_buff[BUFSIZE];
    char	temp_string_buffer[BUFSIZE-2];
    DWORD	local_bufsize;
    FILETIME	temp_pfiletime;
    DWORD	key_index;
    char	input;
    long	code;
    DWORD	value_type;
    DWORD	orig_num_keys;
    DWORD	new_num_keys;
    DWORD	allow_silent;
    цео		foundone = 0;

    code = RegOpenKeyEx(HKEY_LOCAL_MACHINE, uninstall_key, 0,
				     KEY_WOW64_64KEY | KEY_READ, &key_handle);
    CHECK_REG_ERROR(code);

    key_index = 0;
    док (TRUE)
    {
	local_bufsize = sizeof(subkey_name_buff);
	ако (RegEnumKeyEx(key_handle, key_index, subkey_name_buff, &local_bufsize,
		NULL, NULL, NULL, &temp_pfiletime) == ERROR_NO_MORE_ITEMS)
	    break;

	ако (strncmp("Vim", subkey_name_buff, 3) == 0)
	{
	    // Open the key named Vim*
	    code = RegOpenKeyEx(key_handle, subkey_name_buff, 0,
			   KEY_WOW64_64KEY | KEY_READ, &uninstall_key_handle);
	    CHECK_REG_ERROR(code);

	    // get the DisplayName out of it to show the user
	    local_bufsize = sizeof(temp_string_buffer);
	    code = RegQueryValueEx(uninstall_key_handle, "displayname", 0,
		    &value_type, (LPBYTE)temp_string_buffer,
		    &local_bufsize);
	    CHECK_REG_ERROR(code);

	    allow_silent = 0;
	    ако (skip_question)
	    {
		DWORD varsize = sizeof(DWORD);

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

	    ако (skip_question)
		printf("\nRunning uninstall program for \"%s\"\n", temp_string_buffer);
	    инако
		printf("\nDo you want to uninstall \"%s\" now?\n(y)es/(n)o)  ", temp_string_buffer);
	    fflush(stdout);

	    // get the UninstallString
	    local_bufsize = sizeof(temp_string_buffer);
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
		ако (input != 'n')
		    printf("%c is an invalid reply.  Please enter either 'y' or 'n'\n", input);

		ако (skip_question)
		    input = 'y';
		инако
		{
		    rewind(stdin);
		    scanf("%c", &input);
		}
		switch (input)
		{
		    case 'y':
		    case 'Y':
			// save the number of uninstall keys so we can know if
			// it changed
			RegQueryInfoKey(key_handle, NULL, NULL, NULL,
					     &orig_num_keys, NULL, NULL, NULL,
						      NULL, NULL, NULL, NULL);

			// Find existing .bat files before deleting them.
			find_bat_exe(TRUE);

			ако (allow_silent)
			{
			    ако (run_silent_uninstall(temp_string_buffer)
								    == FAIL)
				allow_silent = 0; // Retry with non silent.
			}
			ако (!allow_silent)
			{
			    // Execute the uninstall program.  Put it in double
			    // quotes if there is an embedded space.
			    {
				char buf[BUFSIZE];

				ако (strchr(temp_string_buffer, ' ') != NULL)
				    sprintf(buf, "\"%s\"", temp_string_buffer);
				инако
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
			    ако (num_windows == 0)
			    {
				// Did not find the uninstaller, ask user to
				// press Enter when done. Just in case.
				printf("Press Enter when the uninstaller is finished\n");
				rewind(stdin);
				(void)getchar();
			    }
			    инако
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
				} док (num_windows > 0);
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
			ако (new_num_keys < orig_num_keys)
			    key_index--;

			input = 'y';
			break;

		    case 'n':
		    case 'N':
			// Do not uninstall
			input = 'n';
			break;

		    default: // just drop through and redo the loop
			break;
		}

	    } док (input != 'n' && input != 'y');

	    RegCloseKey(uninstall_key_handle);
	}

	key_index++;
    }
    RegCloseKey(key_handle);

    врати foundone;
}

/*
 * Find out information about the system.
 */
    static void
inspect_system(void)
{
    char	*p;
    char	buf[BUFSIZE];
    FILE	*fd;
    цео		i;
    цео		foundone;

    // This may take a little while, let the user know what we're doing.
    printf("Inspecting system...\n");

    /*
     * If $VIM is set, check that it's pointing to our directory.
     */
    p = getenv("VIM");
    ако (p != NULL && pathcmp(p, -1, installdir, runtimeidx - 1) != 0)
    {
	printf("------------------------------------------------------\n");
	printf("$VIM is set to \"%s\".\n", p);
	printf("This is different from where this version of Vim is:\n");
	strcpy(buf, installdir);
	*(buf + runtimeidx - 1) = NUL;
	printf("\"%s\"\n", buf);
	printf("You must adjust or remove the setting of $VIM,\n");
	ако (interactive)
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
    ако (p != NULL && pathcmp(p, -1, installdir, -1) != 0)
    {
	printf("------------------------------------------------------\n");
	printf("$VIMRUNTIME is set to \"%s\".\n", p);
	printf("This is different from where this version of Vim is:\n");
	printf("\"%s\"\n", installdir);
	printf("You must adjust or remove the setting of $VIMRUNTIME,\n");
	ако (interactive)
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
    за (i = 1; i < TARGET_COUNT; ++i)
    {
	findoldfile(&(targets[i].oldexe));
	ако (targets[i].oldexe != NULL)
	    foundone = 1;
    }

    ако (foundone)
    {
	printf("Warning: Found Vim executable(s) in your $PATH:\n");
	за (i = 1; i < TARGET_COUNT; ++i)
	    ако (targets[i].oldexe != NULL)
		printf("%s\n", targets[i].oldexe);
	printf("It will be used instead of the version you are installing.\n");
	printf("Please delete or rename it, or adjust your $PATH setting.\n");
    }

    /*
     * Check if there is an existing ../_vimrc or ../.vimrc file.
     */
    strcpy(oldvimrc, installdir);
    strcpy(oldvimrc + runtimeidx, "_vimrc");
    ако ((fd = fopen(oldvimrc, "r")) == NULL)
    {
	strcpy(oldvimrc + runtimeidx, "vimrc~1"); // short version of .vimrc
	ако ((fd = fopen(oldvimrc, "r")) == NULL)
	{
	    strcpy(oldvimrc + runtimeidx, ".vimrc");
	    fd = fopen(oldvimrc, "r");
	}
    }
    ако (fd != NULL)
	fclose(fd);
    инако
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
install_bat_choice(цео idx)
{
    char	*batpath = targets[choices[idx].arg].batpath;
    char	*oldname = targets[choices[idx].arg].oldbat;
    char	*exename = targets[choices[idx].arg].exenamearg;
    char	*vimarg = targets[choices[idx].arg].exearg;
    FILE	*fd;

    ако (*batpath == NUL)
	врати;

    fd = fopen(batpath, "w");
    ако (fd == NULL)
    {
	printf("\nERROR: Cannot open \"%s\" for writing.\n", batpath);
	врати;
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

    ако (*exename == 'g')
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

    ако (*exename == 'g')
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
    инако
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
alloc_text(цео idx, char *fmt, char *arg)
{
    ако (choices[idx].text != NULL)
	free(choices[idx].text);

    choices[idx].text = alloc(strlen(fmt) + strlen(arg) - 1);
    sprintf(choices[idx].text, fmt, arg);
}

/*
 * Toggle the "Overwrite .../vim.bat" to "Don't overwrite".
 */
    static void
toggle_bat_choice(цео idx)
{
    char	*batname = targets[choices[idx].arg].batpath;
    char	*oldname = targets[choices[idx].arg].oldbat;

    ако (*batname == NUL)
    {
	alloc_text(idx, "    Overwrite %s", oldname);
	strcpy(batname, oldname);
    }
    инако
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
set_bat_text(цео idx, char *batpath, char *name)
{
    strcat(batpath, name);

    alloc_text(idx, "    Create %s", batpath);
}

/*
 * Select a directory to write the batch file line.
 */
    static void
change_bat_choice(цео idx)
{
    char	*path;
    char	*batpath;
    char	*name;
    цео		n;
    char	*s;
    char	*p;
    цео		count;
    char	**names = NULL;
    цео		i;
    цео		target = choices[idx].arg;

    name = targets[target].batname;
    batpath = targets[target].batpath;

    path = getenv("PATH");
    ако (path == NULL)
    {
	printf("\nERROR: The variable $PATH is not set\n");
	врати;
    }

    /*
     * first round: count number of names in path;
     * second round: save names to names[].
     */
    за (;;)
    {
	count = 1;
	за (p = path; *p; )
	{
	    s = strchr(p, ';');
	    ако (s == NULL)
		s = p + strlen(p);
	    ако (names != NULL)
	    {
		names[count] = alloc(s - p + 1);
		strncpy(names[count], p, s - p);
		names[count][s - p] = NUL;
	    }
	    ++count;
	    p = s;
	    ако (*p != NUL)
		++p;
	}
	ако (names != NULL)
	    break;
	names = alloc((count + 1) * sizeof(char *));
    }
    names[0] = alloc(50);
    sprintf(names[0], "Select directory to create %s in:", name);
    names[count] = alloc(50);
    ако (choices[idx].arg == 0)
	sprintf(names[count], "Do not create any .bat file.");
    инако
	sprintf(names[count], "Do not create a %s file.", name);
    n = get_choice(names, count + 1);

    ако (n == count)
    {
	// Selected last item, don't create bat file.
	*batpath = NUL;
	ако (choices[idx].arg != 0)
	    alloc_text(idx, "    Do NOT create %s", name);
    }
    инако
    {
	// Selected one of the paths.  For the first item only keep the path,
	// for the others append the batch file name.
	strcpy(batpath, names[n]);
	add_pathsep(batpath);
	ако (choices[idx].arg != 0)
	    set_bat_text(idx, batpath, name);
    }

    за (i = 0; i <= count; ++i)
	free(names[i]);
    free(names);
}

char *bat_text_yes = "Install .bat files to use Vim at the command line:";
char *bat_text_no = "do NOT install .bat files to use Vim at the command line";

    static void
change_main_bat_choice(цео idx)
{
    цео		i;

    // let the user select a default directory or NONE
    change_bat_choice(idx);

    ако (targets[0].batpath[0] != NUL)
	choices[idx].text = bat_text_yes;
    инако
	choices[idx].text = bat_text_no;

    // update the individual batch file selections
    за (i = 1; i < TARGET_COUNT; ++i)
    {
	// Only make it active when the first item has a path and the vim.exe
	// or gvim.exe exists (there is a changefunc then).
	ако (targets[0].batpath[0] != NUL
		&& choices[idx + i].changefunc != NULL)
	{
	    choices[idx + i].active = 1;
	    ако (choices[idx + i].changefunc == change_bat_choice
		    && targets[i].batpath[0] != NUL)
	    {
		strcpy(targets[i].batpath, targets[0].batpath);
		set_bat_text(idx + i, targets[i].batpath, targets[i].batname);
	    }
	}
	инако
	    choices[idx + i].active = 0;
    }
}

/*
 * Initialize a choice for creating a batch file.
 */
    static void
init_bat_choice(цео target)
{
    char	*batpath = targets[target].batpath;
    char	*oldbat = targets[target].oldbat;
    char	*p;
    цео		i;

    choices[choice_count].arg = target;
    choices[choice_count].installfunc = install_bat_choice;
    choices[choice_count].active = 1;
    choices[choice_count].text = NULL;	// will be set below
    ако (oldbat != NULL)
    {
	// A [g]vim.bat exists: Only choice is to overwrite it or not.
	choices[choice_count].changefunc = toggle_bat_choice;
	*batpath = NUL;
	toggle_bat_choice(choice_count);
    }
    инако
    {
	ако (default_bat_dir != NULL)
	    // Prefer using the same path as an existing .bat file.
	    strcpy(batpath, default_bat_dir);
	инако
	{
	    // No [g]vim.bat exists: Write it to a directory in $PATH.  Use
	    // $WINDIR by default, if it's empty the first item in $PATH.
	    p = getenv("WINDIR");
	    ако (p != NULL && *p != NUL)
		strcpy(batpath, p);
	    инако
	    {
		p = getenv("PATH");
		ако (p == NULL || *p == NUL)		// "cannot happen"
		    strcpy(batpath, "C:/Windows");
		инако
		{
		    i = 0;
		    док (*p != NUL && *p != ';')
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
    цео		i;

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
    за (i = 1; i < TARGET_COUNT; ++i)
	ако (targets[i].oldexe == NULL
		&& (targets[i].exenamearg[0] == 'g' ? has_gvim : has_vim))
	    init_bat_choice(i);
	инако
	    add_dummy_choice();
}

/*
 * Install the vimrc file.
 */
    static void
install_vimrc(цео idx UNUSED)
{
    FILE	*fd, *tfd;
    char	*fname;

    // If an old vimrc file exists, overwrite it.
    // Otherwise create a new one.
    ако (*oldvimrc != NUL)
	fname = oldvimrc;
    инако
	fname = vimrc;

    fd = fopen(fname, "w");
    ако (fd == NULL)
    {
	printf("\nERROR: Cannot open \"%s\" for writing.\n", fname);
	врати;
    }
    switch (compat_choice)
    {
	case compat_vi:
		    fprintf(fd, "\" Vi compatible\n");
		    fprintf(fd, "set compatible\n");
		    break;
	case compat_vim:
		    fprintf(fd, "\" Vim's default behavior\n");
		    fprintf(fd, "if &compatible\n");
		    fprintf(fd, "  set nocompatible\n");
		    fprintf(fd, "endif\n");
		    break;
	case compat_some_enhancements:
		    fprintf(fd, "\" Vim with some enhancements\n");
		    fprintf(fd, "source $VIMRUNTIME/defaults.vim\n");
		    break;
	case compat_all_enhancements:
		    fprintf(fd, "\" Vim with all enhancements\n");
		    fprintf(fd, "source $VIMRUNTIME/vimrc_example.vim\n");
		    break;
    }
    switch (remap_choice)
    {
	case remap_no:
		    break;
	case remap_win:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Remap a few keys for Windows behavior\n");
		    fprintf(fd, "source $VIMRUNTIME/mswin.vim\n");
		    break;
    }
    switch (mouse_choice)
    {
	case mouse_xterm:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Mouse behavior (the Unix way)\n");
		    fprintf(fd, "behave xterm\n");
		    break;
	case mouse_mswin:
		    fprintf(fd, "\n");
		    fprintf(fd, "\" Mouse behavior (the Windows way)\n");
		    fprintf(fd, "behave mswin\n");
		    break;
	case mouse_default:
		    break;
    }
    ако ((tfd = fopen("diff.exe", "r")) != NULL)
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
change_vimrc_choice(цео idx)
{
    ако (choices[idx].installfunc != NULL)
    {
	// Switch to NOT change or create a vimrc file.
	ако (*oldvimrc != NUL)
	    alloc_text(idx, "Do NOT change startup file %s", oldvimrc);
	инако
	    alloc_text(idx, "Do NOT create startup file %s", vimrc);
	choices[idx].installfunc = NULL;
	choices[idx + 1].active = 0;
	choices[idx + 2].active = 0;
	choices[idx + 3].active = 0;
    }
    инако
    {
	// Switch to change or create a vimrc file.
	ако (*oldvimrc != NUL)
	    alloc_text(idx, "Overwrite startup file %s with:", oldvimrc);
	инако
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
change_run_choice(цео idx)
{
    compat_choice = get_choice(compat_choices, TABLE_SIZE(compat_choices));
    alloc_text(idx, compat_text, compat_choices[compat_choice]);
}

/*
 * Change the choice if keys are to be remapped.
 */
    static void
change_remap_choice(цео idx)
{
    remap_choice = get_choice(remap_choices, TABLE_SIZE(remap_choices));
    alloc_text(idx, remap_text, remap_choices[remap_choice]);
}

/*
 * Change the choice how to select text.
 */
    static void
change_mouse_choice(цео idx)
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
    ако (*oldvimrc == NUL)
	choices[choice_count].installfunc = NULL;
    инако
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
    const char *subkey,
    PHKEY phKey,
    DWORD flag)
{
    DWORD disp;

    *phKey = NULL;
    врати RegCreateKeyEx(
		root, subkey,
		0, NULL, REG_OPTION_NON_VOLATILE,
		flag | KEY_WRITE,
		NULL, phKey, &disp);
}

    static LONG
reg_set_string_value(
    HKEY hKey,
    const char *value_name,
    const char *data)
{
    врати RegSetValueEx(hKey, value_name, 0, REG_SZ,
				     (LPBYTE)data, (DWORD)(1 + strlen(data)));
}

    static LONG
reg_create_key_and_value(
    HKEY hRootKey,
    const char *subkey,
    const char *value_name,
    const char *data,
    DWORD flag)
{
    HKEY hKey;
    LONG lRet = reg_create_key(hRootKey, subkey, &hKey, flag);

    ако (ERROR_SUCCESS == lRet)
    {
	lRet = reg_set_string_value(hKey, value_name, data);
	RegCloseKey(hKey);
    }
    врати lRet;
}

    static LONG
register_inproc_server(
    HKEY hRootKey,
    const char *clsid,
    const char *extname,
    const char *module,
    const char *threading_model,
    DWORD flag)
{
    CHAR subkey[BUFSIZE];
    LONG lRet;

    sprintf(subkey, "CLSID\\%s", clsid);
    lRet = reg_create_key_and_value(hRootKey, subkey, NULL, extname, flag);
    ако (ERROR_SUCCESS == lRet)
    {
	sprintf(subkey, "CLSID\\%s\\InProcServer32", clsid);
	lRet = reg_create_key_and_value(hRootKey, subkey, NULL, module, flag);
	ако (ERROR_SUCCESS == lRet)
	{
	    lRet = reg_create_key_and_value(hRootKey, subkey,
					   "ThreadingModel", threading_model, flag);
	}
    }
    врати lRet;
}

    static LONG
register_shellex(
    HKEY hRootKey,
    const char *clsid,
    const char *name,
    const char *exe_path,
    DWORD flag)
{
    LONG lRet = reg_create_key_and_value(
	    hRootKey,
	    "*\\shellex\\ContextMenuHandlers\\gvim",
	    NULL,
	    clsid,
	    flag);

    ако (ERROR_SUCCESS == lRet)
    {
	lRet = reg_create_key_and_value(
		HKEY_LOCAL_MACHINE,
		"Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved",
		clsid,
		name,
		flag);

	ако (ERROR_SUCCESS == lRet)
	{
	    lRet = reg_create_key_and_value(
		    HKEY_LOCAL_MACHINE,
		    "Software\\Vim\\Gvim",
		    "path",
		    exe_path,
		    flag);
	}
    }
    врати lRet;
}

    static LONG
register_openwith(
    HKEY hRootKey,
    const char *exe_path,
    DWORD flag)
{
    char	exe_cmd[BUFSIZE];
    LONG	lRet;

    sprintf(exe_cmd, "\"%s\" \"%%1\"", exe_path);
    lRet = reg_create_key_and_value(
	    hRootKey,
	    "Applications\\gvim.exe\\shell\\edit\\command",
	    NULL,
	    exe_cmd,
	    flag);

    ако (ERROR_SUCCESS == lRet)
    {
	цео i;
	static const char *openwith[] = {
		".htm\\OpenWithList\\gvim.exe",
		".vim\\OpenWithList\\gvim.exe",
		"*\\OpenWithList\\gvim.exe",
	};

	за (i = 0; ERROR_SUCCESS == lRet && i < (цео)ARRAYSIZE(openwith); i++)
	    lRet = reg_create_key_and_value(hRootKey, openwith[i], NULL, "", flag);
    }

    врати lRet;
}

    static LONG
register_uninstall(
    HKEY hRootKey,
    const char *appname,
    const char *display_name,
    const char *uninstall_string,
    const char *display_icon,
    const char *display_version,
    const char *publisher)
{
    LONG lRet = reg_create_key_and_value(hRootKey, appname,
			     "DisplayName", display_name, KEY_WOW64_64KEY);

    ако (ERROR_SUCCESS == lRet)
	lRet = reg_create_key_and_value(hRootKey, appname,
		     "UninstallString", uninstall_string, KEY_WOW64_64KEY);
    ако (ERROR_SUCCESS == lRet)
	lRet = reg_create_key_and_value(hRootKey, appname,
		     "DisplayIcon", display_icon, KEY_WOW64_64KEY);
    ако (ERROR_SUCCESS == lRet)
	lRet = reg_create_key_and_value(hRootKey, appname,
		     "DisplayVersion", display_version, KEY_WOW64_64KEY);
    ако (ERROR_SUCCESS == lRet)
	lRet = reg_create_key_and_value(hRootKey, appname,
		     "Publisher", publisher, KEY_WOW64_64KEY);
    врати lRet;
}

/*
 * Add some entries to the registry:
 * - to add "Edit with Vim" to the context * menu
 * - to add Vim to the "Open with..." list
 * - to uninstall Vim
 */
//ARGSUSED
    static цео
install_registry(void)
{
    LONG	lRet = ERROR_SUCCESS;
    const char	*vim_ext_ThreadingModel = "Apartment";
    const char	*vim_ext_name = "Vim Shell Extension";
    const char	*vim_ext_clsid = "{51EEE242-AD87-11d3-9C1E-0090278BBD99}";
    char	vim_exe_path[MAX_PATH];
    char	display_name[BUFSIZE];
    char	uninstall_string[BUFSIZE];
    char	icon_string[BUFSIZE];
    char	version_string[BUFSIZE];
    цео		i;
    цео		loop_count = is_64bit_os() ? 2 : 1;
    DWORD	flag;

    sprintf(vim_exe_path, "%s\\gvim.exe", installdir);

    ако (install_popup)
    {
	char	    bufg[BUFSIZE];

	printf("Creating \"Edit with Vim\" popup menu entry\n");

	за (i = 0; i < loop_count; i++)
	{
	    ако (i == 0)
	    {
		sprintf(bufg, "%s\\" GVIMEXT32_PATH, installdir);
		flag = KEY_WOW64_32KEY;
	    }
	    инако
	    {
		sprintf(bufg, "%s\\" GVIMEXT64_PATH, installdir);
		flag = KEY_WOW64_64KEY;
	    }

	    lRet = register_inproc_server(
		    HKEY_CLASSES_ROOT, vim_ext_clsid, vim_ext_name,
		    bufg, vim_ext_ThreadingModel, flag);
	    ако (ERROR_SUCCESS != lRet)
		врати FAIL;
	    lRet = register_shellex(
		    HKEY_CLASSES_ROOT, vim_ext_clsid, vim_ext_name,
		    vim_exe_path, flag);
	    ако (ERROR_SUCCESS != lRet)
		врати FAIL;
	}
    }

    ако (install_openwith)
    {
	printf("Creating \"Open with ...\" list entry\n");

	за (i = 0; i < loop_count; i++)
	{
	    ако (i == 0)
		flag = KEY_WOW64_32KEY;
	    инако
		flag = KEY_WOW64_64KEY;

	    lRet = register_openwith(HKEY_CLASSES_ROOT, vim_exe_path, flag);
	    ако (ERROR_SUCCESS != lRet)
		врати FAIL;
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
    ако (interactive)
	sprintf(uninstall_string, "%s\\uninstall.exe", installdir);
    инако
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
    ако (ERROR_SUCCESS != lRet)
	врати FAIL;

    врати OK;
}

    static void
change_popup_choice(цео idx)
{
    ако (install_popup == 0)
    {
	choices[idx].text = "Install an entry for Vim in the popup menu for the right\n    mouse button so that you can edit any file with Vim";
	install_popup = 1;
    }
    инако
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
    struct stat	st;

    ако (has_gvim
	    && (stat(GVIMEXT32_PATH, &st) >= 0
		|| stat(GVIMEXT64_PATH, &st) >= 0))
    {
	choices[choice_count].changefunc = change_popup_choice;
	choices[choice_count].installfunc = NULL;
	choices[choice_count].active = 1;
	change_popup_choice(choice_count);  // set the text
	++choice_count;
    }
    инако
	add_dummy_choice();
}

    static void
change_openwith_choice(цео idx)
{
    ако (install_openwith == 0)
    {
	choices[idx].text = "Add Vim to the \"Open With...\" list in the popup menu for the right\n    mouse button so that you can edit any file with Vim";
	install_openwith = 1;
    }
    инако
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
    ако (has_gvim)
    {
	choices[choice_count].changefunc = change_openwith_choice;
	choices[choice_count].installfunc = NULL;
	choices[choice_count].active = 1;
	change_openwith_choice(choice_count);  // set the text
	++choice_count;
    }
    инако
	add_dummy_choice();
}

/*
 * Create a shell link.
 *
 * returns 0 on failure, non-zero on successful completion.
 *
 * NOTE:  Currently untested with mingw.
 */
    цео
create_shortcut(
	const char *shortcut_name,
	const char *iconfile_path,
	цео	    iconindex,
	const char *shortcut_target,
	const char *shortcut_args,
	const char *workingdir
	)
{
    IShellLink	    *shelllink_ptr;
    HRESULT	    hres;
    IPersistFile	    *persistfile_ptr;

    // Initialize COM library
    hres = CoInitialize(NULL);
    ако (!SUCCEEDED(hres))
    {
	printf("Error:  Could not open the COM library.  Not creating shortcut.\n");
	врати FAIL;
    }

    // Instantiate a COM object for the ShellLink, store a pointer to it
    // in shelllink_ptr.
    hres = CoCreateInstance(&CLSID_ShellLink,
			   NULL,
			   CLSCTX_INPROC_SERVER,
			   &IID_IShellLink,
			   (void **) &shelllink_ptr);

    ако (SUCCEEDED(hres)) // If the instantiation was successful...
    {
	// ...Then build a PersistFile interface for the ShellLink so we can
	// save it as a file after we build it.
	hres = shelllink_ptr->lpVtbl->QueryInterface(shelllink_ptr,
		&IID_IPersistFile, (void **) &persistfile_ptr);

	ако (SUCCEEDED(hres))
	{
	    wchar_t wsz[BUFSIZE];

	    // translate the (possibly) multibyte shortcut filename to windows
	    // Unicode so it can be used as a file name.
	    MultiByteToWideChar(CP_ACP, 0, shortcut_name, -1, wsz, sizeof(wsz)/sizeof(wsz[0]));

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
	инако
	{
	    printf("QueryInterface Error\n");
	    врати FAIL;
	}

	// Return the ShellLink object
	shelllink_ptr->lpVtbl->Release(shelllink_ptr);
    }
    инако
    {
	printf("CoCreateInstance Error - hres = %08x\n", (цео)hres);
	врати FAIL;
    }

    врати OK;
}

/*
 * Build a path to where we will put a specified link.
 *
 * Return 0 on error, non-zero on success
 */
   цео
build_link_name(
	char	   *link_path,
	const char *link_name,
	const char *shell_folder_name)
{
    char	shell_folder_path[MAX_PATH];

    ако (get_shell_folder_path(shell_folder_path, shell_folder_name) == FAIL)
    {
	printf("An error occurred while attempting to find the path to %s.\n",
							   shell_folder_name);
	врати FAIL;
    }

    // Make sure the directory exists (create Start Menu\Programs\Vim).
    // Ignore errors if it already exists.
    vim_mkdir(shell_folder_path, 0755);

    // build the path to the shortcut and the path to gvim.exe
    sprintf(link_path, "%s\\%s.lnk", shell_folder_path, link_name);

    врати OK;
}

    static цео
build_shortcut(
	const char *name,	// Name of the shortcut
	const char *exename,	// Name of the executable (e.g., vim.exe)
	const char *args,
	const char *shell_folder,
	const char *workingdir)
{
    char	executable_path[BUFSIZE];
    char	link_name[BUFSIZE];

    sprintf(executable_path, "%s\\%s", installdir, exename);

    ако (build_link_name(link_name, name, shell_folder) == FAIL)
    {
	printf("An error has occurred.  A shortcut to %s will not be created %s.\n",
		name,
		*shell_folder == 'd' ? "on the desktop" : "in the Start menu");
	врати FAIL;
    }

    // Create the shortcut:
    врати create_shortcut(link_name, executable_path, 0,
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
install_start_menu(цео idx UNUSED)
{
    need_uninstall_entry = 1;
    printf("Creating start menu\n");
    ако (has_vim)
    {
	ако (build_shortcut("Vim", "vim.exe", "",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    врати;
	ако (build_shortcut("Vim Read-only", "vim.exe", "-R",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    врати;
	ако (build_shortcut("Vim Diff", "vim.exe", "-d",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    врати;
    }
    ако (has_gvim)
    {
	ако (build_shortcut("gVim", "gvim.exe", "",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    врати;
	ако (build_shortcut("gVim Easy", "gvim.exe", "-y",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    врати;
	ако (build_shortcut("gVim Read-only", "gvim.exe", "-R",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    врати;
	ако (build_shortcut("gVim Diff", "gvim.exe", "-d",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	    врати;
    }
    ако (build_shortcut("Uninstall",
		interactive ? "uninstall.exe" : "uninstall-gui.exe", "",
					   VIM_STARTMENU, installdir) == FAIL)
	врати;
    // For Windows NT the working dir of the vimtutor.bat must be right,
    // otherwise gvim.exe won't be found and using gvimbat doesn't work.
    ако (build_shortcut("Vim tutor", "vimtutor.bat", "",
					   VIM_STARTMENU, installdir) == FAIL)
	врати;
    ако (build_shortcut("Help", has_gvim ? "gvim.exe" : "vim.exe", "-c h",
					      VIM_STARTMENU, WORKDIR) == FAIL)
	врати;
    {
	char	shell_folder_path[BUFSIZE];

	// Creating the URL shortcut works a bit differently...
	ако (get_shell_folder_path(shell_folder_path, VIM_STARTMENU) == FAIL)
	{
	    printf("Finding the path of the Start menu failed\n");
	    врати ;
	}
	add_pathsep(shell_folder_path);
	strcat(shell_folder_path, "Vim Online.url");
	ако (!WritePrivateProfileString("InternetShortcut", "URL",
				    "https://www.vim.org/", shell_folder_path))
	{
	    printf("Creating the Vim online URL failed\n");
	    врати;
	}
    }
}

    static void
toggle_startmenu_choice(цео idx)
{
    ако (choices[idx].installfunc == NULL)
    {
	choices[idx].installfunc = install_start_menu;
	choices[idx].text = "Add Vim to the Start menu";
    }
    инако
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
install_shortcut_gvim(цео idx)
{
    // Create shortcut(s) on the desktop
    ако (choices[idx].arg)
    {
	(void)build_shortcut(icon_names[0], "gvim.exe",
						      "", "desktop", WORKDIR);
	need_uninstall_entry = 1;
    }
}

    void
install_shortcut_evim(цео idx)
{
    ако (choices[idx].arg)
    {
	(void)build_shortcut(icon_names[1], "gvim.exe",
						    "-y", "desktop", WORKDIR);
	need_uninstall_entry = 1;
    }
}

    void
install_shortcut_gview(цео idx)
{
    ако (choices[idx].arg)
    {
	(void)build_shortcut(icon_names[2], "gvim.exe",
						    "-R", "desktop", WORKDIR);
	need_uninstall_entry = 1;
    }
}

    void
toggle_shortcut_choice(цео idx)
{
    char	*arg;

    ако (choices[idx].installfunc == install_shortcut_gvim)
	arg = "gVim";
    инако ако (choices[idx].installfunc == install_shortcut_evim)
	arg = "gVim Easy";
    инако
	arg = "gVim Read-only";
    ако (choices[idx].arg)
    {
	choices[idx].arg = 0;
	alloc_text(idx, "Do NOT create a desktop icon for %s", arg);
    }
    инако
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
    char register_command_string[BUFSIZE + 30];

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
dir_remove_last(const char *path, char to[MAX_PATH])
{
    char c;
    long last_char_to_copy;
    long path_length = strlen(path);

    // skip the last character just in case it is a '\\'
    last_char_to_copy = path_length - 2;
    c = path[last_char_to_copy];

    док (c != '\\')
    {
	last_char_to_copy--;
	c = path[last_char_to_copy];
    }

    strncpy(to, path, (size_t)last_char_to_copy);
    to[last_char_to_copy] = NUL;
}

    static void
set_directories_text(цео idx)
{
    цео vimfiles_dir_choice = choices[idx].arg;

    ако (vimfiles_dir_choice == (цео)vimfiles_dir_none)
	alloc_text(idx, "Do NOT create plugin directories%s", "");
    инако
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
static char *homedir = NULL;

    void
init_homedir(void)
{
    char    *var;
    char    buf[MAX_PATH];

    ако (homedir != NULL)
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
    ако (var == NULL || *var == NUL)
    {
	char	*homedrive, *homepath;

	homedrive = getenv("HOMEDRIVE");
	homepath = getenv("HOMEPATH");
	ако (homepath == NULL || *homepath == NUL)
	    homepath = "\\";
	ако (homedrive != NULL
		   && strlen(homedrive) + strlen(homepath) < sizeof(buf))
	{
	    ако (snprintf(buf, sizeof(buf), "%s%s", homedrive, homepath) > 0
		&& buf[0] != NUL)
		var = buf;
	}
    }

    ако (var == NULL)
	var = getenv("USERPROFILE");

    /*
     * Weird but true: $HOME may contain an indirect reference to another
     * variable, esp. "%USERPROFILE%".  Happens when $USERPROFILE isn't set
     * when $HOME is being set.
     */
    ако (var != NULL && *var == '%')
    {
	char	*p;
	char	*exp;

	p = strchr(var + 1, '%');
	ако (p != NULL)
	{
	    strncpy(buf, var + 1, p - (var + 1));
	    buf[p - (var + 1)] = NUL;
	    exp = getenv(buf);
	    ако (exp != NULL && *exp != NUL
				&& strlen(exp) + strlen(p) < sizeof(buf))
	    {
		snprintf(buf, sizeof(buf), "%s%s", exp, p + 1);
		var = buf;
	    }
	}
    }

    ако (var != NULL && *var == NUL)	// empty is same as not set
	var = NULL;

    ако (var == NULL)
	homedir = NULL;
    инако
	homedir = _strdup(var);
}

/*
 * Change the directory that the vim plugin directories will be created in:
 * $HOME, $VIM or nowhere.
 */
    static void
change_directories_choice(цео idx)
{
    цео	    choice_count = TABLE_SIZE(vimfiles_dir_choices);

    // Don't offer the $HOME choice if $HOME isn't set.
    ако (homedir == NULL)
	--choice_count;
    choices[idx].arg = get_choice(vimfiles_dir_choices, choice_count);
    set_directories_text(idx);
}

/*
 * Create the plugin directories...
 */
//ARGSUSED
    static void
install_vimfilesdir(цео idx)
{
    цео i;
    цео vimfiles_dir_choice = choices[idx].arg;
    char *p;
    char vimdir_path[MAX_PATH];
    char vimfiles_path[MAX_PATH + 9];
    char tmp_dirname[BUFSIZE];

    // switch on the location that the user wants the plugin directories
    // built in
    switch (vimfiles_dir_choice)
    {
	case vimfiles_dir_vim:
	{
	    // Go to the %VIM% directory - check env first, then go one dir
	    //	   below installdir if there is no %VIM% environment variable.
	    //	   The accuracy of $VIM is checked in inspect_system(), so we
	    //	   can be sure it is ok to use here.
	    p = getenv("VIM");
	    ако (p == NULL) // No $VIM in path
		dir_remove_last(installdir, vimdir_path);
	    инако
		strcpy(vimdir_path, p);
	    break;
	}
	case vimfiles_dir_home:
	{
	    // Find the $HOME directory.  Its existence was already checked.
	    p = homedir;
	    ако (p == NULL)
	    {
		printf("Internal error: $HOME is NULL\n");
		p = "c:\\";
	    }
	    strcpy(vimdir_path, p);
	    break;
	}
	case vimfiles_dir_none:
	{
	    // Do not create vim plugin directory.
	    врати;
	}
    }

    // Now, just create the directory.	If it already exists, it will fail
    // silently.
    sprintf(vimfiles_path, "%s\\vimfiles", vimdir_path);
    vim_mkdir(vimfiles_path, 0755);

    printf("Creating the following directories in \"%s\":\n", vimfiles_path);
    за (i = 0; i < TABLE_SIZE(vimfiles_subdirs); i++)
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
    struct stat	st;
    char	tmp_dirname[BUFSIZE];
    char	*p;
    цео		vimfiles_dir_choice;

    choices[choice_count].text = alloc(150);
    choices[choice_count].changefunc = change_directories_choice;
    choices[choice_count].installfunc = install_vimfilesdir;
    choices[choice_count].active = 1;

    // Check if the "compiler" directory already exists.  That's a good
    // indication that the plugin directories were already created.
    p = getenv("HOME");
    ако (p != NULL)
    {
	vimfiles_dir_choice = (цео)vimfiles_dir_home;
	sprintf(tmp_dirname, "%s\\vimfiles\\compiler", p);
	ако (stat(tmp_dirname, &st) == 0)
	    vimfiles_dir_choice = (цео)vimfiles_dir_none;
    }
    инако
    {
	vimfiles_dir_choice = (цео)vimfiles_dir_vim;
	p = getenv("VIM");
	ако (p == NULL)  // No $VIM in path, use the install dir.
	    dir_remove_last(installdir, tmp_dirname);
	инако
	    strcpy(tmp_dirname, p);
	strcat(tmp_dirname, "\\vimfiles\\compiler");
	ако (stat(tmp_dirname, &st) == 0)
	    vimfiles_dir_choice = (цео)vimfiles_dir_none;
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
command_line_setup_choices(цео argc, char **argv)
{
    цео i, j;

    за (i = 1; i < argc; i++)
    {
	ако (strcmp(argv[i], "-create-batfiles") == 0)
	{
	    ако (i + 1 == argc)
		continue;
	    док (argv[i + 1][0] != '-' && i < argc)
	    {
		i++;
		за (j = 1; j < TARGET_COUNT; ++j)
		    ако ((targets[j].exenamearg[0] == 'g' ? has_gvim : has_vim)
			    && strcmp(argv[i], targets[j].name) == 0)
		    {
			init_bat_choice(j);
			break;
		    }
		ако (j == TARGET_COUNT)
		    printf("%s is not a valid choice for -create-batfiles\n",
								     argv[i]);

		ако (i + 1 == argc)
		    break;
	    }
	}
	инако ако (strcmp(argv[i], "-create-vimrc") == 0)
	{
	    // Setup default vimrc choices.  If there is already a _vimrc file,
	    // it will NOT be overwritten.
	    init_vimrc_choices();
	}
	инако ако (strcmp(argv[i], "-vimrc-remap") == 0)
	{
	    ако (i + 1 == argc)
		break;
	    i++;
	    ако (strcmp(argv[i], "no") == 0)
		remap_choice = remap_no;
	    инако ако (strcmp(argv[i], "win") == 0)
		remap_choice = remap_win;
	}
	инако ако (strcmp(argv[i], "-vimrc-behave") == 0)
	{
	    ако (i + 1 == argc)
		break;
	    i++;
	    ако (strcmp(argv[i], "unix") == 0)
		mouse_choice = mouse_xterm;
	    инако ако (strcmp(argv[i], "mswin") == 0)
		mouse_choice = mouse_mswin;
	    инако ако (strcmp(argv[i], "default") == 0)
		mouse_choice = mouse_default;
	}
	инако ако (strcmp(argv[i], "-vimrc-compat") == 0)
	{
	    ако (i + 1 == argc)
		break;
	    i++;
	    ако (strcmp(argv[i], "vi") == 0)
		compat_choice = compat_vi;
	    инако ако (strcmp(argv[i], "vim") == 0)
		compat_choice = compat_vim;
	    инако ако (strcmp(argv[i], "defaults") == 0)
		compat_choice = compat_some_enhancements;
	    инако ако (strcmp(argv[i], "all") == 0)
		compat_choice = compat_all_enhancements;
	}
	инако ако (strcmp(argv[i], "-install-popup") == 0)
	{
	    init_popup_choice();
	}
	инако ако (strcmp(argv[i], "-install-openwith") == 0)
	{
	    init_openwith_choice();
	}
	инако ако (strcmp(argv[i], "-add-start-menu") == 0)
	{
	    init_startmenu_choice();
	}
	инако ако (strcmp(argv[i], "-install-icons") == 0)
	{
	    init_shortcut_choices();
	}
	инако ако (strcmp(argv[i], "-create-directories") == 0)
	{
	    цео vimfiles_dir_choice = (цео)vimfiles_dir_none;

	    init_directories_choice();
	    ако (i + 1 < argc && argv[i + 1][0] != '-')
	    {
		i++;
		ако (strcmp(argv[i], "vim") == 0)
		    vimfiles_dir_choice = (цео)vimfiles_dir_vim;
		инако ако (strcmp(argv[i], "home") == 0)
		{
		    ако (homedir == NULL)  // No $HOME in environment
			vimfiles_dir_choice = (цео)vimfiles_dir_none;
		    инако
			vimfiles_dir_choice = (цео)vimfiles_dir_home;
		}
		инако
		{
		    printf("Unknown argument for -create-directories: %s\n",
								     argv[i]);
		    print_cmd_line_help();
		}
	    }
	    инако // No choice specified, default to vim directory
		vimfiles_dir_choice = (цео)vimfiles_dir_vim;
	    choices[choice_count - 1].arg = vimfiles_dir_choice;
	}
	инако ако (strcmp(argv[i], "-register-OLE") == 0)
	{
	    // This is always done when gvim is found
	}
	инако // Unknown switch
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
    static char *(items[]) =
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
    цео		i;
    цео		c;

    rewind(stdin);
    printf("\n");
    за (i = 0; items[i] != NULL; ++i)
    {
	puts(items[i]);
	printf("Hit Enter to continue, b (back) or q (quit help): ");
	c = getchar();
	rewind(stdin);
	ако (c == 'b' || c == 'B')
	{
	    ако (i == 0)
		--i;
	    инако
		i -= 2;
	}
	ако (c == 'q' || c == 'Q')
	    break;
	printf("\n");
    }
}

/*
 * Install the choices.
 */
    static void
install(void)
{
    цео		i;

    // Install the selected choices.
    за (i = 0; i < choice_count; ++i)
	ако (choices[i].installfunc != NULL && choices[i].active)
	    (choices[i].installfunc)(i);

    // Add some entries to the registry, if needed.
    ако (install_popup
	    || install_openwith
	    || (need_uninstall_entry && interactive)
	    || !interactive)
	install_registry();

    // Register gvim with OLE.
    ако (has_gvim)
	install_OLE_register();
}

/*
 * request_choice
 */
    static void
request_choice(void)
{
    цео		      i;

    printf("\n\nInstall will do for you:\n");
    за (i = 0; i < choice_count; ++i)
	ако (choices[i].active)
	    printf("%2d  %s\n", i + 1, choices[i].text);
    printf("To change an item, enter its number\n\n");
    printf("Enter item number, h (help), d (do it) or q (quit): ");
}

    цео
main(цео argc, char **argv)
{
    цео		i;
    char	buf[BUFSIZE];

    /*
     * Run interactively if there are no command line arguments.
     */
    ако (argc > 1)
	interactive = 0;
    инако
	interactive = 1;

    // Initialize this program.
    do_inits(argv);
    init_homedir();

    ако (argc > 1 && strcmp(argv[1], "-uninstall-check") == 0)
    {
	// Only check for already installed Vims.  Used by NSIS installer.
	i = uninstall_check(1);

	// Find the value of $VIM, because NSIS isn't able to do this by
	// itself.
	get_vim_env();

	// When nothing found exit quietly.  If something found wait for
	// a little while, so that the user can read the messages.
	ако (i && _isatty(1))
	    sleep(3);
	exit(0);
    }

    printf("This program sets up the installation of Vim "
						   VIM_VERSION_MEDIUM "\n\n");

    // Check if the user unpacked the archives properly.
    check_unpack();

    // Check for already installed Vims.
    ако (interactive)
	uninstall_check(0);

    // Find out information about the system.
    inspect_system();

    ако (interactive)
    {
	// Setup all the choices.
	setup_choices();

	// Let the user change choices and finally install (or quit).
	за (;;)
	{
	    request_choice();
	    rewind(stdin);
	    ако (scanf("%99s", buf) == 1)
	    {
		ако (isdigit((unsigned char)buf[0]))
		{
		    // Change a choice.
		    i = atoi(buf);
		    ако (i > 0 && i <= choice_count && choices[i - 1].active)
			(choices[i - 1].changefunc)(i - 1);
		    инако
			printf("\nIllegal choice\n");
		}
		инако ако (buf[0] == 'h' || buf[0] == 'H')
		{
		    // Help
		    show_help();
		}
		инако ако (buf[0] == 'd' || buf[0] == 'D')
		{
		    // Install!
		    install();
		    printf("\nThat finishes the installation.  Happy Vimming!\n");
		    break;
		}
		инако ако (buf[0] == 'q' || buf[0] == 'Q')
		{
		    // Quit
		    printf("\nExiting without anything done\n");
		    break;
		}
		инако
		    printf("\nIllegal choice\n");
	    }
	}
	printf("\n");
	myexit(0);
    }
    инако
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

    врати 0;
}
