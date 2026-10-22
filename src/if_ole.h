// this ALWAYS GENERATED file contains the definitions for the interfaces


// File created by MIDL compiler version 3.01.75
// at Wed Jun 06 18:20:37 2001
// Compiler settings for .\if_ole.idl:
//  Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
//  error checks: none
//@@MIDL_FILE_HEADING(  )
#ਅਨ੍ਤਰ੍ਭਾਵ "rpc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rpcndr.h"
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ COM_NO_WINDOWS_H
#ਅਨ੍ਤਰ੍ਭਾਵ "windows.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ole2.h"
#ਜੇ_ਅੰਤ //COM_NO_WINDOWS_H

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __if_ole_h__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __if_ole_h__

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __cplusplus
ਬਾਹਰੀ "C"{
#ਜੇ_ਅੰਤ

// Forward Declarations

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __IVim_FWD_DEFINED__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __IVim_FWD_DEFINED__
ਕਿਸਮਪਰਿਭਾਸ਼ਾ interface IVim IVim;
#ਜੇ_ਅੰਤ	// __IVim_FWD_DEFINED__


#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __Vim_FWD_DEFINED__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __Vim_FWD_DEFINED__

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __cplusplus
ਕਿਸਮਪਰਿਭਾਸ਼ਾ class Vim Vim;
#ਨਹੀਂਤਾਂ
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Vim Vim;
#ਜੇ_ਅੰਤ // __cplusplus

#ਜੇ_ਅੰਤ	// __Vim_FWD_DEFINED__


// header files for imported files
#ਅਨ੍ਤਰ੍ਭਾਵ "oaidl.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __MIDL_user_allocate_free_DEFINED__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __MIDL_user_allocate_free_DEFINED__
    ਖਾਲੀ __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
    ਖਾਲੀ __RPC_USER MIDL_user_free( ਖਾਲੀ __RPC_FAR * );
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __IVim_INTERFACE_DEFINED__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __IVim_INTERFACE_DEFINED__

//***************************************
// Generated header for interface: IVim
// at Wed Jun 06 18:20:37 2001
// using MIDL 3.01.75
//**************************************
// [oleautomation][dual][unique][helpstring][uuid][object]



EXTERN_C ਅਚਲ IID IID_IVim;

#ਜੇ defined(__cplusplus) && !defined(CINTERFACE)

    interface DECLSPEC_UUID("0F0BFAE2-4C90-11d1-82D7-0004AC368519")
    IVim : ਜਨਤਕ IDispatch
    {
    ਜਨਤਕ:
	ਆਭਾਸੀ HRESULT STDMETHODCALLTYPE Send_ਕੁੰਜੀਆਂ(
	    /* [in] */ BSTR ਕੁੰਜੀਆਂ) = 0;

	ਆਭਾਸੀ HRESULT STDMETHODCALLTYPE Eval(
	    /* [in] */ BSTR ਵਿਅੰਜਕ,
	    /* [retval][out] */ BSTR __RPC_FAR *ਨਤੀਜਾ) = 0;

	ਆਭਾਸੀ HRESULT STDMETHODCALLTYPE ਸੈੱਟ_Foreground( ਖਾਲੀ) = 0;

	ਆਭਾਸੀ HRESULT STDMETHODCALLTYPE GetHwnd(
	    /* [retval][out] */ UINT_PTR __RPC_FAR *ਨਤੀਜਾ) = 0;

    };

#ਨਹੀਂਤਾਂ	// C style interface

    ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ IVimVtbl
    {
	BEGIN_INTERFACE

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )(
	    IVim __RPC_FAR * This,
	    /* [in] */ REFIID riid,
	    /* [iid_is][out] */ ਖਾਲੀ __RPC_FAR *__RPC_FAR *ppvObject);

	ULONG ( STDMETHODCALLTYPE __RPC_FAR *ਜੋੜੋ_Ref )(
	    IVim __RPC_FAR * This);

	ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )(
	    IVim __RPC_FAR * This);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Get_Type_Info_ਗਿਣਤੀ )(
	    IVim __RPC_FAR * This,
	    /* [out] */ UINT __RPC_FAR *pctinfo);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )(
	    IVim __RPC_FAR * This,
	    /* [in] */ UINT iTInfo,
	    /* [in] */ LCID lcid,
	    /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Get_IDs_ਦਾ_ਨਾਂ )(
	    IVim __RPC_FAR * This,
	    /* [in] */ REFIID riid,
	    /* [size_is][in] */ LPOLESTR __RPC_FAR *rgsz_ਨਾਂ,
	    /* [in] */ UINT c_ਨਾਂ,
	    /* [in] */ LCID lcid,
	    /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);

	/* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )(
	    IVim __RPC_FAR * This,
	    /* [in] */ DISPID dispIdMember,
	    /* [in] */ REFIID riid,
	    /* [in] */ LCID lcid,
	    /* [in] */ ਸ਼ਬਦ_2 wFlags,
	    /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
	    /* [out] */ VARIANT __RPC_FAR *p_Var_ਨਤੀਜਾ,
	    /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
	    /* [out] */ UINT __RPC_FAR *puArgErr);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Send_ਕੁੰਜੀਆਂ )(
	    IVim __RPC_FAR * This,
	    /* [in] */ BSTR ਕੁੰਜੀਆਂ);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Eval )(
	    IVim __RPC_FAR * This,
	    /* [in] */ BSTR ਵਿਅੰਜਕ,
	    /* [retval][out] */ BSTR __RPC_FAR *ਨਤੀਜਾ);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ਸੈੱਟ_Foreground )(
	    IVim __RPC_FAR * This);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetHwnd )(
	    IVim __RPC_FAR * This,
	    /* [retval][out] */ UINT_PTR __RPC_FAR *ਨਤੀਜਾ);

	ਸਿਰਾ_INTERFACE
    } IVimVtbl;

    interface IVim
    {
	CONST_VTBL ਬਣਤਰ IVimVtbl __RPC_FAR *lpVtbl;
    };

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ COBJMACROS


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVim_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVim_ਜੋੜੋ_Ref(This)	\
    (This)->lpVtbl -> ਜੋੜੋ_Ref(This)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVim_Release(This)	\
    (This)->lpVtbl -> Release(This)


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVim_Get_Type_Info_ਗਿਣਤੀ(This,pctinfo)	\
    (This)->lpVtbl -> Get_Type_Info_ਗਿਣਤੀ(This,pctinfo)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVim_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVim_Get_IDs_ਦਾ_ਨਾਂ(This,riid,rgsz_ਨਾਂ,c_ਨਾਂ,lcid,rgDispId)	\
    (This)->lpVtbl -> Get_IDs_ਦਾ_ਨਾਂ(This,riid,rgsz_ਨਾਂ,c_ਨਾਂ,lcid,rgDispId)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVim_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,p_Var_ਨਤੀਜਾ,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,p_Var_ਨਤੀਜਾ,pExcepInfo,puArgErr)


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVim_Send_ਕੁੰਜੀਆਂ(This,ਕੁੰਜੀਆਂ)	\
    (This)->lpVtbl -> Send_ਕੁੰਜੀਆਂ(This,ਕੁੰਜੀਆਂ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVim_Eval(This,ਵਿਅੰਜਕ,ਨਤੀਜਾ)	\
    (This)->lpVtbl -> Eval(This,ਵਿਅੰਜਕ,ਨਤੀਜਾ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVim_ਸੈੱਟ_Foreground(This)	\
    (This)->lpVtbl -> ਸੈੱਟ_Foreground(This)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVim_GetHwnd(This,ਨਤੀਜਾ)	\
    (This)->lpVtbl -> GetHwnd(This,ਨਤੀਜਾ)

#ਜੇ_ਅੰਤ // COBJMACROS


#ਜੇ_ਅੰਤ	// C style interface



HRESULT STDMETHODCALLTYPE IVim_Send_ਕੁੰਜੀਆਂ_Proxy(
    IVim __RPC_FAR * This,
    /* [in] */ BSTR ਕੁੰਜੀਆਂ);


ਖਾਲੀ __RPC_STUB IVim_Send_ਕੁੰਜੀਆਂ_Stub(
    IRpc_Stub_ਬਫਰ *This,
    IRpc_Channel_ਬਫਰ *p_Rpc_Channel_ਬਫਰ,
    PRPC_ਸੁਨੇਹਾ p_Rpc_ਸੁਨੇਹਾ,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVim_Eval_Proxy(
    IVim __RPC_FAR * This,
    /* [in] */ BSTR ਵਿਅੰਜਕ,
    /* [retval][out] */ BSTR __RPC_FAR *ਨਤੀਜਾ);


ਖਾਲੀ __RPC_STUB IVim_Eval_Stub(
    IRpc_Stub_ਬਫਰ *This,
    IRpc_Channel_ਬਫਰ *p_Rpc_Channel_ਬਫਰ,
    PRPC_ਸੁਨੇਹਾ p_Rpc_ਸੁਨੇਹਾ,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVim_ਸੈੱਟ_Foreground_Proxy(
    IVim __RPC_FAR * This);


ਖਾਲੀ __RPC_STUB IVim_ਸੈੱਟ_Foreground_Stub(
    IRpc_Stub_ਬਫਰ *This,
    IRpc_Channel_ਬਫਰ *p_Rpc_Channel_ਬਫਰ,
    PRPC_ਸੁਨੇਹਾ p_Rpc_ਸੁਨੇਹਾ,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVim_GetHwnd_Proxy(
    IVim __RPC_FAR * This,
    /* [retval][out] */ UINT_PTR __RPC_FAR *ਨਤੀਜਾ);


ਖਾਲੀ __RPC_STUB IVim_GetHwnd_Stub(
    IRpc_Stub_ਬਫਰ *This,
    IRpc_Channel_ਬਫਰ *p_Rpc_Channel_ਬਫਰ,
    PRPC_ਸੁਨੇਹਾ p_Rpc_ਸੁਨੇਹਾ,
    DWORD *_pdwStubPhase);



#ਜੇ_ਅੰਤ	// __IVim_INTERFACE_DEFINED__



#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __Vim_LIBRARY_DEFINED__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __Vim_LIBRARY_DEFINED__

//***************************************
// Generated header for library: Vim
// at Wed Jun 06 18:20:37 2001
// using MIDL 3.01.75
//**************************************
// [version][helpstring][uuid]



EXTERN_C ਅਚਲ IID LIBID_Vim;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __cplusplus
EXTERN_C ਅਚਲ CLSID CLSID_Vim;

class DECLSPEC_UUID("0F0BFAE1-4C90-11d1-82D7-0004AC368519")
Vim;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ // __Vim_LIBRARY_DEFINED__

// Additional Prototypes for ALL interfaces

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ		  __RPC_USER  BSTR_User_ਮਾਪ(	 ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ __RPC_FAR *, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ	     , BSTR __RPC_FAR * );
ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ __RPC_FAR *, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ __RPC_FAR *, BSTR __RPC_FAR * );
ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ __RPC_FAR *, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ __RPC_FAR *, BSTR __RPC_FAR * );
ਖਾਲੀ			  __RPC_USER  BSTR_UserFree(	 ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ __RPC_FAR *, BSTR __RPC_FAR * );

// end of Additional Prototypes

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __cplusplus
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ
