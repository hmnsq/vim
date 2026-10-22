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
    أرجع (scanf(" %c", answer) == 1 && وڏو_اکر((بنانشان حرف)answer[0]) == 'Y');
}

    ساكن صحيح
reg_delete_ڪنجي(HKEY h_پاڙ_ڪنجي, ثابت حرف *ڪنجي, DWORD flag)
{
    ساكن صحيح did_ڀريو = FALSE;
    ساكن HANDLE advapi_lib = عدم;
    ساكن LONG (WINAPI *delete_ڪنجي_ex)(HKEY, LPCTSTR, REGSAM, DWORD) = عدم;

    إذا (!did_ڀريو)
    {
	// The RegDeleteKeyEx() function is only available on new systems.  It
	// is required for 64-bit registry access.  For other systems fall
	// back to RegDeleteKey().
	did_ڀريو = TRUE;
	advapi_lib = ڀريو_Library("ADVAPI32.DLL");
	إذا (advapi_lib != عدم)
	    delete_ڪنجي_ex = (LONG (WINAPI *)(HKEY, LPCTSTR, REGSAM, DWORD))Get_Proc_پتو(advapi_lib, "RegDeleteKeyExA");
    }
    إذا (delete_ڪنجي_ex != عدم)
    {
	أرجع (*delete_ڪنجي_ex)(h_پاڙ_ڪنجي, ڪنجي, flag, 0);
    }
    أرجع Reg_Delete_ڪنجي(h_پاڙ_ڪنجي, ڪنجي);
}

/*
 * Check if the popup menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    ساكن صحيح
popup_gvim_رستو(حرف *buf, DWORD bufsize)
{
    HKEY	ڪنجي_handle;
    DWORD	قدر_type;
    صحيح		r;

    // Open the key where the path to gvim.exe is stored.
    إذا (Reg_Open_ڪنجي_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    ڪنجي_WOW64_64_ڪنجي | ڪنجي_READ, &ڪنجي_handle) != ERROR_SUCCESS)
	إذا (Reg_Open_ڪنجي_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    ڪنجي_WOW64_32_ڪنجي | ڪنجي_READ, &ڪنجي_handle) != ERROR_SUCCESS)
	    أرجع 0;

    // get the DisplayName out of it to show the user
    r = Reg_Query_قدر_Ex(ڪنجي_handle, "path", 0,
					  &قدر_type, (LPBYTE)buf, &bufsize);
    Reg_Close_ڪنجي(ڪنجي_handle);

    أرجع (r == ERROR_SUCCESS);
}

/*
 * Check if the "Open With..." menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    ساكن صحيح
openwith_gvim_رستو(حرف *buf, DWORD bufsize)
{
    HKEY	ڪنجي_handle;
    DWORD	قدر_type;
    صحيح		r;

    // Open the key where the path to gvim.exe is stored.
    إذا (Reg_Open_ڪنجي_Ex(HKEY_CLASSES_پاڙ,
		"Applications\\gvim.exe\\shell\\edit\\command", 0,
		    ڪنجي_WOW64_64_ڪنجي | ڪنجي_READ, &ڪنجي_handle) != ERROR_SUCCESS)
	أرجع 0;

    // get the DisplayName out of it to show the user
    r = Reg_Query_قدر_Ex(ڪنجي_handle, "", 0, &قدر_type, (LPBYTE)buf, &bufsize);
    Reg_Close_ڪنجي(ڪنجي_handle);

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
	إذا (i == 0)
	    flag = ڪنجي_WOW64_32_ڪنجي;
	وإلا
	    flag = ڪنجي_WOW64_64_ڪنجي;

	إذا (reg_delete_ڪنجي(HKEY_CLASSES_پاڙ, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}\\InProcServer32", flag) != ERROR_SUCCESS)
	    ++fail;
	إذا (reg_delete_ڪنجي(HKEY_CLASSES_پاڙ, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}", flag) != ERROR_SUCCESS)
	    ++fail;
	إذا (reg_delete_ڪنجي(HKEY_CLASSES_پاڙ, "*\\shellex\\ContextMenuHandlers\\gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	إذا (Reg_Open_ڪنجي_Ex(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved", 0,
		    flag | ڪنجي_ALL_ACCESS, &kh) != ERROR_SUCCESS)
	    ++fail;
	وإلا
	{
	    إذا (Reg_Delete_قدر(kh, "{51EEE242-AD87-11d3-9C1E-0090278BBD99}") != ERROR_SUCCESS)
		++fail;
	    Reg_Close_ڪنجي(kh);
	}
	إذا (reg_delete_ڪنجي(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	إذا (reg_delete_ڪنجي(HKEY_LOCAL_MACHINE, "Software\\Vim", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    إذا (fail == maxfail)
	لکو("No Vim popup registry entries could be removed\n");
    وإلا إذا (fail > 0)
	لکو("Some Vim popup registry entries could not be removed\n");
    وإلا
	لکو("The Vim popup registry entries have been removed\n");
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
	إذا (i == 0)
	    flag = ڪنجي_WOW64_32_ڪنجي;
	وإلا
	    flag = ڪنجي_WOW64_64_ڪنجي;

	إذا (reg_delete_ڪنجي(HKEY_CLASSES_پاڙ, "Applications\\gvim.exe\\shell\\edit\\command", flag) != ERROR_SUCCESS)
	    ++fail;
	إذا (reg_delete_ڪنجي(HKEY_CLASSES_پاڙ, "Applications\\gvim.exe\\shell\\edit", flag) != ERROR_SUCCESS)
	    ++fail;
	إذا (reg_delete_ڪنجي(HKEY_CLASSES_پاڙ, "Applications\\gvim.exe\\shell", flag) != ERROR_SUCCESS)
	    ++fail;
	إذا (reg_delete_ڪنجي(HKEY_CLASSES_پاڙ, "Applications\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	إذا (reg_delete_ڪنجي(HKEY_CLASSES_پاڙ, ".htm\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	إذا (reg_delete_ڪنجي(HKEY_CLASSES_پاڙ, ".vim\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	إذا (reg_delete_ڪنجي(HKEY_CLASSES_پاڙ, "*\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    إذا (fail == maxfail)
	لکو("No Vim open-with registry entries could be removed\n");
    وإلا إذا (fail > 0)
	لکو("Some Vim open-with registry entries could not be removed\n");
    وإلا
	لکو("The Vim open-with registry entries have been removed\n");
}

/*
 * Check if a batch file is really for the current version.  Don't delete a
 * batch file that was written for another (possibly newer) version.
 */
    ساكن صحيح
