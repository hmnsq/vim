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

#अन्तर्भाव "vim.h"

#अन्तर्भाव <sys/types.h>
#अन्तर्भाव <limits.h>

#अन्तर्भाव <process.h>
#अन्तर्भाव <direct.h>

#सचे !defined(FEAT_GUI_MSWIN)
# include <shellapi.h>
#endif

#सचे defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT)
# include <dlgs.h>
# include <winspool.h>
# include <commdlg.h>
#endif

// Record all output and all keyboard & mouse input
// #define MCH_WRITE_DUMP

#ifdef MCH_WRITE_DUMP
FILE* fdDump = रिक्त;
#endif

#सचे !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
बाह्य वर्ण g_szOrigTitle[];
#endif

#ifdef FEAT_GUI
बाह्य HWND s_hwnd;
#अञ्ञथा
स्थिर HWND s_hwnd = 0;	    // console window handle, set by GetConsoleHwnd()
#endif

#ifdef FEAT_JOB_CHANNEL
पूर्णाङ्क WSInitialized = FALSE; // WinSock is initialized
#endif


/*
 * Save the instance handle of the exe/dll.
 */
    शून्य
SaveInst(HINSTANCE hInst)
{
    g_hinst = hInst;
}

#सचे defined(FEAT_GUI_MSWIN)
/*
 * GUI version of mch_exit().
 * Shut down and exit with status `r'
 * Careful: mch_exit() may be called before mch_init()!
 */
    शून्य
