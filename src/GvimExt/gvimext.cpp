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

#అన్తర్భావ "gvimext.h"
#అన్తర్భావ "../version.h"

#నిర్వచించు ARRAY_నిడివి(a) (పరిమాణం(a) / పరిమాణం((a)[0]))

స్థిర అక్షరం *searchpath(అక్షరం *name);

// Always get an error while putting the following stuff to the
// gvimext.h file as class protected variables, give up and
// declare them as global stuff
FORMATETC fmte = {CF_HDROP,
		  (DVTARGETDEVICE FAR *)ఖాళీ,
		  DVASPECT_CONTENT,
		  -1,
		  TYMED_HGLOBAL
		 };
STGMEDIUM medium;
HRESULT hres = 0;
UINT cbFiles = 0;

/* The buffers size used to be MAX_PATH (260 bytes), but that's not always
 * enough */
#నిర్వచించు BUFSIZE 1100

// The "Edit with Vim" shell extension provides these choices when
// a new instance of Gvim is selected:
//   - use tabpages
//   - enable diff mode
//   - none of the above
#నిర్వచించు EDIT_WITH_VIM_USE_TABPAGES (2)
#నిర్వచించు EDIT_WITH_VIM_లోపల_DIFF_బహుళకం (1)
#నిర్వచించు EDIT_WITH_VIM_NO_OPTIONS   (0)

//
// Get the name of the Gvim executable to use, with the path.
// When "runtime" is non-zero, we were called to find the runtime directory.
// Returns the path in name[BUFSIZE].  It's empty when it fails.
//
    స్థిర శూన్య
getGvimName(అక్షరం *name, BOOL runtime)
{
    HKEY	keyhandle;
    DWORD	hlen;

    // Get the location of gvim from the registry.  Try
    // HKEY_CURRENT_USER first, then fall back to HKEY_LOCAL_MACHINE if
    // not found.
    name[0] = 0;
    ఒకవేళ (Reg_Open_కీలకం_Ex(HKEY_CURRENT_USER, "Software\\Vim\\Gvim", 0,
				       కీలకం_READ, &keyhandle) == ERROR_SUCCESS)
    {
	hlen = BUFSIZE;
	ఒకవేళ (Reg_Query_విలువ_Ex(keyhandle, "path", 0, ఖాళీ, (BYTE *)name, &hlen)
							     != ERROR_SUCCESS)
	    name[0] = 0;
	లేకపోతే
	    name[hlen] = 0;
	Reg_Close_కీలకం(keyhandle);
    }

    ఒకవేళ ((name[0] == 0) &&
	    (Reg_Open_కీలకం_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
				       కీలకం_READ, &keyhandle) == ERROR_SUCCESS))
    {
	hlen = BUFSIZE;
	ఒకవేళ (Reg_Query_విలువ_Ex(keyhandle, "path", 0, ఖాళీ, (BYTE *)name, &hlen)
							     != ERROR_SUCCESS)
	    name[0] = 0;
	లేకపోతే
	    name[hlen] = 0;
	Reg_Close_కీలకం(keyhandle);
    }

    // Registry didn't work, use the search path.
    ఒకవేళ (name[0] == 0)
	strcpy(name, searchpath((అక్షరం *)"gvim.exe"));

    ఒకవేళ (!runtime)
    {
	// Only when looking for the executable, not the runtime dir, we can
	// search for the batch file or a name without a path.
	ఒకవేళ (name[0] == 0)
	    strcpy(name, searchpath((అక్షరం *)"gvim.bat"));
	ఒకవేళ (name[0] == 0)
	    strcpy(name, "gvim");	// finds gvim.bat or gvim.exe
    }
}

    స్థిర శూన్య
getGvimInvocation(అక్షరం *name)
{
    getGvimName(name, FALSE);
    // avoid that Vim tries to expand wildcards in the file names
    strcat(name, " --literal");
}

    స్థిర శూన్య
getGvimInvocationW(wchar_t *nameW)
{
    అక్షరం *name;

    name = (అక్షరం *)జ్ఞాపకం_ఇవ్వు(BUFSIZE);
    getGvimInvocation(name);
    mbstowcs(nameW, name, BUFSIZE);
    విడుదల(name);
}

    స్థిర BOOL
