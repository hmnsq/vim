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

#ضمن "vim.h"

ساكن فراغ shift_block(oparg_T *oap, صحيح amount);
ساكن فراغ	mb_adjust_opend(oparg_T *oap);
ساكن صحيح	do_addsub(صحيح op_type, pos_T *pos, صحيح length, linenr_T Prenum1);
ساكن فراغ	pbyte(pos_T lp, صحيح c);
#define PBYTE(lp, c) pbyte(lp, c)


// Flags for third item in "opchars".
#define OPF_LINES  1	// operator always works on lines
#define OPF_CHANGE 2	// operator changes text

/*
 * The names of operators.
 * IMPORTANT: Index must correspond with defines in vim.h!!!
 * The third field holds OPF_ flags.
 */
ساكن ثابت حرف opchars[][3] =
{
    {NUL, NUL, 0},			// OP_NOP
    {'d', NUL, OPF_CHANGE},		// OP_DELETE
    {'y', NUL, 0},			// OP_YANK
    {'c', NUL, OPF_CHANGE},		// OP_CHANGE
    {'<', NUL, OPF_LINES | OPF_CHANGE},	// OP_LSHIFT
    {'>', NUL, OPF_LINES | OPF_CHANGE},	// OP_RSHIFT
    {'!', NUL, OPF_LINES | OPF_CHANGE},	// OP_FILTER
    {'g', '~', OPF_CHANGE},		// OP_TILDE
    {'=', NUL, OPF_LINES | OPF_CHANGE},	// OP_INDENT
    {'g', 'q', OPF_LINES | OPF_CHANGE},	// OP_FORMAT
    {':', NUL, OPF_LINES},		// OP_COLON
    {'g', 'U', OPF_CHANGE},		// OP_UPPER
    {'g', 'u', OPF_CHANGE},		// OP_LOWER
    {'J', NUL, OPF_LINES | OPF_CHANGE},	// DO_JOIN
    {'g', 'J', OPF_LINES | OPF_CHANGE},	// DO_JOIN_NS
    {'g', '?', OPF_CHANGE},		// OP_ROT13
    {'r', NUL, OPF_CHANGE},		// OP_REPLACE
    {'I', NUL, OPF_CHANGE},		// OP_INSERT
    {'A', NUL, OPF_CHANGE},		// OP_APPEND
    {'z', 'f', OPF_LINES},		// OP_FOLD
    {'z', 'o', OPF_LINES},		// OP_FOLDOPEN
    {'z', 'O', OPF_LINES},		// OP_FOLDOPENREC
    {'z', 'c', OPF_LINES},		// OP_FOLDCLOSE
    {'z', 'C', OPF_LINES},		// OP_FOLDCLOSEREC
    {'z', 'd', OPF_LINES},		// OP_FOLDDEL
    {'z', 'D', OPF_LINES},		// OP_FOLDDELREC
    {'g', 'w', OPF_LINES | OPF_CHANGE},	// OP_FORMAT2
    {'g', '@', OPF_CHANGE},		// OP_FUNCTION
    {Ctrl_A, NUL, OPF_CHANGE},		// OP_NR_ADD
    {Ctrl_X, NUL, OPF_CHANGE},		// OP_NR_SUB
};

/*
 * Translate a command name into an operator type.
 * Must only be called with a valid operator name!
 */
    صحيح
get_op_type(صحيح char1, صحيح char2)
{
    صحيح		i;

    𐡄𐡍 (char1 == 'r')		// ignore second character
	أرجع OP_REPLACE;
    𐡄𐡍 (char1 == '~')		// when tilde is an operator
	أرجع OP_TILDE;
    𐡄𐡍 (char1 == 'g' && char2 == Ctrl_A)	// add
	أرجع OP_NR_ADD;
    𐡄𐡍 (char1 == 'g' && char2 == Ctrl_X)	// subtract
	أرجع OP_NR_SUB;
    𐡄𐡍 (char1 == 'z' && char2 == 'y')	// OP_YANK
	أرجع OP_YANK;
    لكل (i = 0; ; ++i)
    {
	𐡄𐡍 (opchars[i][0] == char1 && opchars[i][1] == char2)
	    اكسر;
	𐡄𐡍 (i == (صحيح)ARRAY_LENGTH(opchars) - 1)
	{
	    internal_error("get_op_type()");
	    اكسر;
	}
    }
    أرجع i;
}

/*
 * Return TRUE if operator "op" always works on whole lines.
 */
    ساكن صحيح
op_on_lines(صحيح op)
{
    أرجع opchars[op][2] & OPF_LINES;
}

#𐡄𐡍 defined(FEAT_JOB_CHANNEL)
/*
 * Return TRUE if operator "op" changes text.
 */
    صحيح
op_is_change(صحيح op)
{
    أرجع opchars[op][2] & OPF_CHANGE;
}
#endif

/*
 * Get first operator command character.
 * Returns 'g' or 'z' if there is another command character.
 */
    صحيح
get_op_char(صحيح optype)
{
    أرجع opchars[optype][0];
}

/*
 * Get second operator command character.
 */
    صحيح
get_extra_op_char(صحيح optype)
{
    أرجع opchars[optype][1];
}

/*
 * op_shift - handle a shift operation
 */
    فراغ
op_shift(oparg_T *oap, صحيح curs_top, صحيح amount)
{
    long	    i;
    صحيح		    first_char;
    صحيح		    block_col = 0;

    𐡄𐡍 (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	أرجع;

    𐡄𐡍 (oap->block_mode)
	block_col = curwin->w_cursor.col;

    لكل (i = oap->line_count; --i >= 0; )
    {
	first_char = *ml_get_curline();
	𐡄𐡍 (first_char == NUL)				// empty line
	    curwin->w_cursor.col = 0;
	وإلا 𐡄𐡍 (oap->block_mode)
	    shift_block(oap, amount);
	وإلا
	    // Move the line right if it doesn't start with '#', 'smartindent'
	    // isn't set or 'cindent' isn't set or '#' isn't in 'cino'.
	    𐡄𐡍 (first_char != '#' || !preprocs_left())
		shift_line(oap->op_type == OP_LSHIFT, p_sr, amount, FALSE);
	++curwin->w_cursor.lnum;
    }

    changed_lines(oap->start.lnum, 0, oap->end.lnum + 1, 0L);
    𐡄𐡍 (oap->block_mode)
    {
	curwin->w_cursor.lnum = oap->start.lnum;
	curwin->w_cursor.col = block_col;
    }
    وإلا 𐡄𐡍 (curs_top)	    // put cursor on first line, for ">>"
    {
	curwin->w_cursor.lnum = oap->start.lnum;
	beginline(BL_SOL | BL_FIX);   // shift_line() may have set cursor.col
    }
    وإلا
	--curwin->w_cursor.lnum;	// put cursor on last line, for ":>"

#ifdef FEAT_FOLDING
    // The cursor line is not in a closed fold
    foldOpenCursor();
#endif


    𐡄𐡍 (oap->line_count > p_report)
    {
	حرف	    *op;
	حرف	    *msg_line_single;
	حرف	    *msg_line_plural;

	𐡄𐡍 (oap->op_type == OP_RSHIFT)
	    op = ">";
	وإلا
	    op = "<";
	msg_line_single = NGETTEXT("%ld line %sed %d time",
					     "%ld line %sed %d times", amount);
	msg_line_plural = NGETTEXT("%ld lines %sed %d time",
					    "%ld lines %sed %d times", amount);
	vim_snprintf((حرف *)IObuff, IOSIZE,
		NGETTEXT(msg_line_single, msg_line_plural, oap->line_count),
		oap->line_count, op, amount);
	msg_attr_keep((حرف *)IObuff, 0, TRUE);
    }

    𐡄𐡍 ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end.lnum = oap->end.lnum;
	curbuf->b_op_end.col = ml_get_len(oap->end.lnum);
	𐡄𐡍 (curbuf->b_op_end.col > 0)
	    --curbuf->b_op_end.col;
    }
}

#ifdef FEAT_VARTABS
/*
 * Return the tabstop width at the index of the variable tabstop array.  If an
 * index greater than the length of the array is given, the last tabstop width
 * in the array is returned.
 */
    ساكن صحيح
get_vts(صحيح *vts_array, صحيح index)
{
    صحيح	ts;

    𐡄𐡍 (index < 1)
	ts = 0;
    وإلا 𐡄𐡍 (index <= vts_array[0])
	ts = vts_array[index];
    وإلا
	ts = vts_array[vts_array[0]];

    أرجع ts;
}

/*
 * Return the sum of all the tabstops through the index-th.
 */
    ساكن صحيح
get_vts_sum(صحيح *vts_array, صحيح index)
{
    صحيح	sum = 0;
    صحيح	i;

    // Perform the summation for indices within the actual array.
    لكل (i = 1; i <= index && i <= vts_array[0]; i++)
	sum += vts_array[i];

    // Add tabstops whose indices exceed the actual array.
    𐡄𐡍 (i <= index)
	sum += vts_array[vts_array[0]] * (index - vts_array[0]);

    أرجع sum;
}
#endif

    ساكن vimlong_T
get_new_sw_indent(
    صحيح		left,		// TRUE if shift is to the left
    صحيح		round,		// TRUE if new indent is to be to a tabstop
    vimlong_T	amount,		// Number of shifts
    vimlong_T	sw_val)
{
    vimlong_T	count = get_indent();
    vimlong_T	i, j;

    𐡄𐡍 (round)			// round off indent
    {
	i = trim_to_int(count) / sw_val;	// number of 'shiftwidth' rounded down
	j = trim_to_int(count) % sw_val;	// extra spaces
	𐡄𐡍 (j && left)		// first remove extra spaces
	    --amount;
	𐡄𐡍 (left)
	{
	    i -= amount;
	    𐡄𐡍 (i < 0)
		i = 0;
	}
	وإلا
	    i += amount;
	count = i * sw_val;
    }
    وإلا			// original vi indent
    {
	𐡄𐡍 (left)
	{
	    count -= sw_val * amount;
	    𐡄𐡍 (count < 0)
		count = 0;
	}
	وإلا
	    count += sw_val * amount;
    }

    أرجع count;
}

#ifdef FEAT_VARTABS
    ساكن vimlong_T
