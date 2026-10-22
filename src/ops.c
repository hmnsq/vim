/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * ops.c: implementation of various operators: op_shift, op_delete, op_tilde,
 *	  op_change, op_yank, do_join
 */

#अन्तर्भाव "vim.h"

स्थिर शून्य shift_block(oparg_T *oap, पूर्णाङ्क amount);
स्थिर शून्य	mb_adjust_opend(oparg_T *oap);
स्थिर पूर्णाङ्क	do_addsub(पूर्णाङ्क op_type, pos_T *pos, पूर्णाङ्क length, linenr_T Prenum1);
स्थिर शून्य	pbyte(pos_T lp, पूर्णाङ्क c);
#define PBYTE(lp, c) pbyte(lp, c)


// Flags for third item in "opchars".
#define OPF_पङ्क्तयः  1	// operator always works on lines
#define OPF_CHANGE 2	// operator changes text

/*
 * The names of operators.
 * IMPORTANT: Index must correspond with defines in vim.h!!!
 * The third field holds OPF_ flags.
 */
स्थिर ध्रुव वर्ण opchars[][3] =
{
    {NUL, NUL, 0},			// OP_NOP
    {'d', NUL, OPF_CHANGE},		// OP_DELETE
    {'y', NUL, 0},			// OP_YANK
    {'c', NUL, OPF_CHANGE},		// OP_CHANGE
    {'<', NUL, OPF_पङ्क्तयः | OPF_CHANGE},	// OP_LSHIFT
    {'>', NUL, OPF_पङ्क्तयः | OPF_CHANGE},	// OP_RSHIFT
    {'!', NUL, OPF_पङ्क्तयः | OPF_CHANGE},	// OP_FILTER
    {'g', '~', OPF_CHANGE},		// OP_TILDE
    {'=', NUL, OPF_पङ्क्तयः | OPF_CHANGE},	// OP_INDENT
    {'g', 'q', OPF_पङ्क्तयः | OPF_CHANGE},	// OP_FORMAT
    {':', NUL, OPF_पङ्क्तयः},		// OP_COLON
    {'g', 'U', OPF_CHANGE},		// OP_UPPER
    {'g', 'u', OPF_CHANGE},		// OP_LOWER
    {'J', NUL, OPF_पङ्क्तयः | OPF_CHANGE},	// DO_JOIN
    {'g', 'J', OPF_पङ्क्तयः | OPF_CHANGE},	// DO_JOIN_NS
    {'g', '?', OPF_CHANGE},		// OP_ROT13
    {'r', NUL, OPF_CHANGE},		// OP_REPLACE
    {'I', NUL, OPF_CHANGE},		// OP_INSERT
    {'A', NUL, OPF_CHANGE},		// OP_APPEND
    {'z', 'f', OPF_पङ्क्तयः},		// OP_FOLD
    {'z', 'o', OPF_पङ्क्तयः},		// OP_FOLDOPEN
    {'z', 'O', OPF_पङ्क्तयः},		// OP_FOLDOPENREC
    {'z', 'c', OPF_पङ्क्तयः},		// OP_FOLDCLOSE
    {'z', 'C', OPF_पङ्क्तयः},		// OP_FOLDCLOSEREC
    {'z', 'd', OPF_पङ्क्तयः},		// OP_FOLDDEL
    {'z', 'D', OPF_पङ्क्तयः},		// OP_FOLDDELREC
    {'g', 'w', OPF_पङ्क्तयः | OPF_CHANGE},	// OP_FORMAT2
    {'g', '@', OPF_CHANGE},		// OP_FUNCTION
    {Ctrl_A, NUL, OPF_CHANGE},		// OP_NR_ADD
    {Ctrl_X, NUL, OPF_CHANGE},		// OP_NR_SUB
};

/*
 * Translate a command name into an operator type.
 * Must only be called with a valid operator name!
 */
    पूर्णाङ्क
get_op_type(पूर्णाङ्क char1, पूर्णाङ्क char2)
{
    पूर्णाङ्क		i;

    सचे (char1 == 'r')		// ignore second character
	पटिनिवत्त OP_REPLACE;
    सचे (char1 == '~')		// when tilde is an operator
	पटिनिवत्त OP_TILDE;
    सचे (char1 == 'g' && char2 == Ctrl_A)	// add
	पटिनिवत्त OP_NR_ADD;
    सचे (char1 == 'g' && char2 == Ctrl_X)	// subtract
	पटिनिवत्त OP_NR_SUB;
    सचे (char1 == 'z' && char2 == 'y')	// OP_YANK
	पटिनिवत्त OP_YANK;
    अत्थाय (i = 0; ; ++i)
    {
	सचे (opchars[i][0] == char1 && opchars[i][1] == char2)
	    भङ्ग;
	सचे (i == (पूर्णाङ्क)ARRAY_LENGTH(opchars) - 1)
	{
	    internal_error("get_op_type()");
	    भङ्ग;
	}
    }
    पटिनिवत्त i;
}

/*
 * Return TRUE if operator "op" always works on whole lines.
 */
    स्थिर पूर्णाङ्क
op_on_पङ्क्तयः(पूर्णाङ्क op)
{
    पटिनिवत्त opchars[op][2] & OPF_पङ्क्तयः;
}

#सचे defined(FEAT_JOB_CHANNEL)
/*
 * Return TRUE if operator "op" changes text.
 */
    पूर्णाङ्क
op_is_change(पूर्णाङ्क op)
{
    पटिनिवत्त opchars[op][2] & OPF_CHANGE;
}
#endif

/*
 * Get first operator command character.
 * Returns 'g' or 'z' if there is another command character.
 */
    पूर्णाङ्क
get_op_char(पूर्णाङ्क optype)
{
    पटिनिवत्त opchars[optype][0];
}

/*
 * Get second operator command character.
 */
    पूर्णाङ्क
get_extra_op_char(पूर्णाङ्क optype)
{
    पटिनिवत्त opchars[optype][1];
}

/*
 * op_shift - handle a shift operation
 */
    शून्य
op_shift(oparg_T *oap, पूर्णाङ्क curs_शिखरम्, पूर्णाङ्क amount)
{
    long	    i;
    पूर्णाङ्क		    first_char;
    पूर्णाङ्क		    block_col = 0;

    सचे (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	पटिनिवत्त;

    सचे (oap->block_mode)
	block_col = curwin->w_cursor.col;

    अत्थाय (i = oap->पङ्क्तिः_गणनम्; --i >= 0; )
    {
	first_char = *ml_get_curline();
	सचे (first_char == NUL)				// empty line
	    curwin->w_cursor.col = 0;
	अञ्ञथा सचे (oap->block_mode)
	    shift_block(oap, amount);
	अञ्ञथा
	    // Move the line right if it doesn't start with '#', 'smartindent'
	    // isn't set or 'cindent' isn't set or '#' isn't in 'cino'.
	    सचे (first_char != '#' || !preprocs_वामः())
		shift_पङ्क्तिः(oap->op_type == OP_LSHIFT, p_sr, amount, FALSE);
	++curwin->w_cursor.lnum;
    }

    changed_पङ्क्तयः(oap->start.lnum, 0, oap->end.lnum + 1, 0L);
    सचे (oap->block_mode)
    {
	curwin->w_cursor.lnum = oap->start.lnum;
	curwin->w_cursor.col = block_col;
    }
    अञ्ञथा सचे (curs_शिखरम्)	    // put cursor on first line, for ">>"
    {
	curwin->w_cursor.lnum = oap->start.lnum;
	beginline(BL_SOL | BL_FIX);   // shift_line() may have set cursor.col
    }
    अञ्ञथा
	--curwin->w_cursor.lnum;	// put cursor on last line, for ":>"

#ifdef FEAT_FOLDING
    // The cursor line is not in a closed fold
    foldOpenCursor();
#endif


    सचे (oap->पङ्क्तिः_गणनम् > p_report)
    {
	वर्ण	    *op;
	वर्ण	    *सन्देशः_पङ्क्तिः_single;
	वर्ण	    *सन्देशः_पङ्क्तिः_plural;

	सचे (oap->op_type == OP_RSHIFT)
	    op = ">";
	अञ्ञथा
	    op = "<";
	सन्देशः_पङ्क्तिः_single = NGETTEXT("%ld line %sed %d time",
					     "%ld line %sed %d times", amount);
	सन्देशः_पङ्क्तिः_plural = NGETTEXT("%ld lines %sed %d time",
					    "%ld lines %sed %d times", amount);
	vim_snprintf((वर्ण *)IObuff, IOSIZE,
		NGETTEXT(सन्देशः_पङ्क्तिः_single, सन्देशः_पङ्क्तिः_plural, oap->पङ्क्तिः_गणनम्),
		oap->पङ्क्तिः_गणनम्, op, amount);
	सन्देशः_attr_keep((वर्ण *)IObuff, 0, TRUE);
    }

    सचे ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end.lnum = oap->end.lnum;
	curbuf->b_op_end.col = ml_get_len(oap->end.lnum);
	सचे (curbuf->b_op_end.col > 0)
	    --curbuf->b_op_end.col;
    }
}

#ifdef FEAT_VARTABS
/*
 * Return the tabstop width at the index of the variable tabstop array.  If an
 * index greater than the length of the array is given, the last tabstop width
 * in the array is returned.
 */
    स्थिर पूर्णाङ्क
get_vts(पूर्णाङ्क *vts_array, पूर्णाङ्क index)
{
    पूर्णाङ्क	ts;

    सचे (index < 1)
	ts = 0;
    अञ्ञथा सचे (index <= vts_array[0])
	ts = vts_array[index];
    अञ्ञथा
	ts = vts_array[vts_array[0]];

    पटिनिवत्त ts;
}

/*
 * Return the sum of all the tabstops through the index-th.
 */
    स्थिर पूर्णाङ्क
get_vts_sum(पूर्णाङ्क *vts_array, पूर्णाङ्क index)
{
    पूर्णाङ्क	sum = 0;
    पूर्णाङ्क	i;

    // Perform the summation for indices within the actual array.
    अत्थाय (i = 1; i <= index && i <= vts_array[0]; i++)
	sum += vts_array[i];

    // Add tabstops whose indices exceed the actual array.
    सचे (i <= index)
	sum += vts_array[vts_array[0]] * (index - vts_array[0]);

    पटिनिवत्त sum;
}
#endif

    स्थिर vimlong_T
get_new_sw_indent(
    पूर्णाङ्क		वामः,		// TRUE if shift is to the left
    पूर्णाङ्क		round,		// TRUE if new indent is to be to a tabstop
    vimlong_T	amount,		// Number of shifts
    vimlong_T	sw_val)
{
    vimlong_T	गणनम् = get_indent();
    vimlong_T	i, j;

    सचे (round)			// round off indent
    {
	i = trim_to_int(गणनम्) / sw_val;	// number of 'shiftwidth' rounded down
	j = trim_to_int(गणनम्) % sw_val;	// extra spaces
	सचे (j && वामः)		// first remove extra spaces
	    --amount;
	सचे (वामः)
	{
	    i -= amount;
	    सचे (i < 0)
		i = 0;
	}
	अञ्ञथा
	    i += amount;
	गणनम् = i * sw_val;
    }
    अञ्ञथा			// original vi indent
    {
	सचे (वामः)
	{
	    गणनम् -= sw_val * amount;
	    सचे (गणनम् < 0)
		गणनम् = 0;
	}
	अञ्ञथा
	    गणनम् += sw_val * amount;
    }

    पटिनिवत्त गणनम्;
}

#ifdef FEAT_VARTABS
    स्थिर vimlong_T
get_new_vts_indent(
    पूर्णाङ्क	वामः,			// TRUE if shift is to the left
    पूर्णाङ्क	round,			// TRUE if new indent is to be to a tabstop
    पूर्णाङ्क	amount,			// Number of shifts
    पूर्णाङ्क	*vts_array)
{
    vimlong_T	indent = get_indent();
    पूर्णाङ्क		vtsi = 0;
    पूर्णाङ्क		vts_indent = 0;
    पूर्णाङ्क		ts = 0;		// Silence uninitialized variable warning.
    पूर्णाङ्क		offset;		// Extra indent spaces to the right of the
				// tabstop

    // Find the tabstop at or to the left of the current indent.
    याव (vts_indent <= indent)
    {
	vtsi++;
	ts = get_vts(vts_array, vtsi);
	vts_indent += ts;
    }
    vts_indent -= ts;
    vtsi--;

    offset = indent - vts_indent;

    सचे (round)
    {
	सचे (वामः)
	{
	    सचे (offset == 0)
		indent = get_vts_sum(vts_array, vtsi - amount);
	    अञ्ञथा
		indent = get_vts_sum(vts_array, vtsi - (amount - 1));
	}
	अञ्ञथा
	    indent = get_vts_sum(vts_array, vtsi + amount);
    }
    अञ्ञथा
    {
	सचे (वामः)
	{
	    सचे (amount > vtsi)
		indent = 0;
	    अञ्ञथा
		indent = get_vts_sum(vts_array, vtsi - amount) + offset;
	}
	अञ्ञथा
	    indent = get_vts_sum(vts_array, vtsi + amount) + offset;
    }

    पटिनिवत्त indent;
}
#endif

/*
 * Shift the current line 'amount' shiftwidth(s) left (if 'left' is TRUE) or
 * right.
 *
 * The rules for choosing a shiftwidth are:  If 'shiftwidth' is non-zero, use
 * 'shiftwidth'; else if 'vartabstop' is not empty, use 'vartabstop'; else use
 * 'tabstop'.  The Vim documentation says nothing about 'softtabstop' or
 * 'varsofttabstop' affecting the shiftwidth, and neither affects the
 * shiftwidth in current versions of Vim, so they are not considered here.
 */
    शून्य
shift_पङ्क्तिः(
    पूर्णाङ्क	वामः,			// TRUE if shift is to the left
    पूर्णाङ्क	round,			// TRUE if new indent is to be to a tabstop
    पूर्णाङ्क	amount,			// Number of shifts
    पूर्णाङ्क	call_changed_bytes)	// call changed_bytes()
{
    vimlong_T	गणनम्;
    long	sw_val = curbuf->b_p_sw;
    long	ts_val = curbuf->b_p_ts;
#ifdef FEAT_VARTABS
    पूर्णाङ्क		*vts_array = curbuf->b_p_vts_array;
#endif

    सचे (sw_val != 0)
	// 'shiftwidth' is not zero; use it as the shift size.
	गणनम् = get_new_sw_indent(वामः, round, amount, sw_val);
    अञ्ञथा
#ifdef FEAT_VARTABS
	सचे ((vts_array == रिक्त) || (vts_array[0] == 0))
#endif
    {
	// 'shiftwidth is zero and 'vartabstop' is empty; use 'tabstop' as the
	// shift size.
	गणनम् = get_new_sw_indent(वामः, round, amount, ts_val);
    }
#ifdef FEAT_VARTABS
    अञ्ञथा
    {
	// 'shiftwidth is zero and 'vartabstop' is defined; use 'vartabstop'
	// to determine the new indent.
	गणनम् = get_new_vts_indent(वामः, round, amount, vts_array);
    }
#endif

    // Set new indent
    सचे (State & VREPLACE_FLAG)
	change_indent(INDENT_SET, trim_to_int(गणनम्), FALSE, NUL, call_changed_bytes);
    अञ्ञथा
	(शून्य)set_indent(trim_to_int(गणनम्), call_changed_bytes ? SIN_CHANGED : 0);
}

/*
 * Shift one line of the current block one shiftwidth right or left.
 * Leaves cursor on first character in block.
 */
    स्थिर शून्य
shift_block(oparg_T *oap, पूर्णाङ्क amount)
{
    पूर्णाङ्क			वामः = (oap->op_type == OP_LSHIFT);
    पूर्णाङ्क			oldstate = State;
    पूर्णाङ्क			total;
    char_u		*newp, *oldp;
    size_t		newlen, oldlen;
    पूर्णाङ्क			oldcol = curwin->w_cursor.col;
    पूर्णाङ्क			sw_val = (पूर्णाङ्क)get_sw_मानम्_indent(curbuf, वामः);
    पूर्णाङ्क			ts_val = (पूर्णाङ्क)curbuf->b_p_ts;
    संरचना block_def	bd;
    पूर्णाङ्क			incr;
    colnr_T		ws_vcol;
    पूर्णाङ्क			added;
    size_t		new_पङ्क्तिः_len;	// the length of the line after the
					// block shift
#ifdef FEAT_RIGHTLEFT
    पूर्णाङ्क			old_p_ri = p_ri;

    p_ri = 0;			// don't want revins in indent
#endif

    State = MODE_निवेशय;	// don't want MODE_REPLACE for State
    block_prep(oap, &bd, curwin->w_cursor.lnum, TRUE);
    सचे (bd.is_short)
	पटिनिवत्त;

    // total is number of screen columns to be inserted/removed
    total = (पूर्णाङ्क)((unsigned)amount * (unsigned)sw_val);
    सचे ((total / sw_val) != amount)
	पटिनिवत्त; // multiplication overflow

    oldp = ml_get_curline();
    oldlen = ml_get_curline_len();

    सचे (!वामः)
    {
	पूर्णाङ्क		tabs = 0, spaces = 0;
	chartabsize_T	cts;

	/*
	 *  1. Get start vcol
	 *  2. Total ws vcols
	 *  3. Divvy into TABs & spp
	 *  4. Construct new string
	 */
	total += bd.pre_whitesp; // all virtual WS up to & incl a split TAB
	ws_vcol = bd.start_vcol - bd.pre_whitesp;
	सचे (bd.startspaces)
	{
	    सचे (has_mbyte)
	    {
		सचे ((*mb_ptr2len)(bd.textstart) == 1)
		    ++bd.textstart;
		अञ्ञथा
		{
		    ws_vcol = 0;
		    bd.startspaces = 0;
		}
	    }
	    अञ्ञथा
		++bd.textstart;
	}

	// TODO: is passing bd.textstart for start of the line OK?
	init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum,
				   bd.start_vcol, bd.textstart, bd.textstart);
	अत्थाय ( ; VIM_ISWHITE(*cts.cts_ptr); )
	{
	    incr = lbr_chartabsize_adv(&cts);
	    total += incr;
	    cts.cts_vcol += incr;
	}
	bd.textstart = cts.cts_ptr;
	bd.start_vcol = cts.cts_vcol;
	clear_chartabsize_arg(&cts);

	// OK, now total=all the VWS reqd, and textstart points at the 1st
	// non-ws char in the block.
#ifdef FEAT_VARTABS
	सचे (!curbuf->b_p_et)
	    tabstop_fromto(ws_vcol, ws_vcol + total,
				ts_val, curbuf->b_p_vts_array, &tabs, &spaces);
	अञ्ञथा
	    spaces = total;
#अञ्ञथा
	सचे (!curbuf->b_p_et)
	    tabs = ((ws_vcol % ts_val) + total) / ts_val; // number of tabs
	सचे (tabs > 0)
	    spaces = ((ws_vcol % ts_val) + total) % ts_val; // number of spp
	अञ्ञथा
	    spaces = total;
#endif
	// if we're splitting a TAB, allow for it
	bd.textcol -= bd.pre_whitesp_c - (bd.startspaces != 0);

	new_पङ्क्तिः_len = bd.textcol + tabs + spaces + (oldlen - (bd.textstart - oldp));
	newp = alloc(new_पङ्क्तिः_len + 1);
	सचे (newp == रिक्त)
	    पटिनिवत्त;
	mch_memmove(newp, oldp, (size_t)bd.textcol);
	newlen = bd.textcol;
	vim_memset(newp + newlen, TAB, (size_t)tabs);
	newlen += tabs;
	vim_memset(newp + newlen, ' ', (size_t)spaces);
	STRCPY(newp + newlen + spaces, bd.textstart);
    }
    अञ्ञथा //वामः
    {
	colnr_T	    destination_col;	// column to which text in block will
					// be shifted
	char_u	    *verbatim_copy_end;	// end of the part of the line which is
					// copied verbatim
	colnr_T	    verbatim_copy_width;// the (displayed) width of this part
					// of line
	size_t	    fill;		// nr of spaces that replace a TAB
	size_t	    block_space_width;
	size_t	    shift_amount;
	char_u	    *non_white = bd.textstart;
	colnr_T	    non_white_col;
	size_t	    fixedlen;		// length of string left of the shift
					// position (ie the string not being shifted)
	chartabsize_T cts;

	/*
	 * Firstly, let's find the first non-whitespace character that is
	 * displayed after the block's start column and the character's column
	 * number. Also, let's calculate the width of all the whitespace
	 * characters that are displayed in the block and precede the searched
	 * non-whitespace character.
	 */

	// If "bd.startspaces" is set, "bd.textstart" points to the character,
	// the part of which is displayed at the block's beginning. Let's start
	// searching from the next character.
	सचे (bd.startspaces)
	    MB_PTR_ADV(non_white);

	// The character's column is in "bd.start_vcol".
	non_white_col = bd.start_vcol;

	init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum,
				   non_white_col, bd.textstart, non_white);
	याव (VIM_ISWHITE(*cts.cts_ptr))
	{
	    incr = lbr_chartabsize_adv(&cts);
	    cts.cts_vcol += incr;
	}
	non_white_col = cts.cts_vcol;
	non_white = cts.cts_ptr;
	clear_chartabsize_arg(&cts);

	block_space_width = non_white_col - oap->start_vcol;
	// We will shift by "total" or "block_space_width", whichever is less.
	shift_amount = (block_space_width < (size_t)total
					 ? block_space_width : (size_t)total);

	// The column to which we will shift the text.
	destination_col = (colnr_T)(non_white_col - shift_amount);

	// Now let's find out how much of the beginning of the line we can
	// reuse without modification.
	verbatim_copy_end = bd.textstart;
	verbatim_copy_width = bd.start_vcol;

	// If "bd.startspaces" is set, "bd.textstart" points to the character
	// preceding the block. We have to subtract its width to obtain its
	// column number.
	सचे (bd.startspaces)
	    verbatim_copy_width -= bd.start_char_vcols;
	init_chartabsize_arg(&cts, curwin, 0, verbatim_copy_width,
					     bd.textstart, verbatim_copy_end);
	याव (cts.cts_vcol < destination_col)
	{
	    incr = lbr_chartabsize(&cts);
	    सचे (cts.cts_vcol + incr > destination_col)
		भङ्ग;
	    cts.cts_vcol += incr;
	    MB_PTR_ADV(cts.cts_ptr);
	}
	verbatim_copy_width = cts.cts_vcol;
	verbatim_copy_end = cts.cts_ptr;
	clear_chartabsize_arg(&cts);

	// If "destination_col" is different from the width of the initial
	// part of the line that will be copied, it means we encountered a tab
	// character, which we will have to partly replace with spaces.
	fill = destination_col - verbatim_copy_width;

	// The replacement line will consist of:
	// - the beginning of the original line up to "verbatim_copy_end",
	// - "fill" number of spaces,
	// - the rest of the line, pointed to by non_white.
	fixedlen = verbatim_copy_end - oldp;
	new_पङ्क्तिः_len = fixedlen + fill + (oldlen - (non_white - oldp));

	newp = alloc(new_पङ्क्तिः_len + 1);
	सचे (newp == रिक्त)
	    पटिनिवत्त;
	mch_memmove(newp, oldp, fixedlen);
	newlen = fixedlen;
	vim_memset(newp + newlen, ' ', (size_t)fill);
	STRCPY(newp + newlen + fill, non_white);
    }
    // replace the line
    ml_replace(curwin->w_cursor.lnum, newp, FALSE);

    // compute the number of bytes added or subtracted.
    // note new_line_len and oldlen are unsigned so we have
    // to be careful about how we calculate this.
    सचे (new_पङ्क्तिः_len >= oldlen)
	added = (पूर्णाङ्क)(new_पङ्क्तिः_len - oldlen);
    अञ्ञथा
	added = 0 - (पूर्णाङ्क)(oldlen - new_पङ्क्तिः_len);
    inserted_bytes(curwin->w_cursor.lnum, bd.textcol, added);
    State = oldstate;
    curwin->w_cursor.col = oldcol;
