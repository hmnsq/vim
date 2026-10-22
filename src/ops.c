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

#அந்தர்பாவ "vim.h"

நிலையான வெறுமை shift_block(oparg_T *oap, முழு amount);
நிலையான வெறுமை	mb_adjust_opend(oparg_T *oap);
நிலையான முழு	do_addsub(முழு op_type, pos_T *pos, முழு length, linenr_T Prenum1);
நிலையான வெறுமை	pbyte(pos_T lp, முழு c);
#define PBYTE(lp, c) pbyte(lp, c)


// Flags for third item in "opchars".
#define OPF_வரிகள்  1	// operator always works on lines
#define OPF_CHANGE 2	// operator changes text

/*
 * The names of operators.
 * IMPORTANT: Index must correspond with defines in vim.h!!!
 * The third field holds OPF_ flags.
 */
நிலையான மாறிலி எழுத்து opchars[][3] =
{
    {NUL, NUL, 0},			// OP_NOP
    {'d', NUL, OPF_CHANGE},		// OP_DELETE
    {'y', NUL, 0},			// OP_YANK
    {'c', NUL, OPF_CHANGE},		// OP_CHANGE
    {'<', NUL, OPF_வரிகள் | OPF_CHANGE},	// OP_LSHIFT
    {'>', NUL, OPF_வரிகள் | OPF_CHANGE},	// OP_RSHIFT
    {'!', NUL, OPF_வரிகள் | OPF_CHANGE},	// OP_FILTER
    {'g', '~', OPF_CHANGE},		// OP_TILDE
    {'=', NUL, OPF_வரிகள் | OPF_CHANGE},	// OP_INDENT
    {'g', 'q', OPF_வரிகள் | OPF_CHANGE},	// OP_FORMAT
    {':', NUL, OPF_வரிகள்},		// OP_COLON
    {'g', 'U', OPF_CHANGE},		// OP_UPPER
    {'g', 'u', OPF_CHANGE},		// OP_LOWER
    {'J', NUL, OPF_வரிகள் | OPF_CHANGE},	// DO_JOIN
    {'g', 'J', OPF_வரிகள் | OPF_CHANGE},	// DO_JOIN_NS
    {'g', '?', OPF_CHANGE},		// OP_ROT13
    {'r', NUL, OPF_CHANGE},		// OP_REPLACE
    {'I', NUL, OPF_CHANGE},		// OP_INSERT
    {'A', NUL, OPF_CHANGE},		// OP_APPEND
    {'z', 'f', OPF_வரிகள்},		// OP_FOLD
    {'z', 'o', OPF_வரிகள்},		// OP_FOLDOPEN
    {'z', 'O', OPF_வரிகள்},		// OP_FOLDOPENREC
    {'z', 'c', OPF_வரிகள்},		// OP_FOLDCLOSE
    {'z', 'C', OPF_வரிகள்},		// OP_FOLDCLOSEREC
    {'z', 'd', OPF_வரிகள்},		// OP_FOLDDEL
    {'z', 'D', OPF_வரிகள்},		// OP_FOLDDELREC
    {'g', 'w', OPF_வரிகள் | OPF_CHANGE},	// OP_FORMAT2
    {'g', '@', OPF_CHANGE},		// OP_FUNCTION
    {Ctrl_A, NUL, OPF_CHANGE},		// OP_NR_ADD
    {Ctrl_X, NUL, OPF_CHANGE},		// OP_NR_SUB
};

/*
 * Translate a command name into an operator type.
 * Must only be called with a valid operator name!
 */
    முழு
get_op_type(முழு char1, முழு char2)
{
    முழு		i;

    எனில் (char1 == 'r')		// ignore second character
	திரும்பு OP_REPLACE;
    எனில் (char1 == '~')		// when tilde is an operator
	திரும்பு OP_TILDE;
    எனில் (char1 == 'g' && char2 == Ctrl_A)	// add
	திரும்பு OP_NR_ADD;
    எனில் (char1 == 'g' && char2 == Ctrl_X)	// subtract
	திரும்பு OP_NR_SUB;
    எனில் (char1 == 'z' && char2 == 'y')	// OP_YANK
	திரும்பு OP_YANK;
    க்கு (i = 0; ; ++i)
    {
	எனில் (opchars[i][0] == char1 && opchars[i][1] == char2)
	    நிறுத்து;
	எனில் (i == (முழு)ARRAY_LENGTH(opchars) - 1)
	{
	    internal_error("get_op_type()");
	    நிறுத்து;
	}
    }
    திரும்பு i;
}

/*
 * Return TRUE if operator "op" always works on whole lines.
 */
    நிலையான முழு
op_on_வரிகள்(முழு op)
{
    திரும்பு opchars[op][2] & OPF_வரிகள்;
}

#எனில் defined(FEAT_JOB_CHANNEL)
/*
 * Return TRUE if operator "op" changes text.
 */
    முழு
op_is_change(முழு op)
{
    திரும்பு opchars[op][2] & OPF_CHANGE;
}
#endif

/*
 * Get first operator command character.
 * Returns 'g' or 'z' if there is another command character.
 */
    முழு
get_op_char(முழு optype)
{
    திரும்பு opchars[optype][0];
}

/*
 * Get second operator command character.
 */
    முழு
get_extra_op_char(முழு optype)
{
    திரும்பு opchars[optype][1];
}

/*
 * op_shift - handle a shift operation
 */
    வெறுமை
op_shift(oparg_T *oap, முழு curs_உச்சி, முழு amount)
{
    long	    i;
    முழு		    first_char;
    முழு		    block_col = 0;

    எனில் (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	திரும்பு;

    எனில் (oap->block_mode)
	block_col = curwin->w_cursor.col;

    க்கு (i = oap->வரி_மொத்தம்; --i >= 0; )
    {
	first_char = *ml_get_curline();
	எனில் (first_char == NUL)				// empty line
	    curwin->w_cursor.col = 0;
	இல்லையெனில் எனில் (oap->block_mode)
	    shift_block(oap, amount);
	இல்லையெனில்
	    // Move the line right if it doesn't start with '#', 'smartindent'
	    // isn't set or 'cindent' isn't set or '#' isn't in 'cino'.
	    எனில் (first_char != '#' || !preprocs_இடது())
		shift_வரி(oap->op_type == OP_LSHIFT, p_sr, amount, FALSE);
	++curwin->w_cursor.lnum;
    }

    changed_வரிகள்(oap->start.lnum, 0, oap->end.lnum + 1, 0L);
    எனில் (oap->block_mode)
    {
	curwin->w_cursor.lnum = oap->start.lnum;
	curwin->w_cursor.col = block_col;
    }
    இல்லையெனில் எனில் (curs_உச்சி)	    // put cursor on first line, for ">>"
    {
	curwin->w_cursor.lnum = oap->start.lnum;
	beginline(BL_SOL | BL_FIX);   // shift_line() may have set cursor.col
    }
    இல்லையெனில்
	--curwin->w_cursor.lnum;	// put cursor on last line, for ":>"

#ifdef FEAT_FOLDING
    // The cursor line is not in a closed fold
    foldOpenCursor();
#endif


    எனில் (oap->வரி_மொத்தம் > p_report)
    {
	எழுத்து	    *op;
	எழுத்து	    *செய்தி_வரி_single;
	எழுத்து	    *செய்தி_வரி_plural;

	எனில் (oap->op_type == OP_RSHIFT)
	    op = ">";
	இல்லையெனில்
	    op = "<";
	செய்தி_வரி_single = NGETTEXT("%ld line %sed %d time",
					     "%ld line %sed %d times", amount);
	செய்தி_வரி_plural = NGETTEXT("%ld lines %sed %d time",
					    "%ld lines %sed %d times", amount);
	vim_snprintf((எழுத்து *)IObuff, IOSIZE,
		NGETTEXT(செய்தி_வரி_single, செய்தி_வரி_plural, oap->வரி_மொத்தம்),
		oap->வரி_மொத்தம், op, amount);
	செய்தி_attr_keep((எழுத்து *)IObuff, 0, TRUE);
    }

    எனில் ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end.lnum = oap->end.lnum;
	curbuf->b_op_end.col = ml_get_len(oap->end.lnum);
	எனில் (curbuf->b_op_end.col > 0)
	    --curbuf->b_op_end.col;
    }
}

#ifdef FEAT_VARTABS
/*
 * Return the tabstop width at the index of the variable tabstop array.  If an
 * index greater than the length of the array is given, the last tabstop width
 * in the array is returned.
 */
    நிலையான முழு
get_vts(முழு *vts_array, முழு index)
{
    முழு	ts;

    எனில் (index < 1)
	ts = 0;
    இல்லையெனில் எனில் (index <= vts_array[0])
	ts = vts_array[index];
    இல்லையெனில்
	ts = vts_array[vts_array[0]];

    திரும்பு ts;
}

/*
 * Return the sum of all the tabstops through the index-th.
 */
    நிலையான முழு
get_vts_sum(முழு *vts_array, முழு index)
{
    முழு	sum = 0;
    முழு	i;

    // Perform the summation for indices within the actual array.
    க்கு (i = 1; i <= index && i <= vts_array[0]; i++)
	sum += vts_array[i];

    // Add tabstops whose indices exceed the actual array.
    எனில் (i <= index)
	sum += vts_array[vts_array[0]] * (index - vts_array[0]);

    திரும்பு sum;
}
#endif

    நிலையான vimlong_T
get_new_sw_indent(
    முழு		இடது,		// TRUE if shift is to the left
    முழு		round,		// TRUE if new indent is to be to a tabstop
    vimlong_T	amount,		// Number of shifts
    vimlong_T	sw_val)
{
    vimlong_T	மொத்தம் = get_indent();
    vimlong_T	i, j;

    எனில் (round)			// round off indent
    {
	i = trim_to_int(மொத்தம்) / sw_val;	// number of 'shiftwidth' rounded down
	j = trim_to_int(மொத்தம்) % sw_val;	// extra spaces
	எனில் (j && இடது)		// first remove extra spaces
	    --amount;
	எனில் (இடது)
	{
	    i -= amount;
	    எனில் (i < 0)
		i = 0;
	}
	இல்லையெனில்
	    i += amount;
	மொத்தம் = i * sw_val;
    }
    இல்லையெனில்			// original vi indent
    {
	எனில் (இடது)
	{
	    மொத்தம் -= sw_val * amount;
	    எனில் (மொத்தம் < 0)
		மொத்தம் = 0;
	}
	இல்லையெனில்
	    மொத்தம் += sw_val * amount;
    }

    திரும்பு மொத்தம்;
}

#ifdef FEAT_VARTABS
    நிலையான vimlong_T
