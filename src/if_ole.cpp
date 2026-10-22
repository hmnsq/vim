/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#ఒకవేళ defined(FEAT_OLE) && defined(FEAT_GUI_MSWIN)
/*
 * OLE server implementation.
 *
 * See os_mswin.c for the client side.
 */
బాహ్య "C" {
# include "vim.h"
}

#అన్తర్భావ <windows.h>
#అన్తర్భావ <oleauto.h>

బాహ్య "C" {
బాహ్య HWND s_hwnd;
బాహ్య HWND vim_మాతృ_hwnd;
}

#ఒకవేళ (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#లేకపోతే
# define FINAL
#అయితే_ముగింపు

#అన్తర్భావ "if_ole.h"	// Interface definitions
#అన్తర్భావ "iid_ole.c"	// UUID definitions (compile here)

/* Supply function prototype to work around bug in Mingw oleauto.h header */
#నిర్వచితమైతే __MINGW32__
WINOLEAUTAPI Un_రిజిస్టరు_Type_Lib(REFGUID libID, మాట_2 wVerMajor,
	    మాట_2 wVerMinor, LCID lcid, SYSKIND syskind);
#అయితే_ముగింపు

/*****************************************************************************
 1. Internal definitions for this file
*****************************************************************************/

తరగతి CVim;
తరగతి CVimCF;

/* Internal data */
// The identifier of the registered class factory
స్థిర గుర్తులేని దీర్ఘ cf_id = 0;

// The identifier of the running application object
స్థిర గుర్తులేని దీర్ఘ app_id = 0;

// The single global instance of the class factory
స్థిర CVimCF *cf = 0;

// The single global instance of the application object
స్థిర CVim *app = 0;

/* GUIDs, versions and type library information */
#నిర్వచించు MYCLSID CLSID_Vim
#నిర్వచించు MYLIBID LIBID_Vim
#నిర్వచించు MYIID IID_IVim

#నిర్వచించు MAJORVER 1
#నిర్వచించు MINORVER 0
#నిర్వచించు LOCALE 0x0409

#నిర్వచించు MYNAME "Vim"
#నిర్వచించు MYPROGID "Vim.Application.1"
#నిర్వచించు MYVIPROGID "Vim.Application"

#నిర్వచించు MAX_CLSID_LEN 100

/*****************************************************************************
 2. The application object
*****************************************************************************/

/* Definition
 * ----------
 */

తరగతి CVim FINAL : బహిరంగ IVim
{
బహిరంగ:
    మాయా ~CVim();
    స్థిర CVim *Create(పూర్ణాంకం *pbDoRestart);

    // IUnknown members
    STDMETHOD(QueryInterface)(REFIID riid, శూన్య ** ppv);
    STDMETHOD_(గుర్తులేని దీర్ఘ, కూడు_Ref)(శూన్య);
    STDMETHOD_(గుర్తులేని దీర్ఘ, Release)(శూన్య);

    // IDispatch members
    STDMETHOD(Get_Type_Info_లెక్క)(UINT *p_లెక్క);
    STDMETHOD(GetTypeInfo)(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo);
    STDMETHOD(Get_IDs_యొక్క_పేర్లు)(స్థిరరాశి IID &iid, OLECHAR **పేర్లు, UINT n, LCID, DISPID *dispids);
    STDMETHOD(Invoke)(DISPID member, స్థిరరాశి IID &iid, LCID, మాట_2 flags, DISPPARAMS *dispparams, VARIANT *ఫలితం, EXCEPINFO *excepinfo, UINT *argerr);

    // IVim members
    STDMETHOD(Send_కీలకాలు)(BSTR కీలకాలు);
    STDMETHOD(Eval)(BSTR సమాసం, BSTR *ఫలితం);
    STDMETHOD(సమితి_Foreground)(శూన్య);
    STDMETHOD(GetHwnd)(UINT_PTR *ఫలితం);

అంతరంగ:
    // Constructor is private - create using CVim::Create()
    CVim() : ref(0), typeinfo(0) {};

    // Reference count
    గుర్తులేని దీర్ఘ ref;

    // The object's TypeInfo
    ITypeInfo *typeinfo;
};

