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
خارجی "C" {
# include "vim.h"
}

#شامل <windows.h>
#شامل <oleauto.h>

خارجی "C" {
خارجی HWND s_hwnd;
خارجی HWND vim_والد_hwnd;
}

#اگر (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#ورنہ
# define FINAL
#اگر_ختم

#شامل "if_ole.h"	// Interface definitions
#شامل "iid_ole.c"	// UUID definitions (compile here)

/* Supply function prototype to work around bug in Mingw oleauto.h header */
#اگر_تعریف_شدہ __MINGW32__
WINOLEAUTAPI Un_رجسٹر_Type_Lib(REFGUID libID, لفظ_2 wVerMajor,
	    لفظ_2 wVerMinor, LCID lcid, SYSKIND syskind);
#اگر_ختم

/*****************************************************************************
 1. Internal definitions for this file
*****************************************************************************/

جماعت CVim;
جماعت CVimCF;

/* Internal data */
// The identifier of the registered class factory
ساکن غیرعلامتی طویل cf_id = 0;

// The identifier of the running application object
ساکن غیرعلامتی طویل app_id = 0;

// The single global instance of the class factory
ساکن CVimCF *cf = 0;

// The single global instance of the application object
ساکن CVim *app = 0;

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

جماعت CVim FINAL : عوامی IVim
{
عوامی:
    مجازی ~CVim();
    ساکن CVim *Create(صحیح *pbDoRestart);

    // IUnknown members
    STDMETHOD(QueryInterface)(REFIID riid, خالی ** ppv);
    STDMETHOD_(غیرعلامتی طویل, جوڑو_Ref)(خالی);
    STDMETHOD_(غیرعلامتی طویل, Release)(خالی);

    // IDispatch members
    STDMETHOD(Get_Type_Info_گنتی)(UINT *p_گنتی);
    STDMETHOD(GetTypeInfo)(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo);
    STDMETHOD(Get_IDs_کا_نام)(ثابت IID &iid, OLECHAR **نام, UINT n, LCID, DISPID *dispids);
    STDMETHOD(Invoke)(DISPID member, ثابت IID &iid, LCID, لفظ_2 flags, DISPPARAMS *dispparams, VARIANT *نتیجہ, EXCEPINFO *excepinfo, UINT *argerr);

    // IVim members
    STDMETHOD(Send_کلیدیں)(BSTR کلیدیں);
    STDMETHOD(Eval)(BSTR اظہاریہ, BSTR *نتیجہ);
    STDMETHOD(سیٹ_Foreground)(خالی);
    STDMETHOD(GetHwnd)(UINT_PTR *نتیجہ);

نجی:
    // Constructor is private - create using CVim::Create()
    CVim() : ref(0), typeinfo(0) {};

    // Reference count
    غیرعلامتی طویل ref;

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
    me = نیا CVim();
    اگر (me == تہی)
    {
	پیغام_Box(0, "Cannot create application object", "Vim Initialisation", 0);
	واپس تہی;
    }

    // Load the type library from the registry
    hr = بھرو_Reg_Type_Lib(MYLIBID, 1, 0, 0x00, &typelib);
    اگر (FAILED(hr))
    {
	HKEY h_کلید;

	// Check we can write to the registry.
	// RegCreateKeyEx succeeds even if key exists. W.Briscoe W2K 20021011
	اگر (Reg_Create_کلید_Ex(HKEY_CLASSES_جڑ, MYVIPROGID, 0, تہی,
		  REG_OPTION_NON_VOLATILE,
		  کلید_ALL_ACCESS, تہی, &h_کلید, تہی))
	{
	    حذف me;
	    واپس تہی; // Unable to write to registry. Quietly fail.
	}
	Reg_Close_کلید(h_کلید);

	اگر (پیغام_Box(0, "Cannot load registered type library.\nDo you want to register Vim now?",
		    "Vim Initialisation", MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
	    حذف me;
	    واپس تہی;
	}

	رجسٹر_Me(FALSE);

	// Load the type library from the registry
	hr = بھرو_Reg_Type_Lib(MYLIBID, 1, 0, 0x00, &typelib);
	اگر (FAILED(hr))
	{
	    پیغام_Box(0, "You must restart Vim in order for the registration to take effect.",
						     "Vim Initialisation", 0);
	    *pbDoRestart = TRUE;
	    حذف me;
	    واپس تہی;
	}
    }

    // Get the type info of the vtable interface
    hr = typelib->Get_Type_Info_کا_Guid(MYIID, &typeinfo);
    typelib->Release();

    اگر (FAILED(hr))
    {
	پیغام_Box(0, "Cannot get interface type information",
						     "Vim Initialisation", 0);
	حذف me;
	واپس تہی;
    }

    // Save the type information
    me->typeinfo = typeinfo;
    واپس me;
}

CVim::~CVim()
{
    اگر (typeinfo && vim_والد_hwnd == تہی)
	typeinfo->Release();
    typeinfo = 0;
}

STDMETHODIMP
CVim::QueryInterface(REFIID riid, خالی **ppv)
{
    اگر (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDispatch) || IsEqualIID(riid, MYIID))
    {
	جوڑو_Ref();
	*ppv = یہ;
	واپس S_OK;
    }

    *ppv = 0;
    واپس E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVim::جوڑو_Ref()
{
    واپس ++ref;
}

STDMETHODIMP_(ULONG)
CVim::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    اگر (ref > 0)
	--ref;
    واپس ref;
}

