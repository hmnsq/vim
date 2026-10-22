// this ALWAYS GENERATED file contains the definitions for the interfaces


// File created by MIDL compiler version 3.01.75
// at Wed Jun 06 18:20:37 2001
// Compiler settings for .\if_ole.idl:
//  Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
//  error checks: none
//@@MIDL_FILE_HEADING(  )
#ضمن "rpc.h"
#ضمن "rpcndr.h"
#که_نه_تعریف_شوی COM_NO_WINDOWS_H
#ضمن "windows.h"
#ضمن "ole2.h"
#د_که_پای //COM_NO_WINDOWS_H

#که_نه_تعریف_شوی __if_ole_h__
#تعریف_کړه __if_ole_h__

#که_تعریف_شوی __cplusplus
خارجي "C"{
#د_که_پای

// Forward Declarations

#که_نه_تعریف_شوی __IVim_FWD_DEFINED__
#تعریف_کړه __IVim_FWD_DEFINED__
تعريفنوع interface IVim IVim;
#د_که_پای	// __IVim_FWD_DEFINED__


#که_نه_تعریف_شوی __Vim_FWD_DEFINED__
#تعریف_کړه __Vim_FWD_DEFINED__

#که_تعریف_شوی __cplusplus
تعريفنوع class Vim Vim;
#وإلا
تعريفنوع بنية Vim Vim;
#د_که_پای // __cplusplus

#د_که_پای	// __Vim_FWD_DEFINED__


// header files for imported files
#ضمن "oaidl.h"

#که_نه_تعریف_شوی __MIDL_user_allocate_free_DEFINED__
#تعریف_کړه __MIDL_user_allocate_free_DEFINED__
    فراغ __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
    فراغ __RPC_USER MIDL_user_free( فراغ __RPC_FAR * );
#د_که_پای

#که_نه_تعریف_شوی __IVim_INTERFACE_DEFINED__
#تعریف_کړه __IVim_INTERFACE_DEFINED__

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
	مجازي HRESULT STDMETHODCALLTYPE Send_کلیګانې(
	    /* [in] */ BSTR کلیګانې) = 0;

	مجازي HRESULT STDMETHODCALLTYPE Eval(
	    /* [in] */ BSTR بیان,
	    /* [retval][out] */ BSTR __RPC_FAR *پایله) = 0;

	مجازي HRESULT STDMETHODCALLTYPE سېټ_Foreground( فراغ) = 0;

	مجازي HRESULT STDMETHODCALLTYPE GetHwnd(
	    /* [retval][out] */ UINT_PTR __RPC_FAR *پایله) = 0;

    };

#وإلا	// C style interface

    تعريفنوع بنية IVimVtbl
    {
	BEGIN_INTERFACE

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )(
	    IVim __RPC_FAR * This,
	    /* [in] */ REFIID riid,
	    /* [iid_is][out] */ فراغ __RPC_FAR *__RPC_FAR *ppvObject);

	ULONG ( STDMETHODCALLTYPE __RPC_FAR *جمع_کړه_Ref )(
	    IVim __RPC_FAR * This);

	ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )(
	    IVim __RPC_FAR * This);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Get_Type_Info_شمېر )(
	    IVim __RPC_FAR * This,
	    /* [out] */ UINT __RPC_FAR *pctinfo);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )(
	    IVim __RPC_FAR * This,
	    /* [in] */ UINT iTInfo,
	    /* [in] */ LCID lcid,
	    /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Get_IDs_د_نومونه )(
	    IVim __RPC_FAR * This,
	    /* [in] */ REFIID riid,
	    /* [size_is][in] */ LPOLESTR __RPC_FAR *rgsz_نومونه,
	    /* [in] */ UINT c_نومونه,
	    /* [in] */ LCID lcid,
	    /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);

	/* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )(
	    IVim __RPC_FAR * This,
	    /* [in] */ DISPID dispIdMember,
	    /* [in] */ REFIID riid,
	    /* [in] */ LCID lcid,
	    /* [in] */ کلمه_2 wFlags,
	    /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
	    /* [out] */ VARIANT __RPC_FAR *p_Var_پایله,
	    /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
	    /* [out] */ UINT __RPC_FAR *puArgErr);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Send_کلیګانې )(
	    IVim __RPC_FAR * This,
	    /* [in] */ BSTR کلیګانې);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Eval )(
	    IVim __RPC_FAR * This,
	    /* [in] */ BSTR بیان,
	    /* [retval][out] */ BSTR __RPC_FAR *پایله);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *سېټ_Foreground )(
	    IVim __RPC_FAR * This);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetHwnd )(
	    IVim __RPC_FAR * This,
	    /* [retval][out] */ UINT_PTR __RPC_FAR *پایله);

	پای_ټکی_INTERFACE
    } IVimVtbl;

    interface IVim
    {
	CONST_VTBL بنية IVimVtbl __RPC_FAR *lpVtbl;
    };

