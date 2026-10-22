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

#అన్తర్భావ "vim.h"

// Declare nv_cmds[].
#అన్తర్భావ "nv_cmds.h"

#అన్తర్భావ <stdio.h>

పూర్ణాంకం ప్రధాన(శూన్య)
{
    size_t i;

    కొరకు (i = 0; i < NV_CMDS_కొలత; i++)
    {
	పూర్ణాంకం cmdchar = nv_cmds[i];

	// Special keys are negative, use the negated value for sorting.
	ఒకవేళ (cmdchar < 0)
	    cmdchar = -cmdchar;
	రాయి("%d\n", cmdchar);
    }
    తిరిగి 0;
}
