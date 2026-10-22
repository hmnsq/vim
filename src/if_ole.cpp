/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#जर defined(FEAT_OLE) && defined(FEAT_GUI_MSWIN)
/*
 * OLE server implementation.
 *
 * See os_mswin.c for the client side.
 */
बाह्य "C" {
# include "vim.h"
}

#अन्तर्भाव <windows.h>
#अन्तर्भाव <oleauto.h>

बाह्य "C" {
बाह्य HWND s_hwnd;
बाह्य HWND vim_parent_hwnd;
}

#जर (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#अन्यथा
# define FINAL
#endif

#अन्तर्भाव "if_ole.h"	// Interface definitions
#अन्तर्भाव "iid_ole.c"	// UUID definitions (compile here)

/* Supply function prototype to work around bug in Mingw oleauto.h header */
#ifdef __MINGW32__
WINOLEAUTAPI UnRegisterTypeLib(REFGUID libID, WORD wVerMajor,
	    WORD wVerMinor, LCID lcid, SYSKIND syskind);
#endif

/*****************************************************************************
 1. Internal definitions for this file
*****************************************************************************/

वर्ग CVim;
वर्ग CVimCF;

/* Internal data */
// The identifier of the registered class factory
स्थिर अचिन्हित दीर्घ cf_id = 0;

// The identifier of the running application object
स्थिर अचिन्हित दीर्घ app_id = 0;

// The single global instance of the class factory
स्थिर CVimCF *cf = 0;

// The single global instance of the application object
स्थिर CVim *app = 0;

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

वर्ग CVim FINAL : सार्वजनिक IVim
{
सार्वजनिक:
    आभासी ~CVim();
    स्थिर CVim *Create(पूर्णांक *pbDoRestart);

    // IUnknown members
    STDMETHOD(QueryInterface)(REFIID riid, शून्य ** ppv);
    STDMETHOD_(अचिन्हित दीर्घ, AddRef)(शून्य);
    STDMETHOD_(अचिन्हित दीर्घ, Release)(शून्य);

    // IDispatch members
    STDMETHOD(Get_Type_Info_मोजणी)(UINT *p_मोजणी);
    STDMETHOD(GetTypeInfo)(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo);
    STDMETHOD(GetIDsOfNames)(स्थिरांक IID &iid, OLECHAR **names, UINT n, LCID, DISPID *dispids);
    STDMETHOD(Invoke)(DISPID member, स्थिरांक IID &iid, LCID, WORD flags, DISPPARAMS *dispparams, VARIANT *result, EXCEPINFO *excepinfo, UINT *argerr);

    // IVim members
    STDMETHOD(Send_किल्ल्या)(BSTR किल्ल्या);
    STDMETHOD(Eval)(BSTR अभिव्यक्ती, BSTR *result);
    STDMETHOD(SetForeground)(शून्य);
    STDMETHOD(GetHwnd)(UINT_PTR *result);

खाजगी:
    // Constructor is private - create using CVim::Create()
    CVim() : ref(0), typeinfo(0) {};

    // Reference count
    अचिन्हित दीर्घ ref;

    // The object's TypeInfo
    ITypeInfo *typeinfo;
};

/* Implementation
 * --------------
 */

