/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#اگر defined(FEAT_OLE) && defined(FEAT_GUI_MSWIN)
/*
 * OLE server implementation.
 *
 * See os_mswin.c for the client side.
 */
بیرونی "C" {
# include "vim.h"
}

#شامل <windows.h>
#شامل <oleauto.h>

بیرونی "C" {
بیرونی HWND s_hwnd;
بیرونی HWND vim_parent_hwnd;
}

#اگر (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#وگرنه
# define FINAL
#endif

#شامل "if_ole.h"	// Interface definitions
#شامل "iid_ole.c"	// UUID definitions (compile here)

/* Supply function prototype to work around bug in Mingw oleauto.h header */
#ifdef __MINGW32__
WINOLEAUTAPI UnRegisterTypeLib(REFGUID libID, WORD wVerMajor,
	    WORD wVerMinor, LCID lcid, SYSKIND syskind);
#endif

/*****************************************************************************
 1. Internal definitions for this file
*****************************************************************************/

رده CVim;
رده CVimCF;

/* Internal data */
// The identifier of the registered class factory
ایستا unsigned long cf_id = 0;

// The identifier of the running application object
ایستا unsigned long app_id = 0;

// The single global instance of the class factory
ایستا CVimCF *cf = 0;

// The single global instance of the application object
ایستا CVim *app = 0;

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

