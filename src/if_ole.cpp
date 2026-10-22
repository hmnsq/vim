/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#ਜੇ defined(FEAT_OLE) && defined(FEAT_GUI_MSWIN)
/*
 * OLE server implementation.
 *
 * See os_mswin.c for the client side.
 */
ਬਾਹਰੀ "C" {
# include "vim.h"
}

#ਅਨ੍ਤਰ੍ਭਾਵ <windows.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <oleauto.h>

ਬਾਹਰੀ "C" {
ਬਾਹਰੀ HWND s_hwnd;
ਬਾਹਰੀ HWND vim_ਮਾਪਾ_hwnd;
}

#ਜੇ (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#ਨਹੀਂਤਾਂ
# define FINAL
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "if_ole.h"	// Interface definitions
#ਅਨ੍ਤਰ੍ਭਾਵ "iid_ole.c"	// UUID definitions (compile here)

/* Supply function prototype to work around bug in Mingw oleauto.h header */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __MINGW32__
WINOLEAUTAPI Un_ਰਜਿਸਟਰ_Type_Lib(REFGUID libID, ਸ਼ਬਦ_2 wVerMajor,
	    ਸ਼ਬਦ_2 wVerMinor, LCID lcid, SYSKIND syskind);
#ਜੇ_ਅੰਤ

/*****************************************************************************
 1. Internal definitions for this file
*****************************************************************************/

ਵਰਗ CVim;
ਵਰਗ CVimCF;

/* Internal data */
// The identifier of the registered class factory
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ cf_id = 0;

// The identifier of the running application object
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ app_id = 0;

// The single global instance of the class factory
ਸਥਿਰ CVimCF *cf = 0;

// The single global instance of the application object
ਸਥਿਰ CVim *app = 0;

/* GUIDs, versions and type library information */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MYCLSID CLSID_Vim
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MYLIBID LIBID_Vim
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MYIID IID_IVim

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAJORVER 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MINORVER 0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOCALE 0x0409

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MYNAME "Vim"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MYPROGID "Vim.Application.1"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MYVIPROGID "Vim.Application"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_CLSID_LEN 100

/*****************************************************************************
 2. The application object
*****************************************************************************/

/* Definition
 * ----------
 */

ਵਰਗ CVim FINAL : ਜਨਤਕ IVim
{
ਜਨਤਕ:
    ਆਭਾਸੀ ~CVim();
    ਸਥਿਰ CVim *Create(ਪੂਰਨ *pbDoRestart);

    // IUnknown members
    STDMETHOD(QueryInterface)(REFIID riid, ਖਾਲੀ ** ppv);
    STDMETHOD_(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ, ਜੋੜੋ_Ref)(ਖਾਲੀ);
    STDMETHOD_(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ, Release)(ਖਾਲੀ);

    // IDispatch members
    STDMETHOD(Get_Type_Info_ਗਿਣਤੀ)(UINT *p_ਗਿਣਤੀ);
    STDMETHOD(GetTypeInfo)(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo);
    STDMETHOD(Get_IDs_ਦਾ_ਨਾਂ)(ਅਚਲ IID &iid, OLECHAR **ਨਾਂ, UINT n, LCID, DISPID *dispids);
    STDMETHOD(Invoke)(DISPID member, ਅਚਲ IID &iid, LCID, ਸ਼ਬਦ_2 flags, DISPPARAMS *dispparams, VARIANT *ਨਤੀਜਾ, EXCEPINFO *excepinfo, UINT *argerr);

    // IVim members
    STDMETHOD(Send_ਕੁੰਜੀਆਂ)(BSTR ਕੁੰਜੀਆਂ);
    STDMETHOD(Eval)(BSTR ਵਿਅੰਜਕ, BSTR *ਨਤੀਜਾ);
    STDMETHOD(ਸੈੱਟ_Foreground)(ਖਾਲੀ);
    STDMETHOD(GetHwnd)(UINT_PTR *ਨਤੀਜਾ);

ਨਿਜੀ:
    // Constructor is private - create using CVim::Create()
    CVim() : ref(0), typeinfo(0) {};

    // Reference count
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ref;

    // The object's TypeInfo
    ITypeInfo *typeinfo;
};