get_new_vts_indent(
    முழு	இடது,			// TRUE if shift is to the left
    முழு	round,			// TRUE if new indent is to be to a tabstop
    முழு	amount,			// Number of shifts
    முழு	*vts_array)
{
    vimlong_T	indent = get_indent();
    முழு		vtsi = 0;
    முழு		vts_indent = 0;
    முழு		ts = 0;		// Silence uninitialized variable warning.
    முழு		offset;		// Extra indent spaces to the right of the
				// tabstop

    // Find the tabstop at or to the left of the current indent.
    போது (vts_indent <= indent)
    {
	vtsi++;
	ts = get_vts(vts_array, vtsi);
	vts_indent += ts;
    }
    vts_indent -= ts;
    vtsi--;

    offset = indent - vts_indent;

    எனில் (round)
    {
	எனில் (இடது)
	{
	    எனில் (offset == 0)
		indent = get_vts_sum(vts_array, vtsi - amount);
	    இல்லையெனில்
		indent = get_vts_sum(vts_array, vtsi - (amount - 1));
	}
	இல்லையெனில்
	    indent = get_vts_sum(vts_array, vtsi + amount);
    }
    இல்லையெனில்
    {
	எனில் (இடது)
	{
	    எனில் (amount > vtsi)
		indent = 0;
	    இல்லையெனில்
		indent = get_vts_sum(vts_array, vtsi - amount) + offset;
	}
	இல்லையெனில்
	    indent = get_vts_sum(vts_array, vtsi + amount) + offset;
    }

    திரும்பு indent;
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
    வெறுமை
shift_வரி(
    முழு	இடது,			// TRUE if shift is to the left
    முழு	round,			// TRUE if new indent is to be to a tabstop
    முழு	amount,			// Number of shifts
    முழு	call_changed_bytes)	// call changed_bytes()
{
    vimlong_T	மொத்தம்;
    long	sw_val = curbuf->b_p_sw;
    long	ts_val = curbuf->b_p_ts;
#ifdef FEAT_VARTABS
    முழு		*vts_array = curbuf->b_p_vts_array;
#endif

    எனில் (sw_val != 0)
	// 'shiftwidth' is not zero; use it as the shift size.
	மொத்தம் = get_new_sw_indent(இடது, round, amount, sw_val);
    இல்லையெனில்
#ifdef FEAT_VARTABS
	எனில் ((vts_array == வெற்று) || (vts_array[0] == 0))
#endif
    {
	// 'shiftwidth is zero and 'vartabstop' is empty; use 'tabstop' as the
	// shift size.
	மொத்தம் = get_new_sw_indent(இடது, round, amount, ts_val);
    }
#ifdef FEAT_VARTABS
    இல்லையெனில்
    {
	// 'shiftwidth is zero and 'vartabstop' is defined; use 'vartabstop'
	// to determine the new indent.
	மொத்தம் = get_new_vts_indent(இடது, round, amount, vts_array);
    }
#endif

    // Set new indent
    எனில் (State & VREPLACE_FLAG)
	change_indent(INDENT_SET, trim_to_int(மொத்தம்), FALSE, NUL, call_changed_bytes);
    இல்லையெனில்
	(வெறுமை)set_indent(trim_to_int(மொத்தம்), call_changed_bytes ? SIN_CHANGED : 0);
}

/*
 * Shift one line of the current block one shiftwidth right or left.
 * Leaves cursor on first character in block.
 */
    நிலையான வெறுமை
shift_block(oparg_T *oap, முழு amount)
{
    முழு			இடது = (oap->op_type == OP_LSHIFT);
    முழு			oldstate = State;
    முழு			total;
    char_u		*newp, *oldp;
    size_t		newlen, oldlen;
    முழு			oldcol = curwin->w_cursor.col;
    முழு			sw_val = (முழு)get_sw_மதிப்பு_indent(curbuf, இடது);
    முழு			ts_val = (முழு)curbuf->b_p_ts;
    கட்டமைப்பு block_def	bd;
    முழு			incr;
    colnr_T		ws_vcol;
    முழு			added;
    size_t		new_வரி_len;	// the length of the line after the
					// block shift
#ifdef FEAT_RIGHTLEFT
    முழு			old_p_ri = p_ri;

    p_ri = 0;			// don't want revins in indent
#endif

    State = MODE_செருகு;	// don't want MODE_REPLACE for State
    block_prep(oap, &bd, curwin->w_cursor.lnum, TRUE);
    எனில் (bd.is_short)
	திரும்பு;

    // total is number of screen columns to be inserted/removed
    total = (முழு)((unsigned)amount * (unsigned)sw_val);
    எனில் ((total / sw_val) != amount)
	திரும்பு; // multiplication overflow

    oldp = ml_get_curline();
    oldlen = ml_get_curline_len();

    எனில் (!இடது)
    {
	முழு		tabs = 0, spaces = 0;
	chartabsize_T	cts;

	/*
	 *  1. Get start vcol
	 *  2. Total ws vcols
	 *  3. Divvy into TABs & spp
	 *  4. Construct new string
	 */
	total += bd.pre_whitesp; // all virtual WS up to & incl a split TAB
	ws_vcol = bd.start_vcol - bd.pre_whitesp;
	எனில் (bd.startspaces)
	{
	    எனில் (has_mbyte)
	    {
		எனில் ((*mb_ptr2len)(bd.textstart) == 1)
		    ++bd.textstart;
		இல்லையெனில்
		{
		    ws_vcol = 0;
		    bd.startspaces = 0;
		}
	    }
	    இல்லையெனில்
		++bd.textstart;
	}

	// TODO: is passing bd.textstart for start of the line OK?
	init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum,
				   bd.start_vcol, bd.textstart, bd.textstart);
	க்கு ( ; VIM_ISWHITE(*cts.cts_ptr); )
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
	எனில் (!curbuf->b_p_et)
	    tabstop_fromto(ws_vcol, ws_vcol + total,
				ts_val, curbuf->b_p_vts_array, &tabs, &spaces);
	இல்லையெனில்
	    spaces = total;
#இல்லையெனில்
	எனில் (!curbuf->b_p_et)
	    tabs = ((ws_vcol % ts_val) + total) / ts_val; // number of tabs
	எனில் (tabs > 0)
	    spaces = ((ws_vcol % ts_val) + total) % ts_val; // number of spp
	இல்லையெனில்
	    spaces = total;
#endif
	// if we're splitting a TAB, allow for it
	bd.textcol -= bd.pre_whitesp_c - (bd.startspaces != 0);

	new_வரி_len = bd.textcol + tabs + spaces + (oldlen - (bd.textstart - oldp));
	newp = alloc(new_வரி_len + 1);
	எனில் (newp == வெற்று)
	    திரும்பு;
	mch_memmove(newp, oldp, (size_t)bd.textcol);
	newlen = bd.textcol;
	vim_memset(newp + newlen, TAB, (size_t)tabs);
	newlen += tabs;
	vim_memset(newp + newlen, ' ', (size_t)spaces);
	STRCPY(newp + newlen + spaces, bd.textstart);
    }
    இல்லையெனில் //இடது
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
	எனில் (bd.startspaces)
	    MB_PTR_ADV(non_white);

	// The character's column is in "bd.start_vcol".
	non_white_col = bd.start_vcol;

	init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum,
				   non_white_col, bd.textstart, non_white);
	போது (VIM_ISWHITE(*cts.cts_ptr))
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
	எனில் (bd.startspaces)
	    verbatim_copy_width -= bd.start_char_vcols;
	init_chartabsize_arg(&cts, curwin, 0, verbatim_copy_width,
					     bd.textstart, verbatim_copy_end);
	போது (cts.cts_vcol < destination_col)
	{
	    incr = lbr_chartabsize(&cts);
	    எனில் (cts.cts_vcol + incr > destination_col)
		நிறுத்து;
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
	new_வரி_len = fixedlen + fill + (oldlen - (non_white - oldp));

	newp = alloc(new_வரி_len + 1);
	எனில் (newp == வெற்று)
	    திரும்பு;
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
    எனில் (new_வரி_len >= oldlen)
	added = (முழு)(new_வரி_len - oldlen);
    இல்லையெனில்
	added = 0 - (முழு)(oldlen - new_வரி_len);
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
    நிலையான வெறுமை
block_செருகு(
    oparg_T		*oap,
    char_u		*s,
    size_t		slen,
    முழு			b_செருகு,
    கட்டமைப்பு block_def	*bdp)
{
    முழு		ts_val;
    முழு		மொத்தம் = 0;	// extra spaces to replace a cut TAB
    முழு		spaces = 0;	// non-zero if cutting a TAB
    colnr_T	offset;		// pointer along new line
    colnr_T	startcol;	// column where insert starts
    char_u	*newp, *oldp;	// new, old lines
    linenr_T	lnum;		// loop var
    முழு		oldstate = State;

    State = MODE_செருகு;	// don't want MODE_REPLACE for State

    க்கு (lnum = oap->start.lnum + 1; lnum <= oap->end.lnum; lnum++)
    {
	block_prep(oap, bdp, lnum, TRUE);
	எனில் (bdp->is_short && b_செருகு)
	    தொடர்;	// OP_INSERT, line ends before block start

	oldp = ml_get(lnum);

	எனில் (b_செருகு)
	{
	    ts_val = bdp->start_char_vcols;
	    spaces = bdp->startspaces;
	    எனில் (spaces != 0)
		மொத்தம் = ts_val - 1; // we're cutting a TAB
	    offset = bdp->textcol;
	}
	இல்லையெனில் // append
	{
	    ts_val = bdp->end_char_vcols;
	    எனில் (!bdp->is_short) // spaces = padding after block
	    {
		spaces = (bdp->endspaces ? ts_val - bdp->endspaces : 0);
		எனில் (spaces != 0)
		    மொத்தம் = ts_val - 1; // we're cutting a TAB
		offset = bdp->textcol + bdp->textlen - (spaces != 0);
	    }
	    இல்லையெனில் // spaces = padding to block edge
	    {
		// if $ used, just append to EOL (ie spaces==0)
		எனில் (!bdp->is_MAX)
		    spaces = (oap->end_vcol - bdp->end_vcol) + 1;
		மொத்தம் = spaces;
		offset = bdp->textcol + bdp->textlen;
	    }
	}

	எனில் (has_mbyte && spaces > 0)
	    // avoid copying part of a multi-byte character
	    offset -= (*mb_head_off)(oldp, oldp + offset);

	எனில் (spaces < 0)  // can happen when the cursor was moved
	    spaces = 0;

	// Make sure the allocated size matches what is actually copied below.
	newp = alloc(ml_get_len(lnum) + spaces + slen
		    + (spaces > 0 && !bdp->is_short ? ts_val - spaces : 0)
								  + மொத்தம் + 1);
	எனில் (newp == வெற்று)
	    தொடர்;

	// copy up to shifted part
	mch_memmove(newp, oldp, (size_t)offset);
	oldp += offset;

	// insert pre-padding
	vim_memset(newp + offset, ' ', (size_t)spaces);
	startcol = offset + spaces;

	// copy the new text
	mch_memmove(newp + startcol, s, slen);
	offset += (முழு)slen;

	எனில் (spaces > 0 && !bdp->is_short)
	{
	    எனில் (*oldp == TAB)
	    {
		// insert post-padding
		vim_memset(newp + offset + spaces, ' ',
						    (size_t)(ts_val - spaces));
		// we're splitting a TAB, don't copy it
		oldp++;
		// We allowed for that TAB, remember this now
		மொத்தம்++;
	    }
	    இல்லையெனில்
		// Not a TAB, no extra spaces
		மொத்தம் = spaces;
	}

	எனில் (spaces > 0)
	    offset += மொத்தம்;
	STRCPY(newp + offset, oldp);

	ml_replace(lnum, newp, FALSE);

	எனில் (b_செருகு)
	    // correct any text properties
	    inserted_bytes(lnum, startcol, (முழு)slen);

	எனில் (lnum == oap->end.lnum)
	{
	    // Set "']" mark to the end of the block instead of the end of
	    // the insert in the first line.
	    curbuf->b_op_end.lnum = oap->end.lnum;
	    curbuf->b_op_end.col = offset;
	    எனில் (curbuf->b_visual.vi_end.coladd)
	    {
		curbuf->b_visual.vi_end.col += curbuf->b_visual.vi_end.coladd;
		curbuf->b_visual.vi_end.coladd = 0;
	    }
	}
    } // for all lnum

    changed_வரிகள்(oap->start.lnum + 1, 0, oap->end.lnum + 1, 0L);

    State = oldstate;
}

/*
 * Handle a delete operation.
 *
 * Return FAIL if undo failed, OK otherwise.
 */
    முழு
op_delete(oparg_T *oap)
{
    முழு			n;
    linenr_T		lnum;
    char_u		*ptr;
    char_u		*newp, *oldp;
    கட்டமைப்பு block_def	bd;
    linenr_T		old_lcount = curbuf->b_ml.ml_வரி_மொத்தம்;
    முழு			did_yank = FALSE;

    எனில் (curbuf->b_ml.ml_flags & ML_EMPTY)	    // nothing to do
	திரும்பு OK;

    // Nothing to delete, return here.	Do prepare undo, for op_change().
    எனில் (oap->empty)
	திரும்பு u_save_cursor();

    எனில் (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	திரும்பு FAIL;
    }

    எனில் (VIsual_select && oap->is_VIsual)
	// use register given with CTRL_R, defaults to zero
	oap->regname = VIsual_select_reg;

#ifdef HAVE_CLIPMETHOD
    adjust_clip_reg(&oap->regname);
#endif

    எனில் (has_mbyte)
	mb_adjust_opend(oap);

    /*
     * Imitate the strange Vi behaviour: If the delete spans more than one
     * line and motion_type == MCHAR and the result is a blank line, make the
     * delete linewise.  Don't do this for the change command or Visual mode.
     */
    எனில் (       oap->motion_type == MCHAR
	    && !oap->is_VIsual
	    && !oap->block_mode
	    && oap->வரி_மொத்தம் > 1
	    && oap->motion_force == NUL
	    && (vim_strchr(p_cpo, CPO_WORD) != வெற்று)
	    && oap->op_type == OP_DELETE)
    {
	ptr = ml_get(oap->end.lnum) + oap->end.col;
	எனில் (*ptr != NUL)
	    ptr += oap->inclusive;
	ptr = skipwhite(ptr);
	எனில் (*ptr == NUL && inindent(0))
	    oap->motion_type = MLINE;
    }

    /*
     * Check for trying to delete (e.g. "D") in an empty line.
     * Note: For the change operator it is ok.
     */
    எனில் (       oap->motion_type == MCHAR
	    && oap->வரி_மொத்தம் == 1
	    && oap->op_type == OP_DELETE
	    && *ml_get(oap->start.lnum) == NUL)
    {
	/*
	 * It's an error to operate on an empty region, when 'E' included in
	 * 'cpoptions' (Vi compatible).
	 */
	எனில் (virtual_op)
	    // Virtual editing: Nothing gets deleted, but we set the '[ and ']
	    // marks as if it happened.
	    செல் setmarks;
	எனில் (vim_strchr(p_cpo, CPO_EMPTYREGION) != வெற்று)
	    beep_flush();
	திரும்பு OK;
    }

    /*
     * Do a yank of whatever we're about to delete.
     * If a yank register was specified, put the deleted text into that
     * register.  For the black hole register '_' don't yank anything.
     */
    எனில் (oap->regname != '_')
    {
#ifdef FEAT_CLIPBOARD_PROVIDER
	inc_clip_provider();
#endif
	எனில் (oap->regname != 0)
	{
	    // check for read-only register
	    எனில் (!valid_yank_reg(oap->regname, TRUE))
	    {
		beep_flush();
#ifdef FEAT_CLIPBOARD_PROVIDER
		dec_clip_provider();
#endif
		திரும்பு OK;
	    }
	    get_yank_register(oap->regname, TRUE); // yank into specif'd reg.
	    எனில் (op_yank(oap, TRUE, FALSE) == OK)   // yank without message
		did_yank = TRUE;
	}
	இல்லையெனில்
	    reset_y_append(); // not appending to unnamed register

	/*
	 * Put deleted text into register 1 and shift number registers if the
	 * delete contains a line break, or when using a specific operator (Vi
	 * compatible)
	 */
	எனில் (oap->motion_type == MLINE || oap->வரி_மொத்தம் > 1
							   || oap->use_reg_one)
	{
	    shift_delete_registers();
	    எனில் (op_yank(oap, TRUE, FALSE) == OK)
		did_yank = TRUE;
	}

	// Yank into small delete register when no named register specified
	// and the delete is within one line.
	எனில் ((
#ifdef HAVE_CLIPMETHOD
	    ((clip_unnamed & CLIP_UNNAMED) && oap->regname == '*') ||
	    ((clip_unnamed & CLIP_UNNAMED_PLUS) && oap->regname == '+') ||
#endif
	    oap->regname == 0) && oap->motion_type != MLINE
						      && oap->வரி_மொத்தம் == 1)
	{
	    oap->regname = '-';
	    get_yank_register(oap->regname, TRUE);
	    எனில் (op_yank(oap, TRUE, FALSE) == OK)
		did_yank = TRUE;
	    oap->regname = 0;
	}

	/*
	 * If there's too much stuff to fit in the yank register, then get a
	 * confirmation before doing the delete. This is crude, but simple.
	 * And it avoids doing a delete of something we can't put back if we
	 * want.
	 */
	எனில் (!did_yank)
	{
	    முழு செய்தி_silent_save = செய்தி_silent;

	    செய்தி_silent = 0;	// must display the prompt
	    n = ask_yesno((char_u *)_("cannot yank; delete anyway"), TRUE);
	    செய்தி_silent = செய்தி_silent_save;
	    எனில் (n != 'y')
	    {
		emsg(_(e_command_aborted));
#ifdef FEAT_CLIPBOARD_PROVIDER
		dec_clip_provider();
#endif
		திரும்பு FAIL;
	    }
	}

#எனில் defined(FEAT_EVAL)
	எனில் (did_yank && has_textyankpost())
	    yank_do_autocmd(oap, get_y_current());
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
    }

    /*
     * block mode delete
     */
    எனில் (oap->block_mode)
    {
	எனில் (u_save((linenr_T)(oap->start.lnum - 1),
			       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	    திரும்பு FAIL;

	க்கு (lnum = curwin->w_cursor.lnum; lnum <= oap->end.lnum; ++lnum)
	{
	    block_prep(oap, &bd, lnum, TRUE);
	    எனில் (bd.textlen == 0)	// nothing to delete
		தொடர்;

	    // Adjust cursor position for tab replaced by spaces and 'lbr'.
	    எனில் (lnum == curwin->w_cursor.lnum)
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
	    எனில் (newp == வெற்று)
		தொடர்;
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
	    எனில் (curbuf->b_has_textprop && n != 0)
		adjust_prop_columns(lnum, bd.textcol, -n, 0);
#endif
	}

	check_cursor_col();
	changed_வரிகள்(curwin->w_cursor.lnum, curwin->w_cursor.col,
						       oap->end.lnum + 1, 0L);
	oap->வரி_மொத்தம் = 0;	    // no lines deleted
    }
    இல்லையெனில் எனில் (oap->motion_type == MLINE)
    {
	எனில் (oap->op_type == OP_CHANGE)
	{
	    // Delete the lines except the first one.  Temporarily move the
	    // cursor to the next line.  Save the current line number, if the
	    // last line is deleted it may be changed.
	    எனில் (oap->வரி_மொத்தம் > 1)
	    {
		lnum = curwin->w_cursor.lnum;
		++curwin->w_cursor.lnum;
		del_வரிகள்((long)(oap->வரி_மொத்தம் - 1), TRUE);
		curwin->w_cursor.lnum = lnum;
	    }
	    எனில் (u_save_cursor() == FAIL)
		திரும்பு FAIL;
	    எனில் (curbuf->b_p_ai)		    // don't delete indent
	    {
		beginline(BL_WHITE);	    // cursor on first non-white
		did_ai = TRUE;		    // delete the indent when ESC hit
		ai_col = curwin->w_cursor.col;
	    }
	    இல்லையெனில்
		beginline(0);		    // cursor in column 0
	    truncate_வரி(FALSE);   // delete the rest of the line,
				    // leaving cursor past last char in line
	    எனில் (oap->வரி_மொத்தம் > 1)
		u_clearline();	    // "U" command not possible after "2cc"
	}
	இல்லையெனில்
	{
	    del_வரிகள்(oap->வரி_மொத்தம், TRUE);
	    beginline(BL_WHITE | BL_FIX);
	    u_clearline();	// "U" command not possible after "dd"
	}
    }
    இல்லையெனில்
    {
	எனில் (virtual_op)
	{
	    முழு		endcol = 0;

	    // For virtualedit: break the tabs that are partly included.
	    எனில் (gchar_pos(&oap->start) == '\t')
	    {
		எனில் (u_save_cursor() == FAIL)	// save first line for undo
		    திரும்பு FAIL;
		எனில் (oap->வரி_மொத்தம் == 1)
		    endcol = getviscol2(oap->end.col, oap->end.coladd);
		coladvance_force(getviscol2(oap->start.col, oap->start.coladd));
		oap->start = curwin->w_cursor;
		எனில் (oap->வரி_மொத்தம் == 1)
		{
		    coladvance(endcol);
		    oap->end.col = curwin->w_cursor.col;
		    oap->end.coladd = curwin->w_cursor.coladd;
		    curwin->w_cursor = oap->start;
		}
	    }

	    // Break a tab only when it's included in the area.
	    எனில் (gchar_pos(&oap->end) == '\t'
				     && (முழு)oap->end.coladd < oap->inclusive)
	    {
		// save last line for undo
		எனில் (u_save((linenr_T)(oap->end.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
		    திரும்பு FAIL;
		curwin->w_cursor = oap->end;
		coladvance_force(getviscol2(oap->end.col, oap->end.coladd));
		oap->end = curwin->w_cursor;
		curwin->w_cursor = oap->start;
	    }
	    எனில் (has_mbyte)
		mb_adjust_opend(oap);
	}

	எனில் (oap->வரி_மொத்தம் == 1)	// delete characters within one line
	{
	    எனில் (u_save_cursor() == FAIL)	// save line for undo
		திரும்பு FAIL;

	    // if 'cpoptions' contains '$', display '$' at end of change
	    எனில் (       vim_strchr(p_cpo, CPO_DOLLAR) != வெற்று
		    && oap->op_type == OP_CHANGE
		    && oap->end.lnum == curwin->w_cursor.lnum
		    && !oap->is_VIsual)
		display_dollar(oap->end.col - !oap->inclusive);

	    n = oap->end.col - oap->start.col + 1 - !oap->inclusive;

	    எனில் (virtual_op)
	    {
		// fix up things for virtualedit-delete:
		// break the tabs which are going to get in our way
		முழு		len = ml_get_curline_len();

		எனில் (oap->end.coladd != 0
			&& (முழு)oap->end.col >= len - 1
			&& !(oap->start.coladd && (முழு)oap->end.col >= len - 1))
		    n++;
		// Delete at least one char (e.g, when on a control char).
		எனில் (n == 0 && oap->start.coladd != oap->end.coladd)
		    n = 1;

		// When deleted a char in the line, reset coladd.
		எனில் (gchar_cursor() != NUL)
		    curwin->w_cursor.coladd = 0;
	    }
	    (வெறுமை)del_bytes((long)n, !virtual_op,
			    oap->op_type == OP_DELETE && !oap->is_VIsual);
	}
	இல்லையெனில்				// delete characters between lines
	{
	    pos_T   curpos;

	    // save deleted and changed lines for undo
	    எனில் (u_save((linenr_T)(curwin->w_cursor.lnum - 1),
		 (linenr_T)(curwin->w_cursor.lnum + oap->வரி_மொத்தம்)) == FAIL)
		திரும்பு FAIL;

	    truncate_வரி(TRUE);	// delete from cursor to end of line

	    curpos = curwin->w_cursor;	// remember curwin->w_cursor
	    ++curwin->w_cursor.lnum;
	    del_வரிகள்((long)(oap->வரி_மொத்தம் - 2), FALSE);

	    // delete from start of line until op_end
	    n = (oap->end.col + 1 - !oap->inclusive);
	    curwin->w_cursor.col = 0;
	    (வெறுமை)del_bytes((long)n, !virtual_op,
			    oap->op_type == OP_DELETE && !oap->is_VIsual);
	    curwin->w_cursor = curpos;	// restore curwin->w_cursor
	    (வெறுமை)do_join(2, FALSE, FALSE, FALSE, FALSE);
	}
	எனில் (oap->op_type == OP_DELETE)
	    auto_format(FALSE, TRUE);
    }

    msgmore(curbuf->b_ml.ml_வரி_மொத்தம் - old_lcount);

setmarks:
    எனில் ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	எனில் (oap->block_mode)
	{
	    curbuf->b_op_end.lnum = oap->end.lnum;
	    curbuf->b_op_end.col = oap->start.col;
	}
	இல்லையெனில்
	    curbuf->b_op_end = oap->start;
	curbuf->b_op_start = oap->start;
    }

    திரும்பு OK;
}

/*
 * Adjust end of operating area for ending on a multi-byte character.
 * Used for deletion.
 */
    நிலையான வெறுமை
mb_adjust_opend(oparg_T *oap)
{
    char_u	*வரி;
    char_u	*ptr;

    எனில் (!oap->inclusive)
	திரும்பு;

    வரி = ml_get(oap->end.lnum);
    ptr = வரி + oap->end.col;
    எனில் (*ptr != NUL)
    {
	ptr -= (*mb_head_off)(வரி, ptr);
	ptr += (*mb_ptr2len)(ptr) - 1;
	oap->end.col = ptr - வரி;
    }
}

/*
 * Replace the character under the cursor with "c".
 * This takes care of multi-byte characters.
 */
    நிலையான வெறுமை
replace_character(முழு c)
{
    முழு n = State;

    State = MODE_REPLACE;
    ins_char(c);
    State = n;
    // Backup to the replaced character.
    dec_cursor();
}

/*
 * Replace a whole area with one character.
 */
    முழு
op_replace(oparg_T *oap, முழு c)
{
    முழு			n, numc;
    முழு			இலக்கம்_chars;
    char_u		*newp, *oldp;
    size_t		newlen, oldlen;
    கட்டமைப்பு block_def	bd;
    char_u		*after_p = வெற்று;
    முழு			had_ctrl_v_cr = FALSE;

    எனில் ((curbuf->b_ml.ml_flags & ML_EMPTY ) || oap->empty)
	திரும்பு OK;	    // nothing to do

    எனில் (c == REPLACE_CR_NCHAR)
    {
	had_ctrl_v_cr = TRUE;
	c = CAR;
    }
    இல்லையெனில் எனில் (c == REPLACE_NL_NCHAR)
    {
	had_ctrl_v_cr = TRUE;
	c = NL;
    }

    எனில் (has_mbyte)
	mb_adjust_opend(oap);

    எனில் (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	திரும்பு FAIL;

    /*
     * block mode replace
     */
    எனில் (oap->block_mode)
    {
	bd.is_MAX = (curwin->w_curswant == MAXCOL);
	க்கு ( ; curwin->w_cursor.lnum <= oap->end.lnum; ++curwin->w_cursor.lnum)
	{
	    curwin->w_cursor.col = 0;  // make sure cursor position is valid
	    block_prep(oap, &bd, curwin->w_cursor.lnum, TRUE);
	    எனில் (bd.textlen == 0 && (!virtual_op || bd.is_MAX))
		தொடர்;	    // nothing to replace

	    // n == number of extra chars required
	    // If we split a TAB, it may be replaced by several characters.
	    // Thus the number of characters may increase!
	    // If the range starts in virtual space, count the initial
	    // coladd offset as part of "startspaces"
	    எனில் (virtual_op && bd.is_short && *bd.textstart == NUL)
	    {
		pos_T vpos;

		vpos.lnum = curwin->w_cursor.lnum;
		getvpos(&vpos, oap->start_vcol);
		bd.startspaces += vpos.coladd;
		n = bd.startspaces;
	    }
	    இல்லையெனில்
		// allow for pre spaces
		n = (bd.startspaces ? bd.start_char_vcols - 1 : 0);

	    // allow for post spp
	    n += (bd.endspaces
		    && !bd.is_oneChar
		    && bd.end_char_vcols > 0) ? bd.end_char_vcols - 1 : 0;
	    // Figure out how many characters to replace.
	    numc = oap->end_vcol - oap->start_vcol + 1;
	    எனில் (bd.is_short && (!virtual_op || bd.is_MAX))
		numc -= (oap->end_vcol - bd.end_vcol) + 1;

	    // A double-wide character can be replaced only up to half the
	    // times.
	    எனில் ((*mb_char2cells)(c) > 1)
	    {
		எனில் ((numc & 1) && !bd.is_short)
		{
		    ++bd.endspaces;
		    ++n;
		}
		numc = numc / 2;
	    }

	    // Compute bytes needed, move character count to num_chars.
	    இலக்கம்_chars = numc;
	    numc *= (*mb_char2len)(c);
	    // oldlen includes textlen, so don't double count
	    n += numc - bd.textlen;

	    oldp = ml_get_curline();
	    oldlen = ml_get_curline_len();
	    newp = alloc(oldlen + 1 + n);
	    எனில் (newp == வெற்று)
		தொடர்;
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
	    எனில் (had_ctrl_v_cr || (c != '\r' && c != '\n'))
	    {
		எனில் (has_mbyte)
		{
		    போது (--இலக்கம்_chars >= 0)
			newlen += (*mb_char2bytes)(c, newp + newlen);
		}
		இல்லையெனில்
		{
		    vim_memset(newp + newlen, c, (size_t)numc);
		    newlen += numc;
		}
		எனில் (!bd.is_short)
		{
		    // insert post-spaces
		    vim_memset(newp + newlen, ' ', (size_t)bd.endspaces);
		    // copy the part after the changed part
		    STRCPY(newp + newlen + bd.endspaces,
				oldp + bd.textcol + bd.textlen);
		}
	    }
	    இல்லையெனில்
	    {
		// Replacing with \r or \n means splitting the line.
		after_p = alloc(oldlen + 1 + n - newlen);
		எனில் (after_p != வெற்று)
		    STRCPY(after_p, oldp + bd.textcol + bd.textlen);
	    }

	    // replace the line
	    ml_replace(curwin->w_cursor.lnum, newp, FALSE);
	    எனில் (after_p != வெற்று)
	    {
		ml_append(curwin->w_cursor.lnum++, after_p, 0, FALSE);
		appended_வரிகள்_mark(curwin->w_cursor.lnum, 1L);
		oap->end.lnum++;
		vim_free(after_p);
	    }
	}
    }
    இல்லையெனில்
    {
	/*
	 * MCHAR and MLINE motion replace.
	 */
	எனில் (oap->motion_type == MLINE)
	{
	    oap->start.col = 0;
	    curwin->w_cursor.col = 0;
	    oap->end.col = ml_get_len(oap->end.lnum);
	    எனில் (oap->end.col)
		--oap->end.col;
	}
	இல்லையெனில் எனில் (!oap->inclusive)
	    dec(&(oap->end));

	போது (LTOREQ_POS(curwin->w_cursor, oap->end))
	{
	    முழு done = FALSE;

	    n = gchar_cursor();
	    எனில் (n != NUL)
	    {
		முழு new_byte_len = (*mb_char2len)(c);
		முழு old_byte_len = mb_ptr2len(ml_get_cursor());

		எனில் (new_byte_len > 1 || old_byte_len > 1)
		{
		    // This is slow, but it handles replacing a single-byte
		    // with a multi-byte and the other way around.
		    எனில் (curwin->w_cursor.lnum == oap->end.lnum)
			oap->end.col += new_byte_len - old_byte_len;
		    replace_character(c);
		    done = TRUE;
		}
		இல்லையெனில்
		{
		    எனில் (n == TAB)
		    {
			முழு end_vcol = 0;

			எனில் (curwin->w_cursor.lnum == oap->end.lnum)
			{
			    // oap->end has to be recalculated when
			    // the tab breaks
			    end_vcol = getviscol2(oap->end.col,
							     oap->end.coladd);
			}
			coladvance_force(getviscol());
			எனில் (curwin->w_cursor.lnum == oap->end.lnum)
			    getvpos(&oap->end, end_vcol);
		    }
		    // with "coladd" set may move to just after a TAB
		    எனில் (gchar_cursor() != NUL)
		    {
			PBYTE(curwin->w_cursor, c);
			done = TRUE;
		    }
		}
	    }
	    எனில் (!done && virtual_op && curwin->w_cursor.lnum == oap->end.lnum)
	    {
		முழு virtcols = oap->end.coladd;

		எனில் (curwin->w_cursor.lnum == oap->start.lnum
			&& oap->start.col == oap->end.col && oap->start.coladd)
		    virtcols -= oap->start.coladd;

		// oap->end has been trimmed so it's effectively inclusive;
		// as a result an extra +1 must be counted so we don't
		// trample the NUL byte.
		coladvance_force(getviscol2(oap->end.col, oap->end.coladd) + 1);
		curwin->w_cursor.col -= (virtcols + 1);
		க்கு (; virtcols >= 0; virtcols--)
		{
		    எனில் ((*mb_char2len)(c) > 1)
		       replace_character(c);
		    இல்லையெனில்
			PBYTE(curwin->w_cursor, c);
		    எனில் (inc(&curwin->w_cursor) == -1)
			நிறுத்து;
		}
	    }

	    // Advance to next character, stop at the end of the file.
	    எனில் (inc_cursor() == -1)
		நிறுத்து;
	}
    }

    curwin->w_cursor = oap->start;
    check_cursor();
    changed_வரிகள்(oap->start.lnum, oap->start.col, oap->end.lnum + 1, 0L);

    எனில் ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
    }

    திரும்பு OK;
}

நிலையான முழு swapchars(முழு op_type, pos_T *pos, முழு length);

/*
 * Handle the (non-standard vi) tilde operator.  Also for "gu", "gU" and "g?".
 */
    நிலையான வெறுமை
op_tilde(oparg_T *oap)
{
    pos_T		pos;
    கட்டமைப்பு block_def	bd;
    முழு			did_change = FALSE;

    எனில் (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	திரும்பு;

    pos = oap->start;
    எனில் (oap->block_mode)		    // Visual block mode
    {
	க்கு (; pos.lnum <= oap->end.lnum; ++pos.lnum)
	{
	    முழு one_change;

	    block_prep(oap, &bd, pos.lnum, FALSE);
	    pos.col = bd.textcol;
	    one_change = swapchars(oap->op_type, &pos, bd.textlen);
	    did_change |= one_change;

#ifdef FEAT_NETBEANS_INTG
	    எனில் (netbeans_active() && one_change)
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
	எனில் (did_change)
	    changed_வரிகள்(oap->start.lnum, 0, oap->end.lnum + 1, 0L);
    }
    இல்லையெனில்				    // not block mode
    {
	எனில் (oap->motion_type == MLINE)
	{
	    oap->start.col = 0;
	    pos.col = 0;
	    oap->end.col = ml_get_len(oap->end.lnum);
	    எனில் (oap->end.col)
		--oap->end.col;
	}
	இல்லையெனில் எனில் (!oap->inclusive)
	    dec(&(oap->end));

	எனில் (pos.lnum == oap->end.lnum)
	    did_change = swapchars(oap->op_type, &pos,
						  oap->end.col - pos.col + 1);
	இல்லையெனில்
	    க்கு (;;)
	    {
		did_change |= swapchars(oap->op_type, &pos,
			pos.lnum == oap->end.lnum ? oap->end.col + 1
						  : ml_get_pos_len(&pos));
		எனில் (LTOREQ_POS(oap->end, pos) || inc(&pos) == -1)
		    நிறுத்து;
	    }
	எனில் (did_change)
	{
	    changed_வரிகள்(oap->start.lnum, oap->start.col, oap->end.lnum + 1,
									  0L);
#ifdef FEAT_NETBEANS_INTG
	    எனில் (netbeans_active())
	    {
		char_u *ptr;
		முழு மொத்தம்;

		pos = oap->start;
		போது (pos.lnum < oap->end.lnum)
		{
		    ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		    மொத்தம் = ml_get_buf_len(curbuf, pos.lnum) - pos.col;
		    netbeans_removed(curbuf, pos.lnum, pos.col, (long)மொத்தம்);
		    // get the line again, it may have been flushed
		    ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		    netbeans_inserted(curbuf, pos.lnum, pos.col,
							&ptr[pos.col], மொத்தம்);
		    pos.col = 0;
		    pos.lnum++;
		}
		மொத்தம் = oap->end.col - pos.col + 1;
		netbeans_removed(curbuf, pos.lnum, pos.col, (long)மொத்தம்);
		// get the line again, it may have been flushed
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, pos.col,
							&ptr[pos.col], மொத்தம்);
	    }
#endif
	}
    }

    எனில் (!did_change && oap->is_VIsual)
	// No change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    எனில் ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set '[ and '] marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
    }

    எனில் (oap->வரி_மொத்தம் > p_report)
	smsg(NGETTEXT("%ld line changed", "%ld lines changed",
					    oap->வரி_மொத்தம்), oap->வரி_மொத்தம்);
}

/*
 * Invoke swapchar() on "length" bytes at position "pos".
 * "pos" is advanced to just after the changed characters.
 * "length" is rounded up to include the whole last multi-byte character.
 * Also works correctly when the number of bytes changes.
 * Returns TRUE if some character was changed.
 */
    நிலையான முழு
swapchars(முழு op_type, pos_T *pos, முழு length)
{
    முழு todo;
    முழு	did_change = 0;

    க்கு (todo = length; todo > 0; --todo)
    {
	எனில் (has_mbyte)
	{
	    முழு len = (*mb_ptr2len)(ml_get_pos(pos));

	    // we're counting bytes, not characters
	    எனில் (len > 0)
		todo -= len - 1;
	}
	did_change |= swapchar(op_type, pos);
	எனில் (inc(pos) == -1)    // at end of file
	    நிறுத்து;
    }
    திரும்பு did_change;
}

/*
 * If op_type == OP_UPPER: make uppercase,
 * if op_type == OP_LOWER: make lowercase,
 * if op_type == OP_ROT13: do rot13 encoding,
 * else swap case of character at 'pos'
 * returns TRUE when something actually changed.
 */
    முழு
swapchar(முழு op_type, pos_T *pos)
{
    முழு	    c;
    முழு	    nc;

    c = gchar_pos(pos);

    // Only do rot13 encoding for ASCII characters.
    எனில் (c >= 0x80 && op_type == OP_ROT13)
	திரும்பு FALSE;

    // ~ is OP_NOP, g~ is OP_TILDE, gU is OP_UPPER
    எனில் ((op_type == OP_UPPER || op_type == OP_NOP || op_type == OP_TILDE)
	    && c == 0xdf
	    && (enc_latin1like || STRCMP(p_enc, "iso-8859-2") == 0))
    {
	pos_T   sp = curwin->w_cursor;

	// Special handling for lowercase German sharp s (ß): convert to uppercase (ẞ).
	curwin->w_cursor = *pos;
	del_char(FALSE);
	ins_char(0x1E9E);
	curwin->w_cursor = sp;
	திரும்பு TRUE;
    }

    எனில் (enc_dbcs != 0 && c >= 0x100)	// No lower/uppercase letter
	திரும்பு FALSE;
    nc = c;
    எனில் (MB_ISLOWER(c))
    {
	எனில் (op_type == OP_ROT13)
	    nc = ROT13(c, 'a');
	இல்லையெனில் எனில் (op_type != OP_LOWER)
	    nc = MB_TOUPPER(c);
    }
    இல்லையெனில் எனில் (MB_ISUPPER(c))
    {
	எனில் (op_type == OP_ROT13)
	    nc = ROT13(c, 'A');
	இல்லையெனில் எனில் (op_type != OP_UPPER)
	    nc = MB_TOLOWER(c);
    }
    எனில் (nc != c)
    {
	எனில் (enc_utf8 && (c >= 0x80 || nc >= 0x80))
	{
	    pos_T   sp = curwin->w_cursor;

	    curwin->w_cursor = *pos;
	    // don't use del_char(), it also removes composing chars
	    del_bytes(utf_ptr2len(ml_get_cursor()), FALSE, FALSE);
	    ins_char(nc);
	    curwin->w_cursor = sp;
	}
	இல்லையெனில்
	    PBYTE(*pos, nc);
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

/*
 * op_insert - Insert and append operators for Visual mode.
 */
    வெறுமை
op_செருகு(oparg_T *oap, long count1)
{
    long		pre_textlen = 0;
    colnr_T		ind_pre_col = 0, ind_post_col;
    முழு			ind_pre_vcol = 0, ind_post_vcol = 0;
    கட்டமைப்பு block_def	bd;
    முழு			i;
    pos_T		t1;
    pos_T		start_செருகு;

    // edit() changes this - record it for OP_APPEND
    bd.is_MAX = (curwin->w_curswant == MAXCOL);

    // vis block is still marked. Get rid of it now.
    curwin->w_cursor.lnum = oap->start.lnum;
    update_screen(UPD_INVERTED);

    எனில் (oap->block_mode)
    {
	// When 'virtualedit' is used, need to insert the extra spaces before
	// doing block_prep().  When only "block" is used, virtual edit is
	// already disabled, but still need it when calling
	// coladvance_force().
	// coladvance_force() uses get_ve_flags() to get the 'virtualedit'
	// state for the current window.  To override that state, we need to
	// set the window-local value of ve_flags rather than the global value.
	எனில் (curwin->w_cursor.coladd > 0)
	{
	    முழு		old_ve_flags = curwin->w_ve_flags;

	    எனில் (u_save_cursor() == FAIL)
		திரும்பு;

	    curwin->w_ve_flags = VE_ALL;
	    coladvance_force(oap->op_type == OP_APPEND
					   ? oap->end_vcol + 1 : getviscol());
	    எனில் (oap->op_type == OP_APPEND)
		--curwin->w_cursor.col;
	    curwin->w_ve_flags = old_ve_flags;
	}
	// Get the info about the block before entering the text
	block_prep(oap, &bd, oap->start.lnum, TRUE);
	// Get indent information
	ind_pre_col = (colnr_T)getwhitecols_curline();
	ind_pre_vcol = get_indent();
	pre_textlen = ml_get_len(oap->start.lnum) - bd.textcol;
	எனில் (oap->op_type == OP_APPEND)
	    pre_textlen -= bd.textlen;
    }

    எனில் (oap->op_type == OP_APPEND)
    {
	எனில் (oap->block_mode && curwin->w_cursor.coladd == 0)
	{
	    // Move the cursor to the character right of the block.
	    curwin->w_set_curswant = true;
	    போது (*ml_get_cursor() != NUL
		    && (curwin->w_cursor.col < bd.textcol + bd.textlen))
		++curwin->w_cursor.col;
	    எனில் (bd.is_short && !bd.is_MAX)
	    {
		// First line was too short, make it longer and adjust the
		// values in "bd".
		எனில் (u_save_cursor() == FAIL)
		    திரும்பு;
		க்கு (i = 0; i < bd.endspaces; ++i)
		    ins_char(' ');
		bd.textlen += bd.endspaces;
	    }
	}
	இல்லையெனில்
	{
	    curwin->w_cursor = oap->end;
	    check_cursor_col();

	    // Works just like an 'i'nsert on the next character.
	    எனில் (!LINEEMPTY(curwin->w_cursor.lnum)
		    && oap->start_vcol != oap->end_vcol)
		inc_cursor();
	}
    }

    t1 = oap->start;
    start_செருகு = curwin->w_cursor;
    (வெறுமை)edit(NUL, FALSE, (linenr_T)count1);

    // When a tab was inserted, and the characters in front of the tab
    // have been converted to a tab as well, the column of the cursor
    // might have actually been reduced, so need to adjust here.
    எனில் (t1.lnum == curbuf->b_op_start_orig.lnum
	    && LT_POS(curbuf->b_op_start_orig, t1))
	oap->start = curbuf->b_op_start_orig;

    // If user has moved off this line, we don't know what to do, so do
    // nothing.
    // Also don't repeat the insert when Insert mode ended with CTRL-C.
    எனில் (curwin->w_cursor.lnum != oap->start.lnum || got_int)
	திரும்பு;

    எனில் (oap->block_mode)
    {
	முழு			ins_len;
	char_u			*firstline, *ins_text;
	கட்டமைப்பு block_def	bd2;
	முழு			did_indent = FALSE;
	size_t			len;
	size_t			add;
	// offset when cursor was moved in insert mode
	முழு			offset = 0;

	// If indent kicked in, the firstline might have changed
	// but only do that, if the indent actually increased.
	ind_post_col = (colnr_T)getwhitecols_curline();
	எனில் (curbuf->b_op_start.col > ind_pre_col && ind_post_col > ind_pre_col)
	{
	    bd.textcol += ind_post_col - ind_pre_col;
	    ind_post_vcol = get_indent();
	    bd.start_vcol += ind_post_vcol - ind_pre_vcol;
	    did_indent = TRUE;
	}

	// The user may have moved the cursor before inserting something, try
	// to adjust the block for that.  But only do it, if the difference
	// does not come from indent kicking in.
	எனில் (oap->start.lnum == curbuf->b_op_start_orig.lnum
						  && !bd.is_MAX && !did_indent)
	{
	    முழு t = getviscol2(curbuf->b_op_start_orig.col,
					       curbuf->b_op_start_orig.coladd);

	    எனில் (oap->op_type == OP_செருகு
		    && oap->start.col + oap->start.coladd
			    != curbuf->b_op_start_orig.col
					      + curbuf->b_op_start_orig.coladd)
	    {
		oap->start.col = curbuf->b_op_start_orig.col;
		pre_textlen -= t - oap->start_vcol;
		oap->start_vcol = t;
	    }
	    இல்லையெனில் எனில் (oap->op_type == OP_APPEND
		    && oap->start.col + oap->start.coladd
			    >= curbuf->b_op_start_orig.col
					      + curbuf->b_op_start_orig.coladd)
	    {
		oap->start.col = curbuf->b_op_start_orig.col;
		// reset pre_textlen to the value of OP_INSERT
		pre_textlen += bd.textlen;
		pre_textlen -= t - oap->start_vcol;
		oap->start_vcol = t;
		oap->op_type = OP_செருகு;
	    }
	}

	// Spaces and tabs in the indent may have changed to other spaces and
	// tabs.  Get the starting column again and correct the length.
	// Don't do this when "$" used, end-of-line will have changed.
	//
	// if indent was added and the inserted text was after the indent,
	// correct the selection for the new indent.
	எனில் (did_indent && bd.textcol - ind_post_col > 0)
	{
	    oap->start.col += ind_post_col - ind_pre_col;
	    oap->start_vcol += ind_post_vcol - ind_pre_vcol;
	    oap->end.col += ind_post_col - ind_pre_col;
	    oap->end_vcol += ind_post_vcol - ind_pre_vcol;
	}
	block_prep(oap, &bd2, oap->start.lnum, TRUE);
	எனில் (did_indent && bd.textcol - ind_post_col > 0)
	{
	    // undo for where "oap" is used below
	    oap->start.col -= ind_post_col - ind_pre_col;
	    oap->start_vcol -= ind_post_vcol - ind_pre_vcol;
	    oap->end.col -= ind_post_col - ind_pre_col;
	    oap->end_vcol -= ind_post_vcol - ind_pre_vcol;
	}
	எனில் (!bd.is_MAX || bd2.textlen < bd.textlen)
	{
	    எனில் (oap->op_type == OP_APPEND)
	    {
		pre_textlen += bd2.textlen - bd.textlen;
		எனில் (bd2.endspaces)
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
	எனில் (oap->op_type == OP_APPEND)
	{
	    add += bd.textlen;
	    // account for pressing cursor in insert mode when '$' was used
	    எனில் (bd.is_MAX
		&& (start_செருகு.lnum == Insstart.lnum
					   && start_செருகு.col > Insstart.col))
	    {
		offset = (start_செருகு.col - Insstart.col);
		add -= offset;
		எனில் (oap->end_vcol > offset)
		    oap->end_vcol -= (offset + 1);
		இல்லையெனில்
		    // moved outside of the visual block, what to do?
		    திரும்பு;
	    }
	}
	எனில் (add > len)
	    add = len;  // short line, point to the NUL
	firstline += add;
	len -= add;
	எனில் (pre_textlen >= 0 && (ins_len = (முழு)len - pre_textlen - offset) > 0)
	{
	    ins_text = vim_strnsave(firstline, ins_len);
	    எனில் (ins_text != வெற்று)
	    {
		// block handled here
		எனில் (u_save(oap->start.lnum,
					 (linenr_T)(oap->end.lnum + 1)) == OK)
		    block_செருகு(oap, ins_text, ins_len, (oap->op_type == OP_செருகு),
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
    முழு
op_change(oparg_T *oap)
{
    colnr_T		l;
    முழு			retval;
    linenr_T		linenr;
    long		pre_textlen = 0;
    long		pre_indent = 0;
    char_u		*firstline;
    char_u		*ins_text, *newp, *oldp;
    கட்டமைப்பு block_def	bd;

    l = oap->start.col;
    எனில் (oap->motion_type == MLINE)
    {
	l = 0;
	can_si = may_do_si();	// Like opening a new line, do smart indent
    }

    // First delete the text in the region.  In an empty buffer only need to
    // save for undo
    எனில் (curbuf->b_ml.ml_flags & ML_EMPTY)
    {
	எனில் (u_save_cursor() == FAIL)
	    திரும்பு FALSE;
    }
    இல்லையெனில் எனில் (op_delete(oap) == FAIL)
	திரும்பு FALSE;

    எனில் ((l > curwin->w_cursor.col) && !LINEEMPTY(curwin->w_cursor.lnum)
							 && !virtual_op)
	inc_cursor();

    // check for still on same line (<CR> in inserted text meaningless)
    // skip blank lines too
    எனில் (oap->block_mode)
    {
	// Add spaces before getting the current line length.
	எனில் (virtual_op && (curwin->w_cursor.coladd > 0
						    || gchar_cursor() == NUL))
	    coladvance_force(getviscol());
	firstline = ml_get(oap->start.lnum);
	pre_textlen = ml_get_len(oap->start.lnum);
	pre_indent = (long)getwhitecols(firstline);
	bd.textcol = curwin->w_cursor.col;
    }

    எனில் (oap->motion_type == MLINE)
	fix_indent();

    // Reset finish_op now, don't want it set inside edit().
    முழு save_finish_op = finish_op;
    finish_op = FALSE;

    retval = edit(NUL, FALSE, (linenr_T)1);

    finish_op = save_finish_op;

    /*
     * In Visual block mode, handle copying the new text to all lines of the
     * block.
     * Don't repeat the insert when Insert mode ended with CTRL-C.
     */
    எனில் (oap->block_mode && oap->start.lnum != oap->end.lnum && !got_int)
    {
	முழு	ins_len;

	// Auto-indenting may have changed the indent.  If the cursor was past
	// the indent, exclude that indent change from the inserted text.
	firstline = ml_get(oap->start.lnum);
	எனில் (bd.textcol > (colnr_T)pre_indent)
	{
	    long new_indent = (long)getwhitecols(firstline);

	    pre_textlen += new_indent - pre_indent;
	    bd.textcol += new_indent - pre_indent;
	}

	ins_len = (முழு)ml_get_len(oap->start.lnum) - pre_textlen;
	எனில் (ins_len > 0)
	{
	    // Subsequent calls to ml_get() flush the firstline data - take a
	    // copy of the inserted text.
	    எனில் ((ins_text = alloc(ins_len + 1)) != வெற்று)
	    {
		vim_strncpy(ins_text, firstline + bd.textcol, ins_len);
		க்கு (linenr = oap->start.lnum + 1; linenr <= oap->end.lnum;
								     linenr++)
		{
		    block_prep(oap, &bd, linenr, TRUE);
		    எனில் (!bd.is_short || virtual_op)
		    {
			pos_T vpos;
			size_t newlen;

			// If the block starts in virtual space, count the
			// initial coladd offset as part of "startspaces"
			எனில் (bd.is_short)
			{
			    vpos.lnum = linenr;
			    (வெறுமை)getvpos(&vpos, oap->start_vcol);
			}
			இல்லையெனில்
			    vpos.coladd = 0;
			oldp = ml_get(linenr);
			newp = alloc(ml_get_len(linenr) + vpos.coladd + ins_len + 1);
			எனில் (newp == வெற்று)
			    தொடர்;
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
			எனில் (curbuf->b_has_textprop)
			    adjust_prop_columns(linenr, bd.textcol,
						     vpos.coladd + (முழு)ins_len, 0);
#endif
		    }
		}
		check_cursor();

		changed_வரிகள்(oap->start.lnum + 1, 0, oap->end.lnum + 1, 0L);
	    }
	    vim_free(ins_text);
	}
    }
    auto_format(FALSE, TRUE);

    திரும்பு retval;
}

/*
 * When the cursor is on the NUL past the end of the line and it should not be
 * there move it left.
 */
    வெறுமை
adjust_cursor_eol(வெறுமை)
{
    unsigned முழு cur_ve_flags = get_ve_flags();

    முழு adj_cursor = (curwin->w_cursor.col > 0
				&& gchar_cursor() == NUL
				&& (cur_ve_flags & VE_ONEMORE) == 0
				&& (cur_ve_flags & VE_ALL) == 0
				&& !(restart_edit || (State & MODE_செருகு)));
    எனில் (!adj_cursor)
	திரும்பு;

    // Put the cursor on the last character in the line.
    dec_cursor();

    எனில் (cur_ve_flags == VE_ALL)
    {
	colnr_T	    scol, ecol;

	// Coladd is set to the width of the last character.
	getvcol(curwin, &curwin->w_cursor, &scol, வெற்று, &ecol, 0);
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
    char_u   *வரி,
    முழு      process,
    முழு	     include_space,
    முழு      *is_comment)
{
    char_u *comment_flags = வெற்று;
    முழு    lead_len;
    முழு    leader_offset = get_last_leader_offset(வரி, &comment_flags);

    *is_comment = FALSE;
    எனில் (leader_offset != -1)
    {
	// Let's check whether the line ends with an unclosed comment.
	// If the last comment leader has COM_END in flags, there's no comment.
	போது (*comment_flags)
	{
	    எனில் (*comment_flags == COM_END
		    || *comment_flags == ':')
		நிறுத்து;
	    ++comment_flags;
	}
	எனில் (*comment_flags != COM_END)
	    *is_comment = TRUE;
    }

    எனில் (process == FALSE)
	திரும்பு வரி;

    lead_len = get_leader_len(வரி, &comment_flags, FALSE, include_space);

    எனில் (lead_len == 0)
	திரும்பு வரி;

    // Find:
    // - COM_END,
    // - colon,
    // whichever comes first.
    போது (*comment_flags)
    {
	எனில் (*comment_flags == COM_END
		|| *comment_flags == ':')
	    நிறுத்து;
	++comment_flags;
    }

    // If we found a colon, it means that we are not processing a line
    // starting with a closing part of a three-part comment. That's good,
    // because we don't want to remove those as this would be annoying.
    எனில் (*comment_flags == ':' || *comment_flags == NUL)
	வரி += lead_len;

    திரும்பு வரி;
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
    முழு
do_join(
    long    மொத்தம்,
    முழு	    செருகு_space,
    முழு	    save_undo,
    முழு	    use_formatoptions UNUSED,
    முழு	    setmark)
{
    char_u	*curr = வெற்று;
    char_u      *curr_start = வெற்று;
    char_u	*cend;
    char_u	*newp;
    size_t	newp_len;
    char_u	*spaces;	// number of spaces inserted before a line
    முழு		endcurr1 = NUL;
    முழு		endcurr2 = NUL;
    முழு		currsize = 0;	// size of the current line
    முழு		sumsize = 0;	// size of the long new line
    linenr_T	t;
    colnr_T	col = 0;
    முழு		ret = OK;
    முழு		*comments = வெற்று;
    முழு		remove_comments = (use_formatoptions == TRUE)
				  && has_format_option(FO_REMOVE_COMS);
    முழு		prev_was_comment;

    எனில் (save_undo && u_save((linenr_T)(curwin->w_cursor.lnum - 1),
			    (linenr_T)(curwin->w_cursor.lnum + மொத்தம்)) == FAIL)
	திரும்பு FAIL;

    // Allocate an array to store the number of spaces inserted before each
    // line.  We will use it to pre-compute the length of the new line and the
    // proper placement of each original line in the new one.
    spaces = lalloc_clear(மொத்தம், TRUE);
    எனில் (spaces == வெற்று)
	திரும்பு FAIL;
    எனில் (remove_comments)
    {
	comments = lalloc_clear(மொத்தம் * அளவு(முழு), TRUE);
	எனில் (comments == வெற்று)
	{
	    vim_free(spaces);
	    திரும்பு FAIL;
	}
    }

    /*
     * Don't move anything yet, just compute the final line length
     * and setup the array of space strings lengths
     * This loops forward over the joined lines.
     */
    க்கு (t = 0; t < மொத்தம்; ++t)
    {
	curr = curr_start = ml_get((linenr_T)(curwin->w_cursor.lnum + t));
	எனில் (t == 0 && setmark && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set the '[ mark.
	    curwin->w_buffer->b_op_start.lnum = curwin->w_cursor.lnum;
	    curwin->w_buffer->b_op_start.col  = (colnr_T)STRLEN(curr);
	}
	எனில் (remove_comments)
	{
	    // We don't want to remove the comment leader if the
	    // previous line is not a comment.
	    எனில் (t > 0 && prev_was_comment)
	    {

		char_u *new_curr = skip_comment(curr, TRUE, செருகு_space,
							   &prev_was_comment);
		comments[t] = (முழு)(new_curr - curr);
		curr = new_curr;
	    }
	    இல்லையெனில்
		curr = skip_comment(curr, FALSE, செருகு_space,
							   &prev_was_comment);
	}

	எனில் (செருகு_space && t > 0)
	{
	    curr = skipwhite(curr);
	    எனில் (*curr != NUL && *curr != ')'
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
		எனில் (endcurr1 == ' ')
		    endcurr1 = endcurr2;
		இல்லையெனில்
		    ++spaces[t];
		// extra space when 'joinspaces' set and line ends in '.'
		எனில் (       p_js
			&& (endcurr1 == '.'
			    || (vim_strchr(p_cpo, CPO_JOINSP) == வெற்று
				&& (endcurr1 == '?' || endcurr1 == '!'))))
		    ++spaces[t];
	    }
	}
	currsize = (முழு)STRLEN(curr);
	sumsize += currsize + spaces[t];
	endcurr1 = endcurr2 = NUL;
	எனில் (செருகு_space && currsize > 0)
	{
	    எனில் (has_mbyte)
	    {
		cend = curr + currsize;
		MB_PTR_BACK(curr, cend);
		endcurr1 = (*mb_ptr2char)(cend);
		எனில் (cend > curr)
		{
		    MB_PTR_BACK(curr, cend);
		    endcurr2 = (*mb_ptr2char)(cend);
		}
	    }
	    இல்லையெனில்
	    {
		endcurr1 = *(curr + currsize - 1);
		எனில் (currsize > 1)
		    endcurr2 = *(curr + currsize - 2);
	    }
	}
	வரி_breakcheck();
	எனில் (got_int)
	{
	    ret = FAIL;
	    செல் theend;
	}
    }

    // store the column position before last line
    col = sumsize - currsize - spaces[மொத்தம் - 1];

    // allocate the space for the new line
    newp_len = sumsize + 1;
    newp = alloc(newp_len);
    எனில் (newp == வெற்று)
    {
	ret = FAIL;
	செல் theend;
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
    முழு curr_off = (முழு)(curr - curr_start);

    curr = curr_start = ml_get((linenr_T)(curwin->w_cursor.lnum + மொத்தம் - 1));
    curr += curr_off;
#endif

    க்கு (t = மொத்தம் - 1; ; --t)
    {
	முழு spaces_removed;

	cend -= currsize;
	mch_memmove(cend, curr, (size_t)currsize);

	எனில் (spaces[t] > 0)
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
		t == மொத்தம் - 1, (long)(cend - newp), spaces_removed);
#endif
	எனில் (t == 0)
	    நிறுத்து;
	curr = curr_start = ml_get((linenr_T)(curwin->w_cursor.lnum + t - 1));
	எனில் (remove_comments)
	    curr += comments[t - 1];
	எனில் (செருகு_space && t > 1)
	    curr = skipwhite(curr);
	currsize = (முழு)STRLEN(curr);
    }

#ifdef FEAT_PROP_POPUP
    எனில் (um.buf != வெற்று)
    {
	um_set_text(&um, newp);
	um_reverse_props(&um);
	um_close(&um);
	newp = வெற்று;  // um_set_text took ownership
    }
    இல்லையெனில்
#endif
    ml_replace_len(curwin->w_cursor.lnum, newp, (colnr_T)newp_len, TRUE, FALSE);

    எனில் (setmark && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set the '] mark.
	curwin->w_buffer->b_op_end.lnum = curwin->w_cursor.lnum;
	curwin->w_buffer->b_op_end.col  = (colnr_T)sumsize;
    }

    // Only report the change in the first line here, del_lines() will report
    // the deleted line.
    changed_வரிகள்(curwin->w_cursor.lnum, currsize,
					       curwin->w_cursor.lnum + 1, 0L);
    /*
     * Delete following lines. To do this we move the cursor there
     * briefly, and then move it back. After del_lines() the cursor may
     * have moved up (last line deleted), so the current lnum is kept in t.
     */
    t = curwin->w_cursor.lnum;
    ++curwin->w_cursor.lnum;
    del_வரிகள்(மொத்தம் - 1, FALSE);
    curwin->w_cursor.lnum = t;

    /*
     * Set the cursor column:
     * Vi compatible: use the column of the first join
     * vim:	      use the column of the last join
     */
    curwin->w_cursor.col =
		    (vim_strchr(p_cpo, CPO_JOINCOL) != வெற்று ? currsize : col);
    check_cursor_col();

    curwin->w_cursor.coladd = 0;
    curwin->w_set_curswant = true;

theend:
    vim_free(spaces);
    எனில் (remove_comments)
	vim_free(comments);
    திரும்பு ret;
}

#ifdef FEAT_LINEBREAK
/*
 * TODO: consider using "tailp" of win_lbr_chartabsize() instead of temporarily
 * resetting 'linebreak'.
 *
 * Reset 'linebreak' and take care of side effects.
 * Returns the previous value, to be passed to restore_lbr().
 */
    முழு
reset_lbr(வெறுமை)
{
    எனில் (!curwin->w_p_lbr)
	திரும்பு FALSE;
    // changing 'linebreak' may require w_virtcol to be updated
    curwin->w_p_lbr = FALSE;
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);
    திரும்பு TRUE;
}

/*
 * Restore 'linebreak' and take care of side effects.
 */
    வெறுமை
restore_lbr(முழு lbr_saved)
{
    எனில் (curwin->w_p_lbr || !lbr_saved)
	திரும்பு;

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
    வெறுமை
block_prep(
    oparg_T		*oap,
    கட்டமைப்பு block_def	*bdp,
    linenr_T		lnum,
    முழு			is_del)
{
    முழு		incr = 0;
    char_u	*pend;
    char_u	*pstart;
    char_u	*வரி;
    char_u	*prev_pstart;
    char_u	*prev_pend;
    chartabsize_T cts;
#ifdef FEAT_LINEBREAK
		// Avoid a problem with unwanted linebreaks in block mode.
    முழு		lbr_saved = reset_lbr();
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

    வரி = ml_get(lnum);
    prev_pstart = வரி;
    init_chartabsize_arg(&cts, curwin, lnum, bdp->start_vcol, வரி, வரி);
    போது (cts.cts_vcol < oap->start_vcol && *cts.cts_ptr != NUL)
    {
	// Count a tab for what it's worth (if list mode not on)
	incr = lbr_chartabsize(&cts);
	cts.cts_vcol += incr;
	எனில் (VIM_ISWHITE(*cts.cts_ptr))
	{
	    bdp->pre_whitesp += incr;
	    bdp->pre_whitesp_c++;
	}
	இல்லையெனில்
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
    எனில் (bdp->start_vcol < oap->start_vcol)	// line too short
    {
	bdp->end_vcol = bdp->start_vcol;
	bdp->is_short = TRUE;
	எனில் (!is_del || oap->op_type == OP_APPEND)
	    bdp->endspaces = oap->end_vcol - oap->start_vcol + 1;
    }
    இல்லையெனில்
    {
	// notice: this converts partly selected Multibyte characters to
	// spaces, too.
	bdp->startspaces = bdp->start_vcol - oap->start_vcol;
	எனில் (is_del && bdp->startspaces)
	    bdp->startspaces = bdp->start_char_vcols - bdp->startspaces;
	pend = pstart;
	bdp->end_vcol = bdp->start_vcol;
	எனில் (bdp->end_vcol > oap->end_vcol)	// it's all in one character
	{
	    bdp->is_oneChar = TRUE;
	    எனில் (oap->op_type == OP_செருகு)
		bdp->endspaces = bdp->start_char_vcols - bdp->startspaces;
	    இல்லையெனில் எனில் (oap->op_type == OP_APPEND)
	    {
		bdp->startspaces += oap->end_vcol - oap->start_vcol + 1;
		bdp->endspaces = bdp->start_char_vcols - bdp->startspaces;
	    }
	    இல்லையெனில்
	    {
		bdp->startspaces = oap->end_vcol - oap->start_vcol + 1;
		எனில் (is_del && oap->op_type != OP_LSHIFT)
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
	இல்லையெனில்
	{
	    init_chartabsize_arg(&cts, curwin, lnum, bdp->end_vcol,
								  வரி, pend);
	    prev_pend = pend;
	    போது (cts.cts_vcol <= oap->end_vcol && *cts.cts_ptr != NUL)
	    {
		// count a tab for what it's worth (if list mode not on)
		prev_pend = cts.cts_ptr;
		incr = lbr_chartabsize_adv(&cts);
		cts.cts_vcol += incr;
	    }
	    bdp->end_vcol = cts.cts_vcol;
	    pend = cts.cts_ptr;
	    clear_chartabsize_arg(&cts);

	    எனில் (bdp->end_vcol <= oap->end_vcol
		    && (!is_del
			|| oap->op_type == OP_APPEND
			|| oap->op_type == OP_REPLACE)) // line too short
	    {
		bdp->is_short = TRUE;
		// Alternative: include spaces to fill up the block.
		// Disadvantage: can lead to trailing spaces when the line is
		// short where the text is put
		// if (!is_del || oap->op_type == OP_APPEND)
		எனில் (oap->op_type == OP_APPEND || virtual_op)
		    bdp->endspaces = oap->end_vcol - bdp->end_vcol
							     + oap->inclusive;
		இல்லையெனில்
		    bdp->endspaces = 0; // replace doesn't add characters
	    }
	    இல்லையெனில் எனில் (bdp->end_vcol > oap->end_vcol)
	    {
		bdp->endspaces = bdp->end_vcol - oap->end_vcol - 1;
		எனில் (!is_del && bdp->endspaces)
		{
		    bdp->endspaces = incr - bdp->endspaces;
		    எனில் (pend != pstart)
			pend = prev_pend;
		}
	    }
	}
	bdp->end_char_vcols = incr;
	எனில் (is_del && bdp->startspaces)
	    pstart = prev_pstart;
	bdp->textlen = (முழு)(pend - pstart);
    }
    bdp->textcol = (colnr_T) (pstart - வரி);
    bdp->textstart = pstart;
#ifdef FEAT_LINEBREAK
    restore_lbr(lbr_saved);
#endif
}

/*
 * Get block text from "start" to "end"
 */
    வெறுமை
charwise_block_prep(
    pos_T		start,
    pos_T		end,
    கட்டமைப்பு block_def	*bdp,
    linenr_T		lnum,
    முழு			inclusive)
{
    colnr_T startcol = 0, endcol = MAXCOL;
    colnr_T cs, ce;
    char_u *p;
    முழு	plen = ml_get_len(lnum);

    p = ml_get(lnum);
    bdp->startspaces = 0;
    bdp->endspaces = 0;
    bdp->is_oneChar = FALSE;
    bdp->start_char_vcols = 0;

    எனில் (lnum == start.lnum)
    {
	startcol = start.col;
	எனில் (virtual_op)
	{
	    getvcol(curwin, &start, &cs, வெற்று, &ce, 0);
	    எனில் (ce != cs && start.coladd > 0)
	    {
		// Part of a tab selected -- but don't
		// double-count it.
		bdp->start_char_vcols = ce - cs + 1;
		bdp->startspaces = bdp->start_char_vcols - start.coladd;
		எனில் (bdp->startspaces < 0)
		    bdp->startspaces = 0;
		startcol++;
	    }
	}
    }

    எனில் (lnum == end.lnum)
    {
	endcol = end.col;
	எனில் (virtual_op)
	{
	    getvcol(curwin, &end, &cs, வெற்று, &ce, 0);
	    எனில் (p[endcol] == NUL || (cs + end.coladd < ce
			// Don't add space for double-wide
			// char; endcol will be on last byte
			// of multi-byte char.
			&& (*mb_head_off)(p, p + endcol) == 0))
	    {
		எனில் (start.lnum == end.lnum && start.col == end.col)
		{
		    // Special case: inside a single char
		    bdp->is_oneChar = TRUE;
		    bdp->startspaces = end.coladd - start.coladd + inclusive;
		    endcol = startcol;
		}
		இல்லையெனில்
		{
		    bdp->endspaces = end.coladd + inclusive;
		    endcol -= inclusive;
		}
	    }
	}
    }
    எனில் (endcol == MAXCOL)
	endcol = ml_get_len(lnum);
    எனில் (startcol > endcol || bdp->is_oneChar)
	bdp->textlen = 0;
    இல்லையெனில்
	bdp->textlen = endcol - startcol + inclusive;
    bdp->textcol = startcol;
    bdp->textstart = startcol <= plen ? p + startcol : p;
}

/*
 * Handle the add/subtract operator.
 */
    வெறுமை
op_addsub(
    oparg_T	*oap,
    linenr_T	Prenum1,	    // Amount of add/subtract
    முழு		g_cmd)		    // was g<c-a>/g<c-x>
{
    pos_T		pos;
    கட்டமைப்பு block_def	bd;
    முழு			change_cnt = 0;
    linenr_T		amount = Prenum1;

    // do_addsub() might trigger re-evaluation of 'foldexpr' halfway, when the
    // buffer is not completely updated yet. Postpone updating folds until before
    // the call to changed_lines().
#ifdef FEAT_FOLDING
    disable_fold_update++;
#endif

    எனில் (!VIsual_active)
    {
	pos = curwin->w_cursor;
	எனில் (u_save_cursor() == FAIL)
	{
#ifdef FEAT_FOLDING
	    disable_fold_update--;
#endif
	    திரும்பு;
	}
	change_cnt = do_addsub(oap->op_type, &pos, 0, amount);
#ifdef FEAT_FOLDING
	disable_fold_update--;
#endif
	எனில் (change_cnt)
	    changed_வரிகள்(pos.lnum, 0, pos.lnum + 1, 0L);
    }
    இல்லையெனில்
    {
	முழு	one_change;
	முழு	length;
	pos_T	startpos;

	எனில் (u_save((linenr_T)(oap->start.lnum - 1),
					(linenr_T)(oap->end.lnum + 1)) == FAIL)
	{
#ifdef FEAT_FOLDING
	    disable_fold_update--;
#endif
	    திரும்பு;
	}

	pos = oap->start;
	க்கு (; pos.lnum <= oap->end.lnum; ++pos.lnum)
	{
	    எனில் (oap->block_mode)		    // Visual block mode
	    {
		block_prep(oap, &bd, pos.lnum, FALSE);
		pos.col = bd.textcol;
		length = bd.textlen;
	    }
	    இல்லையெனில் எனில் (oap->motion_type == MLINE)
	    {
		curwin->w_cursor.col = 0;
		pos.col = 0;
		length = ml_get_len(pos.lnum);
	    }
	    இல்லையெனில் // oap->motion_type == MCHAR
	    {
		எனில் (pos.lnum == oap->start.lnum && !oap->inclusive)
		    dec(&(oap->end));
		length = ml_get_len(pos.lnum);
		pos.col = 0;
		எனில் (pos.lnum == oap->start.lnum)
		{
		    pos.col += oap->start.col;
		    length -= oap->start.col;
		}
		எனில் (pos.lnum == oap->end.lnum)
		{
		    length = ml_get_len(oap->end.lnum);
		    எனில் (oap->end.col >= length)
			oap->end.col = length - 1;
		    length = oap->end.col - pos.col + 1;
		}
	    }
	    one_change = do_addsub(oap->op_type, &pos, length, amount);
	    எனில் (one_change)
	    {
		// Remember the start position of the first change.
		எனில் (change_cnt == 0)
		    startpos = curbuf->b_op_start;
		++change_cnt;
	    }

#ifdef FEAT_NETBEANS_INTG
	    எனில் (netbeans_active() && one_change)
	    {
		char_u *ptr;

		netbeans_removed(curbuf, pos.lnum, pos.col, (long)length);
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, pos.col,
						&ptr[pos.col], length);
	    }
#endif
	    எனில் (g_cmd && one_change)
		amount += Prenum1;
	}

#ifdef FEAT_FOLDING
	disable_fold_update--;
#endif
	எனில் (change_cnt)
	    changed_வரிகள்(oap->start.lnum, 0, oap->end.lnum + 1, 0L);

	எனில் (!change_cnt && oap->is_VIsual)
	    // No change: need to remove the Visual selection
	    redraw_curbuf_later(UPD_INVERTED);

	// Set '[ mark if something changed. Keep the last end
	// position from do_addsub().
	எனில் (change_cnt > 0 && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	    curbuf->b_op_start = startpos;

	எனில் (change_cnt > p_report)
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
    நிலையான முழு
do_addsub(
    முழு		op_type,
    pos_T	*pos,
    முழு		length,
    linenr_T	Prenum1)
{
    முழு		col;
    முழு		pre;		// 'X'/'x': hex; '0': octal; 'B'/'b': bin
    நிலையான முழு	hexupper = FALSE;	// 0xABC
    uvarnumber_T	n;
    uvarnumber_T	oldn;
    char_u	*ptr;
    முழு		linelen;
    முழு		c;
    முழு		todel;
    முழு		do_hex;
    முழு		do_oct;
    முழு		do_bin;
    முழு		do_alpha;
    முழு		do_unsigned;
    முழு		do_blank;
    முழு		blank_unsigned = FALSE;	// blank: treat as unsigned?
    முழு		firstdigit;
    முழு		subtract;
    முழு		negative = FALSE;
    முழு		was_positive = TRUE;
    முழு		visual = VIsual_active;
    முழு		did_change = FALSE;
    pos_T	save_cursor = curwin->w_cursor;
    முழு		maxlen = 0;
    pos_T	startpos;
    pos_T	endpos;
    colnr_T	save_coladd = 0;

    do_hex = (vim_strchr(curbuf->b_p_nf, 'x') != வெற்று);	// "heX"
    do_oct = (vim_strchr(curbuf->b_p_nf, 'o') != வெற்று);	// "Octal"
    do_bin = (vim_strchr(curbuf->b_p_nf, 'b') != வெற்று);	// "Bin"
    do_alpha = (vim_strchr(curbuf->b_p_nf, 'p') != வெற்று);	// "alPha"
    do_unsigned = (vim_strchr(curbuf->b_p_nf, 'u') != வெற்று);	// "Unsigned"
    do_blank = (vim_strchr(curbuf->b_p_nf, 'k') != வெற்று);	// "blanK"

    எனில் (virtual_active())
    {
	save_coladd = pos->coladd;
	pos->coladd = 0;
    }

    curwin->w_cursor = *pos;
    ptr = ml_get(pos->lnum);
    linelen = ml_get_len(pos->lnum);
    col = pos->col;

    எனில் (col + !!save_coladd >= linelen)
	செல் theend;

    /*
     * First check if we are on a hexadecimal number, after the "0x".
     */
    எனில் (!VIsual_active)
    {
	எனில் (do_bin)
	    போது (col > 0 && vim_isbdigit(ptr[col]))
	    {
		--col;
		எனில் (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }

	எனில் (do_hex)
	    போது (col > 0 && vim_isxdigit(ptr[col]))
	    {
		--col;
		எனில் (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }

	எனில் (       do_bin
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

	    போது (col > 0 && vim_isdigit(ptr[col]))
	    {
		col--;
		எனில் (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }
	}

	எனில் ((       do_hex
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
	    எனில் (has_mbyte)
		col -= (*mb_head_off)(ptr, ptr + col);
	}
	இல்லையெனில்
	{
	    /*
	     * Search forward and then backward to find the start of number.
	     */
	    col = pos->col;

	    போது (ptr[col] != NUL
		    && !vim_isdigit(ptr[col])
		    && !(do_alpha && ASCII_ISALPHA(ptr[col])))
		col += mb_ptr2len(ptr + col);

	    போது (col > 0
		    && vim_isdigit(ptr[col - 1])
		    && !(do_alpha && ASCII_ISALPHA(ptr[col])))
	    {
		--col;
		எனில் (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }
	}
    }

    எனில் (visual)
    {
	போது (ptr[col] != NUL && length > 0
		&& !vim_isdigit(ptr[col])
		&& !(do_alpha && ASCII_ISALPHA(ptr[col])))
	{
	    முழு mb_len = mb_ptr2len(ptr + col);

	    col += mb_len;
	    length -= mb_len;
	}

	எனில் (length == 0)
	    செல் theend;

	எனில் (col > pos->col && ptr[col - 1] == '-'
		&& (!has_mbyte || !(*mb_head_off)(ptr, ptr + col - 1))
		&& !do_unsigned)
	{
	    எனில் (do_blank && col >= 2 && !VIM_ISWHITE(ptr[col - 2]))
		blank_unsigned = TRUE;
	    இல்லையெனில்
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
    எனில் (!VIM_ISDIGIT(firstdigit) && !(do_alpha && ASCII_ISALPHA(firstdigit)))
    {
	beep_flush();
	செல் theend;
    }

    எனில் (do_alpha && ASCII_ISALPHA(firstdigit))
    {
	// decrement or increment alphabetic character
	எனில் (op_type == OP_NR_SUB)
	{
	    எனில் (CharOrd(firstdigit) < Prenum1)
	    {
		எனில் (SAFE_isupper(firstdigit))
		    firstdigit = 'A';
		இல்லையெனில்
		    firstdigit = 'a';
	    }
	    இல்லையெனில்
		firstdigit -= Prenum1;
	}
	இல்லையெனில்
	{
	    எனில் (26 - CharOrd(firstdigit) - 1 < Prenum1)
	    {
		எனில் (SAFE_isupper(firstdigit))
		    firstdigit = 'Z';
		இல்லையெனில்
		    firstdigit = 'z';
	    }
	    இல்லையெனில்
		firstdigit += Prenum1;
	}
	curwin->w_cursor.col = col;
	எனில் (!did_change)
	    startpos = curwin->w_cursor;
	did_change = TRUE;
	(வெறுமை)del_char(FALSE);
	ins_char(firstdigit);
	endpos = curwin->w_cursor;
	curwin->w_cursor.col = col;
    }
    இல்லையெனில்
    {
	char_u	*buf1;
	முழு	buf1len;
	char_u	buf2[NUMBUFLEN];
	முழு	buf2len;
	pos_T	save_pos;
	முழு	i;

	எனில் (col > 0 && ptr[col - 1] == '-'
		&& (!has_mbyte ||
		    !(*mb_head_off)(ptr, ptr + col - 1))
		&& !visual
		&& !do_unsigned)
	{
	    எனில் (do_blank && col >= 2 && !VIM_ISWHITE(ptr[col - 2]))
		blank_unsigned = TRUE;
	    இல்லையெனில்
	    {
		// negative number
		--col;
		negative = TRUE;
	    }
	}

	// get the number value (unsigned)
	எனில் (visual && VIsual_mode != 'V')
	    maxlen = (curbuf->b_visual.vi_curswant == MAXCOL
		    ? linelen - col : length);

	முழு overflow = FALSE;
	vim_str2nr(ptr + col, &pre, &length,
		0 + (do_bin ? STR2NR_BIN : 0)
		    + (do_oct ? STR2NR_OCT : 0)
		    + (do_hex ? STR2NR_HEX : 0),
		வெற்று, &n, maxlen, FALSE, &overflow);

	// ignore leading '-' for hex and octal and bin numbers
	எனில் (pre && negative)
	{
	    ++col;
	    --length;
	    negative = FALSE;
	}
	// add or subtract
	subtract = FALSE;
	எனில் (op_type == OP_NR_SUB)
	    subtract ^= TRUE;
	எனில் (negative)
	    subtract ^= TRUE;

	oldn = n;
	எனில் (!overflow)  // if number is too big don't add/subtract
	{
	    எனில் (subtract)
		n -= (uvarnumber_T)Prenum1;
	    இல்லையெனில்
		n += (uvarnumber_T)Prenum1;
	}

	// handle wraparound for decimal numbers
	எனில் (!pre)
	{
	    எனில் (subtract)
	    {
		எனில் (n > oldn)
		{
		    n = 1 + (n ^ (uvarnumber_T)-1);
		    negative ^= TRUE;
		}
	    }
	    இல்லையெனில்
	    {
		// add
		எனில் (n < oldn)
		{
		    n = (n ^ (uvarnumber_T)-1);
		    negative ^= TRUE;
		}
	    }
	    எனில் (n == 0)
		negative = FALSE;
	}

	எனில் ((do_unsigned || blank_unsigned) && negative)
	{
	    எனில் (subtract)
		// sticking at zero.
		n = (uvarnumber_T)0;
	    இல்லையெனில்
		// sticking at 2^64 - 1.
		n = (uvarnumber_T)(-1);
	    negative = FALSE;
	}

	எனில் (visual && !was_positive && !negative && col > 0)
	{
	    // need to remove the '-'
	    col--;
	    length++;
	}

	/*
	 * Delete the old number.
	 */
	curwin->w_cursor.col = col;
	எனில் (!did_change)
	    startpos = curwin->w_cursor;
	did_change = TRUE;
	todel = length;
	c = gchar_cursor();
	/*
	 * Don't include the '-' in the length, only the length of the
	 * part after it is kept the same.
	 */
	எனில் (c == '-')
	    --length;

	save_pos = curwin->w_cursor;
	க்கு (i = 0; i < todel; ++i)
	{
	    எனில் (c < 0x100 && SAFE_isalpha(c))
	    {
		எனில் (SAFE_isupper(c))
		    hexupper = TRUE;
		இல்லையெனில்
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
	எனில் (buf1 == வெற்று)
	    செல் theend;
	ptr = buf1;
	எனில் (negative && (!visual || was_positive))
	    *ptr++ = '-';
	எனில் (pre)
	{
	    *ptr++ = '0';
	    --length;
	}
	எனில் (pre == 'b' || pre == 'B' ||
	    pre == 'x' || pre == 'X')
	{
	    *ptr++ = pre;
	    --length;
	}

	/*
	 * Put the number characters in buf2[].
	 */
	எனில் (pre == 'b' || pre == 'B')
	{
	    முழு bit = 0;
	    முழு bits = அளவு(uvarnumber_T) * 8;

	    // leading zeros
	    க்கு (bit = bits; bit > 0; bit--)
		எனில் ((n >> (bit - 1)) & 0x1) நிறுத்து;

	    க்கு (buf2len = 0; bit > 0 && buf2len < (NUMBUFLEN - 1); bit--)
		buf2[buf2len++] = ((n >> (bit - 1)) & 0x1) ? '1' : '0';

	    buf2[buf2len] = NUL;
	}
	இல்லையெனில் எனில் (pre == 0)
	    buf2len = vim_snprintf((எழுத்து *)buf2, NUMBUFLEN, "%llu", n);
	இல்லையெனில் எனில் (pre == '0')
	    buf2len = vim_snprintf((எழுத்து *)buf2, NUMBUFLEN, "%llo", n);
	இல்லையெனில் எனில் (pre && hexupper)
	    buf2len = vim_snprintf((எழுத்து *)buf2, NUMBUFLEN, "%llX", n);
	இல்லையெனில்
	    buf2len = vim_snprintf((எழுத்து *)buf2, NUMBUFLEN, "%llx", n);
	length -= buf2len;

	/*
	 * Adjust number of zeros to the new number of digits, so the
	 * total length of the number remains the same.
	 * Don't do this when
	 * the result may look like an octal number.
	 */
	எனில் (firstdigit == '0' && !(do_oct && pre == 0))
	    போது (length-- > 0)
		*ptr++ = '0';
	*ptr = NUL;
	buf1len = (முழு)(ptr - buf1);

	STRCPY(buf1 + buf1len, buf2);
	buf1len += buf2len;

	// Insert just after the first character to be removed, so that any
	// text properties will be adjusted.  Then delete the old number
	// afterwards.
	save_pos = curwin->w_cursor;
	எனில் (todel > 0)
	    inc_cursor();
	ins_str(buf1, (size_t)buf1len);		// insert the new number
	vim_free(buf1);

	// del_char() will also mark line needing displaying
	எனில் (todel > 0)
	{
	    முழு bytes_after = ml_get_curline_len() - curwin->w_cursor.col;

	    // Delete the one character before the insert.
	    curwin->w_cursor = save_pos;
	    (வெறுமை)del_char(FALSE);
	    curwin->w_cursor.col = ml_get_curline_len() - bytes_after;
	    --todel;
	}
	போது (todel-- > 0)
	    (வெறுமை)del_char(FALSE);

	endpos = curwin->w_cursor;
	எனில் (did_change && curwin->w_cursor.col)
	    --curwin->w_cursor.col;
    }

    எனில் (did_change && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// set the '[ and '] marks
	curbuf->b_op_start = startpos;
	curbuf->b_op_end = endpos;
	எனில் (curbuf->b_op_end.col > 0)
	    --curbuf->b_op_end.col;
    }

theend:
    எனில் (visual)
	curwin->w_cursor = save_cursor;
    இல்லையெனில் எனில் (did_change)
	curwin->w_set_curswant = true;
    இல்லையெனில் எனில் (virtual_active())
	curwin->w_cursor.coladd = save_coladd;

    திரும்பு did_change;
}

    வெறுமை
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
    நிலையான varnumber_T
வரி_மொத்தம்_info(
    char_u	*வரி,
    varnumber_T	*wc,
    varnumber_T	*cc,
    varnumber_T	limit,
    முழு		eol_size)
{
    varnumber_T	i;
    varnumber_T	சொற்கள் = 0;
    varnumber_T	chars = 0;
    முழு		is_word = 0;

    க்கு (i = 0; i < limit && வரி[i] != NUL; )
    {
	எனில் (is_word)
	{
	    எனில் (vim_isspace(வரி[i]))
	    {
		சொற்கள்++;
		is_word = 0;
	    }
	}
	இல்லையெனில் எனில் (!vim_isspace(வரி[i]))
	    is_word = 1;
	++chars;
	i += (*mb_ptr2len)(வரி + i);
    }

    எனில் (is_word)
	சொற்கள்++;
    *wc += சொற்கள்;

    // Add eol_size if the end of line was reached before hitting limit.
    எனில் (i < limit && வரி[i] == NUL)
    {
	i += eol_size;
	chars += eol_size;
    }
    *cc += chars;
    திரும்பு i;
}

/*
 * Give some info about the position of the cursor (for "g CTRL-G").
 * In Visual mode, give some info about the selected region.  (In this case,
 * the *_count_cursor variables store running totals for the selection.)
 * When "dict" is not NULL store the info there instead of showing it.
 */
    வெறுமை
cursor_pos_info(dict_T *dict)
{
    char_u	*p;
    char_u	buf1[50];
    char_u	buf2[40];
    linenr_T	lnum;
    varnumber_T	byte_மொத்தம் = 0;
    varnumber_T	bom_மொத்தம்  = 0;
    varnumber_T	byte_மொத்தம்_cursor = 0;
    varnumber_T	char_மொத்தம் = 0;
    varnumber_T	char_மொத்தம்_cursor = 0;
    varnumber_T	word_மொத்தம் = 0;
    varnumber_T	word_மொத்தம்_cursor = 0;
    முழு		eol_size;
    varnumber_T	last_check = 100000L;
    long	வரி_மொத்தம்_selected = 0;
    pos_T	min_pos, max_pos;
    oparg_T	oparg;
    கட்டமைப்பு block_def	bd;

    /*
     * Compute the length of the file in characters.
     */
    எனில் (curbuf->b_ml.ml_flags & ML_EMPTY)
    {
	எனில் (dict == வெற்று)
	{
	    செய்தி(_(no_வரிகள்_செய்தி));
	    திரும்பு;
	}
    }
    இல்லையெனில்
    {
	எனில் (get_fileformat(curbuf) == EOL_DOS)
	    eol_size = 2;
	இல்லையெனில்
	    eol_size = 1;

	எனில் (VIsual_active)
	{
	    எனில் (LT_POS(VIsual, curwin->w_cursor))
	    {
		min_pos = VIsual;
		max_pos = curwin->w_cursor;
	    }
	    இல்லையெனில்
	    {
		min_pos = curwin->w_cursor;
		max_pos = VIsual;
	    }
	    எனில் (*p_sel == 'e' && max_pos.col > 0)
		--max_pos.col;

	    எனில் (VIsual_mode == Ctrl_V)
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
		எனில் (curwin->w_curswant == MAXCOL)
		    oparg.end_vcol = MAXCOL;
		// Swap the start, end vcol if needed
		எனில் (oparg.end_vcol < oparg.start_vcol)
		{
		    oparg.end_vcol += oparg.start_vcol;
		    oparg.start_vcol = oparg.end_vcol - oparg.start_vcol;
		    oparg.end_vcol -= oparg.start_vcol;
		}
	    }
	    வரி_மொத்தம்_selected = max_pos.lnum - min_pos.lnum + 1;
	}

	க்கு (lnum = 1; lnum <= curbuf->b_ml.ml_வரி_மொத்தம்; ++lnum)
	{
	    // Check for a CTRL-C every 100000 characters.
	    எனில் (byte_மொத்தம் > last_check)
	    {
		ui_breakcheck();
		எனில் (got_int)
		    திரும்பு;
		last_check = byte_மொத்தம் + 100000L;
	    }

	    // Do extra processing for VIsual mode.
	    எனில் (VIsual_active
		    && lnum >= min_pos.lnum && lnum <= max_pos.lnum)
	    {
		char_u	    *s = வெற்று;
		long	    len = 0L;

		தேர்வு (VIsual_mode)
		{
		    நிலை Ctrl_V:
			virtual_op = virtual_active();
			block_prep(&oparg, &bd, lnum, 0);
			virtual_op = MAYBE;
			s = bd.textstart;
			len = (long)bd.textlen;
			நிறுத்து;
		    நிலை 'V':
			s = ml_get(lnum);
			len = MAXCOL;
			நிறுத்து;
		    நிலை 'v':
			{
			    colnr_T start_col = (lnum == min_pos.lnum)
							   ? min_pos.col : 0;
			    colnr_T end_col = (lnum == max_pos.lnum)
				      ? max_pos.col - start_col + 1 : MAXCOL;

			    s = ml_get(lnum) + start_col;
			    len = end_col;
			}
			நிறுத்து;
		}
		எனில் (s != வெற்று)
		{
		    byte_மொத்தம்_cursor += வரி_மொத்தம்_info(s, &word_மொத்தம்_cursor,
					   &char_மொத்தம்_cursor, len, eol_size);
		    எனில் (lnum == curbuf->b_ml.ml_வரி_மொத்தம்
			    && !curbuf->b_p_eol
			    && (curbuf->b_p_bin || !curbuf->b_p_fixeol)
			    && (long)STRLEN(s) < len)
			byte_மொத்தம்_cursor -= eol_size;
		}
	    }
	    இல்லையெனில்
	    {
		// In non-visual mode, check for the line the cursor is on
		எனில் (lnum == curwin->w_cursor.lnum)
		{
		    word_மொத்தம்_cursor += word_மொத்தம்;
		    char_மொத்தம்_cursor += char_மொத்தம்;
		    byte_மொத்தம்_cursor = byte_மொத்தம் +
			வரி_மொத்தம்_info(ml_get(lnum),
				&word_மொத்தம்_cursor, &char_மொத்தம்_cursor,
				(varnumber_T)(curwin->w_cursor.col + 1),
				eol_size);
		}
	    }
	    // Add to the running totals
	    byte_மொத்தம் += வரி_மொத்தம்_info(ml_get(lnum), &word_மொத்தம்,
					 &char_மொத்தம், (varnumber_T)MAXCOL,
					 eol_size);
	}

	// Correction for when last line doesn't have an EOL.
	எனில் (!curbuf->b_p_eol && (curbuf->b_p_bin || !curbuf->b_p_fixeol))
	    byte_மொத்தம் -= eol_size;

	எனில் (dict == வெற்று)
	{
	    எனில் (VIsual_active)
	    {
		எனில் (VIsual_mode == Ctrl_V && curwin->w_curswant < MAXCOL)
		{
		    getvcols(curwin, &min_pos, &max_pos,
						&min_pos.col, &max_pos.col, 0);
		    vim_snprintf((எழுத்து *)buf1, அளவு(buf1), _("%ld Cols; "),
			    (long)(oparg.end_vcol - oparg.start_vcol + 1));
		}
		இல்லையெனில்
		    buf1[0] = NUL;

		எனில் (char_மொத்தம்_cursor == byte_மொத்தம்_cursor
						    && char_மொத்தம் == byte_மொத்தம்)
		    vim_snprintf((எழுத்து *)IObuff, IOSIZE,
			    _("Selected %s%ld of %ld Lines; %lld of %lld Words; %lld of %lld Bytes"),
			    buf1, வரி_மொத்தம்_selected,
			    (long)curbuf->b_ml.ml_வரி_மொத்தம்,
			    word_மொத்தம்_cursor,
			    word_மொத்தம்,
			    byte_மொத்தம்_cursor,
			    byte_மொத்தம்);
		இல்லையெனில்
		    vim_snprintf((எழுத்து *)IObuff, IOSIZE,
			    _("Selected %s%ld of %ld Lines; %lld of %lld Words; %lld of %lld Chars; %lld of %lld Bytes"),
			    buf1, வரி_மொத்தம்_selected,
			    (long)curbuf->b_ml.ml_வரி_மொத்தம்,
			    word_மொத்தம்_cursor,
			    word_மொத்தம்,
			    char_மொத்தம்_cursor,
			    char_மொத்தம்,
			    byte_மொத்தம்_cursor,
			    byte_மொத்தம்);
	    }
	    இல்லையெனில்
	    {
		p = ml_get_curline();
		validate_virtcol();
		col_print(buf1, அளவு(buf1), (முழு)curwin->w_cursor.col + 1,
			(முழு)curwin->w_virtcol + 1);
		col_print(buf2, அளவு(buf2), ml_get_curline_len(),
							   linetabsize_str(p));

		எனில் (char_மொத்தம்_cursor == byte_மொத்தம்_cursor
			&& char_மொத்தம் == byte_மொத்தம்)
		    vim_snprintf((எழுத்து *)IObuff, IOSIZE,
			_("Col %s of %s; Line %ld of %ld; Word %lld of %lld; Byte %lld of %lld"),
			(எழுத்து *)buf1, (எழுத்து *)buf2,
			(long)curwin->w_cursor.lnum,
			(long)curbuf->b_ml.ml_வரி_மொத்தம்,
			word_மொத்தம்_cursor, word_மொத்தம்,
			byte_மொத்தம்_cursor, byte_மொத்தம்);
		இல்லையெனில்
		    vim_snprintf((எழுத்து *)IObuff, IOSIZE,
			_("Col %s of %s; Line %ld of %ld; Word %lld of %lld; Char %lld of %lld; Byte %lld of %lld"),
			(எழுத்து *)buf1, (எழுத்து *)buf2,
			(long)curwin->w_cursor.lnum,
			(long)curbuf->b_ml.ml_வரி_மொத்தம்,
			word_மொத்தம்_cursor, word_மொத்தம்,
			char_மொத்தம்_cursor, char_மொத்தம்,
			byte_மொத்தம்_cursor, byte_மொத்தம்);
	    }
	}

	bom_மொத்தம் = bomb_size();
	எனில் (dict == வெற்று && bom_மொத்தம் > 0)
	{
	    size_t len = STRLEN(IObuff);

	    vim_snprintf((எழுத்து *)IObuff + len, IOSIZE - len,
				 _("(+%lld for BOM)"), bom_மொத்தம்);
	}
	எனில் (dict == வெற்று)
	{
	    // Don't shorten this message, the user asked for it.
	    p = p_shm;
	    p_shm = (char_u *)"";
	    செய்தி((எழுத்து *)IObuff);
	    p_shm = p;
	}
    }
#எனில் defined(FEAT_EVAL)
    எனில் (dict != வெற்று)
    {
	dict_add_எண்(dict, "words", word_மொத்தம்);
	dict_add_எண்(dict, "chars", char_மொத்தம்);
	dict_add_எண்(dict, "bytes", byte_மொத்தம் + bom_மொத்தம்);
	dict_add_எண்(dict, VIsual_active ? "visual_bytes" : "cursor_bytes",
		byte_மொத்தம்_cursor);
	dict_add_எண்(dict, VIsual_active ? "visual_chars" : "cursor_chars",
		char_மொத்தம்_cursor);
	dict_add_எண்(dict, VIsual_active ? "visual_words" : "cursor_words",
		word_மொத்தம்_cursor);
    }
#endif
}

/*
 * Handle indent and format operators and visual mode ":".
 */
    நிலையான வெறுமை
op_colon(oparg_T *oap)
{
    stuffcharReadbuff(':');
    எனில் (oap->is_VIsual)
	stuffReadbuff((char_u *)"'<,'>");
    இல்லையெனில்
    {
	// Make the range look nice, so it can be repeated.
	எனில் (oap->start.lnum == curwin->w_cursor.lnum)
	    stuffcharReadbuff('.');
	இல்லையெனில்
	    stuffnumReadbuff((long)oap->start.lnum);

#ifdef FEAT_FOLDING
	// When using !! on a closed fold the range ".!" works best to operate
	// on, it will be made the whole closed fold later.
	linenr_T endOfStartFold = oap->start.lnum;
	(வெறுமை)hasFolding(oap->start.lnum, வெற்று, &endOfStartFold);
#endif
	எனில் (oap->end.lnum != oap->start.lnum
#ifdef FEAT_FOLDING
		&& oap->end.lnum != endOfStartFold
#endif
	   )
	{
	    // Make it a range with the end line.
	    stuffcharReadbuff(',');
	    எனில் (oap->end.lnum == curwin->w_cursor.lnum)
		stuffcharReadbuff('.');
	    இல்லையெனில் எனில் (oap->end.lnum == curbuf->b_ml.ml_வரி_மொத்தம்)
		stuffcharReadbuff('$');
	    இல்லையெனில் எனில் (oap->start.lnum == curwin->w_cursor.lnum
#ifdef FEAT_FOLDING
		    // do not use ".+number" for a closed fold, it would count
		    // folded lines twice
		    && !hasFolding(oap->end.lnum, வெற்று, வெற்று)
#endif
		    )
	    {
		stuffReadbuff((char_u *)".+");
		stuffnumReadbuff((long)oap->வரி_மொத்தம் - 1);
	    }
	    இல்லையெனில்
		stuffnumReadbuff((long)oap->end.lnum);
	}
    }
    எனில் (oap->op_type != OP_COLON)
	stuffReadbuff((char_u *)"!");
    எனில் (oap->op_type == OP_INDENT)
    {
	எனில் (*get_equalprg() == NUL)
	    stuffReadbuff((char_u *)"indent");
	இல்லையெனில்
	    stuffReadbuff(get_equalprg());
	stuffReadbuff((char_u *)"\n");
    }
    இல்லையெனில் எனில் (oap->op_type == OP_FORMAT)
    {
	எனில் (*curbuf->b_p_fp != NUL)
	    stuffReadbuff(curbuf->b_p_fp);
	இல்லையெனில் எனில் (*p_fp != NUL)
	    stuffReadbuff(p_fp);
	இல்லையெனில்
	    stuffReadbuff((char_u *)"fmt");
	stuffReadbuff((char_u *)"\n']");
    }

    // do_cmdline() does the rest
}

// callback function for 'operatorfunc'
நிலையான callback_T opfunc_cb;

/*
 * Process the 'operatorfunc' option value.
 * Returns OK or FAIL.
 */
    எழுத்து *
did_set_operatorfunc(optset_T *args UNUSED)
{
    எனில் (option_set_callback_func(p_opfunc, &opfunc_cb) == FAIL)
	திரும்பு e_invalid_argument;

    திரும்பு வெற்று;
}

#எனில் defined(EXITFREE)
    வெறுமை
free_operatorfunc_option(வெறுமை)
{
# ifdef FEAT_EVAL
    free_callback(&opfunc_cb);
# endif
}
#endif

#எனில் defined(FEAT_EVAL)
/*
 * Mark the global 'operatorfunc' callback with "copyID" so that it is not
 * garbage collected.
 */
    முழு
set_ref_in_opfunc(முழு copyID UNUSED)
{
    முழு abort = FALSE;

    abort = set_ref_in_callback(&opfunc_cb, copyID);

    திரும்பு abort;
}
#endif

/*
 * Handle the "g@" operator: call 'operatorfunc'.
 */
    நிலையான வெறுமை
op_function(oparg_T *oap UNUSED)
{
#ifdef FEAT_EVAL
    typval_T	argv[2];
    pos_T	orig_start = curbuf->b_op_start;
    pos_T	orig_end = curbuf->b_op_end;
    typval_T	rettv;

    எனில் (*p_opfunc == NUL)
	emsg(_(e_operatorfunc_is_empty));
    இல்லையெனில்
    {
	// Set '[ and '] marks to text to be operated on.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
	எனில் (oap->motion_type != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_end);

	argv[0].v_type = VAR_STRING;
	எனில் (oap->block_mode)
	    argv[0].vval.v_string = (char_u *)"block";
	இல்லையெனில் எனில் (oap->motion_type == MLINE)
	    argv[0].vval.v_string = (char_u *)"line";
	இல்லையெனில்
	    argv[0].vval.v_string = (char_u *)"char";
	argv[1].v_type = VAR_UNKNOWN;

	// Reset virtual_op so that 'virtualedit' can be changed in the
	// function.
	முழு save_virtual_op = virtual_op;
	virtual_op = MAYBE;

	// Reset finish_op so that mode() returns the right value.
	முழு save_finish_op = finish_op;
	finish_op = FALSE;

	எனில் (call_callback(&opfunc_cb, 0, &rettv, 1, argv) != FAIL)
	    clear_tv(&rettv);

	virtual_op = save_virtual_op;
	finish_op = save_finish_op;
	எனில் (cmdmod.cmod_flags & CMOD_LOCKMARKS)
	{
	    curbuf->b_op_start = orig_start;
	    curbuf->b_op_end = orig_end;
	}
    }
#இல்லையெனில்
    emsg(_(e_eval_feature_not_available));
#endif
}

/*
 * Calculate start/end virtual columns for operating in block mode.
 */
    நிலையான வெறுமை
get_op_vcol(
    oparg_T	*oap,
    colnr_T	redo_VIsual_vcol,
    முழு		initial)    // when TRUE adjust position for 'selectmode'
{
    colnr_T	    start, end;

    எனில் (VIsual_mode != Ctrl_V
	    || (!initial && oap->end.col < curwin->w_width))
	திரும்பு;

    oap->block_mode = TRUE;

    // prevent from moving onto a trail byte
    எனில் (has_mbyte)
	mb_adjustpos(curwin->w_buffer, &oap->end);

    getvvcol(curwin, &(oap->start), &oap->start_vcol, வெற்று, &oap->end_vcol, 0);

    எனில் (!redo_VIsual_busy)
    {
	getvvcol(curwin, &(oap->end), &start, வெற்று, &end, 0);

	எனில் (start < oap->start_vcol)
	    oap->start_vcol = start;
	எனில் (end > oap->end_vcol)
	{
	    எனில் (initial && *p_sel == 'e' && start >= 1
				    && start - 1 >= oap->end_vcol)
		oap->end_vcol = start - 1;
	    இல்லையெனில்
		oap->end_vcol = end;
	}
    }

    // if '$' was used, get oap->end_vcol from longest line
    எனில் (curwin->w_curswant == MAXCOL)
    {
	curwin->w_cursor.col = MAXCOL;
	oap->end_vcol = 0;
	க்கு (curwin->w_cursor.lnum = oap->start.lnum;
		curwin->w_cursor.lnum <= oap->end.lnum;
					++curwin->w_cursor.lnum)
	{
	    getvvcol(curwin, &curwin->w_cursor, வெற்று, வெற்று, &end, 0);
	    எனில் (end > oap->end_vcol)
		oap->end_vcol = end;
	}
    }
    இல்லையெனில் எனில் (redo_VIsual_busy)
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
வகைவரை கட்டமைப்பு {
    முழு		rv_mode;	// 'v', 'V', or Ctrl-V
    linenr_T	rv_வரி_மொத்தம்;	// number of lines
    colnr_T	rv_vcol;	// number of cols or end column
    long	rv_மொத்தம்;	// count for Visual operator
    முழு		rv_arg;		// extra argument
} redo_VIsual_T;

    நிலையான முழு
is_ex_cmdchar(cmdarg_T *cap)
{
    திரும்பு cap->cmdchar == ':'
	|| cap->cmdchar == K_COMMAND
	|| cap->cmdchar == K_SCRIPT_COMMAND;
}

/*
 * Handle an operator after Visual mode or when the movement is finished.
 * "gui_yank" is true when yanking text for the clipboard.
 */
    வெறுமை
do_pending_operator(cmdarg_T *cap, முழு old_col, முழு gui_yank)
{
    oparg_T	*oap = cap->oap;
    pos_T	old_cursor;
    முழு		empty_region_error;
    முழு		restart_edit_save;
#ifdef FEAT_LINEBREAK
    முழு		lbr_saved = curwin->w_p_lbr;
#endif

    // The visual area is remembered for redo
    நிலையான redo_VIsual_T   redo_VIsual = {NUL, 0, 0, 0,0};

    முழு		    include_வரி_break = FALSE;

#எனில் defined(FEAT_CLIPBOARD)
    // Yank the visual area into the GUI selection register before we operate
    // on it and lose it forever.
    // Don't do it if a specific register was specified, so that ""x"*P works.
    // This could call do_pending_operator() recursively, but that's OK
    // because gui_yank will be TRUE for the nested call.
    எனில் ((clip_star.available || clip_plus.available)
	    && oap->op_type != OP_NOP
	    && !gui_yank
	    && VIsual_active
	    && !redo_VIsual_busy
	    && oap->regname == 0)
	clip_auto_select();
#endif
    old_cursor = curwin->w_cursor;

    // If an operation is pending, handle it...
    எனில் ((finish_op || VIsual_active) && oap->op_type != OP_NOP)
    {
	// Yank can be redone when 'y' is in 'cpoptions', but not when yanking
	// for the clipboard.
	முழு	redo_yank = vim_strchr(p_cpo, CPO_YANK) != வெற்று && !gui_yank;

#ifdef FEAT_LINEBREAK
	// Avoid a problem with unwanted linebreaks in block mode.
	(வெறுமை)reset_lbr();
#endif
	oap->is_VIsual = VIsual_active;
	எனில் (oap->motion_force == 'V')
	    oap->motion_type = MLINE;
	இல்லையெனில் எனில் (oap->motion_force == 'v')
	{
	    // If the motion was linewise, "inclusive" will not have been set.
	    // Use "exclusive" to be consistent.  Makes "dvj" work nice.
	    எனில் (oap->motion_type == MLINE)
		oap->inclusive = FALSE;
	    // If the motion already was characterwise, toggle "inclusive"
	    இல்லையெனில் எனில் (oap->motion_type == MCHAR)
		oap->inclusive = !oap->inclusive;
	    oap->motion_type = MCHAR;
	}
	இல்லையெனில் எனில் (oap->motion_force == Ctrl_V)
	{
	    // Change line- or characterwise motion into Visual block mode.
	    எனில் (!VIsual_active)
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
	எனில் ((redo_yank || oap->op_type != OP_YANK)
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
	    எனில் (cap->cmdchar == '/' || cap->cmdchar == '?') // was a search
	    {
		// If 'cpoptions' does not contain 'r', insert the search
		// pattern to really repeat the same command.
		எனில் (vim_strchr(p_cpo, CPO_REDO) == வெற்று)
		    AppendToRedobuffLit(cap->searchbuf, -1);
		AppendToRedobuff(NL_STR);
	    }
	    இல்லையெனில் எனில் (is_ex_cmdchar(cap))
	    {
		// do_cmdline() has stored the first typed line in
		// "repeat_cmdline".  When several lines are typed repeating
		// won't be possible.
		எனில் (repeat_cmdline == வெற்று)
		    ResetRedobuff();
		இல்லையெனில்
		{
		    எனில் (cap->cmdchar == ':')
			AppendToRedobuffLit(repeat_cmdline, -1);
		    இல்லையெனில்
			AppendToRedobuffSpec(repeat_cmdline);
		    AppendToRedobuff(NL_STR);
		    VIM_CLEAR(repeat_cmdline);
		}
	    }
	}

	எனில் (redo_VIsual_busy)
	{
	    // Redo of an operation on a Visual area. Use the same size from
	    // redo_VIsual.rv_line_count and redo_VIsual.rv_vcol.
	    oap->start = curwin->w_cursor;
	    curwin->w_cursor.lnum += redo_VIsual.rv_வரி_மொத்தம் - 1;
	    எனில் (curwin->w_cursor.lnum > curbuf->b_ml.ml_வரி_மொத்தம்)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_வரி_மொத்தம்;
	    VIsual_mode = redo_VIsual.rv_mode;
	    எனில் (redo_VIsual.rv_vcol == MAXCOL || VIsual_mode == 'v')
	    {
		எனில் (VIsual_mode == 'v')
		{
		    எனில் (redo_VIsual.rv_வரி_மொத்தம் <= 1)
		    {
			validate_virtcol();
			curwin->w_curswant =
				     curwin->w_virtcol + redo_VIsual.rv_vcol - 1;
		    }
		    இல்லையெனில்
			curwin->w_curswant = redo_VIsual.rv_vcol;
		}
		இல்லையெனில்
		{
		    curwin->w_curswant = MAXCOL;
		}
		coladvance(curwin->w_curswant);
	    }
	    cap->count0 = redo_VIsual.rv_மொத்தம்;
	    எனில் (redo_VIsual.rv_மொத்தம் != 0)
		cap->count1 = redo_VIsual.rv_மொத்தம்;
	    இல்லையெனில்
		cap->count1 = 1;
	}
	இல்லையெனில் எனில் (VIsual_active)
	{
	    எனில் (!gui_yank)
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
	    எனில் (VIsual_select && VIsual_mode == 'V'
					    && cap->oap->op_type != OP_DELETE)
	    {
		எனில் (LT_POS(VIsual, curwin->w_cursor))
		{
		    VIsual.col = 0;
		    curwin->w_cursor.col = ml_get_len(curwin->w_cursor.lnum);
		}
		இல்லையெனில்
		{
		    curwin->w_cursor.col = 0;
		    VIsual.col = ml_get_len(VIsual.lnum);
		}
		VIsual_mode = 'v';
	    }
	    // If 'selection' is "exclusive", backup one character for
	    // charwise selections.
	    இல்லையெனில் எனில் (VIsual_mode == 'v')
		include_வரி_break = unadjust_for_sel();

	    oap->start = VIsual;
	    எனில் (VIsual_mode == 'V')
	    {
		oap->start.col = 0;
		oap->start.coladd = 0;
	    }
	}

	// Set oap->start to the first position of the operated text, oap->end
	// to the end of the operated text.  w_cursor is equal to oap->start.
	எனில் (LT_POS(oap->start, curwin->w_cursor))
	{
#ifdef FEAT_FOLDING
	    // Include folded lines completely.
	    எனில் (!VIsual_active)
	    {
		எனில் (hasFolding(oap->start.lnum, &oap->start.lnum, வெற்று))
		    oap->start.col = 0;
		எனில் ((curwin->w_cursor.col > 0 || oap->inclusive
						  || oap->motion_type == MLINE)
			&& hasFolding(curwin->w_cursor.lnum, வெற்று,
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
	இல்லையெனில்
	{
#ifdef FEAT_FOLDING
	    // Include folded lines completely.
	    எனில் (!VIsual_active && oap->motion_type == MLINE)
	    {
		எனில் (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum,
									வெற்று))
		    curwin->w_cursor.col = 0;
		எனில் (hasFolding(oap->start.lnum, வெற்று, &oap->start.lnum))
		    oap->start.col = ml_get_len(oap->start.lnum);
	    }
#endif
	    oap->end = oap->start;
	    oap->start = curwin->w_cursor;
	}

	// Just in case lines were deleted that make the position invalid.
	check_pos(curwin->w_buffer, &oap->end);
	oap->வரி_மொத்தம் = oap->end.lnum - oap->start.lnum + 1;

	// Set "virtual_op" before resetting VIsual_active.
	virtual_op = virtual_active();

	எனில் (VIsual_active || redo_VIsual_busy)
	{
	    get_op_vcol(oap, redo_VIsual.rv_vcol, TRUE);

	    எனில் (!redo_VIsual_busy && !gui_yank)
	    {
		// Prepare to reselect and redo Visual: this is based on the
		// size of the Visual text
		resel_VIsual_mode = VIsual_mode;
		எனில் (curwin->w_curswant == MAXCOL)
		    resel_VIsual_vcol = MAXCOL;
		இல்லையெனில்
		{
		    எனில் (VIsual_mode != Ctrl_V)
			getvvcol(curwin, &(oap->end),
						வெற்று, வெற்று, &oap->end_vcol, 0);
		    எனில் (VIsual_mode == Ctrl_V || oap->வரி_மொத்தம் <= 1)
		    {
			எனில் (VIsual_mode != Ctrl_V)
			    getvvcol(curwin, &(oap->start),
					      &oap->start_vcol, வெற்று, வெற்று, 0);
			resel_VIsual_vcol = oap->end_vcol - oap->start_vcol + 1;
		    }
		    இல்லையெனில்
			resel_VIsual_vcol = oap->end_vcol;
		}
		resel_VIsual_வரி_மொத்தம் = oap->வரி_மொத்தம்;
	    }

	    // can't redo yank (unless 'y' is in 'cpoptions') and ":"
	    எனில் ((redo_yank || oap->op_type != OP_YANK)
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
		எனில் (cap->cmdchar == 'g' && (cap->nchar == 'n'
							|| cap->nchar == 'N'))
		    prep_redo(oap->regname, cap->count0,
			    get_op_char(oap->op_type),
			    get_extra_op_char(oap->op_type),
			    oap->motion_force, cap->cmdchar, cap->nchar);
		இல்லையெனில் எனில் (!is_ex_cmdchar(cap))
		{
		    முழு opchar = get_op_char(oap->op_type);
		    முழு extra_opchar = get_extra_op_char(oap->op_type);
		    முழு nchar = oap->op_type == OP_REPLACE ? cap->nchar : NUL;

		    // reverse what nv_replace() did
		    எனில் (nchar == REPLACE_CR_NCHAR)
			nchar = CAR;
		    இல்லையெனில் எனில் (nchar == REPLACE_NL_NCHAR)
			nchar = NL;

		    எனில் (opchar == 'g' && extra_opchar == '@')
			// also repeat the count for 'operatorfunc'
			prep_redo_num2(oap->regname, 0L, NUL, 'v',
				     cap->count0, opchar, extra_opchar, nchar);
		    இல்லையெனில்
			prep_redo(oap->regname, 0L, NUL, 'v',
						  opchar, extra_opchar, nchar);
		}
		எனில் (!redo_VIsual_busy)
		{
		    redo_VIsual.rv_mode = resel_VIsual_mode;
		    redo_VIsual.rv_vcol = resel_VIsual_vcol;
		    redo_VIsual.rv_வரி_மொத்தம் = resel_VIsual_வரி_மொத்தம்;
		    redo_VIsual.rv_மொத்தம் = cap->count0;
		    redo_VIsual.rv_arg = cap->arg;
		}
	    }

	    // oap->inclusive defaults to TRUE.
	    // If oap->end is on a NUL (empty line) oap->inclusive becomes
	    // FALSE.  This makes "d}P" and "v}dP" work the same.
	    எனில் (oap->motion_force == NUL || oap->motion_type == MLINE)
		oap->inclusive = TRUE;
	    எனில் (VIsual_mode == 'V')
		oap->motion_type = MLINE;
	    இல்லையெனில்
	    {
		oap->motion_type = MCHAR;
		எனில் (VIsual_mode != Ctrl_V && *ml_get_pos(&(oap->end)) == NUL
			&& (include_வரி_break || !virtual_op))
		{
		    oap->inclusive = FALSE;
		    // Try to include the newline, unless it's an operator
		    // that works on lines only.
		    எனில் (*p_sel != 'o'
			    && !op_on_வரிகள்(oap->op_type)
			    && oap->end.lnum < curbuf->b_ml.ml_வரி_மொத்தம்)
		    {
			++oap->end.lnum;
			oap->end.col = 0;
			oap->end.coladd = 0;
			++oap->வரி_மொத்தம்;
		    }
		}
	    }

	    redo_VIsual_busy = FALSE;

	    // Switch Visual off now, so screen updating does
	    // not show inverted text when the screen is redrawn.
	    // With OP_YANK and sometimes with OP_COLON and OP_FILTER there is
	    // no screen redraw, so it is done here to remove the inverted
	    // part.
	    எனில் (!gui_yank)
	    {
		VIsual_active = FALSE;
		setmouse();
		mouse_dragging = 0;
		may_clear_cmdline();
		எனில் ((oap->op_type == OP_YANK
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
	எனில் (has_mbyte && oap->inclusive)
	{
	    முழு		l;

	    l = (*mb_ptr2len)(ml_get_pos(&oap->end));
	    எனில் (l > 1)
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
				&& vim_strchr(p_cpo, CPO_EMPTYREGION) != வெற்று);

	// Force a redraw when operating on an empty Visual region, when
	// 'modifiable is off or creating a fold.
	எனில் (oap->is_VIsual && (oap->empty || !curbuf->b_p_ma
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
	எனில் (	   oap->motion_type == MCHAR
		&& oap->inclusive == FALSE
		&& !(cap->retval & CA_NO_ADJ_OP_END)
		&& oap->end.col == 0
		&& (!oap->is_VIsual || *p_sel == 'o')
		&& !oap->block_mode
		&& oap->வரி_மொத்தம் > 1)
	{
	    oap->end_adjusted = TRUE;	    // remember that we did this
	    --oap->வரி_மொத்தம்;
	    --oap->end.lnum;
	    எனில் (inindent(0))
		oap->motion_type = MLINE;
	    இல்லையெனில்
	    {
		oap->end.col = ml_get_len(oap->end.lnum);
		எனில் (oap->end.col)
		{
		    --oap->end.col;
		    oap->inclusive = TRUE;
		}
	    }
	}
	இல்லையெனில்
	    oap->end_adjusted = FALSE;

	தேர்வு (oap->op_type)
	{
	நிலை OP_LSHIFT:
	நிலை OP_RSHIFT:
	    op_shift(oap, TRUE, oap->is_VIsual ? (முழு)cap->count1 : 1);
	    auto_format(FALSE, TRUE);
	    நிறுத்து;

	நிலை OP_JOIN_NS:
	நிலை OP_JOIN:
	    எனில் (oap->வரி_மொத்தம் < 2)
		oap->வரி_மொத்தம் = 2;
	    எனில் (curwin->w_cursor.lnum + oap->வரி_மொத்தம் - 1 >
						   curbuf->b_ml.ml_வரி_மொத்தம்)
		beep_flush();
	    இல்லையெனில்
	    {
		(வெறுமை)do_join(oap->வரி_மொத்தம், oap->op_type == OP_JOIN,
							    TRUE, TRUE, TRUE);
		auto_format(FALSE, TRUE);
	    }
	    நிறுத்து;

	நிலை OP_DELETE:
	    VIsual_reselect = FALSE;	    // don't reselect now
	    எனில் (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    இல்லையெனில்
	    {
		(வெறுமை)op_delete(oap);
		// save cursor line for undo if it wasn't saved yet
		எனில் (oap->motion_type == MLINE && has_format_option(FO_AUTO)
						      && u_save_cursor() == OK)
		    auto_format(FALSE, TRUE);
	    }
	    நிறுத்து;

	நிலை OP_YANK:
	    எனில் (empty_region_error)
	    {
		எனில் (!gui_yank)
		{
		    vim_beep(BO_OPER);
		    CancelRedo();
		}
	    }
	    இல்லையெனில்
	    {
#ifdef FEAT_LINEBREAK
		restore_lbr(lbr_saved);
#endif
		oap->excl_tr_ws = cap->cmdchar == 'z';
		(வெறுமை)op_yank(oap, FALSE, !gui_yank);
	    }
	    check_cursor_col();
	    நிறுத்து;

	நிலை OP_CHANGE:
	    VIsual_reselect = FALSE;	    // don't reselect now
	    எனில் (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    இல்லையெனில்
	    {
		// This is a new edit command, not a restart.  Need to
		// remember it to make 'insertmode' work with mappings for
		// Visual mode.  But do this only once and not when typed and
		// 'insertmode' isn't set.
		எனில் (p_im || !சாவி_Typed)
		    restart_edit_save = restart_edit;
		இல்லையெனில்
		    restart_edit_save = 0;
		restart_edit = 0;
#ifdef FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#endif
		// trigger TextChangedI
		curbuf->b_last_changedtick_i = CHANGEDTICK(curbuf);

		எனில் (op_change(oap))	// will call edit()
		    cap->retval |= CA_COMMAND_BUSY;
		எனில் (restart_edit == 0)
		    restart_edit = restart_edit_save;
	    }
	    நிறுத்து;

	நிலை OP_FILTER:
	    எனில் (vim_strchr(p_cpo, CPO_FILTER) != வெற்று)
		AppendToRedobuff((char_u *)"!\r");  // use any last used !cmd
	    இல்லையெனில்
		bangredo = TRUE;    // do_bang() will put cmd in redo buffer
	    // FALLTHROUGH

	நிலை OP_INDENT:
	நிலை OP_COLON:

	    // If 'equalprg' is empty, do the indenting internally.
	    எனில் (oap->op_type == OP_INDENT && *get_equalprg() == NUL)
	    {
		எனில் (curbuf->b_p_lisp)
		{
#ifdef FEAT_EVAL
		    எனில் (use_indentexpr_for_lisp())
			op_reindent(oap, get_கோவை_indent);
		    இல்லையெனில்
#endif
			op_reindent(oap, get_lisp_indent);
		    நிறுத்து;
		}
		op_reindent(oap,
#ifdef FEAT_EVAL
			*curbuf->b_p_inde != NUL ? get_கோவை_indent :
#endif
			    get_c_indent);
		நிறுத்து;
	    }

	    op_colon(oap);
	    நிறுத்து;

	நிலை OP_TILDE:
	நிலை OP_UPPER:
	நிலை OP_LOWER:
	நிலை OP_ROT13:
	    எனில் (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    இல்லையெனில்
		op_tilde(oap);
	    check_cursor_col();
	    நிறுத்து;

	நிலை OP_FORMAT:
#எனில் defined(FEAT_EVAL)
	    எனில் (*curbuf->b_p_fex != NUL)
		op_formatexpr(oap);	// use expression
	    இல்லையெனில்
#endif
	    {
		எனில் (*p_fp != NUL || *curbuf->b_p_fp != NUL)
		    op_colon(oap);		// use external command
		இல்லையெனில்
		    op_format(oap, FALSE);	// use internal function
	    }
	    நிறுத்து;
	நிலை OP_FORMAT2:
	    op_format(oap, TRUE);	// use internal function
	    நிறுத்து;

	நிலை OP_FUNCTION:
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
		நிறுத்து;
	    }

	நிலை OP_செருகு:
	நிலை OP_APPEND:
	    VIsual_reselect = FALSE;	// don't reselect now
	    எனில் (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    இல்லையெனில்
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

		op_செருகு(oap, cap->count1);
#ifdef FEAT_LINEBREAK
		// Reset linebreak, so that formatting works correctly.
		(வெறுமை)reset_lbr();
#endif

		// TODO: when inserting in several lines, should format all
		// the lines.
		auto_format(FALSE, TRUE);

		எனில் (restart_edit == 0)
		    restart_edit = restart_edit_save;
		இல்லையெனில்
		    cap->retval |= CA_COMMAND_BUSY;
	    }
	    நிறுத்து;

	நிலை OP_REPLACE:
	    VIsual_reselect = FALSE;	// don't reselect now
	    எனில் (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    இல்லையெனில்
	    {
#ifdef FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#endif
		op_replace(oap, cap->nchar);
	    }
	    நிறுத்து;

#ifdef FEAT_FOLDING
	நிலை OP_FOLD:
	    VIsual_reselect = FALSE;	// don't reselect now
	    foldCreate(oap->start.lnum, oap->end.lnum);
	    நிறுத்து;

	நிலை OP_FOLDOPEN:
	நிலை OP_FOLDOPENREC:
	நிலை OP_FOLDCLOSE:
	நிலை OP_FOLDCLOSEREC:
	    VIsual_reselect = FALSE;	// don't reselect now
	    opFoldRange(oap->start.lnum, oap->end.lnum,
		    oap->op_type == OP_FOLDOPEN
					    || oap->op_type == OP_FOLDOPENREC,
		    oap->op_type == OP_FOLDOPENREC
					  || oap->op_type == OP_FOLDCLOSEREC,
					  oap->is_VIsual);
	    நிறுத்து;

	நிலை OP_FOLDDEL:
	நிலை OP_FOLDDELREC:
	    VIsual_reselect = FALSE;	// don't reselect now
	    deleteFold(oap->start.lnum, oap->end.lnum,
			       oap->op_type == OP_FOLDDELREC, oap->is_VIsual);
	    நிறுத்து;
#endif
	நிலை OP_NR_ADD:
	நிலை OP_NR_SUB:
	    எனில் (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    இல்லையெனில்
	    {
		VIsual_active = TRUE;
#ifdef FEAT_LINEBREAK
		restore_lbr(lbr_saved);
#endif
		op_addsub(oap, cap->count1, redo_VIsual.rv_arg);
		VIsual_active = FALSE;
	    }
	    check_cursor_col();
	    நிறுத்து;
	இயல்பு:
	    clearopbeep(oap);
	}
	virtual_op = MAYBE;
	எனில் (!gui_yank)
	{
	    // if 'sol' not set, go back to old column for some commands
	    எனில் (!p_sol && oap->motion_type == MLINE && !oap->end_adjusted
		    && (oap->op_type == OP_LSHIFT || oap->op_type == OP_RSHIFT
						|| oap->op_type == OP_DELETE))
	    {
#ifdef FEAT_LINEBREAK
		(வெறுமை)reset_lbr();
#endif
		coladvance(curwin->w_curswant = old_col);
	    }
	}
	இல்லையெனில்
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
    நிலையான வெறுமை
pbyte(pos_T lp, முழு c)
{
    char_u *p = ml_get_buf(curbuf, lp.lnum, TRUE);
    முழு	len = curbuf->b_ml.ml_வரி_len;

    // safety check
    எனில் (lp.col >= len)
	lp.col = (len > 1 ? len - 2 : 0);
    *(p + lp.col) = c;
}
