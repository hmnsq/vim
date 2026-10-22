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

#include "gvimext.h"
#include "../version.h"

#define ARRAY_LENGTH(a) (sizeof(a) / sizeof((a)[0]))

static char *searchpath(char *name);

// Always get an error while putting the following stuff to the
// gvimext.h file as class protected variables, give up and
// declare them as global stuff
FORMATETC fmte = {CF_HDROP,
		  (DVTARGETDEVICE FAR *)NULL,
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
    static void
getGvimName(char *name, BOOL runtime)
{
    HKEY	keyhandle;
    DWORD	hlen;

    // Get the location of gvim from the registry.  Try
    // HKEY_CURRENT_USER first, then fall back to HKEY_LOCAL_MACHINE if
    // not found.
    name[0] = 0;
    もし (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Vim\\Gvim", 0,
				       KEY_READ, &keyhandle) == ERROR_SUCCESS)
    {
	hlen = BUFSIZE;
	もし (RegQueryValueEx(keyhandle, "path", 0, NULL, (BYTE *)name, &hlen)
							     != ERROR_SUCCESS)
	    name[0] = 0;
	否则
	    name[hlen] = 0;
	RegCloseKey(keyhandle);
    }

    もし ((name[0] == 0) &&
	    (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
				       KEY_READ, &keyhandle) == ERROR_SUCCESS))
    {
	hlen = BUFSIZE;
	もし (RegQueryValueEx(keyhandle, "path", 0, NULL, (BYTE *)name, &hlen)
							     != ERROR_SUCCESS)
	    name[0] = 0;
	否则
	    name[hlen] = 0;
	RegCloseKey(keyhandle);
    }

    // Registry didn't work, use the search path.
    もし (name[0] == 0)
	strcpy(name, searchpath((char *)"gvim.exe"));

    もし (!runtime)
    {
	// Only when looking for the executable, not the runtime dir, we can
	// search for the batch file or a name without a path.
	もし (name[0] == 0)
	    strcpy(name, searchpath((char *)"gvim.bat"));
	もし (name[0] == 0)
	    strcpy(name, "gvim");	// finds gvim.bat or gvim.exe
    }
}

    static void
getGvimInvocation(char *name)
{
    getGvimName(name, FALSE);
    // avoid that Vim tries to expand wildcards in the file names
    strcat(name, " --literal");
}

    static void
getGvimInvocationW(wchar_t *nameW)
{
    char *name;

    name = (char *)malloc(BUFSIZE);
    getGvimInvocation(name);
    mbstowcs(nameW, name, BUFSIZE);
    free(name);
}

    static BOOL
