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

#अन्तर्भाव "gvimext.h"
#अन्तर्भाव "../version.h"

#define ARRAY_LENGTH(a) (आकार(a) / आकार((a)[0]))

स्थिर अक्षर *searchpath(अक्षर *name);

// Always get an error while putting the following stuff to the
// gvimext.h file as class protected variables, give up and
// declare them as global stuff
FORMATETC fmte = {CF_HDROP,
		  (DVTARGETDEVICE FAR *)रिक्त,
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
    स्थिर शून्य
getGvimName(अक्षर *name, BOOL runtime)
{
    HKEY	keyhandle;
    DWORD	hlen;

    // Get the location of gvim from the registry.  Try
    // HKEY_CURRENT_USER first, then fall back to HKEY_LOCAL_MACHINE if
    // not found.
    name[0] = 0;
    जर (Reg_Open_किल्ली_Ex(HKEY_CURRENT_USER, "Software\\Vim\\Gvim", 0,
				       किल्ली_READ, &keyhandle) == ERROR_SUCCESS)
    {
	hlen = BUFSIZE;
	जर (Reg_Query_मूल्य_Ex(keyhandle, "path", 0, रिक्त, (BYTE *)name, &hlen)
							     != ERROR_SUCCESS)
	    name[0] = 0;
	अन्यथा
	    name[hlen] = 0;
	Reg_Close_किल्ली(keyhandle);
    }

    जर ((name[0] == 0) &&
	    (Reg_Open_किल्ली_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
				       किल्ली_READ, &keyhandle) == ERROR_SUCCESS))
    {
	hlen = BUFSIZE;
	जर (Reg_Query_मूल्य_Ex(keyhandle, "path", 0, रिक्त, (BYTE *)name, &hlen)
							     != ERROR_SUCCESS)
	    name[0] = 0;
	अन्यथा
	    name[hlen] = 0;
	Reg_Close_किल्ली(keyhandle);
    }

    // Registry didn't work, use the search path.
    जर (name[0] == 0)
	म_नकल(name, searchpath((अक्षर *)"gvim.exe"));

    जर (!runtime)
    {
	// Only when looking for the executable, not the runtime dir, we can
	// search for the batch file or a name without a path.
	जर (name[0] == 0)
	    म_नकल(name, searchpath((अक्षर *)"gvim.bat"));
	जर (name[0] == 0)
	    म_नकल(name, "gvim");	// finds gvim.bat or gvim.exe
    }
}

    स्थिर शून्य
getGvimInvocation(अक्षर *name)
{
    getGvimName(name, FALSE);
    // avoid that Vim tries to expand wildcards in the file names
    म_जोड़ो(name, " --literal");
}

    स्थिर शून्य
getGvimInvocationW(ब_अक्षर_प्रकार *nameW)
{
    अक्षर *name;

    name = (अक्षर *)दो_स्मृति(BUFSIZE);
    getGvimInvocation(name);
    mbstowcs(nameW, name, BUFSIZE);
    मुक्त(name);
}

    स्थिर BOOL