STDMETHODIMP
CVim::Get_Type_Info_گنتی(UINT *p_گنتی)
{
    *p_گنتی = 1;
    واپس S_OK;
}

STDMETHODIMP
CVim::GetTypeInfo(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo)
{
    *ppITypeInfo = 0;

    اگر (iTypeInfo != 0)
	واپس DISP_E_BADINDEX;

    typeinfo->جوڑو_Ref();
    *ppITypeInfo = typeinfo;
    واپس S_OK;
}

STDMETHODIMP
CVim::Get_IDs_کا_نام(
	ثابت IID &iid,
	OLECHAR **نام,
	UINT n,
	LCID,
	DISPID *dispids)
{
    اگر (iid != IID_NULL)
	واپس DISP_E_UNKNOWNINTERFACE;

    واپس typeinfo->Get_IDs_کا_نام(نام, n, dispids);
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
    اگر (iid != IID_NULL)
	واپس DISP_E_UNKNOWNINTERFACE;

    ::سیٹ_Error_Info(0, تہی);
    واپس typeinfo->Invoke(static_cast<IDispatch*>(یہ),
			    member, flags, dispparams,
			    نتیجہ, excepinfo, argerr);
}

STDMETHODIMP
CVim::GetHwnd(UINT_PTR *نتیجہ)
{
    *نتیجہ = (UINT_PTR)s_hwnd;
    واپس S_OK;
}

STDMETHODIMP
CVim::سیٹ_Foreground(خالی)
{
    /* Make the Vim window come to the foreground */
    gui_mch_سیٹ_foreground();
    واپس S_OK;
}

STDMETHODIMP
CVim::Send_کلیدیں(BSTR کلیدیں)
{
    صحیح len;
    حرف *بفر;
    char_u *str;
    char_u *ptr;

    /* Get a suitable buffer */
    len = Wide_Char_مقصد_Multi_Byte(CP_ACP, 0, کلیدیں, -1, 0, 0, 0, 0);
    بفر = (حرف *)alloc(len+1);

    اگر (بفر == تہی)
	واپس E_OUTOFMEMORY;

    len = Wide_Char_مقصد_Multi_Byte(CP_ACP, 0, کلیدیں, -1, بفر, len, 0, 0);

    اگر (len == 0)
    {
	vim_free(بفر);
	واپس E_INVALIDARG;
    }

    /* Translate key codes like <Esc> */
    str = replace_termcodes((char_u *)بفر, &ptr, 0, REPTERM_DO_LT, تہی);

    /* If ptr was set, then a new buffer was allocated,
     * so we can free the old one.
     */
    اگر (ptr)
	vim_free((char_u *)(بفر));

    /* Reject strings too long to fit in the input buffer. Allow 10 bytes
     * space to cover for the (remote) possibility that characters may enter
     * the input buffer between now and when the WM_OLE message is actually
     * processed. If more than 10 characters enter the input buffer in that
     * time, the WM_OLE processing will simply fail to insert the characters.
     */
    اگر ((صحیح)(STRLEN(str)) > (vim_free_اندر_داخلہ_buf() - 10))
    {
	vim_free(str);
	واپس E_INVALIDARG;
    }

    /* Pass the string to the main input loop. The memory will be freed when
     * the message is processed.  Except for an empty message, we don't need
     * to post it then.
     */
    اگر (*str == NUL)
	vim_free(str);
    ورنہ
	Post_پیغام(تہی, WM_OLE, 0, (LPARAM)str);

    واپس S_OK;
}

