/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#যদি defined(FEAT_OLE) && defined(FEAT_GUI_MSWIN)
/*
 * OLE server implementation.
 *
 * See os_mswin.c for the client side.
 */
বাহ্যিক "C" {
# include "vim.h"
}

#অন্তর্ভাব <windows.h>
#অন্তর্ভাব <oleauto.h>

বাহ্যিক "C" {
বাহ্যিক HWND s_hwnd;
বাহ্যিক HWND vim_parent_hwnd;
}

#যদি (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#অন্যথা
# define FINAL
#endif

#অন্তর্ভাব "if_ole.h"	// Interface definitions
#অন্তর্ভাব "iid_ole.c"	// UUID definitions (compile here)

/* Supply function prototype to work around bug in Mingw oleauto.h header */
#ifdef __MINGW32__
WINOLEAUTAPI UnRegisterTypeLib(REFGUID libID, WORD wVerMajor,
	    WORD wVerMinor, LCID lcid, SYSKIND syskind);
#endif

/*****************************************************************************
 1. Internal definitions for this file
*****************************************************************************/

শ্রেণী CVim;
শ্রেণী CVimCF;

/* Internal data */
// The identifier of the registered class factory
স্থির unsigned long cf_id = 0;

// The identifier of the running application object
স্থির unsigned long app_id = 0;

// The single global instance of the class factory
স্থির CVimCF *cf = 0;

// The single global instance of the application object
স্থির CVim *app = 0;

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