CVim *CVim::Create(पूर्णांक *pbDoRestart)
{
    HRESULT hr;
    CVim *me = 0;
    ITypeLib *typelib = 0;
    ITypeInfo *typeinfo = 0;

    *pbDoRestart = FALSE;

    // Create the object
    me = नवीन CVim();
    जर (me == रिक्त)
    {
	MessageBox(0, "Cannot create application object", "Vim Initialisation", 0);
	परत रिक्त;
    }

    // Load the type library from the registry
    hr = LoadRegTypeLib(MYLIBID, 1, 0, 0x00, &typelib);
    जर (FAILED(hr))
    {
	HKEY h_किल्ली;

	// Check we can write to the registry.
	// RegCreateKeyEx succeeds even if key exists. W.Briscoe W2K 20021011
	जर (Reg_Create_किल्ली_Ex(HKEY_CLASSES_मूळ, MYVIPROGID, 0, रिक्त,
		  REG_OPTION_NON_VOLATILE,
		  किल्ली_ALL_ACCESS, रिक्त, &h_किल्ली, रिक्त))
	{
	    delete me;
	    परत रिक्त; // Unable to write to registry. Quietly fail.
	}
	Reg_Close_किल्ली(h_किल्ली);

	जर (MessageBox(0, "Cannot load registered type library.\nDo you want to register Vim now?",
		    "Vim Initialisation", MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
	    delete me;
	    परत रिक्त;
	}

	RegisterMe(FALSE);

	// Load the type library from the registry
	hr = LoadRegTypeLib(MYLIBID, 1, 0, 0x00, &typelib);
	जर (FAILED(hr))
	{
	    MessageBox(0, "You must restart Vim in order for the registration to take effect.",
						     "Vim Initialisation", 0);
	    *pbDoRestart = TRUE;
	    delete me;
	    परत रिक्त;
	}
    }

    // Get the type info of the vtable interface
    hr = typelib->GetTypeInfoOfGuid(MYIID, &typeinfo);
    typelib->Release();

    जर (FAILED(hr))
    {
	MessageBox(0, "Cannot get interface type information",
						     "Vim Initialisation", 0);
	delete me;
	परत रिक्त;
    }

    // Save the type information
    me->typeinfo = typeinfo;
    परत me;
}

CVim::~CVim()
{
    जर (typeinfo && vim_parent_hwnd == रिक्त)
	typeinfo->Release();
    typeinfo = 0;
}

STDMETHODIMP
CVim::QueryInterface(REFIID riid, शून्य **ppv)
{
    जर (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDispatch) || IsEqualIID(riid, MYIID))
    {
	AddRef();
	*ppv = हे;
	परत S_OK;
    }

    *ppv = 0;
    परत E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVim::AddRef()
{
    परत ++ref;
}

STDMETHODIMP_(ULONG)
CVim::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    जर (ref > 0)
	--ref;
    परत ref;
}

STDMETHODIMP
CVim::Get_Type_Info_मोजणी(UINT *p_मोजणी)
{
    *p_मोजणी = 1;
    परत S_OK;
}

STDMETHODIMP
CVim::GetTypeInfo(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo)
{
    *ppITypeInfo = 0;

    जर (iTypeInfo != 0)
	परत DISP_E_BADINDEX;

    typeinfo->AddRef();
    *ppITypeInfo = typeinfo;
    परत S_OK;
}

STDMETHODIMP
CVim::GetIDsOfNames(
	स्थिरांक IID &iid,
	OLECHAR **names,
	UINT n,
	LCID,
	DISPID *dispids)
{
    जर (iid != IID_NULL)
	परत DISP_E_UNKNOWNINTERFACE;

    परत typeinfo->GetIDsOfNames(names, n, dispids);
}

STDMETHODIMP
CVim::Invoke(
	DISPID member,
	स्थिरांक IID &iid,
	LCID,
	WORD flags,
	DISPPARAMS *dispparams,
	VARIANT *result,
	EXCEPINFO *excepinfo,
	UINT *argerr)
{
    जर (iid != IID_NULL)
	परत DISP_E_UNKNOWNINTERFACE;

    ::SetErrorInfo(0, रिक्त);
    परत typeinfo->Invoke(static_cast<IDispatch*>(हे),
			    member, flags, dispparams,
			    result, excepinfo, argerr);
}

STDMETHODIMP
CVim::GetHwnd(UINT_PTR *result)
{
    *result = (UINT_PTR)s_hwnd;
    परत S_OK;
}

STDMETHODIMP
CVim::SetForeground(शून्य)
{
    /* Make the Vim window come to the foreground */
    gui_mch_set_foreground();
    परत S_OK;
}

