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
#అన్తర్భావ "dosinst.h"

/*
 * Return TRUE if the user types a 'y' or 'Y', FALSE otherwise.
 */
    స్థిర పూర్ణాంకం
confirm(శూన్య)
{
    అక్షరం	answer[10];

    fflush(stdout);
    తిరిగి (scanf(" %c", answer) == 1 && పెద్ద_అక్షరం((గుర్తులేని అక్షరం)answer[0]) == 'Y');
}

    స్థిర పూర్ణాంకం
reg_delete_కీలకం(HKEY h_మూలం_కీలకం, స్థిరరాశి అక్షరం *కీలకం, DWORD flag)
{
    స్థిర పూర్ణాంకం did_ఎక్కించు = FALSE;
    స్థిర HANDLE advapi_lib = ఖాళీ;
    స్థిర LONG (WINAPI *delete_కీలకం_ex)(HKEY, LPCTSTR, REGSAM, DWORD) = ఖాళీ;

    ఒకవేళ (!did_ఎక్కించు)
    {
	// The RegDeleteKeyEx() function is only available on new systems.  It
	// is required for 64-bit registry access.  For other systems fall
	// back to RegDeleteKey().
	did_ఎక్కించు = TRUE;
	advapi_lib = ఎక్కించు_Library("ADVAPI32.DLL");
	ఒకవేళ (advapi_lib != ఖాళీ)
	    delete_కీలకం_ex = (LONG (WINAPI *)(HKEY, LPCTSTR, REGSAM, DWORD))Get_Proc_చిరునామా(advapi_lib, "RegDeleteKeyExA");
    }
    ఒకవేళ (delete_కీలకం_ex != ఖాళీ)
    {
	తిరిగి (*delete_కీలకం_ex)(h_మూలం_కీలకం, కీలకం, flag, 0);
    }
    తిరిగి Reg_Delete_కీలకం(h_మూలం_కీలకం, కీలకం);
}

