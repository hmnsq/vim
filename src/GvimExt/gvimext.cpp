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

#شامل "gvimext.h"
#شامل "../version.h"

#define ARRAY_LENGTH(a) (اندازه(a) / اندازه((a)[0]))

ایستا نویسه *searchpath(نویسه *name);

// Always get an error while putting the following stuff to the
// gvimext.h file as class protected variables, give up and
// declare them as global stuff
FORMATETC fmte = {CF_HDROP,
		  (DVTARGETDEVICE FAR *)پوچ,
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
    ایستا تهی
getGvimName(نویسه *name, BOOL runtime)
{
    HKEY	keyhandle;
    DWORD	hlen;

    // Get the location of gvim from the registry.  Try
    // HKEY_CURRENT_USER first, then fall back to HKEY_LOCAL_MACHINE if
    // not found.
    name[0] = 0;
    اگر (Reg_Open_کلید_Ex(HKEY_CURRENT_USER, "Software\\Vim\\Gvim", 0,
				       کلید_READ, &keyhandle) == ERROR_SUCCESS)
    {
	hlen = BUFSIZE;
	اگر (Reg_Query_مقدار_Ex(keyhandle, "path", 0, پوچ, (BYTE *)name, &hlen)
							     != ERROR_SUCCESS)
	    name[0] = 0;
	وگرنه
	    name[hlen] = 0;
	Reg_Close_کلید(keyhandle);
    }

    اگر ((name[0] == 0) &&
	    (Reg_Open_کلید_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
				       کلید_READ, &keyhandle) == ERROR_SUCCESS))
    {
	hlen = BUFSIZE;
	اگر (Reg_Query_مقدار_Ex(keyhandle, "path", 0, پوچ, (BYTE *)name, &hlen)
							     != ERROR_SUCCESS)
	    name[0] = 0;
	وگرنه
	    name[hlen] = 0;
	Reg_Close_کلید(keyhandle);
    }

    // Registry didn't work, use the search path.
    اگر (name[0] == 0)
	strcpy(name, searchpath((نویسه *)"gvim.exe"));

    اگر (!runtime)
    {
	// Only when looking for the executable, not the runtime dir, we can
	// search for the batch file or a name without a path.
	اگر (name[0] == 0)
	    strcpy(name, searchpath((نویسه *)"gvim.bat"));
	اگر (name[0] == 0)
	    strcpy(name, "gvim");	// finds gvim.bat or gvim.exe
    }
}

    ایستا تهی
getGvimInvocation(نویسه *name)
{
    getGvimName(name, FALSE);
    // avoid that Vim tries to expand wildcards in the file names
    strcat(name, " --literal");
}

    ایستا تهی
getGvimInvocationW(wchar_t *nameW)
{
    نویسه *name;

    name = (نویسه *)malloc(BUFSIZE);
    getGvimInvocation(name);
    mbstowcs(nameW, name, BUFSIZE);
    آزاد(name);
}

    ایستا BOOL
