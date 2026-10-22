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

#અન્તર્ભાવ "vim.h"

#અન્તર્ભાવ <sys/types.h>
#અન્તર્ભાવ <limits.h>

#અન્તર્ભાવ <process.h>
#અન્તર્ભાવ <direct.h>

#જો !defined(FEAT_GUI_MSWIN)
# include <shellapi.h>
#endif

#જો defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT)
# include <dlgs.h>
# include <winspool.h>
# include <commdlg.h>
#endif

// Record all output and all keyboard & mouse input
// #define MCH_WRITE_DUMP

#ifdef MCH_WRITE_DUMP
FILE* fdDump = ખાલી;
#endif

#જો !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
બાહ્ય અક્ષર g_szOrigTitle[];
#endif

#ifdef FEAT_GUI
બાહ્ય HWND s_hwnd;
#નહીંતર
સ્થિર HWND s_hwnd = 0;	    // console window handle, set by GetConsoleHwnd()
#endif

#ifdef FEAT_JOB_CHANNEL
પૂર્ણાંક WSInitialized = FALSE; // WinSock is initialized
#endif


/*
 * Save the instance handle of the exe/dll.
 */
    શૂન્ય
SaveInst(HINSTANCE hInst)
{
    g_hinst = hInst;
}

#જો defined(FEAT_GUI_MSWIN)
/*
 * GUI version of mch_exit().
 * Shut down and exit with status `r'
 * Careful: mch_exit() may be called before mch_init()!
 */
    શૂન્ય