/* Implementation
 * --------------
 */

CVim *CVim::Create(పూర్ణాంకం *pbDoRestart)
{
    HRESULT hr;
    CVim *me = 0;
    ITypeLib *typelib = 0;
    ITypeInfo *typeinfo = 0;

    *pbDoRestart = FALSE;

    // Create the object
    me = కొత్త CVim();
    ఒకవేళ (me == ఖాళీ)
    {
	సందేశం_Box(0, "Cannot create application object", "Vim Initialisation", 0);
	తిరిగి ఖాళీ;
    }

    // Load the type library from the registry
    hr = ఎక్కించు_Reg_Type_Lib(MYLIBID, 1, 0, 0x00, &typelib);
    ఒకవేళ (FAILED(hr))
    {
	HKEY h_కీలకం;

	// Check we can write to the registry.
	// RegCreateKeyEx succeeds even if key exists. W.Briscoe W2K 20021011
	ఒకవేళ (Reg_Create_కీలకం_Ex(HKEY_CLASSES_మూలం, MYVIPROGID, 0, ఖాళీ,
		  REG_OPTION_NON_VOLATILE,
		  కీలకం_ALL_ACCESS, ఖాళీ, &h_కీలకం, ఖాళీ))
	{
	    తొలగించు me;
	    తిరిగి ఖాళీ; // Unable to write to registry. Quietly fail.
	}
	Reg_Close_కీలకం(h_కీలకం);

	ఒకవేళ (సందేశం_Box(0, "Cannot load registered type library.\nDo you want to register Vim now?",
		    "Vim Initialisation", MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
	    తొలగించు me;
	    తిరిగి ఖాళీ;
	}

	రిజిస్టరు_Me(FALSE);

	// Load the type library from the registry
	hr = ఎక్కించు_Reg_Type_Lib(MYLIBID, 1, 0, 0x00, &typelib);
	ఒకవేళ (FAILED(hr))
	{
	    సందేశం_Box(0, "You must restart Vim in order for the registration to take effect.",
						     "Vim Initialisation", 0);
	    *pbDoRestart = TRUE;
	    తొలగించు me;
	    తిరిగి ఖాళీ;
	}
    }

    // Get the type info of the vtable interface
    hr = typelib->Get_Type_Info_యొక్క_Guid(MYIID, &typeinfo);
    typelib->Release();

    ఒకవేళ (FAILED(hr))
    {
	సందేశం_Box(0, "Cannot get interface type information",
						     "Vim Initialisation", 0);
	తొలగించు me;
	తిరిగి ఖాళీ;
    }

    // Save the type information
    me->typeinfo = typeinfo;
    తిరిగి me;
}

CVim::~CVim()
{
    ఒకవేళ (typeinfo && vim_మాతృ_hwnd == ఖాళీ)
	typeinfo->Release();
    typeinfo = 0;
}

STDMETHODIMP
CVim::QueryInterface(REFIID riid, శూన్య **ppv)
{
    ఒకవేళ (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDispatch) || IsEqualIID(riid, MYIID))
    {
	కూడు_Ref();
	*ppv = ఇది;
	తిరిగి S_OK;
    }

    *ppv = 0;
    తిరిగి E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVim::కూడు_Ref()
{
    తిరిగి ++ref;
}

STDMETHODIMP_(ULONG)
CVim::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    ఒకవేళ (ref > 0)
	--ref;
    తిరిగి ref;
}

STDMETHODIMP
CVim::Get_Type_Info_లెక్క(UINT *p_లెక్క)
{
    *p_లెక్క = 1;
    తిరిగి S_OK;
}