FileExists(LPCTSTR szPath)
{
    DWORD dwAttrib = GetFileAttributes(szPath);

    戻る (dwAttrib != INVALID_FILE_ATTRIBUTES
	    && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

//
// Get the Vim runtime directory into buf[BUFSIZE].
// The result is empty when it failed.
// When it works, the path ends in a slash or backslash.
//
    static void
getRuntimeDir(char *buf)
{
    整数		idx;

    getGvimName(buf, TRUE);
    もし (buf[0] == 0)
	戻る;

    // When no path found, use the search path to expand it.
    もし (strchr(buf, '/') == NULL && strchr(buf, '\\') == NULL)
	strcpy(buf, searchpath(buf));

    // remove "gvim.exe" from the end
    ために (idx = (整数)strlen(buf) - 1; idx >= 0; idx--)
	もし (buf[idx] == '\\' || buf[idx] == '/')
	{
	    buf[++idx] = 0;
	    break;
	}

    // Check the existence of defaults.vim
    char *p = buf + idx;
    strcpy(p, "defaults.vim");
    もし (FileExists(buf))
    {
	*p = 0;		// Cut "defaults.vim"
	戻る;
    }
    // Check the existence of vimXX\defaults.vim
    strcpy(p, VIM_VERSION_NODOT "\\" "defaults.vim");
    もし (FileExists(buf))
    {
	p[ARRAY_LENGTH(VIM_VERSION_NODOT)] = 0;	// Cut "defaults.vim"
	戻る;
    }
    // Check the existence of runtime\defaults.vim
    strcpy(p, "runtime" "\\" "defaults.vim");
    もし (FileExists(buf))
    {
	p[ARRAY_LENGTH("runtime")] = 0;		// Cut "defaults.vim"
	戻る;
    }
    // Not found
    buf[0] = 0;
    戻る;
}

    WCHAR *
utf8_to_utf16(const char *s)
{
    整数 size = MultiByteToWideChar(CP_UTF8, 0, s, -1, NULL, 0);
    WCHAR *buf = (WCHAR *)malloc(size * sizeof(WCHAR));
    MultiByteToWideChar(CP_UTF8, 0, s, -1, buf, size);
    戻る buf;
}

    HBITMAP
IconToBitmap(HICON hIcon, HBRUSH hBackground, 整数 width, 整数 height)
{
    HDC hDC = GetDC(NULL);
    HDC hMemDC = CreateCompatibleDC(hDC);
    HBITMAP hMemBmp = CreateCompatibleBitmap(hDC, width, height);
    HBITMAP hResultBmp = NULL;
    HGDIOBJ hOrgBMP = SelectObject(hMemDC, hMemBmp);

    DrawIconEx(hMemDC, 0, 0, hIcon, width, height, 0, hBackground, DI_NORMAL);

    hResultBmp = hMemBmp;
    hMemBmp = NULL;

    SelectObject(hMemDC, hOrgBMP);
    DeleteDC(hMemDC);
    ReleaseDC(NULL, hDC);
    DestroyIcon(hIcon);
    戻る hResultBmp;
}

//
// GETTEXT: translated messages and menu entries
//
#ifndef FEAT_GETTEXT
# define _(x)	    x
# define W_impl(x) _wcsdup(L##x)
# define W(x)	    W_impl(x)
# define set_gettext_codeset()	    NULL
# define restore_gettext_codeset(x)
#否则
# define _(x)	    (*dyn_libintl_gettext)(x)
# define W(x)	    utf8_to_utf16(x)
# define VIMPACKAGE "vim"
# ifndef GETTEXT_DLL
#  define GETTEXT_DLL "libintl.dll"
#  define GETTEXT_DLL_ALT "libintl-8.dll"
# endif

// Dummy functions
static char *null_libintl_gettext(const char *);
static char *null_libintl_textdomain(const char *);
static char *null_libintl_bindtextdomain(const char *, const char *);
static char *null_libintl_bind_textdomain_codeset(const char *, const char *);
static 整数 dyn_libintl_init(char *dir);
static void dyn_libintl_end(void);

static HINSTANCE hLibintlDLL = 0;
static char *(*dyn_libintl_gettext)(const char *) = null_libintl_gettext;
static char *(*dyn_libintl_textdomain)(const char *) = null_libintl_textdomain;
static char *(*dyn_libintl_bindtextdomain)(const char *, const char *)
						= null_libintl_bindtextdomain;
static char *(*dyn_libintl_bind_textdomain_codeset)(const char *, const char *)
				       = null_libintl_bind_textdomain_codeset;

//
// Attempt to load libintl.dll.  If it doesn't work, use dummy functions.
// "dir" is the directory where the libintl.dll might be.
// Return 1 for success, 0 for failure.
//
    static 整数
dyn_libintl_init(char *dir)
{
    整数		i;
    static struct
    {
	char	    *name;
	FARPROC	    *ptr;
    } libintl_entry[] =
    {
	{(char *)"gettext",		(FARPROC*)&dyn_libintl_gettext},
	{(char *)"textdomain",		(FARPROC*)&dyn_libintl_textdomain},
	{(char *)"bindtextdomain",	(FARPROC*)&dyn_libintl_bindtextdomain},
	{(char *)"bind_textdomain_codeset", (FARPROC*)&dyn_libintl_bind_textdomain_codeset},
	{NULL, NULL}
    };
    DWORD	len, len2;
    LPWSTR	buf = NULL;
    LPWSTR	buf2 = NULL;

    // No need to initialize twice.
    もし (hLibintlDLL)
	戻る 1;

    // Load gettext library from $VIMRUNTIME\GvimExt{64,32} directory.
    // Add the directory to $PATH temporarily.
    len = GetEnvironmentVariableW(L"PATH", NULL, 0);
    len2 = MAX_PATH + 1 + len;
    buf = (LPWSTR)malloc(len * sizeof(WCHAR));
    buf2 = (LPWSTR)malloc(len2 * sizeof(WCHAR));
    もし (buf != NULL && buf2 != NULL)
    {
	GetEnvironmentVariableW(L"PATH", buf, len);
# ifdef _WIN64
	_snwprintf(buf2, len2, L"%S\\GvimExt64;%s", dir, buf);
# 否则
	_snwprintf(buf2, len2, L"%S\\GvimExt32;%s", dir, buf);
# endif
	SetEnvironmentVariableW(L"PATH", buf2);
	hLibintlDLL = LoadLibrary(GETTEXT_DLL);
# ifdef GETTEXT_DLL_ALT
	もし (!hLibintlDLL)
	    hLibintlDLL = LoadLibrary(GETTEXT_DLL_ALT);
# endif
	SetEnvironmentVariableW(L"PATH", buf);
    }
    free(buf);
    free(buf2);
    もし (!hLibintlDLL)
	戻る 0;

    // Get the addresses of the functions we need.
    ために (i = 0; libintl_entry[i].name != NULL
					 && libintl_entry[i].ptr != NULL; ++i)
    {
	もし ((*libintl_entry[i].ptr = GetProcAddress(hLibintlDLL,
					      libintl_entry[i].name)) == NULL)
	{
	    dyn_libintl_end();
	    戻る 0;
	}
    }
    戻る 1;
}

    static void
dyn_libintl_end(void)
{
    もし (hLibintlDLL)
	FreeLibrary(hLibintlDLL);
    hLibintlDLL			= NULL;
    dyn_libintl_gettext		= null_libintl_gettext;
    dyn_libintl_textdomain	= null_libintl_textdomain;
    dyn_libintl_bindtextdomain	= null_libintl_bindtextdomain;
    dyn_libintl_bind_textdomain_codeset	= null_libintl_bind_textdomain_codeset;
}

    static char *
null_libintl_gettext(const char *msgid)
{
    戻る (char *)msgid;
}

    static char *
null_libintl_textdomain(const char * /* domainname */)
{
    戻る NULL;
}

    static char *
null_libintl_bindtextdomain(const char * /* domainname */, const char * /* dirname */)
{
    戻る NULL;
}

    static char *
null_libintl_bind_textdomain_codeset(const char * /* domainname */, const char * /* codeset */)
{
    戻る NULL;
}

//
// Setup for translating strings.
//
    static void
dyn_gettext_load(void)
{
    char    szBuff[BUFSIZE];
    DWORD   len;

    // Try to locate the runtime files.  The path is used to find libintl.dll
    // and the vim.mo files.
    getRuntimeDir(szBuff);
    もし (szBuff[0] != 0)
    {
	len = (DWORD)strlen(szBuff);
	もし (dyn_libintl_init(szBuff))
	{
	    strcpy(szBuff + len, "lang");

	    (*dyn_libintl_bindtextdomain)(VIMPACKAGE, szBuff);
	    (*dyn_libintl_textdomain)(VIMPACKAGE);
	}
    }
}

    static void
dyn_gettext_free(void)
{
    dyn_libintl_end();
}

//
// Use UTF-8 for gettext. Returns previous codeset.
//
    static char *
set_gettext_codeset(void)
{
    char *prev = dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, NULL);
    prev = _strdup((prev != NULL) ? prev : "char");
    dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, "utf-8");

    戻る prev;
}

