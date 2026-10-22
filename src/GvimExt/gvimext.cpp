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

#ضمن "gvimext.h"
#ضمن "../version.h"

#وصف_ڪريو ARRAY_ڊيگهائي(a) (حجم(a) / حجم((a)[0]))

ساكن حرف *searchpath(حرف *name);

// Always get an error while putting the following stuff to the
// gvimext.h file as class protected variables, give up and
// declare them as global stuff
FORMATETC fmte = {CF_HDROP,
		  (DVTARGETDEVICE FAR *)عدم,
		  DVASPECT_CONTENT,
		  -1,
		  TYMED_HGLOBAL
		 };
STGMEDIUM medium;
HRESULT hres = 0;
UINT cbFiles = 0;

/* The buffers size used to be MAX_PATH (260 bytes), but that's not always
 * enough */
#وصف_ڪريو BUFSIZE 1100

// The "Edit with Vim" shell extension provides these choices when
// a new instance of Gvim is selected:
//   - use tabpages
//   - enable diff mode
//   - none of the above
#وصف_ڪريو EDIT_WITH_VIM_USE_TABPAGES (2)
#وصف_ڪريو EDIT_WITH_VIM_اندريون_DIFF_گهڻائي (1)
#وصف_ڪريو EDIT_WITH_VIM_NO_OPTIONS   (0)

//
// Get the name of the Gvim executable to use, with the path.
// When "runtime" is non-zero, we were called to find the runtime directory.
// Returns the path in name[BUFSIZE].  It's empty when it fails.
//
    ساكن فراغ
getGvimName(حرف *name, BOOL runtime)
{
    HKEY	keyhandle;
    DWORD	hlen;

    // Get the location of gvim from the registry.  Try
    // HKEY_CURRENT_USER first, then fall back to HKEY_LOCAL_MACHINE if
    // not found.
    name[0] = 0;
    إذا (Reg_Open_ڪنجي_Ex(HKEY_CURRENT_USER, "Software\\Vim\\Gvim", 0,
				       ڪنجي_READ, &keyhandle) == ERROR_SUCCESS)
    {
	hlen = BUFSIZE;
	إذا (Reg_Query_قدر_Ex(keyhandle, "path", 0, عدم, (BYTE *)name, &hlen)
							     != ERROR_SUCCESS)
	    name[0] = 0;
	وإلا
	    name[hlen] = 0;
	Reg_Close_ڪنجي(keyhandle);
    }

    إذا ((name[0] == 0) &&
	    (Reg_Open_ڪنجي_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
				       ڪنجي_READ, &keyhandle) == ERROR_SUCCESS))
    {
	hlen = BUFSIZE;
	إذا (Reg_Query_قدر_Ex(keyhandle, "path", 0, عدم, (BYTE *)name, &hlen)
							     != ERROR_SUCCESS)
	    name[0] = 0;
	وإلا
	    name[hlen] = 0;
	Reg_Close_ڪنجي(keyhandle);
    }

    // Registry didn't work, use the search path.
    إذا (name[0] == 0)
	strcpy(name, searchpath((حرف *)"gvim.exe"));

    إذا (!runtime)
    {
	// Only when looking for the executable, not the runtime dir, we can
	// search for the batch file or a name without a path.
	إذا (name[0] == 0)
	    strcpy(name, searchpath((حرف *)"gvim.bat"));
	إذا (name[0] == 0)
	    strcpy(name, "gvim");	// finds gvim.bat or gvim.exe
    }
}

    ساكن فراغ
getGvimInvocation(حرف *name)
{
    getGvimName(name, FALSE);
    // avoid that Vim tries to expand wildcards in the file names
    strcat(name, " --literal");
}

    ساكن فراغ
getGvimInvocationW(wchar_t *nameW)
{
    حرف *name;

    name = (حرف *)يادگيري_ڏيو(BUFSIZE);
    getGvimInvocation(name);
    mbstowcs(nameW, name, BUFSIZE);
    حرر(name);
}

    ساكن BOOL
