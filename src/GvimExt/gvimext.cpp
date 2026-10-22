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

#تعریف_کرو ARRAY_لمبائی(a) (حجم(a) / حجم((a)[0]))

ساکن حرف *searchpath(حرف *name);

// Always get an error while putting the following stuff to the
// gvimext.h file as class protected variables, give up and
// declare them as global stuff
FORMATETC fmte = {CF_HDROP,
		  (DVTARGETDEVICE FAR *)تہی,
		  DVASPECT_CONTENT,
		  -1,
		  TYMED_HGLOBAL
		 };
STGMEDIUM medium;
HRESULT hres = 0;
UINT cbFiles = 0;

/* The buffers size used to be MAX_PATH (260 bytes), but that's not always
 * enough */
#تعریف_کرو BUFSIZE 1100

// The "Edit with Vim" shell extension provides these choices when
// a new instance of Gvim is selected:
//   - use tabpages
//   - enable diff mode
//   - none of the above
#تعریف_کرو EDIT_WITH_VIM_USE_TABPAGES (2)
#تعریف_کرو EDIT_WITH_VIM_اندر_DIFF_منوال (1)
#تعریف_کرو EDIT_WITH_VIM_NO_OPTIONS   (0)

//
// Get the name of the Gvim executable to use, with the path.
// When "runtime" is non-zero, we were called to find the runtime directory.
// Returns the path in name[BUFSIZE].  It's empty when it fails.
//
    ساکن خالی
getGvimName(حرف *name, BOOL runtime)
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
	اگر (Reg_Query_قدر_Ex(keyhandle, "path", 0, تہی, (BYTE *)name, &hlen)
							     != ERROR_SUCCESS)
	    name[0] = 0;
	ورنہ
	    name[hlen] = 0;
	Reg_Close_کلید(keyhandle);
    }

    اگر ((name[0] == 0) &&
	    (Reg_Open_کلید_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
				       کلید_READ, &keyhandle) == ERROR_SUCCESS))
    {
	hlen = BUFSIZE;
	اگر (Reg_Query_قدر_Ex(keyhandle, "path", 0, تہی, (BYTE *)name, &hlen)
							     != ERROR_SUCCESS)
	    name[0] = 0;
	ورنہ
	    name[hlen] = 0;
	Reg_Close_کلید(keyhandle);
    }

    // Registry didn't work, use the search path.
    اگر (name[0] == 0)
	strcpy(name, searchpath((حرف *)"gvim.exe"));

    اگر (!runtime)
    {
	// Only when looking for the executable, not the runtime dir, we can
	// search for the batch file or a name without a path.
	اگر (name[0] == 0)
	    strcpy(name, searchpath((حرف *)"gvim.bat"));
	اگر (name[0] == 0)
	    strcpy(name, "gvim");	// finds gvim.bat or gvim.exe
    }
}

    ساکن خالی
getGvimInvocation(حرف *name)
{
    getGvimName(name, FALSE);
    // avoid that Vim tries to expand wildcards in the file names
    strcat(name, " --literal");
}

    ساکن خالی
getGvimInvocationW(wchar_t *nameW)
{
    حرف *name;

    name = (حرف *)حافظہ_دو(BUFSIZE);
    getGvimInvocation(name);
    mbstowcs(nameW, name, BUFSIZE);
    آزاد(name);
}

    ساکن BOOL
