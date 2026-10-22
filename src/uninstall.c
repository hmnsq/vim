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
#شامل "dosinst.h"

/*
 * Return TRUE if the user types a 'y' or 'Y', FALSE otherwise.
 */
    ساکن صحیح
confirm(خالی)
{
    حرف	answer[10];

    fflush(stdout);
    واپس (scanf(" %c", answer) == 1 && بڑا_حرف((غیرعلامتی حرف)answer[0]) == 'Y');
}

    ساکن صحیح
reg_delete_کلید(HKEY h_جڑ_کلید, ثابت حرف *کلید, DWORD flag)
{
    ساکن صحیح did_بھرو = FALSE;
    ساکن HANDLE advapi_lib = تہی;
    ساکن LONG (WINAPI *delete_کلید_ex)(HKEY, LPCTSTR, REGSAM, DWORD) = تہی;

    اگر (!did_بھرو)
    {
	// The RegDeleteKeyEx() function is only available on new systems.  It
	// is required for 64-bit registry access.  For other systems fall
	// back to RegDeleteKey().
	did_بھرو = TRUE;
	advapi_lib = بھرو_Library("ADVAPI32.DLL");
	اگر (advapi_lib != تہی)
	    delete_کلید_ex = (LONG (WINAPI *)(HKEY, LPCTSTR, REGSAM, DWORD))Get_Proc_پتہ(advapi_lib, "RegDeleteKeyExA");
    }
    اگر (delete_کلید_ex != تہی)
    {
	واپس (*delete_کلید_ex)(h_جڑ_کلید, کلید, flag, 0);
    }
    واپس Reg_Delete_کلید(h_جڑ_کلید, کلید);
}

/*
 * Check if the popup menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    ساکن صحیح
popup_gvim_راستہ(حرف *buf, DWORD bufsize)
{
    HKEY	کلید_handle;
    DWORD	قدر_type;
    صحیح		r;

    // Open the key where the path to gvim.exe is stored.
    اگر (Reg_Open_کلید_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    کلید_WOW64_64_کلید | کلید_READ, &کلید_handle) != ERROR_SUCCESS)
	اگر (Reg_Open_کلید_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    کلید_WOW64_32_کلید | کلید_READ, &کلید_handle) != ERROR_SUCCESS)
	    واپس 0;

    // get the DisplayName out of it to show the user
    r = Reg_Query_قدر_Ex(کلید_handle, "path", 0,
					  &قدر_type, (LPBYTE)buf, &bufsize);
    Reg_Close_کلید(کلید_handle);

    واپس (r == ERROR_SUCCESS);
}

/*
 * Check if the "Open With..." menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    ساکن صحیح
openwith_gvim_راستہ(حرف *buf, DWORD bufsize)
{
    HKEY	کلید_handle;
    DWORD	قدر_type;
    صحیح		r;

    // Open the key where the path to gvim.exe is stored.
    اگر (Reg_Open_کلید_Ex(HKEY_CLASSES_جڑ,
		"Applications\\gvim.exe\\shell\\edit\\command", 0,
		    کلید_WOW64_64_کلید | کلید_READ, &کلید_handle) != ERROR_SUCCESS)
	واپس 0;

    // get the DisplayName out of it to show the user
    r = Reg_Query_قدر_Ex(کلید_handle, "", 0, &قدر_type, (LPBYTE)buf, &bufsize);
    Reg_Close_کلید(کلید_handle);

    واپس (r == ERROR_SUCCESS);
}

    ساکن خالی
remove_popup(خالی)
{
    صحیح		fail = 0;
    صحیح		i;
    صحیح		loop = is_64bit_os() ? 2 : 1;
    صحیح		maxfail = loop * 6;
    DWORD	flag;
    HKEY	kh;

    برائے (i = 0; i < loop; i++)
    {
	اگر (i == 0)
	    flag = کلید_WOW64_32_کلید;
	ورنہ
	    flag = کلید_WOW64_64_کلید;

	اگر (reg_delete_کلید(HKEY_CLASSES_جڑ, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}\\InProcServer32", flag) != ERROR_SUCCESS)
	    ++fail;
	اگر (reg_delete_کلید(HKEY_CLASSES_جڑ, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}", flag) != ERROR_SUCCESS)
	    ++fail;
	اگر (reg_delete_کلید(HKEY_CLASSES_جڑ, "*\\shellex\\ContextMenuHandlers\\gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	اگر (Reg_Open_کلید_Ex(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved", 0,
		    flag | کلید_ALL_ACCESS, &kh) != ERROR_SUCCESS)
	    ++fail;
	ورنہ
	{
	    اگر (Reg_Delete_قدر(kh, "{51EEE242-AD87-11d3-9C1E-0090278BBD99}") != ERROR_SUCCESS)
		++fail;
	    Reg_Close_کلید(kh);
	}
	اگر (reg_delete_کلید(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	اگر (reg_delete_کلید(HKEY_LOCAL_MACHINE, "Software\\Vim", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    اگر (fail == maxfail)
	لکھو("No Vim popup registry entries could be removed\n");
    ورنہ اگر (fail > 0)
	لکھو("Some Vim popup registry entries could not be removed\n");
    ورنہ
	لکھو("The Vim popup registry entries have been removed\n");
}

    ساکن خالی
remove_openwith(خالی)
{
    صحیح		fail = 0;
    صحیح		i;
    صحیح		loop = is_64bit_os() ? 2 : 1;
    صحیح		maxfail = loop * 7;
    DWORD	flag;

    برائے (i = 0; i < loop; i++)
    {
	اگر (i == 0)
	    flag = کلید_WOW64_32_کلید;
	ورنہ
	    flag = کلید_WOW64_64_کلید;

	اگر (reg_delete_کلید(HKEY_CLASSES_جڑ, "Applications\\gvim.exe\\shell\\edit\\command", flag) != ERROR_SUCCESS)
	    ++fail;
	اگر (reg_delete_کلید(HKEY_CLASSES_جڑ, "Applications\\gvim.exe\\shell\\edit", flag) != ERROR_SUCCESS)
	    ++fail;
	اگر (reg_delete_کلید(HKEY_CLASSES_جڑ, "Applications\\gvim.exe\\shell", flag) != ERROR_SUCCESS)
	    ++fail;
	اگر (reg_delete_کلید(HKEY_CLASSES_جڑ, "Applications\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	اگر (reg_delete_کلید(HKEY_CLASSES_جڑ, ".htm\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	اگر (reg_delete_کلید(HKEY_CLASSES_جڑ, ".vim\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	اگر (reg_delete_کلید(HKEY_CLASSES_جڑ, "*\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    اگر (fail == maxfail)
	لکھو("No Vim open-with registry entries could be removed\n");
    ورنہ اگر (fail > 0)
	لکھو("Some Vim open-with registry entries could not be removed\n");
    ورنہ
	لکھو("The Vim open-with registry entries have been removed\n");
}

/*
 * Check if a batch file is really for the current version.  Don't delete a
 * batch file that was written for another (possibly newer) version.
 */
    ساکن صحیح
