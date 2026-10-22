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
#अन्तर्भाव "dosinst.h"

/*
 * Return TRUE if the user types a 'y' or 'Y', FALSE otherwise.
 */
    स्थिर पूर्णांक
confirm(शून्य)
{
    अक्षर	answer[10];

    ख_साफ(मानक_निकास);
    परत (म_पूछो(" %c", answer) == 1 && बड़े((अचिन्हित अक्षर)answer[0]) == 'Y');
}

    स्थिर पूर्णांक
reg_delete_किल्ली(HKEY h_मूळ_किल्ली, स्थिरांक अक्षर *किल्ली, DWORD flag)
{
    स्थिर पूर्णांक did_load = FALSE;
    स्थिर HANDLE advapi_lib = रिक्त;
    स्थिर LONG (WINAPI *delete_किल्ली_ex)(HKEY, LPCTSTR, REGSAM, DWORD) = रिक्त;

    जर (!did_load)
    {
	// The RegDeleteKeyEx() function is only available on new systems.  It
	// is required for 64-bit registry access.  For other systems fall
	// back to RegDeleteKey().
	did_load = TRUE;
	advapi_lib = LoadLibrary("ADVAPI32.DLL");
	जर (advapi_lib != रिक्त)
	    delete_किल्ली_ex = (LONG (WINAPI *)(HKEY, LPCTSTR, REGSAM, DWORD))GetProcAddress(advapi_lib, "RegDeleteKeyExA");
    }
    जर (delete_किल्ली_ex != रिक्त)
    {
	परत (*delete_किल्ली_ex)(h_मूळ_किल्ली, किल्ली, flag, 0);
    }
    परत Reg_Delete_किल्ली(h_मूळ_किल्ली, किल्ली);
}

/*
 * Check if the popup menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    स्थिर पूर्णांक
popup_gvim_path(अक्षर *buf, DWORD bufsize)
{
    HKEY	किल्ली_handle;
    DWORD	मूल्य_type;
    पूर्णांक		r;

    // Open the key where the path to gvim.exe is stored.
    जर (Reg_Open_किल्ली_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    किल्ली_WOW64_64_किल्ली | किल्ली_READ, &किल्ली_handle) != ERROR_SUCCESS)
	जर (Reg_Open_किल्ली_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
		    किल्ली_WOW64_32_किल्ली | किल्ली_READ, &किल्ली_handle) != ERROR_SUCCESS)
	    परत 0;

    // get the DisplayName out of it to show the user
    r = Reg_Query_मूल्य_Ex(किल्ली_handle, "path", 0,
					  &मूल्य_type, (LPBYTE)buf, &bufsize);
    Reg_Close_किल्ली(किल्ली_handle);

    परत (r == ERROR_SUCCESS);
}

/*
 * Check if the "Open With..." menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    स्थिर पूर्णांक
openwith_gvim_path(अक्षर *buf, DWORD bufsize)
{
    HKEY	किल्ली_handle;
    DWORD	मूल्य_type;
    पूर्णांक		r;

    // Open the key where the path to gvim.exe is stored.
    जर (Reg_Open_किल्ली_Ex(HKEY_CLASSES_मूळ,
		"Applications\\gvim.exe\\shell\\edit\\command", 0,
		    किल्ली_WOW64_64_किल्ली | किल्ली_READ, &किल्ली_handle) != ERROR_SUCCESS)
	परत 0;

    // get the DisplayName out of it to show the user
    r = Reg_Query_मूल्य_Ex(किल्ली_handle, "", 0, &मूल्य_type, (LPBYTE)buf, &bufsize);
    Reg_Close_किल्ली(किल्ली_handle);

    परत (r == ERROR_SUCCESS);
}

    स्थिर शून्य
remove_popup(शून्य)
{
    पूर्णांक		fail = 0;
    पूर्णांक		i;
    पूर्णांक		loop = is_64bit_os() ? 2 : 1;
    पूर्णांक		maxfail = loop * 6;
    DWORD	flag;
    HKEY	kh;

    साठी (i = 0; i < loop; i++)
    {
	जर (i == 0)
	    flag = किल्ली_WOW64_32_किल्ली;
	अन्यथा
	    flag = किल्ली_WOW64_64_किल्ली;

	जर (reg_delete_किल्ली(HKEY_CLASSES_मूळ, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}\\InProcServer32", flag) != ERROR_SUCCESS)
	    ++fail;
	जर (reg_delete_किल्ली(HKEY_CLASSES_मूळ, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}", flag) != ERROR_SUCCESS)
	    ++fail;
	जर (reg_delete_किल्ली(HKEY_CLASSES_मूळ, "*\\shellex\\ContextMenuHandlers\\gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	जर (Reg_Open_किल्ली_Ex(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved", 0,
		    flag | किल्ली_ALL_ACCESS, &kh) != ERROR_SUCCESS)
	    ++fail;
	अन्यथा
	{
	    जर (Reg_Delete_मूल्य(kh, "{51EEE242-AD87-11d3-9C1E-0090278BBD99}") != ERROR_SUCCESS)
		++fail;
	    Reg_Close_किल्ली(kh);
	}
	जर (reg_delete_किल्ली(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", flag) != ERROR_SUCCESS)
	    ++fail;
	जर (reg_delete_किल्ली(HKEY_LOCAL_MACHINE, "Software\\Vim", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    जर (fail == maxfail)
	म_लिखो("No Vim popup registry entries could be removed\n");
    अन्यथा जर (fail > 0)
	म_लिखो("Some Vim popup registry entries could not be removed\n");
    अन्यथा
	म_लिखो("The Vim popup registry entries have been removed\n");
}

    स्थिर शून्य
remove_openwith(शून्य)
{
    पूर्णांक		fail = 0;
    पूर्णांक		i;
    पूर्णांक		loop = is_64bit_os() ? 2 : 1;
    पूर्णांक		maxfail = loop * 7;
    DWORD	flag;

    साठी (i = 0; i < loop; i++)
    {
	जर (i == 0)
	    flag = किल्ली_WOW64_32_किल्ली;
	अन्यथा
	    flag = किल्ली_WOW64_64_किल्ली;

	जर (reg_delete_किल्ली(HKEY_CLASSES_मूळ, "Applications\\gvim.exe\\shell\\edit\\command", flag) != ERROR_SUCCESS)
	    ++fail;
	जर (reg_delete_किल्ली(HKEY_CLASSES_मूळ, "Applications\\gvim.exe\\shell\\edit", flag) != ERROR_SUCCESS)
	    ++fail;
	जर (reg_delete_किल्ली(HKEY_CLASSES_मूळ, "Applications\\gvim.exe\\shell", flag) != ERROR_SUCCESS)
	    ++fail;
	जर (reg_delete_किल्ली(HKEY_CLASSES_मूळ, "Applications\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	जर (reg_delete_किल्ली(HKEY_CLASSES_मूळ, ".htm\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	जर (reg_delete_किल्ली(HKEY_CLASSES_मूळ, ".vim\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
	जर (reg_delete_किल्ली(HKEY_CLASSES_मूळ, "*\\OpenWithList\\gvim.exe", flag) != ERROR_SUCCESS)
	    ++fail;
    }

    जर (fail == maxfail)
	म_लिखो("No Vim open-with registry entries could be removed\n");
    अन्यथा जर (fail > 0)
	म_लिखो("Some Vim open-with registry entries could not be removed\n");
    अन्यथा
	म_लिखो("The Vim open-with registry entries have been removed\n");
}

/*
 * Check if a batch file is really for the current version.  Don't delete a
 * batch file that was written for another (possibly newer) version.
 */
    स्थिर पूर्णांक
