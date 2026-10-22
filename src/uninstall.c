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
    أرجع (scanf(" %c", answer) == 1 && به_بزرگ((بیعلامت حرف)answer[0]) == 'Y');
}

    ساكن صحيح
reg_delete_کلید(HKEY h_ریشه_کلید, ثابت حرف *کلید, DWORD flag)
{
    ساكن صحيح did_بارگیری = FALSE;
    ساكن HANDLE advapi_lib = عدم;
    ساكن LONG (WINAPI *delete_کلید_ex)(HKEY, LPCTSTR, REGSAM, DWORD) = عدم;

    إذا (!did_بارگیری)
    {
	// The RegDeleteKeyEx() function is only available on new systems.  It
	// is required for 64-bit registry access.  For other systems fall
	// back to RegDeleteKey().
	did_بارگیری = TRUE;
	advapi_lib = بارگیری_Library("ADVAPI32.DLL");
	إذا (advapi_lib != عدم)
	    delete_کلید_ex = (LONG (WINAPI *)(HKEY, LPCTSTR, REGSAM, DWORD))Get_Proc_آدرس(advapi_lib, "RegDeleteKeyExA");
    }
    إذا (delete_کلید_ex != عدم)
    {
	أرجع (*delete_کلید_ex)(h_ریشه_کلید, کلید, flag, 0);
    }
    أرجع Reg_Delete_کلید(h_ریشه_کلید, کلید);
}

/*
 * Check if the popup menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    ساكن صحيح
popup_gvim_مسیر(حرف *buf, DWORD bufsize)
{
    HKEY	کلید_handle;
    DWORD	مقدار_type;
    صحيح		r;

    // Open the key where the path to gvim.exe is stored.
    إذا (Reg_Open_کلید_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    کلید_WOW64_64_کلید | کلید_READ, &کلید_handle) != ERROR_SUCCESS)
	إذا (Reg_Open_کلید_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    کلید_WOW64_32_کلید | کلید_READ, &کلید_handle) != ERROR_SUCCESS)
	    أرجع 0;

    // get the DisplayName out of it to show the user
    r = Reg_Query_مقدار_Ex(کلید_handle, "path", 0,
					  &مقدار_type, (LPBYTE)buf, &bufsize);
    Reg_Close_کلید(کلید_handle);

    أرجع (r == ERROR_SUCCESS);
}

/*
 * Check if the "Open With..." menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    ساكن صحيح
openwith_gvim_مسیر(حرف *buf, DWORD bufsize)
{
    HKEY	کلید_handle;
    DWORD	مقدار_type;
    صحيح		r;

    // Open the key where the path to gvim.exe is stored.
    إذا (Reg_Open_کلید_Ex(HKEY_CLASSES_ریشه,
		"Applications\\gvim.exe\\shell\\edit\\command", 0,
		    کلید_WOW64_64_کلید | کلید_READ, &کلید_handle) != ERROR_SUCCESS)
	أرجع 0;

    // get the DisplayName out of it to show the user
    r = Reg_Query_مقدار_Ex(کلید_handle, "", 0, &مقدار_type, (LPBYTE)buf, &bufsize);
    Reg_Close_کلید(کلید_handle);

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
	    flag = کلید_WOW64_32_کلید;
	وإلا
	    flag = کلید_WOW64_64_کلید;

	إذا (reg_delete_کلید(HKEY_CLASSES_ریشه, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}\\InProcServer32", flag) != ERROR_SUCCESS)
	    ++fail;
	إذا (reg_delete_کلید(HKEY_CLASSES_ریشه, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}", flag) != ERROR_SUCCESS)
	    ++fail;
	إذا (reg_delete_کلید(HKEY_CLASSES_ریشه, "*\\shellex\\ContextMenuHandlers\\gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	إذا (Reg_Open_کلید_Ex(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved", 0,
		    flag | کلید_ALL_ACCESS, &kh) != ERROR_SUCCESS)
	    ++fail;
	وإلا
	{
	    إذا (Reg_Delete_مقدار(kh, "{51EEE242-AD87-11d3-9C1E-0090278BBD99}") != ERROR_SUCCESS)
		++fail;
	    Reg_Close_کلید(kh);
	}
	إذا (reg_delete_کلید(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	إذا (reg_delete_کلید(HKEY_LOCAL_MACHINE, "Software\\Vim", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    إذا (fail == maxfail)
	چاپ_کن("No Vim popup registry entries could be removed\n");
    وإلا إذا (fail > 0)
	چاپ_کن("Some Vim popup registry entries could not be removed\n");
    وإلا
	چاپ_کن("The Vim popup registry entries have been removed\n");
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
	    flag = کلید_WOW64_32_کلید;
	وإلا
	    flag = کلید_WOW64_64_کلید;

	إذا (reg_delete_کلید(HKEY_CLASSES_ریشه, "Applications\\gvim.exe\\shell\\edit\\command", flag) != ERROR_SUCCESS)
	    ++fail;
	إذا (reg_delete_کلید(HKEY_CLASSES_ریشه, "Applications\\gvim.exe\\shell\\edit", flag) != ERROR_SUCCESS)
	    ++fail;
	إذا (reg_delete_کلید(HKEY_CLASSES_ریشه, "Applications\\gvim.exe\\shell", flag) != ERROR_SUCCESS)
	    ++fail;
	إذا (reg_delete_کلید(HKEY_CLASSES_ریشه, "Applications\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	إذا (reg_delete_کلید(HKEY_CLASSES_ریشه, ".htm\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	إذا (reg_delete_کلید(HKEY_CLASSES_ریشه, ".vim\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	إذا (reg_delete_کلید(HKEY_CLASSES_ریشه, "*\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    إذا (fail == maxfail)
	چاپ_کن("No Vim open-with registry entries could be removed\n");
    وإلا إذا (fail > 0)
	چاپ_کن("Some Vim open-with registry entries could not be removed\n");
    وإلا
	چاپ_کن("The Vim open-with registry entries have been removed\n");
}

/*
 * Check if a batch file is really for the current version.  Don't delete a
 * batch file that was written for another (possibly newer) version.
 */
    ساكن صحيح
