/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#ಯದಿ defined(FEAT_OLE) && defined(FEAT_GUI_MSWIN)
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
ಬಾಹ್ಯ HWND vim_parent_hwnd;
}

#ಯದಿ (defined(_MSC_VER) && (_MSC_VER >= 1700)) || (__cplusplus >= 201103L)
# define FINAL final
#ಅನ್ಯಥಾ
# define FINAL
#endif

#ಅನ್ತರ್ಭಾವ "if_ole.h"	// Interface definitions
#ಅನ್ತರ್ಭಾವ "iid_ole.c"	// UUID definitions (compile here)

/* Supply function prototype to work around bug in Mingw oleauto.h header */
#ifdef __MINGW32__
WINOLEAUTAPI UnRegisterTypeLib(REFGUID libID, WORD wVerMajor,
	    WORD wVerMinor, LCID lcid, SYSKIND syskind);
#endif

/*****************************************************************************
 1. Internal definitions for this file
*****************************************************************************/

ವರ್ಗ CVim;
ವರ್ಗ CVimCF;

/* Internal data */
// The identifier of the registered class factory
ಸ್ಥಿರ unsigned long cf_id = 0;

// The identifier of the running application object
ಸ್ಥಿರ unsigned long app_id = 0;

// The single global instance of the class factory
ಸ್ಥಿರ CVimCF *cf = 0;

// The single global instance of the application object
ಸ್ಥಿರ CVim *app = 0;

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

