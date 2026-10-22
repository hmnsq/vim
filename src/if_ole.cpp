/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#إذا defined(FEAT_OLE) && defined(FEAT_GUI_MSWIN)
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

#إذا (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
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
    STDMETHOD(Get_Type_Info_تعداد)(UINT *p_تعداد);
    STDMETHOD(GetTypeInfo)(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo);
    STDMETHOD(GetIDsOfNames)(ثابت IID &iid, OLECHAR **names, UINT n, LCID, DISPID *dispids);
    STDMETHOD(Invoke)(DISPID member, ثابت IID &iid, LCID, WORD flags, DISPPARAMS *dispparams, VARIANT *result, EXCEPINFO *excepinfo, UINT *argerr);

    // IVim members
    STDMETHOD(Send_مفاتيح)(BSTR مفاتيح);
    STDMETHOD(Eval)(BSTR تعبير, BSTR *result);
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
    إذا (me == عدم)
    {
	MessageBox(0, "Cannot create application object", "Vim Initialisation", 0);
	إرجاع عدم;
    }

    // Load the type library from the registry
    hr = LoadRegTypeLib(MYLIBID, 1, 0, 0x00, &typelib);
    إذا (FAILED(hr))
    {
	HKEY h_مفتاح;

	// Check we can write to the registry.
	// RegCreateKeyEx succeeds even if key exists. W.Briscoe W2K 20021011
	إذا (Reg_Create_مفتاح_Ex(HKEY_CLASSES_جذر, MYVIPROGID, 0, عدم,
		  REG_OPTION_NON_VOLATILE,
		  مفتاح_ALL_ACCESS, عدم, &h_مفتاح, عدم))
	{
	    delete me;
	    إرجاع عدم; // Unable to write to registry. Quietly fail.
	}
	Reg_Close_مفتاح(h_مفتاح);

	إذا (MessageBox(0, "Cannot load registered type library.\nDo you want to register Vim now?",
		    "Vim Initialisation", MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
	    delete me;
	    إرجاع عدم;
	}

	RegisterMe(FALSE);

	// Load the type library from the registry
	hr = LoadRegTypeLib(MYLIBID, 1, 0, 0x00, &typelib);
	إذا (FAILED(hr))
	{
	    MessageBox(0, "You must restart Vim in order for the registration to take effect.",
						     "Vim Initialisation", 0);
	    *pbDoRestart = TRUE;
	    delete me;
	    إرجاع عدم;
	}
    }

    // Get the type info of the vtable interface
    hr = typelib->GetTypeInfoOfGuid(MYIID, &typeinfo);
    typelib->Release();

    إذا (FAILED(hr))
    {
	MessageBox(0, "Cannot get interface type information",
						     "Vim Initialisation", 0);
	delete me;
	إرجاع عدم;
    }

    // Save the type information
    me->typeinfo = typeinfo;
    إرجاع me;
}

CVim::~CVim()
{
    إذا (typeinfo && vim_parent_hwnd == عدم)
	typeinfo->Release();
    typeinfo = 0;
}

STDMETHODIMP
CVim::QueryInterface(REFIID riid, فراغ **ppv)
{
    إذا (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDispatch) || IsEqualIID(riid, MYIID))
    {
	AddRef();
	*ppv = هذا;
	إرجاع S_OK;
    }

    *ppv = 0;
    إرجاع E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVim::AddRef()
{
    إرجاع ++ref;
}

STDMETHODIMP_(ULONG)
CVim::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    إذا (ref > 0)
	--ref;
    إرجاع ref;
}

STDMETHODIMP
CVim::Get_Type_Info_تعداد(UINT *p_تعداد)
{
    *p_تعداد = 1;
    إرجاع S_OK;
}

STDMETHODIMP
CVim::GetTypeInfo(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo)
{
    *ppITypeInfo = 0;

    إذا (iTypeInfo != 0)
	إرجاع DISP_E_BADINDEX;

    typeinfo->AddRef();
    *ppITypeInfo = typeinfo;
    إرجاع S_OK;
}

