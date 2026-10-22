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
#ضمن "dosinst.h"

/*
 * Return TRUE if the user types a 'y' or 'Y', FALSE otherwise.
 */
    ساكن صحيح
confirm(فراغ)
{
    حرف	answer[10];

    fflush(stdout);
    أرجع (scanf(" %c", answer) == 1 && toupper((unsigned حرف)answer[0]) == 'Y');
}

    ساكن صحيح
reg_delete_key(HKEY hRootKey, ثابت حرف *key, DWORD flag)
{
    ساكن صحيح did_load = FALSE;
    ساكن HANDLE advapi_lib = عدم;
    ساكن LONG (WINAPI *delete_key_ex)(HKEY, LPCTSTR, REGSAM, DWORD) = عدم;

    𐤀𐤌 (!did_load)
    {
	// The RegDeleteKeyEx() function is only available on new systems.  It
	// is required for 64-bit registry access.  For other systems fall
	// back to RegDeleteKey().
	did_load = TRUE;
	advapi_lib = LoadLibrary("ADVAPI32.DLL");
	𐤀𐤌 (advapi_lib != عدم)
	    delete_key_ex = (LONG (WINAPI *)(HKEY, LPCTSTR, REGSAM, DWORD))GetProcAddress(advapi_lib, "RegDeleteKeyExA");
    }
    𐤀𐤌 (delete_key_ex != عدم)
    {
	أرجع (*delete_key_ex)(hRootKey, key, flag, 0);
    }
    أرجع RegDeleteKey(hRootKey, key);
}

/*
 * Check if the popup menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    ساكن صحيح
popup_gvim_path(حرف *buf, DWORD bufsize)
{
    HKEY	key_handle;
    DWORD	value_type;
    صحيح		r;

    // Open the key where the path to gvim.exe is stored.
    𐤀𐤌 (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    KEY_WOW64_64KEY | KEY_READ, &key_handle) != ERROR_SUCCESS)
	𐤀𐤌 (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    KEY_WOW64_32KEY | KEY_READ, &key_handle) != ERROR_SUCCESS)
	    أرجع 0;

    // get the DisplayName out of it to show the user
    r = RegQueryValueEx(key_handle, "path", 0,
					  &value_type, (LPBYTE)buf, &bufsize);
    RegCloseKey(key_handle);

    أرجع (r == ERROR_SUCCESS);
}

/*
 * Check if the "Open With..." menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    ساكن صحيح
openwith_gvim_path(حرف *buf, DWORD bufsize)
{
    HKEY	key_handle;
    DWORD	value_type;
    صحيح		r;

    // Open the key where the path to gvim.exe is stored.
    𐤀𐤌 (RegOpenKeyEx(HKEY_CLASSES_ROOT,
		"Applications\\gvim.exe\\shell\\edit\\command", 0,
		    KEY_WOW64_64KEY | KEY_READ, &key_handle) != ERROR_SUCCESS)
	أرجع 0;

    // get the DisplayName out of it to show the user
    r = RegQueryValueEx(key_handle, "", 0, &value_type, (LPBYTE)buf, &bufsize);
    RegCloseKey(key_handle);

    أرجع (r == ERROR_SUCCESS);
}

    ساكن فراغ
remove_popup(فراغ)
{
    صحيح		fail = 0;
    صحيح		i;
    صحيح		loop = is_64bit_os() ? 2 : 1;
    صحيح		maxfail = loop * 6;
    DWORD	flag;
    HKEY	kh;

    لكل (i = 0; i < loop; i++)
    {
	𐤀𐤌 (i == 0)
	    flag = KEY_WOW64_32KEY;
	وإلا
	    flag = KEY_WOW64_64KEY;

	𐤀𐤌 (reg_delete_key(HKEY_CLASSES_ROOT, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}\\InProcServer32", flag) != ERROR_SUCCESS)
	    ++fail;
	𐤀𐤌 (reg_delete_key(HKEY_CLASSES_ROOT, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}", flag) != ERROR_SUCCESS)
	    ++fail;
	𐤀𐤌 (reg_delete_key(HKEY_CLASSES_ROOT, "*\\shellex\\ContextMenuHandlers\\gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	𐤀𐤌 (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved", 0,
		    flag | KEY_ALL_ACCESS, &kh) != ERROR_SUCCESS)
	    ++fail;
	وإلا
	{
	    𐤀𐤌 (RegDeleteValue(kh, "{51EEE242-AD87-11d3-9C1E-0090278BBD99}") != ERROR_SUCCESS)
		++fail;
	    RegCloseKey(kh);
	}
	𐤀𐤌 (reg_delete_key(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	𐤀𐤌 (reg_delete_key(HKEY_LOCAL_MACHINE, "Software\\Vim", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    𐤀𐤌 (fail == maxfail)
	printf("No Vim popup registry entries could be removed\n");
    وإلا 𐤀𐤌 (fail > 0)
	printf("Some Vim popup registry entries could not be removed\n");
    وإلا
	printf("The Vim popup registry entries have been removed\n");
}

    ساكن فراغ
remove_openwith(فراغ)
{
    صحيح		fail = 0;
    صحيح		i;
    صحيح		loop = is_64bit_os() ? 2 : 1;
    صحيح		maxfail = loop * 7;
    DWORD	flag;

    لكل (i = 0; i < loop; i++)
    {
	𐤀𐤌 (i == 0)
	    flag = KEY_WOW64_32KEY;
	وإلا
	    flag = KEY_WOW64_64KEY;

	𐤀𐤌 (reg_delete_key(HKEY_CLASSES_ROOT, "Applications\\gvim.exe\\shell\\edit\\command", flag) != ERROR_SUCCESS)
	    ++fail;
	𐤀𐤌 (reg_delete_key(HKEY_CLASSES_ROOT, "Applications\\gvim.exe\\shell\\edit", flag) != ERROR_SUCCESS)
	    ++fail;
	𐤀𐤌 (reg_delete_key(HKEY_CLASSES_ROOT, "Applications\\gvim.exe\\shell", flag) != ERROR_SUCCESS)
	    ++fail;
	𐤀𐤌 (reg_delete_key(HKEY_CLASSES_ROOT, "Applications\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	𐤀𐤌 (reg_delete_key(HKEY_CLASSES_ROOT, ".htm\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	𐤀𐤌 (reg_delete_key(HKEY_CLASSES_ROOT, ".vim\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	𐤀𐤌 (reg_delete_key(HKEY_CLASSES_ROOT, "*\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    𐤀𐤌 (fail == maxfail)
	printf("No Vim open-with registry entries could be removed\n");
    وإلا 𐤀𐤌 (fail > 0)
	printf("Some Vim open-with registry entries could not be removed\n");
    وإلا
	printf("The Vim open-with registry entries have been removed\n");
}

/*
 * Check if a batch file is really for the current version.  Don't delete a
 * batch file that was written for another (possibly newer) version.
 */
    ساكن صحيح
