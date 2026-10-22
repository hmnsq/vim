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
خارجي HWND vim_ماپا_hwnd;
}

#إذا (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#وإلا
# define FINAL
#جے_انت

#ضمن "if_ole.h"	// Interface definitions
#ضمن "iid_ole.c"	// UUID definitions (compile here)

/* Supply function prototype to work around bug in Mingw oleauto.h header */
#جے_تعریف_شدہ __MINGW32__
WINOLEAUTAPI Un_رجسٹر_Type_Lib(REFGUID libID, لفظ_2 wVerMajor,
	    لفظ_2 wVerMinor, LCID lcid, SYSKIND syskind);
#جے_انت

/*****************************************************************************
 1. Internal definitions for this file
*****************************************************************************/

صنف CVim;
صنف CVimCF;

/* Internal data */
// The identifier of the registered class factory
ساكن بنانشانی لما cf_id = 0;

// The identifier of the running application object
ساكن بنانشانی لما app_id = 0;

// The single global instance of the class factory
ساكن CVimCF *cf = 0;

// The single global instance of the application object
ساكن CVim *app = 0;

/* GUIDs, versions and type library information */
#تعریف_کرو MYCLSID CLSID_Vim
#تعریف_کرو MYLIBID LIBID_Vim
#تعریف_کرو MYIID IID_IVim

#تعریف_کرو MAJORVER 1
#تعریف_کرو MINORVER 0
#تعریف_کرو LOCALE 0x0409

#تعریف_کرو MYNAME "Vim"
#تعریف_کرو MYPROGID "Vim.Application.1"
#تعریف_کرو MYVIPROGID "Vim.Application"

#تعریف_کرو MAX_CLSID_LEN 100

/*****************************************************************************
 2. The application object
*****************************************************************************/

/* Definition
 * ----------
 */

صنف CVim FINAL : عام IVim
{
عام:
    مجازی ~CVim();
    ساكن CVim *Create(صحيح *pbDoRestart);

    // IUnknown members
    STDMETHOD(QueryInterface)(REFIID riid, فراغ ** ppv);
    STDMETHOD_(بنانشانی لما, جوڑو_Ref)(فراغ);
    STDMETHOD_(بنانشانی لما, Release)(فراغ);

    // IDispatch members
    STDMETHOD(Get_Type_Info_گنتی)(UINT *p_گنتی);
    STDMETHOD(GetTypeInfo)(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo);
    STDMETHOD(Get_IDs_دا_ناں)(ثابت IID &iid, OLECHAR **ناں, UINT n, LCID, DISPID *dispids);
    STDMETHOD(Invoke)(DISPID member, ثابت IID &iid, LCID, لفظ_2 flags, DISPPARAMS *dispparams, VARIANT *نتیجہ, EXCEPINFO *excepinfo, UINT *argerr);

    // IVim members
    STDMETHOD(Send_کلیدیں)(BSTR کلیدیں);
    STDMETHOD(Eval)(BSTR اظہاریہ, BSTR *نتیجہ);
    STDMETHOD(سیٹ_Foreground)(فراغ);
    STDMETHOD(GetHwnd)(UINT_PTR *نتیجہ);

خاص:
    // Constructor is private - create using CVim::Create()
    CVim() : ref(0), typeinfo(0) {};

    // Reference count
    بنانشانی لما ref;

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
	سنیہا_Box(0, "Cannot create application object", "Vim Initialisation", 0);
	أرجع عدم;
    }

    // Load the type library from the registry
    hr = لدو_Reg_Type_Lib(MYLIBID, 1, 0, 0x00, &typelib);
    إذا (FAILED(hr))
    {
	HKEY h_کلید;

	// Check we can write to the registry.
	// RegCreateKeyEx succeeds even if key exists. W.Briscoe W2K 20021011
	إذا (Reg_Create_کلید_Ex(HKEY_CLASSES_جڑ, MYVIPROGID, 0, عدم,
		  REG_OPTION_NON_VOLATILE,
		  کلید_ALL_ACCESS, عدم, &h_کلید, عدم))
	{
	    مٹاؤ me;
	    أرجع عدم; // Unable to write to registry. Quietly fail.
	}
	Reg_Close_کلید(h_کلید);

	إذا (سنیہا_Box(0, "Cannot load registered type library.\nDo you want to register Vim now?",
		    "Vim Initialisation", MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
	    مٹاؤ me;
	    أرجع عدم;
	}

	رجسٹر_Me(FALSE);

	// Load the type library from the registry
	hr = لدو_Reg_Type_Lib(MYLIBID, 1, 0, 0x00, &typelib);
	إذا (FAILED(hr))
	{
	    سنیہا_Box(0, "You must restart Vim in order for the registration to take effect.",
						     "Vim Initialisation", 0);
	    *pbDoRestart = TRUE;
	    مٹاؤ me;
	    أرجع عدم;
	}
    }

    // Get the type info of the vtable interface
    hr = typelib->Get_Type_Info_دا_Guid(MYIID, &typeinfo);
    typelib->Release();

    إذا (FAILED(hr))
    {
	سنیہا_Box(0, "Cannot get interface type information",
						     "Vim Initialisation", 0);
	مٹاؤ me;
	أرجع عدم;
    }

    // Save the type information
    me->typeinfo = typeinfo;
    أرجع me;
}

