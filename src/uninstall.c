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
#include "dosinst.h"

/*
 * Return TRUE if the user types a 'y' or 'Y', FALSE otherwise.
 */
    static ចំនួនគត់
confirm(void)
{
    var_na	answer[10];

    fflush(stdout);
    ត្រឡប់ (scanf(" %c", answer) == 1 && toupper((unsigned var_na)answer[0]) == 'Y');
}

    static ចំនួនគត់
reg_delete_key(HKEY hRootKey, const var_na *key, DWORD flag)
{
    static ចំនួនគត់ did_load = FALSE;
    static HANDLE advapi_lib = NULL;
    static LONG (WINAPI *delete_key_ex)(HKEY, LPCTSTR, REGSAM, DWORD) = NULL;

    ប្រសិនបើ (!did_load)
    {
	// The RegDeleteKeyEx() function is only available on new systems.  It
	// is required for 64-bit registry access.  For other systems fall
	// back to RegDeleteKey().
	did_load = TRUE;
	advapi_lib = LoadLibrary("ADVAPI32.DLL");
	ប្រសិនបើ (advapi_lib != NULL)
	    delete_key_ex = (LONG (WINAPI *)(HKEY, LPCTSTR, REGSAM, DWORD))GetProcAddress(advapi_lib, "RegDeleteKeyExA");
    }
    ប្រសិនបើ (delete_key_ex != NULL)
    {
	ត្រឡប់ (*delete_key_ex)(hRootKey, key, flag, 0);
    }
    ត្រឡប់ RegDeleteKey(hRootKey, key);
}

/*
 * Check if the popup menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    static ចំនួនគត់
popup_gvim_path(var_na *buf, DWORD bufsize)
{
    HKEY	key_handle;
    DWORD	value_type;
    ចំនួនគត់		r;

    // Open the key where the path to gvim.exe is stored.
    ប្រសិនបើ (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    KEY_WOW64_64KEY | KEY_READ, &key_handle) != ERROR_SUCCESS)
	ប្រសិនបើ (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    KEY_WOW64_32KEY | KEY_READ, &key_handle) != ERROR_SUCCESS)
	    ត្រឡប់ 0;

    // get the DisplayName out of it to show the user
    r = RegQueryValueEx(key_handle, "path", 0,
					  &value_type, (LPBYTE)buf, &bufsize);
    RegCloseKey(key_handle);

    ត្រឡប់ (r == ERROR_SUCCESS);
}

/*
 * Check if the "Open With..." menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    static ចំនួនគត់
openwith_gvim_path(var_na *buf, DWORD bufsize)
{
    HKEY	key_handle;
    DWORD	value_type;
    ចំនួនគត់		r;

    // Open the key where the path to gvim.exe is stored.
    ប្រសិនបើ (RegOpenKeyEx(HKEY_CLASSES_ROOT,
		"Applications\\gvim.exe\\shell\\edit\\command", 0,
		    KEY_WOW64_64KEY | KEY_READ, &key_handle) != ERROR_SUCCESS)
	ត្រឡប់ 0;

    // get the DisplayName out of it to show the user
    r = RegQueryValueEx(key_handle, "", 0, &value_type, (LPBYTE)buf, &bufsize);
    RegCloseKey(key_handle);

    ត្រឡប់ (r == ERROR_SUCCESS);
}

    static void
remove_popup(void)
{
    ចំនួនគត់		fail = 0;
    ចំនួនគត់		i;
    ចំនួនគត់		loop = is_64bit_os() ? 2 : 1;
    ចំនួនគត់		maxfail = loop * 6;
    DWORD	flag;
    HKEY	kh;

    សម្រាប់ (i = 0; i < loop; i++)
    {
	ប្រសិនបើ (i == 0)
	    flag = KEY_WOW64_32KEY;
	បើមិនដូច្នេះ
	    flag = KEY_WOW64_64KEY;

	ប្រសិនបើ (reg_delete_key(HKEY_CLASSES_ROOT, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}\\InProcServer32", flag) != ERROR_SUCCESS)
	    ++fail;
	ប្រសិនបើ (reg_delete_key(HKEY_CLASSES_ROOT, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}", flag) != ERROR_SUCCESS)
	    ++fail;
	ប្រសិនបើ (reg_delete_key(HKEY_CLASSES_ROOT, "*\\shellex\\ContextMenuHandlers\\gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	ប្រសិនបើ (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved", 0,
		    flag | KEY_ALL_ACCESS, &kh) != ERROR_SUCCESS)
	    ++fail;
	បើមិនដូច្នេះ
	{
	    ប្រសិនបើ (RegDeleteValue(kh, "{51EEE242-AD87-11d3-9C1E-0090278BBD99}") != ERROR_SUCCESS)
		++fail;
	    RegCloseKey(kh);
	}
	ប្រសិនបើ (reg_delete_key(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	ប្រសិនបើ (reg_delete_key(HKEY_LOCAL_MACHINE, "Software\\Vim", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    ប្រសិនបើ (fail == maxfail)
	printf("No Vim popup registry entries could be removed\n");
    បើមិនដូច្នេះ ប្រសិនបើ (fail > 0)
	printf("Some Vim popup registry entries could not be removed\n");
    បើមិនដូច្នេះ
	printf("The Vim popup registry entries have been removed\n");
}

    static void
remove_openwith(void)
{
    ចំនួនគត់		fail = 0;
    ចំនួនគត់		i;
    ចំនួនគត់		loop = is_64bit_os() ? 2 : 1;
    ចំនួនគត់		maxfail = loop * 7;
    DWORD	flag;

    សម្រាប់ (i = 0; i < loop; i++)
    {
	ប្រសិនបើ (i == 0)
	    flag = KEY_WOW64_32KEY;
	បើមិនដូច្នេះ
	    flag = KEY_WOW64_64KEY;

	ប្រសិនបើ (reg_delete_key(HKEY_CLASSES_ROOT, "Applications\\gvim.exe\\shell\\edit\\command", flag) != ERROR_SUCCESS)
	    ++fail;
	ប្រសិនបើ (reg_delete_key(HKEY_CLASSES_ROOT, "Applications\\gvim.exe\\shell\\edit", flag) != ERROR_SUCCESS)
	    ++fail;
	ប្រសិនបើ (reg_delete_key(HKEY_CLASSES_ROOT, "Applications\\gvim.exe\\shell", flag) != ERROR_SUCCESS)
	    ++fail;
	ប្រសិនបើ (reg_delete_key(HKEY_CLASSES_ROOT, "Applications\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	ប្រសិនបើ (reg_delete_key(HKEY_CLASSES_ROOT, ".htm\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	ប្រសិនបើ (reg_delete_key(HKEY_CLASSES_ROOT, ".vim\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	ប្រសិនបើ (reg_delete_key(HKEY_CLASSES_ROOT, "*\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    ប្រសិនបើ (fail == maxfail)
	printf("No Vim open-with registry entries could be removed\n");
    បើមិនដូច្នេះ ប្រសិនបើ (fail > 0)
	printf("Some Vim open-with registry entries could not be removed\n");
    បើមិនដូច្នេះ
	printf("The Vim open-with registry entries have been removed\n");
}

/*
 * Check if a batch file is really for the current version.  Don't delete a
 * batch file that was written for another (possibly newer) version.
 */
    static ចំនួនគត់
