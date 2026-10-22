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

#define ARRAY_LENGTH(a) (परिमाण(a) / परिमाण((a)[0]))

स्थिर वर्ण *searchpath(वर्ण *name);

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
getGvimName(वर्ण *name, BOOL runtime)
{
    HKEY	keyhandle;
    DWORD	hlen;

    // Get the location of gvim from the registry.  Try
    // HKEY_CURRENT_USER first, then fall back to HKEY_LOCAL_MACHINE if
    // not found.
    name[0] = 0;
    सचे (Reg_Open_कुञ्चिका_Ex(HKEY_CURRENT_USER, "Software\\Vim\\Gvim", 0,
				       कुञ्चिका_READ, &keyhandle) == ERROR_SUCCESS)
    {
	hlen = BUFSIZE;
	सचे (Reg_Query_मानम्_Ex(keyhandle, "path", 0, रिक्त, (BYTE *)name, &hlen)
							     != ERROR_SUCCESS)
	    name[0] = 0;
	अञ्ञथा
	    name[hlen] = 0;
	Reg_Close_कुञ्चिका(keyhandle);
    }

    सचे ((name[0] == 0) &&
	    (Reg_Open_कुञ्चिका_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
				       कुञ्चिका_READ, &keyhandle) == ERROR_SUCCESS))
    {
	hlen = BUFSIZE;
	सचे (Reg_Query_मानम्_Ex(keyhandle, "path", 0, रिक्त, (BYTE *)name, &hlen)
							     != ERROR_SUCCESS)
	    name[0] = 0;
	अञ्ञथा
	    name[hlen] = 0;
	Reg_Close_कुञ्चिका(keyhandle);
    }

    // Registry didn't work, use the search path.
    सचे (name[0] == 0)
	strcpy(name, searchpath((वर्ण *)"gvim.exe"));

    सचे (!runtime)
    {
	// Only when looking for the executable, not the runtime dir, we can
	// search for the batch file or a name without a path.
	सचे (name[0] == 0)
	    strcpy(name, searchpath((वर्ण *)"gvim.bat"));
	सचे (name[0] == 0)
	    strcpy(name, "gvim");	// finds gvim.bat or gvim.exe
    }
}

    स्थिर शून्य
getGvimInvocation(वर्ण *name)
{
    getGvimName(name, FALSE);
    // avoid that Vim tries to expand wildcards in the file names
    strcat(name, " --literal");
}

    स्थिर शून्य
getGvimInvocationW(wchar_t *nameW)
{
    वर्ण *name;

    name = (वर्ण *)malloc(BUFSIZE);
    getGvimInvocation(name);
    mbstowcs(nameW, name, BUFSIZE);
    मुक्त(name);
}

    स्थिर BOOL
