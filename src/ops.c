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

स्थिर शून्य shift_block(oparg_T *oap, पूर्णांक amount);
स्थिर शून्य	mb_adjust_opend(oparg_T *oap);
स्थिर पूर्णांक	do_addsub(पूर्णांक op_type, pos_T *pos, पूर्णांक length, linenr_T Prenum1);
स्थिर शून्य	pbyte(pos_T lp, पूर्णांक c);
#define PBYTE(lp, c) pbyte(lp, c)


// Flags for third item in "opchars".
#define OPF_ओळी  1	// operator always works on lines
#define OPF_CHANGE 2	// operator changes text

/*
 * The names of operators.
 * IMPORTANT: Index must correspond with defines in vim.h!!!
 * The third field holds OPF_ flags.
 */
स्थिर स्थिरांक अक्षर opchars[][3] =
{
    {NUL, NUL, 0},			// OP_NOP
    {'d', NUL, OPF_CHANGE},		// OP_DELETE
    {'y', NUL, 0},			// OP_YANK
    {'c', NUL, OPF_CHANGE},		// OP_CHANGE
    {'<', NUL, OPF_ओळी | OPF_CHANGE},	// OP_LSHIFT
    {'>', NUL, OPF_ओळी | OPF_CHANGE},	// OP_RSHIFT
    {'!', NUL, OPF_ओळी | OPF_CHANGE},	// OP_FILTER
    {'g', '~', OPF_CHANGE},		// OP_TILDE
    {'=', NUL, OPF_ओळी | OPF_CHANGE},	// OP_INDENT
    {'g', 'q', OPF_ओळी | OPF_CHANGE},	// OP_FORMAT
    {':', NUL, OPF_ओळी},		// OP_COLON
    {'g', 'U', OPF_CHANGE},		// OP_UPPER
    {'g', 'u', OPF_CHANGE},		// OP_LOWER
    {'J', NUL, OPF_ओळी | OPF_CHANGE},	// DO_JOIN
    {'g', 'J', OPF_ओळी | OPF_CHANGE},	// DO_JOIN_NS
    {'g', '?', OPF_CHANGE},		// OP_ROT13
    {'r', NUL, OPF_CHANGE},		// OP_REPLACE
    {'I', NUL, OPF_CHANGE},		// OP_INSERT
    {'A', NUL, OPF_CHANGE},		// OP_APPEND
    {'z', 'f', OPF_ओळी},		// OP_FOLD
    {'z', 'o', OPF_ओळी},		// OP_FOLDOPEN
    {'z', 'O', OPF_ओळी},		// OP_FOLDOPENREC
    {'z', 'c', OPF_ओळी},		// OP_FOLDCLOSE
    {'z', 'C', OPF_ओळी},		// OP_FOLDCLOSEREC
    {'z', 'd', OPF_ओळी},		// OP_FOLDDEL
    {'z', 'D', OPF_ओळी},		// OP_FOLDDELREC
    {'g', 'w', OPF_ओळी | OPF_CHANGE},	// OP_FORMAT2
    {'g', '@', OPF_CHANGE},		// OP_FUNCTION
    {Ctrl_A, NUL, OPF_CHANGE},		// OP_NR_ADD
    {Ctrl_X, NUL, OPF_CHANGE},		// OP_NR_SUB
};

/*
 * Translate a command name into an operator type.
 * Must only be called with a valid operator name!
 */
    पूर्णांक
get_op_type(पूर्णांक char1, पूर्णांक char2)
{
    पूर्णांक		i;

    जर (char1 == 'r')		// ignore second character
	परत OP_REPLACE;
    जर (char1 == '~')		// when tilde is an operator
	परत OP_TILDE;
    जर (char1 == 'g' && char2 == Ctrl_A)	// add
	परत OP_NR_ADD;
    जर (char1 == 'g' && char2 == Ctrl_X)	// subtract
	परत OP_NR_SUB;
    जर (char1 == 'z' && char2 == 'y')	// OP_YANK
	परत OP_YANK;
    साठी (i = 0; ; ++i)
    {
	जर (opchars[i][0] == char1 && opchars[i][1] == char2)
	    थांबा;
	जर (i == (पूर्णांक)ARRAY_LENGTH(opchars) - 1)
	{
	    internal_error("get_op_type()");
	    थांबा;
	}
    }
    परत i;
}

/*
 * Return TRUE if operator "op" always works on whole lines.
 */
    स्थिर पूर्णांक
op_on_ओळी(पूर्णांक op)
{
    परत opchars[op][2] & OPF_ओळी;
}

#जर defined(FEAT_JOB_CHANNEL)
/*
 * Return TRUE if operator "op" changes text.
 */
    पूर्णांक
op_is_change(पूर्णांक op)
{
    परत opchars[op][2] & OPF_CHANGE;
}
#endif

/*
 * Get first operator command character.
 * Returns 'g' or 'z' if there is another command character.
 */
    पूर्णांक
get_op_char(पूर्णांक optype)
{
    परत opchars[optype][0];
}

/*
 * Get second operator command character.
 */
    पूर्णांक
get_extra_op_char(पूर्णांक optype)
{
    परत opchars[optype][1];
}

/*
 * op_shift - handle a shift operation
 */
    शून्य
op_shift(oparg_T *oap, पूर्णांक curs_शीर्ष, पूर्णांक amount)
{
    दीर्घ	    i;
    पूर्णांक		    first_char;
    पूर्णांक		    block_col = 0;

    जर (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	परत;

    जर (oap->block_mode)
	block_col = curwin->w_cursor.col;

    साठी (i = oap->ओळ_मोजणी; --i >= 0; )
    {
	first_char = *ml_get_curline();
	जर (first_char == NUL)				// empty line
	    curwin->w_cursor.col = 0;
	अन्यथा जर (oap->block_mode)
	    shift_block(oap, amount);
	अन्यथा
	    // Move the line right if it doesn't start with '#', 'smartindent'
	    // isn't set or 'cindent' isn't set or '#' isn't in 'cino'.
	    जर (first_char != '#' || !preprocs_डावा())
		shift_ओळ(oap->op_type == OP_LSHIFT, p_sr, amount, FALSE);
	++curwin->w_cursor.lnum;
    }

    changed_ओळी(oap->start.lnum, 0, oap->end.lnum + 1, 0L);
    जर (oap->block_mode)
    {
	curwin->w_cursor.lnum = oap->start.lnum;
	curwin->w_cursor.col = block_col;
    }
    अन्यथा जर (curs_शीर्ष)	    // put cursor on first line, for ">>"
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


    जर (oap->ओळ_मोजणी > p_report)
    {
	अक्षर	    *op;
	अक्षर	    *संदेश_ओळ_single;
	अक्षर	    *संदेश_ओळ_plural;

	जर (oap->op_type == OP_RSHIFT)
	    op = ">";
	अन्यथा
	    op = "<";
	संदेश_ओळ_single = NGETTEXT("%ld line %sed %d time",
					     "%ld line %sed %d times", amount);
	संदेश_ओळ_plural = NGETTEXT("%ld lines %sed %d time",
					    "%ld lines %sed %d times", amount);
	vim_snprintf((अक्षर *)IObuff, IOSIZE,
		NGETTEXT(संदेश_ओळ_single, संदेश_ओळ_plural, oap->ओळ_मोजणी),
		oap->ओळ_मोजणी, op, amount);
	संदेश_attr_keep((अक्षर *)IObuff, 0, TRUE);
    }

    जर ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end.lnum = oap->end.lnum;
	curbuf->b_op_end.col = ml_get_len(oap->end.lnum);
	जर (curbuf->b_op_end.col > 0)
	    --curbuf->b_op_end.col;
    }
}

#ifdef FEAT_VARTABS
/*
 * Return the tabstop width at the index of the variable tabstop array.  If an
 * index greater than the length of the array is given, the last tabstop width
 * in the array is returned.
 */
    स्थिर पूर्णांक
get_vts(पूर्णांक *vts_array, पूर्णांक index)
{
    पूर्णांक	ts;

    जर (index < 1)
	ts = 0;
    अन्यथा जर (index <= vts_array[0])
	ts = vts_array[index];
    अन्यथा
	ts = vts_array[vts_array[0]];

    परत ts;
}

/*
 * Return the sum of all the tabstops through the index-th.
 */
    स्थिर पूर्णांक
get_vts_sum(पूर्णांक *vts_array, पूर्णांक index)
{
    पूर्णांक	sum = 0;
    पूर्णांक	i;

    // Perform the summation for indices within the actual array.
    साठी (i = 1; i <= index && i <= vts_array[0]; i++)
	sum += vts_array[i];

    // Add tabstops whose indices exceed the actual array.
    जर (i <= index)
	sum += vts_array[vts_array[0]] * (index - vts_array[0]);

    परत sum;
}
#endif

    स्थिर vimlong_T
get_new_sw_indent(
    पूर्णांक		डावा,		// TRUE if shift is to the left
    पूर्णांक		round,		// TRUE if new indent is to be to a tabstop
    vimlong_T	amount,		// Number of shifts
    vimlong_T	sw_val)
{
    vimlong_T	मोजणी = get_indent();
    vimlong_T	i, j;

    जर (round)			// round off indent
    {
	i = trim_to_int(मोजणी) / sw_val;	// number of 'shiftwidth' rounded down
	j = trim_to_int(मोजणी) % sw_val;	// extra spaces
	जर (j && डावा)		// first remove extra spaces
	    --amount;
	जर (डावा)
	{
	    i -= amount;
	    जर (i < 0)
		i = 0;
	}
	अन्यथा
	    i += amount;
	मोजणी = i * sw_val;
    }
    अन्यथा			// original vi indent
    {
	जर (डावा)
	{
	    मोजणी -= sw_val * amount;
	    जर (मोजणी < 0)
		मोजणी = 0;
	}
	अन्यथा
	    मोजणी += sw_val * amount;
    }

    परत मोजणी;
}

#ifdef FEAT_VARTABS
    स्थिर vimlong_T
