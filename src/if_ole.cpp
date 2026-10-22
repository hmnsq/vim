/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#නම් defined(FEAT_OLE) && defined(FEAT_GUI_MSWIN)
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

#නම් (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#නැතහොත්
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
    static CVim *Create(පූර්ණ_සංඛ්‍යා *pbDoRestart);

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

CVim *CVim::Create(පූර්ණ_සංඛ්‍යා *pbDoRestart)
{
    HRESULT hr;
    CVim *me = 0;
    ITypeLib *typelib = 0;
    ITypeInfo *typeinfo = 0;

    *pbDoRestart = FALSE;

    // Create the object
    me = new CVim();
    නම් (me == NULL)
    {
	MessageBox(0, "Cannot create application object", "Vim Initialisation", 0);
	ආපසු NULL;
    }

    // Load the type library from the registry
    hr = LoadRegTypeLib(MYLIBID, 1, 0, 0x00, &typelib);
    නම් (FAILED(hr))
    {
	HKEY hKey;

	// Check we can write to the registry.
	// RegCreateKeyEx succeeds even if key exists. W.Briscoe W2K 20021011
	නම් (RegCreateKeyEx(HKEY_CLASSES_ROOT, MYVIPROGID, 0, NULL,
		  REG_OPTION_NON_VOLATILE,
		  KEY_ALL_ACCESS, NULL, &hKey, NULL))
	{
	    delete me;
	    ආපසු NULL; // Unable to write to registry. Quietly fail.
	}
	RegCloseKey(hKey);

	නම් (MessageBox(0, "Cannot load registered type library.\nDo you want to register Vim now?",
		    "Vim Initialisation", MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
	    delete me;
	    ආපසු NULL;
	}

	RegisterMe(FALSE);

	// Load the type library from the registry
	hr = LoadRegTypeLib(MYLIBID, 1, 0, 0x00, &typelib);
	නම් (FAILED(hr))
	{
	    MessageBox(0, "You must restart Vim in order for the registration to take effect.",
						     "Vim Initialisation", 0);
	    *pbDoRestart = TRUE;
	    delete me;
	    ආපසු NULL;
	}
    }

    // Get the type info of the vtable interface
    hr = typelib->GetTypeInfoOfGuid(MYIID, &typeinfo);
    typelib->Release();

    නම් (FAILED(hr))
    {
	MessageBox(0, "Cannot get interface type information",
						     "Vim Initialisation", 0);
	delete me;
	ආපසු NULL;
    }

    // Save the type information
    me->typeinfo = typeinfo;
    ආපසු me;
}

CVim::~CVim()
{
    නම් (typeinfo && vim_parent_hwnd == NULL)
	typeinfo->Release();
    typeinfo = 0;
}

STDMETHODIMP
CVim::QueryInterface(REFIID riid, void **ppv)
{
    නම් (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDispatch) || IsEqualIID(riid, MYIID))
    {
	AddRef();
	*ppv = this;
	ආපසු S_OK;
    }

    *ppv = 0;
    ආපසු E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVim::AddRef()
{
    ආපසු ++ref;
}

STDMETHODIMP_(ULONG)
CVim::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    නම් (ref > 0)
	--ref;
    ආපසු ref;
}

STDMETHODIMP
CVim::GetTypeInfoCount(UINT *pCount)
{
    *pCount = 1;
    ආපසු S_OK;
}

STDMETHODIMP
CVim::GetTypeInfo(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo)
{
    *ppITypeInfo = 0;

    නම් (iTypeInfo != 0)
	ආපසු DISP_E_BADINDEX;

    typeinfo->AddRef();
    *ppITypeInfo = typeinfo;
    ආපසු S_OK;
}

STDMETHODIMP
CVim::GetIDsOfNames(
	const IID &iid,
	OLECHAR **names,
	UINT n,
	LCID,
	DISPID *dispids)
{
    නම් (iid != IID_NULL)
	ආපසු DISP_E_UNKNOWNINTERFACE;

    ආපසු typeinfo->GetIDsOfNames(names, n, dispids);
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
    නම් (iid != IID_NULL)
	ආපසු DISP_E_UNKNOWNINTERFACE;

    ::SetErrorInfo(0, NULL);
    ආපසු typeinfo->Invoke(static_cast<IDispatch*>(this),
			    member, flags, dispparams,
			    result, excepinfo, argerr);
}

STDMETHODIMP
CVim::GetHwnd(UINT_PTR *result)
{
    *result = (UINT_PTR)s_hwnd;
    ආපසු S_OK;
}

