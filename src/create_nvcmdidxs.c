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

#شامل "vim.h"

// Declare nv_cmds[].
#شامل "nv_cmds.h"

#شامل <stdio.h>

صحیح مین(خالی)
{
    size_t i;

    برائے (i = 0; i < NV_CMDS_سائز; i++)
    {
	صحیح cmdchar = nv_cmds[i];

	// Special keys are negative, use the negated value for sorting.
	اگر (cmdchar < 0)
	    cmdchar = -cmdchar;
	لکھو("%d\n", cmdchar);
    }
    واپس 0;
}