batfile_thisversion(حرف *رستو)
{
    FILE	*fd;
    حرف	سٽ[BUFSIZE];
    صحيح		ڪنجي_len = لڙي_ڊيگهه(VIMBAT_UNINSTKEY);
    صحيح		مليو = FALSE;

    fd = fopen(رستو, "r");
    إذا (fd == عدم)
	أرجع FALSE;

    طالما (fgets(سٽ, حجم(سٽ), fd) != عدم)
    {
	إذا (لڙي_ڀيٽ(سٽ, VIMBAT_UNINSTKEY, ڪنجي_len) == 0)
	{
	    مليو = TRUE;
	    اكسر;
	}
    }
    fclose(fd);
    أرجع مليو;
}

    ساكن صحيح
remove_batfiles(صحيح doit)
{
    حرف *batfile_رستو;
    صحيح	 i;
    صحيح	 مليو = 0;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    لكل (i = 1; i < هدف_ڳڻپ; ++i)
    {
	batfile_رستو = searchpath_save(targets[i].batname);
	إذا (batfile_رستو != عدم && batfile_thisversion(batfile_رستو))
	{
	    ++مليو;
	    إذا (doit)
	    {
		لکو("removing %s\n", batfile_رستو);
		remove(batfile_رستو);
	    }
	    وإلا
		لکو(" - the batch file %s\n", batfile_رستو);
	    حرر(batfile_رستو);
	}
    }

    mch_chdir(installdir);
    أرجع مليو;
}

    ساكن فراغ
remove_if_exists(حرف *رستو, حرف *filename)
{
    حرف buf[BUFSIZE];
    FILE *fd;

    sprintf(buf, "%s\\%s", رستو, filename);

    fd = fopen(buf, "r");
    إذا (fd == عدم)
	أرجع;

    fclose(fd);
    لکو("removing %s\n", buf);
    remove(buf);
}

    ساكن فراغ
remove_icons(فراغ)
{
    حرف	رستو[BUFSIZE];
    صحيح		i;

    إذا (get_shell_folder_رستو(رستو, "desktop"))
	لكل (i = 0; i < ICON_ڳڻپ; ++i)
	    remove_if_exists(رستو, icon_link_نالا[i]);
}

    ساكن فراغ
