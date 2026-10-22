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
#அந்தர்பாவ "vim.h"

// cproto doesn't create a prototype for VimMain()
#ifdef VIMDLL
__declspec(dllimport)
#endif
முழு VimMain(முழு argc, எழுத்து **argv);

#ifdef VIMDLL
# define SaveInst(hInst)    // Do nothing
#இல்லையெனில்
வெறுமை SaveInst(HINSTANCE hInst);
#endif

#ifdef FEAT_GUI
    முழு WINAPI
wWinMain(
    HINSTANCE	hInstance,
    HINSTANCE	hPrevInst UNUSED,
    LPWSTR	lpsz_Cmd_வரி UNUSED,
    முழு		nCmdShow UNUSED)
{
    SaveInst(hInstance);
    திரும்பு VimMain(0, வெற்று);
}
#இல்லையெனில்
    முழு
wmain(முழு argc UNUSED, wchar_t **argv UNUSED)
{
    SaveInst(GetModuleHandleW(வெற்று));
    திரும்பு VimMain(0, வெற்று);
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
    வெறுமை WINAPI
wWinMainCRTStartup(வெறுமை)
{
    VimMain(0, வெற்று);
}
# இல்லையெனில்
    வெறுமை
wmainCRTStartup(வெறுமை)
{
    VimMain(0, வெற்று);
}
# endif
#endif	// USE_OWNSTARTUP


#எனில் defined(VIMDLL) && defined(FEAT_MZSCHEME)

# எனில் defined(_MSC_VER)
நிலையான __declspec(thread) வெறுமை *tls_space;
வெளி intptr_t _tls_index;
# elif defined(__MINGW32__)
நிலையான __thread வெறுமை *tls_space;
வெளி intptr_t _tls_index;
# endif

// Get TLS information that is needed for if_mzsch.
    __declspec(dllexport) வெறுமை
get_tls_info(வெறுமை ***ptls_space, intptr_t *ptls_index)
{
    *ptls_space = &tls_space;
    *ptls_index = _tls_index;
    திரும்பு;
}
#endif
