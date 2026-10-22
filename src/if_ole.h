// this ALWAYS GENERATED file contains the definitions for the interfaces


// File created by MIDL compiler version 3.01.75
// at Wed Jun 06 18:20:37 2001
// Compiler settings for .\if_ole.idl:
//  Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
//  error checks: none
//@@MIDL_FILE_HEADING(  )
#అన్తర్భావ "rpc.h"
#అన్తర్భావ "rpcndr.h"
#నిర్వచితం_కాకపోతే COM_NO_WINDOWS_H
#అన్తర్భావ "windows.h"
#అన్తర్భావ "ole2.h"
#అయితే_ముగింపు //COM_NO_WINDOWS_H

#నిర్వచితం_కాకపోతే __if_ole_h__
#నిర్వచించు __if_ole_h__

#నిర్వచితమైతే __cplusplus
బాహ్య "C"{
#అయితే_ముగింపు

// Forward Declarations

#నిర్వచితం_కాకపోతే __IVim_FWD_DEFINED__
#నిర్వచించు __IVim_FWD_DEFINED__
రకనిర్వచనం interface IVim IVim;
#అయితే_ముగింపు	// __IVim_FWD_DEFINED__


#నిర్వచితం_కాకపోతే __Vim_FWD_DEFINED__
#నిర్వచించు __Vim_FWD_DEFINED__

#నిర్వచితమైతే __cplusplus
రకనిర్వచనం class Vim Vim;
#లేకపోతే
రకనిర్వచనం నిర్మాణం Vim Vim;
#అయితే_ముగింపు // __cplusplus

#అయితే_ముగింపు	// __Vim_FWD_DEFINED__


// header files for imported files
#అన్తర్భావ "oaidl.h"

#నిర్వచితం_కాకపోతే __MIDL_user_allocate_free_DEFINED__
#నిర్వచించు __MIDL_user_allocate_free_DEFINED__
    శూన్య __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
    శూన్య __RPC_USER MIDL_user_free( శూన్య __RPC_FAR * );
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే __IVim_INTERFACE_DEFINED__
#నిర్వచించు __IVim_INTERFACE_DEFINED__

//***************************************
// Generated header for interface: IVim
// at Wed Jun 06 18:20:37 2001
// using MIDL 3.01.75
//**************************************
// [oleautomation][dual][unique][helpstring][uuid][object]



EXTERN_C స్థిరరాశి IID IID_IVim;

#ఒకవేళ defined(__cplusplus) && !defined(CINTERFACE)

    interface DECLSPEC_UUID("0F0BFAE2-4C90-11d1-82D7-0004AC368519")
    IVim : బహిరంగ IDispatch
    {
    బహిరంగ:
	మాయా HRESULT STDMETHODCALLTYPE Send_కీలకాలు(
	    /* [in] */ BSTR కీలకాలు) = 0;

	మాయా HRESULT STDMETHODCALLTYPE Eval(
	    /* [in] */ BSTR సమాసం,
	    /* [retval][out] */ BSTR __RPC_FAR *ఫలితం) = 0;

	మాయా HRESULT STDMETHODCALLTYPE సమితి_Foreground( శూన్య) = 0;

	మాయా HRESULT STDMETHODCALLTYPE GetHwnd(
	    /* [retval][out] */ UINT_PTR __RPC_FAR *ఫలితం) = 0;

    };

#లేకపోతే	// C style interface

    రకనిర్వచనం నిర్మాణం IVimVtbl
    {
	BEGIN_INTERFACE

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )(
	    IVim __RPC_FAR * This,
	    /* [in] */ REFIID riid,
	    /* [iid_is][out] */ శూన్య __RPC_FAR *__RPC_FAR *ppvObject);

	ULONG ( STDMETHODCALLTYPE __RPC_FAR *కూడు_Ref )(
	    IVim __RPC_FAR * This);

	ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )(
	    IVim __RPC_FAR * This);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Get_Type_Info_లెక్క )(
	    IVim __RPC_FAR * This,
	    /* [out] */ UINT __RPC_FAR *pctinfo);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )(
	    IVim __RPC_FAR * This,
	    /* [in] */ UINT iTInfo,
	    /* [in] */ LCID lcid,
	    /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Get_IDs_యొక్క_పేర్లు )(
	    IVim __RPC_FAR * This,
	    /* [in] */ REFIID riid,
	    /* [size_is][in] */ LPOLESTR __RPC_FAR *rgsz_పేర్లు,
	    /* [in] */ UINT c_పేర్లు,
	    /* [in] */ LCID lcid,
	    /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);

	/* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )(
	    IVim __RPC_FAR * This,
	    /* [in] */ DISPID dispIdMember,
	    /* [in] */ REFIID riid,
	    /* [in] */ LCID lcid,
	    /* [in] */ మాట_2 wFlags,
	    /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
	    /* [out] */ VARIANT __RPC_FAR *p_Var_ఫలితం,
	    /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
	    /* [out] */ UINT __RPC_FAR *puArgErr);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Send_కీలకాలు )(
	    IVim __RPC_FAR * This,
	    /* [in] */ BSTR కీలకాలు);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Eval )(
	    IVim __RPC_FAR * This,
	    /* [in] */ BSTR సమాసం,
	    /* [retval][out] */ BSTR __RPC_FAR *ఫలితం);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *సమితి_Foreground )(
	    IVim __RPC_FAR * This);

	HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetHwnd )(
	    IVim __RPC_FAR * This,
	    /* [retval][out] */ UINT_PTR __RPC_FAR *ఫలితం);

	చివర_INTERFACE
    } IVimVtbl;

    interface IVim
    {
	CONST_VTBL నిర్మాణం IVimVtbl __RPC_FAR *lpVtbl;
    };

