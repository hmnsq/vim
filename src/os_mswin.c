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

#ضمن "vim.h"

#ضمن <sys/types.h>
#ضمن <limits.h>

#ضمن <process.h>
#ضمن <direct.h>

#𐤀𐤌 !defined(FEAT_GUI_MSWIN)
# include <shellapi.h>
#endif

#𐤀𐤌 defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT)
# include <dlgs.h>
# include <winspool.h>
# include <commdlg.h>
#endif

// Record all output and all keyboard & mouse input
// #define MCH_WRITE_DUMP

#ifdef MCH_WRITE_DUMP
FILE* fdDump = عدم;
#endif

#𐤀𐤌 !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
خارجي حرف g_szOrigTitle[];
#endif

#ifdef FEAT_GUI
خارجي HWND s_hwnd;
#وإلا
ساكن HWND s_hwnd = 0;	    // console window handle, set by GetConsoleHwnd()
#endif

#ifdef FEAT_JOB_CHANNEL
صحيح WSInitialized = FALSE; // WinSock is initialized
#endif


/*
 * Save the instance handle of the exe/dll.
 */
    فراغ
SaveInst(HINSTANCE hInst)
{
    g_hinst = hInst;
}

#𐤀𐤌 defined(FEAT_GUI_MSWIN)
/*
 * GUI version of mch_exit().
 * Shut down and exit with status `r'
 * Careful: mch_exit() may be called before mch_init()!
 */
    فراغ
