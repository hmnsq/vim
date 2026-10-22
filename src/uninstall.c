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
#ਅਨ੍ਤਰ੍ਭਾਵ "dosinst.h"

/*
 * Return TRUE if the user types a 'y' or 'Y', FALSE otherwise.
 */
    ਸਥਿਰ ਪੂਰਨ
confirm(ਖਾਲੀ)
{
    ਅੱਖਰ	answer[10];

    fflush(stdout);
    ਵਾਪਸ (scanf(" %c", answer) == 1 && ਵੱਡਾ_ਅੱਖਰ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)answer[0]) == 'Y');
}

    ਸਥਿਰ ਪੂਰਨ
reg_delete_ਕੁੰਜੀ(HKEY h_ਜੜ੍ਹ_ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, DWORD flag)
{
    ਸਥਿਰ ਪੂਰਨ did_ਲੱਦੋ = FALSE;
    ਸਥਿਰ HANDLE advapi_lib = ਨਿਰਾ;
    ਸਥਿਰ LONG (WINAPI *delete_ਕੁੰਜੀ_ex)(HKEY, LPCTSTR, REGSAM, DWORD) = ਨਿਰਾ;

    ਜੇ (!did_ਲੱਦੋ)
    {
	// The RegDeleteKeyEx() function is only available on new systems.  It
	// is required for 64-bit registry access.  For other systems fall
	// back to RegDeleteKey().
	did_ਲੱਦੋ = TRUE;
	advapi_lib = ਲੱਦੋ_Library("ADVAPI32.DLL");
	ਜੇ (advapi_lib != ਨਿਰਾ)
	    delete_ਕੁੰਜੀ_ex = (LONG (WINAPI *)(HKEY, LPCTSTR, REGSAM, DWORD))Get_Proc_ਪਤਾ(advapi_lib, "RegDeleteKeyExA");
    }
    ਜੇ (delete_ਕੁੰਜੀ_ex != ਨਿਰਾ)
    {
	ਵਾਪਸ (*delete_ਕੁੰਜੀ_ex)(h_ਜੜ੍ਹ_ਕੁੰਜੀ, ਕੁੰਜੀ, flag, 0);
    }
    ਵਾਪਸ Reg_Delete_ਕੁੰਜੀ(h_ਜੜ੍ਹ_ਕੁੰਜੀ, ਕੁੰਜੀ);
}

