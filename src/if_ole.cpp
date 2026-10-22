/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#ଯଦି defined(FEAT_OLE) && defined(FEAT_GUI_MSWIN)
/*
 * OLE server implementation.
 *
 * See os_mswin.c for the client side.
 */
ବାହ୍ୟ "C" {
# include "vim.h"
}

#ଅନ୍ତର୍ଭାଵ <windows.h>
#ଅନ୍ତର୍ଭାଵ <oleauto.h>

ବାହ୍ୟ "C" {
ବାହ୍ୟ HWND s_hwnd;
ବାହ୍ୟ HWND vim_parent_hwnd;
}

#ଯଦି (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#ଅନ୍ୟଥା
# define FINAL
#endif

#ଅନ୍ତର୍ଭାଵ "if_ole.h"	// Interface definitions
#ଅନ୍ତର୍ଭାଵ "iid_ole.c"	// UUID definitions (compile here)

/* Supply function prototype to work around bug in Mingw oleauto.h header */
#ifdef __MINGW32__
WINOLEAUTAPI UnRegisterTypeLib(REFGUID libID, WORD wVerMajor,
	    WORD wVerMinor, LCID lcid, SYSKIND syskind);
#endif

/*****************************************************************************
 1. Internal definitions for this file
*****************************************************************************/

ଶ୍ରେଣୀ CVim;
ଶ୍ରେଣୀ CVimCF;

/* Internal data */
// The identifier of the registered class factory
ସ୍ଥିର unsigned long cf_id = 0;

// The identifier of the running application object
ସ୍ଥିର unsigned long app_id = 0;

// The single global instance of the class factory
ସ୍ଥିର CVimCF *cf = 0;

// The single global instance of the application object
ସ୍ଥିର CVim *app = 0;

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