FileExists(LPCTSTR sz_رستو)
{
    DWORD dwAttrib = Get_File_خاصيتون(sz_رستو);

    أرجع (dwAttrib != INVALID_FILE_خاصيتون
	    && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

//
// Get the Vim runtime directory into buf[BUFSIZE].
// The result is empty when it failed.
// When it works, the path ends in a slash or backslash.
//
    ساكن فراغ
getRuntimeDir(حرف *buf)
{
    صحيح		idx;

    getGvimName(buf, TRUE);
    إذا (buf[0] == 0)
	أرجع;

    // When no path found, use the search path to expand it.
    إذا (strchr(buf, '/') == عدم && strchr(buf, '\\') == عدم)
	strcpy(buf, searchpath(buf));

    // remove "gvim.exe" from the end
    لكل (idx = (صحيح)لڙي_ڊيگهه(buf) - 1; idx >= 0; idx--)
	إذا (buf[idx] == '\\' || buf[idx] == '/')
	{
	    buf[++idx] = 0;
	    اكسر;
	}

    // Check the existence of defaults.vim
    حرف *p = buf + idx;
    strcpy(p, "defaults.vim");
    إذا (FileExists(buf))
    {
	*p = 0;		// Cut "defaults.vim"
	أرجع;
    }
    // Check the existence of vimXX\defaults.vim
    strcpy(p, VIM_نسخو_NODOT "\\" "defaults.vim");
    إذا (FileExists(buf))
    {
	p[ARRAY_ڊيگهائي(VIM_نسخو_NODOT)] = 0;	// Cut "defaults.vim"
	أرجع;
    }
    // Check the existence of runtime\defaults.vim
    strcpy(p, "runtime" "\\" "defaults.vim");
    إذا (FileExists(buf))
    {
	p[ARRAY_ڊيگهائي("runtime")] = 0;		// Cut "defaults.vim"
	أرجع;
    }
    // Not found
    buf[0] = 0;
    أرجع;
}

    WCHAR *
utf8_منزل_utf16(ثابت حرف *s)
{
    صحيح سائيز = Multi_Byte_منزل_Wide_Char(CP_UTF8, 0, s, -1, عدم, 0);
    WCHAR *buf = (WCHAR *)يادگيري_ڏيو(سائيز * حجم(WCHAR));
    Multi_Byte_منزل_Wide_Char(CP_UTF8, 0, s, -1, buf, سائيز);
    أرجع buf;
}

    HBITMAP
Icon_منزل_Bitmap(HICON hIcon, HBRUSH hBackground, صحيح ويڪر, صحيح اوچائي)
{
    HDC hDC = GetDC(عدم);
    HDC hMemDC = CreateCompatibleDC(hDC);
    HBITMAP hMemBmp = CreateCompatibleBitmap(hDC, ويڪر, اوچائي);
    HBITMAP h_نتيجو_Bmp = عدم;
    HGDIOBJ hOrgBMP = SelectObject(hMemDC, hMemBmp);

    DrawIconEx(hMemDC, 0, 0, hIcon, ويڪر, اوچائي, 0, hBackground, DI_عام);

    h_نتيجو_Bmp = hMemBmp;
    hMemBmp = عدم;

    SelectObject(hMemDC, hOrgBMP);
    DeleteDC(hMemDC);
    ReleaseDC(عدم, hDC);
    DestroyIcon(hIcon);
    أرجع h_نتيجو_Bmp;
}

//
// GETTEXT: translated messages and menu entries
//
#جيڪڏهن_وصف_نه_ٿيل FEAT_GETTEXT
# define _(x)	    x
# define W_impl(x) _wcsdup(L##x)
# define W(x)	    W_impl(x)
# define سيٽ_gettext_codeset()	    عدم
# define restore_gettext_codeset(x)
#وإلا
# define _(x)	    (*dyn_libintl_gettext)(x)
# define W(x)	    utf8_منزل_utf16(x)
# define VIMPACKAGE "vim"
# ifndef GETTEXT_DLL
#  define GETTEXT_DLL "libintl.dll"
#  define GETTEXT_DLL_ALT "libintl-8.dll"
# endif

// Dummy functions
ساكن حرف *null_libintl_gettext(ثابت حرف *);
ساكن حرف *null_libintl_textdomain(ثابت حرف *);
ساكن حرف *null_libintl_bindtextdomain(ثابت حرف *, ثابت حرف *);
ساكن حرف *null_libintl_bind_textdomain_codeset(ثابت حرف *, ثابت حرف *);
ساكن صحيح dyn_libintl_init(حرف *dir);
ساكن فراغ dyn_libintl_پڇاڙي(فراغ);

ساكن HINSTANCE hLibintlDLL = 0;
ساكن حرف *(*dyn_libintl_gettext)(ثابت حرف *) = null_libintl_gettext;
ساكن حرف *(*dyn_libintl_textdomain)(ثابت حرف *) = null_libintl_textdomain;
ساكن حرف *(*dyn_libintl_bindtextdomain)(ثابت حرف *, ثابت حرف *)
						= null_libintl_bindtextdomain;
ساكن حرف *(*dyn_libintl_bind_textdomain_codeset)(ثابت حرف *, ثابت حرف *)
				       = null_libintl_bind_textdomain_codeset;

//
// Attempt to load libintl.dll.  If it doesn't work, use dummy functions.
// "dir" is the directory where the libintl.dll might be.
// Return 1 for success, 0 for failure.
//
    ساكن صحيح
dyn_libintl_init(حرف *dir)
{
    صحيح		i;
    ساكن بنية
    {
	حرف	    *name;
	FARPROC	    *ptr;
    } libintl_داخلا[] =
    {
	{(حرف *)"gettext",		(FARPROC*)&dyn_libintl_gettext},
	{(حرف *)"textdomain",		(FARPROC*)&dyn_libintl_textdomain},
	{(حرف *)"bindtextdomain",	(FARPROC*)&dyn_libintl_bindtextdomain},
	{(حرف *)"bind_textdomain_codeset", (FARPROC*)&dyn_libintl_bind_textdomain_codeset},
	{عدم, عدم}
    };
    DWORD	len, len2;
    LPWSTR	buf = عدم;
    LPWSTR	buf2 = عدم;

    // No need to initialize twice.
    إذا (hLibintlDLL)
	أرجع 1;

    // Load gettext library from $VIMRUNTIME\GvimExt{64,32} directory.
    // Add the directory to $PATH temporarily.
    len = GetEnvironmentVariableW(L"PATH", عدم, 0);
    len2 = MAX_رستو + 1 + len;
    buf = (LPWSTR)يادگيري_ڏيو(len * حجم(WCHAR));
    buf2 = (LPWSTR)يادگيري_ڏيو(len2 * حجم(WCHAR));
    إذا (buf != عدم && buf2 != عدم)
    {
	GetEnvironmentVariableW(L"PATH", buf, len);
# ifdef _WIN64
	_snwprintf(buf2, len2, L"%S\\GvimExt64;%s", dir, buf);
# وإلا
	_snwprintf(buf2, len2, L"%S\\GvimExt32;%s", dir, buf);
# endif
	سيٽ_Environment_Variable_W(L"PATH", buf2);
	hLibintlDLL = ڀريو_Library(GETTEXT_DLL);
# ifdef GETTEXT_DLL_ALT
	إذا (!hLibintlDLL)
	    hLibintlDLL = ڀريو_Library(GETTEXT_DLL_ALT);
# endif
	سيٽ_Environment_Variable_W(L"PATH", buf);
    }
    حرر(buf);
    حرر(buf2);
    إذا (!hLibintlDLL)
	أرجع 0;

    // Get the addresses of the functions we need.
    لكل (i = 0; libintl_داخلا[i].name != عدم
					 && libintl_داخلا[i].ptr != عدم; ++i)
    {
	إذا ((*libintl_داخلا[i].ptr = Get_Proc_پتو(hLibintlDLL,
					      libintl_داخلا[i].name)) == عدم)
	{
	    dyn_libintl_پڇاڙي();
	    أرجع 0;
	}
    }
    أرجع 1;
}

    ساكن فراغ
dyn_libintl_پڇاڙي(فراغ)
{
    إذا (hLibintlDLL)
	FreeLibrary(hLibintlDLL);
    hLibintlDLL			= عدم;
    dyn_libintl_gettext		= null_libintl_gettext;
    dyn_libintl_textdomain	= null_libintl_textdomain;
    dyn_libintl_bindtextdomain	= null_libintl_bindtextdomain;
    dyn_libintl_bind_textdomain_codeset	= null_libintl_bind_textdomain_codeset;
}

    ساكن حرف *
null_libintl_gettext(ثابت حرف *msgid)
{
    أرجع (حرف *)msgid;
}

    ساكن حرف *
null_libintl_textdomain(ثابت حرف * /* domainname */)
{
    أرجع عدم;
}

    ساكن حرف *
null_libintl_bindtextdomain(ثابت حرف * /* domainname */, ثابت حرف * /* dirname */)
{
    أرجع عدم;
}

    ساكن حرف *
null_libintl_bind_textdomain_codeset(ثابت حرف * /* domainname */, ثابت حرف * /* codeset */)
{
    أرجع عدم;
}

//
// Setup for translating strings.
//
    ساكن فراغ
dyn_gettext_ڀريو(فراغ)
{
    حرف    szBuff[BUFSIZE];
    DWORD   len;

    // Try to locate the runtime files.  The path is used to find libintl.dll
    // and the vim.mo files.
    getRuntimeDir(szBuff);
    إذا (szBuff[0] != 0)
    {
	len = (DWORD)لڙي_ڊيگهه(szBuff);
	إذا (dyn_libintl_init(szBuff))
	{
	    strcpy(szBuff + len, "lang");

	    (*dyn_libintl_bindtextdomain)(VIMPACKAGE, szBuff);
	    (*dyn_libintl_textdomain)(VIMPACKAGE);
	}
    }
}

    ساكن فراغ
dyn_gettext_free(فراغ)
{
    dyn_libintl_پڇاڙي();
}

//
// Use UTF-8 for gettext. Returns previous codeset.
//
    ساكن حرف *
سيٽ_gettext_codeset(فراغ)
{
    حرف *prev = dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, عدم);
    prev = _strdup((prev != عدم) ? prev : "char");
    dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, "utf-8");

    أرجع prev;
}

