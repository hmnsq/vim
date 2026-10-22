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
#include "vim.h"

// cproto doesn't create a prototype for VimMain()
#ifdef VIMDLL
__declspec(dllimport)
#endif
จำนวนเต็ม VimMain(จำนวนเต็ม argc, var_na **argv);

#ifdef VIMDLL
# define SaveInst(hInst)    // Do nothing
#มิฉะนั้น
void SaveInst(HINSTANCE hInst);
#endif

#ifdef FEAT_GUI
    จำนวนเต็ม WINAPI
wWinMain(
    HINSTANCE	hInstance,
    HINSTANCE	hPrevInst UNUSED,
    LPWSTR	lpszCmdLine UNUSED,
    จำนวนเต็ม		nCmdShow UNUSED)
{
    SaveInst(hInstance);
    คืน VimMain(0, NULL);
}
#มิฉะนั้น
    จำนวนเต็ม
wmain(จำนวนเต็ม argc UNUSED, wchar_t **argv UNUSED)
{
    SaveInst(GetModuleHandleW(NULL));
    คืน VimMain(0, NULL);
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
    void WINAPI
wWinMainCRTStartup(void)
{
    VimMain(0, NULL);
}
# มิฉะนั้น
    void
wmainCRTStartup(void)
{
    VimMain(0, NULL);
}
# endif
#endif	// USE_OWNSTARTUP


#ถ้า defined(VIMDLL) && defined(FEAT_MZSCHEME)

# ถ้า defined(_MSC_VER)
static __declspec(thread) void *tls_space;
extern intptr_t _tls_index;
# elif defined(__MINGW32__)
static __thread void *tls_space;
extern intptr_t _tls_index;
# endif

// Get TLS information that is needed for if_mzsch.
    __declspec(dllexport) void
get_tls_info(void ***ptls_space, intptr_t *ptls_index)
{
    *ptls_space = &tls_space;
    *ptls_index = _tls_index;
    คืน;
}
#endif