ವರ್ಗ CVim FINAL : ಸಾರ್ವಜನಿಕ IVim
{
ಸಾರ್ವಜನಿಕ:
    ಆಭಾಸ ~CVim();
    ಸ್ಥಿರ CVim *Create(ಪೂರ್ಣಾಙ್ಕ *pbDoRestart);

    // IUnknown members
    STDMETHOD(QueryInterface)(REFIID riid, ಶೂನ್ಯ ** ppv);
    STDMETHOD_(unsigned long, AddRef)(ಶೂನ್ಯ);
    STDMETHOD_(unsigned long, Release)(ಶೂನ್ಯ);

    // IDispatch members
    STDMETHOD(Get_Type_Info_ಎಣಿಕೆ)(UINT *p_ಎಣಿಕೆ);
    STDMETHOD(GetTypeInfo)(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo);
    STDMETHOD(GetIDsOfNames)(ಧ್ರುವ IID &iid, OLECHAR **names, UINT n, LCID, DISPID *dispids);
    STDMETHOD(Invoke)(DISPID member, ಧ್ರುವ IID &iid, LCID, WORD flags, DISPPARAMS *dispparams, VARIANT *result, EXCEPINFO *excepinfo, UINT *argerr);

    // IVim members
    STDMETHOD(Send_ಕೀಲಿಗಳು)(BSTR ಕೀಲಿಗಳು);
    STDMETHOD(Eval)(BSTR ಅಭಿವ್ಯಕ್ತಿ, BSTR *result);
    STDMETHOD(SetForeground)(ಶೂನ್ಯ);
    STDMETHOD(GetHwnd)(UINT_PTR *result);

ಗುಪ್ತ:
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

CVim *CVim::Create(ಪೂರ್ಣಾಙ್ಕ *pbDoRestart)
{
    HRESULT hr;
    CVim *me = 0;
    ITypeLib *typelib = 0;
    ITypeInfo *typeinfo = 0;

    *pbDoRestart = FALSE;

    // Create the object
    me = ನೂತನ CVim();
    ಯದಿ (me == ರಿಕ್ತ)
    {
	MessageBox(0, "Cannot create application object", "Vim Initialisation", 0);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // Load the type library from the registry
    hr = LoadRegTypeLib(MYLIBID, 1, 0, 0x00, &typelib);
    ಯದಿ (FAILED(hr))
    {
	HKEY h_ಕೀಲಿ;

	// Check we can write to the registry.
	// RegCreateKeyEx succeeds even if key exists. W.Briscoe W2K 20021011
	ಯದಿ (Reg_Create_ಕೀಲಿ_Ex(HKEY_CLASSES_ಬೇರು, MYVIPROGID, 0, ರಿಕ್ತ,
		  REG_OPTION_NON_VOLATILE,
		  ಕೀಲಿ_ALL_ACCESS, ರಿಕ್ತ, &h_ಕೀಲಿ, ರಿಕ್ತ))
	{
	    delete me;
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ; // Unable to write to registry. Quietly fail.
	}
	Reg_Close_ಕೀಲಿ(h_ಕೀಲಿ);

	ಯದಿ (MessageBox(0, "Cannot load registered type library.\nDo you want to register Vim now?",
		    "Vim Initialisation", MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
	    delete me;
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	RegisterMe(FALSE);

	// Load the type library from the registry
	hr = LoadRegTypeLib(MYLIBID, 1, 0, 0x00, &typelib);
	ಯದಿ (FAILED(hr))
	{
	    MessageBox(0, "You must restart Vim in order for the registration to take effect.",
						     "Vim Initialisation", 0);
	    *pbDoRestart = TRUE;
	    delete me;
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
    }

    // Get the type info of the vtable interface
    hr = typelib->GetTypeInfoOfGuid(MYIID, &typeinfo);
    typelib->Release();

    ಯದಿ (FAILED(hr))
    {
	MessageBox(0, "Cannot get interface type information",
						     "Vim Initialisation", 0);
	delete me;
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // Save the type information
    me->typeinfo = typeinfo;
    ಪ್ರತ್ಯಾಗಮ me;
}

CVim::~CVim()
{
    ಯದಿ (typeinfo && vim_parent_hwnd == ರಿಕ್ತ)
	typeinfo->Release();
    typeinfo = 0;
}

STDMETHODIMP
CVim::QueryInterface(REFIID riid, ಶೂನ್ಯ **ppv)
{
    ಯದಿ (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDispatch) || IsEqualIID(riid, MYIID))
    {
	AddRef();
	*ppv = ಏತತ್;
	ಪ್ರತ್ಯಾಗಮ S_OK;
    }

    *ppv = 0;
    ಪ್ರತ್ಯಾಗಮ E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVim::AddRef()
{
    ಪ್ರತ್ಯಾಗಮ ++ref;
}

STDMETHODIMP_(ULONG)
CVim::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    ಯದಿ (ref > 0)
	--ref;
    ಪ್ರತ್ಯಾಗಮ ref;
}

STDMETHODIMP
CVim::Get_Type_Info_ಎಣಿಕೆ(UINT *p_ಎಣಿಕೆ)
{
    *p_ಎಣಿಕೆ = 1;
    ಪ್ರತ್ಯಾಗಮ S_OK;
}

STDMETHODIMP
CVim::GetTypeInfo(UINT iTypeInfo, LCID, ITypeInfo **ppITypeInfo)
{
    *ppITypeInfo = 0;

    ಯದಿ (iTypeInfo != 0)
	ಪ್ರತ್ಯಾಗಮ DISP_E_BADINDEX;

    typeinfo->AddRef();
    *ppITypeInfo = typeinfo;
    ಪ್ರತ್ಯಾಗಮ S_OK;
}

STDMETHODIMP
CVim::GetIDsOfNames(
	ಧ್ರುವ IID &iid,
	OLECHAR **names,
	UINT n,
	LCID,
	DISPID *dispids)
{
    ಯದಿ (iid != IID_NULL)
	ಪ್ರತ್ಯಾಗಮ DISP_E_UNKNOWNINTERFACE;

    ಪ್ರತ್ಯಾಗಮ typeinfo->GetIDsOfNames(names, n, dispids);
}

STDMETHODIMP
CVim::Invoke(
	DISPID member,
	ಧ್ರುವ IID &iid,
	LCID,
	WORD flags,
	DISPPARAMS *dispparams,
	VARIANT *result,
	EXCEPINFO *excepinfo,
	UINT *argerr)
{
    ಯದಿ (iid != IID_NULL)
	ಪ್ರತ್ಯಾಗಮ DISP_E_UNKNOWNINTERFACE;

    ::SetErrorInfo(0, ರಿಕ್ತ);
    ಪ್ರತ್ಯಾಗಮ typeinfo->Invoke(static_cast<IDispatch*>(ಏತತ್),
			    member, flags, dispparams,
			    result, excepinfo, argerr);
}

STDMETHODIMP
CVim::GetHwnd(UINT_PTR *result)
{
    *result = (UINT_PTR)s_hwnd;
    ಪ್ರತ್ಯಾಗಮ S_OK;
}

STDMETHODIMP
CVim::SetForeground(ಶೂನ್ಯ)
{
    /* Make the Vim window come to the foreground */
    gui_mch_set_foreground();
    ಪ್ರತ್ಯಾಗಮ S_OK;
}

STDMETHODIMP
CVim::Send_ಕೀಲಿಗಳು(BSTR ಕೀಲಿಗಳು)
{
    ಪೂರ್ಣಾಙ್ಕ len;
    ವರ್ಣ *buffer;
    char_u *str;
    char_u *ptr;

    /* Get a suitable buffer */
    len = WideCharToMultiByte(CP_ACP, 0, ಕೀಲಿಗಳು, -1, 0, 0, 0, 0);
    buffer = (ವರ್ಣ *)alloc(len+1);

    ಯದಿ (buffer == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ E_OUTOFMEMORY;

    len = WideCharToMultiByte(CP_ACP, 0, ಕೀಲಿಗಳು, -1, buffer, len, 0, 0);

    ಯದಿ (len == 0)
    {
	vim_free(buffer);
	ಪ್ರತ್ಯಾಗಮ E_INVALIDARG;
    }

    /* Translate key codes like <Esc> */
    str = replace_termcodes((char_u *)buffer, &ptr, 0, REPTERM_DO_LT, ರಿಕ್ತ);

    /* If ptr was set, then a new buffer was allocated,
     * so we can free the old one.
     */
    ಯದಿ (ptr)
	vim_free((char_u *)(buffer));

    /* Reject strings too long to fit in the input buffer. Allow 10 bytes
     * space to cover for the (remote) possibility that characters may enter
     * the input buffer between now and when the WM_OLE message is actually
     * processed. If more than 10 characters enter the input buffer in that
     * time, the WM_OLE processing will simply fail to insert the characters.
     */
    ಯದಿ ((ಪೂರ್ಣಾಙ್ಕ)(STRLEN(str)) > (vim_free_in_ಒಳಹರಿವು_buf() - 10))
    {
	vim_free(str);
	ಪ್ರತ್ಯಾಗಮ E_INVALIDARG;
    }

    /* Pass the string to the main input loop. The memory will be freed when
     * the message is processed.  Except for an empty message, we don't need
     * to post it then.
     */
    ಯದಿ (*str == NUL)
	vim_free(str);
    ಅನ್ಯಥಾ
	PostMessage(ರಿಕ್ತ, WM_OLE, 0, (LPARAM)str);

    ಪ್ರತ್ಯಾಗಮ S_OK;
}

STDMETHODIMP
CVim::Eval(BSTR ಅಭಿವ್ಯಕ್ತಿ, BSTR *result)
{
#ifdef FEAT_EVAL
    ಪೂರ್ಣಾಙ್ಕ len;
    ವರ್ಣ *buffer;
    ವರ್ಣ *str;
    wchar_t *w_buffer;

    /* Get a suitable buffer */
    len = WideCharToMultiByte(CP_ACP, 0, ಅಭಿವ್ಯಕ್ತಿ, -1, 0, 0, 0, 0);
    ಯದಿ (len == 0)
	ಪ್ರತ್ಯಾಗಮ E_INVALIDARG;

    buffer = (ವರ್ಣ *)alloc(len);

    ಯದಿ (buffer == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ E_OUTOFMEMORY;

    /* Convert the (wide character) expression to an ASCII string */
    len = WideCharToMultiByte(CP_ACP, 0, ಅಭಿವ್ಯಕ್ತಿ, -1, buffer, len, 0, 0);
    ಯದಿ (len == 0)
	ಪ್ರತ್ಯಾಗಮ E_INVALIDARG;

    /* Evaluate the expression */
    ++emsg_skip;
    str = (ವರ್ಣ *)eval_to_string((char_u *)buffer, TRUE, FALSE);
    --emsg_skip;
    vim_free(buffer);
    ಯದಿ (str == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ E_FAIL;

    /* Convert the result to wide characters */
    MultiByteToWideChar_alloc(CP_ACP, 0, str, -1, &w_buffer, &len);
    vim_free(str);
    ಯದಿ (w_buffer == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ E_OUTOFMEMORY;

    ಯದಿ (len == 0)
    {
	vim_free(w_buffer);
	ಪ್ರತ್ಯಾಗಮ E_FAIL;
    }

    /* Store the result */
    *result = SysAllocString(w_buffer);
    vim_free(w_buffer);

    ಪ್ರತ್ಯಾಗಮ S_OK;
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ E_NOTIMPL;
#endif
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
    ಆಭಾಸ ~CVimCF() {};

    STDMETHOD(QueryInterface)(REFIID riid, ಶೂನ್ಯ ** ppv);
    STDMETHOD_(unsigned long, AddRef)(ಶೂನ್ಯ);
    STDMETHOD_(unsigned long, Release)(ಶೂನ್ಯ);
    STDMETHOD(CreateInstance)(IUnknown *punkOuter, REFIID riid, ಶೂನ್ಯ ** ppv);
    STDMETHOD(LockServer)(BOOL lock);

ಗುಪ್ತ:
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
    CVimCF *me = ನೂತನ CVimCF();

    ಯದಿ (me == ರಿಕ್ತ)
	MessageBox(0, "Cannot create class factory", "Vim Initialisation", 0);

    ಪ್ರತ್ಯಾಗಮ me;
}

STDMETHODIMP
CVimCF::QueryInterface(REFIID riid, ಶೂನ್ಯ **ppv)
{
    ಯದಿ (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	AddRef();
	*ppv = ಏತತ್;
	ಪ್ರತ್ಯಾಗಮ S_OK;
    }

    *ppv = 0;
    ಪ್ರತ್ಯಾಗಮ E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)
CVimCF::AddRef()
{
    ಪ್ರತ್ಯಾಗಮ ++ref;
}

STDMETHODIMP_(ULONG)
CVimCF::Release()
{
    // Don't delete the object when the reference count reaches zero, as there
    // is only a single application object, and its lifetime is controlled by
    // the running instance, not by its reference count.
    ಯದಿ (ref > 0)
	--ref;
    ಪ್ರತ್ಯಾಗಮ ref;
}

/*ARGSUSED*/
STDMETHODIMP
CVimCF::CreateInstance(IUnknown *punkOuter, REFIID riid, ಶೂನ್ಯ **ppv)
{
    ಪ್ರತ್ಯಾಗಮ app->QueryInterface(riid, ppv);
}

/*ARGSUSED*/
STDMETHODIMP
CVimCF::LockServer(BOOL lock)
{
    ಪ್ರತ್ಯಾಗಮ S_OK;
}

/*****************************************************************************
 4. Registry manipulation code
*****************************************************************************/

// Internal use only
ಸ್ಥಿರ ಶೂನ್ಯ Set_ಕೀಲಿ_And_ಮೌಲ್ಯ(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *subkey, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ GUIDtochar(ಧ್ರುವ GUID &guid, ವರ್ಣ *GUID, ಪೂರ್ಣಾಙ್ಕ length);
ಸ್ಥಿರ ಶೂನ್ಯ Recursive_Delete_ಕೀಲಿ(HKEY h_ಕೀಲಿ_Parent, ಧ್ರುವ ವರ್ಣ *child);
ಸ್ಥಿರ ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ GUID_STRING_SIZE = 39;

// Register the component in the registry
// When "silent" is TRUE don't give any messages.

ಬಾಹ್ಯ "C" ಶೂನ್ಯ RegisterMe(ಪೂರ್ಣಾಙ್ಕ silent)
{
    BOOL ok = TRUE;

    // Get the application startup command
    ವರ್ಣ module[MAX_PATH];

    ::GetModuleFileName(ರಿಕ್ತ, module, MAX_PATH);

    // Unregister first (quietly)
    UnregisterMe(FALSE);

    // Convert the CLSID into a char
    ವರ್ಣ clsid[GUID_STRING_SIZE];
    GUIDtochar(MYCLSID, clsid, ಪರಿಮಾಣ(clsid));

    // Convert the LIBID into a char
    ವರ್ಣ libid[GUID_STRING_SIZE];
    GUIDtochar(MYLIBID, libid, ಪರಿಮಾಣ(libid));

    // Build the key CLSID\\{...}
    ವರ್ಣ ಕೀಲಿ_2[MAX_CLSID_LEN];
    strcpy(ಕೀಲಿ_2, "CLSID\\");
    strcat(ಕೀಲಿ_2, clsid);

    // Add the CLSID to the registry
    Set_ಕೀಲಿ_And_ಮೌಲ್ಯ(ಕೀಲಿ_2, ರಿಕ್ತ, MYNAME);
    Set_ಕೀಲಿ_And_ಮೌಲ್ಯ(ಕೀಲಿ_2, "LocalServer32", module);
    Set_ಕೀಲಿ_And_ಮೌಲ್ಯ(ಕೀಲಿ_2, "ProgID", MYPROGID);
    Set_ಕೀಲಿ_And_ಮೌಲ್ಯ(ಕೀಲಿ_2, "VersionIndependentProgID", MYVIPROGID);
    Set_ಕೀಲಿ_And_ಮೌಲ್ಯ(ಕೀಲಿ_2, "TypeLib", libid);

    // Add the version-independent ProgID subkey under HKEY_CLASSES_ROOT
    Set_ಕೀಲಿ_And_ಮೌಲ್ಯ(MYVIPROGID, ರಿಕ್ತ, MYNAME);
    Set_ಕೀಲಿ_And_ಮೌಲ್ಯ(MYVIPROGID, "CLSID", clsid);
    Set_ಕೀಲಿ_And_ಮೌಲ್ಯ(MYVIPROGID, "CurVer", MYPROGID);

    // Add the versioned ProgID subkey under HKEY_CLASSES_ROOT
    Set_ಕೀಲಿ_And_ಮೌಲ್ಯ(MYPROGID, ರಿಕ್ತ, MYNAME);
    Set_ಕೀಲಿ_And_ಮೌಲ್ಯ(MYPROGID, "CLSID", clsid);

    wchar_t w_module[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, module, -1, w_module, MAX_PATH);

    ITypeLib *typelib = ರಿಕ್ತ;
    ಯದಿ (LoadTypeLib(w_module, &typelib) != S_OK)
    {
	ಯದಿ (!silent)
	    MessageBox(0, "Cannot load type library to register",
						       "Vim Registration", 0);
	ok = FALSE;
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (RegisterTypeLib(typelib, w_module, ರಿಕ್ತ) != S_OK)
	{
	    ಯದಿ (!silent)
		MessageBox(0, "Cannot register type library",
						       "Vim Registration", 0);
	    ok = FALSE;
	}
	typelib->Release();
    }

    ಯದಿ (ok && !silent)
	MessageBox(0, "Registered successfully", "Vim", 0);
}

// Remove the component from the registry
//
// Note: There is little error checking in this code, to allow incomplete
// or failed registrations to be undone.
ಬಾಹ್ಯ "C" ಶೂನ್ಯ UnregisterMe(ಪೂರ್ಣಾಙ್ಕ bNotifyUser)
{
    // Unregister the type library
    ITypeLib *typelib;
    ಯದಿ (SUCCEEDED(LoadRegTypeLib(MYLIBID, MAJORVER, MINORVER, LOCALE, &typelib)))
    {
	TLIBATTR *tla;
	ಯದಿ (SUCCEEDED(typelib->GetLibAttr(&tla)))
	{
	    UnRegisterTypeLib(tla->guid, tla->w_Major_Ver_ಅಂಕೆ, tla->w_Minor_Ver_ಅಂಕೆ,
			      tla->lcid, tla->syskind);
	    typelib->ReleaseTLibAttr(tla);
	}
	typelib->Release();
    }

    // Convert the CLSID into a char
    ವರ್ಣ clsid[GUID_STRING_SIZE];
    GUIDtochar(MYCLSID, clsid, ಪರಿಮಾಣ(clsid));

    // Build the key CLSID\\{...}
    ವರ್ಣ ಕೀಲಿ_2[MAX_CLSID_LEN];
    strcpy(ಕೀಲಿ_2, "CLSID\\");
    strcat(ಕೀಲಿ_2, clsid);

    // Delete the CLSID Key - CLSID\{...}
    Recursive_Delete_ಕೀಲಿ(HKEY_CLASSES_ಬೇರು, ಕೀಲಿ_2);

    // Delete the version-independent ProgID Key
    Recursive_Delete_ಕೀಲಿ(HKEY_CLASSES_ಬೇರು, MYVIPROGID);

    // Delete the ProgID key
    Recursive_Delete_ಕೀಲಿ(HKEY_CLASSES_ಬೇರು, MYPROGID);

    ಯದಿ (bNotifyUser)
	MessageBox(0, "Unregistered successfully", "Vim", 0);
}

/****************************************************************************/

// Convert a GUID to a char string
ಸ್ಥಿರ ಶೂನ್ಯ GUIDtochar(ಧ್ರುವ GUID &guid, ವರ್ಣ *GUID, ಪೂರ್ಣಾಙ್ಕ length)
{
    // Get wide string version
    LPOLESTR wGUID = ರಿಕ್ತ;
    StringFromCLSID(guid, &wGUID);

    // Convert from wide characters to non-wide
    wcstombs(GUID, wGUID, length);

    // Free memory
    CoTaskMemFree(wGUID);
}

// Delete a key and all of its descendants
ಸ್ಥಿರ ಶೂನ್ಯ Recursive_Delete_ಕೀಲಿ(HKEY h_ಕೀಲಿ_Parent, ಧ್ರುವ ವರ್ಣ *child)
{
    // Open the child
    HKEY h_ಕೀಲಿ_Child;
    LONG result = Reg_Open_ಕೀಲಿ_Ex(h_ಕೀಲಿ_Parent, child, 0,
			       ಕೀಲಿ_ALL_ACCESS, &h_ಕೀಲಿ_Child);
    ಯದಿ (result != ERROR_SUCCESS)
	ಪ್ರತ್ಯಾಗಮ;

    // Enumerate all of the descendants of this child
    FILETIME time;
    ವರ್ಣ buffer[1024];
    DWORD size = 1024;

    ಯಾವತ್ (Reg_Enum_ಕೀಲಿ_Ex(h_ಕೀಲಿ_Child, 0, buffer, &size, ರಿಕ್ತ,
			ರಿಕ್ತ, ರಿಕ್ತ, &time) == S_OK)
    {
	// Delete the descendants of this child
	Recursive_Delete_ಕೀಲಿ(h_ಕೀಲಿ_Child, buffer);
	size = 256;
    }

    // Close the child
    Reg_Close_ಕೀಲಿ(h_ಕೀಲಿ_Child);

    // Delete this child
    Reg_Delete_ಕೀಲಿ(h_ಕೀಲಿ_Parent, child);
}

// Create a key and set its value
ಸ್ಥಿರ ಶೂನ್ಯ Set_ಕೀಲಿ_And_ಮೌಲ್ಯ(ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ *subkey, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ)
{
    HKEY h_ಕೀಲಿ;
    ವರ್ಣ buffer[1024];

    strcpy(buffer, ಕೀಲಿ);

    // Add subkey name to buffer.
    ಯದಿ (subkey)
    {
	strcat(buffer, "\\");
	strcat(buffer, subkey);
    }

    // Create and open key and subkey.
    long result = Reg_Create_ಕೀಲಿ_Ex(HKEY_CLASSES_ಬೇರು,
				 buffer,
				 0, ರಿಕ್ತ, REG_OPTION_NON_VOLATILE,
				 ಕೀಲಿ_ALL_ACCESS, ರಿಕ್ತ,
				 &h_ಕೀಲಿ, ರಿಕ್ತ);
    ಯದಿ (result != ERROR_SUCCESS)
	ಪ್ರತ್ಯಾಗಮ;

    // Set the value
    ಯದಿ (ಮೌಲ್ಯ)
	Reg_Set_ಮೌಲ್ಯ_Ex(h_ಕೀಲಿ, ರಿಕ್ತ, 0, REG_SZ, (BYTE *)ಮೌಲ್ಯ,
		      (DWORD)STRLEN(ಮೌಲ್ಯ)+1);

    Reg_Close_ಕೀಲಿ(h_ಕೀಲಿ);
}

/*****************************************************************************
 5. OLE Initialisation and shutdown processing
*****************************************************************************/
ಬಾಹ್ಯ "C" ಶೂನ್ಯ InitOLE(ಪೂರ್ಣಾಙ್ಕ *pbDoRestart)
{
    HRESULT hr;

    *pbDoRestart = FALSE;

    // Initialize the OLE libraries
    hr = OleInitialize(ರಿಕ್ತ);
    ಯದಿ (FAILED(hr))
    {
	MessageBox(0, "Cannot initialise OLE", "Vim Initialisation", 0);
	ಗಚ್ಛ error0;
    }

    // Create the application object
    app = CVim::Create(pbDoRestart);
    ಯದಿ (app == ರಿಕ್ತ)
	ಗಚ್ಛ error1;

    // Create the class factory
    cf = CVimCF::Create();
    ಯದಿ (cf == ರಿಕ್ತ)
	ಗಚ್ಛ error1;

    // Register the class factory
    hr = CoRegisterClassObject(
	MYCLSID,
	cf,
	CLSCTX_LOCAL_SERVER,
	REGCLS_MULTIPLEUSE,
	&cf_id);

    ಯದಿ (FAILED(hr))
    {
	MessageBox(0, "Cannot register class factory", "Vim Initialisation", 0);
	ಗಚ್ಛ error1;
    }

    // Register the application object as active
    hr = RegisterActiveObject(
	app,
	MYCLSID,
	0,
	&app_id);

    ಯದಿ (FAILED(hr))
    {
	MessageBox(0, "Cannot register application object", "Vim Initialisation", 0);
	ಗಚ್ಛ error1;
    }

    ಪ್ರತ್ಯಾಗಮ;

    // Errors: tidy up as much as needed and return
error1:
    UninitOLE();
error0:
    ಪ್ರತ್ಯಾಗಮ;
}

ಬಾಹ್ಯ "C" ಶೂನ್ಯ UninitOLE()
{
    // Unregister the application object
    ಯದಿ (app_id)
    {
	RevokeActiveObject(app_id, ರಿಕ್ತ);
	app_id = 0;
    }

    // Unregister the class factory
    ಯದಿ (cf_id)
    {
	CoRevokeClassObject(cf_id);
	cf_id = 0;
    }

    // Shut down the OLE libraries
    OleUninitialize();

    // Delete the application object
    ಯದಿ (app)
    {
	delete app;
	app = ರಿಕ್ತ;
    }

    // Delete the class factory
    ಯದಿ (cf)
    {
	delete cf;
	cf = ರಿಕ್ತ;
    }
}
#endif /* FEAT_OLE */