STDMETHODIMP
CVim::Send_किल्ल्या(BSTR किल्ल्या)
{
    पूर्णांक len;
    अक्षर *buffer;
    char_u *str;
    char_u *ptr;

    /* Get a suitable buffer */
    len = WideCharToMultiByte(CP_ACP, 0, किल्ल्या, -1, 0, 0, 0, 0);
    buffer = (अक्षर *)alloc(len+1);

    जर (buffer == रिक्त)
	परत E_OUTOFMEMORY;

    len = WideCharToMultiByte(CP_ACP, 0, किल्ल्या, -1, buffer, len, 0, 0);

    जर (len == 0)
    {
	vim_free(buffer);
	परत E_INVALIDARG;
    }

    /* Translate key codes like <Esc> */
    str = replace_termcodes((char_u *)buffer, &ptr, 0, REPTERM_DO_LT, रिक्त);

    /* If ptr was set, then a new buffer was allocated,
     * so we can free the old one.
     */
    जर (ptr)
	vim_free((char_u *)(buffer));

    /* Reject strings too long to fit in the input buffer. Allow 10 bytes
     * space to cover for the (remote) possibility that characters may enter
     * the input buffer between now and when the WM_OLE message is actually
     * processed. If more than 10 characters enter the input buffer in that
     * time, the WM_OLE processing will simply fail to insert the characters.
     */
    जर ((पूर्णांक)(STRLEN(str)) > (vim_free_in_आदान_buf() - 10))
    {
	vim_free(str);
	परत E_INVALIDARG;
    }

    /* Pass the string to the main input loop. The memory will be freed when
     * the message is processed.  Except for an empty message, we don't need
     * to post it then.
     */
    जर (*str == NUL)
	vim_free(str);
    अन्यथा
	PostMessage(रिक्त, WM_OLE, 0, (LPARAM)str);

    परत S_OK;
}

STDMETHODIMP
CVim::Eval(BSTR अभिव्यक्ती, BSTR *result)
{
#ifdef FEAT_EVAL
    पूर्णांक len;
    अक्षर *buffer;
    अक्षर *str;
    ब_अक्षर_प्रकार *w_buffer;

    /* Get a suitable buffer */
    len = WideCharToMultiByte(CP_ACP, 0, अभिव्यक्ती, -1, 0, 0, 0, 0);
    जर (len == 0)
	परत E_INVALIDARG;

    buffer = (अक्षर *)alloc(len);

    जर (buffer == रिक्त)
	परत E_OUTOFMEMORY;

    /* Convert the (wide character) expression to an ASCII string */
    len = WideCharToMultiByte(CP_ACP, 0, अभिव्यक्ती, -1, buffer, len, 0, 0);
    जर (len == 0)
	परत E_INVALIDARG;

    /* Evaluate the expression */
    ++emsg_skip;
    str = (अक्षर *)eval_to_string((char_u *)buffer, TRUE, FALSE);
    --emsg_skip;
    vim_free(buffer);
    जर (str == रिक्त)
	परत E_FAIL;

    /* Convert the result to wide characters */
    MultiByteToWideChar_alloc(CP_ACP, 0, str, -1, &w_buffer, &len);
    vim_free(str);
    जर (w_buffer == रिक्त)
	परत E_OUTOFMEMORY;

    जर (len == 0)
    {
	vim_free(w_buffer);
	परत E_FAIL;
    }

    /* Store the result */
    *result = SysAllocString(w_buffer);
    vim_free(w_buffer);

    परत S_OK;
#अन्यथा
    परत E_NOTIMPL;
#endif
}

/*****************************************************************************
 3. The class factory
*****************************************************************************/

/* Definition
 * ----------
 */