STDMETHODIMP
CVim::GetTypeInfo(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo)
{
    *ppITypeInfo = 0;

    ఒకవేళ (iTypeInfo != 0)
	తిరిగి DISP_E_BADINDEX;

    typeinfo->కూడు_Ref();
    *ppITypeInfo = typeinfo;
    తిరిగి S_OK;
}

STDMETHODIMP
CVim::Get_IDs_యొక్క_పేర్లు(
	స్థిరరాశి IID &iid,
	OLECHAR **పేర్లు,
	UINT n,
	LCID,
	DISPID *dispids)
{
    ఒకవేళ (iid != IID_NULL)
	తిరిగి DISP_E_UNKNOWNINTERFACE;

    తిరిగి typeinfo->Get_IDs_యొక్క_పేర్లు(పేర్లు, n, dispids);
}

STDMETHODIMP
CVim::Invoke(
	DISPID member,
	స్థిరరాశి IID &iid,
	LCID,
	మాట_2 flags,
	DISPPARAMS *dispparams,
	VARIANT *ఫలితం,
	EXCEPINFO *excepinfo,
	UINT *argerr)
{
    ఒకవేళ (iid != IID_NULL)
	తిరిగి DISP_E_UNKNOWNINTERFACE;

    ::సమితి_Error_Info(0, ఖాళీ);
    తిరిగి typeinfo->Invoke(static_cast<IDispatch*>(ఇది),
			    member, flags, dispparams,
			    ఫలితం, excepinfo, argerr);
}

STDMETHODIMP
CVim::GetHwnd(UINT_PTR *ఫలితం)
{
    *ఫలితం = (UINT_PTR)s_hwnd;
    తిరిగి S_OK;
}

STDMETHODIMP
CVim::సమితి_Foreground(శూన్య)
{
    /* Make the Vim window come to the foreground */
    gui_mch_సమితి_foreground();
    తిరిగి S_OK;
}

STDMETHODIMP
CVim::Send_కీలకాలు(BSTR కీలకాలు)
{
    పూర్ణాంకం len;
    అక్షరం *బఫరు;
    char_u *str;
    char_u *ptr;

    /* Get a suitable buffer */
    len = Wide_Char_గమ్యం_Multi_Byte(CP_ACP, 0, కీలకాలు, -1, 0, 0, 0, 0);
    బఫరు = (అక్షరం *)alloc(len+1);

    ఒకవేళ (బఫరు == ఖాళీ)
	తిరిగి E_OUTOFMEMORY;

    len = Wide_Char_గమ్యం_Multi_Byte(CP_ACP, 0, కీలకాలు, -1, బఫరు, len, 0, 0);

    ఒకవేళ (len == 0)
    {
	vim_free(బఫరు);
	తిరిగి E_INVALIDARG;
    }

    /* Translate key codes like <Esc> */
    str = replace_termcodes((char_u *)బఫరు, &ptr, 0, REPTERM_DO_LT, ఖాళీ);

    /* If ptr was set, then a new buffer was allocated,
     * so we can free the old one.
     */
    ఒకవేళ (ptr)
	vim_free((char_u *)(బఫరు));

    /* Reject strings too long to fit in the input buffer. Allow 10 bytes
     * space to cover for the (remote) possibility that characters may enter
     * the input buffer between now and when the WM_OLE message is actually
     * processed. If more than 10 characters enter the input buffer in that
     * time, the WM_OLE processing will simply fail to insert the characters.
     */
    ఒకవేళ ((పూర్ణాంకం)(STRLEN(str)) > (vim_free_లోపల_ఆదానం_buf() - 10))
    {
	vim_free(str);
	తిరిగి E_INVALIDARG;
    }

    /* Pass the string to the main input loop. The memory will be freed when
     * the message is processed.  Except for an empty message, we don't need
     * to post it then.
     */
    ఒకవేళ (*str == NUL)
	vim_free(str);
    లేకపోతే
	Post_సందేశం(ఖాళీ, WM_OLE, 0, (LPARAM)str);

    తిరిగి S_OK;
}

