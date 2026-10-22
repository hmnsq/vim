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
#ضمن "vim.h"

// cproto doesn't create a prototype for VimMain()
#جيڪڏهن_وصف_ٿيل VIMDLL
__declspec(dllimport)
#جيڪڏهن_پورو
صحيح VimMain(صحيح argc, حرف **argv);

#جيڪڏهن_وصف_ٿيل VIMDLL
# define SaveInst(hInst)    // Do nothing
#وإلا
فراغ SaveInst(HINSTANCE hInst);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_GUI
    صحيح WINAPI
wWinMain(
    HINSTANCE	hInstance,
    HINSTANCE	hPrevInst اڻ_استعمال_ٿيل,
    LPWSTR	lpsz_Cmd_سٽ اڻ_استعمال_ٿيل,
    صحيح		nCmdShow اڻ_استعمال_ٿيل)
{
    SaveInst(hInstance);
    أرجع VimMain(0, عدم);
}
#وإلا
    صحيح
wmain(صحيح argc اڻ_استعمال_ٿيل, wchar_t **argv اڻ_استعمال_ٿيل)
{
    SaveInst(GetModuleHandleW(عدم));
    أرجع VimMain(0, عدم);
}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_OWNSTARTUP
// Use our own entry point and don't use the default CRT startup code to
// reduce the size of (g)vim.exe.  This works only when VIMDLL is defined.
//
// For MSVC, the /GS- compiler option is needed to avoid the undefined symbol
// error.  (It disables the security check. However, it affects only this
// function and doesn't have any effect on Vim itself.)
// For MinGW, the -nostdlib compiler option and the --entry linker option are
// needed.
# ifdef FEAT_GUI
    فراغ WINAPI
wWinMainCRTStartup(فراغ)
{
    VimMain(0, عدم);
}
# وإلا
    فراغ
wmainCRTStartup(فراغ)
{
    VimMain(0, عدم);
}
# endif
#جيڪڏهن_پورو	// USE_OWNSTARTUP


#إذا defined(VIMDLL) && defined(FEAT_MZSCHEME)

# إذا defined(_MSC_VER)
ساكن __declspec(thread) فراغ *tls_space;
خارجي intptr_t tls_اشاريو;
# elif defined(__MINGW32__)
ساكن __thread فراغ *tls_space;
خارجي intptr_t tls_اشاريو;
# endif

// Get TLS information that is needed for if_mzsch.
    __declspec(dllexport) فراغ
get_tls_info(فراغ ***ptls_space, intptr_t *ptls_اشاريو)
{
    *ptls_space = &tls_space;
    *ptls_اشاريو = tls_اشاريو;
    أرجع;
}
#جيڪڏهن_پورو
