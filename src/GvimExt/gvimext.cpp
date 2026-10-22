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

#ਅਨ੍ਤਰ੍ਭਾਵ "gvimext.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "../version.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ARRAY_ਲੰਮਾਈ(a) (ਆਕਾਰ(a) / ਆਕਾਰ((a)[0]))

ਸਥਿਰ ਅੱਖਰ *searchpath(ਅੱਖਰ *name);

// Always get an error while putting the following stuff to the
// gvimext.h file as class protected variables, give up and
// declare them as global stuff
FORMATETC fmte = {CF_HDROP,
		  (DVTARGETDEVICE FAR *)ਨਿਰਾ,
		  DVASPECT_CONTENT,
		  -1,
		  TYMED_HGLOBAL
		 };
STGMEDIUM medium;
HRESULT hres = 0;
UINT cbFiles = 0;

/* The buffers size used to be MAX_PATH (260 bytes), but that's not always
 * enough */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BUFSIZE 1100

// The "Edit with Vim" shell extension provides these choices when
// a new instance of Gvim is selected:
//   - use tabpages
//   - enable diff mode
//   - none of the above
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EDIT_WITH_VIM_USE_TABPAGES (2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EDIT_WITH_VIM_ਵਿੱਚ_DIFF_ਬਹੁਲਕ (1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EDIT_WITH_VIM_NO_OPTIONS   (0)

//
// Get the name of the Gvim executable to use, with the path.
// When "runtime" is non-zero, we were called to find the runtime directory.
// Returns the path in name[BUFSIZE].  It's empty when it fails.
//
    ਸਥਿਰ ਖਾਲੀ
getGvimName(ਅੱਖਰ *name, BOOL runtime)
{
    HKEY	keyhandle;
    DWORD	hlen;

    // Get the location of gvim from the registry.  Try
    // HKEY_CURRENT_USER first, then fall back to HKEY_LOCAL_MACHINE if
    // not found.
    name[0] = 0;
    ਜੇ (Reg_Open_ਕੁੰਜੀ_Ex(HKEY_CURRENT_USER, "Software\\Vim\\Gvim", 0,
				       ਕੁੰਜੀ_READ, &keyhandle) == ERROR_SUCCESS)
    {
	hlen = BUFSIZE;
	ਜੇ (Reg_Query_ਮੁੱਲ_Ex(keyhandle, "path", 0, ਨਿਰਾ, (BYTE *)name, &hlen)
							     != ERROR_SUCCESS)
	    name[0] = 0;
	ਨਹੀਂਤਾਂ
	    name[hlen] = 0;
	Reg_Close_ਕੁੰਜੀ(keyhandle);
    }

    ਜੇ ((name[0] == 0) &&
	    (Reg_Open_ਕੁੰਜੀ_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
				       ਕੁੰਜੀ_READ, &keyhandle) == ERROR_SUCCESS))
    {
	hlen = BUFSIZE;
	ਜੇ (Reg_Query_ਮੁੱਲ_Ex(keyhandle, "path", 0, ਨਿਰਾ, (BYTE *)name, &hlen)
							     != ERROR_SUCCESS)
	    name[0] = 0;
	ਨਹੀਂਤਾਂ
	    name[hlen] = 0;
	Reg_Close_ਕੁੰਜੀ(keyhandle);
    }

    // Registry didn't work, use the search path.
    ਜੇ (name[0] == 0)
	strcpy(name, searchpath((ਅੱਖਰ *)"gvim.exe"));

    ਜੇ (!runtime)
    {
	// Only when looking for the executable, not the runtime dir, we can
	// search for the batch file or a name without a path.
	ਜੇ (name[0] == 0)
	    strcpy(name, searchpath((ਅੱਖਰ *)"gvim.bat"));
	ਜੇ (name[0] == 0)
	    strcpy(name, "gvim");	// finds gvim.bat or gvim.exe
    }
}

    ਸਥਿਰ ਖਾਲੀ
getGvimInvocation(ਅੱਖਰ *name)
{
    getGvimName(name, FALSE);
    // avoid that Vim tries to expand wildcards in the file names
    strcat(name, " --literal");
}

    ਸਥਿਰ ਖਾਲੀ
getGvimInvocationW(wchar_t *nameW)
{
    ਅੱਖਰ *name;

    name = (ਅੱਖਰ *)ਸਮ੍ਰਿਤੀ_ਦਿਓ(BUFSIZE);
    getGvimInvocation(name);
    mbstowcs(nameW, name, BUFSIZE);
    ਮੁਕਤ(name);
}

    ਸਥਿਰ BOOL
FileExists(LPCTSTR sz_ਰਸਤਾ)
{
    DWORD dwAttrib = Get_File_ਗੁਣ(sz_ਰਸਤਾ);

    ਵਾਪਸ (dwAttrib != INVALID_FILE_ਗੁਣ
	    && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

//
// Get the Vim runtime directory into buf[BUFSIZE].
// The result is empty when it failed.
// When it works, the path ends in a slash or backslash.
//
    ਸਥਿਰ ਖਾਲੀ
getRuntimeDir(ਅੱਖਰ *buf)
{
    ਪੂਰਨ		idx;

    getGvimName(buf, TRUE);
    ਜੇ (buf[0] == 0)
	ਵਾਪਸ;

    // When no path found, use the search path to expand it.
    ਜੇ (strchr(buf, '/') == ਨਿਰਾ && strchr(buf, '\\') == ਨਿਰਾ)
	strcpy(buf, searchpath(buf));

    // remove "gvim.exe" from the end
    ਲਈ (idx = (ਪੂਰਨ)ਲੜੀ_ਲੰਬਾਈ(buf) - 1; idx >= 0; idx--)
	ਜੇ (buf[idx] == '\\' || buf[idx] == '/')
	{
	    buf[++idx] = 0;
	    ਰੋਕੋ;
	}

    // Check the existence of defaults.vim
    ਅੱਖਰ *p = buf + idx;
    strcpy(p, "defaults.vim");
    ਜੇ (FileExists(buf))
    {
	*p = 0;		// Cut "defaults.vim"
	ਵਾਪਸ;
    }
    // Check the existence of vimXX\defaults.vim
    strcpy(p, VIM_ਸੰਸਕਰਣ_NODOT "\\" "defaults.vim");
    ਜੇ (FileExists(buf))
    {
	p[ARRAY_ਲੰਮਾਈ(VIM_ਸੰਸਕਰਣ_NODOT)] = 0;	// Cut "defaults.vim"
	ਵਾਪਸ;
    }
    // Check the existence of runtime\defaults.vim
    strcpy(p, "runtime" "\\" "defaults.vim");
    ਜੇ (FileExists(buf))
    {
	p[ARRAY_ਲੰਮਾਈ("runtime")] = 0;		// Cut "defaults.vim"
	ਵਾਪਸ;
    }
    // Not found
    buf[0] = 0;
    ਵਾਪਸ;
}

    WCHAR *
utf8_ਮੰਜ਼ਿਲ_utf16(ਅਚਲ ਅੱਖਰ *s)
{
    ਪੂਰਨ ਮਾਪ = Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char(CP_UTF8, 0, s, -1, ਨਿਰਾ, 0);
    WCHAR *buf = (WCHAR *)ਸਮ੍ਰਿਤੀ_ਦਿਓ(ਮਾਪ * ਆਕਾਰ(WCHAR));
    Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char(CP_UTF8, 0, s, -1, buf, ਮਾਪ);
    ਵਾਪਸ buf;
}

    HBITMAP
Icon_ਮੰਜ਼ਿਲ_Bitmap(HICON hIcon, HBRUSH hBackground, ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ ਉਚਾਈ)
{
    HDC hDC = GetDC(ਨਿਰਾ);
    HDC hMemDC = CreateCompatibleDC(hDC);
    HBITMAP hMemBmp = CreateCompatibleBitmap(hDC, ਚੌੜਾਈ, ਉਚਾਈ);
    HBITMAP h_ਨਤੀਜਾ_Bmp = ਨਿਰਾ;
    HGDIOBJ hOrgBMP = SelectObject(hMemDC, hMemBmp);

    DrawIconEx(hMemDC, 0, 0, hIcon, ਚੌੜਾਈ, ਉਚਾਈ, 0, hBackground, DI_ਸਾਧਾਰਨ);

    h_ਨਤੀਜਾ_Bmp = hMemBmp;
    hMemBmp = ਨਿਰਾ;

    SelectObject(hMemDC, hOrgBMP);
    DeleteDC(hMemDC);
    ReleaseDC(ਨਿਰਾ, hDC);
    DestroyIcon(hIcon);
    ਵਾਪਸ h_ਨਤੀਜਾ_Bmp;
}

//
// GETTEXT: translated messages and menu entries
//
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_GETTEXT
# define _(x)	    x
# define W_impl(x) _wcsdup(L##x)
# define W(x)	    W_impl(x)
# define ਸੈੱਟ_gettext_codeset()	    ਨਿਰਾ
# define restore_gettext_codeset(x)
#ਨਹੀਂਤਾਂ
# define _(x)	    (*dyn_libintl_gettext)(x)
# define W(x)	    utf8_ਮੰਜ਼ਿਲ_utf16(x)
# define VIMPACKAGE "vim"
# ifndef GETTEXT_DLL
#  define GETTEXT_DLL "libintl.dll"
#  define GETTEXT_DLL_ALT "libintl-8.dll"
# endif

// Dummy functions
ਸਥਿਰ ਅੱਖਰ *null_libintl_gettext(ਅਚਲ ਅੱਖਰ *);
ਸਥਿਰ ਅੱਖਰ *null_libintl_textdomain(ਅਚਲ ਅੱਖਰ *);
ਸਥਿਰ ਅੱਖਰ *null_libintl_bindtextdomain(ਅਚਲ ਅੱਖਰ *, ਅਚਲ ਅੱਖਰ *);
ਸਥਿਰ ਅੱਖਰ *null_libintl_bind_textdomain_codeset(ਅਚਲ ਅੱਖਰ *, ਅਚਲ ਅੱਖਰ *);
ਸਥਿਰ ਪੂਰਨ dyn_libintl_init(ਅੱਖਰ *dir);
ਸਥਿਰ ਖਾਲੀ dyn_libintl_ਸਿਰਾ(ਖਾਲੀ);

ਸਥਿਰ HINSTANCE hLibintlDLL = 0;
ਸਥਿਰ ਅੱਖਰ *(*dyn_libintl_gettext)(ਅਚਲ ਅੱਖਰ *) = null_libintl_gettext;
ਸਥਿਰ ਅੱਖਰ *(*dyn_libintl_textdomain)(ਅਚਲ ਅੱਖਰ *) = null_libintl_textdomain;
ਸਥਿਰ ਅੱਖਰ *(*dyn_libintl_bindtextdomain)(ਅਚਲ ਅੱਖਰ *, ਅਚਲ ਅੱਖਰ *)
						= null_libintl_bindtextdomain;
ਸਥਿਰ ਅੱਖਰ *(*dyn_libintl_bind_textdomain_codeset)(ਅਚਲ ਅੱਖਰ *, ਅਚਲ ਅੱਖਰ *)
				       = null_libintl_bind_textdomain_codeset;

//
// Attempt to load libintl.dll.  If it doesn't work, use dummy functions.
// "dir" is the directory where the libintl.dll might be.
// Return 1 for success, 0 for failure.
//
    ਸਥਿਰ ਪੂਰਨ
dyn_libintl_init(ਅੱਖਰ *dir)
{
    ਪੂਰਨ		i;
    ਸਥਿਰ ਬਣਤਰ
    {
	ਅੱਖਰ	    *name;
	FARPROC	    *ptr;
    } libintl_ਇੰਦਰਾਜ[] =
    {
	{(ਅੱਖਰ *)"gettext",		(FARPROC*)&dyn_libintl_gettext},
	{(ਅੱਖਰ *)"textdomain",		(FARPROC*)&dyn_libintl_textdomain},
	{(ਅੱਖਰ *)"bindtextdomain",	(FARPROC*)&dyn_libintl_bindtextdomain},
	{(ਅੱਖਰ *)"bind_textdomain_codeset", (FARPROC*)&dyn_libintl_bind_textdomain_codeset},
	{ਨਿਰਾ, ਨਿਰਾ}
    };
    DWORD	len, len2;
    LPWSTR	buf = ਨਿਰਾ;
    LPWSTR	buf2 = ਨਿਰਾ;

    // No need to initialize twice.
    ਜੇ (hLibintlDLL)
	ਵਾਪਸ 1;

    // Load gettext library from $VIMRUNTIME\GvimExt{64,32} directory.
    // Add the directory to $PATH temporarily.
    len = GetEnvironmentVariableW(L"PATH", ਨਿਰਾ, 0);
    len2 = MAX_ਰਸਤਾ + 1 + len;
    buf = (LPWSTR)ਸਮ੍ਰਿਤੀ_ਦਿਓ(len * ਆਕਾਰ(WCHAR));
    buf2 = (LPWSTR)ਸਮ੍ਰਿਤੀ_ਦਿਓ(len2 * ਆਕਾਰ(WCHAR));
    ਜੇ (buf != ਨਿਰਾ && buf2 != ਨਿਰਾ)
    {
	GetEnvironmentVariableW(L"PATH", buf, len);
# ifdef _WIN64
	_snwprintf(buf2, len2, L"%S\\GvimExt64;%s", dir, buf);
# ਨਹੀਂਤਾਂ
	_snwprintf(buf2, len2, L"%S\\GvimExt32;%s", dir, buf);
# endif
	ਸੈੱਟ_Environment_Variable_W(L"PATH", buf2);
	hLibintlDLL = ਲੱਦੋ_Library(GETTEXT_DLL);
# ifdef GETTEXT_DLL_ALT
	ਜੇ (!hLibintlDLL)
	    hLibintlDLL = ਲੱਦੋ_Library(GETTEXT_DLL_ALT);
# endif
	ਸੈੱਟ_Environment_Variable_W(L"PATH", buf);
    }
    ਮੁਕਤ(buf);
    ਮੁਕਤ(buf2);
    ਜੇ (!hLibintlDLL)
	ਵਾਪਸ 0;

    // Get the addresses of the functions we need.
    ਲਈ (i = 0; libintl_ਇੰਦਰਾਜ[i].name != ਨਿਰਾ
					 && libintl_ਇੰਦਰਾਜ[i].ptr != ਨਿਰਾ; ++i)
    {
	ਜੇ ((*libintl_ਇੰਦਰਾਜ[i].ptr = Get_Proc_ਪਤਾ(hLibintlDLL,
					      libintl_ਇੰਦਰਾਜ[i].name)) == ਨਿਰਾ)
	{
	    dyn_libintl_ਸਿਰਾ();
	    ਵਾਪਸ 0;
	}
    }
    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਖਾਲੀ
dyn_libintl_ਸਿਰਾ(ਖਾਲੀ)
{
    ਜੇ (hLibintlDLL)
	FreeLibrary(hLibintlDLL);
    hLibintlDLL			= ਨਿਰਾ;
    dyn_libintl_gettext		= null_libintl_gettext;
    dyn_libintl_textdomain	= null_libintl_textdomain;
    dyn_libintl_bindtextdomain	= null_libintl_bindtextdomain;
    dyn_libintl_bind_textdomain_codeset	= null_libintl_bind_textdomain_codeset;
}

    ਸਥਿਰ ਅੱਖਰ *
null_libintl_gettext(ਅਚਲ ਅੱਖਰ *msgid)
{
    ਵਾਪਸ (ਅੱਖਰ *)msgid;
}

    ਸਥਿਰ ਅੱਖਰ *
null_libintl_textdomain(ਅਚਲ ਅੱਖਰ * /* domainname */)
{
    ਵਾਪਸ ਨਿਰਾ;
}

    ਸਥਿਰ ਅੱਖਰ *
null_libintl_bindtextdomain(ਅਚਲ ਅੱਖਰ * /* domainname */, ਅਚਲ ਅੱਖਰ * /* dirname */)
{
    ਵਾਪਸ ਨਿਰਾ;
}

    ਸਥਿਰ ਅੱਖਰ *
null_libintl_bind_textdomain_codeset(ਅਚਲ ਅੱਖਰ * /* domainname */, ਅਚਲ ਅੱਖਰ * /* codeset */)
{
    ਵਾਪਸ ਨਿਰਾ;
}

//
// Setup for translating strings.
//
    ਸਥਿਰ ਖਾਲੀ
dyn_gettext_ਲੱਦੋ(ਖਾਲੀ)
{
    ਅੱਖਰ    szBuff[BUFSIZE];
    DWORD   len;

    // Try to locate the runtime files.  The path is used to find libintl.dll
    // and the vim.mo files.
    getRuntimeDir(szBuff);
    ਜੇ (szBuff[0] != 0)
    {
	len = (DWORD)ਲੜੀ_ਲੰਬਾਈ(szBuff);
	ਜੇ (dyn_libintl_init(szBuff))
	{
	    strcpy(szBuff + len, "lang");

	    (*dyn_libintl_bindtextdomain)(VIMPACKAGE, szBuff);
	    (*dyn_libintl_textdomain)(VIMPACKAGE);
	}
    }
}

    ਸਥਿਰ ਖਾਲੀ
dyn_gettext_free(ਖਾਲੀ)
{
    dyn_libintl_ਸਿਰਾ();
}

//
// Use UTF-8 for gettext. Returns previous codeset.
//
    ਸਥਿਰ ਅੱਖਰ *
ਸੈੱਟ_gettext_codeset(ਖਾਲੀ)
{
    ਅੱਖਰ *prev = dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, ਨਿਰਾ);
    prev = _strdup((prev != ਨਿਰਾ) ? prev : "char");
    dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, "utf-8");

    ਵਾਪਸ prev;
}

//
// Restore previous codeset for gettext.
//
    ਸਥਿਰ ਖਾਲੀ
restore_gettext_codeset(ਅੱਖਰ *prev)
{
    dyn_libintl_bind_textdomain_codeset(VIMPACKAGE, prev);
    ਮੁਕਤ(prev);
}
#ਜੇ_ਅੰਤ // FEAT_GETTEXT

//
// Global variables
//
UINT      g_cRefThisDll = 0;    // Reference count of this DLL.
HINSTANCE g_hmodThisDll = ਨਿਰਾ;	// Handle to this DLL itself.


//---------------------------------------------------------------------------
// DllMain
//---------------------------------------------------------------------------
ਬਾਹਰੀ "C" ਪੂਰਨ APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID  /* lpReserved */)
{
    ਚੋਣ (dwReason)
    {
    ਹਾਲਤ DLL_PROCESS_ATTACH:
	// Extension DLL one-time initialization
	g_hmodThisDll = hInstance;
	ਰੋਕੋ;

    ਹਾਲਤ DLL_PROCESS_DETACH:
	ਰੋਕੋ;
    }

    ਵਾਪਸ 1;   // ok
}

    ਸਥਿਰ ਖਾਲੀ
inc_cRefThisDLL()
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GETTEXT
    ਜੇ (g_cRefThisDll == 0)
	dyn_gettext_ਲੱਦੋ();
#ਜੇ_ਅੰਤ
    InterlockedIncrement((LPLONG)&g_cRefThisDll);
}

    ਸਥਿਰ ਖਾਲੀ