वर्ग CVimCF FINAL : सार्वजनिक IClassFactory
{
सार्वजनिक:
    स्थिर CVimCF *Create();
    आभासी ~CVimCF() {};

    STDMETHOD(QueryInterface)(REFIID riid, शून्य ** ppv);
    STDMETHOD_(अचिन्हित दीर्घ, AddRef)(शून्य);
    STDMETHOD_(अचिन्हित दीर्घ, Release)(शून्य);
    STDMETHOD(CreateInstance)(IUnknown *punkOuter, REFIID riid, शून्य ** ppv);
    STDMETHOD(LockServer)(BOOL lock);

खाजगी:
    // Constructor is private - create via Create()
    CVimCF() : ref(0) {};

    // Reference count
    अचिन्हित दीर्घ ref;
};

/* Implementation
 * --------------
 */

CVimCF *CVimCF::Create()
{
    CVimCF *me = नवीन CVimCF();

    जर (me == रिक्त)
	MessageBox(0, "Cannot create class factory", "Vim Initialisation", 0);

    परत me;
}

STDMETHODIMP
CVimCF::QueryInterface(REFIID riid, शून्य **ppv)
{
    जर (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	AddRef();
	*ppv = हे;
	परत S_OK;
    }

    *ppv = 0;
    परत E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVimCF::AddRef()
{
    परत ++ref;
}

STDMETHODIMP_(ULONG)
CVimCF::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    जर (ref > 0)
	--ref;
    परत ref;
}

/*ARGSUSED*/
STDMETHODIMP
CVimCF::CreateInstance(IUnknown *punkOuter, REFIID riid, शून्य **ppv)
{
    परत app->QueryInterface(riid, ppv);
}

/*ARGSUSED*/
STDMETHODIMP
CVimCF::LockServer(BOOL lock)
{
    परत S_OK;
}

/*****************************************************************************
 4. Registry manipulation code
*****************************************************************************/

// Internal use only
स्थिर शून्य Set_किल्ली_And_मूल्य(स्थिरांक अक्षर *path, स्थिरांक अक्षर *subkey, स्थिरांक अक्षर *मूल्य);
स्थिर शून्य GUIDtochar(स्थिरांक GUID &guid, अक्षर *GUID, पूर्णांक length);
स्थिर शून्य Recursive_Delete_किल्ली(HKEY h_किल्ली_Parent, स्थिरांक अक्षर *child);
स्थिर स्थिरांक पूर्णांक GUID_STRING_SIZE = 39;

// Register the component in the registry
// When "silent" is TRUE don't give any messages.

बाह्य "C" शून्य RegisterMe(पूर्णांक silent)
{
    BOOL ok = TRUE;

    // Get the application startup command
    अक्षर module[MAX_PATH];

    ::GetModuleFileName(रिक्त, module, MAX_PATH);

    // Unregister first (quietly)
    UnregisterMe(FALSE);

    // Convert the CLSID into a char
    अक्षर clsid[GUID_STRING_SIZE];
    GUIDtochar(MYCLSID, clsid, आकार(clsid));

    // Convert the LIBID into a char
    अक्षर libid[GUID_STRING_SIZE];
    GUIDtochar(MYLIBID, libid, आकार(libid));

    // Build the key CLSID\\{...}
    अक्षर किल्ली_2[MAX_CLSID_LEN];
    म_नकल(किल्ली_2, "CLSID\\");
    म_जोड़ो(किल्ली_2, clsid);

    // Add the CLSID to the registry
    Set_किल्ली_And_मूल्य(किल्ली_2, रिक्त, MYNAME);
    Set_किल्ली_And_मूल्य(किल्ली_2, "LocalServer32", module);
    Set_किल्ली_And_मूल्य(किल्ली_2, "ProgID", MYPROGID);
    Set_किल्ली_And_मूल्य(किल्ली_2, "VersionIndependentProgID", MYVIPROGID);
    Set_किल्ली_And_मूल्य(किल्ली_2, "TypeLib", libid);

    // Add the version-independent ProgID subkey under HKEY_CLASSES_ROOT
    Set_किल्ली_And_मूल्य(MYVIPROGID, रिक्त, MYNAME);
    Set_किल्ली_And_मूल्य(MYVIPROGID, "CLSID", clsid);
    Set_किल्ली_And_मूल्य(MYVIPROGID, "CurVer", MYPROGID);

    // Add the versioned ProgID subkey under HKEY_CLASSES_ROOT
    Set_किल्ली_And_मूल्य(MYPROGID, रिक्त, MYNAME);
    Set_किल्ली_And_मूल्य(MYPROGID, "CLSID", clsid);

    ब_अक्षर_प्रकार w_module[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, module, -1, w_module, MAX_PATH);

    ITypeLib *typelib = रिक्त;
    जर (LoadTypeLib(w_module, &typelib) != S_OK)
    {
	जर (!silent)
	    MessageBox(0, "Cannot load type library to register",
						       "Vim Registration", 0);
	ok = FALSE;
    }
    अन्यथा
    {
	जर (RegisterTypeLib(typelib, w_module, रिक्त) != S_OK)
	{
	    जर (!silent)
		MessageBox(0, "Cannot register type library",
						       "Vim Registration", 0);
	    ok = FALSE;
	}
	typelib->Release();
    }

    जर (ok && !silent)
	MessageBox(0, "Registered successfully", "Vim", 0);
}