/* Implementation
 * --------------
 */

CVim *CVim::Create(ਪੂਰਨ *pbDoRestart)
{
    HRESULT hr;
    CVim *me = 0;
    ITypeLib *typelib = 0;
    ITypeInfo *typeinfo = 0;

    *pbDoRestart = FALSE;

    // Create the object
    me = ਨਵਾਂ CVim();
    ਜੇ (me == ਨਿਰਾ)
    {
	ਸੁਨੇਹਾ_Box(0, "Cannot create application object", "Vim Initialisation", 0);
	ਵਾਪਸ ਨਿਰਾ;
    }

    // Load the type library from the registry
    hr = ਲੱਦੋ_Reg_Type_Lib(MYLIBID, 1, 0, 0x00, &typelib);
    ਜੇ (FAILED(hr))
    {
	HKEY h_ਕੁੰਜੀ;

	// Check we can write to the registry.
	// RegCreateKeyEx succeeds even if key exists. W.Briscoe W2K 20021011
	ਜੇ (Reg_Create_ਕੁੰਜੀ_Ex(HKEY_CLASSES_ਜੜ੍ਹ, MYVIPROGID, 0, ਨਿਰਾ,
		  REG_OPTION_NON_VOLATILE,
		  ਕੁੰਜੀ_ALL_ACCESS, ਨਿਰਾ, &h_ਕੁੰਜੀ, ਨਿਰਾ))
	{
	    ਮਿਟਾਓ me;
	    ਵਾਪਸ ਨਿਰਾ; // Unable to write to registry. Quietly fail.
	}
	Reg_Close_ਕੁੰਜੀ(h_ਕੁੰਜੀ);

	ਜੇ (ਸੁਨੇਹਾ_Box(0, "Cannot load registered type library.\nDo you want to register Vim now?",
		    "Vim Initialisation", MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
	    ਮਿਟਾਓ me;
	    ਵਾਪਸ ਨਿਰਾ;
	}

	ਰਜਿਸਟਰ_Me(FALSE);

	// Load the type library from the registry
	hr = ਲੱਦੋ_Reg_Type_Lib(MYLIBID, 1, 0, 0x00, &typelib);
	ਜੇ (FAILED(hr))
	{
	    ਸੁਨੇਹਾ_Box(0, "You must restart Vim in order for the registration to take effect.",
						     "Vim Initialisation", 0);
	    *pbDoRestart = TRUE;
	    ਮਿਟਾਓ me;
	    ਵਾਪਸ ਨਿਰਾ;
	}
    }

    // Get the type info of the vtable interface
    hr = typelib->Get_Type_Info_ਦਾ_Guid(MYIID, &typeinfo);
    typelib->Release();

    ਜੇ (FAILED(hr))
    {
	ਸੁਨੇਹਾ_Box(0, "Cannot get interface type information",
						     "Vim Initialisation", 0);
	ਮਿਟਾਓ me;
	ਵਾਪਸ ਨਿਰਾ;
    }

    // Save the type information
    me->typeinfo = typeinfo;
    ਵਾਪਸ me;
}

CVim::~CVim()
{
    ਜੇ (typeinfo && vim_ਮਾਪਾ_hwnd == ਨਿਰਾ)
	typeinfo->Release();
    typeinfo = 0;
}

STDMETHODIMP
CVim::QueryInterface(REFIID riid, ਖਾਲੀ **ppv)
{
    ਜੇ (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDispatch) || IsEqualIID(riid, MYIID))
    {
	ਜੋੜੋ_Ref();
	*ppv = ਇਹ;
	ਵਾਪਸ S_OK;
    }

    *ppv = 0;
    ਵਾਪਸ E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVim::ਜੋੜੋ_Ref()
{
    ਵਾਪਸ ++ref;
}

STDMETHODIMP_(ULONG)
CVim::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    ਜੇ (ref > 0)
	--ref;
    ਵਾਪਸ ref;
}