mch_exit_g(पूर्णाङ्क r)
{
    exiting = TRUE;

    display_errors();

    ml_close_all(TRUE);		// remove all memfiles

# ifdef FEAT_OLE
    UninitOLE();
# endif
# ifdef FEAT_JOB_CHANNEL
    सचे (WSInitialized)
    {
	WSInitialized = FALSE;
	WSACleanup();
    }
# endif
# ifdef DYNAMIC_GETTEXT
    dyn_libintl_end();
# endif

    सचे (gui.in_use)
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
प्रकारनिर्देश BOOL (WINAPI *PfnRtlGetVersion)(LPOSVERSIONINFOW);
DWORD win_version;
    स्थिर शून्य
win_version_init(शून्य)
{
    OSVERSIONINFOW	osver;
    HMODULE		hNtdll;
    PfnRtlGetVersion	pRtlGetVersion;

    hNtdll = GetModuleHandle("ntdll.dll");
    सचे (hNtdll == रिक्त)
	पटिनिवत्त;

    pRtlGetVersion =
	(PfnRtlGetVersion) GetProcAddress(hNtdll, "RtlGetVersion");
    सचे (pRtlGetVersion == रिक्त)
	पटिनिवत्त;

    osver.dwOSVersionInfoSize = परिमाण(OSVERSIONINFOW);
    pRtlGetVersion(&osver);
    win_version = MAKE_VER(min(osver.dwMajorVersion, 0xFF),
	    min(osver.dwMinorVersion, 0xFF),
	    min(osver.dw_Build_सङ्ख्या, 0xFFFF));
}

/*
 * Init the tables for toupper() and tolower().
 */
    शून्य
mch_early_init(शून्य)
{
    पूर्णाङ्क		i;

    win_version_init();

    PlatformId();

    // Init the tables for toupper() and tolower()
    अत्थाय (i = 0; i < 256; ++i)
	toupper_tab[i] = tolower_tab[i] = i;
    CharUpperBuff((LPSTR)toupper_tab, 256);
    CharLowerBuff((LPSTR)tolower_tab, 256);
}


/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    पूर्णाङ्क
mch_निवेशः_isatty(शून्य)
{
#ifdef FEAT_GUI_MSWIN
# ifdef VIMDLL
    सचे (gui.in_use)
# endif
	पटिनिवत्त TRUE;	    // GUI always has a tty
#endif
#सचे !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    सचे (isatty(read_cmd_fd))
	पटिनिवत्त TRUE;
    पटिनिवत्त FALSE;
#endif
}

/*
 * mch_settitle(): set titlebar of our window
 */
    शून्य
mch_settitle(
    char_u *title,
    char_u *icon UNUSED)
{
#ifdef FEAT_GUI_MSWIN
# ifdef VIMDLL
    सचे (gui.in_use)
# endif
    {
	gui_mch_settitle(title, icon);
	पटिनिवत्त;
    }
#endif
#सचे !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    सचे (title != रिक्त)
    {
	WCHAR	*wp = enc_to_utf16(title, रिक्त);

	सचे (wp == रिक्त)
	    पटिनिवत्त;

	SetConsoleTitleW(wp);
	vim_free(wp);
	पटिनिवत्त;
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
    शून्य
mch_restore_title(पूर्णाङ्क which UNUSED)
{
#सचे !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
    सचे (!gui.in_use)
# endif
	SetConsoleTitle(g_szOrigTitle);
#endif
}


/*
 * Return TRUE if we can restore the title (we can)
 */
    पूर्णाङ्क
mch_can_restore_title(शून्य)
{
    पटिनिवत्त TRUE;
}


/*
 * Return TRUE if we can restore the icon title (we can't)
 */
    पूर्णाङ्क
mch_can_restore_icon(शून्य)
{
    पटिनिवत्त FALSE;
}


/*
 * Get absolute file name into buffer "buf" of length "len" bytes,
 * turning all '/'s into '\\'s and getting the correct case of each component
 * of the file name.  Append a (back)slash to a directory name.
 * When 'shellslash' set do it the other way around.
 * Return OK or FAIL.
 */
    पूर्णाङ्क
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    पूर्णाङ्क		len,
    पूर्णाङ्क		force UNUSED)
{
    पूर्णाङ्क		nResult = FAIL;
    WCHAR	*wname;
    WCHAR	wbuf[MAX_PATH];
    char_u	*cname = रिक्त;

    wname = enc_to_utf16(fname, रिक्त);
    सचे (wname != रिक्त && _wfullpath(wbuf, wname, MAX_PATH) != रिक्त)
    {
	cname = utf16_to_enc((short_u *)wbuf, रिक्त);
	सचे (cname != रिक्त)
	{
	    vim_strncpy(buf, cname, len - 1);
	    nResult = OK;
	}
    }
    vim_free(wname);
    vim_free(cname);

#ifdef USE_FNAME_CASE
    fname_case(buf, len);
#अञ्ञथा
    slash_adjust(buf);
#endif

    पटिनिवत्त nResult;
}


/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    पूर्णाङ्क
mch_isFullName(char_u *fname)
{
    // A name like "d:/foo" and "//server/share" is absolute.  "d:foo" is not.
    // /foo and \foo are absolute too because Windows keeps a current drive.
    // Another way to check is to use mch_FullName() and see if the result is
    // the same as the name or mch_FullName() fails.  However, this has quite a
    // bit of overhead, so let's not do that.
    सचे (*fname == NUL)
	पटिनिवत्त FALSE;
    पटिनिवत्त ((ASCII_ISALPHA(fname[0]) && fname[1] == ':'
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
    शून्य
slash_adjust(char_u *p)
{
    सचे (path_with_url(p))
	पटिनिवत्त;

    सचे (*p == '`')
    {
	size_t len = STRLEN(p);

	// don't replace backslash in backtick quoted strings
	सचे (len > 2 && *(p + len - 1) == '`')
	    पटिनिवत्त;
    }

    याव (*p)
    {
	सचे (*p == psepcN)
	    *p = psepc;
	MB_PTR_ADV(p);
    }
}

    स्थिर पूर्णाङ्क
read_reparse_point(ध्रुव WCHAR *name, char_u *buf, DWORD *buf_len)
{
    HANDLE h;
    BOOL ok;

    h = CreateFileW(name, FILE_READ_ATTRIBUTES,
	    FILE_SHARE_READ | FILE_SHARE_WRITE, रिक्त,
	    OPEN_EXISTING,
	    FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,
	    रिक्त);
    सचे (h == INVALID_HANDLE_मानम्)
	पटिनिवत्त FAIL;

    ok = DeviceIoControl(h, FSCTL_GET_REPARSE_POINT, रिक्त, 0, buf, *buf_len,
	    buf_len, रिक्त);
    CloseHandle(h);

    पटिनिवत्त ok ? OK : FAIL;
}

    char_u *
resolve_appexeclink(char_u *fname)
{
    DWORD		attr = 0;
    पूर्णाङ्क			idx;
    WCHAR		*p, *end, *wname;
    // The buffer size is arbitrarily chosen to be "big enough" (TM), the
    // ceiling should be around 16k.
    char_u		buf[4096];
    DWORD		buf_len = परिमाण(buf);
    REPARSE_DATA_BUFFER *rb = (REPARSE_DATA_BUFFER *)buf;

    wname = enc_to_utf16(fname, रिक्त);
    सचे (wname == रिक्त)
	पटिनिवत्त रिक्त;

    attr = GetFileAttributesW(wname);
    सचे (attr == INVALID_FILE_ATTRIBUTES ||
	    (attr & FILE_ATTRIBUTE_REPARSE_POINT) == 0)
    {
	vim_free(wname);
	पटिनिवत्त रिक्त;
    }

    // The applinks are similar to symlinks but with a huge difference: they can
    // only be executed, any other I/O operation on them is bound to fail with
    // ERROR_FILE_NOT_FOUND even though the file exists.
    सचे (read_reparse_point(wname, buf, &buf_len) == FAIL)
    {
	vim_free(wname);
	पटिनिवत्त रिक्त;
    }
    vim_free(wname);

    सचे (rb->ReparseTag != IO_REPARSE_TAG_APPEXECLINK)
	पटिनिवत्त रिक्त;

    // The (undocumented) reparse buffer contains a set of N null-terminated
    // Unicode strings, the application path is stored in the third one.
    सचे (rb->AppExecLinkReparseBuffer.String_गणनम् < 3)
	पटिनिवत्त रिक्त;

    p = rb->AppExecLinkReparseBuffer.StringList;
    end = p + rb->ReparseDataLength / परिमाण(WCHAR);
    अत्थाय (idx = 0; p < end
	    && idx < (पूर्णाङ्क)rb->AppExecLinkReparseBuffer.String_गणनम्
	    && idx != 2; )
    {
	सचे (*p++ == L'\0')
	    ++idx;
    }

    पटिनिवत्त utf16_to_enc(p, रिक्त);
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
    स्थिर पूर्णाङ्क
mswin_stat_impl(ध्रुव WCHAR *name, stat_T *stp, ध्रुव पूर्णाङ्क resolve)
{
    पूर्णाङ्क			n;
    पूर्णाङ्क			fd;
    BOOL		is_symlink = FALSE;
    HANDLE		hFind, h;
    DWORD		attr = 0;
    DWORD		flag = 0;
    WIN32_FIND_DATAW    findDataW;

#सचे 0 && defined(_UCRT)
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
    सचे (resolve)
	// Universal CRT can handle symlinks properly.
	पटिनिवत्त _wstat(name, stp);
#endif

    // Only reparse points need a directory query to identify symbolic links.
    // In particular, avoid enumerating the directory for each missing file
    // while searching 'runtimepath'.
    attr = GetFileAttributesW(name);
    सचे (attr == INVALID_FILE_ATTRIBUTES)
    {
	DWORD error = GetLastError();

	// A missing file or path cannot provide stat information.  Other
	// errors and UNC paths still need the CRT fallback.
	सचे ((error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND)
		&& !((name[0] == L'\\' || name[0] == L'/')
		    && (name[1] == L'\\' || name[1] == L'/')))
	{
	    errno = ENOENT;
	    पटिनिवत्त -1;
	}
	पटिनिवत्त _wstat(name, stp);
    }
    सचे (!(attr & FILE_ATTRIBUTE_REPARSE_POINT))
	पटिनिवत्त _wstat(name, stp);

    hFind = FindFirstFileW(name, &findDataW);
    सचे (hFind != INVALID_HANDLE_मानम्)
    {
	attr = findDataW.dwFileAttributes;
	सचे ((attr & FILE_ATTRIBUTE_REPARSE_POINT)
		&& (findDataW.dwReserved0 == IO_REPARSE_TAG_SYMLINK))
	    is_symlink = TRUE;
	FindClose(hFind);
    }

    // Use the plain old stat() whenever it's possible.
    सचे (!is_symlink)
	पटिनिवत्त _wstat(name, stp);

    सचे (!resolve && is_symlink)
	flag = FILE_FLAG_OPEN_REPARSE_POINT;
    सचे (attr & FILE_ATTRIBUTE_DIRECTORY)
	flag |= FILE_FLAG_BACKUP_SEMANTICS;

    h = CreateFileW(name, FILE_READ_ATTRIBUTES,
	    FILE_SHARE_READ | FILE_SHARE_WRITE, रिक्त, OPEN_EXISTING, flag,
	    रिक्त);
    सचे (h == INVALID_HANDLE_मानम्)
	पटिनिवत्त -1;

    fd = _open_osfhandle((intptr_t)h, _O_RDONLY);
    n = _fstat(fd, (संरचना _stat *)stp);
    सचे ((n == 0) && (attr & FILE_ATTRIBUTE_DIRECTORY))
	stp->st_mode = (stp->st_mode & ~S_IFMT) | S_IFDIR;
    _close(fd);

    पटिनिवत्त n;
}

/*
 * stat() can't handle a trailing '/' or '\', remove it first.
 * When 'resolve' is true behave as lstat() wrt symlinks.
 */
    स्थिर पूर्णाङ्क
stat_impl(ध्रुव वर्ण *name, stat_T *stp, ध्रुव पूर्णाङ्क resolve)
{
    // WinNT and later can use _MAX_PATH wide characters for a pathname, which
    // means that the maximum pathname is _MAX_PATH * 3 bytes when 'enc' is
    // UTF-8.
    char_u	buf[_MAX_PATH * 3 + 1];
    size_t	buflen;
    char_u	*p;
    WCHAR	*wp;
    पूर्णाङ्क		n;

    vim_strncpy((char_u *)buf, (char_u *)name, परिमाण(buf) - 1);
    buflen = STRLEN(buf);
    p = buf + buflen;
    सचे (p > buf)
	MB_PTR_BACK(buf, p);

    // Remove trailing '\\' except root path.
    सचे (p > buf && (*p == '\\' || *p == '/') && p[-1] != ':')
    {
	*p = NUL;
	buflen = (size_t)(p - buf);
    }

    सचे ((buf[0] == '\\' && buf[1] == '\\') || (buf[0] == '/' && buf[1] == '/'))
    {
	// UNC root path must be followed by '\\'.
	p = vim_strpbrk(buf + 2, (char_u *)"\\/");
	सचे (p != रिक्त)
	{
	    p = vim_strpbrk(p + 1, (char_u *)"\\/");
	    सचे (p == रिक्त)
		STRCPY(buf + buflen, "\\");
	}
    }

    wp = enc_to_utf16(buf, रिक्त);
    सचे (wp == रिक्त)
	पटिनिवत्त -1;

    n = mswin_stat_impl(wp, stp, resolve);
    vim_free(wp);
    पटिनिवत्त n;
}

    पूर्णाङ्क
vim_lstat(ध्रुव वर्ण *name, stat_T *stp)
{
    पटिनिवत्त stat_impl(name, stp, FALSE);
}

    पूर्णाङ्क
vim_stat(ध्रुव वर्ण *name, stat_T *stp)
{
    पटिनिवत्त stat_impl(name, stp, TRUE);
}

#सचे defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    शून्य
mch_settmode(tmode_T tmode UNUSED)
{
    // nothing to do
}

    पूर्णाङ्क
mch_get_shellsize(शून्य)
{
    // never used
    पटिनिवत्त OK;
}

    शून्य
mch_set_shellsize(शून्य)
{
    // never used
}

/*
 * Rows and/or Columns has changed.
 */
    शून्य
mch_new_shellsize(शून्य)
{
    // never used
}

    शून्य
mch_calc_cell_size(संरचना cellsize *cs_out UNUSED)
{
    // never used
}

#endif

/*
 * We have no job control, so fake it by starting a new shell.
 */
    शून्य
mch_suspend(शून्य)
{
    suspend_shell();
}

#सचे defined(USE_MCH_ERRMSG)

# ifdef display_errors
#  undef display_errors
# endif

/*
 * Display the saved error message(s).
 */
    शून्य
display_errors(शून्य)
{
# ifdef FEAT_GUI
    char_u *p;

#  ifdef VIMDLL
    सचे (gui.in_use || gui.starting)
#  endif
    {
	सचे (error_ga.ga_data != रिक्त)
	{
	    // avoid putting up a message box with blanks only
	    अत्थाय (p = (char_u *)error_ga.ga_data; *p; ++p)
		सचे (!SAFE_isspace(*p))
		{
		    // Only use a dialog when not using --gui-dialog-file:
		    // write text to a file.
		    सचे (!gui_dialog_log((char_u *)"Errors", p))
			(शून्य)gui_mch_dialog(
				     gui.starting ? VIM_INFO :
					     VIM_ERROR,
				     gui.starting ? (char_u *)_("Message") :
					     (char_u *)_("Error"),
					     p, (char_u *)_("&Ok"),
					1, रिक्त, FALSE);
		    भङ्ग;
		}
	    ga_clear(&error_ga);
	}
	पटिनिवत्त;
    }
# endif
# सचे !defined(FEAT_GUI) || defined(VIMDLL)
    FlushFileBuffers(GetStdHandle(STD_ERROR_HANDLE));
# endif
}
#endif


/*
 * Return TRUE if "p" contain a wildcard that can be expanded by
 * dos_expandpath().
 */
    पूर्णाङ्क
mch_has_exp_wildcard(char_u *p)
{
    अत्थाय ( ; *p; MB_PTR_ADV(p))
    {
	सचे (vim_strchr((char_u *)"?*[", *p) != रिक्त
		|| (*p == '~' && p[1] != NUL))
	    पटिनिवत्त TRUE;
    }
    पटिनिवत्त FALSE;
}

/*
 * Return TRUE if "p" contain a wildcard or a "~1" kind of thing (could be a
 * shortened file name).
 */
    पूर्णाङ्क
mch_has_wildcard(char_u *p)
{
    अत्थाय ( ; *p; MB_PTR_ADV(p))
    {
	सचे (vim_strchr((char_u *)"?*$[`", *p) != रिक्त
		|| (*p == '~' && p[1] != NUL))
	    पटिनिवत्त TRUE;
    }
    पटिनिवत्त FALSE;
}


/*
 * The normal _chdir() does not change the default drive.  This one does.
 * Returning 0 implies success; -1 implies failure.
 */
    पूर्णाङ्क
mch_chdir(वर्ण *path)
{
    WCHAR   *p;
    पूर्णाङ्क	    n;

    सचे (path[0] == NUL)		// just checking...
	पटिनिवत्त -1;

    सचे (p_verbose >= 5)
    {
	verbose_enter();
	smsg("chdir(%s)", path);
	verbose_leave();
    }
    सचे (SAFE_isalpha(path[0]) && path[1] == ':')	// has a drive name
    {
	// If we can change to the drive, skip that part of the path.  If we
	// can't then the current directory may be invalid, try using chdir()
	// with the whole path.
	सचे (_chdrive(TOLOWER_ASC(path[0]) - 'a' + 1) == 0)
	    path += 2;
    }

    सचे (*path == NUL)		// drive name only
	पटिनिवत्त 0;

    p = enc_to_utf16((char_u *)path, रिक्त);
    सचे (p == रिक्त)
	पटिनिवत्त -1;

    n = _wchdir(p);
    vim_free(p);
    पटिनिवत्त n;
}


#सचे defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
/*
 * return non-zero if a character is available
 */
    पूर्णाङ्क
mch_char_avail(शून्य)
{
    // never used
    पटिनिवत्त TRUE;
}

# सचे defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    पूर्णाङ्क
mch_check_messages(शून्य)
{
    // TODO: check for messages
    पटिनिवत्त TRUE;
}
# endif
#endif


#सचे defined(FEAT_LIBCALL)
/*
 * Call a DLL routine which takes either a string or int param
 * and returns an allocated string.
 * Return OK if it worked, FAIL if not.
 */
प्रकारनिर्देश char_u *(*MYSTRPROCSTR)(char_u *);
प्रकारनिर्देश पूर्णाङ्क (*MYSTRPROCINT)(char_u *);
प्रकारनिर्देश char_u *(*MYINTPROCSTR)(पूर्णाङ्क);
प्रकारनिर्देश पूर्णाङ्क (*MYINTPROCINT)(पूर्णाङ्क);

/*
 * Check if a pointer points to a valid NUL terminated string.
 * Return the length of the string, including terminating NUL.
 * Returns 0 for an invalid pointer, 1 for an empty string.
 */
    स्थिर size_t
check_str_len(char_u *str)
{
    SYSTEM_INFO			si;
    MEMORY_BASIC_INFORMATION	mbi;
    size_t			length = 0;
    size_t			i;
    ध्रुव char_u		*p;

    // get page size
    GetSystemInfo(&si);

    // get memory information
    सचे (!VirtualQuery(str, &mbi, परिमाण(mbi)))
	पटिनिवत्त 0;

    // pre cast these (typing savers)
    long_u dwStr = (long_u)str;
    long_u dwBaseAddress = (long_u)mbi.BaseAddress;

    // get start address of page that str is on
    long_u strPage = dwStr - (dwStr - dwBaseAddress) % si.dwPageSize;

    // get length from str to end of page
    long_u pageLength = si.dwPageSize - (dwStr - strPage);

    अत्थाय (p = str; !IsBadReadPtr(p, (UINT)pageLength);
	    p += pageLength, pageLength = si.dwPageSize)
	अत्थाय (i = 0; i < pageLength; ++i, ++length)
	    सचे (p[i] == NUL)
		पटिनिवत्त length + 1;

    पटिनिवत्त 0;
}

/*
 * Passed to do_in_runtimepath() to load a vim.ico file.
 */
    स्थिर शून्य
mch_icon_load_cb(char_u *fname, शून्य *cookie)
{
    HANDLE *h = (HANDLE *)cookie;

    *h = LoadImage(रिक्त,
		   (LPSTR)fname,
		   IMAGE_ICON,
		   64,
		   64,
		   LR_LOADFROMFILE | LR_LOADMAP3DCOLORS);
}

/*
 * Try loading an icon file from 'runtimepath'.
 */
    पूर्णाङ्क
mch_icon_load(HANDLE *iconp)
{
    पटिनिवत्त do_in_runtimepath((char_u *)"bitmaps/vim.ico",
						  0, mch_icon_load_cb, iconp);
}

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if the OS PRNG is not available or something went wrong.
 */
    पूर्णाङ्क
mch_get_random(char_u *buf, पूर्णाङ्क len)
{
    स्थिर पूर्णाङ्क		initialized = NOTDONE;
    स्थिर HINSTANCE	hInstLib;
    स्थिर BOOL (WINAPI *pProcessPrng)(PUCHAR, ULONG);

    सचे (initialized == NOTDONE)
    {
	hInstLib = vimLoadLib("bcryptprimitives.dll");
	सचे (hInstLib != रिक्त)
	    pProcessPrng = (शून्य *)GetProcAddress(hInstLib, "ProcessPrng");
	सचे (hInstLib == रिक्त || pProcessPrng == रिक्त)
	{
	    FreeLibrary(hInstLib);
	    initialized = FAIL;
	}
	अञ्ञथा
	    initialized = OK;
    }

    सचे (initialized == FAIL)
	पटिनिवत्त FAIL;

    // According to the documentation this call cannot fail.
    pProcessPrng(buf, len);

    पटिनिवत्त OK;
}

    पूर्णाङ्क
mch_libcall(
    char_u	*libname,
    char_u	*funcname,
    char_u	*argstring,	// NULL when using a argint
    पूर्णाङ्क		argint,
    char_u	**string_result,// NULL when using number_result
    पूर्णाङ्क		*सङ्ख्या_result)
{
    HINSTANCE		hinstLib;

    BOOL fRunTimeLinkSuccess = FALSE;

    // Get a handle to the DLL module.
    hinstLib = vimLoadLib((वर्ण *)libname);

    // If the handle is valid, try to get the function address.
    सचे (hinstLib != रिक्त)
    {
	char_u	*retval_str = रिक्त;
	पूर्णाङ्क	retval_int = 0;
	size_t	len;

# ifdef HAVE_TRY_EXCEPT
	__try
	{
# endif
	सचे (argstring != रिक्त)
	{
	    // Call with string argument
	    सचे (string_result != रिक्त)
	    {
		MYSTRPROCSTR proc;

		proc = (MYSTRPROCSTR)GetProcAddress(hinstLib, (LPCSTR)funcname);
		सचे (proc != रिक्त)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_str = proc(argstring);
		}
	    }
	    अञ्ञथा
	    {
		MYSTRPROCINT proc;

		proc = (MYSTRPROCINT)GetProcAddress(hinstLib, (LPCSTR)funcname);
		सचे (proc != रिक्त)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_int = proc(argstring);
		}
	    }
	}
	अञ्ञथा
	{
	    // Call with number argument
	    सचे (string_result != रिक्त)
	    {
		MYINTPROCSTR proc;

		proc = (MYINTPROCSTR)GetProcAddress(hinstLib, (LPCSTR)funcname);
		सचे (proc != रिक्त)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_str = proc(argint);
		}
	    }
	    अञ्ञथा
	    {
		MYINTPROCINT proc;

		proc = (MYINTPROCINT)GetProcAddress(hinstLib, (LPCSTR)funcname);
		सचे (proc != रिक्त)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_int = proc(argint);
		}
	    }
	}

	// Save the string before we free the library.
	// Assume that a "1" result is an illegal pointer.
	सचे (string_result == रिक्त)
	    *सङ्ख्या_result = retval_int;
	अञ्ञथा सचे (retval_str != रिक्त
		&& (len = check_str_len(retval_str)) > 0)
	{
	    *string_result = alloc(len);
	    सचे (*string_result != रिक्त)
		mch_memmove(*string_result, retval_str, len);
	}

# ifdef HAVE_TRY_EXCEPT
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	    सचे (GetExceptionCode() == EXCEPTION_राशिः_OVERFLOW)
		_resetstkoflw();
	    fRunTimeLinkSuccess = FALSE;
	}
# endif

	// Free the DLL module.
	(शून्य)FreeLibrary(hinstLib);
    }

    सचे (!fRunTimeLinkSuccess)
    {
	semsg(_(e_library_call_failed_for_str), funcname);
	पटिनिवत्त FAIL;
    }

    पटिनिवत्त OK;
}
#endif

/*
 * Debugging helper: expose the MCH_WRITE_DUMP stuff to other modules
 */
    शून्य
DumpPutS(ध्रुव वर्ण *psz UNUSED)
{
#ifdef MCH_WRITE_DUMP
    सचे (fdDump)
    {
	fputs(psz, fdDump);
	सचे (psz[strlen(psz) - 1] != '\n')
	    fputc('\n', fdDump);
	fflush(fdDump);
    }
#endif
}

#ifdef _DEBUG

शून्य __cdecl
Trace(
    वर्ण *pszFormat,
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

#सचे !defined(FEAT_GUI) || defined(VIMDLL)
बाह्य HWND g_hWnd;	// This is in os_win32.c.

/*
 * Showing the printer dialog is tricky since we have no GUI
 * window to parent it. The following routines are needed to
 * get the window parenting and Z-order to work properly.
 */
    स्थिर शून्य
GetConsoleHwnd(शून्य)
{
    // Skip if it's already set.
    सचे (s_hwnd != 0)
	पटिनिवत्त;

    // Window handle may have been found by init code (Windows NT only)
    सचे (g_hWnd != 0)
    {
	s_hwnd = g_hWnd;
	पटिनिवत्त;
    }

    s_hwnd = GetConsoleWindow();
}

/*
 * Console implementation of ":winpos".
 */
    पूर्णाङ्क
mch_get_winpos(पूर्णाङ्क *x, पूर्णाङ्क *y)
{
    RECT  rect;

    GetConsoleHwnd();
    GetWindowRect(s_hwnd, &rect);
    *x = rect.वामः;
    *y = rect.शिखरम्;
    पटिनिवत्त OK;
}

/*
 * Console implementation of ":winpos x y".
 */
    शून्य
mch_set_winpos(पूर्णाङ्क x, पूर्णाङ्क y)
{
    GetConsoleHwnd();
    SetWindowPos(s_hwnd, रिक्त, x, y, 0, 0,
		 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}
#endif

#सचे defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT)

//=================================================================
// Win32 printer stuff

स्थिर HFONT		prt_font_handles[2][2][2];
स्थिर PRINTDLGW	prt_dlg;
स्थिर ध्रुव पूर्णाङ्क	boldface[2] = {FW_REGULAR, FW_BOLD};
स्थिर TEXTMETRIC	prt_tm;
स्थिर पूर्णाङ्क		prt_पङ्क्तिः_height;
स्थिर पूर्णाङ्क		prt_सङ्ख्या_width;
स्थिर पूर्णाङ्क		prt_वामः_margin;
स्थिर पूर्णाङ्क		prt_दक्षिणः_margin;
स्थिर पूर्णाङ्क		prt_शिखरम्_margin;
स्थिर char_u		szAppName[] = TEXT("VIM");
स्थिर HWND		hDlgPrint;
स्थिर पूर्णाङ्क		*bUserAbort = रिक्त;
स्थिर char_u		*prt_name = रिक्त;

// Defines which are also in vim.rc.
# define IDC_BOX1		400
# define IDC_PRINTTEXT1		401
# define IDC_PRINTTEXT2		402
# define IDC_PROGRESS		403

    स्थिर BOOL
vimSetDlgItemText(HWND hDlg, पूर्णाङ्क nIDDlgItem, char_u *s)
{
    WCHAR   *wp;
    BOOL    ret;

    wp = enc_to_utf16(s, रिक्त);
    सचे (wp == रिक्त)
	पटिनिवत्त FALSE;

    ret = SetDlgItemTextW(hDlg, nIDDlgItem, wp);
    vim_free(wp);
    पटिनिवत्त ret;
}

/*
 * Convert BGR to RGB for Windows GDI calls
 */
    स्थिर COLORREF
swap_me(COLORREF colorref)
{
    पूर्णाङ्क  temp;
    वर्ण *ptr = (वर्ण *)&colorref;

    temp = *(ptr);
    *(ptr ) = *(ptr + 2);
    *(ptr + 2) = temp;
    पटिनिवत्त colorref;
}

    स्थिर INT_PTR CALLBACK
PrintDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam UNUSED,
	LPARAM lParam UNUSED)
{
# ifdef FEAT_GETTEXT
    NONCLIENTMETRICS nm;
    स्थिर HFONT hfont;
# endif

    विकल्प (message)
    {
	पक्ष WM_INITDIALOG:
# ifdef FEAT_GETTEXT
	    nm.cbSize = परिमाण(NONCLIENTMETRICS);
	    सचे (SystemParametersInfo(
			SPI_GETNONCLIENTMETRICS,
			परिमाण(NONCLIENTMETRICS),
			&nm,
			0))
	    {
		वर्ण buff[MAX_PATH];
		पूर्णाङ्क i;

		// Translate the dialog texts
		hfont = CreateFontIndirect(&nm.lfMessageFont);
		अत्थाय (i = IDC_PRINTTEXT1; i <= IDC_PROGRESS; i++)
		{
		    SendDlgItemMessage(hDlg, i, WM_SETFONT, (WPARAM)hfont, 1);
		    सचे (GetDlgItemText(hDlg,i, buff, परिमाण(buff)))
			vimSetDlgItemText(hDlg,i, (char_u *)_(buff));
		}
		SendDlgItemMessage(hDlg, IDCANCEL,
						WM_SETFONT, (WPARAM)hfont, 1);
		सचे (GetDlgItemText(hDlg,IDCANCEL, buff, परिमाण(buff)))
		    vimSetDlgItemText(hDlg,IDCANCEL, (char_u *)_(buff));
	    }
# endif
	    SetWindowText(hDlg, (LPCSTR)szAppName);
	    सचे (prt_name != रिक्त)
	    {
		vimSetDlgItemText(hDlg, IDC_PRINTTEXT2, (char_u *)prt_name);
		VIM_CLEAR(prt_name);
	    }
	    EnableMenuItem(GetSystemMenu(hDlg, FALSE), SC_CLOSE, MF_GRAYED);
# सचे !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	    सचे (!gui.in_use)
#  endif
		Bring_Window_To_शिखरम्(s_hwnd);
# endif
	    पटिनिवत्त TRUE;

	पक्ष WM_COMMAND:
	    *bUserAbort = TRUE;
	    EnableWindow(GetParent(hDlg), TRUE);
	    DestroyWindow(hDlg);
	    hDlgPrint = रिक्त;
# ifdef FEAT_GETTEXT
	    DeleteObject(hfont);
# endif
	    पटिनिवत्त TRUE;
    }
    पटिनिवत्त FALSE;
}

    स्थिर BOOL CALLBACK
AbortProc(HDC hdcPrn UNUSED, पूर्णाङ्क iCode UNUSED)
{
    सन्देशः_2 सन्देशः;

    याव (!*bUserAbort && PeekMessageW(&सन्देशः, रिक्त, 0, 0, PM_REMOVE))
    {
	सचे (!hDlgPrint || !IsDialogMessageW(hDlgPrint, &सन्देशः))
	{
	    TranslateMessage(&सन्देशः);
	    DispatchMessageW(&सन्देशः);
	}
    }
    पटिनिवत्त !*bUserAbort;
}

    स्थिर UINT_PTR CALLBACK
PrintHookProc(
	HWND hDlg,	// handle to dialog box
	UINT ui_सन्देशः,	// message identifier
	WPARAM wParam UNUSED,	// message parameter
	LPARAM lParam	// message parameter
	)
{
    HWND	hwndOwner;
    RECT	rc, rcDlg, rcOwner;
    PRINTDLGW	*pPD;

    सचे (ui_सन्देशः != WM_INITDIALOG)
	पटिनिवत्त FALSE;

    // Get the owner window and dialog box rectangles.
    सचे ((hwndOwner = GetParent(hDlg)) == रिक्त)
	hwndOwner = GetDesktopWindow();

    GetWindowRect(hwndOwner, &rcOwner);
    GetWindowRect(hDlg, &rcDlg);
    CopyRect(&rc, &rcOwner);

    // Offset the owner and dialog box rectangles so that
    // right and bottom values represent the width and
    // height, and then offset the owner again to discard
    // space taken up by the dialog box.

    OffsetRect(&rcDlg, -rcDlg.वामः, -rcDlg.शिखरम्);
    OffsetRect(&rc, -rc.वामः, -rc.शिखरम्);
    OffsetRect(&rc, -rcDlg.दक्षिणः, -rcDlg.bottom);

    // The new position is the sum of half the remaining
    // space and the owner's original position.

    SetWindowPos(hDlg,
	    HWND_शिखरम्,
	    rcOwner.वामः + (rc.दक्षिणः / 2),
	    rcOwner.शिखरम् + (rc.bottom / 2),
	    0, 0,		// ignores size arguments
	    SWP_NOSIZE);

    //  tackle the printdlg copiesctrl problem
    pPD = (PRINTDLGW *)lParam;
    pPD->nCopies = (WORD)pPD->lCustData;
    SetDlgItemInt( hDlg, edt3, pPD->nCopies, FALSE );
    //  Bring the window to top
    Bring_Window_To_शिखरम्(GetParent(hDlg));
    SetForegroundWindow(hDlg);

    पटिनिवत्त FALSE;
}

    शून्य
mch_print_cleanup(शून्य)
{
    पूर्णाङ्क pifItalic;
    पूर्णाङ्क pifBold;
    पूर्णाङ्क pifUnderline;

    अत्थाय (pifBold = 0; pifBold <= 1; pifBold++)
	अत्थाय (pifItalic = 0; pifItalic <= 1; pifItalic++)
	    अत्थाय (pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
		DeleteObject(prt_font_handles[pifBold][pifItalic][pifUnderline]);

    सचे (prt_dlg.hDC != रिक्त)
	DeleteDC(prt_dlg.hDC);
    सचे (!*bUserAbort && hDlgPrint != रिक्त)
	SendMessage(hDlgPrint, WM_COMMAND, 0, 0);
}

    स्थिर पूर्णाङ्क
to_device_units(पूर्णाङ्क idx, पूर्णाङ्क dpi, पूर्णाङ्क physsize, पूर्णाङ्क offset, पूर्णाङ्क def_सङ्ख्या)
{
    पूर्णाङ्क		ret = 0;
    पूर्णाङ्क		u;
    पूर्णाङ्क		nr;

    u = prt_get_unit(idx);
    सचे (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_सङ्ख्या;
    }
    अञ्ञथा
	nr = printer_opts[idx].सङ्ख्या;

    विकल्प (u)
    {
	पक्ष PRT_UNIT_PERC:
	    ret = (physsize * nr) / 100;
	    भङ्ग;
	पक्ष PRT_UNIT_INCH:
	    ret = (nr * dpi);
	    भङ्ग;
	पक्ष PRT_UNIT_MM:
	    ret = (nr * 10 * dpi) / 254;
	    भङ्ग;
	पक्ष PRT_UNIT_POINT:
	    ret = (nr * 10 * dpi) / 720;
	    भङ्ग;
    }

    सचे (ret < offset)
	पटिनिवत्त 0;
    अञ्ञथा
	पटिनिवत्त ret - offset;
}

    स्थिर पूर्णाङ्क
prt_get_cpl(शून्य)
{
    पूर्णाङ्क		hr;
    पूर्णाङ्क		phyw;
    पूर्णाङ्क		dvoff;
    पूर्णाङ्क		rev_offset;
    पूर्णाङ्क		dpi;

    GetTextMetrics(prt_dlg.hDC, &prt_tm);
    prt_पङ्क्तिः_height = prt_tm.tmHeight + prt_tm.tmExternalLeading;

    hr	    = GetDeviceCaps(prt_dlg.hDC, HORZRES);
    phyw    = GetDeviceCaps(prt_dlg.hDC, PHYSICALWIDTH);
    dvoff   = GetDeviceCaps(prt_dlg.hDC, PHYSICALOFFSETX);
    dpi	    = GetDeviceCaps(prt_dlg.hDC, LOGPIXELSX);

    rev_offset = phyw - (dvoff + hr);

    prt_वामः_margin = to_device_units(OPT_PRINT_वामः, dpi, phyw, dvoff, 10);
    सचे (prt_use_सङ्ख्या())
    {
	prt_सङ्ख्या_width = PRINT_सङ्ख्या_WIDTH * prt_tm.tmAveCharWidth;
	prt_वामः_margin += prt_सङ्ख्या_width;
    }
    अञ्ञथा
	prt_सङ्ख्या_width = 0;

    prt_दक्षिणः_margin = hr - to_device_units(OPT_PRINT_दक्षिणः, dpi, phyw,
							       rev_offset, 5);

    पटिनिवत्त (prt_दक्षिणः_margin - prt_वामः_margin) / prt_tm.tmAveCharWidth;
}

    स्थिर पूर्णाङ्क
prt_get_lpp(शून्य)
{
    पूर्णाङ्क vr;
    पूर्णाङ्क phyw;
    पूर्णाङ्क dvoff;
    पूर्णाङ्क rev_offset;
    पूर्णाङ्क	bottom_margin;
    पूर्णाङ्क	dpi;

    vr	    = GetDeviceCaps(prt_dlg.hDC, VERTRES);
    phyw    = GetDeviceCaps(prt_dlg.hDC, PHYSICALHEIGHT);
    dvoff   = GetDeviceCaps(prt_dlg.hDC, PHYSICALOFFSETY);
    dpi	    = GetDeviceCaps(prt_dlg.hDC, LOGPIXELSY);

    rev_offset = phyw - (dvoff + vr);

    prt_शिखरम्_margin = to_device_units(OPT_PRINT_शिखरम्, dpi, phyw, dvoff, 5);

    // adjust top margin if there is a header
    prt_शिखरम्_margin += prt_पङ्क्तिः_height * prt_header_height();

    bottom_margin = vr - to_device_units(OPT_PRINT_BOT, dpi, phyw,
							       rev_offset, 5);

    पटिनिवत्त (bottom_margin - prt_शिखरम्_margin) / prt_पङ्क्तिः_height;
}

    पूर्णाङ्क
mch_print_init(prt_settings_T *psettings, char_u *jobname, पूर्णाङ्क forceit)
{
    स्थिर HGLOBAL	stored_dm = रिक्त;
    स्थिर HGLOBAL	stored_devn = रिक्त;
    स्थिर पूर्णाङ्क		stored_nCopies = 1;
    स्थिर पूर्णाङ्क		stored_nFlags = 0;

    LOGFONTW		fLogFont;
    पूर्णाङ्क			pifItalic;
    पूर्णाङ्क			pifBold;
    पूर्णाङ्क			pifUnderline;

    DEVMODEW		*mem;
    DEVNAMES		*devname;
    पूर्णाङ्क			i;
    DWORD		err;

    bUserAbort = &(psettings->user_abort);
    CLEAR_FIELD(prt_dlg);
    prt_dlg.lStructSize = परिमाण(PRINTDLGW);
# सचे !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
    सचे (!gui.in_use)
#  endif
	GetConsoleHwnd();	    // get value of s_hwnd
# endif
    prt_dlg.hwndOwner = s_hwnd;
    prt_dlg.Flags = PD_NOPAGENUMS | PD_NOSELECTION | PD_RETURNDC;
    सचे (!forceit)
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
    सचे (forceit
# सचे !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	    || (!gui.in_use && !term_console)
#  अञ्ञथा
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
	सचे (*p_pdev != NUL)
	    prt_dlg.hDC = CreateDC(रिक्त, (LPCSTR)p_pdev, रिक्त, रिक्त);
	अञ्ञथा
	{
	    prt_dlg.Flags |= PD_RETURNDEFAULT;
	    सचे (PrintDlgW(&prt_dlg) == 0)
		गच्छ init_fail_dlg;
	}
    }
    अञ्ञथा सचे (PrintDlgW(&prt_dlg) == 0)
	गच्छ init_fail_dlg;
    अञ्ञथा
    {
	/*
	 * keep the previous driver context
	 */
	stored_dm = prt_dlg.hDevMode;
	stored_devn = prt_dlg.hDevNames;
	stored_nFlags = prt_dlg.Flags;
	stored_nCopies = prt_dlg.nCopies;
    }

    सचे (prt_dlg.hDC == रिक्त)
    {
	emsg(_(e_printer_selection_failed));
	mch_print_cleanup();
	पटिनिवत्त FALSE;
    }

    // Not all printer drivers report the support of color (or grey) in the
    // same way.  Let's set has_color if there appears to be some way to print
    // more than B&W.
    i = GetDeviceCaps(prt_dlg.hDC, NUMCOLORS);
    psettings->has_color = (GetDeviceCaps(prt_dlg.hDC, BITSPIXEL) > 1
				   || GetDeviceCaps(prt_dlg.hDC, PLANES) > 1
				   || i > 2 || i == -1);

    // Ensure all font styles are baseline aligned
    SetTextAlign(prt_dlg.hDC, TA_BASELINE|TA_वामः);

    /*
     * On some windows systems the nCopies parameter is not
     * passed back correctly. It must be retrieved from the
     * hDevMode struct.
     */
    mem = (DEVMODEW *)GlobalLock(prt_dlg.hDevMode);
    सचे (mem != रिक्त)
    {
	सचे (mem->dmCopies != 1)
	    stored_nCopies = mem->dmCopies;
	सचे ((mem->dmFields & DM_DUPLEX) && (mem->dmDuplex & ~DMDUP_SIMPLEX))
	    psettings->duplex = TRUE;
	सचे ((mem->dmFields & DM_COLOR) && (mem->dmColor & DMCOLOR_COLOR))
	    psettings->has_color = TRUE;
    }
    GlobalUnlock(prt_dlg.hDevMode);

    devname = (DEVNAMES *)GlobalLock(prt_dlg.hDevNames);
    सचे (devname != 0)
    {
	WCHAR	*wprinter_name = (WCHAR *)devname + devname->wDeviceOffset;
	WCHAR	*wport_name = (WCHAR *)devname + devname->wOutputOffset;
	char_u	*text = (char_u *)_("to %s on %s");
	char_u  *printer_name = utf16_to_enc(wprinter_name, रिक्त);
	char_u	*port_name = utf16_to_enc(wport_name, रिक्त);

	सचे (printer_name != रिक्त && port_name != रिक्त)
	    prt_name = alloc(STRLEN(printer_name)
					   + STRLEN(port_name) + STRLEN(text));
	सचे (prt_name != रिक्त)
	    wsprintf((वर्ण *)prt_name, (ध्रुव वर्ण *)text,
		    printer_name, port_name);
	vim_free(printer_name);
	vim_free(port_name);
    }
    GlobalUnlock(prt_dlg.hDevNames);

    /*
     * Initialise the font according to 'printfont'
     */
    CLEAR_FIELD(fLogFont);
    सचे (get_logfont(&fLogFont, p_pfn, prt_dlg.hDC, TRUE) == FAIL)
    {
	semsg(_(e_unknown_printer_font_str), p_pfn);
	mch_print_cleanup();
	पटिनिवत्त FALSE;
    }

    अत्थाय (pifBold = 0; pifBold <= 1; pifBold++)
	अत्थाय (pifItalic = 0; pifItalic <= 1; pifItalic++)
	    अत्थाय (pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
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
    psettings->chars_per_पङ्क्तिः = prt_get_cpl();
    psettings->पङ्क्तयः_per_page = prt_get_lpp();
    सचे (prt_dlg.Flags & PD_USEDEVMODECOPIESANDCOLLATE)
    {
	psettings->n_collated_copies = (prt_dlg.Flags & PD_COLLATE)
						    ? prt_dlg.nCopies : 1;
	psettings->n_uncollated_copies = (prt_dlg.Flags & PD_COLLATE)
						    ? 1 : prt_dlg.nCopies;

	सचे (psettings->n_collated_copies == 0)
	    psettings->n_collated_copies = 1;

	सचे (psettings->n_uncollated_copies == 0)
	    psettings->n_uncollated_copies = 1;
    }
    अञ्ञथा
    {
	psettings->n_collated_copies = 1;
	psettings->n_uncollated_copies = 1;
    }

    psettings->jobname = jobname;

    पटिनिवत्त TRUE;

init_fail_dlg:
    err = CommDlgExtendedError();
    सचे (err)
    {
	char_u *buf;

	// I suspect FormatMessage() doesn't work for values returned by
	// CommDlgExtendedError().  What does?
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		रिक्त, err, 0, (LPTSTR)(&buf), 0, रिक्त);
	semsg(_(e_print_error_str),
		buf == रिक्त ? (char_u *)_("Unknown") : buf);
	LocalFree((LPVOID)(buf));
    }
    अञ्ञथा
	सन्देशः_clr_eos(); // Maybe canceled

    mch_print_cleanup();
    पटिनिवत्त FALSE;
}


    पूर्णाङ्क
mch_print_begin(prt_settings_T *psettings)
{
    पूर्णाङ्क			ret = 0;
    वर्ण		szBuffer[300];
    WCHAR		*wp;

    hDlgPrint = CreateDialog(g_hinst, TEXT("PrintDlgBox"),
					     prt_dlg.hwndOwner, PrintDlgProc);
    SetAbortProc(prt_dlg.hDC, AbortProc);
    wsprintf(szBuffer, _("Printing '%s'"), gettail(psettings->jobname));
    vimSetDlgItemText(hDlgPrint, IDC_PRINTTEXT1, (char_u *)szBuffer);

    wp = enc_to_utf16(psettings->jobname, रिक्त);
    सचे (wp != रिक्त)
    {
	DOCINFOW	di;

	CLEAR_FIELD(di);
	di.cbSize = परिमाण(di);
	di.lpszDocName = wp;
	ret = StartDocW(prt_dlg.hDC, &di);
	vim_free(wp);
    }

# ifdef FEAT_GUI
    // Give focus back to main window (when using MDI).
#  ifdef VIMDLL
    सचे (gui.in_use)
#  endif
	SetFocus(s_hwnd);
# endif

    पटिनिवत्त (ret > 0);
}

    शून्य
mch_print_end(prt_settings_T *psettings UNUSED)
{
    EndDoc(prt_dlg.hDC);
    सचे (!*bUserAbort && hDlgPrint != रिक्त)
	SendMessage(hDlgPrint, WM_COMMAND, 0, 0);
}

    पूर्णाङ्क
mch_print_end_page(शून्य)
{
    पटिनिवत्त (EndPage(prt_dlg.hDC) > 0);
}

    पूर्णाङ्क
mch_print_begin_page(char_u *सन्देशः)
{
    सचे (सन्देशः != रिक्त)
	vimSetDlgItemText(hDlgPrint, IDC_PROGRESS, सन्देशः);
    पटिनिवत्त (StartPage(prt_dlg.hDC) > 0);
}

    पूर्णाङ्क
mch_print_blank_page(शून्य)
{
    पटिनिवत्त (mch_print_begin_page(रिक्त) ? (mch_print_end_page()) : FALSE);
}

स्थिर पूर्णाङ्क prt_pos_x = 0;
स्थिर पूर्णाङ्क prt_pos_y = 0;

    शून्य
mch_print_start_पङ्क्तिः(पूर्णाङ्क margin, पूर्णाङ्क page_पङ्क्तिः)
{
    सचे (margin)
	prt_pos_x = -prt_सङ्ख्या_width;
    अञ्ञथा
	prt_pos_x = 0;
    prt_pos_y = page_पङ्क्तिः * prt_पङ्क्तिः_height
				 + prt_tm.tmAscent + prt_tm.tmExternalLeading;
}

    पूर्णाङ्क
mch_print_text_out(char_u *p, पूर्णाङ्क len)
{
    SIZE	sz;
    WCHAR	*wp;
    पूर्णाङ्क		wlen = len;
    पूर्णाङ्क		ret = FALSE;

    wp = enc_to_utf16(p, &wlen);
    सचे (wp == रिक्त)
	पटिनिवत्त FALSE;

    TextOutW(prt_dlg.hDC, prt_pos_x + prt_वामः_margin,
	    prt_pos_y + prt_शिखरम्_margin, wp, wlen);
    GetTextExtentPoint32W(prt_dlg.hDC, wp, wlen, &sz);
    vim_free(wp);
    prt_pos_x += (sz.cx - prt_tm.tmOverhang);
    // This is wrong when printing spaces for a TAB.
    सचे (p[len] != NUL)
    {
	wlen = mb_ptr2len(p + len);
	wp = enc_to_utf16(p + len, &wlen);
	सचे (wp != रिक्त)
	{
	    GetTextExtentPoint32W(prt_dlg.hDC, wp, 1, &sz);
	    ret = (prt_pos_x + prt_वामः_margin + sz.cx > prt_दक्षिणः_margin);
	    vim_free(wp);
	}
    }
    पटिनिवत्त ret;
}

    शून्य
mch_print_set_font(पूर्णाङ्क iBold, पूर्णाङ्क iItalic, पूर्णाङ्क iUnderline)
{
    SelectObject(prt_dlg.hDC, prt_font_handles[iBold][iItalic][iUnderline]);
}

    शून्य
mch_print_set_bg(long_u bgcol)
{
    SetBkColor(prt_dlg.hDC, GetNearestColor(prt_dlg.hDC,
						   swap_me((COLORREF)bgcol)));
    /*
     * With a white background we can draw characters transparent, which is
     * good for italic characters that overlap to the next char cell.
     */
    सचे (bgcol == 0xffffffUL)
	SetBkMode(prt_dlg.hDC, TRANSPARENT);
    अञ्ञथा
	SetBkMode(prt_dlg.hDC, OPAQUE);
}

    शून्य
mch_print_set_fg(long_u fgcol)
{
    SetTextColor(prt_dlg.hDC, GetNearestColor(prt_dlg.hDC,
						   swap_me((COLORREF)fgcol)));
}

#endif // FEAT_PRINTER && !FEAT_POSTSCRIPT



#सचे defined(FEAT_SHORTCUT)
# include <shlobj.h>

# define is_path_sep(c)	    ((c) == L'\\' || (c) == L'/')

    स्थिर पूर्णाङ्क
is_reparse_point_included(LPCWSTR fname)
{
    LPCWSTR	p = fname, q;
    WCHAR	buf[MAX_PATH];
    DWORD	attr;

    सचे (SAFE_isalpha(p[0]) && p[1] == L':' && is_path_sep(p[2]))
	p += 3;
    अञ्ञथा सचे (is_path_sep(p[0]) && is_path_sep(p[1]))
	p += 2;

    याव (*p != L'\0')
    {
	q = wcspbrk(p, L"\\/");
	सचे (q == रिक्त)
	    p = q = fname + wcslen(fname);
	अञ्ञथा
	    p = q + 1;
	सचे (q - fname >= MAX_PATH)
	    पटिनिवत्त FALSE;
	wcsncpy(buf, fname, q - fname);
	buf[q - fname] = L'\0';
	attr = GetFileAttributesW(buf);
	सचे (attr != INVALID_FILE_ATTRIBUTES
		&& (attr & FILE_ATTRIBUTE_REPARSE_POINT) != 0)
	    पटिनिवत्त TRUE;
    }
    पटिनिवत्त FALSE;
}

/*
 * Return the resolved file path, NULL if "fname" is an AppExecLink reparse
 * point, already fully resolved, or it doesn't exists.
 */
    char_u *
resolve_reparse_point(char_u *fname)
{
    HANDLE	    h = INVALID_HANDLE_मानम्;
    DWORD	    size;
    WCHAR	    *p, *wp;
    char_u	    *rfname = रिक्त;
    WCHAR	    *buff = रिक्त;

    p = enc_to_utf16(fname, रिक्त);
    सचे (p == रिक्त)
	गच्छ fail;

    सचे (!is_reparse_point_included(p))
    {
	vim_free(p);
	गच्छ fail;
    }

    h = CreateFileW(p, 0, 0, रिक्त, OPEN_EXISTING,
	    FILE_FLAG_BACKUP_SEMANTICS, रिक्त);
    vim_free(p);

    सचे (h == INVALID_HANDLE_मानम्)
	गच्छ fail;

    size = GetFinalPathNameByHandleW(h, रिक्त, 0, 0);
    सचे (size == 0)
	गच्छ fail;
    buff = ALLOC_MULT(WCHAR, size);
    सचे (buff == रिक्त)
	गच्छ fail;
    सचे (GetFinalPathNameByHandleW(h, buff, size, 0) == 0)
	गच्छ fail;

    सचे (wcsncmp(buff, L"\\\\?\\UNC\\", 8) == 0)
    {
	buff[6] = L'\\';
	wp = buff + 6;
    }
    अञ्ञथा सचे (wcsncmp(buff, L"\\\\?\\", 4) == 0)
	wp = buff + 4;
    अञ्ञथा
	wp = buff;

    rfname = utf16_to_enc(wp, रिक्त);

fail:
    सचे (h != INVALID_HANDLE_मानम्)
	CloseHandle(h);
    सचे (buff != रिक्त)
	vim_free(buff);

    पटिनिवत्त rfname;
}

/*
 * When "fname" is the name of a shortcut (*.lnk) resolve the file it points
 * to and return that name in allocated memory.
 * Otherwise NULL is returned.
 */
    स्थिर char_u *
resolve_shortcut(char_u *fname)
{
    HRESULT		hr;
    IShellLink		*psl = रिक्त;
    IPersistFile	*ppf = रिक्त;
    OLECHAR		wsz[MAX_PATH];
    char_u		*rfname = रिक्त;
    पूर्णाङ्क			len;
    IShellLinkW		*pslw = रिक्त;
    WIN32_FIND_DATAW	ffdw; // we get those free of charge

    // Check if the file name ends in ".lnk". Avoid calling
    // CoCreateInstance(), it's quite slow.
    सचे (fname == रिक्त)
	पटिनिवत्त rfname;
    len = (पूर्णाङ्क)STRLEN(fname);
    सचे (len <= 4 || STRNICMP(fname + len - 4, ".lnk", 4) != 0)
	पटिनिवत्त rfname;

    CoInitialize(रिक्त);

    // create a link manager object and request its interface
    hr = CoCreateInstance(
	    &CLSID_ShellLink, रिक्त, CLSCTX_INPROC_SERVER,
	    &IID_IShellLinkW, (शून्य**)&pslw);
    सचे (hr == S_OK)
    {
	WCHAR	*p = enc_to_utf16(fname, रिक्त);

	सचे (p != रिक्त)
	{
	    // Get a pointer to the IPersistFile interface.
	    hr = pslw->lpVtbl->QueryInterface(
		    pslw, &IID_IPersistFile, (शून्य**)&ppf);
	    सचे (hr != S_OK)
		गच्छ shortcut_errorw;

	    // "load" the name and resolve the link
	    hr = ppf->lpVtbl->Load(ppf, p, STGM_READ);
	    सचे (hr != S_OK)
		गच्छ shortcut_errorw;
# सचे 0  // This makes Vim wait a long time if the target does not exist.
	    hr = pslw->lpVtbl->Resolve(pslw, रिक्त, SLR_NO_UI);
	    सचे (hr != S_OK)
		गच्छ shortcut_errorw;
# endif

	    // Get the path to the link target.
	    ZeroMemory(wsz, MAX_PATH * परिमाण(WCHAR));
	    hr = pslw->lpVtbl->GetPath(pslw, wsz, MAX_PATH, &ffdw, 0);
	    सचे (hr == S_OK && wsz[0] != NUL)
		rfname = utf16_to_enc(wsz, रिक्त);

shortcut_errorw:
	    vim_free(p);
	}
    }

    // Release all interface pointers (both belong to the same object)
    सचे (ppf != रिक्त)
	ppf->lpVtbl->Release(ppf);
    सचे (psl != रिक्त)
	psl->lpVtbl->Release(psl);
    सचे (pslw != रिक्त)
	pslw->lpVtbl->Release(pslw);

    CoUninitialize();
    पटिनिवत्त rfname;
}

    char_u *
mch_resolve_path(char_u *fname, पूर्णाङ्क reparse_point)
{
    char_u  *path = resolve_shortcut(fname);

    सचे (path == रिक्त && reparse_point)
	path = resolve_reparse_point(fname);
    पटिनिवत्त path;
}
#endif

#सचे defined(FEAT_EVAL) && (!defined(FEAT_GUI) || defined(VIMDLL))
/*
 * Bring ourselves to the foreground.  Does work if the OS doesn't allow it.
 */
    शून्य
win32_set_foreground(शून्य)
{
    GetConsoleHwnd();	    // get value of s_hwnd
    सचे (s_hwnd != 0)
	SetForegroundWindow(s_hwnd);
}
#endif

#सचे defined(FEAT_CLIENTSERVER)
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
# define VIM_CLASSNAME_LEN  (परिमाण(VIM_CLASSNAME) - 1)

// Timeout for sending a message to another Vim instance.  Normally this works
// instantly, but it may hang when the other Vim instance is halted.
# define SENDMESSAGE_TIMEOUT	(5 * 1000)

// Communication is via WM_COPYDATA messages. The message type is sent in
// the dwData parameter. Types are defined here.
# define COPYDATA_कुञ्चिकाः		0
# define COPYDATA_REPLY		1
# define COPYDATA_व्यञ्जकम्		10
# define COPYDATA_RESULT	11
# define COPYDATA_ERROR_RESULT	12
# define COPYDATA_ENCODING	20

// This is a structure containing a server HWND and its name.
संरचना server_id
{
    HWND hwnd;
    char_u *name;
};

// Last received 'encoding' that the client uses.
स्थिर char_u	*client_enc = रिक्त;

/*
 * Tell the other side what encoding we are using.
 * Return -1 if timeout happens.  Other errors are ignored.
 */
    स्थिर पूर्णाङ्क
serverSendEnc(HWND target)
{
    COPYDATASTRUCT data;

    data.dwData = COPYDATA_ENCODING;
    data.cbData = (DWORD)STRLEN(p_enc) + 1;
    data.lpData = p_enc;
    सचे (SendMessageTimeout(target, WM_COPYDATA,
	    (WPARAM)message_window, (LPARAM)&data,
	    SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, रिक्त) == 0)
	पटिनिवत्त -1;
    पटिनिवत्त 0;
}

/*
 * Clean up on exit. This destroys the hidden message window.
 */
    स्थिर शून्य
CleanUpMessaging(शून्य)
{
    सचे (message_window == 0)
	पटिनिवत्त;

    DestroyWindow(message_window);
    message_window = 0;
}

स्थिर पूर्णाङ्क save_reply(HWND server, char_u *reply, पूर्णाङ्क व्यञ्जकम्);

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
    स्थिर LRESULT CALLBACK
Messaging_WndProc(HWND hwnd, UINT सन्देशः, WPARAM wParam, LPARAM lParam)
{
    सचे (सन्देशः == WM_COPYDATA)
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
	पूर्णाङ्क		retval;
	DWORD_PTR	dwret = 0;
	char_u		*str;
	char_u		*tofree;

	विकल्प (data->dwData)
	{
	पक्ष COPYDATA_ENCODING:
	    // Remember the encoding that the client uses.
	    vim_free(client_enc);
	    client_enc = enc_canonize((char_u *)data->lpData);
	    पटिनिवत्त 1;

	पक्ष COPYDATA_कुञ्चिकाः:
	    // Remember who sent this, for <client>
	    clientWindow = sender;

	    // Add the received keys to the input buffer.
	    str = serverConvert(client_enc, (char_u *)data->lpData, &tofree);
# ifdef FEAT_GUI
	    सचे (gui.in_use)
		// The GUI may get here re-entrantly, so insert the keys later.
		server_add_निवेशः(str);
	    अञ्ञथा
# endif
		server_to_निवेशः_buf(str);
	    vim_free(tofree);

# ifdef FEAT_GUI
	    // Wake up the main GUI loop.
#  ifdef VIMDLL
	    सचे (gui.in_use)
#  endif
		सचे (s_hwnd != 0)
		    PostMessage(s_hwnd, WM_NULL, 0, 0);
# endif
	    पटिनिवत्त 1;

	पक्ष COPYDATA_व्यञ्जकम्:
	    // Remember who sent this, for <client>
	    clientWindow = sender;

	    str = serverConvert(client_enc, (char_u *)data->lpData, &tofree);
	    res = eval_client_व्यञ्जकम्_to_string(str);

	    सचे (res == रिक्त)
	    {
		वर्ण	*err = _(e_invalid_expression_received);
		size_t	len = STRLEN(str) + STRLEN(err) + 5;

		res = alloc(len);
		सचे (res != रिक्त)
		    reply.cbData = (DWORD)vim_snprintf_safelen(
			(वर्ण *)res, len, "%s: \"%s\"", err, str);
		अञ्ञथा
		    reply.cbData = 0;
		reply.dwData = COPYDATA_ERROR_RESULT;
	    }
	    अञ्ञथा
	    {
		reply.cbData = (DWORD)STRLEN(res) + 1;
		reply.dwData = COPYDATA_RESULT;
	    }
	    reply.lpData = res;

	    सचे (serverSendEnc(sender) < 0)
		retval = -1;
	    अञ्ञथा
	    {
		सचे (SendMessageTimeout(sender, WM_COPYDATA,
			(WPARAM)message_window, (LPARAM)&reply,
			SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
		    retval = -1;
		अञ्ञथा
		    retval = (पूर्णाङ्क)dwret;
	    }
	    vim_free(tofree);
	    vim_free(res);
	    पटिनिवत्त retval;

	पक्ष COPYDATA_REPLY:
	पक्ष COPYDATA_RESULT:
	पक्ष COPYDATA_ERROR_RESULT:
	    सचे (data->lpData != रिक्त)
	    {
		str = serverConvert(client_enc, (char_u *)data->lpData,
								     &tofree);
		सचे (tofree == रिक्त)
		    str = vim_strsave(str);
		सचे (save_reply(sender, str,
			   (data->dwData == COPYDATA_REPLY ?  0 :
			   (data->dwData == COPYDATA_RESULT ? 1 :
							      2))) == FAIL)
		    vim_free(str);
		अञ्ञथा सचे (data->dwData == COPYDATA_REPLY)
		{
		    char_u	winstr[30];

		    sprintf((वर्ण *)winstr, PRINTF_HEX_LONG_U, (long_u)sender);
		    apply_autocmds(EVENT_REMOTEREPLY, winstr, str,
								TRUE, curbuf);
		}
	    }
	    पटिनिवत्त 1;
	}

	पटिनिवत्त 0;
    }

    अञ्ञथा सचे (सन्देशः == WM_ACTIVATE && wParam == WA_ACTIVE)
    {
	// When the message window is activated (brought to the foreground),
	// this actually applies to the text window.
# सचे !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	सचे (!gui.in_use)
#  endif
	    GetConsoleHwnd();	    // get value of s_hwnd
# endif
	सचे (s_hwnd != 0)
	{
	    SetForegroundWindow(s_hwnd);
	    पटिनिवत्त 0;
	}
    }

    पटिनिवत्त DefWindowProc(hwnd, सन्देशः, wParam, lParam);
}

/*
 * Initialise the message handling process.  This involves creating a window
 * to handle messages - the window will not be visible.
 */
    शून्य
serverInitMessaging(शून्य)
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
    wndclass.hIcon = रिक्त;
    wndclass.hCursor = रिक्त;
    wndclass.hbrBackground = रिक्त;
    wndclass.lpszMenuName = रिक्त;
    wndclass.lpszClassName = VIM_CLASSNAME;
    RegisterClass(&wndclass);

    // Create the message window. It will be hidden, so the details don't
    // matter.  Don't use WS_OVERLAPPEDWINDOW, it will make a shortcut remove
    // focus from gvim.
    message_window = CreateWindow(VIM_CLASSNAME, "",
			 WS_POPUPWINDOW | WS_CAPTION,
			 CW_USEDEFAULT, CW_USEDEFAULT,
			 100, 100, रिक्त, रिक्त,
			 g_hinst, रिक्त);
}

// Used by serverSendToVim() to find an alternate server name.
स्थिर char_u *altname_buf_ptr = रिक्त;

/*
 * Get the title of the window "hwnd", which is the Vim server name, in
 * "name[namelen]" and return the length.
 * Returns zero if window "hwnd" is not a Vim server.
 */
    स्थिर पूर्णाङ्क
getVimServerName(HWND hwnd, वर्ण *name, पूर्णाङ्क namelen)
{
    पूर्णाङ्क		len;
    वर्ण	buffer[VIM_CLASSNAME_LEN + 1];

    // Ignore windows which aren't Vim message windows
    len = GetClassName(hwnd, buffer, परिमाण(buffer));
    सचे (len != VIM_CLASSNAME_LEN || STRCMP(buffer, VIM_CLASSNAME) != 0)
	पटिनिवत्त 0;

    // Get the title of the window
    पटिनिवत्त GetWindowText(hwnd, name, namelen);
}

    स्थिर BOOL CALLBACK
enumWindowsGetServer(HWND hwnd, LPARAM lparam)
{
    संरचना	server_id *id = (संरचना server_id *)lparam;
    वर्ण	server[MAX_PATH];

    // Get the title of the window
    सचे (getVimServerName(hwnd, server, परिमाण(server)) == 0)
	पटिनिवत्त TRUE;

    // If this is the server we're looking for, return its HWND
    सचे (STRICMP(server, id->name) == 0)
    {
	id->hwnd = hwnd;
	पटिनिवत्त FALSE;
    }

    // If we are looking for an alternate server, remember this name.
    सचे (altname_buf_ptr != रिक्त)
    {
	size_t	namelen = STRLEN(id->name);

	सचे (STRNICMP(server, id->name, namelen) == 0
	    && vim_isdigit(server[namelen]))
	{
	    STRCPY(altname_buf_ptr, server);
	    altname_buf_ptr = रिक्त;	    // don't use another name
	}
    }

    // Otherwise, keep looking
    पटिनिवत्त TRUE;
}

    स्थिर BOOL CALLBACK
enumWindowsGetNames(HWND hwnd, LPARAM lparam)
{
    list_T	*list = (list_T *)lparam;
    वर्ण	server[MAX_PATH];

    // Get the title of the window
    सचे (getVimServerName(hwnd, server, परिमाण(server)) == 0)
	पटिनिवत्त TRUE;

    list_append_string(list, (char_u *)server, -1);
    पटिनिवत्त TRUE;
}

संरचना enum_windows_s
{
    WNDENUMPROC lpEnumFunc;
    LPARAM      lParam;
};

    स्थिर BOOL CALLBACK
enum_windows_child(HWND hwnd, LPARAM lParam)
{
    संरचना enum_windows_s *ew = (संरचना enum_windows_s *)lParam;

    पटिनिवत्त (ew->lpEnumFunc)(hwnd, ew->lParam);
}

    स्थिर BOOL CALLBACK
enum_windows_toplevel(HWND hwnd, LPARAM lParam)
{
    संरचना enum_windows_s *ew = (संरचना enum_windows_s *)lParam;

    सचे ((ew->lpEnumFunc)(hwnd, ew->lParam))
	पटिनिवत्त TRUE;
    पटिनिवत्त EnumChildWindows(hwnd, enum_windows_child, lParam);
}

/*
 * Enumerate all windows including children.
 */
    स्थिर BOOL
enum_windows(WNDENUMPROC lpEnumFunc, LPARAM lParam)
{
    संरचना enum_windows_s ew;

    ew.lpEnumFunc = lpEnumFunc;
    ew.lParam = lParam;
    पटिनिवत्त EnumWindows(enum_windows_toplevel, (LPARAM)&ew);
}

    स्थिर HWND
findServer(char_u *name)
{
    संरचना server_id id;

    id.name = name;
    id.hwnd = 0;

    enum_windows(enumWindowsGetServer, (LPARAM)(&id));

    पटिनिवत्त id.hwnd;
}

    शून्य
serverSetName(char_u *name)
{
    size_t	namelen;
    char_u	*ok_name;
    HWND	hwnd = 0;
    पूर्णाङ्क		i = 0;
    char_u	*p;

    // Leave enough space for a 9-digit suffix to ensure uniqueness!
    namelen = STRLEN(name);
    ok_name = alloc(namelen + 10);
    सचे (ok_name == रिक्त)
	पटिनिवत्त;

    STRCPY(ok_name, name);
    p = ok_name + namelen;

    अत्थाय (;;)
    {
	// This is inefficient - we're doing an EnumWindows loop for each
	// possible name. It would be better to grab all names in one go,
	// and scan the list each time...
	hwnd = findServer(ok_name);
	सचे (hwnd == 0)
	    भङ्ग;

	++i;
	सचे (i >= 1000)
	    भङ्ग;

	sprintf((वर्ण *)p, "%d", i);
    }

    सचे (hwnd != 0)
	vim_free(ok_name);
    अञ्ञथा
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
serverGetVimNames(शून्य)
{
    list_T *list = list_alloc();

    सचे (list == रिक्त)
	पटिनिवत्त रिक्त;

    enum_windows(enumWindowsGetNames, (LPARAM)list);

    पटिनिवत्त list;
}

    पूर्णाङ्क
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
    sscanf((वर्ण *)name, SCANF_HEX_LONG_U, &n);
    सचे (n == 0)
	पटिनिवत्त -1;

    target = (HWND)n;
    सचे (!IsWindow(target))
	पटिनिवत्त -1;

    data.dwData = COPYDATA_REPLY;
    data.cbData = (DWORD)STRLEN(reply) + 1;
    data.lpData = reply;

    सचे (serverSendEnc(target) < 0)
	पटिनिवत्त -1;
    सचे (SendMessageTimeout(target, WM_COPYDATA,
		(WPARAM)message_window, (LPARAM)&data,
		SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
	पटिनिवत्त -1;
    पटिनिवत्त dwret ? 0 : -1;
}

    पूर्णाङ्क
serverSendToVim(
    char_u	 *name,			// Where to send.
    char_u	 *cmd,			// What to send.
    char_u	 **result,		// Result of eval'ed expression
    शून्य	 *ptarget,		// HWND of server
    पूर्णाङ्क		 as_व्यञ्जकम्,		// Expression or keys?
    पूर्णाङ्क		 timeout,		// timeout in seconds or zero
    पूर्णाङ्क		 silent)		// don't complain about no server
{
    size_t	namelen;
    HWND	target;
    COPYDATASTRUCT data;
    char_u	*retval = रिक्त;
    पूर्णाङ्क		retcode = 0;
    DWORD_PTR	dwret = 0;
    char_u	altname_buf[MAX_PATH];

    // Execute locally if no display or target is ourselves
    सचे (serverName != रिक्त && STRICMP(name, serverName) == 0)
	पटिनिवत्त sendToLocalVim(cmd, as_व्यञ्जकम्, result);

    // If the server name does not end in a digit then we look for an
    // alternate name.  e.g. when "name" is GVIM then we may find GVIM2.
    namelen = STRLEN(name);
    सचे (namelen > 1 && !vim_isdigit(name[namelen - 1]))
	altname_buf_ptr = altname_buf;
    altname_buf[0] = NUL;
    target = findServer(name);
    altname_buf_ptr = रिक्त;
    सचे (target == 0 && altname_buf[0] != NUL)
	// Use another server name we found.
	target = findServer(altname_buf);

    सचे (target == 0)
    {
	सचे (!silent)
	    semsg(_(e_no_registered_server_named_str), name);
	पटिनिवत्त -1;
    }

    सचे (ptarget)
	*(HWND *)ptarget = target;

    data.dwData = as_व्यञ्जकम् ? COPYDATA_व्यञ्जकम् : COPYDATA_कुञ्चिकाः;
    data.cbData = (DWORD)STRLEN(cmd) + 1;
    data.lpData = cmd;

    सचे (serverSendEnc(target) < 0)
	पटिनिवत्त -1;
    सचे (SendMessageTimeout(target, WM_COPYDATA,
		(WPARAM)message_window, (LPARAM)&data,
		SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
	पटिनिवत्त -1;
    सचे (dwret == 0)
	पटिनिवत्त -1;

    सचे (as_व्यञ्जकम्)
	retval = serverGetReply(target, &retcode, TRUE, TRUE, timeout);

    सचे (result == रिक्त)
	vim_free(retval);
    अञ्ञथा
	*result = retval; // Caller assumes responsibility for freeing

    पटिनिवत्त retcode;
}

/*
 * Bring the server to the foreground.
 */
    शून्य
serverForeground(char_u *name)
{
    HWND	target = findServer(name);

    सचे (target != 0)
	SetForegroundWindow(target);
}

// Replies from server need to be stored until the client picks them up via
// remote_read(). So we maintain a list of server-id/reply pairs.
// Note that there could be multiple replies from one server pending if the
// client is slow picking them up.
// We just store the replies in a simple list. When we remove an entry, we
// move list entries down to fill the gap.
// The server ID is simply the HWND.
प्रकारनिर्देश संरचना
{
    HWND	server;		// server window
    char_u	*reply;		// reply string
    पूर्णाङ्क		व्यञ्जकम्_result;	// 0 for REPLY, 1 for RESULT 2 for error
} reply_T;

स्थिर garray_T reply_list = {0, 0, परिमाण(reply_T), 5, 0};

# define REPLY_ITEM(i) ((reply_T *)(reply_list.ga_data) + (i))
# define REPLY_गणनम् (reply_list.ga_len)

// Flag which is used to wait for a reply
स्थिर पूर्णाङ्क reply_received = 0;

/*
 * Store a reply.  "reply" must be allocated memory (or NULL).
 */
    स्थिर पूर्णाङ्क
save_reply(HWND server, char_u *reply, पूर्णाङ्क व्यञ्जकम्)
{
    reply_T *rep;

    सचे (ga_grow(&reply_list, 1) == FAIL)
	पटिनिवत्त FAIL;

    rep = REPLY_ITEM(REPLY_गणनम्);
    rep->server = server;
    rep->reply = reply;
    rep->व्यञ्जकम्_result = व्यञ्जकम्;
    सचे (rep->reply == रिक्त)
	पटिनिवत्त FAIL;

    ++REPLY_गणनम्;
    reply_received = 1;
    पटिनिवत्त OK;
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
serverGetReply(HWND server, पूर्णाङ्क *व्यञ्जकम्_res, पूर्णाङ्क remove, पूर्णाङ्क wait, पूर्णाङ्क timeout)
{
    पूर्णाङ्क		i;
    char_u	*reply;
    reply_T	*rep;
    पूर्णाङ्क		did_process = FALSE;
    time_t	start;
    time_t	now;

    // When waiting, loop until the message waiting for is received.
    time(&start);
    अत्थाय (;;)
    {
	// Reset this here, in case a message arrives while we are going
	// through the already received messages.
	reply_received = 0;

	अत्थाय (i = 0; i < REPLY_गणनम्; ++i)
	{
	    rep = REPLY_ITEM(i);
	    सचे (rep->server == server
		    && ((rep->व्यञ्जकम्_result != 0) == (व्यञ्जकम्_res != रिक्त)))
	    {
		// Save the values we've found for later
		reply = rep->reply;
		सचे (व्यञ्जकम्_res != रिक्त)
		    *व्यञ्जकम्_res = rep->व्यञ्जकम्_result == 1 ? 0 : -1;

		सचे (remove)
		{
		    // Move the rest of the list down to fill the gap
		    mch_memmove(rep, rep + 1,
				     (REPLY_गणनम् - i - 1) * परिमाण(reply_T));
		    --REPLY_गणनम्;
		}

		// Return the reply to the caller, who takes on responsibility
		// for freeing it if "remove" is TRUE.
		पटिनिवत्त reply;
	    }
	}

	// If we got here, we didn't find a reply. Return immediately if the
	// "wait" parameter isn't set.
	सचे (!wait)
	{
	    // Process pending messages once. Without this, looping on
	    // remote_peek() would never get the reply.
	    सचे (!did_process)
	    {
		did_process = TRUE;
		serverProcessPendingMessages();
		अनुवर्त;
	    }
	    भङ्ग;
	}

	// We need to wait for a reply. Enter a message loop until the
	// "reply_received" flag gets set.

	// Loop until we receive a reply
	याव (reply_received == 0)
	{
# ifdef FEAT_TIMERS
	    // TODO: use the return value to decide how long to wait.
	    check_due_timer();
# endif
	    time(&now);
	    सचे (timeout > 0 && (now - start) >= timeout)
		भङ्ग;

	    // Wait for a SendMessage() call to us.  This could be the reply
	    // we are waiting for.  Use a timeout of a second, to catch the
	    // situation that the server died unexpectedly.
	    सन्देशः_Wait_For_Multiple_Objects(0, रिक्त, TRUE, 1000, QS_ALLINPUT);

	    // If the server has died, give up
	    सचे (!IsWindow(server))
		पटिनिवत्त रिक्त;

	    serverProcessPendingMessages();
	}
    }

    पटिनिवत्त रिक्त;
}

/*
 * Process any messages in the Windows message queue.
 */
    शून्य
serverProcessPendingMessages(शून्य)
{
    सन्देशः_2 सन्देशः;

    याव (PeekMessageW(&सन्देशः, रिक्त, 0, 0, PM_REMOVE))
    {
	TranslateMessage(&सन्देशः);
	DispatchMessageW(&सन्देशः);
    }
}

#endif // FEAT_CLIENTSERVER

#सचे defined(FEAT_GUI) || (defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT))

संरचना charset_pair
{
    string_T	name;
    BYTE	charset;
};

स्थिर संरचना charset_pair
charset_pairs[] =
{
    {STR_LITERAL_INIT("ANSI"),		ANSI_CHARSET},
    {STR_LITERAL_INIT("CHINESEBIG5"),	CHINESEBIG5_CHARSET},
    {STR_LITERAL_INIT("DEFAULT"),	DEFAULT_CHARSET},
    {STR_LITERAL_INIT("HANGEUL"),	HANGEUL_CHARSET},
    {STR_LITERAL_INIT("OEM"),		OEM_CHARSET},
    {STR_LITERAL_INIT("SHIFTJIS"),	SHIFTJIS_CHARSET},
    {STR_LITERAL_INIT("SYMBOL"),	चिह्नम्_CHARSET},
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

संरचना quality_pair
{
    string_T	name;
    DWORD	quality;
};

स्थिर संरचना quality_pair
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
    वर्ण *
charset_id2name(पूर्णाङ्क id)
{
    पूर्णाङ्क	i;

    अत्थाय (i = 0; i < (पूर्णाङ्क)ARRAY_LENGTH(charset_pairs); ++i)
    {
	सचे ((BYTE)id == charset_pairs[i].charset)
	    पटिनिवत्त (वर्ण *)charset_pairs[i].name.string;
    }

    पटिनिवत्त रिक्त;
}

/*
 * Convert a quality ID to a name.
 * Return NULL when not recognized.
 */
    वर्ण *
quality_id2name(DWORD id)
{
    पूर्णाङ्क	i;

    अत्थाय (i = 0; i < (पूर्णाङ्क)ARRAY_LENGTH(quality_pairs); ++i)
    {
	सचे (id == quality_pairs[i].quality)
	    पटिनिवत्त (वर्ण *)quality_pairs[i].name.string;
    }

    पटिनिवत्त रिक्त;
}

// The default font height in 100% scaling (96dpi).
// (-16 in 96dpi equates to roughly 12pt)
# define DEFAULT_FONT_HEIGHT	(-16)

स्थिर ध्रुव LOGFONTW s_lfDefault =
{
    DEFAULT_FONT_HEIGHT,
    0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
    PROOF_QUALITY, FIXED_PITCH | FF_DONTCARE,
    L""	// Default font name will be set later based on current language.
};

// This will be initialized when set_default_logfont() is called first time.
// The value will be based on the system DPI.
पूर्णाङ्क current_font_height = 0;		// also used in gui_w32.c

/*
 * Convert a string representing a point size into pixels. The string should
 * be a positive decimal number, with an optional decimal point (eg, "12", or
 * "10.5"). The pixel value is returned, and a pointer to the next unconverted
 * character is stored in *end. The flag "vertical" says whether this
 * calculation is for a vertical (height) size or a horizontal (width) one.
 */
    स्थिर पूर्णाङ्क
points_to_pixels(WCHAR *str, WCHAR **end, पूर्णाङ्क vertical, long_i pprinter_dc)
{
    पूर्णाङ्क		pixels;
    पूर्णाङ्क		points = 0;
    पूर्णाङ्क		भाजकः = 0;
    HWND	hwnd = (HWND)0;
    HDC		hdc;
    HDC		printer_dc = (HDC)pprinter_dc;

    याव (*str != NUL)
    {
	सचे (*str == L'.' && भाजकः == 0)
	{
	    // Start keeping a divisor, for later
	    भाजकः = 1;
	}
	अञ्ञथा
	{
	    सचे (!VIM_ISDIGIT(*str))
		भङ्ग;

	    points *= 10;
	    points += *str - L'0';
	    भाजकः *= 10;
	}
	++str;
    }

    सचे (भाजकः == 0)
	भाजकः = 1;

    सचे (printer_dc == रिक्त)
    {
	hwnd = GetDesktopWindow();
	hdc = GetWindowDC(hwnd);
    }
    अञ्ञथा
	hdc = printer_dc;

    pixels = MulDiv(points,
		    GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX),
		    72 * भाजकः);

    सचे (printer_dc == रिक्त)
	ReleaseDC(hwnd, hdc);

    *end = str;
    पटिनिवत्त pixels;
}

/*
 * Convert pixel into point size. This is a reverse of points_to_pixels.
 */
    स्थिर द्विगुण
pixels_to_points(पूर्णाङ्क pixels, पूर्णाङ्क vertical, long_i pprinter_dc)
{
    द्विगुण	points = 0;
    HWND	hwnd = (HWND)0;
    HDC		hdc;
    HDC		printer_dc = (HDC)pprinter_dc;

    सचे (printer_dc == रिक्त)
    {
	hwnd = GetDesktopWindow();
	hdc = GetWindowDC(hwnd);
    }
    अञ्ञथा
	hdc = printer_dc;

    points = pixels * 72.0 / GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX);
    सचे (printer_dc == रिक्त)
	ReleaseDC(hwnd, hdc);

    पटिनिवत्त points;
}

    स्थिर पूर्णाङ्क CALLBACK
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
    सचे ((ntm->tmPitchAndFamily & 1) != 0)
	पटिनिवत्त 1;
# endif

    // Remember this LOGFONTW as a "possible"
    *lf = elf->elfLogFont;

    // Terminate the scan as soon as we find an ANSI font
    सचे (lf->lfCharSet == ANSI_CHARSET
	    || lf->lfCharSet == OEM_CHARSET
	    || lf->lfCharSet == DEFAULT_CHARSET)
	पटिनिवत्त 0;

    // Continue the scan - we have a non-ANSI font
    पटिनिवत्त 2;
}

    स्थिर पूर्णाङ्क
init_logfont(LOGFONTW *lf)
{
    पूर्णाङ्क		n;
    HWND	hwnd = GetDesktopWindow();
    HDC		hdc = GetWindowDC(hwnd);

    n = EnumFontFamiliesW(hdc,
			 lf->lfFaceName,
			 (FONTENUMPROCW)font_enumproc,
			 (LPARAM)lf);

    ReleaseDC(hwnd, hdc);

    // If we couldn't find a usable font, return failure
    सचे (n == 1)
	पटिनिवत्त FAIL;

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
    पटिनिवत्त OK;
}

/*
 * Call back for EnumFontFamiliesW in expand_font_enumproc.
 *
 */
    स्थिर पूर्णाङ्क CALLBACK
expand_font_enumproc(
    ENUMLOGFONTW    *elf,
    NEWTEXTMETRICW  *ntm UNUSED,
    DWORD	    type UNUSED,
    LPARAM	    lparam)
{
    LOGFONTW *lf = (LOGFONTW*)elf;

# ifndef FEAT_PROPORTIONAL_FONTS
    // Ignore non-monospace fonts without further ado
    सचे ((ntm->tmPitchAndFamily & 1) != 0)
	पटिनिवत्त 1;
# endif

    // Filter only on ANSI. Otherwise will see a lot of random fonts that we
    // usually don't want.
    सचे (lf->lfCharSet != ANSI_CHARSET)
	पटिनिवत्त 1;

    पूर्णाङ्क (*add_match)(char_u *) = (पूर्णाङ्क (*)(char_u *))lparam;

    WCHAR *faceNameW = lf->lfFaceName;
    char_u *faceName = utf16_to_enc(faceNameW, रिक्त);
    सचे (!faceName)
	पटिनिवत्त 0;

    add_match(faceName);
    vim_free(faceName);

    पटिनिवत्त 1;
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
    शून्य
gui_mch_expand_font(
    optexpand_T	*args,
    शून्य	*param UNUSED,
    पूर्णाङ्क		(*add_match)(char_u *val))
{
    expand_T	    *xp = args->oe_xp;
    सचे (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	वर्ण buf[30];
	पूर्णाङ्क	i;

	// Always fill in with the current font size as first option for
	// convenience. We simply round to the closest integer for simplicity.
	पूर्णाङ्क font_height = (पूर्णाङ्क)round(
		pixels_to_points(-current_font_height, TRUE, (long_i)रिक्त));
	vim_snprintf(buf, ARRAY_LENGTH(buf), "h%d", font_height);
	add_match((char_u *)buf);

	// Note: Keep this in sync with get_logfont(). Don't include 'c' and
	// 'q' as we fill in all the values below.
	स्थिर वर्ण *(p_gfn_win_opt_मानानि[]) = {
	    "h" , "w" , "W" , "b" , "i" , "u" , "s"};
	अत्थाय (i = 0; i < (पूर्णाङ्क)ARRAY_LENGTH(p_gfn_win_opt_मानानि); i++)
	    add_match((char_u *)p_gfn_win_opt_मानानि[i]);

	अत्थाय (i = 0; i < (पूर्णाङ्क)ARRAY_LENGTH(charset_pairs); ++i)
	{
	    vim_snprintf(buf, परिमाण(buf), "c%s", charset_pairs[i].name.string);
	    add_match((char_u *)buf);
	}

	अत्थाय (i = 0; i < (पूर्णाङ्क)ARRAY_LENGTH(quality_pairs); ++i)
	{
	    vim_snprintf(buf, परिमाण(buf), "q%s", quality_pairs[i].name.string);
	    add_match((char_u *)buf);
	}
	पटिनिवत्त;
    }

    HWND	hwnd = GetDesktopWindow();
    HDC		hdc = GetWindowDC(hwnd);

    EnumFontFamiliesW(hdc,
	    रिक्त,
	    (FONTENUMPROCW)expand_font_enumproc,
	    (LPARAM)add_match);

    ReleaseDC(hwnd, hdc);
}

/*
 * Compare a UTF-16 string and an ASCII string literally.
 * Only works all the code points are inside ASCII range.
 */
    स्थिर पूर्णाङ्क
utf16ascncmp(ध्रुव WCHAR *w, ध्रुव वर्ण *p, size_t n)
{
    size_t i;

    अत्थाय (i = 0; i < n; i++)
    {
	सचे (w[i] == 0 || w[i] != p[i])
	    पटिनिवत्त w[i] - p[i];
    }
    पटिनिवत्त 0;
}

/*
 * Equivalent of GetDpiForSystem().
 */
    UINT WINAPI
vimGetDpiForSystem(शून्य)
{
    HWND hwnd = GetDesktopWindow();
    HDC hdc = GetWindowDC(hwnd);
    UINT dpi = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(hwnd, hdc);
    पटिनिवत्त dpi;
}

/*
 * Set default logfont based on current language.
 */
    स्थिर शून्य
set_default_logfont(LOGFONTW *lf)
{
    // Default font name for current language on MS-Windows.
    // If not translated, falls back to "Consolas".
    // This must be a fixed-pitch font.
    ध्रुव वर्ण *defaultfontname = N_("DefaultFontNameForWindows");
    वर्ण *fontname = _(defaultfontname);

    सचे (STRCMP(fontname, defaultfontname) == 0)
	fontname = "Consolas";

    *lf = s_lfDefault;
    lf->lfHeight = DEFAULT_FONT_HEIGHT * (पूर्णाङ्क)vimGetDpiForSystem() / 96;
    सचे (current_font_height == 0)
	current_font_height = lf->lfHeight;

    WCHAR *wfontname = enc_to_utf16((char_u*)fontname, रिक्त);
    सचे (wfontname != रिक्त)
    {
	wcscpy_s(lf->lfFaceName, LF_FACESIZE, wfontname);
	vim_free(wfontname);
    }
}

/*
 * Get font info from "name" into logfont "lf".
 * Return OK for a valid name, FAIL otherwise.
 */
    पूर्णाङ्क
get_logfont(
    LOGFONTW	*lf,
    char_u	*name,
    HDC		printer_dc,
    पूर्णाङ्क		verbose)
{
    WCHAR	*p;
    पूर्णाङ्क		i;
    पूर्णाङ्क		ret = FAIL;
    स्थिर LOGFONTW *lastlf = रिक्त;
    WCHAR	*wname;

    set_default_logfont(lf);
    सचे (name == रिक्त)
	पटिनिवत्त OK;

    wname = enc_to_utf16(name, रिक्त);
    सचे (wname == रिक्त)
	पटिनिवत्त FAIL;

    सचे (wcscmp(wname, L"*") == 0)
    {
# सचे defined(FEAT_GUI_MSWIN)
	CHOOSEFONTW	cf;
	// if name is "*", bring up std font dialog:
	CLEAR_FIELD(cf);
	cf.lStructSize = परिमाण(cf);
	cf.hwndOwner = s_hwnd;
	cf.Flags = CF_SCREENFONTS | CF_FIXEDPITCHONLY | CF_INITTOLOGFONTSTRUCT;
	सचे (lastlf != रिक्त)
	    *lf = *lastlf;
	cf.lpLogFont = lf;
	cf.nFontType = 0 ; //REGULAR_FONTTYPE;
	सचे (ChooseFontW(&cf))
	    ret = OK;
# endif
	गच्छ theend;
    }

    /*
     * Split name up, it could be <name>:h<height>:w<width> etc.
     */
    अत्थाय (p = wname; *p && *p != L':'; p++)
    {
	सचे (p - wname + 1 >= LF_FACESIZE)
	    गच्छ theend;			// Name too long
	lf->lfFaceName[p - wname] = *p;
    }
    सचे (p != wname)
	lf->lfFaceName[p - wname] = NUL;

    // First set defaults
    lf->lfHeight = DEFAULT_FONT_HEIGHT * (पूर्णाङ्क)vimGetDpiForSystem() / 96;
    lf->lfWidth = 0;
    lf->lfWeight = FW_NORMAL;
    lf->lfItalic = FALSE;
    lf->lfUnderline = FALSE;
    lf->lfStrikeOut = FALSE;

    /*
     * If the font can't be found, try replacing '_' by ' '.
     */
    सचे (init_logfont(lf) == FAIL)
    {
	पूर्णाङ्क	did_replace = FALSE;

	अत्थाय (i = 0; lf->lfFaceName[i]; ++i)
	    सचे (lf->lfFaceName[i] == L'_')
	    {
		lf->lfFaceName[i] = L' ';
		did_replace = TRUE;
	    }
	सचे (!did_replace || init_logfont(lf) == FAIL)
	    गच्छ theend;
    }

    याव (*p == L':')
	p++;

    // Set the values found after ':'
    याव (*p)
    {
	विकल्प (*p++)
	{
	    // Note: Keep this in sync with gui_mch_expand_font().
	    पक्ष L'h':
		lf->lfHeight = - points_to_pixels(p, &p, TRUE, (long_i)printer_dc);
		भङ्ग;
	    पक्ष L'w':
		lf->lfWidth = points_to_pixels(p, &p, FALSE, (long_i)printer_dc);
		भङ्ग;
	    पक्ष L'W':
		lf->lfWeight = wcstol(p, &p, 10);
		भङ्ग;
	    पक्ष L'b':
		lf->lfWeight = FW_BOLD;
		भङ्ग;
	    पक्ष L'i':
		lf->lfItalic = TRUE;
		भङ्ग;
	    पक्ष L'u':
		lf->lfUnderline = TRUE;
		भङ्ग;
	    पक्ष L's':
		lf->lfStrikeOut = TRUE;
		भङ्ग;
	    पक्ष L'c':
		अत्थाय (i = 0; i < (पूर्णाङ्क)ARRAY_LENGTH(charset_pairs); ++i)
		{
		    सचे (utf16ascncmp(p, (वर्ण *)charset_pairs[i].name.string,
			    charset_pairs[i].name.length) == 0)
		    {
			lf->lfCharSet = charset_pairs[i].charset;
			p += charset_pairs[i].name.length;
			भङ्ग;
		    }
		}

		सचे (i == (पूर्णाङ्क)ARRAY_LENGTH(charset_pairs) && verbose)
		{
		    char_u *s = utf16_to_enc(p, रिक्त);
		    सचे (s != रिक्त)
		    {
			semsg(_(e_illegal_str_name_str_in_font_name_str),
							   "charset", s, name);
			vim_free(s);
		    }
		}
		भङ्ग;
	    पक्ष L'f':
		// Font features (e.g., "fss19=1").
		// Parsed separately by gui_mch_init_font(); skip here.
		याव (*p && *p != L':')
		    p++;
		भङ्ग;
	    पक्ष L'q':
		अत्थाय (i = 0; i < (पूर्णाङ्क)ARRAY_LENGTH(quality_pairs); ++i)
		{
		    सचे (utf16ascncmp(p, (वर्ण *)quality_pairs[i].name.string,
			    quality_pairs[i].name.length) == 0)
		    {
			lf->lfQuality = quality_pairs[i].quality;
			p += quality_pairs[i].name.length;
			भङ्ग;
		    }
		}

		सचे (i == (पूर्णाङ्क)ARRAY_LENGTH(quality_pairs) && verbose)
		{
		    char_u *s = utf16_to_enc(p, रिक्त);
		    सचे (s != रिक्त)
		    {
			semsg(_(e_illegal_str_name_str_in_font_name_str),
							   "quality", s, name);
			vim_free(s);
		    }
		}
		भङ्ग;
	    स्वभाव:
		सचे (verbose)
		    semsg(_(e_illegal_char_nr_in_font_name_str), p[-1], name);
		गच्छ theend;
	}
	याव (*p == L':')
	    p++;
    }
    ret = OK;

theend:
    // ron: init lastlf
    सचे (ret == OK && printer_dc == रिक्त)
    {
	vim_free(lastlf);
	lastlf = ALLOC_ONE(LOGFONTW);
	सचे (lastlf != रिक्त)
	    mch_memmove(lastlf, lf, परिमाण(LOGFONTW));
    }
    vim_free(wname);

    पटिनिवत्त ret;
}

#endif // defined(FEAT_GUI) || defined(FEAT_PRINTER)

#सचे defined(FEAT_JOB_CHANNEL)
/*
 * Initialize the Winsock dll.
 */
    शून्य
channel_init_winsock(शून्य)
{
    WSADATA wsaData;
    पूर्णाङ्क wsaerr;

    सचे (WSInitialized)
	पटिनिवत्त;

    wsaerr = WSAStartup(MAKEWORD(2, 2), &wsaData);
    सचे (wsaerr == 0)
	WSInitialized = TRUE;
}
#endif