batfile_thisversion(अक्षर *path)
{
    खाता	*fd;
    अक्षर	ओळ[BUFSIZE];
    पूर्णांक		किल्ली_len = म_माप(VIMBAT_UNINSTKEY);
    पूर्णांक		found = FALSE;

    fd = ख_खोलो(path, "r");
    जर (fd == रिक्त)
	परत FALSE;

    पर्यंत (ख_माला_लो(ओळ, आकार(ओळ), fd) != रिक्त)
    {
	जर (म_भेदन(ओळ, VIMBAT_UNINSTKEY, किल्ली_len) == 0)
	{
	    found = TRUE;
	    थांबा;
	}
    }
    ख_बंद(fd);
    परत found;
}

    स्थिर पूर्णांक
remove_batfiles(पूर्णांक doit)
{
    अक्षर *batfile_path;
    पूर्णांक	 i;
    पूर्णांक	 found = 0;

    // avoid looking in the "installdir" by chdir to system root
    mch_chdir(sysdrive);
    mch_chdir("\\");

    साठी (i = 1; i < TARGET_मोजणी; ++i)
    {
	batfile_path = searchpath_save(targets[i].batname);
	जर (batfile_path != रिक्त && batfile_thisversion(batfile_path))
	{
	    ++found;
	    जर (doit)
	    {
		म_लिखो("removing %s\n", batfile_path);
		हटाओ(batfile_path);
	    }
	    अन्यथा
		म_लिखो(" - the batch file %s\n", batfile_path);
	    मुक्त(batfile_path);
	}
    }

    mch_chdir(installdir);
    परत found;
}

    स्थिर शून्य
remove_if_exists(अक्षर *path, अक्षर *filename)
{
    अक्षर buf[BUFSIZE];
    खाता *fd;

    प्र_लिखो(buf, "%s\\%s", path, filename);

    fd = ख_खोलो(buf, "r");
    जर (fd == रिक्त)
	परत;

    ख_बंद(fd);
    म_लिखो("removing %s\n", buf);
    हटाओ(buf);
}

    स्थिर शून्य
