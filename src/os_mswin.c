/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * os_mswin.c
 *
 * Routines for Win32.
 */

#అన్తర్భావ "vim.h"

#అన్తర్భావ <sys/types.h>
#అన్తర్భావ <limits.h>

#అన్తర్భావ <process.h>
#అన్తర్భావ <direct.h>

#ఒకవేళ !defined(FEAT_GUI_MSWIN)
# include <shellapi.h>
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT)
# include <dlgs.h>
# include <winspool.h>
# include <commdlg.h>
#అయితే_ముగింపు

// Record all output and all keyboard & mouse input
// #define MCH_WRITE_DUMP

#నిర్వచితమైతే MCH_WRITE_DUMP
FILE* fdDump = ఖాళీ;
#అయితే_ముగింపు

#ఒకవేళ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
బాహ్య అక్షరం g_szOrigTitle[];
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI
బాహ్య HWND s_hwnd;
#లేకపోతే
స్థిర HWND s_hwnd = 0;	    // console window handle, set by GetConsoleHwnd()
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_పని_CHANNEL
పూర్ణాంకం WSInitialized = FALSE; // WinSock is initialized
#అయితే_ముగింపు


/*
 * Save the instance handle of the exe/dll.
 */
    శూన్య
SaveInst(HINSTANCE hInst)
{
    g_hinst = hInst;
}

#ఒకవేళ defined(FEAT_GUI_MSWIN)
/*
 * GUI version of mch_exit().
 * Shut down and exit with status `r'
 * Careful: mch_exit() may be called before mch_init()!
 */
    శూన్య
mch_exit_g(పూర్ణాంకం r)
{
    exiting = TRUE;

    display_errors();

    ml_close_all(TRUE);		// remove all memfiles

# ifdef FEAT_OLE
    UninitOLE();
# endif
# ifdef FEAT_పని_CHANNEL
    ఒకవేళ (WSInitialized)
    {
	WSInitialized = FALSE;
	WSACleanup();
    }
# endif
# ifdef DYNAMIC_GETTEXT
    dyn_libintl_చివర();
# endif

    ఒకవేళ (gui.లోపల_use)
	gui_exit(r);

# ifdef EXITFREE
    free_all_mem();
# endif

    నిష్క్రమించు(r);
}

#అయితే_ముగింపు // FEAT_GUI_MSWIN


/*
 * Get version number including build number
 */
రకనిర్వచనం BOOL (WINAPI *Pfn_Rtl_Get_సంస్కరణ)(LPOSVERSIONINFOW);
DWORD win_సంస్కరణ;
    స్థిర శూన్య
win_సంస్కరణ_init(శూన్య)
{
    OSVERSIONINFOW	osver;
    HMODULE		hNtdll;
    Pfn_Rtl_Get_సంస్కరణ	p_Rtl_Get_సంస్కరణ;

    hNtdll = GetModuleHandle("ntdll.dll");
    ఒకవేళ (hNtdll == ఖాళీ)
	తిరిగి;

    p_Rtl_Get_సంస్కరణ =
	(Pfn_Rtl_Get_సంస్కరణ) Get_Proc_చిరునామా(hNtdll, "RtlGetVersion");
    ఒకవేళ (p_Rtl_Get_సంస్కరణ == ఖాళీ)
	తిరిగి;

    osver.dw_OSVersion_Info_కొలత = పరిమాణం(OSVERSIONINFOW);
    p_Rtl_Get_సంస్కరణ(&osver);
    win_సంస్కరణ = MAKE_VER(min(osver.dw_Major_సంస్కరణ, 0xFF),
	    min(osver.dw_Minor_సంస్కరణ, 0xFF),
	    min(osver.dw_Build_సంఖ్య, 0xFFFF));
}

/*
 * Init the tables for toupper() and tolower().
 */
    శూన్య
mch_early_init(శూన్య)
{
    పూర్ణాంకం		i;

    win_సంస్కరణ_init();

    PlatformId();

    // Init the tables for toupper() and tolower()
    కొరకు (i = 0; i < 256; ++i)
	toupper_tab[i] = tolower_tab[i] = i;
    Char_పై_Buff((LPSTR)toupper_tab, 256);
    Char_క్రింది_Buff((LPSTR)tolower_tab, 256);
}


/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    పూర్ణాంకం
mch_ఆదానం_isatty(శూన్య)
{
#నిర్వచితమైతే FEAT_GUI_MSWIN
# ifdef VIMDLL
    ఒకవేళ (gui.లోపల_use)
# endif
	తిరిగి TRUE;	    // GUI always has a tty
#అయితే_ముగింపు
#ఒకవేళ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    ఒకవేళ (isatty(read_cmd_fd))
	తిరిగి TRUE;
    తిరిగి FALSE;
#అయితే_ముగింపు
}

/*
 * mch_settitle(): set titlebar of our window
 */
    శూన్య
mch_settitle(
    char_u *title,
    char_u *icon వాడని)
{
#నిర్వచితమైతే FEAT_GUI_MSWIN
# ifdef VIMDLL
    ఒకవేళ (gui.లోపల_use)
# endif
    {
	gui_mch_settitle(title, icon);
	తిరిగి;
    }
#అయితే_ముగింపు
#ఒకవేళ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    ఒకవేళ (title != ఖాళీ)
    {
	WCHAR	*wp = enc_గమ్యం_utf16(title, ఖాళీ);

	ఒకవేళ (wp == ఖాళీ)
	    తిరిగి;

	సమితి_Console_Title_W(wp);
	vim_free(wp);
	తిరిగి;
    }
#అయితే_ముగింపు
}


/*
 * Restore the window/icon title.
 * which is one of:
 *  SAVE_RESTORE_TITLE: Just restore title
 *  SAVE_RESTORE_ICON:  Just restore icon (which we don't have)
 *  SAVE_RESTORE_BOTH:  Restore title and icon (which we don't have)
 */
    శూన్య
mch_restore_title(పూర్ణాంకం which వాడని)
{
#ఒకవేళ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
    ఒకవేళ (!gui.లోపల_use)
# endif
	సమితి_Console_Title(g_szOrigTitle);
#అయితే_ముగింపు
}


/*
 * Return TRUE if we can restore the title (we can)
 */
    పూర్ణాంకం
mch_can_restore_title(శూన్య)
{
    తిరిగి TRUE;
}


/*
 * Return TRUE if we can restore the icon title (we can't)
 */
    పూర్ణాంకం
mch_can_restore_icon(శూన్య)
{
    తిరిగి FALSE;
}


/*
 * Get absolute file name into buffer "buf" of length "len" bytes,
 * turning all '/'s into '\\'s and getting the correct case of each component
 * of the file name.  Append a (back)slash to a directory name.
 * When 'shellslash' set do it the other way around.
 * Return OK or FAIL.
 */
    పూర్ణాంకం
mch_నిండిన_Name(
    char_u	*fname,
    char_u	*buf,
    పూర్ణాంకం		len,
    పూర్ణాంకం		force వాడని)
{
    పూర్ణాంకం		n_ఫలితం = FAIL;
    WCHAR	*wname;
    WCHAR	wbuf[MAX_మార్గం];
    char_u	*cname = ఖాళీ;

    wname = enc_గమ్యం_utf16(fname, ఖాళీ);
    ఒకవేళ (wname != ఖాళీ && _wfullpath(wbuf, wname, MAX_మార్గం) != ఖాళీ)
    {
	cname = utf16_గమ్యం_enc((short_u *)wbuf, ఖాళీ);
	ఒకవేళ (cname != ఖాళీ)
	{
	    vim_strncpy(buf, cname, len - 1);
	    n_ఫలితం = OK;
	}
    }
    vim_free(wname);
    vim_free(cname);

#నిర్వచితమైతే USE_FNAME_CASE
    fname_case(buf, len);
#లేకపోతే
    slash_adjust(buf);
#అయితే_ముగింపు

    తిరిగి n_ఫలితం;
}


/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    పూర్ణాంకం
mch_is_నిండిన_Name(char_u *fname)
{
    // A name like "d:/foo" and "//server/share" is absolute.  "d:foo" is not.
    // /foo and \foo are absolute too because Windows keeps a current drive.
    // Another way to check is to use mch_FullName() and see if the result is
    // the same as the name or mch_FullName() fails.  However, this has quite a
    // bit of overhead, so let's not do that.
    ఒకవేళ (*fname == NUL)
	తిరిగి FALSE;
    తిరిగి ((ASCII_ISALPHA(fname[0]) && fname[1] == ':'
				      && (fname[2] == '/' || fname[2] == '\\'))
	    || (fname[0] == '/' || fname[0] == '\\'));
}

/*
 * Replace all slashes by backslashes.
 * This used to be the other way around, but MS-DOS sometimes has problems
 * with slashes (e.g. in a command name).  We can't have mixed slashes and
 * backslashes, because comparing file names will not work correctly.  The
 * commands that use a file name should try to avoid the need to type a
 * backslash twice.
 * When 'shellslash' set do it the other way around.
 * When the path looks like a URL leave it unmodified.
 */
    శూన్య
slash_adjust(char_u *p)
{
    ఒకవేళ (మార్గం_with_url(p))
	తిరిగి;

    ఒకవేళ (*p == '`')
    {
	size_t len = STRLEN(p);

	// don't replace backslash in backtick quoted strings
	ఒకవేళ (len > 2 && *(p + len - 1) == '`')
	    తిరిగి;
    }

    అయితే (*p)
    {
	ఒకవేళ (*p == psepcN)
	    *p = psepc;
	MB_PTR_ADV(p);
    }
}

    స్థిర పూర్ణాంకం
read_reparse_బిందువు(స్థిరరాశి WCHAR *name, char_u *buf, DWORD *buf_len)
{
    HANDLE h;
    BOOL ok;

    h = CreateFileW(name, FILE_READ_లక్షణాలు,
	    FILE_SHARE_READ | FILE_SHARE_WRITE, ఖాళీ,
	    OPEN_EXISTING,
	    FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_బిందువు,
	    ఖాళీ);
    ఒకవేళ (h == INVALID_HANDLE_విలువ)
	తిరిగి FAIL;

    ok = DeviceIoControl(h, FSCTL_GET_REPARSE_బిందువు, ఖాళీ, 0, buf, *buf_len,
	    buf_len, ఖాళీ);
    CloseHandle(h);

    తిరిగి ok ? OK : FAIL;
}

    char_u *
resolve_appexeclink(char_u *fname)
{
    DWORD		attr = 0;
    పూర్ణాంకం			idx;
    WCHAR		*p, *చివర, *wname;
    // The buffer size is arbitrarily chosen to be "big enough" (TM), the
    // ceiling should be around 16k.
    char_u		buf[4096];
    DWORD		buf_len = పరిమాణం(buf);
    REPARSE_దత్తాంశం_బఫరు *rb = (REPARSE_దత్తాంశం_బఫరు *)buf;

    wname = enc_గమ్యం_utf16(fname, ఖాళీ);
    ఒకవేళ (wname == ఖాళీ)
	తిరిగి ఖాళీ;

    attr = Get_File_లక్షణాలు_W(wname);
    ఒకవేళ (attr == INVALID_FILE_లక్షణాలు ||
	    (attr & FILE_ATTRIBUTE_REPARSE_బిందువు) == 0)
    {
	vim_free(wname);
	తిరిగి ఖాళీ;
    }

    // The applinks are similar to symlinks but with a huge difference: they can
    // only be executed, any other I/O operation on them is bound to fail with
    // ERROR_FILE_NOT_FOUND even though the file exists.
    ఒకవేళ (read_reparse_బిందువు(wname, buf, &buf_len) == FAIL)
    {
	vim_free(wname);
	తిరిగి ఖాళీ;
    }
    vim_free(wname);

    ఒకవేళ (rb->Reparse_ట్యాగ్ != IO_REPARSE_ట్యాగ్_APPEXECLINK)
	తిరిగి ఖాళీ;

    // The (undocumented) reparse buffer contains a set of N null-terminated
    // Unicode strings, the application path is stored in the third one.
    ఒకవేళ (rb->App_Exec_Link_Reparse_బఫరు.String_లెక్క < 3)
	తిరిగి ఖాళీ;

    p = rb->App_Exec_Link_Reparse_బఫరు.StringList;
    చివర = p + rb->Reparse_దత్తాంశం_నిడివి / పరిమాణం(WCHAR);
    కొరకు (idx = 0; p < చివర
	    && idx < (పూర్ణాంకం)rb->App_Exec_Link_Reparse_బఫరు.String_లెక్క
	    && idx != 2; )
    {
	ఒకవేళ (*p++ == L'\0')
	    ++idx;
    }

    తిరిగి utf16_గమ్యం_enc(p, ఖాళీ);
}

// Use 64-bit stat functions.
#నిర్వచనం_రద్దు stat
#నిర్వచనం_రద్దు _stat
#నిర్వచనం_రద్దు _wstat
#నిర్వచనం_రద్దు _fstat
#నిర్వచించు stat _stat64
#నిర్వచించు _stat _stat64
#నిర్వచించు _wstat _wstat64
#నిర్వచించు _fstat _fstat64

/*
 * Implements lstat() and stat() that can handle symlinks properly.
 */
    స్థిర పూర్ణాంకం
