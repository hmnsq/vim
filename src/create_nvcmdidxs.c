/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar et al.
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * create_nvcmdidxs.c: helper program for `make nvcmdidxs`
 *
 * This outputs the list of command characters from the nv_cmds table in
 * decimal form, one per line.
 */

#अन्तर्भाव "vim.h"

// Declare nv_cmds[].
#अन्तर्भाव "nv_cmds.h"

#अन्तर्भाव <stdio.h>

पूर्णांक main(शून्य)
{
    माप_प्रकार i;

    साठी (i = 0; i < NV_CMDS_SIZE; i++)
    {
	पूर्णांक cmdchar = nv_cmds[i];

	// Special keys are negative, use the negated value for sorting.
	जर (cmdchar < 0)
	    cmdchar = -cmdchar;
	म_लिखो("%d\n", cmdchar);
    }
    परत 0;
}
