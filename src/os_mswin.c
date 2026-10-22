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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಅನ್ತರ್ಭಾವ <sys/types.h>
#ಅನ್ತರ್ಭಾವ <limits.h>

#ಅನ್ತರ್ಭಾವ <process.h>
#ಅನ್ತರ್ಭಾವ <direct.h>

#ಯದಿ !defined(FEAT_GUI_MSWIN)
# include <shellapi.h>
#endif

#ಯದಿ defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT)
# include <dlgs.h>
# include <winspool.h>
# include <commdlg.h>
#endif

// Record all output and all keyboard & mouse input
// #define MCH_WRITE_DUMP

#ifdef MCH_WRITE_DUMP
FILE* fdDump = ರಿಕ್ತ;
#endif

#ಯದಿ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ಬಾಹ್ಯ ವರ್ಣ g_szOrigTitle[];
#endif

#ifdef FEAT_GUI
ಬಾಹ್ಯ HWND s_hwnd;
#ಅನ್ಯಥಾ
ಸ್ಥಿರ HWND s_hwnd = 0;	    // console window handle, set by GetConsoleHwnd()
#endif

#ifdef FEAT_JOB_CHANNEL
ಪೂರ್ಣಾಙ್ಕ WSInitialized = FALSE; // WinSock is initialized
#endif


/*
 * Save the instance handle of the exe/dll.
 */
    ಶೂನ್ಯ
SaveInst(HINSTANCE hInst)
{
    g_hinst = hInst;
}

#ಯದಿ defined(FEAT_GUI_MSWIN)
/*
 * GUI version of mch_exit().
 * Shut down and exit with status `r'
 * Careful: mch_exit() may be called before mch_init()!
 */
    ಶೂನ್ಯ