STDMETHODIMP
CVim::Get_Type_Info_ਗਿਣਤੀ(UINT *p_ਗਿਣਤੀ)
{
    *p_ਗਿਣਤੀ = 1;
    ਵਾਪਸ S_OK;
}

STDMETHODIMP
CVim::GetTypeInfo(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo)
{
    *ppITypeInfo = 0;

    ਜੇ (iTypeInfo != 0)
	ਵਾਪਸ DISP_E_BADINDEX;

    typeinfo->ਜੋੜੋ_Ref();
    *ppITypeInfo = typeinfo;
    ਵਾਪਸ S_OK;
}

STDMETHODIMP
CVim::Get_IDs_ਦਾ_ਨਾਂ(
	ਅਚਲ IID &iid,
	OLECHAR **ਨਾਂ,
	UINT n,
	LCID,
	DISPID *dispids)
{
    ਜੇ (iid != IID_NULL)
	ਵਾਪਸ DISP_E_UNKNOWNINTERFACE;

    ਵਾਪਸ typeinfo->Get_IDs_ਦਾ_ਨਾਂ(ਨਾਂ, n, dispids);
}

STDMETHODIMP
CVim::Invoke(
	DISPID member,
	ਅਚਲ IID &iid,
	LCID,
	ਸ਼ਬਦ_2 flags,
	DISPPARAMS *dispparams,
	VARIANT *ਨਤੀਜਾ,
	EXCEPINFO *excepinfo,
	UINT *argerr)
{
    ਜੇ (iid != IID_NULL)
	ਵਾਪਸ DISP_E_UNKNOWNINTERFACE;

    ::ਸੈੱਟ_Error_Info(0, ਨਿਰਾ);
    ਵਾਪਸ typeinfo->Invoke(static_cast<IDispatch*>(ਇਹ),
			    member, flags, dispparams,
			    ਨਤੀਜਾ, excepinfo, argerr);
}

STDMETHODIMP
CVim::GetHwnd(UINT_PTR *ਨਤੀਜਾ)
{
    *ਨਤੀਜਾ = (UINT_PTR)s_hwnd;
    ਵਾਪਸ S_OK;
}

STDMETHODIMP
CVim::ਸੈੱਟ_Foreground(ਖਾਲੀ)
{
    /* Make the Vim window come to the foreground */
    gui_mch_ਸੈੱਟ_foreground();
    ਵਾਪਸ S_OK;
}

STDMETHODIMP
CVim::Send_ਕੁੰਜੀਆਂ(BSTR ਕੁੰਜੀਆਂ)
{
    ਪੂਰਨ len;
    ਅੱਖਰ *ਬਫਰ;
    char_u *str;
    char_u *ptr;

    /* Get a suitable buffer */
    len = Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte(CP_ACP, 0, ਕੁੰਜੀਆਂ, -1, 0, 0, 0, 0);
    ਬਫਰ = (ਅੱਖਰ *)alloc(len+1);

    ਜੇ (ਬਫਰ == ਨਿਰਾ)
	ਵਾਪਸ E_OUTOFMEMORY;

    len = Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte(CP_ACP, 0, ਕੁੰਜੀਆਂ, -1, ਬਫਰ, len, 0, 0);

    ਜੇ (len == 0)
    {
	vim_free(ਬਫਰ);
	ਵਾਪਸ E_INVALIDARG;
    }

    /* Translate key codes like <Esc> */
    str = replace_termcodes((char_u *)ਬਫਰ, &ptr, 0, REPTERM_DO_LT, ਨਿਰਾ);

    /* If ptr was set, then a new buffer was allocated,
     * so we can free the old one.
     */
    ਜੇ (ptr)
	vim_free((char_u *)(ਬਫਰ));

    /* Reject strings too long to fit in the input buffer. Allow 10 bytes
     * space to cover for the (remote) possibility that characters may enter
     * the input buffer between now and when the WM_OLE message is actually
     * processed. If more than 10 characters enter the input buffer in that
     * time, the WM_OLE processing will simply fail to insert the characters.
     */
    ਜੇ ((ਪੂਰਨ)(STRLEN(str)) > (vim_free_ਵਿੱਚ_ਨਿਵੇਸ਼_buf() - 10))
    {
	vim_free(str);
	ਵਾਪਸ E_INVALIDARG;
    }

    /* Pass the string to the main input loop. The memory will be freed when
     * the message is processed.  Except for an empty message, we don't need
     * to post it then.
     */
    ਜੇ (*str == NUL)
	vim_free(str);
    ਨਹੀਂਤਾਂ
	Post_ਸੁਨੇਹਾ(ਨਿਰਾ, WM_OLE, 0, (LPARAM)str);

    ਵਾਪਸ S_OK;
}