get_new_vts_indent(
    صحيح	left,			// TRUE if shift is to the left
    صحيح	round,			// TRUE if new indent is to be to a tabstop
    صحيح	amount,			// Number of shifts
    صحيح	*vts_array)
{
    vimlong_T	indent = get_indent();
    صحيح		vtsi = 0;
    صحيح		vts_indent = 0;
    صحيح		ts = 0;		// Silence uninitialized variable warning.
    صحيح		offset;		// Extra indent spaces to the right of the
				// tabstop

    // Find the tabstop at or to the left of the current indent.
    طالما (vts_indent <= indent)
    {
	vtsi++;
	ts = get_vts(vts_array, vtsi);
	vts_indent += ts;
    }
    vts_indent -= ts;
    vtsi--;

    offset = indent - vts_indent;

    𐡄𐡍 (round)
    {
	𐡄𐡍 (left)
	{
	    𐡄𐡍 (offset == 0)
		indent = get_vts_sum(vts_array, vtsi - amount);
	    وإلا
		indent = get_vts_sum(vts_array, vtsi - (amount - 1));
	}
	وإلا
	    indent = get_vts_sum(vts_array, vtsi + amount);
    }
    وإلا
    {
	𐡄𐡍 (left)
	{
	    𐡄𐡍 (amount > vtsi)
		indent = 0;
	    وإلا
		indent = get_vts_sum(vts_array, vtsi - amount) + offset;
	}
	وإلا
	    indent = get_vts_sum(vts_array, vtsi + amount) + offset;
    }

    أرجع indent;
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
    فراغ
shift_line(
    صحيح	left,			// TRUE if shift is to the left
    صحيح	round,			// TRUE if new indent is to be to a tabstop
    صحيح	amount,			// Number of shifts
    صحيح	call_changed_bytes)	// call changed_bytes()
{
    vimlong_T	count;
    long	sw_val = curbuf->b_p_sw;
    long	ts_val = curbuf->b_p_ts;
#ifdef FEAT_VARTABS
    صحيح		*vts_array = curbuf->b_p_vts_array;
#endif

    𐡄𐡍 (sw_val != 0)
	// 'shiftwidth' is not zero; use it as the shift size.
	count = get_new_sw_indent(left, round, amount, sw_val);
    وإلا
#ifdef FEAT_VARTABS
	𐡄𐡍 ((vts_array == عدم) || (vts_array[0] == 0))
#endif
    {
	// 'shiftwidth is zero and 'vartabstop' is empty; use 'tabstop' as the
	// shift size.
	count = get_new_sw_indent(left, round, amount, ts_val);
    }
#ifdef FEAT_VARTABS
    وإلا
    {
	// 'shiftwidth is zero and 'vartabstop' is defined; use 'vartabstop'
	// to determine the new indent.
	count = get_new_vts_indent(left, round, amount, vts_array);
    }
#endif

    // Set new indent
    𐡄𐡍 (State & VREPLACE_FLAG)
	change_indent(INDENT_SET, trim_to_int(count), FALSE, NUL, call_changed_bytes);
    وإلا
	(فراغ)set_indent(trim_to_int(count), call_changed_bytes ? SIN_CHANGED : 0);
}

/*
 * Shift one line of the current block one shiftwidth right or left.
 * Leaves cursor on first character in block.
 */
    ساكن فراغ
shift_block(oparg_T *oap, صحيح amount)
{
    صحيح			left = (oap->op_type == OP_LSHIFT);
    صحيح			oldstate = State;
    صحيح			total;
    char_u		*newp, *oldp;
    size_t		newlen, oldlen;
    صحيح			oldcol = curwin->w_cursor.col;
    صحيح			sw_val = (صحيح)get_sw_value_indent(curbuf, left);
    صحيح			ts_val = (صحيح)curbuf->b_p_ts;
    بنية block_def	bd;
    صحيح			incr;
    colnr_T		ws_vcol;
    صحيح			added;
    size_t		new_line_len;	// the length of the line after the
					// block shift
#ifdef FEAT_RIGHTLEFT
    صحيح			old_p_ri = p_ri;

    p_ri = 0;			// don't want revins in indent
#endif

    State = MODE_INSERT;	// don't want MODE_REPLACE for State
    block_prep(oap, &bd, curwin->w_cursor.lnum, TRUE);
    𐡄𐡍 (bd.is_short)
	أرجع;

    // total is number of screen columns to be inserted/removed
    total = (صحيح)((unsigned)amount * (unsigned)sw_val);
    𐡄𐡍 ((total / sw_val) != amount)
	أرجع; // multiplication overflow

    oldp = ml_get_curline();
    oldlen = ml_get_curline_len();

    𐡄𐡍 (!left)
    {
	صحيح		tabs = 0, spaces = 0;
	chartabsize_T	cts;

	/*
	 *  1. Get start vcol
	 *  2. Total ws vcols
	 *  3. Divvy into TABs & spp
	 *  4. Construct new string
	 */
	total += bd.pre_whitesp; // all virtual WS up to & incl a split TAB
	ws_vcol = bd.start_vcol - bd.pre_whitesp;
	𐡄𐡍 (bd.startspaces)
	{
	    𐡄𐡍 (has_mbyte)
	    {
		𐡄𐡍 ((*mb_ptr2len)(bd.textstart) == 1)
		    ++bd.textstart;
		وإلا
		{
		    ws_vcol = 0;
		    bd.startspaces = 0;
		}
	    }
	    وإلا
		++bd.textstart;
	}

	// TODO: is passing bd.textstart for start of the line OK?
	init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum,
				   bd.start_vcol, bd.textstart, bd.textstart);
	لكل ( ; VIM_ISWHITE(*cts.cts_ptr); )
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
	𐡄𐡍 (!curbuf->b_p_et)
	    tabstop_fromto(ws_vcol, ws_vcol + total,
				ts_val, curbuf->b_p_vts_array, &tabs, &spaces);
	وإلا
	    spaces = total;
#وإلا
	𐡄𐡍 (!curbuf->b_p_et)
	    tabs = ((ws_vcol % ts_val) + total) / ts_val; // number of tabs
	𐡄𐡍 (tabs > 0)
	    spaces = ((ws_vcol % ts_val) + total) % ts_val; // number of spp
	وإلا
	    spaces = total;
#endif
	// if we're splitting a TAB, allow for it
	bd.textcol -= bd.pre_whitesp_c - (bd.startspaces != 0);

	new_line_len = bd.textcol + tabs + spaces + (oldlen - (bd.textstart - oldp));
	newp = alloc(new_line_len + 1);
	𐡄𐡍 (newp == عدم)
	    أرجع;
	mch_memmove(newp, oldp, (size_t)bd.textcol);
	newlen = bd.textcol;
	vim_memset(newp + newlen, TAB, (size_t)tabs);
	newlen += tabs;
	vim_memset(newp + newlen, ' ', (size_t)spaces);
	STRCPY(newp + newlen + spaces, bd.textstart);
    }
    وإلا // left
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
	𐡄𐡍 (bd.startspaces)
	    MB_PTR_ADV(non_white);

	// The character's column is in "bd.start_vcol".
	non_white_col = bd.start_vcol;

	init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum,
				   non_white_col, bd.textstart, non_white);
	طالما (VIM_ISWHITE(*cts.cts_ptr))
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
	𐡄𐡍 (bd.startspaces)
	    verbatim_copy_width -= bd.start_char_vcols;
	init_chartabsize_arg(&cts, curwin, 0, verbatim_copy_width,
					     bd.textstart, verbatim_copy_end);
	طالما (cts.cts_vcol < destination_col)
	{
	    incr = lbr_chartabsize(&cts);
	    𐡄𐡍 (cts.cts_vcol + incr > destination_col)
		اكسر;
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
	new_line_len = fixedlen + fill + (oldlen - (non_white - oldp));

	newp = alloc(new_line_len + 1);
	𐡄𐡍 (newp == عدم)
	    أرجع;
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
    𐡄𐡍 (new_line_len >= oldlen)
	added = (صحيح)(new_line_len - oldlen);
    وإلا
	added = 0 - (صحيح)(oldlen - new_line_len);
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
    ساكن فراغ
block_insert(
    oparg_T		*oap,
    char_u		*s,
    size_t		slen,
    صحيح			b_insert,
    بنية block_def	*bdp)
{
    صحيح		ts_val;
    صحيح		count = 0;	// extra spaces to replace a cut TAB
    صحيح		spaces = 0;	// non-zero if cutting a TAB
    colnr_T	offset;		// pointer along new line
    colnr_T	startcol;	// column where insert starts
    char_u	*newp, *oldp;	// new, old lines
    linenr_T	lnum;		// loop var
    صحيح		oldstate = State;

    State = MODE_INSERT;	// don't want MODE_REPLACE for State

    لكل (lnum = oap->start.lnum + 1; lnum <= oap->end.lnum; lnum++)
    {
	block_prep(oap, bdp, lnum, TRUE);
	𐡄𐡍 (bdp->is_short && b_insert)
	    استمر;	// OP_INSERT, line ends before block start

	oldp = ml_get(lnum);

	𐡄𐡍 (b_insert)
	{
	    ts_val = bdp->start_char_vcols;
	    spaces = bdp->startspaces;
	    𐡄𐡍 (spaces != 0)
		count = ts_val - 1; // we're cutting a TAB
	    offset = bdp->textcol;
	}
	وإلا // append
	{
	    ts_val = bdp->end_char_vcols;
	    𐡄𐡍 (!bdp->is_short) // spaces = padding after block
	    {
		spaces = (bdp->endspaces ? ts_val - bdp->endspaces : 0);
		𐡄𐡍 (spaces != 0)
		    count = ts_val - 1; // we're cutting a TAB
		offset = bdp->textcol + bdp->textlen - (spaces != 0);
	    }
	    وإلا // spaces = padding to block edge
	    {
		// if $ used, just append to EOL (ie spaces==0)
		𐡄𐡍 (!bdp->is_MAX)
		    spaces = (oap->end_vcol - bdp->end_vcol) + 1;
		count = spaces;
		offset = bdp->textcol + bdp->textlen;
	    }
	}

	𐡄𐡍 (has_mbyte && spaces > 0)
	    // avoid copying part of a multi-byte character
	    offset -= (*mb_head_off)(oldp, oldp + offset);

	𐡄𐡍 (spaces < 0)  // can happen when the cursor was moved
	    spaces = 0;

	// Make sure the allocated size matches what is actually copied below.
	newp = alloc(ml_get_len(lnum) + spaces + slen
		    + (spaces > 0 && !bdp->is_short ? ts_val - spaces : 0)
								  + count + 1);
	𐡄𐡍 (newp == عدم)
	    استمر;

	// copy up to shifted part
	mch_memmove(newp, oldp, (size_t)offset);
	oldp += offset;

	// insert pre-padding
	vim_memset(newp + offset, ' ', (size_t)spaces);
	startcol = offset + spaces;

	// copy the new text
	mch_memmove(newp + startcol, s, slen);
	offset += (صحيح)slen;

	𐡄𐡍 (spaces > 0 && !bdp->is_short)
	{
	    𐡄𐡍 (*oldp == TAB)
	    {
		// insert post-padding
		vim_memset(newp + offset + spaces, ' ',
						    (size_t)(ts_val - spaces));
		// we're splitting a TAB, don't copy it
		oldp++;
		// We allowed for that TAB, remember this now
		count++;
	    }
	    وإلا
		// Not a TAB, no extra spaces
		count = spaces;
	}

	𐡄𐡍 (spaces > 0)
	    offset += count;
	STRCPY(newp + offset, oldp);

	ml_replace(lnum, newp, FALSE);

	𐡄𐡍 (b_insert)
	    // correct any text properties
	    inserted_bytes(lnum, startcol, (صحيح)slen);

	𐡄𐡍 (lnum == oap->end.lnum)
	{
	    // Set "']" mark to the end of the block instead of the end of
	    // the insert in the first line.
	    curbuf->b_op_end.lnum = oap->end.lnum;
	    curbuf->b_op_end.col = offset;
	    𐡄𐡍 (curbuf->b_visual.vi_end.coladd)
	    {
		curbuf->b_visual.vi_end.col += curbuf->b_visual.vi_end.coladd;
		curbuf->b_visual.vi_end.coladd = 0;
	    }
	}
    } // for all lnum

    changed_lines(oap->start.lnum + 1, 0, oap->end.lnum + 1, 0L);

    State = oldstate;
}

/*
 * Handle a delete operation.
 *
 * Return FAIL if undo failed, OK otherwise.
 */
    صحيح
op_delete(oparg_T *oap)
{
    صحيح			n;
    linenr_T		lnum;
    char_u		*ptr;
    char_u		*newp, *oldp;
    بنية block_def	bd;
    linenr_T		old_lcount = curbuf->b_ml.ml_line_count;
    صحيح			did_yank = FALSE;

    𐡄𐡍 (curbuf->b_ml.ml_flags & ML_EMPTY)	    // nothing to do
	أرجع OK;

    // Nothing to delete, return here.	Do prepare undo, for op_change().
    𐡄𐡍 (oap->empty)
	أرجع u_save_cursor();

    𐡄𐡍 (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	أرجع FAIL;
    }

    𐡄𐡍 (VIsual_select && oap->is_VIsual)
	// use register given with CTRL_R, defaults to zero
	oap->regname = VIsual_select_reg;

#ifdef HAVE_CLIPMETHOD
    adjust_clip_reg(&oap->regname);
#endif

    𐡄𐡍 (has_mbyte)
	mb_adjust_opend(oap);

    /*
     * Imitate the strange Vi behaviour: If the delete spans more than one
     * line and motion_type == MCHAR and the result is a blank line, make the
     * delete linewise.  Don't do this for the change command or Visual mode.
     */
    𐡄𐡍 (       oap->motion_type == MCHAR
	    && !oap->is_VIsual
	    && !oap->block_mode
	    && oap->line_count > 1
	    && oap->motion_force == NUL
	    && (vim_strchr(p_cpo, CPO_WORD) != عدم)
	    && oap->op_type == OP_DELETE)
    {
	ptr = ml_get(oap->end.lnum) + oap->end.col;
	𐡄𐡍 (*ptr != NUL)
	    ptr += oap->inclusive;
	ptr = skipwhite(ptr);
	𐡄𐡍 (*ptr == NUL && inindent(0))
	    oap->motion_type = MLINE;
    }

    /*
     * Check for trying to delete (e.g. "D") in an empty line.
     * Note: For the change operator it is ok.
     */
    𐡄𐡍 (       oap->motion_type == MCHAR
	    && oap->line_count == 1
	    && oap->op_type == OP_DELETE
	    && *ml_get(oap->start.lnum) == NUL)
    {
	/*
	 * It's an error to operate on an empty region, when 'E' included in
	 * 'cpoptions' (Vi compatible).
	 */
	𐡄𐡍 (virtual_op)
	    // Virtual editing: Nothing gets deleted, but we set the '[ and ']
	    // marks as if it happened.
	    اذهب setmarks;
	𐡄𐡍 (vim_strchr(p_cpo, CPO_EMPTYREGION) != عدم)
	    beep_flush();
	أرجع OK;
    }

    /*
     * Do a yank of whatever we're about to delete.
     * If a yank register was specified, put the deleted text into that
     * register.  For the black hole register '_' don't yank anything.
     */
    𐡄𐡍 (oap->regname != '_')
    {
#ifdef FEAT_CLIPBOARD_PROVIDER
	inc_clip_provider();
#endif
	𐡄𐡍 (oap->regname != 0)
	{
	    // check for read-only register
	    𐡄𐡍 (!valid_yank_reg(oap->regname, TRUE))
	    {
		beep_flush();
#ifdef FEAT_CLIPBOARD_PROVIDER
		dec_clip_provider();
#endif
		أرجع OK;
	    }
	    get_yank_register(oap->regname, TRUE); // yank into specif'd reg.
	    𐡄𐡍 (op_yank(oap, TRUE, FALSE) == OK)   // yank without message
		did_yank = TRUE;
	}
	وإلا
	    reset_y_append(); // not appending to unnamed register

	/*
	 * Put deleted text into register 1 and shift number registers if the
	 * delete contains a line break, or when using a specific operator (Vi
	 * compatible)
	 */
	𐡄𐡍 (oap->motion_type == MLINE || oap->line_count > 1
							   || oap->use_reg_one)
	{
	    shift_delete_registers();
	    𐡄𐡍 (op_yank(oap, TRUE, FALSE) == OK)
		did_yank = TRUE;
	}

	// Yank into small delete register when no named register specified
	// and the delete is within one line.
	𐡄𐡍 ((
#ifdef HAVE_CLIPMETHOD
	    ((clip_unnamed & CLIP_UNNAMED) && oap->regname == '*') ||
	    ((clip_unnamed & CLIP_UNNAMED_PLUS) && oap->regname == '+') ||
#endif
	    oap->regname == 0) && oap->motion_type != MLINE
						      && oap->line_count == 1)
	{
	    oap->regname = '-';
	    get_yank_register(oap->regname, TRUE);
	    𐡄𐡍 (op_yank(oap, TRUE, FALSE) == OK)
		did_yank = TRUE;
	    oap->regname = 0;
	}

	/*
	 * If there's too much stuff to fit in the yank register, then get a
	 * confirmation before doing the delete. This is crude, but simple.
	 * And it avoids doing a delete of something we can't put back if we
	 * want.
	 */
	𐡄𐡍 (!did_yank)
	{
	    صحيح msg_silent_save = msg_silent;

	    msg_silent = 0;	// must display the prompt
	    n = ask_yesno((char_u *)_("cannot yank; delete anyway"), TRUE);
	    msg_silent = msg_silent_save;
	    𐡄𐡍 (n != 'y')
	    {
		emsg(_(e_command_aborted));
#ifdef FEAT_CLIPBOARD_PROVIDER
		dec_clip_provider();
#endif
		أرجع FAIL;
	    }
	}

#𐡄𐡍 defined(FEAT_EVAL)
	𐡄𐡍 (did_yank && has_textyankpost())
	    yank_do_autocmd(oap, get_y_current());
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
    }

    /*
     * block mode delete
     */
    𐡄𐡍 (oap->block_mode)
    {
	𐡄𐡍 (u_save((linenr_T)(oap->start.lnum - 1),
			       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	    أرجع FAIL;

	لكل (lnum = curwin->w_cursor.lnum; lnum <= oap->end.lnum; ++lnum)
	{
	    block_prep(oap, &bd, lnum, TRUE);
	    𐡄𐡍 (bd.textlen == 0)	// nothing to delete
		استمر;

	    // Adjust cursor position for tab replaced by spaces and 'lbr'.
	    𐡄𐡍 (lnum == curwin->w_cursor.lnum)
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
	    𐡄𐡍 (newp == عدم)
		استمر;
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
	    𐡄𐡍 (curbuf->b_has_textprop && n != 0)
		adjust_prop_columns(lnum, bd.textcol, -n, 0);
#endif
	}

	check_cursor_col();
	changed_lines(curwin->w_cursor.lnum, curwin->w_cursor.col,
						       oap->end.lnum + 1, 0L);
	oap->line_count = 0;	    // no lines deleted
    }
    وإلا 𐡄𐡍 (oap->motion_type == MLINE)
    {
	𐡄𐡍 (oap->op_type == OP_CHANGE)
	{
	    // Delete the lines except the first one.  Temporarily move the
	    // cursor to the next line.  Save the current line number, if the
	    // last line is deleted it may be changed.
	    𐡄𐡍 (oap->line_count > 1)
	    {
		lnum = curwin->w_cursor.lnum;
		++curwin->w_cursor.lnum;
		del_lines((long)(oap->line_count - 1), TRUE);
		curwin->w_cursor.lnum = lnum;
	    }
	    𐡄𐡍 (u_save_cursor() == FAIL)
		أرجع FAIL;
	    𐡄𐡍 (curbuf->b_p_ai)		    // don't delete indent
	    {
		beginline(BL_WHITE);	    // cursor on first non-white
		did_ai = TRUE;		    // delete the indent when ESC hit
		ai_col = curwin->w_cursor.col;
	    }
	    وإلا
		beginline(0);		    // cursor in column 0
	    truncate_line(FALSE);   // delete the rest of the line,
				    // leaving cursor past last char in line
	    𐡄𐡍 (oap->line_count > 1)
		u_clearline();	    // "U" command not possible after "2cc"
	}
	وإلا
	{
	    del_lines(oap->line_count, TRUE);
	    beginline(BL_WHITE | BL_FIX);
	    u_clearline();	// "U" command not possible after "dd"
	}
    }
    وإلا
    {
	𐡄𐡍 (virtual_op)
	{
	    صحيح		endcol = 0;

	    // For virtualedit: break the tabs that are partly included.
	    𐡄𐡍 (gchar_pos(&oap->start) == '\t')
	    {
		𐡄𐡍 (u_save_cursor() == FAIL)	// save first line for undo
		    أرجع FAIL;
		𐡄𐡍 (oap->line_count == 1)
		    endcol = getviscol2(oap->end.col, oap->end.coladd);
		coladvance_force(getviscol2(oap->start.col, oap->start.coladd));
		oap->start = curwin->w_cursor;
		𐡄𐡍 (oap->line_count == 1)
		{
		    coladvance(endcol);
		    oap->end.col = curwin->w_cursor.col;
		    oap->end.coladd = curwin->w_cursor.coladd;
		    curwin->w_cursor = oap->start;
		}
	    }

	    // Break a tab only when it's included in the area.
	    𐡄𐡍 (gchar_pos(&oap->end) == '\t'
				     && (صحيح)oap->end.coladd < oap->inclusive)
	    {
		// save last line for undo
		𐡄𐡍 (u_save((linenr_T)(oap->end.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
		    أرجع FAIL;
		curwin->w_cursor = oap->end;
		coladvance_force(getviscol2(oap->end.col, oap->end.coladd));
		oap->end = curwin->w_cursor;
		curwin->w_cursor = oap->start;
	    }
	    𐡄𐡍 (has_mbyte)
		mb_adjust_opend(oap);
	}

	𐡄𐡍 (oap->line_count == 1)	// delete characters within one line
	{
	    𐡄𐡍 (u_save_cursor() == FAIL)	// save line for undo
		أرجع FAIL;

	    // if 'cpoptions' contains '$', display '$' at end of change
	    𐡄𐡍 (       vim_strchr(p_cpo, CPO_DOLLAR) != عدم
		    && oap->op_type == OP_CHANGE
		    && oap->end.lnum == curwin->w_cursor.lnum
		    && !oap->is_VIsual)
		display_dollar(oap->end.col - !oap->inclusive);

	    n = oap->end.col - oap->start.col + 1 - !oap->inclusive;

	    𐡄𐡍 (virtual_op)
	    {
		// fix up things for virtualedit-delete:
		// break the tabs which are going to get in our way
		صحيح		len = ml_get_curline_len();

		𐡄𐡍 (oap->end.coladd != 0
			&& (صحيح)oap->end.col >= len - 1
			&& !(oap->start.coladd && (صحيح)oap->end.col >= len - 1))
		    n++;
		// Delete at least one char (e.g, when on a control char).
		𐡄𐡍 (n == 0 && oap->start.coladd != oap->end.coladd)
		    n = 1;

		// When deleted a char in the line, reset coladd.
		𐡄𐡍 (gchar_cursor() != NUL)
		    curwin->w_cursor.coladd = 0;
	    }
	    (فراغ)del_bytes((long)n, !virtual_op,
			    oap->op_type == OP_DELETE && !oap->is_VIsual);
	}
	وإلا				// delete characters between lines
	{
	    pos_T   curpos;

	    // save deleted and changed lines for undo
	    𐡄𐡍 (u_save((linenr_T)(curwin->w_cursor.lnum - 1),
		 (linenr_T)(curwin->w_cursor.lnum + oap->line_count)) == FAIL)
		أرجع FAIL;

	    truncate_line(TRUE);	// delete from cursor to end of line

	    curpos = curwin->w_cursor;	// remember curwin->w_cursor
	    ++curwin->w_cursor.lnum;
	    del_lines((long)(oap->line_count - 2), FALSE);

	    // delete from start of line until op_end
	    n = (oap->end.col + 1 - !oap->inclusive);
	    curwin->w_cursor.col = 0;
	    (فراغ)del_bytes((long)n, !virtual_op,
			    oap->op_type == OP_DELETE && !oap->is_VIsual);
	    curwin->w_cursor = curpos;	// restore curwin->w_cursor
	    (فراغ)do_join(2, FALSE, FALSE, FALSE, FALSE);
	}
	𐡄𐡍 (oap->op_type == OP_DELETE)
	    auto_format(FALSE, TRUE);
    }

    msgmore(curbuf->b_ml.ml_line_count - old_lcount);

setmarks:
    𐡄𐡍 ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	𐡄𐡍 (oap->block_mode)
	{
	    curbuf->b_op_end.lnum = oap->end.lnum;
	    curbuf->b_op_end.col = oap->start.col;
	}
	وإلا
	    curbuf->b_op_end = oap->start;
	curbuf->b_op_start = oap->start;
    }

    أرجع OK;
}

/*
 * Adjust end of operating area for ending on a multi-byte character.
 * Used for deletion.
 */
    ساكن فراغ
mb_adjust_opend(oparg_T *oap)
{
    char_u	*line;
    char_u	*ptr;

    𐡄𐡍 (!oap->inclusive)
	أرجع;

    line = ml_get(oap->end.lnum);
    ptr = line + oap->end.col;
    𐡄𐡍 (*ptr != NUL)
    {
	ptr -= (*mb_head_off)(line, ptr);
	ptr += (*mb_ptr2len)(ptr) - 1;
	oap->end.col = ptr - line;
    }
}

/*
 * Replace the character under the cursor with "c".
 * This takes care of multi-byte characters.
 */
    ساكن فراغ
replace_character(صحيح c)
{
    صحيح n = State;

    State = MODE_REPLACE;
    ins_char(c);
    State = n;
    // Backup to the replaced character.
    dec_cursor();
}

/*
 * Replace a whole area with one character.
 */
    صحيح
op_replace(oparg_T *oap, صحيح c)
{
    صحيح			n, numc;
    صحيح			num_chars;
    char_u		*newp, *oldp;
    size_t		newlen, oldlen;
    بنية block_def	bd;
    char_u		*after_p = عدم;
    صحيح			had_ctrl_v_cr = FALSE;

    𐡄𐡍 ((curbuf->b_ml.ml_flags & ML_EMPTY ) || oap->empty)
	أرجع OK;	    // nothing to do

    𐡄𐡍 (c == REPLACE_CR_NCHAR)
    {
	had_ctrl_v_cr = TRUE;
	c = CAR;
    }
    وإلا 𐡄𐡍 (c == REPLACE_NL_NCHAR)
    {
	had_ctrl_v_cr = TRUE;
	c = NL;
    }

    𐡄𐡍 (has_mbyte)
	mb_adjust_opend(oap);

    𐡄𐡍 (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	أرجع FAIL;

    /*
     * block mode replace
     */
    𐡄𐡍 (oap->block_mode)
    {
	bd.is_MAX = (curwin->w_curswant == MAXCOL);
	لكل ( ; curwin->w_cursor.lnum <= oap->end.lnum; ++curwin->w_cursor.lnum)
	{
	    curwin->w_cursor.col = 0;  // make sure cursor position is valid
	    block_prep(oap, &bd, curwin->w_cursor.lnum, TRUE);
	    𐡄𐡍 (bd.textlen == 0 && (!virtual_op || bd.is_MAX))
		استمر;	    // nothing to replace

	    // n == number of extra chars required
	    // If we split a TAB, it may be replaced by several characters.
	    // Thus the number of characters may increase!
	    // If the range starts in virtual space, count the initial
	    // coladd offset as part of "startspaces"
	    𐡄𐡍 (virtual_op && bd.is_short && *bd.textstart == NUL)
	    {
		pos_T vpos;

		vpos.lnum = curwin->w_cursor.lnum;
		getvpos(&vpos, oap->start_vcol);
		bd.startspaces += vpos.coladd;
		n = bd.startspaces;
	    }
	    وإلا
		// allow for pre spaces
		n = (bd.startspaces ? bd.start_char_vcols - 1 : 0);

	    // allow for post spp
	    n += (bd.endspaces
		    && !bd.is_oneChar
		    && bd.end_char_vcols > 0) ? bd.end_char_vcols - 1 : 0;
	    // Figure out how many characters to replace.
	    numc = oap->end_vcol - oap->start_vcol + 1;
	    𐡄𐡍 (bd.is_short && (!virtual_op || bd.is_MAX))
		numc -= (oap->end_vcol - bd.end_vcol) + 1;

	    // A double-wide character can be replaced only up to half the
	    // times.
	    𐡄𐡍 ((*mb_char2cells)(c) > 1)
	    {
		𐡄𐡍 ((numc & 1) && !bd.is_short)
		{
		    ++bd.endspaces;
		    ++n;
		}
		numc = numc / 2;
	    }

	    // Compute bytes needed, move character count to num_chars.
	    num_chars = numc;
	    numc *= (*mb_char2len)(c);
	    // oldlen includes textlen, so don't double count
	    n += numc - bd.textlen;

	    oldp = ml_get_curline();
	    oldlen = ml_get_curline_len();
	    newp = alloc(oldlen + 1 + n);
	    𐡄𐡍 (newp == عدم)
		استمر;
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
	    𐡄𐡍 (had_ctrl_v_cr || (c != '\r' && c != '\n'))
	    {
		𐡄𐡍 (has_mbyte)
		{
		    طالما (--num_chars >= 0)
			newlen += (*mb_char2bytes)(c, newp + newlen);
		}
		وإلا
		{
		    vim_memset(newp + newlen, c, (size_t)numc);
		    newlen += numc;
		}
		𐡄𐡍 (!bd.is_short)
		{
		    // insert post-spaces
		    vim_memset(newp + newlen, ' ', (size_t)bd.endspaces);
		    // copy the part after the changed part
		    STRCPY(newp + newlen + bd.endspaces,
				oldp + bd.textcol + bd.textlen);
		}
	    }
	    وإلا
	    {
		// Replacing with \r or \n means splitting the line.
		after_p = alloc(oldlen + 1 + n - newlen);
		𐡄𐡍 (after_p != عدم)
		    STRCPY(after_p, oldp + bd.textcol + bd.textlen);
	    }

	    // replace the line
	    ml_replace(curwin->w_cursor.lnum, newp, FALSE);
	    𐡄𐡍 (after_p != عدم)
	    {
		ml_append(curwin->w_cursor.lnum++, after_p, 0, FALSE);
		appended_lines_mark(curwin->w_cursor.lnum, 1L);
		oap->end.lnum++;
		vim_free(after_p);
	    }
	}
    }
    وإلا
    {
	/*
	 * MCHAR and MLINE motion replace.
	 */
	𐡄𐡍 (oap->motion_type == MLINE)
	{
	    oap->start.col = 0;
	    curwin->w_cursor.col = 0;
	    oap->end.col = ml_get_len(oap->end.lnum);
	    𐡄𐡍 (oap->end.col)
		--oap->end.col;
	}
	وإلا 𐡄𐡍 (!oap->inclusive)
	    dec(&(oap->end));

	طالما (LTOREQ_POS(curwin->w_cursor, oap->end))
	{
	    صحيح done = FALSE;

	    n = gchar_cursor();
	    𐡄𐡍 (n != NUL)
	    {
		صحيح new_byte_len = (*mb_char2len)(c);
		صحيح old_byte_len = mb_ptr2len(ml_get_cursor());

		𐡄𐡍 (new_byte_len > 1 || old_byte_len > 1)
		{
		    // This is slow, but it handles replacing a single-byte
		    // with a multi-byte and the other way around.
		    𐡄𐡍 (curwin->w_cursor.lnum == oap->end.lnum)
			oap->end.col += new_byte_len - old_byte_len;
		    replace_character(c);
		    done = TRUE;
		}
		وإلا
		{
		    𐡄𐡍 (n == TAB)
		    {
			صحيح end_vcol = 0;

			𐡄𐡍 (curwin->w_cursor.lnum == oap->end.lnum)
			{
			    // oap->end has to be recalculated when
			    // the tab breaks
			    end_vcol = getviscol2(oap->end.col,
							     oap->end.coladd);
			}
			coladvance_force(getviscol());
			𐡄𐡍 (curwin->w_cursor.lnum == oap->end.lnum)
			    getvpos(&oap->end, end_vcol);
		    }
		    // with "coladd" set may move to just after a TAB
		    𐡄𐡍 (gchar_cursor() != NUL)
		    {
			PBYTE(curwin->w_cursor, c);
			done = TRUE;
		    }
		}
	    }
	    𐡄𐡍 (!done && virtual_op && curwin->w_cursor.lnum == oap->end.lnum)
	    {
		صحيح virtcols = oap->end.coladd;

		𐡄𐡍 (curwin->w_cursor.lnum == oap->start.lnum
			&& oap->start.col == oap->end.col && oap->start.coladd)
		    virtcols -= oap->start.coladd;

		// oap->end has been trimmed so it's effectively inclusive;
		// as a result an extra +1 must be counted so we don't
		// trample the NUL byte.
		coladvance_force(getviscol2(oap->end.col, oap->end.coladd) + 1);
		curwin->w_cursor.col -= (virtcols + 1);
		لكل (; virtcols >= 0; virtcols--)
		{
		    𐡄𐡍 ((*mb_char2len)(c) > 1)
		       replace_character(c);
		    وإلا
			PBYTE(curwin->w_cursor, c);
		    𐡄𐡍 (inc(&curwin->w_cursor) == -1)
			اكسر;
		}
	    }

	    // Advance to next character, stop at the end of the file.
	    𐡄𐡍 (inc_cursor() == -1)
		اكسر;
	}
    }

    curwin->w_cursor = oap->start;
    check_cursor();
    changed_lines(oap->start.lnum, oap->start.col, oap->end.lnum + 1, 0L);

    𐡄𐡍 ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
    }

    أرجع OK;
}

ساكن صحيح swapchars(صحيح op_type, pos_T *pos, صحيح length);

/*
 * Handle the (non-standard vi) tilde operator.  Also for "gu", "gU" and "g?".
 */
    ساكن فراغ
op_tilde(oparg_T *oap)
{
    pos_T		pos;
    بنية block_def	bd;
    صحيح			did_change = FALSE;

    𐡄𐡍 (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	أرجع;

    pos = oap->start;
    𐡄𐡍 (oap->block_mode)		    // Visual block mode
    {
	لكل (; pos.lnum <= oap->end.lnum; ++pos.lnum)
	{
	    صحيح one_change;

	    block_prep(oap, &bd, pos.lnum, FALSE);
	    pos.col = bd.textcol;
	    one_change = swapchars(oap->op_type, &pos, bd.textlen);
	    did_change |= one_change;

#ifdef FEAT_NETBEANS_INTG
	    𐡄𐡍 (netbeans_active() && one_change)
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
	𐡄𐡍 (did_change)
	    changed_lines(oap->start.lnum, 0, oap->end.lnum + 1, 0L);
    }
    وإلا				    // not block mode
    {
	𐡄𐡍 (oap->motion_type == MLINE)
	{
	    oap->start.col = 0;
	    pos.col = 0;
	    oap->end.col = ml_get_len(oap->end.lnum);
	    𐡄𐡍 (oap->end.col)
		--oap->end.col;
	}
	وإلا 𐡄𐡍 (!oap->inclusive)
	    dec(&(oap->end));

	𐡄𐡍 (pos.lnum == oap->end.lnum)
	    did_change = swapchars(oap->op_type, &pos,
						  oap->end.col - pos.col + 1);
	وإلا
	    لكل (;;)
	    {
		did_change |= swapchars(oap->op_type, &pos,
			pos.lnum == oap->end.lnum ? oap->end.col + 1
						  : ml_get_pos_len(&pos));
		𐡄𐡍 (LTOREQ_POS(oap->end, pos) || inc(&pos) == -1)
		    اكسر;
	    }
	𐡄𐡍 (did_change)
	{
	    changed_lines(oap->start.lnum, oap->start.col, oap->end.lnum + 1,
									  0L);
#ifdef FEAT_NETBEANS_INTG
	    𐡄𐡍 (netbeans_active())
	    {
		char_u *ptr;
		صحيح count;

		pos = oap->start;
		طالما (pos.lnum < oap->end.lnum)
		{
		    ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		    count = ml_get_buf_len(curbuf, pos.lnum) - pos.col;
		    netbeans_removed(curbuf, pos.lnum, pos.col, (long)count);
		    // get the line again, it may have been flushed
		    ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		    netbeans_inserted(curbuf, pos.lnum, pos.col,
							&ptr[pos.col], count);
		    pos.col = 0;
		    pos.lnum++;
		}
		count = oap->end.col - pos.col + 1;
		netbeans_removed(curbuf, pos.lnum, pos.col, (long)count);
		// get the line again, it may have been flushed
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, pos.col,
							&ptr[pos.col], count);
	    }
#endif
	}
    }

    𐡄𐡍 (!did_change && oap->is_VIsual)
	// No change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    𐡄𐡍 ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set '[ and '] marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
    }

    𐡄𐡍 (oap->line_count > p_report)
	smsg(NGETTEXT("%ld line changed", "%ld lines changed",
					    oap->line_count), oap->line_count);
}

/*
 * Invoke swapchar() on "length" bytes at position "pos".
 * "pos" is advanced to just after the changed characters.
 * "length" is rounded up to include the whole last multi-byte character.
 * Also works correctly when the number of bytes changes.
 * Returns TRUE if some character was changed.
 */
    ساكن صحيح
swapchars(صحيح op_type, pos_T *pos, صحيح length)
{
    صحيح todo;
    صحيح	did_change = 0;

    لكل (todo = length; todo > 0; --todo)
    {
	𐡄𐡍 (has_mbyte)
	{
	    صحيح len = (*mb_ptr2len)(ml_get_pos(pos));

	    // we're counting bytes, not characters
	    𐡄𐡍 (len > 0)
		todo -= len - 1;
	}
	did_change |= swapchar(op_type, pos);
	𐡄𐡍 (inc(pos) == -1)    // at end of file
	    اكسر;
    }
    أرجع did_change;
}

/*
 * If op_type == OP_UPPER: make uppercase,
 * if op_type == OP_LOWER: make lowercase,
 * if op_type == OP_ROT13: do rot13 encoding,
 * else swap case of character at 'pos'
 * returns TRUE when something actually changed.
 */
    صحيح
swapchar(صحيح op_type, pos_T *pos)
{
    صحيح	    c;
    صحيح	    nc;

    c = gchar_pos(pos);

    // Only do rot13 encoding for ASCII characters.
    𐡄𐡍 (c >= 0x80 && op_type == OP_ROT13)
	أرجع FALSE;

    // ~ is OP_NOP, g~ is OP_TILDE, gU is OP_UPPER
    𐡄𐡍 ((op_type == OP_UPPER || op_type == OP_NOP || op_type == OP_TILDE)
	    && c == 0xdf
	    && (enc_latin1like || STRCMP(p_enc, "iso-8859-2") == 0))
    {
	pos_T   sp = curwin->w_cursor;

	// Special handling for lowercase German sharp s (ß): convert to uppercase (ẞ).
	curwin->w_cursor = *pos;
	del_char(FALSE);
	ins_char(0x1E9E);
	curwin->w_cursor = sp;
	أرجع TRUE;
    }

    𐡄𐡍 (enc_dbcs != 0 && c >= 0x100)	// No lower/uppercase letter
	أرجع FALSE;
    nc = c;
    𐡄𐡍 (MB_ISLOWER(c))
    {
	𐡄𐡍 (op_type == OP_ROT13)
	    nc = ROT13(c, 'a');
	وإلا 𐡄𐡍 (op_type != OP_LOWER)
	    nc = MB_TOUPPER(c);
    }
    وإلا 𐡄𐡍 (MB_ISUPPER(c))
    {
	𐡄𐡍 (op_type == OP_ROT13)
	    nc = ROT13(c, 'A');
	وإلا 𐡄𐡍 (op_type != OP_UPPER)
	    nc = MB_TOLOWER(c);
    }
    𐡄𐡍 (nc != c)
    {
	𐡄𐡍 (enc_utf8 && (c >= 0x80 || nc >= 0x80))
	{
	    pos_T   sp = curwin->w_cursor;

	    curwin->w_cursor = *pos;
	    // don't use del_char(), it also removes composing chars
	    del_bytes(utf_ptr2len(ml_get_cursor()), FALSE, FALSE);
	    ins_char(nc);
	    curwin->w_cursor = sp;
	}
	وإلا
	    PBYTE(*pos, nc);
	أرجع TRUE;
    }
    أرجع FALSE;
}

/*
 * op_insert - Insert and append operators for Visual mode.
 */
    فراغ
op_insert(oparg_T *oap, long count1)
{
    long		pre_textlen = 0;
    colnr_T		ind_pre_col = 0, ind_post_col;
    صحيح			ind_pre_vcol = 0, ind_post_vcol = 0;
    بنية block_def	bd;
    صحيح			i;
    pos_T		t1;
    pos_T		start_insert;

    // edit() changes this - record it for OP_APPEND
    bd.is_MAX = (curwin->w_curswant == MAXCOL);

    // vis block is still marked. Get rid of it now.
    curwin->w_cursor.lnum = oap->start.lnum;
    update_screen(UPD_INVERTED);

    𐡄𐡍 (oap->block_mode)
    {
	// When 'virtualedit' is used, need to insert the extra spaces before
	// doing block_prep().  When only "block" is used, virtual edit is
	// already disabled, but still need it when calling
	// coladvance_force().
	// coladvance_force() uses get_ve_flags() to get the 'virtualedit'
	// state for the current window.  To override that state, we need to
	// set the window-local value of ve_flags rather than the global value.
	𐡄𐡍 (curwin->w_cursor.coladd > 0)
	{
	    صحيح		old_ve_flags = curwin->w_ve_flags;

	    𐡄𐡍 (u_save_cursor() == FAIL)
		أرجع;

	    curwin->w_ve_flags = VE_ALL;
	    coladvance_force(oap->op_type == OP_APPEND
					   ? oap->end_vcol + 1 : getviscol());
	    𐡄𐡍 (oap->op_type == OP_APPEND)
		--curwin->w_cursor.col;
	    curwin->w_ve_flags = old_ve_flags;
	}
	// Get the info about the block before entering the text
	block_prep(oap, &bd, oap->start.lnum, TRUE);
	// Get indent information
	ind_pre_col = (colnr_T)getwhitecols_curline();
	ind_pre_vcol = get_indent();
	pre_textlen = ml_get_len(oap->start.lnum) - bd.textcol;
	𐡄𐡍 (oap->op_type == OP_APPEND)
	    pre_textlen -= bd.textlen;
    }

    𐡄𐡍 (oap->op_type == OP_APPEND)
    {
	𐡄𐡍 (oap->block_mode && curwin->w_cursor.coladd == 0)
	{
	    // Move the cursor to the character right of the block.
	    curwin->w_set_curswant = true;
	    طالما (*ml_get_cursor() != NUL
		    && (curwin->w_cursor.col < bd.textcol + bd.textlen))
		++curwin->w_cursor.col;
	    𐡄𐡍 (bd.is_short && !bd.is_MAX)
	    {
		// First line was too short, make it longer and adjust the
		// values in "bd".
		𐡄𐡍 (u_save_cursor() == FAIL)
		    أرجع;
		لكل (i = 0; i < bd.endspaces; ++i)
		    ins_char(' ');
		bd.textlen += bd.endspaces;
	    }
	}
	وإلا
	{
	    curwin->w_cursor = oap->end;
	    check_cursor_col();

	    // Works just like an 'i'nsert on the next character.
	    𐡄𐡍 (!LINEEMPTY(curwin->w_cursor.lnum)
		    && oap->start_vcol != oap->end_vcol)
		inc_cursor();
	}
    }

    t1 = oap->start;
    start_insert = curwin->w_cursor;
    (فراغ)edit(NUL, FALSE, (linenr_T)count1);

    // When a tab was inserted, and the characters in front of the tab
    // have been converted to a tab as well, the column of the cursor
    // might have actually been reduced, so need to adjust here.
    𐡄𐡍 (t1.lnum == curbuf->b_op_start_orig.lnum
	    && LT_POS(curbuf->b_op_start_orig, t1))
	oap->start = curbuf->b_op_start_orig;

    // If user has moved off this line, we don't know what to do, so do
    // nothing.
    // Also don't repeat the insert when Insert mode ended with CTRL-C.
    𐡄𐡍 (curwin->w_cursor.lnum != oap->start.lnum || got_int)
	أرجع;

    𐡄𐡍 (oap->block_mode)
    {
	صحيح			ins_len;
	char_u			*firstline, *ins_text;
	بنية block_def	bd2;
	صحيح			did_indent = FALSE;
	size_t			len;
	size_t			add;
	// offset when cursor was moved in insert mode
	صحيح			offset = 0;

	// If indent kicked in, the firstline might have changed
	// but only do that, if the indent actually increased.
	ind_post_col = (colnr_T)getwhitecols_curline();
	𐡄𐡍 (curbuf->b_op_start.col > ind_pre_col && ind_post_col > ind_pre_col)
	{
	    bd.textcol += ind_post_col - ind_pre_col;
	    ind_post_vcol = get_indent();
	    bd.start_vcol += ind_post_vcol - ind_pre_vcol;
	    did_indent = TRUE;
	}

	// The user may have moved the cursor before inserting something, try
	// to adjust the block for that.  But only do it, if the difference
	// does not come from indent kicking in.
	𐡄𐡍 (oap->start.lnum == curbuf->b_op_start_orig.lnum
						  && !bd.is_MAX && !did_indent)
	{
	    صحيح t = getviscol2(curbuf->b_op_start_orig.col,
					       curbuf->b_op_start_orig.coladd);

	    𐡄𐡍 (oap->op_type == OP_INSERT
		    && oap->start.col + oap->start.coladd
			    != curbuf->b_op_start_orig.col
					      + curbuf->b_op_start_orig.coladd)
	    {
		oap->start.col = curbuf->b_op_start_orig.col;
		pre_textlen -= t - oap->start_vcol;
		oap->start_vcol = t;
	    }
	    وإلا 𐡄𐡍 (oap->op_type == OP_APPEND
		    && oap->start.col + oap->start.coladd
			    >= curbuf->b_op_start_orig.col
					      + curbuf->b_op_start_orig.coladd)
	    {
		oap->start.col = curbuf->b_op_start_orig.col;
		// reset pre_textlen to the value of OP_INSERT
		pre_textlen += bd.textlen;
		pre_textlen -= t - oap->start_vcol;
		oap->start_vcol = t;
		oap->op_type = OP_INSERT;
	    }
	}

	// Spaces and tabs in the indent may have changed to other spaces and
	// tabs.  Get the starting column again and correct the length.
	// Don't do this when "$" used, end-of-line will have changed.
	//
	// if indent was added and the inserted text was after the indent,
	// correct the selection for the new indent.
	𐡄𐡍 (did_indent && bd.textcol - ind_post_col > 0)
	{
	    oap->start.col += ind_post_col - ind_pre_col;
	    oap->start_vcol += ind_post_vcol - ind_pre_vcol;
	    oap->end.col += ind_post_col - ind_pre_col;
	    oap->end_vcol += ind_post_vcol - ind_pre_vcol;
	}
	block_prep(oap, &bd2, oap->start.lnum, TRUE);
	𐡄𐡍 (did_indent && bd.textcol - ind_post_col > 0)
	{
	    // undo for where "oap" is used below
	    oap->start.col -= ind_post_col - ind_pre_col;
	    oap->start_vcol -= ind_post_vcol - ind_pre_vcol;
	    oap->end.col -= ind_post_col - ind_pre_col;
	    oap->end_vcol -= ind_post_vcol - ind_pre_vcol;
	}
	𐡄𐡍 (!bd.is_MAX || bd2.textlen < bd.textlen)
	{
	    𐡄𐡍 (oap->op_type == OP_APPEND)
	    {
		pre_textlen += bd2.textlen - bd.textlen;
		𐡄𐡍 (bd2.endspaces)
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
	𐡄𐡍 (oap->op_type == OP_APPEND)
	{
	    add += bd.textlen;
	    // account for pressing cursor in insert mode when '$' was used
	    𐡄𐡍 (bd.is_MAX
		&& (start_insert.lnum == Insstart.lnum
					   && start_insert.col > Insstart.col))
	    {
		offset = (start_insert.col - Insstart.col);
		add -= offset;
		𐡄𐡍 (oap->end_vcol > offset)
		    oap->end_vcol -= (offset + 1);
		وإلا
		    // moved outside of the visual block, what to do?
		    أرجع;
	    }
	}
	𐡄𐡍 (add > len)
	    add = len;  // short line, point to the NUL
	firstline += add;
	len -= add;
	𐡄𐡍 (pre_textlen >= 0 && (ins_len = (صحيح)len - pre_textlen - offset) > 0)
	{
	    ins_text = vim_strnsave(firstline, ins_len);
	    𐡄𐡍 (ins_text != عدم)
	    {
		// block handled here
		𐡄𐡍 (u_save(oap->start.lnum,
					 (linenr_T)(oap->end.lnum + 1)) == OK)
		    block_insert(oap, ins_text, ins_len, (oap->op_type == OP_INSERT),
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
    صحيح
op_change(oparg_T *oap)
{
    colnr_T		l;
    صحيح			retval;
    linenr_T		linenr;
    long		pre_textlen = 0;
    long		pre_indent = 0;
    char_u		*firstline;
    char_u		*ins_text, *newp, *oldp;
    بنية block_def	bd;

    l = oap->start.col;
    𐡄𐡍 (oap->motion_type == MLINE)
    {
	l = 0;
	can_si = may_do_si();	// Like opening a new line, do smart indent
    }

    // First delete the text in the region.  In an empty buffer only need to
    // save for undo
    𐡄𐡍 (curbuf->b_ml.ml_flags & ML_EMPTY)
    {
	𐡄𐡍 (u_save_cursor() == FAIL)
	    أرجع FALSE;
    }
    وإلا 𐡄𐡍 (op_delete(oap) == FAIL)
	أرجع FALSE;

    𐡄𐡍 ((l > curwin->w_cursor.col) && !LINEEMPTY(curwin->w_cursor.lnum)
							 && !virtual_op)
	inc_cursor();

    // check for still on same line (<CR> in inserted text meaningless)
    // skip blank lines too
    𐡄𐡍 (oap->block_mode)
    {
	// Add spaces before getting the current line length.
	𐡄𐡍 (virtual_op && (curwin->w_cursor.coladd > 0
						    || gchar_cursor() == NUL))
	    coladvance_force(getviscol());
	firstline = ml_get(oap->start.lnum);
	pre_textlen = ml_get_len(oap->start.lnum);
	pre_indent = (long)getwhitecols(firstline);
	bd.textcol = curwin->w_cursor.col;
    }

    𐡄𐡍 (oap->motion_type == MLINE)
	fix_indent();

    // Reset finish_op now, don't want it set inside edit().
    صحيح save_finish_op = finish_op;
    finish_op = FALSE;

    retval = edit(NUL, FALSE, (linenr_T)1);

    finish_op = save_finish_op;

    /*
     * In Visual block mode, handle copying the new text to all lines of the
     * block.
     * Don't repeat the insert when Insert mode ended with CTRL-C.
     */
    𐡄𐡍 (oap->block_mode && oap->start.lnum != oap->end.lnum && !got_int)
    {
	صحيح	ins_len;

	// Auto-indenting may have changed the indent.  If the cursor was past
	// the indent, exclude that indent change from the inserted text.
	firstline = ml_get(oap->start.lnum);
	𐡄𐡍 (bd.textcol > (colnr_T)pre_indent)
	{
	    long new_indent = (long)getwhitecols(firstline);

	    pre_textlen += new_indent - pre_indent;
	    bd.textcol += new_indent - pre_indent;
	}

	ins_len = (صحيح)ml_get_len(oap->start.lnum) - pre_textlen;
	𐡄𐡍 (ins_len > 0)
	{
	    // Subsequent calls to ml_get() flush the firstline data - take a
	    // copy of the inserted text.
	    𐡄𐡍 ((ins_text = alloc(ins_len + 1)) != عدم)
	    {
		vim_strncpy(ins_text, firstline + bd.textcol, ins_len);
		لكل (linenr = oap->start.lnum + 1; linenr <= oap->end.lnum;
								     linenr++)
		{
		    block_prep(oap, &bd, linenr, TRUE);
		    𐡄𐡍 (!bd.is_short || virtual_op)
		    {
			pos_T vpos;
			size_t newlen;

			// If the block starts in virtual space, count the
			// initial coladd offset as part of "startspaces"
			𐡄𐡍 (bd.is_short)
			{
			    vpos.lnum = linenr;
			    (فراغ)getvpos(&vpos, oap->start_vcol);
			}
			وإلا
			    vpos.coladd = 0;
			oldp = ml_get(linenr);
			newp = alloc(ml_get_len(linenr) + vpos.coladd + ins_len + 1);
			𐡄𐡍 (newp == عدم)
			    استمر;
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
			𐡄𐡍 (curbuf->b_has_textprop)
			    adjust_prop_columns(linenr, bd.textcol,
						     vpos.coladd + (صحيح)ins_len, 0);
#endif
		    }
		}
		check_cursor();

		changed_lines(oap->start.lnum + 1, 0, oap->end.lnum + 1, 0L);
	    }
	    vim_free(ins_text);
	}
    }
    auto_format(FALSE, TRUE);

    أرجع retval;
}

/*
 * When the cursor is on the NUL past the end of the line and it should not be
 * there move it left.
 */
    فراغ
adjust_cursor_eol(فراغ)
{
    unsigned صحيح cur_ve_flags = get_ve_flags();

    صحيح adj_cursor = (curwin->w_cursor.col > 0
				&& gchar_cursor() == NUL
				&& (cur_ve_flags & VE_ONEMORE) == 0
				&& (cur_ve_flags & VE_ALL) == 0
				&& !(restart_edit || (State & MODE_INSERT)));
    𐡄𐡍 (!adj_cursor)
	أرجع;

    // Put the cursor on the last character in the line.
    dec_cursor();

    𐡄𐡍 (cur_ve_flags == VE_ALL)
    {
	colnr_T	    scol, ecol;

	// Coladd is set to the width of the last character.
	getvcol(curwin, &curwin->w_cursor, &scol, عدم, &ecol, 0);
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
    char_u   *line,
    صحيح      process,
    صحيح	     include_space,
    صحيح      *is_comment)
{
    char_u *comment_flags = عدم;
    صحيح    lead_len;
    صحيح    leader_offset = get_last_leader_offset(line, &comment_flags);

    *is_comment = FALSE;
    𐡄𐡍 (leader_offset != -1)
    {
	// Let's check whether the line ends with an unclosed comment.
	// If the last comment leader has COM_END in flags, there's no comment.
	طالما (*comment_flags)
	{
	    𐡄𐡍 (*comment_flags == COM_END
		    || *comment_flags == ':')
		اكسر;
	    ++comment_flags;
	}
	𐡄𐡍 (*comment_flags != COM_END)
	    *is_comment = TRUE;
    }

    𐡄𐡍 (process == FALSE)
	أرجع line;

    lead_len = get_leader_len(line, &comment_flags, FALSE, include_space);

    𐡄𐡍 (lead_len == 0)
	أرجع line;

    // Find:
    // - COM_END,
    // - colon,
    // whichever comes first.
    طالما (*comment_flags)
    {
	𐡄𐡍 (*comment_flags == COM_END
		|| *comment_flags == ':')
	    اكسر;
	++comment_flags;
    }

    // If we found a colon, it means that we are not processing a line
    // starting with a closing part of a three-part comment. That's good,
    // because we don't want to remove those as this would be annoying.
    𐡄𐡍 (*comment_flags == ':' || *comment_flags == NUL)
	line += lead_len;

    أرجع line;
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
    صحيح
do_join(
    long    count,
    صحيح	    insert_space,
    صحيح	    save_undo,
    صحيح	    use_formatoptions UNUSED,
    صحيح	    setmark)
{
    char_u	*curr = عدم;
    char_u      *curr_start = عدم;
    char_u	*cend;
    char_u	*newp;
    size_t	newp_len;
    char_u	*spaces;	// number of spaces inserted before a line
    صحيح		endcurr1 = NUL;
    صحيح		endcurr2 = NUL;
    صحيح		currsize = 0;	// size of the current line
    صحيح		sumsize = 0;	// size of the long new line
    linenr_T	t;
    colnr_T	col = 0;
    صحيح		ret = OK;
    صحيح		*comments = عدم;
    صحيح		remove_comments = (use_formatoptions == TRUE)
				  && has_format_option(FO_REMOVE_COMS);
    صحيح		prev_was_comment;

    𐡄𐡍 (save_undo && u_save((linenr_T)(curwin->w_cursor.lnum - 1),
			    (linenr_T)(curwin->w_cursor.lnum + count)) == FAIL)
	أرجع FAIL;

    // Allocate an array to store the number of spaces inserted before each
    // line.  We will use it to pre-compute the length of the new line and the
    // proper placement of each original line in the new one.
    spaces = lalloc_clear(count, TRUE);
    𐡄𐡍 (spaces == عدم)
	أرجع FAIL;
    𐡄𐡍 (remove_comments)
    {
	comments = lalloc_clear(count * حجم(صحيح), TRUE);
	𐡄𐡍 (comments == عدم)
	{
	    vim_free(spaces);
	    أرجع FAIL;
	}
    }

    /*
     * Don't move anything yet, just compute the final line length
     * and setup the array of space strings lengths
     * This loops forward over the joined lines.
     */
    لكل (t = 0; t < count; ++t)
    {
	curr = curr_start = ml_get((linenr_T)(curwin->w_cursor.lnum + t));
	𐡄𐡍 (t == 0 && setmark && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set the '[ mark.
	    curwin->w_buffer->b_op_start.lnum = curwin->w_cursor.lnum;
	    curwin->w_buffer->b_op_start.col  = (colnr_T)STRLEN(curr);
	}
	𐡄𐡍 (remove_comments)
	{
	    // We don't want to remove the comment leader if the
	    // previous line is not a comment.
	    𐡄𐡍 (t > 0 && prev_was_comment)
	    {

		char_u *new_curr = skip_comment(curr, TRUE, insert_space,
							   &prev_was_comment);
		comments[t] = (صحيح)(new_curr - curr);
		curr = new_curr;
	    }
	    وإلا
		curr = skip_comment(curr, FALSE, insert_space,
							   &prev_was_comment);
	}

	𐡄𐡍 (insert_space && t > 0)
	{
	    curr = skipwhite(curr);
	    𐡄𐡍 (*curr != NUL && *curr != ')'
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
		𐡄𐡍 (endcurr1 == ' ')
		    endcurr1 = endcurr2;
		وإلا
		    ++spaces[t];
		// extra space when 'joinspaces' set and line ends in '.'
		𐡄𐡍 (       p_js
			&& (endcurr1 == '.'
			    || (vim_strchr(p_cpo, CPO_JOINSP) == عدم
				&& (endcurr1 == '?' || endcurr1 == '!'))))
		    ++spaces[t];
	    }
	}
	currsize = (صحيح)STRLEN(curr);
	sumsize += currsize + spaces[t];
	endcurr1 = endcurr2 = NUL;
	𐡄𐡍 (insert_space && currsize > 0)
	{
	    𐡄𐡍 (has_mbyte)
	    {
		cend = curr + currsize;
		MB_PTR_BACK(curr, cend);
		endcurr1 = (*mb_ptr2char)(cend);
		𐡄𐡍 (cend > curr)
		{
		    MB_PTR_BACK(curr, cend);
		    endcurr2 = (*mb_ptr2char)(cend);
		}
	    }
	    وإلا
	    {
		endcurr1 = *(curr + currsize - 1);
		𐡄𐡍 (currsize > 1)
		    endcurr2 = *(curr + currsize - 2);
	    }
	}
	line_breakcheck();
	𐡄𐡍 (got_int)
	{
	    ret = FAIL;
	    اذهب theend;
	}
    }

    // store the column position before last line
    col = sumsize - currsize - spaces[count - 1];

    // allocate the space for the new line
    newp_len = sumsize + 1;
    newp = alloc(newp_len);
    𐡄𐡍 (newp == عدم)
    {
	ret = FAIL;
	اذهب theend;
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
    صحيح curr_off = (صحيح)(curr - curr_start);

    curr = curr_start = ml_get((linenr_T)(curwin->w_cursor.lnum + count - 1));
    curr += curr_off;
#endif

    لكل (t = count - 1; ; --t)
    {
	صحيح spaces_removed;

	cend -= currsize;
	mch_memmove(cend, curr, (size_t)currsize);

	𐡄𐡍 (spaces[t] > 0)
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
		t == count - 1, (long)(cend - newp), spaces_removed);
#endif
	𐡄𐡍 (t == 0)
	    اكسر;
	curr = curr_start = ml_get((linenr_T)(curwin->w_cursor.lnum + t - 1));
	𐡄𐡍 (remove_comments)
	    curr += comments[t - 1];
	𐡄𐡍 (insert_space && t > 1)
	    curr = skipwhite(curr);
	currsize = (صحيح)STRLEN(curr);
    }

#ifdef FEAT_PROP_POPUP
    𐡄𐡍 (um.buf != عدم)
    {
	um_set_text(&um, newp);
	um_reverse_props(&um);
	um_close(&um);
	newp = عدم;  // um_set_text took ownership
    }
    وإلا
#endif
    ml_replace_len(curwin->w_cursor.lnum, newp, (colnr_T)newp_len, TRUE, FALSE);

    𐡄𐡍 (setmark && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set the '] mark.
	curwin->w_buffer->b_op_end.lnum = curwin->w_cursor.lnum;
	curwin->w_buffer->b_op_end.col  = (colnr_T)sumsize;
    }

    // Only report the change in the first line here, del_lines() will report
    // the deleted line.
    changed_lines(curwin->w_cursor.lnum, currsize,
					       curwin->w_cursor.lnum + 1, 0L);
    /*
     * Delete following lines. To do this we move the cursor there
     * briefly, and then move it back. After del_lines() the cursor may
     * have moved up (last line deleted), so the current lnum is kept in t.
     */
    t = curwin->w_cursor.lnum;
    ++curwin->w_cursor.lnum;
    del_lines(count - 1, FALSE);
    curwin->w_cursor.lnum = t;

    /*
     * Set the cursor column:
     * Vi compatible: use the column of the first join
     * vim:	      use the column of the last join
     */
    curwin->w_cursor.col =
		    (vim_strchr(p_cpo, CPO_JOINCOL) != عدم ? currsize : col);
    check_cursor_col();

    curwin->w_cursor.coladd = 0;
    curwin->w_set_curswant = true;

theend:
    vim_free(spaces);
    𐡄𐡍 (remove_comments)
	vim_free(comments);
    أرجع ret;
}

#ifdef FEAT_LINEBREAK
/*
 * TODO: consider using "tailp" of win_lbr_chartabsize() instead of temporarily
 * resetting 'linebreak'.
 *
 * Reset 'linebreak' and take care of side effects.
 * Returns the previous value, to be passed to restore_lbr().
 */
    صحيح
reset_lbr(فراغ)
{
    𐡄𐡍 (!curwin->w_p_lbr)
	أرجع FALSE;
    // changing 'linebreak' may require w_virtcol to be updated
    curwin->w_p_lbr = FALSE;
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);
    أرجع TRUE;
}

/*
 * Restore 'linebreak' and take care of side effects.
 */
    فراغ
restore_lbr(صحيح lbr_saved)
{
    𐡄𐡍 (curwin->w_p_lbr || !lbr_saved)
	أرجع;

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
    فراغ
block_prep(
    oparg_T		*oap,
    بنية block_def	*bdp,
    linenr_T		lnum,
    صحيح			is_del)
{
    صحيح		incr = 0;
    char_u	*pend;
    char_u	*pstart;
    char_u	*line;
    char_u	*prev_pstart;
    char_u	*prev_pend;
    chartabsize_T cts;
#ifdef FEAT_LINEBREAK
		// Avoid a problem with unwanted linebreaks in block mode.
    صحيح		lbr_saved = reset_lbr();
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

    line = ml_get(lnum);
    prev_pstart = line;
    init_chartabsize_arg(&cts, curwin, lnum, bdp->start_vcol, line, line);
    طالما (cts.cts_vcol < oap->start_vcol && *cts.cts_ptr != NUL)
    {
	// Count a tab for what it's worth (if list mode not on)
	incr = lbr_chartabsize(&cts);
	cts.cts_vcol += incr;
	𐡄𐡍 (VIM_ISWHITE(*cts.cts_ptr))
	{
	    bdp->pre_whitesp += incr;
	    bdp->pre_whitesp_c++;
	}
	وإلا
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
    𐡄𐡍 (bdp->start_vcol < oap->start_vcol)	// line too short
    {
	bdp->end_vcol = bdp->start_vcol;
	bdp->is_short = TRUE;
	𐡄𐡍 (!is_del || oap->op_type == OP_APPEND)
	    bdp->endspaces = oap->end_vcol - oap->start_vcol + 1;
    }
    وإلا
    {
	// notice: this converts partly selected Multibyte characters to
	// spaces, too.
	bdp->startspaces = bdp->start_vcol - oap->start_vcol;
	𐡄𐡍 (is_del && bdp->startspaces)
	    bdp->startspaces = bdp->start_char_vcols - bdp->startspaces;
	pend = pstart;
	bdp->end_vcol = bdp->start_vcol;
	𐡄𐡍 (bdp->end_vcol > oap->end_vcol)	// it's all in one character
	{
	    bdp->is_oneChar = TRUE;
	    𐡄𐡍 (oap->op_type == OP_INSERT)
		bdp->endspaces = bdp->start_char_vcols - bdp->startspaces;
	    وإلا 𐡄𐡍 (oap->op_type == OP_APPEND)
	    {
		bdp->startspaces += oap->end_vcol - oap->start_vcol + 1;
		bdp->endspaces = bdp->start_char_vcols - bdp->startspaces;
	    }
	    وإلا
	    {
		bdp->startspaces = oap->end_vcol - oap->start_vcol + 1;
		𐡄𐡍 (is_del && oap->op_type != OP_LSHIFT)
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
	وإلا
	{
	    init_chartabsize_arg(&cts, curwin, lnum, bdp->end_vcol,
								  line, pend);
	    prev_pend = pend;
	    طالما (cts.cts_vcol <= oap->end_vcol && *cts.cts_ptr != NUL)
	    {
		// count a tab for what it's worth (if list mode not on)
		prev_pend = cts.cts_ptr;
		incr = lbr_chartabsize_adv(&cts);
		cts.cts_vcol += incr;
	    }
	    bdp->end_vcol = cts.cts_vcol;
	    pend = cts.cts_ptr;
	    clear_chartabsize_arg(&cts);

	    𐡄𐡍 (bdp->end_vcol <= oap->end_vcol
		    && (!is_del
			|| oap->op_type == OP_APPEND
			|| oap->op_type == OP_REPLACE)) // line too short
	    {
		bdp->is_short = TRUE;
		// Alternative: include spaces to fill up the block.
		// Disadvantage: can lead to trailing spaces when the line is
		// short where the text is put
		// if (!is_del || oap->op_type == OP_APPEND)
		𐡄𐡍 (oap->op_type == OP_APPEND || virtual_op)
		    bdp->endspaces = oap->end_vcol - bdp->end_vcol
							     + oap->inclusive;
		وإلا
		    bdp->endspaces = 0; // replace doesn't add characters
	    }
	    وإلا 𐡄𐡍 (bdp->end_vcol > oap->end_vcol)
	    {
		bdp->endspaces = bdp->end_vcol - oap->end_vcol - 1;
		𐡄𐡍 (!is_del && bdp->endspaces)
		{
		    bdp->endspaces = incr - bdp->endspaces;
		    𐡄𐡍 (pend != pstart)
			pend = prev_pend;
		}
	    }
	}
	bdp->end_char_vcols = incr;
	𐡄𐡍 (is_del && bdp->startspaces)
	    pstart = prev_pstart;
	bdp->textlen = (صحيح)(pend - pstart);
    }
    bdp->textcol = (colnr_T) (pstart - line);
    bdp->textstart = pstart;
#ifdef FEAT_LINEBREAK
    restore_lbr(lbr_saved);
#endif
}

/*
 * Get block text from "start" to "end"
 */
    فراغ
charwise_block_prep(
    pos_T		start,
    pos_T		end,
    بنية block_def	*bdp,
    linenr_T		lnum,
    صحيح			inclusive)
{
    colnr_T startcol = 0, endcol = MAXCOL;
    colnr_T cs, ce;
    char_u *p;
    صحيح	plen = ml_get_len(lnum);

    p = ml_get(lnum);
    bdp->startspaces = 0;
    bdp->endspaces = 0;
    bdp->is_oneChar = FALSE;
    bdp->start_char_vcols = 0;

    𐡄𐡍 (lnum == start.lnum)
    {
	startcol = start.col;
	𐡄𐡍 (virtual_op)
	{
	    getvcol(curwin, &start, &cs, عدم, &ce, 0);
	    𐡄𐡍 (ce != cs && start.coladd > 0)
	    {
		// Part of a tab selected -- but don't
		// double-count it.
		bdp->start_char_vcols = ce - cs + 1;
		bdp->startspaces = bdp->start_char_vcols - start.coladd;
		𐡄𐡍 (bdp->startspaces < 0)
		    bdp->startspaces = 0;
		startcol++;
	    }
	}
    }

    𐡄𐡍 (lnum == end.lnum)
    {
	endcol = end.col;
	𐡄𐡍 (virtual_op)
	{
	    getvcol(curwin, &end, &cs, عدم, &ce, 0);
	    𐡄𐡍 (p[endcol] == NUL || (cs + end.coladd < ce
			// Don't add space for double-wide
			// char; endcol will be on last byte
			// of multi-byte char.
			&& (*mb_head_off)(p, p + endcol) == 0))
	    {
		𐡄𐡍 (start.lnum == end.lnum && start.col == end.col)
		{
		    // Special case: inside a single char
		    bdp->is_oneChar = TRUE;
		    bdp->startspaces = end.coladd - start.coladd + inclusive;
		    endcol = startcol;
		}
		وإلا
		{
		    bdp->endspaces = end.coladd + inclusive;
		    endcol -= inclusive;
		}
	    }
	}
    }
    𐡄𐡍 (endcol == MAXCOL)
	endcol = ml_get_len(lnum);
    𐡄𐡍 (startcol > endcol || bdp->is_oneChar)
	bdp->textlen = 0;
    وإلا
	bdp->textlen = endcol - startcol + inclusive;
    bdp->textcol = startcol;
    bdp->textstart = startcol <= plen ? p + startcol : p;
}

/*
 * Handle the add/subtract operator.
 */
    فراغ
op_addsub(
    oparg_T	*oap,
    linenr_T	Prenum1,	    // Amount of add/subtract
    صحيح		g_cmd)		    // was g<c-a>/g<c-x>
{
    pos_T		pos;
    بنية block_def	bd;
    صحيح			change_cnt = 0;
    linenr_T		amount = Prenum1;

    // do_addsub() might trigger re-evaluation of 'foldexpr' halfway, when the
    // buffer is not completely updated yet. Postpone updating folds until before
    // the call to changed_lines().
#ifdef FEAT_FOLDING
    disable_fold_update++;
#endif

    𐡄𐡍 (!VIsual_active)
    {
	pos = curwin->w_cursor;
	𐡄𐡍 (u_save_cursor() == FAIL)
	{
#ifdef FEAT_FOLDING
	    disable_fold_update--;
#endif
	    أرجع;
	}
	change_cnt = do_addsub(oap->op_type, &pos, 0, amount);
#ifdef FEAT_FOLDING
	disable_fold_update--;
#endif
	𐡄𐡍 (change_cnt)
	    changed_lines(pos.lnum, 0, pos.lnum + 1, 0L);
    }
    وإلا
    {
	صحيح	one_change;
	صحيح	length;
	pos_T	startpos;

	𐡄𐡍 (u_save((linenr_T)(oap->start.lnum - 1),
					(linenr_T)(oap->end.lnum + 1)) == FAIL)
	{
#ifdef FEAT_FOLDING
	    disable_fold_update--;
#endif
	    أرجع;
	}

	pos = oap->start;
	لكل (; pos.lnum <= oap->end.lnum; ++pos.lnum)
	{
	    𐡄𐡍 (oap->block_mode)		    // Visual block mode
	    {
		block_prep(oap, &bd, pos.lnum, FALSE);
		pos.col = bd.textcol;
		length = bd.textlen;
	    }
	    وإلا 𐡄𐡍 (oap->motion_type == MLINE)
	    {
		curwin->w_cursor.col = 0;
		pos.col = 0;
		length = ml_get_len(pos.lnum);
	    }
	    وإلا // oap->motion_type == MCHAR
	    {
		𐡄𐡍 (pos.lnum == oap->start.lnum && !oap->inclusive)
		    dec(&(oap->end));
		length = ml_get_len(pos.lnum);
		pos.col = 0;
		𐡄𐡍 (pos.lnum == oap->start.lnum)
		{
		    pos.col += oap->start.col;
		    length -= oap->start.col;
		}
		𐡄𐡍 (pos.lnum == oap->end.lnum)
		{
		    length = ml_get_len(oap->end.lnum);
		    𐡄𐡍 (oap->end.col >= length)
			oap->end.col = length - 1;
		    length = oap->end.col - pos.col + 1;
		}
	    }
	    one_change = do_addsub(oap->op_type, &pos, length, amount);
	    𐡄𐡍 (one_change)
	    {
		// Remember the start position of the first change.
		𐡄𐡍 (change_cnt == 0)
		    startpos = curbuf->b_op_start;
		++change_cnt;
	    }

#ifdef FEAT_NETBEANS_INTG
	    𐡄𐡍 (netbeans_active() && one_change)
	    {
		char_u *ptr;

		netbeans_removed(curbuf, pos.lnum, pos.col, (long)length);
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, pos.col,
						&ptr[pos.col], length);
	    }
#endif
	    𐡄𐡍 (g_cmd && one_change)
		amount += Prenum1;
	}

#ifdef FEAT_FOLDING
	disable_fold_update--;
#endif
	𐡄𐡍 (change_cnt)
	    changed_lines(oap->start.lnum, 0, oap->end.lnum + 1, 0L);

	𐡄𐡍 (!change_cnt && oap->is_VIsual)
	    // No change: need to remove the Visual selection
	    redraw_curbuf_later(UPD_INVERTED);

	// Set '[ mark if something changed. Keep the last end
	// position from do_addsub().
	𐡄𐡍 (change_cnt > 0 && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	    curbuf->b_op_start = startpos;

	𐡄𐡍 (change_cnt > p_report)
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
    ساكن صحيح
do_addsub(
    صحيح		op_type,
    pos_T	*pos,
    صحيح		length,
    linenr_T	Prenum1)
{
    صحيح		col;
    صحيح		pre;		// 'X'/'x': hex; '0': octal; 'B'/'b': bin
    ساكن صحيح	hexupper = FALSE;	// 0xABC
    uvarnumber_T	n;
    uvarnumber_T	oldn;
    char_u	*ptr;
    صحيح		linelen;
    صحيح		c;
    صحيح		todel;
    صحيح		do_hex;
    صحيح		do_oct;
    صحيح		do_bin;
    صحيح		do_alpha;
    صحيح		do_unsigned;
    صحيح		do_blank;
    صحيح		blank_unsigned = FALSE;	// blank: treat as unsigned?
    صحيح		firstdigit;
    صحيح		subtract;
    صحيح		negative = FALSE;
    صحيح		was_positive = TRUE;
    صحيح		visual = VIsual_active;
    صحيح		did_change = FALSE;
    pos_T	save_cursor = curwin->w_cursor;
    صحيح		maxlen = 0;
    pos_T	startpos;
    pos_T	endpos;
    colnr_T	save_coladd = 0;

    do_hex = (vim_strchr(curbuf->b_p_nf, 'x') != عدم);	// "heX"
    do_oct = (vim_strchr(curbuf->b_p_nf, 'o') != عدم);	// "Octal"
    do_bin = (vim_strchr(curbuf->b_p_nf, 'b') != عدم);	// "Bin"
    do_alpha = (vim_strchr(curbuf->b_p_nf, 'p') != عدم);	// "alPha"
    do_unsigned = (vim_strchr(curbuf->b_p_nf, 'u') != عدم);	// "Unsigned"
    do_blank = (vim_strchr(curbuf->b_p_nf, 'k') != عدم);	// "blanK"

    𐡄𐡍 (virtual_active())
    {
	save_coladd = pos->coladd;
	pos->coladd = 0;
    }

    curwin->w_cursor = *pos;
    ptr = ml_get(pos->lnum);
    linelen = ml_get_len(pos->lnum);
    col = pos->col;

    𐡄𐡍 (col + !!save_coladd >= linelen)
	اذهب theend;

    /*
     * First check if we are on a hexadecimal number, after the "0x".
     */
    𐡄𐡍 (!VIsual_active)
    {
	𐡄𐡍 (do_bin)
	    طالما (col > 0 && vim_isbdigit(ptr[col]))
	    {
		--col;
		𐡄𐡍 (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }

	𐡄𐡍 (do_hex)
	    طالما (col > 0 && vim_isxdigit(ptr[col]))
	    {
		--col;
		𐡄𐡍 (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }

	𐡄𐡍 (       do_bin
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

	    طالما (col > 0 && vim_isdigit(ptr[col]))
	    {
		col--;
		𐡄𐡍 (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }
	}

	𐡄𐡍 ((       do_hex
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
	    𐡄𐡍 (has_mbyte)
		col -= (*mb_head_off)(ptr, ptr + col);
	}
	وإلا
	{
	    /*
	     * Search forward and then backward to find the start of number.
	     */
	    col = pos->col;

	    طالما (ptr[col] != NUL
		    && !vim_isdigit(ptr[col])
		    && !(do_alpha && ASCII_ISALPHA(ptr[col])))
		col += mb_ptr2len(ptr + col);

	    طالما (col > 0
		    && vim_isdigit(ptr[col - 1])
		    && !(do_alpha && ASCII_ISALPHA(ptr[col])))
	    {
		--col;
		𐡄𐡍 (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }
	}
    }

    𐡄𐡍 (visual)
    {
	طالما (ptr[col] != NUL && length > 0
		&& !vim_isdigit(ptr[col])
		&& !(do_alpha && ASCII_ISALPHA(ptr[col])))
	{
	    صحيح mb_len = mb_ptr2len(ptr + col);

	    col += mb_len;
	    length -= mb_len;
	}

	𐡄𐡍 (length == 0)
	    اذهب theend;

	𐡄𐡍 (col > pos->col && ptr[col - 1] == '-'
		&& (!has_mbyte || !(*mb_head_off)(ptr, ptr + col - 1))
		&& !do_unsigned)
	{
	    𐡄𐡍 (do_blank && col >= 2 && !VIM_ISWHITE(ptr[col - 2]))
		blank_unsigned = TRUE;
	    وإلا
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
    𐡄𐡍 (!VIM_ISDIGIT(firstdigit) && !(do_alpha && ASCII_ISALPHA(firstdigit)))
    {
	beep_flush();
	اذهب theend;
    }

    𐡄𐡍 (do_alpha && ASCII_ISALPHA(firstdigit))
    {
	// decrement or increment alphabetic character
	𐡄𐡍 (op_type == OP_NR_SUB)
	{
	    𐡄𐡍 (CharOrd(firstdigit) < Prenum1)
	    {
		𐡄𐡍 (SAFE_isupper(firstdigit))
		    firstdigit = 'A';
		وإلا
		    firstdigit = 'a';
	    }
	    وإلا
		firstdigit -= Prenum1;
	}
	وإلا
	{
	    𐡄𐡍 (26 - CharOrd(firstdigit) - 1 < Prenum1)
	    {
		𐡄𐡍 (SAFE_isupper(firstdigit))
		    firstdigit = 'Z';
		وإلا
		    firstdigit = 'z';
	    }
	    وإلا
		firstdigit += Prenum1;
	}
	curwin->w_cursor.col = col;
	𐡄𐡍 (!did_change)
	    startpos = curwin->w_cursor;
	did_change = TRUE;
	(فراغ)del_char(FALSE);
	ins_char(firstdigit);
	endpos = curwin->w_cursor;
	curwin->w_cursor.col = col;
    }
    وإلا
    {
	char_u	*buf1;
	صحيح	buf1len;
	char_u	buf2[NUMBUFLEN];
	صحيح	buf2len;
	pos_T	save_pos;
	صحيح	i;

	𐡄𐡍 (col > 0 && ptr[col - 1] == '-'
		&& (!has_mbyte ||
		    !(*mb_head_off)(ptr, ptr + col - 1))
		&& !visual
		&& !do_unsigned)
	{
	    𐡄𐡍 (do_blank && col >= 2 && !VIM_ISWHITE(ptr[col - 2]))
		blank_unsigned = TRUE;
	    وإلا
	    {
		// negative number
		--col;
		negative = TRUE;
	    }
	}

	// get the number value (unsigned)
	𐡄𐡍 (visual && VIsual_mode != 'V')
	    maxlen = (curbuf->b_visual.vi_curswant == MAXCOL
		    ? linelen - col : length);

	صحيح overflow = FALSE;
	vim_str2nr(ptr + col, &pre, &length,
		0 + (do_bin ? STR2NR_BIN : 0)
		    + (do_oct ? STR2NR_OCT : 0)
		    + (do_hex ? STR2NR_HEX : 0),
		عدم, &n, maxlen, FALSE, &overflow);

	// ignore leading '-' for hex and octal and bin numbers
	𐡄𐡍 (pre && negative)
	{
	    ++col;
	    --length;
	    negative = FALSE;
	}
	// add or subtract
	subtract = FALSE;
	𐡄𐡍 (op_type == OP_NR_SUB)
	    subtract ^= TRUE;
	𐡄𐡍 (negative)
	    subtract ^= TRUE;

	oldn = n;
	𐡄𐡍 (!overflow)  // if number is too big don't add/subtract
	{
	    𐡄𐡍 (subtract)
		n -= (uvarnumber_T)Prenum1;
	    وإلا
		n += (uvarnumber_T)Prenum1;
	}

	// handle wraparound for decimal numbers
	𐡄𐡍 (!pre)
	{
	    𐡄𐡍 (subtract)
	    {
		𐡄𐡍 (n > oldn)
		{
		    n = 1 + (n ^ (uvarnumber_T)-1);
		    negative ^= TRUE;
		}
	    }
	    وإلا
	    {
		// add
		𐡄𐡍 (n < oldn)
		{
		    n = (n ^ (uvarnumber_T)-1);
		    negative ^= TRUE;
		}
	    }
	    𐡄𐡍 (n == 0)
		negative = FALSE;
	}

	𐡄𐡍 ((do_unsigned || blank_unsigned) && negative)
	{
	    𐡄𐡍 (subtract)
		// sticking at zero.
		n = (uvarnumber_T)0;
	    وإلا
		// sticking at 2^64 - 1.
		n = (uvarnumber_T)(-1);
	    negative = FALSE;
	}

	𐡄𐡍 (visual && !was_positive && !negative && col > 0)
	{
	    // need to remove the '-'
	    col--;
	    length++;
	}

	/*
	 * Delete the old number.
	 */
	curwin->w_cursor.col = col;
	𐡄𐡍 (!did_change)
	    startpos = curwin->w_cursor;
	did_change = TRUE;
	todel = length;
	c = gchar_cursor();
	/*
	 * Don't include the '-' in the length, only the length of the
	 * part after it is kept the same.
	 */
	𐡄𐡍 (c == '-')
	    --length;

	save_pos = curwin->w_cursor;
	لكل (i = 0; i < todel; ++i)
	{
	    𐡄𐡍 (c < 0x100 && SAFE_isalpha(c))
	    {
		𐡄𐡍 (SAFE_isupper(c))
		    hexupper = TRUE;
		وإلا
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
	𐡄𐡍 (buf1 == عدم)
	    اذهب theend;
	ptr = buf1;
	𐡄𐡍 (negative && (!visual || was_positive))
	    *ptr++ = '-';
	𐡄𐡍 (pre)
	{
	    *ptr++ = '0';
	    --length;
	}
	𐡄𐡍 (pre == 'b' || pre == 'B' ||
	    pre == 'x' || pre == 'X')
	{
	    *ptr++ = pre;
	    --length;
	}

	/*
	 * Put the number characters in buf2[].
	 */
	𐡄𐡍 (pre == 'b' || pre == 'B')
	{
	    صحيح bit = 0;
	    صحيح bits = حجم(uvarnumber_T) * 8;

	    // leading zeros
	    لكل (bit = bits; bit > 0; bit--)
		𐡄𐡍 ((n >> (bit - 1)) & 0x1) اكسر;

	    لكل (buf2len = 0; bit > 0 && buf2len < (NUMBUFLEN - 1); bit--)
		buf2[buf2len++] = ((n >> (bit - 1)) & 0x1) ? '1' : '0';

	    buf2[buf2len] = NUL;
	}
	وإلا 𐡄𐡍 (pre == 0)
	    buf2len = vim_snprintf((حرف *)buf2, NUMBUFLEN, "%llu", n);
	وإلا 𐡄𐡍 (pre == '0')
	    buf2len = vim_snprintf((حرف *)buf2, NUMBUFLEN, "%llo", n);
	وإلا 𐡄𐡍 (pre && hexupper)
	    buf2len = vim_snprintf((حرف *)buf2, NUMBUFLEN, "%llX", n);
	وإلا
	    buf2len = vim_snprintf((حرف *)buf2, NUMBUFLEN, "%llx", n);
	length -= buf2len;

	/*
	 * Adjust number of zeros to the new number of digits, so the
	 * total length of the number remains the same.
	 * Don't do this when
	 * the result may look like an octal number.
	 */
	𐡄𐡍 (firstdigit == '0' && !(do_oct && pre == 0))
	    طالما (length-- > 0)
		*ptr++ = '0';
	*ptr = NUL;
	buf1len = (صحيح)(ptr - buf1);

	STRCPY(buf1 + buf1len, buf2);
	buf1len += buf2len;

	// Insert just after the first character to be removed, so that any
	// text properties will be adjusted.  Then delete the old number
	// afterwards.
	save_pos = curwin->w_cursor;
	𐡄𐡍 (todel > 0)
	    inc_cursor();
	ins_str(buf1, (size_t)buf1len);		// insert the new number
	vim_free(buf1);

	// del_char() will also mark line needing displaying
	𐡄𐡍 (todel > 0)
	{
	    صحيح bytes_after = ml_get_curline_len() - curwin->w_cursor.col;

	    // Delete the one character before the insert.
	    curwin->w_cursor = save_pos;
	    (فراغ)del_char(FALSE);
	    curwin->w_cursor.col = ml_get_curline_len() - bytes_after;
	    --todel;
	}
	طالما (todel-- > 0)
	    (فراغ)del_char(FALSE);

	endpos = curwin->w_cursor;
	𐡄𐡍 (did_change && curwin->w_cursor.col)
	    --curwin->w_cursor.col;
    }

    𐡄𐡍 (did_change && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// set the '[ and '] marks
	curbuf->b_op_start = startpos;
	curbuf->b_op_end = endpos;
	𐡄𐡍 (curbuf->b_op_end.col > 0)
	    --curbuf->b_op_end.col;
    }

theend:
    𐡄𐡍 (visual)
	curwin->w_cursor = save_cursor;
    وإلا 𐡄𐡍 (did_change)
	curwin->w_set_curswant = true;
    وإلا 𐡄𐡍 (virtual_active())
	curwin->w_cursor.coladd = save_coladd;

    أرجع did_change;
}

    فراغ
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
    ساكن varnumber_T
line_count_info(
    char_u	*line,
    varnumber_T	*wc,
    varnumber_T	*cc,
    varnumber_T	limit,
    صحيح		eol_size)
{
    varnumber_T	i;
    varnumber_T	words = 0;
    varnumber_T	chars = 0;
    صحيح		is_word = 0;

    لكل (i = 0; i < limit && line[i] != NUL; )
    {
	𐡄𐡍 (is_word)
	{
	    𐡄𐡍 (vim_isspace(line[i]))
	    {
		words++;
		is_word = 0;
	    }
	}
	وإلا 𐡄𐡍 (!vim_isspace(line[i]))
	    is_word = 1;
	++chars;
	i += (*mb_ptr2len)(line + i);
    }

    𐡄𐡍 (is_word)
	words++;
    *wc += words;

    // Add eol_size if the end of line was reached before hitting limit.
    𐡄𐡍 (i < limit && line[i] == NUL)
    {
	i += eol_size;
	chars += eol_size;
    }
    *cc += chars;
    أرجع i;
}

/*
 * Give some info about the position of the cursor (for "g CTRL-G").
 * In Visual mode, give some info about the selected region.  (In this case,
 * the *_count_cursor variables store running totals for the selection.)
 * When "dict" is not NULL store the info there instead of showing it.
 */
    فراغ
cursor_pos_info(dict_T *dict)
{
    char_u	*p;
    char_u	buf1[50];
    char_u	buf2[40];
    linenr_T	lnum;
    varnumber_T	byte_count = 0;
    varnumber_T	bom_count  = 0;
    varnumber_T	byte_count_cursor = 0;
    varnumber_T	char_count = 0;
    varnumber_T	char_count_cursor = 0;
    varnumber_T	word_count = 0;
    varnumber_T	word_count_cursor = 0;
    صحيح		eol_size;
    varnumber_T	last_check = 100000L;
    long	line_count_selected = 0;
    pos_T	min_pos, max_pos;
    oparg_T	oparg;
    بنية block_def	bd;

    /*
     * Compute the length of the file in characters.
     */
    𐡄𐡍 (curbuf->b_ml.ml_flags & ML_EMPTY)
    {
	𐡄𐡍 (dict == عدم)
	{
	    msg(_(no_lines_msg));
	    أرجع;
	}
    }
    وإلا
    {
	𐡄𐡍 (get_fileformat(curbuf) == EOL_DOS)
	    eol_size = 2;
	وإلا
	    eol_size = 1;

	𐡄𐡍 (VIsual_active)
	{
	    𐡄𐡍 (LT_POS(VIsual, curwin->w_cursor))
	    {
		min_pos = VIsual;
		max_pos = curwin->w_cursor;
	    }
	    وإلا
	    {
		min_pos = curwin->w_cursor;
		max_pos = VIsual;
	    }
	    𐡄𐡍 (*p_sel == 'e' && max_pos.col > 0)
		--max_pos.col;

	    𐡄𐡍 (VIsual_mode == Ctrl_V)
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
		𐡄𐡍 (curwin->w_curswant == MAXCOL)
		    oparg.end_vcol = MAXCOL;
		// Swap the start, end vcol if needed
		𐡄𐡍 (oparg.end_vcol < oparg.start_vcol)
		{
		    oparg.end_vcol += oparg.start_vcol;
		    oparg.start_vcol = oparg.end_vcol - oparg.start_vcol;
		    oparg.end_vcol -= oparg.start_vcol;
		}
	    }
	    line_count_selected = max_pos.lnum - min_pos.lnum + 1;
	}

	لكل (lnum = 1; lnum <= curbuf->b_ml.ml_line_count; ++lnum)
	{
	    // Check for a CTRL-C every 100000 characters.
	    𐡄𐡍 (byte_count > last_check)
	    {
		ui_breakcheck();
		𐡄𐡍 (got_int)
		    أرجع;
		last_check = byte_count + 100000L;
	    }

	    // Do extra processing for VIsual mode.
	    𐡄𐡍 (VIsual_active
		    && lnum >= min_pos.lnum && lnum <= max_pos.lnum)
	    {
		char_u	    *s = عدم;
		long	    len = 0L;

		بدل (VIsual_mode)
		{
		    حالة Ctrl_V:
			virtual_op = virtual_active();
			block_prep(&oparg, &bd, lnum, 0);
			virtual_op = MAYBE;
			s = bd.textstart;
			len = (long)bd.textlen;
			اكسر;
		    حالة 'V':
			s = ml_get(lnum);
			len = MAXCOL;
			اكسر;
		    حالة 'v':
			{
			    colnr_T start_col = (lnum == min_pos.lnum)
							   ? min_pos.col : 0;
			    colnr_T end_col = (lnum == max_pos.lnum)
				      ? max_pos.col - start_col + 1 : MAXCOL;

			    s = ml_get(lnum) + start_col;
			    len = end_col;
			}
			اكسر;
		}
		𐡄𐡍 (s != عدم)
		{
		    byte_count_cursor += line_count_info(s, &word_count_cursor,
					   &char_count_cursor, len, eol_size);
		    𐡄𐡍 (lnum == curbuf->b_ml.ml_line_count
			    && !curbuf->b_p_eol
			    && (curbuf->b_p_bin || !curbuf->b_p_fixeol)
			    && (long)STRLEN(s) < len)
			byte_count_cursor -= eol_size;
		}
	    }
	    وإلا
	    {
		// In non-visual mode, check for the line the cursor is on
		𐡄𐡍 (lnum == curwin->w_cursor.lnum)
		{
		    word_count_cursor += word_count;
		    char_count_cursor += char_count;
		    byte_count_cursor = byte_count +
			line_count_info(ml_get(lnum),
				&word_count_cursor, &char_count_cursor,
				(varnumber_T)(curwin->w_cursor.col + 1),
				eol_size);
		}
	    }
	    // Add to the running totals
	    byte_count += line_count_info(ml_get(lnum), &word_count,
					 &char_count, (varnumber_T)MAXCOL,
					 eol_size);
	}

	// Correction for when last line doesn't have an EOL.
	𐡄𐡍 (!curbuf->b_p_eol && (curbuf->b_p_bin || !curbuf->b_p_fixeol))
	    byte_count -= eol_size;

	𐡄𐡍 (dict == عدم)
	{
	    𐡄𐡍 (VIsual_active)
	    {
		𐡄𐡍 (VIsual_mode == Ctrl_V && curwin->w_curswant < MAXCOL)
		{
		    getvcols(curwin, &min_pos, &max_pos,
						&min_pos.col, &max_pos.col, 0);
		    vim_snprintf((حرف *)buf1, حجم(buf1), _("%ld Cols; "),
			    (long)(oparg.end_vcol - oparg.start_vcol + 1));
		}
		وإلا
		    buf1[0] = NUL;

		𐡄𐡍 (char_count_cursor == byte_count_cursor
						    && char_count == byte_count)
		    vim_snprintf((حرف *)IObuff, IOSIZE,
			    _("Selected %s%ld of %ld Lines; %lld of %lld Words; %lld of %lld Bytes"),
			    buf1, line_count_selected,
			    (long)curbuf->b_ml.ml_line_count,
			    word_count_cursor,
			    word_count,
			    byte_count_cursor,
			    byte_count);
		وإلا
		    vim_snprintf((حرف *)IObuff, IOSIZE,
			    _("Selected %s%ld of %ld Lines; %lld of %lld Words; %lld of %lld Chars; %lld of %lld Bytes"),
			    buf1, line_count_selected,
			    (long)curbuf->b_ml.ml_line_count,
			    word_count_cursor,
			    word_count,
			    char_count_cursor,
			    char_count,
			    byte_count_cursor,
			    byte_count);
	    }
	    وإلا
	    {
		p = ml_get_curline();
		validate_virtcol();
		col_print(buf1, حجم(buf1), (صحيح)curwin->w_cursor.col + 1,
			(صحيح)curwin->w_virtcol + 1);
		col_print(buf2, حجم(buf2), ml_get_curline_len(),
							   linetabsize_str(p));

		𐡄𐡍 (char_count_cursor == byte_count_cursor
			&& char_count == byte_count)
		    vim_snprintf((حرف *)IObuff, IOSIZE,
			_("Col %s of %s; Line %ld of %ld; Word %lld of %lld; Byte %lld of %lld"),
			(حرف *)buf1, (حرف *)buf2,
			(long)curwin->w_cursor.lnum,
			(long)curbuf->b_ml.ml_line_count,
			word_count_cursor, word_count,
			byte_count_cursor, byte_count);
		وإلا
		    vim_snprintf((حرف *)IObuff, IOSIZE,
			_("Col %s of %s; Line %ld of %ld; Word %lld of %lld; Char %lld of %lld; Byte %lld of %lld"),
			(حرف *)buf1, (حرف *)buf2,
			(long)curwin->w_cursor.lnum,
			(long)curbuf->b_ml.ml_line_count,
			word_count_cursor, word_count,
			char_count_cursor, char_count,
			byte_count_cursor, byte_count);
	    }
	}

	bom_count = bomb_size();
	𐡄𐡍 (dict == عدم && bom_count > 0)
	{
	    size_t len = STRLEN(IObuff);

	    vim_snprintf((حرف *)IObuff + len, IOSIZE - len,
				 _("(+%lld for BOM)"), bom_count);
	}
	𐡄𐡍 (dict == عدم)
	{
	    // Don't shorten this message, the user asked for it.
	    p = p_shm;
	    p_shm = (char_u *)"";
	    msg((حرف *)IObuff);
	    p_shm = p;
	}
    }
#𐡄𐡍 defined(FEAT_EVAL)
    𐡄𐡍 (dict != عدم)
    {
	dict_add_number(dict, "words", word_count);
	dict_add_number(dict, "chars", char_count);
	dict_add_number(dict, "bytes", byte_count + bom_count);
	dict_add_number(dict, VIsual_active ? "visual_bytes" : "cursor_bytes",
		byte_count_cursor);
	dict_add_number(dict, VIsual_active ? "visual_chars" : "cursor_chars",
		char_count_cursor);
	dict_add_number(dict, VIsual_active ? "visual_words" : "cursor_words",
		word_count_cursor);
    }
#endif
}

/*
 * Handle indent and format operators and visual mode ":".
 */
    ساكن فراغ
op_colon(oparg_T *oap)
{
    stuffcharReadbuff(':');
    𐡄𐡍 (oap->is_VIsual)
	stuffReadbuff((char_u *)"'<,'>");
    وإلا
    {
	// Make the range look nice, so it can be repeated.
	𐡄𐡍 (oap->start.lnum == curwin->w_cursor.lnum)
	    stuffcharReadbuff('.');
	وإلا
	    stuffnumReadbuff((long)oap->start.lnum);

#ifdef FEAT_FOLDING
	// When using !! on a closed fold the range ".!" works best to operate
	// on, it will be made the whole closed fold later.
	linenr_T endOfStartFold = oap->start.lnum;
	(فراغ)hasFolding(oap->start.lnum, عدم, &endOfStartFold);
#endif
	𐡄𐡍 (oap->end.lnum != oap->start.lnum
#ifdef FEAT_FOLDING
		&& oap->end.lnum != endOfStartFold
#endif
	   )
	{
	    // Make it a range with the end line.
	    stuffcharReadbuff(',');
	    𐡄𐡍 (oap->end.lnum == curwin->w_cursor.lnum)
		stuffcharReadbuff('.');
	    وإلا 𐡄𐡍 (oap->end.lnum == curbuf->b_ml.ml_line_count)
		stuffcharReadbuff('$');
	    وإلا 𐡄𐡍 (oap->start.lnum == curwin->w_cursor.lnum
#ifdef FEAT_FOLDING
		    // do not use ".+number" for a closed fold, it would count
		    // folded lines twice
		    && !hasFolding(oap->end.lnum, عدم, عدم)
#endif
		    )
	    {
		stuffReadbuff((char_u *)".+");
		stuffnumReadbuff((long)oap->line_count - 1);
	    }
	    وإلا
		stuffnumReadbuff((long)oap->end.lnum);
	}
    }
    𐡄𐡍 (oap->op_type != OP_COLON)
	stuffReadbuff((char_u *)"!");
    𐡄𐡍 (oap->op_type == OP_INDENT)
    {
	𐡄𐡍 (*get_equalprg() == NUL)
	    stuffReadbuff((char_u *)"indent");
	وإلا
	    stuffReadbuff(get_equalprg());
	stuffReadbuff((char_u *)"\n");
    }
    وإلا 𐡄𐡍 (oap->op_type == OP_FORMAT)
    {
	𐡄𐡍 (*curbuf->b_p_fp != NUL)
	    stuffReadbuff(curbuf->b_p_fp);
	وإلا 𐡄𐡍 (*p_fp != NUL)
	    stuffReadbuff(p_fp);
	وإلا
	    stuffReadbuff((char_u *)"fmt");
	stuffReadbuff((char_u *)"\n']");
    }

    // do_cmdline() does the rest
}

// callback function for 'operatorfunc'
ساكن callback_T opfunc_cb;

/*
 * Process the 'operatorfunc' option value.
 * Returns OK or FAIL.
 */
    حرف *
did_set_operatorfunc(optset_T *args UNUSED)
{
    𐡄𐡍 (option_set_callback_func(p_opfunc, &opfunc_cb) == FAIL)
	أرجع e_invalid_argument;

    أرجع عدم;
}

#𐡄𐡍 defined(EXITFREE)
    فراغ
free_operatorfunc_option(فراغ)
{
# ifdef FEAT_EVAL
    free_callback(&opfunc_cb);
# endif
}
#endif

#𐡄𐡍 defined(FEAT_EVAL)
/*
 * Mark the global 'operatorfunc' callback with "copyID" so that it is not
 * garbage collected.
 */
    صحيح
set_ref_in_opfunc(صحيح copyID UNUSED)
{
    صحيح abort = FALSE;

    abort = set_ref_in_callback(&opfunc_cb, copyID);

    أرجع abort;
}
#endif

/*
 * Handle the "g@" operator: call 'operatorfunc'.
 */
    ساكن فراغ
op_function(oparg_T *oap UNUSED)
{
#ifdef FEAT_EVAL
    typval_T	argv[2];
    pos_T	orig_start = curbuf->b_op_start;
    pos_T	orig_end = curbuf->b_op_end;
    typval_T	rettv;

    𐡄𐡍 (*p_opfunc == NUL)
	emsg(_(e_operatorfunc_is_empty));
    وإلا
    {
	// Set '[ and '] marks to text to be operated on.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
	𐡄𐡍 (oap->motion_type != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_end);

	argv[0].v_type = VAR_STRING;
	𐡄𐡍 (oap->block_mode)
	    argv[0].vval.v_string = (char_u *)"block";
	وإلا 𐡄𐡍 (oap->motion_type == MLINE)
	    argv[0].vval.v_string = (char_u *)"line";
	وإلا
	    argv[0].vval.v_string = (char_u *)"char";
	argv[1].v_type = VAR_UNKNOWN;

	// Reset virtual_op so that 'virtualedit' can be changed in the
	// function.
	صحيح save_virtual_op = virtual_op;
	virtual_op = MAYBE;

	// Reset finish_op so that mode() returns the right value.
	صحيح save_finish_op = finish_op;
	finish_op = FALSE;

	𐡄𐡍 (call_callback(&opfunc_cb, 0, &rettv, 1, argv) != FAIL)
	    clear_tv(&rettv);

	virtual_op = save_virtual_op;
	finish_op = save_finish_op;
	𐡄𐡍 (cmdmod.cmod_flags & CMOD_LOCKMARKS)
	{
	    curbuf->b_op_start = orig_start;
	    curbuf->b_op_end = orig_end;
	}
    }
#وإلا
    emsg(_(e_eval_feature_not_available));
#endif
}

/*
 * Calculate start/end virtual columns for operating in block mode.
 */
    ساكن فراغ
get_op_vcol(
    oparg_T	*oap,
    colnr_T	redo_VIsual_vcol,
    صحيح		initial)    // when TRUE adjust position for 'selectmode'
{
    colnr_T	    start, end;

    𐡄𐡍 (VIsual_mode != Ctrl_V
	    || (!initial && oap->end.col < curwin->w_width))
	أرجع;

    oap->block_mode = TRUE;

    // prevent from moving onto a trail byte
    𐡄𐡍 (has_mbyte)
	mb_adjustpos(curwin->w_buffer, &oap->end);

    getvvcol(curwin, &(oap->start), &oap->start_vcol, عدم, &oap->end_vcol, 0);

    𐡄𐡍 (!redo_VIsual_busy)
    {
	getvvcol(curwin, &(oap->end), &start, عدم, &end, 0);

	𐡄𐡍 (start < oap->start_vcol)
	    oap->start_vcol = start;
	𐡄𐡍 (end > oap->end_vcol)
	{
	    𐡄𐡍 (initial && *p_sel == 'e' && start >= 1
				    && start - 1 >= oap->end_vcol)
		oap->end_vcol = start - 1;
	    وإلا
		oap->end_vcol = end;
	}
    }

    // if '$' was used, get oap->end_vcol from longest line
    𐡄𐡍 (curwin->w_curswant == MAXCOL)
    {
	curwin->w_cursor.col = MAXCOL;
	oap->end_vcol = 0;
	لكل (curwin->w_cursor.lnum = oap->start.lnum;
		curwin->w_cursor.lnum <= oap->end.lnum;
					++curwin->w_cursor.lnum)
	{
	    getvvcol(curwin, &curwin->w_cursor, عدم, عدم, &end, 0);
	    𐡄𐡍 (end > oap->end_vcol)
		oap->end_vcol = end;
	}
    }
    وإلا 𐡄𐡍 (redo_VIsual_busy)
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
تعريفنوع بنية {
    صحيح		rv_mode;	// 'v', 'V', or Ctrl-V
    linenr_T	rv_line_count;	// number of lines
    colnr_T	rv_vcol;	// number of cols or end column
    long	rv_count;	// count for Visual operator
    صحيح		rv_arg;		// extra argument
} redo_VIsual_T;

    ساكن صحيح
is_ex_cmdchar(cmdarg_T *cap)
{
    أرجع cap->cmdchar == ':'
	|| cap->cmdchar == K_COMMAND
	|| cap->cmdchar == K_SCRIPT_COMMAND;
}

/*
 * Handle an operator after Visual mode or when the movement is finished.
 * "gui_yank" is true when yanking text for the clipboard.
 */
    فراغ
do_pending_operator(cmdarg_T *cap, صحيح old_col, صحيح gui_yank)
{
    oparg_T	*oap = cap->oap;
    pos_T	old_cursor;
    صحيح		empty_region_error;
    صحيح		restart_edit_save;
#ifdef FEAT_LINEBREAK
    صحيح		lbr_saved = curwin->w_p_lbr;
#endif

    // The visual area is remembered for redo
    ساكن redo_VIsual_T   redo_VIsual = {NUL, 0, 0, 0,0};

    صحيح		    include_line_break = FALSE;

#𐡄𐡍 defined(FEAT_CLIPBOARD)
    // Yank the visual area into the GUI selection register before we operate
    // on it and lose it forever.
    // Don't do it if a specific register was specified, so that ""x"*P works.
    // This could call do_pending_operator() recursively, but that's OK
    // because gui_yank will be TRUE for the nested call.
    𐡄𐡍 ((clip_star.available || clip_plus.available)
	    && oap->op_type != OP_NOP
	    && !gui_yank
	    && VIsual_active
	    && !redo_VIsual_busy
	    && oap->regname == 0)
	clip_auto_select();
#endif
    old_cursor = curwin->w_cursor;

    // If an operation is pending, handle it...
    𐡄𐡍 ((finish_op || VIsual_active) && oap->op_type != OP_NOP)
    {
	// Yank can be redone when 'y' is in 'cpoptions', but not when yanking
	// for the clipboard.
	صحيح	redo_yank = vim_strchr(p_cpo, CPO_YANK) != عدم && !gui_yank;

#ifdef FEAT_LINEBREAK
	// Avoid a problem with unwanted linebreaks in block mode.
	(فراغ)reset_lbr();
#endif
	oap->is_VIsual = VIsual_active;
	𐡄𐡍 (oap->motion_force == 'V')
	    oap->motion_type = MLINE;
	وإلا 𐡄𐡍 (oap->motion_force == 'v')
	{
	    // If the motion was linewise, "inclusive" will not have been set.
	    // Use "exclusive" to be consistent.  Makes "dvj" work nice.
	    𐡄𐡍 (oap->motion_type == MLINE)
		oap->inclusive = FALSE;
	    // If the motion already was characterwise, toggle "inclusive"
	    وإلا 𐡄𐡍 (oap->motion_type == MCHAR)
		oap->inclusive = !oap->inclusive;
	    oap->motion_type = MCHAR;
	}
	وإلا 𐡄𐡍 (oap->motion_force == Ctrl_V)
	{
	    // Change line- or characterwise motion into Visual block mode.
	    𐡄𐡍 (!VIsual_active)
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
	𐡄𐡍 ((redo_yank || oap->op_type != OP_YANK)
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
	    𐡄𐡍 (cap->cmdchar == '/' || cap->cmdchar == '?') // was a search
	    {
		// If 'cpoptions' does not contain 'r', insert the search
		// pattern to really repeat the same command.
		𐡄𐡍 (vim_strchr(p_cpo, CPO_REDO) == عدم)
		    AppendToRedobuffLit(cap->searchbuf, -1);
		AppendToRedobuff(NL_STR);
	    }
	    وإلا 𐡄𐡍 (is_ex_cmdchar(cap))
	    {
		// do_cmdline() has stored the first typed line in
		// "repeat_cmdline".  When several lines are typed repeating
		// won't be possible.
		𐡄𐡍 (repeat_cmdline == عدم)
		    ResetRedobuff();
		وإلا
		{
		    𐡄𐡍 (cap->cmdchar == ':')
			AppendToRedobuffLit(repeat_cmdline, -1);
		    وإلا
			AppendToRedobuffSpec(repeat_cmdline);
		    AppendToRedobuff(NL_STR);
		    VIM_CLEAR(repeat_cmdline);
		}
	    }
	}

	𐡄𐡍 (redo_VIsual_busy)
	{
	    // Redo of an operation on a Visual area. Use the same size from
	    // redo_VIsual.rv_line_count and redo_VIsual.rv_vcol.
	    oap->start = curwin->w_cursor;
	    curwin->w_cursor.lnum += redo_VIsual.rv_line_count - 1;
	    𐡄𐡍 (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
	    VIsual_mode = redo_VIsual.rv_mode;
	    𐡄𐡍 (redo_VIsual.rv_vcol == MAXCOL || VIsual_mode == 'v')
	    {
		𐡄𐡍 (VIsual_mode == 'v')
		{
		    𐡄𐡍 (redo_VIsual.rv_line_count <= 1)
		    {
			validate_virtcol();
			curwin->w_curswant =
				     curwin->w_virtcol + redo_VIsual.rv_vcol - 1;
		    }
		    وإلا
			curwin->w_curswant = redo_VIsual.rv_vcol;
		}
		وإلا
		{
		    curwin->w_curswant = MAXCOL;
		}
		coladvance(curwin->w_curswant);
	    }
	    cap->count0 = redo_VIsual.rv_count;
	    𐡄𐡍 (redo_VIsual.rv_count != 0)
		cap->count1 = redo_VIsual.rv_count;
	    وإلا
		cap->count1 = 1;
	}
	وإلا 𐡄𐡍 (VIsual_active)
	{
	    𐡄𐡍 (!gui_yank)
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
	    𐡄𐡍 (VIsual_select && VIsual_mode == 'V'
					    && cap->oap->op_type != OP_DELETE)
	    {
		𐡄𐡍 (LT_POS(VIsual, curwin->w_cursor))
		{
		    VIsual.col = 0;
		    curwin->w_cursor.col = ml_get_len(curwin->w_cursor.lnum);
		}
		وإلا
		{
		    curwin->w_cursor.col = 0;
		    VIsual.col = ml_get_len(VIsual.lnum);
		}
		VIsual_mode = 'v';
	    }
	    // If 'selection' is "exclusive", backup one character for
	    // charwise selections.
	    وإلا 𐡄𐡍 (VIsual_mode == 'v')
		include_line_break = unadjust_for_sel();

	    oap->start = VIsual;
	    𐡄𐡍 (VIsual_mode == 'V')
	    {
		oap->start.col = 0;
		oap->start.coladd = 0;
	    }
	}

	// Set oap->start to the first position of the operated text, oap->end
	// to the end of the operated text.  w_cursor is equal to oap->start.
	𐡄𐡍 (LT_POS(oap->start, curwin->w_cursor))
	{
#ifdef FEAT_FOLDING
	    // Include folded lines completely.
	    𐡄𐡍 (!VIsual_active)
	    {
		𐡄𐡍 (hasFolding(oap->start.lnum, &oap->start.lnum, عدم))
		    oap->start.col = 0;
		𐡄𐡍 ((curwin->w_cursor.col > 0 || oap->inclusive
						  || oap->motion_type == MLINE)
			&& hasFolding(curwin->w_cursor.lnum, عدم,
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
	وإلا
	{
#ifdef FEAT_FOLDING
	    // Include folded lines completely.
	    𐡄𐡍 (!VIsual_active && oap->motion_type == MLINE)
	    {
		𐡄𐡍 (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum,
									عدم))
		    curwin->w_cursor.col = 0;
		𐡄𐡍 (hasFolding(oap->start.lnum, عدم, &oap->start.lnum))
		    oap->start.col = ml_get_len(oap->start.lnum);
	    }
#endif
	    oap->end = oap->start;
	    oap->start = curwin->w_cursor;
	}

	// Just in case lines were deleted that make the position invalid.
	check_pos(curwin->w_buffer, &oap->end);
	oap->line_count = oap->end.lnum - oap->start.lnum + 1;

	// Set "virtual_op" before resetting VIsual_active.
	virtual_op = virtual_active();

	𐡄𐡍 (VIsual_active || redo_VIsual_busy)
	{
	    get_op_vcol(oap, redo_VIsual.rv_vcol, TRUE);

	    𐡄𐡍 (!redo_VIsual_busy && !gui_yank)
	    {
		// Prepare to reselect and redo Visual: this is based on the
		// size of the Visual text
		resel_VIsual_mode = VIsual_mode;
		𐡄𐡍 (curwin->w_curswant == MAXCOL)
		    resel_VIsual_vcol = MAXCOL;
		وإلا
		{
		    𐡄𐡍 (VIsual_mode != Ctrl_V)
			getvvcol(curwin, &(oap->end),
						عدم, عدم, &oap->end_vcol, 0);
		    𐡄𐡍 (VIsual_mode == Ctrl_V || oap->line_count <= 1)
		    {
			𐡄𐡍 (VIsual_mode != Ctrl_V)
			    getvvcol(curwin, &(oap->start),
					      &oap->start_vcol, عدم, عدم, 0);
			resel_VIsual_vcol = oap->end_vcol - oap->start_vcol + 1;
		    }
		    وإلا
			resel_VIsual_vcol = oap->end_vcol;
		}
		resel_VIsual_line_count = oap->line_count;
	    }

	    // can't redo yank (unless 'y' is in 'cpoptions') and ":"
	    𐡄𐡍 ((redo_yank || oap->op_type != OP_YANK)
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
		𐡄𐡍 (cap->cmdchar == 'g' && (cap->nchar == 'n'
							|| cap->nchar == 'N'))
		    prep_redo(oap->regname, cap->count0,
			    get_op_char(oap->op_type),
			    get_extra_op_char(oap->op_type),
			    oap->motion_force, cap->cmdchar, cap->nchar);
		وإلا 𐡄𐡍 (!is_ex_cmdchar(cap))
		{
		    صحيح opchar = get_op_char(oap->op_type);
		    صحيح extra_opchar = get_extra_op_char(oap->op_type);
		    صحيح nchar = oap->op_type == OP_REPLACE ? cap->nchar : NUL;

		    // reverse what nv_replace() did
		    𐡄𐡍 (nchar == REPLACE_CR_NCHAR)
			nchar = CAR;
		    وإلا 𐡄𐡍 (nchar == REPLACE_NL_NCHAR)
			nchar = NL;

		    𐡄𐡍 (opchar == 'g' && extra_opchar == '@')
			// also repeat the count for 'operatorfunc'
			prep_redo_num2(oap->regname, 0L, NUL, 'v',
				     cap->count0, opchar, extra_opchar, nchar);
		    وإلا
			prep_redo(oap->regname, 0L, NUL, 'v',
						  opchar, extra_opchar, nchar);
		}
		𐡄𐡍 (!redo_VIsual_busy)
		{
		    redo_VIsual.rv_mode = resel_VIsual_mode;
		    redo_VIsual.rv_vcol = resel_VIsual_vcol;
		    redo_VIsual.rv_line_count = resel_VIsual_line_count;
		    redo_VIsual.rv_count = cap->count0;
		    redo_VIsual.rv_arg = cap->arg;
		}
	    }

	    // oap->inclusive defaults to TRUE.
	    // If oap->end is on a NUL (empty line) oap->inclusive becomes
	    // FALSE.  This makes "d}P" and "v}dP" work the same.
	    𐡄𐡍 (oap->motion_force == NUL || oap->motion_type == MLINE)
		oap->inclusive = TRUE;
	    𐡄𐡍 (VIsual_mode == 'V')
		oap->motion_type = MLINE;
	    وإلا
	    {
		oap->motion_type = MCHAR;
		𐡄𐡍 (VIsual_mode != Ctrl_V && *ml_get_pos(&(oap->end)) == NUL
			&& (include_line_break || !virtual_op))
		{
		    oap->inclusive = FALSE;
		    // Try to include the newline, unless it's an operator
		    // that works on lines only.
		    𐡄𐡍 (*p_sel != 'o'
			    && !op_on_lines(oap->op_type)
			    && oap->end.lnum < curbuf->b_ml.ml_line_count)
		    {
			++oap->end.lnum;
			oap->end.col = 0;
			oap->end.coladd = 0;
			++oap->line_count;
		    }
		}
	    }

	    redo_VIsual_busy = FALSE;

	    // Switch Visual off now, so screen updating does
	    // not show inverted text when the screen is redrawn.
	    // With OP_YANK and sometimes with OP_COLON and OP_FILTER there is
	    // no screen redraw, so it is done here to remove the inverted
	    // part.
	    𐡄𐡍 (!gui_yank)
	    {
		VIsual_active = FALSE;
		setmouse();
		mouse_dragging = 0;
		may_clear_cmdline();
		𐡄𐡍 ((oap->op_type == OP_YANK
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
	𐡄𐡍 (has_mbyte && oap->inclusive)
	{
	    صحيح		l;

	    l = (*mb_ptr2len)(ml_get_pos(&oap->end));
	    𐡄𐡍 (l > 1)
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
				&& vim_strchr(p_cpo, CPO_EMPTYREGION) != عدم);

	// Force a redraw when operating on an empty Visual region, when
	// 'modifiable is off or creating a fold.
	𐡄𐡍 (oap->is_VIsual && (oap->empty || !curbuf->b_p_ma
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
	𐡄𐡍 (	   oap->motion_type == MCHAR
		&& oap->inclusive == FALSE
		&& !(cap->retval & CA_NO_ADJ_OP_END)
		&& oap->end.col == 0
		&& (!oap->is_VIsual || *p_sel == 'o')
		&& !oap->block_mode
		&& oap->line_count > 1)
	{
	    oap->end_adjusted = TRUE;	    // remember that we did this
	    --oap->line_count;
	    --oap->end.lnum;
	    𐡄𐡍 (inindent(0))
		oap->motion_type = MLINE;
	    وإلا
	    {
		oap->end.col = ml_get_len(oap->end.lnum);
		𐡄𐡍 (oap->end.col)
		{
		    --oap->end.col;
		    oap->inclusive = TRUE;
		}
	    }
	}
	وإلا
	    oap->end_adjusted = FALSE;

	بدل (oap->op_type)
	{
	حالة OP_LSHIFT:
	حالة OP_RSHIFT:
	    op_shift(oap, TRUE, oap->is_VIsual ? (صحيح)cap->count1 : 1);
	    auto_format(FALSE, TRUE);
	    اكسر;

	حالة OP_JOIN_NS:
	حالة OP_JOIN:
	    𐡄𐡍 (oap->line_count < 2)
		oap->line_count = 2;
	    𐡄𐡍 (curwin->w_cursor.lnum + oap->line_count - 1 >
						   curbuf->b_ml.ml_line_count)
		beep_flush();
	    وإلا
	    {
		(فراغ)do_join(oap->line_count, oap->op_type == OP_JOIN,
							    TRUE, TRUE, TRUE);
		auto_format(FALSE, TRUE);
	    }
	    اكسر;

	حالة OP_DELETE:
	    VIsual_reselect = FALSE;	    // don't reselect now
	    𐡄𐡍 (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    وإلا
	    {
		(فراغ)op_delete(oap);
		// save cursor line for undo if it wasn't saved yet
		𐡄𐡍 (oap->motion_type == MLINE && has_format_option(FO_AUTO)
						      && u_save_cursor() == OK)
		    auto_format(FALSE, TRUE);
	    }
	    اكسر;

	حالة OP_YANK:
	    𐡄𐡍 (empty_region_error)
	    {
		𐡄𐡍 (!gui_yank)
		{
		    vim_beep(BO_OPER);
		    CancelRedo();
		}
	    }
	    وإلا
	    {
#ifdef FEAT_LINEBREAK
		restore_lbr(lbr_saved);
#endif
		oap->excl_tr_ws = cap->cmdchar == 'z';
		(فراغ)op_yank(oap, FALSE, !gui_yank);
	    }
	    check_cursor_col();
	    اكسر;

	حالة OP_CHANGE:
	    VIsual_reselect = FALSE;	    // don't reselect now
	    𐡄𐡍 (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    وإلا
	    {
		// This is a new edit command, not a restart.  Need to
		// remember it to make 'insertmode' work with mappings for
		// Visual mode.  But do this only once and not when typed and
		// 'insertmode' isn't set.
		𐡄𐡍 (p_im || !KeyTyped)
		    restart_edit_save = restart_edit;
		وإلا
		    restart_edit_save = 0;
		restart_edit = 0;
#ifdef FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#endif
		// trigger TextChangedI
		curbuf->b_last_changedtick_i = CHANGEDTICK(curbuf);

		𐡄𐡍 (op_change(oap))	// will call edit()
		    cap->retval |= CA_COMMAND_BUSY;
		𐡄𐡍 (restart_edit == 0)
		    restart_edit = restart_edit_save;
	    }
	    اكسر;

	حالة OP_FILTER:
	    𐡄𐡍 (vim_strchr(p_cpo, CPO_FILTER) != عدم)
		AppendToRedobuff((char_u *)"!\r");  // use any last used !cmd
	    وإلا
		bangredo = TRUE;    // do_bang() will put cmd in redo buffer
	    // FALLTHROUGH

	حالة OP_INDENT:
	حالة OP_COLON:

	    // If 'equalprg' is empty, do the indenting internally.
	    𐡄𐡍 (oap->op_type == OP_INDENT && *get_equalprg() == NUL)
	    {
		𐡄𐡍 (curbuf->b_p_lisp)
		{
#ifdef FEAT_EVAL
		    𐡄𐡍 (use_indentexpr_for_lisp())
			op_reindent(oap, get_expr_indent);
		    وإلا
#endif
			op_reindent(oap, get_lisp_indent);
		    اكسر;
		}
		op_reindent(oap,
#ifdef FEAT_EVAL
			*curbuf->b_p_inde != NUL ? get_expr_indent :
#endif
			    get_c_indent);
		اكسر;
	    }

	    op_colon(oap);
	    اكسر;

	حالة OP_TILDE:
	حالة OP_UPPER:
	حالة OP_LOWER:
	حالة OP_ROT13:
	    𐡄𐡍 (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    وإلا
		op_tilde(oap);
	    check_cursor_col();
	    اكسر;

	حالة OP_FORMAT:
#𐡄𐡍 defined(FEAT_EVAL)
	    𐡄𐡍 (*curbuf->b_p_fex != NUL)
		op_formatexpr(oap);	// use expression
	    وإلا
#endif
	    {
		𐡄𐡍 (*p_fp != NUL || *curbuf->b_p_fp != NUL)
		    op_colon(oap);		// use external command
		وإلا
		    op_format(oap, FALSE);	// use internal function
	    }
	    اكسر;
	حالة OP_FORMAT2:
	    op_format(oap, TRUE);	// use internal function
	    اكسر;

	حالة OP_FUNCTION:
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
		اكسر;
	    }

	حالة OP_INSERT:
	حالة OP_APPEND:
	    VIsual_reselect = FALSE;	// don't reselect now
	    𐡄𐡍 (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    وإلا
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

		op_insert(oap, cap->count1);
#ifdef FEAT_LINEBREAK
		// Reset linebreak, so that formatting works correctly.
		(فراغ)reset_lbr();
#endif

		// TODO: when inserting in several lines, should format all
		// the lines.
		auto_format(FALSE, TRUE);

		𐡄𐡍 (restart_edit == 0)
		    restart_edit = restart_edit_save;
		وإلا
		    cap->retval |= CA_COMMAND_BUSY;
	    }
	    اكسر;

	حالة OP_REPLACE:
	    VIsual_reselect = FALSE;	// don't reselect now
	    𐡄𐡍 (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    وإلا
	    {
#ifdef FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#endif
		op_replace(oap, cap->nchar);
	    }
	    اكسر;

#ifdef FEAT_FOLDING
	حالة OP_FOLD:
	    VIsual_reselect = FALSE;	// don't reselect now
	    foldCreate(oap->start.lnum, oap->end.lnum);
	    اكسر;

	حالة OP_FOLDOPEN:
	حالة OP_FOLDOPENREC:
	حالة OP_FOLDCLOSE:
	حالة OP_FOLDCLOSEREC:
	    VIsual_reselect = FALSE;	// don't reselect now
	    opFoldRange(oap->start.lnum, oap->end.lnum,
		    oap->op_type == OP_FOLDOPEN
					    || oap->op_type == OP_FOLDOPENREC,
		    oap->op_type == OP_FOLDOPENREC
					  || oap->op_type == OP_FOLDCLOSEREC,
					  oap->is_VIsual);
	    اكسر;

	حالة OP_FOLDDEL:
	حالة OP_FOLDDELREC:
	    VIsual_reselect = FALSE;	// don't reselect now
	    deleteFold(oap->start.lnum, oap->end.lnum,
			       oap->op_type == OP_FOLDDELREC, oap->is_VIsual);
	    اكسر;
#endif
	حالة OP_NR_ADD:
	حالة OP_NR_SUB:
	    𐡄𐡍 (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    وإلا
	    {
		VIsual_active = TRUE;
#ifdef FEAT_LINEBREAK
		restore_lbr(lbr_saved);
#endif
		op_addsub(oap, cap->count1, redo_VIsual.rv_arg);
		VIsual_active = FALSE;
	    }
	    check_cursor_col();
	    اكسر;
	افتراضي:
	    clearopbeep(oap);
	}
	virtual_op = MAYBE;
	𐡄𐡍 (!gui_yank)
	{
	    // if 'sol' not set, go back to old column for some commands
	    𐡄𐡍 (!p_sol && oap->motion_type == MLINE && !oap->end_adjusted
		    && (oap->op_type == OP_LSHIFT || oap->op_type == OP_RSHIFT
						|| oap->op_type == OP_DELETE))
	    {
#ifdef FEAT_LINEBREAK
		(فراغ)reset_lbr();
#endif
		coladvance(curwin->w_curswant = old_col);
	    }
	}
	وإلا
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
    ساكن فراغ
pbyte(pos_T lp, صحيح c)
{
    char_u *p = ml_get_buf(curbuf, lp.lnum, TRUE);
    صحيح	len = curbuf->b_ml.ml_line_len;

    // safety check
    𐡄𐡍 (lp.col >= len)
	lp.col = (len > 1 ? len - 2 : 0);
    *(p + lp.col) = c;
}
