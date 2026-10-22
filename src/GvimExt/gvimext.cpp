/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	gvimext by Tianmiao Hu
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * gvimext is a DLL which is used for the "Edit with Vim" context menu
 * extension.  It implements a MS defined interface with the Shell.
 *
 * If you have any questions or any suggestions concerning gvimext, please
 * contact Tianmiao Hu: tianmiao@acm.org.
 */

#ಅನ್ತರ್ಭಾವ "gvimext.h"
#ಅನ್ತರ್ಭಾವ "../version.h"

#ವ್ಯಾಖ್ಯಾನಿಸು ARRAY_ಉದ್ದಳತೆ(a) (ಗಾತ್ರ(a) / ಗಾತ್ರ((a)[0]))

ಸ್ಥಿರ ಅಕ್ಷರ *searchpath(ಅಕ್ಷರ *name);

// Always get an error while putting the following stuff to the
// gvimext.h file as class protected variables, give up and
// declare them as global stuff
FORMATETC fmte = {CF_HDROP,
		  (DVTARGETDEVICE FAR *)ಖಾಲಿ,
		  DVASPECT_CONTENT,
		  -1,
		  TYMED_HGLOBAL
		 };
STGMEDIUM medium;
HRESULT hres = 0;
UINT cbFiles = 0;

/* The buffers size used to be MAX_PATH (260 bytes), but that's not always
 * enough */
#ವ್ಯಾಖ್ಯಾನಿಸು BUFSIZE 1100

// The "Edit with Vim" shell extension provides these choices when
// a new instance of Gvim is selected:
//   - use tabpages
//   - enable diff mode
//   - none of the above
#ವ್ಯಾಖ್ಯಾನಿಸು EDIT_WITH_VIM_USE_TABPAGES (2)
#ವ್ಯಾಖ್ಯಾನಿಸು EDIT_WITH_VIM_ಒಳ_DIFF_ಬಹುಲಕ (1)
#ವ್ಯಾಖ್ಯಾನಿಸು EDIT_WITH_VIM_NO_OPTIONS   (0)

//
// Get the name of the Gvim executable to use, with the path.
// When "runtime" is non-zero, we were called to find the runtime directory.
// Returns the path in name[BUFSIZE].  It's empty when it fails.
//
    ಸ್ಥಿರ ಶೂನ್ಯ
