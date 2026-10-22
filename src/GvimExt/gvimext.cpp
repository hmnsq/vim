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

#અન્તર્ભાવ "gvimext.h"
#અન્તર્ભાવ "../version.h"

#define ARRAY_LENGTH(a) (કદ(a) / કદ((a)[0]))

સ્થિર અક્ષર *searchpath(અક્ષર *name);

// Always get an error while putting the following stuff to the
// gvimext.h file as class protected variables, give up and
// declare them as global stuff
FORMATETC fmte = {CF_HDROP,
		  (DVTARGETDEVICE FAR *)ખાલી,
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
    સ્થિર શૂન્ય
getGvimName(અક્ષર *name, BOOL runtime)
{
    HKEY	keyhandle;
    DWORD	hlen;

    // Get the location of gvim from the registry.  Try
    // HKEY_CURRENT_USER first, then fall back to HKEY_LOCAL_MACHINE if
    // not found.
    name[0] = 0;
    જો (Reg_Open_ચાવી_Ex(HKEY_CURRENT_USER, "Software\\Vim\\Gvim", 0,
				       ચાવી_READ, &keyhandle) == ERROR_SUCCESS)
    {
	hlen = BUFSIZE;
	જો (Reg_Query_કિંમત_Ex(keyhandle, "path", 0, ખાલી, (BYTE *)name, &hlen)
							     != ERROR_SUCCESS)
	    name[0] = 0;
	નહીંતર
	    name[hlen] = 0;
	Reg_Close_ચાવી(keyhandle);
    }

    જો ((name[0] == 0) &&
	    (Reg_Open_ચાવી_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
				       ચાવી_READ, &keyhandle) == ERROR_SUCCESS))
    {
	hlen = BUFSIZE;
	જો (Reg_Query_કિંમત_Ex(keyhandle, "path", 0, ખાલી, (BYTE *)name, &hlen)
							     != ERROR_SUCCESS)
	    name[0] = 0;
	નહીંતર
	    name[hlen] = 0;
	Reg_Close_ચાવી(keyhandle);
    }

    // Registry didn't work, use the search path.
    જો (name[0] == 0)
	strcpy(name, searchpath((અક્ષર *)"gvim.exe"));

    જો (!runtime)
    {
	// Only when looking for the executable, not the runtime dir, we can
	// search for the batch file or a name without a path.
	જો (name[0] == 0)
	    strcpy(name, searchpath((અક્ષર *)"gvim.bat"));
	જો (name[0] == 0)
	    strcpy(name, "gvim");	// finds gvim.bat or gvim.exe
    }
}

    સ્થિર શૂન્ય
getGvimInvocation(અક્ષર *name)
{
    getGvimName(name, FALSE);
    // avoid that Vim tries to expand wildcards in the file names
    strcat(name, " --literal");
}

    સ્થિર શૂન્ય
getGvimInvocationW(wchar_t *nameW)
{
    અક્ષર *name;

    name = (અક્ષર *)malloc(BUFSIZE);
    getGvimInvocation(name);
    mbstowcs(nameW, name, BUFSIZE);
    મુક્ત(name);
}

    સ્થિર BOOL