STDMETHODIMP
CVim::Eval(BSTR సమాసం, BSTR *ఫలితం)
{
#నిర్వచితమైతే FEAT_EVAL
    పూర్ణాంకం len;
    అక్షరం *బఫరు;
    అక్షరం *str;
    wchar_t *w_బఫరు;

    /* Get a suitable buffer */
    len = Wide_Char_గమ్యం_Multi_Byte(CP_ACP, 0, సమాసం, -1, 0, 0, 0, 0);
    ఒకవేళ (len == 0)
	తిరిగి E_INVALIDARG;

    బఫరు = (అక్షరం *)alloc(len);

    ఒకవేళ (బఫరు == ఖాళీ)
	తిరిగి E_OUTOFMEMORY;

    /* Convert the (wide character) expression to an ASCII string */
    len = Wide_Char_గమ్యం_Multi_Byte(CP_ACP, 0, సమాసం, -1, బఫరు, len, 0, 0);
    ఒకవేళ (len == 0)
	తిరిగి E_INVALIDARG;

    /* Evaluate the expression */
    ++emsg_skip;
    str = (అక్షరం *)eval_గమ్యం_string((char_u *)బఫరు, TRUE, FALSE);
    --emsg_skip;
    vim_free(బఫరు);
    ఒకవేళ (str == ఖాళీ)
	తిరిగి E_FAIL;

    /* Convert the result to wide characters */
    Multi_Byte_గమ్యం_Wide_Char_alloc(CP_ACP, 0, str, -1, &w_బఫరు, &len);
    vim_free(str);
    ఒకవేళ (w_బఫరు == ఖాళీ)
	తిరిగి E_OUTOFMEMORY;

    ఒకవేళ (len == 0)
    {
	vim_free(w_బఫరు);
	తిరిగి E_FAIL;
    }

    /* Store the result */
    *ఫలితం = SysAllocString(w_బఫరు);
    vim_free(w_బఫరు);

    తిరిగి S_OK;
#లేకపోతే
    తిరిగి E_NOTIMPL;
#అయితే_ముగింపు
}

/*****************************************************************************
 3. The class factory
*****************************************************************************/

/* Definition
 * ----------
 */

తరగతి CVimCF FINAL : బహిరంగ IClassFactory
{
బహిరంగ:
    స్థిర CVimCF *Create();
    మాయా ~CVimCF() {};

    STDMETHOD(QueryInterface)(REFIID riid, శూన్య ** ppv);
    STDMETHOD_(గుర్తులేని దీర్ఘ, కూడు_Ref)(శూన్య);
    STDMETHOD_(గుర్తులేని దీర్ఘ, Release)(శూన్య);
    STDMETHOD(CreateInstance)(IUnknown *punkOuter, REFIID riid, శూన్య ** ppv);
    STDMETHOD(తాళం_Server)(BOOL తాళం);

అంతరంగ:
    // Constructor is private - create via Create()
    CVimCF() : ref(0) {};

    // Reference count
    గుర్తులేని దీర్ఘ ref;
};

/* Implementation
 * --------------
 */

CVimCF *CVimCF::Create()
{
    CVimCF *me = కొత్త CVimCF();

    ఒకవేళ (me == ఖాళీ)
	సందేశం_Box(0, "Cannot create class factory", "Vim Initialisation", 0);

    తిరిగి me;
}

STDMETHODIMP
CVimCF::QueryInterface(REFIID riid, శూన్య **ppv)
{
    ఒకవేళ (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	కూడు_Ref();
	*ppv = ఇది;
	తిరిగి S_OK;
    }

    *ppv = 0;
    తిరిగి E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVimCF::కూడు_Ref()
{
    తిరిగి ++ref;
}

STDMETHODIMP_(ULONG)
CVimCF::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    ఒకవేళ (ref > 0)
	--ref;
    తిరిగి ref;
}

/*ARGSUSED*/
STDMETHODIMP
CVimCF::CreateInstance(IUnknown *punkOuter, REFIID riid, శూన్య **ppv)
{
    తిరిగి app->QueryInterface(riid, ppv);
}

