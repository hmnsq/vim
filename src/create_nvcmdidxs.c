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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

// Declare nv_cmds[].
#ਅਨ੍ਤਰ੍ਭਾਵ "nv_cmds.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>

ਪੂਰਨ ਮੁੱਖ(ਖਾਲੀ)
{
    size_t i;

    ਲਈ (i = 0; i < NV_CMDS_ਮਾਪ; i++)
    {
	ਪੂਰਨ cmdchar = nv_cmds[i];

	// Special keys are negative, use the negated value for sorting.
	ਜੇ (cmdchar < 0)
	    cmdchar = -cmdchar;
	ਲਿਖੋ("%d\n", cmdchar);
    }
    ਵਾਪਸ 0;
}
