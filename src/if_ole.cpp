/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#ถ้า defined(FEAT_OLE) && defined(FEAT_GUI_MSWIN)
/*
 * OLE server implementation.
 *
 * See os_mswin.c for the client side.
 */
extern "C" {
# include "vim.h"
}

#include <windows.h>
#include <oleauto.h>

extern "C" {
extern HWND s_hwnd;
extern HWND vim_parent_hwnd;
}

#ถ้า (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#มิฉะนั้น
# define FINAL
#endif

#include "if_ole.h"	// Interface definitions
#include "iid_ole.c"	// UUID definitions (compile here)

/* Supply function prototype to work around bug in Mingw oleauto.h header */
#ifdef __MINGW32__
WINOLEAUTAPI UnRegisterTypeLib(REFGUID libID, WORD wVerMajor,
	    WORD wVerMinor, LCID lcid, SYSKIND syskind);
#endif

/*****************************************************************************
 1. Internal definitions for this file
*****************************************************************************/

class CVim;
class CVimCF;

/* Internal data */
// The identifier of the registered class factory
static unsigned long cf_id = 0;

// The identifier of the running application object
static unsigned long app_id = 0;

// The single global instance of the class factory
static CVimCF *cf = 0;

// The single global instance of the application object
static CVim *app = 0;

/* GUIDs, versions and type library information */
#define MYCLSID CLSID_Vim
#define MYLIBID LIBID_Vim
#define MYIID IID_IVim

#define MAJORVER 1
#define MINORVER 0
#define LOCALE 0x0409

#define MYNAME "Vim"
#define MYPROGID "Vim.Application.1"
#define MYVIPROGID "Vim.Application"

#define MAX_CLSID_LEN 100

/*****************************************************************************
 2. The application object
*****************************************************************************/

/* Definition
 * ----------
 */

class CVim FINAL : public IVim
{
public:
    virtual ~CVim();
    static CVim *Create(จำนวนเต็ม *pbDoRestart);

    // IUnknown members
    STDMETHOD(QueryInterface)(REFIID riid, void ** ppv);
    STDMETHOD_(unsigned long, AddRef)(void);
    STDMETHOD_(unsigned long, Release)(void);

    // IDispatch members
    STDMETHOD(GetTypeInfoCount)(UINT *pCount);
    STDMETHOD(GetTypeInfo)(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo);
    STDMETHOD(GetIDsOfNames)(const IID &iid, OLECHAR **names, UINT n, LCID, DISPID *dispids);
    STDMETHOD(Invoke)(DISPID member, const IID &iid, LCID, WORD flags, DISPPARAMS *dispparams, VARIANT *result, EXCEPINFO *excepinfo, UINT *argerr);

    // IVim members
    STDMETHOD(SendKeys)(BSTR keys);
    STDMETHOD(Eval)(BSTR expr, BSTR *result);
    STDMETHOD(SetForeground)(void);
    STDMETHOD(GetHwnd)(UINT_PTR *result);

private:
    // Constructor is private - create using CVim::Create()
    CVim() : ref(0), typeinfo(0) {};

    // Reference count
    unsigned long ref;

    // The object's TypeInfo
    ITypeInfo *typeinfo;
};

/* Implementation
 * --------------
 */