FileExists(LPCTSTR szPath)
{
    DWORD dwAttrib = GetFileAttributes(szPath);

    بازگشت (dwAttrib != INVALID_FILE_ATTRIBUTES
	    && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

//
// Get the Vim runtime directory into buf[BUFSIZE].
// The result is empty when it failed.
// When it works, the path ends in a slash or backslash.
//
    ایستا تهی
getRuntimeDir(نویسه *buf)
{
    صحیح		idx;

    getGvimName(buf, TRUE);
    اگر (buf[0] == 0)
	بازگشت;

    // When no path found, use the search path to expand it.
    اگر (strchr(buf, '/') == پوچ && strchr(buf, '\\') == پوچ)
	strcpy(buf, searchpath(buf));

    // remove "gvim.exe" from the end
    برای (idx = (صحیح)strlen(buf) - 1; idx >= 0; idx--)
	اگر (buf[idx] == '\\' || buf[idx] == '/')
	{
	    buf[++idx] = 0;
	    بشکن;
	}

    // Check the existence of defaults.vim
    نویسه *p = buf + idx;
    strcpy(p, "defaults.vim");
    اگر (FileExists(buf))
    {
	*p = 0;		// Cut "defaults.vim"
	بازگشت;
    }
    // Check the existence of vimXX\defaults.vim
    strcpy(p, VIM_VERSION_NODOT "\\" "defaults.vim");
    اگر (FileExists(buf))
    {
	p[ARRAY_LENGTH(VIM_VERSION_NODOT)] = 0;	// Cut "defaults.vim"
	بازگشت;
    }
    // Check the existence of runtime\defaults.vim
    strcpy(p, "runtime" "\\" "defaults.vim");
    اگر (FileExists(buf))
    {
	p[ARRAY_LENGTH("runtime")] = 0;		// Cut "defaults.vim"
	بازگشت;
    }
    // Not found
    buf[0] = 0;
    بازگشت;
}

    WCHAR *
utf8_to_utf16(ثابت نویسه *s)
{
    صحیح size = MultiByteToWideChar(CP_UTF8, 0, s, -1, پوچ, 0);
    WCHAR *buf = (WCHAR *)malloc(size * اندازه(WCHAR));
    MultiByteToWideChar(CP_UTF8, 0, s, -1, buf, size);
    بازگشت buf;
}

    HBITMAP
IconToBitmap(HICON hIcon, HBRUSH hBackground, صحیح width, صحیح height)
{
    HDC hDC = GetDC(پوچ);
    HDC hMemDC = CreateCompatibleDC(hDC);
    HBITMAP hMemBmp = CreateCompatibleBitmap(hDC, width, height);
    HBITMAP hResultBmp = پوچ;
    HGDIOBJ hOrgBMP = SelectObject(hMemDC, hMemBmp);

    DrawIconEx(hMemDC, 0, 0, hIcon, width, height, 0, hBackground, DI_NORMAL);

    hResultBmp = hMemBmp;
    hMemBmp = پوچ;

    SelectObject(hMemDC, hOrgBMP);
    DeleteDC(hMemDC);
    ReleaseDC(پوچ, hDC);
    DestroyIcon(hIcon);
    بازگشت hResultBmp;
}

//
// GETTEXT: translated messages and menu entries
//
#ifndef FEAT_GETTEXT
# define _(x)	    x
# define W_impl(x) _wcsdup(L##x)
# define W(x)	    W_impl(x)
# define set_gettext_codeset()	    پوچ
# define restore_gettext_codeset(x)
#وگرنه
# define _(x)	    (*dyn_libintl_gettext)(x)
# define W(x)	    utf8_to_utf16(x)
# define VIMPACKAGE "vim"
# ifndef GETTEXT_DLL
#  define GETTEXT_DLL "libintl.dll"
#  define GETTEXT_DLL_ALT "libintl-8.dll"
# endif

// Dummy functions
ایستا نویسه *null_libintl_gettext(ثابت نویسه *);
ایستا نویسه *null_libintl_textdomain(ثابت نویسه *);
ایستا نویسه *null_libintl_bindtextdomain(ثابت نویسه *, ثابت نویسه *);
ایستا نویسه *null_libintl_bind_textdomain_codeset(ثابت نویسه *, ثابت نویسه *);
ایستا صحیح dyn_libintl_init(نویسه *dir);
ایستا تهی dyn_libintl_end(تهی);

ایستا HINSTANCE hLibintlDLL = 0;
ایستا نویسه *(*dyn_libintl_gettext)(ثابت نویسه *) = null_libintl_gettext;
ایستا نویسه *(*dyn_libintl_textdomain)(ثابت نویسه *) = null_libintl_textdomain;
ایستا نویسه *(*dyn_libintl_bindtextdomain)(ثابت نویسه *, ثابت نویسه *)
						= null_libintl_bindtextdomain;
ایستا نویسه *(*dyn_libintl_bind_textdomain_codeset)(ثابت نویسه *, ثابت نویسه *)
				       = null_libintl_bind_textdomain_codeset;

//
// Attempt to load libintl.dll.  If it doesn't work, use dummy functions.
// "dir" is the directory where the libintl.dll might be.
// Return 1 for success, 0 for failure.
//
    ایستا صحیح
dyn_libintl_init(نویسه *dir)
{
    صحیح		i;
    ایستا ساختار
    {
	نویسه	    *name;
	FARPROC	    *ptr;
    } libintl_entry[] =
    {
	{(نویسه *)"gettext",		(FARPROC*)&dyn_libintl_gettext},
	{(نویسه *)"textdomain",		(FARPROC*)&dyn_libintl_textdomain},
	{(نویسه *)"bindtextdomain",	(FARPROC*)&dyn_libintl_bindtextdomain},
	{(نویسه *)"bind_textdomain_codeset", (FARPROC*)&dyn_libintl_bind_textdomain_codeset},
	{پوچ, پوچ}
    };
    DWORD	len, len2;
    LPWSTR	buf = پوچ;
    LPWSTR	buf2 = پوچ;

    // No need to initialize twice.
    اگر (hLibintlDLL)
	بازگشت 1;

    // Load gettext library from $VIMRUNTIME\GvimExt{64,32} directory.
    // Add the directory to $PATH temporarily.
    len = GetEnvironmentVariableW(L"PATH", پوچ, 0);
    len2 = MAX_PATH + 1 + len;
    buf = (LPWSTR)malloc(len * اندازه(WCHAR));
    buf2 = (LPWSTR)malloc(len2 * اندازه(WCHAR));
    اگر (buf != پوچ && buf2 != پوچ)
    {
	GetEnvironmentVariableW(L"PATH", buf, len);
# ifdef _WIN64
	_snwprintf(buf2, len2, L"%S\\GvimExt64;%s", dir, buf);
# وگرنه
	_snwprintf(buf2, len2, L"%S\\GvimExt32;%s", dir, buf);
# endif
	SetEnvironmentVariableW(L"PATH", buf2);
	hLibintlDLL = LoadLibrary(GETTEXT_DLL);
# ifdef GETTEXT_DLL_ALT
	اگر (!hLibintlDLL)
	    hLibintlDLL = LoadLibrary(GETTEXT_DLL_ALT);
# endif
	SetEnvironmentVariableW(L"PATH", buf);
    }
    آزاد(buf);
    آزاد(buf2);
    اگر (!hLibintlDLL)
	بازگشت 0;

    // Get the addresses of the functions we need.
    برای (i = 0; libintl_entry[i].name != پوچ
					 && libintl_entry[i].ptr != پوچ; ++i)
    {
	اگر ((*libintl_entry[i].ptr = GetProcAddress(hLibintlDLL,
					      libintl_entry[i].name)) == پوچ)
	{
	    dyn_libintl_end();
	    بازگشت 0;
	}
    }
    بازگشت 1;
}

    ایستا تهی
dyn_libintl_end(تهی)
{
    اگر (hLibintlDLL)
	FreeLibrary(hLibintlDLL);
    hLibintlDLL			= پوچ;
    dyn_libintl_gettext		= null_libintl_gettext;
    dyn_libintl_textdomain	= null_libintl_textdomain;
    dyn_libintl_bindtextdomain	= null_libintl_bindtextdomain;
    dyn_libintl_bind_textdomain_codeset	= null_libintl_bind_textdomain_codeset;
}

    ایستا نویسه *
null_libintl_gettext(ثابت نویسه *msgid)
{
    بازگشت (نویسه *)msgid;
}

    ایستا نویسه *
null_libintl_textdomain(ثابت نویسه * /* domainname */)
{
    بازگشت پوچ;
}

    ایستا نویسه *
null_libintl_bindtextdomain(ثابت نویسه * /* domainname */, ثابت نویسه * /* dirname */)
{
    بازگشت پوچ;
}

    ایستا نویسه *
null_libintl_bind_textdomain_codeset(ثابت نویسه * /* domainname */, ثابت نویسه * /* codeset */)
{
    بازگشت پوچ;
}

//
// Setup for translating strings.
//
    ایستا تهی
dyn_gettext_load(تهی)
{
    نویسه    szBuff[BUFSIZE];
    DWORD   len;

    // Try to locate the runtime files.  The path is used to find libintl.dll
    // and the vim.mo files.
    getRuntimeDir(szBuff);
    اگر (szBuff[0] != 0)
    {
	len = (DWORD)strlen(szBuff);
	اگر (dyn_libintl_init(szBuff))
	{
	    strcpy(szBuff + len, "lang");

	    (*dyn_libintl_bindtextdomain)(VIMPACKAGE, szBuff);
	    (*dyn_libintl_textdomain)(VIMPACKAGE);
	}
    }
}

    ایستا تهی
dyn_gettext_free(تهی)
{
    dyn_libintl_end();
}

//
// Use UTF-8 for gettext. Returns previous codeset.
//
    ایستا نویسه *
set_gettext_codeset(تهی)
{
    نویسه *prev = dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, پوچ);
    prev = _strdup((prev != پوچ) ? prev : "char");
    dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, "utf-8");

    بازگشت prev;
}

