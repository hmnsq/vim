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
خارجي HWND vim_والد_hwnd;
}

#إذا (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#وإلا
# define FINAL
#جيڪڏهن_پورو

#ضمن "if_ole.h"	// Interface definitions
#ضمن "iid_ole.c"	// UUID definitions (compile here)

/* Supply function prototype to work around bug in Mingw oleauto.h header */
#جيڪڏهن_وصف_ٿيل __MINGW32__
WINOLEAUTAPI Un_رجسٽر_Type_Lib(REFGUID libID, لفظ_2 wVerMajor,
	    لفظ_2 wVerMinor, LCID lcid, SYSKIND syskind);
#جيڪڏهن_پورو

/*****************************************************************************
 1. Internal definitions for this file
*****************************************************************************/

صنف CVim;
صنف CVimCF;

/* Internal data */
// The identifier of the registered class factory
ساكن بنانشان ڊگهو cf_id = 0;

// The identifier of the running application object
ساكن بنانشان ڊگهو app_id = 0;

// The single global instance of the class factory
ساكن CVimCF *cf = 0;

// The single global instance of the application object
ساكن CVim *app = 0;

/* GUIDs, versions and type library information */
#وصف_ڪريو MYCLSID CLSID_Vim
#وصف_ڪريو MYLIBID LIBID_Vim
#وصف_ڪريو MYIID IID_IVim

#وصف_ڪريو MAJORVER 1
#وصف_ڪريو MINORVER 0
#وصف_ڪريو LOCALE 0x0409

#وصف_ڪريو MYNAME "Vim"
#وصف_ڪريو MYPROGID "Vim.Application.1"
#وصف_ڪريو MYVIPROGID "Vim.Application"

#وصف_ڪريو MAX_CLSID_LEN 100

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
    STDMETHOD_(بنانشان ڊگهو, جوڙيو_Ref)(فراغ);
    STDMETHOD_(بنانشان ڊگهو, Release)(فراغ);

    // IDispatch members
    STDMETHOD(Get_Type_Info_ڳڻپ)(UINT *p_ڳڻپ);
    STDMETHOD(GetTypeInfo)(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo);
    STDMETHOD(Get_IDs_جو_نالا)(ثابت IID &iid, OLECHAR **نالا, UINT n, LCID, DISPID *dispids);
    STDMETHOD(Invoke)(DISPID member, ثابت IID &iid, LCID, لفظ_2 flags, DISPPARAMS *dispparams, VARIANT *نتيجو, EXCEPINFO *excepinfo, UINT *argerr);

    // IVim members
    STDMETHOD(Send_ڪنجيون)(BSTR ڪنجيون);
    STDMETHOD(Eval)(BSTR اظهار, BSTR *نتيجو);
    STDMETHOD(سيٽ_Foreground)(فراغ);
    STDMETHOD(GetHwnd)(UINT_PTR *نتيجو);

