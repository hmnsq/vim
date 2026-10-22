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
#அந்தர்பாவ "dosinst.h"

/*
 * Return TRUE if the user types a 'y' or 'Y', FALSE otherwise.
 */
    நிலையான முழு
confirm(வெறுமை)
{
    எழுத்து	answer[10];

    fflush(stdout);
    திரும்பு (scanf(" %c", answer) == 1 && toupper((unsigned எழுத்து)answer[0]) == 'Y');
}

    நிலையான முழு
reg_delete_சாவி(HKEY h_வேர்_சாவி, மாறிலி எழுத்து *சாவி, DWORD flag)
{
    நிலையான முழு did_load = FALSE;
    நிலையான HANDLE advapi_lib = வெற்று;
    நிலையான LONG (WINAPI *delete_சாவி_ex)(HKEY, LPCTSTR, REGSAM, DWORD) = வெற்று;

    எனில் (!did_load)
    {
	// The RegDeleteKeyEx() function is only available on new systems.  It
	// is required for 64-bit registry access.  For other systems fall
	// back to RegDeleteKey().
	did_load = TRUE;
	advapi_lib = LoadLibrary("ADVAPI32.DLL");
	எனில் (advapi_lib != வெற்று)
	    delete_சாவி_ex = (LONG (WINAPI *)(HKEY, LPCTSTR, REGSAM, DWORD))GetProcAddress(advapi_lib, "RegDeleteKeyExA");
    }
    எனில் (delete_சாவி_ex != வெற்று)
    {
	திரும்பு (*delete_சாவி_ex)(h_வேர்_சாவி, சாவி, flag, 0);
    }
    திரும்பு Reg_Delete_சாவி(h_வேர்_சாவி, சாவி);
}

/*
 * Check if the popup menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    நிலையான முழு
popup_gvim_path(எழுத்து *buf, DWORD bufsize)
{
    HKEY	சாவி_handle;
    DWORD	மதிப்பு_type;
    முழு		r;

    // Open the key where the path to gvim.exe is stored.
    எனில் (Reg_Open_சாவி_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    சாவி_WOW64_64_சாவி | சாவி_READ, &சாவி_handle) != ERROR_SUCCESS)
	எனில் (Reg_Open_சாவி_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    சாவி_WOW64_32_சாவி | சாவி_READ, &சாவி_handle) != ERROR_SUCCESS)
	    திரும்பு 0;

    // get the DisplayName out of it to show the user
    r = Reg_Query_மதிப்பு_Ex(சாவி_handle, "path", 0,
					  &மதிப்பு_type, (LPBYTE)buf, &bufsize);
    Reg_Close_சாவி(சாவி_handle);

    திரும்பு (r == ERROR_SUCCESS);
}

/*
 * Check if the "Open With..." menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    நிலையான முழு
openwith_gvim_path(எழுத்து *buf, DWORD bufsize)
{
    HKEY	சாவி_handle;
    DWORD	மதிப்பு_type;
    முழு		r;

    // Open the key where the path to gvim.exe is stored.
    எனில் (Reg_Open_சாவி_Ex(HKEY_CLASSES_வேர்,
		"Applications\\gvim.exe\\shell\\edit\\command", 0,
		    சாவி_WOW64_64_சாவி | சாவி_READ, &சாவி_handle) != ERROR_SUCCESS)
	திரும்பு 0;

    // get the DisplayName out of it to show the user
    r = Reg_Query_மதிப்பு_Ex(சாவி_handle, "", 0, &மதிப்பு_type, (LPBYTE)buf, &bufsize);
    Reg_Close_சாவி(சாவி_handle);

    திரும்பு (r == ERROR_SUCCESS);
}

    நிலையான வெறுமை
remove_popup(வெறுமை)
{
    முழு		fail = 0;
    முழு		i;
    முழு		loop = is_64bit_os() ? 2 : 1;
    முழு		maxfail = loop * 6;
    DWORD	flag;
    HKEY	kh;

    க்கு (i = 0; i < loop; i++)
    {
	எனில் (i == 0)
	    flag = சாவி_WOW64_32_சாவி;
	இல்லையெனில்
	    flag = சாவி_WOW64_64_சாவி;

	எனில் (reg_delete_சாவி(HKEY_CLASSES_வேர், "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}\\InProcServer32", flag) != ERROR_SUCCESS)
	    ++fail;
	எனில் (reg_delete_சாவி(HKEY_CLASSES_வேர், "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}", flag) != ERROR_SUCCESS)
	    ++fail;
	எனில் (reg_delete_சாவி(HKEY_CLASSES_வேர், "*\\shellex\\ContextMenuHandlers\\gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	எனில் (Reg_Open_சாவி_Ex(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved", 0,
		    flag | சாவி_ALL_ACCESS, &kh) != ERROR_SUCCESS)
	    ++fail;
	இல்லையெனில்
	{
	    எனில் (Reg_Delete_மதிப்பு(kh, "{51EEE242-AD87-11d3-9C1E-0090278BBD99}") != ERROR_SUCCESS)
		++fail;
	    Reg_Close_சாவி(kh);
	}
	எனில் (reg_delete_சாவி(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	எனில் (reg_delete_சாவி(HKEY_LOCAL_MACHINE, "Software\\Vim", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    எனில் (fail == maxfail)
	printf("No Vim popup registry entries could be removed\n");
    இல்லையெனில் எனில் (fail > 0)
	printf("Some Vim popup registry entries could not be removed\n");
    இல்லையெனில்
	printf("The Vim popup registry entries have been removed\n");
}

    நிலையான வெறுமை
remove_openwith(வெறுமை)
{
    முழு		fail = 0;
    முழு		i;
    முழு		loop = is_64bit_os() ? 2 : 1;
    முழு		maxfail = loop * 7;
    DWORD	flag;

    க்கு (i = 0; i < loop; i++)
    {
	எனில் (i == 0)
	    flag = சாவி_WOW64_32_சாவி;
	இல்லையெனில்
	    flag = சாவி_WOW64_64_சாவி;

	எனில் (reg_delete_சாவி(HKEY_CLASSES_வேர், "Applications\\gvim.exe\\shell\\edit\\command", flag) != ERROR_SUCCESS)
	    ++fail;
	எனில் (reg_delete_சாவி(HKEY_CLASSES_வேர், "Applications\\gvim.exe\\shell\\edit", flag) != ERROR_SUCCESS)
	    ++fail;
	எனில் (reg_delete_சாவி(HKEY_CLASSES_வேர், "Applications\\gvim.exe\\shell", flag) != ERROR_SUCCESS)
	    ++fail;
	எனில் (reg_delete_சாவி(HKEY_CLASSES_வேர், "Applications\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	எனில் (reg_delete_சாவி(HKEY_CLASSES_வேர், ".htm\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	எனில் (reg_delete_சாவி(HKEY_CLASSES_வேர், ".vim\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	எனில் (reg_delete_சாவி(HKEY_CLASSES_வேர், "*\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    எனில் (fail == maxfail)
	printf("No Vim open-with registry entries could be removed\n");
    இல்லையெனில் எனில் (fail > 0)
	printf("Some Vim open-with registry entries could not be removed\n");
    இல்லையெனில்
	printf("The Vim open-with registry entries have been removed\n");
}

/*
 * Check if a batch file is really for the current version.  Don't delete a
 * batch file that was written for another (possibly newer) version.
 */
    நிலையான முழு