CVim *CVim::Create(จำนวนเต็ม *pbDoRestart)
{
    HRESULT hr;
    CVim *me = 0;
    ITypeLib *typelib = 0;
    ITypeInfo *typeinfo = 0;

    *pbDoRestart = FALSE;

    // Create the object
    me = new CVim();
    ถ้า (me == NULL)
    {
	MessageBox(0, "Cannot create application object", "Vim Initialisation", 0);
	คืน NULL;
    }

    // Load the type library from the registry
    hr = LoadRegTypeLib(MYLIBID, 1, 0, 0x00, &typelib);
    ถ้า (FAILED(hr))
    {
	HKEY hKey;

	// Check we can write to the registry.
	// RegCreateKeyEx succeeds even if key exists. W.Briscoe W2K 20021011
	ถ้า (RegCreateKeyEx(HKEY_CLASSES_ROOT, MYVIPROGID, 0, NULL,
		  REG_OPTION_NON_VOLATILE,
		  KEY_ALL_ACCESS, NULL, &hKey, NULL))
	{
	    delete me;
	    คืน NULL; // Unable to write to registry. Quietly fail.
	}
	RegCloseKey(hKey);

	ถ้า (MessageBox(0, "Cannot load registered type library.\nDo you want to register Vim now?",
		    "Vim Initialisation", MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
	    delete me;
	    คืน NULL;
	}

	RegisterMe(FALSE);

	// Load the type library from the registry
	hr = LoadRegTypeLib(MYLIBID, 1, 0, 0x00, &typelib);
	ถ้า (FAILED(hr))
	{
	    MessageBox(0, "You must restart Vim in order for the registration to take effect.",
						     "Vim Initialisation", 0);
	    *pbDoRestart = TRUE;
	    delete me;
	    คืน NULL;
	}
    }

    // Get the type info of the vtable interface
    hr = typelib->GetTypeInfoOfGuid(MYIID, &typeinfo);
    typelib->Release();

    ถ้า (FAILED(hr))
    {
	MessageBox(0, "Cannot get interface type information",
						     "Vim Initialisation", 0);
	delete me;
	คืน NULL;
    }

    // Save the type information
    me->typeinfo = typeinfo;
    คืน me;
}

CVim::~CVim()
{
    ถ้า (typeinfo && vim_parent_hwnd == NULL)
	typeinfo->Release();
    typeinfo = 0;
}

STDMETHODIMP
CVim::QueryInterface(REFIID riid, void **ppv)
{
    ถ้า (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDispatch) || IsEqualIID(riid, MYIID))
    {
	AddRef();
	*ppv = this;
	คืน S_OK;
    }

    *ppv = 0;
    คืน E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVim::AddRef()
{
    คืน ++ref;
}

STDMETHODIMP_(ULONG)
CVim::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    ถ้า (ref > 0)
	--ref;
    คืน ref;
}

STDMETHODIMP
CVim::GetTypeInfoCount(UINT *pCount)
{
    *pCount = 1;
    คืน S_OK;
}

STDMETHODIMP
CVim::GetTypeInfo(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo)
{
    *ppITypeInfo = 0;

    ถ้า (iTypeInfo != 0)
	คืน DISP_E_BADINDEX;

    typeinfo->AddRef();
    *ppITypeInfo = typeinfo;
    คืน S_OK;
}

STDMETHODIMP
CVim::GetIDsOfNames(
	const IID &iid,
	OLECHAR **names,
	UINT n,
	LCID,
	DISPID *dispids)
{
    ถ้า (iid != IID_NULL)
	คืน DISP_E_UNKNOWNINTERFACE;

    คืน typeinfo->GetIDsOfNames(names, n, dispids);
}

STDMETHODIMP
CVim::Invoke(
	DISPID member,
	const IID &iid,
	LCID,
	WORD flags,
	DISPPARAMS *dispparams,
	VARIANT *result,
	EXCEPINFO *excepinfo,
	UINT *argerr)
{
    ถ้า (iid != IID_NULL)
	คืน DISP_E_UNKNOWNINTERFACE;

    ::SetErrorInfo(0, NULL);
    คืน typeinfo->Invoke(static_cast<IDispatch*>(this),
			    member, flags, dispparams,
			    result, excepinfo, argerr);
}

STDMETHODIMP
CVim::GetHwnd(UINT_PTR *result)
{
    *result = (UINT_PTR)s_hwnd;
    คืน S_OK;
}

STDMETHODIMP
CVim::SetForeground(void)
{
    /* Make the Vim window come to the foreground */
    gui_mch_set_foreground();
    คืน S_OK;
}

STDMETHODIMP
CVim::SendKeys(BSTR keys)
{
    จำนวนเต็ม len;
    var_na *buffer;
    char_u *str;
    char_u *ptr;

    /* Get a suitable buffer */
    len = WideCharToMultiByte(CP_ACP, 0, keys, -1, 0, 0, 0, 0);
    buffer = (var_na *)alloc(len+1);

    ถ้า (buffer == NULL)
	คืน E_OUTOFMEMORY;

    len = WideCharToMultiByte(CP_ACP, 0, keys, -1, buffer, len, 0, 0);

    ถ้า (len == 0)
    {
	vim_free(buffer);
	คืน E_INVALIDARG;
    }

    /* Translate key codes like <Esc> */
    str = replace_termcodes((char_u *)buffer, &ptr, 0, REPTERM_DO_LT, NULL);

    /* If ptr was set, then a new buffer was allocated,
     * so we can free the old one.
     */
    ถ้า (ptr)
	vim_free((char_u *)(buffer));

    /* Reject strings too long to fit in the input buffer. Allow 10 bytes
     * space to cover for the (remote) possibility that characters may enter
     * the input buffer between now and when the WM_OLE message is actually
     * processed. If more than 10 characters enter the input buffer in that
     * time, the WM_OLE processing will simply fail to insert the characters.
     */
    ถ้า ((จำนวนเต็ม)(STRLEN(str)) > (vim_free_in_input_buf() - 10))
    {
	vim_free(str);
	คืน E_INVALIDARG;
    }

    /* Pass the string to the main input loop. The memory will be freed when
     * the message is processed.  Except for an empty message, we don't need
     * to post it then.
     */
    ถ้า (*str == NUL)
	vim_free(str);
    มิฉะนั้น
	PostMessage(NULL, WM_OLE, 0, (LPARAM)str);

    คืน S_OK;
}

