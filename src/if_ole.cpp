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
خارجي HWND vim_پلار_hwnd;
}

#إذا (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#وإلا
# define FINAL
#د_که_پای

#ضمن "if_ole.h"	// Interface definitions
#ضمن "iid_ole.c"	// UUID definitions (compile here)

/* Supply function prototype to work around bug in Mingw oleauto.h header */
#که_تعریف_شوی __MINGW32__
WINOLEAUTAPI Un_ثبتوونکی_Type_Lib(REFGUID libID, کلمه_2 wVerMajor,
	    کلمه_2 wVerMinor, LCID lcid, SYSKIND syskind);
#د_که_پای

/*****************************************************************************
 1. Internal definitions for this file
*****************************************************************************/

صنف CVim;
صنف CVimCF;

/* Internal data */
// The identifier of the registered class factory
ساكن بینښه اوږد cf_id = 0;

// The identifier of the running application object
ساكن بینښه اوږد app_id = 0;

// The single global instance of the class factory
ساكن CVimCF *cf = 0;

// The single global instance of the application object
ساكن CVim *app = 0;

/* GUIDs, versions and type library information */
#تعریف_کړه MYCLSID CLSID_Vim
#تعریف_کړه MYLIBID LIBID_Vim
#تعریف_کړه MYIID IID_IVim

#تعریف_کړه MAJORVER 1
#تعریف_کړه MINORVER 0
#تعریف_کړه LOCALE 0x0409

#تعریف_کړه MYNAME "Vim"
#تعریف_کړه MYPROGID "Vim.Application.1"
#تعریف_کړه MYVIPROGID "Vim.Application"

#تعریف_کړه MAX_CLSID_LEN 100

/*****************************************************************************
 2. The application object
*****************************************************************************/

/* Definition
 * ----------
 */

صنف CVim FINAL : عام IVim
{
عام:
    مجازي ~CVim();
    ساكن CVim *Create(صحيح *pbDoRestart);

    // IUnknown members
    STDMETHOD(QueryInterface)(REFIID riid, فراغ ** ppv);
    STDMETHOD_(بینښه اوږد, جمع_کړه_Ref)(فراغ);
    STDMETHOD_(بینښه اوږد, Release)(فراغ);

    // IDispatch members
    STDMETHOD(Get_Type_Info_شمېر)(UINT *p_شمېر);
    STDMETHOD(GetTypeInfo)(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo);
    STDMETHOD(Get_IDs_د_نومونه)(ثابت IID &iid, OLECHAR **نومونه, UINT n, LCID, DISPID *dispids);
    STDMETHOD(Invoke)(DISPID member, ثابت IID &iid, LCID, کلمه_2 flags, DISPPARAMS *dispparams, VARIANT *پایله, EXCEPINFO *excepinfo, UINT *argerr);

    // IVim members
    STDMETHOD(Send_کلیګانې)(BSTR کلیګانې);
    STDMETHOD(Eval)(BSTR بیان, BSTR *پایله);
    STDMETHOD(سېټ_Foreground)(فراغ);
    STDMETHOD(GetHwnd)(UINT_PTR *پایله);

خاص:
    // Constructor is private - create using CVim::Create()
    CVim() : ref(0), typeinfo(0) {};

    // Reference count
    بینښه اوږد ref;

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
	پیغام_Box(0, "Cannot create application object", "Vim Initialisation", 0);
	أرجع عدم;
    }

    // Load the type library from the registry
    hr = بارول_Reg_Type_Lib(MYLIBID, 1, 0, 0x00, &typelib);
    إذا (FAILED(hr))
    {
	HKEY h_کلی;

	// Check we can write to the registry.
	// RegCreateKeyEx succeeds even if key exists. W.Briscoe W2K 20021011
	إذا (Reg_Create_کلی_Ex(HKEY_CLASSES_ریښه, MYVIPROGID, 0, عدم,
		  REG_OPTION_NON_VOLATILE,
		  کلی_ALL_ACCESS, عدم, &h_کلی, عدم))
	{
	    ړنګول me;
	    أرجع عدم; // Unable to write to registry. Quietly fail.
	}
	Reg_Close_کلی(h_کلی);

	إذا (پیغام_Box(0, "Cannot load registered type library.\nDo you want to register Vim now?",
		    "Vim Initialisation", MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
	    ړنګول me;
	    أرجع عدم;
	}

	ثبتوونکی_Me(FALSE);

	// Load the type library from the registry
	hr = بارول_Reg_Type_Lib(MYLIBID, 1, 0, 0x00, &typelib);
	إذا (FAILED(hr))
	{
	    پیغام_Box(0, "You must restart Vim in order for the registration to take effect.",
						     "Vim Initialisation", 0);
	    *pbDoRestart = TRUE;
	    ړنګول me;
	    أرجع عدم;
	}
    }

    // Get the type info of the vtable interface
    hr = typelib->Get_Type_Info_د_Guid(MYIID, &typeinfo);
    typelib->Release();

    إذا (FAILED(hr))
    {
	پیغام_Box(0, "Cannot get interface type information",
						     "Vim Initialisation", 0);
	ړنګول me;
	أرجع عدم;
    }

    // Save the type information
    me->typeinfo = typeinfo;
    أرجع me;
}

