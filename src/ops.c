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

#అన్తర్భావ "vim.h"

స్థిర శూన్య జరుపు_ఖండం(oparg_T *oap, పూర్ణాంకం amount);
స్థిర శూన్య	mb_adjust_opend(oparg_T *oap);
స్థిర పూర్ణాంకం	do_addsub(పూర్ణాంకం op_type, pos_T *pos, పూర్ణాంకం నిడివి, linenr_T Prenum1);
స్థిర శూన్య	pbyte(pos_T lp, పూర్ణాంకం c);
#నిర్వచించు PBYTE(lp, c) pbyte(lp, c)


// Flags for third item in "opchars".
#నిర్వచించు OPF_పంక్తులు  1	// operator always works on lines
#నిర్వచించు OPF_CHANGE 2	// operator changes text

/*
 * The names of operators.
 * IMPORTANT: Index must correspond with defines in vim.h!!!
 * The third field holds OPF_ flags.
 */
స్థిర స్థిరరాశి అక్షరం opchars[][3] =
{
    {NUL, NUL, 0},			// OP_NOP
    {'d', NUL, OPF_CHANGE},		// OP_DELETE
    {'y', NUL, 0},			// OP_YANK
    {'c', NUL, OPF_CHANGE},		// OP_CHANGE
    {'<', NUL, OPF_పంక్తులు | OPF_CHANGE},	// OP_LSHIFT
    {'>', NUL, OPF_పంక్తులు | OPF_CHANGE},	// OP_RSHIFT
    {'!', NUL, OPF_పంక్తులు | OPF_CHANGE},	// OP_FILTER
    {'g', '~', OPF_CHANGE},		// OP_TILDE
    {'=', NUL, OPF_పంక్తులు | OPF_CHANGE},	// OP_INDENT
    {'g', 'q', OPF_పంక్తులు | OPF_CHANGE},	// OP_FORMAT
    {':', NUL, OPF_పంక్తులు},		// OP_COLON
    {'g', 'U', OPF_CHANGE},		// OP_UPPER
    {'g', 'u', OPF_CHANGE},		// OP_LOWER
    {'J', NUL, OPF_పంక్తులు | OPF_CHANGE},	// DO_JOIN
    {'g', 'J', OPF_పంక్తులు | OPF_CHANGE},	// DO_JOIN_NS
    {'g', '?', OPF_CHANGE},		// OP_ROT13
    {'r', NUL, OPF_CHANGE},		// OP_REPLACE
    {'I', NUL, OPF_CHANGE},		// OP_INSERT
    {'A', NUL, OPF_CHANGE},		// OP_APPEND
    {'z', 'f', OPF_పంక్తులు},		// OP_FOLD
    {'z', 'o', OPF_పంక్తులు},		// OP_FOLDOPEN
    {'z', 'O', OPF_పంక్తులు},		// OP_FOLDOPENREC
    {'z', 'c', OPF_పంక్తులు},		// OP_FOLDCLOSE
    {'z', 'C', OPF_పంక్తులు},		// OP_FOLDCLOSEREC
    {'z', 'd', OPF_పంక్తులు},		// OP_FOLDDEL
    {'z', 'D', OPF_పంక్తులు},		// OP_FOLDDELREC
    {'g', 'w', OPF_పంక్తులు | OPF_CHANGE},	// OP_FORMAT2
    {'g', '@', OPF_CHANGE},		// OP_FUNCTION
    {Ctrl_A, NUL, OPF_CHANGE},		// OP_NR_ADD
    {Ctrl_X, NUL, OPF_CHANGE},		// OP_NR_SUB
};

/*
 * Translate a command name into an operator type.
 * Must only be called with a valid operator name!
 */
    పూర్ణాంకం
get_op_type(పూర్ణాంకం char1, పూర్ణాంకం char2)
{
    పూర్ణాంకం		i;

    ఒకవేళ (char1 == 'r')		// ignore second character
	తిరిగి OP_REPLACE;
    ఒకవేళ (char1 == '~')		// when tilde is an operator
	తిరిగి OP_TILDE;
    ఒకవేళ (char1 == 'g' && char2 == Ctrl_A)	//కూడు
	తిరిగి OP_NR_కూడు;
    ఒకవేళ (char1 == 'g' && char2 == Ctrl_X)	//వ్యవకలనం
	తిరిగి OP_NR_SUB;
    ఒకవేళ (char1 == 'z' && char2 == 'y')	// OP_YANK
	తిరిగి OP_YANK;
    కొరకు (i = 0; ; ++i)
    {
	ఒకవేళ (opchars[i][0] == char1 && opchars[i][1] == char2)
	    ఆపు;
	ఒకవేళ (i == (పూర్ణాంకం)ARRAY_నిడివి(opchars) - 1)
	{
	    అంతర్గత_error("get_op_type()");
	    ఆపు;
	}
    }
    తిరిగి i;
}

/*
 * Return TRUE if operator "op" always works on whole lines.
 */
    స్థిర పూర్ణాంకం
op_on_పంక్తులు(పూర్ణాంకం op)
{
    తిరిగి opchars[op][2] & OPF_పంక్తులు;
}

#ఒకవేళ defined(FEAT_పని_CHANNEL)
/*
 * Return TRUE if operator "op" changes text.
 */
    పూర్ణాంకం
op_is_change(పూర్ణాంకం op)
{
    తిరిగి opchars[op][2] & OPF_CHANGE;
}
#అయితే_ముగింపు

/*
 * Get first operator command character.
 * Returns 'g' or 'z' if there is another command character.
 */
    పూర్ణాంకం
get_op_char(పూర్ణాంకం optype)
{
    తిరిగి opchars[optype][0];
}

/*
 * Get second operator command character.
 */
    పూర్ణాంకం
get_extra_op_char(పూర్ణాంకం optype)
{
    తిరిగి opchars[optype][1];
}

/*
 * op_shift - handle a shift operation
 */
    శూన్య
op_జరుపు(oparg_T *oap, పూర్ణాంకం curs_శిఖరం, పూర్ణాంకం amount)
{
    దీర్ఘ	    i;
    పూర్ణాంకం		    మొదటి_char;
    పూర్ణాంకం		    ఖండం_నిలువు = 0;

    ఒకవేళ (u_save((linenr_T)(oap->ప్రారంభం.lnum - 1),
				       (linenr_T)(oap->చివర.lnum + 1)) == FAIL)
	తిరిగి;

    ఒకవేళ (oap->ఖండం_బహుళకం)
	ఖండం_నిలువు = curwin->w_cursor.నిలువు;

    కొరకు (i = oap->పంక్తి_లెక్క; --i >= 0; )
    {
	మొదటి_char = *ml_get_curline();
	ఒకవేళ (మొదటి_char == NUL)				// empty line
	    curwin->w_cursor.నిలువు = 0;
	లేకపోతే ఒకవేళ (oap->ఖండం_బహుళకం)
	    జరుపు_ఖండం(oap, amount);
	లేకపోతే
	    // Move the line right if it doesn't start with '#', 'smartindent'
	    // isn't set or 'cindent' isn't set or '#' isn't in 'cino'.
	    ఒకవేళ (మొదటి_char != '#' || !preprocs_ఎడమ())
		జరుపు_పంక్తి(oap->op_type == OP_LSHIFT, p_sr, amount, FALSE);
	++curwin->w_cursor.lnum;
    }

    మారింది_పంక్తులు(oap->ప్రారంభం.lnum, 0, oap->చివర.lnum + 1, 0L);
    ఒకవేళ (oap->ఖండం_బహుళకం)
    {
	curwin->w_cursor.lnum = oap->ప్రారంభం.lnum;
	curwin->w_cursor.నిలువు = ఖండం_నిలువు;
    }
    లేకపోతే ఒకవేళ (curs_శిఖరం)	    // put cursor on first line, for ">>"
    {
	curwin->w_cursor.lnum = oap->ప్రారంభం.lnum;
	beginline(BL_SOL | BL_FIX);   // shift_line() may have set cursor.col
    }
    లేకపోతే
	--curwin->w_cursor.lnum;	// put cursor on last line, for ":>"

#నిర్వచితమైతే FEAT_FOLDING
    // The cursor line is not in a closed fold
    foldOpenCursor();
#అయితే_ముగింపు


    ఒకవేళ (oap->పంక్తి_లెక్క > p_report)
    {
	అక్షరం	    *op;
	అక్షరం	    *సందేశం_పంక్తి_single;
	అక్షరం	    *సందేశం_పంక్తి_plural;

	ఒకవేళ (oap->op_type == OP_RSHIFT)
	    op = ">";
	లేకపోతే
	    op = "<";
	సందేశం_పంక్తి_single = NGETTEXT("%ld line %sed %d time",
					     "%ld line %sed %d times", amount);
	సందేశం_పంక్తి_plural = NGETTEXT("%ld lines %sed %d time",
					    "%ld lines %sed %d times", amount);
	vim_snprintf((అక్షరం *)IObuff, IOSIZE,
		NGETTEXT(సందేశం_పంక్తి_single, సందేశం_పంక్తి_plural, oap->పంక్తి_లెక్క),
		oap->పంక్తి_లెక్క, op, amount);
	సందేశం_attr_keep((అక్షరం *)IObuff, 0, TRUE);
    }

    ఒకవేళ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_ప్రారంభం = oap->ప్రారంభం;
	curbuf->b_op_చివర.lnum = oap->చివర.lnum;
	curbuf->b_op_చివర.నిలువు = ml_get_len(oap->చివర.lnum);
	ఒకవేళ (curbuf->b_op_చివర.నిలువు > 0)
	    --curbuf->b_op_చివర.నిలువు;
    }
}

#నిర్వచితమైతే FEAT_VARTABS
/*
 * Return the tabstop width at the index of the variable tabstop array.  If an
 * index greater than the length of the array is given, the last tabstop width
 * in the array is returned.
 */
    స్థిర పూర్ణాంకం
get_vts(పూర్ణాంకం *vts_array, పూర్ణాంకం సూచీ)
{
    పూర్ణాంకం	ts;

    ఒకవేళ (సూచీ < 1)
	ts = 0;
    లేకపోతే ఒకవేళ (సూచీ <= vts_array[0])
	ts = vts_array[సూచీ];
    లేకపోతే
	ts = vts_array[vts_array[0]];

    తిరిగి ts;
}

/*
 * Return the sum of all the tabstops through the index-th.
 */
    స్థిర పూర్ణాంకం
get_vts_కూడిక(పూర్ణాంకం *vts_array, పూర్ణాంకం సూచీ)
{
    పూర్ణాంకం	కూడిక = 0;
    పూర్ణాంకం	i;

    // Perform the summation for indices within the actual array.
    కొరకు (i = 1; i <= సూచీ && i <= vts_array[0]; i++)
	కూడిక += vts_array[i];

    // Add tabstops whose indices exceed the actual array.
    ఒకవేళ (i <= సూచీ)
	కూడిక += vts_array[vts_array[0]] * (సూచీ - vts_array[0]);

    తిరిగి కూడిక;
}
#అయితే_ముగింపు

    స్థిర vimlong_T
get_new_sw_indent(
    పూర్ణాంకం		ఎడమ_2,		// TRUE if shift is to the left
    పూర్ణాంకం		విడత,		// TRUE if new indent is to be to a tabstop
    vimlong_T	amount,		// Number of shifts
    vimlong_T	sw_val)
{
    vimlong_T	లెక్క = get_indent();
    vimlong_T	i, j;

    ఒకవేళ (విడత)			// round off indent
    {
	i = trim_గమ్యం_int(లెక్క) / sw_val;	// number of 'shiftwidth' rounded down
	j = trim_గమ్యం_int(లెక్క) % sw_val;	// extra spaces
	ఒకవేళ (j && ఎడమ_2)		// first remove extra spaces
	    --amount;
	ఒకవేళ (ఎడమ_2)
	{
	    i -= amount;
	    ఒకవేళ (i < 0)
		i = 0;
	}
	లేకపోతే
	    i += amount;
	లెక్క = i * sw_val;
    }
    లేకపోతే			// original vi indent
    {
	ఒకవేళ (ఎడమ_2)
	{
	    లెక్క -= sw_val * amount;
	    ఒకవేళ (లెక్క < 0)
		లెక్క = 0;
	}
	లేకపోతే
	    లెక్క += sw_val * amount;
    }

    తిరిగి లెక్క;
}

#నిర్వచితమైతే FEAT_VARTABS
    స్థిర vimlong_T
get_new_vts_indent(
    పూర్ణాంకం	ఎడమ_2,			// TRUE if shift is to the left
    పూర్ణాంకం	విడత,			// TRUE if new indent is to be to a tabstop
    పూర్ణాంకం	amount,			// Number of shifts
    పూర్ణాంకం	*vts_array)
{
    vimlong_T	indent = get_indent();
    పూర్ణాంకం		vtsi = 0;
    పూర్ణాంకం		vts_indent = 0;
    పూర్ణాంకం		ts = 0;		// Silence uninitialized variable warning.
    పూర్ణాంకం		స్థానభ్రంశం;		// Extra indent spaces to the right of the
				// tabstop

    // Find the tabstop at or to the left of the current indent.
    అయితే (vts_indent <= indent)
    {
	vtsi++;
	ts = get_vts(vts_array, vtsi);
	vts_indent += ts;
    }
    vts_indent -= ts;
    vtsi--;

    స్థానభ్రంశం = indent - vts_indent;

    ఒకవేళ (విడత)
    {
	ఒకవేళ (ఎడమ_2)
	{
	    ఒకవేళ (స్థానభ్రంశం == 0)
		indent = get_vts_కూడిక(vts_array, vtsi - amount);
	    లేకపోతే
		indent = get_vts_కూడిక(vts_array, vtsi - (amount - 1));
	}
	లేకపోతే
	    indent = get_vts_కూడిక(vts_array, vtsi + amount);
    }
    లేకపోతే
    {
	ఒకవేళ (ఎడమ_2)
	{
	    ఒకవేళ (amount > vtsi)
		indent = 0;
	    లేకపోతే
		indent = get_vts_కూడిక(vts_array, vtsi - amount) + స్థానభ్రంశం;
	}
	లేకపోతే
	    indent = get_vts_కూడిక(vts_array, vtsi + amount) + స్థానభ్రంశం;
    }

    తిరిగి indent;
}
#అయితే_ముగింపు

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
    శూన్య
జరుపు_పంక్తి(
    పూర్ణాంకం	ఎడమ_2,			// TRUE if shift is to the left
    పూర్ణాంకం	విడత,			// TRUE if new indent is to be to a tabstop
    పూర్ణాంకం	amount,			// Number of shifts
    పూర్ణాంకం	call_మారింది_బైట్లు)	// call changed_bytes()
{
    vimlong_T	లెక్క;
    దీర్ఘ	sw_val = curbuf->b_p_sw;
    దీర్ఘ	ts_val = curbuf->b_p_ts;
#నిర్వచితమైతే FEAT_VARTABS
    పూర్ణాంకం		*vts_array = curbuf->b_p_vts_array;
#అయితే_ముగింపు

    ఒకవేళ (sw_val != 0)
	// 'shiftwidth' is not zero; use it as the shift size.
	లెక్క = get_new_sw_indent(ఎడమ_2, విడత, amount, sw_val);
    లేకపోతే
#నిర్వచితమైతే FEAT_VARTABS
	ఒకవేళ ((vts_array == ఖాళీ) || (vts_array[0] == 0))
#అయితే_ముగింపు
    {
	// 'shiftwidth is zero and 'vartabstop' is empty; use 'tabstop' as the
	// shift size.
	లెక్క = get_new_sw_indent(ఎడమ_2, విడత, amount, ts_val);
    }
#నిర్వచితమైతే FEAT_VARTABS
    లేకపోతే
    {
	// 'shiftwidth is zero and 'vartabstop' is defined; use 'vartabstop'
	// to determine the new indent.
	లెక్క = get_new_vts_indent(ఎడమ_2, విడత, amount, vts_array);
    }
#అయితే_ముగింపు

    // Set new indent
    ఒకవేళ (State & VREPLACE_FLAG)
	change_indent(INDENT_సమితి, trim_గమ్యం_int(లెక్క), FALSE, NUL, call_మారింది_బైట్లు);
    లేకపోతే
	(శూన్య)సమితి_indent(trim_గమ్యం_int(లెక్క), call_మారింది_బైట్లు ? SIN_మారింది : 0);
}

/*
 * Shift one line of the current block one shiftwidth right or left.
 * Leaves cursor on first character in block.
 */
    స్థిర శూన్య
జరుపు_ఖండం(oparg_T *oap, పూర్ణాంకం amount)
{
    పూర్ణాంకం			ఎడమ_2 = (oap->op_type == OP_LSHIFT);
    పూర్ణాంకం			oldstate = State;
    పూర్ణాంకం			మొత్తం;
    char_u		*newp, *oldp;
    size_t		newlen, oldlen;
    పూర్ణాంకం			oldcol = curwin->w_cursor.నిలువు;
    పూర్ణాంకం			sw_val = (పూర్ణాంకం)get_sw_విలువ_indent(curbuf, ఎడమ_2);
    పూర్ణాంకం			ts_val = (పూర్ణాంకం)curbuf->b_p_ts;
    నిర్మాణం ఖండం_def	bd;
    పూర్ణాంకం			incr;
    colnr_T		ws_vcol;
    పూర్ణాంకం			added;
    size_t		new_పంక్తి_len;	// the length of the line after the
					// block shift
#నిర్వచితమైతే FEAT_RIGHTLEFT
    పూర్ణాంకం			old_p_ri = p_ri;

    p_ri = 0;			// don't want revins in indent
#అయితే_ముగింపు

    State = బహుళకం_చొప్పించు;	// don't want MODE_REPLACE for State
    ఖండం_prep(oap, &bd, curwin->w_cursor.lnum, TRUE);
    ఒకవేళ (bd.is_short)
	తిరిగి;

    // total is number of screen columns to be inserted/removed
    మొత్తం = (పూర్ణాంకం)((గుర్తులేని)amount * (గుర్తులేని)sw_val);
    ఒకవేళ ((మొత్తం / sw_val) != amount)
	తిరిగి; // multiplication overflow

    oldp = ml_get_curline();
    oldlen = ml_get_curline_len();

    ఒకవేళ (!ఎడమ_2)
    {
	పూర్ణాంకం		tabs = 0, spaces = 0;
	chartabsize_T	cts;

	/*
	 *  1. Get start vcol
	 *  2. Total ws vcols
	 *  3. Divvy into TABs & spp
	 *  4. Construct new string
	 */
	మొత్తం += bd.pre_whitesp; // all virtual WS up to & incl a split TAB
	ws_vcol = bd.ప్రారంభం_vcol - bd.pre_whitesp;
	ఒకవేళ (bd.startspaces)
	{
	    ఒకవేళ (has_mbyte)
	    {
		ఒకవేళ ((*mb_ptr2len)(bd.textstart) == 1)
		    ++bd.textstart;
		లేకపోతే
		{
		    ws_vcol = 0;
		    bd.startspaces = 0;
		}
	    }
	    లేకపోతే
		++bd.textstart;
	}

	// TODO: is passing bd.textstart for start of the line OK?
	init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum,
				   bd.ప్రారంభం_vcol, bd.textstart, bd.textstart);
	కొరకు ( ; VIM_ISWHITE(*cts.cts_ptr); )
	{
	    incr = lbr_chartabsize_adv(&cts);
	    మొత్తం += incr;
	    cts.cts_vcol += incr;
	}
	bd.textstart = cts.cts_ptr;
	bd.ప్రారంభం_vcol = cts.cts_vcol;
	clear_chartabsize_arg(&cts);

	// OK, now total=all the VWS reqd, and textstart points at the 1st
	// non-ws char in the block.
#నిర్వచితమైతే FEAT_VARTABS
	ఒకవేళ (!curbuf->b_p_et)
	    tabstop_fromto(ws_vcol, ws_vcol + మొత్తం,
				ts_val, curbuf->b_p_vts_array, &tabs, &spaces);
	లేకపోతే
	    spaces = మొత్తం;
#లేకపోతే
	ఒకవేళ (!curbuf->b_p_et)
	    tabs = ((ws_vcol % ts_val) + మొత్తం) / ts_val; // number of tabs
	ఒకవేళ (tabs > 0)
	    spaces = ((ws_vcol % ts_val) + మొత్తం) % ts_val; // number of spp
	లేకపోతే
	    spaces = మొత్తం;
#అయితే_ముగింపు
	// if we're splitting a TAB, allow for it
	bd.textcol -= bd.pre_whitesp_c - (bd.startspaces != 0);

	new_పంక్తి_len = bd.textcol + tabs + spaces + (oldlen - (bd.textstart - oldp));
	newp = alloc(new_పంక్తి_len + 1);
	ఒకవేళ (newp == ఖాళీ)
	    తిరిగి;
	mch_memmove(newp, oldp, (size_t)bd.textcol);
	newlen = bd.textcol;
	vim_memset(newp + newlen, TAB, (size_t)tabs);
	newlen += tabs;
	vim_memset(newp + newlen, ' ', (size_t)spaces);
	STRCPY(newp + newlen + spaces, bd.textstart);
    }
    లేకపోతే //ఎడమ
    {
	colnr_T	    destination_నిలువు;	// column to which text in block will
					// be shifted
	char_u	    *verbatim_copy_చివర;	// end of the part of the line which is
					// copied verbatim
	colnr_T	    verbatim_copy_వెడల్పు;// the (displayed) width of this part
					// of line
	size_t	    fill;		// nr of spaces that replace a TAB
	size_t	    ఖండం_space_వెడల్పు;
	size_t	    జరుపు_amount;
	char_u	    *non_white = bd.textstart;
	colnr_T	    non_white_నిలువు;
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
	ఒకవేళ (bd.startspaces)
	    MB_PTR_ADV(non_white);

	// The character's column is in "bd.start_vcol".
	non_white_నిలువు = bd.ప్రారంభం_vcol;

	init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum,
				   non_white_నిలువు, bd.textstart, non_white);
	అయితే (VIM_ISWHITE(*cts.cts_ptr))
	{
	    incr = lbr_chartabsize_adv(&cts);
	    cts.cts_vcol += incr;
	}
	non_white_నిలువు = cts.cts_vcol;
	non_white = cts.cts_ptr;
	clear_chartabsize_arg(&cts);

	ఖండం_space_వెడల్పు = non_white_నిలువు - oap->ప్రారంభం_vcol;
	// We will shift by "total" or "block_space_width", whichever is less.
	జరుపు_amount = (ఖండం_space_వెడల్పు < (size_t)మొత్తం
					 ? ఖండం_space_వెడల్పు : (size_t)మొత్తం);

	// The column to which we will shift the text.
	destination_నిలువు = (colnr_T)(non_white_నిలువు - జరుపు_amount);

	// Now let's find out how much of the beginning of the line we can
	// reuse without modification.
	verbatim_copy_చివర = bd.textstart;
	verbatim_copy_వెడల్పు = bd.ప్రారంభం_vcol;

	// If "bd.startspaces" is set, "bd.textstart" points to the character
	// preceding the block. We have to subtract its width to obtain its
	// column number.
	ఒకవేళ (bd.startspaces)
	    verbatim_copy_వెడల్పు -= bd.ప్రారంభం_char_vcols;
	init_chartabsize_arg(&cts, curwin, 0, verbatim_copy_వెడల్పు,
					     bd.textstart, verbatim_copy_చివర);
	అయితే (cts.cts_vcol < destination_నిలువు)
	{
	    incr = lbr_chartabsize(&cts);
	    ఒకవేళ (cts.cts_vcol + incr > destination_నిలువు)
		ఆపు;
	    cts.cts_vcol += incr;
	    MB_PTR_ADV(cts.cts_ptr);
	}
	verbatim_copy_వెడల్పు = cts.cts_vcol;
	verbatim_copy_చివర = cts.cts_ptr;
	clear_chartabsize_arg(&cts);

	// If "destination_col" is different from the width of the initial
	// part of the line that will be copied, it means we encountered a tab
	// character, which we will have to partly replace with spaces.
	fill = destination_నిలువు - verbatim_copy_వెడల్పు;

	// The replacement line will consist of:
	// - the beginning of the original line up to "verbatim_copy_end",
	// - "fill" number of spaces,
	// - the rest of the line, pointed to by non_white.
	fixedlen = verbatim_copy_చివర - oldp;
	new_పంక్తి_len = fixedlen + fill + (oldlen - (non_white - oldp));

	newp = alloc(new_పంక్తి_len + 1);
	ఒకవేళ (newp == ఖాళీ)
	    తిరిగి;
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
    ఒకవేళ (new_పంక్తి_len >= oldlen)
	added = (పూర్ణాంకం)(new_పంక్తి_len - oldlen);
    లేకపోతే
	added = 0 - (పూర్ణాంకం)(oldlen - new_పంక్తి_len);
    inserted_బైట్లు(curwin->w_cursor.lnum, bd.textcol, added);
    State = oldstate;
    curwin->w_cursor.నిలువు = oldcol;