FileExists(LPCTSTR sz_راستہ)
{
    DWORD dwAttrib = Get_File_اوصاف(sz_راستہ);

    واپس (dwAttrib != INVALID_FILE_اوصاف
	    && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

//
// Get the Vim runtime directory into buf[BUFSIZE].
// The result is empty when it failed.
// When it works, the path ends in a slash or backslash.
//
    ساکن خالی
getRuntimeDir(حرف *buf)
{
    صحیح		idx;

    getGvimName(buf, TRUE);
    اگر (buf[0] == 0)
	واپس;

    // When no path found, use the search path to expand it.
    اگر (strchr(buf, '/') == تہی && strchr(buf, '\\') == تہی)
	strcpy(buf, searchpath(buf));

    // remove "gvim.exe" from the end
    برائے (idx = (صحیح)لڑی_لمبائی(buf) - 1; idx >= 0; idx--)
	اگر (buf[idx] == '\\' || buf[idx] == '/')
	{
	    buf[++idx] = 0;
	    توڑو;
	}

    // Check the existence of defaults.vim
    حرف *p = buf + idx;
    strcpy(p, "defaults.vim");
    اگر (FileExists(buf))
    {
	*p = 0;		// Cut "defaults.vim"
	واپس;
    }
    // Check the existence of vimXX\defaults.vim
    strcpy(p, VIM_نسخہ_NODOT "\\" "defaults.vim");
    اگر (FileExists(buf))
    {
	p[ARRAY_لمبائی(VIM_نسخہ_NODOT)] = 0;	// Cut "defaults.vim"
	واپس;
    }
    // Check the existence of runtime\defaults.vim
    strcpy(p, "runtime" "\\" "defaults.vim");
    اگر (FileExists(buf))
    {
	p[ARRAY_لمبائی("runtime")] = 0;		// Cut "defaults.vim"
	واپس;
    }
    // Not found
    buf[0] = 0;
    واپس;
}

    WCHAR *
utf8_مقصد_utf16(ثابت حرف *s)
{
    صحیح سائز = Multi_Byte_مقصد_Wide_Char(CP_UTF8, 0, s, -1, تہی, 0);
    WCHAR *buf = (WCHAR *)حافظہ_دو(سائز * حجم(WCHAR));
    Multi_Byte_مقصد_Wide_Char(CP_UTF8, 0, s, -1, buf, سائز);
    واپس buf;
}

    HBITMAP
Icon_مقصد_Bitmap(HICON hIcon, HBRUSH hBackground, صحیح چوڑائی, صحیح اونچائی)
{
    HDC hDC = GetDC(تہی);
    HDC hMemDC = CreateCompatibleDC(hDC);
    HBITMAP hMemBmp = CreateCompatibleBitmap(hDC, چوڑائی, اونچائی);
    HBITMAP h_نتیجہ_Bmp = تہی;
    HGDIOBJ hOrgBMP = SelectObject(hMemDC, hMemBmp);

    DrawIconEx(hMemDC, 0, 0, hIcon, چوڑائی, اونچائی, 0, hBackground, DI_معمول);

    h_نتیجہ_Bmp = hMemBmp;
    hMemBmp = تہی;

    SelectObject(hMemDC, hOrgBMP);
    DeleteDC(hMemDC);
    ReleaseDC(تہی, hDC);
    DestroyIcon(hIcon);
    واپس h_نتیجہ_Bmp;
}

//
// GETTEXT: translated messages and menu entries
//
#اگر_غیر_تعریف_شدہ FEAT_GETTEXT
# define _(x)	    x
# define W_impl(x) _wcsdup(L##x)
# define W(x)	    W_impl(x)
# define سیٹ_gettext_codeset()	    تہی
# define restore_gettext_codeset(x)
#ورنہ
# define _(x)	    (*dyn_libintl_gettext)(x)
# define W(x)	    utf8_مقصد_utf16(x)
# define VIMPACKAGE "vim"
# ifndef GETTEXT_DLL
#  define GETTEXT_DLL "libintl.dll"
#  define GETTEXT_DLL_ALT "libintl-8.dll"
# endif

// Dummy functions
ساکن حرف *null_libintl_gettext(ثابت حرف *);
ساکن حرف *null_libintl_textdomain(ثابت حرف *);
ساکن حرف *null_libintl_bindtextdomain(ثابت حرف *, ثابت حرف *);
ساکن حرف *null_libintl_bind_textdomain_codeset(ثابت حرف *, ثابت حرف *);
ساکن صحیح dyn_libintl_init(حرف *dir);
ساکن خالی dyn_libintl_انتہا(خالی);

ساکن HINSTANCE hLibintlDLL = 0;
ساکن حرف *(*dyn_libintl_gettext)(ثابت حرف *) = null_libintl_gettext;
ساکن حرف *(*dyn_libintl_textdomain)(ثابت حرف *) = null_libintl_textdomain;
ساکن حرف *(*dyn_libintl_bindtextdomain)(ثابت حرف *, ثابت حرف *)
						= null_libintl_bindtextdomain;
ساکن حرف *(*dyn_libintl_bind_textdomain_codeset)(ثابت حرف *, ثابت حرف *)
				       = null_libintl_bind_textdomain_codeset;

//
// Attempt to load libintl.dll.  If it doesn't work, use dummy functions.
// "dir" is the directory where the libintl.dll might be.
// Return 1 for success, 0 for failure.
//
    ساکن صحیح
dyn_libintl_init(حرف *dir)
{
    صحیح		i;
    ساکن ساخت
    {
	حرف	    *name;
	FARPROC	    *ptr;
    } libintl_اندراج[] =
    {
	{(حرف *)"gettext",		(FARPROC*)&dyn_libintl_gettext},
	{(حرف *)"textdomain",		(FARPROC*)&dyn_libintl_textdomain},
	{(حرف *)"bindtextdomain",	(FARPROC*)&dyn_libintl_bindtextdomain},
	{(حرف *)"bind_textdomain_codeset", (FARPROC*)&dyn_libintl_bind_textdomain_codeset},
	{تہی, تہی}
    };
    DWORD	len, len2;
    LPWSTR	buf = تہی;
    LPWSTR	buf2 = تہی;

    // No need to initialize twice.
    اگر (hLibintlDLL)
	واپس 1;

    // Load gettext library from $VIMRUNTIME\GvimExt{64,32} directory.
    // Add the directory to $PATH temporarily.
    len = GetEnvironmentVariableW(L"PATH", تہی, 0);
    len2 = MAX_راستہ + 1 + len;
    buf = (LPWSTR)حافظہ_دو(len * حجم(WCHAR));
    buf2 = (LPWSTR)حافظہ_دو(len2 * حجم(WCHAR));
    اگر (buf != تہی && buf2 != تہی)
    {
	GetEnvironmentVariableW(L"PATH", buf, len);
# ifdef _WIN64
	_snwprintf(buf2, len2, L"%S\\GvimExt64;%s", dir, buf);
# ورنہ
	_snwprintf(buf2, len2, L"%S\\GvimExt32;%s", dir, buf);
# endif
	سیٹ_Environment_Variable_W(L"PATH", buf2);
	hLibintlDLL = بھرو_Library(GETTEXT_DLL);
# ifdef GETTEXT_DLL_ALT
	اگر (!hLibintlDLL)
	    hLibintlDLL = بھرو_Library(GETTEXT_DLL_ALT);
# endif
	سیٹ_Environment_Variable_W(L"PATH", buf);
    }
    آزاد(buf);
    آزاد(buf2);
    اگر (!hLibintlDLL)
	واپس 0;

    // Get the addresses of the functions we need.
    برائے (i = 0; libintl_اندراج[i].name != تہی
					 && libintl_اندراج[i].ptr != تہی; ++i)
    {
	اگر ((*libintl_اندراج[i].ptr = Get_Proc_پتہ(hLibintlDLL,
					      libintl_اندراج[i].name)) == تہی)
	{
	    dyn_libintl_انتہا();
	    واپس 0;
	}
    }
    واپس 1;
}

    ساکن خالی
dyn_libintl_انتہا(خالی)
{
    اگر (hLibintlDLL)
	FreeLibrary(hLibintlDLL);
    hLibintlDLL			= تہی;
    dyn_libintl_gettext		= null_libintl_gettext;
    dyn_libintl_textdomain	= null_libintl_textdomain;
    dyn_libintl_bindtextdomain	= null_libintl_bindtextdomain;
    dyn_libintl_bind_textdomain_codeset	= null_libintl_bind_textdomain_codeset;
}

    ساکن حرف *
null_libintl_gettext(ثابت حرف *msgid)
{
    واپس (حرف *)msgid;
}

    ساکن حرف *
null_libintl_textdomain(ثابت حرف * /* domainname */)
{
    واپس تہی;
}

    ساکن حرف *
null_libintl_bindtextdomain(ثابت حرف * /* domainname */, ثابت حرف * /* dirname */)
{
    واپس تہی;
}

    ساکن حرف *
null_libintl_bind_textdomain_codeset(ثابت حرف * /* domainname */, ثابت حرف * /* codeset */)
{
    واپس تہی;
}

//
// Setup for translating strings.
//
    ساکن خالی
dyn_gettext_بھرو(خالی)
{
    حرف    szBuff[BUFSIZE];
    DWORD   len;

    // Try to locate the runtime files.  The path is used to find libintl.dll
    // and the vim.mo files.
    getRuntimeDir(szBuff);
    اگر (szBuff[0] != 0)
    {
	len = (DWORD)لڑی_لمبائی(szBuff);
	اگر (dyn_libintl_init(szBuff))
	{
	    strcpy(szBuff + len, "lang");

	    (*dyn_libintl_bindtextdomain)(VIMPACKAGE, szBuff);
	    (*dyn_libintl_textdomain)(VIMPACKAGE);
	}
    }
}

    ساکن خالی
dyn_gettext_free(خالی)
{
    dyn_libintl_انتہا();
}

//
// Use UTF-8 for gettext. Returns previous codeset.
//
    ساکن حرف *
سیٹ_gettext_codeset(خالی)
{
    حرف *پچھلا = dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, تہی);
    پچھلا = _strdup((پچھلا != تہی) ? پچھلا : "char");
    dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, "utf-8");

    واپس پچھلا;
}

