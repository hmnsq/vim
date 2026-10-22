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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಅನ್ತರ್ಭಾವ <sys/types.h>
#ಅನ್ತರ್ಭಾವ <limits.h>

#ಅನ್ತರ್ಭಾವ <process.h>
#ಅನ್ತರ್ಭಾವ <direct.h>

#ಆದರೆ !defined(FEAT_GUI_MSWIN)
# include <shellapi.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT)
# include <dlgs.h>
# include <winspool.h>
# include <commdlg.h>
#ಆದರೆ_ಅಂತ್ಯ

// Record all output and all keyboard & mouse input
// #define MCH_WRITE_DUMP

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MCH_WRITE_DUMP
FILE* fdDump = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
ಬಾಹ್ಯ ಅಕ್ಷರ g_szOrigTitle[];
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
ಬಾಹ್ಯ HWND s_hwnd;
#ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ HWND s_hwnd = 0;	    // console window handle, set by GetConsoleHwnd()
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
ಪೂರ್ಣಾಂಕ WSInitialized = FALSE; // WinSock is initialized
#ಆದರೆ_ಅಂತ್ಯ


/*
 * Save the instance handle of the exe/dll.
 */
    ಶೂನ್ಯ
SaveInst(HINSTANCE hInst)
{
    g_hinst = hInst;
}

#ಆದರೆ defined(FEAT_GUI_MSWIN)
/*
 * GUI version of mch_exit().
 * Shut down and exit with status `r'
 * Careful: mch_exit() may be called before mch_init()!
 */
    ಶೂನ್ಯ
