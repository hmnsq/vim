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

#அந்தர்பாவ "gvimext.h"
#அந்தர்பாவ "../version.h"

#define ARRAY_LENGTH(a) (அளவு(a) / அளவு((a)[0]))

நிலையான எழுத்து *searchpath(எழுத்து *name);

// Always get an error while putting the following stuff to the
// gvimext.h file as class protected variables, give up and
// declare them as global stuff
FORMATETC fmte = {CF_HDROP,
		  (DVTARGETDEVICE FAR *)வெற்று,
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
    நிலையான வெறுமை
getGvimName(எழுத்து *name, BOOL runtime)
{
    HKEY	keyhandle;
    DWORD	hlen;

    // Get the location of gvim from the registry.  Try
    // HKEY_CURRENT_USER first, then fall back to HKEY_LOCAL_MACHINE if
    // not found.
    name[0] = 0;
    எனில் (Reg_Open_சாவி_Ex(HKEY_CURRENT_USER, "Software\\Vim\\Gvim", 0,
				       சாவி_READ, &keyhandle) == ERROR_SUCCESS)
    {
	hlen = BUFSIZE;
	எனில் (Reg_Query_மதிப்பு_Ex(keyhandle, "path", 0, வெற்று, (BYTE *)name, &hlen)
							     != ERROR_SUCCESS)
	    name[0] = 0;
	இல்லையெனில்
	    name[hlen] = 0;
	Reg_Close_சாவி(keyhandle);
    }

    எனில் ((name[0] == 0) &&
	    (Reg_Open_சாவி_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
				       சாவி_READ, &keyhandle) == ERROR_SUCCESS))
    {
	hlen = BUFSIZE;
	எனில் (Reg_Query_மதிப்பு_Ex(keyhandle, "path", 0, வெற்று, (BYTE *)name, &hlen)
							     != ERROR_SUCCESS)
	    name[0] = 0;
	இல்லையெனில்
	    name[hlen] = 0;
	Reg_Close_சாவி(keyhandle);
    }

    // Registry didn't work, use the search path.
    எனில் (name[0] == 0)
	strcpy(name, searchpath((எழுத்து *)"gvim.exe"));

    எனில் (!runtime)
    {
	// Only when looking for the executable, not the runtime dir, we can
	// search for the batch file or a name without a path.
	எனில் (name[0] == 0)
	    strcpy(name, searchpath((எழுத்து *)"gvim.bat"));
	எனில் (name[0] == 0)
	    strcpy(name, "gvim");	// finds gvim.bat or gvim.exe
    }
}

    நிலையான வெறுமை
getGvimInvocation(எழுத்து *name)
{
    getGvimName(name, FALSE);
    // avoid that Vim tries to expand wildcards in the file names
    strcat(name, " --literal");
}

    நிலையான வெறுமை
getGvimInvocationW(wchar_t *nameW)
{
    எழுத்து *name;

    name = (எழுத்து *)malloc(BUFSIZE);
    getGvimInvocation(name);
    mbstowcs(nameW, name, BUFSIZE);
    விடுவி(name);
}

    நிலையான BOOL