batfile_thisversion(var_na *path)
{
    FILE	*fd;
    var_na	line[BUFSIZE];
    ចំនួនគត់		key_len = strlen(VIMBAT_UNINSTKEY);
    ចំនួនគត់		found = FALSE;

    fd = fopen(path, "r");
    ប្រសិនបើ (fd == NULL)
	ត្រឡប់ FALSE;

    ខណៈពេល (fgets(line, parimaa_na(line), fd) != NULL)
    {
	ប្រសិនបើ (strncmp(line, VIMBAT_UNINSTKEY, key_len) == 0)
	{
	    found = TRUE;
	    bha_nga;
	}
    }
    fclose(fd);
    ត្រឡប់ found;
}

    static ចំនួនគត់
remove_batfiles(ចំនួនគត់ doit)
{
    var_na *batfile_path;
    ចំនួនគត់	 i;
    ចំនួនគត់	 found = 0;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    សម្រាប់ (i = 1; i < TARGET_COUNT; ++i)
    {
	batfile_path = searchpath_save(targets[i].batname);
	ប្រសិនបើ (batfile_path != NULL && batfile_thisversion(batfile_path))
	{
	    ++found;
	    ប្រសិនបើ (doit)
	    {
		printf("removing %s\n", batfile_path);
		remove(batfile_path);
	    }
	    បើមិនដូច្នេះ
		printf(" - the batch file %s\n", batfile_path);
	    free(batfile_path);
	}
    }

    mch_chdir(installdir);
    ត្រឡប់ found;
}

    static void
remove_if_exists(var_na *path, var_na *filename)
{
    var_na buf[BUFSIZE];
    FILE *fd;

    sprintf(buf, "%s\\%s", path, filename);

    fd = fopen(buf, "r");
    ប្រសិនបើ (fd == NULL)
	ត្រឡប់;

    fclose(fd);
    printf("removing %s\n", buf);
    remove(buf);
}

    static void
remove_icons(void)
{
    var_na	path[BUFSIZE];
    ចំនួនគត់		i;

    ប្រសិនបើ (get_shell_folder_path(path, "desktop"))
	សម្រាប់ (i = 0; i < ICON_COUNT; ++i)
	    remove_if_exists(path, icon_link_names[i]);
}

    static void
