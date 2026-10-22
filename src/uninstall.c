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
#अन्तर्भाव "dosinst.h"

/*
 * Return TRUE if the user types a 'y' or 'Y', FALSE otherwise.
 */
    स्थिर पूर्णाङ्क
confirm(शून्य)
{
    वर्ण	answer[10];

    fflush(stdout);
    प्रत्यागम (scanf(" %c", answer) == 1 && toupper((unsigned वर्ण)answer[0]) == 'Y');
}

    स्थिर पूर्णाङ्क
reg_delete_किल्ली(HKEY h_मूळ_किल्ली, ध्रुव वर्ण *किल्ली, DWORD flag)
{
    स्थिर पूर्णाङ्क did_load = FALSE;
    स्थिर HANDLE advapi_lib = रिक्त;
    स्थिर LONG (WINAPI *delete_किल्ली_ex)(HKEY, LPCTSTR, REGSAM, DWORD) = रिक्त;

    यदि (!did_load)
    {
	// The RegDeleteKeyEx() function is only available on new systems.  It
	// is required for 64-bit registry access.  For other systems fall
	// back to RegDeleteKey().
	did_load = TRUE;
	advapi_lib = LoadLibrary("ADVAPI32.DLL");
	यदि (advapi_lib != रिक्त)
	    delete_किल्ली_ex = (LONG (WINAPI *)(HKEY, LPCTSTR, REGSAM, DWORD))GetProcAddress(advapi_lib, "RegDeleteKeyExA");
    }
    यदि (delete_किल्ली_ex != रिक्त)
    {
	प्रत्यागम (*delete_किल्ली_ex)(h_मूळ_किल्ली, किल्ली, flag, 0);
    }
    प्रत्यागम Reg_Delete_किल्ली(h_मूळ_किल्ली, किल्ली);
}

/*
 * Check if the popup menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    स्थिर पूर्णाङ्क
popup_gvim_path(वर्ण *buf, DWORD bufsize)
{
    HKEY	किल्ली_handle;
    DWORD	मूल्य_type;
    पूर्णाङ्क		r;

    // Open the key where the path to gvim.exe is stored.
    यदि (Reg_Open_किल्ली_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    किल्ली_WOW64_64_किल्ली | किल्ली_READ, &किल्ली_handle) != ERROR_SUCCESS)
	यदि (Reg_Open_किल्ली_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    किल्ली_WOW64_32_किल्ली | किल्ली_READ, &किल्ली_handle) != ERROR_SUCCESS)
	    प्रत्यागम 0;

    // get the DisplayName out of it to show the user
    r = Reg_Query_मूल्य_Ex(किल्ली_handle, "path", 0,
					  &मूल्य_type, (LPBYTE)buf, &bufsize);
    Reg_Close_किल्ली(किल्ली_handle);

    प्रत्यागम (r == ERROR_SUCCESS);
}

/*
 * Check if the "Open With..." menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    स्थिर पूर्णाङ्क
openwith_gvim_path(वर्ण *buf, DWORD bufsize)
{
    HKEY	किल्ली_handle;
    DWORD	मूल्य_type;
    पूर्णाङ्क		r;

    // Open the key where the path to gvim.exe is stored.
    यदि (Reg_Open_किल्ली_Ex(HKEY_CLASSES_मूळ,
		"Applications\\gvim.exe\\shell\\edit\\command", 0,
		    किल्ली_WOW64_64_किल्ली | किल्ली_READ, &किल्ली_handle) != ERROR_SUCCESS)
	प्रत्यागम 0;

    // get the DisplayName out of it to show the user
    r = Reg_Query_मूल्य_Ex(किल्ली_handle, "", 0, &मूल्य_type, (LPBYTE)buf, &bufsize);
    Reg_Close_किल्ली(किल्ली_handle);

    प्रत्यागम (r == ERROR_SUCCESS);
}

    स्थिर शून्य
remove_popup(शून्य)
{
    पूर्णाङ्क		fail = 0;
    पूर्णाङ्क		i;
    पूर्णाङ्क		loop = is_64bit_os() ? 2 : 1;
    पूर्णाङ्क		maxfail = loop * 6;
    DWORD	flag;
    HKEY	kh;

    प्रति (i = 0; i < loop; i++)
    {
	यदि (i == 0)
	    flag = किल्ली_WOW64_32_किल्ली;
	अन्यथा
	    flag = किल्ली_WOW64_64_किल्ली;

	यदि (reg_delete_किल्ली(HKEY_CLASSES_मूळ, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}\\InProcServer32", flag) != ERROR_SUCCESS)
	    ++fail;
	यदि (reg_delete_किल्ली(HKEY_CLASSES_मूळ, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}", flag) != ERROR_SUCCESS)
	    ++fail;
	यदि (reg_delete_किल्ली(HKEY_CLASSES_मूळ, "*\\shellex\\ContextMenuHandlers\\gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	यदि (Reg_Open_किल्ली_Ex(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved", 0,
		    flag | किल्ली_ALL_ACCESS, &kh) != ERROR_SUCCESS)
	    ++fail;
	अन्यथा
	{
	    यदि (Reg_Delete_मूल्य(kh, "{51EEE242-AD87-11d3-9C1E-0090278BBD99}") != ERROR_SUCCESS)
		++fail;
	    Reg_Close_किल्ली(kh);
	}
	यदि (reg_delete_किल्ली(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	यदि (reg_delete_किल्ली(HKEY_LOCAL_MACHINE, "Software\\Vim", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    यदि (fail == maxfail)
	printf("No Vim popup registry entries could be removed\n");
    अन्यथा यदि (fail > 0)
	printf("Some Vim popup registry entries could not be removed\n");
    अन्यथा
	printf("The Vim popup registry entries have been removed\n");
}

    स्थिर शून्य
remove_openwith(शून्य)
{
    पूर्णाङ्क		fail = 0;
    पूर्णाङ्क		i;
    पूर्णाङ्क		loop = is_64bit_os() ? 2 : 1;
    पूर्णाङ्क		maxfail = loop * 7;
    DWORD	flag;

    प्रति (i = 0; i < loop; i++)
    {
	यदि (i == 0)
	    flag = किल्ली_WOW64_32_किल्ली;
	अन्यथा
	    flag = किल्ली_WOW64_64_किल्ली;

	यदि (reg_delete_किल्ली(HKEY_CLASSES_मूळ, "Applications\\gvim.exe\\shell\\edit\\command", flag) != ERROR_SUCCESS)
	    ++fail;
	यदि (reg_delete_किल्ली(HKEY_CLASSES_मूळ, "Applications\\gvim.exe\\shell\\edit", flag) != ERROR_SUCCESS)
	    ++fail;
	यदि (reg_delete_किल्ली(HKEY_CLASSES_मूळ, "Applications\\gvim.exe\\shell", flag) != ERROR_SUCCESS)
	    ++fail;
	यदि (reg_delete_किल्ली(HKEY_CLASSES_मूळ, "Applications\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	यदि (reg_delete_किल्ली(HKEY_CLASSES_मूळ, ".htm\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	यदि (reg_delete_किल्ली(HKEY_CLASSES_मूळ, ".vim\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	यदि (reg_delete_किल्ली(HKEY_CLASSES_मूळ, "*\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    यदि (fail == maxfail)
	printf("No Vim open-with registry entries could be removed\n");
    अन्यथा यदि (fail > 0)
	printf("Some Vim open-with registry entries could not be removed\n");
    अन्यथा
	printf("The Vim open-with registry entries have been removed\n");
}

/*
 * Check if a batch file is really for the current version.  Don't delete a
 * batch file that was written for another (possibly newer) version.
 */
    स्थिर पूर्णाङ्क