/*ARGSUSED*/
STDMETHODIMP
CVimCF::తాళం_Server(BOOL తాళం)
{
    తిరిగి S_OK;
}

/*****************************************************************************
 4. Registry manipulation code
*****************************************************************************/

// Internal use only
స్థిర శూన్య సమితి_కీలకం_And_విలువ(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *subkey, స్థిరరాశి అక్షరం *విలువ);
స్థిర శూన్య GUIDtochar(స్థిరరాశి GUID &guid, అక్షరం *GUID, పూర్ణాంకం నిడివి);
స్థిర శూన్య Recursive_Delete_కీలకం(HKEY h_కీలకం_మాతృ, స్థిరరాశి అక్షరం *బిడ్డ);
స్థిర స్థిరరాశి పూర్ణాంకం GUID_STRING_కొలత = 39;

// Register the component in the registry
// When "silent" is TRUE don't give any messages.

బాహ్య "C" శూన్య రిజిస్టరు_Me(పూర్ణాంకం silent)
{
    BOOL ok = TRUE;

    // Get the application startup command
    అక్షరం module[MAX_మార్గం];

    ::GetModuleFileName(ఖాళీ, module, MAX_మార్గం);

    // Unregister first (quietly)
    UnregisterMe(FALSE);

    // Convert the CLSID into a char
    అక్షరం clsid[GUID_STRING_కొలత];
    GUIDtochar(MYCLSID, clsid, పరిమాణం(clsid));

    // Convert the LIBID into a char
    అక్షరం libid[GUID_STRING_కొలత];
    GUIDtochar(MYLIBID, libid, పరిమాణం(libid));

    // Build the key CLSID\\{...}
    అక్షరం కీలకం_2[MAX_CLSID_LEN];
    strcpy(కీలకం_2, "CLSID\\");
    strcat(కీలకం_2, clsid);

    // Add the CLSID to the registry
    సమితి_కీలకం_And_విలువ(కీలకం_2, ఖాళీ, MYNAME);
    సమితి_కీలకం_And_విలువ(కీలకం_2, "LocalServer32", module);
    సమితి_కీలకం_And_విలువ(కీలకం_2, "ProgID", MYPROGID);
    సమితి_కీలకం_And_విలువ(కీలకం_2, "VersionIndependentProgID", MYVIPROGID);
    సమితి_కీలకం_And_విలువ(కీలకం_2, "TypeLib", libid);

    // Add the version-independent ProgID subkey under HKEY_CLASSES_ROOT
    సమితి_కీలకం_And_విలువ(MYVIPROGID, ఖాళీ, MYNAME);
    సమితి_కీలకం_And_విలువ(MYVIPROGID, "CLSID", clsid);
    సమితి_కీలకం_And_విలువ(MYVIPROGID, "CurVer", MYPROGID);

    // Add the versioned ProgID subkey under HKEY_CLASSES_ROOT
    సమితి_కీలకం_And_విలువ(MYPROGID, ఖాళీ, MYNAME);
    సమితి_కీలకం_And_విలువ(MYPROGID, "CLSID", clsid);

    wchar_t w_module[MAX_మార్గం];
    Multi_Byte_గమ్యం_Wide_Char(CP_ACP, 0, module, -1, w_module, MAX_మార్గం);

    ITypeLib *typelib = ఖాళీ;
    ఒకవేళ (ఎక్కించు_Type_Lib(w_module, &typelib) != S_OK)
    {
	ఒకవేళ (!silent)
	    సందేశం_Box(0, "Cannot load type library to register",
						       "Vim Registration", 0);
	ok = FALSE;
    }
    లేకపోతే
    {
	ఒకవేళ (రిజిస్టరు_Type_Lib(typelib, w_module, ఖాళీ) != S_OK)
	{
	    ఒకవేళ (!silent)
		సందేశం_Box(0, "Cannot register type library",
						       "Vim Registration", 0);
	    ok = FALSE;
	}
	typelib->Release();
    }

    ఒకవేళ (ok && !silent)
	సందేశం_Box(0, "Registered successfully", "Vim", 0);
}

