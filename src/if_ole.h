// this ALWAYS GENERATED file contains the definitions for the interfaces


// File created by MIDL compiler version 3.01.75
// at Wed Jun 06 18:20:37 2001
// Compiler settings for .\if_ole.idl:
//  Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
//  error checks: none
//@@MIDL_FILE_HEADING(  )
#અન્તર્ભાવ "rpc.h"
#અન્તર્ભાવ "rpcndr.h"
#ifndef COM_NO_WINDOWS_H
#અન્તર્ભાવ "windows.h"
#અન્તર્ભાવ "ole2.h"
#endif //COM_NO_WINDOWS_H

#ifndef __if_ole_h__
#define __if_ole_h__

#ifdef __cplusplus
બાહ્ય "C"{
#endif

// Forward Declarations

#ifndef __IVim_FWD_DEFINED__
#define __IVim_FWD_DEFINED__
પ્રકારવ્યાખ્યા interface IVim IVim;
#endif	// __IVim_FWD_DEFINED__


#ifndef __Vim_FWD_DEFINED__
#define __Vim_FWD_DEFINED__

#ifdef __cplusplus
પ્રકારવ્યાખ્યા class Vim Vim;
#નહીંતર
પ્રકારવ્યાખ્યા સંરચના Vim Vim;
#endif // __cplusplus

#endif	// __Vim_FWD_DEFINED__


// header files for imported files
#અન્તર્ભાવ "oaidl.h"

#ifndef __MIDL_user_allocate_free_DEFINED__
#define __MIDL_user_allocate_free_DEFINED__
    શૂન્ય __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
    શૂન્ય __RPC_USER MIDL_user_free( શૂન્ય __RPC_FAR * );
#endif

#ifndef __IVim_INTERFACE_DEFINED__
#define __IVim_INTERFACE_DEFINED__

//***************************************
// Generated header for interface: IVim
// at Wed Jun 06 18:20:37 2001
// using MIDL 3.01.75
//**************************************
// [oleautomation][dual][unique][helpstring][uuid][object]



EXTERN_C અચલ IID IID_IVim;

#જો defined(__cplusplus) && !defined(CINTERFACE)

    interface DECLSPEC_UUID("0F0BFAE2-4C90-11d1-82D7-0004AC368519")
    IVim : જાહેર IDispatch
    {
    જાહેર:
	આભાસી HRESULT STDMETHODCALLTYPE Send_ચાવીઓ(
	    /* [in] */ BSTR ચાવીઓ) = 0;

	આભાસી HRESULT STDMETHODCALLTYPE Eval(
	    /* [in] */ BSTR પદાવલિ,
	    /* [retval][out] */ BSTR __RPC_FAR *result) = 0;

	આભાસી HRESULT STDMETHODCALLTYPE SetForeground( શૂન્ય) = 0;

	આભાસી HRESULT STDMETHODCALLTYPE GetHwnd(
	    /* [retval][out] */ UINT_PTR __RPC_FAR *result) = 0;

    };

#નહીંતર	// C style interface

    પ્રકારવ્યાખ્યા સંરચના IVimVtbl
    {
	BEGIN_INTERFACE

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )(
	    IVim __RPC_FAR * This,
	    /* [in] */ REFIID riid,
	    /* [iid_is][out] */ શૂન્ય __RPC_FAR *__RPC_FAR *ppvObject);

	ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )(
	    IVim __RPC_FAR * This);

	ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )(
	    IVim __RPC_FAR * This);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Get_Type_Info_ગણતરી )(
	    IVim __RPC_FAR * This,
	    /* [out] */ UINT __RPC_FAR *pctinfo);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )(
	    IVim __RPC_FAR * This,
	    /* [in] */ UINT iTInfo,
	    /* [in] */ LCID lcid,
	    /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )(
	    IVim __RPC_FAR * This,
	    /* [in] */ REFIID riid,
	    /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
	    /* [in] */ UINT cNames,
	    /* [in] */ LCID lcid,
	    /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);

	/* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )(
	    IVim __RPC_FAR * This,
	    /* [in] */ DISPID dispIdMember,
	    /* [in] */ REFIID riid,
	    /* [in] */ LCID lcid,
	    /* [in] */ WORD wFlags,
	    /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
	    /* [out] */ VARIANT __RPC_FAR *pVarResult,
	    /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
	    /* [out] */ UINT __RPC_FAR *puArgErr);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Send_ચાવીઓ )(
	    IVim __RPC_FAR * This,
	    /* [in] */ BSTR ચાવીઓ);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Eval )(
	    IVim __RPC_FAR * This,
	    /* [in] */ BSTR પદાવલિ,
	    /* [retval][out] */ BSTR __RPC_FAR *result);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetForeground )(
	    IVim __RPC_FAR * This);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetHwnd )(
	    IVim __RPC_FAR * This,
	    /* [retval][out] */ UINT_PTR __RPC_FAR *result);

	END_INTERFACE
    } IVimVtbl;

    interface IVim
    {
	CONST_VTBL સંરચના IVimVtbl __RPC_FAR *lpVtbl;
    };

