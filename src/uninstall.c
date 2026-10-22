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
    أرجع (scanf(" %c", answer) == 1 && لوی_توری((بینښه حرف)answer[0]) == 'Y');
}

    ساكن صحيح
reg_delete_کلی(HKEY h_ریښه_کلی, ثابت حرف *کلی, DWORD flag)
{
    ساكن صحيح did_بارول = FALSE;
    ساكن HANDLE advapi_lib = عدم;
    ساكن LONG (WINAPI *delete_کلی_ex)(HKEY, LPCTSTR, REGSAM, DWORD) = عدم;

    إذا (!did_بارول)
    {
	// The RegDeleteKeyEx() function is only available on new systems.  It
	// is required for 64-bit registry access.  For other systems fall
	// back to RegDeleteKey().
	did_بارول = TRUE;
	advapi_lib = بارول_Library("ADVAPI32.DLL");
	إذا (advapi_lib != عدم)
	    delete_کلی_ex = (LONG (WINAPI *)(HKEY, LPCTSTR, REGSAM, DWORD))Get_Proc_پته(advapi_lib, "RegDeleteKeyExA");
    }
    إذا (delete_کلی_ex != عدم)
    {
	أرجع (*delete_کلی_ex)(h_ریښه_کلی, کلی, flag, 0);
    }
    أرجع Reg_Delete_کلی(h_ریښه_کلی, کلی);
}

/*
 * Check if the popup menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    ساكن صحيح
popup_gvim_لار(حرف *buf, DWORD bufsize)
{
    HKEY	کلی_handle;
    DWORD	ارزښت_type;
    صحيح		r;

    // Open the key where the path to gvim.exe is stored.
    إذا (Reg_Open_کلی_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    کلی_WOW64_64_کلی | کلی_READ, &کلی_handle) != ERROR_SUCCESS)
	إذا (Reg_Open_کلی_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    کلی_WOW64_32_کلی | کلی_READ, &کلی_handle) != ERROR_SUCCESS)
	    أرجع 0;

    // get the DisplayName out of it to show the user
    r = Reg_Query_ارزښت_Ex(کلی_handle, "path", 0,
					  &ارزښت_type, (LPBYTE)buf, &bufsize);
    Reg_Close_کلی(کلی_handle);

    أرجع (r == ERROR_SUCCESS);
}

/*
 * Check if the "Open With..." menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    ساكن صحيح
openwith_gvim_لار(حرف *buf, DWORD bufsize)
{
    HKEY	کلی_handle;
    DWORD	ارزښت_type;
    صحيح		r;

    // Open the key where the path to gvim.exe is stored.
    إذا (Reg_Open_کلی_Ex(HKEY_CLASSES_ریښه,
		"Applications\\gvim.exe\\shell\\edit\\command", 0,
		    کلی_WOW64_64_کلی | کلی_READ, &کلی_handle) != ERROR_SUCCESS)
	أرجع 0;

    // get the DisplayName out of it to show the user
    r = Reg_Query_ارزښت_Ex(کلی_handle, "", 0, &ارزښت_type, (LPBYTE)buf, &bufsize);
    Reg_Close_کلی(کلی_handle);

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
	    flag = کلی_WOW64_32_کلی;
	وإلا
	    flag = کلی_WOW64_64_کلی;

	إذا (reg_delete_کلی(HKEY_CLASSES_ریښه, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}\\InProcServer32", flag) != ERROR_SUCCESS)
	    ++fail;
	إذا (reg_delete_کلی(HKEY_CLASSES_ریښه, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}", flag) != ERROR_SUCCESS)
	    ++fail;
	إذا (reg_delete_کلی(HKEY_CLASSES_ریښه, "*\\shellex\\ContextMenuHandlers\\gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	إذا (Reg_Open_کلی_Ex(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved", 0,
		    flag | کلی_ALL_ACCESS, &kh) != ERROR_SUCCESS)
	    ++fail;
	وإلا
	{
	    إذا (Reg_Delete_ارزښت(kh, "{51EEE242-AD87-11d3-9C1E-0090278BBD99}") != ERROR_SUCCESS)
		++fail;
	    Reg_Close_کلی(kh);
	}
	إذا (reg_delete_کلی(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	إذا (reg_delete_کلی(HKEY_LOCAL_MACHINE, "Software\\Vim", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    إذا (fail == maxfail)
	چاپ_کړه("No Vim popup registry entries could be removed\n");
    وإلا إذا (fail > 0)
	چاپ_کړه("Some Vim popup registry entries could not be removed\n");
    وإلا
	چاپ_کړه("The Vim popup registry entries have been removed\n");
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
	    flag = کلی_WOW64_32_کلی;
	وإلا
	    flag = کلی_WOW64_64_کلی;

	إذا (reg_delete_کلی(HKEY_CLASSES_ریښه, "Applications\\gvim.exe\\shell\\edit\\command", flag) != ERROR_SUCCESS)
	    ++fail;
	إذا (reg_delete_کلی(HKEY_CLASSES_ریښه, "Applications\\gvim.exe\\shell\\edit", flag) != ERROR_SUCCESS)
	    ++fail;
	إذا (reg_delete_کلی(HKEY_CLASSES_ریښه, "Applications\\gvim.exe\\shell", flag) != ERROR_SUCCESS)
	    ++fail;
	إذا (reg_delete_کلی(HKEY_CLASSES_ریښه, "Applications\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	إذا (reg_delete_کلی(HKEY_CLASSES_ریښه, ".htm\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	إذا (reg_delete_کلی(HKEY_CLASSES_ریښه, ".vim\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	إذا (reg_delete_کلی(HKEY_CLASSES_ریښه, "*\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    إذا (fail == maxfail)
	چاپ_کړه("No Vim open-with registry entries could be removed\n");
    وإلا إذا (fail > 0)
	چاپ_کړه("Some Vim open-with registry entries could not be removed\n");
    وإلا
	چاپ_کړه("The Vim open-with registry entries have been removed\n");
}

/*
 * Check if a batch file is really for the current version.  Don't delete a
 * batch file that was written for another (possibly newer) version.
 */
    ساكن صحيح
