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

#ಅನ್ತರ್ಭಾವ "vim.h"

// Declare nv_cmds[].
#ಅನ್ತರ್ಭಾವ "nv_cmds.h"

#ಅನ್ತರ್ಭಾವ <stdio.h>

ಪೂರ್ಣಾಂಕ ಮುಖ್ಯ(ಶೂನ್ಯ)
{
    size_t i;

    ಗಾಗಿ (i = 0; i < NV_CMDS_ಗಾತ್ರ; i++)
    {
	ಪೂರ್ಣಾಂಕ cmdchar = nv_cmds[i];

	// Special keys are negative, use the negated value for sorting.
	ಆದರೆ (cmdchar < 0)
	    cmdchar = -cmdchar;
	ಬರೆ("%d\n", cmdchar);
    }
    ಹಿಂತಿರುಗಿ 0;
}
