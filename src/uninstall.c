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
#অন্তর্ভাব "dosinst.h"

/*
 * Return TRUE if the user types a 'y' or 'Y', FALSE otherwise.
 */
    স্থির পূর্ণসংখ্যা
confirm(শূন্য)
{
    অক্ষর	answer[10];

    fflush(stdout);
    ফেরত (scanf(" %c", answer) == 1 && toupper((unsigned অক্ষর)answer[0]) == 'Y');
}

    স্থির পূর্ণসংখ্যা
reg_delete_চাবি(HKEY h_মূল_চাবি, ধ্রুবক অক্ষর *চাবি, DWORD flag)
{
    স্থির পূর্ণসংখ্যা did_load = FALSE;
    স্থির HANDLE advapi_lib = ফাঁকা;
    স্থির LONG (WINAPI *delete_চাবি_ex)(HKEY, LPCTSTR, REGSAM, DWORD) = ফাঁকা;

    যদি (!did_load)
    {
	// The RegDeleteKeyEx() function is only available on new systems.  It
	// is required for 64-bit registry access.  For other systems fall
	// back to RegDeleteKey().
	did_load = TRUE;
	advapi_lib = LoadLibrary("ADVAPI32.DLL");
	যদি (advapi_lib != ফাঁকা)
	    delete_চাবি_ex = (LONG (WINAPI *)(HKEY, LPCTSTR, REGSAM, DWORD))GetProcAddress(advapi_lib, "RegDeleteKeyExA");
    }
    যদি (delete_চাবি_ex != ফাঁকা)
    {
	ফেরত (*delete_চাবি_ex)(h_মূল_চাবি, চাবি, flag, 0);
    }
    ফেরত Reg_Delete_চাবি(h_মূল_চাবি, চাবি);
}

/*
 * Check if the popup menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    স্থির পূর্ণসংখ্যা
popup_gvim_path(অক্ষর *buf, DWORD bufsize)
{
    HKEY	চাবি_handle;
    DWORD	মান_type;
    পূর্ণসংখ্যা		r;

    // Open the key where the path to gvim.exe is stored.
    যদি (Reg_Open_চাবি_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    চাবি_WOW64_64_চাবি | চাবি_READ, &চাবি_handle) != ERROR_SUCCESS)
	যদি (Reg_Open_চাবি_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    চাবি_WOW64_32_চাবি | চাবি_READ, &চাবি_handle) != ERROR_SUCCESS)
	    ফেরত 0;

    // get the DisplayName out of it to show the user
    r = Reg_Query_মান_Ex(চাবি_handle, "path", 0,
					  &মান_type, (LPBYTE)buf, &bufsize);
    Reg_Close_চাবি(চাবি_handle);

    ফেরত (r == ERROR_SUCCESS);
}

/*
 * Check if the "Open With..." menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    স্থির পূর্ণসংখ্যা
openwith_gvim_path(অক্ষর *buf, DWORD bufsize)
{
    HKEY	চাবি_handle;
    DWORD	মান_type;
    পূর্ণসংখ্যা		r;

    // Open the key where the path to gvim.exe is stored.
    যদি (Reg_Open_চাবি_Ex(HKEY_CLASSES_মূল,
		"Applications\\gvim.exe\\shell\\edit\\command", 0,
		    চাবি_WOW64_64_চাবি | চাবি_READ, &চাবি_handle) != ERROR_SUCCESS)
	ফেরত 0;

    // get the DisplayName out of it to show the user
    r = Reg_Query_মান_Ex(চাবি_handle, "", 0, &মান_type, (LPBYTE)buf, &bufsize);
    Reg_Close_চাবি(চাবি_handle);

    ফেরত (r == ERROR_SUCCESS);
}

    স্থির শূন্য
remove_popup(শূন্য)
{
    পূর্ণসংখ্যা		fail = 0;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		loop = is_64bit_os() ? 2 : 1;
    পূর্ণসংখ্যা		maxfail = loop * 6;
    DWORD	flag;
    HKEY	kh;

    জন্য (i = 0; i < loop; i++)
    {
	যদি (i == 0)
	    flag = চাবি_WOW64_32_চাবি;
	অন্যথা
	    flag = চাবি_WOW64_64_চাবি;

	যদি (reg_delete_চাবি(HKEY_CLASSES_মূল, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}\\InProcServer32", flag) != ERROR_SUCCESS)
	    ++fail;
	যদি (reg_delete_চাবি(HKEY_CLASSES_মূল, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}", flag) != ERROR_SUCCESS)
	    ++fail;
	যদি (reg_delete_চাবি(HKEY_CLASSES_মূল, "*\\shellex\\ContextMenuHandlers\\gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	যদি (Reg_Open_চাবি_Ex(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved", 0,
		    flag | চাবি_ALL_ACCESS, &kh) != ERROR_SUCCESS)
	    ++fail;
	অন্যথা
	{
	    যদি (Reg_Delete_মান(kh, "{51EEE242-AD87-11d3-9C1E-0090278BBD99}") != ERROR_SUCCESS)
		++fail;
	    Reg_Close_চাবি(kh);
	}
	যদি (reg_delete_চাবি(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	যদি (reg_delete_চাবি(HKEY_LOCAL_MACHINE, "Software\\Vim", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    যদি (fail == maxfail)
	printf("No Vim popup registry entries could be removed\n");
    অন্যথা যদি (fail > 0)
	printf("Some Vim popup registry entries could not be removed\n");
    অন্যথা
	printf("The Vim popup registry entries have been removed\n");
}

    স্থির শূন্য
remove_openwith(শূন্য)
{
    পূর্ণসংখ্যা		fail = 0;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		loop = is_64bit_os() ? 2 : 1;
    পূর্ণসংখ্যা		maxfail = loop * 7;
    DWORD	flag;

    জন্য (i = 0; i < loop; i++)
    {
	যদি (i == 0)
	    flag = চাবি_WOW64_32_চাবি;
	অন্যথা
	    flag = চাবি_WOW64_64_চাবি;

	যদি (reg_delete_চাবি(HKEY_CLASSES_মূল, "Applications\\gvim.exe\\shell\\edit\\command", flag) != ERROR_SUCCESS)
	    ++fail;
	যদি (reg_delete_চাবি(HKEY_CLASSES_মূল, "Applications\\gvim.exe\\shell\\edit", flag) != ERROR_SUCCESS)
	    ++fail;
	যদি (reg_delete_চাবি(HKEY_CLASSES_মূল, "Applications\\gvim.exe\\shell", flag) != ERROR_SUCCESS)
	    ++fail;
	যদি (reg_delete_চাবি(HKEY_CLASSES_মূল, "Applications\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	যদি (reg_delete_চাবি(HKEY_CLASSES_মূল, ".htm\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	যদি (reg_delete_চাবি(HKEY_CLASSES_মূল, ".vim\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	যদি (reg_delete_চাবি(HKEY_CLASSES_মূল, "*\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    যদি (fail == maxfail)
	printf("No Vim open-with registry entries could be removed\n");
    অন্যথা যদি (fail > 0)
	printf("Some Vim open-with registry entries could not be removed\n");
    অন্যথা
	printf("The Vim open-with registry entries have been removed\n");
}

/*
 * Check if a batch file is really for the current version.  Don't delete a
 * batch file that was written for another (possibly newer) version.
 */
    স্থির পূর্ণসংখ্যা