/*
 * Check if the popup menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    స్థిర పూర్ణాంకం
popup_gvim_మార్గం(అక్షరం *buf, DWORD bufsize)
{
    HKEY	కీలకం_handle;
    DWORD	విలువ_type;
    పూర్ణాంకం		r;

    // Open the key where the path to gvim.exe is stored.
    ఒకవేళ (Reg_Open_కీలకం_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    కీలకం_WOW64_64_కీలకం | కీలకం_READ, &కీలకం_handle) != ERROR_SUCCESS)
	ఒకవేళ (Reg_Open_కీలకం_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    కీలకం_WOW64_32_కీలకం | కీలకం_READ, &కీలకం_handle) != ERROR_SUCCESS)
	    తిరిగి 0;

    // get the DisplayName out of it to show the user
    r = Reg_Query_విలువ_Ex(కీలకం_handle, "path", 0,
					  &విలువ_type, (LPBYTE)buf, &bufsize);
    Reg_Close_కీలకం(కీలకం_handle);

    తిరిగి (r == ERROR_SUCCESS);
}

/*
 * Check if the "Open With..." menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    స్థిర పూర్ణాంకం
openwith_gvim_మార్గం(అక్షరం *buf, DWORD bufsize)
{
    HKEY	కీలకం_handle;
    DWORD	విలువ_type;
    పూర్ణాంకం		r;

    // Open the key where the path to gvim.exe is stored.
    ఒకవేళ (Reg_Open_కీలకం_Ex(HKEY_CLASSES_మూలం,
		"Applications\\gvim.exe\\shell\\edit\\command", 0,
		    కీలకం_WOW64_64_కీలకం | కీలకం_READ, &కీలకం_handle) != ERROR_SUCCESS)
	తిరిగి 0;

    // get the DisplayName out of it to show the user
    r = Reg_Query_విలువ_Ex(కీలకం_handle, "", 0, &విలువ_type, (LPBYTE)buf, &bufsize);
    Reg_Close_కీలకం(కీలకం_handle);

    తిరిగి (r == ERROR_SUCCESS);
}

    స్థిర శూన్య
remove_popup(శూన్య)
{
    పూర్ణాంకం		fail = 0;
    పూర్ణాంకం		i;
    పూర్ణాంకం		loop = is_64bit_os() ? 2 : 1;
    పూర్ణాంకం		maxfail = loop * 6;
    DWORD	flag;
    HKEY	kh;

    కొరకు (i = 0; i < loop; i++)
    {
	ఒకవేళ (i == 0)
	    flag = కీలకం_WOW64_32_కీలకం;
	లేకపోతే
	    flag = కీలకం_WOW64_64_కీలకం;

	ఒకవేళ (reg_delete_కీలకం(HKEY_CLASSES_మూలం, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}\\InProcServer32", flag) != ERROR_SUCCESS)
	    ++fail;
	ఒకవేళ (reg_delete_కీలకం(HKEY_CLASSES_మూలం, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}", flag) != ERROR_SUCCESS)
	    ++fail;
	ఒకవేళ (reg_delete_కీలకం(HKEY_CLASSES_మూలం, "*\\shellex\\ContextMenuHandlers\\gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	ఒకవేళ (Reg_Open_కీలకం_Ex(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved", 0,
		    flag | కీలకం_ALL_ACCESS, &kh) != ERROR_SUCCESS)
	    ++fail;
	లేకపోతే
	{
	    ఒకవేళ (Reg_Delete_విలువ(kh, "{51EEE242-AD87-11d3-9C1E-0090278BBD99}") != ERROR_SUCCESS)
		++fail;
	    Reg_Close_కీలకం(kh);
	}
	ఒకవేళ (reg_delete_కీలకం(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	ఒకవేళ (reg_delete_కీలకం(HKEY_LOCAL_MACHINE, "Software\\Vim", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    ఒకవేళ (fail == maxfail)
	రాయి("No Vim popup registry entries could be removed\n");
    లేకపోతే ఒకవేళ (fail > 0)
	రాయి("Some Vim popup registry entries could not be removed\n");
    లేకపోతే
	రాయి("The Vim popup registry entries have been removed\n");
}

    స్థిర శూన్య
remove_openwith(శూన్య)
{
    పూర్ణాంకం		fail = 0;
    పూర్ణాంకం		i;
    పూర్ణాంకం		loop = is_64bit_os() ? 2 : 1;
    పూర్ణాంకం		maxfail = loop * 7;
    DWORD	flag;

    కొరకు (i = 0; i < loop; i++)
    {
	ఒకవేళ (i == 0)
	    flag = కీలకం_WOW64_32_కీలకం;
	లేకపోతే
	    flag = కీలకం_WOW64_64_కీలకం;

	ఒకవేళ (reg_delete_కీలకం(HKEY_CLASSES_మూలం, "Applications\\gvim.exe\\shell\\edit\\command", flag) != ERROR_SUCCESS)
	    ++fail;
	ఒకవేళ (reg_delete_కీలకం(HKEY_CLASSES_మూలం, "Applications\\gvim.exe\\shell\\edit", flag) != ERROR_SUCCESS)
	    ++fail;
	ఒకవేళ (reg_delete_కీలకం(HKEY_CLASSES_మూలం, "Applications\\gvim.exe\\shell", flag) != ERROR_SUCCESS)
	    ++fail;
	ఒకవేళ (reg_delete_కీలకం(HKEY_CLASSES_మూలం, "Applications\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	ఒకవేళ (reg_delete_కీలకం(HKEY_CLASSES_మూలం, ".htm\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	ఒకవేళ (reg_delete_కీలకం(HKEY_CLASSES_మూలం, ".vim\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	ఒకవేళ (reg_delete_కీలకం(HKEY_CLASSES_మూలం, "*\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    ఒకవేళ (fail == maxfail)
	రాయి("No Vim open-with registry entries could be removed\n");
    లేకపోతే ఒకవేళ (fail > 0)
	రాయి("Some Vim open-with registry entries could not be removed\n");
    లేకపోతే
	రాయి("The Vim open-with registry entries have been removed\n");
}

/*
 * Check if a batch file is really for the current version.  Don't delete a
 * batch file that was written for another (possibly newer) version.
 */
    స్థిర పూర్ణాంకం
