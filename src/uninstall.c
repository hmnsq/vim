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
    static ціле
confirm(void)
{
    char	answer[10];

    fflush(stdout);
    повернути (scanf(" %c", answer) == 1 && toupper((unsigned char)answer[0]) == 'Y');
}

    static ціле
reg_delete_key(HKEY hRootKey, const char *key, DWORD flag)
{
    static ціле did_load = FALSE;
    static HANDLE advapi_lib = NULL;
    static LONG (WINAPI *delete_key_ex)(HKEY, LPCTSTR, REGSAM, DWORD) = NULL;

    якщо (!did_load)
    {
	// The RegDeleteKeyEx() function is only available on new systems.  It
	// is required for 64-bit registry access.  For other systems fall
	// back to RegDeleteKey().
	did_load = TRUE;
	advapi_lib = LoadLibrary("ADVAPI32.DLL");
	якщо (advapi_lib != NULL)
	    delete_key_ex = (LONG (WINAPI *)(HKEY, LPCTSTR, REGSAM, DWORD))GetProcAddress(advapi_lib, "RegDeleteKeyExA");
    }
    якщо (delete_key_ex != NULL)
    {
	повернути (*delete_key_ex)(hRootKey, key, flag, 0);
    }
    повернути RegDeleteKey(hRootKey, key);
}

/*
 * Check if the popup menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    static ціле
popup_gvim_path(char *buf, DWORD bufsize)
{
    HKEY	key_handle;
    DWORD	value_type;
    ціле		r;

    // Open the key where the path to gvim.exe is stored.
    якщо (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    KEY_WOW64_64KEY | KEY_READ, &key_handle) != ERROR_SUCCESS)
	якщо (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    KEY_WOW64_32KEY | KEY_READ, &key_handle) != ERROR_SUCCESS)
	    повернути 0;

    // get the DisplayName out of it to show the user
    r = RegQueryValueEx(key_handle, "path", 0,
					  &value_type, (LPBYTE)buf, &bufsize);
    RegCloseKey(key_handle);

    повернути (r == ERROR_SUCCESS);
}

/*
 * Check if the "Open With..." menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    static ціле
openwith_gvim_path(char *buf, DWORD bufsize)
{
    HKEY	key_handle;
    DWORD	value_type;
    ціле		r;

    // Open the key where the path to gvim.exe is stored.
    якщо (RegOpenKeyEx(HKEY_CLASSES_ROOT,
		"Applications\\gvim.exe\\shell\\edit\\command", 0,
		    KEY_WOW64_64KEY | KEY_READ, &key_handle) != ERROR_SUCCESS)
	повернути 0;

    // get the DisplayName out of it to show the user
    r = RegQueryValueEx(key_handle, "", 0, &value_type, (LPBYTE)buf, &bufsize);
    RegCloseKey(key_handle);

    повернути (r == ERROR_SUCCESS);
}

    static void
remove_popup(void)
{
    ціле		fail = 0;
    ціле		i;
    ціле		loop = is_64bit_os() ? 2 : 1;
    ціле		maxfail = loop * 6;
    DWORD	flag;
    HKEY	kh;

    для (i = 0; i < loop; i++)
    {
	якщо (i == 0)
	    flag = KEY_WOW64_32KEY;
	інакше
	    flag = KEY_WOW64_64KEY;

	якщо (reg_delete_key(HKEY_CLASSES_ROOT, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}\\InProcServer32", flag) != ERROR_SUCCESS)
	    ++fail;
	якщо (reg_delete_key(HKEY_CLASSES_ROOT, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}", flag) != ERROR_SUCCESS)
	    ++fail;
	якщо (reg_delete_key(HKEY_CLASSES_ROOT, "*\\shellex\\ContextMenuHandlers\\gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	якщо (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved", 0,
		    flag | KEY_ALL_ACCESS, &kh) != ERROR_SUCCESS)
	    ++fail;
	інакше
	{
	    якщо (RegDeleteValue(kh, "{51EEE242-AD87-11d3-9C1E-0090278BBD99}") != ERROR_SUCCESS)
		++fail;
	    RegCloseKey(kh);
	}
	якщо (reg_delete_key(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	якщо (reg_delete_key(HKEY_LOCAL_MACHINE, "Software\\Vim", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    якщо (fail == maxfail)
	printf("No Vim popup registry entries could be removed\n");
    інакше якщо (fail > 0)
	printf("Some Vim popup registry entries could not be removed\n");
    інакше
	printf("The Vim popup registry entries have been removed\n");
}

    static void
remove_openwith(void)
{
    ціле		fail = 0;
    ціле		i;
    ціле		loop = is_64bit_os() ? 2 : 1;
    ціле		maxfail = loop * 7;
    DWORD	flag;

    для (i = 0; i < loop; i++)
    {
	якщо (i == 0)
	    flag = KEY_WOW64_32KEY;
	інакше
	    flag = KEY_WOW64_64KEY;

	якщо (reg_delete_key(HKEY_CLASSES_ROOT, "Applications\\gvim.exe\\shell\\edit\\command", flag) != ERROR_SUCCESS)
	    ++fail;
	якщо (reg_delete_key(HKEY_CLASSES_ROOT, "Applications\\gvim.exe\\shell\\edit", flag) != ERROR_SUCCESS)
	    ++fail;
	якщо (reg_delete_key(HKEY_CLASSES_ROOT, "Applications\\gvim.exe\\shell", flag) != ERROR_SUCCESS)
	    ++fail;
	якщо (reg_delete_key(HKEY_CLASSES_ROOT, "Applications\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	якщо (reg_delete_key(HKEY_CLASSES_ROOT, ".htm\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	якщо (reg_delete_key(HKEY_CLASSES_ROOT, ".vim\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	якщо (reg_delete_key(HKEY_CLASSES_ROOT, "*\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    якщо (fail == maxfail)
	printf("No Vim open-with registry entries could be removed\n");
    інакше якщо (fail > 0)
	printf("Some Vim open-with registry entries could not be removed\n");
    інакше
	printf("The Vim open-with registry entries have been removed\n");
}

/*
 * Check if a batch file is really for the current version.  Don't delete a
 * batch file that was written for another (possibly newer) version.
 */
    static ціле
