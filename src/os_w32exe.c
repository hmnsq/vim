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
#شامل "vim.h"

// cproto doesn't create a prototype for VimMain()
#اگر_تعریف_شدہ VIMDLL
__declspec(dllimport)
#اگر_ختم
صحیح VimMain(صحیح argc, حرف **argv);

#اگر_تعریف_شدہ VIMDLL
# define SaveInst(hInst)    // Do nothing
#ورنہ
خالی SaveInst(HINSTANCE hInst);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI
    صحیح WINAPI
wWinMain(
    HINSTANCE	hInstance,
    HINSTANCE	h_پچھلا_Inst غیر_مستعمل,
    LPWSTR	lpsz_Cmd_سطر غیر_مستعمل,
    صحیح		nCmdShow غیر_مستعمل)
{
    SaveInst(hInstance);
    واپس VimMain(0, تہی);
}
#ورنہ
    صحیح
wmain(صحیح argc غیر_مستعمل, wchar_t **argv غیر_مستعمل)
{
    SaveInst(GetModuleHandleW(تہی));
    واپس VimMain(0, تہی);
}
#اگر_ختم

#اگر_تعریف_شدہ USE_OWNSTARTUP
// Use our own entry point and don't use the default CRT startup code to
// reduce the size of (g)vim.exe.  This works only when VIMDLL is defined.
//
// For MSVC, the /GS- compiler option is needed to avoid the undefined symbol
// error.  (It disables the security check. However, it affects only this
// function and doesn't have any effect on Vim itself.)
// For MinGW, the -nostdlib compiler option and the --entry linker option are
// needed.
# ifdef FEAT_GUI
    خالی WINAPI
wWinMainCRTStartup(خالی)
{
    VimMain(0, تہی);
}
# ورنہ
    خالی
wmainCRTStartup(خالی)
{
    VimMain(0, تہی);
}
# endif
#اگر_ختم	// USE_OWNSTARTUP


#اگر defined(VIMDLL) && defined(FEAT_MZSCHEME)

# اگر defined(_MSC_VER)
ساکن __declspec(thread) خالی *tls_space;
خارجی intptr_t tls_اشاریہ;
# elif defined(__MINGW32__)
ساکن __thread خالی *tls_space;
خارجی intptr_t tls_اشاریہ;
# endif

// Get TLS information that is needed for if_mzsch.
    __declspec(dllexport) خالی
get_tls_info(خالی ***ptls_space, intptr_t *ptls_اشاریہ)
{
    *ptls_space = &tls_space;
    *ptls_اشاریہ = tls_اشاریہ;
    واپس;
}
#اگر_ختم