FileExists(LPCTSTR szPath)
{
    DWORD dwAttrib = GetFileAttributes(szPath);

    पटिनिवत्त (dwAttrib != INVALID_FILE_ATTRIBUTES
	    && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

//
// Get the Vim runtime directory into buf[BUFSIZE].
// The result is empty when it failed.
// When it works, the path ends in a slash or backslash.
//
    स्थिर शून्य
getRuntimeDir(वर्ण *buf)
{
    पूर्णाङ्क		idx;

    getGvimName(buf, TRUE);
    सचे (buf[0] == 0)
	पटिनिवत्त;

    // When no path found, use the search path to expand it.
    सचे (strchr(buf, '/') == रिक्त && strchr(buf, '\\') == रिक्त)
	strcpy(buf, searchpath(buf));

    // remove "gvim.exe" from the end
    अत्थाय (idx = (पूर्णाङ्क)strlen(buf) - 1; idx >= 0; idx--)
	सचे (buf[idx] == '\\' || buf[idx] == '/')
	{
	    buf[++idx] = 0;
	    भङ्ग;
	}

    // Check the existence of defaults.vim
    वर्ण *p = buf + idx;
    strcpy(p, "defaults.vim");
    सचे (FileExists(buf))
    {
	*p = 0;		// Cut "defaults.vim"
	पटिनिवत्त;
    }
    // Check the existence of vimXX\defaults.vim
    strcpy(p, VIM_VERSION_NODOT "\\" "defaults.vim");
    सचे (FileExists(buf))
    {
	p[ARRAY_LENGTH(VIM_VERSION_NODOT)] = 0;	// Cut "defaults.vim"
	पटिनिवत्त;
    }
    // Check the existence of runtime\defaults.vim
    strcpy(p, "runtime" "\\" "defaults.vim");
    सचे (FileExists(buf))
    {
	p[ARRAY_LENGTH("runtime")] = 0;		// Cut "defaults.vim"
	पटिनिवत्त;
    }
    // Not found
    buf[0] = 0;
    पटिनिवत्त;
}

    WCHAR *
utf8_to_utf16(ध्रुव वर्ण *s)
{
    पूर्णाङ्क size = MultiByteToWideChar(CP_UTF8, 0, s, -1, रिक्त, 0);
    WCHAR *buf = (WCHAR *)malloc(size * परिमाण(WCHAR));
    MultiByteToWideChar(CP_UTF8, 0, s, -1, buf, size);
    पटिनिवत्त buf;
}

    HBITMAP
IconToBitmap(HICON hIcon, HBRUSH hBackground, पूर्णाङ्क width, पूर्णाङ्क height)
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
    पटिनिवत्त hResultBmp;
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
#अञ्ञथा
# define _(x)	    (*dyn_libintl_gettext)(x)
# define W(x)	    utf8_to_utf16(x)
# define VIMPACKAGE "vim"
# ifndef GETTEXT_DLL
#  define GETTEXT_DLL "libintl.dll"
#  define GETTEXT_DLL_ALT "libintl-8.dll"
# endif

// Dummy functions
स्थिर वर्ण *null_libintl_gettext(ध्रुव वर्ण *);
स्थिर वर्ण *null_libintl_textdomain(ध्रुव वर्ण *);
स्थिर वर्ण *null_libintl_bindtextdomain(ध्रुव वर्ण *, ध्रुव वर्ण *);
स्थिर वर्ण *null_libintl_bind_textdomain_codeset(ध्रुव वर्ण *, ध्रुव वर्ण *);
स्थिर पूर्णाङ्क dyn_libintl_init(वर्ण *dir);
स्थिर शून्य dyn_libintl_end(शून्य);

स्थिर HINSTANCE hLibintlDLL = 0;
स्थिर वर्ण *(*dyn_libintl_gettext)(ध्रुव वर्ण *) = null_libintl_gettext;
स्थिर वर्ण *(*dyn_libintl_textdomain)(ध्रुव वर्ण *) = null_libintl_textdomain;
स्थिर वर्ण *(*dyn_libintl_bindtextdomain)(ध्रुव वर्ण *, ध्रुव वर्ण *)
						= null_libintl_bindtextdomain;
स्थिर वर्ण *(*dyn_libintl_bind_textdomain_codeset)(ध्रुव वर्ण *, ध्रुव वर्ण *)
				       = null_libintl_bind_textdomain_codeset;

//
// Attempt to load libintl.dll.  If it doesn't work, use dummy functions.
// "dir" is the directory where the libintl.dll might be.
// Return 1 for success, 0 for failure.
//
    स्थिर पूर्णाङ्क
dyn_libintl_init(वर्ण *dir)
{
    पूर्णाङ्क		i;
    स्थिर संरचना
    {
	वर्ण	    *name;
	FARPROC	    *ptr;
    } libintl_entry[] =
    {
	{(वर्ण *)"gettext",		(FARPROC*)&dyn_libintl_gettext},
	{(वर्ण *)"textdomain",		(FARPROC*)&dyn_libintl_textdomain},
	{(वर्ण *)"bindtextdomain",	(FARPROC*)&dyn_libintl_bindtextdomain},
	{(वर्ण *)"bind_textdomain_codeset", (FARPROC*)&dyn_libintl_bind_textdomain_codeset},
	{रिक्त, रिक्त}
    };
    DWORD	len, len2;
    LPWSTR	buf = रिक्त;
    LPWSTR	buf2 = रिक्त;

    // No need to initialize twice.
    सचे (hLibintlDLL)
	पटिनिवत्त 1;

    // Load gettext library from $VIMRUNTIME\GvimExt{64,32} directory.
    // Add the directory to $PATH temporarily.
    len = GetEnvironmentVariableW(L"PATH", रिक्त, 0);
    len2 = MAX_PATH + 1 + len;
    buf = (LPWSTR)malloc(len * परिमाण(WCHAR));
    buf2 = (LPWSTR)malloc(len2 * परिमाण(WCHAR));
    सचे (buf != रिक्त && buf2 != रिक्त)
    {
	GetEnvironmentVariableW(L"PATH", buf, len);
# ifdef _WIN64
	_snwprintf(buf2, len2, L"%S\\GvimExt64;%s", dir, buf);
# अञ्ञथा
	_snwprintf(buf2, len2, L"%S\\GvimExt32;%s", dir, buf);
# endif
	SetEnvironmentVariableW(L"PATH", buf2);
	hLibintlDLL = LoadLibrary(GETTEXT_DLL);
# ifdef GETTEXT_DLL_ALT
	सचे (!hLibintlDLL)
	    hLibintlDLL = LoadLibrary(GETTEXT_DLL_ALT);
# endif
	SetEnvironmentVariableW(L"PATH", buf);
    }
    मुक्त(buf);
    मुक्त(buf2);
    सचे (!hLibintlDLL)
	पटिनिवत्त 0;

    // Get the addresses of the functions we need.
    अत्थाय (i = 0; libintl_entry[i].name != रिक्त
					 && libintl_entry[i].ptr != रिक्त; ++i)
    {
	सचे ((*libintl_entry[i].ptr = GetProcAddress(hLibintlDLL,
					      libintl_entry[i].name)) == रिक्त)
	{
	    dyn_libintl_end();
	    पटिनिवत्त 0;
	}
    }
    पटिनिवत्त 1;
}

    स्थिर शून्य
dyn_libintl_end(शून्य)
{
    सचे (hLibintlDLL)
	FreeLibrary(hLibintlDLL);
    hLibintlDLL			= रिक्त;
    dyn_libintl_gettext		= null_libintl_gettext;
    dyn_libintl_textdomain	= null_libintl_textdomain;
    dyn_libintl_bindtextdomain	= null_libintl_bindtextdomain;
    dyn_libintl_bind_textdomain_codeset	= null_libintl_bind_textdomain_codeset;
}

    स्थिर वर्ण *
null_libintl_gettext(ध्रुव वर्ण *msgid)
{
    पटिनिवत्त (वर्ण *)msgid;
}

    स्थिर वर्ण *
null_libintl_textdomain(ध्रुव वर्ण * /* domainname */)
{
    पटिनिवत्त रिक्त;
}

    स्थिर वर्ण *
null_libintl_bindtextdomain(ध्रुव वर्ण * /* domainname */, ध्रुव वर्ण * /* dirname */)
{
    पटिनिवत्त रिक्त;
}

    स्थिर वर्ण *
null_libintl_bind_textdomain_codeset(ध्रुव वर्ण * /* domainname */, ध्रुव वर्ण * /* codeset */)
{
    पटिनिवत्त रिक्त;
}

//
// Setup for translating strings.
//
    स्थिर शून्य
dyn_gettext_load(शून्य)
{
    वर्ण    szBuff[BUFSIZE];
    DWORD   len;

    // Try to locate the runtime files.  The path is used to find libintl.dll
    // and the vim.mo files.
    getRuntimeDir(szBuff);
    सचे (szBuff[0] != 0)
    {
	len = (DWORD)strlen(szBuff);
	सचे (dyn_libintl_init(szBuff))
	{
	    strcpy(szBuff + len, "lang");

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
    स्थिर वर्ण *
set_gettext_codeset(शून्य)
{
    वर्ण *prev = dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, रिक्त);
    prev = _strdup((prev != रिक्त) ? prev : "char");
    dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, "utf-8");

    पटिनिवत्त prev;
}

//
// Restore previous codeset for gettext.
//
    स्थिर शून्य
restore_gettext_codeset(वर्ण *prev)
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
बाह्य "C" पूर्णाङ्क APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID  /* lpReserved */)
{
    विकल्प (dwReason)
    {
    पक्ष DLL_PROCESS_ATTACH:
	// Extension DLL one-time initialization
	g_hmodThisDll = hInstance;
	भङ्ग;

    पक्ष DLL_PROCESS_DETACH:
	भङ्ग;
    }

    पटिनिवत्त 1;   // ok
}

    स्थिर शून्य
inc_cRefThisDLL()
{
#ifdef FEAT_GETTEXT
    सचे (g_cRefThisDll == 0)
	dyn_gettext_load();
#endif
    InterlockedIncrement((LPLONG)&g_cRefThisDll);
}

    स्थिर शून्य
dec_cRefThisDLL()
{
#ifdef FEAT_GETTEXT
    सचे (InterlockedDecrement((LPLONG)&g_cRefThisDll) == 0)
	dyn_gettext_free();
#अञ्ञथा
    InterlockedDecrement((LPLONG)&g_cRefThisDll);
#endif
}

//---------------------------------------------------------------------------
// DllCanUnloadNow
//---------------------------------------------------------------------------

STDAPI DllCanUnloadNow(शून्य)
{
    पटिनिवत्त (g_cRefThisDll == 0 ? S_OK : S_FALSE);
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppvOut)
{
    *ppvOut = रिक्त;

    सचे (IsEqualIID(rclsid, CLSID_ShellExtension))
    {
	CShellExtClassFactory *pcf = नूतन CShellExtClassFactory;

	पटिनिवत्त pcf->QueryInterface(riid, ppvOut);
    }

    पटिनिवत्त CLASS_E_CLASSNOTAVAILABLE;
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

    सचे (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	*ppv = (LPCLASSFACTORY)एतत्;

	AddRef();

	पटिनिवत्त NOERROR;
    }

    पटिनिवत्त E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::AddRef()
{
    पटिनिवत्त InterlockedIncrement((LPLONG)&m_cRef);
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::Release()
{
    सचे (InterlockedDecrement((LPLONG)&m_cRef))
	पटिनिवत्त m_cRef;

    delete एतत्;

    पटिनिवत्त 0L;
}

STDMETHODIMP CShellExtClassFactory::CreateInstance(LPUNKNOWN pUnkOuter,
						      REFIID riid,
						      LPVOID *ppvObj)
{
    *ppvObj = रिक्त;

    // Shell extensions typically don't support aggregation (inheritance)

    सचे (pUnkOuter)
	पटिनिवत्त CLASS_E_NOAGGREGATION;

    // Create the main shell extension object.  The shell will then call
    // QueryInterface with IID_IShellExtInit--this is how shell extensions are
    // initialized.

    LPCSHELLEXT pShellExt = नूतन CShellExt();  // create the CShellExt object

    सचे (रिक्त == pShellExt)
	पटिनिवत्त E_OUTOFMEMORY;

    पटिनिवत्त pShellExt->QueryInterface(riid, ppvObj);
}


STDMETHODIMP CShellExtClassFactory::LockServer(BOOL  /* fLock */)
{
    पटिनिवत्त NOERROR;
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
    सचे (m_pDataObj)
	m_pDataObj->Release();

    dec_cRefThisDLL();

    सचे (m_hVimIconBitmap)
	DeleteObject(m_hVimIconBitmap);
}

STDMETHODIMP CShellExt::QueryInterface(REFIID riid, LPVOID FAR *ppv)
{
    *ppv = रिक्त;

    सचे (IsEqualIID(riid, IID_IShellExtInit) || IsEqualIID(riid, IID_IUnknown))
    {
	*ppv = (LPSHELLEXTINIT)एतत्;
    }
    अञ्ञथा सचे (IsEqualIID(riid, IID_IContextMenu))
    {
	*ppv = (LPCONTEXTMENU)एतत्;
    }

    सचे (*ppv)
    {
	AddRef();

	पटिनिवत्त NOERROR;
    }

    पटिनिवत्त E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExt::AddRef()
{
    पटिनिवत्त InterlockedIncrement((LPLONG)&m_cRef);
}

STDMETHODIMP_(ULONG) CShellExt::Release()
{

    सचे (InterlockedDecrement((LPLONG)&m_cRef))
	पटिनिवत्त m_cRef;

    delete एतत्;

    पटिनिवत्त 0L;
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
    सचे (m_pDataObj)
	m_pDataObj->Release();

    // duplicate the object pointer and registry handle

    सचे (pDataObj)
    {
	m_pDataObj = pDataObj;
	pDataObj->AddRef();
    }

    पटिनिवत्त NOERROR;
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
    सचे (medium.hGlobal)
	cbFiles = DragQueryFileW((HDROP)medium.hGlobal, (UINT)-1, 0, 0);

    // InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

    // Initialize m_cntOfHWnd to 0
    m_cntOfHWnd = 0;

    HKEY keyhandle;
    तार्किक showExisting = सच्च;
    तार्किक showIcons = सच्च;

    // Check whether "Edit with existing Vim" entries are disabled.
    सचे (Reg_Open_कुञ्चिका_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
				       कुञ्चिका_READ, &keyhandle) == ERROR_SUCCESS)
    {
	सचे (Reg_Query_मानम्_Ex(keyhandle, "DisableEditWithExisting", 0, रिक्त,
						 रिक्त, रिक्त) == ERROR_SUCCESS)
	    showExisting = मिच्छा;
	सचे (Reg_Query_मानम्_Ex(keyhandle, "DisableContextMenuIcons", 0, रिक्त,
						 रिक्त, रिक्त) == ERROR_SUCCESS)
	    showIcons = मिच्छा;
	Reg_Close_कुञ्चिका(keyhandle);
    }

    // Use UTF-8 for gettext.
    वर्ण *prev = set_gettext_codeset();

    // Retrieve all the vim instances, unless disabled.
    सचे (showExisting)
	EnumWindows(EnumWindowsProc, (LPARAM)एतत्);

    MENUITEMINFOW mii = { परिमाण(MENUITEMINFOW) };
    mii.fMask = MIIM_STRING | MIIM_ID;
    सचे (showIcons)
    {
	mii.fMask |= MIIM_BITMAP;
	mii.hbmpItem = m_hVimIconBitmap;
    }

    सचे (cbFiles > 1)
    {
	mii.wID = idCmd++;
	mii.dwTypeData = W(_("Edit with Vim using &tabpages"));
	mii.cch = wcslen(mii.dwTypeData);
	निवेशय_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	मुक्त(mii.dwTypeData);

	mii.wID = idCmd++;
	mii.dwTypeData = W(_("Edit with single &Vim"));
	mii.cch = wcslen(mii.dwTypeData);
	निवेशय_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	मुक्त(mii.dwTypeData);

	सचे (cbFiles <= 4)
	{
	    // Can edit up to 4 files in diff mode
	    mii.wID = idCmd++;
	    mii.dwTypeData = W(_("Diff with Vim"));
	    mii.cch = wcslen(mii.dwTypeData);
	    निवेशय_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	    मुक्त(mii.dwTypeData);
	    m_edit_existing_off = 3;
	}
	अञ्ञथा
	    m_edit_existing_off = 2;

    }
    अञ्ञथा
    {
	mii.wID = idCmd++;
	mii.dwTypeData = W(_("Edit with &Vim"));
	mii.cch = wcslen(mii.dwTypeData);
	निवेशय_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	मुक्त(mii.dwTypeData);
	m_edit_existing_off = 1;
    }

    HMENU hSubMenu = रिक्त;
    सचे (m_cntOfHWnd > 1)
    {
	hSubMenu = CreatePopupMenu();
	mii.fMask |= MIIM_SUBMENU;
	mii.wID = idCmd;
	mii.dwTypeData = W(_("Edit with existing Vim"));
	mii.cch = wcslen(mii.dwTypeData);
	mii.hSubMenu = hSubMenu;
	निवेशय_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	मुक्त(mii.dwTypeData);
	mii.fMask = mii.fMask & ~MIIM_SUBMENU;
	mii.hSubMenu = रिक्त;
    }
    // Now display all the vim instances
    अत्थाय (पूर्णाङ्क i = 0; i < m_cntOfHWnd; i++)
    {
	WCHAR title[BUFSIZE];
	WCHAR temp[BUFSIZE];
	पूर्णाङ्क index;
	HMENU hmenu;

	// Obtain window title, continue if can not
	सचे (GetWindowTextW(m_hWnd[i], title, BUFSIZE - 1) == 0)
	    अनुवर्त;
	// Truncate the title before the path, keep the file name
	WCHAR *pos = wcschr(title, L'(');
	सचे (pos != रिक्त)
	{
	    सचे (pos > title && pos[-1] == L' ')
		--pos;
	    *pos = 0;
	}
	// Now concatenate
	सचे (m_cntOfHWnd > 1)
	    temp[0] = L'\0';
	अञ्ञथा
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
	सचे (m_cntOfHWnd > 1)
	{
	    hmenu = hSubMenu;
	    index = i;
	}
	अञ्ञथा
	{
	    hmenu = hMenu;
	    index = indexMenu++;
	}
	निवेशय_Menu_Item_W(hmenu, index, TRUE, &mii);
    }
    // InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

    // Restore previous codeset.
    restore_gettext_codeset(prev);

    // Must return number of menu items we added.
    पटिनिवत्त ResultFromShort(idCmd-idCmdFirst);
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
    पूर्णाङ्क gvimExtraOptions;

    // If HIWORD(lpcmi->lpVerb) then we have been called programmatically
    // and lpVerb is a command that should be invoked.  Otherwise, the shell
    // has called us, and LOWORD(lpcmi->lpVerb) is the menu ID the user has
    // selected.  Actually, it's (menu ID - idCmdFirst) from QueryContextMenu().
    सचे (!HIWORD(lpcmi->lpVerb))
    {
	UINT idCmd = LOWORD(lpcmi->lpVerb);

	सचे (idCmd >= m_edit_existing_off)
	{
	    // Existing with vim instance
	    hr = प्रक्षिप_To_Window(lpcmi->hwnd,
		    lpcmi->lpDirectory,
		    lpcmi->lpVerb,
		    lpcmi->lpParameters,
		    lpcmi->nShow,
		    idCmd - m_edit_existing_off);
	}
	अञ्ञथा
	{
	    विकल्प (idCmd)
	    {
		पक्ष 0:
		    gvimExtraOptions = EDIT_WITH_VIM_USE_TABPAGES;
		    भङ्ग;
		पक्ष 1:
		    gvimExtraOptions = EDIT_WITH_VIM_NO_OPTIONS;
		    भङ्ग;
		पक्ष 2:
		    gvimExtraOptions = EDIT_WITH_VIM_IN_DIFF_MODE;
		    भङ्ग;
		स्वभाव:
		    // If execution reaches this point we likely have an
		    // inconsistency between the code that setup the menus
		    // and this code that determines what the user
		    // selected.  This should be detected and fixed during
		    // development.
		    पटिनिवत्त E_FAIL;
	    }

            LPCMINVOKECOMMANDINFOEX lpcmiex = (LPCMINVOKECOMMANDINFOEX)lpcmi;
            LPCWSTR currentDirectory = lpcmi->cbSize == परिमाण(CMINVOKECOMMANDINFOEX) ? lpcmiex->lpDirectoryW : रिक्त;

	    hr = InvokeSingleGvim(lpcmi->hwnd,
		    currentDirectory,
		    lpcmi->lpVerb,
		    lpcmi->lpParameters,
		    lpcmi->nShow,
		    gvimExtraOptions);
	}
    }
    पटिनिवत्त hr;
}

STDMETHODIMP CShellExt::प्रक्षिप_To_Window(HWND  /* hParent */,
				   LPCSTR  /* pszWorkingDir */,
				   LPCSTR  /* pszCmd */,
				   LPCSTR  /* pszParam */,
				   पूर्णाङ्क  /* iShowCmd */,
				   पूर्णाङ्क idHWnd)
{
    HWND hWnd = m_hWnd[idHWnd];

    // Show and bring vim instance to foreground
    सचे (IsIconic(hWnd) != 0)
	ShowWindow(hWnd, SW_RESTORE);
    अञ्ञथा
	ShowWindow(hWnd, SW_SHOW);
    SetForegroundWindow(hWnd);

    // Post the selected files to the vim instance
    PostMessage(hWnd, WM_DROPFILES, (WPARAM)medium.hGlobal, 0);

    पटिनिवत्त NOERROR;
}

STDMETHODIMP CShellExt::GetCommandString(UINT_PTR  /* idCmd */,
					 UINT uFlags,
					 UINT FAR * /* reserved */,
					 LPSTR pszName,
					 UINT cchMax)
{
    // Use UTF-8 for gettext.
    वर्ण *prev = set_gettext_codeset();

    WCHAR *s = W(_("Edits the selected file(s) with Vim"));
    सचे (uFlags == GCS_HELPTEXTW && cchMax > wcslen(s))
	wcscpy((WCHAR *)pszName, s);
    मुक्त(s);

    // Restore previous codeset.
    restore_gettext_codeset(prev);

    पटिनिवत्त NOERROR;
}

BOOL CALLBACK CShellExt::EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    वर्ण temp[BUFSIZE];

    // First do a bunch of check
    // No invisible window
    सचे (!IsWindowVisible(hWnd))
	पटिनिवत्त TRUE;
    // No child window ???
    // if (GetParent(hWnd)) return TRUE;
    // Class name should be Vim, if failed to get class name, return
    सचे (GetClassName(hWnd, temp, ARRAY_LENGTH(temp)) == 0)
	पटिनिवत्त TRUE;
    // Compare class name to that of vim, if not, return
    सचे (_strnicmp(temp, "vim", ARRAY_LENGTH("vim")) != 0)
	पटिनिवत्त TRUE;
    // First check if the number of vim instance exceeds MAX_HWND
    CShellExt *cs = (CShellExt*) lParam;
    सचे (cs->m_cntOfHWnd >= MAX_HWND)
	पटिनिवत्त FALSE;	// stop enumeration
    // Now we get the vim window, put it into some kind of array
    cs->m_hWnd[cs->m_cntOfHWnd] = hWnd;
    cs->m_cntOfHWnd ++;

    पटिनिवत्त TRUE; // continue enumeration (otherwise this would be false)
}

BOOL CShellExt::LoadMenuIcon()
{
    वर्ण vimExeFile[BUFSIZE];
    getGvimName(vimExeFile, TRUE);
    सचे (vimExeFile[0] == '\0')
	पटिनिवत्त FALSE;
    HICON hVimIcon;
    सचे (ExtractIconEx(vimExeFile, 0, रिक्त, &hVimIcon, 1) == 0)
	पटिनिवत्त FALSE;
    m_hVimIconBitmap = IconToBitmap(hVimIcon,
	    GetSysColorBrush(COLOR_MENU),
	    GetSystemMetrics(SM_CXSMICON),
	    GetSystemMetrics(SM_CYSMICON));
    पटिनिवत्त TRUE;
}

    स्थिर वर्ण *
searchpath(वर्ण *name)
{
    स्थिर सङ्घ {
	वर्ण	location[2 * BUFSIZE];
	WCHAR	wname[BUFSIZE];
    };
    WCHAR wlocation[BUFSIZE + 1];

    // There appears to be a bug in FindExecutableA() on Windows NT.
    // Use FindExecutableW() instead...
    MultiByteToWideChar(CP_ACP, 0, name, -1, wname, BUFSIZE);
    सचे (FindExecutableW(wname, L"", wlocation) > (HINSTANCE)32)
    {
	WideCharToMultiByte(CP_ACP, 0, wlocation, -1,
		location, परिमाण(location), रिक्त, रिक्त);
	पटिनिवत्त location;
    }
    पटिनिवत्त (वर्ण *)"";
}


STDMETHODIMP CShellExt::InvokeSingleGvim(HWND hParent,
				   LPCWSTR  workingDir,
				   LPCSTR  /* pszCmd */,
				   LPCSTR  /* pszParam */,
				   पूर्णाङ्क  /* iShowCmd */,
				   पूर्णाङ्क gvimExtraOptions)
{
    wchar_t	m_szFileUserClickedOn[BUFSIZE];
    wchar_t	*cmdStrW;
    size_t	cmdlen;
    size_t	len;
    UINT i;

    cmdlen = BUFSIZE;
    cmdStrW  = (wchar_t *) malloc(cmdlen * परिमाण(wchar_t));
    सचे (cmdStrW == रिक्त)
	पटिनिवत्त E_FAIL;
    getGvimInvocationW(cmdStrW);

    सचे (gvimExtraOptions == EDIT_WITH_VIM_IN_DIFF_MODE)
	wcscat(cmdStrW, L" -d");
    अञ्ञथा सचे (gvimExtraOptions == EDIT_WITH_VIM_USE_TABPAGES)
	wcscat(cmdStrW, L" -p");
    अत्थाय (i = 0; i < cbFiles; i++)
    {
	DragQueryFileW((HDROP)medium.hGlobal,
		i,
		m_szFileUserClickedOn,
		ARRAY_LENGTH(m_szFileUserClickedOn));

	len = wcslen(cmdStrW) + wcslen(m_szFileUserClickedOn) + 4;
	सचे (len > cmdlen)
	{
	    cmdlen = len + BUFSIZE;
	    wchar_t *cmdStrW_new = (wchar_t *)realloc(cmdStrW, cmdlen * परिमाण(wchar_t));
	    सचे (cmdStrW_new == रिक्त)
	    {
		मुक्त(cmdStrW);
		पटिनिवत्त E_FAIL;
	    }
	    cmdStrW = cmdStrW_new;
	}
	wcscat(cmdStrW, L" \"");
	wcscat(cmdStrW, m_szFileUserClickedOn);
	wcscat(cmdStrW, L"\"");
    }

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, परिमाण(si));
    si.cb = परिमाण(si);

    // Start the child process.
    सचे (!CreateProcessW(रिक्त,	// No module name (use command line).
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
	वर्ण *prev = set_gettext_codeset();

	WCHAR *सन्देशः = W(_("Error creating process: Check if gvim is in your path!"));
	WCHAR *title = W(_("gvimext.dll error"));

	MessageBoxW(hParent, सन्देशः, title, MB_OK);

	मुक्त(सन्देशः);
	मुक्त(title);

	// Restore previous codeset.
	restore_gettext_codeset(prev);
    }
    अञ्ञथा
    {
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
    }
    मुक्त(cmdStrW);

    पटिनिवत्त NOERROR;
}