#ifdef FEAT_RIGHTLEFT
    p_ri = old_p_ri;
#endif
}

/*
 * Insert string "s" (b_insert ? before : after) block :AKelly
 * Caller must prepare for undo.
 */
    स्थिर शून्य
block_निवेशय(
    oparg_T		*oap,
    char_u		*s,
    size_t		slen,
    पूर्णाङ्क			b_निवेशय,
    संरचना block_def	*bdp)
{
    पूर्णाङ्क		ts_val;
    पूर्णाङ्क		गणनम् = 0;	// extra spaces to replace a cut TAB
    पूर्णाङ्क		spaces = 0;	// non-zero if cutting a TAB
    colnr_T	offset;		// pointer along new line
    colnr_T	startcol;	// column where insert starts
    char_u	*newp, *oldp;	// new, old lines
    linenr_T	lnum;		// loop var
    पूर्णाङ्क		oldstate = State;

    State = MODE_निवेशय;	// don't want MODE_REPLACE for State

    अत्थाय (lnum = oap->start.lnum + 1; lnum <= oap->end.lnum; lnum++)
    {
	block_prep(oap, bdp, lnum, TRUE);
	सचे (bdp->is_short && b_निवेशय)
	    अनुवर्त;	// OP_INSERT, line ends before block start

	oldp = ml_get(lnum);

	सचे (b_निवेशय)
	{
	    ts_val = bdp->start_char_vcols;
	    spaces = bdp->startspaces;
	    सचे (spaces != 0)
		गणनम् = ts_val - 1; // we're cutting a TAB
	    offset = bdp->textcol;
	}
	अञ्ञथा // append
	{
	    ts_val = bdp->end_char_vcols;
	    सचे (!bdp->is_short) // spaces = padding after block
	    {
		spaces = (bdp->endspaces ? ts_val - bdp->endspaces : 0);
		सचे (spaces != 0)
		    गणनम् = ts_val - 1; // we're cutting a TAB
		offset = bdp->textcol + bdp->textlen - (spaces != 0);
	    }
	    अञ्ञथा // spaces = padding to block edge
	    {
		// if $ used, just append to EOL (ie spaces==0)
		सचे (!bdp->is_MAX)
		    spaces = (oap->end_vcol - bdp->end_vcol) + 1;
		गणनम् = spaces;
		offset = bdp->textcol + bdp->textlen;
	    }
	}

	सचे (has_mbyte && spaces > 0)
	    // avoid copying part of a multi-byte character
	    offset -= (*mb_head_off)(oldp, oldp + offset);

	सचे (spaces < 0)  // can happen when the cursor was moved
	    spaces = 0;

	// Make sure the allocated size matches what is actually copied below.
	newp = alloc(ml_get_len(lnum) + spaces + slen
		    + (spaces > 0 && !bdp->is_short ? ts_val - spaces : 0)
								  + गणनम् + 1);
	सचे (newp == रिक्त)
	    अनुवर्त;

	// copy up to shifted part
	mch_memmove(newp, oldp, (size_t)offset);
	oldp += offset;

	// insert pre-padding
	vim_memset(newp + offset, ' ', (size_t)spaces);
	startcol = offset + spaces;

	// copy the new text
	mch_memmove(newp + startcol, s, slen);
	offset += (पूर्णाङ्क)slen;

	सचे (spaces > 0 && !bdp->is_short)
	{
	    सचे (*oldp == TAB)
	    {
		// insert post-padding
		vim_memset(newp + offset + spaces, ' ',
						    (size_t)(ts_val - spaces));
		// we're splitting a TAB, don't copy it
		oldp++;
		// We allowed for that TAB, remember this now
		गणनम्++;
	    }
	    अञ्ञथा
		// Not a TAB, no extra spaces
		गणनम् = spaces;
	}

	सचे (spaces > 0)
	    offset += गणनम्;
	STRCPY(newp + offset, oldp);

	ml_replace(lnum, newp, FALSE);

	सचे (b_निवेशय)
	    // correct any text properties
	    inserted_bytes(lnum, startcol, (पूर्णाङ्क)slen);

	सचे (lnum == oap->end.lnum)
	{
	    // Set "']" mark to the end of the block instead of the end of
	    // the insert in the first line.
	    curbuf->b_op_end.lnum = oap->end.lnum;
	    curbuf->b_op_end.col = offset;
	    सचे (curbuf->b_visual.vi_end.coladd)
	    {
		curbuf->b_visual.vi_end.col += curbuf->b_visual.vi_end.coladd;
		curbuf->b_visual.vi_end.coladd = 0;
	    }
	}
    } // for all lnum

    changed_पङ्क्तयः(oap->start.lnum + 1, 0, oap->end.lnum + 1, 0L);

    State = oldstate;
}

/*
 * Handle a delete operation.
 *
 * Return FAIL if undo failed, OK otherwise.
 */
    पूर्णाङ्क
