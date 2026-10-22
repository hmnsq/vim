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
#અન્તર્ભાવ "dosinst.h"

/*
 * Return TRUE if the user types a 'y' or 'Y', FALSE otherwise.
 */
    સ્થિર પૂર્ણાંક
confirm(શૂન્ય)
{
    અક્ષર	answer[10];

    fflush(stdout);
    પાછા (scanf(" %c", answer) == 1 && toupper((unsigned અક્ષર)answer[0]) == 'Y');
}

    સ્થિર પૂર્ણાંક
reg_delete_ચાવી(HKEY h_મૂળ_ચાવી, અચલ અક્ષર *ચાવી, DWORD flag)
{
    સ્થિર પૂર્ણાંક did_load = FALSE;
    સ્થિર HANDLE advapi_lib = ખાલી;
    સ્થિર LONG (WINAPI *delete_ચાવી_ex)(HKEY, LPCTSTR, REGSAM, DWORD) = ખાલી;

    જો (!did_load)
    {
	// The RegDeleteKeyEx() function is only available on new systems.  It
	// is required for 64-bit registry access.  For other systems fall
	// back to RegDeleteKey().
	did_load = TRUE;
	advapi_lib = LoadLibrary("ADVAPI32.DLL");
	જો (advapi_lib != ખાલી)
	    delete_ચાવી_ex = (LONG (WINAPI *)(HKEY, LPCTSTR, REGSAM, DWORD))GetProcAddress(advapi_lib, "RegDeleteKeyExA");
    }
    જો (delete_ચાવી_ex != ખાલી)
    {
	પાછા (*delete_ચાવી_ex)(h_મૂળ_ચાવી, ચાવી, flag, 0);
    }
    પાછા Reg_Delete_ચાવી(h_મૂળ_ચાવી, ચાવી);
}

/*
 * Check if the popup menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    સ્થિર પૂર્ણાંક
popup_gvim_path(અક્ષર *buf, DWORD bufsize)
{
    HKEY	ચાવી_handle;
    DWORD	કિંમત_type;
    પૂર્ણાંક		r;

    // Open the key where the path to gvim.exe is stored.
    જો (Reg_Open_ચાવી_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    ચાવી_WOW64_64_ચાવી | ચાવી_READ, &ચાવી_handle) != ERROR_SUCCESS)
	જો (Reg_Open_ચાવી_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    ચાવી_WOW64_32_ચાવી | ચાવી_READ, &ચાવી_handle) != ERROR_SUCCESS)
	    પાછા 0;

    // get the DisplayName out of it to show the user
    r = Reg_Query_કિંમત_Ex(ચાવી_handle, "path", 0,
					  &કિંમત_type, (LPBYTE)buf, &bufsize);
    Reg_Close_ચાવી(ચાવી_handle);

    પાછા (r == ERROR_SUCCESS);
}

/*
 * Check if the "Open With..." menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    સ્થિર પૂર્ણાંક
openwith_gvim_path(અક્ષર *buf, DWORD bufsize)
{
    HKEY	ચાવી_handle;
    DWORD	કિંમત_type;
    પૂર્ણાંક		r;

    // Open the key where the path to gvim.exe is stored.
    જો (Reg_Open_ચાવી_Ex(HKEY_CLASSES_મૂળ,
		"Applications\\gvim.exe\\shell\\edit\\command", 0,
		    ચાવી_WOW64_64_ચાવી | ચાવી_READ, &ચાવી_handle) != ERROR_SUCCESS)
	પાછા 0;

    // get the DisplayName out of it to show the user
    r = Reg_Query_કિંમત_Ex(ચાવી_handle, "", 0, &કિંમત_type, (LPBYTE)buf, &bufsize);
    Reg_Close_ચાવી(ચાવી_handle);

    પાછા (r == ERROR_SUCCESS);
}

    સ્થિર શૂન્ય
remove_popup(શૂન્ય)
{
    પૂર્ણાંક		fail = 0;
    પૂર્ણાંક		i;
    પૂર્ણાંક		loop = is_64bit_os() ? 2 : 1;
    પૂર્ણાંક		maxfail = loop * 6;
    DWORD	flag;
    HKEY	kh;

    માટે (i = 0; i < loop; i++)
    {
	જો (i == 0)
	    flag = ચાવી_WOW64_32_ચાવી;
	નહીંતર
	    flag = ચાવી_WOW64_64_ચાવી;

	જો (reg_delete_ચાવી(HKEY_CLASSES_મૂળ, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}\\InProcServer32", flag) != ERROR_SUCCESS)
	    ++fail;
	જો (reg_delete_ચાવી(HKEY_CLASSES_મૂળ, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}", flag) != ERROR_SUCCESS)
	    ++fail;
	જો (reg_delete_ચાવી(HKEY_CLASSES_મૂળ, "*\\shellex\\ContextMenuHandlers\\gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	જો (Reg_Open_ચાવી_Ex(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved", 0,
		    flag | ચાવી_ALL_ACCESS, &kh) != ERROR_SUCCESS)
	    ++fail;
	નહીંતર
	{
	    જો (Reg_Delete_કિંમત(kh, "{51EEE242-AD87-11d3-9C1E-0090278BBD99}") != ERROR_SUCCESS)
		++fail;
	    Reg_Close_ચાવી(kh);
	}
	જો (reg_delete_ચાવી(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	જો (reg_delete_ચાવી(HKEY_LOCAL_MACHINE, "Software\\Vim", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    જો (fail == maxfail)
	printf("No Vim popup registry entries could be removed\n");
    નહીંતર જો (fail > 0)
	printf("Some Vim popup registry entries could not be removed\n");
    નહીંતર
	printf("The Vim popup registry entries have been removed\n");
}

    સ્થિર શૂન્ય
remove_openwith(શૂન્ય)
{
    પૂર્ણાંક		fail = 0;
    પૂર્ણાંક		i;
    પૂર્ણાંક		loop = is_64bit_os() ? 2 : 1;
    પૂર્ણાંક		maxfail = loop * 7;
    DWORD	flag;

    માટે (i = 0; i < loop; i++)
    {
	જો (i == 0)
	    flag = ચાવી_WOW64_32_ચાવી;
	નહીંતર
	    flag = ચાવી_WOW64_64_ચાવી;

	જો (reg_delete_ચાવી(HKEY_CLASSES_મૂળ, "Applications\\gvim.exe\\shell\\edit\\command", flag) != ERROR_SUCCESS)
	    ++fail;
	જો (reg_delete_ચાવી(HKEY_CLASSES_મૂળ, "Applications\\gvim.exe\\shell\\edit", flag) != ERROR_SUCCESS)
	    ++fail;
	જો (reg_delete_ચાવી(HKEY_CLASSES_મૂળ, "Applications\\gvim.exe\\shell", flag) != ERROR_SUCCESS)
	    ++fail;
	જો (reg_delete_ચાવી(HKEY_CLASSES_મૂળ, "Applications\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	જો (reg_delete_ચાવી(HKEY_CLASSES_મૂળ, ".htm\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	જો (reg_delete_ચાવી(HKEY_CLASSES_મૂળ, ".vim\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	જો (reg_delete_ચાવી(HKEY_CLASSES_મૂળ, "*\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    જો (fail == maxfail)
	printf("No Vim open-with registry entries could be removed\n");
    નહીંતર જો (fail > 0)
	printf("Some Vim open-with registry entries could not be removed\n");
    નહીંતર
	printf("The Vim open-with registry entries have been removed\n");
}

/*
 * Check if a batch file is really for the current version.  Don't delete a
 * batch file that was written for another (possibly newer) version.
 */
    સ્થિર પૂર્ણાંક