/*
 * Check if the popup menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    ਸਥਿਰ ਪੂਰਨ
popup_gvim_ਰਸਤਾ(ਅੱਖਰ *buf, DWORD bufsize)
{
    HKEY	ਕੁੰਜੀ_handle;
    DWORD	ਮੁੱਲ_type;
    ਪੂਰਨ		r;

    // Open the key where the path to gvim.exe is stored.
    ਜੇ (Reg_Open_ਕੁੰਜੀ_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    ਕੁੰਜੀ_WOW64_64_ਕੁੰਜੀ | ਕੁੰਜੀ_READ, &ਕੁੰਜੀ_handle) != ERROR_SUCCESS)
	ਜੇ (Reg_Open_ਕੁੰਜੀ_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    ਕੁੰਜੀ_WOW64_32_ਕੁੰਜੀ | ਕੁੰਜੀ_READ, &ਕੁੰਜੀ_handle) != ERROR_SUCCESS)
	    ਵਾਪਸ 0;

    // get the DisplayName out of it to show the user
    r = Reg_Query_ਮੁੱਲ_Ex(ਕੁੰਜੀ_handle, "path", 0,
					  &ਮੁੱਲ_type, (LPBYTE)buf, &bufsize);
    Reg_Close_ਕੁੰਜੀ(ਕੁੰਜੀ_handle);

    ਵਾਪਸ (r == ERROR_SUCCESS);
}

/*
 * Check if the "Open With..." menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    ਸਥਿਰ ਪੂਰਨ
openwith_gvim_ਰਸਤਾ(ਅੱਖਰ *buf, DWORD bufsize)
{
    HKEY	ਕੁੰਜੀ_handle;
    DWORD	ਮੁੱਲ_type;
    ਪੂਰਨ		r;

    // Open the key where the path to gvim.exe is stored.
    ਜੇ (Reg_Open_ਕੁੰਜੀ_Ex(HKEY_CLASSES_ਜੜ੍ਹ,
		"Applications\\gvim.exe\\shell\\edit\\command", 0,
		    ਕੁੰਜੀ_WOW64_64_ਕੁੰਜੀ | ਕੁੰਜੀ_READ, &ਕੁੰਜੀ_handle) != ERROR_SUCCESS)
	ਵਾਪਸ 0;

    // get the DisplayName out of it to show the user
    r = Reg_Query_ਮੁੱਲ_Ex(ਕੁੰਜੀ_handle, "", 0, &ਮੁੱਲ_type, (LPBYTE)buf, &bufsize);
    Reg_Close_ਕੁੰਜੀ(ਕੁੰਜੀ_handle);

    ਵਾਪਸ (r == ERROR_SUCCESS);
}

    ਸਥਿਰ ਖਾਲੀ
remove_popup(ਖਾਲੀ)
{
    ਪੂਰਨ		fail = 0;
    ਪੂਰਨ		i;
    ਪੂਰਨ		loop = is_64bit_os() ? 2 : 1;
    ਪੂਰਨ		maxfail = loop * 6;
    DWORD	flag;
    HKEY	kh;

    ਲਈ (i = 0; i < loop; i++)
    {
	ਜੇ (i == 0)
	    flag = ਕੁੰਜੀ_WOW64_32_ਕੁੰਜੀ;
	ਨਹੀਂਤਾਂ
	    flag = ਕੁੰਜੀ_WOW64_64_ਕੁੰਜੀ;

	ਜੇ (reg_delete_ਕੁੰਜੀ(HKEY_CLASSES_ਜੜ੍ਹ, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}\\InProcServer32", flag) != ERROR_SUCCESS)
	    ++fail;
	ਜੇ (reg_delete_ਕੁੰਜੀ(HKEY_CLASSES_ਜੜ੍ਹ, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}", flag) != ERROR_SUCCESS)
	    ++fail;
	ਜੇ (reg_delete_ਕੁੰਜੀ(HKEY_CLASSES_ਜੜ੍ਹ, "*\\shellex\\ContextMenuHandlers\\gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	ਜੇ (Reg_Open_ਕੁੰਜੀ_Ex(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved", 0,
		    flag | ਕੁੰਜੀ_ALL_ACCESS, &kh) != ERROR_SUCCESS)
	    ++fail;
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (Reg_Delete_ਮੁੱਲ(kh, "{51EEE242-AD87-11d3-9C1E-0090278BBD99}") != ERROR_SUCCESS)
		++fail;
	    Reg_Close_ਕੁੰਜੀ(kh);
	}
	ਜੇ (reg_delete_ਕੁੰਜੀ(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	ਜੇ (reg_delete_ਕੁੰਜੀ(HKEY_LOCAL_MACHINE, "Software\\Vim", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    ਜੇ (fail == maxfail)
	ਲਿਖੋ("No Vim popup registry entries could be removed\n");
    ਨਹੀਂਤਾਂ ਜੇ (fail > 0)
	ਲਿਖੋ("Some Vim popup registry entries could not be removed\n");
    ਨਹੀਂਤਾਂ
	ਲਿਖੋ("The Vim popup registry entries have been removed\n");
}

    ਸਥਿਰ ਖਾਲੀ
remove_openwith(ਖਾਲੀ)
{
    ਪੂਰਨ		fail = 0;
    ਪੂਰਨ		i;
    ਪੂਰਨ		loop = is_64bit_os() ? 2 : 1;
    ਪੂਰਨ		maxfail = loop * 7;
    DWORD	flag;

    ਲਈ (i = 0; i < loop; i++)
    {
	ਜੇ (i == 0)
	    flag = ਕੁੰਜੀ_WOW64_32_ਕੁੰਜੀ;
	ਨਹੀਂਤਾਂ
	    flag = ਕੁੰਜੀ_WOW64_64_ਕੁੰਜੀ;

	ਜੇ (reg_delete_ਕੁੰਜੀ(HKEY_CLASSES_ਜੜ੍ਹ, "Applications\\gvim.exe\\shell\\edit\\command", flag) != ERROR_SUCCESS)
	    ++fail;
	ਜੇ (reg_delete_ਕੁੰਜੀ(HKEY_CLASSES_ਜੜ੍ਹ, "Applications\\gvim.exe\\shell\\edit", flag) != ERROR_SUCCESS)
	    ++fail;
	ਜੇ (reg_delete_ਕੁੰਜੀ(HKEY_CLASSES_ਜੜ੍ਹ, "Applications\\gvim.exe\\shell", flag) != ERROR_SUCCESS)
	    ++fail;
	ਜੇ (reg_delete_ਕੁੰਜੀ(HKEY_CLASSES_ਜੜ੍ਹ, "Applications\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	ਜੇ (reg_delete_ਕੁੰਜੀ(HKEY_CLASSES_ਜੜ੍ਹ, ".htm\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	ਜੇ (reg_delete_ਕੁੰਜੀ(HKEY_CLASSES_ਜੜ੍ਹ, ".vim\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	ਜੇ (reg_delete_ਕੁੰਜੀ(HKEY_CLASSES_ਜੜ੍ਹ, "*\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    ਜੇ (fail == maxfail)
	ਲਿਖੋ("No Vim open-with registry entries could be removed\n");
    ਨਹੀਂਤਾਂ ਜੇ (fail > 0)
	ਲਿਖੋ("Some Vim open-with registry entries could not be removed\n");
    ਨਹੀਂਤਾਂ
	ਲਿਖੋ("The Vim open-with registry entries have been removed\n");
}

/*
 * Check if a batch file is really for the current version.  Don't delete a
 * batch file that was written for another (possibly newer) version.
 */
    ਸਥਿਰ ਪੂਰਨ