CVim::~CVim()
{
    إذا (typeinfo && vim_پلار_hwnd == عدم)
	typeinfo->Release();
    typeinfo = 0;
}

STDMETHODIMP
CVim::QueryInterface(REFIID riid, فراغ **ppv)
{
    إذا (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDispatch) || IsEqualIID(riid, MYIID))
    {
	جمع_کړه_Ref();
	*ppv = هذا;
	أرجع S_OK;
    }

    *ppv = 0;
    أرجع E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVim::جمع_کړه_Ref()
{
    أرجع ++ref;
}

STDMETHODIMP_(ULONG)
CVim::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    إذا (ref > 0)
	--ref;
    أرجع ref;
}

STDMETHODIMP
CVim::Get_Type_Info_شمېر(UINT *p_شمېر)
{
    *p_شمېر = 1;
    أرجع S_OK;
}

STDMETHODIMP
CVim::GetTypeInfo(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo)
{
    *ppITypeInfo = 0;

    إذا (iTypeInfo != 0)
	أرجع DISP_E_BADINDEX;

    typeinfo->جمع_کړه_Ref();
    *ppITypeInfo = typeinfo;
    أرجع S_OK;
}

STDMETHODIMP
CVim::Get_IDs_د_نومونه(
	ثابت IID &iid,
	OLECHAR **نومونه,
	UINT n,
	LCID,
	DISPID *dispids)
{
    إذا (iid != IID_NULL)
	أرجع DISP_E_UNKNOWNINTERFACE;

    أرجع typeinfo->Get_IDs_د_نومونه(نومونه, n, dispids);
}

STDMETHODIMP
CVim::Invoke(
	DISPID member,
	ثابت IID &iid,
	LCID,
	کلمه_2 flags,
	DISPPARAMS *dispparams,
	VARIANT *پایله,
	EXCEPINFO *excepinfo,
	UINT *argerr)
{
    إذا (iid != IID_NULL)
	أرجع DISP_E_UNKNOWNINTERFACE;

    ::سېټ_Error_Info(0, عدم);
    أرجع typeinfo->Invoke(static_cast<IDispatch*>(هذا),
			    member, flags, dispparams,
			    پایله, excepinfo, argerr);
}

STDMETHODIMP
CVim::GetHwnd(UINT_PTR *پایله)
{
    *پایله = (UINT_PTR)s_hwnd;
    أرجع S_OK;
}

STDMETHODIMP
CVim::سېټ_Foreground(فراغ)
{
    /* Make the Vim window come to the foreground */
    gui_mch_سېټ_foreground();
    أرجع S_OK;
}

