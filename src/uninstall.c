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
    पटिनिवत्त (scanf(" %c", answer) == 1 && toupper((unsigned वर्ण)answer[0]) == 'Y');
}

    स्थिर पूर्णाङ्क
reg_delete_कुञ्चिका(HKEY h_मूलम्_कुञ्चिका, ध्रुव वर्ण *कुञ्चिका, DWORD flag)
{
    स्थिर पूर्णाङ्क did_load = FALSE;
    स्थिर HANDLE advapi_lib = रिक्त;
    स्थिर LONG (WINAPI *delete_कुञ्चिका_ex)(HKEY, LPCTSTR, REGSAM, DWORD) = रिक्त;

    सचे (!did_load)
    {
	// The RegDeleteKeyEx() function is only available on new systems.  It
	// is required for 64-bit registry access.  For other systems fall
	// back to RegDeleteKey().
	did_load = TRUE;
	advapi_lib = LoadLibrary("ADVAPI32.DLL");
	सचे (advapi_lib != रिक्त)
	    delete_कुञ्चिका_ex = (LONG (WINAPI *)(HKEY, LPCTSTR, REGSAM, DWORD))GetProcAddress(advapi_lib, "RegDeleteKeyExA");
    }
    सचे (delete_कुञ्चिका_ex != रिक्त)
    {
	पटिनिवत्त (*delete_कुञ्चिका_ex)(h_मूलम्_कुञ्चिका, कुञ्चिका, flag, 0);
    }
    पटिनिवत्त Reg_Delete_कुञ्चिका(h_मूलम्_कुञ्चिका, कुञ्चिका);
}

