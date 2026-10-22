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

#അന്തര്ഭാവ "vim.h"

സ്ഥിതം ശൂന്യം shift_block(oparg_T *oap, പൂർണ്ണസംഖ്യ amount);
സ്ഥിതം ശൂന്യം	mb_adjust_opend(oparg_T *oap);
സ്ഥിതം പൂർണ്ണസംഖ്യ	do_addsub(പൂർണ്ണസംഖ്യ op_type, pos_T *pos, പൂർണ്ണസംഖ്യ length, linenr_T Prenum1);
സ്ഥിതം ശൂന്യം	pbyte(pos_T lp, പൂർണ്ണസംഖ്യ c);
#define PBYTE(lp, c) pbyte(lp, c)


// Flags for third item in "opchars".
#define OPF_വരികൾ  1	// operator always works on lines
#define OPF_CHANGE 2	// operator changes text

/*
 * The names of operators.
 * IMPORTANT: Index must correspond with defines in vim.h!!!
 * The third field holds OPF_ flags.
 */
സ്ഥിതം സ്ഥിരം അക്ഷരം opchars[][3] =
{
    {NUL, NUL, 0},			// OP_NOP
    {'d', NUL, OPF_CHANGE},		// OP_DELETE
    {'y', NUL, 0},			// OP_YANK
    {'c', NUL, OPF_CHANGE},		// OP_CHANGE
    {'<', NUL, OPF_വരികൾ | OPF_CHANGE},	// OP_LSHIFT
    {'>', NUL, OPF_വരികൾ | OPF_CHANGE},	// OP_RSHIFT
    {'!', NUL, OPF_വരികൾ | OPF_CHANGE},	// OP_FILTER
    {'g', '~', OPF_CHANGE},		// OP_TILDE
    {'=', NUL, OPF_വരികൾ | OPF_CHANGE},	// OP_INDENT
    {'g', 'q', OPF_വരികൾ | OPF_CHANGE},	// OP_FORMAT
    {':', NUL, OPF_വരികൾ},		// OP_COLON
    {'g', 'U', OPF_CHANGE},		// OP_UPPER
    {'g', 'u', OPF_CHANGE},		// OP_LOWER
    {'J', NUL, OPF_വരികൾ | OPF_CHANGE},	// DO_JOIN
    {'g', 'J', OPF_വരികൾ | OPF_CHANGE},	// DO_JOIN_NS
    {'g', '?', OPF_CHANGE},		// OP_ROT13
    {'r', NUL, OPF_CHANGE},		// OP_REPLACE
    {'I', NUL, OPF_CHANGE},		// OP_INSERT
    {'A', NUL, OPF_CHANGE},		// OP_APPEND
    {'z', 'f', OPF_വരികൾ},		// OP_FOLD
    {'z', 'o', OPF_വരികൾ},		// OP_FOLDOPEN
    {'z', 'O', OPF_വരികൾ},		// OP_FOLDOPENREC
    {'z', 'c', OPF_വരികൾ},		// OP_FOLDCLOSE
    {'z', 'C', OPF_വരികൾ},		// OP_FOLDCLOSEREC
    {'z', 'd', OPF_വരികൾ},		// OP_FOLDDEL
    {'z', 'D', OPF_വരികൾ},		// OP_FOLDDELREC
    {'g', 'w', OPF_വരികൾ | OPF_CHANGE},	// OP_FORMAT2
    {'g', '@', OPF_CHANGE},		// OP_FUNCTION
    {Ctrl_A, NUL, OPF_CHANGE},		// OP_NR_ADD
    {Ctrl_X, NUL, OPF_CHANGE},		// OP_NR_SUB
};

/*
 * Translate a command name into an operator type.
 * Must only be called with a valid operator name!
 */
    പൂർണ്ണസംഖ്യ