// Remove the component from the registry
//
// Note: There is little error checking in this code, to allow incomplete
// or failed registrations to be undone.
बाह्य "C" शून्य UnregisterMe(पूर्णांक bNotifyUser)
{
    // Unregister the type library
    ITypeLib *typelib;
    जर (SUCCEEDED(LoadRegTypeLib(MYLIBID, MAJORVER, MINORVER, LOCALE, &typelib)))
    {
	TLIBATTR *tla;
	जर (SUCCEEDED(typelib->GetLibAttr(&tla)))
	{
	    UnRegisterTypeLib(tla->guid, tla->w_Major_Ver_अंक, tla->w_Minor_Ver_अंक,
			      tla->lcid, tla->syskind);
	    typelib->ReleaseTLibAttr(tla);
	}
	typelib->Release();
    }

    // Convert the CLSID into a char
    अक्षर clsid[GUID_STRING_SIZE];
    GUIDtochar(MYCLSID, clsid, आकार(clsid));

    // Build the key CLSID\\{...}
    अक्षर किल्ली_2[MAX_CLSID_LEN];
    म_नकल(किल्ली_2, "CLSID\\");
    म_जोड़ो(किल्ली_2, clsid);

    // Delete the CLSID Key - CLSID\{...}
    Recursive_Delete_किल्ली(HKEY_CLASSES_मूळ, किल्ली_2);

    // Delete the version-independent ProgID Key
    Recursive_Delete_किल्ली(HKEY_CLASSES_मूळ, MYVIPROGID);

    // Delete the ProgID key
    Recursive_Delete_किल्ली(HKEY_CLASSES_मूळ, MYPROGID);

    जर (bNotifyUser)
	MessageBox(0, "Unregistered successfully", "Vim", 0);
}

/****************************************************************************/

// Convert a GUID to a char string
स्थिर शून्य GUIDtochar(स्थिरांक GUID &guid, अक्षर *GUID, पूर्णांक length)
{
    // Get wide string version
    LPOLESTR wGUID = रिक्त;
    StringFromCLSID(guid, &wGUID);

    // Convert from wide characters to non-wide
    wcstombs(GUID, wGUID, length);

    // Free memory
    CoTaskMemFree(wGUID);
}