batfile_thisversion(حرف *لار)
{
    FILE	*fd;
    حرف	کرښه[BUFSIZE];
    صحيح		کلی_len = مزي_اوږدوالی(VIMBAT_UNINSTKEY);
    صحيح		وموندل = FALSE;

    fd = fopen(لار, "r");
    إذا (fd == عدم)
	أرجع FALSE;

    طالما (fgets(کرښه, حجم(کرښه), fd) != عدم)
    {
	إذا (مزي_پرتله(کرښه, VIMBAT_UNINSTKEY, کلی_len) == 0)
	{
	    وموندل = TRUE;
	    اكسر;
	}
    }
    fclose(fd);
    أرجع وموندل;
}

    ساكن صحيح
remove_batfiles(صحيح doit)
{
    حرف *batfile_لار;
    صحيح	 i;
    صحيح	 وموندل = 0;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    لكل (i = 1; i < موخه_شمېر; ++i)
    {
	batfile_لار = searchpath_save(targets[i].batname);
	إذا (batfile_لار != عدم && batfile_thisversion(batfile_لار))
	{
	    ++وموندل;
	    إذا (doit)
	    {
		چاپ_کړه("removing %s\n", batfile_لار);
		remove(batfile_لار);
	    }
	    وإلا
		چاپ_کړه(" - the batch file %s\n", batfile_لار);
	    حرر(batfile_لار);
	}
    }

    mch_chdir(installdir);
    أرجع وموندل;
}

    ساكن فراغ
remove_if_exists(حرف *لار, حرف *filename)
{
    حرف buf[BUFSIZE];
    FILE *fd;

    sprintf(buf, "%s\\%s", لار, filename);

    fd = fopen(buf, "r");
    إذا (fd == عدم)
	أرجع;

    fclose(fd);
    چاپ_کړه("removing %s\n", buf);
    remove(buf);
}

    ساكن فراغ
remove_icons(فراغ)
{
    حرف	لار[BUFSIZE];
    صحيح		i;

    إذا (get_shell_folder_لار(لار, "desktop"))
	لكل (i = 0; i < ICON_شمېر; ++i)
	    remove_if_exists(لار, icon_link_نومونه[i]);
}

    ساكن فراغ