mch_exit_g(પૂર્ણાંક r)
{
    exiting = TRUE;

    display_errors();

    ml_close_all(TRUE);		// remove all memfiles

# ifdef FEAT_OLE
    UninitOLE();
# endif
# ifdef FEAT_JOB_CHANNEL
    જો (WSInitialized)
    {
	WSInitialized = FALSE;
	WSACleanup();
    }
# endif
# ifdef DYNAMIC_GETTEXT
    dyn_libintl_end();
# endif

    જો (gui.in_use)
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
પ્રકારવ્યાખ્યા BOOL (WINAPI *PfnRtlGetVersion)(LPOSVERSIONINFOW);
DWORD win_version;
    સ્થિર શૂન્ય
win_version_init(શૂન્ય)
{
    OSVERSIONINFOW	osver;
    HMODULE		hNtdll;
    PfnRtlGetVersion	pRtlGetVersion;

    hNtdll = GetModuleHandle("ntdll.dll");
    જો (hNtdll == ખાલી)
	પાછા;

    pRtlGetVersion =
	(PfnRtlGetVersion) GetProcAddress(hNtdll, "RtlGetVersion");
    જો (pRtlGetVersion == ખાલી)
	પાછા;

    osver.dwOSVersionInfoSize = કદ(OSVERSIONINFOW);
    pRtlGetVersion(&osver);
    win_version = MAKE_VER(min(osver.dwMajorVersion, 0xFF),
	    min(osver.dwMinorVersion, 0xFF),
	    min(osver.dw_Build_સંખ્યા, 0xFFFF));
}

/*
 * Init the tables for toupper() and tolower().
 */
    શૂન્ય
mch_early_init(શૂન્ય)
{
    પૂર્ણાંક		i;

    win_version_init();

    PlatformId();

    // Init the tables for toupper() and tolower()
    માટે (i = 0; i < 256; ++i)
	toupper_tab[i] = tolower_tab[i] = i;
    CharUpperBuff((LPSTR)toupper_tab, 256);
    CharLowerBuff((LPSTR)tolower_tab, 256);
}


/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    પૂર્ણાંક
mch_આગત_isatty(શૂન્ય)
{
#ifdef FEAT_GUI_MSWIN
# ifdef VIMDLL
    જો (gui.in_use)
# endif
	પાછા TRUE;	    // GUI always has a tty
#endif
#જો !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    જો (isatty(read_cmd_fd))
	પાછા TRUE;
    પાછા FALSE;
#endif
}

/*
 * mch_settitle(): set titlebar of our window
 */
    શૂન્ય
mch_settitle(
    char_u *title,
    char_u *icon UNUSED)
{
#ifdef FEAT_GUI_MSWIN
# ifdef VIMDLL
    જો (gui.in_use)
# endif
    {
	gui_mch_settitle(title, icon);
	પાછા;
    }
#endif
#જો !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    જો (title != ખાલી)
    {
	WCHAR	*wp = enc_to_utf16(title, ખાલી);

	જો (wp == ખાલી)
	    પાછા;

	SetConsoleTitleW(wp);
	vim_free(wp);
	પાછા;
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
    શૂન્ય
mch_restore_title(પૂર્ણાંક which UNUSED)
{
#જો !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
    જો (!gui.in_use)
# endif
	SetConsoleTitle(g_szOrigTitle);
#endif
}


/*
 * Return TRUE if we can restore the title (we can)
 */
    પૂર્ણાંક
mch_can_restore_title(શૂન્ય)
{
    પાછા TRUE;
}


/*
 * Return TRUE if we can restore the icon title (we can't)
 */
    પૂર્ણાંક
mch_can_restore_icon(શૂન્ય)
{
    પાછા FALSE;
}


/*
 * Get absolute file name into buffer "buf" of length "len" bytes,
 * turning all '/'s into '\\'s and getting the correct case of each component
 * of the file name.  Append a (back)slash to a directory name.
 * When 'shellslash' set do it the other way around.
 * Return OK or FAIL.
 */
    પૂર્ણાંક
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    પૂર્ણાંક		len,
    પૂર્ણાંક		force UNUSED)
{
    પૂર્ણાંક		nResult = FAIL;
    WCHAR	*wname;
    WCHAR	wbuf[MAX_PATH];
    char_u	*cname = ખાલી;

    wname = enc_to_utf16(fname, ખાલી);
    જો (wname != ખાલી && _wfullpath(wbuf, wname, MAX_PATH) != ખાલી)
    {
	cname = utf16_to_enc((short_u *)wbuf, ખાલી);
	જો (cname != ખાલી)
	{
	    vim_strncpy(buf, cname, len - 1);
	    nResult = OK;
	}
    }
    vim_free(wname);
    vim_free(cname);

#ifdef USE_FNAME_CASE
    fname_case(buf, len);
#નહીંતર
    slash_adjust(buf);
#endif

    પાછા nResult;
}


/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    પૂર્ણાંક
mch_isFullName(char_u *fname)
{
    // A name like "d:/foo" and "//server/share" is absolute.  "d:foo" is not.
    // /foo and \foo are absolute too because Windows keeps a current drive.
    // Another way to check is to use mch_FullName() and see if the result is
    // the same as the name or mch_FullName() fails.  However, this has quite a
    // bit of overhead, so let's not do that.
    જો (*fname == NUL)
	પાછા FALSE;
    પાછા ((ASCII_ISALPHA(fname[0]) && fname[1] == ':'
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
    શૂન્ય
slash_adjust(char_u *p)
{
    જો (path_with_url(p))
	પાછા;

    જો (*p == '`')
    {
	size_t len = STRLEN(p);

	// don't replace backslash in backtick quoted strings
	જો (len > 2 && *(p + len - 1) == '`')
	    પાછા;
    }

    જ્યારે (*p)
    {
	જો (*p == psepcN)
	    *p = psepc;
	MB_PTR_ADV(p);
    }
}

    સ્થિર પૂર્ણાંક
read_reparse_point(અચલ WCHAR *name, char_u *buf, DWORD *buf_len)
{
    HANDLE h;
    BOOL ok;

    h = CreateFileW(name, FILE_READ_ATTRIBUTES,
	    FILE_SHARE_READ | FILE_SHARE_WRITE, ખાલી,
	    OPEN_EXISTING,
	    FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,
	    ખાલી);
    જો (h == INVALID_HANDLE_કિંમત)
	પાછા FAIL;

    ok = DeviceIoControl(h, FSCTL_GET_REPARSE_POINT, ખાલી, 0, buf, *buf_len,
	    buf_len, ખાલી);
    CloseHandle(h);

    પાછા ok ? OK : FAIL;
}

    char_u *
resolve_appexeclink(char_u *fname)
{
    DWORD		attr = 0;
    પૂર્ણાંક			idx;
    WCHAR		*p, *end, *wname;
    // The buffer size is arbitrarily chosen to be "big enough" (TM), the
    // ceiling should be around 16k.
    char_u		buf[4096];
    DWORD		buf_len = કદ(buf);
    REPARSE_DATA_BUFFER *rb = (REPARSE_DATA_BUFFER *)buf;

    wname = enc_to_utf16(fname, ખાલી);
    જો (wname == ખાલી)
	પાછા ખાલી;

    attr = GetFileAttributesW(wname);
    જો (attr == INVALID_FILE_ATTRIBUTES ||
	    (attr & FILE_ATTRIBUTE_REPARSE_POINT) == 0)
    {
	vim_free(wname);
	પાછા ખાલી;
    }

    // The applinks are similar to symlinks but with a huge difference: they can
    // only be executed, any other I/O operation on them is bound to fail with
    // ERROR_FILE_NOT_FOUND even though the file exists.
    જો (read_reparse_point(wname, buf, &buf_len) == FAIL)
    {
	vim_free(wname);
	પાછા ખાલી;
    }
    vim_free(wname);

    જો (rb->ReparseTag != IO_REPARSE_TAG_APPEXECLINK)
	પાછા ખાલી;

    // The (undocumented) reparse buffer contains a set of N null-terminated
    // Unicode strings, the application path is stored in the third one.
    જો (rb->AppExecLinkReparseBuffer.String_ગણતરી < 3)
	પાછા ખાલી;

    p = rb->AppExecLinkReparseBuffer.StringList;
    end = p + rb->ReparseDataLength / કદ(WCHAR);
    માટે (idx = 0; p < end
	    && idx < (પૂર્ણાંક)rb->AppExecLinkReparseBuffer.String_ગણતરી
	    && idx != 2; )
    {
	જો (*p++ == L'\0')
	    ++idx;
    }

    પાછા utf16_to_enc(p, ખાલી);
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
    સ્થિર પૂર્ણાંક
mswin_stat_impl(અચલ WCHAR *name, stat_T *stp, અચલ પૂર્ણાંક resolve)
{
    પૂર્ણાંક			n;
    પૂર્ણાંક			fd;
    BOOL		is_symlink = FALSE;
    HANDLE		hFind, h;
    DWORD		attr = 0;
    DWORD		flag = 0;
    WIN32_FIND_DATAW    findDataW;

#જો 0 && defined(_UCRT)
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
    જો (resolve)
	// Universal CRT can handle symlinks properly.
	પાછા _wstat(name, stp);
#endif

    // Only reparse points need a directory query to identify symbolic links.
    // In particular, avoid enumerating the directory for each missing file
    // while searching 'runtimepath'.
    attr = GetFileAttributesW(name);
    જો (attr == INVALID_FILE_ATTRIBUTES)
    {
	DWORD error = GetLastError();

	// A missing file or path cannot provide stat information.  Other
	// errors and UNC paths still need the CRT fallback.
	જો ((error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND)
		&& !((name[0] == L'\\' || name[0] == L'/')
		    && (name[1] == L'\\' || name[1] == L'/')))
	{
	    errno = ENOENT;
	    પાછા -1;
	}
	પાછા _wstat(name, stp);
    }
    જો (!(attr & FILE_ATTRIBUTE_REPARSE_POINT))
	પાછા _wstat(name, stp);

    hFind = FindFirstFileW(name, &findDataW);
    જો (hFind != INVALID_HANDLE_કિંમત)
    {
	attr = findDataW.dwFileAttributes;
	જો ((attr & FILE_ATTRIBUTE_REPARSE_POINT)
		&& (findDataW.dwReserved0 == IO_REPARSE_TAG_SYMLINK))
	    is_symlink = TRUE;
	FindClose(hFind);
    }

    // Use the plain old stat() whenever it's possible.
    જો (!is_symlink)
	પાછા _wstat(name, stp);

    જો (!resolve && is_symlink)
	flag = FILE_FLAG_OPEN_REPARSE_POINT;
    જો (attr & FILE_ATTRIBUTE_DIRECTORY)
	flag |= FILE_FLAG_BACKUP_SEMANTICS;

    h = CreateFileW(name, FILE_READ_ATTRIBUTES,
	    FILE_SHARE_READ | FILE_SHARE_WRITE, ખાલી, OPEN_EXISTING, flag,
	    ખાલી);
    જો (h == INVALID_HANDLE_કિંમત)
	પાછા -1;

    fd = _open_osfhandle((intptr_t)h, _O_RDONLY);
    n = _fstat(fd, (સંરચના _stat *)stp);
    જો ((n == 0) && (attr & FILE_ATTRIBUTE_DIRECTORY))
	stp->st_mode = (stp->st_mode & ~S_IFMT) | S_IFDIR;
    _close(fd);

    પાછા n;
}

/*
 * stat() can't handle a trailing '/' or '\', remove it first.
 * When 'resolve' is true behave as lstat() wrt symlinks.
 */
    સ્થિર પૂર્ણાંક
stat_impl(અચલ અક્ષર *name, stat_T *stp, અચલ પૂર્ણાંક resolve)
{
    // WinNT and later can use _MAX_PATH wide characters for a pathname, which
    // means that the maximum pathname is _MAX_PATH * 3 bytes when 'enc' is
    // UTF-8.
    char_u	buf[_MAX_PATH * 3 + 1];
    size_t	buflen;
    char_u	*p;
    WCHAR	*wp;
    પૂર્ણાંક		n;

    vim_strncpy((char_u *)buf, (char_u *)name, કદ(buf) - 1);
    buflen = STRLEN(buf);
    p = buf + buflen;
    જો (p > buf)
	MB_PTR_BACK(buf, p);

    // Remove trailing '\\' except root path.
    જો (p > buf && (*p == '\\' || *p == '/') && p[-1] != ':')
    {
	*p = NUL;
	buflen = (size_t)(p - buf);
    }

    જો ((buf[0] == '\\' && buf[1] == '\\') || (buf[0] == '/' && buf[1] == '/'))
    {
	// UNC root path must be followed by '\\'.
	p = vim_strpbrk(buf + 2, (char_u *)"\\/");
	જો (p != ખાલી)
	{
	    p = vim_strpbrk(p + 1, (char_u *)"\\/");
	    જો (p == ખાલી)
		STRCPY(buf + buflen, "\\");
	}
    }

    wp = enc_to_utf16(buf, ખાલી);
    જો (wp == ખાલી)
	પાછા -1;

    n = mswin_stat_impl(wp, stp, resolve);
    vim_free(wp);
    પાછા n;
}

    પૂર્ણાંક
vim_lstat(અચલ અક્ષર *name, stat_T *stp)
{
    પાછા stat_impl(name, stp, FALSE);
}

    પૂર્ણાંક
vim_stat(અચલ અક્ષર *name, stat_T *stp)
{
    પાછા stat_impl(name, stp, TRUE);
}

#જો defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    શૂન્ય
mch_settmode(tmode_T tmode UNUSED)
{
    // nothing to do
}

    પૂર્ણાંક
mch_get_shellsize(શૂન્ય)
{
    // never used
    પાછા OK;
}

    શૂન્ય
mch_set_shellsize(શૂન્ય)
{
    // never used
}

/*
 * Rows and/or Columns has changed.
 */
    શૂન્ય
mch_new_shellsize(શૂન્ય)
{
    // never used
}

    શૂન્ય
mch_calc_cell_size(સંરચના cellsize *cs_out UNUSED)
{
    // never used
}

#endif

/*
 * We have no job control, so fake it by starting a new shell.
 */
    શૂન્ય
mch_suspend(શૂન્ય)
{
    suspend_shell();
}

#જો defined(USE_MCH_ERRMSG)

# ifdef display_errors
#  undef display_errors
# endif

/*
 * Display the saved error message(s).
 */
    શૂન્ય
display_errors(શૂન્ય)
{
# ifdef FEAT_GUI
    char_u *p;

#  ifdef VIMDLL
    જો (gui.in_use || gui.starting)
#  endif
    {
	જો (error_ga.ga_data != ખાલી)
	{
	    // avoid putting up a message box with blanks only
	    માટે (p = (char_u *)error_ga.ga_data; *p; ++p)
		જો (!SAFE_isspace(*p))
		{
		    // Only use a dialog when not using --gui-dialog-file:
		    // write text to a file.
		    જો (!gui_dialog_log((char_u *)"Errors", p))
			(શૂન્ય)gui_mch_dialog(
				     gui.starting ? VIM_INFO :
					     VIM_ERROR,
				     gui.starting ? (char_u *)_("Message") :
					     (char_u *)_("Error"),
					     p, (char_u *)_("&Ok"),
					1, ખાલી, FALSE);
		    તોડો;
		}
	    ga_clear(&error_ga);
	}
	પાછા;
    }
# endif
# જો !defined(FEAT_GUI) || defined(VIMDLL)
    FlushFileBuffers(GetStdHandle(STD_ERROR_HANDLE));
# endif
}
#endif


/*
 * Return TRUE if "p" contain a wildcard that can be expanded by
 * dos_expandpath().
 */
    પૂર્ણાંક
mch_has_exp_wildcard(char_u *p)
{
    માટે ( ; *p; MB_PTR_ADV(p))
    {
	જો (vim_strchr((char_u *)"?*[", *p) != ખાલી
		|| (*p == '~' && p[1] != NUL))
	    પાછા TRUE;
    }
    પાછા FALSE;
}

/*
 * Return TRUE if "p" contain a wildcard or a "~1" kind of thing (could be a
 * shortened file name).
 */
    પૂર્ણાંક
mch_has_wildcard(char_u *p)
{
    માટે ( ; *p; MB_PTR_ADV(p))
    {
	જો (vim_strchr((char_u *)"?*$[`", *p) != ખાલી
		|| (*p == '~' && p[1] != NUL))
	    પાછા TRUE;
    }
    પાછા FALSE;
}


/*
 * The normal _chdir() does not change the default drive.  This one does.
 * Returning 0 implies success; -1 implies failure.
 */
    પૂર્ણાંક
mch_chdir(અક્ષર *path)
{
    WCHAR   *p;
    પૂર્ણાંક	    n;

    જો (path[0] == NUL)		// just checking...
	પાછા -1;

    જો (p_verbose >= 5)
    {
	verbose_enter();
	smsg("chdir(%s)", path);
	verbose_leave();
    }
    જો (SAFE_isalpha(path[0]) && path[1] == ':')	// has a drive name
    {
	// If we can change to the drive, skip that part of the path.  If we
	// can't then the current directory may be invalid, try using chdir()
	// with the whole path.
	જો (_chdrive(TOLOWER_ASC(path[0]) - 'a' + 1) == 0)
	    path += 2;
    }

    જો (*path == NUL)		// drive name only
	પાછા 0;

    p = enc_to_utf16((char_u *)path, ખાલી);
    જો (p == ખાલી)
	પાછા -1;

    n = _wchdir(p);
    vim_free(p);
    પાછા n;
}


#જો defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
/*
 * return non-zero if a character is available
 */
    પૂર્ણાંક
mch_char_avail(શૂન્ય)
{
    // never used
    પાછા TRUE;
}

# જો defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    પૂર્ણાંક
mch_check_messages(શૂન્ય)
{
    // TODO: check for messages
    પાછા TRUE;
}
# endif
#endif


#જો defined(FEAT_LIBCALL)
/*
 * Call a DLL routine which takes either a string or int param
 * and returns an allocated string.
 * Return OK if it worked, FAIL if not.
 */
પ્રકારવ્યાખ્યા char_u *(*MYSTRPROCSTR)(char_u *);
પ્રકારવ્યાખ્યા પૂર્ણાંક (*MYSTRPROCINT)(char_u *);
પ્રકારવ્યાખ્યા char_u *(*MYINTPROCSTR)(પૂર્ણાંક);
પ્રકારવ્યાખ્યા પૂર્ણાંક (*MYINTPROCINT)(પૂર્ણાંક);

/*
 * Check if a pointer points to a valid NUL terminated string.
 * Return the length of the string, including terminating NUL.
 * Returns 0 for an invalid pointer, 1 for an empty string.
 */
    સ્થિર size_t
check_str_len(char_u *str)
{
    SYSTEM_INFO			si;
    MEMORY_BASIC_INFORMATION	mbi;
    size_t			length = 0;
    size_t			i;
    અચલ char_u		*p;

    // get page size
    GetSystemInfo(&si);

    // get memory information
    જો (!VirtualQuery(str, &mbi, કદ(mbi)))
	પાછા 0;

    // pre cast these (typing savers)
    long_u dwStr = (long_u)str;
    long_u dwBaseAddress = (long_u)mbi.BaseAddress;

    // get start address of page that str is on
    long_u strPage = dwStr - (dwStr - dwBaseAddress) % si.dwPageSize;

    // get length from str to end of page
    long_u pageLength = si.dwPageSize - (dwStr - strPage);

    માટે (p = str; !IsBadReadPtr(p, (UINT)pageLength);
	    p += pageLength, pageLength = si.dwPageSize)
	માટે (i = 0; i < pageLength; ++i, ++length)
	    જો (p[i] == NUL)
		પાછા length + 1;

    પાછા 0;
}

/*
 * Passed to do_in_runtimepath() to load a vim.ico file.
 */
    સ્થિર શૂન્ય
mch_icon_load_cb(char_u *fname, શૂન્ય *cookie)
{
    HANDLE *h = (HANDLE *)cookie;

    *h = LoadImage(ખાલી,
		   (LPSTR)fname,
		   IMAGE_ICON,
		   64,
		   64,
		   LR_LOADFROMFILE | LR_LOADMAP3DCOLORS);
}

/*
 * Try loading an icon file from 'runtimepath'.
 */
    પૂર્ણાંક
mch_icon_load(HANDLE *iconp)
{
    પાછા do_in_runtimepath((char_u *)"bitmaps/vim.ico",
						  0, mch_icon_load_cb, iconp);
}

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if the OS PRNG is not available or something went wrong.
 */
    પૂર્ણાંક
mch_get_random(char_u *buf, પૂર્ણાંક len)
{
    સ્થિર પૂર્ણાંક		initialized = NOTDONE;
    સ્થિર HINSTANCE	hInstLib;
    સ્થિર BOOL (WINAPI *pProcessPrng)(PUCHAR, ULONG);

    જો (initialized == NOTDONE)
    {
	hInstLib = vimLoadLib("bcryptprimitives.dll");
	જો (hInstLib != ખાલી)
	    pProcessPrng = (શૂન્ય *)GetProcAddress(hInstLib, "ProcessPrng");
	જો (hInstLib == ખાલી || pProcessPrng == ખાલી)
	{
	    FreeLibrary(hInstLib);
	    initialized = FAIL;
	}
	નહીંતર
	    initialized = OK;
    }

    જો (initialized == FAIL)
	પાછા FAIL;

    // According to the documentation this call cannot fail.
    pProcessPrng(buf, len);

    પાછા OK;
}

    પૂર્ણાંક
mch_libcall(
    char_u	*libname,
    char_u	*funcname,
    char_u	*argstring,	// NULL when using a argint
    પૂર્ણાંક		argint,
    char_u	**string_result,// NULL when using number_result
    પૂર્ણાંક		*સંખ્યા_result)
{
    HINSTANCE		hinstLib;

    BOOL fRunTimeLinkSuccess = FALSE;

    // Get a handle to the DLL module.
    hinstLib = vimLoadLib((અક્ષર *)libname);

    // If the handle is valid, try to get the function address.
    જો (hinstLib != ખાલી)
    {
	char_u	*retval_str = ખાલી;
	પૂર્ણાંક	retval_int = 0;
	size_t	len;

# ifdef HAVE_TRY_EXCEPT
	__try
	{
# endif
	જો (argstring != ખાલી)
	{
	    // Call with string argument
	    જો (string_result != ખાલી)
	    {
		MYSTRPROCSTR proc;

		proc = (MYSTRPROCSTR)GetProcAddress(hinstLib, (LPCSTR)funcname);
		જો (proc != ખાલી)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_str = proc(argstring);
		}
	    }
	    નહીંતર
	    {
		MYSTRPROCINT proc;

		proc = (MYSTRPROCINT)GetProcAddress(hinstLib, (LPCSTR)funcname);
		જો (proc != ખાલી)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_int = proc(argstring);
		}
	    }
	}
	નહીંતર
	{
	    // Call with number argument
	    જો (string_result != ખાલી)
	    {
		MYINTPROCSTR proc;

		proc = (MYINTPROCSTR)GetProcAddress(hinstLib, (LPCSTR)funcname);
		જો (proc != ખાલી)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_str = proc(argint);
		}
	    }
	    નહીંતર
	    {
		MYINTPROCINT proc;

		proc = (MYINTPROCINT)GetProcAddress(hinstLib, (LPCSTR)funcname);
		જો (proc != ખાલી)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_int = proc(argint);
		}
	    }
	}

	// Save the string before we free the library.
	// Assume that a "1" result is an illegal pointer.
	જો (string_result == ખાલી)
	    *સંખ્યા_result = retval_int;
	નહીંતર જો (retval_str != ખાલી
		&& (len = check_str_len(retval_str)) > 0)
	{
	    *string_result = alloc(len);
	    જો (*string_result != ખાલી)
		mch_memmove(*string_result, retval_str, len);
	}

# ifdef HAVE_TRY_EXCEPT
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	    જો (GetExceptionCode() == EXCEPTION_ઢગલો_OVERFLOW)
		_resetstkoflw();
	    fRunTimeLinkSuccess = FALSE;
	}
# endif

	// Free the DLL module.
	(શૂન્ય)FreeLibrary(hinstLib);
    }

    જો (!fRunTimeLinkSuccess)
    {
	semsg(_(e_library_call_failed_for_str), funcname);
	પાછા FAIL;
    }

    પાછા OK;
}
#endif

/*
 * Debugging helper: expose the MCH_WRITE_DUMP stuff to other modules
 */
    શૂન્ય
DumpPutS(અચલ અક્ષર *psz UNUSED)
{
#ifdef MCH_WRITE_DUMP
    જો (fdDump)
    {
	fputs(psz, fdDump);
	જો (psz[strlen(psz) - 1] != '\n')
	    fputc('\n', fdDump);
	fflush(fdDump);
    }
#endif
}

#ifdef _DEBUG

શૂન્ય __cdecl
Trace(
    અક્ષર *pszFormat,
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

#જો !defined(FEAT_GUI) || defined(VIMDLL)
બાહ્ય HWND g_hWnd;	// This is in os_win32.c.

/*
 * Showing the printer dialog is tricky since we have no GUI
 * window to parent it. The following routines are needed to
 * get the window parenting and Z-order to work properly.
 */
    સ્થિર શૂન્ય
GetConsoleHwnd(શૂન્ય)
{
    // Skip if it's already set.
    જો (s_hwnd != 0)
	પાછા;

    // Window handle may have been found by init code (Windows NT only)
    જો (g_hWnd != 0)
    {
	s_hwnd = g_hWnd;
	પાછા;
    }

    s_hwnd = GetConsoleWindow();
}

/*
 * Console implementation of ":winpos".
 */
    પૂર્ણાંક
mch_get_winpos(પૂર્ણાંક *x, પૂર્ણાંક *y)
{
    RECT  rect;

    GetConsoleHwnd();
    GetWindowRect(s_hwnd, &rect);
    *x = rect.ડાબું;
    *y = rect.ટોચ;
    પાછા OK;
}

/*
 * Console implementation of ":winpos x y".
 */
    શૂન્ય
mch_set_winpos(પૂર્ણાંક x, પૂર્ણાંક y)
{
    GetConsoleHwnd();
    SetWindowPos(s_hwnd, ખાલી, x, y, 0, 0,
		 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}
#endif

#જો defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT)

//=================================================================
// Win32 printer stuff

સ્થિર HFONT		prt_font_handles[2][2][2];
સ્થિર PRINTDLGW	prt_dlg;
સ્થિર અચલ પૂર્ણાંક	boldface[2] = {FW_REGULAR, FW_BOLD};
સ્થિર TEXTMETRIC	prt_tm;
સ્થિર પૂર્ણાંક		prt_લીટી_height;
સ્થિર પૂર્ણાંક		prt_સંખ્યા_width;
સ્થિર પૂર્ણાંક		prt_ડાબું_margin;
સ્થિર પૂર્ણાંક		prt_જમણું_margin;
સ્થિર પૂર્ણાંક		prt_ટોચ_margin;
સ્થિર char_u		szAppName[] = TEXT("VIM");
સ્થિર HWND		hDlgPrint;
સ્થિર પૂર્ણાંક		*bUserAbort = ખાલી;
સ્થિર char_u		*prt_name = ખાલી;

// Defines which are also in vim.rc.
# define IDC_BOX1		400
# define IDC_PRINTTEXT1		401
# define IDC_PRINTTEXT2		402
# define IDC_PROGRESS		403

    સ્થિર BOOL
vimSetDlgItemText(HWND hDlg, પૂર્ણાંક nIDDlgItem, char_u *s)
{
    WCHAR   *wp;
    BOOL    ret;

    wp = enc_to_utf16(s, ખાલી);
    જો (wp == ખાલી)
	પાછા FALSE;

    ret = SetDlgItemTextW(hDlg, nIDDlgItem, wp);
    vim_free(wp);
    પાછા ret;
}

/*
 * Convert BGR to RGB for Windows GDI calls
 */
    સ્થિર COLORREF
swap_me(COLORREF colorref)
{
    પૂર્ણાંક  temp;
    અક્ષર *ptr = (અક્ષર *)&colorref;

    temp = *(ptr);
    *(ptr ) = *(ptr + 2);
    *(ptr + 2) = temp;
    પાછા colorref;
}

    સ્થિર INT_PTR CALLBACK
PrintDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam UNUSED,
	LPARAM lParam UNUSED)
{
# ifdef FEAT_GETTEXT
    NONCLIENTMETRICS nm;
    સ્થિર HFONT hfont;
# endif

    પસંદગી (message)
    {
	સ્થિતિ WM_INITDIALOG:
# ifdef FEAT_GETTEXT
	    nm.cbSize = કદ(NONCLIENTMETRICS);
	    જો (SystemParametersInfo(
			SPI_GETNONCLIENTMETRICS,
			કદ(NONCLIENTMETRICS),
			&nm,
			0))
	    {
		અક્ષર buff[MAX_PATH];
		પૂર્ણાંક i;

		// Translate the dialog texts
		hfont = CreateFontIndirect(&nm.lfMessageFont);
		માટે (i = IDC_PRINTTEXT1; i <= IDC_PROGRESS; i++)
		{
		    SendDlgItemMessage(hDlg, i, WM_SETFONT, (WPARAM)hfont, 1);
		    જો (GetDlgItemText(hDlg,i, buff, કદ(buff)))
			vimSetDlgItemText(hDlg,i, (char_u *)_(buff));
		}
		SendDlgItemMessage(hDlg, IDCANCEL,
						WM_SETFONT, (WPARAM)hfont, 1);
		જો (GetDlgItemText(hDlg,IDCANCEL, buff, કદ(buff)))
		    vimSetDlgItemText(hDlg,IDCANCEL, (char_u *)_(buff));
	    }
# endif
	    SetWindowText(hDlg, (LPCSTR)szAppName);
	    જો (prt_name != ખાલી)
	    {
		vimSetDlgItemText(hDlg, IDC_PRINTTEXT2, (char_u *)prt_name);
		VIM_CLEAR(prt_name);
	    }
	    EnableMenuItem(GetSystemMenu(hDlg, FALSE), SC_CLOSE, MF_GRAYED);
# જો !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	    જો (!gui.in_use)
#  endif
		Bring_Window_To_ટોચ(s_hwnd);
# endif
	    પાછા TRUE;

	સ્થિતિ WM_COMMAND:
	    *bUserAbort = TRUE;
	    EnableWindow(GetParent(hDlg), TRUE);
	    DestroyWindow(hDlg);
	    hDlgPrint = ખાલી;
# ifdef FEAT_GETTEXT
	    DeleteObject(hfont);
# endif
	    પાછા TRUE;
    }
    પાછા FALSE;
}

    સ્થિર BOOL CALLBACK
AbortProc(HDC hdcPrn UNUSED, પૂર્ણાંક iCode UNUSED)
{
    સંદેશ_2 સંદેશ;

    જ્યારે (!*bUserAbort && PeekMessageW(&સંદેશ, ખાલી, 0, 0, PM_REMOVE))
    {
	જો (!hDlgPrint || !IsDialogMessageW(hDlgPrint, &સંદેશ))
	{
	    TranslateMessage(&સંદેશ);
	    DispatchMessageW(&સંદેશ);
	}
    }
    પાછા !*bUserAbort;
}

    સ્થિર UINT_PTR CALLBACK
PrintHookProc(
	HWND hDlg,	// handle to dialog box
	UINT ui_સંદેશ,	// message identifier
	WPARAM wParam UNUSED,	// message parameter
	LPARAM lParam	// message parameter
	)
{
    HWND	hwndOwner;
    RECT	rc, rcDlg, rcOwner;
    PRINTDLGW	*pPD;

    જો (ui_સંદેશ != WM_INITDIALOG)
	પાછા FALSE;

    // Get the owner window and dialog box rectangles.
    જો ((hwndOwner = GetParent(hDlg)) == ખાલી)
	hwndOwner = GetDesktopWindow();

    GetWindowRect(hwndOwner, &rcOwner);
    GetWindowRect(hDlg, &rcDlg);
    CopyRect(&rc, &rcOwner);

    // Offset the owner and dialog box rectangles so that
    // right and bottom values represent the width and
    // height, and then offset the owner again to discard
    // space taken up by the dialog box.

    OffsetRect(&rcDlg, -rcDlg.ડાબું, -rcDlg.ટોચ);
    OffsetRect(&rc, -rc.ડાબું, -rc.ટોચ);
    OffsetRect(&rc, -rcDlg.જમણું, -rcDlg.bottom);

    // The new position is the sum of half the remaining
    // space and the owner's original position.

    SetWindowPos(hDlg,
	    HWND_ટોચ,
	    rcOwner.ડાબું + (rc.જમણું / 2),
	    rcOwner.ટોચ + (rc.bottom / 2),
	    0, 0,		// ignores size arguments
	    SWP_NOSIZE);

    //  tackle the printdlg copiesctrl problem
    pPD = (PRINTDLGW *)lParam;
    pPD->nCopies = (WORD)pPD->lCustData;
    SetDlgItemInt( hDlg, edt3, pPD->nCopies, FALSE );
    //  Bring the window to top
    Bring_Window_To_ટોચ(GetParent(hDlg));
    SetForegroundWindow(hDlg);

    પાછા FALSE;
}

    શૂન્ય
mch_print_cleanup(શૂન્ય)
{
    પૂર્ણાંક pifItalic;
    પૂર્ણાંક pifBold;
    પૂર્ણાંક pifUnderline;

    માટે (pifBold = 0; pifBold <= 1; pifBold++)
	માટે (pifItalic = 0; pifItalic <= 1; pifItalic++)
	    માટે (pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
		DeleteObject(prt_font_handles[pifBold][pifItalic][pifUnderline]);

    જો (prt_dlg.hDC != ખાલી)
	DeleteDC(prt_dlg.hDC);
    જો (!*bUserAbort && hDlgPrint != ખાલી)
	SendMessage(hDlgPrint, WM_COMMAND, 0, 0);
}

    સ્થિર પૂર્ણાંક
to_device_units(પૂર્ણાંક idx, પૂર્ણાંક dpi, પૂર્ણાંક physsize, પૂર્ણાંક offset, પૂર્ણાંક def_સંખ્યા)
{
    પૂર્ણાંક		ret = 0;
    પૂર્ણાંક		u;
    પૂર્ણાંક		nr;

    u = prt_get_unit(idx);
    જો (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_સંખ્યા;
    }
    નહીંતર
	nr = printer_opts[idx].સંખ્યા;

    પસંદગી (u)
    {
	સ્થિતિ PRT_UNIT_PERC:
	    ret = (physsize * nr) / 100;
	    તોડો;
	સ્થિતિ PRT_UNIT_INCH:
	    ret = (nr * dpi);
	    તોડો;
	સ્થિતિ PRT_UNIT_MM:
	    ret = (nr * 10 * dpi) / 254;
	    તોડો;
	સ્થિતિ PRT_UNIT_POINT:
	    ret = (nr * 10 * dpi) / 720;
	    તોડો;
    }

    જો (ret < offset)
	પાછા 0;
    નહીંતર
	પાછા ret - offset;
}

    સ્થિર પૂર્ણાંક
prt_get_cpl(શૂન્ય)
{
    પૂર્ણાંક		hr;
    પૂર્ણાંક		phyw;
    પૂર્ણાંક		dvoff;
    પૂર્ણાંક		rev_offset;
    પૂર્ણાંક		dpi;

    GetTextMetrics(prt_dlg.hDC, &prt_tm);
    prt_લીટી_height = prt_tm.tmHeight + prt_tm.tmExternalLeading;

    hr	    = GetDeviceCaps(prt_dlg.hDC, HORZRES);
    phyw    = GetDeviceCaps(prt_dlg.hDC, PHYSICALWIDTH);
    dvoff   = GetDeviceCaps(prt_dlg.hDC, PHYSICALOFFSETX);
    dpi	    = GetDeviceCaps(prt_dlg.hDC, LOGPIXELSX);

    rev_offset = phyw - (dvoff + hr);

    prt_ડાબું_margin = to_device_units(OPT_PRINT_ડાબું, dpi, phyw, dvoff, 10);
    જો (prt_use_સંખ્યા())
    {
	prt_સંખ્યા_width = PRINT_સંખ્યા_WIDTH * prt_tm.tmAveCharWidth;
	prt_ડાબું_margin += prt_સંખ્યા_width;
    }
    નહીંતર
	prt_સંખ્યા_width = 0;

    prt_જમણું_margin = hr - to_device_units(OPT_PRINT_જમણું, dpi, phyw,
							       rev_offset, 5);

    પાછા (prt_જમણું_margin - prt_ડાબું_margin) / prt_tm.tmAveCharWidth;
}

    સ્થિર પૂર્ણાંક
prt_get_lpp(શૂન્ય)
{
    પૂર્ણાંક vr;
    પૂર્ણાંક phyw;
    પૂર્ણાંક dvoff;
    પૂર્ણાંક rev_offset;
    પૂર્ણાંક	bottom_margin;
    પૂર્ણાંક	dpi;

    vr	    = GetDeviceCaps(prt_dlg.hDC, VERTRES);
    phyw    = GetDeviceCaps(prt_dlg.hDC, PHYSICALHEIGHT);
    dvoff   = GetDeviceCaps(prt_dlg.hDC, PHYSICALOFFSETY);
    dpi	    = GetDeviceCaps(prt_dlg.hDC, LOGPIXELSY);

    rev_offset = phyw - (dvoff + vr);

    prt_ટોચ_margin = to_device_units(OPT_PRINT_ટોચ, dpi, phyw, dvoff, 5);

    // adjust top margin if there is a header
    prt_ટોચ_margin += prt_લીટી_height * prt_header_height();

    bottom_margin = vr - to_device_units(OPT_PRINT_BOT, dpi, phyw,
							       rev_offset, 5);

    પાછા (bottom_margin - prt_ટોચ_margin) / prt_લીટી_height;
}

    પૂર્ણાંક
mch_print_init(prt_settings_T *psettings, char_u *jobname, પૂર્ણાંક forceit)
{
    સ્થિર HGLOBAL	stored_dm = ખાલી;
    સ્થિર HGLOBAL	stored_devn = ખાલી;
    સ્થિર પૂર્ણાંક		stored_nCopies = 1;
    સ્થિર પૂર્ણાંક		stored_nFlags = 0;

    LOGFONTW		fLogFont;
    પૂર્ણાંક			pifItalic;
    પૂર્ણાંક			pifBold;
    પૂર્ણાંક			pifUnderline;

    DEVMODEW		*mem;
    DEVNAMES		*devname;
    પૂર્ણાંક			i;
    DWORD		err;

    bUserAbort = &(psettings->user_abort);
    CLEAR_FIELD(prt_dlg);
    prt_dlg.lStructSize = કદ(PRINTDLGW);
# જો !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
    જો (!gui.in_use)
#  endif
	GetConsoleHwnd();	    // get value of s_hwnd
# endif
    prt_dlg.hwndOwner = s_hwnd;
    prt_dlg.Flags = PD_NOPAGENUMS | PD_NOSELECTION | PD_RETURNDC;
    જો (!forceit)
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
    જો (forceit
# જો !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	    || (!gui.in_use && !term_console)
#  નહીંતર
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
	જો (*p_pdev != NUL)
	    prt_dlg.hDC = CreateDC(ખાલી, (LPCSTR)p_pdev, ખાલી, ખાલી);
	નહીંતર
	{
	    prt_dlg.Flags |= PD_RETURNDEFAULT;
	    જો (PrintDlgW(&prt_dlg) == 0)
		જાઓ init_fail_dlg;
	}
    }
    નહીંતર જો (PrintDlgW(&prt_dlg) == 0)
	જાઓ init_fail_dlg;
    નહીંતર
    {
	/*
	 * keep the previous driver context
	 */
	stored_dm = prt_dlg.hDevMode;
	stored_devn = prt_dlg.hDevNames;
	stored_nFlags = prt_dlg.Flags;
	stored_nCopies = prt_dlg.nCopies;
    }

    જો (prt_dlg.hDC == ખાલી)
    {
	emsg(_(e_printer_selection_failed));
	mch_print_cleanup();
	પાછા FALSE;
    }

    // Not all printer drivers report the support of color (or grey) in the
    // same way.  Let's set has_color if there appears to be some way to print
    // more than B&W.
    i = GetDeviceCaps(prt_dlg.hDC, NUMCOLORS);
    psettings->has_color = (GetDeviceCaps(prt_dlg.hDC, BITSPIXEL) > 1
				   || GetDeviceCaps(prt_dlg.hDC, PLANES) > 1
				   || i > 2 || i == -1);

    // Ensure all font styles are baseline aligned
    SetTextAlign(prt_dlg.hDC, TA_BASELINE|TA_ડાબું);

    /*
     * On some windows systems the nCopies parameter is not
     * passed back correctly. It must be retrieved from the
     * hDevMode struct.
     */
    mem = (DEVMODEW *)GlobalLock(prt_dlg.hDevMode);
    જો (mem != ખાલી)
    {
	જો (mem->dmCopies != 1)
	    stored_nCopies = mem->dmCopies;
	જો ((mem->dmFields & DM_DUPLEX) && (mem->dmDuplex & ~DMDUP_SIMPLEX))
	    psettings->duplex = TRUE;
	જો ((mem->dmFields & DM_COLOR) && (mem->dmColor & DMCOLOR_COLOR))
	    psettings->has_color = TRUE;
    }
    GlobalUnlock(prt_dlg.hDevMode);

    devname = (DEVNAMES *)GlobalLock(prt_dlg.hDevNames);
    જો (devname != 0)
    {
	WCHAR	*wprinter_name = (WCHAR *)devname + devname->wDeviceOffset;
	WCHAR	*wport_name = (WCHAR *)devname + devname->wOutputOffset;
	char_u	*text = (char_u *)_("to %s on %s");
	char_u  *printer_name = utf16_to_enc(wprinter_name, ખાલી);
	char_u	*port_name = utf16_to_enc(wport_name, ખાલી);

	જો (printer_name != ખાલી && port_name != ખાલી)
	    prt_name = alloc(STRLEN(printer_name)
					   + STRLEN(port_name) + STRLEN(text));
	જો (prt_name != ખાલી)
	    wsprintf((અક્ષર *)prt_name, (અચલ અક્ષર *)text,
		    printer_name, port_name);
	vim_free(printer_name);
	vim_free(port_name);
    }
    GlobalUnlock(prt_dlg.hDevNames);

    /*
     * Initialise the font according to 'printfont'
     */
    CLEAR_FIELD(fLogFont);
    જો (get_logfont(&fLogFont, p_pfn, prt_dlg.hDC, TRUE) == FAIL)
    {
	semsg(_(e_unknown_printer_font_str), p_pfn);
	mch_print_cleanup();
	પાછા FALSE;
    }

    માટે (pifBold = 0; pifBold <= 1; pifBold++)
	માટે (pifItalic = 0; pifItalic <= 1; pifItalic++)
	    માટે (pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
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
    psettings->chars_per_લીટી = prt_get_cpl();
    psettings->લીટીઓ_per_page = prt_get_lpp();
    જો (prt_dlg.Flags & PD_USEDEVMODECOPIESANDCOLLATE)
    {
	psettings->n_collated_copies = (prt_dlg.Flags & PD_COLLATE)
						    ? prt_dlg.nCopies : 1;
	psettings->n_uncollated_copies = (prt_dlg.Flags & PD_COLLATE)
						    ? 1 : prt_dlg.nCopies;

	જો (psettings->n_collated_copies == 0)
	    psettings->n_collated_copies = 1;

	જો (psettings->n_uncollated_copies == 0)
	    psettings->n_uncollated_copies = 1;
    }
    નહીંતર
    {
	psettings->n_collated_copies = 1;
	psettings->n_uncollated_copies = 1;
    }

    psettings->jobname = jobname;

    પાછા TRUE;

init_fail_dlg:
    err = CommDlgExtendedError();
    જો (err)
    {
	char_u *buf;

	// I suspect FormatMessage() doesn't work for values returned by
	// CommDlgExtendedError().  What does?
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		ખાલી, err, 0, (LPTSTR)(&buf), 0, ખાલી);
	semsg(_(e_print_error_str),
		buf == ખાલી ? (char_u *)_("Unknown") : buf);
	LocalFree((LPVOID)(buf));
    }
    નહીંતર
	સંદેશ_clr_eos(); // Maybe canceled

    mch_print_cleanup();
    પાછા FALSE;
}


    પૂર્ણાંક
mch_print_begin(prt_settings_T *psettings)
{
    પૂર્ણાંક			ret = 0;
    અક્ષર		szBuffer[300];
    WCHAR		*wp;

    hDlgPrint = CreateDialog(g_hinst, TEXT("PrintDlgBox"),
					     prt_dlg.hwndOwner, PrintDlgProc);
    SetAbortProc(prt_dlg.hDC, AbortProc);
    wsprintf(szBuffer, _("Printing '%s'"), gettail(psettings->jobname));
    vimSetDlgItemText(hDlgPrint, IDC_PRINTTEXT1, (char_u *)szBuffer);

    wp = enc_to_utf16(psettings->jobname, ખાલી);
    જો (wp != ખાલી)
    {
	DOCINFOW	di;

	CLEAR_FIELD(di);
	di.cbSize = કદ(di);
	di.lpszDocName = wp;
	ret = StartDocW(prt_dlg.hDC, &di);
	vim_free(wp);
    }

# ifdef FEAT_GUI
    // Give focus back to main window (when using MDI).
#  ifdef VIMDLL
    જો (gui.in_use)
#  endif
	SetFocus(s_hwnd);
# endif

    પાછા (ret > 0);
}

    શૂન્ય
mch_print_end(prt_settings_T *psettings UNUSED)
{
    EndDoc(prt_dlg.hDC);
    જો (!*bUserAbort && hDlgPrint != ખાલી)
	SendMessage(hDlgPrint, WM_COMMAND, 0, 0);
}

    પૂર્ણાંક
mch_print_end_page(શૂન્ય)
{
    પાછા (EndPage(prt_dlg.hDC) > 0);
}

    પૂર્ણાંક
mch_print_begin_page(char_u *સંદેશ)
{
    જો (સંદેશ != ખાલી)
	vimSetDlgItemText(hDlgPrint, IDC_PROGRESS, સંદેશ);
    પાછા (StartPage(prt_dlg.hDC) > 0);
}

    પૂર્ણાંક
mch_print_blank_page(શૂન્ય)
{
    પાછા (mch_print_begin_page(ખાલી) ? (mch_print_end_page()) : FALSE);
}

સ્થિર પૂર્ણાંક prt_pos_x = 0;
સ્થિર પૂર્ણાંક prt_pos_y = 0;

    શૂન્ય
mch_print_start_લીટી(પૂર્ણાંક margin, પૂર્ણાંક page_લીટી)
{
    જો (margin)
	prt_pos_x = -prt_સંખ્યા_width;
    નહીંતર
	prt_pos_x = 0;
    prt_pos_y = page_લીટી * prt_લીટી_height
				 + prt_tm.tmAscent + prt_tm.tmExternalLeading;
}

    પૂર્ણાંક
mch_print_text_out(char_u *p, પૂર્ણાંક len)
{
    SIZE	sz;
    WCHAR	*wp;
    પૂર્ણાંક		wlen = len;
    પૂર્ણાંક		ret = FALSE;

    wp = enc_to_utf16(p, &wlen);
    જો (wp == ખાલી)
	પાછા FALSE;

    TextOutW(prt_dlg.hDC, prt_pos_x + prt_ડાબું_margin,
	    prt_pos_y + prt_ટોચ_margin, wp, wlen);
    GetTextExtentPoint32W(prt_dlg.hDC, wp, wlen, &sz);
    vim_free(wp);
    prt_pos_x += (sz.cx - prt_tm.tmOverhang);
    // This is wrong when printing spaces for a TAB.
    જો (p[len] != NUL)
    {
	wlen = mb_ptr2len(p + len);
	wp = enc_to_utf16(p + len, &wlen);
	જો (wp != ખાલી)
	{
	    GetTextExtentPoint32W(prt_dlg.hDC, wp, 1, &sz);
	    ret = (prt_pos_x + prt_ડાબું_margin + sz.cx > prt_જમણું_margin);
	    vim_free(wp);
	}
    }
    પાછા ret;
}

    શૂન્ય
mch_print_set_font(પૂર્ણાંક iBold, પૂર્ણાંક iItalic, પૂર્ણાંક iUnderline)
{
    SelectObject(prt_dlg.hDC, prt_font_handles[iBold][iItalic][iUnderline]);
}

    શૂન્ય
mch_print_set_bg(long_u bgcol)
{
    SetBkColor(prt_dlg.hDC, GetNearestColor(prt_dlg.hDC,
						   swap_me((COLORREF)bgcol)));
    /*
     * With a white background we can draw characters transparent, which is
     * good for italic characters that overlap to the next char cell.
     */
    જો (bgcol == 0xffffffUL)
	SetBkMode(prt_dlg.hDC, TRANSPARENT);
    નહીંતર
	SetBkMode(prt_dlg.hDC, OPAQUE);
}

    શૂન્ય
mch_print_set_fg(long_u fgcol)
{
    SetTextColor(prt_dlg.hDC, GetNearestColor(prt_dlg.hDC,
						   swap_me((COLORREF)fgcol)));
}

#endif // FEAT_PRINTER && !FEAT_POSTSCRIPT



#જો defined(FEAT_SHORTCUT)
# include <shlobj.h>

# define is_path_sep(c)	    ((c) == L'\\' || (c) == L'/')

    સ્થિર પૂર્ણાંક
is_reparse_point_included(LPCWSTR fname)
{
    LPCWSTR	p = fname, q;
    WCHAR	buf[MAX_PATH];
    DWORD	attr;

    જો (SAFE_isalpha(p[0]) && p[1] == L':' && is_path_sep(p[2]))
	p += 3;
    નહીંતર જો (is_path_sep(p[0]) && is_path_sep(p[1]))
	p += 2;

    જ્યારે (*p != L'\0')
    {
	q = wcspbrk(p, L"\\/");
	જો (q == ખાલી)
	    p = q = fname + wcslen(fname);
	નહીંતર
	    p = q + 1;
	જો (q - fname >= MAX_PATH)
	    પાછા FALSE;
	wcsncpy(buf, fname, q - fname);
	buf[q - fname] = L'\0';
	attr = GetFileAttributesW(buf);
	જો (attr != INVALID_FILE_ATTRIBUTES
		&& (attr & FILE_ATTRIBUTE_REPARSE_POINT) != 0)
	    પાછા TRUE;
    }
    પાછા FALSE;
}

/*
 * Return the resolved file path, NULL if "fname" is an AppExecLink reparse
 * point, already fully resolved, or it doesn't exists.
 */
    char_u *
resolve_reparse_point(char_u *fname)
{
    HANDLE	    h = INVALID_HANDLE_કિંમત;
    DWORD	    size;
    WCHAR	    *p, *wp;
    char_u	    *rfname = ખાલી;
    WCHAR	    *buff = ખાલી;

    p = enc_to_utf16(fname, ખાલી);
    જો (p == ખાલી)
	જાઓ fail;

    જો (!is_reparse_point_included(p))
    {
	vim_free(p);
	જાઓ fail;
    }

    h = CreateFileW(p, 0, 0, ખાલી, OPEN_EXISTING,
	    FILE_FLAG_BACKUP_SEMANTICS, ખાલી);
    vim_free(p);

    જો (h == INVALID_HANDLE_કિંમત)
	જાઓ fail;

    size = GetFinalPathNameByHandleW(h, ખાલી, 0, 0);
    જો (size == 0)
	જાઓ fail;
    buff = ALLOC_MULT(WCHAR, size);
    જો (buff == ખાલી)
	જાઓ fail;
    જો (GetFinalPathNameByHandleW(h, buff, size, 0) == 0)
	જાઓ fail;

    જો (wcsncmp(buff, L"\\\\?\\UNC\\", 8) == 0)
    {
	buff[6] = L'\\';
	wp = buff + 6;
    }
    નહીંતર જો (wcsncmp(buff, L"\\\\?\\", 4) == 0)
	wp = buff + 4;
    નહીંતર
	wp = buff;

    rfname = utf16_to_enc(wp, ખાલી);

fail:
    જો (h != INVALID_HANDLE_કિંમત)
	CloseHandle(h);
    જો (buff != ખાલી)
	vim_free(buff);

    પાછા rfname;
}

/*
 * When "fname" is the name of a shortcut (*.lnk) resolve the file it points
 * to and return that name in allocated memory.
 * Otherwise NULL is returned.
 */
    સ્થિર char_u *
resolve_shortcut(char_u *fname)
{
    HRESULT		hr;
    IShellLink		*psl = ખાલી;
    IPersistFile	*ppf = ખાલી;
    OLECHAR		wsz[MAX_PATH];
    char_u		*rfname = ખાલી;
    પૂર્ણાંક			len;
    IShellLinkW		*pslw = ખાલી;
    WIN32_FIND_DATAW	ffdw; // we get those free of charge

    // Check if the file name ends in ".lnk". Avoid calling
    // CoCreateInstance(), it's quite slow.
    જો (fname == ખાલી)
	પાછા rfname;
    len = (પૂર્ણાંક)STRLEN(fname);
    જો (len <= 4 || STRNICMP(fname + len - 4, ".lnk", 4) != 0)
	પાછા rfname;

    CoInitialize(ખાલી);

    // create a link manager object and request its interface
    hr = CoCreateInstance(
	    &CLSID_ShellLink, ખાલી, CLSCTX_INPROC_SERVER,
	    &IID_IShellLinkW, (શૂન્ય**)&pslw);
    જો (hr == S_OK)
    {
	WCHAR	*p = enc_to_utf16(fname, ખાલી);

	જો (p != ખાલી)
	{
	    // Get a pointer to the IPersistFile interface.
	    hr = pslw->lpVtbl->QueryInterface(
		    pslw, &IID_IPersistFile, (શૂન્ય**)&ppf);
	    જો (hr != S_OK)
		જાઓ shortcut_errorw;

	    // "load" the name and resolve the link
	    hr = ppf->lpVtbl->Load(ppf, p, STGM_READ);
	    જો (hr != S_OK)
		જાઓ shortcut_errorw;
# જો 0  // This makes Vim wait a long time if the target does not exist.
	    hr = pslw->lpVtbl->Resolve(pslw, ખાલી, SLR_NO_UI);
	    જો (hr != S_OK)
		જાઓ shortcut_errorw;
# endif

	    // Get the path to the link target.
	    ZeroMemory(wsz, MAX_PATH * કદ(WCHAR));
	    hr = pslw->lpVtbl->GetPath(pslw, wsz, MAX_PATH, &ffdw, 0);
	    જો (hr == S_OK && wsz[0] != NUL)
		rfname = utf16_to_enc(wsz, ખાલી);

shortcut_errorw:
	    vim_free(p);
	}
    }

    // Release all interface pointers (both belong to the same object)
    જો (ppf != ખાલી)
	ppf->lpVtbl->Release(ppf);
    જો (psl != ખાલી)
	psl->lpVtbl->Release(psl);
    જો (pslw != ખાલી)
	pslw->lpVtbl->Release(pslw);

    CoUninitialize();
    પાછા rfname;
}

    char_u *
mch_resolve_path(char_u *fname, પૂર્ણાંક reparse_point)
{
    char_u  *path = resolve_shortcut(fname);

    જો (path == ખાલી && reparse_point)
	path = resolve_reparse_point(fname);
    પાછા path;
}
#endif

#જો defined(FEAT_EVAL) && (!defined(FEAT_GUI) || defined(VIMDLL))
/*
 * Bring ourselves to the foreground.  Does work if the OS doesn't allow it.
 */
    શૂન્ય
win32_set_foreground(શૂન્ય)
{
    GetConsoleHwnd();	    // get value of s_hwnd
    જો (s_hwnd != 0)
	SetForegroundWindow(s_hwnd);
}
#endif

#જો defined(FEAT_CLIENTSERVER)
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
# define VIM_CLASSNAME_LEN  (કદ(VIM_CLASSNAME) - 1)

// Timeout for sending a message to another Vim instance.  Normally this works
// instantly, but it may hang when the other Vim instance is halted.
# define SENDMESSAGE_TIMEOUT	(5 * 1000)

// Communication is via WM_COPYDATA messages. The message type is sent in
// the dwData parameter. Types are defined here.
# define COPYDATA_ચાવીઓ		0
# define COPYDATA_REPLY		1
# define COPYDATA_પદાવલિ		10
# define COPYDATA_RESULT	11
# define COPYDATA_ERROR_RESULT	12
# define COPYDATA_ENCODING	20

// This is a structure containing a server HWND and its name.
સંરચના server_id
{
    HWND hwnd;
    char_u *name;
};

// Last received 'encoding' that the client uses.
સ્થિર char_u	*client_enc = ખાલી;

/*
 * Tell the other side what encoding we are using.
 * Return -1 if timeout happens.  Other errors are ignored.
 */
    સ્થિર પૂર્ણાંક
serverSendEnc(HWND target)
{
    COPYDATASTRUCT data;

    data.dwData = COPYDATA_ENCODING;
    data.cbData = (DWORD)STRLEN(p_enc) + 1;
    data.lpData = p_enc;
    જો (SendMessageTimeout(target, WM_COPYDATA,
	    (WPARAM)message_window, (LPARAM)&data,
	    SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, ખાલી) == 0)
	પાછા -1;
    પાછા 0;
}

/*
 * Clean up on exit. This destroys the hidden message window.
 */
    સ્થિર શૂન્ય
CleanUpMessaging(શૂન્ય)
{
    જો (message_window == 0)
	પાછા;

    DestroyWindow(message_window);
    message_window = 0;
}

સ્થિર પૂર્ણાંક save_reply(HWND server, char_u *reply, પૂર્ણાંક પદાવલિ);

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
    સ્થિર LRESULT CALLBACK
Messaging_WndProc(HWND hwnd, UINT સંદેશ, WPARAM wParam, LPARAM lParam)
{
    જો (સંદેશ == WM_COPYDATA)
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
	પૂર્ણાંક		retval;
	DWORD_PTR	dwret = 0;
	char_u		*str;
	char_u		*tofree;

	પસંદગી (data->dwData)
	{
	સ્થિતિ COPYDATA_ENCODING:
	    // Remember the encoding that the client uses.
	    vim_free(client_enc);
	    client_enc = enc_canonize((char_u *)data->lpData);
	    પાછા 1;

	સ્થિતિ COPYDATA_ચાવીઓ:
	    // Remember who sent this, for <client>
	    clientWindow = sender;

	    // Add the received keys to the input buffer.
	    str = serverConvert(client_enc, (char_u *)data->lpData, &tofree);
# ifdef FEAT_GUI
	    જો (gui.in_use)
		// The GUI may get here re-entrantly, so insert the keys later.
		server_add_આગત(str);
	    નહીંતર
# endif
		server_to_આગત_buf(str);
	    vim_free(tofree);

# ifdef FEAT_GUI
	    // Wake up the main GUI loop.
#  ifdef VIMDLL
	    જો (gui.in_use)
#  endif
		જો (s_hwnd != 0)
		    PostMessage(s_hwnd, WM_NULL, 0, 0);
# endif
	    પાછા 1;

	સ્થિતિ COPYDATA_પદાવલિ:
	    // Remember who sent this, for <client>
	    clientWindow = sender;

	    str = serverConvert(client_enc, (char_u *)data->lpData, &tofree);
	    res = eval_client_પદાવલિ_to_string(str);

	    જો (res == ખાલી)
	    {
		અક્ષર	*err = _(e_invalid_expression_received);
		size_t	len = STRLEN(str) + STRLEN(err) + 5;

		res = alloc(len);
		જો (res != ખાલી)
		    reply.cbData = (DWORD)vim_snprintf_safelen(
			(અક્ષર *)res, len, "%s: \"%s\"", err, str);
		નહીંતર
		    reply.cbData = 0;
		reply.dwData = COPYDATA_ERROR_RESULT;
	    }
	    નહીંતર
	    {
		reply.cbData = (DWORD)STRLEN(res) + 1;
		reply.dwData = COPYDATA_RESULT;
	    }
	    reply.lpData = res;

	    જો (serverSendEnc(sender) < 0)
		retval = -1;
	    નહીંતર
	    {
		જો (SendMessageTimeout(sender, WM_COPYDATA,
			(WPARAM)message_window, (LPARAM)&reply,
			SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
		    retval = -1;
		નહીંતર
		    retval = (પૂર્ણાંક)dwret;
	    }
	    vim_free(tofree);
	    vim_free(res);
	    પાછા retval;

	સ્થિતિ COPYDATA_REPLY:
	સ્થિતિ COPYDATA_RESULT:
	સ્થિતિ COPYDATA_ERROR_RESULT:
	    જો (data->lpData != ખાલી)
	    {
		str = serverConvert(client_enc, (char_u *)data->lpData,
								     &tofree);
		જો (tofree == ખાલી)
		    str = vim_strsave(str);
		જો (save_reply(sender, str,
			   (data->dwData == COPYDATA_REPLY ?  0 :
			   (data->dwData == COPYDATA_RESULT ? 1 :
							      2))) == FAIL)
		    vim_free(str);
		નહીંતર જો (data->dwData == COPYDATA_REPLY)
		{
		    char_u	winstr[30];

		    sprintf((અક્ષર *)winstr, PRINTF_HEX_LONG_U, (long_u)sender);
		    apply_autocmds(EVENT_REMOTEREPLY, winstr, str,
								TRUE, curbuf);
		}
	    }
	    પાછા 1;
	}

	પાછા 0;
    }

    નહીંતર જો (સંદેશ == WM_ACTIVATE && wParam == WA_ACTIVE)
    {
	// When the message window is activated (brought to the foreground),
	// this actually applies to the text window.
# જો !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	જો (!gui.in_use)
#  endif
	    GetConsoleHwnd();	    // get value of s_hwnd
# endif
	જો (s_hwnd != 0)
	{
	    SetForegroundWindow(s_hwnd);
	    પાછા 0;
	}
    }

    પાછા DefWindowProc(hwnd, સંદેશ, wParam, lParam);
}

/*
 * Initialise the message handling process.  This involves creating a window
 * to handle messages - the window will not be visible.
 */
    શૂન્ય
serverInitMessaging(શૂન્ય)
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
    wndclass.hIcon = ખાલી;
    wndclass.hCursor = ખાલી;
    wndclass.hbrBackground = ખાલી;
    wndclass.lpszMenuName = ખાલી;
    wndclass.lpszClassName = VIM_CLASSNAME;
    RegisterClass(&wndclass);

    // Create the message window. It will be hidden, so the details don't
    // matter.  Don't use WS_OVERLAPPEDWINDOW, it will make a shortcut remove
    // focus from gvim.
    message_window = CreateWindow(VIM_CLASSNAME, "",
			 WS_POPUPWINDOW | WS_CAPTION,
			 CW_USEDEFAULT, CW_USEDEFAULT,
			 100, 100, ખાલી, ખાલી,
			 g_hinst, ખાલી);
}

// Used by serverSendToVim() to find an alternate server name.
સ્થિર char_u *altname_buf_ptr = ખાલી;

/*
 * Get the title of the window "hwnd", which is the Vim server name, in
 * "name[namelen]" and return the length.
 * Returns zero if window "hwnd" is not a Vim server.
 */
    સ્થિર પૂર્ણાંક
getVimServerName(HWND hwnd, અક્ષર *name, પૂર્ણાંક namelen)
{
    પૂર્ણાંક		len;
    અક્ષર	buffer[VIM_CLASSNAME_LEN + 1];

    // Ignore windows which aren't Vim message windows
    len = GetClassName(hwnd, buffer, કદ(buffer));
    જો (len != VIM_CLASSNAME_LEN || STRCMP(buffer, VIM_CLASSNAME) != 0)
	પાછા 0;

    // Get the title of the window
    પાછા GetWindowText(hwnd, name, namelen);
}

    સ્થિર BOOL CALLBACK
enumWindowsGetServer(HWND hwnd, LPARAM lparam)
{
    સંરચના	server_id *id = (સંરચના server_id *)lparam;
    અક્ષર	server[MAX_PATH];

    // Get the title of the window
    જો (getVimServerName(hwnd, server, કદ(server)) == 0)
	પાછા TRUE;

    // If this is the server we're looking for, return its HWND
    જો (STRICMP(server, id->name) == 0)
    {
	id->hwnd = hwnd;
	પાછા FALSE;
    }

    // If we are looking for an alternate server, remember this name.
    જો (altname_buf_ptr != ખાલી)
    {
	size_t	namelen = STRLEN(id->name);

	જો (STRNICMP(server, id->name, namelen) == 0
	    && vim_isdigit(server[namelen]))
	{
	    STRCPY(altname_buf_ptr, server);
	    altname_buf_ptr = ખાલી;	    // don't use another name
	}
    }

    // Otherwise, keep looking
    પાછા TRUE;
}

    સ્થિર BOOL CALLBACK
enumWindowsGetNames(HWND hwnd, LPARAM lparam)
{
    list_T	*list = (list_T *)lparam;
    અક્ષર	server[MAX_PATH];

    // Get the title of the window
    જો (getVimServerName(hwnd, server, કદ(server)) == 0)
	પાછા TRUE;

    list_append_string(list, (char_u *)server, -1);
    પાછા TRUE;
}

સંરચના enum_windows_s
{
    WNDENUMPROC lpEnumFunc;
    LPARAM      lParam;
};

    સ્થિર BOOL CALLBACK
enum_windows_child(HWND hwnd, LPARAM lParam)
{
    સંરચના enum_windows_s *ew = (સંરચના enum_windows_s *)lParam;

    પાછા (ew->lpEnumFunc)(hwnd, ew->lParam);
}

    સ્થિર BOOL CALLBACK
enum_windows_toplevel(HWND hwnd, LPARAM lParam)
{
    સંરચના enum_windows_s *ew = (સંરચના enum_windows_s *)lParam;

    જો ((ew->lpEnumFunc)(hwnd, ew->lParam))
	પાછા TRUE;
    પાછા EnumChildWindows(hwnd, enum_windows_child, lParam);
}

/*
 * Enumerate all windows including children.
 */
    સ્થિર BOOL
enum_windows(WNDENUMPROC lpEnumFunc, LPARAM lParam)
{
    સંરચના enum_windows_s ew;

    ew.lpEnumFunc = lpEnumFunc;
    ew.lParam = lParam;
    પાછા EnumWindows(enum_windows_toplevel, (LPARAM)&ew);
}

    સ્થિર HWND
findServer(char_u *name)
{
    સંરચના server_id id;

    id.name = name;
    id.hwnd = 0;

    enum_windows(enumWindowsGetServer, (LPARAM)(&id));

    પાછા id.hwnd;
}

    શૂન્ય
serverSetName(char_u *name)
{
    size_t	namelen;
    char_u	*ok_name;
    HWND	hwnd = 0;
    પૂર્ણાંક		i = 0;
    char_u	*p;

    // Leave enough space for a 9-digit suffix to ensure uniqueness!
    namelen = STRLEN(name);
    ok_name = alloc(namelen + 10);
    જો (ok_name == ખાલી)
	પાછા;

    STRCPY(ok_name, name);
    p = ok_name + namelen;

    માટે (;;)
    {
	// This is inefficient - we're doing an EnumWindows loop for each
	// possible name. It would be better to grab all names in one go,
	// and scan the list each time...
	hwnd = findServer(ok_name);
	જો (hwnd == 0)
	    તોડો;

	++i;
	જો (i >= 1000)
	    તોડો;

	sprintf((અક્ષર *)p, "%d", i);
    }

    જો (hwnd != 0)
	vim_free(ok_name);
    નહીંતર
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
serverGetVimNames(શૂન્ય)
{
    list_T *list = list_alloc();

    જો (list == ખાલી)
	પાછા ખાલી;

    enum_windows(enumWindowsGetNames, (LPARAM)list);

    પાછા list;
}

    પૂર્ણાંક
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
    sscanf((અક્ષર *)name, SCANF_HEX_LONG_U, &n);
    જો (n == 0)
	પાછા -1;

    target = (HWND)n;
    જો (!IsWindow(target))
	પાછા -1;

    data.dwData = COPYDATA_REPLY;
    data.cbData = (DWORD)STRLEN(reply) + 1;
    data.lpData = reply;

    જો (serverSendEnc(target) < 0)
	પાછા -1;
    જો (SendMessageTimeout(target, WM_COPYDATA,
		(WPARAM)message_window, (LPARAM)&data,
		SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
	પાછા -1;
    પાછા dwret ? 0 : -1;
}

    પૂર્ણાંક
serverSendToVim(
    char_u	 *name,			// Where to send.
    char_u	 *cmd,			// What to send.
    char_u	 **result,		// Result of eval'ed expression
    શૂન્ય	 *ptarget,		// HWND of server
    પૂર્ણાંક		 as_પદાવલિ,		// Expression or keys?
    પૂર્ણાંક		 timeout,		// timeout in seconds or zero
    પૂર્ણાંક		 silent)		// don't complain about no server
{
    size_t	namelen;
    HWND	target;
    COPYDATASTRUCT data;
    char_u	*retval = ખાલી;
    પૂર્ણાંક		retcode = 0;
    DWORD_PTR	dwret = 0;
    char_u	altname_buf[MAX_PATH];

    // Execute locally if no display or target is ourselves
    જો (serverName != ખાલી && STRICMP(name, serverName) == 0)
	પાછા sendToLocalVim(cmd, as_પદાવલિ, result);

    // If the server name does not end in a digit then we look for an
    // alternate name.  e.g. when "name" is GVIM then we may find GVIM2.
    namelen = STRLEN(name);
    જો (namelen > 1 && !vim_isdigit(name[namelen - 1]))
	altname_buf_ptr = altname_buf;
    altname_buf[0] = NUL;
    target = findServer(name);
    altname_buf_ptr = ખાલી;
    જો (target == 0 && altname_buf[0] != NUL)
	// Use another server name we found.
	target = findServer(altname_buf);

    જો (target == 0)
    {
	જો (!silent)
	    semsg(_(e_no_registered_server_named_str), name);
	પાછા -1;
    }

    જો (ptarget)
	*(HWND *)ptarget = target;

    data.dwData = as_પદાવલિ ? COPYDATA_પદાવલિ : COPYDATA_ચાવીઓ;
    data.cbData = (DWORD)STRLEN(cmd) + 1;
    data.lpData = cmd;

    જો (serverSendEnc(target) < 0)
	પાછા -1;
    જો (SendMessageTimeout(target, WM_COPYDATA,
		(WPARAM)message_window, (LPARAM)&data,
		SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
	પાછા -1;
    જો (dwret == 0)
	પાછા -1;

    જો (as_પદાવલિ)
	retval = serverGetReply(target, &retcode, TRUE, TRUE, timeout);

    જો (result == ખાલી)
	vim_free(retval);
    નહીંતર
	*result = retval; // Caller assumes responsibility for freeing

    પાછા retcode;
}

/*
 * Bring the server to the foreground.
 */
    શૂન્ય
serverForeground(char_u *name)
{
    HWND	target = findServer(name);

    જો (target != 0)
	SetForegroundWindow(target);
}

// Replies from server need to be stored until the client picks them up via
// remote_read(). So we maintain a list of server-id/reply pairs.
// Note that there could be multiple replies from one server pending if the
// client is slow picking them up.
// We just store the replies in a simple list. When we remove an entry, we
// move list entries down to fill the gap.
// The server ID is simply the HWND.
પ્રકારવ્યાખ્યા સંરચના
{
    HWND	server;		// server window
    char_u	*reply;		// reply string
    પૂર્ણાંક		પદાવલિ_result;	// 0 for REPLY, 1 for RESULT 2 for error
} reply_T;

સ્થિર garray_T reply_list = {0, 0, કદ(reply_T), 5, 0};

# define REPLY_ITEM(i) ((reply_T *)(reply_list.ga_data) + (i))
# define REPLY_ગણતરી (reply_list.ga_len)

// Flag which is used to wait for a reply
સ્થિર પૂર્ણાંક reply_received = 0;

/*
 * Store a reply.  "reply" must be allocated memory (or NULL).
 */
    સ્થિર પૂર્ણાંક
save_reply(HWND server, char_u *reply, પૂર્ણાંક પદાવલિ)
{
    reply_T *rep;

    જો (ga_grow(&reply_list, 1) == FAIL)
	પાછા FAIL;

    rep = REPLY_ITEM(REPLY_ગણતરી);
    rep->server = server;
    rep->reply = reply;
    rep->પદાવલિ_result = પદાવલિ;
    જો (rep->reply == ખાલી)
	પાછા FAIL;

    ++REPLY_ગણતરી;
    reply_received = 1;
    પાછા OK;
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
serverGetReply(HWND server, પૂર્ણાંક *પદાવલિ_res, પૂર્ણાંક remove, પૂર્ણાંક wait, પૂર્ણાંક timeout)
{
    પૂર્ણાંક		i;
    char_u	*reply;
    reply_T	*rep;
    પૂર્ણાંક		did_process = FALSE;
    time_t	start;
    time_t	now;

    // When waiting, loop until the message waiting for is received.
    time(&start);
    માટે (;;)
    {
	// Reset this here, in case a message arrives while we are going
	// through the already received messages.
	reply_received = 0;

	માટે (i = 0; i < REPLY_ગણતરી; ++i)
	{
	    rep = REPLY_ITEM(i);
	    જો (rep->server == server
		    && ((rep->પદાવલિ_result != 0) == (પદાવલિ_res != ખાલી)))
	    {
		// Save the values we've found for later
		reply = rep->reply;
		જો (પદાવલિ_res != ખાલી)
		    *પદાવલિ_res = rep->પદાવલિ_result == 1 ? 0 : -1;

		જો (remove)
		{
		    // Move the rest of the list down to fill the gap
		    mch_memmove(rep, rep + 1,
				     (REPLY_ગણતરી - i - 1) * કદ(reply_T));
		    --REPLY_ગણતરી;
		}

		// Return the reply to the caller, who takes on responsibility
		// for freeing it if "remove" is TRUE.
		પાછા reply;
	    }
	}

	// If we got here, we didn't find a reply. Return immediately if the
	// "wait" parameter isn't set.
	જો (!wait)
	{
	    // Process pending messages once. Without this, looping on
	    // remote_peek() would never get the reply.
	    જો (!did_process)
	    {
		did_process = TRUE;
		serverProcessPendingMessages();
		ચાલુ;
	    }
	    તોડો;
	}

	// We need to wait for a reply. Enter a message loop until the
	// "reply_received" flag gets set.

	// Loop until we receive a reply
	જ્યારે (reply_received == 0)
	{
# ifdef FEAT_TIMERS
	    // TODO: use the return value to decide how long to wait.
	    check_due_timer();
# endif
	    time(&now);
	    જો (timeout > 0 && (now - start) >= timeout)
		તોડો;

	    // Wait for a SendMessage() call to us.  This could be the reply
	    // we are waiting for.  Use a timeout of a second, to catch the
	    // situation that the server died unexpectedly.
	    સંદેશ_Wait_For_Multiple_Objects(0, ખાલી, TRUE, 1000, QS_ALLINPUT);

	    // If the server has died, give up
	    જો (!IsWindow(server))
		પાછા ખાલી;

	    serverProcessPendingMessages();
	}
    }

    પાછા ખાલી;
}

/*
 * Process any messages in the Windows message queue.
 */
    શૂન્ય
serverProcessPendingMessages(શૂન્ય)
{
    સંદેશ_2 સંદેશ;

    જ્યારે (PeekMessageW(&સંદેશ, ખાલી, 0, 0, PM_REMOVE))
    {
	TranslateMessage(&સંદેશ);
	DispatchMessageW(&સંદેશ);
    }
}

#endif // FEAT_CLIENTSERVER

#જો defined(FEAT_GUI) || (defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT))

સંરચના charset_pair
{
    string_T	name;
    BYTE	charset;
};

સ્થિર સંરચના charset_pair
charset_pairs[] =
{
    {STR_LITERAL_INIT("ANSI"),		ANSI_CHARSET},
    {STR_LITERAL_INIT("CHINESEBIG5"),	CHINESEBIG5_CHARSET},
    {STR_LITERAL_INIT("DEFAULT"),	DEFAULT_CHARSET},
    {STR_LITERAL_INIT("HANGEUL"),	HANGEUL_CHARSET},
    {STR_LITERAL_INIT("OEM"),		OEM_CHARSET},
    {STR_LITERAL_INIT("SHIFTJIS"),	SHIFTJIS_CHARSET},
    {STR_LITERAL_INIT("SYMBOL"),	પ્રતીક_CHARSET},
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

સંરચના quality_pair
{
    string_T	name;
    DWORD	quality;
};

સ્થિર સંરચના quality_pair
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
    અક્ષર *
charset_id2name(પૂર્ણાંક id)
{
    પૂર્ણાંક	i;

    માટે (i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(charset_pairs); ++i)
    {
	જો ((BYTE)id == charset_pairs[i].charset)
	    પાછા (અક્ષર *)charset_pairs[i].name.string;
    }

    પાછા ખાલી;
}

/*
 * Convert a quality ID to a name.
 * Return NULL when not recognized.
 */
    અક્ષર *
quality_id2name(DWORD id)
{
    પૂર્ણાંક	i;

    માટે (i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(quality_pairs); ++i)
    {
	જો (id == quality_pairs[i].quality)
	    પાછા (અક્ષર *)quality_pairs[i].name.string;
    }

    પાછા ખાલી;
}

// The default font height in 100% scaling (96dpi).
// (-16 in 96dpi equates to roughly 12pt)
# define DEFAULT_FONT_HEIGHT	(-16)

સ્થિર અચલ LOGFONTW s_lfDefault =
{
    DEFAULT_FONT_HEIGHT,
    0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
    PROOF_QUALITY, FIXED_PITCH | FF_DONTCARE,
    L""	// Default font name will be set later based on current language.
};

// This will be initialized when set_default_logfont() is called first time.
// The value will be based on the system DPI.
પૂર્ણાંક current_font_height = 0;		// also used in gui_w32.c

/*
 * Convert a string representing a point size into pixels. The string should
 * be a positive decimal number, with an optional decimal point (eg, "12", or
 * "10.5"). The pixel value is returned, and a pointer to the next unconverted
 * character is stored in *end. The flag "vertical" says whether this
 * calculation is for a vertical (height) size or a horizontal (width) one.
 */
    સ્થિર પૂર્ણાંક
points_to_pixels(WCHAR *str, WCHAR **end, પૂર્ણાંક vertical, long_i pprinter_dc)
{
    પૂર્ણાંક		pixels;
    પૂર્ણાંક		points = 0;
    પૂર્ણાંક		ભાજક = 0;
    HWND	hwnd = (HWND)0;
    HDC		hdc;
    HDC		printer_dc = (HDC)pprinter_dc;

    જ્યારે (*str != NUL)
    {
	જો (*str == L'.' && ભાજક == 0)
	{
	    // Start keeping a divisor, for later
	    ભાજક = 1;
	}
	નહીંતર
	{
	    જો (!VIM_ISDIGIT(*str))
		તોડો;

	    points *= 10;
	    points += *str - L'0';
	    ભાજક *= 10;
	}
	++str;
    }

    જો (ભાજક == 0)
	ભાજક = 1;

    જો (printer_dc == ખાલી)
    {
	hwnd = GetDesktopWindow();
	hdc = GetWindowDC(hwnd);
    }
    નહીંતર
	hdc = printer_dc;

    pixels = MulDiv(points,
		    GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX),
		    72 * ભાજક);

    જો (printer_dc == ખાલી)
	ReleaseDC(hwnd, hdc);

    *end = str;
    પાછા pixels;
}

/*
 * Convert pixel into point size. This is a reverse of points_to_pixels.
 */
    સ્થિર દ્વિગુણ
pixels_to_points(પૂર્ણાંક pixels, પૂર્ણાંક vertical, long_i pprinter_dc)
{
    દ્વિગુણ	points = 0;
    HWND	hwnd = (HWND)0;
    HDC		hdc;
    HDC		printer_dc = (HDC)pprinter_dc;

    જો (printer_dc == ખાલી)
    {
	hwnd = GetDesktopWindow();
	hdc = GetWindowDC(hwnd);
    }
    નહીંતર
	hdc = printer_dc;

    points = pixels * 72.0 / GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX);
    જો (printer_dc == ખાલી)
	ReleaseDC(hwnd, hdc);

    પાછા points;
}

    સ્થિર પૂર્ણાંક CALLBACK
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
    જો ((ntm->tmPitchAndFamily & 1) != 0)
	પાછા 1;
# endif

    // Remember this LOGFONTW as a "possible"
    *lf = elf->elfLogFont;

    // Terminate the scan as soon as we find an ANSI font
    જો (lf->lfCharSet == ANSI_CHARSET
	    || lf->lfCharSet == OEM_CHARSET
	    || lf->lfCharSet == DEFAULT_CHARSET)
	પાછા 0;

    // Continue the scan - we have a non-ANSI font
    પાછા 2;
}

    સ્થિર પૂર્ણાંક
init_logfont(LOGFONTW *lf)
{
    પૂર્ણાંક		n;
    HWND	hwnd = GetDesktopWindow();
    HDC		hdc = GetWindowDC(hwnd);

    n = EnumFontFamiliesW(hdc,
			 lf->lfFaceName,
			 (FONTENUMPROCW)font_enumproc,
			 (LPARAM)lf);

    ReleaseDC(hwnd, hdc);

    // If we couldn't find a usable font, return failure
    જો (n == 1)
	પાછા FAIL;

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
    પાછા OK;
}

/*
 * Call back for EnumFontFamiliesW in expand_font_enumproc.
 *
 */
    સ્થિર પૂર્ણાંક CALLBACK
expand_font_enumproc(
    ENUMLOGFONTW    *elf,
    NEWTEXTMETRICW  *ntm UNUSED,
    DWORD	    type UNUSED,
    LPARAM	    lparam)
{
    LOGFONTW *lf = (LOGFONTW*)elf;

# ifndef FEAT_PROPORTIONAL_FONTS
    // Ignore non-monospace fonts without further ado
    જો ((ntm->tmPitchAndFamily & 1) != 0)
	પાછા 1;
# endif

    // Filter only on ANSI. Otherwise will see a lot of random fonts that we
    // usually don't want.
    જો (lf->lfCharSet != ANSI_CHARSET)
	પાછા 1;

    પૂર્ણાંક (*add_match)(char_u *) = (પૂર્ણાંક (*)(char_u *))lparam;

    WCHAR *faceNameW = lf->lfFaceName;
    char_u *faceName = utf16_to_enc(faceNameW, ખાલી);
    જો (!faceName)
	પાછા 0;

    add_match(faceName);
    vim_free(faceName);

    પાછા 1;
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
    શૂન્ય
gui_mch_expand_font(
    optexpand_T	*args,
    શૂન્ય	*param UNUSED,
    પૂર્ણાંક		(*add_match)(char_u *val))
{
    expand_T	    *xp = args->oe_xp;
    જો (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	અક્ષર buf[30];
	પૂર્ણાંક	i;

	// Always fill in with the current font size as first option for
	// convenience. We simply round to the closest integer for simplicity.
	પૂર્ણાંક font_height = (પૂર્ણાંક)round(
		pixels_to_points(-current_font_height, TRUE, (long_i)ખાલી));
	vim_snprintf(buf, ARRAY_LENGTH(buf), "h%d", font_height);
	add_match((char_u *)buf);

	// Note: Keep this in sync with get_logfont(). Don't include 'c' and
	// 'q' as we fill in all the values below.
	સ્થિર અક્ષર *(p_gfn_win_opt_કિંમતો[]) = {
	    "h" , "w" , "W" , "b" , "i" , "u" , "s"};
	માટે (i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(p_gfn_win_opt_કિંમતો); i++)
	    add_match((char_u *)p_gfn_win_opt_કિંમતો[i]);

	માટે (i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(charset_pairs); ++i)
	{
	    vim_snprintf(buf, કદ(buf), "c%s", charset_pairs[i].name.string);
	    add_match((char_u *)buf);
	}

	માટે (i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(quality_pairs); ++i)
	{
	    vim_snprintf(buf, કદ(buf), "q%s", quality_pairs[i].name.string);
	    add_match((char_u *)buf);
	}
	પાછા;
    }

    HWND	hwnd = GetDesktopWindow();
    HDC		hdc = GetWindowDC(hwnd);

    EnumFontFamiliesW(hdc,
	    ખાલી,
	    (FONTENUMPROCW)expand_font_enumproc,
	    (LPARAM)add_match);

    ReleaseDC(hwnd, hdc);
}

/*
 * Compare a UTF-16 string and an ASCII string literally.
 * Only works all the code points are inside ASCII range.
 */
    સ્થિર પૂર્ણાંક
utf16ascncmp(અચલ WCHAR *w, અચલ અક્ષર *p, size_t n)
{
    size_t i;

    માટે (i = 0; i < n; i++)
    {
	જો (w[i] == 0 || w[i] != p[i])
	    પાછા w[i] - p[i];
    }
    પાછા 0;
}

/*
 * Equivalent of GetDpiForSystem().
 */
    UINT WINAPI
vimGetDpiForSystem(શૂન્ય)
{
    HWND hwnd = GetDesktopWindow();
    HDC hdc = GetWindowDC(hwnd);
    UINT dpi = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(hwnd, hdc);
    પાછા dpi;
}

/*
 * Set default logfont based on current language.
 */
    સ્થિર શૂન્ય
set_default_logfont(LOGFONTW *lf)
{
    // Default font name for current language on MS-Windows.
    // If not translated, falls back to "Consolas".
    // This must be a fixed-pitch font.
    અચલ અક્ષર *defaultfontname = N_("DefaultFontNameForWindows");
    અક્ષર *fontname = _(defaultfontname);

    જો (STRCMP(fontname, defaultfontname) == 0)
	fontname = "Consolas";

    *lf = s_lfDefault;
    lf->lfHeight = DEFAULT_FONT_HEIGHT * (પૂર્ણાંક)vimGetDpiForSystem() / 96;
    જો (current_font_height == 0)
	current_font_height = lf->lfHeight;

    WCHAR *wfontname = enc_to_utf16((char_u*)fontname, ખાલી);
    જો (wfontname != ખાલી)
    {
	wcscpy_s(lf->lfFaceName, LF_FACESIZE, wfontname);
	vim_free(wfontname);
    }
}

/*
 * Get font info from "name" into logfont "lf".
 * Return OK for a valid name, FAIL otherwise.
 */
    પૂર્ણાંક
get_logfont(
    LOGFONTW	*lf,
    char_u	*name,
    HDC		printer_dc,
    પૂર્ણાંક		verbose)
{
    WCHAR	*p;
    પૂર્ણાંક		i;
    પૂર્ણાંક		ret = FAIL;
    સ્થિર LOGFONTW *lastlf = ખાલી;
    WCHAR	*wname;

    set_default_logfont(lf);
    જો (name == ખાલી)
	પાછા OK;

    wname = enc_to_utf16(name, ખાલી);
    જો (wname == ખાલી)
	પાછા FAIL;

    જો (wcscmp(wname, L"*") == 0)
    {
# જો defined(FEAT_GUI_MSWIN)
	CHOOSEFONTW	cf;
	// if name is "*", bring up std font dialog:
	CLEAR_FIELD(cf);
	cf.lStructSize = કદ(cf);
	cf.hwndOwner = s_hwnd;
	cf.Flags = CF_SCREENFONTS | CF_FIXEDPITCHONLY | CF_INITTOLOGFONTSTRUCT;
	જો (lastlf != ખાલી)
	    *lf = *lastlf;
	cf.lpLogFont = lf;
	cf.nFontType = 0 ; //REGULAR_FONTTYPE;
	જો (ChooseFontW(&cf))
	    ret = OK;
# endif
	જાઓ theend;
    }

    /*
     * Split name up, it could be <name>:h<height>:w<width> etc.
     */
    માટે (p = wname; *p && *p != L':'; p++)
    {
	જો (p - wname + 1 >= LF_FACESIZE)
	    જાઓ theend;			// Name too long
	lf->lfFaceName[p - wname] = *p;
    }
    જો (p != wname)
	lf->lfFaceName[p - wname] = NUL;

    // First set defaults
    lf->lfHeight = DEFAULT_FONT_HEIGHT * (પૂર્ણાંક)vimGetDpiForSystem() / 96;
    lf->lfWidth = 0;
    lf->lfWeight = FW_NORMAL;
    lf->lfItalic = FALSE;
    lf->lfUnderline = FALSE;
    lf->lfStrikeOut = FALSE;

    /*
     * If the font can't be found, try replacing '_' by ' '.
     */
    જો (init_logfont(lf) == FAIL)
    {
	પૂર્ણાંક	did_replace = FALSE;

	માટે (i = 0; lf->lfFaceName[i]; ++i)
	    જો (lf->lfFaceName[i] == L'_')
	    {
		lf->lfFaceName[i] = L' ';
		did_replace = TRUE;
	    }
	જો (!did_replace || init_logfont(lf) == FAIL)
	    જાઓ theend;
    }

    જ્યારે (*p == L':')
	p++;

    // Set the values found after ':'
    જ્યારે (*p)
    {
	પસંદગી (*p++)
	{
	    // Note: Keep this in sync with gui_mch_expand_font().
	    સ્થિતિ L'h':
		lf->lfHeight = - points_to_pixels(p, &p, TRUE, (long_i)printer_dc);
		તોડો;
	    સ્થિતિ L'w':
		lf->lfWidth = points_to_pixels(p, &p, FALSE, (long_i)printer_dc);
		તોડો;
	    સ્થિતિ L'W':
		lf->lfWeight = wcstol(p, &p, 10);
		તોડો;
	    સ્થિતિ L'b':
		lf->lfWeight = FW_BOLD;
		તોડો;
	    સ્થિતિ L'i':
		lf->lfItalic = TRUE;
		તોડો;
	    સ્થિતિ L'u':
		lf->lfUnderline = TRUE;
		તોડો;
	    સ્થિતિ L's':
		lf->lfStrikeOut = TRUE;
		તોડો;
	    સ્થિતિ L'c':
		માટે (i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(charset_pairs); ++i)
		{
		    જો (utf16ascncmp(p, (અક્ષર *)charset_pairs[i].name.string,
			    charset_pairs[i].name.length) == 0)
		    {
			lf->lfCharSet = charset_pairs[i].charset;
			p += charset_pairs[i].name.length;
			તોડો;
		    }
		}

		જો (i == (પૂર્ણાંક)ARRAY_LENGTH(charset_pairs) && verbose)
		{
		    char_u *s = utf16_to_enc(p, ખાલી);
		    જો (s != ખાલી)
		    {
			semsg(_(e_illegal_str_name_str_in_font_name_str),
							   "charset", s, name);
			vim_free(s);
		    }
		}
		તોડો;
	    સ્થિતિ L'f':
		// Font features (e.g., "fss19=1").
		// Parsed separately by gui_mch_init_font(); skip here.
		જ્યારે (*p && *p != L':')
		    p++;
		તોડો;
	    સ્થિતિ L'q':
		માટે (i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(quality_pairs); ++i)
		{
		    જો (utf16ascncmp(p, (અક્ષર *)quality_pairs[i].name.string,
			    quality_pairs[i].name.length) == 0)
		    {
			lf->lfQuality = quality_pairs[i].quality;
			p += quality_pairs[i].name.length;
			તોડો;
		    }
		}

		જો (i == (પૂર્ણાંક)ARRAY_LENGTH(quality_pairs) && verbose)
		{
		    char_u *s = utf16_to_enc(p, ખાલી);
		    જો (s != ખાલી)
		    {
			semsg(_(e_illegal_str_name_str_in_font_name_str),
							   "quality", s, name);
			vim_free(s);
		    }
		}
		તોડો;
	    મૂળભૂત:
		જો (verbose)
		    semsg(_(e_illegal_char_nr_in_font_name_str), p[-1], name);
		જાઓ theend;
	}
	જ્યારે (*p == L':')
	    p++;
    }
    ret = OK;

theend:
    // ron: init lastlf
    જો (ret == OK && printer_dc == ખાલી)
    {
	vim_free(lastlf);
	lastlf = ALLOC_ONE(LOGFONTW);
	જો (lastlf != ખાલી)
	    mch_memmove(lastlf, lf, કદ(LOGFONTW));
    }
    vim_free(wname);

    પાછા ret;
}

#endif // defined(FEAT_GUI) || defined(FEAT_PRINTER)

#જો defined(FEAT_JOB_CHANNEL)
/*
 * Initialize the Winsock dll.
 */
    શૂન્ય
channel_init_winsock(શૂન્ય)
{
    WSADATA wsaData;
    પૂર્ણાંક wsaerr;

    જો (WSInitialized)
	પાછા;

    wsaerr = WSAStartup(MAKEWORD(2, 2), &wsaData);
    જો (wsaerr == 0)
	WSInitialized = TRUE;
}
#endif
