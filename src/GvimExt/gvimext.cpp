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

#অন্তর্ভাব "gvimext.h"
#অন্তর্ভাব "../version.h"

#define ARRAY_দৈর্ঘ্য(a) (আকার(a) / আকার((a)[0]))

স্থির অক্ষর *searchpath(অক্ষর *name);

// Always get an error while putting the following stuff to the
// gvimext.h file as class protected variables, give up and
// declare them as global stuff
FORMATETC fmte = {CF_HDROP,
		  (DVTARGETDEVICE FAR *)ফাঁকা,
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
getGvimName(অক্ষর *name, BOOL runtime)
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
	যদি (Reg_Query_মান_Ex(keyhandle, "path", 0, ফাঁকা, (BYTE *)name, &hlen)
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
	যদি (Reg_Query_মান_Ex(keyhandle, "path", 0, ফাঁকা, (BYTE *)name, &hlen)
							     != ERROR_SUCCESS)
	    name[0] = 0;
	অন্যথা
	    name[hlen] = 0;
	Reg_Close_চাবি(keyhandle);
    }

    // Registry didn't work, use the search path.
    যদি (name[0] == 0)
	strcpy(name, searchpath((অক্ষর *)"gvim.exe"));

    যদি (!runtime)
    {
	// Only when looking for the executable, not the runtime dir, we can
	// search for the batch file or a name without a path.
	যদি (name[0] == 0)
	    strcpy(name, searchpath((অক্ষর *)"gvim.bat"));
	যদি (name[0] == 0)
	    strcpy(name, "gvim");	// finds gvim.bat or gvim.exe
    }
}

    স্থির শূন্য
getGvimInvocation(অক্ষর *name)
{
    getGvimName(name, FALSE);
    // avoid that Vim tries to expand wildcards in the file names
    strcat(name, " --literal");
}

    স্থির শূন্য
getGvimInvocationW(wchar_t *nameW)
{
    অক্ষর *name;

    name = (অক্ষর *)malloc(BUFSIZE);
    getGvimInvocation(name);
    mbstowcs(nameW, name, BUFSIZE);
    মুক্ত(name);
}

    স্থির BOOL