op_delete(oparg_T *oap)
{
    पूर्णाङ्क			n;
    linenr_T		lnum;
    char_u		*ptr;
    char_u		*newp, *oldp;
    संरचना block_def	bd;
    linenr_T		old_lcount = curbuf->b_ml.ml_पङ्क्तिः_गणनम्;
    पूर्णाङ्क			did_yank = FALSE;

    सचे (curbuf->b_ml.ml_flags & ML_EMPTY)	    // nothing to do
	पटिनिवत्त OK;

    // Nothing to delete, return here.	Do prepare undo, for op_change().
    सचे (oap->empty)
	पटिनिवत्त u_save_cursor();

    सचे (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	पटिनिवत्त FAIL;
    }

    सचे (VIsual_select && oap->is_VIsual)
	// use register given with CTRL_R, defaults to zero
	oap->regname = VIsual_select_reg;

#ifdef HAVE_CLIPMETHOD
    adjust_clip_reg(&oap->regname);
#endif

    सचे (has_mbyte)
	mb_adjust_opend(oap);

    /*
     * Imitate the strange Vi behaviour: If the delete spans more than one
     * line and motion_type == MCHAR and the result is a blank line, make the
     * delete linewise.  Don't do this for the change command or Visual mode.
     */
    सचे (       oap->motion_type == MCHAR
	    && !oap->is_VIsual
	    && !oap->block_mode
	    && oap->पङ्क्तिः_गणनम् > 1
	    && oap->motion_force == NUL
	    && (vim_strchr(p_cpo, CPO_WORD) != रिक्त)
	    && oap->op_type == OP_DELETE)
    {
	ptr = ml_get(oap->end.lnum) + oap->end.col;
	सचे (*ptr != NUL)
	    ptr += oap->inclusive;
	ptr = skipwhite(ptr);
	सचे (*ptr == NUL && inindent(0))
	    oap->motion_type = MLINE;
    }

    /*
     * Check for trying to delete (e.g. "D") in an empty line.
     * Note: For the change operator it is ok.
     */
    सचे (       oap->motion_type == MCHAR
	    && oap->पङ्क्तिः_गणनम् == 1
	    && oap->op_type == OP_DELETE
	    && *ml_get(oap->start.lnum) == NUL)
    {
	/*
	 * It's an error to operate on an empty region, when 'E' included in
	 * 'cpoptions' (Vi compatible).
	 */
	सचे (virtual_op)
	    // Virtual editing: Nothing gets deleted, but we set the '[ and ']
	    // marks as if it happened.
	    गच्छ setmarks;
	सचे (vim_strchr(p_cpo, CPO_EMPTYREGION) != रिक्त)
	    beep_flush();
	पटिनिवत्त OK;
    }

    /*
     * Do a yank of whatever we're about to delete.
     * If a yank register was specified, put the deleted text into that
     * register.  For the black hole register '_' don't yank anything.
     */
    सचे (oap->regname != '_')
    {
#ifdef FEAT_CLIPBOARD_PROVIDER
	inc_clip_provider();
#endif
	सचे (oap->regname != 0)
	{
	    // check for read-only register
	    सचे (!valid_yank_reg(oap->regname, TRUE))
	    {
		beep_flush();
#ifdef FEAT_CLIPBOARD_PROVIDER
		dec_clip_provider();
#endif
		पटिनिवत्त OK;
	    }
	    get_yank_register(oap->regname, TRUE); // yank into specif'd reg.
	    सचे (op_yank(oap, TRUE, FALSE) == OK)   // yank without message
		did_yank = TRUE;
	}
	अञ्ञथा
	    reset_y_append(); // not appending to unnamed register

	/*
	 * Put deleted text into register 1 and shift number registers if the
	 * delete contains a line break, or when using a specific operator (Vi
	 * compatible)
	 */
	सचे (oap->motion_type == MLINE || oap->पङ्क्तिः_गणनम् > 1
							   || oap->use_reg_one)
	{
	    shift_delete_registers();
	    सचे (op_yank(oap, TRUE, FALSE) == OK)
		did_yank = TRUE;
	}

	// Yank into small delete register when no named register specified
	// and the delete is within one line.
	सचे ((
#ifdef HAVE_CLIPMETHOD
	    ((clip_unnamed & CLIP_UNNAMED) && oap->regname == '*') ||
	    ((clip_unnamed & CLIP_UNNAMED_PLUS) && oap->regname == '+') ||
#endif
	    oap->regname == 0) && oap->motion_type != MLINE
						      && oap->पङ्क्तिः_गणनम् == 1)
	{
	    oap->regname = '-';
	    get_yank_register(oap->regname, TRUE);
	    सचे (op_yank(oap, TRUE, FALSE) == OK)
		did_yank = TRUE;
	    oap->regname = 0;
	}

	/*
	 * If there's too much stuff to fit in the yank register, then get a
	 * confirmation before doing the delete. This is crude, but simple.
	 * And it avoids doing a delete of something we can't put back if we
	 * want.
	 */
	सचे (!did_yank)
	{
	    पूर्णाङ्क सन्देशः_silent_save = सन्देशः_silent;

	    सन्देशः_silent = 0;	// must display the prompt
	    n = ask_yesno((char_u *)_("cannot yank; delete anyway"), TRUE);
	    सन्देशः_silent = सन्देशः_silent_save;
	    सचे (n != 'y')
	    {
		emsg(_(e_command_aborted));
#ifdef FEAT_CLIPBOARD_PROVIDER
		dec_clip_provider();
#endif
		पटिनिवत्त FAIL;
	    }
	}

#सचे defined(FEAT_EVAL)
	सचे (did_yank && has_textyankpost())
	    yank_do_autocmd(oap, get_y_current());
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
    }

    /*
     * block mode delete
     */
    सचे (oap->block_mode)
    {
	सचे (u_save((linenr_T)(oap->start.lnum - 1),
			       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	    पटिनिवत्त FAIL;

	अत्थाय (lnum = curwin->w_cursor.lnum; lnum <= oap->end.lnum; ++lnum)
	{
	    block_prep(oap, &bd, lnum, TRUE);
	    सचे (bd.textlen == 0)	// nothing to delete
		अनुवर्त;

	    // Adjust cursor position for tab replaced by spaces and 'lbr'.
	    सचे (lnum == curwin->w_cursor.lnum)
	    {
		curwin->w_cursor.col = bd.textcol + bd.startspaces;
		curwin->w_cursor.coladd = 0;
	    }

	    // "n" == number of chars deleted
	    // If we delete a TAB, it may be replaced by several characters.
	    // Thus the number of characters may increase!
	    n = bd.textlen - bd.startspaces - bd.endspaces;
	    oldp = ml_get(lnum);
	    newp = alloc(ml_get_len(lnum) + 1 - n);
	    सचे (newp == रिक्त)
		अनुवर्त;
	    // copy up to deleted part
	    mch_memmove(newp, oldp, (size_t)bd.textcol);
	    // insert spaces
	    vim_memset(newp + bd.textcol, ' ',
				     (size_t)(bd.startspaces + bd.endspaces));
	    // copy the part after the deleted part
	    STRCPY(newp + bd.textcol + bd.startspaces + bd.endspaces,
				    oldp + bd.textcol + bd.textlen);
	    // replace the line
	    ml_replace(lnum, newp, FALSE);

#ifdef FEAT_PROP_POPUP
	    सचे (curbuf->b_has_textprop && n != 0)
		adjust_prop_columns(lnum, bd.textcol, -n, 0);
#endif
	}

	check_cursor_col();
	changed_पङ्क्तयः(curwin->w_cursor.lnum, curwin->w_cursor.col,
						       oap->end.lnum + 1, 0L);
	oap->पङ्क्तिः_गणनम् = 0;	    // no lines deleted
    }
    अञ्ञथा सचे (oap->motion_type == MLINE)
    {
	सचे (oap->op_type == OP_CHANGE)
	{
	    // Delete the lines except the first one.  Temporarily move the
	    // cursor to the next line.  Save the current line number, if the
	    // last line is deleted it may be changed.
	    सचे (oap->पङ्क्तिः_गणनम् > 1)
	    {
		lnum = curwin->w_cursor.lnum;
		++curwin->w_cursor.lnum;
		del_पङ्क्तयः((long)(oap->पङ्क्तिः_गणनम् - 1), TRUE);
		curwin->w_cursor.lnum = lnum;
	    }
	    सचे (u_save_cursor() == FAIL)
		पटिनिवत्त FAIL;
	    सचे (curbuf->b_p_ai)		    // don't delete indent
	    {
		beginline(BL_WHITE);	    // cursor on first non-white
		did_ai = TRUE;		    // delete the indent when ESC hit
		ai_col = curwin->w_cursor.col;
	    }
	    अञ्ञथा
		beginline(0);		    // cursor in column 0
	    truncate_पङ्क्तिः(FALSE);   // delete the rest of the line,
				    // leaving cursor past last char in line
	    सचे (oap->पङ्क्तिः_गणनम् > 1)
		u_clearline();	    // "U" command not possible after "2cc"
	}
	अञ्ञथा
	{
	    del_पङ्क्तयः(oap->पङ्क्तिः_गणनम्, TRUE);
	    beginline(BL_WHITE | BL_FIX);
	    u_clearline();	// "U" command not possible after "dd"
	}
    }
    अञ्ञथा
    {
	सचे (virtual_op)
	{
	    पूर्णाङ्क		endcol = 0;

	    // For virtualedit: break the tabs that are partly included.
	    सचे (gchar_pos(&oap->start) == '\t')
	    {
		सचे (u_save_cursor() == FAIL)	// save first line for undo
		    पटिनिवत्त FAIL;
		सचे (oap->पङ्क्तिः_गणनम् == 1)
		    endcol = getviscol2(oap->end.col, oap->end.coladd);
		coladvance_force(getviscol2(oap->start.col, oap->start.coladd));
		oap->start = curwin->w_cursor;
		सचे (oap->पङ्क्तिः_गणनम् == 1)
		{
		    coladvance(endcol);
		    oap->end.col = curwin->w_cursor.col;
		    oap->end.coladd = curwin->w_cursor.coladd;
		    curwin->w_cursor = oap->start;
		}
	    }

	    // Break a tab only when it's included in the area.
	    सचे (gchar_pos(&oap->end) == '\t'
				     && (पूर्णाङ्क)oap->end.coladd < oap->inclusive)
	    {
		// save last line for undo
		सचे (u_save((linenr_T)(oap->end.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
		    पटिनिवत्त FAIL;
		curwin->w_cursor = oap->end;
		coladvance_force(getviscol2(oap->end.col, oap->end.coladd));
		oap->end = curwin->w_cursor;
		curwin->w_cursor = oap->start;
	    }
	    सचे (has_mbyte)
		mb_adjust_opend(oap);
	}

	सचे (oap->पङ्क्तिः_गणनम् == 1)	// delete characters within one line
	{
	    सचे (u_save_cursor() == FAIL)	// save line for undo
		पटिनिवत्त FAIL;

	    // if 'cpoptions' contains '$', display '$' at end of change
	    सचे (       vim_strchr(p_cpo, CPO_DOLLAR) != रिक्त
		    && oap->op_type == OP_CHANGE
		    && oap->end.lnum == curwin->w_cursor.lnum
		    && !oap->is_VIsual)
		display_dollar(oap->end.col - !oap->inclusive);

	    n = oap->end.col - oap->start.col + 1 - !oap->inclusive;

	    सचे (virtual_op)
	    {
		// fix up things for virtualedit-delete:
		// break the tabs which are going to get in our way
		पूर्णाङ्क		len = ml_get_curline_len();

		सचे (oap->end.coladd != 0
			&& (पूर्णाङ्क)oap->end.col >= len - 1
			&& !(oap->start.coladd && (पूर्णाङ्क)oap->end.col >= len - 1))
		    n++;
		// Delete at least one char (e.g, when on a control char).
		सचे (n == 0 && oap->start.coladd != oap->end.coladd)
		    n = 1;

		// When deleted a char in the line, reset coladd.
		सचे (gchar_cursor() != NUL)
		    curwin->w_cursor.coladd = 0;
	    }
	    (शून्य)del_bytes((long)n, !virtual_op,
			    oap->op_type == OP_DELETE && !oap->is_VIsual);
	}
	अञ्ञथा				// delete characters between lines
	{
	    pos_T   curpos;

	    // save deleted and changed lines for undo
	    सचे (u_save((linenr_T)(curwin->w_cursor.lnum - 1),
		 (linenr_T)(curwin->w_cursor.lnum + oap->पङ्क्तिः_गणनम्)) == FAIL)
		पटिनिवत्त FAIL;

	    truncate_पङ्क्तिः(TRUE);	// delete from cursor to end of line

	    curpos = curwin->w_cursor;	// remember curwin->w_cursor
	    ++curwin->w_cursor.lnum;
	    del_पङ्क्तयः((long)(oap->पङ्क्तिः_गणनम् - 2), FALSE);

	    // delete from start of line until op_end
	    n = (oap->end.col + 1 - !oap->inclusive);
	    curwin->w_cursor.col = 0;
	    (शून्य)del_bytes((long)n, !virtual_op,
			    oap->op_type == OP_DELETE && !oap->is_VIsual);
	    curwin->w_cursor = curpos;	// restore curwin->w_cursor
	    (शून्य)do_join(2, FALSE, FALSE, FALSE, FALSE);
	}
	सचे (oap->op_type == OP_DELETE)
	    auto_format(FALSE, TRUE);
    }

    msgmore(curbuf->b_ml.ml_पङ्क्तिः_गणनम् - old_lcount);

setmarks:
    सचे ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	सचे (oap->block_mode)
	{
	    curbuf->b_op_end.lnum = oap->end.lnum;
	    curbuf->b_op_end.col = oap->start.col;
	}
	अञ्ञथा
	    curbuf->b_op_end = oap->start;
	curbuf->b_op_start = oap->start;
    }

    पटिनिवत्त OK;
}

/*
 * Adjust end of operating area for ending on a multi-byte character.
 * Used for deletion.
 */
    स्थिर शून्य
mb_adjust_opend(oparg_T *oap)
{
    char_u	*पङ्क्तिः;
    char_u	*ptr;

    सचे (!oap->inclusive)
	पटिनिवत्त;

    पङ्क्तिः = ml_get(oap->end.lnum);
    ptr = पङ्क्तिः + oap->end.col;
    सचे (*ptr != NUL)
    {
	ptr -= (*mb_head_off)(पङ्क्तिः, ptr);
	ptr += (*mb_ptr2len)(ptr) - 1;
	oap->end.col = ptr - पङ्क्तिः;
    }
}

/*
 * Replace the character under the cursor with "c".
 * This takes care of multi-byte characters.
 */
    स्थिर शून्य
replace_character(पूर्णाङ्क c)
{
    पूर्णाङ्क n = State;

    State = MODE_REPLACE;
    ins_char(c);
    State = n;
    // Backup to the replaced character.
    dec_cursor();
}

/*
 * Replace a whole area with one character.
 */
    पूर्णाङ्क
op_replace(oparg_T *oap, पूर्णाङ्क c)
{
    पूर्णाङ्क			n, numc;
    पूर्णाङ्क			अङ्कः_chars;
    char_u		*newp, *oldp;
    size_t		newlen, oldlen;
    संरचना block_def	bd;
    char_u		*after_p = रिक्त;
    पूर्णाङ्क			had_ctrl_v_cr = FALSE;

    सचे ((curbuf->b_ml.ml_flags & ML_EMPTY ) || oap->empty)
	पटिनिवत्त OK;	    // nothing to do

    सचे (c == REPLACE_CR_NCHAR)
    {
	had_ctrl_v_cr = TRUE;
	c = CAR;
    }
    अञ्ञथा सचे (c == REPLACE_NL_NCHAR)
    {
	had_ctrl_v_cr = TRUE;
	c = NL;
    }

    सचे (has_mbyte)
	mb_adjust_opend(oap);

    सचे (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	पटिनिवत्त FAIL;

    /*
     * block mode replace
     */
    सचे (oap->block_mode)
    {
	bd.is_MAX = (curwin->w_curswant == MAXCOL);
	अत्थाय ( ; curwin->w_cursor.lnum <= oap->end.lnum; ++curwin->w_cursor.lnum)
	{
	    curwin->w_cursor.col = 0;  // make sure cursor position is valid
	    block_prep(oap, &bd, curwin->w_cursor.lnum, TRUE);
	    सचे (bd.textlen == 0 && (!virtual_op || bd.is_MAX))
		अनुवर्त;	    // nothing to replace

	    // n == number of extra chars required
	    // If we split a TAB, it may be replaced by several characters.
	    // Thus the number of characters may increase!
	    // If the range starts in virtual space, count the initial
	    // coladd offset as part of "startspaces"
	    सचे (virtual_op && bd.is_short && *bd.textstart == NUL)
	    {
		pos_T vpos;

		vpos.lnum = curwin->w_cursor.lnum;
		getvpos(&vpos, oap->start_vcol);
		bd.startspaces += vpos.coladd;
		n = bd.startspaces;
	    }
	    अञ्ञथा
		// allow for pre spaces
		n = (bd.startspaces ? bd.start_char_vcols - 1 : 0);

	    // allow for post spp
	    n += (bd.endspaces
		    && !bd.is_oneChar
		    && bd.end_char_vcols > 0) ? bd.end_char_vcols - 1 : 0;
	    // Figure out how many characters to replace.
	    numc = oap->end_vcol - oap->start_vcol + 1;
	    सचे (bd.is_short && (!virtual_op || bd.is_MAX))
		numc -= (oap->end_vcol - bd.end_vcol) + 1;

	    // A double-wide character can be replaced only up to half the
	    // times.
	    सचे ((*mb_char2cells)(c) > 1)
	    {
		सचे ((numc & 1) && !bd.is_short)
		{
		    ++bd.endspaces;
		    ++n;
		}
		numc = numc / 2;
	    }

	    // Compute bytes needed, move character count to num_chars.
	    अङ्कः_chars = numc;
	    numc *= (*mb_char2len)(c);
	    // oldlen includes textlen, so don't double count
	    n += numc - bd.textlen;

	    oldp = ml_get_curline();
	    oldlen = ml_get_curline_len();
	    newp = alloc(oldlen + 1 + n);
	    सचे (newp == रिक्त)
		अनुवर्त;
	    vim_memset(newp, NUL, (size_t)(oldlen + 1 + n));
	    // copy up to deleted part
	    mch_memmove(newp, oldp, (size_t)bd.textcol);
	    newlen = bd.textcol;
	    // insert pre-spaces
	    vim_memset(newp + newlen, ' ', (size_t)bd.startspaces);
	    newlen += bd.startspaces;
	    // insert replacement chars CHECK FOR ALLOCATED SPACE
	    // REPLACE_CR_NCHAR/REPLACE_NL_NCHAR is used for entering CR
	    // literally.
	    सचे (had_ctrl_v_cr || (c != '\r' && c != '\n'))
	    {
		सचे (has_mbyte)
		{
		    याव (--अङ्कः_chars >= 0)
			newlen += (*mb_char2bytes)(c, newp + newlen);
		}
		अञ्ञथा
		{
		    vim_memset(newp + newlen, c, (size_t)numc);
		    newlen += numc;
		}
		सचे (!bd.is_short)
		{
		    // insert post-spaces
		    vim_memset(newp + newlen, ' ', (size_t)bd.endspaces);
		    // copy the part after the changed part
		    STRCPY(newp + newlen + bd.endspaces,
				oldp + bd.textcol + bd.textlen);
		}
	    }
	    अञ्ञथा
	    {
		// Replacing with \r or \n means splitting the line.
		after_p = alloc(oldlen + 1 + n - newlen);
		सचे (after_p != रिक्त)
		    STRCPY(after_p, oldp + bd.textcol + bd.textlen);
	    }

	    // replace the line
	    ml_replace(curwin->w_cursor.lnum, newp, FALSE);
	    सचे (after_p != रिक्त)
	    {
		ml_append(curwin->w_cursor.lnum++, after_p, 0, FALSE);
		appended_पङ्क्तयः_mark(curwin->w_cursor.lnum, 1L);
		oap->end.lnum++;
		vim_free(after_p);
	    }
	}
    }
    अञ्ञथा
    {
	/*
	 * MCHAR and MLINE motion replace.
	 */
	सचे (oap->motion_type == MLINE)
	{
	    oap->start.col = 0;
	    curwin->w_cursor.col = 0;
	    oap->end.col = ml_get_len(oap->end.lnum);
	    सचे (oap->end.col)
		--oap->end.col;
	}
	अञ्ञथा सचे (!oap->inclusive)
	    dec(&(oap->end));

	याव (LTOREQ_POS(curwin->w_cursor, oap->end))
	{
	    पूर्णाङ्क done = FALSE;

	    n = gchar_cursor();
	    सचे (n != NUL)
	    {
		पूर्णाङ्क new_byte_len = (*mb_char2len)(c);
		पूर्णाङ्क old_byte_len = mb_ptr2len(ml_get_cursor());

		सचे (new_byte_len > 1 || old_byte_len > 1)
		{
		    // This is slow, but it handles replacing a single-byte
		    // with a multi-byte and the other way around.
		    सचे (curwin->w_cursor.lnum == oap->end.lnum)
			oap->end.col += new_byte_len - old_byte_len;
		    replace_character(c);
		    done = TRUE;
		}
		अञ्ञथा
		{
		    सचे (n == TAB)
		    {
			पूर्णाङ्क end_vcol = 0;

			सचे (curwin->w_cursor.lnum == oap->end.lnum)
			{
			    // oap->end has to be recalculated when
			    // the tab breaks
			    end_vcol = getviscol2(oap->end.col,
							     oap->end.coladd);
			}
			coladvance_force(getviscol());
			सचे (curwin->w_cursor.lnum == oap->end.lnum)
			    getvpos(&oap->end, end_vcol);
		    }
		    // with "coladd" set may move to just after a TAB
		    सचे (gchar_cursor() != NUL)
		    {
			PBYTE(curwin->w_cursor, c);
			done = TRUE;
		    }
		}
	    }
	    सचे (!done && virtual_op && curwin->w_cursor.lnum == oap->end.lnum)
	    {
		पूर्णाङ्क virtcols = oap->end.coladd;

		सचे (curwin->w_cursor.lnum == oap->start.lnum
			&& oap->start.col == oap->end.col && oap->start.coladd)
		    virtcols -= oap->start.coladd;

		// oap->end has been trimmed so it's effectively inclusive;
		// as a result an extra +1 must be counted so we don't
		// trample the NUL byte.
		coladvance_force(getviscol2(oap->end.col, oap->end.coladd) + 1);
		curwin->w_cursor.col -= (virtcols + 1);
		अत्थाय (; virtcols >= 0; virtcols--)
		{
		    सचे ((*mb_char2len)(c) > 1)
		       replace_character(c);
		    अञ्ञथा
			PBYTE(curwin->w_cursor, c);
		    सचे (inc(&curwin->w_cursor) == -1)
			भङ्ग;
		}
	    }

	    // Advance to next character, stop at the end of the file.
	    सचे (inc_cursor() == -1)
		भङ्ग;
	}
    }

    curwin->w_cursor = oap->start;
    check_cursor();
    changed_पङ्क्तयः(oap->start.lnum, oap->start.col, oap->end.lnum + 1, 0L);

    सचे ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
    }

    पटिनिवत्त OK;
}

स्थिर पूर्णाङ्क swapchars(पूर्णाङ्क op_type, pos_T *pos, पूर्णाङ्क length);

/*
 * Handle the (non-standard vi) tilde operator.  Also for "gu", "gU" and "g?".
 */
    स्थिर शून्य
op_tilde(oparg_T *oap)
{
    pos_T		pos;
    संरचना block_def	bd;
    पूर्णाङ्क			did_change = FALSE;

    सचे (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	पटिनिवत्त;

    pos = oap->start;
    सचे (oap->block_mode)		    // Visual block mode
    {
	अत्थाय (; pos.lnum <= oap->end.lnum; ++pos.lnum)
	{
	    पूर्णाङ्क one_change;

	    block_prep(oap, &bd, pos.lnum, FALSE);
	    pos.col = bd.textcol;
	    one_change = swapchars(oap->op_type, &pos, bd.textlen);
	    did_change |= one_change;

#ifdef FEAT_NETBEANS_INTG
	    सचे (netbeans_active() && one_change)
	    {
		char_u *ptr;

		netbeans_removed(curbuf, pos.lnum, bd.textcol,
							    (long)bd.textlen);
		// get the line now, it may have been flushed
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, bd.textcol,
						&ptr[bd.textcol], bd.textlen);
	    }
#endif
	}
	सचे (did_change)
	    changed_पङ्क्तयः(oap->start.lnum, 0, oap->end.lnum + 1, 0L);
    }
    अञ्ञथा				    // not block mode
    {
	सचे (oap->motion_type == MLINE)
	{
	    oap->start.col = 0;
	    pos.col = 0;
	    oap->end.col = ml_get_len(oap->end.lnum);
	    सचे (oap->end.col)
		--oap->end.col;
	}
	अञ्ञथा सचे (!oap->inclusive)
	    dec(&(oap->end));

	सचे (pos.lnum == oap->end.lnum)
	    did_change = swapchars(oap->op_type, &pos,
						  oap->end.col - pos.col + 1);
	अञ्ञथा
	    अत्थाय (;;)
	    {
		did_change |= swapchars(oap->op_type, &pos,
			pos.lnum == oap->end.lnum ? oap->end.col + 1
						  : ml_get_pos_len(&pos));
		सचे (LTOREQ_POS(oap->end, pos) || inc(&pos) == -1)
		    भङ्ग;
	    }
	सचे (did_change)
	{
	    changed_पङ्क्तयः(oap->start.lnum, oap->start.col, oap->end.lnum + 1,
									  0L);
#ifdef FEAT_NETBEANS_INTG
	    सचे (netbeans_active())
	    {
		char_u *ptr;
		पूर्णाङ्क गणनम्;

		pos = oap->start;
		याव (pos.lnum < oap->end.lnum)
		{
		    ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		    गणनम् = ml_get_buf_len(curbuf, pos.lnum) - pos.col;
		    netbeans_removed(curbuf, pos.lnum, pos.col, (long)गणनम्);
		    // get the line again, it may have been flushed
		    ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		    netbeans_inserted(curbuf, pos.lnum, pos.col,
							&ptr[pos.col], गणनम्);
		    pos.col = 0;
		    pos.lnum++;
		}
		गणनम् = oap->end.col - pos.col + 1;
		netbeans_removed(curbuf, pos.lnum, pos.col, (long)गणनम्);
		// get the line again, it may have been flushed
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, pos.col,
							&ptr[pos.col], गणनम्);
	    }
#endif
	}
    }

    सचे (!did_change && oap->is_VIsual)
	// No change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    सचे ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set '[ and '] marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
    }

    सचे (oap->पङ्क्तिः_गणनम् > p_report)
	smsg(NGETTEXT("%ld line changed", "%ld lines changed",
					    oap->पङ्क्तिः_गणनम्), oap->पङ्क्तिः_गणनम्);
}

/*
 * Invoke swapchar() on "length" bytes at position "pos".
 * "pos" is advanced to just after the changed characters.
 * "length" is rounded up to include the whole last multi-byte character.
 * Also works correctly when the number of bytes changes.
 * Returns TRUE if some character was changed.
 */
    स्थिर पूर्णाङ्क
swapchars(पूर्णाङ्क op_type, pos_T *pos, पूर्णाङ्क length)
{
    पूर्णाङ्क todo;
    पूर्णाङ्क	did_change = 0;

    अत्थाय (todo = length; todo > 0; --todo)
    {
	सचे (has_mbyte)
	{
	    पूर्णाङ्क len = (*mb_ptr2len)(ml_get_pos(pos));

	    // we're counting bytes, not characters
	    सचे (len > 0)
		todo -= len - 1;
	}
	did_change |= swapchar(op_type, pos);
	सचे (inc(pos) == -1)    // at end of file
	    भङ्ग;
    }
    पटिनिवत्त did_change;
}

/*
 * If op_type == OP_UPPER: make uppercase,
 * if op_type == OP_LOWER: make lowercase,
 * if op_type == OP_ROT13: do rot13 encoding,
 * else swap case of character at 'pos'
 * returns TRUE when something actually changed.
 */
    पूर्णाङ्क
swapchar(पूर्णाङ्क op_type, pos_T *pos)
{
    पूर्णाङ्क	    c;
    पूर्णाङ्क	    nc;

    c = gchar_pos(pos);

    // Only do rot13 encoding for ASCII characters.
    सचे (c >= 0x80 && op_type == OP_ROT13)
	पटिनिवत्त FALSE;

    // ~ is OP_NOP, g~ is OP_TILDE, gU is OP_UPPER
    सचे ((op_type == OP_UPPER || op_type == OP_NOP || op_type == OP_TILDE)
	    && c == 0xdf
	    && (enc_latin1like || STRCMP(p_enc, "iso-8859-2") == 0))
    {
	pos_T   sp = curwin->w_cursor;

	// Special handling for lowercase German sharp s (ß): convert to uppercase (ẞ).
	curwin->w_cursor = *pos;
	del_char(FALSE);
	ins_char(0x1E9E);
	curwin->w_cursor = sp;
	पटिनिवत्त TRUE;
    }

    सचे (enc_dbcs != 0 && c >= 0x100)	// No lower/uppercase letter
	पटिनिवत्त FALSE;
    nc = c;
    सचे (MB_ISLOWER(c))
    {
	सचे (op_type == OP_ROT13)
	    nc = ROT13(c, 'a');
	अञ्ञथा सचे (op_type != OP_LOWER)
	    nc = MB_TOUPPER(c);
    }
    अञ्ञथा सचे (MB_ISUPPER(c))
    {
	सचे (op_type == OP_ROT13)
	    nc = ROT13(c, 'A');
	अञ्ञथा सचे (op_type != OP_UPPER)
	    nc = MB_TOLOWER(c);
    }
    सचे (nc != c)
    {
	सचे (enc_utf8 && (c >= 0x80 || nc >= 0x80))
	{
	    pos_T   sp = curwin->w_cursor;

	    curwin->w_cursor = *pos;
	    // don't use del_char(), it also removes composing chars
	    del_bytes(utf_ptr2len(ml_get_cursor()), FALSE, FALSE);
	    ins_char(nc);
	    curwin->w_cursor = sp;
	}
	अञ्ञथा
	    PBYTE(*pos, nc);
	पटिनिवत्त TRUE;
    }
    पटिनिवत्त FALSE;
}

/*
 * op_insert - Insert and append operators for Visual mode.
 */
    शून्य
op_निवेशय(oparg_T *oap, long count1)
{
    long		pre_textlen = 0;
    colnr_T		ind_pre_col = 0, ind_post_col;
    पूर्णाङ्क			ind_pre_vcol = 0, ind_post_vcol = 0;
    संरचना block_def	bd;
    पूर्णाङ्क			i;
    pos_T		t1;
    pos_T		start_निवेशय;

    // edit() changes this - record it for OP_APPEND
    bd.is_MAX = (curwin->w_curswant == MAXCOL);

    // vis block is still marked. Get rid of it now.
    curwin->w_cursor.lnum = oap->start.lnum;
    update_screen(UPD_INVERTED);

    सचे (oap->block_mode)
    {
	// When 'virtualedit' is used, need to insert the extra spaces before
	// doing block_prep().  When only "block" is used, virtual edit is
	// already disabled, but still need it when calling
	// coladvance_force().
	// coladvance_force() uses get_ve_flags() to get the 'virtualedit'
	// state for the current window.  To override that state, we need to
	// set the window-local value of ve_flags rather than the global value.
	सचे (curwin->w_cursor.coladd > 0)
	{
	    पूर्णाङ्क		old_ve_flags = curwin->w_ve_flags;

	    सचे (u_save_cursor() == FAIL)
		पटिनिवत्त;

	    curwin->w_ve_flags = VE_ALL;
	    coladvance_force(oap->op_type == OP_APPEND
					   ? oap->end_vcol + 1 : getviscol());
	    सचे (oap->op_type == OP_APPEND)
		--curwin->w_cursor.col;
	    curwin->w_ve_flags = old_ve_flags;
	}
	// Get the info about the block before entering the text
	block_prep(oap, &bd, oap->start.lnum, TRUE);
	// Get indent information
	ind_pre_col = (colnr_T)getwhitecols_curline();
	ind_pre_vcol = get_indent();
	pre_textlen = ml_get_len(oap->start.lnum) - bd.textcol;
	सचे (oap->op_type == OP_APPEND)
	    pre_textlen -= bd.textlen;
    }

    सचे (oap->op_type == OP_APPEND)
    {
	सचे (oap->block_mode && curwin->w_cursor.coladd == 0)
	{
	    // Move the cursor to the character right of the block.
	    curwin->w_set_curswant = true;
	    याव (*ml_get_cursor() != NUL
		    && (curwin->w_cursor.col < bd.textcol + bd.textlen))
		++curwin->w_cursor.col;
	    सचे (bd.is_short && !bd.is_MAX)
	    {
		// First line was too short, make it longer and adjust the
		// values in "bd".
		सचे (u_save_cursor() == FAIL)
		    पटिनिवत्त;
		अत्थाय (i = 0; i < bd.endspaces; ++i)
		    ins_char(' ');
		bd.textlen += bd.endspaces;
	    }
	}
	अञ्ञथा
	{
	    curwin->w_cursor = oap->end;
	    check_cursor_col();

	    // Works just like an 'i'nsert on the next character.
	    सचे (!LINEEMPTY(curwin->w_cursor.lnum)
		    && oap->start_vcol != oap->end_vcol)
		inc_cursor();
	}
    }

    t1 = oap->start;
    start_निवेशय = curwin->w_cursor;
    (शून्य)edit(NUL, FALSE, (linenr_T)count1);

    // When a tab was inserted, and the characters in front of the tab
    // have been converted to a tab as well, the column of the cursor
    // might have actually been reduced, so need to adjust here.
    सचे (t1.lnum == curbuf->b_op_start_orig.lnum
	    && LT_POS(curbuf->b_op_start_orig, t1))
	oap->start = curbuf->b_op_start_orig;

    // If user has moved off this line, we don't know what to do, so do
    // nothing.
    // Also don't repeat the insert when Insert mode ended with CTRL-C.
    सचे (curwin->w_cursor.lnum != oap->start.lnum || got_int)
	पटिनिवत्त;

    सचे (oap->block_mode)
    {
	पूर्णाङ्क			ins_len;
	char_u			*firstline, *ins_text;
	संरचना block_def	bd2;
	पूर्णाङ्क			did_indent = FALSE;
	size_t			len;
	size_t			add;
	// offset when cursor was moved in insert mode
	पूर्णाङ्क			offset = 0;

	// If indent kicked in, the firstline might have changed
	// but only do that, if the indent actually increased.
	ind_post_col = (colnr_T)getwhitecols_curline();
	सचे (curbuf->b_op_start.col > ind_pre_col && ind_post_col > ind_pre_col)
	{
	    bd.textcol += ind_post_col - ind_pre_col;
	    ind_post_vcol = get_indent();
	    bd.start_vcol += ind_post_vcol - ind_pre_vcol;
	    did_indent = TRUE;
	}

	// The user may have moved the cursor before inserting something, try
	// to adjust the block for that.  But only do it, if the difference
	// does not come from indent kicking in.
	सचे (oap->start.lnum == curbuf->b_op_start_orig.lnum
						  && !bd.is_MAX && !did_indent)
	{
	    पूर्णाङ्क t = getviscol2(curbuf->b_op_start_orig.col,
					       curbuf->b_op_start_orig.coladd);

	    सचे (oap->op_type == OP_निवेशय
		    && oap->start.col + oap->start.coladd
			    != curbuf->b_op_start_orig.col
					      + curbuf->b_op_start_orig.coladd)
	    {
		oap->start.col = curbuf->b_op_start_orig.col;
		pre_textlen -= t - oap->start_vcol;
		oap->start_vcol = t;
	    }
	    अञ्ञथा सचे (oap->op_type == OP_APPEND
		    && oap->start.col + oap->start.coladd
			    >= curbuf->b_op_start_orig.col
					      + curbuf->b_op_start_orig.coladd)
	    {
		oap->start.col = curbuf->b_op_start_orig.col;
		// reset pre_textlen to the value of OP_INSERT
		pre_textlen += bd.textlen;
		pre_textlen -= t - oap->start_vcol;
		oap->start_vcol = t;
		oap->op_type = OP_निवेशय;
	    }
	}

	// Spaces and tabs in the indent may have changed to other spaces and
	// tabs.  Get the starting column again and correct the length.
	// Don't do this when "$" used, end-of-line will have changed.
	//
	// if indent was added and the inserted text was after the indent,
	// correct the selection for the new indent.
	सचे (did_indent && bd.textcol - ind_post_col > 0)
	{
	    oap->start.col += ind_post_col - ind_pre_col;
	    oap->start_vcol += ind_post_vcol - ind_pre_vcol;
	    oap->end.col += ind_post_col - ind_pre_col;
	    oap->end_vcol += ind_post_vcol - ind_pre_vcol;
	}
	block_prep(oap, &bd2, oap->start.lnum, TRUE);
	सचे (did_indent && bd.textcol - ind_post_col > 0)
	{
	    // undo for where "oap" is used below
	    oap->start.col -= ind_post_col - ind_pre_col;
	    oap->start_vcol -= ind_post_vcol - ind_pre_vcol;
	    oap->end.col -= ind_post_col - ind_pre_col;
	    oap->end_vcol -= ind_post_vcol - ind_pre_vcol;
	}
	सचे (!bd.is_MAX || bd2.textlen < bd.textlen)
	{
	    सचे (oap->op_type == OP_APPEND)
	    {
		pre_textlen += bd2.textlen - bd.textlen;
		सचे (bd2.endspaces)
		    --bd2.textlen;
	    }
	    bd.textcol = bd2.textcol;
	    bd.textlen = bd2.textlen;
	}

	/*
	 * Subsequent calls to ml_get() flush the firstline data - take a
	 * copy of the required string.
	 */
	firstline = ml_get(oap->start.lnum);
	len = ml_get_len(oap->start.lnum);
	add = bd.textcol;
	सचे (oap->op_type == OP_APPEND)
	{
	    add += bd.textlen;
	    // account for pressing cursor in insert mode when '$' was used
	    सचे (bd.is_MAX
		&& (start_निवेशय.lnum == Insstart.lnum
					   && start_निवेशय.col > Insstart.col))
	    {
		offset = (start_निवेशय.col - Insstart.col);
		add -= offset;
		सचे (oap->end_vcol > offset)
		    oap->end_vcol -= (offset + 1);
		अञ्ञथा
		    // moved outside of the visual block, what to do?
		    पटिनिवत्त;
	    }
	}
	सचे (add > len)
	    add = len;  // short line, point to the NUL
	firstline += add;
	len -= add;
	सचे (pre_textlen >= 0 && (ins_len = (पूर्णाङ्क)len - pre_textlen - offset) > 0)
	{
	    ins_text = vim_strnsave(firstline, ins_len);
	    सचे (ins_text != रिक्त)
	    {
		// block handled here
		सचे (u_save(oap->start.lnum,
					 (linenr_T)(oap->end.lnum + 1)) == OK)
		    block_निवेशय(oap, ins_text, ins_len, (oap->op_type == OP_निवेशय),
									 &bd);

		curwin->w_cursor.col = oap->start.col;
		check_cursor();
		vim_free(ins_text);
	    }
	}
    }
}

/*
 * op_change - handle a change operation
 *
 * return TRUE if edit() returns because of a CTRL-O command
 */
    पूर्णाङ्क
op_change(oparg_T *oap)
{
    colnr_T		l;
    पूर्णाङ्क			retval;
    linenr_T		linenr;
    long		pre_textlen = 0;
    long		pre_indent = 0;
    char_u		*firstline;
    char_u		*ins_text, *newp, *oldp;
    संरचना block_def	bd;

    l = oap->start.col;
    सचे (oap->motion_type == MLINE)
    {
	l = 0;
	can_si = may_do_si();	// Like opening a new line, do smart indent
    }

    // First delete the text in the region.  In an empty buffer only need to
    // save for undo
    सचे (curbuf->b_ml.ml_flags & ML_EMPTY)
    {
	सचे (u_save_cursor() == FAIL)
	    पटिनिवत्त FALSE;
    }
    अञ्ञथा सचे (op_delete(oap) == FAIL)
	पटिनिवत्त FALSE;

    सचे ((l > curwin->w_cursor.col) && !LINEEMPTY(curwin->w_cursor.lnum)
							 && !virtual_op)
	inc_cursor();

    // check for still on same line (<CR> in inserted text meaningless)
    // skip blank lines too
    सचे (oap->block_mode)
    {
	// Add spaces before getting the current line length.
	सचे (virtual_op && (curwin->w_cursor.coladd > 0
						    || gchar_cursor() == NUL))
	    coladvance_force(getviscol());
	firstline = ml_get(oap->start.lnum);
	pre_textlen = ml_get_len(oap->start.lnum);
	pre_indent = (long)getwhitecols(firstline);
	bd.textcol = curwin->w_cursor.col;
    }

    सचे (oap->motion_type == MLINE)
	fix_indent();

    // Reset finish_op now, don't want it set inside edit().
    पूर्णाङ्क save_finish_op = finish_op;
    finish_op = FALSE;

    retval = edit(NUL, FALSE, (linenr_T)1);

    finish_op = save_finish_op;

    /*
     * In Visual block mode, handle copying the new text to all lines of the
     * block.
     * Don't repeat the insert when Insert mode ended with CTRL-C.
     */
    सचे (oap->block_mode && oap->start.lnum != oap->end.lnum && !got_int)
    {
	पूर्णाङ्क	ins_len;

	// Auto-indenting may have changed the indent.  If the cursor was past
	// the indent, exclude that indent change from the inserted text.
	firstline = ml_get(oap->start.lnum);
	सचे (bd.textcol > (colnr_T)pre_indent)
	{
	    long new_indent = (long)getwhitecols(firstline);

	    pre_textlen += new_indent - pre_indent;
	    bd.textcol += new_indent - pre_indent;
	}

	ins_len = (पूर्णाङ्क)ml_get_len(oap->start.lnum) - pre_textlen;
	सचे (ins_len > 0)
	{
	    // Subsequent calls to ml_get() flush the firstline data - take a
	    // copy of the inserted text.
	    सचे ((ins_text = alloc(ins_len + 1)) != रिक्त)
	    {
		vim_strncpy(ins_text, firstline + bd.textcol, ins_len);
		अत्थाय (linenr = oap->start.lnum + 1; linenr <= oap->end.lnum;
								     linenr++)
		{
		    block_prep(oap, &bd, linenr, TRUE);
		    सचे (!bd.is_short || virtual_op)
		    {
			pos_T vpos;
			size_t newlen;

			// If the block starts in virtual space, count the
			// initial coladd offset as part of "startspaces"
			सचे (bd.is_short)
			{
			    vpos.lnum = linenr;
			    (शून्य)getvpos(&vpos, oap->start_vcol);
			}
			अञ्ञथा
			    vpos.coladd = 0;
			oldp = ml_get(linenr);
			newp = alloc(ml_get_len(linenr) + vpos.coladd + ins_len + 1);
			सचे (newp == रिक्त)
			    अनुवर्त;
			// copy up to block start
			mch_memmove(newp, oldp, (size_t)bd.textcol);
			newlen = bd.textcol;
			vim_memset(newp + newlen, ' ', (size_t)vpos.coladd);
			newlen += vpos.coladd;
			mch_memmove(newp + newlen, ins_text, ins_len);
			STRCPY(newp + newlen + ins_len, oldp + bd.textcol);
			ml_replace(linenr, newp, FALSE);
#ifdef FEAT_PROP_POPUP
			// Shift the properties for linenr as edit() would do.
			सचे (curbuf->b_has_textprop)
			    adjust_prop_columns(linenr, bd.textcol,
						     vpos.coladd + (पूर्णाङ्क)ins_len, 0);
#endif
		    }
		}
		check_cursor();

		changed_पङ्क्तयः(oap->start.lnum + 1, 0, oap->end.lnum + 1, 0L);
	    }
	    vim_free(ins_text);
	}
    }
    auto_format(FALSE, TRUE);

    पटिनिवत्त retval;
}

/*
 * When the cursor is on the NUL past the end of the line and it should not be
 * there move it left.
 */
    शून्य
adjust_cursor_eol(शून्य)
{
    unsigned पूर्णाङ्क cur_ve_flags = get_ve_flags();

    पूर्णाङ्क adj_cursor = (curwin->w_cursor.col > 0
				&& gchar_cursor() == NUL
				&& (cur_ve_flags & VE_ONEMORE) == 0
				&& (cur_ve_flags & VE_ALL) == 0
				&& !(restart_edit || (State & MODE_निवेशय)));
    सचे (!adj_cursor)
	पटिनिवत्त;

    // Put the cursor on the last character in the line.
    dec_cursor();

    सचे (cur_ve_flags == VE_ALL)
    {
	colnr_T	    scol, ecol;

	// Coladd is set to the width of the last character.
	getvcol(curwin, &curwin->w_cursor, &scol, रिक्त, &ecol, 0);
	curwin->w_cursor.coladd = ecol - scol + 1;
    }
}

/*
 * If "process" is TRUE and the line begins with a comment leader (possibly
 * after some white space), return a pointer to the text after it. Put a boolean
 * value indicating whether the line ends with an unclosed comment in
 * "is_comment".
 * line - line to be processed,
 * process - if FALSE, will only check whether the line ends with an unclosed
 *	     comment,
 * include_space - whether to also skip space following the comment leader,
 * is_comment - will indicate whether the current line ends with an unclosed
 *		comment.
 */
    char_u *
skip_comment(
    char_u   *पङ्क्तिः,
    पूर्णाङ्क      process,
    पूर्णाङ्क	     include_space,
    पूर्णाङ्क      *is_comment)
{
    char_u *comment_flags = रिक्त;
    पूर्णाङ्क    lead_len;
    पूर्णाङ्क    leader_offset = get_last_leader_offset(पङ्क्तिः, &comment_flags);

    *is_comment = FALSE;
    सचे (leader_offset != -1)
    {
	// Let's check whether the line ends with an unclosed comment.
	// If the last comment leader has COM_END in flags, there's no comment.
	याव (*comment_flags)
	{
	    सचे (*comment_flags == COM_END
		    || *comment_flags == ':')
		भङ्ग;
	    ++comment_flags;
	}
	सचे (*comment_flags != COM_END)
	    *is_comment = TRUE;
    }

    सचे (process == FALSE)
	पटिनिवत्त पङ्क्तिः;

    lead_len = get_leader_len(पङ्क्तिः, &comment_flags, FALSE, include_space);

    सचे (lead_len == 0)
	पटिनिवत्त पङ्क्तिः;

    // Find:
    // - COM_END,
    // - colon,
    // whichever comes first.
    याव (*comment_flags)
    {
	सचे (*comment_flags == COM_END
		|| *comment_flags == ':')
	    भङ्ग;
	++comment_flags;
    }

    // If we found a colon, it means that we are not processing a line
    // starting with a closing part of a three-part comment. That's good,
    // because we don't want to remove those as this would be annoying.
    सचे (*comment_flags == ':' || *comment_flags == NUL)
	पङ्क्तिः += lead_len;

    पटिनिवत्त पङ्क्तिः;
}

/*
 * Join 'count' lines (minimal 2) at the cursor position.
 * When "save_undo" is TRUE save lines for undo first.
 * Set "use_formatoptions" to FALSE when e.g. processing backspace and comment
 * leaders should not be removed.
 * When setmark is TRUE, sets the '[ and '] mark, else, the caller is expected
 * to set those marks.
 *
 * return FAIL for failure, OK otherwise
 */
    पूर्णाङ्क
do_join(
    long    गणनम्,
    पूर्णाङ्क	    निवेशय_space,
    पूर्णाङ्क	    save_undo,
    पूर्णाङ्क	    use_formatoptions UNUSED,
    पूर्णाङ्क	    setmark)
{
    char_u	*curr = रिक्त;
    char_u      *curr_start = रिक्त;
    char_u	*cend;
    char_u	*newp;
    size_t	newp_len;
    char_u	*spaces;	// number of spaces inserted before a line
    पूर्णाङ्क		endcurr1 = NUL;
    पूर्णाङ्क		endcurr2 = NUL;
    पूर्णाङ्क		currsize = 0;	// size of the current line
    पूर्णाङ्क		sumsize = 0;	// size of the long new line
    linenr_T	t;
    colnr_T	col = 0;
    पूर्णाङ्क		ret = OK;
    पूर्णाङ्क		*comments = रिक्त;
    पूर्णाङ्क		remove_comments = (use_formatoptions == TRUE)
				  && has_format_option(FO_REMOVE_COMS);
    पूर्णाङ्क		prev_was_comment;

    सचे (save_undo && u_save((linenr_T)(curwin->w_cursor.lnum - 1),
			    (linenr_T)(curwin->w_cursor.lnum + गणनम्)) == FAIL)
	पटिनिवत्त FAIL;

    // Allocate an array to store the number of spaces inserted before each
    // line.  We will use it to pre-compute the length of the new line and the
    // proper placement of each original line in the new one.
    spaces = lalloc_clear(गणनम्, TRUE);
    सचे (spaces == रिक्त)
	पटिनिवत्त FAIL;
    सचे (remove_comments)
    {
	comments = lalloc_clear(गणनम् * परिमाण(पूर्णाङ्क), TRUE);
	सचे (comments == रिक्त)
	{
	    vim_free(spaces);
	    पटिनिवत्त FAIL;
	}
    }

    /*
     * Don't move anything yet, just compute the final line length
     * and setup the array of space strings lengths
     * This loops forward over the joined lines.
     */
    अत्थाय (t = 0; t < गणनम्; ++t)
    {
	curr = curr_start = ml_get((linenr_T)(curwin->w_cursor.lnum + t));
	सचे (t == 0 && setmark && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set the '[ mark.
	    curwin->w_buffer->b_op_start.lnum = curwin->w_cursor.lnum;
	    curwin->w_buffer->b_op_start.col  = (colnr_T)STRLEN(curr);
	}
	सचे (remove_comments)
	{
	    // We don't want to remove the comment leader if the
	    // previous line is not a comment.
	    सचे (t > 0 && prev_was_comment)
	    {

		char_u *new_curr = skip_comment(curr, TRUE, निवेशय_space,
							   &prev_was_comment);
		comments[t] = (पूर्णाङ्क)(new_curr - curr);
		curr = new_curr;
	    }
	    अञ्ञथा
		curr = skip_comment(curr, FALSE, निवेशय_space,
							   &prev_was_comment);
	}

	सचे (निवेशय_space && t > 0)
	{
	    curr = skipwhite(curr);
	    सचे (*curr != NUL && *curr != ')'
		    && sumsize != 0 && endcurr1 != TAB
		    && (!has_format_option(FO_MBYTE_JOIN)
			|| (mb_ptr2char(curr) < 0x100 && endcurr1 < 0x100))
		    && (!has_format_option(FO_MBYTE_JOIN2)
			|| (mb_ptr2char(curr) < 0x100
			    && !(enc_utf8 && utf_eat_space(endcurr1)))
			|| (endcurr1 < 0x100
			    && !(enc_utf8 && utf_eat_space(mb_ptr2char(curr)))))
	       )
	    {
		// don't add a space if the line is ending in a space
		सचे (endcurr1 == ' ')
		    endcurr1 = endcurr2;
		अञ्ञथा
		    ++spaces[t];
		// extra space when 'joinspaces' set and line ends in '.'
		सचे (       p_js
			&& (endcurr1 == '.'
			    || (vim_strchr(p_cpo, CPO_JOINSP) == रिक्त
				&& (endcurr1 == '?' || endcurr1 == '!'))))
		    ++spaces[t];
	    }
	}
	currsize = (पूर्णाङ्क)STRLEN(curr);
	sumsize += currsize + spaces[t];
	endcurr1 = endcurr2 = NUL;
	सचे (निवेशय_space && currsize > 0)
	{
	    सचे (has_mbyte)
	    {
		cend = curr + currsize;
		MB_PTR_BACK(curr, cend);
		endcurr1 = (*mb_ptr2char)(cend);
		सचे (cend > curr)
		{
		    MB_PTR_BACK(curr, cend);
		    endcurr2 = (*mb_ptr2char)(cend);
		}
	    }
	    अञ्ञथा
	    {
		endcurr1 = *(curr + currsize - 1);
		सचे (currsize > 1)
		    endcurr2 = *(curr + currsize - 2);
	    }
	}
	पङ्क्तिः_breakcheck();
	सचे (got_int)
	{
	    ret = FAIL;
	    गच्छ theend;
	}
    }

    // store the column position before last line
    col = sumsize - currsize - spaces[गणनम् - 1];

    // allocate the space for the new line
    newp_len = sumsize + 1;
    newp = alloc(newp_len);
    सचे (newp == रिक्त)
    {
	ret = FAIL;
	गच्छ theend;
    }
    cend = newp + sumsize;
    *cend = 0;

    /*
     * Move affected lines to the new long one.
     * This loops backwards over the joined lines, including the original line.
     *
     * Move marks from each deleted line to the joined line, adjusting the
     * column.  This is not Vi compatible, but Vi deletes the marks, thus that
     * should not really be a problem.
     */
#ifdef FEAT_PROP_POPUP
    unpacked_memline_T um = um_open_at_no_props(
				curwin->w_buffer, curwin->w_cursor.lnum, 0);
    // um_open_at_no_props may have invalidated "curr".
    पूर्णाङ्क curr_off = (पूर्णाङ्क)(curr - curr_start);

    curr = curr_start = ml_get((linenr_T)(curwin->w_cursor.lnum + गणनम् - 1));
    curr += curr_off;
#endif

    अत्थाय (t = गणनम् - 1; ; --t)
    {
	पूर्णाङ्क spaces_removed;

	cend -= currsize;
	mch_memmove(cend, curr, (size_t)currsize);

	सचे (spaces[t] > 0)
	{
	    cend -= spaces[t];
	    vim_memset(cend, ' ', (size_t)(spaces[t]));
	}

	// If deleting more spaces than adding, the cursor moves no more than
	// what is added if it is inside these spaces.
	spaces_removed = (curr - curr_start) - spaces[t];

	mark_col_adjust(curwin->w_cursor.lnum + t, (colnr_T)0, -t,
			 (long)(cend - newp - spaces_removed), spaces_removed);
#ifdef FEAT_PROP_POPUP
	prepend_joined_props(&um, curwin->w_cursor.lnum + t,
		t == गणनम् - 1, (long)(cend - newp), spaces_removed);
#endif
	सचे (t == 0)
	    भङ्ग;
	curr = curr_start = ml_get((linenr_T)(curwin->w_cursor.lnum + t - 1));
	सचे (remove_comments)
	    curr += comments[t - 1];
	सचे (निवेशय_space && t > 1)
	    curr = skipwhite(curr);
	currsize = (पूर्णाङ्क)STRLEN(curr);
    }

#ifdef FEAT_PROP_POPUP
    सचे (um.buf != रिक्त)
    {
	um_set_text(&um, newp);
	um_reverse_props(&um);
	um_close(&um);
	newp = रिक्त;  // um_set_text took ownership
    }
    अञ्ञथा
#endif
    ml_replace_len(curwin->w_cursor.lnum, newp, (colnr_T)newp_len, TRUE, FALSE);

    सचे (setmark && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set the '] mark.
	curwin->w_buffer->b_op_end.lnum = curwin->w_cursor.lnum;
	curwin->w_buffer->b_op_end.col  = (colnr_T)sumsize;
    }

    // Only report the change in the first line here, del_lines() will report
    // the deleted line.
    changed_पङ्क्तयः(curwin->w_cursor.lnum, currsize,
					       curwin->w_cursor.lnum + 1, 0L);
    /*
     * Delete following lines. To do this we move the cursor there
     * briefly, and then move it back. After del_lines() the cursor may
     * have moved up (last line deleted), so the current lnum is kept in t.
     */
    t = curwin->w_cursor.lnum;
    ++curwin->w_cursor.lnum;
    del_पङ्क्तयः(गणनम् - 1, FALSE);
    curwin->w_cursor.lnum = t;

    /*
     * Set the cursor column:
     * Vi compatible: use the column of the first join
     * vim:	      use the column of the last join
     */
    curwin->w_cursor.col =
		    (vim_strchr(p_cpo, CPO_JOINCOL) != रिक्त ? currsize : col);
    check_cursor_col();

    curwin->w_cursor.coladd = 0;
    curwin->w_set_curswant = true;

theend:
    vim_free(spaces);
    सचे (remove_comments)
	vim_free(comments);
    पटिनिवत्त ret;
}

#ifdef FEAT_LINEBREAK
/*
 * TODO: consider using "tailp" of win_lbr_chartabsize() instead of temporarily
 * resetting 'linebreak'.
 *
 * Reset 'linebreak' and take care of side effects.
 * Returns the previous value, to be passed to restore_lbr().
 */
    पूर्णाङ्क
reset_lbr(शून्य)
{
    सचे (!curwin->w_p_lbr)
	पटिनिवत्त FALSE;
    // changing 'linebreak' may require w_virtcol to be updated
    curwin->w_p_lbr = FALSE;
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);
    पटिनिवत्त TRUE;
}

/*
 * Restore 'linebreak' and take care of side effects.
 */
    शून्य
restore_lbr(पूर्णाङ्क lbr_saved)
{
    सचे (curwin->w_p_lbr || !lbr_saved)
	पटिनिवत्त;

    // changing 'linebreak' may require w_virtcol to be updated
    curwin->w_p_lbr = TRUE;
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);
}
#endif

