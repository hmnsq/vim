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
#ಅನ್ತರ್ಭಾವ "vim.h"

// cproto doesn't create a prototype for VimMain()
#ifdef VIMDLL
__declspec(dllimport)
#endif
ಪೂರ್ಣಾಙ್ಕ VimMain(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ **argv);

#ifdef VIMDLL
# define SaveInst(hInst)    // Do nothing
#ಅನ್ಯಥಾ
ಶೂನ್ಯ SaveInst(HINSTANCE hInst);
#endif

#ifdef FEAT_GUI
    ಪೂರ್ಣಾಙ್ಕ WINAPI
wWinMain(
    HINSTANCE	hInstance,
    HINSTANCE	hPrevInst UNUSED,
    LPWSTR	lpsz_Cmd_ಸಾಲು UNUSED,
    ಪೂರ್ಣಾಙ್ಕ		nCmdShow UNUSED)
{
    SaveInst(hInstance);
    ಪ್ರತ್ಯಾಗಮ VimMain(0, ರಿಕ್ತ);
}
#ಅನ್ಯಥಾ
    ಪೂರ್ಣಾಙ್ಕ
wmain(ಪೂರ್ಣಾಙ್ಕ argc UNUSED, wchar_t **argv UNUSED)
{
    SaveInst(GetModuleHandleW(ರಿಕ್ತ));
    ಪ್ರತ್ಯಾಗಮ VimMain(0, ರಿಕ್ತ);
}
#endif

#ifdef USE_OWNSTARTUP
// Use our own entry point and don't use the default CRT startup code to
// reduce the size of (g)vim.exe.  This works only when VIMDLL is defined.
//
// For MSVC, the /GS- compiler option is needed to avoid the undefined symbol
// error.  (It disables the security check. However, it affects only this
// function and doesn't have any effect on Vim itself.)
// For MinGW, the -nostdlib compiler option and the --entry linker option are
// needed.
# ifdef FEAT_GUI
    ಶೂನ್ಯ WINAPI
wWinMainCRTStartup(ಶೂನ್ಯ)
{
    VimMain(0, ರಿಕ್ತ);
}
# ಅನ್ಯಥಾ
    ಶೂನ್ಯ
wmainCRTStartup(ಶೂನ್ಯ)
{
    VimMain(0, ರಿಕ್ತ);
}
# endif
#endif	// USE_OWNSTARTUP


#ಯದಿ defined(VIMDLL) && defined(FEAT_MZSCHEME)

# ಯದಿ defined(_MSC_VER)
ಸ್ಥಿರ __declspec(thread) ಶೂನ್ಯ *tls_space;
ಬಾಹ್ಯ intptr_t _tls_index;
# elif defined(__MINGW32__)
ಸ್ಥಿರ __thread ಶೂನ್ಯ *tls_space;
ಬಾಹ್ಯ intptr_t _tls_index;
# endif

// Get TLS information that is needed for if_mzsch.
    __declspec(dllexport) ಶೂನ್ಯ
get_tls_info(ಶೂನ್ಯ ***ptls_space, intptr_t *ptls_index)
{
    *ptls_space = &tls_space;
    *ptls_index = _tls_index;
    ಪ್ರತ್ಯಾಗಮ;
}
#endif
