// this ALWAYS GENERATED file contains the definitions for the interfaces


// File created by MIDL compiler version 3.01.75
// at Wed Jun 06 18:20:37 2001
// Compiler settings for .\if_ole.idl:
//  Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
//  error checks: none
//@@MIDL_FILE_HEADING(  )
#ضمن "rpc.h"
#ضمن "rpcndr.h"
#جيڪڏهن_وصف_نه_ٿيل COM_NO_WINDOWS_H
#ضمن "windows.h"
#ضمن "ole2.h"
#جيڪڏهن_پورو //COM_NO_WINDOWS_H

#جيڪڏهن_وصف_نه_ٿيل __if_ole_h__
#وصف_ڪريو __if_ole_h__

#جيڪڏهن_وصف_ٿيل __cplusplus
خارجي "C"{
#جيڪڏهن_پورو

// Forward Declarations

#جيڪڏهن_وصف_نه_ٿيل __IVim_FWD_DEFINED__
#وصف_ڪريو __IVim_FWD_DEFINED__
تعريفنوع interface IVim IVim;
#جيڪڏهن_پورو	// __IVim_FWD_DEFINED__


#جيڪڏهن_وصف_نه_ٿيل __Vim_FWD_DEFINED__
#وصف_ڪريو __Vim_FWD_DEFINED__

#جيڪڏهن_وصف_ٿيل __cplusplus
تعريفنوع class Vim Vim;
#وإلا
تعريفنوع بنية Vim Vim;
#جيڪڏهن_پورو // __cplusplus

#جيڪڏهن_پورو	// __Vim_FWD_DEFINED__


// header files for imported files
#ضمن "oaidl.h"

#جيڪڏهن_وصف_نه_ٿيل __MIDL_user_allocate_free_DEFINED__
#وصف_ڪريو __MIDL_user_allocate_free_DEFINED__
    فراغ __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
    فراغ __RPC_USER MIDL_user_free( فراغ __RPC_FAR * );
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل __IVim_INTERFACE_DEFINED__
#وصف_ڪريو __IVim_INTERFACE_DEFINED__

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
	مجازي HRESULT STDMETHODCALLTYPE Send_ڪنجيون(
	    /* [in] */ BSTR ڪنجيون) = 0;

	مجازي HRESULT STDMETHODCALLTYPE Eval(
	    /* [in] */ BSTR اظهار,
	    /* [retval][out] */ BSTR __RPC_FAR *نتيجو) = 0;

	مجازي HRESULT STDMETHODCALLTYPE سيٽ_Foreground( فراغ) = 0;

	مجازي HRESULT STDMETHODCALLTYPE GetHwnd(
	    /* [retval][out] */ UINT_PTR __RPC_FAR *نتيجو) = 0;

    };

#وإلا	// C style interface

    تعريفنوع بنية IVimVtbl
    {
	BEGIN_INTERFACE

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )(
	    IVim __RPC_FAR * This,
	    /* [in] */ REFIID riid,
	    /* [iid_is][out] */ فراغ __RPC_FAR *__RPC_FAR *ppvObject);

	ULONG ( STDMETHODCALLTYPE __RPC_FAR *جوڙيو_Ref )(
	    IVim __RPC_FAR * This);

	ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )(
	    IVim __RPC_FAR * This);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Get_Type_Info_ڳڻپ )(
	    IVim __RPC_FAR * This,
	    /* [out] */ UINT __RPC_FAR *pctinfo);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )(
	    IVim __RPC_FAR * This,
	    /* [in] */ UINT iTInfo,
	    /* [in] */ LCID lcid,
	    /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Get_IDs_جو_نالا )(
	    IVim __RPC_FAR * This,
	    /* [in] */ REFIID riid,
	    /* [size_is][in] */ LPOLESTR __RPC_FAR *rgsz_نالا,
	    /* [in] */ UINT c_نالا,
	    /* [in] */ LCID lcid,
	    /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);

	/* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )(
	    IVim __RPC_FAR * This,
	    /* [in] */ DISPID dispIdMember,
	    /* [in] */ REFIID riid,
	    /* [in] */ LCID lcid,
	    /* [in] */ لفظ_2 wFlags,
	    /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
	    /* [out] */ VARIANT __RPC_FAR *p_Var_نتيجو,
	    /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
	    /* [out] */ UINT __RPC_FAR *puArgErr);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Send_ڪنجيون )(
	    IVim __RPC_FAR * This,
	    /* [in] */ BSTR ڪنجيون);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Eval )(
	    IVim __RPC_FAR * This,
	    /* [in] */ BSTR اظهار,
	    /* [retval][out] */ BSTR __RPC_FAR *نتيجو);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *سيٽ_Foreground )(
	    IVim __RPC_FAR * This);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetHwnd )(
	    IVim __RPC_FAR * This,
	    /* [retval][out] */ UINT_PTR __RPC_FAR *نتيجو);

	پڇاڙي_INTERFACE
    } IVimVtbl;

    interface IVim
    {
	CONST_VTBL بنية IVimVtbl __RPC_FAR *lpVtbl;
    };

#جيڪڏهن_وصف_ٿيل COBJMACROS