/*
 * prepare a few things for block mode yank/delete/tilde
 *
 * for delete:
 * - textlen includes the first/last char to be (partly) deleted
 * - start/endspaces is the number of columns that are taken by the
 *   first/last deleted char minus the number of columns that have to be
 *   deleted.
 * for yank and tilde:
 * - textlen includes the first/last char to be wholly yanked
 * - start/endspaces is the number of columns of the first/last yanked char
 *   that are to be yanked.
 */
    शून्य
block_prep(
    oparg_T		*oap,
    संरचना block_def	*bdp,
    linenr_T		lnum,
    पूर्णाङ्क			is_del)
{
    पूर्णाङ्क		incr = 0;
    char_u	*pend;
    char_u	*pstart;
    char_u	*पङ्क्तिः;
    char_u	*prev_pstart;
    char_u	*prev_pend;
    chartabsize_T cts;
#ifdef FEAT_LINEBREAK
		// Avoid a problem with unwanted linebreaks in block mode.
    पूर्णाङ्क		lbr_saved = reset_lbr();
#endif

    bdp->startspaces = 0;
    bdp->endspaces = 0;
    bdp->textlen = 0;
    bdp->start_vcol = 0;
    bdp->end_vcol = 0;
    bdp->is_short = FALSE;
    bdp->is_oneChar = FALSE;
    bdp->pre_whitesp = 0;
    bdp->pre_whitesp_c = 0;
    bdp->end_char_vcols = 0;
    bdp->start_char_vcols = 0;

    पङ्क्तिः = ml_get(lnum);
    prev_pstart = पङ्क्तिः;
    init_chartabsize_arg(&cts, curwin, lnum, bdp->start_vcol, पङ्क्तिः, पङ्क्तिः);
    याव (cts.cts_vcol < oap->start_vcol && *cts.cts_ptr != NUL)
    {
	// Count a tab for what it's worth (if list mode not on)
	incr = lbr_chartabsize(&cts);
	cts.cts_vcol += incr;
	सचे (VIM_ISWHITE(*cts.cts_ptr))
	{
	    bdp->pre_whitesp += incr;
	    bdp->pre_whitesp_c++;
	}
	अञ्ञथा
	{
	    bdp->pre_whitesp = 0;
	    bdp->pre_whitesp_c = 0;
	}
	prev_pstart = cts.cts_ptr;
	MB_PTR_ADV(cts.cts_ptr);
    }
    bdp->start_vcol = cts.cts_vcol;
    pstart = cts.cts_ptr;
    clear_chartabsize_arg(&cts);

    bdp->start_char_vcols = incr;
    सचे (bdp->start_vcol < oap->start_vcol)	// line too short
    {
	bdp->end_vcol = bdp->start_vcol;
	bdp->is_short = TRUE;
	सचे (!is_del || oap->op_type == OP_APPEND)
	    bdp->endspaces = oap->end_vcol - oap->start_vcol + 1;
    }
    अञ्ञथा
    {
	// notice: this converts partly selected Multibyte characters to
	// spaces, too.
	bdp->startspaces = bdp->start_vcol - oap->start_vcol;
	सचे (is_del && bdp->startspaces)
	    bdp->startspaces = bdp->start_char_vcols - bdp->startspaces;
	pend = pstart;
	bdp->end_vcol = bdp->start_vcol;
	सचे (bdp->end_vcol > oap->end_vcol)	// it's all in one character
	{
	    bdp->is_oneChar = TRUE;
	    सचे (oap->op_type == OP_निवेशय)
		bdp->endspaces = bdp->start_char_vcols - bdp->startspaces;
	    अञ्ञथा सचे (oap->op_type == OP_APPEND)
	    {
		bdp->startspaces += oap->end_vcol - oap->start_vcol + 1;
		bdp->endspaces = bdp->start_char_vcols - bdp->startspaces;
	    }
	    अञ्ञथा
	    {
		bdp->startspaces = oap->end_vcol - oap->start_vcol + 1;
		सचे (is_del && oap->op_type != OP_LSHIFT)
		{
		    // just putting the sum of those two into
		    // bdp->startspaces doesn't work for Visual replace,
		    // so we have to split the tab in two
		    bdp->startspaces = bdp->start_char_vcols
					- (bdp->start_vcol - oap->start_vcol);
		    bdp->endspaces = bdp->end_vcol - oap->end_vcol - 1;
		}
	    }
	}
	अञ्ञथा
	{
	    init_chartabsize_arg(&cts, curwin, lnum, bdp->end_vcol,
								  पङ्क्तिः, pend);
	    prev_pend = pend;
	    याव (cts.cts_vcol <= oap->end_vcol && *cts.cts_ptr != NUL)
	    {
		// count a tab for what it's worth (if list mode not on)
		prev_pend = cts.cts_ptr;
		incr = lbr_chartabsize_adv(&cts);
		cts.cts_vcol += incr;
	    }
	    bdp->end_vcol = cts.cts_vcol;
	    pend = cts.cts_ptr;
	    clear_chartabsize_arg(&cts);

	    सचे (bdp->end_vcol <= oap->end_vcol
		    && (!is_del
			|| oap->op_type == OP_APPEND
			|| oap->op_type == OP_REPLACE)) // line too short
	    {
		bdp->is_short = TRUE;
		// Alternative: include spaces to fill up the block.
		// Disadvantage: can lead to trailing spaces when the line is
		// short where the text is put
		// if (!is_del || oap->op_type == OP_APPEND)
		सचे (oap->op_type == OP_APPEND || virtual_op)
		    bdp->endspaces = oap->end_vcol - bdp->end_vcol
							     + oap->inclusive;
		अञ्ञथा
		    bdp->endspaces = 0; // replace doesn't add characters
	    }
	    अञ्ञथा सचे (bdp->end_vcol > oap->end_vcol)
	    {
		bdp->endspaces = bdp->end_vcol - oap->end_vcol - 1;
		सचे (!is_del && bdp->endspaces)
		{
		    bdp->endspaces = incr - bdp->endspaces;
		    सचे (pend != pstart)
			pend = prev_pend;
		}
	    }
	}
	bdp->end_char_vcols = incr;
	सचे (is_del && bdp->startspaces)
	    pstart = prev_pstart;
	bdp->textlen = (पूर्णाङ्क)(pend - pstart);
    }
    bdp->textcol = (colnr_T) (pstart - पङ्क्तिः);
    bdp->textstart = pstart;
#ifdef FEAT_LINEBREAK
    restore_lbr(lbr_saved);
#endif
}

