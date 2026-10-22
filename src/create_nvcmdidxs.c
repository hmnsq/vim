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

#அந்தர்பாவ "vim.h"

// Declare nv_cmds[].
#அந்தர்பாவ "nv_cmds.h"

#அந்தர்பாவ <stdio.h>

முழு முதன்மை(வெறுமை)
{
    size_t i;

    க்கு (i = 0; i < NV_CMDS_SIZE; i++)
    {
	முழு cmdchar = nv_cmds[i];

	// Special keys are negative, use the negated value for sorting.
	எனில் (cmdchar < 0)
	    cmdchar = -cmdchar;
	printf("%d\n", cmdchar);
    }
    திரும்பு 0;
}