remove_icons(शून्य)
{
    अक्षर	path[BUFSIZE];
    पूर्णांक		i;

    जर (get_shell_folder_path(path, "desktop"))
	साठी (i = 0; i < ICON_मोजणी; ++i)
	    remove_if_exists(path, icon_link_names[i]);
}

    स्थिर शून्य
remove_start_menu(शून्य)
{
    अक्षर	path[BUFSIZE];
    पूर्णांक		i;
    संरचना stat st;

    जर (get_shell_folder_path(path, VIM_STARTMENU) == FAIL)
	परत;

    साठी (i = 1; i < TARGET_मोजणी; ++i)
	remove_if_exists(path, targets[i].lnkname);
    remove_if_exists(path, "uninstall.lnk");
    remove_if_exists(path, "Help.lnk");
    // Win95 uses .pif, WinNT uses .lnk
    remove_if_exists(path, "Vim tutor.pif");
    remove_if_exists(path, "Vim tutor.lnk");
    remove_if_exists(path, "Vim online.url");
    जर (stat(path, &st) == 0)
    {
	म_लिखो("removing %s\n", path);
	सूची_हटाओ(path);
    }
}

    स्थिर शून्य
delete_uninstall_किल्ली(शून्य)
{
    reg_delete_किल्ली(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Vim " VIM_VERSION_SHORT, किल्ली_WOW64_64_किल्ली);
}

    पूर्णांक
main(पूर्णांक argc, अक्षर *argv[])
{
    पूर्णांक		found = 0;
    खाता	*fd;
    पूर्णांक		i;
    संरचना stat st;
    अक्षर	icon[BUFSIZE];
    अक्षर	path[MAX_PATH];
    अक्षर	popup_path[MAX_PATH];

    // The nsis uninstaller calls us with a "-nsis" argument.
    जर (argc == 2 && stricmp(argv[1], "-nsis") == 0)
	interactive = FALSE;
    अन्यथा
	interactive = TRUE;

    // Initialize this program.
    do_inits(argv);

    म_लिखो("This program will remove the following items:\n");

    जर (popup_gvim_path(popup_path, आकार(popup_path)))
    {
	म_लिखो(" - the \"Edit with Vim\" entry in the popup menu\n");
	म_लिखो("   which uses \"%s\"\n", popup_path);
	जर (interactive)
	    म_लिखो("\nRemove it (y/n)? ");
	जर (!interactive || confirm())
	{
	    remove_popup();
	    // Assume the "Open With" entry can be removed as well, don't
	    // bother the user with asking him again.
	    remove_openwith();
	}
    }
    अन्यथा जर (openwith_gvim_path(popup_path, आकार(popup_path)))
    {
	म_लिखो(" - the Vim \"Open With...\" entry in the popup menu\n");
	म_लिखो("   which uses \"%s\"\n", popup_path);
	म_लिखो("\nRemove it (y/n)? ");
	जर (confirm())
	    remove_openwith();
    }

    जर (get_shell_folder_path(path, "desktop"))
    {
	म_लिखो("\n");
	साठी (i = 0; i < ICON_मोजणी; ++i)
	{
	    प्र_लिखो(icon, "%s\\%s", path, icon_link_names[i]);
	    जर (stat(icon, &st) == 0)
	    {
		म_लिखो(" - the \"%s\" icon on the desktop\n", icon_names[i]);
		++found;
	    }
	}
	जर (found > 0)
	{
	    जर (interactive)
		म_लिखो("\nRemove %s (y/n)? ", found > 1 ? "them" : "it");
	    जर (!interactive || confirm())
		remove_icons();
	}
    }

    जर (get_shell_folder_path(path, VIM_STARTMENU)
	    && stat(path, &st) == 0)
    {
	म_लिखो("\n - the \"%s\" entry in the Start Menu\n", VIM_STARTMENU);
	जर (interactive)
	    म_लिखो("\nRemove it (y/n)? ");
	जर (!interactive || confirm())
	    remove_start_menu();
    }

    म_लिखो("\n");
    found = remove_batfiles(0);
    जर (found > 0)
    {
	जर (interactive)
	    म_लिखो("\nRemove %s (y/n)? ", found > 1 ? "them" : "it");
	जर (!interactive || confirm())
	    remove_batfiles(1);
    }

    fd = ख_खोलो("gvim.exe", "r");
    जर (fd != रिक्त)
    {
	ख_बंद(fd);
	म_लिखो("gvim.exe detected.  Attempting to unregister gvim with OLE\n");
	प्रणाली("gvim.exe -silent -unregister");
    }

    delete_uninstall_किल्ली();

    जर (interactive)
    {
	म_लिखो("\nYou may now want to delete the Vim executables and runtime files.\n");
	म_लिखो("(They are still where you unpacked them.)\n");
    }

    जर (interactive)
    {
	शुरुआत(मानक_निवेश);
	म_लिखो("\nPress Enter to exit...");
	(शून्य)अक्षर_लो();
    }
    अन्यथा
	sleep(3);

    परत 0;
}
