/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#𐡄𐡍 defined(FEAT_OLE) && defined(FEAT_GUI_MSWIN)
/*
 * OLE server implementation.
 *
 * See os_mswin.c for the client side.
 */
خارجي "C" {
# include "vim.h"
}

#ضمن <windows.h>
#ضمن <oleauto.h>

خارجي "C" {
خارجي HWND s_hwnd;
خارجي HWND vim_parent_hwnd;
}

#𐡄𐡍 (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#وإلا
# define FINAL
#endif

#ضمن "if_ole.h"	// Interface definitions
#ضمن "iid_ole.c"	// UUID definitions (compile here)

/* Supply function prototype to work around bug in Mingw oleauto.h header */
#ifdef __MINGW32__
WINOLEAUTAPI UnRegisterTypeLib(REFGUID libID, WORD wVerMajor,
	    WORD wVerMinor, LCID lcid, SYSKIND syskind);
#endif

/*****************************************************************************
 1. Internal definitions for this file
*****************************************************************************/

صنف CVim;
صنف CVimCF;

/* Internal data */
// The identifier of the registered class factory
ساكن unsigned long cf_id = 0;

// The identifier of the running application object
ساكن unsigned long app_id = 0;

// The single global instance of the class factory
ساكن CVimCF *cf = 0;

// The single global instance of the application object
ساكن CVim *app = 0;

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