#وصف_ڪريو IVim_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#وصف_ڪريو IVim_جوڙيو_Ref(This)	\
    (This)->lpVtbl -> جوڙيو_Ref(This)

#وصف_ڪريو IVim_Release(This)	\
    (This)->lpVtbl -> Release(This)


#وصف_ڪريو IVim_Get_Type_Info_ڳڻپ(This,pctinfo)	\
    (This)->lpVtbl -> Get_Type_Info_ڳڻپ(This,pctinfo)

#وصف_ڪريو IVim_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#وصف_ڪريو IVim_Get_IDs_جو_نالا(This,riid,rgsz_نالا,c_نالا,lcid,rgDispId)	\
    (This)->lpVtbl -> Get_IDs_جو_نالا(This,riid,rgsz_نالا,c_نالا,lcid,rgDispId)

#وصف_ڪريو IVim_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,p_Var_نتيجو,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,p_Var_نتيجو,pExcepInfo,puArgErr)


#وصف_ڪريو IVim_Send_ڪنجيون(This,ڪنجيون)	\
    (This)->lpVtbl -> Send_ڪنجيون(This,ڪنجيون)

#وصف_ڪريو IVim_Eval(This,اظهار,نتيجو)	\
    (This)->lpVtbl -> Eval(This,اظهار,نتيجو)

#وصف_ڪريو IVim_سيٽ_Foreground(This)	\
    (This)->lpVtbl -> سيٽ_Foreground(This)

#وصف_ڪريو IVim_GetHwnd(This,نتيجو)	\
    (This)->lpVtbl -> GetHwnd(This,نتيجو)

#جيڪڏهن_پورو // COBJMACROS


#جيڪڏهن_پورو	// C style interface



HRESULT STDMETHODCALLTYPE IVim_Send_ڪنجيون_Proxy(
    IVim __RPC_FAR * This,
    /* [in] */ BSTR ڪنجيون);


فراغ __RPC_STUB IVim_Send_ڪنجيون_Stub(
    IRpc_Stub_بفر *This,
    IRpc_Channel_بفر *p_Rpc_Channel_بفر,
    PRPC_پيغام p_Rpc_پيغام,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVim_Eval_Proxy(
    IVim __RPC_FAR * This,
    /* [in] */ BSTR اظهار,
    /* [retval][out] */ BSTR __RPC_FAR *نتيجو);


فراغ __RPC_STUB IVim_Eval_Stub(
    IRpc_Stub_بفر *This,
    IRpc_Channel_بفر *p_Rpc_Channel_بفر,
    PRPC_پيغام p_Rpc_پيغام,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVim_سيٽ_Foreground_Proxy(
    IVim __RPC_FAR * This);


فراغ __RPC_STUB IVim_سيٽ_Foreground_Stub(
    IRpc_Stub_بفر *This,
    IRpc_Channel_بفر *p_Rpc_Channel_بفر,
    PRPC_پيغام p_Rpc_پيغام,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVim_GetHwnd_Proxy(
    IVim __RPC_FAR * This,
    /* [retval][out] */ UINT_PTR __RPC_FAR *نتيجو);


فراغ __RPC_STUB IVim_GetHwnd_Stub(
    IRpc_Stub_بفر *This,
    IRpc_Channel_بفر *p_Rpc_Channel_بفر,
    PRPC_پيغام p_Rpc_پيغام,
    DWORD *_pdwStubPhase);



#جيڪڏهن_پورو	// __IVim_INTERFACE_DEFINED__



#جيڪڏهن_وصف_نه_ٿيل __Vim_LIBRARY_DEFINED__
#وصف_ڪريو __Vim_LIBRARY_DEFINED__

//***************************************
// Generated header for library: Vim
// at Wed Jun 06 18:20:37 2001
// using MIDL 3.01.75
//**************************************
// [version][helpstring][uuid]



EXTERN_C ثابت IID LIBID_Vim;

#جيڪڏهن_وصف_ٿيل __cplusplus
EXTERN_C ثابت CLSID CLSID_Vim;

class DECLSPEC_UUID("0F0BFAE1-4C90-11d1-82D7-0004AC368519")
Vim;
#جيڪڏهن_پورو
#جيڪڏهن_پورو // __Vim_LIBRARY_DEFINED__

// Additional Prototypes for ALL interfaces

بنانشان ڊگهو		  __RPC_USER  BSTR_User_سائيز(	 بنانشان ڊگهو __RPC_FAR *, بنانشان ڊگهو	     , BSTR __RPC_FAR * );
بنانشان حرف __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  بنانشان ڊگهو __RPC_FAR *, بنانشان حرف __RPC_FAR *, BSTR __RPC_FAR * );
بنانشان حرف __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(بنانشان ڊگهو __RPC_FAR *, بنانشان حرف __RPC_FAR *, BSTR __RPC_FAR * );
فراغ			  __RPC_USER  BSTR_UserFree(	 بنانشان ڊگهو __RPC_FAR *, BSTR __RPC_FAR * );

// end of Additional Prototypes

#جيڪڏهن_وصف_ٿيل __cplusplus
}
#جيڪڏهن_پورو

#جيڪڏهن_پورو
