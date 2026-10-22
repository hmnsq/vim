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

पूर्णाङ्क main(शून्य)
{
    size_t i;

    प्रति (i = 0; i < NV_CMDS_SIZE; i++)
    {
	पूर्णाङ्क cmdchar = nv_cmds[i];

	// Special keys are negative, use the negated value for sorting.
	यदि (cmdchar < 0)
	    cmdchar = -cmdchar;
	printf("%d\n", cmdchar);
    }
    प्रत्यागम 0;
}