STDMETHODIMP
CVim::GetIDsOfNames(
	ثابت IID &iid,
	OLECHAR **names,
	UINT n,
	LCID,
	DISPID *dispids)
{
    إذا (iid != IID_NULL)
	إرجاع DISP_E_UNKNOWNINTERFACE;

    إرجاع typeinfo->GetIDsOfNames(names, n, dispids);
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
    إذا (iid != IID_NULL)
	إرجاع DISP_E_UNKNOWNINTERFACE;

    ::SetErrorInfo(0, عدم);
    إرجاع typeinfo->Invoke(static_cast<IDispatch*>(هذا),
			    member, flags, dispparams,
			    result, excepinfo, argerr);
}

STDMETHODIMP
CVim::GetHwnd(UINT_PTR *result)
{
    *result = (UINT_PTR)s_hwnd;
    إرجاع S_OK;
}

STDMETHODIMP
CVim::SetForeground(فراغ)
{
    /* Make the Vim window come to the foreground */
    gui_mch_set_foreground();
    إرجاع S_OK;
}

STDMETHODIMP
CVim::Send_مفاتيح(BSTR مفاتيح)
{
    صحيح len;
    حرف *buffer;
    char_u *str;
    char_u *ptr;

    /* Get a suitable buffer */
    len = WideCharToMultiByte(CP_ACP, 0, مفاتيح, -1, 0, 0, 0, 0);
    buffer = (حرف *)alloc(len+1);

    إذا (buffer == عدم)
	إرجاع E_OUTOFMEMORY;

    len = WideCharToMultiByte(CP_ACP, 0, مفاتيح, -1, buffer, len, 0, 0);

    إذا (len == 0)
    {
	vim_free(buffer);
	إرجاع E_INVALIDARG;
    }

    /* Translate key codes like <Esc> */
    str = replace_termcodes((char_u *)buffer, &ptr, 0, REPTERM_DO_LT, عدم);

    /* If ptr was set, then a new buffer was allocated,
     * so we can free the old one.
     */
    إذا (ptr)
	vim_free((char_u *)(buffer));

    /* Reject strings too long to fit in the input buffer. Allow 10 bytes
     * space to cover for the (remote) possibility that characters may enter
     * the input buffer between now and when the WM_OLE message is actually
     * processed. If more than 10 characters enter the input buffer in that
     * time, the WM_OLE processing will simply fail to insert the characters.
     */
    إذا ((صحيح)(STRLEN(str)) > (vim_free_in_إدخال_buf() - 10))
    {
	vim_free(str);
	إرجاع E_INVALIDARG;
    }

    /* Pass the string to the main input loop. The memory will be freed when
     * the message is processed.  Except for an empty message, we don't need
     * to post it then.
     */
    إذا (*str == NUL)
	vim_free(str);
    وإلا
	PostMessage(عدم, WM_OLE, 0, (LPARAM)str);

    إرجاع S_OK;
}

STDMETHODIMP
CVim::Eval(BSTR تعبير, BSTR *result)
{
#ifdef FEAT_EVAL
    صحيح len;
    حرف *buffer;
    حرف *str;
    wchar_t *w_buffer;

    /* Get a suitable buffer */
    len = WideCharToMultiByte(CP_ACP, 0, تعبير, -1, 0, 0, 0, 0);
    إذا (len == 0)
	إرجاع E_INVALIDARG;

    buffer = (حرف *)alloc(len);

    إذا (buffer == عدم)
	إرجاع E_OUTOFMEMORY;

    /* Convert the (wide character) expression to an ASCII string */
    len = WideCharToMultiByte(CP_ACP, 0, تعبير, -1, buffer, len, 0, 0);
    إذا (len == 0)
	إرجاع E_INVALIDARG;

    /* Evaluate the expression */
    ++emsg_skip;
    str = (حرف *)eval_to_string((char_u *)buffer, TRUE, FALSE);
    --emsg_skip;
    vim_free(buffer);
    إذا (str == عدم)
	إرجاع E_FAIL;

    /* Convert the result to wide characters */
    MultiByteToWideChar_alloc(CP_ACP, 0, str, -1, &w_buffer, &len);
    vim_free(str);
    إذا (w_buffer == عدم)
	إرجاع E_OUTOFMEMORY;

    إذا (len == 0)
    {
	vim_free(w_buffer);
	إرجاع E_FAIL;
    }

    /* Store the result */
    *result = SysAllocString(w_buffer);
    vim_free(w_buffer);

    إرجاع S_OK;
#وإلا
    إرجاع E_NOTIMPL;
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

    إذا (me == عدم)
	MessageBox(0, "Cannot create class factory", "Vim Initialisation", 0);

    إرجاع me;
}