STDMETHODIMP
CVim::Eval(BSTR اظہاریہ, BSTR *نتیجہ)
{
#اگر_تعریف_شدہ FEAT_EVAL
    صحیح len;
    حرف *بفر;
    حرف *str;
    wchar_t *w_بفر;

    /* Get a suitable buffer */
    len = Wide_Char_مقصد_Multi_Byte(CP_ACP, 0, اظہاریہ, -1, 0, 0, 0, 0);
    اگر (len == 0)
	واپس E_INVALIDARG;

    بفر = (حرف *)alloc(len);

    اگر (بفر == تہی)
	واپس E_OUTOFMEMORY;

    /* Convert the (wide character) expression to an ASCII string */
    len = Wide_Char_مقصد_Multi_Byte(CP_ACP, 0, اظہاریہ, -1, بفر, len, 0, 0);
    اگر (len == 0)
	واپس E_INVALIDARG;

    /* Evaluate the expression */
    ++emsg_skip;
    str = (حرف *)eval_مقصد_string((char_u *)بفر, TRUE, FALSE);
    --emsg_skip;
    vim_free(بفر);
    اگر (str == تہی)
	واپس E_FAIL;

    /* Convert the result to wide characters */
    Multi_Byte_مقصد_Wide_Char_alloc(CP_ACP, 0, str, -1, &w_بفر, &len);
    vim_free(str);
    اگر (w_بفر == تہی)
	واپس E_OUTOFMEMORY;

    اگر (len == 0)
    {
	vim_free(w_بفر);
	واپس E_FAIL;
    }

    /* Store the result */
    *نتیجہ = SysAllocString(w_بفر);
    vim_free(w_بفر);

    واپس S_OK;
#ورنہ
    واپس E_NOTIMPL;
#اگر_ختم
}

/*****************************************************************************
 3. The class factory
*****************************************************************************/

/* Definition
 * ----------
 */

جماعت CVimCF FINAL : عوامی IClassFactory
{
عوامی:
    ساکن CVimCF *Create();
    مجازی ~CVimCF() {};

    STDMETHOD(QueryInterface)(REFIID riid, خالی ** ppv);
    STDMETHOD_(غیرعلامتی طویل, جوڑو_Ref)(خالی);
    STDMETHOD_(غیرعلامتی طویل, Release)(خالی);
    STDMETHOD(CreateInstance)(IUnknown *punkOuter, REFIID riid, خالی ** ppv);
    STDMETHOD(تالا_Server)(BOOL تالا);

نجی:
    // Constructor is private - create via Create()
    CVimCF() : ref(0) {};

    // Reference count
    غیرعلامتی طویل ref;
};

/* Implementation
 * --------------
 */

CVimCF *CVimCF::Create()
{
    CVimCF *me = نیا CVimCF();

    اگر (me == تہی)
	پیغام_Box(0, "Cannot create class factory", "Vim Initialisation", 0);

    واپس me;
}

STDMETHODIMP
CVimCF::QueryInterface(REFIID riid, خالی **ppv)
{
    اگر (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	جوڑو_Ref();
	*ppv = یہ;
	واپس S_OK;
    }

    *ppv = 0;
    واپس E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVimCF::جوڑو_Ref()
{
    واپس ++ref;
}

STDMETHODIMP_(ULONG)
CVimCF::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    اگر (ref > 0)
	--ref;
    واپس ref;
}

/*ARGSUSED*/
STDMETHODIMP
CVimCF::CreateInstance(IUnknown *punkOuter, REFIID riid, خالی **ppv)
{
    واپس app->QueryInterface(riid, ppv);
}

/*ARGSUSED*/
STDMETHODIMP
CVimCF::تالا_Server(BOOL تالا)
{
    واپس S_OK;
}

/*****************************************************************************
 4. Registry manipulation code
*****************************************************************************/

// Internal use only
ساکن خالی سیٹ_کلید_And_قدر(ثابت حرف *راستہ, ثابت حرف *subkey, ثابت حرف *قدر);
ساکن خالی GUIDtochar(ثابت GUID &guid, حرف *GUID, صحیح لمبائی);
ساکن خالی Recursive_Delete_کلید(HKEY h_کلید_والد, ثابت حرف *بچہ);
ساکن ثابت صحیح GUID_STRING_سائز = 39;