FileExists(LPCTSTR szPath)
{
    DWORD dwAttrib = GetFileAttributes(szPath);

    પાછા (dwAttrib != INVALID_FILE_ATTRIBUTES
	    && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

//
// Get the Vim runtime directory into buf[BUFSIZE].
// The result is empty when it failed.
// When it works, the path ends in a slash or backslash.
//
    સ્થિર શૂન્ય
getRuntimeDir(અક્ષર *buf)
{
    પૂર્ણાંક		idx;

    getGvimName(buf, TRUE);
    જો (buf[0] == 0)
	પાછા;

    // When no path found, use the search path to expand it.
    જો (strchr(buf, '/') == ખાલી && strchr(buf, '\\') == ખાલી)
	strcpy(buf, searchpath(buf));

    // remove "gvim.exe" from the end
    માટે (idx = (પૂર્ણાંક)strlen(buf) - 1; idx >= 0; idx--)
	જો (buf[idx] == '\\' || buf[idx] == '/')
	{
	    buf[++idx] = 0;
	    તોડો;
	}

    // Check the existence of defaults.vim
    અક્ષર *p = buf + idx;
    strcpy(p, "defaults.vim");
    જો (FileExists(buf))
    {
	*p = 0;		// Cut "defaults.vim"
	પાછા;
    }
    // Check the existence of vimXX\defaults.vim
    strcpy(p, VIM_VERSION_NODOT "\\" "defaults.vim");
    જો (FileExists(buf))
    {
	p[ARRAY_LENGTH(VIM_VERSION_NODOT)] = 0;	// Cut "defaults.vim"
	પાછા;
    }
    // Check the existence of runtime\defaults.vim
    strcpy(p, "runtime" "\\" "defaults.vim");
    જો (FileExists(buf))
    {
	p[ARRAY_LENGTH("runtime")] = 0;		// Cut "defaults.vim"
	પાછા;
    }
    // Not found
    buf[0] = 0;
    પાછા;
}

    WCHAR *
utf8_to_utf16(અચલ અક્ષર *s)
{
    પૂર્ણાંક size = MultiByteToWideChar(CP_UTF8, 0, s, -1, ખાલી, 0);
    WCHAR *buf = (WCHAR *)malloc(size * કદ(WCHAR));
    MultiByteToWideChar(CP_UTF8, 0, s, -1, buf, size);
    પાછા buf;
}

    HBITMAP
IconToBitmap(HICON hIcon, HBRUSH hBackground, પૂર્ણાંક width, પૂર્ણાંક height)
{
    HDC hDC = GetDC(ખાલી);
    HDC hMemDC = CreateCompatibleDC(hDC);
    HBITMAP hMemBmp = CreateCompatibleBitmap(hDC, width, height);
    HBITMAP hResultBmp = ખાલી;
    HGDIOBJ hOrgBMP = SelectObject(hMemDC, hMemBmp);

    DrawIconEx(hMemDC, 0, 0, hIcon, width, height, 0, hBackground, DI_NORMAL);

    hResultBmp = hMemBmp;
    hMemBmp = ખાલી;

    SelectObject(hMemDC, hOrgBMP);
    DeleteDC(hMemDC);
    ReleaseDC(ખાલી, hDC);
    DestroyIcon(hIcon);
    પાછા hResultBmp;
}

//
// GETTEXT: translated messages and menu entries
//
#ifndef FEAT_GETTEXT
# define _(x)	    x
# define W_impl(x) _wcsdup(L##x)
# define W(x)	    W_impl(x)
# define set_gettext_codeset()	    ખાલી
# define restore_gettext_codeset(x)
#નહીંતર
# define _(x)	    (*dyn_libintl_gettext)(x)
# define W(x)	    utf8_to_utf16(x)
# define VIMPACKAGE "vim"
# ifndef GETTEXT_DLL
#  define GETTEXT_DLL "libintl.dll"
#  define GETTEXT_DLL_ALT "libintl-8.dll"
# endif

// Dummy functions
સ્થિર અક્ષર *null_libintl_gettext(અચલ અક્ષર *);
સ્થિર અક્ષર *null_libintl_textdomain(અચલ અક્ષર *);
સ્થિર અક્ષર *null_libintl_bindtextdomain(અચલ અક્ષર *, અચલ અક્ષર *);
સ્થિર અક્ષર *null_libintl_bind_textdomain_codeset(અચલ અક્ષર *, અચલ અક્ષર *);
સ્થિર પૂર્ણાંક dyn_libintl_init(અક્ષર *dir);
સ્થિર શૂન્ય dyn_libintl_end(શૂન્ય);

સ્થિર HINSTANCE hLibintlDLL = 0;
સ્થિર અક્ષર *(*dyn_libintl_gettext)(અચલ અક્ષર *) = null_libintl_gettext;
સ્થિર અક્ષર *(*dyn_libintl_textdomain)(અચલ અક્ષર *) = null_libintl_textdomain;
સ્થિર અક્ષર *(*dyn_libintl_bindtextdomain)(અચલ અક્ષર *, અચલ અક્ષર *)
						= null_libintl_bindtextdomain;
સ્થિર અક્ષર *(*dyn_libintl_bind_textdomain_codeset)(અચલ અક્ષર *, અચલ અક્ષર *)
				       = null_libintl_bind_textdomain_codeset;

//
// Attempt to load libintl.dll.  If it doesn't work, use dummy functions.
// "dir" is the directory where the libintl.dll might be.
// Return 1 for success, 0 for failure.
//
    સ્થિર પૂર્ણાંક
dyn_libintl_init(અક્ષર *dir)
{
    પૂર્ણાંક		i;
    સ્થિર સંરચના
    {
	અક્ષર	    *name;
	FARPROC	    *ptr;
    } libintl_entry[] =
    {
	{(અક્ષર *)"gettext",		(FARPROC*)&dyn_libintl_gettext},
	{(અક્ષર *)"textdomain",		(FARPROC*)&dyn_libintl_textdomain},
	{(અક્ષર *)"bindtextdomain",	(FARPROC*)&dyn_libintl_bindtextdomain},
	{(અક્ષર *)"bind_textdomain_codeset", (FARPROC*)&dyn_libintl_bind_textdomain_codeset},
	{ખાલી, ખાલી}
    };
    DWORD	len, len2;
    LPWSTR	buf = ખાલી;
    LPWSTR	buf2 = ખાલી;

    // No need to initialize twice.
    જો (hLibintlDLL)
	પાછા 1;

    // Load gettext library from $VIMRUNTIME\GvimExt{64,32} directory.
    // Add the directory to $PATH temporarily.
    len = GetEnvironmentVariableW(L"PATH", ખાલી, 0);
    len2 = MAX_PATH + 1 + len;
    buf = (LPWSTR)malloc(len * કદ(WCHAR));
    buf2 = (LPWSTR)malloc(len2 * કદ(WCHAR));
    જો (buf != ખાલી && buf2 != ખાલી)
    {
	GetEnvironmentVariableW(L"PATH", buf, len);
# ifdef _WIN64
	_snwprintf(buf2, len2, L"%S\\GvimExt64;%s", dir, buf);
# નહીંતર
	_snwprintf(buf2, len2, L"%S\\GvimExt32;%s", dir, buf);
# endif
	SetEnvironmentVariableW(L"PATH", buf2);
	hLibintlDLL = LoadLibrary(GETTEXT_DLL);
# ifdef GETTEXT_DLL_ALT
	જો (!hLibintlDLL)
	    hLibintlDLL = LoadLibrary(GETTEXT_DLL_ALT);
# endif
	SetEnvironmentVariableW(L"PATH", buf);
    }
    મુક્ત(buf);
    મુક્ત(buf2);
    જો (!hLibintlDLL)
	પાછા 0;

    // Get the addresses of the functions we need.
    માટે (i = 0; libintl_entry[i].name != ખાલી
					 && libintl_entry[i].ptr != ખાલી; ++i)
    {
	જો ((*libintl_entry[i].ptr = GetProcAddress(hLibintlDLL,
					      libintl_entry[i].name)) == ખાલી)
	{
	    dyn_libintl_end();
	    પાછા 0;
	}
    }
    પાછા 1;
}

    સ્થિર શૂન્ય
dyn_libintl_end(શૂન્ય)
{
    જો (hLibintlDLL)
	FreeLibrary(hLibintlDLL);
    hLibintlDLL			= ખાલી;
    dyn_libintl_gettext		= null_libintl_gettext;
    dyn_libintl_textdomain	= null_libintl_textdomain;
    dyn_libintl_bindtextdomain	= null_libintl_bindtextdomain;
    dyn_libintl_bind_textdomain_codeset	= null_libintl_bind_textdomain_codeset;
}

    સ્થિર અક્ષર *
null_libintl_gettext(અચલ અક્ષર *msgid)
{
    પાછા (અક્ષર *)msgid;
}

    સ્થિર અક્ષર *
null_libintl_textdomain(અચલ અક્ષર * /* domainname */)
{
    પાછા ખાલી;
}

    સ્થિર અક્ષર *
null_libintl_bindtextdomain(અચલ અક્ષર * /* domainname */, અચલ અક્ષર * /* dirname */)
{
    પાછા ખાલી;
}

    સ્થિર અક્ષર *
null_libintl_bind_textdomain_codeset(અચલ અક્ષર * /* domainname */, અચલ અક્ષર * /* codeset */)
{
    પાછા ખાલી;
}

//
// Setup for translating strings.
//
    સ્થિર શૂન્ય
dyn_gettext_load(શૂન્ય)
{
    અક્ષર    szBuff[BUFSIZE];
    DWORD   len;

    // Try to locate the runtime files.  The path is used to find libintl.dll
    // and the vim.mo files.
    getRuntimeDir(szBuff);
    જો (szBuff[0] != 0)
    {
	len = (DWORD)strlen(szBuff);
	જો (dyn_libintl_init(szBuff))
	{
	    strcpy(szBuff + len, "lang");

	    (*dyn_libintl_bindtextdomain)(VIMPACKAGE, szBuff);
	    (*dyn_libintl_textdomain)(VIMPACKAGE);
	}
    }
}

    સ્થિર શૂન્ય
dyn_gettext_free(શૂન્ય)
{
    dyn_libintl_end();
}

//
// Use UTF-8 for gettext. Returns previous codeset.
//
    સ્થિર અક્ષર *
set_gettext_codeset(શૂન્ય)
{
    અક્ષર *prev = dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, ખાલી);
    prev = _strdup((prev != ખાલી) ? prev : "char");
    dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, "utf-8");

    પાછા prev;
}