STDMETHODIMP
CVimCF::QueryInterface(REFIID riid, فراغ **ppv)
{
    إذا (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	AddRef();
	*ppv = هذا;
	إرجاع S_OK;
    }

    *ppv = 0;
    إرجاع E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVimCF::AddRef()
{
    إرجاع ++ref;
}

STDMETHODIMP_(ULONG)
CVimCF::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    إذا (ref > 0)
	--ref;
    إرجاع ref;
}

/*ARGSUSED*/
STDMETHODIMP
CVimCF::CreateInstance(IUnknown *punkOuter, REFIID riid, فراغ **ppv)
{
    إرجاع app->QueryInterface(riid, ppv);
}

/*ARGSUSED*/
STDMETHODIMP
CVimCF::LockServer(BOOL lock)
{
    إرجاع S_OK;
}

/*****************************************************************************
 4. Registry manipulation code
*****************************************************************************/

// Internal use only
ساكن فراغ Set_مفتاح_And_قيمة(ثابت حرف *path, ثابت حرف *subkey, ثابت حرف *قيمة);
ساكن فراغ GUIDtochar(ثابت GUID &guid, حرف *GUID, صحيح length);
ساكن فراغ Recursive_Delete_مفتاح(HKEY h_مفتاح_Parent, ثابت حرف *child);
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
    حرف مفتاح_2[MAX_CLSID_LEN];
    strcpy(مفتاح_2, "CLSID\\");
    strcat(مفتاح_2, clsid);

    // Add the CLSID to the registry
    Set_مفتاح_And_قيمة(مفتاح_2, عدم, MYNAME);
    Set_مفتاح_And_قيمة(مفتاح_2, "LocalServer32", module);
    Set_مفتاح_And_قيمة(مفتاح_2, "ProgID", MYPROGID);
    Set_مفتاح_And_قيمة(مفتاح_2, "VersionIndependentProgID", MYVIPROGID);
    Set_مفتاح_And_قيمة(مفتاح_2, "TypeLib", libid);

    // Add the version-independent ProgID subkey under HKEY_CLASSES_ROOT
    Set_مفتاح_And_قيمة(MYVIPROGID, عدم, MYNAME);
    Set_مفتاح_And_قيمة(MYVIPROGID, "CLSID", clsid);
    Set_مفتاح_And_قيمة(MYVIPROGID, "CurVer", MYPROGID);

    // Add the versioned ProgID subkey under HKEY_CLASSES_ROOT
    Set_مفتاح_And_قيمة(MYPROGID, عدم, MYNAME);
    Set_مفتاح_And_قيمة(MYPROGID, "CLSID", clsid);

    wchar_t w_module[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, module, -1, w_module, MAX_PATH);

    ITypeLib *typelib = عدم;
    إذا (LoadTypeLib(w_module, &typelib) != S_OK)
    {
	إذا (!silent)
	    MessageBox(0, "Cannot load type library to register",
						       "Vim Registration", 0);
	ok = FALSE;
    }
    وإلا
    {
	إذا (RegisterTypeLib(typelib, w_module, عدم) != S_OK)
	{
	    إذا (!silent)
		MessageBox(0, "Cannot register type library",
						       "Vim Registration", 0);
	    ok = FALSE;
	}
	typelib->Release();
    }

    إذا (ok && !silent)
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
    إذا (SUCCEEDED(LoadRegTypeLib(MYLIBID, MAJORVER, MINORVER, LOCALE, &typelib)))
    {
	TLIBATTR *tla;
	إذا (SUCCEEDED(typelib->GetLibAttr(&tla)))
	{
	    UnRegisterTypeLib(tla->guid, tla->w_Major_Ver_رقم, tla->w_Minor_Ver_رقم,
			      tla->lcid, tla->syskind);
	    typelib->ReleaseTLibAttr(tla);
	}
	typelib->Release();
    }

    // Convert the CLSID into a char
    حرف clsid[GUID_STRING_SIZE];
    GUIDtochar(MYCLSID, clsid, حجم(clsid));

    // Build the key CLSID\\{...}
    حرف مفتاح_2[MAX_CLSID_LEN];
    strcpy(مفتاح_2, "CLSID\\");
    strcat(مفتاح_2, clsid);

    // Delete the CLSID Key - CLSID\{...}
    Recursive_Delete_مفتاح(HKEY_CLASSES_جذر, مفتاح_2);

    // Delete the version-independent ProgID Key
    Recursive_Delete_مفتاح(HKEY_CLASSES_جذر, MYVIPROGID);

    // Delete the ProgID key
    Recursive_Delete_مفتاح(HKEY_CLASSES_جذر, MYPROGID);

    إذا (bNotifyUser)
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
ساكن فراغ Recursive_Delete_مفتاح(HKEY h_مفتاح_Parent, ثابت حرف *child)
{
    // Open the child
    HKEY h_مفتاح_Child;
    LONG result = Reg_Open_مفتاح_Ex(h_مفتاح_Parent, child, 0,
			       مفتاح_ALL_ACCESS, &h_مفتاح_Child);
    إذا (result != ERROR_SUCCESS)
	إرجاع;

    // Enumerate all of the descendants of this child
    FILETIME time;
    حرف buffer[1024];
    DWORD size = 1024;

    بينما (Reg_Enum_مفتاح_Ex(h_مفتاح_Child, 0, buffer, &size, عدم,
			عدم, عدم, &time) == S_OK)
    {
	// Delete the descendants of this child
	Recursive_Delete_مفتاح(h_مفتاح_Child, buffer);
	size = 256;
    }

    // Close the child
    Reg_Close_مفتاح(h_مفتاح_Child);

    // Delete this child
    Reg_Delete_مفتاح(h_مفتاح_Parent, child);
}

