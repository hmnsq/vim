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
#ಅನ್ತರ್ಭಾವ "dosinst.h"

/*
 * Return TRUE if the user types a 'y' or 'Y', FALSE otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
confirm(ಶೂನ್ಯ)
{
    ವರ್ಣ	answer[10];

    fflush(stdout);
    ಪ್ರತ್ಯಾಗಮ (scanf(" %c", answer) == 1 && toupper((unsigned ವರ್ಣ)answer[0]) == 'Y');
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
reg_delete_ಕೀಲಿ(HKEY h_ಬೇರು_ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, DWORD flag)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ did_load = FALSE;
    ಸ್ಥಿರ HANDLE advapi_lib = ರಿಕ್ತ;
    ಸ್ಥಿರ LONG (WINAPI *delete_ಕೀಲಿ_ex)(HKEY, LPCTSTR, REGSAM, DWORD) = ರಿಕ್ತ;

    ಯದಿ (!did_load)
    {
	// The RegDeleteKeyEx() function is only available on new systems.  It
	// is required for 64-bit registry access.  For other systems fall
	// back to RegDeleteKey().
	did_load = TRUE;
	advapi_lib = LoadLibrary("ADVAPI32.DLL");
	ಯದಿ (advapi_lib != ರಿಕ್ತ)
	    delete_ಕೀಲಿ_ex = (LONG (WINAPI *)(HKEY, LPCTSTR, REGSAM, DWORD))GetProcAddress(advapi_lib, "RegDeleteKeyExA");
    }
    ಯದಿ (delete_ಕೀಲಿ_ex != ರಿಕ್ತ)
    {
	ಪ್ರತ್ಯಾಗಮ (*delete_ಕೀಲಿ_ex)(h_ಬೇರು_ಕೀಲಿ, ಕೀಲಿ, flag, 0);
    }
    ಪ್ರತ್ಯಾಗಮ Reg_Delete_ಕೀಲಿ(h_ಬೇರು_ಕೀಲಿ, ಕೀಲಿ);
}

/*
 * Check if the popup menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
popup_gvim_path(ವರ್ಣ *buf, DWORD bufsize)
{
    HKEY	ಕೀಲಿ_handle;
    DWORD	ಮೌಲ್ಯ_type;
    ಪೂರ್ಣಾಙ್ಕ		r;

    // Open the key where the path to gvim.exe is stored.
    ಯದಿ (Reg_Open_ಕೀಲಿ_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    ಕೀಲಿ_WOW64_64_ಕೀಲಿ | ಕೀಲಿ_READ, &ಕೀಲಿ_handle) != ERROR_SUCCESS)
	ಯದಿ (Reg_Open_ಕೀಲಿ_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    ಕೀಲಿ_WOW64_32_ಕೀಲಿ | ಕೀಲಿ_READ, &ಕೀಲಿ_handle) != ERROR_SUCCESS)
	    ಪ್ರತ್ಯಾಗಮ 0;

    // get the DisplayName out of it to show the user
    r = Reg_Query_ಮೌಲ್ಯ_Ex(ಕೀಲಿ_handle, "path", 0,
					  &ಮೌಲ್ಯ_type, (LPBYTE)buf, &bufsize);
    Reg_Close_ಕೀಲಿ(ಕೀಲಿ_handle);

    ಪ್ರತ್ಯಾಗಮ (r == ERROR_SUCCESS);
}

/*
 * Check if the "Open With..." menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
openwith_gvim_path(ವರ್ಣ *buf, DWORD bufsize)
{
    HKEY	ಕೀಲಿ_handle;
    DWORD	ಮೌಲ್ಯ_type;
    ಪೂರ್ಣಾಙ್ಕ		r;

    // Open the key where the path to gvim.exe is stored.
    ಯದಿ (Reg_Open_ಕೀಲಿ_Ex(HKEY_CLASSES_ಬೇರು,
		"Applications\\gvim.exe\\shell\\edit\\command", 0,
		    ಕೀಲಿ_WOW64_64_ಕೀಲಿ | ಕೀಲಿ_READ, &ಕೀಲಿ_handle) != ERROR_SUCCESS)
	ಪ್ರತ್ಯಾಗಮ 0;

    // get the DisplayName out of it to show the user
    r = Reg_Query_ಮೌಲ್ಯ_Ex(ಕೀಲಿ_handle, "", 0, &ಮೌಲ್ಯ_type, (LPBYTE)buf, &bufsize);
    Reg_Close_ಕೀಲಿ(ಕೀಲಿ_handle);

    ಪ್ರತ್ಯಾಗಮ (r == ERROR_SUCCESS);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
remove_popup(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		fail = 0;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		loop = is_64bit_os() ? 2 : 1;
    ಪೂರ್ಣಾಙ್ಕ		maxfail = loop * 6;
    DWORD	flag;
    HKEY	kh;

    ಪ್ರತಿ (i = 0; i < loop; i++)
    {
	ಯದಿ (i == 0)
	    flag = ಕೀಲಿ_WOW64_32_ಕೀಲಿ;
	ಅನ್ಯಥಾ
	    flag = ಕೀಲಿ_WOW64_64_ಕೀಲಿ;

	ಯದಿ (reg_delete_ಕೀಲಿ(HKEY_CLASSES_ಬೇರು, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}\\InProcServer32", flag) != ERROR_SUCCESS)
	    ++fail;
	ಯದಿ (reg_delete_ಕೀಲಿ(HKEY_CLASSES_ಬೇರು, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}", flag) != ERROR_SUCCESS)
	    ++fail;
	ಯದಿ (reg_delete_ಕೀಲಿ(HKEY_CLASSES_ಬೇರು, "*\\shellex\\ContextMenuHandlers\\gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	ಯದಿ (Reg_Open_ಕೀಲಿ_Ex(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved", 0,
		    flag | ಕೀಲಿ_ALL_ACCESS, &kh) != ERROR_SUCCESS)
	    ++fail;
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (Reg_Delete_ಮೌಲ್ಯ(kh, "{51EEE242-AD87-11d3-9C1E-0090278BBD99}") != ERROR_SUCCESS)
		++fail;
	    Reg_Close_ಕೀಲಿ(kh);
	}
	ಯದಿ (reg_delete_ಕೀಲಿ(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	ಯದಿ (reg_delete_ಕೀಲಿ(HKEY_LOCAL_MACHINE, "Software\\Vim", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    ಯದಿ (fail == maxfail)
	printf("No Vim popup registry entries could be removed\n");
    ಅನ್ಯಥಾ ಯದಿ (fail > 0)
	printf("Some Vim popup registry entries could not be removed\n");
    ಅನ್ಯಥಾ
	printf("The Vim popup registry entries have been removed\n");
}

    ಸ್ಥಿರ ಶೂನ್ಯ
remove_openwith(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		fail = 0;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		loop = is_64bit_os() ? 2 : 1;
    ಪೂರ್ಣಾಙ್ಕ		maxfail = loop * 7;
    DWORD	flag;

    ಪ್ರತಿ (i = 0; i < loop; i++)
    {
	ಯದಿ (i == 0)
	    flag = ಕೀಲಿ_WOW64_32_ಕೀಲಿ;
	ಅನ್ಯಥಾ
	    flag = ಕೀಲಿ_WOW64_64_ಕೀಲಿ;

	ಯದಿ (reg_delete_ಕೀಲಿ(HKEY_CLASSES_ಬೇರು, "Applications\\gvim.exe\\shell\\edit\\command", flag) != ERROR_SUCCESS)
	    ++fail;
	ಯದಿ (reg_delete_ಕೀಲಿ(HKEY_CLASSES_ಬೇರು, "Applications\\gvim.exe\\shell\\edit", flag) != ERROR_SUCCESS)
	    ++fail;
	ಯದಿ (reg_delete_ಕೀಲಿ(HKEY_CLASSES_ಬೇರು, "Applications\\gvim.exe\\shell", flag) != ERROR_SUCCESS)
	    ++fail;
	ಯದಿ (reg_delete_ಕೀಲಿ(HKEY_CLASSES_ಬೇರು, "Applications\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	ಯದಿ (reg_delete_ಕೀಲಿ(HKEY_CLASSES_ಬೇರು, ".htm\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	ಯದಿ (reg_delete_ಕೀಲಿ(HKEY_CLASSES_ಬೇರು, ".vim\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	ಯದಿ (reg_delete_ಕೀಲಿ(HKEY_CLASSES_ಬೇರು, "*\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    ಯದಿ (fail == maxfail)
	printf("No Vim open-with registry entries could be removed\n");
    ಅನ್ಯಥಾ ಯದಿ (fail > 0)
	printf("Some Vim open-with registry entries could not be removed\n");
    ಅನ್ಯಥಾ
	printf("The Vim open-with registry entries have been removed\n");
}

/*
 * Check if a batch file is really for the current version.  Don't delete a
 * batch file that was written for another (possibly newer) version.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
batfile_thisversion(ವರ್ಣ *path)
{
    FILE	*fd;
    ವರ್ಣ	ಸಾಲು[BUFSIZE];
    ಪೂರ್ಣಾಙ್ಕ		ಕೀಲಿ_len = strlen(VIMBAT_UNINSTKEY);
    ಪೂರ್ಣಾಙ್ಕ		found = FALSE;

    fd = fopen(path, "r");
    ಯದಿ (fd == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯಾವತ್ (fgets(ಸಾಲು, ಪರಿಮಾಣ(ಸಾಲು), fd) != ರಿಕ್ತ)
    {
	ಯದಿ (strncmp(ಸಾಲು, VIMBAT_UNINSTKEY, ಕೀಲಿ_len) == 0)
	{
	    found = TRUE;
	    ಭಙ್ಗ;
	}
    }
    fclose(fd);
    ಪ್ರತ್ಯಾಗಮ found;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
remove_batfiles(ಪೂರ್ಣಾಙ್ಕ doit)
{
    ವರ್ಣ *batfile_path;
    ಪೂರ್ಣಾಙ್ಕ	 i;
    ಪೂರ್ಣಾಙ್ಕ	 found = 0;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    ಪ್ರತಿ (i = 1; i < TARGET_ಎಣಿಕೆ; ++i)
    {
	batfile_path = searchpath_save(targets[i].batname);
	ಯದಿ (batfile_path != ರಿಕ್ತ && batfile_thisversion(batfile_path))
	{
	    ++found;
	    ಯದಿ (doit)
	    {
		printf("removing %s\n", batfile_path);
		remove(batfile_path);
	    }
	    ಅನ್ಯಥಾ
		printf(" - the batch file %s\n", batfile_path);
	    ಮುಕ್ತ(batfile_path);
	}
    }

    mch_chdir(installdir);
    ಪ್ರತ್ಯಾಗಮ found;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
remove_if_exists(ವರ್ಣ *path, ವರ್ಣ *filename)
{
    ವರ್ಣ buf[BUFSIZE];
    FILE *fd;

    sprintf(buf, "%s\\%s", path, filename);

    fd = fopen(buf, "r");
    ಯದಿ (fd == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    fclose(fd);
    printf("removing %s\n", buf);
    remove(buf);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
remove_icons(ಶೂನ್ಯ)
{
    ವರ್ಣ	path[BUFSIZE];
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (get_shell_folder_path(path, "desktop"))
	ಪ್ರತಿ (i = 0; i < ICON_ಎಣಿಕೆ; ++i)
	    remove_if_exists(path, icon_link_names[i]);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
remove_start_menu(ಶೂನ್ಯ)
{
    ವರ್ಣ	path[BUFSIZE];
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಸಂರಚನಾ stat st;

    ಯದಿ (get_shell_folder_path(path, VIM_STARTMENU) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (i = 1; i < TARGET_ಎಣಿಕೆ; ++i)
	remove_if_exists(path, targets[i].lnkname);
    remove_if_exists(path, "uninstall.lnk");
    remove_if_exists(path, "Help.lnk");
    // Win95 uses .pif, WinNT uses .lnk
    remove_if_exists(path, "Vim tutor.pif");
    remove_if_exists(path, "Vim tutor.lnk");
    remove_if_exists(path, "Vim online.url");
    ಯದಿ (stat(path, &st) == 0)
    {
	printf("removing %s\n", path);
	rmdir(path);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
delete_uninstall_ಕೀಲಿ(ಶೂನ್ಯ)
{
    reg_delete_ಕೀಲಿ(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Vim " VIM_VERSION_SHORT, ಕೀಲಿ_WOW64_64_ಕೀಲಿ);
}

    ಪೂರ್ಣಾಙ್ಕ
main(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ *argv[])
{
    ಪೂರ್ಣಾಙ್ಕ		found = 0;
    FILE	*fd;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಸಂರಚನಾ stat st;
    ವರ್ಣ	icon[BUFSIZE];
    ವರ್ಣ	path[MAX_PATH];
    ವರ್ಣ	popup_path[MAX_PATH];

    // The nsis uninstaller calls us with a "-nsis" argument.
    ಯದಿ (argc == 2 && stricmp(argv[1], "-nsis") == 0)
	interactive = FALSE;
    ಅನ್ಯಥಾ
	interactive = TRUE;

    // Initialize this program.
    do_inits(argv);

    printf("This program will remove the following items:\n");

    ಯದಿ (popup_gvim_path(popup_path, ಪರಿಮಾಣ(popup_path)))
    {
	printf(" - the \"Edit with Vim\" entry in the popup menu\n");
	printf("   which uses \"%s\"\n", popup_path);
	ಯದಿ (interactive)
	    printf("\nRemove it (y/n)? ");
	ಯದಿ (!interactive || confirm())
	{
	    remove_popup();
	    // Assume the "Open With" entry can be removed as well, don't
	    // bother the user with asking him again.
	    remove_openwith();
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (openwith_gvim_path(popup_path, ಪರಿಮಾಣ(popup_path)))
    {
	printf(" - the Vim \"Open With...\" entry in the popup menu\n");
	printf("   which uses \"%s\"\n", popup_path);
	printf("\nRemove it (y/n)? ");
	ಯದಿ (confirm())
	    remove_openwith();
    }

    ಯದಿ (get_shell_folder_path(path, "desktop"))
    {
	printf("\n");
	ಪ್ರತಿ (i = 0; i < ICON_ಎಣಿಕೆ; ++i)
	{
	    sprintf(icon, "%s\\%s", path, icon_link_names[i]);
	    ಯದಿ (stat(icon, &st) == 0)
	    {
		printf(" - the \"%s\" icon on the desktop\n", icon_names[i]);
		++found;
	    }
	}
	ಯದಿ (found > 0)
	{
	    ಯದಿ (interactive)
		printf("\nRemove %s (y/n)? ", found > 1 ? "them" : "it");
	    ಯದಿ (!interactive || confirm())
		remove_icons();
	}
    }

    ಯದಿ (get_shell_folder_path(path, VIM_STARTMENU)
	    && stat(path, &st) == 0)
    {
	printf("\n - the \"%s\" entry in the Start Menu\n", VIM_STARTMENU);
	ಯದಿ (interactive)
	    printf("\nRemove it (y/n)? ");
	ಯದಿ (!interactive || confirm())
	    remove_start_menu();
    }

    printf("\n");
    found = remove_batfiles(0);
    ಯದಿ (found > 0)
    {
	ಯದಿ (interactive)
	    printf("\nRemove %s (y/n)? ", found > 1 ? "them" : "it");
	ಯದಿ (!interactive || confirm())
	    remove_batfiles(1);
    }

    fd = fopen("gvim.exe", "r");
    ಯದಿ (fd != ರಿಕ್ತ)
    {
	fclose(fd);
	printf("gvim.exe detected.  Attempting to unregister gvim with OLE\n");
	system("gvim.exe -silent -unregister");
    }

    delete_uninstall_ಕೀಲಿ();

    ಯದಿ (interactive)
    {
	printf("\nYou may now want to delete the Vim executables and runtime files.\n");
	printf("(They are still where you unpacked them.)\n");
    }

    ಯದಿ (interactive)
    {
	rewind(stdin);
	printf("\nPress Enter to exit...");
	(ಶೂನ್ಯ)getchar();
    }
    ಅನ್ಯಥಾ
	sleep(3);

    ಪ್ರತ್ಯಾಗಮ 0;
}
