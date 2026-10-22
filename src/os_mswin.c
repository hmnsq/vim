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

#অন্তর্ভাৱ "vim.h"

#অন্তর্ভাৱ <sys/types.h>
#অন্তর্ভাৱ <limits.h>

#অন্তর্ভাৱ <process.h>
#অন্তর্ভাৱ <direct.h>

#যদি !defined(FEAT_GUI_MSWIN)
# include <shellapi.h>
#endif

#যদি defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT)
# include <dlgs.h>
# include <winspool.h>
# include <commdlg.h>
#endif

// Record all output and all keyboard & mouse input
// #define MCH_WRITE_DUMP

#ifdef MCH_WRITE_DUMP
FILE* fdDump = রিক্ত;
#endif

#যদি !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
বাহ্য ৱর্ণ g_szOrigTitle[];
#endif

#ifdef FEAT_GUI
বাহ্য HWND s_hwnd;
#অন্যথা
স্থির HWND s_hwnd = 0;	    // console window handle, set by GetConsoleHwnd()
#endif

#ifdef FEAT_JOB_CHANNEL
পূর্ণাঙ্ক WSInitialized = FALSE; // WinSock is initialized
#endif


/*
 * Save the instance handle of the exe/dll.
 */
    শূন্য
SaveInst(HINSTANCE hInst)
{
    g_hinst = hInst;
}

#যদি defined(FEAT_GUI_MSWIN)
/*
 * GUI version of mch_exit().
 * Shut down and exit with status `r'
 * Careful: mch_exit() may be called before mch_init()!
 */
    শূন্য
mch_exit_g(পূর্ণাঙ্ক r)
{
    exiting = TRUE;

    display_errors();

    ml_close_all(TRUE);		// remove all memfiles

# ifdef FEAT_OLE
    UninitOLE();
# endif
# ifdef FEAT_JOB_CHANNEL
    যদি (WSInitialized)
    {
	WSInitialized = FALSE;
	WSACleanup();
    }
# endif
# ifdef DYNAMIC_GETTEXT
    dyn_libintl_end();
# endif

    যদি (gui.in_use)
	gui_exit(r);

# ifdef EXITFREE
    free_all_mem();
# endif

    exit(r);
}

#endif // FEAT_GUI_MSWIN


/*
 * Get version number including build number
 */
প্রকারনির্দেশ BOOL (WINAPI *PfnRtlGetVersion)(LPOSVERSIONINFOW);
DWORD win_version;
    স্থির শূন্য
win_version_init(শূন্য)
{
    OSVERSIONINFOW	osver;
    HMODULE		hNtdll;
    PfnRtlGetVersion	pRtlGetVersion;

    hNtdll = GetModuleHandle("ntdll.dll");
    যদি (hNtdll == রিক্ত)
	ঘূৰাই;

    pRtlGetVersion =
	(PfnRtlGetVersion) GetProcAddress(hNtdll, "RtlGetVersion");
    যদি (pRtlGetVersion == রিক্ত)
	ঘূৰাই;

    osver.dwOSVersionInfoSize = পরিমাণ(OSVERSIONINFOW);
    pRtlGetVersion(&osver);
    win_version = MAKE_VER(min(osver.dwMajorVersion, 0xFF),
	    min(osver.dwMinorVersion, 0xFF),
	    min(osver.dw_Build_সংখ্যা, 0xFFFF));
}

/*
 * Init the tables for toupper() and tolower().
 */
    শূন্য
mch_early_init(শূন্য)
{
    পূর্ণাঙ্ক		i;

    win_version_init();

    PlatformId();

    // Init the tables for toupper() and tolower()
    বাবে (i = 0; i < 256; ++i)
	toupper_tab[i] = tolower_tab[i] = i;
    CharUpperBuff((LPSTR)toupper_tab, 256);
    CharLowerBuff((LPSTR)tolower_tab, 256);
}


/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    পূর্ণাঙ্ক
mch_প্ৰবেশ_isatty(শূন্য)
{
#ifdef FEAT_GUI_MSWIN
# ifdef VIMDLL
    যদি (gui.in_use)
# endif
	ঘূৰাই TRUE;	    // GUI always has a tty
#endif
#যদি !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    যদি (isatty(read_cmd_fd))
	ঘূৰাই TRUE;
    ঘূৰাই FALSE;
#endif
}

/*
 * mch_settitle(): set titlebar of our window
 */
    শূন্য
mch_settitle(
    char_u *title,
    char_u *icon UNUSED)
{
#ifdef FEAT_GUI_MSWIN
# ifdef VIMDLL
    যদি (gui.in_use)
# endif
    {
	gui_mch_settitle(title, icon);
	ঘূৰাই;
    }
#endif
#যদি !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    যদি (title != রিক্ত)
    {
	WCHAR	*wp = enc_to_utf16(title, রিক্ত);

	যদি (wp == রিক্ত)
	    ঘূৰাই;

	SetConsoleTitleW(wp);
	vim_free(wp);
	ঘূৰাই;
    }
#endif
}


/*
 * Restore the window/icon title.
 * which is one of:
 *  SAVE_RESTORE_TITLE: Just restore title
 *  SAVE_RESTORE_ICON:  Just restore icon (which we don't have)
 *  SAVE_RESTORE_BOTH:  Restore title and icon (which we don't have)
 */
    শূন্য
mch_restore_title(পূর্ণাঙ্ক which UNUSED)
{
#যদি !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
    যদি (!gui.in_use)
# endif
	SetConsoleTitle(g_szOrigTitle);
#endif
}


/*
 * Return TRUE if we can restore the title (we can)
 */
    পূর্ণাঙ্ক
mch_can_restore_title(শূন্য)
{
    ঘূৰাই TRUE;
}


/*
 * Return TRUE if we can restore the icon title (we can't)
 */
    পূর্ণাঙ্ক
mch_can_restore_icon(শূন্য)
{
    ঘূৰাই FALSE;
}


/*
 * Get absolute file name into buffer "buf" of length "len" bytes,
 * turning all '/'s into '\\'s and getting the correct case of each component
 * of the file name.  Append a (back)slash to a directory name.
 * When 'shellslash' set do it the other way around.
 * Return OK or FAIL.
 */
    পূর্ণাঙ্ক
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    পূর্ণাঙ্ক		len,
    পূর্ণাঙ্ক		force UNUSED)
{
    পূর্ণাঙ্ক		nResult = FAIL;
    WCHAR	*wname;
    WCHAR	wbuf[MAX_PATH];
    char_u	*cname = রিক্ত;

    wname = enc_to_utf16(fname, রিক্ত);
    যদি (wname != রিক্ত && _wfullpath(wbuf, wname, MAX_PATH) != রিক্ত)
    {
	cname = utf16_to_enc((short_u *)wbuf, রিক্ত);
	যদি (cname != রিক্ত)
	{
	    vim_strncpy(buf, cname, len - 1);
	    nResult = OK;
	}
    }
    vim_free(wname);
    vim_free(cname);

#ifdef USE_FNAME_CASE
    fname_case(buf, len);
#অন্যথা
    slash_adjust(buf);
#endif

    ঘূৰাই nResult;
}