batfile_thisversion(char *path)
{
    FILE	*fd;
    char	line[BUFSIZE];
    ціле		key_len = strlen(VIMBAT_UNINSTKEY);
    ціле		found = FALSE;

    fd = fopen(path, "r");
    якщо (fd == NULL)
	повернути FALSE;

    поки (fgets(line, sizeof(line), fd) != NULL)
    {
	якщо (strncmp(line, VIMBAT_UNINSTKEY, key_len) == 0)
	{
	    found = TRUE;
	    break;
	}
    }
    fclose(fd);
    повернути found;
}

    static ціле
remove_batfiles(ціле doit)
{
    char *batfile_path;
    ціле	 i;
    ціле	 found = 0;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    для (i = 1; i < TARGET_COUNT; ++i)
    {
	batfile_path = searchpath_save(targets[i].batname);
	якщо (batfile_path != NULL && batfile_thisversion(batfile_path))
	{
	    ++found;
	    якщо (doit)
	    {
		printf("removing %s\n", batfile_path);
		remove(batfile_path);
	    }
	    інакше
		printf(" - the batch file %s\n", batfile_path);
	    free(batfile_path);
	}
    }

    mch_chdir(installdir);
    повернути found;
}

    static void
remove_if_exists(char *path, char *filename)
{
    char buf[BUFSIZE];
    FILE *fd;

    sprintf(buf, "%s\\%s", path, filename);

    fd = fopen(buf, "r");
    якщо (fd == NULL)
	повернути;

    fclose(fd);
    printf("removing %s\n", buf);
    remove(buf);
}

    static void
remove_icons(void)
{
    char	path[BUFSIZE];
    ціле		i;

    якщо (get_shell_folder_path(path, "desktop"))
	для (i = 0; i < ICON_COUNT; ++i)
	    remove_if_exists(path, icon_link_names[i]);
}

    static void
