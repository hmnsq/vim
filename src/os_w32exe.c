/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */
/*
 * Windows GUI/Console: main program (EXE) entry point:
 *
 * Ron Aaron <ronaharon@yahoo.com> wrote this and the DLL support code.
 * Adapted by Ken Takata.
 */
#అన్తర్భావ "vim.h"

// cproto doesn't create a prototype for VimMain()
#నిర్వచితమైతే VIMDLL
__declspec(dllimport)
#అయితే_ముగింపు
పూర్ణాంకం VimMain(పూర్ణాంకం argc, అక్షరం **argv);

#నిర్వచితమైతే VIMDLL
# define SaveInst(hInst)    // Do nothing
#లేకపోతే
శూన్య SaveInst(HINSTANCE hInst);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI
    పూర్ణాంకం WINAPI
wWinMain(
    HINSTANCE	hInstance,
    HINSTANCE	hPrevInst వాడని,
    LPWSTR	lpsz_Cmd_పంక్తి వాడని,
    పూర్ణాంకం		nCmdShow వాడని)
{
    SaveInst(hInstance);
    తిరిగి VimMain(0, ఖాళీ);
}
#లేకపోతే
    పూర్ణాంకం
wmain(పూర్ణాంకం argc వాడని, wchar_t **argv వాడని)
{
    SaveInst(GetModuleHandleW(ఖాళీ));
    తిరిగి VimMain(0, ఖాళీ);
}
#అయితే_ముగింపు

#నిర్వచితమైతే USE_OWNSTARTUP
// Use our own entry point and don't use the default CRT startup code to
// reduce the size of (g)vim.exe.  This works only when VIMDLL is defined.
//
// For MSVC, the /GS- compiler option is needed to avoid the undefined symbol
// error.  (It disables the security check. However, it affects only this
// function and doesn't have any effect on Vim itself.)
// For MinGW, the -nostdlib compiler option and the --entry linker option are
// needed.
# ifdef FEAT_GUI
    శూన్య WINAPI
wWinMainCRTStartup(శూన్య)
{
    VimMain(0, ఖాళీ);
}
# లేకపోతే
    శూన్య
wmainCRTStartup(శూన్య)
{
    VimMain(0, ఖాళీ);
}
# endif
#అయితే_ముగింపు	// USE_OWNSTARTUP


#ఒకవేళ defined(VIMDLL) && defined(FEAT_MZSCHEME)

# ఒకవేళ defined(_MSC_VER)
స్థిర __declspec(thread) శూన్య *tls_space;
బాహ్య intptr_t tls_సూచీ;
# elif defined(__MINGW32__)
స్థిర __thread శూన్య *tls_space;
బాహ్య intptr_t tls_సూచీ;
# endif

// Get TLS information that is needed for if_mzsch.
    __declspec(dllexport) శూన్య
get_tls_info(శూన్య ***ptls_space, intptr_t *ptls_సూచీ)
{
    *ptls_space = &tls_space;
    *ptls_సూచీ = tls_సూచీ;
    తిరిగి;
}
#అయితే_ముగింపు