#నిర్వచితమైతే COBJMACROS


#నిర్వచించు IVim_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#నిర్వచించు IVim_కూడు_Ref(This)	\
    (This)->lpVtbl -> కూడు_Ref(This)

#నిర్వచించు IVim_Release(This)	\
    (This)->lpVtbl -> Release(This)


#నిర్వచించు IVim_Get_Type_Info_లెక్క(This,pctinfo)	\
    (This)->lpVtbl -> Get_Type_Info_లెక్క(This,pctinfo)

#నిర్వచించు IVim_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#నిర్వచించు IVim_Get_IDs_యొక్క_పేర్లు(This,riid,rgsz_పేర్లు,c_పేర్లు,lcid,rgDispId)	\
    (This)->lpVtbl -> Get_IDs_యొక్క_పేర్లు(This,riid,rgsz_పేర్లు,c_పేర్లు,lcid,rgDispId)

#నిర్వచించు IVim_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,p_Var_ఫలితం,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,p_Var_ఫలితం,pExcepInfo,puArgErr)


#నిర్వచించు IVim_Send_కీలకాలు(This,కీలకాలు)	\
    (This)->lpVtbl -> Send_కీలకాలు(This,కీలకాలు)

#నిర్వచించు IVim_Eval(This,సమాసం,ఫలితం)	\
    (This)->lpVtbl -> Eval(This,సమాసం,ఫలితం)

#నిర్వచించు IVim_సమితి_Foreground(This)	\
    (This)->lpVtbl -> సమితి_Foreground(This)

#నిర్వచించు IVim_GetHwnd(This,ఫలితం)	\
    (This)->lpVtbl -> GetHwnd(This,ఫలితం)

#అయితే_ముగింపు // COBJMACROS


#అయితే_ముగింపు	// C style interface



HRESULT STDMETHODCALLTYPE IVim_Send_కీలకాలు_Proxy(
    IVim __RPC_FAR * This,
    /* [in] */ BSTR కీలకాలు);


శూన్య __RPC_STUB IVim_Send_కీలకాలు_Stub(
    IRpc_Stub_బఫరు *This,
    IRpc_Channel_బఫరు *p_Rpc_Channel_బఫరు,
    PRPC_సందేశం p_Rpc_సందేశం,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVim_Eval_Proxy(
    IVim __RPC_FAR * This,
    /* [in] */ BSTR సమాసం,
    /* [retval][out] */ BSTR __RPC_FAR *ఫలితం);


శూన్య __RPC_STUB IVim_Eval_Stub(
    IRpc_Stub_బఫరు *This,
    IRpc_Channel_బఫరు *p_Rpc_Channel_బఫరు,
    PRPC_సందేశం p_Rpc_సందేశం,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVim_సమితి_Foreground_Proxy(
    IVim __RPC_FAR * This);


శూన్య __RPC_STUB IVim_సమితి_Foreground_Stub(
    IRpc_Stub_బఫరు *This,
    IRpc_Channel_బఫరు *p_Rpc_Channel_బఫరు,
    PRPC_సందేశం p_Rpc_సందేశం,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVim_GetHwnd_Proxy(
    IVim __RPC_FAR * This,
    /* [retval][out] */ UINT_PTR __RPC_FAR *ఫలితం);


శూన్య __RPC_STUB IVim_GetHwnd_Stub(
    IRpc_Stub_బఫరు *This,
    IRpc_Channel_బఫరు *p_Rpc_Channel_బఫరు,
    PRPC_సందేశం p_Rpc_సందేశం,
    DWORD *_pdwStubPhase);



#అయితే_ముగింపు	// __IVim_INTERFACE_DEFINED__



#నిర్వచితం_కాకపోతే __Vim_LIBRARY_DEFINED__
#నిర్వచించు __Vim_LIBRARY_DEFINED__

//***************************************
// Generated header for library: Vim
// at Wed Jun 06 18:20:37 2001
// using MIDL 3.01.75
//**************************************
// [version][helpstring][uuid]



EXTERN_C స్థిరరాశి IID LIBID_Vim;

#నిర్వచితమైతే __cplusplus
EXTERN_C స్థిరరాశి CLSID CLSID_Vim;

class DECLSPEC_UUID("0F0BFAE1-4C90-11d1-82D7-0004AC368519")
Vim;
#అయితే_ముగింపు
#అయితే_ముగింపు // __Vim_LIBRARY_DEFINED__

// Additional Prototypes for ALL interfaces

గుర్తులేని దీర్ఘ		  __RPC_USER  BSTR_User_కొలత(	 గుర్తులేని దీర్ఘ __RPC_FAR *, గుర్తులేని దీర్ఘ	     , BSTR __RPC_FAR * );
గుర్తులేని అక్షరం __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  గుర్తులేని దీర్ఘ __RPC_FAR *, గుర్తులేని అక్షరం __RPC_FAR *, BSTR __RPC_FAR * );
గుర్తులేని అక్షరం __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(గుర్తులేని దీర్ఘ __RPC_FAR *, గుర్తులేని అక్షరం __RPC_FAR *, BSTR __RPC_FAR * );
శూన్య			  __RPC_USER  BSTR_UserFree(	 గుర్తులేని దీర్ఘ __RPC_FAR *, BSTR __RPC_FAR * );

// end of Additional Prototypes

#నిర్వచితమైతే __cplusplus
}
#అయితే_ముగింపు

#అయితే_ముగింపు
