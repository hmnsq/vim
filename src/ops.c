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
#define OPF_पंक्तियाँ  1	// operator always works on lines
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
    {'<', NUL, OPF_पंक्तियाँ | OPF_CHANGE},	// OP_LSHIFT
    {'>', NUL, OPF_पंक्तियाँ | OPF_CHANGE},	// OP_RSHIFT
    {'!', NUL, OPF_पंक्तियाँ | OPF_CHANGE},	// OP_FILTER
    {'g', '~', OPF_CHANGE},		// OP_TILDE
    {'=', NUL, OPF_पंक्तियाँ | OPF_CHANGE},	// OP_INDENT
    {'g', 'q', OPF_पंक्तियाँ | OPF_CHANGE},	// OP_FORMAT
    {':', NUL, OPF_पंक्तियाँ},		// OP_COLON
    {'g', 'U', OPF_CHANGE},		// OP_UPPER
    {'g', 'u', OPF_CHANGE},		// OP_LOWER
    {'J', NUL, OPF_पंक्तियाँ | OPF_CHANGE},	// DO_JOIN
    {'g', 'J', OPF_पंक्तियाँ | OPF_CHANGE},	// DO_JOIN_NS
    {'g', '?', OPF_CHANGE},		// OP_ROT13
    {'r', NUL, OPF_CHANGE},		// OP_REPLACE
    {'I', NUL, OPF_CHANGE},		// OP_INSERT
    {'A', NUL, OPF_CHANGE},		// OP_APPEND
    {'z', 'f', OPF_पंक्तियाँ},		// OP_FOLD
    {'z', 'o', OPF_पंक्तियाँ},		// OP_FOLDOPEN
    {'z', 'O', OPF_पंक्तियाँ},		// OP_FOLDOPENREC
    {'z', 'c', OPF_पंक्तियाँ},		// OP_FOLDCLOSE
    {'z', 'C', OPF_पंक्तियाँ},		// OP_FOLDCLOSEREC
    {'z', 'd', OPF_पंक्तियाँ},		// OP_FOLDDEL
    {'z', 'D', OPF_पंक्तियाँ},		// OP_FOLDDELREC
    {'g', 'w', OPF_पंक्तियाँ | OPF_CHANGE},	// OP_FORMAT2
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

    यदि (char1 == 'r')		// ignore second character
	प्रत्यागम OP_REPLACE;
    यदि (char1 == '~')		// when tilde is an operator
	प्रत्यागम OP_TILDE;
    यदि (char1 == 'g' && char2 == Ctrl_A)	// add
	प्रत्यागम OP_NR_ADD;
    यदि (char1 == 'g' && char2 == Ctrl_X)	// subtract
	प्रत्यागम OP_NR_SUB;
    यदि (char1 == 'z' && char2 == 'y')	// OP_YANK
	प्रत्यागम OP_YANK;
    प्रति (i = 0; ; ++i)
    {
	यदि (opchars[i][0] == char1 && opchars[i][1] == char2)
	    भङ्ग;
	यदि (i == (पूर्णाङ्क)ARRAY_LENGTH(opchars) - 1)
	{
	    internal_error("get_op_type()");
	    भङ्ग;
	}
    }
    प्रत्यागम i;
}

/*
 * Return TRUE if operator "op" always works on whole lines.
 */
    स्थिर पूर्णाङ्क
op_on_पंक्तियाँ(पूर्णाङ्क op)
{
    प्रत्यागम opchars[op][2] & OPF_पंक्तियाँ;
}

#यदि defined(FEAT_JOB_CHANNEL)
/*
 * Return TRUE if operator "op" changes text.
 */
    पूर्णाङ्क
op_is_change(पूर्णाङ्क op)
{
    प्रत्यागम opchars[op][2] & OPF_CHANGE;
}
#endif

/*
 * Get first operator command character.
 * Returns 'g' or 'z' if there is another command character.
 */
    पूर्णाङ्क
get_op_char(पूर्णाङ्क optype)
{
    प्रत्यागम opchars[optype][0];
}

/*
 * Get second operator command character.
 */
    पूर्णाङ्क
get_extra_op_char(पूर्णाङ्क optype)
{
    प्रत्यागम opchars[optype][1];
}

/*
 * op_shift - handle a shift operation
 */
    शून्य