batfile_thisversion(वर्ण *path)
{
    FILE	*fd;
    वर्ण	ओळ[BUFSIZE];
    पूर्णाङ्क		किल्ली_len = strlen(VIMBAT_UNINSTKEY);
    पूर्णाङ्क		found = FALSE;

    fd = fopen(path, "r");
    यदि (fd == रिक्त)
	प्रत्यागम FALSE;

    यावत् (fgets(ओळ, परिमाण(ओळ), fd) != रिक्त)
    {
	यदि (strncmp(ओळ, VIMBAT_UNINSTKEY, किल्ली_len) == 0)
	{
	    found = TRUE;
	    भङ्ग;
	}
    }
    fclose(fd);
    प्रत्यागम found;
}

    स्थिर पूर्णाङ्क
remove_batfiles(पूर्णाङ्क doit)
{
    वर्ण *batfile_path;
    पूर्णाङ्क	 i;
    पूर्णाङ्क	 found = 0;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    प्रति (i = 1; i < TARGET_मोजणी; ++i)
    {
	batfile_path = searchpath_save(targets[i].batname);
	यदि (batfile_path != रिक्त && batfile_thisversion(batfile_path))
	{
	    ++found;
	    यदि (doit)
	    {
		printf("removing %s\n", batfile_path);
		remove(batfile_path);
	    }
	    अन्यथा
		printf(" - the batch file %s\n", batfile_path);
	    मुक्त(batfile_path);
	}
    }

    mch_chdir(installdir);
    प्रत्यागम found;
}

    स्थिर शून्य
remove_if_exists(वर्ण *path, वर्ण *filename)
{
    वर्ण buf[BUFSIZE];
    FILE *fd;

    sprintf(buf, "%s\\%s", path, filename);

    fd = fopen(buf, "r");
    यदि (fd == रिक्त)
	प्रत्यागम;

    fclose(fd);
    printf("removing %s\n", buf);
    remove(buf);
}

    स्थिर शून्य