FileExists(LPCTSTR szPath)
{
    DWORD dwAttrib = GetFileAttributes(szPath);

    परत (dwAttrib != INVALID_FILE_ATTRIBUTES
	    && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

//
// Get the Vim runtime directory into buf[BUFSIZE].
// The result is empty when it failed.
// When it works, the path ends in a slash or backslash.
//
    स्थिर शून्य
getRuntimeDir(अक्षर *buf)
{
    पूर्णांक		idx;

    getGvimName(buf, TRUE);
    जर (buf[0] == 0)
	परत;

    // When no path found, use the search path to expand it.
    जर (म_अक्षर(buf, '/') == रिक्त && म_अक्षर(buf, '\\') == रिक्त)
	म_नकल(buf, searchpath(buf));

    // remove "gvim.exe" from the end
    साठी (idx = (पूर्णांक)म_माप(buf) - 1; idx >= 0; idx--)
	जर (buf[idx] == '\\' || buf[idx] == '/')
	{
	    buf[++idx] = 0;
	    थांबा;
	}

    // Check the existence of defaults.vim
    अक्षर *p = buf + idx;
    म_नकल(p, "defaults.vim");
    जर (FileExists(buf))
    {
	*p = 0;		// Cut "defaults.vim"
	परत;
    }
    // Check the existence of vimXX\defaults.vim
    म_नकल(p, VIM_VERSION_NODOT "\\" "defaults.vim");
    जर (FileExists(buf))
    {
	p[ARRAY_LENGTH(VIM_VERSION_NODOT)] = 0;	// Cut "defaults.vim"
	परत;
    }
    // Check the existence of runtime\defaults.vim
    म_नकल(p, "runtime" "\\" "defaults.vim");
    जर (FileExists(buf))
    {
	p[ARRAY_LENGTH("runtime")] = 0;		// Cut "defaults.vim"
	परत;
    }
    // Not found
    buf[0] = 0;
    परत;
}

    WCHAR *
utf8_to_utf16(स्थिरांक अक्षर *s)
{
    पूर्णांक size = MultiByteToWideChar(CP_UTF8, 0, s, -1, रिक्त, 0);
    WCHAR *buf = (WCHAR *)दो_स्मृति(size * आकार(WCHAR));
    MultiByteToWideChar(CP_UTF8, 0, s, -1, buf, size);
    परत buf;
}

    HBITMAP
IconToBitmap(HICON hIcon, HBRUSH hBackground, पूर्णांक width, पूर्णांक height)
{
    HDC hDC = GetDC(रिक्त);
    HDC hMemDC = CreateCompatibleDC(hDC);
    HBITMAP hMemBmp = CreateCompatibleBitmap(hDC, width, height);
    HBITMAP hResultBmp = रिक्त;
    HGDIOBJ hOrgBMP = SelectObject(hMemDC, hMemBmp);

    DrawIconEx(hMemDC, 0, 0, hIcon, width, height, 0, hBackground, DI_NORMAL);

    hResultBmp = hMemBmp;
    hMemBmp = रिक्त;

    SelectObject(hMemDC, hOrgBMP);
    DeleteDC(hMemDC);
    ReleaseDC(रिक्त, hDC);
    DestroyIcon(hIcon);
    परत hResultBmp;
}

//
// GETTEXT: translated messages and menu entries
//
#ifndef FEAT_GETTEXT
# define _(x)	    x
# define W_impl(x) _wcsdup(L##x)
# define W(x)	    W_impl(x)
# define set_gettext_codeset()	    रिक्त
# define restore_gettext_codeset(x)
#अन्यथा
# define _(x)	    (*dyn_libintl_gettext)(x)
# define W(x)	    utf8_to_utf16(x)
# define VIMPACKAGE "vim"
# ifndef GETTEXT_DLL
#  define GETTEXT_DLL "libintl.dll"
#  define GETTEXT_DLL_ALT "libintl-8.dll"
# endif

// Dummy functions
स्थिर अक्षर *null_libintl_gettext(स्थिरांक अक्षर *);
स्थिर अक्षर *null_libintl_textdomain(स्थिरांक अक्षर *);
स्थिर अक्षर *null_libintl_bindtextdomain(स्थिरांक अक्षर *, स्थिरांक अक्षर *);
स्थिर अक्षर *null_libintl_bind_textdomain_codeset(स्थिरांक अक्षर *, स्थिरांक अक्षर *);
स्थिर पूर्णांक dyn_libintl_init(अक्षर *dir);
स्थिर शून्य dyn_libintl_end(शून्य);

स्थिर HINSTANCE hLibintlDLL = 0;
स्थिर अक्षर *(*dyn_libintl_gettext)(स्थिरांक अक्षर *) = null_libintl_gettext;
स्थिर अक्षर *(*dyn_libintl_textdomain)(स्थिरांक अक्षर *) = null_libintl_textdomain;
स्थिर अक्षर *(*dyn_libintl_bindtextdomain)(स्थिरांक अक्षर *, स्थिरांक अक्षर *)
						= null_libintl_bindtextdomain;
स्थिर अक्षर *(*dyn_libintl_bind_textdomain_codeset)(स्थिरांक अक्षर *, स्थिरांक अक्षर *)
				       = null_libintl_bind_textdomain_codeset;

//
// Attempt to load libintl.dll.  If it doesn't work, use dummy functions.
// "dir" is the directory where the libintl.dll might be.
// Return 1 for success, 0 for failure.
//
    स्थिर पूर्णांक
dyn_libintl_init(अक्षर *dir)
{
    पूर्णांक		i;
    स्थिर संरचना
    {
	अक्षर	    *name;
	FARPROC	    *ptr;
    } libintl_entry[] =
    {
	{(अक्षर *)"gettext",		(FARPROC*)&dyn_libintl_gettext},
	{(अक्षर *)"textdomain",		(FARPROC*)&dyn_libintl_textdomain},
	{(अक्षर *)"bindtextdomain",	(FARPROC*)&dyn_libintl_bindtextdomain},
	{(अक्षर *)"bind_textdomain_codeset", (FARPROC*)&dyn_libintl_bind_textdomain_codeset},
	{रिक्त, रिक्त}
    };
    DWORD	len, len2;
    LPWSTR	buf = रिक्त;
    LPWSTR	buf2 = रिक्त;

    // No need to initialize twice.
    जर (hLibintlDLL)
	परत 1;

    // Load gettext library from $VIMRUNTIME\GvimExt{64,32} directory.
    // Add the directory to $PATH temporarily.
    len = GetEnvironmentVariableW(L"PATH", रिक्त, 0);
    len2 = MAX_PATH + 1 + len;
    buf = (LPWSTR)दो_स्मृति(len * आकार(WCHAR));
    buf2 = (LPWSTR)दो_स्मृति(len2 * आकार(WCHAR));
    जर (buf != रिक्त && buf2 != रिक्त)
    {
	GetEnvironmentVariableW(L"PATH", buf, len);
# ifdef _WIN64
	_snwprintf(buf2, len2, L"%S\\GvimExt64;%s", dir, buf);
# अन्यथा
	_snwprintf(buf2, len2, L"%S\\GvimExt32;%s", dir, buf);
# endif
	SetEnvironmentVariableW(L"PATH", buf2);
	hLibintlDLL = LoadLibrary(GETTEXT_DLL);
# ifdef GETTEXT_DLL_ALT
	जर (!hLibintlDLL)
	    hLibintlDLL = LoadLibrary(GETTEXT_DLL_ALT);
# endif
	SetEnvironmentVariableW(L"PATH", buf);
    }
    मुक्त(buf);
    मुक्त(buf2);
    जर (!hLibintlDLL)
	परत 0;

    // Get the addresses of the functions we need.
    साठी (i = 0; libintl_entry[i].name != रिक्त
					 && libintl_entry[i].ptr != रिक्त; ++i)
    {
	जर ((*libintl_entry[i].ptr = GetProcAddress(hLibintlDLL,
					      libintl_entry[i].name)) == रिक्त)
	{
	    dyn_libintl_end();
	    परत 0;
	}
    }
    परत 1;
}

    स्थिर शून्य
dyn_libintl_end(शून्य)
{
    जर (hLibintlDLL)
	FreeLibrary(hLibintlDLL);
    hLibintlDLL			= रिक्त;
    dyn_libintl_gettext		= null_libintl_gettext;
    dyn_libintl_textdomain	= null_libintl_textdomain;
    dyn_libintl_bindtextdomain	= null_libintl_bindtextdomain;
    dyn_libintl_bind_textdomain_codeset	= null_libintl_bind_textdomain_codeset;
}

    स्थिर अक्षर *
null_libintl_gettext(स्थिरांक अक्षर *msgid)
{
    परत (अक्षर *)msgid;
}

    स्थिर अक्षर *
null_libintl_textdomain(स्थिरांक अक्षर * /* domainname */)
{
    परत रिक्त;
}

    स्थिर अक्षर *
null_libintl_bindtextdomain(स्थिरांक अक्षर * /* domainname */, स्थिरांक अक्षर * /* dirname */)
{
    परत रिक्त;
}

    स्थिर अक्षर *
null_libintl_bind_textdomain_codeset(स्थिरांक अक्षर * /* domainname */, स्थिरांक अक्षर * /* codeset */)
{
    परत रिक्त;
}

//
// Setup for translating strings.
//
    स्थिर शून्य
dyn_gettext_load(शून्य)
{
    अक्षर    szBuff[BUFSIZE];
    DWORD   len;

    // Try to locate the runtime files.  The path is used to find libintl.dll
    // and the vim.mo files.
    getRuntimeDir(szBuff);
    जर (szBuff[0] != 0)
    {
	len = (DWORD)म_माप(szBuff);
	जर (dyn_libintl_init(szBuff))
	{
	    म_नकल(szBuff + len, "lang");

	    (*dyn_libintl_bindtextdomain)(VIMPACKAGE, szBuff);
	    (*dyn_libintl_textdomain)(VIMPACKAGE);
	}
    }
}

    स्थिर शून्य
dyn_gettext_free(शून्य)
{
    dyn_libintl_end();
}

//
// Use UTF-8 for gettext. Returns previous codeset.
//
    स्थिर अक्षर *
set_gettext_codeset(शून्य)
{
    अक्षर *prev = dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, रिक्त);
    prev = _strdup((prev != रिक्त) ? prev : "char");
    dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, "utf-8");

    परत prev;
}

