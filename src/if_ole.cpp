/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#ಆದರೆ defined(FEAT_OLE) && defined(FEAT_GUI_MSWIN)
/*
 * OLE server implementation.
 *
 * See os_mswin.c for the client side.
 */
ಬಾಹ್ಯ "C" {
# include "vim.h"
}

#ಅನ್ತರ್ಭಾವ <windows.h>
#ಅನ್ತರ್ಭಾವ <oleauto.h>

ಬಾಹ್ಯ "C" {
ಬಾಹ್ಯ HWND s_hwnd;
ಬಾಹ್ಯ HWND vim_ಪೋಷಕ_hwnd;
}

#ಆದರೆ (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#ಇಲ್ಲದಿದ್ದರೆ
# define FINAL
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "if_ole.h"	// Interface definitions
#ಅನ್ತರ್ಭಾವ "iid_ole.c"	// UUID definitions (compile here)

/* Supply function prototype to work around bug in Mingw oleauto.h header */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __MINGW32__
WINOLEAUTAPI Un_ರಿಜಿಸ್ಟರ್_Type_Lib(REFGUID libID, ಶಬ್ದ_2 wVerMajor,
	    ಶಬ್ದ_2 wVerMinor, LCID lcid, SYSKIND syskind);
#ಆದರೆ_ಅಂತ್ಯ

/*****************************************************************************
 1. Internal definitions for this file
*****************************************************************************/

ವರ್ಗ CVim;
ವರ್ಗ CVimCF;

/* Internal data */
// The identifier of the registered class factory
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ cf_id = 0;

// The identifier of the running application object
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ app_id = 0;

// The single global instance of the class factory
ಸ್ಥಿರ CVimCF *cf = 0;

// The single global instance of the application object
ಸ್ಥಿರ CVim *app = 0;

/* GUIDs, versions and type library information */
#ವ್ಯಾಖ್ಯಾನಿಸು MYCLSID CLSID_Vim
#ವ್ಯಾಖ್ಯಾನಿಸು MYLIBID LIBID_Vim
#ವ್ಯಾಖ್ಯಾನಿಸು MYIID IID_IVim

#ವ್ಯಾಖ್ಯಾನಿಸು MAJORVER 1
#ವ್ಯಾಖ್ಯಾನಿಸು MINORVER 0
#ವ್ಯಾಖ್ಯಾನಿಸು LOCALE 0x0409

#ವ್ಯಾಖ್ಯಾನಿಸು MYNAME "Vim"
#ವ್ಯಾಖ್ಯಾನಿಸು MYPROGID "Vim.Application.1"
#ವ್ಯಾಖ್ಯಾನಿಸು MYVIPROGID "Vim.Application"

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_CLSID_LEN 100

/*****************************************************************************
 2. The application object
*****************************************************************************/

/* Definition
 * ----------
 */

ವರ್ಗ CVim FINAL : ಸಾರ್ವಜನಿಕ IVim
{
ಸಾರ್ವಜನಿಕ:
    ವಾಸ್ತವಿಕ ~CVim();
    ಸ್ಥಿರ CVim *Create(ಪೂರ್ಣಾಂಕ *pbDoRestart);

    // IUnknown members
    STDMETHOD(QueryInterface)(REFIID riid, ಶೂನ್ಯ ** ppv);
    STDMETHOD_(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ, ಕೂಡಿಸು_Ref)(ಶೂನ್ಯ);
    STDMETHOD_(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ, Release)(ಶೂನ್ಯ);

    // IDispatch members
    STDMETHOD(Get_Type_Info_ಎಣಿಕೆ)(UINT *p_ಎಣಿಕೆ);
    STDMETHOD(GetTypeInfo)(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo);
    STDMETHOD(Get_IDs_ದ_ಹೆಸರುಗಳು)(ಸ್ಥಿರಾಂಕ IID &iid, OLECHAR **ಹೆಸರುಗಳು, UINT n, LCID, DISPID *dispids);
    STDMETHOD(Invoke)(DISPID member, ಸ್ಥಿರಾಂಕ IID &iid, LCID, ಶಬ್ದ_2 flags, DISPPARAMS *dispparams, VARIANT *ಫಲಿತಾಂಶ, EXCEPINFO *excepinfo, UINT *argerr);

    // IVim members
    STDMETHOD(Send_ಕೀಲಿಗಳು)(BSTR ಕೀಲಿಗಳು);
    STDMETHOD(Eval)(BSTR ಅಭಿವ್ಯಕ್ತಿ, BSTR *ಫಲಿತಾಂಶ);
    STDMETHOD(ಗಣ_Foreground)(ಶೂನ್ಯ);
    STDMETHOD(GetHwnd)(UINT_PTR *ಫಲಿತಾಂಶ);

ಖಾಸಗಿ:
    // Constructor is private - create using CVim::Create()
    CVim() : ref(0), typeinfo(0) {};

    // Reference count
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ref;

    // The object's TypeInfo
    ITypeInfo *typeinfo;
};

