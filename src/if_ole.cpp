/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#यदि defined(FEAT_OLE) && defined(FEAT_GUI_MSWIN)
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

#यदि (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
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
स्थिर unsigned long cf_id = 0;

// The identifier of the running application object
स्थिर unsigned long app_id = 0;

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
    आभास ~CVim();
    स्थिर CVim *Create(पूर्णाङ्क *pbDoRestart);

    // IUnknown members
    STDMETHOD(QueryInterface)(REFIID riid, शून्य ** ppv);
    STDMETHOD_(unsigned long, AddRef)(शून्य);
    STDMETHOD_(unsigned long, Release)(शून्य);

    // IDispatch members
    STDMETHOD(Get_Type_Info_गिनती)(UINT *p_गिनती);
    STDMETHOD(GetTypeInfo)(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo);
    STDMETHOD(GetIDsOfNames)(ध्रुव IID &iid, OLECHAR **names, UINT n, LCID, DISPID *dispids);
    STDMETHOD(Invoke)(DISPID member, ध्रुव IID &iid, LCID, WORD flags, DISPPARAMS *dispparams, VARIANT *result, EXCEPINFO *excepinfo, UINT *argerr);

    // IVim members
    STDMETHOD(Send_कुंजियाँ)(BSTR कुंजियाँ);
    STDMETHOD(Eval)(BSTR व्यंजक, BSTR *result);
    STDMETHOD(SetForeground)(शून्य);
    STDMETHOD(GetHwnd)(UINT_PTR *result);

गुप्त:
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

CVim *CVim::Create(पूर्णाङ्क *pbDoRestart)
{
    HRESULT hr;
    CVim *me = 0;
    ITypeLib *typelib = 0;
    ITypeInfo *typeinfo = 0;

    *pbDoRestart = FALSE;

    // Create the object
    me = नूतन CVim();
    यदि (me == रिक्त)
    {
	MessageBox(0, "Cannot create application object", "Vim Initialisation", 0);
	प्रत्यागम रिक्त;
    }

    // Load the type library from the registry
    hr = LoadRegTypeLib(MYLIBID, 1, 0, 0x00, &typelib);
    यदि (FAILED(hr))
    {
	HKEY h_कुंजी;

	// Check we can write to the registry.
	// RegCreateKeyEx succeeds even if key exists. W.Briscoe W2K 20021011
	यदि (Reg_Create_कुंजी_Ex(HKEY_CLASSES_मूल, MYVIPROGID, 0, रिक्त,
		  REG_OPTION_NON_VOLATILE,
		  कुंजी_ALL_ACCESS, रिक्त, &h_कुंजी, रिक्त))
	{
	    delete me;
	    प्रत्यागम रिक्त; // Unable to write to registry. Quietly fail.
	}
	Reg_Close_कुंजी(h_कुंजी);

	यदि (MessageBox(0, "Cannot load registered type library.\nDo you want to register Vim now?",
		    "Vim Initialisation", MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
	    delete me;
	    प्रत्यागम रिक्त;
	}

	RegisterMe(FALSE);

	// Load the type library from the registry
	hr = LoadRegTypeLib(MYLIBID, 1, 0, 0x00, &typelib);
	यदि (FAILED(hr))
	{
	    MessageBox(0, "You must restart Vim in order for the registration to take effect.",
						     "Vim Initialisation", 0);
	    *pbDoRestart = TRUE;
	    delete me;
	    प्रत्यागम रिक्त;
	}
    }

    // Get the type info of the vtable interface
    hr = typelib->GetTypeInfoOfGuid(MYIID, &typeinfo);
    typelib->Release();

    यदि (FAILED(hr))
    {
	MessageBox(0, "Cannot get interface type information",
						     "Vim Initialisation", 0);
	delete me;
	प्रत्यागम रिक्त;
    }

    // Save the type information
    me->typeinfo = typeinfo;
    प्रत्यागम me;
}

CVim::~CVim()
{
    यदि (typeinfo && vim_parent_hwnd == रिक्त)
	typeinfo->Release();
    typeinfo = 0;
}

STDMETHODIMP
CVim::QueryInterface(REFIID riid, शून्य **ppv)
{
    यदि (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDispatch) || IsEqualIID(riid, MYIID))
    {
	AddRef();
	*ppv = एतत्;
	प्रत्यागम S_OK;
    }

    *ppv = 0;
    प्रत्यागम E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVim::AddRef()
{
    प्रत्यागम ++ref;
}

STDMETHODIMP_(ULONG)
CVim::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    यदि (ref > 0)
	--ref;
    प्रत्यागम ref;
}

STDMETHODIMP
CVim::Get_Type_Info_गिनती(UINT *p_गिनती)
{
    *p_गिनती = 1;
    प्रत्यागम S_OK;
}

STDMETHODIMP
CVim::GetTypeInfo(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo)
{
    *ppITypeInfo = 0;

    यदि (iTypeInfo != 0)
	प्रत्यागम DISP_E_BADINDEX;

    typeinfo->AddRef();
    *ppITypeInfo = typeinfo;
    प्रत्यागम S_OK;
}

STDMETHODIMP
CVim::GetIDsOfNames(
	ध्रुव IID &iid,
	OLECHAR **names,
	UINT n,
	LCID,
	DISPID *dispids)
{
    यदि (iid != IID_NULL)
	प्रत्यागम DISP_E_UNKNOWNINTERFACE;

    प्रत्यागम typeinfo->GetIDsOfNames(names, n, dispids);
}

STDMETHODIMP
CVim::Invoke(
	DISPID member,
	ध्रुव IID &iid,
	LCID,
	WORD flags,
	DISPPARAMS *dispparams,
	VARIANT *result,
	EXCEPINFO *excepinfo,
	UINT *argerr)
{
    यदि (iid != IID_NULL)
	प्रत्यागम DISP_E_UNKNOWNINTERFACE;

    ::SetErrorInfo(0, रिक्त);
    प्रत्यागम typeinfo->Invoke(static_cast<IDispatch*>(एतत्),
			    member, flags, dispparams,
			    result, excepinfo, argerr);
}

STDMETHODIMP
CVim::GetHwnd(UINT_PTR *result)
{
    *result = (UINT_PTR)s_hwnd;
    प्रत्यागम S_OK;
}

STDMETHODIMP
CVim::SetForeground(शून्य)
{
    /* Make the Vim window come to the foreground */
    gui_mch_set_foreground();
    प्रत्यागम S_OK;
}

STDMETHODIMP
CVim::Send_कुंजियाँ(BSTR कुंजियाँ)
{
    पूर्णाङ्क len;
    वर्ण *buffer;
    char_u *str;
    char_u *ptr;

    /* Get a suitable buffer */
    len = WideCharToMultiByte(CP_ACP, 0, कुंजियाँ, -1, 0, 0, 0, 0);
    buffer = (वर्ण *)alloc(len+1);

    यदि (buffer == रिक्त)
	प्रत्यागम E_OUTOFMEMORY;

    len = WideCharToMultiByte(CP_ACP, 0, कुंजियाँ, -1, buffer, len, 0, 0);

    यदि (len == 0)
    {
	vim_free(buffer);
	प्रत्यागम E_INVALIDARG;
    }

    /* Translate key codes like <Esc> */
    str = replace_termcodes((char_u *)buffer, &ptr, 0, REPTERM_DO_LT, रिक्त);

    /* If ptr was set, then a new buffer was allocated,
     * so we can free the old one.
     */
    यदि (ptr)
	vim_free((char_u *)(buffer));

    /* Reject strings too long to fit in the input buffer. Allow 10 bytes
     * space to cover for the (remote) possibility that characters may enter
     * the input buffer between now and when the WM_OLE message is actually
     * processed. If more than 10 characters enter the input buffer in that
     * time, the WM_OLE processing will simply fail to insert the characters.
     */
    यदि ((पूर्णाङ्क)(STRLEN(str)) > (vim_free_in_निवेश_buf() - 10))
    {
	vim_free(str);
	प्रत्यागम E_INVALIDARG;
    }

    /* Pass the string to the main input loop. The memory will be freed when
     * the message is processed.  Except for an empty message, we don't need
     * to post it then.
     */
    यदि (*str == NUL)
	vim_free(str);
    अन्यथा
	PostMessage(रिक्त, WM_OLE, 0, (LPARAM)str);

    प्रत्यागम S_OK;
}

