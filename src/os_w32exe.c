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
#അന്തര്ഭാവ "vim.h"

// cproto doesn't create a prototype for VimMain()
#ifdef VIMDLL
__declspec(dllimport)
#endif
പൂർണ്ണസംഖ്യ VimMain(പൂർണ്ണസംഖ്യ argc, അക്ഷരം **argv);

#ifdef VIMDLL
# define SaveInst(hInst)    // Do nothing
#അല്ലെങ്കിൽ
ശൂന്യം SaveInst(HINSTANCE hInst);
#endif

#ifdef FEAT_GUI
    പൂർണ്ണസംഖ്യ WINAPI
wWinMain(
    HINSTANCE	hInstance,
    HINSTANCE	hPrevInst UNUSED,
    LPWSTR	lpsz_Cmd_വരി UNUSED,
    പൂർണ്ണസംഖ്യ		nCmdShow UNUSED)
{
    SaveInst(hInstance);
    തിരികെ VimMain(0, ശൂന്യ);
}
#അല്ലെങ്കിൽ
    പൂർണ്ണസംഖ്യ
wmain(പൂർണ്ണസംഖ്യ argc UNUSED, wchar_t **argv UNUSED)
{
    SaveInst(GetModuleHandleW(ശൂന്യ));
    തിരികെ VimMain(0, ശൂന്യ);
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
    ശൂന്യം WINAPI
wWinMainCRTStartup(ശൂന്യം)
{
    VimMain(0, ശൂന്യ);
}
# അല്ലെങ്കിൽ
    ശൂന്യം
wmainCRTStartup(ശൂന്യം)
{
    VimMain(0, ശൂന്യ);
}
# endif
#endif	// USE_OWNSTARTUP


#എങ്കിൽ defined(VIMDLL) && defined(FEAT_MZSCHEME)

# എങ്കിൽ defined(_MSC_VER)
സ്ഥിതം __declspec(thread) ശൂന്യം *tls_space;
ബാഹ്യ intptr_t _tls_index;
# elif defined(__MINGW32__)
സ്ഥിതം __thread ശൂന്യം *tls_space;
ബാഹ്യ intptr_t _tls_index;
# endif

// Get TLS information that is needed for if_mzsch.
    __declspec(dllexport) ശൂന്യം
get_tls_info(ശൂന്യം ***ptls_space, intptr_t *ptls_index)
{
    *ptls_space = &tls_space;
    *ptls_index = _tls_index;
    തിരികെ;
}
#endif