FileExists(LPCTSTR szPath)
{
    DWORD dwAttrib = GetFileAttributes(szPath);

    ফেরত (dwAttrib != INVALID_FILE_ATTRIBUTES
	    && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

//
// Get the Vim runtime directory into buf[BUFSIZE].
// The result is empty when it failed.
// When it works, the path ends in a slash or backslash.
//
    স্থির শূন্য
getRuntimeDir(অক্ষর *buf)
{
    পূর্ণসংখ্যা		idx;

    getGvimName(buf, TRUE);
    যদি (buf[0] == 0)
	ফেরত;

    // When no path found, use the search path to expand it.
    যদি (strchr(buf, '/') == ফাঁকা && strchr(buf, '\\') == ফাঁকা)
	strcpy(buf, searchpath(buf));

    // remove "gvim.exe" from the end
    জন্য (idx = (পূর্ণসংখ্যা)strlen(buf) - 1; idx >= 0; idx--)
	যদি (buf[idx] == '\\' || buf[idx] == '/')
	{
	    buf[++idx] = 0;
	    বিরতি;
	}

    // Check the existence of defaults.vim
    অক্ষর *p = buf + idx;
    strcpy(p, "defaults.vim");
    যদি (FileExists(buf))
    {
	*p = 0;		// Cut "defaults.vim"
	ফেরত;
    }
    // Check the existence of vimXX\defaults.vim
    strcpy(p, VIM_VERSION_NODOT "\\" "defaults.vim");
    যদি (FileExists(buf))
    {
	p[ARRAY_দৈর্ঘ্য(VIM_VERSION_NODOT)] = 0;	// Cut "defaults.vim"
	ফেরত;
    }
    // Check the existence of runtime\defaults.vim
    strcpy(p, "runtime" "\\" "defaults.vim");
    যদি (FileExists(buf))
    {
	p[ARRAY_দৈর্ঘ্য("runtime")] = 0;		// Cut "defaults.vim"
	ফেরত;
    }
    // Not found
    buf[0] = 0;
    ফেরত;
}

    WCHAR *
utf8_to_utf16(ধ্রুবক অক্ষর *s)
{
    পূর্ণসংখ্যা আকার_2 = MultiByteToWideChar(CP_UTF8, 0, s, -1, ফাঁকা, 0);
    WCHAR *buf = (WCHAR *)malloc(আকার_2 * আকার(WCHAR));
    MultiByteToWideChar(CP_UTF8, 0, s, -1, buf, আকার_2);
    ফেরত buf;
}

    HBITMAP
IconToBitmap(HICON hIcon, HBRUSH hBackground, পূর্ণসংখ্যা width, পূর্ণসংখ্যা height)
{
    HDC hDC = GetDC(ফাঁকা);
    HDC hMemDC = CreateCompatibleDC(hDC);
    HBITMAP hMemBmp = CreateCompatibleBitmap(hDC, width, height);
    HBITMAP h_ফলাফল_Bmp = ফাঁকা;
    HGDIOBJ hOrgBMP = SelectObject(hMemDC, hMemBmp);

    DrawIconEx(hMemDC, 0, 0, hIcon, width, height, 0, hBackground, DI_NORMAL);

    h_ফলাফল_Bmp = hMemBmp;
    hMemBmp = ফাঁকা;

    SelectObject(hMemDC, hOrgBMP);
    DeleteDC(hMemDC);
    ReleaseDC(ফাঁকা, hDC);
    DestroyIcon(hIcon);
    ফেরত h_ফলাফল_Bmp;
}

//
// GETTEXT: translated messages and menu entries
//
#ifndef FEAT_GETTEXT
# define _(x)	    x
# define W_impl(x) _wcsdup(L##x)
# define W(x)	    W_impl(x)
# define set_gettext_codeset()	    ফাঁকা
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
স্থির অক্ষর *null_libintl_gettext(ধ্রুবক অক্ষর *);
স্থির অক্ষর *null_libintl_textdomain(ধ্রুবক অক্ষর *);
স্থির অক্ষর *null_libintl_bindtextdomain(ধ্রুবক অক্ষর *, ধ্রুবক অক্ষর *);
স্থির অক্ষর *null_libintl_bind_textdomain_codeset(ধ্রুবক অক্ষর *, ধ্রুবক অক্ষর *);
স্থির পূর্ণসংখ্যা dyn_libintl_init(অক্ষর *dir);
স্থির শূন্য dyn_libintl_end(শূন্য);

স্থির HINSTANCE hLibintlDLL = 0;
স্থির অক্ষর *(*dyn_libintl_gettext)(ধ্রুবক অক্ষর *) = null_libintl_gettext;
স্থির অক্ষর *(*dyn_libintl_textdomain)(ধ্রুবক অক্ষর *) = null_libintl_textdomain;
স্থির অক্ষর *(*dyn_libintl_bindtextdomain)(ধ্রুবক অক্ষর *, ধ্রুবক অক্ষর *)
						= null_libintl_bindtextdomain;
স্থির অক্ষর *(*dyn_libintl_bind_textdomain_codeset)(ধ্রুবক অক্ষর *, ধ্রুবক অক্ষর *)
				       = null_libintl_bind_textdomain_codeset;

//
// Attempt to load libintl.dll.  If it doesn't work, use dummy functions.
// "dir" is the directory where the libintl.dll might be.
// Return 1 for success, 0 for failure.
//
    স্থির পূর্ণসংখ্যা
dyn_libintl_init(অক্ষর *dir)
{
    পূর্ণসংখ্যা		i;
    স্থির কাঠামো
    {
	অক্ষর	    *name;
	FARPROC	    *ptr;
    } libintl_entry[] =
    {
	{(অক্ষর *)"gettext",		(FARPROC*)&dyn_libintl_gettext},
	{(অক্ষর *)"textdomain",		(FARPROC*)&dyn_libintl_textdomain},
	{(অক্ষর *)"bindtextdomain",	(FARPROC*)&dyn_libintl_bindtextdomain},
	{(অক্ষর *)"bind_textdomain_codeset", (FARPROC*)&dyn_libintl_bind_textdomain_codeset},
	{ফাঁকা, ফাঁকা}
    };
    DWORD	len, len2;
    LPWSTR	buf = ফাঁকা;
    LPWSTR	buf2 = ফাঁকা;

    // No need to initialize twice.
    যদি (hLibintlDLL)
	ফেরত 1;

    // Load gettext library from $VIMRUNTIME\GvimExt{64,32} directory.
    // Add the directory to $PATH temporarily.
    len = GetEnvironmentVariableW(L"PATH", ফাঁকা, 0);
    len2 = MAX_PATH + 1 + len;
    buf = (LPWSTR)malloc(len * আকার(WCHAR));
    buf2 = (LPWSTR)malloc(len2 * আকার(WCHAR));
    যদি (buf != ফাঁকা && buf2 != ফাঁকা)
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
	ফেরত 0;

    // Get the addresses of the functions we need.
    জন্য (i = 0; libintl_entry[i].name != ফাঁকা
					 && libintl_entry[i].ptr != ফাঁকা; ++i)
    {
	যদি ((*libintl_entry[i].ptr = GetProcAddress(hLibintlDLL,
					      libintl_entry[i].name)) == ফাঁকা)
	{
	    dyn_libintl_end();
	    ফেরত 0;
	}
    }
    ফেরত 1;
}

    স্থির শূন্য
dyn_libintl_end(শূন্য)
{
    যদি (hLibintlDLL)
	FreeLibrary(hLibintlDLL);
    hLibintlDLL			= ফাঁকা;
    dyn_libintl_gettext		= null_libintl_gettext;
    dyn_libintl_textdomain	= null_libintl_textdomain;
    dyn_libintl_bindtextdomain	= null_libintl_bindtextdomain;
    dyn_libintl_bind_textdomain_codeset	= null_libintl_bind_textdomain_codeset;
}

    স্থির অক্ষর *
null_libintl_gettext(ধ্রুবক অক্ষর *msgid)
{
    ফেরত (অক্ষর *)msgid;
}

    স্থির অক্ষর *
null_libintl_textdomain(ধ্রুবক অক্ষর * /* domainname */)
{
    ফেরত ফাঁকা;
}

    স্থির অক্ষর *
null_libintl_bindtextdomain(ধ্রুবক অক্ষর * /* domainname */, ধ্রুবক অক্ষর * /* dirname */)
{
    ফেরত ফাঁকা;
}

    স্থির অক্ষর *
null_libintl_bind_textdomain_codeset(ধ্রুবক অক্ষর * /* domainname */, ধ্রুবক অক্ষর * /* codeset */)
{
    ফেরত ফাঁকা;
}

//
// Setup for translating strings.
//
    স্থির শূন্য
dyn_gettext_load(শূন্য)
{
    অক্ষর    szBuff[BUFSIZE];
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
    স্থির অক্ষর *
set_gettext_codeset(শূন্য)
{
    অক্ষর *পূর্ব = dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, ফাঁকা);
    পূর্ব = _strdup((পূর্ব != ফাঁকা) ? পূর্ব : "char");
    dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, "utf-8");

    ফেরত পূর্ব;
}

//
// Restore previous codeset for gettext.
//
    স্থির শূন্য
restore_gettext_codeset(অক্ষর *পূর্ব)
{
    dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, পূর্ব);
    মুক্ত(পূর্ব);
}
#endif // FEAT_GETTEXT

