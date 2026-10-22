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
#অন্তর্ভাৱ "vim.h"

// cproto doesn't create a prototype for VimMain()
#ifdef VIMDLL
__declspec(dllimport)
#endif
পূর্ণাঙ্ক VimMain(পূর্ণাঙ্ক argc, ৱর্ণ **argv);

#ifdef VIMDLL
# define SaveInst(hInst)    // Do nothing
#অন্যথা
শূন্য SaveInst(HINSTANCE hInst);
#endif

#ifdef FEAT_GUI
    পূর্ণাঙ্ক WINAPI
wWinMain(
    HINSTANCE	hInstance,
    HINSTANCE	hPrevInst UNUSED,
    LPWSTR	lpsz_Cmd_সাৰি UNUSED,
    পূর্ণাঙ্ক		nCmdShow UNUSED)
{
    SaveInst(hInstance);
    ঘূৰাই VimMain(0, রিক্ত);
}
#অন্যথা
    পূর্ণাঙ্ক
wmain(পূর্ণাঙ্ক argc UNUSED, wchar_t **argv UNUSED)
{
    SaveInst(GetModuleHandleW(রিক্ত));
    ঘূৰাই VimMain(0, রিক্ত);
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
    শূন্য WINAPI
wWinMainCRTStartup(শূন্য)
{
    VimMain(0, রিক্ত);
}
# অন্যথা
    শূন্য
wmainCRTStartup(শূন্য)
{
    VimMain(0, রিক্ত);
}
# endif
#endif	// USE_OWNSTARTUP


#যদি defined(VIMDLL) && defined(FEAT_MZSCHEME)

# যদি defined(_MSC_VER)
স্থির __declspec(thread) শূন্য *tls_space;
বাহ্য intptr_t _tls_index;
# elif defined(__MINGW32__)
স্থির __thread শূন্য *tls_space;
বাহ্য intptr_t _tls_index;
# endif

// Get TLS information that is needed for if_mzsch.
    __declspec(dllexport) শূন্য
get_tls_info(শূন্য ***ptls_space, intptr_t *ptls_index)
{
    *ptls_space = &tls_space;
    *ptls_index = _tls_index;
    ঘূৰাই;
}
#endif