//
// Restore previous codeset for gettext.
//
    static void
restore_gettext_codeset(char *prev)
{
    dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, prev);
    free(prev);
}
#endif // FEAT_GETTEXT

//
// Global variables
//
UINT      g_cRefThisDll = 0;    // Reference count of this DLL.
HINSTANCE g_hmodThisDll = NULL;	// Handle to this DLL itself.


//---------------------------------------------------------------------------
// DllMain
//---------------------------------------------------------------------------
extern "C" 整数 APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID  /* lpReserved */)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
	// Extension DLL one-time initialization
	g_hmodThisDll = hInstance;
	break;

    case DLL_PROCESS_DETACH:
	break;
    }

    戻る 1;   // ok
}

    static void
inc_cRefThisDLL()
{
#ifdef FEAT_GETTEXT
    もし (g_cRefThisDll == 0)
	dyn_gettext_load();
#endif
    InterlockedIncrement((LPLONG)&g_cRefThisDll);
}

    static void
dec_cRefThisDLL()
{
#ifdef FEAT_GETTEXT
    もし (InterlockedDecrement((LPLONG)&g_cRefThisDll) == 0)
	dyn_gettext_free();
#否则
    InterlockedDecrement((LPLONG)&g_cRefThisDll);
#endif
}

//---------------------------------------------------------------------------
// DllCanUnloadNow
//---------------------------------------------------------------------------

