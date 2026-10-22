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
#شامل "dosinst.h"

/*
 * Return TRUE if the user types a 'y' or 'Y', FALSE otherwise.
 */
    ایستا صحیح
confirm(تهی)
{
    نویسه	answer[10];

    fflush(stdout);
    بازگشت (scanf(" %c", answer) == 1 && toupper((unsigned نویسه)answer[0]) == 'Y');
}

    ایستا صحیح
reg_delete_کلید(HKEY h_ریشه_کلید, ثابت نویسه *کلید, DWORD flag)
{
    ایستا صحیح did_load = FALSE;
    ایستا HANDLE advapi_lib = پوچ;
    ایستا LONG (WINAPI *delete_کلید_ex)(HKEY, LPCTSTR, REGSAM, DWORD) = پوچ;

    اگر (!did_load)
    {
	// The RegDeleteKeyEx() function is only available on new systems.  It
	// is required for 64-bit registry access.  For other systems fall
	// back to RegDeleteKey().
	did_load = TRUE;
	advapi_lib = LoadLibrary("ADVAPI32.DLL");
	اگر (advapi_lib != پوچ)
	    delete_کلید_ex = (LONG (WINAPI *)(HKEY, LPCTSTR, REGSAM, DWORD))GetProcAddress(advapi_lib, "RegDeleteKeyExA");
    }
    اگر (delete_کلید_ex != پوچ)
    {
	بازگشت (*delete_کلید_ex)(h_ریشه_کلید, کلید, flag, 0);
    }
    بازگشت Reg_Delete_کلید(h_ریشه_کلید, کلید);
}

/*
 * Check if the popup menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    ایستا صحیح
popup_gvim_path(نویسه *buf, DWORD bufsize)
{
    HKEY	کلید_handle;
    DWORD	مقدار_type;
    صحیح		r;

    // Open the key where the path to gvim.exe is stored.
    اگر (Reg_Open_کلید_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    کلید_WOW64_64_کلید | کلید_READ, &کلید_handle) != ERROR_SUCCESS)
	اگر (Reg_Open_کلید_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    کلید_WOW64_32_کلید | کلید_READ, &کلید_handle) != ERROR_SUCCESS)
	    بازگشت 0;

    // get the DisplayName out of it to show the user
    r = Reg_Query_مقدار_Ex(کلید_handle, "path", 0,
					  &مقدار_type, (LPBYTE)buf, &bufsize);
    Reg_Close_کلید(کلید_handle);

    بازگشت (r == ERROR_SUCCESS);
}

/*
 * Check if the "Open With..." menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    ایستا صحیح
openwith_gvim_path(نویسه *buf, DWORD bufsize)
{
    HKEY	کلید_handle;
    DWORD	مقدار_type;
    صحیح		r;

    // Open the key where the path to gvim.exe is stored.
    اگر (Reg_Open_کلید_Ex(HKEY_CLASSES_ریشه,
		"Applications\\gvim.exe\\shell\\edit\\command", 0,
		    کلید_WOW64_64_کلید | کلید_READ, &کلید_handle) != ERROR_SUCCESS)
	بازگشت 0;

    // get the DisplayName out of it to show the user
    r = Reg_Query_مقدار_Ex(کلید_handle, "", 0, &مقدار_type, (LPBYTE)buf, &bufsize);
    Reg_Close_کلید(کلید_handle);

    بازگشت (r == ERROR_SUCCESS);
}

    ایستا تهی
remove_popup(تهی)
{
    صحیح		fail = 0;
    صحیح		i;
    صحیح		loop = is_64bit_os() ? 2 : 1;
    صحیح		maxfail = loop * 6;
    DWORD	flag;
    HKEY	kh;

    برای (i = 0; i < loop; i++)
    {
	اگر (i == 0)
	    flag = کلید_WOW64_32_کلید;
	وگرنه
	    flag = کلید_WOW64_64_کلید;

	اگر (reg_delete_کلید(HKEY_CLASSES_ریشه, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}\\InProcServer32", flag) != ERROR_SUCCESS)
	    ++fail;
	اگر (reg_delete_کلید(HKEY_CLASSES_ریشه, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}", flag) != ERROR_SUCCESS)
	    ++fail;
	اگر (reg_delete_کلید(HKEY_CLASSES_ریشه, "*\\shellex\\ContextMenuHandlers\\gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	اگر (Reg_Open_کلید_Ex(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved", 0,
		    flag | کلید_ALL_ACCESS, &kh) != ERROR_SUCCESS)
	    ++fail;
	وگرنه
	{
	    اگر (Reg_Delete_مقدار(kh, "{51EEE242-AD87-11d3-9C1E-0090278BBD99}") != ERROR_SUCCESS)
		++fail;
	    Reg_Close_کلید(kh);
	}
	اگر (reg_delete_کلید(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	اگر (reg_delete_کلید(HKEY_LOCAL_MACHINE, "Software\\Vim", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    اگر (fail == maxfail)
	printf("No Vim popup registry entries could be removed\n");
    وگرنه اگر (fail > 0)
	printf("Some Vim popup registry entries could not be removed\n");
    وگرنه
	printf("The Vim popup registry entries have been removed\n");
}

    ایستا تهی
remove_openwith(تهی)
{
    صحیح		fail = 0;
    صحیح		i;
    صحیح		loop = is_64bit_os() ? 2 : 1;
    صحیح		maxfail = loop * 7;
    DWORD	flag;

    برای (i = 0; i < loop; i++)
    {
	اگر (i == 0)
	    flag = کلید_WOW64_32_کلید;
	وگرنه
	    flag = کلید_WOW64_64_کلید;

	اگر (reg_delete_کلید(HKEY_CLASSES_ریشه, "Applications\\gvim.exe\\shell\\edit\\command", flag) != ERROR_SUCCESS)
	    ++fail;
	اگر (reg_delete_کلید(HKEY_CLASSES_ریشه, "Applications\\gvim.exe\\shell\\edit", flag) != ERROR_SUCCESS)
	    ++fail;
	اگر (reg_delete_کلید(HKEY_CLASSES_ریشه, "Applications\\gvim.exe\\shell", flag) != ERROR_SUCCESS)
	    ++fail;
	اگر (reg_delete_کلید(HKEY_CLASSES_ریشه, "Applications\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	اگر (reg_delete_کلید(HKEY_CLASSES_ریشه, ".htm\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	اگر (reg_delete_کلید(HKEY_CLASSES_ریشه, ".vim\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	اگر (reg_delete_کلید(HKEY_CLASSES_ریشه, "*\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    اگر (fail == maxfail)
	printf("No Vim open-with registry entries could be removed\n");
    وگرنه اگر (fail > 0)
	printf("Some Vim open-with registry entries could not be removed\n");
    وگرنه
	printf("The Vim open-with registry entries have been removed\n");
}

/*
 * Check if a batch file is really for the current version.  Don't delete a
 * batch file that was written for another (possibly newer) version.
 */
    ایستا صحیح
