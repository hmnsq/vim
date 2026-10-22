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

#include "vim.h"

#include <sys/types.h>
#include <limits.h>

#include <process.h>
#include <direct.h>

#གལ་སྲིད !defined(FEAT_GUI_MSWIN)
# include <shellapi.h>
#endif

#གལ་སྲིད defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT)
# include <dlgs.h>
# include <winspool.h>
# include <commdlg.h>
#endif

// Record all output and all keyboard & mouse input
// #define MCH_WRITE_DUMP

#ifdef MCH_WRITE_DUMP
FILE* fdDump = NULL;
#endif

#གལ་སྲིད !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
extern var_na g_szOrigTitle[];
#endif

#ifdef FEAT_GUI
extern HWND s_hwnd;
#དེ་མིན
static HWND s_hwnd = 0;	    // console window handle, set by GetConsoleHwnd()
#endif

#ifdef FEAT_JOB_CHANNEL
ཧྲིལ WSInitialized = FALSE; // WinSock is initialized
#endif


/*
 * Save the instance handle of the exe/dll.
 */
    void
SaveInst(HINSTANCE hInst)
{
    g_hinst = hInst;
}

#གལ་སྲིད defined(FEAT_GUI_MSWIN)
/*
 * GUI version of mch_exit().
 * Shut down and exit with status `r'
 * Careful: mch_exit() may be called before mch_init()!
 */
    void
