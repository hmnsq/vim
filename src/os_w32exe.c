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
#अन्तर्भाव "vim.h"

// cproto doesn't create a prototype for VimMain()
#ifdef VIMDLL
__declspec(dllimport)
#endif
पूर्णाङ्क VimMain(पूर्णाङ्क argc, वर्ण **argv);

#ifdef VIMDLL
# define SaveInst(hInst)    // Do nothing
#अण्णहा
शून्य SaveInst(HINSTANCE hInst);
#endif

#ifdef FEAT_GUI
    पूर्णाङ्क WINAPI
wWinMain(
    HINSTANCE	hInstance,
    HINSTANCE	hPrevInst UNUSED,
    LPWSTR	lpsz_Cmd_पङ्क्तिः UNUSED,
    पूर्णाङ्क		nCmdShow UNUSED)
{
    SaveInst(hInstance);
    प्रत्यागम VimMain(0, रिक्त);
}
#अण्णहा
    पूर्णाङ्क
wmain(पूर्णाङ्क argc UNUSED, wchar_t **argv UNUSED)
{
    SaveInst(GetModuleHandleW(रिक्त));
    प्रत्यागम VimMain(0, रिक्त);
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
    शून्य WINAPI
wWinMainCRTStartup(शून्य)
{
    VimMain(0, रिक्त);
}
# अण्णहा
    शून्य
wmainCRTStartup(शून्य)
{
    VimMain(0, रिक्त);
}
# endif
#endif	// USE_OWNSTARTUP


#जइ defined(VIMDLL) && defined(FEAT_MZSCHEME)

# जइ defined(_MSC_VER)
स्थिर __declspec(thread) शून्य *tls_space;
बाह्य intptr_t _tls_index;
# elif defined(__MINGW32__)
स्थिर __thread शून्य *tls_space;
बाह्य intptr_t _tls_index;
# endif

// Get TLS information that is needed for if_mzsch.
    __declspec(dllexport) शून्य
get_tls_info(शून्य ***ptls_space, intptr_t *ptls_index)
{
    *ptls_space = &tls_space;
    *ptls_index = _tls_index;
    प्रत्यागम;
}
#endif