/*
 * Check if the popup menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    स्थिर पूर्णाङ्क
popup_gvim_path(वर्ण *buf, DWORD bufsize)
{
    HKEY	कुञ्चिका_handle;
    DWORD	मानम्_type;
    पूर्णाङ्क		r;

    // Open the key where the path to gvim.exe is stored.
    सचे (Reg_Open_कुञ्चिका_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    कुञ्चिका_WOW64_64_कुञ्चिका | कुञ्चिका_READ, &कुञ्चिका_handle) != ERROR_SUCCESS)
	सचे (Reg_Open_कुञ्चिका_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    कुञ्चिका_WOW64_32_कुञ्चिका | कुञ्चिका_READ, &कुञ्चिका_handle) != ERROR_SUCCESS)
	    पटिनिवत्त 0;

    // get the DisplayName out of it to show the user
    r = Reg_Query_मानम्_Ex(कुञ्चिका_handle, "path", 0,
					  &मानम्_type, (LPBYTE)buf, &bufsize);
    Reg_Close_कुञ्चिका(कुञ्चिका_handle);

    पटिनिवत्त (r == ERROR_SUCCESS);
}

/*
 * Check if the "Open With..." menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    स्थिर पूर्णाङ्क
openwith_gvim_path(वर्ण *buf, DWORD bufsize)
{
    HKEY	कुञ्चिका_handle;
    DWORD	मानम्_type;
    पूर्णाङ्क		r;

    // Open the key where the path to gvim.exe is stored.
    सचे (Reg_Open_कुञ्चिका_Ex(HKEY_CLASSES_मूलम्,
		"Applications\\gvim.exe\\shell\\edit\\command", 0,
		    कुञ्चिका_WOW64_64_कुञ्चिका | कुञ्चिका_READ, &कुञ्चिका_handle) != ERROR_SUCCESS)
	पटिनिवत्त 0;

    // get the DisplayName out of it to show the user
    r = Reg_Query_मानम्_Ex(कुञ्चिका_handle, "", 0, &मानम्_type, (LPBYTE)buf, &bufsize);
    Reg_Close_कुञ्चिका(कुञ्चिका_handle);

    पटिनिवत्त (r == ERROR_SUCCESS);
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

    अत्थाय (i = 0; i < loop; i++)
    {
	सचे (i == 0)
	    flag = कुञ्चिका_WOW64_32_कुञ्चिका;
	अञ्ञथा
	    flag = कुञ्चिका_WOW64_64_कुञ्चिका;

	सचे (reg_delete_कुञ्चिका(HKEY_CLASSES_मूलम्, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}\\InProcServer32", flag) != ERROR_SUCCESS)
	    ++fail;
	सचे (reg_delete_कुञ्चिका(HKEY_CLASSES_मूलम्, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}", flag) != ERROR_SUCCESS)
	    ++fail;
	सचे (reg_delete_कुञ्चिका(HKEY_CLASSES_मूलम्, "*\\shellex\\ContextMenuHandlers\\gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	सचे (Reg_Open_कुञ्चिका_Ex(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved", 0,
		    flag | कुञ्चिका_ALL_ACCESS, &kh) != ERROR_SUCCESS)
	    ++fail;
	अञ्ञथा
	{
	    सचे (Reg_Delete_मानम्(kh, "{51EEE242-AD87-11d3-9C1E-0090278BBD99}") != ERROR_SUCCESS)
		++fail;
	    Reg_Close_कुञ्चिका(kh);
	}
	सचे (reg_delete_कुञ्चिका(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	सचे (reg_delete_कुञ्चिका(HKEY_LOCAL_MACHINE, "Software\\Vim", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    सचे (fail == maxfail)
	printf("No Vim popup registry entries could be removed\n");
    अञ्ञथा सचे (fail > 0)
	printf("Some Vim popup registry entries could not be removed\n");
    अञ्ञथा
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

    अत्थाय (i = 0; i < loop; i++)
    {
	सचे (i == 0)
	    flag = कुञ्चिका_WOW64_32_कुञ्चिका;
	अञ्ञथा
	    flag = कुञ्चिका_WOW64_64_कुञ्चिका;

	सचे (reg_delete_कुञ्चिका(HKEY_CLASSES_मूलम्, "Applications\\gvim.exe\\shell\\edit\\command", flag) != ERROR_SUCCESS)
	    ++fail;
	सचे (reg_delete_कुञ्चिका(HKEY_CLASSES_मूलम्, "Applications\\gvim.exe\\shell\\edit", flag) != ERROR_SUCCESS)
	    ++fail;
	सचे (reg_delete_कुञ्चिका(HKEY_CLASSES_मूलम्, "Applications\\gvim.exe\\shell", flag) != ERROR_SUCCESS)
	    ++fail;
	सचे (reg_delete_कुञ्चिका(HKEY_CLASSES_मूलम्, "Applications\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	सचे (reg_delete_कुञ्चिका(HKEY_CLASSES_मूलम्, ".htm\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	सचे (reg_delete_कुञ्चिका(HKEY_CLASSES_मूलम्, ".vim\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	सचे (reg_delete_कुञ्चिका(HKEY_CLASSES_मूलम्, "*\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    सचे (fail == maxfail)
	printf("No Vim open-with registry entries could be removed\n");
    अञ्ञथा सचे (fail > 0)
	printf("Some Vim open-with registry entries could not be removed\n");
    अञ्ञथा
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
    वर्ण	पङ्क्तिः[BUFSIZE];
    पूर्णाङ्क		कुञ्चिका_len = strlen(VIMBAT_UNINSTKEY);
    पूर्णाङ्क		found = FALSE;

    fd = fopen(path, "r");
    सचे (fd == रिक्त)
	पटिनिवत्त FALSE;

    याव (fgets(पङ्क्तिः, परिमाण(पङ्क्तिः), fd) != रिक्त)
    {
	सचे (strncmp(पङ्क्तिः, VIMBAT_UNINSTKEY, कुञ्चिका_len) == 0)
	{
	    found = TRUE;
	    भङ्ग;
	}
    }
    fclose(fd);
    पटिनिवत्त found;
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

    अत्थाय (i = 1; i < TARGET_गणनम्; ++i)
    {
	batfile_path = searchpath_save(targets[i].batname);
	सचे (batfile_path != रिक्त && batfile_thisversion(batfile_path))
	{
	    ++found;
	    सचे (doit)
	    {
		printf("removing %s\n", batfile_path);
		remove(batfile_path);
	    }
	    अञ्ञथा
		printf(" - the batch file %s\n", batfile_path);
	    मुक्त(batfile_path);
	}
    }

    mch_chdir(installdir);
    पटिनिवत्त found;
}

    स्थिर शून्य
remove_if_exists(वर्ण *path, वर्ण *filename)
{
    वर्ण buf[BUFSIZE];
    FILE *fd;

    sprintf(buf, "%s\\%s", path, filename);

    fd = fopen(buf, "r");
    सचे (fd == रिक्त)
	पटिनिवत्त;

    fclose(fd);
    printf("removing %s\n", buf);
    remove(buf);
}

    स्थिर शून्य
remove_icons(शून्य)
{
    वर्ण	path[BUFSIZE];
    पूर्णाङ्क		i;

    सचे (get_shell_folder_path(path, "desktop"))
	अत्थाय (i = 0; i < ICON_गणनम्; ++i)
	    remove_if_exists(path, icon_link_names[i]);
}

    स्थिर शून्य
remove_start_menu(शून्य)
{
    वर्ण	path[BUFSIZE];
    पूर्णाङ्क		i;
    संरचना stat st;

    सचे (get_shell_folder_path(path, VIM_STARTMENU) == FAIL)
	पटिनिवत्त;

    अत्थाय (i = 1; i < TARGET_गणनम्; ++i)
	remove_if_exists(path, targets[i].lnkname);
    remove_if_exists(path, "uninstall.lnk");
    remove_if_exists(path, "Help.lnk");
    // Win95 uses .pif, WinNT uses .lnk
    remove_if_exists(path, "Vim tutor.pif");
    remove_if_exists(path, "Vim tutor.lnk");
    remove_if_exists(path, "Vim online.url");
    सचे (stat(path, &st) == 0)
    {
	printf("removing %s\n", path);
	rmdir(path);
    }
}

    स्थिर शून्य
delete_uninstall_कुञ्चिका(शून्य)
{
    reg_delete_कुञ्चिका(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Vim " VIM_VERSION_SHORT, कुञ्चिका_WOW64_64_कुञ्चिका);
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
    सचे (argc == 2 && stricmp(argv[1], "-nsis") == 0)
	interactive = FALSE;
    अञ्ञथा
	interactive = TRUE;

    // Initialize this program.
    do_inits(argv);

    printf("This program will remove the following items:\n");

    सचे (popup_gvim_path(popup_path, परिमाण(popup_path)))
    {
	printf(" - the \"Edit with Vim\" entry in the popup menu\n");
	printf("   which uses \"%s\"\n", popup_path);
	सचे (interactive)
	    printf("\nRemove it (y/n)? ");
	सचे (!interactive || confirm())
	{
	    remove_popup();
	    // Assume the "Open With" entry can be removed as well, don't
	    // bother the user with asking him again.
	    remove_openwith();
	}
    }
    अञ्ञथा सचे (openwith_gvim_path(popup_path, परिमाण(popup_path)))
    {
	printf(" - the Vim \"Open With...\" entry in the popup menu\n");
	printf("   which uses \"%s\"\n", popup_path);
	printf("\nRemove it (y/n)? ");
	सचे (confirm())
	    remove_openwith();
    }

    सचे (get_shell_folder_path(path, "desktop"))
    {
	printf("\n");
	अत्थाय (i = 0; i < ICON_गणनम्; ++i)
	{
	    sprintf(icon, "%s\\%s", path, icon_link_names[i]);
	    सचे (stat(icon, &st) == 0)
	    {
		printf(" - the \"%s\" icon on the desktop\n", icon_names[i]);
		++found;
	    }
	}
	सचे (found > 0)
	{
	    सचे (interactive)
		printf("\nRemove %s (y/n)? ", found > 1 ? "them" : "it");
	    सचे (!interactive || confirm())
		remove_icons();
	}
    }

    सचे (get_shell_folder_path(path, VIM_STARTMENU)
	    && stat(path, &st) == 0)
    {
	printf("\n - the \"%s\" entry in the Start Menu\n", VIM_STARTMENU);
	सचे (interactive)
	    printf("\nRemove it (y/n)? ");
	सचे (!interactive || confirm())
	    remove_start_menu();
    }

    printf("\n");
    found = remove_batfiles(0);
    सचे (found > 0)
    {
	सचे (interactive)
	    printf("\nRemove %s (y/n)? ", found > 1 ? "them" : "it");
	सचे (!interactive || confirm())
	    remove_batfiles(1);
    }

    fd = fopen("gvim.exe", "r");
    सचे (fd != रिक्त)
    {
	fclose(fd);
	printf("gvim.exe detected.  Attempting to unregister gvim with OLE\n");
	system("gvim.exe -silent -unregister");
    }

    delete_uninstall_कुञ्चिका();

    सचे (interactive)
    {
	printf("\nYou may now want to delete the Vim executables and runtime files.\n");
	printf("(They are still where you unpacked them.)\n");
    }

    सचे (interactive)
    {
	rewind(stdin);
	printf("\nPress Enter to exit...");
	(शून्य)getchar();
    }
    अञ्ञथा
	sleep(3);

    पटिनिवत्त 0;
}