//
// Restore previous codeset for gettext.
//
    स्थिर शून्य
restore_gettext_codeset(अक्षर *prev)
{
    dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, prev);
    मुक्त(prev);
}
#endif // FEAT_GETTEXT

//
// Global variables
//
UINT      g_cRefThisDll = 0;    // Reference count of this DLL.
HINSTANCE g_hmodThisDll = रिक्त;	// Handle to this DLL itself.


//---------------------------------------------------------------------------
// DllMain
//---------------------------------------------------------------------------
बाह्य "C" पूर्णांक APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID  /* lpReserved */)
{
    निवड (dwReason)
    {
    स्थिती DLL_PROCESS_ATTACH:
	// Extension DLL one-time initialization
	g_hmodThisDll = hInstance;
	थांबा;

    स्थिती DLL_PROCESS_DETACH:
	थांबा;
    }

    परत 1;   // ok
}

    स्थिर शून्य
inc_cRefThisDLL()
{
#ifdef FEAT_GETTEXT
    जर (g_cRefThisDll == 0)
	dyn_gettext_load();
#endif
    InterlockedIncrement((LPLONG)&g_cRefThisDll);
}

    स्थिर शून्य
dec_cRefThisDLL()
{
#ifdef FEAT_GETTEXT
    जर (InterlockedDecrement((LPLONG)&g_cRefThisDll) == 0)
	dyn_gettext_free();
#अन्यथा
    InterlockedDecrement((LPLONG)&g_cRefThisDll);
#endif
}

