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
#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

// cproto doesn't create a prototype for VimMain()
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VIMDLL
__declspec(dllimport)
#ਜੇ_ਅੰਤ
ਪੂਰਨ VimMain(ਪੂਰਨ argc, ਅੱਖਰ **argv);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VIMDLL
# define SaveInst(hInst)    // Do nothing
#ਨਹੀਂਤਾਂ
ਖਾਲੀ SaveInst(HINSTANCE hInst);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਪੂਰਨ WINAPI
wWinMain(
    HINSTANCE	hInstance,
    HINSTANCE	hPrevInst ਅਣਵਰਤਿਆ,
    LPWSTR	lpsz_Cmd_ਸਤਰ ਅਣਵਰਤਿਆ,
    ਪੂਰਨ		nCmdShow ਅਣਵਰਤਿਆ)
{
    SaveInst(hInstance);
    ਵਾਪਸ VimMain(0, ਨਿਰਾ);
}
#ਨਹੀਂਤਾਂ
    ਪੂਰਨ
wmain(ਪੂਰਨ argc ਅਣਵਰਤਿਆ, wchar_t **argv ਅਣਵਰਤਿਆ)
{
    SaveInst(GetModuleHandleW(ਨਿਰਾ));
    ਵਾਪਸ VimMain(0, ਨਿਰਾ);
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OWNSTARTUP
// Use our own entry point and don't use the default CRT startup code to
// reduce the size of (g)vim.exe.  This works only when VIMDLL is defined.
//
// For MSVC, the /GS- compiler option is needed to avoid the undefined symbol
// error.  (It disables the security check. However, it affects only this
// function and doesn't have any effect on Vim itself.)
// For MinGW, the -nostdlib compiler option and the --entry linker option are
// needed.
# ifdef FEAT_GUI
    ਖਾਲੀ WINAPI
wWinMainCRTStartup(ਖਾਲੀ)
{
    VimMain(0, ਨਿਰਾ);
}
# ਨਹੀਂਤਾਂ
    ਖਾਲੀ
wmainCRTStartup(ਖਾਲੀ)
{
    VimMain(0, ਨਿਰਾ);
}
# endif
#ਜੇ_ਅੰਤ	// USE_OWNSTARTUP


#ਜੇ defined(VIMDLL) && defined(FEAT_MZSCHEME)

# ਜੇ defined(_MSC_VER)
ਸਥਿਰ __declspec(thread) ਖਾਲੀ *tls_space;
ਬਾਹਰੀ intptr_t tls_ਸੂਚਕਾਂਕ;
# elif defined(__MINGW32__)
ਸਥਿਰ __thread ਖਾਲੀ *tls_space;
ਬਾਹਰੀ intptr_t tls_ਸੂਚਕਾਂਕ;
# endif

// Get TLS information that is needed for if_mzsch.
    __declspec(dllexport) ਖਾਲੀ
get_tls_info(ਖਾਲੀ ***ptls_space, intptr_t *ptls_ਸੂਚਕਾਂਕ)
{
    *ptls_space = &tls_space;
    *ptls_ਸੂਚਕਾਂਕ = tls_ਸੂਚਕਾਂਕ;
    ਵਾਪਸ;
}
#ਜੇ_ਅੰਤ