batfile_thisversion(అక్షరం *మార్గం)
{
    FILE	*fd;
    అక్షరం	పంక్తి[BUFSIZE];
    పూర్ణాంకం		కీలకం_len = అక్షరశ్రేణి_పొడవు(VIMBAT_UNINSTKEY);
    పూర్ణాంకం		దొరికింది = FALSE;

    fd = fopen(మార్గం, "r");
    ఒకవేళ (fd == ఖాళీ)
	తిరిగి FALSE;

    అయితే (fgets(పంక్తి, పరిమాణం(పంక్తి), fd) != ఖాళీ)
    {
	ఒకవేళ (అక్షరశ్రేణి_పోలిక(పంక్తి, VIMBAT_UNINSTKEY, కీలకం_len) == 0)
	{
	    దొరికింది = TRUE;
	    ఆపు;
	}
    }
    fclose(fd);
    తిరిగి దొరికింది;
}

    స్థిర పూర్ణాంకం
remove_batfiles(పూర్ణాంకం doit)
{
    అక్షరం *batfile_మార్గం;
    పూర్ణాంకం	 i;
    పూర్ణాంకం	 దొరికింది = 0;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    కొరకు (i = 1; i < లక్ష్యం_లెక్క; ++i)
    {
	batfile_మార్గం = searchpath_save(targets[i].batname);
	ఒకవేళ (batfile_మార్గం != ఖాళీ && batfile_thisversion(batfile_మార్గం))
	{
	    ++దొరికింది;
	    ఒకవేళ (doit)
	    {
		రాయి("removing %s\n", batfile_మార్గం);
		remove(batfile_మార్గం);
	    }
	    లేకపోతే
		రాయి(" - the batch file %s\n", batfile_మార్గం);
	    విడుదల(batfile_మార్గం);
	}
    }

    mch_chdir(installdir);
    తిరిగి దొరికింది;
}

    స్థిర శూన్య
remove_if_exists(అక్షరం *మార్గం, అక్షరం *filename)
{
    అక్షరం buf[BUFSIZE];
    FILE *fd;

    sprintf(buf, "%s\\%s", మార్గం, filename);

    fd = fopen(buf, "r");
    ఒకవేళ (fd == ఖాళీ)
	తిరిగి;

    fclose(fd);
    రాయి("removing %s\n", buf);
    remove(buf);
}

    స్థిర శూన్య
remove_icons(శూన్య)
{
    అక్షరం	మార్గం[BUFSIZE];
    పూర్ణాంకం		i;

    ఒకవేళ (get_shell_folder_మార్గం(మార్గం, "desktop"))
	కొరకు (i = 0; i < ICON_లెక్క; ++i)
	    remove_if_exists(మార్గం, icon_link_పేర్లు[i]);
}

    స్థిర శూన్య
remove_ప్రారంభం_menu(శూన్య)
{
    అక్షరం	మార్గం[BUFSIZE];
    పూర్ణాంకం		i;
    నిర్మాణం stat st;

    ఒకవేళ (get_shell_folder_మార్గం(మార్గం, VIM_STARTMENU) == FAIL)
	తిరిగి;

    కొరకు (i = 1; i < లక్ష్యం_లెక్క; ++i)
	remove_if_exists(మార్గం, targets[i].lnkname);
    remove_if_exists(మార్గం, "uninstall.lnk");
    remove_if_exists(మార్గం, "Help.lnk");
    // Win95 uses .pif, WinNT uses .lnk
    remove_if_exists(మార్గం, "Vim tutor.pif");
    remove_if_exists(మార్గం, "Vim tutor.lnk");
    remove_if_exists(మార్గం, "Vim online.url");
    ఒకవేళ (stat(మార్గం, &st) == 0)
    {
	రాయి("removing %s\n", మార్గం);
	rmdir(మార్గం);
    }
}

    స్థిర శూన్య