mch_exit_g(صحيح r)
{
    exiting = TRUE;

    display_errors();

    ml_close_all(TRUE);		// remove all memfiles

# ifdef FEAT_OLE
    UninitOLE();
# endif
# ifdef FEAT_JOB_CHANNEL
    𐤀𐤌 (WSInitialized)
    {
	WSInitialized = FALSE;
	WSACleanup();
    }
# endif
# ifdef DYNAMIC_GETTEXT
    dyn_libintl_end();
# endif

    𐤀𐤌 (gui.in_use)
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
تعريفنوع BOOL (WINAPI *PfnRtlGetVersion)(LPOSVERSIONINFOW);
DWORD win_version;
    ساكن فراغ
win_version_init(فراغ)
{
    OSVERSIONINFOW	osver;
    HMODULE		hNtdll;
    PfnRtlGetVersion	pRtlGetVersion;

    hNtdll = GetModuleHandle("ntdll.dll");
    𐤀𐤌 (hNtdll == عدم)
	أرجع;

    pRtlGetVersion =
	(PfnRtlGetVersion) GetProcAddress(hNtdll, "RtlGetVersion");
    𐤀𐤌 (pRtlGetVersion == عدم)
	أرجع;

    osver.dwOSVersionInfoSize = حجم(OSVERSIONINFOW);
    pRtlGetVersion(&osver);
    win_version = MAKE_VER(min(osver.dwMajorVersion, 0xFF),
	    min(osver.dwMinorVersion, 0xFF),
	    min(osver.dwBuildNumber, 0xFFFF));
}

/*
 * Init the tables for toupper() and tolower().
 */
    فراغ
mch_early_init(فراغ)
{
    صحيح		i;

    win_version_init();

    PlatformId();

    // Init the tables for toupper() and tolower()
    لكل (i = 0; i < 256; ++i)
	toupper_tab[i] = tolower_tab[i] = i;
    CharUpperBuff((LPSTR)toupper_tab, 256);
    CharLowerBuff((LPSTR)tolower_tab, 256);
}


/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    صحيح
mch_input_isatty(فراغ)
{
#ifdef FEAT_GUI_MSWIN
# ifdef VIMDLL
    𐤀𐤌 (gui.in_use)
# endif
	أرجع TRUE;	    // GUI always has a tty
#endif
#𐤀𐤌 !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    𐤀𐤌 (isatty(read_cmd_fd))
	أرجع TRUE;
    أرجع FALSE;
#endif
}

/*
 * mch_settitle(): set titlebar of our window
 */
    فراغ
mch_settitle(
    char_u *title,
    char_u *icon UNUSED)
{
#ifdef FEAT_GUI_MSWIN
# ifdef VIMDLL
    𐤀𐤌 (gui.in_use)
# endif
    {
	gui_mch_settitle(title, icon);
	أرجع;
    }
#endif
#𐤀𐤌 !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    𐤀𐤌 (title != عدم)
    {
	WCHAR	*wp = enc_to_utf16(title, عدم);

	𐤀𐤌 (wp == عدم)
	    أرجع;

	SetConsoleTitleW(wp);
	vim_free(wp);
	أرجع;
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
    فراغ
mch_restore_title(صحيح which UNUSED)
{
#𐤀𐤌 !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
    𐤀𐤌 (!gui.in_use)
# endif
	SetConsoleTitle(g_szOrigTitle);
#endif
}


/*
 * Return TRUE if we can restore the title (we can)
 */
    صحيح
mch_can_restore_title(فراغ)
{
    أرجع TRUE;
}


/*
 * Return TRUE if we can restore the icon title (we can't)
 */
    صحيح
mch_can_restore_icon(فراغ)
{
    أرجع FALSE;
}


/*
 * Get absolute file name into buffer "buf" of length "len" bytes,
 * turning all '/'s into '\\'s and getting the correct case of each component
 * of the file name.  Append a (back)slash to a directory name.
 * When 'shellslash' set do it the other way around.
 * Return OK or FAIL.
 */
    صحيح
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    صحيح		len,
    صحيح		force UNUSED)
{
    صحيح		nResult = FAIL;
    WCHAR	*wname;
    WCHAR	wbuf[MAX_PATH];
    char_u	*cname = عدم;

    wname = enc_to_utf16(fname, عدم);
    𐤀𐤌 (wname != عدم && _wfullpath(wbuf, wname, MAX_PATH) != عدم)
    {
	cname = utf16_to_enc((short_u *)wbuf, عدم);
	𐤀𐤌 (cname != عدم)
	{
	    vim_strncpy(buf, cname, len - 1);
	    nResult = OK;
	}
    }
    vim_free(wname);
    vim_free(cname);

#ifdef USE_FNAME_CASE
    fname_case(buf, len);
#وإلا
    slash_adjust(buf);
#endif

    أرجع nResult;
}


/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    صحيح
mch_isFullName(char_u *fname)
{
    // A name like "d:/foo" and "//server/share" is absolute.  "d:foo" is not.
    // /foo and \foo are absolute too because Windows keeps a current drive.
    // Another way to check is to use mch_FullName() and see if the result is
    // the same as the name or mch_FullName() fails.  However, this has quite a
    // bit of overhead, so let's not do that.
    𐤀𐤌 (*fname == NUL)
	أرجع FALSE;
    أرجع ((ASCII_ISALPHA(fname[0]) && fname[1] == ':'
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
    فراغ
slash_adjust(char_u *p)
{
    𐤀𐤌 (path_with_url(p))
	أرجع;

    𐤀𐤌 (*p == '`')
    {
	size_t len = STRLEN(p);

	// don't replace backslash in backtick quoted strings
	𐤀𐤌 (len > 2 && *(p + len - 1) == '`')
	    أرجع;
    }

    طالما (*p)
    {
	𐤀𐤌 (*p == psepcN)
	    *p = psepc;
	MB_PTR_ADV(p);
    }
}

    ساكن صحيح
read_reparse_point(ثابت WCHAR *name, char_u *buf, DWORD *buf_len)
{
    HANDLE h;
    BOOL ok;

    h = CreateFileW(name, FILE_READ_ATTRIBUTES,
	    FILE_SHARE_READ | FILE_SHARE_WRITE, عدم,
	    OPEN_EXISTING,
	    FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,
	    عدم);
    𐤀𐤌 (h == INVALID_HANDLE_VALUE)
	أرجع FAIL;

    ok = DeviceIoControl(h, FSCTL_GET_REPARSE_POINT, عدم, 0, buf, *buf_len,
	    buf_len, عدم);
    CloseHandle(h);

    أرجع ok ? OK : FAIL;
}

    char_u *
resolve_appexeclink(char_u *fname)
{
    DWORD		attr = 0;
    صحيح			idx;
    WCHAR		*p, *end, *wname;
    // The buffer size is arbitrarily chosen to be "big enough" (TM), the
    // ceiling should be around 16k.
    char_u		buf[4096];
    DWORD		buf_len = حجم(buf);
    REPARSE_DATA_BUFFER *rb = (REPARSE_DATA_BUFFER *)buf;

    wname = enc_to_utf16(fname, عدم);
    𐤀𐤌 (wname == عدم)
	أرجع عدم;

    attr = GetFileAttributesW(wname);
    𐤀𐤌 (attr == INVALID_FILE_ATTRIBUTES ||
	    (attr & FILE_ATTRIBUTE_REPARSE_POINT) == 0)
    {
	vim_free(wname);
	أرجع عدم;
    }

    // The applinks are similar to symlinks but with a huge difference: they can
    // only be executed, any other I/O operation on them is bound to fail with
    // ERROR_FILE_NOT_FOUND even though the file exists.
    𐤀𐤌 (read_reparse_point(wname, buf, &buf_len) == FAIL)
    {
	vim_free(wname);
	أرجع عدم;
    }
    vim_free(wname);

    𐤀𐤌 (rb->ReparseTag != IO_REPARSE_TAG_APPEXECLINK)
	أرجع عدم;

    // The (undocumented) reparse buffer contains a set of N null-terminated
    // Unicode strings, the application path is stored in the third one.
    𐤀𐤌 (rb->AppExecLinkReparseBuffer.StringCount < 3)
	أرجع عدم;

    p = rb->AppExecLinkReparseBuffer.StringList;
    end = p + rb->ReparseDataLength / حجم(WCHAR);
    لكل (idx = 0; p < end
	    && idx < (صحيح)rb->AppExecLinkReparseBuffer.StringCount
	    && idx != 2; )
    {
	𐤀𐤌 (*p++ == L'\0')
	    ++idx;
    }

    أرجع utf16_to_enc(p, عدم);
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
    ساكن صحيح
mswin_stat_impl(ثابت WCHAR *name, stat_T *stp, ثابت صحيح resolve)
{
    صحيح			n;
    صحيح			fd;
    BOOL		is_symlink = FALSE;
    HANDLE		hFind, h;
    DWORD		attr = 0;
    DWORD		flag = 0;
    WIN32_FIND_DATAW    findDataW;

#𐤀𐤌 0 && defined(_UCRT)
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
    𐤀𐤌 (resolve)
	// Universal CRT can handle symlinks properly.
	أرجع _wstat(name, stp);
#endif

    // Only reparse points need a directory query to identify symbolic links.
    // In particular, avoid enumerating the directory for each missing file
    // while searching 'runtimepath'.
    attr = GetFileAttributesW(name);
    𐤀𐤌 (attr == INVALID_FILE_ATTRIBUTES)
    {
	DWORD error = GetLastError();

	// A missing file or path cannot provide stat information.  Other
	// errors and UNC paths still need the CRT fallback.
	𐤀𐤌 ((error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND)
		&& !((name[0] == L'\\' || name[0] == L'/')
		    && (name[1] == L'\\' || name[1] == L'/')))
	{
	    errno = ENOENT;
	    أرجع -1;
	}
	أرجع _wstat(name, stp);
    }
    𐤀𐤌 (!(attr & FILE_ATTRIBUTE_REPARSE_POINT))
	أرجع _wstat(name, stp);

    hFind = FindFirstFileW(name, &findDataW);
    𐤀𐤌 (hFind != INVALID_HANDLE_VALUE)
    {
	attr = findDataW.dwFileAttributes;
	𐤀𐤌 ((attr & FILE_ATTRIBUTE_REPARSE_POINT)
		&& (findDataW.dwReserved0 == IO_REPARSE_TAG_SYMLINK))
	    is_symlink = TRUE;
	FindClose(hFind);
    }

    // Use the plain old stat() whenever it's possible.
    𐤀𐤌 (!is_symlink)
	أرجع _wstat(name, stp);

    𐤀𐤌 (!resolve && is_symlink)
	flag = FILE_FLAG_OPEN_REPARSE_POINT;
    𐤀𐤌 (attr & FILE_ATTRIBUTE_DIRECTORY)
	flag |= FILE_FLAG_BACKUP_SEMANTICS;

    h = CreateFileW(name, FILE_READ_ATTRIBUTES,
	    FILE_SHARE_READ | FILE_SHARE_WRITE, عدم, OPEN_EXISTING, flag,
	    عدم);
    𐤀𐤌 (h == INVALID_HANDLE_VALUE)
	أرجع -1;

    fd = _open_osfhandle((intptr_t)h, _O_RDONLY);
    n = _fstat(fd, (بنية _stat *)stp);
    𐤀𐤌 ((n == 0) && (attr & FILE_ATTRIBUTE_DIRECTORY))
	stp->st_mode = (stp->st_mode & ~S_IFMT) | S_IFDIR;
    _close(fd);

    أرجع n;
}

/*
 * stat() can't handle a trailing '/' or '\', remove it first.
 * When 'resolve' is true behave as lstat() wrt symlinks.
 */
    ساكن صحيح
stat_impl(ثابت حرف *name, stat_T *stp, ثابت صحيح resolve)
{
    // WinNT and later can use _MAX_PATH wide characters for a pathname, which
    // means that the maximum pathname is _MAX_PATH * 3 bytes when 'enc' is
    // UTF-8.
    char_u	buf[_MAX_PATH * 3 + 1];
    size_t	buflen;
    char_u	*p;
    WCHAR	*wp;
    صحيح		n;

    vim_strncpy((char_u *)buf, (char_u *)name, حجم(buf) - 1);
    buflen = STRLEN(buf);
    p = buf + buflen;
    𐤀𐤌 (p > buf)
	MB_PTR_BACK(buf, p);

    // Remove trailing '\\' except root path.
    𐤀𐤌 (p > buf && (*p == '\\' || *p == '/') && p[-1] != ':')
    {
	*p = NUL;
	buflen = (size_t)(p - buf);
    }

    𐤀𐤌 ((buf[0] == '\\' && buf[1] == '\\') || (buf[0] == '/' && buf[1] == '/'))
    {
	// UNC root path must be followed by '\\'.
	p = vim_strpbrk(buf + 2, (char_u *)"\\/");
	𐤀𐤌 (p != عدم)
	{
	    p = vim_strpbrk(p + 1, (char_u *)"\\/");
	    𐤀𐤌 (p == عدم)
		STRCPY(buf + buflen, "\\");
	}
    }

    wp = enc_to_utf16(buf, عدم);
    𐤀𐤌 (wp == عدم)
	أرجع -1;

    n = mswin_stat_impl(wp, stp, resolve);
    vim_free(wp);
    أرجع n;
}

    صحيح
vim_lstat(ثابت حرف *name, stat_T *stp)
{
    أرجع stat_impl(name, stp, FALSE);
}

    صحيح
vim_stat(ثابت حرف *name, stat_T *stp)
{
    أرجع stat_impl(name, stp, TRUE);
}

#𐤀𐤌 defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    فراغ
mch_settmode(tmode_T tmode UNUSED)
{
    // nothing to do
}

    صحيح
mch_get_shellsize(فراغ)
{
    // never used
    أرجع OK;
}

    فراغ
mch_set_shellsize(فراغ)
{
    // never used
}

/*
 * Rows and/or Columns has changed.
 */
    فراغ
mch_new_shellsize(فراغ)
{
    // never used
}

    فراغ
mch_calc_cell_size(بنية cellsize *cs_out UNUSED)
{
    // never used
}

#endif

/*
 * We have no job control, so fake it by starting a new shell.
 */
    فراغ
mch_suspend(فراغ)
{
    suspend_shell();
}

#𐤀𐤌 defined(USE_MCH_ERRMSG)

# ifdef display_errors
#  undef display_errors
# endif

/*
 * Display the saved error message(s).
 */
    فراغ
display_errors(فراغ)
{
# ifdef FEAT_GUI
    char_u *p;

#  ifdef VIMDLL
    𐤀𐤌 (gui.in_use || gui.starting)
#  endif
    {
	𐤀𐤌 (error_ga.ga_data != عدم)
	{
	    // avoid putting up a message box with blanks only
	    لكل (p = (char_u *)error_ga.ga_data; *p; ++p)
		𐤀𐤌 (!SAFE_isspace(*p))
		{
		    // Only use a dialog when not using --gui-dialog-file:
		    // write text to a file.
		    𐤀𐤌 (!gui_dialog_log((char_u *)"Errors", p))
			(فراغ)gui_mch_dialog(
				     gui.starting ? VIM_INFO :
					     VIM_ERROR,
				     gui.starting ? (char_u *)_("Message") :
					     (char_u *)_("Error"),
					     p, (char_u *)_("&Ok"),
					1, عدم, FALSE);
		    اكسر;
		}
	    ga_clear(&error_ga);
	}
	أرجع;
    }
# endif
# 𐤀𐤌 !defined(FEAT_GUI) || defined(VIMDLL)
    FlushFileBuffers(GetStdHandle(STD_ERROR_HANDLE));
# endif
}
#endif


/*
 * Return TRUE if "p" contain a wildcard that can be expanded by
 * dos_expandpath().
 */
    صحيح
mch_has_exp_wildcard(char_u *p)
{
    لكل ( ; *p; MB_PTR_ADV(p))
    {
	𐤀𐤌 (vim_strchr((char_u *)"?*[", *p) != عدم
		|| (*p == '~' && p[1] != NUL))
	    أرجع TRUE;
    }
    أرجع FALSE;
}

/*
 * Return TRUE if "p" contain a wildcard or a "~1" kind of thing (could be a
 * shortened file name).
 */
    صحيح
mch_has_wildcard(char_u *p)
{
    لكل ( ; *p; MB_PTR_ADV(p))
    {
	𐤀𐤌 (vim_strchr((char_u *)"?*$[`", *p) != عدم
		|| (*p == '~' && p[1] != NUL))
	    أرجع TRUE;
    }
    أرجع FALSE;
}


/*
 * The normal _chdir() does not change the default drive.  This one does.
 * Returning 0 implies success; -1 implies failure.
 */
    صحيح
mch_chdir(حرف *path)
{
    WCHAR   *p;
    صحيح	    n;

    𐤀𐤌 (path[0] == NUL)		// just checking...
	أرجع -1;

    𐤀𐤌 (p_verbose >= 5)
    {
	verbose_enter();
	smsg("chdir(%s)", path);
	verbose_leave();
    }
    𐤀𐤌 (SAFE_isalpha(path[0]) && path[1] == ':')	// has a drive name
    {
	// If we can change to the drive, skip that part of the path.  If we
	// can't then the current directory may be invalid, try using chdir()
	// with the whole path.
	𐤀𐤌 (_chdrive(TOLOWER_ASC(path[0]) - 'a' + 1) == 0)
	    path += 2;
    }

    𐤀𐤌 (*path == NUL)		// drive name only
	أرجع 0;

    p = enc_to_utf16((char_u *)path, عدم);
    𐤀𐤌 (p == عدم)
	أرجع -1;

    n = _wchdir(p);
    vim_free(p);
    أرجع n;
}


#𐤀𐤌 defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
/*
 * return non-zero if a character is available
 */
    صحيح
mch_char_avail(فراغ)
{
    // never used
    أرجع TRUE;
}

# 𐤀𐤌 defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    صحيح
mch_check_messages(فراغ)
{
    // TODO: check for messages
    أرجع TRUE;
}
# endif
#endif


#𐤀𐤌 defined(FEAT_LIBCALL)
/*
 * Call a DLL routine which takes either a string or int param
 * and returns an allocated string.
 * Return OK if it worked, FAIL if not.
 */
تعريفنوع char_u *(*MYSTRPROCSTR)(char_u *);
تعريفنوع صحيح (*MYSTRPROCINT)(char_u *);
تعريفنوع char_u *(*MYINTPROCSTR)(صحيح);
تعريفنوع صحيح (*MYINTPROCINT)(صحيح);

/*
 * Check if a pointer points to a valid NUL terminated string.
 * Return the length of the string, including terminating NUL.
 * Returns 0 for an invalid pointer, 1 for an empty string.
 */
    ساكن size_t
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
    𐤀𐤌 (!VirtualQuery(str, &mbi, حجم(mbi)))
	أرجع 0;

    // pre cast these (typing savers)
    long_u dwStr = (long_u)str;
    long_u dwBaseAddress = (long_u)mbi.BaseAddress;

    // get start address of page that str is on
    long_u strPage = dwStr - (dwStr - dwBaseAddress) % si.dwPageSize;

    // get length from str to end of page
    long_u pageLength = si.dwPageSize - (dwStr - strPage);

    لكل (p = str; !IsBadReadPtr(p, (UINT)pageLength);
	    p += pageLength, pageLength = si.dwPageSize)
	لكل (i = 0; i < pageLength; ++i, ++length)
	    𐤀𐤌 (p[i] == NUL)
		أرجع length + 1;

    أرجع 0;
}

/*
 * Passed to do_in_runtimepath() to load a vim.ico file.
 */
    ساكن فراغ
mch_icon_load_cb(char_u *fname, فراغ *cookie)
{
    HANDLE *h = (HANDLE *)cookie;

    *h = LoadImage(عدم,
		   (LPSTR)fname,
		   IMAGE_ICON,
		   64,
		   64,
		   LR_LOADFROMFILE | LR_LOADMAP3DCOLORS);
}

/*
 * Try loading an icon file from 'runtimepath'.
 */
    صحيح
mch_icon_load(HANDLE *iconp)
{
    أرجع do_in_runtimepath((char_u *)"bitmaps/vim.ico",
						  0, mch_icon_load_cb, iconp);
}

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if the OS PRNG is not available or something went wrong.
 */
    صحيح
mch_get_random(char_u *buf, صحيح len)
{
    ساكن صحيح		initialized = NOTDONE;
    ساكن HINSTANCE	hInstLib;
    ساكن BOOL (WINAPI *pProcessPrng)(PUCHAR, ULONG);

    𐤀𐤌 (initialized == NOTDONE)
    {
	hInstLib = vimLoadLib("bcryptprimitives.dll");
	𐤀𐤌 (hInstLib != عدم)
	    pProcessPrng = (فراغ *)GetProcAddress(hInstLib, "ProcessPrng");
	𐤀𐤌 (hInstLib == عدم || pProcessPrng == عدم)
	{
	    FreeLibrary(hInstLib);
	    initialized = FAIL;
	}
	وإلا
	    initialized = OK;
    }

    𐤀𐤌 (initialized == FAIL)
	أرجع FAIL;

    // According to the documentation this call cannot fail.
    pProcessPrng(buf, len);

    أرجع OK;
}

    صحيح
mch_libcall(
    char_u	*libname,
    char_u	*funcname,
    char_u	*argstring,	// NULL when using a argint
    صحيح		argint,
    char_u	**string_result,// NULL when using number_result
    صحيح		*number_result)
{
    HINSTANCE		hinstLib;

    BOOL fRunTimeLinkSuccess = FALSE;

    // Get a handle to the DLL module.
    hinstLib = vimLoadLib((حرف *)libname);

    // If the handle is valid, try to get the function address.
    𐤀𐤌 (hinstLib != عدم)
    {
	char_u	*retval_str = عدم;
	صحيح	retval_int = 0;
	size_t	len;

# ifdef HAVE_TRY_EXCEPT
	__try
	{
# endif
	𐤀𐤌 (argstring != عدم)
	{
	    // Call with string argument
	    𐤀𐤌 (string_result != عدم)
	    {
		MYSTRPROCSTR proc;

		proc = (MYSTRPROCSTR)GetProcAddress(hinstLib, (LPCSTR)funcname);
		𐤀𐤌 (proc != عدم)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_str = proc(argstring);
		}
	    }
	    وإلا
	    {
		MYSTRPROCINT proc;

		proc = (MYSTRPROCINT)GetProcAddress(hinstLib, (LPCSTR)funcname);
		𐤀𐤌 (proc != عدم)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_int = proc(argstring);
		}
	    }
	}
	وإلا
	{
	    // Call with number argument
	    𐤀𐤌 (string_result != عدم)
	    {
		MYINTPROCSTR proc;

		proc = (MYINTPROCSTR)GetProcAddress(hinstLib, (LPCSTR)funcname);
		𐤀𐤌 (proc != عدم)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_str = proc(argint);
		}
	    }
	    وإلا
	    {
		MYINTPROCINT proc;

		proc = (MYINTPROCINT)GetProcAddress(hinstLib, (LPCSTR)funcname);
		𐤀𐤌 (proc != عدم)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_int = proc(argint);
		}
	    }
	}

	// Save the string before we free the library.
	// Assume that a "1" result is an illegal pointer.
	𐤀𐤌 (string_result == عدم)
	    *number_result = retval_int;
	وإلا 𐤀𐤌 (retval_str != عدم
		&& (len = check_str_len(retval_str)) > 0)
	{
	    *string_result = alloc(len);
	    𐤀𐤌 (*string_result != عدم)
		mch_memmove(*string_result, retval_str, len);
	}

# ifdef HAVE_TRY_EXCEPT
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	    𐤀𐤌 (GetExceptionCode() == EXCEPTION_STACK_OVERFLOW)
		_resetstkoflw();
	    fRunTimeLinkSuccess = FALSE;
	}
# endif

	// Free the DLL module.
	(فراغ)FreeLibrary(hinstLib);
    }

    𐤀𐤌 (!fRunTimeLinkSuccess)
    {
	semsg(_(e_library_call_failed_for_str), funcname);
	أرجع FAIL;
    }

    أرجع OK;
}
#endif

/*
 * Debugging helper: expose the MCH_WRITE_DUMP stuff to other modules
 */
    فراغ
DumpPutS(ثابت حرف *psz UNUSED)
{
#ifdef MCH_WRITE_DUMP
    𐤀𐤌 (fdDump)
    {
	fputs(psz, fdDump);
	𐤀𐤌 (psz[strlen(psz) - 1] != '\n')
	    fputc('\n', fdDump);
	fflush(fdDump);
    }
#endif
}

#ifdef _DEBUG

فراغ __cdecl
Trace(
    حرف *pszFormat,
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

#𐤀𐤌 !defined(FEAT_GUI) || defined(VIMDLL)
خارجي HWND g_hWnd;	// This is in os_win32.c.

/*
 * Showing the printer dialog is tricky since we have no GUI
 * window to parent it. The following routines are needed to
 * get the window parenting and Z-order to work properly.
 */
    ساكن فراغ
GetConsoleHwnd(فراغ)
{
    // Skip if it's already set.
    𐤀𐤌 (s_hwnd != 0)
	أرجع;

    // Window handle may have been found by init code (Windows NT only)
    𐤀𐤌 (g_hWnd != 0)
    {
	s_hwnd = g_hWnd;
	أرجع;
    }

    s_hwnd = GetConsoleWindow();
}

/*
 * Console implementation of ":winpos".
 */
    صحيح
mch_get_winpos(صحيح *x, صحيح *y)
{
    RECT  rect;

    GetConsoleHwnd();
    GetWindowRect(s_hwnd, &rect);
    *x = rect.left;
    *y = rect.top;
    أرجع OK;
}

/*
 * Console implementation of ":winpos x y".
 */
    فراغ
mch_set_winpos(صحيح x, صحيح y)
{
    GetConsoleHwnd();
    SetWindowPos(s_hwnd, عدم, x, y, 0, 0,
		 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}
#endif

#𐤀𐤌 defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT)

//=================================================================
// Win32 printer stuff

ساكن HFONT		prt_font_handles[2][2][2];
ساكن PRINTDLGW	prt_dlg;
ساكن ثابت صحيح	boldface[2] = {FW_REGULAR, FW_BOLD};
ساكن TEXTMETRIC	prt_tm;
ساكن صحيح		prt_line_height;
ساكن صحيح		prt_number_width;
ساكن صحيح		prt_left_margin;
ساكن صحيح		prt_right_margin;
ساكن صحيح		prt_top_margin;
ساكن char_u		szAppName[] = TEXT("VIM");
ساكن HWND		hDlgPrint;
ساكن صحيح		*bUserAbort = عدم;
ساكن char_u		*prt_name = عدم;

// Defines which are also in vim.rc.
# define IDC_BOX1		400
# define IDC_PRINTTEXT1		401
# define IDC_PRINTTEXT2		402
# define IDC_PROGRESS		403

    ساكن BOOL
vimSetDlgItemText(HWND hDlg, صحيح nIDDlgItem, char_u *s)
{
    WCHAR   *wp;
    BOOL    ret;

    wp = enc_to_utf16(s, عدم);
    𐤀𐤌 (wp == عدم)
	أرجع FALSE;

    ret = SetDlgItemTextW(hDlg, nIDDlgItem, wp);
    vim_free(wp);
    أرجع ret;
}

/*
 * Convert BGR to RGB for Windows GDI calls
 */
    ساكن COLORREF
swap_me(COLORREF colorref)
{
    صحيح  temp;
    حرف *ptr = (حرف *)&colorref;

    temp = *(ptr);
    *(ptr ) = *(ptr + 2);
    *(ptr + 2) = temp;
    أرجع colorref;
}

    ساكن INT_PTR CALLBACK
PrintDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam UNUSED,
	LPARAM lParam UNUSED)
{
# ifdef FEAT_GETTEXT
    NONCLIENTMETRICS nm;
    ساكن HFONT hfont;
# endif

    بدل (message)
    {
	حالة WM_INITDIALOG:
# ifdef FEAT_GETTEXT
	    nm.cbSize = حجم(NONCLIENTMETRICS);
	    𐤀𐤌 (SystemParametersInfo(
			SPI_GETNONCLIENTMETRICS,
			حجم(NONCLIENTMETRICS),
			&nm,
			0))
	    {
		حرف buff[MAX_PATH];
		صحيح i;

		// Translate the dialog texts
		hfont = CreateFontIndirect(&nm.lfMessageFont);
		لكل (i = IDC_PRINTTEXT1; i <= IDC_PROGRESS; i++)
		{
		    SendDlgItemMessage(hDlg, i, WM_SETFONT, (WPARAM)hfont, 1);
		    𐤀𐤌 (GetDlgItemText(hDlg,i, buff, حجم(buff)))
			vimSetDlgItemText(hDlg,i, (char_u *)_(buff));
		}
		SendDlgItemMessage(hDlg, IDCANCEL,
						WM_SETFONT, (WPARAM)hfont, 1);
		𐤀𐤌 (GetDlgItemText(hDlg,IDCANCEL, buff, حجم(buff)))
		    vimSetDlgItemText(hDlg,IDCANCEL, (char_u *)_(buff));
	    }
# endif
	    SetWindowText(hDlg, (LPCSTR)szAppName);
	    𐤀𐤌 (prt_name != عدم)
	    {
		vimSetDlgItemText(hDlg, IDC_PRINTTEXT2, (char_u *)prt_name);
		VIM_CLEAR(prt_name);
	    }
	    EnableMenuItem(GetSystemMenu(hDlg, FALSE), SC_CLOSE, MF_GRAYED);
# 𐤀𐤌 !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	    𐤀𐤌 (!gui.in_use)
#  endif
		BringWindowToTop(s_hwnd);
# endif
	    أرجع TRUE;

	حالة WM_COMMAND:
	    *bUserAbort = TRUE;
	    EnableWindow(GetParent(hDlg), TRUE);
	    DestroyWindow(hDlg);
	    hDlgPrint = عدم;
# ifdef FEAT_GETTEXT
	    DeleteObject(hfont);
# endif
	    أرجع TRUE;
    }
    أرجع FALSE;
}

    ساكن BOOL CALLBACK
AbortProc(HDC hdcPrn UNUSED, صحيح iCode UNUSED)
{
    MSG msg;

    طالما (!*bUserAbort && PeekMessageW(&msg, عدم, 0, 0, PM_REMOVE))
    {
	𐤀𐤌 (!hDlgPrint || !IsDialogMessageW(hDlgPrint, &msg))
	{
	    TranslateMessage(&msg);
	    DispatchMessageW(&msg);
	}
    }
    أرجع !*bUserAbort;
}

    ساكن UINT_PTR CALLBACK
PrintHookProc(
	HWND hDlg,	// handle to dialog box
	UINT uiMsg,	// message identifier
	WPARAM wParam UNUSED,	// message parameter
	LPARAM lParam	// message parameter
	)
{
    HWND	hwndOwner;
    RECT	rc, rcDlg, rcOwner;
    PRINTDLGW	*pPD;

    𐤀𐤌 (uiMsg != WM_INITDIALOG)
	أرجع FALSE;

    // Get the owner window and dialog box rectangles.
    𐤀𐤌 ((hwndOwner = GetParent(hDlg)) == عدم)
	hwndOwner = GetDesktopWindow();

    GetWindowRect(hwndOwner, &rcOwner);
    GetWindowRect(hDlg, &rcDlg);
    CopyRect(&rc, &rcOwner);

    // Offset the owner and dialog box rectangles so that
    // right and bottom values represent the width and
    // height, and then offset the owner again to discard
    // space taken up by the dialog box.

    OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top);
    OffsetRect(&rc, -rc.left, -rc.top);
    OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom);

    // The new position is the sum of half the remaining
    // space and the owner's original position.

    SetWindowPos(hDlg,
	    HWND_TOP,
	    rcOwner.left + (rc.right / 2),
	    rcOwner.top + (rc.bottom / 2),
	    0, 0,		// ignores size arguments
	    SWP_NOSIZE);

    //  tackle the printdlg copiesctrl problem
    pPD = (PRINTDLGW *)lParam;
    pPD->nCopies = (WORD)pPD->lCustData;
    SetDlgItemInt( hDlg, edt3, pPD->nCopies, FALSE );
    //  Bring the window to top
    BringWindowToTop(GetParent(hDlg));
    SetForegroundWindow(hDlg);

    أرجع FALSE;
}

    فراغ
