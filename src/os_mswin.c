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

#അന്തര്ഭാവ "vim.h"

#അന്തര്ഭാവ <sys/types.h>
#അന്തര്ഭാവ <limits.h>

#അന്തര്ഭാവ <process.h>
#അന്തര്ഭാവ <direct.h>

#എങ്കിൽ !defined(FEAT_GUI_MSWIN)
# include <shellapi.h>
#endif

#എങ്കിൽ defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT)
# include <dlgs.h>
# include <winspool.h>
# include <commdlg.h>
#endif

// Record all output and all keyboard & mouse input
// #define MCH_WRITE_DUMP

#ifdef MCH_WRITE_DUMP
FILE* fdDump = ശൂന്യ;
#endif

#എങ്കിൽ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ബാഹ്യ അക്ഷരം g_szOrigTitle[];
#endif

#ifdef FEAT_GUI
ബാഹ്യ HWND s_hwnd;
#അല്ലെങ്കിൽ
സ്ഥിതം HWND s_hwnd = 0;	    // console window handle, set by GetConsoleHwnd()
#endif

#ifdef FEAT_JOB_CHANNEL
പൂർണ്ണസംഖ്യ WSInitialized = FALSE; // WinSock is initialized
#endif


/*
 * Save the instance handle of the exe/dll.
 */
    ശൂന്യം
SaveInst(HINSTANCE hInst)
{
    g_hinst = hInst;
}

#എങ്കിൽ defined(FEAT_GUI_MSWIN)
/*
 * GUI version of mch_exit().
 * Shut down and exit with status `r'
 * Careful: mch_exit() may be called before mch_init()!
 */
    ശൂന്യം