خاص:
    // Constructor is private - create using CVim::Create()
    CVim() : ref(0), typeinfo(0) {};

    // Reference count
    بنانشان ڊگهو ref;

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
	پيغام_Box(0, "Cannot create application object", "Vim Initialisation", 0);
	أرجع عدم;
    }

    // Load the type library from the registry
    hr = ڀريو_Reg_Type_Lib(MYLIBID, 1, 0, 0x00, &typelib);
    إذا (FAILED(hr))
    {
	HKEY h_ڪنجي;

	// Check we can write to the registry.
	// RegCreateKeyEx succeeds even if key exists. W.Briscoe W2K 20021011
	إذا (Reg_Create_ڪنجي_Ex(HKEY_CLASSES_پاڙ, MYVIPROGID, 0, عدم,
		  REG_OPTION_NON_VOLATILE,
		  ڪنجي_ALL_ACCESS, عدم, &h_ڪنجي, عدم))
	{
	    ميٽيو me;
	    أرجع عدم; // Unable to write to registry. Quietly fail.
	}
	Reg_Close_ڪنجي(h_ڪنجي);

	إذا (پيغام_Box(0, "Cannot load registered type library.\nDo you want to register Vim now?",
		    "Vim Initialisation", MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
	    ميٽيو me;
	    أرجع عدم;
	}

	رجسٽر_Me(FALSE);

	// Load the type library from the registry
	hr = ڀريو_Reg_Type_Lib(MYLIBID, 1, 0, 0x00, &typelib);
	إذا (FAILED(hr))
	{
	    پيغام_Box(0, "You must restart Vim in order for the registration to take effect.",
						     "Vim Initialisation", 0);
	    *pbDoRestart = TRUE;
	    ميٽيو me;
	    أرجع عدم;
	}
    }

    // Get the type info of the vtable interface
    hr = typelib->Get_Type_Info_جو_Guid(MYIID, &typeinfo);
    typelib->Release();

    إذا (FAILED(hr))
    {
	پيغام_Box(0, "Cannot get interface type information",
						     "Vim Initialisation", 0);
	ميٽيو me;
	أرجع عدم;
    }

    // Save the type information
    me->typeinfo = typeinfo;
    أرجع me;
}

CVim::~CVim()
{
    إذا (typeinfo && vim_والد_hwnd == عدم)
	typeinfo->Release();
    typeinfo = 0;
}

STDMETHODIMP
CVim::QueryInterface(REFIID riid, فراغ **ppv)
{
    إذا (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDispatch) || IsEqualIID(riid, MYIID))
    {
	جوڙيو_Ref();
	*ppv = هذا;
	أرجع S_OK;
    }

    *ppv = 0;
    أرجع E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVim::جوڙيو_Ref()
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
CVim::Get_Type_Info_ڳڻپ(UINT *p_ڳڻپ)
{
    *p_ڳڻپ = 1;
    أرجع S_OK;
}

STDMETHODIMP
CVim::GetTypeInfo(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo)
{
    *ppITypeInfo = 0;

    إذا (iTypeInfo != 0)
	أرجع DISP_E_BADINDEX;

    typeinfo->جوڙيو_Ref();
    *ppITypeInfo = typeinfo;
    أرجع S_OK;
}

STDMETHODIMP
CVim::Get_IDs_جو_نالا(
	ثابت IID &iid,
	OLECHAR **نالا,
	UINT n,
	LCID,
	DISPID *dispids)
{
    إذا (iid != IID_NULL)
	أرجع DISP_E_UNKNOWNINTERFACE;

    أرجع typeinfo->Get_IDs_جو_نالا(نالا, n, dispids);
}

STDMETHODIMP
CVim::Invoke(
	DISPID member,
	ثابت IID &iid,
	LCID,
	لفظ_2 flags,
	DISPPARAMS *dispparams,
	VARIANT *نتيجو,
	EXCEPINFO *excepinfo,
	UINT *argerr)
{
    إذا (iid != IID_NULL)
	أرجع DISP_E_UNKNOWNINTERFACE;

    ::سيٽ_Error_Info(0, عدم);
    أرجع typeinfo->Invoke(static_cast<IDispatch*>(هذا),
			    member, flags, dispparams,
			    نتيجو, excepinfo, argerr);
}

STDMETHODIMP
CVim::GetHwnd(UINT_PTR *نتيجو)
{
    *نتيجو = (UINT_PTR)s_hwnd;
    أرجع S_OK;
}

STDMETHODIMP
CVim::سيٽ_Foreground(فراغ)
{
    /* Make the Vim window come to the foreground */
    gui_mch_سيٽ_foreground();
    أرجع S_OK;
}