//
// Global variables
//
UINT      g_cRefThisDll = 0;    // Reference count of this DLL.
HINSTANCE g_hmodThisDll = ফাঁকা;	// Handle to this DLL itself.


//---------------------------------------------------------------------------
// DllMain
//---------------------------------------------------------------------------
বাহ্যিক "C" পূর্ণসংখ্যা APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID  /* lpReserved */)
{
    নির্বাচন (dwReason)
    {
    ক্ষেত্র DLL_PROCESS_ATTACH:
	// Extension DLL one-time initialization
	g_hmodThisDll = hInstance;
	বিরতি;

    ক্ষেত্র DLL_PROCESS_DETACH:
	বিরতি;
    }

    ফেরত 1;   // ok
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
    ফেরত (g_cRefThisDll == 0 ? S_OK : S_FALSE);
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppvOut)
{
    *ppvOut = ফাঁকা;

    যদি (IsEqualIID(rclsid, CLSID_ShellExtension))
    {
	CShellExtClassFactory *pcf = নতুন CShellExtClassFactory;

	ফেরত pcf->QueryInterface(riid, ppvOut);
    }

    ফেরত CLASS_E_CLASSNOTAVAILABLE;
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
    *ppv = ফাঁকা;

    // any interface on this object is the object pointer

    যদি (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	*ppv = (LPCLASSFACTORY)এই;

	AddRef();

	ফেরত NOERROR;
    }

    ফেরত E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::AddRef()
{
    ফেরত InterlockedIncrement((LPLONG)&m_cRef);
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::Release()
{
    যদি (InterlockedDecrement((LPLONG)&m_cRef))
	ফেরত m_cRef;

    delete এই;

    ফেরত 0L;
}

STDMETHODIMP CShellExtClassFactory::CreateInstance(LPUNKNOWN pUnkOuter,
						      REFIID riid,
						      LPVOID *ppvObj)
{
    *ppvObj = ফাঁকা;

    // Shell extensions typically don't support aggregation (inheritance)

    যদি (pUnkOuter)
	ফেরত CLASS_E_NOAGGREGATION;

    // Create the main shell extension object.  The shell will then call
    // QueryInterface with IID_IShellExtInit--this is how shell extensions are
    // initialized.

    LPCSHELLEXT pShellExt = নতুন CShellExt();  // create the CShellExt object

    যদি (ফাঁকা == pShellExt)
	ফেরত E_OUTOFMEMORY;

    ফেরত pShellExt->QueryInterface(riid, ppvObj);
}


STDMETHODIMP CShellExtClassFactory::LockServer(BOOL  /* fLock */)
{
    ফেরত NOERROR;
}

// *********************** CShellExt *************************
CShellExt::CShellExt()
{
    m_cRef = 0L;
    m_p_তথ্য_Obj = ফাঁকা;

    inc_cRefThisDLL();

    LoadMenuIcon();
}

CShellExt::~CShellExt()
{
    যদি (m_p_তথ্য_Obj)
	m_p_তথ্য_Obj->Release();

    dec_cRefThisDLL();

    যদি (m_hVimIconBitmap)
	DeleteObject(m_hVimIconBitmap);
}

STDMETHODIMP CShellExt::QueryInterface(REFIID riid, LPVOID FAR *ppv)
{
    *ppv = ফাঁকা;

    যদি (IsEqualIID(riid, IID_IShellExtInit) || IsEqualIID(riid, IID_IUnknown))
    {
	*ppv = (LPSHELLEXTINIT)এই;
    }
    অন্যথা যদি (IsEqualIID(riid, IID_IContextMenu))
    {
	*ppv = (LPCONTEXTMENU)এই;
    }

    যদি (*ppv)
    {
	AddRef();

	ফেরত NOERROR;
    }

    ফেরত E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExt::AddRef()
{
    ফেরত InterlockedIncrement((LPLONG)&m_cRef);
}

STDMETHODIMP_(ULONG) CShellExt::Release()
{

    যদি (InterlockedDecrement((LPLONG)&m_cRef))
	ফেরত m_cRef;

    delete এই;

    ফেরত 0L;
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
				   LPDATAOBJECT p_তথ্য_Obj,
				   HKEY  /* hRegKey */)
{
    // Initialize can be called more than once
    যদি (m_p_তথ্য_Obj)
	m_p_তথ্য_Obj->Release();

    // duplicate the object pointer and registry handle

    যদি (p_তথ্য_Obj)
    {
	m_p_তথ্য_Obj = p_তথ্য_Obj;
	p_তথ্য_Obj->AddRef();
    }

    ফেরত NOERROR;
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
					 UINT সূচক_Menu,
					 UINT idCmdFirst,
					 UINT  /* idCmdLast */,
					 UINT  /* uFlags */)
{
    UINT idCmd = idCmdFirst;

    hres = m_p_তথ্য_Obj->Get_তথ্য(&fmte, &medium);
    যদি (medium.hGlobal)
	cbFiles = DragQueryFileW((HDROP)medium.hGlobal, (UINT)-1, 0, 0);

    // InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

    // Initialize m_cntOfHWnd to 0
    m_cntOfHWnd = 0;

    HKEY keyhandle;
    যৌক্তিক showExisting = সত্য;
    যৌক্তিক showIcons = সত্য;

    // Check whether "Edit with existing Vim" entries are disabled.
    যদি (Reg_Open_চাবি_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
				       চাবি_READ, &keyhandle) == ERROR_SUCCESS)
    {
	যদি (Reg_Query_মান_Ex(keyhandle, "DisableEditWithExisting", 0, ফাঁকা,
						 ফাঁকা, ফাঁকা) == ERROR_SUCCESS)
	    showExisting = মিথ্যা;
	যদি (Reg_Query_মান_Ex(keyhandle, "DisableContextMenuIcons", 0, ফাঁকা,
						 ফাঁকা, ফাঁকা) == ERROR_SUCCESS)
	    showIcons = মিথ্যা;
	Reg_Close_চাবি(keyhandle);
    }

    // Use UTF-8 for gettext.
    অক্ষর *পূর্ব = set_gettext_codeset();

    // Retrieve all the vim instances, unless disabled.
    যদি (showExisting)
	EnumWindows(EnumWindowsProc, (LPARAM)এই);

    MENUITEMINFOW mii = { আকার(MENUITEMINFOW) };
    mii.fMask = MIIM_STRING | MIIM_ID;
    যদি (showIcons)
    {
	mii.fMask |= MIIM_BITMAP;
	mii.hbmpItem = m_hVimIconBitmap;
    }

    যদি (cbFiles > 1)
    {
	mii.wID = idCmd++;
	mii.dw_Type_তথ্য = W(_("Edit with Vim using &tabpages"));
	mii.cch = wcslen(mii.dw_Type_তথ্য);
	ঢোকাও_Menu_Item_W(hMenu, সূচক_Menu++, TRUE, &mii);
	মুক্ত(mii.dw_Type_তথ্য);

	mii.wID = idCmd++;
	mii.dw_Type_তথ্য = W(_("Edit with single &Vim"));
	mii.cch = wcslen(mii.dw_Type_তথ্য);
	ঢোকাও_Menu_Item_W(hMenu, সূচক_Menu++, TRUE, &mii);
	মুক্ত(mii.dw_Type_তথ্য);

	যদি (cbFiles <= 4)
	{
	    // Can edit up to 4 files in diff mode
	    mii.wID = idCmd++;
	    mii.dw_Type_তথ্য = W(_("Diff with Vim"));
	    mii.cch = wcslen(mii.dw_Type_তথ্য);
	    ঢোকাও_Menu_Item_W(hMenu, সূচক_Menu++, TRUE, &mii);
	    মুক্ত(mii.dw_Type_তথ্য);
	    m_edit_existing_off = 3;
	}
	অন্যথা
	    m_edit_existing_off = 2;

    }
    অন্যথা
    {
	mii.wID = idCmd++;
	mii.dw_Type_তথ্য = W(_("Edit with &Vim"));
	mii.cch = wcslen(mii.dw_Type_তথ্য);
	ঢোকাও_Menu_Item_W(hMenu, সূচক_Menu++, TRUE, &mii);
	মুক্ত(mii.dw_Type_তথ্য);
	m_edit_existing_off = 1;
    }

    HMENU hSubMenu = ফাঁকা;
    যদি (m_cntOfHWnd > 1)
    {
	hSubMenu = CreatePopupMenu();
	mii.fMask |= MIIM_SUBMENU;
	mii.wID = idCmd;
	mii.dw_Type_তথ্য = W(_("Edit with existing Vim"));
	mii.cch = wcslen(mii.dw_Type_তথ্য);
	mii.hSubMenu = hSubMenu;
	ঢোকাও_Menu_Item_W(hMenu, সূচক_Menu++, TRUE, &mii);
	মুক্ত(mii.dw_Type_তথ্য);
	mii.fMask = mii.fMask & ~MIIM_SUBMENU;
	mii.hSubMenu = ফাঁকা;
    }
    // Now display all the vim instances
    জন্য (পূর্ণসংখ্যা i = 0; i < m_cntOfHWnd; i++)
    {
	WCHAR title[BUFSIZE];
	WCHAR অস্থায়ী[BUFSIZE];
	পূর্ণসংখ্যা সূচক;
	HMENU hmenu;

	// Obtain window title, continue if can not
	যদি (GetWindowTextW(m_hWnd[i], title, BUFSIZE - 1) == 0)
	    চালু;
	// Truncate the title before the path, keep the file name
	WCHAR *pos = wcschr(title, L'(');
	যদি (pos != ফাঁকা)
	{
	    যদি (pos > title && pos[-1] == L' ')
		--pos;
	    *pos = 0;
	}
	// Now concatenate
	যদি (m_cntOfHWnd > 1)
	    অস্থায়ী[0] = L'\0';
	অন্যথা
	{
	    WCHAR *s = W(_("Edit with existing Vim - "));
	    wcsncpy(অস্থায়ী, s, BUFSIZE - 1);
	    অস্থায়ী[BUFSIZE - 1] = L'\0';
	    মুক্ত(s);
	}
	wcsncat(অস্থায়ী, title, BUFSIZE - 1 - wcslen(অস্থায়ী));
	অস্থায়ী[BUFSIZE - 1] = L'\0';

	mii.wID = idCmd++;
	mii.dw_Type_তথ্য = অস্থায়ী;
	mii.cch = wcslen(mii.dw_Type_তথ্য);
	যদি (m_cntOfHWnd > 1)
	{
	    hmenu = hSubMenu;
	    সূচক = i;
	}
	অন্যথা
	{
	    hmenu = hMenu;
	    সূচক = সূচক_Menu++;
	}
	ঢোকাও_Menu_Item_W(hmenu, সূচক, TRUE, &mii);
    }
    // InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

    // Restore previous codeset.
    restore_gettext_codeset(পূর্ব);

    // Must return number of menu items we added.
    ফেরত ফলাফল_From_Short(idCmd-idCmdFirst);
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
    পূর্ণসংখ্যা gvimExtraOptions;

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
	    নির্বাচন (idCmd)
	    {
		ক্ষেত্র 0:
		    gvimExtraOptions = EDIT_WITH_VIM_USE_TABPAGES;
		    বিরতি;
		ক্ষেত্র 1:
		    gvimExtraOptions = EDIT_WITH_VIM_NO_OPTIONS;
		    বিরতি;
		ক্ষেত্র 2:
		    gvimExtraOptions = EDIT_WITH_VIM_IN_DIFF_MODE;
		    বিরতি;
		পূর্বনির্ধারিত:
		    // If execution reaches this point we likely have an
		    // inconsistency between the code that setup the menus
		    // and this code that determines what the user
		    // selected.  This should be detected and fixed during
		    // development.
		    ফেরত E_FAIL;
	    }

            LPCMINVOKECOMMANDINFOEX lpcmiex = (LPCMINVOKECOMMANDINFOEX)lpcmi;
            LPCWSTR currentDirectory = lpcmi->cb_আকার == আকার(CMINVOKECOMMANDINFOEX) ? lpcmiex->lpDirectoryW : ফাঁকা;

	    hr = InvokeSingleGvim(lpcmi->hwnd,
		    currentDirectory,
		    lpcmi->lpVerb,
		    lpcmi->lpParameters,
		    lpcmi->nShow,
		    gvimExtraOptions);
	}
    }
    ফেরত hr;
}