mswin_stat_impl(స్థిరరాశి WCHAR *name, stat_T *stp, స్థిరరాశి పూర్ణాంకం resolve)
{
    పూర్ణాంకం			n;
    పూర్ణాంకం			fd;
    BOOL		is_symlink = FALSE;
    HANDLE		h_కనుగొను, h;
    DWORD		attr = 0;
    DWORD		flag = 0;
    WIN32_కనుగొను_DATAW    కనుగొను_దత్తాంశం_W;

#ఒకవేళ 0 && defined(_UCRT)
    // This code was disabled because the behavior of MSVC's _wstat (actually
    // _wstat64) for empty symlinks varies depending on the C runtime you link
    // to.
    //
    // The expected behavior here is for _wstat() to fail for empty symlinks.
    // The expected behavior occurs when linking to a static runtime.  However,
    // the expected behavior does not occur when linking to a dynamic runtime,
    // and it succeeds for empty symlinks.  This causes Test_glob_symlinks in
    // test_functions.vim to fail when linking to a dynamic runtime.
    //
    // For more details, see:
    // https://github.com/koron/vc-stat-behavior-verification
    ఒకవేళ (resolve)
	// Universal CRT can handle symlinks properly.
	తిరిగి _wstat(name, stp);
#అయితే_ముగింపు

    // Only reparse points need a directory query to identify symbolic links.
    // In particular, avoid enumerating the directory for each missing file
    // while searching 'runtimepath'.
    attr = Get_File_లక్షణాలు_W(name);
    ఒకవేళ (attr == INVALID_FILE_లక్షణాలు)
    {
	DWORD error = Get_చివరి_Error();

	// A missing file or path cannot provide stat information.  Other
	// errors and UNC paths still need the CRT fallback.
	ఒకవేళ ((error == ERROR_FILE_NOT_దొరికింది || error == ERROR_మార్గం_NOT_దొరికింది)
		&& !((name[0] == L'\\' || name[0] == L'/')
		    && (name[1] == L'\\' || name[1] == L'/')))
	{
	    errno = ENOENT;
	    తిరిగి -1;
	}
	తిరిగి _wstat(name, stp);
    }
    ఒకవేళ (!(attr & FILE_ATTRIBUTE_REPARSE_బిందువు))
	తిరిగి _wstat(name, stp);

    h_కనుగొను = కనుగొను_మొదటి_File_W(name, &కనుగొను_దత్తాంశం_W);
    ఒకవేళ (h_కనుగొను != INVALID_HANDLE_విలువ)
    {
	attr = కనుగొను_దత్తాంశం_W.dw_File_లక్షణాలు;
	ఒకవేళ ((attr & FILE_ATTRIBUTE_REPARSE_బిందువు)
		&& (కనుగొను_దత్తాంశం_W.dwReserved0 == IO_REPARSE_ట్యాగ్_SYMLINK))
	    is_symlink = TRUE;
	కనుగొను_Close(h_కనుగొను);
    }

    // Use the plain old stat() whenever it's possible.
    ఒకవేళ (!is_symlink)
	తిరిగి _wstat(name, stp);

    ఒకవేళ (!resolve && is_symlink)
	flag = FILE_FLAG_OPEN_REPARSE_బిందువు;
    ఒకవేళ (attr & FILE_ATTRIBUTE_DIRECTORY)
	flag |= FILE_FLAG_BACKUP_SEMANTICS;

    h = CreateFileW(name, FILE_READ_లక్షణాలు,
	    FILE_SHARE_READ | FILE_SHARE_WRITE, ఖాళీ, OPEN_EXISTING, flag,
	    ఖాళీ);
    ఒకవేళ (h == INVALID_HANDLE_విలువ)
	తిరిగి -1;

    fd = _open_osfhandle((intptr_t)h, _O_RDONLY);
    n = _fstat(fd, (నిర్మాణం _stat *)stp);
    ఒకవేళ ((n == 0) && (attr & FILE_ATTRIBUTE_DIRECTORY))
	stp->st_బహుళకం = (stp->st_బహుళకం & ~S_IFMT) | S_IFDIR;
    _close(fd);

    తిరిగి n;
}

/*
 * stat() can't handle a trailing '/' or '\', remove it first.
 * When 'resolve' is true behave as lstat() wrt symlinks.
 */
    స్థిర పూర్ణాంకం
stat_impl(స్థిరరాశి అక్షరం *name, stat_T *stp, స్థిరరాశి పూర్ణాంకం resolve)
{
    // WinNT and later can use _MAX_PATH wide characters for a pathname, which
    // means that the maximum pathname is _MAX_PATH * 3 bytes when 'enc' is
    // UTF-8.
    char_u	buf[MAX_మార్గం_2 * 3 + 1];
    size_t	buflen;
    char_u	*p;
    WCHAR	*wp;
    పూర్ణాంకం		n;

    vim_strncpy((char_u *)buf, (char_u *)name, పరిమాణం(buf) - 1);
    buflen = STRLEN(buf);
    p = buf + buflen;
    ఒకవేళ (p > buf)
	MB_PTR_వెనుక(buf, p);

    // Remove trailing '\\' except root path.
    ఒకవేళ (p > buf && (*p == '\\' || *p == '/') && p[-1] != ':')
    {
	*p = NUL;
	buflen = (size_t)(p - buf);
    }

    ఒకవేళ ((buf[0] == '\\' && buf[1] == '\\') || (buf[0] == '/' && buf[1] == '/'))
    {
	// UNC root path must be followed by '\\'.
	p = vim_strpbrk(buf + 2, (char_u *)"\\/");
	ఒకవేళ (p != ఖాళీ)
	{
	    p = vim_strpbrk(p + 1, (char_u *)"\\/");
	    ఒకవేళ (p == ఖాళీ)
		STRCPY(buf + buflen, "\\");
	}
    }

    wp = enc_గమ్యం_utf16(buf, ఖాళీ);
    ఒకవేళ (wp == ఖాళీ)
	తిరిగి -1;

    n = mswin_stat_impl(wp, stp, resolve);
    vim_free(wp);
    తిరిగి n;
}

    పూర్ణాంకం
vim_lstat(స్థిరరాశి అక్షరం *name, stat_T *stp)
{
    తిరిగి stat_impl(name, stp, FALSE);
}

    పూర్ణాంకం
vim_stat(స్థిరరాశి అక్షరం *name, stat_T *stp)
{
    తిరిగి stat_impl(name, stp, TRUE);
}

#ఒకవేళ defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    శూన్య
mch_settmode(tmode_T tmode వాడని)
{
    // nothing to do
}

    పూర్ణాంకం
mch_get_shellsize(శూన్య)
{
    // never used
    తిరిగి OK;
}

    శూన్య
mch_సమితి_shellsize(శూన్య)
{
    // never used
}

/*
 * Rows and/or Columns has changed.
 */
    శూన్య
mch_new_shellsize(శూన్య)
{
    // never used
}

    శూన్య
mch_calc_cell_కొలత(నిర్మాణం cellsize *cs_బహిర్గతం వాడని)
{
    // never used
}

#అయితే_ముగింపు

/*
 * We have no job control, so fake it by starting a new shell.
 */
    శూన్య
mch_suspend(శూన్య)
{
    suspend_shell();
}

#ఒకవేళ defined(USE_MCH_ERRMSG)

# ifdef display_errors
#  undef display_errors
# endif

/*
 * Display the saved error message(s).
 */
    శూన్య
display_errors(శూన్య)
{
# ifdef FEAT_GUI
    char_u *p;

#  ifdef VIMDLL
    ఒకవేళ (gui.లోపల_use || gui.starting)
#  endif
    {
	ఒకవేళ (error_ga.ga_దత్తాంశం != ఖాళీ)
	{
	    // avoid putting up a message box with blanks only
	    కొరకు (p = (char_u *)error_ga.ga_దత్తాంశం; *p; ++p)
		ఒకవేళ (!సురక్షిత_isspace(*p))
		{
		    // Only use a dialog when not using --gui-dialog-file:
		    // write text to a file.
		    ఒకవేళ (!gui_dialog_log((char_u *)"Errors", p))
			(శూన్య)gui_mch_dialog(
				     gui.starting ? VIM_INFO :
					     VIM_ERROR,
				     gui.starting ? (char_u *)_("Message") :
					     (char_u *)_("Error"),
					     p, (char_u *)_("&Ok"),
					1, ఖాళీ, FALSE);
		    ఆపు;
		}
	    ga_clear(&error_ga);
	}
	తిరిగి;
    }
# endif
# ఒకవేళ !defined(FEAT_GUI) || defined(VIMDLL)
    FlushFileBuffers(GetStdHandle(STD_ERROR_HANDLE));
# endif
}
#అయితే_ముగింపు


/*
 * Return TRUE if "p" contain a wildcard that can be expanded by
 * dos_expandpath().
 */
    పూర్ణాంకం
