/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#എങ്കിൽ defined(FEAT_OLE) && defined(FEAT_GUI_MSWIN)
/*
 * OLE server implementation.
 *
 * See os_mswin.c for the client side.
 */
ബാഹ്യ "C" {
# include "vim.h"
}

#അന്തര്ഭാവ <windows.h>
#അന്തര്ഭാവ <oleauto.h>

ബാഹ്യ "C" {
ബാഹ്യ HWND s_hwnd;
ബാഹ്യ HWND vim_parent_hwnd;
}

#എങ്കിൽ (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#അല്ലെങ്കിൽ
# define FINAL
#endif

#അന്തര്ഭാവ "if_ole.h"	// Interface definitions
#അന്തര്ഭാവ "iid_ole.c"	// UUID definitions (compile here)

/* Supply function prototype to work around bug in Mingw oleauto.h header */
#ifdef __MINGW32__
WINOLEAUTAPI UnRegisterTypeLib(REFGUID libID, WORD wVerMajor,
	    WORD wVerMinor, LCID lcid, SYSKIND syskind);
#endif

/*****************************************************************************
 1. Internal definitions for this file
*****************************************************************************/

വർഗ്ഗം CVim;
വർഗ്ഗം CVimCF;

/* Internal data */
// The identifier of the registered class factory
സ്ഥിതം unsigned long cf_id = 0;

// The identifier of the running application object
സ്ഥിതം unsigned long app_id = 0;

// The single global instance of the class factory
സ്ഥിതം CVimCF *cf = 0;

// The single global instance of the application object
സ്ഥിതം CVim *app = 0;

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

