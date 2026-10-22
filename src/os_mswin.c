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
#اگر_ختم

#اگر defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT)
# include <dlgs.h>
# include <winspool.h>
# include <commdlg.h>
#اگر_ختم

// Record all output and all keyboard & mouse input
// #define MCH_WRITE_DUMP

#اگر_تعریف_شدہ MCH_WRITE_DUMP
FILE* fdDump = تہی;
#اگر_ختم

#اگر !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
خارجی حرف g_szOrigTitle[];
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI
خارجی HWND s_hwnd;
#ورنہ
ساکن HWND s_hwnd = 0;	    // console window handle, set by GetConsoleHwnd()
#اگر_ختم

#اگر_تعریف_شدہ FEAT_کام_CHANNEL
صحیح WSInitialized = FALSE; // WinSock is initialized
#اگر_ختم


/*
 * Save the instance handle of the exe/dll.
 */
    خالی
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
    خالی
mch_exit_g(صحیح r)
{
    exiting = TRUE;

    display_errors();

    ml_close_all(TRUE);		// remove all memfiles

# ifdef FEAT_OLE
    UninitOLE();
# endif
# ifdef FEAT_کام_CHANNEL
    اگر (WSInitialized)
    {
	WSInitialized = FALSE;
	WSACleanup();
    }
# endif
# ifdef DYNAMIC_GETTEXT
    dyn_libintl_انتہا();
# endif

    اگر (gui.اندر_use)
	gui_exit(r);

# ifdef EXITFREE
    free_all_mem();
# endif

    باہر_نکلو(r);
}

#اگر_ختم // FEAT_GUI_MSWIN


/*
 * Get version number including build number
 */
قسمتعریف BOOL (WINAPI *Pfn_Rtl_Get_نسخہ)(LPOSVERSIONINFOW);
DWORD win_نسخہ;
    ساکن خالی
win_نسخہ_init(خالی)
{
    OSVERSIONINFOW	osver;
    HMODULE		hNtdll;
    Pfn_Rtl_Get_نسخہ	p_Rtl_Get_نسخہ;

    hNtdll = GetModuleHandle("ntdll.dll");
    اگر (hNtdll == تہی)
	واپس;

    p_Rtl_Get_نسخہ =
	(Pfn_Rtl_Get_نسخہ) Get_Proc_پتہ(hNtdll, "RtlGetVersion");
    اگر (p_Rtl_Get_نسخہ == تہی)
	واپس;

    osver.dw_OSVersion_Info_سائز = حجم(OSVERSIONINFOW);
    p_Rtl_Get_نسخہ(&osver);
    win_نسخہ = MAKE_VER(min(osver.dw_Major_نسخہ, 0xFF),
	    min(osver.dw_Minor_نسخہ, 0xFF),
	    min(osver.dw_Build_نمبر, 0xFFFF));
}

/*
 * Init the tables for toupper() and tolower().
 */
    خالی
mch_early_init(خالی)
{
    صحیح		i;

    win_نسخہ_init();

    PlatformId();

    // Init the tables for toupper() and tolower()
    برائے (i = 0; i < 256; ++i)
	toupper_tab[i] = tolower_tab[i] = i;
    Char_بالائی_Buff((LPSTR)toupper_tab, 256);
    Char_زیریں_Buff((LPSTR)tolower_tab, 256);
}


/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    صحیح
mch_داخلہ_isatty(خالی)
{
#اگر_تعریف_شدہ FEAT_GUI_MSWIN
# ifdef VIMDLL
    اگر (gui.اندر_use)
# endif
	واپس TRUE;	    // GUI always has a tty
#اگر_ختم
#اگر !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    اگر (isatty(read_cmd_fd))
	واپس TRUE;
    واپس FALSE;
#اگر_ختم
}

/*
 * mch_settitle(): set titlebar of our window
 */
    خالی
mch_settitle(
    char_u *title,
    char_u *icon غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_GUI_MSWIN
# ifdef VIMDLL
    اگر (gui.اندر_use)
# endif
    {
	gui_mch_settitle(title, icon);
	واپس;
    }
#اگر_ختم
#اگر !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    اگر (title != تہی)
    {
	WCHAR	*wp = enc_مقصد_utf16(title, تہی);

	اگر (wp == تہی)
	    واپس;

	سیٹ_Console_Title_W(wp);
	vim_free(wp);
	واپس;
    }
#اگر_ختم
}


/*
 * Restore the window/icon title.
 * which is one of:
 *  SAVE_RESTORE_TITLE: Just restore title
 *  SAVE_RESTORE_ICON:  Just restore icon (which we don't have)
 *  SAVE_RESTORE_BOTH:  Restore title and icon (which we don't have)
 */
    خالی
mch_restore_title(صحیح which غیر_مستعمل)
{
#اگر !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
    اگر (!gui.اندر_use)
# endif
	سیٹ_Console_Title(g_szOrigTitle);
#اگر_ختم
}


/*
 * Return TRUE if we can restore the title (we can)
 */
    صحیح
mch_can_restore_title(خالی)
{
    واپس TRUE;
}


/*
 * Return TRUE if we can restore the icon title (we can't)
 */
    صحیح
mch_can_restore_icon(خالی)
{
    واپس FALSE;
}


/*
 * Get absolute file name into buffer "buf" of length "len" bytes,
 * turning all '/'s into '\\'s and getting the correct case of each component
 * of the file name.  Append a (back)slash to a directory name.
 * When 'shellslash' set do it the other way around.
 * Return OK or FAIL.
 */
    صحیح
mch_بھرا_Name(
    char_u	*fname,
    char_u	*buf,
    صحیح		len,
    صحیح		force غیر_مستعمل)
{
    صحیح		n_نتیجہ = FAIL;
    WCHAR	*wname;
    WCHAR	wbuf[MAX_راستہ];
    char_u	*cname = تہی;

    wname = enc_مقصد_utf16(fname, تہی);
    اگر (wname != تہی && _wfullpath(wbuf, wname, MAX_راستہ) != تہی)
    {
	cname = utf16_مقصد_enc((short_u *)wbuf, تہی);
	اگر (cname != تہی)
	{
	    vim_strncpy(buf, cname, len - 1);
	    n_نتیجہ = OK;
	}
    }
    vim_free(wname);
    vim_free(cname);

#اگر_تعریف_شدہ USE_FNAME_CASE
    fname_case(buf, len);
#ورنہ
    slash_adjust(buf);
#اگر_ختم

    واپس n_نتیجہ;
}