remove_start_menu(void)
{
    var_na	path[BUFSIZE];
    ចំនួនគត់		i;
    sa_nracanaa stat st;

    ប្រសិនបើ (get_shell_folder_path(path, VIM_STARTMENU) == FAIL)
	ត្រឡប់;

    សម្រាប់ (i = 1; i < TARGET_COUNT; ++i)
	remove_if_exists(path, targets[i].lnkname);
    remove_if_exists(path, "uninstall.lnk");
    remove_if_exists(path, "Help.lnk");
    // Win95 uses .pif, WinNT uses .lnk
    remove_if_exists(path, "Vim tutor.pif");
    remove_if_exists(path, "Vim tutor.lnk");
    remove_if_exists(path, "Vim online.url");
    ប្រសិនបើ (stat(path, &st) == 0)
    {
	printf("removing %s\n", path);
	rmdir(path);
    }
}

    static void
delete_uninstall_key(void)
{
    reg_delete_key(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Vim " VIM_VERSION_SHORT, KEY_WOW64_64KEY);
}

    ចំនួនគត់
main(ចំនួនគត់ argc, var_na *argv[])
{
    ចំនួនគត់		found = 0;
    FILE	*fd;
    ចំនួនគត់		i;
    sa_nracanaa stat st;
    var_na	icon[BUFSIZE];
    var_na	path[MAX_PATH];
    var_na	popup_path[MAX_PATH];

    // The nsis uninstaller calls us with a "-nsis" argument.
    ប្រសិនបើ (argc == 2 && stricmp(argv[1], "-nsis") == 0)
	interactive = FALSE;
    បើមិនដូច្នេះ
	interactive = TRUE;

    // Initialize this program.
    do_inits(argv);

    printf("This program will remove the following items:\n");

    ប្រសិនបើ (popup_gvim_path(popup_path, parimaa_na(popup_path)))
    {
	printf(" - the \"Edit with Vim\" entry in the popup menu\n");
	printf("   which uses \"%s\"\n", popup_path);
	ប្រសិនបើ (interactive)
	    printf("\nRemove it (y/n)? ");
	ប្រសិនបើ (!interactive || confirm())
	{
	    remove_popup();
	    // Assume the "Open With" entry can be removed as well, don't
	    // bother the user with asking him again.
	    remove_openwith();
	}
    }
    បើមិនដូច្នេះ ប្រសិនបើ (openwith_gvim_path(popup_path, parimaa_na(popup_path)))
    {
	printf(" - the Vim \"Open With...\" entry in the popup menu\n");
	printf("   which uses \"%s\"\n", popup_path);
	printf("\nRemove it (y/n)? ");
	ប្រសិនបើ (confirm())
	    remove_openwith();
    }

    ប្រសិនបើ (get_shell_folder_path(path, "desktop"))
    {
	printf("\n");
	សម្រាប់ (i = 0; i < ICON_COUNT; ++i)
	{
	    sprintf(icon, "%s\\%s", path, icon_link_names[i]);
	    ប្រសិនបើ (stat(icon, &st) == 0)
	    {
		printf(" - the \"%s\" icon on the desktop\n", icon_names[i]);
		++found;
	    }
	}
	ប្រសិនបើ (found > 0)
	{
	    ប្រសិនបើ (interactive)
		printf("\nRemove %s (y/n)? ", found > 1 ? "them" : "it");
	    ប្រសិនបើ (!interactive || confirm())
		remove_icons();
	}
    }

    ប្រសិនបើ (get_shell_folder_path(path, VIM_STARTMENU)
	    && stat(path, &st) == 0)
    {
	printf("\n - the \"%s\" entry in the Start Menu\n", VIM_STARTMENU);
	ប្រសិនបើ (interactive)
	    printf("\nRemove it (y/n)? ");
	ប្រសិនបើ (!interactive || confirm())
	    remove_start_menu();
    }

    printf("\n");
    found = remove_batfiles(0);
    ប្រសិនបើ (found > 0)
    {
	ប្រសិនបើ (interactive)
	    printf("\nRemove %s (y/n)? ", found > 1 ? "them" : "it");
	ប្រសិនបើ (!interactive || confirm())
	    remove_batfiles(1);
    }

    fd = fopen("gvim.exe", "r");
    ប្រសិនបើ (fd != NULL)
    {
	fclose(fd);
	printf("gvim.exe detected.  Attempting to unregister gvim with OLE\n");
	system("gvim.exe -silent -unregister");
    }

    delete_uninstall_key();

    ប្រសិនបើ (interactive)
    {
	printf("\nYou may now want to delete the Vim executables and runtime files.\n");
	printf("(They are still where you unpacked them.)\n");
    }

    ប្រសិនបើ (interactive)
    {
	rewind(stdin);
	printf("\nPress Enter to exit...");
	(void)getchar();
    }
    បើមិនដូច្នេះ
	sleep(3);

    ត្រឡប់ 0;
}