STDMETHODIMP CShellExt::ঠেলো_To_Window(HWND  /* hParent */,
				   LPCSTR  /* pszWorkingDir */,
				   LPCSTR  /* pszCmd */,
				   LPCSTR  /* pszParam */,
				   পূর্ণসংখ্যা  /* iShowCmd */,
				   পূর্ণসংখ্যা idHWnd)
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

    ফেরত NOERROR;
}

STDMETHODIMP CShellExt::GetCommandString(UINT_PTR  /* idCmd */,
					 UINT uFlags,
					 UINT FAR * /* reserved */,
					 LPSTR pszName,
					 UINT cchMax)
{
    // Use UTF-8 for gettext.
    অক্ষর *পূর্ব = set_gettext_codeset();

    WCHAR *s = W(_("Edits the selected file(s) with Vim"));
    যদি (uFlags == GCS_HELPTEXTW && cchMax > wcslen(s))
	wcscpy((WCHAR *)pszName, s);
    মুক্ত(s);

    // Restore previous codeset.
    restore_gettext_codeset(পূর্ব);

    ফেরত NOERROR;
}

BOOL CALLBACK CShellExt::EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    অক্ষর অস্থায়ী[BUFSIZE];

    // First do a bunch of check
    // No invisible window
    যদি (!IsWindowVisible(hWnd))
	ফেরত TRUE;
    // No child window ???
    // if (GetParent(hWnd)) return TRUE;
    // Class name should be Vim, if failed to get class name, return
    যদি (GetClassName(hWnd, অস্থায়ী, ARRAY_দৈর্ঘ্য(অস্থায়ী)) == 0)
	ফেরত TRUE;
    // Compare class name to that of vim, if not, return
    যদি (_strnicmp(অস্থায়ী, "vim", ARRAY_দৈর্ঘ্য("vim")) != 0)
	ফেরত TRUE;
    // First check if the number of vim instance exceeds MAX_HWND
    CShellExt *cs = (CShellExt*) lParam;
    যদি (cs->m_cntOfHWnd >= MAX_HWND)
	ফেরত FALSE;	// stop enumeration
    // Now we get the vim window, put it into some kind of array
    cs->m_hWnd[cs->m_cntOfHWnd] = hWnd;
    cs->m_cntOfHWnd ++;

    ফেরত TRUE; // continue enumeration (otherwise this would be false)
}