/*
 * Get block text from "start" to "end"
 */
    शून्य
charwise_block_prep(
    pos_T		start,
    pos_T		end,
    संरचना block_def	*bdp,
    linenr_T		lnum,
    पूर्णाङ्क			inclusive)
{
    colnr_T startcol = 0, endcol = MAXCOL;
    colnr_T cs, ce;
    char_u *p;
    पूर्णाङ्क	plen = ml_get_len(lnum);

    p = ml_get(lnum);
    bdp->startspaces = 0;
    bdp->endspaces = 0;
    bdp->is_oneChar = FALSE;
    bdp->start_char_vcols = 0;

    सचे (lnum == start.lnum)
    {
	startcol = start.col;
	सचे (virtual_op)
	{
	    getvcol(curwin, &start, &cs, रिक्त, &ce, 0);
	    सचे (ce != cs && start.coladd > 0)
	    {
		// Part of a tab selected -- but don't
		// double-count it.
		bdp->start_char_vcols = ce - cs + 1;
		bdp->startspaces = bdp->start_char_vcols - start.coladd;
		सचे (bdp->startspaces < 0)
		    bdp->startspaces = 0;
		startcol++;
	    }
	}
    }

    सचे (lnum == end.lnum)
    {
	endcol = end.col;
	सचे (virtual_op)
	{
	    getvcol(curwin, &end, &cs, रिक्त, &ce, 0);
	    सचे (p[endcol] == NUL || (cs + end.coladd < ce
			// Don't add space for double-wide
			// char; endcol will be on last byte
			// of multi-byte char.
			&& (*mb_head_off)(p, p + endcol) == 0))
	    {
		सचे (start.lnum == end.lnum && start.col == end.col)
		{
		    // Special case: inside a single char
		    bdp->is_oneChar = TRUE;
		    bdp->startspaces = end.coladd - start.coladd + inclusive;
		    endcol = startcol;
		}
		अञ्ञथा
		{
		    bdp->endspaces = end.coladd + inclusive;
		    endcol -= inclusive;
		}
	    }
	}
    }
    सचे (endcol == MAXCOL)
	endcol = ml_get_len(lnum);
    सचे (startcol > endcol || bdp->is_oneChar)
	bdp->textlen = 0;
    अञ्ञथा
	bdp->textlen = endcol - startcol + inclusive;
    bdp->textcol = startcol;
    bdp->textstart = startcol <= plen ? p + startcol : p;
}

/*
 * Handle the add/subtract operator.
 */
    शून्य
op_addsub(
    oparg_T	*oap,
    linenr_T	Prenum1,	    // Amount of add/subtract
    पूर्णाङ्क		g_cmd)		    // was g<c-a>/g<c-x>
{
    pos_T		pos;
    संरचना block_def	bd;
    पूर्णाङ्क			change_cnt = 0;
    linenr_T		amount = Prenum1;

    // do_addsub() might trigger re-evaluation of 'foldexpr' halfway, when the
    // buffer is not completely updated yet. Postpone updating folds until before
    // the call to changed_lines().
#ifdef FEAT_FOLDING
    disable_fold_update++;
#endif

    सचे (!VIsual_active)
    {
	pos = curwin->w_cursor;
	सचे (u_save_cursor() == FAIL)
	{
#ifdef FEAT_FOLDING
	    disable_fold_update--;
#endif
	    पटिनिवत्त;
	}
	change_cnt = do_addsub(oap->op_type, &pos, 0, amount);
#ifdef FEAT_FOLDING
	disable_fold_update--;
#endif
	सचे (change_cnt)
	    changed_पङ्क्तयः(pos.lnum, 0, pos.lnum + 1, 0L);
    }
    अञ्ञथा
    {
	पूर्णाङ्क	one_change;
	पूर्णाङ्क	length;
	pos_T	startpos;

	सचे (u_save((linenr_T)(oap->start.lnum - 1),
					(linenr_T)(oap->end.lnum + 1)) == FAIL)
	{
#ifdef FEAT_FOLDING
	    disable_fold_update--;
#endif
	    पटिनिवत्त;
	}

	pos = oap->start;
	अत्थाय (; pos.lnum <= oap->end.lnum; ++pos.lnum)
	{
	    सचे (oap->block_mode)		    // Visual block mode
	    {
		block_prep(oap, &bd, pos.lnum, FALSE);
		pos.col = bd.textcol;
		length = bd.textlen;
	    }
	    अञ्ञथा सचे (oap->motion_type == MLINE)
	    {
		curwin->w_cursor.col = 0;
		pos.col = 0;
		length = ml_get_len(pos.lnum);
	    }
	    अञ्ञथा // oap->motion_type == MCHAR
	    {
		सचे (pos.lnum == oap->start.lnum && !oap->inclusive)
		    dec(&(oap->end));
		length = ml_get_len(pos.lnum);
		pos.col = 0;
		सचे (pos.lnum == oap->start.lnum)
		{
		    pos.col += oap->start.col;
		    length -= oap->start.col;
		}
		सचे (pos.lnum == oap->end.lnum)
		{
		    length = ml_get_len(oap->end.lnum);
		    सचे (oap->end.col >= length)
			oap->end.col = length - 1;
		    length = oap->end.col - pos.col + 1;
		}
	    }
	    one_change = do_addsub(oap->op_type, &pos, length, amount);
	    सचे (one_change)
	    {
		// Remember the start position of the first change.
		सचे (change_cnt == 0)
		    startpos = curbuf->b_op_start;
		++change_cnt;
	    }

#ifdef FEAT_NETBEANS_INTG
	    सचे (netbeans_active() && one_change)
	    {
		char_u *ptr;

		netbeans_removed(curbuf, pos.lnum, pos.col, (long)length);
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, pos.col,
						&ptr[pos.col], length);
	    }
#endif
	    सचे (g_cmd && one_change)
		amount += Prenum1;
	}

#ifdef FEAT_FOLDING
	disable_fold_update--;
#endif
	सचे (change_cnt)
	    changed_पङ्क्तयः(oap->start.lnum, 0, oap->end.lnum + 1, 0L);

	सचे (!change_cnt && oap->is_VIsual)
	    // No change: need to remove the Visual selection
	    redraw_curbuf_later(UPD_INVERTED);

	// Set '[ mark if something changed. Keep the last end
	// position from do_addsub().
	सचे (change_cnt > 0 && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	    curbuf->b_op_start = startpos;

	सचे (change_cnt > p_report)
	    smsg(NGETTEXT("%d line changed", "%d lines changed",
						      change_cnt), change_cnt);
    }
}

/*
 * Add or subtract 'Prenum1' from a number in a line
 * op_type is OP_NR_ADD or OP_NR_SUB
 *
 * Returns TRUE if some character was changed.
 */
    स्थिर पूर्णाङ्क