dec_cRefThisDLL()
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GETTEXT
    ਜੇ (InterlockedDecrement((LPLONG)&g_cRefThisDll) == 0)
	dyn_gettext_free();
#ਨਹੀਂਤਾਂ
    InterlockedDecrement((LPLONG)&g_cRefThisDll);
#ਜੇ_ਅੰਤ
}

//---------------------------------------------------------------------------
// DllCanUnloadNow
//---------------------------------------------------------------------------

STDAPI DllCanUnloadNow(ਖਾਲੀ)
{
    ਵਾਪਸ (g_cRefThisDll == 0 ? S_OK : S_FALSE);
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppv_ਬਾਹਰਲਾ)
{
    *ppv_ਬਾਹਰਲਾ = ਨਿਰਾ;

    ਜੇ (IsEqualIID(rclsid, CLSID_ShellExtension))
    {
	CShellExtClassFactory *pcf = ਨਵਾਂ CShellExtClassFactory;

	ਵਾਪਸ pcf->QueryInterface(riid, ppv_ਬਾਹਰਲਾ);
    }

    ਵਾਪਸ CLASS_E_CLASSNOTAVAILABLE;
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
    *ppv = ਨਿਰਾ;

    // any interface on this object is the object pointer

    ਜੇ (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	*ppv = (LPCLASSFACTORY)ਇਹ;

	ਜੋੜੋ_Ref();

	ਵਾਪਸ NOERROR;
    }

    ਵਾਪਸ E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::ਜੋੜੋ_Ref()
{
    ਵਾਪਸ InterlockedIncrement((LPLONG)&m_cRef);
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::Release()
{
    ਜੇ (InterlockedDecrement((LPLONG)&m_cRef))
	ਵਾਪਸ m_cRef;

    ਮਿਟਾਓ ਇਹ;

    ਵਾਪਸ 0L;
}

STDMETHODIMP CShellExtClassFactory::CreateInstance(LPUNKNOWN pUnkOuter,
						      REFIID riid,
						      LPVOID *ppvObj)
{
    *ppvObj = ਨਿਰਾ;

    // Shell extensions typically don't support aggregation (inheritance)

    ਜੇ (pUnkOuter)
	ਵਾਪਸ CLASS_E_NOAGGREGATION;

    // Create the main shell extension object.  The shell will then call
    // QueryInterface with IID_IShellExtInit--this is how shell extensions are
    // initialized.

    LPCSHELLEXT pShellExt = ਨਵਾਂ CShellExt();  // create the CShellExt object

    ਜੇ (ਨਿਰਾ == pShellExt)
	ਵਾਪਸ E_OUTOFMEMORY;

    ਵਾਪਸ pShellExt->QueryInterface(riid, ppvObj);
}


STDMETHODIMP CShellExtClassFactory::ਤਾਲਾ_Server(BOOL  /* fLock */)
{
    ਵਾਪਸ NOERROR;
}

// *********************** CShellExt *************************
CShellExt::CShellExt()
{
    m_cRef = 0L;
    m_p_ਅੰਕੜੇ_Obj = ਨਿਰਾ;

    inc_cRefThisDLL();

    ਲੱਦੋ_Menu_Icon();
}

CShellExt::~CShellExt()
{
    ਜੇ (m_p_ਅੰਕੜੇ_Obj)
	m_p_ਅੰਕੜੇ_Obj->Release();

    dec_cRefThisDLL();

    ਜੇ (m_hVimIconBitmap)
	DeleteObject(m_hVimIconBitmap);
}

STDMETHODIMP CShellExt::QueryInterface(REFIID riid, LPVOID FAR *ppv)
{
    *ppv = ਨਿਰਾ;

    ਜੇ (IsEqualIID(riid, IID_IShellExtInit) || IsEqualIID(riid, IID_IUnknown))
    {
	*ppv = (LPSHELLEXTINIT)ਇਹ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (IsEqualIID(riid, IID_IContextMenu))
    {
	*ppv = (LPCONTEXTMENU)ਇਹ;
    }

    ਜੇ (*ppv)
    {
	ਜੋੜੋ_Ref();

	ਵਾਪਸ NOERROR;
    }

    ਵਾਪਸ E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExt::ਜੋੜੋ_Ref()
{
    ਵਾਪਸ InterlockedIncrement((LPLONG)&m_cRef);
}

STDMETHODIMP_(ULONG) CShellExt::Release()
{

    ਜੇ (InterlockedDecrement((LPLONG)&m_cRef))
	ਵਾਪਸ m_cRef;

    ਮਿਟਾਓ ਇਹ;

    ਵਾਪਸ 0L;
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
				   LPDATAOBJECT p_ਅੰਕੜੇ_Obj,
				   HKEY  /* hRegKey */)
{
    // Initialize can be called more than once
    ਜੇ (m_p_ਅੰਕੜੇ_Obj)
	m_p_ਅੰਕੜੇ_Obj->Release();

    // duplicate the object pointer and registry handle

    ਜੇ (p_ਅੰਕੜੇ_Obj)
    {
	m_p_ਅੰਕੜੇ_Obj = p_ਅੰਕੜੇ_Obj;
	p_ਅੰਕੜੇ_Obj->ਜੋੜੋ_Ref();
    }

    ਵਾਪਸ NOERROR;
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
					 UINT ਸੂਚਕਾਂਕ_Menu,
					 UINT id_Cmd_ਪਹਿਲਾ,
					 UINT  /* idCmdLast */,
					 UINT  /* uFlags */)
{
    UINT idCmd = id_Cmd_ਪਹਿਲਾ;

    hres = m_p_ਅੰਕੜੇ_Obj->Get_ਅੰਕੜੇ(&fmte, &medium);
    ਜੇ (medium.hGlobal)
	cbFiles = DragQueryFileW((HDROP)medium.hGlobal, (UINT)-1, 0, 0);

    // InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

    // Initialize m_cntOfHWnd to 0
    m_cnt_ਦਾ_HWnd = 0;

    HKEY keyhandle;
    ਤਾਰਕਿਕ showExisting = ਸੱਚ;
    ਤਾਰਕਿਕ showIcons = ਸੱਚ;

    // Check whether "Edit with existing Vim" entries are disabled.
    ਜੇ (Reg_Open_ਕੁੰਜੀ_Ex(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
				       ਕੁੰਜੀ_READ, &keyhandle) == ERROR_SUCCESS)
    {
	ਜੇ (Reg_Query_ਮੁੱਲ_Ex(keyhandle, "DisableEditWithExisting", 0, ਨਿਰਾ,
						 ਨਿਰਾ, ਨਿਰਾ) == ERROR_SUCCESS)
	    showExisting = ਝੂਠ;
	ਜੇ (Reg_Query_ਮੁੱਲ_Ex(keyhandle, "DisableContextMenuIcons", 0, ਨਿਰਾ,
						 ਨਿਰਾ, ਨਿਰਾ) == ERROR_SUCCESS)
	    showIcons = ਝੂਠ;
	Reg_Close_ਕੁੰਜੀ(keyhandle);
    }

    // Use UTF-8 for gettext.
    ਅੱਖਰ *prev = ਸੈੱਟ_gettext_codeset();

    // Retrieve all the vim instances, unless disabled.
    ਜੇ (showExisting)
	EnumWindows(EnumWindowsProc, (LPARAM)ਇਹ);

    MENUITEMINFOW mii = { ਆਕਾਰ(MENUITEMINFOW) };
    mii.f_ਨਕਾਬ = MIIM_STRING | MIIM_ID;
    ਜੇ (showIcons)
    {
	mii.f_ਨਕਾਬ |= MIIM_BITMAP;
	mii.hbmp_ਵਸਤੂ = m_hVimIconBitmap;
    }

    ਜੇ (cbFiles > 1)
    {
	mii.wID = idCmd++;
	mii.dw_Type_ਅੰਕੜੇ = W(_("Edit with Vim using &tabpages"));
	mii.cch = wcslen(mii.dw_Type_ਅੰਕੜੇ);
	ਪਾਓ_Menu_ਵਸਤੂ_W(hMenu, ਸੂਚਕਾਂਕ_Menu++, TRUE, &mii);
	ਮੁਕਤ(mii.dw_Type_ਅੰਕੜੇ);

	mii.wID = idCmd++;
	mii.dw_Type_ਅੰਕੜੇ = W(_("Edit with single &Vim"));
	mii.cch = wcslen(mii.dw_Type_ਅੰਕੜੇ);
	ਪਾਓ_Menu_ਵਸਤੂ_W(hMenu, ਸੂਚਕਾਂਕ_Menu++, TRUE, &mii);
	ਮੁਕਤ(mii.dw_Type_ਅੰਕੜੇ);

	ਜੇ (cbFiles <= 4)
	{
	    // Can edit up to 4 files in diff mode
	    mii.wID = idCmd++;
	    mii.dw_Type_ਅੰਕੜੇ = W(_("Diff with Vim"));
	    mii.cch = wcslen(mii.dw_Type_ਅੰਕੜੇ);
	    ਪਾਓ_Menu_ਵਸਤੂ_W(hMenu, ਸੂਚਕਾਂਕ_Menu++, TRUE, &mii);
	    ਮੁਕਤ(mii.dw_Type_ਅੰਕੜੇ);
	    m_edit_existing_off = 3;
	}
	ਨਹੀਂਤਾਂ
	    m_edit_existing_off = 2;

    }
    ਨਹੀਂਤਾਂ
    {
	mii.wID = idCmd++;
	mii.dw_Type_ਅੰਕੜੇ = W(_("Edit with &Vim"));
	mii.cch = wcslen(mii.dw_Type_ਅੰਕੜੇ);
	ਪਾਓ_Menu_ਵਸਤੂ_W(hMenu, ਸੂਚਕਾਂਕ_Menu++, TRUE, &mii);
	ਮੁਕਤ(mii.dw_Type_ਅੰਕੜੇ);
	m_edit_existing_off = 1;
    }

    HMENU hSubMenu = ਨਿਰਾ;
    ਜੇ (m_cnt_ਦਾ_HWnd > 1)
    {
	hSubMenu = CreatePopupMenu();
	mii.f_ਨਕਾਬ |= MIIM_SUBMENU;
	mii.wID = idCmd;
	mii.dw_Type_ਅੰਕੜੇ = W(_("Edit with existing Vim"));
	mii.cch = wcslen(mii.dw_Type_ਅੰਕੜੇ);
	mii.hSubMenu = hSubMenu;
	ਪਾਓ_Menu_ਵਸਤੂ_W(hMenu, ਸੂਚਕਾਂਕ_Menu++, TRUE, &mii);
	ਮੁਕਤ(mii.dw_Type_ਅੰਕੜੇ);
	mii.f_ਨਕਾਬ = mii.f_ਨਕਾਬ & ~MIIM_SUBMENU;
	mii.hSubMenu = ਨਿਰਾ;
    }
    // Now display all the vim instances
    ਲਈ (ਪੂਰਨ i = 0; i < m_cnt_ਦਾ_HWnd; i++)
    {
	WCHAR title[BUFSIZE];
	WCHAR ਆਰਜ਼ੀ[BUFSIZE];
	ਪੂਰਨ ਸੂਚਕਾਂਕ;
	HMENU hmenu;

	// Obtain window title, continue if can not
	ਜੇ (Get_ਖਿੜਕੀ_ਪਾਠ_W(m_hWnd[i], title, BUFSIZE - 1) == 0)
	    ਜਾਰੀ;
	// Truncate the title before the path, keep the file name
	WCHAR *pos = wcschr(title, L'(');
	ਜੇ (pos != ਨਿਰਾ)
	{
	    ਜੇ (pos > title && pos[-1] == L' ')
		--pos;
	    *pos = 0;
	}
	// Now concatenate
	ਜੇ (m_cnt_ਦਾ_HWnd > 1)
	    ਆਰਜ਼ੀ[0] = L'\0';
	ਨਹੀਂਤਾਂ
	{
	    WCHAR *s = W(_("Edit with existing Vim - "));
	    wcsncpy(ਆਰਜ਼ੀ, s, BUFSIZE - 1);
	    ਆਰਜ਼ੀ[BUFSIZE - 1] = L'\0';
	    ਮੁਕਤ(s);
	}
	wcsncat(ਆਰਜ਼ੀ, title, BUFSIZE - 1 - wcslen(ਆਰਜ਼ੀ));
	ਆਰਜ਼ੀ[BUFSIZE - 1] = L'\0';

	mii.wID = idCmd++;
	mii.dw_Type_ਅੰਕੜੇ = ਆਰਜ਼ੀ;
	mii.cch = wcslen(mii.dw_Type_ਅੰਕੜੇ);
	ਜੇ (m_cnt_ਦਾ_HWnd > 1)
	{
	    hmenu = hSubMenu;
	    ਸੂਚਕਾਂਕ = i;
	}
	ਨਹੀਂਤਾਂ
	{
	    hmenu = hMenu;
	    ਸੂਚਕਾਂਕ = ਸੂਚਕਾਂਕ_Menu++;
	}
	ਪਾਓ_Menu_ਵਸਤੂ_W(hmenu, ਸੂਚਕਾਂਕ, TRUE, &mii);
    }
    // InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

    // Restore previous codeset.
    restore_gettext_codeset(prev);

    // Must return number of menu items we added.
    ਵਾਪਸ ਨਤੀਜਾ_From_Short(idCmd-id_Cmd_ਪਹਿਲਾ);
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
    ਪੂਰਨ gvimExtraOptions;

    // If HIWORD(lpcmi->lpVerb) then we have been called programmatically
    // and lpVerb is a command that should be invoked.  Otherwise, the shell
    // has called us, and LOWORD(lpcmi->lpVerb) is the menu ID the user has
    // selected.  Actually, it's (menu ID - idCmdFirst) from QueryContextMenu().
    ਜੇ (!HIWORD(lpcmi->lpVerb))
    {
	UINT idCmd = LOWORD(lpcmi->lpVerb);

	ਜੇ (idCmd >= m_edit_existing_off)
	{
	    // Existing with vim instance
	    hr = ਧੱਕੋ_ਮੰਜ਼ਿਲ_ਖਿੜਕੀ(lpcmi->hwnd,
		    lpcmi->lpDirectory,
		    lpcmi->lpVerb,
		    lpcmi->lpParameters,
		    lpcmi->nShow,
		    idCmd - m_edit_existing_off);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਚੋਣ (idCmd)
	    {
		ਹਾਲਤ 0:
		    gvimExtraOptions = EDIT_WITH_VIM_USE_TABPAGES;
		    ਰੋਕੋ;
		ਹਾਲਤ 1:
		    gvimExtraOptions = EDIT_WITH_VIM_NO_OPTIONS;
		    ਰੋਕੋ;
		ਹਾਲਤ 2:
		    gvimExtraOptions = EDIT_WITH_VIM_ਵਿੱਚ_DIFF_ਬਹੁਲਕ;
		    ਰੋਕੋ;
		ਮੂਲ:
		    // If execution reaches this point we likely have an
		    // inconsistency between the code that setup the menus
		    // and this code that determines what the user
		    // selected.  This should be detected and fixed during
		    // development.
		    ਵਾਪਸ E_FAIL;
	    }

            LPCMINVOKECOMMANDINFOEX lpcmiex = (LPCMINVOKECOMMANDINFOEX)lpcmi;
            LPCWSTR currentDirectory = lpcmi->cb_ਮਾਪ == ਆਕਾਰ(CMINVOKECOMMANDINFOEX) ? lpcmiex->lpDirectoryW : ਨਿਰਾ;

	    hr = InvokeSingleGvim(lpcmi->hwnd,
		    currentDirectory,
		    lpcmi->lpVerb,
		    lpcmi->lpParameters,
		    lpcmi->nShow,
		    gvimExtraOptions);
	}
    }
    ਵਾਪਸ hr;
}

STDMETHODIMP CShellExt::ਧੱਕੋ_ਮੰਜ਼ਿਲ_ਖਿੜਕੀ(HWND  /* hParent */,
				   LPCSTR  /* pszWorkingDir */,
				   LPCSTR  /* pszCmd */,
				   LPCSTR  /* pszParam */,
				   ਪੂਰਨ  /* iShowCmd */,
				   ਪੂਰਨ idHWnd)
{
    HWND hWnd = m_hWnd[idHWnd];

    // Show and bring vim instance to foreground
    ਜੇ (IsIconic(hWnd) != 0)
	Show_ਖਿੜਕੀ(hWnd, SW_RESTORE);
    ਨਹੀਂਤਾਂ
	Show_ਖਿੜਕੀ(hWnd, SW_SHOW);
    ਸੈੱਟ_Foreground_ਖਿੜਕੀ(hWnd);

    // Post the selected files to the vim instance
    Post_ਸੁਨੇਹਾ(hWnd, WM_DROPFILES, (WPARAM)medium.hGlobal, 0);

    ਵਾਪਸ NOERROR;
}

STDMETHODIMP CShellExt::GetCommandString(UINT_PTR  /* idCmd */,
					 UINT uFlags,
					 UINT FAR * /* reserved */,
					 LPSTR pszName,
					 UINT cchMax)
{
    // Use UTF-8 for gettext.
    ਅੱਖਰ *prev = ਸੈੱਟ_gettext_codeset();

    WCHAR *s = W(_("Edits the selected file(s) with Vim"));
    ਜੇ (uFlags == GCS_HELPTEXTW && cchMax > wcslen(s))
	wcscpy((WCHAR *)pszName, s);
    ਮੁਕਤ(s);

    // Restore previous codeset.
    restore_gettext_codeset(prev);

    ਵਾਪਸ NOERROR;
}

BOOL CALLBACK CShellExt::EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    ਅੱਖਰ ਆਰਜ਼ੀ[BUFSIZE];

    // First do a bunch of check
    // No invisible window
    ਜੇ (!Is_ਖਿੜਕੀ_Visible(hWnd))
	ਵਾਪਸ TRUE;
    // No child window ???
    // if (GetParent(hWnd)) return TRUE;
    // Class name should be Vim, if failed to get class name, return
    ਜੇ (GetClassName(hWnd, ਆਰਜ਼ੀ, ARRAY_ਲੰਮਾਈ(ਆਰਜ਼ੀ)) == 0)
	ਵਾਪਸ TRUE;
    // Compare class name to that of vim, if not, return
    ਜੇ (_strnicmp(ਆਰਜ਼ੀ, "vim", ARRAY_ਲੰਮਾਈ("vim")) != 0)
	ਵਾਪਸ TRUE;
    // First check if the number of vim instance exceeds MAX_HWND
    CShellExt *cs = (CShellExt*) lParam;
    ਜੇ (cs->m_cnt_ਦਾ_HWnd >= MAX_HWND)
	ਵਾਪਸ FALSE;	// stop enumeration
    // Now we get the vim window, put it into some kind of array
    cs->m_hWnd[cs->m_cnt_ਦਾ_HWnd] = hWnd;
    cs->m_cnt_ਦਾ_HWnd ++;

    ਵਾਪਸ TRUE; // continue enumeration (otherwise this would be false)
}

BOOL CShellExt::ਲੱਦੋ_Menu_Icon()
{
    ਅੱਖਰ vimExeFile[BUFSIZE];
    getGvimName(vimExeFile, TRUE);
    ਜੇ (vimExeFile[0] == '\0')
	ਵਾਪਸ FALSE;
    HICON hVimIcon;
    ਜੇ (ਕੱਢੋ_Icon_Ex(vimExeFile, 0, ਨਿਰਾ, &hVimIcon, 1) == 0)
	ਵਾਪਸ FALSE;
    m_hVimIconBitmap = Icon_ਮੰਜ਼ਿਲ_Bitmap(hVimIcon,
	    GetSysColorBrush(COLOR_MENU),
	    GetSystemMetrics(SM_CXSMICON),
	    GetSystemMetrics(SM_CYSMICON));
    ਵਾਪਸ TRUE;
}

    ਸਥਿਰ ਅੱਖਰ *
searchpath(ਅੱਖਰ *name)
{
    ਸਥਿਰ ਸੰਘ {
	ਅੱਖਰ	location[2 * BUFSIZE];
	WCHAR	wname[BUFSIZE];
    };
    WCHAR wlocation[BUFSIZE + 1];

    // There appears to be a bug in FindExecutableA() on Windows NT.
    // Use FindExecutableW() instead...
    Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char(CP_ACP, 0, name, -1, wname, BUFSIZE);
    ਜੇ (ਲੱਭੋ_Executable_W(wname, L"", wlocation) > (HINSTANCE)32)
    {
	Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte(CP_ACP, 0, wlocation, -1,
		location, ਆਕਾਰ(location), ਨਿਰਾ, ਨਿਰਾ);
	ਵਾਪਸ location;
    }
    ਵਾਪਸ (ਅੱਖਰ *)"";
}


STDMETHODIMP CShellExt::InvokeSingleGvim(HWND h_ਮਾਪਾ,
				   LPCWSTR  workingDir,
				   LPCSTR  /* pszCmd */,
				   LPCSTR  /* pszParam */,
				   ਪੂਰਨ  /* iShowCmd */,
				   ਪੂਰਨ gvimExtraOptions)
{
    wchar_t	m_szFileUserClickedOn[BUFSIZE];
    wchar_t	*cmdStrW;
    size_t	cmdlen;
    size_t	len;
    UINT i;

    cmdlen = BUFSIZE;
    cmdStrW  = (wchar_t *) ਸਮ੍ਰਿਤੀ_ਦਿਓ(cmdlen * ਆਕਾਰ(wchar_t));
    ਜੇ (cmdStrW == ਨਿਰਾ)
	ਵਾਪਸ E_FAIL;
    getGvimInvocationW(cmdStrW);

    ਜੇ (gvimExtraOptions == EDIT_WITH_VIM_ਵਿੱਚ_DIFF_ਬਹੁਲਕ)
	wcscat(cmdStrW, L" -d");
    ਨਹੀਂਤਾਂ ਜੇ (gvimExtraOptions == EDIT_WITH_VIM_USE_TABPAGES)
	wcscat(cmdStrW, L" -p");
    ਲਈ (i = 0; i < cbFiles; i++)
    {
	DragQueryFileW((HDROP)medium.hGlobal,
		i,
		m_szFileUserClickedOn,
		ARRAY_ਲੰਮਾਈ(m_szFileUserClickedOn));

	len = wcslen(cmdStrW) + wcslen(m_szFileUserClickedOn) + 4;
	ਜੇ (len > cmdlen)
	{
	    cmdlen = len + BUFSIZE;
	    wchar_t *cmdStrW_new = (wchar_t *)realloc(cmdStrW, cmdlen * ਆਕਾਰ(wchar_t));
	    ਜੇ (cmdStrW_new == ਨਿਰਾ)
	    {
		ਮੁਕਤ(cmdStrW);
		ਵਾਪਸ E_FAIL;
	    }
	    cmdStrW = cmdStrW_new;
	}
	wcscat(cmdStrW, L" \"");
	wcscat(cmdStrW, m_szFileUserClickedOn);
	wcscat(cmdStrW, L"\"");
    }

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    Zero_ਸਮ੍ਰਿਤੀ(&si, ਆਕਾਰ(si));
    si.cb = ਆਕਾਰ(si);

    // Start the child process.
    ਜੇ (!CreateProcessW(ਨਿਰਾ,	// No module name (use command line).
		cmdStrW,	// Command line.
		ਨਿਰਾ,		// Process handle not inheritable.
		ਨਿਰਾ,		// Thread handle not inheritable.
		FALSE,		// Set handle inheritance to FALSE.
		0,		// No creation flags.
		ਨਿਰਾ,		// Use parent's environment block.
		workingDir,	// Use parent's starting directory.
		&si,		// Pointer to STARTUPINFO structure.
		&pi)		// Pointer to PROCESS_INFORMATION structure.
       )
    {
	// Use UTF-8 for gettext.
	ਅੱਖਰ *prev = ਸੈੱਟ_gettext_codeset();

	WCHAR *ਸੁਨੇਹਾ = W(_("Error creating process: Check if gvim is in your path!"));
	WCHAR *title = W(_("gvimext.dll error"));

	ਸੁਨੇਹਾ_Box_W(h_ਮਾਪਾ, ਸੁਨੇਹਾ, title, MB_OK);

	ਮੁਕਤ(ਸੁਨੇਹਾ);
	ਮੁਕਤ(title);

	// Restore previous codeset.
	restore_gettext_codeset(prev);
    }
    ਨਹੀਂਤਾਂ
    {
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
    }
    ਮੁਕਤ(cmdStrW);

    ਵਾਪਸ NOERROR;
}