//
// Restore previous codeset for gettext.
//
    ساكن فراغ
restore_gettext_codeset(حرف *prev)
{
    dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, prev);
    حرر(prev);
}
#جيڪڏهن_پورو // FEAT_GETTEXT

//
// Global variables
//
UINT      g_cRefThisDll = 0;    // Reference count of this DLL.
HINSTANCE g_hmodThisDll = عدم;	// Handle to this DLL itself.


//---------------------------------------------------------------------------
// DllMain
//---------------------------------------------------------------------------
خارجي "C" صحيح APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID  /* lpReserved */)
{
    بدل (dwReason)
    {
    حالة DLL_PROCESS_ATTACH:
	// Extension DLL one-time initialization
	g_hmodThisDll = hInstance;
	اكسر;

    حالة DLL_PROCESS_DETACH:
	اكسر;
    }

    أرجع 1;   // ok
}

    ساكن فراغ
inc_cRefThisDLL()
{
#جيڪڏهن_وصف_ٿيل FEAT_GETTEXT
    إذا (g_cRefThisDll == 0)
	dyn_gettext_ڀريو();
#جيڪڏهن_پورو
    InterlockedIncrement((LPLONG)&g_cRefThisDll);
}

    ساكن فراغ
dec_cRefThisDLL()
{
#جيڪڏهن_وصف_ٿيل FEAT_GETTEXT
    إذا (InterlockedDecrement((LPLONG)&g_cRefThisDll) == 0)
	dyn_gettext_free();
#وإلا
    InterlockedDecrement((LPLONG)&g_cRefThisDll);
#جيڪڏهن_پورو
}