BOOL CShellExt::LoadMenuIcon()
{
    অক্ষর vimExeFile[BUFSIZE];
    getGvimName(vimExeFile, TRUE);
    যদি (vimExeFile[0] == '\0')
	ফেরত FALSE;
    HICON hVimIcon;
    যদি (ExtractIconEx(vimExeFile, 0, ফাঁকা, &hVimIcon, 1) == 0)
	ফেরত FALSE;
    m_hVimIconBitmap = IconToBitmap(hVimIcon,
	    GetSysColorBrush(COLOR_MENU),
	    GetSystemMetrics(SM_CXSMICON),
	    GetSystemMetrics(SM_CYSMICON));
    ফেরত TRUE;
}

    স্থির অক্ষর *
searchpath(অক্ষর *name)
{
    স্থির সংঘ {
	অক্ষর	location[2 * BUFSIZE];
	WCHAR	wname[BUFSIZE];
    };
    WCHAR wlocation[BUFSIZE + 1];

    // There appears to be a bug in FindExecutableA() on Windows NT.
    // Use FindExecutableW() instead...
    MultiByteToWideChar(CP_ACP, 0, name, -1, wname, BUFSIZE);
    যদি (FindExecutableW(wname, L"", wlocation) > (HINSTANCE)32)
    {
	WideCharToMultiByte(CP_ACP, 0, wlocation, -1,
		location, আকার(location), ফাঁকা, ফাঁকা);
	ফেরত location;
    }
    ফেরত (অক্ষর *)"";
}