mch_exit_g(പൂർണ്ണസംഖ്യ r)
{
    exiting = TRUE;

    display_errors();

    ml_close_all(TRUE);		// remove all memfiles

# ifdef FEAT_OLE
    UninitOLE();
# endif
# ifdef FEAT_JOB_CHANNEL
    എങ്കിൽ (WSInitialized)
    {
	WSInitialized = FALSE;
	WSACleanup();
    }
# endif
# ifdef DYNAMIC_GETTEXT
    dyn_libintl_end();
# endif

    എങ്കിൽ (gui.in_use)
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
തരനിർവചനം BOOL (WINAPI *PfnRtlGetVersion)(LPOSVERSIONINFOW);
DWORD win_version;
    സ്ഥിതം ശൂന്യം
win_version_init(ശൂന്യം)
{
    OSVERSIONINFOW	osver;
    HMODULE		hNtdll;
    PfnRtlGetVersion	pRtlGetVersion;

    hNtdll = GetModuleHandle("ntdll.dll");
    എങ്കിൽ (hNtdll == ശൂന്യ)
	തിരികെ;

    pRtlGetVersion =
	(PfnRtlGetVersion) GetProcAddress(hNtdll, "RtlGetVersion");
    എങ്കിൽ (pRtlGetVersion == ശൂന്യ)
	തിരികെ;

    osver.dwOSVersionInfoSize = വലുപ്പം(OSVERSIONINFOW);
    pRtlGetVersion(&osver);
    win_version = MAKE_VER(min(osver.dwMajorVersion, 0xFF),
	    min(osver.dwMinorVersion, 0xFF),
	    min(osver.dw_Build_സംഖ്യ, 0xFFFF));
}

/*
 * Init the tables for toupper() and tolower().
 */
    ശൂന്യം
mch_early_init(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		i;

    win_version_init();

    PlatformId();

    // Init the tables for toupper() and tolower()
    വേണ്ടി (i = 0; i < 256; ++i)
	toupper_tab[i] = tolower_tab[i] = i;
    CharUpperBuff((LPSTR)toupper_tab, 256);
    CharLowerBuff((LPSTR)tolower_tab, 256);
}


/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    പൂർണ്ണസംഖ്യ
mch_നിവേശം_isatty(ശൂന്യം)
{
#ifdef FEAT_GUI_MSWIN
# ifdef VIMDLL
    എങ്കിൽ (gui.in_use)
# endif
	തിരികെ TRUE;	    // GUI always has a tty
#endif
#എങ്കിൽ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    എങ്കിൽ (isatty(read_cmd_fd))
	തിരികെ TRUE;
    തിരികെ FALSE;
#endif
}

/*
 * mch_settitle(): set titlebar of our window
 */
    ശൂന്യം
mch_settitle(
    char_u *title,
    char_u *icon UNUSED)
{
#ifdef FEAT_GUI_MSWIN
# ifdef VIMDLL
    എങ്കിൽ (gui.in_use)
# endif
    {
	gui_mch_settitle(title, icon);
	തിരികെ;
    }
#endif
#എങ്കിൽ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    എങ്കിൽ (title != ശൂന്യ)
    {
	WCHAR	*wp = enc_to_utf16(title, ശൂന്യ);

	എങ്കിൽ (wp == ശൂന്യ)
	    തിരികെ;

	SetConsoleTitleW(wp);
	vim_free(wp);
	തിരികെ;
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
    ശൂന്യം
mch_restore_title(പൂർണ്ണസംഖ്യ which UNUSED)
{
#എങ്കിൽ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
    എങ്കിൽ (!gui.in_use)
# endif
	SetConsoleTitle(g_szOrigTitle);
#endif
}


/*
 * Return TRUE if we can restore the title (we can)
 */
    പൂർണ്ണസംഖ്യ
mch_can_restore_title(ശൂന്യം)
{
    തിരികെ TRUE;
}


/*
 * Return TRUE if we can restore the icon title (we can't)
 */
    പൂർണ്ണസംഖ്യ
mch_can_restore_icon(ശൂന്യം)
{
    തിരികെ FALSE;
}


/*
 * Get absolute file name into buffer "buf" of length "len" bytes,
 * turning all '/'s into '\\'s and getting the correct case of each component
 * of the file name.  Append a (back)slash to a directory name.
 * When 'shellslash' set do it the other way around.
 * Return OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    പൂർണ്ണസംഖ്യ		len,
    പൂർണ്ണസംഖ്യ		force UNUSED)
{
    പൂർണ്ണസംഖ്യ		nResult = FAIL;
    WCHAR	*wname;
    WCHAR	wbuf[MAX_PATH];
    char_u	*cname = ശൂന്യ;

    wname = enc_to_utf16(fname, ശൂന്യ);
    എങ്കിൽ (wname != ശൂന്യ && _wfullpath(wbuf, wname, MAX_PATH) != ശൂന്യ)
    {
	cname = utf16_to_enc((short_u *)wbuf, ശൂന്യ);
	എങ്കിൽ (cname != ശൂന്യ)
	{
	    vim_strncpy(buf, cname, len - 1);
	    nResult = OK;
	}
    }
    vim_free(wname);
    vim_free(cname);

#ifdef USE_FNAME_CASE
    fname_case(buf, len);
#അല്ലെങ്കിൽ
    slash_adjust(buf);
#endif

    തിരികെ nResult;
}


/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    പൂർണ്ണസംഖ്യ
mch_isFullName(char_u *fname)
{
    // A name like "d:/foo" and "//server/share" is absolute.  "d:foo" is not.
    // /foo and \foo are absolute too because Windows keeps a current drive.
    // Another way to check is to use mch_FullName() and see if the result is
    // the same as the name or mch_FullName() fails.  However, this has quite a
    // bit of overhead, so let's not do that.
    എങ്കിൽ (*fname == NUL)
	തിരികെ FALSE;
    തിരികെ ((ASCII_ISALPHA(fname[0]) && fname[1] == ':'
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
    ശൂന്യം
slash_adjust(char_u *p)
{
    എങ്കിൽ (path_with_url(p))
	തിരികെ;

    എങ്കിൽ (*p == '`')
    {
	size_t len = STRLEN(p);

	// don't replace backslash in backtick quoted strings
	എങ്കിൽ (len > 2 && *(p + len - 1) == '`')
	    തിരികെ;
    }

    ആകുമ്പോൾ (*p)
    {
	എങ്കിൽ (*p == psepcN)
	    *p = psepc;
	MB_PTR_ADV(p);
    }
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
read_reparse_point(സ്ഥിരം WCHAR *name, char_u *buf, DWORD *buf_len)
{
    HANDLE h;
    BOOL ok;

    h = CreateFileW(name, FILE_READ_ATTRIBUTES,
	    FILE_SHARE_READ | FILE_SHARE_WRITE, ശൂന്യ,
	    OPEN_EXISTING,
	    FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,
	    ശൂന്യ);
    എങ്കിൽ (h == INVALID_HANDLE_മൂല്യം)
	തിരികെ FAIL;

    ok = DeviceIoControl(h, FSCTL_GET_REPARSE_POINT, ശൂന്യ, 0, buf, *buf_len,
	    buf_len, ശൂന്യ);
    CloseHandle(h);

    തിരികെ ok ? OK : FAIL;
}

    char_u *
resolve_appexeclink(char_u *fname)
{
    DWORD		attr = 0;
    പൂർണ്ണസംഖ്യ			idx;
    WCHAR		*p, *end, *wname;
    // The buffer size is arbitrarily chosen to be "big enough" (TM), the
    // ceiling should be around 16k.
    char_u		buf[4096];
    DWORD		buf_len = വലുപ്പം(buf);
    REPARSE_DATA_BUFFER *rb = (REPARSE_DATA_BUFFER *)buf;

    wname = enc_to_utf16(fname, ശൂന്യ);
    എങ്കിൽ (wname == ശൂന്യ)
	തിരികെ ശൂന്യ;

    attr = GetFileAttributesW(wname);
    എങ്കിൽ (attr == INVALID_FILE_ATTRIBUTES ||
	    (attr & FILE_ATTRIBUTE_REPARSE_POINT) == 0)
    {
	vim_free(wname);
	തിരികെ ശൂന്യ;
    }

    // The applinks are similar to symlinks but with a huge difference: they can
    // only be executed, any other I/O operation on them is bound to fail with
    // ERROR_FILE_NOT_FOUND even though the file exists.
    എങ്കിൽ (read_reparse_point(wname, buf, &buf_len) == FAIL)
    {
	vim_free(wname);
	തിരികെ ശൂന്യ;
    }
    vim_free(wname);

    എങ്കിൽ (rb->ReparseTag != IO_REPARSE_TAG_APPEXECLINK)
	തിരികെ ശൂന്യ;

    // The (undocumented) reparse buffer contains a set of N null-terminated
    // Unicode strings, the application path is stored in the third one.
    എങ്കിൽ (rb->AppExecLinkReparseBuffer.String_എണ്ണം < 3)
	തിരികെ ശൂന്യ;

    p = rb->AppExecLinkReparseBuffer.StringList;
    end = p + rb->ReparseDataLength / വലുപ്പം(WCHAR);
    വേണ്ടി (idx = 0; p < end
	    && idx < (പൂർണ്ണസംഖ്യ)rb->AppExecLinkReparseBuffer.String_എണ്ണം
	    && idx != 2; )
    {
	എങ്കിൽ (*p++ == L'\0')
	    ++idx;
    }

    തിരികെ utf16_to_enc(p, ശൂന്യ);
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
mswin_stat_impl(സ്ഥിരം WCHAR *name, stat_T *stp, സ്ഥിരം പൂർണ്ണസംഖ്യ resolve)
{
    പൂർണ്ണസംഖ്യ			n;
    പൂർണ്ണസംഖ്യ			fd;
    BOOL		is_symlink = FALSE;
    HANDLE		hFind, h;
    DWORD		attr = 0;
    DWORD		flag = 0;
    WIN32_FIND_DATAW    findDataW;

#എങ്കിൽ 0 && defined(_UCRT)
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
    എങ്കിൽ (resolve)
	// Universal CRT can handle symlinks properly.
	തിരികെ _wstat(name, stp);
#endif

    // Only reparse points need a directory query to identify symbolic links.
    // In particular, avoid enumerating the directory for each missing file
    // while searching 'runtimepath'.
    attr = GetFileAttributesW(name);
    എങ്കിൽ (attr == INVALID_FILE_ATTRIBUTES)
    {
	DWORD error = GetLastError();

	// A missing file or path cannot provide stat information.  Other
	// errors and UNC paths still need the CRT fallback.
	എങ്കിൽ ((error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND)
		&& !((name[0] == L'\\' || name[0] == L'/')
		    && (name[1] == L'\\' || name[1] == L'/')))
	{
	    errno = ENOENT;
	    തിരികെ -1;
	}
	തിരികെ _wstat(name, stp);
    }
    എങ്കിൽ (!(attr & FILE_ATTRIBUTE_REPARSE_POINT))
	തിരികെ _wstat(name, stp);

    hFind = FindFirstFileW(name, &findDataW);
    എങ്കിൽ (hFind != INVALID_HANDLE_മൂല്യം)
    {
	attr = findDataW.dwFileAttributes;
	എങ്കിൽ ((attr & FILE_ATTRIBUTE_REPARSE_POINT)
		&& (findDataW.dwReserved0 == IO_REPARSE_TAG_SYMLINK))
	    is_symlink = TRUE;
	FindClose(hFind);
    }

    // Use the plain old stat() whenever it's possible.
    എങ്കിൽ (!is_symlink)
	തിരികെ _wstat(name, stp);

    എങ്കിൽ (!resolve && is_symlink)
	flag = FILE_FLAG_OPEN_REPARSE_POINT;
    എങ്കിൽ (attr & FILE_ATTRIBUTE_DIRECTORY)
	flag |= FILE_FLAG_BACKUP_SEMANTICS;

    h = CreateFileW(name, FILE_READ_ATTRIBUTES,
	    FILE_SHARE_READ | FILE_SHARE_WRITE, ശൂന്യ, OPEN_EXISTING, flag,
	    ശൂന്യ);
    എങ്കിൽ (h == INVALID_HANDLE_മൂല്യം)
	തിരികെ -1;

    fd = _open_osfhandle((intptr_t)h, _O_RDONLY);
    n = _fstat(fd, (ഘടന _stat *)stp);
    എങ്കിൽ ((n == 0) && (attr & FILE_ATTRIBUTE_DIRECTORY))
	stp->st_mode = (stp->st_mode & ~S_IFMT) | S_IFDIR;
    _close(fd);

    തിരികെ n;
}

/*
 * stat() can't handle a trailing '/' or '\', remove it first.
 * When 'resolve' is true behave as lstat() wrt symlinks.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
stat_impl(സ്ഥിരം അക്ഷരം *name, stat_T *stp, സ്ഥിരം പൂർണ്ണസംഖ്യ resolve)
{
    // WinNT and later can use _MAX_PATH wide characters for a pathname, which
    // means that the maximum pathname is _MAX_PATH * 3 bytes when 'enc' is
    // UTF-8.
    char_u	buf[_MAX_PATH * 3 + 1];
    size_t	buflen;
    char_u	*p;
    WCHAR	*wp;
    പൂർണ്ണസംഖ്യ		n;

    vim_strncpy((char_u *)buf, (char_u *)name, വലുപ്പം(buf) - 1);
    buflen = STRLEN(buf);
    p = buf + buflen;
    എങ്കിൽ (p > buf)
	MB_PTR_BACK(buf, p);

    // Remove trailing '\\' except root path.
    എങ്കിൽ (p > buf && (*p == '\\' || *p == '/') && p[-1] != ':')
    {
	*p = NUL;
	buflen = (size_t)(p - buf);
    }

    എങ്കിൽ ((buf[0] == '\\' && buf[1] == '\\') || (buf[0] == '/' && buf[1] == '/'))
    {
	// UNC root path must be followed by '\\'.
	p = vim_strpbrk(buf + 2, (char_u *)"\\/");
	എങ്കിൽ (p != ശൂന്യ)
	{
	    p = vim_strpbrk(p + 1, (char_u *)"\\/");
	    എങ്കിൽ (p == ശൂന്യ)
		STRCPY(buf + buflen, "\\");
	}
    }

    wp = enc_to_utf16(buf, ശൂന്യ);
    എങ്കിൽ (wp == ശൂന്യ)
	തിരികെ -1;

    n = mswin_stat_impl(wp, stp, resolve);
    vim_free(wp);
    തിരികെ n;
}

    പൂർണ്ണസംഖ്യ
vim_lstat(സ്ഥിരം അക്ഷരം *name, stat_T *stp)
{
    തിരികെ stat_impl(name, stp, FALSE);
}

    പൂർണ്ണസംഖ്യ
vim_stat(സ്ഥിരം അക്ഷരം *name, stat_T *stp)
{
    തിരികെ stat_impl(name, stp, TRUE);
}

#എങ്കിൽ defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    ശൂന്യം
mch_settmode(tmode_T tmode UNUSED)
{
    // nothing to do
}

    പൂർണ്ണസംഖ്യ
mch_get_shellsize(ശൂന്യം)
{
    // never used
    തിരികെ OK;
}

    ശൂന്യം
mch_set_shellsize(ശൂന്യം)
{
    // never used
}

/*
 * Rows and/or Columns has changed.
 */
    ശൂന്യം
mch_new_shellsize(ശൂന്യം)
{
    // never used
}

    ശൂന്യം
mch_calc_cell_size(ഘടന cellsize *cs_out UNUSED)
{
    // never used
}

#endif

/*
 * We have no job control, so fake it by starting a new shell.
 */
    ശൂന്യം
mch_suspend(ശൂന്യം)
{
    suspend_shell();
}

#എങ്കിൽ defined(USE_MCH_ERRMSG)

# ifdef display_errors
#  undef display_errors
# endif

/*
 * Display the saved error message(s).
 */
    ശൂന്യം
display_errors(ശൂന്യം)
{
# ifdef FEAT_GUI
    char_u *p;

#  ifdef VIMDLL
    എങ്കിൽ (gui.in_use || gui.starting)
#  endif
    {
	എങ്കിൽ (error_ga.ga_data != ശൂന്യ)
	{
	    // avoid putting up a message box with blanks only
	    വേണ്ടി (p = (char_u *)error_ga.ga_data; *p; ++p)
		എങ്കിൽ (!SAFE_isspace(*p))
		{
		    // Only use a dialog when not using --gui-dialog-file:
		    // write text to a file.
		    എങ്കിൽ (!gui_dialog_log((char_u *)"Errors", p))
			(ശൂന്യം)gui_mch_dialog(
				     gui.starting ? VIM_INFO :
					     VIM_ERROR,
				     gui.starting ? (char_u *)_("Message") :
					     (char_u *)_("Error"),
					     p, (char_u *)_("&Ok"),
					1, ശൂന്യ, FALSE);
		    നിർത്തുക;
		}
	    ga_clear(&error_ga);
	}
	തിരികെ;
    }
# endif
# എങ്കിൽ !defined(FEAT_GUI) || defined(VIMDLL)
    FlushFileBuffers(GetStdHandle(STD_ERROR_HANDLE));
# endif
}
#endif


/*
 * Return TRUE if "p" contain a wildcard that can be expanded by
 * dos_expandpath().
 */
    പൂർണ്ണസംഖ്യ
mch_has_exp_wildcard(char_u *p)
{
    വേണ്ടി ( ; *p; MB_PTR_ADV(p))
    {
	എങ്കിൽ (vim_strchr((char_u *)"?*[", *p) != ശൂന്യ
		|| (*p == '~' && p[1] != NUL))
	    തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

/*
 * Return TRUE if "p" contain a wildcard or a "~1" kind of thing (could be a
 * shortened file name).
 */
    പൂർണ്ണസംഖ്യ
mch_has_wildcard(char_u *p)
{
    വേണ്ടി ( ; *p; MB_PTR_ADV(p))
    {
	എങ്കിൽ (vim_strchr((char_u *)"?*$[`", *p) != ശൂന്യ
		|| (*p == '~' && p[1] != NUL))
	    തിരികെ TRUE;
    }
    തിരികെ FALSE;
}


/*
 * The normal _chdir() does not change the default drive.  This one does.
 * Returning 0 implies success; -1 implies failure.
 */
    പൂർണ്ണസംഖ്യ
mch_chdir(അക്ഷരം *path)
{
    WCHAR   *p;
    പൂർണ്ണസംഖ്യ	    n;

    എങ്കിൽ (path[0] == NUL)		// just checking...
	തിരികെ -1;

    എങ്കിൽ (p_verbose >= 5)
    {
	verbose_enter();
	smsg("chdir(%s)", path);
	verbose_leave();
    }
    എങ്കിൽ (SAFE_isalpha(path[0]) && path[1] == ':')	// has a drive name
    {
	// If we can change to the drive, skip that part of the path.  If we
	// can't then the current directory may be invalid, try using chdir()
	// with the whole path.
	എങ്കിൽ (_chdrive(TOLOWER_ASC(path[0]) - 'a' + 1) == 0)
	    path += 2;
    }

    എങ്കിൽ (*path == NUL)		// drive name only
	തിരികെ 0;

    p = enc_to_utf16((char_u *)path, ശൂന്യ);
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ -1;

    n = _wchdir(p);
    vim_free(p);
    തിരികെ n;
}


#എങ്കിൽ defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
/*
 * return non-zero if a character is available
 */
    പൂർണ്ണസംഖ്യ
mch_char_avail(ശൂന്യം)
{
    // never used
    തിരികെ TRUE;
}

# എങ്കിൽ defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    പൂർണ്ണസംഖ്യ
mch_check_messages(ശൂന്യം)
{
    // TODO: check for messages
    തിരികെ TRUE;
}
# endif
#endif


#എങ്കിൽ defined(FEAT_LIBCALL)
/*
 * Call a DLL routine which takes either a string or int param
 * and returns an allocated string.
 * Return OK if it worked, FAIL if not.
 */
തരനിർവചനം char_u *(*MYSTRPROCSTR)(char_u *);
തരനിർവചനം പൂർണ്ണസംഖ്യ (*MYSTRPROCINT)(char_u *);
തരനിർവചനം char_u *(*MYINTPROCSTR)(പൂർണ്ണസംഖ്യ);
തരനിർവചനം പൂർണ്ണസംഖ്യ (*MYINTPROCINT)(പൂർണ്ണസംഖ്യ);

/*
 * Check if a pointer points to a valid NUL terminated string.
 * Return the length of the string, including terminating NUL.
 * Returns 0 for an invalid pointer, 1 for an empty string.
 */
    സ്ഥിതം size_t
check_str_len(char_u *str)
{
    SYSTEM_INFO			si;
    MEMORY_BASIC_INFORMATION	mbi;
    size_t			length = 0;
    size_t			i;
    സ്ഥിരം char_u		*p;

    // get page size
    GetSystemInfo(&si);

    // get memory information
    എങ്കിൽ (!VirtualQuery(str, &mbi, വലുപ്പം(mbi)))
	തിരികെ 0;

    // pre cast these (typing savers)
    long_u dwStr = (long_u)str;
    long_u dwBaseAddress = (long_u)mbi.BaseAddress;

    // get start address of page that str is on
    long_u strPage = dwStr - (dwStr - dwBaseAddress) % si.dwPageSize;

    // get length from str to end of page
    long_u pageLength = si.dwPageSize - (dwStr - strPage);

    വേണ്ടി (p = str; !IsBadReadPtr(p, (UINT)pageLength);
	    p += pageLength, pageLength = si.dwPageSize)
	വേണ്ടി (i = 0; i < pageLength; ++i, ++length)
	    എങ്കിൽ (p[i] == NUL)
		തിരികെ length + 1;

    തിരികെ 0;
}

/*
 * Passed to do_in_runtimepath() to load a vim.ico file.
 */
    സ്ഥിതം ശൂന്യം
mch_icon_load_cb(char_u *fname, ശൂന്യം *cookie)
{
    HANDLE *h = (HANDLE *)cookie;

    *h = LoadImage(ശൂന്യ,
		   (LPSTR)fname,
		   IMAGE_ICON,
		   64,
		   64,
		   LR_LOADFROMFILE | LR_LOADMAP3DCOLORS);
}

/*
 * Try loading an icon file from 'runtimepath'.
 */
    പൂർണ്ണസംഖ്യ
mch_icon_load(HANDLE *iconp)
{
    തിരികെ do_in_runtimepath((char_u *)"bitmaps/vim.ico",
						  0, mch_icon_load_cb, iconp);
}

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if the OS PRNG is not available or something went wrong.
 */
    പൂർണ്ണസംഖ്യ
mch_get_random(char_u *buf, പൂർണ്ണസംഖ്യ len)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ		initialized = NOTDONE;
    സ്ഥിതം HINSTANCE	hInstLib;
    സ്ഥിതം BOOL (WINAPI *pProcessPrng)(PUCHAR, ULONG);

    എങ്കിൽ (initialized == NOTDONE)
    {
	hInstLib = vimLoadLib("bcryptprimitives.dll");
	എങ്കിൽ (hInstLib != ശൂന്യ)
	    pProcessPrng = (ശൂന്യം *)GetProcAddress(hInstLib, "ProcessPrng");
	എങ്കിൽ (hInstLib == ശൂന്യ || pProcessPrng == ശൂന്യ)
	{
	    FreeLibrary(hInstLib);
	    initialized = FAIL;
	}
	അല്ലെങ്കിൽ
	    initialized = OK;
    }

    എങ്കിൽ (initialized == FAIL)
	തിരികെ FAIL;

    // According to the documentation this call cannot fail.
    pProcessPrng(buf, len);

    തിരികെ OK;
}

    പൂർണ്ണസംഖ്യ
mch_libcall(
    char_u	*libname,
    char_u	*funcname,
    char_u	*argstring,	// NULL when using a argint
    പൂർണ്ണസംഖ്യ		argint,
    char_u	**string_result,// NULL when using number_result
    പൂർണ്ണസംഖ്യ		*സംഖ്യ_result)
{
    HINSTANCE		hinstLib;

    BOOL fRunTimeLinkSuccess = FALSE;

    // Get a handle to the DLL module.
    hinstLib = vimLoadLib((അക്ഷരം *)libname);

    // If the handle is valid, try to get the function address.
    എങ്കിൽ (hinstLib != ശൂന്യ)
    {
	char_u	*retval_str = ശൂന്യ;
	പൂർണ്ണസംഖ്യ	retval_int = 0;
	size_t	len;

# ifdef HAVE_TRY_EXCEPT
	__try
	{
# endif
	എങ്കിൽ (argstring != ശൂന്യ)
	{
	    // Call with string argument
	    എങ്കിൽ (string_result != ശൂന്യ)
	    {
		MYSTRPROCSTR proc;

		proc = (MYSTRPROCSTR)GetProcAddress(hinstLib, (LPCSTR)funcname);
		എങ്കിൽ (proc != ശൂന്യ)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_str = proc(argstring);
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		MYSTRPROCINT proc;

		proc = (MYSTRPROCINT)GetProcAddress(hinstLib, (LPCSTR)funcname);
		എങ്കിൽ (proc != ശൂന്യ)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_int = proc(argstring);
		}
	    }
	}
	അല്ലെങ്കിൽ
	{
	    // Call with number argument
	    എങ്കിൽ (string_result != ശൂന്യ)
	    {
		MYINTPROCSTR proc;

		proc = (MYINTPROCSTR)GetProcAddress(hinstLib, (LPCSTR)funcname);
		എങ്കിൽ (proc != ശൂന്യ)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_str = proc(argint);
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		MYINTPROCINT proc;

		proc = (MYINTPROCINT)GetProcAddress(hinstLib, (LPCSTR)funcname);
		എങ്കിൽ (proc != ശൂന്യ)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_int = proc(argint);
		}
	    }
	}

	// Save the string before we free the library.
	// Assume that a "1" result is an illegal pointer.
	എങ്കിൽ (string_result == ശൂന്യ)
	    *സംഖ്യ_result = retval_int;
	അല്ലെങ്കിൽ എങ്കിൽ (retval_str != ശൂന്യ
		&& (len = check_str_len(retval_str)) > 0)
	{
	    *string_result = alloc(len);
	    എങ്കിൽ (*string_result != ശൂന്യ)
		mch_memmove(*string_result, retval_str, len);
	}

# ifdef HAVE_TRY_EXCEPT
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	    എങ്കിൽ (GetExceptionCode() == EXCEPTION_അട്ടി_OVERFLOW)
		_resetstkoflw();
	    fRunTimeLinkSuccess = FALSE;
	}
# endif

	// Free the DLL module.
	(ശൂന്യം)FreeLibrary(hinstLib);
    }

    എങ്കിൽ (!fRunTimeLinkSuccess)
    {
	semsg(_(e_library_call_failed_for_str), funcname);
	തിരികെ FAIL;
    }

    തിരികെ OK;
}
#endif

/*
 * Debugging helper: expose the MCH_WRITE_DUMP stuff to other modules
 */
    ശൂന്യം
DumpPutS(സ്ഥിരം അക്ഷരം *psz UNUSED)
{
#ifdef MCH_WRITE_DUMP
    എങ്കിൽ (fdDump)
    {
	fputs(psz, fdDump);
	എങ്കിൽ (psz[strlen(psz) - 1] != '\n')
	    fputc('\n', fdDump);
	fflush(fdDump);
    }
#endif
}

#ifdef _DEBUG

ശൂന്യം __cdecl
Trace(
    അക്ഷരം *pszFormat,
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

#എങ്കിൽ !defined(FEAT_GUI) || defined(VIMDLL)
ബാഹ്യ HWND g_hWnd;	// This is in os_win32.c.

/*
 * Showing the printer dialog is tricky since we have no GUI
 * window to parent it. The following routines are needed to
 * get the window parenting and Z-order to work properly.
 */
    സ്ഥിതം ശൂന്യം
GetConsoleHwnd(ശൂന്യം)
{
    // Skip if it's already set.
    എങ്കിൽ (s_hwnd != 0)
	തിരികെ;

    // Window handle may have been found by init code (Windows NT only)
    എങ്കിൽ (g_hWnd != 0)
    {
	s_hwnd = g_hWnd;
	തിരികെ;
    }

    s_hwnd = GetConsoleWindow();
}

/*
 * Console implementation of ":winpos".
 */
    പൂർണ്ണസംഖ്യ
mch_get_winpos(പൂർണ്ണസംഖ്യ *x, പൂർണ്ണസംഖ്യ *y)
{
    RECT  rect;

    GetConsoleHwnd();
    GetWindowRect(s_hwnd, &rect);
    *x = rect.ഇടത്;
    *y = rect.മുകൾ;
    തിരികെ OK;
}

/*
 * Console implementation of ":winpos x y".
 */
    ശൂന്യം
mch_set_winpos(പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y)
{
    GetConsoleHwnd();
    SetWindowPos(s_hwnd, ശൂന്യ, x, y, 0, 0,
		 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}
#endif

#എങ്കിൽ defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT)

//=================================================================
// Win32 printer stuff

സ്ഥിതം HFONT		prt_font_handles[2][2][2];
സ്ഥിതം PRINTDLGW	prt_dlg;
സ്ഥിതം സ്ഥിരം പൂർണ്ണസംഖ്യ	boldface[2] = {FW_REGULAR, FW_BOLD};
സ്ഥിതം TEXTMETRIC	prt_tm;
സ്ഥിതം പൂർണ്ണസംഖ്യ		prt_വരി_height;
സ്ഥിതം പൂർണ്ണസംഖ്യ		prt_സംഖ്യ_width;
സ്ഥിതം പൂർണ്ണസംഖ്യ		prt_ഇടത്_margin;
സ്ഥിതം പൂർണ്ണസംഖ്യ		prt_വലത്_margin;
സ്ഥിതം പൂർണ്ണസംഖ്യ		prt_മുകൾ_margin;
സ്ഥിതം char_u		szAppName[] = TEXT("VIM");
സ്ഥിതം HWND		hDlgPrint;
സ്ഥിതം പൂർണ്ണസംഖ്യ		*bUserAbort = ശൂന്യ;
സ്ഥിതം char_u		*prt_name = ശൂന്യ;

// Defines which are also in vim.rc.
# define IDC_BOX1		400
# define IDC_PRINTTEXT1		401
# define IDC_PRINTTEXT2		402
# define IDC_PROGRESS		403

    സ്ഥിതം BOOL
vimSetDlgItemText(HWND hDlg, പൂർണ്ണസംഖ്യ nIDDlgItem, char_u *s)
{
    WCHAR   *wp;
    BOOL    ret;

    wp = enc_to_utf16(s, ശൂന്യ);
    എങ്കിൽ (wp == ശൂന്യ)
	തിരികെ FALSE;

    ret = SetDlgItemTextW(hDlg, nIDDlgItem, wp);
    vim_free(wp);
    തിരികെ ret;
}

/*
 * Convert BGR to RGB for Windows GDI calls
 */
    സ്ഥിതം COLORREF
swap_me(COLORREF colorref)
{
    പൂർണ്ണസംഖ്യ  temp;
    അക്ഷരം *ptr = (അക്ഷരം *)&colorref;

    temp = *(ptr);
    *(ptr ) = *(ptr + 2);
    *(ptr + 2) = temp;
    തിരികെ colorref;
}

    സ്ഥിതം INT_PTR CALLBACK
PrintDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam UNUSED,
	LPARAM lParam UNUSED)
{
# ifdef FEAT_GETTEXT
    NONCLIENTMETRICS nm;
    സ്ഥിതം HFONT hfont;
# endif

    തിരഞ്ഞെടുപ്പ് (message)
    {
	അവസ്ഥ WM_INITDIALOG:
# ifdef FEAT_GETTEXT
	    nm.cbSize = വലുപ്പം(NONCLIENTMETRICS);
	    എങ്കിൽ (SystemParametersInfo(
			SPI_GETNONCLIENTMETRICS,
			വലുപ്പം(NONCLIENTMETRICS),
			&nm,
			0))
	    {
		അക്ഷരം buff[MAX_PATH];
		പൂർണ്ണസംഖ്യ i;

		// Translate the dialog texts
		hfont = CreateFontIndirect(&nm.lfMessageFont);
		വേണ്ടി (i = IDC_PRINTTEXT1; i <= IDC_PROGRESS; i++)
		{
		    SendDlgItemMessage(hDlg, i, WM_SETFONT, (WPARAM)hfont, 1);
		    എങ്കിൽ (GetDlgItemText(hDlg,i, buff, വലുപ്പം(buff)))
			vimSetDlgItemText(hDlg,i, (char_u *)_(buff));
		}
		SendDlgItemMessage(hDlg, IDCANCEL,
						WM_SETFONT, (WPARAM)hfont, 1);
		എങ്കിൽ (GetDlgItemText(hDlg,IDCANCEL, buff, വലുപ്പം(buff)))
		    vimSetDlgItemText(hDlg,IDCANCEL, (char_u *)_(buff));
	    }
# endif
	    SetWindowText(hDlg, (LPCSTR)szAppName);
	    എങ്കിൽ (prt_name != ശൂന്യ)
	    {
		vimSetDlgItemText(hDlg, IDC_PRINTTEXT2, (char_u *)prt_name);
		VIM_CLEAR(prt_name);
	    }
	    EnableMenuItem(GetSystemMenu(hDlg, FALSE), SC_CLOSE, MF_GRAYED);
# എങ്കിൽ !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	    എങ്കിൽ (!gui.in_use)
#  endif
		Bring_Window_To_മുകൾ(s_hwnd);
# endif
	    തിരികെ TRUE;

	അവസ്ഥ WM_COMMAND:
	    *bUserAbort = TRUE;
	    EnableWindow(GetParent(hDlg), TRUE);
	    DestroyWindow(hDlg);
	    hDlgPrint = ശൂന്യ;
# ifdef FEAT_GETTEXT
	    DeleteObject(hfont);
# endif
	    തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

    സ്ഥിതം BOOL CALLBACK
AbortProc(HDC hdcPrn UNUSED, പൂർണ്ണസംഖ്യ iCode UNUSED)
{
    സന്ദേശം_2 സന്ദേശം;

    ആകുമ്പോൾ (!*bUserAbort && PeekMessageW(&സന്ദേശം, ശൂന്യ, 0, 0, PM_REMOVE))
    {
	എങ്കിൽ (!hDlgPrint || !IsDialogMessageW(hDlgPrint, &സന്ദേശം))
	{
	    TranslateMessage(&സന്ദേശം);
	    DispatchMessageW(&സന്ദേശം);
	}
    }
    തിരികെ !*bUserAbort;
}

    സ്ഥിതം UINT_PTR CALLBACK
PrintHookProc(
	HWND hDlg,	// handle to dialog box
	UINT ui_സന്ദേശം,	// message identifier
	WPARAM wParam UNUSED,	// message parameter
	LPARAM lParam	// message parameter
	)
{
    HWND	hwndOwner;
    RECT	rc, rcDlg, rcOwner;
    PRINTDLGW	*pPD;

    എങ്കിൽ (ui_സന്ദേശം != WM_INITDIALOG)
	തിരികെ FALSE;

    // Get the owner window and dialog box rectangles.
    എങ്കിൽ ((hwndOwner = GetParent(hDlg)) == ശൂന്യ)
	hwndOwner = GetDesktopWindow();

    GetWindowRect(hwndOwner, &rcOwner);
    GetWindowRect(hDlg, &rcDlg);
    CopyRect(&rc, &rcOwner);

    // Offset the owner and dialog box rectangles so that
    // right and bottom values represent the width and
    // height, and then offset the owner again to discard
    // space taken up by the dialog box.

    OffsetRect(&rcDlg, -rcDlg.ഇടത്, -rcDlg.മുകൾ);
    OffsetRect(&rc, -rc.ഇടത്, -rc.മുകൾ);
    OffsetRect(&rc, -rcDlg.വലത്, -rcDlg.bottom);

    // The new position is the sum of half the remaining
    // space and the owner's original position.

    SetWindowPos(hDlg,
	    HWND_മുകൾ,
	    rcOwner.ഇടത് + (rc.വലത് / 2),
	    rcOwner.മുകൾ + (rc.bottom / 2),
	    0, 0,		// ignores size arguments
	    SWP_NOSIZE);

    //  tackle the printdlg copiesctrl problem
    pPD = (PRINTDLGW *)lParam;
    pPD->nCopies = (WORD)pPD->lCustData;
    SetDlgItemInt( hDlg, edt3, pPD->nCopies, FALSE );
    //  Bring the window to top
    Bring_Window_To_മുകൾ(GetParent(hDlg));
    SetForegroundWindow(hDlg);

    തിരികെ FALSE;
}

    ശൂന്യം
mch_print_cleanup(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ pifItalic;
    പൂർണ്ണസംഖ്യ pifBold;
    പൂർണ്ണസംഖ്യ pifUnderline;

    വേണ്ടി (pifBold = 0; pifBold <= 1; pifBold++)
	വേണ്ടി (pifItalic = 0; pifItalic <= 1; pifItalic++)
	    വേണ്ടി (pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
		DeleteObject(prt_font_handles[pifBold][pifItalic][pifUnderline]);

    എങ്കിൽ (prt_dlg.hDC != ശൂന്യ)
	DeleteDC(prt_dlg.hDC);
    എങ്കിൽ (!*bUserAbort && hDlgPrint != ശൂന്യ)
	SendMessage(hDlgPrint, WM_COMMAND, 0, 0);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
to_device_units(പൂർണ്ണസംഖ്യ idx, പൂർണ്ണസംഖ്യ dpi, പൂർണ്ണസംഖ്യ physsize, പൂർണ്ണസംഖ്യ offset, പൂർണ്ണസംഖ്യ def_സംഖ്യ)
{
    പൂർണ്ണസംഖ്യ		ret = 0;
    പൂർണ്ണസംഖ്യ		u;
    പൂർണ്ണസംഖ്യ		nr;

    u = prt_get_unit(idx);
    എങ്കിൽ (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_സംഖ്യ;
    }
    അല്ലെങ്കിൽ
	nr = printer_opts[idx].സംഖ്യ;

    തിരഞ്ഞെടുപ്പ് (u)
    {
	അവസ്ഥ PRT_UNIT_PERC:
	    ret = (physsize * nr) / 100;
	    നിർത്തുക;
	അവസ്ഥ PRT_UNIT_INCH:
	    ret = (nr * dpi);
	    നിർത്തുക;
	അവസ്ഥ PRT_UNIT_MM:
	    ret = (nr * 10 * dpi) / 254;
	    നിർത്തുക;
	അവസ്ഥ PRT_UNIT_POINT:
	    ret = (nr * 10 * dpi) / 720;
	    നിർത്തുക;
    }

    എങ്കിൽ (ret < offset)
	തിരികെ 0;
    അല്ലെങ്കിൽ
	തിരികെ ret - offset;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
prt_get_cpl(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		hr;
    പൂർണ്ണസംഖ്യ		phyw;
    പൂർണ്ണസംഖ്യ		dvoff;
    പൂർണ്ണസംഖ്യ		rev_offset;
    പൂർണ്ണസംഖ്യ		dpi;

    GetTextMetrics(prt_dlg.hDC, &prt_tm);
    prt_വരി_height = prt_tm.tmHeight + prt_tm.tmExternalLeading;

    hr	    = GetDeviceCaps(prt_dlg.hDC, HORZRES);
    phyw    = GetDeviceCaps(prt_dlg.hDC, PHYSICALWIDTH);
    dvoff   = GetDeviceCaps(prt_dlg.hDC, PHYSICALOFFSETX);
    dpi	    = GetDeviceCaps(prt_dlg.hDC, LOGPIXELSX);

    rev_offset = phyw - (dvoff + hr);

    prt_ഇടത്_margin = to_device_units(OPT_PRINT_ഇടത്, dpi, phyw, dvoff, 10);
    എങ്കിൽ (prt_use_സംഖ്യ())
    {
	prt_സംഖ്യ_width = PRINT_സംഖ്യ_WIDTH * prt_tm.tmAveCharWidth;
	prt_ഇടത്_margin += prt_സംഖ്യ_width;
    }
    അല്ലെങ്കിൽ
	prt_സംഖ്യ_width = 0;

    prt_വലത്_margin = hr - to_device_units(OPT_PRINT_വലത്, dpi, phyw,
							       rev_offset, 5);

    തിരികെ (prt_വലത്_margin - prt_ഇടത്_margin) / prt_tm.tmAveCharWidth;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
prt_get_lpp(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ vr;
    പൂർണ്ണസംഖ്യ phyw;
    പൂർണ്ണസംഖ്യ dvoff;
    പൂർണ്ണസംഖ്യ rev_offset;
    പൂർണ്ണസംഖ്യ	bottom_margin;
    പൂർണ്ണസംഖ്യ	dpi;

    vr	    = GetDeviceCaps(prt_dlg.hDC, VERTRES);
    phyw    = GetDeviceCaps(prt_dlg.hDC, PHYSICALHEIGHT);
    dvoff   = GetDeviceCaps(prt_dlg.hDC, PHYSICALOFFSETY);
    dpi	    = GetDeviceCaps(prt_dlg.hDC, LOGPIXELSY);

    rev_offset = phyw - (dvoff + vr);

    prt_മുകൾ_margin = to_device_units(OPT_PRINT_മുകൾ, dpi, phyw, dvoff, 5);

    // adjust top margin if there is a header
    prt_മുകൾ_margin += prt_വരി_height * prt_header_height();

    bottom_margin = vr - to_device_units(OPT_PRINT_BOT, dpi, phyw,
							       rev_offset, 5);

    തിരികെ (bottom_margin - prt_മുകൾ_margin) / prt_വരി_height;
}

    പൂർണ്ണസംഖ്യ
mch_print_init(prt_settings_T *psettings, char_u *jobname, പൂർണ്ണസംഖ്യ forceit)
{
    സ്ഥിതം HGLOBAL	stored_dm = ശൂന്യ;
    സ്ഥിതം HGLOBAL	stored_devn = ശൂന്യ;
    സ്ഥിതം പൂർണ്ണസംഖ്യ		stored_nCopies = 1;
    സ്ഥിതം പൂർണ്ണസംഖ്യ		stored_nFlags = 0;

    LOGFONTW		fLogFont;
    പൂർണ്ണസംഖ്യ			pifItalic;
    പൂർണ്ണസംഖ്യ			pifBold;
    പൂർണ്ണസംഖ്യ			pifUnderline;

    DEVMODEW		*mem;
    DEVNAMES		*devname;
    പൂർണ്ണസംഖ്യ			i;
    DWORD		err;

    bUserAbort = &(psettings->user_abort);
    CLEAR_FIELD(prt_dlg);
    prt_dlg.lStructSize = വലുപ്പം(PRINTDLGW);
# എങ്കിൽ !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
    എങ്കിൽ (!gui.in_use)
#  endif
	GetConsoleHwnd();	    // get value of s_hwnd
# endif
    prt_dlg.hwndOwner = s_hwnd;
    prt_dlg.Flags = PD_NOPAGENUMS | PD_NOSELECTION | PD_RETURNDC;
    എങ്കിൽ (!forceit)
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
    എങ്കിൽ (forceit
# എങ്കിൽ !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	    || (!gui.in_use && !term_console)
#  അല്ലെങ്കിൽ
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
	എങ്കിൽ (*p_pdev != NUL)
	    prt_dlg.hDC = CreateDC(ശൂന്യ, (LPCSTR)p_pdev, ശൂന്യ, ശൂന്യ);
	അല്ലെങ്കിൽ
	{
	    prt_dlg.Flags |= PD_RETURNDEFAULT;
	    എങ്കിൽ (PrintDlgW(&prt_dlg) == 0)
		പോകുക init_fail_dlg;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (PrintDlgW(&prt_dlg) == 0)
	പോകുക init_fail_dlg;
    അല്ലെങ്കിൽ
    {
	/*
	 * keep the previous driver context
	 */
	stored_dm = prt_dlg.hDevMode;
	stored_devn = prt_dlg.hDevNames;
	stored_nFlags = prt_dlg.Flags;
	stored_nCopies = prt_dlg.nCopies;
    }

    എങ്കിൽ (prt_dlg.hDC == ശൂന്യ)
    {
	emsg(_(e_printer_selection_failed));
	mch_print_cleanup();
	തിരികെ FALSE;
    }

    // Not all printer drivers report the support of color (or grey) in the
    // same way.  Let's set has_color if there appears to be some way to print
    // more than B&W.
    i = GetDeviceCaps(prt_dlg.hDC, NUMCOLORS);
    psettings->has_color = (GetDeviceCaps(prt_dlg.hDC, BITSPIXEL) > 1
				   || GetDeviceCaps(prt_dlg.hDC, PLANES) > 1
				   || i > 2 || i == -1);

    // Ensure all font styles are baseline aligned
    SetTextAlign(prt_dlg.hDC, TA_BASELINE|TA_ഇടത്);

    /*
     * On some windows systems the nCopies parameter is not
     * passed back correctly. It must be retrieved from the
     * hDevMode struct.
     */
    mem = (DEVMODEW *)GlobalLock(prt_dlg.hDevMode);
    എങ്കിൽ (mem != ശൂന്യ)
    {
	എങ്കിൽ (mem->dmCopies != 1)
	    stored_nCopies = mem->dmCopies;
	എങ്കിൽ ((mem->dmFields & DM_DUPLEX) && (mem->dmDuplex & ~DMDUP_SIMPLEX))
	    psettings->duplex = TRUE;
	എങ്കിൽ ((mem->dmFields & DM_COLOR) && (mem->dmColor & DMCOLOR_COLOR))
	    psettings->has_color = TRUE;
    }
    GlobalUnlock(prt_dlg.hDevMode);

    devname = (DEVNAMES *)GlobalLock(prt_dlg.hDevNames);
    എങ്കിൽ (devname != 0)
    {
	WCHAR	*wprinter_name = (WCHAR *)devname + devname->wDeviceOffset;
	WCHAR	*wport_name = (WCHAR *)devname + devname->wOutputOffset;
	char_u	*text = (char_u *)_("to %s on %s");
	char_u  *printer_name = utf16_to_enc(wprinter_name, ശൂന്യ);
	char_u	*port_name = utf16_to_enc(wport_name, ശൂന്യ);

	എങ്കിൽ (printer_name != ശൂന്യ && port_name != ശൂന്യ)
	    prt_name = alloc(STRLEN(printer_name)
					   + STRLEN(port_name) + STRLEN(text));
	എങ്കിൽ (prt_name != ശൂന്യ)
	    wsprintf((അക്ഷരം *)prt_name, (സ്ഥിരം അക്ഷരം *)text,
		    printer_name, port_name);
	vim_free(printer_name);
	vim_free(port_name);
    }
    GlobalUnlock(prt_dlg.hDevNames);

    /*
     * Initialise the font according to 'printfont'
     */
    CLEAR_FIELD(fLogFont);
    എങ്കിൽ (get_logfont(&fLogFont, p_pfn, prt_dlg.hDC, TRUE) == FAIL)
    {
	semsg(_(e_unknown_printer_font_str), p_pfn);
	mch_print_cleanup();
	തിരികെ FALSE;
    }

    വേണ്ടി (pifBold = 0; pifBold <= 1; pifBold++)
	വേണ്ടി (pifItalic = 0; pifItalic <= 1; pifItalic++)
	    വേണ്ടി (pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
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
    psettings->chars_per_വരി = prt_get_cpl();
    psettings->വരികൾ_per_page = prt_get_lpp();
    എങ്കിൽ (prt_dlg.Flags & PD_USEDEVMODECOPIESANDCOLLATE)
    {
	psettings->n_collated_copies = (prt_dlg.Flags & PD_COLLATE)
						    ? prt_dlg.nCopies : 1;
	psettings->n_uncollated_copies = (prt_dlg.Flags & PD_COLLATE)
						    ? 1 : prt_dlg.nCopies;

	എങ്കിൽ (psettings->n_collated_copies == 0)
	    psettings->n_collated_copies = 1;

	എങ്കിൽ (psettings->n_uncollated_copies == 0)
	    psettings->n_uncollated_copies = 1;
    }
    അല്ലെങ്കിൽ
    {
	psettings->n_collated_copies = 1;
	psettings->n_uncollated_copies = 1;
    }

    psettings->jobname = jobname;

    തിരികെ TRUE;

init_fail_dlg:
    err = CommDlgExtendedError();
    എങ്കിൽ (err)
    {
	char_u *buf;

	// I suspect FormatMessage() doesn't work for values returned by
	// CommDlgExtendedError().  What does?
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		ശൂന്യ, err, 0, (LPTSTR)(&buf), 0, ശൂന്യ);
	semsg(_(e_print_error_str),
		buf == ശൂന്യ ? (char_u *)_("Unknown") : buf);
	LocalFree((LPVOID)(buf));
    }
    അല്ലെങ്കിൽ
	സന്ദേശം_clr_eos(); // Maybe canceled

    mch_print_cleanup();
    തിരികെ FALSE;
}


    പൂർണ്ണസംഖ്യ
mch_print_begin(prt_settings_T *psettings)
{
    പൂർണ്ണസംഖ്യ			ret = 0;
    അക്ഷരം		szBuffer[300];
    WCHAR		*wp;

    hDlgPrint = CreateDialog(g_hinst, TEXT("PrintDlgBox"),
					     prt_dlg.hwndOwner, PrintDlgProc);
    SetAbortProc(prt_dlg.hDC, AbortProc);
    wsprintf(szBuffer, _("Printing '%s'"), gettail(psettings->jobname));
    vimSetDlgItemText(hDlgPrint, IDC_PRINTTEXT1, (char_u *)szBuffer);

    wp = enc_to_utf16(psettings->jobname, ശൂന്യ);
    എങ്കിൽ (wp != ശൂന്യ)
    {
	DOCINFOW	di;

	CLEAR_FIELD(di);
	di.cbSize = വലുപ്പം(di);
	di.lpszDocName = wp;
	ret = StartDocW(prt_dlg.hDC, &di);
	vim_free(wp);
    }

# ifdef FEAT_GUI
    // Give focus back to main window (when using MDI).
#  ifdef VIMDLL
    എങ്കിൽ (gui.in_use)
#  endif
	SetFocus(s_hwnd);
# endif

    തിരികെ (ret > 0);
}

    ശൂന്യം
mch_print_end(prt_settings_T *psettings UNUSED)
{
    EndDoc(prt_dlg.hDC);
    എങ്കിൽ (!*bUserAbort && hDlgPrint != ശൂന്യ)
	SendMessage(hDlgPrint, WM_COMMAND, 0, 0);
}

    പൂർണ്ണസംഖ്യ
mch_print_end_page(ശൂന്യം)
{
    തിരികെ (EndPage(prt_dlg.hDC) > 0);
}

    പൂർണ്ണസംഖ്യ
mch_print_begin_page(char_u *സന്ദേശം)
{
    എങ്കിൽ (സന്ദേശം != ശൂന്യ)
	vimSetDlgItemText(hDlgPrint, IDC_PROGRESS, സന്ദേശം);
    തിരികെ (StartPage(prt_dlg.hDC) > 0);
}

    പൂർണ്ണസംഖ്യ
mch_print_blank_page(ശൂന്യം)
{
    തിരികെ (mch_print_begin_page(ശൂന്യ) ? (mch_print_end_page()) : FALSE);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ prt_pos_x = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ prt_pos_y = 0;

    ശൂന്യം
mch_print_start_വരി(പൂർണ്ണസംഖ്യ margin, പൂർണ്ണസംഖ്യ page_വരി)
{
    എങ്കിൽ (margin)
	prt_pos_x = -prt_സംഖ്യ_width;
    അല്ലെങ്കിൽ
	prt_pos_x = 0;
    prt_pos_y = page_വരി * prt_വരി_height
				 + prt_tm.tmAscent + prt_tm.tmExternalLeading;
}

    പൂർണ്ണസംഖ്യ
mch_print_text_out(char_u *p, പൂർണ്ണസംഖ്യ len)
{
    SIZE	sz;
    WCHAR	*wp;
    പൂർണ്ണസംഖ്യ		wlen = len;
    പൂർണ്ണസംഖ്യ		ret = FALSE;

    wp = enc_to_utf16(p, &wlen);
    എങ്കിൽ (wp == ശൂന്യ)
	തിരികെ FALSE;

    TextOutW(prt_dlg.hDC, prt_pos_x + prt_ഇടത്_margin,
	    prt_pos_y + prt_മുകൾ_margin, wp, wlen);
    GetTextExtentPoint32W(prt_dlg.hDC, wp, wlen, &sz);
    vim_free(wp);
    prt_pos_x += (sz.cx - prt_tm.tmOverhang);
    // This is wrong when printing spaces for a TAB.
    എങ്കിൽ (p[len] != NUL)
    {
	wlen = mb_ptr2len(p + len);
	wp = enc_to_utf16(p + len, &wlen);
	എങ്കിൽ (wp != ശൂന്യ)
	{
	    GetTextExtentPoint32W(prt_dlg.hDC, wp, 1, &sz);
	    ret = (prt_pos_x + prt_ഇടത്_margin + sz.cx > prt_വലത്_margin);
	    vim_free(wp);
	}
    }
    തിരികെ ret;
}

    ശൂന്യം
mch_print_set_font(പൂർണ്ണസംഖ്യ iBold, പൂർണ്ണസംഖ്യ iItalic, പൂർണ്ണസംഖ്യ iUnderline)
{
    SelectObject(prt_dlg.hDC, prt_font_handles[iBold][iItalic][iUnderline]);
}

    ശൂന്യം
mch_print_set_bg(long_u bgcol)
{
    SetBkColor(prt_dlg.hDC, GetNearestColor(prt_dlg.hDC,
						   swap_me((COLORREF)bgcol)));
    /*
     * With a white background we can draw characters transparent, which is
     * good for italic characters that overlap to the next char cell.
     */
    എങ്കിൽ (bgcol == 0xffffffUL)
	SetBkMode(prt_dlg.hDC, TRANSPARENT);
    അല്ലെങ്കിൽ
	SetBkMode(prt_dlg.hDC, OPAQUE);
}

    ശൂന്യം
mch_print_set_fg(long_u fgcol)
{
    SetTextColor(prt_dlg.hDC, GetNearestColor(prt_dlg.hDC,
						   swap_me((COLORREF)fgcol)));
}

#endif // FEAT_PRINTER && !FEAT_POSTSCRIPT



#എങ്കിൽ defined(FEAT_SHORTCUT)
# include <shlobj.h>

# define is_path_sep(c)	    ((c) == L'\\' || (c) == L'/')

    സ്ഥിതം പൂർണ്ണസംഖ്യ
is_reparse_point_included(LPCWSTR fname)
{
    LPCWSTR	p = fname, q;
    WCHAR	buf[MAX_PATH];
    DWORD	attr;

    എങ്കിൽ (SAFE_isalpha(p[0]) && p[1] == L':' && is_path_sep(p[2]))
	p += 3;
    അല്ലെങ്കിൽ എങ്കിൽ (is_path_sep(p[0]) && is_path_sep(p[1]))
	p += 2;

    ആകുമ്പോൾ (*p != L'\0')
    {
	q = wcspbrk(p, L"\\/");
	എങ്കിൽ (q == ശൂന്യ)
	    p = q = fname + wcslen(fname);
	അല്ലെങ്കിൽ
	    p = q + 1;
	എങ്കിൽ (q - fname >= MAX_PATH)
	    തിരികെ FALSE;
	wcsncpy(buf, fname, q - fname);
	buf[q - fname] = L'\0';
	attr = GetFileAttributesW(buf);
	എങ്കിൽ (attr != INVALID_FILE_ATTRIBUTES
		&& (attr & FILE_ATTRIBUTE_REPARSE_POINT) != 0)
	    തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

/*
 * Return the resolved file path, NULL if "fname" is an AppExecLink reparse
 * point, already fully resolved, or it doesn't exists.
 */
    char_u *
resolve_reparse_point(char_u *fname)
{
    HANDLE	    h = INVALID_HANDLE_മൂല്യം;
    DWORD	    size;
    WCHAR	    *p, *wp;
    char_u	    *rfname = ശൂന്യ;
    WCHAR	    *buff = ശൂന്യ;

    p = enc_to_utf16(fname, ശൂന്യ);
    എങ്കിൽ (p == ശൂന്യ)
	പോകുക fail;

    എങ്കിൽ (!is_reparse_point_included(p))
    {
	vim_free(p);
	പോകുക fail;
    }

    h = CreateFileW(p, 0, 0, ശൂന്യ, OPEN_EXISTING,
	    FILE_FLAG_BACKUP_SEMANTICS, ശൂന്യ);
    vim_free(p);

    എങ്കിൽ (h == INVALID_HANDLE_മൂല്യം)
	പോകുക fail;

    size = GetFinalPathNameByHandleW(h, ശൂന്യ, 0, 0);
    എങ്കിൽ (size == 0)
	പോകുക fail;
    buff = ALLOC_MULT(WCHAR, size);
    എങ്കിൽ (buff == ശൂന്യ)
	പോകുക fail;
    എങ്കിൽ (GetFinalPathNameByHandleW(h, buff, size, 0) == 0)
	പോകുക fail;

    എങ്കിൽ (wcsncmp(buff, L"\\\\?\\UNC\\", 8) == 0)
    {
	buff[6] = L'\\';
	wp = buff + 6;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (wcsncmp(buff, L"\\\\?\\", 4) == 0)
	wp = buff + 4;
    അല്ലെങ്കിൽ
	wp = buff;

    rfname = utf16_to_enc(wp, ശൂന്യ);

fail:
    എങ്കിൽ (h != INVALID_HANDLE_മൂല്യം)
	CloseHandle(h);
    എങ്കിൽ (buff != ശൂന്യ)
	vim_free(buff);

    തിരികെ rfname;
}

/*
 * When "fname" is the name of a shortcut (*.lnk) resolve the file it points
 * to and return that name in allocated memory.
 * Otherwise NULL is returned.
 */
    സ്ഥിതം char_u *
resolve_shortcut(char_u *fname)
{
    HRESULT		hr;
    IShellLink		*psl = ശൂന്യ;
    IPersistFile	*ppf = ശൂന്യ;
    OLECHAR		wsz[MAX_PATH];
    char_u		*rfname = ശൂന്യ;
    പൂർണ്ണസംഖ്യ			len;
    IShellLinkW		*pslw = ശൂന്യ;
    WIN32_FIND_DATAW	ffdw; // we get those free of charge

    // Check if the file name ends in ".lnk". Avoid calling
    // CoCreateInstance(), it's quite slow.
    എങ്കിൽ (fname == ശൂന്യ)
	തിരികെ rfname;
    len = (പൂർണ്ണസംഖ്യ)STRLEN(fname);
    എങ്കിൽ (len <= 4 || STRNICMP(fname + len - 4, ".lnk", 4) != 0)
	തിരികെ rfname;

    CoInitialize(ശൂന്യ);

    // create a link manager object and request its interface
    hr = CoCreateInstance(
	    &CLSID_ShellLink, ശൂന്യ, CLSCTX_INPROC_SERVER,
	    &IID_IShellLinkW, (ശൂന്യം**)&pslw);
    എങ്കിൽ (hr == S_OK)
    {
	WCHAR	*p = enc_to_utf16(fname, ശൂന്യ);

	എങ്കിൽ (p != ശൂന്യ)
	{
	    // Get a pointer to the IPersistFile interface.
	    hr = pslw->lpVtbl->QueryInterface(
		    pslw, &IID_IPersistFile, (ശൂന്യം**)&ppf);
	    എങ്കിൽ (hr != S_OK)
		പോകുക shortcut_errorw;

	    // "load" the name and resolve the link
	    hr = ppf->lpVtbl->Load(ppf, p, STGM_READ);
	    എങ്കിൽ (hr != S_OK)
		പോകുക shortcut_errorw;
# എങ്കിൽ 0  // This makes Vim wait a long time if the target does not exist.
	    hr = pslw->lpVtbl->Resolve(pslw, ശൂന്യ, SLR_NO_UI);
	    എങ്കിൽ (hr != S_OK)
		പോകുക shortcut_errorw;
# endif

	    // Get the path to the link target.
	    ZeroMemory(wsz, MAX_PATH * വലുപ്പം(WCHAR));
	    hr = pslw->lpVtbl->GetPath(pslw, wsz, MAX_PATH, &ffdw, 0);
	    എങ്കിൽ (hr == S_OK && wsz[0] != NUL)
		rfname = utf16_to_enc(wsz, ശൂന്യ);

shortcut_errorw:
	    vim_free(p);
	}
    }

    // Release all interface pointers (both belong to the same object)
    എങ്കിൽ (ppf != ശൂന്യ)
	ppf->lpVtbl->Release(ppf);
    എങ്കിൽ (psl != ശൂന്യ)
	psl->lpVtbl->Release(psl);
    എങ്കിൽ (pslw != ശൂന്യ)
	pslw->lpVtbl->Release(pslw);

    CoUninitialize();
    തിരികെ rfname;
}

    char_u *
mch_resolve_path(char_u *fname, പൂർണ്ണസംഖ്യ reparse_point)
{
    char_u  *path = resolve_shortcut(fname);

    എങ്കിൽ (path == ശൂന്യ && reparse_point)
	path = resolve_reparse_point(fname);
    തിരികെ path;
}
#endif

#എങ്കിൽ defined(FEAT_EVAL) && (!defined(FEAT_GUI) || defined(VIMDLL))
/*
 * Bring ourselves to the foreground.  Does work if the OS doesn't allow it.
 */
    ശൂന്യം
win32_set_foreground(ശൂന്യം)
{
    GetConsoleHwnd();	    // get value of s_hwnd
    എങ്കിൽ (s_hwnd != 0)
	SetForegroundWindow(s_hwnd);
}
#endif

#എങ്കിൽ defined(FEAT_CLIENTSERVER)
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
# define VIM_CLASSNAME_LEN  (വലുപ്പം(VIM_CLASSNAME) - 1)

// Timeout for sending a message to another Vim instance.  Normally this works
// instantly, but it may hang when the other Vim instance is halted.
# define SENDMESSAGE_TIMEOUT	(5 * 1000)

// Communication is via WM_COPYDATA messages. The message type is sent in
// the dwData parameter. Types are defined here.
# define COPYDATA_താക്കോലുകൾ		0
# define COPYDATA_REPLY		1
# define COPYDATA_വ്യഞ്ജകം		10
# define COPYDATA_RESULT	11
# define COPYDATA_ERROR_RESULT	12
# define COPYDATA_ENCODING	20

// This is a structure containing a server HWND and its name.
ഘടന server_id
{
    HWND hwnd;
    char_u *name;
};

// Last received 'encoding' that the client uses.
സ്ഥിതം char_u	*client_enc = ശൂന്യ;

/*
 * Tell the other side what encoding we are using.
 * Return -1 if timeout happens.  Other errors are ignored.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
serverSendEnc(HWND target)
{
    COPYDATASTRUCT data;

    data.dwData = COPYDATA_ENCODING;
    data.cbData = (DWORD)STRLEN(p_enc) + 1;
    data.lpData = p_enc;
    എങ്കിൽ (SendMessageTimeout(target, WM_COPYDATA,
	    (WPARAM)message_window, (LPARAM)&data,
	    SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, ശൂന്യ) == 0)
	തിരികെ -1;
    തിരികെ 0;
}

/*
 * Clean up on exit. This destroys the hidden message window.
 */
    സ്ഥിതം ശൂന്യം
CleanUpMessaging(ശൂന്യം)
{
    എങ്കിൽ (message_window == 0)
	തിരികെ;

    DestroyWindow(message_window);
    message_window = 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ save_reply(HWND server, char_u *reply, പൂർണ്ണസംഖ്യ വ്യഞ്ജകം);

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
    സ്ഥിതം LRESULT CALLBACK
Messaging_WndProc(HWND hwnd, UINT സന്ദേശം, WPARAM wParam, LPARAM lParam)
{
    എങ്കിൽ (സന്ദേശം == WM_COPYDATA)
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
	പൂർണ്ണസംഖ്യ		retval;
	DWORD_PTR	dwret = 0;
	char_u		*str;
	char_u		*tofree;

	തിരഞ്ഞെടുപ്പ് (data->dwData)
	{
	അവസ്ഥ COPYDATA_ENCODING:
	    // Remember the encoding that the client uses.
	    vim_free(client_enc);
	    client_enc = enc_canonize((char_u *)data->lpData);
	    തിരികെ 1;

	അവസ്ഥ COPYDATA_താക്കോലുകൾ:
	    // Remember who sent this, for <client>
	    clientWindow = sender;

	    // Add the received keys to the input buffer.
	    str = serverConvert(client_enc, (char_u *)data->lpData, &tofree);
# ifdef FEAT_GUI
	    എങ്കിൽ (gui.in_use)
		// The GUI may get here re-entrantly, so insert the keys later.
		server_add_നിവേശം(str);
	    അല്ലെങ്കിൽ
# endif
		server_to_നിവേശം_buf(str);
	    vim_free(tofree);

# ifdef FEAT_GUI
	    // Wake up the main GUI loop.
#  ifdef VIMDLL
	    എങ്കിൽ (gui.in_use)
#  endif
		എങ്കിൽ (s_hwnd != 0)
		    PostMessage(s_hwnd, WM_NULL, 0, 0);
# endif
	    തിരികെ 1;

	അവസ്ഥ COPYDATA_വ്യഞ്ജകം:
	    // Remember who sent this, for <client>
	    clientWindow = sender;

	    str = serverConvert(client_enc, (char_u *)data->lpData, &tofree);
	    res = eval_client_വ്യഞ്ജകം_to_string(str);

	    എങ്കിൽ (res == ശൂന്യ)
	    {
		അക്ഷരം	*err = _(e_invalid_expression_received);
		size_t	len = STRLEN(str) + STRLEN(err) + 5;

		res = alloc(len);
		എങ്കിൽ (res != ശൂന്യ)
		    reply.cbData = (DWORD)vim_snprintf_safelen(
			(അക്ഷരം *)res, len, "%s: \"%s\"", err, str);
		അല്ലെങ്കിൽ
		    reply.cbData = 0;
		reply.dwData = COPYDATA_ERROR_RESULT;
	    }
	    അല്ലെങ്കിൽ
	    {
		reply.cbData = (DWORD)STRLEN(res) + 1;
		reply.dwData = COPYDATA_RESULT;
	    }
	    reply.lpData = res;

	    എങ്കിൽ (serverSendEnc(sender) < 0)
		retval = -1;
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (SendMessageTimeout(sender, WM_COPYDATA,
			(WPARAM)message_window, (LPARAM)&reply,
			SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
		    retval = -1;
		അല്ലെങ്കിൽ
		    retval = (പൂർണ്ണസംഖ്യ)dwret;
	    }
	    vim_free(tofree);
	    vim_free(res);
	    തിരികെ retval;

	അവസ്ഥ COPYDATA_REPLY:
	അവസ്ഥ COPYDATA_RESULT:
	അവസ്ഥ COPYDATA_ERROR_RESULT:
	    എങ്കിൽ (data->lpData != ശൂന്യ)
	    {
		str = serverConvert(client_enc, (char_u *)data->lpData,
								     &tofree);
		എങ്കിൽ (tofree == ശൂന്യ)
		    str = vim_strsave(str);
		എങ്കിൽ (save_reply(sender, str,
			   (data->dwData == COPYDATA_REPLY ?  0 :
			   (data->dwData == COPYDATA_RESULT ? 1 :
							      2))) == FAIL)
		    vim_free(str);
		അല്ലെങ്കിൽ എങ്കിൽ (data->dwData == COPYDATA_REPLY)
		{
		    char_u	winstr[30];

		    sprintf((അക്ഷരം *)winstr, PRINTF_HEX_LONG_U, (long_u)sender);
		    apply_autocmds(EVENT_REMOTEREPLY, winstr, str,
								TRUE, curbuf);
		}
	    }
	    തിരികെ 1;
	}

	തിരികെ 0;
    }

    അല്ലെങ്കിൽ എങ്കിൽ (സന്ദേശം == WM_ACTIVATE && wParam == WA_ACTIVE)
    {
	// When the message window is activated (brought to the foreground),
	// this actually applies to the text window.
# എങ്കിൽ !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	എങ്കിൽ (!gui.in_use)
#  endif
	    GetConsoleHwnd();	    // get value of s_hwnd
# endif
	എങ്കിൽ (s_hwnd != 0)
	{
	    SetForegroundWindow(s_hwnd);
	    തിരികെ 0;
	}
    }

    തിരികെ DefWindowProc(hwnd, സന്ദേശം, wParam, lParam);
}

/*
 * Initialise the message handling process.  This involves creating a window
 * to handle messages - the window will not be visible.
 */
    ശൂന്യം
serverInitMessaging(ശൂന്യം)
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
    wndclass.hIcon = ശൂന്യ;
    wndclass.hCursor = ശൂന്യ;
    wndclass.hbrBackground = ശൂന്യ;
    wndclass.lpszMenuName = ശൂന്യ;
    wndclass.lpszClassName = VIM_CLASSNAME;
    RegisterClass(&wndclass);

    // Create the message window. It will be hidden, so the details don't
    // matter.  Don't use WS_OVERLAPPEDWINDOW, it will make a shortcut remove
    // focus from gvim.
    message_window = CreateWindow(VIM_CLASSNAME, "",
			 WS_POPUPWINDOW | WS_CAPTION,
			 CW_USEDEFAULT, CW_USEDEFAULT,
			 100, 100, ശൂന്യ, ശൂന്യ,
			 g_hinst, ശൂന്യ);
}

// Used by serverSendToVim() to find an alternate server name.
സ്ഥിതം char_u *altname_buf_ptr = ശൂന്യ;

/*
 * Get the title of the window "hwnd", which is the Vim server name, in
 * "name[namelen]" and return the length.
 * Returns zero if window "hwnd" is not a Vim server.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
getVimServerName(HWND hwnd, അക്ഷരം *name, പൂർണ്ണസംഖ്യ namelen)
{
    പൂർണ്ണസംഖ്യ		len;
    അക്ഷരം	buffer[VIM_CLASSNAME_LEN + 1];

    // Ignore windows which aren't Vim message windows
    len = GetClassName(hwnd, buffer, വലുപ്പം(buffer));
    എങ്കിൽ (len != VIM_CLASSNAME_LEN || STRCMP(buffer, VIM_CLASSNAME) != 0)
	തിരികെ 0;

    // Get the title of the window
    തിരികെ GetWindowText(hwnd, name, namelen);
}

    സ്ഥിതം BOOL CALLBACK
enumWindowsGetServer(HWND hwnd, LPARAM lparam)
{
    ഘടന	server_id *id = (ഘടന server_id *)lparam;
    അക്ഷരം	server[MAX_PATH];

    // Get the title of the window
    എങ്കിൽ (getVimServerName(hwnd, server, വലുപ്പം(server)) == 0)
	തിരികെ TRUE;

    // If this is the server we're looking for, return its HWND
    എങ്കിൽ (STRICMP(server, id->name) == 0)
    {
	id->hwnd = hwnd;
	തിരികെ FALSE;
    }

    // If we are looking for an alternate server, remember this name.
    എങ്കിൽ (altname_buf_ptr != ശൂന്യ)
    {
	size_t	namelen = STRLEN(id->name);

	എങ്കിൽ (STRNICMP(server, id->name, namelen) == 0
	    && vim_isdigit(server[namelen]))
	{
	    STRCPY(altname_buf_ptr, server);
	    altname_buf_ptr = ശൂന്യ;	    // don't use another name
	}
    }

    // Otherwise, keep looking
    തിരികെ TRUE;
}

    സ്ഥിതം BOOL CALLBACK
enumWindowsGetNames(HWND hwnd, LPARAM lparam)
{
    list_T	*list = (list_T *)lparam;
    അക്ഷരം	server[MAX_PATH];

    // Get the title of the window
    എങ്കിൽ (getVimServerName(hwnd, server, വലുപ്പം(server)) == 0)
	തിരികെ TRUE;

    list_append_string(list, (char_u *)server, -1);
    തിരികെ TRUE;
}

ഘടന enum_windows_s
{
    WNDENUMPROC lpEnumFunc;
    LPARAM      lParam;
};

    സ്ഥിതം BOOL CALLBACK
enum_windows_child(HWND hwnd, LPARAM lParam)
{
    ഘടന enum_windows_s *ew = (ഘടന enum_windows_s *)lParam;

    തിരികെ (ew->lpEnumFunc)(hwnd, ew->lParam);
}

    സ്ഥിതം BOOL CALLBACK
enum_windows_toplevel(HWND hwnd, LPARAM lParam)
{
    ഘടന enum_windows_s *ew = (ഘടന enum_windows_s *)lParam;

    എങ്കിൽ ((ew->lpEnumFunc)(hwnd, ew->lParam))
	തിരികെ TRUE;
    തിരികെ EnumChildWindows(hwnd, enum_windows_child, lParam);
}

/*
 * Enumerate all windows including children.
 */
    സ്ഥിതം BOOL
enum_windows(WNDENUMPROC lpEnumFunc, LPARAM lParam)
{
    ഘടന enum_windows_s ew;

    ew.lpEnumFunc = lpEnumFunc;
    ew.lParam = lParam;
    തിരികെ EnumWindows(enum_windows_toplevel, (LPARAM)&ew);
}

    സ്ഥിതം HWND
findServer(char_u *name)
{
    ഘടന server_id id;

    id.name = name;
    id.hwnd = 0;

    enum_windows(enumWindowsGetServer, (LPARAM)(&id));

    തിരികെ id.hwnd;
}

    ശൂന്യം
serverSetName(char_u *name)
{
    size_t	namelen;
    char_u	*ok_name;
    HWND	hwnd = 0;
    പൂർണ്ണസംഖ്യ		i = 0;
    char_u	*p;

    // Leave enough space for a 9-digit suffix to ensure uniqueness!
    namelen = STRLEN(name);
    ok_name = alloc(namelen + 10);
    എങ്കിൽ (ok_name == ശൂന്യ)
	തിരികെ;

    STRCPY(ok_name, name);
    p = ok_name + namelen;

    വേണ്ടി (;;)
    {
	// This is inefficient - we're doing an EnumWindows loop for each
	// possible name. It would be better to grab all names in one go,
	// and scan the list each time...
	hwnd = findServer(ok_name);
	എങ്കിൽ (hwnd == 0)
	    നിർത്തുക;

	++i;
	എങ്കിൽ (i >= 1000)
	    നിർത്തുക;

	sprintf((അക്ഷരം *)p, "%d", i);
    }

    എങ്കിൽ (hwnd != 0)
	vim_free(ok_name);
    അല്ലെങ്കിൽ
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
serverGetVimNames(ശൂന്യം)
{
    list_T *list = list_alloc();

    എങ്കിൽ (list == ശൂന്യ)
	തിരികെ ശൂന്യ;

    enum_windows(enumWindowsGetNames, (LPARAM)list);

    തിരികെ list;
}

    പൂർണ്ണസംഖ്യ
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
    sscanf((അക്ഷരം *)name, SCANF_HEX_LONG_U, &n);
    എങ്കിൽ (n == 0)
	തിരികെ -1;

    target = (HWND)n;
    എങ്കിൽ (!IsWindow(target))
	തിരികെ -1;

    data.dwData = COPYDATA_REPLY;
    data.cbData = (DWORD)STRLEN(reply) + 1;
    data.lpData = reply;

    എങ്കിൽ (serverSendEnc(target) < 0)
	തിരികെ -1;
    എങ്കിൽ (SendMessageTimeout(target, WM_COPYDATA,
		(WPARAM)message_window, (LPARAM)&data,
		SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
	തിരികെ -1;
    തിരികെ dwret ? 0 : -1;
}

    പൂർണ്ണസംഖ്യ
serverSendToVim(
    char_u	 *name,			// Where to send.
    char_u	 *cmd,			// What to send.
    char_u	 **result,		// Result of eval'ed expression
    ശൂന്യം	 *ptarget,		// HWND of server
    പൂർണ്ണസംഖ്യ		 as_വ്യഞ്ജകം,		// Expression or keys?
    പൂർണ്ണസംഖ്യ		 timeout,		// timeout in seconds or zero
    പൂർണ്ണസംഖ്യ		 silent)		// don't complain about no server
{
    size_t	namelen;
    HWND	target;
    COPYDATASTRUCT data;
    char_u	*retval = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		retcode = 0;
    DWORD_PTR	dwret = 0;
    char_u	altname_buf[MAX_PATH];

    // Execute locally if no display or target is ourselves
    എങ്കിൽ (serverName != ശൂന്യ && STRICMP(name, serverName) == 0)
	തിരികെ sendToLocalVim(cmd, as_വ്യഞ്ജകം, result);

    // If the server name does not end in a digit then we look for an
    // alternate name.  e.g. when "name" is GVIM then we may find GVIM2.
    namelen = STRLEN(name);
    എങ്കിൽ (namelen > 1 && !vim_isdigit(name[namelen - 1]))
	altname_buf_ptr = altname_buf;
    altname_buf[0] = NUL;
    target = findServer(name);
    altname_buf_ptr = ശൂന്യ;
    എങ്കിൽ (target == 0 && altname_buf[0] != NUL)
	// Use another server name we found.
	target = findServer(altname_buf);

    എങ്കിൽ (target == 0)
    {
	എങ്കിൽ (!silent)
	    semsg(_(e_no_registered_server_named_str), name);
	തിരികെ -1;
    }

    എങ്കിൽ (ptarget)
	*(HWND *)ptarget = target;

    data.dwData = as_വ്യഞ്ജകം ? COPYDATA_വ്യഞ്ജകം : COPYDATA_താക്കോലുകൾ;
    data.cbData = (DWORD)STRLEN(cmd) + 1;
    data.lpData = cmd;

    എങ്കിൽ (serverSendEnc(target) < 0)
	തിരികെ -1;
    എങ്കിൽ (SendMessageTimeout(target, WM_COPYDATA,
		(WPARAM)message_window, (LPARAM)&data,
		SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
	തിരികെ -1;
    എങ്കിൽ (dwret == 0)
	തിരികെ -1;

    എങ്കിൽ (as_വ്യഞ്ജകം)
	retval = serverGetReply(target, &retcode, TRUE, TRUE, timeout);

    എങ്കിൽ (result == ശൂന്യ)
	vim_free(retval);
    അല്ലെങ്കിൽ
	*result = retval; // Caller assumes responsibility for freeing

    തിരികെ retcode;
}

/*
 * Bring the server to the foreground.
 */
    ശൂന്യം
serverForeground(char_u *name)
{
    HWND	target = findServer(name);

    എങ്കിൽ (target != 0)
	SetForegroundWindow(target);
}

// Replies from server need to be stored until the client picks them up via
// remote_read(). So we maintain a list of server-id/reply pairs.
// Note that there could be multiple replies from one server pending if the
// client is slow picking them up.
// We just store the replies in a simple list. When we remove an entry, we
// move list entries down to fill the gap.
// The server ID is simply the HWND.
തരനിർവചനം ഘടന
{
    HWND	server;		// server window
    char_u	*reply;		// reply string
    പൂർണ്ണസംഖ്യ		വ്യഞ്ജകം_result;	// 0 for REPLY, 1 for RESULT 2 for error
} reply_T;

സ്ഥിതം garray_T reply_list = {0, 0, വലുപ്പം(reply_T), 5, 0};

# define REPLY_ITEM(i) ((reply_T *)(reply_list.ga_data) + (i))
# define REPLY_എണ്ണം (reply_list.ga_len)

// Flag which is used to wait for a reply
സ്ഥിതം പൂർണ്ണസംഖ്യ reply_received = 0;

/*
 * Store a reply.  "reply" must be allocated memory (or NULL).
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
save_reply(HWND server, char_u *reply, പൂർണ്ണസംഖ്യ വ്യഞ്ജകം)
{
    reply_T *rep;

    എങ്കിൽ (ga_grow(&reply_list, 1) == FAIL)
	തിരികെ FAIL;

    rep = REPLY_ITEM(REPLY_എണ്ണം);
    rep->server = server;
    rep->reply = reply;
    rep->വ്യഞ്ജകം_result = വ്യഞ്ജകം;
    എങ്കിൽ (rep->reply == ശൂന്യ)
	തിരികെ FAIL;

    ++REPLY_എണ്ണം;
    reply_received = 1;
    തിരികെ OK;
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
serverGetReply(HWND server, പൂർണ്ണസംഖ്യ *വ്യഞ്ജകം_res, പൂർണ്ണസംഖ്യ remove, പൂർണ്ണസംഖ്യ wait, പൂർണ്ണസംഖ്യ timeout)
{
    പൂർണ്ണസംഖ്യ		i;
    char_u	*reply;
    reply_T	*rep;
    പൂർണ്ണസംഖ്യ		did_process = FALSE;
    time_t	start;
    time_t	now;

    // When waiting, loop until the message waiting for is received.
    time(&start);
    വേണ്ടി (;;)
    {
	// Reset this here, in case a message arrives while we are going
	// through the already received messages.
	reply_received = 0;

	വേണ്ടി (i = 0; i < REPLY_എണ്ണം; ++i)
	{
	    rep = REPLY_ITEM(i);
	    എങ്കിൽ (rep->server == server
		    && ((rep->വ്യഞ്ജകം_result != 0) == (വ്യഞ്ജകം_res != ശൂന്യ)))
	    {
		// Save the values we've found for later
		reply = rep->reply;
		എങ്കിൽ (വ്യഞ്ജകം_res != ശൂന്യ)
		    *വ്യഞ്ജകം_res = rep->വ്യഞ്ജകം_result == 1 ? 0 : -1;

		എങ്കിൽ (remove)
		{
		    // Move the rest of the list down to fill the gap
		    mch_memmove(rep, rep + 1,
				     (REPLY_എണ്ണം - i - 1) * വലുപ്പം(reply_T));
		    --REPLY_എണ്ണം;
		}

		// Return the reply to the caller, who takes on responsibility
		// for freeing it if "remove" is TRUE.
		തിരികെ reply;
	    }
	}

	// If we got here, we didn't find a reply. Return immediately if the
	// "wait" parameter isn't set.
	എങ്കിൽ (!wait)
	{
	    // Process pending messages once. Without this, looping on
	    // remote_peek() would never get the reply.
	    എങ്കിൽ (!did_process)
	    {
		did_process = TRUE;
		serverProcessPendingMessages();
		തുടരുക;
	    }
	    നിർത്തുക;
	}

	// We need to wait for a reply. Enter a message loop until the
	// "reply_received" flag gets set.

	// Loop until we receive a reply
	ആകുമ്പോൾ (reply_received == 0)
	{
# ifdef FEAT_TIMERS
	    // TODO: use the return value to decide how long to wait.
	    check_due_timer();
# endif
	    time(&now);
	    എങ്കിൽ (timeout > 0 && (now - start) >= timeout)
		നിർത്തുക;

	    // Wait for a SendMessage() call to us.  This could be the reply
	    // we are waiting for.  Use a timeout of a second, to catch the
	    // situation that the server died unexpectedly.
	    സന്ദേശം_Wait_For_Multiple_Objects(0, ശൂന്യ, TRUE, 1000, QS_ALLINPUT);

	    // If the server has died, give up
	    എങ്കിൽ (!IsWindow(server))
		തിരികെ ശൂന്യ;

	    serverProcessPendingMessages();
	}
    }

    തിരികെ ശൂന്യ;
}

/*
 * Process any messages in the Windows message queue.
 */
    ശൂന്യം
serverProcessPendingMessages(ശൂന്യം)
{
    സന്ദേശം_2 സന്ദേശം;

    ആകുമ്പോൾ (PeekMessageW(&സന്ദേശം, ശൂന്യ, 0, 0, PM_REMOVE))
    {
	TranslateMessage(&സന്ദേശം);
	DispatchMessageW(&സന്ദേശം);
    }
}

#endif // FEAT_CLIENTSERVER

#എങ്കിൽ defined(FEAT_GUI) || (defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT))

ഘടന charset_pair
{
    string_T	name;
    BYTE	charset;
};

സ്ഥിതം ഘടന charset_pair
charset_pairs[] =
{
    {STR_LITERAL_INIT("ANSI"),		ANSI_CHARSET},
    {STR_LITERAL_INIT("CHINESEBIG5"),	CHINESEBIG5_CHARSET},
    {STR_LITERAL_INIT("DEFAULT"),	DEFAULT_CHARSET},
    {STR_LITERAL_INIT("HANGEUL"),	HANGEUL_CHARSET},
    {STR_LITERAL_INIT("OEM"),		OEM_CHARSET},
    {STR_LITERAL_INIT("SHIFTJIS"),	SHIFTJIS_CHARSET},
    {STR_LITERAL_INIT("SYMBOL"),	ചിഹ്നം_CHARSET},
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

ഘടന quality_pair
{
    string_T	name;
    DWORD	quality;
};

സ്ഥിതം ഘടന quality_pair
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
    അക്ഷരം *
charset_id2name(പൂർണ്ണസംഖ്യ id)
{
    പൂർണ്ണസംഖ്യ	i;

    വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(charset_pairs); ++i)
    {
	എങ്കിൽ ((BYTE)id == charset_pairs[i].charset)
	    തിരികെ (അക്ഷരം *)charset_pairs[i].name.string;
    }

    തിരികെ ശൂന്യ;
}

/*
 * Convert a quality ID to a name.
 * Return NULL when not recognized.
 */
    അക്ഷരം *
quality_id2name(DWORD id)
{
    പൂർണ്ണസംഖ്യ	i;

    വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(quality_pairs); ++i)
    {
	എങ്കിൽ (id == quality_pairs[i].quality)
	    തിരികെ (അക്ഷരം *)quality_pairs[i].name.string;
    }

    തിരികെ ശൂന്യ;
}

// The default font height in 100% scaling (96dpi).
// (-16 in 96dpi equates to roughly 12pt)
# define DEFAULT_FONT_HEIGHT	(-16)

സ്ഥിതം സ്ഥിരം LOGFONTW s_lfDefault =
{
    DEFAULT_FONT_HEIGHT,
    0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
    PROOF_QUALITY, FIXED_PITCH | FF_DONTCARE,
    L""	// Default font name will be set later based on current language.
};

// This will be initialized when set_default_logfont() is called first time.
// The value will be based on the system DPI.
പൂർണ്ണസംഖ്യ current_font_height = 0;		// also used in gui_w32.c

/*
 * Convert a string representing a point size into pixels. The string should
 * be a positive decimal number, with an optional decimal point (eg, "12", or
 * "10.5"). The pixel value is returned, and a pointer to the next unconverted
 * character is stored in *end. The flag "vertical" says whether this
 * calculation is for a vertical (height) size or a horizontal (width) one.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
points_to_pixels(WCHAR *str, WCHAR **end, പൂർണ്ണസംഖ്യ vertical, long_i pprinter_dc)
{
    പൂർണ്ണസംഖ്യ		pixels;
    പൂർണ്ണസംഖ്യ		points = 0;
    പൂർണ്ണസംഖ്യ		ഹാരകം = 0;
    HWND	hwnd = (HWND)0;
    HDC		hdc;
    HDC		printer_dc = (HDC)pprinter_dc;

    ആകുമ്പോൾ (*str != NUL)
    {
	എങ്കിൽ (*str == L'.' && ഹാരകം == 0)
	{
	    // Start keeping a divisor, for later
	    ഹാരകം = 1;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (!VIM_ISDIGIT(*str))
		നിർത്തുക;

	    points *= 10;
	    points += *str - L'0';
	    ഹാരകം *= 10;
	}
	++str;
    }

    എങ്കിൽ (ഹാരകം == 0)
	ഹാരകം = 1;

    എങ്കിൽ (printer_dc == ശൂന്യ)
    {
	hwnd = GetDesktopWindow();
	hdc = GetWindowDC(hwnd);
    }
    അല്ലെങ്കിൽ
	hdc = printer_dc;

    pixels = MulDiv(points,
		    GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX),
		    72 * ഹാരകം);

    എങ്കിൽ (printer_dc == ശൂന്യ)
	ReleaseDC(hwnd, hdc);

    *end = str;
    തിരികെ pixels;
}

/*
 * Convert pixel into point size. This is a reverse of points_to_pixels.
 */
    സ്ഥിതം ഇരട്ട
pixels_to_points(പൂർണ്ണസംഖ്യ pixels, പൂർണ്ണസംഖ്യ vertical, long_i pprinter_dc)
{
    ഇരട്ട	points = 0;
    HWND	hwnd = (HWND)0;
    HDC		hdc;
    HDC		printer_dc = (HDC)pprinter_dc;

    എങ്കിൽ (printer_dc == ശൂന്യ)
    {
	hwnd = GetDesktopWindow();
	hdc = GetWindowDC(hwnd);
    }
    അല്ലെങ്കിൽ
	hdc = printer_dc;

    points = pixels * 72.0 / GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX);
    എങ്കിൽ (printer_dc == ശൂന്യ)
	ReleaseDC(hwnd, hdc);

    തിരികെ points;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ CALLBACK
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
    എങ്കിൽ ((ntm->tmPitchAndFamily & 1) != 0)
	തിരികെ 1;
# endif

    // Remember this LOGFONTW as a "possible"
    *lf = elf->elfLogFont;

    // Terminate the scan as soon as we find an ANSI font
    എങ്കിൽ (lf->lfCharSet == ANSI_CHARSET
	    || lf->lfCharSet == OEM_CHARSET
	    || lf->lfCharSet == DEFAULT_CHARSET)
	തിരികെ 0;

    // Continue the scan - we have a non-ANSI font
    തിരികെ 2;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
init_logfont(LOGFONTW *lf)
{
    പൂർണ്ണസംഖ്യ		n;
    HWND	hwnd = GetDesktopWindow();
    HDC		hdc = GetWindowDC(hwnd);

    n = EnumFontFamiliesW(hdc,
			 lf->lfFaceName,
			 (FONTENUMPROCW)font_enumproc,
			 (LPARAM)lf);

    ReleaseDC(hwnd, hdc);

    // If we couldn't find a usable font, return failure
    എങ്കിൽ (n == 1)
	തിരികെ FAIL;

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
    തിരികെ OK;
}

/*
 * Call back for EnumFontFamiliesW in expand_font_enumproc.
 *
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ CALLBACK
expand_font_enumproc(
    ENUMLOGFONTW    *elf,
    NEWTEXTMETRICW  *ntm UNUSED,
    DWORD	    type UNUSED,
    LPARAM	    lparam)
{
    LOGFONTW *lf = (LOGFONTW*)elf;

# ifndef FEAT_PROPORTIONAL_FONTS
    // Ignore non-monospace fonts without further ado
    എങ്കിൽ ((ntm->tmPitchAndFamily & 1) != 0)
	തിരികെ 1;
# endif

    // Filter only on ANSI. Otherwise will see a lot of random fonts that we
    // usually don't want.
    എങ്കിൽ (lf->lfCharSet != ANSI_CHARSET)
	തിരികെ 1;

    പൂർണ്ണസംഖ്യ (*add_match)(char_u *) = (പൂർണ്ണസംഖ്യ (*)(char_u *))lparam;

    WCHAR *faceNameW = lf->lfFaceName;
    char_u *faceName = utf16_to_enc(faceNameW, ശൂന്യ);
    എങ്കിൽ (!faceName)
	തിരികെ 0;

    add_match(faceName);
    vim_free(faceName);

    തിരികെ 1;
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
    ശൂന്യം
gui_mch_expand_font(
    optexpand_T	*args,
    ശൂന്യം	*param UNUSED,
    പൂർണ്ണസംഖ്യ		(*add_match)(char_u *val))
{
    expand_T	    *xp = args->oe_xp;
    എങ്കിൽ (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	അക്ഷരം buf[30];
	പൂർണ്ണസംഖ്യ	i;

	// Always fill in with the current font size as first option for
	// convenience. We simply round to the closest integer for simplicity.
	പൂർണ്ണസംഖ്യ font_height = (പൂർണ്ണസംഖ്യ)round(
		pixels_to_points(-current_font_height, TRUE, (long_i)ശൂന്യ));
	vim_snprintf(buf, ARRAY_LENGTH(buf), "h%d", font_height);
	add_match((char_u *)buf);

	// Note: Keep this in sync with get_logfont(). Don't include 'c' and
	// 'q' as we fill in all the values below.
	സ്ഥിതം അക്ഷരം *(p_gfn_win_opt_മൂല്യങ്ങൾ[]) = {
	    "h" , "w" , "W" , "b" , "i" , "u" , "s"};
	വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(p_gfn_win_opt_മൂല്യങ്ങൾ); i++)
	    add_match((char_u *)p_gfn_win_opt_മൂല്യങ്ങൾ[i]);

	വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(charset_pairs); ++i)
	{
	    vim_snprintf(buf, വലുപ്പം(buf), "c%s", charset_pairs[i].name.string);
	    add_match((char_u *)buf);
	}

	വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(quality_pairs); ++i)
	{
	    vim_snprintf(buf, വലുപ്പം(buf), "q%s", quality_pairs[i].name.string);
	    add_match((char_u *)buf);
	}
	തിരികെ;
    }

    HWND	hwnd = GetDesktopWindow();
    HDC		hdc = GetWindowDC(hwnd);

    EnumFontFamiliesW(hdc,
	    ശൂന്യ,
	    (FONTENUMPROCW)expand_font_enumproc,
	    (LPARAM)add_match);

    ReleaseDC(hwnd, hdc);
}

/*
 * Compare a UTF-16 string and an ASCII string literally.
 * Only works all the code points are inside ASCII range.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
utf16ascncmp(സ്ഥിരം WCHAR *w, സ്ഥിരം അക്ഷരം *p, size_t n)
{
    size_t i;

    വേണ്ടി (i = 0; i < n; i++)
    {
	എങ്കിൽ (w[i] == 0 || w[i] != p[i])
	    തിരികെ w[i] - p[i];
    }
    തിരികെ 0;
}

/*
 * Equivalent of GetDpiForSystem().
 */
    UINT WINAPI
vimGetDpiForSystem(ശൂന്യം)
{
    HWND hwnd = GetDesktopWindow();
    HDC hdc = GetWindowDC(hwnd);
    UINT dpi = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(hwnd, hdc);
    തിരികെ dpi;
}

/*
 * Set default logfont based on current language.
 */
    സ്ഥിതം ശൂന്യം
set_default_logfont(LOGFONTW *lf)
{
    // Default font name for current language on MS-Windows.
    // If not translated, falls back to "Consolas".
    // This must be a fixed-pitch font.
    സ്ഥിരം അക്ഷരം *defaultfontname = N_("DefaultFontNameForWindows");
    അക്ഷരം *fontname = _(defaultfontname);

    എങ്കിൽ (STRCMP(fontname, defaultfontname) == 0)
	fontname = "Consolas";

    *lf = s_lfDefault;
    lf->lfHeight = DEFAULT_FONT_HEIGHT * (പൂർണ്ണസംഖ്യ)vimGetDpiForSystem() / 96;
    എങ്കിൽ (current_font_height == 0)
	current_font_height = lf->lfHeight;

    WCHAR *wfontname = enc_to_utf16((char_u*)fontname, ശൂന്യ);
    എങ്കിൽ (wfontname != ശൂന്യ)
    {
	wcscpy_s(lf->lfFaceName, LF_FACESIZE, wfontname);
	vim_free(wfontname);
    }
}

/*
 * Get font info from "name" into logfont "lf".
 * Return OK for a valid name, FAIL otherwise.
 */
    പൂർണ്ണസംഖ്യ
get_logfont(
    LOGFONTW	*lf,
    char_u	*name,
    HDC		printer_dc,
    പൂർണ്ണസംഖ്യ		verbose)
{
    WCHAR	*p;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		ret = FAIL;
    സ്ഥിതം LOGFONTW *lastlf = ശൂന്യ;
    WCHAR	*wname;

    set_default_logfont(lf);
    എങ്കിൽ (name == ശൂന്യ)
	തിരികെ OK;

    wname = enc_to_utf16(name, ശൂന്യ);
    എങ്കിൽ (wname == ശൂന്യ)
	തിരികെ FAIL;

    എങ്കിൽ (wcscmp(wname, L"*") == 0)
    {
# എങ്കിൽ defined(FEAT_GUI_MSWIN)
	CHOOSEFONTW	cf;
	// if name is "*", bring up std font dialog:
	CLEAR_FIELD(cf);
	cf.lStructSize = വലുപ്പം(cf);
	cf.hwndOwner = s_hwnd;
	cf.Flags = CF_SCREENFONTS | CF_FIXEDPITCHONLY | CF_INITTOLOGFONTSTRUCT;
	എങ്കിൽ (lastlf != ശൂന്യ)
	    *lf = *lastlf;
	cf.lpLogFont = lf;
	cf.nFontType = 0 ; //REGULAR_FONTTYPE;
	എങ്കിൽ (ChooseFontW(&cf))
	    ret = OK;
# endif
	പോകുക theend;
    }

    /*
     * Split name up, it could be <name>:h<height>:w<width> etc.
     */
    വേണ്ടി (p = wname; *p && *p != L':'; p++)
    {
	എങ്കിൽ (p - wname + 1 >= LF_FACESIZE)
	    പോകുക theend;			// Name too long
	lf->lfFaceName[p - wname] = *p;
    }
    എങ്കിൽ (p != wname)
	lf->lfFaceName[p - wname] = NUL;

    // First set defaults
    lf->lfHeight = DEFAULT_FONT_HEIGHT * (പൂർണ്ണസംഖ്യ)vimGetDpiForSystem() / 96;
    lf->lfWidth = 0;
    lf->lfWeight = FW_NORMAL;
    lf->lfItalic = FALSE;
    lf->lfUnderline = FALSE;
    lf->lfStrikeOut = FALSE;

    /*
     * If the font can't be found, try replacing '_' by ' '.
     */
    എങ്കിൽ (init_logfont(lf) == FAIL)
    {
	പൂർണ്ണസംഖ്യ	did_replace = FALSE;

	വേണ്ടി (i = 0; lf->lfFaceName[i]; ++i)
	    എങ്കിൽ (lf->lfFaceName[i] == L'_')
	    {
		lf->lfFaceName[i] = L' ';
		did_replace = TRUE;
	    }
	എങ്കിൽ (!did_replace || init_logfont(lf) == FAIL)
	    പോകുക theend;
    }

    ആകുമ്പോൾ (*p == L':')
	p++;

    // Set the values found after ':'
    ആകുമ്പോൾ (*p)
    {
	തിരഞ്ഞെടുപ്പ് (*p++)
	{
	    // Note: Keep this in sync with gui_mch_expand_font().
	    അവസ്ഥ L'h':
		lf->lfHeight = - points_to_pixels(p, &p, TRUE, (long_i)printer_dc);
		നിർത്തുക;
	    അവസ്ഥ L'w':
		lf->lfWidth = points_to_pixels(p, &p, FALSE, (long_i)printer_dc);
		നിർത്തുക;
	    അവസ്ഥ L'W':
		lf->lfWeight = wcstol(p, &p, 10);
		നിർത്തുക;
	    അവസ്ഥ L'b':
		lf->lfWeight = FW_BOLD;
		നിർത്തുക;
	    അവസ്ഥ L'i':
		lf->lfItalic = TRUE;
		നിർത്തുക;
	    അവസ്ഥ L'u':
		lf->lfUnderline = TRUE;
		നിർത്തുക;
	    അവസ്ഥ L's':
		lf->lfStrikeOut = TRUE;
		നിർത്തുക;
	    അവസ്ഥ L'c':
		വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(charset_pairs); ++i)
		{
		    എങ്കിൽ (utf16ascncmp(p, (അക്ഷരം *)charset_pairs[i].name.string,
			    charset_pairs[i].name.length) == 0)
		    {
			lf->lfCharSet = charset_pairs[i].charset;
			p += charset_pairs[i].name.length;
			നിർത്തുക;
		    }
		}

		എങ്കിൽ (i == (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(charset_pairs) && verbose)
		{
		    char_u *s = utf16_to_enc(p, ശൂന്യ);
		    എങ്കിൽ (s != ശൂന്യ)
		    {
			semsg(_(e_illegal_str_name_str_in_font_name_str),
							   "charset", s, name);
			vim_free(s);
		    }
		}
		നിർത്തുക;
	    അവസ്ഥ L'f':
		// Font features (e.g., "fss19=1").
		// Parsed separately by gui_mch_init_font(); skip here.
		ആകുമ്പോൾ (*p && *p != L':')
		    p++;
		നിർത്തുക;
	    അവസ്ഥ L'q':
		വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(quality_pairs); ++i)
		{
		    എങ്കിൽ (utf16ascncmp(p, (അക്ഷരം *)quality_pairs[i].name.string,
			    quality_pairs[i].name.length) == 0)
		    {
			lf->lfQuality = quality_pairs[i].quality;
			p += quality_pairs[i].name.length;
			നിർത്തുക;
		    }
		}

		എങ്കിൽ (i == (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(quality_pairs) && verbose)
		{
		    char_u *s = utf16_to_enc(p, ശൂന്യ);
		    എങ്കിൽ (s != ശൂന്യ)
		    {
			semsg(_(e_illegal_str_name_str_in_font_name_str),
							   "quality", s, name);
			vim_free(s);
		    }
		}
		നിർത്തുക;
	    സ്ഥിരസ്ഥിതി:
		എങ്കിൽ (verbose)
		    semsg(_(e_illegal_char_nr_in_font_name_str), p[-1], name);
		പോകുക theend;
	}
	ആകുമ്പോൾ (*p == L':')
	    p++;
    }
    ret = OK;

theend:
    // ron: init lastlf
    എങ്കിൽ (ret == OK && printer_dc == ശൂന്യ)
    {
	vim_free(lastlf);
	lastlf = ALLOC_ONE(LOGFONTW);
	എങ്കിൽ (lastlf != ശൂന്യ)
	    mch_memmove(lastlf, lf, വലുപ്പം(LOGFONTW));
    }
    vim_free(wname);

    തിരികെ ret;
}

#endif // defined(FEAT_GUI) || defined(FEAT_PRINTER)

#എങ്കിൽ defined(FEAT_JOB_CHANNEL)
/*
 * Initialize the Winsock dll.
 */
    ശൂന്യം
channel_init_winsock(ശൂന്യം)
{
    WSADATA wsaData;
    പൂർണ്ണസംഖ്യ wsaerr;

    എങ്കിൽ (WSInitialized)
	തിരികെ;

    wsaerr = WSAStartup(MAKEWORD(2, 2), &wsaData);
    എങ്കിൽ (wsaerr == 0)
	WSInitialized = TRUE;
}
#endif