// Remove the component from the registry
//
// Note: There is little error checking in this code, to allow incomplete
// or failed registrations to be undone.
బాహ్య "C" శూన్య UnregisterMe(పూర్ణాంకం bNotifyUser)
{
    // Unregister the type library
    ITypeLib *typelib;
    ఒకవేళ (SUCCEEDED(ఎక్కించు_Reg_Type_Lib(MYLIBID, MAJORVER, MINORVER, LOCALE, &typelib)))
    {
	TLIBATTR *tla;
	ఒకవేళ (SUCCEEDED(typelib->GetLibAttr(&tla)))
	{
	    Un_రిజిస్టరు_Type_Lib(tla->guid, tla->w_Major_Ver_అంకె, tla->w_Minor_Ver_అంకె,
			      tla->lcid, tla->syskind);
	    typelib->ReleaseTLibAttr(tla);
	}
	typelib->Release();
    }

    // Convert the CLSID into a char
    అక్షరం clsid[GUID_STRING_కొలత];
    GUIDtochar(MYCLSID, clsid, పరిమాణం(clsid));

    // Build the key CLSID\\{...}
    అక్షరం కీలకం_2[MAX_CLSID_LEN];
    strcpy(కీలకం_2, "CLSID\\");
    strcat(కీలకం_2, clsid);

    // Delete the CLSID Key - CLSID\{...}
    Recursive_Delete_కీలకం(HKEY_CLASSES_మూలం, కీలకం_2);

    // Delete the version-independent ProgID Key
    Recursive_Delete_కీలకం(HKEY_CLASSES_మూలం, MYVIPROGID);

    // Delete the ProgID key
    Recursive_Delete_కీలకం(HKEY_CLASSES_మూలం, MYPROGID);

    ఒకవేళ (bNotifyUser)
	సందేశం_Box(0, "Unregistered successfully", "Vim", 0);
}

/****************************************************************************/

// Convert a GUID to a char string
స్థిర శూన్య GUIDtochar(స్థిరరాశి GUID &guid, అక్షరం *GUID, పూర్ణాంకం నిడివి)
{
    // Get wide string version
    LPOLESTR wGUID = ఖాళీ;
    StringFromCLSID(guid, &wGUID);

    // Convert from wide characters to non-wide
    wcstombs(GUID, wGUID, నిడివి);

    // Free memory
    CoTaskMemFree(wGUID);
}

// Delete a key and all of its descendants
స్థిర శూన్య Recursive_Delete_కీలకం(HKEY h_కీలకం_మాతృ, స్థిరరాశి అక్షరం *బిడ్డ)
{
    // Open the child
    HKEY h_కీలకం_బిడ్డ;
    LONG ఫలితం = Reg_Open_కీలకం_Ex(h_కీలకం_మాతృ, బిడ్డ, 0,
			       కీలకం_ALL_ACCESS, &h_కీలకం_బిడ్డ);
    ఒకవేళ (ఫలితం != ERROR_SUCCESS)
	తిరిగి;

    // Enumerate all of the descendants of this child
    FILETIME సమయం;
    అక్షరం బఫరు[1024];
    DWORD కొలత = 1024;

    అయితే (Reg_Enum_కీలకం_Ex(h_కీలకం_బిడ్డ, 0, బఫరు, &కొలత, ఖాళీ,
			ఖాళీ, ఖాళీ, &సమయం) == S_OK)
    {
	// Delete the descendants of this child
	Recursive_Delete_కీలకం(h_కీలకం_బిడ్డ, బఫరు);
	కొలత = 256;
    }

    // Close the child
    Reg_Close_కీలకం(h_కీలకం_బిడ్డ);

    // Delete this child
    Reg_Delete_కీలకం(h_కీలకం_మాతృ, బిడ్డ);
}