remove_start_menu(void)
{
    char	path[BUFSIZE];
    ціле		i;
    struct stat st;

    якщо (get_shell_folder_path(path, VIM_STARTMENU) == FAIL)
	повернути;

    для (i = 1; i < TARGET_COUNT; ++i)
	remove_if_exists(path, targets[i].lnkname);
    remove_if_exists(path, "uninstall.lnk");
    remove_if_exists(path, "Help.lnk");
    // Win95 uses .pif, WinNT uses .lnk
    remove_if_exists(path, "Vim tutor.pif");
    remove_if_exists(path, "Vim tutor.lnk");
    remove_if_exists(path, "Vim online.url");
    якщо (stat(path, &st) == 0)
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

    ціле
main(ціле argc, char *argv[])
{
    ціле		found = 0;
    FILE	*fd;
    ціле		i;
    struct stat st;
    char	icon[BUFSIZE];
    char	path[MAX_PATH];
    char	popup_path[MAX_PATH];

    // The nsis uninstaller calls us with a "-nsis" argument.
    якщо (argc == 2 && stricmp(argv[1], "-nsis") == 0)
	interactive = FALSE;
    інакше
	interactive = TRUE;

    // Initialize this program.
    do_inits(argv);

    printf("This program will remove the following items:\n");

    якщо (popup_gvim_path(popup_path, sizeof(popup_path)))
    {
	printf(" - the \"Edit with Vim\" entry in the popup menu\n");
	printf("   which uses \"%s\"\n", popup_path);
	якщо (interactive)
	    printf("\nRemove it (y/n)? ");
	якщо (!interactive || confirm())
	{
	    remove_popup();
	    // Assume the "Open With" entry can be removed as well, don't
	    // bother the user with asking him again.
	    remove_openwith();
	}
    }
    інакше якщо (openwith_gvim_path(popup_path, sizeof(popup_path)))
    {
	printf(" - the Vim \"Open With...\" entry in the popup menu\n");
	printf("   which uses \"%s\"\n", popup_path);
	printf("\nRemove it (y/n)? ");
	якщо (confirm())
	    remove_openwith();
    }

    якщо (get_shell_folder_path(path, "desktop"))
    {
	printf("\n");
	для (i = 0; i < ICON_COUNT; ++i)
	{
	    sprintf(icon, "%s\\%s", path, icon_link_names[i]);
	    якщо (stat(icon, &st) == 0)
	    {
		printf(" - the \"%s\" icon on the desktop\n", icon_names[i]);
		++found;
	    }
	}
	якщо (found > 0)
	{
	    якщо (interactive)
		printf("\nRemove %s (y/n)? ", found > 1 ? "them" : "it");
	    якщо (!interactive || confirm())
		remove_icons();
	}
    }

    якщо (get_shell_folder_path(path, VIM_STARTMENU)
	    && stat(path, &st) == 0)
    {
	printf("\n - the \"%s\" entry in the Start Menu\n", VIM_STARTMENU);
	якщо (interactive)
	    printf("\nRemove it (y/n)? ");
	якщо (!interactive || confirm())
	    remove_start_menu();
    }

    printf("\n");
    found = remove_batfiles(0);
    якщо (found > 0)
    {
	якщо (interactive)
	    printf("\nRemove %s (y/n)? ", found > 1 ? "them" : "it");
	якщо (!interactive || confirm())
	    remove_batfiles(1);
    }

    fd = fopen("gvim.exe", "r");
    якщо (fd != NULL)
    {
	fclose(fd);
	printf("gvim.exe detected.  Attempting to unregister gvim with OLE\n");
	system("gvim.exe -silent -unregister");
    }

    delete_uninstall_key();

    якщо (interactive)
    {
	printf("\nYou may now want to delete the Vim executables and runtime files.\n");
	printf("(They are still where you unpacked them.)\n");
    }

    якщо (interactive)
    {
	rewind(stdin);
	printf("\nPress Enter to exit...");
	(void)getchar();
    }
    інакше
	sleep(3);

    повернути 0;
}