ଶ୍ରେଣୀ CVim FINAL : ସର୍ବସାଧାରଣ IVim
{
ସର୍ବସାଧାରଣ:
    ଆଭାସୀ ~CVim();
    ସ୍ଥିର CVim *Create(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *pbDoRestart);

    // IUnknown members
    STDMETHOD(QueryInterface)(REFIID riid, ଶୂନ୍ୟ ** ppv);
    STDMETHOD_(unsigned long, AddRef)(ଶୂନ୍ୟ);
    STDMETHOD_(unsigned long, Release)(ଶୂନ୍ୟ);

    // IDispatch members
    STDMETHOD(Get_Type_Info_ଗଣନା)(UINT *p_ଗଣନା);
    STDMETHOD(GetTypeInfo)(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo);
    STDMETHOD(GetIDsOfNames)(ଅଚଳ IID &iid, OLECHAR **names, UINT n, LCID, DISPID *dispids);
    STDMETHOD(Invoke)(DISPID member, ଅଚଳ IID &iid, LCID, WORD flags, DISPPARAMS *dispparams, VARIANT *result, EXCEPINFO *excepinfo, UINT *argerr);

    // IVim members
    STDMETHOD(Send_ଚାବିଗୁଡ଼ିକ)(BSTR ଚାବିଗୁଡ଼ିକ);
    STDMETHOD(Eval)(BSTR ପଦସମଷ୍ଟି, BSTR *result);
    STDMETHOD(SetForeground)(ଶୂନ୍ୟ);
    STDMETHOD(GetHwnd)(UINT_PTR *result);

ବ୍ୟକ୍ତିଗତ:
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

CVim *CVim::Create(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *pbDoRestart)
{
    HRESULT hr;
    CVim *me = 0;
    ITypeLib *typelib = 0;
    ITypeInfo *typeinfo = 0;

    *pbDoRestart = FALSE;

    // Create the object
    me = ନୂତନ CVim();
    ଯଦି (me == ଖାଲି)
    {
	MessageBox(0, "Cannot create application object", "Vim Initialisation", 0);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    // Load the type library from the registry
    hr = LoadRegTypeLib(MYLIBID, 1, 0, 0x00, &typelib);
    ଯଦି (FAILED(hr))
    {
	HKEY h_ଚାବି;

	// Check we can write to the registry.
	// RegCreateKeyEx succeeds even if key exists. W.Briscoe W2K 20021011
	ଯଦି (Reg_Create_ଚାବି_Ex(HKEY_CLASSES_ମୂଳ, MYVIPROGID, 0, ଖାଲି,
		  REG_OPTION_NON_VOLATILE,
		  ଚାବି_ALL_ACCESS, ଖାଲି, &h_ଚାବି, ଖାଲି))
	{
	    delete me;
	    ଫେରନ୍ତୁ ଖାଲି; // Unable to write to registry. Quietly fail.
	}
	Reg_Close_ଚାବି(h_ଚାବି);

	ଯଦି (MessageBox(0, "Cannot load registered type library.\nDo you want to register Vim now?",
		    "Vim Initialisation", MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
	    delete me;
	    ଫେରନ୍ତୁ ଖାଲି;
	}

	RegisterMe(FALSE);

	// Load the type library from the registry
	hr = LoadRegTypeLib(MYLIBID, 1, 0, 0x00, &typelib);
	ଯଦି (FAILED(hr))
	{
	    MessageBox(0, "You must restart Vim in order for the registration to take effect.",
						     "Vim Initialisation", 0);
	    *pbDoRestart = TRUE;
	    delete me;
	    ଫେରନ୍ତୁ ଖାଲି;
	}
    }

    // Get the type info of the vtable interface
    hr = typelib->GetTypeInfoOfGuid(MYIID, &typeinfo);
    typelib->Release();

    ଯଦି (FAILED(hr))
    {
	MessageBox(0, "Cannot get interface type information",
						     "Vim Initialisation", 0);
	delete me;
	ଫେରନ୍ତୁ ଖାଲି;
    }

    // Save the type information
    me->typeinfo = typeinfo;
    ଫେରନ୍ତୁ me;
}

CVim::~CVim()
{
    ଯଦି (typeinfo && vim_parent_hwnd == ଖାଲି)
	typeinfo->Release();
    typeinfo = 0;
}

STDMETHODIMP
CVim::QueryInterface(REFIID riid, ଶୂନ୍ୟ **ppv)
{
    ଯଦି (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDispatch) || IsEqualIID(riid, MYIID))
    {
	AddRef();
	*ppv = ଏହି;
	ଫେରନ୍ତୁ S_OK;
    }

    *ppv = 0;
    ଫେରନ୍ତୁ E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVim::AddRef()
{
    ଫେରନ୍ତୁ ++ref;
}

STDMETHODIMP_(ULONG)
CVim::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    ଯଦି (ref > 0)
	--ref;
    ଫେରନ୍ତୁ ref;
}

STDMETHODIMP
CVim::Get_Type_Info_ଗଣନା(UINT *p_ଗଣନା)
{
    *p_ଗଣନା = 1;
    ଫେରନ୍ତୁ S_OK;
}

STDMETHODIMP
CVim::GetTypeInfo(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo)
{
    *ppITypeInfo = 0;

    ଯଦି (iTypeInfo != 0)
	ଫେରନ୍ତୁ DISP_E_BADINDEX;

    typeinfo->AddRef();
    *ppITypeInfo = typeinfo;
    ଫେରନ୍ତୁ S_OK;
}

STDMETHODIMP
CVim::GetIDsOfNames(
	ଅଚଳ IID &iid,
	OLECHAR **names,
	UINT n,
	LCID,
	DISPID *dispids)
{
    ଯଦି (iid != IID_NULL)
	ଫେରନ୍ତୁ DISP_E_UNKNOWNINTERFACE;

    ଫେରନ୍ତୁ typeinfo->GetIDsOfNames(names, n, dispids);
}

STDMETHODIMP
CVim::Invoke(
	DISPID member,
	ଅଚଳ IID &iid,
	LCID,
	WORD flags,
	DISPPARAMS *dispparams,
	VARIANT *result,
	EXCEPINFO *excepinfo,
	UINT *argerr)
{
    ଯଦି (iid != IID_NULL)
	ଫେରନ୍ତୁ DISP_E_UNKNOWNINTERFACE;

    ::SetErrorInfo(0, ଖାଲି);
    ଫେରନ୍ତୁ typeinfo->Invoke(static_cast<IDispatch*>(ଏହି),
			    member, flags, dispparams,
			    result, excepinfo, argerr);
}

STDMETHODIMP
CVim::GetHwnd(UINT_PTR *result)
{
    *result = (UINT_PTR)s_hwnd;
    ଫେରନ୍ତୁ S_OK;
}

STDMETHODIMP
CVim::SetForeground(ଶୂନ୍ୟ)
{
    /* Make the Vim window come to the foreground */
    gui_mch_set_foreground();
    ଫେରନ୍ତୁ S_OK;
}

STDMETHODIMP
CVim::Send_ଚାବିଗୁଡ଼ିକ(BSTR ଚାବିଗୁଡ଼ିକ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;
    ଅକ୍ଷର *buffer;
    char_u *str;
    char_u *ptr;

    /* Get a suitable buffer */
    len = WideCharToMultiByte(CP_ACP, 0, ଚାବିଗୁଡ଼ିକ, -1, 0, 0, 0, 0);
    buffer = (ଅକ୍ଷର *)alloc(len+1);

    ଯଦି (buffer == ଖାଲି)
	ଫେରନ୍ତୁ E_OUTOFMEMORY;

    len = WideCharToMultiByte(CP_ACP, 0, ଚାବିଗୁଡ଼ିକ, -1, buffer, len, 0, 0);

    ଯଦି (len == 0)
    {
	vim_free(buffer);
	ଫେରନ୍ତୁ E_INVALIDARG;
    }

    /* Translate key codes like <Esc> */
    str = replace_termcodes((char_u *)buffer, &ptr, 0, REPTERM_DO_LT, ଖାଲି);

    /* If ptr was set, then a new buffer was allocated,
     * so we can free the old one.
     */
    ଯଦି (ptr)
	vim_free((char_u *)(buffer));

    /* Reject strings too long to fit in the input buffer. Allow 10 bytes
     * space to cover for the (remote) possibility that characters may enter
     * the input buffer between now and when the WM_OLE message is actually
     * processed. If more than 10 characters enter the input buffer in that
     * time, the WM_OLE processing will simply fail to insert the characters.
     */
    ଯଦି ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(STRLEN(str)) > (vim_free_in_ନିବେଶ_buf() - 10))
    {
	vim_free(str);
	ଫେରନ୍ତୁ E_INVALIDARG;
    }

    /* Pass the string to the main input loop. The memory will be freed when
     * the message is processed.  Except for an empty message, we don't need
     * to post it then.
     */
    ଯଦି (*str == NUL)
	vim_free(str);
    ଅନ୍ୟଥା
	PostMessage(ଖାଲି, WM_OLE, 0, (LPARAM)str);

    ଫେରନ୍ତୁ S_OK;
}

STDMETHODIMP
CVim::Eval(BSTR ପଦସମଷ୍ଟି, BSTR *result)
{
#ifdef FEAT_EVAL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;
    ଅକ୍ଷର *buffer;
    ଅକ୍ଷର *str;
    wchar_t *w_buffer;

    /* Get a suitable buffer */
    len = WideCharToMultiByte(CP_ACP, 0, ପଦସମଷ୍ଟି, -1, 0, 0, 0, 0);
    ଯଦି (len == 0)
	ଫେରନ୍ତୁ E_INVALIDARG;

    buffer = (ଅକ୍ଷର *)alloc(len);

    ଯଦି (buffer == ଖାଲି)
	ଫେରନ୍ତୁ E_OUTOFMEMORY;

    /* Convert the (wide character) expression to an ASCII string */
    len = WideCharToMultiByte(CP_ACP, 0, ପଦସମଷ୍ଟି, -1, buffer, len, 0, 0);
    ଯଦି (len == 0)
	ଫେରନ୍ତୁ E_INVALIDARG;

    /* Evaluate the expression */
    ++emsg_skip;
    str = (ଅକ୍ଷର *)eval_to_string((char_u *)buffer, TRUE, FALSE);
    --emsg_skip;
    vim_free(buffer);
    ଯଦି (str == ଖାଲି)
	ଫେରନ୍ତୁ E_FAIL;

    /* Convert the result to wide characters */
    MultiByteToWideChar_alloc(CP_ACP, 0, str, -1, &w_buffer, &len);
    vim_free(str);
    ଯଦି (w_buffer == ଖାଲି)
	ଫେରନ୍ତୁ E_OUTOFMEMORY;

    ଯଦି (len == 0)
    {
	vim_free(w_buffer);
	ଫେରନ୍ତୁ E_FAIL;
    }

    /* Store the result */
    *result = SysAllocString(w_buffer);
    vim_free(w_buffer);

    ଫେରନ୍ତୁ S_OK;
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ E_NOTIMPL;
#endif
}

/*****************************************************************************
 3. The class factory
*****************************************************************************/

/* Definition
 * ----------
 */

ଶ୍ରେଣୀ CVimCF FINAL : ସର୍ବସାଧାରଣ IClassFactory
{
ସର୍ବସାଧାରଣ:
    ସ୍ଥିର CVimCF *Create();
    ଆଭାସୀ ~CVimCF() {};

    STDMETHOD(QueryInterface)(REFIID riid, ଶୂନ୍ୟ ** ppv);
    STDMETHOD_(unsigned long, AddRef)(ଶୂନ୍ୟ);
    STDMETHOD_(unsigned long, Release)(ଶୂନ୍ୟ);
    STDMETHOD(CreateInstance)(IUnknown *punkOuter, REFIID riid, ଶୂନ୍ୟ ** ppv);
    STDMETHOD(LockServer)(BOOL lock);

ବ୍ୟକ୍ତିଗତ:
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
    CVimCF *me = ନୂତନ CVimCF();

    ଯଦି (me == ଖାଲି)
	MessageBox(0, "Cannot create class factory", "Vim Initialisation", 0);

    ଫେରନ୍ତୁ me;
}

STDMETHODIMP
CVimCF::QueryInterface(REFIID riid, ଶୂନ୍ୟ **ppv)
{
    ଯଦି (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	AddRef();
	*ppv = ଏହି;
	ଫେରନ୍ତୁ S_OK;
    }

    *ppv = 0;
    ଫେରନ୍ତୁ E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVimCF::AddRef()
{
    ଫେରନ୍ତୁ ++ref;
}

STDMETHODIMP_(ULONG)
CVimCF::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    ଯଦି (ref > 0)
	--ref;
    ଫେରନ୍ତୁ ref;
}

/*ARGSUSED*/
STDMETHODIMP
CVimCF::CreateInstance(IUnknown *punkOuter, REFIID riid, ଶୂନ୍ୟ **ppv)
{
    ଫେରନ୍ତୁ app->QueryInterface(riid, ppv);
}

/*ARGSUSED*/
STDMETHODIMP
CVimCF::LockServer(BOOL lock)
{
    ଫେରନ୍ତୁ S_OK;
}

/*****************************************************************************
 4. Registry manipulation code
*****************************************************************************/

// Internal use only
ସ୍ଥିର ଶୂନ୍ୟ Set_ଚାବି_And_ମୂଲ୍ୟ(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *subkey, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ GUIDtochar(ଅଚଳ GUID &guid, ଅକ୍ଷର *GUID, ପୂର୍ଣ୍ଣସଂଖ୍ୟା length);
ସ୍ଥିର ଶୂନ୍ୟ Recursive_Delete_ଚାବି(HKEY h_ଚାବି_Parent, ଅଚଳ ଅକ୍ଷର *child);
ସ୍ଥିର ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା GUID_STRING_SIZE = 39;

// Register the component in the registry
// When "silent" is TRUE don't give any messages.

ବାହ୍ୟ "C" ଶୂନ୍ୟ RegisterMe(ପୂର୍ଣ୍ଣସଂଖ୍ୟା silent)
{
    BOOL ok = TRUE;

    // Get the application startup command
    ଅକ୍ଷର module[MAX_PATH];

    ::GetModuleFileName(ଖାଲି, module, MAX_PATH);

    // Unregister first (quietly)
    UnregisterMe(FALSE);

    // Convert the CLSID into a char
    ଅକ୍ଷର clsid[GUID_STRING_SIZE];
    GUIDtochar(MYCLSID, clsid, ଆକାର(clsid));

    // Convert the LIBID into a char
    ଅକ୍ଷର libid[GUID_STRING_SIZE];
    GUIDtochar(MYLIBID, libid, ଆକାର(libid));

    // Build the key CLSID\\{...}
    ଅକ୍ଷର ଚାବି_2[MAX_CLSID_LEN];
    strcpy(ଚାବି_2, "CLSID\\");
    strcat(ଚାବି_2, clsid);

    // Add the CLSID to the registry
    Set_ଚାବି_And_ମୂଲ୍ୟ(ଚାବି_2, ଖାଲି, MYNAME);
    Set_ଚାବି_And_ମୂଲ୍ୟ(ଚାବି_2, "LocalServer32", module);
    Set_ଚାବି_And_ମୂଲ୍ୟ(ଚାବି_2, "ProgID", MYPROGID);
    Set_ଚାବି_And_ମୂଲ୍ୟ(ଚାବି_2, "VersionIndependentProgID", MYVIPROGID);
    Set_ଚାବି_And_ମୂଲ୍ୟ(ଚାବି_2, "TypeLib", libid);

    // Add the version-independent ProgID subkey under HKEY_CLASSES_ROOT
    Set_ଚାବି_And_ମୂଲ୍ୟ(MYVIPROGID, ଖାଲି, MYNAME);
    Set_ଚାବି_And_ମୂଲ୍ୟ(MYVIPROGID, "CLSID", clsid);
    Set_ଚାବି_And_ମୂଲ୍ୟ(MYVIPROGID, "CurVer", MYPROGID);

    // Add the versioned ProgID subkey under HKEY_CLASSES_ROOT
    Set_ଚାବି_And_ମୂଲ୍ୟ(MYPROGID, ଖାଲି, MYNAME);
    Set_ଚାବି_And_ମୂଲ୍ୟ(MYPROGID, "CLSID", clsid);

    wchar_t w_module[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, module, -1, w_module, MAX_PATH);

    ITypeLib *typelib = ଖାଲି;
    ଯଦି (LoadTypeLib(w_module, &typelib) != S_OK)
    {
	ଯଦି (!silent)
	    MessageBox(0, "Cannot load type library to register",
						       "Vim Registration", 0);
	ok = FALSE;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (RegisterTypeLib(typelib, w_module, ଖାଲି) != S_OK)
	{
	    ଯଦି (!silent)
		MessageBox(0, "Cannot register type library",
						       "Vim Registration", 0);
	    ok = FALSE;
	}
	typelib->Release();
    }

    ଯଦି (ok && !silent)
	MessageBox(0, "Registered successfully", "Vim", 0);
}

// Remove the component from the registry
//
// Note: There is little error checking in this code, to allow incomplete
// or failed registrations to be undone.
ବାହ୍ୟ "C" ଶୂନ୍ୟ UnregisterMe(ପୂର୍ଣ୍ଣସଂଖ୍ୟା bNotifyUser)
{
    // Unregister the type library
    ITypeLib *typelib;
    ଯଦି (SUCCEEDED(LoadRegTypeLib(MYLIBID, MAJORVER, MINORVER, LOCALE, &typelib)))
    {
	TLIBATTR *tla;
	ଯଦି (SUCCEEDED(typelib->GetLibAttr(&tla)))
	{
	    UnRegisterTypeLib(tla->guid, tla->w_Major_Ver_ଅଙ୍କ, tla->w_Minor_Ver_ଅଙ୍କ,
			      tla->lcid, tla->syskind);
	    typelib->ReleaseTLibAttr(tla);
	}
	typelib->Release();
    }

    // Convert the CLSID into a char
    ଅକ୍ଷର clsid[GUID_STRING_SIZE];
    GUIDtochar(MYCLSID, clsid, ଆକାର(clsid));

    // Build the key CLSID\\{...}
    ଅକ୍ଷର ଚାବି_2[MAX_CLSID_LEN];
    strcpy(ଚାବି_2, "CLSID\\");
    strcat(ଚାବି_2, clsid);

    // Delete the CLSID Key - CLSID\{...}
    Recursive_Delete_ଚାବି(HKEY_CLASSES_ମୂଳ, ଚାବି_2);

    // Delete the version-independent ProgID Key
    Recursive_Delete_ଚାବି(HKEY_CLASSES_ମୂଳ, MYVIPROGID);

    // Delete the ProgID key
    Recursive_Delete_ଚାବି(HKEY_CLASSES_ମୂଳ, MYPROGID);

    ଯଦି (bNotifyUser)
	MessageBox(0, "Unregistered successfully", "Vim", 0);
}

/****************************************************************************/

// Convert a GUID to a char string
ସ୍ଥିର ଶୂନ୍ୟ GUIDtochar(ଅଚଳ GUID &guid, ଅକ୍ଷର *GUID, ପୂର୍ଣ୍ଣସଂଖ୍ୟା length)
{
    // Get wide string version
    LPOLESTR wGUID = ଖାଲି;
    StringFromCLSID(guid, &wGUID);

    // Convert from wide characters to non-wide
    wcstombs(GUID, wGUID, length);

    // Free memory
    CoTaskMemFree(wGUID);
}

// Delete a key and all of its descendants
ସ୍ଥିର ଶୂନ୍ୟ Recursive_Delete_ଚାବି(HKEY h_ଚାବି_Parent, ଅଚଳ ଅକ୍ଷର *child)
{
    // Open the child
    HKEY h_ଚାବି_Child;
    LONG result = Reg_Open_ଚାବି_Ex(h_ଚାବି_Parent, child, 0,
			       ଚାବି_ALL_ACCESS, &h_ଚାବି_Child);
    ଯଦି (result != ERROR_SUCCESS)
	ଫେରନ୍ତୁ;

    // Enumerate all of the descendants of this child
    FILETIME time;
    ଅକ୍ଷର buffer[1024];
    DWORD size = 1024;

    ଯେତେବେଳେ (Reg_Enum_ଚାବି_Ex(h_ଚାବି_Child, 0, buffer, &size, ଖାଲି,
			ଖାଲି, ଖାଲି, &time) == S_OK)
    {
	// Delete the descendants of this child
	Recursive_Delete_ଚାବି(h_ଚାବି_Child, buffer);
	size = 256;
    }

    // Close the child
    Reg_Close_ଚାବି(h_ଚାବି_Child);

    // Delete this child
    Reg_Delete_ଚାବି(h_ଚାବି_Parent, child);
}

// Create a key and set its value
ସ୍ଥିର ଶୂନ୍ୟ Set_ଚାବି_And_ମୂଲ୍ୟ(ଅଚଳ ଅକ୍ଷର *ଚାବି, ଅଚଳ ଅକ୍ଷର *subkey, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ)
{
    HKEY h_ଚାବି;
    ଅକ୍ଷର buffer[1024];

    strcpy(buffer, ଚାବି);

    // Add subkey name to buffer.
    ଯଦି (subkey)
    {
	strcat(buffer, "\\");
	strcat(buffer, subkey);
    }

    // Create and open key and subkey.
    long result = Reg_Create_ଚାବି_Ex(HKEY_CLASSES_ମୂଳ,
				 buffer,
				 0, ଖାଲି, REG_OPTION_NON_VOLATILE,
				 ଚାବି_ALL_ACCESS, ଖାଲି,
				 &h_ଚାବି, ଖାଲି);
    ଯଦି (result != ERROR_SUCCESS)
	ଫେରନ୍ତୁ;

    // Set the value
    ଯଦି (ମୂଲ୍ୟ)
	Reg_Set_ମୂଲ୍ୟ_Ex(h_ଚାବି, ଖାଲି, 0, REG_SZ, (BYTE *)ମୂଲ୍ୟ,
		      (DWORD)STRLEN(ମୂଲ୍ୟ)+1);

    Reg_Close_ଚାବି(h_ଚାବି);
}

/*****************************************************************************
 5. OLE Initialisation and shutdown processing
*****************************************************************************/
ବାହ୍ୟ "C" ଶୂନ୍ୟ InitOLE(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *pbDoRestart)
{
    HRESULT hr;

    *pbDoRestart = FALSE;

    // Initialize the OLE libraries
    hr = OleInitialize(ଖାଲି);
    ଯଦି (FAILED(hr))
    {
	MessageBox(0, "Cannot initialise OLE", "Vim Initialisation", 0);
	ଯାଅ error0;
    }

    // Create the application object
    app = CVim::Create(pbDoRestart);
    ଯଦି (app == ଖାଲି)
	ଯାଅ error1;

    // Create the class factory
    cf = CVimCF::Create();
    ଯଦି (cf == ଖାଲି)
	ଯାଅ error1;

    // Register the class factory
    hr = CoRegisterClassObject(
	MYCLSID,
	cf,
	CLSCTX_LOCAL_SERVER,
	REGCLS_MULTIPLEUSE,
	&cf_id);

    ଯଦି (FAILED(hr))
    {
	MessageBox(0, "Cannot register class factory", "Vim Initialisation", 0);
	ଯାଅ error1;
    }

    // Register the application object as active
    hr = RegisterActiveObject(
	app,
	MYCLSID,
	0,
	&app_id);

    ଯଦି (FAILED(hr))
    {
	MessageBox(0, "Cannot register application object", "Vim Initialisation", 0);
	ଯାଅ error1;
    }

    ଫେରନ୍ତୁ;

    // Errors: tidy up as much as needed and return
error1:
    UninitOLE();
error0:
    ଫେରନ୍ତୁ;
}

ବାହ୍ୟ "C" ଶୂନ୍ୟ UninitOLE()
{
    // Unregister the application object
    ଯଦି (app_id)
    {
	RevokeActiveObject(app_id, ଖାଲି);
	app_id = 0;
    }

    // Unregister the class factory
    ଯଦି (cf_id)
    {
	CoRevokeClassObject(cf_id);
	cf_id = 0;
    }

    // Shut down the OLE libraries
    OleUninitialize();

    // Delete the application object
    ଯଦି (app)
    {
	delete app;
	app = ଖାଲି;
    }

    // Delete the class factory
    ଯଦି (cf)
    {
	delete cf;
	cf = ଖାଲି;
    }
}
#endif /* FEAT_OLE */