remove_icons(शून्य)
{
    वर्ण	path[BUFSIZE];
    पूर्णाङ्क		i;

    यदि (get_shell_folder_path(path, "desktop"))
	प्रति (i = 0; i < ICON_मोजणी; ++i)
	    remove_if_exists(path, icon_link_names[i]);
}

    स्थिर शून्य
remove_start_menu(शून्य)
{
    वर्ण	path[BUFSIZE];
    पूर्णाङ्क		i;
    संरचना stat st;

    यदि (get_shell_folder_path(path, VIM_STARTMENU) == FAIL)
	प्रत्यागम;

    प्रति (i = 1; i < TARGET_मोजणी; ++i)
	remove_if_exists(path, targets[i].lnkname);
    remove_if_exists(path, "uninstall.lnk");
    remove_if_exists(path, "Help.lnk");
    // Win95 uses .pif, WinNT uses .lnk
    remove_if_exists(path, "Vim tutor.pif");
    remove_if_exists(path, "Vim tutor.lnk");
    remove_if_exists(path, "Vim online.url");
    यदि (stat(path, &st) == 0)
    {
	printf("removing %s\n", path);
	rmdir(path);
    }
}

    स्थिर शून्य
delete_uninstall_किल्ली(शून्य)
{
    reg_delete_किल्ली(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Vim " VIM_VERSION_SHORT, किल्ली_WOW64_64_किल्ली);
}

    पूर्णाङ्क
main(पूर्णाङ्क argc, वर्ण *argv[])
{
    पूर्णाङ्क		found = 0;
    FILE	*fd;
    पूर्णाङ्क		i;
    संरचना stat st;
    वर्ण	icon[BUFSIZE];
    वर्ण	path[MAX_PATH];
    वर्ण	popup_path[MAX_PATH];

    // The nsis uninstaller calls us with a "-nsis" argument.
    यदि (argc == 2 && stricmp(argv[1], "-nsis") == 0)
	interactive = FALSE;
    अन्यथा
	interactive = TRUE;

    // Initialize this program.
    do_inits(argv);

    printf("This program will remove the following items:\n");

    यदि (popup_gvim_path(popup_path, परिमाण(popup_path)))
    {
	printf(" - the \"Edit with Vim\" entry in the popup menu\n");
	printf("   which uses \"%s\"\n", popup_path);
	यदि (interactive)
	    printf("\nRemove it (y/n)? ");
	यदि (!interactive || confirm())
	{
	    remove_popup();
	    // Assume the "Open With" entry can be removed as well, don't
	    // bother the user with asking him again.
	    remove_openwith();
	}
    }
    अन्यथा यदि (openwith_gvim_path(popup_path, परिमाण(popup_path)))
    {
	printf(" - the Vim \"Open With...\" entry in the popup menu\n");
	printf("   which uses \"%s\"\n", popup_path);
	printf("\nRemove it (y/n)? ");
	यदि (confirm())
	    remove_openwith();
    }

    यदि (get_shell_folder_path(path, "desktop"))
    {
	printf("\n");
	प्रति (i = 0; i < ICON_मोजणी; ++i)
	{
	    sprintf(icon, "%s\\%s", path, icon_link_names[i]);
	    यदि (stat(icon, &st) == 0)
	    {
		printf(" - the \"%s\" icon on the desktop\n", icon_names[i]);
		++found;
	    }
	}
	यदि (found > 0)
	{
	    यदि (interactive)
		printf("\nRemove %s (y/n)? ", found > 1 ? "them" : "it");
	    यदि (!interactive || confirm())
		remove_icons();
	}
    }

    यदि (get_shell_folder_path(path, VIM_STARTMENU)
	    && stat(path, &st) == 0)
    {
	printf("\n - the \"%s\" entry in the Start Menu\n", VIM_STARTMENU);
	यदि (interactive)
	    printf("\nRemove it (y/n)? ");
	यदि (!interactive || confirm())
	    remove_start_menu();
    }

    printf("\n");
    found = remove_batfiles(0);
    यदि (found > 0)
    {
	यदि (interactive)
	    printf("\nRemove %s (y/n)? ", found > 1 ? "them" : "it");
	यदि (!interactive || confirm())
	    remove_batfiles(1);
    }

    fd = fopen("gvim.exe", "r");
    यदि (fd != रिक्त)
    {
	fclose(fd);
	printf("gvim.exe detected.  Attempting to unregister gvim with OLE\n");
	system("gvim.exe -silent -unregister");
    }

    delete_uninstall_किल्ली();

    यदि (interactive)
    {
	printf("\nYou may now want to delete the Vim executables and runtime files.\n");
	printf("(They are still where you unpacked them.)\n");
    }

    यदि (interactive)
    {
	rewind(stdin);
	printf("\nPress Enter to exit...");
	(शून्य)getchar();
    }
    अन्यथा
	sleep(3);

    प्रत्यागम 0;
}