//
// Restore previous codeset for gettext.
//
    ایستا تهی
restore_gettext_codeset(نویسه *prev)
{
    dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, prev);
    آزاد(prev);
}
#endif // FEAT_GETTEXT

//
// Global variables
//
UINT      g_cRefThisDll = 0;    // Reference count of this DLL.
HINSTANCE g_hmodThisDll = پوچ;	// Handle to this DLL itself.


//---------------------------------------------------------------------------
// DllMain
//---------------------------------------------------------------------------
بیرونی "C" صحیح APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID  /* lpReserved */)
{
    انتخاب (dwReason)
    {
    حالت DLL_PROCESS_ATTACH:
	// Extension DLL one-time initialization
	g_hmodThisDll = hInstance;
	بشکن;

    حالت DLL_PROCESS_DETACH:
	بشکن;
    }

    بازگشت 1;   // ok
}

    ایستا تهی
inc_cRefThisDLL()
{
#ifdef FEAT_GETTEXT
    اگر (g_cRefThisDll == 0)
	dyn_gettext_load();
#endif
    InterlockedIncrement((LPLONG)&g_cRefThisDll);
}

    ایستا تهی
dec_cRefThisDLL()
{
#ifdef FEAT_GETTEXT
    اگر (InterlockedDecrement((LPLONG)&g_cRefThisDll) == 0)
	dyn_gettext_free();
#وگرنه
    InterlockedDecrement((LPLONG)&g_cRefThisDll);
#endif
}

