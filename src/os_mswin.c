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

#شامل "vim.h"

#شامل <sys/types.h>
#شامل <limits.h>

#شامل <process.h>
#شامل <direct.h>

#اگر !defined(FEAT_GUI_MSWIN)
# include <shellapi.h>
#endif

#اگر defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT)
# include <dlgs.h>
# include <winspool.h>
# include <commdlg.h>
#endif

// Record all output and all keyboard & mouse input
// #define MCH_WRITE_DUMP

#ifdef MCH_WRITE_DUMP
FILE* fdDump = پوچ;
#endif

#اگر !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
بیرونی نویسه g_szOrigTitle[];
#endif

#ifdef FEAT_GUI
بیرونی HWND s_hwnd;
#وگرنه
ایستا HWND s_hwnd = 0;	    // console window handle, set by GetConsoleHwnd()
#endif

#ifdef FEAT_JOB_CHANNEL
صحیح WSInitialized = FALSE; // WinSock is initialized
#endif


/*
 * Save the instance handle of the exe/dll.
 */
    تهی
SaveInst(HINSTANCE hInst)
{
    g_hinst = hInst;
}

#اگر defined(FEAT_GUI_MSWIN)
/*
 * GUI version of mch_exit().
 * Shut down and exit with status `r'
 * Careful: mch_exit() may be called before mch_init()!
 */
    تهی