/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    পূর্ণাঙ্ক
mch_isFullName(char_u *fname)
{
    // A name like "d:/foo" and "//server/share" is absolute.  "d:foo" is not.
    // /foo and \foo are absolute too because Windows keeps a current drive.
    // Another way to check is to use mch_FullName() and see if the result is
    // the same as the name or mch_FullName() fails.  However, this has quite a
    // bit of overhead, so let's not do that.
    যদি (*fname == NUL)
	ঘূৰাই FALSE;
    ঘূৰাই ((ASCII_ISALPHA(fname[0]) && fname[1] == ':'
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
    শূন্য
slash_adjust(char_u *p)
{
    যদি (path_with_url(p))
	ঘূৰাই;

    যদি (*p == '`')
    {
	size_t len = STRLEN(p);

	// don't replace backslash in backtick quoted strings
	যদি (len > 2 && *(p + len - 1) == '`')
	    ঘূৰাই;
    }

    যেতিয়ালৈ (*p)
    {
	যদি (*p == psepcN)
	    *p = psepc;
	MB_PTR_ADV(p);
    }
}

    স্থির পূর্ণাঙ্ক
read_reparse_point(ধ্রুৱ WCHAR *name, char_u *buf, DWORD *buf_len)
{
    HANDLE h;
    BOOL ok;

    h = CreateFileW(name, FILE_READ_ATTRIBUTES,
	    FILE_SHARE_READ | FILE_SHARE_WRITE, রিক্ত,
	    OPEN_EXISTING,
	    FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,
	    রিক্ত);
    যদি (h == INVALID_HANDLE_মান)
	ঘূৰাই FAIL;

    ok = DeviceIoControl(h, FSCTL_GET_REPARSE_POINT, রিক্ত, 0, buf, *buf_len,
	    buf_len, রিক্ত);
    CloseHandle(h);

    ঘূৰাই ok ? OK : FAIL;
}

    char_u *
resolve_appexeclink(char_u *fname)
{
    DWORD		attr = 0;
    পূর্ণাঙ্ক			idx;
    WCHAR		*p, *end, *wname;
    // The buffer size is arbitrarily chosen to be "big enough" (TM), the
    // ceiling should be around 16k.
    char_u		buf[4096];
    DWORD		buf_len = পরিমাণ(buf);
    REPARSE_DATA_BUFFER *rb = (REPARSE_DATA_BUFFER *)buf;

    wname = enc_to_utf16(fname, রিক্ত);
    যদি (wname == রিক্ত)
	ঘূৰাই রিক্ত;

    attr = GetFileAttributesW(wname);
    যদি (attr == INVALID_FILE_ATTRIBUTES ||
	    (attr & FILE_ATTRIBUTE_REPARSE_POINT) == 0)
    {
	vim_free(wname);
	ঘূৰাই রিক্ত;
    }

    // The applinks are similar to symlinks but with a huge difference: they can
    // only be executed, any other I/O operation on them is bound to fail with
    // ERROR_FILE_NOT_FOUND even though the file exists.
    যদি (read_reparse_point(wname, buf, &buf_len) == FAIL)
    {
	vim_free(wname);
	ঘূৰাই রিক্ত;
    }
    vim_free(wname);

    যদি (rb->ReparseTag != IO_REPARSE_TAG_APPEXECLINK)
	ঘূৰাই রিক্ত;

    // The (undocumented) reparse buffer contains a set of N null-terminated
    // Unicode strings, the application path is stored in the third one.
    যদি (rb->AppExecLinkReparseBuffer.String_গুনতি < 3)
	ঘূৰাই রিক্ত;

    p = rb->AppExecLinkReparseBuffer.StringList;
    end = p + rb->ReparseDataLength / পরিমাণ(WCHAR);
    বাবে (idx = 0; p < end
	    && idx < (পূর্ণাঙ্ক)rb->AppExecLinkReparseBuffer.String_গুনতি
	    && idx != 2; )
    {
	যদি (*p++ == L'\0')
	    ++idx;
    }

    ঘূৰাই utf16_to_enc(p, রিক্ত);
}

// Use 64-bit stat functions.
#undef stat
#undef _stat
#undef _wstat
#undef _fstat
#define stat _stat64
#define _stat _stat64
#define _wstat _wstat64
#define _fstat _fstat64

/*
 * Implements lstat() and stat() that can handle symlinks properly.
 */
    স্থির পূর্ণাঙ্ক
mswin_stat_impl(ধ্রুৱ WCHAR *name, stat_T *stp, ধ্রুৱ পূর্ণাঙ্ক resolve)
{
    পূর্ণাঙ্ক			n;
    পূর্ণাঙ্ক			fd;
    BOOL		is_symlink = FALSE;
    HANDLE		hFind, h;
    DWORD		attr = 0;
    DWORD		flag = 0;
    WIN32_FIND_DATAW    findDataW;

#যদি 0 && defined(_UCRT)
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
    যদি (resolve)
	// Universal CRT can handle symlinks properly.
	ঘূৰাই _wstat(name, stp);
#endif

    // Only reparse points need a directory query to identify symbolic links.
    // In particular, avoid enumerating the directory for each missing file
    // while searching 'runtimepath'.
    attr = GetFileAttributesW(name);
    যদি (attr == INVALID_FILE_ATTRIBUTES)
    {
	DWORD error = GetLastError();

	// A missing file or path cannot provide stat information.  Other
	// errors and UNC paths still need the CRT fallback.
	যদি ((error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND)
		&& !((name[0] == L'\\' || name[0] == L'/')
		    && (name[1] == L'\\' || name[1] == L'/')))
	{
	    errno = ENOENT;
	    ঘূৰাই -1;
	}
	ঘূৰাই _wstat(name, stp);
    }
    যদি (!(attr & FILE_ATTRIBUTE_REPARSE_POINT))
	ঘূৰাই _wstat(name, stp);

    hFind = FindFirstFileW(name, &findDataW);
    যদি (hFind != INVALID_HANDLE_মান)
    {
	attr = findDataW.dwFileAttributes;
	যদি ((attr & FILE_ATTRIBUTE_REPARSE_POINT)
		&& (findDataW.dwReserved0 == IO_REPARSE_TAG_SYMLINK))
	    is_symlink = TRUE;
	FindClose(hFind);
    }

    // Use the plain old stat() whenever it's possible.
    যদি (!is_symlink)
	ঘূৰাই _wstat(name, stp);

    যদি (!resolve && is_symlink)
	flag = FILE_FLAG_OPEN_REPARSE_POINT;
    যদি (attr & FILE_ATTRIBUTE_DIRECTORY)
	flag |= FILE_FLAG_BACKUP_SEMANTICS;

    h = CreateFileW(name, FILE_READ_ATTRIBUTES,
	    FILE_SHARE_READ | FILE_SHARE_WRITE, রিক্ত, OPEN_EXISTING, flag,
	    রিক্ত);
    যদি (h == INVALID_HANDLE_মান)
	ঘূৰাই -1;

    fd = _open_osfhandle((intptr_t)h, _O_RDONLY);
    n = _fstat(fd, (সংরচনা _stat *)stp);
    যদি ((n == 0) && (attr & FILE_ATTRIBUTE_DIRECTORY))
	stp->st_mode = (stp->st_mode & ~S_IFMT) | S_IFDIR;
    _close(fd);

    ঘূৰাই n;
}

/*
 * stat() can't handle a trailing '/' or '\', remove it first.
 * When 'resolve' is true behave as lstat() wrt symlinks.
 */
    স্থির পূর্ণাঙ্ক
stat_impl(ধ্রুৱ ৱর্ণ *name, stat_T *stp, ধ্রুৱ পূর্ণাঙ্ক resolve)
{
    // WinNT and later can use _MAX_PATH wide characters for a pathname, which
    // means that the maximum pathname is _MAX_PATH * 3 bytes when 'enc' is
    // UTF-8.
    char_u	buf[_MAX_PATH * 3 + 1];
    size_t	buflen;
    char_u	*p;
    WCHAR	*wp;
    পূর্ণাঙ্ক		n;

    vim_strncpy((char_u *)buf, (char_u *)name, পরিমাণ(buf) - 1);
    buflen = STRLEN(buf);
    p = buf + buflen;
    যদি (p > buf)
	MB_PTR_BACK(buf, p);

    // Remove trailing '\\' except root path.
    যদি (p > buf && (*p == '\\' || *p == '/') && p[-1] != ':')
    {
	*p = NUL;
	buflen = (size_t)(p - buf);
    }

    যদি ((buf[0] == '\\' && buf[1] == '\\') || (buf[0] == '/' && buf[1] == '/'))
    {
	// UNC root path must be followed by '\\'.
	p = vim_strpbrk(buf + 2, (char_u *)"\\/");
	যদি (p != রিক্ত)
	{
	    p = vim_strpbrk(p + 1, (char_u *)"\\/");
	    যদি (p == রিক্ত)
		STRCPY(buf + buflen, "\\");
	}
    }

    wp = enc_to_utf16(buf, রিক্ত);
    যদি (wp == রিক্ত)
	ঘূৰাই -1;

    n = mswin_stat_impl(wp, stp, resolve);
    vim_free(wp);
    ঘূৰাই n;
}

    পূর্ণাঙ্ক
vim_lstat(ধ্রুৱ ৱর্ণ *name, stat_T *stp)
{
    ঘূৰাই stat_impl(name, stp, FALSE);
}

    পূর্ণাঙ্ক
vim_stat(ধ্রুৱ ৱর্ণ *name, stat_T *stp)
{
    ঘূৰাই stat_impl(name, stp, TRUE);
}

#যদি defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    শূন্য
mch_settmode(tmode_T tmode UNUSED)
{
    // nothing to do
}

    পূর্ণাঙ্ক
mch_get_shellsize(শূন্য)
{
    // never used
    ঘূৰাই OK;
}

    শূন্য
mch_set_shellsize(শূন্য)
{
    // never used
}

/*
 * Rows and/or Columns has changed.
 */
    শূন্য
mch_new_shellsize(শূন্য)
{
    // never used
}

    শূন্য
mch_calc_cell_size(সংরচনা cellsize *cs_out UNUSED)
{
    // never used
}

#endif

/*
 * We have no job control, so fake it by starting a new shell.
 */
    শূন্য
mch_suspend(শূন্য)
{
    suspend_shell();
}

#যদি defined(USE_MCH_ERRMSG)

# ifdef display_errors
#  undef display_errors
# endif

/*
 * Display the saved error message(s).
 */
    শূন্য
display_errors(শূন্য)
{
# ifdef FEAT_GUI
    char_u *p;

#  ifdef VIMDLL
    যদি (gui.in_use || gui.starting)
#  endif
    {
	যদি (error_ga.ga_data != রিক্ত)
	{
	    // avoid putting up a message box with blanks only
	    বাবে (p = (char_u *)error_ga.ga_data; *p; ++p)
		যদি (!SAFE_isspace(*p))
		{
		    // Only use a dialog when not using --gui-dialog-file:
		    // write text to a file.
		    যদি (!gui_dialog_log((char_u *)"Errors", p))
			(শূন্য)gui_mch_dialog(
				     gui.starting ? VIM_INFO :
					     VIM_ERROR,
				     gui.starting ? (char_u *)_("Message") :
					     (char_u *)_("Error"),
					     p, (char_u *)_("&Ok"),
					1, রিক্ত, FALSE);
		    ভঙ্গ;
		}
	    ga_clear(&error_ga);
	}
	ঘূৰাই;
    }
# endif
# যদি !defined(FEAT_GUI) || defined(VIMDLL)
    FlushFileBuffers(GetStdHandle(STD_ERROR_HANDLE));
# endif
}
#endif


/*
 * Return TRUE if "p" contain a wildcard that can be expanded by
 * dos_expandpath().
 */
    পূর্ণাঙ্ক
mch_has_exp_wildcard(char_u *p)
{
    বাবে ( ; *p; MB_PTR_ADV(p))
    {
	যদি (vim_strchr((char_u *)"?*[", *p) != রিক্ত
		|| (*p == '~' && p[1] != NUL))
	    ঘূৰাই TRUE;
    }
    ঘূৰাই FALSE;
}

/*
 * Return TRUE if "p" contain a wildcard or a "~1" kind of thing (could be a
 * shortened file name).
 */
    পূর্ণাঙ্ক
mch_has_wildcard(char_u *p)
{
    বাবে ( ; *p; MB_PTR_ADV(p))
    {
	যদি (vim_strchr((char_u *)"?*$[`", *p) != রিক্ত
		|| (*p == '~' && p[1] != NUL))
	    ঘূৰাই TRUE;
    }
    ঘূৰাই FALSE;
}


/*
 * The normal _chdir() does not change the default drive.  This one does.
 * Returning 0 implies success; -1 implies failure.
 */
    পূর্ণাঙ্ক
mch_chdir(ৱর্ণ *path)
{
    WCHAR   *p;
    পূর্ণাঙ্ক	    n;

    যদি (path[0] == NUL)		// just checking...
	ঘূৰাই -1;

    যদি (p_verbose >= 5)
    {
	verbose_enter();
	smsg("chdir(%s)", path);
	verbose_leave();
    }
    যদি (SAFE_isalpha(path[0]) && path[1] == ':')	// has a drive name
    {
	// If we can change to the drive, skip that part of the path.  If we
	// can't then the current directory may be invalid, try using chdir()
	// with the whole path.
	যদি (_chdrive(TOLOWER_ASC(path[0]) - 'a' + 1) == 0)
	    path += 2;
    }

    যদি (*path == NUL)		// drive name only
	ঘূৰাই 0;

    p = enc_to_utf16((char_u *)path, রিক্ত);
    যদি (p == রিক্ত)
	ঘূৰাই -1;

    n = _wchdir(p);
    vim_free(p);
    ঘূৰাই n;
}


#যদি defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
/*
 * return non-zero if a character is available
 */
    পূর্ণাঙ্ক
mch_char_avail(শূন্য)
{
    // never used
    ঘূৰাই TRUE;
}

# যদি defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    পূর্ণাঙ্ক
mch_check_messages(শূন্য)
{
    // TODO: check for messages
    ঘূৰাই TRUE;
}
# endif
#endif


#যদি defined(FEAT_LIBCALL)
/*
 * Call a DLL routine which takes either a string or int param
 * and returns an allocated string.
 * Return OK if it worked, FAIL if not.
 */
প্রকারনির্দেশ char_u *(*MYSTRPROCSTR)(char_u *);
প্রকারনির্দেশ পূর্ণাঙ্ক (*MYSTRPROCINT)(char_u *);
প্রকারনির্দেশ char_u *(*MYINTPROCSTR)(পূর্ণাঙ্ক);
প্রকারনির্দেশ পূর্ণাঙ্ক (*MYINTPROCINT)(পূর্ণাঙ্ক);

/*
 * Check if a pointer points to a valid NUL terminated string.
 * Return the length of the string, including terminating NUL.
 * Returns 0 for an invalid pointer, 1 for an empty string.
 */
    স্থির size_t
check_str_len(char_u *str)
{
    SYSTEM_INFO			si;
    MEMORY_BASIC_INFORMATION	mbi;
    size_t			length = 0;
    size_t			i;
    ধ্রুৱ char_u		*p;

    // get page size
    GetSystemInfo(&si);

    // get memory information
    যদি (!VirtualQuery(str, &mbi, পরিমাণ(mbi)))
	ঘূৰাই 0;

    // pre cast these (typing savers)
    long_u dwStr = (long_u)str;
    long_u dwBaseAddress = (long_u)mbi.BaseAddress;

    // get start address of page that str is on
    long_u strPage = dwStr - (dwStr - dwBaseAddress) % si.dwPageSize;

    // get length from str to end of page
    long_u pageLength = si.dwPageSize - (dwStr - strPage);

    বাবে (p = str; !IsBadReadPtr(p, (UINT)pageLength);
	    p += pageLength, pageLength = si.dwPageSize)
	বাবে (i = 0; i < pageLength; ++i, ++length)
	    যদি (p[i] == NUL)
		ঘূৰাই length + 1;

    ঘূৰাই 0;
}

/*
 * Passed to do_in_runtimepath() to load a vim.ico file.
 */
    স্থির শূন্য
mch_icon_load_cb(char_u *fname, শূন্য *cookie)
{
    HANDLE *h = (HANDLE *)cookie;

    *h = LoadImage(রিক্ত,
		   (LPSTR)fname,
		   IMAGE_ICON,
		   64,
		   64,
		   LR_LOADFROMFILE | LR_LOADMAP3DCOLORS);
}

/*
 * Try loading an icon file from 'runtimepath'.
 */
    পূর্ণাঙ্ক
mch_icon_load(HANDLE *iconp)
{
    ঘূৰাই do_in_runtimepath((char_u *)"bitmaps/vim.ico",
						  0, mch_icon_load_cb, iconp);
}

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if the OS PRNG is not available or something went wrong.
 */
    পূর্ণাঙ্ক
mch_get_random(char_u *buf, পূর্ণাঙ্ক len)
{
    স্থির পূর্ণাঙ্ক		initialized = NOTDONE;
    স্থির HINSTANCE	hInstLib;
    স্থির BOOL (WINAPI *pProcessPrng)(PUCHAR, ULONG);

    যদি (initialized == NOTDONE)
    {
	hInstLib = vimLoadLib("bcryptprimitives.dll");
	যদি (hInstLib != রিক্ত)
	    pProcessPrng = (শূন্য *)GetProcAddress(hInstLib, "ProcessPrng");
	যদি (hInstLib == রিক্ত || pProcessPrng == রিক্ত)
	{
	    FreeLibrary(hInstLib);
	    initialized = FAIL;
	}
	অন্যথা
	    initialized = OK;
    }

    যদি (initialized == FAIL)
	ঘূৰাই FAIL;

    // According to the documentation this call cannot fail.
    pProcessPrng(buf, len);

    ঘূৰাই OK;
}

    পূর্ণাঙ্ক
mch_libcall(
    char_u	*libname,
    char_u	*funcname,
    char_u	*argstring,	// NULL when using a argint
    পূর্ণাঙ্ক		argint,
    char_u	**string_result,// NULL when using number_result
    পূর্ণাঙ্ক		*সংখ্যা_result)
{
    HINSTANCE		hinstLib;

    BOOL fRunTimeLinkSuccess = FALSE;

    // Get a handle to the DLL module.
    hinstLib = vimLoadLib((ৱর্ণ *)libname);

    // If the handle is valid, try to get the function address.
    যদি (hinstLib != রিক্ত)
    {
	char_u	*retval_str = রিক্ত;
	পূর্ণাঙ্ক	retval_int = 0;
	size_t	len;

# ifdef HAVE_TRY_EXCEPT
	__try
	{
# endif
	যদি (argstring != রিক্ত)
	{
	    // Call with string argument
	    যদি (string_result != রিক্ত)
	    {
		MYSTRPROCSTR proc;

		proc = (MYSTRPROCSTR)GetProcAddress(hinstLib, (LPCSTR)funcname);
		যদি (proc != রিক্ত)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_str = proc(argstring);
		}
	    }
	    অন্যথা
	    {
		MYSTRPROCINT proc;

		proc = (MYSTRPROCINT)GetProcAddress(hinstLib, (LPCSTR)funcname);
		যদি (proc != রিক্ত)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_int = proc(argstring);
		}
	    }
	}
	অন্যথা
	{
	    // Call with number argument
	    যদি (string_result != রিক্ত)
	    {
		MYINTPROCSTR proc;

		proc = (MYINTPROCSTR)GetProcAddress(hinstLib, (LPCSTR)funcname);
		যদি (proc != রিক্ত)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_str = proc(argint);
		}
	    }
	    অন্যথা
	    {
		MYINTPROCINT proc;

		proc = (MYINTPROCINT)GetProcAddress(hinstLib, (LPCSTR)funcname);
		যদি (proc != রিক্ত)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_int = proc(argint);
		}
	    }
	}

	// Save the string before we free the library.
	// Assume that a "1" result is an illegal pointer.
	যদি (string_result == রিক্ত)
	    *সংখ্যা_result = retval_int;
	অন্যথা যদি (retval_str != রিক্ত
		&& (len = check_str_len(retval_str)) > 0)
	{
	    *string_result = alloc(len);
	    যদি (*string_result != রিক্ত)
		mch_memmove(*string_result, retval_str, len);
	}

# ifdef HAVE_TRY_EXCEPT
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	    যদি (GetExceptionCode() == EXCEPTION_স্তূপ_OVERFLOW)
		_resetstkoflw();
	    fRunTimeLinkSuccess = FALSE;
	}
# endif

	// Free the DLL module.
	(শূন্য)FreeLibrary(hinstLib);
    }

    যদি (!fRunTimeLinkSuccess)
    {
	semsg(_(e_library_call_failed_for_str), funcname);
	ঘূৰাই FAIL;
    }

    ঘূৰাই OK;
}
#endif