STDMETHODIMP
CVim::Eval(BSTR व्यंजक, BSTR *result)
{
#ifdef FEAT_EVAL
    पूर्णाङ्क len;
    वर्ण *buffer;
    वर्ण *str;
    wchar_t *w_buffer;

    /* Get a suitable buffer */
    len = WideCharToMultiByte(CP_ACP, 0, व्यंजक, -1, 0, 0, 0, 0);
    यदि (len == 0)
	प्रत्यागम E_INVALIDARG;

    buffer = (वर्ण *)alloc(len);

    यदि (buffer == रिक्त)
	प्रत्यागम E_OUTOFMEMORY;

    /* Convert the (wide character) expression to an ASCII string */
    len = WideCharToMultiByte(CP_ACP, 0, व्यंजक, -1, buffer, len, 0, 0);
    यदि (len == 0)
	प्रत्यागम E_INVALIDARG;

    /* Evaluate the expression */
    ++emsg_skip;
    str = (वर्ण *)eval_to_string((char_u *)buffer, TRUE, FALSE);
    --emsg_skip;
    vim_free(buffer);
    यदि (str == रिक्त)
	प्रत्यागम E_FAIL;

    /* Convert the result to wide characters */
    MultiByteToWideChar_alloc(CP_ACP, 0, str, -1, &w_buffer, &len);
    vim_free(str);
    यदि (w_buffer == रिक्त)
	प्रत्यागम E_OUTOFMEMORY;

    यदि (len == 0)
    {
	vim_free(w_buffer);
	प्रत्यागम E_FAIL;
    }

    /* Store the result */
    *result = SysAllocString(w_buffer);
    vim_free(w_buffer);

    प्रत्यागम S_OK;
#अन्यथा
    प्रत्यागम E_NOTIMPL;
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
    आभास ~CVimCF() {};

    STDMETHOD(QueryInterface)(REFIID riid, शून्य ** ppv);
    STDMETHOD_(unsigned long, AddRef)(शून्य);
    STDMETHOD_(unsigned long, Release)(शून्य);
    STDMETHOD(CreateInstance)(IUnknown *punkOuter, REFIID riid, शून्य ** ppv);
    STDMETHOD(LockServer)(BOOL lock);

गुप्त:
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
    CVimCF *me = नूतन CVimCF();

    यदि (me == रिक्त)
	MessageBox(0, "Cannot create class factory", "Vim Initialisation", 0);

    प्रत्यागम me;
}

STDMETHODIMP
CVimCF::QueryInterface(REFIID riid, शून्य **ppv)
{
    यदि (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	AddRef();
	*ppv = एतत्;
	प्रत्यागम S_OK;
    }

    *ppv = 0;
    प्रत्यागम E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVimCF::AddRef()
{
    प्रत्यागम ++ref;
}

STDMETHODIMP_(ULONG)
CVimCF::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    यदि (ref > 0)
	--ref;
    प्रत्यागम ref;
}

/*ARGSUSED*/
STDMETHODIMP
CVimCF::CreateInstance(IUnknown *punkOuter, REFIID riid, शून्य **ppv)
{
    प्रत्यागम app->QueryInterface(riid, ppv);
}

/*ARGSUSED*/
STDMETHODIMP
CVimCF::LockServer(BOOL lock)
{
    प्रत्यागम S_OK;
}

/*****************************************************************************
 4. Registry manipulation code
*****************************************************************************/

// Internal use only
स्थिर शून्य Set_कुंजी_And_मूल्य(ध्रुव वर्ण *path, ध्रुव वर्ण *subkey, ध्रुव वर्ण *मूल्य);
स्थिर शून्य GUIDtochar(ध्रुव GUID &guid, वर्ण *GUID, पूर्णाङ्क length);
स्थिर शून्य Recursive_Delete_कुंजी(HKEY h_कुंजी_Parent, ध्रुव वर्ण *child);
स्थिर ध्रुव पूर्णाङ्क GUID_STRING_SIZE = 39;

// Register the component in the registry
// When "silent" is TRUE don't give any messages.

