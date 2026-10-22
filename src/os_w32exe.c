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
#অন্তর্ভাব "vim.h"

// cproto doesn't create a prototype for VimMain()
#ifdef VIMDLL
__declspec(dllimport)
#endif
পূর্ণসংখ্যা VimMain(পূর্ণসংখ্যা argc, অক্ষর **argv);

#ifdef VIMDLL
# define SaveInst(hInst)    // Do nothing
#অন্যথা
শূন্য SaveInst(HINSTANCE hInst);
#endif

#ifdef FEAT_GUI
    পূর্ণসংখ্যা WINAPI
wWinMain(
    HINSTANCE	hInstance,
    HINSTANCE	h_পূর্ব_Inst UNUSED,
    LPWSTR	lpsz_Cmd_সারি UNUSED,
    পূর্ণসংখ্যা		nCmdShow UNUSED)
{
    SaveInst(hInstance);
    ফেরত VimMain(0, ফাঁকা);
}
#অন্যথা
    পূর্ণসংখ্যা
wmain(পূর্ণসংখ্যা argc UNUSED, wchar_t **argv UNUSED)
{
    SaveInst(GetModuleHandleW(ফাঁকা));
    ফেরত VimMain(0, ফাঁকা);
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
    VimMain(0, ফাঁকা);
}
# অন্যথা
    শূন্য
wmainCRTStartup(শূন্য)
{
    VimMain(0, ফাঁকা);
}
# endif
#endif	// USE_OWNSTARTUP


#যদি defined(VIMDLL) && defined(FEAT_MZSCHEME)

# যদি defined(_MSC_VER)
স্থির __declspec(thread) শূন্য *tls_space;
বাহ্যিক intptr_t tls_সূচক;
# elif defined(__MINGW32__)
স্থির __thread শূন্য *tls_space;
বাহ্যিক intptr_t tls_সূচক;
# endif

// Get TLS information that is needed for if_mzsch.
    __declspec(dllexport) শূন্য
get_tls_info(শূন্য ***ptls_space, intptr_t *ptls_সূচক)
{
    *ptls_space = &tls_space;
    *ptls_সূচক = tls_সূচক;
    ফেরত;
}
#endif