শ্রেণী CVim FINAL : সর্বজনীন IVim
{
সর্বজনীন:
    আভাসী ~CVim();
    স্থির CVim *Create(পূর্ণসংখ্যা *pbDoRestart);

    // IUnknown members
    STDMETHOD(QueryInterface)(REFIID riid, শূন্য ** ppv);
    STDMETHOD_(unsigned long, AddRef)(শূন্য);
    STDMETHOD_(unsigned long, Release)(শূন্য);

    // IDispatch members
    STDMETHOD(Get_Type_Info_গুনতি)(UINT *p_গুনতি);
    STDMETHOD(GetTypeInfo)(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo);
    STDMETHOD(GetIDsOfNames)(ধ্রুবক IID &iid, OLECHAR **names, UINT n, LCID, DISPID *dispids);
    STDMETHOD(Invoke)(DISPID member, ধ্রুবক IID &iid, LCID, WORD flags, DISPPARAMS *dispparams, VARIANT *ফলাফল, EXCEPINFO *excepinfo, UINT *argerr);

    // IVim members
    STDMETHOD(Send_চাবিগুলি)(BSTR চাবিগুলি);
    STDMETHOD(Eval)(BSTR রাশিমালা, BSTR *ফলাফল);
    STDMETHOD(SetForeground)(শূন্য);
    STDMETHOD(GetHwnd)(UINT_PTR *ফলাফল);

ব্যক্তিগত:
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

CVim *CVim::Create(পূর্ণসংখ্যা *pbDoRestart)
{
    HRESULT hr;
    CVim *me = 0;
    ITypeLib *typelib = 0;
    ITypeInfo *typeinfo = 0;

    *pbDoRestart = FALSE;

    // Create the object
    me = নতুন CVim();
    যদি (me == ফাঁকা)
    {
	MessageBox(0, "Cannot create application object", "Vim Initialisation", 0);
	ফেরত ফাঁকা;
    }

    // Load the type library from the registry
    hr = LoadRegTypeLib(MYLIBID, 1, 0, 0x00, &typelib);
    যদি (FAILED(hr))
    {
	HKEY h_চাবি;

	// Check we can write to the registry.
	// RegCreateKeyEx succeeds even if key exists. W.Briscoe W2K 20021011
	যদি (Reg_Create_চাবি_Ex(HKEY_CLASSES_মূল, MYVIPROGID, 0, ফাঁকা,
		  REG_OPTION_NON_VOLATILE,
		  চাবি_ALL_ACCESS, ফাঁকা, &h_চাবি, ফাঁকা))
	{
	    delete me;
	    ফেরত ফাঁকা; // Unable to write to registry. Quietly fail.
	}
	Reg_Close_চাবি(h_চাবি);

	যদি (MessageBox(0, "Cannot load registered type library.\nDo you want to register Vim now?",
		    "Vim Initialisation", MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
	    delete me;
	    ফেরত ফাঁকা;
	}

	RegisterMe(FALSE);

	// Load the type library from the registry
	hr = LoadRegTypeLib(MYLIBID, 1, 0, 0x00, &typelib);
	যদি (FAILED(hr))
	{
	    MessageBox(0, "You must restart Vim in order for the registration to take effect.",
						     "Vim Initialisation", 0);
	    *pbDoRestart = TRUE;
	    delete me;
	    ফেরত ফাঁকা;
	}
    }

    // Get the type info of the vtable interface
    hr = typelib->GetTypeInfoOfGuid(MYIID, &typeinfo);
    typelib->Release();

    যদি (FAILED(hr))
    {
	MessageBox(0, "Cannot get interface type information",
						     "Vim Initialisation", 0);
	delete me;
	ফেরত ফাঁকা;
    }

    // Save the type information
    me->typeinfo = typeinfo;
    ফেরত me;
}

CVim::~CVim()
{
    যদি (typeinfo && vim_parent_hwnd == ফাঁকা)
	typeinfo->Release();
    typeinfo = 0;
}

STDMETHODIMP
CVim::QueryInterface(REFIID riid, শূন্য **ppv)
{
    যদি (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDispatch) || IsEqualIID(riid, MYIID))
    {
	AddRef();
	*ppv = এই;
	ফেরত S_OK;
    }

    *ppv = 0;
    ফেরত E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVim::AddRef()
{
    ফেরত ++ref;
}

STDMETHODIMP_(ULONG)
CVim::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    যদি (ref > 0)
	--ref;
    ফেরত ref;
}

STDMETHODIMP
CVim::Get_Type_Info_গুনতি(UINT *p_গুনতি)
{
    *p_গুনতি = 1;
    ফেরত S_OK;
}

STDMETHODIMP
CVim::GetTypeInfo(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo)
{
    *ppITypeInfo = 0;

    যদি (iTypeInfo != 0)
	ফেরত DISP_E_BADINDEX;

    typeinfo->AddRef();
    *ppITypeInfo = typeinfo;
    ফেরত S_OK;
}

STDMETHODIMP
CVim::GetIDsOfNames(
	ধ্রুবক IID &iid,
	OLECHAR **names,
	UINT n,
	LCID,
	DISPID *dispids)
{
    যদি (iid != IID_NULL)
	ফেরত DISP_E_UNKNOWNINTERFACE;

    ফেরত typeinfo->GetIDsOfNames(names, n, dispids);
}

STDMETHODIMP
CVim::Invoke(
	DISPID member,
	ধ্রুবক IID &iid,
	LCID,
	WORD flags,
	DISPPARAMS *dispparams,
	VARIANT *ফলাফল,
	EXCEPINFO *excepinfo,
	UINT *argerr)
{
    যদি (iid != IID_NULL)
	ফেরত DISP_E_UNKNOWNINTERFACE;

    ::SetErrorInfo(0, ফাঁকা);
    ফেরত typeinfo->Invoke(static_cast<IDispatch*>(এই),
			    member, flags, dispparams,
			    ফলাফল, excepinfo, argerr);
}

STDMETHODIMP
CVim::GetHwnd(UINT_PTR *ফলাফল)
{
    *ফলাফল = (UINT_PTR)s_hwnd;
    ফেরত S_OK;
}

STDMETHODIMP
CVim::SetForeground(শূন্য)
{
    /* Make the Vim window come to the foreground */
    gui_mch_set_foreground();
    ফেরত S_OK;
}

STDMETHODIMP
CVim::Send_চাবিগুলি(BSTR চাবিগুলি)
{
    পূর্ণসংখ্যা len;
    অক্ষর *বাফার;
    char_u *str;
    char_u *ptr;

    /* Get a suitable buffer */
    len = WideCharToMultiByte(CP_ACP, 0, চাবিগুলি, -1, 0, 0, 0, 0);
    বাফার = (অক্ষর *)alloc(len+1);

    যদি (বাফার == ফাঁকা)
	ফেরত E_OUTOFMEMORY;

    len = WideCharToMultiByte(CP_ACP, 0, চাবিগুলি, -1, বাফার, len, 0, 0);

    যদি (len == 0)
    {
	vim_free(বাফার);
	ফেরত E_INVALIDARG;
    }

    /* Translate key codes like <Esc> */
    str = replace_termcodes((char_u *)বাফার, &ptr, 0, REPTERM_DO_LT, ফাঁকা);

    /* If ptr was set, then a new buffer was allocated,
     * so we can free the old one.
     */
    যদি (ptr)
	vim_free((char_u *)(বাফার));

    /* Reject strings too long to fit in the input buffer. Allow 10 bytes
     * space to cover for the (remote) possibility that characters may enter
     * the input buffer between now and when the WM_OLE message is actually
     * processed. If more than 10 characters enter the input buffer in that
     * time, the WM_OLE processing will simply fail to insert the characters.
     */
    যদি ((পূর্ণসংখ্যা)(STRLEN(str)) > (vim_free_in_প্রবেশ_buf() - 10))
    {
	vim_free(str);
	ফেরত E_INVALIDARG;
    }

    /* Pass the string to the main input loop. The memory will be freed when
     * the message is processed.  Except for an empty message, we don't need
     * to post it then.
     */
    যদি (*str == NUL)
	vim_free(str);
    অন্যথা
	PostMessage(ফাঁকা, WM_OLE, 0, (LPARAM)str);

    ফেরত S_OK;
}

STDMETHODIMP
CVim::Eval(BSTR রাশিমালা, BSTR *ফলাফল)
{
#ifdef FEAT_EVAL
    পূর্ণসংখ্যা len;
    অক্ষর *বাফার;
    অক্ষর *str;
    wchar_t *w_বাফার;

    /* Get a suitable buffer */
    len = WideCharToMultiByte(CP_ACP, 0, রাশিমালা, -1, 0, 0, 0, 0);
    যদি (len == 0)
	ফেরত E_INVALIDARG;

    বাফার = (অক্ষর *)alloc(len);

    যদি (বাফার == ফাঁকা)
	ফেরত E_OUTOFMEMORY;

    /* Convert the (wide character) expression to an ASCII string */
    len = WideCharToMultiByte(CP_ACP, 0, রাশিমালা, -1, বাফার, len, 0, 0);
    যদি (len == 0)
	ফেরত E_INVALIDARG;

    /* Evaluate the expression */
    ++emsg_skip;
    str = (অক্ষর *)eval_to_string((char_u *)বাফার, TRUE, FALSE);
    --emsg_skip;
    vim_free(বাফার);
    যদি (str == ফাঁকা)
	ফেরত E_FAIL;

    /* Convert the result to wide characters */
    MultiByteToWideChar_alloc(CP_ACP, 0, str, -1, &w_বাফার, &len);
    vim_free(str);
    যদি (w_বাফার == ফাঁকা)
	ফেরত E_OUTOFMEMORY;

    যদি (len == 0)
    {
	vim_free(w_বাফার);
	ফেরত E_FAIL;
    }

    /* Store the result */
    *ফলাফল = SysAllocString(w_বাফার);
    vim_free(w_বাফার);

    ফেরত S_OK;
#অন্যথা
    ফেরত E_NOTIMPL;
#endif
}

/*****************************************************************************
 3. The class factory
*****************************************************************************/

/* Definition
 * ----------
 */

শ্রেণী CVimCF FINAL : সর্বজনীন IClassFactory
{
সর্বজনীন:
    স্থির CVimCF *Create();
    আভাসী ~CVimCF() {};

    STDMETHOD(QueryInterface)(REFIID riid, শূন্য ** ppv);
    STDMETHOD_(unsigned long, AddRef)(শূন্য);
    STDMETHOD_(unsigned long, Release)(শূন্য);
    STDMETHOD(CreateInstance)(IUnknown *punkOuter, REFIID riid, শূন্য ** ppv);
    STDMETHOD(LockServer)(BOOL lock);

ব্যক্তিগত:
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
    CVimCF *me = নতুন CVimCF();

    যদি (me == ফাঁকা)
	MessageBox(0, "Cannot create class factory", "Vim Initialisation", 0);

    ফেরত me;
}

STDMETHODIMP
CVimCF::QueryInterface(REFIID riid, শূন্য **ppv)
{
    যদি (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	AddRef();
	*ppv = এই;
	ফেরত S_OK;
    }

    *ppv = 0;
    ফেরত E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVimCF::AddRef()
{
    ফেরত ++ref;
}

STDMETHODIMP_(ULONG)
CVimCF::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    যদি (ref > 0)
	--ref;
    ফেরত ref;
}

/*ARGSUSED*/
STDMETHODIMP
CVimCF::CreateInstance(IUnknown *punkOuter, REFIID riid, শূন্য **ppv)
{
    ফেরত app->QueryInterface(riid, ppv);
}

/*ARGSUSED*/
STDMETHODIMP
CVimCF::LockServer(BOOL lock)
{
    ফেরত S_OK;
}

/*****************************************************************************
 4. Registry manipulation code
*****************************************************************************/

// Internal use only
স্থির শূন্য Set_চাবি_And_মান(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *subkey, ধ্রুবক অক্ষর *মান);
স্থির শূন্য GUIDtochar(ধ্রুবক GUID &guid, অক্ষর *GUID, পূর্ণসংখ্যা দৈর্ঘ্য);
স্থির শূন্য Recursive_Delete_চাবি(HKEY h_চাবি_Parent, ধ্রুবক অক্ষর *child);
স্থির ধ্রুবক পূর্ণসংখ্যা GUID_STRING_আকার = 39;

// Register the component in the registry
// When "silent" is TRUE don't give any messages.

বাহ্যিক "C" শূন্য RegisterMe(পূর্ণসংখ্যা silent)
{
    BOOL ok = TRUE;

    // Get the application startup command
    অক্ষর module[MAX_PATH];

    ::GetModuleFileName(ফাঁকা, module, MAX_PATH);

    // Unregister first (quietly)
    UnregisterMe(FALSE);

    // Convert the CLSID into a char
    অক্ষর clsid[GUID_STRING_আকার];
    GUIDtochar(MYCLSID, clsid, আকার(clsid));

    // Convert the LIBID into a char
    অক্ষর libid[GUID_STRING_আকার];
    GUIDtochar(MYLIBID, libid, আকার(libid));

    // Build the key CLSID\\{...}
    অক্ষর চাবি_2[MAX_CLSID_LEN];
    strcpy(চাবি_2, "CLSID\\");
    strcat(চাবি_2, clsid);

    // Add the CLSID to the registry
    Set_চাবি_And_মান(চাবি_2, ফাঁকা, MYNAME);
    Set_চাবি_And_মান(চাবি_2, "LocalServer32", module);
    Set_চাবি_And_মান(চাবি_2, "ProgID", MYPROGID);
    Set_চাবি_And_মান(চাবি_2, "VersionIndependentProgID", MYVIPROGID);
    Set_চাবি_And_মান(চাবি_2, "TypeLib", libid);

    // Add the version-independent ProgID subkey under HKEY_CLASSES_ROOT
    Set_চাবি_And_মান(MYVIPROGID, ফাঁকা, MYNAME);
    Set_চাবি_And_মান(MYVIPROGID, "CLSID", clsid);
    Set_চাবি_And_মান(MYVIPROGID, "CurVer", MYPROGID);

    // Add the versioned ProgID subkey under HKEY_CLASSES_ROOT
    Set_চাবি_And_মান(MYPROGID, ফাঁকা, MYNAME);
    Set_চাবি_And_মান(MYPROGID, "CLSID", clsid);

    wchar_t w_module[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, module, -1, w_module, MAX_PATH);

    ITypeLib *typelib = ফাঁকা;
    যদি (LoadTypeLib(w_module, &typelib) != S_OK)
    {
	যদি (!silent)
	    MessageBox(0, "Cannot load type library to register",
						       "Vim Registration", 0);
	ok = FALSE;
    }
    অন্যথা
    {
	যদি (RegisterTypeLib(typelib, w_module, ফাঁকা) != S_OK)
	{
	    যদি (!silent)
		MessageBox(0, "Cannot register type library",
						       "Vim Registration", 0);
	    ok = FALSE;
	}
	typelib->Release();
    }

    যদি (ok && !silent)
	MessageBox(0, "Registered successfully", "Vim", 0);
}

// Remove the component from the registry
//
// Note: There is little error checking in this code, to allow incomplete
// or failed registrations to be undone.
বাহ্যিক "C" শূন্য UnregisterMe(পূর্ণসংখ্যা bNotifyUser)
{
    // Unregister the type library
    ITypeLib *typelib;
    যদি (SUCCEEDED(LoadRegTypeLib(MYLIBID, MAJORVER, MINORVER, LOCALE, &typelib)))
    {
	TLIBATTR *tla;
	যদি (SUCCEEDED(typelib->GetLibAttr(&tla)))
	{
	    UnRegisterTypeLib(tla->guid, tla->w_Major_Ver_অঙ্ক, tla->w_Minor_Ver_অঙ্ক,
			      tla->lcid, tla->syskind);
	    typelib->ReleaseTLibAttr(tla);
	}
	typelib->Release();
    }

    // Convert the CLSID into a char
    অক্ষর clsid[GUID_STRING_আকার];
    GUIDtochar(MYCLSID, clsid, আকার(clsid));

    // Build the key CLSID\\{...}
    অক্ষর চাবি_2[MAX_CLSID_LEN];
    strcpy(চাবি_2, "CLSID\\");
    strcat(চাবি_2, clsid);

    // Delete the CLSID Key - CLSID\{...}
    Recursive_Delete_চাবি(HKEY_CLASSES_মূল, চাবি_2);

    // Delete the version-independent ProgID Key
    Recursive_Delete_চাবি(HKEY_CLASSES_মূল, MYVIPROGID);

    // Delete the ProgID key
    Recursive_Delete_চাবি(HKEY_CLASSES_মূল, MYPROGID);

    যদি (bNotifyUser)
	MessageBox(0, "Unregistered successfully", "Vim", 0);
}

/****************************************************************************/

// Convert a GUID to a char string
স্থির শূন্য GUIDtochar(ধ্রুবক GUID &guid, অক্ষর *GUID, পূর্ণসংখ্যা দৈর্ঘ্য)
{
    // Get wide string version
    LPOLESTR wGUID = ফাঁকা;
    StringFromCLSID(guid, &wGUID);

    // Convert from wide characters to non-wide
    wcstombs(GUID, wGUID, দৈর্ঘ্য);

    // Free memory
    CoTaskMemFree(wGUID);
}

// Delete a key and all of its descendants
স্থির শূন্য Recursive_Delete_চাবি(HKEY h_চাবি_Parent, ধ্রুবক অক্ষর *child)
{
    // Open the child
    HKEY h_চাবি_Child;
    LONG ফলাফল = Reg_Open_চাবি_Ex(h_চাবি_Parent, child, 0,
			       চাবি_ALL_ACCESS, &h_চাবি_Child);
    যদি (ফলাফল != ERROR_SUCCESS)
	ফেরত;

    // Enumerate all of the descendants of this child
    FILETIME time;
    অক্ষর বাফার[1024];
    DWORD আকার_2 = 1024;

    যখন (Reg_Enum_চাবি_Ex(h_চাবি_Child, 0, বাফার, &আকার_2, ফাঁকা,
			ফাঁকা, ফাঁকা, &time) == S_OK)
    {
	// Delete the descendants of this child
	Recursive_Delete_চাবি(h_চাবি_Child, বাফার);
	আকার_2 = 256;
    }

    // Close the child
    Reg_Close_চাবি(h_চাবি_Child);

    // Delete this child
    Reg_Delete_চাবি(h_চাবি_Parent, child);
}

// Create a key and set its value
স্থির শূন্য Set_চাবি_And_মান(ধ্রুবক অক্ষর *চাবি, ধ্রুবক অক্ষর *subkey, ধ্রুবক অক্ষর *মান)
{
    HKEY h_চাবি;
    অক্ষর বাফার[1024];

    strcpy(বাফার, চাবি);

    // Add subkey name to buffer.
    যদি (subkey)
    {
	strcat(বাফার, "\\");
	strcat(বাফার, subkey);
    }

    // Create and open key and subkey.
    long ফলাফল = Reg_Create_চাবি_Ex(HKEY_CLASSES_মূল,
				 বাফার,
				 0, ফাঁকা, REG_OPTION_NON_VOLATILE,
				 চাবি_ALL_ACCESS, ফাঁকা,
				 &h_চাবি, ফাঁকা);
    যদি (ফলাফল != ERROR_SUCCESS)
	ফেরত;

    // Set the value
    যদি (মান)
	Reg_Set_মান_Ex(h_চাবি, ফাঁকা, 0, REG_SZ, (BYTE *)মান,
		      (DWORD)STRLEN(মান)+1);

    Reg_Close_চাবি(h_চাবি);
}

/*****************************************************************************
 5. OLE Initialisation and shutdown processing
*****************************************************************************/
বাহ্যিক "C" শূন্য InitOLE(পূর্ণসংখ্যা *pbDoRestart)
{
    HRESULT hr;

    *pbDoRestart = FALSE;

    // Initialize the OLE libraries
    hr = OleInitialize(ফাঁকা);
    যদি (FAILED(hr))
    {
	MessageBox(0, "Cannot initialise OLE", "Vim Initialisation", 0);
	যাও error0;
    }

    // Create the application object
    app = CVim::Create(pbDoRestart);
    যদি (app == ফাঁকা)
	যাও error1;

    // Create the class factory
    cf = CVimCF::Create();
    যদি (cf == ফাঁকা)
	যাও error1;

    // Register the class factory
    hr = CoRegisterClassObject(
	MYCLSID,
	cf,
	CLSCTX_LOCAL_SERVER,
	REGCLS_MULTIPLEUSE,
	&cf_id);

    যদি (FAILED(hr))
    {
	MessageBox(0, "Cannot register class factory", "Vim Initialisation", 0);
	যাও error1;
    }

    // Register the application object as active
    hr = RegisterActiveObject(
	app,
	MYCLSID,
	0,
	&app_id);

    যদি (FAILED(hr))
    {
	MessageBox(0, "Cannot register application object", "Vim Initialisation", 0);
	যাও error1;
    }

    ফেরত;

    // Errors: tidy up as much as needed and return
error1:
    UninitOLE();
error0:
    ফেরত;
}

বাহ্যিক "C" শূন্য UninitOLE()
{
    // Unregister the application object
    যদি (app_id)
    {
	RevokeActiveObject(app_id, ফাঁকা);
	app_id = 0;
    }

    // Unregister the class factory
    যদি (cf_id)
    {
	CoRevokeClassObject(cf_id);
	cf_id = 0;
    }

    // Shut down the OLE libraries
    OleUninitialize();

    // Delete the application object
    যদি (app)
    {
	delete app;
	app = ফাঁকা;
    }

    // Delete the class factory
    যদি (cf)
    {
	delete cf;
	cf = ফাঁকা;
    }
}
#endif /* FEAT_OLE */