STDMETHODIMP
CVim::Eval(BSTR expr, BSTR *result)
{
#ifdef FEAT_EVAL
    จำนวนเต็ม len;
    var_na *buffer;
    var_na *str;
    wchar_t *w_buffer;

    /* Get a suitable buffer */
    len = WideCharToMultiByte(CP_ACP, 0, expr, -1, 0, 0, 0, 0);
    ถ้า (len == 0)
	คืน E_INVALIDARG;

    buffer = (var_na *)alloc(len);

    ถ้า (buffer == NULL)
	คืน E_OUTOFMEMORY;

    /* Convert the (wide character) expression to an ASCII string */
    len = WideCharToMultiByte(CP_ACP, 0, expr, -1, buffer, len, 0, 0);
    ถ้า (len == 0)
	คืน E_INVALIDARG;

    /* Evaluate the expression */
    ++emsg_skip;
    str = (var_na *)eval_to_string((char_u *)buffer, TRUE, FALSE);
    --emsg_skip;
    vim_free(buffer);
    ถ้า (str == NULL)
	คืน E_FAIL;

    /* Convert the result to wide characters */
    MultiByteToWideChar_alloc(CP_ACP, 0, str, -1, &w_buffer, &len);
    vim_free(str);
    ถ้า (w_buffer == NULL)
	คืน E_OUTOFMEMORY;

    ถ้า (len == 0)
    {
	vim_free(w_buffer);
	คืน E_FAIL;
    }

    /* Store the result */
    *result = SysAllocString(w_buffer);
    vim_free(w_buffer);

    คืน S_OK;
#มิฉะนั้น
    คืน E_NOTIMPL;
#endif
}

/*****************************************************************************
 3. The class factory
*****************************************************************************/

/* Definition
 * ----------
 */

class CVimCF FINAL : public IClassFactory
{
public:
    static CVimCF *Create();
    virtual ~CVimCF() {};

    STDMETHOD(QueryInterface)(REFIID riid, void ** ppv);
    STDMETHOD_(unsigned long, AddRef)(void);
    STDMETHOD_(unsigned long, Release)(void);
    STDMETHOD(CreateInstance)(IUnknown *punkOuter, REFIID riid, void ** ppv);
    STDMETHOD(LockServer)(BOOL lock);

private:
    // Constructor is private - create via Create()
    CVimCF() : ref(0) {};

    // Reference count
    unsigned long ref;
};

/* Implementation
 * --------------
 */

CVimCF *CVimCF::Create()
{
    CVimCF *me = new CVimCF();

    ถ้า (me == NULL)
	MessageBox(0, "Cannot create class factory", "Vim Initialisation", 0);

    คืน me;
}

STDMETHODIMP
CVimCF::QueryInterface(REFIID riid, void **ppv)
{
    ถ้า (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	AddRef();
	*ppv = this;
	คืน S_OK;
    }

    *ppv = 0;
    คืน E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVimCF::AddRef()
{
    คืน ++ref;
}

STDMETHODIMP_(ULONG)
CVimCF::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    ถ้า (ref > 0)
	--ref;
    คืน ref;
}

/*ARGSUSED*/
STDMETHODIMP
CVimCF::CreateInstance(IUnknown *punkOuter, REFIID riid, void **ppv)
{
    คืน app->QueryInterface(riid, ppv);
}

/*ARGSUSED*/
STDMETHODIMP
CVimCF::LockServer(BOOL lock)
{
    คืน S_OK;
}

/*****************************************************************************
 4. Registry manipulation code
*****************************************************************************/