//---------------------------------------------------------------------------
// DllCanUnloadNow
//---------------------------------------------------------------------------

STDAPI DllCanUnloadNow(تهی)
{
    بازگشت (g_cRefThisDll == 0 ? S_OK : S_FALSE);
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppvOut)
{
    *ppvOut = پوچ;

    اگر (IsEqualIID(rclsid, CLSID_ShellExtension))
    {
	CShellExtClassFactory *pcf = جدید CShellExtClassFactory;

	بازگشت pcf->QueryInterface(riid, ppvOut);
    }

    بازگشت CLASS_E_CLASSNOTAVAILABLE;
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
    *ppv = پوچ;

    // any interface on this object is the object pointer

    اگر (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	*ppv = (LPCLASSFACTORY)این;

	AddRef();

	بازگشت NOERROR;
    }

    بازگشت E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::AddRef()
{
    بازگشت InterlockedIncrement((LPLONG)&m_cRef);
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::Release()
{
    اگر (InterlockedDecrement((LPLONG)&m_cRef))
	بازگشت m_cRef;

    delete این;

    بازگشت 0L;
}

STDMETHODIMP CShellExtClassFactory::CreateInstance(LPUNKNOWN pUnkOuter,
						      REFIID riid,
						      LPVOID *ppvObj)
{
    *ppvObj = پوچ;

    // Shell extensions typically don't support aggregation (inheritance)

    اگر (pUnkOuter)
	بازگشت CLASS_E_NOAGGREGATION;

    // Create the main shell extension object.  The shell will then call
    // QueryInterface with IID_IShellExtInit--this is how shell extensions are
    // initialized.

    LPCSHELLEXT pShellExt = جدید CShellExt();  // create the CShellExt object

    اگر (پوچ == pShellExt)
	بازگشت E_OUTOFMEMORY;

    بازگشت pShellExt->QueryInterface(riid, ppvObj);
}


STDMETHODIMP CShellExtClassFactory::LockServer(BOOL  /* fLock */)
{
    بازگشت NOERROR;
}

// *********************** CShellExt *************************
CShellExt::CShellExt()
{
    m_cRef = 0L;
    m_pDataObj = پوچ;

    inc_cRefThisDLL();

    LoadMenuIcon();
}

CShellExt::~CShellExt()
{
    اگر (m_pDataObj)
	m_pDataObj->Release();

    dec_cRefThisDLL();

    اگر (m_hVimIconBitmap)
	DeleteObject(m_hVimIconBitmap);
}

STDMETHODIMP CShellExt::QueryInterface(REFIID riid, LPVOID FAR *ppv)
{
    *ppv = پوچ;

    اگر (IsEqualIID(riid, IID_IShellExtInit) || IsEqualIID(riid, IID_IUnknown))
    {
	*ppv = (LPSHELLEXTINIT)این;
    }
    وگرنه اگر (IsEqualIID(riid, IID_IContextMenu))
    {
	*ppv = (LPCONTEXTMENU)این;
    }

    اگر (*ppv)
    {
	AddRef();

	بازگشت NOERROR;
    }

    بازگشت E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExt::AddRef()
{
    بازگشت InterlockedIncrement((LPLONG)&m_cRef);
}

STDMETHODIMP_(ULONG) CShellExt::Release()
{

    اگر (InterlockedDecrement((LPLONG)&m_cRef))
	بازگشت m_cRef;

    delete این;

    بازگشت 0L;
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
    اگر (m_pDataObj)
	m_pDataObj->Release();

    // duplicate the object pointer and registry handle

    اگر (pDataObj)
    {
	m_pDataObj = pDataObj;
	pDataObj->AddRef();
    }

    بازگشت NOERROR;
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
    اگر (medium.hGlobal)
	cbFiles = DragQueryFileW((HDROP)medium.hGlobal, (UINT)-1, 0, 0);

    // InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

    // Initialize m_cntOfHWnd to 0
    m_cntOfHWnd = 0;

    HKEY keyhandle;
    منطقی showExisting = درست;
    منطقی showIcons = درست;

    // Check whether "Edit with existing Vim" entries are disabled.
    اگر (Reg_Open_کلید_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
				       کلید_READ, &keyhandle) == ERROR_SUCCESS)
    {
	اگر (Reg_Query_مقدار_Ex(keyhandle, "DisableEditWithExisting", 0, پوچ,
						 پوچ, پوچ) == ERROR_SUCCESS)
	    showExisting = نادرست;
	اگر (Reg_Query_مقدار_Ex(keyhandle, "DisableContextMenuIcons", 0, پوچ,
						 پوچ, پوچ) == ERROR_SUCCESS)
	    showIcons = نادرست;
	Reg_Close_کلید(keyhandle);
    }

    // Use UTF-8 for gettext.
    نویسه *prev = set_gettext_codeset();

    // Retrieve all the vim instances, unless disabled.
    اگر (showExisting)
	EnumWindows(EnumWindowsProc, (LPARAM)این);

    MENUITEMINFOW mii = { اندازه(MENUITEMINFOW) };
    mii.fMask = MIIM_STRING | MIIM_ID;
    اگر (showIcons)
    {
	mii.fMask |= MIIM_BITMAP;
	mii.hbmpItem = m_hVimIconBitmap;
    }

    اگر (cbFiles > 1)
    {
	mii.wID = idCmd++;
	mii.dwTypeData = W(_("Edit with Vim using &tabpages"));
	mii.cch = wcslen(mii.dwTypeData);
	درج_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	آزاد(mii.dwTypeData);

	mii.wID = idCmd++;
	mii.dwTypeData = W(_("Edit with single &Vim"));
	mii.cch = wcslen(mii.dwTypeData);
	درج_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	آزاد(mii.dwTypeData);

	اگر (cbFiles <= 4)
	{
	    // Can edit up to 4 files in diff mode
	    mii.wID = idCmd++;
	    mii.dwTypeData = W(_("Diff with Vim"));
	    mii.cch = wcslen(mii.dwTypeData);
	    درج_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	    آزاد(mii.dwTypeData);
	    m_edit_existing_off = 3;
	}
	وگرنه
	    m_edit_existing_off = 2;

    }
    وگرنه
    {
	mii.wID = idCmd++;
	mii.dwTypeData = W(_("Edit with &Vim"));
	mii.cch = wcslen(mii.dwTypeData);
	درج_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	آزاد(mii.dwTypeData);
	m_edit_existing_off = 1;
    }

    HMENU hSubMenu = پوچ;
    اگر (m_cntOfHWnd > 1)
    {
	hSubMenu = CreatePopupMenu();
	mii.fMask |= MIIM_SUBMENU;
	mii.wID = idCmd;
	mii.dwTypeData = W(_("Edit with existing Vim"));
	mii.cch = wcslen(mii.dwTypeData);
	mii.hSubMenu = hSubMenu;
	درج_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	آزاد(mii.dwTypeData);
	mii.fMask = mii.fMask & ~MIIM_SUBMENU;
	mii.hSubMenu = پوچ;
    }
    // Now display all the vim instances
    برای (صحیح i = 0; i < m_cntOfHWnd; i++)
    {
	WCHAR title[BUFSIZE];
	WCHAR temp[BUFSIZE];
	صحیح index;
	HMENU hmenu;

	// Obtain window title, continue if can not
	اگر (GetWindowTextW(m_hWnd[i], title, BUFSIZE - 1) == 0)
	    ادامه;
	// Truncate the title before the path, keep the file name
	WCHAR *pos = wcschr(title, L'(');
	اگر (pos != پوچ)
	{
	    اگر (pos > title && pos[-1] == L' ')
		--pos;
	    *pos = 0;
	}
	// Now concatenate
	اگر (m_cntOfHWnd > 1)
	    temp[0] = L'\0';
	وگرنه
	{
	    WCHAR *s = W(_("Edit with existing Vim - "));
	    wcsncpy(temp, s, BUFSIZE - 1);
	    temp[BUFSIZE - 1] = L'\0';
	    آزاد(s);
	}
	wcsncat(temp, title, BUFSIZE - 1 - wcslen(temp));
	temp[BUFSIZE - 1] = L'\0';

	mii.wID = idCmd++;
	mii.dwTypeData = temp;
	mii.cch = wcslen(mii.dwTypeData);
	اگر (m_cntOfHWnd > 1)
	{
	    hmenu = hSubMenu;
	    index = i;
	}
	وگرنه
	{
	    hmenu = hMenu;
	    index = indexMenu++;
	}
	درج_Menu_Item_W(hmenu, index, TRUE, &mii);
    }
    // InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

    // Restore previous codeset.
    restore_gettext_codeset(prev);

    // Must return number of menu items we added.
    بازگشت ResultFromShort(idCmd-idCmdFirst);
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
    صحیح gvimExtraOptions;

    // If HIWORD(lpcmi->lpVerb) then we have been called programmatically
    // and lpVerb is a command that should be invoked.  Otherwise, the shell
    // has called us, and LOWORD(lpcmi->lpVerb) is the menu ID the user has
    // selected.  Actually, it's (menu ID - idCmdFirst) from QueryContextMenu().
    اگر (!HIWORD(lpcmi->lpVerb))
    {
	UINT idCmd = LOWORD(lpcmi->lpVerb);

	اگر (idCmd >= m_edit_existing_off)
	{
	    // Existing with vim instance
	    hr = افزودن_To_Window(lpcmi->hwnd,
		    lpcmi->lpDirectory,
		    lpcmi->lpVerb,
		    lpcmi->lpParameters,
		    lpcmi->nShow,
		    idCmd - m_edit_existing_off);
	}
	وگرنه
	{
	    انتخاب (idCmd)
	    {
		حالت 0:
		    gvimExtraOptions = EDIT_WITH_VIM_USE_TABPAGES;
		    بشکن;
		حالت 1:
		    gvimExtraOptions = EDIT_WITH_VIM_NO_OPTIONS;
		    بشکن;
		حالت 2:
		    gvimExtraOptions = EDIT_WITH_VIM_IN_DIFF_MODE;
		    بشکن;
		پیشفرض:
		    // If execution reaches this point we likely have an
		    // inconsistency between the code that setup the menus
		    // and this code that determines what the user
		    // selected.  This should be detected and fixed during
		    // development.
		    بازگشت E_FAIL;
	    }

            LPCMINVOKECOMMANDINFOEX lpcmiex = (LPCMINVOKECOMMANDINFOEX)lpcmi;
            LPCWSTR currentDirectory = lpcmi->cbSize == اندازه(CMINVOKECOMMANDINFOEX) ? lpcmiex->lpDirectoryW : پوچ;

	    hr = InvokeSingleGvim(lpcmi->hwnd,
		    currentDirectory,
		    lpcmi->lpVerb,
		    lpcmi->lpParameters,
		    lpcmi->nShow,
		    gvimExtraOptions);
	}
    }
    بازگشت hr;
}

STDMETHODIMP CShellExt::افزودن_To_Window(HWND  /* hParent */,
				   LPCSTR  /* pszWorkingDir */,
				   LPCSTR  /* pszCmd */,
				   LPCSTR  /* pszParam */,
				   صحیح  /* iShowCmd */,
				   صحیح idHWnd)
{
    HWND hWnd = m_hWnd[idHWnd];

    // Show and bring vim instance to foreground
    اگر (IsIconic(hWnd) != 0)
	ShowWindow(hWnd, SW_RESTORE);
    وگرنه
	ShowWindow(hWnd, SW_SHOW);
    SetForegroundWindow(hWnd);

    // Post the selected files to the vim instance
    PostMessage(hWnd, WM_DROPFILES, (WPARAM)medium.hGlobal, 0);

    بازگشت NOERROR;
}

STDMETHODIMP CShellExt::GetCommandString(UINT_PTR  /* idCmd */,
					 UINT uFlags,
					 UINT FAR * /* reserved */,
					 LPSTR pszName,
					 UINT cchMax)
{
    // Use UTF-8 for gettext.
    نویسه *prev = set_gettext_codeset();

    WCHAR *s = W(_("Edits the selected file(s) with Vim"));
    اگر (uFlags == GCS_HELPTEXTW && cchMax > wcslen(s))
	wcscpy((WCHAR *)pszName, s);
    آزاد(s);

    // Restore previous codeset.
    restore_gettext_codeset(prev);

    بازگشت NOERROR;
}

BOOL CALLBACK CShellExt::EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    نویسه temp[BUFSIZE];

    // First do a bunch of check
    // No invisible window
    اگر (!IsWindowVisible(hWnd))
	بازگشت TRUE;
    // No child window ???
    // if (GetParent(hWnd)) return TRUE;
    // Class name should be Vim, if failed to get class name, return
    اگر (GetClassName(hWnd, temp, ARRAY_LENGTH(temp)) == 0)
	بازگشت TRUE;
    // Compare class name to that of vim, if not, return
    اگر (_strnicmp(temp, "vim", ARRAY_LENGTH("vim")) != 0)
	بازگشت TRUE;
    // First check if the number of vim instance exceeds MAX_HWND
    CShellExt *cs = (CShellExt*) lParam;
    اگر (cs->m_cntOfHWnd >= MAX_HWND)
	بازگشت FALSE;	// stop enumeration
    // Now we get the vim window, put it into some kind of array
    cs->m_hWnd[cs->m_cntOfHWnd] = hWnd;
    cs->m_cntOfHWnd ++;

    بازگشت TRUE; // continue enumeration (otherwise this would be false)
}

BOOL CShellExt::LoadMenuIcon()
{
    نویسه vimExeFile[BUFSIZE];
    getGvimName(vimExeFile, TRUE);
    اگر (vimExeFile[0] == '\0')
	بازگشت FALSE;
    HICON hVimIcon;
    اگر (ExtractIconEx(vimExeFile, 0, پوچ, &hVimIcon, 1) == 0)
	بازگشت FALSE;
    m_hVimIconBitmap = IconToBitmap(hVimIcon,
	    GetSysColorBrush(COLOR_MENU),
	    GetSystemMetrics(SM_CXSMICON),
	    GetSystemMetrics(SM_CYSMICON));
    بازگشت TRUE;
}

    ایستا نویسه *
searchpath(نویسه *name)
{
    ایستا اجتماع {
	نویسه	location[2 * BUFSIZE];
	WCHAR	wname[BUFSIZE];
    };
    WCHAR wlocation[BUFSIZE + 1];

    // There appears to be a bug in FindExecutableA() on Windows NT.
    // Use FindExecutableW() instead...
    MultiByteToWideChar(CP_ACP, 0, name, -1, wname, BUFSIZE);
    اگر (FindExecutableW(wname, L"", wlocation) > (HINSTANCE)32)
    {
	WideCharToMultiByte(CP_ACP, 0, wlocation, -1,
		location, اندازه(location), پوچ, پوچ);
	بازگشت location;
    }
    بازگشت (نویسه *)"";
}


STDMETHODIMP CShellExt::InvokeSingleGvim(HWND hParent,
				   LPCWSTR  workingDir,
				   LPCSTR  /* pszCmd */,
				   LPCSTR  /* pszParam */,
				   صحیح  /* iShowCmd */,
				   صحیح gvimExtraOptions)
{
    wchar_t	m_szFileUserClickedOn[BUFSIZE];
    wchar_t	*cmdStrW;
    size_t	cmdlen;
    size_t	len;
    UINT i;

    cmdlen = BUFSIZE;
    cmdStrW  = (wchar_t *) malloc(cmdlen * اندازه(wchar_t));
    اگر (cmdStrW == پوچ)
	بازگشت E_FAIL;
    getGvimInvocationW(cmdStrW);

    اگر (gvimExtraOptions == EDIT_WITH_VIM_IN_DIFF_MODE)
	wcscat(cmdStrW, L" -d");
    وگرنه اگر (gvimExtraOptions == EDIT_WITH_VIM_USE_TABPAGES)
	wcscat(cmdStrW, L" -p");
    برای (i = 0; i < cbFiles; i++)
    {
	DragQueryFileW((HDROP)medium.hGlobal,
		i,
		m_szFileUserClickedOn,
		ARRAY_LENGTH(m_szFileUserClickedOn));

	len = wcslen(cmdStrW) + wcslen(m_szFileUserClickedOn) + 4;
	اگر (len > cmdlen)
	{
	    cmdlen = len + BUFSIZE;
	    wchar_t *cmdStrW_new = (wchar_t *)realloc(cmdStrW, cmdlen * اندازه(wchar_t));
	    اگر (cmdStrW_new == پوچ)
	    {
		آزاد(cmdStrW);
		بازگشت E_FAIL;
	    }
	    cmdStrW = cmdStrW_new;
	}
	wcscat(cmdStrW, L" \"");
	wcscat(cmdStrW, m_szFileUserClickedOn);
	wcscat(cmdStrW, L"\"");
    }

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, اندازه(si));
    si.cb = اندازه(si);

    // Start the child process.
    اگر (!CreateProcessW(پوچ,	// No module name (use command line).
		cmdStrW,	// Command line.
		پوچ,		// Process handle not inheritable.
		پوچ,		// Thread handle not inheritable.
		FALSE,		// Set handle inheritance to FALSE.
		0,		// No creation flags.
		پوچ,		// Use parent's environment block.
		workingDir,	// Use parent's starting directory.
		&si,		// Pointer to STARTUPINFO structure.
		&pi)		// Pointer to PROCESS_INFORMATION structure.
       )
    {
	// Use UTF-8 for gettext.
	نویسه *prev = set_gettext_codeset();

	WCHAR *پیام = W(_("Error creating process: Check if gvim is in your path!"));
	WCHAR *title = W(_("gvimext.dll error"));

	MessageBoxW(hParent, پیام, title, MB_OK);

	آزاد(پیام);
	آزاد(title);

	// Restore previous codeset.
	restore_gettext_codeset(prev);
    }
    وگرنه
    {
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
    }
    آزاد(cmdStrW);

    بازگشت NOERROR;
}