get_new_vts_indent(
    पूर्णांक	डावा,			// TRUE if shift is to the left
    पूर्णांक	round,			// TRUE if new indent is to be to a tabstop
    पूर्णांक	amount,			// Number of shifts
    पूर्णांक	*vts_array)
{
    vimlong_T	indent = get_indent();
    पूर्णांक		vtsi = 0;
    पूर्णांक		vts_indent = 0;
    पूर्णांक		ts = 0;		// Silence uninitialized variable warning.
    पूर्णांक		offset;		// Extra indent spaces to the right of the
				// tabstop

    // Find the tabstop at or to the left of the current indent.
    पर्यंत (vts_indent <= indent)
    {
	vtsi++;
	ts = get_vts(vts_array, vtsi);
	vts_indent += ts;
    }
    vts_indent -= ts;
    vtsi--;

    offset = indent - vts_indent;

    जर (round)
    {
	जर (डावा)
	{
	    जर (offset == 0)
		indent = get_vts_sum(vts_array, vtsi - amount);
	    अन्यथा
		indent = get_vts_sum(vts_array, vtsi - (amount - 1));
	}
	अन्यथा
	    indent = get_vts_sum(vts_array, vtsi + amount);
    }
    अन्यथा
    {
	जर (डावा)
	{
	    जर (amount > vtsi)
		indent = 0;
	    अन्यथा
		indent = get_vts_sum(vts_array, vtsi - amount) + offset;
	}
	अन्यथा
	    indent = get_vts_sum(vts_array, vtsi + amount) + offset;
    }

    परत indent;
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
shift_ओळ(
    पूर्णांक	डावा,			// TRUE if shift is to the left
    पूर्णांक	round,			// TRUE if new indent is to be to a tabstop
    पूर्णांक	amount,			// Number of shifts
    पूर्णांक	call_changed_bytes)	// call changed_bytes()
{
    vimlong_T	मोजणी;
    दीर्घ	sw_val = curbuf->b_p_sw;
    दीर्घ	ts_val = curbuf->b_p_ts;
#ifdef FEAT_VARTABS
    पूर्णांक		*vts_array = curbuf->b_p_vts_array;
#endif

    जर (sw_val != 0)
	// 'shiftwidth' is not zero; use it as the shift size.
	मोजणी = get_new_sw_indent(डावा, round, amount, sw_val);
    अन्यथा
#ifdef FEAT_VARTABS
	जर ((vts_array == रिक्त) || (vts_array[0] == 0))
#endif
    {
	// 'shiftwidth is zero and 'vartabstop' is empty; use 'tabstop' as the
	// shift size.
	मोजणी = get_new_sw_indent(डावा, round, amount, ts_val);
    }
#ifdef FEAT_VARTABS
    अन्यथा
    {
	// 'shiftwidth is zero and 'vartabstop' is defined; use 'vartabstop'
	// to determine the new indent.
	मोजणी = get_new_vts_indent(डावा, round, amount, vts_array);
    }
#endif

    // Set new indent
    जर (State & VREPLACE_FLAG)
	change_indent(INDENT_SET, trim_to_int(मोजणी), FALSE, NUL, call_changed_bytes);
    अन्यथा
	(शून्य)set_indent(trim_to_int(मोजणी), call_changed_bytes ? SIN_CHANGED : 0);
}

/*
 * Shift one line of the current block one shiftwidth right or left.
 * Leaves cursor on first character in block.
 */
    स्थिर शून्य
shift_block(oparg_T *oap, पूर्णांक amount)
{
    पूर्णांक			डावा = (oap->op_type == OP_LSHIFT);
    पूर्णांक			oldstate = State;
    पूर्णांक			total;
    char_u		*newp, *oldp;
    माप_प्रकार		newlen, oldlen;
    पूर्णांक			oldcol = curwin->w_cursor.col;
    पूर्णांक			sw_val = (पूर्णांक)get_sw_मूल्य_indent(curbuf, डावा);
    पूर्णांक			ts_val = (पूर्णांक)curbuf->b_p_ts;
    संरचना block_def	bd;
    पूर्णांक			incr;
    colnr_T		ws_vcol;
    पूर्णांक			added;
    माप_प्रकार		new_ओळ_len;	// the length of the line after the
					// block shift
#ifdef FEAT_RIGHTLEFT
    पूर्णांक			old_p_ri = p_ri;

    p_ri = 0;			// don't want revins in indent
#endif

    State = MODE_घाला;	// don't want MODE_REPLACE for State
    block_prep(oap, &bd, curwin->w_cursor.lnum, TRUE);
    जर (bd.is_short)
	परत;

    // total is number of screen columns to be inserted/removed
    total = (पूर्णांक)((अचिन्हित)amount * (अचिन्हित)sw_val);
    जर ((total / sw_val) != amount)
	परत; // multiplication overflow

    oldp = ml_get_curline();
    oldlen = ml_get_curline_len();

    जर (!डावा)
    {
	पूर्णांक		tabs = 0, spaces = 0;
	chartabsize_T	cts;

	/*
	 *  1. Get start vcol
	 *  2. Total ws vcols
	 *  3. Divvy into TABs & spp
	 *  4. Construct new string
	 */
	total += bd.pre_whitesp; // all virtual WS up to & incl a split TAB
	ws_vcol = bd.start_vcol - bd.pre_whitesp;
	जर (bd.startspaces)
	{
	    जर (has_mbyte)
	    {
		जर ((*mb_ptr2len)(bd.textstart) == 1)
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
	साठी ( ; VIM_ISWHITE(*cts.cts_ptr); )
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
	जर (!curbuf->b_p_et)
	    tabstop_fromto(ws_vcol, ws_vcol + total,
				ts_val, curbuf->b_p_vts_array, &tabs, &spaces);
	अन्यथा
	    spaces = total;
#अन्यथा
	जर (!curbuf->b_p_et)
	    tabs = ((ws_vcol % ts_val) + total) / ts_val; // number of tabs
	जर (tabs > 0)
	    spaces = ((ws_vcol % ts_val) + total) % ts_val; // number of spp
	अन्यथा
	    spaces = total;
#endif
	// if we're splitting a TAB, allow for it
	bd.textcol -= bd.pre_whitesp_c - (bd.startspaces != 0);

	new_ओळ_len = bd.textcol + tabs + spaces + (oldlen - (bd.textstart - oldp));
	newp = alloc(new_ओळ_len + 1);
	जर (newp == रिक्त)
	    परत;
	mch_memmove(newp, oldp, (माप_प्रकार)bd.textcol);
	newlen = bd.textcol;
	vim_memset(newp + newlen, TAB, (माप_प्रकार)tabs);
	newlen += tabs;
	vim_memset(newp + newlen, ' ', (माप_प्रकार)spaces);
	STRCPY(newp + newlen + spaces, bd.textstart);
    }
    अन्यथा //डावा
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
	जर (bd.startspaces)
	    MB_PTR_ADV(non_white);

	// The character's column is in "bd.start_vcol".
	non_white_col = bd.start_vcol;

	init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum,
				   non_white_col, bd.textstart, non_white);
	पर्यंत (VIM_ISWHITE(*cts.cts_ptr))
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
	जर (bd.startspaces)
	    verbatim_copy_width -= bd.start_char_vcols;
	init_chartabsize_arg(&cts, curwin, 0, verbatim_copy_width,
					     bd.textstart, verbatim_copy_end);
	पर्यंत (cts.cts_vcol < destination_col)
	{
	    incr = lbr_chartabsize(&cts);
	    जर (cts.cts_vcol + incr > destination_col)
		थांबा;
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
	new_ओळ_len = fixedlen + fill + (oldlen - (non_white - oldp));

	newp = alloc(new_ओळ_len + 1);
	जर (newp == रिक्त)
	    परत;
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
    जर (new_ओळ_len >= oldlen)
	added = (पूर्णांक)(new_ओळ_len - oldlen);
    अन्यथा
	added = 0 - (पूर्णांक)(oldlen - new_ओळ_len);
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
block_घाला(
    oparg_T		*oap,
    char_u		*s,
    माप_प्रकार		slen,
    पूर्णांक			b_घाला,
    संरचना block_def	*bdp)
{
    पूर्णांक		ts_val;
    पूर्णांक		मोजणी = 0;	// extra spaces to replace a cut TAB
    पूर्णांक		spaces = 0;	// non-zero if cutting a TAB
    colnr_T	offset;		// pointer along new line
    colnr_T	startcol;	// column where insert starts
    char_u	*newp, *oldp;	// new, old lines
    linenr_T	lnum;		// loop var
    पूर्णांक		oldstate = State;

    State = MODE_घाला;	// don't want MODE_REPLACE for State

    साठी (lnum = oap->start.lnum + 1; lnum <= oap->end.lnum; lnum++)
    {
	block_prep(oap, bdp, lnum, TRUE);
	जर (bdp->is_short && b_घाला)
	    सुरूठेवा;	// OP_INSERT, line ends before block start

	oldp = ml_get(lnum);

	जर (b_घाला)
	{
	    ts_val = bdp->start_char_vcols;
	    spaces = bdp->startspaces;
	    जर (spaces != 0)
		मोजणी = ts_val - 1; // we're cutting a TAB
	    offset = bdp->textcol;
	}
	अन्यथा // append
	{
	    ts_val = bdp->end_char_vcols;
	    जर (!bdp->is_short) // spaces = padding after block
	    {
		spaces = (bdp->endspaces ? ts_val - bdp->endspaces : 0);
		जर (spaces != 0)
		    मोजणी = ts_val - 1; // we're cutting a TAB
		offset = bdp->textcol + bdp->textlen - (spaces != 0);
	    }
	    अन्यथा // spaces = padding to block edge
	    {
		// if $ used, just append to EOL (ie spaces==0)
		जर (!bdp->is_MAX)
		    spaces = (oap->end_vcol - bdp->end_vcol) + 1;
		मोजणी = spaces;
		offset = bdp->textcol + bdp->textlen;
	    }
	}

	जर (has_mbyte && spaces > 0)
	    // avoid copying part of a multi-byte character
	    offset -= (*mb_head_off)(oldp, oldp + offset);

	जर (spaces < 0)  // can happen when the cursor was moved
	    spaces = 0;

	// Make sure the allocated size matches what is actually copied below.
	newp = alloc(ml_get_len(lnum) + spaces + slen
		    + (spaces > 0 && !bdp->is_short ? ts_val - spaces : 0)
								  + मोजणी + 1);
	जर (newp == रिक्त)
	    सुरूठेवा;

	// copy up to shifted part
	mch_memmove(newp, oldp, (माप_प्रकार)offset);
	oldp += offset;

	// insert pre-padding
	vim_memset(newp + offset, ' ', (माप_प्रकार)spaces);
	startcol = offset + spaces;

	// copy the new text
	mch_memmove(newp + startcol, s, slen);
	offset += (पूर्णांक)slen;

	जर (spaces > 0 && !bdp->is_short)
	{
	    जर (*oldp == TAB)
	    {
		// insert post-padding
		vim_memset(newp + offset + spaces, ' ',
						    (माप_प्रकार)(ts_val - spaces));
		// we're splitting a TAB, don't copy it
		oldp++;
		// We allowed for that TAB, remember this now
		मोजणी++;
	    }
	    अन्यथा
		// Not a TAB, no extra spaces
		मोजणी = spaces;
	}

	जर (spaces > 0)
	    offset += मोजणी;
	STRCPY(newp + offset, oldp);

	ml_replace(lnum, newp, FALSE);

	जर (b_घाला)
	    // correct any text properties
	    inserted_bytes(lnum, startcol, (पूर्णांक)slen);

	जर (lnum == oap->end.lnum)
	{
	    // Set "']" mark to the end of the block instead of the end of
	    // the insert in the first line.
	    curbuf->b_op_end.lnum = oap->end.lnum;
	    curbuf->b_op_end.col = offset;
	    जर (curbuf->b_visual.vi_end.coladd)
	    {
		curbuf->b_visual.vi_end.col += curbuf->b_visual.vi_end.coladd;
		curbuf->b_visual.vi_end.coladd = 0;
	    }
	}
    } // for all lnum

    changed_ओळी(oap->start.lnum + 1, 0, oap->end.lnum + 1, 0L);

    State = oldstate;
}

/*
 * Handle a delete operation.
 *
 * Return FAIL if undo failed, OK otherwise.
 */
    पूर्णांक
op_delete(oparg_T *oap)
{
    पूर्णांक			n;
    linenr_T		lnum;
    char_u		*ptr;
    char_u		*newp, *oldp;
    संरचना block_def	bd;
    linenr_T		old_lcount = curbuf->b_ml.ml_ओळ_मोजणी;
    पूर्णांक			did_yank = FALSE;

    जर (curbuf->b_ml.ml_flags & ML_EMPTY)	    // nothing to do
	परत OK;

    // Nothing to delete, return here.	Do prepare undo, for op_change().
    जर (oap->empty)
	परत u_save_cursor();

    जर (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	परत FAIL;
    }

    जर (VIsual_select && oap->is_VIsual)
	// use register given with CTRL_R, defaults to zero
	oap->regname = VIsual_select_reg;

#ifdef HAVE_CLIPMETHOD
    adjust_clip_reg(&oap->regname);
#endif

    जर (has_mbyte)
	mb_adjust_opend(oap);

    /*
     * Imitate the strange Vi behaviour: If the delete spans more than one
     * line and motion_type == MCHAR and the result is a blank line, make the
     * delete linewise.  Don't do this for the change command or Visual mode.
     */
    जर (       oap->motion_type == MCHAR
	    && !oap->is_VIsual
	    && !oap->block_mode
	    && oap->ओळ_मोजणी > 1
	    && oap->motion_force == NUL
	    && (vim_strchr(p_cpo, CPO_WORD) != रिक्त)
	    && oap->op_type == OP_DELETE)
    {
	ptr = ml_get(oap->end.lnum) + oap->end.col;
	जर (*ptr != NUL)
	    ptr += oap->inclusive;
	ptr = skipwhite(ptr);
	जर (*ptr == NUL && inindent(0))
	    oap->motion_type = MLINE;
    }

    /*
     * Check for trying to delete (e.g. "D") in an empty line.
     * Note: For the change operator it is ok.
     */
    जर (       oap->motion_type == MCHAR
	    && oap->ओळ_मोजणी == 1
	    && oap->op_type == OP_DELETE
	    && *ml_get(oap->start.lnum) == NUL)
    {
	/*
	 * It's an error to operate on an empty region, when 'E' included in
	 * 'cpoptions' (Vi compatible).
	 */
	जर (virtual_op)
	    // Virtual editing: Nothing gets deleted, but we set the '[ and ']
	    // marks as if it happened.
	    जा setmarks;
	जर (vim_strchr(p_cpo, CPO_EMPTYREGION) != रिक्त)
	    beep_flush();
	परत OK;
    }

    /*
     * Do a yank of whatever we're about to delete.
     * If a yank register was specified, put the deleted text into that
     * register.  For the black hole register '_' don't yank anything.
     */
    जर (oap->regname != '_')
    {
#ifdef FEAT_CLIPBOARD_PROVIDER
	inc_clip_provider();
#endif
	जर (oap->regname != 0)
	{
	    // check for read-only register
	    जर (!valid_yank_reg(oap->regname, TRUE))
	    {
		beep_flush();
#ifdef FEAT_CLIPBOARD_PROVIDER
		dec_clip_provider();
#endif
		परत OK;
	    }
	    get_yank_register(oap->regname, TRUE); // yank into specif'd reg.
	    जर (op_yank(oap, TRUE, FALSE) == OK)   // yank without message
		did_yank = TRUE;
	}
	अन्यथा
	    reset_y_append(); // not appending to unnamed register

	/*
	 * Put deleted text into register 1 and shift number registers if the
	 * delete contains a line break, or when using a specific operator (Vi
	 * compatible)
	 */
	जर (oap->motion_type == MLINE || oap->ओळ_मोजणी > 1
							   || oap->use_reg_one)
	{
	    shift_delete_registers();
	    जर (op_yank(oap, TRUE, FALSE) == OK)
		did_yank = TRUE;
	}

	// Yank into small delete register when no named register specified
	// and the delete is within one line.
	जर ((
#ifdef HAVE_CLIPMETHOD
	    ((clip_unnamed & CLIP_UNNAMED) && oap->regname == '*') ||
	    ((clip_unnamed & CLIP_UNNAMED_PLUS) && oap->regname == '+') ||
#endif
	    oap->regname == 0) && oap->motion_type != MLINE
						      && oap->ओळ_मोजणी == 1)
	{
	    oap->regname = '-';
	    get_yank_register(oap->regname, TRUE);
	    जर (op_yank(oap, TRUE, FALSE) == OK)
		did_yank = TRUE;
	    oap->regname = 0;
	}

	/*
	 * If there's too much stuff to fit in the yank register, then get a
	 * confirmation before doing the delete. This is crude, but simple.
	 * And it avoids doing a delete of something we can't put back if we
	 * want.
	 */
	जर (!did_yank)
	{
	    पूर्णांक संदेश_silent_save = संदेश_silent;

	    संदेश_silent = 0;	// must display the prompt
	    n = ask_yesno((char_u *)_("cannot yank; delete anyway"), TRUE);
	    संदेश_silent = संदेश_silent_save;
	    जर (n != 'y')
	    {
		emsg(_(e_command_aborted));
#ifdef FEAT_CLIPBOARD_PROVIDER
		dec_clip_provider();
#endif
		परत FAIL;
	    }
	}

#जर defined(FEAT_EVAL)
	जर (did_yank && has_textyankpost())
	    yank_do_autocmd(oap, get_y_current());
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
    }

    /*
     * block mode delete
     */
    जर (oap->block_mode)
    {
	जर (u_save((linenr_T)(oap->start.lnum - 1),
			       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	    परत FAIL;

	साठी (lnum = curwin->w_cursor.lnum; lnum <= oap->end.lnum; ++lnum)
	{
	    block_prep(oap, &bd, lnum, TRUE);
	    जर (bd.textlen == 0)	// nothing to delete
		सुरूठेवा;

	    // Adjust cursor position for tab replaced by spaces and 'lbr'.
	    जर (lnum == curwin->w_cursor.lnum)
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
	    जर (newp == रिक्त)
		सुरूठेवा;
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
	    जर (curbuf->b_has_textprop && n != 0)
		adjust_prop_columns(lnum, bd.textcol, -n, 0);
#endif
	}

	check_cursor_col();
	changed_ओळी(curwin->w_cursor.lnum, curwin->w_cursor.col,
						       oap->end.lnum + 1, 0L);
	oap->ओळ_मोजणी = 0;	    // no lines deleted
    }
    अन्यथा जर (oap->motion_type == MLINE)
    {
	जर (oap->op_type == OP_CHANGE)
	{
	    // Delete the lines except the first one.  Temporarily move the
	    // cursor to the next line.  Save the current line number, if the
	    // last line is deleted it may be changed.
	    जर (oap->ओळ_मोजणी > 1)
	    {
		lnum = curwin->w_cursor.lnum;
		++curwin->w_cursor.lnum;
		del_ओळी((दीर्घ)(oap->ओळ_मोजणी - 1), TRUE);
		curwin->w_cursor.lnum = lnum;
	    }
	    जर (u_save_cursor() == FAIL)
		परत FAIL;
	    जर (curbuf->b_p_ai)		    // don't delete indent
	    {
		beginline(BL_WHITE);	    // cursor on first non-white
		did_ai = TRUE;		    // delete the indent when ESC hit
		ai_col = curwin->w_cursor.col;
	    }
	    अन्यथा
		beginline(0);		    // cursor in column 0
	    truncate_ओळ(FALSE);   // delete the rest of the line,
				    // leaving cursor past last char in line
	    जर (oap->ओळ_मोजणी > 1)
		u_clearline();	    // "U" command not possible after "2cc"
	}
	अन्यथा
	{
	    del_ओळी(oap->ओळ_मोजणी, TRUE);
	    beginline(BL_WHITE | BL_FIX);
	    u_clearline();	// "U" command not possible after "dd"
	}
    }
    अन्यथा
    {
	जर (virtual_op)
	{
	    पूर्णांक		endcol = 0;

	    // For virtualedit: break the tabs that are partly included.
	    जर (gchar_pos(&oap->start) == '\t')
	    {
		जर (u_save_cursor() == FAIL)	// save first line for undo
		    परत FAIL;
		जर (oap->ओळ_मोजणी == 1)
		    endcol = getviscol2(oap->end.col, oap->end.coladd);
		coladvance_force(getviscol2(oap->start.col, oap->start.coladd));
		oap->start = curwin->w_cursor;
		जर (oap->ओळ_मोजणी == 1)
		{
		    coladvance(endcol);
		    oap->end.col = curwin->w_cursor.col;
		    oap->end.coladd = curwin->w_cursor.coladd;
		    curwin->w_cursor = oap->start;
		}
	    }

	    // Break a tab only when it's included in the area.
	    जर (gchar_pos(&oap->end) == '\t'
				     && (पूर्णांक)oap->end.coladd < oap->inclusive)
	    {
		// save last line for undo
		जर (u_save((linenr_T)(oap->end.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
		    परत FAIL;
		curwin->w_cursor = oap->end;
		coladvance_force(getviscol2(oap->end.col, oap->end.coladd));
		oap->end = curwin->w_cursor;
		curwin->w_cursor = oap->start;
	    }
	    जर (has_mbyte)
		mb_adjust_opend(oap);
	}

	जर (oap->ओळ_मोजणी == 1)	// delete characters within one line
	{
	    जर (u_save_cursor() == FAIL)	// save line for undo
		परत FAIL;

	    // if 'cpoptions' contains '$', display '$' at end of change
	    जर (       vim_strchr(p_cpo, CPO_DOLLAR) != रिक्त
		    && oap->op_type == OP_CHANGE
		    && oap->end.lnum == curwin->w_cursor.lnum
		    && !oap->is_VIsual)
		display_dollar(oap->end.col - !oap->inclusive);

	    n = oap->end.col - oap->start.col + 1 - !oap->inclusive;

	    जर (virtual_op)
	    {
		// fix up things for virtualedit-delete:
		// break the tabs which are going to get in our way
		पूर्णांक		len = ml_get_curline_len();

		जर (oap->end.coladd != 0
			&& (पूर्णांक)oap->end.col >= len - 1
			&& !(oap->start.coladd && (पूर्णांक)oap->end.col >= len - 1))
		    n++;
		// Delete at least one char (e.g, when on a control char).
		जर (n == 0 && oap->start.coladd != oap->end.coladd)
		    n = 1;

		// When deleted a char in the line, reset coladd.
		जर (gchar_cursor() != NUL)
		    curwin->w_cursor.coladd = 0;
	    }
	    (शून्य)del_bytes((दीर्घ)n, !virtual_op,
			    oap->op_type == OP_DELETE && !oap->is_VIsual);
	}
	अन्यथा				// delete characters between lines
	{
	    pos_T   curpos;

	    // save deleted and changed lines for undo
	    जर (u_save((linenr_T)(curwin->w_cursor.lnum - 1),
		 (linenr_T)(curwin->w_cursor.lnum + oap->ओळ_मोजणी)) == FAIL)
		परत FAIL;

	    truncate_ओळ(TRUE);	// delete from cursor to end of line

	    curpos = curwin->w_cursor;	// remember curwin->w_cursor
	    ++curwin->w_cursor.lnum;
	    del_ओळी((दीर्घ)(oap->ओळ_मोजणी - 2), FALSE);

	    // delete from start of line until op_end
	    n = (oap->end.col + 1 - !oap->inclusive);
	    curwin->w_cursor.col = 0;
	    (शून्य)del_bytes((दीर्घ)n, !virtual_op,
			    oap->op_type == OP_DELETE && !oap->is_VIsual);
	    curwin->w_cursor = curpos;	// restore curwin->w_cursor
	    (शून्य)do_join(2, FALSE, FALSE, FALSE, FALSE);
	}
	जर (oap->op_type == OP_DELETE)
	    auto_format(FALSE, TRUE);
    }

    msgmore(curbuf->b_ml.ml_ओळ_मोजणी - old_lcount);

setmarks:
    जर ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	जर (oap->block_mode)
	{
	    curbuf->b_op_end.lnum = oap->end.lnum;
	    curbuf->b_op_end.col = oap->start.col;
	}
	अन्यथा
	    curbuf->b_op_end = oap->start;
	curbuf->b_op_start = oap->start;
    }

    परत OK;
}

/*
 * Adjust end of operating area for ending on a multi-byte character.
 * Used for deletion.
 */
    स्थिर शून्य
mb_adjust_opend(oparg_T *oap)
{
    char_u	*ओळ;
    char_u	*ptr;

    जर (!oap->inclusive)
	परत;

    ओळ = ml_get(oap->end.lnum);
    ptr = ओळ + oap->end.col;
    जर (*ptr != NUL)
    {
	ptr -= (*mb_head_off)(ओळ, ptr);
	ptr += (*mb_ptr2len)(ptr) - 1;
	oap->end.col = ptr - ओळ;
    }
}

/*
 * Replace the character under the cursor with "c".
 * This takes care of multi-byte characters.
 */
    स्थिर शून्य
replace_character(पूर्णांक c)
{
    पूर्णांक n = State;

    State = MODE_REPLACE;
    ins_char(c);
    State = n;
    // Backup to the replaced character.
    dec_cursor();
}

/*
 * Replace a whole area with one character.
 */
    पूर्णांक
op_replace(oparg_T *oap, पूर्णांक c)
{
    पूर्णांक			n, numc;
    पूर्णांक			अंक_chars;
    char_u		*newp, *oldp;
    माप_प्रकार		newlen, oldlen;
    संरचना block_def	bd;
    char_u		*after_p = रिक्त;
    पूर्णांक			had_ctrl_v_cr = FALSE;

    जर ((curbuf->b_ml.ml_flags & ML_EMPTY ) || oap->empty)
	परत OK;	    // nothing to do

    जर (c == REPLACE_CR_NCHAR)
    {
	had_ctrl_v_cr = TRUE;
	c = CAR;
    }
    अन्यथा जर (c == REPLACE_NL_NCHAR)
    {
	had_ctrl_v_cr = TRUE;
	c = NL;
    }

    जर (has_mbyte)
	mb_adjust_opend(oap);

    जर (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	परत FAIL;

    /*
     * block mode replace
     */
    जर (oap->block_mode)
    {
	bd.is_MAX = (curwin->w_curswant == MAXCOL);
	साठी ( ; curwin->w_cursor.lnum <= oap->end.lnum; ++curwin->w_cursor.lnum)
	{
	    curwin->w_cursor.col = 0;  // make sure cursor position is valid
	    block_prep(oap, &bd, curwin->w_cursor.lnum, TRUE);
	    जर (bd.textlen == 0 && (!virtual_op || bd.is_MAX))
		सुरूठेवा;	    // nothing to replace

	    // n == number of extra chars required
	    // If we split a TAB, it may be replaced by several characters.
	    // Thus the number of characters may increase!
	    // If the range starts in virtual space, count the initial
	    // coladd offset as part of "startspaces"
	    जर (virtual_op && bd.is_short && *bd.textstart == NUL)
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
	    जर (bd.is_short && (!virtual_op || bd.is_MAX))
		numc -= (oap->end_vcol - bd.end_vcol) + 1;

	    // A double-wide character can be replaced only up to half the
	    // times.
	    जर ((*mb_char2cells)(c) > 1)
	    {
		जर ((numc & 1) && !bd.is_short)
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
	    जर (newp == रिक्त)
		सुरूठेवा;
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
	    जर (had_ctrl_v_cr || (c != '\r' && c != '\n'))
	    {
		जर (has_mbyte)
		{
		    पर्यंत (--अंक_chars >= 0)
			newlen += (*mb_char2bytes)(c, newp + newlen);
		}
		अन्यथा
		{
		    vim_memset(newp + newlen, c, (माप_प्रकार)numc);
		    newlen += numc;
		}
		जर (!bd.is_short)
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
		जर (after_p != रिक्त)
		    STRCPY(after_p, oldp + bd.textcol + bd.textlen);
	    }

	    // replace the line
	    ml_replace(curwin->w_cursor.lnum, newp, FALSE);
	    जर (after_p != रिक्त)
	    {
		ml_append(curwin->w_cursor.lnum++, after_p, 0, FALSE);
		appended_ओळी_mark(curwin->w_cursor.lnum, 1L);
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
	जर (oap->motion_type == MLINE)
	{
	    oap->start.col = 0;
	    curwin->w_cursor.col = 0;
	    oap->end.col = ml_get_len(oap->end.lnum);
	    जर (oap->end.col)
		--oap->end.col;
	}
	अन्यथा जर (!oap->inclusive)
	    dec(&(oap->end));

	पर्यंत (LTOREQ_POS(curwin->w_cursor, oap->end))
	{
	    पूर्णांक done = FALSE;

	    n = gchar_cursor();
	    जर (n != NUL)
	    {
		पूर्णांक new_byte_len = (*mb_char2len)(c);
		पूर्णांक old_byte_len = mb_ptr2len(ml_get_cursor());

		जर (new_byte_len > 1 || old_byte_len > 1)
		{
		    // This is slow, but it handles replacing a single-byte
		    // with a multi-byte and the other way around.
		    जर (curwin->w_cursor.lnum == oap->end.lnum)
			oap->end.col += new_byte_len - old_byte_len;
		    replace_character(c);
		    done = TRUE;
		}
		अन्यथा
		{
		    जर (n == TAB)
		    {
			पूर्णांक end_vcol = 0;

			जर (curwin->w_cursor.lnum == oap->end.lnum)
			{
			    // oap->end has to be recalculated when
			    // the tab breaks
			    end_vcol = getviscol2(oap->end.col,
							     oap->end.coladd);
			}
			coladvance_force(getviscol());
			जर (curwin->w_cursor.lnum == oap->end.lnum)
			    getvpos(&oap->end, end_vcol);
		    }
		    // with "coladd" set may move to just after a TAB
		    जर (gchar_cursor() != NUL)
		    {
			PBYTE(curwin->w_cursor, c);
			done = TRUE;
		    }
		}
	    }
	    जर (!done && virtual_op && curwin->w_cursor.lnum == oap->end.lnum)
	    {
		पूर्णांक virtcols = oap->end.coladd;

		जर (curwin->w_cursor.lnum == oap->start.lnum
			&& oap->start.col == oap->end.col && oap->start.coladd)
		    virtcols -= oap->start.coladd;

		// oap->end has been trimmed so it's effectively inclusive;
		// as a result an extra +1 must be counted so we don't
		// trample the NUL byte.
		coladvance_force(getviscol2(oap->end.col, oap->end.coladd) + 1);
		curwin->w_cursor.col -= (virtcols + 1);
		साठी (; virtcols >= 0; virtcols--)
		{
		    जर ((*mb_char2len)(c) > 1)
		       replace_character(c);
		    अन्यथा
			PBYTE(curwin->w_cursor, c);
		    जर (inc(&curwin->w_cursor) == -1)
			थांबा;
		}
	    }

	    // Advance to next character, stop at the end of the file.
	    जर (inc_cursor() == -1)
		थांबा;
	}
    }

    curwin->w_cursor = oap->start;
    check_cursor();
    changed_ओळी(oap->start.lnum, oap->start.col, oap->end.lnum + 1, 0L);

    जर ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
    }

    परत OK;
}

स्थिर पूर्णांक swapchars(पूर्णांक op_type, pos_T *pos, पूर्णांक length);

/*
 * Handle the (non-standard vi) tilde operator.  Also for "gu", "gU" and "g?".
 */
    स्थिर शून्य
op_tilde(oparg_T *oap)
{
    pos_T		pos;
    संरचना block_def	bd;
    पूर्णांक			did_change = FALSE;

    जर (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	परत;

    pos = oap->start;
    जर (oap->block_mode)		    // Visual block mode
    {
	साठी (; pos.lnum <= oap->end.lnum; ++pos.lnum)
	{
	    पूर्णांक one_change;

	    block_prep(oap, &bd, pos.lnum, FALSE);
	    pos.col = bd.textcol;
	    one_change = swapchars(oap->op_type, &pos, bd.textlen);
	    did_change |= one_change;

#ifdef FEAT_NETBEANS_INTG
	    जर (netbeans_active() && one_change)
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
	जर (did_change)
	    changed_ओळी(oap->start.lnum, 0, oap->end.lnum + 1, 0L);
    }
    अन्यथा				    // not block mode
    {
	जर (oap->motion_type == MLINE)
	{
	    oap->start.col = 0;
	    pos.col = 0;
	    oap->end.col = ml_get_len(oap->end.lnum);
	    जर (oap->end.col)
		--oap->end.col;
	}
	अन्यथा जर (!oap->inclusive)
	    dec(&(oap->end));

	जर (pos.lnum == oap->end.lnum)
	    did_change = swapchars(oap->op_type, &pos,
						  oap->end.col - pos.col + 1);
	अन्यथा
	    साठी (;;)
	    {
		did_change |= swapchars(oap->op_type, &pos,
			pos.lnum == oap->end.lnum ? oap->end.col + 1
						  : ml_get_pos_len(&pos));
		जर (LTOREQ_POS(oap->end, pos) || inc(&pos) == -1)
		    थांबा;
	    }
	जर (did_change)
	{
	    changed_ओळी(oap->start.lnum, oap->start.col, oap->end.lnum + 1,
									  0L);
#ifdef FEAT_NETBEANS_INTG
	    जर (netbeans_active())
	    {
		char_u *ptr;
		पूर्णांक मोजणी;

		pos = oap->start;
		पर्यंत (pos.lnum < oap->end.lnum)
		{
		    ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		    मोजणी = ml_get_buf_len(curbuf, pos.lnum) - pos.col;
		    netbeans_removed(curbuf, pos.lnum, pos.col, (दीर्घ)मोजणी);
		    // get the line again, it may have been flushed
		    ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		    netbeans_inserted(curbuf, pos.lnum, pos.col,
							&ptr[pos.col], मोजणी);
		    pos.col = 0;
		    pos.lnum++;
		}
		मोजणी = oap->end.col - pos.col + 1;
		netbeans_removed(curbuf, pos.lnum, pos.col, (दीर्घ)मोजणी);
		// get the line again, it may have been flushed
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, pos.col,
							&ptr[pos.col], मोजणी);
	    }
#endif
	}
    }

    जर (!did_change && oap->is_VIsual)
	// No change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    जर ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set '[ and '] marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
    }

    जर (oap->ओळ_मोजणी > p_report)
	smsg(NGETTEXT("%ld line changed", "%ld lines changed",
					    oap->ओळ_मोजणी), oap->ओळ_मोजणी);
}

/*
 * Invoke swapchar() on "length" bytes at position "pos".
 * "pos" is advanced to just after the changed characters.
 * "length" is rounded up to include the whole last multi-byte character.
 * Also works correctly when the number of bytes changes.
 * Returns TRUE if some character was changed.
 */
    स्थिर पूर्णांक
swapchars(पूर्णांक op_type, pos_T *pos, पूर्णांक length)
{
    पूर्णांक todo;
    पूर्णांक	did_change = 0;

    साठी (todo = length; todo > 0; --todo)
    {
	जर (has_mbyte)
	{
	    पूर्णांक len = (*mb_ptr2len)(ml_get_pos(pos));

	    // we're counting bytes, not characters
	    जर (len > 0)
		todo -= len - 1;
	}
	did_change |= swapchar(op_type, pos);
	जर (inc(pos) == -1)    // at end of file
	    थांबा;
    }
    परत did_change;
}

/*
 * If op_type == OP_UPPER: make uppercase,
 * if op_type == OP_LOWER: make lowercase,
 * if op_type == OP_ROT13: do rot13 encoding,
 * else swap case of character at 'pos'
 * returns TRUE when something actually changed.
 */
    पूर्णांक
swapchar(पूर्णांक op_type, pos_T *pos)
{
    पूर्णांक	    c;
    पूर्णांक	    nc;

    c = gchar_pos(pos);

    // Only do rot13 encoding for ASCII characters.
    जर (c >= 0x80 && op_type == OP_ROT13)
	परत FALSE;

    // ~ is OP_NOP, g~ is OP_TILDE, gU is OP_UPPER
    जर ((op_type == OP_UPPER || op_type == OP_NOP || op_type == OP_TILDE)
	    && c == 0xdf
	    && (enc_latin1like || STRCMP(p_enc, "iso-8859-2") == 0))
    {
	pos_T   sp = curwin->w_cursor;

	// Special handling for lowercase German sharp s (ß): convert to uppercase (ẞ).
	curwin->w_cursor = *pos;
	del_char(FALSE);
	ins_char(0x1E9E);
	curwin->w_cursor = sp;
	परत TRUE;
    }

    जर (enc_dbcs != 0 && c >= 0x100)	// No lower/uppercase letter
	परत FALSE;
    nc = c;
    जर (MB_ISLOWER(c))
    {
	जर (op_type == OP_ROT13)
	    nc = ROT13(c, 'a');
	अन्यथा जर (op_type != OP_LOWER)
	    nc = MB_TOUPPER(c);
    }
    अन्यथा जर (MB_ISUPPER(c))
    {
	जर (op_type == OP_ROT13)
	    nc = ROT13(c, 'A');
	अन्यथा जर (op_type != OP_UPPER)
	    nc = MB_TOLOWER(c);
    }
    जर (nc != c)
    {
	जर (enc_utf8 && (c >= 0x80 || nc >= 0x80))
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
	परत TRUE;
    }
    परत FALSE;
}

/*
 * op_insert - Insert and append operators for Visual mode.
 */
    शून्य
op_घाला(oparg_T *oap, दीर्घ count1)
{
    दीर्घ		pre_textlen = 0;
    colnr_T		ind_pre_col = 0, ind_post_col;
    पूर्णांक			ind_pre_vcol = 0, ind_post_vcol = 0;
    संरचना block_def	bd;
    पूर्णांक			i;
    pos_T		t1;
    pos_T		start_घाला;

    // edit() changes this - record it for OP_APPEND
    bd.is_MAX = (curwin->w_curswant == MAXCOL);

    // vis block is still marked. Get rid of it now.
    curwin->w_cursor.lnum = oap->start.lnum;
    update_screen(UPD_INVERTED);

    जर (oap->block_mode)
    {
	// When 'virtualedit' is used, need to insert the extra spaces before
	// doing block_prep().  When only "block" is used, virtual edit is
	// already disabled, but still need it when calling
	// coladvance_force().
	// coladvance_force() uses get_ve_flags() to get the 'virtualedit'
	// state for the current window.  To override that state, we need to
	// set the window-local value of ve_flags rather than the global value.
	जर (curwin->w_cursor.coladd > 0)
	{
	    पूर्णांक		old_ve_flags = curwin->w_ve_flags;

	    जर (u_save_cursor() == FAIL)
		परत;

	    curwin->w_ve_flags = VE_ALL;
	    coladvance_force(oap->op_type == OP_APPEND
					   ? oap->end_vcol + 1 : getviscol());
	    जर (oap->op_type == OP_APPEND)
		--curwin->w_cursor.col;
	    curwin->w_ve_flags = old_ve_flags;
	}
	// Get the info about the block before entering the text
	block_prep(oap, &bd, oap->start.lnum, TRUE);
	// Get indent information
	ind_pre_col = (colnr_T)getwhitecols_curline();
	ind_pre_vcol = get_indent();
	pre_textlen = ml_get_len(oap->start.lnum) - bd.textcol;
	जर (oap->op_type == OP_APPEND)
	    pre_textlen -= bd.textlen;
    }

    जर (oap->op_type == OP_APPEND)
    {
	जर (oap->block_mode && curwin->w_cursor.coladd == 0)
	{
	    // Move the cursor to the character right of the block.
	    curwin->w_set_curswant = true;
	    पर्यंत (*ml_get_cursor() != NUL
		    && (curwin->w_cursor.col < bd.textcol + bd.textlen))
		++curwin->w_cursor.col;
	    जर (bd.is_short && !bd.is_MAX)
	    {
		// First line was too short, make it longer and adjust the
		// values in "bd".
		जर (u_save_cursor() == FAIL)
		    परत;
		साठी (i = 0; i < bd.endspaces; ++i)
		    ins_char(' ');
		bd.textlen += bd.endspaces;
	    }
	}
	अन्यथा
	{
	    curwin->w_cursor = oap->end;
	    check_cursor_col();

	    // Works just like an 'i'nsert on the next character.
	    जर (!LINEEMPTY(curwin->w_cursor.lnum)
		    && oap->start_vcol != oap->end_vcol)
		inc_cursor();
	}
    }

    t1 = oap->start;
    start_घाला = curwin->w_cursor;
    (शून्य)edit(NUL, FALSE, (linenr_T)count1);

    // When a tab was inserted, and the characters in front of the tab
    // have been converted to a tab as well, the column of the cursor
    // might have actually been reduced, so need to adjust here.
    जर (t1.lnum == curbuf->b_op_start_orig.lnum
	    && LT_POS(curbuf->b_op_start_orig, t1))
	oap->start = curbuf->b_op_start_orig;

    // If user has moved off this line, we don't know what to do, so do
    // nothing.
    // Also don't repeat the insert when Insert mode ended with CTRL-C.
    जर (curwin->w_cursor.lnum != oap->start.lnum || got_int)
	परत;

    जर (oap->block_mode)
    {
	पूर्णांक			ins_len;
	char_u			*firstline, *ins_text;
	संरचना block_def	bd2;
	पूर्णांक			did_indent = FALSE;
	माप_प्रकार			len;
	माप_प्रकार			add;
	// offset when cursor was moved in insert mode
	पूर्णांक			offset = 0;

	// If indent kicked in, the firstline might have changed
	// but only do that, if the indent actually increased.
	ind_post_col = (colnr_T)getwhitecols_curline();
	जर (curbuf->b_op_start.col > ind_pre_col && ind_post_col > ind_pre_col)
	{
	    bd.textcol += ind_post_col - ind_pre_col;
	    ind_post_vcol = get_indent();
	    bd.start_vcol += ind_post_vcol - ind_pre_vcol;
	    did_indent = TRUE;
	}

	// The user may have moved the cursor before inserting something, try
	// to adjust the block for that.  But only do it, if the difference
	// does not come from indent kicking in.
	जर (oap->start.lnum == curbuf->b_op_start_orig.lnum
						  && !bd.is_MAX && !did_indent)
	{
	    पूर्णांक t = getviscol2(curbuf->b_op_start_orig.col,
					       curbuf->b_op_start_orig.coladd);

	    जर (oap->op_type == OP_घाला
		    && oap->start.col + oap->start.coladd
			    != curbuf->b_op_start_orig.col
					      + curbuf->b_op_start_orig.coladd)
	    {
		oap->start.col = curbuf->b_op_start_orig.col;
		pre_textlen -= t - oap->start_vcol;
		oap->start_vcol = t;
	    }
	    अन्यथा जर (oap->op_type == OP_APPEND
		    && oap->start.col + oap->start.coladd
			    >= curbuf->b_op_start_orig.col
					      + curbuf->b_op_start_orig.coladd)
	    {
		oap->start.col = curbuf->b_op_start_orig.col;
		// reset pre_textlen to the value of OP_INSERT
		pre_textlen += bd.textlen;
		pre_textlen -= t - oap->start_vcol;
		oap->start_vcol = t;
		oap->op_type = OP_घाला;
	    }
	}

	// Spaces and tabs in the indent may have changed to other spaces and
	// tabs.  Get the starting column again and correct the length.
	// Don't do this when "$" used, end-of-line will have changed.
	//
	// if indent was added and the inserted text was after the indent,
	// correct the selection for the new indent.
	जर (did_indent && bd.textcol - ind_post_col > 0)
	{
	    oap->start.col += ind_post_col - ind_pre_col;
	    oap->start_vcol += ind_post_vcol - ind_pre_vcol;
	    oap->end.col += ind_post_col - ind_pre_col;
	    oap->end_vcol += ind_post_vcol - ind_pre_vcol;
	}
	block_prep(oap, &bd2, oap->start.lnum, TRUE);
	जर (did_indent && bd.textcol - ind_post_col > 0)
	{
	    // undo for where "oap" is used below
	    oap->start.col -= ind_post_col - ind_pre_col;
	    oap->start_vcol -= ind_post_vcol - ind_pre_vcol;
	    oap->end.col -= ind_post_col - ind_pre_col;
	    oap->end_vcol -= ind_post_vcol - ind_pre_vcol;
	}
	जर (!bd.is_MAX || bd2.textlen < bd.textlen)
	{
	    जर (oap->op_type == OP_APPEND)
	    {
		pre_textlen += bd2.textlen - bd.textlen;
		जर (bd2.endspaces)
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
	जर (oap->op_type == OP_APPEND)
	{
	    add += bd.textlen;
	    // account for pressing cursor in insert mode when '$' was used
	    जर (bd.is_MAX
		&& (start_घाला.lnum == Insstart.lnum
					   && start_घाला.col > Insstart.col))
	    {
		offset = (start_घाला.col - Insstart.col);
		add -= offset;
		जर (oap->end_vcol > offset)
		    oap->end_vcol -= (offset + 1);
		अन्यथा
		    // moved outside of the visual block, what to do?
		    परत;
	    }
	}
	जर (add > len)
	    add = len;  // short line, point to the NUL
	firstline += add;
	len -= add;
	जर (pre_textlen >= 0 && (ins_len = (पूर्णांक)len - pre_textlen - offset) > 0)
	{
	    ins_text = vim_strnsave(firstline, ins_len);
	    जर (ins_text != रिक्त)
	    {
		// block handled here
		जर (u_save(oap->start.lnum,
					 (linenr_T)(oap->end.lnum + 1)) == OK)
		    block_घाला(oap, ins_text, ins_len, (oap->op_type == OP_घाला),
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
    पूर्णांक
op_change(oparg_T *oap)
{
    colnr_T		l;
    पूर्णांक			retval;
    linenr_T		linenr;
    दीर्घ		pre_textlen = 0;
    दीर्घ		pre_indent = 0;
    char_u		*firstline;
    char_u		*ins_text, *newp, *oldp;
    संरचना block_def	bd;

    l = oap->start.col;
    जर (oap->motion_type == MLINE)
    {
	l = 0;
	can_si = may_do_si();	// Like opening a new line, do smart indent
    }

    // First delete the text in the region.  In an empty buffer only need to
    // save for undo
    जर (curbuf->b_ml.ml_flags & ML_EMPTY)
    {
	जर (u_save_cursor() == FAIL)
	    परत FALSE;
    }
    अन्यथा जर (op_delete(oap) == FAIL)
	परत FALSE;

    जर ((l > curwin->w_cursor.col) && !LINEEMPTY(curwin->w_cursor.lnum)
							 && !virtual_op)
	inc_cursor();

    // check for still on same line (<CR> in inserted text meaningless)
    // skip blank lines too
    जर (oap->block_mode)
    {
	// Add spaces before getting the current line length.
	जर (virtual_op && (curwin->w_cursor.coladd > 0
						    || gchar_cursor() == NUL))
	    coladvance_force(getviscol());
	firstline = ml_get(oap->start.lnum);
	pre_textlen = ml_get_len(oap->start.lnum);
	pre_indent = (दीर्घ)getwhitecols(firstline);
	bd.textcol = curwin->w_cursor.col;
    }

    जर (oap->motion_type == MLINE)
	fix_indent();

    // Reset finish_op now, don't want it set inside edit().
    पूर्णांक save_finish_op = finish_op;
    finish_op = FALSE;

    retval = edit(NUL, FALSE, (linenr_T)1);

    finish_op = save_finish_op;

    /*
     * In Visual block mode, handle copying the new text to all lines of the
     * block.
     * Don't repeat the insert when Insert mode ended with CTRL-C.
     */
    जर (oap->block_mode && oap->start.lnum != oap->end.lnum && !got_int)
    {
	पूर्णांक	ins_len;

	// Auto-indenting may have changed the indent.  If the cursor was past
	// the indent, exclude that indent change from the inserted text.
	firstline = ml_get(oap->start.lnum);
	जर (bd.textcol > (colnr_T)pre_indent)
	{
	    दीर्घ new_indent = (दीर्घ)getwhitecols(firstline);

	    pre_textlen += new_indent - pre_indent;
	    bd.textcol += new_indent - pre_indent;
	}

	ins_len = (पूर्णांक)ml_get_len(oap->start.lnum) - pre_textlen;
	जर (ins_len > 0)
	{
	    // Subsequent calls to ml_get() flush the firstline data - take a
	    // copy of the inserted text.
	    जर ((ins_text = alloc(ins_len + 1)) != रिक्त)
	    {
		vim_strncpy(ins_text, firstline + bd.textcol, ins_len);
		साठी (linenr = oap->start.lnum + 1; linenr <= oap->end.lnum;
								     linenr++)
		{
		    block_prep(oap, &bd, linenr, TRUE);
		    जर (!bd.is_short || virtual_op)
		    {
			pos_T vpos;
			माप_प्रकार newlen;

			// If the block starts in virtual space, count the
			// initial coladd offset as part of "startspaces"
			जर (bd.is_short)
			{
			    vpos.lnum = linenr;
			    (शून्य)getvpos(&vpos, oap->start_vcol);
			}
			अन्यथा
			    vpos.coladd = 0;
			oldp = ml_get(linenr);
			newp = alloc(ml_get_len(linenr) + vpos.coladd + ins_len + 1);
			जर (newp == रिक्त)
			    सुरूठेवा;
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
			जर (curbuf->b_has_textprop)
			    adjust_prop_columns(linenr, bd.textcol,
						     vpos.coladd + (पूर्णांक)ins_len, 0);
#endif
		    }
		}
		check_cursor();

		changed_ओळी(oap->start.lnum + 1, 0, oap->end.lnum + 1, 0L);
	    }
	    vim_free(ins_text);
	}
    }
    auto_format(FALSE, TRUE);

    परत retval;
}

/*
 * When the cursor is on the NUL past the end of the line and it should not be
 * there move it left.
 */
    शून्य
adjust_cursor_eol(शून्य)
{
    अचिन्हित पूर्णांक cur_ve_flags = get_ve_flags();

    पूर्णांक adj_cursor = (curwin->w_cursor.col > 0
				&& gchar_cursor() == NUL
				&& (cur_ve_flags & VE_ONEMORE) == 0
				&& (cur_ve_flags & VE_ALL) == 0
				&& !(restart_edit || (State & MODE_घाला)));
    जर (!adj_cursor)
	परत;

    // Put the cursor on the last character in the line.
    dec_cursor();

    जर (cur_ve_flags == VE_ALL)
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
    char_u   *ओळ,
    पूर्णांक      process,
    पूर्णांक	     include_space,
    पूर्णांक      *is_comment)
{
    char_u *comment_flags = रिक्त;
    पूर्णांक    lead_len;
    पूर्णांक    leader_offset = get_last_leader_offset(ओळ, &comment_flags);

    *is_comment = FALSE;
    जर (leader_offset != -1)
    {
	// Let's check whether the line ends with an unclosed comment.
	// If the last comment leader has COM_END in flags, there's no comment.
	पर्यंत (*comment_flags)
	{
	    जर (*comment_flags == COM_END
		    || *comment_flags == ':')
		थांबा;
	    ++comment_flags;
	}
	जर (*comment_flags != COM_END)
	    *is_comment = TRUE;
    }

    जर (process == FALSE)
	परत ओळ;

    lead_len = get_leader_len(ओळ, &comment_flags, FALSE, include_space);

    जर (lead_len == 0)
	परत ओळ;

    // Find:
    // - COM_END,
    // - colon,
    // whichever comes first.
    पर्यंत (*comment_flags)
    {
	जर (*comment_flags == COM_END
		|| *comment_flags == ':')
	    थांबा;
	++comment_flags;
    }

    // If we found a colon, it means that we are not processing a line
    // starting with a closing part of a three-part comment. That's good,
    // because we don't want to remove those as this would be annoying.
    जर (*comment_flags == ':' || *comment_flags == NUL)
	ओळ += lead_len;

    परत ओळ;
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
    पूर्णांक
do_join(
    दीर्घ    मोजणी,
    पूर्णांक	    घाला_space,
    पूर्णांक	    save_undo,
    पूर्णांक	    use_formatoptions UNUSED,
    पूर्णांक	    setmark)
{
    char_u	*curr = रिक्त;
    char_u      *curr_start = रिक्त;
    char_u	*cend;
    char_u	*newp;
    माप_प्रकार	newp_len;
    char_u	*spaces;	// number of spaces inserted before a line
    पूर्णांक		endcurr1 = NUL;
    पूर्णांक		endcurr2 = NUL;
    पूर्णांक		currsize = 0;	// size of the current line
    पूर्णांक		sumsize = 0;	// size of the long new line
    linenr_T	t;
    colnr_T	col = 0;
    पूर्णांक		ret = OK;
    पूर्णांक		*comments = रिक्त;
    पूर्णांक		remove_comments = (use_formatoptions == TRUE)
				  && has_format_option(FO_REMOVE_COMS);
    पूर्णांक		prev_was_comment;

    जर (save_undo && u_save((linenr_T)(curwin->w_cursor.lnum - 1),
			    (linenr_T)(curwin->w_cursor.lnum + मोजणी)) == FAIL)
	परत FAIL;

    // Allocate an array to store the number of spaces inserted before each
    // line.  We will use it to pre-compute the length of the new line and the
    // proper placement of each original line in the new one.
    spaces = lalloc_clear(मोजणी, TRUE);
    जर (spaces == रिक्त)
	परत FAIL;
    जर (remove_comments)
    {
	comments = lalloc_clear(मोजणी * आकार(पूर्णांक), TRUE);
	जर (comments == रिक्त)
	{
	    vim_free(spaces);
	    परत FAIL;
	}
    }

    /*
     * Don't move anything yet, just compute the final line length
     * and setup the array of space strings lengths
     * This loops forward over the joined lines.
     */
    साठी (t = 0; t < मोजणी; ++t)
    {
	curr = curr_start = ml_get((linenr_T)(curwin->w_cursor.lnum + t));
	जर (t == 0 && setmark && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set the '[ mark.
	    curwin->w_buffer->b_op_start.lnum = curwin->w_cursor.lnum;
	    curwin->w_buffer->b_op_start.col  = (colnr_T)STRLEN(curr);
	}
	जर (remove_comments)
	{
	    // We don't want to remove the comment leader if the
	    // previous line is not a comment.
	    जर (t > 0 && prev_was_comment)
	    {

		char_u *new_curr = skip_comment(curr, TRUE, घाला_space,
							   &prev_was_comment);
		comments[t] = (पूर्णांक)(new_curr - curr);
		curr = new_curr;
	    }
	    अन्यथा
		curr = skip_comment(curr, FALSE, घाला_space,
							   &prev_was_comment);
	}

	जर (घाला_space && t > 0)
	{
	    curr = skipwhite(curr);
	    जर (*curr != NUL && *curr != ')'
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
		जर (endcurr1 == ' ')
		    endcurr1 = endcurr2;
		अन्यथा
		    ++spaces[t];
		// extra space when 'joinspaces' set and line ends in '.'
		जर (       p_js
			&& (endcurr1 == '.'
			    || (vim_strchr(p_cpo, CPO_JOINSP) == रिक्त
				&& (endcurr1 == '?' || endcurr1 == '!'))))
		    ++spaces[t];
	    }
	}
	currsize = (पूर्णांक)STRLEN(curr);
	sumsize += currsize + spaces[t];
	endcurr1 = endcurr2 = NUL;
	जर (घाला_space && currsize > 0)
	{
	    जर (has_mbyte)
	    {
		cend = curr + currsize;
		MB_PTR_BACK(curr, cend);
		endcurr1 = (*mb_ptr2char)(cend);
		जर (cend > curr)
		{
		    MB_PTR_BACK(curr, cend);
		    endcurr2 = (*mb_ptr2char)(cend);
		}
	    }
	    अन्यथा
	    {
		endcurr1 = *(curr + currsize - 1);
		जर (currsize > 1)
		    endcurr2 = *(curr + currsize - 2);
	    }
	}
	ओळ_breakcheck();
	जर (got_int)
	{
	    ret = FAIL;
	    जा theend;
	}
    }

    // store the column position before last line
    col = sumsize - currsize - spaces[मोजणी - 1];

    // allocate the space for the new line
    newp_len = sumsize + 1;
    newp = alloc(newp_len);
    जर (newp == रिक्त)
    {
	ret = FAIL;
	जा theend;
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
    पूर्णांक curr_off = (पूर्णांक)(curr - curr_start);

    curr = curr_start = ml_get((linenr_T)(curwin->w_cursor.lnum + मोजणी - 1));
    curr += curr_off;
#endif

    साठी (t = मोजणी - 1; ; --t)
    {
	पूर्णांक spaces_removed;

	cend -= currsize;
	mch_memmove(cend, curr, (माप_प्रकार)currsize);

	जर (spaces[t] > 0)
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
		t == मोजणी - 1, (दीर्घ)(cend - newp), spaces_removed);
#endif
	जर (t == 0)
	    थांबा;
	curr = curr_start = ml_get((linenr_T)(curwin->w_cursor.lnum + t - 1));
	जर (remove_comments)
	    curr += comments[t - 1];
	जर (घाला_space && t > 1)
	    curr = skipwhite(curr);
	currsize = (पूर्णांक)STRLEN(curr);
    }

#ifdef FEAT_PROP_POPUP
    जर (um.buf != रिक्त)
    {
	um_set_text(&um, newp);
	um_reverse_props(&um);
	um_close(&um);
	newp = रिक्त;  // um_set_text took ownership
    }
    अन्यथा
#endif
    ml_replace_len(curwin->w_cursor.lnum, newp, (colnr_T)newp_len, TRUE, FALSE);

    जर (setmark && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set the '] mark.
	curwin->w_buffer->b_op_end.lnum = curwin->w_cursor.lnum;
	curwin->w_buffer->b_op_end.col  = (colnr_T)sumsize;
    }

    // Only report the change in the first line here, del_lines() will report
    // the deleted line.
    changed_ओळी(curwin->w_cursor.lnum, currsize,
					       curwin->w_cursor.lnum + 1, 0L);
    /*
     * Delete following lines. To do this we move the cursor there
     * briefly, and then move it back. After del_lines() the cursor may
     * have moved up (last line deleted), so the current lnum is kept in t.
     */
    t = curwin->w_cursor.lnum;
    ++curwin->w_cursor.lnum;
    del_ओळी(मोजणी - 1, FALSE);
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
    जर (remove_comments)
	vim_free(comments);
    परत ret;
}

#ifdef FEAT_LINEBREAK
/*
 * TODO: consider using "tailp" of win_lbr_chartabsize() instead of temporarily
 * resetting 'linebreak'.
 *
 * Reset 'linebreak' and take care of side effects.
 * Returns the previous value, to be passed to restore_lbr().
 */
    पूर्णांक
reset_lbr(शून्य)
{
    जर (!curwin->w_p_lbr)
	परत FALSE;
    // changing 'linebreak' may require w_virtcol to be updated
    curwin->w_p_lbr = FALSE;
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);
    परत TRUE;
}

/*
 * Restore 'linebreak' and take care of side effects.
 */
    शून्य
restore_lbr(पूर्णांक lbr_saved)
{
    जर (curwin->w_p_lbr || !lbr_saved)
	परत;

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
    पूर्णांक			is_del)
{
    पूर्णांक		incr = 0;
    char_u	*pend;
    char_u	*pstart;
    char_u	*ओळ;
    char_u	*prev_pstart;
    char_u	*prev_pend;
    chartabsize_T cts;
#ifdef FEAT_LINEBREAK
		// Avoid a problem with unwanted linebreaks in block mode.
    पूर्णांक		lbr_saved = reset_lbr();
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

    ओळ = ml_get(lnum);
    prev_pstart = ओळ;
    init_chartabsize_arg(&cts, curwin, lnum, bdp->start_vcol, ओळ, ओळ);
    पर्यंत (cts.cts_vcol < oap->start_vcol && *cts.cts_ptr != NUL)
    {
	// Count a tab for what it's worth (if list mode not on)
	incr = lbr_chartabsize(&cts);
	cts.cts_vcol += incr;
	जर (VIM_ISWHITE(*cts.cts_ptr))
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
    जर (bdp->start_vcol < oap->start_vcol)	// line too short
    {
	bdp->end_vcol = bdp->start_vcol;
	bdp->is_short = TRUE;
	जर (!is_del || oap->op_type == OP_APPEND)
	    bdp->endspaces = oap->end_vcol - oap->start_vcol + 1;
    }
    अन्यथा
    {
	// notice: this converts partly selected Multibyte characters to
	// spaces, too.
	bdp->startspaces = bdp->start_vcol - oap->start_vcol;
	जर (is_del && bdp->startspaces)
	    bdp->startspaces = bdp->start_char_vcols - bdp->startspaces;
	pend = pstart;
	bdp->end_vcol = bdp->start_vcol;
	जर (bdp->end_vcol > oap->end_vcol)	// it's all in one character
	{
	    bdp->is_oneChar = TRUE;
	    जर (oap->op_type == OP_घाला)
		bdp->endspaces = bdp->start_char_vcols - bdp->startspaces;
	    अन्यथा जर (oap->op_type == OP_APPEND)
	    {
		bdp->startspaces += oap->end_vcol - oap->start_vcol + 1;
		bdp->endspaces = bdp->start_char_vcols - bdp->startspaces;
	    }
	    अन्यथा
	    {
		bdp->startspaces = oap->end_vcol - oap->start_vcol + 1;
		जर (is_del && oap->op_type != OP_LSHIFT)
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
								  ओळ, pend);
	    prev_pend = pend;
	    पर्यंत (cts.cts_vcol <= oap->end_vcol && *cts.cts_ptr != NUL)
	    {
		// count a tab for what it's worth (if list mode not on)
		prev_pend = cts.cts_ptr;
		incr = lbr_chartabsize_adv(&cts);
		cts.cts_vcol += incr;
	    }
	    bdp->end_vcol = cts.cts_vcol;
	    pend = cts.cts_ptr;
	    clear_chartabsize_arg(&cts);

	    जर (bdp->end_vcol <= oap->end_vcol
		    && (!is_del
			|| oap->op_type == OP_APPEND
			|| oap->op_type == OP_REPLACE)) // line too short
	    {
		bdp->is_short = TRUE;
		// Alternative: include spaces to fill up the block.
		// Disadvantage: can lead to trailing spaces when the line is
		// short where the text is put
		// if (!is_del || oap->op_type == OP_APPEND)
		जर (oap->op_type == OP_APPEND || virtual_op)
		    bdp->endspaces = oap->end_vcol - bdp->end_vcol
							     + oap->inclusive;
		अन्यथा
		    bdp->endspaces = 0; // replace doesn't add characters
	    }
	    अन्यथा जर (bdp->end_vcol > oap->end_vcol)
	    {
		bdp->endspaces = bdp->end_vcol - oap->end_vcol - 1;
		जर (!is_del && bdp->endspaces)
		{
		    bdp->endspaces = incr - bdp->endspaces;
		    जर (pend != pstart)
			pend = prev_pend;
		}
	    }
	}
	bdp->end_char_vcols = incr;
	जर (is_del && bdp->startspaces)
	    pstart = prev_pstart;
	bdp->textlen = (पूर्णांक)(pend - pstart);
    }
    bdp->textcol = (colnr_T) (pstart - ओळ);
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
    पूर्णांक			inclusive)
{
    colnr_T startcol = 0, endcol = MAXCOL;
    colnr_T cs, ce;
    char_u *p;
    पूर्णांक	plen = ml_get_len(lnum);

    p = ml_get(lnum);
    bdp->startspaces = 0;
    bdp->endspaces = 0;
    bdp->is_oneChar = FALSE;
    bdp->start_char_vcols = 0;

    जर (lnum == start.lnum)
    {
	startcol = start.col;
	जर (virtual_op)
	{
	    getvcol(curwin, &start, &cs, रिक्त, &ce, 0);
	    जर (ce != cs && start.coladd > 0)
	    {
		// Part of a tab selected -- but don't
		// double-count it.
		bdp->start_char_vcols = ce - cs + 1;
		bdp->startspaces = bdp->start_char_vcols - start.coladd;
		जर (bdp->startspaces < 0)
		    bdp->startspaces = 0;
		startcol++;
	    }
	}
    }

    जर (lnum == end.lnum)
    {
	endcol = end.col;
	जर (virtual_op)
	{
	    getvcol(curwin, &end, &cs, रिक्त, &ce, 0);
	    जर (p[endcol] == NUL || (cs + end.coladd < ce
			// Don't add space for double-wide
			// char; endcol will be on last byte
			// of multi-byte char.
			&& (*mb_head_off)(p, p + endcol) == 0))
	    {
		जर (start.lnum == end.lnum && start.col == end.col)
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
    जर (endcol == MAXCOL)
	endcol = ml_get_len(lnum);
    जर (startcol > endcol || bdp->is_oneChar)
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
    पूर्णांक		g_cmd)		    // was g<c-a>/g<c-x>
{
    pos_T		pos;
    संरचना block_def	bd;
    पूर्णांक			change_cnt = 0;
    linenr_T		amount = Prenum1;

    // do_addsub() might trigger re-evaluation of 'foldexpr' halfway, when the
    // buffer is not completely updated yet. Postpone updating folds until before
    // the call to changed_lines().
#ifdef FEAT_FOLDING
    disable_fold_update++;
#endif

    जर (!VIsual_active)
    {
	pos = curwin->w_cursor;
	जर (u_save_cursor() == FAIL)
	{
#ifdef FEAT_FOLDING
	    disable_fold_update--;
#endif
	    परत;
	}
	change_cnt = do_addsub(oap->op_type, &pos, 0, amount);
#ifdef FEAT_FOLDING
	disable_fold_update--;
#endif
	जर (change_cnt)
	    changed_ओळी(pos.lnum, 0, pos.lnum + 1, 0L);
    }
    अन्यथा
    {
	पूर्णांक	one_change;
	पूर्णांक	length;
	pos_T	startpos;

	जर (u_save((linenr_T)(oap->start.lnum - 1),
					(linenr_T)(oap->end.lnum + 1)) == FAIL)
	{
#ifdef FEAT_FOLDING
	    disable_fold_update--;
#endif
	    परत;
	}

	pos = oap->start;
	साठी (; pos.lnum <= oap->end.lnum; ++pos.lnum)
	{
	    जर (oap->block_mode)		    // Visual block mode
	    {
		block_prep(oap, &bd, pos.lnum, FALSE);
		pos.col = bd.textcol;
		length = bd.textlen;
	    }
	    अन्यथा जर (oap->motion_type == MLINE)
	    {
		curwin->w_cursor.col = 0;
		pos.col = 0;
		length = ml_get_len(pos.lnum);
	    }
	    अन्यथा // oap->motion_type == MCHAR
	    {
		जर (pos.lnum == oap->start.lnum && !oap->inclusive)
		    dec(&(oap->end));
		length = ml_get_len(pos.lnum);
		pos.col = 0;
		जर (pos.lnum == oap->start.lnum)
		{
		    pos.col += oap->start.col;
		    length -= oap->start.col;
		}
		जर (pos.lnum == oap->end.lnum)
		{
		    length = ml_get_len(oap->end.lnum);
		    जर (oap->end.col >= length)
			oap->end.col = length - 1;
		    length = oap->end.col - pos.col + 1;
		}
	    }
	    one_change = do_addsub(oap->op_type, &pos, length, amount);
	    जर (one_change)
	    {
		// Remember the start position of the first change.
		जर (change_cnt == 0)
		    startpos = curbuf->b_op_start;
		++change_cnt;
	    }

#ifdef FEAT_NETBEANS_INTG
	    जर (netbeans_active() && one_change)
	    {
		char_u *ptr;

		netbeans_removed(curbuf, pos.lnum, pos.col, (दीर्घ)length);
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, pos.col,
						&ptr[pos.col], length);
	    }
#endif
	    जर (g_cmd && one_change)
		amount += Prenum1;
	}

#ifdef FEAT_FOLDING
	disable_fold_update--;
#endif
	जर (change_cnt)
	    changed_ओळी(oap->start.lnum, 0, oap->end.lnum + 1, 0L);

	जर (!change_cnt && oap->is_VIsual)
	    // No change: need to remove the Visual selection
	    redraw_curbuf_later(UPD_INVERTED);

	// Set '[ mark if something changed. Keep the last end
	// position from do_addsub().
	जर (change_cnt > 0 && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	    curbuf->b_op_start = startpos;

	जर (change_cnt > p_report)
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
    स्थिर पूर्णांक
do_addsub(
    पूर्णांक		op_type,
    pos_T	*pos,
    पूर्णांक		length,
    linenr_T	Prenum1)
{
    पूर्णांक		col;
    पूर्णांक		pre;		// 'X'/'x': hex; '0': octal; 'B'/'b': bin
    स्थिर पूर्णांक	hexupper = FALSE;	// 0xABC
    uvarnumber_T	n;
    uvarnumber_T	oldn;
    char_u	*ptr;
    पूर्णांक		linelen;
    पूर्णांक		c;
    पूर्णांक		todel;
    पूर्णांक		do_hex;
    पूर्णांक		do_oct;
    पूर्णांक		do_bin;
    पूर्णांक		do_alpha;
    पूर्णांक		do_unsigned;
    पूर्णांक		do_blank;
    पूर्णांक		blank_unsigned = FALSE;	// blank: treat as unsigned?
    पूर्णांक		firstdigit;
    पूर्णांक		subtract;
    पूर्णांक		negative = FALSE;
    पूर्णांक		was_positive = TRUE;
    पूर्णांक		visual = VIsual_active;
    पूर्णांक		did_change = FALSE;
    pos_T	save_cursor = curwin->w_cursor;
    पूर्णांक		maxlen = 0;
    pos_T	startpos;
    pos_T	endpos;
    colnr_T	save_coladd = 0;

    do_hex = (vim_strchr(curbuf->b_p_nf, 'x') != रिक्त);	// "heX"
    do_oct = (vim_strchr(curbuf->b_p_nf, 'o') != रिक्त);	// "Octal"
    do_bin = (vim_strchr(curbuf->b_p_nf, 'b') != रिक्त);	// "Bin"
    do_alpha = (vim_strchr(curbuf->b_p_nf, 'p') != रिक्त);	// "alPha"
    do_unsigned = (vim_strchr(curbuf->b_p_nf, 'u') != रिक्त);	// "Unsigned"
    do_blank = (vim_strchr(curbuf->b_p_nf, 'k') != रिक्त);	// "blanK"

    जर (virtual_active())
    {
	save_coladd = pos->coladd;
	pos->coladd = 0;
    }

    curwin->w_cursor = *pos;
    ptr = ml_get(pos->lnum);
    linelen = ml_get_len(pos->lnum);
    col = pos->col;

    जर (col + !!save_coladd >= linelen)
	जा theend;

    /*
     * First check if we are on a hexadecimal number, after the "0x".
     */
    जर (!VIsual_active)
    {
	जर (do_bin)
	    पर्यंत (col > 0 && vim_isbdigit(ptr[col]))
	    {
		--col;
		जर (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }

	जर (do_hex)
	    पर्यंत (col > 0 && vim_isxdigit(ptr[col]))
	    {
		--col;
		जर (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }

	जर (       do_bin
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

	    पर्यंत (col > 0 && vim_isdigit(ptr[col]))
	    {
		col--;
		जर (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }
	}

	जर ((       do_hex
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
	    जर (has_mbyte)
		col -= (*mb_head_off)(ptr, ptr + col);
	}
	अन्यथा
	{
	    /*
	     * Search forward and then backward to find the start of number.
	     */
	    col = pos->col;

	    पर्यंत (ptr[col] != NUL
		    && !vim_isdigit(ptr[col])
		    && !(do_alpha && ASCII_ISALPHA(ptr[col])))
		col += mb_ptr2len(ptr + col);

	    पर्यंत (col > 0
		    && vim_isdigit(ptr[col - 1])
		    && !(do_alpha && ASCII_ISALPHA(ptr[col])))
	    {
		--col;
		जर (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }
	}
    }

    जर (visual)
    {
	पर्यंत (ptr[col] != NUL && length > 0
		&& !vim_isdigit(ptr[col])
		&& !(do_alpha && ASCII_ISALPHA(ptr[col])))
	{
	    पूर्णांक mb_len = mb_ptr2len(ptr + col);

	    col += mb_len;
	    length -= mb_len;
	}

	जर (length == 0)
	    जा theend;

	जर (col > pos->col && ptr[col - 1] == '-'
		&& (!has_mbyte || !(*mb_head_off)(ptr, ptr + col - 1))
		&& !do_unsigned)
	{
	    जर (do_blank && col >= 2 && !VIM_ISWHITE(ptr[col - 2]))
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
    जर (!VIM_ISDIGIT(firstdigit) && !(do_alpha && ASCII_ISALPHA(firstdigit)))
    {
	beep_flush();
	जा theend;
    }

    जर (do_alpha && ASCII_ISALPHA(firstdigit))
    {
	// decrement or increment alphabetic character
	जर (op_type == OP_NR_SUB)
	{
	    जर (CharOrd(firstdigit) < Prenum1)
	    {
		जर (SAFE_isupper(firstdigit))
		    firstdigit = 'A';
		अन्यथा
		    firstdigit = 'a';
	    }
	    अन्यथा
		firstdigit -= Prenum1;
	}
	अन्यथा
	{
	    जर (26 - CharOrd(firstdigit) - 1 < Prenum1)
	    {
		जर (SAFE_isupper(firstdigit))
		    firstdigit = 'Z';
		अन्यथा
		    firstdigit = 'z';
	    }
	    अन्यथा
		firstdigit += Prenum1;
	}
	curwin->w_cursor.col = col;
	जर (!did_change)
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
	पूर्णांक	buf1len;
	char_u	buf2[NUMBUFLEN];
	पूर्णांक	buf2len;
	pos_T	save_pos;
	पूर्णांक	i;

	जर (col > 0 && ptr[col - 1] == '-'
		&& (!has_mbyte ||
		    !(*mb_head_off)(ptr, ptr + col - 1))
		&& !visual
		&& !do_unsigned)
	{
	    जर (do_blank && col >= 2 && !VIM_ISWHITE(ptr[col - 2]))
		blank_unsigned = TRUE;
	    अन्यथा
	    {
		// negative number
		--col;
		negative = TRUE;
	    }
	}

	// get the number value (unsigned)
	जर (visual && VIsual_mode != 'V')
	    maxlen = (curbuf->b_visual.vi_curswant == MAXCOL
		    ? linelen - col : length);

	पूर्णांक overflow = FALSE;
	vim_str2nr(ptr + col, &pre, &length,
		0 + (do_bin ? STR2NR_BIN : 0)
		    + (do_oct ? STR2NR_OCT : 0)
		    + (do_hex ? STR2NR_HEX : 0),
		रिक्त, &n, maxlen, FALSE, &overflow);

	// ignore leading '-' for hex and octal and bin numbers
	जर (pre && negative)
	{
	    ++col;
	    --length;
	    negative = FALSE;
	}
	// add or subtract
	subtract = FALSE;
	जर (op_type == OP_NR_SUB)
	    subtract ^= TRUE;
	जर (negative)
	    subtract ^= TRUE;

	oldn = n;
	जर (!overflow)  // if number is too big don't add/subtract
	{
	    जर (subtract)
		n -= (uvarnumber_T)Prenum1;
	    अन्यथा
		n += (uvarnumber_T)Prenum1;
	}

	// handle wraparound for decimal numbers
	जर (!pre)
	{
	    जर (subtract)
	    {
		जर (n > oldn)
		{
		    n = 1 + (n ^ (uvarnumber_T)-1);
		    negative ^= TRUE;
		}
	    }
	    अन्यथा
	    {
		// add
		जर (n < oldn)
		{
		    n = (n ^ (uvarnumber_T)-1);
		    negative ^= TRUE;
		}
	    }
	    जर (n == 0)
		negative = FALSE;
	}

	जर ((do_unsigned || blank_unsigned) && negative)
	{
	    जर (subtract)
		// sticking at zero.
		n = (uvarnumber_T)0;
	    अन्यथा
		// sticking at 2^64 - 1.
		n = (uvarnumber_T)(-1);
	    negative = FALSE;
	}

	जर (visual && !was_positive && !negative && col > 0)
	{
	    // need to remove the '-'
	    col--;
	    length++;
	}

	/*
	 * Delete the old number.
	 */
	curwin->w_cursor.col = col;
	जर (!did_change)
	    startpos = curwin->w_cursor;
	did_change = TRUE;
	todel = length;
	c = gchar_cursor();
	/*
	 * Don't include the '-' in the length, only the length of the
	 * part after it is kept the same.
	 */
	जर (c == '-')
	    --length;

	save_pos = curwin->w_cursor;
	साठी (i = 0; i < todel; ++i)
	{
	    जर (c < 0x100 && SAFE_isalpha(c))
	    {
		जर (SAFE_isupper(c))
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
	जर (buf1 == रिक्त)
	    जा theend;
	ptr = buf1;
	जर (negative && (!visual || was_positive))
	    *ptr++ = '-';
	जर (pre)
	{
	    *ptr++ = '0';
	    --length;
	}
	जर (pre == 'b' || pre == 'B' ||
	    pre == 'x' || pre == 'X')
	{
	    *ptr++ = pre;
	    --length;
	}

	/*
	 * Put the number characters in buf2[].
	 */
	जर (pre == 'b' || pre == 'B')
	{
	    पूर्णांक bit = 0;
	    पूर्णांक bits = आकार(uvarnumber_T) * 8;

	    // leading zeros
	    साठी (bit = bits; bit > 0; bit--)
		जर ((n >> (bit - 1)) & 0x1) थांबा;

	    साठी (buf2len = 0; bit > 0 && buf2len < (NUMBUFLEN - 1); bit--)
		buf2[buf2len++] = ((n >> (bit - 1)) & 0x1) ? '1' : '0';

	    buf2[buf2len] = NUL;
	}
	अन्यथा जर (pre == 0)
	    buf2len = vim_snprintf((अक्षर *)buf2, NUMBUFLEN, "%llu", n);
	अन्यथा जर (pre == '0')
	    buf2len = vim_snprintf((अक्षर *)buf2, NUMBUFLEN, "%llo", n);
	अन्यथा जर (pre && hexupper)
	    buf2len = vim_snprintf((अक्षर *)buf2, NUMBUFLEN, "%llX", n);
	अन्यथा
	    buf2len = vim_snprintf((अक्षर *)buf2, NUMBUFLEN, "%llx", n);
	length -= buf2len;

	/*
	 * Adjust number of zeros to the new number of digits, so the
	 * total length of the number remains the same.
	 * Don't do this when
	 * the result may look like an octal number.
	 */
	जर (firstdigit == '0' && !(do_oct && pre == 0))
	    पर्यंत (length-- > 0)
		*ptr++ = '0';
	*ptr = NUL;
	buf1len = (पूर्णांक)(ptr - buf1);

	STRCPY(buf1 + buf1len, buf2);
	buf1len += buf2len;

	// Insert just after the first character to be removed, so that any
	// text properties will be adjusted.  Then delete the old number
	// afterwards.
	save_pos = curwin->w_cursor;
	जर (todel > 0)
	    inc_cursor();
	ins_str(buf1, (माप_प्रकार)buf1len);		// insert the new number
	vim_free(buf1);

	// del_char() will also mark line needing displaying
	जर (todel > 0)
	{
	    पूर्णांक bytes_after = ml_get_curline_len() - curwin->w_cursor.col;

	    // Delete the one character before the insert.
	    curwin->w_cursor = save_pos;
	    (शून्य)del_char(FALSE);
	    curwin->w_cursor.col = ml_get_curline_len() - bytes_after;
	    --todel;
	}
	पर्यंत (todel-- > 0)
	    (शून्य)del_char(FALSE);

	endpos = curwin->w_cursor;
	जर (did_change && curwin->w_cursor.col)
	    --curwin->w_cursor.col;
    }

    जर (did_change && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// set the '[ and '] marks
	curbuf->b_op_start = startpos;
	curbuf->b_op_end = endpos;
	जर (curbuf->b_op_end.col > 0)
	    --curbuf->b_op_end.col;
    }

theend:
    जर (visual)
	curwin->w_cursor = save_cursor;
    अन्यथा जर (did_change)
	curwin->w_set_curswant = true;
    अन्यथा जर (virtual_active())
	curwin->w_cursor.coladd = save_coladd;

    परत did_change;
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
ओळ_मोजणी_info(
    char_u	*ओळ,
    varnumber_T	*wc,
    varnumber_T	*cc,
    varnumber_T	limit,
    पूर्णांक		eol_size)
{
    varnumber_T	i;
    varnumber_T	शब्द = 0;
    varnumber_T	chars = 0;
    पूर्णांक		is_word = 0;

    साठी (i = 0; i < limit && ओळ[i] != NUL; )
    {
	जर (is_word)
	{
	    जर (vim_isspace(ओळ[i]))
	    {
		शब्द++;
		is_word = 0;
	    }
	}
	अन्यथा जर (!vim_isspace(ओळ[i]))
	    is_word = 1;
	++chars;
	i += (*mb_ptr2len)(ओळ + i);
    }

    जर (is_word)
	शब्द++;
    *wc += शब्द;

    // Add eol_size if the end of line was reached before hitting limit.
    जर (i < limit && ओळ[i] == NUL)
    {
	i += eol_size;
	chars += eol_size;
    }
    *cc += chars;
    परत i;
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
    varnumber_T	byte_मोजणी = 0;
    varnumber_T	bom_मोजणी  = 0;
    varnumber_T	byte_मोजणी_cursor = 0;
    varnumber_T	char_मोजणी = 0;
    varnumber_T	char_मोजणी_cursor = 0;
    varnumber_T	word_मोजणी = 0;
    varnumber_T	word_मोजणी_cursor = 0;
    पूर्णांक		eol_size;
    varnumber_T	last_check = 100000L;
    दीर्घ	ओळ_मोजणी_selected = 0;
    pos_T	min_pos, max_pos;
    oparg_T	oparg;
    संरचना block_def	bd;

    /*
     * Compute the length of the file in characters.
     */
    जर (curbuf->b_ml.ml_flags & ML_EMPTY)
    {
	जर (dict == रिक्त)
	{
	    संदेश(_(no_ओळी_संदेश));
	    परत;
	}
    }
    अन्यथा
    {
	जर (get_fileformat(curbuf) == EOL_DOS)
	    eol_size = 2;
	अन्यथा
	    eol_size = 1;

	जर (VIsual_active)
	{
	    जर (LT_POS(VIsual, curwin->w_cursor))
	    {
		min_pos = VIsual;
		max_pos = curwin->w_cursor;
	    }
	    अन्यथा
	    {
		min_pos = curwin->w_cursor;
		max_pos = VIsual;
	    }
	    जर (*p_sel == 'e' && max_pos.col > 0)
		--max_pos.col;

	    जर (VIsual_mode == Ctrl_V)
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
		जर (curwin->w_curswant == MAXCOL)
		    oparg.end_vcol = MAXCOL;
		// Swap the start, end vcol if needed
		जर (oparg.end_vcol < oparg.start_vcol)
		{
		    oparg.end_vcol += oparg.start_vcol;
		    oparg.start_vcol = oparg.end_vcol - oparg.start_vcol;
		    oparg.end_vcol -= oparg.start_vcol;
		}
	    }
	    ओळ_मोजणी_selected = max_pos.lnum - min_pos.lnum + 1;
	}

	साठी (lnum = 1; lnum <= curbuf->b_ml.ml_ओळ_मोजणी; ++lnum)
	{
	    // Check for a CTRL-C every 100000 characters.
	    जर (byte_मोजणी > last_check)
	    {
		ui_breakcheck();
		जर (got_int)
		    परत;
		last_check = byte_मोजणी + 100000L;
	    }

	    // Do extra processing for VIsual mode.
	    जर (VIsual_active
		    && lnum >= min_pos.lnum && lnum <= max_pos.lnum)
	    {
		char_u	    *s = रिक्त;
		दीर्घ	    len = 0L;

		निवड (VIsual_mode)
		{
		    स्थिती Ctrl_V:
			virtual_op = virtual_active();
			block_prep(&oparg, &bd, lnum, 0);
			virtual_op = MAYBE;
			s = bd.textstart;
			len = (दीर्घ)bd.textlen;
			थांबा;
		    स्थिती 'V':
			s = ml_get(lnum);
			len = MAXCOL;
			थांबा;
		    स्थिती 'v':
			{
			    colnr_T start_col = (lnum == min_pos.lnum)
							   ? min_pos.col : 0;
			    colnr_T end_col = (lnum == max_pos.lnum)
				      ? max_pos.col - start_col + 1 : MAXCOL;

			    s = ml_get(lnum) + start_col;
			    len = end_col;
			}
			थांबा;
		}
		जर (s != रिक्त)
		{
		    byte_मोजणी_cursor += ओळ_मोजणी_info(s, &word_मोजणी_cursor,
					   &char_मोजणी_cursor, len, eol_size);
		    जर (lnum == curbuf->b_ml.ml_ओळ_मोजणी
			    && !curbuf->b_p_eol
			    && (curbuf->b_p_bin || !curbuf->b_p_fixeol)
			    && (दीर्घ)STRLEN(s) < len)
			byte_मोजणी_cursor -= eol_size;
		}
	    }
	    अन्यथा
	    {
		// In non-visual mode, check for the line the cursor is on
		जर (lnum == curwin->w_cursor.lnum)
		{
		    word_मोजणी_cursor += word_मोजणी;
		    char_मोजणी_cursor += char_मोजणी;
		    byte_मोजणी_cursor = byte_मोजणी +
			ओळ_मोजणी_info(ml_get(lnum),
				&word_मोजणी_cursor, &char_मोजणी_cursor,
				(varnumber_T)(curwin->w_cursor.col + 1),
				eol_size);
		}
	    }
	    // Add to the running totals
	    byte_मोजणी += ओळ_मोजणी_info(ml_get(lnum), &word_मोजणी,
					 &char_मोजणी, (varnumber_T)MAXCOL,
					 eol_size);
	}

	// Correction for when last line doesn't have an EOL.
	जर (!curbuf->b_p_eol && (curbuf->b_p_bin || !curbuf->b_p_fixeol))
	    byte_मोजणी -= eol_size;

	जर (dict == रिक्त)
	{
	    जर (VIsual_active)
	    {
		जर (VIsual_mode == Ctrl_V && curwin->w_curswant < MAXCOL)
		{
		    getvcols(curwin, &min_pos, &max_pos,
						&min_pos.col, &max_pos.col, 0);
		    vim_snprintf((अक्षर *)buf1, आकार(buf1), _("%ld Cols; "),
			    (दीर्घ)(oparg.end_vcol - oparg.start_vcol + 1));
		}
		अन्यथा
		    buf1[0] = NUL;

		जर (char_मोजणी_cursor == byte_मोजणी_cursor
						    && char_मोजणी == byte_मोजणी)
		    vim_snprintf((अक्षर *)IObuff, IOSIZE,
			    _("Selected %s%ld of %ld Lines; %lld of %lld Words; %lld of %lld Bytes"),
			    buf1, ओळ_मोजणी_selected,
			    (दीर्घ)curbuf->b_ml.ml_ओळ_मोजणी,
			    word_मोजणी_cursor,
			    word_मोजणी,
			    byte_मोजणी_cursor,
			    byte_मोजणी);
		अन्यथा
		    vim_snprintf((अक्षर *)IObuff, IOSIZE,
			    _("Selected %s%ld of %ld Lines; %lld of %lld Words; %lld of %lld Chars; %lld of %lld Bytes"),
			    buf1, ओळ_मोजणी_selected,
			    (दीर्घ)curbuf->b_ml.ml_ओळ_मोजणी,
			    word_मोजणी_cursor,
			    word_मोजणी,
			    char_मोजणी_cursor,
			    char_मोजणी,
			    byte_मोजणी_cursor,
			    byte_मोजणी);
	    }
	    अन्यथा
	    {
		p = ml_get_curline();
		validate_virtcol();
		col_print(buf1, आकार(buf1), (पूर्णांक)curwin->w_cursor.col + 1,
			(पूर्णांक)curwin->w_virtcol + 1);
		col_print(buf2, आकार(buf2), ml_get_curline_len(),
							   linetabsize_str(p));

		जर (char_मोजणी_cursor == byte_मोजणी_cursor
			&& char_मोजणी == byte_मोजणी)
		    vim_snprintf((अक्षर *)IObuff, IOSIZE,
			_("Col %s of %s; Line %ld of %ld; Word %lld of %lld; Byte %lld of %lld"),
			(अक्षर *)buf1, (अक्षर *)buf2,
			(दीर्घ)curwin->w_cursor.lnum,
			(दीर्घ)curbuf->b_ml.ml_ओळ_मोजणी,
			word_मोजणी_cursor, word_मोजणी,
			byte_मोजणी_cursor, byte_मोजणी);
		अन्यथा
		    vim_snprintf((अक्षर *)IObuff, IOSIZE,
			_("Col %s of %s; Line %ld of %ld; Word %lld of %lld; Char %lld of %lld; Byte %lld of %lld"),
			(अक्षर *)buf1, (अक्षर *)buf2,
			(दीर्घ)curwin->w_cursor.lnum,
			(दीर्घ)curbuf->b_ml.ml_ओळ_मोजणी,
			word_मोजणी_cursor, word_मोजणी,
			char_मोजणी_cursor, char_मोजणी,
			byte_मोजणी_cursor, byte_मोजणी);
	    }
	}

	bom_मोजणी = bomb_size();
	जर (dict == रिक्त && bom_मोजणी > 0)
	{
	    माप_प्रकार len = STRLEN(IObuff);

	    vim_snprintf((अक्षर *)IObuff + len, IOSIZE - len,
				 _("(+%lld for BOM)"), bom_मोजणी);
	}
	जर (dict == रिक्त)
	{
	    // Don't shorten this message, the user asked for it.
	    p = p_shm;
	    p_shm = (char_u *)"";
	    संदेश((अक्षर *)IObuff);
	    p_shm = p;
	}
    }
#जर defined(FEAT_EVAL)
    जर (dict != रिक्त)
    {
	dict_add_संख्या(dict, "words", word_मोजणी);
	dict_add_संख्या(dict, "chars", char_मोजणी);
	dict_add_संख्या(dict, "bytes", byte_मोजणी + bom_मोजणी);
	dict_add_संख्या(dict, VIsual_active ? "visual_bytes" : "cursor_bytes",
		byte_मोजणी_cursor);
	dict_add_संख्या(dict, VIsual_active ? "visual_chars" : "cursor_chars",
		char_मोजणी_cursor);
	dict_add_संख्या(dict, VIsual_active ? "visual_words" : "cursor_words",
		word_मोजणी_cursor);
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
    जर (oap->is_VIsual)
	stuffReadbuff((char_u *)"'<,'>");
    अन्यथा
    {
	// Make the range look nice, so it can be repeated.
	जर (oap->start.lnum == curwin->w_cursor.lnum)
	    stuffcharReadbuff('.');
	अन्यथा
	    stuffnumReadbuff((दीर्घ)oap->start.lnum);

#ifdef FEAT_FOLDING
	// When using !! on a closed fold the range ".!" works best to operate
	// on, it will be made the whole closed fold later.
	linenr_T endOfStartFold = oap->start.lnum;
	(शून्य)hasFolding(oap->start.lnum, रिक्त, &endOfStartFold);
#endif
	जर (oap->end.lnum != oap->start.lnum
#ifdef FEAT_FOLDING
		&& oap->end.lnum != endOfStartFold
#endif
	   )
	{
	    // Make it a range with the end line.
	    stuffcharReadbuff(',');
	    जर (oap->end.lnum == curwin->w_cursor.lnum)
		stuffcharReadbuff('.');
	    अन्यथा जर (oap->end.lnum == curbuf->b_ml.ml_ओळ_मोजणी)
		stuffcharReadbuff('$');
	    अन्यथा जर (oap->start.lnum == curwin->w_cursor.lnum
#ifdef FEAT_FOLDING
		    // do not use ".+number" for a closed fold, it would count
		    // folded lines twice
		    && !hasFolding(oap->end.lnum, रिक्त, रिक्त)
#endif
		    )
	    {
		stuffReadbuff((char_u *)".+");
		stuffnumReadbuff((दीर्घ)oap->ओळ_मोजणी - 1);
	    }
	    अन्यथा
		stuffnumReadbuff((दीर्घ)oap->end.lnum);
	}
    }
    जर (oap->op_type != OP_COLON)
	stuffReadbuff((char_u *)"!");
    जर (oap->op_type == OP_INDENT)
    {
	जर (*get_equalprg() == NUL)
	    stuffReadbuff((char_u *)"indent");
	अन्यथा
	    stuffReadbuff(get_equalprg());
	stuffReadbuff((char_u *)"\n");
    }
    अन्यथा जर (oap->op_type == OP_FORMAT)
    {
	जर (*curbuf->b_p_fp != NUL)
	    stuffReadbuff(curbuf->b_p_fp);
	अन्यथा जर (*p_fp != NUL)
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
    अक्षर *
did_set_operatorfunc(optset_T *args UNUSED)
{
    जर (option_set_callback_func(p_opfunc, &opfunc_cb) == FAIL)
	परत e_invalid_argument;

    परत रिक्त;
}

#जर defined(EXITFREE)
    शून्य
free_operatorfunc_option(शून्य)
{
# ifdef FEAT_EVAL
    free_callback(&opfunc_cb);
# endif
}
#endif

#जर defined(FEAT_EVAL)
/*
 * Mark the global 'operatorfunc' callback with "copyID" so that it is not
 * garbage collected.
 */
    पूर्णांक
set_ref_in_opfunc(पूर्णांक copyID UNUSED)
{
    पूर्णांक पात = FALSE;

    पात = set_ref_in_callback(&opfunc_cb, copyID);

    परत पात;
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

    जर (*p_opfunc == NUL)
	emsg(_(e_operatorfunc_is_empty));
    अन्यथा
    {
	// Set '[ and '] marks to text to be operated on.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
	जर (oap->motion_type != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_end);

	argv[0].v_type = VAR_STRING;
	जर (oap->block_mode)
	    argv[0].vval.v_string = (char_u *)"block";
	अन्यथा जर (oap->motion_type == MLINE)
	    argv[0].vval.v_string = (char_u *)"line";
	अन्यथा
	    argv[0].vval.v_string = (char_u *)"char";
	argv[1].v_type = VAR_UNKNOWN;

	// Reset virtual_op so that 'virtualedit' can be changed in the
	// function.
	पूर्णांक save_virtual_op = virtual_op;
	virtual_op = MAYBE;

	// Reset finish_op so that mode() returns the right value.
	पूर्णांक save_finish_op = finish_op;
	finish_op = FALSE;

	जर (call_callback(&opfunc_cb, 0, &rettv, 1, argv) != FAIL)
	    clear_tv(&rettv);

	virtual_op = save_virtual_op;
	finish_op = save_finish_op;
	जर (cmdmod.cmod_flags & CMOD_LOCKMARKS)
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
    पूर्णांक		initial)    // when TRUE adjust position for 'selectmode'
{
    colnr_T	    start, end;

    जर (VIsual_mode != Ctrl_V
	    || (!initial && oap->end.col < curwin->w_width))
	परत;

    oap->block_mode = TRUE;

    // prevent from moving onto a trail byte
    जर (has_mbyte)
	mb_adjustpos(curwin->w_buffer, &oap->end);

    getvvcol(curwin, &(oap->start), &oap->start_vcol, रिक्त, &oap->end_vcol, 0);

    जर (!redo_VIsual_busy)
    {
	getvvcol(curwin, &(oap->end), &start, रिक्त, &end, 0);

	जर (start < oap->start_vcol)
	    oap->start_vcol = start;
	जर (end > oap->end_vcol)
	{
	    जर (initial && *p_sel == 'e' && start >= 1
				    && start - 1 >= oap->end_vcol)
		oap->end_vcol = start - 1;
	    अन्यथा
		oap->end_vcol = end;
	}
    }

    // if '$' was used, get oap->end_vcol from longest line
    जर (curwin->w_curswant == MAXCOL)
    {
	curwin->w_cursor.col = MAXCOL;
	oap->end_vcol = 0;
	साठी (curwin->w_cursor.lnum = oap->start.lnum;
		curwin->w_cursor.lnum <= oap->end.lnum;
					++curwin->w_cursor.lnum)
	{
	    getvvcol(curwin, &curwin->w_cursor, रिक्त, रिक्त, &end, 0);
	    जर (end > oap->end_vcol)
		oap->end_vcol = end;
	}
    }
    अन्यथा जर (redo_VIsual_busy)
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
प्रकारव्याख्या संरचना {
    पूर्णांक		rv_mode;	// 'v', 'V', or Ctrl-V
    linenr_T	rv_ओळ_मोजणी;	// number of lines
    colnr_T	rv_vcol;	// number of cols or end column
    दीर्घ	rv_मोजणी;	// count for Visual operator
    पूर्णांक		rv_arg;		// extra argument
} redo_VIsual_T;

    स्थिर पूर्णांक
is_ex_cmdchar(cmdarg_T *cap)
{
    परत cap->cmdchar == ':'
	|| cap->cmdchar == K_COMMAND
	|| cap->cmdchar == K_SCRIPT_COMMAND;
}

/*
 * Handle an operator after Visual mode or when the movement is finished.
 * "gui_yank" is true when yanking text for the clipboard.
 */
    शून्य
do_pending_operator(cmdarg_T *cap, पूर्णांक old_col, पूर्णांक gui_yank)
{
    oparg_T	*oap = cap->oap;
    pos_T	old_cursor;
    पूर्णांक		empty_region_error;
    पूर्णांक		restart_edit_save;
#ifdef FEAT_LINEBREAK
    पूर्णांक		lbr_saved = curwin->w_p_lbr;
#endif

    // The visual area is remembered for redo
    स्थिर redo_VIsual_T   redo_VIsual = {NUL, 0, 0, 0,0};

    पूर्णांक		    include_ओळ_break = FALSE;

#जर defined(FEAT_CLIPBOARD)
    // Yank the visual area into the GUI selection register before we operate
    // on it and lose it forever.
    // Don't do it if a specific register was specified, so that ""x"*P works.
    // This could call do_pending_operator() recursively, but that's OK
    // because gui_yank will be TRUE for the nested call.
    जर ((clip_star.available || clip_plus.available)
	    && oap->op_type != OP_NOP
	    && !gui_yank
	    && VIsual_active
	    && !redo_VIsual_busy
	    && oap->regname == 0)
	clip_auto_select();
#endif
    old_cursor = curwin->w_cursor;

    // If an operation is pending, handle it...
    जर ((finish_op || VIsual_active) && oap->op_type != OP_NOP)
    {
	// Yank can be redone when 'y' is in 'cpoptions', but not when yanking
	// for the clipboard.
	पूर्णांक	redo_yank = vim_strchr(p_cpo, CPO_YANK) != रिक्त && !gui_yank;

#ifdef FEAT_LINEBREAK
	// Avoid a problem with unwanted linebreaks in block mode.
	(शून्य)reset_lbr();
#endif
	oap->is_VIsual = VIsual_active;
	जर (oap->motion_force == 'V')
	    oap->motion_type = MLINE;
	अन्यथा जर (oap->motion_force == 'v')
	{
	    // If the motion was linewise, "inclusive" will not have been set.
	    // Use "exclusive" to be consistent.  Makes "dvj" work nice.
	    जर (oap->motion_type == MLINE)
		oap->inclusive = FALSE;
	    // If the motion already was characterwise, toggle "inclusive"
	    अन्यथा जर (oap->motion_type == MCHAR)
		oap->inclusive = !oap->inclusive;
	    oap->motion_type = MCHAR;
	}
	अन्यथा जर (oap->motion_force == Ctrl_V)
	{
	    // Change line- or characterwise motion into Visual block mode.
	    जर (!VIsual_active)
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
	जर ((redo_yank || oap->op_type != OP_YANK)
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
	    जर (cap->cmdchar == '/' || cap->cmdchar == '?') // was a search
	    {
		// If 'cpoptions' does not contain 'r', insert the search
		// pattern to really repeat the same command.
		जर (vim_strchr(p_cpo, CPO_REDO) == रिक्त)
		    AppendToRedobuffLit(cap->searchbuf, -1);
		AppendToRedobuff(NL_STR);
	    }
	    अन्यथा जर (is_ex_cmdchar(cap))
	    {
		// do_cmdline() has stored the first typed line in
		// "repeat_cmdline".  When several lines are typed repeating
		// won't be possible.
		जर (repeat_cmdline == रिक्त)
		    ResetRedobuff();
		अन्यथा
		{
		    जर (cap->cmdchar == ':')
			AppendToRedobuffLit(repeat_cmdline, -1);
		    अन्यथा
			AppendToRedobuffSpec(repeat_cmdline);
		    AppendToRedobuff(NL_STR);
		    VIM_CLEAR(repeat_cmdline);
		}
	    }
	}

	जर (redo_VIsual_busy)
	{
	    // Redo of an operation on a Visual area. Use the same size from
	    // redo_VIsual.rv_line_count and redo_VIsual.rv_vcol.
	    oap->start = curwin->w_cursor;
	    curwin->w_cursor.lnum += redo_VIsual.rv_ओळ_मोजणी - 1;
	    जर (curwin->w_cursor.lnum > curbuf->b_ml.ml_ओळ_मोजणी)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_ओळ_मोजणी;
	    VIsual_mode = redo_VIsual.rv_mode;
	    जर (redo_VIsual.rv_vcol == MAXCOL || VIsual_mode == 'v')
	    {
		जर (VIsual_mode == 'v')
		{
		    जर (redo_VIsual.rv_ओळ_मोजणी <= 1)
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
	    cap->count0 = redo_VIsual.rv_मोजणी;
	    जर (redo_VIsual.rv_मोजणी != 0)
		cap->count1 = redo_VIsual.rv_मोजणी;
	    अन्यथा
		cap->count1 = 1;
	}
	अन्यथा जर (VIsual_active)
	{
	    जर (!gui_yank)
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
	    जर (VIsual_select && VIsual_mode == 'V'
					    && cap->oap->op_type != OP_DELETE)
	    {
		जर (LT_POS(VIsual, curwin->w_cursor))
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
	    अन्यथा जर (VIsual_mode == 'v')
		include_ओळ_break = unadjust_for_sel();

	    oap->start = VIsual;
	    जर (VIsual_mode == 'V')
	    {
		oap->start.col = 0;
		oap->start.coladd = 0;
	    }
	}

	// Set oap->start to the first position of the operated text, oap->end
	// to the end of the operated text.  w_cursor is equal to oap->start.
	जर (LT_POS(oap->start, curwin->w_cursor))
	{
#ifdef FEAT_FOLDING
	    // Include folded lines completely.
	    जर (!VIsual_active)
	    {
		जर (hasFolding(oap->start.lnum, &oap->start.lnum, रिक्त))
		    oap->start.col = 0;
		जर ((curwin->w_cursor.col > 0 || oap->inclusive
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
	    जर (!VIsual_active && oap->motion_type == MLINE)
	    {
		जर (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum,
									रिक्त))
		    curwin->w_cursor.col = 0;
		जर (hasFolding(oap->start.lnum, रिक्त, &oap->start.lnum))
		    oap->start.col = ml_get_len(oap->start.lnum);
	    }
#endif
	    oap->end = oap->start;
	    oap->start = curwin->w_cursor;
	}

	// Just in case lines were deleted that make the position invalid.
	check_pos(curwin->w_buffer, &oap->end);
	oap->ओळ_मोजणी = oap->end.lnum - oap->start.lnum + 1;

	// Set "virtual_op" before resetting VIsual_active.
	virtual_op = virtual_active();

	जर (VIsual_active || redo_VIsual_busy)
	{
	    get_op_vcol(oap, redo_VIsual.rv_vcol, TRUE);

	    जर (!redo_VIsual_busy && !gui_yank)
	    {
		// Prepare to reselect and redo Visual: this is based on the
		// size of the Visual text
		resel_VIsual_mode = VIsual_mode;
		जर (curwin->w_curswant == MAXCOL)
		    resel_VIsual_vcol = MAXCOL;
		अन्यथा
		{
		    जर (VIsual_mode != Ctrl_V)
			getvvcol(curwin, &(oap->end),
						रिक्त, रिक्त, &oap->end_vcol, 0);
		    जर (VIsual_mode == Ctrl_V || oap->ओळ_मोजणी <= 1)
		    {
			जर (VIsual_mode != Ctrl_V)
			    getvvcol(curwin, &(oap->start),
					      &oap->start_vcol, रिक्त, रिक्त, 0);
			resel_VIsual_vcol = oap->end_vcol - oap->start_vcol + 1;
		    }
		    अन्यथा
			resel_VIsual_vcol = oap->end_vcol;
		}
		resel_VIsual_ओळ_मोजणी = oap->ओळ_मोजणी;
	    }

	    // can't redo yank (unless 'y' is in 'cpoptions') and ":"
	    जर ((redo_yank || oap->op_type != OP_YANK)
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
		जर (cap->cmdchar == 'g' && (cap->nchar == 'n'
							|| cap->nchar == 'N'))
		    prep_redo(oap->regname, cap->count0,
			    get_op_char(oap->op_type),
			    get_extra_op_char(oap->op_type),
			    oap->motion_force, cap->cmdchar, cap->nchar);
		अन्यथा जर (!is_ex_cmdchar(cap))
		{
		    पूर्णांक opchar = get_op_char(oap->op_type);
		    पूर्णांक extra_opchar = get_extra_op_char(oap->op_type);
		    पूर्णांक nchar = oap->op_type == OP_REPLACE ? cap->nchar : NUL;

		    // reverse what nv_replace() did
		    जर (nchar == REPLACE_CR_NCHAR)
			nchar = CAR;
		    अन्यथा जर (nchar == REPLACE_NL_NCHAR)
			nchar = NL;

		    जर (opchar == 'g' && extra_opchar == '@')
			// also repeat the count for 'operatorfunc'
			prep_redo_num2(oap->regname, 0L, NUL, 'v',
				     cap->count0, opchar, extra_opchar, nchar);
		    अन्यथा
			prep_redo(oap->regname, 0L, NUL, 'v',
						  opchar, extra_opchar, nchar);
		}
		जर (!redo_VIsual_busy)
		{
		    redo_VIsual.rv_mode = resel_VIsual_mode;
		    redo_VIsual.rv_vcol = resel_VIsual_vcol;
		    redo_VIsual.rv_ओळ_मोजणी = resel_VIsual_ओळ_मोजणी;
		    redo_VIsual.rv_मोजणी = cap->count0;
		    redo_VIsual.rv_arg = cap->arg;
		}
	    }

	    // oap->inclusive defaults to TRUE.
	    // If oap->end is on a NUL (empty line) oap->inclusive becomes
	    // FALSE.  This makes "d}P" and "v}dP" work the same.
	    जर (oap->motion_force == NUL || oap->motion_type == MLINE)
		oap->inclusive = TRUE;
	    जर (VIsual_mode == 'V')
		oap->motion_type = MLINE;
	    अन्यथा
	    {
		oap->motion_type = MCHAR;
		जर (VIsual_mode != Ctrl_V && *ml_get_pos(&(oap->end)) == NUL
			&& (include_ओळ_break || !virtual_op))
		{
		    oap->inclusive = FALSE;
		    // Try to include the newline, unless it's an operator
		    // that works on lines only.
		    जर (*p_sel != 'o'
			    && !op_on_ओळी(oap->op_type)
			    && oap->end.lnum < curbuf->b_ml.ml_ओळ_मोजणी)
		    {
			++oap->end.lnum;
			oap->end.col = 0;
			oap->end.coladd = 0;
			++oap->ओळ_मोजणी;
		    }
		}
	    }

	    redo_VIsual_busy = FALSE;

	    // Switch Visual off now, so screen updating does
	    // not show inverted text when the screen is redrawn.
	    // With OP_YANK and sometimes with OP_COLON and OP_FILTER there is
	    // no screen redraw, so it is done here to remove the inverted
	    // part.
	    जर (!gui_yank)
	    {
		VIsual_active = FALSE;
		setmouse();
		mouse_dragging = 0;
		may_clear_cmdline();
		जर ((oap->op_type == OP_YANK
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
	जर (has_mbyte && oap->inclusive)
	{
	    पूर्णांक		l;

	    l = (*mb_ptr2len)(ml_get_pos(&oap->end));
	    जर (l > 1)
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
	जर (oap->is_VIsual && (oap->empty || !curbuf->b_p_ma
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
	जर (	   oap->motion_type == MCHAR
		&& oap->inclusive == FALSE
		&& !(cap->retval & CA_NO_ADJ_OP_END)
		&& oap->end.col == 0
		&& (!oap->is_VIsual || *p_sel == 'o')
		&& !oap->block_mode
		&& oap->ओळ_मोजणी > 1)
	{
	    oap->end_adjusted = TRUE;	    // remember that we did this
	    --oap->ओळ_मोजणी;
	    --oap->end.lnum;
	    जर (inindent(0))
		oap->motion_type = MLINE;
	    अन्यथा
	    {
		oap->end.col = ml_get_len(oap->end.lnum);
		जर (oap->end.col)
		{
		    --oap->end.col;
		    oap->inclusive = TRUE;
		}
	    }
	}
	अन्यथा
	    oap->end_adjusted = FALSE;

	निवड (oap->op_type)
	{
	स्थिती OP_LSHIFT:
	स्थिती OP_RSHIFT:
	    op_shift(oap, TRUE, oap->is_VIsual ? (पूर्णांक)cap->count1 : 1);
	    auto_format(FALSE, TRUE);
	    थांबा;

	स्थिती OP_JOIN_NS:
	स्थिती OP_JOIN:
	    जर (oap->ओळ_मोजणी < 2)
		oap->ओळ_मोजणी = 2;
	    जर (curwin->w_cursor.lnum + oap->ओळ_मोजणी - 1 >
						   curbuf->b_ml.ml_ओळ_मोजणी)
		beep_flush();
	    अन्यथा
	    {
		(शून्य)do_join(oap->ओळ_मोजणी, oap->op_type == OP_JOIN,
							    TRUE, TRUE, TRUE);
		auto_format(FALSE, TRUE);
	    }
	    थांबा;

	स्थिती OP_DELETE:
	    VIsual_reselect = FALSE;	    // don't reselect now
	    जर (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    अन्यथा
	    {
		(शून्य)op_delete(oap);
		// save cursor line for undo if it wasn't saved yet
		जर (oap->motion_type == MLINE && has_format_option(FO_AUTO)
						      && u_save_cursor() == OK)
		    auto_format(FALSE, TRUE);
	    }
	    थांबा;

	स्थिती OP_YANK:
	    जर (empty_region_error)
	    {
		जर (!gui_yank)
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
	    थांबा;

	स्थिती OP_CHANGE:
	    VIsual_reselect = FALSE;	    // don't reselect now
	    जर (empty_region_error)
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
		जर (p_im || !किल्ली_Typed)
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

		जर (op_change(oap))	// will call edit()
		    cap->retval |= CA_COMMAND_BUSY;
		जर (restart_edit == 0)
		    restart_edit = restart_edit_save;
	    }
	    थांबा;

	स्थिती OP_FILTER:
	    जर (vim_strchr(p_cpo, CPO_FILTER) != रिक्त)
		AppendToRedobuff((char_u *)"!\r");  // use any last used !cmd
	    अन्यथा
		bangredo = TRUE;    // do_bang() will put cmd in redo buffer
	    // FALLTHROUGH

	स्थिती OP_INDENT:
	स्थिती OP_COLON:

	    // If 'equalprg' is empty, do the indenting internally.
	    जर (oap->op_type == OP_INDENT && *get_equalprg() == NUL)
	    {
		जर (curbuf->b_p_lisp)
		{
#ifdef FEAT_EVAL
		    जर (use_indentexpr_for_lisp())
			op_reindent(oap, get_अभिव्यक्ती_indent);
		    अन्यथा
#endif
			op_reindent(oap, get_lisp_indent);
		    थांबा;
		}
		op_reindent(oap,
#ifdef FEAT_EVAL
			*curbuf->b_p_inde != NUL ? get_अभिव्यक्ती_indent :
#endif
			    get_c_indent);
		थांबा;
	    }

	    op_colon(oap);
	    थांबा;

	स्थिती OP_TILDE:
	स्थिती OP_UPPER:
	स्थिती OP_LOWER:
	स्थिती OP_ROT13:
	    जर (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    अन्यथा
		op_tilde(oap);
	    check_cursor_col();
	    थांबा;

	स्थिती OP_FORMAT:
#जर defined(FEAT_EVAL)
	    जर (*curbuf->b_p_fex != NUL)
		op_formatexpr(oap);	// use expression
	    अन्यथा
#endif
	    {
		जर (*p_fp != NUL || *curbuf->b_p_fp != NUL)
		    op_colon(oap);		// use external command
		अन्यथा
		    op_format(oap, FALSE);	// use internal function
	    }
	    थांबा;
	स्थिती OP_FORMAT2:
	    op_format(oap, TRUE);	// use internal function
	    थांबा;

	स्थिती OP_FUNCTION:
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
		थांबा;
	    }

	स्थिती OP_घाला:
	स्थिती OP_APPEND:
	    VIsual_reselect = FALSE;	// don't reselect now
	    जर (empty_region_error)
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

		op_घाला(oap, cap->count1);
#ifdef FEAT_LINEBREAK
		// Reset linebreak, so that formatting works correctly.
		(शून्य)reset_lbr();
#endif

		// TODO: when inserting in several lines, should format all
		// the lines.
		auto_format(FALSE, TRUE);

		जर (restart_edit == 0)
		    restart_edit = restart_edit_save;
		अन्यथा
		    cap->retval |= CA_COMMAND_BUSY;
	    }
	    थांबा;

	स्थिती OP_REPLACE:
	    VIsual_reselect = FALSE;	// don't reselect now
	    जर (empty_region_error)
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
	    थांबा;

#ifdef FEAT_FOLDING
	स्थिती OP_FOLD:
	    VIsual_reselect = FALSE;	// don't reselect now
	    foldCreate(oap->start.lnum, oap->end.lnum);
	    थांबा;

	स्थिती OP_FOLDOPEN:
	स्थिती OP_FOLDOPENREC:
	स्थिती OP_FOLDCLOSE:
	स्थिती OP_FOLDCLOSEREC:
	    VIsual_reselect = FALSE;	// don't reselect now
	    opFoldRange(oap->start.lnum, oap->end.lnum,
		    oap->op_type == OP_FOLDOPEN
					    || oap->op_type == OP_FOLDOPENREC,
		    oap->op_type == OP_FOLDOPENREC
					  || oap->op_type == OP_FOLDCLOSEREC,
					  oap->is_VIsual);
	    थांबा;

	स्थिती OP_FOLDDEL:
	स्थिती OP_FOLDDELREC:
	    VIsual_reselect = FALSE;	// don't reselect now
	    deleteFold(oap->start.lnum, oap->end.lnum,
			       oap->op_type == OP_FOLDDELREC, oap->is_VIsual);
	    थांबा;
#endif
	स्थिती OP_NR_ADD:
	स्थिती OP_NR_SUB:
	    जर (empty_region_error)
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
	    थांबा;
	पूर्वनिर्धारित:
	    clearopbeep(oap);
	}
	virtual_op = MAYBE;
	जर (!gui_yank)
	{
	    // if 'sol' not set, go back to old column for some commands
	    जर (!p_sol && oap->motion_type == MLINE && !oap->end_adjusted
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
pbyte(pos_T lp, पूर्णांक c)
{
    char_u *p = ml_get_buf(curbuf, lp.lnum, TRUE);
    पूर्णांक	len = curbuf->b_ml.ml_ओळ_len;

    // safety check
    जर (lp.col >= len)
	lp.col = (len > 1 ? len - 2 : 0);
    *(p + lp.col) = c;
}