STDAPI DllCanUnloadNow(void)
{
    戻る (g_cRefThisDll == 0 ? S_OK : S_FALSE);
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppvOut)
{
    *ppvOut = NULL;

    もし (IsEqualIID(rclsid, CLSID_ShellExtension))
    {
	CShellExtClassFactory *pcf = new CShellExtClassFactory;

	戻る pcf->QueryInterface(riid, ppvOut);
    }

    戻る CLASS_E_CLASSNOTAVAILABLE;
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
    *ppv = NULL;

    // any interface on this object is the object pointer

    もし (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	*ppv = (LPCLASSFACTORY)this;

	AddRef();

	戻る NOERROR;
    }

    戻る E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::AddRef()
{
    戻る InterlockedIncrement((LPLONG)&m_cRef);
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::Release()
{
    もし (InterlockedDecrement((LPLONG)&m_cRef))
	戻る m_cRef;

    delete this;

    戻る 0L;
}

STDMETHODIMP CShellExtClassFactory::CreateInstance(LPUNKNOWN pUnkOuter,
						      REFIID riid,
						      LPVOID *ppvObj)
{
    *ppvObj = NULL;

    // Shell extensions typically don't support aggregation (inheritance)

    もし (pUnkOuter)
	戻る CLASS_E_NOAGGREGATION;

    // Create the main shell extension object.  The shell will then call
    // QueryInterface with IID_IShellExtInit--this is how shell extensions are
    // initialized.

    LPCSHELLEXT pShellExt = new CShellExt();  // create the CShellExt object

    もし (NULL == pShellExt)
	戻る E_OUTOFMEMORY;

    戻る pShellExt->QueryInterface(riid, ppvObj);
}


STDMETHODIMP CShellExtClassFactory::LockServer(BOOL  /* fLock */)
{
    戻る NOERROR;
}

// *********************** CShellExt *************************
CShellExt::CShellExt()
{
    m_cRef = 0L;
    m_pDataObj = NULL;

    inc_cRefThisDLL();

    LoadMenuIcon();
}

CShellExt::~CShellExt()
{
    もし (m_pDataObj)
	m_pDataObj->Release();

    dec_cRefThisDLL();

    もし (m_hVimIconBitmap)
	DeleteObject(m_hVimIconBitmap);
}

STDMETHODIMP CShellExt::QueryInterface(REFIID riid, LPVOID FAR *ppv)
{
    *ppv = NULL;

    もし (IsEqualIID(riid, IID_IShellExtInit) || IsEqualIID(riid, IID_IUnknown))
    {
	*ppv = (LPSHELLEXTINIT)this;
    }
    否则 もし (IsEqualIID(riid, IID_IContextMenu))
    {
	*ppv = (LPCONTEXTMENU)this;
    }

    もし (*ppv)
    {
	AddRef();

	戻る NOERROR;
    }

    戻る E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExt::AddRef()
{
    戻る InterlockedIncrement((LPLONG)&m_cRef);
}

STDMETHODIMP_(ULONG) CShellExt::Release()
{

    もし (InterlockedDecrement((LPLONG)&m_cRef))
	戻る m_cRef;

    delete this;

    戻る 0L;
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
    もし (m_pDataObj)
	m_pDataObj->Release();

    // duplicate the object pointer and registry handle

    もし (pDataObj)
    {
	m_pDataObj = pDataObj;
	pDataObj->AddRef();
    }

    戻る NOERROR;
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
    もし (medium.hGlobal)
	cbFiles = DragQueryFileW((HDROP)medium.hGlobal, (UINT)-1, 0, 0);

    // InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

    // Initialize m_cntOfHWnd to 0
    m_cntOfHWnd = 0;

    HKEY keyhandle;
    bool showExisting = true;
    bool showIcons = true;

    // Check whether "Edit with existing Vim" entries are disabled.
    もし (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
				       KEY_READ, &keyhandle) == ERROR_SUCCESS)
    {
	もし (RegQueryValueEx(keyhandle, "DisableEditWithExisting", 0, NULL,
						 NULL, NULL) == ERROR_SUCCESS)
	    showExisting = false;
	もし (RegQueryValueEx(keyhandle, "DisableContextMenuIcons", 0, NULL,
						 NULL, NULL) == ERROR_SUCCESS)
	    showIcons = false;
	RegCloseKey(keyhandle);
    }

    // Use UTF-8 for gettext.
    char *prev = set_gettext_codeset();

    // Retrieve all the vim instances, unless disabled.
    もし (showExisting)
	EnumWindows(EnumWindowsProc, (LPARAM)this);

    MENUITEMINFOW mii = { sizeof(MENUITEMINFOW) };
    mii.fMask = MIIM_STRING | MIIM_ID;
    もし (showIcons)
    {
	mii.fMask |= MIIM_BITMAP;
	mii.hbmpItem = m_hVimIconBitmap;
    }

    もし (cbFiles > 1)
    {
	mii.wID = idCmd++;
	mii.dwTypeData = W(_("Edit with Vim using &tabpages"));
	mii.cch = wcslen(mii.dwTypeData);
	InsertMenuItemW(hMenu, indexMenu++, TRUE, &mii);
	free(mii.dwTypeData);

	mii.wID = idCmd++;
	mii.dwTypeData = W(_("Edit with single &Vim"));
	mii.cch = wcslen(mii.dwTypeData);
	InsertMenuItemW(hMenu, indexMenu++, TRUE, &mii);
	free(mii.dwTypeData);

	もし (cbFiles <= 4)
	{
	    // Can edit up to 4 files in diff mode
	    mii.wID = idCmd++;
	    mii.dwTypeData = W(_("Diff with Vim"));
	    mii.cch = wcslen(mii.dwTypeData);
	    InsertMenuItemW(hMenu, indexMenu++, TRUE, &mii);
	    free(mii.dwTypeData);
	    m_edit_existing_off = 3;
	}
	否则
	    m_edit_existing_off = 2;

    }
    否则
    {
	mii.wID = idCmd++;
	mii.dwTypeData = W(_("Edit with &Vim"));
	mii.cch = wcslen(mii.dwTypeData);
	InsertMenuItemW(hMenu, indexMenu++, TRUE, &mii);
	free(mii.dwTypeData);
	m_edit_existing_off = 1;
    }

    HMENU hSubMenu = NULL;
    もし (m_cntOfHWnd > 1)
    {
	hSubMenu = CreatePopupMenu();
	mii.fMask |= MIIM_SUBMENU;
	mii.wID = idCmd;
	mii.dwTypeData = W(_("Edit with existing Vim"));
	mii.cch = wcslen(mii.dwTypeData);
	mii.hSubMenu = hSubMenu;
	InsertMenuItemW(hMenu, indexMenu++, TRUE, &mii);
	free(mii.dwTypeData);
	mii.fMask = mii.fMask & ~MIIM_SUBMENU;
	mii.hSubMenu = NULL;
    }
    // Now display all the vim instances
    ために (整数 i = 0; i < m_cntOfHWnd; i++)
    {
	WCHAR title[BUFSIZE];
	WCHAR temp[BUFSIZE];
	整数 index;
	HMENU hmenu;

	// Obtain window title, continue if can not
	もし (GetWindowTextW(m_hWnd[i], title, BUFSIZE - 1) == 0)
	    continue;
	// Truncate the title before the path, keep the file name
	WCHAR *pos = wcschr(title, L'(');
	もし (pos != NULL)
	{
	    もし (pos > title && pos[-1] == L' ')
		--pos;
	    *pos = 0;
	}
	// Now concatenate
	もし (m_cntOfHWnd > 1)
	    temp[0] = L'\0';
	否则
	{
	    WCHAR *s = W(_("Edit with existing Vim - "));
	    wcsncpy(temp, s, BUFSIZE - 1);
	    temp[BUFSIZE - 1] = L'\0';
	    free(s);
	}
	wcsncat(temp, title, BUFSIZE - 1 - wcslen(temp));
	temp[BUFSIZE - 1] = L'\0';

	mii.wID = idCmd++;
	mii.dwTypeData = temp;
	mii.cch = wcslen(mii.dwTypeData);
	もし (m_cntOfHWnd > 1)
	{
	    hmenu = hSubMenu;
	    index = i;
	}
	否则
	{
	    hmenu = hMenu;
	    index = indexMenu++;
	}
	InsertMenuItemW(hmenu, index, TRUE, &mii);
    }
    // InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

    // Restore previous codeset.
    restore_gettext_codeset(prev);

    // Must return number of menu items we added.
    戻る ResultFromShort(idCmd-idCmdFirst);
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
    整数 gvimExtraOptions;

    // If HIWORD(lpcmi->lpVerb) then we have been called programmatically
    // and lpVerb is a command that should be invoked.  Otherwise, the shell
    // has called us, and LOWORD(lpcmi->lpVerb) is the menu ID the user has
    // selected.  Actually, it's (menu ID - idCmdFirst) from QueryContextMenu().
    もし (!HIWORD(lpcmi->lpVerb))
    {
	UINT idCmd = LOWORD(lpcmi->lpVerb);

	もし (idCmd >= m_edit_existing_off)
	{
	    // Existing with vim instance
	    hr = PushToWindow(lpcmi->hwnd,
		    lpcmi->lpDirectory,
		    lpcmi->lpVerb,
		    lpcmi->lpParameters,
		    lpcmi->nShow,
		    idCmd - m_edit_existing_off);
	}
	否则
	{
	    switch (idCmd)
	    {
		case 0:
		    gvimExtraOptions = EDIT_WITH_VIM_USE_TABPAGES;
		    break;
		case 1:
		    gvimExtraOptions = EDIT_WITH_VIM_NO_OPTIONS;
		    break;
		case 2:
		    gvimExtraOptions = EDIT_WITH_VIM_IN_DIFF_MODE;
		    break;
		default:
		    // If execution reaches this point we likely have an
		    // inconsistency between the code that setup the menus
		    // and this code that determines what the user
		    // selected.  This should be detected and fixed during
		    // development.
		    戻る E_FAIL;
	    }

            LPCMINVOKECOMMANDINFOEX lpcmiex = (LPCMINVOKECOMMANDINFOEX)lpcmi;
            LPCWSTR currentDirectory = lpcmi->cbSize == sizeof(CMINVOKECOMMANDINFOEX) ? lpcmiex->lpDirectoryW : NULL;

	    hr = InvokeSingleGvim(lpcmi->hwnd,
		    currentDirectory,
		    lpcmi->lpVerb,
		    lpcmi->lpParameters,
		    lpcmi->nShow,
		    gvimExtraOptions);
	}
    }
    戻る hr;
}

STDMETHODIMP CShellExt::PushToWindow(HWND  /* hParent */,
				   LPCSTR  /* pszWorkingDir */,
				   LPCSTR  /* pszCmd */,
				   LPCSTR  /* pszParam */,
				   整数  /* iShowCmd */,
				   整数 idHWnd)
{
    HWND hWnd = m_hWnd[idHWnd];

    // Show and bring vim instance to foreground
    もし (IsIconic(hWnd) != 0)
	ShowWindow(hWnd, SW_RESTORE);
    否则
	ShowWindow(hWnd, SW_SHOW);
    SetForegroundWindow(hWnd);

    // Post the selected files to the vim instance
    PostMessage(hWnd, WM_DROPFILES, (WPARAM)medium.hGlobal, 0);

    戻る NOERROR;
}

STDMETHODIMP CShellExt::GetCommandString(UINT_PTR  /* idCmd */,
					 UINT uFlags,
					 UINT FAR * /* reserved */,
					 LPSTR pszName,
					 UINT cchMax)
{
    // Use UTF-8 for gettext.
    char *prev = set_gettext_codeset();

    WCHAR *s = W(_("Edits the selected file(s) with Vim"));
    もし (uFlags == GCS_HELPTEXTW && cchMax > wcslen(s))
	wcscpy((WCHAR *)pszName, s);
    free(s);

    // Restore previous codeset.
    restore_gettext_codeset(prev);

    戻る NOERROR;
}

BOOL CALLBACK CShellExt::EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    char temp[BUFSIZE];

    // First do a bunch of check
    // No invisible window
    もし (!IsWindowVisible(hWnd))
	戻る TRUE;
    // No child window ???
    // if (GetParent(hWnd)) return TRUE;
    // Class name should be Vim, if failed to get class name, return
    もし (GetClassName(hWnd, temp, ARRAY_LENGTH(temp)) == 0)
	戻る TRUE;
    // Compare class name to that of vim, if not, return
    もし (_strnicmp(temp, "vim", ARRAY_LENGTH("vim")) != 0)
	戻る TRUE;
    // First check if the number of vim instance exceeds MAX_HWND
    CShellExt *cs = (CShellExt*) lParam;
    もし (cs->m_cntOfHWnd >= MAX_HWND)
	戻る FALSE;	// stop enumeration
    // Now we get the vim window, put it into some kind of array
    cs->m_hWnd[cs->m_cntOfHWnd] = hWnd;
    cs->m_cntOfHWnd ++;

    戻る TRUE; // continue enumeration (otherwise this would be false)
}

BOOL CShellExt::LoadMenuIcon()
{
    char vimExeFile[BUFSIZE];
    getGvimName(vimExeFile, TRUE);
    もし (vimExeFile[0] == '\0')
	戻る FALSE;
    HICON hVimIcon;
    もし (ExtractIconEx(vimExeFile, 0, NULL, &hVimIcon, 1) == 0)
	戻る FALSE;
    m_hVimIconBitmap = IconToBitmap(hVimIcon,
	    GetSysColorBrush(COLOR_MENU),
	    GetSystemMetrics(SM_CXSMICON),
	    GetSystemMetrics(SM_CYSMICON));
    戻る TRUE;
}

    static char *
searchpath(char *name)
{
    static union {
	char	location[2 * BUFSIZE];
	WCHAR	wname[BUFSIZE];
    };
    WCHAR wlocation[BUFSIZE + 1];

    // There appears to be a bug in FindExecutableA() on Windows NT.
    // Use FindExecutableW() instead...
    MultiByteToWideChar(CP_ACP, 0, name, -1, wname, BUFSIZE);
    もし (FindExecutableW(wname, L"", wlocation) > (HINSTANCE)32)
    {
	WideCharToMultiByte(CP_ACP, 0, wlocation, -1,
		location, sizeof(location), NULL, NULL);
	戻る location;
    }
    戻る (char *)"";
}


STDMETHODIMP CShellExt::InvokeSingleGvim(HWND hParent,
				   LPCWSTR  workingDir,
				   LPCSTR  /* pszCmd */,
				   LPCSTR  /* pszParam */,
				   整数  /* iShowCmd */,
				   整数 gvimExtraOptions)
{
    wchar_t	m_szFileUserClickedOn[BUFSIZE];
    wchar_t	*cmdStrW;
    size_t	cmdlen;
    size_t	len;
    UINT i;

    cmdlen = BUFSIZE;
    cmdStrW  = (wchar_t *) malloc(cmdlen * sizeof(wchar_t));
    もし (cmdStrW == NULL)
	戻る E_FAIL;
    getGvimInvocationW(cmdStrW);

    もし (gvimExtraOptions == EDIT_WITH_VIM_IN_DIFF_MODE)
	wcscat(cmdStrW, L" -d");
    否则 もし (gvimExtraOptions == EDIT_WITH_VIM_USE_TABPAGES)
	wcscat(cmdStrW, L" -p");
    ために (i = 0; i < cbFiles; i++)
    {
	DragQueryFileW((HDROP)medium.hGlobal,
		i,
		m_szFileUserClickedOn,
		ARRAY_LENGTH(m_szFileUserClickedOn));

	len = wcslen(cmdStrW) + wcslen(m_szFileUserClickedOn) + 4;
	もし (len > cmdlen)
	{
	    cmdlen = len + BUFSIZE;
	    wchar_t *cmdStrW_new = (wchar_t *)realloc(cmdStrW, cmdlen * sizeof(wchar_t));
	    もし (cmdStrW_new == NULL)
	    {
		free(cmdStrW);
		戻る E_FAIL;
	    }
	    cmdStrW = cmdStrW_new;
	}
	wcscat(cmdStrW, L" \"");
	wcscat(cmdStrW, m_szFileUserClickedOn);
	wcscat(cmdStrW, L"\"");
    }

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    // Start the child process.
    もし (!CreateProcessW(NULL,	// No module name (use command line).
		cmdStrW,	// Command line.
		NULL,		// Process handle not inheritable.
		NULL,		// Thread handle not inheritable.
		FALSE,		// Set handle inheritance to FALSE.
		0,		// No creation flags.
		NULL,		// Use parent's environment block.
		workingDir,	// Use parent's starting directory.
		&si,		// Pointer to STARTUPINFO structure.
		&pi)		// Pointer to PROCESS_INFORMATION structure.
       )
    {
	// Use UTF-8 for gettext.
	char *prev = set_gettext_codeset();

	WCHAR *msg = W(_("Error creating process: Check if gvim is in your path!"));
	WCHAR *title = W(_("gvimext.dll error"));

	MessageBoxW(hParent, msg, title, MB_OK);

	free(msg);
	free(title);

	// Restore previous codeset.
	restore_gettext_codeset(prev);
    }
    否则
    {
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
    }
    free(cmdStrW);

    戻る NOERROR;
}