batfile_thisversion(حرف *راستہ)
{
    FILE	*fd;
    حرف	سطر[BUFSIZE];
    صحیح		کلید_len = لڑی_لمبائی(VIMBAT_UNINSTKEY);
    صحیح		ملا = FALSE;

    fd = fopen(راستہ, "r");
    اگر (fd == تہی)
	واپس FALSE;

    جب (fgets(سطر, حجم(سطر), fd) != تہی)
    {
	اگر (لڑی_موازنہ(سطر, VIMBAT_UNINSTKEY, کلید_len) == 0)
	{
	    ملا = TRUE;
	    توڑو;
	}
    }
    fclose(fd);
    واپس ملا;
}

    ساکن صحیح
remove_batfiles(صحیح doit)
{
    حرف *batfile_راستہ;
    صحیح	 i;
    صحیح	 ملا = 0;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    برائے (i = 1; i < ہدف_گنتی; ++i)
    {
	batfile_راستہ = searchpath_save(targets[i].batname);
	اگر (batfile_راستہ != تہی && batfile_thisversion(batfile_راستہ))
	{
	    ++ملا;
	    اگر (doit)
	    {
		لکھو("removing %s\n", batfile_راستہ);
		remove(batfile_راستہ);
	    }
	    ورنہ
		لکھو(" - the batch file %s\n", batfile_راستہ);
	    آزاد(batfile_راستہ);
	}
    }

    mch_chdir(installdir);
    واپس ملا;
}

    ساکن خالی
remove_if_exists(حرف *راستہ, حرف *filename)
{
    حرف buf[BUFSIZE];
    FILE *fd;

    sprintf(buf, "%s\\%s", راستہ, filename);

    fd = fopen(buf, "r");
    اگر (fd == تہی)
	واپس;

    fclose(fd);
    لکھو("removing %s\n", buf);
    remove(buf);
}

    ساکن خالی
remove_icons(خالی)
{
    حرف	راستہ[BUFSIZE];
    صحیح		i;

    اگر (get_shell_folder_راستہ(راستہ, "desktop"))
	برائے (i = 0; i < ICON_گنتی; ++i)
	    remove_if_exists(راستہ, icon_link_نام[i]);
}

    ساکن خالی