STDMETHODIMP
CVim::Eval(BSTR ਵਿਅੰਜਕ, BSTR *ਨਤੀਜਾ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਪੂਰਨ len;
    ਅੱਖਰ *ਬਫਰ;
    ਅੱਖਰ *str;
    wchar_t *w_ਬਫਰ;

    /* Get a suitable buffer */
    len = Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte(CP_ACP, 0, ਵਿਅੰਜਕ, -1, 0, 0, 0, 0);
    ਜੇ (len == 0)
	ਵਾਪਸ E_INVALIDARG;

    ਬਫਰ = (ਅੱਖਰ *)alloc(len);

    ਜੇ (ਬਫਰ == ਨਿਰਾ)
	ਵਾਪਸ E_OUTOFMEMORY;

    /* Convert the (wide character) expression to an ASCII string */
    len = Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte(CP_ACP, 0, ਵਿਅੰਜਕ, -1, ਬਫਰ, len, 0, 0);
    ਜੇ (len == 0)
	ਵਾਪਸ E_INVALIDARG;

    /* Evaluate the expression */
    ++emsg_skip;
    str = (ਅੱਖਰ *)eval_ਮੰਜ਼ਿਲ_string((char_u *)ਬਫਰ, TRUE, FALSE);
    --emsg_skip;
    vim_free(ਬਫਰ);
    ਜੇ (str == ਨਿਰਾ)
	ਵਾਪਸ E_FAIL;

    /* Convert the result to wide characters */
    Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char_alloc(CP_ACP, 0, str, -1, &w_ਬਫਰ, &len);
    vim_free(str);
    ਜੇ (w_ਬਫਰ == ਨਿਰਾ)
	ਵਾਪਸ E_OUTOFMEMORY;

    ਜੇ (len == 0)
    {
	vim_free(w_ਬਫਰ);
	ਵਾਪਸ E_FAIL;
    }

    /* Store the result */
    *ਨਤੀਜਾ = SysAllocString(w_ਬਫਰ);
    vim_free(w_ਬਫਰ);

    ਵਾਪਸ S_OK;
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ E_NOTIMPL;
#ਜੇ_ਅੰਤ
}

/*****************************************************************************
 3. The class factory
*****************************************************************************/

/* Definition
 * ----------
 */

ਵਰਗ CVimCF FINAL : ਜਨਤਕ IClassFactory
{
ਜਨਤਕ:
    ਸਥਿਰ CVimCF *Create();
    ਆਭਾਸੀ ~CVimCF() {};

    STDMETHOD(QueryInterface)(REFIID riid, ਖਾਲੀ ** ppv);
    STDMETHOD_(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ, ਜੋੜੋ_Ref)(ਖਾਲੀ);
    STDMETHOD_(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ, Release)(ਖਾਲੀ);
    STDMETHOD(CreateInstance)(IUnknown *punkOuter, REFIID riid, ਖਾਲੀ ** ppv);
    STDMETHOD(ਤਾਲਾ_Server)(BOOL ਤਾਲਾ);

ਨਿਜੀ:
    // Constructor is private - create via Create()
    CVimCF() : ref(0) {};

    // Reference count
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ref;
};