batfile_thisversion(અક્ષર *path)
{
    FILE	*fd;
    અક્ષર	લીટી[BUFSIZE];
    પૂર્ણાંક		ચાવી_len = strlen(VIMBAT_UNINSTKEY);
    પૂર્ણાંક		found = FALSE;

    fd = fopen(path, "r");
    જો (fd == ખાલી)
	પાછા FALSE;

    જ્યારે (fgets(લીટી, કદ(લીટી), fd) != ખાલી)
    {
	જો (strncmp(લીટી, VIMBAT_UNINSTKEY, ચાવી_len) == 0)
	{
	    found = TRUE;
	    તોડો;
	}
    }
    fclose(fd);
    પાછા found;
}

    સ્થિર પૂર્ણાંક
remove_batfiles(પૂર્ણાંક doit)
{
    અક્ષર *batfile_path;
    પૂર્ણાંક	 i;
    પૂર્ણાંક	 found = 0;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    માટે (i = 1; i < TARGET_ગણતરી; ++i)
    {
	batfile_path = searchpath_save(targets[i].batname);
	જો (batfile_path != ખાલી && batfile_thisversion(batfile_path))
	{
	    ++found;
	    જો (doit)
	    {
		printf("removing %s\n", batfile_path);
		remove(batfile_path);
	    }
	    નહીંતર
		printf(" - the batch file %s\n", batfile_path);
	    મુક્ત(batfile_path);
	}
    }

    mch_chdir(installdir);
    પાછા found;
}

    સ્થિર શૂન્ય
remove_if_exists(અક્ષર *path, અક્ષર *filename)
{
    અક્ષર buf[BUFSIZE];
    FILE *fd;

    sprintf(buf, "%s\\%s", path, filename);

    fd = fopen(buf, "r");
    જો (fd == ખાલી)
	પાછા;

    fclose(fd);
    printf("removing %s\n", buf);
    remove(buf);
}

    સ્થિર શૂન્ય
