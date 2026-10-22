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

#ఒకవేళ !defined(AFX_STDAFX_H__3389658B_AD83_11D3_9C1E_0090278BBD99__INCLUDED_)
#నిర్వచించు AFX_STDAFX_H__3389658B_AD83_11D3_9C1E_0090278BBD99__INCLUDED_

#ఒకవేళ defined(_MSC_VER) && _MSC_VER > 1000
#సూచన once
#అయితే_ముగింపు

// Insert your headers here
// #define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

//--------------------------------------------------------------
// common user interface routines
//
//
//--------------------------------------------------------------

#నిర్వచితం_కాకపోతే STRICT
# define STRICT
#అయితే_ముగింపు

#నిర్వచించు INC_OLE2	// MS-Windows, get ole2 from windows.h

/* Visual Studio 2005 has 'deprecated' many of the standard CRT functions */
#ఒకవేళ defined(_MSC_VER) && _MSC_VER >= 1400
# define _CRT_SECURE_NO_DEPRECATE
# define _CRT_NONSTDC_NO_DEPRECATE
#అయితే_ముగింపు

#అన్తర్భావ <windows.h>
#అన్తర్భావ <windowsx.h>
#అన్తర్భావ <shlobj.h>
#అన్తర్భావ <wchar.h>

#నిర్వచించు ఫలితం_From_Short(i)  ఫలితం_From_Scode(MAKE_SCODE(SEVERITY_SUCCESS, 0, (USHORT)(i)))

// Initialize GUIDs (should be done only and at-least once per DLL/EXE)
//
#సూచన దత్తాంశం_seg(".text")
#నిర్వచించు INITGUID
#అన్తర్భావ <initguid.h>

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
class CShellExtClassFactory : బహిరంగ IClassFactory
{
రక్షిత:
    ULONG	m_cRef;

బహిరంగ:
    CShellExtClassFactory();
    మాయా ~CShellExtClassFactory();

    //IUnknown members
    STDMETHODIMP		QueryInterface(REFIID, LPVOID FAR *);
    STDMETHODIMP_(ULONG)	కూడు_Ref();
    STDMETHODIMP_(ULONG)	Release();

    //IClassFactory members
    STDMETHODIMP		CreateInstance(LPUNKNOWN, REFIID, LPVOID FAR *);
    STDMETHODIMP		తాళం_Server(BOOL);
};
రకనిర్వచనం CShellExtClassFactory *LPCSHELLEXTCLASSFACTORY;
#నిర్వచించు MAX_HWND 100

// this is the actual OLE Shell context menu handler
class CShellExt : బహిరంగ IContextMenu,
			 IShellExtInit
{
అంతరంగ:
    BOOL ఎక్కించు_Menu_Icon();

రక్షిత:
    ULONG	 m_cRef;
    LPDATAOBJECT m_p_దత్తాంశం_Obj;
    UINT	 m_edit_existing_off;
    HBITMAP	 m_hVimIconBitmap;

    // For some reason, this callback must be static
    స్థిర BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);

    STDMETHODIMP నెట్టు_గమ్యం_కిటికీ(HWND h_మాతృ,
	    LPCSTR pszWorkingDir,
	    LPCSTR pszCmd,
	    LPCSTR pszParam,
	    పూర్ణాంకం iShowCmd,
	    పూర్ణాంకం idHWnd);

    STDMETHODIMP InvokeSingleGvim(HWND h_మాతృ,
	    LPCWSTR workingDir,
	    LPCSTR pszCmd,
	    LPCSTR pszParam,
	    పూర్ణాంకం iShowCmd,
	    పూర్ణాంకం gvimExtraOptions);

బహిరంగ:
    పూర్ణాంకం		 m_cnt_యొక్క_HWnd;
    HWND	 m_hWnd[MAX_HWND];
    CShellExt();
    మాయా ~CShellExt();

    //IUnknown members
    STDMETHODIMP QueryInterface(REFIID, LPVOID FAR *);
    STDMETHODIMP_(ULONG) కూడు_Ref();
    STDMETHODIMP_(ULONG) Release();

    //IShell members
    STDMETHODIMP QueryContextMenu(HMENU hMenu,
	    UINT సూచీ_Menu,
	    UINT id_Cmd_మొదటి,
	    UINT id_Cmd_చివరి,
	    UINT uFlags);

    STDMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi);

    STDMETHODIMP GetCommandString(UINT_PTR idCmd,
	    UINT uFlags,
	    UINT FAR *reserved,
	    LPSTR pszName,
	    UINT cchMax);

    //IShellExtInit methods
    STDMETHODIMP Initialize(LPCITEMIDLIST pIDFolder,
	    LPDATAOBJECT p_దత్తాంశం_Obj,
	    HKEY h_కీలకం_ID);
};

రకనిర్వచనం CShellExt *LPCSHELLEXT;
#సూచన దత్తాంశం_seg()

#అయితే_ముగింపు