/* Implementation
 * --------------
 */

CVimCF *CVimCF::Create()
{
    CVimCF *me = ਨਵਾਂ CVimCF();

    ਜੇ (me == ਨਿਰਾ)
	ਸੁਨੇਹਾ_Box(0, "Cannot create class factory", "Vim Initialisation", 0);

    ਵਾਪਸ me;
}

STDMETHODIMP
CVimCF::QueryInterface(REFIID riid, ਖਾਲੀ **ppv)
{
    ਜੇ (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	ਜੋੜੋ_Ref();
	*ppv = ਇਹ;
	ਵਾਪਸ S_OK;
    }

    *ppv = 0;
    ਵਾਪਸ E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVimCF::ਜੋੜੋ_Ref()
{
    ਵਾਪਸ ++ref;
}

STDMETHODIMP_(ULONG)
CVimCF::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    ਜੇ (ref > 0)
	--ref;
    ਵਾਪਸ ref;
}

/*ARGSUSED*/
STDMETHODIMP
CVimCF::CreateInstance(IUnknown *punkOuter, REFIID riid, ਖਾਲੀ **ppv)
{
    ਵਾਪਸ app->QueryInterface(riid, ppv);
}

/*ARGSUSED*/
STDMETHODIMP
CVimCF::ਤਾਲਾ_Server(BOOL ਤਾਲਾ)
{
    ਵਾਪਸ S_OK;
}

/*****************************************************************************
 4. Registry manipulation code
*****************************************************************************/

// Internal use only
ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_ਕੁੰਜੀ_And_ਮੁੱਲ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *subkey, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ);
ਸਥਿਰ ਖਾਲੀ GUIDtochar(ਅਚਲ GUID &guid, ਅੱਖਰ *GUID, ਪੂਰਨ ਲੰਮਾਈ);
ਸਥਿਰ ਖਾਲੀ Recursive_Delete_ਕੁੰਜੀ(HKEY h_ਕੁੰਜੀ_ਮਾਪਾ, ਅਚਲ ਅੱਖਰ *ਬੱਚਾ);
ਸਥਿਰ ਅਚਲ ਪੂਰਨ GUID_STRING_ਮਾਪ = 39;

// Register the component in the registry
// When "silent" is TRUE don't give any messages.

