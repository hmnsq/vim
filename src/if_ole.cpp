/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#எனில் defined(FEAT_OLE) && defined(FEAT_GUI_MSWIN)
/*
 * OLE server implementation.
 *
 * See os_mswin.c for the client side.
 */
வெளி "C" {
# include "vim.h"
}

#அந்தர்பாவ <windows.h>
#அந்தர்பாவ <oleauto.h>

வெளி "C" {
வெளி HWND s_hwnd;
வெளி HWND vim_parent_hwnd;
}

#எனில் (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#இல்லையெனில்
# define FINAL
#endif

#அந்தர்பாவ "if_ole.h"	// Interface definitions
#அந்தர்பாவ "iid_ole.c"	// UUID definitions (compile here)

/* Supply function prototype to work around bug in Mingw oleauto.h header */
#ifdef __MINGW32__
WINOLEAUTAPI UnRegisterTypeLib(REFGUID libID, WORD wVerMajor,
	    WORD wVerMinor, LCID lcid, SYSKIND syskind);
#endif

/*****************************************************************************
 1. Internal definitions for this file
*****************************************************************************/

வகுப்பு CVim;
வகுப்பு CVimCF;

/* Internal data */
// The identifier of the registered class factory
நிலையான unsigned long cf_id = 0;

// The identifier of the running application object
நிலையான unsigned long app_id = 0;

// The single global instance of the class factory
நிலையான CVimCF *cf = 0;

// The single global instance of the application object
நிலையான CVim *app = 0;

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