FileExists(LPCTSTR sz_మార్గం)
{
    DWORD dwAttrib = Get_File_లక్షణాలు(sz_మార్గం);

    తిరిగి (dwAttrib != INVALID_FILE_లక్షణాలు
	    && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

//
// Get the Vim runtime directory into buf[BUFSIZE].
// The result is empty when it failed.
// When it works, the path ends in a slash or backslash.
//
    స్థిర శూన్య
getRuntimeDir(అక్షరం *buf)
{
    పూర్ణాంకం		idx;

    getGvimName(buf, TRUE);
    ఒకవేళ (buf[0] == 0)
	తిరిగి;

    // When no path found, use the search path to expand it.
    ఒకవేళ (strchr(buf, '/') == ఖాళీ && strchr(buf, '\\') == ఖాళీ)
	strcpy(buf, searchpath(buf));

    // remove "gvim.exe" from the end
    కొరకు (idx = (పూర్ణాంకం)అక్షరశ్రేణి_పొడవు(buf) - 1; idx >= 0; idx--)
	ఒకవేళ (buf[idx] == '\\' || buf[idx] == '/')
	{
	    buf[++idx] = 0;
	    ఆపు;
	}

    // Check the existence of defaults.vim
    అక్షరం *p = buf + idx;
    strcpy(p, "defaults.vim");
    ఒకవేళ (FileExists(buf))
    {
	*p = 0;		// Cut "defaults.vim"
	తిరిగి;
    }
    // Check the existence of vimXX\defaults.vim
    strcpy(p, VIM_సంస్కరణ_NODOT "\\" "defaults.vim");
    ఒకవేళ (FileExists(buf))
    {
	p[ARRAY_నిడివి(VIM_సంస్కరణ_NODOT)] = 0;	// Cut "defaults.vim"
	తిరిగి;
    }
    // Check the existence of runtime\defaults.vim
    strcpy(p, "runtime" "\\" "defaults.vim");
    ఒకవేళ (FileExists(buf))
    {
	p[ARRAY_నిడివి("runtime")] = 0;		// Cut "defaults.vim"
	తిరిగి;
    }
    // Not found
    buf[0] = 0;
    తిరిగి;
}

    WCHAR *
utf8_గమ్యం_utf16(స్థిరరాశి అక్షరం *s)
{
    పూర్ణాంకం కొలత = Multi_Byte_గమ్యం_Wide_Char(CP_UTF8, 0, s, -1, ఖాళీ, 0);
    WCHAR *buf = (WCHAR *)జ్ఞాపకం_ఇవ్వు(కొలత * పరిమాణం(WCHAR));
    Multi_Byte_గమ్యం_Wide_Char(CP_UTF8, 0, s, -1, buf, కొలత);
    తిరిగి buf;
}

    HBITMAP
Icon_గమ్యం_Bitmap(HICON hIcon, HBRUSH hBackground, పూర్ణాంకం వెడల్పు, పూర్ణాంకం ఎత్తు)
{
    HDC hDC = GetDC(ఖాళీ);
    HDC hMemDC = CreateCompatibleDC(hDC);
    HBITMAP hMemBmp = CreateCompatibleBitmap(hDC, వెడల్పు, ఎత్తు);
    HBITMAP h_ఫలితం_Bmp = ఖాళీ;
    HGDIOBJ hOrgBMP = SelectObject(hMemDC, hMemBmp);

    DrawIconEx(hMemDC, 0, 0, hIcon, వెడల్పు, ఎత్తు, 0, hBackground, DI_సాధారణ);

    h_ఫలితం_Bmp = hMemBmp;
    hMemBmp = ఖాళీ;

    SelectObject(hMemDC, hOrgBMP);
    DeleteDC(hMemDC);
    ReleaseDC(ఖాళీ, hDC);
    DestroyIcon(hIcon);
    తిరిగి h_ఫలితం_Bmp;
}

//
// GETTEXT: translated messages and menu entries
//
#నిర్వచితం_కాకపోతే FEAT_GETTEXT
# define _(x)	    x
# define W_impl(x) _wcsdup(L##x)
# define W(x)	    W_impl(x)
# define సమితి_gettext_codeset()	    ఖాళీ
# define restore_gettext_codeset(x)
#లేకపోతే
# define _(x)	    (*dyn_libintl_gettext)(x)
# define W(x)	    utf8_గమ్యం_utf16(x)
# define VIMPACKAGE "vim"
# ifndef GETTEXT_DLL
#  define GETTEXT_DLL "libintl.dll"
#  define GETTEXT_DLL_ALT "libintl-8.dll"
# endif

// Dummy functions
స్థిర అక్షరం *null_libintl_gettext(స్థిరరాశి అక్షరం *);
స్థిర అక్షరం *null_libintl_textdomain(స్థిరరాశి అక్షరం *);
స్థిర అక్షరం *null_libintl_bindtextdomain(స్థిరరాశి అక్షరం *, స్థిరరాశి అక్షరం *);
స్థిర అక్షరం *null_libintl_bind_textdomain_codeset(స్థిరరాశి అక్షరం *, స్థిరరాశి అక్షరం *);
స్థిర పూర్ణాంకం dyn_libintl_init(అక్షరం *dir);
స్థిర శూన్య dyn_libintl_చివర(శూన్య);

స్థిర HINSTANCE hLibintlDLL = 0;
స్థిర అక్షరం *(*dyn_libintl_gettext)(స్థిరరాశి అక్షరం *) = null_libintl_gettext;
స్థిర అక్షరం *(*dyn_libintl_textdomain)(స్థిరరాశి అక్షరం *) = null_libintl_textdomain;
స్థిర అక్షరం *(*dyn_libintl_bindtextdomain)(స్థిరరాశి అక్షరం *, స్థిరరాశి అక్షరం *)
						= null_libintl_bindtextdomain;
స్థిర అక్షరం *(*dyn_libintl_bind_textdomain_codeset)(స్థిరరాశి అక్షరం *, స్థిరరాశి అక్షరం *)
				       = null_libintl_bind_textdomain_codeset;

//
// Attempt to load libintl.dll.  If it doesn't work, use dummy functions.
// "dir" is the directory where the libintl.dll might be.
// Return 1 for success, 0 for failure.
//
    స్థిర పూర్ణాంకం
dyn_libintl_init(అక్షరం *dir)
{
    పూర్ణాంకం		i;
    స్థిర నిర్మాణం
    {
	అక్షరం	    *name;
	FARPROC	    *ptr;
    } libintl_నమోదు[] =
    {
	{(అక్షరం *)"gettext",		(FARPROC*)&dyn_libintl_gettext},
	{(అక్షరం *)"textdomain",		(FARPROC*)&dyn_libintl_textdomain},
	{(అక్షరం *)"bindtextdomain",	(FARPROC*)&dyn_libintl_bindtextdomain},
	{(అక్షరం *)"bind_textdomain_codeset", (FARPROC*)&dyn_libintl_bind_textdomain_codeset},
	{ఖాళీ, ఖాళీ}
    };
    DWORD	len, len2;
    LPWSTR	buf = ఖాళీ;
    LPWSTR	buf2 = ఖాళీ;

    // No need to initialize twice.
    ఒకవేళ (hLibintlDLL)
	తిరిగి 1;

    // Load gettext library from $VIMRUNTIME\GvimExt{64,32} directory.
    // Add the directory to $PATH temporarily.
    len = GetEnvironmentVariableW(L"PATH", ఖాళీ, 0);
    len2 = MAX_మార్గం + 1 + len;
    buf = (LPWSTR)జ్ఞాపకం_ఇవ్వు(len * పరిమాణం(WCHAR));
    buf2 = (LPWSTR)జ్ఞాపకం_ఇవ్వు(len2 * పరిమాణం(WCHAR));
    ఒకవేళ (buf != ఖాళీ && buf2 != ఖాళీ)
    {
	GetEnvironmentVariableW(L"PATH", buf, len);
# ifdef _WIN64
	_snwprintf(buf2, len2, L"%S\\GvimExt64;%s", dir, buf);
# లేకపోతే
	_snwprintf(buf2, len2, L"%S\\GvimExt32;%s", dir, buf);
# endif
	సమితి_Environment_Variable_W(L"PATH", buf2);
	hLibintlDLL = ఎక్కించు_Library(GETTEXT_DLL);
# ifdef GETTEXT_DLL_ALT
	ఒకవేళ (!hLibintlDLL)
	    hLibintlDLL = ఎక్కించు_Library(GETTEXT_DLL_ALT);
# endif
	సమితి_Environment_Variable_W(L"PATH", buf);
    }
    విడుదల(buf);
    విడుదల(buf2);
    ఒకవేళ (!hLibintlDLL)
	తిరిగి 0;

    // Get the addresses of the functions we need.
    కొరకు (i = 0; libintl_నమోదు[i].name != ఖాళీ
					 && libintl_నమోదు[i].ptr != ఖాళీ; ++i)
    {
	ఒకవేళ ((*libintl_నమోదు[i].ptr = Get_Proc_చిరునామా(hLibintlDLL,
					      libintl_నమోదు[i].name)) == ఖాళీ)
	{
	    dyn_libintl_చివర();
	    తిరిగి 0;
	}
    }
    తిరిగి 1;
}

    స్థిర శూన్య
dyn_libintl_చివర(శూన్య)
{
    ఒకవేళ (hLibintlDLL)
	FreeLibrary(hLibintlDLL);
    hLibintlDLL			= ఖాళీ;
    dyn_libintl_gettext		= null_libintl_gettext;
    dyn_libintl_textdomain	= null_libintl_textdomain;
    dyn_libintl_bindtextdomain	= null_libintl_bindtextdomain;
    dyn_libintl_bind_textdomain_codeset	= null_libintl_bind_textdomain_codeset;
}

    స్థిర అక్షరం *
null_libintl_gettext(స్థిరరాశి అక్షరం *msgid)
{
    తిరిగి (అక్షరం *)msgid;
}

    స్థిర అక్షరం *
null_libintl_textdomain(స్థిరరాశి అక్షరం * /* domainname */)
{
    తిరిగి ఖాళీ;
}

    స్థిర అక్షరం *
null_libintl_bindtextdomain(స్థిరరాశి అక్షరం * /* domainname */, స్థిరరాశి అక్షరం * /* dirname */)
{
    తిరిగి ఖాళీ;
}

    స్థిర అక్షరం *
null_libintl_bind_textdomain_codeset(స్థిరరాశి అక్షరం * /* domainname */, స్థిరరాశి అక్షరం * /* codeset */)
{
    తిరిగి ఖాళీ;
}

//
// Setup for translating strings.
//
    స్థిర శూన్య
dyn_gettext_ఎక్కించు(శూన్య)
{
    అక్షరం    szBuff[BUFSIZE];
    DWORD   len;

    // Try to locate the runtime files.  The path is used to find libintl.dll
    // and the vim.mo files.
    getRuntimeDir(szBuff);
    ఒకవేళ (szBuff[0] != 0)
    {
	len = (DWORD)అక్షరశ్రేణి_పొడవు(szBuff);
	ఒకవేళ (dyn_libintl_init(szBuff))
	{
	    strcpy(szBuff + len, "lang");

	    (*dyn_libintl_bindtextdomain)(VIMPACKAGE, szBuff);
	    (*dyn_libintl_textdomain)(VIMPACKAGE);
	}
    }
}

    స్థిర శూన్య
dyn_gettext_free(శూన్య)
{
    dyn_libintl_చివర();
}

//
// Use UTF-8 for gettext. Returns previous codeset.
//
    స్థిర అక్షరం *
సమితి_gettext_codeset(శూన్య)
{
    అక్షరం *prev = dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, ఖాళీ);
    prev = _strdup((prev != ఖాళీ) ? prev : "char");
    dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, "utf-8");

    తిరిగి prev;
}