/* Implementation
 * --------------
 */

CVim *CVim::Create(ಪೂರ್ಣಾಂಕ *pbDoRestart)
{
    HRESULT hr;
    CVim *me = 0;
    ITypeLib *typelib = 0;
    ITypeInfo *typeinfo = 0;

    *pbDoRestart = FALSE;

    // Create the object
    me = ಹೊಸ CVim();
    ಆದರೆ (me == ಖಾಲಿ)
    {
	ಸಂದೇಶ_Box(0, "Cannot create application object", "Vim Initialisation", 0);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // Load the type library from the registry
    hr = ತುಂಬು_Reg_Type_Lib(MYLIBID, 1, 0, 0x00, &typelib);
    ಆದರೆ (FAILED(hr))
    {
	HKEY h_ಕೀಲಿ;

	// Check we can write to the registry.
	// RegCreateKeyEx succeeds even if key exists. W.Briscoe W2K 20021011
	ಆದರೆ (Reg_Create_ಕೀಲಿ_Ex(HKEY_CLASSES_ಬೇರು, MYVIPROGID, 0, ಖಾಲಿ,
		  REG_OPTION_NON_VOLATILE,
		  ಕೀಲಿ_ALL_ACCESS, ಖಾಲಿ, &h_ಕೀಲಿ, ಖಾಲಿ))
	{
	    ಅಳಿಸು me;
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ; // Unable to write to registry. Quietly fail.
	}
	Reg_Close_ಕೀಲಿ(h_ಕೀಲಿ);

	ಆದರೆ (ಸಂದೇಶ_Box(0, "Cannot load registered type library.\nDo you want to register Vim now?",
		    "Vim Initialisation", MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
	    ಅಳಿಸು me;
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	ರಿಜಿಸ್ಟರ್_Me(FALSE);

	// Load the type library from the registry
	hr = ತುಂಬು_Reg_Type_Lib(MYLIBID, 1, 0, 0x00, &typelib);
	ಆದರೆ (FAILED(hr))
	{
	    ಸಂದೇಶ_Box(0, "You must restart Vim in order for the registration to take effect.",
						     "Vim Initialisation", 0);
	    *pbDoRestart = TRUE;
	    ಅಳಿಸು me;
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
    }

    // Get the type info of the vtable interface
    hr = typelib->Get_Type_Info_ದ_Guid(MYIID, &typeinfo);
    typelib->Release();

    ಆದರೆ (FAILED(hr))
    {
	ಸಂದೇಶ_Box(0, "Cannot get interface type information",
						     "Vim Initialisation", 0);
	ಅಳಿಸು me;
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // Save the type information
    me->typeinfo = typeinfo;
    ಹಿಂತಿರುಗಿ me;
}

CVim::~CVim()
{
    ಆದರೆ (typeinfo && vim_ಪೋಷಕ_hwnd == ಖಾಲಿ)
	typeinfo->Release();
    typeinfo = 0;
}

STDMETHODIMP
CVim::QueryInterface(REFIID riid, ಶೂನ್ಯ **ppv)
{
    ಆದರೆ (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDispatch) || IsEqualIID(riid, MYIID))
    {
	ಕೂಡಿಸು_Ref();
	*ppv = ಇದು;
	ಹಿಂತಿರುಗಿ S_OK;
    }

    *ppv = 0;
    ಹಿಂತಿರುಗಿ E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVim::ಕೂಡಿಸು_Ref()
{
    ಹಿಂತಿರುಗಿ ++ref;
}

STDMETHODIMP_(ULONG)
CVim::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    ಆದರೆ (ref > 0)
	--ref;
    ಹಿಂತಿರುಗಿ ref;
}

STDMETHODIMP
CVim::Get_Type_Info_ಎಣಿಕೆ(UINT *p_ಎಣಿಕೆ)
{
    *p_ಎಣಿಕೆ = 1;
    ಹಿಂತಿರುಗಿ S_OK;
}

STDMETHODIMP
CVim::GetTypeInfo(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo)
{
    *ppITypeInfo = 0;

    ಆದರೆ (iTypeInfo != 0)
	ಹಿಂತಿರುಗಿ DISP_E_BADINDEX;

    typeinfo->ಕೂಡಿಸು_Ref();
    *ppITypeInfo = typeinfo;
    ಹಿಂತಿರುಗಿ S_OK;
}

STDMETHODIMP
CVim::Get_IDs_ದ_ಹೆಸರುಗಳು(
	ಸ್ಥಿರಾಂಕ IID &iid,
	OLECHAR **ಹೆಸರುಗಳು,
	UINT n,
	LCID,
	DISPID *dispids)
{
    ಆದರೆ (iid != IID_NULL)
	ಹಿಂತಿರುಗಿ DISP_E_UNKNOWNINTERFACE;

    ಹಿಂತಿರುಗಿ typeinfo->Get_IDs_ದ_ಹೆಸರುಗಳು(ಹೆಸರುಗಳು, n, dispids);
}

STDMETHODIMP
CVim::Invoke(
	DISPID member,
	ಸ್ಥಿರಾಂಕ IID &iid,
	LCID,
	ಶಬ್ದ_2 flags,
	DISPPARAMS *dispparams,
	VARIANT *ಫಲಿತಾಂಶ,
	EXCEPINFO *excepinfo,
	UINT *argerr)
{
    ಆದರೆ (iid != IID_NULL)
	ಹಿಂತಿರುಗಿ DISP_E_UNKNOWNINTERFACE;

    ::ಗಣ_Error_Info(0, ಖಾಲಿ);
    ಹಿಂತಿರುಗಿ typeinfo->Invoke(static_cast<IDispatch*>(ಇದು),
			    member, flags, dispparams,
			    ಫಲಿತಾಂಶ, excepinfo, argerr);
}

STDMETHODIMP
CVim::GetHwnd(UINT_PTR *ಫಲಿತಾಂಶ)
{
    *ಫಲಿತಾಂಶ = (UINT_PTR)s_hwnd;
    ಹಿಂತಿರುಗಿ S_OK;
}

STDMETHODIMP
CVim::ಗಣ_Foreground(ಶೂನ್ಯ)
{
    /* Make the Vim window come to the foreground */
    gui_mch_ಗಣ_foreground();
    ಹಿಂತಿರುಗಿ S_OK;
}

STDMETHODIMP
CVim::Send_ಕೀಲಿಗಳು(BSTR ಕೀಲಿಗಳು)
{
    ಪೂರ್ಣಾಂಕ len;
    ಅಕ್ಷರ *ಬಫರ್;
    char_u *str;
    char_u *ptr;

    /* Get a suitable buffer */
    len = Wide_Char_ಗಮ್ಯ_Multi_Byte(CP_ACP, 0, ಕೀಲಿಗಳು, -1, 0, 0, 0, 0);
    ಬಫರ್ = (ಅಕ್ಷರ *)alloc(len+1);

    ಆದರೆ (ಬಫರ್ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ E_OUTOFMEMORY;

    len = Wide_Char_ಗಮ್ಯ_Multi_Byte(CP_ACP, 0, ಕೀಲಿಗಳು, -1, ಬಫರ್, len, 0, 0);

    ಆದರೆ (len == 0)
    {
	vim_free(ಬಫರ್);
	ಹಿಂತಿರುಗಿ E_INVALIDARG;
    }

    /* Translate key codes like <Esc> */
    str = replace_termcodes((char_u *)ಬಫರ್, &ptr, 0, REPTERM_DO_LT, ಖಾಲಿ);

    /* If ptr was set, then a new buffer was allocated,
     * so we can free the old one.
     */
    ಆದರೆ (ptr)
	vim_free((char_u *)(ಬಫರ್));

    /* Reject strings too long to fit in the input buffer. Allow 10 bytes
     * space to cover for the (remote) possibility that characters may enter
     * the input buffer between now and when the WM_OLE message is actually
     * processed. If more than 10 characters enter the input buffer in that
     * time, the WM_OLE processing will simply fail to insert the characters.
     */
    ಆದರೆ ((ಪೂರ್ಣಾಂಕ)(STRLEN(str)) > (vim_free_ಒಳ_ಒಳಹರಿವು_buf() - 10))
    {
	vim_free(str);
	ಹಿಂತಿರುಗಿ E_INVALIDARG;
    }

    /* Pass the string to the main input loop. The memory will be freed when
     * the message is processed.  Except for an empty message, we don't need
     * to post it then.
     */
    ಆದರೆ (*str == NUL)
	vim_free(str);
    ಇಲ್ಲದಿದ್ದರೆ
	Post_ಸಂದೇಶ(ಖಾಲಿ, WM_OLE, 0, (LPARAM)str);

    ಹಿಂತಿರುಗಿ S_OK;
}

STDMETHODIMP
CVim::Eval(BSTR ಅಭಿವ್ಯಕ್ತಿ, BSTR *ಫಲಿತಾಂಶ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಪೂರ್ಣಾಂಕ len;
    ಅಕ್ಷರ *ಬಫರ್;
    ಅಕ್ಷರ *str;
    wchar_t *w_ಬಫರ್;

    /* Get a suitable buffer */
    len = Wide_Char_ಗಮ್ಯ_Multi_Byte(CP_ACP, 0, ಅಭಿವ್ಯಕ್ತಿ, -1, 0, 0, 0, 0);
    ಆದರೆ (len == 0)
	ಹಿಂತಿರುಗಿ E_INVALIDARG;

    ಬಫರ್ = (ಅಕ್ಷರ *)alloc(len);

    ಆದರೆ (ಬಫರ್ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ E_OUTOFMEMORY;

    /* Convert the (wide character) expression to an ASCII string */
    len = Wide_Char_ಗಮ್ಯ_Multi_Byte(CP_ACP, 0, ಅಭಿವ್ಯಕ್ತಿ, -1, ಬಫರ್, len, 0, 0);
    ಆದರೆ (len == 0)
	ಹಿಂತಿರುಗಿ E_INVALIDARG;

    /* Evaluate the expression */
    ++emsg_skip;
    str = (ಅಕ್ಷರ *)eval_ಗಮ್ಯ_string((char_u *)ಬಫರ್, TRUE, FALSE);
    --emsg_skip;
    vim_free(ಬಫರ್);
    ಆದರೆ (str == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ E_FAIL;

    /* Convert the result to wide characters */
    Multi_Byte_ಗಮ್ಯ_Wide_Char_alloc(CP_ACP, 0, str, -1, &w_ಬಫರ್, &len);
    vim_free(str);
    ಆದರೆ (w_ಬಫರ್ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ E_OUTOFMEMORY;

    ಆದರೆ (len == 0)
    {
	vim_free(w_ಬಫರ್);
	ಹಿಂತಿರುಗಿ E_FAIL;
    }

    /* Store the result */
    *ಫಲಿತಾಂಶ = SysAllocString(w_ಬಫರ್);
    vim_free(w_ಬಫರ್);

    ಹಿಂತಿರುಗಿ S_OK;
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ E_NOTIMPL;
#ಆದರೆ_ಅಂತ್ಯ
}

/*****************************************************************************
 3. The class factory
*****************************************************************************/

/* Definition
 * ----------
 */

ವರ್ಗ CVimCF FINAL : ಸಾರ್ವಜನಿಕ IClassFactory
{
ಸಾರ್ವಜನಿಕ:
    ಸ್ಥಿರ CVimCF *Create();
    ವಾಸ್ತವಿಕ ~CVimCF() {};

    STDMETHOD(QueryInterface)(REFIID riid, ಶೂನ್ಯ ** ppv);
    STDMETHOD_(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ, ಕೂಡಿಸು_Ref)(ಶೂನ್ಯ);
    STDMETHOD_(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ, Release)(ಶೂನ್ಯ);
    STDMETHOD(CreateInstance)(IUnknown *punkOuter, REFIID riid, ಶೂನ್ಯ ** ppv);
    STDMETHOD(ಬೀಗ_Server)(BOOL ಬೀಗ);

ಖಾಸಗಿ:
    // Constructor is private - create via Create()
    CVimCF() : ref(0) {};

    // Reference count
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ref;
};

/* Implementation
 * --------------
 */

CVimCF *CVimCF::Create()
{
    CVimCF *me = ಹೊಸ CVimCF();

    ಆದರೆ (me == ಖಾಲಿ)
	ಸಂದೇಶ_Box(0, "Cannot create class factory", "Vim Initialisation", 0);

    ಹಿಂತಿರುಗಿ me;
}

STDMETHODIMP
CVimCF::QueryInterface(REFIID riid, ಶೂನ್ಯ **ppv)
{
    ಆದರೆ (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	ಕೂಡಿಸು_Ref();
	*ppv = ಇದು;
	ಹಿಂತಿರುಗಿ S_OK;
    }

    *ppv = 0;
    ಹಿಂತಿರುಗಿ E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVimCF::ಕೂಡಿಸು_Ref()
{
    ಹಿಂತಿರುಗಿ ++ref;
}

STDMETHODIMP_(ULONG)
CVimCF::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    ಆದರೆ (ref > 0)
	--ref;
    ಹಿಂತಿರುಗಿ ref;
}

/*ARGSUSED*/
STDMETHODIMP
CVimCF::CreateInstance(IUnknown *punkOuter, REFIID riid, ಶೂನ್ಯ **ppv)
{
    ಹಿಂತಿರುಗಿ app->QueryInterface(riid, ppv);
}

/*ARGSUSED*/
STDMETHODIMP
CVimCF::ಬೀಗ_Server(BOOL ಬೀಗ)
{
    ಹಿಂತಿರುಗಿ S_OK;
}

/*****************************************************************************
 4. Registry manipulation code
*****************************************************************************/

// Internal use only
ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_ಕೀಲಿ_And_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *subkey, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ GUIDtochar(ಸ್ಥಿರಾಂಕ GUID &guid, ಅಕ್ಷರ *GUID, ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ);
ಸ್ಥಿರ ಶೂನ್ಯ Recursive_Delete_ಕೀಲಿ(HKEY h_ಕೀಲಿ_ಪೋಷಕ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಗು);
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ GUID_STRING_ಗಾತ್ರ = 39;

// Register the component in the registry
// When "silent" is TRUE don't give any messages.

ಬಾಹ್ಯ "C" ಶೂನ್ಯ ರಿಜಿಸ್ಟರ್_Me(ಪೂರ್ಣಾಂಕ silent)
{
    BOOL ok = TRUE;

    // Get the application startup command
    ಅಕ್ಷರ module[MAX_ಮಾರ್ಗ];

    ::GetModuleFileName(ಖಾಲಿ, module, MAX_ಮಾರ್ಗ);

    // Unregister first (quietly)
    UnregisterMe(FALSE);

    // Convert the CLSID into a char
    ಅಕ್ಷರ clsid[GUID_STRING_ಗಾತ್ರ];
    GUIDtochar(MYCLSID, clsid, ಗಾತ್ರ(clsid));

    // Convert the LIBID into a char
    ಅಕ್ಷರ libid[GUID_STRING_ಗಾತ್ರ];
    GUIDtochar(MYLIBID, libid, ಗಾತ್ರ(libid));

    // Build the key CLSID\\{...}
    ಅಕ್ಷರ ಕೀಲಿ_2[MAX_CLSID_LEN];
    strcpy(ಕೀಲಿ_2, "CLSID\\");
    strcat(ಕೀಲಿ_2, clsid);

    // Add the CLSID to the registry
    ಗಣ_ಕೀಲಿ_And_ಮೌಲ್ಯ(ಕೀಲಿ_2, ಖಾಲಿ, MYNAME);
    ಗಣ_ಕೀಲಿ_And_ಮೌಲ್ಯ(ಕೀಲಿ_2, "LocalServer32", module);
    ಗಣ_ಕೀಲಿ_And_ಮೌಲ್ಯ(ಕೀಲಿ_2, "ProgID", MYPROGID);
    ಗಣ_ಕೀಲಿ_And_ಮೌಲ್ಯ(ಕೀಲಿ_2, "VersionIndependentProgID", MYVIPROGID);
    ಗಣ_ಕೀಲಿ_And_ಮೌಲ್ಯ(ಕೀಲಿ_2, "TypeLib", libid);

    // Add the version-independent ProgID subkey under HKEY_CLASSES_ROOT
    ಗಣ_ಕೀಲಿ_And_ಮೌಲ್ಯ(MYVIPROGID, ಖಾಲಿ, MYNAME);
    ಗಣ_ಕೀಲಿ_And_ಮೌಲ್ಯ(MYVIPROGID, "CLSID", clsid);
    ಗಣ_ಕೀಲಿ_And_ಮೌಲ್ಯ(MYVIPROGID, "CurVer", MYPROGID);

    // Add the versioned ProgID subkey under HKEY_CLASSES_ROOT
    ಗಣ_ಕೀಲಿ_And_ಮೌಲ್ಯ(MYPROGID, ಖಾಲಿ, MYNAME);
    ಗಣ_ಕೀಲಿ_And_ಮೌಲ್ಯ(MYPROGID, "CLSID", clsid);

    wchar_t w_module[MAX_ಮಾರ್ಗ];
    Multi_Byte_ಗಮ್ಯ_Wide_Char(CP_ACP, 0, module, -1, w_module, MAX_ಮಾರ್ಗ);

    ITypeLib *typelib = ಖಾಲಿ;
    ಆದರೆ (ತುಂಬು_Type_Lib(w_module, &typelib) != S_OK)
    {
	ಆದರೆ (!silent)
	    ಸಂದೇಶ_Box(0, "Cannot load type library to register",
						       "Vim Registration", 0);
	ok = FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (ರಿಜಿಸ್ಟರ್_Type_Lib(typelib, w_module, ಖಾಲಿ) != S_OK)
	{
	    ಆದರೆ (!silent)
		ಸಂದೇಶ_Box(0, "Cannot register type library",
						       "Vim Registration", 0);
	    ok = FALSE;
	}
	typelib->Release();
    }

    ಆದರೆ (ok && !silent)
	ಸಂದೇಶ_Box(0, "Registered successfully", "Vim", 0);
}

// Remove the component from the registry
//
// Note: There is little error checking in this code, to allow incomplete
// or failed registrations to be undone.
ಬಾಹ್ಯ "C" ಶೂನ್ಯ UnregisterMe(ಪೂರ್ಣಾಂಕ bNotifyUser)
{
    // Unregister the type library
    ITypeLib *typelib;
    ಆದರೆ (SUCCEEDED(ತುಂಬು_Reg_Type_Lib(MYLIBID, MAJORVER, MINORVER, LOCALE, &typelib)))
    {
	TLIBATTR *tla;
	ಆದರೆ (SUCCEEDED(typelib->GetLibAttr(&tla)))
	{
	    Un_ರಿಜಿಸ್ಟರ್_Type_Lib(tla->guid, tla->w_Major_Ver_ಅಂಕೆ, tla->w_Minor_Ver_ಅಂಕೆ,
			      tla->lcid, tla->syskind);
	    typelib->ReleaseTLibAttr(tla);
	}
	typelib->Release();
    }

    // Convert the CLSID into a char
    ಅಕ್ಷರ clsid[GUID_STRING_ಗಾತ್ರ];
    GUIDtochar(MYCLSID, clsid, ಗಾತ್ರ(clsid));

    // Build the key CLSID\\{...}
    ಅಕ್ಷರ ಕೀಲಿ_2[MAX_CLSID_LEN];
    strcpy(ಕೀಲಿ_2, "CLSID\\");
    strcat(ಕೀಲಿ_2, clsid);

    // Delete the CLSID Key - CLSID\{...}
    Recursive_Delete_ಕೀಲಿ(HKEY_CLASSES_ಬೇರು, ಕೀಲಿ_2);

    // Delete the version-independent ProgID Key
    Recursive_Delete_ಕೀಲಿ(HKEY_CLASSES_ಬೇರು, MYVIPROGID);

    // Delete the ProgID key
    Recursive_Delete_ಕೀಲಿ(HKEY_CLASSES_ಬೇರು, MYPROGID);

    ಆದರೆ (bNotifyUser)
	ಸಂದೇಶ_Box(0, "Unregistered successfully", "Vim", 0);
}

/****************************************************************************/

// Convert a GUID to a char string
ಸ್ಥಿರ ಶೂನ್ಯ GUIDtochar(ಸ್ಥಿರಾಂಕ GUID &guid, ಅಕ್ಷರ *GUID, ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ)
{
    // Get wide string version
    LPOLESTR wGUID = ಖಾಲಿ;
    StringFromCLSID(guid, &wGUID);

    // Convert from wide characters to non-wide
    wcstombs(GUID, wGUID, ಉದ್ದಳತೆ);

    // Free memory
    CoTaskMemFree(wGUID);
}

// Delete a key and all of its descendants
ಸ್ಥಿರ ಶೂನ್ಯ Recursive_Delete_ಕೀಲಿ(HKEY h_ಕೀಲಿ_ಪೋಷಕ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಗು)
{
    // Open the child
    HKEY h_ಕೀಲಿ_ಮಗು;
    LONG ಫಲಿತಾಂಶ = Reg_Open_ಕೀಲಿ_Ex(h_ಕೀಲಿ_ಪೋಷಕ, ಮಗು, 0,
			       ಕೀಲಿ_ALL_ACCESS, &h_ಕೀಲಿ_ಮಗು);
    ಆದರೆ (ಫಲಿತಾಂಶ != ERROR_SUCCESS)
	ಹಿಂತಿರುಗಿ;

    // Enumerate all of the descendants of this child
    FILETIME ಸಮಯ;
    ಅಕ್ಷರ ಬಫರ್[1024];
    DWORD ಗಾತ್ರ_2 = 1024;

    ಆಗ (Reg_Enum_ಕೀಲಿ_Ex(h_ಕೀಲಿ_ಮಗು, 0, ಬಫರ್, &ಗಾತ್ರ_2, ಖಾಲಿ,
			ಖಾಲಿ, ಖಾಲಿ, &ಸಮಯ) == S_OK)
    {
	// Delete the descendants of this child
	Recursive_Delete_ಕೀಲಿ(h_ಕೀಲಿ_ಮಗು, ಬಫರ್);
	ಗಾತ್ರ_2 = 256;
    }

    // Close the child
    Reg_Close_ಕೀಲಿ(h_ಕೀಲಿ_ಮಗು);

    // Delete this child
    Reg_Delete_ಕೀಲಿ(h_ಕೀಲಿ_ಪೋಷಕ, ಮಗು);
}

// Create a key and set its value
ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_ಕೀಲಿ_And_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *subkey, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ)
{
    HKEY h_ಕೀಲಿ;
    ಅಕ್ಷರ ಬಫರ್[1024];

    strcpy(ಬಫರ್, ಕೀಲಿ);

    // Add subkey name to buffer.
    ಆದರೆ (subkey)
    {
	strcat(ಬಫರ್, "\\");
	strcat(ಬಫರ್, subkey);
    }

    // Create and open key and subkey.
    ದೀರ್ಘ ಫಲಿತಾಂಶ = Reg_Create_ಕೀಲಿ_Ex(HKEY_CLASSES_ಬೇರು,
				 ಬಫರ್,
				 0, ಖಾಲಿ, REG_OPTION_NON_VOLATILE,
				 ಕೀಲಿ_ALL_ACCESS, ಖಾಲಿ,
				 &h_ಕೀಲಿ, ಖಾಲಿ);
    ಆದರೆ (ಫಲಿತಾಂಶ != ERROR_SUCCESS)
	ಹಿಂತಿರುಗಿ;

    // Set the value
    ಆದರೆ (ಮೌಲ್ಯ)
	Reg_ಗಣ_ಮೌಲ್ಯ_Ex(h_ಕೀಲಿ, ಖಾಲಿ, 0, REG_SZ, (BYTE *)ಮೌಲ್ಯ,
		      (DWORD)STRLEN(ಮೌಲ್ಯ)+1);

    Reg_Close_ಕೀಲಿ(h_ಕೀಲಿ);
}

/*****************************************************************************
 5. OLE Initialisation and shutdown processing
*****************************************************************************/
ಬಾಹ್ಯ "C" ಶೂನ್ಯ InitOLE(ಪೂರ್ಣಾಂಕ *pbDoRestart)
{
    HRESULT hr;

    *pbDoRestart = FALSE;

    // Initialize the OLE libraries
    hr = OleInitialize(ಖಾಲಿ);
    ಆದರೆ (FAILED(hr))
    {
	ಸಂದೇಶ_Box(0, "Cannot initialise OLE", "Vim Initialisation", 0);
	ಹೋಗು error0;
    }

    // Create the application object
    app = CVim::Create(pbDoRestart);
    ಆದರೆ (app == ಖಾಲಿ)
	ಹೋಗು error1;

    // Create the class factory
    cf = CVimCF::Create();
    ಆದರೆ (cf == ಖಾಲಿ)
	ಹೋಗು error1;

    // Register the class factory
    hr = Co_ರಿಜಿಸ್ಟರ್_Class_Object(
	MYCLSID,
	cf,
	CLSCTX_LOCAL_SERVER,
	REGCLS_MULTIPLEUSE,
	&cf_id);

    ಆದರೆ (FAILED(hr))
    {
	ಸಂದೇಶ_Box(0, "Cannot register class factory", "Vim Initialisation", 0);
	ಹೋಗು error1;
    }

    // Register the application object as active
    hr = ರಿಜಿಸ್ಟರ್_ಸಕ್ರಿಯ_Object(
	app,
	MYCLSID,
	0,
	&app_id);

    ಆದರೆ (FAILED(hr))
    {
	ಸಂದೇಶ_Box(0, "Cannot register application object", "Vim Initialisation", 0);
	ಹೋಗು error1;
    }

    ಹಿಂತಿರುಗಿ;

    // Errors: tidy up as much as needed and return
error1:
    UninitOLE();
error0:
    ಹಿಂತಿರುಗಿ;
}

ಬಾಹ್ಯ "C" ಶೂನ್ಯ UninitOLE()
{
    // Unregister the application object
    ಆದರೆ (app_id)
    {
	Revoke_ಸಕ್ರಿಯ_Object(app_id, ಖಾಲಿ);
	app_id = 0;
    }

    // Unregister the class factory
    ಆದರೆ (cf_id)
    {
	CoRevokeClassObject(cf_id);
	cf_id = 0;
    }

    // Shut down the OLE libraries
    OleUninitialize();

    // Delete the application object
    ಆದರೆ (app)
    {
	ಅಳಿಸು app;
	app = ಖಾಲಿ;
    }

    // Delete the class factory
    ಆದರೆ (cf)
    {
	ಅಳಿಸು cf;
	cf = ಖಾಲಿ;
    }
}
#ಆದರೆ_ಅಂತ್ಯ /* FEAT_OLE */