op_shift(oparg_T *oap, पूर्णाङ्क curs_शीर्ष, पूर्णाङ्क amount)
{
    दीर्घ	    i;
    पूर्णाङ्क		    first_char;
    पूर्णाङ्क		    block_col = 0;

    यदि (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	प्रत्यागम;

    यदि (oap->block_mode)
	block_col = curwin->w_cursor.col;

    प्रति (i = oap->पंक्ति_गिनती; --i >= 0; )
    {
	first_char = *ml_get_curline();
	यदि (first_char == NUL)				// empty line
	    curwin->w_cursor.col = 0;
	अन्यथा यदि (oap->block_mode)
	    shift_block(oap, amount);
	अन्यथा
	    // Move the line right if it doesn't start with '#', 'smartindent'
	    // isn't set or 'cindent' isn't set or '#' isn't in 'cino'.
	    यदि (first_char != '#' || !preprocs_बायाँ())
		shift_पंक्ति(oap->op_type == OP_LSHIFT, p_sr, amount, FALSE);
	++curwin->w_cursor.lnum;
    }

    changed_पंक्तियाँ(oap->start.lnum, 0, oap->end.lnum + 1, 0L);
    यदि (oap->block_mode)
    {
	curwin->w_cursor.lnum = oap->start.lnum;
	curwin->w_cursor.col = block_col;
    }
    अन्यथा यदि (curs_शीर्ष)	    // put cursor on first line, for ">>"
    {
	curwin->w_cursor.lnum = oap->start.lnum;
	beginline(BL_SOL | BL_FIX);   // shift_line() may have set cursor.col
    }
    अन्यथा
	--curwin->w_cursor.lnum;	// put cursor on last line, for ":>"

#ifdef FEAT_FOLDING
    // The cursor line is not in a closed fold
    foldOpenCursor();
#endif


    यदि (oap->पंक्ति_गिनती > p_report)
    {
	वर्ण	    *op;
	वर्ण	    *संदेश_पंक्ति_single;
	वर्ण	    *संदेश_पंक्ति_plural;

	यदि (oap->op_type == OP_RSHIFT)
	    op = ">";
	अन्यथा
	    op = "<";
	संदेश_पंक्ति_single = NGETTEXT("%ld line %sed %d time",
					     "%ld line %sed %d times", amount);
	संदेश_पंक्ति_plural = NGETTEXT("%ld lines %sed %d time",
					    "%ld lines %sed %d times", amount);
	vim_snprintf((वर्ण *)IObuff, IOSIZE,
		NGETTEXT(संदेश_पंक्ति_single, संदेश_पंक्ति_plural, oap->पंक्ति_गिनती),
		oap->पंक्ति_गिनती, op, amount);
	संदेश_attr_keep((वर्ण *)IObuff, 0, TRUE);
    }

    यदि ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end.lnum = oap->end.lnum;
	curbuf->b_op_end.col = ml_get_len(oap->end.lnum);
	यदि (curbuf->b_op_end.col > 0)
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

    यदि (index < 1)
	ts = 0;
    अन्यथा यदि (index <= vts_array[0])
	ts = vts_array[index];
    अन्यथा
	ts = vts_array[vts_array[0]];

    प्रत्यागम ts;
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
    प्रति (i = 1; i <= index && i <= vts_array[0]; i++)
	sum += vts_array[i];

    // Add tabstops whose indices exceed the actual array.
    यदि (i <= index)
	sum += vts_array[vts_array[0]] * (index - vts_array[0]);

    प्रत्यागम sum;
}
#endif

    स्थिर vimlong_T
get_new_sw_indent(
    पूर्णाङ्क		बायाँ,		// TRUE if shift is to the left
    पूर्णाङ्क		round,		// TRUE if new indent is to be to a tabstop
    vimlong_T	amount,		// Number of shifts
    vimlong_T	sw_val)
{
    vimlong_T	गिनती = get_indent();
    vimlong_T	i, j;

    यदि (round)			// round off indent
    {
	i = trim_to_int(गिनती) / sw_val;	// number of 'shiftwidth' rounded down
	j = trim_to_int(गिनती) % sw_val;	// extra spaces
	यदि (j && बायाँ)		// first remove extra spaces
	    --amount;
	यदि (बायाँ)
	{
	    i -= amount;
	    यदि (i < 0)
		i = 0;
	}
	अन्यथा
	    i += amount;
	गिनती = i * sw_val;
    }
    अन्यथा			// original vi indent
    {
	यदि (बायाँ)
	{
	    गिनती -= sw_val * amount;
	    यदि (गिनती < 0)
		गिनती = 0;
	}
	अन्यथा
	    गिनती += sw_val * amount;
    }

    प्रत्यागम गिनती;
}

#ifdef FEAT_VARTABS
    स्थिर vimlong_T
get_new_vts_indent(
    पूर्णाङ्क	बायाँ,			// TRUE if shift is to the left
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
    यावत् (vts_indent <= indent)
    {
	vtsi++;
	ts = get_vts(vts_array, vtsi);
	vts_indent += ts;
    }
    vts_indent -= ts;
    vtsi--;

    offset = indent - vts_indent;

    यदि (round)
    {
	यदि (बायाँ)
	{
	    यदि (offset == 0)
		indent = get_vts_sum(vts_array, vtsi - amount);
	    अन्यथा
		indent = get_vts_sum(vts_array, vtsi - (amount - 1));
	}
	अन्यथा
	    indent = get_vts_sum(vts_array, vtsi + amount);
    }
    अन्यथा
    {
	यदि (बायाँ)
	{
	    यदि (amount > vtsi)
		indent = 0;
	    अन्यथा
		indent = get_vts_sum(vts_array, vtsi - amount) + offset;
	}
	अन्यथा
	    indent = get_vts_sum(vts_array, vtsi + amount) + offset;
    }

    प्रत्यागम indent;
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
shift_पंक्ति(
    पूर्णाङ्क	बायाँ,			// TRUE if shift is to the left
    पूर्णाङ्क	round,			// TRUE if new indent is to be to a tabstop
    पूर्णाङ्क	amount,			// Number of shifts
    पूर्णाङ्क	call_changed_bytes)	// call changed_bytes()
{
    vimlong_T	गिनती;
    दीर्घ	sw_val = curbuf->b_p_sw;
    दीर्घ	ts_val = curbuf->b_p_ts;
#ifdef FEAT_VARTABS
    पूर्णाङ्क		*vts_array = curbuf->b_p_vts_array;
#endif

    यदि (sw_val != 0)
	// 'shiftwidth' is not zero; use it as the shift size.
	गिनती = get_new_sw_indent(बायाँ, round, amount, sw_val);
    अन्यथा
#ifdef FEAT_VARTABS
	यदि ((vts_array == रिक्त) || (vts_array[0] == 0))
#endif
    {
	// 'shiftwidth is zero and 'vartabstop' is empty; use 'tabstop' as the
	// shift size.
	गिनती = get_new_sw_indent(बायाँ, round, amount, ts_val);
    }
#ifdef FEAT_VARTABS
    अन्यथा
    {
	// 'shiftwidth is zero and 'vartabstop' is defined; use 'vartabstop'
	// to determine the new indent.
	गिनती = get_new_vts_indent(बायाँ, round, amount, vts_array);
    }
#endif

    // Set new indent
    यदि (State & VREPLACE_FLAG)
	change_indent(INDENT_SET, trim_to_int(गिनती), FALSE, NUL, call_changed_bytes);
    अन्यथा
	(शून्य)set_indent(trim_to_int(गिनती), call_changed_bytes ? SIN_CHANGED : 0);
}

/*
 * Shift one line of the current block one shiftwidth right or left.
 * Leaves cursor on first character in block.
 */
    स्थिर शून्य
shift_block(oparg_T *oap, पूर्णाङ्क amount)
{
    पूर्णाङ्क			बायाँ = (oap->op_type == OP_LSHIFT);
    पूर्णाङ्क			oldstate = State;
    पूर्णाङ्क			total;
    char_u		*newp, *oldp;
    माप_प्रकार		newlen, oldlen;
    पूर्णाङ्क			oldcol = curwin->w_cursor.col;
    पूर्णाङ्क			sw_val = (पूर्णाङ्क)get_sw_मूल्य_indent(curbuf, बायाँ);
    पूर्णाङ्क			ts_val = (पूर्णाङ्क)curbuf->b_p_ts;
    संरचना block_def	bd;
    पूर्णाङ्क			incr;
    colnr_T		ws_vcol;
    पूर्णाङ्क			added;
    माप_प्रकार		new_पंक्ति_len;	// the length of the line after the
					// block shift
#ifdef FEAT_RIGHTLEFT
    पूर्णाङ्क			old_p_ri = p_ri;

    p_ri = 0;			// don't want revins in indent
#endif

    State = MODE_जोड़ो;	// don't want MODE_REPLACE for State
    block_prep(oap, &bd, curwin->w_cursor.lnum, TRUE);
    यदि (bd.is_short)
	प्रत्यागम;

    // total is number of screen columns to be inserted/removed
    total = (पूर्णाङ्क)((अचिन्हित)amount * (अचिन्हित)sw_val);
    यदि ((total / sw_val) != amount)
	प्रत्यागम; // multiplication overflow

    oldp = ml_get_curline();
    oldlen = ml_get_curline_len();

    यदि (!बायाँ)
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
	यदि (bd.startspaces)
	{
	    यदि (has_mbyte)
	    {
		यदि ((*mb_ptr2len)(bd.textstart) == 1)
		    ++bd.textstart;
		अन्यथा
		{
		    ws_vcol = 0;
		    bd.startspaces = 0;
		}
	    }
	    अन्यथा
		++bd.textstart;
	}

	// TODO: is passing bd.textstart for start of the line OK?
	init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum,
				   bd.start_vcol, bd.textstart, bd.textstart);
	प्रति ( ; VIM_ISWHITE(*cts.cts_ptr); )
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
	यदि (!curbuf->b_p_et)
	    tabstop_fromto(ws_vcol, ws_vcol + total,
				ts_val, curbuf->b_p_vts_array, &tabs, &spaces);
	अन्यथा
	    spaces = total;
#अन्यथा
	यदि (!curbuf->b_p_et)
	    tabs = ((ws_vcol % ts_val) + total) / ts_val; // number of tabs
	यदि (tabs > 0)
	    spaces = ((ws_vcol % ts_val) + total) % ts_val; // number of spp
	अन्यथा
	    spaces = total;
#endif
	// if we're splitting a TAB, allow for it
	bd.textcol -= bd.pre_whitesp_c - (bd.startspaces != 0);

	new_पंक्ति_len = bd.textcol + tabs + spaces + (oldlen - (bd.textstart - oldp));
	newp = alloc(new_पंक्ति_len + 1);
	यदि (newp == रिक्त)
	    प्रत्यागम;
	mch_memmove(newp, oldp, (माप_प्रकार)bd.textcol);
	newlen = bd.textcol;
	vim_memset(newp + newlen, TAB, (माप_प्रकार)tabs);
	newlen += tabs;
	vim_memset(newp + newlen, ' ', (माप_प्रकार)spaces);
	STRCPY(newp + newlen + spaces, bd.textstart);
    }
    अन्यथा //बायाँ
    {
	colnr_T	    destination_col;	// column to which text in block will
					// be shifted
	char_u	    *verbatim_copy_end;	// end of the part of the line which is
					// copied verbatim
	colnr_T	    verbatim_copy_width;// the (displayed) width of this part
					// of line
	माप_प्रकार	    fill;		// nr of spaces that replace a TAB
	माप_प्रकार	    block_space_width;
	माप_प्रकार	    shift_amount;
	char_u	    *non_white = bd.textstart;
	colnr_T	    non_white_col;
	माप_प्रकार	    fixedlen;		// length of string left of the shift
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
	यदि (bd.startspaces)
	    MB_PTR_ADV(non_white);

	// The character's column is in "bd.start_vcol".
	non_white_col = bd.start_vcol;

	init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum,
				   non_white_col, bd.textstart, non_white);
	यावत् (VIM_ISWHITE(*cts.cts_ptr))
	{
	    incr = lbr_chartabsize_adv(&cts);
	    cts.cts_vcol += incr;
	}
	non_white_col = cts.cts_vcol;
	non_white = cts.cts_ptr;
	clear_chartabsize_arg(&cts);

	block_space_width = non_white_col - oap->start_vcol;
	// We will shift by "total" or "block_space_width", whichever is less.
	shift_amount = (block_space_width < (माप_प्रकार)total
					 ? block_space_width : (माप_प्रकार)total);

	// The column to which we will shift the text.
	destination_col = (colnr_T)(non_white_col - shift_amount);

	// Now let's find out how much of the beginning of the line we can
	// reuse without modification.
	verbatim_copy_end = bd.textstart;
	verbatim_copy_width = bd.start_vcol;

	// If "bd.startspaces" is set, "bd.textstart" points to the character
	// preceding the block. We have to subtract its width to obtain its
	// column number.
	यदि (bd.startspaces)
	    verbatim_copy_width -= bd.start_char_vcols;
	init_chartabsize_arg(&cts, curwin, 0, verbatim_copy_width,
					     bd.textstart, verbatim_copy_end);
	यावत् (cts.cts_vcol < destination_col)
	{
	    incr = lbr_chartabsize(&cts);
	    यदि (cts.cts_vcol + incr > destination_col)
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
	new_पंक्ति_len = fixedlen + fill + (oldlen - (non_white - oldp));

	newp = alloc(new_पंक्ति_len + 1);
	यदि (newp == रिक्त)
	    प्रत्यागम;
	mch_memmove(newp, oldp, fixedlen);
	newlen = fixedlen;
	vim_memset(newp + newlen, ' ', (माप_प्रकार)fill);
	STRCPY(newp + newlen + fill, non_white);
    }
    // replace the line
    ml_replace(curwin->w_cursor.lnum, newp, FALSE);

    // compute the number of bytes added or subtracted.
    // note new_line_len and oldlen are unsigned so we have
    // to be careful about how we calculate this.
    यदि (new_पंक्ति_len >= oldlen)
	added = (पूर्णाङ्क)(new_पंक्ति_len - oldlen);
    अन्यथा
	added = 0 - (पूर्णाङ्क)(oldlen - new_पंक्ति_len);
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
block_जोड़ो(
    oparg_T		*oap,
    char_u		*s,
    माप_प्रकार		slen,
    पूर्णाङ्क			b_जोड़ो,
    संरचना block_def	*bdp)
{
    पूर्णाङ्क		ts_val;
    पूर्णाङ्क		गिनती = 0;	// extra spaces to replace a cut TAB
    पूर्णाङ्क		spaces = 0;	// non-zero if cutting a TAB
    colnr_T	offset;		// pointer along new line
    colnr_T	startcol;	// column where insert starts
    char_u	*newp, *oldp;	// new, old lines
    linenr_T	lnum;		// loop var
    पूर्णाङ्क		oldstate = State;

    State = MODE_जोड़ो;	// don't want MODE_REPLACE for State

    प्रति (lnum = oap->start.lnum + 1; lnum <= oap->end.lnum; lnum++)
    {
	block_prep(oap, bdp, lnum, TRUE);
	यदि (bdp->is_short && b_जोड़ो)
	    अनुवर्त;	// OP_INSERT, line ends before block start

	oldp = ml_get(lnum);

	यदि (b_जोड़ो)
	{
	    ts_val = bdp->start_char_vcols;
	    spaces = bdp->startspaces;
	    यदि (spaces != 0)
		गिनती = ts_val - 1; // we're cutting a TAB
	    offset = bdp->textcol;
	}
	अन्यथा // append
	{
	    ts_val = bdp->end_char_vcols;
	    यदि (!bdp->is_short) // spaces = padding after block
	    {
		spaces = (bdp->endspaces ? ts_val - bdp->endspaces : 0);
		यदि (spaces != 0)
		    गिनती = ts_val - 1; // we're cutting a TAB
		offset = bdp->textcol + bdp->textlen - (spaces != 0);
	    }
	    अन्यथा // spaces = padding to block edge
	    {
		// if $ used, just append to EOL (ie spaces==0)
		यदि (!bdp->is_MAX)
		    spaces = (oap->end_vcol - bdp->end_vcol) + 1;
		गिनती = spaces;
		offset = bdp->textcol + bdp->textlen;
	    }
	}

	यदि (has_mbyte && spaces > 0)
	    // avoid copying part of a multi-byte character
	    offset -= (*mb_head_off)(oldp, oldp + offset);

	यदि (spaces < 0)  // can happen when the cursor was moved
	    spaces = 0;

	// Make sure the allocated size matches what is actually copied below.
	newp = alloc(ml_get_len(lnum) + spaces + slen
		    + (spaces > 0 && !bdp->is_short ? ts_val - spaces : 0)
								  + गिनती + 1);
	यदि (newp == रिक्त)
	    अनुवर्त;

	// copy up to shifted part
	mch_memmove(newp, oldp, (माप_प्रकार)offset);
	oldp += offset;

	// insert pre-padding
	vim_memset(newp + offset, ' ', (माप_प्रकार)spaces);
	startcol = offset + spaces;

	// copy the new text
	mch_memmove(newp + startcol, s, slen);
	offset += (पूर्णाङ्क)slen;

	यदि (spaces > 0 && !bdp->is_short)
	{
	    यदि (*oldp == TAB)
	    {
		// insert post-padding
		vim_memset(newp + offset + spaces, ' ',
						    (माप_प्रकार)(ts_val - spaces));
		// we're splitting a TAB, don't copy it
		oldp++;
		// We allowed for that TAB, remember this now
		गिनती++;
	    }
	    अन्यथा
		// Not a TAB, no extra spaces
		गिनती = spaces;
	}

	यदि (spaces > 0)
	    offset += गिनती;
	STRCPY(newp + offset, oldp);

	ml_replace(lnum, newp, FALSE);

	यदि (b_जोड़ो)
	    // correct any text properties
	    inserted_bytes(lnum, startcol, (पूर्णाङ्क)slen);

	यदि (lnum == oap->end.lnum)
	{
	    // Set "']" mark to the end of the block instead of the end of
	    // the insert in the first line.
	    curbuf->b_op_end.lnum = oap->end.lnum;
	    curbuf->b_op_end.col = offset;
	    यदि (curbuf->b_visual.vi_end.coladd)
	    {
		curbuf->b_visual.vi_end.col += curbuf->b_visual.vi_end.coladd;
		curbuf->b_visual.vi_end.coladd = 0;
	    }
	}
    } // for all lnum

    changed_पंक्तियाँ(oap->start.lnum + 1, 0, oap->end.lnum + 1, 0L);

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
    linenr_T		old_lcount = curbuf->b_ml.ml_पंक्ति_गिनती;
    पूर्णाङ्क			did_yank = FALSE;

    यदि (curbuf->b_ml.ml_flags & ML_EMPTY)	    // nothing to do
	प्रत्यागम OK;

    // Nothing to delete, return here.	Do prepare undo, for op_change().
    यदि (oap->empty)
	प्रत्यागम u_save_cursor();

    यदि (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	प्रत्यागम FAIL;
    }

    यदि (VIsual_select && oap->is_VIsual)
	// use register given with CTRL_R, defaults to zero
	oap->regname = VIsual_select_reg;

#ifdef HAVE_CLIPMETHOD
    adjust_clip_reg(&oap->regname);
#endif

    यदि (has_mbyte)
	mb_adjust_opend(oap);

    /*
     * Imitate the strange Vi behaviour: If the delete spans more than one
     * line and motion_type == MCHAR and the result is a blank line, make the
     * delete linewise.  Don't do this for the change command or Visual mode.
     */
    यदि (       oap->motion_type == MCHAR
	    && !oap->is_VIsual
	    && !oap->block_mode
	    && oap->पंक्ति_गिनती > 1
	    && oap->motion_force == NUL
	    && (vim_strchr(p_cpo, CPO_WORD) != रिक्त)
	    && oap->op_type == OP_DELETE)
    {
	ptr = ml_get(oap->end.lnum) + oap->end.col;
	यदि (*ptr != NUL)
	    ptr += oap->inclusive;
	ptr = skipwhite(ptr);
	यदि (*ptr == NUL && inindent(0))
	    oap->motion_type = MLINE;
    }

    /*
     * Check for trying to delete (e.g. "D") in an empty line.
     * Note: For the change operator it is ok.
     */
    यदि (       oap->motion_type == MCHAR
	    && oap->पंक्ति_गिनती == 1
	    && oap->op_type == OP_DELETE
	    && *ml_get(oap->start.lnum) == NUL)
    {
	/*
	 * It's an error to operate on an empty region, when 'E' included in
	 * 'cpoptions' (Vi compatible).
	 */
	यदि (virtual_op)
	    // Virtual editing: Nothing gets deleted, but we set the '[ and ']
	    // marks as if it happened.
	    गच्छ setmarks;
	यदि (vim_strchr(p_cpo, CPO_EMPTYREGION) != रिक्त)
	    beep_flush();
	प्रत्यागम OK;
    }

    /*
     * Do a yank of whatever we're about to delete.
     * If a yank register was specified, put the deleted text into that
     * register.  For the black hole register '_' don't yank anything.
     */
    यदि (oap->regname != '_')
    {
#ifdef FEAT_CLIPBOARD_PROVIDER
	inc_clip_provider();
#endif
	यदि (oap->regname != 0)
	{
	    // check for read-only register
	    यदि (!valid_yank_reg(oap->regname, TRUE))
	    {
		beep_flush();
#ifdef FEAT_CLIPBOARD_PROVIDER
		dec_clip_provider();
#endif
		प्रत्यागम OK;
	    }
	    get_yank_register(oap->regname, TRUE); // yank into specif'd reg.
	    यदि (op_yank(oap, TRUE, FALSE) == OK)   // yank without message
		did_yank = TRUE;
	}
	अन्यथा
	    reset_y_append(); // not appending to unnamed register

	/*
	 * Put deleted text into register 1 and shift number registers if the
	 * delete contains a line break, or when using a specific operator (Vi
	 * compatible)
	 */
	यदि (oap->motion_type == MLINE || oap->पंक्ति_गिनती > 1
							   || oap->use_reg_one)
	{
	    shift_delete_registers();
	    यदि (op_yank(oap, TRUE, FALSE) == OK)
		did_yank = TRUE;
	}

	// Yank into small delete register when no named register specified
	// and the delete is within one line.
	यदि ((
#ifdef HAVE_CLIPMETHOD
	    ((clip_unnamed & CLIP_UNNAMED) && oap->regname == '*') ||
	    ((clip_unnamed & CLIP_UNNAMED_PLUS) && oap->regname == '+') ||
#endif
	    oap->regname == 0) && oap->motion_type != MLINE
						      && oap->पंक्ति_गिनती == 1)
	{
	    oap->regname = '-';
	    get_yank_register(oap->regname, TRUE);
	    यदि (op_yank(oap, TRUE, FALSE) == OK)
		did_yank = TRUE;
	    oap->regname = 0;
	}

	/*
	 * If there's too much stuff to fit in the yank register, then get a
	 * confirmation before doing the delete. This is crude, but simple.
	 * And it avoids doing a delete of something we can't put back if we
	 * want.
	 */
	यदि (!did_yank)
	{
	    पूर्णाङ्क संदेश_silent_save = संदेश_silent;

	    संदेश_silent = 0;	// must display the prompt
	    n = ask_yesno((char_u *)_("cannot yank; delete anyway"), TRUE);
	    संदेश_silent = संदेश_silent_save;
	    यदि (n != 'y')
	    {
		emsg(_(e_command_aborted));
#ifdef FEAT_CLIPBOARD_PROVIDER
		dec_clip_provider();
#endif
		प्रत्यागम FAIL;
	    }
	}

#यदि defined(FEAT_EVAL)
	यदि (did_yank && has_textyankpost())
	    yank_do_autocmd(oap, get_y_current());
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
    }

    /*
     * block mode delete
     */
    यदि (oap->block_mode)
    {
	यदि (u_save((linenr_T)(oap->start.lnum - 1),
			       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	    प्रत्यागम FAIL;

	प्रति (lnum = curwin->w_cursor.lnum; lnum <= oap->end.lnum; ++lnum)
	{
	    block_prep(oap, &bd, lnum, TRUE);
	    यदि (bd.textlen == 0)	// nothing to delete
		अनुवर्त;

	    // Adjust cursor position for tab replaced by spaces and 'lbr'.
	    यदि (lnum == curwin->w_cursor.lnum)
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
	    यदि (newp == रिक्त)
		अनुवर्त;
	    // copy up to deleted part
	    mch_memmove(newp, oldp, (माप_प्रकार)bd.textcol);
	    // insert spaces
	    vim_memset(newp + bd.textcol, ' ',
				     (माप_प्रकार)(bd.startspaces + bd.endspaces));
	    // copy the part after the deleted part
	    STRCPY(newp + bd.textcol + bd.startspaces + bd.endspaces,
				    oldp + bd.textcol + bd.textlen);
	    // replace the line
	    ml_replace(lnum, newp, FALSE);

#ifdef FEAT_PROP_POPUP
	    यदि (curbuf->b_has_textprop && n != 0)
		adjust_prop_columns(lnum, bd.textcol, -n, 0);
#endif
	}

	check_cursor_col();
	changed_पंक्तियाँ(curwin->w_cursor.lnum, curwin->w_cursor.col,
						       oap->end.lnum + 1, 0L);
	oap->पंक्ति_गिनती = 0;	    // no lines deleted
    }
    अन्यथा यदि (oap->motion_type == MLINE)
    {
	यदि (oap->op_type == OP_CHANGE)
	{
	    // Delete the lines except the first one.  Temporarily move the
	    // cursor to the next line.  Save the current line number, if the
	    // last line is deleted it may be changed.
	    यदि (oap->पंक्ति_गिनती > 1)
	    {
		lnum = curwin->w_cursor.lnum;
		++curwin->w_cursor.lnum;
		del_पंक्तियाँ((दीर्घ)(oap->पंक्ति_गिनती - 1), TRUE);
		curwin->w_cursor.lnum = lnum;
	    }
	    यदि (u_save_cursor() == FAIL)
		प्रत्यागम FAIL;
	    यदि (curbuf->b_p_ai)		    // don't delete indent
	    {
		beginline(BL_WHITE);	    // cursor on first non-white
		did_ai = TRUE;		    // delete the indent when ESC hit
		ai_col = curwin->w_cursor.col;
	    }
	    अन्यथा
		beginline(0);		    // cursor in column 0
	    truncate_पंक्ति(FALSE);   // delete the rest of the line,
				    // leaving cursor past last char in line
	    यदि (oap->पंक्ति_गिनती > 1)
		u_clearline();	    // "U" command not possible after "2cc"
	}
	अन्यथा
	{
	    del_पंक्तियाँ(oap->पंक्ति_गिनती, TRUE);
	    beginline(BL_WHITE | BL_FIX);
	    u_clearline();	// "U" command not possible after "dd"
	}
    }
    अन्यथा
    {
	यदि (virtual_op)
	{
	    पूर्णाङ्क		endcol = 0;

	    // For virtualedit: break the tabs that are partly included.
	    यदि (gchar_pos(&oap->start) == '\t')
	    {
		यदि (u_save_cursor() == FAIL)	// save first line for undo
		    प्रत्यागम FAIL;
		यदि (oap->पंक्ति_गिनती == 1)
		    endcol = getviscol2(oap->end.col, oap->end.coladd);
		coladvance_force(getviscol2(oap->start.col, oap->start.coladd));
		oap->start = curwin->w_cursor;
		यदि (oap->पंक्ति_गिनती == 1)
		{
		    coladvance(endcol);
		    oap->end.col = curwin->w_cursor.col;
		    oap->end.coladd = curwin->w_cursor.coladd;
		    curwin->w_cursor = oap->start;
		}
	    }

	    // Break a tab only when it's included in the area.
	    यदि (gchar_pos(&oap->end) == '\t'
				     && (पूर्णाङ्क)oap->end.coladd < oap->inclusive)
	    {
		// save last line for undo
		यदि (u_save((linenr_T)(oap->end.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
		    प्रत्यागम FAIL;
		curwin->w_cursor = oap->end;
		coladvance_force(getviscol2(oap->end.col, oap->end.coladd));
		oap->end = curwin->w_cursor;
		curwin->w_cursor = oap->start;
	    }
	    यदि (has_mbyte)
		mb_adjust_opend(oap);
	}

	यदि (oap->पंक्ति_गिनती == 1)	// delete characters within one line
	{
	    यदि (u_save_cursor() == FAIL)	// save line for undo
		प्रत्यागम FAIL;

	    // if 'cpoptions' contains '$', display '$' at end of change
	    यदि (       vim_strchr(p_cpo, CPO_DOLLAR) != रिक्त
		    && oap->op_type == OP_CHANGE
		    && oap->end.lnum == curwin->w_cursor.lnum
		    && !oap->is_VIsual)
		display_dollar(oap->end.col - !oap->inclusive);

	    n = oap->end.col - oap->start.col + 1 - !oap->inclusive;

	    यदि (virtual_op)
	    {
		// fix up things for virtualedit-delete:
		// break the tabs which are going to get in our way
		पूर्णाङ्क		len = ml_get_curline_len();

		यदि (oap->end.coladd != 0
			&& (पूर्णाङ्क)oap->end.col >= len - 1
			&& !(oap->start.coladd && (पूर्णाङ्क)oap->end.col >= len - 1))
		    n++;
		// Delete at least one char (e.g, when on a control char).
		यदि (n == 0 && oap->start.coladd != oap->end.coladd)
		    n = 1;

		// When deleted a char in the line, reset coladd.
		यदि (gchar_cursor() != NUL)
		    curwin->w_cursor.coladd = 0;
	    }
	    (शून्य)del_bytes((दीर्घ)n, !virtual_op,
			    oap->op_type == OP_DELETE && !oap->is_VIsual);
	}
	अन्यथा				// delete characters between lines
	{
	    pos_T   curpos;

	    // save deleted and changed lines for undo
	    यदि (u_save((linenr_T)(curwin->w_cursor.lnum - 1),
		 (linenr_T)(curwin->w_cursor.lnum + oap->पंक्ति_गिनती)) == FAIL)
		प्रत्यागम FAIL;

	    truncate_पंक्ति(TRUE);	// delete from cursor to end of line

	    curpos = curwin->w_cursor;	// remember curwin->w_cursor
	    ++curwin->w_cursor.lnum;
	    del_पंक्तियाँ((दीर्घ)(oap->पंक्ति_गिनती - 2), FALSE);

	    // delete from start of line until op_end
	    n = (oap->end.col + 1 - !oap->inclusive);
	    curwin->w_cursor.col = 0;
	    (शून्य)del_bytes((दीर्घ)n, !virtual_op,
			    oap->op_type == OP_DELETE && !oap->is_VIsual);
	    curwin->w_cursor = curpos;	// restore curwin->w_cursor
	    (शून्य)do_join(2, FALSE, FALSE, FALSE, FALSE);
	}
	यदि (oap->op_type == OP_DELETE)
	    auto_format(FALSE, TRUE);
    }

    msgmore(curbuf->b_ml.ml_पंक्ति_गिनती - old_lcount);

setmarks:
    यदि ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	यदि (oap->block_mode)
	{
	    curbuf->b_op_end.lnum = oap->end.lnum;
	    curbuf->b_op_end.col = oap->start.col;
	}
	अन्यथा
	    curbuf->b_op_end = oap->start;
	curbuf->b_op_start = oap->start;
    }

    प्रत्यागम OK;
}

/*
 * Adjust end of operating area for ending on a multi-byte character.
 * Used for deletion.
 */
    स्थिर शून्य
mb_adjust_opend(oparg_T *oap)
{
    char_u	*पंक्ति;
    char_u	*ptr;

    यदि (!oap->inclusive)
	प्रत्यागम;

    पंक्ति = ml_get(oap->end.lnum);
    ptr = पंक्ति + oap->end.col;
    यदि (*ptr != NUL)
    {
	ptr -= (*mb_head_off)(पंक्ति, ptr);
	ptr += (*mb_ptr2len)(ptr) - 1;
	oap->end.col = ptr - पंक्ति;
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
    पूर्णाङ्क			अंक_chars;
    char_u		*newp, *oldp;
    माप_प्रकार		newlen, oldlen;
    संरचना block_def	bd;
    char_u		*after_p = रिक्त;
    पूर्णाङ्क			had_ctrl_v_cr = FALSE;

    यदि ((curbuf->b_ml.ml_flags & ML_EMPTY ) || oap->empty)
	प्रत्यागम OK;	    // nothing to do

    यदि (c == REPLACE_CR_NCHAR)
    {
	had_ctrl_v_cr = TRUE;
	c = CAR;
    }
    अन्यथा यदि (c == REPLACE_NL_NCHAR)
    {
	had_ctrl_v_cr = TRUE;
	c = NL;
    }

    यदि (has_mbyte)
	mb_adjust_opend(oap);

    यदि (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	प्रत्यागम FAIL;

    /*
     * block mode replace
     */
    यदि (oap->block_mode)
    {
	bd.is_MAX = (curwin->w_curswant == MAXCOL);
	प्रति ( ; curwin->w_cursor.lnum <= oap->end.lnum; ++curwin->w_cursor.lnum)
	{
	    curwin->w_cursor.col = 0;  // make sure cursor position is valid
	    block_prep(oap, &bd, curwin->w_cursor.lnum, TRUE);
	    यदि (bd.textlen == 0 && (!virtual_op || bd.is_MAX))
		अनुवर्त;	    // nothing to replace

	    // n == number of extra chars required
	    // If we split a TAB, it may be replaced by several characters.
	    // Thus the number of characters may increase!
	    // If the range starts in virtual space, count the initial
	    // coladd offset as part of "startspaces"
	    यदि (virtual_op && bd.is_short && *bd.textstart == NUL)
	    {
		pos_T vpos;

		vpos.lnum = curwin->w_cursor.lnum;
		getvpos(&vpos, oap->start_vcol);
		bd.startspaces += vpos.coladd;
		n = bd.startspaces;
	    }
	    अन्यथा
		// allow for pre spaces
		n = (bd.startspaces ? bd.start_char_vcols - 1 : 0);

	    // allow for post spp
	    n += (bd.endspaces
		    && !bd.is_oneChar
		    && bd.end_char_vcols > 0) ? bd.end_char_vcols - 1 : 0;
	    // Figure out how many characters to replace.
	    numc = oap->end_vcol - oap->start_vcol + 1;
	    यदि (bd.is_short && (!virtual_op || bd.is_MAX))
		numc -= (oap->end_vcol - bd.end_vcol) + 1;

	    // A double-wide character can be replaced only up to half the
	    // times.
	    यदि ((*mb_char2cells)(c) > 1)
	    {
		यदि ((numc & 1) && !bd.is_short)
		{
		    ++bd.endspaces;
		    ++n;
		}
		numc = numc / 2;
	    }

	    // Compute bytes needed, move character count to num_chars.
	    अंक_chars = numc;
	    numc *= (*mb_char2len)(c);
	    // oldlen includes textlen, so don't double count
	    n += numc - bd.textlen;

	    oldp = ml_get_curline();
	    oldlen = ml_get_curline_len();
	    newp = alloc(oldlen + 1 + n);
	    यदि (newp == रिक्त)
		अनुवर्त;
	    vim_memset(newp, NUL, (माप_प्रकार)(oldlen + 1 + n));
	    // copy up to deleted part
	    mch_memmove(newp, oldp, (माप_प्रकार)bd.textcol);
	    newlen = bd.textcol;
	    // insert pre-spaces
	    vim_memset(newp + newlen, ' ', (माप_प्रकार)bd.startspaces);
	    newlen += bd.startspaces;
	    // insert replacement chars CHECK FOR ALLOCATED SPACE
	    // REPLACE_CR_NCHAR/REPLACE_NL_NCHAR is used for entering CR
	    // literally.
	    यदि (had_ctrl_v_cr || (c != '\r' && c != '\n'))
	    {
		यदि (has_mbyte)
		{
		    यावत् (--अंक_chars >= 0)
			newlen += (*mb_char2bytes)(c, newp + newlen);
		}
		अन्यथा
		{
		    vim_memset(newp + newlen, c, (माप_प्रकार)numc);
		    newlen += numc;
		}
		यदि (!bd.is_short)
		{
		    // insert post-spaces
		    vim_memset(newp + newlen, ' ', (माप_प्रकार)bd.endspaces);
		    // copy the part after the changed part
		    STRCPY(newp + newlen + bd.endspaces,
				oldp + bd.textcol + bd.textlen);
		}
	    }
	    अन्यथा
	    {
		// Replacing with \r or \n means splitting the line.
		after_p = alloc(oldlen + 1 + n - newlen);
		यदि (after_p != रिक्त)
		    STRCPY(after_p, oldp + bd.textcol + bd.textlen);
	    }

	    // replace the line
	    ml_replace(curwin->w_cursor.lnum, newp, FALSE);
	    यदि (after_p != रिक्त)
	    {
		ml_append(curwin->w_cursor.lnum++, after_p, 0, FALSE);
		appended_पंक्तियाँ_mark(curwin->w_cursor.lnum, 1L);
		oap->end.lnum++;
		vim_free(after_p);
	    }
	}
    }
    अन्यथा
    {
	/*
	 * MCHAR and MLINE motion replace.
	 */
	यदि (oap->motion_type == MLINE)
	{
	    oap->start.col = 0;
	    curwin->w_cursor.col = 0;
	    oap->end.col = ml_get_len(oap->end.lnum);
	    यदि (oap->end.col)
		--oap->end.col;
	}
	अन्यथा यदि (!oap->inclusive)
	    dec(&(oap->end));

	यावत् (LTOREQ_POS(curwin->w_cursor, oap->end))
	{
	    पूर्णाङ्क done = FALSE;

	    n = gchar_cursor();
	    यदि (n != NUL)
	    {
		पूर्णाङ्क new_byte_len = (*mb_char2len)(c);
		पूर्णाङ्क old_byte_len = mb_ptr2len(ml_get_cursor());

		यदि (new_byte_len > 1 || old_byte_len > 1)
		{
		    // This is slow, but it handles replacing a single-byte
		    // with a multi-byte and the other way around.
		    यदि (curwin->w_cursor.lnum == oap->end.lnum)
			oap->end.col += new_byte_len - old_byte_len;
		    replace_character(c);
		    done = TRUE;
		}
		अन्यथा
		{
		    यदि (n == TAB)
		    {
			पूर्णाङ्क end_vcol = 0;

			यदि (curwin->w_cursor.lnum == oap->end.lnum)
			{
			    // oap->end has to be recalculated when
			    // the tab breaks
			    end_vcol = getviscol2(oap->end.col,
							     oap->end.coladd);
			}
			coladvance_force(getviscol());
			यदि (curwin->w_cursor.lnum == oap->end.lnum)
			    getvpos(&oap->end, end_vcol);
		    }
		    // with "coladd" set may move to just after a TAB
		    यदि (gchar_cursor() != NUL)
		    {
			PBYTE(curwin->w_cursor, c);
			done = TRUE;
		    }
		}
	    }
	    यदि (!done && virtual_op && curwin->w_cursor.lnum == oap->end.lnum)
	    {
		पूर्णाङ्क virtcols = oap->end.coladd;

		यदि (curwin->w_cursor.lnum == oap->start.lnum
			&& oap->start.col == oap->end.col && oap->start.coladd)
		    virtcols -= oap->start.coladd;

		// oap->end has been trimmed so it's effectively inclusive;
		// as a result an extra +1 must be counted so we don't
		// trample the NUL byte.
		coladvance_force(getviscol2(oap->end.col, oap->end.coladd) + 1);
		curwin->w_cursor.col -= (virtcols + 1);
		प्रति (; virtcols >= 0; virtcols--)
		{
		    यदि ((*mb_char2len)(c) > 1)
		       replace_character(c);
		    अन्यथा
			PBYTE(curwin->w_cursor, c);
		    यदि (inc(&curwin->w_cursor) == -1)
			भङ्ग;
		}
	    }

	    // Advance to next character, stop at the end of the file.
	    यदि (inc_cursor() == -1)
		भङ्ग;
	}
    }

    curwin->w_cursor = oap->start;
    check_cursor();
    changed_पंक्तियाँ(oap->start.lnum, oap->start.col, oap->end.lnum + 1, 0L);

    यदि ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
    }

    प्रत्यागम OK;
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

    यदि (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	प्रत्यागम;

    pos = oap->start;
    यदि (oap->block_mode)		    // Visual block mode
    {
	प्रति (; pos.lnum <= oap->end.lnum; ++pos.lnum)
	{
	    पूर्णाङ्क one_change;

	    block_prep(oap, &bd, pos.lnum, FALSE);
	    pos.col = bd.textcol;
	    one_change = swapchars(oap->op_type, &pos, bd.textlen);
	    did_change |= one_change;

#ifdef FEAT_NETBEANS_INTG
	    यदि (netbeans_active() && one_change)
	    {
		char_u *ptr;

		netbeans_removed(curbuf, pos.lnum, bd.textcol,
							    (दीर्घ)bd.textlen);
		// get the line now, it may have been flushed
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, bd.textcol,
						&ptr[bd.textcol], bd.textlen);
	    }
#endif
	}
	यदि (did_change)
	    changed_पंक्तियाँ(oap->start.lnum, 0, oap->end.lnum + 1, 0L);
    }
    अन्यथा				    // not block mode
    {
	यदि (oap->motion_type == MLINE)
	{
	    oap->start.col = 0;
	    pos.col = 0;
	    oap->end.col = ml_get_len(oap->end.lnum);
	    यदि (oap->end.col)
		--oap->end.col;
	}
	अन्यथा यदि (!oap->inclusive)
	    dec(&(oap->end));

	यदि (pos.lnum == oap->end.lnum)
	    did_change = swapchars(oap->op_type, &pos,
						  oap->end.col - pos.col + 1);
	अन्यथा
	    प्रति (;;)
	    {
		did_change |= swapchars(oap->op_type, &pos,
			pos.lnum == oap->end.lnum ? oap->end.col + 1
						  : ml_get_pos_len(&pos));
		यदि (LTOREQ_POS(oap->end, pos) || inc(&pos) == -1)
		    भङ्ग;
	    }
	यदि (did_change)
	{
	    changed_पंक्तियाँ(oap->start.lnum, oap->start.col, oap->end.lnum + 1,
									  0L);
#ifdef FEAT_NETBEANS_INTG
	    यदि (netbeans_active())
	    {
		char_u *ptr;
		पूर्णाङ्क गिनती;

		pos = oap->start;
		यावत् (pos.lnum < oap->end.lnum)
		{
		    ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		    गिनती = ml_get_buf_len(curbuf, pos.lnum) - pos.col;
		    netbeans_removed(curbuf, pos.lnum, pos.col, (दीर्घ)गिनती);
		    // get the line again, it may have been flushed
		    ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		    netbeans_inserted(curbuf, pos.lnum, pos.col,
							&ptr[pos.col], गिनती);
		    pos.col = 0;
		    pos.lnum++;
		}
		गिनती = oap->end.col - pos.col + 1;
		netbeans_removed(curbuf, pos.lnum, pos.col, (दीर्घ)गिनती);
		// get the line again, it may have been flushed
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, pos.col,
							&ptr[pos.col], गिनती);
	    }
#endif
	}
    }

    यदि (!did_change && oap->is_VIsual)
	// No change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    यदि ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set '[ and '] marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
    }

    यदि (oap->पंक्ति_गिनती > p_report)
	smsg(NGETTEXT("%ld line changed", "%ld lines changed",
					    oap->पंक्ति_गिनती), oap->पंक्ति_गिनती);
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

    प्रति (todo = length; todo > 0; --todo)
    {
	यदि (has_mbyte)
	{
	    पूर्णाङ्क len = (*mb_ptr2len)(ml_get_pos(pos));

	    // we're counting bytes, not characters
	    यदि (len > 0)
		todo -= len - 1;
	}
	did_change |= swapchar(op_type, pos);
	यदि (inc(pos) == -1)    // at end of file
	    भङ्ग;
    }
    प्रत्यागम did_change;
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
    यदि (c >= 0x80 && op_type == OP_ROT13)
	प्रत्यागम FALSE;

    // ~ is OP_NOP, g~ is OP_TILDE, gU is OP_UPPER
    यदि ((op_type == OP_UPPER || op_type == OP_NOP || op_type == OP_TILDE)
	    && c == 0xdf
	    && (enc_latin1like || STRCMP(p_enc, "iso-8859-2") == 0))
    {
	pos_T   sp = curwin->w_cursor;

	// Special handling for lowercase German sharp s (ß): convert to uppercase (ẞ).
	curwin->w_cursor = *pos;
	del_char(FALSE);
	ins_char(0x1E9E);
	curwin->w_cursor = sp;
	प्रत्यागम TRUE;
    }

    यदि (enc_dbcs != 0 && c >= 0x100)	// No lower/uppercase letter
	प्रत्यागम FALSE;
    nc = c;
    यदि (MB_ISLOWER(c))
    {
	यदि (op_type == OP_ROT13)
	    nc = ROT13(c, 'a');
	अन्यथा यदि (op_type != OP_LOWER)
	    nc = MB_TOUPPER(c);
    }
    अन्यथा यदि (MB_ISUPPER(c))
    {
	यदि (op_type == OP_ROT13)
	    nc = ROT13(c, 'A');
	अन्यथा यदि (op_type != OP_UPPER)
	    nc = MB_TOLOWER(c);
    }
    यदि (nc != c)
    {
	यदि (enc_utf8 && (c >= 0x80 || nc >= 0x80))
	{
	    pos_T   sp = curwin->w_cursor;

	    curwin->w_cursor = *pos;
	    // don't use del_char(), it also removes composing chars
	    del_bytes(utf_ptr2len(ml_get_cursor()), FALSE, FALSE);
	    ins_char(nc);
	    curwin->w_cursor = sp;
	}
	अन्यथा
	    PBYTE(*pos, nc);
	प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}

/*
 * op_insert - Insert and append operators for Visual mode.
 */
    शून्य
op_जोड़ो(oparg_T *oap, दीर्घ count1)
{
    दीर्घ		pre_textlen = 0;
    colnr_T		ind_pre_col = 0, ind_post_col;
    पूर्णाङ्क			ind_pre_vcol = 0, ind_post_vcol = 0;
    संरचना block_def	bd;
    पूर्णाङ्क			i;
    pos_T		t1;
    pos_T		start_जोड़ो;

    // edit() changes this - record it for OP_APPEND
    bd.is_MAX = (curwin->w_curswant == MAXCOL);

    // vis block is still marked. Get rid of it now.
    curwin->w_cursor.lnum = oap->start.lnum;
    update_screen(UPD_INVERTED);

    यदि (oap->block_mode)
    {
	// When 'virtualedit' is used, need to insert the extra spaces before
	// doing block_prep().  When only "block" is used, virtual edit is
	// already disabled, but still need it when calling
	// coladvance_force().
	// coladvance_force() uses get_ve_flags() to get the 'virtualedit'
	// state for the current window.  To override that state, we need to
	// set the window-local value of ve_flags rather than the global value.
	यदि (curwin->w_cursor.coladd > 0)
	{
	    पूर्णाङ्क		old_ve_flags = curwin->w_ve_flags;

	    यदि (u_save_cursor() == FAIL)
		प्रत्यागम;

	    curwin->w_ve_flags = VE_ALL;
	    coladvance_force(oap->op_type == OP_APPEND
					   ? oap->end_vcol + 1 : getviscol());
	    यदि (oap->op_type == OP_APPEND)
		--curwin->w_cursor.col;
	    curwin->w_ve_flags = old_ve_flags;
	}
	// Get the info about the block before entering the text
	block_prep(oap, &bd, oap->start.lnum, TRUE);
	// Get indent information
	ind_pre_col = (colnr_T)getwhitecols_curline();
	ind_pre_vcol = get_indent();
	pre_textlen = ml_get_len(oap->start.lnum) - bd.textcol;
	यदि (oap->op_type == OP_APPEND)
	    pre_textlen -= bd.textlen;
    }

    यदि (oap->op_type == OP_APPEND)
    {
	यदि (oap->block_mode && curwin->w_cursor.coladd == 0)
	{
	    // Move the cursor to the character right of the block.
	    curwin->w_set_curswant = true;
	    यावत् (*ml_get_cursor() != NUL
		    && (curwin->w_cursor.col < bd.textcol + bd.textlen))
		++curwin->w_cursor.col;
	    यदि (bd.is_short && !bd.is_MAX)
	    {
		// First line was too short, make it longer and adjust the
		// values in "bd".
		यदि (u_save_cursor() == FAIL)
		    प्रत्यागम;
		प्रति (i = 0; i < bd.endspaces; ++i)
		    ins_char(' ');
		bd.textlen += bd.endspaces;
	    }
	}
	अन्यथा
	{
	    curwin->w_cursor = oap->end;
	    check_cursor_col();

	    // Works just like an 'i'nsert on the next character.
	    यदि (!LINEEMPTY(curwin->w_cursor.lnum)
		    && oap->start_vcol != oap->end_vcol)
		inc_cursor();
	}
    }

    t1 = oap->start;
    start_जोड़ो = curwin->w_cursor;
    (शून्य)edit(NUL, FALSE, (linenr_T)count1);

    // When a tab was inserted, and the characters in front of the tab
    // have been converted to a tab as well, the column of the cursor
    // might have actually been reduced, so need to adjust here.
    यदि (t1.lnum == curbuf->b_op_start_orig.lnum
	    && LT_POS(curbuf->b_op_start_orig, t1))
	oap->start = curbuf->b_op_start_orig;

    // If user has moved off this line, we don't know what to do, so do
    // nothing.
    // Also don't repeat the insert when Insert mode ended with CTRL-C.
    यदि (curwin->w_cursor.lnum != oap->start.lnum || got_int)
	प्रत्यागम;

    यदि (oap->block_mode)
    {
	पूर्णाङ्क			ins_len;
	char_u			*firstline, *ins_text;
	संरचना block_def	bd2;
	पूर्णाङ्क			did_indent = FALSE;
	माप_प्रकार			len;
	माप_प्रकार			add;
	// offset when cursor was moved in insert mode
	पूर्णाङ्क			offset = 0;

	// If indent kicked in, the firstline might have changed
	// but only do that, if the indent actually increased.
	ind_post_col = (colnr_T)getwhitecols_curline();
	यदि (curbuf->b_op_start.col > ind_pre_col && ind_post_col > ind_pre_col)
	{
	    bd.textcol += ind_post_col - ind_pre_col;
	    ind_post_vcol = get_indent();
	    bd.start_vcol += ind_post_vcol - ind_pre_vcol;
	    did_indent = TRUE;
	}

	// The user may have moved the cursor before inserting something, try
	// to adjust the block for that.  But only do it, if the difference
	// does not come from indent kicking in.
	यदि (oap->start.lnum == curbuf->b_op_start_orig.lnum
						  && !bd.is_MAX && !did_indent)
	{
	    पूर्णाङ्क t = getviscol2(curbuf->b_op_start_orig.col,
					       curbuf->b_op_start_orig.coladd);

	    यदि (oap->op_type == OP_जोड़ो
		    && oap->start.col + oap->start.coladd
			    != curbuf->b_op_start_orig.col
					      + curbuf->b_op_start_orig.coladd)
	    {
		oap->start.col = curbuf->b_op_start_orig.col;
		pre_textlen -= t - oap->start_vcol;
		oap->start_vcol = t;
	    }
	    अन्यथा यदि (oap->op_type == OP_APPEND
		    && oap->start.col + oap->start.coladd
			    >= curbuf->b_op_start_orig.col
					      + curbuf->b_op_start_orig.coladd)
	    {
		oap->start.col = curbuf->b_op_start_orig.col;
		// reset pre_textlen to the value of OP_INSERT
		pre_textlen += bd.textlen;
		pre_textlen -= t - oap->start_vcol;
		oap->start_vcol = t;
		oap->op_type = OP_जोड़ो;
	    }
	}

	// Spaces and tabs in the indent may have changed to other spaces and
	// tabs.  Get the starting column again and correct the length.
	// Don't do this when "$" used, end-of-line will have changed.
	//
	// if indent was added and the inserted text was after the indent,
	// correct the selection for the new indent.
	यदि (did_indent && bd.textcol - ind_post_col > 0)
	{
	    oap->start.col += ind_post_col - ind_pre_col;
	    oap->start_vcol += ind_post_vcol - ind_pre_vcol;
	    oap->end.col += ind_post_col - ind_pre_col;
	    oap->end_vcol += ind_post_vcol - ind_pre_vcol;
	}
	block_prep(oap, &bd2, oap->start.lnum, TRUE);
	यदि (did_indent && bd.textcol - ind_post_col > 0)
	{
	    // undo for where "oap" is used below
	    oap->start.col -= ind_post_col - ind_pre_col;
	    oap->start_vcol -= ind_post_vcol - ind_pre_vcol;
	    oap->end.col -= ind_post_col - ind_pre_col;
	    oap->end_vcol -= ind_post_vcol - ind_pre_vcol;
	}
	यदि (!bd.is_MAX || bd2.textlen < bd.textlen)
	{
	    यदि (oap->op_type == OP_APPEND)
	    {
		pre_textlen += bd2.textlen - bd.textlen;
		यदि (bd2.endspaces)
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
	यदि (oap->op_type == OP_APPEND)
	{
	    add += bd.textlen;
	    // account for pressing cursor in insert mode when '$' was used
	    यदि (bd.is_MAX
		&& (start_जोड़ो.lnum == Insstart.lnum
					   && start_जोड़ो.col > Insstart.col))
	    {
		offset = (start_जोड़ो.col - Insstart.col);
		add -= offset;
		यदि (oap->end_vcol > offset)
		    oap->end_vcol -= (offset + 1);
		अन्यथा
		    // moved outside of the visual block, what to do?
		    प्रत्यागम;
	    }
	}
	यदि (add > len)
	    add = len;  // short line, point to the NUL
	firstline += add;
	len -= add;
	यदि (pre_textlen >= 0 && (ins_len = (पूर्णाङ्क)len - pre_textlen - offset) > 0)
	{
	    ins_text = vim_strnsave(firstline, ins_len);
	    यदि (ins_text != रिक्त)
	    {
		// block handled here
		यदि (u_save(oap->start.lnum,
					 (linenr_T)(oap->end.lnum + 1)) == OK)
		    block_जोड़ो(oap, ins_text, ins_len, (oap->op_type == OP_जोड़ो),
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
    दीर्घ		pre_textlen = 0;
    दीर्घ		pre_indent = 0;
    char_u		*firstline;
    char_u		*ins_text, *newp, *oldp;
    संरचना block_def	bd;

    l = oap->start.col;
    यदि (oap->motion_type == MLINE)
    {
	l = 0;
	can_si = may_do_si();	// Like opening a new line, do smart indent
    }

    // First delete the text in the region.  In an empty buffer only need to
    // save for undo
    यदि (curbuf->b_ml.ml_flags & ML_EMPTY)
    {
	यदि (u_save_cursor() == FAIL)
	    प्रत्यागम FALSE;
    }
    अन्यथा यदि (op_delete(oap) == FAIL)
	प्रत्यागम FALSE;

    यदि ((l > curwin->w_cursor.col) && !LINEEMPTY(curwin->w_cursor.lnum)
							 && !virtual_op)
	inc_cursor();

    // check for still on same line (<CR> in inserted text meaningless)
    // skip blank lines too
    यदि (oap->block_mode)
    {
	// Add spaces before getting the current line length.
	यदि (virtual_op && (curwin->w_cursor.coladd > 0
						    || gchar_cursor() == NUL))
	    coladvance_force(getviscol());
	firstline = ml_get(oap->start.lnum);
	pre_textlen = ml_get_len(oap->start.lnum);
	pre_indent = (दीर्घ)getwhitecols(firstline);
	bd.textcol = curwin->w_cursor.col;
    }

    यदि (oap->motion_type == MLINE)
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
    यदि (oap->block_mode && oap->start.lnum != oap->end.lnum && !got_int)
    {
	पूर्णाङ्क	ins_len;

	// Auto-indenting may have changed the indent.  If the cursor was past
	// the indent, exclude that indent change from the inserted text.
	firstline = ml_get(oap->start.lnum);
	यदि (bd.textcol > (colnr_T)pre_indent)
	{
	    दीर्घ new_indent = (दीर्घ)getwhitecols(firstline);

	    pre_textlen += new_indent - pre_indent;
	    bd.textcol += new_indent - pre_indent;
	}

	ins_len = (पूर्णाङ्क)ml_get_len(oap->start.lnum) - pre_textlen;
	यदि (ins_len > 0)
	{
	    // Subsequent calls to ml_get() flush the firstline data - take a
	    // copy of the inserted text.
	    यदि ((ins_text = alloc(ins_len + 1)) != रिक्त)
	    {
		vim_strncpy(ins_text, firstline + bd.textcol, ins_len);
		प्रति (linenr = oap->start.lnum + 1; linenr <= oap->end.lnum;
								     linenr++)
		{
		    block_prep(oap, &bd, linenr, TRUE);
		    यदि (!bd.is_short || virtual_op)
		    {
			pos_T vpos;
			माप_प्रकार newlen;

			// If the block starts in virtual space, count the
			// initial coladd offset as part of "startspaces"
			यदि (bd.is_short)
			{
			    vpos.lnum = linenr;
			    (शून्य)getvpos(&vpos, oap->start_vcol);
			}
			अन्यथा
			    vpos.coladd = 0;
			oldp = ml_get(linenr);
			newp = alloc(ml_get_len(linenr) + vpos.coladd + ins_len + 1);
			यदि (newp == रिक्त)
			    अनुवर्त;
			// copy up to block start
			mch_memmove(newp, oldp, (माप_प्रकार)bd.textcol);
			newlen = bd.textcol;
			vim_memset(newp + newlen, ' ', (माप_प्रकार)vpos.coladd);
			newlen += vpos.coladd;
			mch_memmove(newp + newlen, ins_text, ins_len);
			STRCPY(newp + newlen + ins_len, oldp + bd.textcol);
			ml_replace(linenr, newp, FALSE);
#ifdef FEAT_PROP_POPUP
			// Shift the properties for linenr as edit() would do.
			यदि (curbuf->b_has_textprop)
			    adjust_prop_columns(linenr, bd.textcol,
						     vpos.coladd + (पूर्णाङ्क)ins_len, 0);
#endif
		    }
		}
		check_cursor();

		changed_पंक्तियाँ(oap->start.lnum + 1, 0, oap->end.lnum + 1, 0L);
	    }
	    vim_free(ins_text);
	}
    }
    auto_format(FALSE, TRUE);

    प्रत्यागम retval;
}

/*
 * When the cursor is on the NUL past the end of the line and it should not be
 * there move it left.
 */
    शून्य
adjust_cursor_eol(शून्य)
{
    अचिन्हित पूर्णाङ्क cur_ve_flags = get_ve_flags();

    पूर्णाङ्क adj_cursor = (curwin->w_cursor.col > 0
				&& gchar_cursor() == NUL
				&& (cur_ve_flags & VE_ONEMORE) == 0
				&& (cur_ve_flags & VE_ALL) == 0
				&& !(restart_edit || (State & MODE_जोड़ो)));
    यदि (!adj_cursor)
	प्रत्यागम;

    // Put the cursor on the last character in the line.
    dec_cursor();

    यदि (cur_ve_flags == VE_ALL)
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
    char_u   *पंक्ति,
    पूर्णाङ्क      process,
    पूर्णाङ्क	     include_space,
    पूर्णाङ्क      *is_comment)
{
    char_u *comment_flags = रिक्त;
    पूर्णाङ्क    lead_len;
    पूर्णाङ्क    leader_offset = get_last_leader_offset(पंक्ति, &comment_flags);

    *is_comment = FALSE;
    यदि (leader_offset != -1)
    {
	// Let's check whether the line ends with an unclosed comment.
	// If the last comment leader has COM_END in flags, there's no comment.
	यावत् (*comment_flags)
	{
	    यदि (*comment_flags == COM_END
		    || *comment_flags == ':')
		भङ्ग;
	    ++comment_flags;
	}
	यदि (*comment_flags != COM_END)
	    *is_comment = TRUE;
    }

    यदि (process == FALSE)
	प्रत्यागम पंक्ति;

    lead_len = get_leader_len(पंक्ति, &comment_flags, FALSE, include_space);

    यदि (lead_len == 0)
	प्रत्यागम पंक्ति;

    // Find:
    // - COM_END,
    // - colon,
    // whichever comes first.
    यावत् (*comment_flags)
    {
	यदि (*comment_flags == COM_END
		|| *comment_flags == ':')
	    भङ्ग;
	++comment_flags;
    }

    // If we found a colon, it means that we are not processing a line
    // starting with a closing part of a three-part comment. That's good,
    // because we don't want to remove those as this would be annoying.
    यदि (*comment_flags == ':' || *comment_flags == NUL)
	पंक्ति += lead_len;

    प्रत्यागम पंक्ति;
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
    दीर्घ    गिनती,
    पूर्णाङ्क	    जोड़ो_space,
    पूर्णाङ्क	    save_undo,
    पूर्णाङ्क	    use_formatoptions UNUSED,
    पूर्णाङ्क	    setmark)
{
    char_u	*curr = रिक्त;
    char_u      *curr_start = रिक्त;
    char_u	*cend;
    char_u	*newp;
    माप_प्रकार	newp_len;
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

    यदि (save_undo && u_save((linenr_T)(curwin->w_cursor.lnum - 1),
			    (linenr_T)(curwin->w_cursor.lnum + गिनती)) == FAIL)
	प्रत्यागम FAIL;

    // Allocate an array to store the number of spaces inserted before each
    // line.  We will use it to pre-compute the length of the new line and the
    // proper placement of each original line in the new one.
    spaces = lalloc_clear(गिनती, TRUE);
    यदि (spaces == रिक्त)
	प्रत्यागम FAIL;
    यदि (remove_comments)
    {
	comments = lalloc_clear(गिनती * परिमाण(पूर्णाङ्क), TRUE);
	यदि (comments == रिक्त)
	{
	    vim_free(spaces);
	    प्रत्यागम FAIL;
	}
    }

    /*
     * Don't move anything yet, just compute the final line length
     * and setup the array of space strings lengths
     * This loops forward over the joined lines.
     */
    प्रति (t = 0; t < गिनती; ++t)
    {
	curr = curr_start = ml_get((linenr_T)(curwin->w_cursor.lnum + t));
	यदि (t == 0 && setmark && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set the '[ mark.
	    curwin->w_buffer->b_op_start.lnum = curwin->w_cursor.lnum;
	    curwin->w_buffer->b_op_start.col  = (colnr_T)STRLEN(curr);
	}
	यदि (remove_comments)
	{
	    // We don't want to remove the comment leader if the
	    // previous line is not a comment.
	    यदि (t > 0 && prev_was_comment)
	    {

		char_u *new_curr = skip_comment(curr, TRUE, जोड़ो_space,
							   &prev_was_comment);
		comments[t] = (पूर्णाङ्क)(new_curr - curr);
		curr = new_curr;
	    }
	    अन्यथा
		curr = skip_comment(curr, FALSE, जोड़ो_space,
							   &prev_was_comment);
	}

	यदि (जोड़ो_space && t > 0)
	{
	    curr = skipwhite(curr);
	    यदि (*curr != NUL && *curr != ')'
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
		यदि (endcurr1 == ' ')
		    endcurr1 = endcurr2;
		अन्यथा
		    ++spaces[t];
		// extra space when 'joinspaces' set and line ends in '.'
		यदि (       p_js
			&& (endcurr1 == '.'
			    || (vim_strchr(p_cpo, CPO_JOINSP) == रिक्त
				&& (endcurr1 == '?' || endcurr1 == '!'))))
		    ++spaces[t];
	    }
	}
	currsize = (पूर्णाङ्क)STRLEN(curr);
	sumsize += currsize + spaces[t];
	endcurr1 = endcurr2 = NUL;
	यदि (जोड़ो_space && currsize > 0)
	{
	    यदि (has_mbyte)
	    {
		cend = curr + currsize;
		MB_PTR_BACK(curr, cend);
		endcurr1 = (*mb_ptr2char)(cend);
		यदि (cend > curr)
		{
		    MB_PTR_BACK(curr, cend);
		    endcurr2 = (*mb_ptr2char)(cend);
		}
	    }
	    अन्यथा
	    {
		endcurr1 = *(curr + currsize - 1);
		यदि (currsize > 1)
		    endcurr2 = *(curr + currsize - 2);
	    }
	}
	पंक्ति_breakcheck();
	यदि (got_int)
	{
	    ret = FAIL;
	    गच्छ theend;
	}
    }

    // store the column position before last line
    col = sumsize - currsize - spaces[गिनती - 1];

    // allocate the space for the new line
    newp_len = sumsize + 1;
    newp = alloc(newp_len);
    यदि (newp == रिक्त)
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

    curr = curr_start = ml_get((linenr_T)(curwin->w_cursor.lnum + गिनती - 1));
    curr += curr_off;
#endif

    प्रति (t = गिनती - 1; ; --t)
    {
	पूर्णाङ्क spaces_removed;

	cend -= currsize;
	mch_memmove(cend, curr, (माप_प्रकार)currsize);

	यदि (spaces[t] > 0)
	{
	    cend -= spaces[t];
	    vim_memset(cend, ' ', (माप_प्रकार)(spaces[t]));
	}

	// If deleting more spaces than adding, the cursor moves no more than
	// what is added if it is inside these spaces.
	spaces_removed = (curr - curr_start) - spaces[t];

	mark_col_adjust(curwin->w_cursor.lnum + t, (colnr_T)0, -t,
			 (दीर्घ)(cend - newp - spaces_removed), spaces_removed);
#ifdef FEAT_PROP_POPUP
	prepend_joined_props(&um, curwin->w_cursor.lnum + t,
		t == गिनती - 1, (दीर्घ)(cend - newp), spaces_removed);
#endif
	यदि (t == 0)
	    भङ्ग;
	curr = curr_start = ml_get((linenr_T)(curwin->w_cursor.lnum + t - 1));
	यदि (remove_comments)
	    curr += comments[t - 1];
	यदि (जोड़ो_space && t > 1)
	    curr = skipwhite(curr);
	currsize = (पूर्णाङ्क)STRLEN(curr);
    }

#ifdef FEAT_PROP_POPUP
    यदि (um.buf != रिक्त)
    {
	um_set_text(&um, newp);
	um_reverse_props(&um);
	um_close(&um);
	newp = रिक्त;  // um_set_text took ownership
    }
    अन्यथा
#endif
    ml_replace_len(curwin->w_cursor.lnum, newp, (colnr_T)newp_len, TRUE, FALSE);

    यदि (setmark && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set the '] mark.
	curwin->w_buffer->b_op_end.lnum = curwin->w_cursor.lnum;
	curwin->w_buffer->b_op_end.col  = (colnr_T)sumsize;
    }

    // Only report the change in the first line here, del_lines() will report
    // the deleted line.
    changed_पंक्तियाँ(curwin->w_cursor.lnum, currsize,
					       curwin->w_cursor.lnum + 1, 0L);
    /*
     * Delete following lines. To do this we move the cursor there
     * briefly, and then move it back. After del_lines() the cursor may
     * have moved up (last line deleted), so the current lnum is kept in t.
     */
    t = curwin->w_cursor.lnum;
    ++curwin->w_cursor.lnum;
    del_पंक्तियाँ(गिनती - 1, FALSE);
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
    यदि (remove_comments)
	vim_free(comments);
    प्रत्यागम ret;
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
    यदि (!curwin->w_p_lbr)
	प्रत्यागम FALSE;
    // changing 'linebreak' may require w_virtcol to be updated
    curwin->w_p_lbr = FALSE;
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);
    प्रत्यागम TRUE;
}

/*
 * Restore 'linebreak' and take care of side effects.
 */
    शून्य
restore_lbr(पूर्णाङ्क lbr_saved)
{
    यदि (curwin->w_p_lbr || !lbr_saved)
	प्रत्यागम;

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
    char_u	*पंक्ति;
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

    पंक्ति = ml_get(lnum);
    prev_pstart = पंक्ति;
    init_chartabsize_arg(&cts, curwin, lnum, bdp->start_vcol, पंक्ति, पंक्ति);
    यावत् (cts.cts_vcol < oap->start_vcol && *cts.cts_ptr != NUL)
    {
	// Count a tab for what it's worth (if list mode not on)
	incr = lbr_chartabsize(&cts);
	cts.cts_vcol += incr;
	यदि (VIM_ISWHITE(*cts.cts_ptr))
	{
	    bdp->pre_whitesp += incr;
	    bdp->pre_whitesp_c++;
	}
	अन्यथा
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
    यदि (bdp->start_vcol < oap->start_vcol)	// line too short
    {
	bdp->end_vcol = bdp->start_vcol;
	bdp->is_short = TRUE;
	यदि (!is_del || oap->op_type == OP_APPEND)
	    bdp->endspaces = oap->end_vcol - oap->start_vcol + 1;
    }
    अन्यथा
    {
	// notice: this converts partly selected Multibyte characters to
	// spaces, too.
	bdp->startspaces = bdp->start_vcol - oap->start_vcol;
	यदि (is_del && bdp->startspaces)
	    bdp->startspaces = bdp->start_char_vcols - bdp->startspaces;
	pend = pstart;
	bdp->end_vcol = bdp->start_vcol;
	यदि (bdp->end_vcol > oap->end_vcol)	// it's all in one character
	{
	    bdp->is_oneChar = TRUE;
	    यदि (oap->op_type == OP_जोड़ो)
		bdp->endspaces = bdp->start_char_vcols - bdp->startspaces;
	    अन्यथा यदि (oap->op_type == OP_APPEND)
	    {
		bdp->startspaces += oap->end_vcol - oap->start_vcol + 1;
		bdp->endspaces = bdp->start_char_vcols - bdp->startspaces;
	    }
	    अन्यथा
	    {
		bdp->startspaces = oap->end_vcol - oap->start_vcol + 1;
		यदि (is_del && oap->op_type != OP_LSHIFT)
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
	अन्यथा
	{
	    init_chartabsize_arg(&cts, curwin, lnum, bdp->end_vcol,
								  पंक्ति, pend);
	    prev_pend = pend;
	    यावत् (cts.cts_vcol <= oap->end_vcol && *cts.cts_ptr != NUL)
	    {
		// count a tab for what it's worth (if list mode not on)
		prev_pend = cts.cts_ptr;
		incr = lbr_chartabsize_adv(&cts);
		cts.cts_vcol += incr;
	    }
	    bdp->end_vcol = cts.cts_vcol;
	    pend = cts.cts_ptr;
	    clear_chartabsize_arg(&cts);

	    यदि (bdp->end_vcol <= oap->end_vcol
		    && (!is_del
			|| oap->op_type == OP_APPEND
			|| oap->op_type == OP_REPLACE)) // line too short
	    {
		bdp->is_short = TRUE;
		// Alternative: include spaces to fill up the block.
		// Disadvantage: can lead to trailing spaces when the line is
		// short where the text is put
		// if (!is_del || oap->op_type == OP_APPEND)
		यदि (oap->op_type == OP_APPEND || virtual_op)
		    bdp->endspaces = oap->end_vcol - bdp->end_vcol
							     + oap->inclusive;
		अन्यथा
		    bdp->endspaces = 0; // replace doesn't add characters
	    }
	    अन्यथा यदि (bdp->end_vcol > oap->end_vcol)
	    {
		bdp->endspaces = bdp->end_vcol - oap->end_vcol - 1;
		यदि (!is_del && bdp->endspaces)
		{
		    bdp->endspaces = incr - bdp->endspaces;
		    यदि (pend != pstart)
			pend = prev_pend;
		}
	    }
	}
	bdp->end_char_vcols = incr;
	यदि (is_del && bdp->startspaces)
	    pstart = prev_pstart;
	bdp->textlen = (पूर्णाङ्क)(pend - pstart);
    }
    bdp->textcol = (colnr_T) (pstart - पंक्ति);
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

    यदि (lnum == start.lnum)
    {
	startcol = start.col;
	यदि (virtual_op)
	{
	    getvcol(curwin, &start, &cs, रिक्त, &ce, 0);
	    यदि (ce != cs && start.coladd > 0)
	    {
		// Part of a tab selected -- but don't
		// double-count it.
		bdp->start_char_vcols = ce - cs + 1;
		bdp->startspaces = bdp->start_char_vcols - start.coladd;
		यदि (bdp->startspaces < 0)
		    bdp->startspaces = 0;
		startcol++;
	    }
	}
    }

    यदि (lnum == end.lnum)
    {
	endcol = end.col;
	यदि (virtual_op)
	{
	    getvcol(curwin, &end, &cs, रिक्त, &ce, 0);
	    यदि (p[endcol] == NUL || (cs + end.coladd < ce
			// Don't add space for double-wide
			// char; endcol will be on last byte
			// of multi-byte char.
			&& (*mb_head_off)(p, p + endcol) == 0))
	    {
		यदि (start.lnum == end.lnum && start.col == end.col)
		{
		    // Special case: inside a single char
		    bdp->is_oneChar = TRUE;
		    bdp->startspaces = end.coladd - start.coladd + inclusive;
		    endcol = startcol;
		}
		अन्यथा
		{
		    bdp->endspaces = end.coladd + inclusive;
		    endcol -= inclusive;
		}
	    }
	}
    }
    यदि (endcol == MAXCOL)
	endcol = ml_get_len(lnum);
    यदि (startcol > endcol || bdp->is_oneChar)
	bdp->textlen = 0;
    अन्यथा
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

    यदि (!VIsual_active)
    {
	pos = curwin->w_cursor;
	यदि (u_save_cursor() == FAIL)
	{
#ifdef FEAT_FOLDING
	    disable_fold_update--;
#endif
	    प्रत्यागम;
	}
	change_cnt = do_addsub(oap->op_type, &pos, 0, amount);
#ifdef FEAT_FOLDING
	disable_fold_update--;
#endif
	यदि (change_cnt)
	    changed_पंक्तियाँ(pos.lnum, 0, pos.lnum + 1, 0L);
    }
    अन्यथा
    {
	पूर्णाङ्क	one_change;
	पूर्णाङ्क	length;
	pos_T	startpos;

	यदि (u_save((linenr_T)(oap->start.lnum - 1),
					(linenr_T)(oap->end.lnum + 1)) == FAIL)
	{
#ifdef FEAT_FOLDING
	    disable_fold_update--;
#endif
	    प्रत्यागम;
	}

	pos = oap->start;
	प्रति (; pos.lnum <= oap->end.lnum; ++pos.lnum)
	{
	    यदि (oap->block_mode)		    // Visual block mode
	    {
		block_prep(oap, &bd, pos.lnum, FALSE);
		pos.col = bd.textcol;
		length = bd.textlen;
	    }
	    अन्यथा यदि (oap->motion_type == MLINE)
	    {
		curwin->w_cursor.col = 0;
		pos.col = 0;
		length = ml_get_len(pos.lnum);
	    }
	    अन्यथा // oap->motion_type == MCHAR
	    {
		यदि (pos.lnum == oap->start.lnum && !oap->inclusive)
		    dec(&(oap->end));
		length = ml_get_len(pos.lnum);
		pos.col = 0;
		यदि (pos.lnum == oap->start.lnum)
		{
		    pos.col += oap->start.col;
		    length -= oap->start.col;
		}
		यदि (pos.lnum == oap->end.lnum)
		{
		    length = ml_get_len(oap->end.lnum);
		    यदि (oap->end.col >= length)
			oap->end.col = length - 1;
		    length = oap->end.col - pos.col + 1;
		}
	    }
	    one_change = do_addsub(oap->op_type, &pos, length, amount);
	    यदि (one_change)
	    {
		// Remember the start position of the first change.
		यदि (change_cnt == 0)
		    startpos = curbuf->b_op_start;
		++change_cnt;
	    }

#ifdef FEAT_NETBEANS_INTG
	    यदि (netbeans_active() && one_change)
	    {
		char_u *ptr;

		netbeans_removed(curbuf, pos.lnum, pos.col, (दीर्घ)length);
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, pos.col,
						&ptr[pos.col], length);
	    }
#endif
	    यदि (g_cmd && one_change)
		amount += Prenum1;
	}

#ifdef FEAT_FOLDING
	disable_fold_update--;
#endif
	यदि (change_cnt)
	    changed_पंक्तियाँ(oap->start.lnum, 0, oap->end.lnum + 1, 0L);

	यदि (!change_cnt && oap->is_VIsual)
	    // No change: need to remove the Visual selection
	    redraw_curbuf_later(UPD_INVERTED);

	// Set '[ mark if something changed. Keep the last end
	// position from do_addsub().
	यदि (change_cnt > 0 && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	    curbuf->b_op_start = startpos;

	यदि (change_cnt > p_report)
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

    यदि (virtual_active())
    {
	save_coladd = pos->coladd;
	pos->coladd = 0;
    }

    curwin->w_cursor = *pos;
    ptr = ml_get(pos->lnum);
    linelen = ml_get_len(pos->lnum);
    col = pos->col;

    यदि (col + !!save_coladd >= linelen)
	गच्छ theend;

    /*
     * First check if we are on a hexadecimal number, after the "0x".
     */
    यदि (!VIsual_active)
    {
	यदि (do_bin)
	    यावत् (col > 0 && vim_isbdigit(ptr[col]))
	    {
		--col;
		यदि (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }

	यदि (do_hex)
	    यावत् (col > 0 && vim_isxdigit(ptr[col]))
	    {
		--col;
		यदि (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }

	यदि (       do_bin
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

	    यावत् (col > 0 && vim_isdigit(ptr[col]))
	    {
		col--;
		यदि (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }
	}

	यदि ((       do_hex
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
	    यदि (has_mbyte)
		col -= (*mb_head_off)(ptr, ptr + col);
	}
	अन्यथा
	{
	    /*
	     * Search forward and then backward to find the start of number.
	     */
	    col = pos->col;

	    यावत् (ptr[col] != NUL
		    && !vim_isdigit(ptr[col])
		    && !(do_alpha && ASCII_ISALPHA(ptr[col])))
		col += mb_ptr2len(ptr + col);

	    यावत् (col > 0
		    && vim_isdigit(ptr[col - 1])
		    && !(do_alpha && ASCII_ISALPHA(ptr[col])))
	    {
		--col;
		यदि (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }
	}
    }

    यदि (visual)
    {
	यावत् (ptr[col] != NUL && length > 0
		&& !vim_isdigit(ptr[col])
		&& !(do_alpha && ASCII_ISALPHA(ptr[col])))
	{
	    पूर्णाङ्क mb_len = mb_ptr2len(ptr + col);

	    col += mb_len;
	    length -= mb_len;
	}

	यदि (length == 0)
	    गच्छ theend;

	यदि (col > pos->col && ptr[col - 1] == '-'
		&& (!has_mbyte || !(*mb_head_off)(ptr, ptr + col - 1))
		&& !do_unsigned)
	{
	    यदि (do_blank && col >= 2 && !VIM_ISWHITE(ptr[col - 2]))
		blank_unsigned = TRUE;
	    अन्यथा
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
    यदि (!VIM_ISDIGIT(firstdigit) && !(do_alpha && ASCII_ISALPHA(firstdigit)))
    {
	beep_flush();
	गच्छ theend;
    }

    यदि (do_alpha && ASCII_ISALPHA(firstdigit))
    {
	// decrement or increment alphabetic character
	यदि (op_type == OP_NR_SUB)
	{
	    यदि (CharOrd(firstdigit) < Prenum1)
	    {
		यदि (SAFE_isupper(firstdigit))
		    firstdigit = 'A';
		अन्यथा
		    firstdigit = 'a';
	    }
	    अन्यथा
		firstdigit -= Prenum1;
	}
	अन्यथा
	{
	    यदि (26 - CharOrd(firstdigit) - 1 < Prenum1)
	    {
		यदि (SAFE_isupper(firstdigit))
		    firstdigit = 'Z';
		अन्यथा
		    firstdigit = 'z';
	    }
	    अन्यथा
		firstdigit += Prenum1;
	}
	curwin->w_cursor.col = col;
	यदि (!did_change)
	    startpos = curwin->w_cursor;
	did_change = TRUE;
	(शून्य)del_char(FALSE);
	ins_char(firstdigit);
	endpos = curwin->w_cursor;
	curwin->w_cursor.col = col;
    }
    अन्यथा
    {
	char_u	*buf1;
	पूर्णाङ्क	buf1len;
	char_u	buf2[NUMBUFLEN];
	पूर्णाङ्क	buf2len;
	pos_T	save_pos;
	पूर्णाङ्क	i;

	यदि (col > 0 && ptr[col - 1] == '-'
		&& (!has_mbyte ||
		    !(*mb_head_off)(ptr, ptr + col - 1))
		&& !visual
		&& !do_unsigned)
	{
	    यदि (do_blank && col >= 2 && !VIM_ISWHITE(ptr[col - 2]))
		blank_unsigned = TRUE;
	    अन्यथा
	    {
		// negative number
		--col;
		negative = TRUE;
	    }
	}

	// get the number value (unsigned)
	यदि (visual && VIsual_mode != 'V')
	    maxlen = (curbuf->b_visual.vi_curswant == MAXCOL
		    ? linelen - col : length);

	पूर्णाङ्क overflow = FALSE;
	vim_str2nr(ptr + col, &pre, &length,
		0 + (do_bin ? STR2NR_BIN : 0)
		    + (do_oct ? STR2NR_OCT : 0)
		    + (do_hex ? STR2NR_HEX : 0),
		रिक्त, &n, maxlen, FALSE, &overflow);

	// ignore leading '-' for hex and octal and bin numbers
	यदि (pre && negative)
	{
	    ++col;
	    --length;
	    negative = FALSE;
	}
	// add or subtract
	subtract = FALSE;
	यदि (op_type == OP_NR_SUB)
	    subtract ^= TRUE;
	यदि (negative)
	    subtract ^= TRUE;

	oldn = n;
	यदि (!overflow)  // if number is too big don't add/subtract
	{
	    यदि (subtract)
		n -= (uvarnumber_T)Prenum1;
	    अन्यथा
		n += (uvarnumber_T)Prenum1;
	}

	// handle wraparound for decimal numbers
	यदि (!pre)
	{
	    यदि (subtract)
	    {
		यदि (n > oldn)
		{
		    n = 1 + (n ^ (uvarnumber_T)-1);
		    negative ^= TRUE;
		}
	    }
	    अन्यथा
	    {
		// add
		यदि (n < oldn)
		{
		    n = (n ^ (uvarnumber_T)-1);
		    negative ^= TRUE;
		}
	    }
	    यदि (n == 0)
		negative = FALSE;
	}

	यदि ((do_unsigned || blank_unsigned) && negative)
	{
	    यदि (subtract)
		// sticking at zero.
		n = (uvarnumber_T)0;
	    अन्यथा
		// sticking at 2^64 - 1.
		n = (uvarnumber_T)(-1);
	    negative = FALSE;
	}

	यदि (visual && !was_positive && !negative && col > 0)
	{
	    // need to remove the '-'
	    col--;
	    length++;
	}

	/*
	 * Delete the old number.
	 */
	curwin->w_cursor.col = col;
	यदि (!did_change)
	    startpos = curwin->w_cursor;
	did_change = TRUE;
	todel = length;
	c = gchar_cursor();
	/*
	 * Don't include the '-' in the length, only the length of the
	 * part after it is kept the same.
	 */
	यदि (c == '-')
	    --length;

	save_pos = curwin->w_cursor;
	प्रति (i = 0; i < todel; ++i)
	{
	    यदि (c < 0x100 && SAFE_isalpha(c))
	    {
		यदि (SAFE_isupper(c))
		    hexupper = TRUE;
		अन्यथा
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
	यदि (buf1 == रिक्त)
	    गच्छ theend;
	ptr = buf1;
	यदि (negative && (!visual || was_positive))
	    *ptr++ = '-';
	यदि (pre)
	{
	    *ptr++ = '0';
	    --length;
	}
	यदि (pre == 'b' || pre == 'B' ||
	    pre == 'x' || pre == 'X')
	{
	    *ptr++ = pre;
	    --length;
	}

	/*
	 * Put the number characters in buf2[].
	 */
	यदि (pre == 'b' || pre == 'B')
	{
	    पूर्णाङ्क bit = 0;
	    पूर्णाङ्क bits = परिमाण(uvarnumber_T) * 8;

	    // leading zeros
	    प्रति (bit = bits; bit > 0; bit--)
		यदि ((n >> (bit - 1)) & 0x1) भङ्ग;

	    प्रति (buf2len = 0; bit > 0 && buf2len < (NUMBUFLEN - 1); bit--)
		buf2[buf2len++] = ((n >> (bit - 1)) & 0x1) ? '1' : '0';

	    buf2[buf2len] = NUL;
	}
	अन्यथा यदि (pre == 0)
	    buf2len = vim_snprintf((वर्ण *)buf2, NUMBUFLEN, "%llu", n);
	अन्यथा यदि (pre == '0')
	    buf2len = vim_snprintf((वर्ण *)buf2, NUMBUFLEN, "%llo", n);
	अन्यथा यदि (pre && hexupper)
	    buf2len = vim_snprintf((वर्ण *)buf2, NUMBUFLEN, "%llX", n);
	अन्यथा
	    buf2len = vim_snprintf((वर्ण *)buf2, NUMBUFLEN, "%llx", n);
	length -= buf2len;

	/*
	 * Adjust number of zeros to the new number of digits, so the
	 * total length of the number remains the same.
	 * Don't do this when
	 * the result may look like an octal number.
	 */
	यदि (firstdigit == '0' && !(do_oct && pre == 0))
	    यावत् (length-- > 0)
		*ptr++ = '0';
	*ptr = NUL;
	buf1len = (पूर्णाङ्क)(ptr - buf1);

	STRCPY(buf1 + buf1len, buf2);
	buf1len += buf2len;

	// Insert just after the first character to be removed, so that any
	// text properties will be adjusted.  Then delete the old number
	// afterwards.
	save_pos = curwin->w_cursor;
	यदि (todel > 0)
	    inc_cursor();
	ins_str(buf1, (माप_प्रकार)buf1len);		// insert the new number
	vim_free(buf1);

	// del_char() will also mark line needing displaying
	यदि (todel > 0)
	{
	    पूर्णाङ्क bytes_after = ml_get_curline_len() - curwin->w_cursor.col;

	    // Delete the one character before the insert.
	    curwin->w_cursor = save_pos;
	    (शून्य)del_char(FALSE);
	    curwin->w_cursor.col = ml_get_curline_len() - bytes_after;
	    --todel;
	}
	यावत् (todel-- > 0)
	    (शून्य)del_char(FALSE);

	endpos = curwin->w_cursor;
	यदि (did_change && curwin->w_cursor.col)
	    --curwin->w_cursor.col;
    }

    यदि (did_change && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// set the '[ and '] marks
	curbuf->b_op_start = startpos;
	curbuf->b_op_end = endpos;
	यदि (curbuf->b_op_end.col > 0)
	    --curbuf->b_op_end.col;
    }

theend:
    यदि (visual)
	curwin->w_cursor = save_cursor;
    अन्यथा यदि (did_change)
	curwin->w_set_curswant = true;
    अन्यथा यदि (virtual_active())
	curwin->w_cursor.coladd = save_coladd;

    प्रत्यागम did_change;
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
पंक्ति_गिनती_info(
    char_u	*पंक्ति,
    varnumber_T	*wc,
    varnumber_T	*cc,
    varnumber_T	limit,
    पूर्णाङ्क		eol_size)
{
    varnumber_T	i;
    varnumber_T	शब्दगण = 0;
    varnumber_T	chars = 0;
    पूर्णाङ्क		is_word = 0;

    प्रति (i = 0; i < limit && पंक्ति[i] != NUL; )
    {
	यदि (is_word)
	{
	    यदि (vim_isspace(पंक्ति[i]))
	    {
		शब्दगण++;
		is_word = 0;
	    }
	}
	अन्यथा यदि (!vim_isspace(पंक्ति[i]))
	    is_word = 1;
	++chars;
	i += (*mb_ptr2len)(पंक्ति + i);
    }

    यदि (is_word)
	शब्दगण++;
    *wc += शब्दगण;

    // Add eol_size if the end of line was reached before hitting limit.
    यदि (i < limit && पंक्ति[i] == NUL)
    {
	i += eol_size;
	chars += eol_size;
    }
    *cc += chars;
    प्रत्यागम i;
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
    varnumber_T	byte_गिनती = 0;
    varnumber_T	bom_गिनती  = 0;
    varnumber_T	byte_गिनती_cursor = 0;
    varnumber_T	char_गिनती = 0;
    varnumber_T	char_गिनती_cursor = 0;
    varnumber_T	word_गिनती = 0;
    varnumber_T	word_गिनती_cursor = 0;
    पूर्णाङ्क		eol_size;
    varnumber_T	last_check = 100000L;
    दीर्घ	पंक्ति_गिनती_selected = 0;
    pos_T	min_pos, max_pos;
    oparg_T	oparg;
    संरचना block_def	bd;

    /*
     * Compute the length of the file in characters.
     */
    यदि (curbuf->b_ml.ml_flags & ML_EMPTY)
    {
	यदि (dict == रिक्त)
	{
	    संदेश(_(no_पंक्तियाँ_संदेश));
	    प्रत्यागम;
	}
    }
    अन्यथा
    {
	यदि (get_fileformat(curbuf) == EOL_DOS)
	    eol_size = 2;
	अन्यथा
	    eol_size = 1;

	यदि (VIsual_active)
	{
	    यदि (LT_POS(VIsual, curwin->w_cursor))
	    {
		min_pos = VIsual;
		max_pos = curwin->w_cursor;
	    }
	    अन्यथा
	    {
		min_pos = curwin->w_cursor;
		max_pos = VIsual;
	    }
	    यदि (*p_sel == 'e' && max_pos.col > 0)
		--max_pos.col;

	    यदि (VIsual_mode == Ctrl_V)
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
		यदि (curwin->w_curswant == MAXCOL)
		    oparg.end_vcol = MAXCOL;
		// Swap the start, end vcol if needed
		यदि (oparg.end_vcol < oparg.start_vcol)
		{
		    oparg.end_vcol += oparg.start_vcol;
		    oparg.start_vcol = oparg.end_vcol - oparg.start_vcol;
		    oparg.end_vcol -= oparg.start_vcol;
		}
	    }
	    पंक्ति_गिनती_selected = max_pos.lnum - min_pos.lnum + 1;
	}

	प्रति (lnum = 1; lnum <= curbuf->b_ml.ml_पंक्ति_गिनती; ++lnum)
	{
	    // Check for a CTRL-C every 100000 characters.
	    यदि (byte_गिनती > last_check)
	    {
		ui_breakcheck();
		यदि (got_int)
		    प्रत्यागम;
		last_check = byte_गिनती + 100000L;
	    }

	    // Do extra processing for VIsual mode.
	    यदि (VIsual_active
		    && lnum >= min_pos.lnum && lnum <= max_pos.lnum)
	    {
		char_u	    *s = रिक्त;
		दीर्घ	    len = 0L;

		विकल्प (VIsual_mode)
		{
		    पक्ष Ctrl_V:
			virtual_op = virtual_active();
			block_prep(&oparg, &bd, lnum, 0);
			virtual_op = MAYBE;
			s = bd.textstart;
			len = (दीर्घ)bd.textlen;
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
		यदि (s != रिक्त)
		{
		    byte_गिनती_cursor += पंक्ति_गिनती_info(s, &word_गिनती_cursor,
					   &char_गिनती_cursor, len, eol_size);
		    यदि (lnum == curbuf->b_ml.ml_पंक्ति_गिनती
			    && !curbuf->b_p_eol
			    && (curbuf->b_p_bin || !curbuf->b_p_fixeol)
			    && (दीर्घ)STRLEN(s) < len)
			byte_गिनती_cursor -= eol_size;
		}
	    }
	    अन्यथा
	    {
		// In non-visual mode, check for the line the cursor is on
		यदि (lnum == curwin->w_cursor.lnum)
		{
		    word_गिनती_cursor += word_गिनती;
		    char_गिनती_cursor += char_गिनती;
		    byte_गिनती_cursor = byte_गिनती +
			पंक्ति_गिनती_info(ml_get(lnum),
				&word_गिनती_cursor, &char_गिनती_cursor,
				(varnumber_T)(curwin->w_cursor.col + 1),
				eol_size);
		}
	    }
	    // Add to the running totals
	    byte_गिनती += पंक्ति_गिनती_info(ml_get(lnum), &word_गिनती,
					 &char_गिनती, (varnumber_T)MAXCOL,
					 eol_size);
	}

	// Correction for when last line doesn't have an EOL.
	यदि (!curbuf->b_p_eol && (curbuf->b_p_bin || !curbuf->b_p_fixeol))
	    byte_गिनती -= eol_size;

	यदि (dict == रिक्त)
	{
	    यदि (VIsual_active)
	    {
		यदि (VIsual_mode == Ctrl_V && curwin->w_curswant < MAXCOL)
		{
		    getvcols(curwin, &min_pos, &max_pos,
						&min_pos.col, &max_pos.col, 0);
		    vim_snprintf((वर्ण *)buf1, परिमाण(buf1), _("%ld Cols; "),
			    (दीर्घ)(oparg.end_vcol - oparg.start_vcol + 1));
		}
		अन्यथा
		    buf1[0] = NUL;

		यदि (char_गिनती_cursor == byte_गिनती_cursor
						    && char_गिनती == byte_गिनती)
		    vim_snprintf((वर्ण *)IObuff, IOSIZE,
			    _("Selected %s%ld of %ld Lines; %lld of %lld Words; %lld of %lld Bytes"),
			    buf1, पंक्ति_गिनती_selected,
			    (दीर्घ)curbuf->b_ml.ml_पंक्ति_गिनती,
			    word_गिनती_cursor,
			    word_गिनती,
			    byte_गिनती_cursor,
			    byte_गिनती);
		अन्यथा
		    vim_snprintf((वर्ण *)IObuff, IOSIZE,
			    _("Selected %s%ld of %ld Lines; %lld of %lld Words; %lld of %lld Chars; %lld of %lld Bytes"),
			    buf1, पंक्ति_गिनती_selected,
			    (दीर्घ)curbuf->b_ml.ml_पंक्ति_गिनती,
			    word_गिनती_cursor,
			    word_गिनती,
			    char_गिनती_cursor,
			    char_गिनती,
			    byte_गिनती_cursor,
			    byte_गिनती);
	    }
	    अन्यथा
	    {
		p = ml_get_curline();
		validate_virtcol();
		col_print(buf1, परिमाण(buf1), (पूर्णाङ्क)curwin->w_cursor.col + 1,
			(पूर्णाङ्क)curwin->w_virtcol + 1);
		col_print(buf2, परिमाण(buf2), ml_get_curline_len(),
							   linetabsize_str(p));

		यदि (char_गिनती_cursor == byte_गिनती_cursor
			&& char_गिनती == byte_गिनती)
		    vim_snprintf((वर्ण *)IObuff, IOSIZE,
			_("Col %s of %s; Line %ld of %ld; Word %lld of %lld; Byte %lld of %lld"),
			(वर्ण *)buf1, (वर्ण *)buf2,
			(दीर्घ)curwin->w_cursor.lnum,
			(दीर्घ)curbuf->b_ml.ml_पंक्ति_गिनती,
			word_गिनती_cursor, word_गिनती,
			byte_गिनती_cursor, byte_गिनती);
		अन्यथा
		    vim_snprintf((वर्ण *)IObuff, IOSIZE,
			_("Col %s of %s; Line %ld of %ld; Word %lld of %lld; Char %lld of %lld; Byte %lld of %lld"),
			(वर्ण *)buf1, (वर्ण *)buf2,
			(दीर्घ)curwin->w_cursor.lnum,
			(दीर्घ)curbuf->b_ml.ml_पंक्ति_गिनती,
			word_गिनती_cursor, word_गिनती,
			char_गिनती_cursor, char_गिनती,
			byte_गिनती_cursor, byte_गिनती);
	    }
	}

	bom_गिनती = bomb_size();
	यदि (dict == रिक्त && bom_गिनती > 0)
	{
	    माप_प्रकार len = STRLEN(IObuff);

	    vim_snprintf((वर्ण *)IObuff + len, IOSIZE - len,
				 _("(+%lld for BOM)"), bom_गिनती);
	}
	यदि (dict == रिक्त)
	{
	    // Don't shorten this message, the user asked for it.
	    p = p_shm;
	    p_shm = (char_u *)"";
	    संदेश((वर्ण *)IObuff);
	    p_shm = p;
	}
    }
#यदि defined(FEAT_EVAL)
    यदि (dict != रिक्त)
    {
	dict_add_संख्या(dict, "words", word_गिनती);
	dict_add_संख्या(dict, "chars", char_गिनती);
	dict_add_संख्या(dict, "bytes", byte_गिनती + bom_गिनती);
	dict_add_संख्या(dict, VIsual_active ? "visual_bytes" : "cursor_bytes",
		byte_गिनती_cursor);
	dict_add_संख्या(dict, VIsual_active ? "visual_chars" : "cursor_chars",
		char_गिनती_cursor);
	dict_add_संख्या(dict, VIsual_active ? "visual_words" : "cursor_words",
		word_गिनती_cursor);
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
    यदि (oap->is_VIsual)
	stuffReadbuff((char_u *)"'<,'>");
    अन्यथा
    {
	// Make the range look nice, so it can be repeated.
	यदि (oap->start.lnum == curwin->w_cursor.lnum)
	    stuffcharReadbuff('.');
	अन्यथा
	    stuffnumReadbuff((दीर्घ)oap->start.lnum);

#ifdef FEAT_FOLDING
	// When using !! on a closed fold the range ".!" works best to operate
	// on, it will be made the whole closed fold later.
	linenr_T endOfStartFold = oap->start.lnum;
	(शून्य)hasFolding(oap->start.lnum, रिक्त, &endOfStartFold);
#endif
	यदि (oap->end.lnum != oap->start.lnum
#ifdef FEAT_FOLDING
		&& oap->end.lnum != endOfStartFold
#endif
	   )
	{
	    // Make it a range with the end line.
	    stuffcharReadbuff(',');
	    यदि (oap->end.lnum == curwin->w_cursor.lnum)
		stuffcharReadbuff('.');
	    अन्यथा यदि (oap->end.lnum == curbuf->b_ml.ml_पंक्ति_गिनती)
		stuffcharReadbuff('$');
	    अन्यथा यदि (oap->start.lnum == curwin->w_cursor.lnum
#ifdef FEAT_FOLDING
		    // do not use ".+number" for a closed fold, it would count
		    // folded lines twice
		    && !hasFolding(oap->end.lnum, रिक्त, रिक्त)
#endif
		    )
	    {
		stuffReadbuff((char_u *)".+");
		stuffnumReadbuff((दीर्घ)oap->पंक्ति_गिनती - 1);
	    }
	    अन्यथा
		stuffnumReadbuff((दीर्घ)oap->end.lnum);
	}
    }
    यदि (oap->op_type != OP_COLON)
	stuffReadbuff((char_u *)"!");
    यदि (oap->op_type == OP_INDENT)
    {
	यदि (*get_equalprg() == NUL)
	    stuffReadbuff((char_u *)"indent");
	अन्यथा
	    stuffReadbuff(get_equalprg());
	stuffReadbuff((char_u *)"\n");
    }
    अन्यथा यदि (oap->op_type == OP_FORMAT)
    {
	यदि (*curbuf->b_p_fp != NUL)
	    stuffReadbuff(curbuf->b_p_fp);
	अन्यथा यदि (*p_fp != NUL)
	    stuffReadbuff(p_fp);
	अन्यथा
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
    यदि (option_set_callback_func(p_opfunc, &opfunc_cb) == FAIL)
	प्रत्यागम e_invalid_argument;

    प्रत्यागम रिक्त;
}

#यदि defined(EXITFREE)
    शून्य
free_operatorfunc_option(शून्य)
{
# ifdef FEAT_EVAL
    free_callback(&opfunc_cb);
# endif
}
#endif

#यदि defined(FEAT_EVAL)
/*
 * Mark the global 'operatorfunc' callback with "copyID" so that it is not
 * garbage collected.
 */
    पूर्णाङ्क
set_ref_in_opfunc(पूर्णाङ्क copyID UNUSED)
{
    पूर्णाङ्क पात = FALSE;

    पात = set_ref_in_callback(&opfunc_cb, copyID);

    प्रत्यागम पात;
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

    यदि (*p_opfunc == NUL)
	emsg(_(e_operatorfunc_is_empty));
    अन्यथा
    {
	// Set '[ and '] marks to text to be operated on.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
	यदि (oap->motion_type != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_end);

	argv[0].v_type = VAR_STRING;
	यदि (oap->block_mode)
	    argv[0].vval.v_string = (char_u *)"block";
	अन्यथा यदि (oap->motion_type == MLINE)
	    argv[0].vval.v_string = (char_u *)"line";
	अन्यथा
	    argv[0].vval.v_string = (char_u *)"char";
	argv[1].v_type = VAR_UNKNOWN;

	// Reset virtual_op so that 'virtualedit' can be changed in the
	// function.
	पूर्णाङ्क save_virtual_op = virtual_op;
	virtual_op = MAYBE;

	// Reset finish_op so that mode() returns the right value.
	पूर्णाङ्क save_finish_op = finish_op;
	finish_op = FALSE;

	यदि (call_callback(&opfunc_cb, 0, &rettv, 1, argv) != FAIL)
	    clear_tv(&rettv);

	virtual_op = save_virtual_op;
	finish_op = save_finish_op;
	यदि (cmdmod.cmod_flags & CMOD_LOCKMARKS)
	{
	    curbuf->b_op_start = orig_start;
	    curbuf->b_op_end = orig_end;
	}
    }
#अन्यथा
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

    यदि (VIsual_mode != Ctrl_V
	    || (!initial && oap->end.col < curwin->w_width))
	प्रत्यागम;

    oap->block_mode = TRUE;

    // prevent from moving onto a trail byte
    यदि (has_mbyte)
	mb_adjustpos(curwin->w_buffer, &oap->end);

    getvvcol(curwin, &(oap->start), &oap->start_vcol, रिक्त, &oap->end_vcol, 0);

    यदि (!redo_VIsual_busy)
    {
	getvvcol(curwin, &(oap->end), &start, रिक्त, &end, 0);

	यदि (start < oap->start_vcol)
	    oap->start_vcol = start;
	यदि (end > oap->end_vcol)
	{
	    यदि (initial && *p_sel == 'e' && start >= 1
				    && start - 1 >= oap->end_vcol)
		oap->end_vcol = start - 1;
	    अन्यथा
		oap->end_vcol = end;
	}
    }

    // if '$' was used, get oap->end_vcol from longest line
    यदि (curwin->w_curswant == MAXCOL)
    {
	curwin->w_cursor.col = MAXCOL;
	oap->end_vcol = 0;
	प्रति (curwin->w_cursor.lnum = oap->start.lnum;
		curwin->w_cursor.lnum <= oap->end.lnum;
					++curwin->w_cursor.lnum)
	{
	    getvvcol(curwin, &curwin->w_cursor, रिक्त, रिक्त, &end, 0);
	    यदि (end > oap->end_vcol)
		oap->end_vcol = end;
	}
    }
    अन्यथा यदि (redo_VIsual_busy)
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
    linenr_T	rv_पंक्ति_गिनती;	// number of lines
    colnr_T	rv_vcol;	// number of cols or end column
    दीर्घ	rv_गिनती;	// count for Visual operator
    पूर्णाङ्क		rv_arg;		// extra argument
} redo_VIsual_T;

    स्थिर पूर्णाङ्क
is_ex_cmdchar(cmdarg_T *cap)
{
    प्रत्यागम cap->cmdchar == ':'
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

    पूर्णाङ्क		    include_पंक्ति_break = FALSE;

#यदि defined(FEAT_CLIPBOARD)
    // Yank the visual area into the GUI selection register before we operate
    // on it and lose it forever.
    // Don't do it if a specific register was specified, so that ""x"*P works.
    // This could call do_pending_operator() recursively, but that's OK
    // because gui_yank will be TRUE for the nested call.
    यदि ((clip_star.available || clip_plus.available)
	    && oap->op_type != OP_NOP
	    && !gui_yank
	    && VIsual_active
	    && !redo_VIsual_busy
	    && oap->regname == 0)
	clip_auto_select();
#endif
    old_cursor = curwin->w_cursor;

    // If an operation is pending, handle it...
    यदि ((finish_op || VIsual_active) && oap->op_type != OP_NOP)
    {
	// Yank can be redone when 'y' is in 'cpoptions', but not when yanking
	// for the clipboard.
	पूर्णाङ्क	redo_yank = vim_strchr(p_cpo, CPO_YANK) != रिक्त && !gui_yank;

#ifdef FEAT_LINEBREAK
	// Avoid a problem with unwanted linebreaks in block mode.
	(शून्य)reset_lbr();
#endif
	oap->is_VIsual = VIsual_active;
	यदि (oap->motion_force == 'V')
	    oap->motion_type = MLINE;
	अन्यथा यदि (oap->motion_force == 'v')
	{
	    // If the motion was linewise, "inclusive" will not have been set.
	    // Use "exclusive" to be consistent.  Makes "dvj" work nice.
	    यदि (oap->motion_type == MLINE)
		oap->inclusive = FALSE;
	    // If the motion already was characterwise, toggle "inclusive"
	    अन्यथा यदि (oap->motion_type == MCHAR)
		oap->inclusive = !oap->inclusive;
	    oap->motion_type = MCHAR;
	}
	अन्यथा यदि (oap->motion_force == Ctrl_V)
	{
	    // Change line- or characterwise motion into Visual block mode.
	    यदि (!VIsual_active)
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
	यदि ((redo_yank || oap->op_type != OP_YANK)
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
	    यदि (cap->cmdchar == '/' || cap->cmdchar == '?') // was a search
	    {
		// If 'cpoptions' does not contain 'r', insert the search
		// pattern to really repeat the same command.
		यदि (vim_strchr(p_cpo, CPO_REDO) == रिक्त)
		    AppendToRedobuffLit(cap->searchbuf, -1);
		AppendToRedobuff(NL_STR);
	    }
	    अन्यथा यदि (is_ex_cmdchar(cap))
	    {
		// do_cmdline() has stored the first typed line in
		// "repeat_cmdline".  When several lines are typed repeating
		// won't be possible.
		यदि (repeat_cmdline == रिक्त)
		    ResetRedobuff();
		अन्यथा
		{
		    यदि (cap->cmdchar == ':')
			AppendToRedobuffLit(repeat_cmdline, -1);
		    अन्यथा
			AppendToRedobuffSpec(repeat_cmdline);
		    AppendToRedobuff(NL_STR);
		    VIM_CLEAR(repeat_cmdline);
		}
	    }
	}

	यदि (redo_VIsual_busy)
	{
	    // Redo of an operation on a Visual area. Use the same size from
	    // redo_VIsual.rv_line_count and redo_VIsual.rv_vcol.
	    oap->start = curwin->w_cursor;
	    curwin->w_cursor.lnum += redo_VIsual.rv_पंक्ति_गिनती - 1;
	    यदि (curwin->w_cursor.lnum > curbuf->b_ml.ml_पंक्ति_गिनती)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_पंक्ति_गिनती;
	    VIsual_mode = redo_VIsual.rv_mode;
	    यदि (redo_VIsual.rv_vcol == MAXCOL || VIsual_mode == 'v')
	    {
		यदि (VIsual_mode == 'v')
		{
		    यदि (redo_VIsual.rv_पंक्ति_गिनती <= 1)
		    {
			validate_virtcol();
			curwin->w_curswant =
				     curwin->w_virtcol + redo_VIsual.rv_vcol - 1;
		    }
		    अन्यथा
			curwin->w_curswant = redo_VIsual.rv_vcol;
		}
		अन्यथा
		{
		    curwin->w_curswant = MAXCOL;
		}
		coladvance(curwin->w_curswant);
	    }
	    cap->count0 = redo_VIsual.rv_गिनती;
	    यदि (redo_VIsual.rv_गिनती != 0)
		cap->count1 = redo_VIsual.rv_गिनती;
	    अन्यथा
		cap->count1 = 1;
	}
	अन्यथा यदि (VIsual_active)
	{
	    यदि (!gui_yank)
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
	    यदि (VIsual_select && VIsual_mode == 'V'
					    && cap->oap->op_type != OP_DELETE)
	    {
		यदि (LT_POS(VIsual, curwin->w_cursor))
		{
		    VIsual.col = 0;
		    curwin->w_cursor.col = ml_get_len(curwin->w_cursor.lnum);
		}
		अन्यथा
		{
		    curwin->w_cursor.col = 0;
		    VIsual.col = ml_get_len(VIsual.lnum);
		}
		VIsual_mode = 'v';
	    }
	    // If 'selection' is "exclusive", backup one character for
	    // charwise selections.
	    अन्यथा यदि (VIsual_mode == 'v')
		include_पंक्ति_break = unadjust_for_sel();

	    oap->start = VIsual;
	    यदि (VIsual_mode == 'V')
	    {
		oap->start.col = 0;
		oap->start.coladd = 0;
	    }
	}

	// Set oap->start to the first position of the operated text, oap->end
	// to the end of the operated text.  w_cursor is equal to oap->start.
	यदि (LT_POS(oap->start, curwin->w_cursor))
	{
#ifdef FEAT_FOLDING
	    // Include folded lines completely.
	    यदि (!VIsual_active)
	    {
		यदि (hasFolding(oap->start.lnum, &oap->start.lnum, रिक्त))
		    oap->start.col = 0;
		यदि ((curwin->w_cursor.col > 0 || oap->inclusive
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
	अन्यथा
	{
#ifdef FEAT_FOLDING
	    // Include folded lines completely.
	    यदि (!VIsual_active && oap->motion_type == MLINE)
	    {
		यदि (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum,
									रिक्त))
		    curwin->w_cursor.col = 0;
		यदि (hasFolding(oap->start.lnum, रिक्त, &oap->start.lnum))
		    oap->start.col = ml_get_len(oap->start.lnum);
	    }
#endif
	    oap->end = oap->start;
	    oap->start = curwin->w_cursor;
	}

	// Just in case lines were deleted that make the position invalid.
	check_pos(curwin->w_buffer, &oap->end);
	oap->पंक्ति_गिनती = oap->end.lnum - oap->start.lnum + 1;

	// Set "virtual_op" before resetting VIsual_active.
	virtual_op = virtual_active();

	यदि (VIsual_active || redo_VIsual_busy)
	{
	    get_op_vcol(oap, redo_VIsual.rv_vcol, TRUE);

	    यदि (!redo_VIsual_busy && !gui_yank)
	    {
		// Prepare to reselect and redo Visual: this is based on the
		// size of the Visual text
		resel_VIsual_mode = VIsual_mode;
		यदि (curwin->w_curswant == MAXCOL)
		    resel_VIsual_vcol = MAXCOL;
		अन्यथा
		{
		    यदि (VIsual_mode != Ctrl_V)
			getvvcol(curwin, &(oap->end),
						रिक्त, रिक्त, &oap->end_vcol, 0);
		    यदि (VIsual_mode == Ctrl_V || oap->पंक्ति_गिनती <= 1)
		    {
			यदि (VIsual_mode != Ctrl_V)
			    getvvcol(curwin, &(oap->start),
					      &oap->start_vcol, रिक्त, रिक्त, 0);
			resel_VIsual_vcol = oap->end_vcol - oap->start_vcol + 1;
		    }
		    अन्यथा
			resel_VIsual_vcol = oap->end_vcol;
		}
		resel_VIsual_पंक्ति_गिनती = oap->पंक्ति_गिनती;
	    }

	    // can't redo yank (unless 'y' is in 'cpoptions') and ":"
	    यदि ((redo_yank || oap->op_type != OP_YANK)
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
		यदि (cap->cmdchar == 'g' && (cap->nchar == 'n'
							|| cap->nchar == 'N'))
		    prep_redo(oap->regname, cap->count0,
			    get_op_char(oap->op_type),
			    get_extra_op_char(oap->op_type),
			    oap->motion_force, cap->cmdchar, cap->nchar);
		अन्यथा यदि (!is_ex_cmdchar(cap))
		{
		    पूर्णाङ्क opchar = get_op_char(oap->op_type);
		    पूर्णाङ्क extra_opchar = get_extra_op_char(oap->op_type);
		    पूर्णाङ्क nchar = oap->op_type == OP_REPLACE ? cap->nchar : NUL;

		    // reverse what nv_replace() did
		    यदि (nchar == REPLACE_CR_NCHAR)
			nchar = CAR;
		    अन्यथा यदि (nchar == REPLACE_NL_NCHAR)
			nchar = NL;

		    यदि (opchar == 'g' && extra_opchar == '@')
			// also repeat the count for 'operatorfunc'
			prep_redo_num2(oap->regname, 0L, NUL, 'v',
				     cap->count0, opchar, extra_opchar, nchar);
		    अन्यथा
			prep_redo(oap->regname, 0L, NUL, 'v',
						  opchar, extra_opchar, nchar);
		}
		यदि (!redo_VIsual_busy)
		{
		    redo_VIsual.rv_mode = resel_VIsual_mode;
		    redo_VIsual.rv_vcol = resel_VIsual_vcol;
		    redo_VIsual.rv_पंक्ति_गिनती = resel_VIsual_पंक्ति_गिनती;
		    redo_VIsual.rv_गिनती = cap->count0;
		    redo_VIsual.rv_arg = cap->arg;
		}
	    }

	    // oap->inclusive defaults to TRUE.
	    // If oap->end is on a NUL (empty line) oap->inclusive becomes
	    // FALSE.  This makes "d}P" and "v}dP" work the same.
	    यदि (oap->motion_force == NUL || oap->motion_type == MLINE)
		oap->inclusive = TRUE;
	    यदि (VIsual_mode == 'V')
		oap->motion_type = MLINE;
	    अन्यथा
	    {
		oap->motion_type = MCHAR;
		यदि (VIsual_mode != Ctrl_V && *ml_get_pos(&(oap->end)) == NUL
			&& (include_पंक्ति_break || !virtual_op))
		{
		    oap->inclusive = FALSE;
		    // Try to include the newline, unless it's an operator
		    // that works on lines only.
		    यदि (*p_sel != 'o'
			    && !op_on_पंक्तियाँ(oap->op_type)
			    && oap->end.lnum < curbuf->b_ml.ml_पंक्ति_गिनती)
		    {
			++oap->end.lnum;
			oap->end.col = 0;
			oap->end.coladd = 0;
			++oap->पंक्ति_गिनती;
		    }
		}
	    }

	    redo_VIsual_busy = FALSE;

	    // Switch Visual off now, so screen updating does
	    // not show inverted text when the screen is redrawn.
	    // With OP_YANK and sometimes with OP_COLON and OP_FILTER there is
	    // no screen redraw, so it is done here to remove the inverted
	    // part.
	    यदि (!gui_yank)
	    {
		VIsual_active = FALSE;
		setmouse();
		mouse_dragging = 0;
		may_clear_cmdline();
		यदि ((oap->op_type == OP_YANK
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
	यदि (has_mbyte && oap->inclusive)
	{
	    पूर्णाङ्क		l;

	    l = (*mb_ptr2len)(ml_get_pos(&oap->end));
	    यदि (l > 1)
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
	यदि (oap->is_VIsual && (oap->empty || !curbuf->b_p_ma
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
	यदि (	   oap->motion_type == MCHAR
		&& oap->inclusive == FALSE
		&& !(cap->retval & CA_NO_ADJ_OP_END)
		&& oap->end.col == 0
		&& (!oap->is_VIsual || *p_sel == 'o')
		&& !oap->block_mode
		&& oap->पंक्ति_गिनती > 1)
	{
	    oap->end_adjusted = TRUE;	    // remember that we did this
	    --oap->पंक्ति_गिनती;
	    --oap->end.lnum;
	    यदि (inindent(0))
		oap->motion_type = MLINE;
	    अन्यथा
	    {
		oap->end.col = ml_get_len(oap->end.lnum);
		यदि (oap->end.col)
		{
		    --oap->end.col;
		    oap->inclusive = TRUE;
		}
	    }
	}
	अन्यथा
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
	    यदि (oap->पंक्ति_गिनती < 2)
		oap->पंक्ति_गिनती = 2;
	    यदि (curwin->w_cursor.lnum + oap->पंक्ति_गिनती - 1 >
						   curbuf->b_ml.ml_पंक्ति_गिनती)
		beep_flush();
	    अन्यथा
	    {
		(शून्य)do_join(oap->पंक्ति_गिनती, oap->op_type == OP_JOIN,
							    TRUE, TRUE, TRUE);
		auto_format(FALSE, TRUE);
	    }
	    भङ्ग;

	पक्ष OP_DELETE:
	    VIsual_reselect = FALSE;	    // don't reselect now
	    यदि (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    अन्यथा
	    {
		(शून्य)op_delete(oap);
		// save cursor line for undo if it wasn't saved yet
		यदि (oap->motion_type == MLINE && has_format_option(FO_AUTO)
						      && u_save_cursor() == OK)
		    auto_format(FALSE, TRUE);
	    }
	    भङ्ग;

	पक्ष OP_YANK:
	    यदि (empty_region_error)
	    {
		यदि (!gui_yank)
		{
		    vim_beep(BO_OPER);
		    CancelRedo();
		}
	    }
	    अन्यथा
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
	    यदि (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    अन्यथा
	    {
		// This is a new edit command, not a restart.  Need to
		// remember it to make 'insertmode' work with mappings for
		// Visual mode.  But do this only once and not when typed and
		// 'insertmode' isn't set.
		यदि (p_im || !कुंजी_Typed)
		    restart_edit_save = restart_edit;
		अन्यथा
		    restart_edit_save = 0;
		restart_edit = 0;
#ifdef FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#endif
		// trigger TextChangedI
		curbuf->b_last_changedtick_i = CHANGEDTICK(curbuf);

		यदि (op_change(oap))	// will call edit()
		    cap->retval |= CA_COMMAND_BUSY;
		यदि (restart_edit == 0)
		    restart_edit = restart_edit_save;
	    }
	    भङ्ग;

	पक्ष OP_FILTER:
	    यदि (vim_strchr(p_cpo, CPO_FILTER) != रिक्त)
		AppendToRedobuff((char_u *)"!\r");  // use any last used !cmd
	    अन्यथा
		bangredo = TRUE;    // do_bang() will put cmd in redo buffer
	    // FALLTHROUGH

	पक्ष OP_INDENT:
	पक्ष OP_COLON:

	    // If 'equalprg' is empty, do the indenting internally.
	    यदि (oap->op_type == OP_INDENT && *get_equalprg() == NUL)
	    {
		यदि (curbuf->b_p_lisp)
		{
#ifdef FEAT_EVAL
		    यदि (use_indentexpr_for_lisp())
			op_reindent(oap, get_व्यंजक_indent);
		    अन्यथा
#endif
			op_reindent(oap, get_lisp_indent);
		    भङ्ग;
		}
		op_reindent(oap,
#ifdef FEAT_EVAL
			*curbuf->b_p_inde != NUL ? get_व्यंजक_indent :
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
	    यदि (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    अन्यथा
		op_tilde(oap);
	    check_cursor_col();
	    भङ्ग;

	पक्ष OP_FORMAT:
#यदि defined(FEAT_EVAL)
	    यदि (*curbuf->b_p_fex != NUL)
		op_formatexpr(oap);	// use expression
	    अन्यथा
#endif
	    {
		यदि (*p_fp != NUL || *curbuf->b_p_fp != NUL)
		    op_colon(oap);		// use external command
		अन्यथा
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

	पक्ष OP_जोड़ो:
	पक्ष OP_APPEND:
	    VIsual_reselect = FALSE;	// don't reselect now
	    यदि (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    अन्यथा
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

		op_जोड़ो(oap, cap->count1);
#ifdef FEAT_LINEBREAK
		// Reset linebreak, so that formatting works correctly.
		(शून्य)reset_lbr();
#endif

		// TODO: when inserting in several lines, should format all
		// the lines.
		auto_format(FALSE, TRUE);

		यदि (restart_edit == 0)
		    restart_edit = restart_edit_save;
		अन्यथा
		    cap->retval |= CA_COMMAND_BUSY;
	    }
	    भङ्ग;

	पक्ष OP_REPLACE:
	    VIsual_reselect = FALSE;	// don't reselect now
	    यदि (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    अन्यथा
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
	    यदि (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    अन्यथा
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
	यदि (!gui_yank)
	{
	    // if 'sol' not set, go back to old column for some commands
	    यदि (!p_sol && oap->motion_type == MLINE && !oap->end_adjusted
		    && (oap->op_type == OP_LSHIFT || oap->op_type == OP_RSHIFT
						|| oap->op_type == OP_DELETE))
	    {
#ifdef FEAT_LINEBREAK
		(शून्य)reset_lbr();
#endif
		coladvance(curwin->w_curswant = old_col);
	    }
	}
	अन्यथा
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
    पूर्णाङ्क	len = curbuf->b_ml.ml_पंक्ति_len;

    // safety check
    यदि (lp.col >= len)
	lp.col = (len > 1 ? len - 2 : 0);
    *(p + lp.col) = c;
}