//
// Restore previous codeset for gettext.
//
    స్థిర శూన్య
restore_gettext_codeset(అక్షరం *prev)
{
    dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, prev);
    విడుదల(prev);
}
#అయితే_ముగింపు // FEAT_GETTEXT

//
// Global variables
//
UINT      g_cRefThisDll = 0;    // Reference count of this DLL.
HINSTANCE g_hmodThisDll = ఖాళీ;	// Handle to this DLL itself.


//---------------------------------------------------------------------------
// DllMain
//---------------------------------------------------------------------------
బాహ్య "C" పూర్ణాంకం APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID  /* lpReserved */)
{
    ఎంపిక (dwReason)
    {
    స్థితి DLL_PROCESS_ATTACH:
	// Extension DLL one-time initialization
	g_hmodThisDll = hInstance;
	ఆపు;

    స్థితి DLL_PROCESS_DETACH:
	ఆపు;
    }

    తిరిగి 1;   // ok
}

    స్థిర శూన్య
inc_cRefThisDLL()
{
#నిర్వచితమైతే FEAT_GETTEXT
    ఒకవేళ (g_cRefThisDll == 0)
	dyn_gettext_ఎక్కించు();
#అయితే_ముగింపు
    InterlockedIncrement((LPLONG)&g_cRefThisDll);
}

    స్థిర శూన్య