// Register the component in the registry
// When "silent" is TRUE don't give any messages.

خارجی "C" خالی رجسٹر_Me(صحیح silent)
{
    BOOL ok = TRUE;

    // Get the application startup command
    حرف module[MAX_راستہ];

    ::GetModuleFileName(تہی, module, MAX_راستہ);

    // Unregister first (quietly)
    UnregisterMe(FALSE);

    // Convert the CLSID into a char
    حرف clsid[GUID_STRING_سائز];
    GUIDtochar(MYCLSID, clsid, حجم(clsid));

    // Convert the LIBID into a char
    حرف libid[GUID_STRING_سائز];
    GUIDtochar(MYLIBID, libid, حجم(libid));

    // Build the key CLSID\\{...}
    حرف کلید_2[MAX_CLSID_LEN];
    strcpy(کلید_2, "CLSID\\");
    strcat(کلید_2, clsid);

    // Add the CLSID to the registry
    سیٹ_کلید_And_قدر(کلید_2, تہی, MYNAME);
    سیٹ_کلید_And_قدر(کلید_2, "LocalServer32", module);
    سیٹ_کلید_And_قدر(کلید_2, "ProgID", MYPROGID);
    سیٹ_کلید_And_قدر(کلید_2, "VersionIndependentProgID", MYVIPROGID);
    سیٹ_کلید_And_قدر(کلید_2, "TypeLib", libid);

    // Add the version-independent ProgID subkey under HKEY_CLASSES_ROOT
    سیٹ_کلید_And_قدر(MYVIPROGID, تہی, MYNAME);
    سیٹ_کلید_And_قدر(MYVIPROGID, "CLSID", clsid);
    سیٹ_کلید_And_قدر(MYVIPROGID, "CurVer", MYPROGID);

    // Add the versioned ProgID subkey under HKEY_CLASSES_ROOT
    سیٹ_کلید_And_قدر(MYPROGID, تہی, MYNAME);
    سیٹ_کلید_And_قدر(MYPROGID, "CLSID", clsid);

    wchar_t w_module[MAX_راستہ];
    Multi_Byte_مقصد_Wide_Char(CP_ACP, 0, module, -1, w_module, MAX_راستہ);

    ITypeLib *typelib = تہی;
    اگر (بھرو_Type_Lib(w_module, &typelib) != S_OK)
    {
	اگر (!silent)
	    پیغام_Box(0, "Cannot load type library to register",
						       "Vim Registration", 0);
	ok = FALSE;
    }
    ورنہ
    {
	اگر (رجسٹر_Type_Lib(typelib, w_module, تہی) != S_OK)
	{
	    اگر (!silent)
		پیغام_Box(0, "Cannot register type library",
						       "Vim Registration", 0);
	    ok = FALSE;
	}
	typelib->Release();
    }

    اگر (ok && !silent)
	پیغام_Box(0, "Registered successfully", "Vim", 0);
}

// Remove the component from the registry
//
// Note: There is little error checking in this code, to allow incomplete
// or failed registrations to be undone.
خارجی "C" خالی UnregisterMe(صحیح bNotifyUser)
{
    // Unregister the type library
    ITypeLib *typelib;
    اگر (SUCCEEDED(بھرو_Reg_Type_Lib(MYLIBID, MAJORVER, MINORVER, LOCALE, &typelib)))
    {
	TLIBATTR *tla;
	اگر (SUCCEEDED(typelib->GetLibAttr(&tla)))
	{
	    Un_رجسٹر_Type_Lib(tla->guid, tla->w_Major_Ver_عدد, tla->w_Minor_Ver_عدد,
			      tla->lcid, tla->syskind);
	    typelib->ReleaseTLibAttr(tla);
	}
	typelib->Release();
    }

    // Convert the CLSID into a char
    حرف clsid[GUID_STRING_سائز];
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

    اگر (bNotifyUser)
	پیغام_Box(0, "Unregistered successfully", "Vim", 0);
}

/****************************************************************************/