batfile_thisversion(অক্ষর *path)
{
    FILE	*fd;
    অক্ষর	সারি[BUFSIZE];
    পূর্ণসংখ্যা		চাবি_len = strlen(VIMBAT_UNINSTKEY);
    পূর্ণসংখ্যা		found = FALSE;

    fd = fopen(path, "r");
    যদি (fd == ফাঁকা)
	ফেরত FALSE;

    যখন (fgets(সারি, আকার(সারি), fd) != ফাঁকা)
    {
	যদি (strncmp(সারি, VIMBAT_UNINSTKEY, চাবি_len) == 0)
	{
	    found = TRUE;
	    বিরতি;
	}
    }
    fclose(fd);
    ফেরত found;
}

    স্থির পূর্ণসংখ্যা
remove_batfiles(পূর্ণসংখ্যা doit)
{
    অক্ষর *batfile_path;
    পূর্ণসংখ্যা	 i;
    পূর্ণসংখ্যা	 found = 0;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    জন্য (i = 1; i < TARGET_গুনতি; ++i)
    {
	batfile_path = searchpath_save(targets[i].batname);
	যদি (batfile_path != ফাঁকা && batfile_thisversion(batfile_path))
	{
	    ++found;
	    যদি (doit)
	    {
		printf("removing %s\n", batfile_path);
		remove(batfile_path);
	    }
	    অন্যথা
		printf(" - the batch file %s\n", batfile_path);
	    মুক্ত(batfile_path);
	}
    }

    mch_chdir(installdir);
    ফেরত found;
}

    স্থির শূন্য
remove_if_exists(অক্ষর *path, অক্ষর *filename)
{
    অক্ষর buf[BUFSIZE];
    FILE *fd;

    sprintf(buf, "%s\\%s", path, filename);

    fd = fopen(buf, "r");
    যদি (fd == ফাঁকা)
	ফেরত;

    fclose(fd);
    printf("removing %s\n", buf);
    remove(buf);
}

    স্থির শূন্য
