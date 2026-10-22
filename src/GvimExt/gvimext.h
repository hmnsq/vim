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

#إذا !defined(AFX_STDAFX_H__3389658B_AD83_11D3_9C1E_0090278BBD99__INCLUDED_)
#تعریف_کن AFX_STDAFX_H__3389658B_AD83_11D3_9C1E_0090278BBD99__INCLUDED_

#إذا defined(_MSC_VER) && _MSC_VER > 1000
#رهنمود once
#پایان_اگر

// Insert your headers here
// #define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

//--------------------------------------------------------------
// common user interface routines
//
//
//--------------------------------------------------------------

#اگر_تعریف_نشده STRICT
# define STRICT
#پایان_اگر

#تعریف_کن INC_OLE2	// MS-Windows, get ole2 from windows.h

/* Visual Studio 2005 has 'deprecated' many of the standard CRT functions */
#إذا defined(_MSC_VER) && _MSC_VER >= 1400
# define _CRT_SECURE_NO_DEPRECATE
# define _CRT_NONSTDC_NO_DEPRECATE
#پایان_اگر

#ضمن <windows.h>
#ضمن <windowsx.h>
#ضمن <shlobj.h>
#ضمن <wchar.h>

#تعریف_کن نتیجه_From_Short(i)  نتیجه_From_Scode(MAKE_SCODE(SEVERITY_SUCCESS, 0, (USHORT)(i)))

// Initialize GUIDs (should be done only and at-least once per DLL/EXE)
//
#رهنمود داده_seg(".text")
#تعریف_کن INITGUID
#ضمن <initguid.h>

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
class CShellExtClassFactory : عام IClassFactory
{
محمي:
    ULONG	m_cRef;

عام:
    CShellExtClassFactory();
    مجازی ~CShellExtClassFactory();

    //IUnknown members
    STDMETHODIMP		QueryInterface(REFIID, LPVOID FAR *);
    STDMETHODIMP_(ULONG)	جمع_کن_Ref();
    STDMETHODIMP_(ULONG)	Release();

    //IClassFactory members
    STDMETHODIMP		CreateInstance(LPUNKNOWN, REFIID, LPVOID FAR *);
    STDMETHODIMP		قفل_Server(BOOL);
};
تعريفنوع CShellExtClassFactory *LPCSHELLEXTCLASSFACTORY;
#تعریف_کن MAX_HWND 100

// this is the actual OLE Shell context menu handler
class CShellExt : عام IContextMenu,
			 IShellExtInit
{
خاص:
    BOOL بارگیری_Menu_Icon();

محمي:
    ULONG	 m_cRef;
    LPDATAOBJECT m_p_داده_Obj;
    UINT	 m_edit_existing_off;
    HBITMAP	 m_hVimIconBitmap;

    // For some reason, this callback must be static
    ساكن BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);

    STDMETHODIMP افزودن_مقصد_پنجره(HWND h_والد,
	    LPCSTR pszWorkingDir,
	    LPCSTR pszCmd,
	    LPCSTR pszParam,
	    صحيح iShowCmd,
	    صحيح idHWnd);

    STDMETHODIMP InvokeSingleGvim(HWND h_والد,
	    LPCWSTR workingDir,
	    LPCSTR pszCmd,
	    LPCSTR pszParam,
	    صحيح iShowCmd,
	    صحيح gvimExtraOptions);

عام:
    صحيح		 m_cnt_از_HWnd;
    HWND	 m_hWnd[MAX_HWND];
    CShellExt();
    مجازی ~CShellExt();

    //IUnknown members
    STDMETHODIMP QueryInterface(REFIID, LPVOID FAR *);
    STDMETHODIMP_(ULONG) جمع_کن_Ref();
    STDMETHODIMP_(ULONG) Release();

    //IShell members
    STDMETHODIMP QueryContextMenu(HMENU hMenu,
	    UINT شاخص_Menu,
	    UINT id_Cmd_اولی,
	    UINT id_Cmd_آخرین,
	    UINT uFlags);

    STDMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi);

    STDMETHODIMP GetCommandString(UINT_PTR idCmd,
	    UINT uFlags,
	    UINT FAR *reserved,
	    LPSTR pszName,
	    UINT cchMax);

    //IShellExtInit methods
    STDMETHODIMP Initialize(LPCITEMIDLIST pIDFolder,
	    LPDATAOBJECT p_داده_Obj,
	    HKEY h_کلید_ID);
};

تعريفنوع CShellExt *LPCSHELLEXT;
#رهنمود داده_seg()

#پایان_اگر