//
// Restore previous codeset for gettext.
//
    ساکن خالی
restore_gettext_codeset(حرف *پچھلا)
{
    dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, پچھلا);
    آزاد(پچھلا);
}
#اگر_ختم // FEAT_GETTEXT

//
// Global variables
//
UINT      g_cRefThisDll = 0;    // Reference count of this DLL.
HINSTANCE g_hmodThisDll = تہی;	// Handle to this DLL itself.


//---------------------------------------------------------------------------
// DllMain
//---------------------------------------------------------------------------
خارجی "C" صحیح APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID  /* lpReserved */)
{
    انتخاب (dwReason)
    {
    حالت DLL_PROCESS_ATTACH:
	// Extension DLL one-time initialization
	g_hmodThisDll = hInstance;
	توڑو;

    حالت DLL_PROCESS_DETACH:
	توڑو;
    }

    واپس 1;   // ok
}

    ساکن خالی
inc_cRefThisDLL()
{
#اگر_تعریف_شدہ FEAT_GETTEXT
    اگر (g_cRefThisDll == 0)
	dyn_gettext_بھرو();
#اگر_ختم
    InterlockedIncrement((LPLONG)&g_cRefThisDll);
}

    ساکن خالی
dec_cRefThisDLL()
{
#اگر_تعریف_شدہ FEAT_GETTEXT
    اگر (InterlockedDecrement((LPLONG)&g_cRefThisDll) == 0)
	dyn_gettext_free();
#ورنہ
    InterlockedDecrement((LPLONG)&g_cRefThisDll);
#اگر_ختم
}

