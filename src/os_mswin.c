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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <sys/types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <limits.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <process.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <direct.h>

#ਜੇ !defined(FEAT_GUI_MSWIN)
# include <shellapi.h>
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT)
# include <dlgs.h>
# include <winspool.h>
# include <commdlg.h>
#ਜੇ_ਅੰਤ

// Record all output and all keyboard & mouse input
// #define MCH_WRITE_DUMP

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MCH_WRITE_DUMP
FILE* fdDump = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

#ਜੇ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ਬਾਹਰੀ ਅੱਖਰ g_szOrigTitle[];
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
ਬਾਹਰੀ HWND s_hwnd;
#ਨਹੀਂਤਾਂ
ਸਥਿਰ HWND s_hwnd = 0;	    // console window handle, set by GetConsoleHwnd()
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
ਪੂਰਨ WSInitialized = FALSE; // WinSock is initialized
#ਜੇ_ਅੰਤ


/*
 * Save the instance handle of the exe/dll.
 */
    ਖਾਲੀ
SaveInst(HINSTANCE hInst)
{
    g_hinst = hInst;
}

#ਜੇ defined(FEAT_GUI_MSWIN)
/*
 * GUI version of mch_exit().
 * Shut down and exit with status `r'
 * Careful: mch_exit() may be called before mch_init()!
 */
    ਖਾਲੀ
mch_exit_g(ਪੂਰਨ r)
{
    exiting = TRUE;

    display_errors();

    ml_close_all(TRUE);		// remove all memfiles

# ifdef FEAT_OLE
    UninitOLE();
# endif
# ifdef FEAT_ਕੰਮ_CHANNEL
    ਜੇ (WSInitialized)
    {
	WSInitialized = FALSE;
	WSACleanup();
    }
# endif
# ifdef DYNAMIC_GETTEXT
    dyn_libintl_ਸਿਰਾ();
# endif

    ਜੇ (gui.ਵਿੱਚ_use)
	gui_exit(r);

# ifdef EXITFREE
    free_all_mem();
# endif

    ਬਾਹਰ_ਨਿਕਲੋ(r);
}

#ਜੇ_ਅੰਤ // FEAT_GUI_MSWIN


/*
 * Get version number including build number
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ BOOL (WINAPI *Pfn_Rtl_Get_ਸੰਸਕਰਣ)(LPOSVERSIONINFOW);
DWORD win_ਸੰਸਕਰਣ;
    ਸਥਿਰ ਖਾਲੀ
win_ਸੰਸਕਰਣ_init(ਖਾਲੀ)
{
    OSVERSIONINFOW	osver;
    HMODULE		hNtdll;
    Pfn_Rtl_Get_ਸੰਸਕਰਣ	p_Rtl_Get_ਸੰਸਕਰਣ;

    hNtdll = GetModuleHandle("ntdll.dll");
    ਜੇ (hNtdll == ਨਿਰਾ)
	ਵਾਪਸ;

    p_Rtl_Get_ਸੰਸਕਰਣ =
	(Pfn_Rtl_Get_ਸੰਸਕਰਣ) Get_Proc_ਪਤਾ(hNtdll, "RtlGetVersion");
    ਜੇ (p_Rtl_Get_ਸੰਸਕਰਣ == ਨਿਰਾ)
	ਵਾਪਸ;

    osver.dw_OSVersion_Info_ਮਾਪ = ਆਕਾਰ(OSVERSIONINFOW);
    p_Rtl_Get_ਸੰਸਕਰਣ(&osver);
    win_ਸੰਸਕਰਣ = MAKE_VER(min(osver.dw_Major_ਸੰਸਕਰਣ, 0xFF),
	    min(osver.dw_Minor_ਸੰਸਕਰਣ, 0xFF),
	    min(osver.dw_Build_ਸੰਖਿਆ, 0xFFFF));
}

/*
 * Init the tables for toupper() and tolower().
 */
    ਖਾਲੀ
mch_early_init(ਖਾਲੀ)
{
    ਪੂਰਨ		i;

    win_ਸੰਸਕਰਣ_init();

    PlatformId();

    // Init the tables for toupper() and tolower()
    ਲਈ (i = 0; i < 256; ++i)
	toupper_tab[i] = tolower_tab[i] = i;
    Char_ਉੱਤਲਾ_Buff((LPSTR)toupper_tab, 256);
    Char_ਹੇਠਲਾ_Buff((LPSTR)tolower_tab, 256);
}


/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    ਪੂਰਨ
mch_ਨਿਵੇਸ਼_isatty(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
# ifdef VIMDLL
    ਜੇ (gui.ਵਿੱਚ_use)
# endif
	ਵਾਪਸ TRUE;	    // GUI always has a tty
#ਜੇ_ਅੰਤ
#ਜੇ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    ਜੇ (isatty(read_cmd_fd))
	ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
#ਜੇ_ਅੰਤ
}

/*
 * mch_settitle(): set titlebar of our window
 */
    ਖਾਲੀ