#నిర్వచితమైతే FEAT_RIGHTLEFT
    p_ri = old_p_ri;
#అయితే_ముగింపు
}

/*
 * Insert string "s" (b_insert ? before : after) block :AKelly
 * Caller must prepare for undo.
 */
    స్థిర శూన్య
ఖండం_చొప్పించు(
    oparg_T		*oap,
    char_u		*s,
    size_t		slen,
    పూర్ణాంకం			b_చొప్పించు,
    నిర్మాణం ఖండం_def	*bdp)
{
    పూర్ణాంకం		ts_val;
    పూర్ణాంకం		లెక్క = 0;	// extra spaces to replace a cut TAB
    పూర్ణాంకం		spaces = 0;	// non-zero if cutting a TAB
    colnr_T	స్థానభ్రంశం;		// pointer along new line
    colnr_T	startcol;	// column where insert starts
    char_u	*newp, *oldp;	// new, old lines
    linenr_T	lnum;		// loop var
    పూర్ణాంకం		oldstate = State;

    State = బహుళకం_చొప్పించు;	// don't want MODE_REPLACE for State

    కొరకు (lnum = oap->ప్రారంభం.lnum + 1; lnum <= oap->చివర.lnum; lnum++)
    {
	ఖండం_prep(oap, bdp, lnum, TRUE);
	ఒకవేళ (bdp->is_short && b_చొప్పించు)
	    కొనసాగు;	// OP_INSERT, line ends before block start

	oldp = ml_get(lnum);

	ఒకవేళ (b_చొప్పించు)
	{
	    ts_val = bdp->ప్రారంభం_char_vcols;
	    spaces = bdp->startspaces;
	    ఒకవేళ (spaces != 0)
		లెక్క = ts_val - 1; // we're cutting a TAB
	    స్థానభ్రంశం = bdp->textcol;
	}
	లేకపోతే // append
	{
	    ts_val = bdp->చివర_char_vcols;
	    ఒకవేళ (!bdp->is_short) // spaces = padding after block
	    {
		spaces = (bdp->endspaces ? ts_val - bdp->endspaces : 0);
		ఒకవేళ (spaces != 0)
		    లెక్క = ts_val - 1; // we're cutting a TAB
		స్థానభ్రంశం = bdp->textcol + bdp->textlen - (spaces != 0);
	    }
	    లేకపోతే // spaces = padding to block edge
	    {
		// if $ used, just append to EOL (ie spaces==0)
		ఒకవేళ (!bdp->is_MAX)
		    spaces = (oap->చివర_vcol - bdp->చివర_vcol) + 1;
		లెక్క = spaces;
		స్థానభ్రంశం = bdp->textcol + bdp->textlen;
	    }
	}

	ఒకవేళ (has_mbyte && spaces > 0)
	    // avoid copying part of a multi-byte character
	    స్థానభ్రంశం -= (*mb_తల_off)(oldp, oldp + స్థానభ్రంశం);

	ఒకవేళ (spaces < 0)  // can happen when the cursor was moved
	    spaces = 0;

	// Make sure the allocated size matches what is actually copied below.
	newp = alloc(ml_get_len(lnum) + spaces + slen
		    + (spaces > 0 && !bdp->is_short ? ts_val - spaces : 0)
								  + లెక్క + 1);
	ఒకవేళ (newp == ఖాళీ)
	    కొనసాగు;

	// copy up to shifted part
	mch_memmove(newp, oldp, (size_t)స్థానభ్రంశం);
	oldp += స్థానభ్రంశం;

	// insert pre-padding
	vim_memset(newp + స్థానభ్రంశం, ' ', (size_t)spaces);
	startcol = స్థానభ్రంశం + spaces;

	// copy the new text
	mch_memmove(newp + startcol, s, slen);
	స్థానభ్రంశం += (పూర్ణాంకం)slen;

	ఒకవేళ (spaces > 0 && !bdp->is_short)
	{
	    ఒకవేళ (*oldp == TAB)
	    {
		// insert post-padding
		vim_memset(newp + స్థానభ్రంశం + spaces, ' ',
						    (size_t)(ts_val - spaces));
		// we're splitting a TAB, don't copy it
		oldp++;
		// We allowed for that TAB, remember this now
		లెక్క++;
	    }
	    లేకపోతే
		// Not a TAB, no extra spaces
		లెక్క = spaces;
	}

	ఒకవేళ (spaces > 0)
	    స్థానభ్రంశం += లెక్క;
	STRCPY(newp + స్థానభ్రంశం, oldp);

	ml_replace(lnum, newp, FALSE);

	ఒకవేళ (b_చొప్పించు)
	    // correct any text properties
	    inserted_బైట్లు(lnum, startcol, (పూర్ణాంకం)slen);

	ఒకవేళ (lnum == oap->చివర.lnum)
	{
	    // Set "']" mark to the end of the block instead of the end of
	    // the insert in the first line.
	    curbuf->b_op_చివర.lnum = oap->చివర.lnum;
	    curbuf->b_op_చివర.నిలువు = స్థానభ్రంశం;
	    ఒకవేళ (curbuf->b_visual.vi_చివర.coladd)
	    {
		curbuf->b_visual.vi_చివర.నిలువు += curbuf->b_visual.vi_చివర.coladd;
		curbuf->b_visual.vi_చివర.coladd = 0;
	    }
	}
    } // for all lnum

    మారింది_పంక్తులు(oap->ప్రారంభం.lnum + 1, 0, oap->చివర.lnum + 1, 0L);

    State = oldstate;
}

/*
 * Handle a delete operation.
 *
 * Return FAIL if undo failed, OK otherwise.
 */
    పూర్ణాంకం
op_delete(oparg_T *oap)
{
    పూర్ణాంకం			n;
    linenr_T		lnum;
    char_u		*ptr;
    char_u		*newp, *oldp;
    నిర్మాణం ఖండం_def	bd;
    linenr_T		old_lcount = curbuf->b_ml.ml_పంక్తి_లెక్క;
    పూర్ణాంకం			did_yank = FALSE;

    ఒకవేళ (curbuf->b_ml.ml_flags & ML_ఖాళీలు)	    // nothing to do
	తిరిగి OK;

    // Nothing to delete, return here.	Do prepare undo, for op_change().
    ఒకవేళ (oap->ఖాళీలు_2)
	తిరిగి u_save_cursor();

    ఒకవేళ (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	తిరిగి FAIL;
    }

    ఒకవేళ (VIsual_select && oap->is_VIsual)
	// use register given with CTRL_R, defaults to zero
	oap->regname = VIsual_select_reg;

#నిర్వచితమైతే HAVE_CLIPMETHOD
    adjust_clip_reg(&oap->regname);
#అయితే_ముగింపు

    ఒకవేళ (has_mbyte)
	mb_adjust_opend(oap);

    /*
     * Imitate the strange Vi behaviour: If the delete spans more than one
     * line and motion_type == MCHAR and the result is a blank line, make the
     * delete linewise.  Don't do this for the change command or Visual mode.
     */
    ఒకవేళ (       oap->motion_type == MCHAR
	    && !oap->is_VIsual
	    && !oap->ఖండం_బహుళకం
	    && oap->పంక్తి_లెక్క > 1
	    && oap->motion_force == NUL
	    && (vim_strchr(p_cpo, CPO_మాట) != ఖాళీ)
	    && oap->op_type == OP_DELETE)
    {
	ptr = ml_get(oap->చివర.lnum) + oap->చివర.నిలువు;
	ఒకవేళ (*ptr != NUL)
	    ptr += oap->inclusive;
	ptr = skipwhite(ptr);
	ఒకవేళ (*ptr == NUL && inindent(0))
	    oap->motion_type = MLINE;
    }

    /*
     * Check for trying to delete (e.g. "D") in an empty line.
     * Note: For the change operator it is ok.
     */
    ఒకవేళ (       oap->motion_type == MCHAR
	    && oap->పంక్తి_లెక్క == 1
	    && oap->op_type == OP_DELETE
	    && *ml_get(oap->ప్రారంభం.lnum) == NUL)
    {
	/*
	 * It's an error to operate on an empty region, when 'E' included in
	 * 'cpoptions' (Vi compatible).
	 */
	ఒకవేళ (virtual_op)
	    // Virtual editing: Nothing gets deleted, but we set the '[ and ']
	    // marks as if it happened.
	    వెళ్ళు setmarks;
	ఒకవేళ (vim_strchr(p_cpo, CPO_EMPTYREGION) != ఖాళీ)
	    beep_flush();
	తిరిగి OK;
    }

    /*
     * Do a yank of whatever we're about to delete.
     * If a yank register was specified, put the deleted text into that
     * register.  For the black hole register '_' don't yank anything.
     */
    ఒకవేళ (oap->regname != '_')
    {
#నిర్వచితమైతే FEAT_CLIPBOARD_PROVIDER
	inc_clip_provider();
#అయితే_ముగింపు
	ఒకవేళ (oap->regname != 0)
	{
	    // check for read-only register
	    ఒకవేళ (!valid_yank_reg(oap->regname, TRUE))
	    {
		beep_flush();
#నిర్వచితమైతే FEAT_CLIPBOARD_PROVIDER
		dec_clip_provider();
#అయితే_ముగింపు
		తిరిగి OK;
	    }
	    get_yank_రిజిస్టరు(oap->regname, TRUE); // yank into specif'd reg.
	    ఒకవేళ (op_yank(oap, TRUE, FALSE) == OK)   // yank without message
		did_yank = TRUE;
	}
	లేకపోతే
	    reset_y_append(); // not appending to unnamed register

	/*
	 * Put deleted text into register 1 and shift number registers if the
	 * delete contains a line break, or when using a specific operator (Vi
	 * compatible)
	 */
	ఒకవేళ (oap->motion_type == MLINE || oap->పంక్తి_లెక్క > 1
							   || oap->use_reg_one)
	{
	    జరుపు_delete_registers();
	    ఒకవేళ (op_yank(oap, TRUE, FALSE) == OK)
		did_yank = TRUE;
	}

	// Yank into small delete register when no named register specified
	// and the delete is within one line.
	ఒకవేళ ((
#నిర్వచితమైతే HAVE_CLIPMETHOD
	    ((clip_unnamed & CLIP_UNNAMED) && oap->regname == '*') ||
	    ((clip_unnamed & CLIP_UNNAMED_PLUS) && oap->regname == '+') ||
#అయితే_ముగింపు
	    oap->regname == 0) && oap->motion_type != MLINE
						      && oap->పంక్తి_లెక్క == 1)
	{
	    oap->regname = '-';
	    get_yank_రిజిస్టరు(oap->regname, TRUE);
	    ఒకవేళ (op_yank(oap, TRUE, FALSE) == OK)
		did_yank = TRUE;
	    oap->regname = 0;
	}

	/*
	 * If there's too much stuff to fit in the yank register, then get a
	 * confirmation before doing the delete. This is crude, but simple.
	 * And it avoids doing a delete of something we can't put back if we
	 * want.
	 */
	ఒకవేళ (!did_yank)
	{
	    పూర్ణాంకం సందేశం_silent_save = సందేశం_silent;

	    సందేశం_silent = 0;	// must display the prompt
	    n = ask_yesno((char_u *)_("cannot yank; delete anyway"), TRUE);
	    సందేశం_silent = సందేశం_silent_save;
	    ఒకవేళ (n != 'y')
	    {
		emsg(_(e_command_aborted));
#నిర్వచితమైతే FEAT_CLIPBOARD_PROVIDER
		dec_clip_provider();
#అయితే_ముగింపు
		తిరిగి FAIL;
	    }
	}

#ఒకవేళ defined(FEAT_EVAL)
	ఒకవేళ (did_yank && has_textyankpost())
	    yank_do_autocmd(oap, get_y_current());
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#అయితే_ముగింపు
    }

    /*
     * block mode delete
     */
    ఒకవేళ (oap->ఖండం_బహుళకం)
    {
	ఒకవేళ (u_save((linenr_T)(oap->ప్రారంభం.lnum - 1),
			       (linenr_T)(oap->చివర.lnum + 1)) == FAIL)
	    తిరిగి FAIL;

	కొరకు (lnum = curwin->w_cursor.lnum; lnum <= oap->చివర.lnum; ++lnum)
	{
	    ఖండం_prep(oap, &bd, lnum, TRUE);
	    ఒకవేళ (bd.textlen == 0)	// nothing to delete
		కొనసాగు;

	    // Adjust cursor position for tab replaced by spaces and 'lbr'.
	    ఒకవేళ (lnum == curwin->w_cursor.lnum)
	    {
		curwin->w_cursor.నిలువు = bd.textcol + bd.startspaces;
		curwin->w_cursor.coladd = 0;
	    }

	    // "n" == number of chars deleted
	    // If we delete a TAB, it may be replaced by several characters.
	    // Thus the number of characters may increase!
	    n = bd.textlen - bd.startspaces - bd.endspaces;
	    oldp = ml_get(lnum);
	    newp = alloc(ml_get_len(lnum) + 1 - n);
	    ఒకవేళ (newp == ఖాళీ)
		కొనసాగు;
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

#నిర్వచితమైతే FEAT_PROP_POPUP
	    ఒకవేళ (curbuf->b_has_textprop && n != 0)
		adjust_prop_columns(lnum, bd.textcol, -n, 0);
#అయితే_ముగింపు
	}

	check_cursor_నిలువు();
	మారింది_పంక్తులు(curwin->w_cursor.lnum, curwin->w_cursor.నిలువు,
						       oap->చివర.lnum + 1, 0L);
	oap->పంక్తి_లెక్క = 0;	    // no lines deleted
    }
    లేకపోతే ఒకవేళ (oap->motion_type == MLINE)
    {
	ఒకవేళ (oap->op_type == OP_CHANGE)
	{
	    // Delete the lines except the first one.  Temporarily move the
	    // cursor to the next line.  Save the current line number, if the
	    // last line is deleted it may be changed.
	    ఒకవేళ (oap->పంక్తి_లెక్క > 1)
	    {
		lnum = curwin->w_cursor.lnum;
		++curwin->w_cursor.lnum;
		del_పంక్తులు((దీర్ఘ)(oap->పంక్తి_లెక్క - 1), TRUE);
		curwin->w_cursor.lnum = lnum;
	    }
	    ఒకవేళ (u_save_cursor() == FAIL)
		తిరిగి FAIL;
	    ఒకవేళ (curbuf->b_p_ai)		    // don't delete indent
	    {
		beginline(BL_WHITE);	    // cursor on first non-white
		did_ai = TRUE;		    // delete the indent when ESC hit
		ai_నిలువు = curwin->w_cursor.నిలువు;
	    }
	    లేకపోతే
		beginline(0);		    // cursor in column 0
	    truncate_పంక్తి(FALSE);   // delete the rest of the line,
				    // leaving cursor past last char in line
	    ఒకవేళ (oap->పంక్తి_లెక్క > 1)
		u_clearline();	    // "U" command not possible after "2cc"
	}
	లేకపోతే
	{
	    del_పంక్తులు(oap->పంక్తి_లెక్క, TRUE);
	    beginline(BL_WHITE | BL_FIX);
	    u_clearline();	// "U" command not possible after "dd"
	}
    }
    లేకపోతే
    {
	ఒకవేళ (virtual_op)
	{
	    పూర్ణాంకం		endcol = 0;

	    // For virtualedit: break the tabs that are partly included.
	    ఒకవేళ (gchar_pos(&oap->ప్రారంభం) == '\t')
	    {
		ఒకవేళ (u_save_cursor() == FAIL)	// save first line for undo
		    తిరిగి FAIL;
		ఒకవేళ (oap->పంక్తి_లెక్క == 1)
		    endcol = getviscol2(oap->చివర.నిలువు, oap->చివర.coladd);
		coladvance_force(getviscol2(oap->ప్రారంభం.నిలువు, oap->ప్రారంభం.coladd));
		oap->ప్రారంభం = curwin->w_cursor;
		ఒకవేళ (oap->పంక్తి_లెక్క == 1)
		{
		    coladvance(endcol);
		    oap->చివర.నిలువు = curwin->w_cursor.నిలువు;
		    oap->చివర.coladd = curwin->w_cursor.coladd;
		    curwin->w_cursor = oap->ప్రారంభం;
		}
	    }

	    // Break a tab only when it's included in the area.
	    ఒకవేళ (gchar_pos(&oap->చివర) == '\t'
				     && (పూర్ణాంకం)oap->చివర.coladd < oap->inclusive)
	    {
		// save last line for undo
		ఒకవేళ (u_save((linenr_T)(oap->చివర.lnum - 1),
				       (linenr_T)(oap->చివర.lnum + 1)) == FAIL)
		    తిరిగి FAIL;
		curwin->w_cursor = oap->చివర;
		coladvance_force(getviscol2(oap->చివర.నిలువు, oap->చివర.coladd));
		oap->చివర = curwin->w_cursor;
		curwin->w_cursor = oap->ప్రారంభం;
	    }
	    ఒకవేళ (has_mbyte)
		mb_adjust_opend(oap);
	}

	ఒకవేళ (oap->పంక్తి_లెక్క == 1)	// delete characters within one line
	{
	    ఒకవేళ (u_save_cursor() == FAIL)	// save line for undo
		తిరిగి FAIL;

	    // if 'cpoptions' contains '$', display '$' at end of change
	    ఒకవేళ (       vim_strchr(p_cpo, CPO_DOLLAR) != ఖాళీ
		    && oap->op_type == OP_CHANGE
		    && oap->చివర.lnum == curwin->w_cursor.lnum
		    && !oap->is_VIsual)
		display_dollar(oap->చివర.నిలువు - !oap->inclusive);

	    n = oap->చివర.నిలువు - oap->ప్రారంభం.నిలువు + 1 - !oap->inclusive;

	    ఒకవేళ (virtual_op)
	    {
		// fix up things for virtualedit-delete:
		// break the tabs which are going to get in our way
		పూర్ణాంకం		len = ml_get_curline_len();

		ఒకవేళ (oap->చివర.coladd != 0
			&& (పూర్ణాంకం)oap->చివర.నిలువు >= len - 1
			&& !(oap->ప్రారంభం.coladd && (పూర్ణాంకం)oap->చివర.నిలువు >= len - 1))
		    n++;
		// Delete at least one char (e.g, when on a control char).
		ఒకవేళ (n == 0 && oap->ప్రారంభం.coladd != oap->చివర.coladd)
		    n = 1;

		// When deleted a char in the line, reset coladd.
		ఒకవేళ (gchar_cursor() != NUL)
		    curwin->w_cursor.coladd = 0;
	    }
	    (శూన్య)del_బైట్లు((దీర్ఘ)n, !virtual_op,
			    oap->op_type == OP_DELETE && !oap->is_VIsual);
	}
	లేకపోతే				// delete characters between lines
	{
	    pos_T   curpos;

	    // save deleted and changed lines for undo
	    ఒకవేళ (u_save((linenr_T)(curwin->w_cursor.lnum - 1),
		 (linenr_T)(curwin->w_cursor.lnum + oap->పంక్తి_లెక్క)) == FAIL)
		తిరిగి FAIL;

	    truncate_పంక్తి(TRUE);	// delete from cursor to end of line

	    curpos = curwin->w_cursor;	// remember curwin->w_cursor
	    ++curwin->w_cursor.lnum;
	    del_పంక్తులు((దీర్ఘ)(oap->పంక్తి_లెక్క - 2), FALSE);

	    // delete from start of line until op_end
	    n = (oap->చివర.నిలువు + 1 - !oap->inclusive);
	    curwin->w_cursor.నిలువు = 0;
	    (శూన్య)del_బైట్లు((దీర్ఘ)n, !virtual_op,
			    oap->op_type == OP_DELETE && !oap->is_VIsual);
	    curwin->w_cursor = curpos;	// restore curwin->w_cursor
	    (శూన్య)do_join(2, FALSE, FALSE, FALSE, FALSE);
	}
	ఒకవేళ (oap->op_type == OP_DELETE)
	    auto_format(FALSE, TRUE);
    }

    msgmore(curbuf->b_ml.ml_పంక్తి_లెక్క - old_lcount);

setmarks:
    ఒకవేళ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	ఒకవేళ (oap->ఖండం_బహుళకం)
	{
	    curbuf->b_op_చివర.lnum = oap->చివర.lnum;
	    curbuf->b_op_చివర.నిలువు = oap->ప్రారంభం.నిలువు;
	}
	లేకపోతే
	    curbuf->b_op_చివర = oap->ప్రారంభం;
	curbuf->b_op_ప్రారంభం = oap->ప్రారంభం;
    }

    తిరిగి OK;
}