ਬਾਹਰੀ "C" ਖਾਲੀ ਰਜਿਸਟਰ_Me(ਪੂਰਨ silent)
{
    BOOL ok = TRUE;

    // Get the application startup command
    ਅੱਖਰ module[MAX_ਰਸਤਾ];

    ::GetModuleFileName(ਨਿਰਾ, module, MAX_ਰਸਤਾ);

    // Unregister first (quietly)
    UnregisterMe(FALSE);

    // Convert the CLSID into a char
    ਅੱਖਰ clsid[GUID_STRING_ਮਾਪ];
    GUIDtochar(MYCLSID, clsid, ਆਕਾਰ(clsid));

    // Convert the LIBID into a char
    ਅੱਖਰ libid[GUID_STRING_ਮਾਪ];
    GUIDtochar(MYLIBID, libid, ਆਕਾਰ(libid));

    // Build the key CLSID\\{...}
    ਅੱਖਰ ਕੁੰਜੀ_2[MAX_CLSID_LEN];
    strcpy(ਕੁੰਜੀ_2, "CLSID\\");
    strcat(ਕੁੰਜੀ_2, clsid);

    // Add the CLSID to the registry
    ਸੈੱਟ_ਕੁੰਜੀ_And_ਮੁੱਲ(ਕੁੰਜੀ_2, ਨਿਰਾ, MYNAME);
    ਸੈੱਟ_ਕੁੰਜੀ_And_ਮੁੱਲ(ਕੁੰਜੀ_2, "LocalServer32", module);
    ਸੈੱਟ_ਕੁੰਜੀ_And_ਮੁੱਲ(ਕੁੰਜੀ_2, "ProgID", MYPROGID);
    ਸੈੱਟ_ਕੁੰਜੀ_And_ਮੁੱਲ(ਕੁੰਜੀ_2, "VersionIndependentProgID", MYVIPROGID);
    ਸੈੱਟ_ਕੁੰਜੀ_And_ਮੁੱਲ(ਕੁੰਜੀ_2, "TypeLib", libid);

    // Add the version-independent ProgID subkey under HKEY_CLASSES_ROOT
    ਸੈੱਟ_ਕੁੰਜੀ_And_ਮੁੱਲ(MYVIPROGID, ਨਿਰਾ, MYNAME);
    ਸੈੱਟ_ਕੁੰਜੀ_And_ਮੁੱਲ(MYVIPROGID, "CLSID", clsid);
    ਸੈੱਟ_ਕੁੰਜੀ_And_ਮੁੱਲ(MYVIPROGID, "CurVer", MYPROGID);

    // Add the versioned ProgID subkey under HKEY_CLASSES_ROOT
    ਸੈੱਟ_ਕੁੰਜੀ_And_ਮੁੱਲ(MYPROGID, ਨਿਰਾ, MYNAME);
    ਸੈੱਟ_ਕੁੰਜੀ_And_ਮੁੱਲ(MYPROGID, "CLSID", clsid);

    wchar_t w_module[MAX_ਰਸਤਾ];
    Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char(CP_ACP, 0, module, -1, w_module, MAX_ਰਸਤਾ);

    ITypeLib *typelib = ਨਿਰਾ;
    ਜੇ (ਲੱਦੋ_Type_Lib(w_module, &typelib) != S_OK)
    {
	ਜੇ (!silent)
	    ਸੁਨੇਹਾ_Box(0, "Cannot load type library to register",
						       "Vim Registration", 0);
	ok = FALSE;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (ਰਜਿਸਟਰ_Type_Lib(typelib, w_module, ਨਿਰਾ) != S_OK)
	{
	    ਜੇ (!silent)
		ਸੁਨੇਹਾ_Box(0, "Cannot register type library",
						       "Vim Registration", 0);
	    ok = FALSE;
	}
	typelib->Release();
    }

    ਜੇ (ok && !silent)
	ਸੁਨੇਹਾ_Box(0, "Registered successfully", "Vim", 0);
}

// Remove the component from the registry
//
// Note: There is little error checking in this code, to allow incomplete
// or failed registrations to be undone.
ਬਾਹਰੀ "C" ਖਾਲੀ UnregisterMe(ਪੂਰਨ bNotifyUser)
{
    // Unregister the type library
    ITypeLib *typelib;
    ਜੇ (SUCCEEDED(ਲੱਦੋ_Reg_Type_Lib(MYLIBID, MAJORVER, MINORVER, LOCALE, &typelib)))
    {
	TLIBATTR *tla;
	ਜੇ (SUCCEEDED(typelib->GetLibAttr(&tla)))
	{
	    Un_ਰਜਿਸਟਰ_Type_Lib(tla->guid, tla->w_Major_Ver_ਅੰਕ, tla->w_Minor_Ver_ਅੰਕ,
			      tla->lcid, tla->syskind);
	    typelib->ReleaseTLibAttr(tla);
	}
	typelib->Release();
    }

    // Convert the CLSID into a char
    ਅੱਖਰ clsid[GUID_STRING_ਮਾਪ];
    GUIDtochar(MYCLSID, clsid, ਆਕਾਰ(clsid));

    // Build the key CLSID\\{...}
    ਅੱਖਰ ਕੁੰਜੀ_2[MAX_CLSID_LEN];
    strcpy(ਕੁੰਜੀ_2, "CLSID\\");
    strcat(ਕੁੰਜੀ_2, clsid);

    // Delete the CLSID Key - CLSID\{...}
    Recursive_Delete_ਕੁੰਜੀ(HKEY_CLASSES_ਜੜ੍ਹ, ਕੁੰਜੀ_2);

    // Delete the version-independent ProgID Key
    Recursive_Delete_ਕੁੰਜੀ(HKEY_CLASSES_ਜੜ੍ਹ, MYVIPROGID);

    // Delete the ProgID key
    Recursive_Delete_ਕੁੰਜੀ(HKEY_CLASSES_ਜੜ੍ਹ, MYPROGID);

    ਜੇ (bNotifyUser)
	ਸੁਨੇਹਾ_Box(0, "Unregistered successfully", "Vim", 0);
}