//
// Restore previous codeset for gettext.
//
    સ્થિર શૂન્ય
restore_gettext_codeset(અક્ષર *prev)
{
    dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, prev);
    મુક્ત(prev);
}
#endif // FEAT_GETTEXT

//
// Global variables
//
UINT      g_cRefThisDll = 0;    // Reference count of this DLL.
HINSTANCE g_hmodThisDll = ખાલી;	// Handle to this DLL itself.


//---------------------------------------------------------------------------
// DllMain
//---------------------------------------------------------------------------
બાહ્ય "C" પૂર્ણાંક APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID  /* lpReserved */)
{
    પસંદગી (dwReason)
    {
    સ્થિતિ DLL_PROCESS_ATTACH:
	// Extension DLL one-time initialization
	g_hmodThisDll = hInstance;
	તોડો;

    સ્થિતિ DLL_PROCESS_DETACH:
	તોડો;
    }

    પાછા 1;   // ok
}

    સ્થિર શૂન્ય
inc_cRefThisDLL()
{
#ifdef FEAT_GETTEXT
    જો (g_cRefThisDll == 0)
	dyn_gettext_load();
#endif
    InterlockedIncrement((LPLONG)&g_cRefThisDll);
}

    સ્થિર શૂન્ય
dec_cRefThisDLL()
{
#ifdef FEAT_GETTEXT
    જો (InterlockedDecrement((LPLONG)&g_cRefThisDll) == 0)
	dyn_gettext_free();
#નહીંતર
    InterlockedDecrement((LPLONG)&g_cRefThisDll);
#endif
}