remove_شروعات_menu(فراغ)
{
    حرف	رستو[BUFSIZE];
    صحيح		i;
    بنية stat st;

    إذا (get_shell_folder_رستو(رستو, VIM_STARTMENU) == FAIL)
	أرجع;

    لكل (i = 1; i < هدف_ڳڻپ; ++i)
	remove_if_exists(رستو, targets[i].lnkname);
    remove_if_exists(رستو, "uninstall.lnk");
    remove_if_exists(رستو, "Help.lnk");
    // Win95 uses .pif, WinNT uses .lnk
    remove_if_exists(رستو, "Vim tutor.pif");
    remove_if_exists(رستو, "Vim tutor.lnk");
    remove_if_exists(رستو, "Vim online.url");
    إذا (stat(رستو, &st) == 0)
    {
	لکو("removing %s\n", رستو);
	rmdir(رستو);
    }
}

    ساكن فراغ
delete_uninstall_ڪنجي(فراغ)
{
    reg_delete_ڪنجي(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Vim " VIM_نسخو_SHORT, ڪنجي_WOW64_64_ڪنجي);
}

    صحيح
مکيه(صحيح argc, حرف *argv[])
{
    صحيح		مليو = 0;
    FILE	*fd;
    صحيح		i;
    بنية stat st;
    حرف	icon[BUFSIZE];
    حرف	رستو[MAX_رستو];
    حرف	popup_رستو[MAX_رستو];

    // The nsis uninstaller calls us with a "-nsis" argument.
    إذا (argc == 2 && stricmp(argv[1], "-nsis") == 0)
	interactive = FALSE;
    وإلا
	interactive = TRUE;

    // Initialize this program.
    do_inits(argv);

    لکو("This program will remove the following items:\n");

    إذا (popup_gvim_رستو(popup_رستو, حجم(popup_رستو)))
    {
	لکو(" - the \"Edit with Vim\" entry in the popup menu\n");
	لکو("   which uses \"%s\"\n", popup_رستو);
	إذا (interactive)
	    لکو("\nRemove it (y/n)? ");
	إذا (!interactive || confirm())
	{
	    remove_popup();
	    // Assume the "Open With" entry can be removed as well, don't
	    // bother the user with asking him again.
	    remove_openwith();
	}
    }
    وإلا إذا (openwith_gvim_رستو(popup_رستو, حجم(popup_رستو)))
    {
	لکو(" - the Vim \"Open With...\" entry in the popup menu\n");
	لکو("   which uses \"%s\"\n", popup_رستو);
	لکو("\nRemove it (y/n)? ");
	إذا (confirm())
	    remove_openwith();
    }

    إذا (get_shell_folder_رستو(رستو, "desktop"))
    {
	لکو("\n");
	لكل (i = 0; i < ICON_ڳڻپ; ++i)
	{
	    sprintf(icon, "%s\\%s", رستو, icon_link_نالا[i]);
	    إذا (stat(icon, &st) == 0)
	    {
		لکو(" - the \"%s\" icon on the desktop\n", icon_نالا[i]);
		++مليو;
	    }
	}
	إذا (مليو > 0)
	{
	    إذا (interactive)
		لکو("\nRemove %s (y/n)? ", مليو > 1 ? "them" : "it");
	    إذا (!interactive || confirm())
		remove_icons();
	}
    }

    إذا (get_shell_folder_رستو(رستو, VIM_STARTMENU)
	    && stat(رستو, &st) == 0)
    {
	لکو("\n - the \"%s\" entry in the Start Menu\n", VIM_STARTMENU);
	إذا (interactive)
	    لکو("\nRemove it (y/n)? ");
	إذا (!interactive || confirm())
	    remove_شروعات_menu();
    }

    لکو("\n");
    مليو = remove_batfiles(0);
    إذا (مليو > 0)
    {
	إذا (interactive)
	    لکو("\nRemove %s (y/n)? ", مليو > 1 ? "them" : "it");
	إذا (!interactive || confirm())
	    remove_batfiles(1);
    }

    fd = fopen("gvim.exe", "r");
    إذا (fd != عدم)
    {
	fclose(fd);
	لکو("gvim.exe detected.  Attempting to unregister gvim with OLE\n");
	system("gvim.exe -silent -unregister");
    }

    delete_uninstall_ڪنجي();

    إذا (interactive)
    {
	لکو("\nYou may now want to delete the Vim executables and runtime files.\n");
	لکو("(They are still where you unpacked them.)\n");
    }

    إذا (interactive)
    {
	rewind(stdin);
	لکو("\nPress Enter to exit...");
	(فراغ)getchar();
    }
    وإلا
	sleep(3);

    أرجع 0;
}