/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    صحیح
mch_is_بھرا_Name(char_u *fname)
{
    // A name like "d:/foo" and "//server/share" is absolute.  "d:foo" is not.
    // /foo and \foo are absolute too because Windows keeps a current drive.
    // Another way to check is to use mch_FullName() and see if the result is
    // the same as the name or mch_FullName() fails.  However, this has quite a
    // bit of overhead, so let's not do that.
    اگر (*fname == NUL)
	واپس FALSE;
    واپس ((ASCII_ISALPHA(fname[0]) && fname[1] == ':'
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
    خالی
slash_adjust(char_u *p)
{
    اگر (راستہ_with_url(p))
	واپس;

    اگر (*p == '`')
    {
	size_t len = STRLEN(p);

	// don't replace backslash in backtick quoted strings
	اگر (len > 2 && *(p + len - 1) == '`')
	    واپس;
    }

    جب (*p)
    {
	اگر (*p == psepcN)
	    *p = psepc;
	MB_PTR_ADV(p);
    }
}

    ساکن صحیح
read_reparse_نقطہ(ثابت WCHAR *name, char_u *buf, DWORD *buf_len)
{
    HANDLE h;
    BOOL ok;

    h = CreateFileW(name, FILE_READ_اوصاف,
	    FILE_SHARE_READ | FILE_SHARE_WRITE, تہی,
	    OPEN_EXISTING,
	    FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_نقطہ,
	    تہی);
    اگر (h == INVALID_HANDLE_قدر)
	واپس FAIL;

    ok = DeviceIoControl(h, FSCTL_GET_REPARSE_نقطہ, تہی, 0, buf, *buf_len,
	    buf_len, تہی);
    CloseHandle(h);

    واپس ok ? OK : FAIL;
}

    char_u *
resolve_appexeclink(char_u *fname)
{
    DWORD		attr = 0;
    صحیح			idx;
    WCHAR		*p, *انتہا, *wname;
    // The buffer size is arbitrarily chosen to be "big enough" (TM), the
    // ceiling should be around 16k.
    char_u		buf[4096];
    DWORD		buf_len = حجم(buf);
    REPARSE_ڈیٹا_بفر *rb = (REPARSE_ڈیٹا_بفر *)buf;

    wname = enc_مقصد_utf16(fname, تہی);
    اگر (wname == تہی)
	واپس تہی;

    attr = Get_File_اوصاف_W(wname);
    اگر (attr == INVALID_FILE_اوصاف ||
	    (attr & FILE_ATTRIBUTE_REPARSE_نقطہ) == 0)
    {
	vim_free(wname);
	واپس تہی;
    }

    // The applinks are similar to symlinks but with a huge difference: they can
    // only be executed, any other I/O operation on them is bound to fail with
    // ERROR_FILE_NOT_FOUND even though the file exists.
    اگر (read_reparse_نقطہ(wname, buf, &buf_len) == FAIL)
    {
	vim_free(wname);
	واپس تہی;
    }
    vim_free(wname);

    اگر (rb->Reparse_لیبل != IO_REPARSE_لیبل_APPEXECLINK)
	واپس تہی;

    // The (undocumented) reparse buffer contains a set of N null-terminated
    // Unicode strings, the application path is stored in the third one.
    اگر (rb->App_Exec_Link_Reparse_بفر.String_گنتی < 3)
	واپس تہی;

    p = rb->App_Exec_Link_Reparse_بفر.StringList;
    انتہا = p + rb->Reparse_ڈیٹا_لمبائی / حجم(WCHAR);
    برائے (idx = 0; p < انتہا
	    && idx < (صحیح)rb->App_Exec_Link_Reparse_بفر.String_گنتی
	    && idx != 2; )
    {
	اگر (*p++ == L'\0')
	    ++idx;
    }

    واپس utf16_مقصد_enc(p, تہی);
}

// Use 64-bit stat functions.
#تعریف_منسوخ stat
#تعریف_منسوخ _stat
#تعریف_منسوخ _wstat
#تعریف_منسوخ _fstat
#تعریف_کرو stat _stat64
#تعریف_کرو _stat _stat64
#تعریف_کرو _wstat _wstat64
#تعریف_کرو _fstat _fstat64

/*
 * Implements lstat() and stat() that can handle symlinks properly.
 */
    ساکن صحیح
mswin_stat_impl(ثابت WCHAR *name, stat_T *stp, ثابت صحیح resolve)
{
    صحیح			n;
    صحیح			fd;
    BOOL		is_symlink = FALSE;
    HANDLE		h_ڈھونڈو, h;
    DWORD		attr = 0;
    DWORD		flag = 0;
    WIN32_ڈھونڈو_DATAW    ڈھونڈو_ڈیٹا_W;

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
	واپس _wstat(name, stp);
#اگر_ختم

    // Only reparse points need a directory query to identify symbolic links.
    // In particular, avoid enumerating the directory for each missing file
    // while searching 'runtimepath'.
    attr = Get_File_اوصاف_W(name);
    اگر (attr == INVALID_FILE_اوصاف)
    {
	DWORD error = Get_آخری_Error();

	// A missing file or path cannot provide stat information.  Other
	// errors and UNC paths still need the CRT fallback.
	اگر ((error == ERROR_FILE_NOT_ملا || error == ERROR_راستہ_NOT_ملا)
		&& !((name[0] == L'\\' || name[0] == L'/')
		    && (name[1] == L'\\' || name[1] == L'/')))
	{
	    errno = ENOENT;
	    واپس -1;
	}
	واپس _wstat(name, stp);
    }
    اگر (!(attr & FILE_ATTRIBUTE_REPARSE_نقطہ))
	واپس _wstat(name, stp);

    h_ڈھونڈو = ڈھونڈو_پہلا_File_W(name, &ڈھونڈو_ڈیٹا_W);
    اگر (h_ڈھونڈو != INVALID_HANDLE_قدر)
    {
	attr = ڈھونڈو_ڈیٹا_W.dw_File_اوصاف;
	اگر ((attr & FILE_ATTRIBUTE_REPARSE_نقطہ)
		&& (ڈھونڈو_ڈیٹا_W.dwReserved0 == IO_REPARSE_لیبل_SYMLINK))
	    is_symlink = TRUE;
	ڈھونڈو_Close(h_ڈھونڈو);
    }

    // Use the plain old stat() whenever it's possible.
    اگر (!is_symlink)
	واپس _wstat(name, stp);

    اگر (!resolve && is_symlink)
	flag = FILE_FLAG_OPEN_REPARSE_نقطہ;
    اگر (attr & FILE_ATTRIBUTE_DIRECTORY)
	flag |= FILE_FLAG_BACKUP_SEMANTICS;

    h = CreateFileW(name, FILE_READ_اوصاف,
	    FILE_SHARE_READ | FILE_SHARE_WRITE, تہی, OPEN_EXISTING, flag,
	    تہی);
    اگر (h == INVALID_HANDLE_قدر)
	واپس -1;

    fd = _open_osfhandle((intptr_t)h, _O_RDONLY);
    n = _fstat(fd, (ساخت _stat *)stp);
    اگر ((n == 0) && (attr & FILE_ATTRIBUTE_DIRECTORY))
	stp->st_منوال = (stp->st_منوال & ~S_IFMT) | S_IFDIR;
    _close(fd);

    واپس n;
}

/*
 * stat() can't handle a trailing '/' or '\', remove it first.
 * When 'resolve' is true behave as lstat() wrt symlinks.
 */
    ساکن صحیح
stat_impl(ثابت حرف *name, stat_T *stp, ثابت صحیح resolve)
{
    // WinNT and later can use _MAX_PATH wide characters for a pathname, which
    // means that the maximum pathname is _MAX_PATH * 3 bytes when 'enc' is
    // UTF-8.
    char_u	buf[MAX_راستہ_2 * 3 + 1];
    size_t	buflen;
    char_u	*p;
    WCHAR	*wp;
    صحیح		n;

    vim_strncpy((char_u *)buf, (char_u *)name, حجم(buf) - 1);
    buflen = STRLEN(buf);
    p = buf + buflen;
    اگر (p > buf)
	MB_PTR_عقب(buf, p);

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
	اگر (p != تہی)
	{
	    p = vim_strpbrk(p + 1, (char_u *)"\\/");
	    اگر (p == تہی)
		STRCPY(buf + buflen, "\\");
	}
    }

    wp = enc_مقصد_utf16(buf, تہی);
    اگر (wp == تہی)
	واپس -1;

    n = mswin_stat_impl(wp, stp, resolve);
    vim_free(wp);
    واپس n;
}

    صحیح
vim_lstat(ثابت حرف *name, stat_T *stp)
{
    واپس stat_impl(name, stp, FALSE);
}

    صحیح
vim_stat(ثابت حرف *name, stat_T *stp)
{
    واپس stat_impl(name, stp, TRUE);
}

#اگر defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    خالی
mch_settmode(tmode_T tmode غیر_مستعمل)
{
    // nothing to do
}

    صحیح
mch_get_shellsize(خالی)
{
    // never used
    واپس OK;
}

    خالی
mch_سیٹ_shellsize(خالی)
{
    // never used
}

/*
 * Rows and/or Columns has changed.
 */
    خالی
mch_new_shellsize(خالی)
{
    // never used
}

    خالی
mch_calc_cell_سائز(ساخت cellsize *cs_برآمد غیر_مستعمل)
{
    // never used
}

#اگر_ختم

/*
 * We have no job control, so fake it by starting a new shell.
 */
    خالی
mch_suspend(خالی)
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
    خالی
display_errors(خالی)
{
# ifdef FEAT_GUI
    char_u *p;

#  ifdef VIMDLL
    اگر (gui.اندر_use || gui.starting)
#  endif
    {
	اگر (error_ga.ga_ڈیٹا != تہی)
	{
	    // avoid putting up a message box with blanks only
	    برائے (p = (char_u *)error_ga.ga_ڈیٹا; *p; ++p)
		اگر (!محفوظ_isspace(*p))
		{
		    // Only use a dialog when not using --gui-dialog-file:
		    // write text to a file.
		    اگر (!gui_dialog_log((char_u *)"Errors", p))
			(خالی)gui_mch_dialog(
				     gui.starting ? VIM_INFO :
					     VIM_ERROR,
				     gui.starting ? (char_u *)_("Message") :
					     (char_u *)_("Error"),
					     p, (char_u *)_("&Ok"),
					1, تہی, FALSE);
		    توڑو;
		}
	    ga_clear(&error_ga);
	}
	واپس;
    }
# endif
# اگر !defined(FEAT_GUI) || defined(VIMDLL)
    FlushFileBuffers(GetStdHandle(STD_ERROR_HANDLE));
# endif
}
#اگر_ختم


/*
 * Return TRUE if "p" contain a wildcard that can be expanded by
 * dos_expandpath().
 */
    صحیح
mch_has_exp_wildcard(char_u *p)
{
    برائے ( ; *p; MB_PTR_ADV(p))
    {
	اگر (vim_strchr((char_u *)"?*[", *p) != تہی
		|| (*p == '~' && p[1] != NUL))
	    واپس TRUE;
    }
    واپس FALSE;
}

/*
 * Return TRUE if "p" contain a wildcard or a "~1" kind of thing (could be a
 * shortened file name).
 */
    صحیح
mch_has_wildcard(char_u *p)
{
    برائے ( ; *p; MB_PTR_ADV(p))
    {
	اگر (vim_strchr((char_u *)"?*$[`", *p) != تہی
		|| (*p == '~' && p[1] != NUL))
	    واپس TRUE;
    }
    واپس FALSE;
}


/*
 * The normal _chdir() does not change the default drive.  This one does.
 * Returning 0 implies success; -1 implies failure.
 */
    صحیح
mch_chdir(حرف *راستہ)
{
    WCHAR   *p;
    صحیح	    n;

    اگر (راستہ[0] == NUL)		// just checking...
	واپس -1;

    اگر (p_verbose >= 5)
    {
	verbose_enter();
	smsg("chdir(%s)", راستہ);
	verbose_leave();
    }
    اگر (محفوظ_isalpha(راستہ[0]) && راستہ[1] == ':')	// has a drive name
    {
	// If we can change to the drive, skip that part of the path.  If we
	// can't then the current directory may be invalid, try using chdir()
	// with the whole path.
	اگر (_chdrive(TOLOWER_ASC(راستہ[0]) - 'a' + 1) == 0)
	    راستہ += 2;
    }

    اگر (*راستہ == NUL)		// drive name only
	واپس 0;

    p = enc_مقصد_utf16((char_u *)راستہ, تہی);
    اگر (p == تہی)
	واپس -1;

    n = _wchdir(p);
    vim_free(p);
    واپس n;
}


#اگر defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
/*
 * return non-zero if a character is available
 */
    صحیح
mch_char_avail(خالی)
{
    // never used
    واپس TRUE;
}

# اگر defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    صحیح
mch_check_messages(خالی)
{
    // TODO: check for messages
    واپس TRUE;
}
# endif
#اگر_ختم


#اگر defined(FEAT_LIBCALL)
/*
 * Call a DLL routine which takes either a string or int param
 * and returns an allocated string.
 * Return OK if it worked, FAIL if not.
 */
قسمتعریف char_u *(*MYSTRPROCSTR)(char_u *);
قسمتعریف صحیح (*MYSTRPROCINT)(char_u *);
قسمتعریف char_u *(*MYINTPROCSTR)(صحیح);
قسمتعریف صحیح (*MYINTPROCINT)(صحیح);

/*
 * Check if a pointer points to a valid NUL terminated string.
 * Return the length of the string, including terminating NUL.
 * Returns 0 for an invalid pointer, 1 for an empty string.
 */
    ساکن size_t
check_str_len(char_u *str)
{
    SYSTEM_INFO			si;
    حافظہ_BASIC_INFORMATION	mbi;
    size_t			لمبائی = 0;
    size_t			i;
    ثابت char_u		*p;

    // get page size
    GetSystemInfo(&si);

    // get memory information
    اگر (!VirtualQuery(str, &mbi, حجم(mbi)))
	واپس 0;

    // pre cast these (typing savers)
    long_u dwStr = (long_u)str;
    long_u dw_بنیاد_پتہ = (long_u)mbi.بنیاد_پتہ;

    // get start address of page that str is on
    long_u str_صفحہ = dwStr - (dwStr - dw_بنیاد_پتہ) % si.dw_صفحہ_سائز;

    // get length from str to end of page
    long_u صفحہ_لمبائی = si.dw_صفحہ_سائز - (dwStr - str_صفحہ);

    برائے (p = str; !IsBadReadPtr(p, (UINT)صفحہ_لمبائی);
	    p += صفحہ_لمبائی, صفحہ_لمبائی = si.dw_صفحہ_سائز)
	برائے (i = 0; i < صفحہ_لمبائی; ++i, ++لمبائی)
	    اگر (p[i] == NUL)
		واپس لمبائی + 1;

    واپس 0;
}

/*
 * Passed to do_in_runtimepath() to load a vim.ico file.
 */
    ساکن خالی
mch_icon_بھرو_cb(char_u *fname, خالی *cookie)
{
    HANDLE *h = (HANDLE *)cookie;

    *h = بھرو_Image(تہی,
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
mch_icon_بھرو(HANDLE *iconp)
{
    واپس do_اندر_runtimepath((char_u *)"bitmaps/vim.ico",
						  0, mch_icon_بھرو_cb, iconp);
}

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if the OS PRNG is not available or something went wrong.
 */
    صحیح
mch_get_random(char_u *buf, صحیح len)
{
    ساکن صحیح		initialized = NOTDONE;
    ساکن HINSTANCE	hInstLib;
    ساکن BOOL (WINAPI *pProcessPrng)(PUCHAR, ULONG);

    اگر (initialized == NOTDONE)
    {
	hInstLib = vim_بھرو_Lib("bcryptprimitives.dll");
	اگر (hInstLib != تہی)
	    pProcessPrng = (خالی *)Get_Proc_پتہ(hInstLib, "ProcessPrng");
	اگر (hInstLib == تہی || pProcessPrng == تہی)
	{
	    FreeLibrary(hInstLib);
	    initialized = FAIL;
	}
	ورنہ
	    initialized = OK;
    }

    اگر (initialized == FAIL)
	واپس FAIL;

    // According to the documentation this call cannot fail.
    pProcessPrng(buf, len);

    واپس OK;
}

    صحیح
mch_libcall(
    char_u	*libname,
    char_u	*funcname,
    char_u	*argstring,	// NULL when using a argint
    صحیح		argint,
    char_u	**string_نتیجہ,// NULL when using number_result
    صحیح		*نمبر_نتیجہ)
{
    HINSTANCE		hinstLib;

    BOOL f_تسلسل_وقت_Link_Success = FALSE;

    // Get a handle to the DLL module.
    hinstLib = vim_بھرو_Lib((حرف *)libname);

    // If the handle is valid, try to get the function address.
    اگر (hinstLib != تہی)
    {
	char_u	*retval_str = تہی;
	صحیح	retval_int = 0;
	size_t	len;

# ifdef HAVE_TRY_EXCEPT
	__try
	{
# endif
	اگر (argstring != تہی)
	{
	    // Call with string argument
	    اگر (string_نتیجہ != تہی)
	    {
		MYSTRPROCSTR proc;

		proc = (MYSTRPROCSTR)Get_Proc_پتہ(hinstLib, (LPCSTR)funcname);
		اگر (proc != تہی)
		{
		    f_تسلسل_وقت_Link_Success = TRUE;
		    retval_str = proc(argstring);
		}
	    }
	    ورنہ
	    {
		MYSTRPROCINT proc;

		proc = (MYSTRPROCINT)Get_Proc_پتہ(hinstLib, (LPCSTR)funcname);
		اگر (proc != تہی)
		{
		    f_تسلسل_وقت_Link_Success = TRUE;
		    retval_int = proc(argstring);
		}
	    }
	}
	ورنہ
	{
	    // Call with number argument
	    اگر (string_نتیجہ != تہی)
	    {
		MYINTPROCSTR proc;

		proc = (MYINTPROCSTR)Get_Proc_پتہ(hinstLib, (LPCSTR)funcname);
		اگر (proc != تہی)
		{
		    f_تسلسل_وقت_Link_Success = TRUE;
		    retval_str = proc(argint);
		}
	    }
	    ورنہ
	    {
		MYINTPROCINT proc;

		proc = (MYINTPROCINT)Get_Proc_پتہ(hinstLib, (LPCSTR)funcname);
		اگر (proc != تہی)
		{
		    f_تسلسل_وقت_Link_Success = TRUE;
		    retval_int = proc(argint);
		}
	    }
	}

	// Save the string before we free the library.
	// Assume that a "1" result is an illegal pointer.
	اگر (string_نتیجہ == تہی)
	    *نمبر_نتیجہ = retval_int;
	ورنہ اگر (retval_str != تہی
		&& (len = check_str_len(retval_str)) > 0)
	{
	    *string_نتیجہ = alloc(len);
	    اگر (*string_نتیجہ != تہی)
		mch_memmove(*string_نتیجہ, retval_str, len);
	}

# ifdef HAVE_TRY_EXCEPT
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	    اگر (Get_Exception_رمز() == EXCEPTION_ڈھیر_حد_سے_تجاوز)
		_resetstkoflw();
	    f_تسلسل_وقت_Link_Success = FALSE;
	}
# endif

	// Free the DLL module.
	(خالی)FreeLibrary(hinstLib);
    }

    اگر (!f_تسلسل_وقت_Link_Success)
    {
	semsg(_(e_library_call_failed_for_str), funcname);
	واپس FAIL;
    }

    واپس OK;
}
#اگر_ختم

/*
 * Debugging helper: expose the MCH_WRITE_DUMP stuff to other modules
 */
    خالی
DumpPutS(ثابت حرف *psz غیر_مستعمل)
{
#اگر_تعریف_شدہ MCH_WRITE_DUMP
    اگر (fdDump)
    {
	fputs(psz, fdDump);
	اگر (psz[لڑی_لمبائی(psz) - 1] != '\n')
	    fputc('\n', fdDump);
	fflush(fdDump);
    }
#اگر_ختم
}

#اگر_تعریف_شدہ _DEBUG

خالی __cdecl
Trace(
    حرف *pszFormat,
    ...)
{
    CHAR szBuff[2048];
    va_list دلائل;

    va_start(دلائل, pszFormat);
    vsprintf(szBuff, pszFormat, دلائل);
    va_end(دلائل);

    OutputDebugString(szBuff);
}

#اگر_ختم //_DEBUG

#اگر !defined(FEAT_GUI) || defined(VIMDLL)
خارجی HWND g_hWnd;	// This is in os_win32.c.

/*
 * Showing the printer dialog is tricky since we have no GUI
 * window to parent it. The following routines are needed to
 * get the window parenting and Z-order to work properly.
 */
    ساکن خالی
GetConsoleHwnd(خالی)
{
    // Skip if it's already set.
    اگر (s_hwnd != 0)
	واپس;

    // Window handle may have been found by init code (Windows NT only)
    اگر (g_hWnd != 0)
    {
	s_hwnd = g_hWnd;
	واپس;
    }

    s_hwnd = Get_Console_کھڑکی();
}

/*
 * Console implementation of ":winpos".
 */
    صحیح
mch_get_winpos(صحیح *x, صحیح *y)
{
    RECT  rect;

    GetConsoleHwnd();
    Get_کھڑکی_Rect(s_hwnd, &rect);
    *x = rect.بایاں;
    *y = rect.چوٹی;
    واپس OK;
}

/*
 * Console implementation of ":winpos x y".
 */
    خالی
mch_سیٹ_winpos(صحیح x, صحیح y)
{
    GetConsoleHwnd();
    سیٹ_کھڑکی_Pos(s_hwnd, تہی, x, y, 0, 0,
		 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}
#اگر_ختم

#اگر defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT)

//=================================================================
// Win32 printer stuff

ساکن HFONT		prt_font_handles[2][2][2];
ساکن PRINTDLGW	prt_dlg;
ساکن ثابت صحیح	boldface[2] = {FW_REGULAR, FW_BOLD};
ساکن TEXTMETRIC	prt_tm;
ساکن صحیح		prt_سطر_اونچائی;
ساکن صحیح		prt_نمبر_چوڑائی;
ساکن صحیح		prt_بایاں_margin;
ساکن صحیح		prt_دایاں_margin;
ساکن صحیح		prt_چوٹی_margin;
ساکن char_u		szAppName[] = متن_3("VIM");
ساکن HWND		hDlgPrint;
ساکن صحیح		*bUserAbort = تہی;
ساکن char_u		*prt_name = تہی;

// Defines which are also in vim.rc.
# define IDC_BOX1		400
# define IDC_PRINTTEXT1		401
# define IDC_PRINTTEXT2		402
# define IDC_پیش_رفت		403

    ساکن BOOL
vim_سیٹ_Dlg_شے_متن(HWND hDlg, صحیح n_IDDlg_شے, char_u *s)
{
    WCHAR   *wp;
    BOOL    ret;

    wp = enc_مقصد_utf16(s, تہی);
    اگر (wp == تہی)
	واپس FALSE;

    ret = سیٹ_Dlg_شے_متن_W(hDlg, n_IDDlg_شے, wp);
    vim_free(wp);
    واپس ret;
}

/*
 * Convert BGR to RGB for Windows GDI calls
 */
    ساکن COLORREF
تبادلہ_me(COLORREF colorref)
{
    صحیح  عارضی;
    حرف *ptr = (حرف *)&colorref;

    عارضی = *(ptr);
    *(ptr ) = *(ptr + 2);
    *(ptr + 2) = عارضی;
    واپس colorref;
}

    ساکن INT_PTR CALLBACK
PrintDlgProc(
	HWND hDlg,
	UINT پیغام_2,
	WPARAM wParam غیر_مستعمل,
	LPARAM lParam غیر_مستعمل)
{
# ifdef FEAT_GETTEXT
    NONCLIENTMETRICS nm;
    ساکن HFONT hfont;
# endif

    انتخاب (پیغام_2)
    {
	حالت WM_INITDIALOG:
# ifdef FEAT_GETTEXT
	    nm.cb_سائز = حجم(NONCLIENTMETRICS);
	    اگر (SystemParametersInfo(
			SPI_GETNONCLIENTMETRICS,
			حجم(NONCLIENTMETRICS),
			&nm,
			0))
	    {
		حرف buff[MAX_راستہ];
		صحیح i;

		// Translate the dialog texts
		hfont = Create_Font_بالواسطہ(&nm.lf_پیغام_Font);
		برائے (i = IDC_PRINTTEXT1; i <= IDC_پیش_رفت; i++)
		{
		    Send_Dlg_شے_پیغام(hDlg, i, WM_SETFONT, (WPARAM)hfont, 1);
		    اگر (Get_Dlg_شے_متن(hDlg,i, buff, حجم(buff)))
			vim_سیٹ_Dlg_شے_متن(hDlg,i, (char_u *)_(buff));
		}
		Send_Dlg_شے_پیغام(hDlg, IDCANCEL,
						WM_SETFONT, (WPARAM)hfont, 1);
		اگر (Get_Dlg_شے_متن(hDlg,IDCANCEL, buff, حجم(buff)))
		    vim_سیٹ_Dlg_شے_متن(hDlg,IDCANCEL, (char_u *)_(buff));
	    }
# endif
	    سیٹ_کھڑکی_متن(hDlg, (LPCSTR)szAppName);
	    اگر (prt_name != تہی)
	    {
		vim_سیٹ_Dlg_شے_متن(hDlg, IDC_PRINTTEXT2, (char_u *)prt_name);
		VIM_CLEAR(prt_name);
	    }
	    Enable_Menu_شے(GetSystemMenu(hDlg, FALSE), SC_CLOSE, MF_GRAYED);
# اگر !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	    اگر (!gui.اندر_use)
#  endif
		Bring_کھڑکی_مقصد_چوٹی(s_hwnd);
# endif
	    واپس TRUE;

	حالت WM_COMMAND:
	    *bUserAbort = TRUE;
	    Enable_کھڑکی(Get_والد(hDlg), TRUE);
	    Destroy_کھڑکی(hDlg);
	    hDlgPrint = تہی;
# ifdef FEAT_GETTEXT
	    DeleteObject(hfont);
# endif
	    واپس TRUE;
    }
    واپس FALSE;
}

    ساکن BOOL CALLBACK
AbortProc(HDC hdcPrn غیر_مستعمل, صحیح i_رمز غیر_مستعمل)
{
    پیغام_3 پیغام;

    جب (!*bUserAbort && Peek_پیغام_W(&پیغام, تہی, 0, 0, PM_REMOVE))
    {
	اگر (!hDlgPrint || !Is_Dialog_پیغام_W(hDlgPrint, &پیغام))
	{
	    Translate_پیغام(&پیغام);
	    Dispatch_پیغام_W(&پیغام);
	}
    }
    واپس !*bUserAbort;
}

    ساکن UINT_PTR CALLBACK
PrintHookProc(
	HWND hDlg,	// handle to dialog box
	UINT ui_پیغام,	// message identifier
	WPARAM wParam غیر_مستعمل,	// message parameter
	LPARAM lParam	// message parameter
	)
{
    HWND	hwndOwner;
    RECT	rc, rcDlg, rcOwner;
    PRINTDLGW	*pPD;

    اگر (ui_پیغام != WM_INITDIALOG)
	واپس FALSE;

    // Get the owner window and dialog box rectangles.
    اگر ((hwndOwner = Get_والد(hDlg)) == تہی)
	hwndOwner = Get_Desktop_کھڑکی();

    Get_کھڑکی_Rect(hwndOwner, &rcOwner);
    Get_کھڑکی_Rect(hDlg, &rcDlg);
    CopyRect(&rc, &rcOwner);

    // Offset the owner and dialog box rectangles so that
    // right and bottom values represent the width and
    // height, and then offset the owner again to discard
    // space taken up by the dialog box.

    انحراف_Rect(&rcDlg, -rcDlg.بایاں, -rcDlg.چوٹی);
    انحراف_Rect(&rc, -rc.بایاں, -rc.چوٹی);
    انحراف_Rect(&rc, -rcDlg.دایاں, -rcDlg.bottom);

    // The new position is the sum of half the remaining
    // space and the owner's original position.

    سیٹ_کھڑکی_Pos(hDlg,
	    HWND_چوٹی,
	    rcOwner.بایاں + (rc.دایاں / 2),
	    rcOwner.چوٹی + (rc.bottom / 2),
	    0, 0,		// ignores size arguments
	    SWP_NOSIZE);

    //  tackle the printdlg copiesctrl problem
    pPD = (PRINTDLGW *)lParam;
    pPD->nCopies = (لفظ_2)pPD->l_Cust_ڈیٹا;
    سیٹ_Dlg_شے_Int( hDlg, edt3, pPD->nCopies, FALSE );
    //  Bring the window to top
    Bring_کھڑکی_مقصد_چوٹی(Get_والد(hDlg));
    سیٹ_Foreground_کھڑکی(hDlg);

    واپس FALSE;
}

    خالی
mch_print_cleanup(خالی)
{
    صحیح pifItalic;
    صحیح pifBold;
    صحیح pifUnderline;

    برائے (pifBold = 0; pifBold <= 1; pifBold++)
	برائے (pifItalic = 0; pifItalic <= 1; pifItalic++)
	    برائے (pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
		DeleteObject(prt_font_handles[pifBold][pifItalic][pifUnderline]);

    اگر (prt_dlg.hDC != تہی)
	DeleteDC(prt_dlg.hDC);
    اگر (!*bUserAbort && hDlgPrint != تہی)
	Send_پیغام(hDlgPrint, WM_COMMAND, 0, 0);
}

    ساکن صحیح
مقصد_device_units(صحیح idx, صحیح dpi, صحیح physsize, صحیح انحراف, صحیح def_نمبر)
{
    صحیح		ret = 0;
    صحیح		u;
    صحیح		nr;

    u = prt_get_unit(idx);
    اگر (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_نمبر;
    }
    ورنہ
	nr = printer_opts[idx].نمبر;

    انتخاب (u)
    {
	حالت PRT_UNIT_PERC:
	    ret = (physsize * nr) / 100;
	    توڑو;
	حالت PRT_UNIT_INCH:
	    ret = (nr * dpi);
	    توڑو;
	حالت PRT_UNIT_MM:
	    ret = (nr * 10 * dpi) / 254;
	    توڑو;
	حالت PRT_UNIT_نقطہ:
	    ret = (nr * 10 * dpi) / 720;
	    توڑو;
    }

    اگر (ret < انحراف)
	واپس 0;
    ورنہ
	واپس ret - انحراف;
}

    ساکن صحیح
prt_get_cpl(خالی)
{
    صحیح		hr;
    صحیح		phyw;
    صحیح		dvoff;
    صحیح		rev_انحراف;
    صحیح		dpi;

    Get_متن_Metrics(prt_dlg.hDC, &prt_tm);
    prt_سطر_اونچائی = prt_tm.tm_اونچائی + prt_tm.tmExternalLeading;

    hr	    = GetDeviceCaps(prt_dlg.hDC, HORZRES);
    phyw    = GetDeviceCaps(prt_dlg.hDC, PHYSICALWIDTH);
    dvoff   = GetDeviceCaps(prt_dlg.hDC, PHYSICALOFFSETX);
    dpi	    = GetDeviceCaps(prt_dlg.hDC, LOGPIXELSX);

    rev_انحراف = phyw - (dvoff + hr);

    prt_بایاں_margin = مقصد_device_units(OPT_PRINT_بایاں, dpi, phyw, dvoff, 10);
    اگر (prt_use_نمبر())
    {
	prt_نمبر_چوڑائی = PRINT_نمبر_چوڑائی * prt_tm.tm_Ave_Char_چوڑائی;
	prt_بایاں_margin += prt_نمبر_چوڑائی;
    }
    ورنہ
	prt_نمبر_چوڑائی = 0;

    prt_دایاں_margin = hr - مقصد_device_units(OPT_PRINT_دایاں, dpi, phyw,
							       rev_انحراف, 5);

    واپس (prt_دایاں_margin - prt_بایاں_margin) / prt_tm.tm_Ave_Char_چوڑائی;
}

    ساکن صحیح
prt_get_lpp(خالی)
{
    صحیح vr;
    صحیح phyw;
    صحیح dvoff;
    صحیح rev_انحراف;
    صحیح	bottom_margin;
    صحیح	dpi;

    vr	    = GetDeviceCaps(prt_dlg.hDC, VERTRES);
    phyw    = GetDeviceCaps(prt_dlg.hDC, PHYSICALHEIGHT);
    dvoff   = GetDeviceCaps(prt_dlg.hDC, PHYSICALOFFSETY);
    dpi	    = GetDeviceCaps(prt_dlg.hDC, LOGPIXELSY);

    rev_انحراف = phyw - (dvoff + vr);

    prt_چوٹی_margin = مقصد_device_units(OPT_PRINT_چوٹی, dpi, phyw, dvoff, 5);

    // adjust top margin if there is a header
    prt_چوٹی_margin += prt_سطر_اونچائی * prt_سرخی_اونچائی();

    bottom_margin = vr - مقصد_device_units(OPT_PRINT_BOT, dpi, phyw,
							       rev_انحراف, 5);

    واپس (bottom_margin - prt_چوٹی_margin) / prt_سطر_اونچائی;
}

    صحیح
mch_print_init(prt_settings_T *psettings, char_u *jobname, صحیح forceit)
{
    ساکن HGLOBAL	stored_dm = تہی;
    ساکن HGLOBAL	stored_devn = تہی;
    ساکن صحیح		stored_nCopies = 1;
    ساکن صحیح		stored_nFlags = 0;

    LOGFONTW		fLogFont;
    صحیح			pifItalic;
    صحیح			pifBold;
    صحیح			pifUnderline;

    DEVMODEW		*mem;
    DEVNAMES		*devname;
    صحیح			i;
    DWORD		err;

    bUserAbort = &(psettings->user_abort);
    CLEAR_میدان(prt_dlg);
    prt_dlg.l_Struct_سائز = حجم(PRINTDLGW);
# اگر !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
    اگر (!gui.اندر_use)
#  endif
	GetConsoleHwnd();	    // get value of s_hwnd
# endif
    prt_dlg.hwndOwner = s_hwnd;
    prt_dlg.Flags = PD_NOPAGENUMS | PD_NOSELECTION | PD_RETURNDC;
    اگر (!forceit)
    {
	prt_dlg.h_Dev_منوال = stored_dm;
	prt_dlg.h_Dev_نام = stored_devn;
	prt_dlg.l_Cust_ڈیٹا = stored_nCopies; // work around bug in print dialog
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
	    || (!gui.اندر_use && !رکن_console)
#  ورنہ
	    || !رکن_console
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
	    prt_dlg.hDC = CreateDC(تہی, (LPCSTR)p_pdev, تہی, تہی);
	ورنہ
	{
	    prt_dlg.Flags |= PD_RETURNDEFAULT;
	    اگر (PrintDlgW(&prt_dlg) == 0)
		جاؤ init_fail_dlg;
	}
    }
    ورنہ اگر (PrintDlgW(&prt_dlg) == 0)
	جاؤ init_fail_dlg;
    ورنہ
    {
	/*
	 * keep the previous driver context
	 */
	stored_dm = prt_dlg.h_Dev_منوال;
	stored_devn = prt_dlg.h_Dev_نام;
	stored_nFlags = prt_dlg.Flags;
	stored_nCopies = prt_dlg.nCopies;
    }

    اگر (prt_dlg.hDC == تہی)
    {
	emsg(_(e_printer_selection_failed));
	mch_print_cleanup();
	واپس FALSE;
    }

    // Not all printer drivers report the support of color (or grey) in the
    // same way.  Let's set has_color if there appears to be some way to print
    // more than B&W.
    i = GetDeviceCaps(prt_dlg.hDC, NUMCOLORS);
    psettings->has_color = (GetDeviceCaps(prt_dlg.hDC, BITSPIXEL) > 1
				   || GetDeviceCaps(prt_dlg.hDC, PLANES) > 1
				   || i > 2 || i == -1);

    // Ensure all font styles are baseline aligned
    سیٹ_متن_Align(prt_dlg.hDC, TA_BASELINE|TA_بایاں);

    /*
     * On some windows systems the nCopies parameter is not
     * passed back correctly. It must be retrieved from the
     * hDevMode struct.
     */
    mem = (DEVMODEW *)Global_تالا(prt_dlg.h_Dev_منوال);
    اگر (mem != تہی)
    {
	اگر (mem->dmCopies != 1)
	    stored_nCopies = mem->dmCopies;
	اگر ((mem->dmFields & DM_DUPLEX) && (mem->dmDuplex & ~DMDUP_SIMPLEX))
	    psettings->duplex = TRUE;
	اگر ((mem->dmFields & DM_COLOR) && (mem->dmColor & DMCOLOR_COLOR))
	    psettings->has_color = TRUE;
    }
    GlobalUnlock(prt_dlg.h_Dev_منوال);

    devname = (DEVNAMES *)Global_تالا(prt_dlg.h_Dev_نام);
    اگر (devname != 0)
    {
	WCHAR	*wprinter_name = (WCHAR *)devname + devname->w_Device_انحراف;
	WCHAR	*wport_name = (WCHAR *)devname + devname->w_Output_انحراف;
	char_u	*متن = (char_u *)_("to %s on %s");
	char_u  *printer_name = utf16_مقصد_enc(wprinter_name, تہی);
	char_u	*پورٹ_name = utf16_مقصد_enc(wport_name, تہی);

	اگر (printer_name != تہی && پورٹ_name != تہی)
	    prt_name = alloc(STRLEN(printer_name)
					   + STRLEN(پورٹ_name) + STRLEN(متن));
	اگر (prt_name != تہی)
	    wsprintf((حرف *)prt_name, (ثابت حرف *)متن,
		    printer_name, پورٹ_name);
	vim_free(printer_name);
	vim_free(پورٹ_name);
    }
    GlobalUnlock(prt_dlg.h_Dev_نام);

    /*
     * Initialise the font according to 'printfont'
     */
    CLEAR_میدان(fLogFont);
    اگر (get_logfont(&fLogFont, p_pfn, prt_dlg.hDC, TRUE) == FAIL)
    {
	semsg(_(e_unknown_printer_font_str), p_pfn);
	mch_print_cleanup();
	واپس FALSE;
    }

    برائے (pifBold = 0; pifBold <= 1; pifBold++)
	برائے (pifItalic = 0; pifItalic <= 1; pifItalic++)
	    برائے (pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
	    {
		fLogFont.lf_وزن =  boldface[pifBold];
		fLogFont.lfItalic = pifItalic;
		fLogFont.lfUnderline = pifUnderline;
		prt_font_handles[pifBold][pifItalic][pifUnderline]
					      = Create_Font_بالواسطہ_W(&fLogFont);
	    }

    سیٹ_Bk_منوال(prt_dlg.hDC, OPAQUE);
    SelectObject(prt_dlg.hDC, prt_font_handles[0][0][0]);

    /*
     * Fill in the settings struct
     */
    psettings->chars_فی_سطر = prt_get_cpl();
    psettings->سطریں_فی_صفحہ = prt_get_lpp();
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
    ورنہ
    {
	psettings->n_collated_copies = 1;
	psettings->n_uncollated_copies = 1;
    }

    psettings->jobname = jobname;

    واپس TRUE;

init_fail_dlg:
    err = CommDlgExtendedError();
    اگر (err)
    {
	char_u *buf;

	// I suspect FormatMessage() doesn't work for values returned by
	// CommDlgExtendedError().  What does?
	Format_پیغام(FORMAT_پیغام_ALLOCATE_بفر |
		FORMAT_پیغام_FROM_SYSTEM |
		FORMAT_پیغام_IGNORE_INSERTS,
		تہی, err, 0, (LPTSTR)(&buf), 0, تہی);
	semsg(_(e_print_error_str),
		buf == تہی ? (char_u *)_("Unknown") : buf);
	LocalFree((LPVOID)(buf));
    }
    ورنہ
	پیغام_clr_eos(); // Maybe canceled

    mch_print_cleanup();
    واپس FALSE;
}


    صحیح
mch_print_begin(prt_settings_T *psettings)
{
    صحیح			ret = 0;
    حرف		sz_بفر[300];
    WCHAR		*wp;

    hDlgPrint = CreateDialog(g_hinst, متن_3("PrintDlgBox"),
					     prt_dlg.hwndOwner, PrintDlgProc);
    سیٹ_Abort_Proc(prt_dlg.hDC, AbortProc);
    wsprintf(sz_بفر, _("Printing '%s'"), gettail(psettings->jobname));
    vim_سیٹ_Dlg_شے_متن(hDlgPrint, IDC_PRINTTEXT1, (char_u *)sz_بفر);

    wp = enc_مقصد_utf16(psettings->jobname, تہی);
    اگر (wp != تہی)
    {
	DOCINFOW	di;

	CLEAR_میدان(di);
	di.cb_سائز = حجم(di);
	di.lpszDocName = wp;
	ret = آغاز_Doc_W(prt_dlg.hDC, &di);
	vim_free(wp);
    }

# ifdef FEAT_GUI
    // Give focus back to main window (when using MDI).
#  ifdef VIMDLL
    اگر (gui.اندر_use)
#  endif
	سیٹ_Focus(s_hwnd);
# endif

    واپس (ret > 0);
}

    خالی
mch_print_انتہا(prt_settings_T *psettings غیر_مستعمل)
{
    انتہا_Doc(prt_dlg.hDC);
    اگر (!*bUserAbort && hDlgPrint != تہی)
	Send_پیغام(hDlgPrint, WM_COMMAND, 0, 0);
}

    صحیح
mch_print_انتہا_صفحہ(خالی)
{
    واپس (انتہا_صفحہ(prt_dlg.hDC) > 0);
}

    صحیح
mch_print_begin_صفحہ(char_u *پیغام)
{
    اگر (پیغام != تہی)
	vim_سیٹ_Dlg_شے_متن(hDlgPrint, IDC_پیش_رفت, پیغام);
    واپس (آغاز_صفحہ(prt_dlg.hDC) > 0);
}

    صحیح
mch_print_blank_صفحہ(خالی)
{
    واپس (mch_print_begin_صفحہ(تہی) ? (mch_print_انتہا_صفحہ()) : FALSE);
}

ساکن صحیح prt_pos_x = 0;
ساکن صحیح prt_pos_y = 0;

    خالی
mch_print_آغاز_سطر(صحیح margin, صحیح صفحہ_سطر)
{
    اگر (margin)
	prt_pos_x = -prt_نمبر_چوڑائی;
    ورنہ
	prt_pos_x = 0;
    prt_pos_y = صفحہ_سطر * prt_سطر_اونچائی
				 + prt_tm.tmAscent + prt_tm.tmExternalLeading;
}

    صحیح
mch_print_متن_برآمد(char_u *p, صحیح len)
{
    سائز_3	sz;
    WCHAR	*wp;
    صحیح		wlen = len;
    صحیح		ret = FALSE;

    wp = enc_مقصد_utf16(p, &wlen);
    اگر (wp == تہی)
	واپس FALSE;

    متن_برآمد_W(prt_dlg.hDC, prt_pos_x + prt_بایاں_margin,
	    prt_pos_y + prt_چوٹی_margin, wp, wlen);
    Get_متن_Extent_Point32_W(prt_dlg.hDC, wp, wlen, &sz);
    vim_free(wp);
    prt_pos_x += (sz.cx - prt_tm.tmOverhang);
    // This is wrong when printing spaces for a TAB.
    اگر (p[len] != NUL)
    {
	wlen = mb_ptr2len(p + len);
	wp = enc_مقصد_utf16(p + len, &wlen);
	اگر (wp != تہی)
	{
	    Get_متن_Extent_Point32_W(prt_dlg.hDC, wp, 1, &sz);
	    ret = (prt_pos_x + prt_بایاں_margin + sz.cx > prt_دایاں_margin);
	    vim_free(wp);
	}
    }
    واپس ret;
}

    خالی
mch_print_سیٹ_font(صحیح iBold, صحیح iItalic, صحیح iUnderline)
{
    SelectObject(prt_dlg.hDC, prt_font_handles[iBold][iItalic][iUnderline]);
}

    خالی
mch_print_سیٹ_bg(long_u bgcol)
{
    سیٹ_Bk_Color(prt_dlg.hDC, Get_قریب_ترین_Color(prt_dlg.hDC,
						   تبادلہ_me((COLORREF)bgcol)));
    /*
     * With a white background we can draw characters transparent, which is
     * good for italic characters that overlap to the next char cell.
     */
    اگر (bgcol == 0xffffffUL)
	سیٹ_Bk_منوال(prt_dlg.hDC, TRANSPARENT);
    ورنہ
	سیٹ_Bk_منوال(prt_dlg.hDC, OPAQUE);
}

    خالی
mch_print_سیٹ_fg(long_u fgcol)
{
    سیٹ_متن_Color(prt_dlg.hDC, Get_قریب_ترین_Color(prt_dlg.hDC,
						   تبادلہ_me((COLORREF)fgcol)));
}

#اگر_ختم // FEAT_PRINTER && !FEAT_POSTSCRIPT



#اگر defined(FEAT_SHORTCUT)
# include <shlobj.h>

# define is_راستہ_sep(c)	    ((c) == L'\\' || (c) == L'/')

    ساکن صحیح
is_reparse_نقطہ_included(LPCWSTR fname)
{
    LPCWSTR	p = fname, q;
    WCHAR	buf[MAX_راستہ];
    DWORD	attr;

    اگر (محفوظ_isalpha(p[0]) && p[1] == L':' && is_راستہ_sep(p[2]))
	p += 3;
    ورنہ اگر (is_راستہ_sep(p[0]) && is_راستہ_sep(p[1]))
	p += 2;

    جب (*p != L'\0')
    {
	q = wcspbrk(p, L"\\/");
	اگر (q == تہی)
	    p = q = fname + wcslen(fname);
	ورنہ
	    p = q + 1;
	اگر (q - fname >= MAX_راستہ)
	    واپس FALSE;
	wcsncpy(buf, fname, q - fname);
	buf[q - fname] = L'\0';
	attr = Get_File_اوصاف_W(buf);
	اگر (attr != INVALID_FILE_اوصاف
		&& (attr & FILE_ATTRIBUTE_REPARSE_نقطہ) != 0)
	    واپس TRUE;
    }
    واپس FALSE;
}

/*
 * Return the resolved file path, NULL if "fname" is an AppExecLink reparse
 * point, already fully resolved, or it doesn't exists.
 */
    char_u *
resolve_reparse_نقطہ(char_u *fname)
{
    HANDLE	    h = INVALID_HANDLE_قدر;
    DWORD	    سائز;
    WCHAR	    *p, *wp;
    char_u	    *rfname = تہی;
    WCHAR	    *buff = تہی;

    p = enc_مقصد_utf16(fname, تہی);
    اگر (p == تہی)
	جاؤ fail;

    اگر (!is_reparse_نقطہ_included(p))
    {
	vim_free(p);
	جاؤ fail;
    }

    h = CreateFileW(p, 0, 0, تہی, OPEN_EXISTING,
	    FILE_FLAG_BACKUP_SEMANTICS, تہی);
    vim_free(p);

    اگر (h == INVALID_HANDLE_قدر)
	جاؤ fail;

    سائز = Get_Final_راستہ_Name_By_Handle_W(h, تہی, 0, 0);
    اگر (سائز == 0)
	جاؤ fail;
    buff = ALLOC_MULT(WCHAR, سائز);
    اگر (buff == تہی)
	جاؤ fail;
    اگر (Get_Final_راستہ_Name_By_Handle_W(h, buff, سائز, 0) == 0)
	جاؤ fail;

    اگر (wcsncmp(buff, L"\\\\?\\UNC\\", 8) == 0)
    {
	buff[6] = L'\\';
	wp = buff + 6;
    }
    ورنہ اگر (wcsncmp(buff, L"\\\\?\\", 4) == 0)
	wp = buff + 4;
    ورنہ
	wp = buff;

    rfname = utf16_مقصد_enc(wp, تہی);

fail:
    اگر (h != INVALID_HANDLE_قدر)
	CloseHandle(h);
    اگر (buff != تہی)
	vim_free(buff);

    واپس rfname;
}

/*
 * When "fname" is the name of a shortcut (*.lnk) resolve the file it points
 * to and return that name in allocated memory.
 * Otherwise NULL is returned.
 */
    ساکن char_u *
resolve_shortcut(char_u *fname)
{
    HRESULT		hr;
    IShellLink		*psl = تہی;
    IPersistFile	*ppf = تہی;
    OLECHAR		wsz[MAX_راستہ];
    char_u		*rfname = تہی;
    صحیح			len;
    IShellLinkW		*pslw = تہی;
    WIN32_ڈھونڈو_DATAW	ffdw; // we get those free of charge

    // Check if the file name ends in ".lnk". Avoid calling
    // CoCreateInstance(), it's quite slow.
    اگر (fname == تہی)
	واپس rfname;
    len = (صحیح)STRLEN(fname);
    اگر (len <= 4 || STRNICMP(fname + len - 4, ".lnk", 4) != 0)
	واپس rfname;

    CoInitialize(تہی);

    // create a link manager object and request its interface
    hr = CoCreateInstance(
	    &CLSID_ShellLink, تہی, CLSCTX_INPROC_SERVER,
	    &IID_IShellLinkW, (خالی**)&pslw);
    اگر (hr == S_OK)
    {
	WCHAR	*p = enc_مقصد_utf16(fname, تہی);

	اگر (p != تہی)
	{
	    // Get a pointer to the IPersistFile interface.
	    hr = pslw->lpVtbl->QueryInterface(
		    pslw, &IID_IPersistFile, (خالی**)&ppf);
	    اگر (hr != S_OK)
		جاؤ shortcut_errorw;

	    // "load" the name and resolve the link
	    hr = ppf->lpVtbl->بھرو_2(ppf, p, STGM_READ);
	    اگر (hr != S_OK)
		جاؤ shortcut_errorw;
# اگر 0  // This makes Vim wait a long time if the target does not exist.
	    hr = pslw->lpVtbl->Resolve(pslw, تہی, SLR_NO_UI);
	    اگر (hr != S_OK)
		جاؤ shortcut_errorw;
# endif

	    // Get the path to the link target.
	    Zero_حافظہ(wsz, MAX_راستہ * حجم(WCHAR));
	    hr = pslw->lpVtbl->Get_راستہ(pslw, wsz, MAX_راستہ, &ffdw, 0);
	    اگر (hr == S_OK && wsz[0] != NUL)
		rfname = utf16_مقصد_enc(wsz, تہی);

shortcut_errorw:
	    vim_free(p);
	}
    }

    // Release all interface pointers (both belong to the same object)
    اگر (ppf != تہی)
	ppf->lpVtbl->Release(ppf);
    اگر (psl != تہی)
	psl->lpVtbl->Release(psl);
    اگر (pslw != تہی)
	pslw->lpVtbl->Release(pslw);

    CoUninitialize();
    واپس rfname;
}

    char_u *
mch_resolve_راستہ(char_u *fname, صحیح reparse_نقطہ)
{
    char_u  *راستہ = resolve_shortcut(fname);

    اگر (راستہ == تہی && reparse_نقطہ)
	راستہ = resolve_reparse_نقطہ(fname);
    واپس راستہ;
}
#اگر_ختم

#اگر defined(FEAT_EVAL) && (!defined(FEAT_GUI) || defined(VIMDLL))
/*
 * Bring ourselves to the foreground.  Does work if the OS doesn't allow it.
 */
    خالی
win32_سیٹ_foreground(خالی)
{
    GetConsoleHwnd();	    // get value of s_hwnd
    اگر (s_hwnd != 0)
	سیٹ_Foreground_کھڑکی(s_hwnd);
}
#اگر_ختم

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
HWND پیغام_کھڑکی = 0;	    // window that's handling messages

# define VIM_CLASSNAME      "VIM_MESSAGES"
# define VIM_CLASSNAME_LEN  (حجم(VIM_CLASSNAME) - 1)

// Timeout for sending a message to another Vim instance.  Normally this works
// instantly, but it may hang when the other Vim instance is halted.
# define SENDMESSAGE_TIMEOUT	(5 * 1000)

// Communication is via WM_COPYDATA messages. The message type is sent in
// the dwData parameter. Types are defined here.
# define COPYDATA_کلیدیں		0
# define COPYDATA_جواب		1
# define COPYDATA_اظہاریہ		10
# define COPYDATA_نتیجہ	11
# define COPYDATA_ERROR_نتیجہ	12
# define COPYDATA_ENCODING	20

// This is a structure containing a server HWND and its name.
ساخت server_id
{
    HWND hwnd;
    char_u *name;
};

// Last received 'encoding' that the client uses.
ساکن char_u	*client_enc = تہی;

/*
 * Tell the other side what encoding we are using.
 * Return -1 if timeout happens.  Other errors are ignored.
 */
    ساکن صحیح
serverSendEnc(HWND ہدف)
{
    COPYDATASTRUCT ڈیٹا;

    ڈیٹا.dw_ڈیٹا = COPYDATA_ENCODING;
    ڈیٹا.cb_ڈیٹا = (DWORD)STRLEN(p_enc) + 1;
    ڈیٹا.lp_ڈیٹا = p_enc;
    اگر (Send_پیغام_Timeout(ہدف, WM_COPYDATA,
	    (WPARAM)پیغام_کھڑکی, (LPARAM)&ڈیٹا,
	    SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, تہی) == 0)
	واپس -1;
    واپس 0;
}

/*
 * Clean up on exit. This destroys the hidden message window.
 */
    ساکن خالی
Clean_اوپر_Messaging(خالی)
{
    اگر (پیغام_کھڑکی == 0)
	واپس;

    Destroy_کھڑکی(پیغام_کھڑکی);
    پیغام_کھڑکی = 0;
}

ساکن صحیح save_جواب(HWND server, char_u *جواب, صحیح اظہاریہ);

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
    ساکن LRESULT CALLBACK
Messaging_WndProc(HWND hwnd, UINT پیغام, WPARAM wParam, LPARAM lParam)
{
    اگر (پیغام == WM_COPYDATA)
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
	COPYDATASTRUCT	*ڈیٹا = (COPYDATASTRUCT*)lParam;
	HWND		sender = (HWND)wParam;
	COPYDATASTRUCT	جواب;
	char_u		*res;
	صحیح		retval;
	DWORD_PTR	dwret = 0;
	char_u		*str;
	char_u		*tofree;

	انتخاب (ڈیٹا->dw_ڈیٹا)
	{
	حالت COPYDATA_ENCODING:
	    // Remember the encoding that the client uses.
	    vim_free(client_enc);
	    client_enc = enc_canonize((char_u *)ڈیٹا->lp_ڈیٹا);
	    واپس 1;

	حالت COPYDATA_کلیدیں:
	    // Remember who sent this, for <client>
	    client_کھڑکی = sender;

	    // Add the received keys to the input buffer.
	    str = serverConvert(client_enc, (char_u *)ڈیٹا->lp_ڈیٹا, &tofree);
# ifdef FEAT_GUI
	    اگر (gui.اندر_use)
		// The GUI may get here re-entrantly, so insert the keys later.
		server_جوڑو_داخلہ(str);
	    ورنہ
# endif
		server_مقصد_داخلہ_buf(str);
	    vim_free(tofree);

# ifdef FEAT_GUI
	    // Wake up the main GUI loop.
#  ifdef VIMDLL
	    اگر (gui.اندر_use)
#  endif
		اگر (s_hwnd != 0)
		    Post_پیغام(s_hwnd, WM_NULL, 0, 0);
# endif
	    واپس 1;

	حالت COPYDATA_اظہاریہ:
	    // Remember who sent this, for <client>
	    client_کھڑکی = sender;

	    str = serverConvert(client_enc, (char_u *)ڈیٹا->lp_ڈیٹا, &tofree);
	    res = eval_client_اظہاریہ_مقصد_string(str);

	    اگر (res == تہی)
	    {
		حرف	*err = _(e_invalid_عبارت_received);
		size_t	len = STRLEN(str) + STRLEN(err) + 5;

		res = alloc(len);
		اگر (res != تہی)
		    جواب.cb_ڈیٹا = (DWORD)vim_snprintf_safelen(
			(حرف *)res, len, "%s: \"%s\"", err, str);
		ورنہ
		    جواب.cb_ڈیٹا = 0;
		جواب.dw_ڈیٹا = COPYDATA_ERROR_نتیجہ;
	    }
	    ورنہ
	    {
		جواب.cb_ڈیٹا = (DWORD)STRLEN(res) + 1;
		جواب.dw_ڈیٹا = COPYDATA_نتیجہ;
	    }
	    جواب.lp_ڈیٹا = res;

	    اگر (serverSendEnc(sender) < 0)
		retval = -1;
	    ورنہ
	    {
		اگر (Send_پیغام_Timeout(sender, WM_COPYDATA,
			(WPARAM)پیغام_کھڑکی, (LPARAM)&جواب,
			SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
		    retval = -1;
		ورنہ
		    retval = (صحیح)dwret;
	    }
	    vim_free(tofree);
	    vim_free(res);
	    واپس retval;

	حالت COPYDATA_جواب:
	حالت COPYDATA_نتیجہ:
	حالت COPYDATA_ERROR_نتیجہ:
	    اگر (ڈیٹا->lp_ڈیٹا != تہی)
	    {
		str = serverConvert(client_enc, (char_u *)ڈیٹا->lp_ڈیٹا,
								     &tofree);
		اگر (tofree == تہی)
		    str = vim_strsave(str);
		اگر (save_جواب(sender, str,
			   (ڈیٹا->dw_ڈیٹا == COPYDATA_جواب ?  0 :
			   (ڈیٹا->dw_ڈیٹا == COPYDATA_نتیجہ ? 1 :
							      2))) == FAIL)
		    vim_free(str);
		ورنہ اگر (ڈیٹا->dw_ڈیٹا == COPYDATA_جواب)
		{
		    char_u	winstr[30];

		    sprintf((حرف *)winstr, PRINTF_HEX_LONG_U, (long_u)sender);
		    apply_autocmds(EVENT_REMOTEREPLY, winstr, str,
								TRUE, curbuf);
		}
	    }
	    واپس 1;
	}

	واپس 0;
    }

    ورنہ اگر (پیغام == WM_ACTIVATE && wParam == WA_فعال)
    {
	// When the message window is activated (brought to the foreground),
	// this actually applies to the text window.
# اگر !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	اگر (!gui.اندر_use)
#  endif
	    GetConsoleHwnd();	    // get value of s_hwnd
# endif
	اگر (s_hwnd != 0)
	{
	    سیٹ_Foreground_کھڑکی(s_hwnd);
	    واپس 0;
	}
    }

    واپس Def_کھڑکی_Proc(hwnd, پیغام, wParam, lParam);
}

/*
 * Initialise the message handling process.  This involves creating a window
 * to handle messages - the window will not be visible.
 */
    خالی
serverInitMessaging(خالی)
{
    WNDCLASS wndclass;

    // Clean up on exit
    atexit(Clean_اوپر_Messaging);

    // Register a window class - we only really care
    // about the window procedure
    wndclass.style = 0;
    wndclass.lpfnWndProc = Messaging_WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = g_hinst;
    wndclass.hIcon = تہی;
    wndclass.hCursor = تہی;
    wndclass.hbrBackground = تہی;
    wndclass.lpszMenuName = تہی;
    wndclass.lpszClassName = VIM_CLASSNAME;
    رجسٹر_Class(&wndclass);

    // Create the message window. It will be hidden, so the details don't
    // matter.  Don't use WS_OVERLAPPEDWINDOW, it will make a shortcut remove
    // focus from gvim.
    پیغام_کھڑکی = Create_کھڑکی(VIM_CLASSNAME, "",
			 WS_POPUPWINDOW | WS_CAPTION,
			 CW_USEDEFAULT, CW_USEDEFAULT,
			 100, 100, تہی, تہی,
			 g_hinst, تہی);
}

// Used by serverSendToVim() to find an alternate server name.
ساکن char_u *altname_buf_ptr = تہی;

/*
 * Get the title of the window "hwnd", which is the Vim server name, in
 * "name[namelen]" and return the length.
 * Returns zero if window "hwnd" is not a Vim server.
 */
    ساکن صحیح
getVimServerName(HWND hwnd, حرف *name, صحیح namelen)
{
    صحیح		len;
    حرف	بفر[VIM_CLASSNAME_LEN + 1];

    // Ignore windows which aren't Vim message windows
    len = GetClassName(hwnd, بفر, حجم(بفر));
    اگر (len != VIM_CLASSNAME_LEN || STRCMP(بفر, VIM_CLASSNAME) != 0)
	واپس 0;

    // Get the title of the window
    واپس Get_کھڑکی_متن(hwnd, name, namelen);
}

    ساکن BOOL CALLBACK
enumWindowsGetServer(HWND hwnd, LPARAM lparam)
{
    ساخت	server_id *id = (ساخت server_id *)lparam;
    حرف	server[MAX_راستہ];

    // Get the title of the window
    اگر (getVimServerName(hwnd, server, حجم(server)) == 0)
	واپس TRUE;

    // If this is the server we're looking for, return its HWND
    اگر (STRICMP(server, id->name) == 0)
    {
	id->hwnd = hwnd;
	واپس FALSE;
    }

    // If we are looking for an alternate server, remember this name.
    اگر (altname_buf_ptr != تہی)
    {
	size_t	namelen = STRLEN(id->name);

	اگر (STRNICMP(server, id->name, namelen) == 0
	    && vim_isdigit(server[namelen]))
	{
	    STRCPY(altname_buf_ptr, server);
	    altname_buf_ptr = تہی;	    // don't use another name
	}
    }

    // Otherwise, keep looking
    واپس TRUE;
}

    ساکن BOOL CALLBACK
enum_Windows_Get_نام(HWND hwnd, LPARAM lparam)
{
    list_T	*list = (list_T *)lparam;
    حرف	server[MAX_راستہ];

    // Get the title of the window
    اگر (getVimServerName(hwnd, server, حجم(server)) == 0)
	واپس TRUE;

    list_append_string(list, (char_u *)server, -1);
    واپس TRUE;
}

ساخت enum_windows_s
{
    WNDENUMPROC lpEnumFunc;
    LPARAM      lParam;
};

    ساکن BOOL CALLBACK
enum_windows_بچہ(HWND hwnd, LPARAM lParam)
{
    ساخت enum_windows_s *ew = (ساخت enum_windows_s *)lParam;

    واپس (ew->lpEnumFunc)(hwnd, ew->lParam);
}

    ساکن BOOL CALLBACK
enum_windows_toplevel(HWND hwnd, LPARAM lParam)
{
    ساخت enum_windows_s *ew = (ساخت enum_windows_s *)lParam;

    اگر ((ew->lpEnumFunc)(hwnd, ew->lParam))
	واپس TRUE;
    واپس Enum_بچہ_Windows(hwnd, enum_windows_بچہ, lParam);
}

/*
 * Enumerate all windows including children.
 */
    ساکن BOOL
enum_windows(WNDENUMPROC lpEnumFunc, LPARAM lParam)
{
    ساخت enum_windows_s ew;

    ew.lpEnumFunc = lpEnumFunc;
    ew.lParam = lParam;
    واپس EnumWindows(enum_windows_toplevel, (LPARAM)&ew);
}

    ساکن HWND
ڈھونڈو_Server(char_u *name)
{
    ساخت server_id id;

    id.name = name;
    id.hwnd = 0;

    enum_windows(enumWindowsGetServer, (LPARAM)(&id));

    واپس id.hwnd;
}

    خالی
server_سیٹ_Name(char_u *name)
{
    size_t	namelen;
    char_u	*ok_name;
    HWND	hwnd = 0;
    صحیح		i = 0;
    char_u	*p;

    // Leave enough space for a 9-digit suffix to ensure uniqueness!
    namelen = STRLEN(name);
    ok_name = alloc(namelen + 10);
    اگر (ok_name == تہی)
	واپس;

    STRCPY(ok_name, name);
    p = ok_name + namelen;

    برائے (;;)
    {
	// This is inefficient - we're doing an EnumWindows loop for each
	// possible name. It would be better to grab all names in one go,
	// and scan the list each time...
	hwnd = ڈھونڈو_Server(ok_name);
	اگر (hwnd == 0)
	    توڑو;

	++i;
	اگر (i >= 1000)
	    توڑو;

	sprintf((حرف *)p, "%d", i);
    }

    اگر (hwnd != 0)
	vim_free(ok_name);
    ورنہ
    {
	// Remember the name
	serverName = ok_name;
	need_maketitle = TRUE;	// update Vim window title later

	// Update the message window title
	سیٹ_کھڑکی_متن(پیغام_کھڑکی, (LPCSTR)ok_name);

# ifdef FEAT_EVAL
	// Set the servername variable
	سیٹ_vim_var_string(VV_SEND_SERVER, serverName, -1);
# endif
    }
}

    list_T *
server_Get_Vim_نام(خالی)
{
    list_T *list = list_alloc();

    اگر (list == تہی)
	واپس تہی;

    enum_windows(enum_Windows_Get_نام, (LPARAM)list);

    واپس list;
}

    صحیح
server_Send_جواب(
    char_u	*name,		// Where to send.
    char_u	*جواب)		// What to send.
{
    HWND	ہدف;
    COPYDATASTRUCT ڈیٹا;
    long_u	n = 0;
    DWORD_PTR	dwret = 0;

    // The "name" argument is a magic cookie obtained from expand("<client>").
    // It should be of the form 0xXXXXX - i.e. a C hex literal, which is the
    // value of the client's message window HWND.
    لڑی_سے_پڑھو((حرف *)name, SCANF_HEX_LONG_U, &n);
    اگر (n == 0)
	واپس -1;

    ہدف = (HWND)n;
    اگر (!Is_کھڑکی(ہدف))
	واپس -1;

    ڈیٹا.dw_ڈیٹا = COPYDATA_جواب;
    ڈیٹا.cb_ڈیٹا = (DWORD)STRLEN(جواب) + 1;
    ڈیٹا.lp_ڈیٹا = جواب;

    اگر (serverSendEnc(ہدف) < 0)
	واپس -1;
    اگر (Send_پیغام_Timeout(ہدف, WM_COPYDATA,
		(WPARAM)پیغام_کھڑکی, (LPARAM)&ڈیٹا,
		SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
	واپس -1;
    واپس dwret ? 0 : -1;
}

    صحیح
server_Send_مقصد_Vim(
    char_u	 *name,			// Where to send.
    char_u	 *cmd,			// What to send.
    char_u	 **نتیجہ,		// Result of eval'ed expression
    خالی	 *ptarget,		// HWND of server
    صحیح		 as_اظہاریہ,		// Expression or keys?
    صحیح		 timeout,		// timeout in seconds or zero
    صحیح		 silent)		// don't complain about no server
{
    size_t	namelen;
    HWND	ہدف;
    COPYDATASTRUCT ڈیٹا;
    char_u	*retval = تہی;
    صحیح		retcode = 0;
    DWORD_PTR	dwret = 0;
    char_u	altname_buf[MAX_راستہ];

    // Execute locally if no display or target is ourselves
    اگر (serverName != تہی && STRICMP(name, serverName) == 0)
	واپس send_مقصد_Local_Vim(cmd, as_اظہاریہ, نتیجہ);

    // If the server name does not end in a digit then we look for an
    // alternate name.  e.g. when "name" is GVIM then we may find GVIM2.
    namelen = STRLEN(name);
    اگر (namelen > 1 && !vim_isdigit(name[namelen - 1]))
	altname_buf_ptr = altname_buf;
    altname_buf[0] = NUL;
    ہدف = ڈھونڈو_Server(name);
    altname_buf_ptr = تہی;
    اگر (ہدف == 0 && altname_buf[0] != NUL)
	// Use another server name we found.
	ہدف = ڈھونڈو_Server(altname_buf);

    اگر (ہدف == 0)
    {
	اگر (!silent)
	    semsg(_(e_no_registered_server_named_str), name);
	واپس -1;
    }

    اگر (ptarget)
	*(HWND *)ptarget = ہدف;

    ڈیٹا.dw_ڈیٹا = as_اظہاریہ ? COPYDATA_اظہاریہ : COPYDATA_کلیدیں;
    ڈیٹا.cb_ڈیٹا = (DWORD)STRLEN(cmd) + 1;
    ڈیٹا.lp_ڈیٹا = cmd;

    اگر (serverSendEnc(ہدف) < 0)
	واپس -1;
    اگر (Send_پیغام_Timeout(ہدف, WM_COPYDATA,
		(WPARAM)پیغام_کھڑکی, (LPARAM)&ڈیٹا,
		SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
	واپس -1;
    اگر (dwret == 0)
	واپس -1;

    اگر (as_اظہاریہ)
	retval = server_Get_جواب(ہدف, &retcode, TRUE, TRUE, timeout);

    اگر (نتیجہ == تہی)
	vim_free(retval);
    ورنہ
	*نتیجہ = retval; // Caller assumes responsibility for freeing

    واپس retcode;
}

/*
 * Bring the server to the foreground.
 */
    خالی
serverForeground(char_u *name)
{
    HWND	ہدف = ڈھونڈو_Server(name);

    اگر (ہدف != 0)
	سیٹ_Foreground_کھڑکی(ہدف);
}

// Replies from server need to be stored until the client picks them up via
// remote_read(). So we maintain a list of server-id/reply pairs.
// Note that there could be multiple replies from one server pending if the
// client is slow picking them up.
// We just store the replies in a simple list. When we remove an entry, we
// move list entries down to fill the gap.
// The server ID is simply the HWND.
قسمتعریف ساخت
{
    HWND	server;		// server window
    char_u	*جواب;		// reply string
    صحیح		اظہاریہ_نتیجہ;	// 0 for REPLY, 1 for RESULT 2 for error
} جواب_T;

ساکن garray_T جواب_list = {0, 0, حجم(جواب_T), 5, 0};

# define جواب_شے(i) ((جواب_T *)(جواب_list.ga_ڈیٹا) + (i))
# define جواب_گنتی (جواب_list.ga_len)

// Flag which is used to wait for a reply
ساکن صحیح جواب_received = 0;

/*
 * Store a reply.  "reply" must be allocated memory (or NULL).
 */
    ساکن صحیح
save_جواب(HWND server, char_u *جواب, صحیح اظہاریہ)
{
    جواب_T *rep;

    اگر (ga_grow(&جواب_list, 1) == FAIL)
	واپس FAIL;

    rep = جواب_شے(جواب_گنتی);
    rep->server = server;
    rep->جواب = جواب;
    rep->اظہاریہ_نتیجہ = اظہاریہ;
    اگر (rep->جواب == تہی)
	واپس FAIL;

    ++جواب_گنتی;
    جواب_received = 1;
    واپس OK;
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
server_Get_جواب(HWND server, صحیح *اظہاریہ_res, صحیح remove, صحیح wait, صحیح timeout)
{
    صحیح		i;
    char_u	*جواب;
    جواب_T	*rep;
    صحیح		did_process = FALSE;
    time_t	آغاز;
    time_t	now;

    // When waiting, loop until the message waiting for is received.
    وقت(&آغاز);
    برائے (;;)
    {
	// Reset this here, in case a message arrives while we are going
	// through the already received messages.
	جواب_received = 0;

	برائے (i = 0; i < جواب_گنتی; ++i)
	{
	    rep = جواب_شے(i);
	    اگر (rep->server == server
		    && ((rep->اظہاریہ_نتیجہ != 0) == (اظہاریہ_res != تہی)))
	    {
		// Save the values we've found for later
		جواب = rep->جواب;
		اگر (اظہاریہ_res != تہی)
		    *اظہاریہ_res = rep->اظہاریہ_نتیجہ == 1 ? 0 : -1;

		اگر (remove)
		{
		    // Move the rest of the list down to fill the gap
		    mch_memmove(rep, rep + 1,
				     (جواب_گنتی - i - 1) * حجم(جواب_T));
		    --جواب_گنتی;
		}

		// Return the reply to the caller, who takes on responsibility
		// for freeing it if "remove" is TRUE.
		واپس جواب;
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
		جاری;
	    }
	    توڑو;
	}

	// We need to wait for a reply. Enter a message loop until the
	// "reply_received" flag gets set.

	// Loop until we receive a reply
	جب (جواب_received == 0)
	{
# ifdef FEAT_TIMERS
	    // TODO: use the return value to decide how long to wait.
	    check_due_timer();
# endif
	    وقت(&now);
	    اگر (timeout > 0 && (now - آغاز) >= timeout)
		توڑو;

	    // Wait for a SendMessage() call to us.  This could be the reply
	    // we are waiting for.  Use a timeout of a second, to catch the
	    // situation that the server died unexpectedly.
	    پیغام_Wait_For_مضرب_Objects(0, تہی, TRUE, 1000, QS_ALLINPUT);

	    // If the server has died, give up
	    اگر (!Is_کھڑکی(server))
		واپس تہی;

	    serverProcessPendingMessages();
	}
    }

    واپس تہی;
}

/*
 * Process any messages in the Windows message queue.
 */
    خالی
serverProcessPendingMessages(خالی)
{
    پیغام_3 پیغام;

    جب (Peek_پیغام_W(&پیغام, تہی, 0, 0, PM_REMOVE))
    {
	Translate_پیغام(&پیغام);
	Dispatch_پیغام_W(&پیغام);
    }
}

#اگر_ختم // FEAT_CLIENTSERVER

#اگر defined(FEAT_GUI) || (defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT))

ساخت charset_pair
{
    string_T	name;
    BYTE	charset;
};

ساکن ساخت charset_pair
charset_جوڑے[] =
{
    {STR_LITERAL_INIT("ANSI"),		ANSI_CHARSET},
    {STR_LITERAL_INIT("CHINESEBIG5"),	CHINESEBIG5_CHARSET},
    {STR_LITERAL_INIT("DEFAULT"),	DEFAULT_CHARSET},
    {STR_LITERAL_INIT("HANGEUL"),	HANGEUL_CHARSET},
    {STR_LITERAL_INIT("OEM"),		OEM_CHARSET},
    {STR_LITERAL_INIT("SHIFTJIS"),	SHIFTJIS_CHARSET},
    {STR_LITERAL_INIT("SYMBOL"),	علامت_CHARSET},
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

ساخت quality_pair
{
    string_T	name;
    DWORD	quality;
};

ساکن ساخت quality_pair
quality_جوڑے[] = {
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
charset_id2name(صحیح id)
{
    صحیح	i;

    برائے (i = 0; i < (صحیح)ARRAY_لمبائی(charset_جوڑے); ++i)
    {
	اگر ((BYTE)id == charset_جوڑے[i].charset)
	    واپس (حرف *)charset_جوڑے[i].name.string;
    }

    واپس تہی;
}

/*
 * Convert a quality ID to a name.
 * Return NULL when not recognized.
 */
    حرف *
quality_id2name(DWORD id)
{
    صحیح	i;

    برائے (i = 0; i < (صحیح)ARRAY_لمبائی(quality_جوڑے); ++i)
    {
	اگر (id == quality_جوڑے[i].quality)
	    واپس (حرف *)quality_جوڑے[i].name.string;
    }

    واپس تہی;
}

// The default font height in 100% scaling (96dpi).
// (-16 in 96dpi equates to roughly 12pt)
# define DEFAULT_FONT_اونچائی	(-16)

ساکن ثابت LOGFONTW s_lfDefault =
{
    DEFAULT_FONT_اونچائی,
    0, 0, 0, FW_معمول, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
    برآمد_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
    PROOF_QUALITY, FIXED_PITCH | FF_DONTCARE,
    L""	// Default font name will be set later based on current language.
};

// This will be initialized when set_default_logfont() is called first time.
// The value will be based on the system DPI.
صحیح current_font_اونچائی = 0;		// also used in gui_w32.c

/*
 * Convert a string representing a point size into pixels. The string should
 * be a positive decimal number, with an optional decimal point (eg, "12", or
 * "10.5"). The pixel value is returned, and a pointer to the next unconverted
 * character is stored in *end. The flag "vertical" says whether this
 * calculation is for a vertical (height) size or a horizontal (width) one.
 */
    ساکن صحیح
points_مقصد_pixels(WCHAR *str, WCHAR **انتہا, صحیح vertical, long_i pprinter_dc)
{
    صحیح		pixels;
    صحیح		points = 0;
    صحیح		مقسوم_علیہ = 0;
    HWND	hwnd = (HWND)0;
    HDC		hdc;
    HDC		printer_dc = (HDC)pprinter_dc;

    جب (*str != NUL)
    {
	اگر (*str == L'.' && مقسوم_علیہ == 0)
	{
	    // Start keeping a divisor, for later
	    مقسوم_علیہ = 1;
	}
	ورنہ
	{
	    اگر (!VIM_ISDIGIT(*str))
		توڑو;

	    points *= 10;
	    points += *str - L'0';
	    مقسوم_علیہ *= 10;
	}
	++str;
    }

    اگر (مقسوم_علیہ == 0)
	مقسوم_علیہ = 1;

    اگر (printer_dc == تہی)
    {
	hwnd = Get_Desktop_کھڑکی();
	hdc = Get_کھڑکی_DC(hwnd);
    }
    ورنہ
	hdc = printer_dc;

    pixels = MulDiv(points,
		    GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX),
		    72 * مقسوم_علیہ);

    اگر (printer_dc == تہی)
	ReleaseDC(hwnd, hdc);

    *انتہا = str;
    واپس pixels;
}

/*
 * Convert pixel into point size. This is a reverse of points_to_pixels.
 */
    ساکن دوہرا
pixels_مقصد_points(صحیح pixels, صحیح vertical, long_i pprinter_dc)
{
    دوہرا	points = 0;
    HWND	hwnd = (HWND)0;
    HDC		hdc;
    HDC		printer_dc = (HDC)pprinter_dc;

    اگر (printer_dc == تہی)
    {
	hwnd = Get_Desktop_کھڑکی();
	hdc = Get_کھڑکی_DC(hwnd);
    }
    ورنہ
	hdc = printer_dc;

    points = pixels * 72.0 / GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX);
    اگر (printer_dc == تہی)
	ReleaseDC(hwnd, hdc);

    واپس points;
}

    ساکن صحیح CALLBACK
font_enumproc(
    ENUMLOGFONTW    *elf,
    NEWTEXTMETRICW  *ntm غیر_مستعمل,
    DWORD	    type غیر_مستعمل,
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
	واپس 1;
# endif

    // Remember this LOGFONTW as a "possible"
    *lf = elf->elfLogFont;

    // Terminate the scan as soon as we find an ANSI font
    اگر (lf->lf_Char_سیٹ == ANSI_CHARSET
	    || lf->lf_Char_سیٹ == OEM_CHARSET
	    || lf->lf_Char_سیٹ == DEFAULT_CHARSET)
	واپس 0;

    // Continue the scan - we have a non-ANSI font
    واپس 2;
}

    ساکن صحیح
init_logfont(LOGFONTW *lf)
{
    صحیح		n;
    HWND	hwnd = Get_Desktop_کھڑکی();
    HDC		hdc = Get_کھڑکی_DC(hwnd);

    n = EnumFontFamiliesW(hdc,
			 lf->lfFaceName,
			 (FONTENUMPROCW)font_enumproc,
			 (LPARAM)lf);

    ReleaseDC(hwnd, hdc);

    // If we couldn't find a usable font, return failure
    اگر (n == 1)
	واپس FAIL;

    // Tidy up the rest of the LOGFONTW structure. We set to a basic
    // font - get_logfont() sets bold, italic, etc based on the user's
    // input.
    lf->lf_اونچائی = current_font_اونچائی;
    lf->lf_چوڑائی = 0;
    lf->lfItalic = FALSE;
    lf->lfUnderline = FALSE;
    lf->lf_Strike_برآمد = FALSE;
    lf->lf_وزن = FW_معمول;

    // Return success
    واپس OK;
}

/*
 * Call back for EnumFontFamiliesW in expand_font_enumproc.
 *
 */
    ساکن صحیح CALLBACK
expand_font_enumproc(
    ENUMLOGFONTW    *elf,
    NEWTEXTMETRICW  *ntm غیر_مستعمل,
    DWORD	    type غیر_مستعمل,
    LPARAM	    lparam)
{
    LOGFONTW *lf = (LOGFONTW*)elf;

# ifndef FEAT_PROPORTIONAL_FONTS
    // Ignore non-monospace fonts without further ado
    اگر ((ntm->tmPitchAndFamily & 1) != 0)
	واپس 1;
# endif

    // Filter only on ANSI. Otherwise will see a lot of random fonts that we
    // usually don't want.
    اگر (lf->lf_Char_سیٹ != ANSI_CHARSET)
	واپس 1;

    صحیح (*جوڑو_مطابقت)(char_u *) = (صحیح (*)(char_u *))lparam;

    WCHAR *faceNameW = lf->lfFaceName;
    char_u *faceName = utf16_مقصد_enc(faceNameW, تہی);
    اگر (!faceName)
	واپس 0;

    جوڑو_مطابقت(faceName);
    vim_free(faceName);

    واپس 1;
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
    خالی
gui_mch_expand_font(
    optexpand_T	*دلائل,
    خالی	*param غیر_مستعمل,
    صحیح		(*جوڑو_مطابقت)(char_u *val))
{
    expand_T	    *xp = دلائل->oe_xp;
    اگر (xp->xp_نمونہ > دلائل->oe_سیٹ_arg && *(xp->xp_نمونہ-1) == ':')
    {
	حرف buf[30];
	صحیح	i;

	// Always fill in with the current font size as first option for
	// convenience. We simply round to the closest integer for simplicity.
	صحیح font_اونچائی = (صحیح)باری(
		pixels_مقصد_points(-current_font_اونچائی, TRUE, (long_i)تہی));
	vim_snprintf(buf, ARRAY_لمبائی(buf), "h%d", font_اونچائی);
	جوڑو_مطابقت((char_u *)buf);

	// Note: Keep this in sync with get_logfont(). Don't include 'c' and
	// 'q' as we fill in all the values below.
	ساکن حرف *(p_gfn_win_opt_اقدار[]) = {
	    "h" , "w" , "W" , "b" , "i" , "u" , "s"};
	برائے (i = 0; i < (صحیح)ARRAY_لمبائی(p_gfn_win_opt_اقدار); i++)
	    جوڑو_مطابقت((char_u *)p_gfn_win_opt_اقدار[i]);

	برائے (i = 0; i < (صحیح)ARRAY_لمبائی(charset_جوڑے); ++i)
	{
	    vim_snprintf(buf, حجم(buf), "c%s", charset_جوڑے[i].name.string);
	    جوڑو_مطابقت((char_u *)buf);
	}

	برائے (i = 0; i < (صحیح)ARRAY_لمبائی(quality_جوڑے); ++i)
	{
	    vim_snprintf(buf, حجم(buf), "q%s", quality_جوڑے[i].name.string);
	    جوڑو_مطابقت((char_u *)buf);
	}
	واپس;
    }

    HWND	hwnd = Get_Desktop_کھڑکی();
    HDC		hdc = Get_کھڑکی_DC(hwnd);

    EnumFontFamiliesW(hdc,
	    تہی,
	    (FONTENUMPROCW)expand_font_enumproc,
	    (LPARAM)جوڑو_مطابقت);

    ReleaseDC(hwnd, hdc);
}

/*
 * Compare a UTF-16 string and an ASCII string literally.
 * Only works all the code points are inside ASCII range.
 */
    ساکن صحیح
utf16ascncmp(ثابت WCHAR *w, ثابت حرف *p, size_t n)
{
    size_t i;

    برائے (i = 0; i < n; i++)
    {
	اگر (w[i] == 0 || w[i] != p[i])
	    واپس w[i] - p[i];
    }
    واپس 0;
}

/*
 * Equivalent of GetDpiForSystem().
 */
    UINT WINAPI
vimGetDpiForSystem(خالی)
{
    HWND hwnd = Get_Desktop_کھڑکی();
    HDC hdc = Get_کھڑکی_DC(hwnd);
    UINT dpi = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(hwnd, hdc);
    واپس dpi;
}

/*
 * Set default logfont based on current language.
 */
    ساکن خالی
سیٹ_default_logfont(LOGFONTW *lf)
{
    // Default font name for current language on MS-Windows.
    // If not translated, falls back to "Consolas".
    // This must be a fixed-pitch font.
    ثابت حرف *defaultfontname = N_("DefaultFontNameForWindows");
    حرف *fontname = _(defaultfontname);

    اگر (STRCMP(fontname, defaultfontname) == 0)
	fontname = "Consolas";

    *lf = s_lfDefault;
    lf->lf_اونچائی = DEFAULT_FONT_اونچائی * (صحیح)vimGetDpiForSystem() / 96;
    اگر (current_font_اونچائی == 0)
	current_font_اونچائی = lf->lf_اونچائی;

    WCHAR *wfontname = enc_مقصد_utf16((char_u*)fontname, تہی);
    اگر (wfontname != تہی)
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
    ساکن LOGFONTW *lastlf = تہی;
    WCHAR	*wname;

    سیٹ_default_logfont(lf);
    اگر (name == تہی)
	واپس OK;

    wname = enc_مقصد_utf16(name, تہی);
    اگر (wname == تہی)
	واپس FAIL;

    اگر (wcscmp(wname, L"*") == 0)
    {
# اگر defined(FEAT_GUI_MSWIN)
	CHOOSEFONTW	cf;
	// if name is "*", bring up std font dialog:
	CLEAR_میدان(cf);
	cf.l_Struct_سائز = حجم(cf);
	cf.hwndOwner = s_hwnd;
	cf.Flags = CF_SCREENFONTS | CF_FIXEDPITCHONLY | CF_INITTOLOGFONTSTRUCT;
	اگر (lastlf != تہی)
	    *lf = *lastlf;
	cf.lpLogFont = lf;
	cf.nFontType = 0 ; //REGULAR_FONTTYPE;
	اگر (انتخاب_Font_W(&cf))
	    ret = OK;
# endif
	جاؤ theend;
    }

    /*
     * Split name up, it could be <name>:h<height>:w<width> etc.
     */
    برائے (p = wname; *p && *p != L':'; p++)
    {
	اگر (p - wname + 1 >= LF_FACESIZE)
	    جاؤ theend;			// Name too long
	lf->lfFaceName[p - wname] = *p;
    }
    اگر (p != wname)
	lf->lfFaceName[p - wname] = NUL;

    // First set defaults
    lf->lf_اونچائی = DEFAULT_FONT_اونچائی * (صحیح)vimGetDpiForSystem() / 96;
    lf->lf_چوڑائی = 0;
    lf->lf_وزن = FW_معمول;
    lf->lfItalic = FALSE;
    lf->lfUnderline = FALSE;
    lf->lf_Strike_برآمد = FALSE;

    /*
     * If the font can't be found, try replacing '_' by ' '.
     */
    اگر (init_logfont(lf) == FAIL)
    {
	صحیح	did_replace = FALSE;

	برائے (i = 0; lf->lfFaceName[i]; ++i)
	    اگر (lf->lfFaceName[i] == L'_')
	    {
		lf->lfFaceName[i] = L' ';
		did_replace = TRUE;
	    }
	اگر (!did_replace || init_logfont(lf) == FAIL)
	    جاؤ theend;
    }

    جب (*p == L':')
	p++;

    // Set the values found after ':'
    جب (*p)
    {
	انتخاب (*p++)
	{
	    // Note: Keep this in sync with gui_mch_expand_font().
	    حالت L'h':
		lf->lf_اونچائی = - points_مقصد_pixels(p, &p, TRUE, (long_i)printer_dc);
		توڑو;
	    حالت L'w':
		lf->lf_چوڑائی = points_مقصد_pixels(p, &p, FALSE, (long_i)printer_dc);
		توڑو;
	    حالت L'W':
		lf->lf_وزن = wcstol(p, &p, 10);
		توڑو;
	    حالت L'b':
		lf->lf_وزن = FW_BOLD;
		توڑو;
	    حالت L'i':
		lf->lfItalic = TRUE;
		توڑو;
	    حالت L'u':
		lf->lfUnderline = TRUE;
		توڑو;
	    حالت L's':
		lf->lf_Strike_برآمد = TRUE;
		توڑو;
	    حالت L'c':
		برائے (i = 0; i < (صحیح)ARRAY_لمبائی(charset_جوڑے); ++i)
		{
		    اگر (utf16ascncmp(p, (حرف *)charset_جوڑے[i].name.string,
			    charset_جوڑے[i].name.لمبائی) == 0)
		    {
			lf->lf_Char_سیٹ = charset_جوڑے[i].charset;
			p += charset_جوڑے[i].name.لمبائی;
			توڑو;
		    }
		}

		اگر (i == (صحیح)ARRAY_لمبائی(charset_جوڑے) && verbose)
		{
		    char_u *s = utf16_مقصد_enc(p, تہی);
		    اگر (s != تہی)
		    {
			semsg(_(e_illegal_str_name_str_اندر_font_name_str),
							   "charset", s, name);
			vim_free(s);
		    }
		}
		توڑو;
	    حالت L'f':
		// Font features (e.g., "fss19=1").
		// Parsed separately by gui_mch_init_font(); skip here.
		جب (*p && *p != L':')
		    p++;
		توڑو;
	    حالت L'q':
		برائے (i = 0; i < (صحیح)ARRAY_لمبائی(quality_جوڑے); ++i)
		{
		    اگر (utf16ascncmp(p, (حرف *)quality_جوڑے[i].name.string,
			    quality_جوڑے[i].name.لمبائی) == 0)
		    {
			lf->lfQuality = quality_جوڑے[i].quality;
			p += quality_جوڑے[i].name.لمبائی;
			توڑو;
		    }
		}

		اگر (i == (صحیح)ARRAY_لمبائی(quality_جوڑے) && verbose)
		{
		    char_u *s = utf16_مقصد_enc(p, تہی);
		    اگر (s != تہی)
		    {
			semsg(_(e_illegal_str_name_str_اندر_font_name_str),
							   "quality", s, name);
			vim_free(s);
		    }
		}
		توڑو;
	    طےشدہ:
		اگر (verbose)
		    semsg(_(e_illegal_char_nr_اندر_font_name_str), p[-1], name);
		جاؤ theend;
	}
	جب (*p == L':')
	    p++;
    }
    ret = OK;

theend:
    // ron: init lastlf
    اگر (ret == OK && printer_dc == تہی)
    {
	vim_free(lastlf);
	lastlf = ALLOC_ONE(LOGFONTW);
	اگر (lastlf != تہی)
	    mch_memmove(lastlf, lf, حجم(LOGFONTW));
    }
    vim_free(wname);

    واپس ret;
}

#اگر_ختم // defined(FEAT_GUI) || defined(FEAT_PRINTER)

#اگر defined(FEAT_کام_CHANNEL)
/*
 * Initialize the Winsock dll.
 */
    خالی
channel_init_winsock(خالی)
{
    WSADATA wsa_ڈیٹا;
    صحیح wsaerr;

    اگر (WSInitialized)
	واپس;

    wsaerr = WSAStartup(MAKEWORD(2, 2), &wsa_ڈیٹا);
    اگر (wsaerr == 0)
	WSInitialized = TRUE;
}
#اگر_ختم