// Create a key and set its value
స్థిర శూన్య సమితి_కీలకం_And_విలువ(స్థిరరాశి అక్షరం *కీలకం, స్థిరరాశి అక్షరం *subkey, స్థిరరాశి అక్షరం *విలువ)
{
    HKEY h_కీలకం;
    అక్షరం బఫరు[1024];

    strcpy(బఫరు, కీలకం);

    // Add subkey name to buffer.
    ఒకవేళ (subkey)
    {
	strcat(బఫరు, "\\");
	strcat(బఫరు, subkey);
    }

    // Create and open key and subkey.
    దీర్ఘ ఫలితం = Reg_Create_కీలకం_Ex(HKEY_CLASSES_మూలం,
				 బఫరు,
				 0, ఖాళీ, REG_OPTION_NON_VOLATILE,
				 కీలకం_ALL_ACCESS, ఖాళీ,
				 &h_కీలకం, ఖాళీ);
    ఒకవేళ (ఫలితం != ERROR_SUCCESS)
	తిరిగి;

    // Set the value
    ఒకవేళ (విలువ)
	Reg_సమితి_విలువ_Ex(h_కీలకం, ఖాళీ, 0, REG_SZ, (BYTE *)విలువ,
		      (DWORD)STRLEN(విలువ)+1);

    Reg_Close_కీలకం(h_కీలకం);
}

/*****************************************************************************
 5. OLE Initialisation and shutdown processing
*****************************************************************************/
బాహ్య "C" శూన్య InitOLE(పూర్ణాంకం *pbDoRestart)
{
    HRESULT hr;

    *pbDoRestart = FALSE;

    // Initialize the OLE libraries
    hr = OleInitialize(ఖాళీ);
    ఒకవేళ (FAILED(hr))
    {
	సందేశం_Box(0, "Cannot initialise OLE", "Vim Initialisation", 0);
	వెళ్ళు error0;
    }

    // Create the application object
    app = CVim::Create(pbDoRestart);
    ఒకవేళ (app == ఖాళీ)
	వెళ్ళు error1;

    // Create the class factory
    cf = CVimCF::Create();
    ఒకవేళ (cf == ఖాళీ)
	వెళ్ళు error1;

    // Register the class factory
    hr = Co_రిజిస్టరు_Class_Object(
	MYCLSID,
	cf,
	CLSCTX_LOCAL_SERVER,
	REGCLS_MULTIPLEUSE,
	&cf_id);

    ఒకవేళ (FAILED(hr))
    {
	సందేశం_Box(0, "Cannot register class factory", "Vim Initialisation", 0);
	వెళ్ళు error1;
    }

    // Register the application object as active
    hr = రిజిస్టరు_చురుకైన_Object(
	app,
	MYCLSID,
	0,
	&app_id);

    ఒకవేళ (FAILED(hr))
    {
	సందేశం_Box(0, "Cannot register application object", "Vim Initialisation", 0);
	వెళ్ళు error1;
    }

    తిరిగి;

    // Errors: tidy up as much as needed and return
error1:
    UninitOLE();
error0:
    తిరిగి;
}

బాహ్య "C" శూన్య UninitOLE()
{
    // Unregister the application object
    ఒకవేళ (app_id)
    {
	Revoke_చురుకైన_Object(app_id, ఖాళీ);
	app_id = 0;
    }

    // Unregister the class factory
    ఒకవేళ (cf_id)
    {
	CoRevokeClassObject(cf_id);
	cf_id = 0;
    }

    // Shut down the OLE libraries
    OleUninitialize();

    // Delete the application object
    ఒకవేళ (app)
    {
	తొలగించు app;
	app = ఖాళీ;
    }

    // Delete the class factory
    ఒకవేళ (cf)
    {
	తొలగించు cf;
	cf = ఖాళీ;
    }
}
#అయితే_ముగింపు /* FEAT_OLE */