STDMETHODIMP CShellExt::InvokeSingleGvim(HWND hParent,
				   LPCWSTR  workingDir,
				   LPCSTR  /* pszCmd */,
				   LPCSTR  /* pszParam */,
				   পূর্ণসংখ্যা  /* iShowCmd */,
				   পূর্ণসংখ্যা gvimExtraOptions)
{
    wchar_t	m_szFileUserClickedOn[BUFSIZE];
    wchar_t	*cmdStrW;
    size_t	cmdlen;
    size_t	len;
    UINT i;

    cmdlen = BUFSIZE;
    cmdStrW  = (wchar_t *) malloc(cmdlen * আকার(wchar_t));
    যদি (cmdStrW == ফাঁকা)
	ফেরত E_FAIL;
    getGvimInvocationW(cmdStrW);

    যদি (gvimExtraOptions == EDIT_WITH_VIM_IN_DIFF_MODE)
	wcscat(cmdStrW, L" -d");
    অন্যথা যদি (gvimExtraOptions == EDIT_WITH_VIM_USE_TABPAGES)
	wcscat(cmdStrW, L" -p");
    জন্য (i = 0; i < cbFiles; i++)
    {
	DragQueryFileW((HDROP)medium.hGlobal,
		i,
		m_szFileUserClickedOn,
		ARRAY_দৈর্ঘ্য(m_szFileUserClickedOn));

	len = wcslen(cmdStrW) + wcslen(m_szFileUserClickedOn) + 4;
	যদি (len > cmdlen)
	{
	    cmdlen = len + BUFSIZE;
	    wchar_t *cmdStrW_new = (wchar_t *)realloc(cmdStrW, cmdlen * আকার(wchar_t));
	    যদি (cmdStrW_new == ফাঁকা)
	    {
		মুক্ত(cmdStrW);
		ফেরত E_FAIL;
	    }
	    cmdStrW = cmdStrW_new;
	}
	wcscat(cmdStrW, L" \"");
	wcscat(cmdStrW, m_szFileUserClickedOn);
	wcscat(cmdStrW, L"\"");
    }

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, আকার(si));
    si.cb = আকার(si);

    // Start the child process.
    যদি (!CreateProcessW(ফাঁকা,	// No module name (use command line).
		cmdStrW,	// Command line.
		ফাঁকা,		// Process handle not inheritable.
		ফাঁকা,		// Thread handle not inheritable.
		FALSE,		// Set handle inheritance to FALSE.
		0,		// No creation flags.
		ফাঁকা,		// Use parent's environment block.
		workingDir,	// Use parent's starting directory.
		&si,		// Pointer to STARTUPINFO structure.
		&pi)		// Pointer to PROCESS_INFORMATION structure.
       )
    {
	// Use UTF-8 for gettext.
	অক্ষর *পূর্ব = set_gettext_codeset();

	WCHAR *বার্তা = W(_("Error creating process: Check if gvim is in your path!"));
	WCHAR *title = W(_("gvimext.dll error"));

	MessageBoxW(hParent, বার্তা, title, MB_OK);

	মুক্ত(বার্তা);
	মুক্ত(title);

	// Restore previous codeset.
	restore_gettext_codeset(পূর্ব);
    }
    অন্যথা
    {
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
    }
    মুক্ত(cmdStrW);

    ফেরত NOERROR;
}
