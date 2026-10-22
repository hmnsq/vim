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

#شامل "vim.h"

ایستا تهی shift_block(oparg_T *oap, صحیح amount);
ایستا تهی	mb_adjust_opend(oparg_T *oap);
ایستا صحیح	do_addsub(صحیح op_type, pos_T *pos, صحیح length, linenr_T Prenum1);
ایستا تهی	pbyte(pos_T lp, صحیح c);
#define PBYTE(lp, c) pbyte(lp, c)


// Flags for third item in "opchars".
#define OPF_سطرها  1	// operator always works on lines
#define OPF_CHANGE 2	// operator changes text

/*
 * The names of operators.
 * IMPORTANT: Index must correspond with defines in vim.h!!!
 * The third field holds OPF_ flags.
 */
ایستا ثابت نویسه opchars[][3] =
{
    {NUL, NUL, 0},			// OP_NOP
    {'d', NUL, OPF_CHANGE},		// OP_DELETE
    {'y', NUL, 0},			// OP_YANK
    {'c', NUL, OPF_CHANGE},		// OP_CHANGE
    {'<', NUL, OPF_سطرها | OPF_CHANGE},	// OP_LSHIFT
    {'>', NUL, OPF_سطرها | OPF_CHANGE},	// OP_RSHIFT
    {'!', NUL, OPF_سطرها | OPF_CHANGE},	// OP_FILTER
    {'g', '~', OPF_CHANGE},		// OP_TILDE
    {'=', NUL, OPF_سطرها | OPF_CHANGE},	// OP_INDENT
    {'g', 'q', OPF_سطرها | OPF_CHANGE},	// OP_FORMAT
    {':', NUL, OPF_سطرها},		// OP_COLON
    {'g', 'U', OPF_CHANGE},		// OP_UPPER
    {'g', 'u', OPF_CHANGE},		// OP_LOWER
    {'J', NUL, OPF_سطرها | OPF_CHANGE},	// DO_JOIN
    {'g', 'J', OPF_سطرها | OPF_CHANGE},	// DO_JOIN_NS
    {'g', '?', OPF_CHANGE},		// OP_ROT13
    {'r', NUL, OPF_CHANGE},		// OP_REPLACE
    {'I', NUL, OPF_CHANGE},		// OP_INSERT
    {'A', NUL, OPF_CHANGE},		// OP_APPEND
    {'z', 'f', OPF_سطرها},		// OP_FOLD
    {'z', 'o', OPF_سطرها},		// OP_FOLDOPEN
    {'z', 'O', OPF_سطرها},		// OP_FOLDOPENREC
    {'z', 'c', OPF_سطرها},		// OP_FOLDCLOSE
    {'z', 'C', OPF_سطرها},		// OP_FOLDCLOSEREC
    {'z', 'd', OPF_سطرها},		// OP_FOLDDEL
    {'z', 'D', OPF_سطرها},		// OP_FOLDDELREC
    {'g', 'w', OPF_سطرها | OPF_CHANGE},	// OP_FORMAT2
    {'g', '@', OPF_CHANGE},		// OP_FUNCTION
    {Ctrl_A, NUL, OPF_CHANGE},		// OP_NR_ADD
    {Ctrl_X, NUL, OPF_CHANGE},		// OP_NR_SUB
};

/*
 * Translate a command name into an operator type.
 * Must only be called with a valid operator name!
 */
    صحیح
get_op_type(صحیح char1, صحیح char2)
{
    صحیح		i;

    اگر (char1 == 'r')		// ignore second character
	بازگشت OP_REPLACE;
    اگر (char1 == '~')		// when tilde is an operator
	بازگشت OP_TILDE;
    اگر (char1 == 'g' && char2 == Ctrl_A)	// add
	بازگشت OP_NR_ADD;
    اگر (char1 == 'g' && char2 == Ctrl_X)	// subtract
	بازگشت OP_NR_SUB;
    اگر (char1 == 'z' && char2 == 'y')	// OP_YANK
	بازگشت OP_YANK;
    برای (i = 0; ; ++i)
    {
	اگر (opchars[i][0] == char1 && opchars[i][1] == char2)
	    بشکن;
	اگر (i == (صحیح)ARRAY_LENGTH(opchars) - 1)
	{
	    internal_error("get_op_type()");
	    بشکن;
	}
    }
    بازگشت i;
}

/*
 * Return TRUE if operator "op" always works on whole lines.
 */
    ایستا صحیح
op_on_سطرها(صحیح op)
{
    بازگشت opchars[op][2] & OPF_سطرها;
}

#اگر defined(FEAT_JOB_CHANNEL)
/*
 * Return TRUE if operator "op" changes text.
 */
    صحیح
op_is_change(صحیح op)
{
    بازگشت opchars[op][2] & OPF_CHANGE;
}
#endif

/*
 * Get first operator command character.
 * Returns 'g' or 'z' if there is another command character.
 */
    صحیح
get_op_char(صحیح optype)
{
    بازگشت opchars[optype][0];
}

/*
 * Get second operator command character.
 */
    صحیح
get_extra_op_char(صحیح optype)
{
    بازگشت opchars[optype][1];
}

/*
 * op_shift - handle a shift operation
 */
    تهی
op_shift(oparg_T *oap, صحیح curs_بالاترین, صحیح amount)
{
    long	    i;
    صحیح		    first_char;
    صحیح		    block_col = 0;

    اگر (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	بازگشت;

    اگر (oap->block_mode)
	block_col = curwin->w_cursor.col;

    برای (i = oap->سطر_شمار; --i >= 0; )
    {
	first_char = *ml_get_curline();
	اگر (first_char == NUL)				// empty line
	    curwin->w_cursor.col = 0;
	وگرنه اگر (oap->block_mode)
	    shift_block(oap, amount);
	وگرنه
	    // Move the line right if it doesn't start with '#', 'smartindent'
	    // isn't set or 'cindent' isn't set or '#' isn't in 'cino'.
	    اگر (first_char != '#' || !preprocs_چپ())
		shift_سطر(oap->op_type == OP_LSHIFT, p_sr, amount, FALSE);
	++curwin->w_cursor.lnum;
    }

    changed_سطرها(oap->start.lnum, 0, oap->end.lnum + 1, 0L);
    اگر (oap->block_mode)
    {
	curwin->w_cursor.lnum = oap->start.lnum;
	curwin->w_cursor.col = block_col;
    }
    وگرنه اگر (curs_بالاترین)	    // put cursor on first line, for ">>"
    {
	curwin->w_cursor.lnum = oap->start.lnum;
	beginline(BL_SOL | BL_FIX);   // shift_line() may have set cursor.col
    }
    وگرنه
	--curwin->w_cursor.lnum;	// put cursor on last line, for ":>"

#ifdef FEAT_FOLDING
    // The cursor line is not in a closed fold
    foldOpenCursor();
#endif


    اگر (oap->سطر_شمار > p_report)
    {
	نویسه	    *op;
	نویسه	    *پیام_سطر_single;
	نویسه	    *پیام_سطر_plural;

	اگر (oap->op_type == OP_RSHIFT)
	    op = ">";
	وگرنه
	    op = "<";
	پیام_سطر_single = NGETTEXT("%ld line %sed %d time",
					     "%ld line %sed %d times", amount);
	پیام_سطر_plural = NGETTEXT("%ld lines %sed %d time",
					    "%ld lines %sed %d times", amount);
	vim_snprintf((نویسه *)IObuff, IOSIZE,
		NGETTEXT(پیام_سطر_single, پیام_سطر_plural, oap->سطر_شمار),
		oap->سطر_شمار, op, amount);
	پیام_attr_keep((نویسه *)IObuff, 0, TRUE);
    }

    اگر ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end.lnum = oap->end.lnum;
	curbuf->b_op_end.col = ml_get_len(oap->end.lnum);
	اگر (curbuf->b_op_end.col > 0)
	    --curbuf->b_op_end.col;
    }
}

#ifdef FEAT_VARTABS
/*
 * Return the tabstop width at the index of the variable tabstop array.  If an
 * index greater than the length of the array is given, the last tabstop width
 * in the array is returned.
 */
    ایستا صحیح
get_vts(صحیح *vts_array, صحیح index)
{
    صحیح	ts;

    اگر (index < 1)
	ts = 0;
    وگرنه اگر (index <= vts_array[0])
	ts = vts_array[index];
    وگرنه
	ts = vts_array[vts_array[0]];

    بازگشت ts;
}

/*
 * Return the sum of all the tabstops through the index-th.
 */
    ایستا صحیح
get_vts_sum(صحیح *vts_array, صحیح index)
{
    صحیح	sum = 0;
    صحیح	i;

    // Perform the summation for indices within the actual array.
    برای (i = 1; i <= index && i <= vts_array[0]; i++)
	sum += vts_array[i];

    // Add tabstops whose indices exceed the actual array.
    اگر (i <= index)
	sum += vts_array[vts_array[0]] * (index - vts_array[0]);

    بازگشت sum;
}
#endif

    ایستا vimlong_T
get_new_sw_indent(
    صحیح		چپ,		// TRUE if shift is to the left
    صحیح		round,		// TRUE if new indent is to be to a tabstop
    vimlong_T	amount,		// Number of shifts
    vimlong_T	sw_val)
{
    vimlong_T	شمار = get_indent();
    vimlong_T	i, j;

    اگر (round)			// round off indent
    {
	i = trim_to_int(شمار) / sw_val;	// number of 'shiftwidth' rounded down
	j = trim_to_int(شمار) % sw_val;	// extra spaces
	اگر (j && چپ)		// first remove extra spaces
	    --amount;
	اگر (چپ)
	{
	    i -= amount;
	    اگر (i < 0)
		i = 0;
	}
	وگرنه
	    i += amount;
	شمار = i * sw_val;
    }
    وگرنه			// original vi indent
    {
	اگر (چپ)
	{
	    شمار -= sw_val * amount;
	    اگر (شمار < 0)
		شمار = 0;
	}
	وگرنه
	    شمار += sw_val * amount;
    }

    بازگشت شمار;
}

#ifdef FEAT_VARTABS
    ایستا vimlong_T