صنف CVim FINAL : عام IVim
{
عام:
    virtual ~CVim();
    ساكن CVim *Create(صحيح *pbDoRestart);

    // IUnknown members
    STDMETHOD(QueryInterface)(REFIID riid, فراغ ** ppv);
    STDMETHOD_(unsigned long, AddRef)(فراغ);
    STDMETHOD_(unsigned long, Release)(فراغ);

    // IDispatch members
    STDMETHOD(GetTypeInfoCount)(UINT *pCount);
    STDMETHOD(GetTypeInfo)(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo);
    STDMETHOD(GetIDsOfNames)(ثابت IID &iid, OLECHAR **names, UINT n, LCID, DISPID *dispids);
    STDMETHOD(Invoke)(DISPID member, ثابت IID &iid, LCID, WORD flags, DISPPARAMS *dispparams, VARIANT *result, EXCEPINFO *excepinfo, UINT *argerr);

    // IVim members
    STDMETHOD(SendKeys)(BSTR keys);
    STDMETHOD(Eval)(BSTR expr, BSTR *result);
    STDMETHOD(SetForeground)(فراغ);
    STDMETHOD(GetHwnd)(UINT_PTR *result);

خاص:
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

CVim *CVim::Create(صحيح *pbDoRestart)
{
    HRESULT hr;
    CVim *me = 0;
    ITypeLib *typelib = 0;
    ITypeInfo *typeinfo = 0;

    *pbDoRestart = FALSE;

    // Create the object
    me = جديد CVim();
    𐡄𐡍 (me == عدم)
    {
	MessageBox(0, "Cannot create application object", "Vim Initialisation", 0);
	أرجع عدم;
    }

    // Load the type library from the registry
    hr = LoadRegTypeLib(MYLIBID, 1, 0, 0x00, &typelib);
    𐡄𐡍 (FAILED(hr))
    {
	HKEY hKey;

	// Check we can write to the registry.
	// RegCreateKeyEx succeeds even if key exists. W.Briscoe W2K 20021011
	𐡄𐡍 (RegCreateKeyEx(HKEY_CLASSES_ROOT, MYVIPROGID, 0, عدم,
		  REG_OPTION_NON_VOLATILE,
		  KEY_ALL_ACCESS, عدم, &hKey, عدم))
	{
	    delete me;
	    أرجع عدم; // Unable to write to registry. Quietly fail.
	}
	RegCloseKey(hKey);

	𐡄𐡍 (MessageBox(0, "Cannot load registered type library.\nDo you want to register Vim now?",
		    "Vim Initialisation", MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
	    delete me;
	    أرجع عدم;
	}

	RegisterMe(FALSE);

	// Load the type library from the registry
	hr = LoadRegTypeLib(MYLIBID, 1, 0, 0x00, &typelib);
	𐡄𐡍 (FAILED(hr))
	{
	    MessageBox(0, "You must restart Vim in order for the registration to take effect.",
						     "Vim Initialisation", 0);
	    *pbDoRestart = TRUE;
	    delete me;
	    أرجع عدم;
	}
    }

    // Get the type info of the vtable interface
    hr = typelib->GetTypeInfoOfGuid(MYIID, &typeinfo);
    typelib->Release();

    𐡄𐡍 (FAILED(hr))
    {
	MessageBox(0, "Cannot get interface type information",
						     "Vim Initialisation", 0);
	delete me;
	أرجع عدم;
    }

    // Save the type information
    me->typeinfo = typeinfo;
    أرجع me;
}

CVim::~CVim()
{
    𐡄𐡍 (typeinfo && vim_parent_hwnd == عدم)
	typeinfo->Release();
    typeinfo = 0;
}

STDMETHODIMP
CVim::QueryInterface(REFIID riid, فراغ **ppv)
{
    𐡄𐡍 (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDispatch) || IsEqualIID(riid, MYIID))
    {
	AddRef();
	*ppv = هذا;
	أرجع S_OK;
    }

    *ppv = 0;
    أرجع E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVim::AddRef()
{
    أرجع ++ref;
}

STDMETHODIMP_(ULONG)
CVim::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    𐡄𐡍 (ref > 0)
	--ref;
    أرجع ref;
}

STDMETHODIMP
CVim::GetTypeInfoCount(UINT *pCount)
{
    *pCount = 1;
    أرجع S_OK;
}

STDMETHODIMP
CVim::GetTypeInfo(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo)
{
    *ppITypeInfo = 0;

    𐡄𐡍 (iTypeInfo != 0)
	أرجع DISP_E_BADINDEX;

    typeinfo->AddRef();
    *ppITypeInfo = typeinfo;
    أرجع S_OK;
}

STDMETHODIMP
CVim::GetIDsOfNames(
	ثابت IID &iid,
	OLECHAR **names,
	UINT n,
	LCID,
	DISPID *dispids)
{
    𐡄𐡍 (iid != IID_NULL)
	أرجع DISP_E_UNKNOWNINTERFACE;

    أرجع typeinfo->GetIDsOfNames(names, n, dispids);
}

STDMETHODIMP
CVim::Invoke(
	DISPID member,
	ثابت IID &iid,
	LCID,
	WORD flags,
	DISPPARAMS *dispparams,
	VARIANT *result,
	EXCEPINFO *excepinfo,
	UINT *argerr)
{
    𐡄𐡍 (iid != IID_NULL)
	أرجع DISP_E_UNKNOWNINTERFACE;

    ::SetErrorInfo(0, عدم);
    أرجع typeinfo->Invoke(static_cast<IDispatch*>(هذا),
			    member, flags, dispparams,
			    result, excepinfo, argerr);
}

STDMETHODIMP
CVim::GetHwnd(UINT_PTR *result)
{
    *result = (UINT_PTR)s_hwnd;
    أرجع S_OK;
}

STDMETHODIMP
CVim::SetForeground(فراغ)
{
    /* Make the Vim window come to the foreground */
    gui_mch_set_foreground();
    أرجع S_OK;
}

STDMETHODIMP
CVim::SendKeys(BSTR keys)
{
    صحيح len;
    حرف *buffer;
    char_u *str;
    char_u *ptr;

    /* Get a suitable buffer */
    len = WideCharToMultiByte(CP_ACP, 0, keys, -1, 0, 0, 0, 0);
    buffer = (حرف *)alloc(len+1);

    𐡄𐡍 (buffer == عدم)
	أرجع E_OUTOFMEMORY;

    len = WideCharToMultiByte(CP_ACP, 0, keys, -1, buffer, len, 0, 0);

    𐡄𐡍 (len == 0)
    {
	vim_free(buffer);
	أرجع E_INVALIDARG;
    }

    /* Translate key codes like <Esc> */
    str = replace_termcodes((char_u *)buffer, &ptr, 0, REPTERM_DO_LT, عدم);

    /* If ptr was set, then a new buffer was allocated,
     * so we can free the old one.
     */
    𐡄𐡍 (ptr)
	vim_free((char_u *)(buffer));

    /* Reject strings too long to fit in the input buffer. Allow 10 bytes
     * space to cover for the (remote) possibility that characters may enter
     * the input buffer between now and when the WM_OLE message is actually
     * processed. If more than 10 characters enter the input buffer in that
     * time, the WM_OLE processing will simply fail to insert the characters.
     */
    𐡄𐡍 ((صحيح)(STRLEN(str)) > (vim_free_in_input_buf() - 10))
    {
	vim_free(str);
	أرجع E_INVALIDARG;
    }

    /* Pass the string to the main input loop. The memory will be freed when
     * the message is processed.  Except for an empty message, we don't need
     * to post it then.
     */
    𐡄𐡍 (*str == NUL)
	vim_free(str);
    وإلا
	PostMessage(عدم, WM_OLE, 0, (LPARAM)str);

    أرجع S_OK;
}

STDMETHODIMP
CVim::Eval(BSTR expr, BSTR *result)
{
#ifdef FEAT_EVAL
    صحيح len;
    حرف *buffer;
    حرف *str;
    wchar_t *w_buffer;

    /* Get a suitable buffer */
    len = WideCharToMultiByte(CP_ACP, 0, expr, -1, 0, 0, 0, 0);
    𐡄𐡍 (len == 0)
	أرجع E_INVALIDARG;

    buffer = (حرف *)alloc(len);

    𐡄𐡍 (buffer == عدم)
	أرجع E_OUTOFMEMORY;

    /* Convert the (wide character) expression to an ASCII string */
    len = WideCharToMultiByte(CP_ACP, 0, expr, -1, buffer, len, 0, 0);
    𐡄𐡍 (len == 0)
	أرجع E_INVALIDARG;

    /* Evaluate the expression */
    ++emsg_skip;
    str = (حرف *)eval_to_string((char_u *)buffer, TRUE, FALSE);
    --emsg_skip;
    vim_free(buffer);
    𐡄𐡍 (str == عدم)
	أرجع E_FAIL;

    /* Convert the result to wide characters */
    MultiByteToWideChar_alloc(CP_ACP, 0, str, -1, &w_buffer, &len);
    vim_free(str);
    𐡄𐡍 (w_buffer == عدم)
	أرجع E_OUTOFMEMORY;

    𐡄𐡍 (len == 0)
    {
	vim_free(w_buffer);
	أرجع E_FAIL;
    }

    /* Store the result */
    *result = SysAllocString(w_buffer);
    vim_free(w_buffer);

    أرجع S_OK;
#وإلا
    أرجع E_NOTIMPL;
#endif
}

/*****************************************************************************
 3. The class factory
*****************************************************************************/

/* Definition
 * ----------
 */

صنف CVimCF FINAL : عام IClassFactory
{
عام:
    ساكن CVimCF *Create();
    virtual ~CVimCF() {};

    STDMETHOD(QueryInterface)(REFIID riid, فراغ ** ppv);
    STDMETHOD_(unsigned long, AddRef)(فراغ);
    STDMETHOD_(unsigned long, Release)(فراغ);
    STDMETHOD(CreateInstance)(IUnknown *punkOuter, REFIID riid, فراغ ** ppv);
    STDMETHOD(LockServer)(BOOL lock);

خاص:
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
    CVimCF *me = جديد CVimCF();

    𐡄𐡍 (me == عدم)
	MessageBox(0, "Cannot create class factory", "Vim Initialisation", 0);

    أرجع me;
}

STDMETHODIMP
CVimCF::QueryInterface(REFIID riid, فراغ **ppv)
{
    𐡄𐡍 (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	AddRef();
	*ppv = هذا;
	أرجع S_OK;
    }

    *ppv = 0;
    أرجع E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVimCF::AddRef()
{
    أرجع ++ref;
}

STDMETHODIMP_(ULONG)
CVimCF::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    𐡄𐡍 (ref > 0)
	--ref;
    أرجع ref;
}

/*ARGSUSED*/
STDMETHODIMP
CVimCF::CreateInstance(IUnknown *punkOuter, REFIID riid, فراغ **ppv)
{
    أرجع app->QueryInterface(riid, ppv);
}

/*ARGSUSED*/
STDMETHODIMP
CVimCF::LockServer(BOOL lock)
{
    أرجع S_OK;
}

/*****************************************************************************
 4. Registry manipulation code
*****************************************************************************/

// Internal use only
ساكن فراغ SetKeyAndValue(ثابت حرف *path, ثابت حرف *subkey, ثابت حرف *value);
ساكن فراغ GUIDtochar(ثابت GUID &guid, حرف *GUID, صحيح length);
ساكن فراغ RecursiveDeleteKey(HKEY hKeyParent, ثابت حرف *child);
ساكن ثابت صحيح GUID_STRING_SIZE = 39;

// Register the component in the registry
// When "silent" is TRUE don't give any messages.

خارجي "C" فراغ RegisterMe(صحيح silent)
{
    BOOL ok = TRUE;

    // Get the application startup command
    حرف module[MAX_PATH];

    ::GetModuleFileName(عدم, module, MAX_PATH);

    // Unregister first (quietly)
    UnregisterMe(FALSE);

    // Convert the CLSID into a char
    حرف clsid[GUID_STRING_SIZE];
    GUIDtochar(MYCLSID, clsid, حجم(clsid));

    // Convert the LIBID into a char
    حرف libid[GUID_STRING_SIZE];
    GUIDtochar(MYLIBID, libid, حجم(libid));

    // Build the key CLSID\\{...}
    حرف Key[MAX_CLSID_LEN];
    strcpy(Key, "CLSID\\");
    strcat(Key, clsid);

    // Add the CLSID to the registry
    SetKeyAndValue(Key, عدم, MYNAME);
    SetKeyAndValue(Key, "LocalServer32", module);
    SetKeyAndValue(Key, "ProgID", MYPROGID);
    SetKeyAndValue(Key, "VersionIndependentProgID", MYVIPROGID);
    SetKeyAndValue(Key, "TypeLib", libid);

    // Add the version-independent ProgID subkey under HKEY_CLASSES_ROOT
    SetKeyAndValue(MYVIPROGID, عدم, MYNAME);
    SetKeyAndValue(MYVIPROGID, "CLSID", clsid);
    SetKeyAndValue(MYVIPROGID, "CurVer", MYPROGID);

    // Add the versioned ProgID subkey under HKEY_CLASSES_ROOT
    SetKeyAndValue(MYPROGID, عدم, MYNAME);
    SetKeyAndValue(MYPROGID, "CLSID", clsid);

    wchar_t w_module[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, module, -1, w_module, MAX_PATH);

    ITypeLib *typelib = عدم;
    𐡄𐡍 (LoadTypeLib(w_module, &typelib) != S_OK)
    {
	𐡄𐡍 (!silent)
	    MessageBox(0, "Cannot load type library to register",
						       "Vim Registration", 0);
	ok = FALSE;
    }
    وإلا
    {
	𐡄𐡍 (RegisterTypeLib(typelib, w_module, عدم) != S_OK)
	{
	    𐡄𐡍 (!silent)
		MessageBox(0, "Cannot register type library",
						       "Vim Registration", 0);
	    ok = FALSE;
	}
	typelib->Release();
    }

    𐡄𐡍 (ok && !silent)
	MessageBox(0, "Registered successfully", "Vim", 0);
}

// Remove the component from the registry
//
// Note: There is little error checking in this code, to allow incomplete
// or failed registrations to be undone.
خارجي "C" فراغ UnregisterMe(صحيح bNotifyUser)
{
    // Unregister the type library
    ITypeLib *typelib;
    𐡄𐡍 (SUCCEEDED(LoadRegTypeLib(MYLIBID, MAJORVER, MINORVER, LOCALE, &typelib)))
    {
	TLIBATTR *tla;
	𐡄𐡍 (SUCCEEDED(typelib->GetLibAttr(&tla)))
	{
	    UnRegisterTypeLib(tla->guid, tla->wMajorVerNum, tla->wMinorVerNum,
			      tla->lcid, tla->syskind);
	    typelib->ReleaseTLibAttr(tla);
	}
	typelib->Release();
    }

    // Convert the CLSID into a char
    حرف clsid[GUID_STRING_SIZE];
    GUIDtochar(MYCLSID, clsid, حجم(clsid));

    // Build the key CLSID\\{...}
    حرف Key[MAX_CLSID_LEN];
    strcpy(Key, "CLSID\\");
    strcat(Key, clsid);

    // Delete the CLSID Key - CLSID\{...}
    RecursiveDeleteKey(HKEY_CLASSES_ROOT, Key);

    // Delete the version-independent ProgID Key
    RecursiveDeleteKey(HKEY_CLASSES_ROOT, MYVIPROGID);

    // Delete the ProgID key
    RecursiveDeleteKey(HKEY_CLASSES_ROOT, MYPROGID);

    𐡄𐡍 (bNotifyUser)
	MessageBox(0, "Unregistered successfully", "Vim", 0);
}

/****************************************************************************/

// Convert a GUID to a char string
ساكن فراغ GUIDtochar(ثابت GUID &guid, حرف *GUID, صحيح length)
{
    // Get wide string version
    LPOLESTR wGUID = عدم;
    StringFromCLSID(guid, &wGUID);

    // Convert from wide characters to non-wide
    wcstombs(GUID, wGUID, length);

    // Free memory
    CoTaskMemFree(wGUID);
}

// Delete a key and all of its descendants
ساكن فراغ RecursiveDeleteKey(HKEY hKeyParent, ثابت حرف *child)
{
    // Open the child
    HKEY hKeyChild;
    LONG result = RegOpenKeyEx(hKeyParent, child, 0,
			       KEY_ALL_ACCESS, &hKeyChild);
    𐡄𐡍 (result != ERROR_SUCCESS)
	أرجع;

    // Enumerate all of the descendants of this child
    FILETIME time;
    حرف buffer[1024];
    DWORD size = 1024;

    طالما (RegEnumKeyEx(hKeyChild, 0, buffer, &size, عدم,
			عدم, عدم, &time) == S_OK)
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
ساكن فراغ SetKeyAndValue(ثابت حرف *key, ثابت حرف *subkey, ثابت حرف *value)
{
    HKEY hKey;
    حرف buffer[1024];

    strcpy(buffer, key);

    // Add subkey name to buffer.
    𐡄𐡍 (subkey)
    {
	strcat(buffer, "\\");
	strcat(buffer, subkey);
    }

    // Create and open key and subkey.
    long result = RegCreateKeyEx(HKEY_CLASSES_ROOT,
				 buffer,
				 0, عدم, REG_OPTION_NON_VOLATILE,
				 KEY_ALL_ACCESS, عدم,
				 &hKey, عدم);
    𐡄𐡍 (result != ERROR_SUCCESS)
	أرجع;

    // Set the value
    𐡄𐡍 (value)
	RegSetValueEx(hKey, عدم, 0, REG_SZ, (BYTE *)value,
		      (DWORD)STRLEN(value)+1);

    RegCloseKey(hKey);
}

/*****************************************************************************
 5. OLE Initialisation and shutdown processing
*****************************************************************************/
خارجي "C" فراغ InitOLE(صحيح *pbDoRestart)
{
    HRESULT hr;

    *pbDoRestart = FALSE;

    // Initialize the OLE libraries
    hr = OleInitialize(عدم);
    𐡄𐡍 (FAILED(hr))
    {
	MessageBox(0, "Cannot initialise OLE", "Vim Initialisation", 0);
	اذهب error0;
    }

    // Create the application object
    app = CVim::Create(pbDoRestart);
    𐡄𐡍 (app == عدم)
	اذهب error1;

    // Create the class factory
    cf = CVimCF::Create();
    𐡄𐡍 (cf == عدم)
	اذهب error1;

    // Register the class factory
    hr = CoRegisterClassObject(
	MYCLSID,
	cf,
	CLSCTX_LOCAL_SERVER,
	REGCLS_MULTIPLEUSE,
	&cf_id);

    𐡄𐡍 (FAILED(hr))
    {
	MessageBox(0, "Cannot register class factory", "Vim Initialisation", 0);
	اذهب error1;
    }

    // Register the application object as active
    hr = RegisterActiveObject(
	app,
	MYCLSID,
	0,
	&app_id);

    𐡄𐡍 (FAILED(hr))
    {
	MessageBox(0, "Cannot register application object", "Vim Initialisation", 0);
	اذهب error1;
    }

    أرجع;

    // Errors: tidy up as much as needed and return
error1:
    UninitOLE();
error0:
    أرجع;
}

خارجي "C" فراغ UninitOLE()
{
    // Unregister the application object
    𐡄𐡍 (app_id)
    {
	RevokeActiveObject(app_id, عدم);
	app_id = 0;
    }

    // Unregister the class factory
    𐡄𐡍 (cf_id)
    {
	CoRevokeClassObject(cf_id);
	cf_id = 0;
    }

    // Shut down the OLE libraries
    OleUninitialize();

    // Delete the application object
    𐡄𐡍 (app)
    {
	delete app;
	app = عدم;
    }

    // Delete the class factory
    𐡄𐡍 (cf)
    {
	delete cf;
	cf = عدم;
    }
}
#endif /* FEAT_OLE */