बाह्य "C" शून्य RegisterMe(पूर्णाङ्क silent)
{
    BOOL ok = TRUE;

    // Get the application startup command
    वर्ण module[MAX_PATH];

    ::GetModuleFileName(रिक्त, module, MAX_PATH);

    // Unregister first (quietly)
    UnregisterMe(FALSE);

    // Convert the CLSID into a char
    वर्ण clsid[GUID_STRING_SIZE];
    GUIDtochar(MYCLSID, clsid, परिमाण(clsid));

    // Convert the LIBID into a char
    वर्ण libid[GUID_STRING_SIZE];
    GUIDtochar(MYLIBID, libid, परिमाण(libid));

    // Build the key CLSID\\{...}
    वर्ण कुंजी_2[MAX_CLSID_LEN];
    strcpy(कुंजी_2, "CLSID\\");
    strcat(कुंजी_2, clsid);

    // Add the CLSID to the registry
    Set_कुंजी_And_मूल्य(कुंजी_2, रिक्त, MYNAME);
    Set_कुंजी_And_मूल्य(कुंजी_2, "LocalServer32", module);
    Set_कुंजी_And_मूल्य(कुंजी_2, "ProgID", MYPROGID);
    Set_कुंजी_And_मूल्य(कुंजी_2, "VersionIndependentProgID", MYVIPROGID);
    Set_कुंजी_And_मूल्य(कुंजी_2, "TypeLib", libid);

    // Add the version-independent ProgID subkey under HKEY_CLASSES_ROOT
    Set_कुंजी_And_मूल्य(MYVIPROGID, रिक्त, MYNAME);
    Set_कुंजी_And_मूल्य(MYVIPROGID, "CLSID", clsid);
    Set_कुंजी_And_मूल्य(MYVIPROGID, "CurVer", MYPROGID);

    // Add the versioned ProgID subkey under HKEY_CLASSES_ROOT
    Set_कुंजी_And_मूल्य(MYPROGID, रिक्त, MYNAME);
    Set_कुंजी_And_मूल्य(MYPROGID, "CLSID", clsid);

    wchar_t w_module[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, module, -1, w_module, MAX_PATH);

    ITypeLib *typelib = रिक्त;
    यदि (LoadTypeLib(w_module, &typelib) != S_OK)
    {
	यदि (!silent)
	    MessageBox(0, "Cannot load type library to register",
						       "Vim Registration", 0);
	ok = FALSE;
    }
    अन्यथा
    {
	यदि (RegisterTypeLib(typelib, w_module, रिक्त) != S_OK)
	{
	    यदि (!silent)
		MessageBox(0, "Cannot register type library",
						       "Vim Registration", 0);
	    ok = FALSE;
	}
	typelib->Release();
    }

    यदि (ok && !silent)
	MessageBox(0, "Registered successfully", "Vim", 0);
}

// Remove the component from the registry
//
// Note: There is little error checking in this code, to allow incomplete
// or failed registrations to be undone.
बाह्य "C" शून्य UnregisterMe(पूर्णाङ्क bNotifyUser)
{
    // Unregister the type library
    ITypeLib *typelib;
    यदि (SUCCEEDED(LoadRegTypeLib(MYLIBID, MAJORVER, MINORVER, LOCALE, &typelib)))
    {
	TLIBATTR *tla;
	यदि (SUCCEEDED(typelib->GetLibAttr(&tla)))
	{
	    UnRegisterTypeLib(tla->guid, tla->w_Major_Ver_अंक, tla->w_Minor_Ver_अंक,
			      tla->lcid, tla->syskind);
	    typelib->ReleaseTLibAttr(tla);
	}
	typelib->Release();
    }

    // Convert the CLSID into a char
    वर्ण clsid[GUID_STRING_SIZE];
    GUIDtochar(MYCLSID, clsid, परिमाण(clsid));

    // Build the key CLSID\\{...}
    वर्ण कुंजी_2[MAX_CLSID_LEN];
    strcpy(कुंजी_2, "CLSID\\");
    strcat(कुंजी_2, clsid);

    // Delete the CLSID Key - CLSID\{...}
    Recursive_Delete_कुंजी(HKEY_CLASSES_मूल, कुंजी_2);

    // Delete the version-independent ProgID Key
    Recursive_Delete_कुंजी(HKEY_CLASSES_मूल, MYVIPROGID);

    // Delete the ProgID key
    Recursive_Delete_कुंजी(HKEY_CLASSES_मूल, MYPROGID);

    यदि (bNotifyUser)
	MessageBox(0, "Unregistered successfully", "Vim", 0);
}

/****************************************************************************/

