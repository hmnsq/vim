/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * uninstall.c:	Minimalistic uninstall program for Vim on MS-Windows
 *		Removes:
 *		- the "Edit with Vim" popup menu entry
 *		- the Vim "Open With..." popup menu entry
 *		- any Vim Batch files in the path
 *		- icons for Vim on the Desktop
 *		- the Vim entry in the Start Menu
 */

// Include common code for dosinst.c and uninstall.c.
#അന്തര്ഭാവ "dosinst.h"

/*
 * Return TRUE if the user types a 'y' or 'Y', FALSE otherwise.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
confirm(ശൂന്യം)
{
    അക്ഷരം	answer[10];

    fflush(stdout);
    തിരികെ (scanf(" %c", answer) == 1 && toupper((unsigned അക്ഷരം)answer[0]) == 'Y');
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
reg_delete_താക്കോൽ(HKEY h_വേര്_താക്കോൽ, സ്ഥിരം അക്ഷരം *താക്കോൽ, DWORD flag)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ did_load = FALSE;
    സ്ഥിതം HANDLE advapi_lib = ശൂന്യ;
    സ്ഥിതം LONG (WINAPI *delete_താക്കോൽ_ex)(HKEY, LPCTSTR, REGSAM, DWORD) = ശൂന്യ;

    എങ്കിൽ (!did_load)
    {
	// The RegDeleteKeyEx() function is only available on new systems.  It
	// is required for 64-bit registry access.  For other systems fall
	// back to RegDeleteKey().
	did_load = TRUE;
	advapi_lib = LoadLibrary("ADVAPI32.DLL");
	എങ്കിൽ (advapi_lib != ശൂന്യ)
	    delete_താക്കോൽ_ex = (LONG (WINAPI *)(HKEY, LPCTSTR, REGSAM, DWORD))GetProcAddress(advapi_lib, "RegDeleteKeyExA");
    }
    എങ്കിൽ (delete_താക്കോൽ_ex != ശൂന്യ)
    {
	തിരികെ (*delete_താക്കോൽ_ex)(h_വേര്_താക്കോൽ, താക്കോൽ, flag, 0);
    }
    തിരികെ Reg_Delete_താക്കോൽ(h_വേര്_താക്കോൽ, താക്കോൽ);
}

/*
 * Check if the popup menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
popup_gvim_path(അക്ഷരം *buf, DWORD bufsize)
{
    HKEY	താക്കോൽ_handle;
    DWORD	മൂല്യം_type;
    പൂർണ്ണസംഖ്യ		r;

    // Open the key where the path to gvim.exe is stored.
    എങ്കിൽ (Reg_Open_താക്കോൽ_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    താക്കോൽ_WOW64_64_താക്കോൽ | താക്കോൽ_READ, &താക്കോൽ_handle) != ERROR_SUCCESS)
	എങ്കിൽ (Reg_Open_താക്കോൽ_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    താക്കോൽ_WOW64_32_താക്കോൽ | താക്കോൽ_READ, &താക്കോൽ_handle) != ERROR_SUCCESS)
	    തിരികെ 0;

    // get the DisplayName out of it to show the user
    r = Reg_Query_മൂല്യം_Ex(താക്കോൽ_handle, "path", 0,
					  &മൂല്യം_type, (LPBYTE)buf, &bufsize);
    Reg_Close_താക്കോൽ(താക്കോൽ_handle);

    തിരികെ (r == ERROR_SUCCESS);
}

/*
 * Check if the "Open With..." menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
openwith_gvim_path(അക്ഷരം *buf, DWORD bufsize)
{
    HKEY	താക്കോൽ_handle;
    DWORD	മൂല്യം_type;
    പൂർണ്ണസംഖ്യ		r;

    // Open the key where the path to gvim.exe is stored.
    എങ്കിൽ (Reg_Open_താക്കോൽ_Ex(HKEY_CLASSES_വേര്,
		"Applications\\gvim.exe\\shell\\edit\\command", 0,
		    താക്കോൽ_WOW64_64_താക്കോൽ | താക്കോൽ_READ, &താക്കോൽ_handle) != ERROR_SUCCESS)
	തിരികെ 0;

    // get the DisplayName out of it to show the user
    r = Reg_Query_മൂല്യം_Ex(താക്കോൽ_handle, "", 0, &മൂല്യം_type, (LPBYTE)buf, &bufsize);
    Reg_Close_താക്കോൽ(താക്കോൽ_handle);

    തിരികെ (r == ERROR_SUCCESS);
}

    സ്ഥിതം ശൂന്യം
remove_popup(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		fail = 0;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		loop = is_64bit_os() ? 2 : 1;
    പൂർണ്ണസംഖ്യ		maxfail = loop * 6;
    DWORD	flag;
    HKEY	kh;

    വേണ്ടി (i = 0; i < loop; i++)
    {
	എങ്കിൽ (i == 0)
	    flag = താക്കോൽ_WOW64_32_താക്കോൽ;
	അല്ലെങ്കിൽ
	    flag = താക്കോൽ_WOW64_64_താക്കോൽ;

	എങ്കിൽ (reg_delete_താക്കോൽ(HKEY_CLASSES_വേര്, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}\\InProcServer32", flag) != ERROR_SUCCESS)
	    ++fail;
	എങ്കിൽ (reg_delete_താക്കോൽ(HKEY_CLASSES_വേര്, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}", flag) != ERROR_SUCCESS)
	    ++fail;
	എങ്കിൽ (reg_delete_താക്കോൽ(HKEY_CLASSES_വേര്, "*\\shellex\\ContextMenuHandlers\\gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	എങ്കിൽ (Reg_Open_താക്കോൽ_Ex(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved", 0,
		    flag | താക്കോൽ_ALL_ACCESS, &kh) != ERROR_SUCCESS)
	    ++fail;
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (Reg_Delete_മൂല്യം(kh, "{51EEE242-AD87-11d3-9C1E-0090278BBD99}") != ERROR_SUCCESS)
		++fail;
	    Reg_Close_താക്കോൽ(kh);
	}
	എങ്കിൽ (reg_delete_താക്കോൽ(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	എങ്കിൽ (reg_delete_താക്കോൽ(HKEY_LOCAL_MACHINE, "Software\\Vim", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    എങ്കിൽ (fail == maxfail)
	printf("No Vim popup registry entries could be removed\n");
    അല്ലെങ്കിൽ എങ്കിൽ (fail > 0)
	printf("Some Vim popup registry entries could not be removed\n");
    അല്ലെങ്കിൽ
	printf("The Vim popup registry entries have been removed\n");
}

    സ്ഥിതം ശൂന്യം
remove_openwith(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		fail = 0;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		loop = is_64bit_os() ? 2 : 1;
    പൂർണ്ണസംഖ്യ		maxfail = loop * 7;
    DWORD	flag;

    വേണ്ടി (i = 0; i < loop; i++)
    {
	എങ്കിൽ (i == 0)
	    flag = താക്കോൽ_WOW64_32_താക്കോൽ;
	അല്ലെങ്കിൽ
	    flag = താക്കോൽ_WOW64_64_താക്കോൽ;

	എങ്കിൽ (reg_delete_താക്കോൽ(HKEY_CLASSES_വേര്, "Applications\\gvim.exe\\shell\\edit\\command", flag) != ERROR_SUCCESS)
	    ++fail;
	എങ്കിൽ (reg_delete_താക്കോൽ(HKEY_CLASSES_വേര്, "Applications\\gvim.exe\\shell\\edit", flag) != ERROR_SUCCESS)
	    ++fail;
	എങ്കിൽ (reg_delete_താക്കോൽ(HKEY_CLASSES_വേര്, "Applications\\gvim.exe\\shell", flag) != ERROR_SUCCESS)
	    ++fail;
	എങ്കിൽ (reg_delete_താക്കോൽ(HKEY_CLASSES_വേര്, "Applications\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	എങ്കിൽ (reg_delete_താക്കോൽ(HKEY_CLASSES_വേര്, ".htm\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	എങ്കിൽ (reg_delete_താക്കോൽ(HKEY_CLASSES_വേര്, ".vim\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	എങ്കിൽ (reg_delete_താക്കോൽ(HKEY_CLASSES_വേര്, "*\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    എങ്കിൽ (fail == maxfail)
	printf("No Vim open-with registry entries could be removed\n");
    അല്ലെങ്കിൽ എങ്കിൽ (fail > 0)
	printf("Some Vim open-with registry entries could not be removed\n");
    അല്ലെങ്കിൽ
	printf("The Vim open-with registry entries have been removed\n");
}

/*
 * Check if a batch file is really for the current version.  Don't delete a
 * batch file that was written for another (possibly newer) version.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
batfile_thisversion(അക്ഷരം *path)
{
    FILE	*fd;
    അക്ഷരം	വരി[BUFSIZE];
    പൂർണ്ണസംഖ്യ		താക്കോൽ_len = strlen(VIMBAT_UNINSTKEY);
    പൂർണ്ണസംഖ്യ		found = FALSE;

    fd = fopen(path, "r");
    എങ്കിൽ (fd == ശൂന്യ)
	തിരികെ FALSE;

    ആകുമ്പോൾ (fgets(വരി, വലുപ്പം(വരി), fd) != ശൂന്യ)
    {
	എങ്കിൽ (strncmp(വരി, VIMBAT_UNINSTKEY, താക്കോൽ_len) == 0)
	{
	    found = TRUE;
	    നിർത്തുക;
	}
    }
    fclose(fd);
    തിരികെ found;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
remove_batfiles(പൂർണ്ണസംഖ്യ doit)
{
    അക്ഷരം *batfile_path;
    പൂർണ്ണസംഖ്യ	 i;
    പൂർണ്ണസംഖ്യ	 found = 0;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    വേണ്ടി (i = 1; i < TARGET_എണ്ണം; ++i)
    {
	batfile_path = searchpath_save(targets[i].batname);
	എങ്കിൽ (batfile_path != ശൂന്യ && batfile_thisversion(batfile_path))
	{
	    ++found;
	    എങ്കിൽ (doit)
	    {
		printf("removing %s\n", batfile_path);
		remove(batfile_path);
	    }
	    അല്ലെങ്കിൽ
		printf(" - the batch file %s\n", batfile_path);
	    സ്വതന്ത്രം(batfile_path);
	}
    }

    mch_chdir(installdir);
    തിരികെ found;
}

    സ്ഥിതം ശൂന്യം
remove_if_exists(അക്ഷരം *path, അക്ഷരം *filename)
{
    അക്ഷരം buf[BUFSIZE];
    FILE *fd;

    sprintf(buf, "%s\\%s", path, filename);

    fd = fopen(buf, "r");
    എങ്കിൽ (fd == ശൂന്യ)
	തിരികെ;

    fclose(fd);
    printf("removing %s\n", buf);
    remove(buf);
}

    സ്ഥിതം ശൂന്യം
remove_icons(ശൂന്യം)
{
    അക്ഷരം	path[BUFSIZE];
    പൂർണ്ണസംഖ്യ		i;

    എങ്കിൽ (get_shell_folder_path(path, "desktop"))
	വേണ്ടി (i = 0; i < ICON_എണ്ണം; ++i)
	    remove_if_exists(path, icon_link_names[i]);
}

    സ്ഥിതം ശൂന്യം
remove_start_menu(ശൂന്യം)
{
    അക്ഷരം	path[BUFSIZE];
    പൂർണ്ണസംഖ്യ		i;
    ഘടന stat st;

    എങ്കിൽ (get_shell_folder_path(path, VIM_STARTMENU) == FAIL)
	തിരികെ;

    വേണ്ടി (i = 1; i < TARGET_എണ്ണം; ++i)
	remove_if_exists(path, targets[i].lnkname);
    remove_if_exists(path, "uninstall.lnk");
    remove_if_exists(path, "Help.lnk");
    // Win95 uses .pif, WinNT uses .lnk
    remove_if_exists(path, "Vim tutor.pif");
    remove_if_exists(path, "Vim tutor.lnk");
    remove_if_exists(path, "Vim online.url");
    എങ്കിൽ (stat(path, &st) == 0)
    {
	printf("removing %s\n", path);
	rmdir(path);
    }
}

    സ്ഥിതം ശൂന്യം
delete_uninstall_താക്കോൽ(ശൂന്യം)
{
    reg_delete_താക്കോൽ(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Vim " VIM_VERSION_SHORT, താക്കോൽ_WOW64_64_താക്കോൽ);
}

    പൂർണ്ണസംഖ്യ
പ്രധാന(പൂർണ്ണസംഖ്യ argc, അക്ഷരം *argv[])
{
    പൂർണ്ണസംഖ്യ		found = 0;
    FILE	*fd;
    പൂർണ്ണസംഖ്യ		i;
    ഘടന stat st;
    അക്ഷരം	icon[BUFSIZE];
    അക്ഷരം	path[MAX_PATH];
    അക്ഷരം	popup_path[MAX_PATH];

    // The nsis uninstaller calls us with a "-nsis" argument.
    എങ്കിൽ (argc == 2 && stricmp(argv[1], "-nsis") == 0)
	interactive = FALSE;
    അല്ലെങ്കിൽ
	interactive = TRUE;

    // Initialize this program.
    do_inits(argv);

    printf("This program will remove the following items:\n");

    എങ്കിൽ (popup_gvim_path(popup_path, വലുപ്പം(popup_path)))
    {
	printf(" - the \"Edit with Vim\" entry in the popup menu\n");
	printf("   which uses \"%s\"\n", popup_path);
	എങ്കിൽ (interactive)
	    printf("\nRemove it (y/n)? ");
	എങ്കിൽ (!interactive || confirm())
	{
	    remove_popup();
	    // Assume the "Open With" entry can be removed as well, don't
	    // bother the user with asking him again.
	    remove_openwith();
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (openwith_gvim_path(popup_path, വലുപ്പം(popup_path)))
    {
	printf(" - the Vim \"Open With...\" entry in the popup menu\n");
	printf("   which uses \"%s\"\n", popup_path);
	printf("\nRemove it (y/n)? ");
	എങ്കിൽ (confirm())
	    remove_openwith();
    }

    എങ്കിൽ (get_shell_folder_path(path, "desktop"))
    {
	printf("\n");
	വേണ്ടി (i = 0; i < ICON_എണ്ണം; ++i)
	{
	    sprintf(icon, "%s\\%s", path, icon_link_names[i]);
	    എങ്കിൽ (stat(icon, &st) == 0)
	    {
		printf(" - the \"%s\" icon on the desktop\n", icon_names[i]);
		++found;
	    }
	}
	എങ്കിൽ (found > 0)
	{
	    എങ്കിൽ (interactive)
		printf("\nRemove %s (y/n)? ", found > 1 ? "them" : "it");
	    എങ്കിൽ (!interactive || confirm())
		remove_icons();
	}
    }

    എങ്കിൽ (get_shell_folder_path(path, VIM_STARTMENU)
	    && stat(path, &st) == 0)
    {
	printf("\n - the \"%s\" entry in the Start Menu\n", VIM_STARTMENU);
	എങ്കിൽ (interactive)
	    printf("\nRemove it (y/n)? ");
	എങ്കിൽ (!interactive || confirm())
	    remove_start_menu();
    }

    printf("\n");
    found = remove_batfiles(0);
    എങ്കിൽ (found > 0)
    {
	എങ്കിൽ (interactive)
	    printf("\nRemove %s (y/n)? ", found > 1 ? "them" : "it");
	എങ്കിൽ (!interactive || confirm())
	    remove_batfiles(1);
    }

    fd = fopen("gvim.exe", "r");
    എങ്കിൽ (fd != ശൂന്യ)
    {
	fclose(fd);
	printf("gvim.exe detected.  Attempting to unregister gvim with OLE\n");
	system("gvim.exe -silent -unregister");
    }

    delete_uninstall_താക്കോൽ();

    എങ്കിൽ (interactive)
    {
	printf("\nYou may now want to delete the Vim executables and runtime files.\n");
	printf("(They are still where you unpacked them.)\n");
    }

    എങ്കിൽ (interactive)
    {
	rewind(stdin);
	printf("\nPress Enter to exit...");
	(ശൂന്യം)getchar();
    }
    അല്ലെങ്കിൽ
	sleep(3);

    തിരികെ 0;
}
