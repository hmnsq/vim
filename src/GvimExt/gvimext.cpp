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

#অন্তর্ভাৱ "gvimext.h"
#অন্তর্ভাৱ "../version.h"

#define ARRAY_LENGTH(a) (পরিমাণ(a) / পরিমাণ((a)[0]))

স্থির ৱর্ণ *searchpath(ৱর্ণ *name);

// Always get an error while putting the following stuff to the
// gvimext.h file as class protected variables, give up and
// declare them as global stuff
FORMATETC fmte = {CF_HDROP,
		  (DVTARGETDEVICE FAR *)রিক্ত,
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
    স্থির শূন্য
getGvimName(ৱর্ণ *name, BOOL runtime)
{
    HKEY	keyhandle;
    DWORD	hlen;

    // Get the location of gvim from the registry.  Try
    // HKEY_CURRENT_USER first, then fall back to HKEY_LOCAL_MACHINE if
    // not found.
    name[0] = 0;
    যদি (Reg_Open_চাবি_Ex(HKEY_CURRENT_USER, "Software\\Vim\\Gvim", 0,
				       চাবি_READ, &keyhandle) == ERROR_SUCCESS)
    {
	hlen = BUFSIZE;
	যদি (Reg_Query_মান_Ex(keyhandle, "path", 0, রিক্ত, (BYTE *)name, &hlen)
							     != ERROR_SUCCESS)
	    name[0] = 0;
	অন্যথা
	    name[hlen] = 0;
	Reg_Close_চাবি(keyhandle);
    }

    যদি ((name[0] == 0) &&
	    (Reg_Open_চাবি_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
				       চাবি_READ, &keyhandle) == ERROR_SUCCESS))
    {
	hlen = BUFSIZE;
	যদি (Reg_Query_মান_Ex(keyhandle, "path", 0, রিক্ত, (BYTE *)name, &hlen)
							     != ERROR_SUCCESS)
	    name[0] = 0;
	অন্যথা
	    name[hlen] = 0;
	Reg_Close_চাবি(keyhandle);
    }

    // Registry didn't work, use the search path.
    যদি (name[0] == 0)
	strcpy(name, searchpath((ৱর্ণ *)"gvim.exe"));

    যদি (!runtime)
    {
	// Only when looking for the executable, not the runtime dir, we can
	// search for the batch file or a name without a path.
	যদি (name[0] == 0)
	    strcpy(name, searchpath((ৱর্ণ *)"gvim.bat"));
	যদি (name[0] == 0)
	    strcpy(name, "gvim");	// finds gvim.bat or gvim.exe
    }
}

    স্থির শূন্য
getGvimInvocation(ৱর্ণ *name)
{
    getGvimName(name, FALSE);
    // avoid that Vim tries to expand wildcards in the file names
    strcat(name, " --literal");
}

    স্থির শূন্য
getGvimInvocationW(wchar_t *nameW)
{
    ৱর্ণ *name;

    name = (ৱর্ণ *)malloc(BUFSIZE);
    getGvimInvocation(name);
    mbstowcs(nameW, name, BUFSIZE);
    মুক্ত(name);
}

    স্থির BOOL
FileExists(LPCTSTR szPath)
{
    DWORD dwAttrib = GetFileAttributes(szPath);

    ঘূৰাই (dwAttrib != INVALID_FILE_ATTRIBUTES
	    && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

//
// Get the Vim runtime directory into buf[BUFSIZE].
// The result is empty when it failed.
// When it works, the path ends in a slash or backslash.
//
    স্থির শূন্য
getRuntimeDir(ৱর্ণ *buf)
{
    পূর্ণাঙ্ক		idx;

    getGvimName(buf, TRUE);
    যদি (buf[0] == 0)
	ঘূৰাই;

    // When no path found, use the search path to expand it.
    যদি (strchr(buf, '/') == রিক্ত && strchr(buf, '\\') == রিক্ত)
	strcpy(buf, searchpath(buf));

    // remove "gvim.exe" from the end
    বাবে (idx = (পূর্ণাঙ্ক)strlen(buf) - 1; idx >= 0; idx--)
	যদি (buf[idx] == '\\' || buf[idx] == '/')
	{
	    buf[++idx] = 0;
	    ভঙ্গ;
	}

    // Check the existence of defaults.vim
    ৱর্ণ *p = buf + idx;
    strcpy(p, "defaults.vim");
    যদি (FileExists(buf))
    {
	*p = 0;		// Cut "defaults.vim"
	ঘূৰাই;
    }
    // Check the existence of vimXX\defaults.vim
    strcpy(p, VIM_VERSION_NODOT "\\" "defaults.vim");
    যদি (FileExists(buf))
    {
	p[ARRAY_LENGTH(VIM_VERSION_NODOT)] = 0;	// Cut "defaults.vim"
	ঘূৰাই;
    }
    // Check the existence of runtime\defaults.vim
    strcpy(p, "runtime" "\\" "defaults.vim");
    যদি (FileExists(buf))
    {
	p[ARRAY_LENGTH("runtime")] = 0;		// Cut "defaults.vim"
	ঘূৰাই;
    }
    // Not found
    buf[0] = 0;
    ঘূৰাই;
}

    WCHAR *
utf8_to_utf16(ধ্রুৱ ৱর্ণ *s)
{
    পূর্ণাঙ্ক size = MultiByteToWideChar(CP_UTF8, 0, s, -1, রিক্ত, 0);
    WCHAR *buf = (WCHAR *)malloc(size * পরিমাণ(WCHAR));
    MultiByteToWideChar(CP_UTF8, 0, s, -1, buf, size);
    ঘূৰাই buf;
}

    HBITMAP
IconToBitmap(HICON hIcon, HBRUSH hBackground, পূর্ণাঙ্ক width, পূর্ণাঙ্ক height)
{
    HDC hDC = GetDC(রিক্ত);
    HDC hMemDC = CreateCompatibleDC(hDC);
    HBITMAP hMemBmp = CreateCompatibleBitmap(hDC, width, height);
    HBITMAP hResultBmp = রিক্ত;
    HGDIOBJ hOrgBMP = SelectObject(hMemDC, hMemBmp);

    DrawIconEx(hMemDC, 0, 0, hIcon, width, height, 0, hBackground, DI_NORMAL);

    hResultBmp = hMemBmp;
    hMemBmp = রিক্ত;

    SelectObject(hMemDC, hOrgBMP);
    DeleteDC(hMemDC);
    ReleaseDC(রিক্ত, hDC);
    DestroyIcon(hIcon);
    ঘূৰাই hResultBmp;
}

//
// GETTEXT: translated messages and menu entries
//
#ifndef FEAT_GETTEXT
# define _(x)	    x
# define W_impl(x) _wcsdup(L##x)
# define W(x)	    W_impl(x)
# define set_gettext_codeset()	    রিক্ত
# define restore_gettext_codeset(x)
#অন্যথা
# define _(x)	    (*dyn_libintl_gettext)(x)
# define W(x)	    utf8_to_utf16(x)
# define VIMPACKAGE "vim"
# ifndef GETTEXT_DLL
#  define GETTEXT_DLL "libintl.dll"
#  define GETTEXT_DLL_ALT "libintl-8.dll"
# endif

// Dummy functions
স্থির ৱর্ণ *null_libintl_gettext(ধ্রুৱ ৱর্ণ *);
স্থির ৱর্ণ *null_libintl_textdomain(ধ্রুৱ ৱর্ণ *);
স্থির ৱর্ণ *null_libintl_bindtextdomain(ধ্রুৱ ৱর্ণ *, ধ্রুৱ ৱর্ণ *);
স্থির ৱর্ণ *null_libintl_bind_textdomain_codeset(ধ্রুৱ ৱর্ণ *, ধ্রুৱ ৱর্ণ *);
স্থির পূর্ণাঙ্ক dyn_libintl_init(ৱর্ণ *dir);
স্থির শূন্য dyn_libintl_end(শূন্য);

স্থির HINSTANCE hLibintlDLL = 0;
স্থির ৱর্ণ *(*dyn_libintl_gettext)(ধ্রুৱ ৱর্ণ *) = null_libintl_gettext;
স্থির ৱর্ণ *(*dyn_libintl_textdomain)(ধ্রুৱ ৱর্ণ *) = null_libintl_textdomain;
স্থির ৱর্ণ *(*dyn_libintl_bindtextdomain)(ধ্রুৱ ৱর্ণ *, ধ্রুৱ ৱর্ণ *)
						= null_libintl_bindtextdomain;
স্থির ৱর্ণ *(*dyn_libintl_bind_textdomain_codeset)(ধ্রুৱ ৱর্ণ *, ধ্রুৱ ৱর্ণ *)
				       = null_libintl_bind_textdomain_codeset;

//
// Attempt to load libintl.dll.  If it doesn't work, use dummy functions.
// "dir" is the directory where the libintl.dll might be.
// Return 1 for success, 0 for failure.
//
    স্থির পূর্ণাঙ্ক
dyn_libintl_init(ৱর্ণ *dir)
{
    পূর্ণাঙ্ক		i;
    স্থির সংরচনা
    {
	ৱর্ণ	    *name;
	FARPROC	    *ptr;
    } libintl_entry[] =
    {
	{(ৱর্ণ *)"gettext",		(FARPROC*)&dyn_libintl_gettext},
	{(ৱর্ণ *)"textdomain",		(FARPROC*)&dyn_libintl_textdomain},
	{(ৱর্ণ *)"bindtextdomain",	(FARPROC*)&dyn_libintl_bindtextdomain},
	{(ৱর্ণ *)"bind_textdomain_codeset", (FARPROC*)&dyn_libintl_bind_textdomain_codeset},
	{রিক্ত, রিক্ত}
    };
    DWORD	len, len2;
    LPWSTR	buf = রিক্ত;
    LPWSTR	buf2 = রিক্ত;

    // No need to initialize twice.
    যদি (hLibintlDLL)
	ঘূৰাই 1;

    // Load gettext library from $VIMRUNTIME\GvimExt{64,32} directory.
    // Add the directory to $PATH temporarily.
    len = GetEnvironmentVariableW(L"PATH", রিক্ত, 0);
    len2 = MAX_PATH + 1 + len;
    buf = (LPWSTR)malloc(len * পরিমাণ(WCHAR));
    buf2 = (LPWSTR)malloc(len2 * পরিমাণ(WCHAR));
    যদি (buf != রিক্ত && buf2 != রিক্ত)
    {
	GetEnvironmentVariableW(L"PATH", buf, len);
# ifdef _WIN64
	_snwprintf(buf2, len2, L"%S\\GvimExt64;%s", dir, buf);
# অন্যথা
	_snwprintf(buf2, len2, L"%S\\GvimExt32;%s", dir, buf);
# endif
	SetEnvironmentVariableW(L"PATH", buf2);
	hLibintlDLL = LoadLibrary(GETTEXT_DLL);
# ifdef GETTEXT_DLL_ALT
	যদি (!hLibintlDLL)
	    hLibintlDLL = LoadLibrary(GETTEXT_DLL_ALT);
# endif
	SetEnvironmentVariableW(L"PATH", buf);
    }
    মুক্ত(buf);
    মুক্ত(buf2);
    যদি (!hLibintlDLL)
	ঘূৰাই 0;

    // Get the addresses of the functions we need.
    বাবে (i = 0; libintl_entry[i].name != রিক্ত
					 && libintl_entry[i].ptr != রিক্ত; ++i)
    {
	যদি ((*libintl_entry[i].ptr = GetProcAddress(hLibintlDLL,
					      libintl_entry[i].name)) == রিক্ত)
	{
	    dyn_libintl_end();
	    ঘূৰাই 0;
	}
    }
    ঘূৰাই 1;
}

    স্থির শূন্য
dyn_libintl_end(শূন্য)
{
    যদি (hLibintlDLL)
	FreeLibrary(hLibintlDLL);
    hLibintlDLL			= রিক্ত;
    dyn_libintl_gettext		= null_libintl_gettext;
    dyn_libintl_textdomain	= null_libintl_textdomain;
    dyn_libintl_bindtextdomain	= null_libintl_bindtextdomain;
    dyn_libintl_bind_textdomain_codeset	= null_libintl_bind_textdomain_codeset;
}

    স্থির ৱর্ণ *
null_libintl_gettext(ধ্রুৱ ৱর্ণ *msgid)
{
    ঘূৰাই (ৱর্ণ *)msgid;
}

    স্থির ৱর্ণ *
null_libintl_textdomain(ধ্রুৱ ৱর্ণ * /* domainname */)
{
    ঘূৰাই রিক্ত;
}

    স্থির ৱর্ণ *
null_libintl_bindtextdomain(ধ্রুৱ ৱর্ণ * /* domainname */, ধ্রুৱ ৱর্ণ * /* dirname */)
{
    ঘূৰাই রিক্ত;
}

    স্থির ৱর্ণ *
null_libintl_bind_textdomain_codeset(ধ্রুৱ ৱর্ণ * /* domainname */, ধ্রুৱ ৱর্ণ * /* codeset */)
{
    ঘূৰাই রিক্ত;
}

//
// Setup for translating strings.
//
    স্থির শূন্য
dyn_gettext_load(শূন্য)
{
    ৱর্ণ    szBuff[BUFSIZE];
    DWORD   len;

    // Try to locate the runtime files.  The path is used to find libintl.dll
    // and the vim.mo files.
    getRuntimeDir(szBuff);
    যদি (szBuff[0] != 0)
    {
	len = (DWORD)strlen(szBuff);
	যদি (dyn_libintl_init(szBuff))
	{
	    strcpy(szBuff + len, "lang");

	    (*dyn_libintl_bindtextdomain)(VIMPACKAGE, szBuff);
	    (*dyn_libintl_textdomain)(VIMPACKAGE);
	}
    }
}

    স্থির শূন্য
dyn_gettext_free(শূন্য)
{
    dyn_libintl_end();
}

//
// Use UTF-8 for gettext. Returns previous codeset.
//
    স্থির ৱর্ণ *
set_gettext_codeset(শূন্য)
{
    ৱর্ণ *prev = dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, রিক্ত);
    prev = _strdup((prev != রিক্ত) ? prev : "char");
    dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, "utf-8");

    ঘূৰাই prev;
}

