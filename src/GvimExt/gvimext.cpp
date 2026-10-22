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

#അന്തര്ഭാവ "gvimext.h"
#അന്തര്ഭാവ "../version.h"

#define ARRAY_LENGTH(a) (വലുപ്പം(a) / വലുപ്പം((a)[0]))

സ്ഥിതം അക്ഷരം *searchpath(അക്ഷരം *name);

// Always get an error while putting the following stuff to the
// gvimext.h file as class protected variables, give up and
// declare them as global stuff
FORMATETC fmte = {CF_HDROP,
		  (DVTARGETDEVICE FAR *)ശൂന്യ,
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
    സ്ഥിതം ശൂന്യം
getGvimName(അക്ഷരം *name, BOOL runtime)
{
    HKEY	keyhandle;
    DWORD	hlen;

    // Get the location of gvim from the registry.  Try
    // HKEY_CURRENT_USER first, then fall back to HKEY_LOCAL_MACHINE if
    // not found.
    name[0] = 0;
    എങ്കിൽ (Reg_Open_താക്കോൽ_Ex(HKEY_CURRENT_USER, "Software\\Vim\\Gvim", 0,
				       താക്കോൽ_READ, &keyhandle) == ERROR_SUCCESS)
    {
	hlen = BUFSIZE;
	എങ്കിൽ (Reg_Query_മൂല്യം_Ex(keyhandle, "path", 0, ശൂന്യ, (BYTE *)name, &hlen)
							     != ERROR_SUCCESS)
	    name[0] = 0;
	അല്ലെങ്കിൽ
	    name[hlen] = 0;
	Reg_Close_താക്കോൽ(keyhandle);
    }

    എങ്കിൽ ((name[0] == 0) &&
	    (Reg_Open_താക്കോൽ_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
				       താക്കോൽ_READ, &keyhandle) == ERROR_SUCCESS))
    {
	hlen = BUFSIZE;
	എങ്കിൽ (Reg_Query_മൂല്യം_Ex(keyhandle, "path", 0, ശൂന്യ, (BYTE *)name, &hlen)
							     != ERROR_SUCCESS)
	    name[0] = 0;
	അല്ലെങ്കിൽ
	    name[hlen] = 0;
	Reg_Close_താക്കോൽ(keyhandle);
    }

    // Registry didn't work, use the search path.
    എങ്കിൽ (name[0] == 0)
	strcpy(name, searchpath((അക്ഷരം *)"gvim.exe"));

    എങ്കിൽ (!runtime)
    {
	// Only when looking for the executable, not the runtime dir, we can
	// search for the batch file or a name without a path.
	എങ്കിൽ (name[0] == 0)
	    strcpy(name, searchpath((അക്ഷരം *)"gvim.bat"));
	എങ്കിൽ (name[0] == 0)
	    strcpy(name, "gvim");	// finds gvim.bat or gvim.exe
    }
}

    സ്ഥിതം ശൂന്യം
getGvimInvocation(അക്ഷരം *name)
{
    getGvimName(name, FALSE);
    // avoid that Vim tries to expand wildcards in the file names
    strcat(name, " --literal");
}

    സ്ഥിതം ശൂന്യം
getGvimInvocationW(wchar_t *nameW)
{
    അക്ഷരം *name;

    name = (അക്ഷരം *)malloc(BUFSIZE);
    getGvimInvocation(name);
    mbstowcs(nameW, name, BUFSIZE);
    സ്വതന്ത്രം(name);
}

    സ്ഥിതം BOOL