remove_icons(শূন্য)
{
    অক্ষর	path[BUFSIZE];
    পূর্ণসংখ্যা		i;

    যদি (get_shell_folder_path(path, "desktop"))
	জন্য (i = 0; i < ICON_গুনতি; ++i)
	    remove_if_exists(path, icon_link_names[i]);
}

    স্থির শূন্য
remove_start_menu(শূন্য)
{
    অক্ষর	path[BUFSIZE];
    পূর্ণসংখ্যা		i;
    কাঠামো stat st;

    যদি (get_shell_folder_path(path, VIM_STARTMENU) == FAIL)
	ফেরত;

    জন্য (i = 1; i < TARGET_গুনতি; ++i)
	remove_if_exists(path, targets[i].lnkname);
    remove_if_exists(path, "uninstall.lnk");
    remove_if_exists(path, "Help.lnk");
    // Win95 uses .pif, WinNT uses .lnk
    remove_if_exists(path, "Vim tutor.pif");
    remove_if_exists(path, "Vim tutor.lnk");
    remove_if_exists(path, "Vim online.url");
    যদি (stat(path, &st) == 0)
    {
	printf("removing %s\n", path);
	rmdir(path);
    }
}

    স্থির শূন্য
delete_uninstall_চাবি(শূন্য)
{
    reg_delete_চাবি(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Vim " VIM_VERSION_SHORT, চাবি_WOW64_64_চাবি);
}

    পূর্ণসংখ্যা
প্রধান(পূর্ণসংখ্যা argc, অক্ষর *argv[])
{
    পূর্ণসংখ্যা		found = 0;
    FILE	*fd;
    পূর্ণসংখ্যা		i;
    কাঠামো stat st;
    অক্ষর	icon[BUFSIZE];
    অক্ষর	path[MAX_PATH];
    অক্ষর	popup_path[MAX_PATH];

    // The nsis uninstaller calls us with a "-nsis" argument.
    যদি (argc == 2 && stricmp(argv[1], "-nsis") == 0)
	interactive = FALSE;
    অন্যথা
	interactive = TRUE;

    // Initialize this program.
    do_inits(argv);

    printf("This program will remove the following items:\n");

    যদি (popup_gvim_path(popup_path, আকার(popup_path)))
    {
	printf(" - the \"Edit with Vim\" entry in the popup menu\n");
	printf("   which uses \"%s\"\n", popup_path);
	যদি (interactive)
	    printf("\nRemove it (y/n)? ");
	যদি (!interactive || confirm())
	{
	    remove_popup();
	    // Assume the "Open With" entry can be removed as well, don't
	    // bother the user with asking him again.
	    remove_openwith();
	}
    }
    অন্যথা যদি (openwith_gvim_path(popup_path, আকার(popup_path)))
    {
	printf(" - the Vim \"Open With...\" entry in the popup menu\n");
	printf("   which uses \"%s\"\n", popup_path);
	printf("\nRemove it (y/n)? ");
	যদি (confirm())
	    remove_openwith();
    }

    যদি (get_shell_folder_path(path, "desktop"))
    {
	printf("\n");
	জন্য (i = 0; i < ICON_গুনতি; ++i)
	{
	    sprintf(icon, "%s\\%s", path, icon_link_names[i]);
	    যদি (stat(icon, &st) == 0)
	    {
		printf(" - the \"%s\" icon on the desktop\n", icon_names[i]);
		++found;
	    }
	}
	যদি (found > 0)
	{
	    যদি (interactive)
		printf("\nRemove %s (y/n)? ", found > 1 ? "them" : "it");
	    যদি (!interactive || confirm())
		remove_icons();
	}
    }

    যদি (get_shell_folder_path(path, VIM_STARTMENU)
	    && stat(path, &st) == 0)
    {
	printf("\n - the \"%s\" entry in the Start Menu\n", VIM_STARTMENU);
	যদি (interactive)
	    printf("\nRemove it (y/n)? ");
	যদি (!interactive || confirm())
	    remove_start_menu();
    }

    printf("\n");
    found = remove_batfiles(0);
    যদি (found > 0)
    {
	যদি (interactive)
	    printf("\nRemove %s (y/n)? ", found > 1 ? "them" : "it");
	যদি (!interactive || confirm())
	    remove_batfiles(1);
    }

    fd = fopen("gvim.exe", "r");
    যদি (fd != ফাঁকা)
    {
	fclose(fd);
	printf("gvim.exe detected.  Attempting to unregister gvim with OLE\n");
	system("gvim.exe -silent -unregister");
    }

    delete_uninstall_চাবি();

    যদি (interactive)
    {
	printf("\nYou may now want to delete the Vim executables and runtime files.\n");
	printf("(They are still where you unpacked them.)\n");
    }

    যদি (interactive)
    {
	rewind(stdin);
	printf("\nPress Enter to exit...");
	(শূন্য)getchar();
    }
    অন্যথা
	sleep(3);

    ফেরত 0;
}