mch_print_cleanup(فراغ)
{
    صحيح pifItalic;
    صحيح pifBold;
    صحيح pifUnderline;

    لكل (pifBold = 0; pifBold <= 1; pifBold++)
	لكل (pifItalic = 0; pifItalic <= 1; pifItalic++)
	    لكل (pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
		DeleteObject(prt_font_handles[pifBold][pifItalic][pifUnderline]);

    𐤀𐤌 (prt_dlg.hDC != عدم)
	DeleteDC(prt_dlg.hDC);
    𐤀𐤌 (!*bUserAbort && hDlgPrint != عدم)
	SendMessage(hDlgPrint, WM_COMMAND, 0, 0);
}

    ساكن صحيح
to_device_units(صحيح idx, صحيح dpi, صحيح physsize, صحيح offset, صحيح def_number)
{
    صحيح		ret = 0;
    صحيح		u;
    صحيح		nr;

    u = prt_get_unit(idx);
    𐤀𐤌 (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_number;
    }
    وإلا
	nr = printer_opts[idx].number;

    بدل (u)
    {
	حالة PRT_UNIT_PERC:
	    ret = (physsize * nr) / 100;
	    اكسر;
	حالة PRT_UNIT_INCH:
	    ret = (nr * dpi);
	    اكسر;
	حالة PRT_UNIT_MM:
	    ret = (nr * 10 * dpi) / 254;
	    اكسر;
	حالة PRT_UNIT_POINT:
	    ret = (nr * 10 * dpi) / 720;
	    اكسر;
    }

    𐤀𐤌 (ret < offset)
	أرجع 0;
    وإلا
	أرجع ret - offset;
}

    ساكن صحيح
prt_get_cpl(فراغ)
{
    صحيح		hr;
    صحيح		phyw;
    صحيح		dvoff;
    صحيح		rev_offset;
    صحيح		dpi;

    GetTextMetrics(prt_dlg.hDC, &prt_tm);
    prt_line_height = prt_tm.tmHeight + prt_tm.tmExternalLeading;

    hr	    = GetDeviceCaps(prt_dlg.hDC, HORZRES);
    phyw    = GetDeviceCaps(prt_dlg.hDC, PHYSICALWIDTH);
    dvoff   = GetDeviceCaps(prt_dlg.hDC, PHYSICALOFFSETX);
    dpi	    = GetDeviceCaps(prt_dlg.hDC, LOGPIXELSX);

    rev_offset = phyw - (dvoff + hr);

    prt_left_margin = to_device_units(OPT_PRINT_LEFT, dpi, phyw, dvoff, 10);
    𐤀𐤌 (prt_use_number())
    {
	prt_number_width = PRINT_NUMBER_WIDTH * prt_tm.tmAveCharWidth;
	prt_left_margin += prt_number_width;
    }
    وإلا
	prt_number_width = 0;

    prt_right_margin = hr - to_device_units(OPT_PRINT_RIGHT, dpi, phyw,
							       rev_offset, 5);

    أرجع (prt_right_margin - prt_left_margin) / prt_tm.tmAveCharWidth;
}

    ساكن صحيح
prt_get_lpp(فراغ)
{
    صحيح vr;
    صحيح phyw;
    صحيح dvoff;
    صحيح rev_offset;
    صحيح	bottom_margin;
    صحيح	dpi;

    vr	    = GetDeviceCaps(prt_dlg.hDC, VERTRES);
    phyw    = GetDeviceCaps(prt_dlg.hDC, PHYSICALHEIGHT);
    dvoff   = GetDeviceCaps(prt_dlg.hDC, PHYSICALOFFSETY);
    dpi	    = GetDeviceCaps(prt_dlg.hDC, LOGPIXELSY);

    rev_offset = phyw - (dvoff + vr);

    prt_top_margin = to_device_units(OPT_PRINT_TOP, dpi, phyw, dvoff, 5);

    // adjust top margin if there is a header
    prt_top_margin += prt_line_height * prt_header_height();

    bottom_margin = vr - to_device_units(OPT_PRINT_BOT, dpi, phyw,
							       rev_offset, 5);

    أرجع (bottom_margin - prt_top_margin) / prt_line_height;
}

    صحيح
mch_print_init(prt_settings_T *psettings, char_u *jobname, صحيح forceit)
{
    ساكن HGLOBAL	stored_dm = عدم;
    ساكن HGLOBAL	stored_devn = عدم;
    ساكن صحيح		stored_nCopies = 1;
    ساكن صحيح		stored_nFlags = 0;

    LOGFONTW		fLogFont;
    صحيح			pifItalic;
    صحيح			pifBold;
    صحيح			pifUnderline;

    DEVMODEW		*mem;
    DEVNAMES		*devname;
    صحيح			i;
    DWORD		err;

    bUserAbort = &(psettings->user_abort);
    CLEAR_FIELD(prt_dlg);
    prt_dlg.lStructSize = حجم(PRINTDLGW);
# 𐤀𐤌 !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
    𐤀𐤌 (!gui.in_use)
#  endif
	GetConsoleHwnd();	    // get value of s_hwnd
# endif
    prt_dlg.hwndOwner = s_hwnd;
    prt_dlg.Flags = PD_NOPAGENUMS | PD_NOSELECTION | PD_RETURNDC;
    𐤀𐤌 (!forceit)
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
    𐤀𐤌 (forceit
# 𐤀𐤌 !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	    || (!gui.in_use && !term_console)
#  وإلا
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
	𐤀𐤌 (*p_pdev != NUL)
	    prt_dlg.hDC = CreateDC(عدم, (LPCSTR)p_pdev, عدم, عدم);
	وإلا
	{
	    prt_dlg.Flags |= PD_RETURNDEFAULT;
	    𐤀𐤌 (PrintDlgW(&prt_dlg) == 0)
		اذهب init_fail_dlg;
	}
    }
    وإلا 𐤀𐤌 (PrintDlgW(&prt_dlg) == 0)
	اذهب init_fail_dlg;
    وإلا
    {
	/*
	 * keep the previous driver context
	 */
	stored_dm = prt_dlg.hDevMode;
	stored_devn = prt_dlg.hDevNames;
	stored_nFlags = prt_dlg.Flags;
	stored_nCopies = prt_dlg.nCopies;
    }

    𐤀𐤌 (prt_dlg.hDC == عدم)
    {
	emsg(_(e_printer_selection_failed));
	mch_print_cleanup();
	أرجع FALSE;
    }

    // Not all printer drivers report the support of color (or grey) in the
    // same way.  Let's set has_color if there appears to be some way to print
    // more than B&W.
    i = GetDeviceCaps(prt_dlg.hDC, NUMCOLORS);
    psettings->has_color = (GetDeviceCaps(prt_dlg.hDC, BITSPIXEL) > 1
				   || GetDeviceCaps(prt_dlg.hDC, PLANES) > 1
				   || i > 2 || i == -1);

    // Ensure all font styles are baseline aligned
    SetTextAlign(prt_dlg.hDC, TA_BASELINE|TA_LEFT);

    /*
     * On some windows systems the nCopies parameter is not
     * passed back correctly. It must be retrieved from the
     * hDevMode struct.
     */
    mem = (DEVMODEW *)GlobalLock(prt_dlg.hDevMode);
    𐤀𐤌 (mem != عدم)
    {
	𐤀𐤌 (mem->dmCopies != 1)
	    stored_nCopies = mem->dmCopies;
	𐤀𐤌 ((mem->dmFields & DM_DUPLEX) && (mem->dmDuplex & ~DMDUP_SIMPLEX))
	    psettings->duplex = TRUE;
	𐤀𐤌 ((mem->dmFields & DM_COLOR) && (mem->dmColor & DMCOLOR_COLOR))
	    psettings->has_color = TRUE;
    }
    GlobalUnlock(prt_dlg.hDevMode);

    devname = (DEVNAMES *)GlobalLock(prt_dlg.hDevNames);
    𐤀𐤌 (devname != 0)
    {
	WCHAR	*wprinter_name = (WCHAR *)devname + devname->wDeviceOffset;
	WCHAR	*wport_name = (WCHAR *)devname + devname->wOutputOffset;
	char_u	*text = (char_u *)_("to %s on %s");
	char_u  *printer_name = utf16_to_enc(wprinter_name, عدم);
	char_u	*port_name = utf16_to_enc(wport_name, عدم);

	𐤀𐤌 (printer_name != عدم && port_name != عدم)
	    prt_name = alloc(STRLEN(printer_name)
					   + STRLEN(port_name) + STRLEN(text));
	𐤀𐤌 (prt_name != عدم)
	    wsprintf((حرف *)prt_name, (ثابت حرف *)text,
		    printer_name, port_name);
	vim_free(printer_name);
	vim_free(port_name);
    }
    GlobalUnlock(prt_dlg.hDevNames);

    /*
     * Initialise the font according to 'printfont'
     */
    CLEAR_FIELD(fLogFont);
    𐤀𐤌 (get_logfont(&fLogFont, p_pfn, prt_dlg.hDC, TRUE) == FAIL)
    {
	semsg(_(e_unknown_printer_font_str), p_pfn);
	mch_print_cleanup();
	أرجع FALSE;
    }

    لكل (pifBold = 0; pifBold <= 1; pifBold++)
	لكل (pifItalic = 0; pifItalic <= 1; pifItalic++)
	    لكل (pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
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
    psettings->chars_per_line = prt_get_cpl();
    psettings->lines_per_page = prt_get_lpp();
    𐤀𐤌 (prt_dlg.Flags & PD_USEDEVMODECOPIESANDCOLLATE)
    {
	psettings->n_collated_copies = (prt_dlg.Flags & PD_COLLATE)
						    ? prt_dlg.nCopies : 1;
	psettings->n_uncollated_copies = (prt_dlg.Flags & PD_COLLATE)
						    ? 1 : prt_dlg.nCopies;

	𐤀𐤌 (psettings->n_collated_copies == 0)
	    psettings->n_collated_copies = 1;

	𐤀𐤌 (psettings->n_uncollated_copies == 0)
	    psettings->n_uncollated_copies = 1;
    }
    وإلا
    {
	psettings->n_collated_copies = 1;
	psettings->n_uncollated_copies = 1;
    }

    psettings->jobname = jobname;

    أرجع TRUE;

init_fail_dlg:
    err = CommDlgExtendedError();
    𐤀𐤌 (err)
    {
	char_u *buf;

	// I suspect FormatMessage() doesn't work for values returned by
	// CommDlgExtendedError().  What does?
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		عدم, err, 0, (LPTSTR)(&buf), 0, عدم);
	semsg(_(e_print_error_str),
		buf == عدم ? (char_u *)_("Unknown") : buf);
	LocalFree((LPVOID)(buf));
    }
    وإلا
	msg_clr_eos(); // Maybe canceled

    mch_print_cleanup();
    أرجع FALSE;
}


    صحيح
mch_print_begin(prt_settings_T *psettings)
{
    صحيح			ret = 0;
    حرف		szBuffer[300];
    WCHAR		*wp;

    hDlgPrint = CreateDialog(g_hinst, TEXT("PrintDlgBox"),
					     prt_dlg.hwndOwner, PrintDlgProc);
    SetAbortProc(prt_dlg.hDC, AbortProc);
    wsprintf(szBuffer, _("Printing '%s'"), gettail(psettings->jobname));
    vimSetDlgItemText(hDlgPrint, IDC_PRINTTEXT1, (char_u *)szBuffer);

    wp = enc_to_utf16(psettings->jobname, عدم);
    𐤀𐤌 (wp != عدم)
    {
	DOCINFOW	di;

	CLEAR_FIELD(di);
	di.cbSize = حجم(di);
	di.lpszDocName = wp;
	ret = StartDocW(prt_dlg.hDC, &di);
	vim_free(wp);
    }

# ifdef FEAT_GUI
    // Give focus back to main window (when using MDI).
#  ifdef VIMDLL
    𐤀𐤌 (gui.in_use)
#  endif
	SetFocus(s_hwnd);
# endif

    أرجع (ret > 0);
}

    فراغ
mch_print_end(prt_settings_T *psettings UNUSED)
{
    EndDoc(prt_dlg.hDC);
    𐤀𐤌 (!*bUserAbort && hDlgPrint != عدم)
	SendMessage(hDlgPrint, WM_COMMAND, 0, 0);
}

    صحيح
mch_print_end_page(فراغ)
{
    أرجع (EndPage(prt_dlg.hDC) > 0);
}

    صحيح
mch_print_begin_page(char_u *msg)
{
    𐤀𐤌 (msg != عدم)
	vimSetDlgItemText(hDlgPrint, IDC_PROGRESS, msg);
    أرجع (StartPage(prt_dlg.hDC) > 0);
}

    صحيح
mch_print_blank_page(فراغ)
{
    أرجع (mch_print_begin_page(عدم) ? (mch_print_end_page()) : FALSE);
}

ساكن صحيح prt_pos_x = 0;
ساكن صحيح prt_pos_y = 0;

    فراغ
mch_print_start_line(صحيح margin, صحيح page_line)
{
    𐤀𐤌 (margin)
	prt_pos_x = -prt_number_width;
    وإلا
	prt_pos_x = 0;
    prt_pos_y = page_line * prt_line_height
				 + prt_tm.tmAscent + prt_tm.tmExternalLeading;
}

    صحيح
mch_print_text_out(char_u *p, صحيح len)
{
    SIZE	sz;
    WCHAR	*wp;
    صحيح		wlen = len;
    صحيح		ret = FALSE;

    wp = enc_to_utf16(p, &wlen);
    𐤀𐤌 (wp == عدم)
	أرجع FALSE;

    TextOutW(prt_dlg.hDC, prt_pos_x + prt_left_margin,
	    prt_pos_y + prt_top_margin, wp, wlen);
    GetTextExtentPoint32W(prt_dlg.hDC, wp, wlen, &sz);
    vim_free(wp);
    prt_pos_x += (sz.cx - prt_tm.tmOverhang);
    // This is wrong when printing spaces for a TAB.
    𐤀𐤌 (p[len] != NUL)
    {
	wlen = mb_ptr2len(p + len);
	wp = enc_to_utf16(p + len, &wlen);
	𐤀𐤌 (wp != عدم)
	{
	    GetTextExtentPoint32W(prt_dlg.hDC, wp, 1, &sz);
	    ret = (prt_pos_x + prt_left_margin + sz.cx > prt_right_margin);
	    vim_free(wp);
	}
    }
    أرجع ret;
}

    فراغ
mch_print_set_font(صحيح iBold, صحيح iItalic, صحيح iUnderline)
{
    SelectObject(prt_dlg.hDC, prt_font_handles[iBold][iItalic][iUnderline]);
}

    فراغ
mch_print_set_bg(long_u bgcol)
{
    SetBkColor(prt_dlg.hDC, GetNearestColor(prt_dlg.hDC,
						   swap_me((COLORREF)bgcol)));
    /*
     * With a white background we can draw characters transparent, which is
     * good for italic characters that overlap to the next char cell.
     */
    𐤀𐤌 (bgcol == 0xffffffUL)
	SetBkMode(prt_dlg.hDC, TRANSPARENT);
    وإلا
	SetBkMode(prt_dlg.hDC, OPAQUE);
}

    فراغ
mch_print_set_fg(long_u fgcol)
{
    SetTextColor(prt_dlg.hDC, GetNearestColor(prt_dlg.hDC,
						   swap_me((COLORREF)fgcol)));
}

#endif // FEAT_PRINTER && !FEAT_POSTSCRIPT



#𐤀𐤌 defined(FEAT_SHORTCUT)
# include <shlobj.h>

# define is_path_sep(c)	    ((c) == L'\\' || (c) == L'/')

    ساكن صحيح
is_reparse_point_included(LPCWSTR fname)
{
    LPCWSTR	p = fname, q;
    WCHAR	buf[MAX_PATH];
    DWORD	attr;

    𐤀𐤌 (SAFE_isalpha(p[0]) && p[1] == L':' && is_path_sep(p[2]))
	p += 3;
    وإلا 𐤀𐤌 (is_path_sep(p[0]) && is_path_sep(p[1]))
	p += 2;

    طالما (*p != L'\0')
    {
	q = wcspbrk(p, L"\\/");
	𐤀𐤌 (q == عدم)
	    p = q = fname + wcslen(fname);
	وإلا
	    p = q + 1;
	𐤀𐤌 (q - fname >= MAX_PATH)
	    أرجع FALSE;
	wcsncpy(buf, fname, q - fname);
	buf[q - fname] = L'\0';
	attr = GetFileAttributesW(buf);
	𐤀𐤌 (attr != INVALID_FILE_ATTRIBUTES
		&& (attr & FILE_ATTRIBUTE_REPARSE_POINT) != 0)
	    أرجع TRUE;
    }
    أرجع FALSE;
}

/*
 * Return the resolved file path, NULL if "fname" is an AppExecLink reparse
 * point, already fully resolved, or it doesn't exists.
 */
    char_u *
resolve_reparse_point(char_u *fname)
{
    HANDLE	    h = INVALID_HANDLE_VALUE;
    DWORD	    size;
    WCHAR	    *p, *wp;
    char_u	    *rfname = عدم;
    WCHAR	    *buff = عدم;

    p = enc_to_utf16(fname, عدم);
    𐤀𐤌 (p == عدم)
	اذهب fail;

    𐤀𐤌 (!is_reparse_point_included(p))
    {
	vim_free(p);
	اذهب fail;
    }

    h = CreateFileW(p, 0, 0, عدم, OPEN_EXISTING,
	    FILE_FLAG_BACKUP_SEMANTICS, عدم);
    vim_free(p);

    𐤀𐤌 (h == INVALID_HANDLE_VALUE)
	اذهب fail;

    size = GetFinalPathNameByHandleW(h, عدم, 0, 0);
    𐤀𐤌 (size == 0)
	اذهب fail;
    buff = ALLOC_MULT(WCHAR, size);
    𐤀𐤌 (buff == عدم)
	اذهب fail;
    𐤀𐤌 (GetFinalPathNameByHandleW(h, buff, size, 0) == 0)
	اذهب fail;

    𐤀𐤌 (wcsncmp(buff, L"\\\\?\\UNC\\", 8) == 0)
    {
	buff[6] = L'\\';
	wp = buff + 6;
    }
    وإلا 𐤀𐤌 (wcsncmp(buff, L"\\\\?\\", 4) == 0)
	wp = buff + 4;
    وإلا
	wp = buff;

    rfname = utf16_to_enc(wp, عدم);

fail:
    𐤀𐤌 (h != INVALID_HANDLE_VALUE)
	CloseHandle(h);
    𐤀𐤌 (buff != عدم)
	vim_free(buff);

    أرجع rfname;
}

/*
 * When "fname" is the name of a shortcut (*.lnk) resolve the file it points
 * to and return that name in allocated memory.
 * Otherwise NULL is returned.
 */
    ساكن char_u *
resolve_shortcut(char_u *fname)
{
    HRESULT		hr;
    IShellLink		*psl = عدم;
    IPersistFile	*ppf = عدم;
    OLECHAR		wsz[MAX_PATH];
    char_u		*rfname = عدم;
    صحيح			len;
    IShellLinkW		*pslw = عدم;
    WIN32_FIND_DATAW	ffdw; // we get those free of charge

    // Check if the file name ends in ".lnk". Avoid calling
    // CoCreateInstance(), it's quite slow.
    𐤀𐤌 (fname == عدم)
	أرجع rfname;
    len = (صحيح)STRLEN(fname);
    𐤀𐤌 (len <= 4 || STRNICMP(fname + len - 4, ".lnk", 4) != 0)
	أرجع rfname;

    CoInitialize(عدم);

    // create a link manager object and request its interface
    hr = CoCreateInstance(
	    &CLSID_ShellLink, عدم, CLSCTX_INPROC_SERVER,
	    &IID_IShellLinkW, (فراغ**)&pslw);
    𐤀𐤌 (hr == S_OK)
    {
	WCHAR	*p = enc_to_utf16(fname, عدم);

	𐤀𐤌 (p != عدم)
	{
	    // Get a pointer to the IPersistFile interface.
	    hr = pslw->lpVtbl->QueryInterface(
		    pslw, &IID_IPersistFile, (فراغ**)&ppf);
	    𐤀𐤌 (hr != S_OK)
		اذهب shortcut_errorw;

	    // "load" the name and resolve the link
	    hr = ppf->lpVtbl->Load(ppf, p, STGM_READ);
	    𐤀𐤌 (hr != S_OK)
		اذهب shortcut_errorw;
# 𐤀𐤌 0  // This makes Vim wait a long time if the target does not exist.
	    hr = pslw->lpVtbl->Resolve(pslw, عدم, SLR_NO_UI);
	    𐤀𐤌 (hr != S_OK)
		اذهب shortcut_errorw;
# endif

	    // Get the path to the link target.
	    ZeroMemory(wsz, MAX_PATH * حجم(WCHAR));
	    hr = pslw->lpVtbl->GetPath(pslw, wsz, MAX_PATH, &ffdw, 0);
	    𐤀𐤌 (hr == S_OK && wsz[0] != NUL)
		rfname = utf16_to_enc(wsz, عدم);

shortcut_errorw:
	    vim_free(p);
	}
    }

    // Release all interface pointers (both belong to the same object)
    𐤀𐤌 (ppf != عدم)
	ppf->lpVtbl->Release(ppf);
    𐤀𐤌 (psl != عدم)
	psl->lpVtbl->Release(psl);
    𐤀𐤌 (pslw != عدم)
	pslw->lpVtbl->Release(pslw);

    CoUninitialize();
    أرجع rfname;
}

    char_u *
mch_resolve_path(char_u *fname, صحيح reparse_point)
{
    char_u  *path = resolve_shortcut(fname);

    𐤀𐤌 (path == عدم && reparse_point)
	path = resolve_reparse_point(fname);
    أرجع path;
}
#endif

#𐤀𐤌 defined(FEAT_EVAL) && (!defined(FEAT_GUI) || defined(VIMDLL))
/*
 * Bring ourselves to the foreground.  Does work if the OS doesn't allow it.
 */
    فراغ
win32_set_foreground(فراغ)
{
    GetConsoleHwnd();	    // get value of s_hwnd
    𐤀𐤌 (s_hwnd != 0)
	SetForegroundWindow(s_hwnd);
}
#endif

#𐤀𐤌 defined(FEAT_CLIENTSERVER)
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
# define VIM_CLASSNAME_LEN  (حجم(VIM_CLASSNAME) - 1)

// Timeout for sending a message to another Vim instance.  Normally this works
// instantly, but it may hang when the other Vim instance is halted.
# define SENDMESSAGE_TIMEOUT	(5 * 1000)

// Communication is via WM_COPYDATA messages. The message type is sent in
// the dwData parameter. Types are defined here.
# define COPYDATA_KEYS		0
# define COPYDATA_REPLY		1
# define COPYDATA_EXPR		10
# define COPYDATA_RESULT	11
# define COPYDATA_ERROR_RESULT	12
# define COPYDATA_ENCODING	20

// This is a structure containing a server HWND and its name.
بنية server_id
{
    HWND hwnd;
    char_u *name;
};

// Last received 'encoding' that the client uses.
ساكن char_u	*client_enc = عدم;

/*
 * Tell the other side what encoding we are using.
 * Return -1 if timeout happens.  Other errors are ignored.
 */
    ساكن صحيح
serverSendEnc(HWND target)
{
    COPYDATASTRUCT data;

    data.dwData = COPYDATA_ENCODING;
    data.cbData = (DWORD)STRLEN(p_enc) + 1;
    data.lpData = p_enc;
    𐤀𐤌 (SendMessageTimeout(target, WM_COPYDATA,
	    (WPARAM)message_window, (LPARAM)&data,
	    SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, عدم) == 0)
	أرجع -1;
    أرجع 0;
}

/*
 * Clean up on exit. This destroys the hidden message window.
 */
    ساكن فراغ
CleanUpMessaging(فراغ)
{
    𐤀𐤌 (message_window == 0)
	أرجع;

    DestroyWindow(message_window);
    message_window = 0;
}

ساكن صحيح save_reply(HWND server, char_u *reply, صحيح expr);

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
    ساكن LRESULT CALLBACK
Messaging_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    𐤀𐤌 (msg == WM_COPYDATA)
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
	صحيح		retval;
	DWORD_PTR	dwret = 0;
	char_u		*str;
	char_u		*tofree;

	بدل (data->dwData)
	{
	حالة COPYDATA_ENCODING:
	    // Remember the encoding that the client uses.
	    vim_free(client_enc);
	    client_enc = enc_canonize((char_u *)data->lpData);
	    أرجع 1;

	حالة COPYDATA_KEYS:
	    // Remember who sent this, for <client>
	    clientWindow = sender;

	    // Add the received keys to the input buffer.
	    str = serverConvert(client_enc, (char_u *)data->lpData, &tofree);
# ifdef FEAT_GUI
	    𐤀𐤌 (gui.in_use)
		// The GUI may get here re-entrantly, so insert the keys later.
		server_add_input(str);
	    وإلا
# endif
		server_to_input_buf(str);
	    vim_free(tofree);

# ifdef FEAT_GUI
	    // Wake up the main GUI loop.
#  ifdef VIMDLL
	    𐤀𐤌 (gui.in_use)
#  endif
		𐤀𐤌 (s_hwnd != 0)
		    PostMessage(s_hwnd, WM_NULL, 0, 0);
# endif
	    أرجع 1;

	حالة COPYDATA_EXPR:
	    // Remember who sent this, for <client>
	    clientWindow = sender;

	    str = serverConvert(client_enc, (char_u *)data->lpData, &tofree);
	    res = eval_client_expr_to_string(str);

	    𐤀𐤌 (res == عدم)
	    {
		حرف	*err = _(e_invalid_expression_received);
		size_t	len = STRLEN(str) + STRLEN(err) + 5;

		res = alloc(len);
		𐤀𐤌 (res != عدم)
		    reply.cbData = (DWORD)vim_snprintf_safelen(
			(حرف *)res, len, "%s: \"%s\"", err, str);
		وإلا
		    reply.cbData = 0;
		reply.dwData = COPYDATA_ERROR_RESULT;
	    }
	    وإلا
	    {
		reply.cbData = (DWORD)STRLEN(res) + 1;
		reply.dwData = COPYDATA_RESULT;
	    }
	    reply.lpData = res;

	    𐤀𐤌 (serverSendEnc(sender) < 0)
		retval = -1;
	    وإلا
	    {
		𐤀𐤌 (SendMessageTimeout(sender, WM_COPYDATA,
			(WPARAM)message_window, (LPARAM)&reply,
			SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
		    retval = -1;
		وإلا
		    retval = (صحيح)dwret;
	    }
	    vim_free(tofree);
	    vim_free(res);
	    أرجع retval;

	حالة COPYDATA_REPLY:
	حالة COPYDATA_RESULT:
	حالة COPYDATA_ERROR_RESULT:
	    𐤀𐤌 (data->lpData != عدم)
	    {
		str = serverConvert(client_enc, (char_u *)data->lpData,
								     &tofree);
		𐤀𐤌 (tofree == عدم)
		    str = vim_strsave(str);
		𐤀𐤌 (save_reply(sender, str,
			   (data->dwData == COPYDATA_REPLY ?  0 :
			   (data->dwData == COPYDATA_RESULT ? 1 :
							      2))) == FAIL)
		    vim_free(str);
		وإلا 𐤀𐤌 (data->dwData == COPYDATA_REPLY)
		{
		    char_u	winstr[30];

		    sprintf((حرف *)winstr, PRINTF_HEX_LONG_U, (long_u)sender);
		    apply_autocmds(EVENT_REMOTEREPLY, winstr, str,
								TRUE, curbuf);
		}
	    }
	    أرجع 1;
	}

	أرجع 0;
    }

    وإلا 𐤀𐤌 (msg == WM_ACTIVATE && wParam == WA_ACTIVE)
    {
	// When the message window is activated (brought to the foreground),
	// this actually applies to the text window.
# 𐤀𐤌 !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	𐤀𐤌 (!gui.in_use)
#  endif
	    GetConsoleHwnd();	    // get value of s_hwnd
# endif
	𐤀𐤌 (s_hwnd != 0)
	{
	    SetForegroundWindow(s_hwnd);
	    أرجع 0;
	}
    }

    أرجع DefWindowProc(hwnd, msg, wParam, lParam);
}

/*
 * Initialise the message handling process.  This involves creating a window
 * to handle messages - the window will not be visible.
 */
    فراغ
serverInitMessaging(فراغ)
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
    wndclass.hIcon = عدم;
    wndclass.hCursor = عدم;
    wndclass.hbrBackground = عدم;
    wndclass.lpszMenuName = عدم;
    wndclass.lpszClassName = VIM_CLASSNAME;
    RegisterClass(&wndclass);

    // Create the message window. It will be hidden, so the details don't
    // matter.  Don't use WS_OVERLAPPEDWINDOW, it will make a shortcut remove
    // focus from gvim.
    message_window = CreateWindow(VIM_CLASSNAME, "",
			 WS_POPUPWINDOW | WS_CAPTION,
			 CW_USEDEFAULT, CW_USEDEFAULT,
			 100, 100, عدم, عدم,
			 g_hinst, عدم);
}