FileExists(LPCTSTR szPath)
{
    DWORD dwAttrib = GetFileAttributes(szPath);

    തിരികെ (dwAttrib != INVALID_FILE_ATTRIBUTES
	    && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

//
// Get the Vim runtime directory into buf[BUFSIZE].
// The result is empty when it failed.
// When it works, the path ends in a slash or backslash.
//
    സ്ഥിതം ശൂന്യം
getRuntimeDir(അക്ഷരം *buf)
{
    പൂർണ്ണസംഖ്യ		idx;

    getGvimName(buf, TRUE);
    എങ്കിൽ (buf[0] == 0)
	തിരികെ;

    // When no path found, use the search path to expand it.
    എങ്കിൽ (strchr(buf, '/') == ശൂന്യ && strchr(buf, '\\') == ശൂന്യ)
	strcpy(buf, searchpath(buf));

    // remove "gvim.exe" from the end
    വേണ്ടി (idx = (പൂർണ്ണസംഖ്യ)strlen(buf) - 1; idx >= 0; idx--)
	എങ്കിൽ (buf[idx] == '\\' || buf[idx] == '/')
	{
	    buf[++idx] = 0;
	    നിർത്തുക;
	}

    // Check the existence of defaults.vim
    അക്ഷരം *p = buf + idx;
    strcpy(p, "defaults.vim");
    എങ്കിൽ (FileExists(buf))
    {
	*p = 0;		// Cut "defaults.vim"
	തിരികെ;
    }
    // Check the existence of vimXX\defaults.vim
    strcpy(p, VIM_VERSION_NODOT "\\" "defaults.vim");
    എങ്കിൽ (FileExists(buf))
    {
	p[ARRAY_LENGTH(VIM_VERSION_NODOT)] = 0;	// Cut "defaults.vim"
	തിരികെ;
    }
    // Check the existence of runtime\defaults.vim
    strcpy(p, "runtime" "\\" "defaults.vim");
    എങ്കിൽ (FileExists(buf))
    {
	p[ARRAY_LENGTH("runtime")] = 0;		// Cut "defaults.vim"
	തിരികെ;
    }
    // Not found
    buf[0] = 0;
    തിരികെ;
}

    WCHAR *
utf8_to_utf16(സ്ഥിരം അക്ഷരം *s)
{
    പൂർണ്ണസംഖ്യ size = MultiByteToWideChar(CP_UTF8, 0, s, -1, ശൂന്യ, 0);
    WCHAR *buf = (WCHAR *)malloc(size * വലുപ്പം(WCHAR));
    MultiByteToWideChar(CP_UTF8, 0, s, -1, buf, size);
    തിരികെ buf;
}

    HBITMAP
IconToBitmap(HICON hIcon, HBRUSH hBackground, പൂർണ്ണസംഖ്യ width, പൂർണ്ണസംഖ്യ height)
{
    HDC hDC = GetDC(ശൂന്യ);
    HDC hMemDC = CreateCompatibleDC(hDC);
    HBITMAP hMemBmp = CreateCompatibleBitmap(hDC, width, height);
    HBITMAP hResultBmp = ശൂന്യ;
    HGDIOBJ hOrgBMP = SelectObject(hMemDC, hMemBmp);

    DrawIconEx(hMemDC, 0, 0, hIcon, width, height, 0, hBackground, DI_NORMAL);

    hResultBmp = hMemBmp;
    hMemBmp = ശൂന്യ;

    SelectObject(hMemDC, hOrgBMP);
    DeleteDC(hMemDC);
    ReleaseDC(ശൂന്യ, hDC);
    DestroyIcon(hIcon);
    തിരികെ hResultBmp;
}

//
// GETTEXT: translated messages and menu entries
//
#ifndef FEAT_GETTEXT
# define _(x)	    x
# define W_impl(x) _wcsdup(L##x)
# define W(x)	    W_impl(x)
# define set_gettext_codeset()	    ശൂന്യ
# define restore_gettext_codeset(x)
#അല്ലെങ്കിൽ
# define _(x)	    (*dyn_libintl_gettext)(x)
# define W(x)	    utf8_to_utf16(x)
# define VIMPACKAGE "vim"
# ifndef GETTEXT_DLL
#  define GETTEXT_DLL "libintl.dll"
#  define GETTEXT_DLL_ALT "libintl-8.dll"
# endif

// Dummy functions
സ്ഥിതം അക്ഷരം *null_libintl_gettext(സ്ഥിരം അക്ഷരം *);
സ്ഥിതം അക്ഷരം *null_libintl_textdomain(സ്ഥിരം അക്ഷരം *);
സ്ഥിതം അക്ഷരം *null_libintl_bindtextdomain(സ്ഥിരം അക്ഷരം *, സ്ഥിരം അക്ഷരം *);
സ്ഥിതം അക്ഷരം *null_libintl_bind_textdomain_codeset(സ്ഥിരം അക്ഷരം *, സ്ഥിരം അക്ഷരം *);
സ്ഥിതം പൂർണ്ണസംഖ്യ dyn_libintl_init(അക്ഷരം *dir);
സ്ഥിതം ശൂന്യം dyn_libintl_end(ശൂന്യം);

സ്ഥിതം HINSTANCE hLibintlDLL = 0;
സ്ഥിതം അക്ഷരം *(*dyn_libintl_gettext)(സ്ഥിരം അക്ഷരം *) = null_libintl_gettext;
സ്ഥിതം അക്ഷരം *(*dyn_libintl_textdomain)(സ്ഥിരം അക്ഷരം *) = null_libintl_textdomain;
സ്ഥിതം അക്ഷരം *(*dyn_libintl_bindtextdomain)(സ്ഥിരം അക്ഷരം *, സ്ഥിരം അക്ഷരം *)
						= null_libintl_bindtextdomain;
സ്ഥിതം അക്ഷരം *(*dyn_libintl_bind_textdomain_codeset)(സ്ഥിരം അക്ഷരം *, സ്ഥിരം അക്ഷരം *)
				       = null_libintl_bind_textdomain_codeset;

//
// Attempt to load libintl.dll.  If it doesn't work, use dummy functions.
// "dir" is the directory where the libintl.dll might be.
// Return 1 for success, 0 for failure.
//
    സ്ഥിതം പൂർണ്ണസംഖ്യ
dyn_libintl_init(അക്ഷരം *dir)
{
    പൂർണ്ണസംഖ്യ		i;
    സ്ഥിതം ഘടന
    {
	അക്ഷരം	    *name;
	FARPROC	    *ptr;
    } libintl_entry[] =
    {
	{(അക്ഷരം *)"gettext",		(FARPROC*)&dyn_libintl_gettext},
	{(അക്ഷരം *)"textdomain",		(FARPROC*)&dyn_libintl_textdomain},
	{(അക്ഷരം *)"bindtextdomain",	(FARPROC*)&dyn_libintl_bindtextdomain},
	{(അക്ഷരം *)"bind_textdomain_codeset", (FARPROC*)&dyn_libintl_bind_textdomain_codeset},
	{ശൂന്യ, ശൂന്യ}
    };
    DWORD	len, len2;
    LPWSTR	buf = ശൂന്യ;
    LPWSTR	buf2 = ശൂന്യ;

    // No need to initialize twice.
    എങ്കിൽ (hLibintlDLL)
	തിരികെ 1;

    // Load gettext library from $VIMRUNTIME\GvimExt{64,32} directory.
    // Add the directory to $PATH temporarily.
    len = GetEnvironmentVariableW(L"PATH", ശൂന്യ, 0);
    len2 = MAX_PATH + 1 + len;
    buf = (LPWSTR)malloc(len * വലുപ്പം(WCHAR));
    buf2 = (LPWSTR)malloc(len2 * വലുപ്പം(WCHAR));
    എങ്കിൽ (buf != ശൂന്യ && buf2 != ശൂന്യ)
    {
	GetEnvironmentVariableW(L"PATH", buf, len);
# ifdef _WIN64
	_snwprintf(buf2, len2, L"%S\\GvimExt64;%s", dir, buf);
# അല്ലെങ്കിൽ
	_snwprintf(buf2, len2, L"%S\\GvimExt32;%s", dir, buf);
# endif
	SetEnvironmentVariableW(L"PATH", buf2);
	hLibintlDLL = LoadLibrary(GETTEXT_DLL);
# ifdef GETTEXT_DLL_ALT
	എങ്കിൽ (!hLibintlDLL)
	    hLibintlDLL = LoadLibrary(GETTEXT_DLL_ALT);
# endif
	SetEnvironmentVariableW(L"PATH", buf);
    }
    സ്വതന്ത്രം(buf);
    സ്വതന്ത്രം(buf2);
    എങ്കിൽ (!hLibintlDLL)
	തിരികെ 0;

    // Get the addresses of the functions we need.
    വേണ്ടി (i = 0; libintl_entry[i].name != ശൂന്യ
					 && libintl_entry[i].ptr != ശൂന്യ; ++i)
    {
	എങ്കിൽ ((*libintl_entry[i].ptr = GetProcAddress(hLibintlDLL,
					      libintl_entry[i].name)) == ശൂന്യ)
	{
	    dyn_libintl_end();
	    തിരികെ 0;
	}
    }
    തിരികെ 1;
}

    സ്ഥിതം ശൂന്യം
dyn_libintl_end(ശൂന്യം)
{
    എങ്കിൽ (hLibintlDLL)
	FreeLibrary(hLibintlDLL);
    hLibintlDLL			= ശൂന്യ;
    dyn_libintl_gettext		= null_libintl_gettext;
    dyn_libintl_textdomain	= null_libintl_textdomain;
    dyn_libintl_bindtextdomain	= null_libintl_bindtextdomain;
    dyn_libintl_bind_textdomain_codeset	= null_libintl_bind_textdomain_codeset;
}

    സ്ഥിതം അക്ഷരം *
null_libintl_gettext(സ്ഥിരം അക്ഷരം *msgid)
{
    തിരികെ (അക്ഷരം *)msgid;
}

    സ്ഥിതം അക്ഷരം *
null_libintl_textdomain(സ്ഥിരം അക്ഷരം * /* domainname */)
{
    തിരികെ ശൂന്യ;
}

    സ്ഥിതം അക്ഷരം *
null_libintl_bindtextdomain(സ്ഥിരം അക്ഷരം * /* domainname */, സ്ഥിരം അക്ഷരം * /* dirname */)
{
    തിരികെ ശൂന്യ;
}

    സ്ഥിതം അക്ഷരം *
null_libintl_bind_textdomain_codeset(സ്ഥിരം അക്ഷരം * /* domainname */, സ്ഥിരം അക്ഷരം * /* codeset */)
{
    തിരികെ ശൂന്യ;
}

//
// Setup for translating strings.
//
    സ്ഥിതം ശൂന്യം
dyn_gettext_load(ശൂന്യം)
{
    അക്ഷരം    szBuff[BUFSIZE];
    DWORD   len;

    // Try to locate the runtime files.  The path is used to find libintl.dll
    // and the vim.mo files.
    getRuntimeDir(szBuff);
    എങ്കിൽ (szBuff[0] != 0)
    {
	len = (DWORD)strlen(szBuff);
	എങ്കിൽ (dyn_libintl_init(szBuff))
	{
	    strcpy(szBuff + len, "lang");

	    (*dyn_libintl_bindtextdomain)(VIMPACKAGE, szBuff);
	    (*dyn_libintl_textdomain)(VIMPACKAGE);
	}
    }
}

    സ്ഥിതം ശൂന്യം
dyn_gettext_free(ശൂന്യം)
{
    dyn_libintl_end();
}

//
// Use UTF-8 for gettext. Returns previous codeset.
//
    സ്ഥിതം അക്ഷരം *
set_gettext_codeset(ശൂന്യം)
{
    അക്ഷരം *prev = dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, ശൂന്യ);
    prev = _strdup((prev != ശൂന്യ) ? prev : "char");
    dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, "utf-8");

    തിരികെ prev;
}