STDMETHODIMP
CVim::SetForeground(void)
{
    /* Make the Vim window come to the foreground */
    gui_mch_set_foreground();
    ආපසු S_OK;
}

STDMETHODIMP
CVim::SendKeys(BSTR keys)
{
    පූර්ණ_සංඛ්‍යා len;
    var_na *buffer;
    char_u *str;
    char_u *ptr;

    /* Get a suitable buffer */
    len = WideCharToMultiByte(CP_ACP, 0, keys, -1, 0, 0, 0, 0);
    buffer = (var_na *)alloc(len+1);

    නම් (buffer == NULL)
	ආපසු E_OUTOFMEMORY;

    len = WideCharToMultiByte(CP_ACP, 0, keys, -1, buffer, len, 0, 0);

    නම් (len == 0)
    {
	vim_free(buffer);
	ආපසු E_INVALIDARG;
    }

    /* Translate key codes like <Esc> */
    str = replace_termcodes((char_u *)buffer, &ptr, 0, REPTERM_DO_LT, NULL);

    /* If ptr was set, then a new buffer was allocated,
     * so we can free the old one.
     */
    නම් (ptr)
	vim_free((char_u *)(buffer));

    /* Reject strings too long to fit in the input buffer. Allow 10 bytes
     * space to cover for the (remote) possibility that characters may enter
     * the input buffer between now and when the WM_OLE message is actually
     * processed. If more than 10 characters enter the input buffer in that
     * time, the WM_OLE processing will simply fail to insert the characters.
     */
    නම් ((පූර්ණ_සංඛ්‍යා)(STRLEN(str)) > (vim_free_in_input_buf() - 10))
    {
	vim_free(str);
	ආපසු E_INVALIDARG;
    }

    /* Pass the string to the main input loop. The memory will be freed when
     * the message is processed.  Except for an empty message, we don't need
     * to post it then.
     */
    නම් (*str == NUL)
	vim_free(str);
    නැතහොත්
	PostMessage(NULL, WM_OLE, 0, (LPARAM)str);

    ආපසු S_OK;
}

STDMETHODIMP
CVim::Eval(BSTR expr, BSTR *result)
{
#ifdef FEAT_EVAL
    පූර්ණ_සංඛ්‍යා len;
    var_na *buffer;
    var_na *str;
    wchar_t *w_buffer;

    /* Get a suitable buffer */
    len = WideCharToMultiByte(CP_ACP, 0, expr, -1, 0, 0, 0, 0);
    නම් (len == 0)
	ආපසු E_INVALIDARG;

    buffer = (var_na *)alloc(len);

    නම් (buffer == NULL)
	ආපසු E_OUTOFMEMORY;

    /* Convert the (wide character) expression to an ASCII string */
    len = WideCharToMultiByte(CP_ACP, 0, expr, -1, buffer, len, 0, 0);
    නම් (len == 0)
	ආපසු E_INVALIDARG;

    /* Evaluate the expression */
    ++emsg_skip;
    str = (var_na *)eval_to_string((char_u *)buffer, TRUE, FALSE);
    --emsg_skip;
    vim_free(buffer);
    නම් (str == NULL)
	ආපසු E_FAIL;

    /* Convert the result to wide characters */
    MultiByteToWideChar_alloc(CP_ACP, 0, str, -1, &w_buffer, &len);
    vim_free(str);
    නම් (w_buffer == NULL)
	ආපසු E_OUTOFMEMORY;

    නම් (len == 0)
    {
	vim_free(w_buffer);
	ආපසු E_FAIL;
    }

    /* Store the result */
    *result = SysAllocString(w_buffer);
    vim_free(w_buffer);

    ආපසු S_OK;
#නැතහොත්
    ආපසු E_NOTIMPL;
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

    නම් (me == NULL)
	MessageBox(0, "Cannot create class factory", "Vim Initialisation", 0);

    ආපසු me;
}

STDMETHODIMP
CVimCF::QueryInterface(REFIID riid, void **ppv)
{
    නම් (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	AddRef();
	*ppv = this;
	ආපසු S_OK;
    }

    *ppv = 0;
    ආපසු E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVimCF::AddRef()
{
    ආපසු ++ref;
}

STDMETHODIMP_(ULONG)
CVimCF::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    නම් (ref > 0)
	--ref;
    ආපසු ref;
}

/*ARGSUSED*/
STDMETHODIMP
CVimCF::CreateInstance(IUnknown *punkOuter, REFIID riid, void **ppv)
{
    ආපසු app->QueryInterface(riid, ppv);
}

/*ARGSUSED*/
STDMETHODIMP
CVimCF::LockServer(BOOL lock)
{
    ආපසු S_OK;
}