getGvimName(ಅಕ್ಷರ *name, BOOL runtime)
{
    HKEY	keyhandle;
    DWORD	hlen;

    // Get the location of gvim from the registry.  Try
    // HKEY_CURRENT_USER first, then fall back to HKEY_LOCAL_MACHINE if
    // not found.
    name[0] = 0;
    ಆದರೆ (Reg_Open_ಕೀಲಿ_Ex(HKEY_CURRENT_USER, "Software\\Vim\\Gvim", 0,
				       ಕೀಲಿ_READ, &keyhandle) == ERROR_SUCCESS)
    {
	hlen = BUFSIZE;
	ಆದರೆ (Reg_Query_ಮೌಲ್ಯ_Ex(keyhandle, "path", 0, ಖಾಲಿ, (BYTE *)name, &hlen)
							     != ERROR_SUCCESS)
	    name[0] = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	    name[hlen] = 0;
	Reg_Close_ಕೀಲಿ(keyhandle);
    }

    ಆದರೆ ((name[0] == 0) &&
	    (Reg_Open_ಕೀಲಿ_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
				       ಕೀಲಿ_READ, &keyhandle) == ERROR_SUCCESS))
    {
	hlen = BUFSIZE;
	ಆದರೆ (Reg_Query_ಮೌಲ್ಯ_Ex(keyhandle, "path", 0, ಖಾಲಿ, (BYTE *)name, &hlen)
							     != ERROR_SUCCESS)
	    name[0] = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	    name[hlen] = 0;
	Reg_Close_ಕೀಲಿ(keyhandle);
    }

    // Registry didn't work, use the search path.
    ಆದರೆ (name[0] == 0)
	strcpy(name, searchpath((ಅಕ್ಷರ *)"gvim.exe"));

    ಆದರೆ (!runtime)
    {
	// Only when looking for the executable, not the runtime dir, we can
	// search for the batch file or a name without a path.
	ಆದರೆ (name[0] == 0)
	    strcpy(name, searchpath((ಅಕ್ಷರ *)"gvim.bat"));
	ಆದರೆ (name[0] == 0)
	    strcpy(name, "gvim");	// finds gvim.bat or gvim.exe
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
getGvimInvocation(ಅಕ್ಷರ *name)
{
    getGvimName(name, FALSE);
    // avoid that Vim tries to expand wildcards in the file names
    strcat(name, " --literal");
}

    ಸ್ಥಿರ ಶೂನ್ಯ
getGvimInvocationW(wchar_t *nameW)
{
    ಅಕ್ಷರ *name;

    name = (ಅಕ್ಷರ *)ಸ್ಮರಣೆ_ಕೊಡು(BUFSIZE);
    getGvimInvocation(name);
    mbstowcs(nameW, name, BUFSIZE);
    ಬಿಡುಗಡೆ(name);
}

    ಸ್ಥಿರ BOOL
FileExists(LPCTSTR sz_ಮಾರ್ಗ)
{
    DWORD dwAttrib = Get_File_ಗುಣಲಕ್ಷಣಗಳು(sz_ಮಾರ್ಗ);

    ಹಿಂತಿರುಗಿ (dwAttrib != INVALID_FILE_ಗುಣಲಕ್ಷಣಗಳು
	    && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

//
// Get the Vim runtime directory into buf[BUFSIZE].
// The result is empty when it failed.
// When it works, the path ends in a slash or backslash.
//
    ಸ್ಥಿರ ಶೂನ್ಯ
getRuntimeDir(ಅಕ್ಷರ *buf)
{
    ಪೂರ್ಣಾಂಕ		idx;

    getGvimName(buf, TRUE);
    ಆದರೆ (buf[0] == 0)
	ಹಿಂತಿರುಗಿ;

    // When no path found, use the search path to expand it.
    ಆದರೆ (strchr(buf, '/') == ಖಾಲಿ && strchr(buf, '\\') == ಖಾಲಿ)
	strcpy(buf, searchpath(buf));

    // remove "gvim.exe" from the end
    ಗಾಗಿ (idx = (ಪೂರ್ಣಾಂಕ)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(buf) - 1; idx >= 0; idx--)
	ಆದರೆ (buf[idx] == '\\' || buf[idx] == '/')
	{
	    buf[++idx] = 0;
	    ನಿಲ್ಲಿಸು;
	}

    // Check the existence of defaults.vim
    ಅಕ್ಷರ *p = buf + idx;
    strcpy(p, "defaults.vim");
    ಆದರೆ (FileExists(buf))
    {
	*p = 0;		// Cut "defaults.vim"
	ಹಿಂತಿರುಗಿ;
    }
    // Check the existence of vimXX\defaults.vim
    strcpy(p, VIM_ಆವೃತ್ತಿ_NODOT "\\" "defaults.vim");
    ಆದರೆ (FileExists(buf))
    {
	p[ARRAY_ಉದ್ದಳತೆ(VIM_ಆವೃತ್ತಿ_NODOT)] = 0;	// Cut "defaults.vim"
	ಹಿಂತಿರುಗಿ;
    }
    // Check the existence of runtime\defaults.vim
    strcpy(p, "runtime" "\\" "defaults.vim");
    ಆದರೆ (FileExists(buf))
    {
	p[ARRAY_ಉದ್ದಳತೆ("runtime")] = 0;		// Cut "defaults.vim"
	ಹಿಂತಿರುಗಿ;
    }
    // Not found
    buf[0] = 0;
    ಹಿಂತಿರುಗಿ;
}

    WCHAR *
utf8_ಗಮ್ಯ_utf16(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s)
{
    ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2 = Multi_Byte_ಗಮ್ಯ_Wide_Char(CP_UTF8, 0, s, -1, ಖಾಲಿ, 0);
    WCHAR *buf = (WCHAR *)ಸ್ಮರಣೆ_ಕೊಡು(ಗಾತ್ರ_2 * ಗಾತ್ರ(WCHAR));
    Multi_Byte_ಗಮ್ಯ_Wide_Char(CP_UTF8, 0, s, -1, buf, ಗಾತ್ರ_2);
    ಹಿಂತಿರುಗಿ buf;
}

    HBITMAP
Icon_ಗಮ್ಯ_Bitmap(HICON hIcon, HBRUSH hBackground, ಪೂರ್ಣಾಂಕ ಅಗಲ, ಪೂರ್ಣಾಂಕ ಎತ್ತರ)
{
    HDC hDC = GetDC(ಖಾಲಿ);
    HDC hMemDC = CreateCompatibleDC(hDC);
    HBITMAP hMemBmp = CreateCompatibleBitmap(hDC, ಅಗಲ, ಎತ್ತರ);
    HBITMAP h_ಫಲಿತಾಂಶ_Bmp = ಖಾಲಿ;
    HGDIOBJ hOrgBMP = SelectObject(hMemDC, hMemBmp);

    DrawIconEx(hMemDC, 0, 0, hIcon, ಅಗಲ, ಎತ್ತರ, 0, hBackground, DI_ಸಾಮಾನ್ಯ);

    h_ಫಲಿತಾಂಶ_Bmp = hMemBmp;
    hMemBmp = ಖಾಲಿ;

    SelectObject(hMemDC, hOrgBMP);
    DeleteDC(hMemDC);
    ReleaseDC(ಖಾಲಿ, hDC);
    DestroyIcon(hIcon);
    ಹಿಂತಿರುಗಿ h_ಫಲಿತಾಂಶ_Bmp;
}

//
// GETTEXT: translated messages and menu entries
//
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_GETTEXT
# define _(x)	    x
# define W_impl(x) _wcsdup(L##x)
# define W(x)	    W_impl(x)
# define ಗಣ_gettext_codeset()	    ಖಾಲಿ
# define restore_gettext_codeset(x)
#ಇಲ್ಲದಿದ್ದರೆ
# define _(x)	    (*dyn_libintl_gettext)(x)
# define W(x)	    utf8_ಗಮ್ಯ_utf16(x)
# define VIMPACKAGE "vim"
# ifndef GETTEXT_DLL
#  define GETTEXT_DLL "libintl.dll"
#  define GETTEXT_DLL_ALT "libintl-8.dll"
# endif

// Dummy functions
ಸ್ಥಿರ ಅಕ್ಷರ *null_libintl_gettext(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
ಸ್ಥಿರ ಅಕ್ಷರ *null_libintl_textdomain(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
ಸ್ಥಿರ ಅಕ್ಷರ *null_libintl_bindtextdomain(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
ಸ್ಥಿರ ಅಕ್ಷರ *null_libintl_bind_textdomain_codeset(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ dyn_libintl_init(ಅಕ್ಷರ *dir);
ಸ್ಥಿರ ಶೂನ್ಯ dyn_libintl_ಕೊನೆ(ಶೂನ್ಯ);

ಸ್ಥಿರ HINSTANCE hLibintlDLL = 0;
ಸ್ಥಿರ ಅಕ್ಷರ *(*dyn_libintl_gettext)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *) = null_libintl_gettext;
ಸ್ಥಿರ ಅಕ್ಷರ *(*dyn_libintl_textdomain)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *) = null_libintl_textdomain;
ಸ್ಥಿರ ಅಕ್ಷರ *(*dyn_libintl_bindtextdomain)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)
						= null_libintl_bindtextdomain;
ಸ್ಥಿರ ಅಕ್ಷರ *(*dyn_libintl_bind_textdomain_codeset)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)
				       = null_libintl_bind_textdomain_codeset;

//
// Attempt to load libintl.dll.  If it doesn't work, use dummy functions.
// "dir" is the directory where the libintl.dll might be.
// Return 1 for success, 0 for failure.
//
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
dyn_libintl_init(ಅಕ್ಷರ *dir)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಸ್ಥಿರ ರಚನೆ
    {
	ಅಕ್ಷರ	    *name;
	FARPROC	    *ptr;
    } libintl_ನಮೂದು[] =
    {
	{(ಅಕ್ಷರ *)"gettext",		(FARPROC*)&dyn_libintl_gettext},
	{(ಅಕ್ಷರ *)"textdomain",		(FARPROC*)&dyn_libintl_textdomain},
	{(ಅಕ್ಷರ *)"bindtextdomain",	(FARPROC*)&dyn_libintl_bindtextdomain},
	{(ಅಕ್ಷರ *)"bind_textdomain_codeset", (FARPROC*)&dyn_libintl_bind_textdomain_codeset},
	{ಖಾಲಿ, ಖಾಲಿ}
    };
    DWORD	len, len2;
    LPWSTR	buf = ಖಾಲಿ;
    LPWSTR	buf2 = ಖಾಲಿ;

    // No need to initialize twice.
    ಆದರೆ (hLibintlDLL)
	ಹಿಂತಿರುಗಿ 1;

    // Load gettext library from $VIMRUNTIME\GvimExt{64,32} directory.
    // Add the directory to $PATH temporarily.
    len = GetEnvironmentVariableW(L"PATH", ಖಾಲಿ, 0);
    len2 = MAX_ಮಾರ್ಗ + 1 + len;
    buf = (LPWSTR)ಸ್ಮರಣೆ_ಕೊಡು(len * ಗಾತ್ರ(WCHAR));
    buf2 = (LPWSTR)ಸ್ಮರಣೆ_ಕೊಡು(len2 * ಗಾತ್ರ(WCHAR));
    ಆದರೆ (buf != ಖಾಲಿ && buf2 != ಖಾಲಿ)
    {
	GetEnvironmentVariableW(L"PATH", buf, len);
# ifdef _WIN64
	_snwprintf(buf2, len2, L"%S\\GvimExt64;%s", dir, buf);
# ಇಲ್ಲದಿದ್ದರೆ
	_snwprintf(buf2, len2, L"%S\\GvimExt32;%s", dir, buf);
# endif
	ಗಣ_Environment_Variable_W(L"PATH", buf2);
	hLibintlDLL = ತುಂಬು_Library(GETTEXT_DLL);
# ifdef GETTEXT_DLL_ALT
	ಆದರೆ (!hLibintlDLL)
	    hLibintlDLL = ತುಂಬು_Library(GETTEXT_DLL_ALT);
# endif
	ಗಣ_Environment_Variable_W(L"PATH", buf);
    }
    ಬಿಡುಗಡೆ(buf);
    ಬಿಡುಗಡೆ(buf2);
    ಆದರೆ (!hLibintlDLL)
	ಹಿಂತಿರುಗಿ 0;

    // Get the addresses of the functions we need.
    ಗಾಗಿ (i = 0; libintl_ನಮೂದು[i].name != ಖಾಲಿ
					 && libintl_ನಮೂದು[i].ptr != ಖಾಲಿ; ++i)
    {
	ಆದರೆ ((*libintl_ನಮೂದು[i].ptr = Get_Proc_ವಿಳಾಸ(hLibintlDLL,
					      libintl_ನಮೂದು[i].name)) == ಖಾಲಿ)
	{
	    dyn_libintl_ಕೊನೆ();
	    ಹಿಂತಿರುಗಿ 0;
	}
    }
    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
dyn_libintl_ಕೊನೆ(ಶೂನ್ಯ)
{
    ಆದರೆ (hLibintlDLL)
	FreeLibrary(hLibintlDLL);
    hLibintlDLL			= ಖಾಲಿ;
    dyn_libintl_gettext		= null_libintl_gettext;
    dyn_libintl_textdomain	= null_libintl_textdomain;
    dyn_libintl_bindtextdomain	= null_libintl_bindtextdomain;
    dyn_libintl_bind_textdomain_codeset	= null_libintl_bind_textdomain_codeset;
}

    ಸ್ಥಿರ ಅಕ್ಷರ *
null_libintl_gettext(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *msgid)
{
    ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ *)msgid;
}

    ಸ್ಥಿರ ಅಕ್ಷರ *
null_libintl_textdomain(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * /* domainname */)
{
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಸ್ಥಿರ ಅಕ್ಷರ *
null_libintl_bindtextdomain(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * /* domainname */, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * /* dirname */)
{
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಸ್ಥಿರ ಅಕ್ಷರ *
null_libintl_bind_textdomain_codeset(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * /* domainname */, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * /* codeset */)
{
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

//
// Setup for translating strings.
//
    ಸ್ಥಿರ ಶೂನ್ಯ
dyn_gettext_ತುಂಬು(ಶೂನ್ಯ)
{
    ಅಕ್ಷರ    szBuff[BUFSIZE];
    DWORD   len;

    // Try to locate the runtime files.  The path is used to find libintl.dll
    // and the vim.mo files.
    getRuntimeDir(szBuff);
    ಆದರೆ (szBuff[0] != 0)
    {
	len = (DWORD)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(szBuff);
	ಆದರೆ (dyn_libintl_init(szBuff))
	{
	    strcpy(szBuff + len, "lang");

	    (*dyn_libintl_bindtextdomain)(VIMPACKAGE, szBuff);
	    (*dyn_libintl_textdomain)(VIMPACKAGE);
	}
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
dyn_gettext_free(ಶೂನ್ಯ)
{
    dyn_libintl_ಕೊನೆ();
}

//
// Use UTF-8 for gettext. Returns previous codeset.
//
    ಸ್ಥಿರ ಅಕ್ಷರ *
ಗಣ_gettext_codeset(ಶೂನ್ಯ)
{
    ಅಕ್ಷರ *prev = dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, ಖಾಲಿ);
    prev = _strdup((prev != ಖಾಲಿ) ? prev : "char");
    dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, "utf-8");

    ಹಿಂತಿರುಗಿ prev;
}

//
// Restore previous codeset for gettext.
//
    ಸ್ಥಿರ ಶೂನ್ಯ
restore_gettext_codeset(ಅಕ್ಷರ *prev)
{
    dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, prev);
    ಬಿಡುಗಡೆ(prev);
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_GETTEXT

//
// Global variables
//
UINT      g_cRefThisDll = 0;    // Reference count of this DLL.
HINSTANCE g_hmodThisDll = ಖಾಲಿ;	// Handle to this DLL itself.


//---------------------------------------------------------------------------
// DllMain
//---------------------------------------------------------------------------
ಬಾಹ್ಯ "C" ಪೂರ್ಣಾಂಕ APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID  /* lpReserved */)
{
    ಆಯ್ಕೆ (dwReason)
    {
    ಸ್ಥಿತಿ DLL_PROCESS_ATTACH:
	// Extension DLL one-time initialization
	g_hmodThisDll = hInstance;
	ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ DLL_PROCESS_DETACH:
	ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ 1;   // ok
}

    ಸ್ಥಿರ ಶೂನ್ಯ
inc_cRefThisDLL()
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GETTEXT
    ಆದರೆ (g_cRefThisDll == 0)
	dyn_gettext_ತುಂಬು();
#ಆದರೆ_ಅಂತ್ಯ
    InterlockedIncrement((LPLONG)&g_cRefThisDll);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
dec_cRefThisDLL()
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GETTEXT
    ಆದರೆ (InterlockedDecrement((LPLONG)&g_cRefThisDll) == 0)
	dyn_gettext_free();
#ಇಲ್ಲದಿದ್ದರೆ
    InterlockedDecrement((LPLONG)&g_cRefThisDll);
#ಆದರೆ_ಅಂತ್ಯ
}

//---------------------------------------------------------------------------
// DllCanUnloadNow
//---------------------------------------------------------------------------

STDAPI DllCanUnloadNow(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (g_cRefThisDll == 0 ? S_OK : S_FALSE);
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppv_ಹೊರಹೋಗು)
{
    *ppv_ಹೊರಹೋಗು = ಖಾಲಿ;

    ಆದರೆ (IsEqualIID(rclsid, CLSID_ShellExtension))
    {
	CShellExtClassFactory *pcf = ಹೊಸ CShellExtClassFactory;

	ಹಿಂತಿರುಗಿ pcf->QueryInterface(riid, ppv_ಹೊರಹೋಗು);
    }

    ಹಿಂತಿರುಗಿ CLASS_E_CLASSNOTAVAILABLE;
}

CShellExtClassFactory::CShellExtClassFactory()
{
    m_cRef = 0L;

    inc_cRefThisDLL();
}

CShellExtClassFactory::~CShellExtClassFactory()
{
    dec_cRefThisDLL();
}

STDMETHODIMP CShellExtClassFactory::QueryInterface(REFIID riid,
						   LPVOID FAR *ppv)
{
    *ppv = ಖಾಲಿ;

    // any interface on this object is the object pointer

    ಆದರೆ (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	*ppv = (LPCLASSFACTORY)ಇದು;

	ಕೂಡಿಸು_Ref();

	ಹಿಂತಿರುಗಿ NOERROR;
    }

    ಹಿಂತಿರುಗಿ E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::ಕೂಡಿಸು_Ref()
{
    ಹಿಂತಿರುಗಿ InterlockedIncrement((LPLONG)&m_cRef);
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::Release()
{
    ಆದರೆ (InterlockedDecrement((LPLONG)&m_cRef))
	ಹಿಂತಿರುಗಿ m_cRef;

    ಅಳಿಸು ಇದು;

    ಹಿಂತಿರುಗಿ 0L;
}

STDMETHODIMP CShellExtClassFactory::CreateInstance(LPUNKNOWN pUnkOuter,
						      REFIID riid,
						      LPVOID *ppvObj)
{
    *ppvObj = ಖಾಲಿ;

    // Shell extensions typically don't support aggregation (inheritance)

    ಆದರೆ (pUnkOuter)
	ಹಿಂತಿರುಗಿ CLASS_E_NOAGGREGATION;

    // Create the main shell extension object.  The shell will then call
    // QueryInterface with IID_IShellExtInit--this is how shell extensions are
    // initialized.

    LPCSHELLEXT pShellExt = ಹೊಸ CShellExt();  // create the CShellExt object

    ಆದರೆ (ಖಾಲಿ == pShellExt)
	ಹಿಂತಿರುಗಿ E_OUTOFMEMORY;

    ಹಿಂತಿರುಗಿ pShellExt->QueryInterface(riid, ppvObj);
}


STDMETHODIMP CShellExtClassFactory::ಬೀಗ_Server(BOOL  /* fLock */)
{
    ಹಿಂತಿರುಗಿ NOERROR;
}

// *********************** CShellExt *************************
CShellExt::CShellExt()
{
    m_cRef = 0L;
    m_p_ದತ್ತಾಂಶ_Obj = ಖಾಲಿ;

    inc_cRefThisDLL();

    ತುಂಬು_Menu_Icon();
}

CShellExt::~CShellExt()
{
    ಆದರೆ (m_p_ದತ್ತಾಂಶ_Obj)
	m_p_ದತ್ತಾಂಶ_Obj->Release();

    dec_cRefThisDLL();

    ಆದರೆ (m_hVimIconBitmap)
	DeleteObject(m_hVimIconBitmap);
}

STDMETHODIMP CShellExt::QueryInterface(REFIID riid, LPVOID FAR *ppv)
{
    *ppv = ಖಾಲಿ;

    ಆದರೆ (IsEqualIID(riid, IID_IShellExtInit) || IsEqualIID(riid, IID_IUnknown))
    {
	*ppv = (LPSHELLEXTINIT)ಇದು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (IsEqualIID(riid, IID_IContextMenu))
    {
	*ppv = (LPCONTEXTMENU)ಇದು;
    }

    ಆದರೆ (*ppv)
    {
	ಕೂಡಿಸು_Ref();

	ಹಿಂತಿರುಗಿ NOERROR;
    }

    ಹಿಂತಿರುಗಿ E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExt::ಕೂಡಿಸು_Ref()
{
    ಹಿಂತಿರುಗಿ InterlockedIncrement((LPLONG)&m_cRef);
}

STDMETHODIMP_(ULONG) CShellExt::Release()
{

    ಆದರೆ (InterlockedDecrement((LPLONG)&m_cRef))
	ಹಿಂತಿರುಗಿ m_cRef;

    ಅಳಿಸು ಇದು;

    ಹಿಂತಿರುಗಿ 0L;
}


//
//  FUNCTION: CShellExt::Initialize(LPCITEMIDLIST, LPDATAOBJECT, HKEY)
//
//  PURPOSE: Called by the shell when initializing a context menu or property
//	     sheet extension.
//
//  PARAMETERS:
//    pIDFolder - Specifies the parent folder
//    pDataObj  - Specifies the set of items selected in that folder.
//    hRegKey   - Specifies the type of the focused item in the selection.
//
//  RETURN VALUE:
//
//    NOERROR in all cases.
//
//  COMMENTS:   Note that at the time this function is called, we don't know
//		(or care) what type of shell extension is being initialized.
//		It could be a context menu or a property sheet.
//

STDMETHODIMP CShellExt::Initialize(LPCITEMIDLIST  /* pIDFolder */,
				   LPDATAOBJECT p_ದತ್ತಾಂಶ_Obj,
				   HKEY  /* hRegKey */)
{
    // Initialize can be called more than once
    ಆದರೆ (m_p_ದತ್ತಾಂಶ_Obj)
	m_p_ದತ್ತಾಂಶ_Obj->Release();

    // duplicate the object pointer and registry handle

    ಆದರೆ (p_ದತ್ತಾಂಶ_Obj)
    {
	m_p_ದತ್ತಾಂಶ_Obj = p_ದತ್ತಾಂಶ_Obj;
	p_ದತ್ತಾಂಶ_Obj->ಕೂಡಿಸು_Ref();
    }

    ಹಿಂತಿರುಗಿ NOERROR;
}


//
//  FUNCTION: CShellExt::QueryContextMenu(HMENU, UINT, UINT, UINT, UINT)
//
//  PURPOSE: Called by the shell just before the context menu is displayed.
//	     This is where you add your specific menu items.
//
//  PARAMETERS:
//    hMenu      - Handle to the context menu
//    indexMenu  - Index of where to begin inserting menu items
//    idCmdFirst - Lowest value for new menu ID's
//    idCmtLast  - Highest value for new menu ID's
//    uFlags     - Specifies the context of the menu event
//
//  RETURN VALUE:
//
//
//  COMMENTS:
//

STDMETHODIMP CShellExt::QueryContextMenu(HMENU hMenu,
					 UINT ಸೂಚ್ಯಂಕ_Menu,
					 UINT id_Cmd_ಮೊದಲ,
					 UINT  /* idCmdLast */,
					 UINT  /* uFlags */)
{
    UINT idCmd = id_Cmd_ಮೊದಲ;

    hres = m_p_ದತ್ತಾಂಶ_Obj->Get_ದತ್ತಾಂಶ(&fmte, &medium);
    ಆದರೆ (medium.hGlobal)
	cbFiles = DragQueryFileW((HDROP)medium.hGlobal, (UINT)-1, 0, 0);

    // InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

    // Initialize m_cntOfHWnd to 0
    m_cnt_ದ_HWnd = 0;

    HKEY keyhandle;
    ತಾರ್ಕಿಕ showExisting = ನಿಜ;
    ತಾರ್ಕಿಕ showIcons = ನಿಜ;

    // Check whether "Edit with existing Vim" entries are disabled.
    ಆದರೆ (Reg_Open_ಕೀಲಿ_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
				       ಕೀಲಿ_READ, &keyhandle) == ERROR_SUCCESS)
    {
	ಆದರೆ (Reg_Query_ಮೌಲ್ಯ_Ex(keyhandle, "DisableEditWithExisting", 0, ಖಾಲಿ,
						 ಖಾಲಿ, ಖಾಲಿ) == ERROR_SUCCESS)
	    showExisting = ಸುಳ್ಳು;
	ಆದರೆ (Reg_Query_ಮೌಲ್ಯ_Ex(keyhandle, "DisableContextMenuIcons", 0, ಖಾಲಿ,
						 ಖಾಲಿ, ಖಾಲಿ) == ERROR_SUCCESS)
	    showIcons = ಸುಳ್ಳು;
	Reg_Close_ಕೀಲಿ(keyhandle);
    }

    // Use UTF-8 for gettext.
    ಅಕ್ಷರ *prev = ಗಣ_gettext_codeset();

    // Retrieve all the vim instances, unless disabled.
    ಆದರೆ (showExisting)
	EnumWindows(EnumWindowsProc, (LPARAM)ಇದು);

    MENUITEMINFOW mii = { ಗಾತ್ರ(MENUITEMINFOW) };
    mii.f_ಮುಖವಾಡ = MIIM_STRING | MIIM_ID;
    ಆದರೆ (showIcons)
    {
	mii.f_ಮುಖವಾಡ |= MIIM_BITMAP;
	mii.hbmp_ಅಂಶ = m_hVimIconBitmap;
    }

    ಆದರೆ (cbFiles > 1)
    {
	mii.wID = idCmd++;
	mii.dw_Type_ದತ್ತಾಂಶ = W(_("Edit with Vim using &tabpages"));
	mii.cch = wcslen(mii.dw_Type_ದತ್ತಾಂಶ);
	ಸೇರಿಸು_Menu_ಅಂಶ_W(hMenu, ಸೂಚ್ಯಂಕ_Menu++, TRUE, &mii);
	ಬಿಡುಗಡೆ(mii.dw_Type_ದತ್ತಾಂಶ);

	mii.wID = idCmd++;
	mii.dw_Type_ದತ್ತಾಂಶ = W(_("Edit with single &Vim"));
	mii.cch = wcslen(mii.dw_Type_ದತ್ತಾಂಶ);
	ಸೇರಿಸು_Menu_ಅಂಶ_W(hMenu, ಸೂಚ್ಯಂಕ_Menu++, TRUE, &mii);
	ಬಿಡುಗಡೆ(mii.dw_Type_ದತ್ತಾಂಶ);

	ಆದರೆ (cbFiles <= 4)
	{
	    // Can edit up to 4 files in diff mode
	    mii.wID = idCmd++;
	    mii.dw_Type_ದತ್ತಾಂಶ = W(_("Diff with Vim"));
	    mii.cch = wcslen(mii.dw_Type_ದತ್ತಾಂಶ);
	    ಸೇರಿಸು_Menu_ಅಂಶ_W(hMenu, ಸೂಚ್ಯಂಕ_Menu++, TRUE, &mii);
	    ಬಿಡುಗಡೆ(mii.dw_Type_ದತ್ತಾಂಶ);
	    m_edit_existing_off = 3;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    m_edit_existing_off = 2;

    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	mii.wID = idCmd++;
	mii.dw_Type_ದತ್ತಾಂಶ = W(_("Edit with &Vim"));
	mii.cch = wcslen(mii.dw_Type_ದತ್ತಾಂಶ);
	ಸೇರಿಸು_Menu_ಅಂಶ_W(hMenu, ಸೂಚ್ಯಂಕ_Menu++, TRUE, &mii);
	ಬಿಡುಗಡೆ(mii.dw_Type_ದತ್ತಾಂಶ);
	m_edit_existing_off = 1;
    }

    HMENU hSubMenu = ಖಾಲಿ;
    ಆದರೆ (m_cnt_ದ_HWnd > 1)
    {
	hSubMenu = CreatePopupMenu();
	mii.f_ಮುಖವಾಡ |= MIIM_SUBMENU;
	mii.wID = idCmd;
	mii.dw_Type_ದತ್ತಾಂಶ = W(_("Edit with existing Vim"));
	mii.cch = wcslen(mii.dw_Type_ದತ್ತಾಂಶ);
	mii.hSubMenu = hSubMenu;
	ಸೇರಿಸು_Menu_ಅಂಶ_W(hMenu, ಸೂಚ್ಯಂಕ_Menu++, TRUE, &mii);
	ಬಿಡುಗಡೆ(mii.dw_Type_ದತ್ತಾಂಶ);
	mii.f_ಮುಖವಾಡ = mii.f_ಮುಖವಾಡ & ~MIIM_SUBMENU;
	mii.hSubMenu = ಖಾಲಿ;
    }
    // Now display all the vim instances
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < m_cnt_ದ_HWnd; i++)
    {
	WCHAR title[BUFSIZE];
	WCHAR ತಾತ್ಕಾಲಿಕ[BUFSIZE];
	ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ;
	HMENU hmenu;

	// Obtain window title, continue if can not
	ಆದರೆ (Get_ಕಿಟಕಿ_ಪಠ್ಯ_W(m_hWnd[i], title, BUFSIZE - 1) == 0)
	    ಮುಂದುವರಿ;
	// Truncate the title before the path, keep the file name
	WCHAR *pos = wcschr(title, L'(');
	ಆದರೆ (pos != ಖಾಲಿ)
	{
	    ಆದರೆ (pos > title && pos[-1] == L' ')
		--pos;
	    *pos = 0;
	}
	// Now concatenate
	ಆದರೆ (m_cnt_ದ_HWnd > 1)
	    ತಾತ್ಕಾಲಿಕ[0] = L'\0';
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    WCHAR *s = W(_("Edit with existing Vim - "));
	    wcsncpy(ತಾತ್ಕಾಲಿಕ, s, BUFSIZE - 1);
	    ತಾತ್ಕಾಲಿಕ[BUFSIZE - 1] = L'\0';
	    ಬಿಡುಗಡೆ(s);
	}
	wcsncat(ತಾತ್ಕಾಲಿಕ, title, BUFSIZE - 1 - wcslen(ತಾತ್ಕಾಲಿಕ));
	ತಾತ್ಕಾಲಿಕ[BUFSIZE - 1] = L'\0';

	mii.wID = idCmd++;
	mii.dw_Type_ದತ್ತಾಂಶ = ತಾತ್ಕಾಲಿಕ;
	mii.cch = wcslen(mii.dw_Type_ದತ್ತಾಂಶ);
	ಆದರೆ (m_cnt_ದ_HWnd > 1)
	{
	    hmenu = hSubMenu;
	    ಸೂಚ್ಯಂಕ = i;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    hmenu = hMenu;
	    ಸೂಚ್ಯಂಕ = ಸೂಚ್ಯಂಕ_Menu++;
	}
	ಸೇರಿಸು_Menu_ಅಂಶ_W(hmenu, ಸೂಚ್ಯಂಕ, TRUE, &mii);
    }
    // InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

    // Restore previous codeset.
    restore_gettext_codeset(prev);

    // Must return number of menu items we added.
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ_From_Short(idCmd-id_Cmd_ಮೊದಲ);
}

//
//  FUNCTION: CShellExt::InvokeCommand(LPCMINVOKECOMMANDINFO)
//
//  PURPOSE: Called by the shell after the user has selected on of the
//	     menu items that was added in QueryContextMenu().
//
//  PARAMETERS:
//    lpcmi - Pointer to an CMINVOKECOMMANDINFO structure
//
//  RETURN VALUE:
//
//
//  COMMENTS:
//

STDMETHODIMP CShellExt::InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi)
{
    HRESULT hr = E_INVALIDARG;
    ಪೂರ್ಣಾಂಕ gvimExtraOptions;

    // If HIWORD(lpcmi->lpVerb) then we have been called programmatically
    // and lpVerb is a command that should be invoked.  Otherwise, the shell
    // has called us, and LOWORD(lpcmi->lpVerb) is the menu ID the user has
    // selected.  Actually, it's (menu ID - idCmdFirst) from QueryContextMenu().
    ಆದರೆ (!HIWORD(lpcmi->lpVerb))
    {
	UINT idCmd = LOWORD(lpcmi->lpVerb);

	ಆದರೆ (idCmd >= m_edit_existing_off)
	{
	    // Existing with vim instance
	    hr = ತಳ್ಳು_ಗಮ್ಯ_ಕಿಟಕಿ(lpcmi->hwnd,
		    lpcmi->lpDirectory,
		    lpcmi->lpVerb,
		    lpcmi->lpParameters,
		    lpcmi->nShow,
		    idCmd - m_edit_existing_off);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆಯ್ಕೆ (idCmd)
	    {
		ಸ್ಥಿತಿ 0:
		    gvimExtraOptions = EDIT_WITH_VIM_USE_TABPAGES;
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 1:
		    gvimExtraOptions = EDIT_WITH_VIM_NO_OPTIONS;
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 2:
		    gvimExtraOptions = EDIT_WITH_VIM_ಒಳ_DIFF_ಬಹುಲಕ;
		    ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
		    // If execution reaches this point we likely have an
		    // inconsistency between the code that setup the menus
		    // and this code that determines what the user
		    // selected.  This should be detected and fixed during
		    // development.
		    ಹಿಂತಿರುಗಿ E_FAIL;
	    }

            LPCMINVOKECOMMANDINFOEX lpcmiex = (LPCMINVOKECOMMANDINFOEX)lpcmi;
            LPCWSTR currentDirectory = lpcmi->cb_ಗಾತ್ರ == ಗಾತ್ರ(CMINVOKECOMMANDINFOEX) ? lpcmiex->lpDirectoryW : ಖಾಲಿ;

	    hr = InvokeSingleGvim(lpcmi->hwnd,
		    currentDirectory,
		    lpcmi->lpVerb,
		    lpcmi->lpParameters,
		    lpcmi->nShow,
		    gvimExtraOptions);
	}
    }
    ಹಿಂತಿರುಗಿ hr;
}

STDMETHODIMP CShellExt::ತಳ್ಳು_ಗಮ್ಯ_ಕಿಟಕಿ(HWND  /* hParent */,
				   LPCSTR  /* pszWorkingDir */,
				   LPCSTR  /* pszCmd */,
				   LPCSTR  /* pszParam */,
				   ಪೂರ್ಣಾಂಕ  /* iShowCmd */,
				   ಪೂರ್ಣಾಂಕ idHWnd)
{
    HWND hWnd = m_hWnd[idHWnd];

    // Show and bring vim instance to foreground
    ಆದರೆ (IsIconic(hWnd) != 0)
	Show_ಕಿಟಕಿ(hWnd, SW_RESTORE);
    ಇಲ್ಲದಿದ್ದರೆ
	Show_ಕಿಟಕಿ(hWnd, SW_SHOW);
    ಗಣ_Foreground_ಕಿಟಕಿ(hWnd);

    // Post the selected files to the vim instance
    Post_ಸಂದೇಶ(hWnd, WM_DROPFILES, (WPARAM)medium.hGlobal, 0);

    ಹಿಂತಿರುಗಿ NOERROR;
}

STDMETHODIMP CShellExt::GetCommandString(UINT_PTR  /* idCmd */,
					 UINT uFlags,
					 UINT FAR * /* reserved */,
					 LPSTR pszName,
					 UINT cchMax)
{
    // Use UTF-8 for gettext.
    ಅಕ್ಷರ *prev = ಗಣ_gettext_codeset();

    WCHAR *s = W(_("Edits the selected file(s) with Vim"));
    ಆದರೆ (uFlags == GCS_HELPTEXTW && cchMax > wcslen(s))
	wcscpy((WCHAR *)pszName, s);
    ಬಿಡುಗಡೆ(s);

    // Restore previous codeset.
    restore_gettext_codeset(prev);

    ಹಿಂತಿರುಗಿ NOERROR;
}

BOOL CALLBACK CShellExt::EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    ಅಕ್ಷರ ತಾತ್ಕಾಲಿಕ[BUFSIZE];

    // First do a bunch of check
    // No invisible window
    ಆದರೆ (!Is_ಕಿಟಕಿ_Visible(hWnd))
	ಹಿಂತಿರುಗಿ TRUE;
    // No child window ???
    // if (GetParent(hWnd)) return TRUE;
    // Class name should be Vim, if failed to get class name, return
    ಆದರೆ (GetClassName(hWnd, ತಾತ್ಕಾಲಿಕ, ARRAY_ಉದ್ದಳತೆ(ತಾತ್ಕಾಲಿಕ)) == 0)
	ಹಿಂತಿರುಗಿ TRUE;
    // Compare class name to that of vim, if not, return
    ಆದರೆ (_strnicmp(ತಾತ್ಕಾಲಿಕ, "vim", ARRAY_ಉದ್ದಳತೆ("vim")) != 0)
	ಹಿಂತಿರುಗಿ TRUE;
    // First check if the number of vim instance exceeds MAX_HWND
    CShellExt *cs = (CShellExt*) lParam;
    ಆದರೆ (cs->m_cnt_ದ_HWnd >= MAX_HWND)
	ಹಿಂತಿರುಗಿ FALSE;	// stop enumeration
    // Now we get the vim window, put it into some kind of array
    cs->m_hWnd[cs->m_cnt_ದ_HWnd] = hWnd;
    cs->m_cnt_ದ_HWnd ++;

    ಹಿಂತಿರುಗಿ TRUE; // continue enumeration (otherwise this would be false)
}

BOOL CShellExt::ತುಂಬು_Menu_Icon()
{
    ಅಕ್ಷರ vimExeFile[BUFSIZE];
    getGvimName(vimExeFile, TRUE);
    ಆದರೆ (vimExeFile[0] == '\0')
	ಹಿಂತಿರುಗಿ FALSE;
    HICON hVimIcon;
    ಆದರೆ (ಹೊರತೆಗೆ_Icon_Ex(vimExeFile, 0, ಖಾಲಿ, &hVimIcon, 1) == 0)
	ಹಿಂತಿರುಗಿ FALSE;
    m_hVimIconBitmap = Icon_ಗಮ್ಯ_Bitmap(hVimIcon,
	    GetSysColorBrush(COLOR_MENU),
	    GetSystemMetrics(SM_CXSMICON),
	    GetSystemMetrics(SM_CYSMICON));
    ಹಿಂತಿರುಗಿ TRUE;
}

    ಸ್ಥಿರ ಅಕ್ಷರ *
searchpath(ಅಕ್ಷರ *name)
{
    ಸ್ಥಿರ ಒಕ್ಕೂಟ {
	ಅಕ್ಷರ	location[2 * BUFSIZE];
	WCHAR	wname[BUFSIZE];
    };
    WCHAR wlocation[BUFSIZE + 1];

    // There appears to be a bug in FindExecutableA() on Windows NT.
    // Use FindExecutableW() instead...
    Multi_Byte_ಗಮ್ಯ_Wide_Char(CP_ACP, 0, name, -1, wname, BUFSIZE);
    ಆದರೆ (ಕಂಡುಹಿಡಿ_Executable_W(wname, L"", wlocation) > (HINSTANCE)32)
    {
	Wide_Char_ಗಮ್ಯ_Multi_Byte(CP_ACP, 0, wlocation, -1,
		location, ಗಾತ್ರ(location), ಖಾಲಿ, ಖಾಲಿ);
	ಹಿಂತಿರುಗಿ location;
    }
    ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ *)"";
}


STDMETHODIMP CShellExt::InvokeSingleGvim(HWND h_ಪೋಷಕ,
				   LPCWSTR  workingDir,
				   LPCSTR  /* pszCmd */,
				   LPCSTR  /* pszParam */,
				   ಪೂರ್ಣಾಂಕ  /* iShowCmd */,
				   ಪೂರ್ಣಾಂಕ gvimExtraOptions)
{
    wchar_t	m_szFileUserClickedOn[BUFSIZE];
    wchar_t	*cmdStrW;
    size_t	cmdlen;
    size_t	len;
    UINT i;

    cmdlen = BUFSIZE;
    cmdStrW  = (wchar_t *) ಸ್ಮರಣೆ_ಕೊಡು(cmdlen * ಗಾತ್ರ(wchar_t));
    ಆದರೆ (cmdStrW == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ E_FAIL;
    getGvimInvocationW(cmdStrW);

    ಆದರೆ (gvimExtraOptions == EDIT_WITH_VIM_ಒಳ_DIFF_ಬಹುಲಕ)
	wcscat(cmdStrW, L" -d");
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (gvimExtraOptions == EDIT_WITH_VIM_USE_TABPAGES)
	wcscat(cmdStrW, L" -p");
    ಗಾಗಿ (i = 0; i < cbFiles; i++)
    {
	DragQueryFileW((HDROP)medium.hGlobal,
		i,
		m_szFileUserClickedOn,
		ARRAY_ಉದ್ದಳತೆ(m_szFileUserClickedOn));

	len = wcslen(cmdStrW) + wcslen(m_szFileUserClickedOn) + 4;
	ಆದರೆ (len > cmdlen)
	{
	    cmdlen = len + BUFSIZE;
	    wchar_t *cmdStrW_new = (wchar_t *)realloc(cmdStrW, cmdlen * ಗಾತ್ರ(wchar_t));
	    ಆದರೆ (cmdStrW_new == ಖಾಲಿ)
	    {
		ಬಿಡುಗಡೆ(cmdStrW);
		ಹಿಂತಿರುಗಿ E_FAIL;
	    }
	    cmdStrW = cmdStrW_new;
	}
	wcscat(cmdStrW, L" \"");
	wcscat(cmdStrW, m_szFileUserClickedOn);
	wcscat(cmdStrW, L"\"");
    }

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    Zero_ಸ್ಮರಣೆ(&si, ಗಾತ್ರ(si));
    si.cb = ಗಾತ್ರ(si);

    // Start the child process.
    ಆದರೆ (!CreateProcessW(ಖಾಲಿ,	// No module name (use command line).
		cmdStrW,	// Command line.
		ಖಾಲಿ,		// Process handle not inheritable.
		ಖಾಲಿ,		// Thread handle not inheritable.
		FALSE,		// Set handle inheritance to FALSE.
		0,		// No creation flags.
		ಖಾಲಿ,		// Use parent's environment block.
		workingDir,	// Use parent's starting directory.
		&si,		// Pointer to STARTUPINFO structure.
		&pi)		// Pointer to PROCESS_INFORMATION structure.
       )
    {
	// Use UTF-8 for gettext.
	ಅಕ್ಷರ *prev = ಗಣ_gettext_codeset();

	WCHAR *ಸಂದೇಶ = W(_("Error creating process: Check if gvim is in your path!"));
	WCHAR *title = W(_("gvimext.dll error"));

	ಸಂದೇಶ_Box_W(h_ಪೋಷಕ, ಸಂದೇಶ, title, MB_OK);

	ಬಿಡುಗಡೆ(ಸಂದೇಶ);
	ಬಿಡುಗಡೆ(title);

	// Restore previous codeset.
	restore_gettext_codeset(prev);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
    }
    ಬಿಡುಗಡೆ(cmdStrW);

    ಹಿಂತಿರುಗಿ NOERROR;
}