// Internal use only
static void SetKeyAndValue(const var_na *path, const var_na *subkey, const var_na *value);
static void GUIDtochar(const GUID &guid, var_na *GUID, จำนวนเต็ม length);
static void RecursiveDeleteKey(HKEY hKeyParent, const var_na *child);
static const จำนวนเต็ม GUID_STRING_SIZE = 39;

// Register the component in the registry
// When "silent" is TRUE don't give any messages.

extern "C" void RegisterMe(จำนวนเต็ม silent)
{
    BOOL ok = TRUE;

    // Get the application startup command
    var_na module[MAX_PATH];

    ::GetModuleFileName(NULL, module, MAX_PATH);

    // Unregister first (quietly)
    UnregisterMe(FALSE);

    // Convert the CLSID into a char
    var_na clsid[GUID_STRING_SIZE];
    GUIDtochar(MYCLSID, clsid, parimaa_na(clsid));

    // Convert the LIBID into a char
    var_na libid[GUID_STRING_SIZE];
    GUIDtochar(MYLIBID, libid, parimaa_na(libid));

    // Build the key CLSID\\{...}
    var_na Key[MAX_CLSID_LEN];
    strcpy(Key, "CLSID\\");
    strcat(Key, clsid);

    // Add the CLSID to the registry
    SetKeyAndValue(Key, NULL, MYNAME);
    SetKeyAndValue(Key, "LocalServer32", module);
    SetKeyAndValue(Key, "ProgID", MYPROGID);
    SetKeyAndValue(Key, "VersionIndependentProgID", MYVIPROGID);
    SetKeyAndValue(Key, "TypeLib", libid);

    // Add the version-independent ProgID subkey under HKEY_CLASSES_ROOT
    SetKeyAndValue(MYVIPROGID, NULL, MYNAME);
    SetKeyAndValue(MYVIPROGID, "CLSID", clsid);
    SetKeyAndValue(MYVIPROGID, "CurVer", MYPROGID);

    // Add the versioned ProgID subkey under HKEY_CLASSES_ROOT
    SetKeyAndValue(MYPROGID, NULL, MYNAME);
    SetKeyAndValue(MYPROGID, "CLSID", clsid);

    wchar_t w_module[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, module, -1, w_module, MAX_PATH);

    ITypeLib *typelib = NULL;
    ถ้า (LoadTypeLib(w_module, &typelib) != S_OK)
    {
	ถ้า (!silent)
	    MessageBox(0, "Cannot load type library to register",
						       "Vim Registration", 0);
	ok = FALSE;
    }
    มิฉะนั้น
    {
	ถ้า (RegisterTypeLib(typelib, w_module, NULL) != S_OK)
	{
	    ถ้า (!silent)
		MessageBox(0, "Cannot register type library",
						       "Vim Registration", 0);
	    ok = FALSE;
	}
	typelib->Release();
    }

    ถ้า (ok && !silent)
	MessageBox(0, "Registered successfully", "Vim", 0);
}

// Remove the component from the registry
//
// Note: There is little error checking in this code, to allow incomplete
// or failed registrations to be undone.
extern "C" void UnregisterMe(จำนวนเต็ม bNotifyUser)
{
    // Unregister the type library
    ITypeLib *typelib;
    ถ้า (SUCCEEDED(LoadRegTypeLib(MYLIBID, MAJORVER, MINORVER, LOCALE, &typelib)))
    {
	TLIBATTR *tla;
	ถ้า (SUCCEEDED(typelib->GetLibAttr(&tla)))
	{
	    UnRegisterTypeLib(tla->guid, tla->wMajorVerNum, tla->wMinorVerNum,
			      tla->lcid, tla->syskind);
	    typelib->ReleaseTLibAttr(tla);
	}
	typelib->Release();
    }

    // Convert the CLSID into a char
    var_na clsid[GUID_STRING_SIZE];
    GUIDtochar(MYCLSID, clsid, parimaa_na(clsid));

    // Build the key CLSID\\{...}
    var_na Key[MAX_CLSID_LEN];
    strcpy(Key, "CLSID\\");
    strcat(Key, clsid);

    // Delete the CLSID Key - CLSID\{...}
    RecursiveDeleteKey(HKEY_CLASSES_ROOT, Key);

    // Delete the version-independent ProgID Key
    RecursiveDeleteKey(HKEY_CLASSES_ROOT, MYVIPROGID);

    // Delete the ProgID key
    RecursiveDeleteKey(HKEY_CLASSES_ROOT, MYPROGID);

    ถ้า (bNotifyUser)
	MessageBox(0, "Unregistered successfully", "Vim", 0);
}