FileExists(LPCTSTR szPath)
{
    DWORD dwAttrib = GetFileAttributes(szPath);

    திரும்பு (dwAttrib != INVALID_FILE_ATTRIBUTES
	    && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

//
// Get the Vim runtime directory into buf[BUFSIZE].
// The result is empty when it failed.
// When it works, the path ends in a slash or backslash.
//
    நிலையான வெறுமை
getRuntimeDir(எழுத்து *buf)
{
    முழு		idx;

    getGvimName(buf, TRUE);
    எனில் (buf[0] == 0)
	திரும்பு;

    // When no path found, use the search path to expand it.
    எனில் (strchr(buf, '/') == வெற்று && strchr(buf, '\\') == வெற்று)
	strcpy(buf, searchpath(buf));

    // remove "gvim.exe" from the end
    க்கு (idx = (முழு)strlen(buf) - 1; idx >= 0; idx--)
	எனில் (buf[idx] == '\\' || buf[idx] == '/')
	{
	    buf[++idx] = 0;
	    நிறுத்து;
	}

    // Check the existence of defaults.vim
    எழுத்து *p = buf + idx;
    strcpy(p, "defaults.vim");
    எனில் (FileExists(buf))
    {
	*p = 0;		// Cut "defaults.vim"
	திரும்பு;
    }
    // Check the existence of vimXX\defaults.vim
    strcpy(p, VIM_VERSION_NODOT "\\" "defaults.vim");
    எனில் (FileExists(buf))
    {
	p[ARRAY_LENGTH(VIM_VERSION_NODOT)] = 0;	// Cut "defaults.vim"
	திரும்பு;
    }
    // Check the existence of runtime\defaults.vim
    strcpy(p, "runtime" "\\" "defaults.vim");
    எனில் (FileExists(buf))
    {
	p[ARRAY_LENGTH("runtime")] = 0;		// Cut "defaults.vim"
	திரும்பு;
    }
    // Not found
    buf[0] = 0;
    திரும்பு;
}

    WCHAR *
utf8_to_utf16(மாறிலி எழுத்து *s)
{
    முழு size = MultiByteToWideChar(CP_UTF8, 0, s, -1, வெற்று, 0);
    WCHAR *buf = (WCHAR *)malloc(size * அளவு(WCHAR));
    MultiByteToWideChar(CP_UTF8, 0, s, -1, buf, size);
    திரும்பு buf;
}

    HBITMAP
IconToBitmap(HICON hIcon, HBRUSH hBackground, முழு width, முழு height)
{
    HDC hDC = GetDC(வெற்று);
    HDC hMemDC = CreateCompatibleDC(hDC);
    HBITMAP hMemBmp = CreateCompatibleBitmap(hDC, width, height);
    HBITMAP hResultBmp = வெற்று;
    HGDIOBJ hOrgBMP = SelectObject(hMemDC, hMemBmp);

    DrawIconEx(hMemDC, 0, 0, hIcon, width, height, 0, hBackground, DI_NORMAL);

    hResultBmp = hMemBmp;
    hMemBmp = வெற்று;

    SelectObject(hMemDC, hOrgBMP);
    DeleteDC(hMemDC);
    ReleaseDC(வெற்று, hDC);
    DestroyIcon(hIcon);
    திரும்பு hResultBmp;
}

//
// GETTEXT: translated messages and menu entries
//
#ifndef FEAT_GETTEXT
# define _(x)	    x
# define W_impl(x) _wcsdup(L##x)
# define W(x)	    W_impl(x)
# define set_gettext_codeset()	    வெற்று
# define restore_gettext_codeset(x)
#இல்லையெனில்
# define _(x)	    (*dyn_libintl_gettext)(x)
# define W(x)	    utf8_to_utf16(x)
# define VIMPACKAGE "vim"
# ifndef GETTEXT_DLL
#  define GETTEXT_DLL "libintl.dll"
#  define GETTEXT_DLL_ALT "libintl-8.dll"
# endif

// Dummy functions
நிலையான எழுத்து *null_libintl_gettext(மாறிலி எழுத்து *);
நிலையான எழுத்து *null_libintl_textdomain(மாறிலி எழுத்து *);
நிலையான எழுத்து *null_libintl_bindtextdomain(மாறிலி எழுத்து *, மாறிலி எழுத்து *);
நிலையான எழுத்து *null_libintl_bind_textdomain_codeset(மாறிலி எழுத்து *, மாறிலி எழுத்து *);
நிலையான முழு dyn_libintl_init(எழுத்து *dir);
நிலையான வெறுமை dyn_libintl_end(வெறுமை);

நிலையான HINSTANCE hLibintlDLL = 0;
நிலையான எழுத்து *(*dyn_libintl_gettext)(மாறிலி எழுத்து *) = null_libintl_gettext;
நிலையான எழுத்து *(*dyn_libintl_textdomain)(மாறிலி எழுத்து *) = null_libintl_textdomain;
நிலையான எழுத்து *(*dyn_libintl_bindtextdomain)(மாறிலி எழுத்து *, மாறிலி எழுத்து *)
						= null_libintl_bindtextdomain;
நிலையான எழுத்து *(*dyn_libintl_bind_textdomain_codeset)(மாறிலி எழுத்து *, மாறிலி எழுத்து *)
				       = null_libintl_bind_textdomain_codeset;

//
// Attempt to load libintl.dll.  If it doesn't work, use dummy functions.
// "dir" is the directory where the libintl.dll might be.
// Return 1 for success, 0 for failure.
//
    நிலையான முழு
dyn_libintl_init(எழுத்து *dir)
{
    முழு		i;
    நிலையான கட்டமைப்பு
    {
	எழுத்து	    *name;
	FARPROC	    *ptr;
    } libintl_entry[] =
    {
	{(எழுத்து *)"gettext",		(FARPROC*)&dyn_libintl_gettext},
	{(எழுத்து *)"textdomain",		(FARPROC*)&dyn_libintl_textdomain},
	{(எழுத்து *)"bindtextdomain",	(FARPROC*)&dyn_libintl_bindtextdomain},
	{(எழுத்து *)"bind_textdomain_codeset", (FARPROC*)&dyn_libintl_bind_textdomain_codeset},
	{வெற்று, வெற்று}
    };
    DWORD	len, len2;
    LPWSTR	buf = வெற்று;
    LPWSTR	buf2 = வெற்று;

    // No need to initialize twice.
    எனில் (hLibintlDLL)
	திரும்பு 1;

    // Load gettext library from $VIMRUNTIME\GvimExt{64,32} directory.
    // Add the directory to $PATH temporarily.
    len = GetEnvironmentVariableW(L"PATH", வெற்று, 0);
    len2 = MAX_PATH + 1 + len;
    buf = (LPWSTR)malloc(len * அளவு(WCHAR));
    buf2 = (LPWSTR)malloc(len2 * அளவு(WCHAR));
    எனில் (buf != வெற்று && buf2 != வெற்று)
    {
	GetEnvironmentVariableW(L"PATH", buf, len);
# ifdef _WIN64
	_snwprintf(buf2, len2, L"%S\\GvimExt64;%s", dir, buf);
# இல்லையெனில்
	_snwprintf(buf2, len2, L"%S\\GvimExt32;%s", dir, buf);
# endif
	SetEnvironmentVariableW(L"PATH", buf2);
	hLibintlDLL = LoadLibrary(GETTEXT_DLL);
# ifdef GETTEXT_DLL_ALT
	எனில் (!hLibintlDLL)
	    hLibintlDLL = LoadLibrary(GETTEXT_DLL_ALT);
# endif
	SetEnvironmentVariableW(L"PATH", buf);
    }
    விடுவி(buf);
    விடுவி(buf2);
    எனில் (!hLibintlDLL)
	திரும்பு 0;

    // Get the addresses of the functions we need.
    க்கு (i = 0; libintl_entry[i].name != வெற்று
					 && libintl_entry[i].ptr != வெற்று; ++i)
    {
	எனில் ((*libintl_entry[i].ptr = GetProcAddress(hLibintlDLL,
					      libintl_entry[i].name)) == வெற்று)
	{
	    dyn_libintl_end();
	    திரும்பு 0;
	}
    }
    திரும்பு 1;
}

    நிலையான வெறுமை
dyn_libintl_end(வெறுமை)
{
    எனில் (hLibintlDLL)
	FreeLibrary(hLibintlDLL);
    hLibintlDLL			= வெற்று;
    dyn_libintl_gettext		= null_libintl_gettext;
    dyn_libintl_textdomain	= null_libintl_textdomain;
    dyn_libintl_bindtextdomain	= null_libintl_bindtextdomain;
    dyn_libintl_bind_textdomain_codeset	= null_libintl_bind_textdomain_codeset;
}

    நிலையான எழுத்து *
null_libintl_gettext(மாறிலி எழுத்து *msgid)
{
    திரும்பு (எழுத்து *)msgid;
}

    நிலையான எழுத்து *
null_libintl_textdomain(மாறிலி எழுத்து * /* domainname */)
{
    திரும்பு வெற்று;
}

    நிலையான எழுத்து *
null_libintl_bindtextdomain(மாறிலி எழுத்து * /* domainname */, மாறிலி எழுத்து * /* dirname */)
{
    திரும்பு வெற்று;
}

    நிலையான எழுத்து *
null_libintl_bind_textdomain_codeset(மாறிலி எழுத்து * /* domainname */, மாறிலி எழுத்து * /* codeset */)
{
    திரும்பு வெற்று;
}

//
// Setup for translating strings.
//
    நிலையான வெறுமை
dyn_gettext_load(வெறுமை)
{
    எழுத்து    szBuff[BUFSIZE];
    DWORD   len;

    // Try to locate the runtime files.  The path is used to find libintl.dll
    // and the vim.mo files.
    getRuntimeDir(szBuff);
    எனில் (szBuff[0] != 0)
    {
	len = (DWORD)strlen(szBuff);
	எனில் (dyn_libintl_init(szBuff))
	{
	    strcpy(szBuff + len, "lang");

	    (*dyn_libintl_bindtextdomain)(VIMPACKAGE, szBuff);
	    (*dyn_libintl_textdomain)(VIMPACKAGE);
	}
    }
}

    நிலையான வெறுமை
dyn_gettext_free(வெறுமை)
{
    dyn_libintl_end();
}

//
// Use UTF-8 for gettext. Returns previous codeset.
//
    நிலையான எழுத்து *
set_gettext_codeset(வெறுமை)
{
    எழுத்து *prev = dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, வெற்று);
    prev = _strdup((prev != வெற்று) ? prev : "char");
    dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, "utf-8");

    திரும்பு prev;
}