/*
 * Debugging helper: expose the MCH_WRITE_DUMP stuff to other modules
 */
    শূন্য
DumpPutS(ধ্রুৱ ৱর্ণ *psz UNUSED)
{
#ifdef MCH_WRITE_DUMP
    যদি (fdDump)
    {
	fputs(psz, fdDump);
	যদি (psz[strlen(psz) - 1] != '\n')
	    fputc('\n', fdDump);
	fflush(fdDump);
    }
#endif
}

#ifdef _DEBUG

শূন্য __cdecl
Trace(
    ৱর্ণ *pszFormat,
    ...)
{
    CHAR szBuff[2048];
    va_list args;

    va_start(args, pszFormat);
    vsprintf(szBuff, pszFormat, args);
    va_end(args);

    OutputDebugString(szBuff);
}

#endif //_DEBUG

#যদি !defined(FEAT_GUI) || defined(VIMDLL)
বাহ্য HWND g_hWnd;	// This is in os_win32.c.

/*
 * Showing the printer dialog is tricky since we have no GUI
 * window to parent it. The following routines are needed to
 * get the window parenting and Z-order to work properly.
 */
    স্থির শূন্য
GetConsoleHwnd(শূন্য)
{
    // Skip if it's already set.
    যদি (s_hwnd != 0)
	ঘূৰাই;

    // Window handle may have been found by init code (Windows NT only)
    যদি (g_hWnd != 0)
    {
	s_hwnd = g_hWnd;
	ঘূৰাই;
    }

    s_hwnd = GetConsoleWindow();
}

/*
 * Console implementation of ":winpos".
 */
    পূর্ণাঙ্ক
mch_get_winpos(পূর্ণাঙ্ক *x, পূর্ণাঙ্ক *y)
{
    RECT  rect;

    GetConsoleHwnd();
    GetWindowRect(s_hwnd, &rect);
    *x = rect.বাম;
    *y = rect.শীৰ্ষ;
    ঘূৰাই OK;
}

/*
 * Console implementation of ":winpos x y".
 */
    শূন্য
mch_set_winpos(পূর্ণাঙ্ক x, পূর্ণাঙ্ক y)
{
    GetConsoleHwnd();
    SetWindowPos(s_hwnd, রিক্ত, x, y, 0, 0,
		 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}
#endif

#যদি defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT)

//=================================================================
// Win32 printer stuff

স্থির HFONT		prt_font_handles[2][2][2];
স্থির PRINTDLGW	prt_dlg;
স্থির ধ্রুৱ পূর্ণাঙ্ক	boldface[2] = {FW_REGULAR, FW_BOLD};
স্থির TEXTMETRIC	prt_tm;
স্থির পূর্ণাঙ্ক		prt_সাৰি_height;
স্থির পূর্ণাঙ্ক		prt_সংখ্যা_width;
স্থির পূর্ণাঙ্ক		prt_বাম_margin;
স্থির পূর্ণাঙ্ক		prt_ডান_margin;
স্থির পূর্ণাঙ্ক		prt_শীৰ্ষ_margin;
স্থির char_u		szAppName[] = TEXT("VIM");
স্থির HWND		hDlgPrint;
স্থির পূর্ণাঙ্ক		*bUserAbort = রিক্ত;
স্থির char_u		*prt_name = রিক্ত;

// Defines which are also in vim.rc.
# define IDC_BOX1		400
# define IDC_PRINTTEXT1		401
# define IDC_PRINTTEXT2		402
# define IDC_PROGRESS		403

    স্থির BOOL
vimSetDlgItemText(HWND hDlg, পূর্ণাঙ্ক nIDDlgItem, char_u *s)
{
    WCHAR   *wp;
    BOOL    ret;

    wp = enc_to_utf16(s, রিক্ত);
    যদি (wp == রিক্ত)
	ঘূৰাই FALSE;

    ret = SetDlgItemTextW(hDlg, nIDDlgItem, wp);
    vim_free(wp);
    ঘূৰাই ret;
}

/*
 * Convert BGR to RGB for Windows GDI calls
 */
    স্থির COLORREF
swap_me(COLORREF colorref)
{
    পূর্ণাঙ্ক  temp;
    ৱর্ণ *ptr = (ৱর্ণ *)&colorref;

    temp = *(ptr);
    *(ptr ) = *(ptr + 2);
    *(ptr + 2) = temp;
    ঘূৰাই colorref;
}

    স্থির INT_PTR CALLBACK
PrintDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam UNUSED,
	LPARAM lParam UNUSED)
{
# ifdef FEAT_GETTEXT
    NONCLIENTMETRICS nm;
    স্থির HFONT hfont;
# endif

    ৱিকল্প (message)
    {
	পক্ষ WM_INITDIALOG:
# ifdef FEAT_GETTEXT
	    nm.cbSize = পরিমাণ(NONCLIENTMETRICS);
	    যদি (SystemParametersInfo(
			SPI_GETNONCLIENTMETRICS,
			পরিমাণ(NONCLIENTMETRICS),
			&nm,
			0))
	    {
		ৱর্ণ buff[MAX_PATH];
		পূর্ণাঙ্ক i;

		// Translate the dialog texts
		hfont = CreateFontIndirect(&nm.lfMessageFont);
		বাবে (i = IDC_PRINTTEXT1; i <= IDC_PROGRESS; i++)
		{
		    SendDlgItemMessage(hDlg, i, WM_SETFONT, (WPARAM)hfont, 1);
		    যদি (GetDlgItemText(hDlg,i, buff, পরিমাণ(buff)))
			vimSetDlgItemText(hDlg,i, (char_u *)_(buff));
		}
		SendDlgItemMessage(hDlg, IDCANCEL,
						WM_SETFONT, (WPARAM)hfont, 1);
		যদি (GetDlgItemText(hDlg,IDCANCEL, buff, পরিমাণ(buff)))
		    vimSetDlgItemText(hDlg,IDCANCEL, (char_u *)_(buff));
	    }
# endif
	    SetWindowText(hDlg, (LPCSTR)szAppName);
	    যদি (prt_name != রিক্ত)
	    {
		vimSetDlgItemText(hDlg, IDC_PRINTTEXT2, (char_u *)prt_name);
		VIM_CLEAR(prt_name);
	    }
	    EnableMenuItem(GetSystemMenu(hDlg, FALSE), SC_CLOSE, MF_GRAYED);
# যদি !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	    যদি (!gui.in_use)
#  endif
		Bring_Window_To_শীৰ্ষ(s_hwnd);
# endif
	    ঘূৰাই TRUE;

	পক্ষ WM_COMMAND:
	    *bUserAbort = TRUE;
	    EnableWindow(GetParent(hDlg), TRUE);
	    DestroyWindow(hDlg);
	    hDlgPrint = রিক্ত;
# ifdef FEAT_GETTEXT
	    DeleteObject(hfont);
# endif
	    ঘূৰাই TRUE;
    }
    ঘূৰাই FALSE;
}

    স্থির BOOL CALLBACK
AbortProc(HDC hdcPrn UNUSED, পূর্ণাঙ্ক iCode UNUSED)
{
    বাৰ্তা_2 বাৰ্তা;

    যেতিয়ালৈ (!*bUserAbort && PeekMessageW(&বাৰ্তা, রিক্ত, 0, 0, PM_REMOVE))
    {
	যদি (!hDlgPrint || !IsDialogMessageW(hDlgPrint, &বাৰ্তা))
	{
	    TranslateMessage(&বাৰ্তা);
	    DispatchMessageW(&বাৰ্তা);
	}
    }
    ঘূৰাই !*bUserAbort;
}

    স্থির UINT_PTR CALLBACK
PrintHookProc(
	HWND hDlg,	// handle to dialog box
	UINT ui_বাৰ্তা,	// message identifier
	WPARAM wParam UNUSED,	// message parameter
	LPARAM lParam	// message parameter
	)
{
    HWND	hwndOwner;
    RECT	rc, rcDlg, rcOwner;
    PRINTDLGW	*pPD;

    যদি (ui_বাৰ্তা != WM_INITDIALOG)
	ঘূৰাই FALSE;

    // Get the owner window and dialog box rectangles.
    যদি ((hwndOwner = GetParent(hDlg)) == রিক্ত)
	hwndOwner = GetDesktopWindow();

    GetWindowRect(hwndOwner, &rcOwner);
    GetWindowRect(hDlg, &rcDlg);
    CopyRect(&rc, &rcOwner);

    // Offset the owner and dialog box rectangles so that
    // right and bottom values represent the width and
    // height, and then offset the owner again to discard
    // space taken up by the dialog box.

    OffsetRect(&rcDlg, -rcDlg.বাম, -rcDlg.শীৰ্ষ);
    OffsetRect(&rc, -rc.বাম, -rc.শীৰ্ষ);
    OffsetRect(&rc, -rcDlg.ডান, -rcDlg.bottom);

    // The new position is the sum of half the remaining
    // space and the owner's original position.

    SetWindowPos(hDlg,
	    HWND_শীৰ্ষ,
	    rcOwner.বাম + (rc.ডান / 2),
	    rcOwner.শীৰ্ষ + (rc.bottom / 2),
	    0, 0,		// ignores size arguments
	    SWP_NOSIZE);

    //  tackle the printdlg copiesctrl problem
    pPD = (PRINTDLGW *)lParam;
    pPD->nCopies = (WORD)pPD->lCustData;
    SetDlgItemInt( hDlg, edt3, pPD->nCopies, FALSE );
    //  Bring the window to top
    Bring_Window_To_শীৰ্ষ(GetParent(hDlg));
    SetForegroundWindow(hDlg);

    ঘূৰাই FALSE;
}

    শূন্য
mch_print_cleanup(শূন্য)
{
    পূর্ণাঙ্ক pifItalic;
    পূর্ণাঙ্ক pifBold;
    পূর্ণাঙ্ক pifUnderline;

    বাবে (pifBold = 0; pifBold <= 1; pifBold++)
	বাবে (pifItalic = 0; pifItalic <= 1; pifItalic++)
	    বাবে (pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
		DeleteObject(prt_font_handles[pifBold][pifItalic][pifUnderline]);

    যদি (prt_dlg.hDC != রিক্ত)
	DeleteDC(prt_dlg.hDC);
    যদি (!*bUserAbort && hDlgPrint != রিক্ত)
	SendMessage(hDlgPrint, WM_COMMAND, 0, 0);
}

    স্থির পূর্ণাঙ্ক
to_device_units(পূর্ণাঙ্ক idx, পূর্ণাঙ্ক dpi, পূর্ণাঙ্ক physsize, পূর্ণাঙ্ক offset, পূর্ণাঙ্ক def_সংখ্যা)
{
    পূর্ণাঙ্ক		ret = 0;
    পূর্ণাঙ্ক		u;
    পূর্ণাঙ্ক		nr;

    u = prt_get_unit(idx);
    যদি (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_সংখ্যা;
    }
    অন্যথা
	nr = printer_opts[idx].সংখ্যা;

    ৱিকল্প (u)
    {
	পক্ষ PRT_UNIT_PERC:
	    ret = (physsize * nr) / 100;
	    ভঙ্গ;
	পক্ষ PRT_UNIT_INCH:
	    ret = (nr * dpi);
	    ভঙ্গ;
	পক্ষ PRT_UNIT_MM:
	    ret = (nr * 10 * dpi) / 254;
	    ভঙ্গ;
	পক্ষ PRT_UNIT_POINT:
	    ret = (nr * 10 * dpi) / 720;
	    ভঙ্গ;
    }

    যদি (ret < offset)
	ঘূৰাই 0;
    অন্যথা
	ঘূৰাই ret - offset;
}

    স্থির পূর্ণাঙ্ক
prt_get_cpl(শূন্য)
{
    পূর্ণাঙ্ক		hr;
    পূর্ণাঙ্ক		phyw;
    পূর্ণাঙ্ক		dvoff;
    পূর্ণাঙ্ক		rev_offset;
    পূর্ণাঙ্ক		dpi;

    GetTextMetrics(prt_dlg.hDC, &prt_tm);
    prt_সাৰি_height = prt_tm.tmHeight + prt_tm.tmExternalLeading;

    hr	    = GetDeviceCaps(prt_dlg.hDC, HORZRES);
    phyw    = GetDeviceCaps(prt_dlg.hDC, PHYSICALWIDTH);
    dvoff   = GetDeviceCaps(prt_dlg.hDC, PHYSICALOFFSETX);
    dpi	    = GetDeviceCaps(prt_dlg.hDC, LOGPIXELSX);

    rev_offset = phyw - (dvoff + hr);

    prt_বাম_margin = to_device_units(OPT_PRINT_বাম, dpi, phyw, dvoff, 10);
    যদি (prt_use_সংখ্যা())
    {
	prt_সংখ্যা_width = PRINT_সংখ্যা_WIDTH * prt_tm.tmAveCharWidth;
	prt_বাম_margin += prt_সংখ্যা_width;
    }
    অন্যথা
	prt_সংখ্যা_width = 0;

    prt_ডান_margin = hr - to_device_units(OPT_PRINT_ডান, dpi, phyw,
							       rev_offset, 5);

    ঘূৰাই (prt_ডান_margin - prt_বাম_margin) / prt_tm.tmAveCharWidth;
}

    স্থির পূর্ণাঙ্ক
prt_get_lpp(শূন্য)
{
    পূর্ণাঙ্ক vr;
    পূর্ণাঙ্ক phyw;
    পূর্ণাঙ্ক dvoff;
    পূর্ণাঙ্ক rev_offset;
    পূর্ণাঙ্ক	bottom_margin;
    পূর্ণাঙ্ক	dpi;

    vr	    = GetDeviceCaps(prt_dlg.hDC, VERTRES);
    phyw    = GetDeviceCaps(prt_dlg.hDC, PHYSICALHEIGHT);
    dvoff   = GetDeviceCaps(prt_dlg.hDC, PHYSICALOFFSETY);
    dpi	    = GetDeviceCaps(prt_dlg.hDC, LOGPIXELSY);

    rev_offset = phyw - (dvoff + vr);

    prt_শীৰ্ষ_margin = to_device_units(OPT_PRINT_শীৰ্ষ, dpi, phyw, dvoff, 5);

    // adjust top margin if there is a header
    prt_শীৰ্ষ_margin += prt_সাৰি_height * prt_header_height();

    bottom_margin = vr - to_device_units(OPT_PRINT_BOT, dpi, phyw,
							       rev_offset, 5);

    ঘূৰাই (bottom_margin - prt_শীৰ্ষ_margin) / prt_সাৰি_height;
}

    পূর্ণাঙ্ক
mch_print_init(prt_settings_T *psettings, char_u *jobname, পূর্ণাঙ্ক forceit)
{
    স্থির HGLOBAL	stored_dm = রিক্ত;
    স্থির HGLOBAL	stored_devn = রিক্ত;
    স্থির পূর্ণাঙ্ক		stored_nCopies = 1;
    স্থির পূর্ণাঙ্ক		stored_nFlags = 0;

    LOGFONTW		fLogFont;
    পূর্ণাঙ্ক			pifItalic;
    পূর্ণাঙ্ক			pifBold;
    পূর্ণাঙ্ক			pifUnderline;

    DEVMODEW		*mem;
    DEVNAMES		*devname;
    পূর্ণাঙ্ক			i;
    DWORD		err;

    bUserAbort = &(psettings->user_abort);
    CLEAR_FIELD(prt_dlg);
    prt_dlg.lStructSize = পরিমাণ(PRINTDLGW);
# যদি !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
    যদি (!gui.in_use)
#  endif
	GetConsoleHwnd();	    // get value of s_hwnd
# endif
    prt_dlg.hwndOwner = s_hwnd;
    prt_dlg.Flags = PD_NOPAGENUMS | PD_NOSELECTION | PD_RETURNDC;
    যদি (!forceit)
    {
	prt_dlg.hDevMode = stored_dm;
	prt_dlg.hDevNames = stored_devn;
	prt_dlg.lCustData = stored_nCopies; // work around bug in print dialog
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
    যদি (forceit
# যদি !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	    || (!gui.in_use && !term_console)
#  অন্যথা
	    || !term_console
#  endif
# endif
	    )
    {
	prt_dlg.Flags |= PD_RETURNDEFAULT;
	/*
	 * MSDN suggests setting the first parameter to WINSPOOL for
	 * NT, but NULL appears to work just as well.
	 */
	যদি (*p_pdev != NUL)
	    prt_dlg.hDC = CreateDC(রিক্ত, (LPCSTR)p_pdev, রিক্ত, রিক্ত);
	অন্যথা
	{
	    prt_dlg.Flags |= PD_RETURNDEFAULT;
	    যদি (PrintDlgW(&prt_dlg) == 0)
		গচ্ছ init_fail_dlg;
	}
    }
    অন্যথা যদি (PrintDlgW(&prt_dlg) == 0)
	গচ্ছ init_fail_dlg;
    অন্যথা
    {
	/*
	 * keep the previous driver context
	 */
	stored_dm = prt_dlg.hDevMode;
	stored_devn = prt_dlg.hDevNames;
	stored_nFlags = prt_dlg.Flags;
	stored_nCopies = prt_dlg.nCopies;
    }

    যদি (prt_dlg.hDC == রিক্ত)
    {
	emsg(_(e_printer_selection_failed));
	mch_print_cleanup();
	ঘূৰাই FALSE;
    }

    // Not all printer drivers report the support of color (or grey) in the
    // same way.  Let's set has_color if there appears to be some way to print
    // more than B&W.
    i = GetDeviceCaps(prt_dlg.hDC, NUMCOLORS);
    psettings->has_color = (GetDeviceCaps(prt_dlg.hDC, BITSPIXEL) > 1
				   || GetDeviceCaps(prt_dlg.hDC, PLANES) > 1
				   || i > 2 || i == -1);

    // Ensure all font styles are baseline aligned
    SetTextAlign(prt_dlg.hDC, TA_BASELINE|TA_বাম);

    /*
     * On some windows systems the nCopies parameter is not
     * passed back correctly. It must be retrieved from the
     * hDevMode struct.
     */
    mem = (DEVMODEW *)GlobalLock(prt_dlg.hDevMode);
    যদি (mem != রিক্ত)
    {
	যদি (mem->dmCopies != 1)
	    stored_nCopies = mem->dmCopies;
	যদি ((mem->dmFields & DM_DUPLEX) && (mem->dmDuplex & ~DMDUP_SIMPLEX))
	    psettings->duplex = TRUE;
	যদি ((mem->dmFields & DM_COLOR) && (mem->dmColor & DMCOLOR_COLOR))
	    psettings->has_color = TRUE;
    }
    GlobalUnlock(prt_dlg.hDevMode);

    devname = (DEVNAMES *)GlobalLock(prt_dlg.hDevNames);
    যদি (devname != 0)
    {
	WCHAR	*wprinter_name = (WCHAR *)devname + devname->wDeviceOffset;
	WCHAR	*wport_name = (WCHAR *)devname + devname->wOutputOffset;
	char_u	*text = (char_u *)_("to %s on %s");
	char_u  *printer_name = utf16_to_enc(wprinter_name, রিক্ত);
	char_u	*port_name = utf16_to_enc(wport_name, রিক্ত);

	যদি (printer_name != রিক্ত && port_name != রিক্ত)
	    prt_name = alloc(STRLEN(printer_name)
					   + STRLEN(port_name) + STRLEN(text));
	যদি (prt_name != রিক্ত)
	    wsprintf((ৱর্ণ *)prt_name, (ধ্রুৱ ৱর্ণ *)text,
		    printer_name, port_name);
	vim_free(printer_name);
	vim_free(port_name);
    }
    GlobalUnlock(prt_dlg.hDevNames);

    /*
     * Initialise the font according to 'printfont'
     */
    CLEAR_FIELD(fLogFont);
    যদি (get_logfont(&fLogFont, p_pfn, prt_dlg.hDC, TRUE) == FAIL)
    {
	semsg(_(e_unknown_printer_font_str), p_pfn);
	mch_print_cleanup();
	ঘূৰাই FALSE;
    }

    বাবে (pifBold = 0; pifBold <= 1; pifBold++)
	বাবে (pifItalic = 0; pifItalic <= 1; pifItalic++)
	    বাবে (pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
	    {
		fLogFont.lfWeight =  boldface[pifBold];
		fLogFont.lfItalic = pifItalic;
		fLogFont.lfUnderline = pifUnderline;
		prt_font_handles[pifBold][pifItalic][pifUnderline]
					      = CreateFontIndirectW(&fLogFont);
	    }

    SetBkMode(prt_dlg.hDC, OPAQUE);
    SelectObject(prt_dlg.hDC, prt_font_handles[0][0][0]);

    /*
     * Fill in the settings struct
     */
    psettings->chars_per_সাৰি = prt_get_cpl();
    psettings->সাৰিগুলি_per_page = prt_get_lpp();
    যদি (prt_dlg.Flags & PD_USEDEVMODECOPIESANDCOLLATE)
    {
	psettings->n_collated_copies = (prt_dlg.Flags & PD_COLLATE)
						    ? prt_dlg.nCopies : 1;
	psettings->n_uncollated_copies = (prt_dlg.Flags & PD_COLLATE)
						    ? 1 : prt_dlg.nCopies;

	যদি (psettings->n_collated_copies == 0)
	    psettings->n_collated_copies = 1;

	যদি (psettings->n_uncollated_copies == 0)
	    psettings->n_uncollated_copies = 1;
    }
    অন্যথা
    {
	psettings->n_collated_copies = 1;
	psettings->n_uncollated_copies = 1;
    }

    psettings->jobname = jobname;

    ঘূৰাই TRUE;

init_fail_dlg:
    err = CommDlgExtendedError();
    যদি (err)
    {
	char_u *buf;

	// I suspect FormatMessage() doesn't work for values returned by
	// CommDlgExtendedError().  What does?
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		রিক্ত, err, 0, (LPTSTR)(&buf), 0, রিক্ত);
	semsg(_(e_print_error_str),
		buf == রিক্ত ? (char_u *)_("Unknown") : buf);
	LocalFree((LPVOID)(buf));
    }
    অন্যথা
	বাৰ্তা_clr_eos(); // Maybe canceled

    mch_print_cleanup();
    ঘূৰাই FALSE;
}


    পূর্ণাঙ্ক
mch_print_begin(prt_settings_T *psettings)
{
    পূর্ণাঙ্ক			ret = 0;
    ৱর্ণ		szBuffer[300];
    WCHAR		*wp;

    hDlgPrint = CreateDialog(g_hinst, TEXT("PrintDlgBox"),
					     prt_dlg.hwndOwner, PrintDlgProc);
    SetAbortProc(prt_dlg.hDC, AbortProc);
    wsprintf(szBuffer, _("Printing '%s'"), gettail(psettings->jobname));
    vimSetDlgItemText(hDlgPrint, IDC_PRINTTEXT1, (char_u *)szBuffer);

    wp = enc_to_utf16(psettings->jobname, রিক্ত);
    যদি (wp != রিক্ত)
    {
	DOCINFOW	di;

	CLEAR_FIELD(di);
	di.cbSize = পরিমাণ(di);
	di.lpszDocName = wp;
	ret = StartDocW(prt_dlg.hDC, &di);
	vim_free(wp);
    }

# ifdef FEAT_GUI
    // Give focus back to main window (when using MDI).
#  ifdef VIMDLL
    যদি (gui.in_use)
#  endif
	SetFocus(s_hwnd);
# endif

    ঘূৰাই (ret > 0);
}

    শূন্য
mch_print_end(prt_settings_T *psettings UNUSED)
{
    EndDoc(prt_dlg.hDC);
    যদি (!*bUserAbort && hDlgPrint != রিক্ত)
	SendMessage(hDlgPrint, WM_COMMAND, 0, 0);
}

    পূর্ণাঙ্ক
mch_print_end_page(শূন্য)
{
    ঘূৰাই (EndPage(prt_dlg.hDC) > 0);
}

    পূর্ণাঙ্ক
mch_print_begin_page(char_u *বাৰ্তা)
{
    যদি (বাৰ্তা != রিক্ত)
	vimSetDlgItemText(hDlgPrint, IDC_PROGRESS, বাৰ্তা);
    ঘূৰাই (StartPage(prt_dlg.hDC) > 0);
}

    পূর্ণাঙ্ক
mch_print_blank_page(শূন্য)
{
    ঘূৰাই (mch_print_begin_page(রিক্ত) ? (mch_print_end_page()) : FALSE);
}

স্থির পূর্ণাঙ্ক prt_pos_x = 0;
স্থির পূর্ণাঙ্ক prt_pos_y = 0;

    শূন্য
mch_print_start_সাৰি(পূর্ণাঙ্ক margin, পূর্ণাঙ্ক page_সাৰি)
{
    যদি (margin)
	prt_pos_x = -prt_সংখ্যা_width;
    অন্যথা
	prt_pos_x = 0;
    prt_pos_y = page_সাৰি * prt_সাৰি_height
				 + prt_tm.tmAscent + prt_tm.tmExternalLeading;
}

    পূর্ণাঙ্ক
mch_print_text_out(char_u *p, পূর্ণাঙ্ক len)
{
    SIZE	sz;
    WCHAR	*wp;
    পূর্ণাঙ্ক		wlen = len;
    পূর্ণাঙ্ক		ret = FALSE;

    wp = enc_to_utf16(p, &wlen);
    যদি (wp == রিক্ত)
	ঘূৰাই FALSE;

    TextOutW(prt_dlg.hDC, prt_pos_x + prt_বাম_margin,
	    prt_pos_y + prt_শীৰ্ষ_margin, wp, wlen);
    GetTextExtentPoint32W(prt_dlg.hDC, wp, wlen, &sz);
    vim_free(wp);
    prt_pos_x += (sz.cx - prt_tm.tmOverhang);
    // This is wrong when printing spaces for a TAB.
    যদি (p[len] != NUL)
    {
	wlen = mb_ptr2len(p + len);
	wp = enc_to_utf16(p + len, &wlen);
	যদি (wp != রিক্ত)
	{
	    GetTextExtentPoint32W(prt_dlg.hDC, wp, 1, &sz);
	    ret = (prt_pos_x + prt_বাম_margin + sz.cx > prt_ডান_margin);
	    vim_free(wp);
	}
    }
    ঘূৰাই ret;
}

    শূন্য
mch_print_set_font(পূর্ণাঙ্ক iBold, পূর্ণাঙ্ক iItalic, পূর্ণাঙ্ক iUnderline)
{
    SelectObject(prt_dlg.hDC, prt_font_handles[iBold][iItalic][iUnderline]);
}

    শূন্য
mch_print_set_bg(long_u bgcol)
{
    SetBkColor(prt_dlg.hDC, GetNearestColor(prt_dlg.hDC,
						   swap_me((COLORREF)bgcol)));
    /*
     * With a white background we can draw characters transparent, which is
     * good for italic characters that overlap to the next char cell.
     */
    যদি (bgcol == 0xffffffUL)
	SetBkMode(prt_dlg.hDC, TRANSPARENT);
    অন্যথা
	SetBkMode(prt_dlg.hDC, OPAQUE);
}

    শূন্য
mch_print_set_fg(long_u fgcol)
{
    SetTextColor(prt_dlg.hDC, GetNearestColor(prt_dlg.hDC,
						   swap_me((COLORREF)fgcol)));
}

#endif // FEAT_PRINTER && !FEAT_POSTSCRIPT



#যদি defined(FEAT_SHORTCUT)
# include <shlobj.h>

# define is_path_sep(c)	    ((c) == L'\\' || (c) == L'/')

    স্থির পূর্ণাঙ্ক
is_reparse_point_included(LPCWSTR fname)
{
    LPCWSTR	p = fname, q;
    WCHAR	buf[MAX_PATH];
    DWORD	attr;

    যদি (SAFE_isalpha(p[0]) && p[1] == L':' && is_path_sep(p[2]))
	p += 3;
    অন্যথা যদি (is_path_sep(p[0]) && is_path_sep(p[1]))
	p += 2;

    যেতিয়ালৈ (*p != L'\0')
    {
	q = wcspbrk(p, L"\\/");
	যদি (q == রিক্ত)
	    p = q = fname + wcslen(fname);
	অন্যথা
	    p = q + 1;
	যদি (q - fname >= MAX_PATH)
	    ঘূৰাই FALSE;
	wcsncpy(buf, fname, q - fname);
	buf[q - fname] = L'\0';
	attr = GetFileAttributesW(buf);
	যদি (attr != INVALID_FILE_ATTRIBUTES
		&& (attr & FILE_ATTRIBUTE_REPARSE_POINT) != 0)
	    ঘূৰাই TRUE;
    }
    ঘূৰাই FALSE;
}

/*
 * Return the resolved file path, NULL if "fname" is an AppExecLink reparse
 * point, already fully resolved, or it doesn't exists.
 */
    char_u *
resolve_reparse_point(char_u *fname)
{
    HANDLE	    h = INVALID_HANDLE_মান;
    DWORD	    size;
    WCHAR	    *p, *wp;
    char_u	    *rfname = রিক্ত;
    WCHAR	    *buff = রিক্ত;

    p = enc_to_utf16(fname, রিক্ত);
    যদি (p == রিক্ত)
	গচ্ছ fail;

    যদি (!is_reparse_point_included(p))
    {
	vim_free(p);
	গচ্ছ fail;
    }

    h = CreateFileW(p, 0, 0, রিক্ত, OPEN_EXISTING,
	    FILE_FLAG_BACKUP_SEMANTICS, রিক্ত);
    vim_free(p);

    যদি (h == INVALID_HANDLE_মান)
	গচ্ছ fail;

    size = GetFinalPathNameByHandleW(h, রিক্ত, 0, 0);
    যদি (size == 0)
	গচ্ছ fail;
    buff = ALLOC_MULT(WCHAR, size);
    যদি (buff == রিক্ত)
	গচ্ছ fail;
    যদি (GetFinalPathNameByHandleW(h, buff, size, 0) == 0)
	গচ্ছ fail;

    যদি (wcsncmp(buff, L"\\\\?\\UNC\\", 8) == 0)
    {
	buff[6] = L'\\';
	wp = buff + 6;
    }
    অন্যথা যদি (wcsncmp(buff, L"\\\\?\\", 4) == 0)
	wp = buff + 4;
    অন্যথা
	wp = buff;

    rfname = utf16_to_enc(wp, রিক্ত);

fail:
    যদি (h != INVALID_HANDLE_মান)
	CloseHandle(h);
    যদি (buff != রিক্ত)
	vim_free(buff);

    ঘূৰাই rfname;
}

/*
 * When "fname" is the name of a shortcut (*.lnk) resolve the file it points
 * to and return that name in allocated memory.
 * Otherwise NULL is returned.
 */
    স্থির char_u *
resolve_shortcut(char_u *fname)
{
    HRESULT		hr;
    IShellLink		*psl = রিক্ত;
    IPersistFile	*ppf = রিক্ত;
    OLECHAR		wsz[MAX_PATH];
    char_u		*rfname = রিক্ত;
    পূর্ণাঙ্ক			len;
    IShellLinkW		*pslw = রিক্ত;
    WIN32_FIND_DATAW	ffdw; // we get those free of charge

    // Check if the file name ends in ".lnk". Avoid calling
    // CoCreateInstance(), it's quite slow.
    যদি (fname == রিক্ত)
	ঘূৰাই rfname;
    len = (পূর্ণাঙ্ক)STRLEN(fname);
    যদি (len <= 4 || STRNICMP(fname + len - 4, ".lnk", 4) != 0)
	ঘূৰাই rfname;

    CoInitialize(রিক্ত);

    // create a link manager object and request its interface
    hr = CoCreateInstance(
	    &CLSID_ShellLink, রিক্ত, CLSCTX_INPROC_SERVER,
	    &IID_IShellLinkW, (শূন্য**)&pslw);
    যদি (hr == S_OK)
    {
	WCHAR	*p = enc_to_utf16(fname, রিক্ত);

	যদি (p != রিক্ত)
	{
	    // Get a pointer to the IPersistFile interface.
	    hr = pslw->lpVtbl->QueryInterface(
		    pslw, &IID_IPersistFile, (শূন্য**)&ppf);
	    যদি (hr != S_OK)
		গচ্ছ shortcut_errorw;

	    // "load" the name and resolve the link
	    hr = ppf->lpVtbl->Load(ppf, p, STGM_READ);
	    যদি (hr != S_OK)
		গচ্ছ shortcut_errorw;
# যদি 0  // This makes Vim wait a long time if the target does not exist.
	    hr = pslw->lpVtbl->Resolve(pslw, রিক্ত, SLR_NO_UI);
	    যদি (hr != S_OK)
		গচ্ছ shortcut_errorw;
# endif

	    // Get the path to the link target.
	    ZeroMemory(wsz, MAX_PATH * পরিমাণ(WCHAR));
	    hr = pslw->lpVtbl->GetPath(pslw, wsz, MAX_PATH, &ffdw, 0);
	    যদি (hr == S_OK && wsz[0] != NUL)
		rfname = utf16_to_enc(wsz, রিক্ত);

shortcut_errorw:
	    vim_free(p);
	}
    }

    // Release all interface pointers (both belong to the same object)
    যদি (ppf != রিক্ত)
	ppf->lpVtbl->Release(ppf);
    যদি (psl != রিক্ত)
	psl->lpVtbl->Release(psl);
    যদি (pslw != রিক্ত)
	pslw->lpVtbl->Release(pslw);

    CoUninitialize();
    ঘূৰাই rfname;
}

    char_u *
mch_resolve_path(char_u *fname, পূর্ণাঙ্ক reparse_point)
{
    char_u  *path = resolve_shortcut(fname);

    যদি (path == রিক্ত && reparse_point)
	path = resolve_reparse_point(fname);
    ঘূৰাই path;
}
#endif

#যদি defined(FEAT_EVAL) && (!defined(FEAT_GUI) || defined(VIMDLL))
/*
 * Bring ourselves to the foreground.  Does work if the OS doesn't allow it.
 */
    শূন্য
win32_set_foreground(শূন্য)
{
    GetConsoleHwnd();	    // get value of s_hwnd
    যদি (s_hwnd != 0)
	SetForegroundWindow(s_hwnd);
}
#endif

#যদি defined(FEAT_CLIENTSERVER)
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
HWND message_window = 0;	    // window that's handling messages

# define VIM_CLASSNAME      "VIM_MESSAGES"
# define VIM_CLASSNAME_LEN  (পরিমাণ(VIM_CLASSNAME) - 1)

// Timeout for sending a message to another Vim instance.  Normally this works
// instantly, but it may hang when the other Vim instance is halted.
# define SENDMESSAGE_TIMEOUT	(5 * 1000)

// Communication is via WM_COPYDATA messages. The message type is sent in
// the dwData parameter. Types are defined here.
# define COPYDATA_চাবিগুলি		0
# define COPYDATA_REPLY		1
# define COPYDATA_ৰাশিমালা		10
# define COPYDATA_RESULT	11
# define COPYDATA_ERROR_RESULT	12
# define COPYDATA_ENCODING	20

// This is a structure containing a server HWND and its name.
সংরচনা server_id
{
    HWND hwnd;
    char_u *name;
};

// Last received 'encoding' that the client uses.
স্থির char_u	*client_enc = রিক্ত;

/*
 * Tell the other side what encoding we are using.
 * Return -1 if timeout happens.  Other errors are ignored.
 */
    স্থির পূর্ণাঙ্ক
serverSendEnc(HWND target)
{
    COPYDATASTRUCT data;

    data.dwData = COPYDATA_ENCODING;
    data.cbData = (DWORD)STRLEN(p_enc) + 1;
    data.lpData = p_enc;
    যদি (SendMessageTimeout(target, WM_COPYDATA,
	    (WPARAM)message_window, (LPARAM)&data,
	    SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, রিক্ত) == 0)
	ঘূৰাই -1;
    ঘূৰাই 0;
}

/*
 * Clean up on exit. This destroys the hidden message window.
 */
    স্থির শূন্য
CleanUpMessaging(শূন্য)
{
    যদি (message_window == 0)
	ঘূৰাই;

    DestroyWindow(message_window);
    message_window = 0;
}

স্থির পূর্ণাঙ্ক save_reply(HWND server, char_u *reply, পূর্ণাঙ্ক ৰাশিমালা);

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
    স্থির LRESULT CALLBACK
Messaging_WndProc(HWND hwnd, UINT বাৰ্তা, WPARAM wParam, LPARAM lParam)
{
    যদি (বাৰ্তা == WM_COPYDATA)
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
	COPYDATASTRUCT	*data = (COPYDATASTRUCT*)lParam;
	HWND		sender = (HWND)wParam;
	COPYDATASTRUCT	reply;
	char_u		*res;
	পূর্ণাঙ্ক		retval;
	DWORD_PTR	dwret = 0;
	char_u		*str;
	char_u		*tofree;

	ৱিকল্প (data->dwData)
	{
	পক্ষ COPYDATA_ENCODING:
	    // Remember the encoding that the client uses.
	    vim_free(client_enc);
	    client_enc = enc_canonize((char_u *)data->lpData);
	    ঘূৰাই 1;

	পক্ষ COPYDATA_চাবিগুলি:
	    // Remember who sent this, for <client>
	    clientWindow = sender;

	    // Add the received keys to the input buffer.
	    str = serverConvert(client_enc, (char_u *)data->lpData, &tofree);
# ifdef FEAT_GUI
	    যদি (gui.in_use)
		// The GUI may get here re-entrantly, so insert the keys later.
		server_add_প্ৰবেশ(str);
	    অন্যথা
# endif
		server_to_প্ৰবেশ_buf(str);
	    vim_free(tofree);

# ifdef FEAT_GUI
	    // Wake up the main GUI loop.
#  ifdef VIMDLL
	    যদি (gui.in_use)
#  endif
		যদি (s_hwnd != 0)
		    PostMessage(s_hwnd, WM_NULL, 0, 0);
# endif
	    ঘূৰাই 1;

	পক্ষ COPYDATA_ৰাশিমালা:
	    // Remember who sent this, for <client>
	    clientWindow = sender;

	    str = serverConvert(client_enc, (char_u *)data->lpData, &tofree);
	    res = eval_client_ৰাশিমালা_to_string(str);

	    যদি (res == রিক্ত)
	    {
		ৱর্ণ	*err = _(e_invalid_expression_received);
		size_t	len = STRLEN(str) + STRLEN(err) + 5;

		res = alloc(len);
		যদি (res != রিক্ত)
		    reply.cbData = (DWORD)vim_snprintf_safelen(
			(ৱর্ণ *)res, len, "%s: \"%s\"", err, str);
		অন্যথা
		    reply.cbData = 0;
		reply.dwData = COPYDATA_ERROR_RESULT;
	    }
	    অন্যথা
	    {
		reply.cbData = (DWORD)STRLEN(res) + 1;
		reply.dwData = COPYDATA_RESULT;
	    }
	    reply.lpData = res;

	    যদি (serverSendEnc(sender) < 0)
		retval = -1;
	    অন্যথা
	    {
		যদি (SendMessageTimeout(sender, WM_COPYDATA,
			(WPARAM)message_window, (LPARAM)&reply,
			SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
		    retval = -1;
		অন্যথা
		    retval = (পূর্ণাঙ্ক)dwret;
	    }
	    vim_free(tofree);
	    vim_free(res);
	    ঘূৰাই retval;

	পক্ষ COPYDATA_REPLY:
	পক্ষ COPYDATA_RESULT:
	পক্ষ COPYDATA_ERROR_RESULT:
	    যদি (data->lpData != রিক্ত)
	    {
		str = serverConvert(client_enc, (char_u *)data->lpData,
								     &tofree);
		যদি (tofree == রিক্ত)
		    str = vim_strsave(str);
		যদি (save_reply(sender, str,
			   (data->dwData == COPYDATA_REPLY ?  0 :
			   (data->dwData == COPYDATA_RESULT ? 1 :
							      2))) == FAIL)
		    vim_free(str);
		অন্যথা যদি (data->dwData == COPYDATA_REPLY)
		{
		    char_u	winstr[30];

		    sprintf((ৱর্ণ *)winstr, PRINTF_HEX_LONG_U, (long_u)sender);
		    apply_autocmds(EVENT_REMOTEREPLY, winstr, str,
								TRUE, curbuf);
		}
	    }
	    ঘূৰাই 1;
	}

	ঘূৰাই 0;
    }

    অন্যথা যদি (বাৰ্তা == WM_ACTIVATE && wParam == WA_ACTIVE)
    {
	// When the message window is activated (brought to the foreground),
	// this actually applies to the text window.
# যদি !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	যদি (!gui.in_use)
#  endif
	    GetConsoleHwnd();	    // get value of s_hwnd
# endif
	যদি (s_hwnd != 0)
	{
	    SetForegroundWindow(s_hwnd);
	    ঘূৰাই 0;
	}
    }

    ঘূৰাই DefWindowProc(hwnd, বাৰ্তা, wParam, lParam);
}

/*
 * Initialise the message handling process.  This involves creating a window
 * to handle messages - the window will not be visible.
 */
    শূন্য
serverInitMessaging(শূন্য)
{
    WNDCLASS wndclass;

    // Clean up on exit
    atexit(CleanUpMessaging);

    // Register a window class - we only really care
    // about the window procedure
    wndclass.style = 0;
    wndclass.lpfnWndProc = Messaging_WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = g_hinst;
    wndclass.hIcon = রিক্ত;
    wndclass.hCursor = রিক্ত;
    wndclass.hbrBackground = রিক্ত;
    wndclass.lpszMenuName = রিক্ত;
    wndclass.lpszClassName = VIM_CLASSNAME;
    RegisterClass(&wndclass);

    // Create the message window. It will be hidden, so the details don't
    // matter.  Don't use WS_OVERLAPPEDWINDOW, it will make a shortcut remove
    // focus from gvim.
    message_window = CreateWindow(VIM_CLASSNAME, "",
			 WS_POPUPWINDOW | WS_CAPTION,
			 CW_USEDEFAULT, CW_USEDEFAULT,
			 100, 100, রিক্ত, রিক্ত,
			 g_hinst, রিক্ত);
}

// Used by serverSendToVim() to find an alternate server name.
স্থির char_u *altname_buf_ptr = রিক্ত;

/*
 * Get the title of the window "hwnd", which is the Vim server name, in
 * "name[namelen]" and return the length.
 * Returns zero if window "hwnd" is not a Vim server.
 */
    স্থির পূর্ণাঙ্ক
getVimServerName(HWND hwnd, ৱর্ণ *name, পূর্ণাঙ্ক namelen)
{
    পূর্ণাঙ্ক		len;
    ৱর্ণ	buffer[VIM_CLASSNAME_LEN + 1];

    // Ignore windows which aren't Vim message windows
    len = GetClassName(hwnd, buffer, পরিমাণ(buffer));
    যদি (len != VIM_CLASSNAME_LEN || STRCMP(buffer, VIM_CLASSNAME) != 0)
	ঘূৰাই 0;

    // Get the title of the window
    ঘূৰাই GetWindowText(hwnd, name, namelen);
}

    স্থির BOOL CALLBACK
enumWindowsGetServer(HWND hwnd, LPARAM lparam)
{
    সংরচনা	server_id *id = (সংরচনা server_id *)lparam;
    ৱর্ণ	server[MAX_PATH];

    // Get the title of the window
    যদি (getVimServerName(hwnd, server, পরিমাণ(server)) == 0)
	ঘূৰাই TRUE;

    // If this is the server we're looking for, return its HWND
    যদি (STRICMP(server, id->name) == 0)
    {
	id->hwnd = hwnd;
	ঘূৰাই FALSE;
    }

    // If we are looking for an alternate server, remember this name.
    যদি (altname_buf_ptr != রিক্ত)
    {
	size_t	namelen = STRLEN(id->name);

	যদি (STRNICMP(server, id->name, namelen) == 0
	    && vim_isdigit(server[namelen]))
	{
	    STRCPY(altname_buf_ptr, server);
	    altname_buf_ptr = রিক্ত;	    // don't use another name
	}
    }

    // Otherwise, keep looking
    ঘূৰাই TRUE;
}

    স্থির BOOL CALLBACK
enumWindowsGetNames(HWND hwnd, LPARAM lparam)
{
    list_T	*list = (list_T *)lparam;
    ৱর্ণ	server[MAX_PATH];

    // Get the title of the window
    যদি (getVimServerName(hwnd, server, পরিমাণ(server)) == 0)
	ঘূৰাই TRUE;

    list_append_string(list, (char_u *)server, -1);
    ঘূৰাই TRUE;
}

সংরচনা enum_windows_s
{
    WNDENUMPROC lpEnumFunc;
    LPARAM      lParam;
};

    স্থির BOOL CALLBACK
enum_windows_child(HWND hwnd, LPARAM lParam)
{
    সংরচনা enum_windows_s *ew = (সংরচনা enum_windows_s *)lParam;

    ঘূৰাই (ew->lpEnumFunc)(hwnd, ew->lParam);
}

    স্থির BOOL CALLBACK
enum_windows_toplevel(HWND hwnd, LPARAM lParam)
{
    সংরচনা enum_windows_s *ew = (সংরচনা enum_windows_s *)lParam;

    যদি ((ew->lpEnumFunc)(hwnd, ew->lParam))
	ঘূৰাই TRUE;
    ঘূৰাই EnumChildWindows(hwnd, enum_windows_child, lParam);
}

/*
 * Enumerate all windows including children.
 */
    স্থির BOOL
enum_windows(WNDENUMPROC lpEnumFunc, LPARAM lParam)
{
    সংরচনা enum_windows_s ew;

    ew.lpEnumFunc = lpEnumFunc;
    ew.lParam = lParam;
    ঘূৰাই EnumWindows(enum_windows_toplevel, (LPARAM)&ew);
}

    স্থির HWND
findServer(char_u *name)
{
    সংরচনা server_id id;

    id.name = name;
    id.hwnd = 0;

    enum_windows(enumWindowsGetServer, (LPARAM)(&id));

    ঘূৰাই id.hwnd;
}

    শূন্য
serverSetName(char_u *name)
{
    size_t	namelen;
    char_u	*ok_name;
    HWND	hwnd = 0;
    পূর্ণাঙ্ক		i = 0;
    char_u	*p;

    // Leave enough space for a 9-digit suffix to ensure uniqueness!
    namelen = STRLEN(name);
    ok_name = alloc(namelen + 10);
    যদি (ok_name == রিক্ত)
	ঘূৰাই;

    STRCPY(ok_name, name);
    p = ok_name + namelen;

    বাবে (;;)
    {
	// This is inefficient - we're doing an EnumWindows loop for each
	// possible name. It would be better to grab all names in one go,
	// and scan the list each time...
	hwnd = findServer(ok_name);
	যদি (hwnd == 0)
	    ভঙ্গ;

	++i;
	যদি (i >= 1000)
	    ভঙ্গ;

	sprintf((ৱর্ণ *)p, "%d", i);
    }

    যদি (hwnd != 0)
	vim_free(ok_name);
    অন্যথা
    {
	// Remember the name
	serverName = ok_name;
	need_maketitle = TRUE;	// update Vim window title later

	// Update the message window title
	SetWindowText(message_window, (LPCSTR)ok_name);

# ifdef FEAT_EVAL
	// Set the servername variable
	set_vim_var_string(VV_SEND_SERVER, serverName, -1);
# endif
    }
}

    list_T *
serverGetVimNames(শূন্য)
{
    list_T *list = list_alloc();

    যদি (list == রিক্ত)
	ঘূৰাই রিক্ত;

    enum_windows(enumWindowsGetNames, (LPARAM)list);

    ঘূৰাই list;
}

    পূর্ণাঙ্ক
serverSendReply(
    char_u	*name,		// Where to send.
    char_u	*reply)		// What to send.
{
    HWND	target;
    COPYDATASTRUCT data;
    long_u	n = 0;
    DWORD_PTR	dwret = 0;

    // The "name" argument is a magic cookie obtained from expand("<client>").
    // It should be of the form 0xXXXXX - i.e. a C hex literal, which is the
    // value of the client's message window HWND.
    sscanf((ৱর্ণ *)name, SCANF_HEX_LONG_U, &n);
    যদি (n == 0)
	ঘূৰাই -1;

    target = (HWND)n;
    যদি (!IsWindow(target))
	ঘূৰাই -1;

    data.dwData = COPYDATA_REPLY;
    data.cbData = (DWORD)STRLEN(reply) + 1;
    data.lpData = reply;

    যদি (serverSendEnc(target) < 0)
	ঘূৰাই -1;
    যদি (SendMessageTimeout(target, WM_COPYDATA,
		(WPARAM)message_window, (LPARAM)&data,
		SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
	ঘূৰাই -1;
    ঘূৰাই dwret ? 0 : -1;
}

    পূর্ণাঙ্ক
serverSendToVim(
    char_u	 *name,			// Where to send.
    char_u	 *cmd,			// What to send.
    char_u	 **result,		// Result of eval'ed expression
    শূন্য	 *ptarget,		// HWND of server
    পূর্ণাঙ্ক		 as_ৰাশিমালা,		// Expression or keys?
    পূর্ণাঙ্ক		 timeout,		// timeout in seconds or zero
    পূর্ণাঙ্ক		 silent)		// don't complain about no server
{
    size_t	namelen;
    HWND	target;
    COPYDATASTRUCT data;
    char_u	*retval = রিক্ত;
    পূর্ণাঙ্ক		retcode = 0;
    DWORD_PTR	dwret = 0;
    char_u	altname_buf[MAX_PATH];

    // Execute locally if no display or target is ourselves
    যদি (serverName != রিক্ত && STRICMP(name, serverName) == 0)
	ঘূৰাই sendToLocalVim(cmd, as_ৰাশিমালা, result);

    // If the server name does not end in a digit then we look for an
    // alternate name.  e.g. when "name" is GVIM then we may find GVIM2.
    namelen = STRLEN(name);
    যদি (namelen > 1 && !vim_isdigit(name[namelen - 1]))
	altname_buf_ptr = altname_buf;
    altname_buf[0] = NUL;
    target = findServer(name);
    altname_buf_ptr = রিক্ত;
    যদি (target == 0 && altname_buf[0] != NUL)
	// Use another server name we found.
	target = findServer(altname_buf);

    যদি (target == 0)
    {
	যদি (!silent)
	    semsg(_(e_no_registered_server_named_str), name);
	ঘূৰাই -1;
    }

    যদি (ptarget)
	*(HWND *)ptarget = target;

    data.dwData = as_ৰাশিমালা ? COPYDATA_ৰাশিমালা : COPYDATA_চাবিগুলি;
    data.cbData = (DWORD)STRLEN(cmd) + 1;
    data.lpData = cmd;

    যদি (serverSendEnc(target) < 0)
	ঘূৰাই -1;
    যদি (SendMessageTimeout(target, WM_COPYDATA,
		(WPARAM)message_window, (LPARAM)&data,
		SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
	ঘূৰাই -1;
    যদি (dwret == 0)
	ঘূৰাই -1;

    যদি (as_ৰাশিমালা)
	retval = serverGetReply(target, &retcode, TRUE, TRUE, timeout);

    যদি (result == রিক্ত)
	vim_free(retval);
    অন্যথা
	*result = retval; // Caller assumes responsibility for freeing

    ঘূৰাই retcode;
}

/*
 * Bring the server to the foreground.
 */
    শূন্য
serverForeground(char_u *name)
{
    HWND	target = findServer(name);

    যদি (target != 0)
	SetForegroundWindow(target);
}

// Replies from server need to be stored until the client picks them up via
// remote_read(). So we maintain a list of server-id/reply pairs.
// Note that there could be multiple replies from one server pending if the
// client is slow picking them up.
// We just store the replies in a simple list. When we remove an entry, we
// move list entries down to fill the gap.
// The server ID is simply the HWND.
প্রকারনির্দেশ সংরচনা
{
    HWND	server;		// server window
    char_u	*reply;		// reply string
    পূর্ণাঙ্ক		ৰাশিমালা_result;	// 0 for REPLY, 1 for RESULT 2 for error
} reply_T;

স্থির garray_T reply_list = {0, 0, পরিমাণ(reply_T), 5, 0};

# define REPLY_ITEM(i) ((reply_T *)(reply_list.ga_data) + (i))
# define REPLY_গুনতি (reply_list.ga_len)

// Flag which is used to wait for a reply
স্থির পূর্ণাঙ্ক reply_received = 0;

/*
 * Store a reply.  "reply" must be allocated memory (or NULL).
 */
    স্থির পূর্ণাঙ্ক
save_reply(HWND server, char_u *reply, পূর্ণাঙ্ক ৰাশিমালা)
{
    reply_T *rep;

    যদি (ga_grow(&reply_list, 1) == FAIL)
	ঘূৰাই FAIL;

    rep = REPLY_ITEM(REPLY_গুনতি);
    rep->server = server;
    rep->reply = reply;
    rep->ৰাশিমালা_result = ৰাশিমালা;
    যদি (rep->reply == রিক্ত)
	ঘূৰাই FAIL;

    ++REPLY_গুনতি;
    reply_received = 1;
    ঘূৰাই OK;
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
serverGetReply(HWND server, পূর্ণাঙ্ক *ৰাশিমালা_res, পূর্ণাঙ্ক remove, পূর্ণাঙ্ক wait, পূর্ণাঙ্ক timeout)
{
    পূর্ণাঙ্ক		i;
    char_u	*reply;
    reply_T	*rep;
    পূর্ণাঙ্ক		did_process = FALSE;
    time_t	start;
    time_t	now;

    // When waiting, loop until the message waiting for is received.
    time(&start);
    বাবে (;;)
    {
	// Reset this here, in case a message arrives while we are going
	// through the already received messages.
	reply_received = 0;

	বাবে (i = 0; i < REPLY_গুনতি; ++i)
	{
	    rep = REPLY_ITEM(i);
	    যদি (rep->server == server
		    && ((rep->ৰাশিমালা_result != 0) == (ৰাশিমালা_res != রিক্ত)))
	    {
		// Save the values we've found for later
		reply = rep->reply;
		যদি (ৰাশিমালা_res != রিক্ত)
		    *ৰাশিমালা_res = rep->ৰাশিমালা_result == 1 ? 0 : -1;

		যদি (remove)
		{
		    // Move the rest of the list down to fill the gap
		    mch_memmove(rep, rep + 1,
				     (REPLY_গুনতি - i - 1) * পরিমাণ(reply_T));
		    --REPLY_গুনতি;
		}

		// Return the reply to the caller, who takes on responsibility
		// for freeing it if "remove" is TRUE.
		ঘূৰাই reply;
	    }
	}

	// If we got here, we didn't find a reply. Return immediately if the
	// "wait" parameter isn't set.
	যদি (!wait)
	{
	    // Process pending messages once. Without this, looping on
	    // remote_peek() would never get the reply.
	    যদি (!did_process)
	    {
		did_process = TRUE;
		serverProcessPendingMessages();
		অনুৱর্ত;
	    }
	    ভঙ্গ;
	}

	// We need to wait for a reply. Enter a message loop until the
	// "reply_received" flag gets set.

	// Loop until we receive a reply
	যেতিয়ালৈ (reply_received == 0)
	{
# ifdef FEAT_TIMERS
	    // TODO: use the return value to decide how long to wait.
	    check_due_timer();
# endif
	    time(&now);
	    যদি (timeout > 0 && (now - start) >= timeout)
		ভঙ্গ;

	    // Wait for a SendMessage() call to us.  This could be the reply
	    // we are waiting for.  Use a timeout of a second, to catch the
	    // situation that the server died unexpectedly.
	    বাৰ্তা_Wait_For_Multiple_Objects(0, রিক্ত, TRUE, 1000, QS_ALLINPUT);

	    // If the server has died, give up
	    যদি (!IsWindow(server))
		ঘূৰাই রিক্ত;

	    serverProcessPendingMessages();
	}
    }

    ঘূৰাই রিক্ত;
}

/*
 * Process any messages in the Windows message queue.
 */
    শূন্য
serverProcessPendingMessages(শূন্য)
{
    বাৰ্তা_2 বাৰ্তা;

    যেতিয়ালৈ (PeekMessageW(&বাৰ্তা, রিক্ত, 0, 0, PM_REMOVE))
    {
	TranslateMessage(&বাৰ্তা);
	DispatchMessageW(&বাৰ্তা);
    }
}

#endif // FEAT_CLIENTSERVER

#যদি defined(FEAT_GUI) || (defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT))

সংরচনা charset_pair
{
    string_T	name;
    BYTE	charset;
};

স্থির সংরচনা charset_pair
charset_pairs[] =
{
    {STR_LITERAL_INIT("ANSI"),		ANSI_CHARSET},
    {STR_LITERAL_INIT("CHINESEBIG5"),	CHINESEBIG5_CHARSET},
    {STR_LITERAL_INIT("DEFAULT"),	DEFAULT_CHARSET},
    {STR_LITERAL_INIT("HANGEUL"),	HANGEUL_CHARSET},
    {STR_LITERAL_INIT("OEM"),		OEM_CHARSET},
    {STR_LITERAL_INIT("SHIFTJIS"),	SHIFTJIS_CHARSET},
    {STR_LITERAL_INIT("SYMBOL"),	প্ৰতীক_CHARSET},
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

সংরচনা quality_pair
{
    string_T	name;
    DWORD	quality;
};

স্থির সংরচনা quality_pair
quality_pairs[] = {
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
    ৱর্ণ *
charset_id2name(পূর্ণাঙ্ক id)
{
    পূর্ণাঙ্ক	i;

    বাবে (i = 0; i < (পূর্ণাঙ্ক)ARRAY_LENGTH(charset_pairs); ++i)
    {
	যদি ((BYTE)id == charset_pairs[i].charset)
	    ঘূৰাই (ৱর্ণ *)charset_pairs[i].name.string;
    }

    ঘূৰাই রিক্ত;
}

/*
 * Convert a quality ID to a name.
 * Return NULL when not recognized.
 */
    ৱর্ণ *
quality_id2name(DWORD id)
{
    পূর্ণাঙ্ক	i;

    বাবে (i = 0; i < (পূর্ণাঙ্ক)ARRAY_LENGTH(quality_pairs); ++i)
    {
	যদি (id == quality_pairs[i].quality)
	    ঘূৰাই (ৱর্ণ *)quality_pairs[i].name.string;
    }

    ঘূৰাই রিক্ত;
}

// The default font height in 100% scaling (96dpi).
// (-16 in 96dpi equates to roughly 12pt)
# define DEFAULT_FONT_HEIGHT	(-16)

স্থির ধ্রুৱ LOGFONTW s_lfDefault =
{
    DEFAULT_FONT_HEIGHT,
    0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
    PROOF_QUALITY, FIXED_PITCH | FF_DONTCARE,
    L""	// Default font name will be set later based on current language.
};

// This will be initialized when set_default_logfont() is called first time.
// The value will be based on the system DPI.
পূর্ণাঙ্ক current_font_height = 0;		// also used in gui_w32.c

/*
 * Convert a string representing a point size into pixels. The string should
 * be a positive decimal number, with an optional decimal point (eg, "12", or
 * "10.5"). The pixel value is returned, and a pointer to the next unconverted
 * character is stored in *end. The flag "vertical" says whether this
 * calculation is for a vertical (height) size or a horizontal (width) one.
 */
    স্থির পূর্ণাঙ্ক
points_to_pixels(WCHAR *str, WCHAR **end, পূর্ণাঙ্ক vertical, long_i pprinter_dc)
{
    পূর্ণাঙ্ক		pixels;
    পূর্ণাঙ্ক		points = 0;
    পূর্ণাঙ্ক		ভাজক = 0;
    HWND	hwnd = (HWND)0;
    HDC		hdc;
    HDC		printer_dc = (HDC)pprinter_dc;

    যেতিয়ালৈ (*str != NUL)
    {
	যদি (*str == L'.' && ভাজক == 0)
	{
	    // Start keeping a divisor, for later
	    ভাজক = 1;
	}
	অন্যথা
	{
	    যদি (!VIM_ISDIGIT(*str))
		ভঙ্গ;

	    points *= 10;
	    points += *str - L'0';
	    ভাজক *= 10;
	}
	++str;
    }

    যদি (ভাজক == 0)
	ভাজক = 1;

    যদি (printer_dc == রিক্ত)
    {
	hwnd = GetDesktopWindow();
	hdc = GetWindowDC(hwnd);
    }
    অন্যথা
	hdc = printer_dc;

    pixels = MulDiv(points,
		    GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX),
		    72 * ভাজক);

    যদি (printer_dc == রিক্ত)
	ReleaseDC(hwnd, hdc);

    *end = str;
    ঘূৰাই pixels;
}

/*
 * Convert pixel into point size. This is a reverse of points_to_pixels.
 */
    স্থির দ্ৱিগুণ
pixels_to_points(পূর্ণাঙ্ক pixels, পূর্ণাঙ্ক vertical, long_i pprinter_dc)
{
    দ্ৱিগুণ	points = 0;
    HWND	hwnd = (HWND)0;
    HDC		hdc;
    HDC		printer_dc = (HDC)pprinter_dc;

    যদি (printer_dc == রিক্ত)
    {
	hwnd = GetDesktopWindow();
	hdc = GetWindowDC(hwnd);
    }
    অন্যথা
	hdc = printer_dc;

    points = pixels * 72.0 / GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX);
    যদি (printer_dc == রিক্ত)
	ReleaseDC(hwnd, hdc);

    ঘূৰাই points;
}

    স্থির পূর্ণাঙ্ক CALLBACK
font_enumproc(
    ENUMLOGFONTW    *elf,
    NEWTEXTMETRICW  *ntm UNUSED,
    DWORD	    type UNUSED,
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
    যদি ((ntm->tmPitchAndFamily & 1) != 0)
	ঘূৰাই 1;
# endif

    // Remember this LOGFONTW as a "possible"
    *lf = elf->elfLogFont;

    // Terminate the scan as soon as we find an ANSI font
    যদি (lf->lfCharSet == ANSI_CHARSET
	    || lf->lfCharSet == OEM_CHARSET
	    || lf->lfCharSet == DEFAULT_CHARSET)
	ঘূৰাই 0;

    // Continue the scan - we have a non-ANSI font
    ঘূৰাই 2;
}

    স্থির পূর্ণাঙ্ক
init_logfont(LOGFONTW *lf)
{
    পূর্ণাঙ্ক		n;
    HWND	hwnd = GetDesktopWindow();
    HDC		hdc = GetWindowDC(hwnd);

    n = EnumFontFamiliesW(hdc,
			 lf->lfFaceName,
			 (FONTENUMPROCW)font_enumproc,
			 (LPARAM)lf);

    ReleaseDC(hwnd, hdc);

    // If we couldn't find a usable font, return failure
    যদি (n == 1)
	ঘূৰাই FAIL;

    // Tidy up the rest of the LOGFONTW structure. We set to a basic
    // font - get_logfont() sets bold, italic, etc based on the user's
    // input.
    lf->lfHeight = current_font_height;
    lf->lfWidth = 0;
    lf->lfItalic = FALSE;
    lf->lfUnderline = FALSE;
    lf->lfStrikeOut = FALSE;
    lf->lfWeight = FW_NORMAL;

    // Return success
    ঘূৰাই OK;
}

/*
 * Call back for EnumFontFamiliesW in expand_font_enumproc.
 *
 */
    স্থির পূর্ণাঙ্ক CALLBACK
expand_font_enumproc(
    ENUMLOGFONTW    *elf,
    NEWTEXTMETRICW  *ntm UNUSED,
    DWORD	    type UNUSED,
    LPARAM	    lparam)
{
    LOGFONTW *lf = (LOGFONTW*)elf;

# ifndef FEAT_PROPORTIONAL_FONTS
    // Ignore non-monospace fonts without further ado
    যদি ((ntm->tmPitchAndFamily & 1) != 0)
	ঘূৰাই 1;
# endif

    // Filter only on ANSI. Otherwise will see a lot of random fonts that we
    // usually don't want.
    যদি (lf->lfCharSet != ANSI_CHARSET)
	ঘূৰাই 1;

    পূর্ণাঙ্ক (*add_match)(char_u *) = (পূর্ণাঙ্ক (*)(char_u *))lparam;

    WCHAR *faceNameW = lf->lfFaceName;
    char_u *faceName = utf16_to_enc(faceNameW, রিক্ত);
    যদি (!faceName)
	ঘূৰাই 0;

    add_match(faceName);
    vim_free(faceName);

    ঘূৰাই 1;
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
    শূন্য
gui_mch_expand_font(
    optexpand_T	*args,
    শূন্য	*param UNUSED,
    পূর্ণাঙ্ক		(*add_match)(char_u *val))
{
    expand_T	    *xp = args->oe_xp;
    যদি (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	ৱর্ণ buf[30];
	পূর্ণাঙ্ক	i;

	// Always fill in with the current font size as first option for
	// convenience. We simply round to the closest integer for simplicity.
	পূর্ণাঙ্ক font_height = (পূর্ণাঙ্ক)round(
		pixels_to_points(-current_font_height, TRUE, (long_i)রিক্ত));
	vim_snprintf(buf, ARRAY_LENGTH(buf), "h%d", font_height);
	add_match((char_u *)buf);

	// Note: Keep this in sync with get_logfont(). Don't include 'c' and
	// 'q' as we fill in all the values below.
	স্থির ৱর্ণ *(p_gfn_win_opt_মানসমূহ[]) = {
	    "h" , "w" , "W" , "b" , "i" , "u" , "s"};
	বাবে (i = 0; i < (পূর্ণাঙ্ক)ARRAY_LENGTH(p_gfn_win_opt_মানসমূহ); i++)
	    add_match((char_u *)p_gfn_win_opt_মানসমূহ[i]);

	বাবে (i = 0; i < (পূর্ণাঙ্ক)ARRAY_LENGTH(charset_pairs); ++i)
	{
	    vim_snprintf(buf, পরিমাণ(buf), "c%s", charset_pairs[i].name.string);
	    add_match((char_u *)buf);
	}

	বাবে (i = 0; i < (পূর্ণাঙ্ক)ARRAY_LENGTH(quality_pairs); ++i)
	{
	    vim_snprintf(buf, পরিমাণ(buf), "q%s", quality_pairs[i].name.string);
	    add_match((char_u *)buf);
	}
	ঘূৰাই;
    }

    HWND	hwnd = GetDesktopWindow();
    HDC		hdc = GetWindowDC(hwnd);

    EnumFontFamiliesW(hdc,
	    রিক্ত,
	    (FONTENUMPROCW)expand_font_enumproc,
	    (LPARAM)add_match);

    ReleaseDC(hwnd, hdc);
}

/*
 * Compare a UTF-16 string and an ASCII string literally.
 * Only works all the code points are inside ASCII range.
 */
    স্থির পূর্ণাঙ্ক
utf16ascncmp(ধ্রুৱ WCHAR *w, ধ্রুৱ ৱর্ণ *p, size_t n)
{
    size_t i;

    বাবে (i = 0; i < n; i++)
    {
	যদি (w[i] == 0 || w[i] != p[i])
	    ঘূৰাই w[i] - p[i];
    }
    ঘূৰাই 0;
}

/*
 * Equivalent of GetDpiForSystem().
 */
    UINT WINAPI
vimGetDpiForSystem(শূন্য)
{
    HWND hwnd = GetDesktopWindow();
    HDC hdc = GetWindowDC(hwnd);
    UINT dpi = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(hwnd, hdc);
    ঘূৰাই dpi;
}

/*
 * Set default logfont based on current language.
 */
    স্থির শূন্য
set_default_logfont(LOGFONTW *lf)
{
    // Default font name for current language on MS-Windows.
    // If not translated, falls back to "Consolas".
    // This must be a fixed-pitch font.
    ধ্রুৱ ৱর্ণ *defaultfontname = N_("DefaultFontNameForWindows");
    ৱর্ণ *fontname = _(defaultfontname);

    যদি (STRCMP(fontname, defaultfontname) == 0)
	fontname = "Consolas";

    *lf = s_lfDefault;
    lf->lfHeight = DEFAULT_FONT_HEIGHT * (পূর্ণাঙ্ক)vimGetDpiForSystem() / 96;
    যদি (current_font_height == 0)
	current_font_height = lf->lfHeight;

    WCHAR *wfontname = enc_to_utf16((char_u*)fontname, রিক্ত);
    যদি (wfontname != রিক্ত)
    {
	wcscpy_s(lf->lfFaceName, LF_FACESIZE, wfontname);
	vim_free(wfontname);
    }
}

/*
 * Get font info from "name" into logfont "lf".
 * Return OK for a valid name, FAIL otherwise.
 */
    পূর্ণাঙ্ক
get_logfont(
    LOGFONTW	*lf,
    char_u	*name,
    HDC		printer_dc,
    পূর্ণাঙ্ক		verbose)
{
    WCHAR	*p;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		ret = FAIL;
    স্থির LOGFONTW *lastlf = রিক্ত;
    WCHAR	*wname;

    set_default_logfont(lf);
    যদি (name == রিক্ত)
	ঘূৰাই OK;

    wname = enc_to_utf16(name, রিক্ত);
    যদি (wname == রিক্ত)
	ঘূৰাই FAIL;

    যদি (wcscmp(wname, L"*") == 0)
    {
# যদি defined(FEAT_GUI_MSWIN)
	CHOOSEFONTW	cf;
	// if name is "*", bring up std font dialog:
	CLEAR_FIELD(cf);
	cf.lStructSize = পরিমাণ(cf);
	cf.hwndOwner = s_hwnd;
	cf.Flags = CF_SCREENFONTS | CF_FIXEDPITCHONLY | CF_INITTOLOGFONTSTRUCT;
	যদি (lastlf != রিক্ত)
	    *lf = *lastlf;
	cf.lpLogFont = lf;
	cf.nFontType = 0 ; //REGULAR_FONTTYPE;
	যদি (ChooseFontW(&cf))
	    ret = OK;
# endif
	গচ্ছ theend;
    }

    /*
     * Split name up, it could be <name>:h<height>:w<width> etc.
     */
    বাবে (p = wname; *p && *p != L':'; p++)
    {
	যদি (p - wname + 1 >= LF_FACESIZE)
	    গচ্ছ theend;			// Name too long
	lf->lfFaceName[p - wname] = *p;
    }
    যদি (p != wname)
	lf->lfFaceName[p - wname] = NUL;

    // First set defaults
    lf->lfHeight = DEFAULT_FONT_HEIGHT * (পূর্ণাঙ্ক)vimGetDpiForSystem() / 96;
    lf->lfWidth = 0;
    lf->lfWeight = FW_NORMAL;
    lf->lfItalic = FALSE;
    lf->lfUnderline = FALSE;
    lf->lfStrikeOut = FALSE;

    /*
     * If the font can't be found, try replacing '_' by ' '.
     */
    যদি (init_logfont(lf) == FAIL)
    {
	পূর্ণাঙ্ক	did_replace = FALSE;

	বাবে (i = 0; lf->lfFaceName[i]; ++i)
	    যদি (lf->lfFaceName[i] == L'_')
	    {
		lf->lfFaceName[i] = L' ';
		did_replace = TRUE;
	    }
	যদি (!did_replace || init_logfont(lf) == FAIL)
	    গচ্ছ theend;
    }

    যেতিয়ালৈ (*p == L':')
	p++;

    // Set the values found after ':'
    যেতিয়ালৈ (*p)
    {
	ৱিকল্প (*p++)
	{
	    // Note: Keep this in sync with gui_mch_expand_font().
	    পক্ষ L'h':
		lf->lfHeight = - points_to_pixels(p, &p, TRUE, (long_i)printer_dc);
		ভঙ্গ;
	    পক্ষ L'w':
		lf->lfWidth = points_to_pixels(p, &p, FALSE, (long_i)printer_dc);
		ভঙ্গ;
	    পক্ষ L'W':
		lf->lfWeight = wcstol(p, &p, 10);
		ভঙ্গ;
	    পক্ষ L'b':
		lf->lfWeight = FW_BOLD;
		ভঙ্গ;
	    পক্ষ L'i':
		lf->lfItalic = TRUE;
		ভঙ্গ;
	    পক্ষ L'u':
		lf->lfUnderline = TRUE;
		ভঙ্গ;
	    পক্ষ L's':
		lf->lfStrikeOut = TRUE;
		ভঙ্গ;
	    পক্ষ L'c':
		বাবে (i = 0; i < (পূর্ণাঙ্ক)ARRAY_LENGTH(charset_pairs); ++i)
		{
		    যদি (utf16ascncmp(p, (ৱর্ণ *)charset_pairs[i].name.string,
			    charset_pairs[i].name.length) == 0)
		    {
			lf->lfCharSet = charset_pairs[i].charset;
			p += charset_pairs[i].name.length;
			ভঙ্গ;
		    }
		}

		যদি (i == (পূর্ণাঙ্ক)ARRAY_LENGTH(charset_pairs) && verbose)
		{
		    char_u *s = utf16_to_enc(p, রিক্ত);
		    যদি (s != রিক্ত)
		    {
			semsg(_(e_illegal_str_name_str_in_font_name_str),
							   "charset", s, name);
			vim_free(s);
		    }
		}
		ভঙ্গ;
	    পক্ষ L'f':
		// Font features (e.g., "fss19=1").
		// Parsed separately by gui_mch_init_font(); skip here.
		যেতিয়ালৈ (*p && *p != L':')
		    p++;
		ভঙ্গ;
	    পক্ষ L'q':
		বাবে (i = 0; i < (পূর্ণাঙ্ক)ARRAY_LENGTH(quality_pairs); ++i)
		{
		    যদি (utf16ascncmp(p, (ৱর্ণ *)quality_pairs[i].name.string,
			    quality_pairs[i].name.length) == 0)
		    {
			lf->lfQuality = quality_pairs[i].quality;
			p += quality_pairs[i].name.length;
			ভঙ্গ;
		    }
		}

		যদি (i == (পূর্ণাঙ্ক)ARRAY_LENGTH(quality_pairs) && verbose)
		{
		    char_u *s = utf16_to_enc(p, রিক্ত);
		    যদি (s != রিক্ত)
		    {
			semsg(_(e_illegal_str_name_str_in_font_name_str),
							   "quality", s, name);
			vim_free(s);
		    }
		}
		ভঙ্গ;
	    স্ৱভাৱ:
		যদি (verbose)
		    semsg(_(e_illegal_char_nr_in_font_name_str), p[-1], name);
		গচ্ছ theend;
	}
	যেতিয়ালৈ (*p == L':')
	    p++;
    }
    ret = OK;

theend:
    // ron: init lastlf
    যদি (ret == OK && printer_dc == রিক্ত)
    {
	vim_free(lastlf);
	lastlf = ALLOC_ONE(LOGFONTW);
	যদি (lastlf != রিক্ত)
	    mch_memmove(lastlf, lf, পরিমাণ(LOGFONTW));
    }
    vim_free(wname);

    ঘূৰাই ret;
}

#endif // defined(FEAT_GUI) || defined(FEAT_PRINTER)

#যদি defined(FEAT_JOB_CHANNEL)
/*
 * Initialize the Winsock dll.
 */
    শূন্য
channel_init_winsock(শূন্য)
{
    WSADATA wsaData;
    পূর্ণাঙ্ক wsaerr;

    যদি (WSInitialized)
	ঘূৰাই;

    wsaerr = WSAStartup(MAKEWORD(2, 2), &wsaData);
    যদি (wsaerr == 0)
	WSInitialized = TRUE;
}
#endif