mch_exit_g(ಪೂರ್ಣಾಂಕ r)
{
    exiting = TRUE;

    display_errors();

    ml_close_all(TRUE);		// remove all memfiles

# ifdef FEAT_OLE
    UninitOLE();
# endif
# ifdef FEAT_ಕೆಲಸ_CHANNEL
    ಆದರೆ (WSInitialized)
    {
	WSInitialized = FALSE;
	WSACleanup();
    }
# endif
# ifdef DYNAMIC_GETTEXT
    dyn_libintl_ಕೊನೆ();
# endif

    ಆದರೆ (gui.ಒಳ_use)
	gui_exit(r);

# ifdef EXITFREE
    free_all_mem();
# endif

    ನಿರ್ಗಮಿಸು(r);
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI_MSWIN


/*
 * Get version number including build number
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ BOOL (WINAPI *Pfn_Rtl_Get_ಆವೃತ್ತಿ)(LPOSVERSIONINFOW);
DWORD win_ಆವೃತ್ತಿ;
    ಸ್ಥಿರ ಶೂನ್ಯ
win_ಆವೃತ್ತಿ_init(ಶೂನ್ಯ)
{
    OSVERSIONINFOW	osver;
    HMODULE		hNtdll;
    Pfn_Rtl_Get_ಆವೃತ್ತಿ	p_Rtl_Get_ಆವೃತ್ತಿ;

    hNtdll = GetModuleHandle("ntdll.dll");
    ಆದರೆ (hNtdll == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    p_Rtl_Get_ಆವೃತ್ತಿ =
	(Pfn_Rtl_Get_ಆವೃತ್ತಿ) Get_Proc_ವಿಳಾಸ(hNtdll, "RtlGetVersion");
    ಆದರೆ (p_Rtl_Get_ಆವೃತ್ತಿ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    osver.dw_OSVersion_Info_ಗಾತ್ರ = ಗಾತ್ರ(OSVERSIONINFOW);
    p_Rtl_Get_ಆವೃತ್ತಿ(&osver);
    win_ಆವೃತ್ತಿ = MAKE_VER(min(osver.dw_Major_ಆವೃತ್ತಿ, 0xFF),
	    min(osver.dw_Minor_ಆವೃತ್ತಿ, 0xFF),
	    min(osver.dw_Build_ಸಂಖ್ಯೆ, 0xFFFF));
}

/*
 * Init the tables for toupper() and tolower().
 */
    ಶೂನ್ಯ
mch_early_init(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		i;

    win_ಆವೃತ್ತಿ_init();

    PlatformId();

    // Init the tables for toupper() and tolower()
    ಗಾಗಿ (i = 0; i < 256; ++i)
	toupper_tab[i] = tolower_tab[i] = i;
    Char_ಊರ್ಧ್ವ_Buff((LPSTR)toupper_tab, 256);
    Char_ಅಧೋ_Buff((LPSTR)tolower_tab, 256);
}


/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    ಪೂರ್ಣಾಂಕ
mch_ಒಳಹರಿವು_isatty(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
# ifdef VIMDLL
    ಆದರೆ (gui.ಒಳ_use)
# endif
	ಹಿಂತಿರುಗಿ TRUE;	    // GUI always has a tty
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    ಆದರೆ (isatty(read_cmd_fd))
	ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * mch_settitle(): set titlebar of our window
 */
    ಶೂನ್ಯ
mch_settitle(
    char_u *title,
    char_u *icon ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
# ifdef VIMDLL
    ಆದರೆ (gui.ಒಳ_use)
# endif
    {
	gui_mch_settitle(title, icon);
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
    ಆದರೆ (title != ಖಾಲಿ)
    {
	WCHAR	*wp = enc_ಗಮ್ಯ_utf16(title, ಖಾಲಿ);

	ಆದರೆ (wp == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;

	ಗಣ_Console_Title_W(wp);
	vim_free(wp);
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ
}


/*
 * Restore the window/icon title.
 * which is one of:
 *  SAVE_RESTORE_TITLE: Just restore title
 *  SAVE_RESTORE_ICON:  Just restore icon (which we don't have)
 *  SAVE_RESTORE_BOTH:  Restore title and icon (which we don't have)
 */
    ಶೂನ್ಯ
mch_restore_title(ಪೂರ್ಣಾಂಕ which ಬಳಸದ)
{
#ಆದರೆ !defined(FEAT_GUI_MSWIN) || defined(VIMDLL)
# ifdef VIMDLL
    ಆದರೆ (!gui.ಒಳ_use)
# endif
	ಗಣ_Console_Title(g_szOrigTitle);
#ಆದರೆ_ಅಂತ್ಯ
}


/*
 * Return TRUE if we can restore the title (we can)
 */
    ಪೂರ್ಣಾಂಕ
mch_can_restore_title(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ TRUE;
}


/*
 * Return TRUE if we can restore the icon title (we can't)
 */
    ಪೂರ್ಣಾಂಕ
mch_can_restore_icon(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ FALSE;
}


/*
 * Get absolute file name into buffer "buf" of length "len" bytes,
 * turning all '/'s into '\\'s and getting the correct case of each component
 * of the file name.  Append a (back)slash to a directory name.
 * When 'shellslash' set do it the other way around.
 * Return OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
mch_ತುಂಬಿದ_Name(
    char_u	*fname,
    char_u	*buf,
    ಪೂರ್ಣಾಂಕ		len,
    ಪೂರ್ಣಾಂಕ		force ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		n_ಫಲಿತಾಂಶ = FAIL;
    WCHAR	*wname;
    WCHAR	wbuf[MAX_ಮಾರ್ಗ];
    char_u	*cname = ಖಾಲಿ;

    wname = enc_ಗಮ್ಯ_utf16(fname, ಖಾಲಿ);
    ಆದರೆ (wname != ಖಾಲಿ && _wfullpath(wbuf, wname, MAX_ಮಾರ್ಗ) != ಖಾಲಿ)
    {
	cname = utf16_ಗಮ್ಯ_enc((short_u *)wbuf, ಖಾಲಿ);
	ಆದರೆ (cname != ಖಾಲಿ)
	{
	    vim_strncpy(buf, cname, len - 1);
	    n_ಫಲಿತಾಂಶ = OK;
	}
    }
    vim_free(wname);
    vim_free(cname);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_FNAME_CASE
    fname_case(buf, len);
#ಇಲ್ಲದಿದ್ದರೆ
    slash_adjust(buf);
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ n_ಫಲಿತಾಂಶ;
}


/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    ಪೂರ್ಣಾಂಕ
mch_is_ತುಂಬಿದ_Name(char_u *fname)
{
    // A name like "d:/foo" and "//server/share" is absolute.  "d:foo" is not.
    // /foo and \foo are absolute too because Windows keeps a current drive.
    // Another way to check is to use mch_FullName() and see if the result is
    // the same as the name or mch_FullName() fails.  However, this has quite a
    // bit of overhead, so let's not do that.
    ಆದರೆ (*fname == NUL)
	ಹಿಂತಿರುಗಿ FALSE;
    ಹಿಂತಿರುಗಿ ((ASCII_ISALPHA(fname[0]) && fname[1] == ':'
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
    ಶೂನ್ಯ
slash_adjust(char_u *p)
{
    ಆದರೆ (ಮಾರ್ಗ_with_url(p))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (*p == '`')
    {
	size_t len = STRLEN(p);

	// don't replace backslash in backtick quoted strings
	ಆದರೆ (len > 2 && *(p + len - 1) == '`')
	    ಹಿಂತಿರುಗಿ;
    }

    ಆಗ (*p)
    {
	ಆದರೆ (*p == psepcN)
	    *p = psepc;
	MB_PTR_ADV(p);
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
read_reparse_ಬಿಂದು(ಸ್ಥಿರಾಂಕ WCHAR *name, char_u *buf, DWORD *buf_len)
{
    HANDLE h;
    BOOL ok;

    h = CreateFileW(name, FILE_READ_ಗುಣಲಕ್ಷಣಗಳು,
	    FILE_SHARE_READ | FILE_SHARE_WRITE, ಖಾಲಿ,
	    OPEN_EXISTING,
	    FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_ಬಿಂದು,
	    ಖಾಲಿ);
    ಆದರೆ (h == INVALID_HANDLE_ಮೌಲ್ಯ)
	ಹಿಂತಿರುಗಿ FAIL;

    ok = DeviceIoControl(h, FSCTL_GET_REPARSE_ಬಿಂದು, ಖಾಲಿ, 0, buf, *buf_len,
	    buf_len, ಖಾಲಿ);
    CloseHandle(h);

    ಹಿಂತಿರುಗಿ ok ? OK : FAIL;
}

    char_u *
resolve_appexeclink(char_u *fname)
{
    DWORD		attr = 0;
    ಪೂರ್ಣಾಂಕ			idx;
    WCHAR		*p, *ಕೊನೆ, *wname;
    // The buffer size is arbitrarily chosen to be "big enough" (TM), the
    // ceiling should be around 16k.
    char_u		buf[4096];
    DWORD		buf_len = ಗಾತ್ರ(buf);
    REPARSE_ದತ್ತಾಂಶ_ಬಫರ್ *rb = (REPARSE_ದತ್ತಾಂಶ_ಬಫರ್ *)buf;

    wname = enc_ಗಮ್ಯ_utf16(fname, ಖಾಲಿ);
    ಆದರೆ (wname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    attr = Get_File_ಗುಣಲಕ್ಷಣಗಳು_W(wname);
    ಆದರೆ (attr == INVALID_FILE_ಗುಣಲಕ್ಷಣಗಳು ||
	    (attr & FILE_ATTRIBUTE_REPARSE_ಬಿಂದು) == 0)
    {
	vim_free(wname);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // The applinks are similar to symlinks but with a huge difference: they can
    // only be executed, any other I/O operation on them is bound to fail with
    // ERROR_FILE_NOT_FOUND even though the file exists.
    ಆದರೆ (read_reparse_ಬಿಂದು(wname, buf, &buf_len) == FAIL)
    {
	vim_free(wname);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    vim_free(wname);

    ಆದರೆ (rb->Reparse_ಟ್ಯಾಗ್ != IO_REPARSE_ಟ್ಯಾಗ್_APPEXECLINK)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // The (undocumented) reparse buffer contains a set of N null-terminated
    // Unicode strings, the application path is stored in the third one.
    ಆದರೆ (rb->App_Exec_Link_Reparse_ಬಫರ್.String_ಎಣಿಕೆ < 3)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    p = rb->App_Exec_Link_Reparse_ಬಫರ್.StringList;
    ಕೊನೆ = p + rb->Reparse_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ / ಗಾತ್ರ(WCHAR);
    ಗಾಗಿ (idx = 0; p < ಕೊನೆ
	    && idx < (ಪೂರ್ಣಾಂಕ)rb->App_Exec_Link_Reparse_ಬಫರ್.String_ಎಣಿಕೆ
	    && idx != 2; )
    {
	ಆದರೆ (*p++ == L'\0')
	    ++idx;
    }

    ಹಿಂತಿರುಗಿ utf16_ಗಮ್ಯ_enc(p, ಖಾಲಿ);
}

// Use 64-bit stat functions.
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು stat
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು _stat
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು _wstat
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು _fstat
#ವ್ಯಾಖ್ಯಾನಿಸು stat _stat64
#ವ್ಯಾಖ್ಯಾನಿಸು _stat _stat64
#ವ್ಯಾಖ್ಯಾನಿಸು _wstat _wstat64
#ವ್ಯಾಖ್ಯಾನಿಸು _fstat _fstat64

/*
 * Implements lstat() and stat() that can handle symlinks properly.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
mswin_stat_impl(ಸ್ಥಿರಾಂಕ WCHAR *name, stat_T *stp, ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ resolve)
{
    ಪೂರ್ಣಾಂಕ			n;
    ಪೂರ್ಣಾಂಕ			fd;
    BOOL		is_symlink = FALSE;
    HANDLE		h_ಕಂಡುಹಿಡಿ, h;
    DWORD		attr = 0;
    DWORD		flag = 0;
    WIN32_ಕಂಡುಹಿಡಿ_DATAW    ಕಂಡುಹಿಡಿ_ದತ್ತಾಂಶ_W;

#ಆದರೆ 0 && defined(_UCRT)
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
    ಆದರೆ (resolve)
	// Universal CRT can handle symlinks properly.
	ಹಿಂತಿರುಗಿ _wstat(name, stp);
#ಆದರೆ_ಅಂತ್ಯ

    // Only reparse points need a directory query to identify symbolic links.
    // In particular, avoid enumerating the directory for each missing file
    // while searching 'runtimepath'.
    attr = Get_File_ಗುಣಲಕ್ಷಣಗಳು_W(name);
    ಆದರೆ (attr == INVALID_FILE_ಗುಣಲಕ್ಷಣಗಳು)
    {
	DWORD error = Get_ಕೊನೆಯ_Error();

	// A missing file or path cannot provide stat information.  Other
	// errors and UNC paths still need the CRT fallback.
	ಆದರೆ ((error == ERROR_FILE_NOT_ಸಿಕ್ಕಿತು || error == ERROR_ಮಾರ್ಗ_NOT_ಸಿಕ್ಕಿತು)
		&& !((name[0] == L'\\' || name[0] == L'/')
		    && (name[1] == L'\\' || name[1] == L'/')))
	{
	    errno = ENOENT;
	    ಹಿಂತಿರುಗಿ -1;
	}
	ಹಿಂತಿರುಗಿ _wstat(name, stp);
    }
    ಆದರೆ (!(attr & FILE_ATTRIBUTE_REPARSE_ಬಿಂದು))
	ಹಿಂತಿರುಗಿ _wstat(name, stp);

    h_ಕಂಡುಹಿಡಿ = ಕಂಡುಹಿಡಿ_ಮೊದಲ_File_W(name, &ಕಂಡುಹಿಡಿ_ದತ್ತಾಂಶ_W);
    ಆದರೆ (h_ಕಂಡುಹಿಡಿ != INVALID_HANDLE_ಮೌಲ್ಯ)
    {
	attr = ಕಂಡುಹಿಡಿ_ದತ್ತಾಂಶ_W.dw_File_ಗುಣಲಕ್ಷಣಗಳು;
	ಆದರೆ ((attr & FILE_ATTRIBUTE_REPARSE_ಬಿಂದು)
		&& (ಕಂಡುಹಿಡಿ_ದತ್ತಾಂಶ_W.dwReserved0 == IO_REPARSE_ಟ್ಯಾಗ್_SYMLINK))
	    is_symlink = TRUE;
	ಕಂಡುಹಿಡಿ_Close(h_ಕಂಡುಹಿಡಿ);
    }

    // Use the plain old stat() whenever it's possible.
    ಆದರೆ (!is_symlink)
	ಹಿಂತಿರುಗಿ _wstat(name, stp);

    ಆದರೆ (!resolve && is_symlink)
	flag = FILE_FLAG_OPEN_REPARSE_ಬಿಂದು;
    ಆದರೆ (attr & FILE_ATTRIBUTE_DIRECTORY)
	flag |= FILE_FLAG_BACKUP_SEMANTICS;

    h = CreateFileW(name, FILE_READ_ಗುಣಲಕ್ಷಣಗಳು,
	    FILE_SHARE_READ | FILE_SHARE_WRITE, ಖಾಲಿ, OPEN_EXISTING, flag,
	    ಖಾಲಿ);
    ಆದರೆ (h == INVALID_HANDLE_ಮೌಲ್ಯ)
	ಹಿಂತಿರುಗಿ -1;

    fd = _open_osfhandle((intptr_t)h, _O_RDONLY);
    n = _fstat(fd, (ರಚನೆ _stat *)stp);
    ಆದರೆ ((n == 0) && (attr & FILE_ATTRIBUTE_DIRECTORY))
	stp->st_ಬಹುಲಕ = (stp->st_ಬಹುಲಕ & ~S_IFMT) | S_IFDIR;
    _close(fd);

    ಹಿಂತಿರುಗಿ n;
}

/*
 * stat() can't handle a trailing '/' or '\', remove it first.
 * When 'resolve' is true behave as lstat() wrt symlinks.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
stat_impl(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, stat_T *stp, ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ resolve)
{
    // WinNT and later can use _MAX_PATH wide characters for a pathname, which
    // means that the maximum pathname is _MAX_PATH * 3 bytes when 'enc' is
    // UTF-8.
    char_u	buf[MAX_ಮಾರ್ಗ_2 * 3 + 1];
    size_t	buflen;
    char_u	*p;
    WCHAR	*wp;
    ಪೂರ್ಣಾಂಕ		n;

    vim_strncpy((char_u *)buf, (char_u *)name, ಗಾತ್ರ(buf) - 1);
    buflen = STRLEN(buf);
    p = buf + buflen;
    ಆದರೆ (p > buf)
	MB_PTR_ಹಿಂಭಾಗ(buf, p);

    // Remove trailing '\\' except root path.
    ಆದರೆ (p > buf && (*p == '\\' || *p == '/') && p[-1] != ':')
    {
	*p = NUL;
	buflen = (size_t)(p - buf);
    }

    ಆದರೆ ((buf[0] == '\\' && buf[1] == '\\') || (buf[0] == '/' && buf[1] == '/'))
    {
	// UNC root path must be followed by '\\'.
	p = vim_strpbrk(buf + 2, (char_u *)"\\/");
	ಆದರೆ (p != ಖಾಲಿ)
	{
	    p = vim_strpbrk(p + 1, (char_u *)"\\/");
	    ಆದರೆ (p == ಖಾಲಿ)
		STRCPY(buf + buflen, "\\");
	}
    }

    wp = enc_ಗಮ್ಯ_utf16(buf, ಖಾಲಿ);
    ಆದರೆ (wp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    n = mswin_stat_impl(wp, stp, resolve);
    vim_free(wp);
    ಹಿಂತಿರುಗಿ n;
}

    ಪೂರ್ಣಾಂಕ
vim_lstat(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, stat_T *stp)
{
    ಹಿಂತಿರುಗಿ stat_impl(name, stp, FALSE);
}

    ಪೂರ್ಣಾಂಕ
vim_stat(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, stat_T *stp)
{
    ಹಿಂತಿರುಗಿ stat_impl(name, stp, TRUE);
}

#ಆದರೆ defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
    ಶೂನ್ಯ
mch_settmode(tmode_T tmode ಬಳಸದ)
{
    // nothing to do
}

    ಪೂರ್ಣಾಂಕ
mch_get_shellsize(ಶೂನ್ಯ)
{
    // never used
    ಹಿಂತಿರುಗಿ OK;
}

    ಶೂನ್ಯ
mch_ಗಣ_shellsize(ಶೂನ್ಯ)
{
    // never used
}

/*
 * Rows and/or Columns has changed.
 */
    ಶೂನ್ಯ
mch_new_shellsize(ಶೂನ್ಯ)
{
    // never used
}

    ಶೂನ್ಯ
mch_calc_cell_ಗಾತ್ರ(ರಚನೆ cellsize *cs_ಹೊರಹೋಗು ಬಳಸದ)
{
    // never used
}

#ಆದರೆ_ಅಂತ್ಯ

/*
 * We have no job control, so fake it by starting a new shell.
 */
    ಶೂನ್ಯ
mch_suspend(ಶೂನ್ಯ)
{
    suspend_shell();
}

#ಆದರೆ defined(USE_MCH_ERRMSG)

# ifdef display_errors
#  undef display_errors
# endif

/*
 * Display the saved error message(s).
 */
    ಶೂನ್ಯ
display_errors(ಶೂನ್ಯ)
{
# ifdef FEAT_GUI
    char_u *p;

#  ifdef VIMDLL
    ಆದರೆ (gui.ಒಳ_use || gui.starting)
#  endif
    {
	ಆದರೆ (error_ga.ga_ದತ್ತಾಂಶ != ಖಾಲಿ)
	{
	    // avoid putting up a message box with blanks only
	    ಗಾಗಿ (p = (char_u *)error_ga.ga_ದತ್ತಾಂಶ; *p; ++p)
		ಆದರೆ (!ಸುರಕ್ಷಿತ_isspace(*p))
		{
		    // Only use a dialog when not using --gui-dialog-file:
		    // write text to a file.
		    ಆದರೆ (!gui_dialog_log((char_u *)"Errors", p))
			(ಶೂನ್ಯ)gui_mch_dialog(
				     gui.starting ? VIM_INFO :
					     VIM_ERROR,
				     gui.starting ? (char_u *)_("Message") :
					     (char_u *)_("Error"),
					     p, (char_u *)_("&Ok"),
					1, ಖಾಲಿ, FALSE);
		    ನಿಲ್ಲಿಸು;
		}
	    ga_clear(&error_ga);
	}
	ಹಿಂತಿರುಗಿ;
    }
# endif
# ಆದರೆ !defined(FEAT_GUI) || defined(VIMDLL)
    FlushFileBuffers(GetStdHandle(STD_ERROR_HANDLE));
# endif
}
#ಆದರೆ_ಅಂತ್ಯ


/*
 * Return TRUE if "p" contain a wildcard that can be expanded by
 * dos_expandpath().
 */
    ಪೂರ್ಣಾಂಕ
mch_has_exp_wildcard(char_u *p)
{
    ಗಾಗಿ ( ; *p; MB_PTR_ADV(p))
    {
	ಆದರೆ (vim_strchr((char_u *)"?*[", *p) != ಖಾಲಿ
		|| (*p == '~' && p[1] != NUL))
	    ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Return TRUE if "p" contain a wildcard or a "~1" kind of thing (could be a
 * shortened file name).
 */
    ಪೂರ್ಣಾಂಕ
mch_has_wildcard(char_u *p)
{
    ಗಾಗಿ ( ; *p; MB_PTR_ADV(p))
    {
	ಆದರೆ (vim_strchr((char_u *)"?*$[`", *p) != ಖಾಲಿ
		|| (*p == '~' && p[1] != NUL))
	    ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}


/*
 * The normal _chdir() does not change the default drive.  This one does.
 * Returning 0 implies success; -1 implies failure.
 */
    ಪೂರ್ಣಾಂಕ
mch_chdir(ಅಕ್ಷರ *ಮಾರ್ಗ)
{
    WCHAR   *p;
    ಪೂರ್ಣಾಂಕ	    n;

    ಆದರೆ (ಮಾರ್ಗ[0] == NUL)		// just checking...
	ಹಿಂತಿರುಗಿ -1;

    ಆದರೆ (p_verbose >= 5)
    {
	verbose_enter();
	smsg("chdir(%s)", ಮಾರ್ಗ);
	verbose_leave();
    }
    ಆದರೆ (ಸುರಕ್ಷಿತ_isalpha(ಮಾರ್ಗ[0]) && ಮಾರ್ಗ[1] == ':')	// has a drive name
    {
	// If we can change to the drive, skip that part of the path.  If we
	// can't then the current directory may be invalid, try using chdir()
	// with the whole path.
	ಆದರೆ (_chdrive(TOLOWER_ASC(ಮಾರ್ಗ[0]) - 'a' + 1) == 0)
	    ಮಾರ್ಗ += 2;
    }

    ಆದರೆ (*ಮಾರ್ಗ == NUL)		// drive name only
	ಹಿಂತಿರುಗಿ 0;

    p = enc_ಗಮ್ಯ_utf16((char_u *)ಮಾರ್ಗ, ಖಾಲಿ);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    n = _wchdir(p);
    vim_free(p);
    ಹಿಂತಿರುಗಿ n;
}


#ಆದರೆ defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
/*
 * return non-zero if a character is available
 */
    ಪೂರ್ಣಾಂಕ
mch_char_avail(ಶೂನ್ಯ)
{
    // never used
    ಹಿಂತಿರುಗಿ TRUE;
}

# ಆದರೆ defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    ಪೂರ್ಣಾಂಕ
mch_check_messages(ಶೂನ್ಯ)
{
    // TODO: check for messages
    ಹಿಂತಿರುಗಿ TRUE;
}
# endif
#ಆದರೆ_ಅಂತ್ಯ


#ಆದರೆ defined(FEAT_LIBCALL)
/*
 * Call a DLL routine which takes either a string or int param
 * and returns an allocated string.
 * Return OK if it worked, FAIL if not.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ char_u *(*MYSTRPROCSTR)(char_u *);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*MYSTRPROCINT)(char_u *);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ char_u *(*MYINTPROCSTR)(ಪೂರ್ಣಾಂಕ);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*MYINTPROCINT)(ಪೂರ್ಣಾಂಕ);

/*
 * Check if a pointer points to a valid NUL terminated string.
 * Return the length of the string, including terminating NUL.
 * Returns 0 for an invalid pointer, 1 for an empty string.
 */
    ಸ್ಥಿರ size_t
check_str_len(char_u *str)
{
    SYSTEM_INFO			si;
    ಸ್ಮರಣೆ_BASIC_INFORMATION	mbi;
    size_t			ಉದ್ದಳತೆ = 0;
    size_t			i;
    ಸ್ಥಿರಾಂಕ char_u		*p;

    // get page size
    GetSystemInfo(&si);

    // get memory information
    ಆದರೆ (!VirtualQuery(str, &mbi, ಗಾತ್ರ(mbi)))
	ಹಿಂತಿರುಗಿ 0;

    // pre cast these (typing savers)
    long_u dwStr = (long_u)str;
    long_u dw_ತಳ_ವಿಳಾಸ = (long_u)mbi.ತಳ_ವಿಳಾಸ;

    // get start address of page that str is on
    long_u str_ಪುಟ = dwStr - (dwStr - dw_ತಳ_ವಿಳಾಸ) % si.dw_ಪುಟ_ಗಾತ್ರ;

    // get length from str to end of page
    long_u ಪುಟ_ಉದ್ದಳತೆ = si.dw_ಪುಟ_ಗಾತ್ರ - (dwStr - str_ಪುಟ);

    ಗಾಗಿ (p = str; !IsBadReadPtr(p, (UINT)ಪುಟ_ಉದ್ದಳತೆ);
	    p += ಪುಟ_ಉದ್ದಳತೆ, ಪುಟ_ಉದ್ದಳತೆ = si.dw_ಪುಟ_ಗಾತ್ರ)
	ಗಾಗಿ (i = 0; i < ಪುಟ_ಉದ್ದಳತೆ; ++i, ++ಉದ್ದಳತೆ)
	    ಆದರೆ (p[i] == NUL)
		ಹಿಂತಿರುಗಿ ಉದ್ದಳತೆ + 1;

    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Passed to do_in_runtimepath() to load a vim.ico file.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
mch_icon_ತುಂಬು_cb(char_u *fname, ಶೂನ್ಯ *cookie)
{
    HANDLE *h = (HANDLE *)cookie;

    *h = ತುಂಬು_Image(ಖಾಲಿ,
		   (LPSTR)fname,
		   IMAGE_ICON,
		   64,
		   64,
		   LR_LOADFROMFILE | LR_LOADMAP3DCOLORS);
}

/*
 * Try loading an icon file from 'runtimepath'.
 */
    ಪೂರ್ಣಾಂಕ
mch_icon_ತುಂಬು(HANDLE *iconp)
{
    ಹಿಂತಿರುಗಿ do_ಒಳ_runtimepath((char_u *)"bitmaps/vim.ico",
						  0, mch_icon_ತುಂಬು_cb, iconp);
}

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if the OS PRNG is not available or something went wrong.
 */
    ಪೂರ್ಣಾಂಕ
mch_get_random(char_u *buf, ಪೂರ್ಣಾಂಕ len)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		initialized = NOTDONE;
    ಸ್ಥಿರ HINSTANCE	hInstLib;
    ಸ್ಥಿರ BOOL (WINAPI *pProcessPrng)(PUCHAR, ULONG);

    ಆದರೆ (initialized == NOTDONE)
    {
	hInstLib = vim_ತುಂಬು_Lib("bcryptprimitives.dll");
	ಆದರೆ (hInstLib != ಖಾಲಿ)
	    pProcessPrng = (ಶೂನ್ಯ *)Get_Proc_ವಿಳಾಸ(hInstLib, "ProcessPrng");
	ಆದರೆ (hInstLib == ಖಾಲಿ || pProcessPrng == ಖಾಲಿ)
	{
	    FreeLibrary(hInstLib);
	    initialized = FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    initialized = OK;
    }

    ಆದರೆ (initialized == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    // According to the documentation this call cannot fail.
    pProcessPrng(buf, len);

    ಹಿಂತಿರುಗಿ OK;
}

    ಪೂರ್ಣಾಂಕ
mch_libcall(
    char_u	*libname,
    char_u	*funcname,
    char_u	*argstring,	// NULL when using a argint
    ಪೂರ್ಣಾಂಕ		argint,
    char_u	**string_ಫಲಿತಾಂಶ,// NULL when using number_result
    ಪೂರ್ಣಾಂಕ		*ಸಂಖ್ಯೆ_ಫಲಿತಾಂಶ)
{
    HINSTANCE		hinstLib;

    BOOL f_ಸತತ_ಸಮಯ_Link_Success = FALSE;

    // Get a handle to the DLL module.
    hinstLib = vim_ತುಂಬು_Lib((ಅಕ್ಷರ *)libname);

    // If the handle is valid, try to get the function address.
    ಆದರೆ (hinstLib != ಖಾಲಿ)
    {
	char_u	*retval_str = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ	retval_int = 0;
	size_t	len;

# ifdef HAVE_TRY_EXCEPT
	__try
	{
# endif
	ಆದರೆ (argstring != ಖಾಲಿ)
	{
	    // Call with string argument
	    ಆದರೆ (string_ಫಲಿತಾಂಶ != ಖಾಲಿ)
	    {
		MYSTRPROCSTR proc;

		proc = (MYSTRPROCSTR)Get_Proc_ವಿಳಾಸ(hinstLib, (LPCSTR)funcname);
		ಆದರೆ (proc != ಖಾಲಿ)
		{
		    f_ಸತತ_ಸಮಯ_Link_Success = TRUE;
		    retval_str = proc(argstring);
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		MYSTRPROCINT proc;

		proc = (MYSTRPROCINT)Get_Proc_ವಿಳಾಸ(hinstLib, (LPCSTR)funcname);
		ಆದರೆ (proc != ಖಾಲಿ)
		{
		    f_ಸತತ_ಸಮಯ_Link_Success = TRUE;
		    retval_int = proc(argstring);
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Call with number argument
	    ಆದರೆ (string_ಫಲಿತಾಂಶ != ಖಾಲಿ)
	    {
		MYINTPROCSTR proc;

		proc = (MYINTPROCSTR)Get_Proc_ವಿಳಾಸ(hinstLib, (LPCSTR)funcname);
		ಆದರೆ (proc != ಖಾಲಿ)
		{
		    f_ಸತತ_ಸಮಯ_Link_Success = TRUE;
		    retval_str = proc(argint);
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		MYINTPROCINT proc;

		proc = (MYINTPROCINT)Get_Proc_ವಿಳಾಸ(hinstLib, (LPCSTR)funcname);
		ಆದರೆ (proc != ಖಾಲಿ)
		{
		    f_ಸತತ_ಸಮಯ_Link_Success = TRUE;
		    retval_int = proc(argint);
		}
	    }
	}

	// Save the string before we free the library.
	// Assume that a "1" result is an illegal pointer.
	ಆದರೆ (string_ಫಲಿತಾಂಶ == ಖಾಲಿ)
	    *ಸಂಖ್ಯೆ_ಫಲಿತಾಂಶ = retval_int;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (retval_str != ಖಾಲಿ
		&& (len = check_str_len(retval_str)) > 0)
	{
	    *string_ಫಲಿತಾಂಶ = alloc(len);
	    ಆದರೆ (*string_ಫಲಿತಾಂಶ != ಖಾಲಿ)
		mch_memmove(*string_ಫಲಿತಾಂಶ, retval_str, len);
	}

# ifdef HAVE_TRY_EXCEPT
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	    ಆದರೆ (Get_Exception_ಸಂಕೇತ() == EXCEPTION_ರಾಶಿ_ಅತಿಪ್ರವಾಹ)
		_resetstkoflw();
	    f_ಸತತ_ಸಮಯ_Link_Success = FALSE;
	}
# endif

	// Free the DLL module.
	(ಶೂನ್ಯ)FreeLibrary(hinstLib);
    }

    ಆದರೆ (!f_ಸತತ_ಸಮಯ_Link_Success)
    {
	semsg(_(e_library_call_failed_for_str), funcname);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Debugging helper: expose the MCH_WRITE_DUMP stuff to other modules
 */
    ಶೂನ್ಯ
DumpPutS(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *psz ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MCH_WRITE_DUMP
    ಆದರೆ (fdDump)
    {
	fputs(psz, fdDump);
	ಆದರೆ (psz[ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(psz) - 1] != '\n')
	    fputc('\n', fdDump);
	fflush(fdDump);
    }
#ಆದರೆ_ಅಂತ್ಯ
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _DEBUG

ಶೂನ್ಯ __cdecl
Trace(
    ಅಕ್ಷರ *pszFormat,
    ...)
{
    CHAR szBuff[2048];
    va_list ವಾದಗಳು;

    va_start(ವಾದಗಳು, pszFormat);
    vsprintf(szBuff, pszFormat, ವಾದಗಳು);
    va_end(ವಾದಗಳು);

    OutputDebugString(szBuff);
}

#ಆದರೆ_ಅಂತ್ಯ //_DEBUG

#ಆದರೆ !defined(FEAT_GUI) || defined(VIMDLL)
ಬಾಹ್ಯ HWND g_hWnd;	// This is in os_win32.c.

/*
 * Showing the printer dialog is tricky since we have no GUI
 * window to parent it. The following routines are needed to
 * get the window parenting and Z-order to work properly.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
GetConsoleHwnd(ಶೂನ್ಯ)
{
    // Skip if it's already set.
    ಆದರೆ (s_hwnd != 0)
	ಹಿಂತಿರುಗಿ;

    // Window handle may have been found by init code (Windows NT only)
    ಆದರೆ (g_hWnd != 0)
    {
	s_hwnd = g_hWnd;
	ಹಿಂತಿರುಗಿ;
    }

    s_hwnd = Get_Console_ಕಿಟಕಿ();
}

/*
 * Console implementation of ":winpos".
 */
    ಪೂರ್ಣಾಂಕ
mch_get_winpos(ಪೂರ್ಣಾಂಕ *x, ಪೂರ್ಣಾಂಕ *y)
{
    RECT  rect;

    GetConsoleHwnd();
    Get_ಕಿಟಕಿ_Rect(s_hwnd, &rect);
    *x = rect.ಎಡ_2;
    *y = rect.ತುದಿ;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Console implementation of ":winpos x y".
 */
    ಶೂನ್ಯ
mch_ಗಣ_winpos(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y)
{
    GetConsoleHwnd();
    ಗಣ_ಕಿಟಕಿ_Pos(s_hwnd, ಖಾಲಿ, x, y, 0, 0,
		 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT)

//=================================================================
// Win32 printer stuff

ಸ್ಥಿರ HFONT		prt_font_handles[2][2][2];
ಸ್ಥಿರ PRINTDLGW	prt_dlg;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ	boldface[2] = {FW_REGULAR, FW_BOLD};
ಸ್ಥಿರ TEXTMETRIC	prt_tm;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		prt_ಸಾಲು_ಎತ್ತರ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		prt_ಸಂಖ್ಯೆ_ಅಗಲ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		prt_ಎಡ_margin;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		prt_ಬಲ_margin;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		prt_ತುದಿ_margin;
ಸ್ಥಿರ char_u		szAppName[] = ಪಠ್ಯ_3("VIM");
ಸ್ಥಿರ HWND		hDlgPrint;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		*bUserAbort = ಖಾಲಿ;
ಸ್ಥಿರ char_u		*prt_name = ಖಾಲಿ;

// Defines which are also in vim.rc.
# define IDC_BOX1		400
# define IDC_PRINTTEXT1		401
# define IDC_PRINTTEXT2		402
# define IDC_ಪ್ರಗತಿ		403

    ಸ್ಥಿರ BOOL
vim_ಗಣ_Dlg_ಅಂಶ_ಪಠ್ಯ(HWND hDlg, ಪೂರ್ಣಾಂಕ n_IDDlg_ಅಂಶ, char_u *s)
{
    WCHAR   *wp;
    BOOL    ret;

    wp = enc_ಗಮ್ಯ_utf16(s, ಖಾಲಿ);
    ಆದರೆ (wp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    ret = ಗಣ_Dlg_ಅಂಶ_ಪಠ್ಯ_W(hDlg, n_IDDlg_ಅಂಶ, wp);
    vim_free(wp);
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Convert BGR to RGB for Windows GDI calls
 */
    ಸ್ಥಿರ COLORREF
ಅದಲುಬದಲು_me(COLORREF colorref)
{
    ಪೂರ್ಣಾಂಕ  ತಾತ್ಕಾಲಿಕ;
    ಅಕ್ಷರ *ptr = (ಅಕ್ಷರ *)&colorref;

    ತಾತ್ಕಾಲಿಕ = *(ptr);
    *(ptr ) = *(ptr + 2);
    *(ptr + 2) = ತಾತ್ಕಾಲಿಕ;
    ಹಿಂತಿರುಗಿ colorref;
}

    ಸ್ಥಿರ INT_PTR CALLBACK
PrintDlgProc(
	HWND hDlg,
	UINT ಸಂದೇಶ_2,
	WPARAM wParam ಬಳಸದ,
	LPARAM lParam ಬಳಸದ)
{
# ifdef FEAT_GETTEXT
    NONCLIENTMETRICS nm;
    ಸ್ಥಿರ HFONT hfont;
# endif

    ಆಯ್ಕೆ (ಸಂದೇಶ_2)
    {
	ಸ್ಥಿತಿ WM_INITDIALOG:
# ifdef FEAT_GETTEXT
	    nm.cb_ಗಾತ್ರ = ಗಾತ್ರ(NONCLIENTMETRICS);
	    ಆದರೆ (SystemParametersInfo(
			SPI_GETNONCLIENTMETRICS,
			ಗಾತ್ರ(NONCLIENTMETRICS),
			&nm,
			0))
	    {
		ಅಕ್ಷರ buff[MAX_ಮಾರ್ಗ];
		ಪೂರ್ಣಾಂಕ i;

		// Translate the dialog texts
		hfont = Create_Font_ಪರೋಕ್ಷ(&nm.lf_ಸಂದೇಶ_Font);
		ಗಾಗಿ (i = IDC_PRINTTEXT1; i <= IDC_ಪ್ರಗತಿ; i++)
		{
		    Send_Dlg_ಅಂಶ_ಸಂದೇಶ(hDlg, i, WM_SETFONT, (WPARAM)hfont, 1);
		    ಆದರೆ (Get_Dlg_ಅಂಶ_ಪಠ್ಯ(hDlg,i, buff, ಗಾತ್ರ(buff)))
			vim_ಗಣ_Dlg_ಅಂಶ_ಪಠ್ಯ(hDlg,i, (char_u *)_(buff));
		}
		Send_Dlg_ಅಂಶ_ಸಂದೇಶ(hDlg, IDCANCEL,
						WM_SETFONT, (WPARAM)hfont, 1);
		ಆದರೆ (Get_Dlg_ಅಂಶ_ಪಠ್ಯ(hDlg,IDCANCEL, buff, ಗಾತ್ರ(buff)))
		    vim_ಗಣ_Dlg_ಅಂಶ_ಪಠ್ಯ(hDlg,IDCANCEL, (char_u *)_(buff));
	    }
# endif
	    ಗಣ_ಕಿಟಕಿ_ಪಠ್ಯ(hDlg, (LPCSTR)szAppName);
	    ಆದರೆ (prt_name != ಖಾಲಿ)
	    {
		vim_ಗಣ_Dlg_ಅಂಶ_ಪಠ್ಯ(hDlg, IDC_PRINTTEXT2, (char_u *)prt_name);
		VIM_CLEAR(prt_name);
	    }
	    Enable_Menu_ಅಂಶ(GetSystemMenu(hDlg, FALSE), SC_CLOSE, MF_GRAYED);
# ಆದರೆ !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	    ಆದರೆ (!gui.ಒಳ_use)
#  endif
		Bring_ಕಿಟಕಿ_ಗಮ್ಯ_ತುದಿ(s_hwnd);
# endif
	    ಹಿಂತಿರುಗಿ TRUE;

	ಸ್ಥಿತಿ WM_COMMAND:
	    *bUserAbort = TRUE;
	    Enable_ಕಿಟಕಿ(Get_ಪೋಷಕ(hDlg), TRUE);
	    Destroy_ಕಿಟಕಿ(hDlg);
	    hDlgPrint = ಖಾಲಿ;
# ifdef FEAT_GETTEXT
	    DeleteObject(hfont);
# endif
	    ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

    ಸ್ಥಿರ BOOL CALLBACK
AbortProc(HDC hdcPrn ಬಳಸದ, ಪೂರ್ಣಾಂಕ i_ಸಂಕೇತ ಬಳಸದ)
{
    ಸಂದೇಶ_3 ಸಂದೇಶ;

    ಆಗ (!*bUserAbort && Peek_ಸಂದೇಶ_W(&ಸಂದೇಶ, ಖಾಲಿ, 0, 0, PM_REMOVE))
    {
	ಆದರೆ (!hDlgPrint || !Is_Dialog_ಸಂದೇಶ_W(hDlgPrint, &ಸಂದೇಶ))
	{
	    Translate_ಸಂದೇಶ(&ಸಂದೇಶ);
	    Dispatch_ಸಂದೇಶ_W(&ಸಂದೇಶ);
	}
    }
    ಹಿಂತಿರುಗಿ !*bUserAbort;
}

    ಸ್ಥಿರ UINT_PTR CALLBACK
PrintHookProc(
	HWND hDlg,	// handle to dialog box
	UINT ui_ಸಂದೇಶ,	// message identifier
	WPARAM wParam ಬಳಸದ,	// message parameter
	LPARAM lParam	// message parameter
	)
{
    HWND	hwndOwner;
    RECT	rc, rcDlg, rcOwner;
    PRINTDLGW	*pPD;

    ಆದರೆ (ui_ಸಂದೇಶ != WM_INITDIALOG)
	ಹಿಂತಿರುಗಿ FALSE;

    // Get the owner window and dialog box rectangles.
    ಆದರೆ ((hwndOwner = Get_ಪೋಷಕ(hDlg)) == ಖಾಲಿ)
	hwndOwner = Get_Desktop_ಕಿಟಕಿ();

    Get_ಕಿಟಕಿ_Rect(hwndOwner, &rcOwner);
    Get_ಕಿಟಕಿ_Rect(hDlg, &rcDlg);
    CopyRect(&rc, &rcOwner);

    // Offset the owner and dialog box rectangles so that
    // right and bottom values represent the width and
    // height, and then offset the owner again to discard
    // space taken up by the dialog box.

    ಸ್ಥಾನಾಂತರ_Rect(&rcDlg, -rcDlg.ಎಡ_2, -rcDlg.ತುದಿ);
    ಸ್ಥಾನಾಂತರ_Rect(&rc, -rc.ಎಡ_2, -rc.ತುದಿ);
    ಸ್ಥಾನಾಂತರ_Rect(&rc, -rcDlg.ಬಲ_2, -rcDlg.bottom);

    // The new position is the sum of half the remaining
    // space and the owner's original position.

    ಗಣ_ಕಿಟಕಿ_Pos(hDlg,
	    HWND_ತುದಿ,
	    rcOwner.ಎಡ_2 + (rc.ಬಲ_2 / 2),
	    rcOwner.ತುದಿ + (rc.bottom / 2),
	    0, 0,		// ignores size arguments
	    SWP_NOSIZE);

    //  tackle the printdlg copiesctrl problem
    pPD = (PRINTDLGW *)lParam;
    pPD->nCopies = (ಶಬ್ದ_2)pPD->l_Cust_ದತ್ತಾಂಶ;
    ಗಣ_Dlg_ಅಂಶ_Int( hDlg, edt3, pPD->nCopies, FALSE );
    //  Bring the window to top
    Bring_ಕಿಟಕಿ_ಗಮ್ಯ_ತುದಿ(Get_ಪೋಷಕ(hDlg));
    ಗಣ_Foreground_ಕಿಟಕಿ(hDlg);

    ಹಿಂತಿರುಗಿ FALSE;
}

    ಶೂನ್ಯ
mch_print_cleanup(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ pifItalic;
    ಪೂರ್ಣಾಂಕ pifBold;
    ಪೂರ್ಣಾಂಕ pifUnderline;

    ಗಾಗಿ (pifBold = 0; pifBold <= 1; pifBold++)
	ಗಾಗಿ (pifItalic = 0; pifItalic <= 1; pifItalic++)
	    ಗಾಗಿ (pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
		DeleteObject(prt_font_handles[pifBold][pifItalic][pifUnderline]);

    ಆದರೆ (prt_dlg.hDC != ಖಾಲಿ)
	DeleteDC(prt_dlg.hDC);
    ಆದರೆ (!*bUserAbort && hDlgPrint != ಖಾಲಿ)
	Send_ಸಂದೇಶ(hDlgPrint, WM_COMMAND, 0, 0);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಗಮ್ಯ_device_units(ಪೂರ್ಣಾಂಕ idx, ಪೂರ್ಣಾಂಕ dpi, ಪೂರ್ಣಾಂಕ physsize, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ def_ಸಂಖ್ಯೆ)
{
    ಪೂರ್ಣಾಂಕ		ret = 0;
    ಪೂರ್ಣಾಂಕ		u;
    ಪೂರ್ಣಾಂಕ		nr;

    u = prt_get_unit(idx);
    ಆದರೆ (u == PRT_UNIT_NONE)
    {
	u = PRT_UNIT_PERC;
	nr = def_ಸಂಖ್ಯೆ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	nr = printer_opts[idx].ಸಂಖ್ಯೆ;

    ಆಯ್ಕೆ (u)
    {
	ಸ್ಥಿತಿ PRT_UNIT_PERC:
	    ret = (physsize * nr) / 100;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PRT_UNIT_INCH:
	    ret = (nr * dpi);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PRT_UNIT_MM:
	    ret = (nr * 10 * dpi) / 254;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ PRT_UNIT_ಬಿಂದು:
	    ret = (nr * 10 * dpi) / 720;
	    ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (ret < ಸ್ಥಾನಾಂತರ)
	ಹಿಂತಿರುಗಿ 0;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ ret - ಸ್ಥಾನಾಂತರ;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
prt_get_cpl(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		hr;
    ಪೂರ್ಣಾಂಕ		phyw;
    ಪೂರ್ಣಾಂಕ		dvoff;
    ಪೂರ್ಣಾಂಕ		rev_ಸ್ಥಾನಾಂತರ;
    ಪೂರ್ಣಾಂಕ		dpi;

    Get_ಪಠ್ಯ_Metrics(prt_dlg.hDC, &prt_tm);
    prt_ಸಾಲು_ಎತ್ತರ = prt_tm.tm_ಎತ್ತರ + prt_tm.tmExternalLeading;

    hr	    = GetDeviceCaps(prt_dlg.hDC, HORZRES);
    phyw    = GetDeviceCaps(prt_dlg.hDC, PHYSICALWIDTH);
    dvoff   = GetDeviceCaps(prt_dlg.hDC, PHYSICALOFFSETX);
    dpi	    = GetDeviceCaps(prt_dlg.hDC, LOGPIXELSX);

    rev_ಸ್ಥಾನಾಂತರ = phyw - (dvoff + hr);

    prt_ಎಡ_margin = ಗಮ್ಯ_device_units(OPT_PRINT_ಎಡ, dpi, phyw, dvoff, 10);
    ಆದರೆ (prt_use_ಸಂಖ್ಯೆ())
    {
	prt_ಸಂಖ್ಯೆ_ಅಗಲ = PRINT_ಸಂಖ್ಯೆ_ಅಗಲ * prt_tm.tm_Ave_Char_ಅಗಲ;
	prt_ಎಡ_margin += prt_ಸಂಖ್ಯೆ_ಅಗಲ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	prt_ಸಂಖ್ಯೆ_ಅಗಲ = 0;

    prt_ಬಲ_margin = hr - ಗಮ್ಯ_device_units(OPT_PRINT_ಬಲ, dpi, phyw,
							       rev_ಸ್ಥಾನಾಂತರ, 5);

    ಹಿಂತಿರುಗಿ (prt_ಬಲ_margin - prt_ಎಡ_margin) / prt_tm.tm_Ave_Char_ಅಗಲ;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
prt_get_lpp(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ vr;
    ಪೂರ್ಣಾಂಕ phyw;
    ಪೂರ್ಣಾಂಕ dvoff;
    ಪೂರ್ಣಾಂಕ rev_ಸ್ಥಾನಾಂತರ;
    ಪೂರ್ಣಾಂಕ	bottom_margin;
    ಪೂರ್ಣಾಂಕ	dpi;

    vr	    = GetDeviceCaps(prt_dlg.hDC, VERTRES);
    phyw    = GetDeviceCaps(prt_dlg.hDC, PHYSICALHEIGHT);
    dvoff   = GetDeviceCaps(prt_dlg.hDC, PHYSICALOFFSETY);
    dpi	    = GetDeviceCaps(prt_dlg.hDC, LOGPIXELSY);

    rev_ಸ್ಥಾನಾಂತರ = phyw - (dvoff + vr);

    prt_ತುದಿ_margin = ಗಮ್ಯ_device_units(OPT_PRINT_ತುದಿ, dpi, phyw, dvoff, 5);

    // adjust top margin if there is a header
    prt_ತುದಿ_margin += prt_ಸಾಲು_ಎತ್ತರ * prt_ಶೀರ್ಷಿಕೆ_ಎತ್ತರ();

    bottom_margin = vr - ಗಮ್ಯ_device_units(OPT_PRINT_BOT, dpi, phyw,
							       rev_ಸ್ಥಾನಾಂತರ, 5);

    ಹಿಂತಿರುಗಿ (bottom_margin - prt_ತುದಿ_margin) / prt_ಸಾಲು_ಎತ್ತರ;
}

    ಪೂರ್ಣಾಂಕ
mch_print_init(prt_settings_T *psettings, char_u *jobname, ಪೂರ್ಣಾಂಕ forceit)
{
    ಸ್ಥಿರ HGLOBAL	stored_dm = ಖಾಲಿ;
    ಸ್ಥಿರ HGLOBAL	stored_devn = ಖಾಲಿ;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		stored_nCopies = 1;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		stored_nFlags = 0;

    LOGFONTW		fLogFont;
    ಪೂರ್ಣಾಂಕ			pifItalic;
    ಪೂರ್ಣಾಂಕ			pifBold;
    ಪೂರ್ಣಾಂಕ			pifUnderline;

    DEVMODEW		*mem;
    DEVNAMES		*devname;
    ಪೂರ್ಣಾಂಕ			i;
    DWORD		err;

    bUserAbort = &(psettings->user_abort);
    CLEAR_ಕ್ಷೇತ್ರ(prt_dlg);
    prt_dlg.l_Struct_ಗಾತ್ರ = ಗಾತ್ರ(PRINTDLGW);
# ಆದರೆ !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
    ಆದರೆ (!gui.ಒಳ_use)
#  endif
	GetConsoleHwnd();	    // get value of s_hwnd
# endif
    prt_dlg.hwndOwner = s_hwnd;
    prt_dlg.Flags = PD_NOPAGENUMS | PD_NOSELECTION | PD_RETURNDC;
    ಆದರೆ (!forceit)
    {
	prt_dlg.h_Dev_ಬಹುಲಕ = stored_dm;
	prt_dlg.h_Dev_ಹೆಸರುಗಳು = stored_devn;
	prt_dlg.l_Cust_ದತ್ತಾಂಶ = stored_nCopies; // work around bug in print dialog
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
    ಆದರೆ (forceit
# ಆದರೆ !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	    || (!gui.ಒಳ_use && !ಪದ_console)
#  ಇಲ್ಲದಿದ್ದರೆ
	    || !ಪದ_console
#  endif
# endif
	    )
    {
	prt_dlg.Flags |= PD_RETURNDEFAULT;
	/*
	 * MSDN suggests setting the first parameter to WINSPOOL for
	 * NT, but NULL appears to work just as well.
	 */
	ಆದರೆ (*p_pdev != NUL)
	    prt_dlg.hDC = CreateDC(ಖಾಲಿ, (LPCSTR)p_pdev, ಖಾಲಿ, ಖಾಲಿ);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    prt_dlg.Flags |= PD_RETURNDEFAULT;
	    ಆದರೆ (PrintDlgW(&prt_dlg) == 0)
		ಹೋಗು init_fail_dlg;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (PrintDlgW(&prt_dlg) == 0)
	ಹೋಗು init_fail_dlg;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	/*
	 * keep the previous driver context
	 */
	stored_dm = prt_dlg.h_Dev_ಬಹುಲಕ;
	stored_devn = prt_dlg.h_Dev_ಹೆಸರುಗಳು;
	stored_nFlags = prt_dlg.Flags;
	stored_nCopies = prt_dlg.nCopies;
    }

    ಆದರೆ (prt_dlg.hDC == ಖಾಲಿ)
    {
	emsg(_(e_printer_selection_failed));
	mch_print_cleanup();
	ಹಿಂತಿರುಗಿ FALSE;
    }

    // Not all printer drivers report the support of color (or grey) in the
    // same way.  Let's set has_color if there appears to be some way to print
    // more than B&W.
    i = GetDeviceCaps(prt_dlg.hDC, NUMCOLORS);
    psettings->has_color = (GetDeviceCaps(prt_dlg.hDC, BITSPIXEL) > 1
				   || GetDeviceCaps(prt_dlg.hDC, PLANES) > 1
				   || i > 2 || i == -1);

    // Ensure all font styles are baseline aligned
    ಗಣ_ಪಠ್ಯ_Align(prt_dlg.hDC, TA_BASELINE|TA_ಎಡ);

    /*
     * On some windows systems the nCopies parameter is not
     * passed back correctly. It must be retrieved from the
     * hDevMode struct.
     */
    mem = (DEVMODEW *)Global_ಬೀಗ(prt_dlg.h_Dev_ಬಹುಲಕ);
    ಆದರೆ (mem != ಖಾಲಿ)
    {
	ಆದರೆ (mem->dmCopies != 1)
	    stored_nCopies = mem->dmCopies;
	ಆದರೆ ((mem->dmFields & DM_DUPLEX) && (mem->dmDuplex & ~DMDUP_SIMPLEX))
	    psettings->duplex = TRUE;
	ಆದರೆ ((mem->dmFields & DM_COLOR) && (mem->dmColor & DMCOLOR_COLOR))
	    psettings->has_color = TRUE;
    }
    GlobalUnlock(prt_dlg.h_Dev_ಬಹುಲಕ);

    devname = (DEVNAMES *)Global_ಬೀಗ(prt_dlg.h_Dev_ಹೆಸರುಗಳು);
    ಆದರೆ (devname != 0)
    {
	WCHAR	*wprinter_name = (WCHAR *)devname + devname->w_Device_ಸ್ಥಾನಾಂತರ;
	WCHAR	*wport_name = (WCHAR *)devname + devname->w_Output_ಸ್ಥಾನಾಂತರ;
	char_u	*ಪಠ್ಯ = (char_u *)_("to %s on %s");
	char_u  *printer_name = utf16_ಗಮ್ಯ_enc(wprinter_name, ಖಾಲಿ);
	char_u	*ಪೋರ್ಟ್_name = utf16_ಗಮ್ಯ_enc(wport_name, ಖಾಲಿ);

	ಆದರೆ (printer_name != ಖಾಲಿ && ಪೋರ್ಟ್_name != ಖಾಲಿ)
	    prt_name = alloc(STRLEN(printer_name)
					   + STRLEN(ಪೋರ್ಟ್_name) + STRLEN(ಪಠ್ಯ));
	ಆದರೆ (prt_name != ಖಾಲಿ)
	    wsprintf((ಅಕ್ಷರ *)prt_name, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಪಠ್ಯ,
		    printer_name, ಪೋರ್ಟ್_name);
	vim_free(printer_name);
	vim_free(ಪೋರ್ಟ್_name);
    }
    GlobalUnlock(prt_dlg.h_Dev_ಹೆಸರುಗಳು);

    /*
     * Initialise the font according to 'printfont'
     */
    CLEAR_ಕ್ಷೇತ್ರ(fLogFont);
    ಆದರೆ (get_logfont(&fLogFont, p_pfn, prt_dlg.hDC, TRUE) == FAIL)
    {
	semsg(_(e_unknown_printer_font_str), p_pfn);
	mch_print_cleanup();
	ಹಿಂತಿರುಗಿ FALSE;
    }

    ಗಾಗಿ (pifBold = 0; pifBold <= 1; pifBold++)
	ಗಾಗಿ (pifItalic = 0; pifItalic <= 1; pifItalic++)
	    ಗಾಗಿ (pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
	    {
		fLogFont.lf_ತೂಕ =  boldface[pifBold];
		fLogFont.lfItalic = pifItalic;
		fLogFont.lfUnderline = pifUnderline;
		prt_font_handles[pifBold][pifItalic][pifUnderline]
					      = Create_Font_ಪರೋಕ್ಷ_W(&fLogFont);
	    }

    ಗಣ_Bk_ಬಹುಲಕ(prt_dlg.hDC, OPAQUE);
    SelectObject(prt_dlg.hDC, prt_font_handles[0][0][0]);

    /*
     * Fill in the settings struct
     */
    psettings->chars_ಪ್ರತಿ_ಸಾಲು = prt_get_cpl();
    psettings->ಸಾಲುಗಳು_ಪ್ರತಿ_ಪುಟ = prt_get_lpp();
    ಆದರೆ (prt_dlg.Flags & PD_USEDEVMODECOPIESANDCOLLATE)
    {
	psettings->n_collated_copies = (prt_dlg.Flags & PD_COLLATE)
						    ? prt_dlg.nCopies : 1;
	psettings->n_uncollated_copies = (prt_dlg.Flags & PD_COLLATE)
						    ? 1 : prt_dlg.nCopies;

	ಆದರೆ (psettings->n_collated_copies == 0)
	    psettings->n_collated_copies = 1;

	ಆದರೆ (psettings->n_uncollated_copies == 0)
	    psettings->n_uncollated_copies = 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	psettings->n_collated_copies = 1;
	psettings->n_uncollated_copies = 1;
    }

    psettings->jobname = jobname;

    ಹಿಂತಿರುಗಿ TRUE;

init_fail_dlg:
    err = CommDlgExtendedError();
    ಆದರೆ (err)
    {
	char_u *buf;

	// I suspect FormatMessage() doesn't work for values returned by
	// CommDlgExtendedError().  What does?
	Format_ಸಂದೇಶ(FORMAT_ಸಂದೇಶ_ALLOCATE_ಬಫರ್ |
		FORMAT_ಸಂದೇಶ_FROM_SYSTEM |
		FORMAT_ಸಂದೇಶ_IGNORE_INSERTS,
		ಖಾಲಿ, err, 0, (LPTSTR)(&buf), 0, ಖಾಲಿ);
	semsg(_(e_print_error_str),
		buf == ಖಾಲಿ ? (char_u *)_("Unknown") : buf);
	LocalFree((LPVOID)(buf));
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಸಂದೇಶ_clr_eos(); // Maybe canceled

    mch_print_cleanup();
    ಹಿಂತಿರುಗಿ FALSE;
}


    ಪೂರ್ಣಾಂಕ
mch_print_begin(prt_settings_T *psettings)
{
    ಪೂರ್ಣಾಂಕ			ret = 0;
    ಅಕ್ಷರ		sz_ಬಫರ್[300];
    WCHAR		*wp;

    hDlgPrint = CreateDialog(g_hinst, ಪಠ್ಯ_3("PrintDlgBox"),
					     prt_dlg.hwndOwner, PrintDlgProc);
    ಗಣ_Abort_Proc(prt_dlg.hDC, AbortProc);
    wsprintf(sz_ಬಫರ್, _("Printing '%s'"), gettail(psettings->jobname));
    vim_ಗಣ_Dlg_ಅಂಶ_ಪಠ್ಯ(hDlgPrint, IDC_PRINTTEXT1, (char_u *)sz_ಬಫರ್);

    wp = enc_ಗಮ್ಯ_utf16(psettings->jobname, ಖಾಲಿ);
    ಆದರೆ (wp != ಖಾಲಿ)
    {
	DOCINFOW	di;

	CLEAR_ಕ್ಷೇತ್ರ(di);
	di.cb_ಗಾತ್ರ = ಗಾತ್ರ(di);
	di.lpszDocName = wp;
	ret = ಆರಂಭ_Doc_W(prt_dlg.hDC, &di);
	vim_free(wp);
    }

# ifdef FEAT_GUI
    // Give focus back to main window (when using MDI).
#  ifdef VIMDLL
    ಆದರೆ (gui.ಒಳ_use)
#  endif
	ಗಣ_Focus(s_hwnd);
# endif

    ಹಿಂತಿರುಗಿ (ret > 0);
}

    ಶೂನ್ಯ
mch_print_ಕೊನೆ(prt_settings_T *psettings ಬಳಸದ)
{
    ಕೊನೆ_Doc(prt_dlg.hDC);
    ಆದರೆ (!*bUserAbort && hDlgPrint != ಖಾಲಿ)
	Send_ಸಂದೇಶ(hDlgPrint, WM_COMMAND, 0, 0);
}

    ಪೂರ್ಣಾಂಕ
mch_print_ಕೊನೆ_ಪುಟ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (ಕೊನೆ_ಪುಟ(prt_dlg.hDC) > 0);
}

    ಪೂರ್ಣಾಂಕ
mch_print_begin_ಪುಟ(char_u *ಸಂದೇಶ)
{
    ಆದರೆ (ಸಂದೇಶ != ಖಾಲಿ)
	vim_ಗಣ_Dlg_ಅಂಶ_ಪಠ್ಯ(hDlgPrint, IDC_ಪ್ರಗತಿ, ಸಂದೇಶ);
    ಹಿಂತಿರುಗಿ (ಆರಂಭ_ಪುಟ(prt_dlg.hDC) > 0);
}

    ಪೂರ್ಣಾಂಕ
mch_print_blank_ಪುಟ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (mch_print_begin_ಪುಟ(ಖಾಲಿ) ? (mch_print_ಕೊನೆ_ಪುಟ()) : FALSE);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prt_pos_x = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prt_pos_y = 0;

    ಶೂನ್ಯ
mch_print_ಆರಂಭ_ಸಾಲು(ಪೂರ್ಣಾಂಕ margin, ಪೂರ್ಣಾಂಕ ಪುಟ_ಸಾಲು)
{
    ಆದರೆ (margin)
	prt_pos_x = -prt_ಸಂಖ್ಯೆ_ಅಗಲ;
    ಇಲ್ಲದಿದ್ದರೆ
	prt_pos_x = 0;
    prt_pos_y = ಪುಟ_ಸಾಲು * prt_ಸಾಲು_ಎತ್ತರ
				 + prt_tm.tmAscent + prt_tm.tmExternalLeading;
}

    ಪೂರ್ಣಾಂಕ
mch_print_ಪಠ್ಯ_ಹೊರಹೋಗು(char_u *p, ಪೂರ್ಣಾಂಕ len)
{
    ಗಾತ್ರ_4	sz;
    WCHAR	*wp;
    ಪೂರ್ಣಾಂಕ		wlen = len;
    ಪೂರ್ಣಾಂಕ		ret = FALSE;

    wp = enc_ಗಮ್ಯ_utf16(p, &wlen);
    ಆದರೆ (wp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    ಪಠ್ಯ_ಹೊರಹೋಗು_W(prt_dlg.hDC, prt_pos_x + prt_ಎಡ_margin,
	    prt_pos_y + prt_ತುದಿ_margin, wp, wlen);
    Get_ಪಠ್ಯ_Extent_Point32_W(prt_dlg.hDC, wp, wlen, &sz);
    vim_free(wp);
    prt_pos_x += (sz.cx - prt_tm.tmOverhang);
    // This is wrong when printing spaces for a TAB.
    ಆದರೆ (p[len] != NUL)
    {
	wlen = mb_ptr2len(p + len);
	wp = enc_ಗಮ್ಯ_utf16(p + len, &wlen);
	ಆದರೆ (wp != ಖಾಲಿ)
	{
	    Get_ಪಠ್ಯ_Extent_Point32_W(prt_dlg.hDC, wp, 1, &sz);
	    ret = (prt_pos_x + prt_ಎಡ_margin + sz.cx > prt_ಬಲ_margin);
	    vim_free(wp);
	}
    }
    ಹಿಂತಿರುಗಿ ret;
}

    ಶೂನ್ಯ
mch_print_ಗಣ_font(ಪೂರ್ಣಾಂಕ iBold, ಪೂರ್ಣಾಂಕ iItalic, ಪೂರ್ಣಾಂಕ iUnderline)
{
    SelectObject(prt_dlg.hDC, prt_font_handles[iBold][iItalic][iUnderline]);
}

    ಶೂನ್ಯ
mch_print_ಗಣ_bg(long_u bgcol)
{
    ಗಣ_Bk_Color(prt_dlg.hDC, Get_ಹತ್ತಿರದ_Color(prt_dlg.hDC,
						   ಅದಲುಬದಲು_me((COLORREF)bgcol)));
    /*
     * With a white background we can draw characters transparent, which is
     * good for italic characters that overlap to the next char cell.
     */
    ಆದರೆ (bgcol == 0xffffffUL)
	ಗಣ_Bk_ಬಹುಲಕ(prt_dlg.hDC, TRANSPARENT);
    ಇಲ್ಲದಿದ್ದರೆ
	ಗಣ_Bk_ಬಹುಲಕ(prt_dlg.hDC, OPAQUE);
}

    ಶೂನ್ಯ
mch_print_ಗಣ_fg(long_u fgcol)
{
    ಗಣ_ಪಠ್ಯ_Color(prt_dlg.hDC, Get_ಹತ್ತಿರದ_Color(prt_dlg.hDC,
						   ಅದಲುಬದಲು_me((COLORREF)fgcol)));
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_PRINTER && !FEAT_POSTSCRIPT



#ಆದರೆ defined(FEAT_SHORTCUT)
# include <shlobj.h>

# define is_ಮಾರ್ಗ_sep(c)	    ((c) == L'\\' || (c) == L'/')

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
is_reparse_ಬಿಂದು_included(LPCWSTR fname)
{
    LPCWSTR	p = fname, q;
    WCHAR	buf[MAX_ಮಾರ್ಗ];
    DWORD	attr;

    ಆದರೆ (ಸುರಕ್ಷಿತ_isalpha(p[0]) && p[1] == L':' && is_ಮಾರ್ಗ_sep(p[2]))
	p += 3;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_ಮಾರ್ಗ_sep(p[0]) && is_ಮಾರ್ಗ_sep(p[1]))
	p += 2;

    ಆಗ (*p != L'\0')
    {
	q = wcspbrk(p, L"\\/");
	ಆದರೆ (q == ಖಾಲಿ)
	    p = q = fname + wcslen(fname);
	ಇಲ್ಲದಿದ್ದರೆ
	    p = q + 1;
	ಆದರೆ (q - fname >= MAX_ಮಾರ್ಗ)
	    ಹಿಂತಿರುಗಿ FALSE;
	wcsncpy(buf, fname, q - fname);
	buf[q - fname] = L'\0';
	attr = Get_File_ಗುಣಲಕ್ಷಣಗಳು_W(buf);
	ಆದರೆ (attr != INVALID_FILE_ಗುಣಲಕ್ಷಣಗಳು
		&& (attr & FILE_ATTRIBUTE_REPARSE_ಬಿಂದು) != 0)
	    ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Return the resolved file path, NULL if "fname" is an AppExecLink reparse
 * point, already fully resolved, or it doesn't exists.
 */
    char_u *
resolve_reparse_ಬಿಂದು(char_u *fname)
{
    HANDLE	    h = INVALID_HANDLE_ಮೌಲ್ಯ;
    DWORD	    ಗಾತ್ರ_2;
    WCHAR	    *p, *wp;
    char_u	    *rfname = ಖಾಲಿ;
    WCHAR	    *buff = ಖಾಲಿ;

    p = enc_ಗಮ್ಯ_utf16(fname, ಖಾಲಿ);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹೋಗು fail;

    ಆದರೆ (!is_reparse_ಬಿಂದು_included(p))
    {
	vim_free(p);
	ಹೋಗು fail;
    }

    h = CreateFileW(p, 0, 0, ಖಾಲಿ, OPEN_EXISTING,
	    FILE_FLAG_BACKUP_SEMANTICS, ಖಾಲಿ);
    vim_free(p);

    ಆದರೆ (h == INVALID_HANDLE_ಮೌಲ್ಯ)
	ಹೋಗು fail;

    ಗಾತ್ರ_2 = Get_Final_ಮಾರ್ಗ_Name_By_Handle_W(h, ಖಾಲಿ, 0, 0);
    ಆದರೆ (ಗಾತ್ರ_2 == 0)
	ಹೋಗು fail;
    buff = ALLOC_MULT(WCHAR, ಗಾತ್ರ_2);
    ಆದರೆ (buff == ಖಾಲಿ)
	ಹೋಗು fail;
    ಆದರೆ (Get_Final_ಮಾರ್ಗ_Name_By_Handle_W(h, buff, ಗಾತ್ರ_2, 0) == 0)
	ಹೋಗು fail;

    ಆದರೆ (wcsncmp(buff, L"\\\\?\\UNC\\", 8) == 0)
    {
	buff[6] = L'\\';
	wp = buff + 6;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wcsncmp(buff, L"\\\\?\\", 4) == 0)
	wp = buff + 4;
    ಇಲ್ಲದಿದ್ದರೆ
	wp = buff;

    rfname = utf16_ಗಮ್ಯ_enc(wp, ಖಾಲಿ);

fail:
    ಆದರೆ (h != INVALID_HANDLE_ಮೌಲ್ಯ)
	CloseHandle(h);
    ಆದರೆ (buff != ಖಾಲಿ)
	vim_free(buff);

    ಹಿಂತಿರುಗಿ rfname;
}

/*
 * When "fname" is the name of a shortcut (*.lnk) resolve the file it points
 * to and return that name in allocated memory.
 * Otherwise NULL is returned.
 */
    ಸ್ಥಿರ char_u *
resolve_shortcut(char_u *fname)
{
    HRESULT		hr;
    IShellLink		*psl = ಖಾಲಿ;
    IPersistFile	*ppf = ಖಾಲಿ;
    OLECHAR		wsz[MAX_ಮಾರ್ಗ];
    char_u		*rfname = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ			len;
    IShellLinkW		*pslw = ಖಾಲಿ;
    WIN32_ಕಂಡುಹಿಡಿ_DATAW	ffdw; // we get those free of charge

    // Check if the file name ends in ".lnk". Avoid calling
    // CoCreateInstance(), it's quite slow.
    ಆದರೆ (fname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ rfname;
    len = (ಪೂರ್ಣಾಂಕ)STRLEN(fname);
    ಆದರೆ (len <= 4 || STRNICMP(fname + len - 4, ".lnk", 4) != 0)
	ಹಿಂತಿರುಗಿ rfname;

    CoInitialize(ಖಾಲಿ);

    // create a link manager object and request its interface
    hr = CoCreateInstance(
	    &CLSID_ShellLink, ಖಾಲಿ, CLSCTX_INPROC_SERVER,
	    &IID_IShellLinkW, (ಶೂನ್ಯ**)&pslw);
    ಆದರೆ (hr == S_OK)
    {
	WCHAR	*p = enc_ಗಮ್ಯ_utf16(fname, ಖಾಲಿ);

	ಆದರೆ (p != ಖಾಲಿ)
	{
	    // Get a pointer to the IPersistFile interface.
	    hr = pslw->lpVtbl->QueryInterface(
		    pslw, &IID_IPersistFile, (ಶೂನ್ಯ**)&ppf);
	    ಆದರೆ (hr != S_OK)
		ಹೋಗು shortcut_errorw;

	    // "load" the name and resolve the link
	    hr = ppf->lpVtbl->ತುಂಬು_2(ppf, p, STGM_READ);
	    ಆದರೆ (hr != S_OK)
		ಹೋಗು shortcut_errorw;
# ಆದರೆ 0  // This makes Vim wait a long time if the target does not exist.
	    hr = pslw->lpVtbl->Resolve(pslw, ಖಾಲಿ, SLR_NO_UI);
	    ಆದರೆ (hr != S_OK)
		ಹೋಗು shortcut_errorw;
# endif

	    // Get the path to the link target.
	    Zero_ಸ್ಮರಣೆ(wsz, MAX_ಮಾರ್ಗ * ಗಾತ್ರ(WCHAR));
	    hr = pslw->lpVtbl->Get_ಮಾರ್ಗ(pslw, wsz, MAX_ಮಾರ್ಗ, &ffdw, 0);
	    ಆದರೆ (hr == S_OK && wsz[0] != NUL)
		rfname = utf16_ಗಮ್ಯ_enc(wsz, ಖಾಲಿ);

shortcut_errorw:
	    vim_free(p);
	}
    }

    // Release all interface pointers (both belong to the same object)
    ಆದರೆ (ppf != ಖಾಲಿ)
	ppf->lpVtbl->Release(ppf);
    ಆದರೆ (psl != ಖಾಲಿ)
	psl->lpVtbl->Release(psl);
    ಆದರೆ (pslw != ಖಾಲಿ)
	pslw->lpVtbl->Release(pslw);

    CoUninitialize();
    ಹಿಂತಿರುಗಿ rfname;
}

    char_u *
mch_resolve_ಮಾರ್ಗ(char_u *fname, ಪೂರ್ಣಾಂಕ reparse_ಬಿಂದು)
{
    char_u  *ಮಾರ್ಗ = resolve_shortcut(fname);

    ಆದರೆ (ಮಾರ್ಗ == ಖಾಲಿ && reparse_ಬಿಂದು)
	ಮಾರ್ಗ = resolve_reparse_ಬಿಂದು(fname);
    ಹಿಂತಿರುಗಿ ಮಾರ್ಗ;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL) && (!defined(FEAT_GUI) || defined(VIMDLL))
/*
 * Bring ourselves to the foreground.  Does work if the OS doesn't allow it.
 */
    ಶೂನ್ಯ
win32_ಗಣ_foreground(ಶೂನ್ಯ)
{
    GetConsoleHwnd();	    // get value of s_hwnd
    ಆದರೆ (s_hwnd != 0)
	ಗಣ_Foreground_ಕಿಟಕಿ(s_hwnd);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_CLIENTSERVER)
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
HWND ಸಂದೇಶ_ಕಿಟಕಿ = 0;	    // window that's handling messages

# define VIM_CLASSNAME      "VIM_MESSAGES"
# define VIM_CLASSNAME_LEN  (ಗಾತ್ರ(VIM_CLASSNAME) - 1)

// Timeout for sending a message to another Vim instance.  Normally this works
// instantly, but it may hang when the other Vim instance is halted.
# define SENDMESSAGE_TIMEOUT	(5 * 1000)

// Communication is via WM_COPYDATA messages. The message type is sent in
// the dwData parameter. Types are defined here.
# define COPYDATA_ಕೀಲಿಗಳು		0
# define COPYDATA_ಉತ್ತರ		1
# define COPYDATA_ಅಭಿವ್ಯಕ್ತಿ		10
# define COPYDATA_ಫಲಿತಾಂಶ	11
# define COPYDATA_ERROR_ಫಲಿತಾಂಶ	12
# define COPYDATA_ENCODING	20

// This is a structure containing a server HWND and its name.
ರಚನೆ server_id
{
    HWND hwnd;
    char_u *name;
};

// Last received 'encoding' that the client uses.
ಸ್ಥಿರ char_u	*client_enc = ಖಾಲಿ;

/*
 * Tell the other side what encoding we are using.
 * Return -1 if timeout happens.  Other errors are ignored.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
serverSendEnc(HWND ಗುರಿ)
{
    COPYDATASTRUCT ದತ್ತಾಂಶ;

    ದತ್ತಾಂಶ.dw_ದತ್ತಾಂಶ = COPYDATA_ENCODING;
    ದತ್ತಾಂಶ.cb_ದತ್ತಾಂಶ = (DWORD)STRLEN(p_enc) + 1;
    ದತ್ತಾಂಶ.lp_ದತ್ತಾಂಶ = p_enc;
    ಆದರೆ (Send_ಸಂದೇಶ_Timeout(ಗುರಿ, WM_COPYDATA,
	    (WPARAM)ಸಂದೇಶ_ಕಿಟಕಿ, (LPARAM)&ದತ್ತಾಂಶ,
	    SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, ಖಾಲಿ) == 0)
	ಹಿಂತಿರುಗಿ -1;
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Clean up on exit. This destroys the hidden message window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
Clean_ಮೇಲೆ_Messaging(ಶೂನ್ಯ)
{
    ಆದರೆ (ಸಂದೇಶ_ಕಿಟಕಿ == 0)
	ಹಿಂತಿರುಗಿ;

    Destroy_ಕಿಟಕಿ(ಸಂದೇಶ_ಕಿಟಕಿ);
    ಸಂದೇಶ_ಕಿಟಕಿ = 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ save_ಉತ್ತರ(HWND server, char_u *ಉತ್ತರ, ಪೂರ್ಣಾಂಕ ಅಭಿವ್ಯಕ್ತಿ);

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
    ಸ್ಥಿರ LRESULT CALLBACK
Messaging_WndProc(HWND hwnd, UINT ಸಂದೇಶ, WPARAM wParam, LPARAM lParam)
{
    ಆದರೆ (ಸಂದೇಶ == WM_COPYDATA)
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
	COPYDATASTRUCT	*ದತ್ತಾಂಶ = (COPYDATASTRUCT*)lParam;
	HWND		sender = (HWND)wParam;
	COPYDATASTRUCT	ಉತ್ತರ;
	char_u		*res;
	ಪೂರ್ಣಾಂಕ		retval;
	DWORD_PTR	dwret = 0;
	char_u		*str;
	char_u		*tofree;

	ಆಯ್ಕೆ (ದತ್ತಾಂಶ->dw_ದತ್ತಾಂಶ)
	{
	ಸ್ಥಿತಿ COPYDATA_ENCODING:
	    // Remember the encoding that the client uses.
	    vim_free(client_enc);
	    client_enc = enc_canonize((char_u *)ದತ್ತಾಂಶ->lp_ದತ್ತಾಂಶ);
	    ಹಿಂತಿರುಗಿ 1;

	ಸ್ಥಿತಿ COPYDATA_ಕೀಲಿಗಳು:
	    // Remember who sent this, for <client>
	    client_ಕಿಟಕಿ = sender;

	    // Add the received keys to the input buffer.
	    str = serverConvert(client_enc, (char_u *)ದತ್ತಾಂಶ->lp_ದತ್ತಾಂಶ, &tofree);
# ifdef FEAT_GUI
	    ಆದರೆ (gui.ಒಳ_use)
		// The GUI may get here re-entrantly, so insert the keys later.
		server_ಕೂಡಿಸು_ಒಳಹರಿವು(str);
	    ಇಲ್ಲದಿದ್ದರೆ
# endif
		server_ಗಮ್ಯ_ಒಳಹರಿವು_buf(str);
	    vim_free(tofree);

# ifdef FEAT_GUI
	    // Wake up the main GUI loop.
#  ifdef VIMDLL
	    ಆದರೆ (gui.ಒಳ_use)
#  endif
		ಆದರೆ (s_hwnd != 0)
		    Post_ಸಂದೇಶ(s_hwnd, WM_NULL, 0, 0);
# endif
	    ಹಿಂತಿರುಗಿ 1;

	ಸ್ಥಿತಿ COPYDATA_ಅಭಿವ್ಯಕ್ತಿ:
	    // Remember who sent this, for <client>
	    client_ಕಿಟಕಿ = sender;

	    str = serverConvert(client_enc, (char_u *)ದತ್ತಾಂಶ->lp_ದತ್ತಾಂಶ, &tofree);
	    res = eval_client_ಅಭಿವ್ಯಕ್ತಿ_ಗಮ್ಯ_string(str);

	    ಆದರೆ (res == ಖಾಲಿ)
	    {
		ಅಕ್ಷರ	*err = _(e_invalid_ಅಭಿವ್ಯಕ್ತಿ_received);
		size_t	len = STRLEN(str) + STRLEN(err) + 5;

		res = alloc(len);
		ಆದರೆ (res != ಖಾಲಿ)
		    ಉತ್ತರ.cb_ದತ್ತಾಂಶ = (DWORD)vim_snprintf_safelen(
			(ಅಕ್ಷರ *)res, len, "%s: \"%s\"", err, str);
		ಇಲ್ಲದಿದ್ದರೆ
		    ಉತ್ತರ.cb_ದತ್ತಾಂಶ = 0;
		ಉತ್ತರ.dw_ದತ್ತಾಂಶ = COPYDATA_ERROR_ಫಲಿತಾಂಶ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಉತ್ತರ.cb_ದತ್ತಾಂಶ = (DWORD)STRLEN(res) + 1;
		ಉತ್ತರ.dw_ದತ್ತಾಂಶ = COPYDATA_ಫಲಿತಾಂಶ;
	    }
	    ಉತ್ತರ.lp_ದತ್ತಾಂಶ = res;

	    ಆದರೆ (serverSendEnc(sender) < 0)
		retval = -1;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (Send_ಸಂದೇಶ_Timeout(sender, WM_COPYDATA,
			(WPARAM)ಸಂದೇಶ_ಕಿಟಕಿ, (LPARAM)&ಉತ್ತರ,
			SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
		    retval = -1;
		ಇಲ್ಲದಿದ್ದರೆ
		    retval = (ಪೂರ್ಣಾಂಕ)dwret;
	    }
	    vim_free(tofree);
	    vim_free(res);
	    ಹಿಂತಿರುಗಿ retval;

	ಸ್ಥಿತಿ COPYDATA_ಉತ್ತರ:
	ಸ್ಥಿತಿ COPYDATA_ಫಲಿತಾಂಶ:
	ಸ್ಥಿತಿ COPYDATA_ERROR_ಫಲಿತಾಂಶ:
	    ಆದರೆ (ದತ್ತಾಂಶ->lp_ದತ್ತಾಂಶ != ಖಾಲಿ)
	    {
		str = serverConvert(client_enc, (char_u *)ದತ್ತಾಂಶ->lp_ದತ್ತಾಂಶ,
								     &tofree);
		ಆದರೆ (tofree == ಖಾಲಿ)
		    str = vim_strsave(str);
		ಆದರೆ (save_ಉತ್ತರ(sender, str,
			   (ದತ್ತಾಂಶ->dw_ದತ್ತಾಂಶ == COPYDATA_ಉತ್ತರ ?  0 :
			   (ದತ್ತಾಂಶ->dw_ದತ್ತಾಂಶ == COPYDATA_ಫಲಿತಾಂಶ ? 1 :
							      2))) == FAIL)
		    vim_free(str);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ದತ್ತಾಂಶ->dw_ದತ್ತಾಂಶ == COPYDATA_ಉತ್ತರ)
		{
		    char_u	winstr[30];

		    sprintf((ಅಕ್ಷರ *)winstr, PRINTF_HEX_LONG_U, (long_u)sender);
		    apply_autocmds(EVENT_REMOTEREPLY, winstr, str,
								TRUE, curbuf);
		}
	    }
	    ಹಿಂತಿರುಗಿ 1;
	}

	ಹಿಂತಿರುಗಿ 0;
    }

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸಂದೇಶ == WM_ACTIVATE && wParam == WA_ಸಕ್ರಿಯ)
    {
	// When the message window is activated (brought to the foreground),
	// this actually applies to the text window.
# ಆದರೆ !defined(FEAT_GUI) || defined(VIMDLL)
#  ifdef VIMDLL
	ಆದರೆ (!gui.ಒಳ_use)
#  endif
	    GetConsoleHwnd();	    // get value of s_hwnd
# endif
	ಆದರೆ (s_hwnd != 0)
	{
	    ಗಣ_Foreground_ಕಿಟಕಿ(s_hwnd);
	    ಹಿಂತಿರುಗಿ 0;
	}
    }

    ಹಿಂತಿರುಗಿ Def_ಕಿಟಕಿ_Proc(hwnd, ಸಂದೇಶ, wParam, lParam);
}

/*
 * Initialise the message handling process.  This involves creating a window
 * to handle messages - the window will not be visible.
 */
    ಶೂನ್ಯ
serverInitMessaging(ಶೂನ್ಯ)
{
    WNDCLASS wndclass;

    // Clean up on exit
    atexit(Clean_ಮೇಲೆ_Messaging);

    // Register a window class - we only really care
    // about the window procedure
    wndclass.style = 0;
    wndclass.lpfnWndProc = Messaging_WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = g_hinst;
    wndclass.hIcon = ಖಾಲಿ;
    wndclass.hCursor = ಖಾಲಿ;
    wndclass.hbrBackground = ಖಾಲಿ;
    wndclass.lpszMenuName = ಖಾಲಿ;
    wndclass.lpszClassName = VIM_CLASSNAME;
    ರಿಜಿಸ್ಟರ್_Class(&wndclass);

    // Create the message window. It will be hidden, so the details don't
    // matter.  Don't use WS_OVERLAPPEDWINDOW, it will make a shortcut remove
    // focus from gvim.
    ಸಂದೇಶ_ಕಿಟಕಿ = Create_ಕಿಟಕಿ(VIM_CLASSNAME, "",
			 WS_POPUPWINDOW | WS_CAPTION,
			 CW_USEDEFAULT, CW_USEDEFAULT,
			 100, 100, ಖಾಲಿ, ಖಾಲಿ,
			 g_hinst, ಖಾಲಿ);
}

// Used by serverSendToVim() to find an alternate server name.
ಸ್ಥಿರ char_u *altname_buf_ptr = ಖಾಲಿ;

/*
 * Get the title of the window "hwnd", which is the Vim server name, in
 * "name[namelen]" and return the length.
 * Returns zero if window "hwnd" is not a Vim server.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
getVimServerName(HWND hwnd, ಅಕ್ಷರ *name, ಪೂರ್ಣಾಂಕ namelen)
{
    ಪೂರ್ಣಾಂಕ		len;
    ಅಕ್ಷರ	ಬಫರ್[VIM_CLASSNAME_LEN + 1];

    // Ignore windows which aren't Vim message windows
    len = GetClassName(hwnd, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್));
    ಆದರೆ (len != VIM_CLASSNAME_LEN || STRCMP(ಬಫರ್, VIM_CLASSNAME) != 0)
	ಹಿಂತಿರುಗಿ 0;

    // Get the title of the window
    ಹಿಂತಿರುಗಿ Get_ಕಿಟಕಿ_ಪಠ್ಯ(hwnd, name, namelen);
}

    ಸ್ಥಿರ BOOL CALLBACK
enumWindowsGetServer(HWND hwnd, LPARAM lparam)
{
    ರಚನೆ	server_id *id = (ರಚನೆ server_id *)lparam;
    ಅಕ್ಷರ	server[MAX_ಮಾರ್ಗ];

    // Get the title of the window
    ಆದರೆ (getVimServerName(hwnd, server, ಗಾತ್ರ(server)) == 0)
	ಹಿಂತಿರುಗಿ TRUE;

    // If this is the server we're looking for, return its HWND
    ಆದರೆ (STRICMP(server, id->name) == 0)
    {
	id->hwnd = hwnd;
	ಹಿಂತಿರುಗಿ FALSE;
    }

    // If we are looking for an alternate server, remember this name.
    ಆದರೆ (altname_buf_ptr != ಖಾಲಿ)
    {
	size_t	namelen = STRLEN(id->name);

	ಆದರೆ (STRNICMP(server, id->name, namelen) == 0
	    && vim_isdigit(server[namelen]))
	{
	    STRCPY(altname_buf_ptr, server);
	    altname_buf_ptr = ಖಾಲಿ;	    // don't use another name
	}
    }

    // Otherwise, keep looking
    ಹಿಂತಿರುಗಿ TRUE;
}

    ಸ್ಥಿರ BOOL CALLBACK
enum_Windows_Get_ಹೆಸರುಗಳು(HWND hwnd, LPARAM lparam)
{
    list_T	*list = (list_T *)lparam;
    ಅಕ್ಷರ	server[MAX_ಮಾರ್ಗ];

    // Get the title of the window
    ಆದರೆ (getVimServerName(hwnd, server, ಗಾತ್ರ(server)) == 0)
	ಹಿಂತಿರುಗಿ TRUE;

    list_append_string(list, (char_u *)server, -1);
    ಹಿಂತಿರುಗಿ TRUE;
}

ರಚನೆ enum_windows_s
{
    WNDENUMPROC lpEnumFunc;
    LPARAM      lParam;
};

    ಸ್ಥಿರ BOOL CALLBACK
enum_windows_ಮಗು(HWND hwnd, LPARAM lParam)
{
    ರಚನೆ enum_windows_s *ew = (ರಚನೆ enum_windows_s *)lParam;

    ಹಿಂತಿರುಗಿ (ew->lpEnumFunc)(hwnd, ew->lParam);
}

    ಸ್ಥಿರ BOOL CALLBACK
enum_windows_toplevel(HWND hwnd, LPARAM lParam)
{
    ರಚನೆ enum_windows_s *ew = (ರಚನೆ enum_windows_s *)lParam;

    ಆದರೆ ((ew->lpEnumFunc)(hwnd, ew->lParam))
	ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ Enum_ಮಗು_Windows(hwnd, enum_windows_ಮಗು, lParam);
}

/*
 * Enumerate all windows including children.
 */
    ಸ್ಥಿರ BOOL
enum_windows(WNDENUMPROC lpEnumFunc, LPARAM lParam)
{
    ರಚನೆ enum_windows_s ew;

    ew.lpEnumFunc = lpEnumFunc;
    ew.lParam = lParam;
    ಹಿಂತಿರುಗಿ EnumWindows(enum_windows_toplevel, (LPARAM)&ew);
}

    ಸ್ಥಿರ HWND
ಕಂಡುಹಿಡಿ_Server(char_u *name)
{
    ರಚನೆ server_id id;

    id.name = name;
    id.hwnd = 0;

    enum_windows(enumWindowsGetServer, (LPARAM)(&id));

    ಹಿಂತಿರುಗಿ id.hwnd;
}

    ಶೂನ್ಯ
server_ಗಣ_Name(char_u *name)
{
    size_t	namelen;
    char_u	*ok_name;
    HWND	hwnd = 0;
    ಪೂರ್ಣಾಂಕ		i = 0;
    char_u	*p;

    // Leave enough space for a 9-digit suffix to ensure uniqueness!
    namelen = STRLEN(name);
    ok_name = alloc(namelen + 10);
    ಆದರೆ (ok_name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    STRCPY(ok_name, name);
    p = ok_name + namelen;

    ಗಾಗಿ (;;)
    {
	// This is inefficient - we're doing an EnumWindows loop for each
	// possible name. It would be better to grab all names in one go,
	// and scan the list each time...
	hwnd = ಕಂಡುಹಿಡಿ_Server(ok_name);
	ಆದರೆ (hwnd == 0)
	    ನಿಲ್ಲಿಸು;

	++i;
	ಆದರೆ (i >= 1000)
	    ನಿಲ್ಲಿಸು;

	sprintf((ಅಕ್ಷರ *)p, "%d", i);
    }

    ಆದರೆ (hwnd != 0)
	vim_free(ok_name);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Remember the name
	serverName = ok_name;
	need_maketitle = TRUE;	// update Vim window title later

	// Update the message window title
	ಗಣ_ಕಿಟಕಿ_ಪಠ್ಯ(ಸಂದೇಶ_ಕಿಟಕಿ, (LPCSTR)ok_name);

# ifdef FEAT_EVAL
	// Set the servername variable
	ಗಣ_vim_var_string(VV_SEND_SERVER, serverName, -1);
# endif
    }
}

    list_T *
server_Get_Vim_ಹೆಸರುಗಳು(ಶೂನ್ಯ)
{
    list_T *list = list_alloc();

    ಆದರೆ (list == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    enum_windows(enum_Windows_Get_ಹೆಸರುಗಳು, (LPARAM)list);

    ಹಿಂತಿರುಗಿ list;
}

    ಪೂರ್ಣಾಂಕ
server_Send_ಉತ್ತರ(
    char_u	*name,		// Where to send.
    char_u	*ಉತ್ತರ)		// What to send.
{
    HWND	ಗುರಿ;
    COPYDATASTRUCT ದತ್ತಾಂಶ;
    long_u	n = 0;
    DWORD_PTR	dwret = 0;

    // The "name" argument is a magic cookie obtained from expand("<client>").
    // It should be of the form 0xXXXXX - i.e. a C hex literal, which is the
    // value of the client's message window HWND.
    ಅಕ್ಷರಸರಣಿ_ಓದು((ಅಕ್ಷರ *)name, SCANF_HEX_LONG_U, &n);
    ಆದರೆ (n == 0)
	ಹಿಂತಿರುಗಿ -1;

    ಗುರಿ = (HWND)n;
    ಆದರೆ (!Is_ಕಿಟಕಿ(ಗುರಿ))
	ಹಿಂತಿರುಗಿ -1;

    ದತ್ತಾಂಶ.dw_ದತ್ತಾಂಶ = COPYDATA_ಉತ್ತರ;
    ದತ್ತಾಂಶ.cb_ದತ್ತಾಂಶ = (DWORD)STRLEN(ಉತ್ತರ) + 1;
    ದತ್ತಾಂಶ.lp_ದತ್ತಾಂಶ = ಉತ್ತರ;

    ಆದರೆ (serverSendEnc(ಗುರಿ) < 0)
	ಹಿಂತಿರುಗಿ -1;
    ಆದರೆ (Send_ಸಂದೇಶ_Timeout(ಗುರಿ, WM_COPYDATA,
		(WPARAM)ಸಂದೇಶ_ಕಿಟಕಿ, (LPARAM)&ದತ್ತಾಂಶ,
		SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
	ಹಿಂತಿರುಗಿ -1;
    ಹಿಂತಿರುಗಿ dwret ? 0 : -1;
}

    ಪೂರ್ಣಾಂಕ
server_Send_ಗಮ್ಯ_Vim(
    char_u	 *name,			// Where to send.
    char_u	 *cmd,			// What to send.
    char_u	 **ಫಲಿತಾಂಶ,		// Result of eval'ed expression
    ಶೂನ್ಯ	 *ptarget,		// HWND of server
    ಪೂರ್ಣಾಂಕ		 as_ಅಭಿವ್ಯಕ್ತಿ,		// Expression or keys?
    ಪೂರ್ಣಾಂಕ		 timeout,		// timeout in seconds or zero
    ಪೂರ್ಣಾಂಕ		 silent)		// don't complain about no server
{
    size_t	namelen;
    HWND	ಗುರಿ;
    COPYDATASTRUCT ದತ್ತಾಂಶ;
    char_u	*retval = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		retcode = 0;
    DWORD_PTR	dwret = 0;
    char_u	altname_buf[MAX_ಮಾರ್ಗ];

    // Execute locally if no display or target is ourselves
    ಆದರೆ (serverName != ಖಾಲಿ && STRICMP(name, serverName) == 0)
	ಹಿಂತಿರುಗಿ send_ಗಮ್ಯ_Local_Vim(cmd, as_ಅಭಿವ್ಯಕ್ತಿ, ಫಲಿತಾಂಶ);

    // If the server name does not end in a digit then we look for an
    // alternate name.  e.g. when "name" is GVIM then we may find GVIM2.
    namelen = STRLEN(name);
    ಆದರೆ (namelen > 1 && !vim_isdigit(name[namelen - 1]))
	altname_buf_ptr = altname_buf;
    altname_buf[0] = NUL;
    ಗುರಿ = ಕಂಡುಹಿಡಿ_Server(name);
    altname_buf_ptr = ಖಾಲಿ;
    ಆದರೆ (ಗುರಿ == 0 && altname_buf[0] != NUL)
	// Use another server name we found.
	ಗುರಿ = ಕಂಡುಹಿಡಿ_Server(altname_buf);

    ಆದರೆ (ಗುರಿ == 0)
    {
	ಆದರೆ (!silent)
	    semsg(_(e_no_registered_server_named_str), name);
	ಹಿಂತಿರುಗಿ -1;
    }

    ಆದರೆ (ptarget)
	*(HWND *)ptarget = ಗುರಿ;

    ದತ್ತಾಂಶ.dw_ದತ್ತಾಂಶ = as_ಅಭಿವ್ಯಕ್ತಿ ? COPYDATA_ಅಭಿವ್ಯಕ್ತಿ : COPYDATA_ಕೀಲಿಗಳು;
    ದತ್ತಾಂಶ.cb_ದತ್ತಾಂಶ = (DWORD)STRLEN(cmd) + 1;
    ದತ್ತಾಂಶ.lp_ದತ್ತಾಂಶ = cmd;

    ಆದರೆ (serverSendEnc(ಗುರಿ) < 0)
	ಹಿಂತಿರುಗಿ -1;
    ಆದರೆ (Send_ಸಂದೇಶ_Timeout(ಗುರಿ, WM_COPYDATA,
		(WPARAM)ಸಂದೇಶ_ಕಿಟಕಿ, (LPARAM)&ದತ್ತಾಂಶ,
		SMTO_ABORTIFHUNG, SENDMESSAGE_TIMEOUT, &dwret) == 0)
	ಹಿಂತಿರುಗಿ -1;
    ಆದರೆ (dwret == 0)
	ಹಿಂತಿರುಗಿ -1;

    ಆದರೆ (as_ಅಭಿವ್ಯಕ್ತಿ)
	retval = server_Get_ಉತ್ತರ(ಗುರಿ, &retcode, TRUE, TRUE, timeout);

    ಆದರೆ (ಫಲಿತಾಂಶ == ಖಾಲಿ)
	vim_free(retval);
    ಇಲ್ಲದಿದ್ದರೆ
	*ಫಲಿತಾಂಶ = retval; // Caller assumes responsibility for freeing

    ಹಿಂತಿರುಗಿ retcode;
}

/*
 * Bring the server to the foreground.
 */
    ಶೂನ್ಯ
serverForeground(char_u *name)
{
    HWND	ಗುರಿ = ಕಂಡುಹಿಡಿ_Server(name);

    ಆದರೆ (ಗುರಿ != 0)
	ಗಣ_Foreground_ಕಿಟಕಿ(ಗುರಿ);
}

// Replies from server need to be stored until the client picks them up via
// remote_read(). So we maintain a list of server-id/reply pairs.
// Note that there could be multiple replies from one server pending if the
// client is slow picking them up.
// We just store the replies in a simple list. When we remove an entry, we
// move list entries down to fill the gap.
// The server ID is simply the HWND.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    HWND	server;		// server window
    char_u	*ಉತ್ತರ;		// reply string
    ಪೂರ್ಣಾಂಕ		ಅಭಿವ್ಯಕ್ತಿ_ಫಲಿತಾಂಶ;	// 0 for REPLY, 1 for RESULT 2 for error
} ಉತ್ತರ_T;

ಸ್ಥಿರ garray_T ಉತ್ತರ_list = {0, 0, ಗಾತ್ರ(ಉತ್ತರ_T), 5, 0};

# define ಉತ್ತರ_ಅಂಶ(i) ((ಉತ್ತರ_T *)(ಉತ್ತರ_list.ga_ದತ್ತಾಂಶ) + (i))
# define ಉತ್ತರ_ಎಣಿಕೆ (ಉತ್ತರ_list.ga_len)

// Flag which is used to wait for a reply
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಉತ್ತರ_received = 0;

/*
 * Store a reply.  "reply" must be allocated memory (or NULL).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
save_ಉತ್ತರ(HWND server, char_u *ಉತ್ತರ, ಪೂರ್ಣಾಂಕ ಅಭಿವ್ಯಕ್ತಿ)
{
    ಉತ್ತರ_T *rep;

    ಆದರೆ (ga_grow(&ಉತ್ತರ_list, 1) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    rep = ಉತ್ತರ_ಅಂಶ(ಉತ್ತರ_ಎಣಿಕೆ);
    rep->server = server;
    rep->ಉತ್ತರ = ಉತ್ತರ;
    rep->ಅಭಿವ್ಯಕ್ತಿ_ಫಲಿತಾಂಶ = ಅಭಿವ್ಯಕ್ತಿ;
    ಆದರೆ (rep->ಉತ್ತರ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ++ಉತ್ತರ_ಎಣಿಕೆ;
    ಉತ್ತರ_received = 1;
    ಹಿಂತಿರುಗಿ OK;
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
server_Get_ಉತ್ತರ(HWND server, ಪೂರ್ಣಾಂಕ *ಅಭಿವ್ಯಕ್ತಿ_res, ಪೂರ್ಣಾಂಕ remove, ಪೂರ್ಣಾಂಕ wait, ಪೂರ್ಣಾಂಕ timeout)
{
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*ಉತ್ತರ;
    ಉತ್ತರ_T	*rep;
    ಪೂರ್ಣಾಂಕ		did_process = FALSE;
    time_t	ಆರಂಭ;
    time_t	now;

    // When waiting, loop until the message waiting for is received.
    ಸಮಯ(&ಆರಂಭ);
    ಗಾಗಿ (;;)
    {
	// Reset this here, in case a message arrives while we are going
	// through the already received messages.
	ಉತ್ತರ_received = 0;

	ಗಾಗಿ (i = 0; i < ಉತ್ತರ_ಎಣಿಕೆ; ++i)
	{
	    rep = ಉತ್ತರ_ಅಂಶ(i);
	    ಆದರೆ (rep->server == server
		    && ((rep->ಅಭಿವ್ಯಕ್ತಿ_ಫಲಿತಾಂಶ != 0) == (ಅಭಿವ್ಯಕ್ತಿ_res != ಖಾಲಿ)))
	    {
		// Save the values we've found for later
		ಉತ್ತರ = rep->ಉತ್ತರ;
		ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ_res != ಖಾಲಿ)
		    *ಅಭಿವ್ಯಕ್ತಿ_res = rep->ಅಭಿವ್ಯಕ್ತಿ_ಫಲಿತಾಂಶ == 1 ? 0 : -1;

		ಆದರೆ (remove)
		{
		    // Move the rest of the list down to fill the gap
		    mch_memmove(rep, rep + 1,
				     (ಉತ್ತರ_ಎಣಿಕೆ - i - 1) * ಗಾತ್ರ(ಉತ್ತರ_T));
		    --ಉತ್ತರ_ಎಣಿಕೆ;
		}

		// Return the reply to the caller, who takes on responsibility
		// for freeing it if "remove" is TRUE.
		ಹಿಂತಿರುಗಿ ಉತ್ತರ;
	    }
	}

	// If we got here, we didn't find a reply. Return immediately if the
	// "wait" parameter isn't set.
	ಆದರೆ (!wait)
	{
	    // Process pending messages once. Without this, looping on
	    // remote_peek() would never get the reply.
	    ಆದರೆ (!did_process)
	    {
		did_process = TRUE;
		serverProcessPendingMessages();
		ಮುಂದುವರಿ;
	    }
	    ನಿಲ್ಲಿಸು;
	}

	// We need to wait for a reply. Enter a message loop until the
	// "reply_received" flag gets set.

	// Loop until we receive a reply
	ಆಗ (ಉತ್ತರ_received == 0)
	{
# ifdef FEAT_TIMERS
	    // TODO: use the return value to decide how long to wait.
	    check_due_timer();
# endif
	    ಸಮಯ(&now);
	    ಆದರೆ (timeout > 0 && (now - ಆರಂಭ) >= timeout)
		ನಿಲ್ಲಿಸು;

	    // Wait for a SendMessage() call to us.  This could be the reply
	    // we are waiting for.  Use a timeout of a second, to catch the
	    // situation that the server died unexpectedly.
	    ಸಂದೇಶ_Wait_For_ಅಪವರ್ತ್ಯ_Objects(0, ಖಾಲಿ, TRUE, 1000, QS_ALLINPUT);

	    // If the server has died, give up
	    ಆದರೆ (!Is_ಕಿಟಕಿ(server))
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	    serverProcessPendingMessages();
	}
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Process any messages in the Windows message queue.
 */
    ಶೂನ್ಯ
serverProcessPendingMessages(ಶೂನ್ಯ)
{
    ಸಂದೇಶ_3 ಸಂದೇಶ;

    ಆಗ (Peek_ಸಂದೇಶ_W(&ಸಂದೇಶ, ಖಾಲಿ, 0, 0, PM_REMOVE))
    {
	Translate_ಸಂದೇಶ(&ಸಂದೇಶ);
	Dispatch_ಸಂದೇಶ_W(&ಸಂದೇಶ);
    }
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_CLIENTSERVER

#ಆದರೆ defined(FEAT_GUI) || (defined(FEAT_PRINTER) && !defined(FEAT_POSTSCRIPT))

ರಚನೆ charset_pair
{
    string_T	name;
    BYTE	charset;
};

ಸ್ಥಿರ ರಚನೆ charset_pair
charset_ಜೋಡಿಗಳು[] =
{
    {STR_LITERAL_INIT("ANSI"),		ANSI_CHARSET},
    {STR_LITERAL_INIT("CHINESEBIG5"),	CHINESEBIG5_CHARSET},
    {STR_LITERAL_INIT("DEFAULT"),	DEFAULT_CHARSET},
    {STR_LITERAL_INIT("HANGEUL"),	HANGEUL_CHARSET},
    {STR_LITERAL_INIT("OEM"),		OEM_CHARSET},
    {STR_LITERAL_INIT("SHIFTJIS"),	SHIFTJIS_CHARSET},
    {STR_LITERAL_INIT("SYMBOL"),	ಚಿಹ್ನೆ_CHARSET},
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

ರಚನೆ quality_pair
{
    string_T	name;
    DWORD	quality;
};

ಸ್ಥಿರ ರಚನೆ quality_pair
quality_ಜೋಡಿಗಳು[] = {
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
    ಅಕ್ಷರ *
charset_id2name(ಪೂರ್ಣಾಂಕ id)
{
    ಪೂರ್ಣಾಂಕ	i;

    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(charset_ಜೋಡಿಗಳು); ++i)
    {
	ಆದರೆ ((BYTE)id == charset_ಜೋಡಿಗಳು[i].charset)
	    ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ *)charset_ಜೋಡಿಗಳು[i].name.string;
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Convert a quality ID to a name.
 * Return NULL when not recognized.
 */
    ಅಕ್ಷರ *
quality_id2name(DWORD id)
{
    ಪೂರ್ಣಾಂಕ	i;

    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(quality_ಜೋಡಿಗಳು); ++i)
    {
	ಆದರೆ (id == quality_ಜೋಡಿಗಳು[i].quality)
	    ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ *)quality_ಜೋಡಿಗಳು[i].name.string;
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

// The default font height in 100% scaling (96dpi).
// (-16 in 96dpi equates to roughly 12pt)
# define DEFAULT_FONT_ಎತ್ತರ	(-16)

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ LOGFONTW s_lfDefault =
{
    DEFAULT_FONT_ಎತ್ತರ,
    0, 0, 0, FW_ಸಾಮಾನ್ಯ, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
    ಹೊರಹೋಗು_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
    PROOF_QUALITY, FIXED_PITCH | FF_DONTCARE,
    L""	// Default font name will be set later based on current language.
};

// This will be initialized when set_default_logfont() is called first time.
// The value will be based on the system DPI.
ಪೂರ್ಣಾಂಕ current_font_ಎತ್ತರ = 0;		// also used in gui_w32.c

/*
 * Convert a string representing a point size into pixels. The string should
 * be a positive decimal number, with an optional decimal point (eg, "12", or
 * "10.5"). The pixel value is returned, and a pointer to the next unconverted
 * character is stored in *end. The flag "vertical" says whether this
 * calculation is for a vertical (height) size or a horizontal (width) one.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
points_ಗಮ್ಯ_pixels(WCHAR *str, WCHAR **ಕೊನೆ, ಪೂರ್ಣಾಂಕ vertical, long_i pprinter_dc)
{
    ಪೂರ್ಣಾಂಕ		pixels;
    ಪೂರ್ಣಾಂಕ		points = 0;
    ಪೂರ್ಣಾಂಕ		ಭಾಜಕ = 0;
    HWND	hwnd = (HWND)0;
    HDC		hdc;
    HDC		printer_dc = (HDC)pprinter_dc;

    ಆಗ (*str != NUL)
    {
	ಆದರೆ (*str == L'.' && ಭಾಜಕ == 0)
	{
	    // Start keeping a divisor, for later
	    ಭಾಜಕ = 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (!VIM_ISDIGIT(*str))
		ನಿಲ್ಲಿಸು;

	    points *= 10;
	    points += *str - L'0';
	    ಭಾಜಕ *= 10;
	}
	++str;
    }

    ಆದರೆ (ಭಾಜಕ == 0)
	ಭಾಜಕ = 1;

    ಆದರೆ (printer_dc == ಖಾಲಿ)
    {
	hwnd = Get_Desktop_ಕಿಟಕಿ();
	hdc = Get_ಕಿಟಕಿ_DC(hwnd);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	hdc = printer_dc;

    pixels = MulDiv(points,
		    GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX),
		    72 * ಭಾಜಕ);

    ಆದರೆ (printer_dc == ಖಾಲಿ)
	ReleaseDC(hwnd, hdc);

    *ಕೊನೆ = str;
    ಹಿಂತಿರುಗಿ pixels;
}

/*
 * Convert pixel into point size. This is a reverse of points_to_pixels.
 */
    ಸ್ಥಿರ ದ್ವಿಗುಣ
pixels_ಗಮ್ಯ_points(ಪೂರ್ಣಾಂಕ pixels, ಪೂರ್ಣಾಂಕ vertical, long_i pprinter_dc)
{
    ದ್ವಿಗುಣ	points = 0;
    HWND	hwnd = (HWND)0;
    HDC		hdc;
    HDC		printer_dc = (HDC)pprinter_dc;

    ಆದರೆ (printer_dc == ಖಾಲಿ)
    {
	hwnd = Get_Desktop_ಕಿಟಕಿ();
	hdc = Get_ಕಿಟಕಿ_DC(hwnd);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	hdc = printer_dc;

    points = pixels * 72.0 / GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX);
    ಆದರೆ (printer_dc == ಖಾಲಿ)
	ReleaseDC(hwnd, hdc);

    ಹಿಂತಿರುಗಿ points;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ CALLBACK
font_enumproc(
    ENUMLOGFONTW    *elf,
    NEWTEXTMETRICW  *ntm ಬಳಸದ,
    DWORD	    type ಬಳಸದ,
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
    ಆದರೆ ((ntm->tmPitchAndFamily & 1) != 0)
	ಹಿಂತಿರುಗಿ 1;
# endif

    // Remember this LOGFONTW as a "possible"
    *lf = elf->elfLogFont;

    // Terminate the scan as soon as we find an ANSI font
    ಆದರೆ (lf->lf_Char_ಗಣ == ANSI_CHARSET
	    || lf->lf_Char_ಗಣ == OEM_CHARSET
	    || lf->lf_Char_ಗಣ == DEFAULT_CHARSET)
	ಹಿಂತಿರುಗಿ 0;

    // Continue the scan - we have a non-ANSI font
    ಹಿಂತಿರುಗಿ 2;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
init_logfont(LOGFONTW *lf)
{
    ಪೂರ್ಣಾಂಕ		n;
    HWND	hwnd = Get_Desktop_ಕಿಟಕಿ();
    HDC		hdc = Get_ಕಿಟಕಿ_DC(hwnd);

    n = EnumFontFamiliesW(hdc,
			 lf->lfFaceName,
			 (FONTENUMPROCW)font_enumproc,
			 (LPARAM)lf);

    ReleaseDC(hwnd, hdc);

    // If we couldn't find a usable font, return failure
    ಆದರೆ (n == 1)
	ಹಿಂತಿರುಗಿ FAIL;

    // Tidy up the rest of the LOGFONTW structure. We set to a basic
    // font - get_logfont() sets bold, italic, etc based on the user's
    // input.
    lf->lf_ಎತ್ತರ = current_font_ಎತ್ತರ;
    lf->lf_ಅಗಲ = 0;
    lf->lfItalic = FALSE;
    lf->lfUnderline = FALSE;
    lf->lf_Strike_ಹೊರಹೋಗು = FALSE;
    lf->lf_ತೂಕ = FW_ಸಾಮಾನ್ಯ;

    // Return success
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Call back for EnumFontFamiliesW in expand_font_enumproc.
 *
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ CALLBACK
expand_font_enumproc(
    ENUMLOGFONTW    *elf,
    NEWTEXTMETRICW  *ntm ಬಳಸದ,
    DWORD	    type ಬಳಸದ,
    LPARAM	    lparam)
{
    LOGFONTW *lf = (LOGFONTW*)elf;

# ifndef FEAT_PROPORTIONAL_FONTS
    // Ignore non-monospace fonts without further ado
    ಆದರೆ ((ntm->tmPitchAndFamily & 1) != 0)
	ಹಿಂತಿರುಗಿ 1;
# endif

    // Filter only on ANSI. Otherwise will see a lot of random fonts that we
    // usually don't want.
    ಆದರೆ (lf->lf_Char_ಗಣ != ANSI_CHARSET)
	ಹಿಂತಿರುಗಿ 1;

    ಪೂರ್ಣಾಂಕ (*ಕೂಡಿಸು_ಹೊಂದಿಕೆ)(char_u *) = (ಪೂರ್ಣಾಂಕ (*)(char_u *))lparam;

    WCHAR *faceNameW = lf->lfFaceName;
    char_u *faceName = utf16_ಗಮ್ಯ_enc(faceNameW, ಖಾಲಿ);
    ಆದರೆ (!faceName)
	ಹಿಂತಿರುಗಿ 0;

    ಕೂಡಿಸು_ಹೊಂದಿಕೆ(faceName);
    vim_free(faceName);

    ಹಿಂತಿರುಗಿ 1;
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
    ಶೂನ್ಯ
gui_mch_expand_font(
    optexpand_T	*ವಾದಗಳು,
    ಶೂನ್ಯ	*param ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ		(*ಕೂಡಿಸು_ಹೊಂದಿಕೆ)(char_u *val))
{
    expand_T	    *xp = ವಾದಗಳು->oe_xp;
    ಆದರೆ (xp->xp_ನಮೂನೆ > ವಾದಗಳು->oe_ಗಣ_arg && *(xp->xp_ನಮೂನೆ-1) == ':')
    {
	ಅಕ್ಷರ buf[30];
	ಪೂರ್ಣಾಂಕ	i;

	// Always fill in with the current font size as first option for
	// convenience. We simply round to the closest integer for simplicity.
	ಪೂರ್ಣಾಂಕ font_ಎತ್ತರ = (ಪೂರ್ಣಾಂಕ)ಸುತ್ತು(
		pixels_ಗಮ್ಯ_points(-current_font_ಎತ್ತರ, TRUE, (long_i)ಖಾಲಿ));
	vim_snprintf(buf, ARRAY_ಉದ್ದಳತೆ(buf), "h%d", font_ಎತ್ತರ);
	ಕೂಡಿಸು_ಹೊಂದಿಕೆ((char_u *)buf);

	// Note: Keep this in sync with get_logfont(). Don't include 'c' and
	// 'q' as we fill in all the values below.
	ಸ್ಥಿರ ಅಕ್ಷರ *(p_gfn_win_opt_ಮೌಲ್ಯಗಳು[]) = {
	    "h" , "w" , "W" , "b" , "i" , "u" , "s"};
	ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(p_gfn_win_opt_ಮೌಲ್ಯಗಳು); i++)
	    ಕೂಡಿಸು_ಹೊಂದಿಕೆ((char_u *)p_gfn_win_opt_ಮೌಲ್ಯಗಳು[i]);

	ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(charset_ಜೋಡಿಗಳು); ++i)
	{
	    vim_snprintf(buf, ಗಾತ್ರ(buf), "c%s", charset_ಜೋಡಿಗಳು[i].name.string);
	    ಕೂಡಿಸು_ಹೊಂದಿಕೆ((char_u *)buf);
	}

	ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(quality_ಜೋಡಿಗಳು); ++i)
	{
	    vim_snprintf(buf, ಗಾತ್ರ(buf), "q%s", quality_ಜೋಡಿಗಳು[i].name.string);
	    ಕೂಡಿಸು_ಹೊಂದಿಕೆ((char_u *)buf);
	}
	ಹಿಂತಿರುಗಿ;
    }

    HWND	hwnd = Get_Desktop_ಕಿಟಕಿ();
    HDC		hdc = Get_ಕಿಟಕಿ_DC(hwnd);

    EnumFontFamiliesW(hdc,
	    ಖಾಲಿ,
	    (FONTENUMPROCW)expand_font_enumproc,
	    (LPARAM)ಕೂಡಿಸು_ಹೊಂದಿಕೆ);

    ReleaseDC(hwnd, hdc);
}

/*
 * Compare a UTF-16 string and an ASCII string literally.
 * Only works all the code points are inside ASCII range.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
utf16ascncmp(ಸ್ಥಿರಾಂಕ WCHAR *w, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, size_t n)
{
    size_t i;

    ಗಾಗಿ (i = 0; i < n; i++)
    {
	ಆದರೆ (w[i] == 0 || w[i] != p[i])
	    ಹಿಂತಿರುಗಿ w[i] - p[i];
    }
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Equivalent of GetDpiForSystem().
 */
    UINT WINAPI
vimGetDpiForSystem(ಶೂನ್ಯ)
{
    HWND hwnd = Get_Desktop_ಕಿಟಕಿ();
    HDC hdc = Get_ಕಿಟಕಿ_DC(hwnd);
    UINT dpi = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(hwnd, hdc);
    ಹಿಂತಿರುಗಿ dpi;
}

/*
 * Set default logfont based on current language.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_default_logfont(LOGFONTW *lf)
{
    // Default font name for current language on MS-Windows.
    // If not translated, falls back to "Consolas".
    // This must be a fixed-pitch font.
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *defaultfontname = N_("DefaultFontNameForWindows");
    ಅಕ್ಷರ *fontname = _(defaultfontname);

    ಆದರೆ (STRCMP(fontname, defaultfontname) == 0)
	fontname = "Consolas";

    *lf = s_lfDefault;
    lf->lf_ಎತ್ತರ = DEFAULT_FONT_ಎತ್ತರ * (ಪೂರ್ಣಾಂಕ)vimGetDpiForSystem() / 96;
    ಆದರೆ (current_font_ಎತ್ತರ == 0)
	current_font_ಎತ್ತರ = lf->lf_ಎತ್ತರ;

    WCHAR *wfontname = enc_ಗಮ್ಯ_utf16((char_u*)fontname, ಖಾಲಿ);
    ಆದರೆ (wfontname != ಖಾಲಿ)
    {
	wcscpy_s(lf->lfFaceName, LF_FACESIZE, wfontname);
	vim_free(wfontname);
    }
}

/*
 * Get font info from "name" into logfont "lf".
 * Return OK for a valid name, FAIL otherwise.
 */
    ಪೂರ್ಣಾಂಕ
get_logfont(
    LOGFONTW	*lf,
    char_u	*name,
    HDC		printer_dc,
    ಪೂರ್ಣಾಂಕ		verbose)
{
    WCHAR	*p;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		ret = FAIL;
    ಸ್ಥಿರ LOGFONTW *lastlf = ಖಾಲಿ;
    WCHAR	*wname;

    ಗಣ_default_logfont(lf);
    ಆದರೆ (name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ OK;

    wname = enc_ಗಮ್ಯ_utf16(name, ಖಾಲಿ);
    ಆದರೆ (wname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (wcscmp(wname, L"*") == 0)
    {
# ಆದರೆ defined(FEAT_GUI_MSWIN)
	CHOOSEFONTW	cf;
	// if name is "*", bring up std font dialog:
	CLEAR_ಕ್ಷೇತ್ರ(cf);
	cf.l_Struct_ಗಾತ್ರ = ಗಾತ್ರ(cf);
	cf.hwndOwner = s_hwnd;
	cf.Flags = CF_SCREENFONTS | CF_FIXEDPITCHONLY | CF_INITTOLOGFONTSTRUCT;
	ಆದರೆ (lastlf != ಖಾಲಿ)
	    *lf = *lastlf;
	cf.lpLogFont = lf;
	cf.nFontType = 0 ; //REGULAR_FONTTYPE;
	ಆದರೆ (ಆರಿಸು_Font_W(&cf))
	    ret = OK;
# endif
	ಹೋಗು theend;
    }

    /*
     * Split name up, it could be <name>:h<height>:w<width> etc.
     */
    ಗಾಗಿ (p = wname; *p && *p != L':'; p++)
    {
	ಆದರೆ (p - wname + 1 >= LF_FACESIZE)
	    ಹೋಗು theend;			// Name too long
	lf->lfFaceName[p - wname] = *p;
    }
    ಆದರೆ (p != wname)
	lf->lfFaceName[p - wname] = NUL;

    // First set defaults
    lf->lf_ಎತ್ತರ = DEFAULT_FONT_ಎತ್ತರ * (ಪೂರ್ಣಾಂಕ)vimGetDpiForSystem() / 96;
    lf->lf_ಅಗಲ = 0;
    lf->lf_ತೂಕ = FW_ಸಾಮಾನ್ಯ;
    lf->lfItalic = FALSE;
    lf->lfUnderline = FALSE;
    lf->lf_Strike_ಹೊರಹೋಗು = FALSE;

    /*
     * If the font can't be found, try replacing '_' by ' '.
     */
    ಆದರೆ (init_logfont(lf) == FAIL)
    {
	ಪೂರ್ಣಾಂಕ	did_replace = FALSE;

	ಗಾಗಿ (i = 0; lf->lfFaceName[i]; ++i)
	    ಆದರೆ (lf->lfFaceName[i] == L'_')
	    {
		lf->lfFaceName[i] = L' ';
		did_replace = TRUE;
	    }
	ಆದರೆ (!did_replace || init_logfont(lf) == FAIL)
	    ಹೋಗು theend;
    }

    ಆಗ (*p == L':')
	p++;

    // Set the values found after ':'
    ಆಗ (*p)
    {
	ಆಯ್ಕೆ (*p++)
	{
	    // Note: Keep this in sync with gui_mch_expand_font().
	    ಸ್ಥಿತಿ L'h':
		lf->lf_ಎತ್ತರ = - points_ಗಮ್ಯ_pixels(p, &p, TRUE, (long_i)printer_dc);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ L'w':
		lf->lf_ಅಗಲ = points_ಗಮ್ಯ_pixels(p, &p, FALSE, (long_i)printer_dc);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ L'W':
		lf->lf_ತೂಕ = wcstol(p, &p, 10);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ L'b':
		lf->lf_ತೂಕ = FW_BOLD;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ L'i':
		lf->lfItalic = TRUE;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ L'u':
		lf->lfUnderline = TRUE;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ L's':
		lf->lf_Strike_ಹೊರಹೋಗು = TRUE;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ L'c':
		ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(charset_ಜೋಡಿಗಳು); ++i)
		{
		    ಆದರೆ (utf16ascncmp(p, (ಅಕ್ಷರ *)charset_ಜೋಡಿಗಳು[i].name.string,
			    charset_ಜೋಡಿಗಳು[i].name.ಉದ್ದಳತೆ) == 0)
		    {
			lf->lf_Char_ಗಣ = charset_ಜೋಡಿಗಳು[i].charset;
			p += charset_ಜೋಡಿಗಳು[i].name.ಉದ್ದಳತೆ;
			ನಿಲ್ಲಿಸು;
		    }
		}

		ಆದರೆ (i == (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(charset_ಜೋಡಿಗಳು) && verbose)
		{
		    char_u *s = utf16_ಗಮ್ಯ_enc(p, ಖಾಲಿ);
		    ಆದರೆ (s != ಖಾಲಿ)
		    {
			semsg(_(e_illegal_str_name_str_ಒಳ_font_name_str),
							   "charset", s, name);
			vim_free(s);
		    }
		}
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ L'f':
		// Font features (e.g., "fss19=1").
		// Parsed separately by gui_mch_init_font(); skip here.
		ಆಗ (*p && *p != L':')
		    p++;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ L'q':
		ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(quality_ಜೋಡಿಗಳು); ++i)
		{
		    ಆದರೆ (utf16ascncmp(p, (ಅಕ್ಷರ *)quality_ಜೋಡಿಗಳು[i].name.string,
			    quality_ಜೋಡಿಗಳು[i].name.ಉದ್ದಳತೆ) == 0)
		    {
			lf->lfQuality = quality_ಜೋಡಿಗಳು[i].quality;
			p += quality_ಜೋಡಿಗಳು[i].name.ಉದ್ದಳತೆ;
			ನಿಲ್ಲಿಸು;
		    }
		}

		ಆದರೆ (i == (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(quality_ಜೋಡಿಗಳು) && verbose)
		{
		    char_u *s = utf16_ಗಮ್ಯ_enc(p, ಖಾಲಿ);
		    ಆದರೆ (s != ಖಾಲಿ)
		    {
			semsg(_(e_illegal_str_name_str_ಒಳ_font_name_str),
							   "quality", s, name);
			vim_free(s);
		    }
		}
		ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ:
		ಆದರೆ (verbose)
		    semsg(_(e_illegal_char_nr_ಒಳ_font_name_str), p[-1], name);
		ಹೋಗು theend;
	}
	ಆಗ (*p == L':')
	    p++;
    }
    ret = OK;

theend:
    // ron: init lastlf
    ಆದರೆ (ret == OK && printer_dc == ಖಾಲಿ)
    {
	vim_free(lastlf);
	lastlf = ALLOC_ONE(LOGFONTW);
	ಆದರೆ (lastlf != ಖಾಲಿ)
	    mch_memmove(lastlf, lf, ಗಾತ್ರ(LOGFONTW));
    }
    vim_free(wname);

    ಹಿಂತಿರುಗಿ ret;
}

#ಆದರೆ_ಅಂತ್ಯ // defined(FEAT_GUI) || defined(FEAT_PRINTER)

#ಆದರೆ defined(FEAT_ಕೆಲಸ_CHANNEL)
/*
 * Initialize the Winsock dll.
 */
    ಶೂನ್ಯ
channel_init_winsock(ಶೂನ್ಯ)
{
    WSADATA wsa_ದತ್ತಾಂಶ;
    ಪೂರ್ಣಾಂಕ wsaerr;

    ಆದರೆ (WSInitialized)
	ಹಿಂತಿರುಗಿ;

    wsaerr = WSAStartup(MAKEWORD(2, 2), &wsa_ದತ್ತಾಂಶ);
    ಆದರೆ (wsaerr == 0)
	WSInitialized = TRUE;
}
#ಆದರೆ_ಅಂತ್ಯ