//
// Restore previous codeset for gettext.
//
    സ്ഥിതം ശൂന്യം
restore_gettext_codeset(അക്ഷരം *prev)
{
    dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, prev);
    സ്വതന്ത്രം(prev);
}
#endif // FEAT_GETTEXT

//
// Global variables
//
UINT      g_cRefThisDll = 0;    // Reference count of this DLL.
HINSTANCE g_hmodThisDll = ശൂന്യ;	// Handle to this DLL itself.


//---------------------------------------------------------------------------
// DllMain
//---------------------------------------------------------------------------
ബാഹ്യ "C" പൂർണ്ണസംഖ്യ APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID  /* lpReserved */)
{
    തിരഞ്ഞെടുപ്പ് (dwReason)
    {
    അവസ്ഥ DLL_PROCESS_ATTACH:
	// Extension DLL one-time initialization
	g_hmodThisDll = hInstance;
	നിർത്തുക;

    അവസ്ഥ DLL_PROCESS_DETACH:
	നിർത്തുക;
    }

    തിരികെ 1;   // ok
}

    സ്ഥിതം ശൂന്യം
inc_cRefThisDLL()
{
#ifdef FEAT_GETTEXT
    എങ്കിൽ (g_cRefThisDll == 0)
	dyn_gettext_load();
#endif
    InterlockedIncrement((LPLONG)&g_cRefThisDll);
}

    സ്ഥിതം ശൂന്യം