வகுப்பு CVim FINAL : பொது IVim
{
பொது:
    மெய்நிகர் ~CVim();
    நிலையான CVim *Create(முழு *pbDoRestart);

    // IUnknown members
    STDMETHOD(QueryInterface)(REFIID riid, வெறுமை ** ppv);
    STDMETHOD_(unsigned long, AddRef)(வெறுமை);
    STDMETHOD_(unsigned long, Release)(வெறுமை);

    // IDispatch members
    STDMETHOD(Get_Type_Info_மொத்தம்)(UINT *p_மொத்தம்);
    STDMETHOD(GetTypeInfo)(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo);
    STDMETHOD(GetIDsOfNames)(மாறிலி IID &iid, OLECHAR **names, UINT n, LCID, DISPID *dispids);
    STDMETHOD(Invoke)(DISPID member, மாறிலி IID &iid, LCID, WORD flags, DISPPARAMS *dispparams, VARIANT *result, EXCEPINFO *excepinfo, UINT *argerr);

    // IVim members
    STDMETHOD(Send_சாவிகள்)(BSTR சாவிகள்);
    STDMETHOD(Eval)(BSTR கோவை, BSTR *result);
    STDMETHOD(SetForeground)(வெறுமை);
    STDMETHOD(GetHwnd)(UINT_PTR *result);

தனிப்பட்ட:
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

CVim *CVim::Create(முழு *pbDoRestart)
{
    HRESULT hr;
    CVim *me = 0;
    ITypeLib *typelib = 0;
    ITypeInfo *typeinfo = 0;

    *pbDoRestart = FALSE;

    // Create the object
    me = புதிய CVim();
    எனில் (me == வெற்று)
    {
	MessageBox(0, "Cannot create application object", "Vim Initialisation", 0);
	திரும்பு வெற்று;
    }

    // Load the type library from the registry
    hr = LoadRegTypeLib(MYLIBID, 1, 0, 0x00, &typelib);
    எனில் (FAILED(hr))
    {
	HKEY h_சாவி;

	// Check we can write to the registry.
	// RegCreateKeyEx succeeds even if key exists. W.Briscoe W2K 20021011
	எனில் (Reg_Create_சாவி_Ex(HKEY_CLASSES_வேர், MYVIPROGID, 0, வெற்று,
		  REG_OPTION_NON_VOLATILE,
		  சாவி_ALL_ACCESS, வெற்று, &h_சாவி, வெற்று))
	{
	    delete me;
	    திரும்பு வெற்று; // Unable to write to registry. Quietly fail.
	}
	Reg_Close_சாவி(h_சாவி);

	எனில் (MessageBox(0, "Cannot load registered type library.\nDo you want to register Vim now?",
		    "Vim Initialisation", MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
	    delete me;
	    திரும்பு வெற்று;
	}

	RegisterMe(FALSE);

	// Load the type library from the registry
	hr = LoadRegTypeLib(MYLIBID, 1, 0, 0x00, &typelib);
	எனில் (FAILED(hr))
	{
	    MessageBox(0, "You must restart Vim in order for the registration to take effect.",
						     "Vim Initialisation", 0);
	    *pbDoRestart = TRUE;
	    delete me;
	    திரும்பு வெற்று;
	}
    }

    // Get the type info of the vtable interface
    hr = typelib->GetTypeInfoOfGuid(MYIID, &typeinfo);
    typelib->Release();

    எனில் (FAILED(hr))
    {
	MessageBox(0, "Cannot get interface type information",
						     "Vim Initialisation", 0);
	delete me;
	திரும்பு வெற்று;
    }

    // Save the type information
    me->typeinfo = typeinfo;
    திரும்பு me;
}

CVim::~CVim()
{
    எனில் (typeinfo && vim_parent_hwnd == வெற்று)
	typeinfo->Release();
    typeinfo = 0;
}

STDMETHODIMP
CVim::QueryInterface(REFIID riid, வெறுமை **ppv)
{
    எனில் (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDispatch) || IsEqualIID(riid, MYIID))
    {
	AddRef();
	*ppv = இது;
	திரும்பு S_OK;
    }

    *ppv = 0;
    திரும்பு E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVim::AddRef()
{
    திரும்பு ++ref;
}

STDMETHODIMP_(ULONG)
CVim::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    எனில் (ref > 0)
	--ref;
    திரும்பு ref;
}

STDMETHODIMP
CVim::Get_Type_Info_மொத்தம்(UINT *p_மொத்தம்)
{
    *p_மொத்தம் = 1;
    திரும்பு S_OK;
}

STDMETHODIMP
CVim::GetTypeInfo(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo)
{
    *ppITypeInfo = 0;

    எனில் (iTypeInfo != 0)
	திரும்பு DISP_E_BADINDEX;

    typeinfo->AddRef();
    *ppITypeInfo = typeinfo;
    திரும்பு S_OK;
}

STDMETHODIMP
CVim::GetIDsOfNames(
	மாறிலி IID &iid,
	OLECHAR **names,
	UINT n,
	LCID,
	DISPID *dispids)
{
    எனில் (iid != IID_NULL)
	திரும்பு DISP_E_UNKNOWNINTERFACE;

    திரும்பு typeinfo->GetIDsOfNames(names, n, dispids);
}

STDMETHODIMP
CVim::Invoke(
	DISPID member,
	மாறிலி IID &iid,
	LCID,
	WORD flags,
	DISPPARAMS *dispparams,
	VARIANT *result,
	EXCEPINFO *excepinfo,
	UINT *argerr)
{
    எனில் (iid != IID_NULL)
	திரும்பு DISP_E_UNKNOWNINTERFACE;

    ::SetErrorInfo(0, வெற்று);
    திரும்பு typeinfo->Invoke(static_cast<IDispatch*>(இது),
			    member, flags, dispparams,
			    result, excepinfo, argerr);
}

STDMETHODIMP
CVim::GetHwnd(UINT_PTR *result)
{
    *result = (UINT_PTR)s_hwnd;
    திரும்பு S_OK;
}

STDMETHODIMP
CVim::SetForeground(வெறுமை)
{
    /* Make the Vim window come to the foreground */
    gui_mch_set_foreground();
    திரும்பு S_OK;
}

STDMETHODIMP
CVim::Send_சாவிகள்(BSTR சாவிகள்)
{
    முழு len;
    எழுத்து *buffer;
    char_u *str;
    char_u *ptr;

    /* Get a suitable buffer */
    len = WideCharToMultiByte(CP_ACP, 0, சாவிகள், -1, 0, 0, 0, 0);
    buffer = (எழுத்து *)alloc(len+1);

    எனில் (buffer == வெற்று)
	திரும்பு E_OUTOFMEMORY;

    len = WideCharToMultiByte(CP_ACP, 0, சாவிகள், -1, buffer, len, 0, 0);

    எனில் (len == 0)
    {
	vim_free(buffer);
	திரும்பு E_INVALIDARG;
    }

    /* Translate key codes like <Esc> */
    str = replace_termcodes((char_u *)buffer, &ptr, 0, REPTERM_DO_LT, வெற்று);

    /* If ptr was set, then a new buffer was allocated,
     * so we can free the old one.
     */
    எனில் (ptr)
	vim_free((char_u *)(buffer));

    /* Reject strings too long to fit in the input buffer. Allow 10 bytes
     * space to cover for the (remote) possibility that characters may enter
     * the input buffer between now and when the WM_OLE message is actually
     * processed. If more than 10 characters enter the input buffer in that
     * time, the WM_OLE processing will simply fail to insert the characters.
     */
    எனில் ((முழு)(STRLEN(str)) > (vim_free_in_உள்ளீடு_buf() - 10))
    {
	vim_free(str);
	திரும்பு E_INVALIDARG;
    }

    /* Pass the string to the main input loop. The memory will be freed when
     * the message is processed.  Except for an empty message, we don't need
     * to post it then.
     */
    எனில் (*str == NUL)
	vim_free(str);
    இல்லையெனில்
	PostMessage(வெற்று, WM_OLE, 0, (LPARAM)str);

    திரும்பு S_OK;
}

STDMETHODIMP
CVim::Eval(BSTR கோவை, BSTR *result)
{
#ifdef FEAT_EVAL
    முழு len;
    எழுத்து *buffer;
    எழுத்து *str;
    wchar_t *w_buffer;

    /* Get a suitable buffer */
    len = WideCharToMultiByte(CP_ACP, 0, கோவை, -1, 0, 0, 0, 0);
    எனில் (len == 0)
	திரும்பு E_INVALIDARG;

    buffer = (எழுத்து *)alloc(len);

    எனில் (buffer == வெற்று)
	திரும்பு E_OUTOFMEMORY;

    /* Convert the (wide character) expression to an ASCII string */
    len = WideCharToMultiByte(CP_ACP, 0, கோவை, -1, buffer, len, 0, 0);
    எனில் (len == 0)
	திரும்பு E_INVALIDARG;

    /* Evaluate the expression */
    ++emsg_skip;
    str = (எழுத்து *)eval_to_string((char_u *)buffer, TRUE, FALSE);
    --emsg_skip;
    vim_free(buffer);
    எனில் (str == வெற்று)
	திரும்பு E_FAIL;

    /* Convert the result to wide characters */
    MultiByteToWideChar_alloc(CP_ACP, 0, str, -1, &w_buffer, &len);
    vim_free(str);
    எனில் (w_buffer == வெற்று)
	திரும்பு E_OUTOFMEMORY;

    எனில் (len == 0)
    {
	vim_free(w_buffer);
	திரும்பு E_FAIL;
    }

    /* Store the result */
    *result = SysAllocString(w_buffer);
    vim_free(w_buffer);

    திரும்பு S_OK;
#இல்லையெனில்
    திரும்பு E_NOTIMPL;
#endif
}

/*****************************************************************************
 3. The class factory
*****************************************************************************/

/* Definition
 * ----------
 */

வகுப்பு CVimCF FINAL : பொது IClassFactory
{
பொது:
    நிலையான CVimCF *Create();
    மெய்நிகர் ~CVimCF() {};

    STDMETHOD(QueryInterface)(REFIID riid, வெறுமை ** ppv);
    STDMETHOD_(unsigned long, AddRef)(வெறுமை);
    STDMETHOD_(unsigned long, Release)(வெறுமை);
    STDMETHOD(CreateInstance)(IUnknown *punkOuter, REFIID riid, வெறுமை ** ppv);
    STDMETHOD(LockServer)(BOOL lock);

தனிப்பட்ட:
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
    CVimCF *me = புதிய CVimCF();

    எனில் (me == வெற்று)
	MessageBox(0, "Cannot create class factory", "Vim Initialisation", 0);

    திரும்பு me;
}

STDMETHODIMP
CVimCF::QueryInterface(REFIID riid, வெறுமை **ppv)
{
    எனில் (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	AddRef();
	*ppv = இது;
	திரும்பு S_OK;
    }

    *ppv = 0;
    திரும்பு E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVimCF::AddRef()
{
    திரும்பு ++ref;
}

STDMETHODIMP_(ULONG)
CVimCF::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    எனில் (ref > 0)
	--ref;
    திரும்பு ref;
}

/*ARGSUSED*/
STDMETHODIMP
CVimCF::CreateInstance(IUnknown *punkOuter, REFIID riid, வெறுமை **ppv)
{
    திரும்பு app->QueryInterface(riid, ppv);
}

/*ARGSUSED*/
STDMETHODIMP
CVimCF::LockServer(BOOL lock)
{
    திரும்பு S_OK;
}

/*****************************************************************************
 4. Registry manipulation code
*****************************************************************************/

// Internal use only
நிலையான வெறுமை Set_சாவி_And_மதிப்பு(மாறிலி எழுத்து *path, மாறிலி எழுத்து *subkey, மாறிலி எழுத்து *மதிப்பு);
நிலையான வெறுமை GUIDtochar(மாறிலி GUID &guid, எழுத்து *GUID, முழு length);
நிலையான வெறுமை Recursive_Delete_சாவி(HKEY h_சாவி_Parent, மாறிலி எழுத்து *child);
நிலையான மாறிலி முழு GUID_STRING_SIZE = 39;

// Register the component in the registry
// When "silent" is TRUE don't give any messages.

வெளி "C" வெறுமை RegisterMe(முழு silent)
{
    BOOL ok = TRUE;

    // Get the application startup command
    எழுத்து module[MAX_PATH];

    ::GetModuleFileName(வெற்று, module, MAX_PATH);

    // Unregister first (quietly)
    UnregisterMe(FALSE);

    // Convert the CLSID into a char
    எழுத்து clsid[GUID_STRING_SIZE];
    GUIDtochar(MYCLSID, clsid, அளவு(clsid));

    // Convert the LIBID into a char
    எழுத்து libid[GUID_STRING_SIZE];
    GUIDtochar(MYLIBID, libid, அளவு(libid));

    // Build the key CLSID\\{...}
    எழுத்து சாவி_2[MAX_CLSID_LEN];
    strcpy(சாவி_2, "CLSID\\");
    strcat(சாவி_2, clsid);

    // Add the CLSID to the registry
    Set_சாவி_And_மதிப்பு(சாவி_2, வெற்று, MYNAME);
    Set_சாவி_And_மதிப்பு(சாவி_2, "LocalServer32", module);
    Set_சாவி_And_மதிப்பு(சாவி_2, "ProgID", MYPROGID);
    Set_சாவி_And_மதிப்பு(சாவி_2, "VersionIndependentProgID", MYVIPROGID);
    Set_சாவி_And_மதிப்பு(சாவி_2, "TypeLib", libid);

    // Add the version-independent ProgID subkey under HKEY_CLASSES_ROOT
    Set_சாவி_And_மதிப்பு(MYVIPROGID, வெற்று, MYNAME);
    Set_சாவி_And_மதிப்பு(MYVIPROGID, "CLSID", clsid);
    Set_சாவி_And_மதிப்பு(MYVIPROGID, "CurVer", MYPROGID);

    // Add the versioned ProgID subkey under HKEY_CLASSES_ROOT
    Set_சாவி_And_மதிப்பு(MYPROGID, வெற்று, MYNAME);
    Set_சாவி_And_மதிப்பு(MYPROGID, "CLSID", clsid);

    wchar_t w_module[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, module, -1, w_module, MAX_PATH);

    ITypeLib *typelib = வெற்று;
    எனில் (LoadTypeLib(w_module, &typelib) != S_OK)
    {
	எனில் (!silent)
	    MessageBox(0, "Cannot load type library to register",
						       "Vim Registration", 0);
	ok = FALSE;
    }
    இல்லையெனில்
    {
	எனில் (RegisterTypeLib(typelib, w_module, வெற்று) != S_OK)
	{
	    எனில் (!silent)
		MessageBox(0, "Cannot register type library",
						       "Vim Registration", 0);
	    ok = FALSE;
	}
	typelib->Release();
    }

    எனில் (ok && !silent)
	MessageBox(0, "Registered successfully", "Vim", 0);
}

// Remove the component from the registry
//
// Note: There is little error checking in this code, to allow incomplete
// or failed registrations to be undone.
வெளி "C" வெறுமை UnregisterMe(முழு bNotifyUser)
{
    // Unregister the type library
    ITypeLib *typelib;
    எனில் (SUCCEEDED(LoadRegTypeLib(MYLIBID, MAJORVER, MINORVER, LOCALE, &typelib)))
    {
	TLIBATTR *tla;
	எனில் (SUCCEEDED(typelib->GetLibAttr(&tla)))
	{
	    UnRegisterTypeLib(tla->guid, tla->w_Major_Ver_இலக்கம், tla->w_Minor_Ver_இலக்கம்,
			      tla->lcid, tla->syskind);
	    typelib->ReleaseTLibAttr(tla);
	}
	typelib->Release();
    }

    // Convert the CLSID into a char
    எழுத்து clsid[GUID_STRING_SIZE];
    GUIDtochar(MYCLSID, clsid, அளவு(clsid));

    // Build the key CLSID\\{...}
    எழுத்து சாவி_2[MAX_CLSID_LEN];
    strcpy(சாவி_2, "CLSID\\");
    strcat(சாவி_2, clsid);

    // Delete the CLSID Key - CLSID\{...}
    Recursive_Delete_சாவி(HKEY_CLASSES_வேர், சாவி_2);

    // Delete the version-independent ProgID Key
    Recursive_Delete_சாவி(HKEY_CLASSES_வேர், MYVIPROGID);

    // Delete the ProgID key
    Recursive_Delete_சாவி(HKEY_CLASSES_வேர், MYPROGID);

    எனில் (bNotifyUser)
	MessageBox(0, "Unregistered successfully", "Vim", 0);
}

/****************************************************************************/

// Convert a GUID to a char string
நிலையான வெறுமை GUIDtochar(மாறிலி GUID &guid, எழுத்து *GUID, முழு length)
{
    // Get wide string version
    LPOLESTR wGUID = வெற்று;
    StringFromCLSID(guid, &wGUID);

    // Convert from wide characters to non-wide
    wcstombs(GUID, wGUID, length);

    // Free memory
    CoTaskMemFree(wGUID);
}

// Delete a key and all of its descendants
நிலையான வெறுமை Recursive_Delete_சாவி(HKEY h_சாவி_Parent, மாறிலி எழுத்து *child)
{
    // Open the child
    HKEY h_சாவி_Child;
    LONG result = Reg_Open_சாவி_Ex(h_சாவி_Parent, child, 0,
			       சாவி_ALL_ACCESS, &h_சாவி_Child);
    எனில் (result != ERROR_SUCCESS)
	திரும்பு;

    // Enumerate all of the descendants of this child
    FILETIME time;
    எழுத்து buffer[1024];
    DWORD size = 1024;

    போது (Reg_Enum_சாவி_Ex(h_சாவி_Child, 0, buffer, &size, வெற்று,
			வெற்று, வெற்று, &time) == S_OK)
    {
	// Delete the descendants of this child
	Recursive_Delete_சாவி(h_சாவி_Child, buffer);
	size = 256;
    }

    // Close the child
    Reg_Close_சாவி(h_சாவி_Child);

    // Delete this child
    Reg_Delete_சாவி(h_சாவி_Parent, child);
}

// Create a key and set its value
நிலையான வெறுமை Set_சாவி_And_மதிப்பு(மாறிலி எழுத்து *சாவி, மாறிலி எழுத்து *subkey, மாறிலி எழுத்து *மதிப்பு)
{
    HKEY h_சாவி;
    எழுத்து buffer[1024];

    strcpy(buffer, சாவி);

    // Add subkey name to buffer.
    எனில் (subkey)
    {
	strcat(buffer, "\\");
	strcat(buffer, subkey);
    }

    // Create and open key and subkey.
    long result = Reg_Create_சாவி_Ex(HKEY_CLASSES_வேர்,
				 buffer,
				 0, வெற்று, REG_OPTION_NON_VOLATILE,
				 சாவி_ALL_ACCESS, வெற்று,
				 &h_சாவி, வெற்று);
    எனில் (result != ERROR_SUCCESS)
	திரும்பு;

    // Set the value
    எனில் (மதிப்பு)
	Reg_Set_மதிப்பு_Ex(h_சாவி, வெற்று, 0, REG_SZ, (BYTE *)மதிப்பு,
		      (DWORD)STRLEN(மதிப்பு)+1);

    Reg_Close_சாவி(h_சாவி);
}

/*****************************************************************************
 5. OLE Initialisation and shutdown processing
*****************************************************************************/
வெளி "C" வெறுமை InitOLE(முழு *pbDoRestart)
{
    HRESULT hr;

    *pbDoRestart = FALSE;

    // Initialize the OLE libraries
    hr = OleInitialize(வெற்று);
    எனில் (FAILED(hr))
    {
	MessageBox(0, "Cannot initialise OLE", "Vim Initialisation", 0);
	செல் error0;
    }

    // Create the application object
    app = CVim::Create(pbDoRestart);
    எனில் (app == வெற்று)
	செல் error1;

    // Create the class factory
    cf = CVimCF::Create();
    எனில் (cf == வெற்று)
	செல் error1;

    // Register the class factory
    hr = CoRegisterClassObject(
	MYCLSID,
	cf,
	CLSCTX_LOCAL_SERVER,
	REGCLS_MULTIPLEUSE,
	&cf_id);

    எனில் (FAILED(hr))
    {
	MessageBox(0, "Cannot register class factory", "Vim Initialisation", 0);
	செல் error1;
    }

    // Register the application object as active
    hr = RegisterActiveObject(
	app,
	MYCLSID,
	0,
	&app_id);

    எனில் (FAILED(hr))
    {
	MessageBox(0, "Cannot register application object", "Vim Initialisation", 0);
	செல் error1;
    }

    திரும்பு;

    // Errors: tidy up as much as needed and return
error1:
    UninitOLE();
error0:
    திரும்பு;
}

வெளி "C" வெறுமை UninitOLE()
{
    // Unregister the application object
    எனில் (app_id)
    {
	RevokeActiveObject(app_id, வெற்று);
	app_id = 0;
    }

    // Unregister the class factory
    எனில் (cf_id)
    {
	CoRevokeClassObject(cf_id);
	cf_id = 0;
    }

    // Shut down the OLE libraries
    OleUninitialize();

    // Delete the application object
    எனில் (app)
    {
	delete app;
	app = வெற்று;
    }

    // Delete the class factory
    எனில் (cf)
    {
	delete cf;
	cf = வெற்று;
    }
}
#endif /* FEAT_OLE */