//---------------------------------------------------------------------------
// DllCanUnloadNow
//---------------------------------------------------------------------------

STDAPI DllCanUnloadNow(خالی)
{
    واپس (g_cRefThisDll == 0 ? S_OK : S_FALSE);
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppv_برآمد)
{
    *ppv_برآمد = تہی;

    اگر (IsEqualIID(rclsid, CLSID_ShellExtension))
    {
	CShellExtClassFactory *pcf = نیا CShellExtClassFactory;

	واپس pcf->QueryInterface(riid, ppv_برآمد);
    }

    واپس CLASS_E_CLASSNOTAVAILABLE;
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
    *ppv = تہی;

    // any interface on this object is the object pointer

    اگر (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	*ppv = (LPCLASSFACTORY)یہ;

	جوڑو_Ref();

	واپس NOERROR;
    }

    واپس E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::جوڑو_Ref()
{
    واپس InterlockedIncrement((LPLONG)&m_cRef);
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::Release()
{
    اگر (InterlockedDecrement((LPLONG)&m_cRef))
	واپس m_cRef;

    حذف یہ;

    واپس 0L;
}

STDMETHODIMP CShellExtClassFactory::CreateInstance(LPUNKNOWN pUnkOuter,
						      REFIID riid,
						      LPVOID *ppvObj)
{
    *ppvObj = تہی;

    // Shell extensions typically don't support aggregation (inheritance)

    اگر (pUnkOuter)
	واپس CLASS_E_NOAGGREGATION;

    // Create the main shell extension object.  The shell will then call
    // QueryInterface with IID_IShellExtInit--this is how shell extensions are
    // initialized.

    LPCSHELLEXT pShellExt = نیا CShellExt();  // create the CShellExt object

    اگر (تہی == pShellExt)
	واپس E_OUTOFMEMORY;

    واپس pShellExt->QueryInterface(riid, ppvObj);
}


STDMETHODIMP CShellExtClassFactory::تالا_Server(BOOL  /* fLock */)
{
    واپس NOERROR;
}

// *********************** CShellExt *************************
CShellExt::CShellExt()
{
    m_cRef = 0L;
    m_p_ڈیٹا_Obj = تہی;

    inc_cRefThisDLL();

    بھرو_Menu_Icon();
}

CShellExt::~CShellExt()
{
    اگر (m_p_ڈیٹا_Obj)
	m_p_ڈیٹا_Obj->Release();

    dec_cRefThisDLL();

    اگر (m_hVimIconBitmap)
	DeleteObject(m_hVimIconBitmap);
}

STDMETHODIMP CShellExt::QueryInterface(REFIID riid, LPVOID FAR *ppv)
{
    *ppv = تہی;

    اگر (IsEqualIID(riid, IID_IShellExtInit) || IsEqualIID(riid, IID_IUnknown))
    {
	*ppv = (LPSHELLEXTINIT)یہ;
    }
    ورنہ اگر (IsEqualIID(riid, IID_IContextMenu))
    {
	*ppv = (LPCONTEXTMENU)یہ;
    }

    اگر (*ppv)
    {
	جوڑو_Ref();

	واپس NOERROR;
    }

    واپس E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExt::جوڑو_Ref()
{
    واپس InterlockedIncrement((LPLONG)&m_cRef);
}

STDMETHODIMP_(ULONG) CShellExt::Release()
{

    اگر (InterlockedDecrement((LPLONG)&m_cRef))
	واپس m_cRef;

    حذف یہ;

    واپس 0L;
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
				   LPDATAOBJECT p_ڈیٹا_Obj,
				   HKEY  /* hRegKey */)
{
    // Initialize can be called more than once
    اگر (m_p_ڈیٹا_Obj)
	m_p_ڈیٹا_Obj->Release();

    // duplicate the object pointer and registry handle

    اگر (p_ڈیٹا_Obj)
    {
	m_p_ڈیٹا_Obj = p_ڈیٹا_Obj;
	p_ڈیٹا_Obj->جوڑو_Ref();
    }

    واپس NOERROR;
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
					 UINT اشاریہ_Menu,
					 UINT id_Cmd_پہلا,
					 UINT  /* idCmdLast */,
					 UINT  /* uFlags */)
{
    UINT idCmd = id_Cmd_پہلا;

    hres = m_p_ڈیٹا_Obj->Get_ڈیٹا(&fmte, &medium);
    اگر (medium.hGlobal)
	cbFiles = DragQueryFileW((HDROP)medium.hGlobal, (UINT)-1, 0, 0);

    // InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

    // Initialize m_cntOfHWnd to 0
    m_cnt_کا_HWnd = 0;

    HKEY keyhandle;
    منطقی showExisting = سچ;
    منطقی showIcons = سچ;

    // Check whether "Edit with existing Vim" entries are disabled.
    اگر (Reg_Open_کلید_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
				       کلید_READ, &keyhandle) == ERROR_SUCCESS)
    {
	اگر (Reg_Query_قدر_Ex(keyhandle, "DisableEditWithExisting", 0, تہی,
						 تہی, تہی) == ERROR_SUCCESS)
	    showExisting = جھوٹ;
	اگر (Reg_Query_قدر_Ex(keyhandle, "DisableContextMenuIcons", 0, تہی,
						 تہی, تہی) == ERROR_SUCCESS)
	    showIcons = جھوٹ;
	Reg_Close_کلید(keyhandle);
    }

    // Use UTF-8 for gettext.
    حرف *پچھلا = سیٹ_gettext_codeset();

    // Retrieve all the vim instances, unless disabled.
    اگر (showExisting)
	EnumWindows(EnumWindowsProc, (LPARAM)یہ);

    MENUITEMINFOW mii = { حجم(MENUITEMINFOW) };
    mii.f_نقاب = MIIM_STRING | MIIM_ID;
    اگر (showIcons)
    {
	mii.f_نقاب |= MIIM_BITMAP;
	mii.hbmp_شے = m_hVimIconBitmap;
    }

    اگر (cbFiles > 1)
    {
	mii.wID = idCmd++;
	mii.dw_Type_ڈیٹا = W(_("Edit with Vim using &tabpages"));
	mii.cch = wcslen(mii.dw_Type_ڈیٹا);
	داخل_کرو_Menu_شے_W(hMenu, اشاریہ_Menu++, TRUE, &mii);
	آزاد(mii.dw_Type_ڈیٹا);

	mii.wID = idCmd++;
	mii.dw_Type_ڈیٹا = W(_("Edit with single &Vim"));
	mii.cch = wcslen(mii.dw_Type_ڈیٹا);
	داخل_کرو_Menu_شے_W(hMenu, اشاریہ_Menu++, TRUE, &mii);
	آزاد(mii.dw_Type_ڈیٹا);

	اگر (cbFiles <= 4)
	{
	    // Can edit up to 4 files in diff mode
	    mii.wID = idCmd++;
	    mii.dw_Type_ڈیٹا = W(_("Diff with Vim"));
	    mii.cch = wcslen(mii.dw_Type_ڈیٹا);
	    داخل_کرو_Menu_شے_W(hMenu, اشاریہ_Menu++, TRUE, &mii);
	    آزاد(mii.dw_Type_ڈیٹا);
	    m_edit_existing_off = 3;
	}
	ورنہ
	    m_edit_existing_off = 2;

    }
    ورنہ
    {
	mii.wID = idCmd++;
	mii.dw_Type_ڈیٹا = W(_("Edit with &Vim"));
	mii.cch = wcslen(mii.dw_Type_ڈیٹا);
	داخل_کرو_Menu_شے_W(hMenu, اشاریہ_Menu++, TRUE, &mii);
	آزاد(mii.dw_Type_ڈیٹا);
	m_edit_existing_off = 1;
    }

    HMENU hSubMenu = تہی;
    اگر (m_cnt_کا_HWnd > 1)
    {
	hSubMenu = CreatePopupMenu();
	mii.f_نقاب |= MIIM_SUBMENU;
	mii.wID = idCmd;
	mii.dw_Type_ڈیٹا = W(_("Edit with existing Vim"));
	mii.cch = wcslen(mii.dw_Type_ڈیٹا);
	mii.hSubMenu = hSubMenu;
	داخل_کرو_Menu_شے_W(hMenu, اشاریہ_Menu++, TRUE, &mii);
	آزاد(mii.dw_Type_ڈیٹا);
	mii.f_نقاب = mii.f_نقاب & ~MIIM_SUBMENU;
	mii.hSubMenu = تہی;
    }
    // Now display all the vim instances
    برائے (صحیح i = 0; i < m_cnt_کا_HWnd; i++)
    {
	WCHAR title[BUFSIZE];
	WCHAR عارضی[BUFSIZE];
	صحیح اشاریہ;
	HMENU hmenu;

	// Obtain window title, continue if can not
	اگر (Get_کھڑکی_متن_W(m_hWnd[i], title, BUFSIZE - 1) == 0)
	    جاری;
	// Truncate the title before the path, keep the file name
	WCHAR *pos = wcschr(title, L'(');
	اگر (pos != تہی)
	{
	    اگر (pos > title && pos[-1] == L' ')
		--pos;
	    *pos = 0;
	}
	// Now concatenate
	اگر (m_cnt_کا_HWnd > 1)
	    عارضی[0] = L'\0';
	ورنہ
	{
	    WCHAR *s = W(_("Edit with existing Vim - "));
	    wcsncpy(عارضی, s, BUFSIZE - 1);
	    عارضی[BUFSIZE - 1] = L'\0';
	    آزاد(s);
	}
	wcsncat(عارضی, title, BUFSIZE - 1 - wcslen(عارضی));
	عارضی[BUFSIZE - 1] = L'\0';

	mii.wID = idCmd++;
	mii.dw_Type_ڈیٹا = عارضی;
	mii.cch = wcslen(mii.dw_Type_ڈیٹا);
	اگر (m_cnt_کا_HWnd > 1)
	{
	    hmenu = hSubMenu;
	    اشاریہ = i;
	}
	ورنہ
	{
	    hmenu = hMenu;
	    اشاریہ = اشاریہ_Menu++;
	}
	داخل_کرو_Menu_شے_W(hmenu, اشاریہ, TRUE, &mii);
    }
    // InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

    // Restore previous codeset.
    restore_gettext_codeset(پچھلا);

    // Must return number of menu items we added.
    واپس نتیجہ_From_Short(idCmd-id_Cmd_پہلا);
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
	    hr = ڈالو_مقصد_کھڑکی(lpcmi->hwnd,
		    lpcmi->lpDirectory,
		    lpcmi->lpVerb,
		    lpcmi->lpParameters,
		    lpcmi->nShow,
		    idCmd - m_edit_existing_off);
	}
	ورنہ
	{
	    انتخاب (idCmd)
	    {
		حالت 0:
		    gvimExtraOptions = EDIT_WITH_VIM_USE_TABPAGES;
		    توڑو;
		حالت 1:
		    gvimExtraOptions = EDIT_WITH_VIM_NO_OPTIONS;
		    توڑو;
		حالت 2:
		    gvimExtraOptions = EDIT_WITH_VIM_اندر_DIFF_منوال;
		    توڑو;
		طےشدہ:
		    // If execution reaches this point we likely have an
		    // inconsistency between the code that setup the menus
		    // and this code that determines what the user
		    // selected.  This should be detected and fixed during
		    // development.
		    واپس E_FAIL;
	    }

            LPCMINVOKECOMMANDINFOEX lpcmiex = (LPCMINVOKECOMMANDINFOEX)lpcmi;
            LPCWSTR currentDirectory = lpcmi->cb_سائز == حجم(CMINVOKECOMMANDINFOEX) ? lpcmiex->lpDirectoryW : تہی;

	    hr = InvokeSingleGvim(lpcmi->hwnd,
		    currentDirectory,
		    lpcmi->lpVerb,
		    lpcmi->lpParameters,
		    lpcmi->nShow,
		    gvimExtraOptions);
	}
    }
    واپس hr;
}