batfile_thisversion(حرف *مسیر)
{
    FILE	*fd;
    حرف	سطر[BUFSIZE];
    صحيح		کلید_len = طول_رشته(VIMBAT_UNINSTKEY);
    صحيح		یافت = FALSE;

    fd = fopen(مسیر, "r");
    إذا (fd == عدم)
	أرجع FALSE;

    طالما (fgets(سطر, حجم(سطر), fd) != عدم)
    {
	إذا (مقایسه_رشته(سطر, VIMBAT_UNINSTKEY, کلید_len) == 0)
	{
	    یافت = TRUE;
	    اكسر;
	}
    }
    fclose(fd);
    أرجع یافت;
}

    ساكن صحيح
remove_batfiles(صحيح doit)
{
    حرف *batfile_مسیر;
    صحيح	 i;
    صحيح	 یافت = 0;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    لكل (i = 1; i < هدف_شمار; ++i)
    {
	batfile_مسیر = searchpath_save(targets[i].batname);
	إذا (batfile_مسیر != عدم && batfile_thisversion(batfile_مسیر))
	{
	    ++یافت;
	    إذا (doit)
	    {
		چاپ_کن("removing %s\n", batfile_مسیر);
		remove(batfile_مسیر);
	    }
	    وإلا
		چاپ_کن(" - the batch file %s\n", batfile_مسیر);
	    حرر(batfile_مسیر);
	}
    }

    mch_chdir(installdir);
    أرجع یافت;
}

    ساكن فراغ
remove_if_exists(حرف *مسیر, حرف *filename)
{
    حرف buf[BUFSIZE];
    FILE *fd;

    sprintf(buf, "%s\\%s", مسیر, filename);

    fd = fopen(buf, "r");
    إذا (fd == عدم)
	أرجع;

    fclose(fd);
    چاپ_کن("removing %s\n", buf);
    remove(buf);
}

    ساكن فراغ
remove_icons(فراغ)
{
    حرف	مسیر[BUFSIZE];
    صحيح		i;

    إذا (get_shell_folder_مسیر(مسیر, "desktop"))
	لكل (i = 0; i < ICON_شمار; ++i)
	    remove_if_exists(مسیر, icon_link_نامها[i]);
}

    ساكن فراغ