#که_تعریف_شوی COBJMACROS


#تعریف_کړه IVim_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#تعریف_کړه IVim_جمع_کړه_Ref(This)	\
    (This)->lpVtbl -> جمع_کړه_Ref(This)

#تعریف_کړه IVim_Release(This)	\
    (This)->lpVtbl -> Release(This)


#تعریف_کړه IVim_Get_Type_Info_شمېر(This,pctinfo)	\
    (This)->lpVtbl -> Get_Type_Info_شمېر(This,pctinfo)

#تعریف_کړه IVim_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#تعریف_کړه IVim_Get_IDs_د_نومونه(This,riid,rgsz_نومونه,c_نومونه,lcid,rgDispId)	\
    (This)->lpVtbl -> Get_IDs_د_نومونه(This,riid,rgsz_نومونه,c_نومونه,lcid,rgDispId)

#تعریف_کړه IVim_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,p_Var_پایله,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,p_Var_پایله,pExcepInfo,puArgErr)


#تعریف_کړه IVim_Send_کلیګانې(This,کلیګانې)	\
    (This)->lpVtbl -> Send_کلیګانې(This,کلیګانې)

#تعریف_کړه IVim_Eval(This,بیان,پایله)	\
    (This)->lpVtbl -> Eval(This,بیان,پایله)

#تعریف_کړه IVim_سېټ_Foreground(This)	\
    (This)->lpVtbl -> سېټ_Foreground(This)

#تعریف_کړه IVim_GetHwnd(This,پایله)	\
    (This)->lpVtbl -> GetHwnd(This,پایله)

#د_که_پای // COBJMACROS


#د_که_پای	// C style interface



HRESULT STDMETHODCALLTYPE IVim_Send_کلیګانې_Proxy(
    IVim __RPC_FAR * This,
    /* [in] */ BSTR کلیګانې);


فراغ __RPC_STUB IVim_Send_کلیګانې_Stub(
    IRpc_Stub_بفر *This,
    IRpc_Channel_بفر *p_Rpc_Channel_بفر,
    PRPC_پیغام p_Rpc_پیغام,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVim_Eval_Proxy(
    IVim __RPC_FAR * This,
    /* [in] */ BSTR بیان,
    /* [retval][out] */ BSTR __RPC_FAR *پایله);


فراغ __RPC_STUB IVim_Eval_Stub(
    IRpc_Stub_بفر *This,
    IRpc_Channel_بفر *p_Rpc_Channel_بفر,
    PRPC_پیغام p_Rpc_پیغام,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVim_سېټ_Foreground_Proxy(
    IVim __RPC_FAR * This);


فراغ __RPC_STUB IVim_سېټ_Foreground_Stub(
    IRpc_Stub_بفر *This,
    IRpc_Channel_بفر *p_Rpc_Channel_بفر,
    PRPC_پیغام p_Rpc_پیغام,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVim_GetHwnd_Proxy(
    IVim __RPC_FAR * This,
    /* [retval][out] */ UINT_PTR __RPC_FAR *پایله);


فراغ __RPC_STUB IVim_GetHwnd_Stub(
    IRpc_Stub_بفر *This,
    IRpc_Channel_بفر *p_Rpc_Channel_بفر,
    PRPC_پیغام p_Rpc_پیغام,
    DWORD *_pdwStubPhase);



#د_که_پای	// __IVim_INTERFACE_DEFINED__



#که_نه_تعریف_شوی __Vim_LIBRARY_DEFINED__
#تعریف_کړه __Vim_LIBRARY_DEFINED__

//***************************************
// Generated header for library: Vim
// at Wed Jun 06 18:20:37 2001
// using MIDL 3.01.75
//**************************************
// [version][helpstring][uuid]



EXTERN_C ثابت IID LIBID_Vim;

#که_تعریف_شوی __cplusplus
EXTERN_C ثابت CLSID CLSID_Vim;

class DECLSPEC_UUID("0F0BFAE1-4C90-11d1-82D7-0004AC368519")
Vim;
#د_که_پای
#د_که_پای // __Vim_LIBRARY_DEFINED__

// Additional Prototypes for ALL interfaces

بینښه اوږد		  __RPC_USER  BSTR_User_اندازه(	 بینښه اوږد __RPC_FAR *, بینښه اوږد	     , BSTR __RPC_FAR * );
بینښه حرف __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  بینښه اوږد __RPC_FAR *, بینښه حرف __RPC_FAR *, BSTR __RPC_FAR * );
بینښه حرف __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(بینښه اوږد __RPC_FAR *, بینښه حرف __RPC_FAR *, BSTR __RPC_FAR * );
فراغ			  __RPC_USER  BSTR_UserFree(	 بینښه اوږد __RPC_FAR *, BSTR __RPC_FAR * );

// end of Additional Prototypes

#که_تعریف_شوی __cplusplus
}
#د_که_پای

#د_که_پای