remove_پیل_menu(فراغ)
{
    حرف	لار[BUFSIZE];
    صحيح		i;
    بنية stat st;

    إذا (get_shell_folder_لار(لار, VIM_STARTMENU) == FAIL)
	أرجع;

    لكل (i = 1; i < موخه_شمېر; ++i)
	remove_if_exists(لار, targets[i].lnkname);
    remove_if_exists(لار, "uninstall.lnk");
    remove_if_exists(لار, "Help.lnk");
    // Win95 uses .pif, WinNT uses .lnk
    remove_if_exists(لار, "Vim tutor.pif");
    remove_if_exists(لار, "Vim tutor.lnk");
    remove_if_exists(لار, "Vim online.url");
    إذا (stat(لار, &st) == 0)
    {
	چاپ_کړه("removing %s\n", لار);
	rmdir(لار);
    }
}

    ساكن فراغ
delete_uninstall_کلی(فراغ)
{
    reg_delete_کلی(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Vim " VIM_نسخه_SHORT, کلی_WOW64_64_کلی);
}

    صحيح
اصلي(صحيح argc, حرف *argv[])
{
    صحيح		وموندل = 0;
    FILE	*fd;
    صحيح		i;
    بنية stat st;
    حرف	icon[BUFSIZE];
    حرف	لار[MAX_لار];
    حرف	popup_لار[MAX_لار];

    // The nsis uninstaller calls us with a "-nsis" argument.
    إذا (argc == 2 && stricmp(argv[1], "-nsis") == 0)
	interactive = FALSE;
    وإلا
	interactive = TRUE;

    // Initialize this program.
    do_inits(argv);

    چاپ_کړه("This program will remove the following items:\n");

    إذا (popup_gvim_لار(popup_لار, حجم(popup_لار)))
    {
	چاپ_کړه(" - the \"Edit with Vim\" entry in the popup menu\n");
	چاپ_کړه("   which uses \"%s\"\n", popup_لار);
	إذا (interactive)
	    چاپ_کړه("\nRemove it (y/n)? ");
	إذا (!interactive || confirm())
	{
	    remove_popup();
	    // Assume the "Open With" entry can be removed as well, don't
	    // bother the user with asking him again.
	    remove_openwith();
	}
    }
    وإلا إذا (openwith_gvim_لار(popup_لار, حجم(popup_لار)))
    {
	چاپ_کړه(" - the Vim \"Open With...\" entry in the popup menu\n");
	چاپ_کړه("   which uses \"%s\"\n", popup_لار);
	چاپ_کړه("\nRemove it (y/n)? ");
	إذا (confirm())
	    remove_openwith();
    }

    إذا (get_shell_folder_لار(لار, "desktop"))
    {
	چاپ_کړه("\n");
	لكل (i = 0; i < ICON_شمېر; ++i)
	{
	    sprintf(icon, "%s\\%s", لار, icon_link_نومونه[i]);
	    إذا (stat(icon, &st) == 0)
	    {
		چاپ_کړه(" - the \"%s\" icon on the desktop\n", icon_نومونه[i]);
		++وموندل;
	    }
	}
	إذا (وموندل > 0)
	{
	    إذا (interactive)
		چاپ_کړه("\nRemove %s (y/n)? ", وموندل > 1 ? "them" : "it");
	    إذا (!interactive || confirm())
		remove_icons();
	}
    }

    إذا (get_shell_folder_لار(لار, VIM_STARTMENU)
	    && stat(لار, &st) == 0)
    {
	چاپ_کړه("\n - the \"%s\" entry in the Start Menu\n", VIM_STARTMENU);
	إذا (interactive)
	    چاپ_کړه("\nRemove it (y/n)? ");
	إذا (!interactive || confirm())
	    remove_پیل_menu();
    }

    چاپ_کړه("\n");
    وموندل = remove_batfiles(0);
    إذا (وموندل > 0)
    {
	إذا (interactive)
	    چاپ_کړه("\nRemove %s (y/n)? ", وموندل > 1 ? "them" : "it");
	إذا (!interactive || confirm())
	    remove_batfiles(1);
    }

    fd = fopen("gvim.exe", "r");
    إذا (fd != عدم)
    {
	fclose(fd);
	چاپ_کړه("gvim.exe detected.  Attempting to unregister gvim with OLE\n");
	system("gvim.exe -silent -unregister");
    }

    delete_uninstall_کلی();

    إذا (interactive)
    {
	چاپ_کړه("\nYou may now want to delete the Vim executables and runtime files.\n");
	چاپ_کړه("(They are still where you unpacked them.)\n");
    }

    إذا (interactive)
    {
	rewind(stdin);
	چاپ_کړه("\nPress Enter to exit...");
	(فراغ)getchar();
    }
    وإلا
	sleep(3);

    أرجع 0;
}