//---------------------------------------------------------------------------
// DllCanUnloadNow
//---------------------------------------------------------------------------

STDAPI DllCanUnloadNow(શૂન્ય)
{
    પાછા (g_cRefThisDll == 0 ? S_OK : S_FALSE);
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppvOut)
{
    *ppvOut = ખાલી;

    જો (IsEqualIID(rclsid, CLSID_ShellExtension))
    {
	CShellExtClassFactory *pcf = નવું CShellExtClassFactory;

	પાછા pcf->QueryInterface(riid, ppvOut);
    }

    પાછા CLASS_E_CLASSNOTAVAILABLE;
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
    *ppv = ખાલી;

    // any interface on this object is the object pointer

    જો (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	*ppv = (LPCLASSFACTORY)આ;

	AddRef();

	પાછા NOERROR;
    }

    પાછા E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::AddRef()
{
    પાછા InterlockedIncrement((LPLONG)&m_cRef);
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::Release()
{
    જો (InterlockedDecrement((LPLONG)&m_cRef))
	પાછા m_cRef;

    delete આ;

    પાછા 0L;
}

STDMETHODIMP CShellExtClassFactory::CreateInstance(LPUNKNOWN pUnkOuter,
						      REFIID riid,
						      LPVOID *ppvObj)
{
    *ppvObj = ખાલી;

    // Shell extensions typically don't support aggregation (inheritance)

    જો (pUnkOuter)
	પાછા CLASS_E_NOAGGREGATION;

    // Create the main shell extension object.  The shell will then call
    // QueryInterface with IID_IShellExtInit--this is how shell extensions are
    // initialized.

    LPCSHELLEXT pShellExt = નવું CShellExt();  // create the CShellExt object

    જો (ખાલી == pShellExt)
	પાછા E_OUTOFMEMORY;

    પાછા pShellExt->QueryInterface(riid, ppvObj);
}


STDMETHODIMP CShellExtClassFactory::LockServer(BOOL  /* fLock */)
{
    પાછા NOERROR;
}

// *********************** CShellExt *************************
CShellExt::CShellExt()
{
    m_cRef = 0L;
    m_pDataObj = ખાલી;

    inc_cRefThisDLL();

    LoadMenuIcon();
}

CShellExt::~CShellExt()
{
    જો (m_pDataObj)
	m_pDataObj->Release();

    dec_cRefThisDLL();

    જો (m_hVimIconBitmap)
	DeleteObject(m_hVimIconBitmap);
}

STDMETHODIMP CShellExt::QueryInterface(REFIID riid, LPVOID FAR *ppv)
{
    *ppv = ખાલી;

    જો (IsEqualIID(riid, IID_IShellExtInit) || IsEqualIID(riid, IID_IUnknown))
    {
	*ppv = (LPSHELLEXTINIT)આ;
    }
    નહીંતર જો (IsEqualIID(riid, IID_IContextMenu))
    {
	*ppv = (LPCONTEXTMENU)આ;
    }

    જો (*ppv)
    {
	AddRef();

	પાછા NOERROR;
    }

    પાછા E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExt::AddRef()
{
    પાછા InterlockedIncrement((LPLONG)&m_cRef);
}

STDMETHODIMP_(ULONG) CShellExt::Release()
{

    જો (InterlockedDecrement((LPLONG)&m_cRef))
	પાછા m_cRef;

    delete આ;

    પાછા 0L;
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
    જો (m_pDataObj)
	m_pDataObj->Release();

    // duplicate the object pointer and registry handle

    જો (pDataObj)
    {
	m_pDataObj = pDataObj;
	pDataObj->AddRef();
    }

    પાછા NOERROR;
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
    જો (medium.hGlobal)
	cbFiles = DragQueryFileW((HDROP)medium.hGlobal, (UINT)-1, 0, 0);

    // InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

    // Initialize m_cntOfHWnd to 0
    m_cntOfHWnd = 0;

    HKEY keyhandle;
    તાર્કિક showExisting = સાચું;
    તાર્કિક showIcons = સાચું;

    // Check whether "Edit with existing Vim" entries are disabled.
    જો (Reg_Open_ચાવી_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
				       ચાવી_READ, &keyhandle) == ERROR_SUCCESS)
    {
	જો (Reg_Query_કિંમત_Ex(keyhandle, "DisableEditWithExisting", 0, ખાલી,
						 ખાલી, ખાલી) == ERROR_SUCCESS)
	    showExisting = ખોટું;
	જો (Reg_Query_કિંમત_Ex(keyhandle, "DisableContextMenuIcons", 0, ખાલી,
						 ખાલી, ખાલી) == ERROR_SUCCESS)
	    showIcons = ખોટું;
	Reg_Close_ચાવી(keyhandle);
    }

    // Use UTF-8 for gettext.
    અક્ષર *prev = set_gettext_codeset();

    // Retrieve all the vim instances, unless disabled.
    જો (showExisting)
	EnumWindows(EnumWindowsProc, (LPARAM)આ);

    MENUITEMINFOW mii = { કદ(MENUITEMINFOW) };
    mii.fMask = MIIM_STRING | MIIM_ID;
    જો (showIcons)
    {
	mii.fMask |= MIIM_BITMAP;
	mii.hbmpItem = m_hVimIconBitmap;
    }

    જો (cbFiles > 1)
    {
	mii.wID = idCmd++;
	mii.dwTypeData = W(_("Edit with Vim using &tabpages"));
	mii.cch = wcslen(mii.dwTypeData);
	ઉમેરો_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	મુક્ત(mii.dwTypeData);

	mii.wID = idCmd++;
	mii.dwTypeData = W(_("Edit with single &Vim"));
	mii.cch = wcslen(mii.dwTypeData);
	ઉમેરો_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	મુક્ત(mii.dwTypeData);

	જો (cbFiles <= 4)
	{
	    // Can edit up to 4 files in diff mode
	    mii.wID = idCmd++;
	    mii.dwTypeData = W(_("Diff with Vim"));
	    mii.cch = wcslen(mii.dwTypeData);
	    ઉમેરો_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	    મુક્ત(mii.dwTypeData);
	    m_edit_existing_off = 3;
	}
	નહીંતર
	    m_edit_existing_off = 2;

    }
    નહીંતર
    {
	mii.wID = idCmd++;
	mii.dwTypeData = W(_("Edit with &Vim"));
	mii.cch = wcslen(mii.dwTypeData);
	ઉમેરો_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	મુક્ત(mii.dwTypeData);
	m_edit_existing_off = 1;
    }

    HMENU hSubMenu = ખાલી;
    જો (m_cntOfHWnd > 1)
    {
	hSubMenu = CreatePopupMenu();
	mii.fMask |= MIIM_SUBMENU;
	mii.wID = idCmd;
	mii.dwTypeData = W(_("Edit with existing Vim"));
	mii.cch = wcslen(mii.dwTypeData);
	mii.hSubMenu = hSubMenu;
	ઉમેરો_Menu_Item_W(hMenu, indexMenu++, TRUE, &mii);
	મુક્ત(mii.dwTypeData);
	mii.fMask = mii.fMask & ~MIIM_SUBMENU;
	mii.hSubMenu = ખાલી;
    }
    // Now display all the vim instances
    માટે (પૂર્ણાંક i = 0; i < m_cntOfHWnd; i++)
    {
	WCHAR title[BUFSIZE];
	WCHAR temp[BUFSIZE];
	પૂર્ણાંક index;
	HMENU hmenu;

	// Obtain window title, continue if can not
	જો (GetWindowTextW(m_hWnd[i], title, BUFSIZE - 1) == 0)
	    ચાલુ;
	// Truncate the title before the path, keep the file name
	WCHAR *pos = wcschr(title, L'(');
	જો (pos != ખાલી)
	{
	    જો (pos > title && pos[-1] == L' ')
		--pos;
	    *pos = 0;
	}
	// Now concatenate
	જો (m_cntOfHWnd > 1)
	    temp[0] = L'\0';
	નહીંતર
	{
	    WCHAR *s = W(_("Edit with existing Vim - "));
	    wcsncpy(temp, s, BUFSIZE - 1);
	    temp[BUFSIZE - 1] = L'\0';
	    મુક્ત(s);
	}
	wcsncat(temp, title, BUFSIZE - 1 - wcslen(temp));
	temp[BUFSIZE - 1] = L'\0';

	mii.wID = idCmd++;
	mii.dwTypeData = temp;
	mii.cch = wcslen(mii.dwTypeData);
	જો (m_cntOfHWnd > 1)
	{
	    hmenu = hSubMenu;
	    index = i;
	}
	નહીંતર
	{
	    hmenu = hMenu;
	    index = indexMenu++;
	}
	ઉમેરો_Menu_Item_W(hmenu, index, TRUE, &mii);
    }
    // InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

    // Restore previous codeset.
    restore_gettext_codeset(prev);

    // Must return number of menu items we added.
    પાછા ResultFromShort(idCmd-idCmdFirst);
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
    પૂર્ણાંક gvimExtraOptions;

    // If HIWORD(lpcmi->lpVerb) then we have been called programmatically
    // and lpVerb is a command that should be invoked.  Otherwise, the shell
    // has called us, and LOWORD(lpcmi->lpVerb) is the menu ID the user has
    // selected.  Actually, it's (menu ID - idCmdFirst) from QueryContextMenu().
    જો (!HIWORD(lpcmi->lpVerb))
    {
	UINT idCmd = LOWORD(lpcmi->lpVerb);

	જો (idCmd >= m_edit_existing_off)
	{
	    // Existing with vim instance
	    hr = ધકેલો_To_Window(lpcmi->hwnd,
		    lpcmi->lpDirectory,
		    lpcmi->lpVerb,
		    lpcmi->lpParameters,
		    lpcmi->nShow,
		    idCmd - m_edit_existing_off);
	}
	નહીંતર
	{
	    પસંદગી (idCmd)
	    {
		સ્થિતિ 0:
		    gvimExtraOptions = EDIT_WITH_VIM_USE_TABPAGES;
		    તોડો;
		સ્થિતિ 1:
		    gvimExtraOptions = EDIT_WITH_VIM_NO_OPTIONS;
		    તોડો;
		સ્થિતિ 2:
		    gvimExtraOptions = EDIT_WITH_VIM_IN_DIFF_MODE;
		    તોડો;
		મૂળભૂત:
		    // If execution reaches this point we likely have an
		    // inconsistency between the code that setup the menus
		    // and this code that determines what the user
		    // selected.  This should be detected and fixed during
		    // development.
		    પાછા E_FAIL;
	    }

            LPCMINVOKECOMMANDINFOEX lpcmiex = (LPCMINVOKECOMMANDINFOEX)lpcmi;
            LPCWSTR currentDirectory = lpcmi->cbSize == કદ(CMINVOKECOMMANDINFOEX) ? lpcmiex->lpDirectoryW : ખાલી;

	    hr = InvokeSingleGvim(lpcmi->hwnd,
		    currentDirectory,
		    lpcmi->lpVerb,
		    lpcmi->lpParameters,
		    lpcmi->nShow,
		    gvimExtraOptions);
	}
    }
    પાછા hr;
}

STDMETHODIMP CShellExt::ધકેલો_To_Window(HWND  /* hParent */,
				   LPCSTR  /* pszWorkingDir */,
				   LPCSTR  /* pszCmd */,
				   LPCSTR  /* pszParam */,
				   પૂર્ણાંક  /* iShowCmd */,
				   પૂર્ણાંક idHWnd)
{
    HWND hWnd = m_hWnd[idHWnd];

    // Show and bring vim instance to foreground
    જો (IsIconic(hWnd) != 0)
	ShowWindow(hWnd, SW_RESTORE);
    નહીંતર
	ShowWindow(hWnd, SW_SHOW);
    SetForegroundWindow(hWnd);

    // Post the selected files to the vim instance
    PostMessage(hWnd, WM_DROPFILES, (WPARAM)medium.hGlobal, 0);

    પાછા NOERROR;
}

STDMETHODIMP CShellExt::GetCommandString(UINT_PTR  /* idCmd */,
					 UINT uFlags,
					 UINT FAR * /* reserved */,
					 LPSTR pszName,
					 UINT cchMax)
{
    // Use UTF-8 for gettext.
    અક્ષર *prev = set_gettext_codeset();

    WCHAR *s = W(_("Edits the selected file(s) with Vim"));
    જો (uFlags == GCS_HELPTEXTW && cchMax > wcslen(s))
	wcscpy((WCHAR *)pszName, s);
    મુક્ત(s);

    // Restore previous codeset.
    restore_gettext_codeset(prev);

    પાછા NOERROR;
}

BOOL CALLBACK CShellExt::EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    અક્ષર temp[BUFSIZE];

    // First do a bunch of check
    // No invisible window
    જો (!IsWindowVisible(hWnd))
	પાછા TRUE;
    // No child window ???
    // if (GetParent(hWnd)) return TRUE;
    // Class name should be Vim, if failed to get class name, return
    જો (GetClassName(hWnd, temp, ARRAY_LENGTH(temp)) == 0)
	પાછા TRUE;
    // Compare class name to that of vim, if not, return
    જો (_strnicmp(temp, "vim", ARRAY_LENGTH("vim")) != 0)
	પાછા TRUE;
    // First check if the number of vim instance exceeds MAX_HWND
    CShellExt *cs = (CShellExt*) lParam;
    જો (cs->m_cntOfHWnd >= MAX_HWND)
	પાછા FALSE;	// stop enumeration
    // Now we get the vim window, put it into some kind of array
    cs->m_hWnd[cs->m_cntOfHWnd] = hWnd;
    cs->m_cntOfHWnd ++;

    પાછા TRUE; // continue enumeration (otherwise this would be false)
}

BOOL CShellExt::LoadMenuIcon()
{
    અક્ષર vimExeFile[BUFSIZE];
    getGvimName(vimExeFile, TRUE);
    જો (vimExeFile[0] == '\0')
	પાછા FALSE;
    HICON hVimIcon;
    જો (ExtractIconEx(vimExeFile, 0, ખાલી, &hVimIcon, 1) == 0)
	પાછા FALSE;
    m_hVimIconBitmap = IconToBitmap(hVimIcon,
	    GetSysColorBrush(COLOR_MENU),
	    GetSystemMetrics(SM_CXSMICON),
	    GetSystemMetrics(SM_CYSMICON));
    પાછા TRUE;
}

    સ્થિર અક્ષર *
searchpath(અક્ષર *name)
{
    સ્થિર સંઘ {
	અક્ષર	location[2 * BUFSIZE];
	WCHAR	wname[BUFSIZE];
    };
    WCHAR wlocation[BUFSIZE + 1];

    // There appears to be a bug in FindExecutableA() on Windows NT.
    // Use FindExecutableW() instead...
    MultiByteToWideChar(CP_ACP, 0, name, -1, wname, BUFSIZE);
    જો (FindExecutableW(wname, L"", wlocation) > (HINSTANCE)32)
    {
	WideCharToMultiByte(CP_ACP, 0, wlocation, -1,
		location, કદ(location), ખાલી, ખાલી);
	પાછા location;
    }
    પાછા (અક્ષર *)"";
}


STDMETHODIMP CShellExt::InvokeSingleGvim(HWND hParent,
				   LPCWSTR  workingDir,
				   LPCSTR  /* pszCmd */,
				   LPCSTR  /* pszParam */,
				   પૂર્ણાંક  /* iShowCmd */,
				   પૂર્ણાંક gvimExtraOptions)
{
    wchar_t	m_szFileUserClickedOn[BUFSIZE];
    wchar_t	*cmdStrW;
    size_t	cmdlen;
    size_t	len;
    UINT i;

    cmdlen = BUFSIZE;
    cmdStrW  = (wchar_t *) malloc(cmdlen * કદ(wchar_t));
    જો (cmdStrW == ખાલી)
	પાછા E_FAIL;
    getGvimInvocationW(cmdStrW);

    જો (gvimExtraOptions == EDIT_WITH_VIM_IN_DIFF_MODE)
	wcscat(cmdStrW, L" -d");
    નહીંતર જો (gvimExtraOptions == EDIT_WITH_VIM_USE_TABPAGES)
	wcscat(cmdStrW, L" -p");
    માટે (i = 0; i < cbFiles; i++)
    {
	DragQueryFileW((HDROP)medium.hGlobal,
		i,
		m_szFileUserClickedOn,
		ARRAY_LENGTH(m_szFileUserClickedOn));

	len = wcslen(cmdStrW) + wcslen(m_szFileUserClickedOn) + 4;
	જો (len > cmdlen)
	{
	    cmdlen = len + BUFSIZE;
	    wchar_t *cmdStrW_new = (wchar_t *)realloc(cmdStrW, cmdlen * કદ(wchar_t));
	    જો (cmdStrW_new == ખાલી)
	    {
		મુક્ત(cmdStrW);
		પાછા E_FAIL;
	    }
	    cmdStrW = cmdStrW_new;
	}
	wcscat(cmdStrW, L" \"");
	wcscat(cmdStrW, m_szFileUserClickedOn);
	wcscat(cmdStrW, L"\"");
    }

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, કદ(si));
    si.cb = કદ(si);

    // Start the child process.
    જો (!CreateProcessW(ખાલી,	// No module name (use command line).
		cmdStrW,	// Command line.
		ખાલી,		// Process handle not inheritable.
		ખાલી,		// Thread handle not inheritable.
		FALSE,		// Set handle inheritance to FALSE.
		0,		// No creation flags.
		ખાલી,		// Use parent's environment block.
		workingDir,	// Use parent's starting directory.
		&si,		// Pointer to STARTUPINFO structure.
		&pi)		// Pointer to PROCESS_INFORMATION structure.
       )
    {
	// Use UTF-8 for gettext.
	અક્ષર *prev = set_gettext_codeset();

	WCHAR *સંદેશ = W(_("Error creating process: Check if gvim is in your path!"));
	WCHAR *title = W(_("gvimext.dll error"));

	MessageBoxW(hParent, સંદેશ, title, MB_OK);

	મુક્ત(સંદેશ);
	મુક્ત(title);

	// Restore previous codeset.
	restore_gettext_codeset(prev);
    }
    નહીંતર
    {
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
    }
    મુક્ત(cmdStrW);

    પાછા NOERROR;
}