remove_شروع_menu(فراغ)
{
    حرف	مسیر[BUFSIZE];
    صحيح		i;
    بنية stat st;

    إذا (get_shell_folder_مسیر(مسیر, VIM_STARTMENU) == FAIL)
	أرجع;

    لكل (i = 1; i < هدف_شمار; ++i)
	remove_if_exists(مسیر, targets[i].lnkname);
    remove_if_exists(مسیر, "uninstall.lnk");
    remove_if_exists(مسیر, "Help.lnk");
    // Win95 uses .pif, WinNT uses .lnk
    remove_if_exists(مسیر, "Vim tutor.pif");
    remove_if_exists(مسیر, "Vim tutor.lnk");
    remove_if_exists(مسیر, "Vim online.url");
    إذا (stat(مسیر, &st) == 0)
    {
	چاپ_کن("removing %s\n", مسیر);
	rmdir(مسیر);
    }
}

    ساكن فراغ
delete_uninstall_کلید(فراغ)
{
    reg_delete_کلید(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Vim " VIM_نسخه_SHORT, کلید_WOW64_64_کلید);
}

    صحيح
اصلی(صحيح argc, حرف *argv[])
{
    صحيح		یافت = 0;
    FILE	*fd;
    صحيح		i;
    بنية stat st;
    حرف	icon[BUFSIZE];
    حرف	مسیر[MAX_مسیر];
    حرف	popup_مسیر[MAX_مسیر];

    // The nsis uninstaller calls us with a "-nsis" argument.
    إذا (argc == 2 && stricmp(argv[1], "-nsis") == 0)
	interactive = FALSE;
    وإلا
	interactive = TRUE;

    // Initialize this program.
    do_inits(argv);

    چاپ_کن("This program will remove the following items:\n");

    إذا (popup_gvim_مسیر(popup_مسیر, حجم(popup_مسیر)))
    {
	چاپ_کن(" - the \"Edit with Vim\" entry in the popup menu\n");
	چاپ_کن("   which uses \"%s\"\n", popup_مسیر);
	إذا (interactive)
	    چاپ_کن("\nRemove it (y/n)? ");
	إذا (!interactive || confirm())
	{
	    remove_popup();
	    // Assume the "Open With" entry can be removed as well, don't
	    // bother the user with asking him again.
	    remove_openwith();
	}
    }
    وإلا إذا (openwith_gvim_مسیر(popup_مسیر, حجم(popup_مسیر)))
    {
	چاپ_کن(" - the Vim \"Open With...\" entry in the popup menu\n");
	چاپ_کن("   which uses \"%s\"\n", popup_مسیر);
	چاپ_کن("\nRemove it (y/n)? ");
	إذا (confirm())
	    remove_openwith();
    }

    إذا (get_shell_folder_مسیر(مسیر, "desktop"))
    {
	چاپ_کن("\n");
	لكل (i = 0; i < ICON_شمار; ++i)
	{
	    sprintf(icon, "%s\\%s", مسیر, icon_link_نامها[i]);
	    إذا (stat(icon, &st) == 0)
	    {
		چاپ_کن(" - the \"%s\" icon on the desktop\n", icon_نامها[i]);
		++یافت;
	    }
	}
	إذا (یافت > 0)
	{
	    إذا (interactive)
		چاپ_کن("\nRemove %s (y/n)? ", یافت > 1 ? "them" : "it");
	    إذا (!interactive || confirm())
		remove_icons();
	}
    }

    إذا (get_shell_folder_مسیر(مسیر, VIM_STARTMENU)
	    && stat(مسیر, &st) == 0)
    {
	چاپ_کن("\n - the \"%s\" entry in the Start Menu\n", VIM_STARTMENU);
	إذا (interactive)
	    چاپ_کن("\nRemove it (y/n)? ");
	إذا (!interactive || confirm())
	    remove_شروع_menu();
    }

    چاپ_کن("\n");
    یافت = remove_batfiles(0);
    إذا (یافت > 0)
    {
	إذا (interactive)
	    چاپ_کن("\nRemove %s (y/n)? ", یافت > 1 ? "them" : "it");
	إذا (!interactive || confirm())
	    remove_batfiles(1);
    }

    fd = fopen("gvim.exe", "r");
    إذا (fd != عدم)
    {
	fclose(fd);
	چاپ_کن("gvim.exe detected.  Attempting to unregister gvim with OLE\n");
	system("gvim.exe -silent -unregister");
    }

    delete_uninstall_کلید();

    إذا (interactive)
    {
	چاپ_کن("\nYou may now want to delete the Vim executables and runtime files.\n");
	چاپ_کن("(They are still where you unpacked them.)\n");
    }

    إذا (interactive)
    {
	rewind(stdin);
	چاپ_کن("\nPress Enter to exit...");
	(فراغ)getchar();
    }
    وإلا
	sleep(3);

    أرجع 0;
}
