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
#ଅନ୍ତର୍ଭାଵ "vim.h"

// cproto doesn't create a prototype for VimMain()
#ifdef VIMDLL
__declspec(dllimport)
#endif
ପୂର୍ଣ୍ଣସଂଖ୍ୟା VimMain(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର **argv);

#ifdef VIMDLL
# define SaveInst(hInst)    // Do nothing
#ଅନ୍ୟଥା
ଶୂନ୍ୟ SaveInst(HINSTANCE hInst);
#endif

#ifdef FEAT_GUI
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା WINAPI
wWinMain(
    HINSTANCE	hInstance,
    HINSTANCE	hPrevInst UNUSED,
    LPWSTR	lpsz_Cmd_ଧାଡ଼ି UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nCmdShow UNUSED)
{
    SaveInst(hInstance);
    ଫେରନ୍ତୁ VimMain(0, ଖାଲି);
}
#ଅନ୍ୟଥା
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
wmain(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc UNUSED, wchar_t **argv UNUSED)
{
    SaveInst(GetModuleHandleW(ଖାଲି));
    ଫେରନ୍ତୁ VimMain(0, ଖାଲି);
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
    ଶୂନ୍ୟ WINAPI
wWinMainCRTStartup(ଶୂନ୍ୟ)
{
    VimMain(0, ଖାଲି);
}
# ଅନ୍ୟଥା
    ଶୂନ୍ୟ
wmainCRTStartup(ଶୂନ୍ୟ)
{
    VimMain(0, ଖାଲି);
}
# endif
#endif	// USE_OWNSTARTUP


#ଯଦି defined(VIMDLL) && defined(FEAT_MZSCHEME)

# ଯଦି defined(_MSC_VER)
ସ୍ଥିର __declspec(thread) ଶୂନ୍ୟ *tls_space;
ବାହ୍ୟ intptr_t _tls_index;
# elif defined(__MINGW32__)
ସ୍ଥିର __thread ଶୂନ୍ୟ *tls_space;
ବାହ୍ୟ intptr_t _tls_index;
# endif

// Get TLS information that is needed for if_mzsch.
    __declspec(dllexport) ଶୂନ୍ୟ
get_tls_info(ଶୂନ୍ୟ ***ptls_space, intptr_t *ptls_index)
{
    *ptls_space = &tls_space;
    *ptls_index = _tls_index;
    ଫେରନ୍ତୁ;
}
#endif