// Used by serverSendToVim() to find an alternate server name.
ساكن char_u *altname_buf_ptr = عدم;

/*
 * Get the title of the window "hwnd", which is the Vim server name, in
 * "name[namelen]" and return the length.
 * Returns zero if window "hwnd" is not a Vim server.
 */
    ساكن صحيح
getVimServerName(HWND hwnd, حرف *name, صحيح namelen)
{
    صحيح		len;
    حرف	buffer[VIM_CLASSNAME_LEN + 1];

    // Ignore windows which aren't Vim message windows
    len = GetClassName(hwnd, buffer, حجم(buffer));
    𐤀𐤌 (len != VIM_CLASSNAME_LEN || STRCMP(buffer, VIM_CLASSNAME) != 0)
	أرجع 0;

    // Get the title of the window
    أرجع GetWindowText(hwnd, name, namelen);
}

    ساكن BOOL CALLBACK
enumWindowsGetServer(HWND hwnd, LPARAM lparam)
{
    بنية	server_id *id = (بنية server_id *)lparam;
    حرف	server[MAX_PATH];

    // Get the title of the window
    𐤀𐤌 (getVimServerName(hwnd, server, حجم(server)) == 0)
	أرجع TRUE;

    // If this is the server we're looking for, return its HWND
    𐤀𐤌 (STRICMP(server, id->name) == 0)
    {
	id->hwnd = hwnd;
	أرجع FALSE;
    }

    // If we are looking for an alternate server, remember this name.
    𐤀𐤌 (altname_buf_ptr != عدم)
    {
	size_t	namelen = STRLEN(id->name);

	𐤀𐤌 (STRNICMP(server, id->name, namelen) == 0
	    && vim_isdigit(server[namelen]))
	{
	    STRCPY(altname_buf_ptr, server);
	    altname_buf_ptr = عدم;	    // don't use another name
	}
    }

    // Otherwise, keep looking
    أرجع TRUE;
}

    ساكن BOOL CALLBACK