CVim::~CVim()
{
    إذا (typeinfo && vim_ماپا_hwnd == عدم)
	typeinfo->Release();
    typeinfo = 0;
}

STDMETHODIMP
CVim::QueryInterface(REFIID riid, فراغ **ppv)
{
    إذا (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDispatch) || IsEqualIID(riid, MYIID))
    {
	جوڑو_Ref();
	*ppv = هذا;
	أرجع S_OK;
    }

    *ppv = 0;
    أرجع E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVim::جوڑو_Ref()
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
CVim::Get_Type_Info_گنتی(UINT *p_گنتی)
{
    *p_گنتی = 1;
    أرجع S_OK;
}

STDMETHODIMP
CVim::GetTypeInfo(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo)
{
    *ppITypeInfo = 0;

    إذا (iTypeInfo != 0)
	أرجع DISP_E_BADINDEX;

    typeinfo->جوڑو_Ref();
    *ppITypeInfo = typeinfo;
    أرجع S_OK;
}

STDMETHODIMP
CVim::Get_IDs_دا_ناں(
	ثابت IID &iid,
	OLECHAR **ناں,
	UINT n,
	LCID,
	DISPID *dispids)
{
    إذا (iid != IID_NULL)
	أرجع DISP_E_UNKNOWNINTERFACE;

    أرجع typeinfo->Get_IDs_دا_ناں(ناں, n, dispids);
}

STDMETHODIMP
CVim::Invoke(
	DISPID member,
	ثابت IID &iid,
	LCID,
	لفظ_2 flags,
	DISPPARAMS *dispparams,
	VARIANT *نتیجہ,
	EXCEPINFO *excepinfo,
	UINT *argerr)
{
    إذا (iid != IID_NULL)
	أرجع DISP_E_UNKNOWNINTERFACE;

    ::سیٹ_Error_Info(0, عدم);
    أرجع typeinfo->Invoke(static_cast<IDispatch*>(هذا),
			    member, flags, dispparams,
			    نتیجہ, excepinfo, argerr);
}

STDMETHODIMP
CVim::GetHwnd(UINT_PTR *نتیجہ)
{
    *نتیجہ = (UINT_PTR)s_hwnd;
    أرجع S_OK;
}

STDMETHODIMP
CVim::سیٹ_Foreground(فراغ)
{
    /* Make the Vim window come to the foreground */
    gui_mch_سیٹ_foreground();
    أرجع S_OK;
}

STDMETHODIMP
CVim::Send_کلیدیں(BSTR کلیدیں)
{
    صحيح len;
    حرف *بفر;
    char_u *str;
    char_u *ptr;

    /* Get a suitable buffer */
    len = Wide_Char_منزل_Multi_Byte(CP_ACP, 0, کلیدیں, -1, 0, 0, 0, 0);
    بفر = (حرف *)alloc(len+1);

    إذا (بفر == عدم)
	أرجع E_OUTOFMEMORY;

    len = Wide_Char_منزل_Multi_Byte(CP_ACP, 0, کلیدیں, -1, بفر, len, 0, 0);

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
    إذا ((صحيح)(STRLEN(str)) > (vim_free_وچ_داخلہ_buf() - 10))
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
	Post_سنیہا(عدم, WM_OLE, 0, (LPARAM)str);

    أرجع S_OK;
}

STDMETHODIMP
CVim::Eval(BSTR اظہاریہ, BSTR *نتیجہ)
{
#جے_تعریف_شدہ FEAT_EVAL
    صحيح len;
    حرف *بفر;
    حرف *str;
    wchar_t *w_بفر;

    /* Get a suitable buffer */
    len = Wide_Char_منزل_Multi_Byte(CP_ACP, 0, اظہاریہ, -1, 0, 0, 0, 0);
    إذا (len == 0)
	أرجع E_INVALIDARG;

    بفر = (حرف *)alloc(len);

    إذا (بفر == عدم)
	أرجع E_OUTOFMEMORY;

    /* Convert the (wide character) expression to an ASCII string */
    len = Wide_Char_منزل_Multi_Byte(CP_ACP, 0, اظہاریہ, -1, بفر, len, 0, 0);
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
    *نتیجہ = SysAllocString(w_بفر);
    vim_free(w_بفر);

    أرجع S_OK;
#وإلا
    أرجع E_NOTIMPL;
#جے_انت
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
    مجازی ~CVimCF() {};

    STDMETHOD(QueryInterface)(REFIID riid, فراغ ** ppv);
    STDMETHOD_(بنانشانی لما, جوڑو_Ref)(فراغ);
    STDMETHOD_(بنانشانی لما, Release)(فراغ);
    STDMETHOD(CreateInstance)(IUnknown *punkOuter, REFIID riid, فراغ ** ppv);
    STDMETHOD(جندرا_Server)(BOOL جندرا);

خاص:
    // Constructor is private - create via Create()
    CVimCF() : ref(0) {};

    // Reference count
    بنانشانی لما ref;
};

/* Implementation
 * --------------
 */

CVimCF *CVimCF::Create()
{
    CVimCF *me = جديد CVimCF();

    إذا (me == عدم)
	سنیہا_Box(0, "Cannot create class factory", "Vim Initialisation", 0);

    أرجع me;
}

STDMETHODIMP
CVimCF::QueryInterface(REFIID riid, فراغ **ppv)
{
    إذا (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	جوڑو_Ref();
	*ppv = هذا;
	أرجع S_OK;
    }

    *ppv = 0;
    أرجع E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVimCF::جوڑو_Ref()
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
CVimCF::جندرا_Server(BOOL جندرا)
{
    أرجع S_OK;
}

/*****************************************************************************
 4. Registry manipulation code
*****************************************************************************/

// Internal use only
ساكن فراغ سیٹ_کلید_And_قدر(ثابت حرف *رستہ, ثابت حرف *subkey, ثابت حرف *قدر);
ساكن فراغ GUIDtochar(ثابت GUID &guid, حرف *GUID, صحيح لمائی);
ساكن فراغ Recursive_Delete_کلید(HKEY h_کلید_ماپا, ثابت حرف *بچہ);
ساكن ثابت صحيح GUID_STRING_ناپ = 39;

// Register the component in the registry
// When "silent" is TRUE don't give any messages.

خارجي "C" فراغ رجسٹر_Me(صحيح silent)
{
    BOOL ok = TRUE;

    // Get the application startup command
    حرف module[MAX_رستہ];

    ::GetModuleFileName(عدم, module, MAX_رستہ);

    // Unregister first (quietly)
    UnregisterMe(FALSE);

    // Convert the CLSID into a char
    حرف clsid[GUID_STRING_ناپ];
    GUIDtochar(MYCLSID, clsid, حجم(clsid));

    // Convert the LIBID into a char
    حرف libid[GUID_STRING_ناپ];
    GUIDtochar(MYLIBID, libid, حجم(libid));

    // Build the key CLSID\\{...}
    حرف کلید_2[MAX_CLSID_LEN];
    strcpy(کلید_2, "CLSID\\");
    strcat(کلید_2, clsid);

    // Add the CLSID to the registry
    سیٹ_کلید_And_قدر(کلید_2, عدم, MYNAME);
    سیٹ_کلید_And_قدر(کلید_2, "LocalServer32", module);
    سیٹ_کلید_And_قدر(کلید_2, "ProgID", MYPROGID);
    سیٹ_کلید_And_قدر(کلید_2, "VersionIndependentProgID", MYVIPROGID);
    سیٹ_کلید_And_قدر(کلید_2, "TypeLib", libid);

    // Add the version-independent ProgID subkey under HKEY_CLASSES_ROOT
    سیٹ_کلید_And_قدر(MYVIPROGID, عدم, MYNAME);
    سیٹ_کلید_And_قدر(MYVIPROGID, "CLSID", clsid);
    سیٹ_کلید_And_قدر(MYVIPROGID, "CurVer", MYPROGID);

    // Add the versioned ProgID subkey under HKEY_CLASSES_ROOT
    سیٹ_کلید_And_قدر(MYPROGID, عدم, MYNAME);
    سیٹ_کلید_And_قدر(MYPROGID, "CLSID", clsid);

    wchar_t w_module[MAX_رستہ];
    Multi_Byte_منزل_Wide_Char(CP_ACP, 0, module, -1, w_module, MAX_رستہ);

    ITypeLib *typelib = عدم;
    إذا (لدو_Type_Lib(w_module, &typelib) != S_OK)
    {
	إذا (!silent)
	    سنیہا_Box(0, "Cannot load type library to register",
						       "Vim Registration", 0);
	ok = FALSE;
    }
    وإلا
    {
	إذا (رجسٹر_Type_Lib(typelib, w_module, عدم) != S_OK)
	{
	    إذا (!silent)
		سنیہا_Box(0, "Cannot register type library",
						       "Vim Registration", 0);
	    ok = FALSE;
	}
	typelib->Release();
    }

    إذا (ok && !silent)
	سنیہا_Box(0, "Registered successfully", "Vim", 0);
}

// Remove the component from the registry
//
// Note: There is little error checking in this code, to allow incomplete
// or failed registrations to be undone.
خارجي "C" فراغ UnregisterMe(صحيح bNotifyUser)
{
    // Unregister the type library
    ITypeLib *typelib;
    إذا (SUCCEEDED(لدو_Reg_Type_Lib(MYLIBID, MAJORVER, MINORVER, LOCALE, &typelib)))
    {
	TLIBATTR *tla;
	إذا (SUCCEEDED(typelib->GetLibAttr(&tla)))
	{
	    Un_رجسٹر_Type_Lib(tla->guid, tla->w_Major_Ver_عدد, tla->w_Minor_Ver_عدد,
			      tla->lcid, tla->syskind);
	    typelib->ReleaseTLibAttr(tla);
	}
	typelib->Release();
    }

    // Convert the CLSID into a char
    حرف clsid[GUID_STRING_ناپ];
    GUIDtochar(MYCLSID, clsid, حجم(clsid));

    // Build the key CLSID\\{...}
    حرف کلید_2[MAX_CLSID_LEN];
    strcpy(کلید_2, "CLSID\\");
    strcat(کلید_2, clsid);

    // Delete the CLSID Key - CLSID\{...}
    Recursive_Delete_کلید(HKEY_CLASSES_جڑ, کلید_2);

    // Delete the version-independent ProgID Key
    Recursive_Delete_کلید(HKEY_CLASSES_جڑ, MYVIPROGID);

    // Delete the ProgID key
    Recursive_Delete_کلید(HKEY_CLASSES_جڑ, MYPROGID);

    إذا (bNotifyUser)
	سنیہا_Box(0, "Unregistered successfully", "Vim", 0);
}

/****************************************************************************/

// Convert a GUID to a char string
ساكن فراغ GUIDtochar(ثابت GUID &guid, حرف *GUID, صحيح لمائی)
{
    // Get wide string version
    LPOLESTR wGUID = عدم;
    StringFromCLSID(guid, &wGUID);

    // Convert from wide characters to non-wide
    wcstombs(GUID, wGUID, لمائی);

    // Free memory
    CoTaskMemFree(wGUID);
}

// Delete a key and all of its descendants
ساكن فراغ Recursive_Delete_کلید(HKEY h_کلید_ماپا, ثابت حرف *بچہ)
{
    // Open the child
    HKEY h_کلید_بچہ;
    LONG نتیجہ = Reg_Open_کلید_Ex(h_کلید_ماپا, بچہ, 0,
			       کلید_ALL_ACCESS, &h_کلید_بچہ);
    إذا (نتیجہ != ERROR_SUCCESS)
	أرجع;

    // Enumerate all of the descendants of this child
    FILETIME ویلا;
    حرف بفر[1024];
    DWORD ناپ = 1024;

    طالما (Reg_Enum_کلید_Ex(h_کلید_بچہ, 0, بفر, &ناپ, عدم,
			عدم, عدم, &ویلا) == S_OK)
    {
	// Delete the descendants of this child
	Recursive_Delete_کلید(h_کلید_بچہ, بفر);
	ناپ = 256;
    }

    // Close the child
    Reg_Close_کلید(h_کلید_بچہ);

    // Delete this child
    Reg_Delete_کلید(h_کلید_ماپا, بچہ);
}

// Create a key and set its value
ساكن فراغ سیٹ_کلید_And_قدر(ثابت حرف *کلید, ثابت حرف *subkey, ثابت حرف *قدر)
{
    HKEY h_کلید;
    حرف بفر[1024];

    strcpy(بفر, کلید);

    // Add subkey name to buffer.
    إذا (subkey)
    {
	strcat(بفر, "\\");
	strcat(بفر, subkey);
    }

    // Create and open key and subkey.
    لما نتیجہ = Reg_Create_کلید_Ex(HKEY_CLASSES_جڑ,
				 بفر,
				 0, عدم, REG_OPTION_NON_VOLATILE,
				 کلید_ALL_ACCESS, عدم,
				 &h_کلید, عدم);
    إذا (نتیجہ != ERROR_SUCCESS)
	أرجع;

    // Set the value
    إذا (قدر)
	Reg_سیٹ_قدر_Ex(h_کلید, عدم, 0, REG_SZ, (BYTE *)قدر,
		      (DWORD)STRLEN(قدر)+1);

    Reg_Close_کلید(h_کلید);
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
	سنیہا_Box(0, "Cannot initialise OLE", "Vim Initialisation", 0);
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
    hr = Co_رجسٹر_Class_Object(
	MYCLSID,
	cf,
	CLSCTX_LOCAL_SERVER,
	REGCLS_MULTIPLEUSE,
	&cf_id);

    إذا (FAILED(hr))
    {
	سنیہا_Box(0, "Cannot register class factory", "Vim Initialisation", 0);
	اذهب error1;
    }

    // Register the application object as active
    hr = رجسٹر_سرگرم_Object(
	app,
	MYCLSID,
	0,
	&app_id);

    إذا (FAILED(hr))
    {
	سنیہا_Box(0, "Cannot register application object", "Vim Initialisation", 0);
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
	Revoke_سرگرم_Object(app_id, عدم);
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
	مٹاؤ app;
	app = عدم;
    }

    // Delete the class factory
    إذا (cf)
    {
	مٹاؤ cf;
	cf = عدم;
    }
}
#جے_انت /* FEAT_OLE */