STDMETHODIMP
CVim::Send_ڪنجيون(BSTR ڪنجيون)
{
    صحيح len;
    حرف *بفر;
    char_u *str;
    char_u *ptr;

    /* Get a suitable buffer */
    len = Wide_Char_منزل_Multi_Byte(CP_ACP, 0, ڪنجيون, -1, 0, 0, 0, 0);
    بفر = (حرف *)alloc(len+1);

    إذا (بفر == عدم)
	أرجع E_OUTOFMEMORY;

    len = Wide_Char_منزل_Multi_Byte(CP_ACP, 0, ڪنجيون, -1, بفر, len, 0, 0);

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
    إذا ((صحيح)(STRLEN(str)) > (vim_free_اندريون_داخلا_buf() - 10))
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
	Post_پيغام(عدم, WM_OLE, 0, (LPARAM)str);

    أرجع S_OK;
}

STDMETHODIMP
CVim::Eval(BSTR اظهار, BSTR *نتيجو)
{
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    صحيح len;
    حرف *بفر;
    حرف *str;
    wchar_t *w_بفر;

    /* Get a suitable buffer */
    len = Wide_Char_منزل_Multi_Byte(CP_ACP, 0, اظهار, -1, 0, 0, 0, 0);
    إذا (len == 0)
	أرجع E_INVALIDARG;

    بفر = (حرف *)alloc(len);

    إذا (بفر == عدم)
	أرجع E_OUTOFMEMORY;

    /* Convert the (wide character) expression to an ASCII string */
    len = Wide_Char_منزل_Multi_Byte(CP_ACP, 0, اظهار, -1, بفر, len, 0, 0);
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
    *نتيجو = SysAllocString(w_بفر);
    vim_free(w_بفر);

    أرجع S_OK;
#وإلا
    أرجع E_NOTIMPL;
#جيڪڏهن_پورو
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
    STDMETHOD_(بنانشان ڊگهو, جوڙيو_Ref)(فراغ);
    STDMETHOD_(بنانشان ڊگهو, Release)(فراغ);
    STDMETHOD(CreateInstance)(IUnknown *punkOuter, REFIID riid, فراغ ** ppv);
    STDMETHOD(تالو_Server)(BOOL تالو);

خاص:
    // Constructor is private - create via Create()
    CVimCF() : ref(0) {};

    // Reference count
    بنانشان ڊگهو ref;
};

/* Implementation
 * --------------
 */

CVimCF *CVimCF::Create()
{
    CVimCF *me = جديد CVimCF();

    إذا (me == عدم)
	پيغام_Box(0, "Cannot create class factory", "Vim Initialisation", 0);

    أرجع me;
}

