/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			this file by Vince Negri
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * vimrun.c - Tiny Win32 program to safely run an external command in a
 *	      DOS console.
 *	      This program is required to avoid that typing CTRL-C in the DOS
 *	      console kills Vim.  Now it only kills vimrun.
 */

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <conio.h>
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32_LEAN_AND_ಸರಾಸರಿ
# define WIN32_LEAN_AND_ಸರಾಸರಿ
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <windows.h>

    ಪೂರ್ಣಾಂಕ
ಮುಖ್ಯ(ಶೂನ್ಯ)
{
    ಸ್ಥಿರಾಂಕ wchar_t   *p;
    wchar_t	    *cmd;
    size_t	    cmdlen;
    ಪೂರ್ಣಾಂಕ		    retval;
    ಪೂರ್ಣಾಂಕ		    inquote = 0;
    ಪೂರ್ಣಾಂಕ		    silent = 0;
    HANDLE	    hstdout;
    DWORD	    written;

    p = (ಸ್ಥಿರಾಂಕ wchar_t *)Get_Command_ಸಾಲು_W();

    /*
     * Skip the executable name, which might be in "".
     */
    ಆಗ (*p)
    {
	ಆದರೆ (*p == L'"')
	    inquote = !inquote;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!inquote && *p == L' ')
	{
	    ++p;
	    ನಿಲ್ಲಿಸು;
	}
	++p;
    }
    ಆಗ (*p == L' ')
	++p;

    /*
     * "-s" argument: don't wait for a key hit.
     */
    ಆದರೆ (p[0] == L'-' && p[1] == L's' && p[2] == L' ')
    {
	silent = 1;
	p += 3;
	ಆಗ (*p == L' ')
	    ++p;
    }

    // Print the command, including quotes and redirection.
    hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteConsoleW(hstdout, p, wcslen(p), &written, ಖಾಲಿ);
    WriteConsoleW(hstdout, L"\r\n", 2, &written, ಖಾಲಿ);

    // If the command starts and ends with double quotes,
    // Enclose the command in parentheses.
    cmd = ಖಾಲಿ;
    cmdlen = wcslen(p);
    ಆದರೆ (cmdlen >= 2 && p[0] == L'"' && p[cmdlen - 1] == L'"')
    {
	cmdlen += 3;
	cmd = ಸ್ಮರಣೆ_ಕೊಡು(cmdlen * ಗಾತ್ರ(wchar_t));
	ಆದರೆ (cmd == ಖಾಲಿ)
	{
	    perror("vimrun malloc(): ");
	    ಹಿಂತಿರುಗಿ -1;
	}
	_snwprintf(cmd, cmdlen, L"(%s)", p);
	p = cmd;
    }

    /*
     * Do it!
     */
    retval = _wsystem(p);

    ಆದರೆ (cmd)
	ಬಿಡುಗಡೆ(cmd);

    ಆದರೆ (retval == -1)
	perror("vimrun system(): ");
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (retval != 0)
	ಬರೆ("shell returned %d\n", retval);

    ಆದರೆ (!silent)
    {
	puts("Hit any key to close this window...");

	ಆಗ (_kbhit())
	    (ಶೂನ್ಯ)_getch();
	(ಶೂನ್ಯ)_getch();
    }

    ಹಿಂತಿರುಗಿ retval;
}