STDMETHODIMP CShellExt::ڈالو_مقصد_کھڑکی(HWND  /* hParent */,
				   LPCSTR  /* pszWorkingDir */,
				   LPCSTR  /* pszCmd */,
				   LPCSTR  /* pszParam */,
				   صحیح  /* iShowCmd */,
				   صحیح idHWnd)
{
    HWND hWnd = m_hWnd[idHWnd];

    // Show and bring vim instance to foreground
    اگر (IsIconic(hWnd) != 0)
	Show_کھڑکی(hWnd, SW_RESTORE);
    ورنہ
	Show_کھڑکی(hWnd, SW_SHOW);
    سیٹ_Foreground_کھڑکی(hWnd);

    // Post the selected files to the vim instance
    Post_پیغام(hWnd, WM_DROPFILES, (WPARAM)medium.hGlobal, 0);

    واپس NOERROR;
}

STDMETHODIMP CShellExt::GetCommandString(UINT_PTR  /* idCmd */,
					 UINT uFlags,
					 UINT FAR * /* reserved */,
					 LPSTR pszName,
					 UINT cchMax)
{
    // Use UTF-8 for gettext.
    حرف *پچھلا = سیٹ_gettext_codeset();

    WCHAR *s = W(_("Edits the selected file(s) with Vim"));
    اگر (uFlags == GCS_HELPTEXTW && cchMax > wcslen(s))
	wcscpy((WCHAR *)pszName, s);
    آزاد(s);

    // Restore previous codeset.
    restore_gettext_codeset(پچھلا);

    واپس NOERROR;
}