enumWindowsGetNames(HWND hwnd, LPARAM lparam)
{
    list_T	*list = (list_T *)lparam;
    حرف	server[MAX_PATH];

    // Get the title of the window
    𐤀𐤌 (getVimServerName(hwnd, server, حجم(server)) == 0)
	أرجع TRUE;

    list_append_string(list, (char_u *)server, -1);
    أرجع TRUE;
}

بنية enum_windows_s
{
    WNDENUMPROC lpEnumFunc;
    LPARAM      lParam;
};

    ساكن BOOL CALLBACK
enum_windows_child(HWND hwnd, LPARAM lParam)
{
    بنية enum_windows_s *ew = (بنية enum_windows_s *)lParam;

    أرجع (ew->lpEnumFunc)(hwnd, ew->lParam);
}

    ساكن BOOL CALLBACK
enum_windows_toplevel(HWND hwnd, LPARAM lParam)
{
    بنية enum_windows_s *ew = (بنية enum_windows_s *)lParam;

    𐤀𐤌 ((ew->lpEnumFunc)(hwnd, ew->lParam))
	أرجع TRUE;
    أرجع EnumChildWindows(hwnd, enum_windows_child, lParam);
}

/*
 * Enumerate all windows including children.
 */
    ساكن BOOL
enum_windows(WNDENUMPROC lpEnumFunc, LPARAM lParam)
{
    بنية enum_windows_s ew;

    ew.lpEnumFunc = lpEnumFunc;
    ew.lParam = lParam;
    أرجع EnumWindows(enum_windows_toplevel, (LPARAM)&ew);
}

    ساكن HWND
