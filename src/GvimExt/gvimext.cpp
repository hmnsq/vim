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

#define ARRAY_LENGTH(a) (ಪರಿಮಾಣ(a) / ಪರಿಮಾಣ((a)[0]))

ಸ್ಥಿರ ವರ್ಣ *searchpath(ವರ್ಣ *name);

// Always get an error while putting the following stuff to the
// gvimext.h file as class protected variables, give up and
// declare them as global stuff
FORMATETC fmte = {CF_HDROP,
		  (DVTARGETDEVICE FAR *)ರಿಕ್ತ,
		  DVASPECT_CONTENT,
		  -1,
		  TYMED_HGLOBAL
		 };
STGMEDIUM medium;
HRESULT hres = 0;
UINT cbFiles = 0;

/* The buffers size used to be MAX_PATH (260 bytes), but that's not always
 * enough */
#define BUFSIZE 1100

// The "Edit with Vim" shell extension provides these choices when
// a new instance of Gvim is selected:
//   - use tabpages
//   - enable diff mode
//   - none of the above
#define EDIT_WITH_VIM_USE_TABPAGES (2)
#define EDIT_WITH_VIM_IN_DIFF_MODE (1)
#define EDIT_WITH_VIM_NO_OPTIONS   (0)

//
// Get the name of the Gvim executable to use, with the path.
// When "runtime" is non-zero, we were called to find the runtime directory.
// Returns the path in name[BUFSIZE].  It's empty when it fails.
//
    ಸ್ಥಿರ ಶೂನ್ಯ
