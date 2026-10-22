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
#اگر_تعریف_شده VIMDLL
__declspec(dllimport)
#پایان_اگر
صحيح VimMain(صحيح argc, حرف **argv);

#اگر_تعریف_شده VIMDLL
# define SaveInst(hInst)    // Do nothing
#وإلا
فراغ SaveInst(HINSTANCE hInst);
#پایان_اگر

#اگر_تعریف_شده FEAT_GUI
    صحيح WINAPI
wWinMain(
    HINSTANCE	hInstance,
    HINSTANCE	hPrevInst استفادهنشده,
    LPWSTR	lpsz_Cmd_سطر استفادهنشده,
    صحيح		nCmdShow استفادهنشده)
{
    SaveInst(hInstance);
    أرجع VimMain(0, عدم);
}
#وإلا
    صحيح
wmain(صحيح argc استفادهنشده, wchar_t **argv استفادهنشده)
{
    SaveInst(GetModuleHandleW(عدم));
    أرجع VimMain(0, عدم);
}
#پایان_اگر

#اگر_تعریف_شده USE_OWNSTARTUP
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
#پایان_اگر	// USE_OWNSTARTUP


#إذا defined(VIMDLL) && defined(FEAT_MZSCHEME)

# إذا defined(_MSC_VER)
ساكن __declspec(thread) فراغ *tls_space;
خارجي intptr_t tls_شاخص;
# elif defined(__MINGW32__)
ساكن __thread فراغ *tls_space;
خارجي intptr_t tls_شاخص;
# endif

// Get TLS information that is needed for if_mzsch.
    __declspec(dllexport) فراغ
get_tls_info(فراغ ***ptls_space, intptr_t *ptls_شاخص)
{
    *ptls_space = &tls_space;
    *ptls_شاخص = tls_شاخص;
    أرجع;
}
#پایان_اگر