findServer(char_u *name)
{
    بنية server_id id;

    id.name = name;
    id.hwnd = 0;

    enum_windows(enumWindowsGetServer, (LPARAM)(&id));

    أرجع id.hwnd;
}

    فراغ
serverSetName(char_u *name)
{
    size_t	namelen;
    char_u	*ok_name;
    HWND	hwnd = 0;
    صحيح		i = 0;
    char_u	*p;

    // Leave enough space for a 9-digit suffix to ensure uniqueness!
    namelen = STRLEN(name);
    ok_name = alloc(namelen + 10);
    𐤀𐤌 (ok_name == عدم)
	أرجع;

    STRCPY(ok_name, name);
    p = ok_name + namelen;

    لكل (;;)
    {
	// This is inefficient - we're doing an EnumWindows loop for each
	// possible name. It would be better to grab all names in one go,
	// and scan the list each time...
	hwnd = findServer(ok_name);
	𐤀𐤌 (hwnd == 0)
	    اكسر;

	++i;
	𐤀𐤌 (i >= 1000)
	    اكسر;

	sprintf((حرف *)p, "%d", i);
    }

    𐤀𐤌 (hwnd != 0)
	vim_free(ok_name);
    وإلا
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
serverGetVimNames(فراغ)
{
    list_T *list = list_alloc();

    𐤀𐤌 (list == عدم)
	أرجع عدم;

    enum_windows(enumWindowsGetNames, (LPARAM)list);

    أرجع list;
}

    صحيح
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
    sscanf((حرف *)name, SCANF_HEX_LONG_U, &n);
    𐤀𐤌 (n == 0)
	أرجع -1;

    target = (HWND)n;
    𐤀𐤌 (!IsWindow(target))
	أرجع -1;

    data.dwData = COPYDATA_REPLY;
    data.cbData = (DWORD)STRLEN(reply) + 1;
    data.lpData = reply;

    𐤀𐤌 (serverSendEnc(target) < 0)
	أرجع -1;
    𐤀𐤌 (SendMessageTimeout(target, WM_COPYDATA,
		(WPARAM)message_window, (LPARAM)&data,
		SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
	أرجع -1;
    أرجع dwret ? 0 : -1;
}

    صحيح
serverSendToVim(
    char_u	 *name,			// Where to send.
    char_u	 *cmd,			// What to send.
    char_u	 **result,		// Result of eval'ed expression
    فراغ	 *ptarget,		// HWND of server
    صحيح		 asExpr,		// Expression or keys?
    صحيح		 timeout,		// timeout in seconds or zero
    صحيح		 silent)		// don't complain about no server
{
    size_t	namelen;
    HWND	target;
    COPYDATASTRUCT data;
    char_u	*retval = عدم;
    صحيح		retcode = 0;
    DWORD_PTR	dwret = 0;
    char_u	altname_buf[MAX_PATH];

    // Execute locally if no display or target is ourselves
    𐤀𐤌 (serverName != عدم && STRICMP(name, serverName) == 0)
	أرجع sendToLocalVim(cmd, asExpr, result);

    // If the server name does not end in a digit then we look for an
    // alternate name.  e.g. when "name" is GVIM then we may find GVIM2.
    namelen = STRLEN(name);
    𐤀𐤌 (namelen > 1 && !vim_isdigit(name[namelen - 1]))
	altname_buf_ptr = altname_buf;
    altname_buf[0] = NUL;
    target = findServer(name);
    altname_buf_ptr = عدم;
    𐤀𐤌 (target == 0 && altname_buf[0] != NUL)
	// Use another server name we found.
	target = findServer(altname_buf);

    𐤀𐤌 (target == 0)
    {
	𐤀𐤌 (!silent)
	    semsg(_(e_no_registered_server_named_str), name);
	أرجع -1;
    }

    𐤀𐤌 (ptarget)
	*(HWND *)ptarget = target;

    data.dwData = asExpr ? COPYDATA_EXPR : COPYDATA_KEYS;
    data.cbData = (DWORD)STRLEN(cmd) + 1;
    data.lpData = cmd;

    𐤀𐤌 (serverSendEnc(target) < 0)
	أرجع -1;
    𐤀𐤌 (SendMessageTimeout(target, WM_COPYDATA,
		(WPARAM)message_window, (LPARAM)&data,
		SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
	أرجع -1;
    𐤀𐤌 (dwret == 0)
	أرجع -1;

    𐤀𐤌 (asExpr)
	retval = serverGetReply(target, &retcode, TRUE, TRUE, timeout);

    𐤀𐤌 (result == عدم)
	vim_free(retval);
    وإلا
	*result = retval; // Caller assumes responsibility for freeing

    أرجع retcode;
}

/*
 * Bring the server to the foreground.
 */
    فراغ
serverForeground(char_u *name)
{
    HWND	target = findServer(name);

    𐤀𐤌 (target != 0)
	SetForegroundWindow(target);
}

// Replies from server need to be stored until the client picks them up via
// remote_read(). So we maintain a list of server-id/reply pairs.
// Note that there could be multiple replies from one server pending if the
// client is slow picking them up.
// We just store the replies in a simple list. When we remove an entry, we
// move list entries down to fill the gap.
// The server ID is simply the HWND.
تعريفنوع بنية
{
    HWND	server;		// server window
    char_u	*reply;		// reply string
    صحيح		expr_result;	// 0 for REPLY, 1 for RESULT 2 for error
} reply_T;

ساكن garray_T reply_list = {0, 0, حجم(reply_T), 5, 0};

# define REPLY_ITEM(i) ((reply_T *)(reply_list.ga_data) + (i))
# define REPLY_COUNT (reply_list.ga_len)

// Flag which is used to wait for a reply
ساكن صحيح reply_received = 0;

/*
 * Store a reply.  "reply" must be allocated memory (or NULL).
 */
    ساكن صحيح
save_reply(HWND server, char_u *reply, صحيح expr)
{
    reply_T *rep;

    𐤀𐤌 (ga_grow(&reply_list, 1) == FAIL)
	أرجع FAIL;

    rep = REPLY_ITEM(REPLY_COUNT);
    rep->server = server;
    rep->reply = reply;
    rep->expr_result = expr;
    𐤀𐤌 (rep->reply == عدم)
	أرجع FAIL;

    ++REPLY_COUNT;
    reply_received = 1;
    أرجع OK;
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
serverGetReply(HWND server, صحيح *expr_res, صحيح remove, صحيح wait, صحيح timeout)
{
    صحيح		i;
    char_u	*reply;
    reply_T	*rep;
    صحيح		did_process = FALSE;
    time_t	start;
    time_t	now;

    // When waiting, loop until the message waiting for is received.
    time(&start);
    لكل (;;)
    {
	// Reset this here, in case a message arrives while we are going
	// through the already received messages.
	reply_received = 0;

	لكل (i = 0; i < REPLY_COUNT; ++i)
	{
	    rep = REPLY_ITEM(i);
	    𐤀𐤌 (rep->server == server
		    && ((rep->expr_result != 0) == (expr_res != عدم)))
	    {
		// Save the values we've found for later
		reply = rep->reply;
		𐤀𐤌 (expr_res != عدم)
		    *expr_res = rep->expr_result == 1 ? 0 : -1;

		𐤀𐤌 (remove)
		{
		    // Move the rest of the list down to fill the gap
		    mch_memmove(rep, rep + 1,
				     (REPLY_COUNT - i - 1) * حجم(reply_T));
		    --REPLY_COUNT;
		}

		// Return the reply to the caller, who takes on responsibility
		// for freeing it if "remove" is TRUE.
		أرجع reply;
	    }
	}

	// If we got here, we didn't find a reply. Return immediately if the
	// "wait" parameter isn't set.
	𐤀𐤌 (!wait)
	{
	    // Process pending messages once. Without this, looping on
	    // remote_peek() would never get the reply.
	    𐤀𐤌 (!did_process)
	    {
		did_process = TRUE;
		serverProcessPendingMessages();
		استمر;
	    }
	    اكسر;
	}

	// We need to wait for a reply. Enter a message loop until the
	// "reply_received" flag gets set.

	// Loop until we receive a reply
	طالما (reply_received == 0)
	{
# ifdef FEAT_TIMERS
	    // TODO: use the return value to decide how long to wait.
	    check_due_timer();
# endif
	    time(&now);
	    𐤀𐤌 (timeout > 0 && (now - start) >= timeout)
		اكسر;

	    // Wait for a SendMessage() call to us.  This could be the reply
	    // we are waiting for.  Use a timeout of a second, to catch the
	    // situation that the server died unexpectedly.
	    MsgWaitForMultipleObjects(0, عدم, TRUE, 1000, QS_ALLINPUT);

	    // If the server has died, give up
	    𐤀𐤌 (!IsWindow(server))
		أرجع عدم;

	    serverProcessPendingMessages();
	}
    }

    أرجع عدم;
}

/*
 * Process any messages in the Windows message queue.
 */
    فراغ
serverProcessPendingMessages(فراغ)
{
    MSG msg;

    طالما (PeekMessageW(&msg, عدم, 0, 0, PM_REMOVE))
    {
	TranslateMessage(&msg);
	DispatchMessageW(&msg);
    }
}

#endif // FEAT_CLIENTSERVER

#𐤀𐤌 defined(FEAT_GUI) || (defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT))

بنية charset_pair
{
    string_T	name;
    BYTE	charset;
};

ساكن بنية charset_pair
charset_pairs[] =
{
    {STR_LITERAL_INIT("ANSI"),		ANSI_CHARSET},
    {STR_LITERAL_INIT("CHINESEBIG5"),	CHINESEBIG5_CHARSET},
    {STR_LITERAL_INIT("DEFAULT"),	DEFAULT_CHARSET},
    {STR_LITERAL_INIT("HANGEUL"),	HANGEUL_CHARSET},
    {STR_LITERAL_INIT("OEM"),		OEM_CHARSET},
    {STR_LITERAL_INIT("SHIFTJIS"),	SHIFTJIS_CHARSET},
    {STR_LITERAL_INIT("SYMBOL"),	SYMBOL_CHARSET},
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

بنية quality_pair
{
    string_T	name;
    DWORD	quality;
};

ساكن بنية quality_pair
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
    حرف *
charset_id2name(صحيح id)
{
    صحيح	i;

    لكل (i = 0; i < (صحيح)ARRAY_LENGTH(charset_pairs); ++i)
    {
	𐤀𐤌 ((BYTE)id == charset_pairs[i].charset)
	    أرجع (حرف *)charset_pairs[i].name.string;
    }

    أرجع عدم;
}

/*
 * Convert a quality ID to a name.
 * Return NULL when not recognized.
 */
    حرف *
quality_id2name(DWORD id)
{
    صحيح	i;

    لكل (i = 0; i < (صحيح)ARRAY_LENGTH(quality_pairs); ++i)
    {
	𐤀𐤌 (id == quality_pairs[i].quality)
	    أرجع (حرف *)quality_pairs[i].name.string;
    }

    أرجع عدم;
}

// The default font height in 100% scaling (96dpi).
// (-16 in 96dpi equates to roughly 12pt)
# define DEFAULT_FONT_HEIGHT	(-16)

ساكن ثابت LOGFONTW s_lfDefault =
{
    DEFAULT_FONT_HEIGHT,
    0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
    PROOF_QUALITY, FIXED_PITCH | FF_DONTCARE,
    L""	// Default font name will be set later based on current language.
};

// This will be initialized when set_default_logfont() is called first time.
// The value will be based on the system DPI.
صحيح current_font_height = 0;		// also used in gui_w32.c

/*
 * Convert a string representing a point size into pixels. The string should
 * be a positive decimal number, with an optional decimal point (eg, "12", or
 * "10.5"). The pixel value is returned, and a pointer to the next unconverted
 * character is stored in *end. The flag "vertical" says whether this
 * calculation is for a vertical (height) size or a horizontal (width) one.
 */
    ساكن صحيح
points_to_pixels(WCHAR *str, WCHAR **end, صحيح vertical, long_i pprinter_dc)
{
    صحيح		pixels;
    صحيح		points = 0;
    صحيح		divisor = 0;
    HWND	hwnd = (HWND)0;
    HDC		hdc;
    HDC		printer_dc = (HDC)pprinter_dc;

    طالما (*str != NUL)
    {
	𐤀𐤌 (*str == L'.' && divisor == 0)
	{
	    // Start keeping a divisor, for later
	    divisor = 1;
	}
	وإلا
	{
	    𐤀𐤌 (!VIM_ISDIGIT(*str))
		اكسر;

	    points *= 10;
	    points += *str - L'0';
	    divisor *= 10;
	}
	++str;
    }

    𐤀𐤌 (divisor == 0)
	divisor = 1;

    𐤀𐤌 (printer_dc == عدم)
    {
	hwnd = GetDesktopWindow();
	hdc = GetWindowDC(hwnd);
    }
    وإلا
	hdc = printer_dc;

    pixels = MulDiv(points,
		    GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX),
		    72 * divisor);

    𐤀𐤌 (printer_dc == عدم)
	ReleaseDC(hwnd, hdc);

    *end = str;
    أرجع pixels;
}

/*
 * Convert pixel into point size. This is a reverse of points_to_pixels.
 */
    ساكن مزدوج
pixels_to_points(صحيح pixels, صحيح vertical, long_i pprinter_dc)
{
    مزدوج	points = 0;
    HWND	hwnd = (HWND)0;
    HDC		hdc;
    HDC		printer_dc = (HDC)pprinter_dc;

    𐤀𐤌 (printer_dc == عدم)
    {
	hwnd = GetDesktopWindow();
	hdc = GetWindowDC(hwnd);
    }
    وإلا
	hdc = printer_dc;

    points = pixels * 72.0 / GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX);
    𐤀𐤌 (printer_dc == عدم)
	ReleaseDC(hwnd, hdc);

    أرجع points;
}

    ساكن صحيح CALLBACK
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
    𐤀𐤌 ((ntm->tmPitchAndFamily & 1) != 0)
	أرجع 1;
# endif

    // Remember this LOGFONTW as a "possible"
    *lf = elf->elfLogFont;

    // Terminate the scan as soon as we find an ANSI font
    𐤀𐤌 (lf->lfCharSet == ANSI_CHARSET
	    || lf->lfCharSet == OEM_CHARSET
	    || lf->lfCharSet == DEFAULT_CHARSET)
	أرجع 0;

    // Continue the scan - we have a non-ANSI font
    أرجع 2;
}

    ساكن صحيح
init_logfont(LOGFONTW *lf)
{
    صحيح		n;
    HWND	hwnd = GetDesktopWindow();
    HDC		hdc = GetWindowDC(hwnd);

    n = EnumFontFamiliesW(hdc,
			 lf->lfFaceName,
			 (FONTENUMPROCW)font_enumproc,
			 (LPARAM)lf);

    ReleaseDC(hwnd, hdc);

    // If we couldn't find a usable font, return failure
    𐤀𐤌 (n == 1)
	أرجع FAIL;

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
    أرجع OK;
}

/*
 * Call back for EnumFontFamiliesW in expand_font_enumproc.
 *
 */
    ساكن صحيح CALLBACK
expand_font_enumproc(
    ENUMLOGFONTW    *elf,
    NEWTEXTMETRICW  *ntm UNUSED,
    DWORD	    type UNUSED,
    LPARAM	    lparam)
{
    LOGFONTW *lf = (LOGFONTW*)elf;

# ifndef FEAT_PROPORTIONAL_FONTS
    // Ignore non-monospace fonts without further ado
    𐤀𐤌 ((ntm->tmPitchAndFamily & 1) != 0)
	أرجع 1;
# endif

    // Filter only on ANSI. Otherwise will see a lot of random fonts that we
    // usually don't want.
    𐤀𐤌 (lf->lfCharSet != ANSI_CHARSET)
	أرجع 1;

    صحيح (*add_match)(char_u *) = (صحيح (*)(char_u *))lparam;

    WCHAR *faceNameW = lf->lfFaceName;
    char_u *faceName = utf16_to_enc(faceNameW, عدم);
    𐤀𐤌 (!faceName)
	أرجع 0;

    add_match(faceName);
    vim_free(faceName);

    أرجع 1;
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
    فراغ
gui_mch_expand_font(
    optexpand_T	*args,
    فراغ	*param UNUSED,
    صحيح		(*add_match)(char_u *val))
{
    expand_T	    *xp = args->oe_xp;
    𐤀𐤌 (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	حرف buf[30];
	صحيح	i;

	// Always fill in with the current font size as first option for
	// convenience. We simply round to the closest integer for simplicity.
	صحيح font_height = (صحيح)round(
		pixels_to_points(-current_font_height, TRUE, (long_i)عدم));
	vim_snprintf(buf, ARRAY_LENGTH(buf), "h%d", font_height);
	add_match((char_u *)buf);

	// Note: Keep this in sync with get_logfont(). Don't include 'c' and
	// 'q' as we fill in all the values below.
	ساكن حرف *(p_gfn_win_opt_values[]) = {
	    "h" , "w" , "W" , "b" , "i" , "u" , "s"};
	لكل (i = 0; i < (صحيح)ARRAY_LENGTH(p_gfn_win_opt_values); i++)
	    add_match((char_u *)p_gfn_win_opt_values[i]);

	لكل (i = 0; i < (صحيح)ARRAY_LENGTH(charset_pairs); ++i)
	{
	    vim_snprintf(buf, حجم(buf), "c%s", charset_pairs[i].name.string);
	    add_match((char_u *)buf);
	}

	لكل (i = 0; i < (صحيح)ARRAY_LENGTH(quality_pairs); ++i)
	{
	    vim_snprintf(buf, حجم(buf), "q%s", quality_pairs[i].name.string);
	    add_match((char_u *)buf);
	}
	أرجع;
    }

    HWND	hwnd = GetDesktopWindow();
    HDC		hdc = GetWindowDC(hwnd);

    EnumFontFamiliesW(hdc,
	    عدم,
	    (FONTENUMPROCW)expand_font_enumproc,
	    (LPARAM)add_match);

    ReleaseDC(hwnd, hdc);
}

/*
 * Compare a UTF-16 string and an ASCII string literally.
 * Only works all the code points are inside ASCII range.
 */
    ساكن صحيح
utf16ascncmp(ثابت WCHAR *w, ثابت حرف *p, size_t n)
{
    size_t i;

    لكل (i = 0; i < n; i++)
    {
	𐤀𐤌 (w[i] == 0 || w[i] != p[i])
	    أرجع w[i] - p[i];
    }
    أرجع 0;
}

/*
 * Equivalent of GetDpiForSystem().
 */
    UINT WINAPI
vimGetDpiForSystem(فراغ)
{
    HWND hwnd = GetDesktopWindow();
    HDC hdc = GetWindowDC(hwnd);
    UINT dpi = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(hwnd, hdc);
    أرجع dpi;
}

/*
 * Set default logfont based on current language.
 */
    ساكن فراغ
set_default_logfont(LOGFONTW *lf)
{
    // Default font name for current language on MS-Windows.
    // If not translated, falls back to "Consolas".
    // This must be a fixed-pitch font.
    ثابت حرف *defaultfontname = N_("DefaultFontNameForWindows");
    حرف *fontname = _(defaultfontname);

    𐤀𐤌 (STRCMP(fontname, defaultfontname) == 0)
	fontname = "Consolas";

    *lf = s_lfDefault;
    lf->lfHeight = DEFAULT_FONT_HEIGHT * (صحيح)vimGetDpiForSystem() / 96;
    𐤀𐤌 (current_font_height == 0)
	current_font_height = lf->lfHeight;

    WCHAR *wfontname = enc_to_utf16((char_u*)fontname, عدم);
    𐤀𐤌 (wfontname != عدم)
    {
	wcscpy_s(lf->lfFaceName, LF_FACESIZE, wfontname);
	vim_free(wfontname);
    }
}

/*
 * Get font info from "name" into logfont "lf".
 * Return OK for a valid name, FAIL otherwise.
 */
    صحيح
get_logfont(
    LOGFONTW	*lf,
    char_u	*name,
    HDC		printer_dc,
    صحيح		verbose)
{
    WCHAR	*p;
    صحيح		i;
    صحيح		ret = FAIL;
    ساكن LOGFONTW *lastlf = عدم;
    WCHAR	*wname;

    set_default_logfont(lf);
    𐤀𐤌 (name == عدم)
	أرجع OK;

    wname = enc_to_utf16(name, عدم);
    𐤀𐤌 (wname == عدم)
	أرجع FAIL;

    𐤀𐤌 (wcscmp(wname, L"*") == 0)
    {
# 𐤀𐤌 defined(FEAT_GUI_MSWIN)
	CHOOSEFONTW	cf;
	// if name is "*", bring up std font dialog:
	CLEAR_FIELD(cf);
	cf.lStructSize = حجم(cf);
	cf.hwndOwner = s_hwnd;
	cf.Flags = CF_SCREENFONTS | CF_FIXEDPITCHONLY | CF_INITTOLOGFONTSTRUCT;
	𐤀𐤌 (lastlf != عدم)
	    *lf = *lastlf;
	cf.lpLogFont = lf;
	cf.nFontType = 0 ; //REGULAR_FONTTYPE;
	𐤀𐤌 (ChooseFontW(&cf))
	    ret = OK;
# endif
	اذهب theend;
    }

    /*
     * Split name up, it could be <name>:h<height>:w<width> etc.
     */
    لكل (p = wname; *p && *p != L':'; p++)
    {
	𐤀𐤌 (p - wname + 1 >= LF_FACESIZE)
	    اذهب theend;			// Name too long
	lf->lfFaceName[p - wname] = *p;
    }
    𐤀𐤌 (p != wname)
	lf->lfFaceName[p - wname] = NUL;

    // First set defaults
    lf->lfHeight = DEFAULT_FONT_HEIGHT * (صحيح)vimGetDpiForSystem() / 96;
    lf->lfWidth = 0;
    lf->lfWeight = FW_NORMAL;
    lf->lfItalic = FALSE;
    lf->lfUnderline = FALSE;
    lf->lfStrikeOut = FALSE;

    /*
     * If the font can't be found, try replacing '_' by ' '.
     */
    𐤀𐤌 (init_logfont(lf) == FAIL)
    {
	صحيح	did_replace = FALSE;

	لكل (i = 0; lf->lfFaceName[i]; ++i)
	    𐤀𐤌 (lf->lfFaceName[i] == L'_')
	    {
		lf->lfFaceName[i] = L' ';
		did_replace = TRUE;
	    }
	𐤀𐤌 (!did_replace || init_logfont(lf) == FAIL)
	    اذهب theend;
    }

    طالما (*p == L':')
	p++;

    // Set the values found after ':'
    طالما (*p)
    {
	بدل (*p++)
	{
	    // Note: Keep this in sync with gui_mch_expand_font().
	    حالة L'h':
		lf->lfHeight = - points_to_pixels(p, &p, TRUE, (long_i)printer_dc);
		اكسر;
	    حالة L'w':
		lf->lfWidth = points_to_pixels(p, &p, FALSE, (long_i)printer_dc);
		اكسر;
	    حالة L'W':
		lf->lfWeight = wcstol(p, &p, 10);
		اكسر;
	    حالة L'b':
		lf->lfWeight = FW_BOLD;
		اكسر;
	    حالة L'i':
		lf->lfItalic = TRUE;
		اكسر;
	    حالة L'u':
		lf->lfUnderline = TRUE;
		اكسر;
	    حالة L's':
		lf->lfStrikeOut = TRUE;
		اكسر;
	    حالة L'c':
		لكل (i = 0; i < (صحيح)ARRAY_LENGTH(charset_pairs); ++i)
		{
		    𐤀𐤌 (utf16ascncmp(p, (حرف *)charset_pairs[i].name.string,
			    charset_pairs[i].name.length) == 0)
		    {
			lf->lfCharSet = charset_pairs[i].charset;
			p += charset_pairs[i].name.length;
			اكسر;
		    }
		}

		𐤀𐤌 (i == (صحيح)ARRAY_LENGTH(charset_pairs) && verbose)
		{
		    char_u *s = utf16_to_enc(p, عدم);
		    𐤀𐤌 (s != عدم)
		    {
			semsg(_(e_illegal_str_name_str_in_font_name_str),
							   "charset", s, name);
			vim_free(s);
		    }
		}
		اكسر;
	    حالة L'f':
		// Font features (e.g., "fss19=1").
		// Parsed separately by gui_mch_init_font(); skip here.
		طالما (*p && *p != L':')
		    p++;
		اكسر;
	    حالة L'q':
		لكل (i = 0; i < (صحيح)ARRAY_LENGTH(quality_pairs); ++i)
		{
		    𐤀𐤌 (utf16ascncmp(p, (حرف *)quality_pairs[i].name.string,
			    quality_pairs[i].name.length) == 0)
		    {
			lf->lfQuality = quality_pairs[i].quality;
			p += quality_pairs[i].name.length;
			اكسر;
		    }
		}

		𐤀𐤌 (i == (صحيح)ARRAY_LENGTH(quality_pairs) && verbose)
		{
		    char_u *s = utf16_to_enc(p, عدم);
		    𐤀𐤌 (s != عدم)
		    {
			semsg(_(e_illegal_str_name_str_in_font_name_str),
							   "quality", s, name);
			vim_free(s);
		    }
		}
		اكسر;
	    افتراضي:
		𐤀𐤌 (verbose)
		    semsg(_(e_illegal_char_nr_in_font_name_str), p[-1], name);
		اذهب theend;
	}
	طالما (*p == L':')
	    p++;
    }
    ret = OK;

theend:
    // ron: init lastlf
    𐤀𐤌 (ret == OK && printer_dc == عدم)
    {
	vim_free(lastlf);
	lastlf = ALLOC_ONE(LOGFONTW);
	𐤀𐤌 (lastlf != عدم)
	    mch_memmove(lastlf, lf, حجم(LOGFONTW));
    }
    vim_free(wname);

    أرجع ret;
}

#endif // defined(FEAT_GUI) || defined(FEAT_PRINTER)

#𐤀𐤌 defined(FEAT_JOB_CHANNEL)
/*
 * Initialize the Winsock dll.
 */
    فراغ
channel_init_winsock(فراغ)
{
    WSADATA wsaData;
    صحيح wsaerr;

    𐤀𐤌 (WSInitialized)
	أرجع;

    wsaerr = WSAStartup(MAKEWORD(2, 2), &wsaData);
    𐤀𐤌 (wsaerr == 0)
	WSInitialized = TRUE;
}
#endif