//
// Restore previous codeset for gettext.
//
    நிலையான வெறுமை
restore_gettext_codeset(எழுத்து *prev)
{
    dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, prev);
    விடுவி(prev);
}
#endif // FEAT_GETTEXT

//
// Global variables
//
UINT      g_cRefThisDll = 0;    // Reference count of this DLL.
HINSTANCE g_hmodThisDll = வெற்று;	// Handle to this DLL itself.


//---------------------------------------------------------------------------
// DllMain
//---------------------------------------------------------------------------
வெளி "C" முழு APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID  /* lpReserved */)
{
    தேர்வு (dwReason)
    {
    நிலை DLL_PROCESS_ATTACH:
	// Extension DLL one-time initialization
	g_hmodThisDll = hInstance;
	நிறுத்து;

    நிலை DLL_PROCESS_DETACH:
	நிறுத்து;
    }

    திரும்பு 1;   // ok
}

    நிலையான வெறுமை
inc_cRefThisDLL()
{
#ifdef FEAT_GETTEXT
    எனில் (g_cRefThisDll == 0)
	dyn_gettext_load();
#endif
    InterlockedIncrement((LPLONG)&g_cRefThisDll);
}

    நிலையான வெறுமை
dec_cRefThisDLL()
{
#ifdef FEAT_GETTEXT
    எனில் (InterlockedDecrement((LPLONG)&g_cRefThisDll) == 0)
	dyn_gettext_free();
#இல்லையெனில்
    InterlockedDecrement((LPLONG)&g_cRefThisDll);
#endif
}