batfile_thisversion(எழுத்து *path)
{
    FILE	*fd;
    எழுத்து	வரி[BUFSIZE];
    முழு		சாவி_len = strlen(VIMBAT_UNINSTKEY);
    முழு		found = FALSE;

    fd = fopen(path, "r");
    எனில் (fd == வெற்று)
	திரும்பு FALSE;

    போது (fgets(வரி, அளவு(வரி), fd) != வெற்று)
    {
	எனில் (strncmp(வரி, VIMBAT_UNINSTKEY, சாவி_len) == 0)
	{
	    found = TRUE;
	    நிறுத்து;
	}
    }
    fclose(fd);
    திரும்பு found;
}

    நிலையான முழு
remove_batfiles(முழு doit)
{
    எழுத்து *batfile_path;
    முழு	 i;
    முழு	 found = 0;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    க்கு (i = 1; i < TARGET_மொத்தம்; ++i)
    {
	batfile_path = searchpath_save(targets[i].batname);
	எனில் (batfile_path != வெற்று && batfile_thisversion(batfile_path))
	{
	    ++found;
	    எனில் (doit)
	    {
		printf("removing %s\n", batfile_path);
		remove(batfile_path);
	    }
	    இல்லையெனில்
		printf(" - the batch file %s\n", batfile_path);
	    விடுவி(batfile_path);
	}
    }

    mch_chdir(installdir);
    திரும்பு found;
}

    நிலையான வெறுமை
remove_if_exists(எழுத்து *path, எழுத்து *filename)
{
    எழுத்து buf[BUFSIZE];
    FILE *fd;

    sprintf(buf, "%s\\%s", path, filename);

    fd = fopen(buf, "r");
    எனில் (fd == வெற்று)
	திரும்பு;

    fclose(fd);
    printf("removing %s\n", buf);
    remove(buf);
}

    நிலையான வெறுமை
remove_icons(வெறுமை)
{
    எழுத்து	path[BUFSIZE];
    முழு		i;

    எனில் (get_shell_folder_path(path, "desktop"))
	க்கு (i = 0; i < ICON_மொத்தம்; ++i)
	    remove_if_exists(path, icon_link_names[i]);
}

    நிலையான வெறுமை
