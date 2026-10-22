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

#அந்தர்பாவ "vim.h"

#அந்தர்பாவ <sys/types.h>
#அந்தர்பாவ <limits.h>

#அந்தர்பாவ <process.h>
#அந்தர்பாவ <direct.h>

#எனில் !defined(FEAT_GUI_MSWIN)
# include <shellapi.h>
#endif

#எனில் defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT)
# include <dlgs.h>
# include <winspool.h>
# include <commdlg.h>
#endif

// Record all output and all keyboard & mouse input
// #define MCH_WRITE_DUMP

#ifdef MCH_WRITE_DUMP
FILE* fdDump = வெற்று;
#endif

#எனில் !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
வெளி எழுத்து g_szOrigTitle[];
#endif

#ifdef FEAT_GUI
வெளி HWND s_hwnd;
#இல்லையெனில்
நிலையான HWND s_hwnd = 0;	    // console window handle, set by GetConsoleHwnd()
#endif

#ifdef FEAT_JOB_CHANNEL
முழு WSInitialized = FALSE; // WinSock is initialized
#endif


/*
 * Save the instance handle of the exe/dll.
 */
    வெறுமை
SaveInst(HINSTANCE hInst)
{
    g_hinst = hInst;
}

#எனில் defined(FEAT_GUI_MSWIN)
/*
 * GUI version of mch_exit().
 * Shut down and exit with status `r'
 * Careful: mch_exit() may be called before mch_init()!
 */
    வெறுமை