batfile_thisversion(ਅੱਖਰ *ਰਸਤਾ)
{
    FILE	*fd;
    ਅੱਖਰ	ਸਤਰ[BUFSIZE];
    ਪੂਰਨ		ਕੁੰਜੀ_len = ਲੜੀ_ਲੰਬਾਈ(VIMBAT_UNINSTKEY);
    ਪੂਰਨ		ਮਿਲਿਆ = FALSE;

    fd = fopen(ਰਸਤਾ, "r");
    ਜੇ (fd == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    ਜਦੋਂ (fgets(ਸਤਰ, ਆਕਾਰ(ਸਤਰ), fd) != ਨਿਰਾ)
    {
	ਜੇ (ਲੜੀ_ਤੁਲਨਾ(ਸਤਰ, VIMBAT_UNINSTKEY, ਕੁੰਜੀ_len) == 0)
	{
	    ਮਿਲਿਆ = TRUE;
	    ਰੋਕੋ;
	}
    }
    fclose(fd);
    ਵਾਪਸ ਮਿਲਿਆ;
}

    ਸਥਿਰ ਪੂਰਨ
remove_batfiles(ਪੂਰਨ doit)
{
    ਅੱਖਰ *batfile_ਰਸਤਾ;
    ਪੂਰਨ	 i;
    ਪੂਰਨ	 ਮਿਲਿਆ = 0;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    ਲਈ (i = 1; i < ਨਿਸ਼ਾਨਾ_ਗਿਣਤੀ; ++i)
    {
	batfile_ਰਸਤਾ = searchpath_save(targets[i].batname);
	ਜੇ (batfile_ਰਸਤਾ != ਨਿਰਾ && batfile_thisversion(batfile_ਰਸਤਾ))
	{
	    ++ਮਿਲਿਆ;
	    ਜੇ (doit)
	    {
		ਲਿਖੋ("removing %s\n", batfile_ਰਸਤਾ);
		remove(batfile_ਰਸਤਾ);
	    }
	    ਨਹੀਂਤਾਂ
		ਲਿਖੋ(" - the batch file %s\n", batfile_ਰਸਤਾ);
	    ਮੁਕਤ(batfile_ਰਸਤਾ);
	}
    }

    mch_chdir(installdir);
    ਵਾਪਸ ਮਿਲਿਆ;
}

    ਸਥਿਰ ਖਾਲੀ
remove_if_exists(ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ *filename)
{
    ਅੱਖਰ buf[BUFSIZE];
    FILE *fd;

    sprintf(buf, "%s\\%s", ਰਸਤਾ, filename);

    fd = fopen(buf, "r");
    ਜੇ (fd == ਨਿਰਾ)
	ਵਾਪਸ;

    fclose(fd);
    ਲਿਖੋ("removing %s\n", buf);
    remove(buf);
}

    ਸਥਿਰ ਖਾਲੀ
remove_icons(ਖਾਲੀ)
{
    ਅੱਖਰ	ਰਸਤਾ[BUFSIZE];
    ਪੂਰਨ		i;

    ਜੇ (get_shell_folder_ਰਸਤਾ(ਰਸਤਾ, "desktop"))
	ਲਈ (i = 0; i < ICON_ਗਿਣਤੀ; ++i)
	    remove_if_exists(ਰਸਤਾ, icon_link_ਨਾਂ[i]);
}

    ਸਥਿਰ ਖਾਲੀ
remove_ਸ਼ੁਰੂ_menu(ਖਾਲੀ)
{
    ਅੱਖਰ	ਰਸਤਾ[BUFSIZE];
    ਪੂਰਨ		i;
    ਬਣਤਰ stat st;

    ਜੇ (get_shell_folder_ਰਸਤਾ(ਰਸਤਾ, VIM_STARTMENU) == FAIL)
	ਵਾਪਸ;

    ਲਈ (i = 1; i < ਨਿਸ਼ਾਨਾ_ਗਿਣਤੀ; ++i)
	remove_if_exists(ਰਸਤਾ, targets[i].lnkname);
    remove_if_exists(ਰਸਤਾ, "uninstall.lnk");
    remove_if_exists(ਰਸਤਾ, "Help.lnk");
    // Win95 uses .pif, WinNT uses .lnk
    remove_if_exists(ਰਸਤਾ, "Vim tutor.pif");
    remove_if_exists(ਰਸਤਾ, "Vim tutor.lnk");
    remove_if_exists(ਰਸਤਾ, "Vim online.url");
    ਜੇ (stat(ਰਸਤਾ, &st) == 0)
    {
	ਲਿਖੋ("removing %s\n", ਰਸਤਾ);
	rmdir(ਰਸਤਾ);
    }
}

    ਸਥਿਰ ਖਾਲੀ
delete_uninstall_ਕੁੰਜੀ(ਖਾਲੀ)
{
    reg_delete_ਕੁੰਜੀ(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Vim " VIM_ਸੰਸਕਰਣ_SHORT, ਕੁੰਜੀ_WOW64_64_ਕੁੰਜੀ);
}

    ਪੂਰਨ
ਮੁੱਖ(ਪੂਰਨ argc, ਅੱਖਰ *argv[])
{
    ਪੂਰਨ		ਮਿਲਿਆ = 0;
    FILE	*fd;
    ਪੂਰਨ		i;
    ਬਣਤਰ stat st;
    ਅੱਖਰ	icon[BUFSIZE];
    ਅੱਖਰ	ਰਸਤਾ[MAX_ਰਸਤਾ];
    ਅੱਖਰ	popup_ਰਸਤਾ[MAX_ਰਸਤਾ];

    // The nsis uninstaller calls us with a "-nsis" argument.
    ਜੇ (argc == 2 && stricmp(argv[1], "-nsis") == 0)
	interactive = FALSE;
    ਨਹੀਂਤਾਂ
	interactive = TRUE;

    // Initialize this program.
    do_inits(argv);

    ਲਿਖੋ("This program will remove the following items:\n");

    ਜੇ (popup_gvim_ਰਸਤਾ(popup_ਰਸਤਾ, ਆਕਾਰ(popup_ਰਸਤਾ)))
    {
	ਲਿਖੋ(" - the \"Edit with Vim\" entry in the popup menu\n");
	ਲਿਖੋ("   which uses \"%s\"\n", popup_ਰਸਤਾ);
	ਜੇ (interactive)
	    ਲਿਖੋ("\nRemove it (y/n)? ");
	ਜੇ (!interactive || confirm())
	{
	    remove_popup();
	    // Assume the "Open With" entry can be removed as well, don't
	    // bother the user with asking him again.
	    remove_openwith();
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (openwith_gvim_ਰਸਤਾ(popup_ਰਸਤਾ, ਆਕਾਰ(popup_ਰਸਤਾ)))
    {
	ਲਿਖੋ(" - the Vim \"Open With...\" entry in the popup menu\n");
	ਲਿਖੋ("   which uses \"%s\"\n", popup_ਰਸਤਾ);
	ਲਿਖੋ("\nRemove it (y/n)? ");
	ਜੇ (confirm())
	    remove_openwith();
    }

    ਜੇ (get_shell_folder_ਰਸਤਾ(ਰਸਤਾ, "desktop"))
    {
	ਲਿਖੋ("\n");
	ਲਈ (i = 0; i < ICON_ਗਿਣਤੀ; ++i)
	{
	    sprintf(icon, "%s\\%s", ਰਸਤਾ, icon_link_ਨਾਂ[i]);
	    ਜੇ (stat(icon, &st) == 0)
	    {
		ਲਿਖੋ(" - the \"%s\" icon on the desktop\n", icon_ਨਾਂ[i]);
		++ਮਿਲਿਆ;
	    }
	}
	ਜੇ (ਮਿਲਿਆ > 0)
	{
	    ਜੇ (interactive)
		ਲਿਖੋ("\nRemove %s (y/n)? ", ਮਿਲਿਆ > 1 ? "them" : "it");
	    ਜੇ (!interactive || confirm())
		remove_icons();
	}
    }

    ਜੇ (get_shell_folder_ਰਸਤਾ(ਰਸਤਾ, VIM_STARTMENU)
	    && stat(ਰਸਤਾ, &st) == 0)
    {
	ਲਿਖੋ("\n - the \"%s\" entry in the Start Menu\n", VIM_STARTMENU);
	ਜੇ (interactive)
	    ਲਿਖੋ("\nRemove it (y/n)? ");
	ਜੇ (!interactive || confirm())
	    remove_ਸ਼ੁਰੂ_menu();
    }

    ਲਿਖੋ("\n");
    ਮਿਲਿਆ = remove_batfiles(0);
    ਜੇ (ਮਿਲਿਆ > 0)
    {
	ਜੇ (interactive)
	    ਲਿਖੋ("\nRemove %s (y/n)? ", ਮਿਲਿਆ > 1 ? "them" : "it");
	ਜੇ (!interactive || confirm())
	    remove_batfiles(1);
    }

    fd = fopen("gvim.exe", "r");
    ਜੇ (fd != ਨਿਰਾ)
    {
	fclose(fd);
	ਲਿਖੋ("gvim.exe detected.  Attempting to unregister gvim with OLE\n");
	system("gvim.exe -silent -unregister");
    }

    delete_uninstall_ਕੁੰਜੀ();

    ਜੇ (interactive)
    {
	ਲਿਖੋ("\nYou may now want to delete the Vim executables and runtime files.\n");
	ਲਿਖੋ("(They are still where you unpacked them.)\n");
    }

    ਜੇ (interactive)
    {
	rewind(stdin);
	ਲਿਖੋ("\nPress Enter to exit...");
	(ਖਾਲੀ)getchar();
    }
    ਨਹੀਂਤਾਂ
	sleep(3);

    ਵਾਪਸ 0;
}