#ifdef COBJMACROS


#define IVim_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IVim_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IVim_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IVim_Get_Type_Info_ગણતરી(This,pctinfo)	\
    (This)->lpVtbl -> Get_Type_Info_ગણતરી(This,pctinfo)

#define IVim_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IVim_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IVim_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IVim_Send_ચાવીઓ(This,ચાવીઓ)	\
    (This)->lpVtbl -> Send_ચાવીઓ(This,ચાવીઓ)

#define IVim_Eval(This,પદાવલિ,result)	\
    (This)->lpVtbl -> Eval(This,પદાવલિ,result)

#define IVim_SetForeground(This)	\
    (This)->lpVtbl -> SetForeground(This)

#define IVim_GetHwnd(This,result)	\
    (This)->lpVtbl -> GetHwnd(This,result)

#endif // COBJMACROS


#endif	// C style interface



HRESULT STDMETHODCALLTYPE IVim_Send_ચાવીઓ_Proxy(
    IVim __RPC_FAR * This,
    /* [in] */ BSTR ચાવીઓ);


શૂન્ય __RPC_STUB IVim_Send_ચાવીઓ_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVim_Eval_Proxy(
    IVim __RPC_FAR * This,
    /* [in] */ BSTR પદાવલિ,
    /* [retval][out] */ BSTR __RPC_FAR *result);


શૂન્ય __RPC_STUB IVim_Eval_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVim_SetForeground_Proxy(
    IVim __RPC_FAR * This);


શૂન્ય __RPC_STUB IVim_SetForeground_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVim_GetHwnd_Proxy(
    IVim __RPC_FAR * This,
    /* [retval][out] */ UINT_PTR __RPC_FAR *result);


શૂન્ય __RPC_STUB IVim_GetHwnd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif	// __IVim_INTERFACE_DEFINED__



#ifndef __Vim_LIBRARY_DEFINED__
#define __Vim_LIBRARY_DEFINED__

//***************************************
// Generated header for library: Vim
// at Wed Jun 06 18:20:37 2001
// using MIDL 3.01.75
//**************************************
// [version][helpstring][uuid]



EXTERN_C અચલ IID LIBID_Vim;

#ifdef __cplusplus
EXTERN_C અચલ CLSID CLSID_Vim;

class DECLSPEC_UUID("0F0BFAE1-4C90-11d1-82D7-0004AC368519")
Vim;
#endif
#endif // __Vim_LIBRARY_DEFINED__

// Additional Prototypes for ALL interfaces

unsigned long		  __RPC_USER  BSTR_UserSize(	 unsigned long __RPC_FAR *, unsigned long	     , BSTR __RPC_FAR * );
unsigned અક્ષર __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  unsigned long __RPC_FAR *, unsigned અક્ષર __RPC_FAR *, BSTR __RPC_FAR * );
unsigned અક્ષર __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(unsigned long __RPC_FAR *, unsigned અક્ષર __RPC_FAR *, BSTR __RPC_FAR * );
શૂન્ય			  __RPC_USER  BSTR_UserFree(	 unsigned long __RPC_FAR *, BSTR __RPC_FAR * );

// end of Additional Prototypes

#ifdef __cplusplus
}
#endif

#endif