// Convert a GUID to a char string
ساکن خالی GUIDtochar(ثابت GUID &guid, حرف *GUID, صحیح لمبائی)
{
    // Get wide string version
    LPOLESTR wGUID = تہی;
    StringFromCLSID(guid, &wGUID);

    // Convert from wide characters to non-wide
    wcstombs(GUID, wGUID, لمبائی);

    // Free memory
    CoTaskMemFree(wGUID);
}

// Delete a key and all of its descendants
ساکن خالی Recursive_Delete_کلید(HKEY h_کلید_والد, ثابت حرف *بچہ)
{
    // Open the child
    HKEY h_کلید_بچہ;
    LONG نتیجہ = Reg_Open_کلید_Ex(h_کلید_والد, بچہ, 0,
			       کلید_ALL_ACCESS, &h_کلید_بچہ);
    اگر (نتیجہ != ERROR_SUCCESS)
	واپس;

    // Enumerate all of the descendants of this child
    FILETIME وقت;
    حرف بفر[1024];
    DWORD سائز = 1024;

    جب (Reg_Enum_کلید_Ex(h_کلید_بچہ, 0, بفر, &سائز, تہی,
			تہی, تہی, &وقت) == S_OK)
    {
	// Delete the descendants of this child
	Recursive_Delete_کلید(h_کلید_بچہ, بفر);
	سائز = 256;
    }

    // Close the child
    Reg_Close_کلید(h_کلید_بچہ);

    // Delete this child
    Reg_Delete_کلید(h_کلید_والد, بچہ);
}

// Create a key and set its value
ساکن خالی سیٹ_کلید_And_قدر(ثابت حرف *کلید, ثابت حرف *subkey, ثابت حرف *قدر)
{
    HKEY h_کلید;
    حرف بفر[1024];

    strcpy(بفر, کلید);

    // Add subkey name to buffer.
    اگر (subkey)
    {
	strcat(بفر, "\\");
	strcat(بفر, subkey);
    }

    // Create and open key and subkey.
    طویل نتیجہ = Reg_Create_کلید_Ex(HKEY_CLASSES_جڑ,
				 بفر,
				 0, تہی, REG_OPTION_NON_VOLATILE,
				 کلید_ALL_ACCESS, تہی,
				 &h_کلید, تہی);
    اگر (نتیجہ != ERROR_SUCCESS)
	واپس;

    // Set the value
    اگر (قدر)
	Reg_سیٹ_قدر_Ex(h_کلید, تہی, 0, REG_SZ, (BYTE *)قدر,
		      (DWORD)STRLEN(قدر)+1);

    Reg_Close_کلید(h_کلید);
}

/*****************************************************************************
 5. OLE Initialisation and shutdown processing
*****************************************************************************/
خارجی "C" خالی InitOLE(صحیح *pbDoRestart)
{
    HRESULT hr;

    *pbDoRestart = FALSE;

    // Initialize the OLE libraries
    hr = OleInitialize(تہی);
    اگر (FAILED(hr))
    {
	پیغام_Box(0, "Cannot initialise OLE", "Vim Initialisation", 0);
	جاؤ error0;
    }

    // Create the application object
    app = CVim::Create(pbDoRestart);
    اگر (app == تہی)
	جاؤ error1;

    // Create the class factory
    cf = CVimCF::Create();
    اگر (cf == تہی)
	جاؤ error1;

    // Register the class factory
    hr = Co_رجسٹر_Class_Object(
	MYCLSID,
	cf,
	CLSCTX_LOCAL_SERVER,
	REGCLS_MULTIPLEUSE,
	&cf_id);

    اگر (FAILED(hr))
    {
	پیغام_Box(0, "Cannot register class factory", "Vim Initialisation", 0);
	جاؤ error1;
    }

    // Register the application object as active
    hr = رجسٹر_فعال_Object(
	app,
	MYCLSID,
	0,
	&app_id);

    اگر (FAILED(hr))
    {
	پیغام_Box(0, "Cannot register application object", "Vim Initialisation", 0);
	جاؤ error1;
    }

    واپس;

    // Errors: tidy up as much as needed and return
error1:
    UninitOLE();
error0:
    واپس;
}

خارجی "C" خالی UninitOLE()
{
    // Unregister the application object
    اگر (app_id)
    {
	Revoke_فعال_Object(app_id, تہی);
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
	حذف app;
	app = تہی;
    }

    // Delete the class factory
    اگر (cf)
    {
	حذف cf;
	cf = تہی;
    }
}
#اگر_ختم /* FEAT_OLE */
