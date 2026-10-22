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
#ଅନ୍ତର୍ଭାଵ "dosinst.h"

/*
 * Return TRUE if the user types a 'y' or 'Y', FALSE otherwise.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
confirm(ଶୂନ୍ୟ)
{
    ଅକ୍ଷର	answer[10];

    fflush(stdout);
    ଫେରନ୍ତୁ (scanf(" %c", answer) == 1 && toupper((unsigned ଅକ୍ଷର)answer[0]) == 'Y');
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
reg_delete_ଚାବି(HKEY h_ମୂଳ_ଚାବି, ଅଚଳ ଅକ୍ଷର *ଚାବି, DWORD flag)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା did_load = FALSE;
    ସ୍ଥିର HANDLE advapi_lib = ଖାଲି;
    ସ୍ଥିର LONG (WINAPI *delete_ଚାବି_ex)(HKEY, LPCTSTR, REGSAM, DWORD) = ଖାଲି;

    ଯଦି (!did_load)
    {
	// The RegDeleteKeyEx() function is only available on new systems.  It
	// is required for 64-bit registry access.  For other systems fall
	// back to RegDeleteKey().
	did_load = TRUE;
	advapi_lib = LoadLibrary("ADVAPI32.DLL");
	ଯଦି (advapi_lib != ଖାଲି)
	    delete_ଚାବି_ex = (LONG (WINAPI *)(HKEY, LPCTSTR, REGSAM, DWORD))GetProcAddress(advapi_lib, "RegDeleteKeyExA");
    }
    ଯଦି (delete_ଚାବି_ex != ଖାଲି)
    {
	ଫେରନ୍ତୁ (*delete_ଚାବି_ex)(h_ମୂଳ_ଚାବି, ଚାବି, flag, 0);
    }
    ଫେରନ୍ତୁ Reg_Delete_ଚାବି(h_ମୂଳ_ଚାବି, ଚାବି);
}

/*
 * Check if the popup menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
popup_gvim_path(ଅକ୍ଷର *buf, DWORD bufsize)
{
    HKEY	ଚାବି_handle;
    DWORD	ମୂଲ୍ୟ_type;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r;

    // Open the key where the path to gvim.exe is stored.
    ଯଦି (Reg_Open_ଚାବି_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    ଚାବି_WOW64_64_ଚାବି | ଚାବି_READ, &ଚାବି_handle) != ERROR_SUCCESS)
	ଯଦି (Reg_Open_ଚାବି_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    ଚାବି_WOW64_32_ଚାବି | ଚାବି_READ, &ଚାବି_handle) != ERROR_SUCCESS)
	    ଫେରନ୍ତୁ 0;

    // get the DisplayName out of it to show the user
    r = Reg_Query_ମୂଲ୍ୟ_Ex(ଚାବି_handle, "path", 0,
					  &ମୂଲ୍ୟ_type, (LPBYTE)buf, &bufsize);
    Reg_Close_ଚାବି(ଚାବି_handle);

    ଫେରନ୍ତୁ (r == ERROR_SUCCESS);
}

/*
 * Check if the "Open With..." menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
openwith_gvim_path(ଅକ୍ଷର *buf, DWORD bufsize)
{
    HKEY	ଚାବି_handle;
    DWORD	ମୂଲ୍ୟ_type;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r;

    // Open the key where the path to gvim.exe is stored.
    ଯଦି (Reg_Open_ଚାବି_Ex(HKEY_CLASSES_ମୂଳ,
		"Applications\\gvim.exe\\shell\\edit\\command", 0,
		    ଚାବି_WOW64_64_ଚାବି | ଚାବି_READ, &ଚାବି_handle) != ERROR_SUCCESS)
	ଫେରନ୍ତୁ 0;

    // get the DisplayName out of it to show the user
    r = Reg_Query_ମୂଲ୍ୟ_Ex(ଚାବି_handle, "", 0, &ମୂଲ୍ୟ_type, (LPBYTE)buf, &bufsize);
    Reg_Close_ଚାବି(ଚାବି_handle);

    ଫେରନ୍ତୁ (r == ERROR_SUCCESS);
}

    ସ୍ଥିର ଶୂନ୍ୟ
remove_popup(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fail = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		loop = is_64bit_os() ? 2 : 1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxfail = loop * 6;
    DWORD	flag;
    HKEY	kh;

    ପାଇଁ (i = 0; i < loop; i++)
    {
	ଯଦି (i == 0)
	    flag = ଚାବି_WOW64_32_ଚାବି;
	ଅନ୍ୟଥା
	    flag = ଚାବି_WOW64_64_ଚାବି;

	ଯଦି (reg_delete_ଚାବି(HKEY_CLASSES_ମୂଳ, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}\\InProcServer32", flag) != ERROR_SUCCESS)
	    ++fail;
	ଯଦି (reg_delete_ଚାବି(HKEY_CLASSES_ମୂଳ, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}", flag) != ERROR_SUCCESS)
	    ++fail;
	ଯଦି (reg_delete_ଚାବି(HKEY_CLASSES_ମୂଳ, "*\\shellex\\ContextMenuHandlers\\gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	ଯଦି (Reg_Open_ଚାବି_Ex(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved", 0,
		    flag | ଚାବି_ALL_ACCESS, &kh) != ERROR_SUCCESS)
	    ++fail;
	ଅନ୍ୟଥା
	{
	    ଯଦି (Reg_Delete_ମୂଲ୍ୟ(kh, "{51EEE242-AD87-11d3-9C1E-0090278BBD99}") != ERROR_SUCCESS)
		++fail;
	    Reg_Close_ଚାବି(kh);
	}
	ଯଦି (reg_delete_ଚାବି(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	ଯଦି (reg_delete_ଚାବି(HKEY_LOCAL_MACHINE, "Software\\Vim", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    ଯଦି (fail == maxfail)
	printf("No Vim popup registry entries could be removed\n");
    ଅନ୍ୟଥା ଯଦି (fail > 0)
	printf("Some Vim popup registry entries could not be removed\n");
    ଅନ୍ୟଥା
	printf("The Vim popup registry entries have been removed\n");
}

    ସ୍ଥିର ଶୂନ୍ୟ
remove_openwith(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fail = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		loop = is_64bit_os() ? 2 : 1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxfail = loop * 7;
    DWORD	flag;

    ପାଇଁ (i = 0; i < loop; i++)
    {
	ଯଦି (i == 0)
	    flag = ଚାବି_WOW64_32_ଚାବି;
	ଅନ୍ୟଥା
	    flag = ଚାବି_WOW64_64_ଚାବି;

	ଯଦି (reg_delete_ଚାବି(HKEY_CLASSES_ମୂଳ, "Applications\\gvim.exe\\shell\\edit\\command", flag) != ERROR_SUCCESS)
	    ++fail;
	ଯଦି (reg_delete_ଚାବି(HKEY_CLASSES_ମୂଳ, "Applications\\gvim.exe\\shell\\edit", flag) != ERROR_SUCCESS)
	    ++fail;
	ଯଦି (reg_delete_ଚାବି(HKEY_CLASSES_ମୂଳ, "Applications\\gvim.exe\\shell", flag) != ERROR_SUCCESS)
	    ++fail;
	ଯଦି (reg_delete_ଚାବି(HKEY_CLASSES_ମୂଳ, "Applications\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	ଯଦି (reg_delete_ଚାବି(HKEY_CLASSES_ମୂଳ, ".htm\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	ଯଦି (reg_delete_ଚାବି(HKEY_CLASSES_ମୂଳ, ".vim\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	ଯଦି (reg_delete_ଚାବି(HKEY_CLASSES_ମୂଳ, "*\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    ଯଦି (fail == maxfail)
	printf("No Vim open-with registry entries could be removed\n");
    ଅନ୍ୟଥା ଯଦି (fail > 0)
	printf("Some Vim open-with registry entries could not be removed\n");
    ଅନ୍ୟଥା
	printf("The Vim open-with registry entries have been removed\n");
}

/*
 * Check if a batch file is really for the current version.  Don't delete a
 * batch file that was written for another (possibly newer) version.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
batfile_thisversion(ଅକ୍ଷର *path)
{
    FILE	*fd;
    ଅକ୍ଷର	ଧାଡ଼ି[BUFSIZE];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଚାବି_len = strlen(VIMBAT_UNINSTKEY);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found = FALSE;

    fd = fopen(path, "r");
    ଯଦି (fd == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    ଯେତେବେଳେ (fgets(ଧାଡ଼ି, ଆକାର(ଧାଡ଼ି), fd) != ଖାଲି)
    {
	ଯଦି (strncmp(ଧାଡ଼ି, VIMBAT_UNINSTKEY, ଚାବି_len) == 0)
	{
	    found = TRUE;
	    ଭାଙ୍ଗ;
	}
    }
    fclose(fd);
    ଫେରନ୍ତୁ found;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
remove_batfiles(ପୂର୍ଣ୍ଣସଂଖ୍ୟା doit)
{
    ଅକ୍ଷର *batfile_path;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	 i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	 found = 0;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    ପାଇଁ (i = 1; i < TARGET_ଗଣନା; ++i)
    {
	batfile_path = searchpath_save(targets[i].batname);
	ଯଦି (batfile_path != ଖାଲି && batfile_thisversion(batfile_path))
	{
	    ++found;
	    ଯଦି (doit)
	    {
		printf("removing %s\n", batfile_path);
		remove(batfile_path);
	    }
	    ଅନ୍ୟଥା
		printf(" - the batch file %s\n", batfile_path);
	    ମୁକ୍ତ(batfile_path);
	}
    }

    mch_chdir(installdir);
    ଫେରନ୍ତୁ found;
}

    ସ୍ଥିର ଶୂନ୍ୟ
remove_if_exists(ଅକ୍ଷର *path, ଅକ୍ଷର *filename)
{
    ଅକ୍ଷର buf[BUFSIZE];
    FILE *fd;

    sprintf(buf, "%s\\%s", path, filename);

    fd = fopen(buf, "r");
    ଯଦି (fd == ଖାଲି)
	ଫେରନ୍ତୁ;

    fclose(fd);
    printf("removing %s\n", buf);
    remove(buf);
}

    ସ୍ଥିର ଶୂନ୍ୟ
remove_icons(ଶୂନ୍ୟ)
{
    ଅକ୍ଷର	path[BUFSIZE];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (get_shell_folder_path(path, "desktop"))
	ପାଇଁ (i = 0; i < ICON_ଗଣନା; ++i)
	    remove_if_exists(path, icon_link_names[i]);
}

    ସ୍ଥିର ଶୂନ୍ୟ
remove_start_menu(ଶୂନ୍ୟ)
{
    ଅକ୍ଷର	path[BUFSIZE];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ସଂରଚନା stat st;

    ଯଦି (get_shell_folder_path(path, VIM_STARTMENU) == FAIL)
	ଫେରନ୍ତୁ;

    ପାଇଁ (i = 1; i < TARGET_ଗଣନା; ++i)
	remove_if_exists(path, targets[i].lnkname);
    remove_if_exists(path, "uninstall.lnk");
    remove_if_exists(path, "Help.lnk");
    // Win95 uses .pif, WinNT uses .lnk
    remove_if_exists(path, "Vim tutor.pif");
    remove_if_exists(path, "Vim tutor.lnk");
    remove_if_exists(path, "Vim online.url");
    ଯଦି (stat(path, &st) == 0)
    {
	printf("removing %s\n", path);
	rmdir(path);
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
delete_uninstall_ଚାବି(ଶୂନ୍ୟ)
{
    reg_delete_ଚାବି(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Vim " VIM_VERSION_SHORT, ଚାବି_WOW64_64_ଚାବି);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ମୁଖ୍ୟ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର *argv[])
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found = 0;
    FILE	*fd;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ସଂରଚନା stat st;
    ଅକ୍ଷର	icon[BUFSIZE];
    ଅକ୍ଷର	path[MAX_PATH];
    ଅକ୍ଷର	popup_path[MAX_PATH];

    // The nsis uninstaller calls us with a "-nsis" argument.
    ଯଦି (argc == 2 && stricmp(argv[1], "-nsis") == 0)
	interactive = FALSE;
    ଅନ୍ୟଥା
	interactive = TRUE;

    // Initialize this program.
    do_inits(argv);

    printf("This program will remove the following items:\n");

    ଯଦି (popup_gvim_path(popup_path, ଆକାର(popup_path)))
    {
	printf(" - the \"Edit with Vim\" entry in the popup menu\n");
	printf("   which uses \"%s\"\n", popup_path);
	ଯଦି (interactive)
	    printf("\nRemove it (y/n)? ");
	ଯଦି (!interactive || confirm())
	{
	    remove_popup();
	    // Assume the "Open With" entry can be removed as well, don't
	    // bother the user with asking him again.
	    remove_openwith();
	}
    }
    ଅନ୍ୟଥା ଯଦି (openwith_gvim_path(popup_path, ଆକାର(popup_path)))
    {
	printf(" - the Vim \"Open With...\" entry in the popup menu\n");
	printf("   which uses \"%s\"\n", popup_path);
	printf("\nRemove it (y/n)? ");
	ଯଦି (confirm())
	    remove_openwith();
    }

    ଯଦି (get_shell_folder_path(path, "desktop"))
    {
	printf("\n");
	ପାଇଁ (i = 0; i < ICON_ଗଣନା; ++i)
	{
	    sprintf(icon, "%s\\%s", path, icon_link_names[i]);
	    ଯଦି (stat(icon, &st) == 0)
	    {
		printf(" - the \"%s\" icon on the desktop\n", icon_names[i]);
		++found;
	    }
	}
	ଯଦି (found > 0)
	{
	    ଯଦି (interactive)
		printf("\nRemove %s (y/n)? ", found > 1 ? "them" : "it");
	    ଯଦି (!interactive || confirm())
		remove_icons();
	}
    }

    ଯଦି (get_shell_folder_path(path, VIM_STARTMENU)
	    && stat(path, &st) == 0)
    {
	printf("\n - the \"%s\" entry in the Start Menu\n", VIM_STARTMENU);
	ଯଦି (interactive)
	    printf("\nRemove it (y/n)? ");
	ଯଦି (!interactive || confirm())
	    remove_start_menu();
    }

    printf("\n");
    found = remove_batfiles(0);
    ଯଦି (found > 0)
    {
	ଯଦି (interactive)
	    printf("\nRemove %s (y/n)? ", found > 1 ? "them" : "it");
	ଯଦି (!interactive || confirm())
	    remove_batfiles(1);
    }

    fd = fopen("gvim.exe", "r");
    ଯଦି (fd != ଖାଲି)
    {
	fclose(fd);
	printf("gvim.exe detected.  Attempting to unregister gvim with OLE\n");
	system("gvim.exe -silent -unregister");
    }

    delete_uninstall_ଚାବି();

    ଯଦି (interactive)
    {
	printf("\nYou may now want to delete the Vim executables and runtime files.\n");
	printf("(They are still where you unpacked them.)\n");
    }

    ଯଦି (interactive)
    {
	rewind(stdin);
	printf("\nPress Enter to exit...");
	(ଶୂନ୍ୟ)getchar();
    }
    ଅନ୍ୟଥା
	sleep(3);

    ଫେରନ୍ତୁ 0;
}
