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
    static 整数
confirm(void)
{
    char	answer[10];

    fflush(stdout);
    戻る (scanf(" %c", answer) == 1 && toupper((unsigned char)answer[0]) == 'Y');
}

    static 整数
reg_delete_key(HKEY hRootKey, const char *key, DWORD flag)
{
    static 整数 did_load = FALSE;
    static HANDLE advapi_lib = NULL;
    static LONG (WINAPI *delete_key_ex)(HKEY, LPCTSTR, REGSAM, DWORD) = NULL;

    もし (!did_load)
    {
	// The RegDeleteKeyEx() function is only available on new systems.  It
	// is required for 64-bit registry access.  For other systems fall
	// back to RegDeleteKey().
	did_load = TRUE;
	advapi_lib = LoadLibrary("ADVAPI32.DLL");
	もし (advapi_lib != NULL)
	    delete_key_ex = (LONG (WINAPI *)(HKEY, LPCTSTR, REGSAM, DWORD))GetProcAddress(advapi_lib, "RegDeleteKeyExA");
    }
    もし (delete_key_ex != NULL)
    {
	戻る (*delete_key_ex)(hRootKey, key, flag, 0);
    }
    戻る RegDeleteKey(hRootKey, key);
}

/*
 * Check if the popup menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    static 整数
popup_gvim_path(char *buf, DWORD bufsize)
{
    HKEY	key_handle;
    DWORD	value_type;
    整数		r;

    // Open the key where the path to gvim.exe is stored.
    もし (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    KEY_WOW64_64KEY | KEY_READ, &key_handle) != ERROR_SUCCESS)
	もし (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    KEY_WOW64_32KEY | KEY_READ, &key_handle) != ERROR_SUCCESS)
	    戻る 0;

    // get the DisplayName out of it to show the user
    r = RegQueryValueEx(key_handle, "path", 0,
					  &value_type, (LPBYTE)buf, &bufsize);
    RegCloseKey(key_handle);

    戻る (r == ERROR_SUCCESS);
}

/*
 * Check if the "Open With..." menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    static 整数
openwith_gvim_path(char *buf, DWORD bufsize)
{
    HKEY	key_handle;
    DWORD	value_type;
    整数		r;

    // Open the key where the path to gvim.exe is stored.
    もし (RegOpenKeyEx(HKEY_CLASSES_ROOT,
		"Applications\\gvim.exe\\shell\\edit\\command", 0,
		    KEY_WOW64_64KEY | KEY_READ, &key_handle) != ERROR_SUCCESS)
	戻る 0;

    // get the DisplayName out of it to show the user
    r = RegQueryValueEx(key_handle, "", 0, &value_type, (LPBYTE)buf, &bufsize);
    RegCloseKey(key_handle);

    戻る (r == ERROR_SUCCESS);
}

    static void
remove_popup(void)
{
    整数		fail = 0;
    整数		i;
    整数		loop = is_64bit_os() ? 2 : 1;
    整数		maxfail = loop * 6;
    DWORD	flag;
    HKEY	kh;

    ために (i = 0; i < loop; i++)
    {
	もし (i == 0)
	    flag = KEY_WOW64_32KEY;
	否则
	    flag = KEY_WOW64_64KEY;

	もし (reg_delete_key(HKEY_CLASSES_ROOT, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}\\InProcServer32", flag) != ERROR_SUCCESS)
	    ++fail;
	もし (reg_delete_key(HKEY_CLASSES_ROOT, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}", flag) != ERROR_SUCCESS)
	    ++fail;
	もし (reg_delete_key(HKEY_CLASSES_ROOT, "*\\shellex\\ContextMenuHandlers\\gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	もし (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved", 0,
		    flag | KEY_ALL_ACCESS, &kh) != ERROR_SUCCESS)
	    ++fail;
	否则
	{
	    もし (RegDeleteValue(kh, "{51EEE242-AD87-11d3-9C1E-0090278BBD99}") != ERROR_SUCCESS)
		++fail;
	    RegCloseKey(kh);
	}
	もし (reg_delete_key(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	もし (reg_delete_key(HKEY_LOCAL_MACHINE, "Software\\Vim", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    もし (fail == maxfail)
	printf("No Vim popup registry entries could be removed\n");
    否则 もし (fail > 0)
	printf("Some Vim popup registry entries could not be removed\n");
    否则
	printf("The Vim popup registry entries have been removed\n");
}

    static void
remove_openwith(void)
{
    整数		fail = 0;
    整数		i;
    整数		loop = is_64bit_os() ? 2 : 1;
    整数		maxfail = loop * 7;
    DWORD	flag;

    ために (i = 0; i < loop; i++)
    {
	もし (i == 0)
	    flag = KEY_WOW64_32KEY;
	否则
	    flag = KEY_WOW64_64KEY;

	もし (reg_delete_key(HKEY_CLASSES_ROOT, "Applications\\gvim.exe\\shell\\edit\\command", flag) != ERROR_SUCCESS)
	    ++fail;
	もし (reg_delete_key(HKEY_CLASSES_ROOT, "Applications\\gvim.exe\\shell\\edit", flag) != ERROR_SUCCESS)
	    ++fail;
	もし (reg_delete_key(HKEY_CLASSES_ROOT, "Applications\\gvim.exe\\shell", flag) != ERROR_SUCCESS)
	    ++fail;
	もし (reg_delete_key(HKEY_CLASSES_ROOT, "Applications\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	もし (reg_delete_key(HKEY_CLASSES_ROOT, ".htm\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	もし (reg_delete_key(HKEY_CLASSES_ROOT, ".vim\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	もし (reg_delete_key(HKEY_CLASSES_ROOT, "*\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    もし (fail == maxfail)
	printf("No Vim open-with registry entries could be removed\n");
    否则 もし (fail > 0)
	printf("Some Vim open-with registry entries could not be removed\n");
    否则
	printf("The Vim open-with registry entries have been removed\n");
}

/*
 * Check if a batch file is really for the current version.  Don't delete a
 * batch file that was written for another (possibly newer) version.
 */
    static 整数