STDMETHODIMP
CVim::Send_کلیګانې(BSTR کلیګانې)
{
    صحيح len;
    حرف *بفر;
    char_u *str;
    char_u *ptr;

    /* Get a suitable buffer */
    len = Wide_Char_منزل_Multi_Byte(CP_ACP, 0, کلیګانې, -1, 0, 0, 0, 0);
    بفر = (حرف *)alloc(len+1);

    إذا (بفر == عدم)
	أرجع E_OUTOFMEMORY;

    len = Wide_Char_منزل_Multi_Byte(CP_ACP, 0, کلیګانې, -1, بفر, len, 0, 0);

    إذا (len == 0)
    {
	vim_free(بفر);
	أرجع E_INVALIDARG;
    }

    /* Translate key codes like <Esc> */
    str = replace_termcodes((char_u *)بفر, &ptr, 0, REPTERM_DO_LT, عدم);

    /* If ptr was set, then a new buffer was allocated,
     * so we can free the old one.
     */
    إذا (ptr)
	vim_free((char_u *)(بفر));

    /* Reject strings too long to fit in the input buffer. Allow 10 bytes
     * space to cover for the (remote) possibility that characters may enter
     * the input buffer between now and when the WM_OLE message is actually
     * processed. If more than 10 characters enter the input buffer in that
     * time, the WM_OLE processing will simply fail to insert the characters.
     */
    إذا ((صحيح)(STRLEN(str)) > (vim_free_دننه_ننوت_buf() - 10))
    {
	vim_free(str);
	أرجع E_INVALIDARG;
    }

    /* Pass the string to the main input loop. The memory will be freed when
     * the message is processed.  Except for an empty message, we don't need
     * to post it then.
     */
    إذا (*str == NUL)
	vim_free(str);
    وإلا
	Post_پیغام(عدم, WM_OLE, 0, (LPARAM)str);

    أرجع S_OK;
}

STDMETHODIMP
CVim::Eval(BSTR بیان, BSTR *پایله)
{
#که_تعریف_شوی FEAT_EVAL
    صحيح len;
    حرف *بفر;
    حرف *str;
    wchar_t *w_بفر;

    /* Get a suitable buffer */
    len = Wide_Char_منزل_Multi_Byte(CP_ACP, 0, بیان, -1, 0, 0, 0, 0);
    إذا (len == 0)
	أرجع E_INVALIDARG;

    بفر = (حرف *)alloc(len);

    إذا (بفر == عدم)
	أرجع E_OUTOFMEMORY;

    /* Convert the (wide character) expression to an ASCII string */
    len = Wide_Char_منزل_Multi_Byte(CP_ACP, 0, بیان, -1, بفر, len, 0, 0);
    إذا (len == 0)
	أرجع E_INVALIDARG;

    /* Evaluate the expression */
    ++emsg_skip;
    str = (حرف *)eval_منزل_string((char_u *)بفر, TRUE, FALSE);
    --emsg_skip;
    vim_free(بفر);
    إذا (str == عدم)
	أرجع E_FAIL;

    /* Convert the result to wide characters */
    Multi_Byte_منزل_Wide_Char_alloc(CP_ACP, 0, str, -1, &w_بفر, &len);
    vim_free(str);
    إذا (w_بفر == عدم)
	أرجع E_OUTOFMEMORY;

    إذا (len == 0)
    {
	vim_free(w_بفر);
	أرجع E_FAIL;
    }

    /* Store the result */
    *پایله = SysAllocString(w_بفر);
    vim_free(w_بفر);

    أرجع S_OK;
#وإلا
    أرجع E_NOTIMPL;
#د_که_پای
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
    مجازي ~CVimCF() {};

    STDMETHOD(QueryInterface)(REFIID riid, فراغ ** ppv);
    STDMETHOD_(بینښه اوږد, جمع_کړه_Ref)(فراغ);
    STDMETHOD_(بینښه اوږد, Release)(فراغ);
    STDMETHOD(CreateInstance)(IUnknown *punkOuter, REFIID riid, فراغ ** ppv);
    STDMETHOD(قلف_Server)(BOOL قلف);

خاص:
    // Constructor is private - create via Create()
    CVimCF() : ref(0) {};

    // Reference count
    بینښه اوږد ref;
};

/* Implementation
 * --------------
 */

CVimCF *CVimCF::Create()
{
    CVimCF *me = جديد CVimCF();

    إذا (me == عدم)
	پیغام_Box(0, "Cannot create class factory", "Vim Initialisation", 0);

    أرجع me;
}

