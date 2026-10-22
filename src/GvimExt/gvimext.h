/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	gvimext by Tianmiao Hu
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * If you have any questions or any suggestions concerning gvimext, please
 * contact Tianmiao Hu: tianmiao@acm.org.
 */

#ଯଦି !defined(AFX_STDAFX_H__3389658B_AD83_11D3_9C1E_0090278BBD99__INCLUDED_)
#define AFX_STDAFX_H__3389658B_AD83_11D3_9C1E_0090278BBD99__INCLUDED_

#ଯଦି defined(_MSC_VER) && _MSC_VER > 1000
#pragma once
#endif

// Insert your headers here
// #define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

//--------------------------------------------------------------
// common user interface routines
//
//
//--------------------------------------------------------------

#ifndef STRICT
# define STRICT
#endif

#define INC_OLE2	// MS-Windows, get ole2 from windows.h

/* Visual Studio 2005 has 'deprecated' many of the standard CRT functions */
#ଯଦି defined(_MSC_VER) && _MSC_VER >= 1400
# define _CRT_SECURE_NO_DEPRECATE
# define _CRT_NONSTDC_NO_DEPRECATE
#endif

#ଅନ୍ତର୍ଭାଵ <windows.h>
#ଅନ୍ତର୍ଭାଵ <windowsx.h>
#ଅନ୍ତର୍ଭାଵ <shlobj.h>
#ଅନ୍ତର୍ଭାଵ <wchar.h>

#define ResultFromShort(i)  ResultFromScode(MAKE_SCODE(SEVERITY_SUCCESS, 0, (USHORT)(i)))

// Initialize GUIDs (should be done only and at-least once per DLL/EXE)
//
#pragma data_seg(".text")
#define INITGUID
#ଅନ୍ତର୍ଭାଵ <initguid.h>

//
// The class ID of this Shell extension class.
//
// class id:  {51EEE242-AD87-11d3-9C1E-0090278BBD99}
//
//
// NOTE!!!  If you use this shell extension as a starting point,
//	    you MUST change the GUID below.  Simply run UUIDGEN.EXE
//	    to generate a new GUID.
//

// {51EEE242-AD87-11d3-9C1E-0090278BBD99}
// static const GUID <<name>> =
// { 0x51eee242, 0xad87, 0x11d3, { 0x9c, 0x1e, 0x0, 0x90, 0x27, 0x8b, 0xbd, 0x99 } };
//
//

// {51EEE242-AD87-11d3-9C1E-0090278BBD99}
// IMPLEMENT_OLECREATE(<<class>>, <<external_name>>,
// 0x51eee242, 0xad87, 0x11d3, 0x9c, 0x1e, 0x0, 0x90, 0x27, 0x8b, 0xbd, 0x99);
//

// {51EEE242-AD87-11d3-9C1E-0090278BBD99}  -- this is the registry format
DEFINE_GUID(CLSID_ShellExtension, 0x51eee242, 0xad87, 0x11d3, 0x9c, 0x1e, 0x0, 0x90, 0x27, 0x8b, 0xbd, 0x99);

// this class factory object creates context menu handlers for windows 32 shell
class CShellExtClassFactory : ସର୍ବସାଧାରଣ IClassFactory
{
ସୁରକ୍ଷିତ:
    ULONG	m_cRef;

ସର୍ବସାଧାରଣ:
    CShellExtClassFactory();
    ଆଭାସୀ ~CShellExtClassFactory();

    //IUnknown members
    STDMETHODIMP		QueryInterface(REFIID, LPVOID FAR *);
    STDMETHODIMP_(ULONG)	AddRef();
    STDMETHODIMP_(ULONG)	Release();

    //IClassFactory members
    STDMETHODIMP		CreateInstance(LPUNKNOWN, REFIID, LPVOID FAR *);
    STDMETHODIMP		LockServer(BOOL);
};
ପ୍ରକାରବ୍ୟାଖ୍ୟା CShellExtClassFactory *LPCSHELLEXTCLASSFACTORY;
#define MAX_HWND 100

// this is the actual OLE Shell context menu handler
class CShellExt : ସର୍ବସାଧାରଣ IContextMenu,
			 IShellExtInit
{
ବ୍ୟକ୍ତିଗତ:
    BOOL LoadMenuIcon();

ସୁରକ୍ଷିତ:
    ULONG	 m_cRef;
    LPDATAOBJECT m_pDataObj;
    UINT	 m_edit_existing_off;
    HBITMAP	 m_hVimIconBitmap;

    // For some reason, this callback must be static
    ସ୍ଥିର BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);

    STDMETHODIMP ଠେଲ_To_Window(HWND hParent,
	    LPCSTR pszWorkingDir,
	    LPCSTR pszCmd,
	    LPCSTR pszParam,
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା iShowCmd,
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idHWnd);

    STDMETHODIMP InvokeSingleGvim(HWND hParent,
	    LPCWSTR workingDir,
	    LPCSTR pszCmd,
	    LPCSTR pszParam,
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା iShowCmd,
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା gvimExtraOptions);

ସର୍ବସାଧାରଣ:
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		 m_cntOfHWnd;
    HWND	 m_hWnd[MAX_HWND];
    CShellExt();
    ଆଭାସୀ ~CShellExt();

    //IUnknown members
    STDMETHODIMP QueryInterface(REFIID, LPVOID FAR *);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    //IShell members
    STDMETHODIMP QueryContextMenu(HMENU hMenu,
	    UINT indexMenu,
	    UINT idCmdFirst,
	    UINT idCmdLast,
	    UINT uFlags);

    STDMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi);

    STDMETHODIMP GetCommandString(UINT_PTR idCmd,
	    UINT uFlags,
	    UINT FAR *reserved,
	    LPSTR pszName,
	    UINT cchMax);

    //IShellExtInit methods
    STDMETHODIMP Initialize(LPCITEMIDLIST pIDFolder,
	    LPDATAOBJECT pDataObj,
	    HKEY h_ଚାବି_ID);
};

ପ୍ରକାରବ୍ୟାଖ୍ୟା CShellExt *LPCSHELLEXT;
#pragma data_seg()

#endif