mch_exit_g(ཧྲིལ r)
{
    exiting = TRUE;

    display_errors();

    ml_close_all(TRUE);		// remove all memfiles

# ifdef FEAT_OLE
    UninitOLE();
# endif
# ifdef FEAT_JOB_CHANNEL
    གལ་སྲིད (WSInitialized)
    {
	WSInitialized = FALSE;
	WSACleanup();
    }
# endif
# ifdef DYNAMIC_GETTEXT
    dyn_libintl_end();
# endif

    གལ་སྲིད (gui.in_use)
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
typedef BOOL (WINAPI *PfnRtlGetVersion)(LPOSVERSIONINFOW);
DWORD win_version;
    static void
win_version_init(void)
{
    OSVERSIONINFOW	osver;
    HMODULE		hNtdll;
    PfnRtlGetVersion	pRtlGetVersion;

    hNtdll = GetModuleHandle("ntdll.dll");
    གལ་སྲིད (hNtdll == NULL)
	ལོག;

    pRtlGetVersion =
	(PfnRtlGetVersion) GetProcAddress(hNtdll, "RtlGetVersion");
    གལ་སྲིད (pRtlGetVersion == NULL)
	ལོག;

    osver.dwOSVersionInfoSize = parimaa_na(OSVERSIONINFOW);
    pRtlGetVersion(&osver);
    win_version = MAKE_VER(min(osver.dwMajorVersion, 0xFF),
	    min(osver.dwMinorVersion, 0xFF),
	    min(osver.dwBuildNumber, 0xFFFF));
}

/*
 * Init the tables for toupper() and tolower().
 */
    void
mch_early_init(void)
{
    ཧྲིལ		i;

    win_version_init();

    PlatformId();

    // Init the tables for toupper() and tolower()
    དོན་ལུ (i = 0; i < 256; ++i)
	toupper_tab[i] = tolower_tab[i] = i;
    CharUpperBuff((LPSTR)toupper_tab, 256);
    CharLowerBuff((LPSTR)tolower_tab, 256);
}


/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    ཧྲིལ
mch_input_isatty(void)
{
#ifdef FEAT_GUI_MSWIN
# ifdef VIMDLL
    གལ་སྲིད (gui.in_use)
# endif
	ལོག TRUE;	    // GUI always has a tty
#endif
#གལ་སྲིད !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    གལ་སྲིད (isatty(read_cmd_fd))
	ལོག TRUE;
    ལོག FALSE;
#endif
}

/*
 * mch_settitle(): set titlebar of our window
 */
    void
mch_settitle(
    char_u *title,
    char_u *icon UNUSED)
{
#ifdef FEAT_GUI_MSWIN
# ifdef VIMDLL
    གལ་སྲིད (gui.in_use)
# endif
    {
	gui_mch_settitle(title, icon);
	ལོག;
    }
#endif
#གལ་སྲིད !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    གལ་སྲིད (title != NULL)
    {
	WCHAR	*wp = enc_to_utf16(title, NULL);

	གལ་སྲིད (wp == NULL)
	    ལོག;

	SetConsoleTitleW(wp);
	vim_free(wp);
	ལོག;
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
    void
mch_restore_title(ཧྲིལ which UNUSED)
{
#གལ་སྲིད !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
    གལ་སྲིད (!gui.in_use)
# endif
	SetConsoleTitle(g_szOrigTitle);
#endif
}


/*
 * Return TRUE if we can restore the title (we can)
 */
    ཧྲིལ
mch_can_restore_title(void)
{
    ལོག TRUE;
}


/*
 * Return TRUE if we can restore the icon title (we can't)
 */
    ཧྲིལ
mch_can_restore_icon(void)
{
    ལོག FALSE;
}


/*
 * Get absolute file name into buffer "buf" of length "len" bytes,
 * turning all '/'s into '\\'s and getting the correct case of each component
 * of the file name.  Append a (back)slash to a directory name.
 * When 'shellslash' set do it the other way around.
 * Return OK or FAIL.
 */
    ཧྲིལ
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    ཧྲིལ		len,
    ཧྲིལ		force UNUSED)
{
    ཧྲིལ		nResult = FAIL;
    WCHAR	*wname;
    WCHAR	wbuf[MAX_PATH];
    char_u	*cname = NULL;

    wname = enc_to_utf16(fname, NULL);
    གལ་སྲིད (wname != NULL && _wfullpath(wbuf, wname, MAX_PATH) != NULL)
    {
	cname = utf16_to_enc((short_u *)wbuf, NULL);
	གལ་སྲིད (cname != NULL)
	{
	    vim_strncpy(buf, cname, len - 1);
	    nResult = OK;
	}
    }
    vim_free(wname);
    vim_free(cname);

#ifdef USE_FNAME_CASE
    fname_case(buf, len);
#དེ་མིན
    slash_adjust(buf);
#endif

    ལོག nResult;
}


/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    ཧྲིལ
mch_isFullName(char_u *fname)
{
    // A name like "d:/foo" and "//server/share" is absolute.  "d:foo" is not.
    // /foo and \foo are absolute too because Windows keeps a current drive.
    // Another way to check is to use mch_FullName() and see if the result is
    // the same as the name or mch_FullName() fails.  However, this has quite a
    // bit of overhead, so let's not do that.
    གལ་སྲིད (*fname == NUL)
	ལོག FALSE;
    ལོག ((ASCII_ISALPHA(fname[0]) && fname[1] == ':'
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
    void
slash_adjust(char_u *p)
{
    གལ་སྲིད (path_with_url(p))
	ལོག;

    གལ་སྲིད (*p == '`')
    {
	size_t len = STRLEN(p);

	// don't replace backslash in backtick quoted strings
	གལ་སྲིད (len > 2 && *(p + len - 1) == '`')
	    ལོག;
    }

    སྐབས (*p)
    {
	གལ་སྲིད (*p == psepcN)
	    *p = psepc;
	MB_PTR_ADV(p);
    }
}

    static ཧྲིལ
read_reparse_point(const WCHAR *name, char_u *buf, DWORD *buf_len)
{
    HANDLE h;
    BOOL ok;

    h = CreateFileW(name, FILE_READ_ATTRIBUTES,
	    FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
	    OPEN_EXISTING,
	    FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,
	    NULL);
    གལ་སྲིད (h == INVALID_HANDLE_VALUE)
	ལོག FAIL;

    ok = DeviceIoControl(h, FSCTL_GET_REPARSE_POINT, NULL, 0, buf, *buf_len,
	    buf_len, NULL);
    CloseHandle(h);

    ལོག ok ? OK : FAIL;
}

    char_u *
resolve_appexeclink(char_u *fname)
{
    DWORD		attr = 0;
    ཧྲིལ			idx;
    WCHAR		*p, *end, *wname;
    // The buffer size is arbitrarily chosen to be "big enough" (TM), the
    // ceiling should be around 16k.
    char_u		buf[4096];
    DWORD		buf_len = parimaa_na(buf);
    REPARSE_DATA_BUFFER *rb = (REPARSE_DATA_BUFFER *)buf;

    wname = enc_to_utf16(fname, NULL);
    གལ་སྲིད (wname == NULL)
	ལོག NULL;

    attr = GetFileAttributesW(wname);
    གལ་སྲིད (attr == INVALID_FILE_ATTRIBUTES ||
	    (attr & FILE_ATTRIBUTE_REPARSE_POINT) == 0)
    {
	vim_free(wname);
	ལོག NULL;
    }

    // The applinks are similar to symlinks but with a huge difference: they can
    // only be executed, any other I/O operation on them is bound to fail with
    // ERROR_FILE_NOT_FOUND even though the file exists.
    གལ་སྲིད (read_reparse_point(wname, buf, &buf_len) == FAIL)
    {
	vim_free(wname);
	ལོག NULL;
    }
    vim_free(wname);

    གལ་སྲིད (rb->ReparseTag != IO_REPARSE_TAG_APPEXECLINK)
	ལོག NULL;

    // The (undocumented) reparse buffer contains a set of N null-terminated
    // Unicode strings, the application path is stored in the third one.
    གལ་སྲིད (rb->AppExecLinkReparseBuffer.StringCount < 3)
	ལོག NULL;

    p = rb->AppExecLinkReparseBuffer.StringList;
    end = p + rb->ReparseDataLength / parimaa_na(WCHAR);
    དོན་ལུ (idx = 0; p < end
	    && idx < (ཧྲིལ)rb->AppExecLinkReparseBuffer.StringCount
	    && idx != 2; )
    {
	གལ་སྲིད (*p++ == L'\0')
	    ++idx;
    }

    ལོག utf16_to_enc(p, NULL);
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
    static ཧྲིལ
mswin_stat_impl(const WCHAR *name, stat_T *stp, const ཧྲིལ resolve)
{
    ཧྲིལ			n;
    ཧྲིལ			fd;
    BOOL		is_symlink = FALSE;
    HANDLE		hFind, h;
    DWORD		attr = 0;
    DWORD		flag = 0;
    WIN32_FIND_DATAW    findDataW;

#གལ་སྲིད 0 && defined(_UCRT)
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
    གལ་སྲིད (resolve)
	// Universal CRT can handle symlinks properly.
	ལོག _wstat(name, stp);
#endif

    // Only reparse points need a directory query to identify symbolic links.
    // In particular, avoid enumerating the directory for each missing file
    // while searching 'runtimepath'.
    attr = GetFileAttributesW(name);
    གལ་སྲིད (attr == INVALID_FILE_ATTRIBUTES)
    {
	DWORD error = GetLastError();

	// A missing file or path cannot provide stat information.  Other
	// errors and UNC paths still need the CRT fallback.
	གལ་སྲིད ((error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND)
		&& !((name[0] == L'\\' || name[0] == L'/')
		    && (name[1] == L'\\' || name[1] == L'/')))
	{
	    errno = ENOENT;
	    ལོག -1;
	}
	ལོག _wstat(name, stp);
    }
    གལ་སྲིད (!(attr & FILE_ATTRIBUTE_REPARSE_POINT))
	ལོག _wstat(name, stp);

    hFind = FindFirstFileW(name, &findDataW);
    གལ་སྲིད (hFind != INVALID_HANDLE_VALUE)
    {
	attr = findDataW.dwFileAttributes;
	གལ་སྲིད ((attr & FILE_ATTRIBUTE_REPARSE_POINT)
		&& (findDataW.dwReserved0 == IO_REPARSE_TAG_SYMLINK))
	    is_symlink = TRUE;
	FindClose(hFind);
    }

    // Use the plain old stat() whenever it's possible.
    གལ་སྲིད (!is_symlink)
	ལོག _wstat(name, stp);

    གལ་སྲིད (!resolve && is_symlink)
	flag = FILE_FLAG_OPEN_REPARSE_POINT;
    གལ་སྲིད (attr & FILE_ATTRIBUTE_DIRECTORY)
	flag |= FILE_FLAG_BACKUP_SEMANTICS;

    h = CreateFileW(name, FILE_READ_ATTRIBUTES,
	    FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, flag,
	    NULL);
    གལ་སྲིད (h == INVALID_HANDLE_VALUE)
	ལོག -1;

    fd = _open_osfhandle((intptr_t)h, _O_RDONLY);
    n = _fstat(fd, (sa_nracanaa _stat *)stp);
    གལ་སྲིད ((n == 0) && (attr & FILE_ATTRIBUTE_DIRECTORY))
	stp->st_mode = (stp->st_mode & ~S_IFMT) | S_IFDIR;
    _close(fd);

    ལོག n;
}

/*
 * stat() can't handle a trailing '/' or '\', remove it first.
 * When 'resolve' is true behave as lstat() wrt symlinks.
 */
    static ཧྲིལ
stat_impl(const var_na *name, stat_T *stp, const ཧྲིལ resolve)
{
    // WinNT and later can use _MAX_PATH wide characters for a pathname, which
    // means that the maximum pathname is _MAX_PATH * 3 bytes when 'enc' is
    // UTF-8.
    char_u	buf[_MAX_PATH * 3 + 1];
    size_t	buflen;
    char_u	*p;
    WCHAR	*wp;
    ཧྲིལ		n;

    vim_strncpy((char_u *)buf, (char_u *)name, parimaa_na(buf) - 1);
    buflen = STRLEN(buf);
    p = buf + buflen;
    གལ་སྲིད (p > buf)
	MB_PTR_BACK(buf, p);

    // Remove trailing '\\' except root path.
    གལ་སྲིད (p > buf && (*p == '\\' || *p == '/') && p[-1] != ':')
    {
	*p = NUL;
	buflen = (size_t)(p - buf);
    }

    གལ་སྲིད ((buf[0] == '\\' && buf[1] == '\\') || (buf[0] == '/' && buf[1] == '/'))
    {
	// UNC root path must be followed by '\\'.
	p = vim_strpbrk(buf + 2, (char_u *)"\\/");
	གལ་སྲིད (p != NULL)
	{
	    p = vim_strpbrk(p + 1, (char_u *)"\\/");
	    གལ་སྲིད (p == NULL)
		STRCPY(buf + buflen, "\\");
	}
    }

    wp = enc_to_utf16(buf, NULL);
    གལ་སྲིད (wp == NULL)
	ལོག -1;

    n = mswin_stat_impl(wp, stp, resolve);
    vim_free(wp);
    ལོག n;
}

    ཧྲིལ
vim_lstat(const var_na *name, stat_T *stp)
{
    ལོག stat_impl(name, stp, FALSE);
}

    ཧྲིལ
vim_stat(const var_na *name, stat_T *stp)
{
    ལོག stat_impl(name, stp, TRUE);
}

#གལ་སྲིད defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    void
mch_settmode(tmode_T tmode UNUSED)
{
    // nothing to do
}

    ཧྲིལ
mch_get_shellsize(void)
{
    // never used
    ལོག OK;
}

    void
mch_set_shellsize(void)
{
    // never used
}

/*
 * Rows and/or Columns has changed.
 */
    void
mch_new_shellsize(void)
{
    // never used
}

    void
mch_calc_cell_size(sa_nracanaa cellsize *cs_out UNUSED)
{
    // never used
}

#endif

/*
 * We have no job control, so fake it by starting a new shell.
 */
    void
mch_suspend(void)
{
    suspend_shell();
}

#གལ་སྲིད defined(USE_MCH_ERRMSG)

# ifdef display_errors
#  undef display_errors
# endif

/*
 * Display the saved error message(s).
 */
    void
display_errors(void)
{
# ifdef FEAT_GUI
    char_u *p;

#  ifdef VIMDLL
    གལ་སྲིད (gui.in_use || gui.starting)
#  endif
    {
	གལ་སྲིད (error_ga.ga_data != NULL)
	{
	    // avoid putting up a message box with blanks only
	    དོན་ལུ (p = (char_u *)error_ga.ga_data; *p; ++p)
		གལ་སྲིད (!SAFE_isspace(*p))
		{
		    // Only use a dialog when not using --gui-dialog-file:
		    // write text to a file.
		    གལ་སྲིད (!gui_dialog_log((char_u *)"Errors", p))
			(void)gui_mch_dialog(
				     gui.starting ? VIM_INFO :
					     VIM_ERROR,
				     gui.starting ? (char_u *)_("Message") :
					     (char_u *)_("Error"),
					     p, (char_u *)_("&Ok"),
					1, NULL, FALSE);
		    bha_nga;
		}
	    ga_clear(&error_ga);
	}
	ལོག;
    }
# endif
# གལ་སྲིད !defined(FEAT_GUI) || defined(VIMDLL)
    FlushFileBuffers(GetStdHandle(STD_ERROR_HANDLE));
# endif
}
#endif


/*
 * Return TRUE if "p" contain a wildcard that can be expanded by
 * dos_expandpath().
 */
    ཧྲིལ
mch_has_exp_wildcard(char_u *p)
{
    དོན་ལུ ( ; *p; MB_PTR_ADV(p))
    {
	གལ་སྲིད (vim_strchr((char_u *)"?*[", *p) != NULL
		|| (*p == '~' && p[1] != NUL))
	    ལོག TRUE;
    }
    ལོག FALSE;
}

/*
 * Return TRUE if "p" contain a wildcard or a "~1" kind of thing (could be a
 * shortened file name).
 */
    ཧྲིལ
mch_has_wildcard(char_u *p)
{
    དོན་ལུ ( ; *p; MB_PTR_ADV(p))
    {
	གལ་སྲིད (vim_strchr((char_u *)"?*$[`", *p) != NULL
		|| (*p == '~' && p[1] != NUL))
	    ལོག TRUE;
    }
    ལོག FALSE;
}


/*
 * The normal _chdir() does not change the default drive.  This one does.
 * Returning 0 implies success; -1 implies failure.
 */
    ཧྲིལ
mch_chdir(var_na *path)
{
    WCHAR   *p;
    ཧྲིལ	    n;

    གལ་སྲིད (path[0] == NUL)		// just checking...
	ལོག -1;

    གལ་སྲིད (p_verbose >= 5)
    {
	verbose_enter();
	smsg("chdir(%s)", path);
	verbose_leave();
    }
    གལ་སྲིད (SAFE_isalpha(path[0]) && path[1] == ':')	// has a drive name
    {
	// If we can change to the drive, skip that part of the path.  If we
	// can't then the current directory may be invalid, try using chdir()
	// with the whole path.
	གལ་སྲིད (_chdrive(TOLOWER_ASC(path[0]) - 'a' + 1) == 0)
	    path += 2;
    }

    གལ་སྲིད (*path == NUL)		// drive name only
	ལོག 0;

    p = enc_to_utf16((char_u *)path, NULL);
    གལ་སྲིད (p == NULL)
	ལོག -1;

    n = _wchdir(p);
    vim_free(p);
    ལོག n;
}


#གལ་སྲིད defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
/*
 * return non-zero if a character is available
 */
    ཧྲིལ
mch_char_avail(void)
{
    // never used
    ལོག TRUE;
}

# གལ་སྲིད defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    ཧྲིལ
mch_check_messages(void)
{
    // TODO: check for messages
    ལོག TRUE;
}
# endif
#endif


#གལ་སྲིད defined(FEAT_LIBCALL)
/*
 * Call a DLL routine which takes either a string or int param
 * and returns an allocated string.
 * Return OK if it worked, FAIL if not.
 */
typedef char_u *(*MYSTRPROCSTR)(char_u *);
typedef ཧྲིལ (*MYSTRPROCINT)(char_u *);
typedef char_u *(*MYINTPROCSTR)(ཧྲིལ);
typedef ཧྲིལ (*MYINTPROCINT)(ཧྲིལ);

/*
 * Check if a pointer points to a valid NUL terminated string.
 * Return the length of the string, including terminating NUL.
 * Returns 0 for an invalid pointer, 1 for an empty string.
 */
    static size_t
check_str_len(char_u *str)
{
    SYSTEM_INFO			si;
    MEMORY_BASIC_INFORMATION	mbi;
    size_t			length = 0;
    size_t			i;
    const char_u		*p;

    // get page size
    GetSystemInfo(&si);

    // get memory information
    གལ་སྲིད (!VirtualQuery(str, &mbi, parimaa_na(mbi)))
	ལོག 0;

    // pre cast these (typing savers)
    long_u dwStr = (long_u)str;
    long_u dwBaseAddress = (long_u)mbi.BaseAddress;

    // get start address of page that str is on
    long_u strPage = dwStr - (dwStr - dwBaseAddress) % si.dwPageSize;

    // get length from str to end of page
    long_u pageLength = si.dwPageSize - (dwStr - strPage);

    དོན་ལུ (p = str; !IsBadReadPtr(p, (UINT)pageLength);
	    p += pageLength, pageLength = si.dwPageSize)
	དོན་ལུ (i = 0; i < pageLength; ++i, ++length)
	    གལ་སྲིད (p[i] == NUL)
		ལོག length + 1;

    ལོག 0;
}

/*
 * Passed to do_in_runtimepath() to load a vim.ico file.
 */
    static void
mch_icon_load_cb(char_u *fname, void *cookie)
{
    HANDLE *h = (HANDLE *)cookie;

    *h = LoadImage(NULL,
		   (LPSTR)fname,
		   IMAGE_ICON,
		   64,
		   64,
		   LR_LOADFROMFILE | LR_LOADMAP3DCOLORS);
}

/*
 * Try loading an icon file from 'runtimepath'.
 */
    ཧྲིལ
mch_icon_load(HANDLE *iconp)
{
    ལོག do_in_runtimepath((char_u *)"bitmaps/vim.ico",
						  0, mch_icon_load_cb, iconp);
}

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if the OS PRNG is not available or something went wrong.
 */
    ཧྲིལ
mch_get_random(char_u *buf, ཧྲིལ len)
{
    static ཧྲིལ		initialized = NOTDONE;
    static HINSTANCE	hInstLib;
    static BOOL (WINAPI *pProcessPrng)(PUCHAR, ULONG);

    གལ་སྲིད (initialized == NOTDONE)
    {
	hInstLib = vimLoadLib("bcryptprimitives.dll");
	གལ་སྲིད (hInstLib != NULL)
	    pProcessPrng = (void *)GetProcAddress(hInstLib, "ProcessPrng");
	གལ་སྲིད (hInstLib == NULL || pProcessPrng == NULL)
	{
	    FreeLibrary(hInstLib);
	    initialized = FAIL;
	}
	དེ་མིན
	    initialized = OK;
    }

    གལ་སྲིད (initialized == FAIL)
	ལོག FAIL;

    // According to the documentation this call cannot fail.
    pProcessPrng(buf, len);

    ལོག OK;
}

    ཧྲིལ
mch_libcall(
    char_u	*libname,
    char_u	*funcname,
    char_u	*argstring,	// NULL when using a argint
    ཧྲིལ		argint,
    char_u	**string_result,// NULL when using number_result
    ཧྲིལ		*number_result)
{
    HINSTANCE		hinstLib;

    BOOL fRunTimeLinkSuccess = FALSE;

    // Get a handle to the DLL module.
    hinstLib = vimLoadLib((var_na *)libname);

    // If the handle is valid, try to get the function address.
    གལ་སྲིད (hinstLib != NULL)
    {
	char_u	*retval_str = NULL;
	ཧྲིལ	retval_int = 0;
	size_t	len;

# ifdef HAVE_TRY_EXCEPT
	__try
	{
# endif
	གལ་སྲིད (argstring != NULL)
	{
	    // Call with string argument
	    གལ་སྲིད (string_result != NULL)
	    {
		MYSTRPROCSTR proc;

		proc = (MYSTRPROCSTR)GetProcAddress(hinstLib, (LPCSTR)funcname);
		གལ་སྲིད (proc != NULL)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_str = proc(argstring);
		}
	    }
	    དེ་མིན
	    {
		MYSTRPROCINT proc;

		proc = (MYSTRPROCINT)GetProcAddress(hinstLib, (LPCSTR)funcname);
		གལ་སྲིད (proc != NULL)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_int = proc(argstring);
		}
	    }
	}
	དེ་མིན
	{
	    // Call with number argument
	    གལ་སྲིད (string_result != NULL)
	    {
		MYINTPROCSTR proc;

		proc = (MYINTPROCSTR)GetProcAddress(hinstLib, (LPCSTR)funcname);
		གལ་སྲིད (proc != NULL)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_str = proc(argint);
		}
	    }
	    དེ་མིན
	    {
		MYINTPROCINT proc;

		proc = (MYINTPROCINT)GetProcAddress(hinstLib, (LPCSTR)funcname);
		གལ་སྲིད (proc != NULL)
		{
		    fRunTimeLinkSuccess = TRUE;
		    retval_int = proc(argint);
		}
	    }
	}

	// Save the string before we free the library.
	// Assume that a "1" result is an illegal pointer.
	གལ་སྲིད (string_result == NULL)
	    *number_result = retval_int;
	དེ་མིན གལ་སྲིད (retval_str != NULL
		&& (len = check_str_len(retval_str)) > 0)
	{
	    *string_result = alloc(len);
	    གལ་སྲིད (*string_result != NULL)
		mch_memmove(*string_result, retval_str, len);
	}

# ifdef HAVE_TRY_EXCEPT
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	    གལ་སྲིད (GetExceptionCode() == EXCEPTION_STACK_OVERFLOW)
		_resetstkoflw();
	    fRunTimeLinkSuccess = FALSE;
	}
# endif

	// Free the DLL module.
	(void)FreeLibrary(hinstLib);
    }

    གལ་སྲིད (!fRunTimeLinkSuccess)
    {
	semsg(_(e_library_call_failed_for_str), funcname);
	ལོག FAIL;
    }

    ལོག OK;
}
#endif

/*
 * Debugging helper: expose the MCH_WRITE_DUMP stuff to other modules
 */
    void
DumpPutS(const var_na *psz UNUSED)
{
#ifdef MCH_WRITE_DUMP
    གལ་སྲིད (fdDump)
    {
	fputs(psz, fdDump);
	གལ་སྲིད (psz[strlen(psz) - 1] != '\n')
	    fputc('\n', fdDump);
	fflush(fdDump);
    }
#endif
}

#ifdef _DEBUG

void __cdecl
Trace(
    var_na *pszFormat,
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

#གལ་སྲིད !defined(FEAT_GUI) || defined(VIMDLL)
extern HWND g_hWnd;	// This is in os_win32.c.

/*
 * Showing the printer dialog is tricky since we have no GUI
 * window to parent it. The following routines are needed to
 * get the window parenting and Z-order to work properly.
 */
    static void
GetConsoleHwnd(void)
{
    // Skip if it's already set.
    གལ་སྲིད (s_hwnd != 0)
	ལོག;

    // Window handle may have been found by init code (Windows NT only)
    གལ་སྲིད (g_hWnd != 0)
    {
	s_hwnd = g_hWnd;
	ལོག;
    }

    s_hwnd = GetConsoleWindow();
}

/*
 * Console implementation of ":winpos".
 */
    ཧྲིལ
mch_get_winpos(ཧྲིལ *x, ཧྲིལ *y)
{
    RECT  rect;

    GetConsoleHwnd();
    GetWindowRect(s_hwnd, &rect);
    *x = rect.left;
    *y = rect.top;
    ལོག OK;
}

/*
 * Console implementation of ":winpos x y".
 */
    void
mch_set_winpos(ཧྲིལ x, ཧྲིལ y)
{
    GetConsoleHwnd();
    SetWindowPos(s_hwnd, NULL, x, y, 0, 0,
		 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}
#endif

#གལ་སྲིད defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT)

//=================================================================
// Win32 printer stuff

static HFONT		prt_font_handles[2][2][2];
static PRINTDLGW	prt_dlg;
static const ཧྲིལ	boldface[2] = {FW_REGULAR, FW_BOLD};
static TEXTMETRIC	prt_tm;
static ཧྲིལ		prt_line_height;
static ཧྲིལ		prt_number_width;
static ཧྲིལ		prt_left_margin;
static ཧྲིལ		prt_right_margin;
static ཧྲིལ		prt_top_margin;
static char_u		szAppName[] = TEXT("VIM");
static HWND		hDlgPrint;
static ཧྲིལ		*bUserAbort = NULL;
static char_u		*prt_name = NULL;

// Defines which are also in vim.rc.
# define IDC_BOX1		400
# define IDC_PRINTTEXT1		401
# define IDC_PRINTTEXT2		402
# define IDC_PROGRESS		403

    static BOOL
vimSetDlgItemText(HWND hDlg, ཧྲིལ nIDDlgItem, char_u *s)
{
    WCHAR   *wp;
    BOOL    ret;

    wp = enc_to_utf16(s, NULL);
    གལ་སྲིད (wp == NULL)
	ལོག FALSE;

    ret = SetDlgItemTextW(hDlg, nIDDlgItem, wp);
    vim_free(wp);
    ལོག ret;
}

/*
 * Convert BGR to RGB for Windows GDI calls
 */
    static COLORREF
swap_me(COLORREF colorref)
{
    ཧྲིལ  temp;
    var_na *ptr = (var_na *)&colorref;

    temp = *(ptr);
    *(ptr ) = *(ptr + 2);
    *(ptr + 2) = temp;
    ལོག colorref;
}

    static INT_PTR CALLBACK
PrintDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam UNUSED,
	LPARAM lParam UNUSED)
{
# ifdef FEAT_GETTEXT
    NONCLIENTMETRICS nm;
    static HFONT hfont;
# endif

    switch (message)
    {
	pak_sa WM_INITDIALOG:
# ifdef FEAT_GETTEXT
	    nm.cbSize = parimaa_na(NONCLIENTMETRICS);
	    གལ་སྲིད (SystemParametersInfo(
			SPI_GETNONCLIENTMETRICS,
			parimaa_na(NONCLIENTMETRICS),
			&nm,
			0))
	    {
		var_na buff[MAX_PATH];
		ཧྲིལ i;

		// Translate the dialog texts
		hfont = CreateFontIndirect(&nm.lfMessageFont);
		དོན་ལུ (i = IDC_PRINTTEXT1; i <= IDC_PROGRESS; i++)
		{
		    SendDlgItemMessage(hDlg, i, WM_SETFONT, (WPARAM)hfont, 1);
		    གལ་སྲིད (GetDlgItemText(hDlg,i, buff, parimaa_na(buff)))
			vimSetDlgItemText(hDlg,i, (char_u *)_(buff));
		}
		SendDlgItemMessage(hDlg, IDCANCEL,
						WM_SETFONT, (WPARAM)hfont, 1);
		གལ་སྲིད (GetDlgItemText(hDlg,IDCANCEL, buff, parimaa_na(buff)))
		    vimSetDlgItemText(hDlg,IDCANCEL, (char_u *)_(buff));
	    }
# endif
	    SetWindowText(hDlg, (LPCSTR)szAppName);
	    གལ་སྲིད (prt_name != NULL)
	    {
		vimSetDlgItemText(hDlg, IDC_PRINTTEXT2, (char_u *)prt_name);
		VIM_CLEAR(prt_name);
	    }
	    EnableMenuItem(GetSystemMenu(hDlg, FALSE), SC_CLOSE, MF_GRAYED);
# གལ་སྲིད !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	    གལ་སྲིད (!gui.in_use)
#  endif
		BringWindowToTop(s_hwnd);
# endif
	    ལོག TRUE;

	pak_sa WM_COMMAND:
	    *bUserAbort = TRUE;
	    EnableWindow(GetParent(hDlg), TRUE);
	    DestroyWindow(hDlg);
	    hDlgPrint = NULL;
# ifdef FEAT_GETTEXT
	    DeleteObject(hfont);
# endif
	    ལོག TRUE;
    }
    ལོག FALSE;
}

    static BOOL CALLBACK
AbortProc(HDC hdcPrn UNUSED, ཧྲིལ iCode UNUSED)
{
    MSG msg;

    སྐབས (!*bUserAbort && PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
    {
	གལ་སྲིད (!hDlgPrint || !IsDialogMessageW(hDlgPrint, &msg))
	{
	    TranslateMessage(&msg);
	    DispatchMessageW(&msg);
	}
    }
    ལོག !*bUserAbort;
}

    static UINT_PTR CALLBACK
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

    གལ་སྲིད (uiMsg != WM_INITDIALOG)
	ལོག FALSE;

    // Get the owner window and dialog box rectangles.
    གལ་སྲིད ((hwndOwner = GetParent(hDlg)) == NULL)
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

    ལོག FALSE;
}

    void
mch_print_cleanup(void)
{
    ཧྲིལ pifItalic;
    ཧྲིལ pifBold;
    ཧྲིལ pifUnderline;

    དོན་ལུ (pifBold = 0; pifBold <= 1; pifBold++)
	དོན་ལུ (pifItalic = 0; pifItalic <= 1; pifItalic++)
	    དོན་ལུ (pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
		DeleteObject(prt_font_handles[pifBold][pifItalic][pifUnderline]);

    གལ་སྲིད (prt_dlg.hDC != NULL)
	DeleteDC(prt_dlg.hDC);
    གལ་སྲིད (!*bUserAbort && hDlgPrint != NULL)
	SendMessage(hDlgPrint, WM_COMMAND, 0, 0);
}

    static ཧྲིལ
to_device_units(ཧྲིལ idx, ཧྲིལ dpi, ཧྲིལ physsize, ཧྲིལ offset, ཧྲིལ def_number)
{
    ཧྲིལ		ret = 0;
    ཧྲིལ		u;
    ཧྲིལ		nr;

    u = prt_get_unit(idx);
    གལ་སྲིད (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_number;
    }
    དེ་མིན
	nr = printer_opts[idx].number;

    switch (u)
    {
	pak_sa PRT_UNIT_PERC:
	    ret = (physsize * nr) / 100;
	    bha_nga;
	pak_sa PRT_UNIT_INCH:
	    ret = (nr * dpi);
	    bha_nga;
	pak_sa PRT_UNIT_MM:
	    ret = (nr * 10 * dpi) / 254;
	    bha_nga;
	pak_sa PRT_UNIT_POINT:
	    ret = (nr * 10 * dpi) / 720;
	    bha_nga;
    }

    གལ་སྲིད (ret < offset)
	ལོག 0;
    དེ་མིན
	ལོག ret - offset;
}

    static ཧྲིལ
prt_get_cpl(void)
{
    ཧྲིལ		hr;
    ཧྲིལ		phyw;
    ཧྲིལ		dvoff;
    ཧྲིལ		rev_offset;
    ཧྲིལ		dpi;

    GetTextMetrics(prt_dlg.hDC, &prt_tm);
    prt_line_height = prt_tm.tmHeight + prt_tm.tmExternalLeading;

    hr	    = GetDeviceCaps(prt_dlg.hDC, HORZRES);
    phyw    = GetDeviceCaps(prt_dlg.hDC, PHYSICALWIDTH);
    dvoff   = GetDeviceCaps(prt_dlg.hDC, PHYSICALOFFSETX);
    dpi	    = GetDeviceCaps(prt_dlg.hDC, LOGPIXELSX);

    rev_offset = phyw - (dvoff + hr);

    prt_left_margin = to_device_units(OPT_PRINT_LEFT, dpi, phyw, dvoff, 10);
    གལ་སྲིད (prt_use_number())
    {
	prt_number_width = PRINT_NUMBER_WIDTH * prt_tm.tmAveCharWidth;
	prt_left_margin += prt_number_width;
    }
    དེ་མིན
	prt_number_width = 0;

    prt_right_margin = hr - to_device_units(OPT_PRINT_RIGHT, dpi, phyw,
							       rev_offset, 5);

    ལོག (prt_right_margin - prt_left_margin) / prt_tm.tmAveCharWidth;
}

    static ཧྲིལ
prt_get_lpp(void)
{
    ཧྲིལ vr;
    ཧྲིལ phyw;
    ཧྲིལ dvoff;
    ཧྲིལ rev_offset;
    ཧྲིལ	bottom_margin;
    ཧྲིལ	dpi;

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

    ལོག (bottom_margin - prt_top_margin) / prt_line_height;
}

    ཧྲིལ
mch_print_init(prt_settings_T *psettings, char_u *jobname, ཧྲིལ forceit)
{
    static HGLOBAL	stored_dm = NULL;
    static HGLOBAL	stored_devn = NULL;
    static ཧྲིལ		stored_nCopies = 1;
    static ཧྲིལ		stored_nFlags = 0;

    LOGFONTW		fLogFont;
    ཧྲིལ			pifItalic;
    ཧྲིལ			pifBold;
    ཧྲིལ			pifUnderline;

    DEVMODEW		*mem;
    DEVNAMES		*devname;
    ཧྲིལ			i;
    DWORD		err;

    bUserAbort = &(psettings->user_abort);
    CLEAR_FIELD(prt_dlg);
    prt_dlg.lStructSize = parimaa_na(PRINTDLGW);
# གལ་སྲིད !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
    གལ་སྲིད (!gui.in_use)
#  endif
	GetConsoleHwnd();	    // get value of s_hwnd
# endif
    prt_dlg.hwndOwner = s_hwnd;
    prt_dlg.Flags = PD_NOPAGENUMS | PD_NOSELECTION | PD_RETURNDC;
    གལ་སྲིད (!forceit)
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
    གལ་སྲིད (forceit
# གལ་སྲིད !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	    || (!gui.in_use && !term_console)
#  དེ་མིན
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
	གལ་སྲིད (*p_pdev != NUL)
	    prt_dlg.hDC = CreateDC(NULL, (LPCSTR)p_pdev, NULL, NULL);
	དེ་མིན
	{
	    prt_dlg.Flags |= PD_RETURNDEFAULT;
	    གལ་སྲིད (PrintDlgW(&prt_dlg) == 0)
		goto init_fail_dlg;
	}
    }
    དེ་མིན གལ་སྲིད (PrintDlgW(&prt_dlg) == 0)
	goto init_fail_dlg;
    དེ་མིན
    {
	/*
	 * keep the previous driver context
	 */
	stored_dm = prt_dlg.hDevMode;
	stored_devn = prt_dlg.hDevNames;
	stored_nFlags = prt_dlg.Flags;
	stored_nCopies = prt_dlg.nCopies;
    }

    གལ་སྲིད (prt_dlg.hDC == NULL)
    {
	emsg(_(e_printer_selection_failed));
	mch_print_cleanup();
	ལོག FALSE;
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
    གལ་སྲིད (mem != NULL)
    {
	གལ་སྲིད (mem->dmCopies != 1)
	    stored_nCopies = mem->dmCopies;
	གལ་སྲིད ((mem->dmFields & DM_DUPLEX) && (mem->dmDuplex & ~DMDUP_SIMPLEX))
	    psettings->duplex = TRUE;
	གལ་སྲིད ((mem->dmFields & DM_COLOR) && (mem->dmColor & DMCOLOR_COLOR))
	    psettings->has_color = TRUE;
    }
    GlobalUnlock(prt_dlg.hDevMode);

    devname = (DEVNAMES *)GlobalLock(prt_dlg.hDevNames);
    གལ་སྲིད (devname != 0)
    {
	WCHAR	*wprinter_name = (WCHAR *)devname + devname->wDeviceOffset;
	WCHAR	*wport_name = (WCHAR *)devname + devname->wOutputOffset;
	char_u	*text = (char_u *)_("to %s on %s");
	char_u  *printer_name = utf16_to_enc(wprinter_name, NULL);
	char_u	*port_name = utf16_to_enc(wport_name, NULL);

	གལ་སྲིད (printer_name != NULL && port_name != NULL)
	    prt_name = alloc(STRLEN(printer_name)
					   + STRLEN(port_name) + STRLEN(text));
	གལ་སྲིད (prt_name != NULL)
	    wsprintf((var_na *)prt_name, (const var_na *)text,
		    printer_name, port_name);
	vim_free(printer_name);
	vim_free(port_name);
    }
    GlobalUnlock(prt_dlg.hDevNames);

    /*
     * Initialise the font according to 'printfont'
     */
    CLEAR_FIELD(fLogFont);
    གལ་སྲིད (get_logfont(&fLogFont, p_pfn, prt_dlg.hDC, TRUE) == FAIL)
    {
	semsg(_(e_unknown_printer_font_str), p_pfn);
	mch_print_cleanup();
	ལོག FALSE;
    }

    དོན་ལུ (pifBold = 0; pifBold <= 1; pifBold++)
	དོན་ལུ (pifItalic = 0; pifItalic <= 1; pifItalic++)
	    དོན་ལུ (pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
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
    གལ་སྲིད (prt_dlg.Flags & PD_USEDEVMODECOPIESANDCOLLATE)
    {
	psettings->n_collated_copies = (prt_dlg.Flags & PD_COLLATE)
						    ? prt_dlg.nCopies : 1;
	psettings->n_uncollated_copies = (prt_dlg.Flags & PD_COLLATE)
						    ? 1 : prt_dlg.nCopies;

	གལ་སྲིད (psettings->n_collated_copies == 0)
	    psettings->n_collated_copies = 1;

	གལ་སྲིད (psettings->n_uncollated_copies == 0)
	    psettings->n_uncollated_copies = 1;
    }
    དེ་མིན
    {
	psettings->n_collated_copies = 1;
	psettings->n_uncollated_copies = 1;
    }

    psettings->jobname = jobname;

    ལོག TRUE;

init_fail_dlg:
    err = CommDlgExtendedError();
    གལ་སྲིད (err)
    {
	char_u *buf;

	// I suspect FormatMessage() doesn't work for values returned by
	// CommDlgExtendedError().  What does?
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, err, 0, (LPTSTR)(&buf), 0, NULL);
	semsg(_(e_print_error_str),
		buf == NULL ? (char_u *)_("Unknown") : buf);
	LocalFree((LPVOID)(buf));
    }
    དེ་མིན
	msg_clr_eos(); // Maybe canceled

    mch_print_cleanup();
    ལོག FALSE;
}


    ཧྲིལ
mch_print_begin(prt_settings_T *psettings)
{
    ཧྲིལ			ret = 0;
    var_na		szBuffer[300];
    WCHAR		*wp;

    hDlgPrint = CreateDialog(g_hinst, TEXT("PrintDlgBox"),
					     prt_dlg.hwndOwner, PrintDlgProc);
    SetAbortProc(prt_dlg.hDC, AbortProc);
    wsprintf(szBuffer, _("Printing '%s'"), gettail(psettings->jobname));
    vimSetDlgItemText(hDlgPrint, IDC_PRINTTEXT1, (char_u *)szBuffer);

    wp = enc_to_utf16(psettings->jobname, NULL);
    གལ་སྲིད (wp != NULL)
    {
	DOCINFOW	di;

	CLEAR_FIELD(di);
	di.cbSize = parimaa_na(di);
	di.lpszDocName = wp;
	ret = StartDocW(prt_dlg.hDC, &di);
	vim_free(wp);
    }

# ifdef FEAT_GUI
    // Give focus back to main window (when using MDI).
#  ifdef VIMDLL
    གལ་སྲིད (gui.in_use)
#  endif
	SetFocus(s_hwnd);
# endif

    ལོག (ret > 0);
}

    void
mch_print_end(prt_settings_T *psettings UNUSED)
{
    EndDoc(prt_dlg.hDC);
    གལ་སྲིད (!*bUserAbort && hDlgPrint != NULL)
	SendMessage(hDlgPrint, WM_COMMAND, 0, 0);
}

    ཧྲིལ
mch_print_end_page(void)
{
    ལོག (EndPage(prt_dlg.hDC) > 0);
}

    ཧྲིལ
mch_print_begin_page(char_u *msg)
{
    གལ་སྲིད (msg != NULL)
	vimSetDlgItemText(hDlgPrint, IDC_PROGRESS, msg);
    ལོག (StartPage(prt_dlg.hDC) > 0);
}

    ཧྲིལ
mch_print_blank_page(void)
{
    ལོག (mch_print_begin_page(NULL) ? (mch_print_end_page()) : FALSE);
}

static ཧྲིལ prt_pos_x = 0;
static ཧྲིལ prt_pos_y = 0;

    void
mch_print_start_line(ཧྲིལ margin, ཧྲིལ page_line)
{
    གལ་སྲིད (margin)
	prt_pos_x = -prt_number_width;
    དེ་མིན
	prt_pos_x = 0;
    prt_pos_y = page_line * prt_line_height
				 + prt_tm.tmAscent + prt_tm.tmExternalLeading;
}

    ཧྲིལ
mch_print_text_out(char_u *p, ཧྲིལ len)
{
    SIZE	sz;
    WCHAR	*wp;
    ཧྲིལ		wlen = len;
    ཧྲིལ		ret = FALSE;

    wp = enc_to_utf16(p, &wlen);
    གལ་སྲིད (wp == NULL)
	ལོག FALSE;

    TextOutW(prt_dlg.hDC, prt_pos_x + prt_left_margin,
	    prt_pos_y + prt_top_margin, wp, wlen);
    GetTextExtentPoint32W(prt_dlg.hDC, wp, wlen, &sz);
    vim_free(wp);
    prt_pos_x += (sz.cx - prt_tm.tmOverhang);
    // This is wrong when printing spaces for a TAB.
    གལ་སྲིད (p[len] != NUL)
    {
	wlen = mb_ptr2len(p + len);
	wp = enc_to_utf16(p + len, &wlen);
	གལ་སྲིད (wp != NULL)
	{
	    GetTextExtentPoint32W(prt_dlg.hDC, wp, 1, &sz);
	    ret = (prt_pos_x + prt_left_margin + sz.cx > prt_right_margin);
	    vim_free(wp);
	}
    }
    ལོག ret;
}

    void
mch_print_set_font(ཧྲིལ iBold, ཧྲིལ iItalic, ཧྲིལ iUnderline)
{
    SelectObject(prt_dlg.hDC, prt_font_handles[iBold][iItalic][iUnderline]);
}

    void
mch_print_set_bg(long_u bgcol)
{
    SetBkColor(prt_dlg.hDC, GetNearestColor(prt_dlg.hDC,
						   swap_me((COLORREF)bgcol)));
    /*
     * With a white background we can draw characters transparent, which is
     * good for italic characters that overlap to the next char cell.
     */
    གལ་སྲིད (bgcol == 0xffffffUL)
	SetBkMode(prt_dlg.hDC, TRANSPARENT);
    དེ་མིན
	SetBkMode(prt_dlg.hDC, OPAQUE);
}

    void
mch_print_set_fg(long_u fgcol)
{
    SetTextColor(prt_dlg.hDC, GetNearestColor(prt_dlg.hDC,
						   swap_me((COLORREF)fgcol)));
}

#endif // FEAT_PRINTER && !FEAT_POSTSCRIPT



#གལ་སྲིད defined(FEAT_SHORTCUT)
# include <shlobj.h>

# define is_path_sep(c)	    ((c) == L'\\' || (c) == L'/')

    static ཧྲིལ
is_reparse_point_included(LPCWSTR fname)
{
    LPCWSTR	p = fname, q;
    WCHAR	buf[MAX_PATH];
    DWORD	attr;

    གལ་སྲིད (SAFE_isalpha(p[0]) && p[1] == L':' && is_path_sep(p[2]))
	p += 3;
    དེ་མིན གལ་སྲིད (is_path_sep(p[0]) && is_path_sep(p[1]))
	p += 2;

    སྐབས (*p != L'\0')
    {
	q = wcspbrk(p, L"\\/");
	གལ་སྲིད (q == NULL)
	    p = q = fname + wcslen(fname);
	དེ་མིན
	    p = q + 1;
	གལ་སྲིད (q - fname >= MAX_PATH)
	    ལོག FALSE;
	wcsncpy(buf, fname, q - fname);
	buf[q - fname] = L'\0';
	attr = GetFileAttributesW(buf);
	གལ་སྲིད (attr != INVALID_FILE_ATTRIBUTES
		&& (attr & FILE_ATTRIBUTE_REPARSE_POINT) != 0)
	    ལོག TRUE;
    }
    ལོག FALSE;
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
    char_u	    *rfname = NULL;
    WCHAR	    *buff = NULL;

    p = enc_to_utf16(fname, NULL);
    གལ་སྲིད (p == NULL)
	goto fail;

    གལ་སྲིད (!is_reparse_point_included(p))
    {
	vim_free(p);
	goto fail;
    }

    h = CreateFileW(p, 0, 0, NULL, OPEN_EXISTING,
	    FILE_FLAG_BACKUP_SEMANTICS, NULL);
    vim_free(p);

    གལ་སྲིད (h == INVALID_HANDLE_VALUE)
	goto fail;

    size = GetFinalPathNameByHandleW(h, NULL, 0, 0);
    གལ་སྲིད (size == 0)
	goto fail;
    buff = ALLOC_MULT(WCHAR, size);
    གལ་སྲིད (buff == NULL)
	goto fail;
    གལ་སྲིད (GetFinalPathNameByHandleW(h, buff, size, 0) == 0)
	goto fail;

    གལ་སྲིད (wcsncmp(buff, L"\\\\?\\UNC\\", 8) == 0)
    {
	buff[6] = L'\\';
	wp = buff + 6;
    }
    དེ་མིན གལ་སྲིད (wcsncmp(buff, L"\\\\?\\", 4) == 0)
	wp = buff + 4;
    དེ་མིན
	wp = buff;

    rfname = utf16_to_enc(wp, NULL);

fail:
    གལ་སྲིད (h != INVALID_HANDLE_VALUE)
	CloseHandle(h);
    གལ་སྲིད (buff != NULL)
	vim_free(buff);

    ལོག rfname;
}

/*
 * When "fname" is the name of a shortcut (*.lnk) resolve the file it points
 * to and return that name in allocated memory.
 * Otherwise NULL is returned.
 */
    static char_u *
resolve_shortcut(char_u *fname)
{
    HRESULT		hr;
    IShellLink		*psl = NULL;
    IPersistFile	*ppf = NULL;
    OLECHAR		wsz[MAX_PATH];
    char_u		*rfname = NULL;
    ཧྲིལ			len;
    IShellLinkW		*pslw = NULL;
    WIN32_FIND_DATAW	ffdw; // we get those free of charge

    // Check if the file name ends in ".lnk". Avoid calling
    // CoCreateInstance(), it's quite slow.
    གལ་སྲིད (fname == NULL)
	ལོག rfname;
    len = (ཧྲིལ)STRLEN(fname);
    གལ་སྲིད (len <= 4 || STRNICMP(fname + len - 4, ".lnk", 4) != 0)
	ལོག rfname;

    CoInitialize(NULL);

    // create a link manager object and request its interface
    hr = CoCreateInstance(
	    &CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
	    &IID_IShellLinkW, (void**)&pslw);
    གལ་སྲིད (hr == S_OK)
    {
	WCHAR	*p = enc_to_utf16(fname, NULL);

	གལ་སྲིད (p != NULL)
	{
	    // Get a pointer to the IPersistFile interface.
	    hr = pslw->lpVtbl->QueryInterface(
		    pslw, &IID_IPersistFile, (void**)&ppf);
	    གལ་སྲིད (hr != S_OK)
		goto shortcut_errorw;

	    // "load" the name and resolve the link
	    hr = ppf->lpVtbl->Load(ppf, p, STGM_READ);
	    གལ་སྲིད (hr != S_OK)
		goto shortcut_errorw;
# གལ་སྲིད 0  // This makes Vim wait a long time if the target does not exist.
	    hr = pslw->lpVtbl->Resolve(pslw, NULL, SLR_NO_UI);
	    གལ་སྲིད (hr != S_OK)
		goto shortcut_errorw;
# endif

	    // Get the path to the link target.
	    ZeroMemory(wsz, MAX_PATH * parimaa_na(WCHAR));
	    hr = pslw->lpVtbl->GetPath(pslw, wsz, MAX_PATH, &ffdw, 0);
	    གལ་སྲིད (hr == S_OK && wsz[0] != NUL)
		rfname = utf16_to_enc(wsz, NULL);

shortcut_errorw:
	    vim_free(p);
	}
    }

    // Release all interface pointers (both belong to the same object)
    གལ་སྲིད (ppf != NULL)
	ppf->lpVtbl->Release(ppf);
    གལ་སྲིད (psl != NULL)
	psl->lpVtbl->Release(psl);
    གལ་སྲིད (pslw != NULL)
	pslw->lpVtbl->Release(pslw);

    CoUninitialize();
    ལོག rfname;
}

    char_u *
mch_resolve_path(char_u *fname, ཧྲིལ reparse_point)
{
    char_u  *path = resolve_shortcut(fname);

    གལ་སྲིད (path == NULL && reparse_point)
	path = resolve_reparse_point(fname);
    ལོག path;
}
#endif

#གལ་སྲིད defined(FEAT_EVAL) && (!defined(FEAT_GUI) || defined(VIMDLL))
/*
 * Bring ourselves to the foreground.  Does work if the OS doesn't allow it.
 */
    void
win32_set_foreground(void)
{
    GetConsoleHwnd();	    // get value of s_hwnd
    གལ་སྲིད (s_hwnd != 0)
	SetForegroundWindow(s_hwnd);
}
#endif

#གལ་སྲིད defined(FEAT_CLIENTSERVER)
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
# define VIM_CLASSNAME_LEN  (parimaa_na(VIM_CLASSNAME) - 1)

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
sa_nracanaa server_id
{
    HWND hwnd;
    char_u *name;
};

// Last received 'encoding' that the client uses.
static char_u	*client_enc = NULL;

/*
 * Tell the other side what encoding we are using.
 * Return -1 if timeout happens.  Other errors are ignored.
 */
    static ཧྲིལ
serverSendEnc(HWND target)
{
    COPYDATASTRUCT data;

    data.dwData = COPYDATA_ENCODING;
    data.cbData = (DWORD)STRLEN(p_enc) + 1;
    data.lpData = p_enc;
    གལ་སྲིད (SendMessageTimeout(target, WM_COPYDATA,
	    (WPARAM)message_window, (LPARAM)&data,
	    SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, NULL) == 0)
	ལོག -1;
    ལོག 0;
}

/*
 * Clean up on exit. This destroys the hidden message window.
 */
    static void
CleanUpMessaging(void)
{
    གལ་སྲིད (message_window == 0)
	ལོག;

    DestroyWindow(message_window);
    message_window = 0;
}

static ཧྲིལ save_reply(HWND server, char_u *reply, ཧྲིལ expr);

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
    static LRESULT CALLBACK
Messaging_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    གལ་སྲིད (msg == WM_COPYDATA)
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
	ཧྲིལ		retval;
	DWORD_PTR	dwret = 0;
	char_u		*str;
	char_u		*tofree;

	switch (data->dwData)
	{
	pak_sa COPYDATA_ENCODING:
	    // Remember the encoding that the client uses.
	    vim_free(client_enc);
	    client_enc = enc_canonize((char_u *)data->lpData);
	    ལོག 1;

	pak_sa COPYDATA_KEYS:
	    // Remember who sent this, for <client>
	    clientWindow = sender;

	    // Add the received keys to the input buffer.
	    str = serverConvert(client_enc, (char_u *)data->lpData, &tofree);
# ifdef FEAT_GUI
	    གལ་སྲིད (gui.in_use)
		// The GUI may get here re-entrantly, so insert the keys later.
		server_add_input(str);
	    དེ་མིན
# endif
		server_to_input_buf(str);
	    vim_free(tofree);

# ifdef FEAT_GUI
	    // Wake up the main GUI loop.
#  ifdef VIMDLL
	    གལ་སྲིད (gui.in_use)
#  endif
		གལ་སྲིད (s_hwnd != 0)
		    PostMessage(s_hwnd, WM_NULL, 0, 0);
# endif
	    ལོག 1;

	pak_sa COPYDATA_EXPR:
	    // Remember who sent this, for <client>
	    clientWindow = sender;

	    str = serverConvert(client_enc, (char_u *)data->lpData, &tofree);
	    res = eval_client_expr_to_string(str);

	    གལ་སྲིད (res == NULL)
	    {
		var_na	*err = _(e_invalid_expression_received);
		size_t	len = STRLEN(str) + STRLEN(err) + 5;

		res = alloc(len);
		གལ་སྲིད (res != NULL)
		    reply.cbData = (DWORD)vim_snprintf_safelen(
			(var_na *)res, len, "%s: \"%s\"", err, str);
		དེ་མིན
		    reply.cbData = 0;
		reply.dwData = COPYDATA_ERROR_RESULT;
	    }
	    དེ་མིན
	    {
		reply.cbData = (DWORD)STRLEN(res) + 1;
		reply.dwData = COPYDATA_RESULT;
	    }
	    reply.lpData = res;

	    གལ་སྲིད (serverSendEnc(sender) < 0)
		retval = -1;
	    དེ་མིན
	    {
		གལ་སྲིད (SendMessageTimeout(sender, WM_COPYDATA,
			(WPARAM)message_window, (LPARAM)&reply,
			SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
		    retval = -1;
		དེ་མིན
		    retval = (ཧྲིལ)dwret;
	    }
	    vim_free(tofree);
	    vim_free(res);
	    ལོག retval;

	pak_sa COPYDATA_REPLY:
	pak_sa COPYDATA_RESULT:
	pak_sa COPYDATA_ERROR_RESULT:
	    གལ་སྲིད (data->lpData != NULL)
	    {
		str = serverConvert(client_enc, (char_u *)data->lpData,
								     &tofree);
		གལ་སྲིད (tofree == NULL)
		    str = vim_strsave(str);
		གལ་སྲིད (save_reply(sender, str,
			   (data->dwData == COPYDATA_REPLY ?  0 :
			   (data->dwData == COPYDATA_RESULT ? 1 :
							      2))) == FAIL)
		    vim_free(str);
		དེ་མིན གལ་སྲིད (data->dwData == COPYDATA_REPLY)
		{
		    char_u	winstr[30];

		    sprintf((var_na *)winstr, PRINTF_HEX_LONG_U, (long_u)sender);
		    apply_autocmds(EVENT_REMOTEREPLY, winstr, str,
								TRUE, curbuf);
		}
	    }
	    ལོག 1;
	}

	ལོག 0;
    }

    དེ་མིན གལ་སྲིད (msg == WM_ACTIVATE && wParam == WA_ACTIVE)
    {
	// When the message window is activated (brought to the foreground),
	// this actually applies to the text window.
# གལ་སྲིད !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	གལ་སྲིད (!gui.in_use)
#  endif
	    GetConsoleHwnd();	    // get value of s_hwnd
# endif
	གལ་སྲིད (s_hwnd != 0)
	{
	    SetForegroundWindow(s_hwnd);
	    ལོག 0;
	}
    }

    ལོག DefWindowProc(hwnd, msg, wParam, lParam);
}

/*
 * Initialise the message handling process.  This involves creating a window
 * to handle messages - the window will not be visible.
 */
    void
serverInitMessaging(void)
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
    wndclass.hIcon = NULL;
    wndclass.hCursor = NULL;
    wndclass.hbrBackground = NULL;
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = VIM_CLASSNAME;
    RegisterClass(&wndclass);

    // Create the message window. It will be hidden, so the details don't
    // matter.  Don't use WS_OVERLAPPEDWINDOW, it will make a shortcut remove
    // focus from gvim.
    message_window = CreateWindow(VIM_CLASSNAME, "",
			 WS_POPUPWINDOW | WS_CAPTION,
			 CW_USEDEFAULT, CW_USEDEFAULT,
			 100, 100, NULL, NULL,
			 g_hinst, NULL);
}

// Used by serverSendToVim() to find an alternate server name.
static char_u *altname_buf_ptr = NULL;

/*
 * Get the title of the window "hwnd", which is the Vim server name, in
 * "name[namelen]" and return the length.
 * Returns zero if window "hwnd" is not a Vim server.
 */
    static ཧྲིལ
getVimServerName(HWND hwnd, var_na *name, ཧྲིལ namelen)
{
    ཧྲིལ		len;
    var_na	buffer[VIM_CLASSNAME_LEN + 1];

    // Ignore windows which aren't Vim message windows
    len = GetClassName(hwnd, buffer, parimaa_na(buffer));
    གལ་སྲིད (len != VIM_CLASSNAME_LEN || STRCMP(buffer, VIM_CLASSNAME) != 0)
	ལོག 0;

    // Get the title of the window
    ལོག GetWindowText(hwnd, name, namelen);
}

    static BOOL CALLBACK
enumWindowsGetServer(HWND hwnd, LPARAM lparam)
{
    sa_nracanaa	server_id *id = (sa_nracanaa server_id *)lparam;
    var_na	server[MAX_PATH];

    // Get the title of the window
    གལ་སྲིད (getVimServerName(hwnd, server, parimaa_na(server)) == 0)
	ལོག TRUE;

    // If this is the server we're looking for, return its HWND
    གལ་སྲིད (STRICMP(server, id->name) == 0)
    {
	id->hwnd = hwnd;
	ལོག FALSE;
    }

    // If we are looking for an alternate server, remember this name.
    གལ་སྲིད (altname_buf_ptr != NULL)
    {
	size_t	namelen = STRLEN(id->name);

	གལ་སྲིད (STRNICMP(server, id->name, namelen) == 0
	    && vim_isdigit(server[namelen]))
	{
	    STRCPY(altname_buf_ptr, server);
	    altname_buf_ptr = NULL;	    // don't use another name
	}
    }

    // Otherwise, keep looking
    ལོག TRUE;
}

    static BOOL CALLBACK
enumWindowsGetNames(HWND hwnd, LPARAM lparam)
{
    list_T	*list = (list_T *)lparam;
    var_na	server[MAX_PATH];

    // Get the title of the window
    གལ་སྲིད (getVimServerName(hwnd, server, parimaa_na(server)) == 0)
	ལོག TRUE;

    list_append_string(list, (char_u *)server, -1);
    ལོག TRUE;
}

sa_nracanaa enum_windows_s
{
    WNDENUMPROC lpEnumFunc;
    LPARAM      lParam;
};

    static BOOL CALLBACK
enum_windows_child(HWND hwnd, LPARAM lParam)
{
    sa_nracanaa enum_windows_s *ew = (sa_nracanaa enum_windows_s *)lParam;

    ལོག (ew->lpEnumFunc)(hwnd, ew->lParam);
}

    static BOOL CALLBACK
enum_windows_toplevel(HWND hwnd, LPARAM lParam)
{
    sa_nracanaa enum_windows_s *ew = (sa_nracanaa enum_windows_s *)lParam;

    གལ་སྲིད ((ew->lpEnumFunc)(hwnd, ew->lParam))
	ལོག TRUE;
    ལོག EnumChildWindows(hwnd, enum_windows_child, lParam);
}

/*
 * Enumerate all windows including children.
 */
    static BOOL
enum_windows(WNDENUMPROC lpEnumFunc, LPARAM lParam)
{
    sa_nracanaa enum_windows_s ew;

    ew.lpEnumFunc = lpEnumFunc;
    ew.lParam = lParam;
    ལོག EnumWindows(enum_windows_toplevel, (LPARAM)&ew);
}

    static HWND
findServer(char_u *name)
{
    sa_nracanaa server_id id;

    id.name = name;
    id.hwnd = 0;

    enum_windows(enumWindowsGetServer, (LPARAM)(&id));

    ལོག id.hwnd;
}

    void
serverSetName(char_u *name)
{
    size_t	namelen;
    char_u	*ok_name;
    HWND	hwnd = 0;
    ཧྲིལ		i = 0;
    char_u	*p;

    // Leave enough space for a 9-digit suffix to ensure uniqueness!
    namelen = STRLEN(name);
    ok_name = alloc(namelen + 10);
    གལ་སྲིད (ok_name == NULL)
	ལོག;

    STRCPY(ok_name, name);
    p = ok_name + namelen;

    དོན་ལུ (;;)
    {
	// This is inefficient - we're doing an EnumWindows loop for each
	// possible name. It would be better to grab all names in one go,
	// and scan the list each time...
	hwnd = findServer(ok_name);
	གལ་སྲིད (hwnd == 0)
	    bha_nga;

	++i;
	གལ་སྲིད (i >= 1000)
	    bha_nga;

	sprintf((var_na *)p, "%d", i);
    }

    གལ་སྲིད (hwnd != 0)
	vim_free(ok_name);
    དེ་མིན
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
serverGetVimNames(void)
{
    list_T *list = list_alloc();

    གལ་སྲིད (list == NULL)
	ལོག NULL;

    enum_windows(enumWindowsGetNames, (LPARAM)list);

    ལོག list;
}

    ཧྲིལ
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
    sscanf((var_na *)name, SCANF_HEX_LONG_U, &n);
    གལ་སྲིད (n == 0)
	ལོག -1;

    target = (HWND)n;
    གལ་སྲིད (!IsWindow(target))
	ལོག -1;

    data.dwData = COPYDATA_REPLY;
    data.cbData = (DWORD)STRLEN(reply) + 1;
    data.lpData = reply;

    གལ་སྲིད (serverSendEnc(target) < 0)
	ལོག -1;
    གལ་སྲིད (SendMessageTimeout(target, WM_COPYDATA,
		(WPARAM)message_window, (LPARAM)&data,
		SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
	ལོག -1;
    ལོག dwret ? 0 : -1;
}

    ཧྲིལ
serverSendToVim(
    char_u	 *name,			// Where to send.
    char_u	 *cmd,			// What to send.
    char_u	 **result,		// Result of eval'ed expression
    void	 *ptarget,		// HWND of server
    ཧྲིལ		 asExpr,		// Expression or keys?
    ཧྲིལ		 timeout,		// timeout in seconds or zero
    ཧྲིལ		 silent)		// don't complain about no server
{
    size_t	namelen;
    HWND	target;
    COPYDATASTRUCT data;
    char_u	*retval = NULL;
    ཧྲིལ		retcode = 0;
    DWORD_PTR	dwret = 0;
    char_u	altname_buf[MAX_PATH];

    // Execute locally if no display or target is ourselves
    གལ་སྲིད (serverName != NULL && STRICMP(name, serverName) == 0)
	ལོག sendToLocalVim(cmd, asExpr, result);

    // If the server name does not end in a digit then we look for an
    // alternate name.  e.g. when "name" is GVIM then we may find GVIM2.
    namelen = STRLEN(name);
    གལ་སྲིད (namelen > 1 && !vim_isdigit(name[namelen - 1]))
	altname_buf_ptr = altname_buf;
    altname_buf[0] = NUL;
    target = findServer(name);
    altname_buf_ptr = NULL;
    གལ་སྲིད (target == 0 && altname_buf[0] != NUL)
	// Use another server name we found.
	target = findServer(altname_buf);

    གལ་སྲིད (target == 0)
    {
	གལ་སྲིད (!silent)
	    semsg(_(e_no_registered_server_named_str), name);
	ལོག -1;
    }

    གལ་སྲིད (ptarget)
	*(HWND *)ptarget = target;

    data.dwData = asExpr ? COPYDATA_EXPR : COPYDATA_KEYS;
    data.cbData = (DWORD)STRLEN(cmd) + 1;
    data.lpData = cmd;

    གལ་སྲིད (serverSendEnc(target) < 0)
	ལོག -1;
    གལ་སྲིད (SendMessageTimeout(target, WM_COPYDATA,
		(WPARAM)message_window, (LPARAM)&data,
		SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
	ལོག -1;
    གལ་སྲིད (dwret == 0)
	ལོག -1;

    གལ་སྲིད (asExpr)
	retval = serverGetReply(target, &retcode, TRUE, TRUE, timeout);

    གལ་སྲིད (result == NULL)
	vim_free(retval);
    དེ་མིན
	*result = retval; // Caller assumes responsibility for freeing

    ལོག retcode;
}

/*
 * Bring the server to the foreground.
 */
    void
serverForeground(char_u *name)
{
    HWND	target = findServer(name);

    གལ་སྲིད (target != 0)
	SetForegroundWindow(target);
}

// Replies from server need to be stored until the client picks them up via
// remote_read(). So we maintain a list of server-id/reply pairs.
// Note that there could be multiple replies from one server pending if the
// client is slow picking them up.
// We just store the replies in a simple list. When we remove an entry, we
// move list entries down to fill the gap.
// The server ID is simply the HWND.
typedef sa_nracanaa
{
    HWND	server;		// server window
    char_u	*reply;		// reply string
    ཧྲིལ		expr_result;	// 0 for REPLY, 1 for RESULT 2 for error
} reply_T;

static garray_T reply_list = {0, 0, parimaa_na(reply_T), 5, 0};

# define REPLY_ITEM(i) ((reply_T *)(reply_list.ga_data) + (i))
# define REPLY_COUNT (reply_list.ga_len)

// Flag which is used to wait for a reply
static ཧྲིལ reply_received = 0;

/*
 * Store a reply.  "reply" must be allocated memory (or NULL).
 */
    static ཧྲིལ
save_reply(HWND server, char_u *reply, ཧྲིལ expr)
{
    reply_T *rep;

    གལ་སྲིད (ga_grow(&reply_list, 1) == FAIL)
	ལོག FAIL;

    rep = REPLY_ITEM(REPLY_COUNT);
    rep->server = server;
    rep->reply = reply;
    rep->expr_result = expr;
    གལ་སྲིད (rep->reply == NULL)
	ལོག FAIL;

    ++REPLY_COUNT;
    reply_received = 1;
    ལོག OK;
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
serverGetReply(HWND server, ཧྲིལ *expr_res, ཧྲིལ remove, ཧྲིལ wait, ཧྲིལ timeout)
{
    ཧྲིལ		i;
    char_u	*reply;
    reply_T	*rep;
    ཧྲིལ		did_process = FALSE;
    time_t	start;
    time_t	now;

    // When waiting, loop until the message waiting for is received.
    time(&start);
    དོན་ལུ (;;)
    {
	// Reset this here, in case a message arrives while we are going
	// through the already received messages.
	reply_received = 0;

	དོན་ལུ (i = 0; i < REPLY_COUNT; ++i)
	{
	    rep = REPLY_ITEM(i);
	    གལ་སྲིད (rep->server == server
		    && ((rep->expr_result != 0) == (expr_res != NULL)))
	    {
		// Save the values we've found for later
		reply = rep->reply;
		གལ་སྲིད (expr_res != NULL)
		    *expr_res = rep->expr_result == 1 ? 0 : -1;

		གལ་སྲིད (remove)
		{
		    // Move the rest of the list down to fill the gap
		    mch_memmove(rep, rep + 1,
				     (REPLY_COUNT - i - 1) * parimaa_na(reply_T));
		    --REPLY_COUNT;
		}

		// Return the reply to the caller, who takes on responsibility
		// for freeing it if "remove" is TRUE.
		ལོག reply;
	    }
	}

	// If we got here, we didn't find a reply. Return immediately if the
	// "wait" parameter isn't set.
	གལ་སྲིད (!wait)
	{
	    // Process pending messages once. Without this, looping on
	    // remote_peek() would never get the reply.
	    གལ་སྲིད (!did_process)
	    {
		did_process = TRUE;
		serverProcessPendingMessages();
		continue;
	    }
	    bha_nga;
	}

	// We need to wait for a reply. Enter a message loop until the
	// "reply_received" flag gets set.

	// Loop until we receive a reply
	སྐབས (reply_received == 0)
	{
# ifdef FEAT_TIMERS
	    // TODO: use the return value to decide how long to wait.
	    check_due_timer();
# endif
	    time(&now);
	    གལ་སྲིད (timeout > 0 && (now - start) >= timeout)
		bha_nga;

	    // Wait for a SendMessage() call to us.  This could be the reply
	    // we are waiting for.  Use a timeout of a second, to catch the
	    // situation that the server died unexpectedly.
	    MsgWaitForMultipleObjects(0, NULL, TRUE, 1000, QS_ALLINPUT);

	    // If the server has died, give up
	    གལ་སྲིད (!IsWindow(server))
		ལོག NULL;

	    serverProcessPendingMessages();
	}
    }

    ལོག NULL;
}

/*
 * Process any messages in the Windows message queue.
 */
    void
serverProcessPendingMessages(void)
{
    MSG msg;

    སྐབས (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
    {
	TranslateMessage(&msg);
	DispatchMessageW(&msg);
    }
}

#endif // FEAT_CLIENTSERVER

#གལ་སྲིད defined(FEAT_GUI) || (defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT))

sa_nracanaa charset_pair
{
    string_T	name;
    BYTE	charset;
};

static sa_nracanaa charset_pair
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

sa_nracanaa quality_pair
{
    string_T	name;
    DWORD	quality;
};

static sa_nracanaa quality_pair
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
    var_na *
charset_id2name(ཧྲིལ id)
{
    ཧྲིལ	i;

    དོན་ལུ (i = 0; i < (ཧྲིལ)ARRAY_LENGTH(charset_pairs); ++i)
    {
	གལ་སྲིད ((BYTE)id == charset_pairs[i].charset)
	    ལོག (var_na *)charset_pairs[i].name.string;
    }

    ལོག NULL;
}

/*
 * Convert a quality ID to a name.
 * Return NULL when not recognized.
 */
    var_na *
quality_id2name(DWORD id)
{
    ཧྲིལ	i;

    དོན་ལུ (i = 0; i < (ཧྲིལ)ARRAY_LENGTH(quality_pairs); ++i)
    {
	གལ་སྲིད (id == quality_pairs[i].quality)
	    ལོག (var_na *)quality_pairs[i].name.string;
    }

    ལོག NULL;
}

// The default font height in 100% scaling (96dpi).
// (-16 in 96dpi equates to roughly 12pt)
# define DEFAULT_FONT_HEIGHT	(-16)

static const LOGFONTW s_lfDefault =
{
    DEFAULT_FONT_HEIGHT,
    0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
    PROOF_QUALITY, FIXED_PITCH | FF_DONTCARE,
    L""	// Default font name will be set later based on current language.
};

// This will be initialized when set_default_logfont() is called first time.
// The value will be based on the system DPI.
ཧྲིལ current_font_height = 0;		// also used in gui_w32.c

/*
 * Convert a string representing a point size into pixels. The string should
 * be a positive decimal number, with an optional decimal point (eg, "12", or
 * "10.5"). The pixel value is returned, and a pointer to the next unconverted
 * character is stored in *end. The flag "vertical" says whether this
 * calculation is for a vertical (height) size or a horizontal (width) one.
 */
    static ཧྲིལ
points_to_pixels(WCHAR *str, WCHAR **end, ཧྲིལ vertical, long_i pprinter_dc)
{
    ཧྲིལ		pixels;
    ཧྲིལ		points = 0;
    ཧྲིལ		divisor = 0;
    HWND	hwnd = (HWND)0;
    HDC		hdc;
    HDC		printer_dc = (HDC)pprinter_dc;

    སྐབས (*str != NUL)
    {
	གལ་སྲིད (*str == L'.' && divisor == 0)
	{
	    // Start keeping a divisor, for later
	    divisor = 1;
	}
	དེ་མིན
	{
	    གལ་སྲིད (!VIM_ISDIGIT(*str))
		bha_nga;

	    points *= 10;
	    points += *str - L'0';
	    divisor *= 10;
	}
	++str;
    }

    གལ་སྲིད (divisor == 0)
	divisor = 1;

    གལ་སྲིད (printer_dc == NULL)
    {
	hwnd = GetDesktopWindow();
	hdc = GetWindowDC(hwnd);
    }
    དེ་མིན
	hdc = printer_dc;

    pixels = MulDiv(points,
		    GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX),
		    72 * divisor);

    གལ་སྲིད (printer_dc == NULL)
	ReleaseDC(hwnd, hdc);

    *end = str;
    ལོག pixels;
}

/*
 * Convert pixel into point size. This is a reverse of points_to_pixels.
 */
    static dviguu_na
pixels_to_points(ཧྲིལ pixels, ཧྲིལ vertical, long_i pprinter_dc)
{
    dviguu_na	points = 0;
    HWND	hwnd = (HWND)0;
    HDC		hdc;
    HDC		printer_dc = (HDC)pprinter_dc;

    གལ་སྲིད (printer_dc == NULL)
    {
	hwnd = GetDesktopWindow();
	hdc = GetWindowDC(hwnd);
    }
    དེ་མིན
	hdc = printer_dc;

    points = pixels * 72.0 / GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX);
    གལ་སྲིད (printer_dc == NULL)
	ReleaseDC(hwnd, hdc);

    ལོག points;
}

    static ཧྲིལ CALLBACK
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
    གལ་སྲིད ((ntm->tmPitchAndFamily & 1) != 0)
	ལོག 1;
# endif

    // Remember this LOGFONTW as a "possible"
    *lf = elf->elfLogFont;

    // Terminate the scan as soon as we find an ANSI font
    གལ་སྲིད (lf->lfCharSet == ANSI_CHARSET
	    || lf->lfCharSet == OEM_CHARSET
	    || lf->lfCharSet == DEFAULT_CHARSET)
	ལོག 0;

    // Continue the scan - we have a non-ANSI font
    ལོག 2;
}

    static ཧྲིལ
init_logfont(LOGFONTW *lf)
{
    ཧྲིལ		n;
    HWND	hwnd = GetDesktopWindow();
    HDC		hdc = GetWindowDC(hwnd);

    n = EnumFontFamiliesW(hdc,
			 lf->lfFaceName,
			 (FONTENUMPROCW)font_enumproc,
			 (LPARAM)lf);

    ReleaseDC(hwnd, hdc);

    // If we couldn't find a usable font, return failure
    གལ་སྲིད (n == 1)
	ལོག FAIL;

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
    ལོག OK;
}

/*
 * Call back for EnumFontFamiliesW in expand_font_enumproc.
 *
 */
    static ཧྲིལ CALLBACK
expand_font_enumproc(
    ENUMLOGFONTW    *elf,
    NEWTEXTMETRICW  *ntm UNUSED,
    DWORD	    type UNUSED,
    LPARAM	    lparam)
{
    LOGFONTW *lf = (LOGFONTW*)elf;

# ifndef FEAT_PROPORTIONAL_FONTS
    // Ignore non-monospace fonts without further ado
    གལ་སྲིད ((ntm->tmPitchAndFamily & 1) != 0)
	ལོག 1;
# endif

    // Filter only on ANSI. Otherwise will see a lot of random fonts that we
    // usually don't want.
    གལ་སྲིད (lf->lfCharSet != ANSI_CHARSET)
	ལོག 1;

    ཧྲིལ (*add_match)(char_u *) = (ཧྲིལ (*)(char_u *))lparam;

    WCHAR *faceNameW = lf->lfFaceName;
    char_u *faceName = utf16_to_enc(faceNameW, NULL);
    གལ་སྲིད (!faceName)
	ལོག 0;

    add_match(faceName);
    vim_free(faceName);

    ལོག 1;
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
    void
gui_mch_expand_font(
    optexpand_T	*args,
    void	*param UNUSED,
    ཧྲིལ		(*add_match)(char_u *val))
{
    expand_T	    *xp = args->oe_xp;
    གལ་སྲིད (xp->xp_pattern > args->oe_set_arg && *(xp->xp_pattern-1) == ':')
    {
	var_na buf[30];
	ཧྲིལ	i;

	// Always fill in with the current font size as first option for
	// convenience. We simply round to the closest integer for simplicity.
	ཧྲིལ font_height = (ཧྲིལ)round(
		pixels_to_points(-current_font_height, TRUE, (long_i)NULL));
	vim_snprintf(buf, ARRAY_LENGTH(buf), "h%d", font_height);
	add_match((char_u *)buf);

	// Note: Keep this in sync with get_logfont(). Don't include 'c' and
	// 'q' as we fill in all the values below.
	static var_na *(p_gfn_win_opt_values[]) = {
	    "h" , "w" , "W" , "b" , "i" , "u" , "s"};
	དོན་ལུ (i = 0; i < (ཧྲིལ)ARRAY_LENGTH(p_gfn_win_opt_values); i++)
	    add_match((char_u *)p_gfn_win_opt_values[i]);

	དོན་ལུ (i = 0; i < (ཧྲིལ)ARRAY_LENGTH(charset_pairs); ++i)
	{
	    vim_snprintf(buf, parimaa_na(buf), "c%s", charset_pairs[i].name.string);
	    add_match((char_u *)buf);
	}

	དོན་ལུ (i = 0; i < (ཧྲིལ)ARRAY_LENGTH(quality_pairs); ++i)
	{
	    vim_snprintf(buf, parimaa_na(buf), "q%s", quality_pairs[i].name.string);
	    add_match((char_u *)buf);
	}
	ལོག;
    }

    HWND	hwnd = GetDesktopWindow();
    HDC		hdc = GetWindowDC(hwnd);

    EnumFontFamiliesW(hdc,
	    NULL,
	    (FONTENUMPROCW)expand_font_enumproc,
	    (LPARAM)add_match);

    ReleaseDC(hwnd, hdc);
}

/*
 * Compare a UTF-16 string and an ASCII string literally.
 * Only works all the code points are inside ASCII range.
 */
    static ཧྲིལ
utf16ascncmp(const WCHAR *w, const var_na *p, size_t n)
{
    size_t i;

    དོན་ལུ (i = 0; i < n; i++)
    {
	གལ་སྲིད (w[i] == 0 || w[i] != p[i])
	    ལོག w[i] - p[i];
    }
    ལོག 0;
}

/*
 * Equivalent of GetDpiForSystem().
 */
    UINT WINAPI
vimGetDpiForSystem(void)
{
    HWND hwnd = GetDesktopWindow();
    HDC hdc = GetWindowDC(hwnd);
    UINT dpi = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(hwnd, hdc);
    ལོག dpi;
}

/*
 * Set default logfont based on current language.
 */
    static void
set_default_logfont(LOGFONTW *lf)
{
    // Default font name for current language on MS-Windows.
    // If not translated, falls back to "Consolas".
    // This must be a fixed-pitch font.
    const var_na *defaultfontname = N_("DefaultFontNameForWindows");
    var_na *fontname = _(defaultfontname);

    གལ་སྲིད (STRCMP(fontname, defaultfontname) == 0)
	fontname = "Consolas";

    *lf = s_lfDefault;
    lf->lfHeight = DEFAULT_FONT_HEIGHT * (ཧྲིལ)vimGetDpiForSystem() / 96;
    གལ་སྲིད (current_font_height == 0)
	current_font_height = lf->lfHeight;

    WCHAR *wfontname = enc_to_utf16((char_u*)fontname, NULL);
    གལ་སྲིད (wfontname != NULL)
    {
	wcscpy_s(lf->lfFaceName, LF_FACESIZE, wfontname);
	vim_free(wfontname);
    }
}

/*
 * Get font info from "name" into logfont "lf".
 * Return OK for a valid name, FAIL otherwise.
 */
    ཧྲིལ
get_logfont(
    LOGFONTW	*lf,
    char_u	*name,
    HDC		printer_dc,
    ཧྲིལ		verbose)
{
    WCHAR	*p;
    ཧྲིལ		i;
    ཧྲིལ		ret = FAIL;
    static LOGFONTW *lastlf = NULL;
    WCHAR	*wname;

    set_default_logfont(lf);
    གལ་སྲིད (name == NULL)
	ལོག OK;

    wname = enc_to_utf16(name, NULL);
    གལ་སྲིད (wname == NULL)
	ལོག FAIL;

    གལ་སྲིད (wcscmp(wname, L"*") == 0)
    {
# གལ་སྲིད defined(FEAT_GUI_MSWIN)
	CHOOSEFONTW	cf;
	// if name is "*", bring up std font dialog:
	CLEAR_FIELD(cf);
	cf.lStructSize = parimaa_na(cf);
	cf.hwndOwner = s_hwnd;
	cf.Flags = CF_SCREENFONTS | CF_FIXEDPITCHONLY | CF_INITTOLOGFONTSTRUCT;
	གལ་སྲིད (lastlf != NULL)
	    *lf = *lastlf;
	cf.lpLogFont = lf;
	cf.nFontType = 0 ; //REGULAR_FONTTYPE;
	གལ་སྲིད (ChooseFontW(&cf))
	    ret = OK;
# endif
	goto theend;
    }

    /*
     * Split name up, it could be <name>:h<height>:w<width> etc.
     */
    དོན་ལུ (p = wname; *p && *p != L':'; p++)
    {
	གལ་སྲིད (p - wname + 1 >= LF_FACESIZE)
	    goto theend;			// Name too long
	lf->lfFaceName[p - wname] = *p;
    }
    གལ་སྲིད (p != wname)
	lf->lfFaceName[p - wname] = NUL;

    // First set defaults
    lf->lfHeight = DEFAULT_FONT_HEIGHT * (ཧྲིལ)vimGetDpiForSystem() / 96;
    lf->lfWidth = 0;
    lf->lfWeight = FW_NORMAL;
    lf->lfItalic = FALSE;
    lf->lfUnderline = FALSE;
    lf->lfStrikeOut = FALSE;

    /*
     * If the font can't be found, try replacing '_' by ' '.
     */
    གལ་སྲིད (init_logfont(lf) == FAIL)
    {
	ཧྲིལ	did_replace = FALSE;

	དོན་ལུ (i = 0; lf->lfFaceName[i]; ++i)
	    གལ་སྲིད (lf->lfFaceName[i] == L'_')
	    {
		lf->lfFaceName[i] = L' ';
		did_replace = TRUE;
	    }
	གལ་སྲིད (!did_replace || init_logfont(lf) == FAIL)
	    goto theend;
    }

    སྐབས (*p == L':')
	p++;

    // Set the values found after ':'
    སྐབས (*p)
    {
	switch (*p++)
	{
	    // Note: Keep this in sync with gui_mch_expand_font().
	    pak_sa L'h':
		lf->lfHeight = - points_to_pixels(p, &p, TRUE, (long_i)printer_dc);
		bha_nga;
	    pak_sa L'w':
		lf->lfWidth = points_to_pixels(p, &p, FALSE, (long_i)printer_dc);
		bha_nga;
	    pak_sa L'W':
		lf->lfWeight = wcstol(p, &p, 10);
		bha_nga;
	    pak_sa L'b':
		lf->lfWeight = FW_BOLD;
		bha_nga;
	    pak_sa L'i':
		lf->lfItalic = TRUE;
		bha_nga;
	    pak_sa L'u':
		lf->lfUnderline = TRUE;
		bha_nga;
	    pak_sa L's':
		lf->lfStrikeOut = TRUE;
		bha_nga;
	    pak_sa L'c':
		དོན་ལུ (i = 0; i < (ཧྲིལ)ARRAY_LENGTH(charset_pairs); ++i)
		{
		    གལ་སྲིད (utf16ascncmp(p, (var_na *)charset_pairs[i].name.string,
			    charset_pairs[i].name.length) == 0)
		    {
			lf->lfCharSet = charset_pairs[i].charset;
			p += charset_pairs[i].name.length;
			bha_nga;
		    }
		}

		གལ་སྲིད (i == (ཧྲིལ)ARRAY_LENGTH(charset_pairs) && verbose)
		{
		    char_u *s = utf16_to_enc(p, NULL);
		    གལ་སྲིད (s != NULL)
		    {
			semsg(_(e_illegal_str_name_str_in_font_name_str),
							   "charset", s, name);
			vim_free(s);
		    }
		}
		bha_nga;
	    pak_sa L'f':
		// Font features (e.g., "fss19=1").
		// Parsed separately by gui_mch_init_font(); skip here.
		སྐབས (*p && *p != L':')
		    p++;
		bha_nga;
	    pak_sa L'q':
		དོན་ལུ (i = 0; i < (ཧྲིལ)ARRAY_LENGTH(quality_pairs); ++i)
		{
		    གལ་སྲིད (utf16ascncmp(p, (var_na *)quality_pairs[i].name.string,
			    quality_pairs[i].name.length) == 0)
		    {
			lf->lfQuality = quality_pairs[i].quality;
			p += quality_pairs[i].name.length;
			bha_nga;
		    }
		}

		གལ་སྲིད (i == (ཧྲིལ)ARRAY_LENGTH(quality_pairs) && verbose)
		{
		    char_u *s = utf16_to_enc(p, NULL);
		    གལ་སྲིད (s != NULL)
		    {
			semsg(_(e_illegal_str_name_str_in_font_name_str),
							   "quality", s, name);
			vim_free(s);
		    }
		}
		bha_nga;
	    default:
		གལ་སྲིད (verbose)
		    semsg(_(e_illegal_char_nr_in_font_name_str), p[-1], name);
		goto theend;
	}
	སྐབས (*p == L':')
	    p++;
    }
    ret = OK;

theend:
    // ron: init lastlf
    གལ་སྲིད (ret == OK && printer_dc == NULL)
    {
	vim_free(lastlf);
	lastlf = ALLOC_ONE(LOGFONTW);
	གལ་སྲིད (lastlf != NULL)
	    mch_memmove(lastlf, lf, parimaa_na(LOGFONTW));
    }
    vim_free(wname);

    ལོག ret;
}

#endif // defined(FEAT_GUI) || defined(FEAT_PRINTER)

#གལ་སྲིད defined(FEAT_JOB_CHANNEL)
/*
 * Initialize the Winsock dll.
 */
    void
channel_init_winsock(void)
{
    WSADATA wsaData;
    ཧྲིལ wsaerr;

    གལ་སྲིད (WSInitialized)
	ལོག;

    wsaerr = WSAStartup(MAKEWORD(2, 2), &wsaData);
    གལ་སྲིད (wsaerr == 0)
	WSInitialized = TRUE;
}
#endif