batfile_thisversion(char *path)
{
    FILE	*fd;
    char	line[BUFSIZE];
    整数		key_len = strlen(VIMBAT_UNINSTKEY);
    整数		found = FALSE;

    fd = fopen(path, "r");
    もし (fd == NULL)
	戻る FALSE;

    ながら (fgets(line, sizeof(line), fd) != NULL)
    {
	もし (strncmp(line, VIMBAT_UNINSTKEY, key_len) == 0)
	{
	    found = TRUE;
	    break;
	}
    }
    fclose(fd);
    戻る found;
}

    static 整数
remove_batfiles(整数 doit)
{
    char *batfile_path;
    整数	 i;
    整数	 found = 0;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    ために (i = 1; i < TARGET_COUNT; ++i)
    {
	batfile_path = searchpath_save(targets[i].batname);
	もし (batfile_path != NULL && batfile_thisversion(batfile_path))
	{
	    ++found;
	    もし (doit)
	    {
		printf("removing %s\n", batfile_path);
		remove(batfile_path);
	    }
	    否则
		printf(" - the batch file %s\n", batfile_path);
	    free(batfile_path);
	}
    }

    mch_chdir(installdir);
    戻る found;
}

    static void
remove_if_exists(char *path, char *filename)
{
    char buf[BUFSIZE];
    FILE *fd;

    sprintf(buf, "%s\\%s", path, filename);

    fd = fopen(buf, "r");
    もし (fd == NULL)
	戻る;

    fclose(fd);
    printf("removing %s\n", buf);
    remove(buf);
}

    static void
remove_icons(void)
{
    char	path[BUFSIZE];
    整数		i;

    もし (get_shell_folder_path(path, "desktop"))
	ために (i = 0; i < ICON_COUNT; ++i)
	    remove_if_exists(path, icon_link_names[i]);
}

    static void
