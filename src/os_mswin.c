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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଅନ୍ତର୍ଭାଵ <sys/types.h>
#ଅନ୍ତର୍ଭାଵ <limits.h>

#ଅନ୍ତର୍ଭାଵ <process.h>
#ଅନ୍ତର୍ଭାଵ <direct.h>

#ଯଦି !defined(FEAT_GUI_MSWIN)
# include <shellapi.h>
#endif

#ଯଦି defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT)
# include <dlgs.h>
# include <winspool.h>
# include <commdlg.h>
#endif

// Record all output and all keyboard & mouse input
// #define MCH_WRITE_DUMP

#ifdef MCH_WRITE_DUMP
FILE* fdDump = ଖାଲି;
#endif

#ଯଦି !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ବାହ୍ୟ ଅକ୍ଷର g_szOrigTitle[];
#endif

#ifdef FEAT_GUI
ବାହ୍ୟ HWND s_hwnd;
#ଅନ୍ୟଥା
ସ୍ଥିର HWND s_hwnd = 0;	    // console window handle, set by GetConsoleHwnd()
#endif

#ifdef FEAT_JOB_CHANNEL
ପୂର୍ଣ୍ଣସଂଖ୍ୟା WSInitialized = FALSE; // WinSock is initialized
#endif


/*
 * Save the instance handle of the exe/dll.
 */
    ଶୂନ୍ୟ
SaveInst(HINSTANCE hInst)
{
    g_hinst = hInst;
}

#ଯଦି defined(FEAT_GUI_MSWIN)
/*
 * GUI version of mch_exit().
 * Shut down and exit with status `r'
 * Careful: mch_exit() may be called before mch_init()!
 */
    ଶୂନ୍ୟ