mch_exit_g(صحیح r)
{
    exiting = TRUE;

    display_errors();

    ml_close_all(TRUE);		// remove all memfiles

# ifdef FEAT_OLE
    UninitOLE();
# endif
# ifdef FEAT_JOB_CHANNEL
    اگر (WSInitialized)
    {
	WSInitialized = FALSE;
	WSACleanup();
    }
# endif
# ifdef DYNAMIC_GETTEXT
    dyn_libintl_end();
# endif

    اگر (gui.in_use)
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
تعریفنوع BOOL (WINAPI *PfnRtlGetVersion)(LPOSVERSIONINFOW);
DWORD win_version;
    ایستا تهی
win_version_init(تهی)
{
    OSVERSIONINFOW	osver;
    HMODULE		hNtdll;
    PfnRtlGetVersion	pRtlGetVersion;

    hNtdll = GetModuleHandle("ntdll.dll");
    اگر (hNtdll == پوچ)
	بازگشت;

    pRtlGetVersion =
	(PfnRtlGetVersion) GetProcAddress(hNtdll, "RtlGetVersion");
    اگر (pRtlGetVersion == پوچ)
	بازگشت;

    osver.dwOSVersionInfoSize = اندازه(OSVERSIONINFOW);
    pRtlGetVersion(&osver);
    win_version = MAKE_VER(min(osver.dwMajorVersion, 0xFF),
	    min(osver.dwMinorVersion, 0xFF),
	    min(osver.dw_Build_عدد, 0xFFFF));
}

/*
 * Init the tables for toupper() and tolower().
 */
    تهی
mch_early_init(تهی)
{
    صحیح		i;

    win_version_init();

    PlatformId();

    // Init the tables for toupper() and tolower()
    برای (i = 0; i < 256; ++i)
	toupper_tab[i] = tolower_tab[i] = i;
    CharUpperBuff((LPSTR)toupper_tab, 256);
    CharLowerBuff((LPSTR)tolower_tab, 256);
}


/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    صحیح
mch_درون_داد_isatty(تهی)
{
#ifdef FEAT_GUI_MSWIN
# ifdef VIMDLL
    اگر (gui.in_use)
# endif
	بازگشت TRUE;	    // GUI always has a tty
#endif
#اگر !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    اگر (isatty(read_cmd_fd))
	بازگشت TRUE;
    بازگشت FALSE;
#endif
}

/*
 * mch_settitle(): set titlebar of our window
 */
    تهی
mch_settitle(
    char_u *title,
    char_u *icon UNUSED)
{
#ifdef FEAT_GUI_MSWIN
# ifdef VIMDLL
    اگر (gui.in_use)
# endif
    {
	gui_mch_settitle(title, icon);
	بازگشت;
    }
#endif
#اگر !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    اگر (title != پوچ)
    {
	WCHAR	*wp = enc_to_utf16(title, پوچ);

	اگر (wp == پوچ)
	    بازگشت;

	SetConsoleTitleW(wp);
	vim_free(wp);
	بازگشت;
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
    تهی
mch_restore_title(صحیح which UNUSED)
{
#اگر !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
    اگر (!gui.in_use)
# endif
	SetConsoleTitle(g_szOrigTitle);
#endif
}


/*
 * Return TRUE if we can restore the title (we can)
 */
    صحیح
mch_can_restore_title(تهی)
{
    بازگشت TRUE;
}


/*
 * Return TRUE if we can restore the icon title (we can't)
 */
    صحیح
mch_can_restore_icon(تهی)
{
    بازگشت FALSE;
}


/*
 * Get absolute file name into buffer "buf" of length "len" bytes,
 * turning all '/'s into '\\'s and getting the correct case of each component
 * of the file name.  Append a (back)slash to a directory name.
 * When 'shellslash' set do it the other way around.
 * Return OK or FAIL.
 */
    صحیح
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    صحیح		len,
    صحیح		force UNUSED)
{
    صحیح		nResult = FAIL;
    WCHAR	*wname;
    WCHAR	wbuf[MAX_PATH];
    char_u	*cname = پوچ;

    wname = enc_to_utf16(fname, پوچ);
    اگر (wname != پوچ && _wfullpath(wbuf, wname, MAX_PATH) != پوچ)
    {
	cname = utf16_to_enc((short_u *)wbuf, پوچ);
	اگر (cname != پوچ)
	{
	    vim_strncpy(buf, cname, len - 1);
	    nResult = OK;
	}
    }
    vim_free(wname);
    vim_free(cname);

#ifdef USE_FNAME_CASE
    fname_case(buf, len);
#وگرنه
    slash_adjust(buf);
#endif

    بازگشت nResult;
}


/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    صحیح
mch_isFullName(char_u *fname)
{
    // A name like "d:/foo" and "//server/share" is absolute.  "d:foo" is not.
    // /foo and \foo are absolute too because Windows keeps a current drive.
    // Another way to check is to use mch_FullName() and see if the result is
    // the same as the name or mch_FullName() fails.  However, this has quite a
    // bit of overhead, so let's not do that.
    اگر (*fname == NUL)
	بازگشت FALSE;
    بازگشت ((ASCII_ISALPHA(fname[0]) && fname[1] == ':'
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
    تهی
slash_adjust(char_u *p)
{
    اگر (path_with_url(p))
	بازگشت;

    اگر (*p == '`')
    {
	size_t len = STRLEN(p);

	// don't replace backslash in backtick quoted strings
	اگر (len > 2 && *(p + len - 1) == '`')
	    بازگشت;
    }

    وقتی (*p)
    {
	اگر (*p == psepcN)
	    *p = psepc;
	MB_PTR_ADV(p);
    }
}

    ایستا صحیح
read_reparse_point(ثابت WCHAR *name, char_u *buf, DWORD *buf_len)
{
    HANDLE h;
    BOOL ok;

    h = CreateFileW(name, FILE_READ_ATTRIBUTES,
	    FILE_SHARE_READ | FILE_SHARE_WRITE, پوچ,
	    OPEN_EXISTING,
	    FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,
	    پوچ);
    اگر (h == INVALID_HANDLE_مقدار)
	بازگشت FAIL;

    ok = DeviceIoControl(h, FSCTL_GET_REPARSE_POINT, پوچ, 0, buf, *buf_len,
	    buf_len, پوچ);
    CloseHandle(h);

    بازگشت ok ? OK : FAIL;
}

    char_u *
resolve_appexeclink(char_u *fname)
{
    DWORD		attr = 0;
    صحیح			idx;
    WCHAR		*p, *end, *wname;
    // The buffer size is arbitrarily chosen to be "big enough" (TM), the
    // ceiling should be around 16k.
    char_u		buf[4096];
    DWORD		buf_len = اندازه(buf);
    REPARSE_DATA_BUFFER *rb = (REPARSE_DATA_BUFFER *)buf;

    wname = enc_to_utf16(fname, پوچ);
    اگر (wname == پوچ)
	بازگشت پوچ;

    attr = GetFileAttributesW(wname);
    اگر (attr == INVALID_FILE_ATTRIBUTES ||
	    (attr & FILE_ATTRIBUTE_REPARSE_POINT) == 0)
    {
	vim_free(wname);
	بازگشت پوچ;
    }

    // The applinks are similar to symlinks but with a huge difference: they can
    // only be executed, any other I/O operation on them is bound to fail with
    // ERROR_FILE_NOT_FOUND even though the file exists.
    اگر (read_reparse_point(wname, buf, &buf_len) == FAIL)
    {
	vim_free(wname);
	بازگشت پوچ;
    }
    vim_free(wname);

    اگر (rb->ReparseTag != IO_REPARSE_TAG_APPEXECLINK)
	بازگشت پوچ;

    // The (undocumented) reparse buffer contains a set of N null-terminated
    // Unicode strings, the application path is stored in the third one.
    اگر (rb->AppExecLinkReparseBuffer.String_شمار < 3)
	بازگشت پوچ;

    p = rb->AppExecLinkReparseBuffer.StringList;
    end = p + rb->ReparseDataLength / اندازه(WCHAR);
    برای (idx = 0; p < end
	    && idx < (صحیح)rb->AppExecLinkReparseBuffer.String_شمار
	    && idx != 2; )
    {
	اگر (*p++ == L'\0')
	    ++idx;
    }

    بازگشت utf16_to_enc(p, پوچ);
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
    ایستا صحیح
mswin_stat_impl(ثابت WCHAR *name, stat_T *stp, ثابت صحیح resolve)
{
    صحیح			n;
    صحیح			fd;
    BOOL		is_symlink = FALSE;
    HANDLE		hFind, h;
    DWORD		attr = 0;
    DWORD		flag = 0;
    WIN32_FIND_DATAW    findDataW;

#اگر 0 && defined(_UCRT)
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
    اگر (resolve)
	// Universal CRT can handle symlinks properly.
	بازگشت _wstat(name, stp);
#endif

    // Only reparse points need a directory query to identify symbolic links.
    // In particular, avoid enumerating the directory for each missing file
    // while searching 'runtimepath'.
    attr = GetFileAttributesW(name);
    اگر (attr == INVALID_FILE_ATTRIBUTES)
    {
	DWORD error = GetLastError();

	// A missing file or path cannot provide stat information.  Other
	// errors and UNC paths still need the CRT fallback.
	اگر ((error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND)
		&& !((name[0] == L'\\' || name[0] == L'/')
		    && (name[1] == L'\\' || name[1] == L'/')))
	{
	    errno = ENOENT;
	    بازگشت -1;
	}
	بازگشت _wstat(name, stp);
    }
    اگر (!(attr & FILE_ATTRIBUTE_REPARSE_POINT))
	بازگشت _wstat(name, stp);

    hFind = FindFirstFileW(name, &findDataW);
    اگر (hFind != INVALID_HANDLE_مقدار)
    {
	attr = findDataW.dwFileAttributes;
	اگر ((attr & FILE_ATTRIBUTE_REPARSE_POINT)
		&& (findDataW.dwReserved0 == IO_REPARSE_TAG_SYMLINK))
	    is_symlink = TRUE;
	FindClose(hFind);
    }

    // Use the plain old stat() whenever it's possible.
    اگر (!is_symlink)
	بازگشت _wstat(name, stp);

    اگر (!resolve && is_symlink)
	flag = FILE_FLAG_OPEN_REPARSE_POINT;
    اگر (attr & FILE_ATTRIBUTE_DIRECTORY)
	flag |= FILE_FLAG_BACKUP_SEMANTICS;

    h = CreateFileW(name, FILE_READ_ATTRIBUTES,
	    FILE_SHARE_READ | FILE_SHARE_WRITE, پوچ, OPEN_EXISTING, flag,
	    پوچ);
    اگر (h == INVALID_HANDLE_مقدار)
	بازگشت -1;

    fd = _open_osfhandle((intptr_t)h, _O_RDONLY);
    n = _fstat(fd, (ساختار _stat *)stp);
    اگر ((n == 0) && (attr & FILE_ATTRIBUTE_DIRECTORY))
	stp->st_mode = (stp->st_mode & ~S_IFMT) | S_IFDIR;
    _close(fd);

    بازگشت n;
}

/*
 * stat() can't handle a trailing '/' or '\', remove it first.
 * When 'resolve' is true behave as lstat() wrt symlinks.
 */
    ایستا صحیح
stat_impl(ثابت نویسه *name, stat_T *stp, ثابت صحیح resolve)
{
    // WinNT and later can use _MAX_PATH wide characters for a pathname, which
    // means that the maximum pathname is _MAX_PATH * 3 bytes when 'enc' is
    // UTF-8.
    char_u	buf[_MAX_PATH * 3 + 1];
    size_t	buflen;
    char_u	*p;
    WCHAR	*wp;
    صحیح		n;

    vim_strncpy((char_u *)buf, (char_u *)name, اندازه(buf) - 1);
    buflen = STRLEN(buf);
    p = buf + buflen;
    اگر (p > buf)
	MB_PTR_BACK(buf, p);

    // Remove trailing '\\' except root path.
    اگر (p > buf && (*p == '\\' || *p == '/') && p[-1] != ':')
    {
	*p = NUL;
	buflen = (size_t)(p - buf);
    }

    اگر ((buf[0] == '\\' && buf[1] == '\\') || (buf[0] == '/' && buf[1] == '/'))
    {
	// UNC root path must be followed by '\\'.
	p = vim_strpbrk(buf + 2, (char_u *)"\\/");
	اگر (p != پوچ)
	{
	    p = vim_strpbrk(p + 1, (char_u *)"\\/");
	    اگر (p == پوچ)
		STRCPY(buf + buflen, "\\");
	}
    }

    wp = enc_to_utf16(buf, پوچ);
    اگر (wp == پوچ)
	بازگشت -1;

    n = mswin_stat_impl(wp, stp, resolve);
    vim_free(wp);
    بازگشت n;
}

    صحیح
vim_lstat(ثابت نویسه *name, stat_T *stp)
{
    بازگشت stat_impl(name, stp, FALSE);
}

    صحیح
vim_stat(ثابت نویسه *name, stat_T *stp)
{
    بازگشت stat_impl(name, stp, TRUE);
}

#اگر defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    تهی
mch_settmode(tmode_T tmode UNUSED)
{
    // nothing to do
}

    صحیح
mch_get_shellsize(تهی)
{
    // never used
    بازگشت OK;
}

    تهی
mch_set_shellsize(تهی)
{
    // never used
}

/*
 * Rows and/or Columns has changed.
 */
    تهی
mch_new_shellsize(تهی)
{
    // never used
}

    تهی
mch_calc_cell_size(ساختار cellsize *cs_out UNUSED)
{
    // never used
}

#endif

/*
 * We have no job control, so fake it by starting a new shell.
 */
    تهی
mch_suspend(تهی)
{
    suspend_shell();
}

#اگر defined(USE_MCH_ERRMSG)

# ifdef display_errors
#  undef display_errors
# endif

/*
 * Display the saved error message(s).
 */
    تهی
display_errors(تهی)
{
# ifdef FEAT_GUI
    char_u *p;

#  ifdef VIMDLL
    اگر (gui.in_use || gui.starting)
#  endif
    {
	اگر (error_ga.ga_data != پوچ)
	{
	    // avoid putting up a message box with blanks only
	    برای (p = (char_u *)error_ga.ga_data; *p; ++p)
		اگر (!SAFE_isspace(*p))
		{
		    // Only use a dialog when not using --gui-dialog-file:
		    // write text to a file.
		    اگر (!gui_dialog_log((char_u *)"Errors", p))
			(تهی)gui_mch_dialog(
				     gui.starting ? VIM_INFO :
					     VIM_ERROR,
				     gui.starting ? (char_u *)_("Message") :
					     (char_u *)_("Error"),
					     p, (char_u *)_("&Ok"),
					1, پوچ, FALSE);
		    بشکن;
		}
	    ga_clear(&error_ga);
	}
	بازگشت;
    }
# endif
# اگر !defined(FEAT_GUI) || defined(VIMDLL)
    FlushFileBuffers(GetStdHandle(STD_ERROR_HANDLE));
# endif
}
#endif


/*
 * Return TRUE if "p" contain a wildcard that can be expanded by
 * dos_expandpath().
 */
    صحیح
mch_has_exp_wildcard(char_u *p)
{
    برای ( ; *p; MB_PTR_ADV(p))
    {
	اگر (vim_strchr((char_u *)"?*[", *p) != پوچ
		|| (*p == '~' && p[1] != NUL))
	    بازگشت TRUE;
    }
    بازگشت FALSE;
}

/*
 * Return TRUE if "p" contain a wildcard or a "~1" kind of thing (could be a
 * shortened file name).
 */
    صحیح
mch_has_wildcard(char_u *p)
{
    برای ( ; *p; MB_PTR_ADV(p))
    {
	اگر (vim_strchr((char_u *)"?*$[`", *p) != پوچ
		|| (*p == '~' && p[1] != NUL))
	    بازگشت TRUE;
    }
    بازگشت FALSE;
}


/*
 * The normal _chdir() does not change the default drive.  This one does.
 * Returning 0 implies success; -1 implies failure.
 */
    صحیح
mch_chdir(نویسه *path)
{
    WCHAR   *p;
    صحیح	    n;

    اگر (path[0] == NUL)		// just checking...
	بازگشت -1;

    اگر (p_verbose >= 5)
    {
	verbose_enter();
	smsg("chdir(%s)", path);
	verbose_leave();
    }
    اگر (SAFE_isalpha(path[0]) && path[1] == ':')	// has a drive name
    {
	// If we can change to the drive, skip that part of the path.  If we
	// can't then the current directory may be invalid, try using chdir()
	// with the whole path.
	اگر (_chdrive(TOLOWER_ASC(path[0]) - 'a' + 1) == 0)
	    path += 2;
    }

    اگر (*path == NUL)		// drive name only
	بازگشت 0;

    p = enc_to_utf16((char_u *)path, پوچ);
    اگر (p == پوچ)
	بازگشت -1;

    n = _wchdir(p);
    vim_free(p);
    بازگشت n;
}


#اگر defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
/*
 * return non-zero if a character is available
 */
    صحیح
mch_char_avail(تهی)
{
    // never used
    بازگشت TRUE;
}

# اگر defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    صحیح
mch_check_messages(تهی)
{
    // TODO: check for messages
    بازگشت TRUE;
}
# endif
#endif


#اگر defined(FEAT_LIBCALL)
/*
 * Call a DLL routine which takes either a string or int param
 * and returns an allocated string.
 * Return OK if it worked, FAIL if not.
 */
تعریفنوع char_u *(*MYSTRPROCSTR)(char_u *);
تعریفنوع صحیح (*MYSTRPROCINT)(char_u *);
تعریفنوع char_u *(*MYINTPROCSTR)(صحیح);
تعریفنوع صحیح (*MYINTPROCINT)(صحیح);

/*
 * Check if a pointer points to a valid NUL terminated string.
 * Return the length of the string, including terminating NUL.
 * Returns 0 for an invalid pointer, 1 for an empty string.
 */
    ایستا size_t
check_str_len(char_u *str)
{
    SYSTEM_INFO			si;
    MEMORY_BASIC_INFORMATION	mbi;
    size_t			length = 0;
    size_t			i;
    ثابت char_u		*p;

    // get page size
    GetSystemInfo(&si);

    // get memory information
    اگر (!VirtualQuery(str, &mbi, اندازه(mbi)))
	بازگشت 0;

    // pre cast these (typing savers)
    long_u dwStr = (long_u)str;
    long_u dwBaseAddress = (long_u)mbi.BaseAddress;

    // get start address of page that str is on
    long_u strPage = dwStr - (dwStr - dwBaseAddress) % si.dwPageSize;

    // get length from str to end of page
    long_u pageLength = si.dwPageSize - (dwStr - strPage);

    برای (p = str; !IsBadReadPtr(p, (UINT)pageLength);
	    p += pageLength, pageLength = si.dwPageSize)
	برای (i = 0; i < pageLength; ++i, ++length)
	    اگر (p[i] == NUL)
		بازگشت length + 1;

    بازگشت 0;
}

/*
 * Passed to do_in_runtimepath() to load a vim.ico file.
 */
    ایستا تهی
mch_icon_load_cb(char_u *fname, تهی *cookie)
{
    HANDLE *h = (HANDLE *)cookie;

    *h = LoadImage(پوچ,
		   (LPSTR)fname,
		   IMAGE_ICON,
		   64,
		   64,
		   LR_LOADFROMFILE | LR_LOADMAP3DCOLORS);
}

/*
 * Try loading an icon file from 'runtimepath'.
 */
    صحیح
mch_icon_load(HANDLE *iconp)
{
    بازگشت do_in_runtimepath((char_u *)"bitmaps/vim.ico",
						  0, mch_icon_load_cb, iconp);
}

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if the OS PRNG is not available or something went wrong.
 */
    صحیح
mch_get_random(char_u *buf, صحیح len)
{
    ایستا صحیح		initialized = NOTDONE;
    ایستا HINSTANCE	hInstLib;
    ایستا BOOL (WINAPI *pProcessPrng)(PUCHAR, ULONG);

    اگر (initialized == NOTDONE)
    {
	hInstLib = vimLoadLib("bcryptprimitives.dll");
	اگر (hInstLib != پوچ)
	    pProcessPrng = (تهی *)GetProcAddress(hInstLib, "ProcessPrng");
	اگر (hInstLib == پوچ || pProcessPrng == پوچ)
	{
	    FreeLibrary(hInstLib);
	    initialized = FAIL;
	}
	وگرنه
	    initialized = OK;
    }

    اگر (initialized == FAIL)
	بازگشت FAIL;

    // According to the documentation this call cannot fail.
    pProcessPrng(buf, len);

    بازگشت OK;
}

    صحیح
mch_libcall(
    char_u	*libname,
    char_u	*funcname,
    char_u	*argstring,	// NULL when using a argint
    صحیح		argint,
    char_u	**string_result,// NULL when using number_result
    صحیح		*عدد_result)
{
    HINSTANCE		hinstLib;

    BOOL fRunTimeLinkSuccess = FALSE;

    // Get a handle to the DLL module.
    hinstLib = vimLoadLib((نویسه *)libname);

    // If the handle is valid, try to get the function address.
    اگر (hinstLib != پوچ)
    {
	char_u	*retval_str = پوچ;
	صحیح	retval_int = 0;
	size_t	len;

# ifdef HAVE_TRY_EXCEPT
	__try
	{
# endif
	اگر (argstring != پوچ)
	{
	    // Call with string argument
	    اگر (string_result != پوچ)
	    {
		MYSTRPROCSTR proc;

		proc = (MYSTRPROCSTR)GetProcAddress(hinstLib, (LPCSTR)funcname);
		اگر (proc != پوچ)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_str = proc(argstring);
		}
	    }
	    وگرنه
	    {
		MYSTRPROCINT proc;

		proc = (MYSTRPROCINT)GetProcAddress(hinstLib, (LPCSTR)funcname);
		اگر (proc != پوچ)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_int = proc(argstring);
		}
	    }
	}
	وگرنه
	{
	    // Call with number argument
	    اگر (string_result != پوچ)
	    {
		MYINTPROCSTR proc;

		proc = (MYINTPROCSTR)GetProcAddress(hinstLib, (LPCSTR)funcname);
		اگر (proc != پوچ)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_str = proc(argint);
		}
	    }
	    وگرنه
	    {
		MYINTPROCINT proc;

		proc = (MYINTPROCINT)GetProcAddress(hinstLib, (LPCSTR)funcname);
		اگر (proc != پوچ)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_int = proc(argint);
		}
	    }
	}

	// Save the string before we free the library.
	// Assume that a "1" result is an illegal pointer.
	اگر (string_result == پوچ)
	    *عدد_result = retval_int;
	وگرنه اگر (retval_str != پوچ
		&& (len = check_str_len(retval_str)) > 0)
	{
	    *string_result = alloc(len);
	    اگر (*string_result != پوچ)
		mch_memmove(*string_result, retval_str, len);
	}

# ifdef HAVE_TRY_EXCEPT
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	    اگر (GetExceptionCode() == EXCEPTION_پشته_OVERFLOW)
		_resetstkoflw();
	    fRunTimeLinkSuccess = FALSE;
	}
# endif

	// Free the DLL module.
	(تهی)FreeLibrary(hinstLib);
    }

    اگر (!fRunTimeLinkSuccess)
    {
	semsg(_(e_library_call_failed_for_str), funcname);
	بازگشت FAIL;
    }

    بازگشت OK;
}
#endif

/*
 * Debugging helper: expose the MCH_WRITE_DUMP stuff to other modules
 */
    تهی
DumpPutS(ثابت نویسه *psz UNUSED)
{
#ifdef MCH_WRITE_DUMP
    اگر (fdDump)
    {
	fputs(psz, fdDump);
	اگر (psz[strlen(psz) - 1] != '\n')
	    fputc('\n', fdDump);
	fflush(fdDump);
    }
#endif
}

#ifdef _DEBUG

تهی __cdecl
Trace(
    نویسه *pszFormat,
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

#اگر !defined(FEAT_GUI) || defined(VIMDLL)
بیرونی HWND g_hWnd;	// This is in os_win32.c.

/*
 * Showing the printer dialog is tricky since we have no GUI
 * window to parent it. The following routines are needed to
 * get the window parenting and Z-order to work properly.
 */
    ایستا تهی
GetConsoleHwnd(تهی)
{
    // Skip if it's already set.
    اگر (s_hwnd != 0)
	بازگشت;

    // Window handle may have been found by init code (Windows NT only)
    اگر (g_hWnd != 0)
    {
	s_hwnd = g_hWnd;
	بازگشت;
    }

    s_hwnd = GetConsoleWindow();
}

/*
 * Console implementation of ":winpos".
 */
    صحیح
mch_get_winpos(صحیح *x, صحیح *y)
{
    RECT  rect;

    GetConsoleHwnd();
    GetWindowRect(s_hwnd, &rect);
    *x = rect.چپ;
    *y = rect.بالاترین;
    بازگشت OK;
}

/*
 * Console implementation of ":winpos x y".
 */
    تهی
mch_set_winpos(صحیح x, صحیح y)
{
    GetConsoleHwnd();
    SetWindowPos(s_hwnd, پوچ, x, y, 0, 0,
		 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}
#endif

#اگر defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT)

//=================================================================
// Win32 printer stuff

ایستا HFONT		prt_font_handles[2][2][2];
ایستا PRINTDLGW	prt_dlg;
ایستا ثابت صحیح	boldface[2] = {FW_REGULAR, FW_BOLD};
ایستا TEXTMETRIC	prt_tm;
ایستا صحیح		prt_سطر_height;
ایستا صحیح		prt_عدد_width;
ایستا صحیح		prt_چپ_margin;
ایستا صحیح		prt_راست_margin;
ایستا صحیح		prt_بالاترین_margin;
ایستا char_u		szAppName[] = TEXT("VIM");
ایستا HWND		hDlgPrint;
ایستا صحیح		*bUserAbort = پوچ;
ایستا char_u		*prt_name = پوچ;

// Defines which are also in vim.rc.
# define IDC_BOX1		400
# define IDC_PRINTTEXT1		401
# define IDC_PRINTTEXT2		402
# define IDC_PROGRESS		403

    ایستا BOOL
vimSetDlgItemText(HWND hDlg, صحیح nIDDlgItem, char_u *s)
{
    WCHAR   *wp;
    BOOL    ret;

    wp = enc_to_utf16(s, پوچ);
    اگر (wp == پوچ)
	بازگشت FALSE;

    ret = SetDlgItemTextW(hDlg, nIDDlgItem, wp);
    vim_free(wp);
    بازگشت ret;
}

/*
 * Convert BGR to RGB for Windows GDI calls
 */
    ایستا COLORREF
swap_me(COLORREF colorref)
{
    صحیح  temp;
    نویسه *ptr = (نویسه *)&colorref;

    temp = *(ptr);
    *(ptr ) = *(ptr + 2);
    *(ptr + 2) = temp;
    بازگشت colorref;
}

    ایستا INT_PTR CALLBACK
PrintDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam UNUSED,
	LPARAM lParam UNUSED)
{
# ifdef FEAT_GETTEXT
    NONCLIENTMETRICS nm;
    ایستا HFONT hfont;
# endif

    انتخاب (message)
    {
	حالت WM_INITDIALOG:
# ifdef FEAT_GETTEXT
	    nm.cbSize = اندازه(NONCLIENTMETRICS);
	    اگر (SystemParametersInfo(
			SPI_GETNONCLIENTMETRICS,
			اندازه(NONCLIENTMETRICS),
			&nm,
			0))
	    {
		نویسه buff[MAX_PATH];
		صحیح i;

		// Translate the dialog texts
		hfont = CreateFontIndirect(&nm.lfMessageFont);
		برای (i = IDC_PRINTTEXT1; i <= IDC_PROGRESS; i++)
		{
		    SendDlgItemMessage(hDlg, i, WM_SETFONT, (WPARAM)hfont, 1);
		    اگر (GetDlgItemText(hDlg,i, buff, اندازه(buff)))
			vimSetDlgItemText(hDlg,i, (char_u *)_(buff));
		}
		SendDlgItemMessage(hDlg, IDCANCEL,
						WM_SETFONT, (WPARAM)hfont, 1);
		اگر (GetDlgItemText(hDlg,IDCANCEL, buff, اندازه(buff)))
		    vimSetDlgItemText(hDlg,IDCANCEL, (char_u *)_(buff));
	    }
# endif
	    SetWindowText(hDlg, (LPCSTR)szAppName);
	    اگر (prt_name != پوچ)
	    {
		vimSetDlgItemText(hDlg, IDC_PRINTTEXT2, (char_u *)prt_name);
		VIM_CLEAR(prt_name);
	    }
	    EnableMenuItem(GetSystemMenu(hDlg, FALSE), SC_CLOSE, MF_GRAYED);
# اگر !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	    اگر (!gui.in_use)
#  endif
		Bring_Window_To_بالاترین(s_hwnd);
# endif
	    بازگشت TRUE;

	حالت WM_COMMAND:
	    *bUserAbort = TRUE;
	    EnableWindow(GetParent(hDlg), TRUE);
	    DestroyWindow(hDlg);
	    hDlgPrint = پوچ;
# ifdef FEAT_GETTEXT
	    DeleteObject(hfont);
# endif
	    بازگشت TRUE;
    }
    بازگشت FALSE;
}

    ایستا BOOL CALLBACK
AbortProc(HDC hdcPrn UNUSED, صحیح iCode UNUSED)
{
    پیام_2 پیام;

    وقتی (!*bUserAbort && PeekMessageW(&پیام, پوچ, 0, 0, PM_REMOVE))
    {
	اگر (!hDlgPrint || !IsDialogMessageW(hDlgPrint, &پیام))
	{
	    TranslateMessage(&پیام);
	    DispatchMessageW(&پیام);
	}
    }
    بازگشت !*bUserAbort;
}

    ایستا UINT_PTR CALLBACK
PrintHookProc(
	HWND hDlg,	// handle to dialog box
	UINT ui_پیام,	// message identifier
	WPARAM wParam UNUSED,	// message parameter
	LPARAM lParam	// message parameter
	)
{
    HWND	hwndOwner;
    RECT	rc, rcDlg, rcOwner;
    PRINTDLGW	*pPD;

    اگر (ui_پیام != WM_INITDIALOG)
	بازگشت FALSE;

    // Get the owner window and dialog box rectangles.
    اگر ((hwndOwner = GetParent(hDlg)) == پوچ)
	hwndOwner = GetDesktopWindow();

    GetWindowRect(hwndOwner, &rcOwner);
    GetWindowRect(hDlg, &rcDlg);
    CopyRect(&rc, &rcOwner);

    // Offset the owner and dialog box rectangles so that
    // right and bottom values represent the width and
    // height, and then offset the owner again to discard
    // space taken up by the dialog box.

    OffsetRect(&rcDlg, -rcDlg.چپ, -rcDlg.بالاترین);
    OffsetRect(&rc, -rc.چپ, -rc.بالاترین);
    OffsetRect(&rc, -rcDlg.راست, -rcDlg.bottom);

    // The new position is the sum of half the remaining
    // space and the owner's original position.

    SetWindowPos(hDlg,
	    HWND_بالاترین,
	    rcOwner.چپ + (rc.راست / 2),
	    rcOwner.بالاترین + (rc.bottom / 2),
	    0, 0,		// ignores size arguments
	    SWP_NOSIZE);

    //  tackle the printdlg copiesctrl problem
    pPD = (PRINTDLGW *)lParam;
    pPD->nCopies = (WORD)pPD->lCustData;
    SetDlgItemInt( hDlg, edt3, pPD->nCopies, FALSE );
    //  Bring the window to top
    Bring_Window_To_بالاترین(GetParent(hDlg));
    SetForegroundWindow(hDlg);

    بازگشت FALSE;
}

    تهی
mch_print_cleanup(تهی)
{
    صحیح pifItalic;
    صحیح pifBold;
    صحیح pifUnderline;

    برای (pifBold = 0; pifBold <= 1; pifBold++)
	برای (pifItalic = 0; pifItalic <= 1; pifItalic++)
	    برای (pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
		DeleteObject(prt_font_handles[pifBold][pifItalic][pifUnderline]);

    اگر (prt_dlg.hDC != پوچ)
	DeleteDC(prt_dlg.hDC);
    اگر (!*bUserAbort && hDlgPrint != پوچ)
	SendMessage(hDlgPrint, WM_COMMAND, 0, 0);
}

    ایستا صحیح
to_device_units(صحیح idx, صحیح dpi, صحیح physsize, صحیح offset, صحیح def_عدد)
{
    صحیح		ret = 0;
    صحیح		u;
    صحیح		nr;

    u = prt_get_unit(idx);
    اگر (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_عدد;
    }
    وگرنه
	nr = printer_opts[idx].عدد;

    انتخاب (u)
    {
	حالت PRT_UNIT_PERC:
	    ret = (physsize * nr) / 100;
	    بشکن;
	حالت PRT_UNIT_INCH:
	    ret = (nr * dpi);
	    بشکن;
	حالت PRT_UNIT_MM:
	    ret = (nr * 10 * dpi) / 254;
	    بشکن;
	حالت PRT_UNIT_POINT:
	    ret = (nr * 10 * dpi) / 720;
	    بشکن;
    }

    اگر (ret < offset)
	بازگشت 0;
    وگرنه
	بازگشت ret - offset;
}

    ایستا صحیح
prt_get_cpl(تهی)
{
    صحیح		hr;
    صحیح		phyw;
    صحیح		dvoff;
    صحیح		rev_offset;
    صحیح		dpi;

    GetTextMetrics(prt_dlg.hDC, &prt_tm);
    prt_سطر_height = prt_tm.tmHeight + prt_tm.tmExternalLeading;

    hr	    = GetDeviceCaps(prt_dlg.hDC, HORZRES);
    phyw    = GetDeviceCaps(prt_dlg.hDC, PHYSICALWIDTH);
    dvoff   = GetDeviceCaps(prt_dlg.hDC, PHYSICALOFFSETX);
    dpi	    = GetDeviceCaps(prt_dlg.hDC, LOGPIXELSX);

    rev_offset = phyw - (dvoff + hr);

    prt_چپ_margin = to_device_units(OPT_PRINT_چپ, dpi, phyw, dvoff, 10);
    اگر (prt_use_عدد())
    {
	prt_عدد_width = PRINT_عدد_WIDTH * prt_tm.tmAveCharWidth;
	prt_چپ_margin += prt_عدد_width;
    }
    وگرنه
	prt_عدد_width = 0;

    prt_راست_margin = hr - to_device_units(OPT_PRINT_راست, dpi, phyw,
							       rev_offset, 5);

    بازگشت (prt_راست_margin - prt_چپ_margin) / prt_tm.tmAveCharWidth;
}

    ایستا صحیح
prt_get_lpp(تهی)
{
    صحیح vr;
    صحیح phyw;
    صحیح dvoff;
    صحیح rev_offset;
    صحیح	bottom_margin;
    صحیح	dpi;

    vr	    = GetDeviceCaps(prt_dlg.hDC, VERTRES);
    phyw    = GetDeviceCaps(prt_dlg.hDC, PHYSICALHEIGHT);
    dvoff   = GetDeviceCaps(prt_dlg.hDC, PHYSICALOFFSETY);
    dpi	    = GetDeviceCaps(prt_dlg.hDC, LOGPIXELSY);

    rev_offset = phyw - (dvoff + vr);

    prt_بالاترین_margin = to_device_units(OPT_PRINT_بالاترین, dpi, phyw, dvoff, 5);

    // adjust top margin if there is a header
    prt_بالاترین_margin += prt_سطر_height * prt_header_height();

    bottom_margin = vr - to_device_units(OPT_PRINT_BOT, dpi, phyw,
							       rev_offset, 5);

    بازگشت (bottom_margin - prt_بالاترین_margin) / prt_سطر_height;
}

    صحیح
mch_print_init(prt_settings_T *psettings, char_u *jobname, صحیح forceit)
{
    ایستا HGLOBAL	stored_dm = پوچ;
    ایستا HGLOBAL	stored_devn = پوچ;
    ایستا صحیح		stored_nCopies = 1;
    ایستا صحیح		stored_nFlags = 0;

    LOGFONTW		fLogFont;
    صحیح			pifItalic;
    صحیح			pifBold;
    صحیح			pifUnderline;

    DEVMODEW		*mem;
    DEVNAMES		*devname;
    صحیح			i;
    DWORD		err;

    bUserAbort = &(psettings->user_abort);
    CLEAR_FIELD(prt_dlg);
    prt_dlg.lStructSize = اندازه(PRINTDLGW);
# اگر !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
    اگر (!gui.in_use)
#  endif
	GetConsoleHwnd();	    // get value of s_hwnd
# endif
    prt_dlg.hwndOwner = s_hwnd;
    prt_dlg.Flags = PD_NOPAGENUMS | PD_NOSELECTION | PD_RETURNDC;
    اگر (!forceit)
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
    اگر (forceit
# اگر !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	    || (!gui.in_use && !term_console)
#  وگرنه
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
	اگر (*p_pdev != NUL)
	    prt_dlg.hDC = CreateDC(پوچ, (LPCSTR)p_pdev, پوچ, پوچ);
	وگرنه
	{
	    prt_dlg.Flags |= PD_RETURNDEFAULT;
	    اگر (PrintDlgW(&prt_dlg) == 0)
		برو init_fail_dlg;
	}
    }
    وگرنه اگر (PrintDlgW(&prt_dlg) == 0)
	برو init_fail_dlg;
    وگرنه
    {
	/*
	 * keep the previous driver context
	 */
	stored_dm = prt_dlg.hDevMode;
	stored_devn = prt_dlg.hDevNames;
	stored_nFlags = prt_dlg.Flags;
	stored_nCopies = prt_dlg.nCopies;
    }

    اگر (prt_dlg.hDC == پوچ)
    {
	emsg(_(e_printer_selection_failed));
	mch_print_cleanup();
	بازگشت FALSE;
    }

    // Not all printer drivers report the support of color (or grey) in the
    // same way.  Let's set has_color if there appears to be some way to print
    // more than B&W.
    i = GetDeviceCaps(prt_dlg.hDC, NUMCOLORS);
    psettings->has_color = (GetDeviceCaps(prt_dlg.hDC, BITSPIXEL) > 1
				   || GetDeviceCaps(prt_dlg.hDC, PLANES) > 1
				   || i > 2 || i == -1);

    // Ensure all font styles are baseline aligned
    SetTextAlign(prt_dlg.hDC, TA_BASELINE|TA_چپ);

    /*
     * On some windows systems the nCopies parameter is not
     * passed back correctly. It must be retrieved from the
     * hDevMode struct.
     */
    mem = (DEVMODEW *)GlobalLock(prt_dlg.hDevMode);
    اگر (mem != پوچ)
    {
	اگر (mem->dmCopies != 1)
	    stored_nCopies = mem->dmCopies;
	اگر ((mem->dmFields & DM_DUPLEX) && (mem->dmDuplex & ~DMDUP_SIMPLEX))
	    psettings->duplex = TRUE;
	اگر ((mem->dmFields & DM_COLOR) && (mem->dmColor & DMCOLOR_COLOR))
	    psettings->has_color = TRUE;
    }
    GlobalUnlock(prt_dlg.hDevMode);

    devname = (DEVNAMES *)GlobalLock(prt_dlg.hDevNames);
    اگر (devname != 0)
    {
	WCHAR	*wprinter_name = (WCHAR *)devname + devname->wDeviceOffset;
	WCHAR	*wport_name = (WCHAR *)devname + devname->wOutputOffset;
	char_u	*text = (char_u *)_("to %s on %s");
	char_u  *printer_name = utf16_to_enc(wprinter_name, پوچ);
	char_u	*port_name = utf16_to_enc(wport_name, پوچ);

	اگر (printer_name != پوچ && port_name != پوچ)
	    prt_name = alloc(STRLEN(printer_name)
					   + STRLEN(port_name) + STRLEN(text));
	اگر (prt_name != پوچ)
	    wsprintf((نویسه *)prt_name, (ثابت نویسه *)text,
		    printer_name, port_name);
	vim_free(printer_name);
	vim_free(port_name);
    }
    GlobalUnlock(prt_dlg.hDevNames);

    /*
     * Initialise the font according to 'printfont'
     */
    CLEAR_FIELD(fLogFont);
    اگر (get_logfont(&fLogFont, p_pfn, prt_dlg.hDC, TRUE) == FAIL)
    {
	semsg(_(e_unknown_printer_font_str), p_pfn);
	mch_print_cleanup();
	بازگشت FALSE;
    }

    برای (pifBold = 0; pifBold <= 1; pifBold++)
	برای (pifItalic = 0; pifItalic <= 1; pifItalic++)
	    برای (pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
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
    psettings->chars_per_سطر = prt_get_cpl();
    psettings->سطرها_per_page = prt_get_lpp();
    اگر (prt_dlg.Flags & PD_USEDEVMODECOPIESANDCOLLATE)
    {
	psettings->n_collated_copies = (prt_dlg.Flags & PD_COLLATE)
						    ? prt_dlg.nCopies : 1;
	psettings->n_uncollated_copies = (prt_dlg.Flags & PD_COLLATE)
						    ? 1 : prt_dlg.nCopies;

	اگر (psettings->n_collated_copies == 0)
	    psettings->n_collated_copies = 1;

	اگر (psettings->n_uncollated_copies == 0)
	    psettings->n_uncollated_copies = 1;
    }
    وگرنه
    {
	psettings->n_collated_copies = 1;
	psettings->n_uncollated_copies = 1;
    }

    psettings->jobname = jobname;

    بازگشت TRUE;

init_fail_dlg:
    err = CommDlgExtendedError();
    اگر (err)
    {
	char_u *buf;

	// I suspect FormatMessage() doesn't work for values returned by
	// CommDlgExtendedError().  What does?
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		پوچ, err, 0, (LPTSTR)(&buf), 0, پوچ);
	semsg(_(e_print_error_str),
		buf == پوچ ? (char_u *)_("Unknown") : buf);
	LocalFree((LPVOID)(buf));
    }
    وگرنه
	پیام_clr_eos(); // Maybe canceled

    mch_print_cleanup();
    بازگشت FALSE;
}


    صحیح
mch_print_begin(prt_settings_T *psettings)
{
    صحیح			ret = 0;
    نویسه		szBuffer[300];
    WCHAR		*wp;

    hDlgPrint = CreateDialog(g_hinst, TEXT("PrintDlgBox"),
					     prt_dlg.hwndOwner, PrintDlgProc);
    SetAbortProc(prt_dlg.hDC, AbortProc);
    wsprintf(szBuffer, _("Printing '%s'"), gettail(psettings->jobname));
    vimSetDlgItemText(hDlgPrint, IDC_PRINTTEXT1, (char_u *)szBuffer);

    wp = enc_to_utf16(psettings->jobname, پوچ);
    اگر (wp != پوچ)
    {
	DOCINFOW	di;

	CLEAR_FIELD(di);
	di.cbSize = اندازه(di);
	di.lpszDocName = wp;
	ret = StartDocW(prt_dlg.hDC, &di);
	vim_free(wp);
    }

# ifdef FEAT_GUI
    // Give focus back to main window (when using MDI).
#  ifdef VIMDLL
    اگر (gui.in_use)
#  endif
	SetFocus(s_hwnd);
# endif

    بازگشت (ret > 0);
}

    تهی
mch_print_end(prt_settings_T *psettings UNUSED)
{
    EndDoc(prt_dlg.hDC);
    اگر (!*bUserAbort && hDlgPrint != پوچ)
	SendMessage(hDlgPrint, WM_COMMAND, 0, 0);
}

    صحیح
mch_print_end_page(تهی)
{
    بازگشت (EndPage(prt_dlg.hDC) > 0);
}

    صحیح
mch_print_begin_page(char_u *پیام)
{
    اگر (پیام != پوچ)
	vimSetDlgItemText(hDlgPrint, IDC_PROGRESS, پیام);
    بازگشت (StartPage(prt_dlg.hDC) > 0);
}

    صحیح
mch_print_blank_page(تهی)
{
    بازگشت (mch_print_begin_page(پوچ) ? (mch_print_end_page()) : FALSE);
}

ایستا صحیح prt_pos_x = 0;
ایستا صحیح prt_pos_y = 0;

    تهی
mch_print_start_سطر(صحیح margin, صحیح page_سطر)
{
    اگر (margin)
	prt_pos_x = -prt_عدد_width;
    وگرنه
	prt_pos_x = 0;
    prt_pos_y = page_سطر * prt_سطر_height
				 + prt_tm.tmAscent + prt_tm.tmExternalLeading;
}

    صحیح
mch_print_text_out(char_u *p, صحیح len)
{
    SIZE	sz;
    WCHAR	*wp;
    صحیح		wlen = len;
    صحیح		ret = FALSE;

    wp = enc_to_utf16(p, &wlen);
    اگر (wp == پوچ)
	بازگشت FALSE;

    TextOutW(prt_dlg.hDC, prt_pos_x + prt_چپ_margin,
	    prt_pos_y + prt_بالاترین_margin, wp, wlen);
    GetTextExtentPoint32W(prt_dlg.hDC, wp, wlen, &sz);
    vim_free(wp);
    prt_pos_x += (sz.cx - prt_tm.tmOverhang);
    // This is wrong when printing spaces for a TAB.
    اگر (p[len] != NUL)
    {
	wlen = mb_ptr2len(p + len);
	wp = enc_to_utf16(p + len, &wlen);
	اگر (wp != پوچ)
	{
	    GetTextExtentPoint32W(prt_dlg.hDC, wp, 1, &sz);
	    ret = (prt_pos_x + prt_چپ_margin + sz.cx > prt_راست_margin);
	    vim_free(wp);
	}
    }
    بازگشت ret;
}

    تهی
mch_print_set_font(صحیح iBold, صحیح iItalic, صحیح iUnderline)
{
    SelectObject(prt_dlg.hDC, prt_font_handles[iBold][iItalic][iUnderline]);
}

    تهی
mch_print_set_bg(long_u bgcol)
{
    SetBkColor(prt_dlg.hDC, GetNearestColor(prt_dlg.hDC,
						   swap_me((COLORREF)bgcol)));
    /*
     * With a white background we can draw characters transparent, which is
     * good for italic characters that overlap to the next char cell.
     */
    اگر (bgcol == 0xffffffUL)
	SetBkMode(prt_dlg.hDC, TRANSPARENT);
    وگرنه
	SetBkMode(prt_dlg.hDC, OPAQUE);
}

    تهی
mch_print_set_fg(long_u fgcol)
{
    SetTextColor(prt_dlg.hDC, GetNearestColor(prt_dlg.hDC,
						   swap_me((COLORREF)fgcol)));
}

#endif // FEAT_PRINTER && !FEAT_POSTSCRIPT



#اگر defined(FEAT_SHORTCUT)
# include <shlobj.h>

# define is_path_sep(c)	    ((c) == L'\\' || (c) == L'/')

    ایستا صحیح
is_reparse_point_included(LPCWSTR fname)
{
    LPCWSTR	p = fname, q;
    WCHAR	buf[MAX_PATH];
    DWORD	attr;

    اگر (SAFE_isalpha(p[0]) && p[1] == L':' && is_path_sep(p[2]))
	p += 3;
    وگرنه اگر (is_path_sep(p[0]) && is_path_sep(p[1]))
	p += 2;

    وقتی (*p != L'\0')
    {
	q = wcspbrk(p, L"\\/");
	اگر (q == پوچ)
	    p = q = fname + wcslen(fname);
	وگرنه
	    p = q + 1;
	اگر (q - fname >= MAX_PATH)
	    بازگشت FALSE;
	wcsncpy(buf, fname, q - fname);
	buf[q - fname] = L'\0';
	attr = GetFileAttributesW(buf);
	اگر (attr != INVALID_FILE_ATTRIBUTES
		&& (attr & FILE_ATTRIBUTE_REPARSE_POINT) != 0)
	    بازگشت TRUE;
    }
    بازگشت FALSE;
}

/*
 * Return the resolved file path, NULL if "fname" is an AppExecLink reparse
 * point, already fully resolved, or it doesn't exists.
 */
    char_u *
resolve_reparse_point(char_u *fname)
{
    HANDLE	    h = INVALID_HANDLE_مقدار;
    DWORD	    size;
    WCHAR	    *p, *wp;
    char_u	    *rfname = پوچ;
    WCHAR	    *buff = پوچ;

    p = enc_to_utf16(fname, پوچ);
    اگر (p == پوچ)
	برو fail;

    اگر (!is_reparse_point_included(p))
    {
	vim_free(p);
	برو fail;
    }

    h = CreateFileW(p, 0, 0, پوچ, OPEN_EXISTING,
	    FILE_FLAG_BACKUP_SEMANTICS, پوچ);
    vim_free(p);

    اگر (h == INVALID_HANDLE_مقدار)
	برو fail;

    size = GetFinalPathNameByHandleW(h, پوچ, 0, 0);
    اگر (size == 0)
	برو fail;
    buff = ALLOC_MULT(WCHAR, size);
    اگر (buff == پوچ)
	برو fail;
    اگر (GetFinalPathNameByHandleW(h, buff, size, 0) == 0)
	برو fail;

    اگر (wcsncmp(buff, L"\\\\?\\UNC\\", 8) == 0)
    {
	buff[6] = L'\\';
	wp = buff + 6;
    }
    وگرنه اگر (wcsncmp(buff, L"\\\\?\\", 4) == 0)
	wp = buff + 4;
    وگرنه
	wp = buff;

    rfname = utf16_to_enc(wp, پوچ);

fail:
    اگر (h != INVALID_HANDLE_مقدار)
	CloseHandle(h);
    اگر (buff != پوچ)
	vim_free(buff);

    بازگشت rfname;
}

/*
 * When "fname" is the name of a shortcut (*.lnk) resolve the file it points
 * to and return that name in allocated memory.
 * Otherwise NULL is returned.
 */
    ایستا char_u *
resolve_shortcut(char_u *fname)
{
    HRESULT		hr;
    IShellLink		*psl = پوچ;
    IPersistFile	*ppf = پوچ;
    OLECHAR		wsz[MAX_PATH];
    char_u		*rfname = پوچ;
    صحیح			len;
    IShellLinkW		*pslw = پوچ;
    WIN32_FIND_DATAW	ffdw; // we get those free of charge

    // Check if the file name ends in ".lnk". Avoid calling
    // CoCreateInstance(), it's quite slow.
    اگر (fname == پوچ)
	بازگشت rfname;
    len = (صحیح)STRLEN(fname);
    اگر (len <= 4 || STRNICMP(fname + len - 4, ".lnk", 4) != 0)
	بازگشت rfname;

    CoInitialize(پوچ);

    // create a link manager object and request its interface
    hr = CoCreateInstance(
	    &CLSID_ShellLink, پوچ, CLSCTX_INPROC_SERVER,
	    &IID_IShellLinkW, (تهی**)&pslw);
    اگر (hr == S_OK)
    {
	WCHAR	*p = enc_to_utf16(fname, پوچ);

	اگر (p != پوچ)
	{
	    // Get a pointer to the IPersistFile interface.
	    hr = pslw->lpVtbl->QueryInterface(
		    pslw, &IID_IPersistFile, (تهی**)&ppf);
	    اگر (hr != S_OK)
		برو shortcut_errorw;

	    // "load" the name and resolve the link
	    hr = ppf->lpVtbl->Load(ppf, p, STGM_READ);
	    اگر (hr != S_OK)
		برو shortcut_errorw;
# اگر 0  // This makes Vim wait a long time if the target does not exist.
	    hr = pslw->lpVtbl->Resolve(pslw, پوچ, SLR_NO_UI);
	    اگر (hr != S_OK)
		برو shortcut_errorw;
# endif

	    // Get the path to the link target.
	    ZeroMemory(wsz, MAX_PATH * اندازه(WCHAR));
	    hr = pslw->lpVtbl->GetPath(pslw, wsz, MAX_PATH, &ffdw, 0);
	    اگر (hr == S_OK && wsz[0] != NUL)
		rfname = utf16_to_enc(wsz, پوچ);

shortcut_errorw:
	    vim_free(p);
	}
    }

    // Release all interface pointers (both belong to the same object)
    اگر (ppf != پوچ)
	ppf->lpVtbl->Release(ppf);
    اگر (psl != پوچ)
	psl->lpVtbl->Release(psl);
    اگر (pslw != پوچ)
	pslw->lpVtbl->Release(pslw);

    CoUninitialize();
    بازگشت rfname;
}

    char_u *
mch_resolve_path(char_u *fname, صحیح reparse_point)
{
    char_u  *path = resolve_shortcut(fname);

    اگر (path == پوچ && reparse_point)
	path = resolve_reparse_point(fname);
    بازگشت path;
}
#endif

#اگر defined(FEAT_EVAL) && (!defined(FEAT_GUI) || defined(VIMDLL))
/*
 * Bring ourselves to the foreground.  Does work if the OS doesn't allow it.
 */
    تهی
win32_set_foreground(تهی)
{
    GetConsoleHwnd();	    // get value of s_hwnd
    اگر (s_hwnd != 0)
	SetForegroundWindow(s_hwnd);
}
#endif

#اگر defined(FEAT_CLIENTSERVER)
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
# define VIM_CLASSNAME_LEN  (اندازه(VIM_CLASSNAME) - 1)

// Timeout for sending a message to another Vim instance.  Normally this works
// instantly, but it may hang when the other Vim instance is halted.
# define SENDMESSAGE_TIMEOUT	(5 * 1000)

// Communication is via WM_COPYDATA messages. The message type is sent in
// the dwData parameter. Types are defined here.
# define COPYDATA_کلیدها		0
# define COPYDATA_REPLY		1
# define COPYDATA_عبارت		10
# define COPYDATA_RESULT	11
# define COPYDATA_ERROR_RESULT	12
# define COPYDATA_ENCODING	20

// This is a structure containing a server HWND and its name.
ساختار server_id
{
    HWND hwnd;
    char_u *name;
};

// Last received 'encoding' that the client uses.
ایستا char_u	*client_enc = پوچ;

/*
 * Tell the other side what encoding we are using.
 * Return -1 if timeout happens.  Other errors are ignored.
 */
    ایستا صحیح
serverSendEnc(HWND target)
{
    COPYDATASTRUCT data;

    data.dwData = COPYDATA_ENCODING;
    data.cbData = (DWORD)STRLEN(p_enc) + 1;
    data.lpData = p_enc;
    اگر (SendMessageTimeout(target, WM_COPYDATA,
	    (WPARAM)message_window, (LPARAM)&data,
	    SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, پوچ) == 0)
	بازگشت -1;
    بازگشت 0;
}

/*
 * Clean up on exit. This destroys the hidden message window.
 */
    ایستا تهی
CleanUpMessaging(تهی)
{
    اگر (message_window == 0)
	بازگشت;

    DestroyWindow(message_window);
    message_window = 0;
}

ایستا صحیح save_reply(HWND server, char_u *reply, صحیح عبارت);

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
    ایستا LRESULT CALLBACK
Messaging_WndProc(HWND hwnd, UINT پیام, WPARAM wParam, LPARAM lParam)
{
    اگر (پیام == WM_COPYDATA)
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
	صحیح		retval;
	DWORD_PTR	dwret = 0;
	char_u		*str;
	char_u		*tofree;

	انتخاب (data->dwData)
	{
	حالت COPYDATA_ENCODING:
	    // Remember the encoding that the client uses.
	    vim_free(client_enc);
	    client_enc = enc_canonize((char_u *)data->lpData);
	    بازگشت 1;

	حالت COPYDATA_کلیدها:
	    // Remember who sent this, for <client>
	    clientWindow = sender;

	    // Add the received keys to the input buffer.
	    str = serverConvert(client_enc, (char_u *)data->lpData, &tofree);
# ifdef FEAT_GUI
	    اگر (gui.in_use)
		// The GUI may get here re-entrantly, so insert the keys later.
		server_add_درون_داد(str);
	    وگرنه
# endif
		server_to_درون_داد_buf(str);
	    vim_free(tofree);

# ifdef FEAT_GUI
	    // Wake up the main GUI loop.
#  ifdef VIMDLL
	    اگر (gui.in_use)
#  endif
		اگر (s_hwnd != 0)
		    PostMessage(s_hwnd, WM_NULL, 0, 0);
# endif
	    بازگشت 1;

	حالت COPYDATA_عبارت:
	    // Remember who sent this, for <client>
	    clientWindow = sender;

	    str = serverConvert(client_enc, (char_u *)data->lpData, &tofree);
	    res = eval_client_عبارت_to_string(str);

	    اگر (res == پوچ)
	    {
		نویسه	*err = _(e_invalid_expression_received);
		size_t	len = STRLEN(str) + STRLEN(err) + 5;

		res = alloc(len);
		اگر (res != پوچ)
		    reply.cbData = (DWORD)vim_snprintf_safelen(
			(نویسه *)res, len, "%s: \"%s\"", err, str);
		وگرنه
		    reply.cbData = 0;
		reply.dwData = COPYDATA_ERROR_RESULT;
	    }
	    وگرنه
	    {
		reply.cbData = (DWORD)STRLEN(res) + 1;
		reply.dwData = COPYDATA_RESULT;
	    }
	    reply.lpData = res;

	    اگر (serverSendEnc(sender) < 0)
		retval = -1;
	    وگرنه
	    {
		اگر (SendMessageTimeout(sender, WM_COPYDATA,
			(WPARAM)message_window, (LPARAM)&reply,
			SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
		    retval = -1;
		وگرنه
		    retval = (صحیح)dwret;
	    }
	    vim_free(tofree);
	    vim_free(res);
	    بازگشت retval;

	حالت COPYDATA_REPLY:
	حالت COPYDATA_RESULT:
	حالت COPYDATA_ERROR_RESULT:
	    اگر (data->lpData != پوچ)
	    {
		str = serverConvert(client_enc, (char_u *)data->lpData,
								     &tofree);
		اگر (tofree == پوچ)
		    str = vim_strsave(str);
		اگر (save_reply(sender, str,
			   (data->dwData == COPYDATA_REPLY ?  0 :
			   (data->dwData == COPYDATA_RESULT ? 1 :
							      2))) == FAIL)
		    vim_free(str);
		وگرنه اگر (data->dwData == COPYDATA_REPLY)
		{
		    char_u	winstr[30];

		    sprintf((نویسه *)winstr, PRINTF_HEX_LONG_U, (long_u)sender);
		    apply_autocmds(EVENT_REMOTEREPLY, winstr, str,
								TRUE, curbuf);
		}
	    }
	    بازگشت 1;
	}

	بازگشت 0;
    }

    وگرنه اگر (پیام == WM_ACTIVATE && wParam == WA_ACTIVE)
    {
	// When the message window is activated (brought to the foreground),
	// this actually applies to the text window.
# اگر !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	اگر (!gui.in_use)
#  endif
	    GetConsoleHwnd();	    // get value of s_hwnd
# endif
	اگر (s_hwnd != 0)
	{
	    SetForegroundWindow(s_hwnd);
	    بازگشت 0;
	}
    }

    بازگشت DefWindowProc(hwnd, پیام, wParam, lParam);
}

/*
 * Initialise the message handling process.  This involves creating a window
 * to handle messages - the window will not be visible.
 */
    تهی
serverInitMessaging(تهی)
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
    wndclass.hIcon = پوچ;
    wndclass.hCursor = پوچ;
    wndclass.hbrBackground = پوچ;
    wndclass.lpszMenuName = پوچ;
    wndclass.lpszClassName = VIM_CLASSNAME;
    RegisterClass(&wndclass);

    // Create the message window. It will be hidden, so the details don't
    // matter.  Don't use WS_OVERLAPPEDWINDOW, it will make a shortcut remove
    // focus from gvim.
    message_window = CreateWindow(VIM_CLASSNAME, "",
			 WS_POPUPWINDOW | WS_CAPTION,
			 CW_USEDEFAULT, CW_USEDEFAULT,
			 100, 100, پوچ, پوچ,
			 g_hinst, پوچ);
}

// Used by serverSendToVim() to find an alternate server name.
ایستا char_u *altname_buf_ptr = پوچ;

/*
 * Get the title of the window "hwnd", which is the Vim server name, in
 * "name[namelen]" and return the length.
 * Returns zero if window "hwnd" is not a Vim server.
 */
    ایستا صحیح
getVimServerName(HWND hwnd, نویسه *name, صحیح namelen)
{
    صحیح		len;
    نویسه	buffer[VIM_CLASSNAME_LEN + 1];

    // Ignore windows which aren't Vim message windows
    len = GetClassName(hwnd, buffer, اندازه(buffer));
    اگر (len != VIM_CLASSNAME_LEN || STRCMP(buffer, VIM_CLASSNAME) != 0)
	بازگشت 0;

    // Get the title of the window
    بازگشت GetWindowText(hwnd, name, namelen);
}

    ایستا BOOL CALLBACK
enumWindowsGetServer(HWND hwnd, LPARAM lparam)
{
    ساختار	server_id *id = (ساختار server_id *)lparam;
    نویسه	server[MAX_PATH];

    // Get the title of the window
    اگر (getVimServerName(hwnd, server, اندازه(server)) == 0)
	بازگشت TRUE;

    // If this is the server we're looking for, return its HWND
    اگر (STRICMP(server, id->name) == 0)
    {
	id->hwnd = hwnd;
	بازگشت FALSE;
    }

    // If we are looking for an alternate server, remember this name.
    اگر (altname_buf_ptr != پوچ)
    {
	size_t	namelen = STRLEN(id->name);

	اگر (STRNICMP(server, id->name, namelen) == 0
	    && vim_isdigit(server[namelen]))
	{
	    STRCPY(altname_buf_ptr, server);
	    altname_buf_ptr = پوچ;	    // don't use another name
	}
    }

    // Otherwise, keep looking
    بازگشت TRUE;
}

    ایستا BOOL CALLBACK
enumWindowsGetNames(HWND hwnd, LPARAM lparam)
{
    list_T	*list = (list_T *)lparam;
    نویسه	server[MAX_PATH];

    // Get the title of the window
    اگر (getVimServerName(hwnd, server, اندازه(server)) == 0)
	بازگشت TRUE;

    list_append_string(list, (char_u *)server, -1);
    بازگشت TRUE;
}

ساختار enum_windows_s
{
    WNDENUMPROC lpEnumFunc;
    LPARAM      lParam;
};

    ایستا BOOL CALLBACK
enum_windows_child(HWND hwnd, LPARAM lParam)
{
    ساختار enum_windows_s *ew = (ساختار enum_windows_s *)lParam;

    بازگشت (ew->lpEnumFunc)(hwnd, ew->lParam);
}

    ایستا BOOL CALLBACK
enum_windows_toplevel(HWND hwnd, LPARAM lParam)
{
    ساختار enum_windows_s *ew = (ساختار enum_windows_s *)lParam;

    اگر ((ew->lpEnumFunc)(hwnd, ew->lParam))
	بازگشت TRUE;
    بازگشت EnumChildWindows(hwnd, enum_windows_child, lParam);
}

/*
 * Enumerate all windows including children.
 */
    ایستا BOOL
enum_windows(WNDENUMPROC lpEnumFunc, LPARAM lParam)
{
    ساختار enum_windows_s ew;

    ew.lpEnumFunc = lpEnumFunc;
    ew.lParam = lParam;
    بازگشت EnumWindows(enum_windows_toplevel, (LPARAM)&ew);
}

    ایستا HWND
findServer(char_u *name)
{
    ساختار server_id id;

    id.name = name;
    id.hwnd = 0;

    enum_windows(enumWindowsGetServer, (LPARAM)(&id));

    بازگشت id.hwnd;
}

    تهی
serverSetName(char_u *name)
{
    size_t	namelen;
    char_u	*ok_name;
    HWND	hwnd = 0;
    صحیح		i = 0;
    char_u	*p;

    // Leave enough space for a 9-digit suffix to ensure uniqueness!
    namelen = STRLEN(name);
    ok_name = alloc(namelen + 10);
    اگر (ok_name == پوچ)
	بازگشت;

    STRCPY(ok_name, name);
    p = ok_name + namelen;

    برای (;;)
    {
	// This is inefficient - we're doing an EnumWindows loop for each
	// possible name. It would be better to grab all names in one go,
	// and scan the list each time...
	hwnd = findServer(ok_name);
	اگر (hwnd == 0)
	    بشکن;

	++i;
	اگر (i >= 1000)
	    بشکن;

	sprintf((نویسه *)p, "%d", i);
    }

    اگر (hwnd != 0)
	vim_free(ok_name);
    وگرنه
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
serverGetVimNames(تهی)
{
    list_T *list = list_alloc();

    اگر (list == پوچ)
	بازگشت پوچ;

    enum_windows(enumWindowsGetNames, (LPARAM)list);

    بازگشت list;
}

    صحیح
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
    sscanf((نویسه *)name, SCANF_HEX_LONG_U, &n);
    اگر (n == 0)
	بازگشت -1;

    target = (HWND)n;
    اگر (!IsWindow(target))
	بازگشت -1;

    data.dwData = COPYDATA_REPLY;
    data.cbData = (DWORD)STRLEN(reply) + 1;
    data.lpData = reply;

    اگر (serverSendEnc(target) < 0)
	بازگشت -1;
    اگر (SendMessageTimeout(target, WM_COPYDATA,
		(WPARAM)message_window, (LPARAM)&data,
		SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
	بازگشت -1;
    بازگشت dwret ? 0 : -1;
}

    صحیح
serverSendToVim(
    char_u	 *name,			// Where to send.
    char_u	 *cmd,			// What to send.
    char_u	 **result,		// Result of eval'ed expression
    تهی	 *ptarget,		// HWND of server
    صحیح		 as_عبارت,		// Expression or keys?
    صحیح		 timeout,		// timeout in seconds or zero
    صحیح		 silent)		// don't complain about no server
{
    size_t	namelen;
    HWND	target;
    COPYDATASTRUCT data;
    char_u	*retval = پوچ;
    صحیح		retcode = 0;
    DWORD_PTR	dwret = 0;
    char_u	altname_buf[MAX_PATH];

    // Execute locally if no display or target is ourselves
    اگر (serverName != پوچ && STRICMP(name, serverName) == 0)
	بازگشت sendToLocalVim(cmd, as_عبارت, result);

    // If the server name does not end in a digit then we look for an
    // alternate name.  e.g. when "name" is GVIM then we may find GVIM2.
    namelen = STRLEN(name);
    اگر (namelen > 1 && !vim_isdigit(name[namelen - 1]))
	altname_buf_ptr = altname_buf;
    altname_buf[0] = NUL;
    target = findServer(name);
    altname_buf_ptr = پوچ;
    اگر (target == 0 && altname_buf[0] != NUL)
	// Use another server name we found.
	target = findServer(altname_buf);

    اگر (target == 0)
    {
	اگر (!silent)
	    semsg(_(e_no_registered_server_named_str), name);
	بازگشت -1;
    }

    اگر (ptarget)
	*(HWND *)ptarget = target;

    data.dwData = as_عبارت ? COPYDATA_عبارت : COPYDATA_کلیدها;
    data.cbData = (DWORD)STRLEN(cmd) + 1;
    data.lpData = cmd;

    اگر (serverSendEnc(target) < 0)
	بازگشت -1;
    اگر (SendMessageTimeout(target, WM_COPYDATA,
		(WPARAM)message_window, (LPARAM)&data,
		SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
	بازگشت -1;
    اگر (dwret == 0)
	بازگشت -1;

    اگر (as_عبارت)
	retval = serverGetReply(target, &retcode, TRUE, TRUE, timeout);

    اگر (result == پوچ)
	vim_free(retval);
    وگرنه
	*result = retval; // Caller assumes responsibility for freeing

    بازگشت retcode;
}

/*
 * Bring the server to the foreground.
 */
    تهی
serverForeground(char_u *name)
{
    HWND	target = findServer(name);

    اگر (target != 0)
	SetForegroundWindow(target);
}

// Replies from server need to be stored until the client picks them up via
// remote_read(). So we maintain a list of server-id/reply pairs.
// Note that there could be multiple replies from one server pending if the
// client is slow picking them up.
// We just store the replies in a simple list. When we remove an entry, we
// move list entries down to fill the gap.
// The server ID is simply the HWND.
تعریفنوع ساختار
{
    HWND	server;		// server window
    char_u	*reply;		// reply string
    صحیح		عبارت_result;	// 0 for REPLY, 1 for RESULT 2 for error
} reply_T;

ایستا garray_T reply_list = {0, 0, اندازه(reply_T), 5, 0};

# define REPLY_ITEM(i) ((reply_T *)(reply_list.ga_data) + (i))
# define REPLY_شمار (reply_list.ga_len)

// Flag which is used to wait for a reply
ایستا صحیح reply_received = 0;

/*
 * Store a reply.  "reply" must be allocated memory (or NULL).
 */
    ایستا صحیح
save_reply(HWND server, char_u *reply, صحیح عبارت)
{
    reply_T *rep;

    اگر (ga_grow(&reply_list, 1) == FAIL)
	بازگشت FAIL;

    rep = REPLY_ITEM(REPLY_شمار);
    rep->server = server;
    rep->reply = reply;
    rep->عبارت_result = عبارت;
    اگر (rep->reply == پوچ)
	بازگشت FAIL;

    ++REPLY_شمار;
    reply_received = 1;
    بازگشت OK;
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
serverGetReply(HWND server, صحیح *عبارت_res, صحیح remove, صحیح wait, صحیح timeout)
{
    صحیح		i;
    char_u	*reply;
    reply_T	*rep;
    صحیح		did_process = FALSE;
    time_t	start;
    time_t	now;

    // When waiting, loop until the message waiting for is received.
    time(&start);
    برای (;;)
    {
	// Reset this here, in case a message arrives while we are going
	// through the already received messages.
	reply_received = 0;

	برای (i = 0; i < REPLY_شمار; ++i)
	{
	    rep = REPLY_ITEM(i);
	    اگر (rep->server == server
		    && ((rep->عبارت_result != 0) == (عبارت_res != پوچ)))
	    {
		// Save the values we've found for later
		reply = rep->reply;
		اگر (عبارت_res != پوچ)
		    *عبارت_res = rep->عبارت_result == 1 ? 0 : -1;

		اگر (remove)
		{
		    // Move the rest of the list down to fill the gap
		    mch_memmove(rep, rep + 1,
				     (REPLY_شمار - i - 1) * اندازه(reply_T));
		    --REPLY_شمار;
		}

		// Return the reply to the caller, who takes on responsibility
		// for freeing it if "remove" is TRUE.
		بازگشت reply;
	    }
	}

	// If we got here, we didn't find a reply. Return immediately if the
	// "wait" parameter isn't set.
	اگر (!wait)
	{
	    // Process pending messages once. Without this, looping on
	    // remote_peek() would never get the reply.
	    اگر (!did_process)
	    {
		did_process = TRUE;
		serverProcessPendingMessages();
		ادامه;
	    }
	    بشکن;
	}

	// We need to wait for a reply. Enter a message loop until the
	// "reply_received" flag gets set.

	// Loop until we receive a reply
	وقتی (reply_received == 0)
	{
# ifdef FEAT_TIMERS
	    // TODO: use the return value to decide how long to wait.
	    check_due_timer();
# endif
	    time(&now);
	    اگر (timeout > 0 && (now - start) >= timeout)
		بشکن;

	    // Wait for a SendMessage() call to us.  This could be the reply
	    // we are waiting for.  Use a timeout of a second, to catch the
	    // situation that the server died unexpectedly.
	    پیام_Wait_For_Multiple_Objects(0, پوچ, TRUE, 1000, QS_ALLINPUT);

	    // If the server has died, give up
	    اگر (!IsWindow(server))
		بازگشت پوچ;

	    serverProcessPendingMessages();
	}
    }

    بازگشت پوچ;
}

/*
 * Process any messages in the Windows message queue.
 */
    تهی
serverProcessPendingMessages(تهی)
{
    پیام_2 پیام;

    وقتی (PeekMessageW(&پیام, پوچ, 0, 0, PM_REMOVE))
    {
	TranslateMessage(&پیام);
	DispatchMessageW(&پیام);
    }
}

#endif // FEAT_CLIENTSERVER

#اگر defined(FEAT_GUI) || (defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT))

ساختار charset_pair
{
    string_T	name;
    BYTE	charset;
};

ایستا ساختار charset_pair
charset_pairs[] =
{
    {STR_LITERAL_INIT("ANSI"),		ANSI_CHARSET},
    {STR_LITERAL_INIT("CHINESEBIG5"),	CHINESEBIG5_CHARSET},
    {STR_LITERAL_INIT("DEFAULT"),	DEFAULT_CHARSET},
    {STR_LITERAL_INIT("HANGEUL"),	HANGEUL_CHARSET},
    {STR_LITERAL_INIT("OEM"),		OEM_CHARSET},
    {STR_LITERAL_INIT("SHIFTJIS"),	SHIFTJIS_CHARSET},
    {STR_LITERAL_INIT("SYMBOL"),	نماد_CHARSET},
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

ساختار quality_pair
{
    string_T	name;
    DWORD	quality;
};

ایستا ساختار quality_pair
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
    نویسه *
charset_id2name(صحیح id)
{
    صحیح	i;

    برای (i = 0; i < (صحیح)ARRAY_LENGTH(charset_pairs); ++i)
    {
	اگر ((BYTE)id == charset_pairs[i].charset)
	    بازگشت (نویسه *)charset_pairs[i].name.string;
    }

    بازگشت پوچ;
}

/*
 * Convert a quality ID to a name.
 * Return NULL when not recognized.
 */
    نویسه *
quality_id2name(DWORD id)
{
    صحیح	i;

    برای (i = 0; i < (صحیح)ARRAY_LENGTH(quality_pairs); ++i)
    {
	اگر (id == quality_pairs[i].quality)
	    بازگشت (نویسه *)quality_pairs[i].name.string;
    }

    بازگشت پوچ;
}

// The default font height in 100% scaling (96dpi).
// (-16 in 96dpi equates to roughly 12pt)
# define DEFAULT_FONT_HEIGHT	(-16)

ایستا ثابت LOGFONTW s_lfDefault =
{
    DEFAULT_FONT_HEIGHT,
    0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
    PROOF_QUALITY, FIXED_PITCH | FF_DONTCARE,
    L""	// Default font name will be set later based on current language.
};

// This will be initialized when set_default_logfont() is called first time.
// The value will be based on the system DPI.
صحیح current_font_height = 0;		// also used in gui_w32.c

/*
 * Convert a string representing a point size into pixels. The string should
 * be a positive decimal number, with an optional decimal point (eg, "12", or
 * "10.5"). The pixel value is returned, and a pointer to the next unconverted
 * character is stored in *end. The flag "vertical" says whether this
 * calculation is for a vertical (height) size or a horizontal (width) one.
 */
    ایستا صحیح
points_to_pixels(WCHAR *str, WCHAR **end, صحیح vertical, long_i pprinter_dc)
{
    صحیح		pixels;
    صحیح		points = 0;
    صحیح		مقسوم_علیه = 0;
    HWND	hwnd = (HWND)0;
    HDC		hdc;
    HDC		printer_dc = (HDC)pprinter_dc;

    وقتی (*str != NUL)
    {
	اگر (*str == L'.' && مقسوم_علیه == 0)
	{
	    // Start keeping a divisor, for later
	    مقسوم_علیه = 1;
	}
	وگرنه
	{
	    اگر (!VIM_ISDIGIT(*str))
		بشکن;

	    points *= 10;
	    points += *str - L'0';
	    مقسوم_علیه *= 10;
	}
	++str;
    }

    اگر (مقسوم_علیه == 0)
	مقسوم_علیه = 1;

    اگر (printer_dc == پوچ)
    {
	hwnd = GetDesktopWindow();
	hdc = GetWindowDC(hwnd);
    }
    وگرنه
	hdc = printer_dc;

    pixels = MulDiv(points,
		    GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX),
		    72 * مقسوم_علیه);

    اگر (printer_dc == پوچ)
	ReleaseDC(hwnd, hdc);

    *end = str;
    بازگشت pixels;
}

/*
 * Convert pixel into point size. This is a reverse of points_to_pixels.
 */
    ایستا دوگانه
pixels_to_points(صحیح pixels, صحیح vertical, long_i pprinter_dc)
{
    دوگانه	points = 0;
    HWND	hwnd = (HWND)0;
    HDC		hdc;
    HDC		printer_dc = (HDC)pprinter_dc;

    اگر (printer_dc == پوچ)
    {
	hwnd = GetDesktopWindow();
	hdc = GetWindowDC(hwnd);
    }
    وگرنه
	hdc = printer_dc;

    points = pixels * 72.0 / GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX);
    اگر (printer_dc == پوچ)
	ReleaseDC(hwnd, hdc);

    بازگشت points;
}

    ایستا صحیح CALLBACK
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
    اگر ((ntm->tmPitchAndFamily & 1) != 0)
	بازگشت 1;
# endif

    // Remember this LOGFONTW as a "possible"
    *lf = elf->elfLogFont;

    // Terminate the scan as soon as we find an ANSI font
    اگر (lf->lfCharSet == ANSI_CHARSET
	    || lf->lfCharSet == OEM_CHARSET
	    || lf->lfCharSet == DEFAULT_CHARSET)
	بازگشت 0;

    // Continue the scan - we have a non-ANSI font
    بازگشت 2;
}

    ایستا صحیح
init_logfont(LOGFONTW *lf)
{
    صحیح		n;
    HWND	hwnd = GetDesktopWindow();
    HDC		hdc = GetWindowDC(hwnd);

    n = EnumFontFamiliesW(hdc,
			 lf->lfFaceName,
			 (FONTENUMPROCW)font_enumproc,
			 (LPARAM)lf);

    ReleaseDC(hwnd, hdc);

    // If we couldn't find a usable font, return failure
    اگر (n == 1)
	بازگشت FAIL;

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
    بازگشت OK;
}

/*
 * Call back for EnumFontFamiliesW in expand_font_enumproc.
 *
 */
    ایستا صحیح CALLBACK
expand_font_enumproc(
    ENUMLOGFONTW    *elf,
    NEWTEXTMETRICW  *ntm UNUSED,
    DWORD	    type UNUSED,
    LPARAM	    lparam)
{
    LOGFONTW *lf = (LOGFONTW*)elf;

# ifndef FEAT_PROPORTIONAL_FONTS
    // Ignore non-monospace fonts without further ado
    اگر ((ntm->tmPitchAndFamily & 1) != 0)
	بازگشت 1;
# endif

    // Filter only on ANSI. Otherwise will see a lot of random fonts that we
    // usually don't want.
    اگر (lf->lfCharSet != ANSI_CHARSET)
	بازگشت 1;

    صحیح (*add_match)(char_u *) = (صحیح (*)(char_u *))lparam;

    WCHAR *faceNameW = lf->lfFaceName;
    char_u *faceName = utf16_to_enc(faceNameW, پوچ);
    اگر (!faceName)
	بازگشت 0;

    add_match(faceName);
    vim_free(faceName);

    بازگشت 1;
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
    تهی
gui_mch_expand_font(
    optexpand_T	*args,
    تهی	*param UNUSED,
    صحیح		(*add_match)(char_u *val))
{
    expand_T	    *xp = args->oe_xp;
    اگر (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	نویسه buf[30];
	صحیح	i;

	// Always fill in with the current font size as first option for
	// convenience. We simply round to the closest integer for simplicity.
	صحیح font_height = (صحیح)round(
		pixels_to_points(-current_font_height, TRUE, (long_i)پوچ));
	vim_snprintf(buf, ARRAY_LENGTH(buf), "h%d", font_height);
	add_match((char_u *)buf);

	// Note: Keep this in sync with get_logfont(). Don't include 'c' and
	// 'q' as we fill in all the values below.
	ایستا نویسه *(p_gfn_win_opt_مقادیر[]) = {
	    "h" , "w" , "W" , "b" , "i" , "u" , "s"};
	برای (i = 0; i < (صحیح)ARRAY_LENGTH(p_gfn_win_opt_مقادیر); i++)
	    add_match((char_u *)p_gfn_win_opt_مقادیر[i]);

	برای (i = 0; i < (صحیح)ARRAY_LENGTH(charset_pairs); ++i)
	{
	    vim_snprintf(buf, اندازه(buf), "c%s", charset_pairs[i].name.string);
	    add_match((char_u *)buf);
	}

	برای (i = 0; i < (صحیح)ARRAY_LENGTH(quality_pairs); ++i)
	{
	    vim_snprintf(buf, اندازه(buf), "q%s", quality_pairs[i].name.string);
	    add_match((char_u *)buf);
	}
	بازگشت;
    }

    HWND	hwnd = GetDesktopWindow();
    HDC		hdc = GetWindowDC(hwnd);

    EnumFontFamiliesW(hdc,
	    پوچ,
	    (FONTENUMPROCW)expand_font_enumproc,
	    (LPARAM)add_match);

    ReleaseDC(hwnd, hdc);
}

/*
 * Compare a UTF-16 string and an ASCII string literally.
 * Only works all the code points are inside ASCII range.
 */
    ایستا صحیح
utf16ascncmp(ثابت WCHAR *w, ثابت نویسه *p, size_t n)
{
    size_t i;

    برای (i = 0; i < n; i++)
    {
	اگر (w[i] == 0 || w[i] != p[i])
	    بازگشت w[i] - p[i];
    }
    بازگشت 0;
}

/*
 * Equivalent of GetDpiForSystem().
 */
    UINT WINAPI
vimGetDpiForSystem(تهی)
{
    HWND hwnd = GetDesktopWindow();
    HDC hdc = GetWindowDC(hwnd);
    UINT dpi = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(hwnd, hdc);
    بازگشت dpi;
}

/*
 * Set default logfont based on current language.
 */
    ایستا تهی
set_default_logfont(LOGFONTW *lf)
{
    // Default font name for current language on MS-Windows.
    // If not translated, falls back to "Consolas".
    // This must be a fixed-pitch font.
    ثابت نویسه *defaultfontname = N_("DefaultFontNameForWindows");
    نویسه *fontname = _(defaultfontname);

    اگر (STRCMP(fontname, defaultfontname) == 0)
	fontname = "Consolas";

    *lf = s_lfDefault;
    lf->lfHeight = DEFAULT_FONT_HEIGHT * (صحیح)vimGetDpiForSystem() / 96;
    اگر (current_font_height == 0)
	current_font_height = lf->lfHeight;

    WCHAR *wfontname = enc_to_utf16((char_u*)fontname, پوچ);
    اگر (wfontname != پوچ)
    {
	wcscpy_s(lf->lfFaceName, LF_FACESIZE, wfontname);
	vim_free(wfontname);
    }
}

/*
 * Get font info from "name" into logfont "lf".
 * Return OK for a valid name, FAIL otherwise.
 */
    صحیح
get_logfont(
    LOGFONTW	*lf,
    char_u	*name,
    HDC		printer_dc,
    صحیح		verbose)
{
    WCHAR	*p;
    صحیح		i;
    صحیح		ret = FAIL;
    ایستا LOGFONTW *lastlf = پوچ;
    WCHAR	*wname;

    set_default_logfont(lf);
    اگر (name == پوچ)
	بازگشت OK;

    wname = enc_to_utf16(name, پوچ);
    اگر (wname == پوچ)
	بازگشت FAIL;

    اگر (wcscmp(wname, L"*") == 0)
    {
# اگر defined(FEAT_GUI_MSWIN)
	CHOOSEFONTW	cf;
	// if name is "*", bring up std font dialog:
	CLEAR_FIELD(cf);
	cf.lStructSize = اندازه(cf);
	cf.hwndOwner = s_hwnd;
	cf.Flags = CF_SCREENFONTS | CF_FIXEDPITCHONLY | CF_INITTOLOGFONTSTRUCT;
	اگر (lastlf != پوچ)
	    *lf = *lastlf;
	cf.lpLogFont = lf;
	cf.nFontType = 0 ; //REGULAR_FONTTYPE;
	اگر (ChooseFontW(&cf))
	    ret = OK;
# endif
	برو theend;
    }

    /*
     * Split name up, it could be <name>:h<height>:w<width> etc.
     */
    برای (p = wname; *p && *p != L':'; p++)
    {
	اگر (p - wname + 1 >= LF_FACESIZE)
	    برو theend;			// Name too long
	lf->lfFaceName[p - wname] = *p;
    }
    اگر (p != wname)
	lf->lfFaceName[p - wname] = NUL;

    // First set defaults
    lf->lfHeight = DEFAULT_FONT_HEIGHT * (صحیح)vimGetDpiForSystem() / 96;
    lf->lfWidth = 0;
    lf->lfWeight = FW_NORMAL;
    lf->lfItalic = FALSE;
    lf->lfUnderline = FALSE;
    lf->lfStrikeOut = FALSE;

    /*
     * If the font can't be found, try replacing '_' by ' '.
     */
    اگر (init_logfont(lf) == FAIL)
    {
	صحیح	did_replace = FALSE;

	برای (i = 0; lf->lfFaceName[i]; ++i)
	    اگر (lf->lfFaceName[i] == L'_')
	    {
		lf->lfFaceName[i] = L' ';
		did_replace = TRUE;
	    }
	اگر (!did_replace || init_logfont(lf) == FAIL)
	    برو theend;
    }

    وقتی (*p == L':')
	p++;

    // Set the values found after ':'
    وقتی (*p)
    {
	انتخاب (*p++)
	{
	    // Note: Keep this in sync with gui_mch_expand_font().
	    حالت L'h':
		lf->lfHeight = - points_to_pixels(p, &p, TRUE, (long_i)printer_dc);
		بشکن;
	    حالت L'w':
		lf->lfWidth = points_to_pixels(p, &p, FALSE, (long_i)printer_dc);
		بشکن;
	    حالت L'W':
		lf->lfWeight = wcstol(p, &p, 10);
		بشکن;
	    حالت L'b':
		lf->lfWeight = FW_BOLD;
		بشکن;
	    حالت L'i':
		lf->lfItalic = TRUE;
		بشکن;
	    حالت L'u':
		lf->lfUnderline = TRUE;
		بشکن;
	    حالت L's':
		lf->lfStrikeOut = TRUE;
		بشکن;
	    حالت L'c':
		برای (i = 0; i < (صحیح)ARRAY_LENGTH(charset_pairs); ++i)
		{
		    اگر (utf16ascncmp(p, (نویسه *)charset_pairs[i].name.string,
			    charset_pairs[i].name.length) == 0)
		    {
			lf->lfCharSet = charset_pairs[i].charset;
			p += charset_pairs[i].name.length;
			بشکن;
		    }
		}

		اگر (i == (صحیح)ARRAY_LENGTH(charset_pairs) && verbose)
		{
		    char_u *s = utf16_to_enc(p, پوچ);
		    اگر (s != پوچ)
		    {
			semsg(_(e_illegal_str_name_str_in_font_name_str),
							   "charset", s, name);
			vim_free(s);
		    }
		}
		بشکن;
	    حالت L'f':
		// Font features (e.g., "fss19=1").
		// Parsed separately by gui_mch_init_font(); skip here.
		وقتی (*p && *p != L':')
		    p++;
		بشکن;
	    حالت L'q':
		برای (i = 0; i < (صحیح)ARRAY_LENGTH(quality_pairs); ++i)
		{
		    اگر (utf16ascncmp(p, (نویسه *)quality_pairs[i].name.string,
			    quality_pairs[i].name.length) == 0)
		    {
			lf->lfQuality = quality_pairs[i].quality;
			p += quality_pairs[i].name.length;
			بشکن;
		    }
		}

		اگر (i == (صحیح)ARRAY_LENGTH(quality_pairs) && verbose)
		{
		    char_u *s = utf16_to_enc(p, پوچ);
		    اگر (s != پوچ)
		    {
			semsg(_(e_illegal_str_name_str_in_font_name_str),
							   "quality", s, name);
			vim_free(s);
		    }
		}
		بشکن;
	    پیشفرض:
		اگر (verbose)
		    semsg(_(e_illegal_char_nr_in_font_name_str), p[-1], name);
		برو theend;
	}
	وقتی (*p == L':')
	    p++;
    }
    ret = OK;

theend:
    // ron: init lastlf
    اگر (ret == OK && printer_dc == پوچ)
    {
	vim_free(lastlf);
	lastlf = ALLOC_ONE(LOGFONTW);
	اگر (lastlf != پوچ)
	    mch_memmove(lastlf, lf, اندازه(LOGFONTW));
    }
    vim_free(wname);

    بازگشت ret;
}

#endif // defined(FEAT_GUI) || defined(FEAT_PRINTER)

#اگر defined(FEAT_JOB_CHANNEL)
/*
 * Initialize the Winsock dll.
 */
    تهی
channel_init_winsock(تهی)
{
    WSADATA wsaData;
    صحیح wsaerr;

    اگر (WSInitialized)
	بازگشت;

    wsaerr = WSAStartup(MAKEWORD(2, 2), &wsaData);
    اگر (wsaerr == 0)
	WSInitialized = TRUE;
}
#endif