remove_start_menu(வெறுமை)
{
    எழுத்து	path[BUFSIZE];
    முழு		i;
    கட்டமைப்பு stat st;

    எனில் (get_shell_folder_path(path, VIM_STARTMENU) == FAIL)
	திரும்பு;

    க்கு (i = 1; i < TARGET_மொத்தம்; ++i)
	remove_if_exists(path, targets[i].lnkname);
    remove_if_exists(path, "uninstall.lnk");
    remove_if_exists(path, "Help.lnk");
    // Win95 uses .pif, WinNT uses .lnk
    remove_if_exists(path, "Vim tutor.pif");
    remove_if_exists(path, "Vim tutor.lnk");
    remove_if_exists(path, "Vim online.url");
    எனில் (stat(path, &st) == 0)
    {
	printf("removing %s\n", path);
	rmdir(path);
    }
}

    நிலையான வெறுமை
delete_uninstall_சாவி(வெறுமை)
{
    reg_delete_சாவி(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Vim " VIM_VERSION_SHORT, சாவி_WOW64_64_சாவி);
}

    முழு
முதன்மை(முழு argc, எழுத்து *argv[])
{
    முழு		found = 0;
    FILE	*fd;
    முழு		i;
    கட்டமைப்பு stat st;
    எழுத்து	icon[BUFSIZE];
    எழுத்து	path[MAX_PATH];
    எழுத்து	popup_path[MAX_PATH];

    // The nsis uninstaller calls us with a "-nsis" argument.
    எனில் (argc == 2 && stricmp(argv[1], "-nsis") == 0)
	interactive = FALSE;
    இல்லையெனில்
	interactive = TRUE;

    // Initialize this program.
    do_inits(argv);

    printf("This program will remove the following items:\n");

    எனில் (popup_gvim_path(popup_path, அளவு(popup_path)))
    {
	printf(" - the \"Edit with Vim\" entry in the popup menu\n");
	printf("   which uses \"%s\"\n", popup_path);
	எனில் (interactive)
	    printf("\nRemove it (y/n)? ");
	எனில் (!interactive || confirm())
	{
	    remove_popup();
	    // Assume the "Open With" entry can be removed as well, don't
	    // bother the user with asking him again.
	    remove_openwith();
	}
    }
    இல்லையெனில் எனில் (openwith_gvim_path(popup_path, அளவு(popup_path)))
    {
	printf(" - the Vim \"Open With...\" entry in the popup menu\n");
	printf("   which uses \"%s\"\n", popup_path);
	printf("\nRemove it (y/n)? ");
	எனில் (confirm())
	    remove_openwith();
    }

    எனில் (get_shell_folder_path(path, "desktop"))
    {
	printf("\n");
	க்கு (i = 0; i < ICON_மொத்தம்; ++i)
	{
	    sprintf(icon, "%s\\%s", path, icon_link_names[i]);
	    எனில் (stat(icon, &st) == 0)
	    {
		printf(" - the \"%s\" icon on the desktop\n", icon_names[i]);
		++found;
	    }
	}
	எனில் (found > 0)
	{
	    எனில் (interactive)
		printf("\nRemove %s (y/n)? ", found > 1 ? "them" : "it");
	    எனில் (!interactive || confirm())
		remove_icons();
	}
    }

    எனில் (get_shell_folder_path(path, VIM_STARTMENU)
	    && stat(path, &st) == 0)
    {
	printf("\n - the \"%s\" entry in the Start Menu\n", VIM_STARTMENU);
	எனில் (interactive)
	    printf("\nRemove it (y/n)? ");
	எனில் (!interactive || confirm())
	    remove_start_menu();
    }

    printf("\n");
    found = remove_batfiles(0);
    எனில் (found > 0)
    {
	எனில் (interactive)
	    printf("\nRemove %s (y/n)? ", found > 1 ? "them" : "it");
	எனில் (!interactive || confirm())
	    remove_batfiles(1);
    }

    fd = fopen("gvim.exe", "r");
    எனில் (fd != வெற்று)
    {
	fclose(fd);
	printf("gvim.exe detected.  Attempting to unregister gvim with OLE\n");
	system("gvim.exe -silent -unregister");
    }

    delete_uninstall_சாவி();

    எனில் (interactive)
    {
	printf("\nYou may now want to delete the Vim executables and runtime files.\n");
	printf("(They are still where you unpacked them.)\n");
    }

    எனில் (interactive)
    {
	rewind(stdin);
	printf("\nPress Enter to exit...");
	(வெறுமை)getchar();
    }
    இல்லையெனில்
	sleep(3);

    திரும்பு 0;
}