STDMETHODIMP
CVimCF::QueryInterface(REFIID riid, فراغ **ppv)
{
    إذا (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	جمع_کړه_Ref();
	*ppv = هذا;
	أرجع S_OK;
    }

    *ppv = 0;
    أرجع E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVimCF::جمع_کړه_Ref()
{
    أرجع ++ref;
}

STDMETHODIMP_(ULONG)
CVimCF::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    إذا (ref > 0)
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
CVimCF::قلف_Server(BOOL قلف)
{
    أرجع S_OK;
}

/*****************************************************************************
 4. Registry manipulation code
*****************************************************************************/

// Internal use only
ساكن فراغ سېټ_کلی_And_ارزښت(ثابت حرف *لار, ثابت حرف *subkey, ثابت حرف *ارزښت);
ساكن فراغ GUIDtochar(ثابت GUID &guid, حرف *GUID, صحيح اوږدتیا);
ساكن فراغ Recursive_Delete_کلی(HKEY h_کلی_پلار, ثابت حرف *ماشوم);
ساكن ثابت صحيح GUID_STRING_اندازه = 39;

// Register the component in the registry
// When "silent" is TRUE don't give any messages.

خارجي "C" فراغ ثبتوونکی_Me(صحيح silent)
{
    BOOL ok = TRUE;

    // Get the application startup command
    حرف module[MAX_لار];

    ::GetModuleFileName(عدم, module, MAX_لار);

    // Unregister first (quietly)
    UnregisterMe(FALSE);

    // Convert the CLSID into a char
    حرف clsid[GUID_STRING_اندازه];
    GUIDtochar(MYCLSID, clsid, حجم(clsid));

    // Convert the LIBID into a char
    حرف libid[GUID_STRING_اندازه];
    GUIDtochar(MYLIBID, libid, حجم(libid));

    // Build the key CLSID\\{...}
    حرف کلی_2[MAX_CLSID_LEN];
    strcpy(کلی_2, "CLSID\\");
    strcat(کلی_2, clsid);

    // Add the CLSID to the registry
    سېټ_کلی_And_ارزښت(کلی_2, عدم, MYNAME);
    سېټ_کلی_And_ارزښت(کلی_2, "LocalServer32", module);
    سېټ_کلی_And_ارزښت(کلی_2, "ProgID", MYPROGID);
    سېټ_کلی_And_ارزښت(کلی_2, "VersionIndependentProgID", MYVIPROGID);
    سېټ_کلی_And_ارزښت(کلی_2, "TypeLib", libid);

    // Add the version-independent ProgID subkey under HKEY_CLASSES_ROOT
    سېټ_کلی_And_ارزښت(MYVIPROGID, عدم, MYNAME);
    سېټ_کلی_And_ارزښت(MYVIPROGID, "CLSID", clsid);
    سېټ_کلی_And_ارزښت(MYVIPROGID, "CurVer", MYPROGID);

    // Add the versioned ProgID subkey under HKEY_CLASSES_ROOT
    سېټ_کلی_And_ارزښت(MYPROGID, عدم, MYNAME);
    سېټ_کلی_And_ارزښت(MYPROGID, "CLSID", clsid);

    wchar_t w_module[MAX_لار];
    Multi_Byte_منزل_Wide_Char(CP_ACP, 0, module, -1, w_module, MAX_لار);

    ITypeLib *typelib = عدم;
    إذا (بارول_Type_Lib(w_module, &typelib) != S_OK)
    {
	إذا (!silent)
	    پیغام_Box(0, "Cannot load type library to register",
						       "Vim Registration", 0);
	ok = FALSE;
    }
    وإلا
    {
	إذا (ثبتوونکی_Type_Lib(typelib, w_module, عدم) != S_OK)
	{
	    إذا (!silent)
		پیغام_Box(0, "Cannot register type library",
						       "Vim Registration", 0);
	    ok = FALSE;
	}
	typelib->Release();
    }

    إذا (ok && !silent)
	پیغام_Box(0, "Registered successfully", "Vim", 0);
}

// Remove the component from the registry
//
// Note: There is little error checking in this code, to allow incomplete
// or failed registrations to be undone.
خارجي "C" فراغ UnregisterMe(صحيح bNotifyUser)
{
    // Unregister the type library
    ITypeLib *typelib;
    إذا (SUCCEEDED(بارول_Reg_Type_Lib(MYLIBID, MAJORVER, MINORVER, LOCALE, &typelib)))
    {
	TLIBATTR *tla;
	إذا (SUCCEEDED(typelib->GetLibAttr(&tla)))
	{
	    Un_ثبتوونکی_Type_Lib(tla->guid, tla->w_Major_Ver_عدد, tla->w_Minor_Ver_عدد,
			      tla->lcid, tla->syskind);
	    typelib->ReleaseTLibAttr(tla);
	}
	typelib->Release();
    }

    // Convert the CLSID into a char
    حرف clsid[GUID_STRING_اندازه];
    GUIDtochar(MYCLSID, clsid, حجم(clsid));

    // Build the key CLSID\\{...}
    حرف کلی_2[MAX_CLSID_LEN];
    strcpy(کلی_2, "CLSID\\");
    strcat(کلی_2, clsid);

    // Delete the CLSID Key - CLSID\{...}
    Recursive_Delete_کلی(HKEY_CLASSES_ریښه, کلی_2);

    // Delete the version-independent ProgID Key
    Recursive_Delete_کلی(HKEY_CLASSES_ریښه, MYVIPROGID);

    // Delete the ProgID key
    Recursive_Delete_کلی(HKEY_CLASSES_ریښه, MYPROGID);

    إذا (bNotifyUser)
	پیغام_Box(0, "Unregistered successfully", "Vim", 0);
}

/****************************************************************************/

// Convert a GUID to a char string
ساكن فراغ GUIDtochar(ثابت GUID &guid, حرف *GUID, صحيح اوږدتیا)
{
    // Get wide string version
    LPOLESTR wGUID = عدم;
    StringFromCLSID(guid, &wGUID);

    // Convert from wide characters to non-wide
    wcstombs(GUID, wGUID, اوږدتیا);

    // Free memory
    CoTaskMemFree(wGUID);
}

// Delete a key and all of its descendants
ساكن فراغ Recursive_Delete_کلی(HKEY h_کلی_پلار, ثابت حرف *ماشوم)
{
    // Open the child
    HKEY h_کلی_ماشوم;
    LONG پایله = Reg_Open_کلی_Ex(h_کلی_پلار, ماشوم, 0,
			       کلی_ALL_ACCESS, &h_کلی_ماشوم);
    إذا (پایله != ERROR_SUCCESS)
	أرجع;

    // Enumerate all of the descendants of this child
    FILETIME وخت;
    حرف بفر[1024];
    DWORD اندازه = 1024;

    طالما (Reg_Enum_کلی_Ex(h_کلی_ماشوم, 0, بفر, &اندازه, عدم,
			عدم, عدم, &وخت) == S_OK)
    {
	// Delete the descendants of this child
	Recursive_Delete_کلی(h_کلی_ماشوم, بفر);
	اندازه = 256;
    }

    // Close the child
    Reg_Close_کلی(h_کلی_ماشوم);

    // Delete this child
    Reg_Delete_کلی(h_کلی_پلار, ماشوم);
}

// Create a key and set its value
ساكن فراغ سېټ_کلی_And_ارزښت(ثابت حرف *کلی, ثابت حرف *subkey, ثابت حرف *ارزښت)
{
    HKEY h_کلی;
    حرف بفر[1024];

    strcpy(بفر, کلی);

    // Add subkey name to buffer.
    إذا (subkey)
    {
	strcat(بفر, "\\");
	strcat(بفر, subkey);
    }

    // Create and open key and subkey.
    اوږد پایله = Reg_Create_کلی_Ex(HKEY_CLASSES_ریښه,
				 بفر,
				 0, عدم, REG_OPTION_NON_VOLATILE,
				 کلی_ALL_ACCESS, عدم,
				 &h_کلی, عدم);
    إذا (پایله != ERROR_SUCCESS)
	أرجع;

    // Set the value
    إذا (ارزښت)
	Reg_سېټ_ارزښت_Ex(h_کلی, عدم, 0, REG_SZ, (BYTE *)ارزښت,
		      (DWORD)STRLEN(ارزښت)+1);

    Reg_Close_کلی(h_کلی);
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
	پیغام_Box(0, "Cannot initialise OLE", "Vim Initialisation", 0);
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
    hr = Co_ثبتوونکی_Class_Object(
	MYCLSID,
	cf,
	CLSCTX_LOCAL_SERVER,
	REGCLS_MULTIPLEUSE,
	&cf_id);

    إذا (FAILED(hr))
    {
	پیغام_Box(0, "Cannot register class factory", "Vim Initialisation", 0);
	اذهب error1;
    }

    // Register the application object as active
    hr = ثبتوونکی_فعال_Object(
	app,
	MYCLSID,
	0,
	&app_id);

    إذا (FAILED(hr))
    {
	پیغام_Box(0, "Cannot register application object", "Vim Initialisation", 0);
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
    إذا (app_id)
    {
	Revoke_فعال_Object(app_id, عدم);
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
	ړنګول app;
	app = عدم;
    }

    // Delete the class factory
    إذا (cf)
    {
	ړنګول cf;
	cf = عدم;
    }
}
#د_که_پای /* FEAT_OLE */