//
// Restore previous codeset for gettext.
//
    স্থির শূন্য
restore_gettext_codeset(ৱর্ণ *prev)
{
    dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, prev);
    মুক্ত(prev);
}
#endif // FEAT_GETTEXT

//
// Global variables
//
UINT      g_cRefThisDll = 0;    // Reference count of this DLL.
HINSTANCE g_hmodThisDll = রিক্ত;	// Handle to this DLL itself.


//---------------------------------------------------------------------------
// DllMain
//---------------------------------------------------------------------------
বাহ্য "C" পূর্ণাঙ্ক APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID  /* lpReserved */)
{
    ৱিকল্প (dwReason)
    {
    পক্ষ DLL_PROCESS_ATTACH:
	// Extension DLL one-time initialization
	g_hmodThisDll = hInstance;
	ভঙ্গ;

    পক্ষ DLL_PROCESS_DETACH:
	ভঙ্গ;
    }

    ঘূৰাই 1;   // ok
}

    স্থির শূন্য
inc_cRefThisDLL()
{
#ifdef FEAT_GETTEXT
    যদি (g_cRefThisDll == 0)
	dyn_gettext_load();
#endif
    InterlockedIncrement((LPLONG)&g_cRefThisDll);
}

    স্থির শূন্য
dec_cRefThisDLL()
{
#ifdef FEAT_GETTEXT
    যদি (InterlockedDecrement((LPLONG)&g_cRefThisDll) == 0)
	dyn_gettext_free();
#অন্যথা
    InterlockedDecrement((LPLONG)&g_cRefThisDll);
#endif
}