رده CVim FINAL : عمومی IVim
{
عمومی:
    مجازی ~CVim();
    ایستا CVim *Create(صحیح *pbDoRestart);

    // IUnknown members
    STDMETHOD(QueryInterface)(REFIID riid, تهی ** ppv);
    STDMETHOD_(unsigned long, AddRef)(تهی);
    STDMETHOD_(unsigned long, Release)(تهی);

    // IDispatch members
    STDMETHOD(Get_Type_Info_شمار)(UINT *p_شمار);
    STDMETHOD(GetTypeInfo)(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo);
    STDMETHOD(GetIDsOfNames)(ثابت IID &iid, OLECHAR **names, UINT n, LCID, DISPID *dispids);
    STDMETHOD(Invoke)(DISPID member, ثابت IID &iid, LCID, WORD flags, DISPPARAMS *dispparams, VARIANT *result, EXCEPINFO *excepinfo, UINT *argerr);

    // IVim members
    STDMETHOD(Send_کلیدها)(BSTR کلیدها);
    STDMETHOD(Eval)(BSTR عبارت, BSTR *result);
    STDMETHOD(SetForeground)(تهی);
    STDMETHOD(GetHwnd)(UINT_PTR *result);

خصوصی:
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

CVim *CVim::Create(صحیح *pbDoRestart)
{
    HRESULT hr;
    CVim *me = 0;
    ITypeLib *typelib = 0;
    ITypeInfo *typeinfo = 0;

    *pbDoRestart = FALSE;

    // Create the object
    me = جدید CVim();
    اگر (me == پوچ)
    {
	MessageBox(0, "Cannot create application object", "Vim Initialisation", 0);
	بازگشت پوچ;
    }

    // Load the type library from the registry
    hr = LoadRegTypeLib(MYLIBID, 1, 0, 0x00, &typelib);
    اگر (FAILED(hr))
    {
	HKEY h_کلید;

	// Check we can write to the registry.
	// RegCreateKeyEx succeeds even if key exists. W.Briscoe W2K 20021011
	اگر (Reg_Create_کلید_Ex(HKEY_CLASSES_ریشه, MYVIPROGID, 0, پوچ,
		  REG_OPTION_NON_VOLATILE,
		  کلید_ALL_ACCESS, پوچ, &h_کلید, پوچ))
	{
	    delete me;
	    بازگشت پوچ; // Unable to write to registry. Quietly fail.
	}
	Reg_Close_کلید(h_کلید);

	اگر (MessageBox(0, "Cannot load registered type library.\nDo you want to register Vim now?",
		    "Vim Initialisation", MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
	    delete me;
	    بازگشت پوچ;
	}

	RegisterMe(FALSE);

	// Load the type library from the registry
	hr = LoadRegTypeLib(MYLIBID, 1, 0, 0x00, &typelib);
	اگر (FAILED(hr))
	{
	    MessageBox(0, "You must restart Vim in order for the registration to take effect.",
						     "Vim Initialisation", 0);
	    *pbDoRestart = TRUE;
	    delete me;
	    بازگشت پوچ;
	}
    }

    // Get the type info of the vtable interface
    hr = typelib->GetTypeInfoOfGuid(MYIID, &typeinfo);
    typelib->Release();

    اگر (FAILED(hr))
    {
	MessageBox(0, "Cannot get interface type information",
						     "Vim Initialisation", 0);
	delete me;
	بازگشت پوچ;
    }

    // Save the type information
    me->typeinfo = typeinfo;
    بازگشت me;
}

CVim::~CVim()
{
    اگر (typeinfo && vim_parent_hwnd == پوچ)
	typeinfo->Release();
    typeinfo = 0;
}

STDMETHODIMP
CVim::QueryInterface(REFIID riid, تهی **ppv)
{
    اگر (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDispatch) || IsEqualIID(riid, MYIID))
    {
	AddRef();
	*ppv = این;
	بازگشت S_OK;
    }

    *ppv = 0;
    بازگشت E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVim::AddRef()
{
    بازگشت ++ref;
}

STDMETHODIMP_(ULONG)
CVim::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    اگر (ref > 0)
	--ref;
    بازگشت ref;
}

STDMETHODIMP
CVim::Get_Type_Info_شمار(UINT *p_شمار)
{
    *p_شمار = 1;
    بازگشت S_OK;
}

STDMETHODIMP
CVim::GetTypeInfo(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo)
{
    *ppITypeInfo = 0;

    اگر (iTypeInfo != 0)
	بازگشت DISP_E_BADINDEX;

    typeinfo->AddRef();
    *ppITypeInfo = typeinfo;
    بازگشت S_OK;
}

STDMETHODIMP
CVim::GetIDsOfNames(
	ثابت IID &iid,
	OLECHAR **names,
	UINT n,
	LCID,
	DISPID *dispids)
{
    اگر (iid != IID_NULL)
	بازگشت DISP_E_UNKNOWNINTERFACE;

    بازگشت typeinfo->GetIDsOfNames(names, n, dispids);
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
    اگر (iid != IID_NULL)
	بازگشت DISP_E_UNKNOWNINTERFACE;

    ::SetErrorInfo(0, پوچ);
    بازگشت typeinfo->Invoke(static_cast<IDispatch*>(این),
			    member, flags, dispparams,
			    result, excepinfo, argerr);
}

STDMETHODIMP
CVim::GetHwnd(UINT_PTR *result)
{
    *result = (UINT_PTR)s_hwnd;
    بازگشت S_OK;
}

STDMETHODIMP
CVim::SetForeground(تهی)
{
    /* Make the Vim window come to the foreground */
    gui_mch_set_foreground();
    بازگشت S_OK;
}

STDMETHODIMP
CVim::Send_کلیدها(BSTR کلیدها)
{
    صحیح len;
    نویسه *buffer;
    char_u *str;
    char_u *ptr;

    /* Get a suitable buffer */
    len = WideCharToMultiByte(CP_ACP, 0, کلیدها, -1, 0, 0, 0, 0);
    buffer = (نویسه *)alloc(len+1);

    اگر (buffer == پوچ)
	بازگشت E_OUTOFMEMORY;

    len = WideCharToMultiByte(CP_ACP, 0, کلیدها, -1, buffer, len, 0, 0);

    اگر (len == 0)
    {
	vim_free(buffer);
	بازگشت E_INVALIDARG;
    }

    /* Translate key codes like <Esc> */
    str = replace_termcodes((char_u *)buffer, &ptr, 0, REPTERM_DO_LT, پوچ);

    /* If ptr was set, then a new buffer was allocated,
     * so we can free the old one.
     */
    اگر (ptr)
	vim_free((char_u *)(buffer));

    /* Reject strings too long to fit in the input buffer. Allow 10 bytes
     * space to cover for the (remote) possibility that characters may enter
     * the input buffer between now and when the WM_OLE message is actually
     * processed. If more than 10 characters enter the input buffer in that
     * time, the WM_OLE processing will simply fail to insert the characters.
     */
    اگر ((صحیح)(STRLEN(str)) > (vim_free_in_درون_داد_buf() - 10))
    {
	vim_free(str);
	بازگشت E_INVALIDARG;
    }

    /* Pass the string to the main input loop. The memory will be freed when
     * the message is processed.  Except for an empty message, we don't need
     * to post it then.
     */
    اگر (*str == NUL)
	vim_free(str);
    وگرنه
	PostMessage(پوچ, WM_OLE, 0, (LPARAM)str);

    بازگشت S_OK;
}

STDMETHODIMP
CVim::Eval(BSTR عبارت, BSTR *result)
{
#ifdef FEAT_EVAL
    صحیح len;
    نویسه *buffer;
    نویسه *str;
    wchar_t *w_buffer;

    /* Get a suitable buffer */
    len = WideCharToMultiByte(CP_ACP, 0, عبارت, -1, 0, 0, 0, 0);
    اگر (len == 0)
	بازگشت E_INVALIDARG;

    buffer = (نویسه *)alloc(len);

    اگر (buffer == پوچ)
	بازگشت E_OUTOFMEMORY;

    /* Convert the (wide character) expression to an ASCII string */
    len = WideCharToMultiByte(CP_ACP, 0, عبارت, -1, buffer, len, 0, 0);
    اگر (len == 0)
	بازگشت E_INVALIDARG;

    /* Evaluate the expression */
    ++emsg_skip;
    str = (نویسه *)eval_to_string((char_u *)buffer, TRUE, FALSE);
    --emsg_skip;
    vim_free(buffer);
    اگر (str == پوچ)
	بازگشت E_FAIL;

    /* Convert the result to wide characters */
    MultiByteToWideChar_alloc(CP_ACP, 0, str, -1, &w_buffer, &len);
    vim_free(str);
    اگر (w_buffer == پوچ)
	بازگشت E_OUTOFMEMORY;

    اگر (len == 0)
    {
	vim_free(w_buffer);
	بازگشت E_FAIL;
    }

    /* Store the result */
    *result = SysAllocString(w_buffer);
    vim_free(w_buffer);

    بازگشت S_OK;
#وگرنه
    بازگشت E_NOTIMPL;
#endif
}

/*****************************************************************************
 3. The class factory
*****************************************************************************/

/* Definition
 * ----------
 */

رده CVimCF FINAL : عمومی IClassFactory
{
عمومی:
    ایستا CVimCF *Create();
    مجازی ~CVimCF() {};

    STDMETHOD(QueryInterface)(REFIID riid, تهی ** ppv);
    STDMETHOD_(unsigned long, AddRef)(تهی);
    STDMETHOD_(unsigned long, Release)(تهی);
    STDMETHOD(CreateInstance)(IUnknown *punkOuter, REFIID riid, تهی ** ppv);
    STDMETHOD(LockServer)(BOOL lock);

خصوصی:
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
    CVimCF *me = جدید CVimCF();

    اگر (me == پوچ)
	MessageBox(0, "Cannot create class factory", "Vim Initialisation", 0);

    بازگشت me;
}

STDMETHODIMP
CVimCF::QueryInterface(REFIID riid, تهی **ppv)
{
    اگر (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	AddRef();
	*ppv = این;
	بازگشت S_OK;
    }

    *ppv = 0;
    بازگشت E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVimCF::AddRef()
{
    بازگشت ++ref;
}

STDMETHODIMP_(ULONG)
CVimCF::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    اگر (ref > 0)
	--ref;
    بازگشت ref;
}

/*ARGSUSED*/
STDMETHODIMP
CVimCF::CreateInstance(IUnknown *punkOuter, REFIID riid, تهی **ppv)
{
    بازگشت app->QueryInterface(riid, ppv);
}

/*ARGSUSED*/
STDMETHODIMP
CVimCF::LockServer(BOOL lock)
{
    بازگشت S_OK;
}

/*****************************************************************************
 4. Registry manipulation code
*****************************************************************************/

// Internal use only
ایستا تهی Set_کلید_And_مقدار(ثابت نویسه *path, ثابت نویسه *subkey, ثابت نویسه *مقدار);
ایستا تهی GUIDtochar(ثابت GUID &guid, نویسه *GUID, صحیح length);
ایستا تهی Recursive_Delete_کلید(HKEY h_کلید_Parent, ثابت نویسه *child);
ایستا ثابت صحیح GUID_STRING_SIZE = 39;

// Register the component in the registry
// When "silent" is TRUE don't give any messages.

بیرونی "C" تهی RegisterMe(صحیح silent)
{
    BOOL ok = TRUE;

    // Get the application startup command
    نویسه module[MAX_PATH];

    ::GetModuleFileName(پوچ, module, MAX_PATH);

    // Unregister first (quietly)
    UnregisterMe(FALSE);

    // Convert the CLSID into a char
    نویسه clsid[GUID_STRING_SIZE];
    GUIDtochar(MYCLSID, clsid, اندازه(clsid));

    // Convert the LIBID into a char
    نویسه libid[GUID_STRING_SIZE];
    GUIDtochar(MYLIBID, libid, اندازه(libid));

    // Build the key CLSID\\{...}
    نویسه کلید_2[MAX_CLSID_LEN];
    strcpy(کلید_2, "CLSID\\");
    strcat(کلید_2, clsid);

    // Add the CLSID to the registry
    Set_کلید_And_مقدار(کلید_2, پوچ, MYNAME);
    Set_کلید_And_مقدار(کلید_2, "LocalServer32", module);
    Set_کلید_And_مقدار(کلید_2, "ProgID", MYPROGID);
    Set_کلید_And_مقدار(کلید_2, "VersionIndependentProgID", MYVIPROGID);
    Set_کلید_And_مقدار(کلید_2, "TypeLib", libid);

    // Add the version-independent ProgID subkey under HKEY_CLASSES_ROOT
    Set_کلید_And_مقدار(MYVIPROGID, پوچ, MYNAME);
    Set_کلید_And_مقدار(MYVIPROGID, "CLSID", clsid);
    Set_کلید_And_مقدار(MYVIPROGID, "CurVer", MYPROGID);

    // Add the versioned ProgID subkey under HKEY_CLASSES_ROOT
    Set_کلید_And_مقدار(MYPROGID, پوچ, MYNAME);
    Set_کلید_And_مقدار(MYPROGID, "CLSID", clsid);

    wchar_t w_module[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, module, -1, w_module, MAX_PATH);

    ITypeLib *typelib = پوچ;
    اگر (LoadTypeLib(w_module, &typelib) != S_OK)
    {
	اگر (!silent)
	    MessageBox(0, "Cannot load type library to register",
						       "Vim Registration", 0);
	ok = FALSE;
    }
    وگرنه
    {
	اگر (RegisterTypeLib(typelib, w_module, پوچ) != S_OK)
	{
	    اگر (!silent)
		MessageBox(0, "Cannot register type library",
						       "Vim Registration", 0);
	    ok = FALSE;
	}
	typelib->Release();
    }

    اگر (ok && !silent)
	MessageBox(0, "Registered successfully", "Vim", 0);
}

// Remove the component from the registry
//
// Note: There is little error checking in this code, to allow incomplete
// or failed registrations to be undone.
بیرونی "C" تهی UnregisterMe(صحیح bNotifyUser)
{
    // Unregister the type library
    ITypeLib *typelib;
    اگر (SUCCEEDED(LoadRegTypeLib(MYLIBID, MAJORVER, MINORVER, LOCALE, &typelib)))
    {
	TLIBATTR *tla;
	اگر (SUCCEEDED(typelib->GetLibAttr(&tla)))
	{
	    UnRegisterTypeLib(tla->guid, tla->w_Major_Ver_رقم, tla->w_Minor_Ver_رقم,
			      tla->lcid, tla->syskind);
	    typelib->ReleaseTLibAttr(tla);
	}
	typelib->Release();
    }

    // Convert the CLSID into a char
    نویسه clsid[GUID_STRING_SIZE];
    GUIDtochar(MYCLSID, clsid, اندازه(clsid));

    // Build the key CLSID\\{...}
    نویسه کلید_2[MAX_CLSID_LEN];
    strcpy(کلید_2, "CLSID\\");
    strcat(کلید_2, clsid);

    // Delete the CLSID Key - CLSID\{...}
    Recursive_Delete_کلید(HKEY_CLASSES_ریشه, کلید_2);

    // Delete the version-independent ProgID Key
    Recursive_Delete_کلید(HKEY_CLASSES_ریشه, MYVIPROGID);

    // Delete the ProgID key
    Recursive_Delete_کلید(HKEY_CLASSES_ریشه, MYPROGID);

    اگر (bNotifyUser)
	MessageBox(0, "Unregistered successfully", "Vim", 0);
}

/****************************************************************************/

// Convert a GUID to a char string
ایستا تهی GUIDtochar(ثابت GUID &guid, نویسه *GUID, صحیح length)
{
    // Get wide string version
    LPOLESTR wGUID = پوچ;
    StringFromCLSID(guid, &wGUID);

    // Convert from wide characters to non-wide
    wcstombs(GUID, wGUID, length);

    // Free memory
    CoTaskMemFree(wGUID);
}

// Delete a key and all of its descendants
ایستا تهی Recursive_Delete_کلید(HKEY h_کلید_Parent, ثابت نویسه *child)
{
    // Open the child
    HKEY h_کلید_Child;
    LONG result = Reg_Open_کلید_Ex(h_کلید_Parent, child, 0,
			       کلید_ALL_ACCESS, &h_کلید_Child);
    اگر (result != ERROR_SUCCESS)
	بازگشت;

    // Enumerate all of the descendants of this child
    FILETIME time;
    نویسه buffer[1024];
    DWORD size = 1024;

    وقتی (Reg_Enum_کلید_Ex(h_کلید_Child, 0, buffer, &size, پوچ,
			پوچ, پوچ, &time) == S_OK)
    {
	// Delete the descendants of this child
	Recursive_Delete_کلید(h_کلید_Child, buffer);
	size = 256;
    }

    // Close the child
    Reg_Close_کلید(h_کلید_Child);

    // Delete this child
    Reg_Delete_کلید(h_کلید_Parent, child);
}

// Create a key and set its value
ایستا تهی Set_کلید_And_مقدار(ثابت نویسه *کلید, ثابت نویسه *subkey, ثابت نویسه *مقدار)
{
    HKEY h_کلید;
    نویسه buffer[1024];

    strcpy(buffer, کلید);

    // Add subkey name to buffer.
    اگر (subkey)
    {
	strcat(buffer, "\\");
	strcat(buffer, subkey);
    }

    // Create and open key and subkey.
    long result = Reg_Create_کلید_Ex(HKEY_CLASSES_ریشه,
				 buffer,
				 0, پوچ, REG_OPTION_NON_VOLATILE,
				 کلید_ALL_ACCESS, پوچ,
				 &h_کلید, پوچ);
    اگر (result != ERROR_SUCCESS)
	بازگشت;

    // Set the value
    اگر (مقدار)
	Reg_Set_مقدار_Ex(h_کلید, پوچ, 0, REG_SZ, (BYTE *)مقدار,
		      (DWORD)STRLEN(مقدار)+1);

    Reg_Close_کلید(h_کلید);
}

/*****************************************************************************
 5. OLE Initialisation and shutdown processing
*****************************************************************************/
بیرونی "C" تهی InitOLE(صحیح *pbDoRestart)
{
    HRESULT hr;

    *pbDoRestart = FALSE;

    // Initialize the OLE libraries
    hr = OleInitialize(پوچ);
    اگر (FAILED(hr))
    {
	MessageBox(0, "Cannot initialise OLE", "Vim Initialisation", 0);
	برو error0;
    }

    // Create the application object
    app = CVim::Create(pbDoRestart);
    اگر (app == پوچ)
	برو error1;

    // Create the class factory
    cf = CVimCF::Create();
    اگر (cf == پوچ)
	برو error1;

    // Register the class factory
    hr = CoRegisterClassObject(
	MYCLSID,
	cf,
	CLSCTX_LOCAL_SERVER,
	REGCLS_MULTIPLEUSE,
	&cf_id);

    اگر (FAILED(hr))
    {
	MessageBox(0, "Cannot register class factory", "Vim Initialisation", 0);
	برو error1;
    }

    // Register the application object as active
    hr = RegisterActiveObject(
	app,
	MYCLSID,
	0,
	&app_id);

    اگر (FAILED(hr))
    {
	MessageBox(0, "Cannot register application object", "Vim Initialisation", 0);
	برو error1;
    }

    بازگشت;

    // Errors: tidy up as much as needed and return
error1:
    UninitOLE();
error0:
    بازگشت;
}

بیرونی "C" تهی UninitOLE()
{
    // Unregister the application object
    اگر (app_id)
    {
	RevokeActiveObject(app_id, پوچ);
	app_id = 0;
    }

    // Unregister the class factory
    اگر (cf_id)
    {
	CoRevokeClassObject(cf_id);
	cf_id = 0;
    }

    // Shut down the OLE libraries
    OleUninitialize();

    // Delete the application object
    اگر (app)
    {
	delete app;
	app = پوچ;
    }

    // Delete the class factory
    اگر (cf)
    {
	delete cf;
	cf = پوچ;
    }
}
#endif /* FEAT_OLE */