batfile_thisversion(نویسه *path)
{
    FILE	*fd;
    نویسه	سطر[BUFSIZE];
    صحیح		کلید_len = strlen(VIMBAT_UNINSTKEY);
    صحیح		found = FALSE;

    fd = fopen(path, "r");
    اگر (fd == پوچ)
	بازگشت FALSE;

    وقتی (fgets(سطر, اندازه(سطر), fd) != پوچ)
    {
	اگر (strncmp(سطر, VIMBAT_UNINSTKEY, کلید_len) == 0)
	{
	    found = TRUE;
	    بشکن;
	}
    }
    fclose(fd);
    بازگشت found;
}

    ایستا صحیح
remove_batfiles(صحیح doit)
{
    نویسه *batfile_path;
    صحیح	 i;
    صحیح	 found = 0;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    برای (i = 1; i < TARGET_شمار; ++i)
    {
	batfile_path = searchpath_save(targets[i].batname);
	اگر (batfile_path != پوچ && batfile_thisversion(batfile_path))
	{
	    ++found;
	    اگر (doit)
	    {
		printf("removing %s\n", batfile_path);
		remove(batfile_path);
	    }
	    وگرنه
		printf(" - the batch file %s\n", batfile_path);
	    آزاد(batfile_path);
	}
    }

    mch_chdir(installdir);
    بازگشت found;
}

    ایستا تهی
remove_if_exists(نویسه *path, نویسه *filename)
{
    نویسه buf[BUFSIZE];
    FILE *fd;

    sprintf(buf, "%s\\%s", path, filename);

    fd = fopen(buf, "r");
    اگر (fd == پوچ)
	بازگشت;

    fclose(fd);
    printf("removing %s\n", buf);
    remove(buf);
}

    ایستا تهی
remove_icons(تهی)
{
    نویسه	path[BUFSIZE];
    صحیح		i;

    اگر (get_shell_folder_path(path, "desktop"))
	برای (i = 0; i < ICON_شمار; ++i)
	    remove_if_exists(path, icon_link_names[i]);
}

    ایستا تهی