mch_has_exp_wildcard(char_u *p)
{
    కొరకు ( ; *p; MB_PTR_ADV(p))
    {
	ఒకవేళ (vim_strchr((char_u *)"?*[", *p) != ఖాళీ
		|| (*p == '~' && p[1] != NUL))
	    తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * Return TRUE if "p" contain a wildcard or a "~1" kind of thing (could be a
 * shortened file name).
 */
    పూర్ణాంకం
mch_has_wildcard(char_u *p)
{
    కొరకు ( ; *p; MB_PTR_ADV(p))
    {
	ఒకవేళ (vim_strchr((char_u *)"?*$[`", *p) != ఖాళీ
		|| (*p == '~' && p[1] != NUL))
	    తిరిగి TRUE;
    }
    తిరిగి FALSE;
}


/*
 * The normal _chdir() does not change the default drive.  This one does.
 * Returning 0 implies success; -1 implies failure.
 */
    పూర్ణాంకం
mch_chdir(అక్షరం *మార్గం)
{
    WCHAR   *p;
    పూర్ణాంకం	    n;

    ఒకవేళ (మార్గం[0] == NUL)		// just checking...
	తిరిగి -1;

    ఒకవేళ (p_verbose >= 5)
    {
	verbose_enter();
	smsg("chdir(%s)", మార్గం);
	verbose_leave();
    }
    ఒకవేళ (సురక్షిత_isalpha(మార్గం[0]) && మార్గం[1] == ':')	// has a drive name
    {
	// If we can change to the drive, skip that part of the path.  If we
	// can't then the current directory may be invalid, try using chdir()
	// with the whole path.
	ఒకవేళ (_chdrive(TOLOWER_ASC(మార్గం[0]) - 'a' + 1) == 0)
	    మార్గం += 2;
    }

    ఒకవేళ (*మార్గం == NUL)		// drive name only
	తిరిగి 0;

    p = enc_గమ్యం_utf16((char_u *)మార్గం, ఖాళీ);
    ఒకవేళ (p == ఖాళీ)
	తిరిగి -1;

    n = _wchdir(p);
    vim_free(p);
    తిరిగి n;
}


#ఒకవేళ defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
/*
 * return non-zero if a character is available
 */
    పూర్ణాంకం
mch_char_avail(శూన్య)
{
    // never used
    తిరిగి TRUE;
}

# ఒకవేళ defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    పూర్ణాంకం
mch_check_messages(శూన్య)
{
    // TODO: check for messages
    తిరిగి TRUE;
}
# endif
#అయితే_ముగింపు


#ఒకవేళ defined(FEAT_LIBCALL)
/*
 * Call a DLL routine which takes either a string or int param
 * and returns an allocated string.
 * Return OK if it worked, FAIL if not.
 */
రకనిర్వచనం char_u *(*MYSTRPROCSTR)(char_u *);
రకనిర్వచనం పూర్ణాంకం (*MYSTRPROCINT)(char_u *);
రకనిర్వచనం char_u *(*MYINTPROCSTR)(పూర్ణాంకం);
రకనిర్వచనం పూర్ణాంకం (*MYINTPROCINT)(పూర్ణాంకం);

/*
 * Check if a pointer points to a valid NUL terminated string.
 * Return the length of the string, including terminating NUL.
 * Returns 0 for an invalid pointer, 1 for an empty string.
 */
    స్థిర size_t
check_str_len(char_u *str)
{
    SYSTEM_INFO			si;
    జ్ఞాపకం_BASIC_INFORMATION	mbi;
    size_t			నిడివి = 0;
    size_t			i;
    స్థిరరాశి char_u		*p;

    // get page size
    GetSystemInfo(&si);

    // get memory information
    ఒకవేళ (!VirtualQuery(str, &mbi, పరిమాణం(mbi)))
	తిరిగి 0;

    // pre cast these (typing savers)
    long_u dwStr = (long_u)str;
    long_u dw_పునాది_చిరునామా = (long_u)mbi.పునాది_చిరునామా;

    // get start address of page that str is on
    long_u str_పుట = dwStr - (dwStr - dw_పునాది_చిరునామా) % si.dw_పుట_కొలత;

    // get length from str to end of page
    long_u పుట_నిడివి = si.dw_పుట_కొలత - (dwStr - str_పుట);

    కొరకు (p = str; !IsBadReadPtr(p, (UINT)పుట_నిడివి);
	    p += పుట_నిడివి, పుట_నిడివి = si.dw_పుట_కొలత)
	కొరకు (i = 0; i < పుట_నిడివి; ++i, ++నిడివి)
	    ఒకవేళ (p[i] == NUL)
		తిరిగి నిడివి + 1;

    తిరిగి 0;
}

/*
 * Passed to do_in_runtimepath() to load a vim.ico file.
 */
    స్థిర శూన్య
mch_icon_ఎక్కించు_cb(char_u *fname, శూన్య *cookie)
{
    HANDLE *h = (HANDLE *)cookie;

    *h = ఎక్కించు_Image(ఖాళీ,
		   (LPSTR)fname,
		   IMAGE_ICON,
		   64,
		   64,
		   LR_LOADFROMFILE | LR_LOADMAP3DCOLORS);
}

/*
 * Try loading an icon file from 'runtimepath'.
 */
    పూర్ణాంకం
mch_icon_ఎక్కించు(HANDLE *iconp)
{
    తిరిగి do_లోపల_runtimepath((char_u *)"bitmaps/vim.ico",
						  0, mch_icon_ఎక్కించు_cb, iconp);
}

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if the OS PRNG is not available or something went wrong.
 */
    పూర్ణాంకం
mch_get_random(char_u *buf, పూర్ణాంకం len)
{
    స్థిర పూర్ణాంకం		initialized = NOTDONE;
    స్థిర HINSTANCE	hInstLib;
    స్థిర BOOL (WINAPI *pProcessPrng)(PUCHAR, ULONG);

    ఒకవేళ (initialized == NOTDONE)
    {
	hInstLib = vim_ఎక్కించు_Lib("bcryptprimitives.dll");
	ఒకవేళ (hInstLib != ఖాళీ)
	    pProcessPrng = (శూన్య *)Get_Proc_చిరునామా(hInstLib, "ProcessPrng");
	ఒకవేళ (hInstLib == ఖాళీ || pProcessPrng == ఖాళీ)
	{
	    FreeLibrary(hInstLib);
	    initialized = FAIL;
	}
	లేకపోతే
	    initialized = OK;
    }

    ఒకవేళ (initialized == FAIL)
	తిరిగి FAIL;

    // According to the documentation this call cannot fail.
    pProcessPrng(buf, len);

    తిరిగి OK;
}

    పూర్ణాంకం
mch_libcall(
    char_u	*libname,
    char_u	*funcname,
    char_u	*argstring,	// NULL when using a argint
    పూర్ణాంకం		argint,
    char_u	**string_ఫలితం,// NULL when using number_result
    పూర్ణాంకం		*సంఖ్య_ఫలితం)
{
    HINSTANCE		hinstLib;

    BOOL f_నిరంతరం_సమయం_Link_Success = FALSE;

    // Get a handle to the DLL module.
    hinstLib = vim_ఎక్కించు_Lib((అక్షరం *)libname);

    // If the handle is valid, try to get the function address.
    ఒకవేళ (hinstLib != ఖాళీ)
    {
	char_u	*retval_str = ఖాళీ;
	పూర్ణాంకం	retval_int = 0;
	size_t	len;

# ifdef HAVE_TRY_EXCEPT
	__try
	{
# endif
	ఒకవేళ (argstring != ఖాళీ)
	{
	    // Call with string argument
	    ఒకవేళ (string_ఫలితం != ఖాళీ)
	    {
		MYSTRPROCSTR proc;

		proc = (MYSTRPROCSTR)Get_Proc_చిరునామా(hinstLib, (LPCSTR)funcname);
		ఒకవేళ (proc != ఖాళీ)
		{
		    f_నిరంతరం_సమయం_Link_Success = TRUE;
		    retval_str = proc(argstring);
		}
	    }
	    లేకపోతే
	    {
		MYSTRPROCINT proc;

		proc = (MYSTRPROCINT)Get_Proc_చిరునామా(hinstLib, (LPCSTR)funcname);
		ఒకవేళ (proc != ఖాళీ)
		{
		    f_నిరంతరం_సమయం_Link_Success = TRUE;
		    retval_int = proc(argstring);
		}
	    }
	}
	లేకపోతే
	{
	    // Call with number argument
	    ఒకవేళ (string_ఫలితం != ఖాళీ)
	    {
		MYINTPROCSTR proc;

		proc = (MYINTPROCSTR)Get_Proc_చిరునామా(hinstLib, (LPCSTR)funcname);
		ఒకవేళ (proc != ఖాళీ)
		{
		    f_నిరంతరం_సమయం_Link_Success = TRUE;
		    retval_str = proc(argint);
		}
	    }
	    లేకపోతే
	    {
		MYINTPROCINT proc;

		proc = (MYINTPROCINT)Get_Proc_చిరునామా(hinstLib, (LPCSTR)funcname);
		ఒకవేళ (proc != ఖాళీ)
		{
		    f_నిరంతరం_సమయం_Link_Success = TRUE;
		    retval_int = proc(argint);
		}
	    }
	}

	// Save the string before we free the library.
	// Assume that a "1" result is an illegal pointer.
	ఒకవేళ (string_ఫలితం == ఖాళీ)
	    *సంఖ్య_ఫలితం = retval_int;
	లేకపోతే ఒకవేళ (retval_str != ఖాళీ
		&& (len = check_str_len(retval_str)) > 0)
	{
	    *string_ఫలితం = alloc(len);
	    ఒకవేళ (*string_ఫలితం != ఖాళీ)
		mch_memmove(*string_ఫలితం, retval_str, len);
	}

# ifdef HAVE_TRY_EXCEPT
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	    ఒకవేళ (Get_Exception_సంకేతం() == EXCEPTION_దొంతర_అధికప్రవాహం)
		_resetstkoflw();
	    f_నిరంతరం_సమయం_Link_Success = FALSE;
	}
# endif

	// Free the DLL module.
	(శూన్య)FreeLibrary(hinstLib);
    }

    ఒకవేళ (!f_నిరంతరం_సమయం_Link_Success)
    {
	semsg(_(e_library_call_failed_for_str), funcname);
	తిరిగి FAIL;
    }

    తిరిగి OK;
}
#అయితే_ముగింపు

/*
 * Debugging helper: expose the MCH_WRITE_DUMP stuff to other modules
 */
    శూన్య
DumpPutS(స్థిరరాశి అక్షరం *psz వాడని)
{
#నిర్వచితమైతే MCH_WRITE_DUMP
    ఒకవేళ (fdDump)
    {
	fputs(psz, fdDump);
	ఒకవేళ (psz[అక్షరశ్రేణి_పొడవు(psz) - 1] != '\n')
	    fputc('\n', fdDump);
	fflush(fdDump);
    }
#అయితే_ముగింపు
}

#నిర్వచితమైతే _DEBUG

శూన్య __cdecl
Trace(
    అక్షరం *pszFormat,
    ...)
{
    CHAR szBuff[2048];
    va_list వాదనలు;

    va_start(వాదనలు, pszFormat);
    vsprintf(szBuff, pszFormat, వాదనలు);
    va_end(వాదనలు);

    OutputDebugString(szBuff);
}

#అయితే_ముగింపు //_DEBUG

#ఒకవేళ !defined(FEAT_GUI) || defined(VIMDLL)
బాహ్య HWND g_hWnd;	// This is in os_win32.c.

/*
 * Showing the printer dialog is tricky since we have no GUI
 * window to parent it. The following routines are needed to
 * get the window parenting and Z-order to work properly.
 */
    స్థిర శూన్య
GetConsoleHwnd(శూన్య)
{
    // Skip if it's already set.
    ఒకవేళ (s_hwnd != 0)
	తిరిగి;

    // Window handle may have been found by init code (Windows NT only)
    ఒకవేళ (g_hWnd != 0)
    {
	s_hwnd = g_hWnd;
	తిరిగి;
    }

    s_hwnd = Get_Console_కిటికీ();
}

/*
 * Console implementation of ":winpos".
 */
    పూర్ణాంకం
mch_get_winpos(పూర్ణాంకం *x, పూర్ణాంకం *y)
{
    RECT  rect;

    GetConsoleHwnd();
    Get_కిటికీ_Rect(s_hwnd, &rect);
    *x = rect.ఎడమ_2;
    *y = rect.శిఖరం;
    తిరిగి OK;
}

/*
 * Console implementation of ":winpos x y".
 */
    శూన్య
mch_సమితి_winpos(పూర్ణాంకం x, పూర్ణాంకం y)
{
    GetConsoleHwnd();
    సమితి_కిటికీ_Pos(s_hwnd, ఖాళీ, x, y, 0, 0,
		 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT)

//=================================================================
// Win32 printer stuff

స్థిర HFONT		prt_font_handles[2][2][2];
స్థిర PRINTDLGW	prt_dlg;
స్థిర స్థిరరాశి పూర్ణాంకం	boldface[2] = {FW_REGULAR, FW_BOLD};
స్థిర TEXTMETRIC	prt_tm;
స్థిర పూర్ణాంకం		prt_పంక్తి_ఎత్తు;
స్థిర పూర్ణాంకం		prt_సంఖ్య_వెడల్పు;
స్థిర పూర్ణాంకం		prt_ఎడమ_margin;
స్థిర పూర్ణాంకం		prt_కుడి_margin;
స్థిర పూర్ణాంకం		prt_శిఖరం_margin;
స్థిర char_u		szAppName[] = పాఠ్యం_3("VIM");
స్థిర HWND		hDlgPrint;
స్థిర పూర్ణాంకం		*bUserAbort = ఖాళీ;
స్థిర char_u		*prt_name = ఖాళీ;

// Defines which are also in vim.rc.
# define IDC_BOX1		400
# define IDC_PRINTTEXT1		401
# define IDC_PRINTTEXT2		402
# define IDC_పురోగతి		403

    స్థిర BOOL
vim_సమితి_Dlg_అంశం_పాఠ్యం(HWND hDlg, పూర్ణాంకం n_IDDlg_అంశం, char_u *s)
{
    WCHAR   *wp;
    BOOL    ret;

    wp = enc_గమ్యం_utf16(s, ఖాళీ);
    ఒకవేళ (wp == ఖాళీ)
	తిరిగి FALSE;

    ret = సమితి_Dlg_అంశం_పాఠ్యం_W(hDlg, n_IDDlg_అంశం, wp);
    vim_free(wp);
    తిరిగి ret;
}

/*
 * Convert BGR to RGB for Windows GDI calls
 */
    స్థిర COLORREF
మార్పిడి_me(COLORREF colorref)
{
    పూర్ణాంకం  తాత్కాలిక;
    అక్షరం *ptr = (అక్షరం *)&colorref;

    తాత్కాలిక = *(ptr);
    *(ptr ) = *(ptr + 2);
    *(ptr + 2) = తాత్కాలిక;
    తిరిగి colorref;
}

    స్థిర INT_PTR CALLBACK
PrintDlgProc(
	HWND hDlg,
	UINT సందేశం_2,
	WPARAM wParam వాడని,
	LPARAM lParam వాడని)
{
# ifdef FEAT_GETTEXT
    NONCLIENTMETRICS nm;
    స్థిర HFONT hfont;
# endif

    ఎంపిక (సందేశం_2)
    {
	స్థితి WM_INITDIALOG:
# ifdef FEAT_GETTEXT
	    nm.cb_కొలత = పరిమాణం(NONCLIENTMETRICS);
	    ఒకవేళ (SystemParametersInfo(
			SPI_GETNONCLIENTMETRICS,
			పరిమాణం(NONCLIENTMETRICS),
			&nm,
			0))
	    {
		అక్షరం buff[MAX_మార్గం];
		పూర్ణాంకం i;

		// Translate the dialog texts
		hfont = Create_Font_పరోక్ష(&nm.lf_సందేశం_Font);
		కొరకు (i = IDC_PRINTTEXT1; i <= IDC_పురోగతి; i++)
		{
		    Send_Dlg_అంశం_సందేశం(hDlg, i, WM_SETFONT, (WPARAM)hfont, 1);
		    ఒకవేళ (Get_Dlg_అంశం_పాఠ్యం(hDlg,i, buff, పరిమాణం(buff)))
			vim_సమితి_Dlg_అంశం_పాఠ్యం(hDlg,i, (char_u *)_(buff));
		}
		Send_Dlg_అంశం_సందేశం(hDlg, IDCANCEL,
						WM_SETFONT, (WPARAM)hfont, 1);
		ఒకవేళ (Get_Dlg_అంశం_పాఠ్యం(hDlg,IDCANCEL, buff, పరిమాణం(buff)))
		    vim_సమితి_Dlg_అంశం_పాఠ్యం(hDlg,IDCANCEL, (char_u *)_(buff));
	    }
# endif
	    సమితి_కిటికీ_పాఠ్యం(hDlg, (LPCSTR)szAppName);
	    ఒకవేళ (prt_name != ఖాళీ)
	    {
		vim_సమితి_Dlg_అంశం_పాఠ్యం(hDlg, IDC_PRINTTEXT2, (char_u *)prt_name);
		VIM_CLEAR(prt_name);
	    }
	    Enable_Menu_అంశం(GetSystemMenu(hDlg, FALSE), SC_CLOSE, MF_GRAYED);
# ఒకవేళ !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	    ఒకవేళ (!gui.లోపల_use)
#  endif
		Bring_కిటికీ_గమ్యం_శిఖరం(s_hwnd);
# endif
	    తిరిగి TRUE;

	స్థితి WM_COMMAND:
	    *bUserAbort = TRUE;
	    Enable_కిటికీ(Get_మాతృ(hDlg), TRUE);
	    Destroy_కిటికీ(hDlg);
	    hDlgPrint = ఖాళీ;
# ifdef FEAT_GETTEXT
	    DeleteObject(hfont);
# endif
	    తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

    స్థిర BOOL CALLBACK
AbortProc(HDC hdcPrn వాడని, పూర్ణాంకం i_సంకేతం వాడని)
{
    సందేశం_3 సందేశం;

    అయితే (!*bUserAbort && Peek_సందేశం_W(&సందేశం, ఖాళీ, 0, 0, PM_REMOVE))
    {
	ఒకవేళ (!hDlgPrint || !Is_Dialog_సందేశం_W(hDlgPrint, &సందేశం))
	{
	    Translate_సందేశం(&సందేశం);
	    Dispatch_సందేశం_W(&సందేశం);
	}
    }
    తిరిగి !*bUserAbort;
}

    స్థిర UINT_PTR CALLBACK
PrintHookProc(
	HWND hDlg,	// handle to dialog box
	UINT ui_సందేశం,	// message identifier
	WPARAM wParam వాడని,	// message parameter
	LPARAM lParam	// message parameter
	)
{
    HWND	hwndOwner;
    RECT	rc, rcDlg, rcOwner;
    PRINTDLGW	*pPD;

    ఒకవేళ (ui_సందేశం != WM_INITDIALOG)
	తిరిగి FALSE;

    // Get the owner window and dialog box rectangles.
    ఒకవేళ ((hwndOwner = Get_మాతృ(hDlg)) == ఖాళీ)
	hwndOwner = Get_Desktop_కిటికీ();

    Get_కిటికీ_Rect(hwndOwner, &rcOwner);
    Get_కిటికీ_Rect(hDlg, &rcDlg);
    CopyRect(&rc, &rcOwner);

    // Offset the owner and dialog box rectangles so that
    // right and bottom values represent the width and
    // height, and then offset the owner again to discard
    // space taken up by the dialog box.

    స్థానభ్రంశం_Rect(&rcDlg, -rcDlg.ఎడమ_2, -rcDlg.శిఖరం);
    స్థానభ్రంశం_Rect(&rc, -rc.ఎడమ_2, -rc.శిఖరం);
    స్థానభ్రంశం_Rect(&rc, -rcDlg.కుడి_2, -rcDlg.bottom);

    // The new position is the sum of half the remaining
    // space and the owner's original position.

    సమితి_కిటికీ_Pos(hDlg,
	    HWND_శిఖరం,
	    rcOwner.ఎడమ_2 + (rc.కుడి_2 / 2),
	    rcOwner.శిఖరం + (rc.bottom / 2),
	    0, 0,		// ignores size arguments
	    SWP_NOSIZE);

    //  tackle the printdlg copiesctrl problem
    pPD = (PRINTDLGW *)lParam;
    pPD->nCopies = (మాట_2)pPD->l_Cust_దత్తాంశం;
    సమితి_Dlg_అంశం_Int( hDlg, edt3, pPD->nCopies, FALSE );
    //  Bring the window to top
    Bring_కిటికీ_గమ్యం_శిఖరం(Get_మాతృ(hDlg));
    సమితి_Foreground_కిటికీ(hDlg);

    తిరిగి FALSE;
}

    శూన్య
mch_print_cleanup(శూన్య)
{
    పూర్ణాంకం pifItalic;
    పూర్ణాంకం pifBold;
    పూర్ణాంకం pifUnderline;

    కొరకు (pifBold = 0; pifBold <= 1; pifBold++)
	కొరకు (pifItalic = 0; pifItalic <= 1; pifItalic++)
	    కొరకు (pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
		DeleteObject(prt_font_handles[pifBold][pifItalic][pifUnderline]);

    ఒకవేళ (prt_dlg.hDC != ఖాళీ)
	DeleteDC(prt_dlg.hDC);
    ఒకవేళ (!*bUserAbort && hDlgPrint != ఖాళీ)
	Send_సందేశం(hDlgPrint, WM_COMMAND, 0, 0);
}

    స్థిర పూర్ణాంకం
గమ్యం_device_units(పూర్ణాంకం idx, పూర్ణాంకం dpi, పూర్ణాంకం physsize, పూర్ణాంకం స్థానభ్రంశం, పూర్ణాంకం def_సంఖ్య)
{
    పూర్ణాంకం		ret = 0;
    పూర్ణాంకం		u;
    పూర్ణాంకం		nr;

    u = prt_get_unit(idx);
    ఒకవేళ (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_సంఖ్య;
    }
    లేకపోతే
	nr = printer_opts[idx].సంఖ్య;

    ఎంపిక (u)
    {
	స్థితి PRT_UNIT_PERC:
	    ret = (physsize * nr) / 100;
	    ఆపు;
	స్థితి PRT_UNIT_INCH:
	    ret = (nr * dpi);
	    ఆపు;
	స్థితి PRT_UNIT_MM:
	    ret = (nr * 10 * dpi) / 254;
	    ఆపు;
	స్థితి PRT_UNIT_బిందువు:
	    ret = (nr * 10 * dpi) / 720;
	    ఆపు;
    }

    ఒకవేళ (ret < స్థానభ్రంశం)
	తిరిగి 0;
    లేకపోతే
	తిరిగి ret - స్థానభ్రంశం;
}

    స్థిర పూర్ణాంకం
prt_get_cpl(శూన్య)
{
    పూర్ణాంకం		hr;
    పూర్ణాంకం		phyw;
    పూర్ణాంకం		dvoff;
    పూర్ణాంకం		rev_స్థానభ్రంశం;
    పూర్ణాంకం		dpi;

    Get_పాఠ్యం_Metrics(prt_dlg.hDC, &prt_tm);
    prt_పంక్తి_ఎత్తు = prt_tm.tm_ఎత్తు + prt_tm.tmExternalLeading;

    hr	    = GetDeviceCaps(prt_dlg.hDC, HORZRES);
    phyw    = GetDeviceCaps(prt_dlg.hDC, PHYSICALWIDTH);
    dvoff   = GetDeviceCaps(prt_dlg.hDC, PHYSICALOFFSETX);
    dpi	    = GetDeviceCaps(prt_dlg.hDC, LOGPIXELSX);

    rev_స్థానభ్రంశం = phyw - (dvoff + hr);

    prt_ఎడమ_margin = గమ్యం_device_units(OPT_PRINT_ఎడమ, dpi, phyw, dvoff, 10);
    ఒకవేళ (prt_use_సంఖ్య())
    {
	prt_సంఖ్య_వెడల్పు = PRINT_సంఖ్య_వెడల్పు * prt_tm.tm_Ave_Char_వెడల్పు;
	prt_ఎడమ_margin += prt_సంఖ్య_వెడల్పు;
    }
    లేకపోతే
	prt_సంఖ్య_వెడల్పు = 0;

    prt_కుడి_margin = hr - గమ్యం_device_units(OPT_PRINT_కుడి, dpi, phyw,
							       rev_స్థానభ్రంశం, 5);

    తిరిగి (prt_కుడి_margin - prt_ఎడమ_margin) / prt_tm.tm_Ave_Char_వెడల్పు;
}

    స్థిర పూర్ణాంకం
prt_get_lpp(శూన్య)
{
    పూర్ణాంకం vr;
    పూర్ణాంకం phyw;
    పూర్ణాంకం dvoff;
    పూర్ణాంకం rev_స్థానభ్రంశం;
    పూర్ణాంకం	bottom_margin;
    పూర్ణాంకం	dpi;

    vr	    = GetDeviceCaps(prt_dlg.hDC, VERTRES);
    phyw    = GetDeviceCaps(prt_dlg.hDC, PHYSICALHEIGHT);
    dvoff   = GetDeviceCaps(prt_dlg.hDC, PHYSICALOFFSETY);
    dpi	    = GetDeviceCaps(prt_dlg.hDC, LOGPIXELSY);

    rev_స్థానభ్రంశం = phyw - (dvoff + vr);

    prt_శిఖరం_margin = గమ్యం_device_units(OPT_PRINT_శిఖరం, dpi, phyw, dvoff, 5);

    // adjust top margin if there is a header
    prt_శిఖరం_margin += prt_పంక్తి_ఎత్తు * prt_శీర్షిక_ఎత్తు();

    bottom_margin = vr - గమ్యం_device_units(OPT_PRINT_BOT, dpi, phyw,
							       rev_స్థానభ్రంశం, 5);

    తిరిగి (bottom_margin - prt_శిఖరం_margin) / prt_పంక్తి_ఎత్తు;
}

    పూర్ణాంకం
mch_print_init(prt_settings_T *psettings, char_u *jobname, పూర్ణాంకం forceit)
{
    స్థిర HGLOBAL	stored_dm = ఖాళీ;
    స్థిర HGLOBAL	stored_devn = ఖాళీ;
    స్థిర పూర్ణాంకం		stored_nCopies = 1;
    స్థిర పూర్ణాంకం		stored_nFlags = 0;

    LOGFONTW		fLogFont;
    పూర్ణాంకం			pifItalic;
    పూర్ణాంకం			pifBold;
    పూర్ణాంకం			pifUnderline;

    DEVMODEW		*mem;
    DEVNAMES		*devname;
    పూర్ణాంకం			i;
    DWORD		err;

    bUserAbort = &(psettings->user_abort);
    CLEAR_క్షేత్రం(prt_dlg);
    prt_dlg.l_Struct_కొలత = పరిమాణం(PRINTDLGW);
# ఒకవేళ !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
    ఒకవేళ (!gui.లోపల_use)
#  endif
	GetConsoleHwnd();	    // get value of s_hwnd
# endif
    prt_dlg.hwndOwner = s_hwnd;
    prt_dlg.Flags = PD_NOPAGENUMS | PD_NOSELECTION | PD_RETURNDC;
    ఒకవేళ (!forceit)
    {
	prt_dlg.h_Dev_బహుళకం = stored_dm;
	prt_dlg.h_Dev_పేర్లు = stored_devn;
	prt_dlg.l_Cust_దత్తాంశం = stored_nCopies; // work around bug in print dialog
	/*
	 * Use hook to prevent print dialog being sent to back.
	 */
	prt_dlg.lpfnPrintHook = PrintHookProc;
	prt_dlg.Flags |= PD_ENABLEPRINTHOOK;
	prt_dlg.Flags |= stored_nFlags;
    }

    /*
     * If bang present, return default printer setup with no dialog
     * never show dialog if we are running over telnet
     */
    ఒకవేళ (forceit
# ఒకవేళ !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	    || (!gui.లోపల_use && !పదం_console)
#  లేకపోతే
	    || !పదం_console
#  endif
# endif
	    )
    {
	prt_dlg.Flags |= PD_RETURNDEFAULT;
	/*
	 * MSDN suggests setting the first parameter to WINSPOOL for
	 * NT, but NULL appears to work just as well.
	 */
	ఒకవేళ (*p_pdev != NUL)
	    prt_dlg.hDC = CreateDC(ఖాళీ, (LPCSTR)p_pdev, ఖాళీ, ఖాళీ);
	లేకపోతే
	{
	    prt_dlg.Flags |= PD_RETURNDEFAULT;
	    ఒకవేళ (PrintDlgW(&prt_dlg) == 0)
		వెళ్ళు init_fail_dlg;
	}
    }
    లేకపోతే ఒకవేళ (PrintDlgW(&prt_dlg) == 0)
	వెళ్ళు init_fail_dlg;
    లేకపోతే
    {
	/*
	 * keep the previous driver context
	 */
	stored_dm = prt_dlg.h_Dev_బహుళకం;
	stored_devn = prt_dlg.h_Dev_పేర్లు;
	stored_nFlags = prt_dlg.Flags;
	stored_nCopies = prt_dlg.nCopies;
    }

    ఒకవేళ (prt_dlg.hDC == ఖాళీ)
    {
	emsg(_(e_printer_selection_failed));
	mch_print_cleanup();
	తిరిగి FALSE;
    }

    // Not all printer drivers report the support of color (or grey) in the
    // same way.  Let's set has_color if there appears to be some way to print
    // more than B&W.
    i = GetDeviceCaps(prt_dlg.hDC, NUMCOLORS);
    psettings->has_color = (GetDeviceCaps(prt_dlg.hDC, BITSPIXEL) > 1
				   || GetDeviceCaps(prt_dlg.hDC, PLANES) > 1
				   || i > 2 || i == -1);

    // Ensure all font styles are baseline aligned
    సమితి_పాఠ్యం_Align(prt_dlg.hDC, TA_BASELINE|TA_ఎడమ);

    /*
     * On some windows systems the nCopies parameter is not
     * passed back correctly. It must be retrieved from the
     * hDevMode struct.
     */
    mem = (DEVMODEW *)Global_తాళం(prt_dlg.h_Dev_బహుళకం);
    ఒకవేళ (mem != ఖాళీ)
    {
	ఒకవేళ (mem->dmCopies != 1)
	    stored_nCopies = mem->dmCopies;
	ఒకవేళ ((mem->dmFields & DM_DUPLEX) && (mem->dmDuplex & ~DMDUP_SIMPLEX))
	    psettings->duplex = TRUE;
	ఒకవేళ ((mem->dmFields & DM_COLOR) && (mem->dmColor & DMCOLOR_COLOR))
	    psettings->has_color = TRUE;
    }
    GlobalUnlock(prt_dlg.h_Dev_బహుళకం);

    devname = (DEVNAMES *)Global_తాళం(prt_dlg.h_Dev_పేర్లు);
    ఒకవేళ (devname != 0)
    {
	WCHAR	*wprinter_name = (WCHAR *)devname + devname->w_Device_స్థానభ్రంశం;
	WCHAR	*wport_name = (WCHAR *)devname + devname->w_Output_స్థానభ్రంశం;
	char_u	*పాఠ్యం = (char_u *)_("to %s on %s");
	char_u  *printer_name = utf16_గమ్యం_enc(wprinter_name, ఖాళీ);
	char_u	*పోర్టు_name = utf16_గమ్యం_enc(wport_name, ఖాళీ);

	ఒకవేళ (printer_name != ఖాళీ && పోర్టు_name != ఖాళీ)
	    prt_name = alloc(STRLEN(printer_name)
					   + STRLEN(పోర్టు_name) + STRLEN(పాఠ్యం));
	ఒకవేళ (prt_name != ఖాళీ)
	    wsprintf((అక్షరం *)prt_name, (స్థిరరాశి అక్షరం *)పాఠ్యం,
		    printer_name, పోర్టు_name);
	vim_free(printer_name);
	vim_free(పోర్టు_name);
    }
    GlobalUnlock(prt_dlg.h_Dev_పేర్లు);

    /*
     * Initialise the font according to 'printfont'
     */
    CLEAR_క్షేత్రం(fLogFont);
    ఒకవేళ (get_logfont(&fLogFont, p_pfn, prt_dlg.hDC, TRUE) == FAIL)
    {
	semsg(_(e_unknown_printer_font_str), p_pfn);
	mch_print_cleanup();
	తిరిగి FALSE;
    }

    కొరకు (pifBold = 0; pifBold <= 1; pifBold++)
	కొరకు (pifItalic = 0; pifItalic <= 1; pifItalic++)
	    కొరకు (pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
	    {
		fLogFont.lf_బరువు =  boldface[pifBold];
		fLogFont.lfItalic = pifItalic;
		fLogFont.lfUnderline = pifUnderline;
		prt_font_handles[pifBold][pifItalic][pifUnderline]
					      = Create_Font_పరోక్ష_W(&fLogFont);
	    }

    సమితి_Bk_బహుళకం(prt_dlg.hDC, OPAQUE);
    SelectObject(prt_dlg.hDC, prt_font_handles[0][0][0]);

    /*
     * Fill in the settings struct
     */
    psettings->chars_చొప్పున_పంక్తి = prt_get_cpl();
    psettings->పంక్తులు_చొప్పున_పుట = prt_get_lpp();
    ఒకవేళ (prt_dlg.Flags & PD_USEDEVMODECOPIESANDCOLLATE)
    {
	psettings->n_collated_copies = (prt_dlg.Flags & PD_COLLATE)
						    ? prt_dlg.nCopies : 1;
	psettings->n_uncollated_copies = (prt_dlg.Flags & PD_COLLATE)
						    ? 1 : prt_dlg.nCopies;

	ఒకవేళ (psettings->n_collated_copies == 0)
	    psettings->n_collated_copies = 1;

	ఒకవేళ (psettings->n_uncollated_copies == 0)
	    psettings->n_uncollated_copies = 1;
    }
    లేకపోతే
    {
	psettings->n_collated_copies = 1;
	psettings->n_uncollated_copies = 1;
    }

    psettings->jobname = jobname;

    తిరిగి TRUE;

init_fail_dlg:
    err = CommDlgExtendedError();
    ఒకవేళ (err)
    {
	char_u *buf;

	// I suspect FormatMessage() doesn't work for values returned by
	// CommDlgExtendedError().  What does?
	Format_సందేశం(FORMAT_సందేశం_ALLOCATE_బఫరు |
		FORMAT_సందేశం_FROM_SYSTEM |
		FORMAT_సందేశం_IGNORE_INSERTS,
		ఖాళీ, err, 0, (LPTSTR)(&buf), 0, ఖాళీ);
	semsg(_(e_print_error_str),
		buf == ఖాళీ ? (char_u *)_("Unknown") : buf);
	LocalFree((LPVOID)(buf));
    }
    లేకపోతే
	సందేశం_clr_eos(); // Maybe canceled

    mch_print_cleanup();
    తిరిగి FALSE;
}


    పూర్ణాంకం
mch_print_begin(prt_settings_T *psettings)
{
    పూర్ణాంకం			ret = 0;
    అక్షరం		sz_బఫరు[300];
    WCHAR		*wp;

    hDlgPrint = CreateDialog(g_hinst, పాఠ్యం_3("PrintDlgBox"),
					     prt_dlg.hwndOwner, PrintDlgProc);
    సమితి_Abort_Proc(prt_dlg.hDC, AbortProc);
    wsprintf(sz_బఫరు, _("Printing '%s'"), gettail(psettings->jobname));
    vim_సమితి_Dlg_అంశం_పాఠ్యం(hDlgPrint, IDC_PRINTTEXT1, (char_u *)sz_బఫరు);

    wp = enc_గమ్యం_utf16(psettings->jobname, ఖాళీ);
    ఒకవేళ (wp != ఖాళీ)
    {
	DOCINFOW	di;

	CLEAR_క్షేత్రం(di);
	di.cb_కొలత = పరిమాణం(di);
	di.lpszDocName = wp;
	ret = ప్రారంభం_Doc_W(prt_dlg.hDC, &di);
	vim_free(wp);
    }

# ifdef FEAT_GUI
    // Give focus back to main window (when using MDI).
#  ifdef VIMDLL
    ఒకవేళ (gui.లోపల_use)
#  endif
	సమితి_Focus(s_hwnd);
# endif

    తిరిగి (ret > 0);
}

    శూన్య
mch_print_చివర(prt_settings_T *psettings వాడని)
{
    చివర_Doc(prt_dlg.hDC);
    ఒకవేళ (!*bUserAbort && hDlgPrint != ఖాళీ)
	Send_సందేశం(hDlgPrint, WM_COMMAND, 0, 0);
}

    పూర్ణాంకం
mch_print_చివర_పుట(శూన్య)
{
    తిరిగి (చివర_పుట(prt_dlg.hDC) > 0);
}

    పూర్ణాంకం
mch_print_begin_పుట(char_u *సందేశం)
{
    ఒకవేళ (సందేశం != ఖాళీ)
	vim_సమితి_Dlg_అంశం_పాఠ్యం(hDlgPrint, IDC_పురోగతి, సందేశం);
    తిరిగి (ప్రారంభం_పుట(prt_dlg.hDC) > 0);
}

    పూర్ణాంకం
mch_print_blank_పుట(శూన్య)
{
    తిరిగి (mch_print_begin_పుట(ఖాళీ) ? (mch_print_చివర_పుట()) : FALSE);
}

స్థిర పూర్ణాంకం prt_pos_x = 0;
స్థిర పూర్ణాంకం prt_pos_y = 0;

    శూన్య
mch_print_ప్రారంభం_పంక్తి(పూర్ణాంకం margin, పూర్ణాంకం పుట_పంక్తి)
{
    ఒకవేళ (margin)
	prt_pos_x = -prt_సంఖ్య_వెడల్పు;
    లేకపోతే
	prt_pos_x = 0;
    prt_pos_y = పుట_పంక్తి * prt_పంక్తి_ఎత్తు
				 + prt_tm.tmAscent + prt_tm.tmExternalLeading;
}

    పూర్ణాంకం
mch_print_పాఠ్యం_బహిర్గతం(char_u *p, పూర్ణాంకం len)
{
    కొలత_3	sz;
    WCHAR	*wp;
    పూర్ణాంకం		wlen = len;
    పూర్ణాంకం		ret = FALSE;

    wp = enc_గమ్యం_utf16(p, &wlen);
    ఒకవేళ (wp == ఖాళీ)
	తిరిగి FALSE;

    పాఠ్యం_బహిర్గతం_W(prt_dlg.hDC, prt_pos_x + prt_ఎడమ_margin,
	    prt_pos_y + prt_శిఖరం_margin, wp, wlen);
    Get_పాఠ్యం_Extent_Point32_W(prt_dlg.hDC, wp, wlen, &sz);
    vim_free(wp);
    prt_pos_x += (sz.cx - prt_tm.tmOverhang);
    // This is wrong when printing spaces for a TAB.
    ఒకవేళ (p[len] != NUL)
    {
	wlen = mb_ptr2len(p + len);
	wp = enc_గమ్యం_utf16(p + len, &wlen);
	ఒకవేళ (wp != ఖాళీ)
	{
	    Get_పాఠ్యం_Extent_Point32_W(prt_dlg.hDC, wp, 1, &sz);
	    ret = (prt_pos_x + prt_ఎడమ_margin + sz.cx > prt_కుడి_margin);
	    vim_free(wp);
	}
    }
    తిరిగి ret;
}

    శూన్య
mch_print_సమితి_font(పూర్ణాంకం iBold, పూర్ణాంకం iItalic, పూర్ణాంకం iUnderline)
{
    SelectObject(prt_dlg.hDC, prt_font_handles[iBold][iItalic][iUnderline]);
}

    శూన్య
mch_print_సమితి_bg(long_u bgcol)
{
    సమితి_Bk_Color(prt_dlg.hDC, Get_సమీప_Color(prt_dlg.hDC,
						   మార్పిడి_me((COLORREF)bgcol)));
    /*
     * With a white background we can draw characters transparent, which is
     * good for italic characters that overlap to the next char cell.
     */
    ఒకవేళ (bgcol == 0xffffffUL)
	సమితి_Bk_బహుళకం(prt_dlg.hDC, TRANSPARENT);
    లేకపోతే
	సమితి_Bk_బహుళకం(prt_dlg.hDC, OPAQUE);
}

    శూన్య
mch_print_సమితి_fg(long_u fgcol)
{
    సమితి_పాఠ్యం_Color(prt_dlg.hDC, Get_సమీప_Color(prt_dlg.hDC,
						   మార్పిడి_me((COLORREF)fgcol)));
}

#అయితే_ముగింపు // FEAT_PRINTER && !FEAT_POSTSCRIPT



#ఒకవేళ defined(FEAT_SHORTCUT)
# include <shlobj.h>

# define is_మార్గం_sep(c)	    ((c) == L'\\' || (c) == L'/')

    స్థిర పూర్ణాంకం
is_reparse_బిందువు_included(LPCWSTR fname)
{
    LPCWSTR	p = fname, q;
    WCHAR	buf[MAX_మార్గం];
    DWORD	attr;

    ఒకవేళ (సురక్షిత_isalpha(p[0]) && p[1] == L':' && is_మార్గం_sep(p[2]))
	p += 3;
    లేకపోతే ఒకవేళ (is_మార్గం_sep(p[0]) && is_మార్గం_sep(p[1]))
	p += 2;

    అయితే (*p != L'\0')
    {
	q = wcspbrk(p, L"\\/");
	ఒకవేళ (q == ఖాళీ)
	    p = q = fname + wcslen(fname);
	లేకపోతే
	    p = q + 1;
	ఒకవేళ (q - fname >= MAX_మార్గం)
	    తిరిగి FALSE;
	wcsncpy(buf, fname, q - fname);
	buf[q - fname] = L'\0';
	attr = Get_File_లక్షణాలు_W(buf);
	ఒకవేళ (attr != INVALID_FILE_లక్షణాలు
		&& (attr & FILE_ATTRIBUTE_REPARSE_బిందువు) != 0)
	    తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * Return the resolved file path, NULL if "fname" is an AppExecLink reparse
 * point, already fully resolved, or it doesn't exists.
 */
    char_u *
resolve_reparse_బిందువు(char_u *fname)
{
    HANDLE	    h = INVALID_HANDLE_విలువ;
    DWORD	    కొలత;
    WCHAR	    *p, *wp;
    char_u	    *rfname = ఖాళీ;
    WCHAR	    *buff = ఖాళీ;

    p = enc_గమ్యం_utf16(fname, ఖాళీ);
    ఒకవేళ (p == ఖాళీ)
	వెళ్ళు fail;

    ఒకవేళ (!is_reparse_బిందువు_included(p))
    {
	vim_free(p);
	వెళ్ళు fail;
    }

    h = CreateFileW(p, 0, 0, ఖాళీ, OPEN_EXISTING,
	    FILE_FLAG_BACKUP_SEMANTICS, ఖాళీ);
    vim_free(p);

    ఒకవేళ (h == INVALID_HANDLE_విలువ)
	వెళ్ళు fail;

    కొలత = Get_Final_మార్గం_Name_By_Handle_W(h, ఖాళీ, 0, 0);
    ఒకవేళ (కొలత == 0)
	వెళ్ళు fail;
    buff = ALLOC_MULT(WCHAR, కొలత);
    ఒకవేళ (buff == ఖాళీ)
	వెళ్ళు fail;
    ఒకవేళ (Get_Final_మార్గం_Name_By_Handle_W(h, buff, కొలత, 0) == 0)
	వెళ్ళు fail;

    ఒకవేళ (wcsncmp(buff, L"\\\\?\\UNC\\", 8) == 0)
    {
	buff[6] = L'\\';
	wp = buff + 6;
    }
    లేకపోతే ఒకవేళ (wcsncmp(buff, L"\\\\?\\", 4) == 0)
	wp = buff + 4;
    లేకపోతే
	wp = buff;

    rfname = utf16_గమ్యం_enc(wp, ఖాళీ);

fail:
    ఒకవేళ (h != INVALID_HANDLE_విలువ)
	CloseHandle(h);
    ఒకవేళ (buff != ఖాళీ)
	vim_free(buff);

    తిరిగి rfname;
}

/*
 * When "fname" is the name of a shortcut (*.lnk) resolve the file it points
 * to and return that name in allocated memory.
 * Otherwise NULL is returned.
 */
    స్థిర char_u *
resolve_shortcut(char_u *fname)
{
    HRESULT		hr;
    IShellLink		*psl = ఖాళీ;
    IPersistFile	*ppf = ఖాళీ;
    OLECHAR		wsz[MAX_మార్గం];
    char_u		*rfname = ఖాళీ;
    పూర్ణాంకం			len;
    IShellLinkW		*pslw = ఖాళీ;
    WIN32_కనుగొను_DATAW	ffdw; // we get those free of charge

    // Check if the file name ends in ".lnk". Avoid calling
    // CoCreateInstance(), it's quite slow.
    ఒకవేళ (fname == ఖాళీ)
	తిరిగి rfname;
    len = (పూర్ణాంకం)STRLEN(fname);
    ఒకవేళ (len <= 4 || STRNICMP(fname + len - 4, ".lnk", 4) != 0)
	తిరిగి rfname;

    CoInitialize(ఖాళీ);

    // create a link manager object and request its interface
    hr = CoCreateInstance(
	    &CLSID_ShellLink, ఖాళీ, CLSCTX_INPROC_SERVER,
	    &IID_IShellLinkW, (శూన్య**)&pslw);
    ఒకవేళ (hr == S_OK)
    {
	WCHAR	*p = enc_గమ్యం_utf16(fname, ఖాళీ);

	ఒకవేళ (p != ఖాళీ)
	{
	    // Get a pointer to the IPersistFile interface.
	    hr = pslw->lpVtbl->QueryInterface(
		    pslw, &IID_IPersistFile, (శూన్య**)&ppf);
	    ఒకవేళ (hr != S_OK)
		వెళ్ళు shortcut_errorw;

	    // "load" the name and resolve the link
	    hr = ppf->lpVtbl->ఎక్కించు_2(ppf, p, STGM_READ);
	    ఒకవేళ (hr != S_OK)
		వెళ్ళు shortcut_errorw;
# ఒకవేళ 0  // This makes Vim wait a long time if the target does not exist.
	    hr = pslw->lpVtbl->Resolve(pslw, ఖాళీ, SLR_NO_UI);
	    ఒకవేళ (hr != S_OK)
		వెళ్ళు shortcut_errorw;
# endif

	    // Get the path to the link target.
	    Zero_జ్ఞాపకం(wsz, MAX_మార్గం * పరిమాణం(WCHAR));
	    hr = pslw->lpVtbl->Get_మార్గం(pslw, wsz, MAX_మార్గం, &ffdw, 0);
	    ఒకవేళ (hr == S_OK && wsz[0] != NUL)
		rfname = utf16_గమ్యం_enc(wsz, ఖాళీ);

shortcut_errorw:
	    vim_free(p);
	}
    }

    // Release all interface pointers (both belong to the same object)
    ఒకవేళ (ppf != ఖాళీ)
	ppf->lpVtbl->Release(ppf);
    ఒకవేళ (psl != ఖాళీ)
	psl->lpVtbl->Release(psl);
    ఒకవేళ (pslw != ఖాళీ)
	pslw->lpVtbl->Release(pslw);

    CoUninitialize();
    తిరిగి rfname;
}

    char_u *
mch_resolve_మార్గం(char_u *fname, పూర్ణాంకం reparse_బిందువు)
{
    char_u  *మార్గం = resolve_shortcut(fname);

    ఒకవేళ (మార్గం == ఖాళీ && reparse_బిందువు)
	మార్గం = resolve_reparse_బిందువు(fname);
    తిరిగి మార్గం;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_EVAL) && (!defined(FEAT_GUI) || defined(VIMDLL))
/*
 * Bring ourselves to the foreground.  Does work if the OS doesn't allow it.
 */
    శూన్య
win32_సమితి_foreground(శూన్య)
{
    GetConsoleHwnd();	    // get value of s_hwnd
    ఒకవేళ (s_hwnd != 0)
	సమితి_Foreground_కిటికీ(s_hwnd);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_CLIENTSERVER)
/*
 * Client-server code for Vim
 *
 * Originally written by Paul Moore
 */

// In order to handle inter-process messages, we need to have a window. But
// the functions in this module can be called before the main GUI window is
// created (and may also be called in the console version, where there is no
// GUI window at all).
//
// So we create a hidden window, and arrange to destroy it on exit.
HWND సందేశం_కిటికీ = 0;	    // window that's handling messages

# define VIM_CLASSNAME      "VIM_MESSAGES"
# define VIM_CLASSNAME_LEN  (పరిమాణం(VIM_CLASSNAME) - 1)

// Timeout for sending a message to another Vim instance.  Normally this works
// instantly, but it may hang when the other Vim instance is halted.
# define SENDMESSAGE_TIMEOUT	(5 * 1000)

// Communication is via WM_COPYDATA messages. The message type is sent in
// the dwData parameter. Types are defined here.
# define COPYDATA_కీలకాలు		0
# define COPYDATA_జవాబు		1
# define COPYDATA_సమాసం		10
# define COPYDATA_ఫలితం	11
# define COPYDATA_ERROR_ఫలితం	12
# define COPYDATA_ENCODING	20

// This is a structure containing a server HWND and its name.
నిర్మాణం server_id
{
    HWND hwnd;
    char_u *name;
};

// Last received 'encoding' that the client uses.
స్థిర char_u	*client_enc = ఖాళీ;

/*
 * Tell the other side what encoding we are using.
 * Return -1 if timeout happens.  Other errors are ignored.
 */
    స్థిర పూర్ణాంకం
serverSendEnc(HWND లక్ష్యం)
{
    COPYDATASTRUCT దత్తాంశం;

    దత్తాంశం.dw_దత్తాంశం = COPYDATA_ENCODING;
    దత్తాంశం.cb_దత్తాంశం = (DWORD)STRLEN(p_enc) + 1;
    దత్తాంశం.lp_దత్తాంశం = p_enc;
    ఒకవేళ (Send_సందేశం_Timeout(లక్ష్యం, WM_COPYDATA,
	    (WPARAM)సందేశం_కిటికీ, (LPARAM)&దత్తాంశం,
	    SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, ఖాళీ) == 0)
	తిరిగి -1;
    తిరిగి 0;
}

/*
 * Clean up on exit. This destroys the hidden message window.
 */
    స్థిర శూన్య
Clean_పైకి_Messaging(శూన్య)
{
    ఒకవేళ (సందేశం_కిటికీ == 0)
	తిరిగి;

    Destroy_కిటికీ(సందేశం_కిటికీ);
    సందేశం_కిటికీ = 0;
}

స్థిర పూర్ణాంకం save_జవాబు(HWND server, char_u *జవాబు, పూర్ణాంకం సమాసం);

/*
 * The window procedure for the hidden message window.
 * It handles callback messages and notifications from servers.
 * In order to process these messages, it is necessary to run a
 * message loop. Code which may run before the main message loop
 * is started (in the GUI) is careful to pump messages when it needs
 * to. Features which require message delivery during normal use will
 * not work in the console version - this basically means those
 * features which allow Vim to act as a server, rather than a client.
 */
    స్థిర LRESULT CALLBACK
Messaging_WndProc(HWND hwnd, UINT సందేశం, WPARAM wParam, LPARAM lParam)
{
    ఒకవేళ (సందేశం == WM_COPYDATA)
    {
	// This is a message from another Vim. The dwData member of the
	// COPYDATASTRUCT determines the type of message:
	//   COPYDATA_ENCODING:
	//	The encoding that the client uses. Following messages will
	//	use this encoding, convert if needed.
	//   COPYDATA_KEYS:
	//	A key sequence. We are a server, and a client wants these keys
	//	adding to the input queue.
	//   COPYDATA_REPLY:
	//	A reply. We are a client, and a server has sent this message
	//	in response to a request.  (server2client())
	//   COPYDATA_EXPR:
	//	An expression. We are a server, and a client wants us to
	//	evaluate this expression.
	//   COPYDATA_RESULT:
	//	A reply. We are a client, and a server has sent this message
	//	in response to a COPYDATA_EXPR.
	//   COPYDATA_ERROR_RESULT:
	//	A reply. We are a client, and a server has sent this message
	//	in response to a COPYDATA_EXPR that failed to evaluate.
	COPYDATASTRUCT	*దత్తాంశం = (COPYDATASTRUCT*)lParam;
	HWND		sender = (HWND)wParam;
	COPYDATASTRUCT	జవాబు;
	char_u		*res;
	పూర్ణాంకం		retval;
	DWORD_PTR	dwret = 0;
	char_u		*str;
	char_u		*tofree;

	ఎంపిక (దత్తాంశం->dw_దత్తాంశం)
	{
	స్థితి COPYDATA_ENCODING:
	    // Remember the encoding that the client uses.
	    vim_free(client_enc);
	    client_enc = enc_canonize((char_u *)దత్తాంశం->lp_దత్తాంశం);
	    తిరిగి 1;

	స్థితి COPYDATA_కీలకాలు:
	    // Remember who sent this, for <client>
	    client_కిటికీ = sender;

	    // Add the received keys to the input buffer.
	    str = serverConvert(client_enc, (char_u *)దత్తాంశం->lp_దత్తాంశం, &tofree);
# ifdef FEAT_GUI
	    ఒకవేళ (gui.లోపల_use)
		// The GUI may get here re-entrantly, so insert the keys later.
		server_కూడు_ఆదానం(str);
	    లేకపోతే
# endif
		server_గమ్యం_ఆదానం_buf(str);
	    vim_free(tofree);

# ifdef FEAT_GUI
	    // Wake up the main GUI loop.
#  ifdef VIMDLL
	    ఒకవేళ (gui.లోపల_use)
#  endif
		ఒకవేళ (s_hwnd != 0)
		    Post_సందేశం(s_hwnd, WM_NULL, 0, 0);
# endif
	    తిరిగి 1;

	స్థితి COPYDATA_సమాసం:
	    // Remember who sent this, for <client>
	    client_కిటికీ = sender;

	    str = serverConvert(client_enc, (char_u *)దత్తాంశం->lp_దత్తాంశం, &tofree);
	    res = eval_client_సమాసం_గమ్యం_string(str);

	    ఒకవేళ (res == ఖాళీ)
	    {
		అక్షరం	*err = _(e_invalid_వ్యక్తీకరణ_received);
		size_t	len = STRLEN(str) + STRLEN(err) + 5;

		res = alloc(len);
		ఒకవేళ (res != ఖాళీ)
		    జవాబు.cb_దత్తాంశం = (DWORD)vim_snprintf_safelen(
			(అక్షరం *)res, len, "%s: \"%s\"", err, str);
		లేకపోతే
		    జవాబు.cb_దత్తాంశం = 0;
		జవాబు.dw_దత్తాంశం = COPYDATA_ERROR_ఫలితం;
	    }
	    లేకపోతే
	    {
		జవాబు.cb_దత్తాంశం = (DWORD)STRLEN(res) + 1;
		జవాబు.dw_దత్తాంశం = COPYDATA_ఫలితం;
	    }
	    జవాబు.lp_దత్తాంశం = res;

	    ఒకవేళ (serverSendEnc(sender) < 0)
		retval = -1;
	    లేకపోతే
	    {
		ఒకవేళ (Send_సందేశం_Timeout(sender, WM_COPYDATA,
			(WPARAM)సందేశం_కిటికీ, (LPARAM)&జవాబు,
			SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
		    retval = -1;
		లేకపోతే
		    retval = (పూర్ణాంకం)dwret;
	    }
	    vim_free(tofree);
	    vim_free(res);
	    తిరిగి retval;

	స్థితి COPYDATA_జవాబు:
	స్థితి COPYDATA_ఫలితం:
	స్థితి COPYDATA_ERROR_ఫలితం:
	    ఒకవేళ (దత్తాంశం->lp_దత్తాంశం != ఖాళీ)
	    {
		str = serverConvert(client_enc, (char_u *)దత్తాంశం->lp_దత్తాంశం,
								     &tofree);
		ఒకవేళ (tofree == ఖాళీ)
		    str = vim_strsave(str);
		ఒకవేళ (save_జవాబు(sender, str,
			   (దత్తాంశం->dw_దత్తాంశం == COPYDATA_జవాబు ?  0 :
			   (దత్తాంశం->dw_దత్తాంశం == COPYDATA_ఫలితం ? 1 :
							      2))) == FAIL)
		    vim_free(str);
		లేకపోతే ఒకవేళ (దత్తాంశం->dw_దత్తాంశం == COPYDATA_జవాబు)
		{
		    char_u	winstr[30];

		    sprintf((అక్షరం *)winstr, PRINTF_HEX_LONG_U, (long_u)sender);
		    apply_autocmds(EVENT_REMOTEREPLY, winstr, str,
								TRUE, curbuf);
		}
	    }
	    తిరిగి 1;
	}

	తిరిగి 0;
    }

    లేకపోతే ఒకవేళ (సందేశం == WM_ACTIVATE && wParam == WA_చురుకైన)
    {
	// When the message window is activated (brought to the foreground),
	// this actually applies to the text window.
# ఒకవేళ !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	ఒకవేళ (!gui.లోపల_use)
#  endif
	    GetConsoleHwnd();	    // get value of s_hwnd
# endif
	ఒకవేళ (s_hwnd != 0)
	{
	    సమితి_Foreground_కిటికీ(s_hwnd);
	    తిరిగి 0;
	}
    }

    తిరిగి Def_కిటికీ_Proc(hwnd, సందేశం, wParam, lParam);
}

/*
 * Initialise the message handling process.  This involves creating a window
 * to handle messages - the window will not be visible.
 */
    శూన్య
serverInitMessaging(శూన్య)
{
    WNDCLASS wndclass;

    // Clean up on exit
    atexit(Clean_పైకి_Messaging);

    // Register a window class - we only really care
    // about the window procedure
    wndclass.style = 0;
    wndclass.lpfnWndProc = Messaging_WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = g_hinst;
    wndclass.hIcon = ఖాళీ;
    wndclass.hCursor = ఖాళీ;
    wndclass.hbrBackground = ఖాళీ;
    wndclass.lpszMenuName = ఖాళీ;
    wndclass.lpszClassName = VIM_CLASSNAME;
    రిజిస్టరు_Class(&wndclass);

    // Create the message window. It will be hidden, so the details don't
    // matter.  Don't use WS_OVERLAPPEDWINDOW, it will make a shortcut remove
    // focus from gvim.
    సందేశం_కిటికీ = Create_కిటికీ(VIM_CLASSNAME, "",
			 WS_POPUPWINDOW | WS_CAPTION,
			 CW_USEDEFAULT, CW_USEDEFAULT,
			 100, 100, ఖాళీ, ఖాళీ,
			 g_hinst, ఖాళీ);
}

// Used by serverSendToVim() to find an alternate server name.
స్థిర char_u *altname_buf_ptr = ఖాళీ;

/*
 * Get the title of the window "hwnd", which is the Vim server name, in
 * "name[namelen]" and return the length.
 * Returns zero if window "hwnd" is not a Vim server.
 */
    స్థిర పూర్ణాంకం
getVimServerName(HWND hwnd, అక్షరం *name, పూర్ణాంకం namelen)
{
    పూర్ణాంకం		len;
    అక్షరం	బఫరు[VIM_CLASSNAME_LEN + 1];

    // Ignore windows which aren't Vim message windows
    len = GetClassName(hwnd, బఫరు, పరిమాణం(బఫరు));
    ఒకవేళ (len != VIM_CLASSNAME_LEN || STRCMP(బఫరు, VIM_CLASSNAME) != 0)
	తిరిగి 0;

    // Get the title of the window
    తిరిగి Get_కిటికీ_పాఠ్యం(hwnd, name, namelen);
}

    స్థిర BOOL CALLBACK
enumWindowsGetServer(HWND hwnd, LPARAM lparam)
{
    నిర్మాణం	server_id *id = (నిర్మాణం server_id *)lparam;
    అక్షరం	server[MAX_మార్గం];

    // Get the title of the window
    ఒకవేళ (getVimServerName(hwnd, server, పరిమాణం(server)) == 0)
	తిరిగి TRUE;

    // If this is the server we're looking for, return its HWND
    ఒకవేళ (STRICMP(server, id->name) == 0)
    {
	id->hwnd = hwnd;
	తిరిగి FALSE;
    }

    // If we are looking for an alternate server, remember this name.
    ఒకవేళ (altname_buf_ptr != ఖాళీ)
    {
	size_t	namelen = STRLEN(id->name);

	ఒకవేళ (STRNICMP(server, id->name, namelen) == 0
	    && vim_isdigit(server[namelen]))
	{
	    STRCPY(altname_buf_ptr, server);
	    altname_buf_ptr = ఖాళీ;	    // don't use another name
	}
    }

    // Otherwise, keep looking
    తిరిగి TRUE;
}

    స్థిర BOOL CALLBACK
enum_Windows_Get_పేర్లు(HWND hwnd, LPARAM lparam)
{
    list_T	*list = (list_T *)lparam;
    అక్షరం	server[MAX_మార్గం];

    // Get the title of the window
    ఒకవేళ (getVimServerName(hwnd, server, పరిమాణం(server)) == 0)
	తిరిగి TRUE;

    list_append_string(list, (char_u *)server, -1);
    తిరిగి TRUE;
}

నిర్మాణం enum_windows_s
{
    WNDENUMPROC lpEnumFunc;
    LPARAM      lParam;
};

    స్థిర BOOL CALLBACK
enum_windows_బిడ్డ(HWND hwnd, LPARAM lParam)
{
    నిర్మాణం enum_windows_s *ew = (నిర్మాణం enum_windows_s *)lParam;

    తిరిగి (ew->lpEnumFunc)(hwnd, ew->lParam);
}

    స్థిర BOOL CALLBACK
enum_windows_toplevel(HWND hwnd, LPARAM lParam)
{
    నిర్మాణం enum_windows_s *ew = (నిర్మాణం enum_windows_s *)lParam;

    ఒకవేళ ((ew->lpEnumFunc)(hwnd, ew->lParam))
	తిరిగి TRUE;
    తిరిగి Enum_బిడ్డ_Windows(hwnd, enum_windows_బిడ్డ, lParam);
}

/*
 * Enumerate all windows including children.
 */
    స్థిర BOOL
enum_windows(WNDENUMPROC lpEnumFunc, LPARAM lParam)
{
    నిర్మాణం enum_windows_s ew;

    ew.lpEnumFunc = lpEnumFunc;
    ew.lParam = lParam;
    తిరిగి EnumWindows(enum_windows_toplevel, (LPARAM)&ew);
}

    స్థిర HWND
కనుగొను_Server(char_u *name)
{
    నిర్మాణం server_id id;

    id.name = name;
    id.hwnd = 0;

    enum_windows(enumWindowsGetServer, (LPARAM)(&id));

    తిరిగి id.hwnd;
}

    శూన్య
server_సమితి_Name(char_u *name)
{
    size_t	namelen;
    char_u	*ok_name;
    HWND	hwnd = 0;
    పూర్ణాంకం		i = 0;
    char_u	*p;

    // Leave enough space for a 9-digit suffix to ensure uniqueness!
    namelen = STRLEN(name);
    ok_name = alloc(namelen + 10);
    ఒకవేళ (ok_name == ఖాళీ)
	తిరిగి;

    STRCPY(ok_name, name);
    p = ok_name + namelen;

    కొరకు (;;)
    {
	// This is inefficient - we're doing an EnumWindows loop for each
	// possible name. It would be better to grab all names in one go,
	// and scan the list each time...
	hwnd = కనుగొను_Server(ok_name);
	ఒకవేళ (hwnd == 0)
	    ఆపు;

	++i;
	ఒకవేళ (i >= 1000)
	    ఆపు;

	sprintf((అక్షరం *)p, "%d", i);
    }

    ఒకవేళ (hwnd != 0)
	vim_free(ok_name);
    లేకపోతే
    {
	// Remember the name
	serverName = ok_name;
	need_maketitle = TRUE;	// update Vim window title later

	// Update the message window title
	సమితి_కిటికీ_పాఠ్యం(సందేశం_కిటికీ, (LPCSTR)ok_name);

# ifdef FEAT_EVAL
	// Set the servername variable
	సమితి_vim_var_string(VV_SEND_SERVER, serverName, -1);
# endif
    }
}

    list_T *
server_Get_Vim_పేర్లు(శూన్య)
{
    list_T *list = list_alloc();

    ఒకవేళ (list == ఖాళీ)
	తిరిగి ఖాళీ;

    enum_windows(enum_Windows_Get_పేర్లు, (LPARAM)list);

    తిరిగి list;
}

    పూర్ణాంకం
server_Send_జవాబు(
    char_u	*name,		// Where to send.
    char_u	*జవాబు)		// What to send.
{
    HWND	లక్ష్యం;
    COPYDATASTRUCT దత్తాంశం;
    long_u	n = 0;
    DWORD_PTR	dwret = 0;

    // The "name" argument is a magic cookie obtained from expand("<client>").
    // It should be of the form 0xXXXXX - i.e. a C hex literal, which is the
    // value of the client's message window HWND.
    అక్షరశ్రేణి_చదువు((అక్షరం *)name, SCANF_HEX_LONG_U, &n);
    ఒకవేళ (n == 0)
	తిరిగి -1;

    లక్ష్యం = (HWND)n;
    ఒకవేళ (!Is_కిటికీ(లక్ష్యం))
	తిరిగి -1;

    దత్తాంశం.dw_దత్తాంశం = COPYDATA_జవాబు;
    దత్తాంశం.cb_దత్తాంశం = (DWORD)STRLEN(జవాబు) + 1;
    దత్తాంశం.lp_దత్తాంశం = జవాబు;

    ఒకవేళ (serverSendEnc(లక్ష్యం) < 0)
	తిరిగి -1;
    ఒకవేళ (Send_సందేశం_Timeout(లక్ష్యం, WM_COPYDATA,
		(WPARAM)సందేశం_కిటికీ, (LPARAM)&దత్తాంశం,
		SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
	తిరిగి -1;
    తిరిగి dwret ? 0 : -1;
}

    పూర్ణాంకం
server_Send_గమ్యం_Vim(
    char_u	 *name,			// Where to send.
    char_u	 *cmd,			// What to send.
    char_u	 **ఫలితం,		// Result of eval'ed expression
    శూన్య	 *ptarget,		// HWND of server
    పూర్ణాంకం		 as_సమాసం,		// Expression or keys?
    పూర్ణాంకం		 timeout,		// timeout in seconds or zero
    పూర్ణాంకం		 silent)		// don't complain about no server
{
    size_t	namelen;
    HWND	లక్ష్యం;
    COPYDATASTRUCT దత్తాంశం;
    char_u	*retval = ఖాళీ;
    పూర్ణాంకం		retcode = 0;
    DWORD_PTR	dwret = 0;
    char_u	altname_buf[MAX_మార్గం];

    // Execute locally if no display or target is ourselves
    ఒకవేళ (serverName != ఖాళీ && STRICMP(name, serverName) == 0)
	తిరిగి send_గమ్యం_Local_Vim(cmd, as_సమాసం, ఫలితం);

    // If the server name does not end in a digit then we look for an
    // alternate name.  e.g. when "name" is GVIM then we may find GVIM2.
    namelen = STRLEN(name);
    ఒకవేళ (namelen > 1 && !vim_isdigit(name[namelen - 1]))
	altname_buf_ptr = altname_buf;
    altname_buf[0] = NUL;
    లక్ష్యం = కనుగొను_Server(name);
    altname_buf_ptr = ఖాళీ;
    ఒకవేళ (లక్ష్యం == 0 && altname_buf[0] != NUL)
	// Use another server name we found.
	లక్ష్యం = కనుగొను_Server(altname_buf);

    ఒకవేళ (లక్ష్యం == 0)
    {
	ఒకవేళ (!silent)
	    semsg(_(e_no_registered_server_named_str), name);
	తిరిగి -1;
    }

    ఒకవేళ (ptarget)
	*(HWND *)ptarget = లక్ష్యం;

    దత్తాంశం.dw_దత్తాంశం = as_సమాసం ? COPYDATA_సమాసం : COPYDATA_కీలకాలు;
    దత్తాంశం.cb_దత్తాంశం = (DWORD)STRLEN(cmd) + 1;
    దత్తాంశం.lp_దత్తాంశం = cmd;

    ఒకవేళ (serverSendEnc(లక్ష్యం) < 0)
	తిరిగి -1;
    ఒకవేళ (Send_సందేశం_Timeout(లక్ష్యం, WM_COPYDATA,
		(WPARAM)సందేశం_కిటికీ, (LPARAM)&దత్తాంశం,
		SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
	తిరిగి -1;
    ఒకవేళ (dwret == 0)
	తిరిగి -1;

    ఒకవేళ (as_సమాసం)
	retval = server_Get_జవాబు(లక్ష్యం, &retcode, TRUE, TRUE, timeout);

    ఒకవేళ (ఫలితం == ఖాళీ)
	vim_free(retval);
    లేకపోతే
	*ఫలితం = retval; // Caller assumes responsibility for freeing

    తిరిగి retcode;
}

/*
 * Bring the server to the foreground.
 */
    శూన్య
serverForeground(char_u *name)
{
    HWND	లక్ష్యం = కనుగొను_Server(name);

    ఒకవేళ (లక్ష్యం != 0)
	సమితి_Foreground_కిటికీ(లక్ష్యం);
}

// Replies from server need to be stored until the client picks them up via
// remote_read(). So we maintain a list of server-id/reply pairs.
// Note that there could be multiple replies from one server pending if the
// client is slow picking them up.
// We just store the replies in a simple list. When we remove an entry, we
// move list entries down to fill the gap.
// The server ID is simply the HWND.
రకనిర్వచనం నిర్మాణం
{
    HWND	server;		// server window
    char_u	*జవాబు;		// reply string
    పూర్ణాంకం		సమాసం_ఫలితం;	// 0 for REPLY, 1 for RESULT 2 for error
} జవాబు_T;

స్థిర garray_T జవాబు_list = {0, 0, పరిమాణం(జవాబు_T), 5, 0};

# define జవాబు_అంశం(i) ((జవాబు_T *)(జవాబు_list.ga_దత్తాంశం) + (i))
# define జవాబు_లెక్క (జవాబు_list.ga_len)

// Flag which is used to wait for a reply
స్థిర పూర్ణాంకం జవాబు_received = 0;

/*
 * Store a reply.  "reply" must be allocated memory (or NULL).
 */
    స్థిర పూర్ణాంకం
save_జవాబు(HWND server, char_u *జవాబు, పూర్ణాంకం సమాసం)
{
    జవాబు_T *rep;

    ఒకవేళ (ga_grow(&జవాబు_list, 1) == FAIL)
	తిరిగి FAIL;

    rep = జవాబు_అంశం(జవాబు_లెక్క);
    rep->server = server;
    rep->జవాబు = జవాబు;
    rep->సమాసం_ఫలితం = సమాసం;
    ఒకవేళ (rep->జవాబు == ఖాళీ)
	తిరిగి FAIL;

    ++జవాబు_లెక్క;
    జవాబు_received = 1;
    తిరిగి OK;
}

/*
 * Get a reply from server "server".
 * When "expr_res" is non NULL, get the result of an expression, otherwise a
 * server2client() message.
 * When non NULL, point to return code. 0 => OK, -1 => ERROR
 * If "remove" is TRUE, consume the message, the caller must free it then.
 * if "wait" is TRUE block until a message arrives (or the server exits).
 */
    char_u *
server_Get_జవాబు(HWND server, పూర్ణాంకం *సమాసం_res, పూర్ణాంకం remove, పూర్ణాంకం wait, పూర్ణాంకం timeout)
{
    పూర్ణాంకం		i;
    char_u	*జవాబు;
    జవాబు_T	*rep;
    పూర్ణాంకం		did_process = FALSE;
    time_t	ప్రారంభం;
    time_t	now;

    // When waiting, loop until the message waiting for is received.
    సమయం(&ప్రారంభం);
    కొరకు (;;)
    {
	// Reset this here, in case a message arrives while we are going
	// through the already received messages.
	జవాబు_received = 0;

	కొరకు (i = 0; i < జవాబు_లెక్క; ++i)
	{
	    rep = జవాబు_అంశం(i);
	    ఒకవేళ (rep->server == server
		    && ((rep->సమాసం_ఫలితం != 0) == (సమాసం_res != ఖాళీ)))
	    {
		// Save the values we've found for later
		జవాబు = rep->జవాబు;
		ఒకవేళ (సమాసం_res != ఖాళీ)
		    *సమాసం_res = rep->సమాసం_ఫలితం == 1 ? 0 : -1;

		ఒకవేళ (remove)
		{
		    // Move the rest of the list down to fill the gap
		    mch_memmove(rep, rep + 1,
				     (జవాబు_లెక్క - i - 1) * పరిమాణం(జవాబు_T));
		    --జవాబు_లెక్క;
		}

		// Return the reply to the caller, who takes on responsibility
		// for freeing it if "remove" is TRUE.
		తిరిగి జవాబు;
	    }
	}

	// If we got here, we didn't find a reply. Return immediately if the
	// "wait" parameter isn't set.
	ఒకవేళ (!wait)
	{
	    // Process pending messages once. Without this, looping on
	    // remote_peek() would never get the reply.
	    ఒకవేళ (!did_process)
	    {
		did_process = TRUE;
		serverProcessPendingMessages();
		కొనసాగు;
	    }
	    ఆపు;
	}

	// We need to wait for a reply. Enter a message loop until the
	// "reply_received" flag gets set.

	// Loop until we receive a reply
	అయితే (జవాబు_received == 0)
	{
# ifdef FEAT_TIMERS
	    // TODO: use the return value to decide how long to wait.
	    check_due_timer();
# endif
	    సమయం(&now);
	    ఒకవేళ (timeout > 0 && (now - ప్రారంభం) >= timeout)
		ఆపు;

	    // Wait for a SendMessage() call to us.  This could be the reply
	    // we are waiting for.  Use a timeout of a second, to catch the
	    // situation that the server died unexpectedly.
	    సందేశం_Wait_For_గుణిజం_Objects(0, ఖాళీ, TRUE, 1000, QS_ALLINPUT);

	    // If the server has died, give up
	    ఒకవేళ (!Is_కిటికీ(server))
		తిరిగి ఖాళీ;

	    serverProcessPendingMessages();
	}
    }

    తిరిగి ఖాళీ;
}

/*
 * Process any messages in the Windows message queue.
 */
    శూన్య
serverProcessPendingMessages(శూన్య)
{
    సందేశం_3 సందేశం;

    అయితే (Peek_సందేశం_W(&సందేశం, ఖాళీ, 0, 0, PM_REMOVE))
    {
	Translate_సందేశం(&సందేశం);
	Dispatch_సందేశం_W(&సందేశం);
    }
}

#అయితే_ముగింపు // FEAT_CLIENTSERVER

#ఒకవేళ defined(FEAT_GUI) || (defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT))

నిర్మాణం charset_pair
{
    string_T	name;
    BYTE	charset;
};

స్థిర నిర్మాణం charset_pair
charset_జతలు[] =
{
    {STR_LITERAL_INIT("ANSI"),		ANSI_CHARSET},
    {STR_LITERAL_INIT("CHINESEBIG5"),	CHINESEBIG5_CHARSET},
    {STR_LITERAL_INIT("DEFAULT"),	DEFAULT_CHARSET},
    {STR_LITERAL_INIT("HANGEUL"),	HANGEUL_CHARSET},
    {STR_LITERAL_INIT("OEM"),		OEM_CHARSET},
    {STR_LITERAL_INIT("SHIFTJIS"),	SHIFTJIS_CHARSET},
    {STR_LITERAL_INIT("SYMBOL"),	చిహ్నం_CHARSET},
    {STR_LITERAL_INIT("ARABIC"),	ARABIC_CHARSET},
    {STR_LITERAL_INIT("BALTIC"),	BALTIC_CHARSET},
    {STR_LITERAL_INIT("EASTEUROPE"),	EASTEUROPE_CHARSET},
    {STR_LITERAL_INIT("GB2312"),	GB2312_CHARSET},
    {STR_LITERAL_INIT("GREEK"),		GREEK_CHARSET},
    {STR_LITERAL_INIT("HEBREW"),	HEBREW_CHARSET},
    {STR_LITERAL_INIT("JOHAB"),		JOHAB_CHARSET},
    {STR_LITERAL_INIT("MAC"),		MAC_CHARSET},
    {STR_LITERAL_INIT("RUSSIAN"),	RUSSIAN_CHARSET},
    {STR_LITERAL_INIT("THAI"),		THAI_CHARSET},
    {STR_LITERAL_INIT("TURKISH"),	TURKISH_CHARSET}
# ifdef VIETNAMESE_CHARSET
    ,
    {STR_LITERAL_INIT("VIETNAMESE"),	VIETNAMESE_CHARSET}
# endif
};

నిర్మాణం quality_pair
{
    string_T	name;
    DWORD	quality;
};

స్థిర నిర్మాణం quality_pair
quality_జతలు[] = {
# ifdef CLEARTYPE_QUALITY
    {STR_LITERAL_INIT("CLEARTYPE"),	CLEARTYPE_QUALITY},
# endif
# ifdef ANTIALIASED_QUALITY
    {STR_LITERAL_INIT("ANTIALIASED"),	ANTIALIASED_QUALITY},
# endif
# ifdef NONANTIALIASED_QUALITY
    {STR_LITERAL_INIT("NONANTIALIASED"), NONANTIALIASED_QUALITY},
# endif
# ifdef PROOF_QUALITY
    {STR_LITERAL_INIT("PROOF"),		PROOF_QUALITY},
# endif
# ifdef DRAFT_QUALITY
    {STR_LITERAL_INIT("DRAFT"),		DRAFT_QUALITY},
# endif
    {STR_LITERAL_INIT("DEFAULT"),	DEFAULT_QUALITY}
};

/*
 * Convert a charset ID to a name.
 * Return NULL when not recognized.
 */
    అక్షరం *
charset_id2name(పూర్ణాంకం id)
{
    పూర్ణాంకం	i;

    కొరకు (i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(charset_జతలు); ++i)
    {
	ఒకవేళ ((BYTE)id == charset_జతలు[i].charset)
	    తిరిగి (అక్షరం *)charset_జతలు[i].name.string;
    }

    తిరిగి ఖాళీ;
}

/*
 * Convert a quality ID to a name.
 * Return NULL when not recognized.
 */
    అక్షరం *
quality_id2name(DWORD id)
{
    పూర్ణాంకం	i;

    కొరకు (i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(quality_జతలు); ++i)
    {
	ఒకవేళ (id == quality_జతలు[i].quality)
	    తిరిగి (అక్షరం *)quality_జతలు[i].name.string;
    }

    తిరిగి ఖాళీ;
}

// The default font height in 100% scaling (96dpi).
// (-16 in 96dpi equates to roughly 12pt)
# define DEFAULT_FONT_ఎత్తు	(-16)

స్థిర స్థిరరాశి LOGFONTW s_lfDefault =
{
    DEFAULT_FONT_ఎత్తు,
    0, 0, 0, FW_సాధారణ, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
    బహిర్గతం_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
    PROOF_QUALITY, FIXED_PITCH | FF_DONTCARE,
    L""	// Default font name will be set later based on current language.
};

// This will be initialized when set_default_logfont() is called first time.
// The value will be based on the system DPI.
పూర్ణాంకం current_font_ఎత్తు = 0;		// also used in gui_w32.c

/*
 * Convert a string representing a point size into pixels. The string should
 * be a positive decimal number, with an optional decimal point (eg, "12", or
 * "10.5"). The pixel value is returned, and a pointer to the next unconverted
 * character is stored in *end. The flag "vertical" says whether this
 * calculation is for a vertical (height) size or a horizontal (width) one.
 */
    స్థిర పూర్ణాంకం
points_గమ్యం_pixels(WCHAR *str, WCHAR **చివర, పూర్ణాంకం vertical, long_i pprinter_dc)
{
    పూర్ణాంకం		pixels;
    పూర్ణాంకం		points = 0;
    పూర్ణాంకం		భాజకం = 0;
    HWND	hwnd = (HWND)0;
    HDC		hdc;
    HDC		printer_dc = (HDC)pprinter_dc;

    అయితే (*str != NUL)
    {
	ఒకవేళ (*str == L'.' && భాజకం == 0)
	{
	    // Start keeping a divisor, for later
	    భాజకం = 1;
	}
	లేకపోతే
	{
	    ఒకవేళ (!VIM_ISDIGIT(*str))
		ఆపు;

	    points *= 10;
	    points += *str - L'0';
	    భాజకం *= 10;
	}
	++str;
    }

    ఒకవేళ (భాజకం == 0)
	భాజకం = 1;

    ఒకవేళ (printer_dc == ఖాళీ)
    {
	hwnd = Get_Desktop_కిటికీ();
	hdc = Get_కిటికీ_DC(hwnd);
    }
    లేకపోతే
	hdc = printer_dc;

    pixels = MulDiv(points,
		    GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX),
		    72 * భాజకం);

    ఒకవేళ (printer_dc == ఖాళీ)
	ReleaseDC(hwnd, hdc);

    *చివర = str;
    తిరిగి pixels;
}

/*
 * Convert pixel into point size. This is a reverse of points_to_pixels.
 */
    స్థిర ద్విగుణ
pixels_గమ్యం_points(పూర్ణాంకం pixels, పూర్ణాంకం vertical, long_i pprinter_dc)
{
    ద్విగుణ	points = 0;
    HWND	hwnd = (HWND)0;
    HDC		hdc;
    HDC		printer_dc = (HDC)pprinter_dc;

    ఒకవేళ (printer_dc == ఖాళీ)
    {
	hwnd = Get_Desktop_కిటికీ();
	hdc = Get_కిటికీ_DC(hwnd);
    }
    లేకపోతే
	hdc = printer_dc;

    points = pixels * 72.0 / GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX);
    ఒకవేళ (printer_dc == ఖాళీ)
	ReleaseDC(hwnd, hdc);

    తిరిగి points;
}

    స్థిర పూర్ణాంకం CALLBACK
font_enumproc(
    ENUMLOGFONTW    *elf,
    NEWTEXTMETRICW  *ntm వాడని,
    DWORD	    type వాడని,
    LPARAM	    lparam)
{
    // Return value:
    //	  0 = terminate now (monospace & ANSI)
    //	  1 = continue, still no luck...
    //	  2 = continue, but we have an acceptable LOGFONTW
    //	      (monospace, not ANSI)
    // We use these values, as EnumFontFamilies returns 1 if the
    // callback function is never called. So, we check the return as
    // 0 = perfect, 2 = OK, 1 = no good...
    // It's not pretty, but it works!

    LOGFONTW *lf = (LOGFONTW *)(lparam);

# ifndef FEAT_PROPORTIONAL_FONTS
    // Ignore non-monospace fonts without further ado
    ఒకవేళ ((ntm->tmPitchAndFamily & 1) != 0)
	తిరిగి 1;
# endif

    // Remember this LOGFONTW as a "possible"
    *lf = elf->elfLogFont;

    // Terminate the scan as soon as we find an ANSI font
    ఒకవేళ (lf->lf_Char_సమితి == ANSI_CHARSET
	    || lf->lf_Char_సమితి == OEM_CHARSET
	    || lf->lf_Char_సమితి == DEFAULT_CHARSET)
	తిరిగి 0;

    // Continue the scan - we have a non-ANSI font
    తిరిగి 2;
}

    స్థిర పూర్ణాంకం
init_logfont(LOGFONTW *lf)
{
    పూర్ణాంకం		n;
    HWND	hwnd = Get_Desktop_కిటికీ();
    HDC		hdc = Get_కిటికీ_DC(hwnd);

    n = EnumFontFamiliesW(hdc,
			 lf->lfFaceName,
			 (FONTENUMPROCW)font_enumproc,
			 (LPARAM)lf);

    ReleaseDC(hwnd, hdc);

    // If we couldn't find a usable font, return failure
    ఒకవేళ (n == 1)
	తిరిగి FAIL;

    // Tidy up the rest of the LOGFONTW structure. We set to a basic
    // font - get_logfont() sets bold, italic, etc based on the user's
    // input.
    lf->lf_ఎత్తు = current_font_ఎత్తు;
    lf->lf_వెడల్పు = 0;
    lf->lfItalic = FALSE;
    lf->lfUnderline = FALSE;
    lf->lf_Strike_బహిర్గతం = FALSE;
    lf->lf_బరువు = FW_సాధారణ;

    // Return success
    తిరిగి OK;
}

/*
 * Call back for EnumFontFamiliesW in expand_font_enumproc.
 *
 */
    స్థిర పూర్ణాంకం CALLBACK
expand_font_enumproc(
    ENUMLOGFONTW    *elf,
    NEWTEXTMETRICW  *ntm వాడని,
    DWORD	    type వాడని,
    LPARAM	    lparam)
{
    LOGFONTW *lf = (LOGFONTW*)elf;

# ifndef FEAT_PROPORTIONAL_FONTS
    // Ignore non-monospace fonts without further ado
    ఒకవేళ ((ntm->tmPitchAndFamily & 1) != 0)
	తిరిగి 1;
# endif

    // Filter only on ANSI. Otherwise will see a lot of random fonts that we
    // usually don't want.
    ఒకవేళ (lf->lf_Char_సమితి != ANSI_CHARSET)
	తిరిగి 1;

    పూర్ణాంకం (*కూడు_సరిపోలిక)(char_u *) = (పూర్ణాంకం (*)(char_u *))lparam;

    WCHAR *faceNameW = lf->lfFaceName;
    char_u *faceName = utf16_గమ్యం_enc(faceNameW, ఖాళీ);
    ఒకవేళ (!faceName)
	తిరిగి 0;

    కూడు_సరిపోలిక(faceName);
    vim_free(faceName);

    తిరిగి 1;
}

/*
 * Cmdline expansion for setting 'guifont'. Will enumerate through all
 * monospace fonts for completion. If used after ':', will expand to possible
 * font configuration options like font sizes.
 *
 * This function has "gui" in its name because in some platforms (GTK) font
 * handling is done by the GUI code, whereas in Windows it's part of the
 * platform code.
 */
    శూన్య
gui_mch_expand_font(
    optexpand_T	*వాదనలు,
    శూన్య	*param వాడని,
    పూర్ణాంకం		(*కూడు_సరిపోలిక)(char_u *val))
{
    expand_T	    *xp = వాదనలు->oe_xp;
    ఒకవేళ (xp->xp_నమూనా > వాదనలు->oe_సమితి_arg && *(xp->xp_నమూనా-1) == ':')
    {
	అక్షరం buf[30];
	పూర్ణాంకం	i;

	// Always fill in with the current font size as first option for
	// convenience. We simply round to the closest integer for simplicity.
	పూర్ణాంకం font_ఎత్తు = (పూర్ణాంకం)విడత(
		pixels_గమ్యం_points(-current_font_ఎత్తు, TRUE, (long_i)ఖాళీ));
	vim_snprintf(buf, ARRAY_నిడివి(buf), "h%d", font_ఎత్తు);
	కూడు_సరిపోలిక((char_u *)buf);

	// Note: Keep this in sync with get_logfont(). Don't include 'c' and
	// 'q' as we fill in all the values below.
	స్థిర అక్షరం *(p_gfn_win_opt_విలువలు[]) = {
	    "h" , "w" , "W" , "b" , "i" , "u" , "s"};
	కొరకు (i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(p_gfn_win_opt_విలువలు); i++)
	    కూడు_సరిపోలిక((char_u *)p_gfn_win_opt_విలువలు[i]);

	కొరకు (i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(charset_జతలు); ++i)
	{
	    vim_snprintf(buf, పరిమాణం(buf), "c%s", charset_జతలు[i].name.string);
	    కూడు_సరిపోలిక((char_u *)buf);
	}

	కొరకు (i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(quality_జతలు); ++i)
	{
	    vim_snprintf(buf, పరిమాణం(buf), "q%s", quality_జతలు[i].name.string);
	    కూడు_సరిపోలిక((char_u *)buf);
	}
	తిరిగి;
    }

    HWND	hwnd = Get_Desktop_కిటికీ();
    HDC		hdc = Get_కిటికీ_DC(hwnd);

    EnumFontFamiliesW(hdc,
	    ఖాళీ,
	    (FONTENUMPROCW)expand_font_enumproc,
	    (LPARAM)కూడు_సరిపోలిక);

    ReleaseDC(hwnd, hdc);
}

/*
 * Compare a UTF-16 string and an ASCII string literally.
 * Only works all the code points are inside ASCII range.
 */
    స్థిర పూర్ణాంకం
utf16ascncmp(స్థిరరాశి WCHAR *w, స్థిరరాశి అక్షరం *p, size_t n)
{
    size_t i;

    కొరకు (i = 0; i < n; i++)
    {
	ఒకవేళ (w[i] == 0 || w[i] != p[i])
	    తిరిగి w[i] - p[i];
    }
    తిరిగి 0;
}

/*
 * Equivalent of GetDpiForSystem().
 */
    UINT WINAPI
vimGetDpiForSystem(శూన్య)
{
    HWND hwnd = Get_Desktop_కిటికీ();
    HDC hdc = Get_కిటికీ_DC(hwnd);
    UINT dpi = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(hwnd, hdc);
    తిరిగి dpi;
}

/*
 * Set default logfont based on current language.
 */
    స్థిర శూన్య
సమితి_default_logfont(LOGFONTW *lf)
{
    // Default font name for current language on MS-Windows.
    // If not translated, falls back to "Consolas".
    // This must be a fixed-pitch font.
    స్థిరరాశి అక్షరం *defaultfontname = N_("DefaultFontNameForWindows");
    అక్షరం *fontname = _(defaultfontname);

    ఒకవేళ (STRCMP(fontname, defaultfontname) == 0)
	fontname = "Consolas";

    *lf = s_lfDefault;
    lf->lf_ఎత్తు = DEFAULT_FONT_ఎత్తు * (పూర్ణాంకం)vimGetDpiForSystem() / 96;
    ఒకవేళ (current_font_ఎత్తు == 0)
	current_font_ఎత్తు = lf->lf_ఎత్తు;

    WCHAR *wfontname = enc_గమ్యం_utf16((char_u*)fontname, ఖాళీ);
    ఒకవేళ (wfontname != ఖాళీ)
    {
	wcscpy_s(lf->lfFaceName, LF_FACESIZE, wfontname);
	vim_free(wfontname);
    }
}

/*
 * Get font info from "name" into logfont "lf".
 * Return OK for a valid name, FAIL otherwise.
 */
    పూర్ణాంకం
get_logfont(
    LOGFONTW	*lf,
    char_u	*name,
    HDC		printer_dc,
    పూర్ణాంకం		verbose)
{
    WCHAR	*p;
    పూర్ణాంకం		i;
    పూర్ణాంకం		ret = FAIL;
    స్థిర LOGFONTW *lastlf = ఖాళీ;
    WCHAR	*wname;

    సమితి_default_logfont(lf);
    ఒకవేళ (name == ఖాళీ)
	తిరిగి OK;

    wname = enc_గమ్యం_utf16(name, ఖాళీ);
    ఒకవేళ (wname == ఖాళీ)
	తిరిగి FAIL;

    ఒకవేళ (wcscmp(wname, L"*") == 0)
    {
# ఒకవేళ defined(FEAT_GUI_MSWIN)
	CHOOSEFONTW	cf;
	// if name is "*", bring up std font dialog:
	CLEAR_క్షేత్రం(cf);
	cf.l_Struct_కొలత = పరిమాణం(cf);
	cf.hwndOwner = s_hwnd;
	cf.Flags = CF_SCREENFONTS | CF_FIXEDPITCHONLY | CF_INITTOLOGFONTSTRUCT;
	ఒకవేళ (lastlf != ఖాళీ)
	    *lf = *lastlf;
	cf.lpLogFont = lf;
	cf.nFontType = 0 ; //REGULAR_FONTTYPE;
	ఒకవేళ (ఎంచుకో_Font_W(&cf))
	    ret = OK;
# endif
	వెళ్ళు theend;
    }

    /*
     * Split name up, it could be <name>:h<height>:w<width> etc.
     */
    కొరకు (p = wname; *p && *p != L':'; p++)
    {
	ఒకవేళ (p - wname + 1 >= LF_FACESIZE)
	    వెళ్ళు theend;			// Name too long
	lf->lfFaceName[p - wname] = *p;
    }
    ఒకవేళ (p != wname)
	lf->lfFaceName[p - wname] = NUL;

    // First set defaults
    lf->lf_ఎత్తు = DEFAULT_FONT_ఎత్తు * (పూర్ణాంకం)vimGetDpiForSystem() / 96;
    lf->lf_వెడల్పు = 0;
    lf->lf_బరువు = FW_సాధారణ;
    lf->lfItalic = FALSE;
    lf->lfUnderline = FALSE;
    lf->lf_Strike_బహిర్గతం = FALSE;

    /*
     * If the font can't be found, try replacing '_' by ' '.
     */
    ఒకవేళ (init_logfont(lf) == FAIL)
    {
	పూర్ణాంకం	did_replace = FALSE;

	కొరకు (i = 0; lf->lfFaceName[i]; ++i)
	    ఒకవేళ (lf->lfFaceName[i] == L'_')
	    {
		lf->lfFaceName[i] = L' ';
		did_replace = TRUE;
	    }
	ఒకవేళ (!did_replace || init_logfont(lf) == FAIL)
	    వెళ్ళు theend;
    }

    అయితే (*p == L':')
	p++;

    // Set the values found after ':'
    అయితే (*p)
    {
	ఎంపిక (*p++)
	{
	    // Note: Keep this in sync with gui_mch_expand_font().
	    స్థితి L'h':
		lf->lf_ఎత్తు = - points_గమ్యం_pixels(p, &p, TRUE, (long_i)printer_dc);
		ఆపు;
	    స్థితి L'w':
		lf->lf_వెడల్పు = points_గమ్యం_pixels(p, &p, FALSE, (long_i)printer_dc);
		ఆపు;
	    స్థితి L'W':
		lf->lf_బరువు = wcstol(p, &p, 10);
		ఆపు;
	    స్థితి L'b':
		lf->lf_బరువు = FW_BOLD;
		ఆపు;
	    స్థితి L'i':
		lf->lfItalic = TRUE;
		ఆపు;
	    స్థితి L'u':
		lf->lfUnderline = TRUE;
		ఆపు;
	    స్థితి L's':
		lf->lf_Strike_బహిర్గతం = TRUE;
		ఆపు;
	    స్థితి L'c':
		కొరకు (i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(charset_జతలు); ++i)
		{
		    ఒకవేళ (utf16ascncmp(p, (అక్షరం *)charset_జతలు[i].name.string,
			    charset_జతలు[i].name.నిడివి) == 0)
		    {
			lf->lf_Char_సమితి = charset_జతలు[i].charset;
			p += charset_జతలు[i].name.నిడివి;
			ఆపు;
		    }
		}

		ఒకవేళ (i == (పూర్ణాంకం)ARRAY_నిడివి(charset_జతలు) && verbose)
		{
		    char_u *s = utf16_గమ్యం_enc(p, ఖాళీ);
		    ఒకవేళ (s != ఖాళీ)
		    {
			semsg(_(e_illegal_str_name_str_లోపల_font_name_str),
							   "charset", s, name);
			vim_free(s);
		    }
		}
		ఆపు;
	    స్థితి L'f':
		// Font features (e.g., "fss19=1").
		// Parsed separately by gui_mch_init_font(); skip here.
		అయితే (*p && *p != L':')
		    p++;
		ఆపు;
	    స్థితి L'q':
		కొరకు (i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(quality_జతలు); ++i)
		{
		    ఒకవేళ (utf16ascncmp(p, (అక్షరం *)quality_జతలు[i].name.string,
			    quality_జతలు[i].name.నిడివి) == 0)
		    {
			lf->lfQuality = quality_జతలు[i].quality;
			p += quality_జతలు[i].name.నిడివి;
			ఆపు;
		    }
		}

		ఒకవేళ (i == (పూర్ణాంకం)ARRAY_నిడివి(quality_జతలు) && verbose)
		{
		    char_u *s = utf16_గమ్యం_enc(p, ఖాళీ);
		    ఒకవేళ (s != ఖాళీ)
		    {
			semsg(_(e_illegal_str_name_str_లోపల_font_name_str),
							   "quality", s, name);
			vim_free(s);
		    }
		}
		ఆపు;
	    అప్రమేయ:
		ఒకవేళ (verbose)
		    semsg(_(e_illegal_char_nr_లోపల_font_name_str), p[-1], name);
		వెళ్ళు theend;
	}
	అయితే (*p == L':')
	    p++;
    }
    ret = OK;

theend:
    // ron: init lastlf
    ఒకవేళ (ret == OK && printer_dc == ఖాళీ)
    {
	vim_free(lastlf);
	lastlf = ALLOC_ONE(LOGFONTW);
	ఒకవేళ (lastlf != ఖాళీ)
	    mch_memmove(lastlf, lf, పరిమాణం(LOGFONTW));
    }
    vim_free(wname);

    తిరిగి ret;
}

#అయితే_ముగింపు // defined(FEAT_GUI) || defined(FEAT_PRINTER)

#ఒకవేళ defined(FEAT_పని_CHANNEL)
/*
 * Initialize the Winsock dll.
 */
    శూన్య
channel_init_winsock(శూన్య)
{
    WSADATA wsa_దత్తాంశం;
    పూర్ణాంకం wsaerr;

    ఒకవేళ (WSInitialized)
	తిరిగి;

    wsaerr = WSAStartup(MAKEWORD(2, 2), &wsa_దత్తాంశం);
    ఒకవేళ (wsaerr == 0)
	WSInitialized = TRUE;
}
#అయితే_ముగింపు