mch_exit_g(ಪೂರ್ಣಾಙ್ಕ r)
{
    exiting = TRUE;

    display_errors();

    ml_close_all(TRUE);		// remove all memfiles

# ifdef FEAT_OLE
    UninitOLE();
# endif
# ifdef FEAT_JOB_CHANNEL
    ಯದಿ (WSInitialized)
    {
	WSInitialized = FALSE;
	WSACleanup();
    }
# endif
# ifdef DYNAMIC_GETTEXT
    dyn_libintl_end();
# endif

    ಯದಿ (gui.in_use)
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
ಪ್ರಕಾರನಿರ್ದೇಶ BOOL (WINAPI *PfnRtlGetVersion)(LPOSVERSIONINFOW);
DWORD win_version;
    ಸ್ಥಿರ ಶೂನ್ಯ
win_version_init(ಶೂನ್ಯ)
{
    OSVERSIONINFOW	osver;
    HMODULE		hNtdll;
    PfnRtlGetVersion	pRtlGetVersion;

    hNtdll = GetModuleHandle("ntdll.dll");
    ಯದಿ (hNtdll == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    pRtlGetVersion =
	(PfnRtlGetVersion) GetProcAddress(hNtdll, "RtlGetVersion");
    ಯದಿ (pRtlGetVersion == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    osver.dwOSVersionInfoSize = ಪರಿಮಾಣ(OSVERSIONINFOW);
    pRtlGetVersion(&osver);
    win_version = MAKE_VER(min(osver.dwMajorVersion, 0xFF),
	    min(osver.dwMinorVersion, 0xFF),
	    min(osver.dw_Build_ಸಂಖ್ಯೆ, 0xFFFF));
}

/*
 * Init the tables for toupper() and tolower().
 */
    ಶೂನ್ಯ
mch_early_init(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    win_version_init();

    PlatformId();

    // Init the tables for toupper() and tolower()
    ಪ್ರತಿ (i = 0; i < 256; ++i)
	toupper_tab[i] = tolower_tab[i] = i;
    CharUpperBuff((LPSTR)toupper_tab, 256);
    CharLowerBuff((LPSTR)tolower_tab, 256);
}


/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_ಒಳಹರಿವು_isatty(ಶೂನ್ಯ)
{
#ifdef FEAT_GUI_MSWIN
# ifdef VIMDLL
    ಯದಿ (gui.in_use)
# endif
	ಪ್ರತ್ಯಾಗಮ TRUE;	    // GUI always has a tty
#endif
#ಯದಿ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    ಯದಿ (isatty(read_cmd_fd))
	ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
#endif
}

/*
 * mch_settitle(): set titlebar of our window
 */
    ಶೂನ್ಯ
mch_settitle(
    char_u *title,
    char_u *icon UNUSED)
{
#ifdef FEAT_GUI_MSWIN
# ifdef VIMDLL
    ಯದಿ (gui.in_use)
# endif
    {
	gui_mch_settitle(title, icon);
	ಪ್ರತ್ಯಾಗಮ;
    }
#endif
#ಯದಿ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    ಯದಿ (title != ರಿಕ್ತ)
    {
	WCHAR	*wp = enc_to_utf16(title, ರಿಕ್ತ);

	ಯದಿ (wp == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;

	SetConsoleTitleW(wp);
	vim_free(wp);
	ಪ್ರತ್ಯಾಗಮ;
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
    ಶೂನ್ಯ
mch_restore_title(ಪೂರ್ಣಾಙ್ಕ which UNUSED)
{
#ಯದಿ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
    ಯದಿ (!gui.in_use)
# endif
	SetConsoleTitle(g_szOrigTitle);
#endif
}


/*
 * Return TRUE if we can restore the title (we can)
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_can_restore_title(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ TRUE;
}


/*
 * Return TRUE if we can restore the icon title (we can't)
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_can_restore_icon(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ FALSE;
}


/*
 * Get absolute file name into buffer "buf" of length "len" bytes,
 * turning all '/'s into '\\'s and getting the correct case of each component
 * of the file name.  Append a (back)slash to a directory name.
 * When 'shellslash' set do it the other way around.
 * Return OK or FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    ಪೂರ್ಣಾಙ್ಕ		len,
    ಪೂರ್ಣಾಙ್ಕ		force UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ		nResult = FAIL;
    WCHAR	*wname;
    WCHAR	wbuf[MAX_PATH];
    char_u	*cname = ರಿಕ್ತ;

    wname = enc_to_utf16(fname, ರಿಕ್ತ);
    ಯದಿ (wname != ರಿಕ್ತ && _wfullpath(wbuf, wname, MAX_PATH) != ರಿಕ್ತ)
    {
	cname = utf16_to_enc((short_u *)wbuf, ರಿಕ್ತ);
	ಯದಿ (cname != ರಿಕ್ತ)
	{
	    vim_strncpy(buf, cname, len - 1);
	    nResult = OK;
	}
    }
    vim_free(wname);
    vim_free(cname);

#ifdef USE_FNAME_CASE
    fname_case(buf, len);
#ಅನ್ಯಥಾ
    slash_adjust(buf);
#endif

    ಪ್ರತ್ಯಾಗಮ nResult;
}


/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_isFullName(char_u *fname)
{
    // A name like "d:/foo" and "//server/share" is absolute.  "d:foo" is not.
    // /foo and \foo are absolute too because Windows keeps a current drive.
    // Another way to check is to use mch_FullName() and see if the result is
    // the same as the name or mch_FullName() fails.  However, this has quite a
    // bit of overhead, so let's not do that.
    ಯದಿ (*fname == NUL)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತ್ಯಾಗಮ ((ASCII_ISALPHA(fname[0]) && fname[1] == ':'
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
    ಶೂನ್ಯ
slash_adjust(char_u *p)
{
    ಯದಿ (path_with_url(p))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (*p == '`')
    {
	size_t len = STRLEN(p);

	// don't replace backslash in backtick quoted strings
	ಯದಿ (len > 2 && *(p + len - 1) == '`')
	    ಪ್ರತ್ಯಾಗಮ;
    }

    ಯಾವತ್ (*p)
    {
	ಯದಿ (*p == psepcN)
	    *p = psepc;
	MB_PTR_ADV(p);
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
read_reparse_point(ಧ್ರುವ WCHAR *name, char_u *buf, DWORD *buf_len)
{
    HANDLE h;
    BOOL ok;

    h = CreateFileW(name, FILE_READ_ATTRIBUTES,
	    FILE_SHARE_READ | FILE_SHARE_WRITE, ರಿಕ್ತ,
	    OPEN_EXISTING,
	    FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,
	    ರಿಕ್ತ);
    ಯದಿ (h == INVALID_HANDLE_ಮೌಲ್ಯ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ok = DeviceIoControl(h, FSCTL_GET_REPARSE_POINT, ರಿಕ್ತ, 0, buf, *buf_len,
	    buf_len, ರಿಕ್ತ);
    CloseHandle(h);

    ಪ್ರತ್ಯಾಗಮ ok ? OK : FAIL;
}

    char_u *
resolve_appexeclink(char_u *fname)
{
    DWORD		attr = 0;
    ಪೂರ್ಣಾಙ್ಕ			idx;
    WCHAR		*p, *end, *wname;
    // The buffer size is arbitrarily chosen to be "big enough" (TM), the
    // ceiling should be around 16k.
    char_u		buf[4096];
    DWORD		buf_len = ಪರಿಮಾಣ(buf);
    REPARSE_DATA_BUFFER *rb = (REPARSE_DATA_BUFFER *)buf;

    wname = enc_to_utf16(fname, ರಿಕ್ತ);
    ಯದಿ (wname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    attr = GetFileAttributesW(wname);
    ಯದಿ (attr == INVALID_FILE_ATTRIBUTES ||
	    (attr & FILE_ATTRIBUTE_REPARSE_POINT) == 0)
    {
	vim_free(wname);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // The applinks are similar to symlinks but with a huge difference: they can
    // only be executed, any other I/O operation on them is bound to fail with
    // ERROR_FILE_NOT_FOUND even though the file exists.
    ಯದಿ (read_reparse_point(wname, buf, &buf_len) == FAIL)
    {
	vim_free(wname);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    vim_free(wname);

    ಯದಿ (rb->ReparseTag != IO_REPARSE_TAG_APPEXECLINK)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // The (undocumented) reparse buffer contains a set of N null-terminated
    // Unicode strings, the application path is stored in the third one.
    ಯದಿ (rb->AppExecLinkReparseBuffer.String_ಎಣಿಕೆ < 3)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    p = rb->AppExecLinkReparseBuffer.StringList;
    end = p + rb->ReparseDataLength / ಪರಿಮಾಣ(WCHAR);
    ಪ್ರತಿ (idx = 0; p < end
	    && idx < (ಪೂರ್ಣಾಙ್ಕ)rb->AppExecLinkReparseBuffer.String_ಎಣಿಕೆ
	    && idx != 2; )
    {
	ಯದಿ (*p++ == L'\0')
	    ++idx;
    }

    ಪ್ರತ್ಯಾಗಮ utf16_to_enc(p, ರಿಕ್ತ);
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
mswin_stat_impl(ಧ್ರುವ WCHAR *name, stat_T *stp, ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ resolve)
{
    ಪೂರ್ಣಾಙ್ಕ			n;
    ಪೂರ್ಣಾಙ್ಕ			fd;
    BOOL		is_symlink = FALSE;
    HANDLE		hFind, h;
    DWORD		attr = 0;
    DWORD		flag = 0;
    WIN32_FIND_DATAW    findDataW;

#ಯದಿ 0 && defined(_UCRT)
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
    ಯದಿ (resolve)
	// Universal CRT can handle symlinks properly.
	ಪ್ರತ್ಯಾಗಮ _wstat(name, stp);
#endif

    // Only reparse points need a directory query to identify symbolic links.
    // In particular, avoid enumerating the directory for each missing file
    // while searching 'runtimepath'.
    attr = GetFileAttributesW(name);
    ಯದಿ (attr == INVALID_FILE_ATTRIBUTES)
    {
	DWORD error = GetLastError();

	// A missing file or path cannot provide stat information.  Other
	// errors and UNC paths still need the CRT fallback.
	ಯದಿ ((error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND)
		&& !((name[0] == L'\\' || name[0] == L'/')
		    && (name[1] == L'\\' || name[1] == L'/')))
	{
	    errno = ENOENT;
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
	ಪ್ರತ್ಯಾಗಮ _wstat(name, stp);
    }
    ಯದಿ (!(attr & FILE_ATTRIBUTE_REPARSE_POINT))
	ಪ್ರತ್ಯಾಗಮ _wstat(name, stp);

    hFind = FindFirstFileW(name, &findDataW);
    ಯದಿ (hFind != INVALID_HANDLE_ಮೌಲ್ಯ)
    {
	attr = findDataW.dwFileAttributes;
	ಯದಿ ((attr & FILE_ATTRIBUTE_REPARSE_POINT)
		&& (findDataW.dwReserved0 == IO_REPARSE_TAG_SYMLINK))
	    is_symlink = TRUE;
	FindClose(hFind);
    }

    // Use the plain old stat() whenever it's possible.
    ಯದಿ (!is_symlink)
	ಪ್ರತ್ಯಾಗಮ _wstat(name, stp);

    ಯದಿ (!resolve && is_symlink)
	flag = FILE_FLAG_OPEN_REPARSE_POINT;
    ಯದಿ (attr & FILE_ATTRIBUTE_DIRECTORY)
	flag |= FILE_FLAG_BACKUP_SEMANTICS;

    h = CreateFileW(name, FILE_READ_ATTRIBUTES,
	    FILE_SHARE_READ | FILE_SHARE_WRITE, ರಿಕ್ತ, OPEN_EXISTING, flag,
	    ರಿಕ್ತ);
    ಯದಿ (h == INVALID_HANDLE_ಮೌಲ್ಯ)
	ಪ್ರತ್ಯಾಗಮ -1;

    fd = _open_osfhandle((intptr_t)h, _O_RDONLY);
    n = _fstat(fd, (ಸಂರಚನಾ _stat *)stp);
    ಯದಿ ((n == 0) && (attr & FILE_ATTRIBUTE_DIRECTORY))
	stp->st_mode = (stp->st_mode & ~S_IFMT) | S_IFDIR;
    _close(fd);

    ಪ್ರತ್ಯಾಗಮ n;
}

/*
 * stat() can't handle a trailing '/' or '\', remove it first.
 * When 'resolve' is true behave as lstat() wrt symlinks.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
stat_impl(ಧ್ರುವ ವರ್ಣ *name, stat_T *stp, ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ resolve)
{
    // WinNT and later can use _MAX_PATH wide characters for a pathname, which
    // means that the maximum pathname is _MAX_PATH * 3 bytes when 'enc' is
    // UTF-8.
    char_u	buf[_MAX_PATH * 3 + 1];
    size_t	buflen;
    char_u	*p;
    WCHAR	*wp;
    ಪೂರ್ಣಾಙ್ಕ		n;

    vim_strncpy((char_u *)buf, (char_u *)name, ಪರಿಮಾಣ(buf) - 1);
    buflen = STRLEN(buf);
    p = buf + buflen;
    ಯದಿ (p > buf)
	MB_PTR_BACK(buf, p);

    // Remove trailing '\\' except root path.
    ಯದಿ (p > buf && (*p == '\\' || *p == '/') && p[-1] != ':')
    {
	*p = NUL;
	buflen = (size_t)(p - buf);
    }

    ಯದಿ ((buf[0] == '\\' && buf[1] == '\\') || (buf[0] == '/' && buf[1] == '/'))
    {
	// UNC root path must be followed by '\\'.
	p = vim_strpbrk(buf + 2, (char_u *)"\\/");
	ಯದಿ (p != ರಿಕ್ತ)
	{
	    p = vim_strpbrk(p + 1, (char_u *)"\\/");
	    ಯದಿ (p == ರಿಕ್ತ)
		STRCPY(buf + buflen, "\\");
	}
    }

    wp = enc_to_utf16(buf, ರಿಕ್ತ);
    ಯದಿ (wp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    n = mswin_stat_impl(wp, stp, resolve);
    vim_free(wp);
    ಪ್ರತ್ಯಾಗಮ n;
}

    ಪೂರ್ಣಾಙ್ಕ
vim_lstat(ಧ್ರುವ ವರ್ಣ *name, stat_T *stp)
{
    ಪ್ರತ್ಯಾಗಮ stat_impl(name, stp, FALSE);
}

    ಪೂರ್ಣಾಙ್ಕ
vim_stat(ಧ್ರುವ ವರ್ಣ *name, stat_T *stp)
{
    ಪ್ರತ್ಯಾಗಮ stat_impl(name, stp, TRUE);
}

#ಯದಿ defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    ಶೂನ್ಯ
mch_settmode(tmode_T tmode UNUSED)
{
    // nothing to do
}

    ಪೂರ್ಣಾಙ್ಕ
mch_get_shellsize(ಶೂನ್ಯ)
{
    // never used
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಶೂನ್ಯ
mch_set_shellsize(ಶೂನ್ಯ)
{
    // never used
}

/*
 * Rows and/or Columns has changed.
 */
    ಶೂನ್ಯ
mch_new_shellsize(ಶೂನ್ಯ)
{
    // never used
}

    ಶೂನ್ಯ
mch_calc_cell_size(ಸಂರಚನಾ cellsize *cs_out UNUSED)
{
    // never used
}

#endif

/*
 * We have no job control, so fake it by starting a new shell.
 */
    ಶೂನ್ಯ
mch_suspend(ಶೂನ್ಯ)
{
    suspend_shell();
}

#ಯದಿ defined(USE_MCH_ERRMSG)

# ifdef display_errors
#  undef display_errors
# endif

/*
 * Display the saved error message(s).
 */
    ಶೂನ್ಯ
display_errors(ಶೂನ್ಯ)
{
# ifdef FEAT_GUI
    char_u *p;

#  ifdef VIMDLL
    ಯದಿ (gui.in_use || gui.starting)
#  endif
    {
	ಯದಿ (error_ga.ga_data != ರಿಕ್ತ)
	{
	    // avoid putting up a message box with blanks only
	    ಪ್ರತಿ (p = (char_u *)error_ga.ga_data; *p; ++p)
		ಯದಿ (!SAFE_isspace(*p))
		{
		    // Only use a dialog when not using --gui-dialog-file:
		    // write text to a file.
		    ಯದಿ (!gui_dialog_log((char_u *)"Errors", p))
			(ಶೂನ್ಯ)gui_mch_dialog(
				     gui.starting ? VIM_INFO :
					     VIM_ERROR,
				     gui.starting ? (char_u *)_("Message") :
					     (char_u *)_("Error"),
					     p, (char_u *)_("&Ok"),
					1, ರಿಕ್ತ, FALSE);
		    ಭಙ್ಗ;
		}
	    ga_clear(&error_ga);
	}
	ಪ್ರತ್ಯಾಗಮ;
    }
# endif
# ಯದಿ !defined(FEAT_GUI) || defined(VIMDLL)
    FlushFileBuffers(GetStdHandle(STD_ERROR_HANDLE));
# endif
}
#endif


/*
 * Return TRUE if "p" contain a wildcard that can be expanded by
 * dos_expandpath().
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_has_exp_wildcard(char_u *p)
{
    ಪ್ರತಿ ( ; *p; MB_PTR_ADV(p))
    {
	ಯದಿ (vim_strchr((char_u *)"?*[", *p) != ರಿಕ್ತ
		|| (*p == '~' && p[1] != NUL))
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Return TRUE if "p" contain a wildcard or a "~1" kind of thing (could be a
 * shortened file name).
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_has_wildcard(char_u *p)
{
    ಪ್ರತಿ ( ; *p; MB_PTR_ADV(p))
    {
	ಯದಿ (vim_strchr((char_u *)"?*$[`", *p) != ರಿಕ್ತ
		|| (*p == '~' && p[1] != NUL))
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}


/*
 * The normal _chdir() does not change the default drive.  This one does.
 * Returning 0 implies success; -1 implies failure.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_chdir(ವರ್ಣ *path)
{
    WCHAR   *p;
    ಪೂರ್ಣಾಙ್ಕ	    n;

    ಯದಿ (path[0] == NUL)		// just checking...
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯದಿ (p_verbose >= 5)
    {
	verbose_enter();
	smsg("chdir(%s)", path);
	verbose_leave();
    }
    ಯದಿ (SAFE_isalpha(path[0]) && path[1] == ':')	// has a drive name
    {
	// If we can change to the drive, skip that part of the path.  If we
	// can't then the current directory may be invalid, try using chdir()
	// with the whole path.
	ಯದಿ (_chdrive(TOLOWER_ASC(path[0]) - 'a' + 1) == 0)
	    path += 2;
    }

    ಯದಿ (*path == NUL)		// drive name only
	ಪ್ರತ್ಯಾಗಮ 0;

    p = enc_to_utf16((char_u *)path, ರಿಕ್ತ);
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    n = _wchdir(p);
    vim_free(p);
    ಪ್ರತ್ಯಾಗಮ n;
}


#ಯದಿ defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
/*
 * return non-zero if a character is available
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_char_avail(ಶೂನ್ಯ)
{
    // never used
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

# ಯದಿ defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_check_messages(ಶೂನ್ಯ)
{
    // TODO: check for messages
    ಪ್ರತ್ಯಾಗಮ TRUE;
}
# endif
#endif


#ಯದಿ defined(FEAT_LIBCALL)
/*
 * Call a DLL routine which takes either a string or int param
 * and returns an allocated string.
 * Return OK if it worked, FAIL if not.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ char_u *(*MYSTRPROCSTR)(char_u *);
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*MYSTRPROCINT)(char_u *);
ಪ್ರಕಾರನಿರ್ದೇಶ char_u *(*MYINTPROCSTR)(ಪೂರ್ಣಾಙ್ಕ);
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*MYINTPROCINT)(ಪೂರ್ಣಾಙ್ಕ);

/*
 * Check if a pointer points to a valid NUL terminated string.
 * Return the length of the string, including terminating NUL.
 * Returns 0 for an invalid pointer, 1 for an empty string.
 */
    ಸ್ಥಿರ size_t
check_str_len(char_u *str)
{
    SYSTEM_INFO			si;
    MEMORY_BASIC_INFORMATION	mbi;
    size_t			length = 0;
    size_t			i;
    ಧ್ರುವ char_u		*p;

    // get page size
    GetSystemInfo(&si);

    // get memory information
    ಯದಿ (!VirtualQuery(str, &mbi, ಪರಿಮಾಣ(mbi)))
	ಪ್ರತ್ಯಾಗಮ 0;

    // pre cast these (typing savers)
    long_u dwStr = (long_u)str;
    long_u dwBaseAddress = (long_u)mbi.BaseAddress;

    // get start address of page that str is on
    long_u strPage = dwStr - (dwStr - dwBaseAddress) % si.dwPageSize;

    // get length from str to end of page
    long_u pageLength = si.dwPageSize - (dwStr - strPage);

    ಪ್ರತಿ (p = str; !IsBadReadPtr(p, (UINT)pageLength);
	    p += pageLength, pageLength = si.dwPageSize)
	ಪ್ರತಿ (i = 0; i < pageLength; ++i, ++length)
	    ಯದಿ (p[i] == NUL)
		ಪ್ರತ್ಯಾಗಮ length + 1;

    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Passed to do_in_runtimepath() to load a vim.ico file.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
mch_icon_load_cb(char_u *fname, ಶೂನ್ಯ *cookie)
{
    HANDLE *h = (HANDLE *)cookie;

    *h = LoadImage(ರಿಕ್ತ,
		   (LPSTR)fname,
		   IMAGE_ICON,
		   64,
		   64,
		   LR_LOADFROMFILE | LR_LOADMAP3DCOLORS);
}

/*
 * Try loading an icon file from 'runtimepath'.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_icon_load(HANDLE *iconp)
{
    ಪ್ರತ್ಯಾಗಮ do_in_runtimepath((char_u *)"bitmaps/vim.ico",
						  0, mch_icon_load_cb, iconp);
}

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if the OS PRNG is not available or something went wrong.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_get_random(char_u *buf, ಪೂರ್ಣಾಙ್ಕ len)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		initialized = NOTDONE;
    ಸ್ಥಿರ HINSTANCE	hInstLib;
    ಸ್ಥಿರ BOOL (WINAPI *pProcessPrng)(PUCHAR, ULONG);

    ಯದಿ (initialized == NOTDONE)
    {
	hInstLib = vimLoadLib("bcryptprimitives.dll");
	ಯದಿ (hInstLib != ರಿಕ್ತ)
	    pProcessPrng = (ಶೂನ್ಯ *)GetProcAddress(hInstLib, "ProcessPrng");
	ಯದಿ (hInstLib == ರಿಕ್ತ || pProcessPrng == ರಿಕ್ತ)
	{
	    FreeLibrary(hInstLib);
	    initialized = FAIL;
	}
	ಅನ್ಯಥಾ
	    initialized = OK;
    }

    ಯದಿ (initialized == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // According to the documentation this call cannot fail.
    pProcessPrng(buf, len);

    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಪೂರ್ಣಾಙ್ಕ
mch_libcall(
    char_u	*libname,
    char_u	*funcname,
    char_u	*argstring,	// NULL when using a argint
    ಪೂರ್ಣಾಙ್ಕ		argint,
    char_u	**string_result,// NULL when using number_result
    ಪೂರ್ಣಾಙ್ಕ		*ಸಂಖ್ಯೆ_result)
{
    HINSTANCE		hinstLib;

    BOOL fRunTimeLinkSuccess = FALSE;

    // Get a handle to the DLL module.
    hinstLib = vimLoadLib((ವರ್ಣ *)libname);

    // If the handle is valid, try to get the function address.
    ಯದಿ (hinstLib != ರಿಕ್ತ)
    {
	char_u	*retval_str = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ	retval_int = 0;
	size_t	len;

# ifdef HAVE_TRY_EXCEPT
	__try
	{
# endif
	ಯದಿ (argstring != ರಿಕ್ತ)
	{
	    // Call with string argument
	    ಯದಿ (string_result != ರಿಕ್ತ)
	    {
		MYSTRPROCSTR proc;

		proc = (MYSTRPROCSTR)GetProcAddress(hinstLib, (LPCSTR)funcname);
		ಯದಿ (proc != ರಿಕ್ತ)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_str = proc(argstring);
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		MYSTRPROCINT proc;

		proc = (MYSTRPROCINT)GetProcAddress(hinstLib, (LPCSTR)funcname);
		ಯದಿ (proc != ರಿಕ್ತ)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_int = proc(argstring);
		}
	    }
	}
	ಅನ್ಯಥಾ
	{
	    // Call with number argument
	    ಯದಿ (string_result != ರಿಕ್ತ)
	    {
		MYINTPROCSTR proc;

		proc = (MYINTPROCSTR)GetProcAddress(hinstLib, (LPCSTR)funcname);
		ಯದಿ (proc != ರಿಕ್ತ)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_str = proc(argint);
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		MYINTPROCINT proc;

		proc = (MYINTPROCINT)GetProcAddress(hinstLib, (LPCSTR)funcname);
		ಯದಿ (proc != ರಿಕ್ತ)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_int = proc(argint);
		}
	    }
	}

	// Save the string before we free the library.
	// Assume that a "1" result is an illegal pointer.
	ಯದಿ (string_result == ರಿಕ್ತ)
	    *ಸಂಖ್ಯೆ_result = retval_int;
	ಅನ್ಯಥಾ ಯದಿ (retval_str != ರಿಕ್ತ
		&& (len = check_str_len(retval_str)) > 0)
	{
	    *string_result = alloc(len);
	    ಯದಿ (*string_result != ರಿಕ್ತ)
		mch_memmove(*string_result, retval_str, len);
	}

# ifdef HAVE_TRY_EXCEPT
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	    ಯದಿ (GetExceptionCode() == EXCEPTION_ರಾಶಿ_OVERFLOW)
		_resetstkoflw();
	    fRunTimeLinkSuccess = FALSE;
	}
# endif

	// Free the DLL module.
	(ಶೂನ್ಯ)FreeLibrary(hinstLib);
    }

    ಯದಿ (!fRunTimeLinkSuccess)
    {
	semsg(_(e_library_call_failed_for_str), funcname);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}
#endif

/*
 * Debugging helper: expose the MCH_WRITE_DUMP stuff to other modules
 */
    ಶೂನ್ಯ
DumpPutS(ಧ್ರುವ ವರ್ಣ *psz UNUSED)
{
#ifdef MCH_WRITE_DUMP
    ಯದಿ (fdDump)
    {
	fputs(psz, fdDump);
	ಯದಿ (psz[strlen(psz) - 1] != '\n')
	    fputc('\n', fdDump);
	fflush(fdDump);
    }
#endif
}

#ifdef _DEBUG

ಶೂನ್ಯ __cdecl
Trace(
    ವರ್ಣ *pszFormat,
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

#ಯದಿ !defined(FEAT_GUI) || defined(VIMDLL)
ಬಾಹ್ಯ HWND g_hWnd;	// This is in os_win32.c.

/*
 * Showing the printer dialog is tricky since we have no GUI
 * window to parent it. The following routines are needed to
 * get the window parenting and Z-order to work properly.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
GetConsoleHwnd(ಶೂನ್ಯ)
{
    // Skip if it's already set.
    ಯದಿ (s_hwnd != 0)
	ಪ್ರತ್ಯಾಗಮ;

    // Window handle may have been found by init code (Windows NT only)
    ಯದಿ (g_hWnd != 0)
    {
	s_hwnd = g_hWnd;
	ಪ್ರತ್ಯಾಗಮ;
    }

    s_hwnd = GetConsoleWindow();
}

/*
 * Console implementation of ":winpos".
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_get_winpos(ಪೂರ್ಣಾಙ್ಕ *x, ಪೂರ್ಣಾಙ್ಕ *y)
{
    RECT  rect;

    GetConsoleHwnd();
    GetWindowRect(s_hwnd, &rect);
    *x = rect.ಎಡ;
    *y = rect.ತುದಿ;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Console implementation of ":winpos x y".
 */
    ಶೂನ್ಯ
mch_set_winpos(ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y)
{
    GetConsoleHwnd();
    SetWindowPos(s_hwnd, ರಿಕ್ತ, x, y, 0, 0,
		 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}
#endif

#ಯದಿ defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT)

//=================================================================
// Win32 printer stuff

ಸ್ಥಿರ HFONT		prt_font_handles[2][2][2];
ಸ್ಥಿರ PRINTDLGW	prt_dlg;
ಸ್ಥಿರ ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ	boldface[2] = {FW_REGULAR, FW_BOLD};
ಸ್ಥಿರ TEXTMETRIC	prt_tm;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		prt_ಸಾಲು_height;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		prt_ಸಂಖ್ಯೆ_width;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		prt_ಎಡ_margin;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		prt_ಬಲ_margin;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		prt_ತುದಿ_margin;
ಸ್ಥಿರ char_u		szAppName[] = TEXT("VIM");
ಸ್ಥಿರ HWND		hDlgPrint;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		*bUserAbort = ರಿಕ್ತ;
ಸ್ಥಿರ char_u		*prt_name = ರಿಕ್ತ;

// Defines which are also in vim.rc.
# define IDC_BOX1		400
# define IDC_PRINTTEXT1		401
# define IDC_PRINTTEXT2		402
# define IDC_PROGRESS		403

    ಸ್ಥಿರ BOOL
vimSetDlgItemText(HWND hDlg, ಪೂರ್ಣಾಙ್ಕ nIDDlgItem, char_u *s)
{
    WCHAR   *wp;
    BOOL    ret;

    wp = enc_to_utf16(s, ರಿಕ್ತ);
    ಯದಿ (wp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ret = SetDlgItemTextW(hDlg, nIDDlgItem, wp);
    vim_free(wp);
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Convert BGR to RGB for Windows GDI calls
 */
    ಸ್ಥಿರ COLORREF
swap_me(COLORREF colorref)
{
    ಪೂರ್ಣಾಙ್ಕ  temp;
    ವರ್ಣ *ptr = (ವರ್ಣ *)&colorref;

    temp = *(ptr);
    *(ptr ) = *(ptr + 2);
    *(ptr + 2) = temp;
    ಪ್ರತ್ಯಾಗಮ colorref;
}

    ಸ್ಥಿರ INT_PTR CALLBACK
PrintDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam UNUSED,
	LPARAM lParam UNUSED)
{
# ifdef FEAT_GETTEXT
    NONCLIENTMETRICS nm;
    ಸ್ಥಿರ HFONT hfont;
# endif

    ವಿಕಲ್ಪ (message)
    {
	ಪಕ್ಷ WM_INITDIALOG:
# ifdef FEAT_GETTEXT
	    nm.cbSize = ಪರಿಮಾಣ(NONCLIENTMETRICS);
	    ಯದಿ (SystemParametersInfo(
			SPI_GETNONCLIENTMETRICS,
			ಪರಿಮಾಣ(NONCLIENTMETRICS),
			&nm,
			0))
	    {
		ವರ್ಣ buff[MAX_PATH];
		ಪೂರ್ಣಾಙ್ಕ i;

		// Translate the dialog texts
		hfont = CreateFontIndirect(&nm.lfMessageFont);
		ಪ್ರತಿ (i = IDC_PRINTTEXT1; i <= IDC_PROGRESS; i++)
		{
		    SendDlgItemMessage(hDlg, i, WM_SETFONT, (WPARAM)hfont, 1);
		    ಯದಿ (GetDlgItemText(hDlg,i, buff, ಪರಿಮಾಣ(buff)))
			vimSetDlgItemText(hDlg,i, (char_u *)_(buff));
		}
		SendDlgItemMessage(hDlg, IDCANCEL,
						WM_SETFONT, (WPARAM)hfont, 1);
		ಯದಿ (GetDlgItemText(hDlg,IDCANCEL, buff, ಪರಿಮಾಣ(buff)))
		    vimSetDlgItemText(hDlg,IDCANCEL, (char_u *)_(buff));
	    }
# endif
	    SetWindowText(hDlg, (LPCSTR)szAppName);
	    ಯದಿ (prt_name != ರಿಕ್ತ)
	    {
		vimSetDlgItemText(hDlg, IDC_PRINTTEXT2, (char_u *)prt_name);
		VIM_CLEAR(prt_name);
	    }
	    EnableMenuItem(GetSystemMenu(hDlg, FALSE), SC_CLOSE, MF_GRAYED);
# ಯದಿ !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	    ಯದಿ (!gui.in_use)
#  endif
		Bring_Window_To_ತುದಿ(s_hwnd);
# endif
	    ಪ್ರತ್ಯಾಗಮ TRUE;

	ಪಕ್ಷ WM_COMMAND:
	    *bUserAbort = TRUE;
	    EnableWindow(GetParent(hDlg), TRUE);
	    DestroyWindow(hDlg);
	    hDlgPrint = ರಿಕ್ತ;
# ifdef FEAT_GETTEXT
	    DeleteObject(hfont);
# endif
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

    ಸ್ಥಿರ BOOL CALLBACK
AbortProc(HDC hdcPrn UNUSED, ಪೂರ್ಣಾಙ್ಕ iCode UNUSED)
{
    ಸಂದೇಶ_2 ಸಂದೇಶ;

    ಯಾವತ್ (!*bUserAbort && PeekMessageW(&ಸಂದೇಶ, ರಿಕ್ತ, 0, 0, PM_REMOVE))
    {
	ಯದಿ (!hDlgPrint || !IsDialogMessageW(hDlgPrint, &ಸಂದೇಶ))
	{
	    TranslateMessage(&ಸಂದೇಶ);
	    DispatchMessageW(&ಸಂದೇಶ);
	}
    }
    ಪ್ರತ್ಯಾಗಮ !*bUserAbort;
}

    ಸ್ಥಿರ UINT_PTR CALLBACK
PrintHookProc(
	HWND hDlg,	// handle to dialog box
	UINT ui_ಸಂದೇಶ,	// message identifier
	WPARAM wParam UNUSED,	// message parameter
	LPARAM lParam	// message parameter
	)
{
    HWND	hwndOwner;
    RECT	rc, rcDlg, rcOwner;
    PRINTDLGW	*pPD;

    ಯದಿ (ui_ಸಂದೇಶ != WM_INITDIALOG)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // Get the owner window and dialog box rectangles.
    ಯದಿ ((hwndOwner = GetParent(hDlg)) == ರಿಕ್ತ)
	hwndOwner = GetDesktopWindow();

    GetWindowRect(hwndOwner, &rcOwner);
    GetWindowRect(hDlg, &rcDlg);
    CopyRect(&rc, &rcOwner);

    // Offset the owner and dialog box rectangles so that
    // right and bottom values represent the width and
    // height, and then offset the owner again to discard
    // space taken up by the dialog box.

    OffsetRect(&rcDlg, -rcDlg.ಎಡ, -rcDlg.ತುದಿ);
    OffsetRect(&rc, -rc.ಎಡ, -rc.ತುದಿ);
    OffsetRect(&rc, -rcDlg.ಬಲ, -rcDlg.bottom);

    // The new position is the sum of half the remaining
    // space and the owner's original position.

    SetWindowPos(hDlg,
	    HWND_ತುದಿ,
	    rcOwner.ಎಡ + (rc.ಬಲ / 2),
	    rcOwner.ತುದಿ + (rc.bottom / 2),
	    0, 0,		// ignores size arguments
	    SWP_NOSIZE);

    //  tackle the printdlg copiesctrl problem
    pPD = (PRINTDLGW *)lParam;
    pPD->nCopies = (WORD)pPD->lCustData;
    SetDlgItemInt( hDlg, edt3, pPD->nCopies, FALSE );
    //  Bring the window to top
    Bring_Window_To_ತುದಿ(GetParent(hDlg));
    SetForegroundWindow(hDlg);

    ಪ್ರತ್ಯಾಗಮ FALSE;
}

    ಶೂನ್ಯ
mch_print_cleanup(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ pifItalic;
    ಪೂರ್ಣಾಙ್ಕ pifBold;
    ಪೂರ್ಣಾಙ್ಕ pifUnderline;

    ಪ್ರತಿ (pifBold = 0; pifBold <= 1; pifBold++)
	ಪ್ರತಿ (pifItalic = 0; pifItalic <= 1; pifItalic++)
	    ಪ್ರತಿ (pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
		DeleteObject(prt_font_handles[pifBold][pifItalic][pifUnderline]);

    ಯದಿ (prt_dlg.hDC != ರಿಕ್ತ)
	DeleteDC(prt_dlg.hDC);
    ಯದಿ (!*bUserAbort && hDlgPrint != ರಿಕ್ತ)
	SendMessage(hDlgPrint, WM_COMMAND, 0, 0);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
to_device_units(ಪೂರ್ಣಾಙ್ಕ idx, ಪೂರ್ಣಾಙ್ಕ dpi, ಪೂರ್ಣಾಙ್ಕ physsize, ಪೂರ್ಣಾಙ್ಕ offset, ಪೂರ್ಣಾಙ್ಕ def_ಸಂಖ್ಯೆ)
{
    ಪೂರ್ಣಾಙ್ಕ		ret = 0;
    ಪೂರ್ಣಾಙ್ಕ		u;
    ಪೂರ್ಣಾಙ್ಕ		nr;

    u = prt_get_unit(idx);
    ಯದಿ (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_ಸಂಖ್ಯೆ;
    }
    ಅನ್ಯಥಾ
	nr = printer_opts[idx].ಸಂಖ್ಯೆ;

    ವಿಕಲ್ಪ (u)
    {
	ಪಕ್ಷ PRT_UNIT_PERC:
	    ret = (physsize * nr) / 100;
	    ಭಙ್ಗ;
	ಪಕ್ಷ PRT_UNIT_INCH:
	    ret = (nr * dpi);
	    ಭಙ್ಗ;
	ಪಕ್ಷ PRT_UNIT_MM:
	    ret = (nr * 10 * dpi) / 254;
	    ಭಙ್ಗ;
	ಪಕ್ಷ PRT_UNIT_POINT:
	    ret = (nr * 10 * dpi) / 720;
	    ಭಙ್ಗ;
    }

    ಯದಿ (ret < offset)
	ಪ್ರತ್ಯಾಗಮ 0;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ ret - offset;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
prt_get_cpl(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		hr;
    ಪೂರ್ಣಾಙ್ಕ		phyw;
    ಪೂರ್ಣಾಙ್ಕ		dvoff;
    ಪೂರ್ಣಾಙ್ಕ		rev_offset;
    ಪೂರ್ಣಾಙ್ಕ		dpi;

    GetTextMetrics(prt_dlg.hDC, &prt_tm);
    prt_ಸಾಲು_height = prt_tm.tmHeight + prt_tm.tmExternalLeading;

    hr	    = GetDeviceCaps(prt_dlg.hDC, HORZRES);
    phyw    = GetDeviceCaps(prt_dlg.hDC, PHYSICALWIDTH);
    dvoff   = GetDeviceCaps(prt_dlg.hDC, PHYSICALOFFSETX);
    dpi	    = GetDeviceCaps(prt_dlg.hDC, LOGPIXELSX);

    rev_offset = phyw - (dvoff + hr);

    prt_ಎಡ_margin = to_device_units(OPT_PRINT_ಎಡ, dpi, phyw, dvoff, 10);
    ಯದಿ (prt_use_ಸಂಖ್ಯೆ())
    {
	prt_ಸಂಖ್ಯೆ_width = PRINT_ಸಂಖ್ಯೆ_WIDTH * prt_tm.tmAveCharWidth;
	prt_ಎಡ_margin += prt_ಸಂಖ್ಯೆ_width;
    }
    ಅನ್ಯಥಾ
	prt_ಸಂಖ್ಯೆ_width = 0;

    prt_ಬಲ_margin = hr - to_device_units(OPT_PRINT_ಬಲ, dpi, phyw,
							       rev_offset, 5);

    ಪ್ರತ್ಯಾಗಮ (prt_ಬಲ_margin - prt_ಎಡ_margin) / prt_tm.tmAveCharWidth;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
prt_get_lpp(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ vr;
    ಪೂರ್ಣಾಙ್ಕ phyw;
    ಪೂರ್ಣಾಙ್ಕ dvoff;
    ಪೂರ್ಣಾಙ್ಕ rev_offset;
    ಪೂರ್ಣಾಙ್ಕ	bottom_margin;
    ಪೂರ್ಣಾಙ್ಕ	dpi;

    vr	    = GetDeviceCaps(prt_dlg.hDC, VERTRES);
    phyw    = GetDeviceCaps(prt_dlg.hDC, PHYSICALHEIGHT);
    dvoff   = GetDeviceCaps(prt_dlg.hDC, PHYSICALOFFSETY);
    dpi	    = GetDeviceCaps(prt_dlg.hDC, LOGPIXELSY);

    rev_offset = phyw - (dvoff + vr);

    prt_ತುದಿ_margin = to_device_units(OPT_PRINT_ತುದಿ, dpi, phyw, dvoff, 5);

    // adjust top margin if there is a header
    prt_ತುದಿ_margin += prt_ಸಾಲು_height * prt_header_height();

    bottom_margin = vr - to_device_units(OPT_PRINT_BOT, dpi, phyw,
							       rev_offset, 5);

    ಪ್ರತ್ಯಾಗಮ (bottom_margin - prt_ತುದಿ_margin) / prt_ಸಾಲು_height;
}

    ಪೂರ್ಣಾಙ್ಕ
mch_print_init(prt_settings_T *psettings, char_u *jobname, ಪೂರ್ಣಾಙ್ಕ forceit)
{
    ಸ್ಥಿರ HGLOBAL	stored_dm = ರಿಕ್ತ;
    ಸ್ಥಿರ HGLOBAL	stored_devn = ರಿಕ್ತ;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		stored_nCopies = 1;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		stored_nFlags = 0;

    LOGFONTW		fLogFont;
    ಪೂರ್ಣಾಙ್ಕ			pifItalic;
    ಪೂರ್ಣಾಙ್ಕ			pifBold;
    ಪೂರ್ಣಾಙ್ಕ			pifUnderline;

    DEVMODEW		*mem;
    DEVNAMES		*devname;
    ಪೂರ್ಣಾಙ್ಕ			i;
    DWORD		err;

    bUserAbort = &(psettings->user_abort);
    CLEAR_FIELD(prt_dlg);
    prt_dlg.lStructSize = ಪರಿಮಾಣ(PRINTDLGW);
# ಯದಿ !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
    ಯದಿ (!gui.in_use)
#  endif
	GetConsoleHwnd();	    // get value of s_hwnd
# endif
    prt_dlg.hwndOwner = s_hwnd;
    prt_dlg.Flags = PD_NOPAGENUMS | PD_NOSELECTION | PD_RETURNDC;
    ಯದಿ (!forceit)
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
    ಯದಿ (forceit
# ಯದಿ !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	    || (!gui.in_use && !term_console)
#  ಅನ್ಯಥಾ
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
	ಯದಿ (*p_pdev != NUL)
	    prt_dlg.hDC = CreateDC(ರಿಕ್ತ, (LPCSTR)p_pdev, ರಿಕ್ತ, ರಿಕ್ತ);
	ಅನ್ಯಥಾ
	{
	    prt_dlg.Flags |= PD_RETURNDEFAULT;
	    ಯದಿ (PrintDlgW(&prt_dlg) == 0)
		ಗಚ್ಛ init_fail_dlg;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (PrintDlgW(&prt_dlg) == 0)
	ಗಚ್ಛ init_fail_dlg;
    ಅನ್ಯಥಾ
    {
	/*
	 * keep the previous driver context
	 */
	stored_dm = prt_dlg.hDevMode;
	stored_devn = prt_dlg.hDevNames;
	stored_nFlags = prt_dlg.Flags;
	stored_nCopies = prt_dlg.nCopies;
    }

    ಯದಿ (prt_dlg.hDC == ರಿಕ್ತ)
    {
	emsg(_(e_printer_selection_failed));
	mch_print_cleanup();
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    // Not all printer drivers report the support of color (or grey) in the
    // same way.  Let's set has_color if there appears to be some way to print
    // more than B&W.
    i = GetDeviceCaps(prt_dlg.hDC, NUMCOLORS);
    psettings->has_color = (GetDeviceCaps(prt_dlg.hDC, BITSPIXEL) > 1
				   || GetDeviceCaps(prt_dlg.hDC, PLANES) > 1
				   || i > 2 || i == -1);

    // Ensure all font styles are baseline aligned
    SetTextAlign(prt_dlg.hDC, TA_BASELINE|TA_ಎಡ);

    /*
     * On some windows systems the nCopies parameter is not
     * passed back correctly. It must be retrieved from the
     * hDevMode struct.
     */
    mem = (DEVMODEW *)GlobalLock(prt_dlg.hDevMode);
    ಯದಿ (mem != ರಿಕ್ತ)
    {
	ಯದಿ (mem->dmCopies != 1)
	    stored_nCopies = mem->dmCopies;
	ಯದಿ ((mem->dmFields & DM_DUPLEX) && (mem->dmDuplex & ~DMDUP_SIMPLEX))
	    psettings->duplex = TRUE;
	ಯದಿ ((mem->dmFields & DM_COLOR) && (mem->dmColor & DMCOLOR_COLOR))
	    psettings->has_color = TRUE;
    }
    GlobalUnlock(prt_dlg.hDevMode);

    devname = (DEVNAMES *)GlobalLock(prt_dlg.hDevNames);
    ಯದಿ (devname != 0)
    {
	WCHAR	*wprinter_name = (WCHAR *)devname + devname->wDeviceOffset;
	WCHAR	*wport_name = (WCHAR *)devname + devname->wOutputOffset;
	char_u	*text = (char_u *)_("to %s on %s");
	char_u  *printer_name = utf16_to_enc(wprinter_name, ರಿಕ್ತ);
	char_u	*port_name = utf16_to_enc(wport_name, ರಿಕ್ತ);

	ಯದಿ (printer_name != ರಿಕ್ತ && port_name != ರಿಕ್ತ)
	    prt_name = alloc(STRLEN(printer_name)
					   + STRLEN(port_name) + STRLEN(text));
	ಯದಿ (prt_name != ರಿಕ್ತ)
	    wsprintf((ವರ್ಣ *)prt_name, (ಧ್ರುವ ವರ್ಣ *)text,
		    printer_name, port_name);
	vim_free(printer_name);
	vim_free(port_name);
    }
    GlobalUnlock(prt_dlg.hDevNames);

    /*
     * Initialise the font according to 'printfont'
     */
    CLEAR_FIELD(fLogFont);
    ಯದಿ (get_logfont(&fLogFont, p_pfn, prt_dlg.hDC, TRUE) == FAIL)
    {
	semsg(_(e_unknown_printer_font_str), p_pfn);
	mch_print_cleanup();
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    ಪ್ರತಿ (pifBold = 0; pifBold <= 1; pifBold++)
	ಪ್ರತಿ (pifItalic = 0; pifItalic <= 1; pifItalic++)
	    ಪ್ರತಿ (pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
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
    psettings->chars_per_ಸಾಲು = prt_get_cpl();
    psettings->ಸಾಲುಗಳು_per_page = prt_get_lpp();
    ಯದಿ (prt_dlg.Flags & PD_USEDEVMODECOPIESANDCOLLATE)
    {
	psettings->n_collated_copies = (prt_dlg.Flags & PD_COLLATE)
						    ? prt_dlg.nCopies : 1;
	psettings->n_uncollated_copies = (prt_dlg.Flags & PD_COLLATE)
						    ? 1 : prt_dlg.nCopies;

	ಯದಿ (psettings->n_collated_copies == 0)
	    psettings->n_collated_copies = 1;

	ಯದಿ (psettings->n_uncollated_copies == 0)
	    psettings->n_uncollated_copies = 1;
    }
    ಅನ್ಯಥಾ
    {
	psettings->n_collated_copies = 1;
	psettings->n_uncollated_copies = 1;
    }

    psettings->jobname = jobname;

    ಪ್ರತ್ಯಾಗಮ TRUE;

init_fail_dlg:
    err = CommDlgExtendedError();
    ಯದಿ (err)
    {
	char_u *buf;

	// I suspect FormatMessage() doesn't work for values returned by
	// CommDlgExtendedError().  What does?
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		ರಿಕ್ತ, err, 0, (LPTSTR)(&buf), 0, ರಿಕ್ತ);
	semsg(_(e_print_error_str),
		buf == ರಿಕ್ತ ? (char_u *)_("Unknown") : buf);
	LocalFree((LPVOID)(buf));
    }
    ಅನ್ಯಥಾ
	ಸಂದೇಶ_clr_eos(); // Maybe canceled

    mch_print_cleanup();
    ಪ್ರತ್ಯಾಗಮ FALSE;
}


    ಪೂರ್ಣಾಙ್ಕ
mch_print_begin(prt_settings_T *psettings)
{
    ಪೂರ್ಣಾಙ್ಕ			ret = 0;
    ವರ್ಣ		szBuffer[300];
    WCHAR		*wp;

    hDlgPrint = CreateDialog(g_hinst, TEXT("PrintDlgBox"),
					     prt_dlg.hwndOwner, PrintDlgProc);
    SetAbortProc(prt_dlg.hDC, AbortProc);
    wsprintf(szBuffer, _("Printing '%s'"), gettail(psettings->jobname));
    vimSetDlgItemText(hDlgPrint, IDC_PRINTTEXT1, (char_u *)szBuffer);

    wp = enc_to_utf16(psettings->jobname, ರಿಕ್ತ);
    ಯದಿ (wp != ರಿಕ್ತ)
    {
	DOCINFOW	di;

	CLEAR_FIELD(di);
	di.cbSize = ಪರಿಮಾಣ(di);
	di.lpszDocName = wp;
	ret = StartDocW(prt_dlg.hDC, &di);
	vim_free(wp);
    }

# ifdef FEAT_GUI
    // Give focus back to main window (when using MDI).
#  ifdef VIMDLL
    ಯದಿ (gui.in_use)
#  endif
	SetFocus(s_hwnd);
# endif

    ಪ್ರತ್ಯಾಗಮ (ret > 0);
}

    ಶೂನ್ಯ
mch_print_end(prt_settings_T *psettings UNUSED)
{
    EndDoc(prt_dlg.hDC);
    ಯದಿ (!*bUserAbort && hDlgPrint != ರಿಕ್ತ)
	SendMessage(hDlgPrint, WM_COMMAND, 0, 0);
}

    ಪೂರ್ಣಾಙ್ಕ
mch_print_end_page(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (EndPage(prt_dlg.hDC) > 0);
}

    ಪೂರ್ಣಾಙ್ಕ
mch_print_begin_page(char_u *ಸಂದೇಶ)
{
    ಯದಿ (ಸಂದೇಶ != ರಿಕ್ತ)
	vimSetDlgItemText(hDlgPrint, IDC_PROGRESS, ಸಂದೇಶ);
    ಪ್ರತ್ಯಾಗಮ (StartPage(prt_dlg.hDC) > 0);
}

    ಪೂರ್ಣಾಙ್ಕ
mch_print_blank_page(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (mch_print_begin_page(ರಿಕ್ತ) ? (mch_print_end_page()) : FALSE);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prt_pos_x = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prt_pos_y = 0;

    ಶೂನ್ಯ
mch_print_start_ಸಾಲು(ಪೂರ್ಣಾಙ್ಕ margin, ಪೂರ್ಣಾಙ್ಕ page_ಸಾಲು)
{
    ಯದಿ (margin)
	prt_pos_x = -prt_ಸಂಖ್ಯೆ_width;
    ಅನ್ಯಥಾ
	prt_pos_x = 0;
    prt_pos_y = page_ಸಾಲು * prt_ಸಾಲು_height
				 + prt_tm.tmAscent + prt_tm.tmExternalLeading;
}

    ಪೂರ್ಣಾಙ್ಕ
mch_print_text_out(char_u *p, ಪೂರ್ಣಾಙ್ಕ len)
{
    SIZE	sz;
    WCHAR	*wp;
    ಪೂರ್ಣಾಙ್ಕ		wlen = len;
    ಪೂರ್ಣಾಙ್ಕ		ret = FALSE;

    wp = enc_to_utf16(p, &wlen);
    ಯದಿ (wp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    TextOutW(prt_dlg.hDC, prt_pos_x + prt_ಎಡ_margin,
	    prt_pos_y + prt_ತುದಿ_margin, wp, wlen);
    GetTextExtentPoint32W(prt_dlg.hDC, wp, wlen, &sz);
    vim_free(wp);
    prt_pos_x += (sz.cx - prt_tm.tmOverhang);
    // This is wrong when printing spaces for a TAB.
    ಯದಿ (p[len] != NUL)
    {
	wlen = mb_ptr2len(p + len);
	wp = enc_to_utf16(p + len, &wlen);
	ಯದಿ (wp != ರಿಕ್ತ)
	{
	    GetTextExtentPoint32W(prt_dlg.hDC, wp, 1, &sz);
	    ret = (prt_pos_x + prt_ಎಡ_margin + sz.cx > prt_ಬಲ_margin);
	    vim_free(wp);
	}
    }
    ಪ್ರತ್ಯಾಗಮ ret;
}

    ಶೂನ್ಯ
mch_print_set_font(ಪೂರ್ಣಾಙ್ಕ iBold, ಪೂರ್ಣಾಙ್ಕ iItalic, ಪೂರ್ಣಾಙ್ಕ iUnderline)
{
    SelectObject(prt_dlg.hDC, prt_font_handles[iBold][iItalic][iUnderline]);
}

    ಶೂನ್ಯ
mch_print_set_bg(long_u bgcol)
{
    SetBkColor(prt_dlg.hDC, GetNearestColor(prt_dlg.hDC,
						   swap_me((COLORREF)bgcol)));
    /*
     * With a white background we can draw characters transparent, which is
     * good for italic characters that overlap to the next char cell.
     */
    ಯದಿ (bgcol == 0xffffffUL)
	SetBkMode(prt_dlg.hDC, TRANSPARENT);
    ಅನ್ಯಥಾ
	SetBkMode(prt_dlg.hDC, OPAQUE);
}

    ಶೂನ್ಯ
mch_print_set_fg(long_u fgcol)
{
    SetTextColor(prt_dlg.hDC, GetNearestColor(prt_dlg.hDC,
						   swap_me((COLORREF)fgcol)));
}

#endif // FEAT_PRINTER && !FEAT_POSTSCRIPT



#ಯದಿ defined(FEAT_SHORTCUT)
# include <shlobj.h>

# define is_path_sep(c)	    ((c) == L'\\' || (c) == L'/')

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
is_reparse_point_included(LPCWSTR fname)
{
    LPCWSTR	p = fname, q;
    WCHAR	buf[MAX_PATH];
    DWORD	attr;

    ಯದಿ (SAFE_isalpha(p[0]) && p[1] == L':' && is_path_sep(p[2]))
	p += 3;
    ಅನ್ಯಥಾ ಯದಿ (is_path_sep(p[0]) && is_path_sep(p[1]))
	p += 2;

    ಯಾವತ್ (*p != L'\0')
    {
	q = wcspbrk(p, L"\\/");
	ಯದಿ (q == ರಿಕ್ತ)
	    p = q = fname + wcslen(fname);
	ಅನ್ಯಥಾ
	    p = q + 1;
	ಯದಿ (q - fname >= MAX_PATH)
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	wcsncpy(buf, fname, q - fname);
	buf[q - fname] = L'\0';
	attr = GetFileAttributesW(buf);
	ಯದಿ (attr != INVALID_FILE_ATTRIBUTES
		&& (attr & FILE_ATTRIBUTE_REPARSE_POINT) != 0)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Return the resolved file path, NULL if "fname" is an AppExecLink reparse
 * point, already fully resolved, or it doesn't exists.
 */
    char_u *
resolve_reparse_point(char_u *fname)
{
    HANDLE	    h = INVALID_HANDLE_ಮೌಲ್ಯ;
    DWORD	    size;
    WCHAR	    *p, *wp;
    char_u	    *rfname = ರಿಕ್ತ;
    WCHAR	    *buff = ರಿಕ್ತ;

    p = enc_to_utf16(fname, ರಿಕ್ತ);
    ಯದಿ (p == ರಿಕ್ತ)
	ಗಚ್ಛ fail;

    ಯದಿ (!is_reparse_point_included(p))
    {
	vim_free(p);
	ಗಚ್ಛ fail;
    }

    h = CreateFileW(p, 0, 0, ರಿಕ್ತ, OPEN_EXISTING,
	    FILE_FLAG_BACKUP_SEMANTICS, ರಿಕ್ತ);
    vim_free(p);

    ಯದಿ (h == INVALID_HANDLE_ಮೌಲ್ಯ)
	ಗಚ್ಛ fail;

    size = GetFinalPathNameByHandleW(h, ರಿಕ್ತ, 0, 0);
    ಯದಿ (size == 0)
	ಗಚ್ಛ fail;
    buff = ALLOC_MULT(WCHAR, size);
    ಯದಿ (buff == ರಿಕ್ತ)
	ಗಚ್ಛ fail;
    ಯದಿ (GetFinalPathNameByHandleW(h, buff, size, 0) == 0)
	ಗಚ್ಛ fail;

    ಯದಿ (wcsncmp(buff, L"\\\\?\\UNC\\", 8) == 0)
    {
	buff[6] = L'\\';
	wp = buff + 6;
    }
    ಅನ್ಯಥಾ ಯದಿ (wcsncmp(buff, L"\\\\?\\", 4) == 0)
	wp = buff + 4;
    ಅನ್ಯಥಾ
	wp = buff;

    rfname = utf16_to_enc(wp, ರಿಕ್ತ);

fail:
    ಯದಿ (h != INVALID_HANDLE_ಮೌಲ್ಯ)
	CloseHandle(h);
    ಯದಿ (buff != ರಿಕ್ತ)
	vim_free(buff);

    ಪ್ರತ್ಯಾಗಮ rfname;
}

/*
 * When "fname" is the name of a shortcut (*.lnk) resolve the file it points
 * to and return that name in allocated memory.
 * Otherwise NULL is returned.
 */
    ಸ್ಥಿರ char_u *
resolve_shortcut(char_u *fname)
{
    HRESULT		hr;
    IShellLink		*psl = ರಿಕ್ತ;
    IPersistFile	*ppf = ರಿಕ್ತ;
    OLECHAR		wsz[MAX_PATH];
    char_u		*rfname = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ			len;
    IShellLinkW		*pslw = ರಿಕ್ತ;
    WIN32_FIND_DATAW	ffdw; // we get those free of charge

    // Check if the file name ends in ".lnk". Avoid calling
    // CoCreateInstance(), it's quite slow.
    ಯದಿ (fname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ rfname;
    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(fname);
    ಯದಿ (len <= 4 || STRNICMP(fname + len - 4, ".lnk", 4) != 0)
	ಪ್ರತ್ಯಾಗಮ rfname;

    CoInitialize(ರಿಕ್ತ);

    // create a link manager object and request its interface
    hr = CoCreateInstance(
	    &CLSID_ShellLink, ರಿಕ್ತ, CLSCTX_INPROC_SERVER,
	    &IID_IShellLinkW, (ಶೂನ್ಯ**)&pslw);
    ಯದಿ (hr == S_OK)
    {
	WCHAR	*p = enc_to_utf16(fname, ರಿಕ್ತ);

	ಯದಿ (p != ರಿಕ್ತ)
	{
	    // Get a pointer to the IPersistFile interface.
	    hr = pslw->lpVtbl->QueryInterface(
		    pslw, &IID_IPersistFile, (ಶೂನ್ಯ**)&ppf);
	    ಯದಿ (hr != S_OK)
		ಗಚ್ಛ shortcut_errorw;

	    // "load" the name and resolve the link
	    hr = ppf->lpVtbl->Load(ppf, p, STGM_READ);
	    ಯದಿ (hr != S_OK)
		ಗಚ್ಛ shortcut_errorw;
# ಯದಿ 0  // This makes Vim wait a long time if the target does not exist.
	    hr = pslw->lpVtbl->Resolve(pslw, ರಿಕ್ತ, SLR_NO_UI);
	    ಯದಿ (hr != S_OK)
		ಗಚ್ಛ shortcut_errorw;
# endif

	    // Get the path to the link target.
	    ZeroMemory(wsz, MAX_PATH * ಪರಿಮಾಣ(WCHAR));
	    hr = pslw->lpVtbl->GetPath(pslw, wsz, MAX_PATH, &ffdw, 0);
	    ಯದಿ (hr == S_OK && wsz[0] != NUL)
		rfname = utf16_to_enc(wsz, ರಿಕ್ತ);

shortcut_errorw:
	    vim_free(p);
	}
    }

    // Release all interface pointers (both belong to the same object)
    ಯದಿ (ppf != ರಿಕ್ತ)
	ppf->lpVtbl->Release(ppf);
    ಯದಿ (psl != ರಿಕ್ತ)
	psl->lpVtbl->Release(psl);
    ಯದಿ (pslw != ರಿಕ್ತ)
	pslw->lpVtbl->Release(pslw);

    CoUninitialize();
    ಪ್ರತ್ಯಾಗಮ rfname;
}

    char_u *
mch_resolve_path(char_u *fname, ಪೂರ್ಣಾಙ್ಕ reparse_point)
{
    char_u  *path = resolve_shortcut(fname);

    ಯದಿ (path == ರಿಕ್ತ && reparse_point)
	path = resolve_reparse_point(fname);
    ಪ್ರತ್ಯಾಗಮ path;
}
#endif

#ಯದಿ defined(FEAT_EVAL) && (!defined(FEAT_GUI) || defined(VIMDLL))
/*
 * Bring ourselves to the foreground.  Does work if the OS doesn't allow it.
 */
    ಶೂನ್ಯ
win32_set_foreground(ಶೂನ್ಯ)
{
    GetConsoleHwnd();	    // get value of s_hwnd
    ಯದಿ (s_hwnd != 0)
	SetForegroundWindow(s_hwnd);
}
#endif

#ಯದಿ defined(FEAT_CLIENTSERVER)
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
# define VIM_CLASSNAME_LEN  (ಪರಿಮಾಣ(VIM_CLASSNAME) - 1)

// Timeout for sending a message to another Vim instance.  Normally this works
// instantly, but it may hang when the other Vim instance is halted.
# define SENDMESSAGE_TIMEOUT	(5 * 1000)

// Communication is via WM_COPYDATA messages. The message type is sent in
// the dwData parameter. Types are defined here.
# define COPYDATA_ಕೀಲಿಗಳು		0
# define COPYDATA_REPLY		1
# define COPYDATA_ಅಭಿವ್ಯಕ್ತಿ		10
# define COPYDATA_RESULT	11
# define COPYDATA_ERROR_RESULT	12
# define COPYDATA_ENCODING	20

// This is a structure containing a server HWND and its name.
ಸಂರಚನಾ server_id
{
    HWND hwnd;
    char_u *name;
};

// Last received 'encoding' that the client uses.
ಸ್ಥಿರ char_u	*client_enc = ರಿಕ್ತ;

/*
 * Tell the other side what encoding we are using.
 * Return -1 if timeout happens.  Other errors are ignored.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
serverSendEnc(HWND target)
{
    COPYDATASTRUCT data;

    data.dwData = COPYDATA_ENCODING;
    data.cbData = (DWORD)STRLEN(p_enc) + 1;
    data.lpData = p_enc;
    ಯದಿ (SendMessageTimeout(target, WM_COPYDATA,
	    (WPARAM)message_window, (LPARAM)&data,
	    SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, ರಿಕ್ತ) == 0)
	ಪ್ರತ್ಯಾಗಮ -1;
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Clean up on exit. This destroys the hidden message window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
CleanUpMessaging(ಶೂನ್ಯ)
{
    ಯದಿ (message_window == 0)
	ಪ್ರತ್ಯಾಗಮ;

    DestroyWindow(message_window);
    message_window = 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ save_reply(HWND server, char_u *reply, ಪೂರ್ಣಾಙ್ಕ ಅಭಿವ್ಯಕ್ತಿ);

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
    ಸ್ಥಿರ LRESULT CALLBACK
Messaging_WndProc(HWND hwnd, UINT ಸಂದೇಶ, WPARAM wParam, LPARAM lParam)
{
    ಯದಿ (ಸಂದೇಶ == WM_COPYDATA)
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
	ಪೂರ್ಣಾಙ್ಕ		retval;
	DWORD_PTR	dwret = 0;
	char_u		*str;
	char_u		*tofree;

	ವಿಕಲ್ಪ (data->dwData)
	{
	ಪಕ್ಷ COPYDATA_ENCODING:
	    // Remember the encoding that the client uses.
	    vim_free(client_enc);
	    client_enc = enc_canonize((char_u *)data->lpData);
	    ಪ್ರತ್ಯಾಗಮ 1;

	ಪಕ್ಷ COPYDATA_ಕೀಲಿಗಳು:
	    // Remember who sent this, for <client>
	    clientWindow = sender;

	    // Add the received keys to the input buffer.
	    str = serverConvert(client_enc, (char_u *)data->lpData, &tofree);
# ifdef FEAT_GUI
	    ಯದಿ (gui.in_use)
		// The GUI may get here re-entrantly, so insert the keys later.
		server_add_ಒಳಹರಿವು(str);
	    ಅನ್ಯಥಾ
# endif
		server_to_ಒಳಹರಿವು_buf(str);
	    vim_free(tofree);

# ifdef FEAT_GUI
	    // Wake up the main GUI loop.
#  ifdef VIMDLL
	    ಯದಿ (gui.in_use)
#  endif
		ಯದಿ (s_hwnd != 0)
		    PostMessage(s_hwnd, WM_NULL, 0, 0);
# endif
	    ಪ್ರತ್ಯಾಗಮ 1;

	ಪಕ್ಷ COPYDATA_ಅಭಿವ್ಯಕ್ತಿ:
	    // Remember who sent this, for <client>
	    clientWindow = sender;

	    str = serverConvert(client_enc, (char_u *)data->lpData, &tofree);
	    res = eval_client_ಅಭಿವ್ಯಕ್ತಿ_to_string(str);

	    ಯದಿ (res == ರಿಕ್ತ)
	    {
		ವರ್ಣ	*err = _(e_invalid_expression_received);
		size_t	len = STRLEN(str) + STRLEN(err) + 5;

		res = alloc(len);
		ಯದಿ (res != ರಿಕ್ತ)
		    reply.cbData = (DWORD)vim_snprintf_safelen(
			(ವರ್ಣ *)res, len, "%s: \"%s\"", err, str);
		ಅನ್ಯಥಾ
		    reply.cbData = 0;
		reply.dwData = COPYDATA_ERROR_RESULT;
	    }
	    ಅನ್ಯಥಾ
	    {
		reply.cbData = (DWORD)STRLEN(res) + 1;
		reply.dwData = COPYDATA_RESULT;
	    }
	    reply.lpData = res;

	    ಯದಿ (serverSendEnc(sender) < 0)
		retval = -1;
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (SendMessageTimeout(sender, WM_COPYDATA,
			(WPARAM)message_window, (LPARAM)&reply,
			SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
		    retval = -1;
		ಅನ್ಯಥಾ
		    retval = (ಪೂರ್ಣಾಙ್ಕ)dwret;
	    }
	    vim_free(tofree);
	    vim_free(res);
	    ಪ್ರತ್ಯಾಗಮ retval;

	ಪಕ್ಷ COPYDATA_REPLY:
	ಪಕ್ಷ COPYDATA_RESULT:
	ಪಕ್ಷ COPYDATA_ERROR_RESULT:
	    ಯದಿ (data->lpData != ರಿಕ್ತ)
	    {
		str = serverConvert(client_enc, (char_u *)data->lpData,
								     &tofree);
		ಯದಿ (tofree == ರಿಕ್ತ)
		    str = vim_strsave(str);
		ಯದಿ (save_reply(sender, str,
			   (data->dwData == COPYDATA_REPLY ?  0 :
			   (data->dwData == COPYDATA_RESULT ? 1 :
							      2))) == FAIL)
		    vim_free(str);
		ಅನ್ಯಥಾ ಯದಿ (data->dwData == COPYDATA_REPLY)
		{
		    char_u	winstr[30];

		    sprintf((ವರ್ಣ *)winstr, PRINTF_HEX_LONG_U, (long_u)sender);
		    apply_autocmds(EVENT_REMOTEREPLY, winstr, str,
								TRUE, curbuf);
		}
	    }
	    ಪ್ರತ್ಯಾಗಮ 1;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಅನ್ಯಥಾ ಯದಿ (ಸಂದೇಶ == WM_ACTIVATE && wParam == WA_ACTIVE)
    {
	// When the message window is activated (brought to the foreground),
	// this actually applies to the text window.
# ಯದಿ !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	ಯದಿ (!gui.in_use)
#  endif
	    GetConsoleHwnd();	    // get value of s_hwnd
# endif
	ಯದಿ (s_hwnd != 0)
	{
	    SetForegroundWindow(s_hwnd);
	    ಪ್ರತ್ಯಾಗಮ 0;
	}
    }

    ಪ್ರತ್ಯಾಗಮ DefWindowProc(hwnd, ಸಂದೇಶ, wParam, lParam);
}

/*
 * Initialise the message handling process.  This involves creating a window
 * to handle messages - the window will not be visible.
 */
    ಶೂನ್ಯ
serverInitMessaging(ಶೂನ್ಯ)
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
    wndclass.hIcon = ರಿಕ್ತ;
    wndclass.hCursor = ರಿಕ್ತ;
    wndclass.hbrBackground = ರಿಕ್ತ;
    wndclass.lpszMenuName = ರಿಕ್ತ;
    wndclass.lpszClassName = VIM_CLASSNAME;
    RegisterClass(&wndclass);

    // Create the message window. It will be hidden, so the details don't
    // matter.  Don't use WS_OVERLAPPEDWINDOW, it will make a shortcut remove
    // focus from gvim.
    message_window = CreateWindow(VIM_CLASSNAME, "",
			 WS_POPUPWINDOW | WS_CAPTION,
			 CW_USEDEFAULT, CW_USEDEFAULT,
			 100, 100, ರಿಕ್ತ, ರಿಕ್ತ,
			 g_hinst, ರಿಕ್ತ);
}

// Used by serverSendToVim() to find an alternate server name.
ಸ್ಥಿರ char_u *altname_buf_ptr = ರಿಕ್ತ;

/*
 * Get the title of the window "hwnd", which is the Vim server name, in
 * "name[namelen]" and return the length.
 * Returns zero if window "hwnd" is not a Vim server.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
getVimServerName(HWND hwnd, ವರ್ಣ *name, ಪೂರ್ಣಾಙ್ಕ namelen)
{
    ಪೂರ್ಣಾಙ್ಕ		len;
    ವರ್ಣ	buffer[VIM_CLASSNAME_LEN + 1];

    // Ignore windows which aren't Vim message windows
    len = GetClassName(hwnd, buffer, ಪರಿಮಾಣ(buffer));
    ಯದಿ (len != VIM_CLASSNAME_LEN || STRCMP(buffer, VIM_CLASSNAME) != 0)
	ಪ್ರತ್ಯಾಗಮ 0;

    // Get the title of the window
    ಪ್ರತ್ಯಾಗಮ GetWindowText(hwnd, name, namelen);
}

    ಸ್ಥಿರ BOOL CALLBACK
enumWindowsGetServer(HWND hwnd, LPARAM lparam)
{
    ಸಂರಚನಾ	server_id *id = (ಸಂರಚನಾ server_id *)lparam;
    ವರ್ಣ	server[MAX_PATH];

    // Get the title of the window
    ಯದಿ (getVimServerName(hwnd, server, ಪರಿಮಾಣ(server)) == 0)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    // If this is the server we're looking for, return its HWND
    ಯದಿ (STRICMP(server, id->name) == 0)
    {
	id->hwnd = hwnd;
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    // If we are looking for an alternate server, remember this name.
    ಯದಿ (altname_buf_ptr != ರಿಕ್ತ)
    {
	size_t	namelen = STRLEN(id->name);

	ಯದಿ (STRNICMP(server, id->name, namelen) == 0
	    && vim_isdigit(server[namelen]))
	{
	    STRCPY(altname_buf_ptr, server);
	    altname_buf_ptr = ರಿಕ್ತ;	    // don't use another name
	}
    }

    // Otherwise, keep looking
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

    ಸ್ಥಿರ BOOL CALLBACK
enumWindowsGetNames(HWND hwnd, LPARAM lparam)
{
    list_T	*list = (list_T *)lparam;
    ವರ್ಣ	server[MAX_PATH];

    // Get the title of the window
    ಯದಿ (getVimServerName(hwnd, server, ಪರಿಮಾಣ(server)) == 0)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    list_append_string(list, (char_u *)server, -1);
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

ಸಂರಚನಾ enum_windows_s
{
    WNDENUMPROC lpEnumFunc;
    LPARAM      lParam;
};

    ಸ್ಥಿರ BOOL CALLBACK
enum_windows_child(HWND hwnd, LPARAM lParam)
{
    ಸಂರಚನಾ enum_windows_s *ew = (ಸಂರಚನಾ enum_windows_s *)lParam;

    ಪ್ರತ್ಯಾಗಮ (ew->lpEnumFunc)(hwnd, ew->lParam);
}

    ಸ್ಥಿರ BOOL CALLBACK
enum_windows_toplevel(HWND hwnd, LPARAM lParam)
{
    ಸಂರಚನಾ enum_windows_s *ew = (ಸಂರಚನಾ enum_windows_s *)lParam;

    ಯದಿ ((ew->lpEnumFunc)(hwnd, ew->lParam))
	ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ EnumChildWindows(hwnd, enum_windows_child, lParam);
}

/*
 * Enumerate all windows including children.
 */
    ಸ್ಥಿರ BOOL
enum_windows(WNDENUMPROC lpEnumFunc, LPARAM lParam)
{
    ಸಂರಚನಾ enum_windows_s ew;

    ew.lpEnumFunc = lpEnumFunc;
    ew.lParam = lParam;
    ಪ್ರತ್ಯಾಗಮ EnumWindows(enum_windows_toplevel, (LPARAM)&ew);
}

    ಸ್ಥಿರ HWND
findServer(char_u *name)
{
    ಸಂರಚನಾ server_id id;

    id.name = name;
    id.hwnd = 0;

    enum_windows(enumWindowsGetServer, (LPARAM)(&id));

    ಪ್ರತ್ಯಾಗಮ id.hwnd;
}

    ಶೂನ್ಯ
serverSetName(char_u *name)
{
    size_t	namelen;
    char_u	*ok_name;
    HWND	hwnd = 0;
    ಪೂರ್ಣಾಙ್ಕ		i = 0;
    char_u	*p;

    // Leave enough space for a 9-digit suffix to ensure uniqueness!
    namelen = STRLEN(name);
    ok_name = alloc(namelen + 10);
    ಯದಿ (ok_name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    STRCPY(ok_name, name);
    p = ok_name + namelen;

    ಪ್ರತಿ (;;)
    {
	// This is inefficient - we're doing an EnumWindows loop for each
	// possible name. It would be better to grab all names in one go,
	// and scan the list each time...
	hwnd = findServer(ok_name);
	ಯದಿ (hwnd == 0)
	    ಭಙ್ಗ;

	++i;
	ಯದಿ (i >= 1000)
	    ಭಙ್ಗ;

	sprintf((ವರ್ಣ *)p, "%d", i);
    }

    ಯದಿ (hwnd != 0)
	vim_free(ok_name);
    ಅನ್ಯಥಾ
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
serverGetVimNames(ಶೂನ್ಯ)
{
    list_T *list = list_alloc();

    ಯದಿ (list == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    enum_windows(enumWindowsGetNames, (LPARAM)list);

    ಪ್ರತ್ಯಾಗಮ list;
}

    ಪೂರ್ಣಾಙ್ಕ
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
    sscanf((ವರ್ಣ *)name, SCANF_HEX_LONG_U, &n);
    ಯದಿ (n == 0)
	ಪ್ರತ್ಯಾಗಮ -1;

    target = (HWND)n;
    ಯದಿ (!IsWindow(target))
	ಪ್ರತ್ಯಾಗಮ -1;

    data.dwData = COPYDATA_REPLY;
    data.cbData = (DWORD)STRLEN(reply) + 1;
    data.lpData = reply;

    ಯದಿ (serverSendEnc(target) < 0)
	ಪ್ರತ್ಯಾಗಮ -1;
    ಯದಿ (SendMessageTimeout(target, WM_COPYDATA,
		(WPARAM)message_window, (LPARAM)&data,
		SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
	ಪ್ರತ್ಯಾಗಮ -1;
    ಪ್ರತ್ಯಾಗಮ dwret ? 0 : -1;
}

    ಪೂರ್ಣಾಙ್ಕ
serverSendToVim(
    char_u	 *name,			// Where to send.
    char_u	 *cmd,			// What to send.
    char_u	 **result,		// Result of eval'ed expression
    ಶೂನ್ಯ	 *ptarget,		// HWND of server
    ಪೂರ್ಣಾಙ್ಕ		 as_ಅಭಿವ್ಯಕ್ತಿ,		// Expression or keys?
    ಪೂರ್ಣಾಙ್ಕ		 timeout,		// timeout in seconds or zero
    ಪೂರ್ಣಾಙ್ಕ		 silent)		// don't complain about no server
{
    size_t	namelen;
    HWND	target;
    COPYDATASTRUCT data;
    char_u	*retval = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		retcode = 0;
    DWORD_PTR	dwret = 0;
    char_u	altname_buf[MAX_PATH];

    // Execute locally if no display or target is ourselves
    ಯದಿ (serverName != ರಿಕ್ತ && STRICMP(name, serverName) == 0)
	ಪ್ರತ್ಯಾಗಮ sendToLocalVim(cmd, as_ಅಭಿವ್ಯಕ್ತಿ, result);

    // If the server name does not end in a digit then we look for an
    // alternate name.  e.g. when "name" is GVIM then we may find GVIM2.
    namelen = STRLEN(name);
    ಯದಿ (namelen > 1 && !vim_isdigit(name[namelen - 1]))
	altname_buf_ptr = altname_buf;
    altname_buf[0] = NUL;
    target = findServer(name);
    altname_buf_ptr = ರಿಕ್ತ;
    ಯದಿ (target == 0 && altname_buf[0] != NUL)
	// Use another server name we found.
	target = findServer(altname_buf);

    ಯದಿ (target == 0)
    {
	ಯದಿ (!silent)
	    semsg(_(e_no_registered_server_named_str), name);
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಯದಿ (ptarget)
	*(HWND *)ptarget = target;

    data.dwData = as_ಅಭಿವ್ಯಕ್ತಿ ? COPYDATA_ಅಭಿವ್ಯಕ್ತಿ : COPYDATA_ಕೀಲಿಗಳು;
    data.cbData = (DWORD)STRLEN(cmd) + 1;
    data.lpData = cmd;

    ಯದಿ (serverSendEnc(target) < 0)
	ಪ್ರತ್ಯಾಗಮ -1;
    ಯದಿ (SendMessageTimeout(target, WM_COPYDATA,
		(WPARAM)message_window, (LPARAM)&data,
		SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
	ಪ್ರತ್ಯಾಗಮ -1;
    ಯದಿ (dwret == 0)
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯದಿ (as_ಅಭಿವ್ಯಕ್ತಿ)
	retval = serverGetReply(target, &retcode, TRUE, TRUE, timeout);

    ಯದಿ (result == ರಿಕ್ತ)
	vim_free(retval);
    ಅನ್ಯಥಾ
	*result = retval; // Caller assumes responsibility for freeing

    ಪ್ರತ್ಯಾಗಮ retcode;
}

/*
 * Bring the server to the foreground.
 */
    ಶೂನ್ಯ
serverForeground(char_u *name)
{
    HWND	target = findServer(name);

    ಯದಿ (target != 0)
	SetForegroundWindow(target);
}

// Replies from server need to be stored until the client picks them up via
// remote_read(). So we maintain a list of server-id/reply pairs.
// Note that there could be multiple replies from one server pending if the
// client is slow picking them up.
// We just store the replies in a simple list. When we remove an entry, we
// move list entries down to fill the gap.
// The server ID is simply the HWND.
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    HWND	server;		// server window
    char_u	*reply;		// reply string
    ಪೂರ್ಣಾಙ್ಕ		ಅಭಿವ್ಯಕ್ತಿ_result;	// 0 for REPLY, 1 for RESULT 2 for error
} reply_T;

ಸ್ಥಿರ garray_T reply_list = {0, 0, ಪರಿಮಾಣ(reply_T), 5, 0};

# define REPLY_ITEM(i) ((reply_T *)(reply_list.ga_data) + (i))
# define REPLY_ಎಣಿಕೆ (reply_list.ga_len)

// Flag which is used to wait for a reply
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ reply_received = 0;

/*
 * Store a reply.  "reply" must be allocated memory (or NULL).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
save_reply(HWND server, char_u *reply, ಪೂರ್ಣಾಙ್ಕ ಅಭಿವ್ಯಕ್ತಿ)
{
    reply_T *rep;

    ಯದಿ (ga_grow(&reply_list, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    rep = REPLY_ITEM(REPLY_ಎಣಿಕೆ);
    rep->server = server;
    rep->reply = reply;
    rep->ಅಭಿವ್ಯಕ್ತಿ_result = ಅಭಿವ್ಯಕ್ತಿ;
    ಯದಿ (rep->reply == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ++REPLY_ಎಣಿಕೆ;
    reply_received = 1;
    ಪ್ರತ್ಯಾಗಮ OK;
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
serverGetReply(HWND server, ಪೂರ್ಣಾಙ್ಕ *ಅಭಿವ್ಯಕ್ತಿ_res, ಪೂರ್ಣಾಙ್ಕ remove, ಪೂರ್ಣಾಙ್ಕ wait, ಪೂರ್ಣಾಙ್ಕ timeout)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*reply;
    reply_T	*rep;
    ಪೂರ್ಣಾಙ್ಕ		did_process = FALSE;
    time_t	start;
    time_t	now;

    // When waiting, loop until the message waiting for is received.
    time(&start);
    ಪ್ರತಿ (;;)
    {
	// Reset this here, in case a message arrives while we are going
	// through the already received messages.
	reply_received = 0;

	ಪ್ರತಿ (i = 0; i < REPLY_ಎಣಿಕೆ; ++i)
	{
	    rep = REPLY_ITEM(i);
	    ಯದಿ (rep->server == server
		    && ((rep->ಅಭಿವ್ಯಕ್ತಿ_result != 0) == (ಅಭಿವ್ಯಕ್ತಿ_res != ರಿಕ್ತ)))
	    {
		// Save the values we've found for later
		reply = rep->reply;
		ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ_res != ರಿಕ್ತ)
		    *ಅಭಿವ್ಯಕ್ತಿ_res = rep->ಅಭಿವ್ಯಕ್ತಿ_result == 1 ? 0 : -1;

		ಯದಿ (remove)
		{
		    // Move the rest of the list down to fill the gap
		    mch_memmove(rep, rep + 1,
				     (REPLY_ಎಣಿಕೆ - i - 1) * ಪರಿಮಾಣ(reply_T));
		    --REPLY_ಎಣಿಕೆ;
		}

		// Return the reply to the caller, who takes on responsibility
		// for freeing it if "remove" is TRUE.
		ಪ್ರತ್ಯಾಗಮ reply;
	    }
	}

	// If we got here, we didn't find a reply. Return immediately if the
	// "wait" parameter isn't set.
	ಯದಿ (!wait)
	{
	    // Process pending messages once. Without this, looping on
	    // remote_peek() would never get the reply.
	    ಯದಿ (!did_process)
	    {
		did_process = TRUE;
		serverProcessPendingMessages();
		ಅನುವರ್ತ;
	    }
	    ಭಙ್ಗ;
	}

	// We need to wait for a reply. Enter a message loop until the
	// "reply_received" flag gets set.

	// Loop until we receive a reply
	ಯಾವತ್ (reply_received == 0)
	{
# ifdef FEAT_TIMERS
	    // TODO: use the return value to decide how long to wait.
	    check_due_timer();
# endif
	    time(&now);
	    ಯದಿ (timeout > 0 && (now - start) >= timeout)
		ಭಙ್ಗ;

	    // Wait for a SendMessage() call to us.  This could be the reply
	    // we are waiting for.  Use a timeout of a second, to catch the
	    // situation that the server died unexpectedly.
	    ಸಂದೇಶ_Wait_For_Multiple_Objects(0, ರಿಕ್ತ, TRUE, 1000, QS_ALLINPUT);

	    // If the server has died, give up
	    ಯದಿ (!IsWindow(server))
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	    serverProcessPendingMessages();
	}
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Process any messages in the Windows message queue.
 */
    ಶೂನ್ಯ
serverProcessPendingMessages(ಶೂನ್ಯ)
{
    ಸಂದೇಶ_2 ಸಂದೇಶ;

    ಯಾವತ್ (PeekMessageW(&ಸಂದೇಶ, ರಿಕ್ತ, 0, 0, PM_REMOVE))
    {
	TranslateMessage(&ಸಂದೇಶ);
	DispatchMessageW(&ಸಂದೇಶ);
    }
}

#endif // FEAT_CLIENTSERVER

#ಯದಿ defined(FEAT_GUI) || (defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT))

ಸಂರಚನಾ charset_pair
{
    string_T	name;
    BYTE	charset;
};

ಸ್ಥಿರ ಸಂರಚನಾ charset_pair
charset_pairs[] =
{
    {STR_LITERAL_INIT("ANSI"),		ANSI_CHARSET},
    {STR_LITERAL_INIT("CHINESEBIG5"),	CHINESEBIG5_CHARSET},
    {STR_LITERAL_INIT("DEFAULT"),	DEFAULT_CHARSET},
    {STR_LITERAL_INIT("HANGEUL"),	HANGEUL_CHARSET},
    {STR_LITERAL_INIT("OEM"),		OEM_CHARSET},
    {STR_LITERAL_INIT("SHIFTJIS"),	SHIFTJIS_CHARSET},
    {STR_LITERAL_INIT("SYMBOL"),	ಚಿಹ್ನೆ_CHARSET},
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

ಸಂರಚನಾ quality_pair
{
    string_T	name;
    DWORD	quality;
};

ಸ್ಥಿರ ಸಂರಚನಾ quality_pair
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
    ವರ್ಣ *
charset_id2name(ಪೂರ್ಣಾಙ್ಕ id)
{
    ಪೂರ್ಣಾಙ್ಕ	i;

    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(charset_pairs); ++i)
    {
	ಯದಿ ((BYTE)id == charset_pairs[i].charset)
	    ಪ್ರತ್ಯಾಗಮ (ವರ್ಣ *)charset_pairs[i].name.string;
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Convert a quality ID to a name.
 * Return NULL when not recognized.
 */
    ವರ್ಣ *
quality_id2name(DWORD id)
{
    ಪೂರ್ಣಾಙ್ಕ	i;

    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(quality_pairs); ++i)
    {
	ಯದಿ (id == quality_pairs[i].quality)
	    ಪ್ರತ್ಯಾಗಮ (ವರ್ಣ *)quality_pairs[i].name.string;
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

// The default font height in 100% scaling (96dpi).
// (-16 in 96dpi equates to roughly 12pt)
# define DEFAULT_FONT_HEIGHT	(-16)

ಸ್ಥಿರ ಧ್ರುವ LOGFONTW s_lfDefault =
{
    DEFAULT_FONT_HEIGHT,
    0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
    PROOF_QUALITY, FIXED_PITCH | FF_DONTCARE,
    L""	// Default font name will be set later based on current language.
};

// This will be initialized when set_default_logfont() is called first time.
// The value will be based on the system DPI.
ಪೂರ್ಣಾಙ್ಕ current_font_height = 0;		// also used in gui_w32.c

/*
 * Convert a string representing a point size into pixels. The string should
 * be a positive decimal number, with an optional decimal point (eg, "12", or
 * "10.5"). The pixel value is returned, and a pointer to the next unconverted
 * character is stored in *end. The flag "vertical" says whether this
 * calculation is for a vertical (height) size or a horizontal (width) one.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
points_to_pixels(WCHAR *str, WCHAR **end, ಪೂರ್ಣಾಙ್ಕ vertical, long_i pprinter_dc)
{
    ಪೂರ್ಣಾಙ್ಕ		pixels;
    ಪೂರ್ಣಾಙ್ಕ		points = 0;
    ಪೂರ್ಣಾಙ್ಕ		ಭಾಜಕ = 0;
    HWND	hwnd = (HWND)0;
    HDC		hdc;
    HDC		printer_dc = (HDC)pprinter_dc;

    ಯಾವತ್ (*str != NUL)
    {
	ಯದಿ (*str == L'.' && ಭಾಜಕ == 0)
	{
	    // Start keeping a divisor, for later
	    ಭಾಜಕ = 1;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (!VIM_ISDIGIT(*str))
		ಭಙ್ಗ;

	    points *= 10;
	    points += *str - L'0';
	    ಭಾಜಕ *= 10;
	}
	++str;
    }

    ಯದಿ (ಭಾಜಕ == 0)
	ಭಾಜಕ = 1;

    ಯದಿ (printer_dc == ರಿಕ್ತ)
    {
	hwnd = GetDesktopWindow();
	hdc = GetWindowDC(hwnd);
    }
    ಅನ್ಯಥಾ
	hdc = printer_dc;

    pixels = MulDiv(points,
		    GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX),
		    72 * ಭಾಜಕ);

    ಯದಿ (printer_dc == ರಿಕ್ತ)
	ReleaseDC(hwnd, hdc);

    *end = str;
    ಪ್ರತ್ಯಾಗಮ pixels;
}

/*
 * Convert pixel into point size. This is a reverse of points_to_pixels.
 */
    ಸ್ಥಿರ ದ್ವಿಗುಣ
pixels_to_points(ಪೂರ್ಣಾಙ್ಕ pixels, ಪೂರ್ಣಾಙ್ಕ vertical, long_i pprinter_dc)
{
    ದ್ವಿಗುಣ	points = 0;
    HWND	hwnd = (HWND)0;
    HDC		hdc;
    HDC		printer_dc = (HDC)pprinter_dc;

    ಯದಿ (printer_dc == ರಿಕ್ತ)
    {
	hwnd = GetDesktopWindow();
	hdc = GetWindowDC(hwnd);
    }
    ಅನ್ಯಥಾ
	hdc = printer_dc;

    points = pixels * 72.0 / GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX);
    ಯದಿ (printer_dc == ರಿಕ್ತ)
	ReleaseDC(hwnd, hdc);

    ಪ್ರತ್ಯಾಗಮ points;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ CALLBACK
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
    ಯದಿ ((ntm->tmPitchAndFamily & 1) != 0)
	ಪ್ರತ್ಯಾಗಮ 1;
# endif

    // Remember this LOGFONTW as a "possible"
    *lf = elf->elfLogFont;

    // Terminate the scan as soon as we find an ANSI font
    ಯದಿ (lf->lfCharSet == ANSI_CHARSET
	    || lf->lfCharSet == OEM_CHARSET
	    || lf->lfCharSet == DEFAULT_CHARSET)
	ಪ್ರತ್ಯಾಗಮ 0;

    // Continue the scan - we have a non-ANSI font
    ಪ್ರತ್ಯಾಗಮ 2;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
init_logfont(LOGFONTW *lf)
{
    ಪೂರ್ಣಾಙ್ಕ		n;
    HWND	hwnd = GetDesktopWindow();
    HDC		hdc = GetWindowDC(hwnd);

    n = EnumFontFamiliesW(hdc,
			 lf->lfFaceName,
			 (FONTENUMPROCW)font_enumproc,
			 (LPARAM)lf);

    ReleaseDC(hwnd, hdc);

    // If we couldn't find a usable font, return failure
    ಯದಿ (n == 1)
	ಪ್ರತ್ಯಾಗಮ FAIL;

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
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Call back for EnumFontFamiliesW in expand_font_enumproc.
 *
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ CALLBACK
expand_font_enumproc(
    ENUMLOGFONTW    *elf,
    NEWTEXTMETRICW  *ntm UNUSED,
    DWORD	    type UNUSED,
    LPARAM	    lparam)
{
    LOGFONTW *lf = (LOGFONTW*)elf;

# ifndef FEAT_PROPORTIONAL_FONTS
    // Ignore non-monospace fonts without further ado
    ಯದಿ ((ntm->tmPitchAndFamily & 1) != 0)
	ಪ್ರತ್ಯಾಗಮ 1;
# endif

    // Filter only on ANSI. Otherwise will see a lot of random fonts that we
    // usually don't want.
    ಯದಿ (lf->lfCharSet != ANSI_CHARSET)
	ಪ್ರತ್ಯಾಗಮ 1;

    ಪೂರ್ಣಾಙ್ಕ (*add_match)(char_u *) = (ಪೂರ್ಣಾಙ್ಕ (*)(char_u *))lparam;

    WCHAR *faceNameW = lf->lfFaceName;
    char_u *faceName = utf16_to_enc(faceNameW, ರಿಕ್ತ);
    ಯದಿ (!faceName)
	ಪ್ರತ್ಯಾಗಮ 0;

    add_match(faceName);
    vim_free(faceName);

    ಪ್ರತ್ಯಾಗಮ 1;
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
    ಶೂನ್ಯ
gui_mch_expand_font(
    optexpand_T	*args,
    ಶೂನ್ಯ	*param UNUSED,
    ಪೂರ್ಣಾಙ್ಕ		(*add_match)(char_u *val))
{
    expand_T	    *xp = args->oe_xp;
    ಯದಿ (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	ವರ್ಣ buf[30];
	ಪೂರ್ಣಾಙ್ಕ	i;

	// Always fill in with the current font size as first option for
	// convenience. We simply round to the closest integer for simplicity.
	ಪೂರ್ಣಾಙ್ಕ font_height = (ಪೂರ್ಣಾಙ್ಕ)round(
		pixels_to_points(-current_font_height, TRUE, (long_i)ರಿಕ್ತ));
	vim_snprintf(buf, ARRAY_LENGTH(buf), "h%d", font_height);
	add_match((char_u *)buf);

	// Note: Keep this in sync with get_logfont(). Don't include 'c' and
	// 'q' as we fill in all the values below.
	ಸ್ಥಿರ ವರ್ಣ *(p_gfn_win_opt_ಮೌಲ್ಯಗಳು[]) = {
	    "h" , "w" , "W" , "b" , "i" , "u" , "s"};
	ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(p_gfn_win_opt_ಮೌಲ್ಯಗಳು); i++)
	    add_match((char_u *)p_gfn_win_opt_ಮೌಲ್ಯಗಳು[i]);

	ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(charset_pairs); ++i)
	{
	    vim_snprintf(buf, ಪರಿಮಾಣ(buf), "c%s", charset_pairs[i].name.string);
	    add_match((char_u *)buf);
	}

	ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(quality_pairs); ++i)
	{
	    vim_snprintf(buf, ಪರಿಮಾಣ(buf), "q%s", quality_pairs[i].name.string);
	    add_match((char_u *)buf);
	}
	ಪ್ರತ್ಯಾಗಮ;
    }

    HWND	hwnd = GetDesktopWindow();
    HDC		hdc = GetWindowDC(hwnd);

    EnumFontFamiliesW(hdc,
	    ರಿಕ್ತ,
	    (FONTENUMPROCW)expand_font_enumproc,
	    (LPARAM)add_match);

    ReleaseDC(hwnd, hdc);
}

/*
 * Compare a UTF-16 string and an ASCII string literally.
 * Only works all the code points are inside ASCII range.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
utf16ascncmp(ಧ್ರುವ WCHAR *w, ಧ್ರುವ ವರ್ಣ *p, size_t n)
{
    size_t i;

    ಪ್ರತಿ (i = 0; i < n; i++)
    {
	ಯದಿ (w[i] == 0 || w[i] != p[i])
	    ಪ್ರತ್ಯಾಗಮ w[i] - p[i];
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Equivalent of GetDpiForSystem().
 */
    UINT WINAPI
vimGetDpiForSystem(ಶೂನ್ಯ)
{
    HWND hwnd = GetDesktopWindow();
    HDC hdc = GetWindowDC(hwnd);
    UINT dpi = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(hwnd, hdc);
    ಪ್ರತ್ಯಾಗಮ dpi;
}

/*
 * Set default logfont based on current language.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_default_logfont(LOGFONTW *lf)
{
    // Default font name for current language on MS-Windows.
    // If not translated, falls back to "Consolas".
    // This must be a fixed-pitch font.
    ಧ್ರುವ ವರ್ಣ *defaultfontname = N_("DefaultFontNameForWindows");
    ವರ್ಣ *fontname = _(defaultfontname);

    ಯದಿ (STRCMP(fontname, defaultfontname) == 0)
	fontname = "Consolas";

    *lf = s_lfDefault;
    lf->lfHeight = DEFAULT_FONT_HEIGHT * (ಪೂರ್ಣಾಙ್ಕ)vimGetDpiForSystem() / 96;
    ಯದಿ (current_font_height == 0)
	current_font_height = lf->lfHeight;

    WCHAR *wfontname = enc_to_utf16((char_u*)fontname, ರಿಕ್ತ);
    ಯದಿ (wfontname != ರಿಕ್ತ)
    {
	wcscpy_s(lf->lfFaceName, LF_FACESIZE, wfontname);
	vim_free(wfontname);
    }
}

/*
 * Get font info from "name" into logfont "lf".
 * Return OK for a valid name, FAIL otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_logfont(
    LOGFONTW	*lf,
    char_u	*name,
    HDC		printer_dc,
    ಪೂರ್ಣಾಙ್ಕ		verbose)
{
    WCHAR	*p;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		ret = FAIL;
    ಸ್ಥಿರ LOGFONTW *lastlf = ರಿಕ್ತ;
    WCHAR	*wname;

    set_default_logfont(lf);
    ಯದಿ (name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ OK;

    wname = enc_to_utf16(name, ರಿಕ್ತ);
    ಯದಿ (wname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (wcscmp(wname, L"*") == 0)
    {
# ಯದಿ defined(FEAT_GUI_MSWIN)
	CHOOSEFONTW	cf;
	// if name is "*", bring up std font dialog:
	CLEAR_FIELD(cf);
	cf.lStructSize = ಪರಿಮಾಣ(cf);
	cf.hwndOwner = s_hwnd;
	cf.Flags = CF_SCREENFONTS | CF_FIXEDPITCHONLY | CF_INITTOLOGFONTSTRUCT;
	ಯದಿ (lastlf != ರಿಕ್ತ)
	    *lf = *lastlf;
	cf.lpLogFont = lf;
	cf.nFontType = 0 ; //REGULAR_FONTTYPE;
	ಯದಿ (ChooseFontW(&cf))
	    ret = OK;
# endif
	ಗಚ್ಛ theend;
    }

    /*
     * Split name up, it could be <name>:h<height>:w<width> etc.
     */
    ಪ್ರತಿ (p = wname; *p && *p != L':'; p++)
    {
	ಯದಿ (p - wname + 1 >= LF_FACESIZE)
	    ಗಚ್ಛ theend;			// Name too long
	lf->lfFaceName[p - wname] = *p;
    }
    ಯದಿ (p != wname)
	lf->lfFaceName[p - wname] = NUL;

    // First set defaults
    lf->lfHeight = DEFAULT_FONT_HEIGHT * (ಪೂರ್ಣಾಙ್ಕ)vimGetDpiForSystem() / 96;
    lf->lfWidth = 0;
    lf->lfWeight = FW_NORMAL;
    lf->lfItalic = FALSE;
    lf->lfUnderline = FALSE;
    lf->lfStrikeOut = FALSE;

    /*
     * If the font can't be found, try replacing '_' by ' '.
     */
    ಯದಿ (init_logfont(lf) == FAIL)
    {
	ಪೂರ್ಣಾಙ್ಕ	did_replace = FALSE;

	ಪ್ರತಿ (i = 0; lf->lfFaceName[i]; ++i)
	    ಯದಿ (lf->lfFaceName[i] == L'_')
	    {
		lf->lfFaceName[i] = L' ';
		did_replace = TRUE;
	    }
	ಯದಿ (!did_replace || init_logfont(lf) == FAIL)
	    ಗಚ್ಛ theend;
    }

    ಯಾವತ್ (*p == L':')
	p++;

    // Set the values found after ':'
    ಯಾವತ್ (*p)
    {
	ವಿಕಲ್ಪ (*p++)
	{
	    // Note: Keep this in sync with gui_mch_expand_font().
	    ಪಕ್ಷ L'h':
		lf->lfHeight = - points_to_pixels(p, &p, TRUE, (long_i)printer_dc);
		ಭಙ್ಗ;
	    ಪಕ್ಷ L'w':
		lf->lfWidth = points_to_pixels(p, &p, FALSE, (long_i)printer_dc);
		ಭಙ್ಗ;
	    ಪಕ್ಷ L'W':
		lf->lfWeight = wcstol(p, &p, 10);
		ಭಙ್ಗ;
	    ಪಕ್ಷ L'b':
		lf->lfWeight = FW_BOLD;
		ಭಙ್ಗ;
	    ಪಕ್ಷ L'i':
		lf->lfItalic = TRUE;
		ಭಙ್ಗ;
	    ಪಕ್ಷ L'u':
		lf->lfUnderline = TRUE;
		ಭಙ್ಗ;
	    ಪಕ್ಷ L's':
		lf->lfStrikeOut = TRUE;
		ಭಙ್ಗ;
	    ಪಕ್ಷ L'c':
		ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(charset_pairs); ++i)
		{
		    ಯದಿ (utf16ascncmp(p, (ವರ್ಣ *)charset_pairs[i].name.string,
			    charset_pairs[i].name.length) == 0)
		    {
			lf->lfCharSet = charset_pairs[i].charset;
			p += charset_pairs[i].name.length;
			ಭಙ್ಗ;
		    }
		}

		ಯದಿ (i == (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(charset_pairs) && verbose)
		{
		    char_u *s = utf16_to_enc(p, ರಿಕ್ತ);
		    ಯದಿ (s != ರಿಕ್ತ)
		    {
			semsg(_(e_illegal_str_name_str_in_font_name_str),
							   "charset", s, name);
			vim_free(s);
		    }
		}
		ಭಙ್ಗ;
	    ಪಕ್ಷ L'f':
		// Font features (e.g., "fss19=1").
		// Parsed separately by gui_mch_init_font(); skip here.
		ಯಾವತ್ (*p && *p != L':')
		    p++;
		ಭಙ್ಗ;
	    ಪಕ್ಷ L'q':
		ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(quality_pairs); ++i)
		{
		    ಯದಿ (utf16ascncmp(p, (ವರ್ಣ *)quality_pairs[i].name.string,
			    quality_pairs[i].name.length) == 0)
		    {
			lf->lfQuality = quality_pairs[i].quality;
			p += quality_pairs[i].name.length;
			ಭಙ್ಗ;
		    }
		}

		ಯದಿ (i == (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(quality_pairs) && verbose)
		{
		    char_u *s = utf16_to_enc(p, ರಿಕ್ತ);
		    ಯದಿ (s != ರಿಕ್ತ)
		    {
			semsg(_(e_illegal_str_name_str_in_font_name_str),
							   "quality", s, name);
			vim_free(s);
		    }
		}
		ಭಙ್ಗ;
	    ಸ್ವಭಾವ:
		ಯದಿ (verbose)
		    semsg(_(e_illegal_char_nr_in_font_name_str), p[-1], name);
		ಗಚ್ಛ theend;
	}
	ಯಾವತ್ (*p == L':')
	    p++;
    }
    ret = OK;

theend:
    // ron: init lastlf
    ಯದಿ (ret == OK && printer_dc == ರಿಕ್ತ)
    {
	vim_free(lastlf);
	lastlf = ALLOC_ONE(LOGFONTW);
	ಯದಿ (lastlf != ರಿಕ್ತ)
	    mch_memmove(lastlf, lf, ಪರಿಮಾಣ(LOGFONTW));
    }
    vim_free(wname);

    ಪ್ರತ್ಯಾಗಮ ret;
}

#endif // defined(FEAT_GUI) || defined(FEAT_PRINTER)

#ಯದಿ defined(FEAT_JOB_CHANNEL)
/*
 * Initialize the Winsock dll.
 */
    ಶೂನ್ಯ
channel_init_winsock(ಶೂನ್ಯ)
{
    WSADATA wsaData;
    ಪೂರ್ಣಾಙ್ಕ wsaerr;

    ಯದಿ (WSInitialized)
	ಪ್ರತ್ಯಾಗಮ;

    wsaerr = WSAStartup(MAKEWORD(2, 2), &wsaData);
    ಯದಿ (wsaerr == 0)
	WSInitialized = TRUE;
}
#endif