/*****************************************************************************
 4. Registry manipulation code
*****************************************************************************/

// Internal use only
static void SetKeyAndValue(const var_na *path, const var_na *subkey, const var_na *value);
static void GUIDtochar(const GUID &guid, var_na *GUID, පූර්ණ_සංඛ්‍යා length);
static void RecursiveDeleteKey(HKEY hKeyParent, const var_na *child);
static const පූර්ණ_සංඛ්‍යා GUID_STRING_SIZE = 39;

// Register the component in the registry
// When "silent" is TRUE don't give any messages.

extern "C" void RegisterMe(පූර්ණ_සංඛ්‍යා silent)
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
    නම් (LoadTypeLib(w_module, &typelib) != S_OK)
    {
	නම් (!silent)
	    MessageBox(0, "Cannot load type library to register",
						       "Vim Registration", 0);
	ok = FALSE;
    }
    නැතහොත්
    {
	නම් (RegisterTypeLib(typelib, w_module, NULL) != S_OK)
	{
	    නම් (!silent)
		MessageBox(0, "Cannot register type library",
						       "Vim Registration", 0);
	    ok = FALSE;
	}
	typelib->Release();
    }

    නම් (ok && !silent)
	MessageBox(0, "Registered successfully", "Vim", 0);
}

// Remove the component from the registry
//
// Note: There is little error checking in this code, to allow incomplete
// or failed registrations to be undone.
extern "C" void UnregisterMe(පූර්ණ_සංඛ්‍යා bNotifyUser)
{
    // Unregister the type library
    ITypeLib *typelib;
    නම් (SUCCEEDED(LoadRegTypeLib(MYLIBID, MAJORVER, MINORVER, LOCALE, &typelib)))
    {
	TLIBATTR *tla;
	නම් (SUCCEEDED(typelib->GetLibAttr(&tla)))
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

    නම් (bNotifyUser)
	MessageBox(0, "Unregistered successfully", "Vim", 0);
}

/****************************************************************************/

// Convert a GUID to a char string
static void GUIDtochar(const GUID &guid, var_na *GUID, පූර්ණ_සංඛ්‍යා length)
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
    නම් (result != ERROR_SUCCESS)
	ආපසු;

    // Enumerate all of the descendants of this child
    FILETIME time;
    var_na buffer[1024];
    DWORD size = 1024;

    අතරතුර (RegEnumKeyEx(hKeyChild, 0, buffer, &size, NULL,
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
    නම් (subkey)
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
    නම් (result != ERROR_SUCCESS)
	ආපසු;

    // Set the value
    නම් (value)
	RegSetValueEx(hKey, NULL, 0, REG_SZ, (BYTE *)value,
		      (DWORD)STRLEN(value)+1);

    RegCloseKey(hKey);
}

/*****************************************************************************
 5. OLE Initialisation and shutdown processing
*****************************************************************************/
extern "C" void InitOLE(පූර්ණ_සංඛ්‍යා *pbDoRestart)
{
    HRESULT hr;

    *pbDoRestart = FALSE;

    // Initialize the OLE libraries
    hr = OleInitialize(NULL);
    නම් (FAILED(hr))
    {
	MessageBox(0, "Cannot initialise OLE", "Vim Initialisation", 0);
	goto error0;
    }

    // Create the application object
    app = CVim::Create(pbDoRestart);
    නම් (app == NULL)
	goto error1;

    // Create the class factory
    cf = CVimCF::Create();
    නම් (cf == NULL)
	goto error1;

    // Register the class factory
    hr = CoRegisterClassObject(
	MYCLSID,
	cf,
	CLSCTX_LOCAL_SERVER,
	REGCLS_MULTIPLEUSE,
	&cf_id);

    නම් (FAILED(hr))
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

    නම් (FAILED(hr))
    {
	MessageBox(0, "Cannot register application object", "Vim Initialisation", 0);
	goto error1;
    }

    ආපසු;

    // Errors: tidy up as much as needed and return
error1:
    UninitOLE();
error0:
    ආපසු;
}

extern "C" void UninitOLE()
{
    // Unregister the application object
    නම් (app_id)
    {
	RevokeActiveObject(app_id, NULL);
	app_id = 0;
    }

    // Unregister the class factory
    නම් (cf_id)
    {
	CoRevokeClassObject(cf_id);
	cf_id = 0;
    }

    // Shut down the OLE libraries
    OleUninitialize();

    // Delete the application object
    නම් (app)
    {
	delete app;
	app = NULL;
    }

    // Delete the class factory
    නම් (cf)
    {
	delete cf;
	cf = NULL;
    }
}
#endif /* FEAT_OLE */