//---------------------------------------------------------------------------
// DllCanUnloadNow
//---------------------------------------------------------------------------

STDAPI DllCanUnloadNow(शून्य)
{
    परत (g_cRefThisDll == 0 ? S_OK : S_FALSE);
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppvOut)
{
    *ppvOut = रिक्त;

    जर (IsEqualIID(rclsid, CLSID_ShellExtension))
    {
	CShellExtClassFactory *pcf = नवीन CShellExtClassFactory;

	परत pcf->QueryInterface(riid, ppvOut);
    }

    परत CLASS_E_CLASSNOTAVAILABLE;
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
    *ppv = रिक्त;

    // any interface on this object is the object pointer

    जर (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	*ppv = (LPCLASSFACTORY)हे;

	AddRef();

	परत NOERROR;
    }

    परत E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::AddRef()
{
    परत InterlockedIncrement((LPLONG)&m_cRef);
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::Release()
{
    जर (InterlockedDecrement((LPLONG)&m_cRef))
	परत m_cRef;

    delete हे;

    परत 0L;
}

STDMETHODIMP CShellExtClassFactory::CreateInstance(LPUNKNOWN pUnkOuter,
						      REFIID riid,
						      LPVOID *ppvObj)
{
    *ppvObj = रिक्त;

    // Shell extensions typically don't support aggregation (inheritance)

    जर (pUnkOuter)
	परत CLASS_E_NOAGGREGATION;

    // Create the main shell extension object.  The shell will then call
    // QueryInterface with IID_IShellExtInit--this is how shell extensions are
    // initialized.

    LPCSHELLEXT pShellExt = नवीन CShellExt();  // create the CShellExt object

    जर (रिक्त == pShellExt)
	परत E_OUTOFMEMORY;

    परत pShellExt->QueryInterface(riid, ppvObj);
}


STDMETHODIMP CShellExtClassFactory::LockServer(BOOL  /* fLock */)
{
    परत NOERROR;
}

// *********************** CShellExt *************************
CShellExt::CShellExt()
{
    m_cRef = 0L;
    m_pDataObj = रिक्त;

    inc_cRefThisDLL();

    LoadMenuIcon();
}

CShellExt::~CShellExt()
{
    जर (m_pDataObj)
	m_pDataObj->Release();

    dec_cRefThisDLL();

    जर (m_hVimIconBitmap)
	DeleteObject(m_hVimIconBitmap);
}

STDMETHODIMP CShellExt::QueryInterface(REFIID riid, LPVOID FAR *ppv)
{
    *ppv = रिक्त;

    जर (IsEqualIID(riid, IID_IShellExtInit) || IsEqualIID(riid, IID_IUnknown))
    {
	*ppv = (LPSHELLEXTINIT)हे;
    }
    अन्यथा जर (IsEqualIID(riid, IID_IContextMenu))
    {
	*ppv = (LPCONTEXTMENU)हे;
    }

    जर (*ppv)
    {
	AddRef();

	परत NOERROR;
    }

    परत E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExt::AddRef()
{
    परत InterlockedIncrement((LPLONG)&m_cRef);
}

STDMETHODIMP_(ULONG) CShellExt::Release()
{

    जर (InterlockedDecrement((LPLONG)&m_cRef))
	परत m_cRef;

    delete हे;

    परत 0L;
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
    जर (m_pDataObj)
	m_pDataObj->Release();

    // duplicate the object pointer and registry handle

    जर (pDataObj)
    {
	m_pDataObj = pDataObj;
	pDataObj->AddRef();
    }

    परत NOERROR;
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
    जर (medium.hGlobal)
	cbFiles = DragQueryFileW((HDROP)medium.hGlobal, (UINT)-1, 0, 0);

    // InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

    // Initialize m_cntOfHWnd to 0
    m_cntOfHWnd = 0;

    HKEY keyhandle;
    तार्किक showExisting = सत्य;
    तार्किक showIcons = सत्य;

    // Check whether "Edit with existing Vim" entries are disabled.
    जर (Reg_Open_किल्ली_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
				       किल्ली_READ, &keyhandle) == ERROR_SUCCESS)
    {
	जर (Reg_Query_मूल्य_Ex(keyhandle, "DisableEditWithExisting", 0, रिक्त,
						 रिक्त, रिक्त) == ERROR_SUCCESS)
	    showExisting = असत्य;
	जर (Reg_Query_मूल्य_Ex(keyhandle, "DisableContextMenuIcons", 0, रिक्त,
						 रिक्त, रिक्त) == ERROR_SUCCESS)
	    showIcons = असत्य;
	Reg_Close_किल्ली(keyhandle);
    }

    // Use UTF-8 for gettext.
    अक्षर *prev = set_gettext_codeset();

    // Retrieve all the vim instances, unless disabled.
    जर (showExisting)
	EnumWindows(EnumWindowsProc, (LPARAM)हे);

    MENUITEMINFOW mii = { आकार(MENUITEMINFOW) };
    mii.fMask = MIIM_STRING | MIIM_ID;
    जर (showIcons)
    {
	mii.fMask |= MIIM_BITMAP;
	mii.hbmpItem = m_hVimIconBitmap;
    }

    जर (cbFiles > 1)
    {
	mii.wID = idCmd++;
	mii.dwTypeData = W(_("Edit with Vim using &tabpages"));
	mii.cch = wcslen(mii.dwTypeData);
	घाला_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	मुक्त(mii.dwTypeData);

	mii.wID = idCmd++;
	mii.dwTypeData = W(_("Edit with single &Vim"));
	mii.cch = wcslen(mii.dwTypeData);
	घाला_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	मुक्त(mii.dwTypeData);

	जर (cbFiles <= 4)
	{
	    // Can edit up to 4 files in diff mode
	    mii.wID = idCmd++;
	    mii.dwTypeData = W(_("Diff with Vim"));
	    mii.cch = wcslen(mii.dwTypeData);
	    घाला_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	    मुक्त(mii.dwTypeData);
	    m_edit_existing_off = 3;
	}
	अन्यथा
	    m_edit_existing_off = 2;

    }
    अन्यथा
    {
	mii.wID = idCmd++;
	mii.dwTypeData = W(_("Edit with &Vim"));
	mii.cch = wcslen(mii.dwTypeData);
	घाला_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	मुक्त(mii.dwTypeData);
	m_edit_existing_off = 1;
    }

    HMENU hSubMenu = रिक्त;
    जर (m_cntOfHWnd > 1)
    {
	hSubMenu = CreatePopupMenu();
	mii.fMask |= MIIM_SUBMENU;
	mii.wID = idCmd;
	mii.dwTypeData = W(_("Edit with existing Vim"));
	mii.cch = wcslen(mii.dwTypeData);
	mii.hSubMenu = hSubMenu;
	घाला_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	मुक्त(mii.dwTypeData);
	mii.fMask = mii.fMask & ~MIIM_SUBMENU;
	mii.hSubMenu = रिक्त;
    }
    // Now display all the vim instances
    साठी (पूर्णांक i = 0; i < m_cntOfHWnd; i++)
    {
	WCHAR title[BUFSIZE];
	WCHAR temp[BUFSIZE];
	पूर्णांक index;
	HMENU hmenu;

	// Obtain window title, continue if can not
	जर (GetWindowTextW(m_hWnd[i], title, BUFSIZE - 1) == 0)
	    सुरूठेवा;
	// Truncate the title before the path, keep the file name
	WCHAR *pos = wcschr(title, L'(');
	जर (pos != रिक्त)
	{
	    जर (pos > title && pos[-1] == L' ')
		--pos;
	    *pos = 0;
	}
	// Now concatenate
	जर (m_cntOfHWnd > 1)
	    temp[0] = L'\0';
	अन्यथा
	{
	    WCHAR *s = W(_("Edit with existing Vim - "));
	    wcsncpy(temp, s, BUFSIZE - 1);
	    temp[BUFSIZE - 1] = L'\0';
	    मुक्त(s);
	}
	wcsncat(temp, title, BUFSIZE - 1 - wcslen(temp));
	temp[BUFSIZE - 1] = L'\0';

	mii.wID = idCmd++;
	mii.dwTypeData = temp;
	mii.cch = wcslen(mii.dwTypeData);
	जर (m_cntOfHWnd > 1)
	{
	    hmenu = hSubMenu;
	    index = i;
	}
	अन्यथा
	{
	    hmenu = hMenu;
	    index = indexMenu++;
	}
	घाला_Menu_Item_W(hmenu, index, TRUE, &mii);
    }
    // InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

    // Restore previous codeset.
    restore_gettext_codeset(prev);

    // Must return number of menu items we added.
    परत ResultFromShort(idCmd-idCmdFirst);
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
    पूर्णांक gvimExtraOptions;

    // If HIWORD(lpcmi->lpVerb) then we have been called programmatically
    // and lpVerb is a command that should be invoked.  Otherwise, the shell
    // has called us, and LOWORD(lpcmi->lpVerb) is the menu ID the user has
    // selected.  Actually, it's (menu ID - idCmdFirst) from QueryContextMenu().
    जर (!HIWORD(lpcmi->lpVerb))
    {
	UINT idCmd = LOWORD(lpcmi->lpVerb);

	जर (idCmd >= m_edit_existing_off)
	{
	    // Existing with vim instance
	    hr = ढकला_To_Window(lpcmi->hwnd,
		    lpcmi->lpDirectory,
		    lpcmi->lpVerb,
		    lpcmi->lpParameters,
		    lpcmi->nShow,
		    idCmd - m_edit_existing_off);
	}
	अन्यथा
	{
	    निवड (idCmd)
	    {
		स्थिती 0:
		    gvimExtraOptions = EDIT_WITH_VIM_USE_TABPAGES;
		    थांबा;
		स्थिती 1:
		    gvimExtraOptions = EDIT_WITH_VIM_NO_OPTIONS;
		    थांबा;
		स्थिती 2:
		    gvimExtraOptions = EDIT_WITH_VIM_IN_DIFF_MODE;
		    थांबा;
		पूर्वनिर्धारित:
		    // If execution reaches this point we likely have an
		    // inconsistency between the code that setup the menus
		    // and this code that determines what the user
		    // selected.  This should be detected and fixed during
		    // development.
		    परत E_FAIL;
	    }

            LPCMINVOKECOMMANDINFOEX lpcmiex = (LPCMINVOKECOMMANDINFOEX)lpcmi;
            LPCWSTR currentDirectory = lpcmi->cbSize == आकार(CMINVOKECOMMANDINFOEX) ? lpcmiex->lpDirectoryW : रिक्त;

	    hr = InvokeSingleGvim(lpcmi->hwnd,
		    currentDirectory,
		    lpcmi->lpVerb,
		    lpcmi->lpParameters,
		    lpcmi->nShow,
		    gvimExtraOptions);
	}
    }
    परत hr;
}

STDMETHODIMP CShellExt::ढकला_To_Window(HWND  /* hParent */,
				   LPCSTR  /* pszWorkingDir */,
				   LPCSTR  /* pszCmd */,
				   LPCSTR  /* pszParam */,
				   पूर्णांक  /* iShowCmd */,
				   पूर्णांक idHWnd)
{
    HWND hWnd = m_hWnd[idHWnd];

    // Show and bring vim instance to foreground
    जर (IsIconic(hWnd) != 0)
	ShowWindow(hWnd, SW_RESTORE);
    अन्यथा
	ShowWindow(hWnd, SW_SHOW);
    SetForegroundWindow(hWnd);

    // Post the selected files to the vim instance
    PostMessage(hWnd, WM_DROPFILES, (WPARAM)medium.hGlobal, 0);

    परत NOERROR;
}

STDMETHODIMP CShellExt::GetCommandString(UINT_PTR  /* idCmd */,
					 UINT uFlags,
					 UINT FAR * /* reserved */,
					 LPSTR pszName,
					 UINT cchMax)
{
    // Use UTF-8 for gettext.
    अक्षर *prev = set_gettext_codeset();

    WCHAR *s = W(_("Edits the selected file(s) with Vim"));
    जर (uFlags == GCS_HELPTEXTW && cchMax > wcslen(s))
	wcscpy((WCHAR *)pszName, s);
    मुक्त(s);

    // Restore previous codeset.
    restore_gettext_codeset(prev);

    परत NOERROR;
}

BOOL CALLBACK CShellExt::EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    अक्षर temp[BUFSIZE];

    // First do a bunch of check
    // No invisible window
    जर (!IsWindowVisible(hWnd))
	परत TRUE;
    // No child window ???
    // if (GetParent(hWnd)) return TRUE;
    // Class name should be Vim, if failed to get class name, return
    जर (GetClassName(hWnd, temp, ARRAY_LENGTH(temp)) == 0)
	परत TRUE;
    // Compare class name to that of vim, if not, return
    जर (_strnicmp(temp, "vim", ARRAY_LENGTH("vim")) != 0)
	परत TRUE;
    // First check if the number of vim instance exceeds MAX_HWND
    CShellExt *cs = (CShellExt*) lParam;
    जर (cs->m_cntOfHWnd >= MAX_HWND)
	परत FALSE;	// stop enumeration
    // Now we get the vim window, put it into some kind of array
    cs->m_hWnd[cs->m_cntOfHWnd] = hWnd;
    cs->m_cntOfHWnd ++;

    परत TRUE; // continue enumeration (otherwise this would be false)
}

BOOL CShellExt::LoadMenuIcon()
{
    अक्षर vimExeFile[BUFSIZE];
    getGvimName(vimExeFile, TRUE);
    जर (vimExeFile[0] == '\0')
	परत FALSE;
    HICON hVimIcon;
    जर (ExtractIconEx(vimExeFile, 0, रिक्त, &hVimIcon, 1) == 0)
	परत FALSE;
    m_hVimIconBitmap = IconToBitmap(hVimIcon,
	    GetSysColorBrush(COLOR_MENU),
	    GetSystemMetrics(SM_CXSMICON),
	    GetSystemMetrics(SM_CYSMICON));
    परत TRUE;
}

    स्थिर अक्षर *
searchpath(अक्षर *name)
{
    स्थिर संघ {
	अक्षर	location[2 * BUFSIZE];
	WCHAR	wname[BUFSIZE];
    };
    WCHAR wlocation[BUFSIZE + 1];

    // There appears to be a bug in FindExecutableA() on Windows NT.
    // Use FindExecutableW() instead...
    MultiByteToWideChar(CP_ACP, 0, name, -1, wname, BUFSIZE);
    जर (FindExecutableW(wname, L"", wlocation) > (HINSTANCE)32)
    {
	WideCharToMultiByte(CP_ACP, 0, wlocation, -1,
		location, आकार(location), रिक्त, रिक्त);
	परत location;
    }
    परत (अक्षर *)"";
}


STDMETHODIMP CShellExt::InvokeSingleGvim(HWND hParent,
				   LPCWSTR  workingDir,
				   LPCSTR  /* pszCmd */,
				   LPCSTR  /* pszParam */,
				   पूर्णांक  /* iShowCmd */,
				   पूर्णांक gvimExtraOptions)
{
    ब_अक्षर_प्रकार	m_szFileUserClickedOn[BUFSIZE];
    ब_अक्षर_प्रकार	*cmdStrW;
    माप_प्रकार	cmdlen;
    माप_प्रकार	len;
    UINT i;

    cmdlen = BUFSIZE;
    cmdStrW  = (ब_अक्षर_प्रकार *) दो_स्मृति(cmdlen * आकार(ब_अक्षर_प्रकार));
    जर (cmdStrW == रिक्त)
	परत E_FAIL;
    getGvimInvocationW(cmdStrW);

    जर (gvimExtraOptions == EDIT_WITH_VIM_IN_DIFF_MODE)
	wcscat(cmdStrW, L" -d");
    अन्यथा जर (gvimExtraOptions == EDIT_WITH_VIM_USE_TABPAGES)
	wcscat(cmdStrW, L" -p");
    साठी (i = 0; i < cbFiles; i++)
    {
	DragQueryFileW((HDROP)medium.hGlobal,
		i,
		m_szFileUserClickedOn,
		ARRAY_LENGTH(m_szFileUserClickedOn));

	len = wcslen(cmdStrW) + wcslen(m_szFileUserClickedOn) + 4;
	जर (len > cmdlen)
	{
	    cmdlen = len + BUFSIZE;
	    ब_अक्षर_प्रकार *cmdStrW_new = (ब_अक्षर_प्रकार *)पुनः_स्मृति(cmdStrW, cmdlen * आकार(ब_अक्षर_प्रकार));
	    जर (cmdStrW_new == रिक्त)
	    {
		मुक्त(cmdStrW);
		परत E_FAIL;
	    }
	    cmdStrW = cmdStrW_new;
	}
	wcscat(cmdStrW, L" \"");
	wcscat(cmdStrW, m_szFileUserClickedOn);
	wcscat(cmdStrW, L"\"");
    }

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, आकार(si));
    si.cb = आकार(si);

    // Start the child process.
    जर (!CreateProcessW(रिक्त,	// No module name (use command line).
		cmdStrW,	// Command line.
		रिक्त,		// Process handle not inheritable.
		रिक्त,		// Thread handle not inheritable.
		FALSE,		// Set handle inheritance to FALSE.
		0,		// No creation flags.
		रिक्त,		// Use parent's environment block.
		workingDir,	// Use parent's starting directory.
		&si,		// Pointer to STARTUPINFO structure.
		&pi)		// Pointer to PROCESS_INFORMATION structure.
       )
    {
	// Use UTF-8 for gettext.
	अक्षर *prev = set_gettext_codeset();

	WCHAR *संदेश = W(_("Error creating process: Check if gvim is in your path!"));
	WCHAR *title = W(_("gvimext.dll error"));

	MessageBoxW(hParent, संदेश, title, MB_OK);

	मुक्त(संदेश);
	मुक्त(title);

	// Restore previous codeset.
	restore_gettext_codeset(prev);
    }
    अन्यथा
    {
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
    }
    मुक्त(cmdStrW);

    परत NOERROR;
}