delete_uninstall_కీలకం(శూన్య)
{
    reg_delete_కీలకం(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Vim " VIM_సంస్కరణ_SHORT, కీలకం_WOW64_64_కీలకం);
}

    పూర్ణాంకం
ప్రధాన(పూర్ణాంకం argc, అక్షరం *argv[])
{
    పూర్ణాంకం		దొరికింది = 0;
    FILE	*fd;
    పూర్ణాంకం		i;
    నిర్మాణం stat st;
    అక్షరం	icon[BUFSIZE];
    అక్షరం	మార్గం[MAX_మార్గం];
    అక్షరం	popup_మార్గం[MAX_మార్గం];

    // The nsis uninstaller calls us with a "-nsis" argument.
    ఒకవేళ (argc == 2 && stricmp(argv[1], "-nsis") == 0)
	interactive = FALSE;
    లేకపోతే
	interactive = TRUE;

    // Initialize this program.
    do_inits(argv);

    రాయి("This program will remove the following items:\n");

    ఒకవేళ (popup_gvim_మార్గం(popup_మార్గం, పరిమాణం(popup_మార్గం)))
    {
	రాయి(" - the \"Edit with Vim\" entry in the popup menu\n");
	రాయి("   which uses \"%s\"\n", popup_మార్గం);
	ఒకవేళ (interactive)
	    రాయి("\nRemove it (y/n)? ");
	ఒకవేళ (!interactive || confirm())
	{
	    remove_popup();
	    // Assume the "Open With" entry can be removed as well, don't
	    // bother the user with asking him again.
	    remove_openwith();
	}
    }
    లేకపోతే ఒకవేళ (openwith_gvim_మార్గం(popup_మార్గం, పరిమాణం(popup_మార్గం)))
    {
	రాయి(" - the Vim \"Open With...\" entry in the popup menu\n");
	రాయి("   which uses \"%s\"\n", popup_మార్గం);
	రాయి("\nRemove it (y/n)? ");
	ఒకవేళ (confirm())
	    remove_openwith();
    }

    ఒకవేళ (get_shell_folder_మార్గం(మార్గం, "desktop"))
    {
	రాయి("\n");
	కొరకు (i = 0; i < ICON_లెక్క; ++i)
	{
	    sprintf(icon, "%s\\%s", మార్గం, icon_link_పేర్లు[i]);
	    ఒకవేళ (stat(icon, &st) == 0)
	    {
		రాయి(" - the \"%s\" icon on the desktop\n", icon_పేర్లు[i]);
		++దొరికింది;
	    }
	}
	ఒకవేళ (దొరికింది > 0)
	{
	    ఒకవేళ (interactive)
		రాయి("\nRemove %s (y/n)? ", దొరికింది > 1 ? "them" : "it");
	    ఒకవేళ (!interactive || confirm())
		remove_icons();
	}
    }

    ఒకవేళ (get_shell_folder_మార్గం(మార్గం, VIM_STARTMENU)
	    && stat(మార్గం, &st) == 0)
    {
	రాయి("\n - the \"%s\" entry in the Start Menu\n", VIM_STARTMENU);
	ఒకవేళ (interactive)
	    రాయి("\nRemove it (y/n)? ");
	ఒకవేళ (!interactive || confirm())
	    remove_ప్రారంభం_menu();
    }

    రాయి("\n");
    దొరికింది = remove_batfiles(0);
    ఒకవేళ (దొరికింది > 0)
    {
	ఒకవేళ (interactive)
	    రాయి("\nRemove %s (y/n)? ", దొరికింది > 1 ? "them" : "it");
	ఒకవేళ (!interactive || confirm())
	    remove_batfiles(1);
    }

    fd = fopen("gvim.exe", "r");
    ఒకవేళ (fd != ఖాళీ)
    {
	fclose(fd);
	రాయి("gvim.exe detected.  Attempting to unregister gvim with OLE\n");
	system("gvim.exe -silent -unregister");
    }

    delete_uninstall_కీలకం();

    ఒకవేళ (interactive)
    {
	రాయి("\nYou may now want to delete the Vim executables and runtime files.\n");
	రాయి("(They are still where you unpacked them.)\n");
    }

    ఒకవేళ (interactive)
    {
	rewind(stdin);
	రాయి("\nPress Enter to exit...");
	(శూన్య)getchar();
    }
    లేకపోతే
	sleep(3);

    తిరిగి 0;
}