mch_exit_g(ପୂର୍ଣ୍ଣସଂଖ୍ୟା r)
{
    exiting = TRUE;

    display_errors();

    ml_close_all(TRUE);		// remove all memfiles

# ifdef FEAT_OLE
    UninitOLE();
# endif
# ifdef FEAT_JOB_CHANNEL
    ଯଦି (WSInitialized)
    {
	WSInitialized = FALSE;
	WSACleanup();
    }
# endif
# ifdef DYNAMIC_GETTEXT
    dyn_libintl_end();
# endif

    ଯଦି (gui.in_use)
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
ପ୍ରକାରବ୍ୟାଖ୍ୟା BOOL (WINAPI *PfnRtlGetVersion)(LPOSVERSIONINFOW);
DWORD win_version;
    ସ୍ଥିର ଶୂନ୍ୟ
win_version_init(ଶୂନ୍ୟ)
{
    OSVERSIONINFOW	osver;
    HMODULE		hNtdll;
    PfnRtlGetVersion	pRtlGetVersion;

    hNtdll = GetModuleHandle("ntdll.dll");
    ଯଦି (hNtdll == ଖାଲି)
	ଫେରନ୍ତୁ;

    pRtlGetVersion =
	(PfnRtlGetVersion) GetProcAddress(hNtdll, "RtlGetVersion");
    ଯଦି (pRtlGetVersion == ଖାଲି)
	ଫେରନ୍ତୁ;

    osver.dwOSVersionInfoSize = ଆକାର(OSVERSIONINFOW);
    pRtlGetVersion(&osver);
    win_version = MAKE_VER(min(osver.dwMajorVersion, 0xFF),
	    min(osver.dwMinorVersion, 0xFF),
	    min(osver.dw_Build_ସଂଖ୍ୟା, 0xFFFF));
}

/*
 * Init the tables for toupper() and tolower().
 */
    ଶୂନ୍ୟ
mch_early_init(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    win_version_init();

    PlatformId();

    // Init the tables for toupper() and tolower()
    ପାଇଁ (i = 0; i < 256; ++i)
	toupper_tab[i] = tolower_tab[i] = i;
    CharUpperBuff((LPSTR)toupper_tab, 256);
    CharLowerBuff((LPSTR)tolower_tab, 256);
}


/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_ନିବେଶ_isatty(ଶୂନ୍ୟ)
{
#ifdef FEAT_GUI_MSWIN
# ifdef VIMDLL
    ଯଦି (gui.in_use)
# endif
	ଫେରନ୍ତୁ TRUE;	    // GUI always has a tty
#endif
#ଯଦି !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    ଯଦି (isatty(read_cmd_fd))
	ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
#endif
}

/*
 * mch_settitle(): set titlebar of our window
 */
    ଶୂନ୍ୟ
mch_settitle(
    char_u *title,
    char_u *icon UNUSED)
{
#ifdef FEAT_GUI_MSWIN
# ifdef VIMDLL
    ଯଦି (gui.in_use)
# endif
    {
	gui_mch_settitle(title, icon);
	ଫେରନ୍ତୁ;
    }
#endif
#ଯଦି !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    ଯଦି (title != ଖାଲି)
    {
	WCHAR	*wp = enc_to_utf16(title, ଖାଲି);

	ଯଦି (wp == ଖାଲି)
	    ଫେରନ୍ତୁ;

	SetConsoleTitleW(wp);
	vim_free(wp);
	ଫେରନ୍ତୁ;
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
    ଶୂନ୍ୟ
mch_restore_title(ପୂର୍ଣ୍ଣସଂଖ୍ୟା which UNUSED)
{
#ଯଦି !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
    ଯଦି (!gui.in_use)
# endif
	SetConsoleTitle(g_szOrigTitle);
#endif
}


/*
 * Return TRUE if we can restore the title (we can)
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_can_restore_title(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ TRUE;
}


/*
 * Return TRUE if we can restore the icon title (we can't)
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_can_restore_icon(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ FALSE;
}


/*
 * Get absolute file name into buffer "buf" of length "len" bytes,
 * turning all '/'s into '\\'s and getting the correct case of each component
 * of the file name.  Append a (back)slash to a directory name.
 * When 'shellslash' set do it the other way around.
 * Return OK or FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		force UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nResult = FAIL;
    WCHAR	*wname;
    WCHAR	wbuf[MAX_PATH];
    char_u	*cname = ଖାଲି;

    wname = enc_to_utf16(fname, ଖାଲି);
    ଯଦି (wname != ଖାଲି && _wfullpath(wbuf, wname, MAX_PATH) != ଖାଲି)
    {
	cname = utf16_to_enc((short_u *)wbuf, ଖାଲି);
	ଯଦି (cname != ଖାଲି)
	{
	    vim_strncpy(buf, cname, len - 1);
	    nResult = OK;
	}
    }
    vim_free(wname);
    vim_free(cname);

#ifdef USE_FNAME_CASE
    fname_case(buf, len);
#ଅନ୍ୟଥା
    slash_adjust(buf);
#endif

    ଫେରନ୍ତୁ nResult;
}


/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_isFullName(char_u *fname)
{
    // A name like "d:/foo" and "//server/share" is absolute.  "d:foo" is not.
    // /foo and \foo are absolute too because Windows keeps a current drive.
    // Another way to check is to use mch_FullName() and see if the result is
    // the same as the name or mch_FullName() fails.  However, this has quite a
    // bit of overhead, so let's not do that.
    ଯଦି (*fname == NUL)
	ଫେରନ୍ତୁ FALSE;
    ଫେରନ୍ତୁ ((ASCII_ISALPHA(fname[0]) && fname[1] == ':'
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
    ଶୂନ୍ୟ
slash_adjust(char_u *p)
{
    ଯଦି (path_with_url(p))
	ଫେରନ୍ତୁ;

    ଯଦି (*p == '`')
    {
	size_t len = STRLEN(p);

	// don't replace backslash in backtick quoted strings
	ଯଦି (len > 2 && *(p + len - 1) == '`')
	    ଫେରନ୍ତୁ;
    }

    ଯେତେବେଳେ (*p)
    {
	ଯଦି (*p == psepcN)
	    *p = psepc;
	MB_PTR_ADV(p);
    }
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
read_reparse_point(ଅଚଳ WCHAR *name, char_u *buf, DWORD *buf_len)
{
    HANDLE h;
    BOOL ok;

    h = CreateFileW(name, FILE_READ_ATTRIBUTES,
	    FILE_SHARE_READ | FILE_SHARE_WRITE, ଖାଲି,
	    OPEN_EXISTING,
	    FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,
	    ଖାଲି);
    ଯଦି (h == INVALID_HANDLE_ମୂଲ୍ୟ)
	ଫେରନ୍ତୁ FAIL;

    ok = DeviceIoControl(h, FSCTL_GET_REPARSE_POINT, ଖାଲି, 0, buf, *buf_len,
	    buf_len, ଖାଲି);
    CloseHandle(h);

    ଫେରନ୍ତୁ ok ? OK : FAIL;
}

    char_u *
resolve_appexeclink(char_u *fname)
{
    DWORD		attr = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			idx;
    WCHAR		*p, *end, *wname;
    // The buffer size is arbitrarily chosen to be "big enough" (TM), the
    // ceiling should be around 16k.
    char_u		buf[4096];
    DWORD		buf_len = ଆକାର(buf);
    REPARSE_DATA_BUFFER *rb = (REPARSE_DATA_BUFFER *)buf;

    wname = enc_to_utf16(fname, ଖାଲି);
    ଯଦି (wname == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    attr = GetFileAttributesW(wname);
    ଯଦି (attr == INVALID_FILE_ATTRIBUTES ||
	    (attr & FILE_ATTRIBUTE_REPARSE_POINT) == 0)
    {
	vim_free(wname);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    // The applinks are similar to symlinks but with a huge difference: they can
    // only be executed, any other I/O operation on them is bound to fail with
    // ERROR_FILE_NOT_FOUND even though the file exists.
    ଯଦି (read_reparse_point(wname, buf, &buf_len) == FAIL)
    {
	vim_free(wname);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    vim_free(wname);

    ଯଦି (rb->ReparseTag != IO_REPARSE_TAG_APPEXECLINK)
	ଫେରନ୍ତୁ ଖାଲି;

    // The (undocumented) reparse buffer contains a set of N null-terminated
    // Unicode strings, the application path is stored in the third one.
    ଯଦି (rb->AppExecLinkReparseBuffer.String_ଗଣନା < 3)
	ଫେରନ୍ତୁ ଖାଲି;

    p = rb->AppExecLinkReparseBuffer.StringList;
    end = p + rb->ReparseDataLength / ଆକାର(WCHAR);
    ପାଇଁ (idx = 0; p < end
	    && idx < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)rb->AppExecLinkReparseBuffer.String_ଗଣନା
	    && idx != 2; )
    {
	ଯଦି (*p++ == L'\0')
	    ++idx;
    }

    ଫେରନ୍ତୁ utf16_to_enc(p, ଖାଲି);
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mswin_stat_impl(ଅଚଳ WCHAR *name, stat_T *stp, ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା resolve)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			n;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			fd;
    BOOL		is_symlink = FALSE;
    HANDLE		hFind, h;
    DWORD		attr = 0;
    DWORD		flag = 0;
    WIN32_FIND_DATAW    findDataW;

#ଯଦି 0 && defined(_UCRT)
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
    ଯଦି (resolve)
	// Universal CRT can handle symlinks properly.
	ଫେରନ୍ତୁ _wstat(name, stp);
#endif

    // Only reparse points need a directory query to identify symbolic links.
    // In particular, avoid enumerating the directory for each missing file
    // while searching 'runtimepath'.
    attr = GetFileAttributesW(name);
    ଯଦି (attr == INVALID_FILE_ATTRIBUTES)
    {
	DWORD error = GetLastError();

	// A missing file or path cannot provide stat information.  Other
	// errors and UNC paths still need the CRT fallback.
	ଯଦି ((error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND)
		&& !((name[0] == L'\\' || name[0] == L'/')
		    && (name[1] == L'\\' || name[1] == L'/')))
	{
	    errno = ENOENT;
	    ଫେରନ୍ତୁ -1;
	}
	ଫେରନ୍ତୁ _wstat(name, stp);
    }
    ଯଦି (!(attr & FILE_ATTRIBUTE_REPARSE_POINT))
	ଫେରନ୍ତୁ _wstat(name, stp);

    hFind = FindFirstFileW(name, &findDataW);
    ଯଦି (hFind != INVALID_HANDLE_ମୂଲ୍ୟ)
    {
	attr = findDataW.dwFileAttributes;
	ଯଦି ((attr & FILE_ATTRIBUTE_REPARSE_POINT)
		&& (findDataW.dwReserved0 == IO_REPARSE_TAG_SYMLINK))
	    is_symlink = TRUE;
	FindClose(hFind);
    }

    // Use the plain old stat() whenever it's possible.
    ଯଦି (!is_symlink)
	ଫେରନ୍ତୁ _wstat(name, stp);

    ଯଦି (!resolve && is_symlink)
	flag = FILE_FLAG_OPEN_REPARSE_POINT;
    ଯଦି (attr & FILE_ATTRIBUTE_DIRECTORY)
	flag |= FILE_FLAG_BACKUP_SEMANTICS;

    h = CreateFileW(name, FILE_READ_ATTRIBUTES,
	    FILE_SHARE_READ | FILE_SHARE_WRITE, ଖାଲି, OPEN_EXISTING, flag,
	    ଖାଲି);
    ଯଦି (h == INVALID_HANDLE_ମୂଲ୍ୟ)
	ଫେରନ୍ତୁ -1;

    fd = _open_osfhandle((intptr_t)h, _O_RDONLY);
    n = _fstat(fd, (ସଂରଚନା _stat *)stp);
    ଯଦି ((n == 0) && (attr & FILE_ATTRIBUTE_DIRECTORY))
	stp->st_mode = (stp->st_mode & ~S_IFMT) | S_IFDIR;
    _close(fd);

    ଫେରନ୍ତୁ n;
}

/*
 * stat() can't handle a trailing '/' or '\', remove it first.
 * When 'resolve' is true behave as lstat() wrt symlinks.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
stat_impl(ଅଚଳ ଅକ୍ଷର *name, stat_T *stp, ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା resolve)
{
    // WinNT and later can use _MAX_PATH wide characters for a pathname, which
    // means that the maximum pathname is _MAX_PATH * 3 bytes when 'enc' is
    // UTF-8.
    char_u	buf[_MAX_PATH * 3 + 1];
    size_t	buflen;
    char_u	*p;
    WCHAR	*wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;

    vim_strncpy((char_u *)buf, (char_u *)name, ଆକାର(buf) - 1);
    buflen = STRLEN(buf);
    p = buf + buflen;
    ଯଦି (p > buf)
	MB_PTR_BACK(buf, p);

    // Remove trailing '\\' except root path.
    ଯଦି (p > buf && (*p == '\\' || *p == '/') && p[-1] != ':')
    {
	*p = NUL;
	buflen = (size_t)(p - buf);
    }

    ଯଦି ((buf[0] == '\\' && buf[1] == '\\') || (buf[0] == '/' && buf[1] == '/'))
    {
	// UNC root path must be followed by '\\'.
	p = vim_strpbrk(buf + 2, (char_u *)"\\/");
	ଯଦି (p != ଖାଲି)
	{
	    p = vim_strpbrk(p + 1, (char_u *)"\\/");
	    ଯଦି (p == ଖାଲି)
		STRCPY(buf + buflen, "\\");
	}
    }

    wp = enc_to_utf16(buf, ଖାଲି);
    ଯଦି (wp == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    n = mswin_stat_impl(wp, stp, resolve);
    vim_free(wp);
    ଫେରନ୍ତୁ n;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_lstat(ଅଚଳ ଅକ୍ଷର *name, stat_T *stp)
{
    ଫେରନ୍ତୁ stat_impl(name, stp, FALSE);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_stat(ଅଚଳ ଅକ୍ଷର *name, stat_T *stp)
{
    ଫେରନ୍ତୁ stat_impl(name, stp, TRUE);
}

#ଯଦି defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    ଶୂନ୍ୟ
mch_settmode(tmode_T tmode UNUSED)
{
    // nothing to do
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_get_shellsize(ଶୂନ୍ୟ)
{
    // never used
    ଫେରନ୍ତୁ OK;
}

    ଶୂନ୍ୟ
mch_set_shellsize(ଶୂନ୍ୟ)
{
    // never used
}

/*
 * Rows and/or Columns has changed.
 */
    ଶୂନ୍ୟ
mch_new_shellsize(ଶୂନ୍ୟ)
{
    // never used
}

    ଶୂନ୍ୟ
mch_calc_cell_size(ସଂରଚନା cellsize *cs_out UNUSED)
{
    // never used
}

#endif

/*
 * We have no job control, so fake it by starting a new shell.
 */
    ଶୂନ୍ୟ
mch_suspend(ଶୂନ୍ୟ)
{
    suspend_shell();
}

#ଯଦି defined(USE_MCH_ERRMSG)

# ifdef display_errors
#  undef display_errors
# endif

/*
 * Display the saved error message(s).
 */
    ଶୂନ୍ୟ
display_errors(ଶୂନ୍ୟ)
{
# ifdef FEAT_GUI
    char_u *p;

#  ifdef VIMDLL
    ଯଦି (gui.in_use || gui.starting)
#  endif
    {
	ଯଦି (error_ga.ga_data != ଖାଲି)
	{
	    // avoid putting up a message box with blanks only
	    ପାଇଁ (p = (char_u *)error_ga.ga_data; *p; ++p)
		ଯଦି (!SAFE_isspace(*p))
		{
		    // Only use a dialog when not using --gui-dialog-file:
		    // write text to a file.
		    ଯଦି (!gui_dialog_log((char_u *)"Errors", p))
			(ଶୂନ୍ୟ)gui_mch_dialog(
				     gui.starting ? VIM_INFO :
					     VIM_ERROR,
				     gui.starting ? (char_u *)_("Message") :
					     (char_u *)_("Error"),
					     p, (char_u *)_("&Ok"),
					1, ଖାଲି, FALSE);
		    ଭାଙ୍ଗ;
		}
	    ga_clear(&error_ga);
	}
	ଫେରନ୍ତୁ;
    }
# endif
# ଯଦି !defined(FEAT_GUI) || defined(VIMDLL)
    FlushFileBuffers(GetStdHandle(STD_ERROR_HANDLE));
# endif
}
#endif


/*
 * Return TRUE if "p" contain a wildcard that can be expanded by
 * dos_expandpath().
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_has_exp_wildcard(char_u *p)
{
    ପାଇଁ ( ; *p; MB_PTR_ADV(p))
    {
	ଯଦି (vim_strchr((char_u *)"?*[", *p) != ଖାଲି
		|| (*p == '~' && p[1] != NUL))
	    ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Return TRUE if "p" contain a wildcard or a "~1" kind of thing (could be a
 * shortened file name).
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_has_wildcard(char_u *p)
{
    ପାଇଁ ( ; *p; MB_PTR_ADV(p))
    {
	ଯଦି (vim_strchr((char_u *)"?*$[`", *p) != ଖାଲି
		|| (*p == '~' && p[1] != NUL))
	    ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}


/*
 * The normal _chdir() does not change the default drive.  This one does.
 * Returning 0 implies success; -1 implies failure.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_chdir(ଅକ୍ଷର *path)
{
    WCHAR   *p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    n;

    ଯଦି (path[0] == NUL)		// just checking...
	ଫେରନ୍ତୁ -1;

    ଯଦି (p_verbose >= 5)
    {
	verbose_enter();
	smsg("chdir(%s)", path);
	verbose_leave();
    }
    ଯଦି (SAFE_isalpha(path[0]) && path[1] == ':')	// has a drive name
    {
	// If we can change to the drive, skip that part of the path.  If we
	// can't then the current directory may be invalid, try using chdir()
	// with the whole path.
	ଯଦି (_chdrive(TOLOWER_ASC(path[0]) - 'a' + 1) == 0)
	    path += 2;
    }

    ଯଦି (*path == NUL)		// drive name only
	ଫେରନ୍ତୁ 0;

    p = enc_to_utf16((char_u *)path, ଖାଲି);
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    n = _wchdir(p);
    vim_free(p);
    ଫେରନ୍ତୁ n;
}


#ଯଦି defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
/*
 * return non-zero if a character is available
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_char_avail(ଶୂନ୍ୟ)
{
    // never used
    ଫେରନ୍ତୁ TRUE;
}

# ଯଦି defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_check_messages(ଶୂନ୍ୟ)
{
    // TODO: check for messages
    ଫେରନ୍ତୁ TRUE;
}
# endif
#endif


#ଯଦି defined(FEAT_LIBCALL)
/*
 * Call a DLL routine which takes either a string or int param
 * and returns an allocated string.
 * Return OK if it worked, FAIL if not.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା char_u *(*MYSTRPROCSTR)(char_u *);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*MYSTRPROCINT)(char_u *);
ପ୍ରକାରବ୍ୟାଖ୍ୟା char_u *(*MYINTPROCSTR)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*MYINTPROCINT)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

/*
 * Check if a pointer points to a valid NUL terminated string.
 * Return the length of the string, including terminating NUL.
 * Returns 0 for an invalid pointer, 1 for an empty string.
 */
    ସ୍ଥିର size_t
check_str_len(char_u *str)
{
    SYSTEM_INFO			si;
    MEMORY_BASIC_INFORMATION	mbi;
    size_t			length = 0;
    size_t			i;
    ଅଚଳ char_u		*p;

    // get page size
    GetSystemInfo(&si);

    // get memory information
    ଯଦି (!VirtualQuery(str, &mbi, ଆକାର(mbi)))
	ଫେରନ୍ତୁ 0;

    // pre cast these (typing savers)
    long_u dwStr = (long_u)str;
    long_u dwBaseAddress = (long_u)mbi.BaseAddress;

    // get start address of page that str is on
    long_u strPage = dwStr - (dwStr - dwBaseAddress) % si.dwPageSize;

    // get length from str to end of page
    long_u pageLength = si.dwPageSize - (dwStr - strPage);

    ପାଇଁ (p = str; !IsBadReadPtr(p, (UINT)pageLength);
	    p += pageLength, pageLength = si.dwPageSize)
	ପାଇଁ (i = 0; i < pageLength; ++i, ++length)
	    ଯଦି (p[i] == NUL)
		ଫେରନ୍ତୁ length + 1;

    ଫେରନ୍ତୁ 0;
}

/*
 * Passed to do_in_runtimepath() to load a vim.ico file.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
mch_icon_load_cb(char_u *fname, ଶୂନ୍ୟ *cookie)
{
    HANDLE *h = (HANDLE *)cookie;

    *h = LoadImage(ଖାଲି,
		   (LPSTR)fname,
		   IMAGE_ICON,
		   64,
		   64,
		   LR_LOADFROMFILE | LR_LOADMAP3DCOLORS);
}

/*
 * Try loading an icon file from 'runtimepath'.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_icon_load(HANDLE *iconp)
{
    ଫେରନ୍ତୁ do_in_runtimepath((char_u *)"bitmaps/vim.ico",
						  0, mch_icon_load_cb, iconp);
}

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if the OS PRNG is not available or something went wrong.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_get_random(char_u *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		initialized = NOTDONE;
    ସ୍ଥିର HINSTANCE	hInstLib;
    ସ୍ଥିର BOOL (WINAPI *pProcessPrng)(PUCHAR, ULONG);

    ଯଦି (initialized == NOTDONE)
    {
	hInstLib = vimLoadLib("bcryptprimitives.dll");
	ଯଦି (hInstLib != ଖାଲି)
	    pProcessPrng = (ଶୂନ୍ୟ *)GetProcAddress(hInstLib, "ProcessPrng");
	ଯଦି (hInstLib == ଖାଲି || pProcessPrng == ଖାଲି)
	{
	    FreeLibrary(hInstLib);
	    initialized = FAIL;
	}
	ଅନ୍ୟଥା
	    initialized = OK;
    }

    ଯଦି (initialized == FAIL)
	ଫେରନ୍ତୁ FAIL;

    // According to the documentation this call cannot fail.
    pProcessPrng(buf, len);

    ଫେରନ୍ତୁ OK;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_libcall(
    char_u	*libname,
    char_u	*funcname,
    char_u	*argstring,	// NULL when using a argint
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argint,
    char_u	**string_result,// NULL when using number_result
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ସଂଖ୍ୟା_result)
{
    HINSTANCE		hinstLib;

    BOOL fRunTimeLinkSuccess = FALSE;

    // Get a handle to the DLL module.
    hinstLib = vimLoadLib((ଅକ୍ଷର *)libname);

    // If the handle is valid, try to get the function address.
    ଯଦି (hinstLib != ଖାଲି)
    {
	char_u	*retval_str = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	retval_int = 0;
	size_t	len;

# ifdef HAVE_TRY_EXCEPT
	__try
	{
# endif
	ଯଦି (argstring != ଖାଲି)
	{
	    // Call with string argument
	    ଯଦି (string_result != ଖାଲି)
	    {
		MYSTRPROCSTR proc;

		proc = (MYSTRPROCSTR)GetProcAddress(hinstLib, (LPCSTR)funcname);
		ଯଦି (proc != ଖାଲି)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_str = proc(argstring);
		}
	    }
	    ଅନ୍ୟଥା
	    {
		MYSTRPROCINT proc;

		proc = (MYSTRPROCINT)GetProcAddress(hinstLib, (LPCSTR)funcname);
		ଯଦି (proc != ଖାଲି)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_int = proc(argstring);
		}
	    }
	}
	ଅନ୍ୟଥା
	{
	    // Call with number argument
	    ଯଦି (string_result != ଖାଲି)
	    {
		MYINTPROCSTR proc;

		proc = (MYINTPROCSTR)GetProcAddress(hinstLib, (LPCSTR)funcname);
		ଯଦି (proc != ଖାଲି)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_str = proc(argint);
		}
	    }
	    ଅନ୍ୟଥା
	    {
		MYINTPROCINT proc;

		proc = (MYINTPROCINT)GetProcAddress(hinstLib, (LPCSTR)funcname);
		ଯଦି (proc != ଖାଲି)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_int = proc(argint);
		}
	    }
	}

	// Save the string before we free the library.
	// Assume that a "1" result is an illegal pointer.
	ଯଦି (string_result == ଖାଲି)
	    *ସଂଖ୍ୟା_result = retval_int;
	ଅନ୍ୟଥା ଯଦି (retval_str != ଖାଲି
		&& (len = check_str_len(retval_str)) > 0)
	{
	    *string_result = alloc(len);
	    ଯଦି (*string_result != ଖାଲି)
		mch_memmove(*string_result, retval_str, len);
	}

# ifdef HAVE_TRY_EXCEPT
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	    ଯଦି (GetExceptionCode() == EXCEPTION_ଗଦା_OVERFLOW)
		_resetstkoflw();
	    fRunTimeLinkSuccess = FALSE;
	}
# endif

	// Free the DLL module.
	(ଶୂନ୍ୟ)FreeLibrary(hinstLib);
    }

    ଯଦି (!fRunTimeLinkSuccess)
    {
	semsg(_(e_library_call_failed_for_str), funcname);
	ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ OK;
}
#endif

/*
 * Debugging helper: expose the MCH_WRITE_DUMP stuff to other modules
 */
    ଶୂନ୍ୟ
DumpPutS(ଅଚଳ ଅକ୍ଷର *psz UNUSED)
{
#ifdef MCH_WRITE_DUMP
    ଯଦି (fdDump)
    {
	fputs(psz, fdDump);
	ଯଦି (psz[strlen(psz) - 1] != '\n')
	    fputc('\n', fdDump);
	fflush(fdDump);
    }
#endif
}

#ifdef _DEBUG

ଶୂନ୍ୟ __cdecl
Trace(
    ଅକ୍ଷର *pszFormat,
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

#ଯଦି !defined(FEAT_GUI) || defined(VIMDLL)
ବାହ୍ୟ HWND g_hWnd;	// This is in os_win32.c.

/*
 * Showing the printer dialog is tricky since we have no GUI
 * window to parent it. The following routines are needed to
 * get the window parenting and Z-order to work properly.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
GetConsoleHwnd(ଶୂନ୍ୟ)
{
    // Skip if it's already set.
    ଯଦି (s_hwnd != 0)
	ଫେରନ୍ତୁ;

    // Window handle may have been found by init code (Windows NT only)
    ଯଦି (g_hWnd != 0)
    {
	s_hwnd = g_hWnd;
	ଫେରନ୍ତୁ;
    }

    s_hwnd = GetConsoleWindow();
}

/*
 * Console implementation of ":winpos".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_get_winpos(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *y)
{
    RECT  rect;

    GetConsoleHwnd();
    GetWindowRect(s_hwnd, &rect);
    *x = rect.ବାମ;
    *y = rect.ଶୀର୍ଷ;
    ଫେରନ୍ତୁ OK;
}

/*
 * Console implementation of ":winpos x y".
 */
    ଶୂନ୍ୟ
mch_set_winpos(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y)
{
    GetConsoleHwnd();
    SetWindowPos(s_hwnd, ଖାଲି, x, y, 0, 0,
		 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}
#endif

#ଯଦି defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT)

//=================================================================
// Win32 printer stuff

ସ୍ଥିର HFONT		prt_font_handles[2][2][2];
ସ୍ଥିର PRINTDLGW	prt_dlg;
ସ୍ଥିର ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା	boldface[2] = {FW_REGULAR, FW_BOLD};
ସ୍ଥିର TEXTMETRIC	prt_tm;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prt_ଧାଡ଼ି_height;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prt_ସଂଖ୍ୟା_width;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prt_ବାମ_margin;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prt_ଡାହାଣ_margin;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prt_ଶୀର୍ଷ_margin;
ସ୍ଥିର char_u		szAppName[] = TEXT("VIM");
ସ୍ଥିର HWND		hDlgPrint;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*bUserAbort = ଖାଲି;
ସ୍ଥିର char_u		*prt_name = ଖାଲି;

// Defines which are also in vim.rc.
# define IDC_BOX1		400
# define IDC_PRINTTEXT1		401
# define IDC_PRINTTEXT2		402
# define IDC_PROGRESS		403

    ସ୍ଥିର BOOL
vimSetDlgItemText(HWND hDlg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nIDDlgItem, char_u *s)
{
    WCHAR   *wp;
    BOOL    ret;

    wp = enc_to_utf16(s, ଖାଲି);
    ଯଦି (wp == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    ret = SetDlgItemTextW(hDlg, nIDDlgItem, wp);
    vim_free(wp);
    ଫେରନ୍ତୁ ret;
}

/*
 * Convert BGR to RGB for Windows GDI calls
 */
    ସ୍ଥିର COLORREF
swap_me(COLORREF colorref)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା  temp;
    ଅକ୍ଷର *ptr = (ଅକ୍ଷର *)&colorref;

    temp = *(ptr);
    *(ptr ) = *(ptr + 2);
    *(ptr + 2) = temp;
    ଫେରନ୍ତୁ colorref;
}

    ସ୍ଥିର INT_PTR CALLBACK
PrintDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam UNUSED,
	LPARAM lParam UNUSED)
{
# ifdef FEAT_GETTEXT
    NONCLIENTMETRICS nm;
    ସ୍ଥିର HFONT hfont;
# endif

    ବାଛ (message)
    {
	ସ୍ଥିତି WM_INITDIALOG:
# ifdef FEAT_GETTEXT
	    nm.cbSize = ଆକାର(NONCLIENTMETRICS);
	    ଯଦି (SystemParametersInfo(
			SPI_GETNONCLIENTMETRICS,
			ଆକାର(NONCLIENTMETRICS),
			&nm,
			0))
	    {
		ଅକ୍ଷର buff[MAX_PATH];
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

		// Translate the dialog texts
		hfont = CreateFontIndirect(&nm.lfMessageFont);
		ପାଇଁ (i = IDC_PRINTTEXT1; i <= IDC_PROGRESS; i++)
		{
		    SendDlgItemMessage(hDlg, i, WM_SETFONT, (WPARAM)hfont, 1);
		    ଯଦି (GetDlgItemText(hDlg,i, buff, ଆକାର(buff)))
			vimSetDlgItemText(hDlg,i, (char_u *)_(buff));
		}
		SendDlgItemMessage(hDlg, IDCANCEL,
						WM_SETFONT, (WPARAM)hfont, 1);
		ଯଦି (GetDlgItemText(hDlg,IDCANCEL, buff, ଆକାର(buff)))
		    vimSetDlgItemText(hDlg,IDCANCEL, (char_u *)_(buff));
	    }
# endif
	    SetWindowText(hDlg, (LPCSTR)szAppName);
	    ଯଦି (prt_name != ଖାଲି)
	    {
		vimSetDlgItemText(hDlg, IDC_PRINTTEXT2, (char_u *)prt_name);
		VIM_CLEAR(prt_name);
	    }
	    EnableMenuItem(GetSystemMenu(hDlg, FALSE), SC_CLOSE, MF_GRAYED);
# ଯଦି !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	    ଯଦି (!gui.in_use)
#  endif
		Bring_Window_To_ଶୀର୍ଷ(s_hwnd);
# endif
	    ଫେରନ୍ତୁ TRUE;

	ସ୍ଥିତି WM_COMMAND:
	    *bUserAbort = TRUE;
	    EnableWindow(GetParent(hDlg), TRUE);
	    DestroyWindow(hDlg);
	    hDlgPrint = ଖାଲି;
# ifdef FEAT_GETTEXT
	    DeleteObject(hfont);
# endif
	    ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

    ସ୍ଥିର BOOL CALLBACK
AbortProc(HDC hdcPrn UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା iCode UNUSED)
{
    ସନ୍ଦେଶ_2 ସନ୍ଦେଶ;

    ଯେତେବେଳେ (!*bUserAbort && PeekMessageW(&ସନ୍ଦେଶ, ଖାଲି, 0, 0, PM_REMOVE))
    {
	ଯଦି (!hDlgPrint || !IsDialogMessageW(hDlgPrint, &ସନ୍ଦେଶ))
	{
	    TranslateMessage(&ସନ୍ଦେଶ);
	    DispatchMessageW(&ସନ୍ଦେଶ);
	}
    }
    ଫେରନ୍ତୁ !*bUserAbort;
}

    ସ୍ଥିର UINT_PTR CALLBACK
PrintHookProc(
	HWND hDlg,	// handle to dialog box
	UINT ui_ସନ୍ଦେଶ,	// message identifier
	WPARAM wParam UNUSED,	// message parameter
	LPARAM lParam	// message parameter
	)
{
    HWND	hwndOwner;
    RECT	rc, rcDlg, rcOwner;
    PRINTDLGW	*pPD;

    ଯଦି (ui_ସନ୍ଦେଶ != WM_INITDIALOG)
	ଫେରନ୍ତୁ FALSE;

    // Get the owner window and dialog box rectangles.
    ଯଦି ((hwndOwner = GetParent(hDlg)) == ଖାଲି)
	hwndOwner = GetDesktopWindow();

    GetWindowRect(hwndOwner, &rcOwner);
    GetWindowRect(hDlg, &rcDlg);
    CopyRect(&rc, &rcOwner);

    // Offset the owner and dialog box rectangles so that
    // right and bottom values represent the width and
    // height, and then offset the owner again to discard
    // space taken up by the dialog box.

    OffsetRect(&rcDlg, -rcDlg.ବାମ, -rcDlg.ଶୀର୍ଷ);
    OffsetRect(&rc, -rc.ବାମ, -rc.ଶୀର୍ଷ);
    OffsetRect(&rc, -rcDlg.ଡାହାଣ, -rcDlg.bottom);

    // The new position is the sum of half the remaining
    // space and the owner's original position.

    SetWindowPos(hDlg,
	    HWND_ଶୀର୍ଷ,
	    rcOwner.ବାମ + (rc.ଡାହାଣ / 2),
	    rcOwner.ଶୀର୍ଷ + (rc.bottom / 2),
	    0, 0,		// ignores size arguments
	    SWP_NOSIZE);

    //  tackle the printdlg copiesctrl problem
    pPD = (PRINTDLGW *)lParam;
    pPD->nCopies = (WORD)pPD->lCustData;
    SetDlgItemInt( hDlg, edt3, pPD->nCopies, FALSE );
    //  Bring the window to top
    Bring_Window_To_ଶୀର୍ଷ(GetParent(hDlg));
    SetForegroundWindow(hDlg);

    ଫେରନ୍ତୁ FALSE;
}

    ଶୂନ୍ୟ
mch_print_cleanup(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା pifItalic;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା pifBold;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା pifUnderline;

    ପାଇଁ (pifBold = 0; pifBold <= 1; pifBold++)
	ପାଇଁ (pifItalic = 0; pifItalic <= 1; pifItalic++)
	    ପାଇଁ (pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
		DeleteObject(prt_font_handles[pifBold][pifItalic][pifUnderline]);

    ଯଦି (prt_dlg.hDC != ଖାଲି)
	DeleteDC(prt_dlg.hDC);
    ଯଦି (!*bUserAbort && hDlgPrint != ଖାଲି)
	SendMessage(hDlgPrint, WM_COMMAND, 0, 0);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
to_device_units(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dpi, ପୂର୍ଣ୍ଣସଂଖ୍ୟା physsize, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା def_ସଂଖ୍ୟା)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		u;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nr;

    u = prt_get_unit(idx);
    ଯଦି (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_ସଂଖ୍ୟା;
    }
    ଅନ୍ୟଥା
	nr = printer_opts[idx].ସଂଖ୍ୟା;

    ବାଛ (u)
    {
	ସ୍ଥିତି PRT_UNIT_PERC:
	    ret = (physsize * nr) / 100;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି PRT_UNIT_INCH:
	    ret = (nr * dpi);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି PRT_UNIT_MM:
	    ret = (nr * 10 * dpi) / 254;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି PRT_UNIT_POINT:
	    ret = (nr * 10 * dpi) / 720;
	    ଭାଙ୍ଗ;
    }

    ଯଦି (ret < offset)
	ଫେରନ୍ତୁ 0;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ ret - offset;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
prt_get_cpl(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		hr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		phyw;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dvoff;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		rev_offset;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dpi;

    GetTextMetrics(prt_dlg.hDC, &prt_tm);
    prt_ଧାଡ଼ି_height = prt_tm.tmHeight + prt_tm.tmExternalLeading;

    hr	    = GetDeviceCaps(prt_dlg.hDC, HORZRES);
    phyw    = GetDeviceCaps(prt_dlg.hDC, PHYSICALWIDTH);
    dvoff   = GetDeviceCaps(prt_dlg.hDC, PHYSICALOFFSETX);
    dpi	    = GetDeviceCaps(prt_dlg.hDC, LOGPIXELSX);

    rev_offset = phyw - (dvoff + hr);

    prt_ବାମ_margin = to_device_units(OPT_PRINT_ବାମ, dpi, phyw, dvoff, 10);
    ଯଦି (prt_use_ସଂଖ୍ୟା())
    {
	prt_ସଂଖ୍ୟା_width = PRINT_ସଂଖ୍ୟା_WIDTH * prt_tm.tmAveCharWidth;
	prt_ବାମ_margin += prt_ସଂଖ୍ୟା_width;
    }
    ଅନ୍ୟଥା
	prt_ସଂଖ୍ୟା_width = 0;

    prt_ଡାହାଣ_margin = hr - to_device_units(OPT_PRINT_ଡାହାଣ, dpi, phyw,
							       rev_offset, 5);

    ଫେରନ୍ତୁ (prt_ଡାହାଣ_margin - prt_ବାମ_margin) / prt_tm.tmAveCharWidth;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
prt_get_lpp(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା vr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା phyw;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା dvoff;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rev_offset;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	bottom_margin;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	dpi;

    vr	    = GetDeviceCaps(prt_dlg.hDC, VERTRES);
    phyw    = GetDeviceCaps(prt_dlg.hDC, PHYSICALHEIGHT);
    dvoff   = GetDeviceCaps(prt_dlg.hDC, PHYSICALOFFSETY);
    dpi	    = GetDeviceCaps(prt_dlg.hDC, LOGPIXELSY);

    rev_offset = phyw - (dvoff + vr);

    prt_ଶୀର୍ଷ_margin = to_device_units(OPT_PRINT_ଶୀର୍ଷ, dpi, phyw, dvoff, 5);

    // adjust top margin if there is a header
    prt_ଶୀର୍ଷ_margin += prt_ଧାଡ଼ି_height * prt_header_height();

    bottom_margin = vr - to_device_units(OPT_PRINT_BOT, dpi, phyw,
							       rev_offset, 5);

    ଫେରନ୍ତୁ (bottom_margin - prt_ଶୀର୍ଷ_margin) / prt_ଧାଡ଼ି_height;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_print_init(prt_settings_T *psettings, char_u *jobname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା forceit)
{
    ସ୍ଥିର HGLOBAL	stored_dm = ଖାଲି;
    ସ୍ଥିର HGLOBAL	stored_devn = ଖାଲି;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		stored_nCopies = 1;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		stored_nFlags = 0;

    LOGFONTW		fLogFont;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			pifItalic;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			pifBold;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			pifUnderline;

    DEVMODEW		*mem;
    DEVNAMES		*devname;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			i;
    DWORD		err;

    bUserAbort = &(psettings->user_abort);
    CLEAR_FIELD(prt_dlg);
    prt_dlg.lStructSize = ଆକାର(PRINTDLGW);
# ଯଦି !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
    ଯଦି (!gui.in_use)
#  endif
	GetConsoleHwnd();	    // get value of s_hwnd
# endif
    prt_dlg.hwndOwner = s_hwnd;
    prt_dlg.Flags = PD_NOPAGENUMS | PD_NOSELECTION | PD_RETURNDC;
    ଯଦି (!forceit)
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
    ଯଦି (forceit
# ଯଦି !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	    || (!gui.in_use && !term_console)
#  ଅନ୍ୟଥା
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
	ଯଦି (*p_pdev != NUL)
	    prt_dlg.hDC = CreateDC(ଖାଲି, (LPCSTR)p_pdev, ଖାଲି, ଖାଲି);
	ଅନ୍ୟଥା
	{
	    prt_dlg.Flags |= PD_RETURNDEFAULT;
	    ଯଦି (PrintDlgW(&prt_dlg) == 0)
		ଯାଅ init_fail_dlg;
	}
    }
    ଅନ୍ୟଥା ଯଦି (PrintDlgW(&prt_dlg) == 0)
	ଯାଅ init_fail_dlg;
    ଅନ୍ୟଥା
    {
	/*
	 * keep the previous driver context
	 */
	stored_dm = prt_dlg.hDevMode;
	stored_devn = prt_dlg.hDevNames;
	stored_nFlags = prt_dlg.Flags;
	stored_nCopies = prt_dlg.nCopies;
    }

    ଯଦି (prt_dlg.hDC == ଖାଲି)
    {
	emsg(_(e_printer_selection_failed));
	mch_print_cleanup();
	ଫେରନ୍ତୁ FALSE;
    }

    // Not all printer drivers report the support of color (or grey) in the
    // same way.  Let's set has_color if there appears to be some way to print
    // more than B&W.
    i = GetDeviceCaps(prt_dlg.hDC, NUMCOLORS);
    psettings->has_color = (GetDeviceCaps(prt_dlg.hDC, BITSPIXEL) > 1
				   || GetDeviceCaps(prt_dlg.hDC, PLANES) > 1
				   || i > 2 || i == -1);

    // Ensure all font styles are baseline aligned
    SetTextAlign(prt_dlg.hDC, TA_BASELINE|TA_ବାମ);

    /*
     * On some windows systems the nCopies parameter is not
     * passed back correctly. It must be retrieved from the
     * hDevMode struct.
     */
    mem = (DEVMODEW *)GlobalLock(prt_dlg.hDevMode);
    ଯଦି (mem != ଖାଲି)
    {
	ଯଦି (mem->dmCopies != 1)
	    stored_nCopies = mem->dmCopies;
	ଯଦି ((mem->dmFields & DM_DUPLEX) && (mem->dmDuplex & ~DMDUP_SIMPLEX))
	    psettings->duplex = TRUE;
	ଯଦି ((mem->dmFields & DM_COLOR) && (mem->dmColor & DMCOLOR_COLOR))
	    psettings->has_color = TRUE;
    }
    GlobalUnlock(prt_dlg.hDevMode);

    devname = (DEVNAMES *)GlobalLock(prt_dlg.hDevNames);
    ଯଦି (devname != 0)
    {
	WCHAR	*wprinter_name = (WCHAR *)devname + devname->wDeviceOffset;
	WCHAR	*wport_name = (WCHAR *)devname + devname->wOutputOffset;
	char_u	*text = (char_u *)_("to %s on %s");
	char_u  *printer_name = utf16_to_enc(wprinter_name, ଖାଲି);
	char_u	*port_name = utf16_to_enc(wport_name, ଖାଲି);

	ଯଦି (printer_name != ଖାଲି && port_name != ଖାଲି)
	    prt_name = alloc(STRLEN(printer_name)
					   + STRLEN(port_name) + STRLEN(text));
	ଯଦି (prt_name != ଖାଲି)
	    wsprintf((ଅକ୍ଷର *)prt_name, (ଅଚଳ ଅକ୍ଷର *)text,
		    printer_name, port_name);
	vim_free(printer_name);
	vim_free(port_name);
    }
    GlobalUnlock(prt_dlg.hDevNames);

    /*
     * Initialise the font according to 'printfont'
     */
    CLEAR_FIELD(fLogFont);
    ଯଦି (get_logfont(&fLogFont, p_pfn, prt_dlg.hDC, TRUE) == FAIL)
    {
	semsg(_(e_unknown_printer_font_str), p_pfn);
	mch_print_cleanup();
	ଫେରନ୍ତୁ FALSE;
    }

    ପାଇଁ (pifBold = 0; pifBold <= 1; pifBold++)
	ପାଇଁ (pifItalic = 0; pifItalic <= 1; pifItalic++)
	    ପାଇଁ (pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
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
    psettings->chars_per_ଧାଡ଼ି = prt_get_cpl();
    psettings->ଧାଡ଼ିଗୁଡ଼ିକ_per_page = prt_get_lpp();
    ଯଦି (prt_dlg.Flags & PD_USEDEVMODECOPIESANDCOLLATE)
    {
	psettings->n_collated_copies = (prt_dlg.Flags & PD_COLLATE)
						    ? prt_dlg.nCopies : 1;
	psettings->n_uncollated_copies = (prt_dlg.Flags & PD_COLLATE)
						    ? 1 : prt_dlg.nCopies;

	ଯଦି (psettings->n_collated_copies == 0)
	    psettings->n_collated_copies = 1;

	ଯଦି (psettings->n_uncollated_copies == 0)
	    psettings->n_uncollated_copies = 1;
    }
    ଅନ୍ୟଥା
    {
	psettings->n_collated_copies = 1;
	psettings->n_uncollated_copies = 1;
    }

    psettings->jobname = jobname;

    ଫେରନ୍ତୁ TRUE;

init_fail_dlg:
    err = CommDlgExtendedError();
    ଯଦି (err)
    {
	char_u *buf;

	// I suspect FormatMessage() doesn't work for values returned by
	// CommDlgExtendedError().  What does?
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		ଖାଲି, err, 0, (LPTSTR)(&buf), 0, ଖାଲି);
	semsg(_(e_print_error_str),
		buf == ଖାଲି ? (char_u *)_("Unknown") : buf);
	LocalFree((LPVOID)(buf));
    }
    ଅନ୍ୟଥା
	ସନ୍ଦେଶ_clr_eos(); // Maybe canceled

    mch_print_cleanup();
    ଫେରନ୍ତୁ FALSE;
}


    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_print_begin(prt_settings_T *psettings)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			ret = 0;
    ଅକ୍ଷର		szBuffer[300];
    WCHAR		*wp;

    hDlgPrint = CreateDialog(g_hinst, TEXT("PrintDlgBox"),
					     prt_dlg.hwndOwner, PrintDlgProc);
    SetAbortProc(prt_dlg.hDC, AbortProc);
    wsprintf(szBuffer, _("Printing '%s'"), gettail(psettings->jobname));
    vimSetDlgItemText(hDlgPrint, IDC_PRINTTEXT1, (char_u *)szBuffer);

    wp = enc_to_utf16(psettings->jobname, ଖାଲି);
    ଯଦି (wp != ଖାଲି)
    {
	DOCINFOW	di;

	CLEAR_FIELD(di);
	di.cbSize = ଆକାର(di);
	di.lpszDocName = wp;
	ret = StartDocW(prt_dlg.hDC, &di);
	vim_free(wp);
    }

# ifdef FEAT_GUI
    // Give focus back to main window (when using MDI).
#  ifdef VIMDLL
    ଯଦି (gui.in_use)
#  endif
	SetFocus(s_hwnd);
# endif

    ଫେରନ୍ତୁ (ret > 0);
}

    ଶୂନ୍ୟ
mch_print_end(prt_settings_T *psettings UNUSED)
{
    EndDoc(prt_dlg.hDC);
    ଯଦି (!*bUserAbort && hDlgPrint != ଖାଲି)
	SendMessage(hDlgPrint, WM_COMMAND, 0, 0);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_print_end_page(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (EndPage(prt_dlg.hDC) > 0);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_print_begin_page(char_u *ସନ୍ଦେଶ)
{
    ଯଦି (ସନ୍ଦେଶ != ଖାଲି)
	vimSetDlgItemText(hDlgPrint, IDC_PROGRESS, ସନ୍ଦେଶ);
    ଫେରନ୍ତୁ (StartPage(prt_dlg.hDC) > 0);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_print_blank_page(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (mch_print_begin_page(ଖାଲି) ? (mch_print_end_page()) : FALSE);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prt_pos_x = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା prt_pos_y = 0;

    ଶୂନ୍ୟ
mch_print_start_ଧାଡ଼ି(ପୂର୍ଣ୍ଣସଂଖ୍ୟା margin, ପୂର୍ଣ୍ଣସଂଖ୍ୟା page_ଧାଡ଼ି)
{
    ଯଦି (margin)
	prt_pos_x = -prt_ସଂଖ୍ୟା_width;
    ଅନ୍ୟଥା
	prt_pos_x = 0;
    prt_pos_y = page_ଧାଡ଼ି * prt_ଧାଡ଼ି_height
				 + prt_tm.tmAscent + prt_tm.tmExternalLeading;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_print_text_out(char_u *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    SIZE	sz;
    WCHAR	*wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wlen = len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = FALSE;

    wp = enc_to_utf16(p, &wlen);
    ଯଦି (wp == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    TextOutW(prt_dlg.hDC, prt_pos_x + prt_ବାମ_margin,
	    prt_pos_y + prt_ଶୀର୍ଷ_margin, wp, wlen);
    GetTextExtentPoint32W(prt_dlg.hDC, wp, wlen, &sz);
    vim_free(wp);
    prt_pos_x += (sz.cx - prt_tm.tmOverhang);
    // This is wrong when printing spaces for a TAB.
    ଯଦି (p[len] != NUL)
    {
	wlen = mb_ptr2len(p + len);
	wp = enc_to_utf16(p + len, &wlen);
	ଯଦି (wp != ଖାଲି)
	{
	    GetTextExtentPoint32W(prt_dlg.hDC, wp, 1, &sz);
	    ret = (prt_pos_x + prt_ବାମ_margin + sz.cx > prt_ଡାହାଣ_margin);
	    vim_free(wp);
	}
    }
    ଫେରନ୍ତୁ ret;
}

    ଶୂନ୍ୟ
mch_print_set_font(ପୂର୍ଣ୍ଣସଂଖ୍ୟା iBold, ପୂର୍ଣ୍ଣସଂଖ୍ୟା iItalic, ପୂର୍ଣ୍ଣସଂଖ୍ୟା iUnderline)
{
    SelectObject(prt_dlg.hDC, prt_font_handles[iBold][iItalic][iUnderline]);
}

    ଶୂନ୍ୟ
mch_print_set_bg(long_u bgcol)
{
    SetBkColor(prt_dlg.hDC, GetNearestColor(prt_dlg.hDC,
						   swap_me((COLORREF)bgcol)));
    /*
     * With a white background we can draw characters transparent, which is
     * good for italic characters that overlap to the next char cell.
     */
    ଯଦି (bgcol == 0xffffffUL)
	SetBkMode(prt_dlg.hDC, TRANSPARENT);
    ଅନ୍ୟଥା
	SetBkMode(prt_dlg.hDC, OPAQUE);
}

    ଶୂନ୍ୟ
mch_print_set_fg(long_u fgcol)
{
    SetTextColor(prt_dlg.hDC, GetNearestColor(prt_dlg.hDC,
						   swap_me((COLORREF)fgcol)));
}

#endif // FEAT_PRINTER && !FEAT_POSTSCRIPT



#ଯଦି defined(FEAT_SHORTCUT)
# include <shlobj.h>

# define is_path_sep(c)	    ((c) == L'\\' || (c) == L'/')

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_reparse_point_included(LPCWSTR fname)
{
    LPCWSTR	p = fname, q;
    WCHAR	buf[MAX_PATH];
    DWORD	attr;

    ଯଦି (SAFE_isalpha(p[0]) && p[1] == L':' && is_path_sep(p[2]))
	p += 3;
    ଅନ୍ୟଥା ଯଦି (is_path_sep(p[0]) && is_path_sep(p[1]))
	p += 2;

    ଯେତେବେଳେ (*p != L'\0')
    {
	q = wcspbrk(p, L"\\/");
	ଯଦି (q == ଖାଲି)
	    p = q = fname + wcslen(fname);
	ଅନ୍ୟଥା
	    p = q + 1;
	ଯଦି (q - fname >= MAX_PATH)
	    ଫେରନ୍ତୁ FALSE;
	wcsncpy(buf, fname, q - fname);
	buf[q - fname] = L'\0';
	attr = GetFileAttributesW(buf);
	ଯଦି (attr != INVALID_FILE_ATTRIBUTES
		&& (attr & FILE_ATTRIBUTE_REPARSE_POINT) != 0)
	    ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Return the resolved file path, NULL if "fname" is an AppExecLink reparse
 * point, already fully resolved, or it doesn't exists.
 */
    char_u *
resolve_reparse_point(char_u *fname)
{
    HANDLE	    h = INVALID_HANDLE_ମୂଲ୍ୟ;
    DWORD	    size;
    WCHAR	    *p, *wp;
    char_u	    *rfname = ଖାଲି;
    WCHAR	    *buff = ଖାଲି;

    p = enc_to_utf16(fname, ଖାଲି);
    ଯଦି (p == ଖାଲି)
	ଯାଅ fail;

    ଯଦି (!is_reparse_point_included(p))
    {
	vim_free(p);
	ଯାଅ fail;
    }

    h = CreateFileW(p, 0, 0, ଖାଲି, OPEN_EXISTING,
	    FILE_FLAG_BACKUP_SEMANTICS, ଖାଲି);
    vim_free(p);

    ଯଦି (h == INVALID_HANDLE_ମୂଲ୍ୟ)
	ଯାଅ fail;

    size = GetFinalPathNameByHandleW(h, ଖାଲି, 0, 0);
    ଯଦି (size == 0)
	ଯାଅ fail;
    buff = ALLOC_MULT(WCHAR, size);
    ଯଦି (buff == ଖାଲି)
	ଯାଅ fail;
    ଯଦି (GetFinalPathNameByHandleW(h, buff, size, 0) == 0)
	ଯାଅ fail;

    ଯଦି (wcsncmp(buff, L"\\\\?\\UNC\\", 8) == 0)
    {
	buff[6] = L'\\';
	wp = buff + 6;
    }
    ଅନ୍ୟଥା ଯଦି (wcsncmp(buff, L"\\\\?\\", 4) == 0)
	wp = buff + 4;
    ଅନ୍ୟଥା
	wp = buff;

    rfname = utf16_to_enc(wp, ଖାଲି);

fail:
    ଯଦି (h != INVALID_HANDLE_ମୂଲ୍ୟ)
	CloseHandle(h);
    ଯଦି (buff != ଖାଲି)
	vim_free(buff);

    ଫେରନ୍ତୁ rfname;
}

/*
 * When "fname" is the name of a shortcut (*.lnk) resolve the file it points
 * to and return that name in allocated memory.
 * Otherwise NULL is returned.
 */
    ସ୍ଥିର char_u *
resolve_shortcut(char_u *fname)
{
    HRESULT		hr;
    IShellLink		*psl = ଖାଲି;
    IPersistFile	*ppf = ଖାଲି;
    OLECHAR		wsz[MAX_PATH];
    char_u		*rfname = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			len;
    IShellLinkW		*pslw = ଖାଲି;
    WIN32_FIND_DATAW	ffdw; // we get those free of charge

    // Check if the file name ends in ".lnk". Avoid calling
    // CoCreateInstance(), it's quite slow.
    ଯଦି (fname == ଖାଲି)
	ଫେରନ୍ତୁ rfname;
    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(fname);
    ଯଦି (len <= 4 || STRNICMP(fname + len - 4, ".lnk", 4) != 0)
	ଫେରନ୍ତୁ rfname;

    CoInitialize(ଖାଲି);

    // create a link manager object and request its interface
    hr = CoCreateInstance(
	    &CLSID_ShellLink, ଖାଲି, CLSCTX_INPROC_SERVER,
	    &IID_IShellLinkW, (ଶୂନ୍ୟ**)&pslw);
    ଯଦି (hr == S_OK)
    {
	WCHAR	*p = enc_to_utf16(fname, ଖାଲି);

	ଯଦି (p != ଖାଲି)
	{
	    // Get a pointer to the IPersistFile interface.
	    hr = pslw->lpVtbl->QueryInterface(
		    pslw, &IID_IPersistFile, (ଶୂନ୍ୟ**)&ppf);
	    ଯଦି (hr != S_OK)
		ଯାଅ shortcut_errorw;

	    // "load" the name and resolve the link
	    hr = ppf->lpVtbl->Load(ppf, p, STGM_READ);
	    ଯଦି (hr != S_OK)
		ଯାଅ shortcut_errorw;
# ଯଦି 0  // This makes Vim wait a long time if the target does not exist.
	    hr = pslw->lpVtbl->Resolve(pslw, ଖାଲି, SLR_NO_UI);
	    ଯଦି (hr != S_OK)
		ଯାଅ shortcut_errorw;
# endif

	    // Get the path to the link target.
	    ZeroMemory(wsz, MAX_PATH * ଆକାର(WCHAR));
	    hr = pslw->lpVtbl->GetPath(pslw, wsz, MAX_PATH, &ffdw, 0);
	    ଯଦି (hr == S_OK && wsz[0] != NUL)
		rfname = utf16_to_enc(wsz, ଖାଲି);

shortcut_errorw:
	    vim_free(p);
	}
    }

    // Release all interface pointers (both belong to the same object)
    ଯଦି (ppf != ଖାଲି)
	ppf->lpVtbl->Release(ppf);
    ଯଦି (psl != ଖାଲି)
	psl->lpVtbl->Release(psl);
    ଯଦି (pslw != ଖାଲି)
	pslw->lpVtbl->Release(pslw);

    CoUninitialize();
    ଫେରନ୍ତୁ rfname;
}

    char_u *
mch_resolve_path(char_u *fname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା reparse_point)
{
    char_u  *path = resolve_shortcut(fname);

    ଯଦି (path == ଖାଲି && reparse_point)
	path = resolve_reparse_point(fname);
    ଫେରନ୍ତୁ path;
}
#endif

#ଯଦି defined(FEAT_EVAL) && (!defined(FEAT_GUI) || defined(VIMDLL))
/*
 * Bring ourselves to the foreground.  Does work if the OS doesn't allow it.
 */
    ଶୂନ୍ୟ
win32_set_foreground(ଶୂନ୍ୟ)
{
    GetConsoleHwnd();	    // get value of s_hwnd
    ଯଦି (s_hwnd != 0)
	SetForegroundWindow(s_hwnd);
}
#endif

#ଯଦି defined(FEAT_CLIENTSERVER)
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
# define VIM_CLASSNAME_LEN  (ଆକାର(VIM_CLASSNAME) - 1)

// Timeout for sending a message to another Vim instance.  Normally this works
// instantly, but it may hang when the other Vim instance is halted.
# define SENDMESSAGE_TIMEOUT	(5 * 1000)

// Communication is via WM_COPYDATA messages. The message type is sent in
// the dwData parameter. Types are defined here.
# define COPYDATA_ଚାବିଗୁଡ଼ିକ		0
# define COPYDATA_REPLY		1
# define COPYDATA_ପଦସମଷ୍ଟି		10
# define COPYDATA_RESULT	11
# define COPYDATA_ERROR_RESULT	12
# define COPYDATA_ENCODING	20

// This is a structure containing a server HWND and its name.
ସଂରଚନା server_id
{
    HWND hwnd;
    char_u *name;
};

// Last received 'encoding' that the client uses.
ସ୍ଥିର char_u	*client_enc = ଖାଲି;

/*
 * Tell the other side what encoding we are using.
 * Return -1 if timeout happens.  Other errors are ignored.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
serverSendEnc(HWND target)
{
    COPYDATASTRUCT data;

    data.dwData = COPYDATA_ENCODING;
    data.cbData = (DWORD)STRLEN(p_enc) + 1;
    data.lpData = p_enc;
    ଯଦି (SendMessageTimeout(target, WM_COPYDATA,
	    (WPARAM)message_window, (LPARAM)&data,
	    SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, ଖାଲି) == 0)
	ଫେରନ୍ତୁ -1;
    ଫେରନ୍ତୁ 0;
}

/*
 * Clean up on exit. This destroys the hidden message window.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
CleanUpMessaging(ଶୂନ୍ୟ)
{
    ଯଦି (message_window == 0)
	ଫେରନ୍ତୁ;

    DestroyWindow(message_window);
    message_window = 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_reply(HWND server, char_u *reply, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ପଦସମଷ୍ଟି);

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
    ସ୍ଥିର LRESULT CALLBACK
Messaging_WndProc(HWND hwnd, UINT ସନ୍ଦେଶ, WPARAM wParam, LPARAM lParam)
{
    ଯଦି (ସନ୍ଦେଶ == WM_COPYDATA)
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
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval;
	DWORD_PTR	dwret = 0;
	char_u		*str;
	char_u		*tofree;

	ବାଛ (data->dwData)
	{
	ସ୍ଥିତି COPYDATA_ENCODING:
	    // Remember the encoding that the client uses.
	    vim_free(client_enc);
	    client_enc = enc_canonize((char_u *)data->lpData);
	    ଫେରନ୍ତୁ 1;

	ସ୍ଥିତି COPYDATA_ଚାବିଗୁଡ଼ିକ:
	    // Remember who sent this, for <client>
	    clientWindow = sender;

	    // Add the received keys to the input buffer.
	    str = serverConvert(client_enc, (char_u *)data->lpData, &tofree);
# ifdef FEAT_GUI
	    ଯଦି (gui.in_use)
		// The GUI may get here re-entrantly, so insert the keys later.
		server_add_ନିବେଶ(str);
	    ଅନ୍ୟଥା
# endif
		server_to_ନିବେଶ_buf(str);
	    vim_free(tofree);

# ifdef FEAT_GUI
	    // Wake up the main GUI loop.
#  ifdef VIMDLL
	    ଯଦି (gui.in_use)
#  endif
		ଯଦି (s_hwnd != 0)
		    PostMessage(s_hwnd, WM_NULL, 0, 0);
# endif
	    ଫେରନ୍ତୁ 1;

	ସ୍ଥିତି COPYDATA_ପଦସମଷ୍ଟି:
	    // Remember who sent this, for <client>
	    clientWindow = sender;

	    str = serverConvert(client_enc, (char_u *)data->lpData, &tofree);
	    res = eval_client_ପଦସମଷ୍ଟି_to_string(str);

	    ଯଦି (res == ଖାଲି)
	    {
		ଅକ୍ଷର	*err = _(e_invalid_expression_received);
		size_t	len = STRLEN(str) + STRLEN(err) + 5;

		res = alloc(len);
		ଯଦି (res != ଖାଲି)
		    reply.cbData = (DWORD)vim_snprintf_safelen(
			(ଅକ୍ଷର *)res, len, "%s: \"%s\"", err, str);
		ଅନ୍ୟଥା
		    reply.cbData = 0;
		reply.dwData = COPYDATA_ERROR_RESULT;
	    }
	    ଅନ୍ୟଥା
	    {
		reply.cbData = (DWORD)STRLEN(res) + 1;
		reply.dwData = COPYDATA_RESULT;
	    }
	    reply.lpData = res;

	    ଯଦି (serverSendEnc(sender) < 0)
		retval = -1;
	    ଅନ୍ୟଥା
	    {
		ଯଦି (SendMessageTimeout(sender, WM_COPYDATA,
			(WPARAM)message_window, (LPARAM)&reply,
			SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
		    retval = -1;
		ଅନ୍ୟଥା
		    retval = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dwret;
	    }
	    vim_free(tofree);
	    vim_free(res);
	    ଫେରନ୍ତୁ retval;

	ସ୍ଥିତି COPYDATA_REPLY:
	ସ୍ଥିତି COPYDATA_RESULT:
	ସ୍ଥିତି COPYDATA_ERROR_RESULT:
	    ଯଦି (data->lpData != ଖାଲି)
	    {
		str = serverConvert(client_enc, (char_u *)data->lpData,
								     &tofree);
		ଯଦି (tofree == ଖାଲି)
		    str = vim_strsave(str);
		ଯଦି (save_reply(sender, str,
			   (data->dwData == COPYDATA_REPLY ?  0 :
			   (data->dwData == COPYDATA_RESULT ? 1 :
							      2))) == FAIL)
		    vim_free(str);
		ଅନ୍ୟଥା ଯଦି (data->dwData == COPYDATA_REPLY)
		{
		    char_u	winstr[30];

		    sprintf((ଅକ୍ଷର *)winstr, PRINTF_HEX_LONG_U, (long_u)sender);
		    apply_autocmds(EVENT_REMOTEREPLY, winstr, str,
								TRUE, curbuf);
		}
	    }
	    ଫେରନ୍ତୁ 1;
	}

	ଫେରନ୍ତୁ 0;
    }

    ଅନ୍ୟଥା ଯଦି (ସନ୍ଦେଶ == WM_ACTIVATE && wParam == WA_ACTIVE)
    {
	// When the message window is activated (brought to the foreground),
	// this actually applies to the text window.
# ଯଦି !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	ଯଦି (!gui.in_use)
#  endif
	    GetConsoleHwnd();	    // get value of s_hwnd
# endif
	ଯଦି (s_hwnd != 0)
	{
	    SetForegroundWindow(s_hwnd);
	    ଫେରନ୍ତୁ 0;
	}
    }

    ଫେରନ୍ତୁ DefWindowProc(hwnd, ସନ୍ଦେଶ, wParam, lParam);
}

/*
 * Initialise the message handling process.  This involves creating a window
 * to handle messages - the window will not be visible.
 */
    ଶୂନ୍ୟ
serverInitMessaging(ଶୂନ୍ୟ)
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
    wndclass.hIcon = ଖାଲି;
    wndclass.hCursor = ଖାଲି;
    wndclass.hbrBackground = ଖାଲି;
    wndclass.lpszMenuName = ଖାଲି;
    wndclass.lpszClassName = VIM_CLASSNAME;
    RegisterClass(&wndclass);

    // Create the message window. It will be hidden, so the details don't
    // matter.  Don't use WS_OVERLAPPEDWINDOW, it will make a shortcut remove
    // focus from gvim.
    message_window = CreateWindow(VIM_CLASSNAME, "",
			 WS_POPUPWINDOW | WS_CAPTION,
			 CW_USEDEFAULT, CW_USEDEFAULT,
			 100, 100, ଖାଲି, ଖାଲି,
			 g_hinst, ଖାଲି);
}

// Used by serverSendToVim() to find an alternate server name.
ସ୍ଥିର char_u *altname_buf_ptr = ଖାଲି;

/*
 * Get the title of the window "hwnd", which is the Vim server name, in
 * "name[namelen]" and return the length.
 * Returns zero if window "hwnd" is not a Vim server.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
getVimServerName(HWND hwnd, ଅକ୍ଷର *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା namelen)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ଅକ୍ଷର	buffer[VIM_CLASSNAME_LEN + 1];

    // Ignore windows which aren't Vim message windows
    len = GetClassName(hwnd, buffer, ଆକାର(buffer));
    ଯଦି (len != VIM_CLASSNAME_LEN || STRCMP(buffer, VIM_CLASSNAME) != 0)
	ଫେରନ୍ତୁ 0;

    // Get the title of the window
    ଫେରନ୍ତୁ GetWindowText(hwnd, name, namelen);
}

    ସ୍ଥିର BOOL CALLBACK
enumWindowsGetServer(HWND hwnd, LPARAM lparam)
{
    ସଂରଚନା	server_id *id = (ସଂରଚନା server_id *)lparam;
    ଅକ୍ଷର	server[MAX_PATH];

    // Get the title of the window
    ଯଦି (getVimServerName(hwnd, server, ଆକାର(server)) == 0)
	ଫେରନ୍ତୁ TRUE;

    // If this is the server we're looking for, return its HWND
    ଯଦି (STRICMP(server, id->name) == 0)
    {
	id->hwnd = hwnd;
	ଫେରନ୍ତୁ FALSE;
    }

    // If we are looking for an alternate server, remember this name.
    ଯଦି (altname_buf_ptr != ଖାଲି)
    {
	size_t	namelen = STRLEN(id->name);

	ଯଦି (STRNICMP(server, id->name, namelen) == 0
	    && vim_isdigit(server[namelen]))
	{
	    STRCPY(altname_buf_ptr, server);
	    altname_buf_ptr = ଖାଲି;	    // don't use another name
	}
    }

    // Otherwise, keep looking
    ଫେରନ୍ତୁ TRUE;
}

    ସ୍ଥିର BOOL CALLBACK
enumWindowsGetNames(HWND hwnd, LPARAM lparam)
{
    list_T	*list = (list_T *)lparam;
    ଅକ୍ଷର	server[MAX_PATH];

    // Get the title of the window
    ଯଦି (getVimServerName(hwnd, server, ଆକାର(server)) == 0)
	ଫେରନ୍ତୁ TRUE;

    list_append_string(list, (char_u *)server, -1);
    ଫେରନ୍ତୁ TRUE;
}

ସଂରଚନା enum_windows_s
{
    WNDENUMPROC lpEnumFunc;
    LPARAM      lParam;
};

    ସ୍ଥିର BOOL CALLBACK
enum_windows_child(HWND hwnd, LPARAM lParam)
{
    ସଂରଚନା enum_windows_s *ew = (ସଂରଚନା enum_windows_s *)lParam;

    ଫେରନ୍ତୁ (ew->lpEnumFunc)(hwnd, ew->lParam);
}

    ସ୍ଥିର BOOL CALLBACK
enum_windows_toplevel(HWND hwnd, LPARAM lParam)
{
    ସଂରଚନା enum_windows_s *ew = (ସଂରଚନା enum_windows_s *)lParam;

    ଯଦି ((ew->lpEnumFunc)(hwnd, ew->lParam))
	ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ EnumChildWindows(hwnd, enum_windows_child, lParam);
}

/*
 * Enumerate all windows including children.
 */
    ସ୍ଥିର BOOL
enum_windows(WNDENUMPROC lpEnumFunc, LPARAM lParam)
{
    ସଂରଚନା enum_windows_s ew;

    ew.lpEnumFunc = lpEnumFunc;
    ew.lParam = lParam;
    ଫେରନ୍ତୁ EnumWindows(enum_windows_toplevel, (LPARAM)&ew);
}

    ସ୍ଥିର HWND
findServer(char_u *name)
{
    ସଂରଚନା server_id id;

    id.name = name;
    id.hwnd = 0;

    enum_windows(enumWindowsGetServer, (LPARAM)(&id));

    ଫେରନ୍ତୁ id.hwnd;
}

    ଶୂନ୍ୟ
serverSetName(char_u *name)
{
    size_t	namelen;
    char_u	*ok_name;
    HWND	hwnd = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i = 0;
    char_u	*p;

    // Leave enough space for a 9-digit suffix to ensure uniqueness!
    namelen = STRLEN(name);
    ok_name = alloc(namelen + 10);
    ଯଦି (ok_name == ଖାଲି)
	ଫେରନ୍ତୁ;

    STRCPY(ok_name, name);
    p = ok_name + namelen;

    ପାଇଁ (;;)
    {
	// This is inefficient - we're doing an EnumWindows loop for each
	// possible name. It would be better to grab all names in one go,
	// and scan the list each time...
	hwnd = findServer(ok_name);
	ଯଦି (hwnd == 0)
	    ଭାଙ୍ଗ;

	++i;
	ଯଦି (i >= 1000)
	    ଭାଙ୍ଗ;

	sprintf((ଅକ୍ଷର *)p, "%d", i);
    }

    ଯଦି (hwnd != 0)
	vim_free(ok_name);
    ଅନ୍ୟଥା
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
serverGetVimNames(ଶୂନ୍ୟ)
{
    list_T *list = list_alloc();

    ଯଦି (list == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    enum_windows(enumWindowsGetNames, (LPARAM)list);

    ଫେରନ୍ତୁ list;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
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
    sscanf((ଅକ୍ଷର *)name, SCANF_HEX_LONG_U, &n);
    ଯଦି (n == 0)
	ଫେରନ୍ତୁ -1;

    target = (HWND)n;
    ଯଦି (!IsWindow(target))
	ଫେରନ୍ତୁ -1;

    data.dwData = COPYDATA_REPLY;
    data.cbData = (DWORD)STRLEN(reply) + 1;
    data.lpData = reply;

    ଯଦି (serverSendEnc(target) < 0)
	ଫେରନ୍ତୁ -1;
    ଯଦି (SendMessageTimeout(target, WM_COPYDATA,
		(WPARAM)message_window, (LPARAM)&data,
		SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
	ଫେରନ୍ତୁ -1;
    ଫେରନ୍ତୁ dwret ? 0 : -1;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
serverSendToVim(
    char_u	 *name,			// Where to send.
    char_u	 *cmd,			// What to send.
    char_u	 **result,		// Result of eval'ed expression
    ଶୂନ୍ୟ	 *ptarget,		// HWND of server
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		 as_ପଦସମଷ୍ଟି,		// Expression or keys?
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		 timeout,		// timeout in seconds or zero
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		 silent)		// don't complain about no server
{
    size_t	namelen;
    HWND	target;
    COPYDATASTRUCT data;
    char_u	*retval = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retcode = 0;
    DWORD_PTR	dwret = 0;
    char_u	altname_buf[MAX_PATH];

    // Execute locally if no display or target is ourselves
    ଯଦି (serverName != ଖାଲି && STRICMP(name, serverName) == 0)
	ଫେରନ୍ତୁ sendToLocalVim(cmd, as_ପଦସମଷ୍ଟି, result);

    // If the server name does not end in a digit then we look for an
    // alternate name.  e.g. when "name" is GVIM then we may find GVIM2.
    namelen = STRLEN(name);
    ଯଦି (namelen > 1 && !vim_isdigit(name[namelen - 1]))
	altname_buf_ptr = altname_buf;
    altname_buf[0] = NUL;
    target = findServer(name);
    altname_buf_ptr = ଖାଲି;
    ଯଦି (target == 0 && altname_buf[0] != NUL)
	// Use another server name we found.
	target = findServer(altname_buf);

    ଯଦି (target == 0)
    {
	ଯଦି (!silent)
	    semsg(_(e_no_registered_server_named_str), name);
	ଫେରନ୍ତୁ -1;
    }

    ଯଦି (ptarget)
	*(HWND *)ptarget = target;

    data.dwData = as_ପଦସମଷ୍ଟି ? COPYDATA_ପଦସମଷ୍ଟି : COPYDATA_ଚାବିଗୁଡ଼ିକ;
    data.cbData = (DWORD)STRLEN(cmd) + 1;
    data.lpData = cmd;

    ଯଦି (serverSendEnc(target) < 0)
	ଫେରନ୍ତୁ -1;
    ଯଦି (SendMessageTimeout(target, WM_COPYDATA,
		(WPARAM)message_window, (LPARAM)&data,
		SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
	ଫେରନ୍ତୁ -1;
    ଯଦି (dwret == 0)
	ଫେରନ୍ତୁ -1;

    ଯଦି (as_ପଦସମଷ୍ଟି)
	retval = serverGetReply(target, &retcode, TRUE, TRUE, timeout);

    ଯଦି (result == ଖାଲି)
	vim_free(retval);
    ଅନ୍ୟଥା
	*result = retval; // Caller assumes responsibility for freeing

    ଫେରନ୍ତୁ retcode;
}

/*
 * Bring the server to the foreground.
 */
    ଶୂନ୍ୟ
serverForeground(char_u *name)
{
    HWND	target = findServer(name);

    ଯଦି (target != 0)
	SetForegroundWindow(target);
}

// Replies from server need to be stored until the client picks them up via
// remote_read(). So we maintain a list of server-id/reply pairs.
// Note that there could be multiple replies from one server pending if the
// client is slow picking them up.
// We just store the replies in a simple list. When we remove an entry, we
// move list entries down to fill the gap.
// The server ID is simply the HWND.
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    HWND	server;		// server window
    char_u	*reply;		// reply string
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ପଦସମଷ୍ଟି_result;	// 0 for REPLY, 1 for RESULT 2 for error
} reply_T;

ସ୍ଥିର garray_T reply_list = {0, 0, ଆକାର(reply_T), 5, 0};

# define REPLY_ITEM(i) ((reply_T *)(reply_list.ga_data) + (i))
# define REPLY_ଗଣନା (reply_list.ga_len)

// Flag which is used to wait for a reply
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା reply_received = 0;

/*
 * Store a reply.  "reply" must be allocated memory (or NULL).
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
save_reply(HWND server, char_u *reply, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ପଦସମଷ୍ଟି)
{
    reply_T *rep;

    ଯଦି (ga_grow(&reply_list, 1) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    rep = REPLY_ITEM(REPLY_ଗଣନା);
    rep->server = server;
    rep->reply = reply;
    rep->ପଦସମଷ୍ଟି_result = ପଦସମଷ୍ଟି;
    ଯଦି (rep->reply == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ++REPLY_ଗଣନା;
    reply_received = 1;
    ଫେରନ୍ତୁ OK;
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
serverGetReply(HWND server, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ପଦସମଷ୍ଟି_res, ପୂର୍ଣ୍ଣସଂଖ୍ୟା remove, ପୂର୍ଣ୍ଣସଂଖ୍ୟା wait, ପୂର୍ଣ୍ଣସଂଖ୍ୟା timeout)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*reply;
    reply_T	*rep;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_process = FALSE;
    time_t	start;
    time_t	now;

    // When waiting, loop until the message waiting for is received.
    time(&start);
    ପାଇଁ (;;)
    {
	// Reset this here, in case a message arrives while we are going
	// through the already received messages.
	reply_received = 0;

	ପାଇଁ (i = 0; i < REPLY_ଗଣନା; ++i)
	{
	    rep = REPLY_ITEM(i);
	    ଯଦି (rep->server == server
		    && ((rep->ପଦସମଷ୍ଟି_result != 0) == (ପଦସମଷ୍ଟି_res != ଖାଲି)))
	    {
		// Save the values we've found for later
		reply = rep->reply;
		ଯଦି (ପଦସମଷ୍ଟି_res != ଖାଲି)
		    *ପଦସମଷ୍ଟି_res = rep->ପଦସମଷ୍ଟି_result == 1 ? 0 : -1;

		ଯଦି (remove)
		{
		    // Move the rest of the list down to fill the gap
		    mch_memmove(rep, rep + 1,
				     (REPLY_ଗଣନା - i - 1) * ଆକାର(reply_T));
		    --REPLY_ଗଣନା;
		}

		// Return the reply to the caller, who takes on responsibility
		// for freeing it if "remove" is TRUE.
		ଫେରନ୍ତୁ reply;
	    }
	}

	// If we got here, we didn't find a reply. Return immediately if the
	// "wait" parameter isn't set.
	ଯଦି (!wait)
	{
	    // Process pending messages once. Without this, looping on
	    // remote_peek() would never get the reply.
	    ଯଦି (!did_process)
	    {
		did_process = TRUE;
		serverProcessPendingMessages();
		ଜାରି;
	    }
	    ଭାଙ୍ଗ;
	}

	// We need to wait for a reply. Enter a message loop until the
	// "reply_received" flag gets set.

	// Loop until we receive a reply
	ଯେତେବେଳେ (reply_received == 0)
	{
# ifdef FEAT_TIMERS
	    // TODO: use the return value to decide how long to wait.
	    check_due_timer();
# endif
	    time(&now);
	    ଯଦି (timeout > 0 && (now - start) >= timeout)
		ଭାଙ୍ଗ;

	    // Wait for a SendMessage() call to us.  This could be the reply
	    // we are waiting for.  Use a timeout of a second, to catch the
	    // situation that the server died unexpectedly.
	    ସନ୍ଦେଶ_Wait_For_Multiple_Objects(0, ଖାଲି, TRUE, 1000, QS_ALLINPUT);

	    // If the server has died, give up
	    ଯଦି (!IsWindow(server))
		ଫେରନ୍ତୁ ଖାଲି;

	    serverProcessPendingMessages();
	}
    }

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Process any messages in the Windows message queue.
 */
    ଶୂନ୍ୟ
serverProcessPendingMessages(ଶୂନ୍ୟ)
{
    ସନ୍ଦେଶ_2 ସନ୍ଦେଶ;

    ଯେତେବେଳେ (PeekMessageW(&ସନ୍ଦେଶ, ଖାଲି, 0, 0, PM_REMOVE))
    {
	TranslateMessage(&ସନ୍ଦେଶ);
	DispatchMessageW(&ସନ୍ଦେଶ);
    }
}

#endif // FEAT_CLIENTSERVER

#ଯଦି defined(FEAT_GUI) || (defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT))

ସଂରଚନା charset_pair
{
    string_T	name;
    BYTE	charset;
};

ସ୍ଥିର ସଂରଚନା charset_pair
charset_pairs[] =
{
    {STR_LITERAL_INIT("ANSI"),		ANSI_CHARSET},
    {STR_LITERAL_INIT("CHINESEBIG5"),	CHINESEBIG5_CHARSET},
    {STR_LITERAL_INIT("DEFAULT"),	DEFAULT_CHARSET},
    {STR_LITERAL_INIT("HANGEUL"),	HANGEUL_CHARSET},
    {STR_LITERAL_INIT("OEM"),		OEM_CHARSET},
    {STR_LITERAL_INIT("SHIFTJIS"),	SHIFTJIS_CHARSET},
    {STR_LITERAL_INIT("SYMBOL"),	ପ୍ରତୀକ_CHARSET},
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

ସଂରଚନା quality_pair
{
    string_T	name;
    DWORD	quality;
};

ସ୍ଥିର ସଂରଚନା quality_pair
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
    ଅକ୍ଷର *
charset_id2name(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;

    ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(charset_pairs); ++i)
    {
	ଯଦି ((BYTE)id == charset_pairs[i].charset)
	    ଫେରନ୍ତୁ (ଅକ୍ଷର *)charset_pairs[i].name.string;
    }

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Convert a quality ID to a name.
 * Return NULL when not recognized.
 */
    ଅକ୍ଷର *
quality_id2name(DWORD id)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;

    ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(quality_pairs); ++i)
    {
	ଯଦି (id == quality_pairs[i].quality)
	    ଫେରନ୍ତୁ (ଅକ୍ଷର *)quality_pairs[i].name.string;
    }

    ଫେରନ୍ତୁ ଖାଲି;
}

// The default font height in 100% scaling (96dpi).
// (-16 in 96dpi equates to roughly 12pt)
# define DEFAULT_FONT_HEIGHT	(-16)

ସ୍ଥିର ଅଚଳ LOGFONTW s_lfDefault =
{
    DEFAULT_FONT_HEIGHT,
    0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
    PROOF_QUALITY, FIXED_PITCH | FF_DONTCARE,
    L""	// Default font name will be set later based on current language.
};

// This will be initialized when set_default_logfont() is called first time.
// The value will be based on the system DPI.
ପୂର୍ଣ୍ଣସଂଖ୍ୟା current_font_height = 0;		// also used in gui_w32.c

/*
 * Convert a string representing a point size into pixels. The string should
 * be a positive decimal number, with an optional decimal point (eg, "12", or
 * "10.5"). The pixel value is returned, and a pointer to the next unconverted
 * character is stored in *end. The flag "vertical" says whether this
 * calculation is for a vertical (height) size or a horizontal (width) one.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
points_to_pixels(WCHAR *str, WCHAR **end, ପୂର୍ଣ୍ଣସଂଖ୍ୟା vertical, long_i pprinter_dc)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pixels;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		points = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଭାଜକ = 0;
    HWND	hwnd = (HWND)0;
    HDC		hdc;
    HDC		printer_dc = (HDC)pprinter_dc;

    ଯେତେବେଳେ (*str != NUL)
    {
	ଯଦି (*str == L'.' && ଭାଜକ == 0)
	{
	    // Start keeping a divisor, for later
	    ଭାଜକ = 1;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (!VIM_ISDIGIT(*str))
		ଭାଙ୍ଗ;

	    points *= 10;
	    points += *str - L'0';
	    ଭାଜକ *= 10;
	}
	++str;
    }

    ଯଦି (ଭାଜକ == 0)
	ଭାଜକ = 1;

    ଯଦି (printer_dc == ଖାଲି)
    {
	hwnd = GetDesktopWindow();
	hdc = GetWindowDC(hwnd);
    }
    ଅନ୍ୟଥା
	hdc = printer_dc;

    pixels = MulDiv(points,
		    GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX),
		    72 * ଭାଜକ);

    ଯଦି (printer_dc == ଖାଲି)
	ReleaseDC(hwnd, hdc);

    *end = str;
    ଫେରନ୍ତୁ pixels;
}

/*
 * Convert pixel into point size. This is a reverse of points_to_pixels.
 */
    ସ୍ଥିର ଦ୍ୱିଗୁଣ
pixels_to_points(ପୂର୍ଣ୍ଣସଂଖ୍ୟା pixels, ପୂର୍ଣ୍ଣସଂଖ୍ୟା vertical, long_i pprinter_dc)
{
    ଦ୍ୱିଗୁଣ	points = 0;
    HWND	hwnd = (HWND)0;
    HDC		hdc;
    HDC		printer_dc = (HDC)pprinter_dc;

    ଯଦି (printer_dc == ଖାଲି)
    {
	hwnd = GetDesktopWindow();
	hdc = GetWindowDC(hwnd);
    }
    ଅନ୍ୟଥା
	hdc = printer_dc;

    points = pixels * 72.0 / GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX);
    ଯଦି (printer_dc == ଖାଲି)
	ReleaseDC(hwnd, hdc);

    ଫେରନ୍ତୁ points;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା CALLBACK
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
    ଯଦି ((ntm->tmPitchAndFamily & 1) != 0)
	ଫେରନ୍ତୁ 1;
# endif

    // Remember this LOGFONTW as a "possible"
    *lf = elf->elfLogFont;

    // Terminate the scan as soon as we find an ANSI font
    ଯଦି (lf->lfCharSet == ANSI_CHARSET
	    || lf->lfCharSet == OEM_CHARSET
	    || lf->lfCharSet == DEFAULT_CHARSET)
	ଫେରନ୍ତୁ 0;

    // Continue the scan - we have a non-ANSI font
    ଫେରନ୍ତୁ 2;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
init_logfont(LOGFONTW *lf)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    HWND	hwnd = GetDesktopWindow();
    HDC		hdc = GetWindowDC(hwnd);

    n = EnumFontFamiliesW(hdc,
			 lf->lfFaceName,
			 (FONTENUMPROCW)font_enumproc,
			 (LPARAM)lf);

    ReleaseDC(hwnd, hdc);

    // If we couldn't find a usable font, return failure
    ଯଦି (n == 1)
	ଫେରନ୍ତୁ FAIL;

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
    ଫେରନ୍ତୁ OK;
}

/*
 * Call back for EnumFontFamiliesW in expand_font_enumproc.
 *
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା CALLBACK
expand_font_enumproc(
    ENUMLOGFONTW    *elf,
    NEWTEXTMETRICW  *ntm UNUSED,
    DWORD	    type UNUSED,
    LPARAM	    lparam)
{
    LOGFONTW *lf = (LOGFONTW*)elf;

# ifndef FEAT_PROPORTIONAL_FONTS
    // Ignore non-monospace fonts without further ado
    ଯଦି ((ntm->tmPitchAndFamily & 1) != 0)
	ଫେରନ୍ତୁ 1;
# endif

    // Filter only on ANSI. Otherwise will see a lot of random fonts that we
    // usually don't want.
    ଯଦି (lf->lfCharSet != ANSI_CHARSET)
	ଫେରନ୍ତୁ 1;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*add_match)(char_u *) = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*)(char_u *))lparam;

    WCHAR *faceNameW = lf->lfFaceName;
    char_u *faceName = utf16_to_enc(faceNameW, ଖାଲି);
    ଯଦି (!faceName)
	ଫେରନ୍ତୁ 0;

    add_match(faceName);
    vim_free(faceName);

    ଫେରନ୍ତୁ 1;
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
    ଶୂନ୍ୟ
gui_mch_expand_font(
    optexpand_T	*args,
    ଶୂନ୍ୟ	*param UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		(*add_match)(char_u *val))
{
    expand_T	    *xp = args->oe_xp;
    ଯଦି (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	ଅକ୍ଷର buf[30];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;

	// Always fill in with the current font size as first option for
	// convenience. We simply round to the closest integer for simplicity.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା font_height = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)round(
		pixels_to_points(-current_font_height, TRUE, (long_i)ଖାଲି));
	vim_snprintf(buf, ARRAY_LENGTH(buf), "h%d", font_height);
	add_match((char_u *)buf);

	// Note: Keep this in sync with get_logfont(). Don't include 'c' and
	// 'q' as we fill in all the values below.
	ସ୍ଥିର ଅକ୍ଷର *(p_gfn_win_opt_ମୂଲ୍ୟଗୁଡ଼ିକ[]) = {
	    "h" , "w" , "W" , "b" , "i" , "u" , "s"};
	ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(p_gfn_win_opt_ମୂଲ୍ୟଗୁଡ଼ିକ); i++)
	    add_match((char_u *)p_gfn_win_opt_ମୂଲ୍ୟଗୁଡ଼ିକ[i]);

	ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(charset_pairs); ++i)
	{
	    vim_snprintf(buf, ଆକାର(buf), "c%s", charset_pairs[i].name.string);
	    add_match((char_u *)buf);
	}

	ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(quality_pairs); ++i)
	{
	    vim_snprintf(buf, ଆକାର(buf), "q%s", quality_pairs[i].name.string);
	    add_match((char_u *)buf);
	}
	ଫେରନ୍ତୁ;
    }

    HWND	hwnd = GetDesktopWindow();
    HDC		hdc = GetWindowDC(hwnd);

    EnumFontFamiliesW(hdc,
	    ଖାଲି,
	    (FONTENUMPROCW)expand_font_enumproc,
	    (LPARAM)add_match);

    ReleaseDC(hwnd, hdc);
}

/*
 * Compare a UTF-16 string and an ASCII string literally.
 * Only works all the code points are inside ASCII range.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
utf16ascncmp(ଅଚଳ WCHAR *w, ଅଚଳ ଅକ୍ଷର *p, size_t n)
{
    size_t i;

    ପାଇଁ (i = 0; i < n; i++)
    {
	ଯଦି (w[i] == 0 || w[i] != p[i])
	    ଫେରନ୍ତୁ w[i] - p[i];
    }
    ଫେରନ୍ତୁ 0;
}

/*
 * Equivalent of GetDpiForSystem().
 */
    UINT WINAPI
vimGetDpiForSystem(ଶୂନ୍ୟ)
{
    HWND hwnd = GetDesktopWindow();
    HDC hdc = GetWindowDC(hwnd);
    UINT dpi = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(hwnd, hdc);
    ଫେରନ୍ତୁ dpi;
}

/*
 * Set default logfont based on current language.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_default_logfont(LOGFONTW *lf)
{
    // Default font name for current language on MS-Windows.
    // If not translated, falls back to "Consolas".
    // This must be a fixed-pitch font.
    ଅଚଳ ଅକ୍ଷର *defaultfontname = N_("DefaultFontNameForWindows");
    ଅକ୍ଷର *fontname = _(defaultfontname);

    ଯଦି (STRCMP(fontname, defaultfontname) == 0)
	fontname = "Consolas";

    *lf = s_lfDefault;
    lf->lfHeight = DEFAULT_FONT_HEIGHT * (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vimGetDpiForSystem() / 96;
    ଯଦି (current_font_height == 0)
	current_font_height = lf->lfHeight;

    WCHAR *wfontname = enc_to_utf16((char_u*)fontname, ଖାଲି);
    ଯଦି (wfontname != ଖାଲି)
    {
	wcscpy_s(lf->lfFaceName, LF_FACESIZE, wfontname);
	vim_free(wfontname);
    }
}

/*
 * Get font info from "name" into logfont "lf".
 * Return OK for a valid name, FAIL otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_logfont(
    LOGFONTW	*lf,
    char_u	*name,
    HDC		printer_dc,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		verbose)
{
    WCHAR	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = FAIL;
    ସ୍ଥିର LOGFONTW *lastlf = ଖାଲି;
    WCHAR	*wname;

    set_default_logfont(lf);
    ଯଦି (name == ଖାଲି)
	ଫେରନ୍ତୁ OK;

    wname = enc_to_utf16(name, ଖାଲି);
    ଯଦି (wname == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (wcscmp(wname, L"*") == 0)
    {
# ଯଦି defined(FEAT_GUI_MSWIN)
	CHOOSEFONTW	cf;
	// if name is "*", bring up std font dialog:
	CLEAR_FIELD(cf);
	cf.lStructSize = ଆକାର(cf);
	cf.hwndOwner = s_hwnd;
	cf.Flags = CF_SCREENFONTS | CF_FIXEDPITCHONLY | CF_INITTOLOGFONTSTRUCT;
	ଯଦି (lastlf != ଖାଲି)
	    *lf = *lastlf;
	cf.lpLogFont = lf;
	cf.nFontType = 0 ; //REGULAR_FONTTYPE;
	ଯଦି (ChooseFontW(&cf))
	    ret = OK;
# endif
	ଯାଅ theend;
    }

    /*
     * Split name up, it could be <name>:h<height>:w<width> etc.
     */
    ପାଇଁ (p = wname; *p && *p != L':'; p++)
    {
	ଯଦି (p - wname + 1 >= LF_FACESIZE)
	    ଯାଅ theend;			// Name too long
	lf->lfFaceName[p - wname] = *p;
    }
    ଯଦି (p != wname)
	lf->lfFaceName[p - wname] = NUL;

    // First set defaults
    lf->lfHeight = DEFAULT_FONT_HEIGHT * (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vimGetDpiForSystem() / 96;
    lf->lfWidth = 0;
    lf->lfWeight = FW_NORMAL;
    lf->lfItalic = FALSE;
    lf->lfUnderline = FALSE;
    lf->lfStrikeOut = FALSE;

    /*
     * If the font can't be found, try replacing '_' by ' '.
     */
    ଯଦି (init_logfont(lf) == FAIL)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	did_replace = FALSE;

	ପାଇଁ (i = 0; lf->lfFaceName[i]; ++i)
	    ଯଦି (lf->lfFaceName[i] == L'_')
	    {
		lf->lfFaceName[i] = L' ';
		did_replace = TRUE;
	    }
	ଯଦି (!did_replace || init_logfont(lf) == FAIL)
	    ଯାଅ theend;
    }

    ଯେତେବେଳେ (*p == L':')
	p++;

    // Set the values found after ':'
    ଯେତେବେଳେ (*p)
    {
	ବାଛ (*p++)
	{
	    // Note: Keep this in sync with gui_mch_expand_font().
	    ସ୍ଥିତି L'h':
		lf->lfHeight = - points_to_pixels(p, &p, TRUE, (long_i)printer_dc);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି L'w':
		lf->lfWidth = points_to_pixels(p, &p, FALSE, (long_i)printer_dc);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି L'W':
		lf->lfWeight = wcstol(p, &p, 10);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି L'b':
		lf->lfWeight = FW_BOLD;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି L'i':
		lf->lfItalic = TRUE;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି L'u':
		lf->lfUnderline = TRUE;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି L's':
		lf->lfStrikeOut = TRUE;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି L'c':
		ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(charset_pairs); ++i)
		{
		    ଯଦି (utf16ascncmp(p, (ଅକ୍ଷର *)charset_pairs[i].name.string,
			    charset_pairs[i].name.length) == 0)
		    {
			lf->lfCharSet = charset_pairs[i].charset;
			p += charset_pairs[i].name.length;
			ଭାଙ୍ଗ;
		    }
		}

		ଯଦି (i == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(charset_pairs) && verbose)
		{
		    char_u *s = utf16_to_enc(p, ଖାଲି);
		    ଯଦି (s != ଖାଲି)
		    {
			semsg(_(e_illegal_str_name_str_in_font_name_str),
							   "charset", s, name);
			vim_free(s);
		    }
		}
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି L'f':
		// Font features (e.g., "fss19=1").
		// Parsed separately by gui_mch_init_font(); skip here.
		ଯେତେବେଳେ (*p && *p != L':')
		    p++;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି L'q':
		ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(quality_pairs); ++i)
		{
		    ଯଦି (utf16ascncmp(p, (ଅକ୍ଷର *)quality_pairs[i].name.string,
			    quality_pairs[i].name.length) == 0)
		    {
			lf->lfQuality = quality_pairs[i].quality;
			p += quality_pairs[i].name.length;
			ଭାଙ୍ଗ;
		    }
		}

		ଯଦି (i == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(quality_pairs) && verbose)
		{
		    char_u *s = utf16_to_enc(p, ଖାଲି);
		    ଯଦି (s != ଖାଲି)
		    {
			semsg(_(e_illegal_str_name_str_in_font_name_str),
							   "quality", s, name);
			vim_free(s);
		    }
		}
		ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଯଦି (verbose)
		    semsg(_(e_illegal_char_nr_in_font_name_str), p[-1], name);
		ଯାଅ theend;
	}
	ଯେତେବେଳେ (*p == L':')
	    p++;
    }
    ret = OK;

theend:
    // ron: init lastlf
    ଯଦି (ret == OK && printer_dc == ଖାଲି)
    {
	vim_free(lastlf);
	lastlf = ALLOC_ONE(LOGFONTW);
	ଯଦି (lastlf != ଖାଲି)
	    mch_memmove(lastlf, lf, ଆକାର(LOGFONTW));
    }
    vim_free(wname);

    ଫେରନ୍ତୁ ret;
}

#endif // defined(FEAT_GUI) || defined(FEAT_PRINTER)

#ଯଦି defined(FEAT_JOB_CHANNEL)
/*
 * Initialize the Winsock dll.
 */
    ଶୂନ୍ୟ
channel_init_winsock(ଶୂନ୍ୟ)
{
    WSADATA wsaData;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା wsaerr;

    ଯଦି (WSInitialized)
	ଫେରନ୍ତୁ;

    wsaerr = WSAStartup(MAKEWORD(2, 2), &wsaData);
    ଯଦି (wsaerr == 0)
	WSInitialized = TRUE;
}
#endif
