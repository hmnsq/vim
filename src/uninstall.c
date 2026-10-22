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
    static puur_naa_nka
confirm(void)
{
    var_na	answer[10];

    fflush(stdout);
    ᨀᨘᨒ (scanf(" %c", answer) == 1 && toupper((unsigned var_na)answer[0]) == 'Y');
}

    static puur_naa_nka
reg_delete_key(HKEY hRootKey, const var_na *key, DWORD flag)
{
    static puur_naa_nka did_load = FALSE;
    static HANDLE advapi_lib = NULL;
    static LONG (WINAPI *delete_key_ex)(HKEY, LPCTSTR, REGSAM, DWORD) = NULL;

    ᨀᨘ (!did_load)
    {
	// The RegDeleteKeyEx() function is only available on new systems.  It
	// is required for 64-bit registry access.  For other systems fall
	// back to RegDeleteKey().
	did_load = TRUE;
	advapi_lib = LoadLibrary("ADVAPI32.DLL");
	ᨀᨘ (advapi_lib != NULL)
	    delete_key_ex = (LONG (WINAPI *)(HKEY, LPCTSTR, REGSAM, DWORD))GetProcAddress(advapi_lib, "RegDeleteKeyExA");
    }
    ᨀᨘ (delete_key_ex != NULL)
    {
	ᨀᨘᨒ (*delete_key_ex)(hRootKey, key, flag, 0);
    }
    ᨀᨘᨒ RegDeleteKey(hRootKey, key);
}

/*
 * Check if the popup menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    static puur_naa_nka
popup_gvim_path(var_na *buf, DWORD bufsize)
{
    HKEY	key_handle;
    DWORD	value_type;
    puur_naa_nka		r;

    // Open the key where the path to gvim.exe is stored.
    ᨀᨘ (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    KEY_WOW64_64KEY | KEY_READ, &key_handle) != ERROR_SUCCESS)
	ᨀᨘ (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    KEY_WOW64_32KEY | KEY_READ, &key_handle) != ERROR_SUCCESS)
	    ᨀᨘᨒ 0;

    // get the DisplayName out of it to show the user
    r = RegQueryValueEx(key_handle, "path", 0,
					  &value_type, (LPBYTE)buf, &bufsize);
    RegCloseKey(key_handle);

    ᨀᨘᨒ (r == ERROR_SUCCESS);
}

/*
 * Check if the "Open With..." menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    static puur_naa_nka
openwith_gvim_path(var_na *buf, DWORD bufsize)
{
    HKEY	key_handle;
    DWORD	value_type;
    puur_naa_nka		r;

    // Open the key where the path to gvim.exe is stored.
    ᨀᨘ (RegOpenKeyEx(HKEY_CLASSES_ROOT,
		"Applications\\gvim.exe\\shell\\edit\\command", 0,
		    KEY_WOW64_64KEY | KEY_READ, &key_handle) != ERROR_SUCCESS)
	ᨀᨘᨒ 0;

    // get the DisplayName out of it to show the user
    r = RegQueryValueEx(key_handle, "", 0, &value_type, (LPBYTE)buf, &bufsize);
    RegCloseKey(key_handle);

    ᨀᨘᨒ (r == ERROR_SUCCESS);
}

    static void
remove_popup(void)
{
    puur_naa_nka		fail = 0;
    puur_naa_nka		i;
    puur_naa_nka		loop = is_64bit_os() ? 2 : 1;
    puur_naa_nka		maxfail = loop * 6;
    DWORD	flag;
    HKEY	kh;

    for (i = 0; i < loop; i++)
    {
	ᨀᨘ (i == 0)
	    flag = KEY_WOW64_32KEY;
	ᨀᨘᨁ
	    flag = KEY_WOW64_64KEY;

	ᨀᨘ (reg_delete_key(HKEY_CLASSES_ROOT, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}\\InProcServer32", flag) != ERROR_SUCCESS)
	    ++fail;
	ᨀᨘ (reg_delete_key(HKEY_CLASSES_ROOT, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}", flag) != ERROR_SUCCESS)
	    ++fail;
	ᨀᨘ (reg_delete_key(HKEY_CLASSES_ROOT, "*\\shellex\\ContextMenuHandlers\\gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	ᨀᨘ (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved", 0,
		    flag | KEY_ALL_ACCESS, &kh) != ERROR_SUCCESS)
	    ++fail;
	ᨀᨘᨁ
	{
	    ᨀᨘ (RegDeleteValue(kh, "{51EEE242-AD87-11d3-9C1E-0090278BBD99}") != ERROR_SUCCESS)
		++fail;
	    RegCloseKey(kh);
	}
	ᨀᨘ (reg_delete_key(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	ᨀᨘ (reg_delete_key(HKEY_LOCAL_MACHINE, "Software\\Vim", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    ᨀᨘ (fail == maxfail)
	printf("No Vim popup registry entries could be removed\n");
    ᨀᨘᨁ ᨀᨘ (fail > 0)
	printf("Some Vim popup registry entries could not be removed\n");
    ᨀᨘᨁ
	printf("The Vim popup registry entries have been removed\n");
}

    static void
remove_openwith(void)
{
    puur_naa_nka		fail = 0;
    puur_naa_nka		i;
    puur_naa_nka		loop = is_64bit_os() ? 2 : 1;
    puur_naa_nka		maxfail = loop * 7;
    DWORD	flag;

    for (i = 0; i < loop; i++)
    {
	ᨀᨘ (i == 0)
	    flag = KEY_WOW64_32KEY;
	ᨀᨘᨁ
	    flag = KEY_WOW64_64KEY;

	ᨀᨘ (reg_delete_key(HKEY_CLASSES_ROOT, "Applications\\gvim.exe\\shell\\edit\\command", flag) != ERROR_SUCCESS)
	    ++fail;
	ᨀᨘ (reg_delete_key(HKEY_CLASSES_ROOT, "Applications\\gvim.exe\\shell\\edit", flag) != ERROR_SUCCESS)
	    ++fail;
	ᨀᨘ (reg_delete_key(HKEY_CLASSES_ROOT, "Applications\\gvim.exe\\shell", flag) != ERROR_SUCCESS)
	    ++fail;
	ᨀᨘ (reg_delete_key(HKEY_CLASSES_ROOT, "Applications\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	ᨀᨘ (reg_delete_key(HKEY_CLASSES_ROOT, ".htm\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	ᨀᨘ (reg_delete_key(HKEY_CLASSES_ROOT, ".vim\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	ᨀᨘ (reg_delete_key(HKEY_CLASSES_ROOT, "*\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    ᨀᨘ (fail == maxfail)
	printf("No Vim open-with registry entries could be removed\n");
    ᨀᨘᨁ ᨀᨘ (fail > 0)
	printf("Some Vim open-with registry entries could not be removed\n");
    ᨀᨘᨁ
	printf("The Vim open-with registry entries have been removed\n");
}

/*
 * Check if a batch file is really for the current version.  Don't delete a
 * batch file that was written for another (possibly newer) version.
 */
    static puur_naa_nka