// Create a key and set its value
ساكن فراغ Set_مفتاح_And_قيمة(ثابت حرف *مفتاح, ثابت حرف *subkey, ثابت حرف *قيمة)
{
    HKEY h_مفتاح;
    حرف buffer[1024];

    strcpy(buffer, مفتاح);

    // Add subkey name to buffer.
    إذا (subkey)
    {
	strcat(buffer, "\\");
	strcat(buffer, subkey);
    }

    // Create and open key and subkey.
    long result = Reg_Create_مفتاح_Ex(HKEY_CLASSES_جذر,
				 buffer,
				 0, عدم, REG_OPTION_NON_VOLATILE,
				 مفتاح_ALL_ACCESS, عدم,
				 &h_مفتاح, عدم);
    إذا (result != ERROR_SUCCESS)
	إرجاع;

    // Set the value
    إذا (قيمة)
	Reg_Set_قيمة_Ex(h_مفتاح, عدم, 0, REG_SZ, (BYTE *)قيمة,
		      (DWORD)STRLEN(قيمة)+1);

    Reg_Close_مفتاح(h_مفتاح);
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
    إذا (FAILED(hr))
    {
	MessageBox(0, "Cannot initialise OLE", "Vim Initialisation", 0);
	اذهب error0;
    }

    // Create the application object
    app = CVim::Create(pbDoRestart);
    إذا (app == عدم)
	اذهب error1;

    // Create the class factory
    cf = CVimCF::Create();
    إذا (cf == عدم)
	اذهب error1;

    // Register the class factory
    hr = CoRegisterClassObject(
	MYCLSID,
	cf,
	CLSCTX_LOCAL_SERVER,
	REGCLS_MULTIPLEUSE,
	&cf_id);

    إذا (FAILED(hr))
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

    إذا (FAILED(hr))
    {
	MessageBox(0, "Cannot register application object", "Vim Initialisation", 0);
	اذهب error1;
    }

    إرجاع;

    // Errors: tidy up as much as needed and return
error1:
    UninitOLE();
error0:
    إرجاع;
}

خارجي "C" فراغ UninitOLE()
{
    // Unregister the application object
    إذا (app_id)
    {
	RevokeActiveObject(app_id, عدم);
	app_id = 0;
    }

    // Unregister the class factory
    إذا (cf_id)
    {
	CoRevokeClassObject(cf_id);
	cf_id = 0;
    }

    // Shut down the OLE libraries
    OleUninitialize();

    // Delete the application object
    إذا (app)
    {
	delete app;
	app = عدم;
    }

    // Delete the class factory
    إذا (cf)
    {
	delete cf;
	cf = عدم;
    }
}
#endif /* FEAT_OLE */