do_addsub(
    पूर्णाङ्क		op_type,
    pos_T	*pos,
    पूर्णाङ्क		length,
    linenr_T	Prenum1)
{
    पूर्णाङ्क		col;
    पूर्णाङ्क		pre;		// 'X'/'x': hex; '0': octal; 'B'/'b': bin
    स्थिर पूर्णाङ्क	hexupper = FALSE;	// 0xABC
    uvarnumber_T	n;
    uvarnumber_T	oldn;
    char_u	*ptr;
    पूर्णाङ्क		linelen;
    पूर्णाङ्क		c;
    पूर्णाङ्क		todel;
    पूर्णाङ्क		do_hex;
    पूर्णाङ्क		do_oct;
    पूर्णाङ्क		do_bin;
    पूर्णाङ्क		do_alpha;
    पूर्णाङ्क		do_unsigned;
    पूर्णाङ्क		do_blank;
    पूर्णाङ्क		blank_unsigned = FALSE;	// blank: treat as unsigned?
    पूर्णाङ्क		firstdigit;
    पूर्णाङ्क		subtract;
    पूर्णाङ्क		negative = FALSE;
    पूर्णाङ्क		was_positive = TRUE;
    पूर्णाङ्क		visual = VIsual_active;
    पूर्णाङ्क		did_change = FALSE;
    pos_T	save_cursor = curwin->w_cursor;
    पूर्णाङ्क		maxlen = 0;
    pos_T	startpos;
    pos_T	endpos;
    colnr_T	save_coladd = 0;

    do_hex = (vim_strchr(curbuf->b_p_nf, 'x') != रिक्त);	// "heX"
    do_oct = (vim_strchr(curbuf->b_p_nf, 'o') != रिक्त);	// "Octal"
    do_bin = (vim_strchr(curbuf->b_p_nf, 'b') != रिक्त);	// "Bin"
    do_alpha = (vim_strchr(curbuf->b_p_nf, 'p') != रिक्त);	// "alPha"
    do_unsigned = (vim_strchr(curbuf->b_p_nf, 'u') != रिक्त);	// "Unsigned"
    do_blank = (vim_strchr(curbuf->b_p_nf, 'k') != रिक्त);	// "blanK"

    सचे (virtual_active())
    {
	save_coladd = pos->coladd;
	pos->coladd = 0;
    }

    curwin->w_cursor = *pos;
    ptr = ml_get(pos->lnum);
    linelen = ml_get_len(pos->lnum);
    col = pos->col;

    सचे (col + !!save_coladd >= linelen)
	गच्छ theend;

    /*
     * First check if we are on a hexadecimal number, after the "0x".
     */
    सचे (!VIsual_active)
    {
	सचे (do_bin)
	    याव (col > 0 && vim_isbdigit(ptr[col]))
	    {
		--col;
		सचे (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }

	सचे (do_hex)
	    याव (col > 0 && vim_isxdigit(ptr[col]))
	    {
		--col;
		सचे (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }

	सचे (       do_bin
		&& do_hex
		&& ! ((col > 0
		    && (ptr[col] == 'X'
			|| ptr[col] == 'x')
		    && ptr[col - 1] == '0'
		    && (!has_mbyte ||
			!(*mb_head_off)(ptr, ptr + col - 1))
		    && vim_isxdigit(ptr[col + 1]))))
	{

	    // In case of binary/hexadecimal pattern overlap match, rescan

	    col = pos->col;

	    याव (col > 0 && vim_isdigit(ptr[col]))
	    {
		col--;
		सचे (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }
	}

	सचे ((       do_hex
		&& col > 0
		&& (ptr[col] == 'X'
		    || ptr[col] == 'x')
		&& ptr[col - 1] == '0'
		&& (!has_mbyte ||
		    !(*mb_head_off)(ptr, ptr + col - 1))
		&& vim_isxdigit(ptr[col + 1])) ||
	    (       do_bin
		&& col > 0
		&& (ptr[col] == 'B'
		    || ptr[col] == 'b')
		&& ptr[col - 1] == '0'
		&& (!has_mbyte ||
		    !(*mb_head_off)(ptr, ptr + col - 1))
		&& vim_isbdigit(ptr[col + 1])))
	{
	    // Found hexadecimal or binary number, move to its start.
	    --col;
	    सचे (has_mbyte)
		col -= (*mb_head_off)(ptr, ptr + col);
	}
	अञ्ञथा
	{
	    /*
	     * Search forward and then backward to find the start of number.
	     */
	    col = pos->col;

	    याव (ptr[col] != NUL
		    && !vim_isdigit(ptr[col])
		    && !(do_alpha && ASCII_ISALPHA(ptr[col])))
		col += mb_ptr2len(ptr + col);

	    याव (col > 0
		    && vim_isdigit(ptr[col - 1])
		    && !(do_alpha && ASCII_ISALPHA(ptr[col])))
	    {
		--col;
		सचे (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }
	}
    }

    सचे (visual)
    {
	याव (ptr[col] != NUL && length > 0
		&& !vim_isdigit(ptr[col])
		&& !(do_alpha && ASCII_ISALPHA(ptr[col])))
	{
	    पूर्णाङ्क mb_len = mb_ptr2len(ptr + col);

	    col += mb_len;
	    length -= mb_len;
	}

	सचे (length == 0)
	    गच्छ theend;

	सचे (col > pos->col && ptr[col - 1] == '-'
		&& (!has_mbyte || !(*mb_head_off)(ptr, ptr + col - 1))
		&& !do_unsigned)
	{
	    सचे (do_blank && col >= 2 && !VIM_ISWHITE(ptr[col - 2]))
		blank_unsigned = TRUE;
	    अञ्ञथा
	    {
		negative = TRUE;
		was_positive = FALSE;
	    }
	}
    }

    /*
     * If a number was found, and saving for undo works, replace the number.
     */
    firstdigit = ptr[col];
    सचे (!VIM_ISDIGIT(firstdigit) && !(do_alpha && ASCII_ISALPHA(firstdigit)))
    {
	beep_flush();
	गच्छ theend;
    }

    सचे (do_alpha && ASCII_ISALPHA(firstdigit))
    {
	// decrement or increment alphabetic character
	सचे (op_type == OP_NR_SUB)
	{
	    सचे (CharOrd(firstdigit) < Prenum1)
	    {
		सचे (SAFE_isupper(firstdigit))
		    firstdigit = 'A';
		अञ्ञथा
		    firstdigit = 'a';
	    }
	    अञ्ञथा
		firstdigit -= Prenum1;
	}
	अञ्ञथा
	{
	    सचे (26 - CharOrd(firstdigit) - 1 < Prenum1)
	    {
		सचे (SAFE_isupper(firstdigit))
		    firstdigit = 'Z';
		अञ्ञथा
		    firstdigit = 'z';
	    }
	    अञ्ञथा
		firstdigit += Prenum1;
	}
	curwin->w_cursor.col = col;
	सचे (!did_change)
	    startpos = curwin->w_cursor;
	did_change = TRUE;
	(शून्य)del_char(FALSE);
	ins_char(firstdigit);
	endpos = curwin->w_cursor;
	curwin->w_cursor.col = col;
    }
    अञ्ञथा
    {
	char_u	*buf1;
	पूर्णाङ्क	buf1len;
	char_u	buf2[NUMBUFLEN];
	पूर्णाङ्क	buf2len;
	pos_T	save_pos;
	पूर्णाङ्क	i;

	सचे (col > 0 && ptr[col - 1] == '-'
		&& (!has_mbyte ||
		    !(*mb_head_off)(ptr, ptr + col - 1))
		&& !visual
		&& !do_unsigned)
	{
	    सचे (do_blank && col >= 2 && !VIM_ISWHITE(ptr[col - 2]))
		blank_unsigned = TRUE;
	    अञ्ञथा
	    {
		// negative number
		--col;
		negative = TRUE;
	    }
	}

	// get the number value (unsigned)
	सचे (visual && VIsual_mode != 'V')
	    maxlen = (curbuf->b_visual.vi_curswant == MAXCOL
		    ? linelen - col : length);

	पूर्णाङ्क overflow = FALSE;
	vim_str2nr(ptr + col, &pre, &length,
		0 + (do_bin ? STR2NR_BIN : 0)
		    + (do_oct ? STR2NR_OCT : 0)
		    + (do_hex ? STR2NR_HEX : 0),
		रिक्त, &n, maxlen, FALSE, &overflow);

	// ignore leading '-' for hex and octal and bin numbers
	सचे (pre && negative)
	{
	    ++col;
	    --length;
	    negative = FALSE;
	}
	// add or subtract
	subtract = FALSE;
	सचे (op_type == OP_NR_SUB)
	    subtract ^= TRUE;
	सचे (negative)
	    subtract ^= TRUE;

	oldn = n;
	सचे (!overflow)  // if number is too big don't add/subtract
	{
	    सचे (subtract)
		n -= (uvarnumber_T)Prenum1;
	    अञ्ञथा
		n += (uvarnumber_T)Prenum1;
	}

	// handle wraparound for decimal numbers
	सचे (!pre)
	{
	    सचे (subtract)
	    {
		सचे (n > oldn)
		{
		    n = 1 + (n ^ (uvarnumber_T)-1);
		    negative ^= TRUE;
		}
	    }
	    अञ्ञथा
	    {
		// add
		सचे (n < oldn)
		{
		    n = (n ^ (uvarnumber_T)-1);
		    negative ^= TRUE;
		}
	    }
	    सचे (n == 0)
		negative = FALSE;
	}

	सचे ((do_unsigned || blank_unsigned) && negative)
	{
	    सचे (subtract)
		// sticking at zero.
		n = (uvarnumber_T)0;
	    अञ्ञथा
		// sticking at 2^64 - 1.
		n = (uvarnumber_T)(-1);
	    negative = FALSE;
	}

	सचे (visual && !was_positive && !negative && col > 0)
	{
	    // need to remove the '-'
	    col--;
	    length++;
	}

	/*
	 * Delete the old number.
	 */
	curwin->w_cursor.col = col;
	सचे (!did_change)
	    startpos = curwin->w_cursor;
	did_change = TRUE;
	todel = length;
	c = gchar_cursor();
	/*
	 * Don't include the '-' in the length, only the length of the
	 * part after it is kept the same.
	 */
	सचे (c == '-')
	    --length;

	save_pos = curwin->w_cursor;
	अत्थाय (i = 0; i < todel; ++i)
	{
	    सचे (c < 0x100 && SAFE_isalpha(c))
	    {
		सचे (SAFE_isupper(c))
		    hexupper = TRUE;
		अञ्ञथा
		    hexupper = FALSE;
	    }
	    inc_cursor();
	    c = gchar_cursor();
	}
	curwin->w_cursor = save_pos;

	/*
	 * Prepare the leading characters in buf1[].
	 * When there are many leading zeros it could be very long.
	 * Allocate a bit too much.
	 */
	buf1 = alloc(length + NUMBUFLEN);
	सचे (buf1 == रिक्त)
	    गच्छ theend;
	ptr = buf1;
	सचे (negative && (!visual || was_positive))
	    *ptr++ = '-';
	सचे (pre)
	{
	    *ptr++ = '0';
	    --length;
	}
	सचे (pre == 'b' || pre == 'B' ||
	    pre == 'x' || pre == 'X')
	{
	    *ptr++ = pre;
	    --length;
	}

	/*
	 * Put the number characters in buf2[].
	 */
	सचे (pre == 'b' || pre == 'B')
	{
	    पूर्णाङ्क bit = 0;
	    पूर्णाङ्क bits = परिमाण(uvarnumber_T) * 8;

	    // leading zeros
	    अत्थाय (bit = bits; bit > 0; bit--)
		सचे ((n >> (bit - 1)) & 0x1) भङ्ग;

	    अत्थाय (buf2len = 0; bit > 0 && buf2len < (NUMBUFLEN - 1); bit--)
		buf2[buf2len++] = ((n >> (bit - 1)) & 0x1) ? '1' : '0';

	    buf2[buf2len] = NUL;
	}
	अञ्ञथा सचे (pre == 0)
	    buf2len = vim_snprintf((वर्ण *)buf2, NUMBUFLEN, "%llu", n);
	अञ्ञथा सचे (pre == '0')
	    buf2len = vim_snprintf((वर्ण *)buf2, NUMBUFLEN, "%llo", n);
	अञ्ञथा सचे (pre && hexupper)
	    buf2len = vim_snprintf((वर्ण *)buf2, NUMBUFLEN, "%llX", n);
	अञ्ञथा
	    buf2len = vim_snprintf((वर्ण *)buf2, NUMBUFLEN, "%llx", n);
	length -= buf2len;

	/*
	 * Adjust number of zeros to the new number of digits, so the
	 * total length of the number remains the same.
	 * Don't do this when
	 * the result may look like an octal number.
	 */
	सचे (firstdigit == '0' && !(do_oct && pre == 0))
	    याव (length-- > 0)
		*ptr++ = '0';
	*ptr = NUL;
	buf1len = (पूर्णाङ्क)(ptr - buf1);

	STRCPY(buf1 + buf1len, buf2);
	buf1len += buf2len;

	// Insert just after the first character to be removed, so that any
	// text properties will be adjusted.  Then delete the old number
	// afterwards.
	save_pos = curwin->w_cursor;
	सचे (todel > 0)
	    inc_cursor();
	ins_str(buf1, (size_t)buf1len);		// insert the new number
	vim_free(buf1);

	// del_char() will also mark line needing displaying
	सचे (todel > 0)
	{
	    पूर्णाङ्क bytes_after = ml_get_curline_len() - curwin->w_cursor.col;

	    // Delete the one character before the insert.
	    curwin->w_cursor = save_pos;
	    (शून्य)del_char(FALSE);
	    curwin->w_cursor.col = ml_get_curline_len() - bytes_after;
	    --todel;
	}
	याव (todel-- > 0)
	    (शून्य)del_char(FALSE);

	endpos = curwin->w_cursor;
	सचे (did_change && curwin->w_cursor.col)
	    --curwin->w_cursor.col;
    }

    सचे (did_change && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// set the '[ and '] marks
	curbuf->b_op_start = startpos;
	curbuf->b_op_end = endpos;
	सचे (curbuf->b_op_end.col > 0)
	    --curbuf->b_op_end.col;
    }

theend:
    सचे (visual)
	curwin->w_cursor = save_cursor;
    अञ्ञथा सचे (did_change)
	curwin->w_set_curswant = true;
    अञ्ञथा सचे (virtual_active())
	curwin->w_cursor.coladd = save_coladd;

    पटिनिवत्त did_change;
}

    शून्य
clear_oparg(oparg_T *oap)
{
    CLEAR_POINTER(oap);
}

/*
 *  Count the number of bytes, characters and "words" in a line.
 *
 *  "Words" are counted by looking for boundaries between non-space and
 *  space characters.  (it seems to produce results that match 'wc'.)
 *
 *  Return value is byte count; word count for the line is added to "*wc".
 *  Char count is added to "*cc".
 *
 *  The function will only examine the first "limit" characters in the
 *  line, stopping if it encounters an end-of-line (NUL byte).  In that
 *  case, eol_size will be added to the character count to account for
 *  the size of the EOL character.
 */
    स्थिर varnumber_T
पङ्क्तिः_गणनम्_info(
    char_u	*पङ्क्तिः,
    varnumber_T	*wc,
    varnumber_T	*cc,
    varnumber_T	limit,
    पूर्णाङ्क		eol_size)
{
    varnumber_T	i;
    varnumber_T	शब्दाः = 0;
    varnumber_T	chars = 0;
    पूर्णाङ्क		is_word = 0;

    अत्थाय (i = 0; i < limit && पङ्क्तिः[i] != NUL; )
    {
	सचे (is_word)
	{
	    सचे (vim_isspace(पङ्क्तिः[i]))
	    {
		शब्दाः++;
		is_word = 0;
	    }
	}
	अञ्ञथा सचे (!vim_isspace(पङ्क्तिः[i]))
	    is_word = 1;
	++chars;
	i += (*mb_ptr2len)(पङ्क्तिः + i);
    }

    सचे (is_word)
	शब्दाः++;
    *wc += शब्दाः;

    // Add eol_size if the end of line was reached before hitting limit.
    सचे (i < limit && पङ्क्तिः[i] == NUL)
    {
	i += eol_size;
	chars += eol_size;
    }
    *cc += chars;
    पटिनिवत्त i;
}

/*
 * Give some info about the position of the cursor (for "g CTRL-G").
 * In Visual mode, give some info about the selected region.  (In this case,
 * the *_count_cursor variables store running totals for the selection.)
 * When "dict" is not NULL store the info there instead of showing it.
 */
    शून्य
cursor_pos_info(dict_T *dict)
{
    char_u	*p;
    char_u	buf1[50];
    char_u	buf2[40];
    linenr_T	lnum;
    varnumber_T	byte_गणनम् = 0;
    varnumber_T	bom_गणनम्  = 0;
    varnumber_T	byte_गणनम्_cursor = 0;
    varnumber_T	char_गणनम् = 0;
    varnumber_T	char_गणनम्_cursor = 0;
    varnumber_T	word_गणनम् = 0;
    varnumber_T	word_गणनम्_cursor = 0;
    पूर्णाङ्क		eol_size;
    varnumber_T	last_check = 100000L;
    long	पङ्क्तिः_गणनम्_selected = 0;
    pos_T	min_pos, max_pos;
    oparg_T	oparg;
    संरचना block_def	bd;

    /*
     * Compute the length of the file in characters.
     */
    सचे (curbuf->b_ml.ml_flags & ML_EMPTY)
    {
	सचे (dict == रिक्त)
	{
	    सन्देशः(_(no_पङ्क्तयः_सन्देशः));
	    पटिनिवत्त;
	}
    }
    अञ्ञथा
    {
	सचे (get_fileformat(curbuf) == EOL_DOS)
	    eol_size = 2;
	अञ्ञथा
	    eol_size = 1;

	सचे (VIsual_active)
	{
	    सचे (LT_POS(VIsual, curwin->w_cursor))
	    {
		min_pos = VIsual;
		max_pos = curwin->w_cursor;
	    }
	    अञ्ञथा
	    {
		min_pos = curwin->w_cursor;
		max_pos = VIsual;
	    }
	    सचे (*p_sel == 'e' && max_pos.col > 0)
		--max_pos.col;

	    सचे (VIsual_mode == Ctrl_V)
	    {
#ifdef FEAT_LINEBREAK
		char_u * saved_sbr = p_sbr;
		char_u * saved_w_sbr = curwin->w_p_sbr;

		// Make 'sbr' empty for a moment to get the correct size.
		p_sbr = empty_option;
		curwin->w_p_sbr = empty_option;
#endif
		oparg.is_VIsual = 1;
		oparg.block_mode = TRUE;
		oparg.op_type = OP_NOP;
		getvcols(curwin, &min_pos, &max_pos,
					&oparg.start_vcol, &oparg.end_vcol, 0);
#ifdef FEAT_LINEBREAK
		p_sbr = saved_sbr;
		curwin->w_p_sbr = saved_w_sbr;
#endif
		सचे (curwin->w_curswant == MAXCOL)
		    oparg.end_vcol = MAXCOL;
		// Swap the start, end vcol if needed
		सचे (oparg.end_vcol < oparg.start_vcol)
		{
		    oparg.end_vcol += oparg.start_vcol;
		    oparg.start_vcol = oparg.end_vcol - oparg.start_vcol;
		    oparg.end_vcol -= oparg.start_vcol;
		}
	    }
	    पङ्क्तिः_गणनम्_selected = max_pos.lnum - min_pos.lnum + 1;
	}

	अत्थाय (lnum = 1; lnum <= curbuf->b_ml.ml_पङ्क्तिः_गणनम्; ++lnum)
	{
	    // Check for a CTRL-C every 100000 characters.
	    सचे (byte_गणनम् > last_check)
	    {
		ui_breakcheck();
		सचे (got_int)
		    पटिनिवत्त;
		last_check = byte_गणनम् + 100000L;
	    }

	    // Do extra processing for VIsual mode.
	    सचे (VIsual_active
		    && lnum >= min_pos.lnum && lnum <= max_pos.lnum)
	    {
		char_u	    *s = रिक्त;
		long	    len = 0L;

		विकल्प (VIsual_mode)
		{
		    पक्ष Ctrl_V:
			virtual_op = virtual_active();
			block_prep(&oparg, &bd, lnum, 0);
			virtual_op = MAYBE;
			s = bd.textstart;
			len = (long)bd.textlen;
			भङ्ग;
		    पक्ष 'V':
			s = ml_get(lnum);
			len = MAXCOL;
			भङ्ग;
		    पक्ष 'v':
			{
			    colnr_T start_col = (lnum == min_pos.lnum)
							   ? min_pos.col : 0;
			    colnr_T end_col = (lnum == max_pos.lnum)
				      ? max_pos.col - start_col + 1 : MAXCOL;

			    s = ml_get(lnum) + start_col;
			    len = end_col;
			}
			भङ्ग;
		}
		सचे (s != रिक्त)
		{
		    byte_गणनम्_cursor += पङ्क्तिः_गणनम्_info(s, &word_गणनम्_cursor,
					   &char_गणनम्_cursor, len, eol_size);
		    सचे (lnum == curbuf->b_ml.ml_पङ्क्तिः_गणनम्
			    && !curbuf->b_p_eol
			    && (curbuf->b_p_bin || !curbuf->b_p_fixeol)
			    && (long)STRLEN(s) < len)
			byte_गणनम्_cursor -= eol_size;
		}
	    }
	    अञ्ञथा
	    {
		// In non-visual mode, check for the line the cursor is on
		सचे (lnum == curwin->w_cursor.lnum)
		{
		    word_गणनम्_cursor += word_गणनम्;
		    char_गणनम्_cursor += char_गणनम्;
		    byte_गणनम्_cursor = byte_गणनम् +
			पङ्क्तिः_गणनम्_info(ml_get(lnum),
				&word_गणनम्_cursor, &char_गणनम्_cursor,
				(varnumber_T)(curwin->w_cursor.col + 1),
				eol_size);
		}
	    }
	    // Add to the running totals
	    byte_गणनम् += पङ्क्तिः_गणनम्_info(ml_get(lnum), &word_गणनम्,
					 &char_गणनम्, (varnumber_T)MAXCOL,
					 eol_size);
	}

	// Correction for when last line doesn't have an EOL.
	सचे (!curbuf->b_p_eol && (curbuf->b_p_bin || !curbuf->b_p_fixeol))
	    byte_गणनम् -= eol_size;

	सचे (dict == रिक्त)
	{
	    सचे (VIsual_active)
	    {
		सचे (VIsual_mode == Ctrl_V && curwin->w_curswant < MAXCOL)
		{
		    getvcols(curwin, &min_pos, &max_pos,
						&min_pos.col, &max_pos.col, 0);
		    vim_snprintf((वर्ण *)buf1, परिमाण(buf1), _("%ld Cols; "),
			    (long)(oparg.end_vcol - oparg.start_vcol + 1));
		}
		अञ्ञथा
		    buf1[0] = NUL;

		सचे (char_गणनम्_cursor == byte_गणनम्_cursor
						    && char_गणनम् == byte_गणनम्)
		    vim_snprintf((वर्ण *)IObuff, IOSIZE,
			    _("Selected %s%ld of %ld Lines; %lld of %lld Words; %lld of %lld Bytes"),
			    buf1, पङ्क्तिः_गणनम्_selected,
			    (long)curbuf->b_ml.ml_पङ्क्तिः_गणनम्,
			    word_गणनम्_cursor,
			    word_गणनम्,
			    byte_गणनम्_cursor,
			    byte_गणनम्);
		अञ्ञथा
		    vim_snprintf((वर्ण *)IObuff, IOSIZE,
			    _("Selected %s%ld of %ld Lines; %lld of %lld Words; %lld of %lld Chars; %lld of %lld Bytes"),
			    buf1, पङ्क्तिः_गणनम्_selected,
			    (long)curbuf->b_ml.ml_पङ्क्तिः_गणनम्,
			    word_गणनम्_cursor,
			    word_गणनम्,
			    char_गणनम्_cursor,
			    char_गणनम्,
			    byte_गणनम्_cursor,
			    byte_गणनम्);
	    }
	    अञ्ञथा
	    {
		p = ml_get_curline();
		validate_virtcol();
		col_print(buf1, परिमाण(buf1), (पूर्णाङ्क)curwin->w_cursor.col + 1,
			(पूर्णाङ्क)curwin->w_virtcol + 1);
		col_print(buf2, परिमाण(buf2), ml_get_curline_len(),
							   linetabsize_str(p));

		सचे (char_गणनम्_cursor == byte_गणनम्_cursor
			&& char_गणनम् == byte_गणनम्)
		    vim_snprintf((वर्ण *)IObuff, IOSIZE,
			_("Col %s of %s; Line %ld of %ld; Word %lld of %lld; Byte %lld of %lld"),
			(वर्ण *)buf1, (वर्ण *)buf2,
			(long)curwin->w_cursor.lnum,
			(long)curbuf->b_ml.ml_पङ्क्तिः_गणनम्,
			word_गणनम्_cursor, word_गणनम्,
			byte_गणनम्_cursor, byte_गणनम्);
		अञ्ञथा
		    vim_snprintf((वर्ण *)IObuff, IOSIZE,
			_("Col %s of %s; Line %ld of %ld; Word %lld of %lld; Char %lld of %lld; Byte %lld of %lld"),
			(वर्ण *)buf1, (वर्ण *)buf2,
			(long)curwin->w_cursor.lnum,
			(long)curbuf->b_ml.ml_पङ्क्तिः_गणनम्,
			word_गणनम्_cursor, word_गणनम्,
			char_गणनम्_cursor, char_गणनम्,
			byte_गणनम्_cursor, byte_गणनम्);
	    }
	}

	bom_गणनम् = bomb_size();
	सचे (dict == रिक्त && bom_गणनम् > 0)
	{
	    size_t len = STRLEN(IObuff);

	    vim_snprintf((वर्ण *)IObuff + len, IOSIZE - len,
				 _("(+%lld for BOM)"), bom_गणनम्);
	}
	सचे (dict == रिक्त)
	{
	    // Don't shorten this message, the user asked for it.
	    p = p_shm;
	    p_shm = (char_u *)"";
	    सन्देशः((वर्ण *)IObuff);
	    p_shm = p;
	}
    }
#सचे defined(FEAT_EVAL)
    सचे (dict != रिक्त)
    {
	dict_add_सङ्ख्या(dict, "words", word_गणनम्);
	dict_add_सङ्ख्या(dict, "chars", char_गणनम्);
	dict_add_सङ्ख्या(dict, "bytes", byte_गणनम् + bom_गणनम्);
	dict_add_सङ्ख्या(dict, VIsual_active ? "visual_bytes" : "cursor_bytes",
		byte_गणनम्_cursor);
	dict_add_सङ्ख्या(dict, VIsual_active ? "visual_chars" : "cursor_chars",
		char_गणनम्_cursor);
	dict_add_सङ्ख्या(dict, VIsual_active ? "visual_words" : "cursor_words",
		word_गणनम्_cursor);
    }
#endif
}

/*
 * Handle indent and format operators and visual mode ":".
 */
    स्थिर शून्य
op_colon(oparg_T *oap)
{
    stuffcharReadbuff(':');
    सचे (oap->is_VIsual)
	stuffReadbuff((char_u *)"'<,'>");
    अञ्ञथा
    {
	// Make the range look nice, so it can be repeated.
	सचे (oap->start.lnum == curwin->w_cursor.lnum)
	    stuffcharReadbuff('.');
	अञ्ञथा
	    stuffnumReadbuff((long)oap->start.lnum);

#ifdef FEAT_FOLDING
	// When using !! on a closed fold the range ".!" works best to operate
	// on, it will be made the whole closed fold later.
	linenr_T endOfStartFold = oap->start.lnum;
	(शून्य)hasFolding(oap->start.lnum, रिक्त, &endOfStartFold);
#endif
	सचे (oap->end.lnum != oap->start.lnum
#ifdef FEAT_FOLDING
		&& oap->end.lnum != endOfStartFold
#endif
	   )
	{
	    // Make it a range with the end line.
	    stuffcharReadbuff(',');
	    सचे (oap->end.lnum == curwin->w_cursor.lnum)
		stuffcharReadbuff('.');
	    अञ्ञथा सचे (oap->end.lnum == curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
		stuffcharReadbuff('$');
	    अञ्ञथा सचे (oap->start.lnum == curwin->w_cursor.lnum
#ifdef FEAT_FOLDING
		    // do not use ".+number" for a closed fold, it would count
		    // folded lines twice
		    && !hasFolding(oap->end.lnum, रिक्त, रिक्त)
#endif
		    )
	    {
		stuffReadbuff((char_u *)".+");
		stuffnumReadbuff((long)oap->पङ्क्तिः_गणनम् - 1);
	    }
	    अञ्ञथा
		stuffnumReadbuff((long)oap->end.lnum);
	}
    }
    सचे (oap->op_type != OP_COLON)
	stuffReadbuff((char_u *)"!");
    सचे (oap->op_type == OP_INDENT)
    {
	सचे (*get_equalprg() == NUL)
	    stuffReadbuff((char_u *)"indent");
	अञ्ञथा
	    stuffReadbuff(get_equalprg());
	stuffReadbuff((char_u *)"\n");
    }
    अञ्ञथा सचे (oap->op_type == OP_FORMAT)
    {
	सचे (*curbuf->b_p_fp != NUL)
	    stuffReadbuff(curbuf->b_p_fp);
	अञ्ञथा सचे (*p_fp != NUL)
	    stuffReadbuff(p_fp);
	अञ्ञथा
	    stuffReadbuff((char_u *)"fmt");
	stuffReadbuff((char_u *)"\n']");
    }

    // do_cmdline() does the rest
}

// callback function for 'operatorfunc'
स्थिर callback_T opfunc_cb;

/*
 * Process the 'operatorfunc' option value.
 * Returns OK or FAIL.
 */
    वर्ण *
did_set_operatorfunc(optset_T *args UNUSED)
{
    सचे (option_set_callback_func(p_opfunc, &opfunc_cb) == FAIL)
	पटिनिवत्त e_invalid_argument;

    पटिनिवत्त रिक्त;
}

#सचे defined(EXITFREE)
    शून्य
free_operatorfunc_option(शून्य)
{
# ifdef FEAT_EVAL
    free_callback(&opfunc_cb);
# endif
}
#endif

#सचे defined(FEAT_EVAL)
/*
 * Mark the global 'operatorfunc' callback with "copyID" so that it is not
 * garbage collected.
 */
    पूर्णाङ्क
set_ref_in_opfunc(पूर्णाङ्क copyID UNUSED)
{
    पूर्णाङ्क abort = FALSE;

    abort = set_ref_in_callback(&opfunc_cb, copyID);

    पटिनिवत्त abort;
}
#endif

/*
 * Handle the "g@" operator: call 'operatorfunc'.
 */
    स्थिर शून्य
op_function(oparg_T *oap UNUSED)
{
#ifdef FEAT_EVAL
    typval_T	argv[2];
    pos_T	orig_start = curbuf->b_op_start;
    pos_T	orig_end = curbuf->b_op_end;
    typval_T	rettv;

    सचे (*p_opfunc == NUL)
	emsg(_(e_operatorfunc_is_empty));
    अञ्ञथा
    {
	// Set '[ and '] marks to text to be operated on.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
	सचे (oap->motion_type != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_end);

	argv[0].v_type = VAR_STRING;
	सचे (oap->block_mode)
	    argv[0].vval.v_string = (char_u *)"block";
	अञ्ञथा सचे (oap->motion_type == MLINE)
	    argv[0].vval.v_string = (char_u *)"line";
	अञ्ञथा
	    argv[0].vval.v_string = (char_u *)"char";
	argv[1].v_type = VAR_UNKNOWN;

	// Reset virtual_op so that 'virtualedit' can be changed in the
	// function.
	पूर्णाङ्क save_virtual_op = virtual_op;
	virtual_op = MAYBE;

	// Reset finish_op so that mode() returns the right value.
	पूर्णाङ्क save_finish_op = finish_op;
	finish_op = FALSE;

	सचे (call_callback(&opfunc_cb, 0, &rettv, 1, argv) != FAIL)
	    clear_tv(&rettv);

	virtual_op = save_virtual_op;
	finish_op = save_finish_op;
	सचे (cmdmod.cmod_flags & CMOD_LOCKMARKS)
	{
	    curbuf->b_op_start = orig_start;
	    curbuf->b_op_end = orig_end;
	}
    }
#अञ्ञथा
    emsg(_(e_eval_feature_not_available));
#endif
}

/*
 * Calculate start/end virtual columns for operating in block mode.
 */
    स्थिर शून्य
get_op_vcol(
    oparg_T	*oap,
    colnr_T	redo_VIsual_vcol,
    पूर्णाङ्क		initial)    // when TRUE adjust position for 'selectmode'
{
    colnr_T	    start, end;

    सचे (VIsual_mode != Ctrl_V
	    || (!initial && oap->end.col < curwin->w_width))
	पटिनिवत्त;

    oap->block_mode = TRUE;

    // prevent from moving onto a trail byte
    सचे (has_mbyte)
	mb_adjustpos(curwin->w_buffer, &oap->end);

    getvvcol(curwin, &(oap->start), &oap->start_vcol, रिक्त, &oap->end_vcol, 0);

    सचे (!redo_VIsual_busy)
    {
	getvvcol(curwin, &(oap->end), &start, रिक्त, &end, 0);

	सचे (start < oap->start_vcol)
	    oap->start_vcol = start;
	सचे (end > oap->end_vcol)
	{
	    सचे (initial && *p_sel == 'e' && start >= 1
				    && start - 1 >= oap->end_vcol)
		oap->end_vcol = start - 1;
	    अञ्ञथा
		oap->end_vcol = end;
	}
    }

    // if '$' was used, get oap->end_vcol from longest line
    सचे (curwin->w_curswant == MAXCOL)
    {
	curwin->w_cursor.col = MAXCOL;
	oap->end_vcol = 0;
	अत्थाय (curwin->w_cursor.lnum = oap->start.lnum;
		curwin->w_cursor.lnum <= oap->end.lnum;
					++curwin->w_cursor.lnum)
	{
	    getvvcol(curwin, &curwin->w_cursor, रिक्त, रिक्त, &end, 0);
	    सचे (end > oap->end_vcol)
		oap->end_vcol = end;
	}
    }
    अञ्ञथा सचे (redo_VIsual_busy)
	oap->end_vcol = oap->start_vcol + redo_VIsual_vcol - 1;
    // Correct oap->end.col and oap->start.col to be the
    // upper-left and lower-right corner of the block area.
    //
    // (Actually, this does convert column positions into character
    // positions)
    curwin->w_cursor.lnum = oap->end.lnum;
    coladvance(oap->end_vcol);
    oap->end = curwin->w_cursor;

    curwin->w_cursor = oap->start;
    coladvance(oap->start_vcol);
    oap->start = curwin->w_cursor;
}

// Information for redoing the previous Visual selection.
प्रकारनिर्देश संरचना {
    पूर्णाङ्क		rv_mode;	// 'v', 'V', or Ctrl-V
    linenr_T	rv_पङ्क्तिः_गणनम्;	// number of lines
    colnr_T	rv_vcol;	// number of cols or end column
    long	rv_गणनम्;	// count for Visual operator
    पूर्णाङ्क		rv_arg;		// extra argument
} redo_VIsual_T;

    स्थिर पूर्णाङ्क
is_ex_cmdchar(cmdarg_T *cap)
{
    पटिनिवत्त cap->cmdchar == ':'
	|| cap->cmdchar == K_COMMAND
	|| cap->cmdchar == K_SCRIPT_COMMAND;
}

/*
 * Handle an operator after Visual mode or when the movement is finished.
 * "gui_yank" is true when yanking text for the clipboard.
 */
    शून्य
do_pending_operator(cmdarg_T *cap, पूर्णाङ्क old_col, पूर्णाङ्क gui_yank)
{
    oparg_T	*oap = cap->oap;
    pos_T	old_cursor;
    पूर्णाङ्क		empty_region_error;
    पूर्णाङ्क		restart_edit_save;
#ifdef FEAT_LINEBREAK
    पूर्णाङ्क		lbr_saved = curwin->w_p_lbr;
#endif

    // The visual area is remembered for redo
    स्थिर redo_VIsual_T   redo_VIsual = {NUL, 0, 0, 0,0};

    पूर्णाङ्क		    include_पङ्क्तिः_break = FALSE;

#सचे defined(FEAT_CLIPBOARD)
    // Yank the visual area into the GUI selection register before we operate
    // on it and lose it forever.
    // Don't do it if a specific register was specified, so that ""x"*P works.
    // This could call do_pending_operator() recursively, but that's OK
    // because gui_yank will be TRUE for the nested call.
    सचे ((clip_star.available || clip_plus.available)
	    && oap->op_type != OP_NOP
	    && !gui_yank
	    && VIsual_active
	    && !redo_VIsual_busy
	    && oap->regname == 0)
	clip_auto_select();
#endif
    old_cursor = curwin->w_cursor;

    // If an operation is pending, handle it...
    सचे ((finish_op || VIsual_active) && oap->op_type != OP_NOP)
    {
	// Yank can be redone when 'y' is in 'cpoptions', but not when yanking
	// for the clipboard.
	पूर्णाङ्क	redo_yank = vim_strchr(p_cpo, CPO_YANK) != रिक्त && !gui_yank;

#ifdef FEAT_LINEBREAK
	// Avoid a problem with unwanted linebreaks in block mode.
	(शून्य)reset_lbr();
#endif
	oap->is_VIsual = VIsual_active;
	सचे (oap->motion_force == 'V')
	    oap->motion_type = MLINE;
	अञ्ञथा सचे (oap->motion_force == 'v')
	{
	    // If the motion was linewise, "inclusive" will not have been set.
	    // Use "exclusive" to be consistent.  Makes "dvj" work nice.
	    सचे (oap->motion_type == MLINE)
		oap->inclusive = FALSE;
	    // If the motion already was characterwise, toggle "inclusive"
	    अञ्ञथा सचे (oap->motion_type == MCHAR)
		oap->inclusive = !oap->inclusive;
	    oap->motion_type = MCHAR;
	}
	अञ्ञथा सचे (oap->motion_force == Ctrl_V)
	{
	    // Change line- or characterwise motion into Visual block mode.
	    सचे (!VIsual_active)
	    {
		VIsual_active = TRUE;
		VIsual = oap->start;
	    }
	    VIsual_mode = Ctrl_V;
	    VIsual_select = FALSE;
	    VIsual_reselect = FALSE;
	}

	// Only redo yank when 'y' flag is in 'cpoptions'.
	// Never redo "zf" (define fold).
	सचे ((redo_yank || oap->op_type != OP_YANK)
		&& ((!VIsual_active || oap->motion_force)
		    // Also redo Operator-pending Visual mode mappings
		    || (VIsual_active
			    && is_ex_cmdchar(cap) && oap->op_type != OP_COLON))
		&& cap->cmdchar != 'D'
#ifdef FEAT_FOLDING
		&& oap->op_type != OP_FOLD
		&& oap->op_type != OP_FOLDOPEN
		&& oap->op_type != OP_FOLDOPENREC
		&& oap->op_type != OP_FOLDCLOSE
		&& oap->op_type != OP_FOLDCLOSEREC
		&& oap->op_type != OP_FOLDDEL
		&& oap->op_type != OP_FOLDDELREC
#endif
		)
	{
	    prep_redo(oap->regname, cap->count0,
		    get_op_char(oap->op_type), get_extra_op_char(oap->op_type),
		    oap->motion_force, cap->cmdchar, cap->nchar);
	    सचे (cap->cmdchar == '/' || cap->cmdchar == '?') // was a search
	    {
		// If 'cpoptions' does not contain 'r', insert the search
		// pattern to really repeat the same command.
		सचे (vim_strchr(p_cpo, CPO_REDO) == रिक्त)
		    AppendToRedobuffLit(cap->searchbuf, -1);
		AppendToRedobuff(NL_STR);
	    }
	    अञ्ञथा सचे (is_ex_cmdchar(cap))
	    {
		// do_cmdline() has stored the first typed line in
		// "repeat_cmdline".  When several lines are typed repeating
		// won't be possible.
		सचे (repeat_cmdline == रिक्त)
		    ResetRedobuff();
		अञ्ञथा
		{
		    सचे (cap->cmdchar == ':')
			AppendToRedobuffLit(repeat_cmdline, -1);
		    अञ्ञथा
			AppendToRedobuffSpec(repeat_cmdline);
		    AppendToRedobuff(NL_STR);
		    VIM_CLEAR(repeat_cmdline);
		}
	    }
	}

	सचे (redo_VIsual_busy)
	{
	    // Redo of an operation on a Visual area. Use the same size from
	    // redo_VIsual.rv_line_count and redo_VIsual.rv_vcol.
	    oap->start = curwin->w_cursor;
	    curwin->w_cursor.lnum += redo_VIsual.rv_पङ्क्तिः_गणनम् - 1;
	    सचे (curwin->w_cursor.lnum > curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_पङ्क्तिः_गणनम्;
	    VIsual_mode = redo_VIsual.rv_mode;
	    सचे (redo_VIsual.rv_vcol == MAXCOL || VIsual_mode == 'v')
	    {
		सचे (VIsual_mode == 'v')
		{
		    सचे (redo_VIsual.rv_पङ्क्तिः_गणनम् <= 1)
		    {
			validate_virtcol();
			curwin->w_curswant =
				     curwin->w_virtcol + redo_VIsual.rv_vcol - 1;
		    }
		    अञ्ञथा
			curwin->w_curswant = redo_VIsual.rv_vcol;
		}
		अञ्ञथा
		{
		    curwin->w_curswant = MAXCOL;
		}
		coladvance(curwin->w_curswant);
	    }
	    cap->count0 = redo_VIsual.rv_गणनम्;
	    सचे (redo_VIsual.rv_गणनम् != 0)
		cap->count1 = redo_VIsual.rv_गणनम्;
	    अञ्ञथा
		cap->count1 = 1;
	}
	अञ्ञथा सचे (VIsual_active)
	{
	    सचे (!gui_yank)
	    {
		// Save the current VIsual area for '< and '> marks, and "gv"
		curbuf->b_visual.vi_start = VIsual;
		curbuf->b_visual.vi_end = curwin->w_cursor;
		curbuf->b_visual.vi_mode = VIsual_mode;
		restore_visual_mode();
		curbuf->b_visual.vi_curswant = curwin->w_curswant;
#ifdef FEAT_EVAL
		curbuf->b_visual_mode_eval = VIsual_mode;
#endif
	    }

	    // In Select mode, a linewise selection is operated upon like a
	    // characterwise selection.
	    // Special case: gH<Del> deletes the last line.
	    सचे (VIsual_select && VIsual_mode == 'V'
					    && cap->oap->op_type != OP_DELETE)
	    {
		सचे (LT_POS(VIsual, curwin->w_cursor))
		{
		    VIsual.col = 0;
		    curwin->w_cursor.col = ml_get_len(curwin->w_cursor.lnum);
		}
		अञ्ञथा
		{
		    curwin->w_cursor.col = 0;
		    VIsual.col = ml_get_len(VIsual.lnum);
		}
		VIsual_mode = 'v';
	    }
	    // If 'selection' is "exclusive", backup one character for
	    // charwise selections.
	    अञ्ञथा सचे (VIsual_mode == 'v')
		include_पङ्क्तिः_break = unadjust_for_sel();

	    oap->start = VIsual;
	    सचे (VIsual_mode == 'V')
	    {
		oap->start.col = 0;
		oap->start.coladd = 0;
	    }
	}

	// Set oap->start to the first position of the operated text, oap->end
	// to the end of the operated text.  w_cursor is equal to oap->start.
	सचे (LT_POS(oap->start, curwin->w_cursor))
	{
#ifdef FEAT_FOLDING
	    // Include folded lines completely.
	    सचे (!VIsual_active)
	    {
		सचे (hasFolding(oap->start.lnum, &oap->start.lnum, रिक्त))
		    oap->start.col = 0;
		सचे ((curwin->w_cursor.col > 0 || oap->inclusive
						  || oap->motion_type == MLINE)
			&& hasFolding(curwin->w_cursor.lnum, रिक्त,
						      &curwin->w_cursor.lnum))
		    curwin->w_cursor.col = ml_get_curline_len();
	    }
#endif
	    oap->end = curwin->w_cursor;
	    curwin->w_cursor = oap->start;

	    // w_virtcol may have been updated; if the cursor goes back to its
	    // previous position w_virtcol becomes invalid and isn't updated
	    // automatically.
	    curwin->w_valid &= ~VALID_VIRTCOL;
	}
	अञ्ञथा
	{
#ifdef FEAT_FOLDING
	    // Include folded lines completely.
	    सचे (!VIsual_active && oap->motion_type == MLINE)
	    {
		सचे (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum,
									रिक्त))
		    curwin->w_cursor.col = 0;
		सचे (hasFolding(oap->start.lnum, रिक्त, &oap->start.lnum))
		    oap->start.col = ml_get_len(oap->start.lnum);
	    }
#endif
	    oap->end = oap->start;
	    oap->start = curwin->w_cursor;
	}

	// Just in case lines were deleted that make the position invalid.
	check_pos(curwin->w_buffer, &oap->end);
	oap->पङ्क्तिः_गणनम् = oap->end.lnum - oap->start.lnum + 1;

	// Set "virtual_op" before resetting VIsual_active.
	virtual_op = virtual_active();

	सचे (VIsual_active || redo_VIsual_busy)
	{
	    get_op_vcol(oap, redo_VIsual.rv_vcol, TRUE);

	    सचे (!redo_VIsual_busy && !gui_yank)
	    {
		// Prepare to reselect and redo Visual: this is based on the
		// size of the Visual text
		resel_VIsual_mode = VIsual_mode;
		सचे (curwin->w_curswant == MAXCOL)
		    resel_VIsual_vcol = MAXCOL;
		अञ्ञथा
		{
		    सचे (VIsual_mode != Ctrl_V)
			getvvcol(curwin, &(oap->end),
						रिक्त, रिक्त, &oap->end_vcol, 0);
		    सचे (VIsual_mode == Ctrl_V || oap->पङ्क्तिः_गणनम् <= 1)
		    {
			सचे (VIsual_mode != Ctrl_V)
			    getvvcol(curwin, &(oap->start),
					      &oap->start_vcol, रिक्त, रिक्त, 0);
			resel_VIsual_vcol = oap->end_vcol - oap->start_vcol + 1;
		    }
		    अञ्ञथा
			resel_VIsual_vcol = oap->end_vcol;
		}
		resel_VIsual_पङ्क्तिः_गणनम् = oap->पङ्क्तिः_गणनम्;
	    }

	    // can't redo yank (unless 'y' is in 'cpoptions') and ":"
	    सचे ((redo_yank || oap->op_type != OP_YANK)
		    && oap->op_type != OP_COLON
#ifdef FEAT_FOLDING
		    && oap->op_type != OP_FOLD
		    && oap->op_type != OP_FOLDOPEN
		    && oap->op_type != OP_FOLDOPENREC
		    && oap->op_type != OP_FOLDCLOSE
		    && oap->op_type != OP_FOLDCLOSEREC
		    && oap->op_type != OP_FOLDDEL
		    && oap->op_type != OP_FOLDDELREC
#endif
		    && oap->motion_force == NUL
		    )
	    {
		// Prepare for redoing.  Only use the nchar field for "r",
		// otherwise it might be the second char of the operator.
		सचे (cap->cmdchar == 'g' && (cap->nchar == 'n'
							|| cap->nchar == 'N'))
		    prep_redo(oap->regname, cap->count0,
			    get_op_char(oap->op_type),
			    get_extra_op_char(oap->op_type),
			    oap->motion_force, cap->cmdchar, cap->nchar);
		अञ्ञथा सचे (!is_ex_cmdchar(cap))
		{
		    पूर्णाङ्क opchar = get_op_char(oap->op_type);
		    पूर्णाङ्क extra_opchar = get_extra_op_char(oap->op_type);
		    पूर्णाङ्क nchar = oap->op_type == OP_REPLACE ? cap->nchar : NUL;

		    // reverse what nv_replace() did
		    सचे (nchar == REPLACE_CR_NCHAR)
			nchar = CAR;
		    अञ्ञथा सचे (nchar == REPLACE_NL_NCHAR)
			nchar = NL;

		    सचे (opchar == 'g' && extra_opchar == '@')
			// also repeat the count for 'operatorfunc'
			prep_redo_num2(oap->regname, 0L, NUL, 'v',
				     cap->count0, opchar, extra_opchar, nchar);
		    अञ्ञथा
			prep_redo(oap->regname, 0L, NUL, 'v',
						  opchar, extra_opchar, nchar);
		}
		सचे (!redo_VIsual_busy)
		{
		    redo_VIsual.rv_mode = resel_VIsual_mode;
		    redo_VIsual.rv_vcol = resel_VIsual_vcol;
		    redo_VIsual.rv_पङ्क्तिः_गणनम् = resel_VIsual_पङ्क्तिः_गणनम्;
		    redo_VIsual.rv_गणनम् = cap->count0;
		    redo_VIsual.rv_arg = cap->arg;
		}
	    }

	    // oap->inclusive defaults to TRUE.
	    // If oap->end is on a NUL (empty line) oap->inclusive becomes
	    // FALSE.  This makes "d}P" and "v}dP" work the same.
	    सचे (oap->motion_force == NUL || oap->motion_type == MLINE)
		oap->inclusive = TRUE;
	    सचे (VIsual_mode == 'V')
		oap->motion_type = MLINE;
	    अञ्ञथा
	    {
		oap->motion_type = MCHAR;
		सचे (VIsual_mode != Ctrl_V && *ml_get_pos(&(oap->end)) == NUL
			&& (include_पङ्क्तिः_break || !virtual_op))
		{
		    oap->inclusive = FALSE;
		    // Try to include the newline, unless it's an operator
		    // that works on lines only.
		    सचे (*p_sel != 'o'
			    && !op_on_पङ्क्तयः(oap->op_type)
			    && oap->end.lnum < curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
		    {
			++oap->end.lnum;
			oap->end.col = 0;
			oap->end.coladd = 0;
			++oap->पङ्क्तिः_गणनम्;
		    }
		}
	    }

	    redo_VIsual_busy = FALSE;

	    // Switch Visual off now, so screen updating does
	    // not show inverted text when the screen is redrawn.
	    // With OP_YANK and sometimes with OP_COLON and OP_FILTER there is
	    // no screen redraw, so it is done here to remove the inverted
	    // part.
	    सचे (!gui_yank)
	    {
		VIsual_active = FALSE;
		setmouse();
		mouse_dragging = 0;
		may_clear_cmdline();
		सचे ((oap->op_type == OP_YANK
			    || oap->op_type == OP_COLON
			    || oap->op_type == OP_FUNCTION
			    || oap->op_type == OP_FILTER)
			&& oap->motion_force == NUL)
		{
#ifdef FEAT_LINEBREAK
		    // make sure redrawing is correct
		    restore_lbr(lbr_saved);
#endif
		    redraw_curbuf_later(UPD_INVERTED);
		}
	    }
	}

	// Include the trailing byte of a multi-byte char.
	सचे (has_mbyte && oap->inclusive)
	{
	    पूर्णाङ्क		l;

	    l = (*mb_ptr2len)(ml_get_pos(&oap->end));
	    सचे (l > 1)
		oap->end.col += l - 1;
	}
	curwin->w_set_curswant = true;

	// oap->empty is set when start and end are the same.  The inclusive
	// flag affects this too, unless yanking and the end is on a NUL.
	oap->empty = (oap->motion_type == MCHAR
		    && (!oap->inclusive
			|| (oap->op_type == OP_YANK
			    && gchar_pos(&oap->end) == NUL))
		    && EQUAL_POS(oap->start, oap->end)
		    && !(virtual_op && oap->start.coladd != oap->end.coladd));
	// For delete, change and yank, it's an error to operate on an
	// empty region, when 'E' included in 'cpoptions' (Vi compatible).
	empty_region_error = (oap->empty
				&& vim_strchr(p_cpo, CPO_EMPTYREGION) != रिक्त);

	// Force a redraw when operating on an empty Visual region, when
	// 'modifiable is off or creating a fold.
	सचे (oap->is_VIsual && (oap->empty || !curbuf->b_p_ma
#ifdef FEAT_FOLDING
		    || oap->op_type == OP_FOLD
#endif
		    ))
	{
#ifdef FEAT_LINEBREAK
	    restore_lbr(lbr_saved);
#endif
	    redraw_curbuf_later(UPD_INVERTED);
	}

	// If the end of an operator is in column one while oap->motion_type
	// is MCHAR and oap->inclusive is FALSE, we put op_end after the last
	// character in the previous line. If op_start is on or before the
	// first non-blank in the line, the operator becomes linewise
	// (strange, but that's the way vi does it).
	सचे (	   oap->motion_type == MCHAR
		&& oap->inclusive == FALSE
		&& !(cap->retval & CA_NO_ADJ_OP_END)
		&& oap->end.col == 0
		&& (!oap->is_VIsual || *p_sel == 'o')
		&& !oap->block_mode
		&& oap->पङ्क्तिः_गणनम् > 1)
	{
	    oap->end_adjusted = TRUE;	    // remember that we did this
	    --oap->पङ्क्तिः_गणनम्;
	    --oap->end.lnum;
	    सचे (inindent(0))
		oap->motion_type = MLINE;
	    अञ्ञथा
	    {
		oap->end.col = ml_get_len(oap->end.lnum);
		सचे (oap->end.col)
		{
		    --oap->end.col;
		    oap->inclusive = TRUE;
		}
	    }
	}
	अञ्ञथा
	    oap->end_adjusted = FALSE;

	विकल्प (oap->op_type)
	{
	पक्ष OP_LSHIFT:
	पक्ष OP_RSHIFT:
	    op_shift(oap, TRUE, oap->is_VIsual ? (पूर्णाङ्क)cap->count1 : 1);
	    auto_format(FALSE, TRUE);
	    भङ्ग;

	पक्ष OP_JOIN_NS:
	पक्ष OP_JOIN:
	    सचे (oap->पङ्क्तिः_गणनम् < 2)
		oap->पङ्क्तिः_गणनम् = 2;
	    सचे (curwin->w_cursor.lnum + oap->पङ्क्तिः_गणनम् - 1 >
						   curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
		beep_flush();
	    अञ्ञथा
	    {
		(शून्य)do_join(oap->पङ्क्तिः_गणनम्, oap->op_type == OP_JOIN,
							    TRUE, TRUE, TRUE);
		auto_format(FALSE, TRUE);
	    }
	    भङ्ग;

	पक्ष OP_DELETE:
	    VIsual_reselect = FALSE;	    // don't reselect now
	    सचे (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    अञ्ञथा
	    {
		(शून्य)op_delete(oap);
		// save cursor line for undo if it wasn't saved yet
		सचे (oap->motion_type == MLINE && has_format_option(FO_AUTO)
						      && u_save_cursor() == OK)
		    auto_format(FALSE, TRUE);
	    }
	    भङ्ग;

	पक्ष OP_YANK:
	    सचे (empty_region_error)
	    {
		सचे (!gui_yank)
		{
		    vim_beep(BO_OPER);
		    CancelRedo();
		}
	    }
	    अञ्ञथा
	    {
#ifdef FEAT_LINEBREAK
		restore_lbr(lbr_saved);
#endif
		oap->excl_tr_ws = cap->cmdchar == 'z';
		(शून्य)op_yank(oap, FALSE, !gui_yank);
	    }
	    check_cursor_col();
	    भङ्ग;

	पक्ष OP_CHANGE:
	    VIsual_reselect = FALSE;	    // don't reselect now
	    सचे (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    अञ्ञथा
	    {
		// This is a new edit command, not a restart.  Need to
		// remember it to make 'insertmode' work with mappings for
		// Visual mode.  But do this only once and not when typed and
		// 'insertmode' isn't set.
		सचे (p_im || !कुञ्चिका_Typed)
		    restart_edit_save = restart_edit;
		अञ्ञथा
		    restart_edit_save = 0;
		restart_edit = 0;
#ifdef FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#endif
		// trigger TextChangedI
		curbuf->b_last_changedtick_i = CHANGEDTICK(curbuf);

		सचे (op_change(oap))	// will call edit()
		    cap->retval |= CA_COMMAND_BUSY;
		सचे (restart_edit == 0)
		    restart_edit = restart_edit_save;
	    }
	    भङ्ग;

	पक्ष OP_FILTER:
	    सचे (vim_strchr(p_cpo, CPO_FILTER) != रिक्त)
		AppendToRedobuff((char_u *)"!\r");  // use any last used !cmd
	    अञ्ञथा
		bangredo = TRUE;    // do_bang() will put cmd in redo buffer
	    // FALLTHROUGH

	पक्ष OP_INDENT:
	पक्ष OP_COLON:

	    // If 'equalprg' is empty, do the indenting internally.
	    सचे (oap->op_type == OP_INDENT && *get_equalprg() == NUL)
	    {
		सचे (curbuf->b_p_lisp)
		{
#ifdef FEAT_EVAL
		    सचे (use_indentexpr_for_lisp())
			op_reindent(oap, get_व्यञ्जकम्_indent);
		    अञ्ञथा
#endif
			op_reindent(oap, get_lisp_indent);
		    भङ्ग;
		}
		op_reindent(oap,
#ifdef FEAT_EVAL
			*curbuf->b_p_inde != NUL ? get_व्यञ्जकम्_indent :
#endif
			    get_c_indent);
		भङ्ग;
	    }

	    op_colon(oap);
	    भङ्ग;

	पक्ष OP_TILDE:
	पक्ष OP_UPPER:
	पक्ष OP_LOWER:
	पक्ष OP_ROT13:
	    सचे (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    अञ्ञथा
		op_tilde(oap);
	    check_cursor_col();
	    भङ्ग;

	पक्ष OP_FORMAT:
#सचे defined(FEAT_EVAL)
	    सचे (*curbuf->b_p_fex != NUL)
		op_formatexpr(oap);	// use expression
	    अञ्ञथा
#endif
	    {
		सचे (*p_fp != NUL || *curbuf->b_p_fp != NUL)
		    op_colon(oap);		// use external command
		अञ्ञथा
		    op_format(oap, FALSE);	// use internal function
	    }
	    भङ्ग;
	पक्ष OP_FORMAT2:
	    op_format(oap, TRUE);	// use internal function
	    भङ्ग;

	पक्ष OP_FUNCTION:
	    {
		redo_VIsual_T   save_redo_VIsual = redo_VIsual;

#ifdef FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#endif
		// call 'operatorfunc'
		op_function(oap);

		// Restore the info for redoing Visual mode, the function may
		// invoke another operator and unintentionally change it.
		redo_VIsual = save_redo_VIsual;
		भङ्ग;
	    }

	पक्ष OP_निवेशय:
	पक्ष OP_APPEND:
	    VIsual_reselect = FALSE;	// don't reselect now
	    सचे (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    अञ्ञथा
	    {
		// This is a new edit command, not a restart.  Need to
		// remember it to make 'insertmode' work with mappings for
		// Visual mode.  But do this only once.
		restart_edit_save = restart_edit;
		restart_edit = 0;
#ifdef FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#endif
		// trigger TextChangedI
		curbuf->b_last_changedtick_i = CHANGEDTICK(curbuf);

		op_निवेशय(oap, cap->count1);
#ifdef FEAT_LINEBREAK
		// Reset linebreak, so that formatting works correctly.
		(शून्य)reset_lbr();
#endif

		// TODO: when inserting in several lines, should format all
		// the lines.
		auto_format(FALSE, TRUE);

		सचे (restart_edit == 0)
		    restart_edit = restart_edit_save;
		अञ्ञथा
		    cap->retval |= CA_COMMAND_BUSY;
	    }
	    भङ्ग;

	पक्ष OP_REPLACE:
	    VIsual_reselect = FALSE;	// don't reselect now
	    सचे (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    अञ्ञथा
	    {
#ifdef FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#endif
		op_replace(oap, cap->nchar);
	    }
	    भङ्ग;

#ifdef FEAT_FOLDING
	पक्ष OP_FOLD:
	    VIsual_reselect = FALSE;	// don't reselect now
	    foldCreate(oap->start.lnum, oap->end.lnum);
	    भङ्ग;

	पक्ष OP_FOLDOPEN:
	पक्ष OP_FOLDOPENREC:
	पक्ष OP_FOLDCLOSE:
	पक्ष OP_FOLDCLOSEREC:
	    VIsual_reselect = FALSE;	// don't reselect now
	    opFoldRange(oap->start.lnum, oap->end.lnum,
		    oap->op_type == OP_FOLDOPEN
					    || oap->op_type == OP_FOLDOPENREC,
		    oap->op_type == OP_FOLDOPENREC
					  || oap->op_type == OP_FOLDCLOSEREC,
					  oap->is_VIsual);
	    भङ्ग;

	पक्ष OP_FOLDDEL:
	पक्ष OP_FOLDDELREC:
	    VIsual_reselect = FALSE;	// don't reselect now
	    deleteFold(oap->start.lnum, oap->end.lnum,
			       oap->op_type == OP_FOLDDELREC, oap->is_VIsual);
	    भङ्ग;
#endif
	पक्ष OP_NR_ADD:
	पक्ष OP_NR_SUB:
	    सचे (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    अञ्ञथा
	    {
		VIsual_active = TRUE;
#ifdef FEAT_LINEBREAK
		restore_lbr(lbr_saved);
#endif
		op_addsub(oap, cap->count1, redo_VIsual.rv_arg);
		VIsual_active = FALSE;
	    }
	    check_cursor_col();
	    भङ्ग;
	स्वभाव:
	    clearopbeep(oap);
	}
	virtual_op = MAYBE;
	सचे (!gui_yank)
	{
	    // if 'sol' not set, go back to old column for some commands
	    सचे (!p_sol && oap->motion_type == MLINE && !oap->end_adjusted
		    && (oap->op_type == OP_LSHIFT || oap->op_type == OP_RSHIFT
						|| oap->op_type == OP_DELETE))
	    {
#ifdef FEAT_LINEBREAK
		(शून्य)reset_lbr();
#endif
		coladvance(curwin->w_curswant = old_col);
	    }
	}
	अञ्ञथा
	{
	    curwin->w_cursor = old_cursor;
	}
	oap->block_mode = FALSE;
	clearop(oap);
	motion_force = NUL;
    }
#ifdef FEAT_LINEBREAK
    restore_lbr(lbr_saved);
#endif
}

// put byte 'c' at position 'lp', but
// verify, that the position to place
// is actually safe
    स्थिर शून्य
pbyte(pos_T lp, पूर्णाङ्क c)
{
    char_u *p = ml_get_buf(curbuf, lp.lnum, TRUE);
    पूर्णाङ्क	len = curbuf->b_ml.ml_पङ्क्तिः_len;

    // safety check
    सचे (lp.col >= len)
	lp.col = (len > 1 ? len - 2 : 0);
    *(p + lp.col) = c;
}
