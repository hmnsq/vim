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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VIMDLL
__declspec(dllimport)
#ಆದರೆ_ಅಂತ್ಯ
ಪೂರ್ಣಾಂಕ VimMain(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ **argv);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VIMDLL
# define SaveInst(hInst)    // Do nothing
#ಇಲ್ಲದಿದ್ದರೆ
ಶೂನ್ಯ SaveInst(HINSTANCE hInst);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಪೂರ್ಣಾಂಕ WINAPI
wWinMain(
    HINSTANCE	hInstance,
    HINSTANCE	hPrevInst ಬಳಸದ,
    LPWSTR	lpsz_Cmd_ಸಾಲು ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ		nCmdShow ಬಳಸದ)
{
    SaveInst(hInstance);
    ಹಿಂತಿರುಗಿ VimMain(0, ಖಾಲಿ);
}
#ಇಲ್ಲದಿದ್ದರೆ
    ಪೂರ್ಣಾಂಕ
wmain(ಪೂರ್ಣಾಂಕ argc ಬಳಸದ, wchar_t **argv ಬಳಸದ)
{
    SaveInst(GetModuleHandleW(ಖಾಲಿ));
    ಹಿಂತಿರುಗಿ VimMain(0, ಖಾಲಿ);
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OWNSTARTUP
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
    VimMain(0, ಖಾಲಿ);
}
# ಇಲ್ಲದಿದ್ದರೆ
    ಶೂನ್ಯ
wmainCRTStartup(ಶೂನ್ಯ)
{
    VimMain(0, ಖಾಲಿ);
}
# endif
#ಆದರೆ_ಅಂತ್ಯ	// USE_OWNSTARTUP


#ಆದರೆ defined(VIMDLL) && defined(FEAT_MZSCHEME)

# ಆದರೆ defined(_MSC_VER)
ಸ್ಥಿರ __declspec(thread) ಶೂನ್ಯ *tls_space;
ಬಾಹ್ಯ intptr_t tls_ಸೂಚ್ಯಂಕ;
# elif defined(__MINGW32__)
ಸ್ಥಿರ __thread ಶೂನ್ಯ *tls_space;
ಬಾಹ್ಯ intptr_t tls_ಸೂಚ್ಯಂಕ;
# endif

// Get TLS information that is needed for if_mzsch.
    __declspec(dllexport) ಶೂನ್ಯ
get_tls_info(ಶೂನ್ಯ ***ptls_space, intptr_t *ptls_ಸೂಚ್ಯಂಕ)
{
    *ptls_space = &tls_space;
    *ptls_ಸೂಚ್ಯಂಕ = tls_ಸೂಚ್ಯಂಕ;
    ಹಿಂತಿರುಗಿ;
}
#ಆದರೆ_ಅಂತ್ಯ