BOOL CALLBACK CShellExt::EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    حرف عارضی[BUFSIZE];

    // First do a bunch of check
    // No invisible window
    اگر (!Is_کھڑکی_Visible(hWnd))
	واپس TRUE;
    // No child window ???
    // if (GetParent(hWnd)) return TRUE;
    // Class name should be Vim, if failed to get class name, return
    اگر (GetClassName(hWnd, عارضی, ARRAY_لمبائی(عارضی)) == 0)
	واپس TRUE;
    // Compare class name to that of vim, if not, return
    اگر (_strnicmp(عارضی, "vim", ARRAY_لمبائی("vim")) != 0)
	واپس TRUE;
    // First check if the number of vim instance exceeds MAX_HWND
    CShellExt *cs = (CShellExt*) lParam;
    اگر (cs->m_cnt_کا_HWnd >= MAX_HWND)
	واپس FALSE;	// stop enumeration
    // Now we get the vim window, put it into some kind of array
    cs->m_hWnd[cs->m_cnt_کا_HWnd] = hWnd;
    cs->m_cnt_کا_HWnd ++;

    واپس TRUE; // continue enumeration (otherwise this would be false)
}

BOOL CShellExt::بھرو_Menu_Icon()
{
    حرف vimExeFile[BUFSIZE];
    getGvimName(vimExeFile, TRUE);
    اگر (vimExeFile[0] == '\0')
	واپس FALSE;
    HICON hVimIcon;
    اگر (نکالو_Icon_Ex(vimExeFile, 0, تہی, &hVimIcon, 1) == 0)
	واپس FALSE;
    m_hVimIconBitmap = Icon_مقصد_Bitmap(hVimIcon,
	    GetSysColorBrush(COLOR_MENU),
	    GetSystemMetrics(SM_CXSMICON),
	    GetSystemMetrics(SM_CYSMICON));
    واپس TRUE;
}

    ساکن حرف *
