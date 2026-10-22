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

#إذا !defined(FEAT_GUI_MSWIN)
# include <shellapi.h>
#د_که_پای

#إذا defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT)
# include <dlgs.h>
# include <winspool.h>
# include <commdlg.h>
#د_که_پای

// Record all output and all keyboard & mouse input
// #define MCH_WRITE_DUMP

#که_تعریف_شوی MCH_WRITE_DUMP
FILE* fdDump = عدم;
#د_که_پای

#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
خارجي حرف g_szOrigTitle[];
#د_که_پای

#که_تعریف_شوی FEAT_GUI
خارجي HWND s_hwnd;
#وإلا
ساكن HWND s_hwnd = 0;	    // console window handle, set by GetConsoleHwnd()
#د_که_پای

#که_تعریف_شوی FEAT_دنده_CHANNEL
صحيح WSInitialized = FALSE; // WinSock is initialized
#د_که_پای


/*
 * Save the instance handle of the exe/dll.
 */
    فراغ
SaveInst(HINSTANCE hInst)
{
    g_hinst = hInst;
}

#إذا defined(FEAT_GUI_MSWIN)
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
# ifdef FEAT_دنده_CHANNEL
    إذا (WSInitialized)
    {
	WSInitialized = FALSE;
	WSACleanup();
    }
# endif
# ifdef DYNAMIC_GETTEXT
    dyn_libintl_پای_ټکی();
# endif

    إذا (gui.دننه_use)
	gui_exit(r);

# ifdef EXITFREE
    free_all_mem();
# endif

    وتل(r);
}

#د_که_پای // FEAT_GUI_MSWIN


/*
 * Get version number including build number
 */
تعريفنوع BOOL (WINAPI *Pfn_Rtl_Get_نسخه)(LPOSVERSIONINFOW);
DWORD win_نسخه;
    ساكن فراغ
win_نسخه_init(فراغ)
{
    OSVERSIONINFOW	osver;
    HMODULE		hNtdll;
    Pfn_Rtl_Get_نسخه	p_Rtl_Get_نسخه;

    hNtdll = GetModuleHandle("ntdll.dll");
    إذا (hNtdll == عدم)
	أرجع;

    p_Rtl_Get_نسخه =
	(Pfn_Rtl_Get_نسخه) Get_Proc_پته(hNtdll, "RtlGetVersion");
    إذا (p_Rtl_Get_نسخه == عدم)
	أرجع;

    osver.dw_OSVersion_Info_اندازه = حجم(OSVERSIONINFOW);
    p_Rtl_Get_نسخه(&osver);
    win_نسخه = MAKE_VER(min(osver.dw_Major_نسخه, 0xFF),
	    min(osver.dw_Minor_نسخه, 0xFF),
	    min(osver.dw_Build_شمېره, 0xFFFF));
}

/*
 * Init the tables for toupper() and tolower().
 */
    فراغ
mch_early_init(فراغ)
{
    صحيح		i;

    win_نسخه_init();

    PlatformId();

    // Init the tables for toupper() and tolower()
    لكل (i = 0; i < 256; ++i)
	toupper_tab[i] = tolower_tab[i] = i;
    Char_پاسنی_Buff((LPSTR)toupper_tab, 256);
    Char_ښکتنی_Buff((LPSTR)tolower_tab, 256);
}


/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    صحيح
mch_ننوت_isatty(فراغ)
{
#که_تعریف_شوی FEAT_GUI_MSWIN
# ifdef VIMDLL
    إذا (gui.دننه_use)
# endif
	أرجع TRUE;	    // GUI always has a tty
#د_که_پای
#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    إذا (isatty(read_cmd_fd))
	أرجع TRUE;
    أرجع FALSE;
#د_که_پای
}

/*
 * mch_settitle(): set titlebar of our window
 */
    فراغ
mch_settitle(
    char_u *title,
    char_u *icon ناکارول_شوی)
{
#که_تعریف_شوی FEAT_GUI_MSWIN
# ifdef VIMDLL
    إذا (gui.دننه_use)
# endif
    {
	gui_mch_settitle(title, icon);
	أرجع;
    }
#د_که_پای
#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    إذا (title != عدم)
    {
	WCHAR	*wp = enc_منزل_utf16(title, عدم);

	إذا (wp == عدم)
	    أرجع;

	سېټ_Console_Title_W(wp);
	vim_free(wp);
	أرجع;
    }
#د_که_پای
}


/*
 * Restore the window/icon title.
 * which is one of:
 *  SAVE_RESTORE_TITLE: Just restore title
 *  SAVE_RESTORE_ICON:  Just restore icon (which we don't have)
 *  SAVE_RESTORE_BOTH:  Restore title and icon (which we don't have)
 */
    فراغ