/****************************************************************************/

// Convert a GUID to a char string
ਸਥਿਰ ਖਾਲੀ GUIDtochar(ਅਚਲ GUID &guid, ਅੱਖਰ *GUID, ਪੂਰਨ ਲੰਮਾਈ)
{
    // Get wide string version
    LPOLESTR wGUID = ਨਿਰਾ;
    StringFromCLSID(guid, &wGUID);

    // Convert from wide characters to non-wide
    wcstombs(GUID, wGUID, ਲੰਮਾਈ);

    // Free memory
    CoTaskMemFree(wGUID);
}

// Delete a key and all of its descendants
ਸਥਿਰ ਖਾਲੀ Recursive_Delete_ਕੁੰਜੀ(HKEY h_ਕੁੰਜੀ_ਮਾਪਾ, ਅਚਲ ਅੱਖਰ *ਬੱਚਾ)
{
    // Open the child
    HKEY h_ਕੁੰਜੀ_ਬੱਚਾ;
    LONG ਨਤੀਜਾ = Reg_Open_ਕੁੰਜੀ_Ex(h_ਕੁੰਜੀ_ਮਾਪਾ, ਬੱਚਾ, 0,
			       ਕੁੰਜੀ_ALL_ACCESS, &h_ਕੁੰਜੀ_ਬੱਚਾ);
    ਜੇ (ਨਤੀਜਾ != ERROR_SUCCESS)
	ਵਾਪਸ;

    // Enumerate all of the descendants of this child
    FILETIME ਸਮਾਂ;
    ਅੱਖਰ ਬਫਰ[1024];
    DWORD ਮਾਪ = 1024;

    ਜਦੋਂ (Reg_Enum_ਕੁੰਜੀ_Ex(h_ਕੁੰਜੀ_ਬੱਚਾ, 0, ਬਫਰ, &ਮਾਪ, ਨਿਰਾ,
			ਨਿਰਾ, ਨਿਰਾ, &ਸਮਾਂ) == S_OK)
    {
	// Delete the descendants of this child
	Recursive_Delete_ਕੁੰਜੀ(h_ਕੁੰਜੀ_ਬੱਚਾ, ਬਫਰ);
	ਮਾਪ = 256;
    }

    // Close the child
    Reg_Close_ਕੁੰਜੀ(h_ਕੁੰਜੀ_ਬੱਚਾ);

    // Delete this child
    Reg_Delete_ਕੁੰਜੀ(h_ਕੁੰਜੀ_ਮਾਪਾ, ਬੱਚਾ);
}

