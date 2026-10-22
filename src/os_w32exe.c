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
#અન્તર્ભાવ "vim.h"

// cproto doesn't create a prototype for VimMain()
#ifdef VIMDLL
__declspec(dllimport)
#endif
પૂર્ણાંક VimMain(પૂર્ણાંક argc, અક્ષર **argv);

#ifdef VIMDLL
# define SaveInst(hInst)    // Do nothing
#નહીંતર
શૂન્ય SaveInst(HINSTANCE hInst);
#endif

#ifdef FEAT_GUI
    પૂર્ણાંક WINAPI
wWinMain(
    HINSTANCE	hInstance,
    HINSTANCE	hPrevInst UNUSED,
    LPWSTR	lpsz_Cmd_લીટી UNUSED,
    પૂર્ણાંક		nCmdShow UNUSED)
{
    SaveInst(hInstance);
    પાછા VimMain(0, ખાલી);
}
#નહીંતર
    પૂર્ણાંક
wmain(પૂર્ણાંક argc UNUSED, wchar_t **argv UNUSED)
{
    SaveInst(GetModuleHandleW(ખાલી));
    પાછા VimMain(0, ખાલી);
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
    શૂન્ય WINAPI
wWinMainCRTStartup(શૂન્ય)
{
    VimMain(0, ખાલી);
}
# નહીંતર
    શૂન્ય
wmainCRTStartup(શૂન્ય)
{
    VimMain(0, ખાલી);
}
# endif
#endif	// USE_OWNSTARTUP


#જો defined(VIMDLL) && defined(FEAT_MZSCHEME)

# જો defined(_MSC_VER)
સ્થિર __declspec(thread) શૂન્ય *tls_space;
બાહ્ય intptr_t _tls_index;
# elif defined(__MINGW32__)
સ્થિર __thread શૂન્ય *tls_space;
બાહ્ય intptr_t _tls_index;
# endif

// Get TLS information that is needed for if_mzsch.
    __declspec(dllexport) શૂન્ય
get_tls_info(શૂન્ય ***ptls_space, intptr_t *ptls_index)
{
    *ptls_space = &tls_space;
    *ptls_index = _tls_index;
    પાછા;
}
#endif