mch_settitle(
    char_u *title,
    char_u *icon ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
# ifdef VIMDLL
    ਜੇ (gui.ਵਿੱਚ_use)
# endif
    {
	gui_mch_settitle(title, icon);
	ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ
#ਜੇ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    ਜੇ (title != ਨਿਰਾ)
    {
	WCHAR	*wp = enc_ਮੰਜ਼ਿਲ_utf16(title, ਨਿਰਾ);

	ਜੇ (wp == ਨਿਰਾ)
	    ਵਾਪਸ;

	ਸੈੱਟ_Console_Title_W(wp);
	vim_free(wp);
	ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ
}


/*
 * Restore the window/icon title.
 * which is one of:
 *  SAVE_RESTORE_TITLE: Just restore title
 *  SAVE_RESTORE_ICON:  Just restore icon (which we don't have)
 *  SAVE_RESTORE_BOTH:  Restore title and icon (which we don't have)
 */
    ਖਾਲੀ
mch_restore_title(ਪੂਰਨ which ਅਣਵਰਤਿਆ)
{
#ਜੇ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
    ਜੇ (!gui.ਵਿੱਚ_use)
# endif
	ਸੈੱਟ_Console_Title(g_szOrigTitle);
#ਜੇ_ਅੰਤ
}


/*
 * Return TRUE if we can restore the title (we can)
 */
    ਪੂਰਨ
mch_can_restore_title(ਖਾਲੀ)
{
    ਵਾਪਸ TRUE;
}


/*
 * Return TRUE if we can restore the icon title (we can't)
 */
    ਪੂਰਨ
mch_can_restore_icon(ਖਾਲੀ)
{
    ਵਾਪਸ FALSE;
}


/*
 * Get absolute file name into buffer "buf" of length "len" bytes,
 * turning all '/'s into '\\'s and getting the correct case of each component
 * of the file name.  Append a (back)slash to a directory name.
 * When 'shellslash' set do it the other way around.
 * Return OK or FAIL.
 */
    ਪੂਰਨ
mch_ਭਰਿਆ_Name(
    char_u	*fname,
    char_u	*buf,
    ਪੂਰਨ		len,
    ਪੂਰਨ		force ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		n_ਨਤੀਜਾ = FAIL;
    WCHAR	*wname;
    WCHAR	wbuf[MAX_ਰਸਤਾ];
    char_u	*cname = ਨਿਰਾ;

    wname = enc_ਮੰਜ਼ਿਲ_utf16(fname, ਨਿਰਾ);
    ਜੇ (wname != ਨਿਰਾ && _wfullpath(wbuf, wname, MAX_ਰਸਤਾ) != ਨਿਰਾ)
    {
	cname = utf16_ਮੰਜ਼ਿਲ_enc((short_u *)wbuf, ਨਿਰਾ);
	ਜੇ (cname != ਨਿਰਾ)
	{
	    vim_strncpy(buf, cname, len - 1);
	    n_ਨਤੀਜਾ = OK;
	}
    }
    vim_free(wname);
    vim_free(cname);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_FNAME_CASE
    fname_case(buf, len);
#ਨਹੀਂਤਾਂ
    slash_adjust(buf);
#ਜੇ_ਅੰਤ

    ਵਾਪਸ n_ਨਤੀਜਾ;
}


/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    ਪੂਰਨ
mch_is_ਭਰਿਆ_Name(char_u *fname)
{
    // A name like "d:/foo" and "//server/share" is absolute.  "d:foo" is not.
    // /foo and \foo are absolute too because Windows keeps a current drive.
    // Another way to check is to use mch_FullName() and see if the result is
    // the same as the name or mch_FullName() fails.  However, this has quite a
    // bit of overhead, so let's not do that.
    ਜੇ (*fname == NUL)
	ਵਾਪਸ FALSE;
    ਵਾਪਸ ((ASCII_ISALPHA(fname[0]) && fname[1] == ':'
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
    ਖਾਲੀ
slash_adjust(char_u *p)
{
    ਜੇ (ਰਸਤਾ_with_url(p))
	ਵਾਪਸ;

    ਜੇ (*p == '`')
    {
	size_t len = STRLEN(p);

	// don't replace backslash in backtick quoted strings
	ਜੇ (len > 2 && *(p + len - 1) == '`')
	    ਵਾਪਸ;
    }

    ਜਦੋਂ (*p)
    {
	ਜੇ (*p == psepcN)
	    *p = psepc;
	MB_PTR_ADV(p);
    }
}

    ਸਥਿਰ ਪੂਰਨ
read_reparse_ਬਿੰਦੂ(ਅਚਲ WCHAR *name, char_u *buf, DWORD *buf_len)
{
    HANDLE h;
    BOOL ok;

    h = CreateFileW(name, FILE_READ_ਗੁਣ,
	    FILE_SHARE_READ | FILE_SHARE_WRITE, ਨਿਰਾ,
	    OPEN_EXISTING,
	    FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_ਬਿੰਦੂ,
	    ਨਿਰਾ);
    ਜੇ (h == INVALID_HANDLE_ਮੁੱਲ)
	ਵਾਪਸ FAIL;

    ok = DeviceIoControl(h, FSCTL_GET_REPARSE_ਬਿੰਦੂ, ਨਿਰਾ, 0, buf, *buf_len,
	    buf_len, ਨਿਰਾ);
    CloseHandle(h);

    ਵਾਪਸ ok ? OK : FAIL;
}

    char_u *
resolve_appexeclink(char_u *fname)
{
    DWORD		attr = 0;
    ਪੂਰਨ			idx;
    WCHAR		*p, *ਸਿਰਾ, *wname;
    // The buffer size is arbitrarily chosen to be "big enough" (TM), the
    // ceiling should be around 16k.
    char_u		buf[4096];
    DWORD		buf_len = ਆਕਾਰ(buf);
    REPARSE_ਅੰਕੜੇ_ਬਫਰ *rb = (REPARSE_ਅੰਕੜੇ_ਬਫਰ *)buf;

    wname = enc_ਮੰਜ਼ਿਲ_utf16(fname, ਨਿਰਾ);
    ਜੇ (wname == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    attr = Get_File_ਗੁਣ_W(wname);
    ਜੇ (attr == INVALID_FILE_ਗੁਣ ||
	    (attr & FILE_ATTRIBUTE_REPARSE_ਬਿੰਦੂ) == 0)
    {
	vim_free(wname);
	ਵਾਪਸ ਨਿਰਾ;
    }

    // The applinks are similar to symlinks but with a huge difference: they can
    // only be executed, any other I/O operation on them is bound to fail with
    // ERROR_FILE_NOT_FOUND even though the file exists.
    ਜੇ (read_reparse_ਬਿੰਦੂ(wname, buf, &buf_len) == FAIL)
    {
	vim_free(wname);
	ਵਾਪਸ ਨਿਰਾ;
    }
    vim_free(wname);

    ਜੇ (rb->Reparse_ਟੈਗ != IO_REPARSE_ਟੈਗ_APPEXECLINK)
	ਵਾਪਸ ਨਿਰਾ;

    // The (undocumented) reparse buffer contains a set of N null-terminated
    // Unicode strings, the application path is stored in the third one.
    ਜੇ (rb->App_Exec_Link_Reparse_ਬਫਰ.String_ਗਿਣਤੀ < 3)
	ਵਾਪਸ ਨਿਰਾ;

    p = rb->App_Exec_Link_Reparse_ਬਫਰ.StringList;
    ਸਿਰਾ = p + rb->Reparse_ਅੰਕੜੇ_ਲੰਮਾਈ / ਆਕਾਰ(WCHAR);
    ਲਈ (idx = 0; p < ਸਿਰਾ
	    && idx < (ਪੂਰਨ)rb->App_Exec_Link_Reparse_ਬਫਰ.String_ਗਿਣਤੀ
	    && idx != 2; )
    {
	ਜੇ (*p++ == L'\0')
	    ++idx;
    }

    ਵਾਪਸ utf16_ਮੰਜ਼ਿਲ_enc(p, ਨਿਰਾ);
}

// Use 64-bit stat functions.
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ stat
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ _stat
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ _wstat
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ _fstat
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ stat _stat64
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _stat _stat64
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _wstat _wstat64
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _fstat _fstat64

/*
 * Implements lstat() and stat() that can handle symlinks properly.
 */
    ਸਥਿਰ ਪੂਰਨ
mswin_stat_impl(ਅਚਲ WCHAR *name, stat_T *stp, ਅਚਲ ਪੂਰਨ resolve)
{
    ਪੂਰਨ			n;
    ਪੂਰਨ			fd;
    BOOL		is_symlink = FALSE;
    HANDLE		h_ਲੱਭੋ, h;
    DWORD		attr = 0;
    DWORD		flag = 0;
    WIN32_ਲੱਭੋ_DATAW    ਲੱਭੋ_ਅੰਕੜੇ_W;

#ਜੇ 0 && defined(_UCRT)
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
    ਜੇ (resolve)
	// Universal CRT can handle symlinks properly.
	ਵਾਪਸ _wstat(name, stp);
#ਜੇ_ਅੰਤ

    // Only reparse points need a directory query to identify symbolic links.
    // In particular, avoid enumerating the directory for each missing file
    // while searching 'runtimepath'.
    attr = Get_File_ਗੁਣ_W(name);
    ਜੇ (attr == INVALID_FILE_ਗੁਣ)
    {
	DWORD error = Get_ਆਖਰੀ_Error();

	// A missing file or path cannot provide stat information.  Other
	// errors and UNC paths still need the CRT fallback.
	ਜੇ ((error == ERROR_FILE_NOT_ਮਿਲਿਆ || error == ERROR_ਰਸਤਾ_NOT_ਮਿਲਿਆ)
		&& !((name[0] == L'\\' || name[0] == L'/')
		    && (name[1] == L'\\' || name[1] == L'/')))
	{
	    errno = ENOENT;
	    ਵਾਪਸ -1;
	}
	ਵਾਪਸ _wstat(name, stp);
    }
    ਜੇ (!(attr & FILE_ATTRIBUTE_REPARSE_ਬਿੰਦੂ))
	ਵਾਪਸ _wstat(name, stp);

    h_ਲੱਭੋ = ਲੱਭੋ_ਪਹਿਲਾ_File_W(name, &ਲੱਭੋ_ਅੰਕੜੇ_W);
    ਜੇ (h_ਲੱਭੋ != INVALID_HANDLE_ਮੁੱਲ)
    {
	attr = ਲੱਭੋ_ਅੰਕੜੇ_W.dw_File_ਗੁਣ;
	ਜੇ ((attr & FILE_ATTRIBUTE_REPARSE_ਬਿੰਦੂ)
		&& (ਲੱਭੋ_ਅੰਕੜੇ_W.dwReserved0 == IO_REPARSE_ਟੈਗ_SYMLINK))
	    is_symlink = TRUE;
	ਲੱਭੋ_Close(h_ਲੱਭੋ);
    }

    // Use the plain old stat() whenever it's possible.
    ਜੇ (!is_symlink)
	ਵਾਪਸ _wstat(name, stp);

    ਜੇ (!resolve && is_symlink)
	flag = FILE_FLAG_OPEN_REPARSE_ਬਿੰਦੂ;
    ਜੇ (attr & FILE_ATTRIBUTE_DIRECTORY)
	flag |= FILE_FLAG_BACKUP_SEMANTICS;

    h = CreateFileW(name, FILE_READ_ਗੁਣ,
	    FILE_SHARE_READ | FILE_SHARE_WRITE, ਨਿਰਾ, OPEN_EXISTING, flag,
	    ਨਿਰਾ);
    ਜੇ (h == INVALID_HANDLE_ਮੁੱਲ)
	ਵਾਪਸ -1;

    fd = _open_osfhandle((intptr_t)h, _O_RDONLY);
    n = _fstat(fd, (ਬਣਤਰ _stat *)stp);
    ਜੇ ((n == 0) && (attr & FILE_ATTRIBUTE_DIRECTORY))
	stp->st_ਬਹੁਲਕ = (stp->st_ਬਹੁਲਕ & ~S_IFMT) | S_IFDIR;
    _close(fd);

    ਵਾਪਸ n;
}

/*
 * stat() can't handle a trailing '/' or '\', remove it first.
 * When 'resolve' is true behave as lstat() wrt symlinks.
 */
    ਸਥਿਰ ਪੂਰਨ
stat_impl(ਅਚਲ ਅੱਖਰ *name, stat_T *stp, ਅਚਲ ਪੂਰਨ resolve)
{
    // WinNT and later can use _MAX_PATH wide characters for a pathname, which
    // means that the maximum pathname is _MAX_PATH * 3 bytes when 'enc' is
    // UTF-8.
    char_u	buf[MAX_ਰਸਤਾ_2 * 3 + 1];
    size_t	buflen;
    char_u	*p;
    WCHAR	*wp;
    ਪੂਰਨ		n;

    vim_strncpy((char_u *)buf, (char_u *)name, ਆਕਾਰ(buf) - 1);
    buflen = STRLEN(buf);
    p = buf + buflen;
    ਜੇ (p > buf)
	MB_PTR_ਪਿੱਠ(buf, p);

    // Remove trailing '\\' except root path.
    ਜੇ (p > buf && (*p == '\\' || *p == '/') && p[-1] != ':')
    {
	*p = NUL;
	buflen = (size_t)(p - buf);
    }

    ਜੇ ((buf[0] == '\\' && buf[1] == '\\') || (buf[0] == '/' && buf[1] == '/'))
    {
	// UNC root path must be followed by '\\'.
	p = vim_strpbrk(buf + 2, (char_u *)"\\/");
	ਜੇ (p != ਨਿਰਾ)
	{
	    p = vim_strpbrk(p + 1, (char_u *)"\\/");
	    ਜੇ (p == ਨਿਰਾ)
		STRCPY(buf + buflen, "\\");
	}
    }

    wp = enc_ਮੰਜ਼ਿਲ_utf16(buf, ਨਿਰਾ);
    ਜੇ (wp == ਨਿਰਾ)
	ਵਾਪਸ -1;

    n = mswin_stat_impl(wp, stp, resolve);
    vim_free(wp);
    ਵਾਪਸ n;
}

    ਪੂਰਨ
vim_lstat(ਅਚਲ ਅੱਖਰ *name, stat_T *stp)
{
    ਵਾਪਸ stat_impl(name, stp, FALSE);
}

    ਪੂਰਨ
vim_stat(ਅਚਲ ਅੱਖਰ *name, stat_T *stp)
{
    ਵਾਪਸ stat_impl(name, stp, TRUE);
}

#ਜੇ defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    ਖਾਲੀ
mch_settmode(tmode_T tmode ਅਣਵਰਤਿਆ)
{
    // nothing to do
}

    ਪੂਰਨ
mch_get_shellsize(ਖਾਲੀ)
{
    // never used
    ਵਾਪਸ OK;
}

    ਖਾਲੀ
mch_ਸੈੱਟ_shellsize(ਖਾਲੀ)
{
    // never used
}

/*
 * Rows and/or Columns has changed.
 */
    ਖਾਲੀ
mch_new_shellsize(ਖਾਲੀ)
{
    // never used
}

    ਖਾਲੀ
mch_calc_cell_ਮਾਪ(ਬਣਤਰ cellsize *cs_ਬਾਹਰਲਾ ਅਣਵਰਤਿਆ)
{
    // never used
}

#ਜੇ_ਅੰਤ

/*
 * We have no job control, so fake it by starting a new shell.
 */
    ਖਾਲੀ
mch_suspend(ਖਾਲੀ)
{
    suspend_shell();
}

#ਜੇ defined(USE_MCH_ERRMSG)

# ifdef display_errors
#  undef display_errors
# endif

/*
 * Display the saved error message(s).
 */
    ਖਾਲੀ
display_errors(ਖਾਲੀ)
{
# ifdef FEAT_GUI
    char_u *p;

#  ifdef VIMDLL
    ਜੇ (gui.ਵਿੱਚ_use || gui.starting)
#  endif
    {
	ਜੇ (error_ga.ga_ਅੰਕੜੇ != ਨਿਰਾ)
	{
	    // avoid putting up a message box with blanks only
	    ਲਈ (p = (char_u *)error_ga.ga_ਅੰਕੜੇ; *p; ++p)
		ਜੇ (!ਸੁਰੱਖਿਅਤ_isspace(*p))
		{
		    // Only use a dialog when not using --gui-dialog-file:
		    // write text to a file.
		    ਜੇ (!gui_dialog_log((char_u *)"Errors", p))
			(ਖਾਲੀ)gui_mch_dialog(
				     gui.starting ? VIM_INFO :
					     VIM_ERROR,
				     gui.starting ? (char_u *)_("Message") :
					     (char_u *)_("Error"),
					     p, (char_u *)_("&Ok"),
					1, ਨਿਰਾ, FALSE);
		    ਰੋਕੋ;
		}
	    ga_clear(&error_ga);
	}
	ਵਾਪਸ;
    }
# endif
# ਜੇ !defined(FEAT_GUI) || defined(VIMDLL)
    FlushFileBuffers(GetStdHandle(STD_ERROR_HANDLE));
# endif
}
#ਜੇ_ਅੰਤ


/*
 * Return TRUE if "p" contain a wildcard that can be expanded by
 * dos_expandpath().
 */
    ਪੂਰਨ
mch_has_exp_wildcard(char_u *p)
{
    ਲਈ ( ; *p; MB_PTR_ADV(p))
    {
	ਜੇ (vim_strchr((char_u *)"?*[", *p) != ਨਿਰਾ
		|| (*p == '~' && p[1] != NUL))
	    ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Return TRUE if "p" contain a wildcard or a "~1" kind of thing (could be a
 * shortened file name).
 */
    ਪੂਰਨ
mch_has_wildcard(char_u *p)
{
    ਲਈ ( ; *p; MB_PTR_ADV(p))
    {
	ਜੇ (vim_strchr((char_u *)"?*$[`", *p) != ਨਿਰਾ
		|| (*p == '~' && p[1] != NUL))
	    ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}


/*
 * The normal _chdir() does not change the default drive.  This one does.
 * Returning 0 implies success; -1 implies failure.
 */
    ਪੂਰਨ
mch_chdir(ਅੱਖਰ *ਰਸਤਾ)
{
    WCHAR   *p;
    ਪੂਰਨ	    n;

    ਜੇ (ਰਸਤਾ[0] == NUL)		// just checking...
	ਵਾਪਸ -1;

    ਜੇ (p_verbose >= 5)
    {
	verbose_enter();
	smsg("chdir(%s)", ਰਸਤਾ);
	verbose_leave();
    }
    ਜੇ (ਸੁਰੱਖਿਅਤ_isalpha(ਰਸਤਾ[0]) && ਰਸਤਾ[1] == ':')	// has a drive name
    {
	// If we can change to the drive, skip that part of the path.  If we
	// can't then the current directory may be invalid, try using chdir()
	// with the whole path.
	ਜੇ (_chdrive(TOLOWER_ASC(ਰਸਤਾ[0]) - 'a' + 1) == 0)
	    ਰਸਤਾ += 2;
    }

    ਜੇ (*ਰਸਤਾ == NUL)		// drive name only
	ਵਾਪਸ 0;

    p = enc_ਮੰਜ਼ਿਲ_utf16((char_u *)ਰਸਤਾ, ਨਿਰਾ);
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ -1;

    n = _wchdir(p);
    vim_free(p);
    ਵਾਪਸ n;
}


#ਜੇ defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
/*
 * return non-zero if a character is available
 */
    ਪੂਰਨ
mch_char_avail(ਖਾਲੀ)
{
    // never used
    ਵਾਪਸ TRUE;
}

# ਜੇ defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    ਪੂਰਨ
mch_check_messages(ਖਾਲੀ)
{
    // TODO: check for messages
    ਵਾਪਸ TRUE;
}
# endif
#ਜੇ_ਅੰਤ


#ਜੇ defined(FEAT_LIBCALL)
/*
 * Call a DLL routine which takes either a string or int param
 * and returns an allocated string.
 * Return OK if it worked, FAIL if not.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ char_u *(*MYSTRPROCSTR)(char_u *);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*MYSTRPROCINT)(char_u *);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ char_u *(*MYINTPROCSTR)(ਪੂਰਨ);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*MYINTPROCINT)(ਪੂਰਨ);

/*
 * Check if a pointer points to a valid NUL terminated string.
 * Return the length of the string, including terminating NUL.
 * Returns 0 for an invalid pointer, 1 for an empty string.
 */
    ਸਥਿਰ size_t
check_str_len(char_u *str)
{
    SYSTEM_INFO			si;
    ਸਮ੍ਰਿਤੀ_BASIC_INFORMATION	mbi;
    size_t			ਲੰਮਾਈ = 0;
    size_t			i;
    ਅਚਲ char_u		*p;

    // get page size
    GetSystemInfo(&si);

    // get memory information
    ਜੇ (!VirtualQuery(str, &mbi, ਆਕਾਰ(mbi)))
	ਵਾਪਸ 0;

    // pre cast these (typing savers)
    long_u dwStr = (long_u)str;
    long_u dw_ਅਧਾਰ_ਪਤਾ = (long_u)mbi.ਅਧਾਰ_ਪਤਾ;

    // get start address of page that str is on
    long_u str_ਪੰਨਾ = dwStr - (dwStr - dw_ਅਧਾਰ_ਪਤਾ) % si.dw_ਪੰਨਾ_ਮਾਪ;

    // get length from str to end of page
    long_u ਪੰਨਾ_ਲੰਮਾਈ = si.dw_ਪੰਨਾ_ਮਾਪ - (dwStr - str_ਪੰਨਾ);

    ਲਈ (p = str; !IsBadReadPtr(p, (UINT)ਪੰਨਾ_ਲੰਮਾਈ);
	    p += ਪੰਨਾ_ਲੰਮਾਈ, ਪੰਨਾ_ਲੰਮਾਈ = si.dw_ਪੰਨਾ_ਮਾਪ)
	ਲਈ (i = 0; i < ਪੰਨਾ_ਲੰਮਾਈ; ++i, ++ਲੰਮਾਈ)
	    ਜੇ (p[i] == NUL)
		ਵਾਪਸ ਲੰਮਾਈ + 1;

    ਵਾਪਸ 0;
}

/*
 * Passed to do_in_runtimepath() to load a vim.ico file.
 */
    ਸਥਿਰ ਖਾਲੀ
mch_icon_ਲੱਦੋ_cb(char_u *fname, ਖਾਲੀ *cookie)
{
    HANDLE *h = (HANDLE *)cookie;

    *h = ਲੱਦੋ_Image(ਨਿਰਾ,
		   (LPSTR)fname,
		   IMAGE_ICON,
		   64,
		   64,
		   LR_LOADFROMFILE | LR_LOADMAP3DCOLORS);
}

/*
 * Try loading an icon file from 'runtimepath'.
 */
    ਪੂਰਨ
mch_icon_ਲੱਦੋ(HANDLE *iconp)
{
    ਵਾਪਸ do_ਵਿੱਚ_runtimepath((char_u *)"bitmaps/vim.ico",
						  0, mch_icon_ਲੱਦੋ_cb, iconp);
}

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if the OS PRNG is not available or something went wrong.
 */
    ਪੂਰਨ
mch_get_random(char_u *buf, ਪੂਰਨ len)
{
    ਸਥਿਰ ਪੂਰਨ		initialized = NOTDONE;
    ਸਥਿਰ HINSTANCE	hInstLib;
    ਸਥਿਰ BOOL (WINAPI *pProcessPrng)(PUCHAR, ULONG);

    ਜੇ (initialized == NOTDONE)
    {
	hInstLib = vim_ਲੱਦੋ_Lib("bcryptprimitives.dll");
	ਜੇ (hInstLib != ਨਿਰਾ)
	    pProcessPrng = (ਖਾਲੀ *)Get_Proc_ਪਤਾ(hInstLib, "ProcessPrng");
	ਜੇ (hInstLib == ਨਿਰਾ || pProcessPrng == ਨਿਰਾ)
	{
	    FreeLibrary(hInstLib);
	    initialized = FAIL;
	}
	ਨਹੀਂਤਾਂ
	    initialized = OK;
    }

    ਜੇ (initialized == FAIL)
	ਵਾਪਸ FAIL;

    // According to the documentation this call cannot fail.
    pProcessPrng(buf, len);

    ਵਾਪਸ OK;
}

    ਪੂਰਨ
mch_libcall(
    char_u	*libname,
    char_u	*funcname,
    char_u	*argstring,	// NULL when using a argint
    ਪੂਰਨ		argint,
    char_u	**string_ਨਤੀਜਾ,// NULL when using number_result
    ਪੂਰਨ		*ਸੰਖਿਆ_ਨਤੀਜਾ)
{
    HINSTANCE		hinstLib;

    BOOL f_ਲੜੀਵਾਰ_ਸਮਾਂ_Link_Success = FALSE;

    // Get a handle to the DLL module.
    hinstLib = vim_ਲੱਦੋ_Lib((ਅੱਖਰ *)libname);

    // If the handle is valid, try to get the function address.
    ਜੇ (hinstLib != ਨਿਰਾ)
    {
	char_u	*retval_str = ਨਿਰਾ;
	ਪੂਰਨ	retval_int = 0;
	size_t	len;

# ifdef HAVE_TRY_EXCEPT
	__try
	{
# endif
	ਜੇ (argstring != ਨਿਰਾ)
	{
	    // Call with string argument
	    ਜੇ (string_ਨਤੀਜਾ != ਨਿਰਾ)
	    {
		MYSTRPROCSTR proc;

		proc = (MYSTRPROCSTR)Get_Proc_ਪਤਾ(hinstLib, (LPCSTR)funcname);
		ਜੇ (proc != ਨਿਰਾ)
		{
		    f_ਲੜੀਵਾਰ_ਸਮਾਂ_Link_Success = TRUE;
		    retval_str = proc(argstring);
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		MYSTRPROCINT proc;

		proc = (MYSTRPROCINT)Get_Proc_ਪਤਾ(hinstLib, (LPCSTR)funcname);
		ਜੇ (proc != ਨਿਰਾ)
		{
		    f_ਲੜੀਵਾਰ_ਸਮਾਂ_Link_Success = TRUE;
		    retval_int = proc(argstring);
		}
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    // Call with number argument
	    ਜੇ (string_ਨਤੀਜਾ != ਨਿਰਾ)
	    {
		MYINTPROCSTR proc;

		proc = (MYINTPROCSTR)Get_Proc_ਪਤਾ(hinstLib, (LPCSTR)funcname);
		ਜੇ (proc != ਨਿਰਾ)
		{
		    f_ਲੜੀਵਾਰ_ਸਮਾਂ_Link_Success = TRUE;
		    retval_str = proc(argint);
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		MYINTPROCINT proc;

		proc = (MYINTPROCINT)Get_Proc_ਪਤਾ(hinstLib, (LPCSTR)funcname);
		ਜੇ (proc != ਨਿਰਾ)
		{
		    f_ਲੜੀਵਾਰ_ਸਮਾਂ_Link_Success = TRUE;
		    retval_int = proc(argint);
		}
	    }
	}

	// Save the string before we free the library.
	// Assume that a "1" result is an illegal pointer.
	ਜੇ (string_ਨਤੀਜਾ == ਨਿਰਾ)
	    *ਸੰਖਿਆ_ਨਤੀਜਾ = retval_int;
	ਨਹੀਂਤਾਂ ਜੇ (retval_str != ਨਿਰਾ
		&& (len = check_str_len(retval_str)) > 0)
	{
	    *string_ਨਤੀਜਾ = alloc(len);
	    ਜੇ (*string_ਨਤੀਜਾ != ਨਿਰਾ)
		mch_memmove(*string_ਨਤੀਜਾ, retval_str, len);
	}

# ifdef HAVE_TRY_EXCEPT
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	    ਜੇ (Get_Exception_ਕੋਡ() == EXCEPTION_ਢੇਰ_ਉਛਾਲ)
		_resetstkoflw();
	    f_ਲੜੀਵਾਰ_ਸਮਾਂ_Link_Success = FALSE;
	}
# endif

	// Free the DLL module.
	(ਖਾਲੀ)FreeLibrary(hinstLib);
    }

    ਜੇ (!f_ਲੜੀਵਾਰ_ਸਮਾਂ_Link_Success)
    {
	semsg(_(e_library_call_failed_for_str), funcname);
	ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
}
#ਜੇ_ਅੰਤ

/*
 * Debugging helper: expose the MCH_WRITE_DUMP stuff to other modules
 */
    ਖਾਲੀ
DumpPutS(ਅਚਲ ਅੱਖਰ *psz ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MCH_WRITE_DUMP
    ਜੇ (fdDump)
    {
	fputs(psz, fdDump);
	ਜੇ (psz[ਲੜੀ_ਲੰਬਾਈ(psz) - 1] != '\n')
	    fputc('\n', fdDump);
	fflush(fdDump);
    }
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _DEBUG

ਖਾਲੀ __cdecl
Trace(
    ਅੱਖਰ *pszFormat,
    ...)
{
    CHAR szBuff[2048];
    va_list ਦਲੀਲਾਂ;

    va_start(ਦਲੀਲਾਂ, pszFormat);
    vsprintf(szBuff, pszFormat, ਦਲੀਲਾਂ);
    va_end(ਦਲੀਲਾਂ);

    OutputDebugString(szBuff);
}

#ਜੇ_ਅੰਤ //_DEBUG

#ਜੇ !defined(FEAT_GUI) || defined(VIMDLL)
ਬਾਹਰੀ HWND g_hWnd;	// This is in os_win32.c.

/*
 * Showing the printer dialog is tricky since we have no GUI
 * window to parent it. The following routines are needed to
 * get the window parenting and Z-order to work properly.
 */
    ਸਥਿਰ ਖਾਲੀ
GetConsoleHwnd(ਖਾਲੀ)
{
    // Skip if it's already set.
    ਜੇ (s_hwnd != 0)
	ਵਾਪਸ;

    // Window handle may have been found by init code (Windows NT only)
    ਜੇ (g_hWnd != 0)
    {
	s_hwnd = g_hWnd;
	ਵਾਪਸ;
    }

    s_hwnd = Get_Console_ਖਿੜਕੀ();
}

/*
 * Console implementation of ":winpos".
 */
    ਪੂਰਨ
mch_get_winpos(ਪੂਰਨ *x, ਪੂਰਨ *y)
{
    RECT  rect;

    GetConsoleHwnd();
    Get_ਖਿੜਕੀ_Rect(s_hwnd, &rect);
    *x = rect.ਖੱਬਾ;
    *y = rect.ਸਿਖਰ;
    ਵਾਪਸ OK;
}

/*
 * Console implementation of ":winpos x y".
 */
    ਖਾਲੀ
mch_ਸੈੱਟ_winpos(ਪੂਰਨ x, ਪੂਰਨ y)
{
    GetConsoleHwnd();
    ਸੈੱਟ_ਖਿੜਕੀ_Pos(s_hwnd, ਨਿਰਾ, x, y, 0, 0,
		 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT)

//=================================================================
// Win32 printer stuff

ਸਥਿਰ HFONT		prt_font_handles[2][2][2];
ਸਥਿਰ PRINTDLGW	prt_dlg;
ਸਥਿਰ ਅਚਲ ਪੂਰਨ	boldface[2] = {FW_REGULAR, FW_BOLD};
ਸਥਿਰ TEXTMETRIC	prt_tm;
ਸਥਿਰ ਪੂਰਨ		prt_ਸਤਰ_ਉਚਾਈ;
ਸਥਿਰ ਪੂਰਨ		prt_ਸੰਖਿਆ_ਚੌੜਾਈ;
ਸਥਿਰ ਪੂਰਨ		prt_ਖੱਬਾ_margin;
ਸਥਿਰ ਪੂਰਨ		prt_ਸੱਜਾ_margin;
ਸਥਿਰ ਪੂਰਨ		prt_ਸਿਖਰ_margin;
ਸਥਿਰ char_u		szAppName[] = ਪਾਠ_3("VIM");
ਸਥਿਰ HWND		hDlgPrint;
ਸਥਿਰ ਪੂਰਨ		*bUserAbort = ਨਿਰਾ;
ਸਥਿਰ char_u		*prt_name = ਨਿਰਾ;

// Defines which are also in vim.rc.
# define IDC_BOX1		400
# define IDC_PRINTTEXT1		401
# define IDC_PRINTTEXT2		402
# define IDC_ਤਰੱਕੀ		403

    ਸਥਿਰ BOOL
vim_ਸੈੱਟ_Dlg_ਵਸਤੂ_ਪਾਠ(HWND hDlg, ਪੂਰਨ n_IDDlg_ਵਸਤੂ, char_u *s)
{
    WCHAR   *wp;
    BOOL    ret;

    wp = enc_ਮੰਜ਼ਿਲ_utf16(s, ਨਿਰਾ);
    ਜੇ (wp == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    ret = ਸੈੱਟ_Dlg_ਵਸਤੂ_ਪਾਠ_W(hDlg, n_IDDlg_ਵਸਤੂ, wp);
    vim_free(wp);
    ਵਾਪਸ ret;
}

/*
 * Convert BGR to RGB for Windows GDI calls
 */
    ਸਥਿਰ COLORREF
ਅਦਲਾ_ਬਦਲੀ_me(COLORREF colorref)
{
    ਪੂਰਨ  ਆਰਜ਼ੀ;
    ਅੱਖਰ *ptr = (ਅੱਖਰ *)&colorref;

    ਆਰਜ਼ੀ = *(ptr);
    *(ptr ) = *(ptr + 2);
    *(ptr + 2) = ਆਰਜ਼ੀ;
    ਵਾਪਸ colorref;
}

    ਸਥਿਰ INT_PTR CALLBACK
PrintDlgProc(
	HWND hDlg,
	UINT ਸੁਨੇਹਾ_2,
	WPARAM wParam ਅਣਵਰਤਿਆ,
	LPARAM lParam ਅਣਵਰਤਿਆ)
{
# ifdef FEAT_GETTEXT
    NONCLIENTMETRICS nm;
    ਸਥਿਰ HFONT hfont;
# endif

    ਚੋਣ (ਸੁਨੇਹਾ_2)
    {
	ਹਾਲਤ WM_INITDIALOG:
# ifdef FEAT_GETTEXT
	    nm.cb_ਮਾਪ = ਆਕਾਰ(NONCLIENTMETRICS);
	    ਜੇ (SystemParametersInfo(
			SPI_GETNONCLIENTMETRICS,
			ਆਕਾਰ(NONCLIENTMETRICS),
			&nm,
			0))
	    {
		ਅੱਖਰ buff[MAX_ਰਸਤਾ];
		ਪੂਰਨ i;

		// Translate the dialog texts
		hfont = Create_Font_ਅਸਿੱਧਾ(&nm.lf_ਸੁਨੇਹਾ_Font);
		ਲਈ (i = IDC_PRINTTEXT1; i <= IDC_ਤਰੱਕੀ; i++)
		{
		    Send_Dlg_ਵਸਤੂ_ਸੁਨੇਹਾ(hDlg, i, WM_SETFONT, (WPARAM)hfont, 1);
		    ਜੇ (Get_Dlg_ਵਸਤੂ_ਪਾਠ(hDlg,i, buff, ਆਕਾਰ(buff)))
			vim_ਸੈੱਟ_Dlg_ਵਸਤੂ_ਪਾਠ(hDlg,i, (char_u *)_(buff));
		}
		Send_Dlg_ਵਸਤੂ_ਸੁਨੇਹਾ(hDlg, IDCANCEL,
						WM_SETFONT, (WPARAM)hfont, 1);
		ਜੇ (Get_Dlg_ਵਸਤੂ_ਪਾਠ(hDlg,IDCANCEL, buff, ਆਕਾਰ(buff)))
		    vim_ਸੈੱਟ_Dlg_ਵਸਤੂ_ਪਾਠ(hDlg,IDCANCEL, (char_u *)_(buff));
	    }
# endif
	    ਸੈੱਟ_ਖਿੜਕੀ_ਪਾਠ(hDlg, (LPCSTR)szAppName);
	    ਜੇ (prt_name != ਨਿਰਾ)
	    {
		vim_ਸੈੱਟ_Dlg_ਵਸਤੂ_ਪਾਠ(hDlg, IDC_PRINTTEXT2, (char_u *)prt_name);
		VIM_CLEAR(prt_name);
	    }
	    Enable_Menu_ਵਸਤੂ(GetSystemMenu(hDlg, FALSE), SC_CLOSE, MF_GRAYED);
# ਜੇ !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	    ਜੇ (!gui.ਵਿੱਚ_use)
#  endif
		Bring_ਖਿੜਕੀ_ਮੰਜ਼ਿਲ_ਸਿਖਰ(s_hwnd);
# endif
	    ਵਾਪਸ TRUE;

	ਹਾਲਤ WM_COMMAND:
	    *bUserAbort = TRUE;
	    Enable_ਖਿੜਕੀ(Get_ਮਾਪਾ(hDlg), TRUE);
	    Destroy_ਖਿੜਕੀ(hDlg);
	    hDlgPrint = ਨਿਰਾ;
# ifdef FEAT_GETTEXT
	    DeleteObject(hfont);
# endif
	    ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

    ਸਥਿਰ BOOL CALLBACK
AbortProc(HDC hdcPrn ਅਣਵਰਤਿਆ, ਪੂਰਨ i_ਕੋਡ ਅਣਵਰਤਿਆ)
{
    ਸੁਨੇਹਾ_3 ਸੁਨੇਹਾ;

    ਜਦੋਂ (!*bUserAbort && Peek_ਸੁਨੇਹਾ_W(&ਸੁਨੇਹਾ, ਨਿਰਾ, 0, 0, PM_REMOVE))
    {
	ਜੇ (!hDlgPrint || !Is_Dialog_ਸੁਨੇਹਾ_W(hDlgPrint, &ਸੁਨੇਹਾ))
	{
	    Translate_ਸੁਨੇਹਾ(&ਸੁਨੇਹਾ);
	    Dispatch_ਸੁਨੇਹਾ_W(&ਸੁਨੇਹਾ);
	}
    }
    ਵਾਪਸ !*bUserAbort;
}

    ਸਥਿਰ UINT_PTR CALLBACK
PrintHookProc(
	HWND hDlg,	// handle to dialog box
	UINT ui_ਸੁਨੇਹਾ,	// message identifier
	WPARAM wParam ਅਣਵਰਤਿਆ,	// message parameter
	LPARAM lParam	// message parameter
	)
{
    HWND	hwndOwner;
    RECT	rc, rcDlg, rcOwner;
    PRINTDLGW	*pPD;

    ਜੇ (ui_ਸੁਨੇਹਾ != WM_INITDIALOG)
	ਵਾਪਸ FALSE;

    // Get the owner window and dialog box rectangles.
    ਜੇ ((hwndOwner = Get_ਮਾਪਾ(hDlg)) == ਨਿਰਾ)
	hwndOwner = Get_Desktop_ਖਿੜਕੀ();

    Get_ਖਿੜਕੀ_Rect(hwndOwner, &rcOwner);
    Get_ਖਿੜਕੀ_Rect(hDlg, &rcDlg);
    CopyRect(&rc, &rcOwner);

    // Offset the owner and dialog box rectangles so that
    // right and bottom values represent the width and
    // height, and then offset the owner again to discard
    // space taken up by the dialog box.

    ਵਿੱਥ_Rect(&rcDlg, -rcDlg.ਖੱਬਾ, -rcDlg.ਸਿਖਰ);
    ਵਿੱਥ_Rect(&rc, -rc.ਖੱਬਾ, -rc.ਸਿਖਰ);
    ਵਿੱਥ_Rect(&rc, -rcDlg.ਸੱਜਾ, -rcDlg.bottom);

    // The new position is the sum of half the remaining
    // space and the owner's original position.

    ਸੈੱਟ_ਖਿੜਕੀ_Pos(hDlg,
	    HWND_ਸਿਖਰ,
	    rcOwner.ਖੱਬਾ + (rc.ਸੱਜਾ / 2),
	    rcOwner.ਸਿਖਰ + (rc.bottom / 2),
	    0, 0,		// ignores size arguments
	    SWP_NOSIZE);

    //  tackle the printdlg copiesctrl problem
    pPD = (PRINTDLGW *)lParam;
    pPD->nCopies = (ਸ਼ਬਦ_2)pPD->l_Cust_ਅੰਕੜੇ;
    ਸੈੱਟ_Dlg_ਵਸਤੂ_Int( hDlg, edt3, pPD->nCopies, FALSE );
    //  Bring the window to top
    Bring_ਖਿੜਕੀ_ਮੰਜ਼ਿਲ_ਸਿਖਰ(Get_ਮਾਪਾ(hDlg));
    ਸੈੱਟ_Foreground_ਖਿੜਕੀ(hDlg);

    ਵਾਪਸ FALSE;
}

    ਖਾਲੀ
mch_print_cleanup(ਖਾਲੀ)
{
    ਪੂਰਨ pifItalic;
    ਪੂਰਨ pifBold;
    ਪੂਰਨ pifUnderline;

    ਲਈ (pifBold = 0; pifBold <= 1; pifBold++)
	ਲਈ (pifItalic = 0; pifItalic <= 1; pifItalic++)
	    ਲਈ (pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
		DeleteObject(prt_font_handles[pifBold][pifItalic][pifUnderline]);

    ਜੇ (prt_dlg.hDC != ਨਿਰਾ)
	DeleteDC(prt_dlg.hDC);
    ਜੇ (!*bUserAbort && hDlgPrint != ਨਿਰਾ)
	Send_ਸੁਨੇਹਾ(hDlgPrint, WM_COMMAND, 0, 0);
}

    ਸਥਿਰ ਪੂਰਨ
ਮੰਜ਼ਿਲ_device_units(ਪੂਰਨ idx, ਪੂਰਨ dpi, ਪੂਰਨ physsize, ਪੂਰਨ ਵਿੱਥ, ਪੂਰਨ def_ਸੰਖਿਆ)
{
    ਪੂਰਨ		ret = 0;
    ਪੂਰਨ		u;
    ਪੂਰਨ		nr;

    u = prt_get_unit(idx);
    ਜੇ (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_ਸੰਖਿਆ;
    }
    ਨਹੀਂਤਾਂ
	nr = printer_opts[idx].ਸੰਖਿਆ;

    ਚੋਣ (u)
    {
	ਹਾਲਤ PRT_UNIT_PERC:
	    ret = (physsize * nr) / 100;
	    ਰੋਕੋ;
	ਹਾਲਤ PRT_UNIT_INCH:
	    ret = (nr * dpi);
	    ਰੋਕੋ;
	ਹਾਲਤ PRT_UNIT_MM:
	    ret = (nr * 10 * dpi) / 254;
	    ਰੋਕੋ;
	ਹਾਲਤ PRT_UNIT_ਬਿੰਦੂ:
	    ret = (nr * 10 * dpi) / 720;
	    ਰੋਕੋ;
    }

    ਜੇ (ret < ਵਿੱਥ)
	ਵਾਪਸ 0;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ ret - ਵਿੱਥ;
}

    ਸਥਿਰ ਪੂਰਨ
prt_get_cpl(ਖਾਲੀ)
{
    ਪੂਰਨ		hr;
    ਪੂਰਨ		phyw;
    ਪੂਰਨ		dvoff;
    ਪੂਰਨ		rev_ਵਿੱਥ;
    ਪੂਰਨ		dpi;

    Get_ਪਾਠ_Metrics(prt_dlg.hDC, &prt_tm);
    prt_ਸਤਰ_ਉਚਾਈ = prt_tm.tm_ਉਚਾਈ + prt_tm.tmExternalLeading;

    hr	    = GetDeviceCaps(prt_dlg.hDC, HORZRES);
    phyw    = GetDeviceCaps(prt_dlg.hDC, PHYSICALWIDTH);
    dvoff   = GetDeviceCaps(prt_dlg.hDC, PHYSICALOFFSETX);
    dpi	    = GetDeviceCaps(prt_dlg.hDC, LOGPIXELSX);

    rev_ਵਿੱਥ = phyw - (dvoff + hr);

    prt_ਖੱਬਾ_margin = ਮੰਜ਼ਿਲ_device_units(OPT_PRINT_ਖੱਬਾ, dpi, phyw, dvoff, 10);
    ਜੇ (prt_use_ਸੰਖਿਆ())
    {
	prt_ਸੰਖਿਆ_ਚੌੜਾਈ = PRINT_ਸੰਖਿਆ_ਚੌੜਾਈ * prt_tm.tm_Ave_Char_ਚੌੜਾਈ;
	prt_ਖੱਬਾ_margin += prt_ਸੰਖਿਆ_ਚੌੜਾਈ;
    }
    ਨਹੀਂਤਾਂ
	prt_ਸੰਖਿਆ_ਚੌੜਾਈ = 0;

    prt_ਸੱਜਾ_margin = hr - ਮੰਜ਼ਿਲ_device_units(OPT_PRINT_ਸੱਜਾ, dpi, phyw,
							       rev_ਵਿੱਥ, 5);

    ਵਾਪਸ (prt_ਸੱਜਾ_margin - prt_ਖੱਬਾ_margin) / prt_tm.tm_Ave_Char_ਚੌੜਾਈ;
}

    ਸਥਿਰ ਪੂਰਨ
prt_get_lpp(ਖਾਲੀ)
{
    ਪੂਰਨ vr;
    ਪੂਰਨ phyw;
    ਪੂਰਨ dvoff;
    ਪੂਰਨ rev_ਵਿੱਥ;
    ਪੂਰਨ	bottom_margin;
    ਪੂਰਨ	dpi;

    vr	    = GetDeviceCaps(prt_dlg.hDC, VERTRES);
    phyw    = GetDeviceCaps(prt_dlg.hDC, PHYSICALHEIGHT);
    dvoff   = GetDeviceCaps(prt_dlg.hDC, PHYSICALOFFSETY);
    dpi	    = GetDeviceCaps(prt_dlg.hDC, LOGPIXELSY);

    rev_ਵਿੱਥ = phyw - (dvoff + vr);

    prt_ਸਿਖਰ_margin = ਮੰਜ਼ਿਲ_device_units(OPT_PRINT_ਸਿਖਰ, dpi, phyw, dvoff, 5);

    // adjust top margin if there is a header
    prt_ਸਿਖਰ_margin += prt_ਸਤਰ_ਉਚਾਈ * prt_ਸਿਰਲੇਖ_ਉਚਾਈ();

    bottom_margin = vr - ਮੰਜ਼ਿਲ_device_units(OPT_PRINT_BOT, dpi, phyw,
							       rev_ਵਿੱਥ, 5);

    ਵਾਪਸ (bottom_margin - prt_ਸਿਖਰ_margin) / prt_ਸਤਰ_ਉਚਾਈ;
}

    ਪੂਰਨ
mch_print_init(prt_settings_T *psettings, char_u *jobname, ਪੂਰਨ forceit)
{
    ਸਥਿਰ HGLOBAL	stored_dm = ਨਿਰਾ;
    ਸਥਿਰ HGLOBAL	stored_devn = ਨਿਰਾ;
    ਸਥਿਰ ਪੂਰਨ		stored_nCopies = 1;
    ਸਥਿਰ ਪੂਰਨ		stored_nFlags = 0;

    LOGFONTW		fLogFont;
    ਪੂਰਨ			pifItalic;
    ਪੂਰਨ			pifBold;
    ਪੂਰਨ			pifUnderline;

    DEVMODEW		*mem;
    DEVNAMES		*devname;
    ਪੂਰਨ			i;
    DWORD		err;

    bUserAbort = &(psettings->user_abort);
    CLEAR_ਖੇਤਰ(prt_dlg);
    prt_dlg.l_Struct_ਮਾਪ = ਆਕਾਰ(PRINTDLGW);
# ਜੇ !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
    ਜੇ (!gui.ਵਿੱਚ_use)
#  endif
	GetConsoleHwnd();	    // get value of s_hwnd
# endif
    prt_dlg.hwndOwner = s_hwnd;
    prt_dlg.Flags = PD_NOPAGENUMS | PD_NOSELECTION | PD_RETURNDC;
    ਜੇ (!forceit)
    {
	prt_dlg.h_Dev_ਬਹੁਲਕ = stored_dm;
	prt_dlg.h_Dev_ਨਾਂ = stored_devn;
	prt_dlg.l_Cust_ਅੰਕੜੇ = stored_nCopies; // work around bug in print dialog
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
    ਜੇ (forceit
# ਜੇ !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	    || (!gui.ਵਿੱਚ_use && !ਪਦ_console)
#  ਨਹੀਂਤਾਂ
	    || !ਪਦ_console
#  endif
# endif
	    )
    {
	prt_dlg.Flags |= PD_RETURNDEFAULT;
	/*
	 * MSDN suggests setting the first parameter to WINSPOOL for
	 * NT, but NULL appears to work just as well.
	 */
	ਜੇ (*p_pdev != NUL)
	    prt_dlg.hDC = CreateDC(ਨਿਰਾ, (LPCSTR)p_pdev, ਨਿਰਾ, ਨਿਰਾ);
	ਨਹੀਂਤਾਂ
	{
	    prt_dlg.Flags |= PD_RETURNDEFAULT;
	    ਜੇ (PrintDlgW(&prt_dlg) == 0)
		ਜਾਓ init_fail_dlg;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (PrintDlgW(&prt_dlg) == 0)
	ਜਾਓ init_fail_dlg;
    ਨਹੀਂਤਾਂ
    {
	/*
	 * keep the previous driver context
	 */
	stored_dm = prt_dlg.h_Dev_ਬਹੁਲਕ;
	stored_devn = prt_dlg.h_Dev_ਨਾਂ;
	stored_nFlags = prt_dlg.Flags;
	stored_nCopies = prt_dlg.nCopies;
    }

    ਜੇ (prt_dlg.hDC == ਨਿਰਾ)
    {
	emsg(_(e_printer_selection_failed));
	mch_print_cleanup();
	ਵਾਪਸ FALSE;
    }

    // Not all printer drivers report the support of color (or grey) in the
    // same way.  Let's set has_color if there appears to be some way to print
    // more than B&W.
    i = GetDeviceCaps(prt_dlg.hDC, NUMCOLORS);
    psettings->has_color = (GetDeviceCaps(prt_dlg.hDC, BITSPIXEL) > 1
				   || GetDeviceCaps(prt_dlg.hDC, PLANES) > 1
				   || i > 2 || i == -1);

    // Ensure all font styles are baseline aligned
    ਸੈੱਟ_ਪਾਠ_Align(prt_dlg.hDC, TA_BASELINE|TA_ਖੱਬਾ);

    /*
     * On some windows systems the nCopies parameter is not
     * passed back correctly. It must be retrieved from the
     * hDevMode struct.
     */
    mem = (DEVMODEW *)Global_ਤਾਲਾ(prt_dlg.h_Dev_ਬਹੁਲਕ);
    ਜੇ (mem != ਨਿਰਾ)
    {
	ਜੇ (mem->dmCopies != 1)
	    stored_nCopies = mem->dmCopies;
	ਜੇ ((mem->dmFields & DM_DUPLEX) && (mem->dmDuplex & ~DMDUP_SIMPLEX))
	    psettings->duplex = TRUE;
	ਜੇ ((mem->dmFields & DM_COLOR) && (mem->dmColor & DMCOLOR_COLOR))
	    psettings->has_color = TRUE;
    }
    GlobalUnlock(prt_dlg.h_Dev_ਬਹੁਲਕ);

    devname = (DEVNAMES *)Global_ਤਾਲਾ(prt_dlg.h_Dev_ਨਾਂ);
    ਜੇ (devname != 0)
    {
	WCHAR	*wprinter_name = (WCHAR *)devname + devname->w_Device_ਵਿੱਥ;
	WCHAR	*wport_name = (WCHAR *)devname + devname->w_Output_ਵਿੱਥ;
	char_u	*ਪਾਠ = (char_u *)_("to %s on %s");
	char_u  *printer_name = utf16_ਮੰਜ਼ਿਲ_enc(wprinter_name, ਨਿਰਾ);
	char_u	*ਪੋਰਟ_name = utf16_ਮੰਜ਼ਿਲ_enc(wport_name, ਨਿਰਾ);

	ਜੇ (printer_name != ਨਿਰਾ && ਪੋਰਟ_name != ਨਿਰਾ)
	    prt_name = alloc(STRLEN(printer_name)
					   + STRLEN(ਪੋਰਟ_name) + STRLEN(ਪਾਠ));
	ਜੇ (prt_name != ਨਿਰਾ)
	    wsprintf((ਅੱਖਰ *)prt_name, (ਅਚਲ ਅੱਖਰ *)ਪਾਠ,
		    printer_name, ਪੋਰਟ_name);
	vim_free(printer_name);
	vim_free(ਪੋਰਟ_name);
    }
    GlobalUnlock(prt_dlg.h_Dev_ਨਾਂ);

    /*
     * Initialise the font according to 'printfont'
     */
    CLEAR_ਖੇਤਰ(fLogFont);
    ਜੇ (get_logfont(&fLogFont, p_pfn, prt_dlg.hDC, TRUE) == FAIL)
    {
	semsg(_(e_unknown_printer_font_str), p_pfn);
	mch_print_cleanup();
	ਵਾਪਸ FALSE;
    }

    ਲਈ (pifBold = 0; pifBold <= 1; pifBold++)
	ਲਈ (pifItalic = 0; pifItalic <= 1; pifItalic++)
	    ਲਈ (pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
	    {
		fLogFont.lf_ਭਾਰ =  boldface[pifBold];
		fLogFont.lfItalic = pifItalic;
		fLogFont.lfUnderline = pifUnderline;
		prt_font_handles[pifBold][pifItalic][pifUnderline]
					      = Create_Font_ਅਸਿੱਧਾ_W(&fLogFont);
	    }

    ਸੈੱਟ_Bk_ਬਹੁਲਕ(prt_dlg.hDC, OPAQUE);
    SelectObject(prt_dlg.hDC, prt_font_handles[0][0][0]);

    /*
     * Fill in the settings struct
     */
    psettings->chars_ਪ੍ਰਤੀ_ਸਤਰ = prt_get_cpl();
    psettings->ਸਤਰਾਂ_ਪ੍ਰਤੀ_ਪੰਨਾ = prt_get_lpp();
    ਜੇ (prt_dlg.Flags & PD_USEDEVMODECOPIESANDCOLLATE)
    {
	psettings->n_collated_copies = (prt_dlg.Flags & PD_COLLATE)
						    ? prt_dlg.nCopies : 1;
	psettings->n_uncollated_copies = (prt_dlg.Flags & PD_COLLATE)
						    ? 1 : prt_dlg.nCopies;

	ਜੇ (psettings->n_collated_copies == 0)
	    psettings->n_collated_copies = 1;

	ਜੇ (psettings->n_uncollated_copies == 0)
	    psettings->n_uncollated_copies = 1;
    }
    ਨਹੀਂਤਾਂ
    {
	psettings->n_collated_copies = 1;
	psettings->n_uncollated_copies = 1;
    }

    psettings->jobname = jobname;

    ਵਾਪਸ TRUE;

init_fail_dlg:
    err = CommDlgExtendedError();
    ਜੇ (err)
    {
	char_u *buf;

	// I suspect FormatMessage() doesn't work for values returned by
	// CommDlgExtendedError().  What does?
	Format_ਸੁਨੇਹਾ(FORMAT_ਸੁਨੇਹਾ_ALLOCATE_ਬਫਰ |
		FORMAT_ਸੁਨੇਹਾ_FROM_SYSTEM |
		FORMAT_ਸੁਨੇਹਾ_IGNORE_INSERTS,
		ਨਿਰਾ, err, 0, (LPTSTR)(&buf), 0, ਨਿਰਾ);
	semsg(_(e_print_error_str),
		buf == ਨਿਰਾ ? (char_u *)_("Unknown") : buf);
	LocalFree((LPVOID)(buf));
    }
    ਨਹੀਂਤਾਂ
	ਸੁਨੇਹਾ_clr_eos(); // Maybe canceled

    mch_print_cleanup();
    ਵਾਪਸ FALSE;
}


    ਪੂਰਨ
mch_print_begin(prt_settings_T *psettings)
{
    ਪੂਰਨ			ret = 0;
    ਅੱਖਰ		sz_ਬਫਰ[300];
    WCHAR		*wp;

    hDlgPrint = CreateDialog(g_hinst, ਪਾਠ_3("PrintDlgBox"),
					     prt_dlg.hwndOwner, PrintDlgProc);
    ਸੈੱਟ_Abort_Proc(prt_dlg.hDC, AbortProc);
    wsprintf(sz_ਬਫਰ, _("Printing '%s'"), gettail(psettings->jobname));
    vim_ਸੈੱਟ_Dlg_ਵਸਤੂ_ਪਾਠ(hDlgPrint, IDC_PRINTTEXT1, (char_u *)sz_ਬਫਰ);

    wp = enc_ਮੰਜ਼ਿਲ_utf16(psettings->jobname, ਨਿਰਾ);
    ਜੇ (wp != ਨਿਰਾ)
    {
	DOCINFOW	di;

	CLEAR_ਖੇਤਰ(di);
	di.cb_ਮਾਪ = ਆਕਾਰ(di);
	di.lpszDocName = wp;
	ret = ਸ਼ੁਰੂ_Doc_W(prt_dlg.hDC, &di);
	vim_free(wp);
    }

# ifdef FEAT_GUI
    // Give focus back to main window (when using MDI).
#  ifdef VIMDLL
    ਜੇ (gui.ਵਿੱਚ_use)
#  endif
	ਸੈੱਟ_Focus(s_hwnd);
# endif

    ਵਾਪਸ (ret > 0);
}

    ਖਾਲੀ
mch_print_ਸਿਰਾ(prt_settings_T *psettings ਅਣਵਰਤਿਆ)
{
    ਸਿਰਾ_Doc(prt_dlg.hDC);
    ਜੇ (!*bUserAbort && hDlgPrint != ਨਿਰਾ)
	Send_ਸੁਨੇਹਾ(hDlgPrint, WM_COMMAND, 0, 0);
}

    ਪੂਰਨ
mch_print_ਸਿਰਾ_ਪੰਨਾ(ਖਾਲੀ)
{
    ਵਾਪਸ (ਸਿਰਾ_ਪੰਨਾ(prt_dlg.hDC) > 0);
}

    ਪੂਰਨ
mch_print_begin_ਪੰਨਾ(char_u *ਸੁਨੇਹਾ)
{
    ਜੇ (ਸੁਨੇਹਾ != ਨਿਰਾ)
	vim_ਸੈੱਟ_Dlg_ਵਸਤੂ_ਪਾਠ(hDlgPrint, IDC_ਤਰੱਕੀ, ਸੁਨੇਹਾ);
    ਵਾਪਸ (ਸ਼ੁਰੂ_ਪੰਨਾ(prt_dlg.hDC) > 0);
}

    ਪੂਰਨ
mch_print_blank_ਪੰਨਾ(ਖਾਲੀ)
{
    ਵਾਪਸ (mch_print_begin_ਪੰਨਾ(ਨਿਰਾ) ? (mch_print_ਸਿਰਾ_ਪੰਨਾ()) : FALSE);
}

ਸਥਿਰ ਪੂਰਨ prt_pos_x = 0;
ਸਥਿਰ ਪੂਰਨ prt_pos_y = 0;

    ਖਾਲੀ
mch_print_ਸ਼ੁਰੂ_ਸਤਰ(ਪੂਰਨ margin, ਪੂਰਨ ਪੰਨਾ_ਸਤਰ)
{
    ਜੇ (margin)
	prt_pos_x = -prt_ਸੰਖਿਆ_ਚੌੜਾਈ;
    ਨਹੀਂਤਾਂ
	prt_pos_x = 0;
    prt_pos_y = ਪੰਨਾ_ਸਤਰ * prt_ਸਤਰ_ਉਚਾਈ
				 + prt_tm.tmAscent + prt_tm.tmExternalLeading;
}

    ਪੂਰਨ
mch_print_ਪਾਠ_ਬਾਹਰਲਾ(char_u *p, ਪੂਰਨ len)
{
    ਮਾਪ_3	sz;
    WCHAR	*wp;
    ਪੂਰਨ		wlen = len;
    ਪੂਰਨ		ret = FALSE;

    wp = enc_ਮੰਜ਼ਿਲ_utf16(p, &wlen);
    ਜੇ (wp == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    ਪਾਠ_ਬਾਹਰਲਾ_W(prt_dlg.hDC, prt_pos_x + prt_ਖੱਬਾ_margin,
	    prt_pos_y + prt_ਸਿਖਰ_margin, wp, wlen);
    Get_ਪਾਠ_Extent_Point32_W(prt_dlg.hDC, wp, wlen, &sz);
    vim_free(wp);
    prt_pos_x += (sz.cx - prt_tm.tmOverhang);
    // This is wrong when printing spaces for a TAB.
    ਜੇ (p[len] != NUL)
    {
	wlen = mb_ptr2len(p + len);
	wp = enc_ਮੰਜ਼ਿਲ_utf16(p + len, &wlen);
	ਜੇ (wp != ਨਿਰਾ)
	{
	    Get_ਪਾਠ_Extent_Point32_W(prt_dlg.hDC, wp, 1, &sz);
	    ret = (prt_pos_x + prt_ਖੱਬਾ_margin + sz.cx > prt_ਸੱਜਾ_margin);
	    vim_free(wp);
	}
    }
    ਵਾਪਸ ret;
}

    ਖਾਲੀ
mch_print_ਸੈੱਟ_font(ਪੂਰਨ iBold, ਪੂਰਨ iItalic, ਪੂਰਨ iUnderline)
{
    SelectObject(prt_dlg.hDC, prt_font_handles[iBold][iItalic][iUnderline]);
}

    ਖਾਲੀ
mch_print_ਸੈੱਟ_bg(long_u bgcol)
{
    ਸੈੱਟ_Bk_Color(prt_dlg.hDC, Get_ਸਭ_ਤੋਂ_ਨੇੜੇ_Color(prt_dlg.hDC,
						   ਅਦਲਾ_ਬਦਲੀ_me((COLORREF)bgcol)));
    /*
     * With a white background we can draw characters transparent, which is
     * good for italic characters that overlap to the next char cell.
     */
    ਜੇ (bgcol == 0xffffffUL)
	ਸੈੱਟ_Bk_ਬਹੁਲਕ(prt_dlg.hDC, TRANSPARENT);
    ਨਹੀਂਤਾਂ
	ਸੈੱਟ_Bk_ਬਹੁਲਕ(prt_dlg.hDC, OPAQUE);
}

    ਖਾਲੀ
mch_print_ਸੈੱਟ_fg(long_u fgcol)
{
    ਸੈੱਟ_ਪਾਠ_Color(prt_dlg.hDC, Get_ਸਭ_ਤੋਂ_ਨੇੜੇ_Color(prt_dlg.hDC,
						   ਅਦਲਾ_ਬਦਲੀ_me((COLORREF)fgcol)));
}

#ਜੇ_ਅੰਤ // FEAT_PRINTER && !FEAT_POSTSCRIPT



#ਜੇ defined(FEAT_SHORTCUT)
# include <shlobj.h>

# define is_ਰਸਤਾ_sep(c)	    ((c) == L'\\' || (c) == L'/')

    ਸਥਿਰ ਪੂਰਨ
is_reparse_ਬਿੰਦੂ_included(LPCWSTR fname)
{
    LPCWSTR	p = fname, q;
    WCHAR	buf[MAX_ਰਸਤਾ];
    DWORD	attr;

    ਜੇ (ਸੁਰੱਖਿਅਤ_isalpha(p[0]) && p[1] == L':' && is_ਰਸਤਾ_sep(p[2]))
	p += 3;
    ਨਹੀਂਤਾਂ ਜੇ (is_ਰਸਤਾ_sep(p[0]) && is_ਰਸਤਾ_sep(p[1]))
	p += 2;

    ਜਦੋਂ (*p != L'\0')
    {
	q = wcspbrk(p, L"\\/");
	ਜੇ (q == ਨਿਰਾ)
	    p = q = fname + wcslen(fname);
	ਨਹੀਂਤਾਂ
	    p = q + 1;
	ਜੇ (q - fname >= MAX_ਰਸਤਾ)
	    ਵਾਪਸ FALSE;
	wcsncpy(buf, fname, q - fname);
	buf[q - fname] = L'\0';
	attr = Get_File_ਗੁਣ_W(buf);
	ਜੇ (attr != INVALID_FILE_ਗੁਣ
		&& (attr & FILE_ATTRIBUTE_REPARSE_ਬਿੰਦੂ) != 0)
	    ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Return the resolved file path, NULL if "fname" is an AppExecLink reparse
 * point, already fully resolved, or it doesn't exists.
 */
    char_u *
resolve_reparse_ਬਿੰਦੂ(char_u *fname)
{
    HANDLE	    h = INVALID_HANDLE_ਮੁੱਲ;
    DWORD	    ਮਾਪ;
    WCHAR	    *p, *wp;
    char_u	    *rfname = ਨਿਰਾ;
    WCHAR	    *buff = ਨਿਰਾ;

    p = enc_ਮੰਜ਼ਿਲ_utf16(fname, ਨਿਰਾ);
    ਜੇ (p == ਨਿਰਾ)
	ਜਾਓ fail;

    ਜੇ (!is_reparse_ਬਿੰਦੂ_included(p))
    {
	vim_free(p);
	ਜਾਓ fail;
    }

    h = CreateFileW(p, 0, 0, ਨਿਰਾ, OPEN_EXISTING,
	    FILE_FLAG_BACKUP_SEMANTICS, ਨਿਰਾ);
    vim_free(p);

    ਜੇ (h == INVALID_HANDLE_ਮੁੱਲ)
	ਜਾਓ fail;

    ਮਾਪ = Get_Final_ਰਸਤਾ_Name_By_Handle_W(h, ਨਿਰਾ, 0, 0);
    ਜੇ (ਮਾਪ == 0)
	ਜਾਓ fail;
    buff = ALLOC_MULT(WCHAR, ਮਾਪ);
    ਜੇ (buff == ਨਿਰਾ)
	ਜਾਓ fail;
    ਜੇ (Get_Final_ਰਸਤਾ_Name_By_Handle_W(h, buff, ਮਾਪ, 0) == 0)
	ਜਾਓ fail;

    ਜੇ (wcsncmp(buff, L"\\\\?\\UNC\\", 8) == 0)
    {
	buff[6] = L'\\';
	wp = buff + 6;
    }
    ਨਹੀਂਤਾਂ ਜੇ (wcsncmp(buff, L"\\\\?\\", 4) == 0)
	wp = buff + 4;
    ਨਹੀਂਤਾਂ
	wp = buff;

    rfname = utf16_ਮੰਜ਼ਿਲ_enc(wp, ਨਿਰਾ);

fail:
    ਜੇ (h != INVALID_HANDLE_ਮੁੱਲ)
	CloseHandle(h);
    ਜੇ (buff != ਨਿਰਾ)
	vim_free(buff);

    ਵਾਪਸ rfname;
}

/*
 * When "fname" is the name of a shortcut (*.lnk) resolve the file it points
 * to and return that name in allocated memory.
 * Otherwise NULL is returned.
 */
    ਸਥਿਰ char_u *
resolve_shortcut(char_u *fname)
{
    HRESULT		hr;
    IShellLink		*psl = ਨਿਰਾ;
    IPersistFile	*ppf = ਨਿਰਾ;
    OLECHAR		wsz[MAX_ਰਸਤਾ];
    char_u		*rfname = ਨਿਰਾ;
    ਪੂਰਨ			len;
    IShellLinkW		*pslw = ਨਿਰਾ;
    WIN32_ਲੱਭੋ_DATAW	ffdw; // we get those free of charge

    // Check if the file name ends in ".lnk". Avoid calling
    // CoCreateInstance(), it's quite slow.
    ਜੇ (fname == ਨਿਰਾ)
	ਵਾਪਸ rfname;
    len = (ਪੂਰਨ)STRLEN(fname);
    ਜੇ (len <= 4 || STRNICMP(fname + len - 4, ".lnk", 4) != 0)
	ਵਾਪਸ rfname;

    CoInitialize(ਨਿਰਾ);

    // create a link manager object and request its interface
    hr = CoCreateInstance(
	    &CLSID_ShellLink, ਨਿਰਾ, CLSCTX_INPROC_SERVER,
	    &IID_IShellLinkW, (ਖਾਲੀ**)&pslw);
    ਜੇ (hr == S_OK)
    {
	WCHAR	*p = enc_ਮੰਜ਼ਿਲ_utf16(fname, ਨਿਰਾ);

	ਜੇ (p != ਨਿਰਾ)
	{
	    // Get a pointer to the IPersistFile interface.
	    hr = pslw->lpVtbl->QueryInterface(
		    pslw, &IID_IPersistFile, (ਖਾਲੀ**)&ppf);
	    ਜੇ (hr != S_OK)
		ਜਾਓ shortcut_errorw;

	    // "load" the name and resolve the link
	    hr = ppf->lpVtbl->ਲੱਦੋ_2(ppf, p, STGM_READ);
	    ਜੇ (hr != S_OK)
		ਜਾਓ shortcut_errorw;
# ਜੇ 0  // This makes Vim wait a long time if the target does not exist.
	    hr = pslw->lpVtbl->Resolve(pslw, ਨਿਰਾ, SLR_NO_UI);
	    ਜੇ (hr != S_OK)
		ਜਾਓ shortcut_errorw;
# endif

	    // Get the path to the link target.
	    Zero_ਸਮ੍ਰਿਤੀ(wsz, MAX_ਰਸਤਾ * ਆਕਾਰ(WCHAR));
	    hr = pslw->lpVtbl->Get_ਰਸਤਾ(pslw, wsz, MAX_ਰਸਤਾ, &ffdw, 0);
	    ਜੇ (hr == S_OK && wsz[0] != NUL)
		rfname = utf16_ਮੰਜ਼ਿਲ_enc(wsz, ਨਿਰਾ);

shortcut_errorw:
	    vim_free(p);
	}
    }

    // Release all interface pointers (both belong to the same object)
    ਜੇ (ppf != ਨਿਰਾ)
	ppf->lpVtbl->Release(ppf);
    ਜੇ (psl != ਨਿਰਾ)
	psl->lpVtbl->Release(psl);
    ਜੇ (pslw != ਨਿਰਾ)
	pslw->lpVtbl->Release(pslw);

    CoUninitialize();
    ਵਾਪਸ rfname;
}

    char_u *
mch_resolve_ਰਸਤਾ(char_u *fname, ਪੂਰਨ reparse_ਬਿੰਦੂ)
{
    char_u  *ਰਸਤਾ = resolve_shortcut(fname);

    ਜੇ (ਰਸਤਾ == ਨਿਰਾ && reparse_ਬਿੰਦੂ)
	ਰਸਤਾ = resolve_reparse_ਬਿੰਦੂ(fname);
    ਵਾਪਸ ਰਸਤਾ;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_EVAL) && (!defined(FEAT_GUI) || defined(VIMDLL))
/*
 * Bring ourselves to the foreground.  Does work if the OS doesn't allow it.
 */
    ਖਾਲੀ
win32_ਸੈੱਟ_foreground(ਖਾਲੀ)
{
    GetConsoleHwnd();	    // get value of s_hwnd
    ਜੇ (s_hwnd != 0)
	ਸੈੱਟ_Foreground_ਖਿੜਕੀ(s_hwnd);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_CLIENTSERVER)
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
HWND ਸੁਨੇਹਾ_ਖਿੜਕੀ = 0;	    // window that's handling messages

# define VIM_CLASSNAME      "VIM_MESSAGES"
# define VIM_CLASSNAME_LEN  (ਆਕਾਰ(VIM_CLASSNAME) - 1)

// Timeout for sending a message to another Vim instance.  Normally this works
// instantly, but it may hang when the other Vim instance is halted.
# define SENDMESSAGE_TIMEOUT	(5 * 1000)

// Communication is via WM_COPYDATA messages. The message type is sent in
// the dwData parameter. Types are defined here.
# define COPYDATA_ਕੁੰਜੀਆਂ		0
# define COPYDATA_ਜਵਾਬ		1
# define COPYDATA_ਵਿਅੰਜਕ		10
# define COPYDATA_ਨਤੀਜਾ	11
# define COPYDATA_ERROR_ਨਤੀਜਾ	12
# define COPYDATA_ENCODING	20

// This is a structure containing a server HWND and its name.
ਬਣਤਰ server_id
{
    HWND hwnd;
    char_u *name;
};

// Last received 'encoding' that the client uses.
ਸਥਿਰ char_u	*client_enc = ਨਿਰਾ;

/*
 * Tell the other side what encoding we are using.
 * Return -1 if timeout happens.  Other errors are ignored.
 */
    ਸਥਿਰ ਪੂਰਨ
serverSendEnc(HWND ਨਿਸ਼ਾਨਾ)
{
    COPYDATASTRUCT ਅੰਕੜੇ;

    ਅੰਕੜੇ.dw_ਅੰਕੜੇ = COPYDATA_ENCODING;
    ਅੰਕੜੇ.cb_ਅੰਕੜੇ = (DWORD)STRLEN(p_enc) + 1;
    ਅੰਕੜੇ.lp_ਅੰਕੜੇ = p_enc;
    ਜੇ (Send_ਸੁਨੇਹਾ_Timeout(ਨਿਸ਼ਾਨਾ, WM_COPYDATA,
	    (WPARAM)ਸੁਨੇਹਾ_ਖਿੜਕੀ, (LPARAM)&ਅੰਕੜੇ,
	    SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, ਨਿਰਾ) == 0)
	ਵਾਪਸ -1;
    ਵਾਪਸ 0;
}

/*
 * Clean up on exit. This destroys the hidden message window.
 */
    ਸਥਿਰ ਖਾਲੀ
Clean_ਉੱਪਰ_Messaging(ਖਾਲੀ)
{
    ਜੇ (ਸੁਨੇਹਾ_ਖਿੜਕੀ == 0)
	ਵਾਪਸ;

    Destroy_ਖਿੜਕੀ(ਸੁਨੇਹਾ_ਖਿੜਕੀ);
    ਸੁਨੇਹਾ_ਖਿੜਕੀ = 0;
}

ਸਥਿਰ ਪੂਰਨ save_ਜਵਾਬ(HWND server, char_u *ਜਵਾਬ, ਪੂਰਨ ਵਿਅੰਜਕ);

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
    ਸਥਿਰ LRESULT CALLBACK
Messaging_WndProc(HWND hwnd, UINT ਸੁਨੇਹਾ, WPARAM wParam, LPARAM lParam)
{
    ਜੇ (ਸੁਨੇਹਾ == WM_COPYDATA)
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
	COPYDATASTRUCT	*ਅੰਕੜੇ = (COPYDATASTRUCT*)lParam;
	HWND		sender = (HWND)wParam;
	COPYDATASTRUCT	ਜਵਾਬ;
	char_u		*res;
	ਪੂਰਨ		retval;
	DWORD_PTR	dwret = 0;
	char_u		*str;
	char_u		*tofree;

	ਚੋਣ (ਅੰਕੜੇ->dw_ਅੰਕੜੇ)
	{
	ਹਾਲਤ COPYDATA_ENCODING:
	    // Remember the encoding that the client uses.
	    vim_free(client_enc);
	    client_enc = enc_canonize((char_u *)ਅੰਕੜੇ->lp_ਅੰਕੜੇ);
	    ਵਾਪਸ 1;

	ਹਾਲਤ COPYDATA_ਕੁੰਜੀਆਂ:
	    // Remember who sent this, for <client>
	    client_ਖਿੜਕੀ = sender;

	    // Add the received keys to the input buffer.
	    str = serverConvert(client_enc, (char_u *)ਅੰਕੜੇ->lp_ਅੰਕੜੇ, &tofree);
# ifdef FEAT_GUI
	    ਜੇ (gui.ਵਿੱਚ_use)
		// The GUI may get here re-entrantly, so insert the keys later.
		server_ਜੋੜੋ_ਨਿਵੇਸ਼(str);
	    ਨਹੀਂਤਾਂ
# endif
		server_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(str);
	    vim_free(tofree);

# ifdef FEAT_GUI
	    // Wake up the main GUI loop.
#  ifdef VIMDLL
	    ਜੇ (gui.ਵਿੱਚ_use)
#  endif
		ਜੇ (s_hwnd != 0)
		    Post_ਸੁਨੇਹਾ(s_hwnd, WM_NULL, 0, 0);
# endif
	    ਵਾਪਸ 1;

	ਹਾਲਤ COPYDATA_ਵਿਅੰਜਕ:
	    // Remember who sent this, for <client>
	    client_ਖਿੜਕੀ = sender;

	    str = serverConvert(client_enc, (char_u *)ਅੰਕੜੇ->lp_ਅੰਕੜੇ, &tofree);
	    res = eval_client_ਵਿਅੰਜਕ_ਮੰਜ਼ਿਲ_string(str);

	    ਜੇ (res == ਨਿਰਾ)
	    {
		ਅੱਖਰ	*err = _(e_invalid_ਵਿਅੰਜਕ_received);
		size_t	len = STRLEN(str) + STRLEN(err) + 5;

		res = alloc(len);
		ਜੇ (res != ਨਿਰਾ)
		    ਜਵਾਬ.cb_ਅੰਕੜੇ = (DWORD)vim_snprintf_safelen(
			(ਅੱਖਰ *)res, len, "%s: \"%s\"", err, str);
		ਨਹੀਂਤਾਂ
		    ਜਵਾਬ.cb_ਅੰਕੜੇ = 0;
		ਜਵਾਬ.dw_ਅੰਕੜੇ = COPYDATA_ERROR_ਨਤੀਜਾ;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜਵਾਬ.cb_ਅੰਕੜੇ = (DWORD)STRLEN(res) + 1;
		ਜਵਾਬ.dw_ਅੰਕੜੇ = COPYDATA_ਨਤੀਜਾ;
	    }
	    ਜਵਾਬ.lp_ਅੰਕੜੇ = res;

	    ਜੇ (serverSendEnc(sender) < 0)
		retval = -1;
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (Send_ਸੁਨੇਹਾ_Timeout(sender, WM_COPYDATA,
			(WPARAM)ਸੁਨੇਹਾ_ਖਿੜਕੀ, (LPARAM)&ਜਵਾਬ,
			SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
		    retval = -1;
		ਨਹੀਂਤਾਂ
		    retval = (ਪੂਰਨ)dwret;
	    }
	    vim_free(tofree);
	    vim_free(res);
	    ਵਾਪਸ retval;

	ਹਾਲਤ COPYDATA_ਜਵਾਬ:
	ਹਾਲਤ COPYDATA_ਨਤੀਜਾ:
	ਹਾਲਤ COPYDATA_ERROR_ਨਤੀਜਾ:
	    ਜੇ (ਅੰਕੜੇ->lp_ਅੰਕੜੇ != ਨਿਰਾ)
	    {
		str = serverConvert(client_enc, (char_u *)ਅੰਕੜੇ->lp_ਅੰਕੜੇ,
								     &tofree);
		ਜੇ (tofree == ਨਿਰਾ)
		    str = vim_strsave(str);
		ਜੇ (save_ਜਵਾਬ(sender, str,
			   (ਅੰਕੜੇ->dw_ਅੰਕੜੇ == COPYDATA_ਜਵਾਬ ?  0 :
			   (ਅੰਕੜੇ->dw_ਅੰਕੜੇ == COPYDATA_ਨਤੀਜਾ ? 1 :
							      2))) == FAIL)
		    vim_free(str);
		ਨਹੀਂਤਾਂ ਜੇ (ਅੰਕੜੇ->dw_ਅੰਕੜੇ == COPYDATA_ਜਵਾਬ)
		{
		    char_u	winstr[30];

		    sprintf((ਅੱਖਰ *)winstr, PRINTF_HEX_LONG_U, (long_u)sender);
		    apply_autocmds(EVENT_REMOTEREPLY, winstr, str,
								TRUE, curbuf);
		}
	    }
	    ਵਾਪਸ 1;
	}

	ਵਾਪਸ 0;
    }

    ਨਹੀਂਤਾਂ ਜੇ (ਸੁਨੇਹਾ == WM_ACTIVATE && wParam == WA_ਸਰਗਰਮ)
    {
	// When the message window is activated (brought to the foreground),
	// this actually applies to the text window.
# ਜੇ !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	ਜੇ (!gui.ਵਿੱਚ_use)
#  endif
	    GetConsoleHwnd();	    // get value of s_hwnd
# endif
	ਜੇ (s_hwnd != 0)
	{
	    ਸੈੱਟ_Foreground_ਖਿੜਕੀ(s_hwnd);
	    ਵਾਪਸ 0;
	}
    }

    ਵਾਪਸ Def_ਖਿੜਕੀ_Proc(hwnd, ਸੁਨੇਹਾ, wParam, lParam);
}

/*
 * Initialise the message handling process.  This involves creating a window
 * to handle messages - the window will not be visible.
 */
    ਖਾਲੀ
serverInitMessaging(ਖਾਲੀ)
{
    WNDCLASS wndclass;

    // Clean up on exit
    atexit(Clean_ਉੱਪਰ_Messaging);

    // Register a window class - we only really care
    // about the window procedure
    wndclass.style = 0;
    wndclass.lpfnWndProc = Messaging_WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = g_hinst;
    wndclass.hIcon = ਨਿਰਾ;
    wndclass.hCursor = ਨਿਰਾ;
    wndclass.hbrBackground = ਨਿਰਾ;
    wndclass.lpszMenuName = ਨਿਰਾ;
    wndclass.lpszClassName = VIM_CLASSNAME;
    ਰਜਿਸਟਰ_Class(&wndclass);

    // Create the message window. It will be hidden, so the details don't
    // matter.  Don't use WS_OVERLAPPEDWINDOW, it will make a shortcut remove
    // focus from gvim.
    ਸੁਨੇਹਾ_ਖਿੜਕੀ = Create_ਖਿੜਕੀ(VIM_CLASSNAME, "",
			 WS_POPUPWINDOW | WS_CAPTION,
			 CW_USEDEFAULT, CW_USEDEFAULT,
			 100, 100, ਨਿਰਾ, ਨਿਰਾ,
			 g_hinst, ਨਿਰਾ);
}

// Used by serverSendToVim() to find an alternate server name.
ਸਥਿਰ char_u *altname_buf_ptr = ਨਿਰਾ;

/*
 * Get the title of the window "hwnd", which is the Vim server name, in
 * "name[namelen]" and return the length.
 * Returns zero if window "hwnd" is not a Vim server.
 */
    ਸਥਿਰ ਪੂਰਨ
getVimServerName(HWND hwnd, ਅੱਖਰ *name, ਪੂਰਨ namelen)
{
    ਪੂਰਨ		len;
    ਅੱਖਰ	ਬਫਰ[VIM_CLASSNAME_LEN + 1];

    // Ignore windows which aren't Vim message windows
    len = GetClassName(hwnd, ਬਫਰ, ਆਕਾਰ(ਬਫਰ));
    ਜੇ (len != VIM_CLASSNAME_LEN || STRCMP(ਬਫਰ, VIM_CLASSNAME) != 0)
	ਵਾਪਸ 0;

    // Get the title of the window
    ਵਾਪਸ Get_ਖਿੜਕੀ_ਪਾਠ(hwnd, name, namelen);
}

    ਸਥਿਰ BOOL CALLBACK
enumWindowsGetServer(HWND hwnd, LPARAM lparam)
{
    ਬਣਤਰ	server_id *id = (ਬਣਤਰ server_id *)lparam;
    ਅੱਖਰ	server[MAX_ਰਸਤਾ];

    // Get the title of the window
    ਜੇ (getVimServerName(hwnd, server, ਆਕਾਰ(server)) == 0)
	ਵਾਪਸ TRUE;

    // If this is the server we're looking for, return its HWND
    ਜੇ (STRICMP(server, id->name) == 0)
    {
	id->hwnd = hwnd;
	ਵਾਪਸ FALSE;
    }

    // If we are looking for an alternate server, remember this name.
    ਜੇ (altname_buf_ptr != ਨਿਰਾ)
    {
	size_t	namelen = STRLEN(id->name);

	ਜੇ (STRNICMP(server, id->name, namelen) == 0
	    && vim_isdigit(server[namelen]))
	{
	    STRCPY(altname_buf_ptr, server);
	    altname_buf_ptr = ਨਿਰਾ;	    // don't use another name
	}
    }

    // Otherwise, keep looking
    ਵਾਪਸ TRUE;
}

    ਸਥਿਰ BOOL CALLBACK
enum_Windows_Get_ਨਾਂ(HWND hwnd, LPARAM lparam)
{
    list_T	*list = (list_T *)lparam;
    ਅੱਖਰ	server[MAX_ਰਸਤਾ];

    // Get the title of the window
    ਜੇ (getVimServerName(hwnd, server, ਆਕਾਰ(server)) == 0)
	ਵਾਪਸ TRUE;

    list_append_string(list, (char_u *)server, -1);
    ਵਾਪਸ TRUE;
}

ਬਣਤਰ enum_windows_s
{
    WNDENUMPROC lpEnumFunc;
    LPARAM      lParam;
};

    ਸਥਿਰ BOOL CALLBACK
enum_windows_ਬੱਚਾ(HWND hwnd, LPARAM lParam)
{
    ਬਣਤਰ enum_windows_s *ew = (ਬਣਤਰ enum_windows_s *)lParam;

    ਵਾਪਸ (ew->lpEnumFunc)(hwnd, ew->lParam);
}

    ਸਥਿਰ BOOL CALLBACK
enum_windows_toplevel(HWND hwnd, LPARAM lParam)
{
    ਬਣਤਰ enum_windows_s *ew = (ਬਣਤਰ enum_windows_s *)lParam;

    ਜੇ ((ew->lpEnumFunc)(hwnd, ew->lParam))
	ਵਾਪਸ TRUE;
    ਵਾਪਸ Enum_ਬੱਚਾ_Windows(hwnd, enum_windows_ਬੱਚਾ, lParam);
}

/*
 * Enumerate all windows including children.
 */
    ਸਥਿਰ BOOL
enum_windows(WNDENUMPROC lpEnumFunc, LPARAM lParam)
{
    ਬਣਤਰ enum_windows_s ew;

    ew.lpEnumFunc = lpEnumFunc;
    ew.lParam = lParam;
    ਵਾਪਸ EnumWindows(enum_windows_toplevel, (LPARAM)&ew);
}

    ਸਥਿਰ HWND
ਲੱਭੋ_Server(char_u *name)
{
    ਬਣਤਰ server_id id;

    id.name = name;
    id.hwnd = 0;

    enum_windows(enumWindowsGetServer, (LPARAM)(&id));

    ਵਾਪਸ id.hwnd;
}

    ਖਾਲੀ
server_ਸੈੱਟ_Name(char_u *name)
{
    size_t	namelen;
    char_u	*ok_name;
    HWND	hwnd = 0;
    ਪੂਰਨ		i = 0;
    char_u	*p;

    // Leave enough space for a 9-digit suffix to ensure uniqueness!
    namelen = STRLEN(name);
    ok_name = alloc(namelen + 10);
    ਜੇ (ok_name == ਨਿਰਾ)
	ਵਾਪਸ;

    STRCPY(ok_name, name);
    p = ok_name + namelen;

    ਲਈ (;;)
    {
	// This is inefficient - we're doing an EnumWindows loop for each
	// possible name. It would be better to grab all names in one go,
	// and scan the list each time...
	hwnd = ਲੱਭੋ_Server(ok_name);
	ਜੇ (hwnd == 0)
	    ਰੋਕੋ;

	++i;
	ਜੇ (i >= 1000)
	    ਰੋਕੋ;

	sprintf((ਅੱਖਰ *)p, "%d", i);
    }

    ਜੇ (hwnd != 0)
	vim_free(ok_name);
    ਨਹੀਂਤਾਂ
    {
	// Remember the name
	serverName = ok_name;
	need_maketitle = TRUE;	// update Vim window title later

	// Update the message window title
	ਸੈੱਟ_ਖਿੜਕੀ_ਪਾਠ(ਸੁਨੇਹਾ_ਖਿੜਕੀ, (LPCSTR)ok_name);

# ifdef FEAT_EVAL
	// Set the servername variable
	ਸੈੱਟ_vim_var_string(VV_SEND_SERVER, serverName, -1);
# endif
    }
}

    list_T *
server_Get_Vim_ਨਾਂ(ਖਾਲੀ)
{
    list_T *list = list_alloc();

    ਜੇ (list == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    enum_windows(enum_Windows_Get_ਨਾਂ, (LPARAM)list);

    ਵਾਪਸ list;
}

    ਪੂਰਨ
server_Send_ਜਵਾਬ(
    char_u	*name,		// Where to send.
    char_u	*ਜਵਾਬ)		// What to send.
{
    HWND	ਨਿਸ਼ਾਨਾ;
    COPYDATASTRUCT ਅੰਕੜੇ;
    long_u	n = 0;
    DWORD_PTR	dwret = 0;

    // The "name" argument is a magic cookie obtained from expand("<client>").
    // It should be of the form 0xXXXXX - i.e. a C hex literal, which is the
    // value of the client's message window HWND.
    ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ((ਅੱਖਰ *)name, SCANF_HEX_LONG_U, &n);
    ਜੇ (n == 0)
	ਵਾਪਸ -1;

    ਨਿਸ਼ਾਨਾ = (HWND)n;
    ਜੇ (!Is_ਖਿੜਕੀ(ਨਿਸ਼ਾਨਾ))
	ਵਾਪਸ -1;

    ਅੰਕੜੇ.dw_ਅੰਕੜੇ = COPYDATA_ਜਵਾਬ;
    ਅੰਕੜੇ.cb_ਅੰਕੜੇ = (DWORD)STRLEN(ਜਵਾਬ) + 1;
    ਅੰਕੜੇ.lp_ਅੰਕੜੇ = ਜਵਾਬ;

    ਜੇ (serverSendEnc(ਨਿਸ਼ਾਨਾ) < 0)
	ਵਾਪਸ -1;
    ਜੇ (Send_ਸੁਨੇਹਾ_Timeout(ਨਿਸ਼ਾਨਾ, WM_COPYDATA,
		(WPARAM)ਸੁਨੇਹਾ_ਖਿੜਕੀ, (LPARAM)&ਅੰਕੜੇ,
		SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
	ਵਾਪਸ -1;
    ਵਾਪਸ dwret ? 0 : -1;
}

    ਪੂਰਨ
server_Send_ਮੰਜ਼ਿਲ_Vim(
    char_u	 *name,			// Where to send.
    char_u	 *cmd,			// What to send.
    char_u	 **ਨਤੀਜਾ,		// Result of eval'ed expression
    ਖਾਲੀ	 *ptarget,		// HWND of server
    ਪੂਰਨ		 as_ਵਿਅੰਜਕ,		// Expression or keys?
    ਪੂਰਨ		 timeout,		// timeout in seconds or zero
    ਪੂਰਨ		 silent)		// don't complain about no server
{
    size_t	namelen;
    HWND	ਨਿਸ਼ਾਨਾ;
    COPYDATASTRUCT ਅੰਕੜੇ;
    char_u	*retval = ਨਿਰਾ;
    ਪੂਰਨ		retcode = 0;
    DWORD_PTR	dwret = 0;
    char_u	altname_buf[MAX_ਰਸਤਾ];

    // Execute locally if no display or target is ourselves
    ਜੇ (serverName != ਨਿਰਾ && STRICMP(name, serverName) == 0)
	ਵਾਪਸ send_ਮੰਜ਼ਿਲ_Local_Vim(cmd, as_ਵਿਅੰਜਕ, ਨਤੀਜਾ);

    // If the server name does not end in a digit then we look for an
    // alternate name.  e.g. when "name" is GVIM then we may find GVIM2.
    namelen = STRLEN(name);
    ਜੇ (namelen > 1 && !vim_isdigit(name[namelen - 1]))
	altname_buf_ptr = altname_buf;
    altname_buf[0] = NUL;
    ਨਿਸ਼ਾਨਾ = ਲੱਭੋ_Server(name);
    altname_buf_ptr = ਨਿਰਾ;
    ਜੇ (ਨਿਸ਼ਾਨਾ == 0 && altname_buf[0] != NUL)
	// Use another server name we found.
	ਨਿਸ਼ਾਨਾ = ਲੱਭੋ_Server(altname_buf);

    ਜੇ (ਨਿਸ਼ਾਨਾ == 0)
    {
	ਜੇ (!silent)
	    semsg(_(e_no_registered_server_named_str), name);
	ਵਾਪਸ -1;
    }

    ਜੇ (ptarget)
	*(HWND *)ptarget = ਨਿਸ਼ਾਨਾ;

    ਅੰਕੜੇ.dw_ਅੰਕੜੇ = as_ਵਿਅੰਜਕ ? COPYDATA_ਵਿਅੰਜਕ : COPYDATA_ਕੁੰਜੀਆਂ;
    ਅੰਕੜੇ.cb_ਅੰਕੜੇ = (DWORD)STRLEN(cmd) + 1;
    ਅੰਕੜੇ.lp_ਅੰਕੜੇ = cmd;

    ਜੇ (serverSendEnc(ਨਿਸ਼ਾਨਾ) < 0)
	ਵਾਪਸ -1;
    ਜੇ (Send_ਸੁਨੇਹਾ_Timeout(ਨਿਸ਼ਾਨਾ, WM_COPYDATA,
		(WPARAM)ਸੁਨੇਹਾ_ਖਿੜਕੀ, (LPARAM)&ਅੰਕੜੇ,
		SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
	ਵਾਪਸ -1;
    ਜੇ (dwret == 0)
	ਵਾਪਸ -1;

    ਜੇ (as_ਵਿਅੰਜਕ)
	retval = server_Get_ਜਵਾਬ(ਨਿਸ਼ਾਨਾ, &retcode, TRUE, TRUE, timeout);

    ਜੇ (ਨਤੀਜਾ == ਨਿਰਾ)
	vim_free(retval);
    ਨਹੀਂਤਾਂ
	*ਨਤੀਜਾ = retval; // Caller assumes responsibility for freeing

    ਵਾਪਸ retcode;
}

/*
 * Bring the server to the foreground.
 */
    ਖਾਲੀ
serverForeground(char_u *name)
{
    HWND	ਨਿਸ਼ਾਨਾ = ਲੱਭੋ_Server(name);

    ਜੇ (ਨਿਸ਼ਾਨਾ != 0)
	ਸੈੱਟ_Foreground_ਖਿੜਕੀ(ਨਿਸ਼ਾਨਾ);
}

// Replies from server need to be stored until the client picks them up via
// remote_read(). So we maintain a list of server-id/reply pairs.
// Note that there could be multiple replies from one server pending if the
// client is slow picking them up.
// We just store the replies in a simple list. When we remove an entry, we
// move list entries down to fill the gap.
// The server ID is simply the HWND.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    HWND	server;		// server window
    char_u	*ਜਵਾਬ;		// reply string
    ਪੂਰਨ		ਵਿਅੰਜਕ_ਨਤੀਜਾ;	// 0 for REPLY, 1 for RESULT 2 for error
} ਜਵਾਬ_T;

ਸਥਿਰ garray_T ਜਵਾਬ_list = {0, 0, ਆਕਾਰ(ਜਵਾਬ_T), 5, 0};

# define ਜਵਾਬ_ਵਸਤੂ(i) ((ਜਵਾਬ_T *)(ਜਵਾਬ_list.ga_ਅੰਕੜੇ) + (i))
# define ਜਵਾਬ_ਗਿਣਤੀ (ਜਵਾਬ_list.ga_len)

// Flag which is used to wait for a reply
ਸਥਿਰ ਪੂਰਨ ਜਵਾਬ_received = 0;

/*
 * Store a reply.  "reply" must be allocated memory (or NULL).
 */
    ਸਥਿਰ ਪੂਰਨ
save_ਜਵਾਬ(HWND server, char_u *ਜਵਾਬ, ਪੂਰਨ ਵਿਅੰਜਕ)
{
    ਜਵਾਬ_T *rep;

    ਜੇ (ga_grow(&ਜਵਾਬ_list, 1) == FAIL)
	ਵਾਪਸ FAIL;

    rep = ਜਵਾਬ_ਵਸਤੂ(ਜਵਾਬ_ਗਿਣਤੀ);
    rep->server = server;
    rep->ਜਵਾਬ = ਜਵਾਬ;
    rep->ਵਿਅੰਜਕ_ਨਤੀਜਾ = ਵਿਅੰਜਕ;
    ਜੇ (rep->ਜਵਾਬ == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ++ਜਵਾਬ_ਗਿਣਤੀ;
    ਜਵਾਬ_received = 1;
    ਵਾਪਸ OK;
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
server_Get_ਜਵਾਬ(HWND server, ਪੂਰਨ *ਵਿਅੰਜਕ_res, ਪੂਰਨ remove, ਪੂਰਨ wait, ਪੂਰਨ timeout)
{
    ਪੂਰਨ		i;
    char_u	*ਜਵਾਬ;
    ਜਵਾਬ_T	*rep;
    ਪੂਰਨ		did_process = FALSE;
    time_t	ਸ਼ੁਰੂ;
    time_t	now;

    // When waiting, loop until the message waiting for is received.
    ਸਮਾਂ(&ਸ਼ੁਰੂ);
    ਲਈ (;;)
    {
	// Reset this here, in case a message arrives while we are going
	// through the already received messages.
	ਜਵਾਬ_received = 0;

	ਲਈ (i = 0; i < ਜਵਾਬ_ਗਿਣਤੀ; ++i)
	{
	    rep = ਜਵਾਬ_ਵਸਤੂ(i);
	    ਜੇ (rep->server == server
		    && ((rep->ਵਿਅੰਜਕ_ਨਤੀਜਾ != 0) == (ਵਿਅੰਜਕ_res != ਨਿਰਾ)))
	    {
		// Save the values we've found for later
		ਜਵਾਬ = rep->ਜਵਾਬ;
		ਜੇ (ਵਿਅੰਜਕ_res != ਨਿਰਾ)
		    *ਵਿਅੰਜਕ_res = rep->ਵਿਅੰਜਕ_ਨਤੀਜਾ == 1 ? 0 : -1;

		ਜੇ (remove)
		{
		    // Move the rest of the list down to fill the gap
		    mch_memmove(rep, rep + 1,
				     (ਜਵਾਬ_ਗਿਣਤੀ - i - 1) * ਆਕਾਰ(ਜਵਾਬ_T));
		    --ਜਵਾਬ_ਗਿਣਤੀ;
		}

		// Return the reply to the caller, who takes on responsibility
		// for freeing it if "remove" is TRUE.
		ਵਾਪਸ ਜਵਾਬ;
	    }
	}

	// If we got here, we didn't find a reply. Return immediately if the
	// "wait" parameter isn't set.
	ਜੇ (!wait)
	{
	    // Process pending messages once. Without this, looping on
	    // remote_peek() would never get the reply.
	    ਜੇ (!did_process)
	    {
		did_process = TRUE;
		serverProcessPendingMessages();
		ਜਾਰੀ;
	    }
	    ਰੋਕੋ;
	}

	// We need to wait for a reply. Enter a message loop until the
	// "reply_received" flag gets set.

	// Loop until we receive a reply
	ਜਦੋਂ (ਜਵਾਬ_received == 0)
	{
# ifdef FEAT_TIMERS
	    // TODO: use the return value to decide how long to wait.
	    check_due_timer();
# endif
	    ਸਮਾਂ(&now);
	    ਜੇ (timeout > 0 && (now - ਸ਼ੁਰੂ) >= timeout)
		ਰੋਕੋ;

	    // Wait for a SendMessage() call to us.  This could be the reply
	    // we are waiting for.  Use a timeout of a second, to catch the
	    // situation that the server died unexpectedly.
	    ਸੁਨੇਹਾ_Wait_For_ਗੁਣਜ_Objects(0, ਨਿਰਾ, TRUE, 1000, QS_ALLINPUT);

	    // If the server has died, give up
	    ਜੇ (!Is_ਖਿੜਕੀ(server))
		ਵਾਪਸ ਨਿਰਾ;

	    serverProcessPendingMessages();
	}
    }

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Process any messages in the Windows message queue.
 */
    ਖਾਲੀ
serverProcessPendingMessages(ਖਾਲੀ)
{
    ਸੁਨੇਹਾ_3 ਸੁਨੇਹਾ;

    ਜਦੋਂ (Peek_ਸੁਨੇਹਾ_W(&ਸੁਨੇਹਾ, ਨਿਰਾ, 0, 0, PM_REMOVE))
    {
	Translate_ਸੁਨੇਹਾ(&ਸੁਨੇਹਾ);
	Dispatch_ਸੁਨੇਹਾ_W(&ਸੁਨੇਹਾ);
    }
}

#ਜੇ_ਅੰਤ // FEAT_CLIENTSERVER

#ਜੇ defined(FEAT_GUI) || (defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT))

ਬਣਤਰ charset_pair
{
    string_T	name;
    BYTE	charset;
};

ਸਥਿਰ ਬਣਤਰ charset_pair
charset_ਜੋੜੇ[] =
{
    {STR_LITERAL_INIT("ANSI"),		ANSI_CHARSET},
    {STR_LITERAL_INIT("CHINESEBIG5"),	CHINESEBIG5_CHARSET},
    {STR_LITERAL_INIT("DEFAULT"),	DEFAULT_CHARSET},
    {STR_LITERAL_INIT("HANGEUL"),	HANGEUL_CHARSET},
    {STR_LITERAL_INIT("OEM"),		OEM_CHARSET},
    {STR_LITERAL_INIT("SHIFTJIS"),	SHIFTJIS_CHARSET},
    {STR_LITERAL_INIT("SYMBOL"),	ਚਿੰਨ੍ਹ_CHARSET},
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

ਬਣਤਰ quality_pair
{
    string_T	name;
    DWORD	quality;
};

ਸਥਿਰ ਬਣਤਰ quality_pair
quality_ਜੋੜੇ[] = {
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
    ਅੱਖਰ *
charset_id2name(ਪੂਰਨ id)
{
    ਪੂਰਨ	i;

    ਲਈ (i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(charset_ਜੋੜੇ); ++i)
    {
	ਜੇ ((BYTE)id == charset_ਜੋੜੇ[i].charset)
	    ਵਾਪਸ (ਅੱਖਰ *)charset_ਜੋੜੇ[i].name.string;
    }

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Convert a quality ID to a name.
 * Return NULL when not recognized.
 */
    ਅੱਖਰ *
quality_id2name(DWORD id)
{
    ਪੂਰਨ	i;

    ਲਈ (i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(quality_ਜੋੜੇ); ++i)
    {
	ਜੇ (id == quality_ਜੋੜੇ[i].quality)
	    ਵਾਪਸ (ਅੱਖਰ *)quality_ਜੋੜੇ[i].name.string;
    }

    ਵਾਪਸ ਨਿਰਾ;
}

// The default font height in 100% scaling (96dpi).
// (-16 in 96dpi equates to roughly 12pt)
# define DEFAULT_FONT_ਉਚਾਈ	(-16)

ਸਥਿਰ ਅਚਲ LOGFONTW s_lfDefault =
{
    DEFAULT_FONT_ਉਚਾਈ,
    0, 0, 0, FW_ਸਾਧਾਰਨ, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
    ਬਾਹਰਲਾ_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
    PROOF_QUALITY, FIXED_PITCH | FF_DONTCARE,
    L""	// Default font name will be set later based on current language.
};

// This will be initialized when set_default_logfont() is called first time.
// The value will be based on the system DPI.
ਪੂਰਨ current_font_ਉਚਾਈ = 0;		// also used in gui_w32.c

/*
 * Convert a string representing a point size into pixels. The string should
 * be a positive decimal number, with an optional decimal point (eg, "12", or
 * "10.5"). The pixel value is returned, and a pointer to the next unconverted
 * character is stored in *end. The flag "vertical" says whether this
 * calculation is for a vertical (height) size or a horizontal (width) one.
 */
    ਸਥਿਰ ਪੂਰਨ
points_ਮੰਜ਼ਿਲ_pixels(WCHAR *str, WCHAR **ਸਿਰਾ, ਪੂਰਨ vertical, long_i pprinter_dc)
{
    ਪੂਰਨ		pixels;
    ਪੂਰਨ		points = 0;
    ਪੂਰਨ		ਭਾਜਕ = 0;
    HWND	hwnd = (HWND)0;
    HDC		hdc;
    HDC		printer_dc = (HDC)pprinter_dc;

    ਜਦੋਂ (*str != NUL)
    {
	ਜੇ (*str == L'.' && ਭਾਜਕ == 0)
	{
	    // Start keeping a divisor, for later
	    ਭਾਜਕ = 1;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (!VIM_ISDIGIT(*str))
		ਰੋਕੋ;

	    points *= 10;
	    points += *str - L'0';
	    ਭਾਜਕ *= 10;
	}
	++str;
    }

    ਜੇ (ਭਾਜਕ == 0)
	ਭਾਜਕ = 1;

    ਜੇ (printer_dc == ਨਿਰਾ)
    {
	hwnd = Get_Desktop_ਖਿੜਕੀ();
	hdc = Get_ਖਿੜਕੀ_DC(hwnd);
    }
    ਨਹੀਂਤਾਂ
	hdc = printer_dc;

    pixels = MulDiv(points,
		    GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX),
		    72 * ਭਾਜਕ);

    ਜੇ (printer_dc == ਨਿਰਾ)
	ReleaseDC(hwnd, hdc);

    *ਸਿਰਾ = str;
    ਵਾਪਸ pixels;
}

/*
 * Convert pixel into point size. This is a reverse of points_to_pixels.
 */
    ਸਥਿਰ ਦੁੱਗਣਾ
pixels_ਮੰਜ਼ਿਲ_points(ਪੂਰਨ pixels, ਪੂਰਨ vertical, long_i pprinter_dc)
{
    ਦੁੱਗਣਾ	points = 0;
    HWND	hwnd = (HWND)0;
    HDC		hdc;
    HDC		printer_dc = (HDC)pprinter_dc;

    ਜੇ (printer_dc == ਨਿਰਾ)
    {
	hwnd = Get_Desktop_ਖਿੜਕੀ();
	hdc = Get_ਖਿੜਕੀ_DC(hwnd);
    }
    ਨਹੀਂਤਾਂ
	hdc = printer_dc;

    points = pixels * 72.0 / GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX);
    ਜੇ (printer_dc == ਨਿਰਾ)
	ReleaseDC(hwnd, hdc);

    ਵਾਪਸ points;
}

    ਸਥਿਰ ਪੂਰਨ CALLBACK
font_enumproc(
    ENUMLOGFONTW    *elf,
    NEWTEXTMETRICW  *ntm ਅਣਵਰਤਿਆ,
    DWORD	    type ਅਣਵਰਤਿਆ,
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
    ਜੇ ((ntm->tmPitchAndFamily & 1) != 0)
	ਵਾਪਸ 1;
# endif

    // Remember this LOGFONTW as a "possible"
    *lf = elf->elfLogFont;

    // Terminate the scan as soon as we find an ANSI font
    ਜੇ (lf->lf_Char_ਸੈੱਟ == ANSI_CHARSET
	    || lf->lf_Char_ਸੈੱਟ == OEM_CHARSET
	    || lf->lf_Char_ਸੈੱਟ == DEFAULT_CHARSET)
	ਵਾਪਸ 0;

    // Continue the scan - we have a non-ANSI font
    ਵਾਪਸ 2;
}

    ਸਥਿਰ ਪੂਰਨ
init_logfont(LOGFONTW *lf)
{
    ਪੂਰਨ		n;
    HWND	hwnd = Get_Desktop_ਖਿੜਕੀ();
    HDC		hdc = Get_ਖਿੜਕੀ_DC(hwnd);

    n = EnumFontFamiliesW(hdc,
			 lf->lfFaceName,
			 (FONTENUMPROCW)font_enumproc,
			 (LPARAM)lf);

    ReleaseDC(hwnd, hdc);

    // If we couldn't find a usable font, return failure
    ਜੇ (n == 1)
	ਵਾਪਸ FAIL;

    // Tidy up the rest of the LOGFONTW structure. We set to a basic
    // font - get_logfont() sets bold, italic, etc based on the user's
    // input.
    lf->lf_ਉਚਾਈ = current_font_ਉਚਾਈ;
    lf->lf_ਚੌੜਾਈ = 0;
    lf->lfItalic = FALSE;
    lf->lfUnderline = FALSE;
    lf->lf_Strike_ਬਾਹਰਲਾ = FALSE;
    lf->lf_ਭਾਰ = FW_ਸਾਧਾਰਨ;

    // Return success
    ਵਾਪਸ OK;
}

/*
 * Call back for EnumFontFamiliesW in expand_font_enumproc.
 *
 */
    ਸਥਿਰ ਪੂਰਨ CALLBACK
expand_font_enumproc(
    ENUMLOGFONTW    *elf,
    NEWTEXTMETRICW  *ntm ਅਣਵਰਤਿਆ,
    DWORD	    type ਅਣਵਰਤਿਆ,
    LPARAM	    lparam)
{
    LOGFONTW *lf = (LOGFONTW*)elf;

# ifndef FEAT_PROPORTIONAL_FONTS
    // Ignore non-monospace fonts without further ado
    ਜੇ ((ntm->tmPitchAndFamily & 1) != 0)
	ਵਾਪਸ 1;
# endif

    // Filter only on ANSI. Otherwise will see a lot of random fonts that we
    // usually don't want.
    ਜੇ (lf->lf_Char_ਸੈੱਟ != ANSI_CHARSET)
	ਵਾਪਸ 1;

    ਪੂਰਨ (*ਜੋੜੋ_ਮੇਲ)(char_u *) = (ਪੂਰਨ (*)(char_u *))lparam;

    WCHAR *faceNameW = lf->lfFaceName;
    char_u *faceName = utf16_ਮੰਜ਼ਿਲ_enc(faceNameW, ਨਿਰਾ);
    ਜੇ (!faceName)
	ਵਾਪਸ 0;

    ਜੋੜੋ_ਮੇਲ(faceName);
    vim_free(faceName);

    ਵਾਪਸ 1;
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
    ਖਾਲੀ
gui_mch_expand_font(
    optexpand_T	*ਦਲੀਲਾਂ,
    ਖਾਲੀ	*param ਅਣਵਰਤਿਆ,
    ਪੂਰਨ		(*ਜੋੜੋ_ਮੇਲ)(char_u *val))
{
    expand_T	    *xp = ਦਲੀਲਾਂ->oe_xp;
    ਜੇ (xp->xp_ਨਮੂਨਾ > ਦਲੀਲਾਂ->oe_ਸੈੱਟ_arg && *(xp->xp_ਨਮੂਨਾ-1) == ':')
    {
	ਅੱਖਰ buf[30];
	ਪੂਰਨ	i;

	// Always fill in with the current font size as first option for
	// convenience. We simply round to the closest integer for simplicity.
	ਪੂਰਨ font_ਉਚਾਈ = (ਪੂਰਨ)ਗੇੜਾ(
		pixels_ਮੰਜ਼ਿਲ_points(-current_font_ਉਚਾਈ, TRUE, (long_i)ਨਿਰਾ));
	vim_snprintf(buf, ARRAY_ਲੰਮਾਈ(buf), "h%d", font_ਉਚਾਈ);
	ਜੋੜੋ_ਮੇਲ((char_u *)buf);

	// Note: Keep this in sync with get_logfont(). Don't include 'c' and
	// 'q' as we fill in all the values below.
	ਸਥਿਰ ਅੱਖਰ *(p_gfn_win_opt_ਮੁੱਲਾਂ[]) = {
	    "h" , "w" , "W" , "b" , "i" , "u" , "s"};
	ਲਈ (i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(p_gfn_win_opt_ਮੁੱਲਾਂ); i++)
	    ਜੋੜੋ_ਮੇਲ((char_u *)p_gfn_win_opt_ਮੁੱਲਾਂ[i]);

	ਲਈ (i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(charset_ਜੋੜੇ); ++i)
	{
	    vim_snprintf(buf, ਆਕਾਰ(buf), "c%s", charset_ਜੋੜੇ[i].name.string);
	    ਜੋੜੋ_ਮੇਲ((char_u *)buf);
	}

	ਲਈ (i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(quality_ਜੋੜੇ); ++i)
	{
	    vim_snprintf(buf, ਆਕਾਰ(buf), "q%s", quality_ਜੋੜੇ[i].name.string);
	    ਜੋੜੋ_ਮੇਲ((char_u *)buf);
	}
	ਵਾਪਸ;
    }

    HWND	hwnd = Get_Desktop_ਖਿੜਕੀ();
    HDC		hdc = Get_ਖਿੜਕੀ_DC(hwnd);

    EnumFontFamiliesW(hdc,
	    ਨਿਰਾ,
	    (FONTENUMPROCW)expand_font_enumproc,
	    (LPARAM)ਜੋੜੋ_ਮੇਲ);

    ReleaseDC(hwnd, hdc);
}

/*
 * Compare a UTF-16 string and an ASCII string literally.
 * Only works all the code points are inside ASCII range.
 */
    ਸਥਿਰ ਪੂਰਨ
utf16ascncmp(ਅਚਲ WCHAR *w, ਅਚਲ ਅੱਖਰ *p, size_t n)
{
    size_t i;

    ਲਈ (i = 0; i < n; i++)
    {
	ਜੇ (w[i] == 0 || w[i] != p[i])
	    ਵਾਪਸ w[i] - p[i];
    }
    ਵਾਪਸ 0;
}

/*
 * Equivalent of GetDpiForSystem().
 */
    UINT WINAPI
vimGetDpiForSystem(ਖਾਲੀ)
{
    HWND hwnd = Get_Desktop_ਖਿੜਕੀ();
    HDC hdc = Get_ਖਿੜਕੀ_DC(hwnd);
    UINT dpi = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(hwnd, hdc);
    ਵਾਪਸ dpi;
}

/*
 * Set default logfont based on current language.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_default_logfont(LOGFONTW *lf)
{
    // Default font name for current language on MS-Windows.
    // If not translated, falls back to "Consolas".
    // This must be a fixed-pitch font.
    ਅਚਲ ਅੱਖਰ *defaultfontname = N_("DefaultFontNameForWindows");
    ਅੱਖਰ *fontname = _(defaultfontname);

    ਜੇ (STRCMP(fontname, defaultfontname) == 0)
	fontname = "Consolas";

    *lf = s_lfDefault;
    lf->lf_ਉਚਾਈ = DEFAULT_FONT_ਉਚਾਈ * (ਪੂਰਨ)vimGetDpiForSystem() / 96;
    ਜੇ (current_font_ਉਚਾਈ == 0)
	current_font_ਉਚਾਈ = lf->lf_ਉਚਾਈ;

    WCHAR *wfontname = enc_ਮੰਜ਼ਿਲ_utf16((char_u*)fontname, ਨਿਰਾ);
    ਜੇ (wfontname != ਨਿਰਾ)
    {
	wcscpy_s(lf->lfFaceName, LF_FACESIZE, wfontname);
	vim_free(wfontname);
    }
}

/*
 * Get font info from "name" into logfont "lf".
 * Return OK for a valid name, FAIL otherwise.
 */
    ਪੂਰਨ
get_logfont(
    LOGFONTW	*lf,
    char_u	*name,
    HDC		printer_dc,
    ਪੂਰਨ		verbose)
{
    WCHAR	*p;
    ਪੂਰਨ		i;
    ਪੂਰਨ		ret = FAIL;
    ਸਥਿਰ LOGFONTW *lastlf = ਨਿਰਾ;
    WCHAR	*wname;

    ਸੈੱਟ_default_logfont(lf);
    ਜੇ (name == ਨਿਰਾ)
	ਵਾਪਸ OK;

    wname = enc_ਮੰਜ਼ਿਲ_utf16(name, ਨਿਰਾ);
    ਜੇ (wname == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਜੇ (wcscmp(wname, L"*") == 0)
    {
# ਜੇ defined(FEAT_GUI_MSWIN)
	CHOOSEFONTW	cf;
	// if name is "*", bring up std font dialog:
	CLEAR_ਖੇਤਰ(cf);
	cf.l_Struct_ਮਾਪ = ਆਕਾਰ(cf);
	cf.hwndOwner = s_hwnd;
	cf.Flags = CF_SCREENFONTS | CF_FIXEDPITCHONLY | CF_INITTOLOGFONTSTRUCT;
	ਜੇ (lastlf != ਨਿਰਾ)
	    *lf = *lastlf;
	cf.lpLogFont = lf;
	cf.nFontType = 0 ; //REGULAR_FONTTYPE;
	ਜੇ (ਚੁਣੋ_Font_W(&cf))
	    ret = OK;
# endif
	ਜਾਓ theend;
    }

    /*
     * Split name up, it could be <name>:h<height>:w<width> etc.
     */
    ਲਈ (p = wname; *p && *p != L':'; p++)
    {
	ਜੇ (p - wname + 1 >= LF_FACESIZE)
	    ਜਾਓ theend;			// Name too long
	lf->lfFaceName[p - wname] = *p;
    }
    ਜੇ (p != wname)
	lf->lfFaceName[p - wname] = NUL;

    // First set defaults
    lf->lf_ਉਚਾਈ = DEFAULT_FONT_ਉਚਾਈ * (ਪੂਰਨ)vimGetDpiForSystem() / 96;
    lf->lf_ਚੌੜਾਈ = 0;
    lf->lf_ਭਾਰ = FW_ਸਾਧਾਰਨ;
    lf->lfItalic = FALSE;
    lf->lfUnderline = FALSE;
    lf->lf_Strike_ਬਾਹਰਲਾ = FALSE;

    /*
     * If the font can't be found, try replacing '_' by ' '.
     */
    ਜੇ (init_logfont(lf) == FAIL)
    {
	ਪੂਰਨ	did_replace = FALSE;

	ਲਈ (i = 0; lf->lfFaceName[i]; ++i)
	    ਜੇ (lf->lfFaceName[i] == L'_')
	    {
		lf->lfFaceName[i] = L' ';
		did_replace = TRUE;
	    }
	ਜੇ (!did_replace || init_logfont(lf) == FAIL)
	    ਜਾਓ theend;
    }

    ਜਦੋਂ (*p == L':')
	p++;

    // Set the values found after ':'
    ਜਦੋਂ (*p)
    {
	ਚੋਣ (*p++)
	{
	    // Note: Keep this in sync with gui_mch_expand_font().
	    ਹਾਲਤ L'h':
		lf->lf_ਉਚਾਈ = - points_ਮੰਜ਼ਿਲ_pixels(p, &p, TRUE, (long_i)printer_dc);
		ਰੋਕੋ;
	    ਹਾਲਤ L'w':
		lf->lf_ਚੌੜਾਈ = points_ਮੰਜ਼ਿਲ_pixels(p, &p, FALSE, (long_i)printer_dc);
		ਰੋਕੋ;
	    ਹਾਲਤ L'W':
		lf->lf_ਭਾਰ = wcstol(p, &p, 10);
		ਰੋਕੋ;
	    ਹਾਲਤ L'b':
		lf->lf_ਭਾਰ = FW_BOLD;
		ਰੋਕੋ;
	    ਹਾਲਤ L'i':
		lf->lfItalic = TRUE;
		ਰੋਕੋ;
	    ਹਾਲਤ L'u':
		lf->lfUnderline = TRUE;
		ਰੋਕੋ;
	    ਹਾਲਤ L's':
		lf->lf_Strike_ਬਾਹਰਲਾ = TRUE;
		ਰੋਕੋ;
	    ਹਾਲਤ L'c':
		ਲਈ (i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(charset_ਜੋੜੇ); ++i)
		{
		    ਜੇ (utf16ascncmp(p, (ਅੱਖਰ *)charset_ਜੋੜੇ[i].name.string,
			    charset_ਜੋੜੇ[i].name.ਲੰਮਾਈ) == 0)
		    {
			lf->lf_Char_ਸੈੱਟ = charset_ਜੋੜੇ[i].charset;
			p += charset_ਜੋੜੇ[i].name.ਲੰਮਾਈ;
			ਰੋਕੋ;
		    }
		}

		ਜੇ (i == (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(charset_ਜੋੜੇ) && verbose)
		{
		    char_u *s = utf16_ਮੰਜ਼ਿਲ_enc(p, ਨਿਰਾ);
		    ਜੇ (s != ਨਿਰਾ)
		    {
			semsg(_(e_illegal_str_name_str_ਵਿੱਚ_font_name_str),
							   "charset", s, name);
			vim_free(s);
		    }
		}
		ਰੋਕੋ;
	    ਹਾਲਤ L'f':
		// Font features (e.g., "fss19=1").
		// Parsed separately by gui_mch_init_font(); skip here.
		ਜਦੋਂ (*p && *p != L':')
		    p++;
		ਰੋਕੋ;
	    ਹਾਲਤ L'q':
		ਲਈ (i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(quality_ਜੋੜੇ); ++i)
		{
		    ਜੇ (utf16ascncmp(p, (ਅੱਖਰ *)quality_ਜੋੜੇ[i].name.string,
			    quality_ਜੋੜੇ[i].name.ਲੰਮਾਈ) == 0)
		    {
			lf->lfQuality = quality_ਜੋੜੇ[i].quality;
			p += quality_ਜੋੜੇ[i].name.ਲੰਮਾਈ;
			ਰੋਕੋ;
		    }
		}

		ਜੇ (i == (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(quality_ਜੋੜੇ) && verbose)
		{
		    char_u *s = utf16_ਮੰਜ਼ਿਲ_enc(p, ਨਿਰਾ);
		    ਜੇ (s != ਨਿਰਾ)
		    {
			semsg(_(e_illegal_str_name_str_ਵਿੱਚ_font_name_str),
							   "quality", s, name);
			vim_free(s);
		    }
		}
		ਰੋਕੋ;
	    ਮੂਲ:
		ਜੇ (verbose)
		    semsg(_(e_illegal_char_nr_ਵਿੱਚ_font_name_str), p[-1], name);
		ਜਾਓ theend;
	}
	ਜਦੋਂ (*p == L':')
	    p++;
    }
    ret = OK;

theend:
    // ron: init lastlf
    ਜੇ (ret == OK && printer_dc == ਨਿਰਾ)
    {
	vim_free(lastlf);
	lastlf = ALLOC_ONE(LOGFONTW);
	ਜੇ (lastlf != ਨਿਰਾ)
	    mch_memmove(lastlf, lf, ਆਕਾਰ(LOGFONTW));
    }
    vim_free(wname);

    ਵਾਪਸ ret;
}

#ਜੇ_ਅੰਤ // defined(FEAT_GUI) || defined(FEAT_PRINTER)

#ਜੇ defined(FEAT_ਕੰਮ_CHANNEL)
/*
 * Initialize the Winsock dll.
 */
    ਖਾਲੀ
channel_init_winsock(ਖਾਲੀ)
{
    WSADATA wsa_ਅੰਕੜੇ;
    ਪੂਰਨ wsaerr;

    ਜੇ (WSInitialized)
	ਵਾਪਸ;

    wsaerr = WSAStartup(MAKEWORD(2, 2), &wsa_ਅੰਕੜੇ);
    ਜੇ (wsaerr == 0)
	WSInitialized = TRUE;
}
#ਜੇ_ਅੰਤ
