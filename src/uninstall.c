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
#ಅನ್ತರ್ಭಾವ "dosinst.h"

/*
 * Return TRUE if the user types a 'y' or 'Y', FALSE otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
confirm(ಶೂನ್ಯ)
{
    ಅಕ್ಷರ	answer[10];

    fflush(stdout);
    ಹಿಂತಿರುಗಿ (scanf(" %c", answer) == 1 && ದೊಡ್ಡ_ಅಕ್ಷರ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)answer[0]) == 'Y');
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
reg_delete_ಕೀಲಿ(HKEY h_ಬೇರು_ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, DWORD flag)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ did_ತುಂಬು = FALSE;
    ಸ್ಥಿರ HANDLE advapi_lib = ಖಾಲಿ;
    ಸ್ಥಿರ LONG (WINAPI *delete_ಕೀಲಿ_ex)(HKEY, LPCTSTR, REGSAM, DWORD) = ಖಾಲಿ;

    ಆದರೆ (!did_ತುಂಬು)
    {
	// The RegDeleteKeyEx() function is only available on new systems.  It
	// is required for 64-bit registry access.  For other systems fall
	// back to RegDeleteKey().
	did_ತುಂಬು = TRUE;
	advapi_lib = ತುಂಬು_Library("ADVAPI32.DLL");
	ಆದರೆ (advapi_lib != ಖಾಲಿ)
	    delete_ಕೀಲಿ_ex = (LONG (WINAPI *)(HKEY, LPCTSTR, REGSAM, DWORD))Get_Proc_ವಿಳಾಸ(advapi_lib, "RegDeleteKeyExA");
    }
    ಆದರೆ (delete_ಕೀಲಿ_ex != ಖಾಲಿ)
    {
	ಹಿಂತಿರುಗಿ (*delete_ಕೀಲಿ_ex)(h_ಬೇರು_ಕೀಲಿ, ಕೀಲಿ, flag, 0);
    }
    ಹಿಂತಿರುಗಿ Reg_Delete_ಕೀಲಿ(h_ಬೇರು_ಕೀಲಿ, ಕೀಲಿ);
}

/*
 * Check if the popup menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
popup_gvim_ಮಾರ್ಗ(ಅಕ್ಷರ *buf, DWORD bufsize)
{
    HKEY	ಕೀಲಿ_handle;
    DWORD	ಮೌಲ್ಯ_type;
    ಪೂರ್ಣಾಂಕ		r;

    // Open the key where the path to gvim.exe is stored.
    ಆದರೆ (Reg_Open_ಕೀಲಿ_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    ಕೀಲಿ_WOW64_64_ಕೀಲಿ | ಕೀಲಿ_READ, &ಕೀಲಿ_handle) != ERROR_SUCCESS)
	ಆದರೆ (Reg_Open_ಕೀಲಿ_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    ಕೀಲಿ_WOW64_32_ಕೀಲಿ | ಕೀಲಿ_READ, &ಕೀಲಿ_handle) != ERROR_SUCCESS)
	    ಹಿಂತಿರುಗಿ 0;

    // get the DisplayName out of it to show the user
    r = Reg_Query_ಮೌಲ್ಯ_Ex(ಕೀಲಿ_handle, "path", 0,
					  &ಮೌಲ್ಯ_type, (LPBYTE)buf, &bufsize);
    Reg_Close_ಕೀಲಿ(ಕೀಲಿ_handle);

    ಹಿಂತಿರುಗಿ (r == ERROR_SUCCESS);
}

/*
 * Check if the "Open With..." menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
openwith_gvim_ಮಾರ್ಗ(ಅಕ್ಷರ *buf, DWORD bufsize)
{
    HKEY	ಕೀಲಿ_handle;
    DWORD	ಮೌಲ್ಯ_type;
    ಪೂರ್ಣಾಂಕ		r;

    // Open the key where the path to gvim.exe is stored.
    ಆದರೆ (Reg_Open_ಕೀಲಿ_Ex(HKEY_CLASSES_ಬೇರು,
		"Applications\\gvim.exe\\shell\\edit\\command", 0,
		    ಕೀಲಿ_WOW64_64_ಕೀಲಿ | ಕೀಲಿ_READ, &ಕೀಲಿ_handle) != ERROR_SUCCESS)
	ಹಿಂತಿರುಗಿ 0;

    // get the DisplayName out of it to show the user
    r = Reg_Query_ಮೌಲ್ಯ_Ex(ಕೀಲಿ_handle, "", 0, &ಮೌಲ್ಯ_type, (LPBYTE)buf, &bufsize);
    Reg_Close_ಕೀಲಿ(ಕೀಲಿ_handle);

    ಹಿಂತಿರುಗಿ (r == ERROR_SUCCESS);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
remove_popup(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		fail = 0;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		loop = is_64bit_os() ? 2 : 1;
    ಪೂರ್ಣಾಂಕ		maxfail = loop * 6;
    DWORD	flag;
    HKEY	kh;

    ಗಾಗಿ (i = 0; i < loop; i++)
    {
	ಆದರೆ (i == 0)
	    flag = ಕೀಲಿ_WOW64_32_ಕೀಲಿ;
	ಇಲ್ಲದಿದ್ದರೆ
	    flag = ಕೀಲಿ_WOW64_64_ಕೀಲಿ;

	ಆದರೆ (reg_delete_ಕೀಲಿ(HKEY_CLASSES_ಬೇರು, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}\\InProcServer32", flag) != ERROR_SUCCESS)
	    ++fail;
	ಆದರೆ (reg_delete_ಕೀಲಿ(HKEY_CLASSES_ಬೇರು, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}", flag) != ERROR_SUCCESS)
	    ++fail;
	ಆದರೆ (reg_delete_ಕೀಲಿ(HKEY_CLASSES_ಬೇರು, "*\\shellex\\ContextMenuHandlers\\gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	ಆದರೆ (Reg_Open_ಕೀಲಿ_Ex(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved", 0,
		    flag | ಕೀಲಿ_ALL_ACCESS, &kh) != ERROR_SUCCESS)
	    ++fail;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (Reg_Delete_ಮೌಲ್ಯ(kh, "{51EEE242-AD87-11d3-9C1E-0090278BBD99}") != ERROR_SUCCESS)
		++fail;
	    Reg_Close_ಕೀಲಿ(kh);
	}
	ಆದರೆ (reg_delete_ಕೀಲಿ(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	ಆದರೆ (reg_delete_ಕೀಲಿ(HKEY_LOCAL_MACHINE, "Software\\Vim", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    ಆದರೆ (fail == maxfail)
	ಬರೆ("No Vim popup registry entries could be removed\n");
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fail > 0)
	ಬರೆ("Some Vim popup registry entries could not be removed\n");
    ಇಲ್ಲದಿದ್ದರೆ
	ಬರೆ("The Vim popup registry entries have been removed\n");
}

    ಸ್ಥಿರ ಶೂನ್ಯ
remove_openwith(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		fail = 0;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		loop = is_64bit_os() ? 2 : 1;
    ಪೂರ್ಣಾಂಕ		maxfail = loop * 7;
    DWORD	flag;

    ಗಾಗಿ (i = 0; i < loop; i++)
    {
	ಆದರೆ (i == 0)
	    flag = ಕೀಲಿ_WOW64_32_ಕೀಲಿ;
	ಇಲ್ಲದಿದ್ದರೆ
	    flag = ಕೀಲಿ_WOW64_64_ಕೀಲಿ;

	ಆದರೆ (reg_delete_ಕೀಲಿ(HKEY_CLASSES_ಬೇರು, "Applications\\gvim.exe\\shell\\edit\\command", flag) != ERROR_SUCCESS)
	    ++fail;
	ಆದರೆ (reg_delete_ಕೀಲಿ(HKEY_CLASSES_ಬೇರು, "Applications\\gvim.exe\\shell\\edit", flag) != ERROR_SUCCESS)
	    ++fail;
	ಆದರೆ (reg_delete_ಕೀಲಿ(HKEY_CLASSES_ಬೇರು, "Applications\\gvim.exe\\shell", flag) != ERROR_SUCCESS)
	    ++fail;
	ಆದರೆ (reg_delete_ಕೀಲಿ(HKEY_CLASSES_ಬೇರು, "Applications\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	ಆದರೆ (reg_delete_ಕೀಲಿ(HKEY_CLASSES_ಬೇರು, ".htm\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	ಆದರೆ (reg_delete_ಕೀಲಿ(HKEY_CLASSES_ಬೇರು, ".vim\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	ಆದರೆ (reg_delete_ಕೀಲಿ(HKEY_CLASSES_ಬೇರು, "*\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    ಆದರೆ (fail == maxfail)
	ಬರೆ("No Vim open-with registry entries could be removed\n");
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fail > 0)
	ಬರೆ("Some Vim open-with registry entries could not be removed\n");
    ಇಲ್ಲದಿದ್ದರೆ
	ಬರೆ("The Vim open-with registry entries have been removed\n");
}

/*
 * Check if a batch file is really for the current version.  Don't delete a
 * batch file that was written for another (possibly newer) version.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
batfile_thisversion(ಅಕ್ಷರ *ಮಾರ್ಗ)
{
    FILE	*fd;
    ಅಕ್ಷರ	ಸಾಲು[BUFSIZE];
    ಪೂರ್ಣಾಂಕ		ಕೀಲಿ_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(VIMBAT_UNINSTKEY);
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು = FALSE;

    fd = fopen(ಮಾರ್ಗ, "r");
    ಆದರೆ (fd == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    ಆಗ (fgets(ಸಾಲು, ಗಾತ್ರ(ಸಾಲು), fd) != ಖಾಲಿ)
    {
	ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(ಸಾಲು, VIMBAT_UNINSTKEY, ಕೀಲಿ_len) == 0)
	{
	    ಸಿಕ್ಕಿತು = TRUE;
	    ನಿಲ್ಲಿಸು;
	}
    }
    fclose(fd);
    ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
remove_batfiles(ಪೂರ್ಣಾಂಕ doit)
{
    ಅಕ್ಷರ *batfile_ಮಾರ್ಗ;
    ಪೂರ್ಣಾಂಕ	 i;
    ಪೂರ್ಣಾಂಕ	 ಸಿಕ್ಕಿತು = 0;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    ಗಾಗಿ (i = 1; i < ಗುರಿ_ಎಣಿಕೆ; ++i)
    {
	batfile_ಮಾರ್ಗ = searchpath_save(targets[i].batname);
	ಆದರೆ (batfile_ಮಾರ್ಗ != ಖಾಲಿ && batfile_thisversion(batfile_ಮಾರ್ಗ))
	{
	    ++ಸಿಕ್ಕಿತು;
	    ಆದರೆ (doit)
	    {
		ಬರೆ("removing %s\n", batfile_ಮಾರ್ಗ);
		remove(batfile_ಮಾರ್ಗ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ಬರೆ(" - the batch file %s\n", batfile_ಮಾರ್ಗ);
	    ಬಿಡುಗಡೆ(batfile_ಮಾರ್ಗ);
	}
    }

    mch_chdir(installdir);
    ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
remove_if_exists(ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ *filename)
{
    ಅಕ್ಷರ buf[BUFSIZE];
    FILE *fd;

    sprintf(buf, "%s\\%s", ಮಾರ್ಗ, filename);

    fd = fopen(buf, "r");
    ಆದರೆ (fd == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    fclose(fd);
    ಬರೆ("removing %s\n", buf);
    remove(buf);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
remove_icons(ಶೂನ್ಯ)
{
    ಅಕ್ಷರ	ಮಾರ್ಗ[BUFSIZE];
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (get_shell_folder_ಮಾರ್ಗ(ಮಾರ್ಗ, "desktop"))
	ಗಾಗಿ (i = 0; i < ICON_ಎಣಿಕೆ; ++i)
	    remove_if_exists(ಮಾರ್ಗ, icon_link_ಹೆಸರುಗಳು[i]);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
remove_ಆರಂಭ_menu(ಶೂನ್ಯ)
{
    ಅಕ್ಷರ	ಮಾರ್ಗ[BUFSIZE];
    ಪೂರ್ಣಾಂಕ		i;
    ರಚನೆ stat st;

    ಆದರೆ (get_shell_folder_ಮಾರ್ಗ(ಮಾರ್ಗ, VIM_STARTMENU) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (i = 1; i < ಗುರಿ_ಎಣಿಕೆ; ++i)
	remove_if_exists(ಮಾರ್ಗ, targets[i].lnkname);
    remove_if_exists(ಮಾರ್ಗ, "uninstall.lnk");
    remove_if_exists(ಮಾರ್ಗ, "Help.lnk");
    // Win95 uses .pif, WinNT uses .lnk
    remove_if_exists(ಮಾರ್ಗ, "Vim tutor.pif");
    remove_if_exists(ಮಾರ್ಗ, "Vim tutor.lnk");
    remove_if_exists(ಮಾರ್ಗ, "Vim online.url");
    ಆದರೆ (stat(ಮಾರ್ಗ, &st) == 0)
    {
	ಬರೆ("removing %s\n", ಮಾರ್ಗ);
	rmdir(ಮಾರ್ಗ);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
delete_uninstall_ಕೀಲಿ(ಶೂನ್ಯ)
{
    reg_delete_ಕೀಲಿ(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Vim " VIM_ಆವೃತ್ತಿ_SHORT, ಕೀಲಿ_WOW64_64_ಕೀಲಿ);
}

    ಪೂರ್ಣಾಂಕ
ಮುಖ್ಯ(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ *argv[])
{
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು = 0;
    FILE	*fd;
    ಪೂರ್ಣಾಂಕ		i;
    ರಚನೆ stat st;
    ಅಕ್ಷರ	icon[BUFSIZE];
    ಅಕ್ಷರ	ಮಾರ್ಗ[MAX_ಮಾರ್ಗ];
    ಅಕ್ಷರ	popup_ಮಾರ್ಗ[MAX_ಮಾರ್ಗ];

    // The nsis uninstaller calls us with a "-nsis" argument.
    ಆದರೆ (argc == 2 && stricmp(argv[1], "-nsis") == 0)
	interactive = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ
	interactive = TRUE;

    // Initialize this program.
    do_inits(argv);

    ಬರೆ("This program will remove the following items:\n");

    ಆದರೆ (popup_gvim_ಮಾರ್ಗ(popup_ಮಾರ್ಗ, ಗಾತ್ರ(popup_ಮಾರ್ಗ)))
    {
	ಬರೆ(" - the \"Edit with Vim\" entry in the popup menu\n");
	ಬರೆ("   which uses \"%s\"\n", popup_ಮಾರ್ಗ);
	ಆದರೆ (interactive)
	    ಬರೆ("\nRemove it (y/n)? ");
	ಆದರೆ (!interactive || confirm())
	{
	    remove_popup();
	    // Assume the "Open With" entry can be removed as well, don't
	    // bother the user with asking him again.
	    remove_openwith();
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (openwith_gvim_ಮಾರ್ಗ(popup_ಮಾರ್ಗ, ಗಾತ್ರ(popup_ಮಾರ್ಗ)))
    {
	ಬರೆ(" - the Vim \"Open With...\" entry in the popup menu\n");
	ಬರೆ("   which uses \"%s\"\n", popup_ಮಾರ್ಗ);
	ಬರೆ("\nRemove it (y/n)? ");
	ಆದರೆ (confirm())
	    remove_openwith();
    }

    ಆದರೆ (get_shell_folder_ಮಾರ್ಗ(ಮಾರ್ಗ, "desktop"))
    {
	ಬರೆ("\n");
	ಗಾಗಿ (i = 0; i < ICON_ಎಣಿಕೆ; ++i)
	{
	    sprintf(icon, "%s\\%s", ಮಾರ್ಗ, icon_link_ಹೆಸರುಗಳು[i]);
	    ಆದರೆ (stat(icon, &st) == 0)
	    {
		ಬರೆ(" - the \"%s\" icon on the desktop\n", icon_ಹೆಸರುಗಳು[i]);
		++ಸಿಕ್ಕಿತು;
	    }
	}
	ಆದರೆ (ಸಿಕ್ಕಿತು > 0)
	{
	    ಆದರೆ (interactive)
		ಬರೆ("\nRemove %s (y/n)? ", ಸಿಕ್ಕಿತು > 1 ? "them" : "it");
	    ಆದರೆ (!interactive || confirm())
		remove_icons();
	}
    }

    ಆದರೆ (get_shell_folder_ಮಾರ್ಗ(ಮಾರ್ಗ, VIM_STARTMENU)
	    && stat(ಮಾರ್ಗ, &st) == 0)
    {
	ಬರೆ("\n - the \"%s\" entry in the Start Menu\n", VIM_STARTMENU);
	ಆದರೆ (interactive)
	    ಬರೆ("\nRemove it (y/n)? ");
	ಆದರೆ (!interactive || confirm())
	    remove_ಆರಂಭ_menu();
    }

    ಬರೆ("\n");
    ಸಿಕ್ಕಿತು = remove_batfiles(0);
    ಆದರೆ (ಸಿಕ್ಕಿತು > 0)
    {
	ಆದರೆ (interactive)
	    ಬರೆ("\nRemove %s (y/n)? ", ಸಿಕ್ಕಿತು > 1 ? "them" : "it");
	ಆದರೆ (!interactive || confirm())
	    remove_batfiles(1);
    }

    fd = fopen("gvim.exe", "r");
    ಆದರೆ (fd != ಖಾಲಿ)
    {
	fclose(fd);
	ಬರೆ("gvim.exe detected.  Attempting to unregister gvim with OLE\n");
	system("gvim.exe -silent -unregister");
    }

    delete_uninstall_ಕೀಲಿ();

    ಆದರೆ (interactive)
    {
	ಬರೆ("\nYou may now want to delete the Vim executables and runtime files.\n");
	ಬರೆ("(They are still where you unpacked them.)\n");
    }

    ಆದರೆ (interactive)
    {
	rewind(stdin);
	ಬರೆ("\nPress Enter to exit...");
	(ಶೂನ್ಯ)getchar();
    }
    ಇಲ್ಲದಿದ್ದರೆ
	sleep(3);

    ಹಿಂತಿರುಗಿ 0;
}