batfile_thisversion(حرف *path)
{
    FILE	*fd;
    حرف	line[BUFSIZE];
    صحيح		key_len = strlen(VIMBAT_UNINSTKEY);
    صحيح		found = FALSE;

    fd = fopen(path, "r");
    𐤀𐤌 (fd == عدم)
	أرجع FALSE;

    طالما (fgets(line, حجم(line), fd) != عدم)
    {
	𐤀𐤌 (strncmp(line, VIMBAT_UNINSTKEY, key_len) == 0)
	{
	    found = TRUE;
	    اكسر;
	}
    }
    fclose(fd);
    أرجع found;
}

    ساكن صحيح
remove_batfiles(صحيح doit)
{
    حرف *batfile_path;
    صحيح	 i;
    صحيح	 found = 0;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    لكل (i = 1; i < TARGET_COUNT; ++i)
    {
	batfile_path = searchpath_save(targets[i].batname);
	𐤀𐤌 (batfile_path != عدم && batfile_thisversion(batfile_path))
	{
	    ++found;
	    𐤀𐤌 (doit)
	    {
		printf("removing %s\n", batfile_path);
		remove(batfile_path);
	    }
	    وإلا
		printf(" - the batch file %s\n", batfile_path);
	    حرر(batfile_path);
	}
    }

    mch_chdir(installdir);
    أرجع found;
}

    ساكن فراغ
remove_if_exists(حرف *path, حرف *filename)
{
    حرف buf[BUFSIZE];
    FILE *fd;

    sprintf(buf, "%s\\%s", path, filename);

    fd = fopen(buf, "r");
    𐤀𐤌 (fd == عدم)
	أرجع;

    fclose(fd);
    printf("removing %s\n", buf);
    remove(buf);
}

    ساكن فراغ
remove_icons(فراغ)
{
    حرف	path[BUFSIZE];
    صحيح		i;

    𐤀𐤌 (get_shell_folder_path(path, "desktop"))
	لكل (i = 0; i < ICON_COUNT; ++i)
	    remove_if_exists(path, icon_link_names[i]);
}

    ساكن فراغ
