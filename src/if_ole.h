// this ALWAYS GENERATED file contains the definitions for the interfaces


// File created by MIDL compiler version 3.01.75
// at Wed Jun 06 18:20:37 2001
// Compiler settings for .\if_ole.idl:
//  Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
//  error checks: none
//@@MIDL_FILE_HEADING(  )
#شامل "rpc.h"
#شامل "rpcndr.h"
#اگر_غیر_تعریف_شدہ COM_NO_WINDOWS_H
#شامل "windows.h"
#شامل "ole2.h"
#اگر_ختم //COM_NO_WINDOWS_H

#اگر_غیر_تعریف_شدہ __if_ole_h__
#تعریف_کرو __if_ole_h__

#اگر_تعریف_شدہ __cplusplus
خارجی "C"{
#اگر_ختم

// Forward Declarations

#اگر_غیر_تعریف_شدہ __IVim_FWD_DEFINED__
#تعریف_کرو __IVim_FWD_DEFINED__
قسمتعریف interface IVim IVim;
#اگر_ختم	// __IVim_FWD_DEFINED__


#اگر_غیر_تعریف_شدہ __Vim_FWD_DEFINED__
#تعریف_کرو __Vim_FWD_DEFINED__

#اگر_تعریف_شدہ __cplusplus
قسمتعریف class Vim Vim;
#ورنہ
قسمتعریف ساخت Vim Vim;
#اگر_ختم // __cplusplus

#اگر_ختم	// __Vim_FWD_DEFINED__


// header files for imported files
#شامل "oaidl.h"

#اگر_غیر_تعریف_شدہ __MIDL_user_allocate_free_DEFINED__
#تعریف_کرو __MIDL_user_allocate_free_DEFINED__
    خالی __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
    خالی __RPC_USER MIDL_user_free( خالی __RPC_FAR * );
#اگر_ختم

#اگر_غیر_تعریف_شدہ __IVim_INTERFACE_DEFINED__
#تعریف_کرو __IVim_INTERFACE_DEFINED__

//***************************************
// Generated header for interface: IVim
// at Wed Jun 06 18:20:37 2001
// using MIDL 3.01.75
//**************************************
// [oleautomation][dual][unique][helpstring][uuid][object]



EXTERN_C ثابت IID IID_IVim;

#اگر defined(__cplusplus) && !defined(CINTERFACE)

    interface DECLSPEC_UUID("0F0BFAE2-4C90-11d1-82D7-0004AC368519")
    IVim : عوامی IDispatch
    {
    عوامی:
	مجازی HRESULT STDMETHODCALLTYPE Send_کلیدیں(
	    /* [in] */ BSTR کلیدیں) = 0;

	مجازی HRESULT STDMETHODCALLTYPE Eval(
	    /* [in] */ BSTR اظہاریہ,
	    /* [retval][out] */ BSTR __RPC_FAR *نتیجہ) = 0;

	مجازی HRESULT STDMETHODCALLTYPE سیٹ_Foreground( خالی) = 0;

	مجازی HRESULT STDMETHODCALLTYPE GetHwnd(
	    /* [retval][out] */ UINT_PTR __RPC_FAR *نتیجہ) = 0;

    };

#ورنہ	// C style interface

    قسمتعریف ساخت IVimVtbl
    {
	BEGIN_INTERFACE

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )(
	    IVim __RPC_FAR * This,
	    /* [in] */ REFIID riid,
	    /* [iid_is][out] */ خالی __RPC_FAR *__RPC_FAR *ppvObject);

	ULONG ( STDMETHODCALLTYPE __RPC_FAR *جوڑو_Ref )(
	    IVim __RPC_FAR * This);

	ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )(
	    IVim __RPC_FAR * This);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Get_Type_Info_گنتی )(
	    IVim __RPC_FAR * This,
	    /* [out] */ UINT __RPC_FAR *pctinfo);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )(
	    IVim __RPC_FAR * This,
	    /* [in] */ UINT iTInfo,
	    /* [in] */ LCID lcid,
	    /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Get_IDs_کا_نام )(
	    IVim __RPC_FAR * This,
	    /* [in] */ REFIID riid,
	    /* [size_is][in] */ LPOLESTR __RPC_FAR *rgsz_نام,
	    /* [in] */ UINT c_نام,
	    /* [in] */ LCID lcid,
	    /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);

	/* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )(
	    IVim __RPC_FAR * This,
	    /* [in] */ DISPID dispIdMember,
	    /* [in] */ REFIID riid,
	    /* [in] */ LCID lcid,
	    /* [in] */ لفظ_2 wFlags,
	    /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
	    /* [out] */ VARIANT __RPC_FAR *p_Var_نتیجہ,
	    /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
	    /* [out] */ UINT __RPC_FAR *puArgErr);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Send_کلیدیں )(
	    IVim __RPC_FAR * This,
	    /* [in] */ BSTR کلیدیں);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Eval )(
	    IVim __RPC_FAR * This,
	    /* [in] */ BSTR اظہاریہ,
	    /* [retval][out] */ BSTR __RPC_FAR *نتیجہ);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *سیٹ_Foreground )(
	    IVim __RPC_FAR * This);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetHwnd )(
	    IVim __RPC_FAR * This,
	    /* [retval][out] */ UINT_PTR __RPC_FAR *نتیجہ);

	انتہا_INTERFACE
    } IVimVtbl;

    interface IVim
    {
	CONST_VTBL ساخت IVimVtbl __RPC_FAR *lpVtbl;
    };