getGvimName(ವರ್ಣ *name, BOOL runtime)
{
    HKEY	keyhandle;
    DWORD	hlen;

    // Get the location of gvim from the registry.  Try
    // HKEY_CURRENT_USER first, then fall back to HKEY_LOCAL_MACHINE if
    // not found.
    name[0] = 0;
    ಯದಿ (Reg_Open_ಕೀಲಿ_Ex(HKEY_CURRENT_USER, "Software\\Vim\\Gvim", 0,
				       ಕೀಲಿ_READ, &keyhandle) == ERROR_SUCCESS)
    {
	hlen = BUFSIZE;
	ಯದಿ (Reg_Query_ಮೌಲ್ಯ_Ex(keyhandle, "path", 0, ರಿಕ್ತ, (BYTE *)name, &hlen)
							     != ERROR_SUCCESS)
	    name[0] = 0;
	ಅನ್ಯಥಾ
	    name[hlen] = 0;
	Reg_Close_ಕೀಲಿ(keyhandle);
    }

    ಯದಿ ((name[0] == 0) &&
	    (Reg_Open_ಕೀಲಿ_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
				       ಕೀಲಿ_READ, &keyhandle) == ERROR_SUCCESS))
    {
	hlen = BUFSIZE;
	ಯದಿ (Reg_Query_ಮೌಲ್ಯ_Ex(keyhandle, "path", 0, ರಿಕ್ತ, (BYTE *)name, &hlen)
							     != ERROR_SUCCESS)
	    name[0] = 0;
	ಅನ್ಯಥಾ
	    name[hlen] = 0;
	Reg_Close_ಕೀಲಿ(keyhandle);
    }

    // Registry didn't work, use the search path.
    ಯದಿ (name[0] == 0)
	strcpy(name, searchpath((ವರ್ಣ *)"gvim.exe"));

    ಯದಿ (!runtime)
    {
	// Only when looking for the executable, not the runtime dir, we can
	// search for the batch file or a name without a path.
	ಯದಿ (name[0] == 0)
	    strcpy(name, searchpath((ವರ್ಣ *)"gvim.bat"));
	ಯದಿ (name[0] == 0)
	    strcpy(name, "gvim");	// finds gvim.bat or gvim.exe
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
getGvimInvocation(ವರ್ಣ *name)
{
    getGvimName(name, FALSE);
    // avoid that Vim tries to expand wildcards in the file names
    strcat(name, " --literal");
}

    ಸ್ಥಿರ ಶೂನ್ಯ
getGvimInvocationW(wchar_t *nameW)
{
    ವರ್ಣ *name;

    name = (ವರ್ಣ *)malloc(BUFSIZE);
    getGvimInvocation(name);
    mbstowcs(nameW, name, BUFSIZE);
    ಮುಕ್ತ(name);
}

    ಸ್ಥಿರ BOOL
FileExists(LPCTSTR szPath)
{
    DWORD dwAttrib = GetFileAttributes(szPath);

    ಪ್ರತ್ಯಾಗಮ (dwAttrib != INVALID_FILE_ATTRIBUTES
	    && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

//
// Get the Vim runtime directory into buf[BUFSIZE].
// The result is empty when it failed.
// When it works, the path ends in a slash or backslash.
//
    ಸ್ಥಿರ ಶೂನ್ಯ
getRuntimeDir(ವರ್ಣ *buf)
{
    ಪೂರ್ಣಾಙ್ಕ		idx;

    getGvimName(buf, TRUE);
    ಯದಿ (buf[0] == 0)
	ಪ್ರತ್ಯಾಗಮ;

    // When no path found, use the search path to expand it.
    ಯದಿ (strchr(buf, '/') == ರಿಕ್ತ && strchr(buf, '\\') == ರಿಕ್ತ)
	strcpy(buf, searchpath(buf));

    // remove "gvim.exe" from the end
    ಪ್ರತಿ (idx = (ಪೂರ್ಣಾಙ್ಕ)strlen(buf) - 1; idx >= 0; idx--)
	ಯದಿ (buf[idx] == '\\' || buf[idx] == '/')
	{
	    buf[++idx] = 0;
	    ಭಙ್ಗ;
	}

    // Check the existence of defaults.vim
    ವರ್ಣ *p = buf + idx;
    strcpy(p, "defaults.vim");
    ಯದಿ (FileExists(buf))
    {
	*p = 0;		// Cut "defaults.vim"
	ಪ್ರತ್ಯಾಗಮ;
    }
    // Check the existence of vimXX\defaults.vim
    strcpy(p, VIM_VERSION_NODOT "\\" "defaults.vim");
    ಯದಿ (FileExists(buf))
    {
	p[ARRAY_LENGTH(VIM_VERSION_NODOT)] = 0;	// Cut "defaults.vim"
	ಪ್ರತ್ಯಾಗಮ;
    }
    // Check the existence of runtime\defaults.vim
    strcpy(p, "runtime" "\\" "defaults.vim");
    ಯದಿ (FileExists(buf))
    {
	p[ARRAY_LENGTH("runtime")] = 0;		// Cut "defaults.vim"
	ಪ್ರತ್ಯಾಗಮ;
    }
    // Not found
    buf[0] = 0;
    ಪ್ರತ್ಯಾಗಮ;
}

    WCHAR *
utf8_to_utf16(ಧ್ರುವ ವರ್ಣ *s)
{
    ಪೂರ್ಣಾಙ್ಕ size = MultiByteToWideChar(CP_UTF8, 0, s, -1, ರಿಕ್ತ, 0);
    WCHAR *buf = (WCHAR *)malloc(size * ಪರಿಮಾಣ(WCHAR));
    MultiByteToWideChar(CP_UTF8, 0, s, -1, buf, size);
    ಪ್ರತ್ಯಾಗಮ buf;
}

    HBITMAP
IconToBitmap(HICON hIcon, HBRUSH hBackground, ಪೂರ್ಣಾಙ್ಕ width, ಪೂರ್ಣಾಙ್ಕ height)
{
    HDC hDC = GetDC(ರಿಕ್ತ);
    HDC hMemDC = CreateCompatibleDC(hDC);
    HBITMAP hMemBmp = CreateCompatibleBitmap(hDC, width, height);
    HBITMAP hResultBmp = ರಿಕ್ತ;
    HGDIOBJ hOrgBMP = SelectObject(hMemDC, hMemBmp);

    DrawIconEx(hMemDC, 0, 0, hIcon, width, height, 0, hBackground, DI_NORMAL);

    hResultBmp = hMemBmp;
    hMemBmp = ರಿಕ್ತ;

    SelectObject(hMemDC, hOrgBMP);
    DeleteDC(hMemDC);
    ReleaseDC(ರಿಕ್ತ, hDC);
    DestroyIcon(hIcon);
    ಪ್ರತ್ಯಾಗಮ hResultBmp;
}

//
// GETTEXT: translated messages and menu entries
//
#ifndef FEAT_GETTEXT
# define _(x)	    x
# define W_impl(x) _wcsdup(L##x)
# define W(x)	    W_impl(x)
# define set_gettext_codeset()	    ರಿಕ್ತ
# define restore_gettext_codeset(x)
#ಅನ್ಯಥಾ
# define _(x)	    (*dyn_libintl_gettext)(x)
# define W(x)	    utf8_to_utf16(x)
# define VIMPACKAGE "vim"
# ifndef GETTEXT_DLL
#  define GETTEXT_DLL "libintl.dll"
#  define GETTEXT_DLL_ALT "libintl-8.dll"
# endif

// Dummy functions
ಸ್ಥಿರ ವರ್ಣ *null_libintl_gettext(ಧ್ರುವ ವರ್ಣ *);
ಸ್ಥಿರ ವರ್ಣ *null_libintl_textdomain(ಧ್ರುವ ವರ್ಣ *);
ಸ್ಥಿರ ವರ್ಣ *null_libintl_bindtextdomain(ಧ್ರುವ ವರ್ಣ *, ಧ್ರುವ ವರ್ಣ *);
ಸ್ಥಿರ ವರ್ಣ *null_libintl_bind_textdomain_codeset(ಧ್ರುವ ವರ್ಣ *, ಧ್ರುವ ವರ್ಣ *);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ dyn_libintl_init(ವರ್ಣ *dir);
ಸ್ಥಿರ ಶೂನ್ಯ dyn_libintl_end(ಶೂನ್ಯ);

ಸ್ಥಿರ HINSTANCE hLibintlDLL = 0;
ಸ್ಥಿರ ವರ್ಣ *(*dyn_libintl_gettext)(ಧ್ರುವ ವರ್ಣ *) = null_libintl_gettext;
ಸ್ಥಿರ ವರ್ಣ *(*dyn_libintl_textdomain)(ಧ್ರುವ ವರ್ಣ *) = null_libintl_textdomain;
ಸ್ಥಿರ ವರ್ಣ *(*dyn_libintl_bindtextdomain)(ಧ್ರುವ ವರ್ಣ *, ಧ್ರುವ ವರ್ಣ *)
						= null_libintl_bindtextdomain;
ಸ್ಥಿರ ವರ್ಣ *(*dyn_libintl_bind_textdomain_codeset)(ಧ್ರುವ ವರ್ಣ *, ಧ್ರುವ ವರ್ಣ *)
				       = null_libintl_bind_textdomain_codeset;

//
// Attempt to load libintl.dll.  If it doesn't work, use dummy functions.
// "dir" is the directory where the libintl.dll might be.
// Return 1 for success, 0 for failure.
//
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
dyn_libintl_init(ವರ್ಣ *dir)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಸ್ಥಿರ ಸಂರಚನಾ
    {
	ವರ್ಣ	    *name;
	FARPROC	    *ptr;
    } libintl_entry[] =
    {
	{(ವರ್ಣ *)"gettext",		(FARPROC*)&dyn_libintl_gettext},
	{(ವರ್ಣ *)"textdomain",		(FARPROC*)&dyn_libintl_textdomain},
	{(ವರ್ಣ *)"bindtextdomain",	(FARPROC*)&dyn_libintl_bindtextdomain},
	{(ವರ್ಣ *)"bind_textdomain_codeset", (FARPROC*)&dyn_libintl_bind_textdomain_codeset},
	{ರಿಕ್ತ, ರಿಕ್ತ}
    };
    DWORD	len, len2;
    LPWSTR	buf = ರಿಕ್ತ;
    LPWSTR	buf2 = ರಿಕ್ತ;

    // No need to initialize twice.
    ಯದಿ (hLibintlDLL)
	ಪ್ರತ್ಯಾಗಮ 1;

    // Load gettext library from $VIMRUNTIME\GvimExt{64,32} directory.
    // Add the directory to $PATH temporarily.
    len = GetEnvironmentVariableW(L"PATH", ರಿಕ್ತ, 0);
    len2 = MAX_PATH + 1 + len;
    buf = (LPWSTR)malloc(len * ಪರಿಮಾಣ(WCHAR));
    buf2 = (LPWSTR)malloc(len2 * ಪರಿಮಾಣ(WCHAR));
    ಯದಿ (buf != ರಿಕ್ತ && buf2 != ರಿಕ್ತ)
    {
	GetEnvironmentVariableW(L"PATH", buf, len);
# ifdef _WIN64
	_snwprintf(buf2, len2, L"%S\\GvimExt64;%s", dir, buf);
# ಅನ್ಯಥಾ
	_snwprintf(buf2, len2, L"%S\\GvimExt32;%s", dir, buf);
# endif
	SetEnvironmentVariableW(L"PATH", buf2);
	hLibintlDLL = LoadLibrary(GETTEXT_DLL);
# ifdef GETTEXT_DLL_ALT
	ಯದಿ (!hLibintlDLL)
	    hLibintlDLL = LoadLibrary(GETTEXT_DLL_ALT);
# endif
	SetEnvironmentVariableW(L"PATH", buf);
    }
    ಮುಕ್ತ(buf);
    ಮುಕ್ತ(buf2);
    ಯದಿ (!hLibintlDLL)
	ಪ್ರತ್ಯಾಗಮ 0;

    // Get the addresses of the functions we need.
    ಪ್ರತಿ (i = 0; libintl_entry[i].name != ರಿಕ್ತ
					 && libintl_entry[i].ptr != ರಿಕ್ತ; ++i)
    {
	ಯದಿ ((*libintl_entry[i].ptr = GetProcAddress(hLibintlDLL,
					      libintl_entry[i].name)) == ರಿಕ್ತ)
	{
	    dyn_libintl_end();
	    ಪ್ರತ್ಯಾಗಮ 0;
	}
    }
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
dyn_libintl_end(ಶೂನ್ಯ)
{
    ಯದಿ (hLibintlDLL)
	FreeLibrary(hLibintlDLL);
    hLibintlDLL			= ರಿಕ್ತ;
    dyn_libintl_gettext		= null_libintl_gettext;
    dyn_libintl_textdomain	= null_libintl_textdomain;
    dyn_libintl_bindtextdomain	= null_libintl_bindtextdomain;
    dyn_libintl_bind_textdomain_codeset	= null_libintl_bind_textdomain_codeset;
}

    ಸ್ಥಿರ ವರ್ಣ *
null_libintl_gettext(ಧ್ರುವ ವರ್ಣ *msgid)
{
    ಪ್ರತ್ಯಾಗಮ (ವರ್ಣ *)msgid;
}

    ಸ್ಥಿರ ವರ್ಣ *
null_libintl_textdomain(ಧ್ರುವ ವರ್ಣ * /* domainname */)
{
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಸ್ಥಿರ ವರ್ಣ *
null_libintl_bindtextdomain(ಧ್ರುವ ವರ್ಣ * /* domainname */, ಧ್ರುವ ವರ್ಣ * /* dirname */)
{
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಸ್ಥಿರ ವರ್ಣ *
null_libintl_bind_textdomain_codeset(ಧ್ರುವ ವರ್ಣ * /* domainname */, ಧ್ರುವ ವರ್ಣ * /* codeset */)
{
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

//
// Setup for translating strings.
//
    ಸ್ಥಿರ ಶೂನ್ಯ
dyn_gettext_load(ಶೂನ್ಯ)
{
    ವರ್ಣ    szBuff[BUFSIZE];
    DWORD   len;

    // Try to locate the runtime files.  The path is used to find libintl.dll
    // and the vim.mo files.
    getRuntimeDir(szBuff);
    ಯದಿ (szBuff[0] != 0)
    {
	len = (DWORD)strlen(szBuff);
	ಯದಿ (dyn_libintl_init(szBuff))
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
    dyn_libintl_end();
}

//
// Use UTF-8 for gettext. Returns previous codeset.
//
    ಸ್ಥಿರ ವರ್ಣ *
set_gettext_codeset(ಶೂನ್ಯ)
{
    ವರ್ಣ *prev = dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, ರಿಕ್ತ);
    prev = _strdup((prev != ರಿಕ್ತ) ? prev : "char");
    dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, "utf-8");

    ಪ್ರತ್ಯಾಗಮ prev;
}

//
// Restore previous codeset for gettext.
//
    ಸ್ಥಿರ ಶೂನ್ಯ
restore_gettext_codeset(ವರ್ಣ *prev)
{
    dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, prev);
    ಮುಕ್ತ(prev);
}
#endif // FEAT_GETTEXT

//
// Global variables
//
UINT      g_cRefThisDll = 0;    // Reference count of this DLL.
HINSTANCE g_hmodThisDll = ರಿಕ್ತ;	// Handle to this DLL itself.


//---------------------------------------------------------------------------
// DllMain
//---------------------------------------------------------------------------
ಬಾಹ್ಯ "C" ಪೂರ್ಣಾಙ್ಕ APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID  /* lpReserved */)
{
    ವಿಕಲ್ಪ (dwReason)
    {
    ಪಕ್ಷ DLL_PROCESS_ATTACH:
	// Extension DLL one-time initialization
	g_hmodThisDll = hInstance;
	ಭಙ್ಗ;

    ಪಕ್ಷ DLL_PROCESS_DETACH:
	ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ 1;   // ok
}

    ಸ್ಥಿರ ಶೂನ್ಯ
inc_cRefThisDLL()
{
#ifdef FEAT_GETTEXT
    ಯದಿ (g_cRefThisDll == 0)
	dyn_gettext_load();
#endif
    InterlockedIncrement((LPLONG)&g_cRefThisDll);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
dec_cRefThisDLL()
{
#ifdef FEAT_GETTEXT
    ಯದಿ (InterlockedDecrement((LPLONG)&g_cRefThisDll) == 0)
	dyn_gettext_free();
#ಅನ್ಯಥಾ
    InterlockedDecrement((LPLONG)&g_cRefThisDll);
#endif
}

//---------------------------------------------------------------------------
// DllCanUnloadNow
//---------------------------------------------------------------------------

STDAPI DllCanUnloadNow(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (g_cRefThisDll == 0 ? S_OK : S_FALSE);
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppvOut)
{
    *ppvOut = ರಿಕ್ತ;

    ಯದಿ (IsEqualIID(rclsid, CLSID_ShellExtension))
    {
	CShellExtClassFactory *pcf = ನೂತನ CShellExtClassFactory;

	ಪ್ರತ್ಯಾಗಮ pcf->QueryInterface(riid, ppvOut);
    }

    ಪ್ರತ್ಯಾಗಮ CLASS_E_CLASSNOTAVAILABLE;
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
    *ppv = ರಿಕ್ತ;

    // any interface on this object is the object pointer

    ಯದಿ (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	*ppv = (LPCLASSFACTORY)ಏತತ್;

	AddRef();

	ಪ್ರತ್ಯಾಗಮ NOERROR;
    }

    ಪ್ರತ್ಯಾಗಮ E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::AddRef()
{
    ಪ್ರತ್ಯಾಗಮ InterlockedIncrement((LPLONG)&m_cRef);
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::Release()
{
    ಯದಿ (InterlockedDecrement((LPLONG)&m_cRef))
	ಪ್ರತ್ಯಾಗಮ m_cRef;

    delete ಏತತ್;

    ಪ್ರತ್ಯಾಗಮ 0L;
}

STDMETHODIMP CShellExtClassFactory::CreateInstance(LPUNKNOWN pUnkOuter,
						      REFIID riid,
						      LPVOID *ppvObj)
{
    *ppvObj = ರಿಕ್ತ;

    // Shell extensions typically don't support aggregation (inheritance)

    ಯದಿ (pUnkOuter)
	ಪ್ರತ್ಯಾಗಮ CLASS_E_NOAGGREGATION;

    // Create the main shell extension object.  The shell will then call
    // QueryInterface with IID_IShellExtInit--this is how shell extensions are
    // initialized.

    LPCSHELLEXT pShellExt = ನೂತನ CShellExt();  // create the CShellExt object

    ಯದಿ (ರಿಕ್ತ == pShellExt)
	ಪ್ರತ್ಯಾಗಮ E_OUTOFMEMORY;

    ಪ್ರತ್ಯಾಗಮ pShellExt->QueryInterface(riid, ppvObj);
}


STDMETHODIMP CShellExtClassFactory::LockServer(BOOL  /* fLock */)
{
    ಪ್ರತ್ಯಾಗಮ NOERROR;
}

// *********************** CShellExt *************************
CShellExt::CShellExt()
{
    m_cRef = 0L;
    m_pDataObj = ರಿಕ್ತ;

    inc_cRefThisDLL();

    LoadMenuIcon();
}

CShellExt::~CShellExt()
{
    ಯದಿ (m_pDataObj)
	m_pDataObj->Release();

    dec_cRefThisDLL();

    ಯದಿ (m_hVimIconBitmap)
	DeleteObject(m_hVimIconBitmap);
}

STDMETHODIMP CShellExt::QueryInterface(REFIID riid, LPVOID FAR *ppv)
{
    *ppv = ರಿಕ್ತ;

    ಯದಿ (IsEqualIID(riid, IID_IShellExtInit) || IsEqualIID(riid, IID_IUnknown))
    {
	*ppv = (LPSHELLEXTINIT)ಏತತ್;
    }
    ಅನ್ಯಥಾ ಯದಿ (IsEqualIID(riid, IID_IContextMenu))
    {
	*ppv = (LPCONTEXTMENU)ಏತತ್;
    }

    ಯದಿ (*ppv)
    {
	AddRef();

	ಪ್ರತ್ಯಾಗಮ NOERROR;
    }

    ಪ್ರತ್ಯಾಗಮ E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExt::AddRef()
{
    ಪ್ರತ್ಯಾಗಮ InterlockedIncrement((LPLONG)&m_cRef);
}

STDMETHODIMP_(ULONG) CShellExt::Release()
{

    ಯದಿ (InterlockedDecrement((LPLONG)&m_cRef))
	ಪ್ರತ್ಯಾಗಮ m_cRef;

    delete ಏತತ್;

    ಪ್ರತ್ಯಾಗಮ 0L;
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
				   LPDATAOBJECT pDataObj,
				   HKEY  /* hRegKey */)
{
    // Initialize can be called more than once
    ಯದಿ (m_pDataObj)
	m_pDataObj->Release();

    // duplicate the object pointer and registry handle

    ಯದಿ (pDataObj)
    {
	m_pDataObj = pDataObj;
	pDataObj->AddRef();
    }

    ಪ್ರತ್ಯಾಗಮ NOERROR;
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
					 UINT indexMenu,
					 UINT idCmdFirst,
					 UINT  /* idCmdLast */,
					 UINT  /* uFlags */)
{
    UINT idCmd = idCmdFirst;

    hres = m_pDataObj->GetData(&fmte, &medium);
    ಯದಿ (medium.hGlobal)
	cbFiles = DragQueryFileW((HDROP)medium.hGlobal, (UINT)-1, 0, 0);

    // InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

    // Initialize m_cntOfHWnd to 0
    m_cntOfHWnd = 0;

    HKEY keyhandle;
    ತಾರ್ಕಿಕ showExisting = ಸತ್ಯ;
    ತಾರ್ಕಿಕ showIcons = ಸತ್ಯ;

    // Check whether "Edit with existing Vim" entries are disabled.
    ಯದಿ (Reg_Open_ಕೀಲಿ_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
				       ಕೀಲಿ_READ, &keyhandle) == ERROR_SUCCESS)
    {
	ಯದಿ (Reg_Query_ಮೌಲ್ಯ_Ex(keyhandle, "DisableEditWithExisting", 0, ರಿಕ್ತ,
						 ರಿಕ್ತ, ರಿಕ್ತ) == ERROR_SUCCESS)
	    showExisting = ಅಸತ್ಯ;
	ಯದಿ (Reg_Query_ಮೌಲ್ಯ_Ex(keyhandle, "DisableContextMenuIcons", 0, ರಿಕ್ತ,
						 ರಿಕ್ತ, ರಿಕ್ತ) == ERROR_SUCCESS)
	    showIcons = ಅಸತ್ಯ;
	Reg_Close_ಕೀಲಿ(keyhandle);
    }

    // Use UTF-8 for gettext.
    ವರ್ಣ *prev = set_gettext_codeset();

    // Retrieve all the vim instances, unless disabled.
    ಯದಿ (showExisting)
	EnumWindows(EnumWindowsProc, (LPARAM)ಏತತ್);

    MENUITEMINFOW mii = { ಪರಿಮಾಣ(MENUITEMINFOW) };
    mii.fMask = MIIM_STRING | MIIM_ID;
    ಯದಿ (showIcons)
    {
	mii.fMask |= MIIM_BITMAP;
	mii.hbmpItem = m_hVimIconBitmap;
    }

    ಯದಿ (cbFiles > 1)
    {
	mii.wID = idCmd++;
	mii.dwTypeData = W(_("Edit with Vim using &tabpages"));
	mii.cch = wcslen(mii.dwTypeData);
	ಸೇರಿಸು_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	ಮುಕ್ತ(mii.dwTypeData);

	mii.wID = idCmd++;
	mii.dwTypeData = W(_("Edit with single &Vim"));
	mii.cch = wcslen(mii.dwTypeData);
	ಸೇರಿಸು_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	ಮುಕ್ತ(mii.dwTypeData);

	ಯದಿ (cbFiles <= 4)
	{
	    // Can edit up to 4 files in diff mode
	    mii.wID = idCmd++;
	    mii.dwTypeData = W(_("Diff with Vim"));
	    mii.cch = wcslen(mii.dwTypeData);
	    ಸೇರಿಸು_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	    ಮುಕ್ತ(mii.dwTypeData);
	    m_edit_existing_off = 3;
	}
	ಅನ್ಯಥಾ
	    m_edit_existing_off = 2;

    }
    ಅನ್ಯಥಾ
    {
	mii.wID = idCmd++;
	mii.dwTypeData = W(_("Edit with &Vim"));
	mii.cch = wcslen(mii.dwTypeData);
	ಸೇರಿಸು_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	ಮುಕ್ತ(mii.dwTypeData);
	m_edit_existing_off = 1;
    }

    HMENU hSubMenu = ರಿಕ್ತ;
    ಯದಿ (m_cntOfHWnd > 1)
    {
	hSubMenu = CreatePopupMenu();
	mii.fMask |= MIIM_SUBMENU;
	mii.wID = idCmd;
	mii.dwTypeData = W(_("Edit with existing Vim"));
	mii.cch = wcslen(mii.dwTypeData);
	mii.hSubMenu = hSubMenu;
	ಸೇರಿಸು_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	ಮುಕ್ತ(mii.dwTypeData);
	mii.fMask = mii.fMask & ~MIIM_SUBMENU;
	mii.hSubMenu = ರಿಕ್ತ;
    }
    // Now display all the vim instances
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < m_cntOfHWnd; i++)
    {
	WCHAR title[BUFSIZE];
	WCHAR temp[BUFSIZE];
	ಪೂರ್ಣಾಙ್ಕ index;
	HMENU hmenu;

	// Obtain window title, continue if can not
	ಯದಿ (GetWindowTextW(m_hWnd[i], title, BUFSIZE - 1) == 0)
	    ಅನುವರ್ತ;
	// Truncate the title before the path, keep the file name
	WCHAR *pos = wcschr(title, L'(');
	ಯದಿ (pos != ರಿಕ್ತ)
	{
	    ಯದಿ (pos > title && pos[-1] == L' ')
		--pos;
	    *pos = 0;
	}
	// Now concatenate
	ಯದಿ (m_cntOfHWnd > 1)
	    temp[0] = L'\0';
	ಅನ್ಯಥಾ
	{
	    WCHAR *s = W(_("Edit with existing Vim - "));
	    wcsncpy(temp, s, BUFSIZE - 1);
	    temp[BUFSIZE - 1] = L'\0';
	    ಮುಕ್ತ(s);
	}
	wcsncat(temp, title, BUFSIZE - 1 - wcslen(temp));
	temp[BUFSIZE - 1] = L'\0';

	mii.wID = idCmd++;
	mii.dwTypeData = temp;
	mii.cch = wcslen(mii.dwTypeData);
	ಯದಿ (m_cntOfHWnd > 1)
	{
	    hmenu = hSubMenu;
	    index = i;
	}
	ಅನ್ಯಥಾ
	{
	    hmenu = hMenu;
	    index = indexMenu++;
	}
	ಸೇರಿಸು_Menu_Item_W(hmenu, index, TRUE, &mii);
    }
    // InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

    // Restore previous codeset.
    restore_gettext_codeset(prev);

    // Must return number of menu items we added.
    ಪ್ರತ್ಯಾಗಮ ResultFromShort(idCmd-idCmdFirst);
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
    ಪೂರ್ಣಾಙ್ಕ gvimExtraOptions;

    // If HIWORD(lpcmi->lpVerb) then we have been called programmatically
    // and lpVerb is a command that should be invoked.  Otherwise, the shell
    // has called us, and LOWORD(lpcmi->lpVerb) is the menu ID the user has
    // selected.  Actually, it's (menu ID - idCmdFirst) from QueryContextMenu().
    ಯದಿ (!HIWORD(lpcmi->lpVerb))
    {
	UINT idCmd = LOWORD(lpcmi->lpVerb);

	ಯದಿ (idCmd >= m_edit_existing_off)
	{
	    // Existing with vim instance
	    hr = ತಳ್ಳು_To_Window(lpcmi->hwnd,
		    lpcmi->lpDirectory,
		    lpcmi->lpVerb,
		    lpcmi->lpParameters,
		    lpcmi->nShow,
		    idCmd - m_edit_existing_off);
	}
	ಅನ್ಯಥಾ
	{
	    ವಿಕಲ್ಪ (idCmd)
	    {
		ಪಕ್ಷ 0:
		    gvimExtraOptions = EDIT_WITH_VIM_USE_TABPAGES;
		    ಭಙ್ಗ;
		ಪಕ್ಷ 1:
		    gvimExtraOptions = EDIT_WITH_VIM_NO_OPTIONS;
		    ಭಙ್ಗ;
		ಪಕ್ಷ 2:
		    gvimExtraOptions = EDIT_WITH_VIM_IN_DIFF_MODE;
		    ಭಙ್ಗ;
		ಸ್ವಭಾವ:
		    // If execution reaches this point we likely have an
		    // inconsistency between the code that setup the menus
		    // and this code that determines what the user
		    // selected.  This should be detected and fixed during
		    // development.
		    ಪ್ರತ್ಯಾಗಮ E_FAIL;
	    }

            LPCMINVOKECOMMANDINFOEX lpcmiex = (LPCMINVOKECOMMANDINFOEX)lpcmi;
            LPCWSTR currentDirectory = lpcmi->cbSize == ಪರಿಮಾಣ(CMINVOKECOMMANDINFOEX) ? lpcmiex->lpDirectoryW : ರಿಕ್ತ;

	    hr = InvokeSingleGvim(lpcmi->hwnd,
		    currentDirectory,
		    lpcmi->lpVerb,
		    lpcmi->lpParameters,
		    lpcmi->nShow,
		    gvimExtraOptions);
	}
    }
    ಪ್ರತ್ಯಾಗಮ hr;
}

STDMETHODIMP CShellExt::ತಳ್ಳು_To_Window(HWND  /* hParent */,
				   LPCSTR  /* pszWorkingDir */,
				   LPCSTR  /* pszCmd */,
				   LPCSTR  /* pszParam */,
				   ಪೂರ್ಣಾಙ್ಕ  /* iShowCmd */,
				   ಪೂರ್ಣಾಙ್ಕ idHWnd)
{
    HWND hWnd = m_hWnd[idHWnd];

    // Show and bring vim instance to foreground
    ಯದಿ (IsIconic(hWnd) != 0)
	ShowWindow(hWnd, SW_RESTORE);
    ಅನ್ಯಥಾ
	ShowWindow(hWnd, SW_SHOW);
    SetForegroundWindow(hWnd);

    // Post the selected files to the vim instance
    PostMessage(hWnd, WM_DROPFILES, (WPARAM)medium.hGlobal, 0);

    ಪ್ರತ್ಯಾಗಮ NOERROR;
}

STDMETHODIMP CShellExt::GetCommandString(UINT_PTR  /* idCmd */,
					 UINT uFlags,
					 UINT FAR * /* reserved */,
					 LPSTR pszName,
					 UINT cchMax)
{
    // Use UTF-8 for gettext.
    ವರ್ಣ *prev = set_gettext_codeset();

    WCHAR *s = W(_("Edits the selected file(s) with Vim"));
    ಯದಿ (uFlags == GCS_HELPTEXTW && cchMax > wcslen(s))
	wcscpy((WCHAR *)pszName, s);
    ಮುಕ್ತ(s);

    // Restore previous codeset.
    restore_gettext_codeset(prev);

    ಪ್ರತ್ಯಾಗಮ NOERROR;
}

BOOL CALLBACK CShellExt::EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    ವರ್ಣ temp[BUFSIZE];

    // First do a bunch of check
    // No invisible window
    ಯದಿ (!IsWindowVisible(hWnd))
	ಪ್ರತ್ಯಾಗಮ TRUE;
    // No child window ???
    // if (GetParent(hWnd)) return TRUE;
    // Class name should be Vim, if failed to get class name, return
    ಯದಿ (GetClassName(hWnd, temp, ARRAY_LENGTH(temp)) == 0)
	ಪ್ರತ್ಯಾಗಮ TRUE;
    // Compare class name to that of vim, if not, return
    ಯದಿ (_strnicmp(temp, "vim", ARRAY_LENGTH("vim")) != 0)
	ಪ್ರತ್ಯಾಗಮ TRUE;
    // First check if the number of vim instance exceeds MAX_HWND
    CShellExt *cs = (CShellExt*) lParam;
    ಯದಿ (cs->m_cntOfHWnd >= MAX_HWND)
	ಪ್ರತ್ಯಾಗಮ FALSE;	// stop enumeration
    // Now we get the vim window, put it into some kind of array
    cs->m_hWnd[cs->m_cntOfHWnd] = hWnd;
    cs->m_cntOfHWnd ++;

    ಪ್ರತ್ಯಾಗಮ TRUE; // continue enumeration (otherwise this would be false)
}

BOOL CShellExt::LoadMenuIcon()
{
    ವರ್ಣ vimExeFile[BUFSIZE];
    getGvimName(vimExeFile, TRUE);
    ಯದಿ (vimExeFile[0] == '\0')
	ಪ್ರತ್ಯಾಗಮ FALSE;
    HICON hVimIcon;
    ಯದಿ (ExtractIconEx(vimExeFile, 0, ರಿಕ್ತ, &hVimIcon, 1) == 0)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    m_hVimIconBitmap = IconToBitmap(hVimIcon,
	    GetSysColorBrush(COLOR_MENU),
	    GetSystemMetrics(SM_CXSMICON),
	    GetSystemMetrics(SM_CYSMICON));
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

    ಸ್ಥಿರ ವರ್ಣ *
searchpath(ವರ್ಣ *name)
{
    ಸ್ಥಿರ ಸಙ್ಘ {
	ವರ್ಣ	location[2 * BUFSIZE];
	WCHAR	wname[BUFSIZE];
    };
    WCHAR wlocation[BUFSIZE + 1];

    // There appears to be a bug in FindExecutableA() on Windows NT.
    // Use FindExecutableW() instead...
    MultiByteToWideChar(CP_ACP, 0, name, -1, wname, BUFSIZE);
    ಯದಿ (FindExecutableW(wname, L"", wlocation) > (HINSTANCE)32)
    {
	WideCharToMultiByte(CP_ACP, 0, wlocation, -1,
		location, ಪರಿಮಾಣ(location), ರಿಕ್ತ, ರಿಕ್ತ);
	ಪ್ರತ್ಯಾಗಮ location;
    }
    ಪ್ರತ್ಯಾಗಮ (ವರ್ಣ *)"";
}


STDMETHODIMP CShellExt::InvokeSingleGvim(HWND hParent,
				   LPCWSTR  workingDir,
				   LPCSTR  /* pszCmd */,
				   LPCSTR  /* pszParam */,
				   ಪೂರ್ಣಾಙ್ಕ  /* iShowCmd */,
				   ಪೂರ್ಣಾಙ್ಕ gvimExtraOptions)
{
    wchar_t	m_szFileUserClickedOn[BUFSIZE];
    wchar_t	*cmdStrW;
    size_t	cmdlen;
    size_t	len;
    UINT i;

    cmdlen = BUFSIZE;
    cmdStrW  = (wchar_t *) malloc(cmdlen * ಪರಿಮಾಣ(wchar_t));
    ಯದಿ (cmdStrW == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ E_FAIL;
    getGvimInvocationW(cmdStrW);

    ಯದಿ (gvimExtraOptions == EDIT_WITH_VIM_IN_DIFF_MODE)
	wcscat(cmdStrW, L" -d");
    ಅನ್ಯಥಾ ಯದಿ (gvimExtraOptions == EDIT_WITH_VIM_USE_TABPAGES)
	wcscat(cmdStrW, L" -p");
    ಪ್ರತಿ (i = 0; i < cbFiles; i++)
    {
	DragQueryFileW((HDROP)medium.hGlobal,
		i,
		m_szFileUserClickedOn,
		ARRAY_LENGTH(m_szFileUserClickedOn));

	len = wcslen(cmdStrW) + wcslen(m_szFileUserClickedOn) + 4;
	ಯದಿ (len > cmdlen)
	{
	    cmdlen = len + BUFSIZE;
	    wchar_t *cmdStrW_new = (wchar_t *)realloc(cmdStrW, cmdlen * ಪರಿಮಾಣ(wchar_t));
	    ಯದಿ (cmdStrW_new == ರಿಕ್ತ)
	    {
		ಮುಕ್ತ(cmdStrW);
		ಪ್ರತ್ಯಾಗಮ E_FAIL;
	    }
	    cmdStrW = cmdStrW_new;
	}
	wcscat(cmdStrW, L" \"");
	wcscat(cmdStrW, m_szFileUserClickedOn);
	wcscat(cmdStrW, L"\"");
    }

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, ಪರಿಮಾಣ(si));
    si.cb = ಪರಿಮಾಣ(si);

    // Start the child process.
    ಯದಿ (!CreateProcessW(ರಿಕ್ತ,	// No module name (use command line).
		cmdStrW,	// Command line.
		ರಿಕ್ತ,		// Process handle not inheritable.
		ರಿಕ್ತ,		// Thread handle not inheritable.
		FALSE,		// Set handle inheritance to FALSE.
		0,		// No creation flags.
		ರಿಕ್ತ,		// Use parent's environment block.
		workingDir,	// Use parent's starting directory.
		&si,		// Pointer to STARTUPINFO structure.
		&pi)		// Pointer to PROCESS_INFORMATION structure.
       )
    {
	// Use UTF-8 for gettext.
	ವರ್ಣ *prev = set_gettext_codeset();

	WCHAR *ಸಂದೇಶ = W(_("Error creating process: Check if gvim is in your path!"));
	WCHAR *title = W(_("gvimext.dll error"));

	MessageBoxW(hParent, ಸಂದೇಶ, title, MB_OK);

	ಮುಕ್ತ(ಸಂದೇಶ);
	ಮುಕ್ತ(title);

	// Restore previous codeset.
	restore_gettext_codeset(prev);
    }
    ಅನ್ಯಥಾ
    {
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
    }
    ಮುಕ್ತ(cmdStrW);

    ಪ್ರತ್ಯಾಗಮ NOERROR;
}