get_new_vts_indent(
    صحیح	چپ,			// TRUE if shift is to the left
    صحیح	round,			// TRUE if new indent is to be to a tabstop
    صحیح	amount,			// Number of shifts
    صحیح	*vts_array)
{
    vimlong_T	indent = get_indent();
    صحیح		vtsi = 0;
    صحیح		vts_indent = 0;
    صحیح		ts = 0;		// Silence uninitialized variable warning.
    صحیح		offset;		// Extra indent spaces to the right of the
				// tabstop

    // Find the tabstop at or to the left of the current indent.
    وقتی (vts_indent <= indent)
    {
	vtsi++;
	ts = get_vts(vts_array, vtsi);
	vts_indent += ts;
    }
    vts_indent -= ts;
    vtsi--;

    offset = indent - vts_indent;

    اگر (round)
    {
	اگر (چپ)
	{
	    اگر (offset == 0)
		indent = get_vts_sum(vts_array, vtsi - amount);
	    وگرنه
		indent = get_vts_sum(vts_array, vtsi - (amount - 1));
	}
	وگرنه
	    indent = get_vts_sum(vts_array, vtsi + amount);
    }
    وگرنه
    {
	اگر (چپ)
	{
	    اگر (amount > vtsi)
		indent = 0;
	    وگرنه
		indent = get_vts_sum(vts_array, vtsi - amount) + offset;
	}
	وگرنه
	    indent = get_vts_sum(vts_array, vtsi + amount) + offset;
    }

    بازگشت indent;
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
    تهی
shift_سطر(
    صحیح	چپ,			// TRUE if shift is to the left
    صحیح	round,			// TRUE if new indent is to be to a tabstop
    صحیح	amount,			// Number of shifts
    صحیح	call_changed_bytes)	// call changed_bytes()
{
    vimlong_T	شمار;
    long	sw_val = curbuf->b_p_sw;
    long	ts_val = curbuf->b_p_ts;
#ifdef FEAT_VARTABS
    صحیح		*vts_array = curbuf->b_p_vts_array;
#endif

    اگر (sw_val != 0)
	// 'shiftwidth' is not zero; use it as the shift size.
	شمار = get_new_sw_indent(چپ, round, amount, sw_val);
    وگرنه
#ifdef FEAT_VARTABS
	اگر ((vts_array == پوچ) || (vts_array[0] == 0))
#endif
    {
	// 'shiftwidth is zero and 'vartabstop' is empty; use 'tabstop' as the
	// shift size.
	شمار = get_new_sw_indent(چپ, round, amount, ts_val);
    }
#ifdef FEAT_VARTABS
    وگرنه
    {
	// 'shiftwidth is zero and 'vartabstop' is defined; use 'vartabstop'
	// to determine the new indent.
	شمار = get_new_vts_indent(چپ, round, amount, vts_array);
    }
#endif

    // Set new indent
    اگر (State & VREPLACE_FLAG)
	change_indent(INDENT_SET, trim_to_int(شمار), FALSE, NUL, call_changed_bytes);
    وگرنه
	(تهی)set_indent(trim_to_int(شمار), call_changed_bytes ? SIN_CHANGED : 0);
}

/*
 * Shift one line of the current block one shiftwidth right or left.
 * Leaves cursor on first character in block.
 */
    ایستا تهی
shift_block(oparg_T *oap, صحیح amount)
{
    صحیح			چپ = (oap->op_type == OP_LSHIFT);
    صحیح			oldstate = State;
    صحیح			total;
    char_u		*newp, *oldp;
    size_t		newlen, oldlen;
    صحیح			oldcol = curwin->w_cursor.col;
    صحیح			sw_val = (صحیح)get_sw_مقدار_indent(curbuf, چپ);
    صحیح			ts_val = (صحیح)curbuf->b_p_ts;
    ساختار block_def	bd;
    صحیح			incr;
    colnr_T		ws_vcol;
    صحیح			added;
    size_t		new_سطر_len;	// the length of the line after the
					// block shift
#ifdef FEAT_RIGHTLEFT
    صحیح			old_p_ri = p_ri;

    p_ri = 0;			// don't want revins in indent
#endif

    State = MODE_درج;	// don't want MODE_REPLACE for State
    block_prep(oap, &bd, curwin->w_cursor.lnum, TRUE);
    اگر (bd.is_short)
	بازگشت;

    // total is number of screen columns to be inserted/removed
    total = (صحیح)((unsigned)amount * (unsigned)sw_val);
    اگر ((total / sw_val) != amount)
	بازگشت; // multiplication overflow

    oldp = ml_get_curline();
    oldlen = ml_get_curline_len();

    اگر (!چپ)
    {
	صحیح		tabs = 0, spaces = 0;
	chartabsize_T	cts;

	/*
	 *  1. Get start vcol
	 *  2. Total ws vcols
	 *  3. Divvy into TABs & spp
	 *  4. Construct new string
	 */
	total += bd.pre_whitesp; // all virtual WS up to & incl a split TAB
	ws_vcol = bd.start_vcol - bd.pre_whitesp;
	اگر (bd.startspaces)
	{
	    اگر (has_mbyte)
	    {
		اگر ((*mb_ptr2len)(bd.textstart) == 1)
		    ++bd.textstart;
		وگرنه
		{
		    ws_vcol = 0;
		    bd.startspaces = 0;
		}
	    }
	    وگرنه
		++bd.textstart;
	}

	// TODO: is passing bd.textstart for start of the line OK?
	init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum,
				   bd.start_vcol, bd.textstart, bd.textstart);
	برای ( ; VIM_ISWHITE(*cts.cts_ptr); )
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
	اگر (!curbuf->b_p_et)
	    tabstop_fromto(ws_vcol, ws_vcol + total,
				ts_val, curbuf->b_p_vts_array, &tabs, &spaces);
	وگرنه
	    spaces = total;
#وگرنه
	اگر (!curbuf->b_p_et)
	    tabs = ((ws_vcol % ts_val) + total) / ts_val; // number of tabs
	اگر (tabs > 0)
	    spaces = ((ws_vcol % ts_val) + total) % ts_val; // number of spp
	وگرنه
	    spaces = total;
#endif
	// if we're splitting a TAB, allow for it
	bd.textcol -= bd.pre_whitesp_c - (bd.startspaces != 0);

	new_سطر_len = bd.textcol + tabs + spaces + (oldlen - (bd.textstart - oldp));
	newp = alloc(new_سطر_len + 1);
	اگر (newp == پوچ)
	    بازگشت;
	mch_memmove(newp, oldp, (size_t)bd.textcol);
	newlen = bd.textcol;
	vim_memset(newp + newlen, TAB, (size_t)tabs);
	newlen += tabs;
	vim_memset(newp + newlen, ' ', (size_t)spaces);
	STRCPY(newp + newlen + spaces, bd.textstart);
    }
    وگرنه //چپ
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
	اگر (bd.startspaces)
	    MB_PTR_ADV(non_white);

	// The character's column is in "bd.start_vcol".
	non_white_col = bd.start_vcol;

	init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum,
				   non_white_col, bd.textstart, non_white);
	وقتی (VIM_ISWHITE(*cts.cts_ptr))
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
	اگر (bd.startspaces)
	    verbatim_copy_width -= bd.start_char_vcols;
	init_chartabsize_arg(&cts, curwin, 0, verbatim_copy_width,
					     bd.textstart, verbatim_copy_end);
	وقتی (cts.cts_vcol < destination_col)
	{
	    incr = lbr_chartabsize(&cts);
	    اگر (cts.cts_vcol + incr > destination_col)
		بشکن;
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
	new_سطر_len = fixedlen + fill + (oldlen - (non_white - oldp));

	newp = alloc(new_سطر_len + 1);
	اگر (newp == پوچ)
	    بازگشت;
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
    اگر (new_سطر_len >= oldlen)
	added = (صحیح)(new_سطر_len - oldlen);
    وگرنه
	added = 0 - (صحیح)(oldlen - new_سطر_len);
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
    ایستا تهی
block_درج(
    oparg_T		*oap,
    char_u		*s,
    size_t		slen,
    صحیح			b_درج,
    ساختار block_def	*bdp)
{
    صحیح		ts_val;
    صحیح		شمار = 0;	// extra spaces to replace a cut TAB
    صحیح		spaces = 0;	// non-zero if cutting a TAB
    colnr_T	offset;		// pointer along new line
    colnr_T	startcol;	// column where insert starts
    char_u	*newp, *oldp;	// new, old lines
    linenr_T	lnum;		// loop var
    صحیح		oldstate = State;

    State = MODE_درج;	// don't want MODE_REPLACE for State

    برای (lnum = oap->start.lnum + 1; lnum <= oap->end.lnum; lnum++)
    {
	block_prep(oap, bdp, lnum, TRUE);
	اگر (bdp->is_short && b_درج)
	    ادامه;	// OP_INSERT, line ends before block start

	oldp = ml_get(lnum);

	اگر (b_درج)
	{
	    ts_val = bdp->start_char_vcols;
	    spaces = bdp->startspaces;
	    اگر (spaces != 0)
		شمار = ts_val - 1; // we're cutting a TAB
	    offset = bdp->textcol;
	}
	وگرنه // append
	{
	    ts_val = bdp->end_char_vcols;
	    اگر (!bdp->is_short) // spaces = padding after block
	    {
		spaces = (bdp->endspaces ? ts_val - bdp->endspaces : 0);
		اگر (spaces != 0)
		    شمار = ts_val - 1; // we're cutting a TAB
		offset = bdp->textcol + bdp->textlen - (spaces != 0);
	    }
	    وگرنه // spaces = padding to block edge
	    {
		// if $ used, just append to EOL (ie spaces==0)
		اگر (!bdp->is_MAX)
		    spaces = (oap->end_vcol - bdp->end_vcol) + 1;
		شمار = spaces;
		offset = bdp->textcol + bdp->textlen;
	    }
	}

	اگر (has_mbyte && spaces > 0)
	    // avoid copying part of a multi-byte character
	    offset -= (*mb_head_off)(oldp, oldp + offset);

	اگر (spaces < 0)  // can happen when the cursor was moved
	    spaces = 0;

	// Make sure the allocated size matches what is actually copied below.
	newp = alloc(ml_get_len(lnum) + spaces + slen
		    + (spaces > 0 && !bdp->is_short ? ts_val - spaces : 0)
								  + شمار + 1);
	اگر (newp == پوچ)
	    ادامه;

	// copy up to shifted part
	mch_memmove(newp, oldp, (size_t)offset);
	oldp += offset;

	// insert pre-padding
	vim_memset(newp + offset, ' ', (size_t)spaces);
	startcol = offset + spaces;

	// copy the new text
	mch_memmove(newp + startcol, s, slen);
	offset += (صحیح)slen;

	اگر (spaces > 0 && !bdp->is_short)
	{
	    اگر (*oldp == TAB)
	    {
		// insert post-padding
		vim_memset(newp + offset + spaces, ' ',
						    (size_t)(ts_val - spaces));
		// we're splitting a TAB, don't copy it
		oldp++;
		// We allowed for that TAB, remember this now
		شمار++;
	    }
	    وگرنه
		// Not a TAB, no extra spaces
		شمار = spaces;
	}

	اگر (spaces > 0)
	    offset += شمار;
	STRCPY(newp + offset, oldp);

	ml_replace(lnum, newp, FALSE);

	اگر (b_درج)
	    // correct any text properties
	    inserted_bytes(lnum, startcol, (صحیح)slen);

	اگر (lnum == oap->end.lnum)
	{
	    // Set "']" mark to the end of the block instead of the end of
	    // the insert in the first line.
	    curbuf->b_op_end.lnum = oap->end.lnum;
	    curbuf->b_op_end.col = offset;
	    اگر (curbuf->b_visual.vi_end.coladd)
	    {
		curbuf->b_visual.vi_end.col += curbuf->b_visual.vi_end.coladd;
		curbuf->b_visual.vi_end.coladd = 0;
	    }
	}
    } // for all lnum

    changed_سطرها(oap->start.lnum + 1, 0, oap->end.lnum + 1, 0L);

    State = oldstate;
}

/*
 * Handle a delete operation.
 *
 * Return FAIL if undo failed, OK otherwise.
 */
    صحیح
op_delete(oparg_T *oap)
{
    صحیح			n;
    linenr_T		lnum;
    char_u		*ptr;
    char_u		*newp, *oldp;
    ساختار block_def	bd;
    linenr_T		old_lcount = curbuf->b_ml.ml_سطر_شمار;
    صحیح			did_yank = FALSE;

    اگر (curbuf->b_ml.ml_flags & ML_EMPTY)	    // nothing to do
	بازگشت OK;

    // Nothing to delete, return here.	Do prepare undo, for op_change().
    اگر (oap->empty)
	بازگشت u_save_cursor();

    اگر (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	بازگشت FAIL;
    }

    اگر (VIsual_select && oap->is_VIsual)
	// use register given with CTRL_R, defaults to zero
	oap->regname = VIsual_select_reg;

#ifdef HAVE_CLIPMETHOD
    adjust_clip_reg(&oap->regname);
#endif

    اگر (has_mbyte)
	mb_adjust_opend(oap);

    /*
     * Imitate the strange Vi behaviour: If the delete spans more than one
     * line and motion_type == MCHAR and the result is a blank line, make the
     * delete linewise.  Don't do this for the change command or Visual mode.
     */
    اگر (       oap->motion_type == MCHAR
	    && !oap->is_VIsual
	    && !oap->block_mode
	    && oap->سطر_شمار > 1
	    && oap->motion_force == NUL
	    && (vim_strchr(p_cpo, CPO_WORD) != پوچ)
	    && oap->op_type == OP_DELETE)
    {
	ptr = ml_get(oap->end.lnum) + oap->end.col;
	اگر (*ptr != NUL)
	    ptr += oap->inclusive;
	ptr = skipwhite(ptr);
	اگر (*ptr == NUL && inindent(0))
	    oap->motion_type = MLINE;
    }

    /*
     * Check for trying to delete (e.g. "D") in an empty line.
     * Note: For the change operator it is ok.
     */
    اگر (       oap->motion_type == MCHAR
	    && oap->سطر_شمار == 1
	    && oap->op_type == OP_DELETE
	    && *ml_get(oap->start.lnum) == NUL)
    {
	/*
	 * It's an error to operate on an empty region, when 'E' included in
	 * 'cpoptions' (Vi compatible).
	 */
	اگر (virtual_op)
	    // Virtual editing: Nothing gets deleted, but we set the '[ and ']
	    // marks as if it happened.
	    برو setmarks;
	اگر (vim_strchr(p_cpo, CPO_EMPTYREGION) != پوچ)
	    beep_flush();
	بازگشت OK;
    }

    /*
     * Do a yank of whatever we're about to delete.
     * If a yank register was specified, put the deleted text into that
     * register.  For the black hole register '_' don't yank anything.
     */
    اگر (oap->regname != '_')
    {
#ifdef FEAT_CLIPBOARD_PROVIDER
	inc_clip_provider();
#endif
	اگر (oap->regname != 0)
	{
	    // check for read-only register
	    اگر (!valid_yank_reg(oap->regname, TRUE))
	    {
		beep_flush();
#ifdef FEAT_CLIPBOARD_PROVIDER
		dec_clip_provider();
#endif
		بازگشت OK;
	    }
	    get_yank_register(oap->regname, TRUE); // yank into specif'd reg.
	    اگر (op_yank(oap, TRUE, FALSE) == OK)   // yank without message
		did_yank = TRUE;
	}
	وگرنه
	    reset_y_append(); // not appending to unnamed register

	/*
	 * Put deleted text into register 1 and shift number registers if the
	 * delete contains a line break, or when using a specific operator (Vi
	 * compatible)
	 */
	اگر (oap->motion_type == MLINE || oap->سطر_شمار > 1
							   || oap->use_reg_one)
	{
	    shift_delete_registers();
	    اگر (op_yank(oap, TRUE, FALSE) == OK)
		did_yank = TRUE;
	}

	// Yank into small delete register when no named register specified
	// and the delete is within one line.
	اگر ((
#ifdef HAVE_CLIPMETHOD
	    ((clip_unnamed & CLIP_UNNAMED) && oap->regname == '*') ||
	    ((clip_unnamed & CLIP_UNNAMED_PLUS) && oap->regname == '+') ||
#endif
	    oap->regname == 0) && oap->motion_type != MLINE
						      && oap->سطر_شمار == 1)
	{
	    oap->regname = '-';
	    get_yank_register(oap->regname, TRUE);
	    اگر (op_yank(oap, TRUE, FALSE) == OK)
		did_yank = TRUE;
	    oap->regname = 0;
	}

	/*
	 * If there's too much stuff to fit in the yank register, then get a
	 * confirmation before doing the delete. This is crude, but simple.
	 * And it avoids doing a delete of something we can't put back if we
	 * want.
	 */
	اگر (!did_yank)
	{
	    صحیح پیام_silent_save = پیام_silent;

	    پیام_silent = 0;	// must display the prompt
	    n = ask_yesno((char_u *)_("cannot yank; delete anyway"), TRUE);
	    پیام_silent = پیام_silent_save;
	    اگر (n != 'y')
	    {
		emsg(_(e_command_aborted));
#ifdef FEAT_CLIPBOARD_PROVIDER
		dec_clip_provider();
#endif
		بازگشت FAIL;
	    }
	}

#اگر defined(FEAT_EVAL)
	اگر (did_yank && has_textyankpost())
	    yank_do_autocmd(oap, get_y_current());
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
    }

    /*
     * block mode delete
     */
    اگر (oap->block_mode)
    {
	اگر (u_save((linenr_T)(oap->start.lnum - 1),
			       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	    بازگشت FAIL;

	برای (lnum = curwin->w_cursor.lnum; lnum <= oap->end.lnum; ++lnum)
	{
	    block_prep(oap, &bd, lnum, TRUE);
	    اگر (bd.textlen == 0)	// nothing to delete
		ادامه;

	    // Adjust cursor position for tab replaced by spaces and 'lbr'.
	    اگر (lnum == curwin->w_cursor.lnum)
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
	    اگر (newp == پوچ)
		ادامه;
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
	    اگر (curbuf->b_has_textprop && n != 0)
		adjust_prop_columns(lnum, bd.textcol, -n, 0);
#endif
	}

	check_cursor_col();
	changed_سطرها(curwin->w_cursor.lnum, curwin->w_cursor.col,
						       oap->end.lnum + 1, 0L);
	oap->سطر_شمار = 0;	    // no lines deleted
    }
    وگرنه اگر (oap->motion_type == MLINE)
    {
	اگر (oap->op_type == OP_CHANGE)
	{
	    // Delete the lines except the first one.  Temporarily move the
	    // cursor to the next line.  Save the current line number, if the
	    // last line is deleted it may be changed.
	    اگر (oap->سطر_شمار > 1)
	    {
		lnum = curwin->w_cursor.lnum;
		++curwin->w_cursor.lnum;
		del_سطرها((long)(oap->سطر_شمار - 1), TRUE);
		curwin->w_cursor.lnum = lnum;
	    }
	    اگر (u_save_cursor() == FAIL)
		بازگشت FAIL;
	    اگر (curbuf->b_p_ai)		    // don't delete indent
	    {
		beginline(BL_WHITE);	    // cursor on first non-white
		did_ai = TRUE;		    // delete the indent when ESC hit
		ai_col = curwin->w_cursor.col;
	    }
	    وگرنه
		beginline(0);		    // cursor in column 0
	    truncate_سطر(FALSE);   // delete the rest of the line,
				    // leaving cursor past last char in line
	    اگر (oap->سطر_شمار > 1)
		u_clearline();	    // "U" command not possible after "2cc"
	}
	وگرنه
	{
	    del_سطرها(oap->سطر_شمار, TRUE);
	    beginline(BL_WHITE | BL_FIX);
	    u_clearline();	// "U" command not possible after "dd"
	}
    }
    وگرنه
    {
	اگر (virtual_op)
	{
	    صحیح		endcol = 0;

	    // For virtualedit: break the tabs that are partly included.
	    اگر (gchar_pos(&oap->start) == '\t')
	    {
		اگر (u_save_cursor() == FAIL)	// save first line for undo
		    بازگشت FAIL;
		اگر (oap->سطر_شمار == 1)
		    endcol = getviscol2(oap->end.col, oap->end.coladd);
		coladvance_force(getviscol2(oap->start.col, oap->start.coladd));
		oap->start = curwin->w_cursor;
		اگر (oap->سطر_شمار == 1)
		{
		    coladvance(endcol);
		    oap->end.col = curwin->w_cursor.col;
		    oap->end.coladd = curwin->w_cursor.coladd;
		    curwin->w_cursor = oap->start;
		}
	    }

	    // Break a tab only when it's included in the area.
	    اگر (gchar_pos(&oap->end) == '\t'
				     && (صحیح)oap->end.coladd < oap->inclusive)
	    {
		// save last line for undo
		اگر (u_save((linenr_T)(oap->end.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
		    بازگشت FAIL;
		curwin->w_cursor = oap->end;
		coladvance_force(getviscol2(oap->end.col, oap->end.coladd));
		oap->end = curwin->w_cursor;
		curwin->w_cursor = oap->start;
	    }
	    اگر (has_mbyte)
		mb_adjust_opend(oap);
	}

	اگر (oap->سطر_شمار == 1)	// delete characters within one line
	{
	    اگر (u_save_cursor() == FAIL)	// save line for undo
		بازگشت FAIL;

	    // if 'cpoptions' contains '$', display '$' at end of change
	    اگر (       vim_strchr(p_cpo, CPO_DOLLAR) != پوچ
		    && oap->op_type == OP_CHANGE
		    && oap->end.lnum == curwin->w_cursor.lnum
		    && !oap->is_VIsual)
		display_dollar(oap->end.col - !oap->inclusive);

	    n = oap->end.col - oap->start.col + 1 - !oap->inclusive;

	    اگر (virtual_op)
	    {
		// fix up things for virtualedit-delete:
		// break the tabs which are going to get in our way
		صحیح		len = ml_get_curline_len();

		اگر (oap->end.coladd != 0
			&& (صحیح)oap->end.col >= len - 1
			&& !(oap->start.coladd && (صحیح)oap->end.col >= len - 1))
		    n++;
		// Delete at least one char (e.g, when on a control char).
		اگر (n == 0 && oap->start.coladd != oap->end.coladd)
		    n = 1;

		// When deleted a char in the line, reset coladd.
		اگر (gchar_cursor() != NUL)
		    curwin->w_cursor.coladd = 0;
	    }
	    (تهی)del_bytes((long)n, !virtual_op,
			    oap->op_type == OP_DELETE && !oap->is_VIsual);
	}
	وگرنه				// delete characters between lines
	{
	    pos_T   curpos;

	    // save deleted and changed lines for undo
	    اگر (u_save((linenr_T)(curwin->w_cursor.lnum - 1),
		 (linenr_T)(curwin->w_cursor.lnum + oap->سطر_شمار)) == FAIL)
		بازگشت FAIL;

	    truncate_سطر(TRUE);	// delete from cursor to end of line

	    curpos = curwin->w_cursor;	// remember curwin->w_cursor
	    ++curwin->w_cursor.lnum;
	    del_سطرها((long)(oap->سطر_شمار - 2), FALSE);

	    // delete from start of line until op_end
	    n = (oap->end.col + 1 - !oap->inclusive);
	    curwin->w_cursor.col = 0;
	    (تهی)del_bytes((long)n, !virtual_op,
			    oap->op_type == OP_DELETE && !oap->is_VIsual);
	    curwin->w_cursor = curpos;	// restore curwin->w_cursor
	    (تهی)do_join(2, FALSE, FALSE, FALSE, FALSE);
	}
	اگر (oap->op_type == OP_DELETE)
	    auto_format(FALSE, TRUE);
    }

    msgmore(curbuf->b_ml.ml_سطر_شمار - old_lcount);

setmarks:
    اگر ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	اگر (oap->block_mode)
	{
	    curbuf->b_op_end.lnum = oap->end.lnum;
	    curbuf->b_op_end.col = oap->start.col;
	}
	وگرنه
	    curbuf->b_op_end = oap->start;
	curbuf->b_op_start = oap->start;
    }

    بازگشت OK;
}

/*
 * Adjust end of operating area for ending on a multi-byte character.
 * Used for deletion.
 */
    ایستا تهی
mb_adjust_opend(oparg_T *oap)
{
    char_u	*سطر;
    char_u	*ptr;

    اگر (!oap->inclusive)
	بازگشت;

    سطر = ml_get(oap->end.lnum);
    ptr = سطر + oap->end.col;
    اگر (*ptr != NUL)
    {
	ptr -= (*mb_head_off)(سطر, ptr);
	ptr += (*mb_ptr2len)(ptr) - 1;
	oap->end.col = ptr - سطر;
    }
}

/*
 * Replace the character under the cursor with "c".
 * This takes care of multi-byte characters.
 */
    ایستا تهی
replace_character(صحیح c)
{
    صحیح n = State;

    State = MODE_REPLACE;
    ins_char(c);
    State = n;
    // Backup to the replaced character.
    dec_cursor();
}

/*
 * Replace a whole area with one character.
 */
    صحیح
op_replace(oparg_T *oap, صحیح c)
{
    صحیح			n, numc;
    صحیح			رقم_chars;
    char_u		*newp, *oldp;
    size_t		newlen, oldlen;
    ساختار block_def	bd;
    char_u		*after_p = پوچ;
    صحیح			had_ctrl_v_cr = FALSE;

    اگر ((curbuf->b_ml.ml_flags & ML_EMPTY ) || oap->empty)
	بازگشت OK;	    // nothing to do

    اگر (c == REPLACE_CR_NCHAR)
    {
	had_ctrl_v_cr = TRUE;
	c = CAR;
    }
    وگرنه اگر (c == REPLACE_NL_NCHAR)
    {
	had_ctrl_v_cr = TRUE;
	c = NL;
    }

    اگر (has_mbyte)
	mb_adjust_opend(oap);

    اگر (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	بازگشت FAIL;

    /*
     * block mode replace
     */
    اگر (oap->block_mode)
    {
	bd.is_MAX = (curwin->w_curswant == MAXCOL);
	برای ( ; curwin->w_cursor.lnum <= oap->end.lnum; ++curwin->w_cursor.lnum)
	{
	    curwin->w_cursor.col = 0;  // make sure cursor position is valid
	    block_prep(oap, &bd, curwin->w_cursor.lnum, TRUE);
	    اگر (bd.textlen == 0 && (!virtual_op || bd.is_MAX))
		ادامه;	    // nothing to replace

	    // n == number of extra chars required
	    // If we split a TAB, it may be replaced by several characters.
	    // Thus the number of characters may increase!
	    // If the range starts in virtual space, count the initial
	    // coladd offset as part of "startspaces"
	    اگر (virtual_op && bd.is_short && *bd.textstart == NUL)
	    {
		pos_T vpos;

		vpos.lnum = curwin->w_cursor.lnum;
		getvpos(&vpos, oap->start_vcol);
		bd.startspaces += vpos.coladd;
		n = bd.startspaces;
	    }
	    وگرنه
		// allow for pre spaces
		n = (bd.startspaces ? bd.start_char_vcols - 1 : 0);

	    // allow for post spp
	    n += (bd.endspaces
		    && !bd.is_oneChar
		    && bd.end_char_vcols > 0) ? bd.end_char_vcols - 1 : 0;
	    // Figure out how many characters to replace.
	    numc = oap->end_vcol - oap->start_vcol + 1;
	    اگر (bd.is_short && (!virtual_op || bd.is_MAX))
		numc -= (oap->end_vcol - bd.end_vcol) + 1;

	    // A double-wide character can be replaced only up to half the
	    // times.
	    اگر ((*mb_char2cells)(c) > 1)
	    {
		اگر ((numc & 1) && !bd.is_short)
		{
		    ++bd.endspaces;
		    ++n;
		}
		numc = numc / 2;
	    }

	    // Compute bytes needed, move character count to num_chars.
	    رقم_chars = numc;
	    numc *= (*mb_char2len)(c);
	    // oldlen includes textlen, so don't double count
	    n += numc - bd.textlen;

	    oldp = ml_get_curline();
	    oldlen = ml_get_curline_len();
	    newp = alloc(oldlen + 1 + n);
	    اگر (newp == پوچ)
		ادامه;
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
	    اگر (had_ctrl_v_cr || (c != '\r' && c != '\n'))
	    {
		اگر (has_mbyte)
		{
		    وقتی (--رقم_chars >= 0)
			newlen += (*mb_char2bytes)(c, newp + newlen);
		}
		وگرنه
		{
		    vim_memset(newp + newlen, c, (size_t)numc);
		    newlen += numc;
		}
		اگر (!bd.is_short)
		{
		    // insert post-spaces
		    vim_memset(newp + newlen, ' ', (size_t)bd.endspaces);
		    // copy the part after the changed part
		    STRCPY(newp + newlen + bd.endspaces,
				oldp + bd.textcol + bd.textlen);
		}
	    }
	    وگرنه
	    {
		// Replacing with \r or \n means splitting the line.
		after_p = alloc(oldlen + 1 + n - newlen);
		اگر (after_p != پوچ)
		    STRCPY(after_p, oldp + bd.textcol + bd.textlen);
	    }

	    // replace the line
	    ml_replace(curwin->w_cursor.lnum, newp, FALSE);
	    اگر (after_p != پوچ)
	    {
		ml_append(curwin->w_cursor.lnum++, after_p, 0, FALSE);
		appended_سطرها_mark(curwin->w_cursor.lnum, 1L);
		oap->end.lnum++;
		vim_free(after_p);
	    }
	}
    }
    وگرنه
    {
	/*
	 * MCHAR and MLINE motion replace.
	 */
	اگر (oap->motion_type == MLINE)
	{
	    oap->start.col = 0;
	    curwin->w_cursor.col = 0;
	    oap->end.col = ml_get_len(oap->end.lnum);
	    اگر (oap->end.col)
		--oap->end.col;
	}
	وگرنه اگر (!oap->inclusive)
	    dec(&(oap->end));

	وقتی (LTOREQ_POS(curwin->w_cursor, oap->end))
	{
	    صحیح done = FALSE;

	    n = gchar_cursor();
	    اگر (n != NUL)
	    {
		صحیح new_byte_len = (*mb_char2len)(c);
		صحیح old_byte_len = mb_ptr2len(ml_get_cursor());

		اگر (new_byte_len > 1 || old_byte_len > 1)
		{
		    // This is slow, but it handles replacing a single-byte
		    // with a multi-byte and the other way around.
		    اگر (curwin->w_cursor.lnum == oap->end.lnum)
			oap->end.col += new_byte_len - old_byte_len;
		    replace_character(c);
		    done = TRUE;
		}
		وگرنه
		{
		    اگر (n == TAB)
		    {
			صحیح end_vcol = 0;

			اگر (curwin->w_cursor.lnum == oap->end.lnum)
			{
			    // oap->end has to be recalculated when
			    // the tab breaks
			    end_vcol = getviscol2(oap->end.col,
							     oap->end.coladd);
			}
			coladvance_force(getviscol());
			اگر (curwin->w_cursor.lnum == oap->end.lnum)
			    getvpos(&oap->end, end_vcol);
		    }
		    // with "coladd" set may move to just after a TAB
		    اگر (gchar_cursor() != NUL)
		    {
			PBYTE(curwin->w_cursor, c);
			done = TRUE;
		    }
		}
	    }
	    اگر (!done && virtual_op && curwin->w_cursor.lnum == oap->end.lnum)
	    {
		صحیح virtcols = oap->end.coladd;

		اگر (curwin->w_cursor.lnum == oap->start.lnum
			&& oap->start.col == oap->end.col && oap->start.coladd)
		    virtcols -= oap->start.coladd;

		// oap->end has been trimmed so it's effectively inclusive;
		// as a result an extra +1 must be counted so we don't
		// trample the NUL byte.
		coladvance_force(getviscol2(oap->end.col, oap->end.coladd) + 1);
		curwin->w_cursor.col -= (virtcols + 1);
		برای (; virtcols >= 0; virtcols--)
		{
		    اگر ((*mb_char2len)(c) > 1)
		       replace_character(c);
		    وگرنه
			PBYTE(curwin->w_cursor, c);
		    اگر (inc(&curwin->w_cursor) == -1)
			بشکن;
		}
	    }

	    // Advance to next character, stop at the end of the file.
	    اگر (inc_cursor() == -1)
		بشکن;
	}
    }

    curwin->w_cursor = oap->start;
    check_cursor();
    changed_سطرها(oap->start.lnum, oap->start.col, oap->end.lnum + 1, 0L);

    اگر ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
    }

    بازگشت OK;
}

ایستا صحیح swapchars(صحیح op_type, pos_T *pos, صحیح length);

/*
 * Handle the (non-standard vi) tilde operator.  Also for "gu", "gU" and "g?".
 */
    ایستا تهی
op_tilde(oparg_T *oap)
{
    pos_T		pos;
    ساختار block_def	bd;
    صحیح			did_change = FALSE;

    اگر (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	بازگشت;

    pos = oap->start;
    اگر (oap->block_mode)		    // Visual block mode
    {
	برای (; pos.lnum <= oap->end.lnum; ++pos.lnum)
	{
	    صحیح one_change;

	    block_prep(oap, &bd, pos.lnum, FALSE);
	    pos.col = bd.textcol;
	    one_change = swapchars(oap->op_type, &pos, bd.textlen);
	    did_change |= one_change;

#ifdef FEAT_NETBEANS_INTG
	    اگر (netbeans_active() && one_change)
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
	اگر (did_change)
	    changed_سطرها(oap->start.lnum, 0, oap->end.lnum + 1, 0L);
    }
    وگرنه				    // not block mode
    {
	اگر (oap->motion_type == MLINE)
	{
	    oap->start.col = 0;
	    pos.col = 0;
	    oap->end.col = ml_get_len(oap->end.lnum);
	    اگر (oap->end.col)
		--oap->end.col;
	}
	وگرنه اگر (!oap->inclusive)
	    dec(&(oap->end));

	اگر (pos.lnum == oap->end.lnum)
	    did_change = swapchars(oap->op_type, &pos,
						  oap->end.col - pos.col + 1);
	وگرنه
	    برای (;;)
	    {
		did_change |= swapchars(oap->op_type, &pos,
			pos.lnum == oap->end.lnum ? oap->end.col + 1
						  : ml_get_pos_len(&pos));
		اگر (LTOREQ_POS(oap->end, pos) || inc(&pos) == -1)
		    بشکن;
	    }
	اگر (did_change)
	{
	    changed_سطرها(oap->start.lnum, oap->start.col, oap->end.lnum + 1,
									  0L);
#ifdef FEAT_NETBEANS_INTG
	    اگر (netbeans_active())
	    {
		char_u *ptr;
		صحیح شمار;

		pos = oap->start;
		وقتی (pos.lnum < oap->end.lnum)
		{
		    ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		    شمار = ml_get_buf_len(curbuf, pos.lnum) - pos.col;
		    netbeans_removed(curbuf, pos.lnum, pos.col, (long)شمار);
		    // get the line again, it may have been flushed
		    ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		    netbeans_inserted(curbuf, pos.lnum, pos.col,
							&ptr[pos.col], شمار);
		    pos.col = 0;
		    pos.lnum++;
		}
		شمار = oap->end.col - pos.col + 1;
		netbeans_removed(curbuf, pos.lnum, pos.col, (long)شمار);
		// get the line again, it may have been flushed
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, pos.col,
							&ptr[pos.col], شمار);
	    }
#endif
	}
    }

    اگر (!did_change && oap->is_VIsual)
	// No change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    اگر ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set '[ and '] marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
    }

    اگر (oap->سطر_شمار > p_report)
	smsg(NGETTEXT("%ld line changed", "%ld lines changed",
					    oap->سطر_شمار), oap->سطر_شمار);
}

/*
 * Invoke swapchar() on "length" bytes at position "pos".
 * "pos" is advanced to just after the changed characters.
 * "length" is rounded up to include the whole last multi-byte character.
 * Also works correctly when the number of bytes changes.
 * Returns TRUE if some character was changed.
 */
    ایستا صحیح
swapchars(صحیح op_type, pos_T *pos, صحیح length)
{
    صحیح todo;
    صحیح	did_change = 0;

    برای (todo = length; todo > 0; --todo)
    {
	اگر (has_mbyte)
	{
	    صحیح len = (*mb_ptr2len)(ml_get_pos(pos));

	    // we're counting bytes, not characters
	    اگر (len > 0)
		todo -= len - 1;
	}
	did_change |= swapchar(op_type, pos);
	اگر (inc(pos) == -1)    // at end of file
	    بشکن;
    }
    بازگشت did_change;
}

/*
 * If op_type == OP_UPPER: make uppercase,
 * if op_type == OP_LOWER: make lowercase,
 * if op_type == OP_ROT13: do rot13 encoding,
 * else swap case of character at 'pos'
 * returns TRUE when something actually changed.
 */
    صحیح
swapchar(صحیح op_type, pos_T *pos)
{
    صحیح	    c;
    صحیح	    nc;

    c = gchar_pos(pos);

    // Only do rot13 encoding for ASCII characters.
    اگر (c >= 0x80 && op_type == OP_ROT13)
	بازگشت FALSE;

    // ~ is OP_NOP, g~ is OP_TILDE, gU is OP_UPPER
    اگر ((op_type == OP_UPPER || op_type == OP_NOP || op_type == OP_TILDE)
	    && c == 0xdf
	    && (enc_latin1like || STRCMP(p_enc, "iso-8859-2") == 0))
    {
	pos_T   sp = curwin->w_cursor;

	// Special handling for lowercase German sharp s (ß): convert to uppercase (ẞ).
	curwin->w_cursor = *pos;
	del_char(FALSE);
	ins_char(0x1E9E);
	curwin->w_cursor = sp;
	بازگشت TRUE;
    }

    اگر (enc_dbcs != 0 && c >= 0x100)	// No lower/uppercase letter
	بازگشت FALSE;
    nc = c;
    اگر (MB_ISLOWER(c))
    {
	اگر (op_type == OP_ROT13)
	    nc = ROT13(c, 'a');
	وگرنه اگر (op_type != OP_LOWER)
	    nc = MB_TOUPPER(c);
    }
    وگرنه اگر (MB_ISUPPER(c))
    {
	اگر (op_type == OP_ROT13)
	    nc = ROT13(c, 'A');
	وگرنه اگر (op_type != OP_UPPER)
	    nc = MB_TOLOWER(c);
    }
    اگر (nc != c)
    {
	اگر (enc_utf8 && (c >= 0x80 || nc >= 0x80))
	{
	    pos_T   sp = curwin->w_cursor;

	    curwin->w_cursor = *pos;
	    // don't use del_char(), it also removes composing chars
	    del_bytes(utf_ptr2len(ml_get_cursor()), FALSE, FALSE);
	    ins_char(nc);
	    curwin->w_cursor = sp;
	}
	وگرنه
	    PBYTE(*pos, nc);
	بازگشت TRUE;
    }
    بازگشت FALSE;
}

/*
 * op_insert - Insert and append operators for Visual mode.
 */
    تهی
op_درج(oparg_T *oap, long count1)
{
    long		pre_textlen = 0;
    colnr_T		ind_pre_col = 0, ind_post_col;
    صحیح			ind_pre_vcol = 0, ind_post_vcol = 0;
    ساختار block_def	bd;
    صحیح			i;
    pos_T		t1;
    pos_T		start_درج;

    // edit() changes this - record it for OP_APPEND
    bd.is_MAX = (curwin->w_curswant == MAXCOL);

    // vis block is still marked. Get rid of it now.
    curwin->w_cursor.lnum = oap->start.lnum;
    update_screen(UPD_INVERTED);

    اگر (oap->block_mode)
    {
	// When 'virtualedit' is used, need to insert the extra spaces before
	// doing block_prep().  When only "block" is used, virtual edit is
	// already disabled, but still need it when calling
	// coladvance_force().
	// coladvance_force() uses get_ve_flags() to get the 'virtualedit'
	// state for the current window.  To override that state, we need to
	// set the window-local value of ve_flags rather than the global value.
	اگر (curwin->w_cursor.coladd > 0)
	{
	    صحیح		old_ve_flags = curwin->w_ve_flags;

	    اگر (u_save_cursor() == FAIL)
		بازگشت;

	    curwin->w_ve_flags = VE_ALL;
	    coladvance_force(oap->op_type == OP_APPEND
					   ? oap->end_vcol + 1 : getviscol());
	    اگر (oap->op_type == OP_APPEND)
		--curwin->w_cursor.col;
	    curwin->w_ve_flags = old_ve_flags;
	}
	// Get the info about the block before entering the text
	block_prep(oap, &bd, oap->start.lnum, TRUE);
	// Get indent information
	ind_pre_col = (colnr_T)getwhitecols_curline();
	ind_pre_vcol = get_indent();
	pre_textlen = ml_get_len(oap->start.lnum) - bd.textcol;
	اگر (oap->op_type == OP_APPEND)
	    pre_textlen -= bd.textlen;
    }

    اگر (oap->op_type == OP_APPEND)
    {
	اگر (oap->block_mode && curwin->w_cursor.coladd == 0)
	{
	    // Move the cursor to the character right of the block.
	    curwin->w_set_curswant = true;
	    وقتی (*ml_get_cursor() != NUL
		    && (curwin->w_cursor.col < bd.textcol + bd.textlen))
		++curwin->w_cursor.col;
	    اگر (bd.is_short && !bd.is_MAX)
	    {
		// First line was too short, make it longer and adjust the
		// values in "bd".
		اگر (u_save_cursor() == FAIL)
		    بازگشت;
		برای (i = 0; i < bd.endspaces; ++i)
		    ins_char(' ');
		bd.textlen += bd.endspaces;
	    }
	}
	وگرنه
	{
	    curwin->w_cursor = oap->end;
	    check_cursor_col();

	    // Works just like an 'i'nsert on the next character.
	    اگر (!LINEEMPTY(curwin->w_cursor.lnum)
		    && oap->start_vcol != oap->end_vcol)
		inc_cursor();
	}
    }

    t1 = oap->start;
    start_درج = curwin->w_cursor;
    (تهی)edit(NUL, FALSE, (linenr_T)count1);

    // When a tab was inserted, and the characters in front of the tab
    // have been converted to a tab as well, the column of the cursor
    // might have actually been reduced, so need to adjust here.
    اگر (t1.lnum == curbuf->b_op_start_orig.lnum
	    && LT_POS(curbuf->b_op_start_orig, t1))
	oap->start = curbuf->b_op_start_orig;

    // If user has moved off this line, we don't know what to do, so do
    // nothing.
    // Also don't repeat the insert when Insert mode ended with CTRL-C.
    اگر (curwin->w_cursor.lnum != oap->start.lnum || got_int)
	بازگشت;

    اگر (oap->block_mode)
    {
	صحیح			ins_len;
	char_u			*firstline, *ins_text;
	ساختار block_def	bd2;
	صحیح			did_indent = FALSE;
	size_t			len;
	size_t			add;
	// offset when cursor was moved in insert mode
	صحیح			offset = 0;

	// If indent kicked in, the firstline might have changed
	// but only do that, if the indent actually increased.
	ind_post_col = (colnr_T)getwhitecols_curline();
	اگر (curbuf->b_op_start.col > ind_pre_col && ind_post_col > ind_pre_col)
	{
	    bd.textcol += ind_post_col - ind_pre_col;
	    ind_post_vcol = get_indent();
	    bd.start_vcol += ind_post_vcol - ind_pre_vcol;
	    did_indent = TRUE;
	}

	// The user may have moved the cursor before inserting something, try
	// to adjust the block for that.  But only do it, if the difference
	// does not come from indent kicking in.
	اگر (oap->start.lnum == curbuf->b_op_start_orig.lnum
						  && !bd.is_MAX && !did_indent)
	{
	    صحیح t = getviscol2(curbuf->b_op_start_orig.col,
					       curbuf->b_op_start_orig.coladd);

	    اگر (oap->op_type == OP_درج
		    && oap->start.col + oap->start.coladd
			    != curbuf->b_op_start_orig.col
					      + curbuf->b_op_start_orig.coladd)
	    {
		oap->start.col = curbuf->b_op_start_orig.col;
		pre_textlen -= t - oap->start_vcol;
		oap->start_vcol = t;
	    }
	    وگرنه اگر (oap->op_type == OP_APPEND
		    && oap->start.col + oap->start.coladd
			    >= curbuf->b_op_start_orig.col
					      + curbuf->b_op_start_orig.coladd)
	    {
		oap->start.col = curbuf->b_op_start_orig.col;
		// reset pre_textlen to the value of OP_INSERT
		pre_textlen += bd.textlen;
		pre_textlen -= t - oap->start_vcol;
		oap->start_vcol = t;
		oap->op_type = OP_درج;
	    }
	}

	// Spaces and tabs in the indent may have changed to other spaces and
	// tabs.  Get the starting column again and correct the length.
	// Don't do this when "$" used, end-of-line will have changed.
	//
	// if indent was added and the inserted text was after the indent,
	// correct the selection for the new indent.
	اگر (did_indent && bd.textcol - ind_post_col > 0)
	{
	    oap->start.col += ind_post_col - ind_pre_col;
	    oap->start_vcol += ind_post_vcol - ind_pre_vcol;
	    oap->end.col += ind_post_col - ind_pre_col;
	    oap->end_vcol += ind_post_vcol - ind_pre_vcol;
	}
	block_prep(oap, &bd2, oap->start.lnum, TRUE);
	اگر (did_indent && bd.textcol - ind_post_col > 0)
	{
	    // undo for where "oap" is used below
	    oap->start.col -= ind_post_col - ind_pre_col;
	    oap->start_vcol -= ind_post_vcol - ind_pre_vcol;
	    oap->end.col -= ind_post_col - ind_pre_col;
	    oap->end_vcol -= ind_post_vcol - ind_pre_vcol;
	}
	اگر (!bd.is_MAX || bd2.textlen < bd.textlen)
	{
	    اگر (oap->op_type == OP_APPEND)
	    {
		pre_textlen += bd2.textlen - bd.textlen;
		اگر (bd2.endspaces)
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
	اگر (oap->op_type == OP_APPEND)
	{
	    add += bd.textlen;
	    // account for pressing cursor in insert mode when '$' was used
	    اگر (bd.is_MAX
		&& (start_درج.lnum == Insstart.lnum
					   && start_درج.col > Insstart.col))
	    {
		offset = (start_درج.col - Insstart.col);
		add -= offset;
		اگر (oap->end_vcol > offset)
		    oap->end_vcol -= (offset + 1);
		وگرنه
		    // moved outside of the visual block, what to do?
		    بازگشت;
	    }
	}
	اگر (add > len)
	    add = len;  // short line, point to the NUL
	firstline += add;
	len -= add;
	اگر (pre_textlen >= 0 && (ins_len = (صحیح)len - pre_textlen - offset) > 0)
	{
	    ins_text = vim_strnsave(firstline, ins_len);
	    اگر (ins_text != پوچ)
	    {
		// block handled here
		اگر (u_save(oap->start.lnum,
					 (linenr_T)(oap->end.lnum + 1)) == OK)
		    block_درج(oap, ins_text, ins_len, (oap->op_type == OP_درج),
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
    صحیح
op_change(oparg_T *oap)
{
    colnr_T		l;
    صحیح			retval;
    linenr_T		linenr;
    long		pre_textlen = 0;
    long		pre_indent = 0;
    char_u		*firstline;
    char_u		*ins_text, *newp, *oldp;
    ساختار block_def	bd;

    l = oap->start.col;
    اگر (oap->motion_type == MLINE)
    {
	l = 0;
	can_si = may_do_si();	// Like opening a new line, do smart indent
    }

    // First delete the text in the region.  In an empty buffer only need to
    // save for undo
    اگر (curbuf->b_ml.ml_flags & ML_EMPTY)
    {
	اگر (u_save_cursor() == FAIL)
	    بازگشت FALSE;
    }
    وگرنه اگر (op_delete(oap) == FAIL)
	بازگشت FALSE;

    اگر ((l > curwin->w_cursor.col) && !LINEEMPTY(curwin->w_cursor.lnum)
							 && !virtual_op)
	inc_cursor();

    // check for still on same line (<CR> in inserted text meaningless)
    // skip blank lines too
    اگر (oap->block_mode)
    {
	// Add spaces before getting the current line length.
	اگر (virtual_op && (curwin->w_cursor.coladd > 0
						    || gchar_cursor() == NUL))
	    coladvance_force(getviscol());
	firstline = ml_get(oap->start.lnum);
	pre_textlen = ml_get_len(oap->start.lnum);
	pre_indent = (long)getwhitecols(firstline);
	bd.textcol = curwin->w_cursor.col;
    }

    اگر (oap->motion_type == MLINE)
	fix_indent();

    // Reset finish_op now, don't want it set inside edit().
    صحیح save_finish_op = finish_op;
    finish_op = FALSE;

    retval = edit(NUL, FALSE, (linenr_T)1);

    finish_op = save_finish_op;

    /*
     * In Visual block mode, handle copying the new text to all lines of the
     * block.
     * Don't repeat the insert when Insert mode ended with CTRL-C.
     */
    اگر (oap->block_mode && oap->start.lnum != oap->end.lnum && !got_int)
    {
	صحیح	ins_len;

	// Auto-indenting may have changed the indent.  If the cursor was past
	// the indent, exclude that indent change from the inserted text.
	firstline = ml_get(oap->start.lnum);
	اگر (bd.textcol > (colnr_T)pre_indent)
	{
	    long new_indent = (long)getwhitecols(firstline);

	    pre_textlen += new_indent - pre_indent;
	    bd.textcol += new_indent - pre_indent;
	}

	ins_len = (صحیح)ml_get_len(oap->start.lnum) - pre_textlen;
	اگر (ins_len > 0)
	{
	    // Subsequent calls to ml_get() flush the firstline data - take a
	    // copy of the inserted text.
	    اگر ((ins_text = alloc(ins_len + 1)) != پوچ)
	    {
		vim_strncpy(ins_text, firstline + bd.textcol, ins_len);
		برای (linenr = oap->start.lnum + 1; linenr <= oap->end.lnum;
								     linenr++)
		{
		    block_prep(oap, &bd, linenr, TRUE);
		    اگر (!bd.is_short || virtual_op)
		    {
			pos_T vpos;
			size_t newlen;

			// If the block starts in virtual space, count the
			// initial coladd offset as part of "startspaces"
			اگر (bd.is_short)
			{
			    vpos.lnum = linenr;
			    (تهی)getvpos(&vpos, oap->start_vcol);
			}
			وگرنه
			    vpos.coladd = 0;
			oldp = ml_get(linenr);
			newp = alloc(ml_get_len(linenr) + vpos.coladd + ins_len + 1);
			اگر (newp == پوچ)
			    ادامه;
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
			اگر (curbuf->b_has_textprop)
			    adjust_prop_columns(linenr, bd.textcol,
						     vpos.coladd + (صحیح)ins_len, 0);
#endif
		    }
		}
		check_cursor();

		changed_سطرها(oap->start.lnum + 1, 0, oap->end.lnum + 1, 0L);
	    }
	    vim_free(ins_text);
	}
    }
    auto_format(FALSE, TRUE);

    بازگشت retval;
}

/*
 * When the cursor is on the NUL past the end of the line and it should not be
 * there move it left.
 */
    تهی
adjust_cursor_eol(تهی)
{
    unsigned صحیح cur_ve_flags = get_ve_flags();

    صحیح adj_cursor = (curwin->w_cursor.col > 0
				&& gchar_cursor() == NUL
				&& (cur_ve_flags & VE_ONEMORE) == 0
				&& (cur_ve_flags & VE_ALL) == 0
				&& !(restart_edit || (State & MODE_درج)));
    اگر (!adj_cursor)
	بازگشت;

    // Put the cursor on the last character in the line.
    dec_cursor();

    اگر (cur_ve_flags == VE_ALL)
    {
	colnr_T	    scol, ecol;

	// Coladd is set to the width of the last character.
	getvcol(curwin, &curwin->w_cursor, &scol, پوچ, &ecol, 0);
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
    char_u   *سطر,
    صحیح      process,
    صحیح	     include_space,
    صحیح      *is_comment)
{
    char_u *comment_flags = پوچ;
    صحیح    lead_len;
    صحیح    leader_offset = get_last_leader_offset(سطر, &comment_flags);

    *is_comment = FALSE;
    اگر (leader_offset != -1)
    {
	// Let's check whether the line ends with an unclosed comment.
	// If the last comment leader has COM_END in flags, there's no comment.
	وقتی (*comment_flags)
	{
	    اگر (*comment_flags == COM_END
		    || *comment_flags == ':')
		بشکن;
	    ++comment_flags;
	}
	اگر (*comment_flags != COM_END)
	    *is_comment = TRUE;
    }

    اگر (process == FALSE)
	بازگشت سطر;

    lead_len = get_leader_len(سطر, &comment_flags, FALSE, include_space);

    اگر (lead_len == 0)
	بازگشت سطر;

    // Find:
    // - COM_END,
    // - colon,
    // whichever comes first.
    وقتی (*comment_flags)
    {
	اگر (*comment_flags == COM_END
		|| *comment_flags == ':')
	    بشکن;
	++comment_flags;
    }

    // If we found a colon, it means that we are not processing a line
    // starting with a closing part of a three-part comment. That's good,
    // because we don't want to remove those as this would be annoying.
    اگر (*comment_flags == ':' || *comment_flags == NUL)
	سطر += lead_len;

    بازگشت سطر;
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
    صحیح
do_join(
    long    شمار,
    صحیح	    درج_space,
    صحیح	    save_undo,
    صحیح	    use_formatoptions UNUSED,
    صحیح	    setmark)
{
    char_u	*curr = پوچ;
    char_u      *curr_start = پوچ;
    char_u	*cend;
    char_u	*newp;
    size_t	newp_len;
    char_u	*spaces;	// number of spaces inserted before a line
    صحیح		endcurr1 = NUL;
    صحیح		endcurr2 = NUL;
    صحیح		currsize = 0;	// size of the current line
    صحیح		sumsize = 0;	// size of the long new line
    linenr_T	t;
    colnr_T	col = 0;
    صحیح		ret = OK;
    صحیح		*comments = پوچ;
    صحیح		remove_comments = (use_formatoptions == TRUE)
				  && has_format_option(FO_REMOVE_COMS);
    صحیح		prev_was_comment;

    اگر (save_undo && u_save((linenr_T)(curwin->w_cursor.lnum - 1),
			    (linenr_T)(curwin->w_cursor.lnum + شمار)) == FAIL)
	بازگشت FAIL;

    // Allocate an array to store the number of spaces inserted before each
    // line.  We will use it to pre-compute the length of the new line and the
    // proper placement of each original line in the new one.
    spaces = lalloc_clear(شمار, TRUE);
    اگر (spaces == پوچ)
	بازگشت FAIL;
    اگر (remove_comments)
    {
	comments = lalloc_clear(شمار * اندازه(صحیح), TRUE);
	اگر (comments == پوچ)
	{
	    vim_free(spaces);
	    بازگشت FAIL;
	}
    }

    /*
     * Don't move anything yet, just compute the final line length
     * and setup the array of space strings lengths
     * This loops forward over the joined lines.
     */
    برای (t = 0; t < شمار; ++t)
    {
	curr = curr_start = ml_get((linenr_T)(curwin->w_cursor.lnum + t));
	اگر (t == 0 && setmark && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set the '[ mark.
	    curwin->w_buffer->b_op_start.lnum = curwin->w_cursor.lnum;
	    curwin->w_buffer->b_op_start.col  = (colnr_T)STRLEN(curr);
	}
	اگر (remove_comments)
	{
	    // We don't want to remove the comment leader if the
	    // previous line is not a comment.
	    اگر (t > 0 && prev_was_comment)
	    {

		char_u *new_curr = skip_comment(curr, TRUE, درج_space,
							   &prev_was_comment);
		comments[t] = (صحیح)(new_curr - curr);
		curr = new_curr;
	    }
	    وگرنه
		curr = skip_comment(curr, FALSE, درج_space,
							   &prev_was_comment);
	}

	اگر (درج_space && t > 0)
	{
	    curr = skipwhite(curr);
	    اگر (*curr != NUL && *curr != ')'
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
		اگر (endcurr1 == ' ')
		    endcurr1 = endcurr2;
		وگرنه
		    ++spaces[t];
		// extra space when 'joinspaces' set and line ends in '.'
		اگر (       p_js
			&& (endcurr1 == '.'
			    || (vim_strchr(p_cpo, CPO_JOINSP) == پوچ
				&& (endcurr1 == '?' || endcurr1 == '!'))))
		    ++spaces[t];
	    }
	}
	currsize = (صحیح)STRLEN(curr);
	sumsize += currsize + spaces[t];
	endcurr1 = endcurr2 = NUL;
	اگر (درج_space && currsize > 0)
	{
	    اگر (has_mbyte)
	    {
		cend = curr + currsize;
		MB_PTR_BACK(curr, cend);
		endcurr1 = (*mb_ptr2char)(cend);
		اگر (cend > curr)
		{
		    MB_PTR_BACK(curr, cend);
		    endcurr2 = (*mb_ptr2char)(cend);
		}
	    }
	    وگرنه
	    {
		endcurr1 = *(curr + currsize - 1);
		اگر (currsize > 1)
		    endcurr2 = *(curr + currsize - 2);
	    }
	}
	سطر_breakcheck();
	اگر (got_int)
	{
	    ret = FAIL;
	    برو theend;
	}
    }

    // store the column position before last line
    col = sumsize - currsize - spaces[شمار - 1];

    // allocate the space for the new line
    newp_len = sumsize + 1;
    newp = alloc(newp_len);
    اگر (newp == پوچ)
    {
	ret = FAIL;
	برو theend;
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
    صحیح curr_off = (صحیح)(curr - curr_start);

    curr = curr_start = ml_get((linenr_T)(curwin->w_cursor.lnum + شمار - 1));
    curr += curr_off;
#endif

    برای (t = شمار - 1; ; --t)
    {
	صحیح spaces_removed;

	cend -= currsize;
	mch_memmove(cend, curr, (size_t)currsize);

	اگر (spaces[t] > 0)
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
		t == شمار - 1, (long)(cend - newp), spaces_removed);
#endif
	اگر (t == 0)
	    بشکن;
	curr = curr_start = ml_get((linenr_T)(curwin->w_cursor.lnum + t - 1));
	اگر (remove_comments)
	    curr += comments[t - 1];
	اگر (درج_space && t > 1)
	    curr = skipwhite(curr);
	currsize = (صحیح)STRLEN(curr);
    }

#ifdef FEAT_PROP_POPUP
    اگر (um.buf != پوچ)
    {
	um_set_text(&um, newp);
	um_reverse_props(&um);
	um_close(&um);
	newp = پوچ;  // um_set_text took ownership
    }
    وگرنه
#endif
    ml_replace_len(curwin->w_cursor.lnum, newp, (colnr_T)newp_len, TRUE, FALSE);

    اگر (setmark && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set the '] mark.
	curwin->w_buffer->b_op_end.lnum = curwin->w_cursor.lnum;
	curwin->w_buffer->b_op_end.col  = (colnr_T)sumsize;
    }

    // Only report the change in the first line here, del_lines() will report
    // the deleted line.
    changed_سطرها(curwin->w_cursor.lnum, currsize,
					       curwin->w_cursor.lnum + 1, 0L);
    /*
     * Delete following lines. To do this we move the cursor there
     * briefly, and then move it back. After del_lines() the cursor may
     * have moved up (last line deleted), so the current lnum is kept in t.
     */
    t = curwin->w_cursor.lnum;
    ++curwin->w_cursor.lnum;
    del_سطرها(شمار - 1, FALSE);
    curwin->w_cursor.lnum = t;

    /*
     * Set the cursor column:
     * Vi compatible: use the column of the first join
     * vim:	      use the column of the last join
     */
    curwin->w_cursor.col =
		    (vim_strchr(p_cpo, CPO_JOINCOL) != پوچ ? currsize : col);
    check_cursor_col();

    curwin->w_cursor.coladd = 0;
    curwin->w_set_curswant = true;

theend:
    vim_free(spaces);
    اگر (remove_comments)
	vim_free(comments);
    بازگشت ret;
}

#ifdef FEAT_LINEBREAK
/*
 * TODO: consider using "tailp" of win_lbr_chartabsize() instead of temporarily
 * resetting 'linebreak'.
 *
 * Reset 'linebreak' and take care of side effects.
 * Returns the previous value, to be passed to restore_lbr().
 */
    صحیح
reset_lbr(تهی)
{
    اگر (!curwin->w_p_lbr)
	بازگشت FALSE;
    // changing 'linebreak' may require w_virtcol to be updated
    curwin->w_p_lbr = FALSE;
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);
    بازگشت TRUE;
}

/*
 * Restore 'linebreak' and take care of side effects.
 */
    تهی
restore_lbr(صحیح lbr_saved)
{
    اگر (curwin->w_p_lbr || !lbr_saved)
	بازگشت;

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
    تهی
block_prep(
    oparg_T		*oap,
    ساختار block_def	*bdp,
    linenr_T		lnum,
    صحیح			is_del)
{
    صحیح		incr = 0;
    char_u	*pend;
    char_u	*pstart;
    char_u	*سطر;
    char_u	*prev_pstart;
    char_u	*prev_pend;
    chartabsize_T cts;
#ifdef FEAT_LINEBREAK
		// Avoid a problem with unwanted linebreaks in block mode.
    صحیح		lbr_saved = reset_lbr();
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

    سطر = ml_get(lnum);
    prev_pstart = سطر;
    init_chartabsize_arg(&cts, curwin, lnum, bdp->start_vcol, سطر, سطر);
    وقتی (cts.cts_vcol < oap->start_vcol && *cts.cts_ptr != NUL)
    {
	// Count a tab for what it's worth (if list mode not on)
	incr = lbr_chartabsize(&cts);
	cts.cts_vcol += incr;
	اگر (VIM_ISWHITE(*cts.cts_ptr))
	{
	    bdp->pre_whitesp += incr;
	    bdp->pre_whitesp_c++;
	}
	وگرنه
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
    اگر (bdp->start_vcol < oap->start_vcol)	// line too short
    {
	bdp->end_vcol = bdp->start_vcol;
	bdp->is_short = TRUE;
	اگر (!is_del || oap->op_type == OP_APPEND)
	    bdp->endspaces = oap->end_vcol - oap->start_vcol + 1;
    }
    وگرنه
    {
	// notice: this converts partly selected Multibyte characters to
	// spaces, too.
	bdp->startspaces = bdp->start_vcol - oap->start_vcol;
	اگر (is_del && bdp->startspaces)
	    bdp->startspaces = bdp->start_char_vcols - bdp->startspaces;
	pend = pstart;
	bdp->end_vcol = bdp->start_vcol;
	اگر (bdp->end_vcol > oap->end_vcol)	// it's all in one character
	{
	    bdp->is_oneChar = TRUE;
	    اگر (oap->op_type == OP_درج)
		bdp->endspaces = bdp->start_char_vcols - bdp->startspaces;
	    وگرنه اگر (oap->op_type == OP_APPEND)
	    {
		bdp->startspaces += oap->end_vcol - oap->start_vcol + 1;
		bdp->endspaces = bdp->start_char_vcols - bdp->startspaces;
	    }
	    وگرنه
	    {
		bdp->startspaces = oap->end_vcol - oap->start_vcol + 1;
		اگر (is_del && oap->op_type != OP_LSHIFT)
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
	وگرنه
	{
	    init_chartabsize_arg(&cts, curwin, lnum, bdp->end_vcol,
								  سطر, pend);
	    prev_pend = pend;
	    وقتی (cts.cts_vcol <= oap->end_vcol && *cts.cts_ptr != NUL)
	    {
		// count a tab for what it's worth (if list mode not on)
		prev_pend = cts.cts_ptr;
		incr = lbr_chartabsize_adv(&cts);
		cts.cts_vcol += incr;
	    }
	    bdp->end_vcol = cts.cts_vcol;
	    pend = cts.cts_ptr;
	    clear_chartabsize_arg(&cts);

	    اگر (bdp->end_vcol <= oap->end_vcol
		    && (!is_del
			|| oap->op_type == OP_APPEND
			|| oap->op_type == OP_REPLACE)) // line too short
	    {
		bdp->is_short = TRUE;
		// Alternative: include spaces to fill up the block.
		// Disadvantage: can lead to trailing spaces when the line is
		// short where the text is put
		// if (!is_del || oap->op_type == OP_APPEND)
		اگر (oap->op_type == OP_APPEND || virtual_op)
		    bdp->endspaces = oap->end_vcol - bdp->end_vcol
							     + oap->inclusive;
		وگرنه
		    bdp->endspaces = 0; // replace doesn't add characters
	    }
	    وگرنه اگر (bdp->end_vcol > oap->end_vcol)
	    {
		bdp->endspaces = bdp->end_vcol - oap->end_vcol - 1;
		اگر (!is_del && bdp->endspaces)
		{
		    bdp->endspaces = incr - bdp->endspaces;
		    اگر (pend != pstart)
			pend = prev_pend;
		}
	    }
	}
	bdp->end_char_vcols = incr;
	اگر (is_del && bdp->startspaces)
	    pstart = prev_pstart;
	bdp->textlen = (صحیح)(pend - pstart);
    }
    bdp->textcol = (colnr_T) (pstart - سطر);
    bdp->textstart = pstart;
#ifdef FEAT_LINEBREAK
    restore_lbr(lbr_saved);
#endif
}

/*
 * Get block text from "start" to "end"
 */
    تهی
charwise_block_prep(
    pos_T		start,
    pos_T		end,
    ساختار block_def	*bdp,
    linenr_T		lnum,
    صحیح			inclusive)
{
    colnr_T startcol = 0, endcol = MAXCOL;
    colnr_T cs, ce;
    char_u *p;
    صحیح	plen = ml_get_len(lnum);

    p = ml_get(lnum);
    bdp->startspaces = 0;
    bdp->endspaces = 0;
    bdp->is_oneChar = FALSE;
    bdp->start_char_vcols = 0;

    اگر (lnum == start.lnum)
    {
	startcol = start.col;
	اگر (virtual_op)
	{
	    getvcol(curwin, &start, &cs, پوچ, &ce, 0);
	    اگر (ce != cs && start.coladd > 0)
	    {
		// Part of a tab selected -- but don't
		// double-count it.
		bdp->start_char_vcols = ce - cs + 1;
		bdp->startspaces = bdp->start_char_vcols - start.coladd;
		اگر (bdp->startspaces < 0)
		    bdp->startspaces = 0;
		startcol++;
	    }
	}
    }

    اگر (lnum == end.lnum)
    {
	endcol = end.col;
	اگر (virtual_op)
	{
	    getvcol(curwin, &end, &cs, پوچ, &ce, 0);
	    اگر (p[endcol] == NUL || (cs + end.coladd < ce
			// Don't add space for double-wide
			// char; endcol will be on last byte
			// of multi-byte char.
			&& (*mb_head_off)(p, p + endcol) == 0))
	    {
		اگر (start.lnum == end.lnum && start.col == end.col)
		{
		    // Special case: inside a single char
		    bdp->is_oneChar = TRUE;
		    bdp->startspaces = end.coladd - start.coladd + inclusive;
		    endcol = startcol;
		}
		وگرنه
		{
		    bdp->endspaces = end.coladd + inclusive;
		    endcol -= inclusive;
		}
	    }
	}
    }
    اگر (endcol == MAXCOL)
	endcol = ml_get_len(lnum);
    اگر (startcol > endcol || bdp->is_oneChar)
	bdp->textlen = 0;
    وگرنه
	bdp->textlen = endcol - startcol + inclusive;
    bdp->textcol = startcol;
    bdp->textstart = startcol <= plen ? p + startcol : p;
}

/*
 * Handle the add/subtract operator.
 */
    تهی
op_addsub(
    oparg_T	*oap,
    linenr_T	Prenum1,	    // Amount of add/subtract
    صحیح		g_cmd)		    // was g<c-a>/g<c-x>
{
    pos_T		pos;
    ساختار block_def	bd;
    صحیح			change_cnt = 0;
    linenr_T		amount = Prenum1;

    // do_addsub() might trigger re-evaluation of 'foldexpr' halfway, when the
    // buffer is not completely updated yet. Postpone updating folds until before
    // the call to changed_lines().
#ifdef FEAT_FOLDING
    disable_fold_update++;
#endif

    اگر (!VIsual_active)
    {
	pos = curwin->w_cursor;
	اگر (u_save_cursor() == FAIL)
	{
#ifdef FEAT_FOLDING
	    disable_fold_update--;
#endif
	    بازگشت;
	}
	change_cnt = do_addsub(oap->op_type, &pos, 0, amount);
#ifdef FEAT_FOLDING
	disable_fold_update--;
#endif
	اگر (change_cnt)
	    changed_سطرها(pos.lnum, 0, pos.lnum + 1, 0L);
    }
    وگرنه
    {
	صحیح	one_change;
	صحیح	length;
	pos_T	startpos;

	اگر (u_save((linenr_T)(oap->start.lnum - 1),
					(linenr_T)(oap->end.lnum + 1)) == FAIL)
	{
#ifdef FEAT_FOLDING
	    disable_fold_update--;
#endif
	    بازگشت;
	}

	pos = oap->start;
	برای (; pos.lnum <= oap->end.lnum; ++pos.lnum)
	{
	    اگر (oap->block_mode)		    // Visual block mode
	    {
		block_prep(oap, &bd, pos.lnum, FALSE);
		pos.col = bd.textcol;
		length = bd.textlen;
	    }
	    وگرنه اگر (oap->motion_type == MLINE)
	    {
		curwin->w_cursor.col = 0;
		pos.col = 0;
		length = ml_get_len(pos.lnum);
	    }
	    وگرنه // oap->motion_type == MCHAR
	    {
		اگر (pos.lnum == oap->start.lnum && !oap->inclusive)
		    dec(&(oap->end));
		length = ml_get_len(pos.lnum);
		pos.col = 0;
		اگر (pos.lnum == oap->start.lnum)
		{
		    pos.col += oap->start.col;
		    length -= oap->start.col;
		}
		اگر (pos.lnum == oap->end.lnum)
		{
		    length = ml_get_len(oap->end.lnum);
		    اگر (oap->end.col >= length)
			oap->end.col = length - 1;
		    length = oap->end.col - pos.col + 1;
		}
	    }
	    one_change = do_addsub(oap->op_type, &pos, length, amount);
	    اگر (one_change)
	    {
		// Remember the start position of the first change.
		اگر (change_cnt == 0)
		    startpos = curbuf->b_op_start;
		++change_cnt;
	    }

#ifdef FEAT_NETBEANS_INTG
	    اگر (netbeans_active() && one_change)
	    {
		char_u *ptr;

		netbeans_removed(curbuf, pos.lnum, pos.col, (long)length);
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, pos.col,
						&ptr[pos.col], length);
	    }
#endif
	    اگر (g_cmd && one_change)
		amount += Prenum1;
	}

#ifdef FEAT_FOLDING
	disable_fold_update--;
#endif
	اگر (change_cnt)
	    changed_سطرها(oap->start.lnum, 0, oap->end.lnum + 1, 0L);

	اگر (!change_cnt && oap->is_VIsual)
	    // No change: need to remove the Visual selection
	    redraw_curbuf_later(UPD_INVERTED);

	// Set '[ mark if something changed. Keep the last end
	// position from do_addsub().
	اگر (change_cnt > 0 && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	    curbuf->b_op_start = startpos;

	اگر (change_cnt > p_report)
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
    ایستا صحیح
do_addsub(
    صحیح		op_type,
    pos_T	*pos,
    صحیح		length,
    linenr_T	Prenum1)
{
    صحیح		col;
    صحیح		pre;		// 'X'/'x': hex; '0': octal; 'B'/'b': bin
    ایستا صحیح	hexupper = FALSE;	// 0xABC
    uvarnumber_T	n;
    uvarnumber_T	oldn;
    char_u	*ptr;
    صحیح		linelen;
    صحیح		c;
    صحیح		todel;
    صحیح		do_hex;
    صحیح		do_oct;
    صحیح		do_bin;
    صحیح		do_alpha;
    صحیح		do_unsigned;
    صحیح		do_blank;
    صحیح		blank_unsigned = FALSE;	// blank: treat as unsigned?
    صحیح		firstdigit;
    صحیح		subtract;
    صحیح		negative = FALSE;
    صحیح		was_positive = TRUE;
    صحیح		visual = VIsual_active;
    صحیح		did_change = FALSE;
    pos_T	save_cursor = curwin->w_cursor;
    صحیح		maxlen = 0;
    pos_T	startpos;
    pos_T	endpos;
    colnr_T	save_coladd = 0;

    do_hex = (vim_strchr(curbuf->b_p_nf, 'x') != پوچ);	// "heX"
    do_oct = (vim_strchr(curbuf->b_p_nf, 'o') != پوچ);	// "Octal"
    do_bin = (vim_strchr(curbuf->b_p_nf, 'b') != پوچ);	// "Bin"
    do_alpha = (vim_strchr(curbuf->b_p_nf, 'p') != پوچ);	// "alPha"
    do_unsigned = (vim_strchr(curbuf->b_p_nf, 'u') != پوچ);	// "Unsigned"
    do_blank = (vim_strchr(curbuf->b_p_nf, 'k') != پوچ);	// "blanK"

    اگر (virtual_active())
    {
	save_coladd = pos->coladd;
	pos->coladd = 0;
    }

    curwin->w_cursor = *pos;
    ptr = ml_get(pos->lnum);
    linelen = ml_get_len(pos->lnum);
    col = pos->col;

    اگر (col + !!save_coladd >= linelen)
	برو theend;

    /*
     * First check if we are on a hexadecimal number, after the "0x".
     */
    اگر (!VIsual_active)
    {
	اگر (do_bin)
	    وقتی (col > 0 && vim_isbdigit(ptr[col]))
	    {
		--col;
		اگر (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }

	اگر (do_hex)
	    وقتی (col > 0 && vim_isxdigit(ptr[col]))
	    {
		--col;
		اگر (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }

	اگر (       do_bin
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

	    وقتی (col > 0 && vim_isdigit(ptr[col]))
	    {
		col--;
		اگر (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }
	}

	اگر ((       do_hex
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
	    اگر (has_mbyte)
		col -= (*mb_head_off)(ptr, ptr + col);
	}
	وگرنه
	{
	    /*
	     * Search forward and then backward to find the start of number.
	     */
	    col = pos->col;

	    وقتی (ptr[col] != NUL
		    && !vim_isdigit(ptr[col])
		    && !(do_alpha && ASCII_ISALPHA(ptr[col])))
		col += mb_ptr2len(ptr + col);

	    وقتی (col > 0
		    && vim_isdigit(ptr[col - 1])
		    && !(do_alpha && ASCII_ISALPHA(ptr[col])))
	    {
		--col;
		اگر (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }
	}
    }

    اگر (visual)
    {
	وقتی (ptr[col] != NUL && length > 0
		&& !vim_isdigit(ptr[col])
		&& !(do_alpha && ASCII_ISALPHA(ptr[col])))
	{
	    صحیح mb_len = mb_ptr2len(ptr + col);

	    col += mb_len;
	    length -= mb_len;
	}

	اگر (length == 0)
	    برو theend;

	اگر (col > pos->col && ptr[col - 1] == '-'
		&& (!has_mbyte || !(*mb_head_off)(ptr, ptr + col - 1))
		&& !do_unsigned)
	{
	    اگر (do_blank && col >= 2 && !VIM_ISWHITE(ptr[col - 2]))
		blank_unsigned = TRUE;
	    وگرنه
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
    اگر (!VIM_ISDIGIT(firstdigit) && !(do_alpha && ASCII_ISALPHA(firstdigit)))
    {
	beep_flush();
	برو theend;
    }

    اگر (do_alpha && ASCII_ISALPHA(firstdigit))
    {
	// decrement or increment alphabetic character
	اگر (op_type == OP_NR_SUB)
	{
	    اگر (CharOrd(firstdigit) < Prenum1)
	    {
		اگر (SAFE_isupper(firstdigit))
		    firstdigit = 'A';
		وگرنه
		    firstdigit = 'a';
	    }
	    وگرنه
		firstdigit -= Prenum1;
	}
	وگرنه
	{
	    اگر (26 - CharOrd(firstdigit) - 1 < Prenum1)
	    {
		اگر (SAFE_isupper(firstdigit))
		    firstdigit = 'Z';
		وگرنه
		    firstdigit = 'z';
	    }
	    وگرنه
		firstdigit += Prenum1;
	}
	curwin->w_cursor.col = col;
	اگر (!did_change)
	    startpos = curwin->w_cursor;
	did_change = TRUE;
	(تهی)del_char(FALSE);
	ins_char(firstdigit);
	endpos = curwin->w_cursor;
	curwin->w_cursor.col = col;
    }
    وگرنه
    {
	char_u	*buf1;
	صحیح	buf1len;
	char_u	buf2[NUMBUFLEN];
	صحیح	buf2len;
	pos_T	save_pos;
	صحیح	i;

	اگر (col > 0 && ptr[col - 1] == '-'
		&& (!has_mbyte ||
		    !(*mb_head_off)(ptr, ptr + col - 1))
		&& !visual
		&& !do_unsigned)
	{
	    اگر (do_blank && col >= 2 && !VIM_ISWHITE(ptr[col - 2]))
		blank_unsigned = TRUE;
	    وگرنه
	    {
		// negative number
		--col;
		negative = TRUE;
	    }
	}

	// get the number value (unsigned)
	اگر (visual && VIsual_mode != 'V')
	    maxlen = (curbuf->b_visual.vi_curswant == MAXCOL
		    ? linelen - col : length);

	صحیح overflow = FALSE;
	vim_str2nr(ptr + col, &pre, &length,
		0 + (do_bin ? STR2NR_BIN : 0)
		    + (do_oct ? STR2NR_OCT : 0)
		    + (do_hex ? STR2NR_HEX : 0),
		پوچ, &n, maxlen, FALSE, &overflow);

	// ignore leading '-' for hex and octal and bin numbers
	اگر (pre && negative)
	{
	    ++col;
	    --length;
	    negative = FALSE;
	}
	// add or subtract
	subtract = FALSE;
	اگر (op_type == OP_NR_SUB)
	    subtract ^= TRUE;
	اگر (negative)
	    subtract ^= TRUE;

	oldn = n;
	اگر (!overflow)  // if number is too big don't add/subtract
	{
	    اگر (subtract)
		n -= (uvarnumber_T)Prenum1;
	    وگرنه
		n += (uvarnumber_T)Prenum1;
	}

	// handle wraparound for decimal numbers
	اگر (!pre)
	{
	    اگر (subtract)
	    {
		اگر (n > oldn)
		{
		    n = 1 + (n ^ (uvarnumber_T)-1);
		    negative ^= TRUE;
		}
	    }
	    وگرنه
	    {
		// add
		اگر (n < oldn)
		{
		    n = (n ^ (uvarnumber_T)-1);
		    negative ^= TRUE;
		}
	    }
	    اگر (n == 0)
		negative = FALSE;
	}

	اگر ((do_unsigned || blank_unsigned) && negative)
	{
	    اگر (subtract)
		// sticking at zero.
		n = (uvarnumber_T)0;
	    وگرنه
		// sticking at 2^64 - 1.
		n = (uvarnumber_T)(-1);
	    negative = FALSE;
	}

	اگر (visual && !was_positive && !negative && col > 0)
	{
	    // need to remove the '-'
	    col--;
	    length++;
	}

	/*
	 * Delete the old number.
	 */
	curwin->w_cursor.col = col;
	اگر (!did_change)
	    startpos = curwin->w_cursor;
	did_change = TRUE;
	todel = length;
	c = gchar_cursor();
	/*
	 * Don't include the '-' in the length, only the length of the
	 * part after it is kept the same.
	 */
	اگر (c == '-')
	    --length;

	save_pos = curwin->w_cursor;
	برای (i = 0; i < todel; ++i)
	{
	    اگر (c < 0x100 && SAFE_isalpha(c))
	    {
		اگر (SAFE_isupper(c))
		    hexupper = TRUE;
		وگرنه
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
	اگر (buf1 == پوچ)
	    برو theend;
	ptr = buf1;
	اگر (negative && (!visual || was_positive))
	    *ptr++ = '-';
	اگر (pre)
	{
	    *ptr++ = '0';
	    --length;
	}
	اگر (pre == 'b' || pre == 'B' ||
	    pre == 'x' || pre == 'X')
	{
	    *ptr++ = pre;
	    --length;
	}

	/*
	 * Put the number characters in buf2[].
	 */
	اگر (pre == 'b' || pre == 'B')
	{
	    صحیح bit = 0;
	    صحیح bits = اندازه(uvarnumber_T) * 8;

	    // leading zeros
	    برای (bit = bits; bit > 0; bit--)
		اگر ((n >> (bit - 1)) & 0x1) بشکن;

	    برای (buf2len = 0; bit > 0 && buf2len < (NUMBUFLEN - 1); bit--)
		buf2[buf2len++] = ((n >> (bit - 1)) & 0x1) ? '1' : '0';

	    buf2[buf2len] = NUL;
	}
	وگرنه اگر (pre == 0)
	    buf2len = vim_snprintf((نویسه *)buf2, NUMBUFLEN, "%llu", n);
	وگرنه اگر (pre == '0')
	    buf2len = vim_snprintf((نویسه *)buf2, NUMBUFLEN, "%llo", n);
	وگرنه اگر (pre && hexupper)
	    buf2len = vim_snprintf((نویسه *)buf2, NUMBUFLEN, "%llX", n);
	وگرنه
	    buf2len = vim_snprintf((نویسه *)buf2, NUMBUFLEN, "%llx", n);
	length -= buf2len;

	/*
	 * Adjust number of zeros to the new number of digits, so the
	 * total length of the number remains the same.
	 * Don't do this when
	 * the result may look like an octal number.
	 */
	اگر (firstdigit == '0' && !(do_oct && pre == 0))
	    وقتی (length-- > 0)
		*ptr++ = '0';
	*ptr = NUL;
	buf1len = (صحیح)(ptr - buf1);

	STRCPY(buf1 + buf1len, buf2);
	buf1len += buf2len;

	// Insert just after the first character to be removed, so that any
	// text properties will be adjusted.  Then delete the old number
	// afterwards.
	save_pos = curwin->w_cursor;
	اگر (todel > 0)
	    inc_cursor();
	ins_str(buf1, (size_t)buf1len);		// insert the new number
	vim_free(buf1);

	// del_char() will also mark line needing displaying
	اگر (todel > 0)
	{
	    صحیح bytes_after = ml_get_curline_len() - curwin->w_cursor.col;

	    // Delete the one character before the insert.
	    curwin->w_cursor = save_pos;
	    (تهی)del_char(FALSE);
	    curwin->w_cursor.col = ml_get_curline_len() - bytes_after;
	    --todel;
	}
	وقتی (todel-- > 0)
	    (تهی)del_char(FALSE);

	endpos = curwin->w_cursor;
	اگر (did_change && curwin->w_cursor.col)
	    --curwin->w_cursor.col;
    }

    اگر (did_change && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// set the '[ and '] marks
	curbuf->b_op_start = startpos;
	curbuf->b_op_end = endpos;
	اگر (curbuf->b_op_end.col > 0)
	    --curbuf->b_op_end.col;
    }

theend:
    اگر (visual)
	curwin->w_cursor = save_cursor;
    وگرنه اگر (did_change)
	curwin->w_set_curswant = true;
    وگرنه اگر (virtual_active())
	curwin->w_cursor.coladd = save_coladd;

    بازگشت did_change;
}

    تهی
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
    ایستا varnumber_T
سطر_شمار_info(
    char_u	*سطر,
    varnumber_T	*wc,
    varnumber_T	*cc,
    varnumber_T	limit,
    صحیح		eol_size)
{
    varnumber_T	i;
    varnumber_T	واژگان = 0;
    varnumber_T	chars = 0;
    صحیح		is_word = 0;

    برای (i = 0; i < limit && سطر[i] != NUL; )
    {
	اگر (is_word)
	{
	    اگر (vim_isspace(سطر[i]))
	    {
		واژگان++;
		is_word = 0;
	    }
	}
	وگرنه اگر (!vim_isspace(سطر[i]))
	    is_word = 1;
	++chars;
	i += (*mb_ptr2len)(سطر + i);
    }

    اگر (is_word)
	واژگان++;
    *wc += واژگان;

    // Add eol_size if the end of line was reached before hitting limit.
    اگر (i < limit && سطر[i] == NUL)
    {
	i += eol_size;
	chars += eol_size;
    }
    *cc += chars;
    بازگشت i;
}

/*
 * Give some info about the position of the cursor (for "g CTRL-G").
 * In Visual mode, give some info about the selected region.  (In this case,
 * the *_count_cursor variables store running totals for the selection.)
 * When "dict" is not NULL store the info there instead of showing it.
 */
    تهی
cursor_pos_info(dict_T *dict)
{
    char_u	*p;
    char_u	buf1[50];
    char_u	buf2[40];
    linenr_T	lnum;
    varnumber_T	byte_شمار = 0;
    varnumber_T	bom_شمار  = 0;
    varnumber_T	byte_شمار_cursor = 0;
    varnumber_T	char_شمار = 0;
    varnumber_T	char_شمار_cursor = 0;
    varnumber_T	word_شمار = 0;
    varnumber_T	word_شمار_cursor = 0;
    صحیح		eol_size;
    varnumber_T	last_check = 100000L;
    long	سطر_شمار_selected = 0;
    pos_T	min_pos, max_pos;
    oparg_T	oparg;
    ساختار block_def	bd;

    /*
     * Compute the length of the file in characters.
     */
    اگر (curbuf->b_ml.ml_flags & ML_EMPTY)
    {
	اگر (dict == پوچ)
	{
	    پیام(_(no_سطرها_پیام));
	    بازگشت;
	}
    }
    وگرنه
    {
	اگر (get_fileformat(curbuf) == EOL_DOS)
	    eol_size = 2;
	وگرنه
	    eol_size = 1;

	اگر (VIsual_active)
	{
	    اگر (LT_POS(VIsual, curwin->w_cursor))
	    {
		min_pos = VIsual;
		max_pos = curwin->w_cursor;
	    }
	    وگرنه
	    {
		min_pos = curwin->w_cursor;
		max_pos = VIsual;
	    }
	    اگر (*p_sel == 'e' && max_pos.col > 0)
		--max_pos.col;

	    اگر (VIsual_mode == Ctrl_V)
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
		اگر (curwin->w_curswant == MAXCOL)
		    oparg.end_vcol = MAXCOL;
		// Swap the start, end vcol if needed
		اگر (oparg.end_vcol < oparg.start_vcol)
		{
		    oparg.end_vcol += oparg.start_vcol;
		    oparg.start_vcol = oparg.end_vcol - oparg.start_vcol;
		    oparg.end_vcol -= oparg.start_vcol;
		}
	    }
	    سطر_شمار_selected = max_pos.lnum - min_pos.lnum + 1;
	}

	برای (lnum = 1; lnum <= curbuf->b_ml.ml_سطر_شمار; ++lnum)
	{
	    // Check for a CTRL-C every 100000 characters.
	    اگر (byte_شمار > last_check)
	    {
		ui_breakcheck();
		اگر (got_int)
		    بازگشت;
		last_check = byte_شمار + 100000L;
	    }

	    // Do extra processing for VIsual mode.
	    اگر (VIsual_active
		    && lnum >= min_pos.lnum && lnum <= max_pos.lnum)
	    {
		char_u	    *s = پوچ;
		long	    len = 0L;

		انتخاب (VIsual_mode)
		{
		    حالت Ctrl_V:
			virtual_op = virtual_active();
			block_prep(&oparg, &bd, lnum, 0);
			virtual_op = MAYBE;
			s = bd.textstart;
			len = (long)bd.textlen;
			بشکن;
		    حالت 'V':
			s = ml_get(lnum);
			len = MAXCOL;
			بشکن;
		    حالت 'v':
			{
			    colnr_T start_col = (lnum == min_pos.lnum)
							   ? min_pos.col : 0;
			    colnr_T end_col = (lnum == max_pos.lnum)
				      ? max_pos.col - start_col + 1 : MAXCOL;

			    s = ml_get(lnum) + start_col;
			    len = end_col;
			}
			بشکن;
		}
		اگر (s != پوچ)
		{
		    byte_شمار_cursor += سطر_شمار_info(s, &word_شمار_cursor,
					   &char_شمار_cursor, len, eol_size);
		    اگر (lnum == curbuf->b_ml.ml_سطر_شمار
			    && !curbuf->b_p_eol
			    && (curbuf->b_p_bin || !curbuf->b_p_fixeol)
			    && (long)STRLEN(s) < len)
			byte_شمار_cursor -= eol_size;
		}
	    }
	    وگرنه
	    {
		// In non-visual mode, check for the line the cursor is on
		اگر (lnum == curwin->w_cursor.lnum)
		{
		    word_شمار_cursor += word_شمار;
		    char_شمار_cursor += char_شمار;
		    byte_شمار_cursor = byte_شمار +
			سطر_شمار_info(ml_get(lnum),
				&word_شمار_cursor, &char_شمار_cursor,
				(varnumber_T)(curwin->w_cursor.col + 1),
				eol_size);
		}
	    }
	    // Add to the running totals
	    byte_شمار += سطر_شمار_info(ml_get(lnum), &word_شمار,
					 &char_شمار, (varnumber_T)MAXCOL,
					 eol_size);
	}

	// Correction for when last line doesn't have an EOL.
	اگر (!curbuf->b_p_eol && (curbuf->b_p_bin || !curbuf->b_p_fixeol))
	    byte_شمار -= eol_size;

	اگر (dict == پوچ)
	{
	    اگر (VIsual_active)
	    {
		اگر (VIsual_mode == Ctrl_V && curwin->w_curswant < MAXCOL)
		{
		    getvcols(curwin, &min_pos, &max_pos,
						&min_pos.col, &max_pos.col, 0);
		    vim_snprintf((نویسه *)buf1, اندازه(buf1), _("%ld Cols; "),
			    (long)(oparg.end_vcol - oparg.start_vcol + 1));
		}
		وگرنه
		    buf1[0] = NUL;

		اگر (char_شمار_cursor == byte_شمار_cursor
						    && char_شمار == byte_شمار)
		    vim_snprintf((نویسه *)IObuff, IOSIZE,
			    _("Selected %s%ld of %ld Lines; %lld of %lld Words; %lld of %lld Bytes"),
			    buf1, سطر_شمار_selected,
			    (long)curbuf->b_ml.ml_سطر_شمار,
			    word_شمار_cursor,
			    word_شمار,
			    byte_شمار_cursor,
			    byte_شمار);
		وگرنه
		    vim_snprintf((نویسه *)IObuff, IOSIZE,
			    _("Selected %s%ld of %ld Lines; %lld of %lld Words; %lld of %lld Chars; %lld of %lld Bytes"),
			    buf1, سطر_شمار_selected,
			    (long)curbuf->b_ml.ml_سطر_شمار,
			    word_شمار_cursor,
			    word_شمار,
			    char_شمار_cursor,
			    char_شمار,
			    byte_شمار_cursor,
			    byte_شمار);
	    }
	    وگرنه
	    {
		p = ml_get_curline();
		validate_virtcol();
		col_print(buf1, اندازه(buf1), (صحیح)curwin->w_cursor.col + 1,
			(صحیح)curwin->w_virtcol + 1);
		col_print(buf2, اندازه(buf2), ml_get_curline_len(),
							   linetabsize_str(p));

		اگر (char_شمار_cursor == byte_شمار_cursor
			&& char_شمار == byte_شمار)
		    vim_snprintf((نویسه *)IObuff, IOSIZE,
			_("Col %s of %s; Line %ld of %ld; Word %lld of %lld; Byte %lld of %lld"),
			(نویسه *)buf1, (نویسه *)buf2,
			(long)curwin->w_cursor.lnum,
			(long)curbuf->b_ml.ml_سطر_شمار,
			word_شمار_cursor, word_شمار,
			byte_شمار_cursor, byte_شمار);
		وگرنه
		    vim_snprintf((نویسه *)IObuff, IOSIZE,
			_("Col %s of %s; Line %ld of %ld; Word %lld of %lld; Char %lld of %lld; Byte %lld of %lld"),
			(نویسه *)buf1, (نویسه *)buf2,
			(long)curwin->w_cursor.lnum,
			(long)curbuf->b_ml.ml_سطر_شمار,
			word_شمار_cursor, word_شمار,
			char_شمار_cursor, char_شمار,
			byte_شمار_cursor, byte_شمار);
	    }
	}

	bom_شمار = bomb_size();
	اگر (dict == پوچ && bom_شمار > 0)
	{
	    size_t len = STRLEN(IObuff);

	    vim_snprintf((نویسه *)IObuff + len, IOSIZE - len,
				 _("(+%lld for BOM)"), bom_شمار);
	}
	اگر (dict == پوچ)
	{
	    // Don't shorten this message, the user asked for it.
	    p = p_shm;
	    p_shm = (char_u *)"";
	    پیام((نویسه *)IObuff);
	    p_shm = p;
	}
    }
#اگر defined(FEAT_EVAL)
    اگر (dict != پوچ)
    {
	dict_add_عدد(dict, "words", word_شمار);
	dict_add_عدد(dict, "chars", char_شمار);
	dict_add_عدد(dict, "bytes", byte_شمار + bom_شمار);
	dict_add_عدد(dict, VIsual_active ? "visual_bytes" : "cursor_bytes",
		byte_شمار_cursor);
	dict_add_عدد(dict, VIsual_active ? "visual_chars" : "cursor_chars",
		char_شمار_cursor);
	dict_add_عدد(dict, VIsual_active ? "visual_words" : "cursor_words",
		word_شمار_cursor);
    }
#endif
}

/*
 * Handle indent and format operators and visual mode ":".
 */
    ایستا تهی
op_colon(oparg_T *oap)
{
    stuffcharReadbuff(':');
    اگر (oap->is_VIsual)
	stuffReadbuff((char_u *)"'<,'>");
    وگرنه
    {
	// Make the range look nice, so it can be repeated.
	اگر (oap->start.lnum == curwin->w_cursor.lnum)
	    stuffcharReadbuff('.');
	وگرنه
	    stuffnumReadbuff((long)oap->start.lnum);

#ifdef FEAT_FOLDING
	// When using !! on a closed fold the range ".!" works best to operate
	// on, it will be made the whole closed fold later.
	linenr_T endOfStartFold = oap->start.lnum;
	(تهی)hasFolding(oap->start.lnum, پوچ, &endOfStartFold);
#endif
	اگر (oap->end.lnum != oap->start.lnum
#ifdef FEAT_FOLDING
		&& oap->end.lnum != endOfStartFold
#endif
	   )
	{
	    // Make it a range with the end line.
	    stuffcharReadbuff(',');
	    اگر (oap->end.lnum == curwin->w_cursor.lnum)
		stuffcharReadbuff('.');
	    وگرنه اگر (oap->end.lnum == curbuf->b_ml.ml_سطر_شمار)
		stuffcharReadbuff('$');
	    وگرنه اگر (oap->start.lnum == curwin->w_cursor.lnum
#ifdef FEAT_FOLDING
		    // do not use ".+number" for a closed fold, it would count
		    // folded lines twice
		    && !hasFolding(oap->end.lnum, پوچ, پوچ)
#endif
		    )
	    {
		stuffReadbuff((char_u *)".+");
		stuffnumReadbuff((long)oap->سطر_شمار - 1);
	    }
	    وگرنه
		stuffnumReadbuff((long)oap->end.lnum);
	}
    }
    اگر (oap->op_type != OP_COLON)
	stuffReadbuff((char_u *)"!");
    اگر (oap->op_type == OP_INDENT)
    {
	اگر (*get_equalprg() == NUL)
	    stuffReadbuff((char_u *)"indent");
	وگرنه
	    stuffReadbuff(get_equalprg());
	stuffReadbuff((char_u *)"\n");
    }
    وگرنه اگر (oap->op_type == OP_FORMAT)
    {
	اگر (*curbuf->b_p_fp != NUL)
	    stuffReadbuff(curbuf->b_p_fp);
	وگرنه اگر (*p_fp != NUL)
	    stuffReadbuff(p_fp);
	وگرنه
	    stuffReadbuff((char_u *)"fmt");
	stuffReadbuff((char_u *)"\n']");
    }

    // do_cmdline() does the rest
}

// callback function for 'operatorfunc'
ایستا callback_T opfunc_cb;

/*
 * Process the 'operatorfunc' option value.
 * Returns OK or FAIL.
 */
    نویسه *
did_set_operatorfunc(optset_T *args UNUSED)
{
    اگر (option_set_callback_func(p_opfunc, &opfunc_cb) == FAIL)
	بازگشت e_invalid_argument;

    بازگشت پوچ;
}

#اگر defined(EXITFREE)
    تهی
free_operatorfunc_option(تهی)
{
# ifdef FEAT_EVAL
    free_callback(&opfunc_cb);
# endif
}
#endif

#اگر defined(FEAT_EVAL)
/*
 * Mark the global 'operatorfunc' callback with "copyID" so that it is not
 * garbage collected.
 */
    صحیح
set_ref_in_opfunc(صحیح copyID UNUSED)
{
    صحیح abort = FALSE;

    abort = set_ref_in_callback(&opfunc_cb, copyID);

    بازگشت abort;
}
#endif

/*
 * Handle the "g@" operator: call 'operatorfunc'.
 */
    ایستا تهی
op_function(oparg_T *oap UNUSED)
{
#ifdef FEAT_EVAL
    typval_T	argv[2];
    pos_T	orig_start = curbuf->b_op_start;
    pos_T	orig_end = curbuf->b_op_end;
    typval_T	rettv;

    اگر (*p_opfunc == NUL)
	emsg(_(e_operatorfunc_is_empty));
    وگرنه
    {
	// Set '[ and '] marks to text to be operated on.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
	اگر (oap->motion_type != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_end);

	argv[0].v_type = VAR_STRING;
	اگر (oap->block_mode)
	    argv[0].vval.v_string = (char_u *)"block";
	وگرنه اگر (oap->motion_type == MLINE)
	    argv[0].vval.v_string = (char_u *)"line";
	وگرنه
	    argv[0].vval.v_string = (char_u *)"char";
	argv[1].v_type = VAR_UNKNOWN;

	// Reset virtual_op so that 'virtualedit' can be changed in the
	// function.
	صحیح save_virtual_op = virtual_op;
	virtual_op = MAYBE;

	// Reset finish_op so that mode() returns the right value.
	صحیح save_finish_op = finish_op;
	finish_op = FALSE;

	اگر (call_callback(&opfunc_cb, 0, &rettv, 1, argv) != FAIL)
	    clear_tv(&rettv);

	virtual_op = save_virtual_op;
	finish_op = save_finish_op;
	اگر (cmdmod.cmod_flags & CMOD_LOCKMARKS)
	{
	    curbuf->b_op_start = orig_start;
	    curbuf->b_op_end = orig_end;
	}
    }
#وگرنه
    emsg(_(e_eval_feature_not_available));
#endif
}

/*
 * Calculate start/end virtual columns for operating in block mode.
 */
    ایستا تهی
get_op_vcol(
    oparg_T	*oap,
    colnr_T	redo_VIsual_vcol,
    صحیح		initial)    // when TRUE adjust position for 'selectmode'
{
    colnr_T	    start, end;

    اگر (VIsual_mode != Ctrl_V
	    || (!initial && oap->end.col < curwin->w_width))
	بازگشت;

    oap->block_mode = TRUE;

    // prevent from moving onto a trail byte
    اگر (has_mbyte)
	mb_adjustpos(curwin->w_buffer, &oap->end);

    getvvcol(curwin, &(oap->start), &oap->start_vcol, پوچ, &oap->end_vcol, 0);

    اگر (!redo_VIsual_busy)
    {
	getvvcol(curwin, &(oap->end), &start, پوچ, &end, 0);

	اگر (start < oap->start_vcol)
	    oap->start_vcol = start;
	اگر (end > oap->end_vcol)
	{
	    اگر (initial && *p_sel == 'e' && start >= 1
				    && start - 1 >= oap->end_vcol)
		oap->end_vcol = start - 1;
	    وگرنه
		oap->end_vcol = end;
	}
    }

    // if '$' was used, get oap->end_vcol from longest line
    اگر (curwin->w_curswant == MAXCOL)
    {
	curwin->w_cursor.col = MAXCOL;
	oap->end_vcol = 0;
	برای (curwin->w_cursor.lnum = oap->start.lnum;
		curwin->w_cursor.lnum <= oap->end.lnum;
					++curwin->w_cursor.lnum)
	{
	    getvvcol(curwin, &curwin->w_cursor, پوچ, پوچ, &end, 0);
	    اگر (end > oap->end_vcol)
		oap->end_vcol = end;
	}
    }
    وگرنه اگر (redo_VIsual_busy)
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
تعریفنوع ساختار {
    صحیح		rv_mode;	// 'v', 'V', or Ctrl-V
    linenr_T	rv_سطر_شمار;	// number of lines
    colnr_T	rv_vcol;	// number of cols or end column
    long	rv_شمار;	// count for Visual operator
    صحیح		rv_arg;		// extra argument
} redo_VIsual_T;

    ایستا صحیح
is_ex_cmdchar(cmdarg_T *cap)
{
    بازگشت cap->cmdchar == ':'
	|| cap->cmdchar == K_COMMAND
	|| cap->cmdchar == K_SCRIPT_COMMAND;
}

/*
 * Handle an operator after Visual mode or when the movement is finished.
 * "gui_yank" is true when yanking text for the clipboard.
 */
    تهی
do_pending_operator(cmdarg_T *cap, صحیح old_col, صحیح gui_yank)
{
    oparg_T	*oap = cap->oap;
    pos_T	old_cursor;
    صحیح		empty_region_error;
    صحیح		restart_edit_save;
#ifdef FEAT_LINEBREAK
    صحیح		lbr_saved = curwin->w_p_lbr;
#endif

    // The visual area is remembered for redo
    ایستا redo_VIsual_T   redo_VIsual = {NUL, 0, 0, 0,0};

    صحیح		    include_سطر_break = FALSE;

#اگر defined(FEAT_CLIPBOARD)
    // Yank the visual area into the GUI selection register before we operate
    // on it and lose it forever.
    // Don't do it if a specific register was specified, so that ""x"*P works.
    // This could call do_pending_operator() recursively, but that's OK
    // because gui_yank will be TRUE for the nested call.
    اگر ((clip_star.available || clip_plus.available)
	    && oap->op_type != OP_NOP
	    && !gui_yank
	    && VIsual_active
	    && !redo_VIsual_busy
	    && oap->regname == 0)
	clip_auto_select();
#endif
    old_cursor = curwin->w_cursor;

    // If an operation is pending, handle it...
    اگر ((finish_op || VIsual_active) && oap->op_type != OP_NOP)
    {
	// Yank can be redone when 'y' is in 'cpoptions', but not when yanking
	// for the clipboard.
	صحیح	redo_yank = vim_strchr(p_cpo, CPO_YANK) != پوچ && !gui_yank;

#ifdef FEAT_LINEBREAK
	// Avoid a problem with unwanted linebreaks in block mode.
	(تهی)reset_lbr();
#endif
	oap->is_VIsual = VIsual_active;
	اگر (oap->motion_force == 'V')
	    oap->motion_type = MLINE;
	وگرنه اگر (oap->motion_force == 'v')
	{
	    // If the motion was linewise, "inclusive" will not have been set.
	    // Use "exclusive" to be consistent.  Makes "dvj" work nice.
	    اگر (oap->motion_type == MLINE)
		oap->inclusive = FALSE;
	    // If the motion already was characterwise, toggle "inclusive"
	    وگرنه اگر (oap->motion_type == MCHAR)
		oap->inclusive = !oap->inclusive;
	    oap->motion_type = MCHAR;
	}
	وگرنه اگر (oap->motion_force == Ctrl_V)
	{
	    // Change line- or characterwise motion into Visual block mode.
	    اگر (!VIsual_active)
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
	اگر ((redo_yank || oap->op_type != OP_YANK)
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
	    اگر (cap->cmdchar == '/' || cap->cmdchar == '?') // was a search
	    {
		// If 'cpoptions' does not contain 'r', insert the search
		// pattern to really repeat the same command.
		اگر (vim_strchr(p_cpo, CPO_REDO) == پوچ)
		    AppendToRedobuffLit(cap->searchbuf, -1);
		AppendToRedobuff(NL_STR);
	    }
	    وگرنه اگر (is_ex_cmdchar(cap))
	    {
		// do_cmdline() has stored the first typed line in
		// "repeat_cmdline".  When several lines are typed repeating
		// won't be possible.
		اگر (repeat_cmdline == پوچ)
		    ResetRedobuff();
		وگرنه
		{
		    اگر (cap->cmdchar == ':')
			AppendToRedobuffLit(repeat_cmdline, -1);
		    وگرنه
			AppendToRedobuffSpec(repeat_cmdline);
		    AppendToRedobuff(NL_STR);
		    VIM_CLEAR(repeat_cmdline);
		}
	    }
	}

	اگر (redo_VIsual_busy)
	{
	    // Redo of an operation on a Visual area. Use the same size from
	    // redo_VIsual.rv_line_count and redo_VIsual.rv_vcol.
	    oap->start = curwin->w_cursor;
	    curwin->w_cursor.lnum += redo_VIsual.rv_سطر_شمار - 1;
	    اگر (curwin->w_cursor.lnum > curbuf->b_ml.ml_سطر_شمار)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_سطر_شمار;
	    VIsual_mode = redo_VIsual.rv_mode;
	    اگر (redo_VIsual.rv_vcol == MAXCOL || VIsual_mode == 'v')
	    {
		اگر (VIsual_mode == 'v')
		{
		    اگر (redo_VIsual.rv_سطر_شمار <= 1)
		    {
			validate_virtcol();
			curwin->w_curswant =
				     curwin->w_virtcol + redo_VIsual.rv_vcol - 1;
		    }
		    وگرنه
			curwin->w_curswant = redo_VIsual.rv_vcol;
		}
		وگرنه
		{
		    curwin->w_curswant = MAXCOL;
		}
		coladvance(curwin->w_curswant);
	    }
	    cap->count0 = redo_VIsual.rv_شمار;
	    اگر (redo_VIsual.rv_شمار != 0)
		cap->count1 = redo_VIsual.rv_شمار;
	    وگرنه
		cap->count1 = 1;
	}
	وگرنه اگر (VIsual_active)
	{
	    اگر (!gui_yank)
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
	    اگر (VIsual_select && VIsual_mode == 'V'
					    && cap->oap->op_type != OP_DELETE)
	    {
		اگر (LT_POS(VIsual, curwin->w_cursor))
		{
		    VIsual.col = 0;
		    curwin->w_cursor.col = ml_get_len(curwin->w_cursor.lnum);
		}
		وگرنه
		{
		    curwin->w_cursor.col = 0;
		    VIsual.col = ml_get_len(VIsual.lnum);
		}
		VIsual_mode = 'v';
	    }
	    // If 'selection' is "exclusive", backup one character for
	    // charwise selections.
	    وگرنه اگر (VIsual_mode == 'v')
		include_سطر_break = unadjust_for_sel();

	    oap->start = VIsual;
	    اگر (VIsual_mode == 'V')
	    {
		oap->start.col = 0;
		oap->start.coladd = 0;
	    }
	}

	// Set oap->start to the first position of the operated text, oap->end
	// to the end of the operated text.  w_cursor is equal to oap->start.
	اگر (LT_POS(oap->start, curwin->w_cursor))
	{
#ifdef FEAT_FOLDING
	    // Include folded lines completely.
	    اگر (!VIsual_active)
	    {
		اگر (hasFolding(oap->start.lnum, &oap->start.lnum, پوچ))
		    oap->start.col = 0;
		اگر ((curwin->w_cursor.col > 0 || oap->inclusive
						  || oap->motion_type == MLINE)
			&& hasFolding(curwin->w_cursor.lnum, پوچ,
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
	وگرنه
	{
#ifdef FEAT_FOLDING
	    // Include folded lines completely.
	    اگر (!VIsual_active && oap->motion_type == MLINE)
	    {
		اگر (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum,
									پوچ))
		    curwin->w_cursor.col = 0;
		اگر (hasFolding(oap->start.lnum, پوچ, &oap->start.lnum))
		    oap->start.col = ml_get_len(oap->start.lnum);
	    }
#endif
	    oap->end = oap->start;
	    oap->start = curwin->w_cursor;
	}

	// Just in case lines were deleted that make the position invalid.
	check_pos(curwin->w_buffer, &oap->end);
	oap->سطر_شمار = oap->end.lnum - oap->start.lnum + 1;

	// Set "virtual_op" before resetting VIsual_active.
	virtual_op = virtual_active();

	اگر (VIsual_active || redo_VIsual_busy)
	{
	    get_op_vcol(oap, redo_VIsual.rv_vcol, TRUE);

	    اگر (!redo_VIsual_busy && !gui_yank)
	    {
		// Prepare to reselect and redo Visual: this is based on the
		// size of the Visual text
		resel_VIsual_mode = VIsual_mode;
		اگر (curwin->w_curswant == MAXCOL)
		    resel_VIsual_vcol = MAXCOL;
		وگرنه
		{
		    اگر (VIsual_mode != Ctrl_V)
			getvvcol(curwin, &(oap->end),
						پوچ, پوچ, &oap->end_vcol, 0);
		    اگر (VIsual_mode == Ctrl_V || oap->سطر_شمار <= 1)
		    {
			اگر (VIsual_mode != Ctrl_V)
			    getvvcol(curwin, &(oap->start),
					      &oap->start_vcol, پوچ, پوچ, 0);
			resel_VIsual_vcol = oap->end_vcol - oap->start_vcol + 1;
		    }
		    وگرنه
			resel_VIsual_vcol = oap->end_vcol;
		}
		resel_VIsual_سطر_شمار = oap->سطر_شمار;
	    }

	    // can't redo yank (unless 'y' is in 'cpoptions') and ":"
	    اگر ((redo_yank || oap->op_type != OP_YANK)
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
		اگر (cap->cmdchar == 'g' && (cap->nchar == 'n'
							|| cap->nchar == 'N'))
		    prep_redo(oap->regname, cap->count0,
			    get_op_char(oap->op_type),
			    get_extra_op_char(oap->op_type),
			    oap->motion_force, cap->cmdchar, cap->nchar);
		وگرنه اگر (!is_ex_cmdchar(cap))
		{
		    صحیح opchar = get_op_char(oap->op_type);
		    صحیح extra_opchar = get_extra_op_char(oap->op_type);
		    صحیح nchar = oap->op_type == OP_REPLACE ? cap->nchar : NUL;

		    // reverse what nv_replace() did
		    اگر (nchar == REPLACE_CR_NCHAR)
			nchar = CAR;
		    وگرنه اگر (nchar == REPLACE_NL_NCHAR)
			nchar = NL;

		    اگر (opchar == 'g' && extra_opchar == '@')
			// also repeat the count for 'operatorfunc'
			prep_redo_num2(oap->regname, 0L, NUL, 'v',
				     cap->count0, opchar, extra_opchar, nchar);
		    وگرنه
			prep_redo(oap->regname, 0L, NUL, 'v',
						  opchar, extra_opchar, nchar);
		}
		اگر (!redo_VIsual_busy)
		{
		    redo_VIsual.rv_mode = resel_VIsual_mode;
		    redo_VIsual.rv_vcol = resel_VIsual_vcol;
		    redo_VIsual.rv_سطر_شمار = resel_VIsual_سطر_شمار;
		    redo_VIsual.rv_شمار = cap->count0;
		    redo_VIsual.rv_arg = cap->arg;
		}
	    }

	    // oap->inclusive defaults to TRUE.
	    // If oap->end is on a NUL (empty line) oap->inclusive becomes
	    // FALSE.  This makes "d}P" and "v}dP" work the same.
	    اگر (oap->motion_force == NUL || oap->motion_type == MLINE)
		oap->inclusive = TRUE;
	    اگر (VIsual_mode == 'V')
		oap->motion_type = MLINE;
	    وگرنه
	    {
		oap->motion_type = MCHAR;
		اگر (VIsual_mode != Ctrl_V && *ml_get_pos(&(oap->end)) == NUL
			&& (include_سطر_break || !virtual_op))
		{
		    oap->inclusive = FALSE;
		    // Try to include the newline, unless it's an operator
		    // that works on lines only.
		    اگر (*p_sel != 'o'
			    && !op_on_سطرها(oap->op_type)
			    && oap->end.lnum < curbuf->b_ml.ml_سطر_شمار)
		    {
			++oap->end.lnum;
			oap->end.col = 0;
			oap->end.coladd = 0;
			++oap->سطر_شمار;
		    }
		}
	    }

	    redo_VIsual_busy = FALSE;

	    // Switch Visual off now, so screen updating does
	    // not show inverted text when the screen is redrawn.
	    // With OP_YANK and sometimes with OP_COLON and OP_FILTER there is
	    // no screen redraw, so it is done here to remove the inverted
	    // part.
	    اگر (!gui_yank)
	    {
		VIsual_active = FALSE;
		setmouse();
		mouse_dragging = 0;
		may_clear_cmdline();
		اگر ((oap->op_type == OP_YANK
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
	اگر (has_mbyte && oap->inclusive)
	{
	    صحیح		l;

	    l = (*mb_ptr2len)(ml_get_pos(&oap->end));
	    اگر (l > 1)
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
				&& vim_strchr(p_cpo, CPO_EMPTYREGION) != پوچ);

	// Force a redraw when operating on an empty Visual region, when
	// 'modifiable is off or creating a fold.
	اگر (oap->is_VIsual && (oap->empty || !curbuf->b_p_ma
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
	اگر (	   oap->motion_type == MCHAR
		&& oap->inclusive == FALSE
		&& !(cap->retval & CA_NO_ADJ_OP_END)
		&& oap->end.col == 0
		&& (!oap->is_VIsual || *p_sel == 'o')
		&& !oap->block_mode
		&& oap->سطر_شمار > 1)
	{
	    oap->end_adjusted = TRUE;	    // remember that we did this
	    --oap->سطر_شمار;
	    --oap->end.lnum;
	    اگر (inindent(0))
		oap->motion_type = MLINE;
	    وگرنه
	    {
		oap->end.col = ml_get_len(oap->end.lnum);
		اگر (oap->end.col)
		{
		    --oap->end.col;
		    oap->inclusive = TRUE;
		}
	    }
	}
	وگرنه
	    oap->end_adjusted = FALSE;

	انتخاب (oap->op_type)
	{
	حالت OP_LSHIFT:
	حالت OP_RSHIFT:
	    op_shift(oap, TRUE, oap->is_VIsual ? (صحیح)cap->count1 : 1);
	    auto_format(FALSE, TRUE);
	    بشکن;

	حالت OP_JOIN_NS:
	حالت OP_JOIN:
	    اگر (oap->سطر_شمار < 2)
		oap->سطر_شمار = 2;
	    اگر (curwin->w_cursor.lnum + oap->سطر_شمار - 1 >
						   curbuf->b_ml.ml_سطر_شمار)
		beep_flush();
	    وگرنه
	    {
		(تهی)do_join(oap->سطر_شمار, oap->op_type == OP_JOIN,
							    TRUE, TRUE, TRUE);
		auto_format(FALSE, TRUE);
	    }
	    بشکن;

	حالت OP_DELETE:
	    VIsual_reselect = FALSE;	    // don't reselect now
	    اگر (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    وگرنه
	    {
		(تهی)op_delete(oap);
		// save cursor line for undo if it wasn't saved yet
		اگر (oap->motion_type == MLINE && has_format_option(FO_AUTO)
						      && u_save_cursor() == OK)
		    auto_format(FALSE, TRUE);
	    }
	    بشکن;

	حالت OP_YANK:
	    اگر (empty_region_error)
	    {
		اگر (!gui_yank)
		{
		    vim_beep(BO_OPER);
		    CancelRedo();
		}
	    }
	    وگرنه
	    {
#ifdef FEAT_LINEBREAK
		restore_lbr(lbr_saved);
#endif
		oap->excl_tr_ws = cap->cmdchar == 'z';
		(تهی)op_yank(oap, FALSE, !gui_yank);
	    }
	    check_cursor_col();
	    بشکن;

	حالت OP_CHANGE:
	    VIsual_reselect = FALSE;	    // don't reselect now
	    اگر (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    وگرنه
	    {
		// This is a new edit command, not a restart.  Need to
		// remember it to make 'insertmode' work with mappings for
		// Visual mode.  But do this only once and not when typed and
		// 'insertmode' isn't set.
		اگر (p_im || !کلید_Typed)
		    restart_edit_save = restart_edit;
		وگرنه
		    restart_edit_save = 0;
		restart_edit = 0;
#ifdef FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#endif
		// trigger TextChangedI
		curbuf->b_last_changedtick_i = CHANGEDTICK(curbuf);

		اگر (op_change(oap))	// will call edit()
		    cap->retval |= CA_COMMAND_BUSY;
		اگر (restart_edit == 0)
		    restart_edit = restart_edit_save;
	    }
	    بشکن;

	حالت OP_FILTER:
	    اگر (vim_strchr(p_cpo, CPO_FILTER) != پوچ)
		AppendToRedobuff((char_u *)"!\r");  // use any last used !cmd
	    وگرنه
		bangredo = TRUE;    // do_bang() will put cmd in redo buffer
	    // FALLTHROUGH

	حالت OP_INDENT:
	حالت OP_COLON:

	    // If 'equalprg' is empty, do the indenting internally.
	    اگر (oap->op_type == OP_INDENT && *get_equalprg() == NUL)
	    {
		اگر (curbuf->b_p_lisp)
		{
#ifdef FEAT_EVAL
		    اگر (use_indentexpr_for_lisp())
			op_reindent(oap, get_عبارت_indent);
		    وگرنه
#endif
			op_reindent(oap, get_lisp_indent);
		    بشکن;
		}
		op_reindent(oap,
#ifdef FEAT_EVAL
			*curbuf->b_p_inde != NUL ? get_عبارت_indent :
#endif
			    get_c_indent);
		بشکن;
	    }

	    op_colon(oap);
	    بشکن;

	حالت OP_TILDE:
	حالت OP_UPPER:
	حالت OP_LOWER:
	حالت OP_ROT13:
	    اگر (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    وگرنه
		op_tilde(oap);
	    check_cursor_col();
	    بشکن;

	حالت OP_FORMAT:
#اگر defined(FEAT_EVAL)
	    اگر (*curbuf->b_p_fex != NUL)
		op_formatexpr(oap);	// use expression
	    وگرنه
#endif
	    {
		اگر (*p_fp != NUL || *curbuf->b_p_fp != NUL)
		    op_colon(oap);		// use external command
		وگرنه
		    op_format(oap, FALSE);	// use internal function
	    }
	    بشکن;
	حالت OP_FORMAT2:
	    op_format(oap, TRUE);	// use internal function
	    بشکن;

	حالت OP_FUNCTION:
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
		بشکن;
	    }

	حالت OP_درج:
	حالت OP_APPEND:
	    VIsual_reselect = FALSE;	// don't reselect now
	    اگر (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    وگرنه
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

		op_درج(oap, cap->count1);
#ifdef FEAT_LINEBREAK
		// Reset linebreak, so that formatting works correctly.
		(تهی)reset_lbr();
#endif

		// TODO: when inserting in several lines, should format all
		// the lines.
		auto_format(FALSE, TRUE);

		اگر (restart_edit == 0)
		    restart_edit = restart_edit_save;
		وگرنه
		    cap->retval |= CA_COMMAND_BUSY;
	    }
	    بشکن;

	حالت OP_REPLACE:
	    VIsual_reselect = FALSE;	// don't reselect now
	    اگر (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    وگرنه
	    {
#ifdef FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#endif
		op_replace(oap, cap->nchar);
	    }
	    بشکن;

#ifdef FEAT_FOLDING
	حالت OP_FOLD:
	    VIsual_reselect = FALSE;	// don't reselect now
	    foldCreate(oap->start.lnum, oap->end.lnum);
	    بشکن;

	حالت OP_FOLDOPEN:
	حالت OP_FOLDOPENREC:
	حالت OP_FOLDCLOSE:
	حالت OP_FOLDCLOSEREC:
	    VIsual_reselect = FALSE;	// don't reselect now
	    opFoldRange(oap->start.lnum, oap->end.lnum,
		    oap->op_type == OP_FOLDOPEN
					    || oap->op_type == OP_FOLDOPENREC,
		    oap->op_type == OP_FOLDOPENREC
					  || oap->op_type == OP_FOLDCLOSEREC,
					  oap->is_VIsual);
	    بشکن;

	حالت OP_FOLDDEL:
	حالت OP_FOLDDELREC:
	    VIsual_reselect = FALSE;	// don't reselect now
	    deleteFold(oap->start.lnum, oap->end.lnum,
			       oap->op_type == OP_FOLDDELREC, oap->is_VIsual);
	    بشکن;
#endif
	حالت OP_NR_ADD:
	حالت OP_NR_SUB:
	    اگر (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    وگرنه
	    {
		VIsual_active = TRUE;
#ifdef FEAT_LINEBREAK
		restore_lbr(lbr_saved);
#endif
		op_addsub(oap, cap->count1, redo_VIsual.rv_arg);
		VIsual_active = FALSE;
	    }
	    check_cursor_col();
	    بشکن;
	پیشفرض:
	    clearopbeep(oap);
	}
	virtual_op = MAYBE;
	اگر (!gui_yank)
	{
	    // if 'sol' not set, go back to old column for some commands
	    اگر (!p_sol && oap->motion_type == MLINE && !oap->end_adjusted
		    && (oap->op_type == OP_LSHIFT || oap->op_type == OP_RSHIFT
						|| oap->op_type == OP_DELETE))
	    {
#ifdef FEAT_LINEBREAK
		(تهی)reset_lbr();
#endif
		coladvance(curwin->w_curswant = old_col);
	    }
	}
	وگرنه
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
    ایستا تهی
pbyte(pos_T lp, صحیح c)
{
    char_u *p = ml_get_buf(curbuf, lp.lnum, TRUE);
    صحیح	len = curbuf->b_ml.ml_سطر_len;

    // safety check
    اگر (lp.col >= len)
	lp.col = (len > 1 ? len - 2 : 0);
    *(p + lp.col) = c;
}
