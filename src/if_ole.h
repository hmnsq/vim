// this ALWAYS GENERATED file contains the definitions for the interfaces


// File created by MIDL compiler version 3.01.75
// at Wed Jun 06 18:20:37 2001
// Compiler settings for .\if_ole.idl:
//  Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
//  error checks: none
//@@MIDL_FILE_HEADING(  )
#ಅನ್ತರ್ಭಾವ "rpc.h"
#ಅನ್ತರ್ಭಾವ "rpcndr.h"
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ COM_NO_WINDOWS_H
#ಅನ್ತರ್ಭಾವ "windows.h"
#ಅನ್ತರ್ಭಾವ "ole2.h"
#ಆದರೆ_ಅಂತ್ಯ //COM_NO_WINDOWS_H

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __if_ole_h__
#ವ್ಯಾಖ್ಯಾನಿಸು __if_ole_h__

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __cplusplus
ಬಾಹ್ಯ "C"{
#ಆದರೆ_ಅಂತ್ಯ

// Forward Declarations

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __IVim_FWD_DEFINED__
#ವ್ಯಾಖ್ಯಾನಿಸು __IVim_FWD_DEFINED__
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ interface IVim IVim;
#ಆದರೆ_ಅಂತ್ಯ	// __IVim_FWD_DEFINED__


#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __Vim_FWD_DEFINED__
#ವ್ಯಾಖ್ಯಾನಿಸು __Vim_FWD_DEFINED__

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __cplusplus
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ class Vim Vim;
#ಇಲ್ಲದಿದ್ದರೆ
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Vim Vim;
#ಆದರೆ_ಅಂತ್ಯ // __cplusplus

#ಆದರೆ_ಅಂತ್ಯ	// __Vim_FWD_DEFINED__


// header files for imported files
#ಅನ್ತರ್ಭಾವ "oaidl.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __MIDL_user_allocate_free_DEFINED__
#ವ್ಯಾಖ್ಯಾನಿಸು __MIDL_user_allocate_free_DEFINED__
    ಶೂನ್ಯ __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
    ಶೂನ್ಯ __RPC_USER MIDL_user_free( ಶೂನ್ಯ __RPC_FAR * );
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __IVim_INTERFACE_DEFINED__
#ವ್ಯಾಖ್ಯಾನಿಸು __IVim_INTERFACE_DEFINED__

//***************************************
// Generated header for interface: IVim
// at Wed Jun 06 18:20:37 2001
// using MIDL 3.01.75
//**************************************
// [oleautomation][dual][unique][helpstring][uuid][object]



EXTERN_C ಸ್ಥಿರಾಂಕ IID IID_IVim;

#ಆದರೆ defined(__cplusplus) && !defined(CINTERFACE)

    interface DECLSPEC_UUID("0F0BFAE2-4C90-11d1-82D7-0004AC368519")
    IVim : ಸಾರ್ವಜನಿಕ IDispatch
    {
    ಸಾರ್ವಜನಿಕ:
	ವಾಸ್ತವಿಕ HRESULT STDMETHODCALLTYPE Send_ಕೀಲಿಗಳು(
	    /* [in] */ BSTR ಕೀಲಿಗಳು) = 0;

	ವಾಸ್ತವಿಕ HRESULT STDMETHODCALLTYPE Eval(
	    /* [in] */ BSTR ಅಭಿವ್ಯಕ್ತಿ,
	    /* [retval][out] */ BSTR __RPC_FAR *ಫಲಿತಾಂಶ) = 0;

	ವಾಸ್ತವಿಕ HRESULT STDMETHODCALLTYPE ಗಣ_Foreground( ಶೂನ್ಯ) = 0;

	ವಾಸ್ತವಿಕ HRESULT STDMETHODCALLTYPE GetHwnd(
	    /* [retval][out] */ UINT_PTR __RPC_FAR *ಫಲಿತಾಂಶ) = 0;

    };

#ಇಲ್ಲದಿದ್ದರೆ	// C style interface

    ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ IVimVtbl
    {
	BEGIN_INTERFACE

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )(
	    IVim __RPC_FAR * This,
	    /* [in] */ REFIID riid,
	    /* [iid_is][out] */ ಶೂನ್ಯ __RPC_FAR *__RPC_FAR *ppvObject);

	ULONG ( STDMETHODCALLTYPE __RPC_FAR *ಕೂಡಿಸು_Ref )(
	    IVim __RPC_FAR * This);

	ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )(
	    IVim __RPC_FAR * This);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Get_Type_Info_ಎಣಿಕೆ )(
	    IVim __RPC_FAR * This,
	    /* [out] */ UINT __RPC_FAR *pctinfo);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )(
	    IVim __RPC_FAR * This,
	    /* [in] */ UINT iTInfo,
	    /* [in] */ LCID lcid,
	    /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Get_IDs_ದ_ಹೆಸರುಗಳು )(
	    IVim __RPC_FAR * This,
	    /* [in] */ REFIID riid,
	    /* [size_is][in] */ LPOLESTR __RPC_FAR *rgsz_ಹೆಸರುಗಳು,
	    /* [in] */ UINT c_ಹೆಸರುಗಳು,
	    /* [in] */ LCID lcid,
	    /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);

	/* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )(
	    IVim __RPC_FAR * This,
	    /* [in] */ DISPID dispIdMember,
	    /* [in] */ REFIID riid,
	    /* [in] */ LCID lcid,
	    /* [in] */ ಶಬ್ದ_2 wFlags,
	    /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
	    /* [out] */ VARIANT __RPC_FAR *p_Var_ಫಲಿತಾಂಶ,
	    /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
	    /* [out] */ UINT __RPC_FAR *puArgErr);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Send_ಕೀಲಿಗಳು )(
	    IVim __RPC_FAR * This,
	    /* [in] */ BSTR ಕೀಲಿಗಳು);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Eval )(
	    IVim __RPC_FAR * This,
	    /* [in] */ BSTR ಅಭಿವ್ಯಕ್ತಿ,
	    /* [retval][out] */ BSTR __RPC_FAR *ಫಲಿತಾಂಶ);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ಗಣ_Foreground )(
	    IVim __RPC_FAR * This);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetHwnd )(
	    IVim __RPC_FAR * This,
	    /* [retval][out] */ UINT_PTR __RPC_FAR *ಫಲಿತಾಂಶ);

	ಕೊನೆ_INTERFACE
    } IVimVtbl;

    interface IVim
    {
	CONST_VTBL ರಚನೆ IVimVtbl __RPC_FAR *lpVtbl;
    };

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ COBJMACROS


#ವ್ಯಾಖ್ಯಾನಿಸು IVim_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#ವ್ಯಾಖ್ಯಾನಿಸು IVim_ಕೂಡಿಸು_Ref(This)	\
    (This)->lpVtbl -> ಕೂಡಿಸು_Ref(This)

#ವ್ಯಾಖ್ಯಾನಿಸು IVim_Release(This)	\
    (This)->lpVtbl -> Release(This)


#ವ್ಯಾಖ್ಯಾನಿಸು IVim_Get_Type_Info_ಎಣಿಕೆ(This,pctinfo)	\
    (This)->lpVtbl -> Get_Type_Info_ಎಣಿಕೆ(This,pctinfo)

#ವ್ಯಾಖ್ಯಾನಿಸು IVim_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#ವ್ಯಾಖ್ಯಾನಿಸು IVim_Get_IDs_ದ_ಹೆಸರುಗಳು(This,riid,rgsz_ಹೆಸರುಗಳು,c_ಹೆಸರುಗಳು,lcid,rgDispId)	\
    (This)->lpVtbl -> Get_IDs_ದ_ಹೆಸರುಗಳು(This,riid,rgsz_ಹೆಸರುಗಳು,c_ಹೆಸರುಗಳು,lcid,rgDispId)

#ವ್ಯಾಖ್ಯಾನಿಸು IVim_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,p_Var_ಫಲಿತಾಂಶ,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,p_Var_ಫಲಿತಾಂಶ,pExcepInfo,puArgErr)


#ವ್ಯಾಖ್ಯಾನಿಸು IVim_Send_ಕೀಲಿಗಳು(This,ಕೀಲಿಗಳು)	\
    (This)->lpVtbl -> Send_ಕೀಲಿಗಳು(This,ಕೀಲಿಗಳು)

#ವ್ಯಾಖ್ಯಾನಿಸು IVim_Eval(This,ಅಭಿವ್ಯಕ್ತಿ,ಫಲಿತಾಂಶ)	\
    (This)->lpVtbl -> Eval(This,ಅಭಿವ್ಯಕ್ತಿ,ಫಲಿತಾಂಶ)