dec_cRefThisDLL()
{
#నిర్వచితమైతే FEAT_GETTEXT
    ఒకవేళ (InterlockedDecrement((LPLONG)&g_cRefThisDll) == 0)
	dyn_gettext_free();
#లేకపోతే
    InterlockedDecrement((LPLONG)&g_cRefThisDll);
#అయితే_ముగింపు
}

//---------------------------------------------------------------------------
// DllCanUnloadNow
//---------------------------------------------------------------------------

STDAPI DllCanUnloadNow(శూన్య)
{
    తిరిగి (g_cRefThisDll == 0 ? S_OK : S_FALSE);
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppv_బహిర్గతం)
{
    *ppv_బహిర్గతం = ఖాళీ;

    ఒకవేళ (IsEqualIID(rclsid, CLSID_ShellExtension))
    {
	CShellExtClassFactory *pcf = కొత్త CShellExtClassFactory;

	తిరిగి pcf->QueryInterface(riid, ppv_బహిర్గతం);
    }

    తిరిగి CLASS_E_CLASSNOTAVAILABLE;
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
    *ppv = ఖాళీ;

    // any interface on this object is the object pointer

    ఒకవేళ (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	*ppv = (LPCLASSFACTORY)ఇది;

	కూడు_Ref();

	తిరిగి NOERROR;
    }

    తిరిగి E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::కూడు_Ref()
{
    తిరిగి InterlockedIncrement((LPLONG)&m_cRef);
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::Release()
{
    ఒకవేళ (InterlockedDecrement((LPLONG)&m_cRef))
	తిరిగి m_cRef;

    తొలగించు ఇది;

    తిరిగి 0L;
}

STDMETHODIMP CShellExtClassFactory::CreateInstance(LPUNKNOWN pUnkOuter,
						      REFIID riid,
						      LPVOID *ppvObj)
{
    *ppvObj = ఖాళీ;

    // Shell extensions typically don't support aggregation (inheritance)

    ఒకవేళ (pUnkOuter)
	తిరిగి CLASS_E_NOAGGREGATION;

    // Create the main shell extension object.  The shell will then call
    // QueryInterface with IID_IShellExtInit--this is how shell extensions are
    // initialized.

    LPCSHELLEXT pShellExt = కొత్త CShellExt();  // create the CShellExt object

    ఒకవేళ (ఖాళీ == pShellExt)
	తిరిగి E_OUTOFMEMORY;

    తిరిగి pShellExt->QueryInterface(riid, ppvObj);
}


STDMETHODIMP CShellExtClassFactory::తాళం_Server(BOOL  /* fLock */)
{
    తిరిగి NOERROR;
}

// *********************** CShellExt *************************
CShellExt::CShellExt()
{
    m_cRef = 0L;
    m_p_దత్తాంశం_Obj = ఖాళీ;

    inc_cRefThisDLL();

    ఎక్కించు_Menu_Icon();
}

CShellExt::~CShellExt()
{
    ఒకవేళ (m_p_దత్తాంశం_Obj)
	m_p_దత్తాంశం_Obj->Release();

    dec_cRefThisDLL();

    ఒకవేళ (m_hVimIconBitmap)
	DeleteObject(m_hVimIconBitmap);
}

STDMETHODIMP CShellExt::QueryInterface(REFIID riid, LPVOID FAR *ppv)
{
    *ppv = ఖాళీ;

    ఒకవేళ (IsEqualIID(riid, IID_IShellExtInit) || IsEqualIID(riid, IID_IUnknown))
    {
	*ppv = (LPSHELLEXTINIT)ఇది;
    }
    లేకపోతే ఒకవేళ (IsEqualIID(riid, IID_IContextMenu))
    {
	*ppv = (LPCONTEXTMENU)ఇది;
    }

    ఒకవేళ (*ppv)
    {
	కూడు_Ref();

	తిరిగి NOERROR;
    }

    తిరిగి E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExt::కూడు_Ref()
{
    తిరిగి InterlockedIncrement((LPLONG)&m_cRef);
}

STDMETHODIMP_(ULONG) CShellExt::Release()
{

    ఒకవేళ (InterlockedDecrement((LPLONG)&m_cRef))
	తిరిగి m_cRef;

    తొలగించు ఇది;

    తిరిగి 0L;
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
				   LPDATAOBJECT p_దత్తాంశం_Obj,
				   HKEY  /* hRegKey */)
{
    // Initialize can be called more than once
    ఒకవేళ (m_p_దత్తాంశం_Obj)
	m_p_దత్తాంశం_Obj->Release();

    // duplicate the object pointer and registry handle

    ఒకవేళ (p_దత్తాంశం_Obj)
    {
	m_p_దత్తాంశం_Obj = p_దత్తాంశం_Obj;
	p_దత్తాంశం_Obj->కూడు_Ref();
    }

    తిరిగి NOERROR;
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
					 UINT సూచీ_Menu,
					 UINT id_Cmd_మొదటి,
					 UINT  /* idCmdLast */,
					 UINT  /* uFlags */)
{
    UINT idCmd = id_Cmd_మొదటి;

    hres = m_p_దత్తాంశం_Obj->Get_దత్తాంశం(&fmte, &medium);
    ఒకవేళ (medium.hGlobal)
	cbFiles = DragQueryFileW((HDROP)medium.hGlobal, (UINT)-1, 0, 0);

    // InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

    // Initialize m_cntOfHWnd to 0
    m_cnt_యొక్క_HWnd = 0;

    HKEY keyhandle;
    తార్కిక showExisting = నిజం;
    తార్కిక showIcons = నిజం;

    // Check whether "Edit with existing Vim" entries are disabled.
    ఒకవేళ (Reg_Open_కీలకం_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
				       కీలకం_READ, &keyhandle) == ERROR_SUCCESS)
    {
	ఒకవేళ (Reg_Query_విలువ_Ex(keyhandle, "DisableEditWithExisting", 0, ఖాళీ,
						 ఖాళీ, ఖాళీ) == ERROR_SUCCESS)
	    showExisting = అబద్ధం;
	ఒకవేళ (Reg_Query_విలువ_Ex(keyhandle, "DisableContextMenuIcons", 0, ఖాళీ,
						 ఖాళీ, ఖాళీ) == ERROR_SUCCESS)
	    showIcons = అబద్ధం;
	Reg_Close_కీలకం(keyhandle);
    }

    // Use UTF-8 for gettext.
    అక్షరం *prev = సమితి_gettext_codeset();

    // Retrieve all the vim instances, unless disabled.
    ఒకవేళ (showExisting)
	EnumWindows(EnumWindowsProc, (LPARAM)ఇది);

    MENUITEMINFOW mii = { పరిమాణం(MENUITEMINFOW) };
    mii.f_ముసుగు = MIIM_STRING | MIIM_ID;
    ఒకవేళ (showIcons)
    {
	mii.f_ముసుగు |= MIIM_BITMAP;
	mii.hbmp_అంశం = m_hVimIconBitmap;
    }

    ఒకవేళ (cbFiles > 1)
    {
	mii.wID = idCmd++;
	mii.dw_Type_దత్తాంశం = W(_("Edit with Vim using &tabpages"));
	mii.cch = wcslen(mii.dw_Type_దత్తాంశం);
	చొప్పించు_Menu_అంశం_W(hMenu, సూచీ_Menu++, TRUE, &mii);
	విడుదల(mii.dw_Type_దత్తాంశం);

	mii.wID = idCmd++;
	mii.dw_Type_దత్తాంశం = W(_("Edit with single &Vim"));
	mii.cch = wcslen(mii.dw_Type_దత్తాంశం);
	చొప్పించు_Menu_అంశం_W(hMenu, సూచీ_Menu++, TRUE, &mii);
	విడుదల(mii.dw_Type_దత్తాంశం);

	ఒకవేళ (cbFiles <= 4)
	{
	    // Can edit up to 4 files in diff mode
	    mii.wID = idCmd++;
	    mii.dw_Type_దత్తాంశం = W(_("Diff with Vim"));
	    mii.cch = wcslen(mii.dw_Type_దత్తాంశం);
	    చొప్పించు_Menu_అంశం_W(hMenu, సూచీ_Menu++, TRUE, &mii);
	    విడుదల(mii.dw_Type_దత్తాంశం);
	    m_edit_existing_off = 3;
	}
	లేకపోతే
	    m_edit_existing_off = 2;

    }
    లేకపోతే
    {
	mii.wID = idCmd++;
	mii.dw_Type_దత్తాంశం = W(_("Edit with &Vim"));
	mii.cch = wcslen(mii.dw_Type_దత్తాంశం);
	చొప్పించు_Menu_అంశం_W(hMenu, సూచీ_Menu++, TRUE, &mii);
	విడుదల(mii.dw_Type_దత్తాంశం);
	m_edit_existing_off = 1;
    }

    HMENU hSubMenu = ఖాళీ;
    ఒకవేళ (m_cnt_యొక్క_HWnd > 1)
    {
	hSubMenu = CreatePopupMenu();
	mii.f_ముసుగు |= MIIM_SUBMENU;
	mii.wID = idCmd;
	mii.dw_Type_దత్తాంశం = W(_("Edit with existing Vim"));
	mii.cch = wcslen(mii.dw_Type_దత్తాంశం);
	mii.hSubMenu = hSubMenu;
	చొప్పించు_Menu_అంశం_W(hMenu, సూచీ_Menu++, TRUE, &mii);
	విడుదల(mii.dw_Type_దత్తాంశం);
	mii.f_ముసుగు = mii.f_ముసుగు & ~MIIM_SUBMENU;
	mii.hSubMenu = ఖాళీ;
    }
    // Now display all the vim instances
    కొరకు (పూర్ణాంకం i = 0; i < m_cnt_యొక్క_HWnd; i++)
    {
	WCHAR title[BUFSIZE];
	WCHAR తాత్కాలిక[BUFSIZE];
	పూర్ణాంకం సూచీ;
	HMENU hmenu;

	// Obtain window title, continue if can not
	ఒకవేళ (Get_కిటికీ_పాఠ్యం_W(m_hWnd[i], title, BUFSIZE - 1) == 0)
	    కొనసాగు;
	// Truncate the title before the path, keep the file name
	WCHAR *pos = wcschr(title, L'(');
	ఒకవేళ (pos != ఖాళీ)
	{
	    ఒకవేళ (pos > title && pos[-1] == L' ')
		--pos;
	    *pos = 0;
	}
	// Now concatenate
	ఒకవేళ (m_cnt_యొక్క_HWnd > 1)
	    తాత్కాలిక[0] = L'\0';
	లేకపోతే
	{
	    WCHAR *s = W(_("Edit with existing Vim - "));
	    wcsncpy(తాత్కాలిక, s, BUFSIZE - 1);
	    తాత్కాలిక[BUFSIZE - 1] = L'\0';
	    విడుదల(s);
	}
	wcsncat(తాత్కాలిక, title, BUFSIZE - 1 - wcslen(తాత్కాలిక));
	తాత్కాలిక[BUFSIZE - 1] = L'\0';

	mii.wID = idCmd++;
	mii.dw_Type_దత్తాంశం = తాత్కాలిక;
	mii.cch = wcslen(mii.dw_Type_దత్తాంశం);
	ఒకవేళ (m_cnt_యొక్క_HWnd > 1)
	{
	    hmenu = hSubMenu;
	    సూచీ = i;
	}
	లేకపోతే
	{
	    hmenu = hMenu;
	    సూచీ = సూచీ_Menu++;
	}
	చొప్పించు_Menu_అంశం_W(hmenu, సూచీ, TRUE, &mii);
    }
    // InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

    // Restore previous codeset.
    restore_gettext_codeset(prev);

    // Must return number of menu items we added.
    తిరిగి ఫలితం_From_Short(idCmd-id_Cmd_మొదటి);
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
    పూర్ణాంకం gvimExtraOptions;

    // If HIWORD(lpcmi->lpVerb) then we have been called programmatically
    // and lpVerb is a command that should be invoked.  Otherwise, the shell
    // has called us, and LOWORD(lpcmi->lpVerb) is the menu ID the user has
    // selected.  Actually, it's (menu ID - idCmdFirst) from QueryContextMenu().
    ఒకవేళ (!HIWORD(lpcmi->lpVerb))
    {
	UINT idCmd = LOWORD(lpcmi->lpVerb);

	ఒకవేళ (idCmd >= m_edit_existing_off)
	{
	    // Existing with vim instance
	    hr = నెట్టు_గమ్యం_కిటికీ(lpcmi->hwnd,
		    lpcmi->lpDirectory,
		    lpcmi->lpVerb,
		    lpcmi->lpParameters,
		    lpcmi->nShow,
		    idCmd - m_edit_existing_off);
	}
	లేకపోతే
	{
	    ఎంపిక (idCmd)
	    {
		స్థితి 0:
		    gvimExtraOptions = EDIT_WITH_VIM_USE_TABPAGES;
		    ఆపు;
		స్థితి 1:
		    gvimExtraOptions = EDIT_WITH_VIM_NO_OPTIONS;
		    ఆపు;
		స్థితి 2:
		    gvimExtraOptions = EDIT_WITH_VIM_లోపల_DIFF_బహుళకం;
		    ఆపు;
		అప్రమేయ:
		    // If execution reaches this point we likely have an
		    // inconsistency between the code that setup the menus
		    // and this code that determines what the user
		    // selected.  This should be detected and fixed during
		    // development.
		    తిరిగి E_FAIL;
	    }

            LPCMINVOKECOMMANDINFOEX lpcmiex = (LPCMINVOKECOMMANDINFOEX)lpcmi;
            LPCWSTR currentDirectory = lpcmi->cb_కొలత == పరిమాణం(CMINVOKECOMMANDINFOEX) ? lpcmiex->lpDirectoryW : ఖాళీ;

	    hr = InvokeSingleGvim(lpcmi->hwnd,
		    currentDirectory,
		    lpcmi->lpVerb,
		    lpcmi->lpParameters,
		    lpcmi->nShow,
		    gvimExtraOptions);
	}
    }
    తిరిగి hr;
}

STDMETHODIMP CShellExt::నెట్టు_గమ్యం_కిటికీ(HWND  /* hParent */,
				   LPCSTR  /* pszWorkingDir */,
				   LPCSTR  /* pszCmd */,
				   LPCSTR  /* pszParam */,
				   పూర్ణాంకం  /* iShowCmd */,
				   పూర్ణాంకం idHWnd)
{
    HWND hWnd = m_hWnd[idHWnd];

    // Show and bring vim instance to foreground
    ఒకవేళ (IsIconic(hWnd) != 0)
	Show_కిటికీ(hWnd, SW_RESTORE);
    లేకపోతే
	Show_కిటికీ(hWnd, SW_SHOW);
    సమితి_Foreground_కిటికీ(hWnd);

    // Post the selected files to the vim instance
    Post_సందేశం(hWnd, WM_DROPFILES, (WPARAM)medium.hGlobal, 0);

    తిరిగి NOERROR;
}

STDMETHODIMP CShellExt::GetCommandString(UINT_PTR  /* idCmd */,
					 UINT uFlags,
					 UINT FAR * /* reserved */,
					 LPSTR pszName,
					 UINT cchMax)
{
    // Use UTF-8 for gettext.
    అక్షరం *prev = సమితి_gettext_codeset();

    WCHAR *s = W(_("Edits the selected file(s) with Vim"));
    ఒకవేళ (uFlags == GCS_HELPTEXTW && cchMax > wcslen(s))
	wcscpy((WCHAR *)pszName, s);
    విడుదల(s);

    // Restore previous codeset.
    restore_gettext_codeset(prev);

    తిరిగి NOERROR;
}

BOOL CALLBACK CShellExt::EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    అక్షరం తాత్కాలిక[BUFSIZE];

    // First do a bunch of check
    // No invisible window
    ఒకవేళ (!Is_కిటికీ_Visible(hWnd))
	తిరిగి TRUE;
    // No child window ???
    // if (GetParent(hWnd)) return TRUE;
    // Class name should be Vim, if failed to get class name, return
    ఒకవేళ (GetClassName(hWnd, తాత్కాలిక, ARRAY_నిడివి(తాత్కాలిక)) == 0)
	తిరిగి TRUE;
    // Compare class name to that of vim, if not, return
    ఒకవేళ (_strnicmp(తాత్కాలిక, "vim", ARRAY_నిడివి("vim")) != 0)
	తిరిగి TRUE;
    // First check if the number of vim instance exceeds MAX_HWND
    CShellExt *cs = (CShellExt*) lParam;
    ఒకవేళ (cs->m_cnt_యొక్క_HWnd >= MAX_HWND)
	తిరిగి FALSE;	// stop enumeration
    // Now we get the vim window, put it into some kind of array
    cs->m_hWnd[cs->m_cnt_యొక్క_HWnd] = hWnd;
    cs->m_cnt_యొక్క_HWnd ++;

    తిరిగి TRUE; // continue enumeration (otherwise this would be false)
}

BOOL CShellExt::ఎక్కించు_Menu_Icon()
{
    అక్షరం vimExeFile[BUFSIZE];
    getGvimName(vimExeFile, TRUE);
    ఒకవేళ (vimExeFile[0] == '\0')
	తిరిగి FALSE;
    HICON hVimIcon;
    ఒకవేళ (వెలికితీయి_Icon_Ex(vimExeFile, 0, ఖాళీ, &hVimIcon, 1) == 0)
	తిరిగి FALSE;
    m_hVimIconBitmap = Icon_గమ్యం_Bitmap(hVimIcon,
	    GetSysColorBrush(COLOR_MENU),
	    GetSystemMetrics(SM_CXSMICON),
	    GetSystemMetrics(SM_CYSMICON));
    తిరిగి TRUE;
}

    స్థిర అక్షరం *
searchpath(అక్షరం *name)
{
    స్థిర సంఘం {
	అక్షరం	location[2 * BUFSIZE];
	WCHAR	wname[BUFSIZE];
    };
    WCHAR wlocation[BUFSIZE + 1];

    // There appears to be a bug in FindExecutableA() on Windows NT.
    // Use FindExecutableW() instead...
    Multi_Byte_గమ్యం_Wide_Char(CP_ACP, 0, name, -1, wname, BUFSIZE);
    ఒకవేళ (కనుగొను_Executable_W(wname, L"", wlocation) > (HINSTANCE)32)
    {
	Wide_Char_గమ్యం_Multi_Byte(CP_ACP, 0, wlocation, -1,
		location, పరిమాణం(location), ఖాళీ, ఖాళీ);
	తిరిగి location;
    }
    తిరిగి (అక్షరం *)"";
}


STDMETHODIMP CShellExt::InvokeSingleGvim(HWND h_మాతృ,
				   LPCWSTR  workingDir,
				   LPCSTR  /* pszCmd */,
				   LPCSTR  /* pszParam */,
				   పూర్ణాంకం  /* iShowCmd */,
				   పూర్ణాంకం gvimExtraOptions)
{
    wchar_t	m_szFileUserClickedOn[BUFSIZE];
    wchar_t	*cmdStrW;
    size_t	cmdlen;
    size_t	len;
    UINT i;

    cmdlen = BUFSIZE;
    cmdStrW  = (wchar_t *) జ్ఞాపకం_ఇవ్వు(cmdlen * పరిమాణం(wchar_t));
    ఒకవేళ (cmdStrW == ఖాళీ)
	తిరిగి E_FAIL;
    getGvimInvocationW(cmdStrW);

    ఒకవేళ (gvimExtraOptions == EDIT_WITH_VIM_లోపల_DIFF_బహుళకం)
	wcscat(cmdStrW, L" -d");
    లేకపోతే ఒకవేళ (gvimExtraOptions == EDIT_WITH_VIM_USE_TABPAGES)
	wcscat(cmdStrW, L" -p");
    కొరకు (i = 0; i < cbFiles; i++)
    {
	DragQueryFileW((HDROP)medium.hGlobal,
		i,
		m_szFileUserClickedOn,
		ARRAY_నిడివి(m_szFileUserClickedOn));

	len = wcslen(cmdStrW) + wcslen(m_szFileUserClickedOn) + 4;
	ఒకవేళ (len > cmdlen)
	{
	    cmdlen = len + BUFSIZE;
	    wchar_t *cmdStrW_new = (wchar_t *)realloc(cmdStrW, cmdlen * పరిమాణం(wchar_t));
	    ఒకవేళ (cmdStrW_new == ఖాళీ)
	    {
		విడుదల(cmdStrW);
		తిరిగి E_FAIL;
	    }
	    cmdStrW = cmdStrW_new;
	}
	wcscat(cmdStrW, L" \"");
	wcscat(cmdStrW, m_szFileUserClickedOn);
	wcscat(cmdStrW, L"\"");
    }

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    Zero_జ్ఞాపకం(&si, పరిమాణం(si));
    si.cb = పరిమాణం(si);

    // Start the child process.
    ఒకవేళ (!CreateProcessW(ఖాళీ,	// No module name (use command line).
		cmdStrW,	// Command line.
		ఖాళీ,		// Process handle not inheritable.
		ఖాళీ,		// Thread handle not inheritable.
		FALSE,		// Set handle inheritance to FALSE.
		0,		// No creation flags.
		ఖాళీ,		// Use parent's environment block.
		workingDir,	// Use parent's starting directory.
		&si,		// Pointer to STARTUPINFO structure.
		&pi)		// Pointer to PROCESS_INFORMATION structure.
       )
    {
	// Use UTF-8 for gettext.
	అక్షరం *prev = సమితి_gettext_codeset();

	WCHAR *సందేశం = W(_("Error creating process: Check if gvim is in your path!"));
	WCHAR *title = W(_("gvimext.dll error"));

	సందేశం_Box_W(h_మాతృ, సందేశం, title, MB_OK);

	విడుదల(సందేశం);
	విడుదల(title);

	// Restore previous codeset.
	restore_gettext_codeset(prev);
    }
    లేకపోతే
    {
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
    }
    విడుదల(cmdStrW);

    తిరిగి NOERROR;
}