mch_restore_title(صحيح which ناکارول_شوی)
{
#إذا !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
    إذا (!gui.دننه_use)
# endif
	سېټ_Console_Title(g_szOrigTitle);
#د_که_پای
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
mch_ډک_Name(
    char_u	*fname,
    char_u	*buf,
    صحيح		len,
    صحيح		force ناکارول_شوی)
{
    صحيح		n_پایله = FAIL;
    WCHAR	*wname;
    WCHAR	wbuf[MAX_لار];
    char_u	*cname = عدم;

    wname = enc_منزل_utf16(fname, عدم);
    إذا (wname != عدم && _wfullpath(wbuf, wname, MAX_لار) != عدم)
    {
	cname = utf16_منزل_enc((short_u *)wbuf, عدم);
	إذا (cname != عدم)
	{
	    vim_strncpy(buf, cname, len - 1);
	    n_پایله = OK;
	}
    }
    vim_free(wname);
    vim_free(cname);

#که_تعریف_شوی USE_FNAME_CASE
    fname_case(buf, len);
#وإلا
    slash_adjust(buf);
#د_که_پای

    أرجع n_پایله;
}


/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    صحيح
mch_is_ډک_Name(char_u *fname)
{
    // A name like "d:/foo" and "//server/share" is absolute.  "d:foo" is not.
    // /foo and \foo are absolute too because Windows keeps a current drive.
    // Another way to check is to use mch_FullName() and see if the result is
    // the same as the name or mch_FullName() fails.  However, this has quite a
    // bit of overhead, so let's not do that.
    إذا (*fname == NUL)
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
    إذا (لار_with_url(p))
	أرجع;

    إذا (*p == '`')
    {
	size_t len = STRLEN(p);

	// don't replace backslash in backtick quoted strings
	إذا (len > 2 && *(p + len - 1) == '`')
	    أرجع;
    }

    طالما (*p)
    {
	إذا (*p == psepcN)
	    *p = psepc;
	MB_PTR_ADV(p);
    }
}

    ساكن صحيح
read_reparse_ټکی(ثابت WCHAR *name, char_u *buf, DWORD *buf_len)
{
    HANDLE h;
    BOOL ok;

    h = CreateFileW(name, FILE_READ_ځانګړتیاوې,
	    FILE_SHARE_READ | FILE_SHARE_WRITE, عدم,
	    OPEN_EXISTING,
	    FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_ټکی,
	    عدم);
    إذا (h == INVALID_HANDLE_ارزښت)
	أرجع FAIL;

    ok = DeviceIoControl(h, FSCTL_GET_REPARSE_ټکی, عدم, 0, buf, *buf_len,
	    buf_len, عدم);
    CloseHandle(h);

    أرجع ok ? OK : FAIL;
}

    char_u *
resolve_appexeclink(char_u *fname)
{
    DWORD		attr = 0;
    صحيح			idx;
    WCHAR		*p, *پای_ټکی, *wname;
    // The buffer size is arbitrarily chosen to be "big enough" (TM), the
    // ceiling should be around 16k.
    char_u		buf[4096];
    DWORD		buf_len = حجم(buf);
    REPARSE_معلومات_بفر *rb = (REPARSE_معلومات_بفر *)buf;

    wname = enc_منزل_utf16(fname, عدم);
    إذا (wname == عدم)
	أرجع عدم;

    attr = Get_File_ځانګړتیاوې_W(wname);
    إذا (attr == INVALID_FILE_ځانګړتیاوې ||
	    (attr & FILE_ATTRIBUTE_REPARSE_ټکی) == 0)
    {
	vim_free(wname);
	أرجع عدم;
    }

    // The applinks are similar to symlinks but with a huge difference: they can
    // only be executed, any other I/O operation on them is bound to fail with
    // ERROR_FILE_NOT_FOUND even though the file exists.
    إذا (read_reparse_ټکی(wname, buf, &buf_len) == FAIL)
    {
	vim_free(wname);
	أرجع عدم;
    }
    vim_free(wname);

    إذا (rb->Reparse_نښان != IO_REPARSE_نښان_APPEXECLINK)
	أرجع عدم;

    // The (undocumented) reparse buffer contains a set of N null-terminated
    // Unicode strings, the application path is stored in the third one.
    إذا (rb->App_Exec_Link_Reparse_بفر.String_شمېر < 3)
	أرجع عدم;

    p = rb->App_Exec_Link_Reparse_بفر.StringList;
    پای_ټکی = p + rb->Reparse_معلومات_اوږدتیا / حجم(WCHAR);
    لكل (idx = 0; p < پای_ټکی
	    && idx < (صحيح)rb->App_Exec_Link_Reparse_بفر.String_شمېر
	    && idx != 2; )
    {
	إذا (*p++ == L'\0')
	    ++idx;
    }

    أرجع utf16_منزل_enc(p, عدم);
}

// Use 64-bit stat functions.
#تعریف_لغوه stat
#تعریف_لغوه _stat
#تعریف_لغوه _wstat
#تعریف_لغوه _fstat
#تعریف_کړه stat _stat64
#تعریف_کړه _stat _stat64
#تعریف_کړه _wstat _wstat64
#تعریف_کړه _fstat _fstat64

/*
 * Implements lstat() and stat() that can handle symlinks properly.
 */
    ساكن صحيح
mswin_stat_impl(ثابت WCHAR *name, stat_T *stp, ثابت صحيح resolve)
{
    صحيح			n;
    صحيح			fd;
    BOOL		is_symlink = FALSE;
    HANDLE		h_پیدا_کړه, h;
    DWORD		attr = 0;
    DWORD		flag = 0;
    WIN32_پیدا_کړه_DATAW    پیدا_کړه_معلومات_W;

#إذا 0 && defined(_UCRT)
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
    إذا (resolve)
	// Universal CRT can handle symlinks properly.
	أرجع _wstat(name, stp);
#د_که_پای

    // Only reparse points need a directory query to identify symbolic links.
    // In particular, avoid enumerating the directory for each missing file
    // while searching 'runtimepath'.
    attr = Get_File_ځانګړتیاوې_W(name);
    إذا (attr == INVALID_FILE_ځانګړتیاوې)
    {
	DWORD error = Get_وروستی_Error();

	// A missing file or path cannot provide stat information.  Other
	// errors and UNC paths still need the CRT fallback.
	إذا ((error == ERROR_FILE_NOT_وموندل || error == ERROR_لار_NOT_وموندل)
		&& !((name[0] == L'\\' || name[0] == L'/')
		    && (name[1] == L'\\' || name[1] == L'/')))
	{
	    errno = ENOENT;
	    أرجع -1;
	}
	أرجع _wstat(name, stp);
    }
    إذا (!(attr & FILE_ATTRIBUTE_REPARSE_ټکی))
	أرجع _wstat(name, stp);

    h_پیدا_کړه = پیدا_کړه_لومړی_File_W(name, &پیدا_کړه_معلومات_W);
    إذا (h_پیدا_کړه != INVALID_HANDLE_ارزښت)
    {
	attr = پیدا_کړه_معلومات_W.dw_File_ځانګړتیاوې;
	إذا ((attr & FILE_ATTRIBUTE_REPARSE_ټکی)
		&& (پیدا_کړه_معلومات_W.dwReserved0 == IO_REPARSE_نښان_SYMLINK))
	    is_symlink = TRUE;
	پیدا_کړه_Close(h_پیدا_کړه);
    }

    // Use the plain old stat() whenever it's possible.
    إذا (!is_symlink)
	أرجع _wstat(name, stp);

    إذا (!resolve && is_symlink)
	flag = FILE_FLAG_OPEN_REPARSE_ټکی;
    إذا (attr & FILE_ATTRIBUTE_DIRECTORY)
	flag |= FILE_FLAG_BACKUP_SEMANTICS;

    h = CreateFileW(name, FILE_READ_ځانګړتیاوې,
	    FILE_SHARE_READ | FILE_SHARE_WRITE, عدم, OPEN_EXISTING, flag,
	    عدم);
    إذا (h == INVALID_HANDLE_ارزښت)
	أرجع -1;

    fd = _open_osfhandle((intptr_t)h, _O_RDONLY);
    n = _fstat(fd, (بنية _stat *)stp);
    إذا ((n == 0) && (attr & FILE_ATTRIBUTE_DIRECTORY))
	stp->st_ډېرپېښ = (stp->st_ډېرپېښ & ~S_IFMT) | S_IFDIR;
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
    char_u	buf[MAX_لار_2 * 3 + 1];
    size_t	buflen;
    char_u	*p;
    WCHAR	*wp;
    صحيح		n;

    vim_strncpy((char_u *)buf, (char_u *)name, حجم(buf) - 1);
    buflen = STRLEN(buf);
    p = buf + buflen;
    إذا (p > buf)
	MB_PTR_شا(buf, p);

    // Remove trailing '\\' except root path.
    إذا (p > buf && (*p == '\\' || *p == '/') && p[-1] != ':')
    {
	*p = NUL;
	buflen = (size_t)(p - buf);
    }

    إذا ((buf[0] == '\\' && buf[1] == '\\') || (buf[0] == '/' && buf[1] == '/'))
    {
	// UNC root path must be followed by '\\'.
	p = vim_strpbrk(buf + 2, (char_u *)"\\/");
	إذا (p != عدم)
	{
	    p = vim_strpbrk(p + 1, (char_u *)"\\/");
	    إذا (p == عدم)
		STRCPY(buf + buflen, "\\");
	}
    }

    wp = enc_منزل_utf16(buf, عدم);
    إذا (wp == عدم)
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

#إذا defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    فراغ
mch_settmode(tmode_T tmode ناکارول_شوی)
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
mch_سېټ_shellsize(فراغ)
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
mch_calc_cell_اندازه(بنية cellsize *cs_بهر ناکارول_شوی)
{
    // never used
}

#د_که_پای

/*
 * We have no job control, so fake it by starting a new shell.
 */
    فراغ
mch_suspend(فراغ)
{
    suspend_shell();
}

#إذا defined(USE_MCH_ERRMSG)

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
    إذا (gui.دننه_use || gui.starting)
#  endif
    {
	إذا (error_ga.ga_معلومات != عدم)
	{
	    // avoid putting up a message box with blanks only
	    لكل (p = (char_u *)error_ga.ga_معلومات; *p; ++p)
		إذا (!خوندي_isspace(*p))
		{
		    // Only use a dialog when not using --gui-dialog-file:
		    // write text to a file.
		    إذا (!gui_dialog_log((char_u *)"Errors", p))
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
# إذا !defined(FEAT_GUI) || defined(VIMDLL)
    FlushFileBuffers(GetStdHandle(STD_ERROR_HANDLE));
# endif
}
#د_که_پای


/*
 * Return TRUE if "p" contain a wildcard that can be expanded by
 * dos_expandpath().
 */
    صحيح
mch_has_exp_wildcard(char_u *p)
{
    لكل ( ; *p; MB_PTR_ADV(p))
    {
	إذا (vim_strchr((char_u *)"?*[", *p) != عدم
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
	إذا (vim_strchr((char_u *)"?*$[`", *p) != عدم
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
mch_chdir(حرف *لار)
{
    WCHAR   *p;
    صحيح	    n;

    إذا (لار[0] == NUL)		// just checking...
	أرجع -1;

    إذا (p_verbose >= 5)
    {
	verbose_enter();
	smsg("chdir(%s)", لار);
	verbose_leave();
    }
    إذا (خوندي_isalpha(لار[0]) && لار[1] == ':')	// has a drive name
    {
	// If we can change to the drive, skip that part of the path.  If we
	// can't then the current directory may be invalid, try using chdir()
	// with the whole path.
	إذا (_chdrive(TOLOWER_ASC(لار[0]) - 'a' + 1) == 0)
	    لار += 2;
    }

    إذا (*لار == NUL)		// drive name only
	أرجع 0;

    p = enc_منزل_utf16((char_u *)لار, عدم);
    إذا (p == عدم)
	أرجع -1;

    n = _wchdir(p);
    vim_free(p);
    أرجع n;
}


#إذا defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
/*
 * return non-zero if a character is available
 */
    صحيح
mch_char_avail(فراغ)
{
    // never used
    أرجع TRUE;
}

# إذا defined(FEAT_TERMINAL)
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
#د_که_پای


#إذا defined(FEAT_LIBCALL)
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
    حافظه_BASIC_INFORMATION	mbi;
    size_t			اوږدتیا = 0;
    size_t			i;
    ثابت char_u		*p;

    // get page size
    GetSystemInfo(&si);

    // get memory information
    إذا (!VirtualQuery(str, &mbi, حجم(mbi)))
	أرجع 0;

    // pre cast these (typing savers)
    long_u dwStr = (long_u)str;
    long_u dw_بنسټ_پته = (long_u)mbi.بنسټ_پته;

    // get start address of page that str is on
    long_u str_پاڼه = dwStr - (dwStr - dw_بنسټ_پته) % si.dw_پاڼه_اندازه;

    // get length from str to end of page
    long_u پاڼه_اوږدتیا = si.dw_پاڼه_اندازه - (dwStr - str_پاڼه);

    لكل (p = str; !IsBadReadPtr(p, (UINT)پاڼه_اوږدتیا);
	    p += پاڼه_اوږدتیا, پاڼه_اوږدتیا = si.dw_پاڼه_اندازه)
	لكل (i = 0; i < پاڼه_اوږدتیا; ++i, ++اوږدتیا)
	    إذا (p[i] == NUL)
		أرجع اوږدتیا + 1;

    أرجع 0;
}

/*
 * Passed to do_in_runtimepath() to load a vim.ico file.
 */
    ساكن فراغ
mch_icon_بارول_cb(char_u *fname, فراغ *cookie)
{
    HANDLE *h = (HANDLE *)cookie;

    *h = بارول_Image(عدم,
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
mch_icon_بارول(HANDLE *iconp)
{
    أرجع do_دننه_runtimepath((char_u *)"bitmaps/vim.ico",
						  0, mch_icon_بارول_cb, iconp);
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

    إذا (initialized == NOTDONE)
    {
	hInstLib = vim_بارول_Lib("bcryptprimitives.dll");
	إذا (hInstLib != عدم)
	    pProcessPrng = (فراغ *)Get_Proc_پته(hInstLib, "ProcessPrng");
	إذا (hInstLib == عدم || pProcessPrng == عدم)
	{
	    FreeLibrary(hInstLib);
	    initialized = FAIL;
	}
	وإلا
	    initialized = OK;
    }

    إذا (initialized == FAIL)
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
    char_u	**string_پایله,// NULL when using number_result
    صحيح		*شمېره_پایله)
{
    HINSTANCE		hinstLib;

    BOOL f_پرلپسې_وخت_Link_Success = FALSE;

    // Get a handle to the DLL module.
    hinstLib = vim_بارول_Lib((حرف *)libname);

    // If the handle is valid, try to get the function address.
    إذا (hinstLib != عدم)
    {
	char_u	*retval_str = عدم;
	صحيح	retval_int = 0;
	size_t	len;

# ifdef HAVE_TRY_EXCEPT
	__try
	{
# endif
	إذا (argstring != عدم)
	{
	    // Call with string argument
	    إذا (string_پایله != عدم)
	    {
		MYSTRPROCSTR proc;

		proc = (MYSTRPROCSTR)Get_Proc_پته(hinstLib, (LPCSTR)funcname);
		إذا (proc != عدم)
		{
		    f_پرلپسې_وخت_Link_Success = TRUE;
		    retval_str = proc(argstring);
		}
	    }
	    وإلا
	    {
		MYSTRPROCINT proc;

		proc = (MYSTRPROCINT)Get_Proc_پته(hinstLib, (LPCSTR)funcname);
		إذا (proc != عدم)
		{
		    f_پرلپسې_وخت_Link_Success = TRUE;
		    retval_int = proc(argstring);
		}
	    }
	}
	وإلا
	{
	    // Call with number argument
	    إذا (string_پایله != عدم)
	    {
		MYINTPROCSTR proc;

		proc = (MYINTPROCSTR)Get_Proc_پته(hinstLib, (LPCSTR)funcname);
		إذا (proc != عدم)
		{
		    f_پرلپسې_وخت_Link_Success = TRUE;
		    retval_str = proc(argint);
		}
	    }
	    وإلا
	    {
		MYINTPROCINT proc;

		proc = (MYINTPROCINT)Get_Proc_پته(hinstLib, (LPCSTR)funcname);
		إذا (proc != عدم)
		{
		    f_پرلپسې_وخت_Link_Success = TRUE;
		    retval_int = proc(argint);
		}
	    }
	}

	// Save the string before we free the library.
	// Assume that a "1" result is an illegal pointer.
	إذا (string_پایله == عدم)
	    *شمېره_پایله = retval_int;
	وإلا إذا (retval_str != عدم
		&& (len = check_str_len(retval_str)) > 0)
	{
	    *string_پایله = alloc(len);
	    إذا (*string_پایله != عدم)
		mch_memmove(*string_پایله, retval_str, len);
	}

# ifdef HAVE_TRY_EXCEPT
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	    إذا (Get_Exception_کوډ() == EXCEPTION_ډېری_ډکېدنه)
		_resetstkoflw();
	    f_پرلپسې_وخت_Link_Success = FALSE;
	}
# endif

	// Free the DLL module.
	(فراغ)FreeLibrary(hinstLib);
    }

    إذا (!f_پرلپسې_وخت_Link_Success)
    {
	semsg(_(e_library_call_failed_for_str), funcname);
	أرجع FAIL;
    }

    أرجع OK;
}
#د_که_پای

/*
 * Debugging helper: expose the MCH_WRITE_DUMP stuff to other modules
 */
    فراغ
DumpPutS(ثابت حرف *psz ناکارول_شوی)
{
#که_تعریف_شوی MCH_WRITE_DUMP
    إذا (fdDump)
    {
	fputs(psz, fdDump);
	إذا (psz[مزي_اوږدوالی(psz) - 1] != '\n')
	    fputc('\n', fdDump);
	fflush(fdDump);
    }
#د_که_پای
}

#که_تعریف_شوی _DEBUG

فراغ __cdecl
Trace(
    حرف *pszFormat,
    ...)
{
    CHAR szBuff[2048];
    va_list دلیلونه;

    va_start(دلیلونه, pszFormat);
    vsprintf(szBuff, pszFormat, دلیلونه);
    va_end(دلیلونه);

    OutputDebugString(szBuff);
}

#د_که_پای //_DEBUG

#إذا !defined(FEAT_GUI) || defined(VIMDLL)
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
    إذا (s_hwnd != 0)
	أرجع;

    // Window handle may have been found by init code (Windows NT only)
    إذا (g_hWnd != 0)
    {
	s_hwnd = g_hWnd;
	أرجع;
    }

    s_hwnd = Get_Console_کړکۍ();
}

/*
 * Console implementation of ":winpos".
 */
    صحيح
mch_get_winpos(صحيح *x, صحيح *y)
{
    RECT  rect;

    GetConsoleHwnd();
    Get_کړکۍ_Rect(s_hwnd, &rect);
    *x = rect.کیڼ_2;
    *y = rect.سر_2;
    أرجع OK;
}

/*
 * Console implementation of ":winpos x y".
 */
    فراغ
mch_سېټ_winpos(صحيح x, صحيح y)
{
    GetConsoleHwnd();
    سېټ_کړکۍ_Pos(s_hwnd, عدم, x, y, 0, 0,
		 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}
#د_که_پای

#إذا defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT)

//=================================================================
// Win32 printer stuff

ساكن HFONT		prt_font_handles[2][2][2];
ساكن PRINTDLGW	prt_dlg;
ساكن ثابت صحيح	boldface[2] = {FW_REGULAR, FW_BOLD};
ساكن TEXTMETRIC	prt_tm;
ساكن صحيح		prt_کرښه_لوړوالی;
ساكن صحيح		prt_شمېره_پلنوالی;
ساكن صحيح		prt_کیڼ_margin;
ساكن صحيح		prt_ښی_margin;
ساكن صحيح		prt_سر_margin;
ساكن char_u		szAppName[] = متن_3("VIM");
ساكن HWND		hDlgPrint;
ساكن صحيح		*bUserAbort = عدم;
ساكن char_u		*prt_name = عدم;

// Defines which are also in vim.rc.
# define IDC_BOX1		400
# define IDC_PRINTTEXT1		401
# define IDC_PRINTTEXT2		402
# define IDC_پرمختګ		403

    ساكن BOOL
vim_سېټ_Dlg_توکی_متن(HWND hDlg, صحيح n_IDDlg_توکی, char_u *s)
{
    WCHAR   *wp;
    BOOL    ret;

    wp = enc_منزل_utf16(s, عدم);
    إذا (wp == عدم)
	أرجع FALSE;

    ret = سېټ_Dlg_توکی_متن_W(hDlg, n_IDDlg_توکی, wp);
    vim_free(wp);
    أرجع ret;
}

/*
 * Convert BGR to RGB for Windows GDI calls
 */
    ساكن COLORREF
بدلون_me(COLORREF colorref)
{
    صحيح  لنډمهاله;
    حرف *ptr = (حرف *)&colorref;

    لنډمهاله = *(ptr);
    *(ptr ) = *(ptr + 2);
    *(ptr + 2) = لنډمهاله;
    أرجع colorref;
}

    ساكن INT_PTR CALLBACK
PrintDlgProc(
	HWND hDlg,
	UINT پیغام_2,
	WPARAM wParam ناکارول_شوی,
	LPARAM lParam ناکارول_شوی)
{
# ifdef FEAT_GETTEXT
    NONCLIENTMETRICS nm;
    ساكن HFONT hfont;
# endif

    بدل (پیغام_2)
    {
	حالة WM_INITDIALOG:
# ifdef FEAT_GETTEXT
	    nm.cb_اندازه = حجم(NONCLIENTMETRICS);
	    إذا (SystemParametersInfo(
			SPI_GETNONCLIENTMETRICS,
			حجم(NONCLIENTMETRICS),
			&nm,
			0))
	    {
		حرف buff[MAX_لار];
		صحيح i;

		// Translate the dialog texts
		hfont = Create_Font_غیرمستقیم(&nm.lf_پیغام_Font);
		لكل (i = IDC_PRINTTEXT1; i <= IDC_پرمختګ; i++)
		{
		    Send_Dlg_توکی_پیغام(hDlg, i, WM_SETFONT, (WPARAM)hfont, 1);
		    إذا (Get_Dlg_توکی_متن(hDlg,i, buff, حجم(buff)))
			vim_سېټ_Dlg_توکی_متن(hDlg,i, (char_u *)_(buff));
		}
		Send_Dlg_توکی_پیغام(hDlg, IDCANCEL,
						WM_SETFONT, (WPARAM)hfont, 1);
		إذا (Get_Dlg_توکی_متن(hDlg,IDCANCEL, buff, حجم(buff)))
		    vim_سېټ_Dlg_توکی_متن(hDlg,IDCANCEL, (char_u *)_(buff));
	    }
# endif
	    سېټ_کړکۍ_متن(hDlg, (LPCSTR)szAppName);
	    إذا (prt_name != عدم)
	    {
		vim_سېټ_Dlg_توکی_متن(hDlg, IDC_PRINTTEXT2, (char_u *)prt_name);
		VIM_CLEAR(prt_name);
	    }
	    Enable_Menu_توکی(GetSystemMenu(hDlg, FALSE), SC_CLOSE, MF_GRAYED);
# إذا !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	    إذا (!gui.دننه_use)
#  endif
		Bring_کړکۍ_منزل_سر(s_hwnd);
# endif
	    أرجع TRUE;

	حالة WM_COMMAND:
	    *bUserAbort = TRUE;
	    Enable_کړکۍ(Get_پلار(hDlg), TRUE);
	    Destroy_کړکۍ(hDlg);
	    hDlgPrint = عدم;
# ifdef FEAT_GETTEXT
	    DeleteObject(hfont);
# endif
	    أرجع TRUE;
    }
    أرجع FALSE;
}

    ساكن BOOL CALLBACK
AbortProc(HDC hdcPrn ناکارول_شوی, صحيح i_کوډ ناکارول_شوی)
{
    پیغام_3 پیغام;

    طالما (!*bUserAbort && Peek_پیغام_W(&پیغام, عدم, 0, 0, PM_REMOVE))
    {
	إذا (!hDlgPrint || !Is_Dialog_پیغام_W(hDlgPrint, &پیغام))
	{
	    Translate_پیغام(&پیغام);
	    Dispatch_پیغام_W(&پیغام);
	}
    }
    أرجع !*bUserAbort;
}

    ساكن UINT_PTR CALLBACK
PrintHookProc(
	HWND hDlg,	// handle to dialog box
	UINT ui_پیغام,	// message identifier
	WPARAM wParam ناکارول_شوی,	// message parameter
	LPARAM lParam	// message parameter
	)
{
    HWND	hwndOwner;
    RECT	rc, rcDlg, rcOwner;
    PRINTDLGW	*pPD;

    إذا (ui_پیغام != WM_INITDIALOG)
	أرجع FALSE;

    // Get the owner window and dialog box rectangles.
    إذا ((hwndOwner = Get_پلار(hDlg)) == عدم)
	hwndOwner = Get_Desktop_کړکۍ();

    Get_کړکۍ_Rect(hwndOwner, &rcOwner);
    Get_کړکۍ_Rect(hDlg, &rcDlg);
    CopyRect(&rc, &rcOwner);

    // Offset the owner and dialog box rectangles so that
    // right and bottom values represent the width and
    // height, and then offset the owner again to discard
    // space taken up by the dialog box.

    دننه_واټن_Rect(&rcDlg, -rcDlg.کیڼ_2, -rcDlg.سر_2);
    دننه_واټن_Rect(&rc, -rc.کیڼ_2, -rc.سر_2);
    دننه_واټن_Rect(&rc, -rcDlg.ښی_2, -rcDlg.bottom);

    // The new position is the sum of half the remaining
    // space and the owner's original position.

    سېټ_کړکۍ_Pos(hDlg,
	    HWND_سر,
	    rcOwner.کیڼ_2 + (rc.ښی_2 / 2),
	    rcOwner.سر_2 + (rc.bottom / 2),
	    0, 0,		// ignores size arguments
	    SWP_NOSIZE);

    //  tackle the printdlg copiesctrl problem
    pPD = (PRINTDLGW *)lParam;
    pPD->nCopies = (کلمه_2)pPD->l_Cust_معلومات;
    سېټ_Dlg_توکی_Int( hDlg, edt3, pPD->nCopies, FALSE );
    //  Bring the window to top
    Bring_کړکۍ_منزل_سر(Get_پلار(hDlg));
    سېټ_Foreground_کړکۍ(hDlg);

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

    إذا (prt_dlg.hDC != عدم)
	DeleteDC(prt_dlg.hDC);
    إذا (!*bUserAbort && hDlgPrint != عدم)
	Send_پیغام(hDlgPrint, WM_COMMAND, 0, 0);
}

    ساكن صحيح
منزل_device_units(صحيح idx, صحيح dpi, صحيح physsize, صحيح دننه_واټن, صحيح def_شمېره)
{
    صحيح		ret = 0;
    صحيح		u;
    صحيح		nr;

    u = prt_get_unit(idx);
    إذا (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_شمېره;
    }
    وإلا
	nr = printer_opts[idx].شمېره;

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
	حالة PRT_UNIT_ټکی:
	    ret = (nr * 10 * dpi) / 720;
	    اكسر;
    }

    إذا (ret < دننه_واټن)
	أرجع 0;
    وإلا
	أرجع ret - دننه_واټن;
}

    ساكن صحيح
prt_get_cpl(فراغ)
{
    صحيح		hr;
    صحيح		phyw;
    صحيح		dvoff;
    صحيح		rev_دننه_واټن;
    صحيح		dpi;

    Get_متن_Metrics(prt_dlg.hDC, &prt_tm);
    prt_کرښه_لوړوالی = prt_tm.tm_لوړوالی + prt_tm.tmExternalLeading;

    hr	    = GetDeviceCaps(prt_dlg.hDC, HORZRES);
    phyw    = GetDeviceCaps(prt_dlg.hDC, PHYSICALWIDTH);
    dvoff   = GetDeviceCaps(prt_dlg.hDC, PHYSICALOFFSETX);
    dpi	    = GetDeviceCaps(prt_dlg.hDC, LOGPIXELSX);

    rev_دننه_واټن = phyw - (dvoff + hr);

    prt_کیڼ_margin = منزل_device_units(OPT_PRINT_کیڼ, dpi, phyw, dvoff, 10);
    إذا (prt_use_شمېره())
    {
	prt_شمېره_پلنوالی = PRINT_شمېره_پلنوالی * prt_tm.tm_Ave_Char_پلنوالی;
	prt_کیڼ_margin += prt_شمېره_پلنوالی;
    }
    وإلا
	prt_شمېره_پلنوالی = 0;

    prt_ښی_margin = hr - منزل_device_units(OPT_PRINT_ښی, dpi, phyw,
							       rev_دننه_واټن, 5);

    أرجع (prt_ښی_margin - prt_کیڼ_margin) / prt_tm.tm_Ave_Char_پلنوالی;
}

    ساكن صحيح
prt_get_lpp(فراغ)
{
    صحيح vr;
    صحيح phyw;
    صحيح dvoff;
    صحيح rev_دننه_واټن;
    صحيح	bottom_margin;
    صحيح	dpi;

    vr	    = GetDeviceCaps(prt_dlg.hDC, VERTRES);
    phyw    = GetDeviceCaps(prt_dlg.hDC, PHYSICALHEIGHT);
    dvoff   = GetDeviceCaps(prt_dlg.hDC, PHYSICALOFFSETY);
    dpi	    = GetDeviceCaps(prt_dlg.hDC, LOGPIXELSY);

    rev_دننه_واټن = phyw - (dvoff + vr);

    prt_سر_margin = منزل_device_units(OPT_PRINT_سر, dpi, phyw, dvoff, 5);

    // adjust top margin if there is a header
    prt_سر_margin += prt_کرښه_لوړوالی * prt_سرلیک_لوړوالی();

    bottom_margin = vr - منزل_device_units(OPT_PRINT_BOT, dpi, phyw,
							       rev_دننه_واټن, 5);

    أرجع (bottom_margin - prt_سر_margin) / prt_کرښه_لوړوالی;
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
    CLEAR_ډګر(prt_dlg);
    prt_dlg.l_Struct_اندازه = حجم(PRINTDLGW);
# إذا !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
    إذا (!gui.دننه_use)
#  endif
	GetConsoleHwnd();	    // get value of s_hwnd
# endif
    prt_dlg.hwndOwner = s_hwnd;
    prt_dlg.Flags = PD_NOPAGENUMS | PD_NOSELECTION | PD_RETURNDC;
    إذا (!forceit)
    {
	prt_dlg.h_Dev_ډېرپېښ = stored_dm;
	prt_dlg.h_Dev_نومونه = stored_devn;
	prt_dlg.l_Cust_معلومات = stored_nCopies; // work around bug in print dialog
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
    إذا (forceit
# إذا !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	    || (!gui.دننه_use && !غړی_console)
#  وإلا
	    || !غړی_console
#  endif
# endif
	    )
    {
	prt_dlg.Flags |= PD_RETURNDEFAULT;
	/*
	 * MSDN suggests setting the first parameter to WINSPOOL for
	 * NT, but NULL appears to work just as well.
	 */
	إذا (*p_pdev != NUL)
	    prt_dlg.hDC = CreateDC(عدم, (LPCSTR)p_pdev, عدم, عدم);
	وإلا
	{
	    prt_dlg.Flags |= PD_RETURNDEFAULT;
	    إذا (PrintDlgW(&prt_dlg) == 0)
		اذهب init_fail_dlg;
	}
    }
    وإلا إذا (PrintDlgW(&prt_dlg) == 0)
	اذهب init_fail_dlg;
    وإلا
    {
	/*
	 * keep the previous driver context
	 */
	stored_dm = prt_dlg.h_Dev_ډېرپېښ;
	stored_devn = prt_dlg.h_Dev_نومونه;
	stored_nFlags = prt_dlg.Flags;
	stored_nCopies = prt_dlg.nCopies;
    }

    إذا (prt_dlg.hDC == عدم)
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
    سېټ_متن_Align(prt_dlg.hDC, TA_BASELINE|TA_کیڼ);

    /*
     * On some windows systems the nCopies parameter is not
     * passed back correctly. It must be retrieved from the
     * hDevMode struct.
     */
    mem = (DEVMODEW *)Global_قلف(prt_dlg.h_Dev_ډېرپېښ);
    إذا (mem != عدم)
    {
	إذا (mem->dmCopies != 1)
	    stored_nCopies = mem->dmCopies;
	إذا ((mem->dmFields & DM_DUPLEX) && (mem->dmDuplex & ~DMDUP_SIMPLEX))
	    psettings->duplex = TRUE;
	إذا ((mem->dmFields & DM_COLOR) && (mem->dmColor & DMCOLOR_COLOR))
	    psettings->has_color = TRUE;
    }
    GlobalUnlock(prt_dlg.h_Dev_ډېرپېښ);

    devname = (DEVNAMES *)Global_قلف(prt_dlg.h_Dev_نومونه);
    إذا (devname != 0)
    {
	WCHAR	*wprinter_name = (WCHAR *)devname + devname->w_Device_دننه_واټن;
	WCHAR	*wport_name = (WCHAR *)devname + devname->w_Output_دننه_واټن;
	char_u	*متن = (char_u *)_("to %s on %s");
	char_u  *printer_name = utf16_منزل_enc(wprinter_name, عدم);
	char_u	*پورټ_name = utf16_منزل_enc(wport_name, عدم);

	إذا (printer_name != عدم && پورټ_name != عدم)
	    prt_name = alloc(STRLEN(printer_name)
					   + STRLEN(پورټ_name) + STRLEN(متن));
	إذا (prt_name != عدم)
	    wsprintf((حرف *)prt_name, (ثابت حرف *)متن,
		    printer_name, پورټ_name);
	vim_free(printer_name);
	vim_free(پورټ_name);
    }
    GlobalUnlock(prt_dlg.h_Dev_نومونه);

    /*
     * Initialise the font according to 'printfont'
     */
    CLEAR_ډګر(fLogFont);
    إذا (get_logfont(&fLogFont, p_pfn, prt_dlg.hDC, TRUE) == FAIL)
    {
	semsg(_(e_unknown_printer_font_str), p_pfn);
	mch_print_cleanup();
	أرجع FALSE;
    }

    لكل (pifBold = 0; pifBold <= 1; pifBold++)
	لكل (pifItalic = 0; pifItalic <= 1; pifItalic++)
	    لكل (pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
	    {
		fLogFont.lf_وزن =  boldface[pifBold];
		fLogFont.lfItalic = pifItalic;
		fLogFont.lfUnderline = pifUnderline;
		prt_font_handles[pifBold][pifItalic][pifUnderline]
					      = Create_Font_غیرمستقیم_W(&fLogFont);
	    }

    سېټ_Bk_ډېرپېښ(prt_dlg.hDC, OPAQUE);
    SelectObject(prt_dlg.hDC, prt_font_handles[0][0][0]);

    /*
     * Fill in the settings struct
     */
    psettings->chars_فی_کرښه = prt_get_cpl();
    psettings->کرښې_فی_پاڼه = prt_get_lpp();
    إذا (prt_dlg.Flags & PD_USEDEVMODECOPIESANDCOLLATE)
    {
	psettings->n_collated_copies = (prt_dlg.Flags & PD_COLLATE)
						    ? prt_dlg.nCopies : 1;
	psettings->n_uncollated_copies = (prt_dlg.Flags & PD_COLLATE)
						    ? 1 : prt_dlg.nCopies;

	إذا (psettings->n_collated_copies == 0)
	    psettings->n_collated_copies = 1;

	إذا (psettings->n_uncollated_copies == 0)
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
    إذا (err)
    {
	char_u *buf;

	// I suspect FormatMessage() doesn't work for values returned by
	// CommDlgExtendedError().  What does?
	Format_پیغام(FORMAT_پیغام_ALLOCATE_بفر |
		FORMAT_پیغام_FROM_SYSTEM |
		FORMAT_پیغام_IGNORE_INSERTS,
		عدم, err, 0, (LPTSTR)(&buf), 0, عدم);
	semsg(_(e_print_error_str),
		buf == عدم ? (char_u *)_("Unknown") : buf);
	LocalFree((LPVOID)(buf));
    }
    وإلا
	پیغام_clr_eos(); // Maybe canceled

    mch_print_cleanup();
    أرجع FALSE;
}


    صحيح
mch_print_begin(prt_settings_T *psettings)
{
    صحيح			ret = 0;
    حرف		sz_بفر[300];
    WCHAR		*wp;

    hDlgPrint = CreateDialog(g_hinst, متن_3("PrintDlgBox"),
					     prt_dlg.hwndOwner, PrintDlgProc);
    سېټ_Abort_Proc(prt_dlg.hDC, AbortProc);
    wsprintf(sz_بفر, _("Printing '%s'"), gettail(psettings->jobname));
    vim_سېټ_Dlg_توکی_متن(hDlgPrint, IDC_PRINTTEXT1, (char_u *)sz_بفر);

    wp = enc_منزل_utf16(psettings->jobname, عدم);
    إذا (wp != عدم)
    {
	DOCINFOW	di;

	CLEAR_ډګر(di);
	di.cb_اندازه = حجم(di);
	di.lpszDocName = wp;
	ret = پیل_Doc_W(prt_dlg.hDC, &di);
	vim_free(wp);
    }

# ifdef FEAT_GUI
    // Give focus back to main window (when using MDI).
#  ifdef VIMDLL
    إذا (gui.دننه_use)
#  endif
	سېټ_Focus(s_hwnd);
# endif

    أرجع (ret > 0);
}

    فراغ
mch_print_پای_ټکی(prt_settings_T *psettings ناکارول_شوی)
{
    پای_ټکی_Doc(prt_dlg.hDC);
    إذا (!*bUserAbort && hDlgPrint != عدم)
	Send_پیغام(hDlgPrint, WM_COMMAND, 0, 0);
}

    صحيح
mch_print_پای_ټکی_پاڼه(فراغ)
{
    أرجع (پای_ټکی_پاڼه(prt_dlg.hDC) > 0);
}

    صحيح
mch_print_begin_پاڼه(char_u *پیغام)
{
    إذا (پیغام != عدم)
	vim_سېټ_Dlg_توکی_متن(hDlgPrint, IDC_پرمختګ, پیغام);
    أرجع (پیل_پاڼه(prt_dlg.hDC) > 0);
}

    صحيح
mch_print_blank_پاڼه(فراغ)
{
    أرجع (mch_print_begin_پاڼه(عدم) ? (mch_print_پای_ټکی_پاڼه()) : FALSE);
}

ساكن صحيح prt_pos_x = 0;
ساكن صحيح prt_pos_y = 0;

    فراغ
mch_print_پیل_کرښه(صحيح margin, صحيح پاڼه_کرښه)
{
    إذا (margin)
	prt_pos_x = -prt_شمېره_پلنوالی;
    وإلا
	prt_pos_x = 0;
    prt_pos_y = پاڼه_کرښه * prt_کرښه_لوړوالی
				 + prt_tm.tmAscent + prt_tm.tmExternalLeading;
}

    صحيح
mch_print_متن_بهر(char_u *p, صحيح len)
{
    اندازه_3	sz;
    WCHAR	*wp;
    صحيح		wlen = len;
    صحيح		ret = FALSE;

    wp = enc_منزل_utf16(p, &wlen);
    إذا (wp == عدم)
	أرجع FALSE;

    متن_بهر_W(prt_dlg.hDC, prt_pos_x + prt_کیڼ_margin,
	    prt_pos_y + prt_سر_margin, wp, wlen);
    Get_متن_Extent_Point32_W(prt_dlg.hDC, wp, wlen, &sz);
    vim_free(wp);
    prt_pos_x += (sz.cx - prt_tm.tmOverhang);
    // This is wrong when printing spaces for a TAB.
    إذا (p[len] != NUL)
    {
	wlen = mb_ptr2len(p + len);
	wp = enc_منزل_utf16(p + len, &wlen);
	إذا (wp != عدم)
	{
	    Get_متن_Extent_Point32_W(prt_dlg.hDC, wp, 1, &sz);
	    ret = (prt_pos_x + prt_کیڼ_margin + sz.cx > prt_ښی_margin);
	    vim_free(wp);
	}
    }
    أرجع ret;
}

    فراغ
mch_print_سېټ_font(صحيح iBold, صحيح iItalic, صحيح iUnderline)
{
    SelectObject(prt_dlg.hDC, prt_font_handles[iBold][iItalic][iUnderline]);
}

    فراغ
mch_print_سېټ_bg(long_u bgcol)
{
    سېټ_Bk_Color(prt_dlg.hDC, Get_تر_ټولو_نږدې_Color(prt_dlg.hDC,
						   بدلون_me((COLORREF)bgcol)));
    /*
     * With a white background we can draw characters transparent, which is
     * good for italic characters that overlap to the next char cell.
     */
    إذا (bgcol == 0xffffffUL)
	سېټ_Bk_ډېرپېښ(prt_dlg.hDC, TRANSPARENT);
    وإلا
	سېټ_Bk_ډېرپېښ(prt_dlg.hDC, OPAQUE);
}

    فراغ
mch_print_سېټ_fg(long_u fgcol)
{
    سېټ_متن_Color(prt_dlg.hDC, Get_تر_ټولو_نږدې_Color(prt_dlg.hDC,
						   بدلون_me((COLORREF)fgcol)));
}

#د_که_پای // FEAT_PRINTER && !FEAT_POSTSCRIPT



#إذا defined(FEAT_SHORTCUT)
# include <shlobj.h>

# define is_لار_sep(c)	    ((c) == L'\\' || (c) == L'/')

    ساكن صحيح
is_reparse_ټکی_included(LPCWSTR fname)
{
    LPCWSTR	p = fname, q;
    WCHAR	buf[MAX_لار];
    DWORD	attr;

    إذا (خوندي_isalpha(p[0]) && p[1] == L':' && is_لار_sep(p[2]))
	p += 3;
    وإلا إذا (is_لار_sep(p[0]) && is_لار_sep(p[1]))
	p += 2;

    طالما (*p != L'\0')
    {
	q = wcspbrk(p, L"\\/");
	إذا (q == عدم)
	    p = q = fname + wcslen(fname);
	وإلا
	    p = q + 1;
	إذا (q - fname >= MAX_لار)
	    أرجع FALSE;
	wcsncpy(buf, fname, q - fname);
	buf[q - fname] = L'\0';
	attr = Get_File_ځانګړتیاوې_W(buf);
	إذا (attr != INVALID_FILE_ځانګړتیاوې
		&& (attr & FILE_ATTRIBUTE_REPARSE_ټکی) != 0)
	    أرجع TRUE;
    }
    أرجع FALSE;
}

/*
 * Return the resolved file path, NULL if "fname" is an AppExecLink reparse
 * point, already fully resolved, or it doesn't exists.
 */
    char_u *
resolve_reparse_ټکی(char_u *fname)
{
    HANDLE	    h = INVALID_HANDLE_ارزښت;
    DWORD	    اندازه;
    WCHAR	    *p, *wp;
    char_u	    *rfname = عدم;
    WCHAR	    *buff = عدم;

    p = enc_منزل_utf16(fname, عدم);
    إذا (p == عدم)
	اذهب fail;

    إذا (!is_reparse_ټکی_included(p))
    {
	vim_free(p);
	اذهب fail;
    }

    h = CreateFileW(p, 0, 0, عدم, OPEN_EXISTING,
	    FILE_FLAG_BACKUP_SEMANTICS, عدم);
    vim_free(p);

    إذا (h == INVALID_HANDLE_ارزښت)
	اذهب fail;

    اندازه = Get_Final_لار_Name_By_Handle_W(h, عدم, 0, 0);
    إذا (اندازه == 0)
	اذهب fail;
    buff = ALLOC_MULT(WCHAR, اندازه);
    إذا (buff == عدم)
	اذهب fail;
    إذا (Get_Final_لار_Name_By_Handle_W(h, buff, اندازه, 0) == 0)
	اذهب fail;

    إذا (wcsncmp(buff, L"\\\\?\\UNC\\", 8) == 0)
    {
	buff[6] = L'\\';
	wp = buff + 6;
    }
    وإلا إذا (wcsncmp(buff, L"\\\\?\\", 4) == 0)
	wp = buff + 4;
    وإلا
	wp = buff;

    rfname = utf16_منزل_enc(wp, عدم);

fail:
    إذا (h != INVALID_HANDLE_ارزښت)
	CloseHandle(h);
    إذا (buff != عدم)
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
    OLECHAR		wsz[MAX_لار];
    char_u		*rfname = عدم;
    صحيح			len;
    IShellLinkW		*pslw = عدم;
    WIN32_پیدا_کړه_DATAW	ffdw; // we get those free of charge

    // Check if the file name ends in ".lnk". Avoid calling
    // CoCreateInstance(), it's quite slow.
    إذا (fname == عدم)
	أرجع rfname;
    len = (صحيح)STRLEN(fname);
    إذا (len <= 4 || STRNICMP(fname + len - 4, ".lnk", 4) != 0)
	أرجع rfname;

    CoInitialize(عدم);

    // create a link manager object and request its interface
    hr = CoCreateInstance(
	    &CLSID_ShellLink, عدم, CLSCTX_INPROC_SERVER,
	    &IID_IShellLinkW, (فراغ**)&pslw);
    إذا (hr == S_OK)
    {
	WCHAR	*p = enc_منزل_utf16(fname, عدم);

	إذا (p != عدم)
	{
	    // Get a pointer to the IPersistFile interface.
	    hr = pslw->lpVtbl->QueryInterface(
		    pslw, &IID_IPersistFile, (فراغ**)&ppf);
	    إذا (hr != S_OK)
		اذهب shortcut_errorw;

	    // "load" the name and resolve the link
	    hr = ppf->lpVtbl->بارول_2(ppf, p, STGM_READ);
	    إذا (hr != S_OK)
		اذهب shortcut_errorw;
# إذا 0  // This makes Vim wait a long time if the target does not exist.
	    hr = pslw->lpVtbl->Resolve(pslw, عدم, SLR_NO_UI);
	    إذا (hr != S_OK)
		اذهب shortcut_errorw;
# endif

	    // Get the path to the link target.
	    Zero_حافظه(wsz, MAX_لار * حجم(WCHAR));
	    hr = pslw->lpVtbl->Get_لار(pslw, wsz, MAX_لار, &ffdw, 0);
	    إذا (hr == S_OK && wsz[0] != NUL)
		rfname = utf16_منزل_enc(wsz, عدم);

shortcut_errorw:
	    vim_free(p);
	}
    }

    // Release all interface pointers (both belong to the same object)
    إذا (ppf != عدم)
	ppf->lpVtbl->Release(ppf);
    إذا (psl != عدم)
	psl->lpVtbl->Release(psl);
    إذا (pslw != عدم)
	pslw->lpVtbl->Release(pslw);

    CoUninitialize();
    أرجع rfname;
}

    char_u *
mch_resolve_لار(char_u *fname, صحيح reparse_ټکی)
{
    char_u  *لار = resolve_shortcut(fname);

    إذا (لار == عدم && reparse_ټکی)
	لار = resolve_reparse_ټکی(fname);
    أرجع لار;
}
#د_که_پای

#إذا defined(FEAT_EVAL) && (!defined(FEAT_GUI) || defined(VIMDLL))
/*
 * Bring ourselves to the foreground.  Does work if the OS doesn't allow it.
 */
    فراغ
win32_سېټ_foreground(فراغ)
{
    GetConsoleHwnd();	    // get value of s_hwnd
    إذا (s_hwnd != 0)
	سېټ_Foreground_کړکۍ(s_hwnd);
}
#د_که_پای

#إذا defined(FEAT_CLIENTSERVER)
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
HWND پیغام_کړکۍ = 0;	    // window that's handling messages

# define VIM_CLASSNAME      "VIM_MESSAGES"
# define VIM_CLASSNAME_LEN  (حجم(VIM_CLASSNAME) - 1)

// Timeout for sending a message to another Vim instance.  Normally this works
// instantly, but it may hang when the other Vim instance is halted.
# define SENDMESSAGE_TIMEOUT	(5 * 1000)

// Communication is via WM_COPYDATA messages. The message type is sent in
// the dwData parameter. Types are defined here.
# define COPYDATA_کلیګانې		0
# define COPYDATA_ځواب		1
# define COPYDATA_بیان		10
# define COPYDATA_پایله	11
# define COPYDATA_ERROR_پایله	12
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
serverSendEnc(HWND موخه)
{
    COPYDATASTRUCT معلومات;

    معلومات.dw_معلومات = COPYDATA_ENCODING;
    معلومات.cb_معلومات = (DWORD)STRLEN(p_enc) + 1;
    معلومات.lp_معلومات = p_enc;
    إذا (Send_پیغام_Timeout(موخه, WM_COPYDATA,
	    (WPARAM)پیغام_کړکۍ, (LPARAM)&معلومات,
	    SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, عدم) == 0)
	أرجع -1;
    أرجع 0;
}

/*
 * Clean up on exit. This destroys the hidden message window.
 */
    ساكن فراغ
Clean_پورته_Messaging(فراغ)
{
    إذا (پیغام_کړکۍ == 0)
	أرجع;

    Destroy_کړکۍ(پیغام_کړکۍ);
    پیغام_کړکۍ = 0;
}

ساكن صحيح save_ځواب(HWND server, char_u *ځواب, صحيح بیان);

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
Messaging_WndProc(HWND hwnd, UINT پیغام, WPARAM wParam, LPARAM lParam)
{
    إذا (پیغام == WM_COPYDATA)
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
	COPYDATASTRUCT	*معلومات = (COPYDATASTRUCT*)lParam;
	HWND		sender = (HWND)wParam;
	COPYDATASTRUCT	ځواب;
	char_u		*res;
	صحيح		retval;
	DWORD_PTR	dwret = 0;
	char_u		*str;
	char_u		*tofree;

	بدل (معلومات->dw_معلومات)
	{
	حالة COPYDATA_ENCODING:
	    // Remember the encoding that the client uses.
	    vim_free(client_enc);
	    client_enc = enc_canonize((char_u *)معلومات->lp_معلومات);
	    أرجع 1;

	حالة COPYDATA_کلیګانې:
	    // Remember who sent this, for <client>
	    client_کړکۍ = sender;

	    // Add the received keys to the input buffer.
	    str = serverConvert(client_enc, (char_u *)معلومات->lp_معلومات, &tofree);
# ifdef FEAT_GUI
	    إذا (gui.دننه_use)
		// The GUI may get here re-entrantly, so insert the keys later.
		server_جمع_کړه_ننوت(str);
	    وإلا
# endif
		server_منزل_ننوت_buf(str);
	    vim_free(tofree);

# ifdef FEAT_GUI
	    // Wake up the main GUI loop.
#  ifdef VIMDLL
	    إذا (gui.دننه_use)
#  endif
		إذا (s_hwnd != 0)
		    Post_پیغام(s_hwnd, WM_NULL, 0, 0);
# endif
	    أرجع 1;

	حالة COPYDATA_بیان:
	    // Remember who sent this, for <client>
	    client_کړکۍ = sender;

	    str = serverConvert(client_enc, (char_u *)معلومات->lp_معلومات, &tofree);
	    res = eval_client_بیان_منزل_string(str);

	    إذا (res == عدم)
	    {
		حرف	*err = _(e_invalid_افاده_received);
		size_t	len = STRLEN(str) + STRLEN(err) + 5;

		res = alloc(len);
		إذا (res != عدم)
		    ځواب.cb_معلومات = (DWORD)vim_snprintf_safelen(
			(حرف *)res, len, "%s: \"%s\"", err, str);
		وإلا
		    ځواب.cb_معلومات = 0;
		ځواب.dw_معلومات = COPYDATA_ERROR_پایله;
	    }
	    وإلا
	    {
		ځواب.cb_معلومات = (DWORD)STRLEN(res) + 1;
		ځواب.dw_معلومات = COPYDATA_پایله;
	    }
	    ځواب.lp_معلومات = res;

	    إذا (serverSendEnc(sender) < 0)
		retval = -1;
	    وإلا
	    {
		إذا (Send_پیغام_Timeout(sender, WM_COPYDATA,
			(WPARAM)پیغام_کړکۍ, (LPARAM)&ځواب,
			SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
		    retval = -1;
		وإلا
		    retval = (صحيح)dwret;
	    }
	    vim_free(tofree);
	    vim_free(res);
	    أرجع retval;

	حالة COPYDATA_ځواب:
	حالة COPYDATA_پایله:
	حالة COPYDATA_ERROR_پایله:
	    إذا (معلومات->lp_معلومات != عدم)
	    {
		str = serverConvert(client_enc, (char_u *)معلومات->lp_معلومات,
								     &tofree);
		إذا (tofree == عدم)
		    str = vim_strsave(str);
		إذا (save_ځواب(sender, str,
			   (معلومات->dw_معلومات == COPYDATA_ځواب ?  0 :
			   (معلومات->dw_معلومات == COPYDATA_پایله ? 1 :
							      2))) == FAIL)
		    vim_free(str);
		وإلا إذا (معلومات->dw_معلومات == COPYDATA_ځواب)
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

    وإلا إذا (پیغام == WM_ACTIVATE && wParam == WA_فعال)
    {
	// When the message window is activated (brought to the foreground),
	// this actually applies to the text window.
# إذا !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	إذا (!gui.دننه_use)
#  endif
	    GetConsoleHwnd();	    // get value of s_hwnd
# endif
	إذا (s_hwnd != 0)
	{
	    سېټ_Foreground_کړکۍ(s_hwnd);
	    أرجع 0;
	}
    }

    أرجع Def_کړکۍ_Proc(hwnd, پیغام, wParam, lParam);
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
    atexit(Clean_پورته_Messaging);

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
    ثبتوونکی_Class(&wndclass);

    // Create the message window. It will be hidden, so the details don't
    // matter.  Don't use WS_OVERLAPPEDWINDOW, it will make a shortcut remove
    // focus from gvim.
    پیغام_کړکۍ = Create_کړکۍ(VIM_CLASSNAME, "",
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
    حرف	بفر[VIM_CLASSNAME_LEN + 1];

    // Ignore windows which aren't Vim message windows
    len = GetClassName(hwnd, بفر, حجم(بفر));
    إذا (len != VIM_CLASSNAME_LEN || STRCMP(بفر, VIM_CLASSNAME) != 0)
	أرجع 0;

    // Get the title of the window
    أرجع Get_کړکۍ_متن(hwnd, name, namelen);
}

    ساكن BOOL CALLBACK
enumWindowsGetServer(HWND hwnd, LPARAM lparam)
{
    بنية	server_id *id = (بنية server_id *)lparam;
    حرف	server[MAX_لار];

    // Get the title of the window
    إذا (getVimServerName(hwnd, server, حجم(server)) == 0)
	أرجع TRUE;

    // If this is the server we're looking for, return its HWND
    إذا (STRICMP(server, id->name) == 0)
    {
	id->hwnd = hwnd;
	أرجع FALSE;
    }

    // If we are looking for an alternate server, remember this name.
    إذا (altname_buf_ptr != عدم)
    {
	size_t	namelen = STRLEN(id->name);

	إذا (STRNICMP(server, id->name, namelen) == 0
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
enum_Windows_Get_نومونه(HWND hwnd, LPARAM lparam)
{
    list_T	*list = (list_T *)lparam;
    حرف	server[MAX_لار];

    // Get the title of the window
    إذا (getVimServerName(hwnd, server, حجم(server)) == 0)
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
enum_windows_ماشوم(HWND hwnd, LPARAM lParam)
{
    بنية enum_windows_s *ew = (بنية enum_windows_s *)lParam;

    أرجع (ew->lpEnumFunc)(hwnd, ew->lParam);
}

    ساكن BOOL CALLBACK
enum_windows_toplevel(HWND hwnd, LPARAM lParam)
{
    بنية enum_windows_s *ew = (بنية enum_windows_s *)lParam;

    إذا ((ew->lpEnumFunc)(hwnd, ew->lParam))
	أرجع TRUE;
    أرجع Enum_ماشوم_Windows(hwnd, enum_windows_ماشوم, lParam);
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
پیدا_کړه_Server(char_u *name)
{
    بنية server_id id;

    id.name = name;
    id.hwnd = 0;

    enum_windows(enumWindowsGetServer, (LPARAM)(&id));

    أرجع id.hwnd;
}

    فراغ
server_سېټ_Name(char_u *name)
{
    size_t	namelen;
    char_u	*ok_name;
    HWND	hwnd = 0;
    صحيح		i = 0;
    char_u	*p;

    // Leave enough space for a 9-digit suffix to ensure uniqueness!
    namelen = STRLEN(name);
    ok_name = alloc(namelen + 10);
    إذا (ok_name == عدم)
	أرجع;

    STRCPY(ok_name, name);
    p = ok_name + namelen;

    لكل (;;)
    {
	// This is inefficient - we're doing an EnumWindows loop for each
	// possible name. It would be better to grab all names in one go,
	// and scan the list each time...
	hwnd = پیدا_کړه_Server(ok_name);
	إذا (hwnd == 0)
	    اكسر;

	++i;
	إذا (i >= 1000)
	    اكسر;

	sprintf((حرف *)p, "%d", i);
    }

    إذا (hwnd != 0)
	vim_free(ok_name);
    وإلا
    {
	// Remember the name
	serverName = ok_name;
	need_maketitle = TRUE;	// update Vim window title later

	// Update the message window title
	سېټ_کړکۍ_متن(پیغام_کړکۍ, (LPCSTR)ok_name);

# ifdef FEAT_EVAL
	// Set the servername variable
	سېټ_vim_var_string(VV_SEND_SERVER, serverName, -1);
# endif
    }
}

    list_T *
server_Get_Vim_نومونه(فراغ)
{
    list_T *list = list_alloc();

    إذا (list == عدم)
	أرجع عدم;

    enum_windows(enum_Windows_Get_نومونه, (LPARAM)list);

    أرجع list;
}

    صحيح
server_Send_ځواب(
    char_u	*name,		// Where to send.
    char_u	*ځواب)		// What to send.
{
    HWND	موخه;
    COPYDATASTRUCT معلومات;
    long_u	n = 0;
    DWORD_PTR	dwret = 0;

    // The "name" argument is a magic cookie obtained from expand("<client>").
    // It should be of the form 0xXXXXX - i.e. a C hex literal, which is the
    // value of the client's message window HWND.
    له_مزي_ولوله((حرف *)name, SCANF_HEX_LONG_U, &n);
    إذا (n == 0)
	أرجع -1;

    موخه = (HWND)n;
    إذا (!Is_کړکۍ(موخه))
	أرجع -1;

    معلومات.dw_معلومات = COPYDATA_ځواب;
    معلومات.cb_معلومات = (DWORD)STRLEN(ځواب) + 1;
    معلومات.lp_معلومات = ځواب;

    إذا (serverSendEnc(موخه) < 0)
	أرجع -1;
    إذا (Send_پیغام_Timeout(موخه, WM_COPYDATA,
		(WPARAM)پیغام_کړکۍ, (LPARAM)&معلومات,
		SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
	أرجع -1;
    أرجع dwret ? 0 : -1;
}

    صحيح
server_Send_منزل_Vim(
    char_u	 *name,			// Where to send.
    char_u	 *cmd,			// What to send.
    char_u	 **پایله,		// Result of eval'ed expression
    فراغ	 *ptarget,		// HWND of server
    صحيح		 as_بیان,		// Expression or keys?
    صحيح		 timeout,		// timeout in seconds or zero
    صحيح		 silent)		// don't complain about no server
{
    size_t	namelen;
    HWND	موخه;
    COPYDATASTRUCT معلومات;
    char_u	*retval = عدم;
    صحيح		retcode = 0;
    DWORD_PTR	dwret = 0;
    char_u	altname_buf[MAX_لار];

    // Execute locally if no display or target is ourselves
    إذا (serverName != عدم && STRICMP(name, serverName) == 0)
	أرجع send_منزل_Local_Vim(cmd, as_بیان, پایله);

    // If the server name does not end in a digit then we look for an
    // alternate name.  e.g. when "name" is GVIM then we may find GVIM2.
    namelen = STRLEN(name);
    إذا (namelen > 1 && !vim_isdigit(name[namelen - 1]))
	altname_buf_ptr = altname_buf;
    altname_buf[0] = NUL;
    موخه = پیدا_کړه_Server(name);
    altname_buf_ptr = عدم;
    إذا (موخه == 0 && altname_buf[0] != NUL)
	// Use another server name we found.
	موخه = پیدا_کړه_Server(altname_buf);

    إذا (موخه == 0)
    {
	إذا (!silent)
	    semsg(_(e_no_registered_server_named_str), name);
	أرجع -1;
    }

    إذا (ptarget)
	*(HWND *)ptarget = موخه;

    معلومات.dw_معلومات = as_بیان ? COPYDATA_بیان : COPYDATA_کلیګانې;
    معلومات.cb_معلومات = (DWORD)STRLEN(cmd) + 1;
    معلومات.lp_معلومات = cmd;

    إذا (serverSendEnc(موخه) < 0)
	أرجع -1;
    إذا (Send_پیغام_Timeout(موخه, WM_COPYDATA,
		(WPARAM)پیغام_کړکۍ, (LPARAM)&معلومات,
		SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
	أرجع -1;
    إذا (dwret == 0)
	أرجع -1;

    إذا (as_بیان)
	retval = server_Get_ځواب(موخه, &retcode, TRUE, TRUE, timeout);

    إذا (پایله == عدم)
	vim_free(retval);
    وإلا
	*پایله = retval; // Caller assumes responsibility for freeing

    أرجع retcode;
}

/*
 * Bring the server to the foreground.
 */
    فراغ
serverForeground(char_u *name)
{
    HWND	موخه = پیدا_کړه_Server(name);

    إذا (موخه != 0)
	سېټ_Foreground_کړکۍ(موخه);
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
    char_u	*ځواب;		// reply string
    صحيح		بیان_پایله;	// 0 for REPLY, 1 for RESULT 2 for error
} ځواب_T;

ساكن garray_T ځواب_list = {0, 0, حجم(ځواب_T), 5, 0};

# define ځواب_توکی(i) ((ځواب_T *)(ځواب_list.ga_معلومات) + (i))
# define ځواب_شمېر (ځواب_list.ga_len)

// Flag which is used to wait for a reply
ساكن صحيح ځواب_received = 0;

/*
 * Store a reply.  "reply" must be allocated memory (or NULL).
 */
    ساكن صحيح
save_ځواب(HWND server, char_u *ځواب, صحيح بیان)
{
    ځواب_T *rep;

    إذا (ga_grow(&ځواب_list, 1) == FAIL)
	أرجع FAIL;

    rep = ځواب_توکی(ځواب_شمېر);
    rep->server = server;
    rep->ځواب = ځواب;
    rep->بیان_پایله = بیان;
    إذا (rep->ځواب == عدم)
	أرجع FAIL;

    ++ځواب_شمېر;
    ځواب_received = 1;
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
server_Get_ځواب(HWND server, صحيح *بیان_res, صحيح remove, صحيح wait, صحيح timeout)
{
    صحيح		i;
    char_u	*ځواب;
    ځواب_T	*rep;
    صحيح		did_process = FALSE;
    time_t	پیل;
    time_t	now;

    // When waiting, loop until the message waiting for is received.
    وخت(&پیل);
    لكل (;;)
    {
	// Reset this here, in case a message arrives while we are going
	// through the already received messages.
	ځواب_received = 0;

	لكل (i = 0; i < ځواب_شمېر; ++i)
	{
	    rep = ځواب_توکی(i);
	    إذا (rep->server == server
		    && ((rep->بیان_پایله != 0) == (بیان_res != عدم)))
	    {
		// Save the values we've found for later
		ځواب = rep->ځواب;
		إذا (بیان_res != عدم)
		    *بیان_res = rep->بیان_پایله == 1 ? 0 : -1;

		إذا (remove)
		{
		    // Move the rest of the list down to fill the gap
		    mch_memmove(rep, rep + 1,
				     (ځواب_شمېر - i - 1) * حجم(ځواب_T));
		    --ځواب_شمېر;
		}

		// Return the reply to the caller, who takes on responsibility
		// for freeing it if "remove" is TRUE.
		أرجع ځواب;
	    }
	}

	// If we got here, we didn't find a reply. Return immediately if the
	// "wait" parameter isn't set.
	إذا (!wait)
	{
	    // Process pending messages once. Without this, looping on
	    // remote_peek() would never get the reply.
	    إذا (!did_process)
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
	طالما (ځواب_received == 0)
	{
# ifdef FEAT_TIMERS
	    // TODO: use the return value to decide how long to wait.
	    check_due_timer();
# endif
	    وخت(&now);
	    إذا (timeout > 0 && (now - پیل) >= timeout)
		اكسر;

	    // Wait for a SendMessage() call to us.  This could be the reply
	    // we are waiting for.  Use a timeout of a second, to catch the
	    // situation that the server died unexpectedly.
	    پیغام_Wait_For_مضرب_Objects(0, عدم, TRUE, 1000, QS_ALLINPUT);

	    // If the server has died, give up
	    إذا (!Is_کړکۍ(server))
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
    پیغام_3 پیغام;

    طالما (Peek_پیغام_W(&پیغام, عدم, 0, 0, PM_REMOVE))
    {
	Translate_پیغام(&پیغام);
	Dispatch_پیغام_W(&پیغام);
    }
}

#د_که_پای // FEAT_CLIENTSERVER

#إذا defined(FEAT_GUI) || (defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT))

بنية charset_pair
{
    string_T	name;
    BYTE	charset;
};

ساكن بنية charset_pair
charset_جوړې[] =
{
    {STR_LITERAL_INIT("ANSI"),		ANSI_CHARSET},
    {STR_LITERAL_INIT("CHINESEBIG5"),	CHINESEBIG5_CHARSET},
    {STR_LITERAL_INIT("DEFAULT"),	DEFAULT_CHARSET},
    {STR_LITERAL_INIT("HANGEUL"),	HANGEUL_CHARSET},
    {STR_LITERAL_INIT("OEM"),		OEM_CHARSET},
    {STR_LITERAL_INIT("SHIFTJIS"),	SHIFTJIS_CHARSET},
    {STR_LITERAL_INIT("SYMBOL"),	نښه_CHARSET},
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
quality_جوړې[] = {
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

    لكل (i = 0; i < (صحيح)ARRAY_اوږدتیا(charset_جوړې); ++i)
    {
	إذا ((BYTE)id == charset_جوړې[i].charset)
	    أرجع (حرف *)charset_جوړې[i].name.string;
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

    لكل (i = 0; i < (صحيح)ARRAY_اوږدتیا(quality_جوړې); ++i)
    {
	إذا (id == quality_جوړې[i].quality)
	    أرجع (حرف *)quality_جوړې[i].name.string;
    }

    أرجع عدم;
}

// The default font height in 100% scaling (96dpi).
// (-16 in 96dpi equates to roughly 12pt)
# define DEFAULT_FONT_لوړوالی	(-16)

ساكن ثابت LOGFONTW s_lfDefault =
{
    DEFAULT_FONT_لوړوالی,
    0, 0, 0, FW_نورمال, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
    بهر_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
    PROOF_QUALITY, FIXED_PITCH | FF_DONTCARE,
    L""	// Default font name will be set later based on current language.
};

// This will be initialized when set_default_logfont() is called first time.
// The value will be based on the system DPI.
صحيح current_font_لوړوالی = 0;		// also used in gui_w32.c

/*
 * Convert a string representing a point size into pixels. The string should
 * be a positive decimal number, with an optional decimal point (eg, "12", or
 * "10.5"). The pixel value is returned, and a pointer to the next unconverted
 * character is stored in *end. The flag "vertical" says whether this
 * calculation is for a vertical (height) size or a horizontal (width) one.
 */
    ساكن صحيح
points_منزل_pixels(WCHAR *str, WCHAR **پای_ټکی, صحيح vertical, long_i pprinter_dc)
{
    صحيح		pixels;
    صحيح		points = 0;
    صحيح		وېشونکی = 0;
    HWND	hwnd = (HWND)0;
    HDC		hdc;
    HDC		printer_dc = (HDC)pprinter_dc;

    طالما (*str != NUL)
    {
	إذا (*str == L'.' && وېشونکی == 0)
	{
	    // Start keeping a divisor, for later
	    وېشونکی = 1;
	}
	وإلا
	{
	    إذا (!VIM_ISDIGIT(*str))
		اكسر;

	    points *= 10;
	    points += *str - L'0';
	    وېشونکی *= 10;
	}
	++str;
    }

    إذا (وېشونکی == 0)
	وېشونکی = 1;

    إذا (printer_dc == عدم)
    {
	hwnd = Get_Desktop_کړکۍ();
	hdc = Get_کړکۍ_DC(hwnd);
    }
    وإلا
	hdc = printer_dc;

    pixels = MulDiv(points,
		    GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX),
		    72 * وېشونکی);

    إذا (printer_dc == عدم)
	ReleaseDC(hwnd, hdc);

    *پای_ټکی = str;
    أرجع pixels;
}

/*
 * Convert pixel into point size. This is a reverse of points_to_pixels.
 */
    ساكن مزدوج
pixels_منزل_points(صحيح pixels, صحيح vertical, long_i pprinter_dc)
{
    مزدوج	points = 0;
    HWND	hwnd = (HWND)0;
    HDC		hdc;
    HDC		printer_dc = (HDC)pprinter_dc;

    إذا (printer_dc == عدم)
    {
	hwnd = Get_Desktop_کړکۍ();
	hdc = Get_کړکۍ_DC(hwnd);
    }
    وإلا
	hdc = printer_dc;

    points = pixels * 72.0 / GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX);
    إذا (printer_dc == عدم)
	ReleaseDC(hwnd, hdc);

    أرجع points;
}

    ساكن صحيح CALLBACK
font_enumproc(
    ENUMLOGFONTW    *elf,
    NEWTEXTMETRICW  *ntm ناکارول_شوی,
    DWORD	    type ناکارول_شوی,
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
    إذا ((ntm->tmPitchAndFamily & 1) != 0)
	أرجع 1;
# endif

    // Remember this LOGFONTW as a "possible"
    *lf = elf->elfLogFont;

    // Terminate the scan as soon as we find an ANSI font
    إذا (lf->lf_Char_سېټ == ANSI_CHARSET
	    || lf->lf_Char_سېټ == OEM_CHARSET
	    || lf->lf_Char_سېټ == DEFAULT_CHARSET)
	أرجع 0;

    // Continue the scan - we have a non-ANSI font
    أرجع 2;
}

    ساكن صحيح
init_logfont(LOGFONTW *lf)
{
    صحيح		n;
    HWND	hwnd = Get_Desktop_کړکۍ();
    HDC		hdc = Get_کړکۍ_DC(hwnd);

    n = EnumFontFamiliesW(hdc,
			 lf->lfFaceName,
			 (FONTENUMPROCW)font_enumproc,
			 (LPARAM)lf);

    ReleaseDC(hwnd, hdc);

    // If we couldn't find a usable font, return failure
    إذا (n == 1)
	أرجع FAIL;

    // Tidy up the rest of the LOGFONTW structure. We set to a basic
    // font - get_logfont() sets bold, italic, etc based on the user's
    // input.
    lf->lf_لوړوالی = current_font_لوړوالی;
    lf->lf_پلنوالی = 0;
    lf->lfItalic = FALSE;
    lf->lfUnderline = FALSE;
    lf->lf_Strike_بهر = FALSE;
    lf->lf_وزن = FW_نورمال;

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
    NEWTEXTMETRICW  *ntm ناکارول_شوی,
    DWORD	    type ناکارول_شوی,
    LPARAM	    lparam)
{
    LOGFONTW *lf = (LOGFONTW*)elf;

# ifndef FEAT_PROPORTIONAL_FONTS
    // Ignore non-monospace fonts without further ado
    إذا ((ntm->tmPitchAndFamily & 1) != 0)
	أرجع 1;
# endif

    // Filter only on ANSI. Otherwise will see a lot of random fonts that we
    // usually don't want.
    إذا (lf->lf_Char_سېټ != ANSI_CHARSET)
	أرجع 1;

    صحيح (*جمع_کړه_سمون)(char_u *) = (صحيح (*)(char_u *))lparam;

    WCHAR *faceNameW = lf->lfFaceName;
    char_u *faceName = utf16_منزل_enc(faceNameW, عدم);
    إذا (!faceName)
	أرجع 0;

    جمع_کړه_سمون(faceName);
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
    optexpand_T	*دلیلونه,
    فراغ	*param ناکارول_شوی,
    صحيح		(*جمع_کړه_سمون)(char_u *val))
{
    expand_T	    *xp = دلیلونه->oe_xp;
    إذا (xp->xp_نمونه > دلیلونه->oe_سېټ_arg && *(xp->xp_نمونه-1) == ':')
    {
	حرف buf[30];
	صحيح	i;

	// Always fill in with the current font size as first option for
	// convenience. We simply round to the closest integer for simplicity.
	صحيح font_لوړوالی = (صحيح)پړاو(
		pixels_منزل_points(-current_font_لوړوالی, TRUE, (long_i)عدم));
	vim_snprintf(buf, ARRAY_اوږدتیا(buf), "h%d", font_لوړوالی);
	جمع_کړه_سمون((char_u *)buf);

	// Note: Keep this in sync with get_logfont(). Don't include 'c' and
	// 'q' as we fill in all the values below.
	ساكن حرف *(p_gfn_win_opt_ارزښتونه[]) = {
	    "h" , "w" , "W" , "b" , "i" , "u" , "s"};
	لكل (i = 0; i < (صحيح)ARRAY_اوږدتیا(p_gfn_win_opt_ارزښتونه); i++)
	    جمع_کړه_سمون((char_u *)p_gfn_win_opt_ارزښتونه[i]);

	لكل (i = 0; i < (صحيح)ARRAY_اوږدتیا(charset_جوړې); ++i)
	{
	    vim_snprintf(buf, حجم(buf), "c%s", charset_جوړې[i].name.string);
	    جمع_کړه_سمون((char_u *)buf);
	}

	لكل (i = 0; i < (صحيح)ARRAY_اوږدتیا(quality_جوړې); ++i)
	{
	    vim_snprintf(buf, حجم(buf), "q%s", quality_جوړې[i].name.string);
	    جمع_کړه_سمون((char_u *)buf);
	}
	أرجع;
    }

    HWND	hwnd = Get_Desktop_کړکۍ();
    HDC		hdc = Get_کړکۍ_DC(hwnd);

    EnumFontFamiliesW(hdc,
	    عدم,
	    (FONTENUMPROCW)expand_font_enumproc,
	    (LPARAM)جمع_کړه_سمون);

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
	إذا (w[i] == 0 || w[i] != p[i])
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
    HWND hwnd = Get_Desktop_کړکۍ();
    HDC hdc = Get_کړکۍ_DC(hwnd);
    UINT dpi = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(hwnd, hdc);
    أرجع dpi;
}

/*
 * Set default logfont based on current language.
 */
    ساكن فراغ
سېټ_default_logfont(LOGFONTW *lf)
{
    // Default font name for current language on MS-Windows.
    // If not translated, falls back to "Consolas".
    // This must be a fixed-pitch font.
    ثابت حرف *defaultfontname = N_("DefaultFontNameForWindows");
    حرف *fontname = _(defaultfontname);

    إذا (STRCMP(fontname, defaultfontname) == 0)
	fontname = "Consolas";

    *lf = s_lfDefault;
    lf->lf_لوړوالی = DEFAULT_FONT_لوړوالی * (صحيح)vimGetDpiForSystem() / 96;
    إذا (current_font_لوړوالی == 0)
	current_font_لوړوالی = lf->lf_لوړوالی;

    WCHAR *wfontname = enc_منزل_utf16((char_u*)fontname, عدم);
    إذا (wfontname != عدم)
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

    سېټ_default_logfont(lf);
    إذا (name == عدم)
	أرجع OK;

    wname = enc_منزل_utf16(name, عدم);
    إذا (wname == عدم)
	أرجع FAIL;

    إذا (wcscmp(wname, L"*") == 0)
    {
# إذا defined(FEAT_GUI_MSWIN)
	CHOOSEFONTW	cf;
	// if name is "*", bring up std font dialog:
	CLEAR_ډګر(cf);
	cf.l_Struct_اندازه = حجم(cf);
	cf.hwndOwner = s_hwnd;
	cf.Flags = CF_SCREENFONTS | CF_FIXEDPITCHONLY | CF_INITTOLOGFONTSTRUCT;
	إذا (lastlf != عدم)
	    *lf = *lastlf;
	cf.lpLogFont = lf;
	cf.nFontType = 0 ; //REGULAR_FONTTYPE;
	إذا (ټاکنه_Font_W(&cf))
	    ret = OK;
# endif
	اذهب theend;
    }

    /*
     * Split name up, it could be <name>:h<height>:w<width> etc.
     */
    لكل (p = wname; *p && *p != L':'; p++)
    {
	إذا (p - wname + 1 >= LF_FACESIZE)
	    اذهب theend;			// Name too long
	lf->lfFaceName[p - wname] = *p;
    }
    إذا (p != wname)
	lf->lfFaceName[p - wname] = NUL;

    // First set defaults
    lf->lf_لوړوالی = DEFAULT_FONT_لوړوالی * (صحيح)vimGetDpiForSystem() / 96;
    lf->lf_پلنوالی = 0;
    lf->lf_وزن = FW_نورمال;
    lf->lfItalic = FALSE;
    lf->lfUnderline = FALSE;
    lf->lf_Strike_بهر = FALSE;

    /*
     * If the font can't be found, try replacing '_' by ' '.
     */
    إذا (init_logfont(lf) == FAIL)
    {
	صحيح	did_replace = FALSE;

	لكل (i = 0; lf->lfFaceName[i]; ++i)
	    إذا (lf->lfFaceName[i] == L'_')
	    {
		lf->lfFaceName[i] = L' ';
		did_replace = TRUE;
	    }
	إذا (!did_replace || init_logfont(lf) == FAIL)
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
		lf->lf_لوړوالی = - points_منزل_pixels(p, &p, TRUE, (long_i)printer_dc);
		اكسر;
	    حالة L'w':
		lf->lf_پلنوالی = points_منزل_pixels(p, &p, FALSE, (long_i)printer_dc);
		اكسر;
	    حالة L'W':
		lf->lf_وزن = wcstol(p, &p, 10);
		اكسر;
	    حالة L'b':
		lf->lf_وزن = FW_BOLD;
		اكسر;
	    حالة L'i':
		lf->lfItalic = TRUE;
		اكسر;
	    حالة L'u':
		lf->lfUnderline = TRUE;
		اكسر;
	    حالة L's':
		lf->lf_Strike_بهر = TRUE;
		اكسر;
	    حالة L'c':
		لكل (i = 0; i < (صحيح)ARRAY_اوږدتیا(charset_جوړې); ++i)
		{
		    إذا (utf16ascncmp(p, (حرف *)charset_جوړې[i].name.string,
			    charset_جوړې[i].name.اوږدتیا) == 0)
		    {
			lf->lf_Char_سېټ = charset_جوړې[i].charset;
			p += charset_جوړې[i].name.اوږدتیا;
			اكسر;
		    }
		}

		إذا (i == (صحيح)ARRAY_اوږدتیا(charset_جوړې) && verbose)
		{
		    char_u *s = utf16_منزل_enc(p, عدم);
		    إذا (s != عدم)
		    {
			semsg(_(e_illegal_str_name_str_دننه_font_name_str),
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
		لكل (i = 0; i < (صحيح)ARRAY_اوږدتیا(quality_جوړې); ++i)
		{
		    إذا (utf16ascncmp(p, (حرف *)quality_جوړې[i].name.string,
			    quality_جوړې[i].name.اوږدتیا) == 0)
		    {
			lf->lfQuality = quality_جوړې[i].quality;
			p += quality_جوړې[i].name.اوږدتیا;
			اكسر;
		    }
		}

		إذا (i == (صحيح)ARRAY_اوږدتیا(quality_جوړې) && verbose)
		{
		    char_u *s = utf16_منزل_enc(p, عدم);
		    إذا (s != عدم)
		    {
			semsg(_(e_illegal_str_name_str_دننه_font_name_str),
							   "quality", s, name);
			vim_free(s);
		    }
		}
		اكسر;
	    افتراضي:
		إذا (verbose)
		    semsg(_(e_illegal_char_nr_دننه_font_name_str), p[-1], name);
		اذهب theend;
	}
	طالما (*p == L':')
	    p++;
    }
    ret = OK;

theend:
    // ron: init lastlf
    إذا (ret == OK && printer_dc == عدم)
    {
	vim_free(lastlf);
	lastlf = ALLOC_ONE(LOGFONTW);
	إذا (lastlf != عدم)
	    mch_memmove(lastlf, lf, حجم(LOGFONTW));
    }
    vim_free(wname);

    أرجع ret;
}

#د_که_پای // defined(FEAT_GUI) || defined(FEAT_PRINTER)

#إذا defined(FEAT_دنده_CHANNEL)
/*
 * Initialize the Winsock dll.
 */
    فراغ
channel_init_winsock(فراغ)
{
    WSADATA wsa_معلومات;
    صحيح wsaerr;

    إذا (WSInitialized)
	أرجع;

    wsaerr = WSAStartup(MAKEWORD(2, 2), &wsa_معلومات);
    إذا (wsaerr == 0)
	WSInitialized = TRUE;
}
#د_که_پای