// Convert a GUID to a char string
स्थिर शून्य GUIDtochar(ध्रुव GUID &guid, वर्ण *GUID, पूर्णाङ्क length)
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
स्थिर शून्य Recursive_Delete_कुंजी(HKEY h_कुंजी_Parent, ध्रुव वर्ण *child)
{
    // Open the child
    HKEY h_कुंजी_Child;
    LONG result = Reg_Open_कुंजी_Ex(h_कुंजी_Parent, child, 0,
			       कुंजी_ALL_ACCESS, &h_कुंजी_Child);
    यदि (result != ERROR_SUCCESS)
	प्रत्यागम;

    // Enumerate all of the descendants of this child
    FILETIME time;
    वर्ण buffer[1024];
    DWORD size = 1024;

    यावत् (Reg_Enum_कुंजी_Ex(h_कुंजी_Child, 0, buffer, &size, रिक्त,
			रिक्त, रिक्त, &time) == S_OK)
    {
	// Delete the descendants of this child
	Recursive_Delete_कुंजी(h_कुंजी_Child, buffer);
	size = 256;
    }

    // Close the child
    Reg_Close_कुंजी(h_कुंजी_Child);

    // Delete this child
    Reg_Delete_कुंजी(h_कुंजी_Parent, child);
}

// Create a key and set its value
स्थिर शून्य Set_कुंजी_And_मूल्य(ध्रुव वर्ण *कुंजी, ध्रुव वर्ण *subkey, ध्रुव वर्ण *मूल्य)
{
    HKEY h_कुंजी;
    वर्ण buffer[1024];

    strcpy(buffer, कुंजी);

    // Add subkey name to buffer.
    यदि (subkey)
    {
	strcat(buffer, "\\");
	strcat(buffer, subkey);
    }

    // Create and open key and subkey.
    long result = Reg_Create_कुंजी_Ex(HKEY_CLASSES_मूल,
				 buffer,
				 0, रिक्त, REG_OPTION_NON_VOLATILE,
				 कुंजी_ALL_ACCESS, रिक्त,
				 &h_कुंजी, रिक्त);
    यदि (result != ERROR_SUCCESS)
	प्रत्यागम;

    // Set the value
    यदि (मूल्य)
	Reg_Set_मूल्य_Ex(h_कुंजी, रिक्त, 0, REG_SZ, (BYTE *)मूल्य,
		      (DWORD)STRLEN(मूल्य)+1);

    Reg_Close_कुंजी(h_कुंजी);
}

/*****************************************************************************
 5. OLE Initialisation and shutdown processing
*****************************************************************************/
बाह्य "C" शून्य InitOLE(पूर्णाङ्क *pbDoRestart)
{
    HRESULT hr;

    *pbDoRestart = FALSE;

    // Initialize the OLE libraries
    hr = OleInitialize(रिक्त);
    यदि (FAILED(hr))
    {
	MessageBox(0, "Cannot initialise OLE", "Vim Initialisation", 0);
	गच्छ error0;
    }

    // Create the application object
    app = CVim::Create(pbDoRestart);
    यदि (app == रिक्त)
	गच्छ error1;

    // Create the class factory
    cf = CVimCF::Create();
    यदि (cf == रिक्त)
	गच्छ error1;

    // Register the class factory
    hr = CoRegisterClassObject(
	MYCLSID,
	cf,
	CLSCTX_LOCAL_SERVER,
	REGCLS_MULTIPLEUSE,
	&cf_id);

    यदि (FAILED(hr))
    {
	MessageBox(0, "Cannot register class factory", "Vim Initialisation", 0);
	गच्छ error1;
    }

    // Register the application object as active
    hr = RegisterActiveObject(
	app,
	MYCLSID,
	0,
	&app_id);

    यदि (FAILED(hr))
    {
	MessageBox(0, "Cannot register application object", "Vim Initialisation", 0);
	गच्छ error1;
    }

    प्रत्यागम;

    // Errors: tidy up as much as needed and return
error1:
    UninitOLE();
error0:
    प्रत्यागम;
}

बाह्य "C" शून्य UninitOLE()
{
    // Unregister the application object
    यदि (app_id)
    {
	RevokeActiveObject(app_id, रिक्त);
	app_id = 0;
    }

    // Unregister the class factory
    यदि (cf_id)
    {
	CoRevokeClassObject(cf_id);
	cf_id = 0;
    }

    // Shut down the OLE libraries
    OleUninitialize();

    // Delete the application object
    यदि (app)
    {
	delete app;
	app = रिक्त;
    }

    // Delete the class factory
    यदि (cf)
    {
	delete cf;
	cf = रिक्त;
    }
}
#endif /* FEAT_OLE */