#اگر_تعریف_شدہ COBJMACROS


#تعریف_کرو IVim_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#تعریف_کرو IVim_جوڑو_Ref(This)	\
    (This)->lpVtbl -> جوڑو_Ref(This)

#تعریف_کرو IVim_Release(This)	\
    (This)->lpVtbl -> Release(This)


#تعریف_کرو IVim_Get_Type_Info_گنتی(This,pctinfo)	\
    (This)->lpVtbl -> Get_Type_Info_گنتی(This,pctinfo)

#تعریف_کرو IVim_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#تعریف_کرو IVim_Get_IDs_کا_نام(This,riid,rgsz_نام,c_نام,lcid,rgDispId)	\
    (This)->lpVtbl -> Get_IDs_کا_نام(This,riid,rgsz_نام,c_نام,lcid,rgDispId)

#تعریف_کرو IVim_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,p_Var_نتیجہ,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,p_Var_نتیجہ,pExcepInfo,puArgErr)


#تعریف_کرو IVim_Send_کلیدیں(This,کلیدیں)	\
    (This)->lpVtbl -> Send_کلیدیں(This,کلیدیں)

#تعریف_کرو IVim_Eval(This,اظہاریہ,نتیجہ)	\
    (This)->lpVtbl -> Eval(This,اظہاریہ,نتیجہ)

#تعریف_کرو IVim_سیٹ_Foreground(This)	\
    (This)->lpVtbl -> سیٹ_Foreground(This)

#تعریف_کرو IVim_GetHwnd(This,نتیجہ)	\
    (This)->lpVtbl -> GetHwnd(This,نتیجہ)

#اگر_ختم // COBJMACROS


#اگر_ختم	// C style interface



HRESULT STDMETHODCALLTYPE IVim_Send_کلیدیں_Proxy(
    IVim __RPC_FAR * This,
    /* [in] */ BSTR کلیدیں);


خالی __RPC_STUB IVim_Send_کلیدیں_Stub(
    IRpc_Stub_بفر *This,
    IRpc_Channel_بفر *p_Rpc_Channel_بفر,
    PRPC_پیغام p_Rpc_پیغام,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVim_Eval_Proxy(
    IVim __RPC_FAR * This,
    /* [in] */ BSTR اظہاریہ,
    /* [retval][out] */ BSTR __RPC_FAR *نتیجہ);


خالی __RPC_STUB IVim_Eval_Stub(
    IRpc_Stub_بفر *This,
    IRpc_Channel_بفر *p_Rpc_Channel_بفر,
    PRPC_پیغام p_Rpc_پیغام,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVim_سیٹ_Foreground_Proxy(
    IVim __RPC_FAR * This);


خالی __RPC_STUB IVim_سیٹ_Foreground_Stub(
    IRpc_Stub_بفر *This,
    IRpc_Channel_بفر *p_Rpc_Channel_بفر,
    PRPC_پیغام p_Rpc_پیغام,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVim_GetHwnd_Proxy(
    IVim __RPC_FAR * This,
    /* [retval][out] */ UINT_PTR __RPC_FAR *نتیجہ);


خالی __RPC_STUB IVim_GetHwnd_Stub(
    IRpc_Stub_بفر *This,
    IRpc_Channel_بفر *p_Rpc_Channel_بفر,
    PRPC_پیغام p_Rpc_پیغام,
    DWORD *_pdwStubPhase);



#اگر_ختم	// __IVim_INTERFACE_DEFINED__



#اگر_غیر_تعریف_شدہ __Vim_LIBRARY_DEFINED__
#تعریف_کرو __Vim_LIBRARY_DEFINED__

//***************************************
// Generated header for library: Vim
// at Wed Jun 06 18:20:37 2001
// using MIDL 3.01.75
//**************************************
// [version][helpstring][uuid]



EXTERN_C ثابت IID LIBID_Vim;

#اگر_تعریف_شدہ __cplusplus
EXTERN_C ثابت CLSID CLSID_Vim;

class DECLSPEC_UUID("0F0BFAE1-4C90-11d1-82D7-0004AC368519")
Vim;
#اگر_ختم
#اگر_ختم // __Vim_LIBRARY_DEFINED__

// Additional Prototypes for ALL interfaces

غیرعلامتی طویل		  __RPC_USER  BSTR_User_سائز(	 غیرعلامتی طویل __RPC_FAR *, غیرعلامتی طویل	     , BSTR __RPC_FAR * );
غیرعلامتی حرف __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  غیرعلامتی طویل __RPC_FAR *, غیرعلامتی حرف __RPC_FAR *, BSTR __RPC_FAR * );
غیرعلامتی حرف __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(غیرعلامتی طویل __RPC_FAR *, غیرعلامتی حرف __RPC_FAR *, BSTR __RPC_FAR * );
خالی			  __RPC_USER  BSTR_UserFree(	 غیرعلامتی طویل __RPC_FAR *, BSTR __RPC_FAR * );

// end of Additional Prototypes

#اگر_تعریف_شدہ __cplusplus
}
#اگر_ختم

#اگر_ختم
