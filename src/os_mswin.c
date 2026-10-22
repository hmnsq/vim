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

#অন্তর্ভাব "vim.h"

#অন্তর্ভাব <sys/types.h>
#অন্তর্ভাব <limits.h>

#অন্তর্ভাব <process.h>
#অন্তর্ভাব <direct.h>

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
FILE* fdDump = ফাঁকা;
#endif

#যদি !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
বাহ্যিক অক্ষর g_szOrigTitle[];
#endif

#ifdef FEAT_GUI
বাহ্যিক HWND s_hwnd;
#অন্যথা
স্থির HWND s_hwnd = 0;	    // console window handle, set by GetConsoleHwnd()
#endif

#ifdef FEAT_JOB_CHANNEL
পূর্ণসংখ্যা WSInitialized = FALSE; // WinSock is initialized
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
mch_exit_g(পূর্ণসংখ্যা r)
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
প্রকারলেখ BOOL (WINAPI *PfnRtlGetVersion)(LPOSVERSIONINFOW);
DWORD win_version;
    স্থির শূন্য
win_version_init(শূন্য)
{
    OSVERSIONINFOW	osver;
    HMODULE		hNtdll;
    PfnRtlGetVersion	pRtlGetVersion;

    hNtdll = GetModuleHandle("ntdll.dll");
    যদি (hNtdll == ফাঁকা)
	ফেরত;

    pRtlGetVersion =
	(PfnRtlGetVersion) GetProcAddress(hNtdll, "RtlGetVersion");
    যদি (pRtlGetVersion == ফাঁকা)
	ফেরত;

    osver.dw_OSVersion_Info_আকার = আকার(OSVERSIONINFOW);
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
    পূর্ণসংখ্যা		i;

    win_version_init();

    PlatformId();

    // Init the tables for toupper() and tolower()
    জন্য (i = 0; i < 256; ++i)
	toupper_tab[i] = tolower_tab[i] = i;
    CharUpperBuff((LPSTR)toupper_tab, 256);
    CharLowerBuff((LPSTR)tolower_tab, 256);
}


/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    পূর্ণসংখ্যা
mch_প্রবেশ_isatty(শূন্য)
{
#ifdef FEAT_GUI_MSWIN
# ifdef VIMDLL
    যদি (gui.in_use)
# endif
	ফেরত TRUE;	    // GUI always has a tty
#endif
#যদি !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    যদি (isatty(read_cmd_fd))
	ফেরত TRUE;
    ফেরত FALSE;
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
	ফেরত;
    }
#endif
#যদি !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    যদি (title != ফাঁকা)
    {
	WCHAR	*wp = enc_to_utf16(title, ফাঁকা);

	যদি (wp == ফাঁকা)
	    ফেরত;

	SetConsoleTitleW(wp);
	vim_free(wp);
	ফেরত;
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
mch_restore_title(পূর্ণসংখ্যা which UNUSED)
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
    পূর্ণসংখ্যা
mch_can_restore_title(শূন্য)
{
    ফেরত TRUE;
}


/*
 * Return TRUE if we can restore the icon title (we can't)
 */
    পূর্ণসংখ্যা
mch_can_restore_icon(শূন্য)
{
    ফেরত FALSE;
}


/*
 * Get absolute file name into buffer "buf" of length "len" bytes,
 * turning all '/'s into '\\'s and getting the correct case of each component
 * of the file name.  Append a (back)slash to a directory name.
 * When 'shellslash' set do it the other way around.
 * Return OK or FAIL.
 */
    পূর্ণসংখ্যা
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    পূর্ণসংখ্যা		len,
    পূর্ণসংখ্যা		force UNUSED)
{
    পূর্ণসংখ্যা		n_ফলাফল = FAIL;
    WCHAR	*wname;
    WCHAR	wbuf[MAX_PATH];
    char_u	*cname = ফাঁকা;

    wname = enc_to_utf16(fname, ফাঁকা);
    যদি (wname != ফাঁকা && _wfullpath(wbuf, wname, MAX_PATH) != ফাঁকা)
    {
	cname = utf16_to_enc((short_u *)wbuf, ফাঁকা);
	যদি (cname != ফাঁকা)
	{
	    vim_strncpy(buf, cname, len - 1);
	    n_ফলাফল = OK;
	}
    }
    vim_free(wname);
    vim_free(cname);

#ifdef USE_FNAME_CASE
    fname_case(buf, len);
#অন্যথা
    slash_adjust(buf);
#endif

    ফেরত n_ফলাফল;
}


/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    পূর্ণসংখ্যা
mch_isFullName(char_u *fname)
{
    // A name like "d:/foo" and "//server/share" is absolute.  "d:foo" is not.
    // /foo and \foo are absolute too because Windows keeps a current drive.
    // Another way to check is to use mch_FullName() and see if the result is
    // the same as the name or mch_FullName() fails.  However, this has quite a
    // bit of overhead, so let's not do that.
    যদি (*fname == NUL)
	ফেরত FALSE;
    ফেরত ((ASCII_ISALPHA(fname[0]) && fname[1] == ':'
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
	ফেরত;

    যদি (*p == '`')
    {
	size_t len = STRLEN(p);

	// don't replace backslash in backtick quoted strings
	যদি (len > 2 && *(p + len - 1) == '`')
	    ফেরত;
    }

    যখন (*p)
    {
	যদি (*p == psepcN)
	    *p = psepc;
	MB_PTR_ADV(p);
    }
}

    স্থির পূর্ণসংখ্যা
read_reparse_point(ধ্রুবক WCHAR *name, char_u *buf, DWORD *buf_len)
{
    HANDLE h;
    BOOL ok;

    h = CreateFileW(name, FILE_READ_ATTRIBUTES,
	    FILE_SHARE_READ | FILE_SHARE_WRITE, ফাঁকা,
	    OPEN_EXISTING,
	    FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,
	    ফাঁকা);
    যদি (h == INVALID_HANDLE_মান)
	ফেরত FAIL;

    ok = DeviceIoControl(h, FSCTL_GET_REPARSE_POINT, ফাঁকা, 0, buf, *buf_len,
	    buf_len, ফাঁকা);
    CloseHandle(h);

    ফেরত ok ? OK : FAIL;
}

    char_u *
resolve_appexeclink(char_u *fname)
{
    DWORD		attr = 0;
    পূর্ণসংখ্যা			idx;
    WCHAR		*p, *end, *wname;
    // The buffer size is arbitrarily chosen to be "big enough" (TM), the
    // ceiling should be around 16k.
    char_u		buf[4096];
    DWORD		buf_len = আকার(buf);
    REPARSE_তথ্য_বাফার *rb = (REPARSE_তথ্য_বাফার *)buf;

    wname = enc_to_utf16(fname, ফাঁকা);
    যদি (wname == ফাঁকা)
	ফেরত ফাঁকা;

    attr = GetFileAttributesW(wname);
    যদি (attr == INVALID_FILE_ATTRIBUTES ||
	    (attr & FILE_ATTRIBUTE_REPARSE_POINT) == 0)
    {
	vim_free(wname);
	ফেরত ফাঁকা;
    }

    // The applinks are similar to symlinks but with a huge difference: they can
    // only be executed, any other I/O operation on them is bound to fail with
    // ERROR_FILE_NOT_FOUND even though the file exists.
    যদি (read_reparse_point(wname, buf, &buf_len) == FAIL)
    {
	vim_free(wname);
	ফেরত ফাঁকা;
    }
    vim_free(wname);

    যদি (rb->ReparseTag != IO_REPARSE_TAG_APPEXECLINK)
	ফেরত ফাঁকা;

    // The (undocumented) reparse buffer contains a set of N null-terminated
    // Unicode strings, the application path is stored in the third one.
    যদি (rb->App_Exec_Link_Reparse_বাফার.String_গুনতি < 3)
	ফেরত ফাঁকা;

    p = rb->App_Exec_Link_Reparse_বাফার.StringList;
    end = p + rb->Reparse_তথ্য_দৈর্ঘ্য / আকার(WCHAR);
    জন্য (idx = 0; p < end
	    && idx < (পূর্ণসংখ্যা)rb->App_Exec_Link_Reparse_বাফার.String_গুনতি
	    && idx != 2; )
    {
	যদি (*p++ == L'\0')
	    ++idx;
    }

    ফেরত utf16_to_enc(p, ফাঁকা);
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
    স্থির পূর্ণসংখ্যা
mswin_stat_impl(ধ্রুবক WCHAR *name, stat_T *stp, ধ্রুবক পূর্ণসংখ্যা resolve)
{
    পূর্ণসংখ্যা			n;
    পূর্ণসংখ্যা			fd;
    BOOL		is_symlink = FALSE;
    HANDLE		hFind, h;
    DWORD		attr = 0;
    DWORD		flag = 0;
    WIN32_FIND_DATAW    find_তথ্য_W;

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
	ফেরত _wstat(name, stp);
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
	    ফেরত -1;
	}
	ফেরত _wstat(name, stp);
    }
    যদি (!(attr & FILE_ATTRIBUTE_REPARSE_POINT))
	ফেরত _wstat(name, stp);

    hFind = FindFirstFileW(name, &find_তথ্য_W);
    যদি (hFind != INVALID_HANDLE_মান)
    {
	attr = find_তথ্য_W.dwFileAttributes;
	যদি ((attr & FILE_ATTRIBUTE_REPARSE_POINT)
		&& (find_তথ্য_W.dwReserved0 == IO_REPARSE_TAG_SYMLINK))
	    is_symlink = TRUE;
	FindClose(hFind);
    }

    // Use the plain old stat() whenever it's possible.
    যদি (!is_symlink)
	ফেরত _wstat(name, stp);

    যদি (!resolve && is_symlink)
	flag = FILE_FLAG_OPEN_REPARSE_POINT;
    যদি (attr & FILE_ATTRIBUTE_DIRECTORY)
	flag |= FILE_FLAG_BACKUP_SEMANTICS;

    h = CreateFileW(name, FILE_READ_ATTRIBUTES,
	    FILE_SHARE_READ | FILE_SHARE_WRITE, ফাঁকা, OPEN_EXISTING, flag,
	    ফাঁকা);
    যদি (h == INVALID_HANDLE_মান)
	ফেরত -1;

    fd = _open_osfhandle((intptr_t)h, _O_RDONLY);
    n = _fstat(fd, (কাঠামো _stat *)stp);
    যদি ((n == 0) && (attr & FILE_ATTRIBUTE_DIRECTORY))
	stp->st_mode = (stp->st_mode & ~S_IFMT) | S_IFDIR;
    _close(fd);

    ফেরত n;
}

/*
 * stat() can't handle a trailing '/' or '\', remove it first.
 * When 'resolve' is true behave as lstat() wrt symlinks.
 */
    স্থির পূর্ণসংখ্যা
stat_impl(ধ্রুবক অক্ষর *name, stat_T *stp, ধ্রুবক পূর্ণসংখ্যা resolve)
{
    // WinNT and later can use _MAX_PATH wide characters for a pathname, which
    // means that the maximum pathname is _MAX_PATH * 3 bytes when 'enc' is
    // UTF-8.
    char_u	buf[_MAX_PATH * 3 + 1];
    size_t	buflen;
    char_u	*p;
    WCHAR	*wp;
    পূর্ণসংখ্যা		n;

    vim_strncpy((char_u *)buf, (char_u *)name, আকার(buf) - 1);
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
	যদি (p != ফাঁকা)
	{
	    p = vim_strpbrk(p + 1, (char_u *)"\\/");
	    যদি (p == ফাঁকা)
		STRCPY(buf + buflen, "\\");
	}
    }

    wp = enc_to_utf16(buf, ফাঁকা);
    যদি (wp == ফাঁকা)
	ফেরত -1;

    n = mswin_stat_impl(wp, stp, resolve);
    vim_free(wp);
    ফেরত n;
}

    পূর্ণসংখ্যা
vim_lstat(ধ্রুবক অক্ষর *name, stat_T *stp)
{
    ফেরত stat_impl(name, stp, FALSE);
}

    পূর্ণসংখ্যা
vim_stat(ধ্রুবক অক্ষর *name, stat_T *stp)
{
    ফেরত stat_impl(name, stp, TRUE);
}

#যদি defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    শূন্য
mch_settmode(tmode_T tmode UNUSED)
{
    // nothing to do
}

    পূর্ণসংখ্যা
mch_get_shellsize(শূন্য)
{
    // never used
    ফেরত OK;
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
mch_calc_cell_আকার(কাঠামো cellsize *cs_out UNUSED)
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
	যদি (error_ga.ga_তথ্য != ফাঁকা)
	{
	    // avoid putting up a message box with blanks only
	    জন্য (p = (char_u *)error_ga.ga_তথ্য; *p; ++p)
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
					1, ফাঁকা, FALSE);
		    বিরতি;
		}
	    ga_clear(&error_ga);
	}
	ফেরত;
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
    পূর্ণসংখ্যা
mch_has_exp_wildcard(char_u *p)
{
    জন্য ( ; *p; MB_PTR_ADV(p))
    {
	যদি (vim_strchr((char_u *)"?*[", *p) != ফাঁকা
		|| (*p == '~' && p[1] != NUL))
	    ফেরত TRUE;
    }
    ফেরত FALSE;
}

/*
 * Return TRUE if "p" contain a wildcard or a "~1" kind of thing (could be a
 * shortened file name).
 */
    পূর্ণসংখ্যা
mch_has_wildcard(char_u *p)
{
    জন্য ( ; *p; MB_PTR_ADV(p))
    {
	যদি (vim_strchr((char_u *)"?*$[`", *p) != ফাঁকা
		|| (*p == '~' && p[1] != NUL))
	    ফেরত TRUE;
    }
    ফেরত FALSE;
}


/*
 * The normal _chdir() does not change the default drive.  This one does.
 * Returning 0 implies success; -1 implies failure.
 */
    পূর্ণসংখ্যা
mch_chdir(অক্ষর *path)
{
    WCHAR   *p;
    পূর্ণসংখ্যা	    n;

    যদি (path[0] == NUL)		// just checking...
	ফেরত -1;

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
	ফেরত 0;

    p = enc_to_utf16((char_u *)path, ফাঁকা);
    যদি (p == ফাঁকা)
	ফেরত -1;

    n = _wchdir(p);
    vim_free(p);
    ফেরত n;
}


#যদি defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
/*
 * return non-zero if a character is available
 */
    পূর্ণসংখ্যা
mch_char_avail(শূন্য)
{
    // never used
    ফেরত TRUE;
}

# যদি defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    পূর্ণসংখ্যা
mch_check_messages(শূন্য)
{
    // TODO: check for messages
    ফেরত TRUE;
}
# endif
#endif


#যদি defined(FEAT_LIBCALL)
/*
 * Call a DLL routine which takes either a string or int param
 * and returns an allocated string.
 * Return OK if it worked, FAIL if not.
 */
প্রকারলেখ char_u *(*MYSTRPROCSTR)(char_u *);
প্রকারলেখ পূর্ণসংখ্যা (*MYSTRPROCINT)(char_u *);
প্রকারলেখ char_u *(*MYINTPROCSTR)(পূর্ণসংখ্যা);
প্রকারলেখ পূর্ণসংখ্যা (*MYINTPROCINT)(পূর্ণসংখ্যা);

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
    size_t			দৈর্ঘ্য = 0;
    size_t			i;
    ধ্রুবক char_u		*p;

    // get page size
    GetSystemInfo(&si);

    // get memory information
    যদি (!VirtualQuery(str, &mbi, আকার(mbi)))
	ফেরত 0;

    // pre cast these (typing savers)
    long_u dwStr = (long_u)str;
    long_u dwBaseAddress = (long_u)mbi.BaseAddress;

    // get start address of page that str is on
    long_u strPage = dwStr - (dwStr - dwBaseAddress) % si.dw_Page_আকার;

    // get length from str to end of page
    long_u page_দৈর্ঘ্য = si.dw_Page_আকার - (dwStr - strPage);

    জন্য (p = str; !IsBadReadPtr(p, (UINT)page_দৈর্ঘ্য);
	    p += page_দৈর্ঘ্য, page_দৈর্ঘ্য = si.dw_Page_আকার)
	জন্য (i = 0; i < page_দৈর্ঘ্য; ++i, ++দৈর্ঘ্য)
	    যদি (p[i] == NUL)
		ফেরত দৈর্ঘ্য + 1;

    ফেরত 0;
}

/*
 * Passed to do_in_runtimepath() to load a vim.ico file.
 */
    স্থির শূন্য
mch_icon_load_cb(char_u *fname, শূন্য *cookie)
{
    HANDLE *h = (HANDLE *)cookie;

    *h = LoadImage(ফাঁকা,
		   (LPSTR)fname,
		   IMAGE_ICON,
		   64,
		   64,
		   LR_LOADFROMFILE | LR_LOADMAP3DCOLORS);
}

/*
 * Try loading an icon file from 'runtimepath'.
 */
    পূর্ণসংখ্যা
mch_icon_load(HANDLE *iconp)
{
    ফেরত do_in_runtimepath((char_u *)"bitmaps/vim.ico",
						  0, mch_icon_load_cb, iconp);
}

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if the OS PRNG is not available or something went wrong.
 */
    পূর্ণসংখ্যা
mch_get_random(char_u *buf, পূর্ণসংখ্যা len)
{
    স্থির পূর্ণসংখ্যা		initialized = NOTDONE;
    স্থির HINSTANCE	hInstLib;
    স্থির BOOL (WINAPI *pProcessPrng)(PUCHAR, ULONG);

    যদি (initialized == NOTDONE)
    {
	hInstLib = vimLoadLib("bcryptprimitives.dll");
	যদি (hInstLib != ফাঁকা)
	    pProcessPrng = (শূন্য *)GetProcAddress(hInstLib, "ProcessPrng");
	যদি (hInstLib == ফাঁকা || pProcessPrng == ফাঁকা)
	{
	    FreeLibrary(hInstLib);
	    initialized = FAIL;
	}
	অন্যথা
	    initialized = OK;
    }

    যদি (initialized == FAIL)
	ফেরত FAIL;

    // According to the documentation this call cannot fail.
    pProcessPrng(buf, len);

    ফেরত OK;
}

    পূর্ণসংখ্যা
mch_libcall(
    char_u	*libname,
    char_u	*funcname,
    char_u	*argstring,	// NULL when using a argint
    পূর্ণসংখ্যা		argint,
    char_u	**string_ফলাফল,// NULL when using number_result
    পূর্ণসংখ্যা		*সংখ্যা_ফলাফল)
{
    HINSTANCE		hinstLib;

    BOOL fRunTimeLinkSuccess = FALSE;

    // Get a handle to the DLL module.
    hinstLib = vimLoadLib((অক্ষর *)libname);

    // If the handle is valid, try to get the function address.
    যদি (hinstLib != ফাঁকা)
    {
	char_u	*retval_str = ফাঁকা;
	পূর্ণসংখ্যা	retval_int = 0;
	size_t	len;

# ifdef HAVE_TRY_EXCEPT
	__try
	{
# endif
	যদি (argstring != ফাঁকা)
	{
	    // Call with string argument
	    যদি (string_ফলাফল != ফাঁকা)
	    {
		MYSTRPROCSTR proc;

		proc = (MYSTRPROCSTR)GetProcAddress(hinstLib, (LPCSTR)funcname);
		যদি (proc != ফাঁকা)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_str = proc(argstring);
		}
	    }
	    অন্যথা
	    {
		MYSTRPROCINT proc;

		proc = (MYSTRPROCINT)GetProcAddress(hinstLib, (LPCSTR)funcname);
		যদি (proc != ফাঁকা)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_int = proc(argstring);
		}
	    }
	}
	অন্যথা
	{
	    // Call with number argument
	    যদি (string_ফলাফল != ফাঁকা)
	    {
		MYINTPROCSTR proc;

		proc = (MYINTPROCSTR)GetProcAddress(hinstLib, (LPCSTR)funcname);
		যদি (proc != ফাঁকা)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_str = proc(argint);
		}
	    }
	    অন্যথা
	    {
		MYINTPROCINT proc;

		proc = (MYINTPROCINT)GetProcAddress(hinstLib, (LPCSTR)funcname);
		যদি (proc != ফাঁকা)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_int = proc(argint);
		}
	    }
	}

	// Save the string before we free the library.
	// Assume that a "1" result is an illegal pointer.
	যদি (string_ফলাফল == ফাঁকা)
	    *সংখ্যা_ফলাফল = retval_int;
	অন্যথা যদি (retval_str != ফাঁকা
		&& (len = check_str_len(retval_str)) > 0)
	{
	    *string_ফলাফল = alloc(len);
	    যদি (*string_ফলাফল != ফাঁকা)
		mch_memmove(*string_ফলাফল, retval_str, len);
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
	ফেরত FAIL;
    }

    ফেরত OK;
}
#endif

/*
 * Debugging helper: expose the MCH_WRITE_DUMP stuff to other modules
 */
    শূন্য
DumpPutS(ধ্রুবক অক্ষর *psz UNUSED)
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
    অক্ষর *pszFormat,
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
বাহ্যিক HWND g_hWnd;	// This is in os_win32.c.

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
	ফেরত;

    // Window handle may have been found by init code (Windows NT only)
    যদি (g_hWnd != 0)
    {
	s_hwnd = g_hWnd;
	ফেরত;
    }

    s_hwnd = GetConsoleWindow();
}

/*
 * Console implementation of ":winpos".
 */
    পূর্ণসংখ্যা
mch_get_winpos(পূর্ণসংখ্যা *x, পূর্ণসংখ্যা *y)
{
    RECT  rect;

    GetConsoleHwnd();
    GetWindowRect(s_hwnd, &rect);
    *x = rect.বাম;
    *y = rect.শীর্ষ_2;
    ফেরত OK;
}

/*
 * Console implementation of ":winpos x y".
 */
    শূন্য
mch_set_winpos(পূর্ণসংখ্যা x, পূর্ণসংখ্যা y)
{
    GetConsoleHwnd();
    SetWindowPos(s_hwnd, ফাঁকা, x, y, 0, 0,
		 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}
#endif

#যদি defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT)

//=================================================================
// Win32 printer stuff

স্থির HFONT		prt_font_handles[2][2][2];
স্থির PRINTDLGW	prt_dlg;
স্থির ধ্রুবক পূর্ণসংখ্যা	boldface[2] = {FW_REGULAR, FW_BOLD};
স্থির TEXTMETRIC	prt_tm;
স্থির পূর্ণসংখ্যা		prt_সারি_height;
স্থির পূর্ণসংখ্যা		prt_সংখ্যা_width;
স্থির পূর্ণসংখ্যা		prt_বাম_margin;
স্থির পূর্ণসংখ্যা		prt_ডান_margin;
স্থির পূর্ণসংখ্যা		prt_শীর্ষ_margin;
স্থির char_u		szAppName[] = TEXT("VIM");
স্থির HWND		hDlgPrint;
স্থির পূর্ণসংখ্যা		*bUserAbort = ফাঁকা;
স্থির char_u		*prt_name = ফাঁকা;

// Defines which are also in vim.rc.
# define IDC_BOX1		400
# define IDC_PRINTTEXT1		401
# define IDC_PRINTTEXT2		402
# define IDC_PROGRESS		403

    স্থির BOOL
vimSetDlgItemText(HWND hDlg, পূর্ণসংখ্যা nIDDlgItem, char_u *s)
{
    WCHAR   *wp;
    BOOL    ret;

    wp = enc_to_utf16(s, ফাঁকা);
    যদি (wp == ফাঁকা)
	ফেরত FALSE;

    ret = SetDlgItemTextW(hDlg, nIDDlgItem, wp);
    vim_free(wp);
    ফেরত ret;
}

/*
 * Convert BGR to RGB for Windows GDI calls
 */
    স্থির COLORREF
swap_me(COLORREF colorref)
{
    পূর্ণসংখ্যা  অস্থায়ী;
    অক্ষর *ptr = (অক্ষর *)&colorref;

    অস্থায়ী = *(ptr);
    *(ptr ) = *(ptr + 2);
    *(ptr + 2) = অস্থায়ী;
    ফেরত colorref;
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

    নির্বাচন (message)
    {
	ক্ষেত্র WM_INITDIALOG:
# ifdef FEAT_GETTEXT
	    nm.cb_আকার = আকার(NONCLIENTMETRICS);
	    যদি (SystemParametersInfo(
			SPI_GETNONCLIENTMETRICS,
			আকার(NONCLIENTMETRICS),
			&nm,
			0))
	    {
		অক্ষর buff[MAX_PATH];
		পূর্ণসংখ্যা i;

		// Translate the dialog texts
		hfont = CreateFontIndirect(&nm.lfMessageFont);
		জন্য (i = IDC_PRINTTEXT1; i <= IDC_PROGRESS; i++)
		{
		    SendDlgItemMessage(hDlg, i, WM_SETFONT, (WPARAM)hfont, 1);
		    যদি (GetDlgItemText(hDlg,i, buff, আকার(buff)))
			vimSetDlgItemText(hDlg,i, (char_u *)_(buff));
		}
		SendDlgItemMessage(hDlg, IDCANCEL,
						WM_SETFONT, (WPARAM)hfont, 1);
		যদি (GetDlgItemText(hDlg,IDCANCEL, buff, আকার(buff)))
		    vimSetDlgItemText(hDlg,IDCANCEL, (char_u *)_(buff));
	    }
# endif
	    SetWindowText(hDlg, (LPCSTR)szAppName);
	    যদি (prt_name != ফাঁকা)
	    {
		vimSetDlgItemText(hDlg, IDC_PRINTTEXT2, (char_u *)prt_name);
		VIM_CLEAR(prt_name);
	    }
	    EnableMenuItem(GetSystemMenu(hDlg, FALSE), SC_CLOSE, MF_GRAYED);
# যদি !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	    যদি (!gui.in_use)
#  endif
		Bring_Window_To_শীর্ষ(s_hwnd);
# endif
	    ফেরত TRUE;

	ক্ষেত্র WM_COMMAND:
	    *bUserAbort = TRUE;
	    EnableWindow(GetParent(hDlg), TRUE);
	    DestroyWindow(hDlg);
	    hDlgPrint = ফাঁকা;
# ifdef FEAT_GETTEXT
	    DeleteObject(hfont);
# endif
	    ফেরত TRUE;
    }
    ফেরত FALSE;
}

    স্থির BOOL CALLBACK
AbortProc(HDC hdcPrn UNUSED, পূর্ণসংখ্যা iCode UNUSED)
{
    বার্তা_2 বার্তা;

    যখন (!*bUserAbort && PeekMessageW(&বার্তা, ফাঁকা, 0, 0, PM_REMOVE))
    {
	যদি (!hDlgPrint || !IsDialogMessageW(hDlgPrint, &বার্তা))
	{
	    TranslateMessage(&বার্তা);
	    DispatchMessageW(&বার্তা);
	}
    }
    ফেরত !*bUserAbort;
}

    স্থির UINT_PTR CALLBACK
PrintHookProc(
	HWND hDlg,	// handle to dialog box
	UINT ui_বার্তা,	// message identifier
	WPARAM wParam UNUSED,	// message parameter
	LPARAM lParam	// message parameter
	)
{
    HWND	hwndOwner;
    RECT	rc, rcDlg, rcOwner;
    PRINTDLGW	*pPD;

    যদি (ui_বার্তা != WM_INITDIALOG)
	ফেরত FALSE;

    // Get the owner window and dialog box rectangles.
    যদি ((hwndOwner = GetParent(hDlg)) == ফাঁকা)
	hwndOwner = GetDesktopWindow();

    GetWindowRect(hwndOwner, &rcOwner);
    GetWindowRect(hDlg, &rcDlg);
    CopyRect(&rc, &rcOwner);

    // Offset the owner and dialog box rectangles so that
    // right and bottom values represent the width and
    // height, and then offset the owner again to discard
    // space taken up by the dialog box.

    OffsetRect(&rcDlg, -rcDlg.বাম, -rcDlg.শীর্ষ_2);
    OffsetRect(&rc, -rc.বাম, -rc.শীর্ষ_2);
    OffsetRect(&rc, -rcDlg.ডান, -rcDlg.bottom);

    // The new position is the sum of half the remaining
    // space and the owner's original position.

    SetWindowPos(hDlg,
	    HWND_শীর্ষ,
	    rcOwner.বাম + (rc.ডান / 2),
	    rcOwner.শীর্ষ_2 + (rc.bottom / 2),
	    0, 0,		// ignores size arguments
	    SWP_NOSIZE);

    //  tackle the printdlg copiesctrl problem
    pPD = (PRINTDLGW *)lParam;
    pPD->nCopies = (WORD)pPD->l_Cust_তথ্য;
    SetDlgItemInt( hDlg, edt3, pPD->nCopies, FALSE );
    //  Bring the window to top
    Bring_Window_To_শীর্ষ(GetParent(hDlg));
    SetForegroundWindow(hDlg);

    ফেরত FALSE;
}

    শূন্য
mch_print_cleanup(শূন্য)
{
    পূর্ণসংখ্যা pifItalic;
    পূর্ণসংখ্যা pifBold;
    পূর্ণসংখ্যা pifUnderline;

    জন্য (pifBold = 0; pifBold <= 1; pifBold++)
	জন্য (pifItalic = 0; pifItalic <= 1; pifItalic++)
	    জন্য (pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
		DeleteObject(prt_font_handles[pifBold][pifItalic][pifUnderline]);

    যদি (prt_dlg.hDC != ফাঁকা)
	DeleteDC(prt_dlg.hDC);
    যদি (!*bUserAbort && hDlgPrint != ফাঁকা)
	SendMessage(hDlgPrint, WM_COMMAND, 0, 0);
}

    স্থির পূর্ণসংখ্যা
to_device_units(পূর্ণসংখ্যা idx, পূর্ণসংখ্যা dpi, পূর্ণসংখ্যা physsize, পূর্ণসংখ্যা offset, পূর্ণসংখ্যা def_সংখ্যা)
{
    পূর্ণসংখ্যা		ret = 0;
    পূর্ণসংখ্যা		u;
    পূর্ণসংখ্যা		nr;

    u = prt_get_unit(idx);
    যদি (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_সংখ্যা;
    }
    অন্যথা
	nr = printer_opts[idx].সংখ্যা;

    নির্বাচন (u)
    {
	ক্ষেত্র PRT_UNIT_PERC:
	    ret = (physsize * nr) / 100;
	    বিরতি;
	ক্ষেত্র PRT_UNIT_INCH:
	    ret = (nr * dpi);
	    বিরতি;
	ক্ষেত্র PRT_UNIT_MM:
	    ret = (nr * 10 * dpi) / 254;
	    বিরতি;
	ক্ষেত্র PRT_UNIT_POINT:
	    ret = (nr * 10 * dpi) / 720;
	    বিরতি;
    }

    যদি (ret < offset)
	ফেরত 0;
    অন্যথা
	ফেরত ret - offset;
}

    স্থির পূর্ণসংখ্যা
prt_get_cpl(শূন্য)
{
    পূর্ণসংখ্যা		hr;
    পূর্ণসংখ্যা		phyw;
    পূর্ণসংখ্যা		dvoff;
    পূর্ণসংখ্যা		rev_offset;
    পূর্ণসংখ্যা		dpi;

    GetTextMetrics(prt_dlg.hDC, &prt_tm);
    prt_সারি_height = prt_tm.tmHeight + prt_tm.tmExternalLeading;

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

    ফেরত (prt_ডান_margin - prt_বাম_margin) / prt_tm.tmAveCharWidth;
}

    স্থির পূর্ণসংখ্যা
prt_get_lpp(শূন্য)
{
    পূর্ণসংখ্যা vr;
    পূর্ণসংখ্যা phyw;
    পূর্ণসংখ্যা dvoff;
    পূর্ণসংখ্যা rev_offset;
    পূর্ণসংখ্যা	bottom_margin;
    পূর্ণসংখ্যা	dpi;

    vr	    = GetDeviceCaps(prt_dlg.hDC, VERTRES);
    phyw    = GetDeviceCaps(prt_dlg.hDC, PHYSICALHEIGHT);
    dvoff   = GetDeviceCaps(prt_dlg.hDC, PHYSICALOFFSETY);
    dpi	    = GetDeviceCaps(prt_dlg.hDC, LOGPIXELSY);

    rev_offset = phyw - (dvoff + vr);

    prt_শীর্ষ_margin = to_device_units(OPT_PRINT_শীর্ষ, dpi, phyw, dvoff, 5);

    // adjust top margin if there is a header
    prt_শীর্ষ_margin += prt_সারি_height * prt_header_height();

    bottom_margin = vr - to_device_units(OPT_PRINT_BOT, dpi, phyw,
							       rev_offset, 5);

    ফেরত (bottom_margin - prt_শীর্ষ_margin) / prt_সারি_height;
}

    পূর্ণসংখ্যা
mch_print_init(prt_settings_T *psettings, char_u *jobname, পূর্ণসংখ্যা forceit)
{
    স্থির HGLOBAL	stored_dm = ফাঁকা;
    স্থির HGLOBAL	stored_devn = ফাঁকা;
    স্থির পূর্ণসংখ্যা		stored_nCopies = 1;
    স্থির পূর্ণসংখ্যা		stored_nFlags = 0;

    LOGFONTW		fLogFont;
    পূর্ণসংখ্যা			pifItalic;
    পূর্ণসংখ্যা			pifBold;
    পূর্ণসংখ্যা			pifUnderline;

    DEVMODEW		*mem;
    DEVNAMES		*devname;
    পূর্ণসংখ্যা			i;
    DWORD		err;

    bUserAbort = &(psettings->user_abort);
    CLEAR_FIELD(prt_dlg);
    prt_dlg.l_Struct_আকার = আকার(PRINTDLGW);
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
	prt_dlg.l_Cust_তথ্য = stored_nCopies; // work around bug in print dialog
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
	    prt_dlg.hDC = CreateDC(ফাঁকা, (LPCSTR)p_pdev, ফাঁকা, ফাঁকা);
	অন্যথা
	{
	    prt_dlg.Flags |= PD_RETURNDEFAULT;
	    যদি (PrintDlgW(&prt_dlg) == 0)
		যাও init_fail_dlg;
	}
    }
    অন্যথা যদি (PrintDlgW(&prt_dlg) == 0)
	যাও init_fail_dlg;
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

    যদি (prt_dlg.hDC == ফাঁকা)
    {
	emsg(_(e_printer_selection_failed));
	mch_print_cleanup();
	ফেরত FALSE;
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
    যদি (mem != ফাঁকা)
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
	char_u  *printer_name = utf16_to_enc(wprinter_name, ফাঁকা);
	char_u	*port_name = utf16_to_enc(wport_name, ফাঁকা);

	যদি (printer_name != ফাঁকা && port_name != ফাঁকা)
	    prt_name = alloc(STRLEN(printer_name)
					   + STRLEN(port_name) + STRLEN(text));
	যদি (prt_name != ফাঁকা)
	    wsprintf((অক্ষর *)prt_name, (ধ্রুবক অক্ষর *)text,
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
	ফেরত FALSE;
    }

    জন্য (pifBold = 0; pifBold <= 1; pifBold++)
	জন্য (pifItalic = 0; pifItalic <= 1; pifItalic++)
	    জন্য (pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
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
    psettings->chars_per_সারি = prt_get_cpl();
    psettings->সারিগুলি_per_page = prt_get_lpp();
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

    ফেরত TRUE;

init_fail_dlg:
    err = CommDlgExtendedError();
    যদি (err)
    {
	char_u *buf;

	// I suspect FormatMessage() doesn't work for values returned by
	// CommDlgExtendedError().  What does?
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_বাফার |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		ফাঁকা, err, 0, (LPTSTR)(&buf), 0, ফাঁকা);
	semsg(_(e_print_error_str),
		buf == ফাঁকা ? (char_u *)_("Unknown") : buf);
	LocalFree((LPVOID)(buf));
    }
    অন্যথা
	বার্তা_clr_eos(); // Maybe canceled

    mch_print_cleanup();
    ফেরত FALSE;
}


    পূর্ণসংখ্যা
mch_print_begin(prt_settings_T *psettings)
{
    পূর্ণসংখ্যা			ret = 0;
    অক্ষর		sz_বাফার[300];
    WCHAR		*wp;

    hDlgPrint = CreateDialog(g_hinst, TEXT("PrintDlgBox"),
					     prt_dlg.hwndOwner, PrintDlgProc);
    SetAbortProc(prt_dlg.hDC, AbortProc);
    wsprintf(sz_বাফার, _("Printing '%s'"), gettail(psettings->jobname));
    vimSetDlgItemText(hDlgPrint, IDC_PRINTTEXT1, (char_u *)sz_বাফার);

    wp = enc_to_utf16(psettings->jobname, ফাঁকা);
    যদি (wp != ফাঁকা)
    {
	DOCINFOW	di;

	CLEAR_FIELD(di);
	di.cb_আকার = আকার(di);
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

    ফেরত (ret > 0);
}

    শূন্য
mch_print_end(prt_settings_T *psettings UNUSED)
{
    EndDoc(prt_dlg.hDC);
    যদি (!*bUserAbort && hDlgPrint != ফাঁকা)
	SendMessage(hDlgPrint, WM_COMMAND, 0, 0);
}

    পূর্ণসংখ্যা
mch_print_end_page(শূন্য)
{
    ফেরত (EndPage(prt_dlg.hDC) > 0);
}

    পূর্ণসংখ্যা
mch_print_begin_page(char_u *বার্তা)
{
    যদি (বার্তা != ফাঁকা)
	vimSetDlgItemText(hDlgPrint, IDC_PROGRESS, বার্তা);
    ফেরত (StartPage(prt_dlg.hDC) > 0);
}

    পূর্ণসংখ্যা
mch_print_blank_page(শূন্য)
{
    ফেরত (mch_print_begin_page(ফাঁকা) ? (mch_print_end_page()) : FALSE);
}

স্থির পূর্ণসংখ্যা prt_pos_x = 0;
স্থির পূর্ণসংখ্যা prt_pos_y = 0;

    শূন্য
mch_print_start_সারি(পূর্ণসংখ্যা margin, পূর্ণসংখ্যা page_সারি)
{
    যদি (margin)
	prt_pos_x = -prt_সংখ্যা_width;
    অন্যথা
	prt_pos_x = 0;
    prt_pos_y = page_সারি * prt_সারি_height
				 + prt_tm.tmAscent + prt_tm.tmExternalLeading;
}

    পূর্ণসংখ্যা
mch_print_text_out(char_u *p, পূর্ণসংখ্যা len)
{
    আকার_4	sz;
    WCHAR	*wp;
    পূর্ণসংখ্যা		wlen = len;
    পূর্ণসংখ্যা		ret = FALSE;

    wp = enc_to_utf16(p, &wlen);
    যদি (wp == ফাঁকা)
	ফেরত FALSE;

    TextOutW(prt_dlg.hDC, prt_pos_x + prt_বাম_margin,
	    prt_pos_y + prt_শীর্ষ_margin, wp, wlen);
    GetTextExtentPoint32W(prt_dlg.hDC, wp, wlen, &sz);
    vim_free(wp);
    prt_pos_x += (sz.cx - prt_tm.tmOverhang);
    // This is wrong when printing spaces for a TAB.
    যদি (p[len] != NUL)
    {
	wlen = mb_ptr2len(p + len);
	wp = enc_to_utf16(p + len, &wlen);
	যদি (wp != ফাঁকা)
	{
	    GetTextExtentPoint32W(prt_dlg.hDC, wp, 1, &sz);
	    ret = (prt_pos_x + prt_বাম_margin + sz.cx > prt_ডান_margin);
	    vim_free(wp);
	}
    }
    ফেরত ret;
}

    শূন্য
mch_print_set_font(পূর্ণসংখ্যা iBold, পূর্ণসংখ্যা iItalic, পূর্ণসংখ্যা iUnderline)
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

    স্থির পূর্ণসংখ্যা
is_reparse_point_included(LPCWSTR fname)
{
    LPCWSTR	p = fname, q;
    WCHAR	buf[MAX_PATH];
    DWORD	attr;

    যদি (SAFE_isalpha(p[0]) && p[1] == L':' && is_path_sep(p[2]))
	p += 3;
    অন্যথা যদি (is_path_sep(p[0]) && is_path_sep(p[1]))
	p += 2;

    যখন (*p != L'\0')
    {
	q = wcspbrk(p, L"\\/");
	যদি (q == ফাঁকা)
	    p = q = fname + wcslen(fname);
	অন্যথা
	    p = q + 1;
	যদি (q - fname >= MAX_PATH)
	    ফেরত FALSE;
	wcsncpy(buf, fname, q - fname);
	buf[q - fname] = L'\0';
	attr = GetFileAttributesW(buf);
	যদি (attr != INVALID_FILE_ATTRIBUTES
		&& (attr & FILE_ATTRIBUTE_REPARSE_POINT) != 0)
	    ফেরত TRUE;
    }
    ফেরত FALSE;
}

/*
 * Return the resolved file path, NULL if "fname" is an AppExecLink reparse
 * point, already fully resolved, or it doesn't exists.
 */
    char_u *
resolve_reparse_point(char_u *fname)
{
    HANDLE	    h = INVALID_HANDLE_মান;
    DWORD	    আকার_2;
    WCHAR	    *p, *wp;
    char_u	    *rfname = ফাঁকা;
    WCHAR	    *buff = ফাঁকা;

    p = enc_to_utf16(fname, ফাঁকা);
    যদি (p == ফাঁকা)
	যাও fail;

    যদি (!is_reparse_point_included(p))
    {
	vim_free(p);
	যাও fail;
    }

    h = CreateFileW(p, 0, 0, ফাঁকা, OPEN_EXISTING,
	    FILE_FLAG_BACKUP_SEMANTICS, ফাঁকা);
    vim_free(p);

    যদি (h == INVALID_HANDLE_মান)
	যাও fail;

    আকার_2 = GetFinalPathNameByHandleW(h, ফাঁকা, 0, 0);
    যদি (আকার_2 == 0)
	যাও fail;
    buff = ALLOC_MULT(WCHAR, আকার_2);
    যদি (buff == ফাঁকা)
	যাও fail;
    যদি (GetFinalPathNameByHandleW(h, buff, আকার_2, 0) == 0)
	যাও fail;

    যদি (wcsncmp(buff, L"\\\\?\\UNC\\", 8) == 0)
    {
	buff[6] = L'\\';
	wp = buff + 6;
    }
    অন্যথা যদি (wcsncmp(buff, L"\\\\?\\", 4) == 0)
	wp = buff + 4;
    অন্যথা
	wp = buff;

    rfname = utf16_to_enc(wp, ফাঁকা);

fail:
    যদি (h != INVALID_HANDLE_মান)
	CloseHandle(h);
    যদি (buff != ফাঁকা)
	vim_free(buff);

    ফেরত rfname;
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
    IShellLink		*psl = ফাঁকা;
    IPersistFile	*ppf = ফাঁকা;
    OLECHAR		wsz[MAX_PATH];
    char_u		*rfname = ফাঁকা;
    পূর্ণসংখ্যা			len;
    IShellLinkW		*pslw = ফাঁকা;
    WIN32_FIND_DATAW	ffdw; // we get those free of charge

    // Check if the file name ends in ".lnk". Avoid calling
    // CoCreateInstance(), it's quite slow.
    যদি (fname == ফাঁকা)
	ফেরত rfname;
    len = (পূর্ণসংখ্যা)STRLEN(fname);
    যদি (len <= 4 || STRNICMP(fname + len - 4, ".lnk", 4) != 0)
	ফেরত rfname;

    CoInitialize(ফাঁকা);

    // create a link manager object and request its interface
    hr = CoCreateInstance(
	    &CLSID_ShellLink, ফাঁকা, CLSCTX_INPROC_SERVER,
	    &IID_IShellLinkW, (শূন্য**)&pslw);
    যদি (hr == S_OK)
    {
	WCHAR	*p = enc_to_utf16(fname, ফাঁকা);

	যদি (p != ফাঁকা)
	{
	    // Get a pointer to the IPersistFile interface.
	    hr = pslw->lpVtbl->QueryInterface(
		    pslw, &IID_IPersistFile, (শূন্য**)&ppf);
	    যদি (hr != S_OK)
		যাও shortcut_errorw;

	    // "load" the name and resolve the link
	    hr = ppf->lpVtbl->Load(ppf, p, STGM_READ);
	    যদি (hr != S_OK)
		যাও shortcut_errorw;
# যদি 0  // This makes Vim wait a long time if the target does not exist.
	    hr = pslw->lpVtbl->Resolve(pslw, ফাঁকা, SLR_NO_UI);
	    যদি (hr != S_OK)
		যাও shortcut_errorw;
# endif

	    // Get the path to the link target.
	    ZeroMemory(wsz, MAX_PATH * আকার(WCHAR));
	    hr = pslw->lpVtbl->GetPath(pslw, wsz, MAX_PATH, &ffdw, 0);
	    যদি (hr == S_OK && wsz[0] != NUL)
		rfname = utf16_to_enc(wsz, ফাঁকা);

shortcut_errorw:
	    vim_free(p);
	}
    }

    // Release all interface pointers (both belong to the same object)
    যদি (ppf != ফাঁকা)
	ppf->lpVtbl->Release(ppf);
    যদি (psl != ফাঁকা)
	psl->lpVtbl->Release(psl);
    যদি (pslw != ফাঁকা)
	pslw->lpVtbl->Release(pslw);

    CoUninitialize();
    ফেরত rfname;
}

    char_u *
mch_resolve_path(char_u *fname, পূর্ণসংখ্যা reparse_point)
{
    char_u  *path = resolve_shortcut(fname);

    যদি (path == ফাঁকা && reparse_point)
	path = resolve_reparse_point(fname);
    ফেরত path;
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
# define VIM_CLASSNAME_LEN  (আকার(VIM_CLASSNAME) - 1)

// Timeout for sending a message to another Vim instance.  Normally this works
// instantly, but it may hang when the other Vim instance is halted.
# define SENDMESSAGE_TIMEOUT	(5 * 1000)

// Communication is via WM_COPYDATA messages. The message type is sent in
// the dwData parameter. Types are defined here.
# define COPYDATA_চাবিগুলি		0
# define COPYDATA_REPLY		1
# define COPYDATA_রাশিমালা		10
# define COPYDATA_ফলাফল	11
# define COPYDATA_ERROR_ফলাফল	12
# define COPYDATA_ENCODING	20

// This is a structure containing a server HWND and its name.
কাঠামো server_id
{
    HWND hwnd;
    char_u *name;
};

// Last received 'encoding' that the client uses.
স্থির char_u	*client_enc = ফাঁকা;

/*
 * Tell the other side what encoding we are using.
 * Return -1 if timeout happens.  Other errors are ignored.
 */
    স্থির পূর্ণসংখ্যা
serverSendEnc(HWND target)
{
    COPYDATASTRUCT তথ্য;

    তথ্য.dw_তথ্য = COPYDATA_ENCODING;
    তথ্য.cb_তথ্য = (DWORD)STRLEN(p_enc) + 1;
    তথ্য.lp_তথ্য = p_enc;
    যদি (SendMessageTimeout(target, WM_COPYDATA,
	    (WPARAM)message_window, (LPARAM)&তথ্য,
	    SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, ফাঁকা) == 0)
	ফেরত -1;
    ফেরত 0;
}

/*
 * Clean up on exit. This destroys the hidden message window.
 */
    স্থির শূন্য
CleanUpMessaging(শূন্য)
{
    যদি (message_window == 0)
	ফেরত;

    DestroyWindow(message_window);
    message_window = 0;
}

স্থির পূর্ণসংখ্যা save_reply(HWND server, char_u *reply, পূর্ণসংখ্যা রাশিমালা);

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
Messaging_WndProc(HWND hwnd, UINT বার্তা, WPARAM wParam, LPARAM lParam)
{
    যদি (বার্তা == WM_COPYDATA)
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
	COPYDATASTRUCT	*তথ্য = (COPYDATASTRUCT*)lParam;
	HWND		sender = (HWND)wParam;
	COPYDATASTRUCT	reply;
	char_u		*res;
	পূর্ণসংখ্যা		retval;
	DWORD_PTR	dwret = 0;
	char_u		*str;
	char_u		*tofree;

	নির্বাচন (তথ্য->dw_তথ্য)
	{
	ক্ষেত্র COPYDATA_ENCODING:
	    // Remember the encoding that the client uses.
	    vim_free(client_enc);
	    client_enc = enc_canonize((char_u *)তথ্য->lp_তথ্য);
	    ফেরত 1;

	ক্ষেত্র COPYDATA_চাবিগুলি:
	    // Remember who sent this, for <client>
	    clientWindow = sender;

	    // Add the received keys to the input buffer.
	    str = serverConvert(client_enc, (char_u *)তথ্য->lp_তথ্য, &tofree);
# ifdef FEAT_GUI
	    যদি (gui.in_use)
		// The GUI may get here re-entrantly, so insert the keys later.
		server_add_প্রবেশ(str);
	    অন্যথা
# endif
		server_to_প্রবেশ_buf(str);
	    vim_free(tofree);

# ifdef FEAT_GUI
	    // Wake up the main GUI loop.
#  ifdef VIMDLL
	    যদি (gui.in_use)
#  endif
		যদি (s_hwnd != 0)
		    PostMessage(s_hwnd, WM_NULL, 0, 0);
# endif
	    ফেরত 1;

	ক্ষেত্র COPYDATA_রাশিমালা:
	    // Remember who sent this, for <client>
	    clientWindow = sender;

	    str = serverConvert(client_enc, (char_u *)তথ্য->lp_তথ্য, &tofree);
	    res = eval_client_রাশিমালা_to_string(str);

	    যদি (res == ফাঁকা)
	    {
		অক্ষর	*err = _(e_invalid_expression_received);
		size_t	len = STRLEN(str) + STRLEN(err) + 5;

		res = alloc(len);
		যদি (res != ফাঁকা)
		    reply.cb_তথ্য = (DWORD)vim_snprintf_safelen(
			(অক্ষর *)res, len, "%s: \"%s\"", err, str);
		অন্যথা
		    reply.cb_তথ্য = 0;
		reply.dw_তথ্য = COPYDATA_ERROR_ফলাফল;
	    }
	    অন্যথা
	    {
		reply.cb_তথ্য = (DWORD)STRLEN(res) + 1;
		reply.dw_তথ্য = COPYDATA_ফলাফল;
	    }
	    reply.lp_তথ্য = res;

	    যদি (serverSendEnc(sender) < 0)
		retval = -1;
	    অন্যথা
	    {
		যদি (SendMessageTimeout(sender, WM_COPYDATA,
			(WPARAM)message_window, (LPARAM)&reply,
			SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
		    retval = -1;
		অন্যথা
		    retval = (পূর্ণসংখ্যা)dwret;
	    }
	    vim_free(tofree);
	    vim_free(res);
	    ফেরত retval;

	ক্ষেত্র COPYDATA_REPLY:
	ক্ষেত্র COPYDATA_ফলাফল:
	ক্ষেত্র COPYDATA_ERROR_ফলাফল:
	    যদি (তথ্য->lp_তথ্য != ফাঁকা)
	    {
		str = serverConvert(client_enc, (char_u *)তথ্য->lp_তথ্য,
								     &tofree);
		যদি (tofree == ফাঁকা)
		    str = vim_strsave(str);
		যদি (save_reply(sender, str,
			   (তথ্য->dw_তথ্য == COPYDATA_REPLY ?  0 :
			   (তথ্য->dw_তথ্য == COPYDATA_ফলাফল ? 1 :
							      2))) == FAIL)
		    vim_free(str);
		অন্যথা যদি (তথ্য->dw_তথ্য == COPYDATA_REPLY)
		{
		    char_u	winstr[30];

		    sprintf((অক্ষর *)winstr, PRINTF_HEX_LONG_U, (long_u)sender);
		    apply_autocmds(EVENT_REMOTEREPLY, winstr, str,
								TRUE, curbuf);
		}
	    }
	    ফেরত 1;
	}

	ফেরত 0;
    }

    অন্যথা যদি (বার্তা == WM_ACTIVATE && wParam == WA_ACTIVE)
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
	    ফেরত 0;
	}
    }

    ফেরত DefWindowProc(hwnd, বার্তা, wParam, lParam);
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
    wndclass.hIcon = ফাঁকা;
    wndclass.hCursor = ফাঁকা;
    wndclass.hbrBackground = ফাঁকা;
    wndclass.lpszMenuName = ফাঁকা;
    wndclass.lpszClassName = VIM_CLASSNAME;
    RegisterClass(&wndclass);

    // Create the message window. It will be hidden, so the details don't
    // matter.  Don't use WS_OVERLAPPEDWINDOW, it will make a shortcut remove
    // focus from gvim.
    message_window = CreateWindow(VIM_CLASSNAME, "",
			 WS_POPUPWINDOW | WS_CAPTION,
			 CW_USEDEFAULT, CW_USEDEFAULT,
			 100, 100, ফাঁকা, ফাঁকা,
			 g_hinst, ফাঁকা);
}

// Used by serverSendToVim() to find an alternate server name.
স্থির char_u *altname_buf_ptr = ফাঁকা;

/*
 * Get the title of the window "hwnd", which is the Vim server name, in
 * "name[namelen]" and return the length.
 * Returns zero if window "hwnd" is not a Vim server.
 */
    স্থির পূর্ণসংখ্যা
getVimServerName(HWND hwnd, অক্ষর *name, পূর্ণসংখ্যা namelen)
{
    পূর্ণসংখ্যা		len;
    অক্ষর	বাফার[VIM_CLASSNAME_LEN + 1];

    // Ignore windows which aren't Vim message windows
    len = GetClassName(hwnd, বাফার, আকার(বাফার));
    যদি (len != VIM_CLASSNAME_LEN || STRCMP(বাফার, VIM_CLASSNAME) != 0)
	ফেরত 0;

    // Get the title of the window
    ফেরত GetWindowText(hwnd, name, namelen);
}

    স্থির BOOL CALLBACK
enumWindowsGetServer(HWND hwnd, LPARAM lparam)
{
    কাঠামো	server_id *id = (কাঠামো server_id *)lparam;
    অক্ষর	server[MAX_PATH];

    // Get the title of the window
    যদি (getVimServerName(hwnd, server, আকার(server)) == 0)
	ফেরত TRUE;

    // If this is the server we're looking for, return its HWND
    যদি (STRICMP(server, id->name) == 0)
    {
	id->hwnd = hwnd;
	ফেরত FALSE;
    }

    // If we are looking for an alternate server, remember this name.
    যদি (altname_buf_ptr != ফাঁকা)
    {
	size_t	namelen = STRLEN(id->name);

	যদি (STRNICMP(server, id->name, namelen) == 0
	    && vim_isdigit(server[namelen]))
	{
	    STRCPY(altname_buf_ptr, server);
	    altname_buf_ptr = ফাঁকা;	    // don't use another name
	}
    }

    // Otherwise, keep looking
    ফেরত TRUE;
}

    স্থির BOOL CALLBACK
enumWindowsGetNames(HWND hwnd, LPARAM lparam)
{
    list_T	*list = (list_T *)lparam;
    অক্ষর	server[MAX_PATH];

    // Get the title of the window
    যদি (getVimServerName(hwnd, server, আকার(server)) == 0)
	ফেরত TRUE;

    list_append_string(list, (char_u *)server, -1);
    ফেরত TRUE;
}

কাঠামো enum_windows_s
{
    WNDENUMPROC lpEnumFunc;
    LPARAM      lParam;
};

    স্থির BOOL CALLBACK
enum_windows_child(HWND hwnd, LPARAM lParam)
{
    কাঠামো enum_windows_s *ew = (কাঠামো enum_windows_s *)lParam;

    ফেরত (ew->lpEnumFunc)(hwnd, ew->lParam);
}

    স্থির BOOL CALLBACK
enum_windows_toplevel(HWND hwnd, LPARAM lParam)
{
    কাঠামো enum_windows_s *ew = (কাঠামো enum_windows_s *)lParam;

    যদি ((ew->lpEnumFunc)(hwnd, ew->lParam))
	ফেরত TRUE;
    ফেরত EnumChildWindows(hwnd, enum_windows_child, lParam);
}

/*
 * Enumerate all windows including children.
 */
    স্থির BOOL
enum_windows(WNDENUMPROC lpEnumFunc, LPARAM lParam)
{
    কাঠামো enum_windows_s ew;

    ew.lpEnumFunc = lpEnumFunc;
    ew.lParam = lParam;
    ফেরত EnumWindows(enum_windows_toplevel, (LPARAM)&ew);
}

    স্থির HWND
findServer(char_u *name)
{
    কাঠামো server_id id;

    id.name = name;
    id.hwnd = 0;

    enum_windows(enumWindowsGetServer, (LPARAM)(&id));

    ফেরত id.hwnd;
}

    শূন্য
serverSetName(char_u *name)
{
    size_t	namelen;
    char_u	*ok_name;
    HWND	hwnd = 0;
    পূর্ণসংখ্যা		i = 0;
    char_u	*p;

    // Leave enough space for a 9-digit suffix to ensure uniqueness!
    namelen = STRLEN(name);
    ok_name = alloc(namelen + 10);
    যদি (ok_name == ফাঁকা)
	ফেরত;

    STRCPY(ok_name, name);
    p = ok_name + namelen;

    জন্য (;;)
    {
	// This is inefficient - we're doing an EnumWindows loop for each
	// possible name. It would be better to grab all names in one go,
	// and scan the list each time...
	hwnd = findServer(ok_name);
	যদি (hwnd == 0)
	    বিরতি;

	++i;
	যদি (i >= 1000)
	    বিরতি;

	sprintf((অক্ষর *)p, "%d", i);
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

    যদি (list == ফাঁকা)
	ফেরত ফাঁকা;

    enum_windows(enumWindowsGetNames, (LPARAM)list);

    ফেরত list;
}

    পূর্ণসংখ্যা
serverSendReply(
    char_u	*name,		// Where to send.
    char_u	*reply)		// What to send.
{
    HWND	target;
    COPYDATASTRUCT তথ্য;
    long_u	n = 0;
    DWORD_PTR	dwret = 0;

    // The "name" argument is a magic cookie obtained from expand("<client>").
    // It should be of the form 0xXXXXX - i.e. a C hex literal, which is the
    // value of the client's message window HWND.
    sscanf((অক্ষর *)name, SCANF_HEX_LONG_U, &n);
    যদি (n == 0)
	ফেরত -1;

    target = (HWND)n;
    যদি (!IsWindow(target))
	ফেরত -1;

    তথ্য.dw_তথ্য = COPYDATA_REPLY;
    তথ্য.cb_তথ্য = (DWORD)STRLEN(reply) + 1;
    তথ্য.lp_তথ্য = reply;

    যদি (serverSendEnc(target) < 0)
	ফেরত -1;
    যদি (SendMessageTimeout(target, WM_COPYDATA,
		(WPARAM)message_window, (LPARAM)&তথ্য,
		SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
	ফেরত -1;
    ফেরত dwret ? 0 : -1;
}

    পূর্ণসংখ্যা
serverSendToVim(
    char_u	 *name,			// Where to send.
    char_u	 *cmd,			// What to send.
    char_u	 **ফলাফল,		// Result of eval'ed expression
    শূন্য	 *ptarget,		// HWND of server
    পূর্ণসংখ্যা		 as_রাশিমালা,		// Expression or keys?
    পূর্ণসংখ্যা		 timeout,		// timeout in seconds or zero
    পূর্ণসংখ্যা		 silent)		// don't complain about no server
{
    size_t	namelen;
    HWND	target;
    COPYDATASTRUCT তথ্য;
    char_u	*retval = ফাঁকা;
    পূর্ণসংখ্যা		retcode = 0;
    DWORD_PTR	dwret = 0;
    char_u	altname_buf[MAX_PATH];

    // Execute locally if no display or target is ourselves
    যদি (serverName != ফাঁকা && STRICMP(name, serverName) == 0)
	ফেরত sendToLocalVim(cmd, as_রাশিমালা, ফলাফল);

    // If the server name does not end in a digit then we look for an
    // alternate name.  e.g. when "name" is GVIM then we may find GVIM2.
    namelen = STRLEN(name);
    যদি (namelen > 1 && !vim_isdigit(name[namelen - 1]))
	altname_buf_ptr = altname_buf;
    altname_buf[0] = NUL;
    target = findServer(name);
    altname_buf_ptr = ফাঁকা;
    যদি (target == 0 && altname_buf[0] != NUL)
	// Use another server name we found.
	target = findServer(altname_buf);

    যদি (target == 0)
    {
	যদি (!silent)
	    semsg(_(e_no_registered_server_named_str), name);
	ফেরত -1;
    }

    যদি (ptarget)
	*(HWND *)ptarget = target;

    তথ্য.dw_তথ্য = as_রাশিমালা ? COPYDATA_রাশিমালা : COPYDATA_চাবিগুলি;
    তথ্য.cb_তথ্য = (DWORD)STRLEN(cmd) + 1;
    তথ্য.lp_তথ্য = cmd;

    যদি (serverSendEnc(target) < 0)
	ফেরত -1;
    যদি (SendMessageTimeout(target, WM_COPYDATA,
		(WPARAM)message_window, (LPARAM)&তথ্য,
		SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
	ফেরত -1;
    যদি (dwret == 0)
	ফেরত -1;

    যদি (as_রাশিমালা)
	retval = serverGetReply(target, &retcode, TRUE, TRUE, timeout);

    যদি (ফলাফল == ফাঁকা)
	vim_free(retval);
    অন্যথা
	*ফলাফল = retval; // Caller assumes responsibility for freeing

    ফেরত retcode;
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
প্রকারলেখ কাঠামো
{
    HWND	server;		// server window
    char_u	*reply;		// reply string
    পূর্ণসংখ্যা		রাশিমালা_ফলাফল;	// 0 for REPLY, 1 for RESULT 2 for error
} reply_T;

স্থির garray_T reply_list = {0, 0, আকার(reply_T), 5, 0};

# define REPLY_ITEM(i) ((reply_T *)(reply_list.ga_তথ্য) + (i))
# define REPLY_গুনতি (reply_list.ga_len)

// Flag which is used to wait for a reply
স্থির পূর্ণসংখ্যা reply_received = 0;

/*
 * Store a reply.  "reply" must be allocated memory (or NULL).
 */
    স্থির পূর্ণসংখ্যা
save_reply(HWND server, char_u *reply, পূর্ণসংখ্যা রাশিমালা)
{
    reply_T *rep;

    যদি (ga_grow(&reply_list, 1) == FAIL)
	ফেরত FAIL;

    rep = REPLY_ITEM(REPLY_গুনতি);
    rep->server = server;
    rep->reply = reply;
    rep->রাশিমালা_ফলাফল = রাশিমালা;
    যদি (rep->reply == ফাঁকা)
	ফেরত FAIL;

    ++REPLY_গুনতি;
    reply_received = 1;
    ফেরত OK;
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
serverGetReply(HWND server, পূর্ণসংখ্যা *রাশিমালা_res, পূর্ণসংখ্যা remove, পূর্ণসংখ্যা wait, পূর্ণসংখ্যা timeout)
{
    পূর্ণসংখ্যা		i;
    char_u	*reply;
    reply_T	*rep;
    পূর্ণসংখ্যা		did_process = FALSE;
    time_t	start;
    time_t	now;

    // When waiting, loop until the message waiting for is received.
    time(&start);
    জন্য (;;)
    {
	// Reset this here, in case a message arrives while we are going
	// through the already received messages.
	reply_received = 0;

	জন্য (i = 0; i < REPLY_গুনতি; ++i)
	{
	    rep = REPLY_ITEM(i);
	    যদি (rep->server == server
		    && ((rep->রাশিমালা_ফলাফল != 0) == (রাশিমালা_res != ফাঁকা)))
	    {
		// Save the values we've found for later
		reply = rep->reply;
		যদি (রাশিমালা_res != ফাঁকা)
		    *রাশিমালা_res = rep->রাশিমালা_ফলাফল == 1 ? 0 : -1;

		যদি (remove)
		{
		    // Move the rest of the list down to fill the gap
		    mch_memmove(rep, rep + 1,
				     (REPLY_গুনতি - i - 1) * আকার(reply_T));
		    --REPLY_গুনতি;
		}

		// Return the reply to the caller, who takes on responsibility
		// for freeing it if "remove" is TRUE.
		ফেরত reply;
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
		চালু;
	    }
	    বিরতি;
	}

	// We need to wait for a reply. Enter a message loop until the
	// "reply_received" flag gets set.

	// Loop until we receive a reply
	যখন (reply_received == 0)
	{
# ifdef FEAT_TIMERS
	    // TODO: use the return value to decide how long to wait.
	    check_due_timer();
# endif
	    time(&now);
	    যদি (timeout > 0 && (now - start) >= timeout)
		বিরতি;

	    // Wait for a SendMessage() call to us.  This could be the reply
	    // we are waiting for.  Use a timeout of a second, to catch the
	    // situation that the server died unexpectedly.
	    বার্তা_Wait_For_Multiple_Objects(0, ফাঁকা, TRUE, 1000, QS_ALLINPUT);

	    // If the server has died, give up
	    যদি (!IsWindow(server))
		ফেরত ফাঁকা;

	    serverProcessPendingMessages();
	}
    }

    ফেরত ফাঁকা;
}

/*
 * Process any messages in the Windows message queue.
 */
    শূন্য
serverProcessPendingMessages(শূন্য)
{
    বার্তা_2 বার্তা;

    যখন (PeekMessageW(&বার্তা, ফাঁকা, 0, 0, PM_REMOVE))
    {
	TranslateMessage(&বার্তা);
	DispatchMessageW(&বার্তা);
    }
}

#endif // FEAT_CLIENTSERVER

#যদি defined(FEAT_GUI) || (defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT))

কাঠামো charset_pair
{
    string_T	name;
    BYTE	charset;
};

স্থির কাঠামো charset_pair
charset_pairs[] =
{
    {STR_LITERAL_INIT("ANSI"),		ANSI_CHARSET},
    {STR_LITERAL_INIT("CHINESEBIG5"),	CHINESEBIG5_CHARSET},
    {STR_LITERAL_INIT("DEFAULT"),	DEFAULT_CHARSET},
    {STR_LITERAL_INIT("HANGEUL"),	HANGEUL_CHARSET},
    {STR_LITERAL_INIT("OEM"),		OEM_CHARSET},
    {STR_LITERAL_INIT("SHIFTJIS"),	SHIFTJIS_CHARSET},
    {STR_LITERAL_INIT("SYMBOL"),	প্রতীক_CHARSET},
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

কাঠামো quality_pair
{
    string_T	name;
    DWORD	quality;
};

স্থির কাঠামো quality_pair
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
    অক্ষর *
charset_id2name(পূর্ণসংখ্যা id)
{
    পূর্ণসংখ্যা	i;

    জন্য (i = 0; i < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(charset_pairs); ++i)
    {
	যদি ((BYTE)id == charset_pairs[i].charset)
	    ফেরত (অক্ষর *)charset_pairs[i].name.string;
    }

    ফেরত ফাঁকা;
}

/*
 * Convert a quality ID to a name.
 * Return NULL when not recognized.
 */
    অক্ষর *
quality_id2name(DWORD id)
{
    পূর্ণসংখ্যা	i;

    জন্য (i = 0; i < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(quality_pairs); ++i)
    {
	যদি (id == quality_pairs[i].quality)
	    ফেরত (অক্ষর *)quality_pairs[i].name.string;
    }

    ফেরত ফাঁকা;
}

// The default font height in 100% scaling (96dpi).
// (-16 in 96dpi equates to roughly 12pt)
# define DEFAULT_FONT_HEIGHT	(-16)

স্থির ধ্রুবক LOGFONTW s_lfDefault =
{
    DEFAULT_FONT_HEIGHT,
    0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
    PROOF_QUALITY, FIXED_PITCH | FF_DONTCARE,
    L""	// Default font name will be set later based on current language.
};

// This will be initialized when set_default_logfont() is called first time.
// The value will be based on the system DPI.
পূর্ণসংখ্যা current_font_height = 0;		// also used in gui_w32.c

/*
 * Convert a string representing a point size into pixels. The string should
 * be a positive decimal number, with an optional decimal point (eg, "12", or
 * "10.5"). The pixel value is returned, and a pointer to the next unconverted
 * character is stored in *end. The flag "vertical" says whether this
 * calculation is for a vertical (height) size or a horizontal (width) one.
 */
    স্থির পূর্ণসংখ্যা
points_to_pixels(WCHAR *str, WCHAR **end, পূর্ণসংখ্যা vertical, long_i pprinter_dc)
{
    পূর্ণসংখ্যা		pixels;
    পূর্ণসংখ্যা		points = 0;
    পূর্ণসংখ্যা		ভাজক = 0;
    HWND	hwnd = (HWND)0;
    HDC		hdc;
    HDC		printer_dc = (HDC)pprinter_dc;

    যখন (*str != NUL)
    {
	যদি (*str == L'.' && ভাজক == 0)
	{
	    // Start keeping a divisor, for later
	    ভাজক = 1;
	}
	অন্যথা
	{
	    যদি (!VIM_ISDIGIT(*str))
		বিরতি;

	    points *= 10;
	    points += *str - L'0';
	    ভাজক *= 10;
	}
	++str;
    }

    যদি (ভাজক == 0)
	ভাজক = 1;

    যদি (printer_dc == ফাঁকা)
    {
	hwnd = GetDesktopWindow();
	hdc = GetWindowDC(hwnd);
    }
    অন্যথা
	hdc = printer_dc;

    pixels = MulDiv(points,
		    GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX),
		    72 * ভাজক);

    যদি (printer_dc == ফাঁকা)
	ReleaseDC(hwnd, hdc);

    *end = str;
    ফেরত pixels;
}

/*
 * Convert pixel into point size. This is a reverse of points_to_pixels.
 */
    স্থির দ্বিগুণ
pixels_to_points(পূর্ণসংখ্যা pixels, পূর্ণসংখ্যা vertical, long_i pprinter_dc)
{
    দ্বিগুণ	points = 0;
    HWND	hwnd = (HWND)0;
    HDC		hdc;
    HDC		printer_dc = (HDC)pprinter_dc;

    যদি (printer_dc == ফাঁকা)
    {
	hwnd = GetDesktopWindow();
	hdc = GetWindowDC(hwnd);
    }
    অন্যথা
	hdc = printer_dc;

    points = pixels * 72.0 / GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX);
    যদি (printer_dc == ফাঁকা)
	ReleaseDC(hwnd, hdc);

    ফেরত points;
}

    স্থির পূর্ণসংখ্যা CALLBACK
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
	ফেরত 1;
# endif

    // Remember this LOGFONTW as a "possible"
    *lf = elf->elfLogFont;

    // Terminate the scan as soon as we find an ANSI font
    যদি (lf->lfCharSet == ANSI_CHARSET
	    || lf->lfCharSet == OEM_CHARSET
	    || lf->lfCharSet == DEFAULT_CHARSET)
	ফেরত 0;

    // Continue the scan - we have a non-ANSI font
    ফেরত 2;
}

    স্থির পূর্ণসংখ্যা
init_logfont(LOGFONTW *lf)
{
    পূর্ণসংখ্যা		n;
    HWND	hwnd = GetDesktopWindow();
    HDC		hdc = GetWindowDC(hwnd);

    n = EnumFontFamiliesW(hdc,
			 lf->lfFaceName,
			 (FONTENUMPROCW)font_enumproc,
			 (LPARAM)lf);

    ReleaseDC(hwnd, hdc);

    // If we couldn't find a usable font, return failure
    যদি (n == 1)
	ফেরত FAIL;

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
    ফেরত OK;
}

/*
 * Call back for EnumFontFamiliesW in expand_font_enumproc.
 *
 */
    স্থির পূর্ণসংখ্যা CALLBACK
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
	ফেরত 1;
# endif

    // Filter only on ANSI. Otherwise will see a lot of random fonts that we
    // usually don't want.
    যদি (lf->lfCharSet != ANSI_CHARSET)
	ফেরত 1;

    পূর্ণসংখ্যা (*add_match)(char_u *) = (পূর্ণসংখ্যা (*)(char_u *))lparam;

    WCHAR *faceNameW = lf->lfFaceName;
    char_u *faceName = utf16_to_enc(faceNameW, ফাঁকা);
    যদি (!faceName)
	ফেরত 0;

    add_match(faceName);
    vim_free(faceName);

    ফেরত 1;
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
    পূর্ণসংখ্যা		(*add_match)(char_u *val))
{
    expand_T	    *xp = args->oe_xp;
    যদি (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	অক্ষর buf[30];
	পূর্ণসংখ্যা	i;

	// Always fill in with the current font size as first option for
	// convenience. We simply round to the closest integer for simplicity.
	পূর্ণসংখ্যা font_height = (পূর্ণসংখ্যা)round(
		pixels_to_points(-current_font_height, TRUE, (long_i)ফাঁকা));
	vim_snprintf(buf, ARRAY_দৈর্ঘ্য(buf), "h%d", font_height);
	add_match((char_u *)buf);

	// Note: Keep this in sync with get_logfont(). Don't include 'c' and
	// 'q' as we fill in all the values below.
	স্থির অক্ষর *(p_gfn_win_opt_মানসমূহ[]) = {
	    "h" , "w" , "W" , "b" , "i" , "u" , "s"};
	জন্য (i = 0; i < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(p_gfn_win_opt_মানসমূহ); i++)
	    add_match((char_u *)p_gfn_win_opt_মানসমূহ[i]);

	জন্য (i = 0; i < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(charset_pairs); ++i)
	{
	    vim_snprintf(buf, আকার(buf), "c%s", charset_pairs[i].name.string);
	    add_match((char_u *)buf);
	}

	জন্য (i = 0; i < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(quality_pairs); ++i)
	{
	    vim_snprintf(buf, আকার(buf), "q%s", quality_pairs[i].name.string);
	    add_match((char_u *)buf);
	}
	ফেরত;
    }

    HWND	hwnd = GetDesktopWindow();
    HDC		hdc = GetWindowDC(hwnd);

    EnumFontFamiliesW(hdc,
	    ফাঁকা,
	    (FONTENUMPROCW)expand_font_enumproc,
	    (LPARAM)add_match);

    ReleaseDC(hwnd, hdc);
}

/*
 * Compare a UTF-16 string and an ASCII string literally.
 * Only works all the code points are inside ASCII range.
 */
    স্থির পূর্ণসংখ্যা
utf16ascncmp(ধ্রুবক WCHAR *w, ধ্রুবক অক্ষর *p, size_t n)
{
    size_t i;

    জন্য (i = 0; i < n; i++)
    {
	যদি (w[i] == 0 || w[i] != p[i])
	    ফেরত w[i] - p[i];
    }
    ফেরত 0;
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
    ফেরত dpi;
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
    ধ্রুবক অক্ষর *defaultfontname = N_("DefaultFontNameForWindows");
    অক্ষর *fontname = _(defaultfontname);

    যদি (STRCMP(fontname, defaultfontname) == 0)
	fontname = "Consolas";

    *lf = s_lfDefault;
    lf->lfHeight = DEFAULT_FONT_HEIGHT * (পূর্ণসংখ্যা)vimGetDpiForSystem() / 96;
    যদি (current_font_height == 0)
	current_font_height = lf->lfHeight;

    WCHAR *wfontname = enc_to_utf16((char_u*)fontname, ফাঁকা);
    যদি (wfontname != ফাঁকা)
    {
	wcscpy_s(lf->lfFaceName, LF_FACESIZE, wfontname);
	vim_free(wfontname);
    }
}

/*
 * Get font info from "name" into logfont "lf".
 * Return OK for a valid name, FAIL otherwise.
 */
    পূর্ণসংখ্যা
get_logfont(
    LOGFONTW	*lf,
    char_u	*name,
    HDC		printer_dc,
    পূর্ণসংখ্যা		verbose)
{
    WCHAR	*p;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		ret = FAIL;
    স্থির LOGFONTW *lastlf = ফাঁকা;
    WCHAR	*wname;

    set_default_logfont(lf);
    যদি (name == ফাঁকা)
	ফেরত OK;

    wname = enc_to_utf16(name, ফাঁকা);
    যদি (wname == ফাঁকা)
	ফেরত FAIL;

    যদি (wcscmp(wname, L"*") == 0)
    {
# যদি defined(FEAT_GUI_MSWIN)
	CHOOSEFONTW	cf;
	// if name is "*", bring up std font dialog:
	CLEAR_FIELD(cf);
	cf.l_Struct_আকার = আকার(cf);
	cf.hwndOwner = s_hwnd;
	cf.Flags = CF_SCREENFONTS | CF_FIXEDPITCHONLY | CF_INITTOLOGFONTSTRUCT;
	যদি (lastlf != ফাঁকা)
	    *lf = *lastlf;
	cf.lpLogFont = lf;
	cf.nFontType = 0 ; //REGULAR_FONTTYPE;
	যদি (ChooseFontW(&cf))
	    ret = OK;
# endif
	যাও theend;
    }

    /*
     * Split name up, it could be <name>:h<height>:w<width> etc.
     */
    জন্য (p = wname; *p && *p != L':'; p++)
    {
	যদি (p - wname + 1 >= LF_FACESIZE)
	    যাও theend;			// Name too long
	lf->lfFaceName[p - wname] = *p;
    }
    যদি (p != wname)
	lf->lfFaceName[p - wname] = NUL;

    // First set defaults
    lf->lfHeight = DEFAULT_FONT_HEIGHT * (পূর্ণসংখ্যা)vimGetDpiForSystem() / 96;
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
	পূর্ণসংখ্যা	did_replace = FALSE;

	জন্য (i = 0; lf->lfFaceName[i]; ++i)
	    যদি (lf->lfFaceName[i] == L'_')
	    {
		lf->lfFaceName[i] = L' ';
		did_replace = TRUE;
	    }
	যদি (!did_replace || init_logfont(lf) == FAIL)
	    যাও theend;
    }

    যখন (*p == L':')
	p++;

    // Set the values found after ':'
    যখন (*p)
    {
	নির্বাচন (*p++)
	{
	    // Note: Keep this in sync with gui_mch_expand_font().
	    ক্ষেত্র L'h':
		lf->lfHeight = - points_to_pixels(p, &p, TRUE, (long_i)printer_dc);
		বিরতি;
	    ক্ষেত্র L'w':
		lf->lfWidth = points_to_pixels(p, &p, FALSE, (long_i)printer_dc);
		বিরতি;
	    ক্ষেত্র L'W':
		lf->lfWeight = wcstol(p, &p, 10);
		বিরতি;
	    ক্ষেত্র L'b':
		lf->lfWeight = FW_BOLD;
		বিরতি;
	    ক্ষেত্র L'i':
		lf->lfItalic = TRUE;
		বিরতি;
	    ক্ষেত্র L'u':
		lf->lfUnderline = TRUE;
		বিরতি;
	    ক্ষেত্র L's':
		lf->lfStrikeOut = TRUE;
		বিরতি;
	    ক্ষেত্র L'c':
		জন্য (i = 0; i < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(charset_pairs); ++i)
		{
		    যদি (utf16ascncmp(p, (অক্ষর *)charset_pairs[i].name.string,
			    charset_pairs[i].name.দৈর্ঘ্য) == 0)
		    {
			lf->lfCharSet = charset_pairs[i].charset;
			p += charset_pairs[i].name.দৈর্ঘ্য;
			বিরতি;
		    }
		}

		যদি (i == (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(charset_pairs) && verbose)
		{
		    char_u *s = utf16_to_enc(p, ফাঁকা);
		    যদি (s != ফাঁকা)
		    {
			semsg(_(e_illegal_str_name_str_in_font_name_str),
							   "charset", s, name);
			vim_free(s);
		    }
		}
		বিরতি;
	    ক্ষেত্র L'f':
		// Font features (e.g., "fss19=1").
		// Parsed separately by gui_mch_init_font(); skip here.
		যখন (*p && *p != L':')
		    p++;
		বিরতি;
	    ক্ষেত্র L'q':
		জন্য (i = 0; i < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(quality_pairs); ++i)
		{
		    যদি (utf16ascncmp(p, (অক্ষর *)quality_pairs[i].name.string,
			    quality_pairs[i].name.দৈর্ঘ্য) == 0)
		    {
			lf->lfQuality = quality_pairs[i].quality;
			p += quality_pairs[i].name.দৈর্ঘ্য;
			বিরতি;
		    }
		}

		যদি (i == (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(quality_pairs) && verbose)
		{
		    char_u *s = utf16_to_enc(p, ফাঁকা);
		    যদি (s != ফাঁকা)
		    {
			semsg(_(e_illegal_str_name_str_in_font_name_str),
							   "quality", s, name);
			vim_free(s);
		    }
		}
		বিরতি;
	    পূর্বনির্ধারিত:
		যদি (verbose)
		    semsg(_(e_illegal_char_nr_in_font_name_str), p[-1], name);
		যাও theend;
	}
	যখন (*p == L':')
	    p++;
    }
    ret = OK;

theend:
    // ron: init lastlf
    যদি (ret == OK && printer_dc == ফাঁকা)
    {
	vim_free(lastlf);
	lastlf = ALLOC_ONE(LOGFONTW);
	যদি (lastlf != ফাঁকা)
	    mch_memmove(lastlf, lf, আকার(LOGFONTW));
    }
    vim_free(wname);

    ফেরত ret;
}

#endif // defined(FEAT_GUI) || defined(FEAT_PRINTER)

#যদি defined(FEAT_JOB_CHANNEL)
/*
 * Initialize the Winsock dll.
 */
    শূন্য
channel_init_winsock(শূন্য)
{
    WSADATA wsa_তথ্য;
    পূর্ণসংখ্যা wsaerr;

    যদি (WSInitialized)
	ফেরত;

    wsaerr = WSAStartup(MAKEWORD(2, 2), &wsa_তথ্য);
    যদি (wsaerr == 0)
	WSInitialized = TRUE;
}
#endif