//---------------------------------------------------------------------------
// DllCanUnloadNow
//---------------------------------------------------------------------------

STDAPI DllCanUnloadNow(فراغ)
{
    أرجع (g_cRefThisDll == 0 ? S_OK : S_FALSE);
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppv_ٻاهريون)
{
    *ppv_ٻاهريون = عدم;

    إذا (IsEqualIID(rclsid, CLSID_ShellExtension))
    {
	CShellExtClassFactory *pcf = جديد CShellExtClassFactory;

	أرجع pcf->QueryInterface(riid, ppv_ٻاهريون);
    }

    أرجع CLASS_E_CLASSNOTAVAILABLE;
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
    *ppv = عدم;

    // any interface on this object is the object pointer

    إذا (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	*ppv = (LPCLASSFACTORY)هذا;

	جوڙيو_Ref();

	أرجع NOERROR;
    }

    أرجع E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::جوڙيو_Ref()
{
    أرجع InterlockedIncrement((LPLONG)&m_cRef);
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::Release()
{
    إذا (InterlockedDecrement((LPLONG)&m_cRef))
	أرجع m_cRef;

    ميٽيو هذا;

    أرجع 0L;
}

STDMETHODIMP CShellExtClassFactory::CreateInstance(LPUNKNOWN pUnkOuter,
						      REFIID riid,
						      LPVOID *ppvObj)
{
    *ppvObj = عدم;

    // Shell extensions typically don't support aggregation (inheritance)

    إذا (pUnkOuter)
	أرجع CLASS_E_NOAGGREGATION;

    // Create the main shell extension object.  The shell will then call
    // QueryInterface with IID_IShellExtInit--this is how shell extensions are
    // initialized.

    LPCSHELLEXT pShellExt = جديد CShellExt();  // create the CShellExt object

    إذا (عدم == pShellExt)
	أرجع E_OUTOFMEMORY;

    أرجع pShellExt->QueryInterface(riid, ppvObj);
}


STDMETHODIMP CShellExtClassFactory::تالو_Server(BOOL  /* fLock */)
{
    أرجع NOERROR;
}

// *********************** CShellExt *************************
CShellExt::CShellExt()
{
    m_cRef = 0L;
    m_p_معلومات_Obj = عدم;

    inc_cRefThisDLL();

    ڀريو_Menu_Icon();
}

CShellExt::~CShellExt()
{
    إذا (m_p_معلومات_Obj)
	m_p_معلومات_Obj->Release();

    dec_cRefThisDLL();

    إذا (m_hVimIconBitmap)
	DeleteObject(m_hVimIconBitmap);
}

STDMETHODIMP CShellExt::QueryInterface(REFIID riid, LPVOID FAR *ppv)
{
    *ppv = عدم;

    إذا (IsEqualIID(riid, IID_IShellExtInit) || IsEqualIID(riid, IID_IUnknown))
    {
	*ppv = (LPSHELLEXTINIT)هذا;
    }
    وإلا إذا (IsEqualIID(riid, IID_IContextMenu))
    {
	*ppv = (LPCONTEXTMENU)هذا;
    }

    إذا (*ppv)
    {
	جوڙيو_Ref();

	أرجع NOERROR;
    }

    أرجع E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExt::جوڙيو_Ref()
{
    أرجع InterlockedIncrement((LPLONG)&m_cRef);
}

STDMETHODIMP_(ULONG) CShellExt::Release()
{

    إذا (InterlockedDecrement((LPLONG)&m_cRef))
	أرجع m_cRef;

    ميٽيو هذا;

    أرجع 0L;
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
				   LPDATAOBJECT p_معلومات_Obj,
				   HKEY  /* hRegKey */)
{
    // Initialize can be called more than once
    إذا (m_p_معلومات_Obj)
	m_p_معلومات_Obj->Release();

    // duplicate the object pointer and registry handle

    إذا (p_معلومات_Obj)
    {
	m_p_معلومات_Obj = p_معلومات_Obj;
	p_معلومات_Obj->جوڙيو_Ref();
    }

    أرجع NOERROR;
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
					 UINT اشاريو_Menu,
					 UINT id_Cmd_پهريون,
					 UINT  /* idCmdLast */,
					 UINT  /* uFlags */)
{
    UINT idCmd = id_Cmd_پهريون;

    hres = m_p_معلومات_Obj->Get_معلومات(&fmte, &medium);
    إذا (medium.hGlobal)
	cbFiles = DragQueryFileW((HDROP)medium.hGlobal, (UINT)-1, 0, 0);

    // InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

    // Initialize m_cntOfHWnd to 0
    m_cnt_جو_HWnd = 0;

    HKEY keyhandle;
    منطقي showExisting = سچ;
    منطقي showIcons = سچ;

    // Check whether "Edit with existing Vim" entries are disabled.
    إذا (Reg_Open_ڪنجي_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
				       ڪنجي_READ, &keyhandle) == ERROR_SUCCESS)
    {
	إذا (Reg_Query_قدر_Ex(keyhandle, "DisableEditWithExisting", 0, عدم,
						 عدم, عدم) == ERROR_SUCCESS)
	    showExisting = خطأ;
	إذا (Reg_Query_قدر_Ex(keyhandle, "DisableContextMenuIcons", 0, عدم,
						 عدم, عدم) == ERROR_SUCCESS)
	    showIcons = خطأ;
	Reg_Close_ڪنجي(keyhandle);
    }

    // Use UTF-8 for gettext.
    حرف *prev = سيٽ_gettext_codeset();

    // Retrieve all the vim instances, unless disabled.
    إذا (showExisting)
	EnumWindows(EnumWindowsProc, (LPARAM)هذا);

    MENUITEMINFOW mii = { حجم(MENUITEMINFOW) };
    mii.f_نقاب = MIIM_STRING | MIIM_ID;
    إذا (showIcons)
    {
	mii.f_نقاب |= MIIM_BITMAP;
	mii.hbmp_شي = m_hVimIconBitmap;
    }

    إذا (cbFiles > 1)
    {
	mii.wID = idCmd++;
	mii.dw_Type_معلومات = W(_("Edit with Vim using &tabpages"));
	mii.cch = wcslen(mii.dw_Type_معلومات);
	وجهو_Menu_شي_W(hMenu, اشاريو_Menu++, TRUE, &mii);
	حرر(mii.dw_Type_معلومات);

	mii.wID = idCmd++;
	mii.dw_Type_معلومات = W(_("Edit with single &Vim"));
	mii.cch = wcslen(mii.dw_Type_معلومات);
	وجهو_Menu_شي_W(hMenu, اشاريو_Menu++, TRUE, &mii);
	حرر(mii.dw_Type_معلومات);

	إذا (cbFiles <= 4)
	{
	    // Can edit up to 4 files in diff mode
	    mii.wID = idCmd++;
	    mii.dw_Type_معلومات = W(_("Diff with Vim"));
	    mii.cch = wcslen(mii.dw_Type_معلومات);
	    وجهو_Menu_شي_W(hMenu, اشاريو_Menu++, TRUE, &mii);
	    حرر(mii.dw_Type_معلومات);
	    m_edit_existing_off = 3;
	}
	وإلا
	    m_edit_existing_off = 2;

    }
    وإلا
    {
	mii.wID = idCmd++;
	mii.dw_Type_معلومات = W(_("Edit with &Vim"));
	mii.cch = wcslen(mii.dw_Type_معلومات);
	وجهو_Menu_شي_W(hMenu, اشاريو_Menu++, TRUE, &mii);
	حرر(mii.dw_Type_معلومات);
	m_edit_existing_off = 1;
    }

    HMENU hSubMenu = عدم;
    إذا (m_cnt_جو_HWnd > 1)
    {
	hSubMenu = CreatePopupMenu();
	mii.f_نقاب |= MIIM_SUBMENU;
	mii.wID = idCmd;
	mii.dw_Type_معلومات = W(_("Edit with existing Vim"));
	mii.cch = wcslen(mii.dw_Type_معلومات);
	mii.hSubMenu = hSubMenu;
	وجهو_Menu_شي_W(hMenu, اشاريو_Menu++, TRUE, &mii);
	حرر(mii.dw_Type_معلومات);
	mii.f_نقاب = mii.f_نقاب & ~MIIM_SUBMENU;
	mii.hSubMenu = عدم;
    }
    // Now display all the vim instances
    لكل (صحيح i = 0; i < m_cnt_جو_HWnd; i++)
    {
	WCHAR title[BUFSIZE];
	WCHAR عارضي[BUFSIZE];
	صحيح اشاريو;
	HMENU hmenu;

	// Obtain window title, continue if can not
	إذا (Get_دري_متن_W(m_hWnd[i], title, BUFSIZE - 1) == 0)
	    استمر;
	// Truncate the title before the path, keep the file name
	WCHAR *pos = wcschr(title, L'(');
	إذا (pos != عدم)
	{
	    إذا (pos > title && pos[-1] == L' ')
		--pos;
	    *pos = 0;
	}
	// Now concatenate
	إذا (m_cnt_جو_HWnd > 1)
	    عارضي[0] = L'\0';
	وإلا
	{
	    WCHAR *s = W(_("Edit with existing Vim - "));
	    wcsncpy(عارضي, s, BUFSIZE - 1);
	    عارضي[BUFSIZE - 1] = L'\0';
	    حرر(s);
	}
	wcsncat(عارضي, title, BUFSIZE - 1 - wcslen(عارضي));
	عارضي[BUFSIZE - 1] = L'\0';

	mii.wID = idCmd++;
	mii.dw_Type_معلومات = عارضي;
	mii.cch = wcslen(mii.dw_Type_معلومات);
	إذا (m_cnt_جو_HWnd > 1)
	{
	    hmenu = hSubMenu;
	    اشاريو = i;
	}
	وإلا
	{
	    hmenu = hMenu;
	    اشاريو = اشاريو_Menu++;
	}
	وجهو_Menu_شي_W(hmenu, اشاريو, TRUE, &mii);
    }
    // InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

    // Restore previous codeset.
    restore_gettext_codeset(prev);

    // Must return number of menu items we added.
    أرجع نتيجو_From_Short(idCmd-id_Cmd_پهريون);
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
    صحيح gvimExtraOptions;

    // If HIWORD(lpcmi->lpVerb) then we have been called programmatically
    // and lpVerb is a command that should be invoked.  Otherwise, the shell
    // has called us, and LOWORD(lpcmi->lpVerb) is the menu ID the user has
    // selected.  Actually, it's (menu ID - idCmdFirst) from QueryContextMenu().
    إذا (!HIWORD(lpcmi->lpVerb))
    {
	UINT idCmd = LOWORD(lpcmi->lpVerb);

	إذا (idCmd >= m_edit_existing_off)
	{
	    // Existing with vim instance
	    hr = ڌڪو_منزل_دري(lpcmi->hwnd,
		    lpcmi->lpDirectory,
		    lpcmi->lpVerb,
		    lpcmi->lpParameters,
		    lpcmi->nShow,
		    idCmd - m_edit_existing_off);
	}
	وإلا
	{
	    بدل (idCmd)
	    {
		حالة 0:
		    gvimExtraOptions = EDIT_WITH_VIM_USE_TABPAGES;
		    اكسر;
		حالة 1:
		    gvimExtraOptions = EDIT_WITH_VIM_NO_OPTIONS;
		    اكسر;
		حالة 2:
		    gvimExtraOptions = EDIT_WITH_VIM_اندريون_DIFF_گهڻائي;
		    اكسر;
		افتراضي:
		    // If execution reaches this point we likely have an
		    // inconsistency between the code that setup the menus
		    // and this code that determines what the user
		    // selected.  This should be detected and fixed during
		    // development.
		    أرجع E_FAIL;
	    }

            LPCMINVOKECOMMANDINFOEX lpcmiex = (LPCMINVOKECOMMANDINFOEX)lpcmi;
            LPCWSTR currentDirectory = lpcmi->cb_سائيز == حجم(CMINVOKECOMMANDINFOEX) ? lpcmiex->lpDirectoryW : عدم;

	    hr = InvokeSingleGvim(lpcmi->hwnd,
		    currentDirectory,
		    lpcmi->lpVerb,
		    lpcmi->lpParameters,
		    lpcmi->nShow,
		    gvimExtraOptions);
	}
    }
    أرجع hr;
}

STDMETHODIMP CShellExt::ڌڪو_منزل_دري(HWND  /* hParent */,
				   LPCSTR  /* pszWorkingDir */,
				   LPCSTR  /* pszCmd */,
				   LPCSTR  /* pszParam */,
				   صحيح  /* iShowCmd */,
				   صحيح idHWnd)
{
    HWND hWnd = m_hWnd[idHWnd];

    // Show and bring vim instance to foreground
    إذا (IsIconic(hWnd) != 0)
	Show_دري(hWnd, SW_RESTORE);
    وإلا
	Show_دري(hWnd, SW_SHOW);
    سيٽ_Foreground_دري(hWnd);

    // Post the selected files to the vim instance
    Post_پيغام(hWnd, WM_DROPFILES, (WPARAM)medium.hGlobal, 0);

    أرجع NOERROR;
}

STDMETHODIMP CShellExt::GetCommandString(UINT_PTR  /* idCmd */,
					 UINT uFlags,
					 UINT FAR * /* reserved */,
					 LPSTR pszName,
					 UINT cchMax)
{
    // Use UTF-8 for gettext.
    حرف *prev = سيٽ_gettext_codeset();

    WCHAR *s = W(_("Edits the selected file(s) with Vim"));
    إذا (uFlags == GCS_HELPTEXTW && cchMax > wcslen(s))
	wcscpy((WCHAR *)pszName, s);
    حرر(s);

    // Restore previous codeset.
    restore_gettext_codeset(prev);

    أرجع NOERROR;
}

BOOL CALLBACK CShellExt::EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    حرف عارضي[BUFSIZE];

    // First do a bunch of check
    // No invisible window
    إذا (!Is_دري_Visible(hWnd))
	أرجع TRUE;
    // No child window ???
    // if (GetParent(hWnd)) return TRUE;
    // Class name should be Vim, if failed to get class name, return
    إذا (GetClassName(hWnd, عارضي, ARRAY_ڊيگهائي(عارضي)) == 0)
	أرجع TRUE;
    // Compare class name to that of vim, if not, return
    إذا (_strnicmp(عارضي, "vim", ARRAY_ڊيگهائي("vim")) != 0)
	أرجع TRUE;
    // First check if the number of vim instance exceeds MAX_HWND
    CShellExt *cs = (CShellExt*) lParam;
    إذا (cs->m_cnt_جو_HWnd >= MAX_HWND)
	أرجع FALSE;	// stop enumeration
    // Now we get the vim window, put it into some kind of array
    cs->m_hWnd[cs->m_cnt_جو_HWnd] = hWnd;
    cs->m_cnt_جو_HWnd ++;

    أرجع TRUE; // continue enumeration (otherwise this would be false)
}

BOOL CShellExt::ڀريو_Menu_Icon()
{
    حرف vimExeFile[BUFSIZE];
    getGvimName(vimExeFile, TRUE);
    إذا (vimExeFile[0] == '\0')
	أرجع FALSE;
    HICON hVimIcon;
    إذا (ڪڍو_Icon_Ex(vimExeFile, 0, عدم, &hVimIcon, 1) == 0)
	أرجع FALSE;
    m_hVimIconBitmap = Icon_منزل_Bitmap(hVimIcon,
	    GetSysColorBrush(COLOR_MENU),
	    GetSystemMetrics(SM_CXSMICON),
	    GetSystemMetrics(SM_CYSMICON));
    أرجع TRUE;
}

    ساكن حرف *
searchpath(حرف *name)
{
    ساكن اتحاد {
	حرف	location[2 * BUFSIZE];
	WCHAR	wname[BUFSIZE];
    };
    WCHAR wlocation[BUFSIZE + 1];

    // There appears to be a bug in FindExecutableA() on Windows NT.
    // Use FindExecutableW() instead...
    Multi_Byte_منزل_Wide_Char(CP_ACP, 0, name, -1, wname, BUFSIZE);
    إذا (ڳوليو_Executable_W(wname, L"", wlocation) > (HINSTANCE)32)
    {
	Wide_Char_منزل_Multi_Byte(CP_ACP, 0, wlocation, -1,
		location, حجم(location), عدم, عدم);
	أرجع location;
    }
    أرجع (حرف *)"";
}


STDMETHODIMP CShellExt::InvokeSingleGvim(HWND h_والد,
				   LPCWSTR  workingDir,
				   LPCSTR  /* pszCmd */,
				   LPCSTR  /* pszParam */,
				   صحيح  /* iShowCmd */,
				   صحيح gvimExtraOptions)
{
    wchar_t	m_szFileUserClickedOn[BUFSIZE];
    wchar_t	*cmdStrW;
    size_t	cmdlen;
    size_t	len;
    UINT i;

    cmdlen = BUFSIZE;
    cmdStrW  = (wchar_t *) يادگيري_ڏيو(cmdlen * حجم(wchar_t));
    إذا (cmdStrW == عدم)
	أرجع E_FAIL;
    getGvimInvocationW(cmdStrW);

    إذا (gvimExtraOptions == EDIT_WITH_VIM_اندريون_DIFF_گهڻائي)
	wcscat(cmdStrW, L" -d");
    وإلا إذا (gvimExtraOptions == EDIT_WITH_VIM_USE_TABPAGES)
	wcscat(cmdStrW, L" -p");
    لكل (i = 0; i < cbFiles; i++)
    {
	DragQueryFileW((HDROP)medium.hGlobal,
		i,
		m_szFileUserClickedOn,
		ARRAY_ڊيگهائي(m_szFileUserClickedOn));

	len = wcslen(cmdStrW) + wcslen(m_szFileUserClickedOn) + 4;
	إذا (len > cmdlen)
	{
	    cmdlen = len + BUFSIZE;
	    wchar_t *cmdStrW_new = (wchar_t *)realloc(cmdStrW, cmdlen * حجم(wchar_t));
	    إذا (cmdStrW_new == عدم)
	    {
		حرر(cmdStrW);
		أرجع E_FAIL;
	    }
	    cmdStrW = cmdStrW_new;
	}
	wcscat(cmdStrW, L" \"");
	wcscat(cmdStrW, m_szFileUserClickedOn);
	wcscat(cmdStrW, L"\"");
    }

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    Zero_يادگيري(&si, حجم(si));
    si.cb = حجم(si);

    // Start the child process.
    إذا (!CreateProcessW(عدم,	// No module name (use command line).
		cmdStrW,	// Command line.
		عدم,		// Process handle not inheritable.
		عدم,		// Thread handle not inheritable.
		FALSE,		// Set handle inheritance to FALSE.
		0,		// No creation flags.
		عدم,		// Use parent's environment block.
		workingDir,	// Use parent's starting directory.
		&si,		// Pointer to STARTUPINFO structure.
		&pi)		// Pointer to PROCESS_INFORMATION structure.
       )
    {
	// Use UTF-8 for gettext.
	حرف *prev = سيٽ_gettext_codeset();

	WCHAR *نياپو = W(_("Error creating process: Check if gvim is in your path!"));
	WCHAR *title = W(_("gvimext.dll error"));

	پيغام_Box_W(h_والد, نياپو, title, MB_OK);

	حرر(نياپو);
	حرر(title);

	// Restore previous codeset.
	restore_gettext_codeset(prev);
    }
    وإلا
    {
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
    }
    حرر(cmdStrW);

    أرجع NOERROR;
}
