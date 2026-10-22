// this ALWAYS GENERATED file contains the definitions for the interfaces


// File created by MIDL compiler version 3.01.75
// at Wed Jun 06 18:20:37 2001
// Compiler settings for .\if_ole.idl:
//  Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
//  error checks: none
//@@MIDL_FILE_HEADING(  )
#ضمن "rpc.h"
#ضمن "rpcndr.h"
#اگر_تعریف_نشده COM_NO_WINDOWS_H
#ضمن "windows.h"
#ضمن "ole2.h"
#پایان_اگر //COM_NO_WINDOWS_H

#اگر_تعریف_نشده __if_ole_h__
#تعریف_کن __if_ole_h__

#اگر_تعریف_شده __cplusplus
خارجي "C"{
#پایان_اگر

// Forward Declarations

#اگر_تعریف_نشده __IVim_FWD_DEFINED__
#تعریف_کن __IVim_FWD_DEFINED__
تعريفنوع interface IVim IVim;
#پایان_اگر	// __IVim_FWD_DEFINED__


#اگر_تعریف_نشده __Vim_FWD_DEFINED__
#تعریف_کن __Vim_FWD_DEFINED__

#اگر_تعریف_شده __cplusplus
تعريفنوع class Vim Vim;
#وإلا
تعريفنوع بنية Vim Vim;
#پایان_اگر // __cplusplus

#پایان_اگر	// __Vim_FWD_DEFINED__


// header files for imported files
#ضمن "oaidl.h"

#اگر_تعریف_نشده __MIDL_user_allocate_free_DEFINED__
#تعریف_کن __MIDL_user_allocate_free_DEFINED__
    فراغ __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
    فراغ __RPC_USER MIDL_user_free( فراغ __RPC_FAR * );
#پایان_اگر

#اگر_تعریف_نشده __IVim_INTERFACE_DEFINED__
#تعریف_کن __IVim_INTERFACE_DEFINED__

//***************************************
// Generated header for interface: IVim
// at Wed Jun 06 18:20:37 2001
// using MIDL 3.01.75
//**************************************
// [oleautomation][dual][unique][helpstring][uuid][object]



EXTERN_C ثابت IID IID_IVim;

#إذا defined(__cplusplus) && !defined(CINTERFACE)

    interface DECLSPEC_UUID("0F0BFAE2-4C90-11d1-82D7-0004AC368519")
    IVim : عام IDispatch
    {
    عام:
	مجازی HRESULT STDMETHODCALLTYPE Send_کلیدها(
	    /* [in] */ BSTR کلیدها) = 0;

	مجازی HRESULT STDMETHODCALLTYPE Eval(
	    /* [in] */ BSTR عبارت,
	    /* [retval][out] */ BSTR __RPC_FAR *نتیجه) = 0;

	مجازی HRESULT STDMETHODCALLTYPE مجموعه_Foreground( فراغ) = 0;

	مجازی HRESULT STDMETHODCALLTYPE GetHwnd(
	    /* [retval][out] */ UINT_PTR __RPC_FAR *نتیجه) = 0;

    };

#وإلا	// C style interface

    تعريفنوع بنية IVimVtbl
    {
	BEGIN_INTERFACE

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )(
	    IVim __RPC_FAR * This,
	    /* [in] */ REFIID riid,
	    /* [iid_is][out] */ فراغ __RPC_FAR *__RPC_FAR *ppvObject);

	ULONG ( STDMETHODCALLTYPE __RPC_FAR *جمع_کن_Ref )(
	    IVim __RPC_FAR * This);

	ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )(
	    IVim __RPC_FAR * This);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Get_Type_Info_شمار )(
	    IVim __RPC_FAR * This,
	    /* [out] */ UINT __RPC_FAR *pctinfo);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )(
	    IVim __RPC_FAR * This,
	    /* [in] */ UINT iTInfo,
	    /* [in] */ LCID lcid,
	    /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Get_IDs_از_نامها )(
	    IVim __RPC_FAR * This,
	    /* [in] */ REFIID riid,
	    /* [size_is][in] */ LPOLESTR __RPC_FAR *rgsz_نامها,
	    /* [in] */ UINT c_نامها,
	    /* [in] */ LCID lcid,
	    /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);

	/* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )(
	    IVim __RPC_FAR * This,
	    /* [in] */ DISPID dispIdMember,
	    /* [in] */ REFIID riid,
	    /* [in] */ LCID lcid,
	    /* [in] */ کلمه_2 wFlags,
	    /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
	    /* [out] */ VARIANT __RPC_FAR *p_Var_نتیجه,
	    /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
	    /* [out] */ UINT __RPC_FAR *puArgErr);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Send_کلیدها )(
	    IVim __RPC_FAR * This,
	    /* [in] */ BSTR کلیدها);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Eval )(
	    IVim __RPC_FAR * This,
	    /* [in] */ BSTR عبارت,
	    /* [retval][out] */ BSTR __RPC_FAR *نتیجه);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *مجموعه_Foreground )(
	    IVim __RPC_FAR * This);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetHwnd )(
	    IVim __RPC_FAR * This,
	    /* [retval][out] */ UINT_PTR __RPC_FAR *نتیجه);

	انتها_INTERFACE
    } IVimVtbl;

    interface IVim
    {
	CONST_VTBL بنية IVimVtbl __RPC_FAR *lpVtbl;
    };