//---------------------------------------------------------------------------
// DllCanUnloadNow
//---------------------------------------------------------------------------

STDAPI DllCanUnloadNow(வெறுமை)
{
    திரும்பு (g_cRefThisDll == 0 ? S_OK : S_FALSE);
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppvOut)
{
    *ppvOut = வெற்று;

    எனில் (IsEqualIID(rclsid, CLSID_ShellExtension))
    {
	CShellExtClassFactory *pcf = புதிய CShellExtClassFactory;

	திரும்பு pcf->QueryInterface(riid, ppvOut);
    }

    திரும்பு CLASS_E_CLASSNOTAVAILABLE;
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
    *ppv = வெற்று;

    // any interface on this object is the object pointer

    எனில் (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	*ppv = (LPCLASSFACTORY)இது;

	AddRef();

	திரும்பு NOERROR;
    }

    திரும்பு E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::AddRef()
{
    திரும்பு InterlockedIncrement((LPLONG)&m_cRef);
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::Release()
{
    எனில் (InterlockedDecrement((LPLONG)&m_cRef))
	திரும்பு m_cRef;

    delete இது;

    திரும்பு 0L;
}

STDMETHODIMP CShellExtClassFactory::CreateInstance(LPUNKNOWN pUnkOuter,
						      REFIID riid,
						      LPVOID *ppvObj)
{
    *ppvObj = வெற்று;

    // Shell extensions typically don't support aggregation (inheritance)

    எனில் (pUnkOuter)
	திரும்பு CLASS_E_NOAGGREGATION;

    // Create the main shell extension object.  The shell will then call
    // QueryInterface with IID_IShellExtInit--this is how shell extensions are
    // initialized.

    LPCSHELLEXT pShellExt = புதிய CShellExt();  // create the CShellExt object

    எனில் (வெற்று == pShellExt)
	திரும்பு E_OUTOFMEMORY;

    திரும்பு pShellExt->QueryInterface(riid, ppvObj);
}


STDMETHODIMP CShellExtClassFactory::LockServer(BOOL  /* fLock */)
{
    திரும்பு NOERROR;
}

// *********************** CShellExt *************************
CShellExt::CShellExt()
{
    m_cRef = 0L;
    m_pDataObj = வெற்று;

    inc_cRefThisDLL();

    LoadMenuIcon();
}

CShellExt::~CShellExt()
{
    எனில் (m_pDataObj)
	m_pDataObj->Release();

    dec_cRefThisDLL();

    எனில் (m_hVimIconBitmap)
	DeleteObject(m_hVimIconBitmap);
}

STDMETHODIMP CShellExt::QueryInterface(REFIID riid, LPVOID FAR *ppv)
{
    *ppv = வெற்று;

    எனில் (IsEqualIID(riid, IID_IShellExtInit) || IsEqualIID(riid, IID_IUnknown))
    {
	*ppv = (LPSHELLEXTINIT)இது;
    }
    இல்லையெனில் எனில் (IsEqualIID(riid, IID_IContextMenu))
    {
	*ppv = (LPCONTEXTMENU)இது;
    }

    எனில் (*ppv)
    {
	AddRef();

	திரும்பு NOERROR;
    }

    திரும்பு E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExt::AddRef()
{
    திரும்பு InterlockedIncrement((LPLONG)&m_cRef);
}

STDMETHODIMP_(ULONG) CShellExt::Release()
{

    எனில் (InterlockedDecrement((LPLONG)&m_cRef))
	திரும்பு m_cRef;

    delete இது;

    திரும்பு 0L;
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
    எனில் (m_pDataObj)
	m_pDataObj->Release();

    // duplicate the object pointer and registry handle

    எனில் (pDataObj)
    {
	m_pDataObj = pDataObj;
	pDataObj->AddRef();
    }

    திரும்பு NOERROR;
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
    எனில் (medium.hGlobal)
	cbFiles = DragQueryFileW((HDROP)medium.hGlobal, (UINT)-1, 0, 0);

    // InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

    // Initialize m_cntOfHWnd to 0
    m_cntOfHWnd = 0;

    HKEY keyhandle;
    தருக்க showExisting = உண்மை;
    தருக்க showIcons = உண்மை;

    // Check whether "Edit with existing Vim" entries are disabled.
    எனில் (Reg_Open_சாவி_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
				       சாவி_READ, &keyhandle) == ERROR_SUCCESS)
    {
	எனில் (Reg_Query_மதிப்பு_Ex(keyhandle, "DisableEditWithExisting", 0, வெற்று,
						 வெற்று, வெற்று) == ERROR_SUCCESS)
	    showExisting = பொய்;
	எனில் (Reg_Query_மதிப்பு_Ex(keyhandle, "DisableContextMenuIcons", 0, வெற்று,
						 வெற்று, வெற்று) == ERROR_SUCCESS)
	    showIcons = பொய்;
	Reg_Close_சாவி(keyhandle);
    }

    // Use UTF-8 for gettext.
    எழுத்து *prev = set_gettext_codeset();

    // Retrieve all the vim instances, unless disabled.
    எனில் (showExisting)
	EnumWindows(EnumWindowsProc, (LPARAM)இது);

    MENUITEMINFOW mii = { அளவு(MENUITEMINFOW) };
    mii.fMask = MIIM_STRING | MIIM_ID;
    எனில் (showIcons)
    {
	mii.fMask |= MIIM_BITMAP;
	mii.hbmpItem = m_hVimIconBitmap;
    }

    எனில் (cbFiles > 1)
    {
	mii.wID = idCmd++;
	mii.dwTypeData = W(_("Edit with Vim using &tabpages"));
	mii.cch = wcslen(mii.dwTypeData);
	செருகு_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	விடுவி(mii.dwTypeData);

	mii.wID = idCmd++;
	mii.dwTypeData = W(_("Edit with single &Vim"));
	mii.cch = wcslen(mii.dwTypeData);
	செருகு_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	விடுவி(mii.dwTypeData);

	எனில் (cbFiles <= 4)
	{
	    // Can edit up to 4 files in diff mode
	    mii.wID = idCmd++;
	    mii.dwTypeData = W(_("Diff with Vim"));
	    mii.cch = wcslen(mii.dwTypeData);
	    செருகு_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	    விடுவி(mii.dwTypeData);
	    m_edit_existing_off = 3;
	}
	இல்லையெனில்
	    m_edit_existing_off = 2;

    }
    இல்லையெனில்
    {
	mii.wID = idCmd++;
	mii.dwTypeData = W(_("Edit with &Vim"));
	mii.cch = wcslen(mii.dwTypeData);
	செருகு_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	விடுவி(mii.dwTypeData);
	m_edit_existing_off = 1;
    }

    HMENU hSubMenu = வெற்று;
    எனில் (m_cntOfHWnd > 1)
    {
	hSubMenu = CreatePopupMenu();
	mii.fMask |= MIIM_SUBMENU;
	mii.wID = idCmd;
	mii.dwTypeData = W(_("Edit with existing Vim"));
	mii.cch = wcslen(mii.dwTypeData);
	mii.hSubMenu = hSubMenu;
	செருகு_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	விடுவி(mii.dwTypeData);
	mii.fMask = mii.fMask & ~MIIM_SUBMENU;
	mii.hSubMenu = வெற்று;
    }
    // Now display all the vim instances
    க்கு (முழு i = 0; i < m_cntOfHWnd; i++)
    {
	WCHAR title[BUFSIZE];
	WCHAR temp[BUFSIZE];
	முழு index;
	HMENU hmenu;

	// Obtain window title, continue if can not
	எனில் (GetWindowTextW(m_hWnd[i], title, BUFSIZE - 1) == 0)
	    தொடர்;
	// Truncate the title before the path, keep the file name
	WCHAR *pos = wcschr(title, L'(');
	எனில் (pos != வெற்று)
	{
	    எனில் (pos > title && pos[-1] == L' ')
		--pos;
	    *pos = 0;
	}
	// Now concatenate
	எனில் (m_cntOfHWnd > 1)
	    temp[0] = L'\0';
	இல்லையெனில்
	{
	    WCHAR *s = W(_("Edit with existing Vim - "));
	    wcsncpy(temp, s, BUFSIZE - 1);
	    temp[BUFSIZE - 1] = L'\0';
	    விடுவி(s);
	}
	wcsncat(temp, title, BUFSIZE - 1 - wcslen(temp));
	temp[BUFSIZE - 1] = L'\0';

	mii.wID = idCmd++;
	mii.dwTypeData = temp;
	mii.cch = wcslen(mii.dwTypeData);
	எனில் (m_cntOfHWnd > 1)
	{
	    hmenu = hSubMenu;
	    index = i;
	}
	இல்லையெனில்
	{
	    hmenu = hMenu;
	    index = indexMenu++;
	}
	செருகு_Menu_Item_W(hmenu, index, TRUE, &mii);
    }
    // InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

    // Restore previous codeset.
    restore_gettext_codeset(prev);

    // Must return number of menu items we added.
    திரும்பு ResultFromShort(idCmd-idCmdFirst);
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
    முழு gvimExtraOptions;

    // If HIWORD(lpcmi->lpVerb) then we have been called programmatically
    // and lpVerb is a command that should be invoked.  Otherwise, the shell
    // has called us, and LOWORD(lpcmi->lpVerb) is the menu ID the user has
    // selected.  Actually, it's (menu ID - idCmdFirst) from QueryContextMenu().
    எனில் (!HIWORD(lpcmi->lpVerb))
    {
	UINT idCmd = LOWORD(lpcmi->lpVerb);

	எனில் (idCmd >= m_edit_existing_off)
	{
	    // Existing with vim instance
	    hr = தள்ளு_To_Window(lpcmi->hwnd,
		    lpcmi->lpDirectory,
		    lpcmi->lpVerb,
		    lpcmi->lpParameters,
		    lpcmi->nShow,
		    idCmd - m_edit_existing_off);
	}
	இல்லையெனில்
	{
	    தேர்வு (idCmd)
	    {
		நிலை 0:
		    gvimExtraOptions = EDIT_WITH_VIM_USE_TABPAGES;
		    நிறுத்து;
		நிலை 1:
		    gvimExtraOptions = EDIT_WITH_VIM_NO_OPTIONS;
		    நிறுத்து;
		நிலை 2:
		    gvimExtraOptions = EDIT_WITH_VIM_IN_DIFF_MODE;
		    நிறுத்து;
		இயல்பு:
		    // If execution reaches this point we likely have an
		    // inconsistency between the code that setup the menus
		    // and this code that determines what the user
		    // selected.  This should be detected and fixed during
		    // development.
		    திரும்பு E_FAIL;
	    }

            LPCMINVOKECOMMANDINFOEX lpcmiex = (LPCMINVOKECOMMANDINFOEX)lpcmi;
            LPCWSTR currentDirectory = lpcmi->cbSize == அளவு(CMINVOKECOMMANDINFOEX) ? lpcmiex->lpDirectoryW : வெற்று;

	    hr = InvokeSingleGvim(lpcmi->hwnd,
		    currentDirectory,
		    lpcmi->lpVerb,
		    lpcmi->lpParameters,
		    lpcmi->nShow,
		    gvimExtraOptions);
	}
    }
    திரும்பு hr;
}

STDMETHODIMP CShellExt::தள்ளு_To_Window(HWND  /* hParent */,
				   LPCSTR  /* pszWorkingDir */,
				   LPCSTR  /* pszCmd */,
				   LPCSTR  /* pszParam */,
				   முழு  /* iShowCmd */,
				   முழு idHWnd)
{
    HWND hWnd = m_hWnd[idHWnd];

    // Show and bring vim instance to foreground
    எனில் (IsIconic(hWnd) != 0)
	ShowWindow(hWnd, SW_RESTORE);
    இல்லையெனில்
	ShowWindow(hWnd, SW_SHOW);
    SetForegroundWindow(hWnd);

    // Post the selected files to the vim instance
    PostMessage(hWnd, WM_DROPFILES, (WPARAM)medium.hGlobal, 0);

    திரும்பு NOERROR;
}

STDMETHODIMP CShellExt::GetCommandString(UINT_PTR  /* idCmd */,
					 UINT uFlags,
					 UINT FAR * /* reserved */,
					 LPSTR pszName,
					 UINT cchMax)
{
    // Use UTF-8 for gettext.
    எழுத்து *prev = set_gettext_codeset();

    WCHAR *s = W(_("Edits the selected file(s) with Vim"));
    எனில் (uFlags == GCS_HELPTEXTW && cchMax > wcslen(s))
	wcscpy((WCHAR *)pszName, s);
    விடுவி(s);

    // Restore previous codeset.
    restore_gettext_codeset(prev);

    திரும்பு NOERROR;
}

BOOL CALLBACK CShellExt::EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    எழுத்து temp[BUFSIZE];

    // First do a bunch of check
    // No invisible window
    எனில் (!IsWindowVisible(hWnd))
	திரும்பு TRUE;
    // No child window ???
    // if (GetParent(hWnd)) return TRUE;
    // Class name should be Vim, if failed to get class name, return
    எனில் (GetClassName(hWnd, temp, ARRAY_LENGTH(temp)) == 0)
	திரும்பு TRUE;
    // Compare class name to that of vim, if not, return
    எனில் (_strnicmp(temp, "vim", ARRAY_LENGTH("vim")) != 0)
	திரும்பு TRUE;
    // First check if the number of vim instance exceeds MAX_HWND
    CShellExt *cs = (CShellExt*) lParam;
    எனில் (cs->m_cntOfHWnd >= MAX_HWND)
	திரும்பு FALSE;	// stop enumeration
    // Now we get the vim window, put it into some kind of array
    cs->m_hWnd[cs->m_cntOfHWnd] = hWnd;
    cs->m_cntOfHWnd ++;

    திரும்பு TRUE; // continue enumeration (otherwise this would be false)
}

BOOL CShellExt::LoadMenuIcon()
{
    எழுத்து vimExeFile[BUFSIZE];
    getGvimName(vimExeFile, TRUE);
    எனில் (vimExeFile[0] == '\0')
	திரும்பு FALSE;
    HICON hVimIcon;
    எனில் (ExtractIconEx(vimExeFile, 0, வெற்று, &hVimIcon, 1) == 0)
	திரும்பு FALSE;
    m_hVimIconBitmap = IconToBitmap(hVimIcon,
	    GetSysColorBrush(COLOR_MENU),
	    GetSystemMetrics(SM_CXSMICON),
	    GetSystemMetrics(SM_CYSMICON));
    திரும்பு TRUE;
}

    நிலையான எழுத்து *
searchpath(எழுத்து *name)
{
    நிலையான ஒன்றிப்பு {
	எழுத்து	location[2 * BUFSIZE];
	WCHAR	wname[BUFSIZE];
    };
    WCHAR wlocation[BUFSIZE + 1];

    // There appears to be a bug in FindExecutableA() on Windows NT.
    // Use FindExecutableW() instead...
    MultiByteToWideChar(CP_ACP, 0, name, -1, wname, BUFSIZE);
    எனில் (FindExecutableW(wname, L"", wlocation) > (HINSTANCE)32)
    {
	WideCharToMultiByte(CP_ACP, 0, wlocation, -1,
		location, அளவு(location), வெற்று, வெற்று);
	திரும்பு location;
    }
    திரும்பு (எழுத்து *)"";
}


STDMETHODIMP CShellExt::InvokeSingleGvim(HWND hParent,
				   LPCWSTR  workingDir,
				   LPCSTR  /* pszCmd */,
				   LPCSTR  /* pszParam */,
				   முழு  /* iShowCmd */,
				   முழு gvimExtraOptions)
{
    wchar_t	m_szFileUserClickedOn[BUFSIZE];
    wchar_t	*cmdStrW;
    size_t	cmdlen;
    size_t	len;
    UINT i;

    cmdlen = BUFSIZE;
    cmdStrW  = (wchar_t *) malloc(cmdlen * அளவு(wchar_t));
    எனில் (cmdStrW == வெற்று)
	திரும்பு E_FAIL;
    getGvimInvocationW(cmdStrW);

    எனில் (gvimExtraOptions == EDIT_WITH_VIM_IN_DIFF_MODE)
	wcscat(cmdStrW, L" -d");
    இல்லையெனில் எனில் (gvimExtraOptions == EDIT_WITH_VIM_USE_TABPAGES)
	wcscat(cmdStrW, L" -p");
    க்கு (i = 0; i < cbFiles; i++)
    {
	DragQueryFileW((HDROP)medium.hGlobal,
		i,
		m_szFileUserClickedOn,
		ARRAY_LENGTH(m_szFileUserClickedOn));

	len = wcslen(cmdStrW) + wcslen(m_szFileUserClickedOn) + 4;
	எனில் (len > cmdlen)
	{
	    cmdlen = len + BUFSIZE;
	    wchar_t *cmdStrW_new = (wchar_t *)realloc(cmdStrW, cmdlen * அளவு(wchar_t));
	    எனில் (cmdStrW_new == வெற்று)
	    {
		விடுவி(cmdStrW);
		திரும்பு E_FAIL;
	    }
	    cmdStrW = cmdStrW_new;
	}
	wcscat(cmdStrW, L" \"");
	wcscat(cmdStrW, m_szFileUserClickedOn);
	wcscat(cmdStrW, L"\"");
    }

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, அளவு(si));
    si.cb = அளவு(si);

    // Start the child process.
    எனில் (!CreateProcessW(வெற்று,	// No module name (use command line).
		cmdStrW,	// Command line.
		வெற்று,		// Process handle not inheritable.
		வெற்று,		// Thread handle not inheritable.
		FALSE,		// Set handle inheritance to FALSE.
		0,		// No creation flags.
		வெற்று,		// Use parent's environment block.
		workingDir,	// Use parent's starting directory.
		&si,		// Pointer to STARTUPINFO structure.
		&pi)		// Pointer to PROCESS_INFORMATION structure.
       )
    {
	// Use UTF-8 for gettext.
	எழுத்து *prev = set_gettext_codeset();

	WCHAR *செய்தி = W(_("Error creating process: Check if gvim is in your path!"));
	WCHAR *title = W(_("gvimext.dll error"));

	MessageBoxW(hParent, செய்தி, title, MB_OK);

	விடுவி(செய்தி);
	விடுவி(title);

	// Restore previous codeset.
	restore_gettext_codeset(prev);
    }
    இல்லையெனில்
    {
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
    }
    விடுவி(cmdStrW);

    திரும்பு NOERROR;
}