/*
 * Adjust end of operating area for ending on a multi-byte character.
 * Used for deletion.
 */
    స్థిర శూన్య
mb_adjust_opend(oparg_T *oap)
{
    char_u	*పంక్తి;
    char_u	*ptr;

    ఒకవేళ (!oap->inclusive)
	తిరిగి;

    పంక్తి = ml_get(oap->చివర.lnum);
    ptr = పంక్తి + oap->చివర.నిలువు;
    ఒకవేళ (*ptr != NUL)
    {
	ptr -= (*mb_తల_off)(పంక్తి, ptr);
	ptr += (*mb_ptr2len)(ptr) - 1;
	oap->చివర.నిలువు = ptr - పంక్తి;
    }
}

/*
 * Replace the character under the cursor with "c".
 * This takes care of multi-byte characters.
 */
    స్థిర శూన్య
replace_character(పూర్ణాంకం c)
{
    పూర్ణాంకం n = State;

    State = బహుళకం_REPLACE;
    ins_char(c);
    State = n;
    // Backup to the replaced character.
    dec_cursor();
}

/*
 * Replace a whole area with one character.
 */
    పూర్ణాంకం
op_replace(oparg_T *oap, పూర్ణాంకం c)
{
    పూర్ణాంకం			n, numc;
    పూర్ణాంకం			అంకె_chars;
    char_u		*newp, *oldp;
    size_t		newlen, oldlen;
    నిర్మాణం ఖండం_def	bd;
    char_u		*after_p = ఖాళీ;
    పూర్ణాంకం			had_ctrl_v_cr = FALSE;

    ఒకవేళ ((curbuf->b_ml.ml_flags & ML_ఖాళీలు ) || oap->ఖాళీలు_2)
	తిరిగి OK;	    // nothing to do

    ఒకవేళ (c == REPLACE_CR_NCHAR)
    {
	had_ctrl_v_cr = TRUE;
	c = CAR;
    }
    లేకపోతే ఒకవేళ (c == REPLACE_NL_NCHAR)
    {
	had_ctrl_v_cr = TRUE;
	c = NL;
    }

    ఒకవేళ (has_mbyte)
	mb_adjust_opend(oap);

    ఒకవేళ (u_save((linenr_T)(oap->ప్రారంభం.lnum - 1),
				       (linenr_T)(oap->చివర.lnum + 1)) == FAIL)
	తిరిగి FAIL;

    /*
     * block mode replace
     */
    ఒకవేళ (oap->ఖండం_బహుళకం)
    {
	bd.is_MAX = (curwin->w_curswant == MAXCOL);
	కొరకు ( ; curwin->w_cursor.lnum <= oap->చివర.lnum; ++curwin->w_cursor.lnum)
	{
	    curwin->w_cursor.నిలువు = 0;  // make sure cursor position is valid
	    ఖండం_prep(oap, &bd, curwin->w_cursor.lnum, TRUE);
	    ఒకవేళ (bd.textlen == 0 && (!virtual_op || bd.is_MAX))
		కొనసాగు;	    // nothing to replace

	    // n == number of extra chars required
	    // If we split a TAB, it may be replaced by several characters.
	    // Thus the number of characters may increase!
	    // If the range starts in virtual space, count the initial
	    // coladd offset as part of "startspaces"
	    ఒకవేళ (virtual_op && bd.is_short && *bd.textstart == NUL)
	    {
		pos_T vpos;

		vpos.lnum = curwin->w_cursor.lnum;
		getvpos(&vpos, oap->ప్రారంభం_vcol);
		bd.startspaces += vpos.coladd;
		n = bd.startspaces;
	    }
	    లేకపోతే
		// allow for pre spaces
		n = (bd.startspaces ? bd.ప్రారంభం_char_vcols - 1 : 0);

	    // allow for post spp
	    n += (bd.endspaces
		    && !bd.is_oneChar
		    && bd.చివర_char_vcols > 0) ? bd.చివర_char_vcols - 1 : 0;
	    // Figure out how many characters to replace.
	    numc = oap->చివర_vcol - oap->ప్రారంభం_vcol + 1;
	    ఒకవేళ (bd.is_short && (!virtual_op || bd.is_MAX))
		numc -= (oap->చివర_vcol - bd.చివర_vcol) + 1;

	    // A double-wide character can be replaced only up to half the
	    // times.
	    ఒకవేళ ((*mb_char2cells)(c) > 1)
	    {
		ఒకవేళ ((numc & 1) && !bd.is_short)
		{
		    ++bd.endspaces;
		    ++n;
		}
		numc = numc / 2;
	    }

	    // Compute bytes needed, move character count to num_chars.
	    అంకె_chars = numc;
	    numc *= (*mb_char2len)(c);
	    // oldlen includes textlen, so don't double count
	    n += numc - bd.textlen;

	    oldp = ml_get_curline();
	    oldlen = ml_get_curline_len();
	    newp = alloc(oldlen + 1 + n);
	    ఒకవేళ (newp == ఖాళీ)
		కొనసాగు;
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
	    ఒకవేళ (had_ctrl_v_cr || (c != '\r' && c != '\n'))
	    {
		ఒకవేళ (has_mbyte)
		{
		    అయితే (--అంకె_chars >= 0)
			newlen += (*mb_char2bytes)(c, newp + newlen);
		}
		లేకపోతే
		{
		    vim_memset(newp + newlen, c, (size_t)numc);
		    newlen += numc;
		}
		ఒకవేళ (!bd.is_short)
		{
		    // insert post-spaces
		    vim_memset(newp + newlen, ' ', (size_t)bd.endspaces);
		    // copy the part after the changed part
		    STRCPY(newp + newlen + bd.endspaces,
				oldp + bd.textcol + bd.textlen);
		}
	    }
	    లేకపోతే
	    {
		// Replacing with \r or \n means splitting the line.
		after_p = alloc(oldlen + 1 + n - newlen);
		ఒకవేళ (after_p != ఖాళీ)
		    STRCPY(after_p, oldp + bd.textcol + bd.textlen);
	    }

	    // replace the line
	    ml_replace(curwin->w_cursor.lnum, newp, FALSE);
	    ఒకవేళ (after_p != ఖాళీ)
	    {
		ml_append(curwin->w_cursor.lnum++, after_p, 0, FALSE);
		appended_పంక్తులు_mark(curwin->w_cursor.lnum, 1L);
		oap->చివర.lnum++;
		vim_free(after_p);
	    }
	}
    }
    లేకపోతే
    {
	/*
	 * MCHAR and MLINE motion replace.
	 */
	ఒకవేళ (oap->motion_type == MLINE)
	{
	    oap->ప్రారంభం.నిలువు = 0;
	    curwin->w_cursor.నిలువు = 0;
	    oap->చివర.నిలువు = ml_get_len(oap->చివర.lnum);
	    ఒకవేళ (oap->చివర.నిలువు)
		--oap->చివర.నిలువు;
	}
	లేకపోతే ఒకవేళ (!oap->inclusive)
	    dec(&(oap->చివర));

	అయితే (LTOREQ_POS(curwin->w_cursor, oap->చివర))
	{
	    పూర్ణాంకం పూర్తి = FALSE;

	    n = gchar_cursor();
	    ఒకవేళ (n != NUL)
	    {
		పూర్ణాంకం new_byte_len = (*mb_char2len)(c);
		పూర్ణాంకం old_byte_len = mb_ptr2len(ml_get_cursor());

		ఒకవేళ (new_byte_len > 1 || old_byte_len > 1)
		{
		    // This is slow, but it handles replacing a single-byte
		    // with a multi-byte and the other way around.
		    ఒకవేళ (curwin->w_cursor.lnum == oap->చివర.lnum)
			oap->చివర.నిలువు += new_byte_len - old_byte_len;
		    replace_character(c);
		    పూర్తి = TRUE;
		}
		లేకపోతే
		{
		    ఒకవేళ (n == TAB)
		    {
			పూర్ణాంకం చివర_vcol = 0;

			ఒకవేళ (curwin->w_cursor.lnum == oap->చివర.lnum)
			{
			    // oap->end has to be recalculated when
			    // the tab breaks
			    చివర_vcol = getviscol2(oap->చివర.నిలువు,
							     oap->చివర.coladd);
			}
			coladvance_force(getviscol());
			ఒకవేళ (curwin->w_cursor.lnum == oap->చివర.lnum)
			    getvpos(&oap->చివర, చివర_vcol);
		    }
		    // with "coladd" set may move to just after a TAB
		    ఒకవేళ (gchar_cursor() != NUL)
		    {
			PBYTE(curwin->w_cursor, c);
			పూర్తి = TRUE;
		    }
		}
	    }
	    ఒకవేళ (!పూర్తి && virtual_op && curwin->w_cursor.lnum == oap->చివర.lnum)
	    {
		పూర్ణాంకం virtcols = oap->చివర.coladd;

		ఒకవేళ (curwin->w_cursor.lnum == oap->ప్రారంభం.lnum
			&& oap->ప్రారంభం.నిలువు == oap->చివర.నిలువు && oap->ప్రారంభం.coladd)
		    virtcols -= oap->ప్రారంభం.coladd;

		// oap->end has been trimmed so it's effectively inclusive;
		// as a result an extra +1 must be counted so we don't
		// trample the NUL byte.
		coladvance_force(getviscol2(oap->చివర.నిలువు, oap->చివర.coladd) + 1);
		curwin->w_cursor.నిలువు -= (virtcols + 1);
		కొరకు (; virtcols >= 0; virtcols--)
		{
		    ఒకవేళ ((*mb_char2len)(c) > 1)
		       replace_character(c);
		    లేకపోతే
			PBYTE(curwin->w_cursor, c);
		    ఒకవేళ (inc(&curwin->w_cursor) == -1)
			ఆపు;
		}
	    }

	    // Advance to next character, stop at the end of the file.
	    ఒకవేళ (inc_cursor() == -1)
		ఆపు;
	}
    }

    curwin->w_cursor = oap->ప్రారంభం;
    check_cursor();
    మారింది_పంక్తులు(oap->ప్రారంభం.lnum, oap->ప్రారంభం.నిలువు, oap->చివర.lnum + 1, 0L);

    ఒకవేళ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_ప్రారంభం = oap->ప్రారంభం;
	curbuf->b_op_చివర = oap->చివర;
    }

    తిరిగి OK;
}

స్థిర పూర్ణాంకం swapchars(పూర్ణాంకం op_type, pos_T *pos, పూర్ణాంకం నిడివి);

/*
 * Handle the (non-standard vi) tilde operator.  Also for "gu", "gU" and "g?".
 */
    స్థిర శూన్య
op_tilde(oparg_T *oap)
{
    pos_T		pos;
    నిర్మాణం ఖండం_def	bd;
    పూర్ణాంకం			did_change = FALSE;

    ఒకవేళ (u_save((linenr_T)(oap->ప్రారంభం.lnum - 1),
				       (linenr_T)(oap->చివర.lnum + 1)) == FAIL)
	తిరిగి;

    pos = oap->ప్రారంభం;
    ఒకవేళ (oap->ఖండం_బహుళకం)		    // Visual block mode
    {
	కొరకు (; pos.lnum <= oap->చివర.lnum; ++pos.lnum)
	{
	    పూర్ణాంకం one_change;

	    ఖండం_prep(oap, &bd, pos.lnum, FALSE);
	    pos.నిలువు = bd.textcol;
	    one_change = swapchars(oap->op_type, &pos, bd.textlen);
	    did_change |= one_change;

#నిర్వచితమైతే FEAT_NETBEANS_INTG
	    ఒకవేళ (netbeans_చురుకైన() && one_change)
	    {
		char_u *ptr;

		netbeans_removed(curbuf, pos.lnum, bd.textcol,
							    (దీర్ఘ)bd.textlen);
		// get the line now, it may have been flushed
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, bd.textcol,
						&ptr[bd.textcol], bd.textlen);
	    }
#అయితే_ముగింపు
	}
	ఒకవేళ (did_change)
	    మారింది_పంక్తులు(oap->ప్రారంభం.lnum, 0, oap->చివర.lnum + 1, 0L);
    }
    లేకపోతే				    // not block mode
    {
	ఒకవేళ (oap->motion_type == MLINE)
	{
	    oap->ప్రారంభం.నిలువు = 0;
	    pos.నిలువు = 0;
	    oap->చివర.నిలువు = ml_get_len(oap->చివర.lnum);
	    ఒకవేళ (oap->చివర.నిలువు)
		--oap->చివర.నిలువు;
	}
	లేకపోతే ఒకవేళ (!oap->inclusive)
	    dec(&(oap->చివర));

	ఒకవేళ (pos.lnum == oap->చివర.lnum)
	    did_change = swapchars(oap->op_type, &pos,
						  oap->చివర.నిలువు - pos.నిలువు + 1);
	లేకపోతే
	    కొరకు (;;)
	    {
		did_change |= swapchars(oap->op_type, &pos,
			pos.lnum == oap->చివర.lnum ? oap->చివర.నిలువు + 1
						  : ml_get_pos_len(&pos));
		ఒకవేళ (LTOREQ_POS(oap->చివర, pos) || inc(&pos) == -1)
		    ఆపు;
	    }
	ఒకవేళ (did_change)
	{
	    మారింది_పంక్తులు(oap->ప్రారంభం.lnum, oap->ప్రారంభం.నిలువు, oap->చివర.lnum + 1,
									  0L);
#నిర్వచితమైతే FEAT_NETBEANS_INTG
	    ఒకవేళ (netbeans_చురుకైన())
	    {
		char_u *ptr;
		పూర్ణాంకం లెక్క;

		pos = oap->ప్రారంభం;
		అయితే (pos.lnum < oap->చివర.lnum)
		{
		    ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		    లెక్క = ml_get_buf_len(curbuf, pos.lnum) - pos.నిలువు;
		    netbeans_removed(curbuf, pos.lnum, pos.నిలువు, (దీర్ఘ)లెక్క);
		    // get the line again, it may have been flushed
		    ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		    netbeans_inserted(curbuf, pos.lnum, pos.నిలువు,
							&ptr[pos.నిలువు], లెక్క);
		    pos.నిలువు = 0;
		    pos.lnum++;
		}
		లెక్క = oap->చివర.నిలువు - pos.నిలువు + 1;
		netbeans_removed(curbuf, pos.lnum, pos.నిలువు, (దీర్ఘ)లెక్క);
		// get the line again, it may have been flushed
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, pos.నిలువు,
							&ptr[pos.నిలువు], లెక్క);
	    }
#అయితే_ముగింపు
	}
    }

    ఒకవేళ (!did_change && oap->is_VIsual)
	// No change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    ఒకవేళ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set '[ and '] marks.
	curbuf->b_op_ప్రారంభం = oap->ప్రారంభం;
	curbuf->b_op_చివర = oap->చివర;
    }

    ఒకవేళ (oap->పంక్తి_లెక్క > p_report)
	smsg(NGETTEXT("%ld line changed", "%ld lines changed",
					    oap->పంక్తి_లెక్క), oap->పంక్తి_లెక్క);
}

/*
 * Invoke swapchar() on "length" bytes at position "pos".
 * "pos" is advanced to just after the changed characters.
 * "length" is rounded up to include the whole last multi-byte character.
 * Also works correctly when the number of bytes changes.
 * Returns TRUE if some character was changed.
 */
    స్థిర పూర్ణాంకం
swapchars(పూర్ణాంకం op_type, pos_T *pos, పూర్ణాంకం నిడివి)
{
    పూర్ణాంకం todo;
    పూర్ణాంకం	did_change = 0;

    కొరకు (todo = నిడివి; todo > 0; --todo)
    {
	ఒకవేళ (has_mbyte)
	{
	    పూర్ణాంకం len = (*mb_ptr2len)(ml_get_pos(pos));

	    // we're counting bytes, not characters
	    ఒకవేళ (len > 0)
		todo -= len - 1;
	}
	did_change |= swapchar(op_type, pos);
	ఒకవేళ (inc(pos) == -1)    // at end of file
	    ఆపు;
    }
    తిరిగి did_change;
}

/*
 * If op_type == OP_UPPER: make uppercase,
 * if op_type == OP_LOWER: make lowercase,
 * if op_type == OP_ROT13: do rot13 encoding,
 * else swap case of character at 'pos'
 * returns TRUE when something actually changed.
 */
    పూర్ణాంకం