remove_start_menu(فراغ)
{
    حرف	path[BUFSIZE];
    صحيح		i;
    بنية stat st;

    𐤀𐤌 (get_shell_folder_path(path, VIM_STARTMENU) == FAIL)
	أرجع;

    لكل (i = 1; i < TARGET_COUNT; ++i)
	remove_if_exists(path, targets[i].lnkname);
    remove_if_exists(path, "uninstall.lnk");
    remove_if_exists(path, "Help.lnk");
    // Win95 uses .pif, WinNT uses .lnk
    remove_if_exists(path, "Vim tutor.pif");
    remove_if_exists(path, "Vim tutor.lnk");
    remove_if_exists(path, "Vim online.url");
    𐤀𐤌 (stat(path, &st) == 0)
    {
	printf("removing %s\n", path);
	rmdir(path);
    }
}

    ساكن فراغ
delete_uninstall_key(فراغ)
{
    reg_delete_key(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Vim " VIM_VERSION_SHORT, KEY_WOW64_64KEY);
}

    صحيح
main(صحيح argc, حرف *argv[])
{
    صحيح		found = 0;
    FILE	*fd;
    صحيح		i;
    بنية stat st;
    حرف	icon[BUFSIZE];
    حرف	path[MAX_PATH];
    حرف	popup_path[MAX_PATH];

    // The nsis uninstaller calls us with a "-nsis" argument.
    𐤀𐤌 (argc == 2 && stricmp(argv[1], "-nsis") == 0)
	interactive = FALSE;
    وإلا
	interactive = TRUE;

    // Initialize this program.
    do_inits(argv);

    printf("This program will remove the following items:\n");

    𐤀𐤌 (popup_gvim_path(popup_path, حجم(popup_path)))
    {
	printf(" - the \"Edit with Vim\" entry in the popup menu\n");
	printf("   which uses \"%s\"\n", popup_path);
	𐤀𐤌 (interactive)
	    printf("\nRemove it (y/n)? ");
	𐤀𐤌 (!interactive || confirm())
	{
	    remove_popup();
	    // Assume the "Open With" entry can be removed as well, don't
	    // bother the user with asking him again.
	    remove_openwith();
	}
    }
    وإلا 𐤀𐤌 (openwith_gvim_path(popup_path, حجم(popup_path)))
    {
	printf(" - the Vim \"Open With...\" entry in the popup menu\n");
	printf("   which uses \"%s\"\n", popup_path);
	printf("\nRemove it (y/n)? ");
	𐤀𐤌 (confirm())
	    remove_openwith();
    }

    𐤀𐤌 (get_shell_folder_path(path, "desktop"))
    {
	printf("\n");
	لكل (i = 0; i < ICON_COUNT; ++i)
	{
	    sprintf(icon, "%s\\%s", path, icon_link_names[i]);
	    𐤀𐤌 (stat(icon, &st) == 0)
	    {
		printf(" - the \"%s\" icon on the desktop\n", icon_names[i]);
		++found;
	    }
	}
	𐤀𐤌 (found > 0)
	{
	    𐤀𐤌 (interactive)
		printf("\nRemove %s (y/n)? ", found > 1 ? "them" : "it");
	    𐤀𐤌 (!interactive || confirm())
		remove_icons();
	}
    }

    𐤀𐤌 (get_shell_folder_path(path, VIM_STARTMENU)
	    && stat(path, &st) == 0)
    {
	printf("\n - the \"%s\" entry in the Start Menu\n", VIM_STARTMENU);
	𐤀𐤌 (interactive)
	    printf("\nRemove it (y/n)? ");
	𐤀𐤌 (!interactive || confirm())
	    remove_start_menu();
    }

    printf("\n");
    found = remove_batfiles(0);
    𐤀𐤌 (found > 0)
    {
	𐤀𐤌 (interactive)
	    printf("\nRemove %s (y/n)? ", found > 1 ? "them" : "it");
	𐤀𐤌 (!interactive || confirm())
	    remove_batfiles(1);
    }

    fd = fopen("gvim.exe", "r");
    𐤀𐤌 (fd != عدم)
    {
	fclose(fd);
	printf("gvim.exe detected.  Attempting to unregister gvim with OLE\n");
	system("gvim.exe -silent -unregister");
    }

    delete_uninstall_key();

    𐤀𐤌 (interactive)
    {
	printf("\nYou may now want to delete the Vim executables and runtime files.\n");
	printf("(They are still where you unpacked them.)\n");
    }

    𐤀𐤌 (interactive)
    {
	rewind(stdin);
	printf("\nPress Enter to exit...");
	(فراغ)getchar();
    }
    وإلا
	sleep(3);

    أرجع 0;
}