STDMETHODIMP
CVimCF::QueryInterface(REFIID riid, فراغ **ppv)
{
    إذا (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	جوڙيو_Ref();
	*ppv = هذا;
	أرجع S_OK;
    }

    *ppv = 0;
    أرجع E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVimCF::جوڙيو_Ref()
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
CVimCF::تالو_Server(BOOL تالو)
{
    أرجع S_OK;
}

/*****************************************************************************
 4. Registry manipulation code
*****************************************************************************/

// Internal use only
ساكن فراغ سيٽ_ڪنجي_And_قدر(ثابت حرف *رستو, ثابت حرف *subkey, ثابت حرف *قدر);
ساكن فراغ GUIDtochar(ثابت GUID &guid, حرف *GUID, صحيح ڊيگهائي);
ساكن فراغ Recursive_Delete_ڪنجي(HKEY h_ڪنجي_والد, ثابت حرف *ٻار);
ساكن ثابت صحيح GUID_STRING_سائيز = 39;

// Register the component in the registry
// When "silent" is TRUE don't give any messages.

خارجي "C" فراغ رجسٽر_Me(صحيح silent)
{
    BOOL ok = TRUE;

    // Get the application startup command
    حرف module[MAX_رستو];

    ::GetModuleFileName(عدم, module, MAX_رستو);

    // Unregister first (quietly)
    UnregisterMe(FALSE);

    // Convert the CLSID into a char
    حرف clsid[GUID_STRING_سائيز];
    GUIDtochar(MYCLSID, clsid, حجم(clsid));

    // Convert the LIBID into a char
    حرف libid[GUID_STRING_سائيز];
    GUIDtochar(MYLIBID, libid, حجم(libid));

    // Build the key CLSID\\{...}
    حرف ڪنجي_2[MAX_CLSID_LEN];
    strcpy(ڪنجي_2, "CLSID\\");
    strcat(ڪنجي_2, clsid);

    // Add the CLSID to the registry
    سيٽ_ڪنجي_And_قدر(ڪنجي_2, عدم, MYNAME);
    سيٽ_ڪنجي_And_قدر(ڪنجي_2, "LocalServer32", module);
    سيٽ_ڪنجي_And_قدر(ڪنجي_2, "ProgID", MYPROGID);
    سيٽ_ڪنجي_And_قدر(ڪنجي_2, "VersionIndependentProgID", MYVIPROGID);
    سيٽ_ڪنجي_And_قدر(ڪنجي_2, "TypeLib", libid);

    // Add the version-independent ProgID subkey under HKEY_CLASSES_ROOT
    سيٽ_ڪنجي_And_قدر(MYVIPROGID, عدم, MYNAME);
    سيٽ_ڪنجي_And_قدر(MYVIPROGID, "CLSID", clsid);
    سيٽ_ڪنجي_And_قدر(MYVIPROGID, "CurVer", MYPROGID);

    // Add the versioned ProgID subkey under HKEY_CLASSES_ROOT
    سيٽ_ڪنجي_And_قدر(MYPROGID, عدم, MYNAME);
    سيٽ_ڪنجي_And_قدر(MYPROGID, "CLSID", clsid);

    wchar_t w_module[MAX_رستو];
    Multi_Byte_منزل_Wide_Char(CP_ACP, 0, module, -1, w_module, MAX_رستو);

    ITypeLib *typelib = عدم;
    إذا (ڀريو_Type_Lib(w_module, &typelib) != S_OK)
    {
	إذا (!silent)
	    پيغام_Box(0, "Cannot load type library to register",
						       "Vim Registration", 0);
	ok = FALSE;
    }
    وإلا
    {
	إذا (رجسٽر_Type_Lib(typelib, w_module, عدم) != S_OK)
	{
	    إذا (!silent)
		پيغام_Box(0, "Cannot register type library",
						       "Vim Registration", 0);
	    ok = FALSE;
	}
	typelib->Release();
    }

    إذا (ok && !silent)
	پيغام_Box(0, "Registered successfully", "Vim", 0);
}

// Remove the component from the registry
//
// Note: There is little error checking in this code, to allow incomplete
// or failed registrations to be undone.
خارجي "C" فراغ UnregisterMe(صحيح bNotifyUser)
{
    // Unregister the type library
    ITypeLib *typelib;
    إذا (SUCCEEDED(ڀريو_Reg_Type_Lib(MYLIBID, MAJORVER, MINORVER, LOCALE, &typelib)))
    {
	TLIBATTR *tla;
	إذا (SUCCEEDED(typelib->GetLibAttr(&tla)))
	{
	    Un_رجسٽر_Type_Lib(tla->guid, tla->w_Major_Ver_انگ, tla->w_Minor_Ver_انگ,
			      tla->lcid, tla->syskind);
	    typelib->ReleaseTLibAttr(tla);
	}
	typelib->Release();
    }

    // Convert the CLSID into a char
    حرف clsid[GUID_STRING_سائيز];
    GUIDtochar(MYCLSID, clsid, حجم(clsid));

    // Build the key CLSID\\{...}
    حرف ڪنجي_2[MAX_CLSID_LEN];
    strcpy(ڪنجي_2, "CLSID\\");
    strcat(ڪنجي_2, clsid);

    // Delete the CLSID Key - CLSID\{...}
    Recursive_Delete_ڪنجي(HKEY_CLASSES_پاڙ, ڪنجي_2);

    // Delete the version-independent ProgID Key
    Recursive_Delete_ڪنجي(HKEY_CLASSES_پاڙ, MYVIPROGID);

    // Delete the ProgID key
    Recursive_Delete_ڪنجي(HKEY_CLASSES_پاڙ, MYPROGID);

    إذا (bNotifyUser)
	پيغام_Box(0, "Unregistered successfully", "Vim", 0);
}

/****************************************************************************/

// Convert a GUID to a char string
ساكن فراغ GUIDtochar(ثابت GUID &guid, حرف *GUID, صحيح ڊيگهائي)
{
    // Get wide string version
    LPOLESTR wGUID = عدم;
    StringFromCLSID(guid, &wGUID);

    // Convert from wide characters to non-wide
    wcstombs(GUID, wGUID, ڊيگهائي);

    // Free memory
    CoTaskMemFree(wGUID);
}

// Delete a key and all of its descendants
ساكن فراغ Recursive_Delete_ڪنجي(HKEY h_ڪنجي_والد, ثابت حرف *ٻار)
{
    // Open the child
    HKEY h_ڪنجي_ٻار;
    LONG نتيجو = Reg_Open_ڪنجي_Ex(h_ڪنجي_والد, ٻار, 0,
			       ڪنجي_ALL_ACCESS, &h_ڪنجي_ٻار);
    إذا (نتيجو != ERROR_SUCCESS)
	أرجع;

    // Enumerate all of the descendants of this child
    FILETIME وقت;
    حرف بفر[1024];
    DWORD سائيز = 1024;

    طالما (Reg_Enum_ڪنجي_Ex(h_ڪنجي_ٻار, 0, بفر, &سائيز, عدم,
			عدم, عدم, &وقت) == S_OK)
    {
	// Delete the descendants of this child
	Recursive_Delete_ڪنجي(h_ڪنجي_ٻار, بفر);
	سائيز = 256;
    }

    // Close the child
    Reg_Close_ڪنجي(h_ڪنجي_ٻار);

    // Delete this child
    Reg_Delete_ڪنجي(h_ڪنجي_والد, ٻار);
}

// Create a key and set its value
ساكن فراغ سيٽ_ڪنجي_And_قدر(ثابت حرف *ڪنجي, ثابت حرف *subkey, ثابت حرف *قدر)
{
    HKEY h_ڪنجي;
    حرف بفر[1024];

    strcpy(بفر, ڪنجي);

    // Add subkey name to buffer.
    إذا (subkey)
    {
	strcat(بفر, "\\");
	strcat(بفر, subkey);
    }

    // Create and open key and subkey.
    ڊگهو نتيجو = Reg_Create_ڪنجي_Ex(HKEY_CLASSES_پاڙ,
				 بفر,
				 0, عدم, REG_OPTION_NON_VOLATILE,
				 ڪنجي_ALL_ACCESS, عدم,
				 &h_ڪنجي, عدم);
    إذا (نتيجو != ERROR_SUCCESS)
	أرجع;

    // Set the value
    إذا (قدر)
	Reg_سيٽ_قدر_Ex(h_ڪنجي, عدم, 0, REG_SZ, (BYTE *)قدر,
		      (DWORD)STRLEN(قدر)+1);

    Reg_Close_ڪنجي(h_ڪنجي);
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
	پيغام_Box(0, "Cannot initialise OLE", "Vim Initialisation", 0);
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
    hr = Co_رجسٽر_Class_Object(
	MYCLSID,
	cf,
	CLSCTX_LOCAL_SERVER,
	REGCLS_MULTIPLEUSE,
	&cf_id);

    إذا (FAILED(hr))
    {
	پيغام_Box(0, "Cannot register class factory", "Vim Initialisation", 0);
	اذهب error1;
    }

    // Register the application object as active
    hr = رجسٽر_سرگرم_Object(
	app,
	MYCLSID,
	0,
	&app_id);

    إذا (FAILED(hr))
    {
	پيغام_Box(0, "Cannot register application object", "Vim Initialisation", 0);
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
	ميٽيو app;
	app = عدم;
    }

    // Delete the class factory
    إذا (cf)
    {
	ميٽيو cf;
	cf = عدم;
    }
}
#جيڪڏهن_پورو /* FEAT_OLE */