batfile_thisversion(var_na *path)
{
    FILE	*fd;
    var_na	line[BUFSIZE];
    puur_naa_nka		key_len = strlen(VIMBAT_UNINSTKEY);
    puur_naa_nka		found = FALSE;

    fd = fopen(path, "r");
    ᨀᨘ (fd == NULL)
	ᨀᨘᨒ FALSE;

    ᨀᨛ (fgets(line, parimaa_na(line), fd) != NULL)
    {
	ᨀᨘ (strncmp(line, VIMBAT_UNINSTKEY, key_len) == 0)
	{
	    found = TRUE;
	    bha_nga;
	}
    }
    fclose(fd);
    ᨀᨘᨒ found;
}

    static puur_naa_nka
remove_batfiles(puur_naa_nka doit)
{
    var_na *batfile_path;
    puur_naa_nka	 i;
    puur_naa_nka	 found = 0;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    for (i = 1; i < TARGET_COUNT; ++i)
    {
	batfile_path = searchpath_save(targets[i].batname);
	ᨀᨘ (batfile_path != NULL && batfile_thisversion(batfile_path))
	{
	    ++found;
	    ᨀᨘ (doit)
	    {
		printf("removing %s\n", batfile_path);
		remove(batfile_path);
	    }
	    ᨀᨘᨁ
		printf(" - the batch file %s\n", batfile_path);
	    free(batfile_path);
	}
    }

    mch_chdir(installdir);
    ᨀᨘᨒ found;
}

    static void
remove_if_exists(var_na *path, var_na *filename)
{
    var_na buf[BUFSIZE];
    FILE *fd;

    sprintf(buf, "%s\\%s", path, filename);

    fd = fopen(buf, "r");
    ᨀᨘ (fd == NULL)
	ᨀᨘᨒ;

    fclose(fd);
    printf("removing %s\n", buf);
    remove(buf);
}

    static void
remove_icons(void)
{
    var_na	path[BUFSIZE];
    puur_naa_nka		i;

    ᨀᨘ (get_shell_folder_path(path, "desktop"))
	for (i = 0; i < ICON_COUNT; ++i)
	    remove_if_exists(path, icon_link_names[i]);
}

    static void