#ವ್ಯಾಖ್ಯಾನಿಸು IVim_ಗಣ_Foreground(This)	\
    (This)->lpVtbl -> ಗಣ_Foreground(This)

#ವ್ಯಾಖ್ಯಾನಿಸು IVim_GetHwnd(This,ಫಲಿತಾಂಶ)	\
    (This)->lpVtbl -> GetHwnd(This,ಫಲಿತಾಂಶ)

#ಆದರೆ_ಅಂತ್ಯ // COBJMACROS


#ಆದರೆ_ಅಂತ್ಯ	// C style interface



HRESULT STDMETHODCALLTYPE IVim_Send_ಕೀಲಿಗಳು_Proxy(
    IVim __RPC_FAR * This,
    /* [in] */ BSTR ಕೀಲಿಗಳು);


ಶೂನ್ಯ __RPC_STUB IVim_Send_ಕೀಲಿಗಳು_Stub(
    IRpc_Stub_ಬಫರ್ *This,
    IRpc_Channel_ಬಫರ್ *p_Rpc_Channel_ಬಫರ್,
    PRPC_ಸಂದೇಶ p_Rpc_ಸಂದೇಶ,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVim_Eval_Proxy(
    IVim __RPC_FAR * This,
    /* [in] */ BSTR ಅಭಿವ್ಯಕ್ತಿ,
    /* [retval][out] */ BSTR __RPC_FAR *ಫಲಿತಾಂಶ);


ಶೂನ್ಯ __RPC_STUB IVim_Eval_Stub(
    IRpc_Stub_ಬಫರ್ *This,
    IRpc_Channel_ಬಫರ್ *p_Rpc_Channel_ಬಫರ್,
    PRPC_ಸಂದೇಶ p_Rpc_ಸಂದೇಶ,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVim_ಗಣ_Foreground_Proxy(
    IVim __RPC_FAR * This);


ಶೂನ್ಯ __RPC_STUB IVim_ಗಣ_Foreground_Stub(
    IRpc_Stub_ಬಫರ್ *This,
    IRpc_Channel_ಬಫರ್ *p_Rpc_Channel_ಬಫರ್,
    PRPC_ಸಂದೇಶ p_Rpc_ಸಂದೇಶ,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVim_GetHwnd_Proxy(
    IVim __RPC_FAR * This,
    /* [retval][out] */ UINT_PTR __RPC_FAR *ಫಲಿತಾಂಶ);


ಶೂನ್ಯ __RPC_STUB IVim_GetHwnd_Stub(
    IRpc_Stub_ಬಫರ್ *This,
    IRpc_Channel_ಬಫರ್ *p_Rpc_Channel_ಬಫರ್,
    PRPC_ಸಂದೇಶ p_Rpc_ಸಂದೇಶ,
    DWORD *_pdwStubPhase);



#ಆದರೆ_ಅಂತ್ಯ	// __IVim_INTERFACE_DEFINED__



#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __Vim_LIBRARY_DEFINED__
#ವ್ಯಾಖ್ಯಾನಿಸು __Vim_LIBRARY_DEFINED__

//***************************************
// Generated header for library: Vim
// at Wed Jun 06 18:20:37 2001
// using MIDL 3.01.75
//**************************************
// [version][helpstring][uuid]



EXTERN_C ಸ್ಥಿರಾಂಕ IID LIBID_Vim;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __cplusplus
EXTERN_C ಸ್ಥಿರಾಂಕ CLSID CLSID_Vim;

class DECLSPEC_UUID("0F0BFAE1-4C90-11d1-82D7-0004AC368519")
Vim;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ // __Vim_LIBRARY_DEFINED__

// Additional Prototypes for ALL interfaces

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ		  __RPC_USER  BSTR_User_ಗಾತ್ರ(	 ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ __RPC_FAR *, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ	     , BSTR __RPC_FAR * );
ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ __RPC_FAR *, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ __RPC_FAR *, BSTR __RPC_FAR * );
ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ __RPC_FAR *, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ __RPC_FAR *, BSTR __RPC_FAR * );
ಶೂನ್ಯ			  __RPC_USER  BSTR_UserFree(	 ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ __RPC_FAR *, BSTR __RPC_FAR * );

// end of Additional Prototypes

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __cplusplus
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ
