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

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <conio.h>
#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif
#अन्तर्भाव <windows.h>

    पूर्णांक
main(शून्य)
{
    स्थिरांक ब_अक्षर_प्रकार   *p;
    ब_अक्षर_प्रकार	    *cmd;
    माप_प्रकार	    cmdlen;
    पूर्णांक		    retval;
    पूर्णांक		    inquote = 0;
    पूर्णांक		    silent = 0;
    HANDLE	    hstdout;
    DWORD	    written;

    p = (स्थिरांक ब_अक्षर_प्रकार *)Get_Command_ओळ_W();

    /*
     * Skip the executable name, which might be in "".
     */
    पर्यंत (*p)
    {
	जर (*p == L'"')
	    inquote = !inquote;
	अन्यथा जर (!inquote && *p == L' ')
	{
	    ++p;
	    थांबा;
	}
	++p;
    }
    पर्यंत (*p == L' ')
	++p;

    /*
     * "-s" argument: don't wait for a key hit.
     */
    जर (p[0] == L'-' && p[1] == L's' && p[2] == L' ')
    {
	silent = 1;
	p += 3;
	पर्यंत (*p == L' ')
	    ++p;
    }

    // Print the command, including quotes and redirection.
    hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteConsoleW(hstdout, p, wcslen(p), &written, रिक्त);
    WriteConsoleW(hstdout, L"\r\n", 2, &written, रिक्त);

    // If the command starts and ends with double quotes,
    // Enclose the command in parentheses.
    cmd = रिक्त;
    cmdlen = wcslen(p);
    जर (cmdlen >= 2 && p[0] == L'"' && p[cmdlen - 1] == L'"')
    {
	cmdlen += 3;
	cmd = दो_स्मृति(cmdlen * आकार(ब_अक्षर_प्रकार));
	जर (cmd == रिक्त)
	{
	    लिखो_त्रुटि("vimrun malloc(): ");
	    परत -1;
	}
	_snwprintf(cmd, cmdlen, L"(%s)", p);
	p = cmd;
    }

    /*
     * Do it!
     */
    retval = _wsystem(p);

    जर (cmd)
	मुक्त(cmd);

    जर (retval == -1)
	लिखो_त्रुटि("vimrun system(): ");
    अन्यथा जर (retval != 0)
	म_लिखो("shell returned %d\n", retval);

    जर (!silent)
    {
	माला_दो("Hit any key to close this window...");

	पर्यंत (_kbhit())
	    (शून्य)_getch();
	(शून्य)_getch();
    }

    परत retval;
}