searchpath(حرف *name)
{
    ساکن اتحاد {
	حرف	location[2 * BUFSIZE];
	WCHAR	wname[BUFSIZE];
    };
    WCHAR wlocation[BUFSIZE + 1];

    // There appears to be a bug in FindExecutableA() on Windows NT.
    // Use FindExecutableW() instead...
    Multi_Byte_مقصد_Wide_Char(CP_ACP, 0, name, -1, wname, BUFSIZE);
    اگر (ڈھونڈو_Executable_W(wname, L"", wlocation) > (HINSTANCE)32)
    {
	Wide_Char_مقصد_Multi_Byte(CP_ACP, 0, wlocation, -1,
		location, حجم(location), تہی, تہی);
	واپس location;
    }
    واپس (حرف *)"";
}


STDMETHODIMP CShellExt::InvokeSingleGvim(HWND h_والد,
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
    cmdStrW  = (wchar_t *) حافظہ_دو(cmdlen * حجم(wchar_t));
    اگر (cmdStrW == تہی)
	واپس E_FAIL;
    getGvimInvocationW(cmdStrW);

    اگر (gvimExtraOptions == EDIT_WITH_VIM_اندر_DIFF_منوال)
	wcscat(cmdStrW, L" -d");
    ورنہ اگر (gvimExtraOptions == EDIT_WITH_VIM_USE_TABPAGES)
	wcscat(cmdStrW, L" -p");
    برائے (i = 0; i < cbFiles; i++)
    {
	DragQueryFileW((HDROP)medium.hGlobal,
		i,
		m_szFileUserClickedOn,
		ARRAY_لمبائی(m_szFileUserClickedOn));

	len = wcslen(cmdStrW) + wcslen(m_szFileUserClickedOn) + 4;
	اگر (len > cmdlen)
	{
	    cmdlen = len + BUFSIZE;
	    wchar_t *cmdStrW_new = (wchar_t *)realloc(cmdStrW, cmdlen * حجم(wchar_t));
	    اگر (cmdStrW_new == تہی)
	    {
		آزاد(cmdStrW);
		واپس E_FAIL;
	    }
	    cmdStrW = cmdStrW_new;
	}
	wcscat(cmdStrW, L" \"");
	wcscat(cmdStrW, m_szFileUserClickedOn);
	wcscat(cmdStrW, L"\"");
    }

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    Zero_حافظہ(&si, حجم(si));
    si.cb = حجم(si);

    // Start the child process.
    اگر (!CreateProcessW(تہی,	// No module name (use command line).
		cmdStrW,	// Command line.
		تہی,		// Process handle not inheritable.
		تہی,		// Thread handle not inheritable.
		FALSE,		// Set handle inheritance to FALSE.
		0,		// No creation flags.
		تہی,		// Use parent's environment block.
		workingDir,	// Use parent's starting directory.
		&si,		// Pointer to STARTUPINFO structure.
		&pi)		// Pointer to PROCESS_INFORMATION structure.
       )
    {
	// Use UTF-8 for gettext.
	حرف *پچھلا = سیٹ_gettext_codeset();

	WCHAR *پیغام = W(_("Error creating process: Check if gvim is in your path!"));
	WCHAR *title = W(_("gvimext.dll error"));

	پیغام_Box_W(h_والد, پیغام, title, MB_OK);

	آزاد(پیغام);
	آزاد(title);

	// Restore previous codeset.
	restore_gettext_codeset(پچھلا);
    }
    ورنہ
    {
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
    }
    آزاد(cmdStrW);

    واپس NOERROR;
}