dec_cRefThisDLL()
{
#ifdef FEAT_GETTEXT
    എങ്കിൽ (InterlockedDecrement((LPLONG)&g_cRefThisDll) == 0)
	dyn_gettext_free();
#അല്ലെങ്കിൽ
    InterlockedDecrement((LPLONG)&g_cRefThisDll);
#endif
}

//---------------------------------------------------------------------------
// DllCanUnloadNow
//---------------------------------------------------------------------------

STDAPI DllCanUnloadNow(ശൂന്യം)
{
    തിരികെ (g_cRefThisDll == 0 ? S_OK : S_FALSE);
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppvOut)
{
    *ppvOut = ശൂന്യ;

    എങ്കിൽ (IsEqualIID(rclsid, CLSID_ShellExtension))
    {
	CShellExtClassFactory *pcf = പുതിയ CShellExtClassFactory;

	തിരികെ pcf->QueryInterface(riid, ppvOut);
    }

    തിരികെ CLASS_E_CLASSNOTAVAILABLE;
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
    *ppv = ശൂന്യ;

    // any interface on this object is the object pointer

    എങ്കിൽ (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	*ppv = (LPCLASSFACTORY)ഇത്;

	AddRef();

	തിരികെ NOERROR;
    }

    തിരികെ E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::AddRef()
{
    തിരികെ InterlockedIncrement((LPLONG)&m_cRef);
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::Release()
{
    എങ്കിൽ (InterlockedDecrement((LPLONG)&m_cRef))
	തിരികെ m_cRef;

    delete ഇത്;

    തിരികെ 0L;
}

STDMETHODIMP CShellExtClassFactory::CreateInstance(LPUNKNOWN pUnkOuter,
						      REFIID riid,
						      LPVOID *ppvObj)
{
    *ppvObj = ശൂന്യ;

    // Shell extensions typically don't support aggregation (inheritance)

    എങ്കിൽ (pUnkOuter)
	തിരികെ CLASS_E_NOAGGREGATION;

    // Create the main shell extension object.  The shell will then call
    // QueryInterface with IID_IShellExtInit--this is how shell extensions are
    // initialized.

    LPCSHELLEXT pShellExt = പുതിയ CShellExt();  // create the CShellExt object

    എങ്കിൽ (ശൂന്യ == pShellExt)
	തിരികെ E_OUTOFMEMORY;

    തിരികെ pShellExt->QueryInterface(riid, ppvObj);
}


STDMETHODIMP CShellExtClassFactory::LockServer(BOOL  /* fLock */)
{
    തിരികെ NOERROR;
}

// *********************** CShellExt *************************
CShellExt::CShellExt()
{
    m_cRef = 0L;
    m_pDataObj = ശൂന്യ;

    inc_cRefThisDLL();

    LoadMenuIcon();
}

CShellExt::~CShellExt()
{
    എങ്കിൽ (m_pDataObj)
	m_pDataObj->Release();

    dec_cRefThisDLL();

    എങ്കിൽ (m_hVimIconBitmap)
	DeleteObject(m_hVimIconBitmap);
}

STDMETHODIMP CShellExt::QueryInterface(REFIID riid, LPVOID FAR *ppv)
{
    *ppv = ശൂന്യ;

    എങ്കിൽ (IsEqualIID(riid, IID_IShellExtInit) || IsEqualIID(riid, IID_IUnknown))
    {
	*ppv = (LPSHELLEXTINIT)ഇത്;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (IsEqualIID(riid, IID_IContextMenu))
    {
	*ppv = (LPCONTEXTMENU)ഇത്;
    }

    എങ്കിൽ (*ppv)
    {
	AddRef();

	തിരികെ NOERROR;
    }

    തിരികെ E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExt::AddRef()
{
    തിരികെ InterlockedIncrement((LPLONG)&m_cRef);
}

STDMETHODIMP_(ULONG) CShellExt::Release()
{

    എങ്കിൽ (InterlockedDecrement((LPLONG)&m_cRef))
	തിരികെ m_cRef;

    delete ഇത്;

    തിരികെ 0L;
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
    എങ്കിൽ (m_pDataObj)
	m_pDataObj->Release();

    // duplicate the object pointer and registry handle

    എങ്കിൽ (pDataObj)
    {
	m_pDataObj = pDataObj;
	pDataObj->AddRef();
    }

    തിരികെ NOERROR;
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
    എങ്കിൽ (medium.hGlobal)
	cbFiles = DragQueryFileW((HDROP)medium.hGlobal, (UINT)-1, 0, 0);

    // InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

    // Initialize m_cntOfHWnd to 0
    m_cntOfHWnd = 0;

    HKEY keyhandle;
    തർക്കം showExisting = സത്യം;
    തർക്കം showIcons = സത്യം;

    // Check whether "Edit with existing Vim" entries are disabled.
    എങ്കിൽ (Reg_Open_താക്കോൽ_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
				       താക്കോൽ_READ, &keyhandle) == ERROR_SUCCESS)
    {
	എങ്കിൽ (Reg_Query_മൂല്യം_Ex(keyhandle, "DisableEditWithExisting", 0, ശൂന്യ,
						 ശൂന്യ, ശൂന്യ) == ERROR_SUCCESS)
	    showExisting = അസത്യം;
	എങ്കിൽ (Reg_Query_മൂല്യം_Ex(keyhandle, "DisableContextMenuIcons", 0, ശൂന്യ,
						 ശൂന്യ, ശൂന്യ) == ERROR_SUCCESS)
	    showIcons = അസത്യം;
	Reg_Close_താക്കോൽ(keyhandle);
    }

    // Use UTF-8 for gettext.
    അക്ഷരം *prev = set_gettext_codeset();

    // Retrieve all the vim instances, unless disabled.
    എങ്കിൽ (showExisting)
	EnumWindows(EnumWindowsProc, (LPARAM)ഇത്);

    MENUITEMINFOW mii = { വലുപ്പം(MENUITEMINFOW) };
    mii.fMask = MIIM_STRING | MIIM_ID;
    എങ്കിൽ (showIcons)
    {
	mii.fMask |= MIIM_BITMAP;
	mii.hbmpItem = m_hVimIconBitmap;
    }

    എങ്കിൽ (cbFiles > 1)
    {
	mii.wID = idCmd++;
	mii.dwTypeData = W(_("Edit with Vim using &tabpages"));
	mii.cch = wcslen(mii.dwTypeData);
	തിരുകുക_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	സ്വതന്ത്രം(mii.dwTypeData);

	mii.wID = idCmd++;
	mii.dwTypeData = W(_("Edit with single &Vim"));
	mii.cch = wcslen(mii.dwTypeData);
	തിരുകുക_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	സ്വതന്ത്രം(mii.dwTypeData);

	എങ്കിൽ (cbFiles <= 4)
	{
	    // Can edit up to 4 files in diff mode
	    mii.wID = idCmd++;
	    mii.dwTypeData = W(_("Diff with Vim"));
	    mii.cch = wcslen(mii.dwTypeData);
	    തിരുകുക_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	    സ്വതന്ത്രം(mii.dwTypeData);
	    m_edit_existing_off = 3;
	}
	അല്ലെങ്കിൽ
	    m_edit_existing_off = 2;

    }
    അല്ലെങ്കിൽ
    {
	mii.wID = idCmd++;
	mii.dwTypeData = W(_("Edit with &Vim"));
	mii.cch = wcslen(mii.dwTypeData);
	തിരുകുക_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	സ്വതന്ത്രം(mii.dwTypeData);
	m_edit_existing_off = 1;
    }

    HMENU hSubMenu = ശൂന്യ;
    എങ്കിൽ (m_cntOfHWnd > 1)
    {
	hSubMenu = CreatePopupMenu();
	mii.fMask |= MIIM_SUBMENU;
	mii.wID = idCmd;
	mii.dwTypeData = W(_("Edit with existing Vim"));
	mii.cch = wcslen(mii.dwTypeData);
	mii.hSubMenu = hSubMenu;
	തിരുകുക_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	സ്വതന്ത്രം(mii.dwTypeData);
	mii.fMask = mii.fMask & ~MIIM_SUBMENU;
	mii.hSubMenu = ശൂന്യ;
    }
    // Now display all the vim instances
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < m_cntOfHWnd; i++)
    {
	WCHAR title[BUFSIZE];
	WCHAR temp[BUFSIZE];
	പൂർണ്ണസംഖ്യ index;
	HMENU hmenu;

	// Obtain window title, continue if can not
	എങ്കിൽ (GetWindowTextW(m_hWnd[i], title, BUFSIZE - 1) == 0)
	    തുടരുക;
	// Truncate the title before the path, keep the file name
	WCHAR *pos = wcschr(title, L'(');
	എങ്കിൽ (pos != ശൂന്യ)
	{
	    എങ്കിൽ (pos > title && pos[-1] == L' ')
		--pos;
	    *pos = 0;
	}
	// Now concatenate
	എങ്കിൽ (m_cntOfHWnd > 1)
	    temp[0] = L'\0';
	അല്ലെങ്കിൽ
	{
	    WCHAR *s = W(_("Edit with existing Vim - "));
	    wcsncpy(temp, s, BUFSIZE - 1);
	    temp[BUFSIZE - 1] = L'\0';
	    സ്വതന്ത്രം(s);
	}
	wcsncat(temp, title, BUFSIZE - 1 - wcslen(temp));
	temp[BUFSIZE - 1] = L'\0';

	mii.wID = idCmd++;
	mii.dwTypeData = temp;
	mii.cch = wcslen(mii.dwTypeData);
	എങ്കിൽ (m_cntOfHWnd > 1)
	{
	    hmenu = hSubMenu;
	    index = i;
	}
	അല്ലെങ്കിൽ
	{
	    hmenu = hMenu;
	    index = indexMenu++;
	}
	തിരുകുക_Menu_Item_W(hmenu, index, TRUE, &mii);
    }
    // InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

    // Restore previous codeset.
    restore_gettext_codeset(prev);

    // Must return number of menu items we added.
    തിരികെ ResultFromShort(idCmd-idCmdFirst);
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
    പൂർണ്ണസംഖ്യ gvimExtraOptions;

    // If HIWORD(lpcmi->lpVerb) then we have been called programmatically
    // and lpVerb is a command that should be invoked.  Otherwise, the shell
    // has called us, and LOWORD(lpcmi->lpVerb) is the menu ID the user has
    // selected.  Actually, it's (menu ID - idCmdFirst) from QueryContextMenu().
    എങ്കിൽ (!HIWORD(lpcmi->lpVerb))
    {
	UINT idCmd = LOWORD(lpcmi->lpVerb);

	എങ്കിൽ (idCmd >= m_edit_existing_off)
	{
	    // Existing with vim instance
	    hr = തള്ളുക_To_Window(lpcmi->hwnd,
		    lpcmi->lpDirectory,
		    lpcmi->lpVerb,
		    lpcmi->lpParameters,
		    lpcmi->nShow,
		    idCmd - m_edit_existing_off);
	}
	അല്ലെങ്കിൽ
	{
	    തിരഞ്ഞെടുപ്പ് (idCmd)
	    {
		അവസ്ഥ 0:
		    gvimExtraOptions = EDIT_WITH_VIM_USE_TABPAGES;
		    നിർത്തുക;
		അവസ്ഥ 1:
		    gvimExtraOptions = EDIT_WITH_VIM_NO_OPTIONS;
		    നിർത്തുക;
		അവസ്ഥ 2:
		    gvimExtraOptions = EDIT_WITH_VIM_IN_DIFF_MODE;
		    നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
		    // If execution reaches this point we likely have an
		    // inconsistency between the code that setup the menus
		    // and this code that determines what the user
		    // selected.  This should be detected and fixed during
		    // development.
		    തിരികെ E_FAIL;
	    }

            LPCMINVOKECOMMANDINFOEX lpcmiex = (LPCMINVOKECOMMANDINFOEX)lpcmi;
            LPCWSTR currentDirectory = lpcmi->cbSize == വലുപ്പം(CMINVOKECOMMANDINFOEX) ? lpcmiex->lpDirectoryW : ശൂന്യ;

	    hr = InvokeSingleGvim(lpcmi->hwnd,
		    currentDirectory,
		    lpcmi->lpVerb,
		    lpcmi->lpParameters,
		    lpcmi->nShow,
		    gvimExtraOptions);
	}
    }
    തിരികെ hr;
}

STDMETHODIMP CShellExt::തള്ളുക_To_Window(HWND  /* hParent */,
				   LPCSTR  /* pszWorkingDir */,
				   LPCSTR  /* pszCmd */,
				   LPCSTR  /* pszParam */,
				   പൂർണ്ണസംഖ്യ  /* iShowCmd */,
				   പൂർണ്ണസംഖ്യ idHWnd)
{
    HWND hWnd = m_hWnd[idHWnd];

    // Show and bring vim instance to foreground
    എങ്കിൽ (IsIconic(hWnd) != 0)
	ShowWindow(hWnd, SW_RESTORE);
    അല്ലെങ്കിൽ
	ShowWindow(hWnd, SW_SHOW);
    SetForegroundWindow(hWnd);

    // Post the selected files to the vim instance
    PostMessage(hWnd, WM_DROPFILES, (WPARAM)medium.hGlobal, 0);

    തിരികെ NOERROR;
}

STDMETHODIMP CShellExt::GetCommandString(UINT_PTR  /* idCmd */,
					 UINT uFlags,
					 UINT FAR * /* reserved */,
					 LPSTR pszName,
					 UINT cchMax)
{
    // Use UTF-8 for gettext.
    അക്ഷരം *prev = set_gettext_codeset();

    WCHAR *s = W(_("Edits the selected file(s) with Vim"));
    എങ്കിൽ (uFlags == GCS_HELPTEXTW && cchMax > wcslen(s))
	wcscpy((WCHAR *)pszName, s);
    സ്വതന്ത്രം(s);

    // Restore previous codeset.
    restore_gettext_codeset(prev);

    തിരികെ NOERROR;
}

BOOL CALLBACK CShellExt::EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    അക്ഷരം temp[BUFSIZE];

    // First do a bunch of check
    // No invisible window
    എങ്കിൽ (!IsWindowVisible(hWnd))
	തിരികെ TRUE;
    // No child window ???
    // if (GetParent(hWnd)) return TRUE;
    // Class name should be Vim, if failed to get class name, return
    എങ്കിൽ (GetClassName(hWnd, temp, ARRAY_LENGTH(temp)) == 0)
	തിരികെ TRUE;
    // Compare class name to that of vim, if not, return
    എങ്കിൽ (_strnicmp(temp, "vim", ARRAY_LENGTH("vim")) != 0)
	തിരികെ TRUE;
    // First check if the number of vim instance exceeds MAX_HWND
    CShellExt *cs = (CShellExt*) lParam;
    എങ്കിൽ (cs->m_cntOfHWnd >= MAX_HWND)
	തിരികെ FALSE;	// stop enumeration
    // Now we get the vim window, put it into some kind of array
    cs->m_hWnd[cs->m_cntOfHWnd] = hWnd;
    cs->m_cntOfHWnd ++;

    തിരികെ TRUE; // continue enumeration (otherwise this would be false)
}

BOOL CShellExt::LoadMenuIcon()
{
    അക്ഷരം vimExeFile[BUFSIZE];
    getGvimName(vimExeFile, TRUE);
    എങ്കിൽ (vimExeFile[0] == '\0')
	തിരികെ FALSE;
    HICON hVimIcon;
    എങ്കിൽ (ExtractIconEx(vimExeFile, 0, ശൂന്യ, &hVimIcon, 1) == 0)
	തിരികെ FALSE;
    m_hVimIconBitmap = IconToBitmap(hVimIcon,
	    GetSysColorBrush(COLOR_MENU),
	    GetSystemMetrics(SM_CXSMICON),
	    GetSystemMetrics(SM_CYSMICON));
    തിരികെ TRUE;
}

    സ്ഥിതം അക്ഷരം *
searchpath(അക്ഷരം *name)
{
    സ്ഥിതം സംഘം {
	അക്ഷരം	location[2 * BUFSIZE];
	WCHAR	wname[BUFSIZE];
    };
    WCHAR wlocation[BUFSIZE + 1];

    // There appears to be a bug in FindExecutableA() on Windows NT.
    // Use FindExecutableW() instead...
    MultiByteToWideChar(CP_ACP, 0, name, -1, wname, BUFSIZE);
    എങ്കിൽ (FindExecutableW(wname, L"", wlocation) > (HINSTANCE)32)
    {
	WideCharToMultiByte(CP_ACP, 0, wlocation, -1,
		location, വലുപ്പം(location), ശൂന്യ, ശൂന്യ);
	തിരികെ location;
    }
    തിരികെ (അക്ഷരം *)"";
}


STDMETHODIMP CShellExt::InvokeSingleGvim(HWND hParent,
				   LPCWSTR  workingDir,
				   LPCSTR  /* pszCmd */,
				   LPCSTR  /* pszParam */,
				   പൂർണ്ണസംഖ്യ  /* iShowCmd */,
				   പൂർണ്ണസംഖ്യ gvimExtraOptions)
{
    wchar_t	m_szFileUserClickedOn[BUFSIZE];
    wchar_t	*cmdStrW;
    size_t	cmdlen;
    size_t	len;
    UINT i;

    cmdlen = BUFSIZE;
    cmdStrW  = (wchar_t *) malloc(cmdlen * വലുപ്പം(wchar_t));
    എങ്കിൽ (cmdStrW == ശൂന്യ)
	തിരികെ E_FAIL;
    getGvimInvocationW(cmdStrW);

    എങ്കിൽ (gvimExtraOptions == EDIT_WITH_VIM_IN_DIFF_MODE)
	wcscat(cmdStrW, L" -d");
    അല്ലെങ്കിൽ എങ്കിൽ (gvimExtraOptions == EDIT_WITH_VIM_USE_TABPAGES)
	wcscat(cmdStrW, L" -p");
    വേണ്ടി (i = 0; i < cbFiles; i++)
    {
	DragQueryFileW((HDROP)medium.hGlobal,
		i,
		m_szFileUserClickedOn,
		ARRAY_LENGTH(m_szFileUserClickedOn));

	len = wcslen(cmdStrW) + wcslen(m_szFileUserClickedOn) + 4;
	എങ്കിൽ (len > cmdlen)
	{
	    cmdlen = len + BUFSIZE;
	    wchar_t *cmdStrW_new = (wchar_t *)realloc(cmdStrW, cmdlen * വലുപ്പം(wchar_t));
	    എങ്കിൽ (cmdStrW_new == ശൂന്യ)
	    {
		സ്വതന്ത്രം(cmdStrW);
		തിരികെ E_FAIL;
	    }
	    cmdStrW = cmdStrW_new;
	}
	wcscat(cmdStrW, L" \"");
	wcscat(cmdStrW, m_szFileUserClickedOn);
	wcscat(cmdStrW, L"\"");
    }

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, വലുപ്പം(si));
    si.cb = വലുപ്പം(si);

    // Start the child process.
    എങ്കിൽ (!CreateProcessW(ശൂന്യ,	// No module name (use command line).
		cmdStrW,	// Command line.
		ശൂന്യ,		// Process handle not inheritable.
		ശൂന്യ,		// Thread handle not inheritable.
		FALSE,		// Set handle inheritance to FALSE.
		0,		// No creation flags.
		ശൂന്യ,		// Use parent's environment block.
		workingDir,	// Use parent's starting directory.
		&si,		// Pointer to STARTUPINFO structure.
		&pi)		// Pointer to PROCESS_INFORMATION structure.
       )
    {
	// Use UTF-8 for gettext.
	അക്ഷരം *prev = set_gettext_codeset();

	WCHAR *സന്ദേശം = W(_("Error creating process: Check if gvim is in your path!"));
	WCHAR *title = W(_("gvimext.dll error"));

	MessageBoxW(hParent, സന്ദേശം, title, MB_OK);

	സ്വതന്ത്രം(സന്ദേശം);
	സ്വതന്ത്രം(title);

	// Restore previous codeset.
	restore_gettext_codeset(prev);
    }
    അല്ലെങ്കിൽ
    {
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
    }
    സ്വതന്ത്രം(cmdStrW);

    തിരികെ NOERROR;
}