വർഗ്ഗം CVim FINAL : പൊതു IVim
{
പൊതു:
    മായാ ~CVim();
    സ്ഥിതം CVim *Create(പൂർണ്ണസംഖ്യ *pbDoRestart);

    // IUnknown members
    STDMETHOD(QueryInterface)(REFIID riid, ശൂന്യം ** ppv);
    STDMETHOD_(unsigned long, AddRef)(ശൂന്യം);
    STDMETHOD_(unsigned long, Release)(ശൂന്യം);

    // IDispatch members
    STDMETHOD(Get_Type_Info_എണ്ണം)(UINT *p_എണ്ണം);
    STDMETHOD(GetTypeInfo)(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo);
    STDMETHOD(GetIDsOfNames)(സ്ഥിരം IID &iid, OLECHAR **names, UINT n, LCID, DISPID *dispids);
    STDMETHOD(Invoke)(DISPID member, സ്ഥിരം IID &iid, LCID, WORD flags, DISPPARAMS *dispparams, VARIANT *result, EXCEPINFO *excepinfo, UINT *argerr);

    // IVim members
    STDMETHOD(Send_താക്കോലുകൾ)(BSTR താക്കോലുകൾ);
    STDMETHOD(Eval)(BSTR വ്യഞ്ജകം, BSTR *result);
    STDMETHOD(SetForeground)(ശൂന്യം);
    STDMETHOD(GetHwnd)(UINT_PTR *result);

സ്വകാര്യ:
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

CVim *CVim::Create(പൂർണ്ണസംഖ്യ *pbDoRestart)
{
    HRESULT hr;
    CVim *me = 0;
    ITypeLib *typelib = 0;
    ITypeInfo *typeinfo = 0;

    *pbDoRestart = FALSE;

    // Create the object
    me = പുതിയ CVim();
    എങ്കിൽ (me == ശൂന്യ)
    {
	MessageBox(0, "Cannot create application object", "Vim Initialisation", 0);
	തിരികെ ശൂന്യ;
    }

    // Load the type library from the registry
    hr = LoadRegTypeLib(MYLIBID, 1, 0, 0x00, &typelib);
    എങ്കിൽ (FAILED(hr))
    {
	HKEY h_താക്കോൽ;

	// Check we can write to the registry.
	// RegCreateKeyEx succeeds even if key exists. W.Briscoe W2K 20021011
	എങ്കിൽ (Reg_Create_താക്കോൽ_Ex(HKEY_CLASSES_വേര്, MYVIPROGID, 0, ശൂന്യ,
		  REG_OPTION_NON_VOLATILE,
		  താക്കോൽ_ALL_ACCESS, ശൂന്യ, &h_താക്കോൽ, ശൂന്യ))
	{
	    delete me;
	    തിരികെ ശൂന്യ; // Unable to write to registry. Quietly fail.
	}
	Reg_Close_താക്കോൽ(h_താക്കോൽ);

	എങ്കിൽ (MessageBox(0, "Cannot load registered type library.\nDo you want to register Vim now?",
		    "Vim Initialisation", MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
	    delete me;
	    തിരികെ ശൂന്യ;
	}

	RegisterMe(FALSE);

	// Load the type library from the registry
	hr = LoadRegTypeLib(MYLIBID, 1, 0, 0x00, &typelib);
	എങ്കിൽ (FAILED(hr))
	{
	    MessageBox(0, "You must restart Vim in order for the registration to take effect.",
						     "Vim Initialisation", 0);
	    *pbDoRestart = TRUE;
	    delete me;
	    തിരികെ ശൂന്യ;
	}
    }

    // Get the type info of the vtable interface
    hr = typelib->GetTypeInfoOfGuid(MYIID, &typeinfo);
    typelib->Release();

    എങ്കിൽ (FAILED(hr))
    {
	MessageBox(0, "Cannot get interface type information",
						     "Vim Initialisation", 0);
	delete me;
	തിരികെ ശൂന്യ;
    }

    // Save the type information
    me->typeinfo = typeinfo;
    തിരികെ me;
}

CVim::~CVim()
{
    എങ്കിൽ (typeinfo && vim_parent_hwnd == ശൂന്യ)
	typeinfo->Release();
    typeinfo = 0;
}

STDMETHODIMP
CVim::QueryInterface(REFIID riid, ശൂന്യം **ppv)
{
    എങ്കിൽ (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDispatch) || IsEqualIID(riid, MYIID))
    {
	AddRef();
	*ppv = ഇത്;
	തിരികെ S_OK;
    }

    *ppv = 0;
    തിരികെ E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVim::AddRef()
{
    തിരികെ ++ref;
}

STDMETHODIMP_(ULONG)
CVim::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    എങ്കിൽ (ref > 0)
	--ref;
    തിരികെ ref;
}

STDMETHODIMP
CVim::Get_Type_Info_എണ്ണം(UINT *p_എണ്ണം)
{
    *p_എണ്ണം = 1;
    തിരികെ S_OK;
}

STDMETHODIMP
CVim::GetTypeInfo(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo)
{
    *ppITypeInfo = 0;

    എങ്കിൽ (iTypeInfo != 0)
	തിരികെ DISP_E_BADINDEX;

    typeinfo->AddRef();
    *ppITypeInfo = typeinfo;
    തിരികെ S_OK;
}

STDMETHODIMP
CVim::GetIDsOfNames(
	സ്ഥിരം IID &iid,
	OLECHAR **names,
	UINT n,
	LCID,
	DISPID *dispids)
{
    എങ്കിൽ (iid != IID_NULL)
	തിരികെ DISP_E_UNKNOWNINTERFACE;

    തിരികെ typeinfo->GetIDsOfNames(names, n, dispids);
}

STDMETHODIMP
CVim::Invoke(
	DISPID member,
	സ്ഥിരം IID &iid,
	LCID,
	WORD flags,
	DISPPARAMS *dispparams,
	VARIANT *result,
	EXCEPINFO *excepinfo,
	UINT *argerr)
{
    എങ്കിൽ (iid != IID_NULL)
	തിരികെ DISP_E_UNKNOWNINTERFACE;

    ::SetErrorInfo(0, ശൂന്യ);
    തിരികെ typeinfo->Invoke(static_cast<IDispatch*>(ഇത്),
			    member, flags, dispparams,
			    result, excepinfo, argerr);
}

STDMETHODIMP
CVim::GetHwnd(UINT_PTR *result)
{
    *result = (UINT_PTR)s_hwnd;
    തിരികെ S_OK;
}

STDMETHODIMP
CVim::SetForeground(ശൂന്യം)
{
    /* Make the Vim window come to the foreground */
    gui_mch_set_foreground();
    തിരികെ S_OK;
}

STDMETHODIMP
CVim::Send_താക്കോലുകൾ(BSTR താക്കോലുകൾ)
{
    പൂർണ്ണസംഖ്യ len;
    അക്ഷരം *buffer;
    char_u *str;
    char_u *ptr;

    /* Get a suitable buffer */
    len = WideCharToMultiByte(CP_ACP, 0, താക്കോലുകൾ, -1, 0, 0, 0, 0);
    buffer = (അക്ഷരം *)alloc(len+1);

    എങ്കിൽ (buffer == ശൂന്യ)
	തിരികെ E_OUTOFMEMORY;

    len = WideCharToMultiByte(CP_ACP, 0, താക്കോലുകൾ, -1, buffer, len, 0, 0);

    എങ്കിൽ (len == 0)
    {
	vim_free(buffer);
	തിരികെ E_INVALIDARG;
    }

    /* Translate key codes like <Esc> */
    str = replace_termcodes((char_u *)buffer, &ptr, 0, REPTERM_DO_LT, ശൂന്യ);

    /* If ptr was set, then a new buffer was allocated,
     * so we can free the old one.
     */
    എങ്കിൽ (ptr)
	vim_free((char_u *)(buffer));

    /* Reject strings too long to fit in the input buffer. Allow 10 bytes
     * space to cover for the (remote) possibility that characters may enter
     * the input buffer between now and when the WM_OLE message is actually
     * processed. If more than 10 characters enter the input buffer in that
     * time, the WM_OLE processing will simply fail to insert the characters.
     */
    എങ്കിൽ ((പൂർണ്ണസംഖ്യ)(STRLEN(str)) > (vim_free_in_നിവേശം_buf() - 10))
    {
	vim_free(str);
	തിരികെ E_INVALIDARG;
    }

    /* Pass the string to the main input loop. The memory will be freed when
     * the message is processed.  Except for an empty message, we don't need
     * to post it then.
     */
    എങ്കിൽ (*str == NUL)
	vim_free(str);
    അല്ലെങ്കിൽ
	PostMessage(ശൂന്യ, WM_OLE, 0, (LPARAM)str);

    തിരികെ S_OK;
}

STDMETHODIMP
CVim::Eval(BSTR വ്യഞ്ജകം, BSTR *result)
{
#ifdef FEAT_EVAL
    പൂർണ്ണസംഖ്യ len;
    അക്ഷരം *buffer;
    അക്ഷരം *str;
    wchar_t *w_buffer;

    /* Get a suitable buffer */
    len = WideCharToMultiByte(CP_ACP, 0, വ്യഞ്ജകം, -1, 0, 0, 0, 0);
    എങ്കിൽ (len == 0)
	തിരികെ E_INVALIDARG;

    buffer = (അക്ഷരം *)alloc(len);

    എങ്കിൽ (buffer == ശൂന്യ)
	തിരികെ E_OUTOFMEMORY;

    /* Convert the (wide character) expression to an ASCII string */
    len = WideCharToMultiByte(CP_ACP, 0, വ്യഞ്ജകം, -1, buffer, len, 0, 0);
    എങ്കിൽ (len == 0)
	തിരികെ E_INVALIDARG;

    /* Evaluate the expression */
    ++emsg_skip;
    str = (അക്ഷരം *)eval_to_string((char_u *)buffer, TRUE, FALSE);
    --emsg_skip;
    vim_free(buffer);
    എങ്കിൽ (str == ശൂന്യ)
	തിരികെ E_FAIL;

    /* Convert the result to wide characters */
    MultiByteToWideChar_alloc(CP_ACP, 0, str, -1, &w_buffer, &len);
    vim_free(str);
    എങ്കിൽ (w_buffer == ശൂന്യ)
	തിരികെ E_OUTOFMEMORY;

    എങ്കിൽ (len == 0)
    {
	vim_free(w_buffer);
	തിരികെ E_FAIL;
    }

    /* Store the result */
    *result = SysAllocString(w_buffer);
    vim_free(w_buffer);

    തിരികെ S_OK;
#അല്ലെങ്കിൽ
    തിരികെ E_NOTIMPL;
#endif
}

/*****************************************************************************
 3. The class factory
*****************************************************************************/

/* Definition
 * ----------
 */

വർഗ്ഗം CVimCF FINAL : പൊതു IClassFactory
{
പൊതു:
    സ്ഥിതം CVimCF *Create();
    മായാ ~CVimCF() {};

    STDMETHOD(QueryInterface)(REFIID riid, ശൂന്യം ** ppv);
    STDMETHOD_(unsigned long, AddRef)(ശൂന്യം);
    STDMETHOD_(unsigned long, Release)(ശൂന്യം);
    STDMETHOD(CreateInstance)(IUnknown *punkOuter, REFIID riid, ശൂന്യം ** ppv);
    STDMETHOD(LockServer)(BOOL lock);

സ്വകാര്യ:
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
    CVimCF *me = പുതിയ CVimCF();

    എങ്കിൽ (me == ശൂന്യ)
	MessageBox(0, "Cannot create class factory", "Vim Initialisation", 0);

    തിരികെ me;
}

STDMETHODIMP
CVimCF::QueryInterface(REFIID riid, ശൂന്യം **ppv)
{
    എങ്കിൽ (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	AddRef();
	*ppv = ഇത്;
	തിരികെ S_OK;
    }

    *ppv = 0;
    തിരികെ E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVimCF::AddRef()
{
    തിരികെ ++ref;
}

STDMETHODIMP_(ULONG)
CVimCF::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    എങ്കിൽ (ref > 0)
	--ref;
    തിരികെ ref;
}

/*ARGSUSED*/
STDMETHODIMP
CVimCF::CreateInstance(IUnknown *punkOuter, REFIID riid, ശൂന്യം **ppv)
{
    തിരികെ app->QueryInterface(riid, ppv);
}

/*ARGSUSED*/
STDMETHODIMP
CVimCF::LockServer(BOOL lock)
{
    തിരികെ S_OK;
}

/*****************************************************************************
 4. Registry manipulation code
*****************************************************************************/

// Internal use only
സ്ഥിതം ശൂന്യം Set_താക്കോൽ_And_മൂല്യം(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *subkey, സ്ഥിരം അക്ഷരം *മൂല്യം);
സ്ഥിതം ശൂന്യം GUIDtochar(സ്ഥിരം GUID &guid, അക്ഷരം *GUID, പൂർണ്ണസംഖ്യ length);
സ്ഥിതം ശൂന്യം Recursive_Delete_താക്കോൽ(HKEY h_താക്കോൽ_Parent, സ്ഥിരം അക്ഷരം *child);
സ്ഥിതം സ്ഥിരം പൂർണ്ണസംഖ്യ GUID_STRING_SIZE = 39;

// Register the component in the registry
// When "silent" is TRUE don't give any messages.

ബാഹ്യ "C" ശൂന്യം RegisterMe(പൂർണ്ണസംഖ്യ silent)
{
    BOOL ok = TRUE;

    // Get the application startup command
    അക്ഷരം module[MAX_PATH];

    ::GetModuleFileName(ശൂന്യ, module, MAX_PATH);

    // Unregister first (quietly)
    UnregisterMe(FALSE);

    // Convert the CLSID into a char
    അക്ഷരം clsid[GUID_STRING_SIZE];
    GUIDtochar(MYCLSID, clsid, വലുപ്പം(clsid));

    // Convert the LIBID into a char
    അക്ഷരം libid[GUID_STRING_SIZE];
    GUIDtochar(MYLIBID, libid, വലുപ്പം(libid));

    // Build the key CLSID\\{...}
    അക്ഷരം താക്കോൽ_2[MAX_CLSID_LEN];
    strcpy(താക്കോൽ_2, "CLSID\\");
    strcat(താക്കോൽ_2, clsid);

    // Add the CLSID to the registry
    Set_താക്കോൽ_And_മൂല്യം(താക്കോൽ_2, ശൂന്യ, MYNAME);
    Set_താക്കോൽ_And_മൂല്യം(താക്കോൽ_2, "LocalServer32", module);
    Set_താക്കോൽ_And_മൂല്യം(താക്കോൽ_2, "ProgID", MYPROGID);
    Set_താക്കോൽ_And_മൂല്യം(താക്കോൽ_2, "VersionIndependentProgID", MYVIPROGID);
    Set_താക്കോൽ_And_മൂല്യം(താക്കോൽ_2, "TypeLib", libid);

    // Add the version-independent ProgID subkey under HKEY_CLASSES_ROOT
    Set_താക്കോൽ_And_മൂല്യം(MYVIPROGID, ശൂന്യ, MYNAME);
    Set_താക്കോൽ_And_മൂല്യം(MYVIPROGID, "CLSID", clsid);
    Set_താക്കോൽ_And_മൂല്യം(MYVIPROGID, "CurVer", MYPROGID);

    // Add the versioned ProgID subkey under HKEY_CLASSES_ROOT
    Set_താക്കോൽ_And_മൂല്യം(MYPROGID, ശൂന്യ, MYNAME);
    Set_താക്കോൽ_And_മൂല്യം(MYPROGID, "CLSID", clsid);

    wchar_t w_module[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, module, -1, w_module, MAX_PATH);

    ITypeLib *typelib = ശൂന്യ;
    എങ്കിൽ (LoadTypeLib(w_module, &typelib) != S_OK)
    {
	എങ്കിൽ (!silent)
	    MessageBox(0, "Cannot load type library to register",
						       "Vim Registration", 0);
	ok = FALSE;
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (RegisterTypeLib(typelib, w_module, ശൂന്യ) != S_OK)
	{
	    എങ്കിൽ (!silent)
		MessageBox(0, "Cannot register type library",
						       "Vim Registration", 0);
	    ok = FALSE;
	}
	typelib->Release();
    }

    എങ്കിൽ (ok && !silent)
	MessageBox(0, "Registered successfully", "Vim", 0);
}

// Remove the component from the registry
//
// Note: There is little error checking in this code, to allow incomplete
// or failed registrations to be undone.
ബാഹ്യ "C" ശൂന്യം UnregisterMe(പൂർണ്ണസംഖ്യ bNotifyUser)
{
    // Unregister the type library
    ITypeLib *typelib;
    എങ്കിൽ (SUCCEEDED(LoadRegTypeLib(MYLIBID, MAJORVER, MINORVER, LOCALE, &typelib)))
    {
	TLIBATTR *tla;
	എങ്കിൽ (SUCCEEDED(typelib->GetLibAttr(&tla)))
	{
	    UnRegisterTypeLib(tla->guid, tla->w_Major_Ver_അക്കം, tla->w_Minor_Ver_അക്കം,
			      tla->lcid, tla->syskind);
	    typelib->ReleaseTLibAttr(tla);
	}
	typelib->Release();
    }

    // Convert the CLSID into a char
    അക്ഷരം clsid[GUID_STRING_SIZE];
    GUIDtochar(MYCLSID, clsid, വലുപ്പം(clsid));

    // Build the key CLSID\\{...}
    അക്ഷരം താക്കോൽ_2[MAX_CLSID_LEN];
    strcpy(താക്കോൽ_2, "CLSID\\");
    strcat(താക്കോൽ_2, clsid);

    // Delete the CLSID Key - CLSID\{...}
    Recursive_Delete_താക്കോൽ(HKEY_CLASSES_വേര്, താക്കോൽ_2);

    // Delete the version-independent ProgID Key
    Recursive_Delete_താക്കോൽ(HKEY_CLASSES_വേര്, MYVIPROGID);

    // Delete the ProgID key
    Recursive_Delete_താക്കോൽ(HKEY_CLASSES_വേര്, MYPROGID);

    എങ്കിൽ (bNotifyUser)
	MessageBox(0, "Unregistered successfully", "Vim", 0);
}

/****************************************************************************/

// Convert a GUID to a char string
സ്ഥിതം ശൂന്യം GUIDtochar(സ്ഥിരം GUID &guid, അക്ഷരം *GUID, പൂർണ്ണസംഖ്യ length)
{
    // Get wide string version
    LPOLESTR wGUID = ശൂന്യ;
    StringFromCLSID(guid, &wGUID);

    // Convert from wide characters to non-wide
    wcstombs(GUID, wGUID, length);

    // Free memory
    CoTaskMemFree(wGUID);
}

// Delete a key and all of its descendants
സ്ഥിതം ശൂന്യം Recursive_Delete_താക്കോൽ(HKEY h_താക്കോൽ_Parent, സ്ഥിരം അക്ഷരം *child)
{
    // Open the child
    HKEY h_താക്കോൽ_Child;
    LONG result = Reg_Open_താക്കോൽ_Ex(h_താക്കോൽ_Parent, child, 0,
			       താക്കോൽ_ALL_ACCESS, &h_താക്കോൽ_Child);
    എങ്കിൽ (result != ERROR_SUCCESS)
	തിരികെ;

    // Enumerate all of the descendants of this child
    FILETIME time;
    അക്ഷരം buffer[1024];
    DWORD size = 1024;

    ആകുമ്പോൾ (Reg_Enum_താക്കോൽ_Ex(h_താക്കോൽ_Child, 0, buffer, &size, ശൂന്യ,
			ശൂന്യ, ശൂന്യ, &time) == S_OK)
    {
	// Delete the descendants of this child
	Recursive_Delete_താക്കോൽ(h_താക്കോൽ_Child, buffer);
	size = 256;
    }

    // Close the child
    Reg_Close_താക്കോൽ(h_താക്കോൽ_Child);

    // Delete this child
    Reg_Delete_താക്കോൽ(h_താക്കോൽ_Parent, child);
}

// Create a key and set its value
സ്ഥിതം ശൂന്യം Set_താക്കോൽ_And_മൂല്യം(സ്ഥിരം അക്ഷരം *താക്കോൽ, സ്ഥിരം അക്ഷരം *subkey, സ്ഥിരം അക്ഷരം *മൂല്യം)
{
    HKEY h_താക്കോൽ;
    അക്ഷരം buffer[1024];

    strcpy(buffer, താക്കോൽ);

    // Add subkey name to buffer.
    എങ്കിൽ (subkey)
    {
	strcat(buffer, "\\");
	strcat(buffer, subkey);
    }

    // Create and open key and subkey.
    long result = Reg_Create_താക്കോൽ_Ex(HKEY_CLASSES_വേര്,
				 buffer,
				 0, ശൂന്യ, REG_OPTION_NON_VOLATILE,
				 താക്കോൽ_ALL_ACCESS, ശൂന്യ,
				 &h_താക്കോൽ, ശൂന്യ);
    എങ്കിൽ (result != ERROR_SUCCESS)
	തിരികെ;

    // Set the value
    എങ്കിൽ (മൂല്യം)
	Reg_Set_മൂല്യം_Ex(h_താക്കോൽ, ശൂന്യ, 0, REG_SZ, (BYTE *)മൂല്യം,
		      (DWORD)STRLEN(മൂല്യം)+1);

    Reg_Close_താക്കോൽ(h_താക്കോൽ);
}

/*****************************************************************************
 5. OLE Initialisation and shutdown processing
*****************************************************************************/
ബാഹ്യ "C" ശൂന്യം InitOLE(പൂർണ്ണസംഖ്യ *pbDoRestart)
{
    HRESULT hr;

    *pbDoRestart = FALSE;

    // Initialize the OLE libraries
    hr = OleInitialize(ശൂന്യ);
    എങ്കിൽ (FAILED(hr))
    {
	MessageBox(0, "Cannot initialise OLE", "Vim Initialisation", 0);
	പോകുക error0;
    }

    // Create the application object
    app = CVim::Create(pbDoRestart);
    എങ്കിൽ (app == ശൂന്യ)
	പോകുക error1;

    // Create the class factory
    cf = CVimCF::Create();
    എങ്കിൽ (cf == ശൂന്യ)
	പോകുക error1;

    // Register the class factory
    hr = CoRegisterClassObject(
	MYCLSID,
	cf,
	CLSCTX_LOCAL_SERVER,
	REGCLS_MULTIPLEUSE,
	&cf_id);

    എങ്കിൽ (FAILED(hr))
    {
	MessageBox(0, "Cannot register class factory", "Vim Initialisation", 0);
	പോകുക error1;
    }

    // Register the application object as active
    hr = RegisterActiveObject(
	app,
	MYCLSID,
	0,
	&app_id);

    എങ്കിൽ (FAILED(hr))
    {
	MessageBox(0, "Cannot register application object", "Vim Initialisation", 0);
	പോകുക error1;
    }

    തിരികെ;

    // Errors: tidy up as much as needed and return
error1:
    UninitOLE();
error0:
    തിരികെ;
}

ബാഹ്യ "C" ശൂന്യം UninitOLE()
{
    // Unregister the application object
    എങ്കിൽ (app_id)
    {
	RevokeActiveObject(app_id, ശൂന്യ);
	app_id = 0;
    }

    // Unregister the class factory
    എങ്കിൽ (cf_id)
    {
	CoRevokeClassObject(cf_id);
	cf_id = 0;
    }

    // Shut down the OLE libraries
    OleUninitialize();

    // Delete the application object
    എങ്കിൽ (app)
    {
	delete app;
	app = ശൂന്യ;
    }

    // Delete the class factory
    എങ്കിൽ (cf)
    {
	delete cf;
	cf = ശൂന്യ;
    }
}
#endif /* FEAT_OLE */