// Delete a key and all of its descendants
स्थिर शून्य Recursive_Delete_किल्ली(HKEY h_किल्ली_Parent, स्थिरांक अक्षर *child)
{
    // Open the child
    HKEY h_किल्ली_Child;
    LONG result = Reg_Open_किल्ली_Ex(h_किल्ली_Parent, child, 0,
			       किल्ली_ALL_ACCESS, &h_किल्ली_Child);
    जर (result != ERROR_SUCCESS)
	परत;

    // Enumerate all of the descendants of this child
    FILETIME समय;
    अक्षर buffer[1024];
    DWORD size = 1024;

    पर्यंत (Reg_Enum_किल्ली_Ex(h_किल्ली_Child, 0, buffer, &size, रिक्त,
			रिक्त, रिक्त, &समय) == S_OK)
    {
	// Delete the descendants of this child
	Recursive_Delete_किल्ली(h_किल्ली_Child, buffer);
	size = 256;
    }

    // Close the child
    Reg_Close_किल्ली(h_किल्ली_Child);

    // Delete this child
    Reg_Delete_किल्ली(h_किल्ली_Parent, child);
}

// Create a key and set its value
स्थिर शून्य Set_किल्ली_And_मूल्य(स्थिरांक अक्षर *किल्ली, स्थिरांक अक्षर *subkey, स्थिरांक अक्षर *मूल्य)
{
    HKEY h_किल्ली;
    अक्षर buffer[1024];

    म_नकल(buffer, किल्ली);

    // Add subkey name to buffer.
    जर (subkey)
    {
	म_जोड़ो(buffer, "\\");
	म_जोड़ो(buffer, subkey);
    }

    // Create and open key and subkey.
    दीर्घ result = Reg_Create_किल्ली_Ex(HKEY_CLASSES_मूळ,
				 buffer,
				 0, रिक्त, REG_OPTION_NON_VOLATILE,
				 किल्ली_ALL_ACCESS, रिक्त,
				 &h_किल्ली, रिक्त);
    जर (result != ERROR_SUCCESS)
	परत;

    // Set the value
    जर (मूल्य)
	Reg_Set_मूल्य_Ex(h_किल्ली, रिक्त, 0, REG_SZ, (BYTE *)मूल्य,
		      (DWORD)STRLEN(मूल्य)+1);

    Reg_Close_किल्ली(h_किल्ली);
}

/*****************************************************************************
 5. OLE Initialisation and shutdown processing
*****************************************************************************/
बाह्य "C" शून्य InitOLE(पूर्णांक *pbDoRestart)
{
    HRESULT hr;

    *pbDoRestart = FALSE;

    // Initialize the OLE libraries
    hr = OleInitialize(रिक्त);
    जर (FAILED(hr))
    {
	MessageBox(0, "Cannot initialise OLE", "Vim Initialisation", 0);
	जा error0;
    }

    // Create the application object
    app = CVim::Create(pbDoRestart);
    जर (app == रिक्त)
	जा error1;

    // Create the class factory
    cf = CVimCF::Create();
    जर (cf == रिक्त)
	जा error1;

    // Register the class factory
    hr = CoRegisterClassObject(
	MYCLSID,
	cf,
	CLSCTX_LOCAL_SERVER,
	REGCLS_MULTIPLEUSE,
	&cf_id);

    जर (FAILED(hr))
    {
	MessageBox(0, "Cannot register class factory", "Vim Initialisation", 0);
	जा error1;
    }

    // Register the application object as active
    hr = RegisterActiveObject(
	app,
	MYCLSID,
	0,
	&app_id);

    जर (FAILED(hr))
    {
	MessageBox(0, "Cannot register application object", "Vim Initialisation", 0);
	जा error1;
    }

    परत;

    // Errors: tidy up as much as needed and return
error1:
    UninitOLE();
error0:
    परत;
}

बाह्य "C" शून्य UninitOLE()
{
    // Unregister the application object
    जर (app_id)
    {
	RevokeActiveObject(app_id, रिक्त);
	app_id = 0;
    }

    // Unregister the class factory
    जर (cf_id)
    {
	CoRevokeClassObject(cf_id);
	cf_id = 0;
    }

    // Shut down the OLE libraries
    OleUninitialize();

    // Delete the application object
    जर (app)
    {
	delete app;
	app = रिक्त;
    }

    // Delete the class factory
    जर (cf)
    {
	delete cf;
	cf = रिक्त;
    }
}
#endif /* FEAT_OLE */