remove_آغاز_menu(خالی)
{
    حرف	راستہ[BUFSIZE];
    صحیح		i;
    ساخت stat st;

    اگر (get_shell_folder_راستہ(راستہ, VIM_STARTMENU) == FAIL)
	واپس;

    برائے (i = 1; i < ہدف_گنتی; ++i)
	remove_if_exists(راستہ, targets[i].lnkname);
    remove_if_exists(راستہ, "uninstall.lnk");
    remove_if_exists(راستہ, "Help.lnk");
    // Win95 uses .pif, WinNT uses .lnk
    remove_if_exists(راستہ, "Vim tutor.pif");
    remove_if_exists(راستہ, "Vim tutor.lnk");
    remove_if_exists(راستہ, "Vim online.url");
    اگر (stat(راستہ, &st) == 0)
    {
	لکھو("removing %s\n", راستہ);
	rmdir(راستہ);
    }
}

    ساکن خالی
delete_uninstall_کلید(خالی)
{
    reg_delete_کلید(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Vim " VIM_نسخہ_SHORT, کلید_WOW64_64_کلید);
}

    صحیح
مین(صحیح argc, حرف *argv[])
{
    صحیح		ملا = 0;
    FILE	*fd;
    صحیح		i;
    ساخت stat st;
    حرف	icon[BUFSIZE];
    حرف	راستہ[MAX_راستہ];
    حرف	popup_راستہ[MAX_راستہ];

    // The nsis uninstaller calls us with a "-nsis" argument.
    اگر (argc == 2 && stricmp(argv[1], "-nsis") == 0)
	interactive = FALSE;
    ورنہ
	interactive = TRUE;

    // Initialize this program.
    do_inits(argv);

    لکھو("This program will remove the following items:\n");

    اگر (popup_gvim_راستہ(popup_راستہ, حجم(popup_راستہ)))
    {
	لکھو(" - the \"Edit with Vim\" entry in the popup menu\n");
	لکھو("   which uses \"%s\"\n", popup_راستہ);
	اگر (interactive)
	    لکھو("\nRemove it (y/n)? ");
	اگر (!interactive || confirm())
	{
	    remove_popup();
	    // Assume the "Open With" entry can be removed as well, don't
	    // bother the user with asking him again.
	    remove_openwith();
	}
    }
    ورنہ اگر (openwith_gvim_راستہ(popup_راستہ, حجم(popup_راستہ)))
    {
	لکھو(" - the Vim \"Open With...\" entry in the popup menu\n");
	لکھو("   which uses \"%s\"\n", popup_راستہ);
	لکھو("\nRemove it (y/n)? ");
	اگر (confirm())
	    remove_openwith();
    }

    اگر (get_shell_folder_راستہ(راستہ, "desktop"))
    {
	لکھو("\n");
	برائے (i = 0; i < ICON_گنتی; ++i)
	{
	    sprintf(icon, "%s\\%s", راستہ, icon_link_نام[i]);
	    اگر (stat(icon, &st) == 0)
	    {
		لکھو(" - the \"%s\" icon on the desktop\n", icon_نام[i]);
		++ملا;
	    }
	}
	اگر (ملا > 0)
	{
	    اگر (interactive)
		لکھو("\nRemove %s (y/n)? ", ملا > 1 ? "them" : "it");
	    اگر (!interactive || confirm())
		remove_icons();
	}
    }

    اگر (get_shell_folder_راستہ(راستہ, VIM_STARTMENU)
	    && stat(راستہ, &st) == 0)
    {
	لکھو("\n - the \"%s\" entry in the Start Menu\n", VIM_STARTMENU);
	اگر (interactive)
	    لکھو("\nRemove it (y/n)? ");
	اگر (!interactive || confirm())
	    remove_آغاز_menu();
    }

    لکھو("\n");
    ملا = remove_batfiles(0);
    اگر (ملا > 0)
    {
	اگر (interactive)
	    لکھو("\nRemove %s (y/n)? ", ملا > 1 ? "them" : "it");
	اگر (!interactive || confirm())
	    remove_batfiles(1);
    }

    fd = fopen("gvim.exe", "r");
    اگر (fd != تہی)
    {
	fclose(fd);
	لکھو("gvim.exe detected.  Attempting to unregister gvim with OLE\n");
	system("gvim.exe -silent -unregister");
    }

    delete_uninstall_کلید();

    اگر (interactive)
    {
	لکھو("\nYou may now want to delete the Vim executables and runtime files.\n");
	لکھو("(They are still where you unpacked them.)\n");
    }

    اگر (interactive)
    {
	rewind(stdin);
	لکھو("\nPress Enter to exit...");
	(خالی)getchar();
    }
    ورنہ
	sleep(3);

    واپس 0;
}
