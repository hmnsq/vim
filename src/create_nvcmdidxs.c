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

#include "vim.h"

// Declare nv_cmds[].
#include "nv_cmds.h"

#include <stdio.h>

số_nguyên main(void)
{
    size_t i;

    cho (i = 0; i < NV_CMDS_SIZE; i++)
    {
	số_nguyên cmdchar = nv_cmds[i];

	// Special keys are negative, use the negated value for sorting.
	nếu (cmdchar < 0)
	    cmdchar = -cmdchar;
	printf("%d\n", cmdchar);
    }
    trả_về 0;
}