swapchar(పూర్ణాంకం op_type, pos_T *pos)
{
    పూర్ణాంకం	    c;
    పూర్ణాంకం	    nc;

    c = gchar_pos(pos);

    // Only do rot13 encoding for ASCII characters.
    ఒకవేళ (c >= 0x80 && op_type == OP_ROT13)
	తిరిగి FALSE;

    // ~ is OP_NOP, g~ is OP_TILDE, gU is OP_UPPER
    ఒకవేళ ((op_type == OP_పై || op_type == OP_NOP || op_type == OP_TILDE)
	    && c == 0xdf
	    && (enc_latin1like || STRCMP(p_enc, "iso-8859-2") == 0))
    {
	pos_T   sp = curwin->w_cursor;

	// Special handling for lowercase German sharp s (ß): convert to uppercase (ẞ).
	curwin->w_cursor = *pos;
	del_char(FALSE);
	ins_char(0x1E9E);
	curwin->w_cursor = sp;
	తిరిగి TRUE;
    }

    ఒకవేళ (enc_dbcs != 0 && c >= 0x100)	// No lower/uppercase letter
	తిరిగి FALSE;
    nc = c;
    ఒకవేళ (MB_ISLOWER(c))
    {
	ఒకవేళ (op_type == OP_ROT13)
	    nc = ROT13(c, 'a');
	లేకపోతే ఒకవేళ (op_type != OP_క్రింది)
	    nc = MB_TOUPPER(c);
    }
    లేకపోతే ఒకవేళ (MB_ISUPPER(c))
    {
	ఒకవేళ (op_type == OP_ROT13)
	    nc = ROT13(c, 'A');
	లేకపోతే ఒకవేళ (op_type != OP_పై)
	    nc = MB_TOLOWER(c);
    }
    ఒకవేళ (nc != c)
    {
	ఒకవేళ (enc_utf8 && (c >= 0x80 || nc >= 0x80))
	{
	    pos_T   sp = curwin->w_cursor;

	    curwin->w_cursor = *pos;
	    // don't use del_char(), it also removes composing chars
	    del_బైట్లు(utf_ptr2len(ml_get_cursor()), FALSE, FALSE);
	    ins_char(nc);
	    curwin->w_cursor = sp;
	}
	లేకపోతే
	    PBYTE(*pos, nc);
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * op_insert - Insert and append operators for Visual mode.
 */
    శూన్య
op_చొప్పించు(oparg_T *oap, దీర్ఘ count1)
{
    దీర్ఘ		pre_textlen = 0;
    colnr_T		ind_pre_నిలువు = 0, ind_post_నిలువు;
    పూర్ణాంకం			ind_pre_vcol = 0, ind_post_vcol = 0;
    నిర్మాణం ఖండం_def	bd;
    పూర్ణాంకం			i;
    pos_T		t1;
    pos_T		ప్రారంభం_చొప్పించు;

    // edit() changes this - record it for OP_APPEND
    bd.is_MAX = (curwin->w_curswant == MAXCOL);

    // vis block is still marked. Get rid of it now.
    curwin->w_cursor.lnum = oap->ప్రారంభం.lnum;
    update_screen(UPD_INVERTED);

    ఒకవేళ (oap->ఖండం_బహుళకం)
    {
	// When 'virtualedit' is used, need to insert the extra spaces before
	// doing block_prep().  When only "block" is used, virtual edit is
	// already disabled, but still need it when calling
	// coladvance_force().
	// coladvance_force() uses get_ve_flags() to get the 'virtualedit'
	// state for the current window.  To override that state, we need to
	// set the window-local value of ve_flags rather than the global value.
	ఒకవేళ (curwin->w_cursor.coladd > 0)
	{
	    పూర్ణాంకం		old_ve_flags = curwin->w_ve_flags;

	    ఒకవేళ (u_save_cursor() == FAIL)
		తిరిగి;

	    curwin->w_ve_flags = VE_ALL;
	    coladvance_force(oap->op_type == OP_APPEND
					   ? oap->చివర_vcol + 1 : getviscol());
	    ఒకవేళ (oap->op_type == OP_APPEND)
		--curwin->w_cursor.నిలువు;
	    curwin->w_ve_flags = old_ve_flags;
	}
	// Get the info about the block before entering the text
	ఖండం_prep(oap, &bd, oap->ప్రారంభం.lnum, TRUE);
	// Get indent information
	ind_pre_నిలువు = (colnr_T)getwhitecols_curline();
	ind_pre_vcol = get_indent();
	pre_textlen = ml_get_len(oap->ప్రారంభం.lnum) - bd.textcol;
	ఒకవేళ (oap->op_type == OP_APPEND)
	    pre_textlen -= bd.textlen;
    }

    ఒకవేళ (oap->op_type == OP_APPEND)
    {
	ఒకవేళ (oap->ఖండం_బహుళకం && curwin->w_cursor.coladd == 0)
	{
	    // Move the cursor to the character right of the block.
	    curwin->w_సమితి_curswant = true;
	    అయితే (*ml_get_cursor() != NUL
		    && (curwin->w_cursor.నిలువు < bd.textcol + bd.textlen))
		++curwin->w_cursor.నిలువు;
	    ఒకవేళ (bd.is_short && !bd.is_MAX)
	    {
		// First line was too short, make it longer and adjust the
		// values in "bd".
		ఒకవేళ (u_save_cursor() == FAIL)
		    తిరిగి;
		కొరకు (i = 0; i < bd.endspaces; ++i)
		    ins_char(' ');
		bd.textlen += bd.endspaces;
	    }
	}
	లేకపోతే
	{
	    curwin->w_cursor = oap->చివర;
	    check_cursor_నిలువు();

	    // Works just like an 'i'nsert on the next character.
	    ఒకవేళ (!LINEEMPTY(curwin->w_cursor.lnum)
		    && oap->ప్రారంభం_vcol != oap->చివర_vcol)
		inc_cursor();
	}
    }

    t1 = oap->ప్రారంభం;
    ప్రారంభం_చొప్పించు = curwin->w_cursor;
    (శూన్య)edit(NUL, FALSE, (linenr_T)count1);

    // When a tab was inserted, and the characters in front of the tab
    // have been converted to a tab as well, the column of the cursor
    // might have actually been reduced, so need to adjust here.
    ఒకవేళ (t1.lnum == curbuf->b_op_ప్రారంభం_orig.lnum
	    && LT_POS(curbuf->b_op_ప్రారంభం_orig, t1))
	oap->ప్రారంభం = curbuf->b_op_ప్రారంభం_orig;

    // If user has moved off this line, we don't know what to do, so do
    // nothing.
    // Also don't repeat the insert when Insert mode ended with CTRL-C.
    ఒకవేళ (curwin->w_cursor.lnum != oap->ప్రారంభం.lnum || got_int)
	తిరిగి;

    ఒకవేళ (oap->ఖండం_బహుళకం)
    {
	పూర్ణాంకం			ins_len;
	char_u			*firstline, *ins_పాఠ్యం;
	నిర్మాణం ఖండం_def	bd2;
	పూర్ణాంకం			did_indent = FALSE;
	size_t			len;
	size_t			కూడు;
	// offset when cursor was moved in insert mode
	పూర్ణాంకం			స్థానభ్రంశం = 0;

	// If indent kicked in, the firstline might have changed
	// but only do that, if the indent actually increased.
	ind_post_నిలువు = (colnr_T)getwhitecols_curline();
	ఒకవేళ (curbuf->b_op_ప్రారంభం.నిలువు > ind_pre_నిలువు && ind_post_నిలువు > ind_pre_నిలువు)
	{
	    bd.textcol += ind_post_నిలువు - ind_pre_నిలువు;
	    ind_post_vcol = get_indent();
	    bd.ప్రారంభం_vcol += ind_post_vcol - ind_pre_vcol;
	    did_indent = TRUE;
	}

	// The user may have moved the cursor before inserting something, try
	// to adjust the block for that.  But only do it, if the difference
	// does not come from indent kicking in.
	ఒకవేళ (oap->ప్రారంభం.lnum == curbuf->b_op_ప్రారంభం_orig.lnum
						  && !bd.is_MAX && !did_indent)
	{
	    పూర్ణాంకం t = getviscol2(curbuf->b_op_ప్రారంభం_orig.నిలువు,
					       curbuf->b_op_ప్రారంభం_orig.coladd);

	    ఒకవేళ (oap->op_type == OP_చొప్పించు
		    && oap->ప్రారంభం.నిలువు + oap->ప్రారంభం.coladd
			    != curbuf->b_op_ప్రారంభం_orig.నిలువు
					      + curbuf->b_op_ప్రారంభం_orig.coladd)
	    {
		oap->ప్రారంభం.నిలువు = curbuf->b_op_ప్రారంభం_orig.నిలువు;
		pre_textlen -= t - oap->ప్రారంభం_vcol;
		oap->ప్రారంభం_vcol = t;
	    }
	    లేకపోతే ఒకవేళ (oap->op_type == OP_APPEND
		    && oap->ప్రారంభం.నిలువు + oap->ప్రారంభం.coladd
			    >= curbuf->b_op_ప్రారంభం_orig.నిలువు
					      + curbuf->b_op_ప్రారంభం_orig.coladd)
	    {
		oap->ప్రారంభం.నిలువు = curbuf->b_op_ప్రారంభం_orig.నిలువు;
		// reset pre_textlen to the value of OP_INSERT
		pre_textlen += bd.textlen;
		pre_textlen -= t - oap->ప్రారంభం_vcol;
		oap->ప్రారంభం_vcol = t;
		oap->op_type = OP_చొప్పించు;
	    }
	}

	// Spaces and tabs in the indent may have changed to other spaces and
	// tabs.  Get the starting column again and correct the length.
	// Don't do this when "$" used, end-of-line will have changed.
	//
	// if indent was added and the inserted text was after the indent,
	// correct the selection for the new indent.
	ఒకవేళ (did_indent && bd.textcol - ind_post_నిలువు > 0)
	{
	    oap->ప్రారంభం.నిలువు += ind_post_నిలువు - ind_pre_నిలువు;
	    oap->ప్రారంభం_vcol += ind_post_vcol - ind_pre_vcol;
	    oap->చివర.నిలువు += ind_post_నిలువు - ind_pre_నిలువు;
	    oap->చివర_vcol += ind_post_vcol - ind_pre_vcol;
	}
	ఖండం_prep(oap, &bd2, oap->ప్రారంభం.lnum, TRUE);
	ఒకవేళ (did_indent && bd.textcol - ind_post_నిలువు > 0)
	{
	    // undo for where "oap" is used below
	    oap->ప్రారంభం.నిలువు -= ind_post_నిలువు - ind_pre_నిలువు;
	    oap->ప్రారంభం_vcol -= ind_post_vcol - ind_pre_vcol;
	    oap->చివర.నిలువు -= ind_post_నిలువు - ind_pre_నిలువు;
	    oap->చివర_vcol -= ind_post_vcol - ind_pre_vcol;
	}
	ఒకవేళ (!bd.is_MAX || bd2.textlen < bd.textlen)
	{
	    ఒకవేళ (oap->op_type == OP_APPEND)
	    {
		pre_textlen += bd2.textlen - bd.textlen;
		ఒకవేళ (bd2.endspaces)
		    --bd2.textlen;
	    }
	    bd.textcol = bd2.textcol;
	    bd.textlen = bd2.textlen;
	}

	/*
	 * Subsequent calls to ml_get() flush the firstline data - take a
	 * copy of the required string.
	 */
	firstline = ml_get(oap->ప్రారంభం.lnum);
	len = ml_get_len(oap->ప్రారంభం.lnum);
	కూడు = bd.textcol;
	ఒకవేళ (oap->op_type == OP_APPEND)
	{
	    కూడు += bd.textlen;
	    // account for pressing cursor in insert mode when '$' was used
	    ఒకవేళ (bd.is_MAX
		&& (ప్రారంభం_చొప్పించు.lnum == Insstart.lnum
					   && ప్రారంభం_చొప్పించు.నిలువు > Insstart.నిలువు))
	    {
		స్థానభ్రంశం = (ప్రారంభం_చొప్పించు.నిలువు - Insstart.నిలువు);
		కూడు -= స్థానభ్రంశం;
		ఒకవేళ (oap->చివర_vcol > స్థానభ్రంశం)
		    oap->చివర_vcol -= (స్థానభ్రంశం + 1);
		లేకపోతే
		    // moved outside of the visual block, what to do?
		    తిరిగి;
	    }
	}
	ఒకవేళ (కూడు > len)
	    కూడు = len;  // short line, point to the NUL
	firstline += కూడు;
	len -= కూడు;
	ఒకవేళ (pre_textlen >= 0 && (ins_len = (పూర్ణాంకం)len - pre_textlen - స్థానభ్రంశం) > 0)
	{
	    ins_పాఠ్యం = vim_strnsave(firstline, ins_len);
	    ఒకవేళ (ins_పాఠ్యం != ఖాళీ)
	    {
		// block handled here
		ఒకవేళ (u_save(oap->ప్రారంభం.lnum,
					 (linenr_T)(oap->చివర.lnum + 1)) == OK)
		    ఖండం_చొప్పించు(oap, ins_పాఠ్యం, ins_len, (oap->op_type == OP_చొప్పించు),
									 &bd);

		curwin->w_cursor.నిలువు = oap->ప్రారంభం.నిలువు;
		check_cursor();
		vim_free(ins_పాఠ్యం);
	    }
	}
    }
}

/*
 * op_change - handle a change operation
 *
 * return TRUE if edit() returns because of a CTRL-O command
 */
    పూర్ణాంకం
op_change(oparg_T *oap)
{
    colnr_T		l;
    పూర్ణాంకం			retval;
    linenr_T		linenr;
    దీర్ఘ		pre_textlen = 0;
    దీర్ఘ		pre_indent = 0;
    char_u		*firstline;
    char_u		*ins_పాఠ్యం, *newp, *oldp;
    నిర్మాణం ఖండం_def	bd;

    l = oap->ప్రారంభం.నిలువు;
    ఒకవేళ (oap->motion_type == MLINE)
    {
	l = 0;
	can_si = may_do_si();	// Like opening a new line, do smart indent
    }

    // First delete the text in the region.  In an empty buffer only need to
    // save for undo
    ఒకవేళ (curbuf->b_ml.ml_flags & ML_ఖాళీలు)
    {
	ఒకవేళ (u_save_cursor() == FAIL)
	    తిరిగి FALSE;
    }
    లేకపోతే ఒకవేళ (op_delete(oap) == FAIL)
	తిరిగి FALSE;

    ఒకవేళ ((l > curwin->w_cursor.నిలువు) && !LINEEMPTY(curwin->w_cursor.lnum)
							 && !virtual_op)
	inc_cursor();

    // check for still on same line (<CR> in inserted text meaningless)
    // skip blank lines too
    ఒకవేళ (oap->ఖండం_బహుళకం)
    {
	// Add spaces before getting the current line length.
	ఒకవేళ (virtual_op && (curwin->w_cursor.coladd > 0
						    || gchar_cursor() == NUL))
	    coladvance_force(getviscol());
	firstline = ml_get(oap->ప్రారంభం.lnum);
	pre_textlen = ml_get_len(oap->ప్రారంభం.lnum);
	pre_indent = (దీర్ఘ)getwhitecols(firstline);
	bd.textcol = curwin->w_cursor.నిలువు;
    }

    ఒకవేళ (oap->motion_type == MLINE)
	fix_indent();

    // Reset finish_op now, don't want it set inside edit().
    పూర్ణాంకం save_ముగింపు_op = ముగింపు_op;
    ముగింపు_op = FALSE;

    retval = edit(NUL, FALSE, (linenr_T)1);

    ముగింపు_op = save_ముగింపు_op;

    /*
     * In Visual block mode, handle copying the new text to all lines of the
     * block.
     * Don't repeat the insert when Insert mode ended with CTRL-C.
     */
    ఒకవేళ (oap->ఖండం_బహుళకం && oap->ప్రారంభం.lnum != oap->చివర.lnum && !got_int)
    {
	పూర్ణాంకం	ins_len;

	// Auto-indenting may have changed the indent.  If the cursor was past
	// the indent, exclude that indent change from the inserted text.
	firstline = ml_get(oap->ప్రారంభం.lnum);
	ఒకవేళ (bd.textcol > (colnr_T)pre_indent)
	{
	    దీర్ఘ new_indent = (దీర్ఘ)getwhitecols(firstline);

	    pre_textlen += new_indent - pre_indent;
	    bd.textcol += new_indent - pre_indent;
	}

	ins_len = (పూర్ణాంకం)ml_get_len(oap->ప్రారంభం.lnum) - pre_textlen;
	ఒకవేళ (ins_len > 0)
	{
	    // Subsequent calls to ml_get() flush the firstline data - take a
	    // copy of the inserted text.
	    ఒకవేళ ((ins_పాఠ్యం = alloc(ins_len + 1)) != ఖాళీ)
	    {
		vim_strncpy(ins_పాఠ్యం, firstline + bd.textcol, ins_len);
		కొరకు (linenr = oap->ప్రారంభం.lnum + 1; linenr <= oap->చివర.lnum;
								     linenr++)
		{
		    ఖండం_prep(oap, &bd, linenr, TRUE);
		    ఒకవేళ (!bd.is_short || virtual_op)
		    {
			pos_T vpos;
			size_t newlen;

			// If the block starts in virtual space, count the
			// initial coladd offset as part of "startspaces"
			ఒకవేళ (bd.is_short)
			{
			    vpos.lnum = linenr;
			    (శూన్య)getvpos(&vpos, oap->ప్రారంభం_vcol);
			}
			లేకపోతే
			    vpos.coladd = 0;
			oldp = ml_get(linenr);
			newp = alloc(ml_get_len(linenr) + vpos.coladd + ins_len + 1);
			ఒకవేళ (newp == ఖాళీ)
			    కొనసాగు;
			// copy up to block start
			mch_memmove(newp, oldp, (size_t)bd.textcol);
			newlen = bd.textcol;
			vim_memset(newp + newlen, ' ', (size_t)vpos.coladd);
			newlen += vpos.coladd;
			mch_memmove(newp + newlen, ins_పాఠ్యం, ins_len);
			STRCPY(newp + newlen + ins_len, oldp + bd.textcol);
			ml_replace(linenr, newp, FALSE);
#నిర్వచితమైతే FEAT_PROP_POPUP
			// Shift the properties for linenr as edit() would do.
			ఒకవేళ (curbuf->b_has_textprop)
			    adjust_prop_columns(linenr, bd.textcol,
						     vpos.coladd + (పూర్ణాంకం)ins_len, 0);
#అయితే_ముగింపు
		    }
		}
		check_cursor();

		మారింది_పంక్తులు(oap->ప్రారంభం.lnum + 1, 0, oap->చివర.lnum + 1, 0L);
	    }
	    vim_free(ins_పాఠ్యం);
	}
    }
    auto_format(FALSE, TRUE);

    తిరిగి retval;
}

/*
 * When the cursor is on the NUL past the end of the line and it should not be
 * there move it left.
 */
    శూన్య
adjust_cursor_eol(శూన్య)
{
    గుర్తులేని పూర్ణాంకం cur_ve_flags = get_ve_flags();

    పూర్ణాంకం adj_cursor = (curwin->w_cursor.నిలువు > 0
				&& gchar_cursor() == NUL
				&& (cur_ve_flags & VE_ONEMORE) == 0
				&& (cur_ve_flags & VE_ALL) == 0
				&& !(restart_edit || (State & బహుళకం_చొప్పించు)));
    ఒకవేళ (!adj_cursor)
	తిరిగి;

    // Put the cursor on the last character in the line.
    dec_cursor();

    ఒకవేళ (cur_ve_flags == VE_ALL)
    {
	colnr_T	    scol, ecol;

	// Coladd is set to the width of the last character.
	getvcol(curwin, &curwin->w_cursor, &scol, ఖాళీ, &ecol, 0);
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
    char_u   *పంక్తి,
    పూర్ణాంకం      process,
    పూర్ణాంకం	     include_space,
    పూర్ణాంకం      *is_comment)
{
    char_u *comment_flags = ఖాళీ;
    పూర్ణాంకం    lead_len;
    పూర్ణాంకం    leader_స్థానభ్రంశం = get_చివరి_leader_స్థానభ్రంశం(పంక్తి, &comment_flags);

    *is_comment = FALSE;
    ఒకవేళ (leader_స్థానభ్రంశం != -1)
    {
	// Let's check whether the line ends with an unclosed comment.
	// If the last comment leader has COM_END in flags, there's no comment.
	అయితే (*comment_flags)
	{
	    ఒకవేళ (*comment_flags == COM_చివర
		    || *comment_flags == ':')
		ఆపు;
	    ++comment_flags;
	}
	ఒకవేళ (*comment_flags != COM_చివర)
	    *is_comment = TRUE;
    }

    ఒకవేళ (process == FALSE)
	తిరిగి పంక్తి;

    lead_len = get_leader_len(పంక్తి, &comment_flags, FALSE, include_space);

    ఒకవేళ (lead_len == 0)
	తిరిగి పంక్తి;

    // Find:
    // - COM_END,
    // - colon,
    // whichever comes first.
    అయితే (*comment_flags)
    {
	ఒకవేళ (*comment_flags == COM_చివర
		|| *comment_flags == ':')
	    ఆపు;
	++comment_flags;
    }

    // If we found a colon, it means that we are not processing a line
    // starting with a closing part of a three-part comment. That's good,
    // because we don't want to remove those as this would be annoying.
    ఒకవేళ (*comment_flags == ':' || *comment_flags == NUL)
	పంక్తి += lead_len;

    తిరిగి పంక్తి;
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
    పూర్ణాంకం
do_join(
    దీర్ఘ    లెక్క,
    పూర్ణాంకం	    చొప్పించు_space,
    పూర్ణాంకం	    save_undo,
    పూర్ణాంకం	    use_formatoptions వాడని,
    పూర్ణాంకం	    setmark)
{
    char_u	*curr = ఖాళీ;
    char_u      *curr_ప్రారంభం = ఖాళీ;
    char_u	*cend;
    char_u	*newp;
    size_t	newp_len;
    char_u	*spaces;	// number of spaces inserted before a line
    పూర్ణాంకం		endcurr1 = NUL;
    పూర్ణాంకం		endcurr2 = NUL;
    పూర్ణాంకం		currsize = 0;	// size of the current line
    పూర్ణాంకం		sumsize = 0;	// size of the long new line
    linenr_T	t;
    colnr_T	నిలువు = 0;
    పూర్ణాంకం		ret = OK;
    పూర్ణాంకం		*comments = ఖాళీ;
    పూర్ణాంకం		remove_comments = (use_formatoptions == TRUE)
				  && has_format_option(FO_REMOVE_COMS);
    పూర్ణాంకం		prev_was_comment;

    ఒకవేళ (save_undo && u_save((linenr_T)(curwin->w_cursor.lnum - 1),
			    (linenr_T)(curwin->w_cursor.lnum + లెక్క)) == FAIL)
	తిరిగి FAIL;

    // Allocate an array to store the number of spaces inserted before each
    // line.  We will use it to pre-compute the length of the new line and the
    // proper placement of each original line in the new one.
    spaces = lalloc_clear(లెక్క, TRUE);
    ఒకవేళ (spaces == ఖాళీ)
	తిరిగి FAIL;
    ఒకవేళ (remove_comments)
    {
	comments = lalloc_clear(లెక్క * పరిమాణం(పూర్ణాంకం), TRUE);
	ఒకవేళ (comments == ఖాళీ)
	{
	    vim_free(spaces);
	    తిరిగి FAIL;
	}
    }

    /*
     * Don't move anything yet, just compute the final line length
     * and setup the array of space strings lengths
     * This loops forward over the joined lines.
     */
    కొరకు (t = 0; t < లెక్క; ++t)
    {
	curr = curr_ప్రారంభం = ml_get((linenr_T)(curwin->w_cursor.lnum + t));
	ఒకవేళ (t == 0 && setmark && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set the '[ mark.
	    curwin->w_బఫరు->b_op_ప్రారంభం.lnum = curwin->w_cursor.lnum;
	    curwin->w_బఫరు->b_op_ప్రారంభం.నిలువు  = (colnr_T)STRLEN(curr);
	}
	ఒకవేళ (remove_comments)
	{
	    // We don't want to remove the comment leader if the
	    // previous line is not a comment.
	    ఒకవేళ (t > 0 && prev_was_comment)
	    {

		char_u *new_curr = skip_comment(curr, TRUE, చొప్పించు_space,
							   &prev_was_comment);
		comments[t] = (పూర్ణాంకం)(new_curr - curr);
		curr = new_curr;
	    }
	    లేకపోతే
		curr = skip_comment(curr, FALSE, చొప్పించు_space,
							   &prev_was_comment);
	}

	ఒకవేళ (చొప్పించు_space && t > 0)
	{
	    curr = skipwhite(curr);
	    ఒకవేళ (*curr != NUL && *curr != ')'
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
		ఒకవేళ (endcurr1 == ' ')
		    endcurr1 = endcurr2;
		లేకపోతే
		    ++spaces[t];
		// extra space when 'joinspaces' set and line ends in '.'
		ఒకవేళ (       p_js
			&& (endcurr1 == '.'
			    || (vim_strchr(p_cpo, CPO_JOINSP) == ఖాళీ
				&& (endcurr1 == '?' || endcurr1 == '!'))))
		    ++spaces[t];
	    }
	}
	currsize = (పూర్ణాంకం)STRLEN(curr);
	sumsize += currsize + spaces[t];
	endcurr1 = endcurr2 = NUL;
	ఒకవేళ (చొప్పించు_space && currsize > 0)
	{
	    ఒకవేళ (has_mbyte)
	    {
		cend = curr + currsize;
		MB_PTR_వెనుక(curr, cend);
		endcurr1 = (*mb_ptr2char)(cend);
		ఒకవేళ (cend > curr)
		{
		    MB_PTR_వెనుక(curr, cend);
		    endcurr2 = (*mb_ptr2char)(cend);
		}
	    }
	    లేకపోతే
	    {
		endcurr1 = *(curr + currsize - 1);
		ఒకవేళ (currsize > 1)
		    endcurr2 = *(curr + currsize - 2);
	    }
	}
	పంక్తి_breakcheck();
	ఒకవేళ (got_int)
	{
	    ret = FAIL;
	    వెళ్ళు theend;
	}
    }

    // store the column position before last line
    నిలువు = sumsize - currsize - spaces[లెక్క - 1];

    // allocate the space for the new line
    newp_len = sumsize + 1;
    newp = alloc(newp_len);
    ఒకవేళ (newp == ఖాళీ)
    {
	ret = FAIL;
	వెళ్ళు theend;
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
#నిర్వచితమైతే FEAT_PROP_POPUP
    unpacked_memline_T um = um_open_at_no_props(
				curwin->w_బఫరు, curwin->w_cursor.lnum, 0);
    // um_open_at_no_props may have invalidated "curr".
    పూర్ణాంకం curr_off = (పూర్ణాంకం)(curr - curr_ప్రారంభం);

    curr = curr_ప్రారంభం = ml_get((linenr_T)(curwin->w_cursor.lnum + లెక్క - 1));
    curr += curr_off;
#అయితే_ముగింపు

    కొరకు (t = లెక్క - 1; ; --t)
    {
	పూర్ణాంకం spaces_removed;

	cend -= currsize;
	mch_memmove(cend, curr, (size_t)currsize);

	ఒకవేళ (spaces[t] > 0)
	{
	    cend -= spaces[t];
	    vim_memset(cend, ' ', (size_t)(spaces[t]));
	}

	// If deleting more spaces than adding, the cursor moves no more than
	// what is added if it is inside these spaces.
	spaces_removed = (curr - curr_ప్రారంభం) - spaces[t];

	mark_నిలువు_adjust(curwin->w_cursor.lnum + t, (colnr_T)0, -t,
			 (దీర్ఘ)(cend - newp - spaces_removed), spaces_removed);
#నిర్వచితమైతే FEAT_PROP_POPUP
	prepend_joined_props(&um, curwin->w_cursor.lnum + t,
		t == లెక్క - 1, (దీర్ఘ)(cend - newp), spaces_removed);
#అయితే_ముగింపు
	ఒకవేళ (t == 0)
	    ఆపు;
	curr = curr_ప్రారంభం = ml_get((linenr_T)(curwin->w_cursor.lnum + t - 1));
	ఒకవేళ (remove_comments)
	    curr += comments[t - 1];
	ఒకవేళ (చొప్పించు_space && t > 1)
	    curr = skipwhite(curr);
	currsize = (పూర్ణాంకం)STRLEN(curr);
    }

#నిర్వచితమైతే FEAT_PROP_POPUP
    ఒకవేళ (um.buf != ఖాళీ)
    {
	um_సమితి_పాఠ్యం(&um, newp);
	um_తిరగేయి_props(&um);
	um_close(&um);
	newp = ఖాళీ;  // um_set_text took ownership
    }
    లేకపోతే
#అయితే_ముగింపు
    ml_replace_len(curwin->w_cursor.lnum, newp, (colnr_T)newp_len, TRUE, FALSE);

    ఒకవేళ (setmark && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set the '] mark.
	curwin->w_బఫరు->b_op_చివర.lnum = curwin->w_cursor.lnum;
	curwin->w_బఫరు->b_op_చివర.నిలువు  = (colnr_T)sumsize;
    }

    // Only report the change in the first line here, del_lines() will report
    // the deleted line.
    మారింది_పంక్తులు(curwin->w_cursor.lnum, currsize,
					       curwin->w_cursor.lnum + 1, 0L);
    /*
     * Delete following lines. To do this we move the cursor there
     * briefly, and then move it back. After del_lines() the cursor may
     * have moved up (last line deleted), so the current lnum is kept in t.
     */
    t = curwin->w_cursor.lnum;
    ++curwin->w_cursor.lnum;
    del_పంక్తులు(లెక్క - 1, FALSE);
    curwin->w_cursor.lnum = t;

    /*
     * Set the cursor column:
     * Vi compatible: use the column of the first join
     * vim:	      use the column of the last join
     */
    curwin->w_cursor.నిలువు =
		    (vim_strchr(p_cpo, CPO_JOINCOL) != ఖాళీ ? currsize : నిలువు);
    check_cursor_నిలువు();

    curwin->w_cursor.coladd = 0;
    curwin->w_సమితి_curswant = true;

theend:
    vim_free(spaces);
    ఒకవేళ (remove_comments)
	vim_free(comments);
    తిరిగి ret;
}

#నిర్వచితమైతే FEAT_LINEBREAK
/*
 * TODO: consider using "tailp" of win_lbr_chartabsize() instead of temporarily
 * resetting 'linebreak'.
 *
 * Reset 'linebreak' and take care of side effects.
 * Returns the previous value, to be passed to restore_lbr().
 */
    పూర్ణాంకం
reset_lbr(శూన్య)
{
    ఒకవేళ (!curwin->w_p_lbr)
	తిరిగి FALSE;
    // changing 'linebreak' may require w_virtcol to be updated
    curwin->w_p_lbr = FALSE;
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);
    తిరిగి TRUE;
}

/*
 * Restore 'linebreak' and take care of side effects.
 */
    శూన్య
restore_lbr(పూర్ణాంకం lbr_saved)
{
    ఒకవేళ (curwin->w_p_lbr || !lbr_saved)
	తిరిగి;

    // changing 'linebreak' may require w_virtcol to be updated
    curwin->w_p_lbr = TRUE;
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);
}
#అయితే_ముగింపు

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
    శూన్య
ఖండం_prep(
    oparg_T		*oap,
    నిర్మాణం ఖండం_def	*bdp,
    linenr_T		lnum,
    పూర్ణాంకం			is_del)
{
    పూర్ణాంకం		incr = 0;
    char_u	*pend;
    char_u	*pstart;
    char_u	*పంక్తి;
    char_u	*prev_pstart;
    char_u	*prev_pend;
    chartabsize_T cts;
#నిర్వచితమైతే FEAT_LINEBREAK
		// Avoid a problem with unwanted linebreaks in block mode.
    పూర్ణాంకం		lbr_saved = reset_lbr();
#అయితే_ముగింపు

    bdp->startspaces = 0;
    bdp->endspaces = 0;
    bdp->textlen = 0;
    bdp->ప్రారంభం_vcol = 0;
    bdp->చివర_vcol = 0;
    bdp->is_short = FALSE;
    bdp->is_oneChar = FALSE;
    bdp->pre_whitesp = 0;
    bdp->pre_whitesp_c = 0;
    bdp->చివర_char_vcols = 0;
    bdp->ప్రారంభం_char_vcols = 0;

    పంక్తి = ml_get(lnum);
    prev_pstart = పంక్తి;
    init_chartabsize_arg(&cts, curwin, lnum, bdp->ప్రారంభం_vcol, పంక్తి, పంక్తి);
    అయితే (cts.cts_vcol < oap->ప్రారంభం_vcol && *cts.cts_ptr != NUL)
    {
	// Count a tab for what it's worth (if list mode not on)
	incr = lbr_chartabsize(&cts);
	cts.cts_vcol += incr;
	ఒకవేళ (VIM_ISWHITE(*cts.cts_ptr))
	{
	    bdp->pre_whitesp += incr;
	    bdp->pre_whitesp_c++;
	}
	లేకపోతే
	{
	    bdp->pre_whitesp = 0;
	    bdp->pre_whitesp_c = 0;
	}
	prev_pstart = cts.cts_ptr;
	MB_PTR_ADV(cts.cts_ptr);
    }
    bdp->ప్రారంభం_vcol = cts.cts_vcol;
    pstart = cts.cts_ptr;
    clear_chartabsize_arg(&cts);

    bdp->ప్రారంభం_char_vcols = incr;
    ఒకవేళ (bdp->ప్రారంభం_vcol < oap->ప్రారంభం_vcol)	// line too short
    {
	bdp->చివర_vcol = bdp->ప్రారంభం_vcol;
	bdp->is_short = TRUE;
	ఒకవేళ (!is_del || oap->op_type == OP_APPEND)
	    bdp->endspaces = oap->చివర_vcol - oap->ప్రారంభం_vcol + 1;
    }
    లేకపోతే
    {
	// notice: this converts partly selected Multibyte characters to
	// spaces, too.
	bdp->startspaces = bdp->ప్రారంభం_vcol - oap->ప్రారంభం_vcol;
	ఒకవేళ (is_del && bdp->startspaces)
	    bdp->startspaces = bdp->ప్రారంభం_char_vcols - bdp->startspaces;
	pend = pstart;
	bdp->చివర_vcol = bdp->ప్రారంభం_vcol;
	ఒకవేళ (bdp->చివర_vcol > oap->చివర_vcol)	// it's all in one character
	{
	    bdp->is_oneChar = TRUE;
	    ఒకవేళ (oap->op_type == OP_చొప్పించు)
		bdp->endspaces = bdp->ప్రారంభం_char_vcols - bdp->startspaces;
	    లేకపోతే ఒకవేళ (oap->op_type == OP_APPEND)
	    {
		bdp->startspaces += oap->చివర_vcol - oap->ప్రారంభం_vcol + 1;
		bdp->endspaces = bdp->ప్రారంభం_char_vcols - bdp->startspaces;
	    }
	    లేకపోతే
	    {
		bdp->startspaces = oap->చివర_vcol - oap->ప్రారంభం_vcol + 1;
		ఒకవేళ (is_del && oap->op_type != OP_LSHIFT)
		{
		    // just putting the sum of those two into
		    // bdp->startspaces doesn't work for Visual replace,
		    // so we have to split the tab in two
		    bdp->startspaces = bdp->ప్రారంభం_char_vcols
					- (bdp->ప్రారంభం_vcol - oap->ప్రారంభం_vcol);
		    bdp->endspaces = bdp->చివర_vcol - oap->చివర_vcol - 1;
		}
	    }
	}
	లేకపోతే
	{
	    init_chartabsize_arg(&cts, curwin, lnum, bdp->చివర_vcol,
								  పంక్తి, pend);
	    prev_pend = pend;
	    అయితే (cts.cts_vcol <= oap->చివర_vcol && *cts.cts_ptr != NUL)
	    {
		// count a tab for what it's worth (if list mode not on)
		prev_pend = cts.cts_ptr;
		incr = lbr_chartabsize_adv(&cts);
		cts.cts_vcol += incr;
	    }
	    bdp->చివర_vcol = cts.cts_vcol;
	    pend = cts.cts_ptr;
	    clear_chartabsize_arg(&cts);

	    ఒకవేళ (bdp->చివర_vcol <= oap->చివర_vcol
		    && (!is_del
			|| oap->op_type == OP_APPEND
			|| oap->op_type == OP_REPLACE)) // line too short
	    {
		bdp->is_short = TRUE;
		// Alternative: include spaces to fill up the block.
		// Disadvantage: can lead to trailing spaces when the line is
		// short where the text is put
		// if (!is_del || oap->op_type == OP_APPEND)
		ఒకవేళ (oap->op_type == OP_APPEND || virtual_op)
		    bdp->endspaces = oap->చివర_vcol - bdp->చివర_vcol
							     + oap->inclusive;
		లేకపోతే
		    bdp->endspaces = 0; // replace doesn't add characters
	    }
	    లేకపోతే ఒకవేళ (bdp->చివర_vcol > oap->చివర_vcol)
	    {
		bdp->endspaces = bdp->చివర_vcol - oap->చివర_vcol - 1;
		ఒకవేళ (!is_del && bdp->endspaces)
		{
		    bdp->endspaces = incr - bdp->endspaces;
		    ఒకవేళ (pend != pstart)
			pend = prev_pend;
		}
	    }
	}
	bdp->చివర_char_vcols = incr;
	ఒకవేళ (is_del && bdp->startspaces)
	    pstart = prev_pstart;
	bdp->textlen = (పూర్ణాంకం)(pend - pstart);
    }
    bdp->textcol = (colnr_T) (pstart - పంక్తి);
    bdp->textstart = pstart;
#నిర్వచితమైతే FEAT_LINEBREAK
    restore_lbr(lbr_saved);
#అయితే_ముగింపు
}

/*
 * Get block text from "start" to "end"
 */
    శూన్య
charwise_ఖండం_prep(
    pos_T		ప్రారంభం,
    pos_T		చివర,
    నిర్మాణం ఖండం_def	*bdp,
    linenr_T		lnum,
    పూర్ణాంకం			inclusive)
{
    colnr_T startcol = 0, endcol = MAXCOL;
    colnr_T cs, ce;
    char_u *p;
    పూర్ణాంకం	plen = ml_get_len(lnum);

    p = ml_get(lnum);
    bdp->startspaces = 0;
    bdp->endspaces = 0;
    bdp->is_oneChar = FALSE;
    bdp->ప్రారంభం_char_vcols = 0;

    ఒకవేళ (lnum == ప్రారంభం.lnum)
    {
	startcol = ప్రారంభం.నిలువు;
	ఒకవేళ (virtual_op)
	{
	    getvcol(curwin, &ప్రారంభం, &cs, ఖాళీ, &ce, 0);
	    ఒకవేళ (ce != cs && ప్రారంభం.coladd > 0)
	    {
		// Part of a tab selected -- but don't
		// double-count it.
		bdp->ప్రారంభం_char_vcols = ce - cs + 1;
		bdp->startspaces = bdp->ప్రారంభం_char_vcols - ప్రారంభం.coladd;
		ఒకవేళ (bdp->startspaces < 0)
		    bdp->startspaces = 0;
		startcol++;
	    }
	}
    }

    ఒకవేళ (lnum == చివర.lnum)
    {
	endcol = చివర.నిలువు;
	ఒకవేళ (virtual_op)
	{
	    getvcol(curwin, &చివర, &cs, ఖాళీ, &ce, 0);
	    ఒకవేళ (p[endcol] == NUL || (cs + చివర.coladd < ce
			// Don't add space for double-wide
			// char; endcol will be on last byte
			// of multi-byte char.
			&& (*mb_తల_off)(p, p + endcol) == 0))
	    {
		ఒకవేళ (ప్రారంభం.lnum == చివర.lnum && ప్రారంభం.నిలువు == చివర.నిలువు)
		{
		    // Special case: inside a single char
		    bdp->is_oneChar = TRUE;
		    bdp->startspaces = చివర.coladd - ప్రారంభం.coladd + inclusive;
		    endcol = startcol;
		}
		లేకపోతే
		{
		    bdp->endspaces = చివర.coladd + inclusive;
		    endcol -= inclusive;
		}
	    }
	}
    }
    ఒకవేళ (endcol == MAXCOL)
	endcol = ml_get_len(lnum);
    ఒకవేళ (startcol > endcol || bdp->is_oneChar)
	bdp->textlen = 0;
    లేకపోతే
	bdp->textlen = endcol - startcol + inclusive;
    bdp->textcol = startcol;
    bdp->textstart = startcol <= plen ? p + startcol : p;
}

/*
 * Handle the add/subtract operator.
 */
    శూన్య
op_addsub(
    oparg_T	*oap,
    linenr_T	Prenum1,	    // Amount of add/subtract
    పూర్ణాంకం		g_cmd)		    // was g<c-a>/g<c-x>
{
    pos_T		pos;
    నిర్మాణం ఖండం_def	bd;
    పూర్ణాంకం			change_cnt = 0;
    linenr_T		amount = Prenum1;

    // do_addsub() might trigger re-evaluation of 'foldexpr' halfway, when the
    // buffer is not completely updated yet. Postpone updating folds until before
    // the call to changed_lines().
#నిర్వచితమైతే FEAT_FOLDING
    disable_fold_update++;
#అయితే_ముగింపు

    ఒకవేళ (!VIsual_చురుకైన)
    {
	pos = curwin->w_cursor;
	ఒకవేళ (u_save_cursor() == FAIL)
	{
#నిర్వచితమైతే FEAT_FOLDING
	    disable_fold_update--;
#అయితే_ముగింపు
	    తిరిగి;
	}
	change_cnt = do_addsub(oap->op_type, &pos, 0, amount);
#నిర్వచితమైతే FEAT_FOLDING
	disable_fold_update--;
#అయితే_ముగింపు
	ఒకవేళ (change_cnt)
	    మారింది_పంక్తులు(pos.lnum, 0, pos.lnum + 1, 0L);
    }
    లేకపోతే
    {
	పూర్ణాంకం	one_change;
	పూర్ణాంకం	నిడివి;
	pos_T	startpos;

	ఒకవేళ (u_save((linenr_T)(oap->ప్రారంభం.lnum - 1),
					(linenr_T)(oap->చివర.lnum + 1)) == FAIL)
	{
#నిర్వచితమైతే FEAT_FOLDING
	    disable_fold_update--;
#అయితే_ముగింపు
	    తిరిగి;
	}

	pos = oap->ప్రారంభం;
	కొరకు (; pos.lnum <= oap->చివర.lnum; ++pos.lnum)
	{
	    ఒకవేళ (oap->ఖండం_బహుళకం)		    // Visual block mode
	    {
		ఖండం_prep(oap, &bd, pos.lnum, FALSE);
		pos.నిలువు = bd.textcol;
		నిడివి = bd.textlen;
	    }
	    లేకపోతే ఒకవేళ (oap->motion_type == MLINE)
	    {
		curwin->w_cursor.నిలువు = 0;
		pos.నిలువు = 0;
		నిడివి = ml_get_len(pos.lnum);
	    }
	    లేకపోతే // oap->motion_type == MCHAR
	    {
		ఒకవేళ (pos.lnum == oap->ప్రారంభం.lnum && !oap->inclusive)
		    dec(&(oap->చివర));
		నిడివి = ml_get_len(pos.lnum);
		pos.నిలువు = 0;
		ఒకవేళ (pos.lnum == oap->ప్రారంభం.lnum)
		{
		    pos.నిలువు += oap->ప్రారంభం.నిలువు;
		    నిడివి -= oap->ప్రారంభం.నిలువు;
		}
		ఒకవేళ (pos.lnum == oap->చివర.lnum)
		{
		    నిడివి = ml_get_len(oap->చివర.lnum);
		    ఒకవేళ (oap->చివర.నిలువు >= నిడివి)
			oap->చివర.నిలువు = నిడివి - 1;
		    నిడివి = oap->చివర.నిలువు - pos.నిలువు + 1;
		}
	    }
	    one_change = do_addsub(oap->op_type, &pos, నిడివి, amount);
	    ఒకవేళ (one_change)
	    {
		// Remember the start position of the first change.
		ఒకవేళ (change_cnt == 0)
		    startpos = curbuf->b_op_ప్రారంభం;
		++change_cnt;
	    }

#నిర్వచితమైతే FEAT_NETBEANS_INTG
	    ఒకవేళ (netbeans_చురుకైన() && one_change)
	    {
		char_u *ptr;

		netbeans_removed(curbuf, pos.lnum, pos.నిలువు, (దీర్ఘ)నిడివి);
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, pos.నిలువు,
						&ptr[pos.నిలువు], నిడివి);
	    }
#అయితే_ముగింపు
	    ఒకవేళ (g_cmd && one_change)
		amount += Prenum1;
	}

#నిర్వచితమైతే FEAT_FOLDING
	disable_fold_update--;
#అయితే_ముగింపు
	ఒకవేళ (change_cnt)
	    మారింది_పంక్తులు(oap->ప్రారంభం.lnum, 0, oap->చివర.lnum + 1, 0L);

	ఒకవేళ (!change_cnt && oap->is_VIsual)
	    // No change: need to remove the Visual selection
	    redraw_curbuf_later(UPD_INVERTED);

	// Set '[ mark if something changed. Keep the last end
	// position from do_addsub().
	ఒకవేళ (change_cnt > 0 && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	    curbuf->b_op_ప్రారంభం = startpos;

	ఒకవేళ (change_cnt > p_report)
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
    స్థిర పూర్ణాంకం
do_addsub(
    పూర్ణాంకం		op_type,
    pos_T	*pos,
    పూర్ణాంకం		నిడివి,
    linenr_T	Prenum1)
{
    పూర్ణాంకం		నిలువు;
    పూర్ణాంకం		pre;		// 'X'/'x': hex; '0': octal; 'B'/'b': bin
    స్థిర పూర్ణాంకం	hexupper = FALSE;	// 0xABC
    uvarnumber_T	n;
    uvarnumber_T	oldn;
    char_u	*ptr;
    పూర్ణాంకం		linelen;
    పూర్ణాంకం		c;
    పూర్ణాంకం		todel;
    పూర్ణాంకం		do_hex;
    పూర్ణాంకం		do_oct;
    పూర్ణాంకం		do_bin;
    పూర్ణాంకం		do_alpha;
    పూర్ణాంకం		do_unsigned;
    పూర్ణాంకం		do_blank;
    పూర్ణాంకం		blank_unsigned = FALSE;	// blank: treat as unsigned?
    పూర్ణాంకం		firstdigit;
    పూర్ణాంకం		వ్యవకలనం;
    పూర్ణాంకం		negative = FALSE;
    పూర్ణాంకం		was_ధనాత్మక = TRUE;
    పూర్ణాంకం		visual = VIsual_చురుకైన;
    పూర్ణాంకం		did_change = FALSE;
    pos_T	save_cursor = curwin->w_cursor;
    పూర్ణాంకం		maxlen = 0;
    pos_T	startpos;
    pos_T	endpos;
    colnr_T	save_coladd = 0;

    do_hex = (vim_strchr(curbuf->b_p_nf, 'x') != ఖాళీ);	// "heX"
    do_oct = (vim_strchr(curbuf->b_p_nf, 'o') != ఖాళీ);	// "Octal"
    do_bin = (vim_strchr(curbuf->b_p_nf, 'b') != ఖాళీ);	// "Bin"
    do_alpha = (vim_strchr(curbuf->b_p_nf, 'p') != ఖాళీ);	// "alPha"
    do_unsigned = (vim_strchr(curbuf->b_p_nf, 'u') != ఖాళీ);	// "Unsigned"
    do_blank = (vim_strchr(curbuf->b_p_nf, 'k') != ఖాళీ);	// "blanK"

    ఒకవేళ (virtual_చురుకైన())
    {
	save_coladd = pos->coladd;
	pos->coladd = 0;
    }

    curwin->w_cursor = *pos;
    ptr = ml_get(pos->lnum);
    linelen = ml_get_len(pos->lnum);
    నిలువు = pos->నిలువు;

    ఒకవేళ (నిలువు + !!save_coladd >= linelen)
	వెళ్ళు theend;

    /*
     * First check if we are on a hexadecimal number, after the "0x".
     */
    ఒకవేళ (!VIsual_చురుకైన)
    {
	ఒకవేళ (do_bin)
	    అయితే (నిలువు > 0 && vim_isbdigit(ptr[నిలువు]))
	    {
		--నిలువు;
		ఒకవేళ (has_mbyte)
		    నిలువు -= (*mb_తల_off)(ptr, ptr + నిలువు);
	    }

	ఒకవేళ (do_hex)
	    అయితే (నిలువు > 0 && vim_isxdigit(ptr[నిలువు]))
	    {
		--నిలువు;
		ఒకవేళ (has_mbyte)
		    నిలువు -= (*mb_తల_off)(ptr, ptr + నిలువు);
	    }

	ఒకవేళ (       do_bin
		&& do_hex
		&& ! ((నిలువు > 0
		    && (ptr[నిలువు] == 'X'
			|| ptr[నిలువు] == 'x')
		    && ptr[నిలువు - 1] == '0'
		    && (!has_mbyte ||
			!(*mb_తల_off)(ptr, ptr + నిలువు - 1))
		    && vim_isxdigit(ptr[నిలువు + 1]))))
	{

	    // In case of binary/hexadecimal pattern overlap match, rescan

	    నిలువు = pos->నిలువు;

	    అయితే (నిలువు > 0 && vim_isdigit(ptr[నిలువు]))
	    {
		నిలువు--;
		ఒకవేళ (has_mbyte)
		    నిలువు -= (*mb_తల_off)(ptr, ptr + నిలువు);
	    }
	}

	ఒకవేళ ((       do_hex
		&& నిలువు > 0
		&& (ptr[నిలువు] == 'X'
		    || ptr[నిలువు] == 'x')
		&& ptr[నిలువు - 1] == '0'
		&& (!has_mbyte ||
		    !(*mb_తల_off)(ptr, ptr + నిలువు - 1))
		&& vim_isxdigit(ptr[నిలువు + 1])) ||
	    (       do_bin
		&& నిలువు > 0
		&& (ptr[నిలువు] == 'B'
		    || ptr[నిలువు] == 'b')
		&& ptr[నిలువు - 1] == '0'
		&& (!has_mbyte ||
		    !(*mb_తల_off)(ptr, ptr + నిలువు - 1))
		&& vim_isbdigit(ptr[నిలువు + 1])))
	{
	    // Found hexadecimal or binary number, move to its start.
	    --నిలువు;
	    ఒకవేళ (has_mbyte)
		నిలువు -= (*mb_తల_off)(ptr, ptr + నిలువు);
	}
	లేకపోతే
	{
	    /*
	     * Search forward and then backward to find the start of number.
	     */
	    నిలువు = pos->నిలువు;

	    అయితే (ptr[నిలువు] != NUL
		    && !vim_isdigit(ptr[నిలువు])
		    && !(do_alpha && ASCII_ISALPHA(ptr[నిలువు])))
		నిలువు += mb_ptr2len(ptr + నిలువు);

	    అయితే (నిలువు > 0
		    && vim_isdigit(ptr[నిలువు - 1])
		    && !(do_alpha && ASCII_ISALPHA(ptr[నిలువు])))
	    {
		--నిలువు;
		ఒకవేళ (has_mbyte)
		    నిలువు -= (*mb_తల_off)(ptr, ptr + నిలువు);
	    }
	}
    }

    ఒకవేళ (visual)
    {
	అయితే (ptr[నిలువు] != NUL && నిడివి > 0
		&& !vim_isdigit(ptr[నిలువు])
		&& !(do_alpha && ASCII_ISALPHA(ptr[నిలువు])))
	{
	    పూర్ణాంకం mb_len = mb_ptr2len(ptr + నిలువు);

	    నిలువు += mb_len;
	    నిడివి -= mb_len;
	}

	ఒకవేళ (నిడివి == 0)
	    వెళ్ళు theend;

	ఒకవేళ (నిలువు > pos->నిలువు && ptr[నిలువు - 1] == '-'
		&& (!has_mbyte || !(*mb_తల_off)(ptr, ptr + నిలువు - 1))
		&& !do_unsigned)
	{
	    ఒకవేళ (do_blank && నిలువు >= 2 && !VIM_ISWHITE(ptr[నిలువు - 2]))
		blank_unsigned = TRUE;
	    లేకపోతే
	    {
		negative = TRUE;
		was_ధనాత్మక = FALSE;
	    }
	}
    }

    /*
     * If a number was found, and saving for undo works, replace the number.
     */
    firstdigit = ptr[నిలువు];
    ఒకవేళ (!VIM_ISDIGIT(firstdigit) && !(do_alpha && ASCII_ISALPHA(firstdigit)))
    {
	beep_flush();
	వెళ్ళు theend;
    }

    ఒకవేళ (do_alpha && ASCII_ISALPHA(firstdigit))
    {
	// decrement or increment alphabetic character
	ఒకవేళ (op_type == OP_NR_SUB)
	{
	    ఒకవేళ (CharOrd(firstdigit) < Prenum1)
	    {
		ఒకవేళ (సురక్షిత_isupper(firstdigit))
		    firstdigit = 'A';
		లేకపోతే
		    firstdigit = 'a';
	    }
	    లేకపోతే
		firstdigit -= Prenum1;
	}
	లేకపోతే
	{
	    ఒకవేళ (26 - CharOrd(firstdigit) - 1 < Prenum1)
	    {
		ఒకవేళ (సురక్షిత_isupper(firstdigit))
		    firstdigit = 'Z';
		లేకపోతే
		    firstdigit = 'z';
	    }
	    లేకపోతే
		firstdigit += Prenum1;
	}
	curwin->w_cursor.నిలువు = నిలువు;
	ఒకవేళ (!did_change)
	    startpos = curwin->w_cursor;
	did_change = TRUE;
	(శూన్య)del_char(FALSE);
	ins_char(firstdigit);
	endpos = curwin->w_cursor;
	curwin->w_cursor.నిలువు = నిలువు;
    }
    లేకపోతే
    {
	char_u	*buf1;
	పూర్ణాంకం	buf1len;
	char_u	buf2[NUMBUFLEN];
	పూర్ణాంకం	buf2len;
	pos_T	save_pos;
	పూర్ణాంకం	i;

	ఒకవేళ (నిలువు > 0 && ptr[నిలువు - 1] == '-'
		&& (!has_mbyte ||
		    !(*mb_తల_off)(ptr, ptr + నిలువు - 1))
		&& !visual
		&& !do_unsigned)
	{
	    ఒకవేళ (do_blank && నిలువు >= 2 && !VIM_ISWHITE(ptr[నిలువు - 2]))
		blank_unsigned = TRUE;
	    లేకపోతే
	    {
		// negative number
		--నిలువు;
		negative = TRUE;
	    }
	}

	// get the number value (unsigned)
	ఒకవేళ (visual && VIsual_బహుళకం != 'V')
	    maxlen = (curbuf->b_visual.vi_curswant == MAXCOL
		    ? linelen - నిలువు : నిడివి);

	పూర్ణాంకం అధికప్రవాహం = FALSE;
	vim_str2nr(ptr + నిలువు, &pre, &నిడివి,
		0 + (do_bin ? STR2NR_BIN : 0)
		    + (do_oct ? STR2NR_OCT : 0)
		    + (do_hex ? STR2NR_HEX : 0),
		ఖాళీ, &n, maxlen, FALSE, &అధికప్రవాహం);

	// ignore leading '-' for hex and octal and bin numbers
	ఒకవేళ (pre && negative)
	{
	    ++నిలువు;
	    --నిడివి;
	    negative = FALSE;
	}
	// add or subtract
	వ్యవకలనం = FALSE;
	ఒకవేళ (op_type == OP_NR_SUB)
	    వ్యవకలనం ^= TRUE;
	ఒకవేళ (negative)
	    వ్యవకలనం ^= TRUE;

	oldn = n;
	ఒకవేళ (!అధికప్రవాహం)  // if number is too big don't add/subtract
	{
	    ఒకవేళ (వ్యవకలనం)
		n -= (uvarnumber_T)Prenum1;
	    లేకపోతే
		n += (uvarnumber_T)Prenum1;
	}

	// handle wraparound for decimal numbers
	ఒకవేళ (!pre)
	{
	    ఒకవేళ (వ్యవకలనం)
	    {
		ఒకవేళ (n > oldn)
		{
		    n = 1 + (n ^ (uvarnumber_T)-1);
		    negative ^= TRUE;
		}
	    }
	    లేకపోతే
	    {
		//కూడు
		ఒకవేళ (n < oldn)
		{
		    n = (n ^ (uvarnumber_T)-1);
		    negative ^= TRUE;
		}
	    }
	    ఒకవేళ (n == 0)
		negative = FALSE;
	}

	ఒకవేళ ((do_unsigned || blank_unsigned) && negative)
	{
	    ఒకవేళ (వ్యవకలనం)
		// sticking at zero.
		n = (uvarnumber_T)0;
	    లేకపోతే
		// sticking at 2^64 - 1.
		n = (uvarnumber_T)(-1);
	    negative = FALSE;
	}

	ఒకవేళ (visual && !was_ధనాత్మక && !negative && నిలువు > 0)
	{
	    // need to remove the '-'
	    నిలువు--;
	    నిడివి++;
	}

	/*
	 * Delete the old number.
	 */
	curwin->w_cursor.నిలువు = నిలువు;
	ఒకవేళ (!did_change)
	    startpos = curwin->w_cursor;
	did_change = TRUE;
	todel = నిడివి;
	c = gchar_cursor();
	/*
	 * Don't include the '-' in the length, only the length of the
	 * part after it is kept the same.
	 */
	ఒకవేళ (c == '-')
	    --నిడివి;

	save_pos = curwin->w_cursor;
	కొరకు (i = 0; i < todel; ++i)
	{
	    ఒకవేళ (c < 0x100 && సురక్షిత_isalpha(c))
	    {
		ఒకవేళ (సురక్షిత_isupper(c))
		    hexupper = TRUE;
		లేకపోతే
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
	buf1 = alloc(నిడివి + NUMBUFLEN);
	ఒకవేళ (buf1 == ఖాళీ)
	    వెళ్ళు theend;
	ptr = buf1;
	ఒకవేళ (negative && (!visual || was_ధనాత్మక))
	    *ptr++ = '-';
	ఒకవేళ (pre)
	{
	    *ptr++ = '0';
	    --నిడివి;
	}
	ఒకవేళ (pre == 'b' || pre == 'B' ||
	    pre == 'x' || pre == 'X')
	{
	    *ptr++ = pre;
	    --నిడివి;
	}

	/*
	 * Put the number characters in buf2[].
	 */
	ఒకవేళ (pre == 'b' || pre == 'B')
	{
	    పూర్ణాంకం బిట్టు = 0;
	    పూర్ణాంకం బిట్లు = పరిమాణం(uvarnumber_T) * 8;

	    // leading zeros
	    కొరకు (బిట్టు = బిట్లు; బిట్టు > 0; బిట్టు--)
		ఒకవేళ ((n >> (బిట్టు - 1)) & 0x1) ఆపు;

	    కొరకు (buf2len = 0; బిట్టు > 0 && buf2len < (NUMBUFLEN - 1); బిట్టు--)
		buf2[buf2len++] = ((n >> (బిట్టు - 1)) & 0x1) ? '1' : '0';

	    buf2[buf2len] = NUL;
	}
	లేకపోతే ఒకవేళ (pre == 0)
	    buf2len = vim_snprintf((అక్షరం *)buf2, NUMBUFLEN, "%llu", n);
	లేకపోతే ఒకవేళ (pre == '0')
	    buf2len = vim_snprintf((అక్షరం *)buf2, NUMBUFLEN, "%llo", n);
	లేకపోతే ఒకవేళ (pre && hexupper)
	    buf2len = vim_snprintf((అక్షరం *)buf2, NUMBUFLEN, "%llX", n);
	లేకపోతే
	    buf2len = vim_snprintf((అక్షరం *)buf2, NUMBUFLEN, "%llx", n);
	నిడివి -= buf2len;

	/*
	 * Adjust number of zeros to the new number of digits, so the
	 * total length of the number remains the same.
	 * Don't do this when
	 * the result may look like an octal number.
	 */
	ఒకవేళ (firstdigit == '0' && !(do_oct && pre == 0))
	    అయితే (నిడివి-- > 0)
		*ptr++ = '0';
	*ptr = NUL;
	buf1len = (పూర్ణాంకం)(ptr - buf1);

	STRCPY(buf1 + buf1len, buf2);
	buf1len += buf2len;

	// Insert just after the first character to be removed, so that any
	// text properties will be adjusted.  Then delete the old number
	// afterwards.
	save_pos = curwin->w_cursor;
	ఒకవేళ (todel > 0)
	    inc_cursor();
	ins_str(buf1, (size_t)buf1len);		// insert the new number
	vim_free(buf1);

	// del_char() will also mark line needing displaying
	ఒకవేళ (todel > 0)
	{
	    పూర్ణాంకం బైట్లు_after = ml_get_curline_len() - curwin->w_cursor.నిలువు;

	    // Delete the one character before the insert.
	    curwin->w_cursor = save_pos;
	    (శూన్య)del_char(FALSE);
	    curwin->w_cursor.నిలువు = ml_get_curline_len() - బైట్లు_after;
	    --todel;
	}
	అయితే (todel-- > 0)
	    (శూన్య)del_char(FALSE);

	endpos = curwin->w_cursor;
	ఒకవేళ (did_change && curwin->w_cursor.నిలువు)
	    --curwin->w_cursor.నిలువు;
    }

    ఒకవేళ (did_change && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// set the '[ and '] marks
	curbuf->b_op_ప్రారంభం = startpos;
	curbuf->b_op_చివర = endpos;
	ఒకవేళ (curbuf->b_op_చివర.నిలువు > 0)
	    --curbuf->b_op_చివర.నిలువు;
    }

theend:
    ఒకవేళ (visual)
	curwin->w_cursor = save_cursor;
    లేకపోతే ఒకవేళ (did_change)
	curwin->w_సమితి_curswant = true;
    లేకపోతే ఒకవేళ (virtual_చురుకైన())
	curwin->w_cursor.coladd = save_coladd;

    తిరిగి did_change;
}

    శూన్య
clear_oparg(oparg_T *oap)
{
    CLEAR_సూచిక(oap);
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
    స్థిర varnumber_T
పంక్తి_లెక్క_info(
    char_u	*పంక్తి,
    varnumber_T	*wc,
    varnumber_T	*cc,
    varnumber_T	హద్దు,
    పూర్ణాంకం		eol_కొలత)
{
    varnumber_T	i;
    varnumber_T	పదాలు = 0;
    varnumber_T	chars = 0;
    పూర్ణాంకం		is_మాట = 0;

    కొరకు (i = 0; i < హద్దు && పంక్తి[i] != NUL; )
    {
	ఒకవేళ (is_మాట)
	{
	    ఒకవేళ (vim_isspace(పంక్తి[i]))
	    {
		పదాలు++;
		is_మాట = 0;
	    }
	}
	లేకపోతే ఒకవేళ (!vim_isspace(పంక్తి[i]))
	    is_మాట = 1;
	++chars;
	i += (*mb_ptr2len)(పంక్తి + i);
    }

    ఒకవేళ (is_మాట)
	పదాలు++;
    *wc += పదాలు;

    // Add eol_size if the end of line was reached before hitting limit.
    ఒకవేళ (i < హద్దు && పంక్తి[i] == NUL)
    {
	i += eol_కొలత;
	chars += eol_కొలత;
    }
    *cc += chars;
    తిరిగి i;
}

/*
 * Give some info about the position of the cursor (for "g CTRL-G").
 * In Visual mode, give some info about the selected region.  (In this case,
 * the *_count_cursor variables store running totals for the selection.)
 * When "dict" is not NULL store the info there instead of showing it.
 */
    శూన్య
cursor_pos_info(dict_T *dict)
{
    char_u	*p;
    char_u	buf1[50];
    char_u	buf2[40];
    linenr_T	lnum;
    varnumber_T	byte_లెక్క = 0;
    varnumber_T	bom_లెక్క  = 0;
    varnumber_T	byte_లెక్క_cursor = 0;
    varnumber_T	char_లెక్క = 0;
    varnumber_T	char_లెక్క_cursor = 0;
    varnumber_T	మాట_లెక్క = 0;
    varnumber_T	మాట_లెక్క_cursor = 0;
    పూర్ణాంకం		eol_కొలత;
    varnumber_T	చివరి_check = 100000L;
    దీర్ఘ	పంక్తి_లెక్క_selected = 0;
    pos_T	min_pos, max_pos;
    oparg_T	oparg;
    నిర్మాణం ఖండం_def	bd;

    /*
     * Compute the length of the file in characters.
     */
    ఒకవేళ (curbuf->b_ml.ml_flags & ML_ఖాళీలు)
    {
	ఒకవేళ (dict == ఖాళీ)
	{
	    సందేశం(_(no_పంక్తులు_సందేశం));
	    తిరిగి;
	}
    }
    లేకపోతే
    {
	ఒకవేళ (get_fileformat(curbuf) == EOL_DOS)
	    eol_కొలత = 2;
	లేకపోతే
	    eol_కొలత = 1;

	ఒకవేళ (VIsual_చురుకైన)
	{
	    ఒకవేళ (LT_POS(VIsual, curwin->w_cursor))
	    {
		min_pos = VIsual;
		max_pos = curwin->w_cursor;
	    }
	    లేకపోతే
	    {
		min_pos = curwin->w_cursor;
		max_pos = VIsual;
	    }
	    ఒకవేళ (*p_sel == 'e' && max_pos.నిలువు > 0)
		--max_pos.నిలువు;

	    ఒకవేళ (VIsual_బహుళకం == Ctrl_V)
	    {
#నిర్వచితమైతే FEAT_LINEBREAK
		char_u * saved_sbr = p_sbr;
		char_u * saved_w_sbr = curwin->w_p_sbr;

		// Make 'sbr' empty for a moment to get the correct size.
		p_sbr = ఖాళీలు_option;
		curwin->w_p_sbr = ఖాళీలు_option;
#అయితే_ముగింపు
		oparg.is_VIsual = 1;
		oparg.ఖండం_బహుళకం = TRUE;
		oparg.op_type = OP_NOP;
		getvcols(curwin, &min_pos, &max_pos,
					&oparg.ప్రారంభం_vcol, &oparg.చివర_vcol, 0);
#నిర్వచితమైతే FEAT_LINEBREAK
		p_sbr = saved_sbr;
		curwin->w_p_sbr = saved_w_sbr;
#అయితే_ముగింపు
		ఒకవేళ (curwin->w_curswant == MAXCOL)
		    oparg.చివర_vcol = MAXCOL;
		// Swap the start, end vcol if needed
		ఒకవేళ (oparg.చివర_vcol < oparg.ప్రారంభం_vcol)
		{
		    oparg.చివర_vcol += oparg.ప్రారంభం_vcol;
		    oparg.ప్రారంభం_vcol = oparg.చివర_vcol - oparg.ప్రారంభం_vcol;
		    oparg.చివర_vcol -= oparg.ప్రారంభం_vcol;
		}
	    }
	    పంక్తి_లెక్క_selected = max_pos.lnum - min_pos.lnum + 1;
	}

	కొరకు (lnum = 1; lnum <= curbuf->b_ml.ml_పంక్తి_లెక్క; ++lnum)
	{
	    // Check for a CTRL-C every 100000 characters.
	    ఒకవేళ (byte_లెక్క > చివరి_check)
	    {
		ui_breakcheck();
		ఒకవేళ (got_int)
		    తిరిగి;
		చివరి_check = byte_లెక్క + 100000L;
	    }

	    // Do extra processing for VIsual mode.
	    ఒకవేళ (VIsual_చురుకైన
		    && lnum >= min_pos.lnum && lnum <= max_pos.lnum)
	    {
		char_u	    *s = ఖాళీ;
		దీర్ఘ	    len = 0L;

		ఎంపిక (VIsual_బహుళకం)
		{
		    స్థితి Ctrl_V:
			virtual_op = virtual_చురుకైన();
			ఖండం_prep(&oparg, &bd, lnum, 0);
			virtual_op = MAYBE;
			s = bd.textstart;
			len = (దీర్ఘ)bd.textlen;
			ఆపు;
		    స్థితి 'V':
			s = ml_get(lnum);
			len = MAXCOL;
			ఆపు;
		    స్థితి 'v':
			{
			    colnr_T ప్రారంభం_నిలువు = (lnum == min_pos.lnum)
							   ? min_pos.నిలువు : 0;
			    colnr_T చివర_నిలువు = (lnum == max_pos.lnum)
				      ? max_pos.నిలువు - ప్రారంభం_నిలువు + 1 : MAXCOL;

			    s = ml_get(lnum) + ప్రారంభం_నిలువు;
			    len = చివర_నిలువు;
			}
			ఆపు;
		}
		ఒకవేళ (s != ఖాళీ)
		{
		    byte_లెక్క_cursor += పంక్తి_లెక్క_info(s, &మాట_లెక్క_cursor,
					   &char_లెక్క_cursor, len, eol_కొలత);
		    ఒకవేళ (lnum == curbuf->b_ml.ml_పంక్తి_లెక్క
			    && !curbuf->b_p_eol
			    && (curbuf->b_p_bin || !curbuf->b_p_fixeol)
			    && (దీర్ఘ)STRLEN(s) < len)
			byte_లెక్క_cursor -= eol_కొలత;
		}
	    }
	    లేకపోతే
	    {
		// In non-visual mode, check for the line the cursor is on
		ఒకవేళ (lnum == curwin->w_cursor.lnum)
		{
		    మాట_లెక్క_cursor += మాట_లెక్క;
		    char_లెక్క_cursor += char_లెక్క;
		    byte_లెక్క_cursor = byte_లెక్క +
			పంక్తి_లెక్క_info(ml_get(lnum),
				&మాట_లెక్క_cursor, &char_లెక్క_cursor,
				(varnumber_T)(curwin->w_cursor.నిలువు + 1),
				eol_కొలత);
		}
	    }
	    // Add to the running totals
	    byte_లెక్క += పంక్తి_లెక్క_info(ml_get(lnum), &మాట_లెక్క,
					 &char_లెక్క, (varnumber_T)MAXCOL,
					 eol_కొలత);
	}

	// Correction for when last line doesn't have an EOL.
	ఒకవేళ (!curbuf->b_p_eol && (curbuf->b_p_bin || !curbuf->b_p_fixeol))
	    byte_లెక్క -= eol_కొలత;

	ఒకవేళ (dict == ఖాళీ)
	{
	    ఒకవేళ (VIsual_చురుకైన)
	    {
		ఒకవేళ (VIsual_బహుళకం == Ctrl_V && curwin->w_curswant < MAXCOL)
		{
		    getvcols(curwin, &min_pos, &max_pos,
						&min_pos.నిలువు, &max_pos.నిలువు, 0);
		    vim_snprintf((అక్షరం *)buf1, పరిమాణం(buf1), _("%ld Cols; "),
			    (దీర్ఘ)(oparg.చివర_vcol - oparg.ప్రారంభం_vcol + 1));
		}
		లేకపోతే
		    buf1[0] = NUL;

		ఒకవేళ (char_లెక్క_cursor == byte_లెక్క_cursor
						    && char_లెక్క == byte_లెక్క)
		    vim_snprintf((అక్షరం *)IObuff, IOSIZE,
			    _("Selected %s%ld of %ld Lines; %lld of %lld Words; %lld of %lld Bytes"),
			    buf1, పంక్తి_లెక్క_selected,
			    (దీర్ఘ)curbuf->b_ml.ml_పంక్తి_లెక్క,
			    మాట_లెక్క_cursor,
			    మాట_లెక్క,
			    byte_లెక్క_cursor,
			    byte_లెక్క);
		లేకపోతే
		    vim_snprintf((అక్షరం *)IObuff, IOSIZE,
			    _("Selected %s%ld of %ld Lines; %lld of %lld Words; %lld of %lld Chars; %lld of %lld Bytes"),
			    buf1, పంక్తి_లెక్క_selected,
			    (దీర్ఘ)curbuf->b_ml.ml_పంక్తి_లెక్క,
			    మాట_లెక్క_cursor,
			    మాట_లెక్క,
			    char_లెక్క_cursor,
			    char_లెక్క,
			    byte_లెక్క_cursor,
			    byte_లెక్క);
	    }
	    లేకపోతే
	    {
		p = ml_get_curline();
		validate_virtcol();
		నిలువు_print(buf1, పరిమాణం(buf1), (పూర్ణాంకం)curwin->w_cursor.నిలువు + 1,
			(పూర్ణాంకం)curwin->w_virtcol + 1);
		నిలువు_print(buf2, పరిమాణం(buf2), ml_get_curline_len(),
							   linetabsize_str(p));

		ఒకవేళ (char_లెక్క_cursor == byte_లెక్క_cursor
			&& char_లెక్క == byte_లెక్క)
		    vim_snprintf((అక్షరం *)IObuff, IOSIZE,
			_("Col %s of %s; Line %ld of %ld; Word %lld of %lld; Byte %lld of %lld"),
			(అక్షరం *)buf1, (అక్షరం *)buf2,
			(దీర్ఘ)curwin->w_cursor.lnum,
			(దీర్ఘ)curbuf->b_ml.ml_పంక్తి_లెక్క,
			మాట_లెక్క_cursor, మాట_లెక్క,
			byte_లెక్క_cursor, byte_లెక్క);
		లేకపోతే
		    vim_snprintf((అక్షరం *)IObuff, IOSIZE,
			_("Col %s of %s; Line %ld of %ld; Word %lld of %lld; Char %lld of %lld; Byte %lld of %lld"),
			(అక్షరం *)buf1, (అక్షరం *)buf2,
			(దీర్ఘ)curwin->w_cursor.lnum,
			(దీర్ఘ)curbuf->b_ml.ml_పంక్తి_లెక్క,
			మాట_లెక్క_cursor, మాట_లెక్క,
			char_లెక్క_cursor, char_లెక్క,
			byte_లెక్క_cursor, byte_లెక్క);
	    }
	}

	bom_లెక్క = bomb_కొలత();
	ఒకవేళ (dict == ఖాళీ && bom_లెక్క > 0)
	{
	    size_t len = STRLEN(IObuff);

	    vim_snprintf((అక్షరం *)IObuff + len, IOSIZE - len,
				 _("(+%lld for BOM)"), bom_లెక్క);
	}
	ఒకవేళ (dict == ఖాళీ)
	{
	    // Don't shorten this message, the user asked for it.
	    p = p_shm;
	    p_shm = (char_u *)"";
	    సందేశం((అక్షరం *)IObuff);
	    p_shm = p;
	}
    }
#ఒకవేళ defined(FEAT_EVAL)
    ఒకవేళ (dict != ఖాళీ)
    {
	dict_కూడు_సంఖ్య(dict, "words", మాట_లెక్క);
	dict_కూడు_సంఖ్య(dict, "chars", char_లెక్క);
	dict_కూడు_సంఖ్య(dict, "bytes", byte_లెక్క + bom_లెక్క);
	dict_కూడు_సంఖ్య(dict, VIsual_చురుకైన ? "visual_bytes" : "cursor_bytes",
		byte_లెక్క_cursor);
	dict_కూడు_సంఖ్య(dict, VIsual_చురుకైన ? "visual_chars" : "cursor_chars",
		char_లెక్క_cursor);
	dict_కూడు_సంఖ్య(dict, VIsual_చురుకైన ? "visual_words" : "cursor_words",
		మాట_లెక్క_cursor);
    }
#అయితే_ముగింపు
}

/*
 * Handle indent and format operators and visual mode ":".
 */
    స్థిర శూన్య
op_colon(oparg_T *oap)
{
    stuffcharReadbuff(':');
    ఒకవేళ (oap->is_VIsual)
	stuffReadbuff((char_u *)"'<,'>");
    లేకపోతే
    {
	// Make the range look nice, so it can be repeated.
	ఒకవేళ (oap->ప్రారంభం.lnum == curwin->w_cursor.lnum)
	    stuffcharReadbuff('.');
	లేకపోతే
	    stuffnumReadbuff((దీర్ఘ)oap->ప్రారంభం.lnum);

#నిర్వచితమైతే FEAT_FOLDING
	// When using !! on a closed fold the range ".!" works best to operate
	// on, it will be made the whole closed fold later.
	linenr_T చివర_యొక్క_ప్రారంభం_Fold = oap->ప్రారంభం.lnum;
	(శూన్య)hasFolding(oap->ప్రారంభం.lnum, ఖాళీ, &చివర_యొక్క_ప్రారంభం_Fold);
#అయితే_ముగింపు
	ఒకవేళ (oap->చివర.lnum != oap->ప్రారంభం.lnum
#నిర్వచితమైతే FEAT_FOLDING
		&& oap->చివర.lnum != చివర_యొక్క_ప్రారంభం_Fold
#అయితే_ముగింపు
	   )
	{
	    // Make it a range with the end line.
	    stuffcharReadbuff(',');
	    ఒకవేళ (oap->చివర.lnum == curwin->w_cursor.lnum)
		stuffcharReadbuff('.');
	    లేకపోతే ఒకవేళ (oap->చివర.lnum == curbuf->b_ml.ml_పంక్తి_లెక్క)
		stuffcharReadbuff('$');
	    లేకపోతే ఒకవేళ (oap->ప్రారంభం.lnum == curwin->w_cursor.lnum
#నిర్వచితమైతే FEAT_FOLDING
		    // do not use ".+number" for a closed fold, it would count
		    // folded lines twice
		    && !hasFolding(oap->చివర.lnum, ఖాళీ, ఖాళీ)
#అయితే_ముగింపు
		    )
	    {
		stuffReadbuff((char_u *)".+");
		stuffnumReadbuff((దీర్ఘ)oap->పంక్తి_లెక్క - 1);
	    }
	    లేకపోతే
		stuffnumReadbuff((దీర్ఘ)oap->చివర.lnum);
	}
    }
    ఒకవేళ (oap->op_type != OP_COLON)
	stuffReadbuff((char_u *)"!");
    ఒకవేళ (oap->op_type == OP_INDENT)
    {
	ఒకవేళ (*get_equalprg() == NUL)
	    stuffReadbuff((char_u *)"indent");
	లేకపోతే
	    stuffReadbuff(get_equalprg());
	stuffReadbuff((char_u *)"\n");
    }
    లేకపోతే ఒకవేళ (oap->op_type == OP_FORMAT)
    {
	ఒకవేళ (*curbuf->b_p_fp != NUL)
	    stuffReadbuff(curbuf->b_p_fp);
	లేకపోతే ఒకవేళ (*p_fp != NUL)
	    stuffReadbuff(p_fp);
	లేకపోతే
	    stuffReadbuff((char_u *)"fmt");
	stuffReadbuff((char_u *)"\n']");
    }

    // do_cmdline() does the rest
}

// callback function for 'operatorfunc'
స్థిర callback_T opfunc_cb;

/*
 * Process the 'operatorfunc' option value.
 * Returns OK or FAIL.
 */
    అక్షరం *
did_సమితి_operatorfunc(optset_T *వాదనలు వాడని)
{
    ఒకవేళ (option_సమితి_callback_func(p_opfunc, &opfunc_cb) == FAIL)
	తిరిగి e_invalid_వాదం;

    తిరిగి ఖాళీ;
}

#ఒకవేళ defined(EXITFREE)
    శూన్య
free_operatorfunc_option(శూన్య)
{
# ifdef FEAT_EVAL
    free_callback(&opfunc_cb);
# endif
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Mark the global 'operatorfunc' callback with "copyID" so that it is not
 * garbage collected.
 */
    పూర్ణాంకం
సమితి_ref_లోపల_opfunc(పూర్ణాంకం copyID వాడని)
{
    పూర్ణాంకం abort = FALSE;

    abort = సమితి_ref_లోపల_callback(&opfunc_cb, copyID);

    తిరిగి abort;
}
#అయితే_ముగింపు

/*
 * Handle the "g@" operator: call 'operatorfunc'.
 */
    స్థిర శూన్య
op_function(oparg_T *oap వాడని)
{
#నిర్వచితమైతే FEAT_EVAL
    typval_T	argv[2];
    pos_T	orig_ప్రారంభం = curbuf->b_op_ప్రారంభం;
    pos_T	orig_చివర = curbuf->b_op_చివర;
    typval_T	rettv;

    ఒకవేళ (*p_opfunc == NUL)
	emsg(_(e_operatorfunc_is_ఖాళీలు));
    లేకపోతే
    {
	// Set '[ and '] marks to text to be operated on.
	curbuf->b_op_ప్రారంభం = oap->ప్రారంభం;
	curbuf->b_op_చివర = oap->చివర;
	ఒకవేళ (oap->motion_type != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_చివర);

	argv[0].v_type = VAR_STRING;
	ఒకవేళ (oap->ఖండం_బహుళకం)
	    argv[0].vval.v_string = (char_u *)"block";
	లేకపోతే ఒకవేళ (oap->motion_type == MLINE)
	    argv[0].vval.v_string = (char_u *)"line";
	లేకపోతే
	    argv[0].vval.v_string = (char_u *)"char";
	argv[1].v_type = VAR_UNKNOWN;

	// Reset virtual_op so that 'virtualedit' can be changed in the
	// function.
	పూర్ణాంకం save_virtual_op = virtual_op;
	virtual_op = MAYBE;

	// Reset finish_op so that mode() returns the right value.
	పూర్ణాంకం save_ముగింపు_op = ముగింపు_op;
	ముగింపు_op = FALSE;

	ఒకవేళ (call_callback(&opfunc_cb, 0, &rettv, 1, argv) != FAIL)
	    clear_tv(&rettv);

	virtual_op = save_virtual_op;
	ముగింపు_op = save_ముగింపు_op;
	ఒకవేళ (cmdmod.cmod_flags & CMOD_LOCKMARKS)
	{
	    curbuf->b_op_ప్రారంభం = orig_ప్రారంభం;
	    curbuf->b_op_చివర = orig_చివర;
	}
    }
#లేకపోతే
    emsg(_(e_eval_feature_not_అందుబాటు));
#అయితే_ముగింపు
}

/*
 * Calculate start/end virtual columns for operating in block mode.
 */
    స్థిర శూన్య
get_op_vcol(
    oparg_T	*oap,
    colnr_T	redo_VIsual_vcol,
    పూర్ణాంకం		initial)    // when TRUE adjust position for 'selectmode'
{
    colnr_T	    ప్రారంభం, చివర;

    ఒకవేళ (VIsual_బహుళకం != Ctrl_V
	    || (!initial && oap->చివర.నిలువు < curwin->w_వెడల్పు))
	తిరిగి;

    oap->ఖండం_బహుళకం = TRUE;

    // prevent from moving onto a trail byte
    ఒకవేళ (has_mbyte)
	mb_adjustpos(curwin->w_బఫరు, &oap->చివర);

    getvvcol(curwin, &(oap->ప్రారంభం), &oap->ప్రారంభం_vcol, ఖాళీ, &oap->చివర_vcol, 0);

    ఒకవేళ (!redo_VIsual_busy)
    {
	getvvcol(curwin, &(oap->చివర), &ప్రారంభం, ఖాళీ, &చివర, 0);

	ఒకవేళ (ప్రారంభం < oap->ప్రారంభం_vcol)
	    oap->ప్రారంభం_vcol = ప్రారంభం;
	ఒకవేళ (చివర > oap->చివర_vcol)
	{
	    ఒకవేళ (initial && *p_sel == 'e' && ప్రారంభం >= 1
				    && ప్రారంభం - 1 >= oap->చివర_vcol)
		oap->చివర_vcol = ప్రారంభం - 1;
	    లేకపోతే
		oap->చివర_vcol = చివర;
	}
    }

    // if '$' was used, get oap->end_vcol from longest line
    ఒకవేళ (curwin->w_curswant == MAXCOL)
    {
	curwin->w_cursor.నిలువు = MAXCOL;
	oap->చివర_vcol = 0;
	కొరకు (curwin->w_cursor.lnum = oap->ప్రారంభం.lnum;
		curwin->w_cursor.lnum <= oap->చివర.lnum;
					++curwin->w_cursor.lnum)
	{
	    getvvcol(curwin, &curwin->w_cursor, ఖాళీ, ఖాళీ, &చివర, 0);
	    ఒకవేళ (చివర > oap->చివర_vcol)
		oap->చివర_vcol = చివర;
	}
    }
    లేకపోతే ఒకవేళ (redo_VIsual_busy)
	oap->చివర_vcol = oap->ప్రారంభం_vcol + redo_VIsual_vcol - 1;
    // Correct oap->end.col and oap->start.col to be the
    // upper-left and lower-right corner of the block area.
    //
    // (Actually, this does convert column positions into character
    // positions)
    curwin->w_cursor.lnum = oap->చివర.lnum;
    coladvance(oap->చివర_vcol);
    oap->చివర = curwin->w_cursor;

    curwin->w_cursor = oap->ప్రారంభం;
    coladvance(oap->ప్రారంభం_vcol);
    oap->ప్రారంభం = curwin->w_cursor;
}

// Information for redoing the previous Visual selection.
రకనిర్వచనం నిర్మాణం {
    పూర్ణాంకం		rv_బహుళకం;	// 'v', 'V', or Ctrl-V
    linenr_T	rv_పంక్తి_లెక్క;	// number of lines
    colnr_T	rv_vcol;	// number of cols or end column
    దీర్ఘ	rv_లెక్క;	// count for Visual operator
    పూర్ణాంకం		rv_arg;		// extra argument
} redo_VIsual_T;

    స్థిర పూర్ణాంకం
is_ex_cmdchar(cmdarg_T *cap)
{
    తిరిగి cap->cmdchar == ':'
	|| cap->cmdchar == K_COMMAND
	|| cap->cmdchar == K_SCRIPT_COMMAND;
}

/*
 * Handle an operator after Visual mode or when the movement is finished.
 * "gui_yank" is true when yanking text for the clipboard.
 */
    శూన్య
do_pending_operator(cmdarg_T *cap, పూర్ణాంకం old_నిలువు, పూర్ణాంకం gui_yank)
{
    oparg_T	*oap = cap->oap;
    pos_T	old_cursor;
    పూర్ణాంకం		ఖాళీలు_region_error;
    పూర్ణాంకం		restart_edit_save;
#నిర్వచితమైతే FEAT_LINEBREAK
    పూర్ణాంకం		lbr_saved = curwin->w_p_lbr;
#అయితే_ముగింపు

    // The visual area is remembered for redo
    స్థిర redo_VIsual_T   redo_VIsual = {NUL, 0, 0, 0,0};

    పూర్ణాంకం		    include_పంక్తి_break = FALSE;

#ఒకవేళ defined(FEAT_CLIPBOARD)
    // Yank the visual area into the GUI selection register before we operate
    // on it and lose it forever.
    // Don't do it if a specific register was specified, so that ""x"*P works.
    // This could call do_pending_operator() recursively, but that's OK
    // because gui_yank will be TRUE for the nested call.
    ఒకవేళ ((clip_నక్షత్రం.అందుబాటు || clip_plus.అందుబాటు)
	    && oap->op_type != OP_NOP
	    && !gui_yank
	    && VIsual_చురుకైన
	    && !redo_VIsual_busy
	    && oap->regname == 0)
	clip_auto_select();
#అయితే_ముగింపు
    old_cursor = curwin->w_cursor;

    // If an operation is pending, handle it...
    ఒకవేళ ((ముగింపు_op || VIsual_చురుకైన) && oap->op_type != OP_NOP)
    {
	// Yank can be redone when 'y' is in 'cpoptions', but not when yanking
	// for the clipboard.
	పూర్ణాంకం	redo_yank = vim_strchr(p_cpo, CPO_YANK) != ఖాళీ && !gui_yank;

#నిర్వచితమైతే FEAT_LINEBREAK
	// Avoid a problem with unwanted linebreaks in block mode.
	(శూన్య)reset_lbr();
#అయితే_ముగింపు
	oap->is_VIsual = VIsual_చురుకైన;
	ఒకవేళ (oap->motion_force == 'V')
	    oap->motion_type = MLINE;
	లేకపోతే ఒకవేళ (oap->motion_force == 'v')
	{
	    // If the motion was linewise, "inclusive" will not have been set.
	    // Use "exclusive" to be consistent.  Makes "dvj" work nice.
	    ఒకవేళ (oap->motion_type == MLINE)
		oap->inclusive = FALSE;
	    // If the motion already was characterwise, toggle "inclusive"
	    లేకపోతే ఒకవేళ (oap->motion_type == MCHAR)
		oap->inclusive = !oap->inclusive;
	    oap->motion_type = MCHAR;
	}
	లేకపోతే ఒకవేళ (oap->motion_force == Ctrl_V)
	{
	    // Change line- or characterwise motion into Visual block mode.
	    ఒకవేళ (!VIsual_చురుకైన)
	    {
		VIsual_చురుకైన = TRUE;
		VIsual = oap->ప్రారంభం;
	    }
	    VIsual_బహుళకం = Ctrl_V;
	    VIsual_select = FALSE;
	    VIsual_reselect = FALSE;
	}

	// Only redo yank when 'y' flag is in 'cpoptions'.
	// Never redo "zf" (define fold).
	ఒకవేళ ((redo_yank || oap->op_type != OP_YANK)
		&& ((!VIsual_చురుకైన || oap->motion_force)
		    // Also redo Operator-pending Visual mode mappings
		    || (VIsual_చురుకైన
			    && is_ex_cmdchar(cap) && oap->op_type != OP_COLON))
		&& cap->cmdchar != 'D'
#నిర్వచితమైతే FEAT_FOLDING
		&& oap->op_type != OP_FOLD
		&& oap->op_type != OP_FOLDOPEN
		&& oap->op_type != OP_FOLDOPENREC
		&& oap->op_type != OP_FOLDCLOSE
		&& oap->op_type != OP_FOLDCLOSEREC
		&& oap->op_type != OP_FOLDDEL
		&& oap->op_type != OP_FOLDDELREC
#అయితే_ముగింపు
		)
	{
	    prep_redo(oap->regname, cap->count0,
		    get_op_char(oap->op_type), get_extra_op_char(oap->op_type),
		    oap->motion_force, cap->cmdchar, cap->nchar);
	    ఒకవేళ (cap->cmdchar == '/' || cap->cmdchar == '?') // was a search
	    {
		// If 'cpoptions' does not contain 'r', insert the search
		// pattern to really repeat the same command.
		ఒకవేళ (vim_strchr(p_cpo, CPO_REDO) == ఖాళీ)
		    Append_గమ్యం_Redobuff_Lit(cap->searchbuf, -1);
		Append_గమ్యం_Redobuff(NL_STR);
	    }
	    లేకపోతే ఒకవేళ (is_ex_cmdchar(cap))
	    {
		// do_cmdline() has stored the first typed line in
		// "repeat_cmdline".  When several lines are typed repeating
		// won't be possible.
		ఒకవేళ (repeat_cmdline == ఖాళీ)
		    ResetRedobuff();
		లేకపోతే
		{
		    ఒకవేళ (cap->cmdchar == ':')
			Append_గమ్యం_Redobuff_Lit(repeat_cmdline, -1);
		    లేకపోతే
			Append_గమ్యం_Redobuff_Spec(repeat_cmdline);
		    Append_గమ్యం_Redobuff(NL_STR);
		    VIM_CLEAR(repeat_cmdline);
		}
	    }
	}

	ఒకవేళ (redo_VIsual_busy)
	{
	    // Redo of an operation on a Visual area. Use the same size from
	    // redo_VIsual.rv_line_count and redo_VIsual.rv_vcol.
	    oap->ప్రారంభం = curwin->w_cursor;
	    curwin->w_cursor.lnum += redo_VIsual.rv_పంక్తి_లెక్క - 1;
	    ఒకవేళ (curwin->w_cursor.lnum > curbuf->b_ml.ml_పంక్తి_లెక్క)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_పంక్తి_లెక్క;
	    VIsual_బహుళకం = redo_VIsual.rv_బహుళకం;
	    ఒకవేళ (redo_VIsual.rv_vcol == MAXCOL || VIsual_బహుళకం == 'v')
	    {
		ఒకవేళ (VIsual_బహుళకం == 'v')
		{
		    ఒకవేళ (redo_VIsual.rv_పంక్తి_లెక్క <= 1)
		    {
			validate_virtcol();
			curwin->w_curswant =
				     curwin->w_virtcol + redo_VIsual.rv_vcol - 1;
		    }
		    లేకపోతే
			curwin->w_curswant = redo_VIsual.rv_vcol;
		}
		లేకపోతే
		{
		    curwin->w_curswant = MAXCOL;
		}
		coladvance(curwin->w_curswant);
	    }
	    cap->count0 = redo_VIsual.rv_లెక్క;
	    ఒకవేళ (redo_VIsual.rv_లెక్క != 0)
		cap->count1 = redo_VIsual.rv_లెక్క;
	    లేకపోతే
		cap->count1 = 1;
	}
	లేకపోతే ఒకవేళ (VIsual_చురుకైన)
	{
	    ఒకవేళ (!gui_yank)
	    {
		// Save the current VIsual area for '< and '> marks, and "gv"
		curbuf->b_visual.vi_ప్రారంభం = VIsual;
		curbuf->b_visual.vi_చివర = curwin->w_cursor;
		curbuf->b_visual.vi_బహుళకం = VIsual_బహుళకం;
		restore_visual_బహుళకం();
		curbuf->b_visual.vi_curswant = curwin->w_curswant;
#నిర్వచితమైతే FEAT_EVAL
		curbuf->b_visual_బహుళకం_eval = VIsual_బహుళకం;
#అయితే_ముగింపు
	    }

	    // In Select mode, a linewise selection is operated upon like a
	    // characterwise selection.
	    // Special case: gH<Del> deletes the last line.
	    ఒకవేళ (VIsual_select && VIsual_బహుళకం == 'V'
					    && cap->oap->op_type != OP_DELETE)
	    {
		ఒకవేళ (LT_POS(VIsual, curwin->w_cursor))
		{
		    VIsual.నిలువు = 0;
		    curwin->w_cursor.నిలువు = ml_get_len(curwin->w_cursor.lnum);
		}
		లేకపోతే
		{
		    curwin->w_cursor.నిలువు = 0;
		    VIsual.నిలువు = ml_get_len(VIsual.lnum);
		}
		VIsual_బహుళకం = 'v';
	    }
	    // If 'selection' is "exclusive", backup one character for
	    // charwise selections.
	    లేకపోతే ఒకవేళ (VIsual_బహుళకం == 'v')
		include_పంక్తి_break = unadjust_for_sel();

	    oap->ప్రారంభం = VIsual;
	    ఒకవేళ (VIsual_బహుళకం == 'V')
	    {
		oap->ప్రారంభం.నిలువు = 0;
		oap->ప్రారంభం.coladd = 0;
	    }
	}

	// Set oap->start to the first position of the operated text, oap->end
	// to the end of the operated text.  w_cursor is equal to oap->start.
	ఒకవేళ (LT_POS(oap->ప్రారంభం, curwin->w_cursor))
	{
#నిర్వచితమైతే FEAT_FOLDING
	    // Include folded lines completely.
	    ఒకవేళ (!VIsual_చురుకైన)
	    {
		ఒకవేళ (hasFolding(oap->ప్రారంభం.lnum, &oap->ప్రారంభం.lnum, ఖాళీ))
		    oap->ప్రారంభం.నిలువు = 0;
		ఒకవేళ ((curwin->w_cursor.నిలువు > 0 || oap->inclusive
						  || oap->motion_type == MLINE)
			&& hasFolding(curwin->w_cursor.lnum, ఖాళీ,
						      &curwin->w_cursor.lnum))
		    curwin->w_cursor.నిలువు = ml_get_curline_len();
	    }
#అయితే_ముగింపు
	    oap->చివర = curwin->w_cursor;
	    curwin->w_cursor = oap->ప్రారంభం;

	    // w_virtcol may have been updated; if the cursor goes back to its
	    // previous position w_virtcol becomes invalid and isn't updated
	    // automatically.
	    curwin->w_valid &= ~VALID_VIRTCOL;
	}
	లేకపోతే
	{
#నిర్వచితమైతే FEAT_FOLDING
	    // Include folded lines completely.
	    ఒకవేళ (!VIsual_చురుకైన && oap->motion_type == MLINE)
	    {
		ఒకవేళ (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum,
									ఖాళీ))
		    curwin->w_cursor.నిలువు = 0;
		ఒకవేళ (hasFolding(oap->ప్రారంభం.lnum, ఖాళీ, &oap->ప్రారంభం.lnum))
		    oap->ప్రారంభం.నిలువు = ml_get_len(oap->ప్రారంభం.lnum);
	    }
#అయితే_ముగింపు
	    oap->చివర = oap->ప్రారంభం;
	    oap->ప్రారంభం = curwin->w_cursor;
	}

	// Just in case lines were deleted that make the position invalid.
	check_pos(curwin->w_బఫరు, &oap->చివర);
	oap->పంక్తి_లెక్క = oap->చివర.lnum - oap->ప్రారంభం.lnum + 1;

	// Set "virtual_op" before resetting VIsual_active.
	virtual_op = virtual_చురుకైన();

	ఒకవేళ (VIsual_చురుకైన || redo_VIsual_busy)
	{
	    get_op_vcol(oap, redo_VIsual.rv_vcol, TRUE);

	    ఒకవేళ (!redo_VIsual_busy && !gui_yank)
	    {
		// Prepare to reselect and redo Visual: this is based on the
		// size of the Visual text
		resel_VIsual_బహుళకం = VIsual_బహుళకం;
		ఒకవేళ (curwin->w_curswant == MAXCOL)
		    resel_VIsual_vcol = MAXCOL;
		లేకపోతే
		{
		    ఒకవేళ (VIsual_బహుళకం != Ctrl_V)
			getvvcol(curwin, &(oap->చివర),
						ఖాళీ, ఖాళీ, &oap->చివర_vcol, 0);
		    ఒకవేళ (VIsual_బహుళకం == Ctrl_V || oap->పంక్తి_లెక్క <= 1)
		    {
			ఒకవేళ (VIsual_బహుళకం != Ctrl_V)
			    getvvcol(curwin, &(oap->ప్రారంభం),
					      &oap->ప్రారంభం_vcol, ఖాళీ, ఖాళీ, 0);
			resel_VIsual_vcol = oap->చివర_vcol - oap->ప్రారంభం_vcol + 1;
		    }
		    లేకపోతే
			resel_VIsual_vcol = oap->చివర_vcol;
		}
		resel_VIsual_పంక్తి_లెక్క = oap->పంక్తి_లెక్క;
	    }

	    // can't redo yank (unless 'y' is in 'cpoptions') and ":"
	    ఒకవేళ ((redo_yank || oap->op_type != OP_YANK)
		    && oap->op_type != OP_COLON
#నిర్వచితమైతే FEAT_FOLDING
		    && oap->op_type != OP_FOLD
		    && oap->op_type != OP_FOLDOPEN
		    && oap->op_type != OP_FOLDOPENREC
		    && oap->op_type != OP_FOLDCLOSE
		    && oap->op_type != OP_FOLDCLOSEREC
		    && oap->op_type != OP_FOLDDEL
		    && oap->op_type != OP_FOLDDELREC
#అయితే_ముగింపు
		    && oap->motion_force == NUL
		    )
	    {
		// Prepare for redoing.  Only use the nchar field for "r",
		// otherwise it might be the second char of the operator.
		ఒకవేళ (cap->cmdchar == 'g' && (cap->nchar == 'n'
							|| cap->nchar == 'N'))
		    prep_redo(oap->regname, cap->count0,
			    get_op_char(oap->op_type),
			    get_extra_op_char(oap->op_type),
			    oap->motion_force, cap->cmdchar, cap->nchar);
		లేకపోతే ఒకవేళ (!is_ex_cmdchar(cap))
		{
		    పూర్ణాంకం opchar = get_op_char(oap->op_type);
		    పూర్ణాంకం extra_opchar = get_extra_op_char(oap->op_type);
		    పూర్ణాంకం nchar = oap->op_type == OP_REPLACE ? cap->nchar : NUL;

		    // reverse what nv_replace() did
		    ఒకవేళ (nchar == REPLACE_CR_NCHAR)
			nchar = CAR;
		    లేకపోతే ఒకవేళ (nchar == REPLACE_NL_NCHAR)
			nchar = NL;

		    ఒకవేళ (opchar == 'g' && extra_opchar == '@')
			// also repeat the count for 'operatorfunc'
			prep_redo_num2(oap->regname, 0L, NUL, 'v',
				     cap->count0, opchar, extra_opchar, nchar);
		    లేకపోతే
			prep_redo(oap->regname, 0L, NUL, 'v',
						  opchar, extra_opchar, nchar);
		}
		ఒకవేళ (!redo_VIsual_busy)
		{
		    redo_VIsual.rv_బహుళకం = resel_VIsual_బహుళకం;
		    redo_VIsual.rv_vcol = resel_VIsual_vcol;
		    redo_VIsual.rv_పంక్తి_లెక్క = resel_VIsual_పంక్తి_లెక్క;
		    redo_VIsual.rv_లెక్క = cap->count0;
		    redo_VIsual.rv_arg = cap->arg;
		}
	    }

	    // oap->inclusive defaults to TRUE.
	    // If oap->end is on a NUL (empty line) oap->inclusive becomes
	    // FALSE.  This makes "d}P" and "v}dP" work the same.
	    ఒకవేళ (oap->motion_force == NUL || oap->motion_type == MLINE)
		oap->inclusive = TRUE;
	    ఒకవేళ (VIsual_బహుళకం == 'V')
		oap->motion_type = MLINE;
	    లేకపోతే
	    {
		oap->motion_type = MCHAR;
		ఒకవేళ (VIsual_బహుళకం != Ctrl_V && *ml_get_pos(&(oap->చివర)) == NUL
			&& (include_పంక్తి_break || !virtual_op))
		{
		    oap->inclusive = FALSE;
		    // Try to include the newline, unless it's an operator
		    // that works on lines only.
		    ఒకవేళ (*p_sel != 'o'
			    && !op_on_పంక్తులు(oap->op_type)
			    && oap->చివర.lnum < curbuf->b_ml.ml_పంక్తి_లెక్క)
		    {
			++oap->చివర.lnum;
			oap->చివర.నిలువు = 0;
			oap->చివర.coladd = 0;
			++oap->పంక్తి_లెక్క;
		    }
		}
	    }

	    redo_VIsual_busy = FALSE;

	    // Switch Visual off now, so screen updating does
	    // not show inverted text when the screen is redrawn.
	    // With OP_YANK and sometimes with OP_COLON and OP_FILTER there is
	    // no screen redraw, so it is done here to remove the inverted
	    // part.
	    ఒకవేళ (!gui_yank)
	    {
		VIsual_చురుకైన = FALSE;
		setmouse();
		mouse_dragging = 0;
		may_clear_cmdline();
		ఒకవేళ ((oap->op_type == OP_YANK
			    || oap->op_type == OP_COLON
			    || oap->op_type == OP_FUNCTION
			    || oap->op_type == OP_FILTER)
			&& oap->motion_force == NUL)
		{
#నిర్వచితమైతే FEAT_LINEBREAK
		    // make sure redrawing is correct
		    restore_lbr(lbr_saved);
#అయితే_ముగింపు
		    redraw_curbuf_later(UPD_INVERTED);
		}
	    }
	}

	// Include the trailing byte of a multi-byte char.
	ఒకవేళ (has_mbyte && oap->inclusive)
	{
	    పూర్ణాంకం		l;

	    l = (*mb_ptr2len)(ml_get_pos(&oap->చివర));
	    ఒకవేళ (l > 1)
		oap->చివర.నిలువు += l - 1;
	}
	curwin->w_సమితి_curswant = true;

	// oap->empty is set when start and end are the same.  The inclusive
	// flag affects this too, unless yanking and the end is on a NUL.
	oap->ఖాళీలు_2 = (oap->motion_type == MCHAR
		    && (!oap->inclusive
			|| (oap->op_type == OP_YANK
			    && gchar_pos(&oap->చివర) == NUL))
		    && EQUAL_POS(oap->ప్రారంభం, oap->చివర)
		    && !(virtual_op && oap->ప్రారంభం.coladd != oap->చివర.coladd));
	// For delete, change and yank, it's an error to operate on an
	// empty region, when 'E' included in 'cpoptions' (Vi compatible).
	ఖాళీలు_region_error = (oap->ఖాళీలు_2
				&& vim_strchr(p_cpo, CPO_EMPTYREGION) != ఖాళీ);

	// Force a redraw when operating on an empty Visual region, when
	// 'modifiable is off or creating a fold.
	ఒకవేళ (oap->is_VIsual && (oap->ఖాళీలు_2 || !curbuf->b_p_ma
#నిర్వచితమైతే FEAT_FOLDING
		    || oap->op_type == OP_FOLD
#అయితే_ముగింపు
		    ))
	{
#నిర్వచితమైతే FEAT_LINEBREAK
	    restore_lbr(lbr_saved);
#అయితే_ముగింపు
	    redraw_curbuf_later(UPD_INVERTED);
	}

	// If the end of an operator is in column one while oap->motion_type
	// is MCHAR and oap->inclusive is FALSE, we put op_end after the last
	// character in the previous line. If op_start is on or before the
	// first non-blank in the line, the operator becomes linewise
	// (strange, but that's the way vi does it).
	ఒకవేళ (	   oap->motion_type == MCHAR
		&& oap->inclusive == FALSE
		&& !(cap->retval & CA_NO_ADJ_OP_చివర)
		&& oap->చివర.నిలువు == 0
		&& (!oap->is_VIsual || *p_sel == 'o')
		&& !oap->ఖండం_బహుళకం
		&& oap->పంక్తి_లెక్క > 1)
	{
	    oap->చివర_adjusted = TRUE;	    // remember that we did this
	    --oap->పంక్తి_లెక్క;
	    --oap->చివర.lnum;
	    ఒకవేళ (inindent(0))
		oap->motion_type = MLINE;
	    లేకపోతే
	    {
		oap->చివర.నిలువు = ml_get_len(oap->చివర.lnum);
		ఒకవేళ (oap->చివర.నిలువు)
		{
		    --oap->చివర.నిలువు;
		    oap->inclusive = TRUE;
		}
	    }
	}
	లేకపోతే
	    oap->చివర_adjusted = FALSE;

	ఎంపిక (oap->op_type)
	{
	స్థితి OP_LSHIFT:
	స్థితి OP_RSHIFT:
	    op_జరుపు(oap, TRUE, oap->is_VIsual ? (పూర్ణాంకం)cap->count1 : 1);
	    auto_format(FALSE, TRUE);
	    ఆపు;

	స్థితి OP_JOIN_NS:
	స్థితి OP_JOIN:
	    ఒకవేళ (oap->పంక్తి_లెక్క < 2)
		oap->పంక్తి_లెక్క = 2;
	    ఒకవేళ (curwin->w_cursor.lnum + oap->పంక్తి_లెక్క - 1 >
						   curbuf->b_ml.ml_పంక్తి_లెక్క)
		beep_flush();
	    లేకపోతే
	    {
		(శూన్య)do_join(oap->పంక్తి_లెక్క, oap->op_type == OP_JOIN,
							    TRUE, TRUE, TRUE);
		auto_format(FALSE, TRUE);
	    }
	    ఆపు;

	స్థితి OP_DELETE:
	    VIsual_reselect = FALSE;	    // don't reselect now
	    ఒకవేళ (ఖాళీలు_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    లేకపోతే
	    {
		(శూన్య)op_delete(oap);
		// save cursor line for undo if it wasn't saved yet
		ఒకవేళ (oap->motion_type == MLINE && has_format_option(FO_AUTO)
						      && u_save_cursor() == OK)
		    auto_format(FALSE, TRUE);
	    }
	    ఆపు;

	స్థితి OP_YANK:
	    ఒకవేళ (ఖాళీలు_region_error)
	    {
		ఒకవేళ (!gui_yank)
		{
		    vim_beep(BO_OPER);
		    CancelRedo();
		}
	    }
	    లేకపోతే
	    {
#నిర్వచితమైతే FEAT_LINEBREAK
		restore_lbr(lbr_saved);
#అయితే_ముగింపు
		oap->excl_tr_ws = cap->cmdchar == 'z';
		(శూన్య)op_yank(oap, FALSE, !gui_yank);
	    }
	    check_cursor_నిలువు();
	    ఆపు;

	స్థితి OP_CHANGE:
	    VIsual_reselect = FALSE;	    // don't reselect now
	    ఒకవేళ (ఖాళీలు_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    లేకపోతే
	    {
		// This is a new edit command, not a restart.  Need to
		// remember it to make 'insertmode' work with mappings for
		// Visual mode.  But do this only once and not when typed and
		// 'insertmode' isn't set.
		ఒకవేళ (p_im || !కీలకం_Typed)
		    restart_edit_save = restart_edit;
		లేకపోతే
		    restart_edit_save = 0;
		restart_edit = 0;
#నిర్వచితమైతే FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#అయితే_ముగింపు
		// trigger TextChangedI
		curbuf->b_చివరి_changedtick_i = CHANGEDTICK(curbuf);

		ఒకవేళ (op_change(oap))	// will call edit()
		    cap->retval |= CA_COMMAND_BUSY;
		ఒకవేళ (restart_edit == 0)
		    restart_edit = restart_edit_save;
	    }
	    ఆపు;

	స్థితి OP_FILTER:
	    ఒకవేళ (vim_strchr(p_cpo, CPO_FILTER) != ఖాళీ)
		Append_గమ్యం_Redobuff((char_u *)"!\r");  // use any last used !cmd
	    లేకపోతే
		bangredo = TRUE;    // do_bang() will put cmd in redo buffer
	    // FALLTHROUGH

	స్థితి OP_INDENT:
	స్థితి OP_COLON:

	    // If 'equalprg' is empty, do the indenting internally.
	    ఒకవేళ (oap->op_type == OP_INDENT && *get_equalprg() == NUL)
	    {
		ఒకవేళ (curbuf->b_p_lisp)
		{
#నిర్వచితమైతే FEAT_EVAL
		    ఒకవేళ (use_indentexpr_for_lisp())
			op_reindent(oap, get_సమాసం_indent);
		    లేకపోతే
#అయితే_ముగింపు
			op_reindent(oap, get_lisp_indent);
		    ఆపు;
		}
		op_reindent(oap,
#నిర్వచితమైతే FEAT_EVAL
			*curbuf->b_p_inde != NUL ? get_సమాసం_indent :
#అయితే_ముగింపు
			    get_c_indent);
		ఆపు;
	    }

	    op_colon(oap);
	    ఆపు;

	స్థితి OP_TILDE:
	స్థితి OP_పై:
	స్థితి OP_క్రింది:
	స్థితి OP_ROT13:
	    ఒకవేళ (ఖాళీలు_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    లేకపోతే
		op_tilde(oap);
	    check_cursor_నిలువు();
	    ఆపు;

	స్థితి OP_FORMAT:
#ఒకవేళ defined(FEAT_EVAL)
	    ఒకవేళ (*curbuf->b_p_fex != NUL)
		op_formatexpr(oap);	// use expression
	    లేకపోతే
#అయితే_ముగింపు
	    {
		ఒకవేళ (*p_fp != NUL || *curbuf->b_p_fp != NUL)
		    op_colon(oap);		// use external command
		లేకపోతే
		    op_format(oap, FALSE);	// use internal function
	    }
	    ఆపు;
	స్థితి OP_FORMAT2:
	    op_format(oap, TRUE);	// use internal function
	    ఆపు;

	స్థితి OP_FUNCTION:
	    {
		redo_VIsual_T   save_redo_VIsual = redo_VIsual;

#నిర్వచితమైతే FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#అయితే_ముగింపు
		// call 'operatorfunc'
		op_function(oap);

		// Restore the info for redoing Visual mode, the function may
		// invoke another operator and unintentionally change it.
		redo_VIsual = save_redo_VIsual;
		ఆపు;
	    }

	స్థితి OP_చొప్పించు:
	స్థితి OP_APPEND:
	    VIsual_reselect = FALSE;	// don't reselect now
	    ఒకవేళ (ఖాళీలు_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    లేకపోతే
	    {
		// This is a new edit command, not a restart.  Need to
		// remember it to make 'insertmode' work with mappings for
		// Visual mode.  But do this only once.
		restart_edit_save = restart_edit;
		restart_edit = 0;
#నిర్వచితమైతే FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#అయితే_ముగింపు
		// trigger TextChangedI
		curbuf->b_చివరి_changedtick_i = CHANGEDTICK(curbuf);

		op_చొప్పించు(oap, cap->count1);
#నిర్వచితమైతే FEAT_LINEBREAK
		// Reset linebreak, so that formatting works correctly.
		(శూన్య)reset_lbr();
#అయితే_ముగింపు

		// TODO: when inserting in several lines, should format all
		// the lines.
		auto_format(FALSE, TRUE);

		ఒకవేళ (restart_edit == 0)
		    restart_edit = restart_edit_save;
		లేకపోతే
		    cap->retval |= CA_COMMAND_BUSY;
	    }
	    ఆపు;

	స్థితి OP_REPLACE:
	    VIsual_reselect = FALSE;	// don't reselect now
	    ఒకవేళ (ఖాళీలు_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    లేకపోతే
	    {
#నిర్వచితమైతే FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#అయితే_ముగింపు
		op_replace(oap, cap->nchar);
	    }
	    ఆపు;

#నిర్వచితమైతే FEAT_FOLDING
	స్థితి OP_FOLD:
	    VIsual_reselect = FALSE;	// don't reselect now
	    foldCreate(oap->ప్రారంభం.lnum, oap->చివర.lnum);
	    ఆపు;

	స్థితి OP_FOLDOPEN:
	స్థితి OP_FOLDOPENREC:
	స్థితి OP_FOLDCLOSE:
	స్థితి OP_FOLDCLOSEREC:
	    VIsual_reselect = FALSE;	// don't reselect now
	    opFoldRange(oap->ప్రారంభం.lnum, oap->చివర.lnum,
		    oap->op_type == OP_FOLDOPEN
					    || oap->op_type == OP_FOLDOPENREC,
		    oap->op_type == OP_FOLDOPENREC
					  || oap->op_type == OP_FOLDCLOSEREC,
					  oap->is_VIsual);
	    ఆపు;

	స్థితి OP_FOLDDEL:
	స్థితి OP_FOLDDELREC:
	    VIsual_reselect = FALSE;	// don't reselect now
	    deleteFold(oap->ప్రారంభం.lnum, oap->చివర.lnum,
			       oap->op_type == OP_FOLDDELREC, oap->is_VIsual);
	    ఆపు;
#అయితే_ముగింపు
	స్థితి OP_NR_కూడు:
	స్థితి OP_NR_SUB:
	    ఒకవేళ (ఖాళీలు_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    లేకపోతే
	    {
		VIsual_చురుకైన = TRUE;
#నిర్వచితమైతే FEAT_LINEBREAK
		restore_lbr(lbr_saved);
#అయితే_ముగింపు
		op_addsub(oap, cap->count1, redo_VIsual.rv_arg);
		VIsual_చురుకైన = FALSE;
	    }
	    check_cursor_నిలువు();
	    ఆపు;
	అప్రమేయ:
	    clearopbeep(oap);
	}
	virtual_op = MAYBE;
	ఒకవేళ (!gui_yank)
	{
	    // if 'sol' not set, go back to old column for some commands
	    ఒకవేళ (!p_sol && oap->motion_type == MLINE && !oap->చివర_adjusted
		    && (oap->op_type == OP_LSHIFT || oap->op_type == OP_RSHIFT
						|| oap->op_type == OP_DELETE))
	    {
#నిర్వచితమైతే FEAT_LINEBREAK
		(శూన్య)reset_lbr();
#అయితే_ముగింపు
		coladvance(curwin->w_curswant = old_నిలువు);
	    }
	}
	లేకపోతే
	{
	    curwin->w_cursor = old_cursor;
	}
	oap->ఖండం_బహుళకం = FALSE;
	clearop(oap);
	motion_force = NUL;
    }
#నిర్వచితమైతే FEAT_LINEBREAK
    restore_lbr(lbr_saved);
#అయితే_ముగింపు
}

// put byte 'c' at position 'lp', but
// verify, that the position to place
// is actually safe
    స్థిర శూన్య
pbyte(pos_T lp, పూర్ణాంకం c)
{
    char_u *p = ml_get_buf(curbuf, lp.lnum, TRUE);
    పూర్ణాంకం	len = curbuf->b_ml.ml_పంక్తి_len;

    // safety check
    ఒకవేళ (lp.నిలువు >= len)
	lp.నిలువు = (len > 1 ? len - 2 : 0);
    *(p + lp.నిలువు) = c;
}