remove_start_menu(تهی)
{
    نویسه	path[BUFSIZE];
    صحیح		i;
    ساختار stat st;

    اگر (get_shell_folder_path(path, VIM_STARTMENU) == FAIL)
	بازگشت;

    برای (i = 1; i < TARGET_شمار; ++i)
	remove_if_exists(path, targets[i].lnkname);
    remove_if_exists(path, "uninstall.lnk");
    remove_if_exists(path, "Help.lnk");
    // Win95 uses .pif, WinNT uses .lnk
    remove_if_exists(path, "Vim tutor.pif");
    remove_if_exists(path, "Vim tutor.lnk");
    remove_if_exists(path, "Vim online.url");
    اگر (stat(path, &st) == 0)
    {
	printf("removing %s\n", path);
	rmdir(path);
    }
}

    ایستا تهی
delete_uninstall_کلید(تهی)
{
    reg_delete_کلید(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Vim " VIM_VERSION_SHORT, کلید_WOW64_64_کلید);
}

    صحیح
اصلی(صحیح argc, نویسه *argv[])
{
    صحیح		found = 0;
    FILE	*fd;
    صحیح		i;
    ساختار stat st;
    نویسه	icon[BUFSIZE];
    نویسه	path[MAX_PATH];
    نویسه	popup_path[MAX_PATH];

    // The nsis uninstaller calls us with a "-nsis" argument.
    اگر (argc == 2 && stricmp(argv[1], "-nsis") == 0)
	interactive = FALSE;
    وگرنه
	interactive = TRUE;

    // Initialize this program.
    do_inits(argv);

    printf("This program will remove the following items:\n");

    اگر (popup_gvim_path(popup_path, اندازه(popup_path)))
    {
	printf(" - the \"Edit with Vim\" entry in the popup menu\n");
	printf("   which uses \"%s\"\n", popup_path);
	اگر (interactive)
	    printf("\nRemove it (y/n)? ");
	اگر (!interactive || confirm())
	{
	    remove_popup();
	    // Assume the "Open With" entry can be removed as well, don't
	    // bother the user with asking him again.
	    remove_openwith();
	}
    }
    وگرنه اگر (openwith_gvim_path(popup_path, اندازه(popup_path)))
    {
	printf(" - the Vim \"Open With...\" entry in the popup menu\n");
	printf("   which uses \"%s\"\n", popup_path);
	printf("\nRemove it (y/n)? ");
	اگر (confirm())
	    remove_openwith();
    }

    اگر (get_shell_folder_path(path, "desktop"))
    {
	printf("\n");
	برای (i = 0; i < ICON_شمار; ++i)
	{
	    sprintf(icon, "%s\\%s", path, icon_link_names[i]);
	    اگر (stat(icon, &st) == 0)
	    {
		printf(" - the \"%s\" icon on the desktop\n", icon_names[i]);
		++found;
	    }
	}
	اگر (found > 0)
	{
	    اگر (interactive)
		printf("\nRemove %s (y/n)? ", found > 1 ? "them" : "it");
	    اگر (!interactive || confirm())
		remove_icons();
	}
    }

    اگر (get_shell_folder_path(path, VIM_STARTMENU)
	    && stat(path, &st) == 0)
    {
	printf("\n - the \"%s\" entry in the Start Menu\n", VIM_STARTMENU);
	اگر (interactive)
	    printf("\nRemove it (y/n)? ");
	اگر (!interactive || confirm())
	    remove_start_menu();
    }

    printf("\n");
    found = remove_batfiles(0);
    اگر (found > 0)
    {
	اگر (interactive)
	    printf("\nRemove %s (y/n)? ", found > 1 ? "them" : "it");
	اگر (!interactive || confirm())
	    remove_batfiles(1);
    }

    fd = fopen("gvim.exe", "r");
    اگر (fd != پوچ)
    {
	fclose(fd);
	printf("gvim.exe detected.  Attempting to unregister gvim with OLE\n");
	system("gvim.exe -silent -unregister");
    }

    delete_uninstall_کلید();

    اگر (interactive)
    {
	printf("\nYou may now want to delete the Vim executables and runtime files.\n");
	printf("(They are still where you unpacked them.)\n");
    }

    اگر (interactive)
    {
	rewind(stdin);
	printf("\nPress Enter to exit...");
	(تهی)getchar();
    }
    وگرنه
	sleep(3);

    بازگشت 0;
}