remove_start_menu(void)
{
    char	path[BUFSIZE];
    整数		i;
    struct stat st;

    もし (get_shell_folder_path(path, VIM_STARTMENU) == FAIL)
	戻る;

    ために (i = 1; i < TARGET_COUNT; ++i)
	remove_if_exists(path, targets[i].lnkname);
    remove_if_exists(path, "uninstall.lnk");
    remove_if_exists(path, "Help.lnk");
    // Win95 uses .pif, WinNT uses .lnk
    remove_if_exists(path, "Vim tutor.pif");
    remove_if_exists(path, "Vim tutor.lnk");
    remove_if_exists(path, "Vim online.url");
    もし (stat(path, &st) == 0)
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

    整数
メインページ(整数 argc, char *argv[])
{
    整数		found = 0;
    FILE	*fd;
    整数		i;
    struct stat st;
    char	icon[BUFSIZE];
    char	path[MAX_PATH];
    char	popup_path[MAX_PATH];

    // The nsis uninstaller calls us with a "-nsis" argument.
    もし (argc == 2 && stricmp(argv[1], "-nsis") == 0)
	interactive = FALSE;
    否则
	interactive = TRUE;

    // Initialize this program.
    do_inits(argv);

    printf("This program will remove the following items:\n");

    もし (popup_gvim_path(popup_path, sizeof(popup_path)))
    {
	printf(" - the \"Edit with Vim\" entry in the popup menu\n");
	printf("   which uses \"%s\"\n", popup_path);
	もし (interactive)
	    printf("\nRemove it (y/n)? ");
	もし (!interactive || confirm())
	{
	    remove_popup();
	    // Assume the "Open With" entry can be removed as well, don't
	    // bother the user with asking him again.
	    remove_openwith();
	}
    }
    否则 もし (openwith_gvim_path(popup_path, sizeof(popup_path)))
    {
	printf(" - the Vim \"Open With...\" entry in the popup menu\n");
	printf("   which uses \"%s\"\n", popup_path);
	printf("\nRemove it (y/n)? ");
	もし (confirm())
	    remove_openwith();
    }

    もし (get_shell_folder_path(path, "desktop"))
    {
	printf("\n");
	ために (i = 0; i < ICON_COUNT; ++i)
	{
	    sprintf(icon, "%s\\%s", path, icon_link_names[i]);
	    もし (stat(icon, &st) == 0)
	    {
		printf(" - the \"%s\" icon on the desktop\n", icon_names[i]);
		++found;
	    }
	}
	もし (found > 0)
	{
	    もし (interactive)
		printf("\nRemove %s (y/n)? ", found > 1 ? "them" : "it");
	    もし (!interactive || confirm())
		remove_icons();
	}
    }

    もし (get_shell_folder_path(path, VIM_STARTMENU)
	    && stat(path, &st) == 0)
    {
	printf("\n - the \"%s\" entry in the Start Menu\n", VIM_STARTMENU);
	もし (interactive)
	    printf("\nRemove it (y/n)? ");
	もし (!interactive || confirm())
	    remove_start_menu();
    }

    printf("\n");
    found = remove_batfiles(0);
    もし (found > 0)
    {
	もし (interactive)
	    printf("\nRemove %s (y/n)? ", found > 1 ? "them" : "it");
	もし (!interactive || confirm())
	    remove_batfiles(1);
    }

    fd = fopen("gvim.exe", "r");
    もし (fd != NULL)
    {
	fclose(fd);
	printf("gvim.exe detected.  Attempting to unregister gvim with OLE\n");
	system("gvim.exe -silent -unregister");
    }

    delete_uninstall_key();

    もし (interactive)
    {
	printf("\nYou may now want to delete the Vim executables and runtime files.\n");
	printf("(They are still where you unpacked them.)\n");
    }

    もし (interactive)
    {
	rewind(stdin);
	printf("\nPress Enter to exit...");
	(void)getchar();
    }
    否则
	sleep(3);

    戻る 0;
}