//---------------------------------------------------------------------------
// DllCanUnloadNow
//---------------------------------------------------------------------------

STDAPI DllCanUnloadNow(শূন্য)
{
    ঘূৰাই (g_cRefThisDll == 0 ? S_OK : S_FALSE);
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppvOut)
{
    *ppvOut = রিক্ত;

    যদি (IsEqualIID(rclsid, CLSID_ShellExtension))
    {
	CShellExtClassFactory *pcf = নূতন CShellExtClassFactory;

	ঘূৰাই pcf->QueryInterface(riid, ppvOut);
    }

    ঘূৰাই CLASS_E_CLASSNOTAVAILABLE;
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
    *ppv = রিক্ত;

    // any interface on this object is the object pointer

    যদি (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	*ppv = (LPCLASSFACTORY)এতত্;

	AddRef();

	ঘূৰাই NOERROR;
    }

    ঘূৰাই E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::AddRef()
{
    ঘূৰাই InterlockedIncrement((LPLONG)&m_cRef);
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::Release()
{
    যদি (InterlockedDecrement((LPLONG)&m_cRef))
	ঘূৰাই m_cRef;

    delete এতত্;

    ঘূৰাই 0L;
}

STDMETHODIMP CShellExtClassFactory::CreateInstance(LPUNKNOWN pUnkOuter,
						      REFIID riid,
						      LPVOID *ppvObj)
{
    *ppvObj = রিক্ত;

    // Shell extensions typically don't support aggregation (inheritance)

    যদি (pUnkOuter)
	ঘূৰাই CLASS_E_NOAGGREGATION;

    // Create the main shell extension object.  The shell will then call
    // QueryInterface with IID_IShellExtInit--this is how shell extensions are
    // initialized.

    LPCSHELLEXT pShellExt = নূতন CShellExt();  // create the CShellExt object

    যদি (রিক্ত == pShellExt)
	ঘূৰাই E_OUTOFMEMORY;

    ঘূৰাই pShellExt->QueryInterface(riid, ppvObj);
}


STDMETHODIMP CShellExtClassFactory::LockServer(BOOL  /* fLock */)
{
    ঘূৰাই NOERROR;
}

// *********************** CShellExt *************************
CShellExt::CShellExt()
{
    m_cRef = 0L;
    m_pDataObj = রিক্ত;

    inc_cRefThisDLL();

    LoadMenuIcon();
}

CShellExt::~CShellExt()
{
    যদি (m_pDataObj)
	m_pDataObj->Release();

    dec_cRefThisDLL();

    যদি (m_hVimIconBitmap)
	DeleteObject(m_hVimIconBitmap);
}

STDMETHODIMP CShellExt::QueryInterface(REFIID riid, LPVOID FAR *ppv)
{
    *ppv = রিক্ত;

    যদি (IsEqualIID(riid, IID_IShellExtInit) || IsEqualIID(riid, IID_IUnknown))
    {
	*ppv = (LPSHELLEXTINIT)এতত্;
    }
    অন্যথা যদি (IsEqualIID(riid, IID_IContextMenu))
    {
	*ppv = (LPCONTEXTMENU)এতত্;
    }

    যদি (*ppv)
    {
	AddRef();

	ঘূৰাই NOERROR;
    }

    ঘূৰাই E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExt::AddRef()
{
    ঘূৰাই InterlockedIncrement((LPLONG)&m_cRef);
}

STDMETHODIMP_(ULONG) CShellExt::Release()
{

    যদি (InterlockedDecrement((LPLONG)&m_cRef))
	ঘূৰাই m_cRef;

    delete এতত্;

    ঘূৰাই 0L;
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
    যদি (m_pDataObj)
	m_pDataObj->Release();

    // duplicate the object pointer and registry handle

    যদি (pDataObj)
    {
	m_pDataObj = pDataObj;
	pDataObj->AddRef();
    }

    ঘূৰাই NOERROR;
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
    যদি (medium.hGlobal)
	cbFiles = DragQueryFileW((HDROP)medium.hGlobal, (UINT)-1, 0, 0);

    // InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

    // Initialize m_cntOfHWnd to 0
    m_cntOfHWnd = 0;

    HKEY keyhandle;
    তার্কিক showExisting = সঁচা;
    তার্কিক showIcons = সঁচা;

    // Check whether "Edit with existing Vim" entries are disabled.
    যদি (Reg_Open_চাবি_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
				       চাবি_READ, &keyhandle) == ERROR_SUCCESS)
    {
	যদি (Reg_Query_মান_Ex(keyhandle, "DisableEditWithExisting", 0, রিক্ত,
						 রিক্ত, রিক্ত) == ERROR_SUCCESS)
	    showExisting = মিছা;
	যদি (Reg_Query_মান_Ex(keyhandle, "DisableContextMenuIcons", 0, রিক্ত,
						 রিক্ত, রিক্ত) == ERROR_SUCCESS)
	    showIcons = মিছা;
	Reg_Close_চাবি(keyhandle);
    }

    // Use UTF-8 for gettext.
    ৱর্ণ *prev = set_gettext_codeset();

    // Retrieve all the vim instances, unless disabled.
    যদি (showExisting)
	EnumWindows(EnumWindowsProc, (LPARAM)এতত্);

    MENUITEMINFOW mii = { পরিমাণ(MENUITEMINFOW) };
    mii.fMask = MIIM_STRING | MIIM_ID;
    যদি (showIcons)
    {
	mii.fMask |= MIIM_BITMAP;
	mii.hbmpItem = m_hVimIconBitmap;
    }

    যদি (cbFiles > 1)
    {
	mii.wID = idCmd++;
	mii.dwTypeData = W(_("Edit with Vim using &tabpages"));
	mii.cch = wcslen(mii.dwTypeData);
	ঢোকাও_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	মুক্ত(mii.dwTypeData);

	mii.wID = idCmd++;
	mii.dwTypeData = W(_("Edit with single &Vim"));
	mii.cch = wcslen(mii.dwTypeData);
	ঢোকাও_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	মুক্ত(mii.dwTypeData);

	যদি (cbFiles <= 4)
	{
	    // Can edit up to 4 files in diff mode
	    mii.wID = idCmd++;
	    mii.dwTypeData = W(_("Diff with Vim"));
	    mii.cch = wcslen(mii.dwTypeData);
	    ঢোকাও_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	    মুক্ত(mii.dwTypeData);
	    m_edit_existing_off = 3;
	}
	অন্যথা
	    m_edit_existing_off = 2;

    }
    অন্যথা
    {
	mii.wID = idCmd++;
	mii.dwTypeData = W(_("Edit with &Vim"));
	mii.cch = wcslen(mii.dwTypeData);
	ঢোকাও_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	মুক্ত(mii.dwTypeData);
	m_edit_existing_off = 1;
    }

    HMENU hSubMenu = রিক্ত;
    যদি (m_cntOfHWnd > 1)
    {
	hSubMenu = CreatePopupMenu();
	mii.fMask |= MIIM_SUBMENU;
	mii.wID = idCmd;
	mii.dwTypeData = W(_("Edit with existing Vim"));
	mii.cch = wcslen(mii.dwTypeData);
	mii.hSubMenu = hSubMenu;
	ঢোকাও_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	মুক্ত(mii.dwTypeData);
	mii.fMask = mii.fMask & ~MIIM_SUBMENU;
	mii.hSubMenu = রিক্ত;
    }
    // Now display all the vim instances
    বাবে (পূর্ণাঙ্ক i = 0; i < m_cntOfHWnd; i++)
    {
	WCHAR title[BUFSIZE];
	WCHAR temp[BUFSIZE];
	পূর্ণাঙ্ক index;
	HMENU hmenu;

	// Obtain window title, continue if can not
	যদি (GetWindowTextW(m_hWnd[i], title, BUFSIZE - 1) == 0)
	    অনুৱর্ত;
	// Truncate the title before the path, keep the file name
	WCHAR *pos = wcschr(title, L'(');
	যদি (pos != রিক্ত)
	{
	    যদি (pos > title && pos[-1] == L' ')
		--pos;
	    *pos = 0;
	}
	// Now concatenate
	যদি (m_cntOfHWnd > 1)
	    temp[0] = L'\0';
	অন্যথা
	{
	    WCHAR *s = W(_("Edit with existing Vim - "));
	    wcsncpy(temp, s, BUFSIZE - 1);
	    temp[BUFSIZE - 1] = L'\0';
	    মুক্ত(s);
	}
	wcsncat(temp, title, BUFSIZE - 1 - wcslen(temp));
	temp[BUFSIZE - 1] = L'\0';

	mii.wID = idCmd++;
	mii.dwTypeData = temp;
	mii.cch = wcslen(mii.dwTypeData);
	যদি (m_cntOfHWnd > 1)
	{
	    hmenu = hSubMenu;
	    index = i;
	}
	অন্যথা
	{
	    hmenu = hMenu;
	    index = indexMenu++;
	}
	ঢোকাও_Menu_Item_W(hmenu, index, TRUE, &mii);
    }
    // InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

    // Restore previous codeset.
    restore_gettext_codeset(prev);

    // Must return number of menu items we added.
    ঘূৰাই ResultFromShort(idCmd-idCmdFirst);
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
    পূর্ণাঙ্ক gvimExtraOptions;

    // If HIWORD(lpcmi->lpVerb) then we have been called programmatically
    // and lpVerb is a command that should be invoked.  Otherwise, the shell
    // has called us, and LOWORD(lpcmi->lpVerb) is the menu ID the user has
    // selected.  Actually, it's (menu ID - idCmdFirst) from QueryContextMenu().
    যদি (!HIWORD(lpcmi->lpVerb))
    {
	UINT idCmd = LOWORD(lpcmi->lpVerb);

	যদি (idCmd >= m_edit_existing_off)
	{
	    // Existing with vim instance
	    hr = ঠেলো_To_Window(lpcmi->hwnd,
		    lpcmi->lpDirectory,
		    lpcmi->lpVerb,
		    lpcmi->lpParameters,
		    lpcmi->nShow,
		    idCmd - m_edit_existing_off);
	}
	অন্যথা
	{
	    ৱিকল্প (idCmd)
	    {
		পক্ষ 0:
		    gvimExtraOptions = EDIT_WITH_VIM_USE_TABPAGES;
		    ভঙ্গ;
		পক্ষ 1:
		    gvimExtraOptions = EDIT_WITH_VIM_NO_OPTIONS;
		    ভঙ্গ;
		পক্ষ 2:
		    gvimExtraOptions = EDIT_WITH_VIM_IN_DIFF_MODE;
		    ভঙ্গ;
		স্ৱভাৱ:
		    // If execution reaches this point we likely have an
		    // inconsistency between the code that setup the menus
		    // and this code that determines what the user
		    // selected.  This should be detected and fixed during
		    // development.
		    ঘূৰাই E_FAIL;
	    }

            LPCMINVOKECOMMANDINFOEX lpcmiex = (LPCMINVOKECOMMANDINFOEX)lpcmi;
            LPCWSTR currentDirectory = lpcmi->cbSize == পরিমাণ(CMINVOKECOMMANDINFOEX) ? lpcmiex->lpDirectoryW : রিক্ত;

	    hr = InvokeSingleGvim(lpcmi->hwnd,
		    currentDirectory,
		    lpcmi->lpVerb,
		    lpcmi->lpParameters,
		    lpcmi->nShow,
		    gvimExtraOptions);
	}
    }
    ঘূৰাই hr;
}

STDMETHODIMP CShellExt::ঠেলো_To_Window(HWND  /* hParent */,
				   LPCSTR  /* pszWorkingDir */,
				   LPCSTR  /* pszCmd */,
				   LPCSTR  /* pszParam */,
				   পূর্ণাঙ্ক  /* iShowCmd */,
				   পূর্ণাঙ্ক idHWnd)
{
    HWND hWnd = m_hWnd[idHWnd];

    // Show and bring vim instance to foreground
    যদি (IsIconic(hWnd) != 0)
	ShowWindow(hWnd, SW_RESTORE);
    অন্যথা
	ShowWindow(hWnd, SW_SHOW);
    SetForegroundWindow(hWnd);

    // Post the selected files to the vim instance
    PostMessage(hWnd, WM_DROPFILES, (WPARAM)medium.hGlobal, 0);

    ঘূৰাই NOERROR;
}

STDMETHODIMP CShellExt::GetCommandString(UINT_PTR  /* idCmd */,
					 UINT uFlags,
					 UINT FAR * /* reserved */,
					 LPSTR pszName,
					 UINT cchMax)
{
    // Use UTF-8 for gettext.
    ৱর্ণ *prev = set_gettext_codeset();

    WCHAR *s = W(_("Edits the selected file(s) with Vim"));
    যদি (uFlags == GCS_HELPTEXTW && cchMax > wcslen(s))
	wcscpy((WCHAR *)pszName, s);
    মুক্ত(s);

    // Restore previous codeset.
    restore_gettext_codeset(prev);

    ঘূৰাই NOERROR;
}

BOOL CALLBACK CShellExt::EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    ৱর্ণ temp[BUFSIZE];

    // First do a bunch of check
    // No invisible window
    যদি (!IsWindowVisible(hWnd))
	ঘূৰাই TRUE;
    // No child window ???
    // if (GetParent(hWnd)) return TRUE;
    // Class name should be Vim, if failed to get class name, return
    যদি (GetClassName(hWnd, temp, ARRAY_LENGTH(temp)) == 0)
	ঘূৰাই TRUE;
    // Compare class name to that of vim, if not, return
    যদি (_strnicmp(temp, "vim", ARRAY_LENGTH("vim")) != 0)
	ঘূৰাই TRUE;
    // First check if the number of vim instance exceeds MAX_HWND
    CShellExt *cs = (CShellExt*) lParam;
    যদি (cs->m_cntOfHWnd >= MAX_HWND)
	ঘূৰাই FALSE;	// stop enumeration
    // Now we get the vim window, put it into some kind of array
    cs->m_hWnd[cs->m_cntOfHWnd] = hWnd;
    cs->m_cntOfHWnd ++;

    ঘূৰাই TRUE; // continue enumeration (otherwise this would be false)
}

BOOL CShellExt::LoadMenuIcon()
{
    ৱর্ণ vimExeFile[BUFSIZE];
    getGvimName(vimExeFile, TRUE);
    যদি (vimExeFile[0] == '\0')
	ঘূৰাই FALSE;
    HICON hVimIcon;
    যদি (ExtractIconEx(vimExeFile, 0, রিক্ত, &hVimIcon, 1) == 0)
	ঘূৰাই FALSE;
    m_hVimIconBitmap = IconToBitmap(hVimIcon,
	    GetSysColorBrush(COLOR_MENU),
	    GetSystemMetrics(SM_CXSMICON),
	    GetSystemMetrics(SM_CYSMICON));
    ঘূৰাই TRUE;
}

    স্থির ৱর্ণ *
searchpath(ৱর্ণ *name)
{
    স্থির সঙ্ঘ {
	ৱর্ণ	location[2 * BUFSIZE];
	WCHAR	wname[BUFSIZE];
    };
    WCHAR wlocation[BUFSIZE + 1];

    // There appears to be a bug in FindExecutableA() on Windows NT.
    // Use FindExecutableW() instead...
    MultiByteToWideChar(CP_ACP, 0, name, -1, wname, BUFSIZE);
    যদি (FindExecutableW(wname, L"", wlocation) > (HINSTANCE)32)
    {
	WideCharToMultiByte(CP_ACP, 0, wlocation, -1,
		location, পরিমাণ(location), রিক্ত, রিক্ত);
	ঘূৰাই location;
    }
    ঘূৰাই (ৱর্ণ *)"";
}


STDMETHODIMP CShellExt::InvokeSingleGvim(HWND hParent,
				   LPCWSTR  workingDir,
				   LPCSTR  /* pszCmd */,
				   LPCSTR  /* pszParam */,
				   পূর্ণাঙ্ক  /* iShowCmd */,
				   পূর্ণাঙ্ক gvimExtraOptions)
{
    wchar_t	m_szFileUserClickedOn[BUFSIZE];
    wchar_t	*cmdStrW;
    size_t	cmdlen;
    size_t	len;
    UINT i;

    cmdlen = BUFSIZE;
    cmdStrW  = (wchar_t *) malloc(cmdlen * পরিমাণ(wchar_t));
    যদি (cmdStrW == রিক্ত)
	ঘূৰাই E_FAIL;
    getGvimInvocationW(cmdStrW);

    যদি (gvimExtraOptions == EDIT_WITH_VIM_IN_DIFF_MODE)
	wcscat(cmdStrW, L" -d");
    অন্যথা যদি (gvimExtraOptions == EDIT_WITH_VIM_USE_TABPAGES)
	wcscat(cmdStrW, L" -p");
    বাবে (i = 0; i < cbFiles; i++)
    {
	DragQueryFileW((HDROP)medium.hGlobal,
		i,
		m_szFileUserClickedOn,
		ARRAY_LENGTH(m_szFileUserClickedOn));

	len = wcslen(cmdStrW) + wcslen(m_szFileUserClickedOn) + 4;
	যদি (len > cmdlen)
	{
	    cmdlen = len + BUFSIZE;
	    wchar_t *cmdStrW_new = (wchar_t *)realloc(cmdStrW, cmdlen * পরিমাণ(wchar_t));
	    যদি (cmdStrW_new == রিক্ত)
	    {
		মুক্ত(cmdStrW);
		ঘূৰাই E_FAIL;
	    }
	    cmdStrW = cmdStrW_new;
	}
	wcscat(cmdStrW, L" \"");
	wcscat(cmdStrW, m_szFileUserClickedOn);
	wcscat(cmdStrW, L"\"");
    }

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, পরিমাণ(si));
    si.cb = পরিমাণ(si);

    // Start the child process.
    যদি (!CreateProcessW(রিক্ত,	// No module name (use command line).
		cmdStrW,	// Command line.
		রিক্ত,		// Process handle not inheritable.
		রিক্ত,		// Thread handle not inheritable.
		FALSE,		// Set handle inheritance to FALSE.
		0,		// No creation flags.
		রিক্ত,		// Use parent's environment block.
		workingDir,	// Use parent's starting directory.
		&si,		// Pointer to STARTUPINFO structure.
		&pi)		// Pointer to PROCESS_INFORMATION structure.
       )
    {
	// Use UTF-8 for gettext.
	ৱর্ণ *prev = set_gettext_codeset();

	WCHAR *বাৰ্তা = W(_("Error creating process: Check if gvim is in your path!"));
	WCHAR *title = W(_("gvimext.dll error"));

	MessageBoxW(hParent, বাৰ্তা, title, MB_OK);

	মুক্ত(বাৰ্তা);
	মুক্ত(title);

	// Restore previous codeset.
	restore_gettext_codeset(prev);
    }
    অন্যথা
    {
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
    }
    মুক্ত(cmdStrW);

    ঘূৰাই NOERROR;
}