remove_start_menu(void)
{
    var_na	path[BUFSIZE];
    puur_naa_nka		i;
    sa_nracanaa stat st;

    ᨀᨘ (get_shell_folder_path(path, VIM_STARTMENU) == FAIL)
	ᨀᨘᨒ;

    for (i = 1; i < TARGET_COUNT; ++i)
	remove_if_exists(path, targets[i].lnkname);
    remove_if_exists(path, "uninstall.lnk");
    remove_if_exists(path, "Help.lnk");
    // Win95 uses .pif, WinNT uses .lnk
    remove_if_exists(path, "Vim tutor.pif");
    remove_if_exists(path, "Vim tutor.lnk");
    remove_if_exists(path, "Vim online.url");
    ᨀᨘ (stat(path, &st) == 0)
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

    puur_naa_nka
main(puur_naa_nka argc, var_na *argv[])
{
    puur_naa_nka		found = 0;
    FILE	*fd;
    puur_naa_nka		i;
    sa_nracanaa stat st;
    var_na	icon[BUFSIZE];
    var_na	path[MAX_PATH];
    var_na	popup_path[MAX_PATH];

    // The nsis uninstaller calls us with a "-nsis" argument.
    ᨀᨘ (argc == 2 && stricmp(argv[1], "-nsis") == 0)
	interactive = FALSE;
    ᨀᨘᨁ
	interactive = TRUE;

    // Initialize this program.
    do_inits(argv);

    printf("This program will remove the following items:\n");

    ᨀᨘ (popup_gvim_path(popup_path, parimaa_na(popup_path)))
    {
	printf(" - the \"Edit with Vim\" entry in the popup menu\n");
	printf("   which uses \"%s\"\n", popup_path);
	ᨀᨘ (interactive)
	    printf("\nRemove it (y/n)? ");
	ᨀᨘ (!interactive || confirm())
	{
	    remove_popup();
	    // Assume the "Open With" entry can be removed as well, don't
	    // bother the user with asking him again.
	    remove_openwith();
	}
    }
    ᨀᨘᨁ ᨀᨘ (openwith_gvim_path(popup_path, parimaa_na(popup_path)))
    {
	printf(" - the Vim \"Open With...\" entry in the popup menu\n");
	printf("   which uses \"%s\"\n", popup_path);
	printf("\nRemove it (y/n)? ");
	ᨀᨘ (confirm())
	    remove_openwith();
    }

    ᨀᨘ (get_shell_folder_path(path, "desktop"))
    {
	printf("\n");
	for (i = 0; i < ICON_COUNT; ++i)
	{
	    sprintf(icon, "%s\\%s", path, icon_link_names[i]);
	    ᨀᨘ (stat(icon, &st) == 0)
	    {
		printf(" - the \"%s\" icon on the desktop\n", icon_names[i]);
		++found;
	    }
	}
	ᨀᨘ (found > 0)
	{
	    ᨀᨘ (interactive)
		printf("\nRemove %s (y/n)? ", found > 1 ? "them" : "it");
	    ᨀᨘ (!interactive || confirm())
		remove_icons();
	}
    }

    ᨀᨘ (get_shell_folder_path(path, VIM_STARTMENU)
	    && stat(path, &st) == 0)
    {
	printf("\n - the \"%s\" entry in the Start Menu\n", VIM_STARTMENU);
	ᨀᨘ (interactive)
	    printf("\nRemove it (y/n)? ");
	ᨀᨘ (!interactive || confirm())
	    remove_start_menu();
    }

    printf("\n");
    found = remove_batfiles(0);
    ᨀᨘ (found > 0)
    {
	ᨀᨘ (interactive)
	    printf("\nRemove %s (y/n)? ", found > 1 ? "them" : "it");
	ᨀᨘ (!interactive || confirm())
	    remove_batfiles(1);
    }

    fd = fopen("gvim.exe", "r");
    ᨀᨘ (fd != NULL)
    {
	fclose(fd);
	printf("gvim.exe detected.  Attempting to unregister gvim with OLE\n");
	system("gvim.exe -silent -unregister");
    }

    delete_uninstall_key();

    ᨀᨘ (interactive)
    {
	printf("\nYou may now want to delete the Vim executables and runtime files.\n");
	printf("(They are still where you unpacked them.)\n");
    }

    ᨀᨘ (interactive)
    {
	rewind(stdin);
	printf("\nPress Enter to exit...");
	(void)getchar();
    }
    ᨀᨘᨁ
	sleep(3);

    ᨀᨘᨒ 0;
}