#اگر_تعریف_شده COBJMACROS


#تعریف_کن IVim_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#تعریف_کن IVim_جمع_کن_Ref(This)	\
    (This)->lpVtbl -> جمع_کن_Ref(This)

#تعریف_کن IVim_Release(This)	\
    (This)->lpVtbl -> Release(This)


#تعریف_کن IVim_Get_Type_Info_شمار(This,pctinfo)	\
    (This)->lpVtbl -> Get_Type_Info_شمار(This,pctinfo)

#تعریف_کن IVim_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#تعریف_کن IVim_Get_IDs_از_نامها(This,riid,rgsz_نامها,c_نامها,lcid,rgDispId)	\
    (This)->lpVtbl -> Get_IDs_از_نامها(This,riid,rgsz_نامها,c_نامها,lcid,rgDispId)

#تعریف_کن IVim_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,p_Var_نتیجه,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,p_Var_نتیجه,pExcepInfo,puArgErr)


#تعریف_کن IVim_Send_کلیدها(This,کلیدها)	\
    (This)->lpVtbl -> Send_کلیدها(This,کلیدها)

#تعریف_کن IVim_Eval(This,عبارت,نتیجه)	\
    (This)->lpVtbl -> Eval(This,عبارت,نتیجه)

#تعریف_کن IVim_مجموعه_Foreground(This)	\
    (This)->lpVtbl -> مجموعه_Foreground(This)

#تعریف_کن IVim_GetHwnd(This,نتیجه)	\
    (This)->lpVtbl -> GetHwnd(This,نتیجه)

#پایان_اگر // COBJMACROS


#پایان_اگر	// C style interface



HRESULT STDMETHODCALLTYPE IVim_Send_کلیدها_Proxy(
    IVim __RPC_FAR * This,
    /* [in] */ BSTR کلیدها);


فراغ __RPC_STUB IVim_Send_کلیدها_Stub(
    IRpc_Stub_میانگیر *This,
    IRpc_Channel_میانگیر *p_Rpc_Channel_میانگیر,
    PRPC_پیام p_Rpc_پیام,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVim_Eval_Proxy(
    IVim __RPC_FAR * This,
    /* [in] */ BSTR عبارت,
    /* [retval][out] */ BSTR __RPC_FAR *نتیجه);


فراغ __RPC_STUB IVim_Eval_Stub(
    IRpc_Stub_میانگیر *This,
    IRpc_Channel_میانگیر *p_Rpc_Channel_میانگیر,
    PRPC_پیام p_Rpc_پیام,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVim_مجموعه_Foreground_Proxy(
    IVim __RPC_FAR * This);


فراغ __RPC_STUB IVim_مجموعه_Foreground_Stub(
    IRpc_Stub_میانگیر *This,
    IRpc_Channel_میانگیر *p_Rpc_Channel_میانگیر,
    PRPC_پیام p_Rpc_پیام,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVim_GetHwnd_Proxy(
    IVim __RPC_FAR * This,
    /* [retval][out] */ UINT_PTR __RPC_FAR *نتیجه);


فراغ __RPC_STUB IVim_GetHwnd_Stub(
    IRpc_Stub_میانگیر *This,
    IRpc_Channel_میانگیر *p_Rpc_Channel_میانگیر,
    PRPC_پیام p_Rpc_پیام,
    DWORD *_pdwStubPhase);



#پایان_اگر	// __IVim_INTERFACE_DEFINED__



#اگر_تعریف_نشده __Vim_LIBRARY_DEFINED__
#تعریف_کن __Vim_LIBRARY_DEFINED__

//***************************************
// Generated header for library: Vim
// at Wed Jun 06 18:20:37 2001
// using MIDL 3.01.75
//**************************************
// [version][helpstring][uuid]



EXTERN_C ثابت IID LIBID_Vim;

#اگر_تعریف_شده __cplusplus
EXTERN_C ثابت CLSID CLSID_Vim;

class DECLSPEC_UUID("0F0BFAE1-4C90-11d1-82D7-0004AC368519")
Vim;
#پایان_اگر
#پایان_اگر // __Vim_LIBRARY_DEFINED__

// Additional Prototypes for ALL interfaces

بیعلامت دراز		  __RPC_USER  BSTR_User_اندازه(	 بیعلامت دراز __RPC_FAR *, بیعلامت دراز	     , BSTR __RPC_FAR * );
بیعلامت حرف __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  بیعلامت دراز __RPC_FAR *, بیعلامت حرف __RPC_FAR *, BSTR __RPC_FAR * );
بیعلامت حرف __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(بیعلامت دراز __RPC_FAR *, بیعلامت حرف __RPC_FAR *, BSTR __RPC_FAR * );
فراغ			  __RPC_USER  BSTR_UserFree(	 بیعلامت دراز __RPC_FAR *, BSTR __RPC_FAR * );

// end of Additional Prototypes

#اگر_تعریف_شده __cplusplus
}
#پایان_اگر

#پایان_اگر