get_op_type(പൂർണ്ണസംഖ്യ char1, പൂർണ്ണസംഖ്യ char2)
{
    പൂർണ്ണസംഖ്യ		i;

    എങ്കിൽ (char1 == 'r')		// ignore second character
	തിരികെ OP_REPLACE;
    എങ്കിൽ (char1 == '~')		// when tilde is an operator
	തിരികെ OP_TILDE;
    എങ്കിൽ (char1 == 'g' && char2 == Ctrl_A)	// add
	തിരികെ OP_NR_ADD;
    എങ്കിൽ (char1 == 'g' && char2 == Ctrl_X)	// subtract
	തിരികെ OP_NR_SUB;
    എങ്കിൽ (char1 == 'z' && char2 == 'y')	// OP_YANK
	തിരികെ OP_YANK;
    വേണ്ടി (i = 0; ; ++i)
    {
	എങ്കിൽ (opchars[i][0] == char1 && opchars[i][1] == char2)
	    നിർത്തുക;
	എങ്കിൽ (i == (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(opchars) - 1)
	{
	    internal_error("get_op_type()");
	    നിർത്തുക;
	}
    }
    തിരികെ i;
}

/*
 * Return TRUE if operator "op" always works on whole lines.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
op_on_വരികൾ(പൂർണ്ണസംഖ്യ op)
{
    തിരികെ opchars[op][2] & OPF_വരികൾ;
}

#എങ്കിൽ defined(FEAT_JOB_CHANNEL)
/*
 * Return TRUE if operator "op" changes text.
 */
    പൂർണ്ണസംഖ്യ
op_is_change(പൂർണ്ണസംഖ്യ op)
{
    തിരികെ opchars[op][2] & OPF_CHANGE;
}
#endif

/*
 * Get first operator command character.
 * Returns 'g' or 'z' if there is another command character.
 */
    പൂർണ്ണസംഖ്യ
get_op_char(പൂർണ്ണസംഖ്യ optype)
{
    തിരികെ opchars[optype][0];
}

/*
 * Get second operator command character.
 */
    പൂർണ്ണസംഖ്യ
get_extra_op_char(പൂർണ്ണസംഖ്യ optype)
{
    തിരികെ opchars[optype][1];
}

/*
 * op_shift - handle a shift operation
 */
    ശൂന്യം
op_shift(oparg_T *oap, പൂർണ്ണസംഖ്യ curs_മുകൾ, പൂർണ്ണസംഖ്യ amount)
{
    long	    i;
    പൂർണ്ണസംഖ്യ		    first_char;
    പൂർണ്ണസംഖ്യ		    block_col = 0;

    എങ്കിൽ (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	തിരികെ;

    എങ്കിൽ (oap->block_mode)
	block_col = curwin->w_cursor.col;

    വേണ്ടി (i = oap->വരി_എണ്ണം; --i >= 0; )
    {
	first_char = *ml_get_curline();
	എങ്കിൽ (first_char == NUL)				// empty line
	    curwin->w_cursor.col = 0;
	അല്ലെങ്കിൽ എങ്കിൽ (oap->block_mode)
	    shift_block(oap, amount);
	അല്ലെങ്കിൽ
	    // Move the line right if it doesn't start with '#', 'smartindent'
	    // isn't set or 'cindent' isn't set or '#' isn't in 'cino'.
	    എങ്കിൽ (first_char != '#' || !preprocs_ഇടത്())
		shift_വരി(oap->op_type == OP_LSHIFT, p_sr, amount, FALSE);
	++curwin->w_cursor.lnum;
    }

    changed_വരികൾ(oap->start.lnum, 0, oap->end.lnum + 1, 0L);
    എങ്കിൽ (oap->block_mode)
    {
	curwin->w_cursor.lnum = oap->start.lnum;
	curwin->w_cursor.col = block_col;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (curs_മുകൾ)	    // put cursor on first line, for ">>"
    {
	curwin->w_cursor.lnum = oap->start.lnum;
	beginline(BL_SOL | BL_FIX);   // shift_line() may have set cursor.col
    }
    അല്ലെങ്കിൽ
	--curwin->w_cursor.lnum;	// put cursor on last line, for ":>"

#ifdef FEAT_FOLDING
    // The cursor line is not in a closed fold
    foldOpenCursor();
#endif


    എങ്കിൽ (oap->വരി_എണ്ണം > p_report)
    {
	അക്ഷരം	    *op;
	അക്ഷരം	    *സന്ദേശം_വരി_single;
	അക്ഷരം	    *സന്ദേശം_വരി_plural;

	എങ്കിൽ (oap->op_type == OP_RSHIFT)
	    op = ">";
	അല്ലെങ്കിൽ
	    op = "<";
	സന്ദേശം_വരി_single = NGETTEXT("%ld line %sed %d time",
					     "%ld line %sed %d times", amount);
	സന്ദേശം_വരി_plural = NGETTEXT("%ld lines %sed %d time",
					    "%ld lines %sed %d times", amount);
	vim_snprintf((അക്ഷരം *)IObuff, IOSIZE,
		NGETTEXT(സന്ദേശം_വരി_single, സന്ദേശം_വരി_plural, oap->വരി_എണ്ണം),
		oap->വരി_എണ്ണം, op, amount);
	സന്ദേശം_attr_keep((അക്ഷരം *)IObuff, 0, TRUE);
    }

    എങ്കിൽ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end.lnum = oap->end.lnum;
	curbuf->b_op_end.col = ml_get_len(oap->end.lnum);
	എങ്കിൽ (curbuf->b_op_end.col > 0)
	    --curbuf->b_op_end.col;
    }
}

#ifdef FEAT_VARTABS
/*
 * Return the tabstop width at the index of the variable tabstop array.  If an
 * index greater than the length of the array is given, the last tabstop width
 * in the array is returned.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_vts(പൂർണ്ണസംഖ്യ *vts_array, പൂർണ്ണസംഖ്യ index)
{
    പൂർണ്ണസംഖ്യ	ts;

    എങ്കിൽ (index < 1)
	ts = 0;
    അല്ലെങ്കിൽ എങ്കിൽ (index <= vts_array[0])
	ts = vts_array[index];
    അല്ലെങ്കിൽ
	ts = vts_array[vts_array[0]];

    തിരികെ ts;
}

/*
 * Return the sum of all the tabstops through the index-th.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_vts_sum(പൂർണ്ണസംഖ്യ *vts_array, പൂർണ്ണസംഖ്യ index)
{
    പൂർണ്ണസംഖ്യ	sum = 0;
    പൂർണ്ണസംഖ്യ	i;

    // Perform the summation for indices within the actual array.
    വേണ്ടി (i = 1; i <= index && i <= vts_array[0]; i++)
	sum += vts_array[i];

    // Add tabstops whose indices exceed the actual array.
    എങ്കിൽ (i <= index)
	sum += vts_array[vts_array[0]] * (index - vts_array[0]);

    തിരികെ sum;
}
#endif

    സ്ഥിതം vimlong_T
get_new_sw_indent(
    പൂർണ്ണസംഖ്യ		ഇടത്,		// TRUE if shift is to the left
    പൂർണ്ണസംഖ്യ		round,		// TRUE if new indent is to be to a tabstop
    vimlong_T	amount,		// Number of shifts
    vimlong_T	sw_val)
{
    vimlong_T	എണ്ണം = get_indent();
    vimlong_T	i, j;

    എങ്കിൽ (round)			// round off indent
    {
	i = trim_to_int(എണ്ണം) / sw_val;	// number of 'shiftwidth' rounded down
	j = trim_to_int(എണ്ണം) % sw_val;	// extra spaces
	എങ്കിൽ (j && ഇടത്)		// first remove extra spaces
	    --amount;
	എങ്കിൽ (ഇടത്)
	{
	    i -= amount;
	    എങ്കിൽ (i < 0)
		i = 0;
	}
	അല്ലെങ്കിൽ
	    i += amount;
	എണ്ണം = i * sw_val;
    }
    അല്ലെങ്കിൽ			// original vi indent
    {
	എങ്കിൽ (ഇടത്)
	{
	    എണ്ണം -= sw_val * amount;
	    എങ്കിൽ (എണ്ണം < 0)
		എണ്ണം = 0;
	}
	അല്ലെങ്കിൽ
	    എണ്ണം += sw_val * amount;
    }

    തിരികെ എണ്ണം;
}

#ifdef FEAT_VARTABS
    സ്ഥിതം vimlong_T
get_new_vts_indent(
    പൂർണ്ണസംഖ്യ	ഇടത്,			// TRUE if shift is to the left
    പൂർണ്ണസംഖ്യ	round,			// TRUE if new indent is to be to a tabstop
    പൂർണ്ണസംഖ്യ	amount,			// Number of shifts
    പൂർണ്ണസംഖ്യ	*vts_array)
{
    vimlong_T	indent = get_indent();
    പൂർണ്ണസംഖ്യ		vtsi = 0;
    പൂർണ്ണസംഖ്യ		vts_indent = 0;
    പൂർണ്ണസംഖ്യ		ts = 0;		// Silence uninitialized variable warning.
    പൂർണ്ണസംഖ്യ		offset;		// Extra indent spaces to the right of the
				// tabstop

    // Find the tabstop at or to the left of the current indent.
    ആകുമ്പോൾ (vts_indent <= indent)
    {
	vtsi++;
	ts = get_vts(vts_array, vtsi);
	vts_indent += ts;
    }
    vts_indent -= ts;
    vtsi--;

    offset = indent - vts_indent;

    എങ്കിൽ (round)
    {
	എങ്കിൽ (ഇടത്)
	{
	    എങ്കിൽ (offset == 0)
		indent = get_vts_sum(vts_array, vtsi - amount);
	    അല്ലെങ്കിൽ
		indent = get_vts_sum(vts_array, vtsi - (amount - 1));
	}
	അല്ലെങ്കിൽ
	    indent = get_vts_sum(vts_array, vtsi + amount);
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (ഇടത്)
	{
	    എങ്കിൽ (amount > vtsi)
		indent = 0;
	    അല്ലെങ്കിൽ
		indent = get_vts_sum(vts_array, vtsi - amount) + offset;
	}
	അല്ലെങ്കിൽ
	    indent = get_vts_sum(vts_array, vtsi + amount) + offset;
    }

    തിരികെ indent;
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
    ശൂന്യം
shift_വരി(
    പൂർണ്ണസംഖ്യ	ഇടത്,			// TRUE if shift is to the left
    പൂർണ്ണസംഖ്യ	round,			// TRUE if new indent is to be to a tabstop
    പൂർണ്ണസംഖ്യ	amount,			// Number of shifts
    പൂർണ്ണസംഖ്യ	call_changed_bytes)	// call changed_bytes()
{
    vimlong_T	എണ്ണം;
    long	sw_val = curbuf->b_p_sw;
    long	ts_val = curbuf->b_p_ts;
#ifdef FEAT_VARTABS
    പൂർണ്ണസംഖ്യ		*vts_array = curbuf->b_p_vts_array;
#endif

    എങ്കിൽ (sw_val != 0)
	// 'shiftwidth' is not zero; use it as the shift size.
	എണ്ണം = get_new_sw_indent(ഇടത്, round, amount, sw_val);
    അല്ലെങ്കിൽ
#ifdef FEAT_VARTABS
	എങ്കിൽ ((vts_array == ശൂന്യ) || (vts_array[0] == 0))
#endif
    {
	// 'shiftwidth is zero and 'vartabstop' is empty; use 'tabstop' as the
	// shift size.
	എണ്ണം = get_new_sw_indent(ഇടത്, round, amount, ts_val);
    }
#ifdef FEAT_VARTABS
    അല്ലെങ്കിൽ
    {
	// 'shiftwidth is zero and 'vartabstop' is defined; use 'vartabstop'
	// to determine the new indent.
	എണ്ണം = get_new_vts_indent(ഇടത്, round, amount, vts_array);
    }
#endif

    // Set new indent
    എങ്കിൽ (State & VREPLACE_FLAG)
	change_indent(INDENT_SET, trim_to_int(എണ്ണം), FALSE, NUL, call_changed_bytes);
    അല്ലെങ്കിൽ
	(ശൂന്യം)set_indent(trim_to_int(എണ്ണം), call_changed_bytes ? SIN_CHANGED : 0);
}

/*
 * Shift one line of the current block one shiftwidth right or left.
 * Leaves cursor on first character in block.
 */
    സ്ഥിതം ശൂന്യം
shift_block(oparg_T *oap, പൂർണ്ണസംഖ്യ amount)
{
    പൂർണ്ണസംഖ്യ			ഇടത് = (oap->op_type == OP_LSHIFT);
    പൂർണ്ണസംഖ്യ			oldstate = State;
    പൂർണ്ണസംഖ്യ			total;
    char_u		*newp, *oldp;
    size_t		newlen, oldlen;
    പൂർണ്ണസംഖ്യ			oldcol = curwin->w_cursor.col;
    പൂർണ്ണസംഖ്യ			sw_val = (പൂർണ്ണസംഖ്യ)get_sw_മൂല്യം_indent(curbuf, ഇടത്);
    പൂർണ്ണസംഖ്യ			ts_val = (പൂർണ്ണസംഖ്യ)curbuf->b_p_ts;
    ഘടന block_def	bd;
    പൂർണ്ണസംഖ്യ			incr;
    colnr_T		ws_vcol;
    പൂർണ്ണസംഖ്യ			added;
    size_t		new_വരി_len;	// the length of the line after the
					// block shift
#ifdef FEAT_RIGHTLEFT
    പൂർണ്ണസംഖ്യ			old_p_ri = p_ri;

    p_ri = 0;			// don't want revins in indent
#endif

    State = MODE_തിരുകുക;	// don't want MODE_REPLACE for State
    block_prep(oap, &bd, curwin->w_cursor.lnum, TRUE);
    എങ്കിൽ (bd.is_short)
	തിരികെ;

    // total is number of screen columns to be inserted/removed
    total = (പൂർണ്ണസംഖ്യ)((unsigned)amount * (unsigned)sw_val);
    എങ്കിൽ ((total / sw_val) != amount)
	തിരികെ; // multiplication overflow

    oldp = ml_get_curline();
    oldlen = ml_get_curline_len();

    എങ്കിൽ (!ഇടത്)
    {
	പൂർണ്ണസംഖ്യ		tabs = 0, spaces = 0;
	chartabsize_T	cts;

	/*
	 *  1. Get start vcol
	 *  2. Total ws vcols
	 *  3. Divvy into TABs & spp
	 *  4. Construct new string
	 */
	total += bd.pre_whitesp; // all virtual WS up to & incl a split TAB
	ws_vcol = bd.start_vcol - bd.pre_whitesp;
	എങ്കിൽ (bd.startspaces)
	{
	    എങ്കിൽ (has_mbyte)
	    {
		എങ്കിൽ ((*mb_ptr2len)(bd.textstart) == 1)
		    ++bd.textstart;
		അല്ലെങ്കിൽ
		{
		    ws_vcol = 0;
		    bd.startspaces = 0;
		}
	    }
	    അല്ലെങ്കിൽ
		++bd.textstart;
	}

	// TODO: is passing bd.textstart for start of the line OK?
	init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum,
				   bd.start_vcol, bd.textstart, bd.textstart);
	വേണ്ടി ( ; VIM_ISWHITE(*cts.cts_ptr); )
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
	എങ്കിൽ (!curbuf->b_p_et)
	    tabstop_fromto(ws_vcol, ws_vcol + total,
				ts_val, curbuf->b_p_vts_array, &tabs, &spaces);
	അല്ലെങ്കിൽ
	    spaces = total;
#അല്ലെങ്കിൽ
	എങ്കിൽ (!curbuf->b_p_et)
	    tabs = ((ws_vcol % ts_val) + total) / ts_val; // number of tabs
	എങ്കിൽ (tabs > 0)
	    spaces = ((ws_vcol % ts_val) + total) % ts_val; // number of spp
	അല്ലെങ്കിൽ
	    spaces = total;
#endif
	// if we're splitting a TAB, allow for it
	bd.textcol -= bd.pre_whitesp_c - (bd.startspaces != 0);

	new_വരി_len = bd.textcol + tabs + spaces + (oldlen - (bd.textstart - oldp));
	newp = alloc(new_വരി_len + 1);
	എങ്കിൽ (newp == ശൂന്യ)
	    തിരികെ;
	mch_memmove(newp, oldp, (size_t)bd.textcol);
	newlen = bd.textcol;
	vim_memset(newp + newlen, TAB, (size_t)tabs);
	newlen += tabs;
	vim_memset(newp + newlen, ' ', (size_t)spaces);
	STRCPY(newp + newlen + spaces, bd.textstart);
    }
    അല്ലെങ്കിൽ //ഇടത്
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
	എങ്കിൽ (bd.startspaces)
	    MB_PTR_ADV(non_white);

	// The character's column is in "bd.start_vcol".
	non_white_col = bd.start_vcol;

	init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum,
				   non_white_col, bd.textstart, non_white);
	ആകുമ്പോൾ (VIM_ISWHITE(*cts.cts_ptr))
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
	എങ്കിൽ (bd.startspaces)
	    verbatim_copy_width -= bd.start_char_vcols;
	init_chartabsize_arg(&cts, curwin, 0, verbatim_copy_width,
					     bd.textstart, verbatim_copy_end);
	ആകുമ്പോൾ (cts.cts_vcol < destination_col)
	{
	    incr = lbr_chartabsize(&cts);
	    എങ്കിൽ (cts.cts_vcol + incr > destination_col)
		നിർത്തുക;
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
	new_വരി_len = fixedlen + fill + (oldlen - (non_white - oldp));

	newp = alloc(new_വരി_len + 1);
	എങ്കിൽ (newp == ശൂന്യ)
	    തിരികെ;
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
    എങ്കിൽ (new_വരി_len >= oldlen)
	added = (പൂർണ്ണസംഖ്യ)(new_വരി_len - oldlen);
    അല്ലെങ്കിൽ
	added = 0 - (പൂർണ്ണസംഖ്യ)(oldlen - new_വരി_len);
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
    സ്ഥിതം ശൂന്യം
block_തിരുകുക(
    oparg_T		*oap,
    char_u		*s,
    size_t		slen,
    പൂർണ്ണസംഖ്യ			b_തിരുകുക,
    ഘടന block_def	*bdp)
{
    പൂർണ്ണസംഖ്യ		ts_val;
    പൂർണ്ണസംഖ്യ		എണ്ണം = 0;	// extra spaces to replace a cut TAB
    പൂർണ്ണസംഖ്യ		spaces = 0;	// non-zero if cutting a TAB
    colnr_T	offset;		// pointer along new line
    colnr_T	startcol;	// column where insert starts
    char_u	*newp, *oldp;	// new, old lines
    linenr_T	lnum;		// loop var
    പൂർണ്ണസംഖ്യ		oldstate = State;

    State = MODE_തിരുകുക;	// don't want MODE_REPLACE for State

    വേണ്ടി (lnum = oap->start.lnum + 1; lnum <= oap->end.lnum; lnum++)
    {
	block_prep(oap, bdp, lnum, TRUE);
	എങ്കിൽ (bdp->is_short && b_തിരുകുക)
	    തുടരുക;	// OP_INSERT, line ends before block start

	oldp = ml_get(lnum);

	എങ്കിൽ (b_തിരുകുക)
	{
	    ts_val = bdp->start_char_vcols;
	    spaces = bdp->startspaces;
	    എങ്കിൽ (spaces != 0)
		എണ്ണം = ts_val - 1; // we're cutting a TAB
	    offset = bdp->textcol;
	}
	അല്ലെങ്കിൽ // append
	{
	    ts_val = bdp->end_char_vcols;
	    എങ്കിൽ (!bdp->is_short) // spaces = padding after block
	    {
		spaces = (bdp->endspaces ? ts_val - bdp->endspaces : 0);
		എങ്കിൽ (spaces != 0)
		    എണ്ണം = ts_val - 1; // we're cutting a TAB
		offset = bdp->textcol + bdp->textlen - (spaces != 0);
	    }
	    അല്ലെങ്കിൽ // spaces = padding to block edge
	    {
		// if $ used, just append to EOL (ie spaces==0)
		എങ്കിൽ (!bdp->is_MAX)
		    spaces = (oap->end_vcol - bdp->end_vcol) + 1;
		എണ്ണം = spaces;
		offset = bdp->textcol + bdp->textlen;
	    }
	}

	എങ്കിൽ (has_mbyte && spaces > 0)
	    // avoid copying part of a multi-byte character
	    offset -= (*mb_head_off)(oldp, oldp + offset);

	എങ്കിൽ (spaces < 0)  // can happen when the cursor was moved
	    spaces = 0;

	// Make sure the allocated size matches what is actually copied below.
	newp = alloc(ml_get_len(lnum) + spaces + slen
		    + (spaces > 0 && !bdp->is_short ? ts_val - spaces : 0)
								  + എണ്ണം + 1);
	എങ്കിൽ (newp == ശൂന്യ)
	    തുടരുക;

	// copy up to shifted part
	mch_memmove(newp, oldp, (size_t)offset);
	oldp += offset;

	// insert pre-padding
	vim_memset(newp + offset, ' ', (size_t)spaces);
	startcol = offset + spaces;

	// copy the new text
	mch_memmove(newp + startcol, s, slen);
	offset += (പൂർണ്ണസംഖ്യ)slen;

	എങ്കിൽ (spaces > 0 && !bdp->is_short)
	{
	    എങ്കിൽ (*oldp == TAB)
	    {
		// insert post-padding
		vim_memset(newp + offset + spaces, ' ',
						    (size_t)(ts_val - spaces));
		// we're splitting a TAB, don't copy it
		oldp++;
		// We allowed for that TAB, remember this now
		എണ്ണം++;
	    }
	    അല്ലെങ്കിൽ
		// Not a TAB, no extra spaces
		എണ്ണം = spaces;
	}

	എങ്കിൽ (spaces > 0)
	    offset += എണ്ണം;
	STRCPY(newp + offset, oldp);

	ml_replace(lnum, newp, FALSE);

	എങ്കിൽ (b_തിരുകുക)
	    // correct any text properties
	    inserted_bytes(lnum, startcol, (പൂർണ്ണസംഖ്യ)slen);

	എങ്കിൽ (lnum == oap->end.lnum)
	{
	    // Set "']" mark to the end of the block instead of the end of
	    // the insert in the first line.
	    curbuf->b_op_end.lnum = oap->end.lnum;
	    curbuf->b_op_end.col = offset;
	    എങ്കിൽ (curbuf->b_visual.vi_end.coladd)
	    {
		curbuf->b_visual.vi_end.col += curbuf->b_visual.vi_end.coladd;
		curbuf->b_visual.vi_end.coladd = 0;
	    }
	}
    } // for all lnum

    changed_വരികൾ(oap->start.lnum + 1, 0, oap->end.lnum + 1, 0L);

    State = oldstate;
}

/*
 * Handle a delete operation.
 *
 * Return FAIL if undo failed, OK otherwise.
 */
    പൂർണ്ണസംഖ്യ
op_delete(oparg_T *oap)
{
    പൂർണ്ണസംഖ്യ			n;
    linenr_T		lnum;
    char_u		*ptr;
    char_u		*newp, *oldp;
    ഘടന block_def	bd;
    linenr_T		old_lcount = curbuf->b_ml.ml_വരി_എണ്ണം;
    പൂർണ്ണസംഖ്യ			did_yank = FALSE;

    എങ്കിൽ (curbuf->b_ml.ml_flags & ML_EMPTY)	    // nothing to do
	തിരികെ OK;

    // Nothing to delete, return here.	Do prepare undo, for op_change().
    എങ്കിൽ (oap->empty)
	തിരികെ u_save_cursor();

    എങ്കിൽ (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	തിരികെ FAIL;
    }

    എങ്കിൽ (VIsual_select && oap->is_VIsual)
	// use register given with CTRL_R, defaults to zero
	oap->regname = VIsual_select_reg;

#ifdef HAVE_CLIPMETHOD
    adjust_clip_reg(&oap->regname);
#endif

    എങ്കിൽ (has_mbyte)
	mb_adjust_opend(oap);

    /*
     * Imitate the strange Vi behaviour: If the delete spans more than one
     * line and motion_type == MCHAR and the result is a blank line, make the
     * delete linewise.  Don't do this for the change command or Visual mode.
     */
    എങ്കിൽ (       oap->motion_type == MCHAR
	    && !oap->is_VIsual
	    && !oap->block_mode
	    && oap->വരി_എണ്ണം > 1
	    && oap->motion_force == NUL
	    && (vim_strchr(p_cpo, CPO_WORD) != ശൂന്യ)
	    && oap->op_type == OP_DELETE)
    {
	ptr = ml_get(oap->end.lnum) + oap->end.col;
	എങ്കിൽ (*ptr != NUL)
	    ptr += oap->inclusive;
	ptr = skipwhite(ptr);
	എങ്കിൽ (*ptr == NUL && inindent(0))
	    oap->motion_type = MLINE;
    }

    /*
     * Check for trying to delete (e.g. "D") in an empty line.
     * Note: For the change operator it is ok.
     */
    എങ്കിൽ (       oap->motion_type == MCHAR
	    && oap->വരി_എണ്ണം == 1
	    && oap->op_type == OP_DELETE
	    && *ml_get(oap->start.lnum) == NUL)
    {
	/*
	 * It's an error to operate on an empty region, when 'E' included in
	 * 'cpoptions' (Vi compatible).
	 */
	എങ്കിൽ (virtual_op)
	    // Virtual editing: Nothing gets deleted, but we set the '[ and ']
	    // marks as if it happened.
	    പോകുക setmarks;
	എങ്കിൽ (vim_strchr(p_cpo, CPO_EMPTYREGION) != ശൂന്യ)
	    beep_flush();
	തിരികെ OK;
    }

    /*
     * Do a yank of whatever we're about to delete.
     * If a yank register was specified, put the deleted text into that
     * register.  For the black hole register '_' don't yank anything.
     */
    എങ്കിൽ (oap->regname != '_')
    {
#ifdef FEAT_CLIPBOARD_PROVIDER
	inc_clip_provider();
#endif
	എങ്കിൽ (oap->regname != 0)
	{
	    // check for read-only register
	    എങ്കിൽ (!valid_yank_reg(oap->regname, TRUE))
	    {
		beep_flush();
#ifdef FEAT_CLIPBOARD_PROVIDER
		dec_clip_provider();
#endif
		തിരികെ OK;
	    }
	    get_yank_register(oap->regname, TRUE); // yank into specif'd reg.
	    എങ്കിൽ (op_yank(oap, TRUE, FALSE) == OK)   // yank without message
		did_yank = TRUE;
	}
	അല്ലെങ്കിൽ
	    reset_y_append(); // not appending to unnamed register

	/*
	 * Put deleted text into register 1 and shift number registers if the
	 * delete contains a line break, or when using a specific operator (Vi
	 * compatible)
	 */
	എങ്കിൽ (oap->motion_type == MLINE || oap->വരി_എണ്ണം > 1
							   || oap->use_reg_one)
	{
	    shift_delete_registers();
	    എങ്കിൽ (op_yank(oap, TRUE, FALSE) == OK)
		did_yank = TRUE;
	}

	// Yank into small delete register when no named register specified
	// and the delete is within one line.
	എങ്കിൽ ((
#ifdef HAVE_CLIPMETHOD
	    ((clip_unnamed & CLIP_UNNAMED) && oap->regname == '*') ||
	    ((clip_unnamed & CLIP_UNNAMED_PLUS) && oap->regname == '+') ||
#endif
	    oap->regname == 0) && oap->motion_type != MLINE
						      && oap->വരി_എണ്ണം == 1)
	{
	    oap->regname = '-';
	    get_yank_register(oap->regname, TRUE);
	    എങ്കിൽ (op_yank(oap, TRUE, FALSE) == OK)
		did_yank = TRUE;
	    oap->regname = 0;
	}

	/*
	 * If there's too much stuff to fit in the yank register, then get a
	 * confirmation before doing the delete. This is crude, but simple.
	 * And it avoids doing a delete of something we can't put back if we
	 * want.
	 */
	എങ്കിൽ (!did_yank)
	{
	    പൂർണ്ണസംഖ്യ സന്ദേശം_silent_save = സന്ദേശം_silent;

	    സന്ദേശം_silent = 0;	// must display the prompt
	    n = ask_yesno((char_u *)_("cannot yank; delete anyway"), TRUE);
	    സന്ദേശം_silent = സന്ദേശം_silent_save;
	    എങ്കിൽ (n != 'y')
	    {
		emsg(_(e_command_aborted));
#ifdef FEAT_CLIPBOARD_PROVIDER
		dec_clip_provider();
#endif
		തിരികെ FAIL;
	    }
	}

#എങ്കിൽ defined(FEAT_EVAL)
	എങ്കിൽ (did_yank && has_textyankpost())
	    yank_do_autocmd(oap, get_y_current());
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
    }

    /*
     * block mode delete
     */
    എങ്കിൽ (oap->block_mode)
    {
	എങ്കിൽ (u_save((linenr_T)(oap->start.lnum - 1),
			       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	    തിരികെ FAIL;

	വേണ്ടി (lnum = curwin->w_cursor.lnum; lnum <= oap->end.lnum; ++lnum)
	{
	    block_prep(oap, &bd, lnum, TRUE);
	    എങ്കിൽ (bd.textlen == 0)	// nothing to delete
		തുടരുക;

	    // Adjust cursor position for tab replaced by spaces and 'lbr'.
	    എങ്കിൽ (lnum == curwin->w_cursor.lnum)
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
	    എങ്കിൽ (newp == ശൂന്യ)
		തുടരുക;
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
	    എങ്കിൽ (curbuf->b_has_textprop && n != 0)
		adjust_prop_columns(lnum, bd.textcol, -n, 0);
#endif
	}

	check_cursor_col();
	changed_വരികൾ(curwin->w_cursor.lnum, curwin->w_cursor.col,
						       oap->end.lnum + 1, 0L);
	oap->വരി_എണ്ണം = 0;	    // no lines deleted
    }
    അല്ലെങ്കിൽ എങ്കിൽ (oap->motion_type == MLINE)
    {
	എങ്കിൽ (oap->op_type == OP_CHANGE)
	{
	    // Delete the lines except the first one.  Temporarily move the
	    // cursor to the next line.  Save the current line number, if the
	    // last line is deleted it may be changed.
	    എങ്കിൽ (oap->വരി_എണ്ണം > 1)
	    {
		lnum = curwin->w_cursor.lnum;
		++curwin->w_cursor.lnum;
		del_വരികൾ((long)(oap->വരി_എണ്ണം - 1), TRUE);
		curwin->w_cursor.lnum = lnum;
	    }
	    എങ്കിൽ (u_save_cursor() == FAIL)
		തിരികെ FAIL;
	    എങ്കിൽ (curbuf->b_p_ai)		    // don't delete indent
	    {
		beginline(BL_WHITE);	    // cursor on first non-white
		did_ai = TRUE;		    // delete the indent when ESC hit
		ai_col = curwin->w_cursor.col;
	    }
	    അല്ലെങ്കിൽ
		beginline(0);		    // cursor in column 0
	    truncate_വരി(FALSE);   // delete the rest of the line,
				    // leaving cursor past last char in line
	    എങ്കിൽ (oap->വരി_എണ്ണം > 1)
		u_clearline();	    // "U" command not possible after "2cc"
	}
	അല്ലെങ്കിൽ
	{
	    del_വരികൾ(oap->വരി_എണ്ണം, TRUE);
	    beginline(BL_WHITE | BL_FIX);
	    u_clearline();	// "U" command not possible after "dd"
	}
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (virtual_op)
	{
	    പൂർണ്ണസംഖ്യ		endcol = 0;

	    // For virtualedit: break the tabs that are partly included.
	    എങ്കിൽ (gchar_pos(&oap->start) == '\t')
	    {
		എങ്കിൽ (u_save_cursor() == FAIL)	// save first line for undo
		    തിരികെ FAIL;
		എങ്കിൽ (oap->വരി_എണ്ണം == 1)
		    endcol = getviscol2(oap->end.col, oap->end.coladd);
		coladvance_force(getviscol2(oap->start.col, oap->start.coladd));
		oap->start = curwin->w_cursor;
		എങ്കിൽ (oap->വരി_എണ്ണം == 1)
		{
		    coladvance(endcol);
		    oap->end.col = curwin->w_cursor.col;
		    oap->end.coladd = curwin->w_cursor.coladd;
		    curwin->w_cursor = oap->start;
		}
	    }

	    // Break a tab only when it's included in the area.
	    എങ്കിൽ (gchar_pos(&oap->end) == '\t'
				     && (പൂർണ്ണസംഖ്യ)oap->end.coladd < oap->inclusive)
	    {
		// save last line for undo
		എങ്കിൽ (u_save((linenr_T)(oap->end.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
		    തിരികെ FAIL;
		curwin->w_cursor = oap->end;
		coladvance_force(getviscol2(oap->end.col, oap->end.coladd));
		oap->end = curwin->w_cursor;
		curwin->w_cursor = oap->start;
	    }
	    എങ്കിൽ (has_mbyte)
		mb_adjust_opend(oap);
	}

	എങ്കിൽ (oap->വരി_എണ്ണം == 1)	// delete characters within one line
	{
	    എങ്കിൽ (u_save_cursor() == FAIL)	// save line for undo
		തിരികെ FAIL;

	    // if 'cpoptions' contains '$', display '$' at end of change
	    എങ്കിൽ (       vim_strchr(p_cpo, CPO_DOLLAR) != ശൂന്യ
		    && oap->op_type == OP_CHANGE
		    && oap->end.lnum == curwin->w_cursor.lnum
		    && !oap->is_VIsual)
		display_dollar(oap->end.col - !oap->inclusive);

	    n = oap->end.col - oap->start.col + 1 - !oap->inclusive;

	    എങ്കിൽ (virtual_op)
	    {
		// fix up things for virtualedit-delete:
		// break the tabs which are going to get in our way
		പൂർണ്ണസംഖ്യ		len = ml_get_curline_len();

		എങ്കിൽ (oap->end.coladd != 0
			&& (പൂർണ്ണസംഖ്യ)oap->end.col >= len - 1
			&& !(oap->start.coladd && (പൂർണ്ണസംഖ്യ)oap->end.col >= len - 1))
		    n++;
		// Delete at least one char (e.g, when on a control char).
		എങ്കിൽ (n == 0 && oap->start.coladd != oap->end.coladd)
		    n = 1;

		// When deleted a char in the line, reset coladd.
		എങ്കിൽ (gchar_cursor() != NUL)
		    curwin->w_cursor.coladd = 0;
	    }
	    (ശൂന്യം)del_bytes((long)n, !virtual_op,
			    oap->op_type == OP_DELETE && !oap->is_VIsual);
	}
	അല്ലെങ്കിൽ				// delete characters between lines
	{
	    pos_T   curpos;

	    // save deleted and changed lines for undo
	    എങ്കിൽ (u_save((linenr_T)(curwin->w_cursor.lnum - 1),
		 (linenr_T)(curwin->w_cursor.lnum + oap->വരി_എണ്ണം)) == FAIL)
		തിരികെ FAIL;

	    truncate_വരി(TRUE);	// delete from cursor to end of line

	    curpos = curwin->w_cursor;	// remember curwin->w_cursor
	    ++curwin->w_cursor.lnum;
	    del_വരികൾ((long)(oap->വരി_എണ്ണം - 2), FALSE);

	    // delete from start of line until op_end
	    n = (oap->end.col + 1 - !oap->inclusive);
	    curwin->w_cursor.col = 0;
	    (ശൂന്യം)del_bytes((long)n, !virtual_op,
			    oap->op_type == OP_DELETE && !oap->is_VIsual);
	    curwin->w_cursor = curpos;	// restore curwin->w_cursor
	    (ശൂന്യം)do_join(2, FALSE, FALSE, FALSE, FALSE);
	}
	എങ്കിൽ (oap->op_type == OP_DELETE)
	    auto_format(FALSE, TRUE);
    }

    msgmore(curbuf->b_ml.ml_വരി_എണ്ണം - old_lcount);

setmarks:
    എങ്കിൽ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	എങ്കിൽ (oap->block_mode)
	{
	    curbuf->b_op_end.lnum = oap->end.lnum;
	    curbuf->b_op_end.col = oap->start.col;
	}
	അല്ലെങ്കിൽ
	    curbuf->b_op_end = oap->start;
	curbuf->b_op_start = oap->start;
    }

    തിരികെ OK;
}

/*
 * Adjust end of operating area for ending on a multi-byte character.
 * Used for deletion.
 */
    സ്ഥിതം ശൂന്യം
mb_adjust_opend(oparg_T *oap)
{
    char_u	*വരി;
    char_u	*ptr;

    എങ്കിൽ (!oap->inclusive)
	തിരികെ;

    വരി = ml_get(oap->end.lnum);
    ptr = വരി + oap->end.col;
    എങ്കിൽ (*ptr != NUL)
    {
	ptr -= (*mb_head_off)(വരി, ptr);
	ptr += (*mb_ptr2len)(ptr) - 1;
	oap->end.col = ptr - വരി;
    }
}

/*
 * Replace the character under the cursor with "c".
 * This takes care of multi-byte characters.
 */
    സ്ഥിതം ശൂന്യം
replace_character(പൂർണ്ണസംഖ്യ c)
{
    പൂർണ്ണസംഖ്യ n = State;

    State = MODE_REPLACE;
    ins_char(c);
    State = n;
    // Backup to the replaced character.
    dec_cursor();
}

/*
 * Replace a whole area with one character.
 */
    പൂർണ്ണസംഖ്യ
op_replace(oparg_T *oap, പൂർണ്ണസംഖ്യ c)
{
    പൂർണ്ണസംഖ്യ			n, numc;
    പൂർണ്ണസംഖ്യ			അക്കം_chars;
    char_u		*newp, *oldp;
    size_t		newlen, oldlen;
    ഘടന block_def	bd;
    char_u		*after_p = ശൂന്യ;
    പൂർണ്ണസംഖ്യ			had_ctrl_v_cr = FALSE;

    എങ്കിൽ ((curbuf->b_ml.ml_flags & ML_EMPTY ) || oap->empty)
	തിരികെ OK;	    // nothing to do

    എങ്കിൽ (c == REPLACE_CR_NCHAR)
    {
	had_ctrl_v_cr = TRUE;
	c = CAR;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (c == REPLACE_NL_NCHAR)
    {
	had_ctrl_v_cr = TRUE;
	c = NL;
    }

    എങ്കിൽ (has_mbyte)
	mb_adjust_opend(oap);

    എങ്കിൽ (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	തിരികെ FAIL;

    /*
     * block mode replace
     */
    എങ്കിൽ (oap->block_mode)
    {
	bd.is_MAX = (curwin->w_curswant == MAXCOL);
	വേണ്ടി ( ; curwin->w_cursor.lnum <= oap->end.lnum; ++curwin->w_cursor.lnum)
	{
	    curwin->w_cursor.col = 0;  // make sure cursor position is valid
	    block_prep(oap, &bd, curwin->w_cursor.lnum, TRUE);
	    എങ്കിൽ (bd.textlen == 0 && (!virtual_op || bd.is_MAX))
		തുടരുക;	    // nothing to replace

	    // n == number of extra chars required
	    // If we split a TAB, it may be replaced by several characters.
	    // Thus the number of characters may increase!
	    // If the range starts in virtual space, count the initial
	    // coladd offset as part of "startspaces"
	    എങ്കിൽ (virtual_op && bd.is_short && *bd.textstart == NUL)
	    {
		pos_T vpos;

		vpos.lnum = curwin->w_cursor.lnum;
		getvpos(&vpos, oap->start_vcol);
		bd.startspaces += vpos.coladd;
		n = bd.startspaces;
	    }
	    അല്ലെങ്കിൽ
		// allow for pre spaces
		n = (bd.startspaces ? bd.start_char_vcols - 1 : 0);

	    // allow for post spp
	    n += (bd.endspaces
		    && !bd.is_oneChar
		    && bd.end_char_vcols > 0) ? bd.end_char_vcols - 1 : 0;
	    // Figure out how many characters to replace.
	    numc = oap->end_vcol - oap->start_vcol + 1;
	    എങ്കിൽ (bd.is_short && (!virtual_op || bd.is_MAX))
		numc -= (oap->end_vcol - bd.end_vcol) + 1;

	    // A double-wide character can be replaced only up to half the
	    // times.
	    എങ്കിൽ ((*mb_char2cells)(c) > 1)
	    {
		എങ്കിൽ ((numc & 1) && !bd.is_short)
		{
		    ++bd.endspaces;
		    ++n;
		}
		numc = numc / 2;
	    }

	    // Compute bytes needed, move character count to num_chars.
	    അക്കം_chars = numc;
	    numc *= (*mb_char2len)(c);
	    // oldlen includes textlen, so don't double count
	    n += numc - bd.textlen;

	    oldp = ml_get_curline();
	    oldlen = ml_get_curline_len();
	    newp = alloc(oldlen + 1 + n);
	    എങ്കിൽ (newp == ശൂന്യ)
		തുടരുക;
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
	    എങ്കിൽ (had_ctrl_v_cr || (c != '\r' && c != '\n'))
	    {
		എങ്കിൽ (has_mbyte)
		{
		    ആകുമ്പോൾ (--അക്കം_chars >= 0)
			newlen += (*mb_char2bytes)(c, newp + newlen);
		}
		അല്ലെങ്കിൽ
		{
		    vim_memset(newp + newlen, c, (size_t)numc);
		    newlen += numc;
		}
		എങ്കിൽ (!bd.is_short)
		{
		    // insert post-spaces
		    vim_memset(newp + newlen, ' ', (size_t)bd.endspaces);
		    // copy the part after the changed part
		    STRCPY(newp + newlen + bd.endspaces,
				oldp + bd.textcol + bd.textlen);
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		// Replacing with \r or \n means splitting the line.
		after_p = alloc(oldlen + 1 + n - newlen);
		എങ്കിൽ (after_p != ശൂന്യ)
		    STRCPY(after_p, oldp + bd.textcol + bd.textlen);
	    }

	    // replace the line
	    ml_replace(curwin->w_cursor.lnum, newp, FALSE);
	    എങ്കിൽ (after_p != ശൂന്യ)
	    {
		ml_append(curwin->w_cursor.lnum++, after_p, 0, FALSE);
		appended_വരികൾ_mark(curwin->w_cursor.lnum, 1L);
		oap->end.lnum++;
		vim_free(after_p);
	    }
	}
    }
    അല്ലെങ്കിൽ
    {
	/*
	 * MCHAR and MLINE motion replace.
	 */
	എങ്കിൽ (oap->motion_type == MLINE)
	{
	    oap->start.col = 0;
	    curwin->w_cursor.col = 0;
	    oap->end.col = ml_get_len(oap->end.lnum);
	    എങ്കിൽ (oap->end.col)
		--oap->end.col;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (!oap->inclusive)
	    dec(&(oap->end));

	ആകുമ്പോൾ (LTOREQ_POS(curwin->w_cursor, oap->end))
	{
	    പൂർണ്ണസംഖ്യ done = FALSE;

	    n = gchar_cursor();
	    എങ്കിൽ (n != NUL)
	    {
		പൂർണ്ണസംഖ്യ new_byte_len = (*mb_char2len)(c);
		പൂർണ്ണസംഖ്യ old_byte_len = mb_ptr2len(ml_get_cursor());

		എങ്കിൽ (new_byte_len > 1 || old_byte_len > 1)
		{
		    // This is slow, but it handles replacing a single-byte
		    // with a multi-byte and the other way around.
		    എങ്കിൽ (curwin->w_cursor.lnum == oap->end.lnum)
			oap->end.col += new_byte_len - old_byte_len;
		    replace_character(c);
		    done = TRUE;
		}
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (n == TAB)
		    {
			പൂർണ്ണസംഖ്യ end_vcol = 0;

			എങ്കിൽ (curwin->w_cursor.lnum == oap->end.lnum)
			{
			    // oap->end has to be recalculated when
			    // the tab breaks
			    end_vcol = getviscol2(oap->end.col,
							     oap->end.coladd);
			}
			coladvance_force(getviscol());
			എങ്കിൽ (curwin->w_cursor.lnum == oap->end.lnum)
			    getvpos(&oap->end, end_vcol);
		    }
		    // with "coladd" set may move to just after a TAB
		    എങ്കിൽ (gchar_cursor() != NUL)
		    {
			PBYTE(curwin->w_cursor, c);
			done = TRUE;
		    }
		}
	    }
	    എങ്കിൽ (!done && virtual_op && curwin->w_cursor.lnum == oap->end.lnum)
	    {
		പൂർണ്ണസംഖ്യ virtcols = oap->end.coladd;

		എങ്കിൽ (curwin->w_cursor.lnum == oap->start.lnum
			&& oap->start.col == oap->end.col && oap->start.coladd)
		    virtcols -= oap->start.coladd;

		// oap->end has been trimmed so it's effectively inclusive;
		// as a result an extra +1 must be counted so we don't
		// trample the NUL byte.
		coladvance_force(getviscol2(oap->end.col, oap->end.coladd) + 1);
		curwin->w_cursor.col -= (virtcols + 1);
		വേണ്ടി (; virtcols >= 0; virtcols--)
		{
		    എങ്കിൽ ((*mb_char2len)(c) > 1)
		       replace_character(c);
		    അല്ലെങ്കിൽ
			PBYTE(curwin->w_cursor, c);
		    എങ്കിൽ (inc(&curwin->w_cursor) == -1)
			നിർത്തുക;
		}
	    }

	    // Advance to next character, stop at the end of the file.
	    എങ്കിൽ (inc_cursor() == -1)
		നിർത്തുക;
	}
    }

    curwin->w_cursor = oap->start;
    check_cursor();
    changed_വരികൾ(oap->start.lnum, oap->start.col, oap->end.lnum + 1, 0L);

    എങ്കിൽ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
    }

    തിരികെ OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ swapchars(പൂർണ്ണസംഖ്യ op_type, pos_T *pos, പൂർണ്ണസംഖ്യ length);

/*
 * Handle the (non-standard vi) tilde operator.  Also for "gu", "gU" and "g?".
 */
    സ്ഥിതം ശൂന്യം
op_tilde(oparg_T *oap)
{
    pos_T		pos;
    ഘടന block_def	bd;
    പൂർണ്ണസംഖ്യ			did_change = FALSE;

    എങ്കിൽ (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	തിരികെ;

    pos = oap->start;
    എങ്കിൽ (oap->block_mode)		    // Visual block mode
    {
	വേണ്ടി (; pos.lnum <= oap->end.lnum; ++pos.lnum)
	{
	    പൂർണ്ണസംഖ്യ one_change;

	    block_prep(oap, &bd, pos.lnum, FALSE);
	    pos.col = bd.textcol;
	    one_change = swapchars(oap->op_type, &pos, bd.textlen);
	    did_change |= one_change;

#ifdef FEAT_NETBEANS_INTG
	    എങ്കിൽ (netbeans_active() && one_change)
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
	എങ്കിൽ (did_change)
	    changed_വരികൾ(oap->start.lnum, 0, oap->end.lnum + 1, 0L);
    }
    അല്ലെങ്കിൽ				    // not block mode
    {
	എങ്കിൽ (oap->motion_type == MLINE)
	{
	    oap->start.col = 0;
	    pos.col = 0;
	    oap->end.col = ml_get_len(oap->end.lnum);
	    എങ്കിൽ (oap->end.col)
		--oap->end.col;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (!oap->inclusive)
	    dec(&(oap->end));

	എങ്കിൽ (pos.lnum == oap->end.lnum)
	    did_change = swapchars(oap->op_type, &pos,
						  oap->end.col - pos.col + 1);
	അല്ലെങ്കിൽ
	    വേണ്ടി (;;)
	    {
		did_change |= swapchars(oap->op_type, &pos,
			pos.lnum == oap->end.lnum ? oap->end.col + 1
						  : ml_get_pos_len(&pos));
		എങ്കിൽ (LTOREQ_POS(oap->end, pos) || inc(&pos) == -1)
		    നിർത്തുക;
	    }
	എങ്കിൽ (did_change)
	{
	    changed_വരികൾ(oap->start.lnum, oap->start.col, oap->end.lnum + 1,
									  0L);
#ifdef FEAT_NETBEANS_INTG
	    എങ്കിൽ (netbeans_active())
	    {
		char_u *ptr;
		പൂർണ്ണസംഖ്യ എണ്ണം;

		pos = oap->start;
		ആകുമ്പോൾ (pos.lnum < oap->end.lnum)
		{
		    ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		    എണ്ണം = ml_get_buf_len(curbuf, pos.lnum) - pos.col;
		    netbeans_removed(curbuf, pos.lnum, pos.col, (long)എണ്ണം);
		    // get the line again, it may have been flushed
		    ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		    netbeans_inserted(curbuf, pos.lnum, pos.col,
							&ptr[pos.col], എണ്ണം);
		    pos.col = 0;
		    pos.lnum++;
		}
		എണ്ണം = oap->end.col - pos.col + 1;
		netbeans_removed(curbuf, pos.lnum, pos.col, (long)എണ്ണം);
		// get the line again, it may have been flushed
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, pos.col,
							&ptr[pos.col], എണ്ണം);
	    }
#endif
	}
    }

    എങ്കിൽ (!did_change && oap->is_VIsual)
	// No change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    എങ്കിൽ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set '[ and '] marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
    }

    എങ്കിൽ (oap->വരി_എണ്ണം > p_report)
	smsg(NGETTEXT("%ld line changed", "%ld lines changed",
					    oap->വരി_എണ്ണം), oap->വരി_എണ്ണം);
}

/*
 * Invoke swapchar() on "length" bytes at position "pos".
 * "pos" is advanced to just after the changed characters.
 * "length" is rounded up to include the whole last multi-byte character.
 * Also works correctly when the number of bytes changes.
 * Returns TRUE if some character was changed.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
swapchars(പൂർണ്ണസംഖ്യ op_type, pos_T *pos, പൂർണ്ണസംഖ്യ length)
{
    പൂർണ്ണസംഖ്യ todo;
    പൂർണ്ണസംഖ്യ	did_change = 0;

    വേണ്ടി (todo = length; todo > 0; --todo)
    {
	എങ്കിൽ (has_mbyte)
	{
	    പൂർണ്ണസംഖ്യ len = (*mb_ptr2len)(ml_get_pos(pos));

	    // we're counting bytes, not characters
	    എങ്കിൽ (len > 0)
		todo -= len - 1;
	}
	did_change |= swapchar(op_type, pos);
	എങ്കിൽ (inc(pos) == -1)    // at end of file
	    നിർത്തുക;
    }
    തിരികെ did_change;
}

/*
 * If op_type == OP_UPPER: make uppercase,
 * if op_type == OP_LOWER: make lowercase,
 * if op_type == OP_ROT13: do rot13 encoding,
 * else swap case of character at 'pos'
 * returns TRUE when something actually changed.
 */
    പൂർണ്ണസംഖ്യ
swapchar(പൂർണ്ണസംഖ്യ op_type, pos_T *pos)
{
    പൂർണ്ണസംഖ്യ	    c;
    പൂർണ്ണസംഖ്യ	    nc;

    c = gchar_pos(pos);

    // Only do rot13 encoding for ASCII characters.
    എങ്കിൽ (c >= 0x80 && op_type == OP_ROT13)
	തിരികെ FALSE;

    // ~ is OP_NOP, g~ is OP_TILDE, gU is OP_UPPER
    എങ്കിൽ ((op_type == OP_UPPER || op_type == OP_NOP || op_type == OP_TILDE)
	    && c == 0xdf
	    && (enc_latin1like || STRCMP(p_enc, "iso-8859-2") == 0))
    {
	pos_T   sp = curwin->w_cursor;

	// Special handling for lowercase German sharp s (ß): convert to uppercase (ẞ).
	curwin->w_cursor = *pos;
	del_char(FALSE);
	ins_char(0x1E9E);
	curwin->w_cursor = sp;
	തിരികെ TRUE;
    }

    എങ്കിൽ (enc_dbcs != 0 && c >= 0x100)	// No lower/uppercase letter
	തിരികെ FALSE;
    nc = c;
    എങ്കിൽ (MB_ISLOWER(c))
    {
	എങ്കിൽ (op_type == OP_ROT13)
	    nc = ROT13(c, 'a');
	അല്ലെങ്കിൽ എങ്കിൽ (op_type != OP_LOWER)
	    nc = MB_TOUPPER(c);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (MB_ISUPPER(c))
    {
	എങ്കിൽ (op_type == OP_ROT13)
	    nc = ROT13(c, 'A');
	അല്ലെങ്കിൽ എങ്കിൽ (op_type != OP_UPPER)
	    nc = MB_TOLOWER(c);
    }
    എങ്കിൽ (nc != c)
    {
	എങ്കിൽ (enc_utf8 && (c >= 0x80 || nc >= 0x80))
	{
	    pos_T   sp = curwin->w_cursor;

	    curwin->w_cursor = *pos;
	    // don't use del_char(), it also removes composing chars
	    del_bytes(utf_ptr2len(ml_get_cursor()), FALSE, FALSE);
	    ins_char(nc);
	    curwin->w_cursor = sp;
	}
	അല്ലെങ്കിൽ
	    PBYTE(*pos, nc);
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

/*
 * op_insert - Insert and append operators for Visual mode.
 */
    ശൂന്യം
op_തിരുകുക(oparg_T *oap, long count1)
{
    long		pre_textlen = 0;
    colnr_T		ind_pre_col = 0, ind_post_col;
    പൂർണ്ണസംഖ്യ			ind_pre_vcol = 0, ind_post_vcol = 0;
    ഘടന block_def	bd;
    പൂർണ്ണസംഖ്യ			i;
    pos_T		t1;
    pos_T		start_തിരുകുക;

    // edit() changes this - record it for OP_APPEND
    bd.is_MAX = (curwin->w_curswant == MAXCOL);

    // vis block is still marked. Get rid of it now.
    curwin->w_cursor.lnum = oap->start.lnum;
    update_screen(UPD_INVERTED);

    എങ്കിൽ (oap->block_mode)
    {
	// When 'virtualedit' is used, need to insert the extra spaces before
	// doing block_prep().  When only "block" is used, virtual edit is
	// already disabled, but still need it when calling
	// coladvance_force().
	// coladvance_force() uses get_ve_flags() to get the 'virtualedit'
	// state for the current window.  To override that state, we need to
	// set the window-local value of ve_flags rather than the global value.
	എങ്കിൽ (curwin->w_cursor.coladd > 0)
	{
	    പൂർണ്ണസംഖ്യ		old_ve_flags = curwin->w_ve_flags;

	    എങ്കിൽ (u_save_cursor() == FAIL)
		തിരികെ;

	    curwin->w_ve_flags = VE_ALL;
	    coladvance_force(oap->op_type == OP_APPEND
					   ? oap->end_vcol + 1 : getviscol());
	    എങ്കിൽ (oap->op_type == OP_APPEND)
		--curwin->w_cursor.col;
	    curwin->w_ve_flags = old_ve_flags;
	}
	// Get the info about the block before entering the text
	block_prep(oap, &bd, oap->start.lnum, TRUE);
	// Get indent information
	ind_pre_col = (colnr_T)getwhitecols_curline();
	ind_pre_vcol = get_indent();
	pre_textlen = ml_get_len(oap->start.lnum) - bd.textcol;
	എങ്കിൽ (oap->op_type == OP_APPEND)
	    pre_textlen -= bd.textlen;
    }

    എങ്കിൽ (oap->op_type == OP_APPEND)
    {
	എങ്കിൽ (oap->block_mode && curwin->w_cursor.coladd == 0)
	{
	    // Move the cursor to the character right of the block.
	    curwin->w_set_curswant = true;
	    ആകുമ്പോൾ (*ml_get_cursor() != NUL
		    && (curwin->w_cursor.col < bd.textcol + bd.textlen))
		++curwin->w_cursor.col;
	    എങ്കിൽ (bd.is_short && !bd.is_MAX)
	    {
		// First line was too short, make it longer and adjust the
		// values in "bd".
		എങ്കിൽ (u_save_cursor() == FAIL)
		    തിരികെ;
		വേണ്ടി (i = 0; i < bd.endspaces; ++i)
		    ins_char(' ');
		bd.textlen += bd.endspaces;
	    }
	}
	അല്ലെങ്കിൽ
	{
	    curwin->w_cursor = oap->end;
	    check_cursor_col();

	    // Works just like an 'i'nsert on the next character.
	    എങ്കിൽ (!LINEEMPTY(curwin->w_cursor.lnum)
		    && oap->start_vcol != oap->end_vcol)
		inc_cursor();
	}
    }

    t1 = oap->start;
    start_തിരുകുക = curwin->w_cursor;
    (ശൂന്യം)edit(NUL, FALSE, (linenr_T)count1);

    // When a tab was inserted, and the characters in front of the tab
    // have been converted to a tab as well, the column of the cursor
    // might have actually been reduced, so need to adjust here.
    എങ്കിൽ (t1.lnum == curbuf->b_op_start_orig.lnum
	    && LT_POS(curbuf->b_op_start_orig, t1))
	oap->start = curbuf->b_op_start_orig;

    // If user has moved off this line, we don't know what to do, so do
    // nothing.
    // Also don't repeat the insert when Insert mode ended with CTRL-C.
    എങ്കിൽ (curwin->w_cursor.lnum != oap->start.lnum || got_int)
	തിരികെ;

    എങ്കിൽ (oap->block_mode)
    {
	പൂർണ്ണസംഖ്യ			ins_len;
	char_u			*firstline, *ins_text;
	ഘടന block_def	bd2;
	പൂർണ്ണസംഖ്യ			did_indent = FALSE;
	size_t			len;
	size_t			add;
	// offset when cursor was moved in insert mode
	പൂർണ്ണസംഖ്യ			offset = 0;

	// If indent kicked in, the firstline might have changed
	// but only do that, if the indent actually increased.
	ind_post_col = (colnr_T)getwhitecols_curline();
	എങ്കിൽ (curbuf->b_op_start.col > ind_pre_col && ind_post_col > ind_pre_col)
	{
	    bd.textcol += ind_post_col - ind_pre_col;
	    ind_post_vcol = get_indent();
	    bd.start_vcol += ind_post_vcol - ind_pre_vcol;
	    did_indent = TRUE;
	}

	// The user may have moved the cursor before inserting something, try
	// to adjust the block for that.  But only do it, if the difference
	// does not come from indent kicking in.
	എങ്കിൽ (oap->start.lnum == curbuf->b_op_start_orig.lnum
						  && !bd.is_MAX && !did_indent)
	{
	    പൂർണ്ണസംഖ്യ t = getviscol2(curbuf->b_op_start_orig.col,
					       curbuf->b_op_start_orig.coladd);

	    എങ്കിൽ (oap->op_type == OP_തിരുകുക
		    && oap->start.col + oap->start.coladd
			    != curbuf->b_op_start_orig.col
					      + curbuf->b_op_start_orig.coladd)
	    {
		oap->start.col = curbuf->b_op_start_orig.col;
		pre_textlen -= t - oap->start_vcol;
		oap->start_vcol = t;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (oap->op_type == OP_APPEND
		    && oap->start.col + oap->start.coladd
			    >= curbuf->b_op_start_orig.col
					      + curbuf->b_op_start_orig.coladd)
	    {
		oap->start.col = curbuf->b_op_start_orig.col;
		// reset pre_textlen to the value of OP_INSERT
		pre_textlen += bd.textlen;
		pre_textlen -= t - oap->start_vcol;
		oap->start_vcol = t;
		oap->op_type = OP_തിരുകുക;
	    }
	}

	// Spaces and tabs in the indent may have changed to other spaces and
	// tabs.  Get the starting column again and correct the length.
	// Don't do this when "$" used, end-of-line will have changed.
	//
	// if indent was added and the inserted text was after the indent,
	// correct the selection for the new indent.
	എങ്കിൽ (did_indent && bd.textcol - ind_post_col > 0)
	{
	    oap->start.col += ind_post_col - ind_pre_col;
	    oap->start_vcol += ind_post_vcol - ind_pre_vcol;
	    oap->end.col += ind_post_col - ind_pre_col;
	    oap->end_vcol += ind_post_vcol - ind_pre_vcol;
	}
	block_prep(oap, &bd2, oap->start.lnum, TRUE);
	എങ്കിൽ (did_indent && bd.textcol - ind_post_col > 0)
	{
	    // undo for where "oap" is used below
	    oap->start.col -= ind_post_col - ind_pre_col;
	    oap->start_vcol -= ind_post_vcol - ind_pre_vcol;
	    oap->end.col -= ind_post_col - ind_pre_col;
	    oap->end_vcol -= ind_post_vcol - ind_pre_vcol;
	}
	എങ്കിൽ (!bd.is_MAX || bd2.textlen < bd.textlen)
	{
	    എങ്കിൽ (oap->op_type == OP_APPEND)
	    {
		pre_textlen += bd2.textlen - bd.textlen;
		എങ്കിൽ (bd2.endspaces)
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
	എങ്കിൽ (oap->op_type == OP_APPEND)
	{
	    add += bd.textlen;
	    // account for pressing cursor in insert mode when '$' was used
	    എങ്കിൽ (bd.is_MAX
		&& (start_തിരുകുക.lnum == Insstart.lnum
					   && start_തിരുകുക.col > Insstart.col))
	    {
		offset = (start_തിരുകുക.col - Insstart.col);
		add -= offset;
		എങ്കിൽ (oap->end_vcol > offset)
		    oap->end_vcol -= (offset + 1);
		അല്ലെങ്കിൽ
		    // moved outside of the visual block, what to do?
		    തിരികെ;
	    }
	}
	എങ്കിൽ (add > len)
	    add = len;  // short line, point to the NUL
	firstline += add;
	len -= add;
	എങ്കിൽ (pre_textlen >= 0 && (ins_len = (പൂർണ്ണസംഖ്യ)len - pre_textlen - offset) > 0)
	{
	    ins_text = vim_strnsave(firstline, ins_len);
	    എങ്കിൽ (ins_text != ശൂന്യ)
	    {
		// block handled here
		എങ്കിൽ (u_save(oap->start.lnum,
					 (linenr_T)(oap->end.lnum + 1)) == OK)
		    block_തിരുകുക(oap, ins_text, ins_len, (oap->op_type == OP_തിരുകുക),
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
    പൂർണ്ണസംഖ്യ
op_change(oparg_T *oap)
{
    colnr_T		l;
    പൂർണ്ണസംഖ്യ			retval;
    linenr_T		linenr;
    long		pre_textlen = 0;
    long		pre_indent = 0;
    char_u		*firstline;
    char_u		*ins_text, *newp, *oldp;
    ഘടന block_def	bd;

    l = oap->start.col;
    എങ്കിൽ (oap->motion_type == MLINE)
    {
	l = 0;
	can_si = may_do_si();	// Like opening a new line, do smart indent
    }

    // First delete the text in the region.  In an empty buffer only need to
    // save for undo
    എങ്കിൽ (curbuf->b_ml.ml_flags & ML_EMPTY)
    {
	എങ്കിൽ (u_save_cursor() == FAIL)
	    തിരികെ FALSE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (op_delete(oap) == FAIL)
	തിരികെ FALSE;

    എങ്കിൽ ((l > curwin->w_cursor.col) && !LINEEMPTY(curwin->w_cursor.lnum)
							 && !virtual_op)
	inc_cursor();

    // check for still on same line (<CR> in inserted text meaningless)
    // skip blank lines too
    എങ്കിൽ (oap->block_mode)
    {
	// Add spaces before getting the current line length.
	എങ്കിൽ (virtual_op && (curwin->w_cursor.coladd > 0
						    || gchar_cursor() == NUL))
	    coladvance_force(getviscol());
	firstline = ml_get(oap->start.lnum);
	pre_textlen = ml_get_len(oap->start.lnum);
	pre_indent = (long)getwhitecols(firstline);
	bd.textcol = curwin->w_cursor.col;
    }

    എങ്കിൽ (oap->motion_type == MLINE)
	fix_indent();

    // Reset finish_op now, don't want it set inside edit().
    പൂർണ്ണസംഖ്യ save_finish_op = finish_op;
    finish_op = FALSE;

    retval = edit(NUL, FALSE, (linenr_T)1);

    finish_op = save_finish_op;

    /*
     * In Visual block mode, handle copying the new text to all lines of the
     * block.
     * Don't repeat the insert when Insert mode ended with CTRL-C.
     */
    എങ്കിൽ (oap->block_mode && oap->start.lnum != oap->end.lnum && !got_int)
    {
	പൂർണ്ണസംഖ്യ	ins_len;

	// Auto-indenting may have changed the indent.  If the cursor was past
	// the indent, exclude that indent change from the inserted text.
	firstline = ml_get(oap->start.lnum);
	എങ്കിൽ (bd.textcol > (colnr_T)pre_indent)
	{
	    long new_indent = (long)getwhitecols(firstline);

	    pre_textlen += new_indent - pre_indent;
	    bd.textcol += new_indent - pre_indent;
	}

	ins_len = (പൂർണ്ണസംഖ്യ)ml_get_len(oap->start.lnum) - pre_textlen;
	എങ്കിൽ (ins_len > 0)
	{
	    // Subsequent calls to ml_get() flush the firstline data - take a
	    // copy of the inserted text.
	    എങ്കിൽ ((ins_text = alloc(ins_len + 1)) != ശൂന്യ)
	    {
		vim_strncpy(ins_text, firstline + bd.textcol, ins_len);
		വേണ്ടി (linenr = oap->start.lnum + 1; linenr <= oap->end.lnum;
								     linenr++)
		{
		    block_prep(oap, &bd, linenr, TRUE);
		    എങ്കിൽ (!bd.is_short || virtual_op)
		    {
			pos_T vpos;
			size_t newlen;

			// If the block starts in virtual space, count the
			// initial coladd offset as part of "startspaces"
			എങ്കിൽ (bd.is_short)
			{
			    vpos.lnum = linenr;
			    (ശൂന്യം)getvpos(&vpos, oap->start_vcol);
			}
			അല്ലെങ്കിൽ
			    vpos.coladd = 0;
			oldp = ml_get(linenr);
			newp = alloc(ml_get_len(linenr) + vpos.coladd + ins_len + 1);
			എങ്കിൽ (newp == ശൂന്യ)
			    തുടരുക;
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
			എങ്കിൽ (curbuf->b_has_textprop)
			    adjust_prop_columns(linenr, bd.textcol,
						     vpos.coladd + (പൂർണ്ണസംഖ്യ)ins_len, 0);
#endif
		    }
		}
		check_cursor();

		changed_വരികൾ(oap->start.lnum + 1, 0, oap->end.lnum + 1, 0L);
	    }
	    vim_free(ins_text);
	}
    }
    auto_format(FALSE, TRUE);

    തിരികെ retval;
}

/*
 * When the cursor is on the NUL past the end of the line and it should not be
 * there move it left.
 */
    ശൂന്യം
adjust_cursor_eol(ശൂന്യം)
{
    unsigned പൂർണ്ണസംഖ്യ cur_ve_flags = get_ve_flags();

    പൂർണ്ണസംഖ്യ adj_cursor = (curwin->w_cursor.col > 0
				&& gchar_cursor() == NUL
				&& (cur_ve_flags & VE_ONEMORE) == 0
				&& (cur_ve_flags & VE_ALL) == 0
				&& !(restart_edit || (State & MODE_തിരുകുക)));
    എങ്കിൽ (!adj_cursor)
	തിരികെ;

    // Put the cursor on the last character in the line.
    dec_cursor();

    എങ്കിൽ (cur_ve_flags == VE_ALL)
    {
	colnr_T	    scol, ecol;

	// Coladd is set to the width of the last character.
	getvcol(curwin, &curwin->w_cursor, &scol, ശൂന്യ, &ecol, 0);
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
    char_u   *വരി,
    പൂർണ്ണസംഖ്യ      process,
    പൂർണ്ണസംഖ്യ	     include_space,
    പൂർണ്ണസംഖ്യ      *is_comment)
{
    char_u *comment_flags = ശൂന്യ;
    പൂർണ്ണസംഖ്യ    lead_len;
    പൂർണ്ണസംഖ്യ    leader_offset = get_last_leader_offset(വരി, &comment_flags);

    *is_comment = FALSE;
    എങ്കിൽ (leader_offset != -1)
    {
	// Let's check whether the line ends with an unclosed comment.
	// If the last comment leader has COM_END in flags, there's no comment.
	ആകുമ്പോൾ (*comment_flags)
	{
	    എങ്കിൽ (*comment_flags == COM_END
		    || *comment_flags == ':')
		നിർത്തുക;
	    ++comment_flags;
	}
	എങ്കിൽ (*comment_flags != COM_END)
	    *is_comment = TRUE;
    }

    എങ്കിൽ (process == FALSE)
	തിരികെ വരി;

    lead_len = get_leader_len(വരി, &comment_flags, FALSE, include_space);

    എങ്കിൽ (lead_len == 0)
	തിരികെ വരി;

    // Find:
    // - COM_END,
    // - colon,
    // whichever comes first.
    ആകുമ്പോൾ (*comment_flags)
    {
	എങ്കിൽ (*comment_flags == COM_END
		|| *comment_flags == ':')
	    നിർത്തുക;
	++comment_flags;
    }

    // If we found a colon, it means that we are not processing a line
    // starting with a closing part of a three-part comment. That's good,
    // because we don't want to remove those as this would be annoying.
    എങ്കിൽ (*comment_flags == ':' || *comment_flags == NUL)
	വരി += lead_len;

    തിരികെ വരി;
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
    പൂർണ്ണസംഖ്യ
do_join(
    long    എണ്ണം,
    പൂർണ്ണസംഖ്യ	    തിരുകുക_space,
    പൂർണ്ണസംഖ്യ	    save_undo,
    പൂർണ്ണസംഖ്യ	    use_formatoptions UNUSED,
    പൂർണ്ണസംഖ്യ	    setmark)
{
    char_u	*curr = ശൂന്യ;
    char_u      *curr_start = ശൂന്യ;
    char_u	*cend;
    char_u	*newp;
    size_t	newp_len;
    char_u	*spaces;	// number of spaces inserted before a line
    പൂർണ്ണസംഖ്യ		endcurr1 = NUL;
    പൂർണ്ണസംഖ്യ		endcurr2 = NUL;
    പൂർണ്ണസംഖ്യ		currsize = 0;	// size of the current line
    പൂർണ്ണസംഖ്യ		sumsize = 0;	// size of the long new line
    linenr_T	t;
    colnr_T	col = 0;
    പൂർണ്ണസംഖ്യ		ret = OK;
    പൂർണ്ണസംഖ്യ		*comments = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		remove_comments = (use_formatoptions == TRUE)
				  && has_format_option(FO_REMOVE_COMS);
    പൂർണ്ണസംഖ്യ		prev_was_comment;

    എങ്കിൽ (save_undo && u_save((linenr_T)(curwin->w_cursor.lnum - 1),
			    (linenr_T)(curwin->w_cursor.lnum + എണ്ണം)) == FAIL)
	തിരികെ FAIL;

    // Allocate an array to store the number of spaces inserted before each
    // line.  We will use it to pre-compute the length of the new line and the
    // proper placement of each original line in the new one.
    spaces = lalloc_clear(എണ്ണം, TRUE);
    എങ്കിൽ (spaces == ശൂന്യ)
	തിരികെ FAIL;
    എങ്കിൽ (remove_comments)
    {
	comments = lalloc_clear(എണ്ണം * വലുപ്പം(പൂർണ്ണസംഖ്യ), TRUE);
	എങ്കിൽ (comments == ശൂന്യ)
	{
	    vim_free(spaces);
	    തിരികെ FAIL;
	}
    }

    /*
     * Don't move anything yet, just compute the final line length
     * and setup the array of space strings lengths
     * This loops forward over the joined lines.
     */
    വേണ്ടി (t = 0; t < എണ്ണം; ++t)
    {
	curr = curr_start = ml_get((linenr_T)(curwin->w_cursor.lnum + t));
	എങ്കിൽ (t == 0 && setmark && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set the '[ mark.
	    curwin->w_buffer->b_op_start.lnum = curwin->w_cursor.lnum;
	    curwin->w_buffer->b_op_start.col  = (colnr_T)STRLEN(curr);
	}
	എങ്കിൽ (remove_comments)
	{
	    // We don't want to remove the comment leader if the
	    // previous line is not a comment.
	    എങ്കിൽ (t > 0 && prev_was_comment)
	    {

		char_u *new_curr = skip_comment(curr, TRUE, തിരുകുക_space,
							   &prev_was_comment);
		comments[t] = (പൂർണ്ണസംഖ്യ)(new_curr - curr);
		curr = new_curr;
	    }
	    അല്ലെങ്കിൽ
		curr = skip_comment(curr, FALSE, തിരുകുക_space,
							   &prev_was_comment);
	}

	എങ്കിൽ (തിരുകുക_space && t > 0)
	{
	    curr = skipwhite(curr);
	    എങ്കിൽ (*curr != NUL && *curr != ')'
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
		എങ്കിൽ (endcurr1 == ' ')
		    endcurr1 = endcurr2;
		അല്ലെങ്കിൽ
		    ++spaces[t];
		// extra space when 'joinspaces' set and line ends in '.'
		എങ്കിൽ (       p_js
			&& (endcurr1 == '.'
			    || (vim_strchr(p_cpo, CPO_JOINSP) == ശൂന്യ
				&& (endcurr1 == '?' || endcurr1 == '!'))))
		    ++spaces[t];
	    }
	}
	currsize = (പൂർണ്ണസംഖ്യ)STRLEN(curr);
	sumsize += currsize + spaces[t];
	endcurr1 = endcurr2 = NUL;
	എങ്കിൽ (തിരുകുക_space && currsize > 0)
	{
	    എങ്കിൽ (has_mbyte)
	    {
		cend = curr + currsize;
		MB_PTR_BACK(curr, cend);
		endcurr1 = (*mb_ptr2char)(cend);
		എങ്കിൽ (cend > curr)
		{
		    MB_PTR_BACK(curr, cend);
		    endcurr2 = (*mb_ptr2char)(cend);
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		endcurr1 = *(curr + currsize - 1);
		എങ്കിൽ (currsize > 1)
		    endcurr2 = *(curr + currsize - 2);
	    }
	}
	വരി_breakcheck();
	എങ്കിൽ (got_int)
	{
	    ret = FAIL;
	    പോകുക theend;
	}
    }

    // store the column position before last line
    col = sumsize - currsize - spaces[എണ്ണം - 1];

    // allocate the space for the new line
    newp_len = sumsize + 1;
    newp = alloc(newp_len);
    എങ്കിൽ (newp == ശൂന്യ)
    {
	ret = FAIL;
	പോകുക theend;
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
    പൂർണ്ണസംഖ്യ curr_off = (പൂർണ്ണസംഖ്യ)(curr - curr_start);

    curr = curr_start = ml_get((linenr_T)(curwin->w_cursor.lnum + എണ്ണം - 1));
    curr += curr_off;
#endif

    വേണ്ടി (t = എണ്ണം - 1; ; --t)
    {
	പൂർണ്ണസംഖ്യ spaces_removed;

	cend -= currsize;
	mch_memmove(cend, curr, (size_t)currsize);

	എങ്കിൽ (spaces[t] > 0)
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
		t == എണ്ണം - 1, (long)(cend - newp), spaces_removed);
#endif
	എങ്കിൽ (t == 0)
	    നിർത്തുക;
	curr = curr_start = ml_get((linenr_T)(curwin->w_cursor.lnum + t - 1));
	എങ്കിൽ (remove_comments)
	    curr += comments[t - 1];
	എങ്കിൽ (തിരുകുക_space && t > 1)
	    curr = skipwhite(curr);
	currsize = (പൂർണ്ണസംഖ്യ)STRLEN(curr);
    }

#ifdef FEAT_PROP_POPUP
    എങ്കിൽ (um.buf != ശൂന്യ)
    {
	um_set_text(&um, newp);
	um_reverse_props(&um);
	um_close(&um);
	newp = ശൂന്യ;  // um_set_text took ownership
    }
    അല്ലെങ്കിൽ
#endif
    ml_replace_len(curwin->w_cursor.lnum, newp, (colnr_T)newp_len, TRUE, FALSE);

    എങ്കിൽ (setmark && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set the '] mark.
	curwin->w_buffer->b_op_end.lnum = curwin->w_cursor.lnum;
	curwin->w_buffer->b_op_end.col  = (colnr_T)sumsize;
    }

    // Only report the change in the first line here, del_lines() will report
    // the deleted line.
    changed_വരികൾ(curwin->w_cursor.lnum, currsize,
					       curwin->w_cursor.lnum + 1, 0L);
    /*
     * Delete following lines. To do this we move the cursor there
     * briefly, and then move it back. After del_lines() the cursor may
     * have moved up (last line deleted), so the current lnum is kept in t.
     */
    t = curwin->w_cursor.lnum;
    ++curwin->w_cursor.lnum;
    del_വരികൾ(എണ്ണം - 1, FALSE);
    curwin->w_cursor.lnum = t;

    /*
     * Set the cursor column:
     * Vi compatible: use the column of the first join
     * vim:	      use the column of the last join
     */
    curwin->w_cursor.col =
		    (vim_strchr(p_cpo, CPO_JOINCOL) != ശൂന്യ ? currsize : col);
    check_cursor_col();

    curwin->w_cursor.coladd = 0;
    curwin->w_set_curswant = true;

theend:
    vim_free(spaces);
    എങ്കിൽ (remove_comments)
	vim_free(comments);
    തിരികെ ret;
}

#ifdef FEAT_LINEBREAK
/*
 * TODO: consider using "tailp" of win_lbr_chartabsize() instead of temporarily
 * resetting 'linebreak'.
 *
 * Reset 'linebreak' and take care of side effects.
 * Returns the previous value, to be passed to restore_lbr().
 */
    പൂർണ്ണസംഖ്യ
reset_lbr(ശൂന്യം)
{
    എങ്കിൽ (!curwin->w_p_lbr)
	തിരികെ FALSE;
    // changing 'linebreak' may require w_virtcol to be updated
    curwin->w_p_lbr = FALSE;
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);
    തിരികെ TRUE;
}

/*
 * Restore 'linebreak' and take care of side effects.
 */
    ശൂന്യം
restore_lbr(പൂർണ്ണസംഖ്യ lbr_saved)
{
    എങ്കിൽ (curwin->w_p_lbr || !lbr_saved)
	തിരികെ;

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
    ശൂന്യം
block_prep(
    oparg_T		*oap,
    ഘടന block_def	*bdp,
    linenr_T		lnum,
    പൂർണ്ണസംഖ്യ			is_del)
{
    പൂർണ്ണസംഖ്യ		incr = 0;
    char_u	*pend;
    char_u	*pstart;
    char_u	*വരി;
    char_u	*prev_pstart;
    char_u	*prev_pend;
    chartabsize_T cts;
#ifdef FEAT_LINEBREAK
		// Avoid a problem with unwanted linebreaks in block mode.
    പൂർണ്ണസംഖ്യ		lbr_saved = reset_lbr();
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

    വരി = ml_get(lnum);
    prev_pstart = വരി;
    init_chartabsize_arg(&cts, curwin, lnum, bdp->start_vcol, വരി, വരി);
    ആകുമ്പോൾ (cts.cts_vcol < oap->start_vcol && *cts.cts_ptr != NUL)
    {
	// Count a tab for what it's worth (if list mode not on)
	incr = lbr_chartabsize(&cts);
	cts.cts_vcol += incr;
	എങ്കിൽ (VIM_ISWHITE(*cts.cts_ptr))
	{
	    bdp->pre_whitesp += incr;
	    bdp->pre_whitesp_c++;
	}
	അല്ലെങ്കിൽ
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
    എങ്കിൽ (bdp->start_vcol < oap->start_vcol)	// line too short
    {
	bdp->end_vcol = bdp->start_vcol;
	bdp->is_short = TRUE;
	എങ്കിൽ (!is_del || oap->op_type == OP_APPEND)
	    bdp->endspaces = oap->end_vcol - oap->start_vcol + 1;
    }
    അല്ലെങ്കിൽ
    {
	// notice: this converts partly selected Multibyte characters to
	// spaces, too.
	bdp->startspaces = bdp->start_vcol - oap->start_vcol;
	എങ്കിൽ (is_del && bdp->startspaces)
	    bdp->startspaces = bdp->start_char_vcols - bdp->startspaces;
	pend = pstart;
	bdp->end_vcol = bdp->start_vcol;
	എങ്കിൽ (bdp->end_vcol > oap->end_vcol)	// it's all in one character
	{
	    bdp->is_oneChar = TRUE;
	    എങ്കിൽ (oap->op_type == OP_തിരുകുക)
		bdp->endspaces = bdp->start_char_vcols - bdp->startspaces;
	    അല്ലെങ്കിൽ എങ്കിൽ (oap->op_type == OP_APPEND)
	    {
		bdp->startspaces += oap->end_vcol - oap->start_vcol + 1;
		bdp->endspaces = bdp->start_char_vcols - bdp->startspaces;
	    }
	    അല്ലെങ്കിൽ
	    {
		bdp->startspaces = oap->end_vcol - oap->start_vcol + 1;
		എങ്കിൽ (is_del && oap->op_type != OP_LSHIFT)
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
	അല്ലെങ്കിൽ
	{
	    init_chartabsize_arg(&cts, curwin, lnum, bdp->end_vcol,
								  വരി, pend);
	    prev_pend = pend;
	    ആകുമ്പോൾ (cts.cts_vcol <= oap->end_vcol && *cts.cts_ptr != NUL)
	    {
		// count a tab for what it's worth (if list mode not on)
		prev_pend = cts.cts_ptr;
		incr = lbr_chartabsize_adv(&cts);
		cts.cts_vcol += incr;
	    }
	    bdp->end_vcol = cts.cts_vcol;
	    pend = cts.cts_ptr;
	    clear_chartabsize_arg(&cts);

	    എങ്കിൽ (bdp->end_vcol <= oap->end_vcol
		    && (!is_del
			|| oap->op_type == OP_APPEND
			|| oap->op_type == OP_REPLACE)) // line too short
	    {
		bdp->is_short = TRUE;
		// Alternative: include spaces to fill up the block.
		// Disadvantage: can lead to trailing spaces when the line is
		// short where the text is put
		// if (!is_del || oap->op_type == OP_APPEND)
		എങ്കിൽ (oap->op_type == OP_APPEND || virtual_op)
		    bdp->endspaces = oap->end_vcol - bdp->end_vcol
							     + oap->inclusive;
		അല്ലെങ്കിൽ
		    bdp->endspaces = 0; // replace doesn't add characters
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (bdp->end_vcol > oap->end_vcol)
	    {
		bdp->endspaces = bdp->end_vcol - oap->end_vcol - 1;
		എങ്കിൽ (!is_del && bdp->endspaces)
		{
		    bdp->endspaces = incr - bdp->endspaces;
		    എങ്കിൽ (pend != pstart)
			pend = prev_pend;
		}
	    }
	}
	bdp->end_char_vcols = incr;
	എങ്കിൽ (is_del && bdp->startspaces)
	    pstart = prev_pstart;
	bdp->textlen = (പൂർണ്ണസംഖ്യ)(pend - pstart);
    }
    bdp->textcol = (colnr_T) (pstart - വരി);
    bdp->textstart = pstart;
#ifdef FEAT_LINEBREAK
    restore_lbr(lbr_saved);
#endif
}

/*
 * Get block text from "start" to "end"
 */
    ശൂന്യം
charwise_block_prep(
    pos_T		start,
    pos_T		end,
    ഘടന block_def	*bdp,
    linenr_T		lnum,
    പൂർണ്ണസംഖ്യ			inclusive)
{
    colnr_T startcol = 0, endcol = MAXCOL;
    colnr_T cs, ce;
    char_u *p;
    പൂർണ്ണസംഖ്യ	plen = ml_get_len(lnum);

    p = ml_get(lnum);
    bdp->startspaces = 0;
    bdp->endspaces = 0;
    bdp->is_oneChar = FALSE;
    bdp->start_char_vcols = 0;

    എങ്കിൽ (lnum == start.lnum)
    {
	startcol = start.col;
	എങ്കിൽ (virtual_op)
	{
	    getvcol(curwin, &start, &cs, ശൂന്യ, &ce, 0);
	    എങ്കിൽ (ce != cs && start.coladd > 0)
	    {
		// Part of a tab selected -- but don't
		// double-count it.
		bdp->start_char_vcols = ce - cs + 1;
		bdp->startspaces = bdp->start_char_vcols - start.coladd;
		എങ്കിൽ (bdp->startspaces < 0)
		    bdp->startspaces = 0;
		startcol++;
	    }
	}
    }

    എങ്കിൽ (lnum == end.lnum)
    {
	endcol = end.col;
	എങ്കിൽ (virtual_op)
	{
	    getvcol(curwin, &end, &cs, ശൂന്യ, &ce, 0);
	    എങ്കിൽ (p[endcol] == NUL || (cs + end.coladd < ce
			// Don't add space for double-wide
			// char; endcol will be on last byte
			// of multi-byte char.
			&& (*mb_head_off)(p, p + endcol) == 0))
	    {
		എങ്കിൽ (start.lnum == end.lnum && start.col == end.col)
		{
		    // Special case: inside a single char
		    bdp->is_oneChar = TRUE;
		    bdp->startspaces = end.coladd - start.coladd + inclusive;
		    endcol = startcol;
		}
		അല്ലെങ്കിൽ
		{
		    bdp->endspaces = end.coladd + inclusive;
		    endcol -= inclusive;
		}
	    }
	}
    }
    എങ്കിൽ (endcol == MAXCOL)
	endcol = ml_get_len(lnum);
    എങ്കിൽ (startcol > endcol || bdp->is_oneChar)
	bdp->textlen = 0;
    അല്ലെങ്കിൽ
	bdp->textlen = endcol - startcol + inclusive;
    bdp->textcol = startcol;
    bdp->textstart = startcol <= plen ? p + startcol : p;
}

/*
 * Handle the add/subtract operator.
 */
    ശൂന്യം
op_addsub(
    oparg_T	*oap,
    linenr_T	Prenum1,	    // Amount of add/subtract
    പൂർണ്ണസംഖ്യ		g_cmd)		    // was g<c-a>/g<c-x>
{
    pos_T		pos;
    ഘടന block_def	bd;
    പൂർണ്ണസംഖ്യ			change_cnt = 0;
    linenr_T		amount = Prenum1;

    // do_addsub() might trigger re-evaluation of 'foldexpr' halfway, when the
    // buffer is not completely updated yet. Postpone updating folds until before
    // the call to changed_lines().
#ifdef FEAT_FOLDING
    disable_fold_update++;
#endif

    എങ്കിൽ (!VIsual_active)
    {
	pos = curwin->w_cursor;
	എങ്കിൽ (u_save_cursor() == FAIL)
	{
#ifdef FEAT_FOLDING
	    disable_fold_update--;
#endif
	    തിരികെ;
	}
	change_cnt = do_addsub(oap->op_type, &pos, 0, amount);
#ifdef FEAT_FOLDING
	disable_fold_update--;
#endif
	എങ്കിൽ (change_cnt)
	    changed_വരികൾ(pos.lnum, 0, pos.lnum + 1, 0L);
    }
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ	one_change;
	പൂർണ്ണസംഖ്യ	length;
	pos_T	startpos;

	എങ്കിൽ (u_save((linenr_T)(oap->start.lnum - 1),
					(linenr_T)(oap->end.lnum + 1)) == FAIL)
	{
#ifdef FEAT_FOLDING
	    disable_fold_update--;
#endif
	    തിരികെ;
	}

	pos = oap->start;
	വേണ്ടി (; pos.lnum <= oap->end.lnum; ++pos.lnum)
	{
	    എങ്കിൽ (oap->block_mode)		    // Visual block mode
	    {
		block_prep(oap, &bd, pos.lnum, FALSE);
		pos.col = bd.textcol;
		length = bd.textlen;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (oap->motion_type == MLINE)
	    {
		curwin->w_cursor.col = 0;
		pos.col = 0;
		length = ml_get_len(pos.lnum);
	    }
	    അല്ലെങ്കിൽ // oap->motion_type == MCHAR
	    {
		എങ്കിൽ (pos.lnum == oap->start.lnum && !oap->inclusive)
		    dec(&(oap->end));
		length = ml_get_len(pos.lnum);
		pos.col = 0;
		എങ്കിൽ (pos.lnum == oap->start.lnum)
		{
		    pos.col += oap->start.col;
		    length -= oap->start.col;
		}
		എങ്കിൽ (pos.lnum == oap->end.lnum)
		{
		    length = ml_get_len(oap->end.lnum);
		    എങ്കിൽ (oap->end.col >= length)
			oap->end.col = length - 1;
		    length = oap->end.col - pos.col + 1;
		}
	    }
	    one_change = do_addsub(oap->op_type, &pos, length, amount);
	    എങ്കിൽ (one_change)
	    {
		// Remember the start position of the first change.
		എങ്കിൽ (change_cnt == 0)
		    startpos = curbuf->b_op_start;
		++change_cnt;
	    }

#ifdef FEAT_NETBEANS_INTG
	    എങ്കിൽ (netbeans_active() && one_change)
	    {
		char_u *ptr;

		netbeans_removed(curbuf, pos.lnum, pos.col, (long)length);
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, pos.col,
						&ptr[pos.col], length);
	    }
#endif
	    എങ്കിൽ (g_cmd && one_change)
		amount += Prenum1;
	}

#ifdef FEAT_FOLDING
	disable_fold_update--;
#endif
	എങ്കിൽ (change_cnt)
	    changed_വരികൾ(oap->start.lnum, 0, oap->end.lnum + 1, 0L);

	എങ്കിൽ (!change_cnt && oap->is_VIsual)
	    // No change: need to remove the Visual selection
	    redraw_curbuf_later(UPD_INVERTED);

	// Set '[ mark if something changed. Keep the last end
	// position from do_addsub().
	എങ്കിൽ (change_cnt > 0 && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	    curbuf->b_op_start = startpos;

	എങ്കിൽ (change_cnt > p_report)
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
do_addsub(
    പൂർണ്ണസംഖ്യ		op_type,
    pos_T	*pos,
    പൂർണ്ണസംഖ്യ		length,
    linenr_T	Prenum1)
{
    പൂർണ്ണസംഖ്യ		col;
    പൂർണ്ണസംഖ്യ		pre;		// 'X'/'x': hex; '0': octal; 'B'/'b': bin
    സ്ഥിതം പൂർണ്ണസംഖ്യ	hexupper = FALSE;	// 0xABC
    uvarnumber_T	n;
    uvarnumber_T	oldn;
    char_u	*ptr;
    പൂർണ്ണസംഖ്യ		linelen;
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		todel;
    പൂർണ്ണസംഖ്യ		do_hex;
    പൂർണ്ണസംഖ്യ		do_oct;
    പൂർണ്ണസംഖ്യ		do_bin;
    പൂർണ്ണസംഖ്യ		do_alpha;
    പൂർണ്ണസംഖ്യ		do_unsigned;
    പൂർണ്ണസംഖ്യ		do_blank;
    പൂർണ്ണസംഖ്യ		blank_unsigned = FALSE;	// blank: treat as unsigned?
    പൂർണ്ണസംഖ്യ		firstdigit;
    പൂർണ്ണസംഖ്യ		subtract;
    പൂർണ്ണസംഖ്യ		negative = FALSE;
    പൂർണ്ണസംഖ്യ		was_positive = TRUE;
    പൂർണ്ണസംഖ്യ		visual = VIsual_active;
    പൂർണ്ണസംഖ്യ		did_change = FALSE;
    pos_T	save_cursor = curwin->w_cursor;
    പൂർണ്ണസംഖ്യ		maxlen = 0;
    pos_T	startpos;
    pos_T	endpos;
    colnr_T	save_coladd = 0;

    do_hex = (vim_strchr(curbuf->b_p_nf, 'x') != ശൂന്യ);	// "heX"
    do_oct = (vim_strchr(curbuf->b_p_nf, 'o') != ശൂന്യ);	// "Octal"
    do_bin = (vim_strchr(curbuf->b_p_nf, 'b') != ശൂന്യ);	// "Bin"
    do_alpha = (vim_strchr(curbuf->b_p_nf, 'p') != ശൂന്യ);	// "alPha"
    do_unsigned = (vim_strchr(curbuf->b_p_nf, 'u') != ശൂന്യ);	// "Unsigned"
    do_blank = (vim_strchr(curbuf->b_p_nf, 'k') != ശൂന്യ);	// "blanK"

    എങ്കിൽ (virtual_active())
    {
	save_coladd = pos->coladd;
	pos->coladd = 0;
    }

    curwin->w_cursor = *pos;
    ptr = ml_get(pos->lnum);
    linelen = ml_get_len(pos->lnum);
    col = pos->col;

    എങ്കിൽ (col + !!save_coladd >= linelen)
	പോകുക theend;

    /*
     * First check if we are on a hexadecimal number, after the "0x".
     */
    എങ്കിൽ (!VIsual_active)
    {
	എങ്കിൽ (do_bin)
	    ആകുമ്പോൾ (col > 0 && vim_isbdigit(ptr[col]))
	    {
		--col;
		എങ്കിൽ (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }

	എങ്കിൽ (do_hex)
	    ആകുമ്പോൾ (col > 0 && vim_isxdigit(ptr[col]))
	    {
		--col;
		എങ്കിൽ (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }

	എങ്കിൽ (       do_bin
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

	    ആകുമ്പോൾ (col > 0 && vim_isdigit(ptr[col]))
	    {
		col--;
		എങ്കിൽ (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }
	}

	എങ്കിൽ ((       do_hex
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
	    എങ്കിൽ (has_mbyte)
		col -= (*mb_head_off)(ptr, ptr + col);
	}
	അല്ലെങ്കിൽ
	{
	    /*
	     * Search forward and then backward to find the start of number.
	     */
	    col = pos->col;

	    ആകുമ്പോൾ (ptr[col] != NUL
		    && !vim_isdigit(ptr[col])
		    && !(do_alpha && ASCII_ISALPHA(ptr[col])))
		col += mb_ptr2len(ptr + col);

	    ആകുമ്പോൾ (col > 0
		    && vim_isdigit(ptr[col - 1])
		    && !(do_alpha && ASCII_ISALPHA(ptr[col])))
	    {
		--col;
		എങ്കിൽ (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }
	}
    }

    എങ്കിൽ (visual)
    {
	ആകുമ്പോൾ (ptr[col] != NUL && length > 0
		&& !vim_isdigit(ptr[col])
		&& !(do_alpha && ASCII_ISALPHA(ptr[col])))
	{
	    പൂർണ്ണസംഖ്യ mb_len = mb_ptr2len(ptr + col);

	    col += mb_len;
	    length -= mb_len;
	}

	എങ്കിൽ (length == 0)
	    പോകുക theend;

	എങ്കിൽ (col > pos->col && ptr[col - 1] == '-'
		&& (!has_mbyte || !(*mb_head_off)(ptr, ptr + col - 1))
		&& !do_unsigned)
	{
	    എങ്കിൽ (do_blank && col >= 2 && !VIM_ISWHITE(ptr[col - 2]))
		blank_unsigned = TRUE;
	    അല്ലെങ്കിൽ
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
    എങ്കിൽ (!VIM_ISDIGIT(firstdigit) && !(do_alpha && ASCII_ISALPHA(firstdigit)))
    {
	beep_flush();
	പോകുക theend;
    }

    എങ്കിൽ (do_alpha && ASCII_ISALPHA(firstdigit))
    {
	// decrement or increment alphabetic character
	എങ്കിൽ (op_type == OP_NR_SUB)
	{
	    എങ്കിൽ (CharOrd(firstdigit) < Prenum1)
	    {
		എങ്കിൽ (SAFE_isupper(firstdigit))
		    firstdigit = 'A';
		അല്ലെങ്കിൽ
		    firstdigit = 'a';
	    }
	    അല്ലെങ്കിൽ
		firstdigit -= Prenum1;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (26 - CharOrd(firstdigit) - 1 < Prenum1)
	    {
		എങ്കിൽ (SAFE_isupper(firstdigit))
		    firstdigit = 'Z';
		അല്ലെങ്കിൽ
		    firstdigit = 'z';
	    }
	    അല്ലെങ്കിൽ
		firstdigit += Prenum1;
	}
	curwin->w_cursor.col = col;
	എങ്കിൽ (!did_change)
	    startpos = curwin->w_cursor;
	did_change = TRUE;
	(ശൂന്യം)del_char(FALSE);
	ins_char(firstdigit);
	endpos = curwin->w_cursor;
	curwin->w_cursor.col = col;
    }
    അല്ലെങ്കിൽ
    {
	char_u	*buf1;
	പൂർണ്ണസംഖ്യ	buf1len;
	char_u	buf2[NUMBUFLEN];
	പൂർണ്ണസംഖ്യ	buf2len;
	pos_T	save_pos;
	പൂർണ്ണസംഖ്യ	i;

	എങ്കിൽ (col > 0 && ptr[col - 1] == '-'
		&& (!has_mbyte ||
		    !(*mb_head_off)(ptr, ptr + col - 1))
		&& !visual
		&& !do_unsigned)
	{
	    എങ്കിൽ (do_blank && col >= 2 && !VIM_ISWHITE(ptr[col - 2]))
		blank_unsigned = TRUE;
	    അല്ലെങ്കിൽ
	    {
		// negative number
		--col;
		negative = TRUE;
	    }
	}

	// get the number value (unsigned)
	എങ്കിൽ (visual && VIsual_mode != 'V')
	    maxlen = (curbuf->b_visual.vi_curswant == MAXCOL
		    ? linelen - col : length);

	പൂർണ്ണസംഖ്യ overflow = FALSE;
	vim_str2nr(ptr + col, &pre, &length,
		0 + (do_bin ? STR2NR_BIN : 0)
		    + (do_oct ? STR2NR_OCT : 0)
		    + (do_hex ? STR2NR_HEX : 0),
		ശൂന്യ, &n, maxlen, FALSE, &overflow);

	// ignore leading '-' for hex and octal and bin numbers
	എങ്കിൽ (pre && negative)
	{
	    ++col;
	    --length;
	    negative = FALSE;
	}
	// add or subtract
	subtract = FALSE;
	എങ്കിൽ (op_type == OP_NR_SUB)
	    subtract ^= TRUE;
	എങ്കിൽ (negative)
	    subtract ^= TRUE;

	oldn = n;
	എങ്കിൽ (!overflow)  // if number is too big don't add/subtract
	{
	    എങ്കിൽ (subtract)
		n -= (uvarnumber_T)Prenum1;
	    അല്ലെങ്കിൽ
		n += (uvarnumber_T)Prenum1;
	}

	// handle wraparound for decimal numbers
	എങ്കിൽ (!pre)
	{
	    എങ്കിൽ (subtract)
	    {
		എങ്കിൽ (n > oldn)
		{
		    n = 1 + (n ^ (uvarnumber_T)-1);
		    negative ^= TRUE;
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		// add
		എങ്കിൽ (n < oldn)
		{
		    n = (n ^ (uvarnumber_T)-1);
		    negative ^= TRUE;
		}
	    }
	    എങ്കിൽ (n == 0)
		negative = FALSE;
	}

	എങ്കിൽ ((do_unsigned || blank_unsigned) && negative)
	{
	    എങ്കിൽ (subtract)
		// sticking at zero.
		n = (uvarnumber_T)0;
	    അല്ലെങ്കിൽ
		// sticking at 2^64 - 1.
		n = (uvarnumber_T)(-1);
	    negative = FALSE;
	}

	എങ്കിൽ (visual && !was_positive && !negative && col > 0)
	{
	    // need to remove the '-'
	    col--;
	    length++;
	}

	/*
	 * Delete the old number.
	 */
	curwin->w_cursor.col = col;
	എങ്കിൽ (!did_change)
	    startpos = curwin->w_cursor;
	did_change = TRUE;
	todel = length;
	c = gchar_cursor();
	/*
	 * Don't include the '-' in the length, only the length of the
	 * part after it is kept the same.
	 */
	എങ്കിൽ (c == '-')
	    --length;

	save_pos = curwin->w_cursor;
	വേണ്ടി (i = 0; i < todel; ++i)
	{
	    എങ്കിൽ (c < 0x100 && SAFE_isalpha(c))
	    {
		എങ്കിൽ (SAFE_isupper(c))
		    hexupper = TRUE;
		അല്ലെങ്കിൽ
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
	എങ്കിൽ (buf1 == ശൂന്യ)
	    പോകുക theend;
	ptr = buf1;
	എങ്കിൽ (negative && (!visual || was_positive))
	    *ptr++ = '-';
	എങ്കിൽ (pre)
	{
	    *ptr++ = '0';
	    --length;
	}
	എങ്കിൽ (pre == 'b' || pre == 'B' ||
	    pre == 'x' || pre == 'X')
	{
	    *ptr++ = pre;
	    --length;
	}

	/*
	 * Put the number characters in buf2[].
	 */
	എങ്കിൽ (pre == 'b' || pre == 'B')
	{
	    പൂർണ്ണസംഖ്യ bit = 0;
	    പൂർണ്ണസംഖ്യ bits = വലുപ്പം(uvarnumber_T) * 8;

	    // leading zeros
	    വേണ്ടി (bit = bits; bit > 0; bit--)
		എങ്കിൽ ((n >> (bit - 1)) & 0x1) നിർത്തുക;

	    വേണ്ടി (buf2len = 0; bit > 0 && buf2len < (NUMBUFLEN - 1); bit--)
		buf2[buf2len++] = ((n >> (bit - 1)) & 0x1) ? '1' : '0';

	    buf2[buf2len] = NUL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (pre == 0)
	    buf2len = vim_snprintf((അക്ഷരം *)buf2, NUMBUFLEN, "%llu", n);
	അല്ലെങ്കിൽ എങ്കിൽ (pre == '0')
	    buf2len = vim_snprintf((അക്ഷരം *)buf2, NUMBUFLEN, "%llo", n);
	അല്ലെങ്കിൽ എങ്കിൽ (pre && hexupper)
	    buf2len = vim_snprintf((അക്ഷരം *)buf2, NUMBUFLEN, "%llX", n);
	അല്ലെങ്കിൽ
	    buf2len = vim_snprintf((അക്ഷരം *)buf2, NUMBUFLEN, "%llx", n);
	length -= buf2len;

	/*
	 * Adjust number of zeros to the new number of digits, so the
	 * total length of the number remains the same.
	 * Don't do this when
	 * the result may look like an octal number.
	 */
	എങ്കിൽ (firstdigit == '0' && !(do_oct && pre == 0))
	    ആകുമ്പോൾ (length-- > 0)
		*ptr++ = '0';
	*ptr = NUL;
	buf1len = (പൂർണ്ണസംഖ്യ)(ptr - buf1);

	STRCPY(buf1 + buf1len, buf2);
	buf1len += buf2len;

	// Insert just after the first character to be removed, so that any
	// text properties will be adjusted.  Then delete the old number
	// afterwards.
	save_pos = curwin->w_cursor;
	എങ്കിൽ (todel > 0)
	    inc_cursor();
	ins_str(buf1, (size_t)buf1len);		// insert the new number
	vim_free(buf1);

	// del_char() will also mark line needing displaying
	എങ്കിൽ (todel > 0)
	{
	    പൂർണ്ണസംഖ്യ bytes_after = ml_get_curline_len() - curwin->w_cursor.col;

	    // Delete the one character before the insert.
	    curwin->w_cursor = save_pos;
	    (ശൂന്യം)del_char(FALSE);
	    curwin->w_cursor.col = ml_get_curline_len() - bytes_after;
	    --todel;
	}
	ആകുമ്പോൾ (todel-- > 0)
	    (ശൂന്യം)del_char(FALSE);

	endpos = curwin->w_cursor;
	എങ്കിൽ (did_change && curwin->w_cursor.col)
	    --curwin->w_cursor.col;
    }

    എങ്കിൽ (did_change && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// set the '[ and '] marks
	curbuf->b_op_start = startpos;
	curbuf->b_op_end = endpos;
	എങ്കിൽ (curbuf->b_op_end.col > 0)
	    --curbuf->b_op_end.col;
    }

theend:
    എങ്കിൽ (visual)
	curwin->w_cursor = save_cursor;
    അല്ലെങ്കിൽ എങ്കിൽ (did_change)
	curwin->w_set_curswant = true;
    അല്ലെങ്കിൽ എങ്കിൽ (virtual_active())
	curwin->w_cursor.coladd = save_coladd;

    തിരികെ did_change;
}

    ശൂന്യം
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
    സ്ഥിതം varnumber_T
വരി_എണ്ണം_info(
    char_u	*വരി,
    varnumber_T	*wc,
    varnumber_T	*cc,
    varnumber_T	limit,
    പൂർണ്ണസംഖ്യ		eol_size)
{
    varnumber_T	i;
    varnumber_T	വാക്കുകൾ = 0;
    varnumber_T	chars = 0;
    പൂർണ്ണസംഖ്യ		is_word = 0;

    വേണ്ടി (i = 0; i < limit && വരി[i] != NUL; )
    {
	എങ്കിൽ (is_word)
	{
	    എങ്കിൽ (vim_isspace(വരി[i]))
	    {
		വാക്കുകൾ++;
		is_word = 0;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (!vim_isspace(വരി[i]))
	    is_word = 1;
	++chars;
	i += (*mb_ptr2len)(വരി + i);
    }

    എങ്കിൽ (is_word)
	വാക്കുകൾ++;
    *wc += വാക്കുകൾ;

    // Add eol_size if the end of line was reached before hitting limit.
    എങ്കിൽ (i < limit && വരി[i] == NUL)
    {
	i += eol_size;
	chars += eol_size;
    }
    *cc += chars;
    തിരികെ i;
}

/*
 * Give some info about the position of the cursor (for "g CTRL-G").
 * In Visual mode, give some info about the selected region.  (In this case,
 * the *_count_cursor variables store running totals for the selection.)
 * When "dict" is not NULL store the info there instead of showing it.
 */
    ശൂന്യം
cursor_pos_info(dict_T *dict)
{
    char_u	*p;
    char_u	buf1[50];
    char_u	buf2[40];
    linenr_T	lnum;
    varnumber_T	byte_എണ്ണം = 0;
    varnumber_T	bom_എണ്ണം  = 0;
    varnumber_T	byte_എണ്ണം_cursor = 0;
    varnumber_T	char_എണ്ണം = 0;
    varnumber_T	char_എണ്ണം_cursor = 0;
    varnumber_T	word_എണ്ണം = 0;
    varnumber_T	word_എണ്ണം_cursor = 0;
    പൂർണ്ണസംഖ്യ		eol_size;
    varnumber_T	last_check = 100000L;
    long	വരി_എണ്ണം_selected = 0;
    pos_T	min_pos, max_pos;
    oparg_T	oparg;
    ഘടന block_def	bd;

    /*
     * Compute the length of the file in characters.
     */
    എങ്കിൽ (curbuf->b_ml.ml_flags & ML_EMPTY)
    {
	എങ്കിൽ (dict == ശൂന്യ)
	{
	    സന്ദേശം(_(no_വരികൾ_സന്ദേശം));
	    തിരികെ;
	}
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (get_fileformat(curbuf) == EOL_DOS)
	    eol_size = 2;
	അല്ലെങ്കിൽ
	    eol_size = 1;

	എങ്കിൽ (VIsual_active)
	{
	    എങ്കിൽ (LT_POS(VIsual, curwin->w_cursor))
	    {
		min_pos = VIsual;
		max_pos = curwin->w_cursor;
	    }
	    അല്ലെങ്കിൽ
	    {
		min_pos = curwin->w_cursor;
		max_pos = VIsual;
	    }
	    എങ്കിൽ (*p_sel == 'e' && max_pos.col > 0)
		--max_pos.col;

	    എങ്കിൽ (VIsual_mode == Ctrl_V)
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
		എങ്കിൽ (curwin->w_curswant == MAXCOL)
		    oparg.end_vcol = MAXCOL;
		// Swap the start, end vcol if needed
		എങ്കിൽ (oparg.end_vcol < oparg.start_vcol)
		{
		    oparg.end_vcol += oparg.start_vcol;
		    oparg.start_vcol = oparg.end_vcol - oparg.start_vcol;
		    oparg.end_vcol -= oparg.start_vcol;
		}
	    }
	    വരി_എണ്ണം_selected = max_pos.lnum - min_pos.lnum + 1;
	}

	വേണ്ടി (lnum = 1; lnum <= curbuf->b_ml.ml_വരി_എണ്ണം; ++lnum)
	{
	    // Check for a CTRL-C every 100000 characters.
	    എങ്കിൽ (byte_എണ്ണം > last_check)
	    {
		ui_breakcheck();
		എങ്കിൽ (got_int)
		    തിരികെ;
		last_check = byte_എണ്ണം + 100000L;
	    }

	    // Do extra processing for VIsual mode.
	    എങ്കിൽ (VIsual_active
		    && lnum >= min_pos.lnum && lnum <= max_pos.lnum)
	    {
		char_u	    *s = ശൂന്യ;
		long	    len = 0L;

		തിരഞ്ഞെടുപ്പ് (VIsual_mode)
		{
		    അവസ്ഥ Ctrl_V:
			virtual_op = virtual_active();
			block_prep(&oparg, &bd, lnum, 0);
			virtual_op = MAYBE;
			s = bd.textstart;
			len = (long)bd.textlen;
			നിർത്തുക;
		    അവസ്ഥ 'V':
			s = ml_get(lnum);
			len = MAXCOL;
			നിർത്തുക;
		    അവസ്ഥ 'v':
			{
			    colnr_T start_col = (lnum == min_pos.lnum)
							   ? min_pos.col : 0;
			    colnr_T end_col = (lnum == max_pos.lnum)
				      ? max_pos.col - start_col + 1 : MAXCOL;

			    s = ml_get(lnum) + start_col;
			    len = end_col;
			}
			നിർത്തുക;
		}
		എങ്കിൽ (s != ശൂന്യ)
		{
		    byte_എണ്ണം_cursor += വരി_എണ്ണം_info(s, &word_എണ്ണം_cursor,
					   &char_എണ്ണം_cursor, len, eol_size);
		    എങ്കിൽ (lnum == curbuf->b_ml.ml_വരി_എണ്ണം
			    && !curbuf->b_p_eol
			    && (curbuf->b_p_bin || !curbuf->b_p_fixeol)
			    && (long)STRLEN(s) < len)
			byte_എണ്ണം_cursor -= eol_size;
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		// In non-visual mode, check for the line the cursor is on
		എങ്കിൽ (lnum == curwin->w_cursor.lnum)
		{
		    word_എണ്ണം_cursor += word_എണ്ണം;
		    char_എണ്ണം_cursor += char_എണ്ണം;
		    byte_എണ്ണം_cursor = byte_എണ്ണം +
			വരി_എണ്ണം_info(ml_get(lnum),
				&word_എണ്ണം_cursor, &char_എണ്ണം_cursor,
				(varnumber_T)(curwin->w_cursor.col + 1),
				eol_size);
		}
	    }
	    // Add to the running totals
	    byte_എണ്ണം += വരി_എണ്ണം_info(ml_get(lnum), &word_എണ്ണം,
					 &char_എണ്ണം, (varnumber_T)MAXCOL,
					 eol_size);
	}

	// Correction for when last line doesn't have an EOL.
	എങ്കിൽ (!curbuf->b_p_eol && (curbuf->b_p_bin || !curbuf->b_p_fixeol))
	    byte_എണ്ണം -= eol_size;

	എങ്കിൽ (dict == ശൂന്യ)
	{
	    എങ്കിൽ (VIsual_active)
	    {
		എങ്കിൽ (VIsual_mode == Ctrl_V && curwin->w_curswant < MAXCOL)
		{
		    getvcols(curwin, &min_pos, &max_pos,
						&min_pos.col, &max_pos.col, 0);
		    vim_snprintf((അക്ഷരം *)buf1, വലുപ്പം(buf1), _("%ld Cols; "),
			    (long)(oparg.end_vcol - oparg.start_vcol + 1));
		}
		അല്ലെങ്കിൽ
		    buf1[0] = NUL;

		എങ്കിൽ (char_എണ്ണം_cursor == byte_എണ്ണം_cursor
						    && char_എണ്ണം == byte_എണ്ണം)
		    vim_snprintf((അക്ഷരം *)IObuff, IOSIZE,
			    _("Selected %s%ld of %ld Lines; %lld of %lld Words; %lld of %lld Bytes"),
			    buf1, വരി_എണ്ണം_selected,
			    (long)curbuf->b_ml.ml_വരി_എണ്ണം,
			    word_എണ്ണം_cursor,
			    word_എണ്ണം,
			    byte_എണ്ണം_cursor,
			    byte_എണ്ണം);
		അല്ലെങ്കിൽ
		    vim_snprintf((അക്ഷരം *)IObuff, IOSIZE,
			    _("Selected %s%ld of %ld Lines; %lld of %lld Words; %lld of %lld Chars; %lld of %lld Bytes"),
			    buf1, വരി_എണ്ണം_selected,
			    (long)curbuf->b_ml.ml_വരി_എണ്ണം,
			    word_എണ്ണം_cursor,
			    word_എണ്ണം,
			    char_എണ്ണം_cursor,
			    char_എണ്ണം,
			    byte_എണ്ണം_cursor,
			    byte_എണ്ണം);
	    }
	    അല്ലെങ്കിൽ
	    {
		p = ml_get_curline();
		validate_virtcol();
		col_print(buf1, വലുപ്പം(buf1), (പൂർണ്ണസംഖ്യ)curwin->w_cursor.col + 1,
			(പൂർണ്ണസംഖ്യ)curwin->w_virtcol + 1);
		col_print(buf2, വലുപ്പം(buf2), ml_get_curline_len(),
							   linetabsize_str(p));

		എങ്കിൽ (char_എണ്ണം_cursor == byte_എണ്ണം_cursor
			&& char_എണ്ണം == byte_എണ്ണം)
		    vim_snprintf((അക്ഷരം *)IObuff, IOSIZE,
			_("Col %s of %s; Line %ld of %ld; Word %lld of %lld; Byte %lld of %lld"),
			(അക്ഷരം *)buf1, (അക്ഷരം *)buf2,
			(long)curwin->w_cursor.lnum,
			(long)curbuf->b_ml.ml_വരി_എണ്ണം,
			word_എണ്ണം_cursor, word_എണ്ണം,
			byte_എണ്ണം_cursor, byte_എണ്ണം);
		അല്ലെങ്കിൽ
		    vim_snprintf((അക്ഷരം *)IObuff, IOSIZE,
			_("Col %s of %s; Line %ld of %ld; Word %lld of %lld; Char %lld of %lld; Byte %lld of %lld"),
			(അക്ഷരം *)buf1, (അക്ഷരം *)buf2,
			(long)curwin->w_cursor.lnum,
			(long)curbuf->b_ml.ml_വരി_എണ്ണം,
			word_എണ്ണം_cursor, word_എണ്ണം,
			char_എണ്ണം_cursor, char_എണ്ണം,
			byte_എണ്ണം_cursor, byte_എണ്ണം);
	    }
	}

	bom_എണ്ണം = bomb_size();
	എങ്കിൽ (dict == ശൂന്യ && bom_എണ്ണം > 0)
	{
	    size_t len = STRLEN(IObuff);

	    vim_snprintf((അക്ഷരം *)IObuff + len, IOSIZE - len,
				 _("(+%lld for BOM)"), bom_എണ്ണം);
	}
	എങ്കിൽ (dict == ശൂന്യ)
	{
	    // Don't shorten this message, the user asked for it.
	    p = p_shm;
	    p_shm = (char_u *)"";
	    സന്ദേശം((അക്ഷരം *)IObuff);
	    p_shm = p;
	}
    }
#എങ്കിൽ defined(FEAT_EVAL)
    എങ്കിൽ (dict != ശൂന്യ)
    {
	dict_add_സംഖ്യ(dict, "words", word_എണ്ണം);
	dict_add_സംഖ്യ(dict, "chars", char_എണ്ണം);
	dict_add_സംഖ്യ(dict, "bytes", byte_എണ്ണം + bom_എണ്ണം);
	dict_add_സംഖ്യ(dict, VIsual_active ? "visual_bytes" : "cursor_bytes",
		byte_എണ്ണം_cursor);
	dict_add_സംഖ്യ(dict, VIsual_active ? "visual_chars" : "cursor_chars",
		char_എണ്ണം_cursor);
	dict_add_സംഖ്യ(dict, VIsual_active ? "visual_words" : "cursor_words",
		word_എണ്ണം_cursor);
    }
#endif
}

/*
 * Handle indent and format operators and visual mode ":".
 */
    സ്ഥിതം ശൂന്യം
op_colon(oparg_T *oap)
{
    stuffcharReadbuff(':');
    എങ്കിൽ (oap->is_VIsual)
	stuffReadbuff((char_u *)"'<,'>");
    അല്ലെങ്കിൽ
    {
	// Make the range look nice, so it can be repeated.
	എങ്കിൽ (oap->start.lnum == curwin->w_cursor.lnum)
	    stuffcharReadbuff('.');
	അല്ലെങ്കിൽ
	    stuffnumReadbuff((long)oap->start.lnum);

#ifdef FEAT_FOLDING
	// When using !! on a closed fold the range ".!" works best to operate
	// on, it will be made the whole closed fold later.
	linenr_T endOfStartFold = oap->start.lnum;
	(ശൂന്യം)hasFolding(oap->start.lnum, ശൂന്യ, &endOfStartFold);
#endif
	എങ്കിൽ (oap->end.lnum != oap->start.lnum
#ifdef FEAT_FOLDING
		&& oap->end.lnum != endOfStartFold
#endif
	   )
	{
	    // Make it a range with the end line.
	    stuffcharReadbuff(',');
	    എങ്കിൽ (oap->end.lnum == curwin->w_cursor.lnum)
		stuffcharReadbuff('.');
	    അല്ലെങ്കിൽ എങ്കിൽ (oap->end.lnum == curbuf->b_ml.ml_വരി_എണ്ണം)
		stuffcharReadbuff('$');
	    അല്ലെങ്കിൽ എങ്കിൽ (oap->start.lnum == curwin->w_cursor.lnum
#ifdef FEAT_FOLDING
		    // do not use ".+number" for a closed fold, it would count
		    // folded lines twice
		    && !hasFolding(oap->end.lnum, ശൂന്യ, ശൂന്യ)
#endif
		    )
	    {
		stuffReadbuff((char_u *)".+");
		stuffnumReadbuff((long)oap->വരി_എണ്ണം - 1);
	    }
	    അല്ലെങ്കിൽ
		stuffnumReadbuff((long)oap->end.lnum);
	}
    }
    എങ്കിൽ (oap->op_type != OP_COLON)
	stuffReadbuff((char_u *)"!");
    എങ്കിൽ (oap->op_type == OP_INDENT)
    {
	എങ്കിൽ (*get_equalprg() == NUL)
	    stuffReadbuff((char_u *)"indent");
	അല്ലെങ്കിൽ
	    stuffReadbuff(get_equalprg());
	stuffReadbuff((char_u *)"\n");
    }
    അല്ലെങ്കിൽ എങ്കിൽ (oap->op_type == OP_FORMAT)
    {
	എങ്കിൽ (*curbuf->b_p_fp != NUL)
	    stuffReadbuff(curbuf->b_p_fp);
	അല്ലെങ്കിൽ എങ്കിൽ (*p_fp != NUL)
	    stuffReadbuff(p_fp);
	അല്ലെങ്കിൽ
	    stuffReadbuff((char_u *)"fmt");
	stuffReadbuff((char_u *)"\n']");
    }

    // do_cmdline() does the rest
}

// callback function for 'operatorfunc'
സ്ഥിതം callback_T opfunc_cb;

/*
 * Process the 'operatorfunc' option value.
 * Returns OK or FAIL.
 */
    അക്ഷരം *
did_set_operatorfunc(optset_T *args UNUSED)
{
    എങ്കിൽ (option_set_callback_func(p_opfunc, &opfunc_cb) == FAIL)
	തിരികെ e_invalid_argument;

    തിരികെ ശൂന്യ;
}

#എങ്കിൽ defined(EXITFREE)
    ശൂന്യം
free_operatorfunc_option(ശൂന്യം)
{
# ifdef FEAT_EVAL
    free_callback(&opfunc_cb);
# endif
}
#endif

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Mark the global 'operatorfunc' callback with "copyID" so that it is not
 * garbage collected.
 */
    പൂർണ്ണസംഖ്യ
set_ref_in_opfunc(പൂർണ്ണസംഖ്യ copyID UNUSED)
{
    പൂർണ്ണസംഖ്യ abort = FALSE;

    abort = set_ref_in_callback(&opfunc_cb, copyID);

    തിരികെ abort;
}
#endif

/*
 * Handle the "g@" operator: call 'operatorfunc'.
 */
    സ്ഥിതം ശൂന്യം
op_function(oparg_T *oap UNUSED)
{
#ifdef FEAT_EVAL
    typval_T	argv[2];
    pos_T	orig_start = curbuf->b_op_start;
    pos_T	orig_end = curbuf->b_op_end;
    typval_T	rettv;

    എങ്കിൽ (*p_opfunc == NUL)
	emsg(_(e_operatorfunc_is_empty));
    അല്ലെങ്കിൽ
    {
	// Set '[ and '] marks to text to be operated on.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
	എങ്കിൽ (oap->motion_type != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_end);

	argv[0].v_type = VAR_STRING;
	എങ്കിൽ (oap->block_mode)
	    argv[0].vval.v_string = (char_u *)"block";
	അല്ലെങ്കിൽ എങ്കിൽ (oap->motion_type == MLINE)
	    argv[0].vval.v_string = (char_u *)"line";
	അല്ലെങ്കിൽ
	    argv[0].vval.v_string = (char_u *)"char";
	argv[1].v_type = VAR_UNKNOWN;

	// Reset virtual_op so that 'virtualedit' can be changed in the
	// function.
	പൂർണ്ണസംഖ്യ save_virtual_op = virtual_op;
	virtual_op = MAYBE;

	// Reset finish_op so that mode() returns the right value.
	പൂർണ്ണസംഖ്യ save_finish_op = finish_op;
	finish_op = FALSE;

	എങ്കിൽ (call_callback(&opfunc_cb, 0, &rettv, 1, argv) != FAIL)
	    clear_tv(&rettv);

	virtual_op = save_virtual_op;
	finish_op = save_finish_op;
	എങ്കിൽ (cmdmod.cmod_flags & CMOD_LOCKMARKS)
	{
	    curbuf->b_op_start = orig_start;
	    curbuf->b_op_end = orig_end;
	}
    }
#അല്ലെങ്കിൽ
    emsg(_(e_eval_feature_not_available));
#endif
}

/*
 * Calculate start/end virtual columns for operating in block mode.
 */
    സ്ഥിതം ശൂന്യം
get_op_vcol(
    oparg_T	*oap,
    colnr_T	redo_VIsual_vcol,
    പൂർണ്ണസംഖ്യ		initial)    // when TRUE adjust position for 'selectmode'
{
    colnr_T	    start, end;

    എങ്കിൽ (VIsual_mode != Ctrl_V
	    || (!initial && oap->end.col < curwin->w_width))
	തിരികെ;

    oap->block_mode = TRUE;

    // prevent from moving onto a trail byte
    എങ്കിൽ (has_mbyte)
	mb_adjustpos(curwin->w_buffer, &oap->end);

    getvvcol(curwin, &(oap->start), &oap->start_vcol, ശൂന്യ, &oap->end_vcol, 0);

    എങ്കിൽ (!redo_VIsual_busy)
    {
	getvvcol(curwin, &(oap->end), &start, ശൂന്യ, &end, 0);

	എങ്കിൽ (start < oap->start_vcol)
	    oap->start_vcol = start;
	എങ്കിൽ (end > oap->end_vcol)
	{
	    എങ്കിൽ (initial && *p_sel == 'e' && start >= 1
				    && start - 1 >= oap->end_vcol)
		oap->end_vcol = start - 1;
	    അല്ലെങ്കിൽ
		oap->end_vcol = end;
	}
    }

    // if '$' was used, get oap->end_vcol from longest line
    എങ്കിൽ (curwin->w_curswant == MAXCOL)
    {
	curwin->w_cursor.col = MAXCOL;
	oap->end_vcol = 0;
	വേണ്ടി (curwin->w_cursor.lnum = oap->start.lnum;
		curwin->w_cursor.lnum <= oap->end.lnum;
					++curwin->w_cursor.lnum)
	{
	    getvvcol(curwin, &curwin->w_cursor, ശൂന്യ, ശൂന്യ, &end, 0);
	    എങ്കിൽ (end > oap->end_vcol)
		oap->end_vcol = end;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (redo_VIsual_busy)
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
തരനിർവചനം ഘടന {
    പൂർണ്ണസംഖ്യ		rv_mode;	// 'v', 'V', or Ctrl-V
    linenr_T	rv_വരി_എണ്ണം;	// number of lines
    colnr_T	rv_vcol;	// number of cols or end column
    long	rv_എണ്ണം;	// count for Visual operator
    പൂർണ്ണസംഖ്യ		rv_arg;		// extra argument
} redo_VIsual_T;

    സ്ഥിതം പൂർണ്ണസംഖ്യ
is_ex_cmdchar(cmdarg_T *cap)
{
    തിരികെ cap->cmdchar == ':'
	|| cap->cmdchar == K_COMMAND
	|| cap->cmdchar == K_SCRIPT_COMMAND;
}

/*
 * Handle an operator after Visual mode or when the movement is finished.
 * "gui_yank" is true when yanking text for the clipboard.
 */
    ശൂന്യം
do_pending_operator(cmdarg_T *cap, പൂർണ്ണസംഖ്യ old_col, പൂർണ്ണസംഖ്യ gui_yank)
{
    oparg_T	*oap = cap->oap;
    pos_T	old_cursor;
    പൂർണ്ണസംഖ്യ		empty_region_error;
    പൂർണ്ണസംഖ്യ		restart_edit_save;
#ifdef FEAT_LINEBREAK
    പൂർണ്ണസംഖ്യ		lbr_saved = curwin->w_p_lbr;
#endif

    // The visual area is remembered for redo
    സ്ഥിതം redo_VIsual_T   redo_VIsual = {NUL, 0, 0, 0,0};

    പൂർണ്ണസംഖ്യ		    include_വരി_break = FALSE;

#എങ്കിൽ defined(FEAT_CLIPBOARD)
    // Yank the visual area into the GUI selection register before we operate
    // on it and lose it forever.
    // Don't do it if a specific register was specified, so that ""x"*P works.
    // This could call do_pending_operator() recursively, but that's OK
    // because gui_yank will be TRUE for the nested call.
    എങ്കിൽ ((clip_star.available || clip_plus.available)
	    && oap->op_type != OP_NOP
	    && !gui_yank
	    && VIsual_active
	    && !redo_VIsual_busy
	    && oap->regname == 0)
	clip_auto_select();
#endif
    old_cursor = curwin->w_cursor;

    // If an operation is pending, handle it...
    എങ്കിൽ ((finish_op || VIsual_active) && oap->op_type != OP_NOP)
    {
	// Yank can be redone when 'y' is in 'cpoptions', but not when yanking
	// for the clipboard.
	പൂർണ്ണസംഖ്യ	redo_yank = vim_strchr(p_cpo, CPO_YANK) != ശൂന്യ && !gui_yank;

#ifdef FEAT_LINEBREAK
	// Avoid a problem with unwanted linebreaks in block mode.
	(ശൂന്യം)reset_lbr();
#endif
	oap->is_VIsual = VIsual_active;
	എങ്കിൽ (oap->motion_force == 'V')
	    oap->motion_type = MLINE;
	അല്ലെങ്കിൽ എങ്കിൽ (oap->motion_force == 'v')
	{
	    // If the motion was linewise, "inclusive" will not have been set.
	    // Use "exclusive" to be consistent.  Makes "dvj" work nice.
	    എങ്കിൽ (oap->motion_type == MLINE)
		oap->inclusive = FALSE;
	    // If the motion already was characterwise, toggle "inclusive"
	    അല്ലെങ്കിൽ എങ്കിൽ (oap->motion_type == MCHAR)
		oap->inclusive = !oap->inclusive;
	    oap->motion_type = MCHAR;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (oap->motion_force == Ctrl_V)
	{
	    // Change line- or characterwise motion into Visual block mode.
	    എങ്കിൽ (!VIsual_active)
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
	എങ്കിൽ ((redo_yank || oap->op_type != OP_YANK)
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
	    എങ്കിൽ (cap->cmdchar == '/' || cap->cmdchar == '?') // was a search
	    {
		// If 'cpoptions' does not contain 'r', insert the search
		// pattern to really repeat the same command.
		എങ്കിൽ (vim_strchr(p_cpo, CPO_REDO) == ശൂന്യ)
		    AppendToRedobuffLit(cap->searchbuf, -1);
		AppendToRedobuff(NL_STR);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (is_ex_cmdchar(cap))
	    {
		// do_cmdline() has stored the first typed line in
		// "repeat_cmdline".  When several lines are typed repeating
		// won't be possible.
		എങ്കിൽ (repeat_cmdline == ശൂന്യ)
		    ResetRedobuff();
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (cap->cmdchar == ':')
			AppendToRedobuffLit(repeat_cmdline, -1);
		    അല്ലെങ്കിൽ
			AppendToRedobuffSpec(repeat_cmdline);
		    AppendToRedobuff(NL_STR);
		    VIM_CLEAR(repeat_cmdline);
		}
	    }
	}

	എങ്കിൽ (redo_VIsual_busy)
	{
	    // Redo of an operation on a Visual area. Use the same size from
	    // redo_VIsual.rv_line_count and redo_VIsual.rv_vcol.
	    oap->start = curwin->w_cursor;
	    curwin->w_cursor.lnum += redo_VIsual.rv_വരി_എണ്ണം - 1;
	    എങ്കിൽ (curwin->w_cursor.lnum > curbuf->b_ml.ml_വരി_എണ്ണം)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_വരി_എണ്ണം;
	    VIsual_mode = redo_VIsual.rv_mode;
	    എങ്കിൽ (redo_VIsual.rv_vcol == MAXCOL || VIsual_mode == 'v')
	    {
		എങ്കിൽ (VIsual_mode == 'v')
		{
		    എങ്കിൽ (redo_VIsual.rv_വരി_എണ്ണം <= 1)
		    {
			validate_virtcol();
			curwin->w_curswant =
				     curwin->w_virtcol + redo_VIsual.rv_vcol - 1;
		    }
		    അല്ലെങ്കിൽ
			curwin->w_curswant = redo_VIsual.rv_vcol;
		}
		അല്ലെങ്കിൽ
		{
		    curwin->w_curswant = MAXCOL;
		}
		coladvance(curwin->w_curswant);
	    }
	    cap->count0 = redo_VIsual.rv_എണ്ണം;
	    എങ്കിൽ (redo_VIsual.rv_എണ്ണം != 0)
		cap->count1 = redo_VIsual.rv_എണ്ണം;
	    അല്ലെങ്കിൽ
		cap->count1 = 1;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (VIsual_active)
	{
	    എങ്കിൽ (!gui_yank)
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
	    എങ്കിൽ (VIsual_select && VIsual_mode == 'V'
					    && cap->oap->op_type != OP_DELETE)
	    {
		എങ്കിൽ (LT_POS(VIsual, curwin->w_cursor))
		{
		    VIsual.col = 0;
		    curwin->w_cursor.col = ml_get_len(curwin->w_cursor.lnum);
		}
		അല്ലെങ്കിൽ
		{
		    curwin->w_cursor.col = 0;
		    VIsual.col = ml_get_len(VIsual.lnum);
		}
		VIsual_mode = 'v';
	    }
	    // If 'selection' is "exclusive", backup one character for
	    // charwise selections.
	    അല്ലെങ്കിൽ എങ്കിൽ (VIsual_mode == 'v')
		include_വരി_break = unadjust_for_sel();

	    oap->start = VIsual;
	    എങ്കിൽ (VIsual_mode == 'V')
	    {
		oap->start.col = 0;
		oap->start.coladd = 0;
	    }
	}

	// Set oap->start to the first position of the operated text, oap->end
	// to the end of the operated text.  w_cursor is equal to oap->start.
	എങ്കിൽ (LT_POS(oap->start, curwin->w_cursor))
	{
#ifdef FEAT_FOLDING
	    // Include folded lines completely.
	    എങ്കിൽ (!VIsual_active)
	    {
		എങ്കിൽ (hasFolding(oap->start.lnum, &oap->start.lnum, ശൂന്യ))
		    oap->start.col = 0;
		എങ്കിൽ ((curwin->w_cursor.col > 0 || oap->inclusive
						  || oap->motion_type == MLINE)
			&& hasFolding(curwin->w_cursor.lnum, ശൂന്യ,
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
	അല്ലെങ്കിൽ
	{
#ifdef FEAT_FOLDING
	    // Include folded lines completely.
	    എങ്കിൽ (!VIsual_active && oap->motion_type == MLINE)
	    {
		എങ്കിൽ (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum,
									ശൂന്യ))
		    curwin->w_cursor.col = 0;
		എങ്കിൽ (hasFolding(oap->start.lnum, ശൂന്യ, &oap->start.lnum))
		    oap->start.col = ml_get_len(oap->start.lnum);
	    }
#endif
	    oap->end = oap->start;
	    oap->start = curwin->w_cursor;
	}

	// Just in case lines were deleted that make the position invalid.
	check_pos(curwin->w_buffer, &oap->end);
	oap->വരി_എണ്ണം = oap->end.lnum - oap->start.lnum + 1;

	// Set "virtual_op" before resetting VIsual_active.
	virtual_op = virtual_active();

	എങ്കിൽ (VIsual_active || redo_VIsual_busy)
	{
	    get_op_vcol(oap, redo_VIsual.rv_vcol, TRUE);

	    എങ്കിൽ (!redo_VIsual_busy && !gui_yank)
	    {
		// Prepare to reselect and redo Visual: this is based on the
		// size of the Visual text
		resel_VIsual_mode = VIsual_mode;
		എങ്കിൽ (curwin->w_curswant == MAXCOL)
		    resel_VIsual_vcol = MAXCOL;
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (VIsual_mode != Ctrl_V)
			getvvcol(curwin, &(oap->end),
						ശൂന്യ, ശൂന്യ, &oap->end_vcol, 0);
		    എങ്കിൽ (VIsual_mode == Ctrl_V || oap->വരി_എണ്ണം <= 1)
		    {
			എങ്കിൽ (VIsual_mode != Ctrl_V)
			    getvvcol(curwin, &(oap->start),
					      &oap->start_vcol, ശൂന്യ, ശൂന്യ, 0);
			resel_VIsual_vcol = oap->end_vcol - oap->start_vcol + 1;
		    }
		    അല്ലെങ്കിൽ
			resel_VIsual_vcol = oap->end_vcol;
		}
		resel_VIsual_വരി_എണ്ണം = oap->വരി_എണ്ണം;
	    }

	    // can't redo yank (unless 'y' is in 'cpoptions') and ":"
	    എങ്കിൽ ((redo_yank || oap->op_type != OP_YANK)
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
		എങ്കിൽ (cap->cmdchar == 'g' && (cap->nchar == 'n'
							|| cap->nchar == 'N'))
		    prep_redo(oap->regname, cap->count0,
			    get_op_char(oap->op_type),
			    get_extra_op_char(oap->op_type),
			    oap->motion_force, cap->cmdchar, cap->nchar);
		അല്ലെങ്കിൽ എങ്കിൽ (!is_ex_cmdchar(cap))
		{
		    പൂർണ്ണസംഖ്യ opchar = get_op_char(oap->op_type);
		    പൂർണ്ണസംഖ്യ extra_opchar = get_extra_op_char(oap->op_type);
		    പൂർണ്ണസംഖ്യ nchar = oap->op_type == OP_REPLACE ? cap->nchar : NUL;

		    // reverse what nv_replace() did
		    എങ്കിൽ (nchar == REPLACE_CR_NCHAR)
			nchar = CAR;
		    അല്ലെങ്കിൽ എങ്കിൽ (nchar == REPLACE_NL_NCHAR)
			nchar = NL;

		    എങ്കിൽ (opchar == 'g' && extra_opchar == '@')
			// also repeat the count for 'operatorfunc'
			prep_redo_num2(oap->regname, 0L, NUL, 'v',
				     cap->count0, opchar, extra_opchar, nchar);
		    അല്ലെങ്കിൽ
			prep_redo(oap->regname, 0L, NUL, 'v',
						  opchar, extra_opchar, nchar);
		}
		എങ്കിൽ (!redo_VIsual_busy)
		{
		    redo_VIsual.rv_mode = resel_VIsual_mode;
		    redo_VIsual.rv_vcol = resel_VIsual_vcol;
		    redo_VIsual.rv_വരി_എണ്ണം = resel_VIsual_വരി_എണ്ണം;
		    redo_VIsual.rv_എണ്ണം = cap->count0;
		    redo_VIsual.rv_arg = cap->arg;
		}
	    }

	    // oap->inclusive defaults to TRUE.
	    // If oap->end is on a NUL (empty line) oap->inclusive becomes
	    // FALSE.  This makes "d}P" and "v}dP" work the same.
	    എങ്കിൽ (oap->motion_force == NUL || oap->motion_type == MLINE)
		oap->inclusive = TRUE;
	    എങ്കിൽ (VIsual_mode == 'V')
		oap->motion_type = MLINE;
	    അല്ലെങ്കിൽ
	    {
		oap->motion_type = MCHAR;
		എങ്കിൽ (VIsual_mode != Ctrl_V && *ml_get_pos(&(oap->end)) == NUL
			&& (include_വരി_break || !virtual_op))
		{
		    oap->inclusive = FALSE;
		    // Try to include the newline, unless it's an operator
		    // that works on lines only.
		    എങ്കിൽ (*p_sel != 'o'
			    && !op_on_വരികൾ(oap->op_type)
			    && oap->end.lnum < curbuf->b_ml.ml_വരി_എണ്ണം)
		    {
			++oap->end.lnum;
			oap->end.col = 0;
			oap->end.coladd = 0;
			++oap->വരി_എണ്ണം;
		    }
		}
	    }

	    redo_VIsual_busy = FALSE;

	    // Switch Visual off now, so screen updating does
	    // not show inverted text when the screen is redrawn.
	    // With OP_YANK and sometimes with OP_COLON and OP_FILTER there is
	    // no screen redraw, so it is done here to remove the inverted
	    // part.
	    എങ്കിൽ (!gui_yank)
	    {
		VIsual_active = FALSE;
		setmouse();
		mouse_dragging = 0;
		may_clear_cmdline();
		എങ്കിൽ ((oap->op_type == OP_YANK
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
	എങ്കിൽ (has_mbyte && oap->inclusive)
	{
	    പൂർണ്ണസംഖ്യ		l;

	    l = (*mb_ptr2len)(ml_get_pos(&oap->end));
	    എങ്കിൽ (l > 1)
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
				&& vim_strchr(p_cpo, CPO_EMPTYREGION) != ശൂന്യ);

	// Force a redraw when operating on an empty Visual region, when
	// 'modifiable is off or creating a fold.
	എങ്കിൽ (oap->is_VIsual && (oap->empty || !curbuf->b_p_ma
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
	എങ്കിൽ (	   oap->motion_type == MCHAR
		&& oap->inclusive == FALSE
		&& !(cap->retval & CA_NO_ADJ_OP_END)
		&& oap->end.col == 0
		&& (!oap->is_VIsual || *p_sel == 'o')
		&& !oap->block_mode
		&& oap->വരി_എണ്ണം > 1)
	{
	    oap->end_adjusted = TRUE;	    // remember that we did this
	    --oap->വരി_എണ്ണം;
	    --oap->end.lnum;
	    എങ്കിൽ (inindent(0))
		oap->motion_type = MLINE;
	    അല്ലെങ്കിൽ
	    {
		oap->end.col = ml_get_len(oap->end.lnum);
		എങ്കിൽ (oap->end.col)
		{
		    --oap->end.col;
		    oap->inclusive = TRUE;
		}
	    }
	}
	അല്ലെങ്കിൽ
	    oap->end_adjusted = FALSE;

	തിരഞ്ഞെടുപ്പ് (oap->op_type)
	{
	അവസ്ഥ OP_LSHIFT:
	അവസ്ഥ OP_RSHIFT:
	    op_shift(oap, TRUE, oap->is_VIsual ? (പൂർണ്ണസംഖ്യ)cap->count1 : 1);
	    auto_format(FALSE, TRUE);
	    നിർത്തുക;

	അവസ്ഥ OP_JOIN_NS:
	അവസ്ഥ OP_JOIN:
	    എങ്കിൽ (oap->വരി_എണ്ണം < 2)
		oap->വരി_എണ്ണം = 2;
	    എങ്കിൽ (curwin->w_cursor.lnum + oap->വരി_എണ്ണം - 1 >
						   curbuf->b_ml.ml_വരി_എണ്ണം)
		beep_flush();
	    അല്ലെങ്കിൽ
	    {
		(ശൂന്യം)do_join(oap->വരി_എണ്ണം, oap->op_type == OP_JOIN,
							    TRUE, TRUE, TRUE);
		auto_format(FALSE, TRUE);
	    }
	    നിർത്തുക;

	അവസ്ഥ OP_DELETE:
	    VIsual_reselect = FALSE;	    // don't reselect now
	    എങ്കിൽ (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    അല്ലെങ്കിൽ
	    {
		(ശൂന്യം)op_delete(oap);
		// save cursor line for undo if it wasn't saved yet
		എങ്കിൽ (oap->motion_type == MLINE && has_format_option(FO_AUTO)
						      && u_save_cursor() == OK)
		    auto_format(FALSE, TRUE);
	    }
	    നിർത്തുക;

	അവസ്ഥ OP_YANK:
	    എങ്കിൽ (empty_region_error)
	    {
		എങ്കിൽ (!gui_yank)
		{
		    vim_beep(BO_OPER);
		    CancelRedo();
		}
	    }
	    അല്ലെങ്കിൽ
	    {
#ifdef FEAT_LINEBREAK
		restore_lbr(lbr_saved);
#endif
		oap->excl_tr_ws = cap->cmdchar == 'z';
		(ശൂന്യം)op_yank(oap, FALSE, !gui_yank);
	    }
	    check_cursor_col();
	    നിർത്തുക;

	അവസ്ഥ OP_CHANGE:
	    VIsual_reselect = FALSE;	    // don't reselect now
	    എങ്കിൽ (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    അല്ലെങ്കിൽ
	    {
		// This is a new edit command, not a restart.  Need to
		// remember it to make 'insertmode' work with mappings for
		// Visual mode.  But do this only once and not when typed and
		// 'insertmode' isn't set.
		എങ്കിൽ (p_im || !താക്കോൽ_Typed)
		    restart_edit_save = restart_edit;
		അല്ലെങ്കിൽ
		    restart_edit_save = 0;
		restart_edit = 0;
#ifdef FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#endif
		// trigger TextChangedI
		curbuf->b_last_changedtick_i = CHANGEDTICK(curbuf);

		എങ്കിൽ (op_change(oap))	// will call edit()
		    cap->retval |= CA_COMMAND_BUSY;
		എങ്കിൽ (restart_edit == 0)
		    restart_edit = restart_edit_save;
	    }
	    നിർത്തുക;

	അവസ്ഥ OP_FILTER:
	    എങ്കിൽ (vim_strchr(p_cpo, CPO_FILTER) != ശൂന്യ)
		AppendToRedobuff((char_u *)"!\r");  // use any last used !cmd
	    അല്ലെങ്കിൽ
		bangredo = TRUE;    // do_bang() will put cmd in redo buffer
	    // FALLTHROUGH

	അവസ്ഥ OP_INDENT:
	അവസ്ഥ OP_COLON:

	    // If 'equalprg' is empty, do the indenting internally.
	    എങ്കിൽ (oap->op_type == OP_INDENT && *get_equalprg() == NUL)
	    {
		എങ്കിൽ (curbuf->b_p_lisp)
		{
#ifdef FEAT_EVAL
		    എങ്കിൽ (use_indentexpr_for_lisp())
			op_reindent(oap, get_വ്യഞ്ജകം_indent);
		    അല്ലെങ്കിൽ
#endif
			op_reindent(oap, get_lisp_indent);
		    നിർത്തുക;
		}
		op_reindent(oap,
#ifdef FEAT_EVAL
			*curbuf->b_p_inde != NUL ? get_വ്യഞ്ജകം_indent :
#endif
			    get_c_indent);
		നിർത്തുക;
	    }

	    op_colon(oap);
	    നിർത്തുക;

	അവസ്ഥ OP_TILDE:
	അവസ്ഥ OP_UPPER:
	അവസ്ഥ OP_LOWER:
	അവസ്ഥ OP_ROT13:
	    എങ്കിൽ (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    അല്ലെങ്കിൽ
		op_tilde(oap);
	    check_cursor_col();
	    നിർത്തുക;

	അവസ്ഥ OP_FORMAT:
#എങ്കിൽ defined(FEAT_EVAL)
	    എങ്കിൽ (*curbuf->b_p_fex != NUL)
		op_formatexpr(oap);	// use expression
	    അല്ലെങ്കിൽ
#endif
	    {
		എങ്കിൽ (*p_fp != NUL || *curbuf->b_p_fp != NUL)
		    op_colon(oap);		// use external command
		അല്ലെങ്കിൽ
		    op_format(oap, FALSE);	// use internal function
	    }
	    നിർത്തുക;
	അവസ്ഥ OP_FORMAT2:
	    op_format(oap, TRUE);	// use internal function
	    നിർത്തുക;

	അവസ്ഥ OP_FUNCTION:
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
		നിർത്തുക;
	    }

	അവസ്ഥ OP_തിരുകുക:
	അവസ്ഥ OP_APPEND:
	    VIsual_reselect = FALSE;	// don't reselect now
	    എങ്കിൽ (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    അല്ലെങ്കിൽ
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

		op_തിരുകുക(oap, cap->count1);
#ifdef FEAT_LINEBREAK
		// Reset linebreak, so that formatting works correctly.
		(ശൂന്യം)reset_lbr();
#endif

		// TODO: when inserting in several lines, should format all
		// the lines.
		auto_format(FALSE, TRUE);

		എങ്കിൽ (restart_edit == 0)
		    restart_edit = restart_edit_save;
		അല്ലെങ്കിൽ
		    cap->retval |= CA_COMMAND_BUSY;
	    }
	    നിർത്തുക;

	അവസ്ഥ OP_REPLACE:
	    VIsual_reselect = FALSE;	// don't reselect now
	    എങ്കിൽ (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    അല്ലെങ്കിൽ
	    {
#ifdef FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#endif
		op_replace(oap, cap->nchar);
	    }
	    നിർത്തുക;

#ifdef FEAT_FOLDING
	അവസ്ഥ OP_FOLD:
	    VIsual_reselect = FALSE;	// don't reselect now
	    foldCreate(oap->start.lnum, oap->end.lnum);
	    നിർത്തുക;

	അവസ്ഥ OP_FOLDOPEN:
	അവസ്ഥ OP_FOLDOPENREC:
	അവസ്ഥ OP_FOLDCLOSE:
	അവസ്ഥ OP_FOLDCLOSEREC:
	    VIsual_reselect = FALSE;	// don't reselect now
	    opFoldRange(oap->start.lnum, oap->end.lnum,
		    oap->op_type == OP_FOLDOPEN
					    || oap->op_type == OP_FOLDOPENREC,
		    oap->op_type == OP_FOLDOPENREC
					  || oap->op_type == OP_FOLDCLOSEREC,
					  oap->is_VIsual);
	    നിർത്തുക;

	അവസ്ഥ OP_FOLDDEL:
	അവസ്ഥ OP_FOLDDELREC:
	    VIsual_reselect = FALSE;	// don't reselect now
	    deleteFold(oap->start.lnum, oap->end.lnum,
			       oap->op_type == OP_FOLDDELREC, oap->is_VIsual);
	    നിർത്തുക;
#endif
	അവസ്ഥ OP_NR_ADD:
	അവസ്ഥ OP_NR_SUB:
	    എങ്കിൽ (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    അല്ലെങ്കിൽ
	    {
		VIsual_active = TRUE;
#ifdef FEAT_LINEBREAK
		restore_lbr(lbr_saved);
#endif
		op_addsub(oap, cap->count1, redo_VIsual.rv_arg);
		VIsual_active = FALSE;
	    }
	    check_cursor_col();
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    clearopbeep(oap);
	}
	virtual_op = MAYBE;
	എങ്കിൽ (!gui_yank)
	{
	    // if 'sol' not set, go back to old column for some commands
	    എങ്കിൽ (!p_sol && oap->motion_type == MLINE && !oap->end_adjusted
		    && (oap->op_type == OP_LSHIFT || oap->op_type == OP_RSHIFT
						|| oap->op_type == OP_DELETE))
	    {
#ifdef FEAT_LINEBREAK
		(ശൂന്യം)reset_lbr();
#endif
		coladvance(curwin->w_curswant = old_col);
	    }
	}
	അല്ലെങ്കിൽ
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
    സ്ഥിതം ശൂന്യം
pbyte(pos_T lp, പൂർണ്ണസംഖ്യ c)
{
    char_u *p = ml_get_buf(curbuf, lp.lnum, TRUE);
    പൂർണ്ണസംഖ്യ	len = curbuf->b_ml.ml_വരി_len;

    // safety check
    എങ്കിൽ (lp.col >= len)
	lp.col = (len > 1 ? len - 2 : 0);
    *(p + lp.col) = c;
}
