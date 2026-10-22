/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */
/*
 * Windows GUI/Console: main program (DLL) entry point:
 *
 * Ron Aaron <ronaharon@yahoo.com> wrote this and the DLL support code.
 * Adapted by Ken Takata.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32_LEAN_AND_ಸರಾಸರಿ
# define WIN32_LEAN_AND_ಸರಾಸರಿ
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <windows.h>

ಶೂನ್ಯ SaveInst(HINSTANCE hInst);

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    ಆದರೆ (fdwReason == DLL_PROCESS_ATTACH)
	SaveInst(hinstDLL);
    ಹಿಂತಿರುಗಿ TRUE;
}