mch_exit_g(முழு r)
{
    exiting = TRUE;

    display_errors();

    ml_close_all(TRUE);		// remove all memfiles

# ifdef FEAT_OLE
    UninitOLE();
# endif
# ifdef FEAT_JOB_CHANNEL
    எனில் (WSInitialized)
    {
	WSInitialized = FALSE;
	WSACleanup();
    }
# endif
# ifdef DYNAMIC_GETTEXT
    dyn_libintl_end();
# endif

    எனில் (gui.in_use)
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
வகைவரை BOOL (WINAPI *PfnRtlGetVersion)(LPOSVERSIONINFOW);
DWORD win_version;
    நிலையான வெறுமை
win_version_init(வெறுமை)
{
    OSVERSIONINFOW	osver;
    HMODULE		hNtdll;
    PfnRtlGetVersion	pRtlGetVersion;

    hNtdll = GetModuleHandle("ntdll.dll");
    எனில் (hNtdll == வெற்று)
	திரும்பு;

    pRtlGetVersion =
	(PfnRtlGetVersion) GetProcAddress(hNtdll, "RtlGetVersion");
    எனில் (pRtlGetVersion == வெற்று)
	திரும்பு;

    osver.dwOSVersionInfoSize = அளவு(OSVERSIONINFOW);
    pRtlGetVersion(&osver);
    win_version = MAKE_VER(min(osver.dwMajorVersion, 0xFF),
	    min(osver.dwMinorVersion, 0xFF),
	    min(osver.dw_Build_எண், 0xFFFF));
}

/*
 * Init the tables for toupper() and tolower().
 */
    வெறுமை
mch_early_init(வெறுமை)
{
    முழு		i;

    win_version_init();

    PlatformId();

    // Init the tables for toupper() and tolower()
    க்கு (i = 0; i < 256; ++i)
	toupper_tab[i] = tolower_tab[i] = i;
    CharUpperBuff((LPSTR)toupper_tab, 256);
    CharLowerBuff((LPSTR)tolower_tab, 256);
}


/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    முழு
mch_உள்ளீடு_isatty(வெறுமை)
{
#ifdef FEAT_GUI_MSWIN
# ifdef VIMDLL
    எனில் (gui.in_use)
# endif
	திரும்பு TRUE;	    // GUI always has a tty
#endif
#எனில் !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    எனில் (isatty(read_cmd_fd))
	திரும்பு TRUE;
    திரும்பு FALSE;
#endif
}

/*
 * mch_settitle(): set titlebar of our window
 */
    வெறுமை
mch_settitle(
    char_u *title,
    char_u *icon UNUSED)
{
#ifdef FEAT_GUI_MSWIN
# ifdef VIMDLL
    எனில் (gui.in_use)
# endif
    {
	gui_mch_settitle(title, icon);
	திரும்பு;
    }
#endif
#எனில் !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    எனில் (title != வெற்று)
    {
	WCHAR	*wp = enc_to_utf16(title, வெற்று);

	எனில் (wp == வெற்று)
	    திரும்பு;

	SetConsoleTitleW(wp);
	vim_free(wp);
	திரும்பு;
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
    வெறுமை
mch_restore_title(முழு which UNUSED)
{
#எனில் !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
    எனில் (!gui.in_use)
# endif
	SetConsoleTitle(g_szOrigTitle);
#endif
}


/*
 * Return TRUE if we can restore the title (we can)
 */
    முழு
mch_can_restore_title(வெறுமை)
{
    திரும்பு TRUE;
}


/*
 * Return TRUE if we can restore the icon title (we can't)
 */
    முழு
mch_can_restore_icon(வெறுமை)
{
    திரும்பு FALSE;
}


/*
 * Get absolute file name into buffer "buf" of length "len" bytes,
 * turning all '/'s into '\\'s and getting the correct case of each component
 * of the file name.  Append a (back)slash to a directory name.
 * When 'shellslash' set do it the other way around.
 * Return OK or FAIL.
 */
    முழு
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    முழு		len,
    முழு		force UNUSED)
{
    முழு		nResult = FAIL;
    WCHAR	*wname;
    WCHAR	wbuf[MAX_PATH];
    char_u	*cname = வெற்று;

    wname = enc_to_utf16(fname, வெற்று);
    எனில் (wname != வெற்று && _wfullpath(wbuf, wname, MAX_PATH) != வெற்று)
    {
	cname = utf16_to_enc((short_u *)wbuf, வெற்று);
	எனில் (cname != வெற்று)
	{
	    vim_strncpy(buf, cname, len - 1);
	    nResult = OK;
	}
    }
    vim_free(wname);
    vim_free(cname);

#ifdef USE_FNAME_CASE
    fname_case(buf, len);
#இல்லையெனில்
    slash_adjust(buf);
#endif

    திரும்பு nResult;
}


/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    முழு
mch_isFullName(char_u *fname)
{
    // A name like "d:/foo" and "//server/share" is absolute.  "d:foo" is not.
    // /foo and \foo are absolute too because Windows keeps a current drive.
    // Another way to check is to use mch_FullName() and see if the result is
    // the same as the name or mch_FullName() fails.  However, this has quite a
    // bit of overhead, so let's not do that.
    எனில் (*fname == NUL)
	திரும்பு FALSE;
    திரும்பு ((ASCII_ISALPHA(fname[0]) && fname[1] == ':'
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
    வெறுமை
slash_adjust(char_u *p)
{
    எனில் (path_with_url(p))
	திரும்பு;

    எனில் (*p == '`')
    {
	size_t len = STRLEN(p);

	// don't replace backslash in backtick quoted strings
	எனில் (len > 2 && *(p + len - 1) == '`')
	    திரும்பு;
    }

    போது (*p)
    {
	எனில் (*p == psepcN)
	    *p = psepc;
	MB_PTR_ADV(p);
    }
}

    நிலையான முழு
read_reparse_point(மாறிலி WCHAR *name, char_u *buf, DWORD *buf_len)
{
    HANDLE h;
    BOOL ok;

    h = CreateFileW(name, FILE_READ_ATTRIBUTES,
	    FILE_SHARE_READ | FILE_SHARE_WRITE, வெற்று,
	    OPEN_EXISTING,
	    FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,
	    வெற்று);
    எனில் (h == INVALID_HANDLE_மதிப்பு)
	திரும்பு FAIL;

    ok = DeviceIoControl(h, FSCTL_GET_REPARSE_POINT, வெற்று, 0, buf, *buf_len,
	    buf_len, வெற்று);
    CloseHandle(h);

    திரும்பு ok ? OK : FAIL;
}

    char_u *
resolve_appexeclink(char_u *fname)
{
    DWORD		attr = 0;
    முழு			idx;
    WCHAR		*p, *end, *wname;
    // The buffer size is arbitrarily chosen to be "big enough" (TM), the
    // ceiling should be around 16k.
    char_u		buf[4096];
    DWORD		buf_len = அளவு(buf);
    REPARSE_DATA_BUFFER *rb = (REPARSE_DATA_BUFFER *)buf;

    wname = enc_to_utf16(fname, வெற்று);
    எனில் (wname == வெற்று)
	திரும்பு வெற்று;

    attr = GetFileAttributesW(wname);
    எனில் (attr == INVALID_FILE_ATTRIBUTES ||
	    (attr & FILE_ATTRIBUTE_REPARSE_POINT) == 0)
    {
	vim_free(wname);
	திரும்பு வெற்று;
    }

    // The applinks are similar to symlinks but with a huge difference: they can
    // only be executed, any other I/O operation on them is bound to fail with
    // ERROR_FILE_NOT_FOUND even though the file exists.
    எனில் (read_reparse_point(wname, buf, &buf_len) == FAIL)
    {
	vim_free(wname);
	திரும்பு வெற்று;
    }
    vim_free(wname);

    எனில் (rb->ReparseTag != IO_REPARSE_TAG_APPEXECLINK)
	திரும்பு வெற்று;

    // The (undocumented) reparse buffer contains a set of N null-terminated
    // Unicode strings, the application path is stored in the third one.
    எனில் (rb->AppExecLinkReparseBuffer.String_மொத்தம் < 3)
	திரும்பு வெற்று;

    p = rb->AppExecLinkReparseBuffer.StringList;
    end = p + rb->ReparseDataLength / அளவு(WCHAR);
    க்கு (idx = 0; p < end
	    && idx < (முழு)rb->AppExecLinkReparseBuffer.String_மொத்தம்
	    && idx != 2; )
    {
	எனில் (*p++ == L'\0')
	    ++idx;
    }

    திரும்பு utf16_to_enc(p, வெற்று);
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
    நிலையான முழு
mswin_stat_impl(மாறிலி WCHAR *name, stat_T *stp, மாறிலி முழு resolve)
{
    முழு			n;
    முழு			fd;
    BOOL		is_symlink = FALSE;
    HANDLE		hFind, h;
    DWORD		attr = 0;
    DWORD		flag = 0;
    WIN32_FIND_DATAW    findDataW;

#எனில் 0 && defined(_UCRT)
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
    எனில் (resolve)
	// Universal CRT can handle symlinks properly.
	திரும்பு _wstat(name, stp);
#endif

    // Only reparse points need a directory query to identify symbolic links.
    // In particular, avoid enumerating the directory for each missing file
    // while searching 'runtimepath'.
    attr = GetFileAttributesW(name);
    எனில் (attr == INVALID_FILE_ATTRIBUTES)
    {
	DWORD error = GetLastError();

	// A missing file or path cannot provide stat information.  Other
	// errors and UNC paths still need the CRT fallback.
	எனில் ((error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND)
		&& !((name[0] == L'\\' || name[0] == L'/')
		    && (name[1] == L'\\' || name[1] == L'/')))
	{
	    errno = ENOENT;
	    திரும்பு -1;
	}
	திரும்பு _wstat(name, stp);
    }
    எனில் (!(attr & FILE_ATTRIBUTE_REPARSE_POINT))
	திரும்பு _wstat(name, stp);

    hFind = FindFirstFileW(name, &findDataW);
    எனில் (hFind != INVALID_HANDLE_மதிப்பு)
    {
	attr = findDataW.dwFileAttributes;
	எனில் ((attr & FILE_ATTRIBUTE_REPARSE_POINT)
		&& (findDataW.dwReserved0 == IO_REPARSE_TAG_SYMLINK))
	    is_symlink = TRUE;
	FindClose(hFind);
    }

    // Use the plain old stat() whenever it's possible.
    எனில் (!is_symlink)
	திரும்பு _wstat(name, stp);

    எனில் (!resolve && is_symlink)
	flag = FILE_FLAG_OPEN_REPARSE_POINT;
    எனில் (attr & FILE_ATTRIBUTE_DIRECTORY)
	flag |= FILE_FLAG_BACKUP_SEMANTICS;

    h = CreateFileW(name, FILE_READ_ATTRIBUTES,
	    FILE_SHARE_READ | FILE_SHARE_WRITE, வெற்று, OPEN_EXISTING, flag,
	    வெற்று);
    எனில் (h == INVALID_HANDLE_மதிப்பு)
	திரும்பு -1;

    fd = _open_osfhandle((intptr_t)h, _O_RDONLY);
    n = _fstat(fd, (கட்டமைப்பு _stat *)stp);
    எனில் ((n == 0) && (attr & FILE_ATTRIBUTE_DIRECTORY))
	stp->st_mode = (stp->st_mode & ~S_IFMT) | S_IFDIR;
    _close(fd);

    திரும்பு n;
}

/*
 * stat() can't handle a trailing '/' or '\', remove it first.
 * When 'resolve' is true behave as lstat() wrt symlinks.
 */
    நிலையான முழு
stat_impl(மாறிலி எழுத்து *name, stat_T *stp, மாறிலி முழு resolve)
{
    // WinNT and later can use _MAX_PATH wide characters for a pathname, which
    // means that the maximum pathname is _MAX_PATH * 3 bytes when 'enc' is
    // UTF-8.
    char_u	buf[_MAX_PATH * 3 + 1];
    size_t	buflen;
    char_u	*p;
    WCHAR	*wp;
    முழு		n;

    vim_strncpy((char_u *)buf, (char_u *)name, அளவு(buf) - 1);
    buflen = STRLEN(buf);
    p = buf + buflen;
    எனில் (p > buf)
	MB_PTR_BACK(buf, p);

    // Remove trailing '\\' except root path.
    எனில் (p > buf && (*p == '\\' || *p == '/') && p[-1] != ':')
    {
	*p = NUL;
	buflen = (size_t)(p - buf);
    }

    எனில் ((buf[0] == '\\' && buf[1] == '\\') || (buf[0] == '/' && buf[1] == '/'))
    {
	// UNC root path must be followed by '\\'.
	p = vim_strpbrk(buf + 2, (char_u *)"\\/");
	எனில் (p != வெற்று)
	{
	    p = vim_strpbrk(p + 1, (char_u *)"\\/");
	    எனில் (p == வெற்று)
		STRCPY(buf + buflen, "\\");
	}
    }

    wp = enc_to_utf16(buf, வெற்று);
    எனில் (wp == வெற்று)
	திரும்பு -1;

    n = mswin_stat_impl(wp, stp, resolve);
    vim_free(wp);
    திரும்பு n;
}

    முழு
vim_lstat(மாறிலி எழுத்து *name, stat_T *stp)
{
    திரும்பு stat_impl(name, stp, FALSE);
}

    முழு
vim_stat(மாறிலி எழுத்து *name, stat_T *stp)
{
    திரும்பு stat_impl(name, stp, TRUE);
}

#எனில் defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    வெறுமை
mch_settmode(tmode_T tmode UNUSED)
{
    // nothing to do
}

    முழு
mch_get_shellsize(வெறுமை)
{
    // never used
    திரும்பு OK;
}

    வெறுமை
mch_set_shellsize(வெறுமை)
{
    // never used
}

/*
 * Rows and/or Columns has changed.
 */
    வெறுமை
mch_new_shellsize(வெறுமை)
{
    // never used
}

    வெறுமை
mch_calc_cell_size(கட்டமைப்பு cellsize *cs_out UNUSED)
{
    // never used
}

#endif

/*
 * We have no job control, so fake it by starting a new shell.
 */
    வெறுமை
mch_suspend(வெறுமை)
{
    suspend_shell();
}

#எனில் defined(USE_MCH_ERRMSG)

# ifdef display_errors
#  undef display_errors
# endif

/*
 * Display the saved error message(s).
 */
    வெறுமை
display_errors(வெறுமை)
{
# ifdef FEAT_GUI
    char_u *p;

#  ifdef VIMDLL
    எனில் (gui.in_use || gui.starting)
#  endif
    {
	எனில் (error_ga.ga_data != வெற்று)
	{
	    // avoid putting up a message box with blanks only
	    க்கு (p = (char_u *)error_ga.ga_data; *p; ++p)
		எனில் (!SAFE_isspace(*p))
		{
		    // Only use a dialog when not using --gui-dialog-file:
		    // write text to a file.
		    எனில் (!gui_dialog_log((char_u *)"Errors", p))
			(வெறுமை)gui_mch_dialog(
				     gui.starting ? VIM_INFO :
					     VIM_ERROR,
				     gui.starting ? (char_u *)_("Message") :
					     (char_u *)_("Error"),
					     p, (char_u *)_("&Ok"),
					1, வெற்று, FALSE);
		    நிறுத்து;
		}
	    ga_clear(&error_ga);
	}
	திரும்பு;
    }
# endif
# எனில் !defined(FEAT_GUI) || defined(VIMDLL)
    FlushFileBuffers(GetStdHandle(STD_ERROR_HANDLE));
# endif
}
#endif


/*
 * Return TRUE if "p" contain a wildcard that can be expanded by
 * dos_expandpath().
 */
    முழு
mch_has_exp_wildcard(char_u *p)
{
    க்கு ( ; *p; MB_PTR_ADV(p))
    {
	எனில் (vim_strchr((char_u *)"?*[", *p) != வெற்று
		|| (*p == '~' && p[1] != NUL))
	    திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

/*
 * Return TRUE if "p" contain a wildcard or a "~1" kind of thing (could be a
 * shortened file name).
 */
    முழு
mch_has_wildcard(char_u *p)
{
    க்கு ( ; *p; MB_PTR_ADV(p))
    {
	எனில் (vim_strchr((char_u *)"?*$[`", *p) != வெற்று
		|| (*p == '~' && p[1] != NUL))
	    திரும்பு TRUE;
    }
    திரும்பு FALSE;
}


/*
 * The normal _chdir() does not change the default drive.  This one does.
 * Returning 0 implies success; -1 implies failure.
 */
    முழு
mch_chdir(எழுத்து *path)
{
    WCHAR   *p;
    முழு	    n;

    எனில் (path[0] == NUL)		// just checking...
	திரும்பு -1;

    எனில் (p_verbose >= 5)
    {
	verbose_enter();
	smsg("chdir(%s)", path);
	verbose_leave();
    }
    எனில் (SAFE_isalpha(path[0]) && path[1] == ':')	// has a drive name
    {
	// If we can change to the drive, skip that part of the path.  If we
	// can't then the current directory may be invalid, try using chdir()
	// with the whole path.
	எனில் (_chdrive(TOLOWER_ASC(path[0]) - 'a' + 1) == 0)
	    path += 2;
    }

    எனில் (*path == NUL)		// drive name only
	திரும்பு 0;

    p = enc_to_utf16((char_u *)path, வெற்று);
    எனில் (p == வெற்று)
	திரும்பு -1;

    n = _wchdir(p);
    vim_free(p);
    திரும்பு n;
}


#எனில் defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
/*
 * return non-zero if a character is available
 */
    முழு
mch_char_avail(வெறுமை)
{
    // never used
    திரும்பு TRUE;
}

# எனில் defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    முழு
mch_check_messages(வெறுமை)
{
    // TODO: check for messages
    திரும்பு TRUE;
}
# endif
#endif


#எனில் defined(FEAT_LIBCALL)
/*
 * Call a DLL routine which takes either a string or int param
 * and returns an allocated string.
 * Return OK if it worked, FAIL if not.
 */
வகைவரை char_u *(*MYSTRPROCSTR)(char_u *);
வகைவரை முழு (*MYSTRPROCINT)(char_u *);
வகைவரை char_u *(*MYINTPROCSTR)(முழு);
வகைவரை முழு (*MYINTPROCINT)(முழு);

/*
 * Check if a pointer points to a valid NUL terminated string.
 * Return the length of the string, including terminating NUL.
 * Returns 0 for an invalid pointer, 1 for an empty string.
 */
    நிலையான size_t
check_str_len(char_u *str)
{
    SYSTEM_INFO			si;
    MEMORY_BASIC_INFORMATION	mbi;
    size_t			length = 0;
    size_t			i;
    மாறிலி char_u		*p;

    // get page size
    GetSystemInfo(&si);

    // get memory information
    எனில் (!VirtualQuery(str, &mbi, அளவு(mbi)))
	திரும்பு 0;

    // pre cast these (typing savers)
    long_u dwStr = (long_u)str;
    long_u dwBaseAddress = (long_u)mbi.BaseAddress;

    // get start address of page that str is on
    long_u strPage = dwStr - (dwStr - dwBaseAddress) % si.dwPageSize;

    // get length from str to end of page
    long_u pageLength = si.dwPageSize - (dwStr - strPage);

    க்கு (p = str; !IsBadReadPtr(p, (UINT)pageLength);
	    p += pageLength, pageLength = si.dwPageSize)
	க்கு (i = 0; i < pageLength; ++i, ++length)
	    எனில் (p[i] == NUL)
		திரும்பு length + 1;

    திரும்பு 0;
}

/*
 * Passed to do_in_runtimepath() to load a vim.ico file.
 */
    நிலையான வெறுமை
mch_icon_load_cb(char_u *fname, வெறுமை *cookie)
{
    HANDLE *h = (HANDLE *)cookie;

    *h = LoadImage(வெற்று,
		   (LPSTR)fname,
		   IMAGE_ICON,
		   64,
		   64,
		   LR_LOADFROMFILE | LR_LOADMAP3DCOLORS);
}

/*
 * Try loading an icon file from 'runtimepath'.
 */
    முழு
mch_icon_load(HANDLE *iconp)
{
    திரும்பு do_in_runtimepath((char_u *)"bitmaps/vim.ico",
						  0, mch_icon_load_cb, iconp);
}

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if the OS PRNG is not available or something went wrong.
 */
    முழு
mch_get_random(char_u *buf, முழு len)
{
    நிலையான முழு		initialized = NOTDONE;
    நிலையான HINSTANCE	hInstLib;
    நிலையான BOOL (WINAPI *pProcessPrng)(PUCHAR, ULONG);

    எனில் (initialized == NOTDONE)
    {
	hInstLib = vimLoadLib("bcryptprimitives.dll");
	எனில் (hInstLib != வெற்று)
	    pProcessPrng = (வெறுமை *)GetProcAddress(hInstLib, "ProcessPrng");
	எனில் (hInstLib == வெற்று || pProcessPrng == வெற்று)
	{
	    FreeLibrary(hInstLib);
	    initialized = FAIL;
	}
	இல்லையெனில்
	    initialized = OK;
    }

    எனில் (initialized == FAIL)
	திரும்பு FAIL;

    // According to the documentation this call cannot fail.
    pProcessPrng(buf, len);

    திரும்பு OK;
}

    முழு
mch_libcall(
    char_u	*libname,
    char_u	*funcname,
    char_u	*argstring,	// NULL when using a argint
    முழு		argint,
    char_u	**string_result,// NULL when using number_result
    முழு		*எண்_result)
{
    HINSTANCE		hinstLib;

    BOOL fRunTimeLinkSuccess = FALSE;

    // Get a handle to the DLL module.
    hinstLib = vimLoadLib((எழுத்து *)libname);

    // If the handle is valid, try to get the function address.
    எனில் (hinstLib != வெற்று)
    {
	char_u	*retval_str = வெற்று;
	முழு	retval_int = 0;
	size_t	len;

# ifdef HAVE_TRY_EXCEPT
	__try
	{
# endif
	எனில் (argstring != வெற்று)
	{
	    // Call with string argument
	    எனில் (string_result != வெற்று)
	    {
		MYSTRPROCSTR proc;

		proc = (MYSTRPROCSTR)GetProcAddress(hinstLib, (LPCSTR)funcname);
		எனில் (proc != வெற்று)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_str = proc(argstring);
		}
	    }
	    இல்லையெனில்
	    {
		MYSTRPROCINT proc;

		proc = (MYSTRPROCINT)GetProcAddress(hinstLib, (LPCSTR)funcname);
		எனில் (proc != வெற்று)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_int = proc(argstring);
		}
	    }
	}
	இல்லையெனில்
	{
	    // Call with number argument
	    எனில் (string_result != வெற்று)
	    {
		MYINTPROCSTR proc;

		proc = (MYINTPROCSTR)GetProcAddress(hinstLib, (LPCSTR)funcname);
		எனில் (proc != வெற்று)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_str = proc(argint);
		}
	    }
	    இல்லையெனில்
	    {
		MYINTPROCINT proc;

		proc = (MYINTPROCINT)GetProcAddress(hinstLib, (LPCSTR)funcname);
		எனில் (proc != வெற்று)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_int = proc(argint);
		}
	    }
	}

	// Save the string before we free the library.
	// Assume that a "1" result is an illegal pointer.
	எனில் (string_result == வெற்று)
	    *எண்_result = retval_int;
	இல்லையெனில் எனில் (retval_str != வெற்று
		&& (len = check_str_len(retval_str)) > 0)
	{
	    *string_result = alloc(len);
	    எனில் (*string_result != வெற்று)
		mch_memmove(*string_result, retval_str, len);
	}

# ifdef HAVE_TRY_EXCEPT
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	    எனில் (GetExceptionCode() == EXCEPTION_அடுக்கு_OVERFLOW)
		_resetstkoflw();
	    fRunTimeLinkSuccess = FALSE;
	}
# endif

	// Free the DLL module.
	(வெறுமை)FreeLibrary(hinstLib);
    }

    எனில் (!fRunTimeLinkSuccess)
    {
	semsg(_(e_library_call_failed_for_str), funcname);
	திரும்பு FAIL;
    }

    திரும்பு OK;
}
#endif

/*
 * Debugging helper: expose the MCH_WRITE_DUMP stuff to other modules
 */
    வெறுமை
DumpPutS(மாறிலி எழுத்து *psz UNUSED)
{
#ifdef MCH_WRITE_DUMP
    எனில் (fdDump)
    {
	fputs(psz, fdDump);
	எனில் (psz[strlen(psz) - 1] != '\n')
	    fputc('\n', fdDump);
	fflush(fdDump);
    }
#endif
}

#ifdef _DEBUG

வெறுமை __cdecl
Trace(
    எழுத்து *pszFormat,
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

#எனில் !defined(FEAT_GUI) || defined(VIMDLL)
வெளி HWND g_hWnd;	// This is in os_win32.c.

/*
 * Showing the printer dialog is tricky since we have no GUI
 * window to parent it. The following routines are needed to
 * get the window parenting and Z-order to work properly.
 */
    நிலையான வெறுமை
GetConsoleHwnd(வெறுமை)
{
    // Skip if it's already set.
    எனில் (s_hwnd != 0)
	திரும்பு;

    // Window handle may have been found by init code (Windows NT only)
    எனில் (g_hWnd != 0)
    {
	s_hwnd = g_hWnd;
	திரும்பு;
    }

    s_hwnd = GetConsoleWindow();
}

/*
 * Console implementation of ":winpos".
 */
    முழு
mch_get_winpos(முழு *x, முழு *y)
{
    RECT  rect;

    GetConsoleHwnd();
    GetWindowRect(s_hwnd, &rect);
    *x = rect.இடது;
    *y = rect.உச்சி;
    திரும்பு OK;
}

/*
 * Console implementation of ":winpos x y".
 */
    வெறுமை
mch_set_winpos(முழு x, முழு y)
{
    GetConsoleHwnd();
    SetWindowPos(s_hwnd, வெற்று, x, y, 0, 0,
		 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}
#endif

#எனில் defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT)

//=================================================================
// Win32 printer stuff

நிலையான HFONT		prt_font_handles[2][2][2];
நிலையான PRINTDLGW	prt_dlg;
நிலையான மாறிலி முழு	boldface[2] = {FW_REGULAR, FW_BOLD};
நிலையான TEXTMETRIC	prt_tm;
நிலையான முழு		prt_வரி_height;
நிலையான முழு		prt_எண்_width;
நிலையான முழு		prt_இடது_margin;
நிலையான முழு		prt_வலது_margin;
நிலையான முழு		prt_உச்சி_margin;
நிலையான char_u		szAppName[] = TEXT("VIM");
நிலையான HWND		hDlgPrint;
நிலையான முழு		*bUserAbort = வெற்று;
நிலையான char_u		*prt_name = வெற்று;

// Defines which are also in vim.rc.
# define IDC_BOX1		400
# define IDC_PRINTTEXT1		401
# define IDC_PRINTTEXT2		402
# define IDC_PROGRESS		403

    நிலையான BOOL
vimSetDlgItemText(HWND hDlg, முழு nIDDlgItem, char_u *s)
{
    WCHAR   *wp;
    BOOL    ret;

    wp = enc_to_utf16(s, வெற்று);
    எனில் (wp == வெற்று)
	திரும்பு FALSE;

    ret = SetDlgItemTextW(hDlg, nIDDlgItem, wp);
    vim_free(wp);
    திரும்பு ret;
}

/*
 * Convert BGR to RGB for Windows GDI calls
 */
    நிலையான COLORREF
swap_me(COLORREF colorref)
{
    முழு  temp;
    எழுத்து *ptr = (எழுத்து *)&colorref;

    temp = *(ptr);
    *(ptr ) = *(ptr + 2);
    *(ptr + 2) = temp;
    திரும்பு colorref;
}

    நிலையான INT_PTR CALLBACK
PrintDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam UNUSED,
	LPARAM lParam UNUSED)
{
# ifdef FEAT_GETTEXT
    NONCLIENTMETRICS nm;
    நிலையான HFONT hfont;
# endif

    தேர்வு (message)
    {
	நிலை WM_INITDIALOG:
# ifdef FEAT_GETTEXT
	    nm.cbSize = அளவு(NONCLIENTMETRICS);
	    எனில் (SystemParametersInfo(
			SPI_GETNONCLIENTMETRICS,
			அளவு(NONCLIENTMETRICS),
			&nm,
			0))
	    {
		எழுத்து buff[MAX_PATH];
		முழு i;

		// Translate the dialog texts
		hfont = CreateFontIndirect(&nm.lfMessageFont);
		க்கு (i = IDC_PRINTTEXT1; i <= IDC_PROGRESS; i++)
		{
		    SendDlgItemMessage(hDlg, i, WM_SETFONT, (WPARAM)hfont, 1);
		    எனில் (GetDlgItemText(hDlg,i, buff, அளவு(buff)))
			vimSetDlgItemText(hDlg,i, (char_u *)_(buff));
		}
		SendDlgItemMessage(hDlg, IDCANCEL,
						WM_SETFONT, (WPARAM)hfont, 1);
		எனில் (GetDlgItemText(hDlg,IDCANCEL, buff, அளவு(buff)))
		    vimSetDlgItemText(hDlg,IDCANCEL, (char_u *)_(buff));
	    }
# endif
	    SetWindowText(hDlg, (LPCSTR)szAppName);
	    எனில் (prt_name != வெற்று)
	    {
		vimSetDlgItemText(hDlg, IDC_PRINTTEXT2, (char_u *)prt_name);
		VIM_CLEAR(prt_name);
	    }
	    EnableMenuItem(GetSystemMenu(hDlg, FALSE), SC_CLOSE, MF_GRAYED);
# எனில் !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	    எனில் (!gui.in_use)
#  endif
		Bring_Window_To_உச்சி(s_hwnd);
# endif
	    திரும்பு TRUE;

	நிலை WM_COMMAND:
	    *bUserAbort = TRUE;
	    EnableWindow(GetParent(hDlg), TRUE);
	    DestroyWindow(hDlg);
	    hDlgPrint = வெற்று;
# ifdef FEAT_GETTEXT
	    DeleteObject(hfont);
# endif
	    திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

    நிலையான BOOL CALLBACK
AbortProc(HDC hdcPrn UNUSED, முழு iCode UNUSED)
{
    செய்தி_2 செய்தி;

    போது (!*bUserAbort && PeekMessageW(&செய்தி, வெற்று, 0, 0, PM_REMOVE))
    {
	எனில் (!hDlgPrint || !IsDialogMessageW(hDlgPrint, &செய்தி))
	{
	    TranslateMessage(&செய்தி);
	    DispatchMessageW(&செய்தி);
	}
    }
    திரும்பு !*bUserAbort;
}

    நிலையான UINT_PTR CALLBACK
PrintHookProc(
	HWND hDlg,	// handle to dialog box
	UINT ui_செய்தி,	// message identifier
	WPARAM wParam UNUSED,	// message parameter
	LPARAM lParam	// message parameter
	)
{
    HWND	hwndOwner;
    RECT	rc, rcDlg, rcOwner;
    PRINTDLGW	*pPD;

    எனில் (ui_செய்தி != WM_INITDIALOG)
	திரும்பு FALSE;

    // Get the owner window and dialog box rectangles.
    எனில் ((hwndOwner = GetParent(hDlg)) == வெற்று)
	hwndOwner = GetDesktopWindow();

    GetWindowRect(hwndOwner, &rcOwner);
    GetWindowRect(hDlg, &rcDlg);
    CopyRect(&rc, &rcOwner);

    // Offset the owner and dialog box rectangles so that
    // right and bottom values represent the width and
    // height, and then offset the owner again to discard
    // space taken up by the dialog box.

    OffsetRect(&rcDlg, -rcDlg.இடது, -rcDlg.உச்சி);
    OffsetRect(&rc, -rc.இடது, -rc.உச்சி);
    OffsetRect(&rc, -rcDlg.வலது, -rcDlg.bottom);

    // The new position is the sum of half the remaining
    // space and the owner's original position.

    SetWindowPos(hDlg,
	    HWND_உச்சி,
	    rcOwner.இடது + (rc.வலது / 2),
	    rcOwner.உச்சி + (rc.bottom / 2),
	    0, 0,		// ignores size arguments
	    SWP_NOSIZE);

    //  tackle the printdlg copiesctrl problem
    pPD = (PRINTDLGW *)lParam;
    pPD->nCopies = (WORD)pPD->lCustData;
    SetDlgItemInt( hDlg, edt3, pPD->nCopies, FALSE );
    //  Bring the window to top
    Bring_Window_To_உச்சி(GetParent(hDlg));
    SetForegroundWindow(hDlg);

    திரும்பு FALSE;
}

    வெறுமை
mch_print_cleanup(வெறுமை)
{
    முழு pifItalic;
    முழு pifBold;
    முழு pifUnderline;

    க்கு (pifBold = 0; pifBold <= 1; pifBold++)
	க்கு (pifItalic = 0; pifItalic <= 1; pifItalic++)
	    க்கு (pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
		DeleteObject(prt_font_handles[pifBold][pifItalic][pifUnderline]);

    எனில் (prt_dlg.hDC != வெற்று)
	DeleteDC(prt_dlg.hDC);
    எனில் (!*bUserAbort && hDlgPrint != வெற்று)
	SendMessage(hDlgPrint, WM_COMMAND, 0, 0);
}

    நிலையான முழு
to_device_units(முழு idx, முழு dpi, முழு physsize, முழு offset, முழு def_எண்)
{
    முழு		ret = 0;
    முழு		u;
    முழு		nr;

    u = prt_get_unit(idx);
    எனில் (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_எண்;
    }
    இல்லையெனில்
	nr = printer_opts[idx].எண்;

    தேர்வு (u)
    {
	நிலை PRT_UNIT_PERC:
	    ret = (physsize * nr) / 100;
	    நிறுத்து;
	நிலை PRT_UNIT_INCH:
	    ret = (nr * dpi);
	    நிறுத்து;
	நிலை PRT_UNIT_MM:
	    ret = (nr * 10 * dpi) / 254;
	    நிறுத்து;
	நிலை PRT_UNIT_POINT:
	    ret = (nr * 10 * dpi) / 720;
	    நிறுத்து;
    }

    எனில் (ret < offset)
	திரும்பு 0;
    இல்லையெனில்
	திரும்பு ret - offset;
}

    நிலையான முழு
prt_get_cpl(வெறுமை)
{
    முழு		hr;
    முழு		phyw;
    முழு		dvoff;
    முழு		rev_offset;
    முழு		dpi;

    GetTextMetrics(prt_dlg.hDC, &prt_tm);
    prt_வரி_height = prt_tm.tmHeight + prt_tm.tmExternalLeading;

    hr	    = GetDeviceCaps(prt_dlg.hDC, HORZRES);
    phyw    = GetDeviceCaps(prt_dlg.hDC, PHYSICALWIDTH);
    dvoff   = GetDeviceCaps(prt_dlg.hDC, PHYSICALOFFSETX);
    dpi	    = GetDeviceCaps(prt_dlg.hDC, LOGPIXELSX);

    rev_offset = phyw - (dvoff + hr);

    prt_இடது_margin = to_device_units(OPT_PRINT_இடது, dpi, phyw, dvoff, 10);
    எனில் (prt_use_எண்())
    {
	prt_எண்_width = PRINT_எண்_WIDTH * prt_tm.tmAveCharWidth;
	prt_இடது_margin += prt_எண்_width;
    }
    இல்லையெனில்
	prt_எண்_width = 0;

    prt_வலது_margin = hr - to_device_units(OPT_PRINT_வலது, dpi, phyw,
							       rev_offset, 5);

    திரும்பு (prt_வலது_margin - prt_இடது_margin) / prt_tm.tmAveCharWidth;
}

    நிலையான முழு
prt_get_lpp(வெறுமை)
{
    முழு vr;
    முழு phyw;
    முழு dvoff;
    முழு rev_offset;
    முழு	bottom_margin;
    முழு	dpi;

    vr	    = GetDeviceCaps(prt_dlg.hDC, VERTRES);
    phyw    = GetDeviceCaps(prt_dlg.hDC, PHYSICALHEIGHT);
    dvoff   = GetDeviceCaps(prt_dlg.hDC, PHYSICALOFFSETY);
    dpi	    = GetDeviceCaps(prt_dlg.hDC, LOGPIXELSY);

    rev_offset = phyw - (dvoff + vr);

    prt_உச்சி_margin = to_device_units(OPT_PRINT_உச்சி, dpi, phyw, dvoff, 5);

    // adjust top margin if there is a header
    prt_உச்சி_margin += prt_வரி_height * prt_header_height();

    bottom_margin = vr - to_device_units(OPT_PRINT_BOT, dpi, phyw,
							       rev_offset, 5);

    திரும்பு (bottom_margin - prt_உச்சி_margin) / prt_வரி_height;
}

    முழு
mch_print_init(prt_settings_T *psettings, char_u *jobname, முழு forceit)
{
    நிலையான HGLOBAL	stored_dm = வெற்று;
    நிலையான HGLOBAL	stored_devn = வெற்று;
    நிலையான முழு		stored_nCopies = 1;
    நிலையான முழு		stored_nFlags = 0;

    LOGFONTW		fLogFont;
    முழு			pifItalic;
    முழு			pifBold;
    முழு			pifUnderline;

    DEVMODEW		*mem;
    DEVNAMES		*devname;
    முழு			i;
    DWORD		err;

    bUserAbort = &(psettings->user_abort);
    CLEAR_FIELD(prt_dlg);
    prt_dlg.lStructSize = அளவு(PRINTDLGW);
# எனில் !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
    எனில் (!gui.in_use)
#  endif
	GetConsoleHwnd();	    // get value of s_hwnd
# endif
    prt_dlg.hwndOwner = s_hwnd;
    prt_dlg.Flags = PD_NOPAGENUMS | PD_NOSELECTION | PD_RETURNDC;
    எனில் (!forceit)
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
    எனில் (forceit
# எனில் !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	    || (!gui.in_use && !term_console)
#  இல்லையெனில்
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
	எனில் (*p_pdev != NUL)
	    prt_dlg.hDC = CreateDC(வெற்று, (LPCSTR)p_pdev, வெற்று, வெற்று);
	இல்லையெனில்
	{
	    prt_dlg.Flags |= PD_RETURNDEFAULT;
	    எனில் (PrintDlgW(&prt_dlg) == 0)
		செல் init_fail_dlg;
	}
    }
    இல்லையெனில் எனில் (PrintDlgW(&prt_dlg) == 0)
	செல் init_fail_dlg;
    இல்லையெனில்
    {
	/*
	 * keep the previous driver context
	 */
	stored_dm = prt_dlg.hDevMode;
	stored_devn = prt_dlg.hDevNames;
	stored_nFlags = prt_dlg.Flags;
	stored_nCopies = prt_dlg.nCopies;
    }

    எனில் (prt_dlg.hDC == வெற்று)
    {
	emsg(_(e_printer_selection_failed));
	mch_print_cleanup();
	திரும்பு FALSE;
    }

    // Not all printer drivers report the support of color (or grey) in the
    // same way.  Let's set has_color if there appears to be some way to print
    // more than B&W.
    i = GetDeviceCaps(prt_dlg.hDC, NUMCOLORS);
    psettings->has_color = (GetDeviceCaps(prt_dlg.hDC, BITSPIXEL) > 1
				   || GetDeviceCaps(prt_dlg.hDC, PLANES) > 1
				   || i > 2 || i == -1);

    // Ensure all font styles are baseline aligned
    SetTextAlign(prt_dlg.hDC, TA_BASELINE|TA_இடது);

    /*
     * On some windows systems the nCopies parameter is not
     * passed back correctly. It must be retrieved from the
     * hDevMode struct.
     */
    mem = (DEVMODEW *)GlobalLock(prt_dlg.hDevMode);
    எனில் (mem != வெற்று)
    {
	எனில் (mem->dmCopies != 1)
	    stored_nCopies = mem->dmCopies;
	எனில் ((mem->dmFields & DM_DUPLEX) && (mem->dmDuplex & ~DMDUP_SIMPLEX))
	    psettings->duplex = TRUE;
	எனில் ((mem->dmFields & DM_COLOR) && (mem->dmColor & DMCOLOR_COLOR))
	    psettings->has_color = TRUE;
    }
    GlobalUnlock(prt_dlg.hDevMode);

    devname = (DEVNAMES *)GlobalLock(prt_dlg.hDevNames);
    எனில் (devname != 0)
    {
	WCHAR	*wprinter_name = (WCHAR *)devname + devname->wDeviceOffset;
	WCHAR	*wport_name = (WCHAR *)devname + devname->wOutputOffset;
	char_u	*text = (char_u *)_("to %s on %s");
	char_u  *printer_name = utf16_to_enc(wprinter_name, வெற்று);
	char_u	*port_name = utf16_to_enc(wport_name, வெற்று);

	எனில் (printer_name != வெற்று && port_name != வெற்று)
	    prt_name = alloc(STRLEN(printer_name)
					   + STRLEN(port_name) + STRLEN(text));
	எனில் (prt_name != வெற்று)
	    wsprintf((எழுத்து *)prt_name, (மாறிலி எழுத்து *)text,
		    printer_name, port_name);
	vim_free(printer_name);
	vim_free(port_name);
    }
    GlobalUnlock(prt_dlg.hDevNames);

    /*
     * Initialise the font according to 'printfont'
     */
    CLEAR_FIELD(fLogFont);
    எனில் (get_logfont(&fLogFont, p_pfn, prt_dlg.hDC, TRUE) == FAIL)
    {
	semsg(_(e_unknown_printer_font_str), p_pfn);
	mch_print_cleanup();
	திரும்பு FALSE;
    }

    க்கு (pifBold = 0; pifBold <= 1; pifBold++)
	க்கு (pifItalic = 0; pifItalic <= 1; pifItalic++)
	    க்கு (pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
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
    psettings->chars_per_வரி = prt_get_cpl();
    psettings->வரிகள்_per_page = prt_get_lpp();
    எனில் (prt_dlg.Flags & PD_USEDEVMODECOPIESANDCOLLATE)
    {
	psettings->n_collated_copies = (prt_dlg.Flags & PD_COLLATE)
						    ? prt_dlg.nCopies : 1;
	psettings->n_uncollated_copies = (prt_dlg.Flags & PD_COLLATE)
						    ? 1 : prt_dlg.nCopies;

	எனில் (psettings->n_collated_copies == 0)
	    psettings->n_collated_copies = 1;

	எனில் (psettings->n_uncollated_copies == 0)
	    psettings->n_uncollated_copies = 1;
    }
    இல்லையெனில்
    {
	psettings->n_collated_copies = 1;
	psettings->n_uncollated_copies = 1;
    }

    psettings->jobname = jobname;

    திரும்பு TRUE;

init_fail_dlg:
    err = CommDlgExtendedError();
    எனில் (err)
    {
	char_u *buf;

	// I suspect FormatMessage() doesn't work for values returned by
	// CommDlgExtendedError().  What does?
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		வெற்று, err, 0, (LPTSTR)(&buf), 0, வெற்று);
	semsg(_(e_print_error_str),
		buf == வெற்று ? (char_u *)_("Unknown") : buf);
	LocalFree((LPVOID)(buf));
    }
    இல்லையெனில்
	செய்தி_clr_eos(); // Maybe canceled

    mch_print_cleanup();
    திரும்பு FALSE;
}


    முழு
mch_print_begin(prt_settings_T *psettings)
{
    முழு			ret = 0;
    எழுத்து		szBuffer[300];
    WCHAR		*wp;

    hDlgPrint = CreateDialog(g_hinst, TEXT("PrintDlgBox"),
					     prt_dlg.hwndOwner, PrintDlgProc);
    SetAbortProc(prt_dlg.hDC, AbortProc);
    wsprintf(szBuffer, _("Printing '%s'"), gettail(psettings->jobname));
    vimSetDlgItemText(hDlgPrint, IDC_PRINTTEXT1, (char_u *)szBuffer);

    wp = enc_to_utf16(psettings->jobname, வெற்று);
    எனில் (wp != வெற்று)
    {
	DOCINFOW	di;

	CLEAR_FIELD(di);
	di.cbSize = அளவு(di);
	di.lpszDocName = wp;
	ret = StartDocW(prt_dlg.hDC, &di);
	vim_free(wp);
    }

# ifdef FEAT_GUI
    // Give focus back to main window (when using MDI).
#  ifdef VIMDLL
    எனில் (gui.in_use)
#  endif
	SetFocus(s_hwnd);
# endif

    திரும்பு (ret > 0);
}

    வெறுமை
mch_print_end(prt_settings_T *psettings UNUSED)
{
    EndDoc(prt_dlg.hDC);
    எனில் (!*bUserAbort && hDlgPrint != வெற்று)
	SendMessage(hDlgPrint, WM_COMMAND, 0, 0);
}

    முழு
mch_print_end_page(வெறுமை)
{
    திரும்பு (EndPage(prt_dlg.hDC) > 0);
}

    முழு
mch_print_begin_page(char_u *செய்தி)
{
    எனில் (செய்தி != வெற்று)
	vimSetDlgItemText(hDlgPrint, IDC_PROGRESS, செய்தி);
    திரும்பு (StartPage(prt_dlg.hDC) > 0);
}

    முழு
mch_print_blank_page(வெறுமை)
{
    திரும்பு (mch_print_begin_page(வெற்று) ? (mch_print_end_page()) : FALSE);
}

நிலையான முழு prt_pos_x = 0;
நிலையான முழு prt_pos_y = 0;

    வெறுமை
mch_print_start_வரி(முழு margin, முழு page_வரி)
{
    எனில் (margin)
	prt_pos_x = -prt_எண்_width;
    இல்லையெனில்
	prt_pos_x = 0;
    prt_pos_y = page_வரி * prt_வரி_height
				 + prt_tm.tmAscent + prt_tm.tmExternalLeading;
}

    முழு
mch_print_text_out(char_u *p, முழு len)
{
    SIZE	sz;
    WCHAR	*wp;
    முழு		wlen = len;
    முழு		ret = FALSE;

    wp = enc_to_utf16(p, &wlen);
    எனில் (wp == வெற்று)
	திரும்பு FALSE;

    TextOutW(prt_dlg.hDC, prt_pos_x + prt_இடது_margin,
	    prt_pos_y + prt_உச்சி_margin, wp, wlen);
    GetTextExtentPoint32W(prt_dlg.hDC, wp, wlen, &sz);
    vim_free(wp);
    prt_pos_x += (sz.cx - prt_tm.tmOverhang);
    // This is wrong when printing spaces for a TAB.
    எனில் (p[len] != NUL)
    {
	wlen = mb_ptr2len(p + len);
	wp = enc_to_utf16(p + len, &wlen);
	எனில் (wp != வெற்று)
	{
	    GetTextExtentPoint32W(prt_dlg.hDC, wp, 1, &sz);
	    ret = (prt_pos_x + prt_இடது_margin + sz.cx > prt_வலது_margin);
	    vim_free(wp);
	}
    }
    திரும்பு ret;
}

    வெறுமை
mch_print_set_font(முழு iBold, முழு iItalic, முழு iUnderline)
{
    SelectObject(prt_dlg.hDC, prt_font_handles[iBold][iItalic][iUnderline]);
}

    வெறுமை
mch_print_set_bg(long_u bgcol)
{
    SetBkColor(prt_dlg.hDC, GetNearestColor(prt_dlg.hDC,
						   swap_me((COLORREF)bgcol)));
    /*
     * With a white background we can draw characters transparent, which is
     * good for italic characters that overlap to the next char cell.
     */
    எனில் (bgcol == 0xffffffUL)
	SetBkMode(prt_dlg.hDC, TRANSPARENT);
    இல்லையெனில்
	SetBkMode(prt_dlg.hDC, OPAQUE);
}

    வெறுமை
mch_print_set_fg(long_u fgcol)
{
    SetTextColor(prt_dlg.hDC, GetNearestColor(prt_dlg.hDC,
						   swap_me((COLORREF)fgcol)));
}

#endif // FEAT_PRINTER && !FEAT_POSTSCRIPT



#எனில் defined(FEAT_SHORTCUT)
# include <shlobj.h>

# define is_path_sep(c)	    ((c) == L'\\' || (c) == L'/')

    நிலையான முழு
is_reparse_point_included(LPCWSTR fname)
{
    LPCWSTR	p = fname, q;
    WCHAR	buf[MAX_PATH];
    DWORD	attr;

    எனில் (SAFE_isalpha(p[0]) && p[1] == L':' && is_path_sep(p[2]))
	p += 3;
    இல்லையெனில் எனில் (is_path_sep(p[0]) && is_path_sep(p[1]))
	p += 2;

    போது (*p != L'\0')
    {
	q = wcspbrk(p, L"\\/");
	எனில் (q == வெற்று)
	    p = q = fname + wcslen(fname);
	இல்லையெனில்
	    p = q + 1;
	எனில் (q - fname >= MAX_PATH)
	    திரும்பு FALSE;
	wcsncpy(buf, fname, q - fname);
	buf[q - fname] = L'\0';
	attr = GetFileAttributesW(buf);
	எனில் (attr != INVALID_FILE_ATTRIBUTES
		&& (attr & FILE_ATTRIBUTE_REPARSE_POINT) != 0)
	    திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

/*
 * Return the resolved file path, NULL if "fname" is an AppExecLink reparse
 * point, already fully resolved, or it doesn't exists.
 */
    char_u *
resolve_reparse_point(char_u *fname)
{
    HANDLE	    h = INVALID_HANDLE_மதிப்பு;
    DWORD	    size;
    WCHAR	    *p, *wp;
    char_u	    *rfname = வெற்று;
    WCHAR	    *buff = வெற்று;

    p = enc_to_utf16(fname, வெற்று);
    எனில் (p == வெற்று)
	செல் fail;

    எனில் (!is_reparse_point_included(p))
    {
	vim_free(p);
	செல் fail;
    }

    h = CreateFileW(p, 0, 0, வெற்று, OPEN_EXISTING,
	    FILE_FLAG_BACKUP_SEMANTICS, வெற்று);
    vim_free(p);

    எனில் (h == INVALID_HANDLE_மதிப்பு)
	செல் fail;

    size = GetFinalPathNameByHandleW(h, வெற்று, 0, 0);
    எனில் (size == 0)
	செல் fail;
    buff = ALLOC_MULT(WCHAR, size);
    எனில் (buff == வெற்று)
	செல் fail;
    எனில் (GetFinalPathNameByHandleW(h, buff, size, 0) == 0)
	செல் fail;

    எனில் (wcsncmp(buff, L"\\\\?\\UNC\\", 8) == 0)
    {
	buff[6] = L'\\';
	wp = buff + 6;
    }
    இல்லையெனில் எனில் (wcsncmp(buff, L"\\\\?\\", 4) == 0)
	wp = buff + 4;
    இல்லையெனில்
	wp = buff;

    rfname = utf16_to_enc(wp, வெற்று);

fail:
    எனில் (h != INVALID_HANDLE_மதிப்பு)
	CloseHandle(h);
    எனில் (buff != வெற்று)
	vim_free(buff);

    திரும்பு rfname;
}

/*
 * When "fname" is the name of a shortcut (*.lnk) resolve the file it points
 * to and return that name in allocated memory.
 * Otherwise NULL is returned.
 */
    நிலையான char_u *
resolve_shortcut(char_u *fname)
{
    HRESULT		hr;
    IShellLink		*psl = வெற்று;
    IPersistFile	*ppf = வெற்று;
    OLECHAR		wsz[MAX_PATH];
    char_u		*rfname = வெற்று;
    முழு			len;
    IShellLinkW		*pslw = வெற்று;
    WIN32_FIND_DATAW	ffdw; // we get those free of charge

    // Check if the file name ends in ".lnk". Avoid calling
    // CoCreateInstance(), it's quite slow.
    எனில் (fname == வெற்று)
	திரும்பு rfname;
    len = (முழு)STRLEN(fname);
    எனில் (len <= 4 || STRNICMP(fname + len - 4, ".lnk", 4) != 0)
	திரும்பு rfname;

    CoInitialize(வெற்று);

    // create a link manager object and request its interface
    hr = CoCreateInstance(
	    &CLSID_ShellLink, வெற்று, CLSCTX_INPROC_SERVER,
	    &IID_IShellLinkW, (வெறுமை**)&pslw);
    எனில் (hr == S_OK)
    {
	WCHAR	*p = enc_to_utf16(fname, வெற்று);

	எனில் (p != வெற்று)
	{
	    // Get a pointer to the IPersistFile interface.
	    hr = pslw->lpVtbl->QueryInterface(
		    pslw, &IID_IPersistFile, (வெறுமை**)&ppf);
	    எனில் (hr != S_OK)
		செல் shortcut_errorw;

	    // "load" the name and resolve the link
	    hr = ppf->lpVtbl->Load(ppf, p, STGM_READ);
	    எனில் (hr != S_OK)
		செல் shortcut_errorw;
# எனில் 0  // This makes Vim wait a long time if the target does not exist.
	    hr = pslw->lpVtbl->Resolve(pslw, வெற்று, SLR_NO_UI);
	    எனில் (hr != S_OK)
		செல் shortcut_errorw;
# endif

	    // Get the path to the link target.
	    ZeroMemory(wsz, MAX_PATH * அளவு(WCHAR));
	    hr = pslw->lpVtbl->GetPath(pslw, wsz, MAX_PATH, &ffdw, 0);
	    எனில் (hr == S_OK && wsz[0] != NUL)
		rfname = utf16_to_enc(wsz, வெற்று);

shortcut_errorw:
	    vim_free(p);
	}
    }

    // Release all interface pointers (both belong to the same object)
    எனில் (ppf != வெற்று)
	ppf->lpVtbl->Release(ppf);
    எனில் (psl != வெற்று)
	psl->lpVtbl->Release(psl);
    எனில் (pslw != வெற்று)
	pslw->lpVtbl->Release(pslw);

    CoUninitialize();
    திரும்பு rfname;
}

    char_u *
mch_resolve_path(char_u *fname, முழு reparse_point)
{
    char_u  *path = resolve_shortcut(fname);

    எனில் (path == வெற்று && reparse_point)
	path = resolve_reparse_point(fname);
    திரும்பு path;
}
#endif

#எனில் defined(FEAT_EVAL) && (!defined(FEAT_GUI) || defined(VIMDLL))
/*
 * Bring ourselves to the foreground.  Does work if the OS doesn't allow it.
 */
    வெறுமை
win32_set_foreground(வெறுமை)
{
    GetConsoleHwnd();	    // get value of s_hwnd
    எனில் (s_hwnd != 0)
	SetForegroundWindow(s_hwnd);
}
#endif

#எனில் defined(FEAT_CLIENTSERVER)
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
# define VIM_CLASSNAME_LEN  (அளவு(VIM_CLASSNAME) - 1)

// Timeout for sending a message to another Vim instance.  Normally this works
// instantly, but it may hang when the other Vim instance is halted.
# define SENDMESSAGE_TIMEOUT	(5 * 1000)

// Communication is via WM_COPYDATA messages. The message type is sent in
// the dwData parameter. Types are defined here.
# define COPYDATA_சாவிகள்		0
# define COPYDATA_REPLY		1
# define COPYDATA_கோவை		10
# define COPYDATA_RESULT	11
# define COPYDATA_ERROR_RESULT	12
# define COPYDATA_ENCODING	20

// This is a structure containing a server HWND and its name.
கட்டமைப்பு server_id
{
    HWND hwnd;
    char_u *name;
};

// Last received 'encoding' that the client uses.
நிலையான char_u	*client_enc = வெற்று;

/*
 * Tell the other side what encoding we are using.
 * Return -1 if timeout happens.  Other errors are ignored.
 */
    நிலையான முழு
serverSendEnc(HWND target)
{
    COPYDATASTRUCT data;

    data.dwData = COPYDATA_ENCODING;
    data.cbData = (DWORD)STRLEN(p_enc) + 1;
    data.lpData = p_enc;
    எனில் (SendMessageTimeout(target, WM_COPYDATA,
	    (WPARAM)message_window, (LPARAM)&data,
	    SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, வெற்று) == 0)
	திரும்பு -1;
    திரும்பு 0;
}

/*
 * Clean up on exit. This destroys the hidden message window.
 */
    நிலையான வெறுமை
CleanUpMessaging(வெறுமை)
{
    எனில் (message_window == 0)
	திரும்பு;

    DestroyWindow(message_window);
    message_window = 0;
}

நிலையான முழு save_reply(HWND server, char_u *reply, முழு கோவை);

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
    நிலையான LRESULT CALLBACK
Messaging_WndProc(HWND hwnd, UINT செய்தி, WPARAM wParam, LPARAM lParam)
{
    எனில் (செய்தி == WM_COPYDATA)
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
	முழு		retval;
	DWORD_PTR	dwret = 0;
	char_u		*str;
	char_u		*tofree;

	தேர்வு (data->dwData)
	{
	நிலை COPYDATA_ENCODING:
	    // Remember the encoding that the client uses.
	    vim_free(client_enc);
	    client_enc = enc_canonize((char_u *)data->lpData);
	    திரும்பு 1;

	நிலை COPYDATA_சாவிகள்:
	    // Remember who sent this, for <client>
	    clientWindow = sender;

	    // Add the received keys to the input buffer.
	    str = serverConvert(client_enc, (char_u *)data->lpData, &tofree);
# ifdef FEAT_GUI
	    எனில் (gui.in_use)
		// The GUI may get here re-entrantly, so insert the keys later.
		server_add_உள்ளீடு(str);
	    இல்லையெனில்
# endif
		server_to_உள்ளீடு_buf(str);
	    vim_free(tofree);

# ifdef FEAT_GUI
	    // Wake up the main GUI loop.
#  ifdef VIMDLL
	    எனில் (gui.in_use)
#  endif
		எனில் (s_hwnd != 0)
		    PostMessage(s_hwnd, WM_NULL, 0, 0);
# endif
	    திரும்பு 1;

	நிலை COPYDATA_கோவை:
	    // Remember who sent this, for <client>
	    clientWindow = sender;

	    str = serverConvert(client_enc, (char_u *)data->lpData, &tofree);
	    res = eval_client_கோவை_to_string(str);

	    எனில் (res == வெற்று)
	    {
		எழுத்து	*err = _(e_invalid_expression_received);
		size_t	len = STRLEN(str) + STRLEN(err) + 5;

		res = alloc(len);
		எனில் (res != வெற்று)
		    reply.cbData = (DWORD)vim_snprintf_safelen(
			(எழுத்து *)res, len, "%s: \"%s\"", err, str);
		இல்லையெனில்
		    reply.cbData = 0;
		reply.dwData = COPYDATA_ERROR_RESULT;
	    }
	    இல்லையெனில்
	    {
		reply.cbData = (DWORD)STRLEN(res) + 1;
		reply.dwData = COPYDATA_RESULT;
	    }
	    reply.lpData = res;

	    எனில் (serverSendEnc(sender) < 0)
		retval = -1;
	    இல்லையெனில்
	    {
		எனில் (SendMessageTimeout(sender, WM_COPYDATA,
			(WPARAM)message_window, (LPARAM)&reply,
			SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
		    retval = -1;
		இல்லையெனில்
		    retval = (முழு)dwret;
	    }
	    vim_free(tofree);
	    vim_free(res);
	    திரும்பு retval;

	நிலை COPYDATA_REPLY:
	நிலை COPYDATA_RESULT:
	நிலை COPYDATA_ERROR_RESULT:
	    எனில் (data->lpData != வெற்று)
	    {
		str = serverConvert(client_enc, (char_u *)data->lpData,
								     &tofree);
		எனில் (tofree == வெற்று)
		    str = vim_strsave(str);
		எனில் (save_reply(sender, str,
			   (data->dwData == COPYDATA_REPLY ?  0 :
			   (data->dwData == COPYDATA_RESULT ? 1 :
							      2))) == FAIL)
		    vim_free(str);
		இல்லையெனில் எனில் (data->dwData == COPYDATA_REPLY)
		{
		    char_u	winstr[30];

		    sprintf((எழுத்து *)winstr, PRINTF_HEX_LONG_U, (long_u)sender);
		    apply_autocmds(EVENT_REMOTEREPLY, winstr, str,
								TRUE, curbuf);
		}
	    }
	    திரும்பு 1;
	}

	திரும்பு 0;
    }

    இல்லையெனில் எனில் (செய்தி == WM_ACTIVATE && wParam == WA_ACTIVE)
    {
	// When the message window is activated (brought to the foreground),
	// this actually applies to the text window.
# எனில் !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	எனில் (!gui.in_use)
#  endif
	    GetConsoleHwnd();	    // get value of s_hwnd
# endif
	எனில் (s_hwnd != 0)
	{
	    SetForegroundWindow(s_hwnd);
	    திரும்பு 0;
	}
    }

    திரும்பு DefWindowProc(hwnd, செய்தி, wParam, lParam);
}

/*
 * Initialise the message handling process.  This involves creating a window
 * to handle messages - the window will not be visible.
 */
    வெறுமை
serverInitMessaging(வெறுமை)
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
    wndclass.hIcon = வெற்று;
    wndclass.hCursor = வெற்று;
    wndclass.hbrBackground = வெற்று;
    wndclass.lpszMenuName = வெற்று;
    wndclass.lpszClassName = VIM_CLASSNAME;
    RegisterClass(&wndclass);

    // Create the message window. It will be hidden, so the details don't
    // matter.  Don't use WS_OVERLAPPEDWINDOW, it will make a shortcut remove
    // focus from gvim.
    message_window = CreateWindow(VIM_CLASSNAME, "",
			 WS_POPUPWINDOW | WS_CAPTION,
			 CW_USEDEFAULT, CW_USEDEFAULT,
			 100, 100, வெற்று, வெற்று,
			 g_hinst, வெற்று);
}

// Used by serverSendToVim() to find an alternate server name.
நிலையான char_u *altname_buf_ptr = வெற்று;

/*
 * Get the title of the window "hwnd", which is the Vim server name, in
 * "name[namelen]" and return the length.
 * Returns zero if window "hwnd" is not a Vim server.
 */
    நிலையான முழு
getVimServerName(HWND hwnd, எழுத்து *name, முழு namelen)
{
    முழு		len;
    எழுத்து	buffer[VIM_CLASSNAME_LEN + 1];

    // Ignore windows which aren't Vim message windows
    len = GetClassName(hwnd, buffer, அளவு(buffer));
    எனில் (len != VIM_CLASSNAME_LEN || STRCMP(buffer, VIM_CLASSNAME) != 0)
	திரும்பு 0;

    // Get the title of the window
    திரும்பு GetWindowText(hwnd, name, namelen);
}

    நிலையான BOOL CALLBACK
enumWindowsGetServer(HWND hwnd, LPARAM lparam)
{
    கட்டமைப்பு	server_id *id = (கட்டமைப்பு server_id *)lparam;
    எழுத்து	server[MAX_PATH];

    // Get the title of the window
    எனில் (getVimServerName(hwnd, server, அளவு(server)) == 0)
	திரும்பு TRUE;

    // If this is the server we're looking for, return its HWND
    எனில் (STRICMP(server, id->name) == 0)
    {
	id->hwnd = hwnd;
	திரும்பு FALSE;
    }

    // If we are looking for an alternate server, remember this name.
    எனில் (altname_buf_ptr != வெற்று)
    {
	size_t	namelen = STRLEN(id->name);

	எனில் (STRNICMP(server, id->name, namelen) == 0
	    && vim_isdigit(server[namelen]))
	{
	    STRCPY(altname_buf_ptr, server);
	    altname_buf_ptr = வெற்று;	    // don't use another name
	}
    }

    // Otherwise, keep looking
    திரும்பு TRUE;
}

    நிலையான BOOL CALLBACK
enumWindowsGetNames(HWND hwnd, LPARAM lparam)
{
    list_T	*list = (list_T *)lparam;
    எழுத்து	server[MAX_PATH];

    // Get the title of the window
    எனில் (getVimServerName(hwnd, server, அளவு(server)) == 0)
	திரும்பு TRUE;

    list_append_string(list, (char_u *)server, -1);
    திரும்பு TRUE;
}

கட்டமைப்பு enum_windows_s
{
    WNDENUMPROC lpEnumFunc;
    LPARAM      lParam;
};

    நிலையான BOOL CALLBACK
enum_windows_child(HWND hwnd, LPARAM lParam)
{
    கட்டமைப்பு enum_windows_s *ew = (கட்டமைப்பு enum_windows_s *)lParam;

    திரும்பு (ew->lpEnumFunc)(hwnd, ew->lParam);
}

    நிலையான BOOL CALLBACK
enum_windows_toplevel(HWND hwnd, LPARAM lParam)
{
    கட்டமைப்பு enum_windows_s *ew = (கட்டமைப்பு enum_windows_s *)lParam;

    எனில் ((ew->lpEnumFunc)(hwnd, ew->lParam))
	திரும்பு TRUE;
    திரும்பு EnumChildWindows(hwnd, enum_windows_child, lParam);
}

/*
 * Enumerate all windows including children.
 */
    நிலையான BOOL
enum_windows(WNDENUMPROC lpEnumFunc, LPARAM lParam)
{
    கட்டமைப்பு enum_windows_s ew;

    ew.lpEnumFunc = lpEnumFunc;
    ew.lParam = lParam;
    திரும்பு EnumWindows(enum_windows_toplevel, (LPARAM)&ew);
}

    நிலையான HWND
findServer(char_u *name)
{
    கட்டமைப்பு server_id id;

    id.name = name;
    id.hwnd = 0;

    enum_windows(enumWindowsGetServer, (LPARAM)(&id));

    திரும்பு id.hwnd;
}

    வெறுமை
serverSetName(char_u *name)
{
    size_t	namelen;
    char_u	*ok_name;
    HWND	hwnd = 0;
    முழு		i = 0;
    char_u	*p;

    // Leave enough space for a 9-digit suffix to ensure uniqueness!
    namelen = STRLEN(name);
    ok_name = alloc(namelen + 10);
    எனில் (ok_name == வெற்று)
	திரும்பு;

    STRCPY(ok_name, name);
    p = ok_name + namelen;

    க்கு (;;)
    {
	// This is inefficient - we're doing an EnumWindows loop for each
	// possible name. It would be better to grab all names in one go,
	// and scan the list each time...
	hwnd = findServer(ok_name);
	எனில் (hwnd == 0)
	    நிறுத்து;

	++i;
	எனில் (i >= 1000)
	    நிறுத்து;

	sprintf((எழுத்து *)p, "%d", i);
    }

    எனில் (hwnd != 0)
	vim_free(ok_name);
    இல்லையெனில்
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
serverGetVimNames(வெறுமை)
{
    list_T *list = list_alloc();

    எனில் (list == வெற்று)
	திரும்பு வெற்று;

    enum_windows(enumWindowsGetNames, (LPARAM)list);

    திரும்பு list;
}

    முழு
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
    sscanf((எழுத்து *)name, SCANF_HEX_LONG_U, &n);
    எனில் (n == 0)
	திரும்பு -1;

    target = (HWND)n;
    எனில் (!IsWindow(target))
	திரும்பு -1;

    data.dwData = COPYDATA_REPLY;
    data.cbData = (DWORD)STRLEN(reply) + 1;
    data.lpData = reply;

    எனில் (serverSendEnc(target) < 0)
	திரும்பு -1;
    எனில் (SendMessageTimeout(target, WM_COPYDATA,
		(WPARAM)message_window, (LPARAM)&data,
		SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
	திரும்பு -1;
    திரும்பு dwret ? 0 : -1;
}

    முழு
serverSendToVim(
    char_u	 *name,			// Where to send.
    char_u	 *cmd,			// What to send.
    char_u	 **result,		// Result of eval'ed expression
    வெறுமை	 *ptarget,		// HWND of server
    முழு		 as_கோவை,		// Expression or keys?
    முழு		 timeout,		// timeout in seconds or zero
    முழு		 silent)		// don't complain about no server
{
    size_t	namelen;
    HWND	target;
    COPYDATASTRUCT data;
    char_u	*retval = வெற்று;
    முழு		retcode = 0;
    DWORD_PTR	dwret = 0;
    char_u	altname_buf[MAX_PATH];

    // Execute locally if no display or target is ourselves
    எனில் (serverName != வெற்று && STRICMP(name, serverName) == 0)
	திரும்பு sendToLocalVim(cmd, as_கோவை, result);

    // If the server name does not end in a digit then we look for an
    // alternate name.  e.g. when "name" is GVIM then we may find GVIM2.
    namelen = STRLEN(name);
    எனில் (namelen > 1 && !vim_isdigit(name[namelen - 1]))
	altname_buf_ptr = altname_buf;
    altname_buf[0] = NUL;
    target = findServer(name);
    altname_buf_ptr = வெற்று;
    எனில் (target == 0 && altname_buf[0] != NUL)
	// Use another server name we found.
	target = findServer(altname_buf);

    எனில் (target == 0)
    {
	எனில் (!silent)
	    semsg(_(e_no_registered_server_named_str), name);
	திரும்பு -1;
    }

    எனில் (ptarget)
	*(HWND *)ptarget = target;

    data.dwData = as_கோவை ? COPYDATA_கோவை : COPYDATA_சாவிகள்;
    data.cbData = (DWORD)STRLEN(cmd) + 1;
    data.lpData = cmd;

    எனில் (serverSendEnc(target) < 0)
	திரும்பு -1;
    எனில் (SendMessageTimeout(target, WM_COPYDATA,
		(WPARAM)message_window, (LPARAM)&data,
		SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
	திரும்பு -1;
    எனில் (dwret == 0)
	திரும்பு -1;

    எனில் (as_கோவை)
	retval = serverGetReply(target, &retcode, TRUE, TRUE, timeout);

    எனில் (result == வெற்று)
	vim_free(retval);
    இல்லையெனில்
	*result = retval; // Caller assumes responsibility for freeing

    திரும்பு retcode;
}

/*
 * Bring the server to the foreground.
 */
    வெறுமை
serverForeground(char_u *name)
{
    HWND	target = findServer(name);

    எனில் (target != 0)
	SetForegroundWindow(target);
}

// Replies from server need to be stored until the client picks them up via
// remote_read(). So we maintain a list of server-id/reply pairs.
// Note that there could be multiple replies from one server pending if the
// client is slow picking them up.
// We just store the replies in a simple list. When we remove an entry, we
// move list entries down to fill the gap.
// The server ID is simply the HWND.
வகைவரை கட்டமைப்பு
{
    HWND	server;		// server window
    char_u	*reply;		// reply string
    முழு		கோவை_result;	// 0 for REPLY, 1 for RESULT 2 for error
} reply_T;

நிலையான garray_T reply_list = {0, 0, அளவு(reply_T), 5, 0};

# define REPLY_ITEM(i) ((reply_T *)(reply_list.ga_data) + (i))
# define REPLY_மொத்தம் (reply_list.ga_len)

// Flag which is used to wait for a reply
நிலையான முழு reply_received = 0;

/*
 * Store a reply.  "reply" must be allocated memory (or NULL).
 */
    நிலையான முழு
save_reply(HWND server, char_u *reply, முழு கோவை)
{
    reply_T *rep;

    எனில் (ga_grow(&reply_list, 1) == FAIL)
	திரும்பு FAIL;

    rep = REPLY_ITEM(REPLY_மொத்தம்);
    rep->server = server;
    rep->reply = reply;
    rep->கோவை_result = கோவை;
    எனில் (rep->reply == வெற்று)
	திரும்பு FAIL;

    ++REPLY_மொத்தம்;
    reply_received = 1;
    திரும்பு OK;
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
serverGetReply(HWND server, முழு *கோவை_res, முழு remove, முழு wait, முழு timeout)
{
    முழு		i;
    char_u	*reply;
    reply_T	*rep;
    முழு		did_process = FALSE;
    time_t	start;
    time_t	now;

    // When waiting, loop until the message waiting for is received.
    time(&start);
    க்கு (;;)
    {
	// Reset this here, in case a message arrives while we are going
	// through the already received messages.
	reply_received = 0;

	க்கு (i = 0; i < REPLY_மொத்தம்; ++i)
	{
	    rep = REPLY_ITEM(i);
	    எனில் (rep->server == server
		    && ((rep->கோவை_result != 0) == (கோவை_res != வெற்று)))
	    {
		// Save the values we've found for later
		reply = rep->reply;
		எனில் (கோவை_res != வெற்று)
		    *கோவை_res = rep->கோவை_result == 1 ? 0 : -1;

		எனில் (remove)
		{
		    // Move the rest of the list down to fill the gap
		    mch_memmove(rep, rep + 1,
				     (REPLY_மொத்தம் - i - 1) * அளவு(reply_T));
		    --REPLY_மொத்தம்;
		}

		// Return the reply to the caller, who takes on responsibility
		// for freeing it if "remove" is TRUE.
		திரும்பு reply;
	    }
	}

	// If we got here, we didn't find a reply. Return immediately if the
	// "wait" parameter isn't set.
	எனில் (!wait)
	{
	    // Process pending messages once. Without this, looping on
	    // remote_peek() would never get the reply.
	    எனில் (!did_process)
	    {
		did_process = TRUE;
		serverProcessPendingMessages();
		தொடர்;
	    }
	    நிறுத்து;
	}

	// We need to wait for a reply. Enter a message loop until the
	// "reply_received" flag gets set.

	// Loop until we receive a reply
	போது (reply_received == 0)
	{
# ifdef FEAT_TIMERS
	    // TODO: use the return value to decide how long to wait.
	    check_due_timer();
# endif
	    time(&now);
	    எனில் (timeout > 0 && (now - start) >= timeout)
		நிறுத்து;

	    // Wait for a SendMessage() call to us.  This could be the reply
	    // we are waiting for.  Use a timeout of a second, to catch the
	    // situation that the server died unexpectedly.
	    செய்தி_Wait_For_Multiple_Objects(0, வெற்று, TRUE, 1000, QS_ALLINPUT);

	    // If the server has died, give up
	    எனில் (!IsWindow(server))
		திரும்பு வெற்று;

	    serverProcessPendingMessages();
	}
    }

    திரும்பு வெற்று;
}

/*
 * Process any messages in the Windows message queue.
 */
    வெறுமை
serverProcessPendingMessages(வெறுமை)
{
    செய்தி_2 செய்தி;

    போது (PeekMessageW(&செய்தி, வெற்று, 0, 0, PM_REMOVE))
    {
	TranslateMessage(&செய்தி);
	DispatchMessageW(&செய்தி);
    }
}

#endif // FEAT_CLIENTSERVER

#எனில் defined(FEAT_GUI) || (defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT))

கட்டமைப்பு charset_pair
{
    string_T	name;
    BYTE	charset;
};

நிலையான கட்டமைப்பு charset_pair
charset_pairs[] =
{
    {STR_LITERAL_INIT("ANSI"),		ANSI_CHARSET},
    {STR_LITERAL_INIT("CHINESEBIG5"),	CHINESEBIG5_CHARSET},
    {STR_LITERAL_INIT("DEFAULT"),	DEFAULT_CHARSET},
    {STR_LITERAL_INIT("HANGEUL"),	HANGEUL_CHARSET},
    {STR_LITERAL_INIT("OEM"),		OEM_CHARSET},
    {STR_LITERAL_INIT("SHIFTJIS"),	SHIFTJIS_CHARSET},
    {STR_LITERAL_INIT("SYMBOL"),	குறியீடு_CHARSET},
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

கட்டமைப்பு quality_pair
{
    string_T	name;
    DWORD	quality;
};

நிலையான கட்டமைப்பு quality_pair
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
    எழுத்து *
charset_id2name(முழு id)
{
    முழு	i;

    க்கு (i = 0; i < (முழு)ARRAY_LENGTH(charset_pairs); ++i)
    {
	எனில் ((BYTE)id == charset_pairs[i].charset)
	    திரும்பு (எழுத்து *)charset_pairs[i].name.string;
    }

    திரும்பு வெற்று;
}

/*
 * Convert a quality ID to a name.
 * Return NULL when not recognized.
 */
    எழுத்து *
quality_id2name(DWORD id)
{
    முழு	i;

    க்கு (i = 0; i < (முழு)ARRAY_LENGTH(quality_pairs); ++i)
    {
	எனில் (id == quality_pairs[i].quality)
	    திரும்பு (எழுத்து *)quality_pairs[i].name.string;
    }

    திரும்பு வெற்று;
}

// The default font height in 100% scaling (96dpi).
// (-16 in 96dpi equates to roughly 12pt)
# define DEFAULT_FONT_HEIGHT	(-16)

நிலையான மாறிலி LOGFONTW s_lfDefault =
{
    DEFAULT_FONT_HEIGHT,
    0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
    PROOF_QUALITY, FIXED_PITCH | FF_DONTCARE,
    L""	// Default font name will be set later based on current language.
};

// This will be initialized when set_default_logfont() is called first time.
// The value will be based on the system DPI.
முழு current_font_height = 0;		// also used in gui_w32.c

/*
 * Convert a string representing a point size into pixels. The string should
 * be a positive decimal number, with an optional decimal point (eg, "12", or
 * "10.5"). The pixel value is returned, and a pointer to the next unconverted
 * character is stored in *end. The flag "vertical" says whether this
 * calculation is for a vertical (height) size or a horizontal (width) one.
 */
    நிலையான முழு
points_to_pixels(WCHAR *str, WCHAR **end, முழு vertical, long_i pprinter_dc)
{
    முழு		pixels;
    முழு		points = 0;
    முழு		வகுஎண் = 0;
    HWND	hwnd = (HWND)0;
    HDC		hdc;
    HDC		printer_dc = (HDC)pprinter_dc;

    போது (*str != NUL)
    {
	எனில் (*str == L'.' && வகுஎண் == 0)
	{
	    // Start keeping a divisor, for later
	    வகுஎண் = 1;
	}
	இல்லையெனில்
	{
	    எனில் (!VIM_ISDIGIT(*str))
		நிறுத்து;

	    points *= 10;
	    points += *str - L'0';
	    வகுஎண் *= 10;
	}
	++str;
    }

    எனில் (வகுஎண் == 0)
	வகுஎண் = 1;

    எனில் (printer_dc == வெற்று)
    {
	hwnd = GetDesktopWindow();
	hdc = GetWindowDC(hwnd);
    }
    இல்லையெனில்
	hdc = printer_dc;

    pixels = MulDiv(points,
		    GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX),
		    72 * வகுஎண்);

    எனில் (printer_dc == வெற்று)
	ReleaseDC(hwnd, hdc);

    *end = str;
    திரும்பு pixels;
}

/*
 * Convert pixel into point size. This is a reverse of points_to_pixels.
 */
    நிலையான இரட்டை
pixels_to_points(முழு pixels, முழு vertical, long_i pprinter_dc)
{
    இரட்டை	points = 0;
    HWND	hwnd = (HWND)0;
    HDC		hdc;
    HDC		printer_dc = (HDC)pprinter_dc;

    எனில் (printer_dc == வெற்று)
    {
	hwnd = GetDesktopWindow();
	hdc = GetWindowDC(hwnd);
    }
    இல்லையெனில்
	hdc = printer_dc;

    points = pixels * 72.0 / GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX);
    எனில் (printer_dc == வெற்று)
	ReleaseDC(hwnd, hdc);

    திரும்பு points;
}

    நிலையான முழு CALLBACK
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
    எனில் ((ntm->tmPitchAndFamily & 1) != 0)
	திரும்பு 1;
# endif

    // Remember this LOGFONTW as a "possible"
    *lf = elf->elfLogFont;

    // Terminate the scan as soon as we find an ANSI font
    எனில் (lf->lfCharSet == ANSI_CHARSET
	    || lf->lfCharSet == OEM_CHARSET
	    || lf->lfCharSet == DEFAULT_CHARSET)
	திரும்பு 0;

    // Continue the scan - we have a non-ANSI font
    திரும்பு 2;
}

    நிலையான முழு
init_logfont(LOGFONTW *lf)
{
    முழு		n;
    HWND	hwnd = GetDesktopWindow();
    HDC		hdc = GetWindowDC(hwnd);

    n = EnumFontFamiliesW(hdc,
			 lf->lfFaceName,
			 (FONTENUMPROCW)font_enumproc,
			 (LPARAM)lf);

    ReleaseDC(hwnd, hdc);

    // If we couldn't find a usable font, return failure
    எனில் (n == 1)
	திரும்பு FAIL;

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
    திரும்பு OK;
}

/*
 * Call back for EnumFontFamiliesW in expand_font_enumproc.
 *
 */
    நிலையான முழு CALLBACK
expand_font_enumproc(
    ENUMLOGFONTW    *elf,
    NEWTEXTMETRICW  *ntm UNUSED,
    DWORD	    type UNUSED,
    LPARAM	    lparam)
{
    LOGFONTW *lf = (LOGFONTW*)elf;

# ifndef FEAT_PROPORTIONAL_FONTS
    // Ignore non-monospace fonts without further ado
    எனில் ((ntm->tmPitchAndFamily & 1) != 0)
	திரும்பு 1;
# endif

    // Filter only on ANSI. Otherwise will see a lot of random fonts that we
    // usually don't want.
    எனில் (lf->lfCharSet != ANSI_CHARSET)
	திரும்பு 1;

    முழு (*add_match)(char_u *) = (முழு (*)(char_u *))lparam;

    WCHAR *faceNameW = lf->lfFaceName;
    char_u *faceName = utf16_to_enc(faceNameW, வெற்று);
    எனில் (!faceName)
	திரும்பு 0;

    add_match(faceName);
    vim_free(faceName);

    திரும்பு 1;
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
    வெறுமை
gui_mch_expand_font(
    optexpand_T	*args,
    வெறுமை	*param UNUSED,
    முழு		(*add_match)(char_u *val))
{
    expand_T	    *xp = args->oe_xp;
    எனில் (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	எழுத்து buf[30];
	முழு	i;

	// Always fill in with the current font size as first option for
	// convenience. We simply round to the closest integer for simplicity.
	முழு font_height = (முழு)round(
		pixels_to_points(-current_font_height, TRUE, (long_i)வெற்று));
	vim_snprintf(buf, ARRAY_LENGTH(buf), "h%d", font_height);
	add_match((char_u *)buf);

	// Note: Keep this in sync with get_logfont(). Don't include 'c' and
	// 'q' as we fill in all the values below.
	நிலையான எழுத்து *(p_gfn_win_opt_மதிப்புகள்[]) = {
	    "h" , "w" , "W" , "b" , "i" , "u" , "s"};
	க்கு (i = 0; i < (முழு)ARRAY_LENGTH(p_gfn_win_opt_மதிப்புகள்); i++)
	    add_match((char_u *)p_gfn_win_opt_மதிப்புகள்[i]);

	க்கு (i = 0; i < (முழு)ARRAY_LENGTH(charset_pairs); ++i)
	{
	    vim_snprintf(buf, அளவு(buf), "c%s", charset_pairs[i].name.string);
	    add_match((char_u *)buf);
	}

	க்கு (i = 0; i < (முழு)ARRAY_LENGTH(quality_pairs); ++i)
	{
	    vim_snprintf(buf, அளவு(buf), "q%s", quality_pairs[i].name.string);
	    add_match((char_u *)buf);
	}
	திரும்பு;
    }

    HWND	hwnd = GetDesktopWindow();
    HDC		hdc = GetWindowDC(hwnd);

    EnumFontFamiliesW(hdc,
	    வெற்று,
	    (FONTENUMPROCW)expand_font_enumproc,
	    (LPARAM)add_match);

    ReleaseDC(hwnd, hdc);
}

/*
 * Compare a UTF-16 string and an ASCII string literally.
 * Only works all the code points are inside ASCII range.
 */
    நிலையான முழு
utf16ascncmp(மாறிலி WCHAR *w, மாறிலி எழுத்து *p, size_t n)
{
    size_t i;

    க்கு (i = 0; i < n; i++)
    {
	எனில் (w[i] == 0 || w[i] != p[i])
	    திரும்பு w[i] - p[i];
    }
    திரும்பு 0;
}

/*
 * Equivalent of GetDpiForSystem().
 */
    UINT WINAPI
vimGetDpiForSystem(வெறுமை)
{
    HWND hwnd = GetDesktopWindow();
    HDC hdc = GetWindowDC(hwnd);
    UINT dpi = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(hwnd, hdc);
    திரும்பு dpi;
}

/*
 * Set default logfont based on current language.
 */
    நிலையான வெறுமை
set_default_logfont(LOGFONTW *lf)
{
    // Default font name for current language on MS-Windows.
    // If not translated, falls back to "Consolas".
    // This must be a fixed-pitch font.
    மாறிலி எழுத்து *defaultfontname = N_("DefaultFontNameForWindows");
    எழுத்து *fontname = _(defaultfontname);

    எனில் (STRCMP(fontname, defaultfontname) == 0)
	fontname = "Consolas";

    *lf = s_lfDefault;
    lf->lfHeight = DEFAULT_FONT_HEIGHT * (முழு)vimGetDpiForSystem() / 96;
    எனில் (current_font_height == 0)
	current_font_height = lf->lfHeight;

    WCHAR *wfontname = enc_to_utf16((char_u*)fontname, வெற்று);
    எனில் (wfontname != வெற்று)
    {
	wcscpy_s(lf->lfFaceName, LF_FACESIZE, wfontname);
	vim_free(wfontname);
    }
}

/*
 * Get font info from "name" into logfont "lf".
 * Return OK for a valid name, FAIL otherwise.
 */
    முழு
get_logfont(
    LOGFONTW	*lf,
    char_u	*name,
    HDC		printer_dc,
    முழு		verbose)
{
    WCHAR	*p;
    முழு		i;
    முழு		ret = FAIL;
    நிலையான LOGFONTW *lastlf = வெற்று;
    WCHAR	*wname;

    set_default_logfont(lf);
    எனில் (name == வெற்று)
	திரும்பு OK;

    wname = enc_to_utf16(name, வெற்று);
    எனில் (wname == வெற்று)
	திரும்பு FAIL;

    எனில் (wcscmp(wname, L"*") == 0)
    {
# எனில் defined(FEAT_GUI_MSWIN)
	CHOOSEFONTW	cf;
	// if name is "*", bring up std font dialog:
	CLEAR_FIELD(cf);
	cf.lStructSize = அளவு(cf);
	cf.hwndOwner = s_hwnd;
	cf.Flags = CF_SCREENFONTS | CF_FIXEDPITCHONLY | CF_INITTOLOGFONTSTRUCT;
	எனில் (lastlf != வெற்று)
	    *lf = *lastlf;
	cf.lpLogFont = lf;
	cf.nFontType = 0 ; //REGULAR_FONTTYPE;
	எனில் (ChooseFontW(&cf))
	    ret = OK;
# endif
	செல் theend;
    }

    /*
     * Split name up, it could be <name>:h<height>:w<width> etc.
     */
    க்கு (p = wname; *p && *p != L':'; p++)
    {
	எனில் (p - wname + 1 >= LF_FACESIZE)
	    செல் theend;			// Name too long
	lf->lfFaceName[p - wname] = *p;
    }
    எனில் (p != wname)
	lf->lfFaceName[p - wname] = NUL;

    // First set defaults
    lf->lfHeight = DEFAULT_FONT_HEIGHT * (முழு)vimGetDpiForSystem() / 96;
    lf->lfWidth = 0;
    lf->lfWeight = FW_NORMAL;
    lf->lfItalic = FALSE;
    lf->lfUnderline = FALSE;
    lf->lfStrikeOut = FALSE;

    /*
     * If the font can't be found, try replacing '_' by ' '.
     */
    எனில் (init_logfont(lf) == FAIL)
    {
	முழு	did_replace = FALSE;

	க்கு (i = 0; lf->lfFaceName[i]; ++i)
	    எனில் (lf->lfFaceName[i] == L'_')
	    {
		lf->lfFaceName[i] = L' ';
		did_replace = TRUE;
	    }
	எனில் (!did_replace || init_logfont(lf) == FAIL)
	    செல் theend;
    }

    போது (*p == L':')
	p++;

    // Set the values found after ':'
    போது (*p)
    {
	தேர்வு (*p++)
	{
	    // Note: Keep this in sync with gui_mch_expand_font().
	    நிலை L'h':
		lf->lfHeight = - points_to_pixels(p, &p, TRUE, (long_i)printer_dc);
		நிறுத்து;
	    நிலை L'w':
		lf->lfWidth = points_to_pixels(p, &p, FALSE, (long_i)printer_dc);
		நிறுத்து;
	    நிலை L'W':
		lf->lfWeight = wcstol(p, &p, 10);
		நிறுத்து;
	    நிலை L'b':
		lf->lfWeight = FW_BOLD;
		நிறுத்து;
	    நிலை L'i':
		lf->lfItalic = TRUE;
		நிறுத்து;
	    நிலை L'u':
		lf->lfUnderline = TRUE;
		நிறுத்து;
	    நிலை L's':
		lf->lfStrikeOut = TRUE;
		நிறுத்து;
	    நிலை L'c':
		க்கு (i = 0; i < (முழு)ARRAY_LENGTH(charset_pairs); ++i)
		{
		    எனில் (utf16ascncmp(p, (எழுத்து *)charset_pairs[i].name.string,
			    charset_pairs[i].name.length) == 0)
		    {
			lf->lfCharSet = charset_pairs[i].charset;
			p += charset_pairs[i].name.length;
			நிறுத்து;
		    }
		}

		எனில் (i == (முழு)ARRAY_LENGTH(charset_pairs) && verbose)
		{
		    char_u *s = utf16_to_enc(p, வெற்று);
		    எனில் (s != வெற்று)
		    {
			semsg(_(e_illegal_str_name_str_in_font_name_str),
							   "charset", s, name);
			vim_free(s);
		    }
		}
		நிறுத்து;
	    நிலை L'f':
		// Font features (e.g., "fss19=1").
		// Parsed separately by gui_mch_init_font(); skip here.
		போது (*p && *p != L':')
		    p++;
		நிறுத்து;
	    நிலை L'q':
		க்கு (i = 0; i < (முழு)ARRAY_LENGTH(quality_pairs); ++i)
		{
		    எனில் (utf16ascncmp(p, (எழுத்து *)quality_pairs[i].name.string,
			    quality_pairs[i].name.length) == 0)
		    {
			lf->lfQuality = quality_pairs[i].quality;
			p += quality_pairs[i].name.length;
			நிறுத்து;
		    }
		}

		எனில் (i == (முழு)ARRAY_LENGTH(quality_pairs) && verbose)
		{
		    char_u *s = utf16_to_enc(p, வெற்று);
		    எனில் (s != வெற்று)
		    {
			semsg(_(e_illegal_str_name_str_in_font_name_str),
							   "quality", s, name);
			vim_free(s);
		    }
		}
		நிறுத்து;
	    இயல்பு:
		எனில் (verbose)
		    semsg(_(e_illegal_char_nr_in_font_name_str), p[-1], name);
		செல் theend;
	}
	போது (*p == L':')
	    p++;
    }
    ret = OK;

theend:
    // ron: init lastlf
    எனில் (ret == OK && printer_dc == வெற்று)
    {
	vim_free(lastlf);
	lastlf = ALLOC_ONE(LOGFONTW);
	எனில் (lastlf != வெற்று)
	    mch_memmove(lastlf, lf, அளவு(LOGFONTW));
    }
    vim_free(wname);

    திரும்பு ret;
}

#endif // defined(FEAT_GUI) || defined(FEAT_PRINTER)

#எனில் defined(FEAT_JOB_CHANNEL)
/*
 * Initialize the Winsock dll.
 */
    வெறுமை
channel_init_winsock(வெறுமை)
{
    WSADATA wsaData;
    முழு wsaerr;

    எனில் (WSInitialized)
	திரும்பு;

    wsaerr = WSAStartup(MAKEWORD(2, 2), &wsaData);
    எனில் (wsaerr == 0)
	WSInitialized = TRUE;
}
#endif