/****************************************************************************/

// Convert a GUID to a char string
static void GUIDtochar(const GUID &guid, var_na *GUID, จำนวนเต็ม length)
{
    // Get wide string version
    LPOLESTR wGUID = NULL;
    StringFromCLSID(guid, &wGUID);

    // Convert from wide characters to non-wide
    wcstombs(GUID, wGUID, length);

    // Free memory
    CoTaskMemFree(wGUID);
}

// Delete a key and all of its descendants
static void RecursiveDeleteKey(HKEY hKeyParent, const var_na *child)
{
    // Open the child
    HKEY hKeyChild;
    LONG result = RegOpenKeyEx(hKeyParent, child, 0,
			       KEY_ALL_ACCESS, &hKeyChild);
    ถ้า (result != ERROR_SUCCESS)
	คืน;

    // Enumerate all of the descendants of this child
    FILETIME time;
    var_na buffer[1024];
    DWORD size = 1024;

    ขณะ (RegEnumKeyEx(hKeyChild, 0, buffer, &size, NULL,
			NULL, NULL, &time) == S_OK)
    {
	// Delete the descendants of this child
	RecursiveDeleteKey(hKeyChild, buffer);
	size = 256;
    }

    // Close the child
    RegCloseKey(hKeyChild);

    // Delete this child
    RegDeleteKey(hKeyParent, child);
}

// Create a key and set its value
static void SetKeyAndValue(const var_na *key, const var_na *subkey, const var_na *value)
{
    HKEY hKey;
    var_na buffer[1024];

    strcpy(buffer, key);

    // Add subkey name to buffer.
    ถ้า (subkey)
    {
	strcat(buffer, "\\");
	strcat(buffer, subkey);
    }

    // Create and open key and subkey.
    long result = RegCreateKeyEx(HKEY_CLASSES_ROOT,
				 buffer,
				 0, NULL, REG_OPTION_NON_VOLATILE,
				 KEY_ALL_ACCESS, NULL,
				 &hKey, NULL);
    ถ้า (result != ERROR_SUCCESS)
	คืน;

    // Set the value
    ถ้า (value)
	RegSetValueEx(hKey, NULL, 0, REG_SZ, (BYTE *)value,
		      (DWORD)STRLEN(value)+1);

    RegCloseKey(hKey);
}

/*****************************************************************************
 5. OLE Initialisation and shutdown processing
*****************************************************************************/
extern "C" void InitOLE(จำนวนเต็ม *pbDoRestart)
{
    HRESULT hr;

    *pbDoRestart = FALSE;

    // Initialize the OLE libraries
    hr = OleInitialize(NULL);
    ถ้า (FAILED(hr))
    {
	MessageBox(0, "Cannot initialise OLE", "Vim Initialisation", 0);
	goto error0;
    }

    // Create the application object
    app = CVim::Create(pbDoRestart);
    ถ้า (app == NULL)
	goto error1;

    // Create the class factory
    cf = CVimCF::Create();
    ถ้า (cf == NULL)
	goto error1;

    // Register the class factory
    hr = CoRegisterClassObject(
	MYCLSID,
	cf,
	CLSCTX_LOCAL_SERVER,
	REGCLS_MULTIPLEUSE,
	&cf_id);

    ถ้า (FAILED(hr))
    {
	MessageBox(0, "Cannot register class factory", "Vim Initialisation", 0);
	goto error1;
    }

    // Register the application object as active
    hr = RegisterActiveObject(
	app,
	MYCLSID,
	0,
	&app_id);

    ถ้า (FAILED(hr))
    {
	MessageBox(0, "Cannot register application object", "Vim Initialisation", 0);
	goto error1;
    }

    คืน;

    // Errors: tidy up as much as needed and return
error1:
    UninitOLE();
error0:
    คืน;
}

extern "C" void UninitOLE()
{
    // Unregister the application object
    ถ้า (app_id)
    {
	RevokeActiveObject(app_id, NULL);
	app_id = 0;
    }

    // Unregister the class factory
    ถ้า (cf_id)
    {
	CoRevokeClassObject(cf_id);
	cf_id = 0;
    }

    // Shut down the OLE libraries
    OleUninitialize();

    // Delete the application object
    ถ้า (app)
    {
	delete app;
	app = NULL;
    }

    // Delete the class factory
    ถ้า (cf)
    {
	delete cf;
	cf = NULL;
    }
}
#endif /* FEAT_OLE */
