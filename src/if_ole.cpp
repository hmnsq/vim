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
বাহ্য "C" {
# include "vim.h"
}

#অন্তর্ভাৱ <windows.h>
#অন্তর্ভাৱ <oleauto.h>

বাহ্য "C" {
বাহ্য HWND s_hwnd;
বাহ্য HWND vim_parent_hwnd;
}

#যদি (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#অন্যথা
# define FINAL
#endif

#অন্তর্ভাৱ "if_ole.h"	// Interface definitions
#অন্তর্ভাৱ "iid_ole.c"	// UUID definitions (compile here)

/* Supply function prototype to work around bug in Mingw oleauto.h header */
#ifdef __MINGW32__
WINOLEAUTAPI UnRegisterTypeLib(REFGUID libID, WORD wVerMajor,
	    WORD wVerMinor, LCID lcid, SYSKIND syskind);
#endif

/*****************************************************************************
 1. Internal definitions for this file
*****************************************************************************/

শ্ৰেণী CVim;
শ্ৰেণী CVimCF;

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

শ্ৰেণী CVim FINAL : সার্ৱজনিক IVim
{
সার্ৱজনিক:
    আভাস ~CVim();
    স্থির CVim *Create(পূর্ণাঙ্ক *pbDoRestart);

    // IUnknown members
    STDMETHOD(QueryInterface)(REFIID riid, শূন্য ** ppv);
    STDMETHOD_(unsigned long, AddRef)(শূন্য);
    STDMETHOD_(unsigned long, Release)(শূন্য);

    // IDispatch members
    STDMETHOD(Get_Type_Info_গুনতি)(UINT *p_গুনতি);
    STDMETHOD(GetTypeInfo)(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo);
    STDMETHOD(GetIDsOfNames)(ধ্রুৱ IID &iid, OLECHAR **names, UINT n, LCID, DISPID *dispids);
    STDMETHOD(Invoke)(DISPID member, ধ্রুৱ IID &iid, LCID, WORD flags, DISPPARAMS *dispparams, VARIANT *result, EXCEPINFO *excepinfo, UINT *argerr);

    // IVim members
    STDMETHOD(Send_চাবিগুলি)(BSTR চাবিগুলি);
    STDMETHOD(Eval)(BSTR ৰাশিমালা, BSTR *result);
    STDMETHOD(SetForeground)(শূন্য);
    STDMETHOD(GetHwnd)(UINT_PTR *result);

গুপ্ত:
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

CVim *CVim::Create(পূর্ণাঙ্ক *pbDoRestart)
{
    HRESULT hr;
    CVim *me = 0;
    ITypeLib *typelib = 0;
    ITypeInfo *typeinfo = 0;

    *pbDoRestart = FALSE;

    // Create the object
    me = নূতন CVim();
    যদি (me == রিক্ত)
    {
	MessageBox(0, "Cannot create application object", "Vim Initialisation", 0);
	ঘূৰাই রিক্ত;
    }

    // Load the type library from the registry
    hr = LoadRegTypeLib(MYLIBID, 1, 0, 0x00, &typelib);
    যদি (FAILED(hr))
    {
	HKEY h_চাবি;

	// Check we can write to the registry.
	// RegCreateKeyEx succeeds even if key exists. W.Briscoe W2K 20021011
	যদি (Reg_Create_চাবি_Ex(HKEY_CLASSES_মূল, MYVIPROGID, 0, রিক্ত,
		  REG_OPTION_NON_VOLATILE,
		  চাবি_ALL_ACCESS, রিক্ত, &h_চাবি, রিক্ত))
	{
	    delete me;
	    ঘূৰাই রিক্ত; // Unable to write to registry. Quietly fail.
	}
	Reg_Close_চাবি(h_চাবি);

	যদি (MessageBox(0, "Cannot load registered type library.\nDo you want to register Vim now?",
		    "Vim Initialisation", MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
	    delete me;
	    ঘূৰাই রিক্ত;
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
	    ঘূৰাই রিক্ত;
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
	ঘূৰাই রিক্ত;
    }

    // Save the type information
    me->typeinfo = typeinfo;
    ঘূৰাই me;
}

CVim::~CVim()
{
    যদি (typeinfo && vim_parent_hwnd == রিক্ত)
	typeinfo->Release();
    typeinfo = 0;
}

STDMETHODIMP
CVim::QueryInterface(REFIID riid, শূন্য **ppv)
{
    যদি (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDispatch) || IsEqualIID(riid, MYIID))
    {
	AddRef();
	*ppv = এতত্;
	ঘূৰাই S_OK;
    }

    *ppv = 0;
    ঘূৰাই E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVim::AddRef()
{
    ঘূৰাই ++ref;
}

STDMETHODIMP_(ULONG)
CVim::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    যদি (ref > 0)
	--ref;
    ঘূৰাই ref;
}

STDMETHODIMP
CVim::Get_Type_Info_গুনতি(UINT *p_গুনতি)
{
    *p_গুনতি = 1;
    ঘূৰাই S_OK;
}

STDMETHODIMP
CVim::GetTypeInfo(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo)
{
    *ppITypeInfo = 0;

    যদি (iTypeInfo != 0)
	ঘূৰাই DISP_E_BADINDEX;

    typeinfo->AddRef();
    *ppITypeInfo = typeinfo;
    ঘূৰাই S_OK;
}

STDMETHODIMP
CVim::GetIDsOfNames(
	ধ্রুৱ IID &iid,
	OLECHAR **names,
	UINT n,
	LCID,
	DISPID *dispids)
{
    যদি (iid != IID_NULL)
	ঘূৰাই DISP_E_UNKNOWNINTERFACE;

    ঘূৰাই typeinfo->GetIDsOfNames(names, n, dispids);
}

STDMETHODIMP
CVim::Invoke(
	DISPID member,
	ধ্রুৱ IID &iid,
	LCID,
	WORD flags,
	DISPPARAMS *dispparams,
	VARIANT *result,
	EXCEPINFO *excepinfo,
	UINT *argerr)
{
    যদি (iid != IID_NULL)
	ঘূৰাই DISP_E_UNKNOWNINTERFACE;

    ::SetErrorInfo(0, রিক্ত);
    ঘূৰাই typeinfo->Invoke(static_cast<IDispatch*>(এতত্),
			    member, flags, dispparams,
			    result, excepinfo, argerr);
}

STDMETHODIMP
CVim::GetHwnd(UINT_PTR *result)
{
    *result = (UINT_PTR)s_hwnd;
    ঘূৰাই S_OK;
}

STDMETHODIMP
CVim::SetForeground(শূন্য)
{
    /* Make the Vim window come to the foreground */
    gui_mch_set_foreground();
    ঘূৰাই S_OK;
}

STDMETHODIMP
CVim::Send_চাবিগুলি(BSTR চাবিগুলি)
{
    পূর্ণাঙ্ক len;
    ৱর্ণ *buffer;
    char_u *str;
    char_u *ptr;

    /* Get a suitable buffer */
    len = WideCharToMultiByte(CP_ACP, 0, চাবিগুলি, -1, 0, 0, 0, 0);
    buffer = (ৱর্ণ *)alloc(len+1);

    যদি (buffer == রিক্ত)
	ঘূৰাই E_OUTOFMEMORY;

    len = WideCharToMultiByte(CP_ACP, 0, চাবিগুলি, -1, buffer, len, 0, 0);

    যদি (len == 0)
    {
	vim_free(buffer);
	ঘূৰাই E_INVALIDARG;
    }

    /* Translate key codes like <Esc> */
    str = replace_termcodes((char_u *)buffer, &ptr, 0, REPTERM_DO_LT, রিক্ত);

    /* If ptr was set, then a new buffer was allocated,
     * so we can free the old one.
     */
    যদি (ptr)
	vim_free((char_u *)(buffer));

    /* Reject strings too long to fit in the input buffer. Allow 10 bytes
     * space to cover for the (remote) possibility that characters may enter
     * the input buffer between now and when the WM_OLE message is actually
     * processed. If more than 10 characters enter the input buffer in that
     * time, the WM_OLE processing will simply fail to insert the characters.
     */
    যদি ((পূর্ণাঙ্ক)(STRLEN(str)) > (vim_free_in_প্ৰবেশ_buf() - 10))
    {
	vim_free(str);
	ঘূৰাই E_INVALIDARG;
    }

    /* Pass the string to the main input loop. The memory will be freed when
     * the message is processed.  Except for an empty message, we don't need
     * to post it then.
     */
    যদি (*str == NUL)
	vim_free(str);
    অন্যথা
	PostMessage(রিক্ত, WM_OLE, 0, (LPARAM)str);

    ঘূৰাই S_OK;
}

STDMETHODIMP
CVim::Eval(BSTR ৰাশিমালা, BSTR *result)
{
#ifdef FEAT_EVAL
    পূর্ণাঙ্ক len;
    ৱর্ণ *buffer;
    ৱর্ণ *str;
    wchar_t *w_buffer;

    /* Get a suitable buffer */
    len = WideCharToMultiByte(CP_ACP, 0, ৰাশিমালা, -1, 0, 0, 0, 0);
    যদি (len == 0)
	ঘূৰাই E_INVALIDARG;

    buffer = (ৱর্ণ *)alloc(len);

    যদি (buffer == রিক্ত)
	ঘূৰাই E_OUTOFMEMORY;

    /* Convert the (wide character) expression to an ASCII string */
    len = WideCharToMultiByte(CP_ACP, 0, ৰাশিমালা, -1, buffer, len, 0, 0);
    যদি (len == 0)
	ঘূৰাই E_INVALIDARG;

    /* Evaluate the expression */
    ++emsg_skip;
    str = (ৱর্ণ *)eval_to_string((char_u *)buffer, TRUE, FALSE);
    --emsg_skip;
    vim_free(buffer);
    যদি (str == রিক্ত)
	ঘূৰাই E_FAIL;

    /* Convert the result to wide characters */
    MultiByteToWideChar_alloc(CP_ACP, 0, str, -1, &w_buffer, &len);
    vim_free(str);
    যদি (w_buffer == রিক্ত)
	ঘূৰাই E_OUTOFMEMORY;

    যদি (len == 0)
    {
	vim_free(w_buffer);
	ঘূৰাই E_FAIL;
    }

    /* Store the result */
    *result = SysAllocString(w_buffer);
    vim_free(w_buffer);

    ঘূৰাই S_OK;
#অন্যথা
    ঘূৰাই E_NOTIMPL;
#endif
}

/*****************************************************************************
 3. The class factory
*****************************************************************************/

/* Definition
 * ----------
 */

শ্ৰেণী CVimCF FINAL : সার্ৱজনিক IClassFactory
{
সার্ৱজনিক:
    স্থির CVimCF *Create();
    আভাস ~CVimCF() {};

    STDMETHOD(QueryInterface)(REFIID riid, শূন্য ** ppv);
    STDMETHOD_(unsigned long, AddRef)(শূন্য);
    STDMETHOD_(unsigned long, Release)(শূন্য);
    STDMETHOD(CreateInstance)(IUnknown *punkOuter, REFIID riid, শূন্য ** ppv);
    STDMETHOD(LockServer)(BOOL lock);

গুপ্ত:
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
    CVimCF *me = নূতন CVimCF();

    যদি (me == রিক্ত)
	MessageBox(0, "Cannot create class factory", "Vim Initialisation", 0);

    ঘূৰাই me;
}

STDMETHODIMP
CVimCF::QueryInterface(REFIID riid, শূন্য **ppv)
{
    যদি (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	AddRef();
	*ppv = এতত্;
	ঘূৰাই S_OK;
    }

    *ppv = 0;
    ঘূৰাই E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVimCF::AddRef()
{
    ঘূৰাই ++ref;
}

STDMETHODIMP_(ULONG)
CVimCF::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    যদি (ref > 0)
	--ref;
    ঘূৰাই ref;
}

/*ARGSUSED*/
STDMETHODIMP
CVimCF::CreateInstance(IUnknown *punkOuter, REFIID riid, শূন্য **ppv)
{
    ঘূৰাই app->QueryInterface(riid, ppv);
}

/*ARGSUSED*/
STDMETHODIMP
CVimCF::LockServer(BOOL lock)
{
    ঘূৰাই S_OK;
}

/*****************************************************************************
 4. Registry manipulation code
*****************************************************************************/

// Internal use only
স্থির শূন্য Set_চাবি_And_মান(ধ্রুৱ ৱর্ণ *path, ধ্রুৱ ৱর্ণ *subkey, ধ্রুৱ ৱর্ণ *মান);
স্থির শূন্য GUIDtochar(ধ্রুৱ GUID &guid, ৱর্ণ *GUID, পূর্ণাঙ্ক length);
স্থির শূন্য Recursive_Delete_চাবি(HKEY h_চাবি_Parent, ধ্রুৱ ৱর্ণ *child);
স্থির ধ্রুৱ পূর্ণাঙ্ক GUID_STRING_SIZE = 39;

// Register the component in the registry
// When "silent" is TRUE don't give any messages.

বাহ্য "C" শূন্য RegisterMe(পূর্ণাঙ্ক silent)
{
    BOOL ok = TRUE;

    // Get the application startup command
    ৱর্ণ module[MAX_PATH];

    ::GetModuleFileName(রিক্ত, module, MAX_PATH);

    // Unregister first (quietly)
    UnregisterMe(FALSE);

    // Convert the CLSID into a char
    ৱর্ণ clsid[GUID_STRING_SIZE];
    GUIDtochar(MYCLSID, clsid, পরিমাণ(clsid));

    // Convert the LIBID into a char
    ৱর্ণ libid[GUID_STRING_SIZE];
    GUIDtochar(MYLIBID, libid, পরিমাণ(libid));

    // Build the key CLSID\\{...}
    ৱর্ণ চাবি_2[MAX_CLSID_LEN];
    strcpy(চাবি_2, "CLSID\\");
    strcat(চাবি_2, clsid);

    // Add the CLSID to the registry
    Set_চাবি_And_মান(চাবি_2, রিক্ত, MYNAME);
    Set_চাবি_And_মান(চাবি_2, "LocalServer32", module);
    Set_চাবি_And_মান(চাবি_2, "ProgID", MYPROGID);
    Set_চাবি_And_মান(চাবি_2, "VersionIndependentProgID", MYVIPROGID);
    Set_চাবি_And_মান(চাবি_2, "TypeLib", libid);

    // Add the version-independent ProgID subkey under HKEY_CLASSES_ROOT
    Set_চাবি_And_মান(MYVIPROGID, রিক্ত, MYNAME);
    Set_চাবি_And_মান(MYVIPROGID, "CLSID", clsid);
    Set_চাবি_And_মান(MYVIPROGID, "CurVer", MYPROGID);

    // Add the versioned ProgID subkey under HKEY_CLASSES_ROOT
    Set_চাবি_And_মান(MYPROGID, রিক্ত, MYNAME);
    Set_চাবি_And_মান(MYPROGID, "CLSID", clsid);

    wchar_t w_module[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, module, -1, w_module, MAX_PATH);

    ITypeLib *typelib = রিক্ত;
    যদি (LoadTypeLib(w_module, &typelib) != S_OK)
    {
	যদি (!silent)
	    MessageBox(0, "Cannot load type library to register",
						       "Vim Registration", 0);
	ok = FALSE;
    }
    অন্যথা
    {
	যদি (RegisterTypeLib(typelib, w_module, রিক্ত) != S_OK)
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
বাহ্য "C" শূন্য UnregisterMe(পূর্ণাঙ্ক bNotifyUser)
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
    ৱর্ণ clsid[GUID_STRING_SIZE];
    GUIDtochar(MYCLSID, clsid, পরিমাণ(clsid));

    // Build the key CLSID\\{...}
    ৱর্ণ চাবি_2[MAX_CLSID_LEN];
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
স্থির শূন্য GUIDtochar(ধ্রুৱ GUID &guid, ৱর্ণ *GUID, পূর্ণাঙ্ক length)
{
    // Get wide string version
    LPOLESTR wGUID = রিক্ত;
    StringFromCLSID(guid, &wGUID);

    // Convert from wide characters to non-wide
    wcstombs(GUID, wGUID, length);

    // Free memory
    CoTaskMemFree(wGUID);
}

// Delete a key and all of its descendants
স্থির শূন্য Recursive_Delete_চাবি(HKEY h_চাবি_Parent, ধ্রুৱ ৱর্ণ *child)
{
    // Open the child
    HKEY h_চাবি_Child;
    LONG result = Reg_Open_চাবি_Ex(h_চাবি_Parent, child, 0,
			       চাবি_ALL_ACCESS, &h_চাবি_Child);
    যদি (result != ERROR_SUCCESS)
	ঘূৰাই;

    // Enumerate all of the descendants of this child
    FILETIME time;
    ৱর্ণ buffer[1024];
    DWORD size = 1024;

    যেতিয়ালৈ (Reg_Enum_চাবি_Ex(h_চাবি_Child, 0, buffer, &size, রিক্ত,
			রিক্ত, রিক্ত, &time) == S_OK)
    {
	// Delete the descendants of this child
	Recursive_Delete_চাবি(h_চাবি_Child, buffer);
	size = 256;
    }

    // Close the child
    Reg_Close_চাবি(h_চাবি_Child);

    // Delete this child
    Reg_Delete_চাবি(h_চাবি_Parent, child);
}

// Create a key and set its value
স্থির শূন্য Set_চাবি_And_মান(ধ্রুৱ ৱর্ণ *চাবি, ধ্রুৱ ৱর্ণ *subkey, ধ্রুৱ ৱর্ণ *মান)
{
    HKEY h_চাবি;
    ৱর্ণ buffer[1024];

    strcpy(buffer, চাবি);

    // Add subkey name to buffer.
    যদি (subkey)
    {
	strcat(buffer, "\\");
	strcat(buffer, subkey);
    }

    // Create and open key and subkey.
    long result = Reg_Create_চাবি_Ex(HKEY_CLASSES_মূল,
				 buffer,
				 0, রিক্ত, REG_OPTION_NON_VOLATILE,
				 চাবি_ALL_ACCESS, রিক্ত,
				 &h_চাবি, রিক্ত);
    যদি (result != ERROR_SUCCESS)
	ঘূৰাই;

    // Set the value
    যদি (মান)
	Reg_Set_মান_Ex(h_চাবি, রিক্ত, 0, REG_SZ, (BYTE *)মান,
		      (DWORD)STRLEN(মান)+1);

    Reg_Close_চাবি(h_চাবি);
}

/*****************************************************************************
 5. OLE Initialisation and shutdown processing
*****************************************************************************/
বাহ্য "C" শূন্য InitOLE(পূর্ণাঙ্ক *pbDoRestart)
{
    HRESULT hr;

    *pbDoRestart = FALSE;

    // Initialize the OLE libraries
    hr = OleInitialize(রিক্ত);
    যদি (FAILED(hr))
    {
	MessageBox(0, "Cannot initialise OLE", "Vim Initialisation", 0);
	গচ্ছ error0;
    }

    // Create the application object
    app = CVim::Create(pbDoRestart);
    যদি (app == রিক্ত)
	গচ্ছ error1;

    // Create the class factory
    cf = CVimCF::Create();
    যদি (cf == রিক্ত)
	গচ্ছ error1;

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
	গচ্ছ error1;
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
	গচ্ছ error1;
    }

    ঘূৰাই;

    // Errors: tidy up as much as needed and return
error1:
    UninitOLE();
error0:
    ঘূৰাই;
}

বাহ্য "C" শূন্য UninitOLE()
{
    // Unregister the application object
    যদি (app_id)
    {
	RevokeActiveObject(app_id, রিক্ত);
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
	app = রিক্ত;
    }

    // Delete the class factory
    যদি (cf)
    {
	delete cf;
	cf = রিক্ত;
    }
}
#endif /* FEAT_OLE */