// Create a key and set its value
ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_ਕੁੰਜੀ_And_ਮੁੱਲ(ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ *subkey, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ)
{
    HKEY h_ਕੁੰਜੀ;
    ਅੱਖਰ ਬਫਰ[1024];

    strcpy(ਬਫਰ, ਕੁੰਜੀ);

    // Add subkey name to buffer.
    ਜੇ (subkey)
    {
	strcat(ਬਫਰ, "\\");
	strcat(ਬਫਰ, subkey);
    }

    // Create and open key and subkey.
    ਲੰਮਾ ਨਤੀਜਾ = Reg_Create_ਕੁੰਜੀ_Ex(HKEY_CLASSES_ਜੜ੍ਹ,
				 ਬਫਰ,
				 0, ਨਿਰਾ, REG_OPTION_NON_VOLATILE,
				 ਕੁੰਜੀ_ALL_ACCESS, ਨਿਰਾ,
				 &h_ਕੁੰਜੀ, ਨਿਰਾ);
    ਜੇ (ਨਤੀਜਾ != ERROR_SUCCESS)
	ਵਾਪਸ;

    // Set the value
    ਜੇ (ਮੁੱਲ)
	Reg_ਸੈੱਟ_ਮੁੱਲ_Ex(h_ਕੁੰਜੀ, ਨਿਰਾ, 0, REG_SZ, (BYTE *)ਮੁੱਲ,
		      (DWORD)STRLEN(ਮੁੱਲ)+1);

    Reg_Close_ਕੁੰਜੀ(h_ਕੁੰਜੀ);
}

/*****************************************************************************
 5. OLE Initialisation and shutdown processing
*****************************************************************************/
ਬਾਹਰੀ "C" ਖਾਲੀ InitOLE(ਪੂਰਨ *pbDoRestart)
{
    HRESULT hr;

    *pbDoRestart = FALSE;

    // Initialize the OLE libraries
    hr = OleInitialize(ਨਿਰਾ);
    ਜੇ (FAILED(hr))
    {
	ਸੁਨੇਹਾ_Box(0, "Cannot initialise OLE", "Vim Initialisation", 0);
	ਜਾਓ error0;
    }

    // Create the application object
    app = CVim::Create(pbDoRestart);
    ਜੇ (app == ਨਿਰਾ)
	ਜਾਓ error1;

    // Create the class factory
    cf = CVimCF::Create();
    ਜੇ (cf == ਨਿਰਾ)
	ਜਾਓ error1;

    // Register the class factory
    hr = Co_ਰਜਿਸਟਰ_Class_Object(
	MYCLSID,
	cf,
	CLSCTX_LOCAL_SERVER,
	REGCLS_MULTIPLEUSE,
	&cf_id);

    ਜੇ (FAILED(hr))
    {
	ਸੁਨੇਹਾ_Box(0, "Cannot register class factory", "Vim Initialisation", 0);
	ਜਾਓ error1;
    }

    // Register the application object as active
    hr = ਰਜਿਸਟਰ_ਸਰਗਰਮ_Object(
	app,
	MYCLSID,
	0,
	&app_id);

    ਜੇ (FAILED(hr))
    {
	ਸੁਨੇਹਾ_Box(0, "Cannot register application object", "Vim Initialisation", 0);
	ਜਾਓ error1;
    }

    ਵਾਪਸ;

    // Errors: tidy up as much as needed and return
error1:
    UninitOLE();
error0:
    ਵਾਪਸ;
}

ਬਾਹਰੀ "C" ਖਾਲੀ UninitOLE()
{
    // Unregister the application object
    ਜੇ (app_id)
    {
	Revoke_ਸਰਗਰਮ_Object(app_id, ਨਿਰਾ);
	app_id = 0;
    }

    // Unregister the class factory
    ਜੇ (cf_id)
    {
	CoRevokeClassObject(cf_id);
	cf_id = 0;
    }

    // Shut down the OLE libraries
    OleUninitialize();

    // Delete the application object
    ਜੇ (app)
    {
	ਮਿਟਾਓ app;
	app = ਨਿਰਾ;
    }

    // Delete the class factory
    ਜੇ (cf)
    {
	ਮਿਟਾਓ cf;
	cf = ਨਿਰਾ;
    }
}
#ਜੇ_ਅੰਤ /* FEAT_OLE */