remove_icons(શૂન્ય)
{
    અક્ષર	path[BUFSIZE];
    પૂર્ણાંક		i;

    જો (get_shell_folder_path(path, "desktop"))
	માટે (i = 0; i < ICON_ગણતરી; ++i)
	    remove_if_exists(path, icon_link_names[i]);
}

    સ્થિર શૂન્ય
remove_start_menu(શૂન્ય)
{
    અક્ષર	path[BUFSIZE];
    પૂર્ણાંક		i;
    સંરચના stat st;

    જો (get_shell_folder_path(path, VIM_STARTMENU) == FAIL)
	પાછા;

    માટે (i = 1; i < TARGET_ગણતરી; ++i)
	remove_if_exists(path, targets[i].lnkname);
    remove_if_exists(path, "uninstall.lnk");
    remove_if_exists(path, "Help.lnk");
    // Win95 uses .pif, WinNT uses .lnk
    remove_if_exists(path, "Vim tutor.pif");
    remove_if_exists(path, "Vim tutor.lnk");
    remove_if_exists(path, "Vim online.url");
    જો (stat(path, &st) == 0)
    {
	printf("removing %s\n", path);
	rmdir(path);
    }
}

    સ્થિર શૂન્ય
delete_uninstall_ચાવી(શૂન્ય)
{
    reg_delete_ચાવી(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Vim " VIM_VERSION_SHORT, ચાવી_WOW64_64_ચાવી);
}

    પૂર્ણાંક
મુખ્ય(પૂર્ણાંક argc, અક્ષર *argv[])
{
    પૂર્ણાંક		found = 0;
    FILE	*fd;
    પૂર્ણાંક		i;
    સંરચના stat st;
    અક્ષર	icon[BUFSIZE];
    અક્ષર	path[MAX_PATH];
    અક્ષર	popup_path[MAX_PATH];

    // The nsis uninstaller calls us with a "-nsis" argument.
    જો (argc == 2 && stricmp(argv[1], "-nsis") == 0)
	interactive = FALSE;
    નહીંતર
	interactive = TRUE;

    // Initialize this program.
    do_inits(argv);

    printf("This program will remove the following items:\n");

    જો (popup_gvim_path(popup_path, કદ(popup_path)))
    {
	printf(" - the \"Edit with Vim\" entry in the popup menu\n");
	printf("   which uses \"%s\"\n", popup_path);
	જો (interactive)
	    printf("\nRemove it (y/n)? ");
	જો (!interactive || confirm())
	{
	    remove_popup();
	    // Assume the "Open With" entry can be removed as well, don't
	    // bother the user with asking him again.
	    remove_openwith();
	}
    }
    નહીંતર જો (openwith_gvim_path(popup_path, કદ(popup_path)))
    {
	printf(" - the Vim \"Open With...\" entry in the popup menu\n");
	printf("   which uses \"%s\"\n", popup_path);
	printf("\nRemove it (y/n)? ");
	જો (confirm())
	    remove_openwith();
    }

    જો (get_shell_folder_path(path, "desktop"))
    {
	printf("\n");
	માટે (i = 0; i < ICON_ગણતરી; ++i)
	{
	    sprintf(icon, "%s\\%s", path, icon_link_names[i]);
	    જો (stat(icon, &st) == 0)
	    {
		printf(" - the \"%s\" icon on the desktop\n", icon_names[i]);
		++found;
	    }
	}
	જો (found > 0)
	{
	    જો (interactive)
		printf("\nRemove %s (y/n)? ", found > 1 ? "them" : "it");
	    જો (!interactive || confirm())
		remove_icons();
	}
    }

    જો (get_shell_folder_path(path, VIM_STARTMENU)
	    && stat(path, &st) == 0)
    {
	printf("\n - the \"%s\" entry in the Start Menu\n", VIM_STARTMENU);
	જો (interactive)
	    printf("\nRemove it (y/n)? ");
	જો (!interactive || confirm())
	    remove_start_menu();
    }

    printf("\n");
    found = remove_batfiles(0);
    જો (found > 0)
    {
	જો (interactive)
	    printf("\nRemove %s (y/n)? ", found > 1 ? "them" : "it");
	જો (!interactive || confirm())
	    remove_batfiles(1);
    }

    fd = fopen("gvim.exe", "r");
    જો (fd != ખાલી)
    {
	fclose(fd);
	printf("gvim.exe detected.  Attempting to unregister gvim with OLE\n");
	system("gvim.exe -silent -unregister");
    }

    delete_uninstall_ચાવી();

    જો (interactive)
    {
	printf("\nYou may now want to delete the Vim executables and runtime files.\n");
	printf("(They are still where you unpacked them.)\n");
    }

    જો (interactive)
    {
	rewind(stdin);
	printf("\nPress Enter to exit...");
	(શૂન્ય)getchar();
    }
    નહીંતર
	sleep(3);

    પાછા 0;
}
