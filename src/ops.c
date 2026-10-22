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

#অন্তর্ভাব "vim.h"

স্থির শূন্য shift_block(oparg_T *oap, পূর্ণসংখ্যা amount);
স্থির শূন্য	mb_adjust_opend(oparg_T *oap);
স্থির পূর্ণসংখ্যা	do_addsub(পূর্ণসংখ্যা op_type, pos_T *pos, পূর্ণসংখ্যা দৈর্ঘ্য, linenr_T Prenum1);
স্থির শূন্য	pbyte(pos_T lp, পূর্ণসংখ্যা c);
#define PBYTE(lp, c) pbyte(lp, c)


// Flags for third item in "opchars".
#define OPF_সারিগুলি  1	// operator always works on lines
#define OPF_CHANGE 2	// operator changes text

/*
 * The names of operators.
 * IMPORTANT: Index must correspond with defines in vim.h!!!
 * The third field holds OPF_ flags.
 */
স্থির ধ্রুবক অক্ষর opchars[][3] =
{
    {NUL, NUL, 0},			// OP_NOP
    {'d', NUL, OPF_CHANGE},		// OP_DELETE
    {'y', NUL, 0},			// OP_YANK
    {'c', NUL, OPF_CHANGE},		// OP_CHANGE
    {'<', NUL, OPF_সারিগুলি | OPF_CHANGE},	// OP_LSHIFT
    {'>', NUL, OPF_সারিগুলি | OPF_CHANGE},	// OP_RSHIFT
    {'!', NUL, OPF_সারিগুলি | OPF_CHANGE},	// OP_FILTER
    {'g', '~', OPF_CHANGE},		// OP_TILDE
    {'=', NUL, OPF_সারিগুলি | OPF_CHANGE},	// OP_INDENT
    {'g', 'q', OPF_সারিগুলি | OPF_CHANGE},	// OP_FORMAT
    {':', NUL, OPF_সারিগুলি},		// OP_COLON
    {'g', 'U', OPF_CHANGE},		// OP_UPPER
    {'g', 'u', OPF_CHANGE},		// OP_LOWER
    {'J', NUL, OPF_সারিগুলি | OPF_CHANGE},	// DO_JOIN
    {'g', 'J', OPF_সারিগুলি | OPF_CHANGE},	// DO_JOIN_NS
    {'g', '?', OPF_CHANGE},		// OP_ROT13
    {'r', NUL, OPF_CHANGE},		// OP_REPLACE
    {'I', NUL, OPF_CHANGE},		// OP_INSERT
    {'A', NUL, OPF_CHANGE},		// OP_APPEND
    {'z', 'f', OPF_সারিগুলি},		// OP_FOLD
    {'z', 'o', OPF_সারিগুলি},		// OP_FOLDOPEN
    {'z', 'O', OPF_সারিগুলি},		// OP_FOLDOPENREC
    {'z', 'c', OPF_সারিগুলি},		// OP_FOLDCLOSE
    {'z', 'C', OPF_সারিগুলি},		// OP_FOLDCLOSEREC
    {'z', 'd', OPF_সারিগুলি},		// OP_FOLDDEL
    {'z', 'D', OPF_সারিগুলি},		// OP_FOLDDELREC
    {'g', 'w', OPF_সারিগুলি | OPF_CHANGE},	// OP_FORMAT2
    {'g', '@', OPF_CHANGE},		// OP_FUNCTION
    {Ctrl_A, NUL, OPF_CHANGE},		// OP_NR_ADD
    {Ctrl_X, NUL, OPF_CHANGE},		// OP_NR_SUB
};

/*
 * Translate a command name into an operator type.
 * Must only be called with a valid operator name!
 */
    পূর্ণসংখ্যা
get_op_type(পূর্ণসংখ্যা char1, পূর্ণসংখ্যা char2)
{
    পূর্ণসংখ্যা		i;

    যদি (char1 == 'r')		// ignore second character
	ফেরত OP_REPLACE;
    যদি (char1 == '~')		// when tilde is an operator
	ফেরত OP_TILDE;
    যদি (char1 == 'g' && char2 == Ctrl_A)	// add
	ফেরত OP_NR_ADD;
    যদি (char1 == 'g' && char2 == Ctrl_X)	// subtract
	ফেরত OP_NR_SUB;
    যদি (char1 == 'z' && char2 == 'y')	// OP_YANK
	ফেরত OP_YANK;
    জন্য (i = 0; ; ++i)
    {
	যদি (opchars[i][0] == char1 && opchars[i][1] == char2)
	    বিরতি;
	যদি (i == (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(opchars) - 1)
	{
	    internal_error("get_op_type()");
	    বিরতি;
	}
    }
    ফেরত i;
}

/*
 * Return TRUE if operator "op" always works on whole lines.
 */
    স্থির পূর্ণসংখ্যা
op_on_সারিগুলি(পূর্ণসংখ্যা op)
{
    ফেরত opchars[op][2] & OPF_সারিগুলি;
}

#যদি defined(FEAT_JOB_CHANNEL)
/*
 * Return TRUE if operator "op" changes text.
 */
    পূর্ণসংখ্যা
op_is_change(পূর্ণসংখ্যা op)
{
    ফেরত opchars[op][2] & OPF_CHANGE;
}
#endif

/*
 * Get first operator command character.
 * Returns 'g' or 'z' if there is another command character.
 */
    পূর্ণসংখ্যা
get_op_char(পূর্ণসংখ্যা optype)
{
    ফেরত opchars[optype][0];
}

/*
 * Get second operator command character.
 */
    পূর্ণসংখ্যা
get_extra_op_char(পূর্ণসংখ্যা optype)
{
    ফেরত opchars[optype][1];
}

/*
 * op_shift - handle a shift operation
 */
    শূন্য
op_shift(oparg_T *oap, পূর্ণসংখ্যা curs_শীর্ষ, পূর্ণসংখ্যা amount)
{
    long	    i;
    পূর্ণসংখ্যা		    first_char;
    পূর্ণসংখ্যা		    block_col = 0;

    যদি (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	ফেরত;

    যদি (oap->block_mode)
	block_col = curwin->w_cursor.col;

    জন্য (i = oap->সারি_গুনতি; --i >= 0; )
    {
	first_char = *ml_get_curline();
	যদি (first_char == NUL)				// empty line
	    curwin->w_cursor.col = 0;
	অন্যথা যদি (oap->block_mode)
	    shift_block(oap, amount);
	অন্যথা
	    // Move the line right if it doesn't start with '#', 'smartindent'
	    // isn't set or 'cindent' isn't set or '#' isn't in 'cino'.
	    যদি (first_char != '#' || !preprocs_বাম())
		shift_সারি(oap->op_type == OP_LSHIFT, p_sr, amount, FALSE);
	++curwin->w_cursor.lnum;
    }

    changed_সারিগুলি(oap->start.lnum, 0, oap->end.lnum + 1, 0L);
    যদি (oap->block_mode)
    {
	curwin->w_cursor.lnum = oap->start.lnum;
	curwin->w_cursor.col = block_col;
    }
    অন্যথা যদি (curs_শীর্ষ)	    // put cursor on first line, for ">>"
    {
	curwin->w_cursor.lnum = oap->start.lnum;
	beginline(BL_SOL | BL_FIX);   // shift_line() may have set cursor.col
    }
    অন্যথা
	--curwin->w_cursor.lnum;	// put cursor on last line, for ":>"

#ifdef FEAT_FOLDING
    // The cursor line is not in a closed fold
    foldOpenCursor();
#endif


    যদি (oap->সারি_গুনতি > p_report)
    {
	অক্ষর	    *op;
	অক্ষর	    *বার্তা_সারি_single;
	অক্ষর	    *বার্তা_সারি_plural;

	যদি (oap->op_type == OP_RSHIFT)
	    op = ">";
	অন্যথা
	    op = "<";
	বার্তা_সারি_single = NGETTEXT("%ld line %sed %d time",
					     "%ld line %sed %d times", amount);
	বার্তা_সারি_plural = NGETTEXT("%ld lines %sed %d time",
					    "%ld lines %sed %d times", amount);
	vim_snprintf((অক্ষর *)IObuff, IOSIZE,
		NGETTEXT(বার্তা_সারি_single, বার্তা_সারি_plural, oap->সারি_গুনতি),
		oap->সারি_গুনতি, op, amount);
	বার্তা_attr_keep((অক্ষর *)IObuff, 0, TRUE);
    }

    যদি ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end.lnum = oap->end.lnum;
	curbuf->b_op_end.col = ml_get_len(oap->end.lnum);
	যদি (curbuf->b_op_end.col > 0)
	    --curbuf->b_op_end.col;
    }
}

#ifdef FEAT_VARTABS
/*
 * Return the tabstop width at the index of the variable tabstop array.  If an
 * index greater than the length of the array is given, the last tabstop width
 * in the array is returned.
 */
    স্থির পূর্ণসংখ্যা
get_vts(পূর্ণসংখ্যা *vts_array, পূর্ণসংখ্যা সূচক)
{
    পূর্ণসংখ্যা	ts;

    যদি (সূচক < 1)
	ts = 0;
    অন্যথা যদি (সূচক <= vts_array[0])
	ts = vts_array[সূচক];
    অন্যথা
	ts = vts_array[vts_array[0]];

    ফেরত ts;
}

/*
 * Return the sum of all the tabstops through the index-th.
 */
    স্থির পূর্ণসংখ্যা
get_vts_যোগ(পূর্ণসংখ্যা *vts_array, পূর্ণসংখ্যা সূচক)
{
    পূর্ণসংখ্যা	যোগ_2 = 0;
    পূর্ণসংখ্যা	i;

    // Perform the summation for indices within the actual array.
    জন্য (i = 1; i <= সূচক && i <= vts_array[0]; i++)
	যোগ_2 += vts_array[i];

    // Add tabstops whose indices exceed the actual array.
    যদি (i <= সূচক)
	যোগ_2 += vts_array[vts_array[0]] * (সূচক - vts_array[0]);

    ফেরত যোগ_2;
}
#endif

    স্থির vimlong_T
get_new_sw_indent(
    পূর্ণসংখ্যা		বাম,		// TRUE if shift is to the left
    পূর্ণসংখ্যা		round,		// TRUE if new indent is to be to a tabstop
    vimlong_T	amount,		// Number of shifts
    vimlong_T	sw_val)
{
    vimlong_T	গুনতি = get_indent();
    vimlong_T	i, j;

    যদি (round)			// round off indent
    {
	i = trim_to_int(গুনতি) / sw_val;	// number of 'shiftwidth' rounded down
	j = trim_to_int(গুনতি) % sw_val;	// extra spaces
	যদি (j && বাম)		// first remove extra spaces
	    --amount;
	যদি (বাম)
	{
	    i -= amount;
	    যদি (i < 0)
		i = 0;
	}
	অন্যথা
	    i += amount;
	গুনতি = i * sw_val;
    }
    অন্যথা			// original vi indent
    {
	যদি (বাম)
	{
	    গুনতি -= sw_val * amount;
	    যদি (গুনতি < 0)
		গুনতি = 0;
	}
	অন্যথা
	    গুনতি += sw_val * amount;
    }

    ফেরত গুনতি;
}

#ifdef FEAT_VARTABS
    স্থির vimlong_T
get_new_vts_indent(
    পূর্ণসংখ্যা	বাম,			// TRUE if shift is to the left
    পূর্ণসংখ্যা	round,			// TRUE if new indent is to be to a tabstop
    পূর্ণসংখ্যা	amount,			// Number of shifts
    পূর্ণসংখ্যা	*vts_array)
{
    vimlong_T	indent = get_indent();
    পূর্ণসংখ্যা		vtsi = 0;
    পূর্ণসংখ্যা		vts_indent = 0;
    পূর্ণসংখ্যা		ts = 0;		// Silence uninitialized variable warning.
    পূর্ণসংখ্যা		offset;		// Extra indent spaces to the right of the
				// tabstop

    // Find the tabstop at or to the left of the current indent.
    যখন (vts_indent <= indent)
    {
	vtsi++;
	ts = get_vts(vts_array, vtsi);
	vts_indent += ts;
    }
    vts_indent -= ts;
    vtsi--;

    offset = indent - vts_indent;

    যদি (round)
    {
	যদি (বাম)
	{
	    যদি (offset == 0)
		indent = get_vts_যোগ(vts_array, vtsi - amount);
	    অন্যথা
		indent = get_vts_যোগ(vts_array, vtsi - (amount - 1));
	}
	অন্যথা
	    indent = get_vts_যোগ(vts_array, vtsi + amount);
    }
    অন্যথা
    {
	যদি (বাম)
	{
	    যদি (amount > vtsi)
		indent = 0;
	    অন্যথা
		indent = get_vts_যোগ(vts_array, vtsi - amount) + offset;
	}
	অন্যথা
	    indent = get_vts_যোগ(vts_array, vtsi + amount) + offset;
    }

    ফেরত indent;
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
    শূন্য
shift_সারি(
    পূর্ণসংখ্যা	বাম,			// TRUE if shift is to the left
    পূর্ণসংখ্যা	round,			// TRUE if new indent is to be to a tabstop
    পূর্ণসংখ্যা	amount,			// Number of shifts
    পূর্ণসংখ্যা	call_changed_bytes)	// call changed_bytes()
{
    vimlong_T	গুনতি;
    long	sw_val = curbuf->b_p_sw;
    long	ts_val = curbuf->b_p_ts;
#ifdef FEAT_VARTABS
    পূর্ণসংখ্যা		*vts_array = curbuf->b_p_vts_array;
#endif

    যদি (sw_val != 0)
	// 'shiftwidth' is not zero; use it as the shift size.
	গুনতি = get_new_sw_indent(বাম, round, amount, sw_val);
    অন্যথা
#ifdef FEAT_VARTABS
	যদি ((vts_array == ফাঁকা) || (vts_array[0] == 0))
#endif
    {
	// 'shiftwidth is zero and 'vartabstop' is empty; use 'tabstop' as the
	// shift size.
	গুনতি = get_new_sw_indent(বাম, round, amount, ts_val);
    }
#ifdef FEAT_VARTABS
    অন্যথা
    {
	// 'shiftwidth is zero and 'vartabstop' is defined; use 'vartabstop'
	// to determine the new indent.
	গুনতি = get_new_vts_indent(বাম, round, amount, vts_array);
    }
#endif

    // Set new indent
    যদি (State & VREPLACE_FLAG)
	change_indent(INDENT_SET, trim_to_int(গুনতি), FALSE, NUL, call_changed_bytes);
    অন্যথা
	(শূন্য)set_indent(trim_to_int(গুনতি), call_changed_bytes ? SIN_CHANGED : 0);
}

/*
 * Shift one line of the current block one shiftwidth right or left.
 * Leaves cursor on first character in block.
 */
    স্থির শূন্য
shift_block(oparg_T *oap, পূর্ণসংখ্যা amount)
{
    পূর্ণসংখ্যা			বাম = (oap->op_type == OP_LSHIFT);
    পূর্ণসংখ্যা			oldstate = State;
    পূর্ণসংখ্যা			যোগ;
    char_u		*newp, *oldp;
    size_t		newlen, oldlen;
    পূর্ণসংখ্যা			oldcol = curwin->w_cursor.col;
    পূর্ণসংখ্যা			sw_val = (পূর্ণসংখ্যা)get_sw_মান_indent(curbuf, বাম);
    পূর্ণসংখ্যা			ts_val = (পূর্ণসংখ্যা)curbuf->b_p_ts;
    কাঠামো block_def	bd;
    পূর্ণসংখ্যা			incr;
    colnr_T		ws_vcol;
    পূর্ণসংখ্যা			added;
    size_t		new_সারি_len;	// the length of the line after the
					// block shift
#ifdef FEAT_RIGHTLEFT
    পূর্ণসংখ্যা			old_p_ri = p_ri;

    p_ri = 0;			// don't want revins in indent
#endif

    State = MODE_ঢোকাও;	// don't want MODE_REPLACE for State
    block_prep(oap, &bd, curwin->w_cursor.lnum, TRUE);
    যদি (bd.is_short)
	ফেরত;

    // total is number of screen columns to be inserted/removed
    যোগ = (পূর্ণসংখ্যা)((unsigned)amount * (unsigned)sw_val);
    যদি ((যোগ / sw_val) != amount)
	ফেরত; // multiplication overflow

    oldp = ml_get_curline();
    oldlen = ml_get_curline_len();

    যদি (!বাম)
    {
	পূর্ণসংখ্যা		tabs = 0, spaces = 0;
	chartabsize_T	cts;

	/*
	 *  1. Get start vcol
	 *  2. Total ws vcols
	 *  3. Divvy into TABs & spp
	 *  4. Construct new string
	 */
	যোগ += bd.pre_whitesp; // all virtual WS up to & incl a split TAB
	ws_vcol = bd.start_vcol - bd.pre_whitesp;
	যদি (bd.startspaces)
	{
	    যদি (has_mbyte)
	    {
		যদি ((*mb_ptr2len)(bd.textstart) == 1)
		    ++bd.textstart;
		অন্যথা
		{
		    ws_vcol = 0;
		    bd.startspaces = 0;
		}
	    }
	    অন্যথা
		++bd.textstart;
	}

	// TODO: is passing bd.textstart for start of the line OK?
	init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum,
				   bd.start_vcol, bd.textstart, bd.textstart);
	জন্য ( ; VIM_ISWHITE(*cts.cts_ptr); )
	{
	    incr = lbr_chartabsize_adv(&cts);
	    যোগ += incr;
	    cts.cts_vcol += incr;
	}
	bd.textstart = cts.cts_ptr;
	bd.start_vcol = cts.cts_vcol;
	clear_chartabsize_arg(&cts);

	// OK, now total=all the VWS reqd, and textstart points at the 1st
	// non-ws char in the block.
#ifdef FEAT_VARTABS
	যদি (!curbuf->b_p_et)
	    tabstop_fromto(ws_vcol, ws_vcol + যোগ,
				ts_val, curbuf->b_p_vts_array, &tabs, &spaces);
	অন্যথা
	    spaces = যোগ;
#অন্যথা
	যদি (!curbuf->b_p_et)
	    tabs = ((ws_vcol % ts_val) + যোগ) / ts_val; // number of tabs
	যদি (tabs > 0)
	    spaces = ((ws_vcol % ts_val) + যোগ) % ts_val; // number of spp
	অন্যথা
	    spaces = যোগ;
#endif
	// if we're splitting a TAB, allow for it
	bd.textcol -= bd.pre_whitesp_c - (bd.startspaces != 0);

	new_সারি_len = bd.textcol + tabs + spaces + (oldlen - (bd.textstart - oldp));
	newp = alloc(new_সারি_len + 1);
	যদি (newp == ফাঁকা)
	    ফেরত;
	mch_memmove(newp, oldp, (size_t)bd.textcol);
	newlen = bd.textcol;
	vim_memset(newp + newlen, TAB, (size_t)tabs);
	newlen += tabs;
	vim_memset(newp + newlen, ' ', (size_t)spaces);
	STRCPY(newp + newlen + spaces, bd.textstart);
    }
    অন্যথা //বাম
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
	যদি (bd.startspaces)
	    MB_PTR_ADV(non_white);

	// The character's column is in "bd.start_vcol".
	non_white_col = bd.start_vcol;

	init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum,
				   non_white_col, bd.textstart, non_white);
	যখন (VIM_ISWHITE(*cts.cts_ptr))
	{
	    incr = lbr_chartabsize_adv(&cts);
	    cts.cts_vcol += incr;
	}
	non_white_col = cts.cts_vcol;
	non_white = cts.cts_ptr;
	clear_chartabsize_arg(&cts);

	block_space_width = non_white_col - oap->start_vcol;
	// We will shift by "total" or "block_space_width", whichever is less.
	shift_amount = (block_space_width < (size_t)যোগ
					 ? block_space_width : (size_t)যোগ);

	// The column to which we will shift the text.
	destination_col = (colnr_T)(non_white_col - shift_amount);

	// Now let's find out how much of the beginning of the line we can
	// reuse without modification.
	verbatim_copy_end = bd.textstart;
	verbatim_copy_width = bd.start_vcol;

	// If "bd.startspaces" is set, "bd.textstart" points to the character
	// preceding the block. We have to subtract its width to obtain its
	// column number.
	যদি (bd.startspaces)
	    verbatim_copy_width -= bd.start_char_vcols;
	init_chartabsize_arg(&cts, curwin, 0, verbatim_copy_width,
					     bd.textstart, verbatim_copy_end);
	যখন (cts.cts_vcol < destination_col)
	{
	    incr = lbr_chartabsize(&cts);
	    যদি (cts.cts_vcol + incr > destination_col)
		বিরতি;
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
	new_সারি_len = fixedlen + fill + (oldlen - (non_white - oldp));

	newp = alloc(new_সারি_len + 1);
	যদি (newp == ফাঁকা)
	    ফেরত;
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
    যদি (new_সারি_len >= oldlen)
	added = (পূর্ণসংখ্যা)(new_সারি_len - oldlen);
    অন্যথা
	added = 0 - (পূর্ণসংখ্যা)(oldlen - new_সারি_len);
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
    স্থির শূন্য
block_ঢোকাও(
    oparg_T		*oap,
    char_u		*s,
    size_t		slen,
    পূর্ণসংখ্যা			b_ঢোকাও,
    কাঠামো block_def	*bdp)
{
    পূর্ণসংখ্যা		ts_val;
    পূর্ণসংখ্যা		গুনতি = 0;	// extra spaces to replace a cut TAB
    পূর্ণসংখ্যা		spaces = 0;	// non-zero if cutting a TAB
    colnr_T	offset;		// pointer along new line
    colnr_T	startcol;	// column where insert starts
    char_u	*newp, *oldp;	// new, old lines
    linenr_T	lnum;		// loop var
    পূর্ণসংখ্যা		oldstate = State;

    State = MODE_ঢোকাও;	// don't want MODE_REPLACE for State

    জন্য (lnum = oap->start.lnum + 1; lnum <= oap->end.lnum; lnum++)
    {
	block_prep(oap, bdp, lnum, TRUE);
	যদি (bdp->is_short && b_ঢোকাও)
	    চালু;	// OP_INSERT, line ends before block start

	oldp = ml_get(lnum);

	যদি (b_ঢোকাও)
	{
	    ts_val = bdp->start_char_vcols;
	    spaces = bdp->startspaces;
	    যদি (spaces != 0)
		গুনতি = ts_val - 1; // we're cutting a TAB
	    offset = bdp->textcol;
	}
	অন্যথা // append
	{
	    ts_val = bdp->end_char_vcols;
	    যদি (!bdp->is_short) // spaces = padding after block
	    {
		spaces = (bdp->endspaces ? ts_val - bdp->endspaces : 0);
		যদি (spaces != 0)
		    গুনতি = ts_val - 1; // we're cutting a TAB
		offset = bdp->textcol + bdp->textlen - (spaces != 0);
	    }
	    অন্যথা // spaces = padding to block edge
	    {
		// if $ used, just append to EOL (ie spaces==0)
		যদি (!bdp->is_MAX)
		    spaces = (oap->end_vcol - bdp->end_vcol) + 1;
		গুনতি = spaces;
		offset = bdp->textcol + bdp->textlen;
	    }
	}

	যদি (has_mbyte && spaces > 0)
	    // avoid copying part of a multi-byte character
	    offset -= (*mb_শীর্ষ_off)(oldp, oldp + offset);

	যদি (spaces < 0)  // can happen when the cursor was moved
	    spaces = 0;

	// Make sure the allocated size matches what is actually copied below.
	newp = alloc(ml_get_len(lnum) + spaces + slen
		    + (spaces > 0 && !bdp->is_short ? ts_val - spaces : 0)
								  + গুনতি + 1);
	যদি (newp == ফাঁকা)
	    চালু;

	// copy up to shifted part
	mch_memmove(newp, oldp, (size_t)offset);
	oldp += offset;

	// insert pre-padding
	vim_memset(newp + offset, ' ', (size_t)spaces);
	startcol = offset + spaces;

	// copy the new text
	mch_memmove(newp + startcol, s, slen);
	offset += (পূর্ণসংখ্যা)slen;

	যদি (spaces > 0 && !bdp->is_short)
	{
	    যদি (*oldp == TAB)
	    {
		// insert post-padding
		vim_memset(newp + offset + spaces, ' ',
						    (size_t)(ts_val - spaces));
		// we're splitting a TAB, don't copy it
		oldp++;
		// We allowed for that TAB, remember this now
		গুনতি++;
	    }
	    অন্যথা
		// Not a TAB, no extra spaces
		গুনতি = spaces;
	}

	যদি (spaces > 0)
	    offset += গুনতি;
	STRCPY(newp + offset, oldp);

	ml_replace(lnum, newp, FALSE);

	যদি (b_ঢোকাও)
	    // correct any text properties
	    inserted_bytes(lnum, startcol, (পূর্ণসংখ্যা)slen);

	যদি (lnum == oap->end.lnum)
	{
	    // Set "']" mark to the end of the block instead of the end of
	    // the insert in the first line.
	    curbuf->b_op_end.lnum = oap->end.lnum;
	    curbuf->b_op_end.col = offset;
	    যদি (curbuf->b_visual.vi_end.coladd)
	    {
		curbuf->b_visual.vi_end.col += curbuf->b_visual.vi_end.coladd;
		curbuf->b_visual.vi_end.coladd = 0;
	    }
	}
    } // for all lnum

    changed_সারিগুলি(oap->start.lnum + 1, 0, oap->end.lnum + 1, 0L);

    State = oldstate;
}

/*
 * Handle a delete operation.
 *
 * Return FAIL if undo failed, OK otherwise.
 */
    পূর্ণসংখ্যা
op_delete(oparg_T *oap)
{
    পূর্ণসংখ্যা			n;
    linenr_T		lnum;
    char_u		*ptr;
    char_u		*newp, *oldp;
    কাঠামো block_def	bd;
    linenr_T		old_lcount = curbuf->b_ml.ml_সারি_গুনতি;
    পূর্ণসংখ্যা			did_yank = FALSE;

    যদি (curbuf->b_ml.ml_flags & ML_EMPTY)	    // nothing to do
	ফেরত OK;

    // Nothing to delete, return here.	Do prepare undo, for op_change().
    যদি (oap->empty)
	ফেরত u_save_cursor();

    যদি (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	ফেরত FAIL;
    }

    যদি (VIsual_select && oap->is_VIsual)
	// use register given with CTRL_R, defaults to zero
	oap->regname = VIsual_select_reg;

#ifdef HAVE_CLIPMETHOD
    adjust_clip_reg(&oap->regname);
#endif

    যদি (has_mbyte)
	mb_adjust_opend(oap);

    /*
     * Imitate the strange Vi behaviour: If the delete spans more than one
     * line and motion_type == MCHAR and the result is a blank line, make the
     * delete linewise.  Don't do this for the change command or Visual mode.
     */
    যদি (       oap->motion_type == MCHAR
	    && !oap->is_VIsual
	    && !oap->block_mode
	    && oap->সারি_গুনতি > 1
	    && oap->motion_force == NUL
	    && (vim_strchr(p_cpo, CPO_WORD) != ফাঁকা)
	    && oap->op_type == OP_DELETE)
    {
	ptr = ml_get(oap->end.lnum) + oap->end.col;
	যদি (*ptr != NUL)
	    ptr += oap->inclusive;
	ptr = skipwhite(ptr);
	যদি (*ptr == NUL && inindent(0))
	    oap->motion_type = MLINE;
    }

    /*
     * Check for trying to delete (e.g. "D") in an empty line.
     * Note: For the change operator it is ok.
     */
    যদি (       oap->motion_type == MCHAR
	    && oap->সারি_গুনতি == 1
	    && oap->op_type == OP_DELETE
	    && *ml_get(oap->start.lnum) == NUL)
    {
	/*
	 * It's an error to operate on an empty region, when 'E' included in
	 * 'cpoptions' (Vi compatible).
	 */
	যদি (virtual_op)
	    // Virtual editing: Nothing gets deleted, but we set the '[ and ']
	    // marks as if it happened.
	    যাও setmarks;
	যদি (vim_strchr(p_cpo, CPO_EMPTYREGION) != ফাঁকা)
	    beep_flush();
	ফেরত OK;
    }

    /*
     * Do a yank of whatever we're about to delete.
     * If a yank register was specified, put the deleted text into that
     * register.  For the black hole register '_' don't yank anything.
     */
    যদি (oap->regname != '_')
    {
#ifdef FEAT_CLIPBOARD_PROVIDER
	inc_clip_provider();
#endif
	যদি (oap->regname != 0)
	{
	    // check for read-only register
	    যদি (!valid_yank_reg(oap->regname, TRUE))
	    {
		beep_flush();
#ifdef FEAT_CLIPBOARD_PROVIDER
		dec_clip_provider();
#endif
		ফেরত OK;
	    }
	    get_yank_register(oap->regname, TRUE); // yank into specif'd reg.
	    যদি (op_yank(oap, TRUE, FALSE) == OK)   // yank without message
		did_yank = TRUE;
	}
	অন্যথা
	    reset_y_append(); // not appending to unnamed register

	/*
	 * Put deleted text into register 1 and shift number registers if the
	 * delete contains a line break, or when using a specific operator (Vi
	 * compatible)
	 */
	যদি (oap->motion_type == MLINE || oap->সারি_গুনতি > 1
							   || oap->use_reg_one)
	{
	    shift_delete_registers();
	    যদি (op_yank(oap, TRUE, FALSE) == OK)
		did_yank = TRUE;
	}

	// Yank into small delete register when no named register specified
	// and the delete is within one line.
	যদি ((
#ifdef HAVE_CLIPMETHOD
	    ((clip_unnamed & CLIP_UNNAMED) && oap->regname == '*') ||
	    ((clip_unnamed & CLIP_UNNAMED_PLUS) && oap->regname == '+') ||
#endif
	    oap->regname == 0) && oap->motion_type != MLINE
						      && oap->সারি_গুনতি == 1)
	{
	    oap->regname = '-';
	    get_yank_register(oap->regname, TRUE);
	    যদি (op_yank(oap, TRUE, FALSE) == OK)
		did_yank = TRUE;
	    oap->regname = 0;
	}

	/*
	 * If there's too much stuff to fit in the yank register, then get a
	 * confirmation before doing the delete. This is crude, but simple.
	 * And it avoids doing a delete of something we can't put back if we
	 * want.
	 */
	যদি (!did_yank)
	{
	    পূর্ণসংখ্যা বার্তা_silent_save = বার্তা_silent;

	    বার্তা_silent = 0;	// must display the prompt
	    n = ask_yesno((char_u *)_("cannot yank; delete anyway"), TRUE);
	    বার্তা_silent = বার্তা_silent_save;
	    যদি (n != 'y')
	    {
		emsg(_(e_command_aborted));
#ifdef FEAT_CLIPBOARD_PROVIDER
		dec_clip_provider();
#endif
		ফেরত FAIL;
	    }
	}

#যদি defined(FEAT_EVAL)
	যদি (did_yank && has_textyankpost())
	    yank_do_autocmd(oap, get_y_current());
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
    }

    /*
     * block mode delete
     */
    যদি (oap->block_mode)
    {
	যদি (u_save((linenr_T)(oap->start.lnum - 1),
			       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	    ফেরত FAIL;

	জন্য (lnum = curwin->w_cursor.lnum; lnum <= oap->end.lnum; ++lnum)
	{
	    block_prep(oap, &bd, lnum, TRUE);
	    যদি (bd.textlen == 0)	// nothing to delete
		চালু;

	    // Adjust cursor position for tab replaced by spaces and 'lbr'.
	    যদি (lnum == curwin->w_cursor.lnum)
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
	    যদি (newp == ফাঁকা)
		চালু;
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
	    যদি (curbuf->b_has_textprop && n != 0)
		adjust_prop_columns(lnum, bd.textcol, -n, 0);
#endif
	}

	check_cursor_col();
	changed_সারিগুলি(curwin->w_cursor.lnum, curwin->w_cursor.col,
						       oap->end.lnum + 1, 0L);
	oap->সারি_গুনতি = 0;	    // no lines deleted
    }
    অন্যথা যদি (oap->motion_type == MLINE)
    {
	যদি (oap->op_type == OP_CHANGE)
	{
	    // Delete the lines except the first one.  Temporarily move the
	    // cursor to the next line.  Save the current line number, if the
	    // last line is deleted it may be changed.
	    যদি (oap->সারি_গুনতি > 1)
	    {
		lnum = curwin->w_cursor.lnum;
		++curwin->w_cursor.lnum;
		del_সারিগুলি((long)(oap->সারি_গুনতি - 1), TRUE);
		curwin->w_cursor.lnum = lnum;
	    }
	    যদি (u_save_cursor() == FAIL)
		ফেরত FAIL;
	    যদি (curbuf->b_p_ai)		    // don't delete indent
	    {
		beginline(BL_WHITE);	    // cursor on first non-white
		did_ai = TRUE;		    // delete the indent when ESC hit
		ai_col = curwin->w_cursor.col;
	    }
	    অন্যথা
		beginline(0);		    // cursor in column 0
	    truncate_সারি(FALSE);   // delete the rest of the line,
				    // leaving cursor past last char in line
	    যদি (oap->সারি_গুনতি > 1)
		u_clearline();	    // "U" command not possible after "2cc"
	}
	অন্যথা
	{
	    del_সারিগুলি(oap->সারি_গুনতি, TRUE);
	    beginline(BL_WHITE | BL_FIX);
	    u_clearline();	// "U" command not possible after "dd"
	}
    }
    অন্যথা
    {
	যদি (virtual_op)
	{
	    পূর্ণসংখ্যা		endcol = 0;

	    // For virtualedit: break the tabs that are partly included.
	    যদি (gchar_pos(&oap->start) == '\t')
	    {
		যদি (u_save_cursor() == FAIL)	// save first line for undo
		    ফেরত FAIL;
		যদি (oap->সারি_গুনতি == 1)
		    endcol = getviscol2(oap->end.col, oap->end.coladd);
		coladvance_force(getviscol2(oap->start.col, oap->start.coladd));
		oap->start = curwin->w_cursor;
		যদি (oap->সারি_গুনতি == 1)
		{
		    coladvance(endcol);
		    oap->end.col = curwin->w_cursor.col;
		    oap->end.coladd = curwin->w_cursor.coladd;
		    curwin->w_cursor = oap->start;
		}
	    }

	    // Break a tab only when it's included in the area.
	    যদি (gchar_pos(&oap->end) == '\t'
				     && (পূর্ণসংখ্যা)oap->end.coladd < oap->inclusive)
	    {
		// save last line for undo
		যদি (u_save((linenr_T)(oap->end.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
		    ফেরত FAIL;
		curwin->w_cursor = oap->end;
		coladvance_force(getviscol2(oap->end.col, oap->end.coladd));
		oap->end = curwin->w_cursor;
		curwin->w_cursor = oap->start;
	    }
	    যদি (has_mbyte)
		mb_adjust_opend(oap);
	}

	যদি (oap->সারি_গুনতি == 1)	// delete characters within one line
	{
	    যদি (u_save_cursor() == FAIL)	// save line for undo
		ফেরত FAIL;

	    // if 'cpoptions' contains '$', display '$' at end of change
	    যদি (       vim_strchr(p_cpo, CPO_DOLLAR) != ফাঁকা
		    && oap->op_type == OP_CHANGE
		    && oap->end.lnum == curwin->w_cursor.lnum
		    && !oap->is_VIsual)
		display_dollar(oap->end.col - !oap->inclusive);

	    n = oap->end.col - oap->start.col + 1 - !oap->inclusive;

	    যদি (virtual_op)
	    {
		// fix up things for virtualedit-delete:
		// break the tabs which are going to get in our way
		পূর্ণসংখ্যা		len = ml_get_curline_len();

		যদি (oap->end.coladd != 0
			&& (পূর্ণসংখ্যা)oap->end.col >= len - 1
			&& !(oap->start.coladd && (পূর্ণসংখ্যা)oap->end.col >= len - 1))
		    n++;
		// Delete at least one char (e.g, when on a control char).
		যদি (n == 0 && oap->start.coladd != oap->end.coladd)
		    n = 1;

		// When deleted a char in the line, reset coladd.
		যদি (gchar_cursor() != NUL)
		    curwin->w_cursor.coladd = 0;
	    }
	    (শূন্য)del_bytes((long)n, !virtual_op,
			    oap->op_type == OP_DELETE && !oap->is_VIsual);
	}
	অন্যথা				// delete characters between lines
	{
	    pos_T   curpos;

	    // save deleted and changed lines for undo
	    যদি (u_save((linenr_T)(curwin->w_cursor.lnum - 1),
		 (linenr_T)(curwin->w_cursor.lnum + oap->সারি_গুনতি)) == FAIL)
		ফেরত FAIL;

	    truncate_সারি(TRUE);	// delete from cursor to end of line

	    curpos = curwin->w_cursor;	// remember curwin->w_cursor
	    ++curwin->w_cursor.lnum;
	    del_সারিগুলি((long)(oap->সারি_গুনতি - 2), FALSE);

	    // delete from start of line until op_end
	    n = (oap->end.col + 1 - !oap->inclusive);
	    curwin->w_cursor.col = 0;
	    (শূন্য)del_bytes((long)n, !virtual_op,
			    oap->op_type == OP_DELETE && !oap->is_VIsual);
	    curwin->w_cursor = curpos;	// restore curwin->w_cursor
	    (শূন্য)do_join(2, FALSE, FALSE, FALSE, FALSE);
	}
	যদি (oap->op_type == OP_DELETE)
	    auto_format(FALSE, TRUE);
    }

    msgmore(curbuf->b_ml.ml_সারি_গুনতি - old_lcount);

setmarks:
    যদি ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	যদি (oap->block_mode)
	{
	    curbuf->b_op_end.lnum = oap->end.lnum;
	    curbuf->b_op_end.col = oap->start.col;
	}
	অন্যথা
	    curbuf->b_op_end = oap->start;
	curbuf->b_op_start = oap->start;
    }

    ফেরত OK;
}

/*
 * Adjust end of operating area for ending on a multi-byte character.
 * Used for deletion.
 */
    স্থির শূন্য
mb_adjust_opend(oparg_T *oap)
{
    char_u	*সারি;
    char_u	*ptr;

    যদি (!oap->inclusive)
	ফেরত;

    সারি = ml_get(oap->end.lnum);
    ptr = সারি + oap->end.col;
    যদি (*ptr != NUL)
    {
	ptr -= (*mb_শীর্ষ_off)(সারি, ptr);
	ptr += (*mb_ptr2len)(ptr) - 1;
	oap->end.col = ptr - সারি;
    }
}

/*
 * Replace the character under the cursor with "c".
 * This takes care of multi-byte characters.
 */
    স্থির শূন্য
replace_character(পূর্ণসংখ্যা c)
{
    পূর্ণসংখ্যা n = State;

    State = MODE_REPLACE;
    ins_char(c);
    State = n;
    // Backup to the replaced character.
    dec_cursor();
}

/*
 * Replace a whole area with one character.
 */
    পূর্ণসংখ্যা
op_replace(oparg_T *oap, পূর্ণসংখ্যা c)
{
    পূর্ণসংখ্যা			n, numc;
    পূর্ণসংখ্যা			অঙ্ক_chars;
    char_u		*newp, *oldp;
    size_t		newlen, oldlen;
    কাঠামো block_def	bd;
    char_u		*after_p = ফাঁকা;
    পূর্ণসংখ্যা			had_ctrl_v_cr = FALSE;

    যদি ((curbuf->b_ml.ml_flags & ML_EMPTY ) || oap->empty)
	ফেরত OK;	    // nothing to do

    যদি (c == REPLACE_CR_NCHAR)
    {
	had_ctrl_v_cr = TRUE;
	c = CAR;
    }
    অন্যথা যদি (c == REPLACE_NL_NCHAR)
    {
	had_ctrl_v_cr = TRUE;
	c = NL;
    }

    যদি (has_mbyte)
	mb_adjust_opend(oap);

    যদি (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	ফেরত FAIL;

    /*
     * block mode replace
     */
    যদি (oap->block_mode)
    {
	bd.is_MAX = (curwin->w_curswant == MAXCOL);
	জন্য ( ; curwin->w_cursor.lnum <= oap->end.lnum; ++curwin->w_cursor.lnum)
	{
	    curwin->w_cursor.col = 0;  // make sure cursor position is valid
	    block_prep(oap, &bd, curwin->w_cursor.lnum, TRUE);
	    যদি (bd.textlen == 0 && (!virtual_op || bd.is_MAX))
		চালু;	    // nothing to replace

	    // n == number of extra chars required
	    // If we split a TAB, it may be replaced by several characters.
	    // Thus the number of characters may increase!
	    // If the range starts in virtual space, count the initial
	    // coladd offset as part of "startspaces"
	    যদি (virtual_op && bd.is_short && *bd.textstart == NUL)
	    {
		pos_T vpos;

		vpos.lnum = curwin->w_cursor.lnum;
		getvpos(&vpos, oap->start_vcol);
		bd.startspaces += vpos.coladd;
		n = bd.startspaces;
	    }
	    অন্যথা
		// allow for pre spaces
		n = (bd.startspaces ? bd.start_char_vcols - 1 : 0);

	    // allow for post spp
	    n += (bd.endspaces
		    && !bd.is_oneChar
		    && bd.end_char_vcols > 0) ? bd.end_char_vcols - 1 : 0;
	    // Figure out how many characters to replace.
	    numc = oap->end_vcol - oap->start_vcol + 1;
	    যদি (bd.is_short && (!virtual_op || bd.is_MAX))
		numc -= (oap->end_vcol - bd.end_vcol) + 1;

	    // A double-wide character can be replaced only up to half the
	    // times.
	    যদি ((*mb_char2cells)(c) > 1)
	    {
		যদি ((numc & 1) && !bd.is_short)
		{
		    ++bd.endspaces;
		    ++n;
		}
		numc = numc / 2;
	    }

	    // Compute bytes needed, move character count to num_chars.
	    অঙ্ক_chars = numc;
	    numc *= (*mb_char2len)(c);
	    // oldlen includes textlen, so don't double count
	    n += numc - bd.textlen;

	    oldp = ml_get_curline();
	    oldlen = ml_get_curline_len();
	    newp = alloc(oldlen + 1 + n);
	    যদি (newp == ফাঁকা)
		চালু;
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
	    যদি (had_ctrl_v_cr || (c != '\r' && c != '\n'))
	    {
		যদি (has_mbyte)
		{
		    যখন (--অঙ্ক_chars >= 0)
			newlen += (*mb_char2bytes)(c, newp + newlen);
		}
		অন্যথা
		{
		    vim_memset(newp + newlen, c, (size_t)numc);
		    newlen += numc;
		}
		যদি (!bd.is_short)
		{
		    // insert post-spaces
		    vim_memset(newp + newlen, ' ', (size_t)bd.endspaces);
		    // copy the part after the changed part
		    STRCPY(newp + newlen + bd.endspaces,
				oldp + bd.textcol + bd.textlen);
		}
	    }
	    অন্যথা
	    {
		// Replacing with \r or \n means splitting the line.
		after_p = alloc(oldlen + 1 + n - newlen);
		যদি (after_p != ফাঁকা)
		    STRCPY(after_p, oldp + bd.textcol + bd.textlen);
	    }

	    // replace the line
	    ml_replace(curwin->w_cursor.lnum, newp, FALSE);
	    যদি (after_p != ফাঁকা)
	    {
		ml_append(curwin->w_cursor.lnum++, after_p, 0, FALSE);
		appended_সারিগুলি_mark(curwin->w_cursor.lnum, 1L);
		oap->end.lnum++;
		vim_free(after_p);
	    }
	}
    }
    অন্যথা
    {
	/*
	 * MCHAR and MLINE motion replace.
	 */
	যদি (oap->motion_type == MLINE)
	{
	    oap->start.col = 0;
	    curwin->w_cursor.col = 0;
	    oap->end.col = ml_get_len(oap->end.lnum);
	    যদি (oap->end.col)
		--oap->end.col;
	}
	অন্যথা যদি (!oap->inclusive)
	    dec(&(oap->end));

	যখন (LTOREQ_POS(curwin->w_cursor, oap->end))
	{
	    পূর্ণসংখ্যা done = FALSE;

	    n = gchar_cursor();
	    যদি (n != NUL)
	    {
		পূর্ণসংখ্যা new_byte_len = (*mb_char2len)(c);
		পূর্ণসংখ্যা old_byte_len = mb_ptr2len(ml_get_cursor());

		যদি (new_byte_len > 1 || old_byte_len > 1)
		{
		    // This is slow, but it handles replacing a single-byte
		    // with a multi-byte and the other way around.
		    যদি (curwin->w_cursor.lnum == oap->end.lnum)
			oap->end.col += new_byte_len - old_byte_len;
		    replace_character(c);
		    done = TRUE;
		}
		অন্যথা
		{
		    যদি (n == TAB)
		    {
			পূর্ণসংখ্যা end_vcol = 0;

			যদি (curwin->w_cursor.lnum == oap->end.lnum)
			{
			    // oap->end has to be recalculated when
			    // the tab breaks
			    end_vcol = getviscol2(oap->end.col,
							     oap->end.coladd);
			}
			coladvance_force(getviscol());
			যদি (curwin->w_cursor.lnum == oap->end.lnum)
			    getvpos(&oap->end, end_vcol);
		    }
		    // with "coladd" set may move to just after a TAB
		    যদি (gchar_cursor() != NUL)
		    {
			PBYTE(curwin->w_cursor, c);
			done = TRUE;
		    }
		}
	    }
	    যদি (!done && virtual_op && curwin->w_cursor.lnum == oap->end.lnum)
	    {
		পূর্ণসংখ্যা virtcols = oap->end.coladd;

		যদি (curwin->w_cursor.lnum == oap->start.lnum
			&& oap->start.col == oap->end.col && oap->start.coladd)
		    virtcols -= oap->start.coladd;

		// oap->end has been trimmed so it's effectively inclusive;
		// as a result an extra +1 must be counted so we don't
		// trample the NUL byte.
		coladvance_force(getviscol2(oap->end.col, oap->end.coladd) + 1);
		curwin->w_cursor.col -= (virtcols + 1);
		জন্য (; virtcols >= 0; virtcols--)
		{
		    যদি ((*mb_char2len)(c) > 1)
		       replace_character(c);
		    অন্যথা
			PBYTE(curwin->w_cursor, c);
		    যদি (inc(&curwin->w_cursor) == -1)
			বিরতি;
		}
	    }

	    // Advance to next character, stop at the end of the file.
	    যদি (inc_cursor() == -1)
		বিরতি;
	}
    }

    curwin->w_cursor = oap->start;
    check_cursor();
    changed_সারিগুলি(oap->start.lnum, oap->start.col, oap->end.lnum + 1, 0L);

    যদি ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
    }

    ফেরত OK;
}

স্থির পূর্ণসংখ্যা swapchars(পূর্ণসংখ্যা op_type, pos_T *pos, পূর্ণসংখ্যা দৈর্ঘ্য);

/*
 * Handle the (non-standard vi) tilde operator.  Also for "gu", "gU" and "g?".
 */
    স্থির শূন্য
op_tilde(oparg_T *oap)
{
    pos_T		pos;
    কাঠামো block_def	bd;
    পূর্ণসংখ্যা			did_change = FALSE;

    যদি (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	ফেরত;

    pos = oap->start;
    যদি (oap->block_mode)		    // Visual block mode
    {
	জন্য (; pos.lnum <= oap->end.lnum; ++pos.lnum)
	{
	    পূর্ণসংখ্যা one_change;

	    block_prep(oap, &bd, pos.lnum, FALSE);
	    pos.col = bd.textcol;
	    one_change = swapchars(oap->op_type, &pos, bd.textlen);
	    did_change |= one_change;

#ifdef FEAT_NETBEANS_INTG
	    যদি (netbeans_active() && one_change)
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
	যদি (did_change)
	    changed_সারিগুলি(oap->start.lnum, 0, oap->end.lnum + 1, 0L);
    }
    অন্যথা				    // not block mode
    {
	যদি (oap->motion_type == MLINE)
	{
	    oap->start.col = 0;
	    pos.col = 0;
	    oap->end.col = ml_get_len(oap->end.lnum);
	    যদি (oap->end.col)
		--oap->end.col;
	}
	অন্যথা যদি (!oap->inclusive)
	    dec(&(oap->end));

	যদি (pos.lnum == oap->end.lnum)
	    did_change = swapchars(oap->op_type, &pos,
						  oap->end.col - pos.col + 1);
	অন্যথা
	    জন্য (;;)
	    {
		did_change |= swapchars(oap->op_type, &pos,
			pos.lnum == oap->end.lnum ? oap->end.col + 1
						  : ml_get_pos_len(&pos));
		যদি (LTOREQ_POS(oap->end, pos) || inc(&pos) == -1)
		    বিরতি;
	    }
	যদি (did_change)
	{
	    changed_সারিগুলি(oap->start.lnum, oap->start.col, oap->end.lnum + 1,
									  0L);
#ifdef FEAT_NETBEANS_INTG
	    যদি (netbeans_active())
	    {
		char_u *ptr;
		পূর্ণসংখ্যা গুনতি;

		pos = oap->start;
		যখন (pos.lnum < oap->end.lnum)
		{
		    ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		    গুনতি = ml_get_buf_len(curbuf, pos.lnum) - pos.col;
		    netbeans_removed(curbuf, pos.lnum, pos.col, (long)গুনতি);
		    // get the line again, it may have been flushed
		    ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		    netbeans_inserted(curbuf, pos.lnum, pos.col,
							&ptr[pos.col], গুনতি);
		    pos.col = 0;
		    pos.lnum++;
		}
		গুনতি = oap->end.col - pos.col + 1;
		netbeans_removed(curbuf, pos.lnum, pos.col, (long)গুনতি);
		// get the line again, it may have been flushed
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, pos.col,
							&ptr[pos.col], গুনতি);
	    }
#endif
	}
    }

    যদি (!did_change && oap->is_VIsual)
	// No change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    যদি ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set '[ and '] marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
    }

    যদি (oap->সারি_গুনতি > p_report)
	smsg(NGETTEXT("%ld line changed", "%ld lines changed",
					    oap->সারি_গুনতি), oap->সারি_গুনতি);
}

/*
 * Invoke swapchar() on "length" bytes at position "pos".
 * "pos" is advanced to just after the changed characters.
 * "length" is rounded up to include the whole last multi-byte character.
 * Also works correctly when the number of bytes changes.
 * Returns TRUE if some character was changed.
 */
    স্থির পূর্ণসংখ্যা
swapchars(পূর্ণসংখ্যা op_type, pos_T *pos, পূর্ণসংখ্যা দৈর্ঘ্য)
{
    পূর্ণসংখ্যা todo;
    পূর্ণসংখ্যা	did_change = 0;

    জন্য (todo = দৈর্ঘ্য; todo > 0; --todo)
    {
	যদি (has_mbyte)
	{
	    পূর্ণসংখ্যা len = (*mb_ptr2len)(ml_get_pos(pos));

	    // we're counting bytes, not characters
	    যদি (len > 0)
		todo -= len - 1;
	}
	did_change |= swapchar(op_type, pos);
	যদি (inc(pos) == -1)    // at end of file
	    বিরতি;
    }
    ফেরত did_change;
}

/*
 * If op_type == OP_UPPER: make uppercase,
 * if op_type == OP_LOWER: make lowercase,
 * if op_type == OP_ROT13: do rot13 encoding,
 * else swap case of character at 'pos'
 * returns TRUE when something actually changed.
 */
    পূর্ণসংখ্যা
swapchar(পূর্ণসংখ্যা op_type, pos_T *pos)
{
    পূর্ণসংখ্যা	    c;
    পূর্ণসংখ্যা	    nc;

    c = gchar_pos(pos);

    // Only do rot13 encoding for ASCII characters.
    যদি (c >= 0x80 && op_type == OP_ROT13)
	ফেরত FALSE;

    // ~ is OP_NOP, g~ is OP_TILDE, gU is OP_UPPER
    যদি ((op_type == OP_UPPER || op_type == OP_NOP || op_type == OP_TILDE)
	    && c == 0xdf
	    && (enc_latin1like || STRCMP(p_enc, "iso-8859-2") == 0))
    {
	pos_T   sp = curwin->w_cursor;

	// Special handling for lowercase German sharp s (ß): convert to uppercase (ẞ).
	curwin->w_cursor = *pos;
	del_char(FALSE);
	ins_char(0x1E9E);
	curwin->w_cursor = sp;
	ফেরত TRUE;
    }

    যদি (enc_dbcs != 0 && c >= 0x100)	// No lower/uppercase letter
	ফেরত FALSE;
    nc = c;
    যদি (MB_ISLOWER(c))
    {
	যদি (op_type == OP_ROT13)
	    nc = ROT13(c, 'a');
	অন্যথা যদি (op_type != OP_LOWER)
	    nc = MB_TOUPPER(c);
    }
    অন্যথা যদি (MB_ISUPPER(c))
    {
	যদি (op_type == OP_ROT13)
	    nc = ROT13(c, 'A');
	অন্যথা যদি (op_type != OP_UPPER)
	    nc = MB_TOLOWER(c);
    }
    যদি (nc != c)
    {
	যদি (enc_utf8 && (c >= 0x80 || nc >= 0x80))
	{
	    pos_T   sp = curwin->w_cursor;

	    curwin->w_cursor = *pos;
	    // don't use del_char(), it also removes composing chars
	    del_bytes(utf_ptr2len(ml_get_cursor()), FALSE, FALSE);
	    ins_char(nc);
	    curwin->w_cursor = sp;
	}
	অন্যথা
	    PBYTE(*pos, nc);
	ফেরত TRUE;
    }
    ফেরত FALSE;
}

/*
 * op_insert - Insert and append operators for Visual mode.
 */
    শূন্য
op_ঢোকাও(oparg_T *oap, long count1)
{
    long		pre_textlen = 0;
    colnr_T		ind_pre_col = 0, ind_post_col;
    পূর্ণসংখ্যা			ind_pre_vcol = 0, ind_post_vcol = 0;
    কাঠামো block_def	bd;
    পূর্ণসংখ্যা			i;
    pos_T		t1;
    pos_T		start_ঢোকাও;

    // edit() changes this - record it for OP_APPEND
    bd.is_MAX = (curwin->w_curswant == MAXCOL);

    // vis block is still marked. Get rid of it now.
    curwin->w_cursor.lnum = oap->start.lnum;
    update_screen(UPD_INVERTED);

    যদি (oap->block_mode)
    {
	// When 'virtualedit' is used, need to insert the extra spaces before
	// doing block_prep().  When only "block" is used, virtual edit is
	// already disabled, but still need it when calling
	// coladvance_force().
	// coladvance_force() uses get_ve_flags() to get the 'virtualedit'
	// state for the current window.  To override that state, we need to
	// set the window-local value of ve_flags rather than the global value.
	যদি (curwin->w_cursor.coladd > 0)
	{
	    পূর্ণসংখ্যা		old_ve_flags = curwin->w_ve_flags;

	    যদি (u_save_cursor() == FAIL)
		ফেরত;

	    curwin->w_ve_flags = VE_ALL;
	    coladvance_force(oap->op_type == OP_APPEND
					   ? oap->end_vcol + 1 : getviscol());
	    যদি (oap->op_type == OP_APPEND)
		--curwin->w_cursor.col;
	    curwin->w_ve_flags = old_ve_flags;
	}
	// Get the info about the block before entering the text
	block_prep(oap, &bd, oap->start.lnum, TRUE);
	// Get indent information
	ind_pre_col = (colnr_T)getwhitecols_curline();
	ind_pre_vcol = get_indent();
	pre_textlen = ml_get_len(oap->start.lnum) - bd.textcol;
	যদি (oap->op_type == OP_APPEND)
	    pre_textlen -= bd.textlen;
    }

    যদি (oap->op_type == OP_APPEND)
    {
	যদি (oap->block_mode && curwin->w_cursor.coladd == 0)
	{
	    // Move the cursor to the character right of the block.
	    curwin->w_set_curswant = true;
	    যখন (*ml_get_cursor() != NUL
		    && (curwin->w_cursor.col < bd.textcol + bd.textlen))
		++curwin->w_cursor.col;
	    যদি (bd.is_short && !bd.is_MAX)
	    {
		// First line was too short, make it longer and adjust the
		// values in "bd".
		যদি (u_save_cursor() == FAIL)
		    ফেরত;
		জন্য (i = 0; i < bd.endspaces; ++i)
		    ins_char(' ');
		bd.textlen += bd.endspaces;
	    }
	}
	অন্যথা
	{
	    curwin->w_cursor = oap->end;
	    check_cursor_col();

	    // Works just like an 'i'nsert on the next character.
	    যদি (!LINEEMPTY(curwin->w_cursor.lnum)
		    && oap->start_vcol != oap->end_vcol)
		inc_cursor();
	}
    }

    t1 = oap->start;
    start_ঢোকাও = curwin->w_cursor;
    (শূন্য)edit(NUL, FALSE, (linenr_T)count1);

    // When a tab was inserted, and the characters in front of the tab
    // have been converted to a tab as well, the column of the cursor
    // might have actually been reduced, so need to adjust here.
    যদি (t1.lnum == curbuf->b_op_start_orig.lnum
	    && LT_POS(curbuf->b_op_start_orig, t1))
	oap->start = curbuf->b_op_start_orig;

    // If user has moved off this line, we don't know what to do, so do
    // nothing.
    // Also don't repeat the insert when Insert mode ended with CTRL-C.
    যদি (curwin->w_cursor.lnum != oap->start.lnum || got_int)
	ফেরত;

    যদি (oap->block_mode)
    {
	পূর্ণসংখ্যা			ins_len;
	char_u			*firstline, *ins_text;
	কাঠামো block_def	bd2;
	পূর্ণসংখ্যা			did_indent = FALSE;
	size_t			len;
	size_t			add;
	// offset when cursor was moved in insert mode
	পূর্ণসংখ্যা			offset = 0;

	// If indent kicked in, the firstline might have changed
	// but only do that, if the indent actually increased.
	ind_post_col = (colnr_T)getwhitecols_curline();
	যদি (curbuf->b_op_start.col > ind_pre_col && ind_post_col > ind_pre_col)
	{
	    bd.textcol += ind_post_col - ind_pre_col;
	    ind_post_vcol = get_indent();
	    bd.start_vcol += ind_post_vcol - ind_pre_vcol;
	    did_indent = TRUE;
	}

	// The user may have moved the cursor before inserting something, try
	// to adjust the block for that.  But only do it, if the difference
	// does not come from indent kicking in.
	যদি (oap->start.lnum == curbuf->b_op_start_orig.lnum
						  && !bd.is_MAX && !did_indent)
	{
	    পূর্ণসংখ্যা t = getviscol2(curbuf->b_op_start_orig.col,
					       curbuf->b_op_start_orig.coladd);

	    যদি (oap->op_type == OP_ঢোকাও
		    && oap->start.col + oap->start.coladd
			    != curbuf->b_op_start_orig.col
					      + curbuf->b_op_start_orig.coladd)
	    {
		oap->start.col = curbuf->b_op_start_orig.col;
		pre_textlen -= t - oap->start_vcol;
		oap->start_vcol = t;
	    }
	    অন্যথা যদি (oap->op_type == OP_APPEND
		    && oap->start.col + oap->start.coladd
			    >= curbuf->b_op_start_orig.col
					      + curbuf->b_op_start_orig.coladd)
	    {
		oap->start.col = curbuf->b_op_start_orig.col;
		// reset pre_textlen to the value of OP_INSERT
		pre_textlen += bd.textlen;
		pre_textlen -= t - oap->start_vcol;
		oap->start_vcol = t;
		oap->op_type = OP_ঢোকাও;
	    }
	}

	// Spaces and tabs in the indent may have changed to other spaces and
	// tabs.  Get the starting column again and correct the length.
	// Don't do this when "$" used, end-of-line will have changed.
	//
	// if indent was added and the inserted text was after the indent,
	// correct the selection for the new indent.
	যদি (did_indent && bd.textcol - ind_post_col > 0)
	{
	    oap->start.col += ind_post_col - ind_pre_col;
	    oap->start_vcol += ind_post_vcol - ind_pre_vcol;
	    oap->end.col += ind_post_col - ind_pre_col;
	    oap->end_vcol += ind_post_vcol - ind_pre_vcol;
	}
	block_prep(oap, &bd2, oap->start.lnum, TRUE);
	যদি (did_indent && bd.textcol - ind_post_col > 0)
	{
	    // undo for where "oap" is used below
	    oap->start.col -= ind_post_col - ind_pre_col;
	    oap->start_vcol -= ind_post_vcol - ind_pre_vcol;
	    oap->end.col -= ind_post_col - ind_pre_col;
	    oap->end_vcol -= ind_post_vcol - ind_pre_vcol;
	}
	যদি (!bd.is_MAX || bd2.textlen < bd.textlen)
	{
	    যদি (oap->op_type == OP_APPEND)
	    {
		pre_textlen += bd2.textlen - bd.textlen;
		যদি (bd2.endspaces)
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
	যদি (oap->op_type == OP_APPEND)
	{
	    add += bd.textlen;
	    // account for pressing cursor in insert mode when '$' was used
	    যদি (bd.is_MAX
		&& (start_ঢোকাও.lnum == Insstart.lnum
					   && start_ঢোকাও.col > Insstart.col))
	    {
		offset = (start_ঢোকাও.col - Insstart.col);
		add -= offset;
		যদি (oap->end_vcol > offset)
		    oap->end_vcol -= (offset + 1);
		অন্যথা
		    // moved outside of the visual block, what to do?
		    ফেরত;
	    }
	}
	যদি (add > len)
	    add = len;  // short line, point to the NUL
	firstline += add;
	len -= add;
	যদি (pre_textlen >= 0 && (ins_len = (পূর্ণসংখ্যা)len - pre_textlen - offset) > 0)
	{
	    ins_text = vim_strnsave(firstline, ins_len);
	    যদি (ins_text != ফাঁকা)
	    {
		// block handled here
		যদি (u_save(oap->start.lnum,
					 (linenr_T)(oap->end.lnum + 1)) == OK)
		    block_ঢোকাও(oap, ins_text, ins_len, (oap->op_type == OP_ঢোকাও),
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
    পূর্ণসংখ্যা
op_change(oparg_T *oap)
{
    colnr_T		l;
    পূর্ণসংখ্যা			retval;
    linenr_T		linenr;
    long		pre_textlen = 0;
    long		pre_indent = 0;
    char_u		*firstline;
    char_u		*ins_text, *newp, *oldp;
    কাঠামো block_def	bd;

    l = oap->start.col;
    যদি (oap->motion_type == MLINE)
    {
	l = 0;
	can_si = may_do_si();	// Like opening a new line, do smart indent
    }

    // First delete the text in the region.  In an empty buffer only need to
    // save for undo
    যদি (curbuf->b_ml.ml_flags & ML_EMPTY)
    {
	যদি (u_save_cursor() == FAIL)
	    ফেরত FALSE;
    }
    অন্যথা যদি (op_delete(oap) == FAIL)
	ফেরত FALSE;

    যদি ((l > curwin->w_cursor.col) && !LINEEMPTY(curwin->w_cursor.lnum)
							 && !virtual_op)
	inc_cursor();

    // check for still on same line (<CR> in inserted text meaningless)
    // skip blank lines too
    যদি (oap->block_mode)
    {
	// Add spaces before getting the current line length.
	যদি (virtual_op && (curwin->w_cursor.coladd > 0
						    || gchar_cursor() == NUL))
	    coladvance_force(getviscol());
	firstline = ml_get(oap->start.lnum);
	pre_textlen = ml_get_len(oap->start.lnum);
	pre_indent = (long)getwhitecols(firstline);
	bd.textcol = curwin->w_cursor.col;
    }

    যদি (oap->motion_type == MLINE)
	fix_indent();

    // Reset finish_op now, don't want it set inside edit().
    পূর্ণসংখ্যা save_finish_op = finish_op;
    finish_op = FALSE;

    retval = edit(NUL, FALSE, (linenr_T)1);

    finish_op = save_finish_op;

    /*
     * In Visual block mode, handle copying the new text to all lines of the
     * block.
     * Don't repeat the insert when Insert mode ended with CTRL-C.
     */
    যদি (oap->block_mode && oap->start.lnum != oap->end.lnum && !got_int)
    {
	পূর্ণসংখ্যা	ins_len;

	// Auto-indenting may have changed the indent.  If the cursor was past
	// the indent, exclude that indent change from the inserted text.
	firstline = ml_get(oap->start.lnum);
	যদি (bd.textcol > (colnr_T)pre_indent)
	{
	    long new_indent = (long)getwhitecols(firstline);

	    pre_textlen += new_indent - pre_indent;
	    bd.textcol += new_indent - pre_indent;
	}

	ins_len = (পূর্ণসংখ্যা)ml_get_len(oap->start.lnum) - pre_textlen;
	যদি (ins_len > 0)
	{
	    // Subsequent calls to ml_get() flush the firstline data - take a
	    // copy of the inserted text.
	    যদি ((ins_text = alloc(ins_len + 1)) != ফাঁকা)
	    {
		vim_strncpy(ins_text, firstline + bd.textcol, ins_len);
		জন্য (linenr = oap->start.lnum + 1; linenr <= oap->end.lnum;
								     linenr++)
		{
		    block_prep(oap, &bd, linenr, TRUE);
		    যদি (!bd.is_short || virtual_op)
		    {
			pos_T vpos;
			size_t newlen;

			// If the block starts in virtual space, count the
			// initial coladd offset as part of "startspaces"
			যদি (bd.is_short)
			{
			    vpos.lnum = linenr;
			    (শূন্য)getvpos(&vpos, oap->start_vcol);
			}
			অন্যথা
			    vpos.coladd = 0;
			oldp = ml_get(linenr);
			newp = alloc(ml_get_len(linenr) + vpos.coladd + ins_len + 1);
			যদি (newp == ফাঁকা)
			    চালু;
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
			যদি (curbuf->b_has_textprop)
			    adjust_prop_columns(linenr, bd.textcol,
						     vpos.coladd + (পূর্ণসংখ্যা)ins_len, 0);
#endif
		    }
		}
		check_cursor();

		changed_সারিগুলি(oap->start.lnum + 1, 0, oap->end.lnum + 1, 0L);
	    }
	    vim_free(ins_text);
	}
    }
    auto_format(FALSE, TRUE);

    ফেরত retval;
}

/*
 * When the cursor is on the NUL past the end of the line and it should not be
 * there move it left.
 */
    শূন্য
adjust_cursor_eol(শূন্য)
{
    unsigned পূর্ণসংখ্যা cur_ve_flags = get_ve_flags();

    পূর্ণসংখ্যা adj_cursor = (curwin->w_cursor.col > 0
				&& gchar_cursor() == NUL
				&& (cur_ve_flags & VE_ONEMORE) == 0
				&& (cur_ve_flags & VE_ALL) == 0
				&& !(restart_edit || (State & MODE_ঢোকাও)));
    যদি (!adj_cursor)
	ফেরত;

    // Put the cursor on the last character in the line.
    dec_cursor();

    যদি (cur_ve_flags == VE_ALL)
    {
	colnr_T	    scol, ecol;

	// Coladd is set to the width of the last character.
	getvcol(curwin, &curwin->w_cursor, &scol, ফাঁকা, &ecol, 0);
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
    char_u   *সারি,
    পূর্ণসংখ্যা      process,
    পূর্ণসংখ্যা	     include_space,
    পূর্ণসংখ্যা      *is_comment)
{
    char_u *comment_flags = ফাঁকা;
    পূর্ণসংখ্যা    lead_len;
    পূর্ণসংখ্যা    leader_offset = get_last_leader_offset(সারি, &comment_flags);

    *is_comment = FALSE;
    যদি (leader_offset != -1)
    {
	// Let's check whether the line ends with an unclosed comment.
	// If the last comment leader has COM_END in flags, there's no comment.
	যখন (*comment_flags)
	{
	    যদি (*comment_flags == COM_END
		    || *comment_flags == ':')
		বিরতি;
	    ++comment_flags;
	}
	যদি (*comment_flags != COM_END)
	    *is_comment = TRUE;
    }

    যদি (process == FALSE)
	ফেরত সারি;

    lead_len = get_leader_len(সারি, &comment_flags, FALSE, include_space);

    যদি (lead_len == 0)
	ফেরত সারি;

    // Find:
    // - COM_END,
    // - colon,
    // whichever comes first.
    যখন (*comment_flags)
    {
	যদি (*comment_flags == COM_END
		|| *comment_flags == ':')
	    বিরতি;
	++comment_flags;
    }

    // If we found a colon, it means that we are not processing a line
    // starting with a closing part of a three-part comment. That's good,
    // because we don't want to remove those as this would be annoying.
    যদি (*comment_flags == ':' || *comment_flags == NUL)
	সারি += lead_len;

    ফেরত সারি;
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
    পূর্ণসংখ্যা
do_join(
    long    গুনতি,
    পূর্ণসংখ্যা	    ঢোকাও_space,
    পূর্ণসংখ্যা	    save_undo,
    পূর্ণসংখ্যা	    use_formatoptions UNUSED,
    পূর্ণসংখ্যা	    setmark)
{
    char_u	*curr = ফাঁকা;
    char_u      *curr_start = ফাঁকা;
    char_u	*cend;
    char_u	*newp;
    size_t	newp_len;
    char_u	*spaces;	// number of spaces inserted before a line
    পূর্ণসংখ্যা		endcurr1 = NUL;
    পূর্ণসংখ্যা		endcurr2 = NUL;
    পূর্ণসংখ্যা		currsize = 0;	// size of the current line
    পূর্ণসংখ্যা		sumsize = 0;	// size of the long new line
    linenr_T	t;
    colnr_T	col = 0;
    পূর্ণসংখ্যা		ret = OK;
    পূর্ণসংখ্যা		*comments = ফাঁকা;
    পূর্ণসংখ্যা		remove_comments = (use_formatoptions == TRUE)
				  && has_format_option(FO_REMOVE_COMS);
    পূর্ণসংখ্যা		পূর্ব_was_comment;

    যদি (save_undo && u_save((linenr_T)(curwin->w_cursor.lnum - 1),
			    (linenr_T)(curwin->w_cursor.lnum + গুনতি)) == FAIL)
	ফেরত FAIL;

    // Allocate an array to store the number of spaces inserted before each
    // line.  We will use it to pre-compute the length of the new line and the
    // proper placement of each original line in the new one.
    spaces = lalloc_clear(গুনতি, TRUE);
    যদি (spaces == ফাঁকা)
	ফেরত FAIL;
    যদি (remove_comments)
    {
	comments = lalloc_clear(গুনতি * আকার(পূর্ণসংখ্যা), TRUE);
	যদি (comments == ফাঁকা)
	{
	    vim_free(spaces);
	    ফেরত FAIL;
	}
    }

    /*
     * Don't move anything yet, just compute the final line length
     * and setup the array of space strings lengths
     * This loops forward over the joined lines.
     */
    জন্য (t = 0; t < গুনতি; ++t)
    {
	curr = curr_start = ml_get((linenr_T)(curwin->w_cursor.lnum + t));
	যদি (t == 0 && setmark && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set the '[ mark.
	    curwin->w_বাফার->b_op_start.lnum = curwin->w_cursor.lnum;
	    curwin->w_বাফার->b_op_start.col  = (colnr_T)STRLEN(curr);
	}
	যদি (remove_comments)
	{
	    // We don't want to remove the comment leader if the
	    // previous line is not a comment.
	    যদি (t > 0 && পূর্ব_was_comment)
	    {

		char_u *new_curr = skip_comment(curr, TRUE, ঢোকাও_space,
							   &পূর্ব_was_comment);
		comments[t] = (পূর্ণসংখ্যা)(new_curr - curr);
		curr = new_curr;
	    }
	    অন্যথা
		curr = skip_comment(curr, FALSE, ঢোকাও_space,
							   &পূর্ব_was_comment);
	}

	যদি (ঢোকাও_space && t > 0)
	{
	    curr = skipwhite(curr);
	    যদি (*curr != NUL && *curr != ')'
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
		যদি (endcurr1 == ' ')
		    endcurr1 = endcurr2;
		অন্যথা
		    ++spaces[t];
		// extra space when 'joinspaces' set and line ends in '.'
		যদি (       p_js
			&& (endcurr1 == '.'
			    || (vim_strchr(p_cpo, CPO_JOINSP) == ফাঁকা
				&& (endcurr1 == '?' || endcurr1 == '!'))))
		    ++spaces[t];
	    }
	}
	currsize = (পূর্ণসংখ্যা)STRLEN(curr);
	sumsize += currsize + spaces[t];
	endcurr1 = endcurr2 = NUL;
	যদি (ঢোকাও_space && currsize > 0)
	{
	    যদি (has_mbyte)
	    {
		cend = curr + currsize;
		MB_PTR_BACK(curr, cend);
		endcurr1 = (*mb_ptr2char)(cend);
		যদি (cend > curr)
		{
		    MB_PTR_BACK(curr, cend);
		    endcurr2 = (*mb_ptr2char)(cend);
		}
	    }
	    অন্যথা
	    {
		endcurr1 = *(curr + currsize - 1);
		যদি (currsize > 1)
		    endcurr2 = *(curr + currsize - 2);
	    }
	}
	সারি_breakcheck();
	যদি (got_int)
	{
	    ret = FAIL;
	    যাও theend;
	}
    }

    // store the column position before last line
    col = sumsize - currsize - spaces[গুনতি - 1];

    // allocate the space for the new line
    newp_len = sumsize + 1;
    newp = alloc(newp_len);
    যদি (newp == ফাঁকা)
    {
	ret = FAIL;
	যাও theend;
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
				curwin->w_বাফার, curwin->w_cursor.lnum, 0);
    // um_open_at_no_props may have invalidated "curr".
    পূর্ণসংখ্যা curr_off = (পূর্ণসংখ্যা)(curr - curr_start);

    curr = curr_start = ml_get((linenr_T)(curwin->w_cursor.lnum + গুনতি - 1));
    curr += curr_off;
#endif

    জন্য (t = গুনতি - 1; ; --t)
    {
	পূর্ণসংখ্যা spaces_removed;

	cend -= currsize;
	mch_memmove(cend, curr, (size_t)currsize);

	যদি (spaces[t] > 0)
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
		t == গুনতি - 1, (long)(cend - newp), spaces_removed);
#endif
	যদি (t == 0)
	    বিরতি;
	curr = curr_start = ml_get((linenr_T)(curwin->w_cursor.lnum + t - 1));
	যদি (remove_comments)
	    curr += comments[t - 1];
	যদি (ঢোকাও_space && t > 1)
	    curr = skipwhite(curr);
	currsize = (পূর্ণসংখ্যা)STRLEN(curr);
    }

#ifdef FEAT_PROP_POPUP
    যদি (um.buf != ফাঁকা)
    {
	um_set_text(&um, newp);
	um_reverse_props(&um);
	um_close(&um);
	newp = ফাঁকা;  // um_set_text took ownership
    }
    অন্যথা
#endif
    ml_replace_len(curwin->w_cursor.lnum, newp, (colnr_T)newp_len, TRUE, FALSE);

    যদি (setmark && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set the '] mark.
	curwin->w_বাফার->b_op_end.lnum = curwin->w_cursor.lnum;
	curwin->w_বাফার->b_op_end.col  = (colnr_T)sumsize;
    }

    // Only report the change in the first line here, del_lines() will report
    // the deleted line.
    changed_সারিগুলি(curwin->w_cursor.lnum, currsize,
					       curwin->w_cursor.lnum + 1, 0L);
    /*
     * Delete following lines. To do this we move the cursor there
     * briefly, and then move it back. After del_lines() the cursor may
     * have moved up (last line deleted), so the current lnum is kept in t.
     */
    t = curwin->w_cursor.lnum;
    ++curwin->w_cursor.lnum;
    del_সারিগুলি(গুনতি - 1, FALSE);
    curwin->w_cursor.lnum = t;

    /*
     * Set the cursor column:
     * Vi compatible: use the column of the first join
     * vim:	      use the column of the last join
     */
    curwin->w_cursor.col =
		    (vim_strchr(p_cpo, CPO_JOINCOL) != ফাঁকা ? currsize : col);
    check_cursor_col();

    curwin->w_cursor.coladd = 0;
    curwin->w_set_curswant = true;

theend:
    vim_free(spaces);
    যদি (remove_comments)
	vim_free(comments);
    ফেরত ret;
}

#ifdef FEAT_LINEBREAK
/*
 * TODO: consider using "tailp" of win_lbr_chartabsize() instead of temporarily
 * resetting 'linebreak'.
 *
 * Reset 'linebreak' and take care of side effects.
 * Returns the previous value, to be passed to restore_lbr().
 */
    পূর্ণসংখ্যা
reset_lbr(শূন্য)
{
    যদি (!curwin->w_p_lbr)
	ফেরত FALSE;
    // changing 'linebreak' may require w_virtcol to be updated
    curwin->w_p_lbr = FALSE;
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);
    ফেরত TRUE;
}

/*
 * Restore 'linebreak' and take care of side effects.
 */
    শূন্য
restore_lbr(পূর্ণসংখ্যা lbr_saved)
{
    যদি (curwin->w_p_lbr || !lbr_saved)
	ফেরত;

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
    শূন্য
block_prep(
    oparg_T		*oap,
    কাঠামো block_def	*bdp,
    linenr_T		lnum,
    পূর্ণসংখ্যা			is_del)
{
    পূর্ণসংখ্যা		incr = 0;
    char_u	*pend;
    char_u	*pstart;
    char_u	*সারি;
    char_u	*পূর্ব_pstart;
    char_u	*পূর্ব_pend;
    chartabsize_T cts;
#ifdef FEAT_LINEBREAK
		// Avoid a problem with unwanted linebreaks in block mode.
    পূর্ণসংখ্যা		lbr_saved = reset_lbr();
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

    সারি = ml_get(lnum);
    পূর্ব_pstart = সারি;
    init_chartabsize_arg(&cts, curwin, lnum, bdp->start_vcol, সারি, সারি);
    যখন (cts.cts_vcol < oap->start_vcol && *cts.cts_ptr != NUL)
    {
	// Count a tab for what it's worth (if list mode not on)
	incr = lbr_chartabsize(&cts);
	cts.cts_vcol += incr;
	যদি (VIM_ISWHITE(*cts.cts_ptr))
	{
	    bdp->pre_whitesp += incr;
	    bdp->pre_whitesp_c++;
	}
	অন্যথা
	{
	    bdp->pre_whitesp = 0;
	    bdp->pre_whitesp_c = 0;
	}
	পূর্ব_pstart = cts.cts_ptr;
	MB_PTR_ADV(cts.cts_ptr);
    }
    bdp->start_vcol = cts.cts_vcol;
    pstart = cts.cts_ptr;
    clear_chartabsize_arg(&cts);

    bdp->start_char_vcols = incr;
    যদি (bdp->start_vcol < oap->start_vcol)	// line too short
    {
	bdp->end_vcol = bdp->start_vcol;
	bdp->is_short = TRUE;
	যদি (!is_del || oap->op_type == OP_APPEND)
	    bdp->endspaces = oap->end_vcol - oap->start_vcol + 1;
    }
    অন্যথা
    {
	// notice: this converts partly selected Multibyte characters to
	// spaces, too.
	bdp->startspaces = bdp->start_vcol - oap->start_vcol;
	যদি (is_del && bdp->startspaces)
	    bdp->startspaces = bdp->start_char_vcols - bdp->startspaces;
	pend = pstart;
	bdp->end_vcol = bdp->start_vcol;
	যদি (bdp->end_vcol > oap->end_vcol)	// it's all in one character
	{
	    bdp->is_oneChar = TRUE;
	    যদি (oap->op_type == OP_ঢোকাও)
		bdp->endspaces = bdp->start_char_vcols - bdp->startspaces;
	    অন্যথা যদি (oap->op_type == OP_APPEND)
	    {
		bdp->startspaces += oap->end_vcol - oap->start_vcol + 1;
		bdp->endspaces = bdp->start_char_vcols - bdp->startspaces;
	    }
	    অন্যথা
	    {
		bdp->startspaces = oap->end_vcol - oap->start_vcol + 1;
		যদি (is_del && oap->op_type != OP_LSHIFT)
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
	অন্যথা
	{
	    init_chartabsize_arg(&cts, curwin, lnum, bdp->end_vcol,
								  সারি, pend);
	    পূর্ব_pend = pend;
	    যখন (cts.cts_vcol <= oap->end_vcol && *cts.cts_ptr != NUL)
	    {
		// count a tab for what it's worth (if list mode not on)
		পূর্ব_pend = cts.cts_ptr;
		incr = lbr_chartabsize_adv(&cts);
		cts.cts_vcol += incr;
	    }
	    bdp->end_vcol = cts.cts_vcol;
	    pend = cts.cts_ptr;
	    clear_chartabsize_arg(&cts);

	    যদি (bdp->end_vcol <= oap->end_vcol
		    && (!is_del
			|| oap->op_type == OP_APPEND
			|| oap->op_type == OP_REPLACE)) // line too short
	    {
		bdp->is_short = TRUE;
		// Alternative: include spaces to fill up the block.
		// Disadvantage: can lead to trailing spaces when the line is
		// short where the text is put
		// if (!is_del || oap->op_type == OP_APPEND)
		যদি (oap->op_type == OP_APPEND || virtual_op)
		    bdp->endspaces = oap->end_vcol - bdp->end_vcol
							     + oap->inclusive;
		অন্যথা
		    bdp->endspaces = 0; // replace doesn't add characters
	    }
	    অন্যথা যদি (bdp->end_vcol > oap->end_vcol)
	    {
		bdp->endspaces = bdp->end_vcol - oap->end_vcol - 1;
		যদি (!is_del && bdp->endspaces)
		{
		    bdp->endspaces = incr - bdp->endspaces;
		    যদি (pend != pstart)
			pend = পূর্ব_pend;
		}
	    }
	}
	bdp->end_char_vcols = incr;
	যদি (is_del && bdp->startspaces)
	    pstart = পূর্ব_pstart;
	bdp->textlen = (পূর্ণসংখ্যা)(pend - pstart);
    }
    bdp->textcol = (colnr_T) (pstart - সারি);
    bdp->textstart = pstart;
#ifdef FEAT_LINEBREAK
    restore_lbr(lbr_saved);
#endif
}

/*
 * Get block text from "start" to "end"
 */
    শূন্য
charwise_block_prep(
    pos_T		start,
    pos_T		end,
    কাঠামো block_def	*bdp,
    linenr_T		lnum,
    পূর্ণসংখ্যা			inclusive)
{
    colnr_T startcol = 0, endcol = MAXCOL;
    colnr_T cs, ce;
    char_u *p;
    পূর্ণসংখ্যা	plen = ml_get_len(lnum);

    p = ml_get(lnum);
    bdp->startspaces = 0;
    bdp->endspaces = 0;
    bdp->is_oneChar = FALSE;
    bdp->start_char_vcols = 0;

    যদি (lnum == start.lnum)
    {
	startcol = start.col;
	যদি (virtual_op)
	{
	    getvcol(curwin, &start, &cs, ফাঁকা, &ce, 0);
	    যদি (ce != cs && start.coladd > 0)
	    {
		// Part of a tab selected -- but don't
		// double-count it.
		bdp->start_char_vcols = ce - cs + 1;
		bdp->startspaces = bdp->start_char_vcols - start.coladd;
		যদি (bdp->startspaces < 0)
		    bdp->startspaces = 0;
		startcol++;
	    }
	}
    }

    যদি (lnum == end.lnum)
    {
	endcol = end.col;
	যদি (virtual_op)
	{
	    getvcol(curwin, &end, &cs, ফাঁকা, &ce, 0);
	    যদি (p[endcol] == NUL || (cs + end.coladd < ce
			// Don't add space for double-wide
			// char; endcol will be on last byte
			// of multi-byte char.
			&& (*mb_শীর্ষ_off)(p, p + endcol) == 0))
	    {
		যদি (start.lnum == end.lnum && start.col == end.col)
		{
		    // Special case: inside a single char
		    bdp->is_oneChar = TRUE;
		    bdp->startspaces = end.coladd - start.coladd + inclusive;
		    endcol = startcol;
		}
		অন্যথা
		{
		    bdp->endspaces = end.coladd + inclusive;
		    endcol -= inclusive;
		}
	    }
	}
    }
    যদি (endcol == MAXCOL)
	endcol = ml_get_len(lnum);
    যদি (startcol > endcol || bdp->is_oneChar)
	bdp->textlen = 0;
    অন্যথা
	bdp->textlen = endcol - startcol + inclusive;
    bdp->textcol = startcol;
    bdp->textstart = startcol <= plen ? p + startcol : p;
}

/*
 * Handle the add/subtract operator.
 */
    শূন্য
op_addsub(
    oparg_T	*oap,
    linenr_T	Prenum1,	    // Amount of add/subtract
    পূর্ণসংখ্যা		g_cmd)		    // was g<c-a>/g<c-x>
{
    pos_T		pos;
    কাঠামো block_def	bd;
    পূর্ণসংখ্যা			change_cnt = 0;
    linenr_T		amount = Prenum1;

    // do_addsub() might trigger re-evaluation of 'foldexpr' halfway, when the
    // buffer is not completely updated yet. Postpone updating folds until before
    // the call to changed_lines().
#ifdef FEAT_FOLDING
    disable_fold_update++;
#endif

    যদি (!VIsual_active)
    {
	pos = curwin->w_cursor;
	যদি (u_save_cursor() == FAIL)
	{
#ifdef FEAT_FOLDING
	    disable_fold_update--;
#endif
	    ফেরত;
	}
	change_cnt = do_addsub(oap->op_type, &pos, 0, amount);
#ifdef FEAT_FOLDING
	disable_fold_update--;
#endif
	যদি (change_cnt)
	    changed_সারিগুলি(pos.lnum, 0, pos.lnum + 1, 0L);
    }
    অন্যথা
    {
	পূর্ণসংখ্যা	one_change;
	পূর্ণসংখ্যা	দৈর্ঘ্য;
	pos_T	startpos;

	যদি (u_save((linenr_T)(oap->start.lnum - 1),
					(linenr_T)(oap->end.lnum + 1)) == FAIL)
	{
#ifdef FEAT_FOLDING
	    disable_fold_update--;
#endif
	    ফেরত;
	}

	pos = oap->start;
	জন্য (; pos.lnum <= oap->end.lnum; ++pos.lnum)
	{
	    যদি (oap->block_mode)		    // Visual block mode
	    {
		block_prep(oap, &bd, pos.lnum, FALSE);
		pos.col = bd.textcol;
		দৈর্ঘ্য = bd.textlen;
	    }
	    অন্যথা যদি (oap->motion_type == MLINE)
	    {
		curwin->w_cursor.col = 0;
		pos.col = 0;
		দৈর্ঘ্য = ml_get_len(pos.lnum);
	    }
	    অন্যথা // oap->motion_type == MCHAR
	    {
		যদি (pos.lnum == oap->start.lnum && !oap->inclusive)
		    dec(&(oap->end));
		দৈর্ঘ্য = ml_get_len(pos.lnum);
		pos.col = 0;
		যদি (pos.lnum == oap->start.lnum)
		{
		    pos.col += oap->start.col;
		    দৈর্ঘ্য -= oap->start.col;
		}
		যদি (pos.lnum == oap->end.lnum)
		{
		    দৈর্ঘ্য = ml_get_len(oap->end.lnum);
		    যদি (oap->end.col >= দৈর্ঘ্য)
			oap->end.col = দৈর্ঘ্য - 1;
		    দৈর্ঘ্য = oap->end.col - pos.col + 1;
		}
	    }
	    one_change = do_addsub(oap->op_type, &pos, দৈর্ঘ্য, amount);
	    যদি (one_change)
	    {
		// Remember the start position of the first change.
		যদি (change_cnt == 0)
		    startpos = curbuf->b_op_start;
		++change_cnt;
	    }

#ifdef FEAT_NETBEANS_INTG
	    যদি (netbeans_active() && one_change)
	    {
		char_u *ptr;

		netbeans_removed(curbuf, pos.lnum, pos.col, (long)দৈর্ঘ্য);
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, pos.col,
						&ptr[pos.col], দৈর্ঘ্য);
	    }
#endif
	    যদি (g_cmd && one_change)
		amount += Prenum1;
	}

#ifdef FEAT_FOLDING
	disable_fold_update--;
#endif
	যদি (change_cnt)
	    changed_সারিগুলি(oap->start.lnum, 0, oap->end.lnum + 1, 0L);

	যদি (!change_cnt && oap->is_VIsual)
	    // No change: need to remove the Visual selection
	    redraw_curbuf_later(UPD_INVERTED);

	// Set '[ mark if something changed. Keep the last end
	// position from do_addsub().
	যদি (change_cnt > 0 && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	    curbuf->b_op_start = startpos;

	যদি (change_cnt > p_report)
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
    স্থির পূর্ণসংখ্যা
do_addsub(
    পূর্ণসংখ্যা		op_type,
    pos_T	*pos,
    পূর্ণসংখ্যা		দৈর্ঘ্য,
    linenr_T	Prenum1)
{
    পূর্ণসংখ্যা		col;
    পূর্ণসংখ্যা		pre;		// 'X'/'x': hex; '0': octal; 'B'/'b': bin
    স্থির পূর্ণসংখ্যা	hexupper = FALSE;	// 0xABC
    uvarnumber_T	n;
    uvarnumber_T	oldn;
    char_u	*ptr;
    পূর্ণসংখ্যা		linelen;
    পূর্ণসংখ্যা		c;
    পূর্ণসংখ্যা		todel;
    পূর্ণসংখ্যা		do_hex;
    পূর্ণসংখ্যা		do_oct;
    পূর্ণসংখ্যা		do_bin;
    পূর্ণসংখ্যা		do_alpha;
    পূর্ণসংখ্যা		do_unsigned;
    পূর্ণসংখ্যা		do_blank;
    পূর্ণসংখ্যা		blank_unsigned = FALSE;	// blank: treat as unsigned?
    পূর্ণসংখ্যা		firstdigit;
    পূর্ণসংখ্যা		subtract;
    পূর্ণসংখ্যা		negative = FALSE;
    পূর্ণসংখ্যা		was_positive = TRUE;
    পূর্ণসংখ্যা		visual = VIsual_active;
    পূর্ণসংখ্যা		did_change = FALSE;
    pos_T	save_cursor = curwin->w_cursor;
    পূর্ণসংখ্যা		maxlen = 0;
    pos_T	startpos;
    pos_T	endpos;
    colnr_T	save_coladd = 0;

    do_hex = (vim_strchr(curbuf->b_p_nf, 'x') != ফাঁকা);	// "heX"
    do_oct = (vim_strchr(curbuf->b_p_nf, 'o') != ফাঁকা);	// "Octal"
    do_bin = (vim_strchr(curbuf->b_p_nf, 'b') != ফাঁকা);	// "Bin"
    do_alpha = (vim_strchr(curbuf->b_p_nf, 'p') != ফাঁকা);	// "alPha"
    do_unsigned = (vim_strchr(curbuf->b_p_nf, 'u') != ফাঁকা);	// "Unsigned"
    do_blank = (vim_strchr(curbuf->b_p_nf, 'k') != ফাঁকা);	// "blanK"

    যদি (virtual_active())
    {
	save_coladd = pos->coladd;
	pos->coladd = 0;
    }

    curwin->w_cursor = *pos;
    ptr = ml_get(pos->lnum);
    linelen = ml_get_len(pos->lnum);
    col = pos->col;

    যদি (col + !!save_coladd >= linelen)
	যাও theend;

    /*
     * First check if we are on a hexadecimal number, after the "0x".
     */
    যদি (!VIsual_active)
    {
	যদি (do_bin)
	    যখন (col > 0 && vim_isbdigit(ptr[col]))
	    {
		--col;
		যদি (has_mbyte)
		    col -= (*mb_শীর্ষ_off)(ptr, ptr + col);
	    }

	যদি (do_hex)
	    যখন (col > 0 && vim_isxdigit(ptr[col]))
	    {
		--col;
		যদি (has_mbyte)
		    col -= (*mb_শীর্ষ_off)(ptr, ptr + col);
	    }

	যদি (       do_bin
		&& do_hex
		&& ! ((col > 0
		    && (ptr[col] == 'X'
			|| ptr[col] == 'x')
		    && ptr[col - 1] == '0'
		    && (!has_mbyte ||
			!(*mb_শীর্ষ_off)(ptr, ptr + col - 1))
		    && vim_isxdigit(ptr[col + 1]))))
	{

	    // In case of binary/hexadecimal pattern overlap match, rescan

	    col = pos->col;

	    যখন (col > 0 && vim_isdigit(ptr[col]))
	    {
		col--;
		যদি (has_mbyte)
		    col -= (*mb_শীর্ষ_off)(ptr, ptr + col);
	    }
	}

	যদি ((       do_hex
		&& col > 0
		&& (ptr[col] == 'X'
		    || ptr[col] == 'x')
		&& ptr[col - 1] == '0'
		&& (!has_mbyte ||
		    !(*mb_শীর্ষ_off)(ptr, ptr + col - 1))
		&& vim_isxdigit(ptr[col + 1])) ||
	    (       do_bin
		&& col > 0
		&& (ptr[col] == 'B'
		    || ptr[col] == 'b')
		&& ptr[col - 1] == '0'
		&& (!has_mbyte ||
		    !(*mb_শীর্ষ_off)(ptr, ptr + col - 1))
		&& vim_isbdigit(ptr[col + 1])))
	{
	    // Found hexadecimal or binary number, move to its start.
	    --col;
	    যদি (has_mbyte)
		col -= (*mb_শীর্ষ_off)(ptr, ptr + col);
	}
	অন্যথা
	{
	    /*
	     * Search forward and then backward to find the start of number.
	     */
	    col = pos->col;

	    যখন (ptr[col] != NUL
		    && !vim_isdigit(ptr[col])
		    && !(do_alpha && ASCII_ISALPHA(ptr[col])))
		col += mb_ptr2len(ptr + col);

	    যখন (col > 0
		    && vim_isdigit(ptr[col - 1])
		    && !(do_alpha && ASCII_ISALPHA(ptr[col])))
	    {
		--col;
		যদি (has_mbyte)
		    col -= (*mb_শীর্ষ_off)(ptr, ptr + col);
	    }
	}
    }

    যদি (visual)
    {
	যখন (ptr[col] != NUL && দৈর্ঘ্য > 0
		&& !vim_isdigit(ptr[col])
		&& !(do_alpha && ASCII_ISALPHA(ptr[col])))
	{
	    পূর্ণসংখ্যা mb_len = mb_ptr2len(ptr + col);

	    col += mb_len;
	    দৈর্ঘ্য -= mb_len;
	}

	যদি (দৈর্ঘ্য == 0)
	    যাও theend;

	যদি (col > pos->col && ptr[col - 1] == '-'
		&& (!has_mbyte || !(*mb_শীর্ষ_off)(ptr, ptr + col - 1))
		&& !do_unsigned)
	{
	    যদি (do_blank && col >= 2 && !VIM_ISWHITE(ptr[col - 2]))
		blank_unsigned = TRUE;
	    অন্যথা
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
    যদি (!VIM_ISDIGIT(firstdigit) && !(do_alpha && ASCII_ISALPHA(firstdigit)))
    {
	beep_flush();
	যাও theend;
    }

    যদি (do_alpha && ASCII_ISALPHA(firstdigit))
    {
	// decrement or increment alphabetic character
	যদি (op_type == OP_NR_SUB)
	{
	    যদি (CharOrd(firstdigit) < Prenum1)
	    {
		যদি (SAFE_isupper(firstdigit))
		    firstdigit = 'A';
		অন্যথা
		    firstdigit = 'a';
	    }
	    অন্যথা
		firstdigit -= Prenum1;
	}
	অন্যথা
	{
	    যদি (26 - CharOrd(firstdigit) - 1 < Prenum1)
	    {
		যদি (SAFE_isupper(firstdigit))
		    firstdigit = 'Z';
		অন্যথা
		    firstdigit = 'z';
	    }
	    অন্যথা
		firstdigit += Prenum1;
	}
	curwin->w_cursor.col = col;
	যদি (!did_change)
	    startpos = curwin->w_cursor;
	did_change = TRUE;
	(শূন্য)del_char(FALSE);
	ins_char(firstdigit);
	endpos = curwin->w_cursor;
	curwin->w_cursor.col = col;
    }
    অন্যথা
    {
	char_u	*buf1;
	পূর্ণসংখ্যা	buf1len;
	char_u	buf2[NUMBUFLEN];
	পূর্ণসংখ্যা	buf2len;
	pos_T	save_pos;
	পূর্ণসংখ্যা	i;

	যদি (col > 0 && ptr[col - 1] == '-'
		&& (!has_mbyte ||
		    !(*mb_শীর্ষ_off)(ptr, ptr + col - 1))
		&& !visual
		&& !do_unsigned)
	{
	    যদি (do_blank && col >= 2 && !VIM_ISWHITE(ptr[col - 2]))
		blank_unsigned = TRUE;
	    অন্যথা
	    {
		// negative number
		--col;
		negative = TRUE;
	    }
	}

	// get the number value (unsigned)
	যদি (visual && VIsual_mode != 'V')
	    maxlen = (curbuf->b_visual.vi_curswant == MAXCOL
		    ? linelen - col : দৈর্ঘ্য);

	পূর্ণসংখ্যা overflow = FALSE;
	vim_str2nr(ptr + col, &pre, &দৈর্ঘ্য,
		0 + (do_bin ? STR2NR_BIN : 0)
		    + (do_oct ? STR2NR_OCT : 0)
		    + (do_hex ? STR2NR_HEX : 0),
		ফাঁকা, &n, maxlen, FALSE, &overflow);

	// ignore leading '-' for hex and octal and bin numbers
	যদি (pre && negative)
	{
	    ++col;
	    --দৈর্ঘ্য;
	    negative = FALSE;
	}
	// add or subtract
	subtract = FALSE;
	যদি (op_type == OP_NR_SUB)
	    subtract ^= TRUE;
	যদি (negative)
	    subtract ^= TRUE;

	oldn = n;
	যদি (!overflow)  // if number is too big don't add/subtract
	{
	    যদি (subtract)
		n -= (uvarnumber_T)Prenum1;
	    অন্যথা
		n += (uvarnumber_T)Prenum1;
	}

	// handle wraparound for decimal numbers
	যদি (!pre)
	{
	    যদি (subtract)
	    {
		যদি (n > oldn)
		{
		    n = 1 + (n ^ (uvarnumber_T)-1);
		    negative ^= TRUE;
		}
	    }
	    অন্যথা
	    {
		// add
		যদি (n < oldn)
		{
		    n = (n ^ (uvarnumber_T)-1);
		    negative ^= TRUE;
		}
	    }
	    যদি (n == 0)
		negative = FALSE;
	}

	যদি ((do_unsigned || blank_unsigned) && negative)
	{
	    যদি (subtract)
		// sticking at zero.
		n = (uvarnumber_T)0;
	    অন্যথা
		// sticking at 2^64 - 1.
		n = (uvarnumber_T)(-1);
	    negative = FALSE;
	}

	যদি (visual && !was_positive && !negative && col > 0)
	{
	    // need to remove the '-'
	    col--;
	    দৈর্ঘ্য++;
	}

	/*
	 * Delete the old number.
	 */
	curwin->w_cursor.col = col;
	যদি (!did_change)
	    startpos = curwin->w_cursor;
	did_change = TRUE;
	todel = দৈর্ঘ্য;
	c = gchar_cursor();
	/*
	 * Don't include the '-' in the length, only the length of the
	 * part after it is kept the same.
	 */
	যদি (c == '-')
	    --দৈর্ঘ্য;

	save_pos = curwin->w_cursor;
	জন্য (i = 0; i < todel; ++i)
	{
	    যদি (c < 0x100 && SAFE_isalpha(c))
	    {
		যদি (SAFE_isupper(c))
		    hexupper = TRUE;
		অন্যথা
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
	buf1 = alloc(দৈর্ঘ্য + NUMBUFLEN);
	যদি (buf1 == ফাঁকা)
	    যাও theend;
	ptr = buf1;
	যদি (negative && (!visual || was_positive))
	    *ptr++ = '-';
	যদি (pre)
	{
	    *ptr++ = '0';
	    --দৈর্ঘ্য;
	}
	যদি (pre == 'b' || pre == 'B' ||
	    pre == 'x' || pre == 'X')
	{
	    *ptr++ = pre;
	    --দৈর্ঘ্য;
	}

	/*
	 * Put the number characters in buf2[].
	 */
	যদি (pre == 'b' || pre == 'B')
	{
	    পূর্ণসংখ্যা bit = 0;
	    পূর্ণসংখ্যা bits = আকার(uvarnumber_T) * 8;

	    // leading zeros
	    জন্য (bit = bits; bit > 0; bit--)
		যদি ((n >> (bit - 1)) & 0x1) বিরতি;

	    জন্য (buf2len = 0; bit > 0 && buf2len < (NUMBUFLEN - 1); bit--)
		buf2[buf2len++] = ((n >> (bit - 1)) & 0x1) ? '1' : '0';

	    buf2[buf2len] = NUL;
	}
	অন্যথা যদি (pre == 0)
	    buf2len = vim_snprintf((অক্ষর *)buf2, NUMBUFLEN, "%llu", n);
	অন্যথা যদি (pre == '0')
	    buf2len = vim_snprintf((অক্ষর *)buf2, NUMBUFLEN, "%llo", n);
	অন্যথা যদি (pre && hexupper)
	    buf2len = vim_snprintf((অক্ষর *)buf2, NUMBUFLEN, "%llX", n);
	অন্যথা
	    buf2len = vim_snprintf((অক্ষর *)buf2, NUMBUFLEN, "%llx", n);
	দৈর্ঘ্য -= buf2len;

	/*
	 * Adjust number of zeros to the new number of digits, so the
	 * total length of the number remains the same.
	 * Don't do this when
	 * the result may look like an octal number.
	 */
	যদি (firstdigit == '0' && !(do_oct && pre == 0))
	    যখন (দৈর্ঘ্য-- > 0)
		*ptr++ = '0';
	*ptr = NUL;
	buf1len = (পূর্ণসংখ্যা)(ptr - buf1);

	STRCPY(buf1 + buf1len, buf2);
	buf1len += buf2len;

	// Insert just after the first character to be removed, so that any
	// text properties will be adjusted.  Then delete the old number
	// afterwards.
	save_pos = curwin->w_cursor;
	যদি (todel > 0)
	    inc_cursor();
	ins_str(buf1, (size_t)buf1len);		// insert the new number
	vim_free(buf1);

	// del_char() will also mark line needing displaying
	যদি (todel > 0)
	{
	    পূর্ণসংখ্যা bytes_after = ml_get_curline_len() - curwin->w_cursor.col;

	    // Delete the one character before the insert.
	    curwin->w_cursor = save_pos;
	    (শূন্য)del_char(FALSE);
	    curwin->w_cursor.col = ml_get_curline_len() - bytes_after;
	    --todel;
	}
	যখন (todel-- > 0)
	    (শূন্য)del_char(FALSE);

	endpos = curwin->w_cursor;
	যদি (did_change && curwin->w_cursor.col)
	    --curwin->w_cursor.col;
    }

    যদি (did_change && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// set the '[ and '] marks
	curbuf->b_op_start = startpos;
	curbuf->b_op_end = endpos;
	যদি (curbuf->b_op_end.col > 0)
	    --curbuf->b_op_end.col;
    }

theend:
    যদি (visual)
	curwin->w_cursor = save_cursor;
    অন্যথা যদি (did_change)
	curwin->w_set_curswant = true;
    অন্যথা যদি (virtual_active())
	curwin->w_cursor.coladd = save_coladd;

    ফেরত did_change;
}

    শূন্য
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
    স্থির varnumber_T
সারি_গুনতি_info(
    char_u	*সারি,
    varnumber_T	*wc,
    varnumber_T	*cc,
    varnumber_T	limit,
    পূর্ণসংখ্যা		eol_আকার)
{
    varnumber_T	i;
    varnumber_T	শব্দগুলি = 0;
    varnumber_T	chars = 0;
    পূর্ণসংখ্যা		is_word = 0;

    জন্য (i = 0; i < limit && সারি[i] != NUL; )
    {
	যদি (is_word)
	{
	    যদি (vim_isspace(সারি[i]))
	    {
		শব্দগুলি++;
		is_word = 0;
	    }
	}
	অন্যথা যদি (!vim_isspace(সারি[i]))
	    is_word = 1;
	++chars;
	i += (*mb_ptr2len)(সারি + i);
    }

    যদি (is_word)
	শব্দগুলি++;
    *wc += শব্দগুলি;

    // Add eol_size if the end of line was reached before hitting limit.
    যদি (i < limit && সারি[i] == NUL)
    {
	i += eol_আকার;
	chars += eol_আকার;
    }
    *cc += chars;
    ফেরত i;
}

/*
 * Give some info about the position of the cursor (for "g CTRL-G").
 * In Visual mode, give some info about the selected region.  (In this case,
 * the *_count_cursor variables store running totals for the selection.)
 * When "dict" is not NULL store the info there instead of showing it.
 */
    শূন্য
cursor_pos_info(dict_T *dict)
{
    char_u	*p;
    char_u	buf1[50];
    char_u	buf2[40];
    linenr_T	lnum;
    varnumber_T	byte_গুনতি = 0;
    varnumber_T	bom_গুনতি  = 0;
    varnumber_T	byte_গুনতি_cursor = 0;
    varnumber_T	char_গুনতি = 0;
    varnumber_T	char_গুনতি_cursor = 0;
    varnumber_T	word_গুনতি = 0;
    varnumber_T	word_গুনতি_cursor = 0;
    পূর্ণসংখ্যা		eol_আকার;
    varnumber_T	last_check = 100000L;
    long	সারি_গুনতি_selected = 0;
    pos_T	min_pos, max_pos;
    oparg_T	oparg;
    কাঠামো block_def	bd;

    /*
     * Compute the length of the file in characters.
     */
    যদি (curbuf->b_ml.ml_flags & ML_EMPTY)
    {
	যদি (dict == ফাঁকা)
	{
	    বার্তা(_(no_সারিগুলি_বার্তা));
	    ফেরত;
	}
    }
    অন্যথা
    {
	যদি (get_fileformat(curbuf) == EOL_DOS)
	    eol_আকার = 2;
	অন্যথা
	    eol_আকার = 1;

	যদি (VIsual_active)
	{
	    যদি (LT_POS(VIsual, curwin->w_cursor))
	    {
		min_pos = VIsual;
		max_pos = curwin->w_cursor;
	    }
	    অন্যথা
	    {
		min_pos = curwin->w_cursor;
		max_pos = VIsual;
	    }
	    যদি (*p_sel == 'e' && max_pos.col > 0)
		--max_pos.col;

	    যদি (VIsual_mode == Ctrl_V)
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
		যদি (curwin->w_curswant == MAXCOL)
		    oparg.end_vcol = MAXCOL;
		// Swap the start, end vcol if needed
		যদি (oparg.end_vcol < oparg.start_vcol)
		{
		    oparg.end_vcol += oparg.start_vcol;
		    oparg.start_vcol = oparg.end_vcol - oparg.start_vcol;
		    oparg.end_vcol -= oparg.start_vcol;
		}
	    }
	    সারি_গুনতি_selected = max_pos.lnum - min_pos.lnum + 1;
	}

	জন্য (lnum = 1; lnum <= curbuf->b_ml.ml_সারি_গুনতি; ++lnum)
	{
	    // Check for a CTRL-C every 100000 characters.
	    যদি (byte_গুনতি > last_check)
	    {
		ui_breakcheck();
		যদি (got_int)
		    ফেরত;
		last_check = byte_গুনতি + 100000L;
	    }

	    // Do extra processing for VIsual mode.
	    যদি (VIsual_active
		    && lnum >= min_pos.lnum && lnum <= max_pos.lnum)
	    {
		char_u	    *s = ফাঁকা;
		long	    len = 0L;

		নির্বাচন (VIsual_mode)
		{
		    ক্ষেত্র Ctrl_V:
			virtual_op = virtual_active();
			block_prep(&oparg, &bd, lnum, 0);
			virtual_op = MAYBE;
			s = bd.textstart;
			len = (long)bd.textlen;
			বিরতি;
		    ক্ষেত্র 'V':
			s = ml_get(lnum);
			len = MAXCOL;
			বিরতি;
		    ক্ষেত্র 'v':
			{
			    colnr_T start_col = (lnum == min_pos.lnum)
							   ? min_pos.col : 0;
			    colnr_T end_col = (lnum == max_pos.lnum)
				      ? max_pos.col - start_col + 1 : MAXCOL;

			    s = ml_get(lnum) + start_col;
			    len = end_col;
			}
			বিরতি;
		}
		যদি (s != ফাঁকা)
		{
		    byte_গুনতি_cursor += সারি_গুনতি_info(s, &word_গুনতি_cursor,
					   &char_গুনতি_cursor, len, eol_আকার);
		    যদি (lnum == curbuf->b_ml.ml_সারি_গুনতি
			    && !curbuf->b_p_eol
			    && (curbuf->b_p_bin || !curbuf->b_p_fixeol)
			    && (long)STRLEN(s) < len)
			byte_গুনতি_cursor -= eol_আকার;
		}
	    }
	    অন্যথা
	    {
		// In non-visual mode, check for the line the cursor is on
		যদি (lnum == curwin->w_cursor.lnum)
		{
		    word_গুনতি_cursor += word_গুনতি;
		    char_গুনতি_cursor += char_গুনতি;
		    byte_গুনতি_cursor = byte_গুনতি +
			সারি_গুনতি_info(ml_get(lnum),
				&word_গুনতি_cursor, &char_গুনতি_cursor,
				(varnumber_T)(curwin->w_cursor.col + 1),
				eol_আকার);
		}
	    }
	    // Add to the running totals
	    byte_গুনতি += সারি_গুনতি_info(ml_get(lnum), &word_গুনতি,
					 &char_গুনতি, (varnumber_T)MAXCOL,
					 eol_আকার);
	}

	// Correction for when last line doesn't have an EOL.
	যদি (!curbuf->b_p_eol && (curbuf->b_p_bin || !curbuf->b_p_fixeol))
	    byte_গুনতি -= eol_আকার;

	যদি (dict == ফাঁকা)
	{
	    যদি (VIsual_active)
	    {
		যদি (VIsual_mode == Ctrl_V && curwin->w_curswant < MAXCOL)
		{
		    getvcols(curwin, &min_pos, &max_pos,
						&min_pos.col, &max_pos.col, 0);
		    vim_snprintf((অক্ষর *)buf1, আকার(buf1), _("%ld Cols; "),
			    (long)(oparg.end_vcol - oparg.start_vcol + 1));
		}
		অন্যথা
		    buf1[0] = NUL;

		যদি (char_গুনতি_cursor == byte_গুনতি_cursor
						    && char_গুনতি == byte_গুনতি)
		    vim_snprintf((অক্ষর *)IObuff, IOSIZE,
			    _("Selected %s%ld of %ld Lines; %lld of %lld Words; %lld of %lld Bytes"),
			    buf1, সারি_গুনতি_selected,
			    (long)curbuf->b_ml.ml_সারি_গুনতি,
			    word_গুনতি_cursor,
			    word_গুনতি,
			    byte_গুনতি_cursor,
			    byte_গুনতি);
		অন্যথা
		    vim_snprintf((অক্ষর *)IObuff, IOSIZE,
			    _("Selected %s%ld of %ld Lines; %lld of %lld Words; %lld of %lld Chars; %lld of %lld Bytes"),
			    buf1, সারি_গুনতি_selected,
			    (long)curbuf->b_ml.ml_সারি_গুনতি,
			    word_গুনতি_cursor,
			    word_গুনতি,
			    char_গুনতি_cursor,
			    char_গুনতি,
			    byte_গুনতি_cursor,
			    byte_গুনতি);
	    }
	    অন্যথা
	    {
		p = ml_get_curline();
		validate_virtcol();
		col_print(buf1, আকার(buf1), (পূর্ণসংখ্যা)curwin->w_cursor.col + 1,
			(পূর্ণসংখ্যা)curwin->w_virtcol + 1);
		col_print(buf2, আকার(buf2), ml_get_curline_len(),
							   linetabsize_str(p));

		যদি (char_গুনতি_cursor == byte_গুনতি_cursor
			&& char_গুনতি == byte_গুনতি)
		    vim_snprintf((অক্ষর *)IObuff, IOSIZE,
			_("Col %s of %s; Line %ld of %ld; Word %lld of %lld; Byte %lld of %lld"),
			(অক্ষর *)buf1, (অক্ষর *)buf2,
			(long)curwin->w_cursor.lnum,
			(long)curbuf->b_ml.ml_সারি_গুনতি,
			word_গুনতি_cursor, word_গুনতি,
			byte_গুনতি_cursor, byte_গুনতি);
		অন্যথা
		    vim_snprintf((অক্ষর *)IObuff, IOSIZE,
			_("Col %s of %s; Line %ld of %ld; Word %lld of %lld; Char %lld of %lld; Byte %lld of %lld"),
			(অক্ষর *)buf1, (অক্ষর *)buf2,
			(long)curwin->w_cursor.lnum,
			(long)curbuf->b_ml.ml_সারি_গুনতি,
			word_গুনতি_cursor, word_গুনতি,
			char_গুনতি_cursor, char_গুনতি,
			byte_গুনতি_cursor, byte_গুনতি);
	    }
	}

	bom_গুনতি = bomb_আকার();
	যদি (dict == ফাঁকা && bom_গুনতি > 0)
	{
	    size_t len = STRLEN(IObuff);

	    vim_snprintf((অক্ষর *)IObuff + len, IOSIZE - len,
				 _("(+%lld for BOM)"), bom_গুনতি);
	}
	যদি (dict == ফাঁকা)
	{
	    // Don't shorten this message, the user asked for it.
	    p = p_shm;
	    p_shm = (char_u *)"";
	    বার্তা((অক্ষর *)IObuff);
	    p_shm = p;
	}
    }
#যদি defined(FEAT_EVAL)
    যদি (dict != ফাঁকা)
    {
	dict_add_সংখ্যা(dict, "words", word_গুনতি);
	dict_add_সংখ্যা(dict, "chars", char_গুনতি);
	dict_add_সংখ্যা(dict, "bytes", byte_গুনতি + bom_গুনতি);
	dict_add_সংখ্যা(dict, VIsual_active ? "visual_bytes" : "cursor_bytes",
		byte_গুনতি_cursor);
	dict_add_সংখ্যা(dict, VIsual_active ? "visual_chars" : "cursor_chars",
		char_গুনতি_cursor);
	dict_add_সংখ্যা(dict, VIsual_active ? "visual_words" : "cursor_words",
		word_গুনতি_cursor);
    }
#endif
}

/*
 * Handle indent and format operators and visual mode ":".
 */
    স্থির শূন্য
op_colon(oparg_T *oap)
{
    stuffcharReadbuff(':');
    যদি (oap->is_VIsual)
	stuffReadbuff((char_u *)"'<,'>");
    অন্যথা
    {
	// Make the range look nice, so it can be repeated.
	যদি (oap->start.lnum == curwin->w_cursor.lnum)
	    stuffcharReadbuff('.');
	অন্যথা
	    stuffnumReadbuff((long)oap->start.lnum);

#ifdef FEAT_FOLDING
	// When using !! on a closed fold the range ".!" works best to operate
	// on, it will be made the whole closed fold later.
	linenr_T endOfStartFold = oap->start.lnum;
	(শূন্য)hasFolding(oap->start.lnum, ফাঁকা, &endOfStartFold);
#endif
	যদি (oap->end.lnum != oap->start.lnum
#ifdef FEAT_FOLDING
		&& oap->end.lnum != endOfStartFold
#endif
	   )
	{
	    // Make it a range with the end line.
	    stuffcharReadbuff(',');
	    যদি (oap->end.lnum == curwin->w_cursor.lnum)
		stuffcharReadbuff('.');
	    অন্যথা যদি (oap->end.lnum == curbuf->b_ml.ml_সারি_গুনতি)
		stuffcharReadbuff('$');
	    অন্যথা যদি (oap->start.lnum == curwin->w_cursor.lnum
#ifdef FEAT_FOLDING
		    // do not use ".+number" for a closed fold, it would count
		    // folded lines twice
		    && !hasFolding(oap->end.lnum, ফাঁকা, ফাঁকা)
#endif
		    )
	    {
		stuffReadbuff((char_u *)".+");
		stuffnumReadbuff((long)oap->সারি_গুনতি - 1);
	    }
	    অন্যথা
		stuffnumReadbuff((long)oap->end.lnum);
	}
    }
    যদি (oap->op_type != OP_COLON)
	stuffReadbuff((char_u *)"!");
    যদি (oap->op_type == OP_INDENT)
    {
	যদি (*get_equalprg() == NUL)
	    stuffReadbuff((char_u *)"indent");
	অন্যথা
	    stuffReadbuff(get_equalprg());
	stuffReadbuff((char_u *)"\n");
    }
    অন্যথা যদি (oap->op_type == OP_FORMAT)
    {
	যদি (*curbuf->b_p_fp != NUL)
	    stuffReadbuff(curbuf->b_p_fp);
	অন্যথা যদি (*p_fp != NUL)
	    stuffReadbuff(p_fp);
	অন্যথা
	    stuffReadbuff((char_u *)"fmt");
	stuffReadbuff((char_u *)"\n']");
    }

    // do_cmdline() does the rest
}

// callback function for 'operatorfunc'
স্থির callback_T opfunc_cb;

/*
 * Process the 'operatorfunc' option value.
 * Returns OK or FAIL.
 */
    অক্ষর *
did_set_operatorfunc(optset_T *args UNUSED)
{
    যদি (option_set_callback_func(p_opfunc, &opfunc_cb) == FAIL)
	ফেরত e_invalid_argument;

    ফেরত ফাঁকা;
}

#যদি defined(EXITFREE)
    শূন্য
free_operatorfunc_option(শূন্য)
{
# ifdef FEAT_EVAL
    free_callback(&opfunc_cb);
# endif
}
#endif

#যদি defined(FEAT_EVAL)
/*
 * Mark the global 'operatorfunc' callback with "copyID" so that it is not
 * garbage collected.
 */
    পূর্ণসংখ্যা
set_ref_in_opfunc(পূর্ণসংখ্যা copyID UNUSED)
{
    পূর্ণসংখ্যা abort = FALSE;

    abort = set_ref_in_callback(&opfunc_cb, copyID);

    ফেরত abort;
}
#endif

/*
 * Handle the "g@" operator: call 'operatorfunc'.
 */
    স্থির শূন্য
op_function(oparg_T *oap UNUSED)
{
#ifdef FEAT_EVAL
    typval_T	argv[2];
    pos_T	orig_start = curbuf->b_op_start;
    pos_T	orig_end = curbuf->b_op_end;
    typval_T	rettv;

    যদি (*p_opfunc == NUL)
	emsg(_(e_operatorfunc_is_empty));
    অন্যথা
    {
	// Set '[ and '] marks to text to be operated on.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
	যদি (oap->motion_type != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_end);

	argv[0].v_type = VAR_STRING;
	যদি (oap->block_mode)
	    argv[0].vval.v_string = (char_u *)"block";
	অন্যথা যদি (oap->motion_type == MLINE)
	    argv[0].vval.v_string = (char_u *)"line";
	অন্যথা
	    argv[0].vval.v_string = (char_u *)"char";
	argv[1].v_type = VAR_UNKNOWN;

	// Reset virtual_op so that 'virtualedit' can be changed in the
	// function.
	পূর্ণসংখ্যা save_virtual_op = virtual_op;
	virtual_op = MAYBE;

	// Reset finish_op so that mode() returns the right value.
	পূর্ণসংখ্যা save_finish_op = finish_op;
	finish_op = FALSE;

	যদি (call_callback(&opfunc_cb, 0, &rettv, 1, argv) != FAIL)
	    clear_tv(&rettv);

	virtual_op = save_virtual_op;
	finish_op = save_finish_op;
	যদি (cmdmod.cmod_flags & CMOD_LOCKMARKS)
	{
	    curbuf->b_op_start = orig_start;
	    curbuf->b_op_end = orig_end;
	}
    }
#অন্যথা
    emsg(_(e_eval_feature_not_available));
#endif
}

/*
 * Calculate start/end virtual columns for operating in block mode.
 */
    স্থির শূন্য
get_op_vcol(
    oparg_T	*oap,
    colnr_T	redo_VIsual_vcol,
    পূর্ণসংখ্যা		initial)    // when TRUE adjust position for 'selectmode'
{
    colnr_T	    start, end;

    যদি (VIsual_mode != Ctrl_V
	    || (!initial && oap->end.col < curwin->w_width))
	ফেরত;

    oap->block_mode = TRUE;

    // prevent from moving onto a trail byte
    যদি (has_mbyte)
	mb_adjustpos(curwin->w_বাফার, &oap->end);

    getvvcol(curwin, &(oap->start), &oap->start_vcol, ফাঁকা, &oap->end_vcol, 0);

    যদি (!redo_VIsual_busy)
    {
	getvvcol(curwin, &(oap->end), &start, ফাঁকা, &end, 0);

	যদি (start < oap->start_vcol)
	    oap->start_vcol = start;
	যদি (end > oap->end_vcol)
	{
	    যদি (initial && *p_sel == 'e' && start >= 1
				    && start - 1 >= oap->end_vcol)
		oap->end_vcol = start - 1;
	    অন্যথা
		oap->end_vcol = end;
	}
    }

    // if '$' was used, get oap->end_vcol from longest line
    যদি (curwin->w_curswant == MAXCOL)
    {
	curwin->w_cursor.col = MAXCOL;
	oap->end_vcol = 0;
	জন্য (curwin->w_cursor.lnum = oap->start.lnum;
		curwin->w_cursor.lnum <= oap->end.lnum;
					++curwin->w_cursor.lnum)
	{
	    getvvcol(curwin, &curwin->w_cursor, ফাঁকা, ফাঁকা, &end, 0);
	    যদি (end > oap->end_vcol)
		oap->end_vcol = end;
	}
    }
    অন্যথা যদি (redo_VIsual_busy)
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
প্রকারলেখ কাঠামো {
    পূর্ণসংখ্যা		rv_mode;	// 'v', 'V', or Ctrl-V
    linenr_T	rv_সারি_গুনতি;	// number of lines
    colnr_T	rv_vcol;	// number of cols or end column
    long	rv_গুনতি;	// count for Visual operator
    পূর্ণসংখ্যা		rv_arg;		// extra argument
} redo_VIsual_T;

    স্থির পূর্ণসংখ্যা
is_ex_cmdchar(cmdarg_T *cap)
{
    ফেরত cap->cmdchar == ':'
	|| cap->cmdchar == K_COMMAND
	|| cap->cmdchar == K_SCRIPT_COMMAND;
}

/*
 * Handle an operator after Visual mode or when the movement is finished.
 * "gui_yank" is true when yanking text for the clipboard.
 */
    শূন্য
do_pending_operator(cmdarg_T *cap, পূর্ণসংখ্যা old_col, পূর্ণসংখ্যা gui_yank)
{
    oparg_T	*oap = cap->oap;
    pos_T	old_cursor;
    পূর্ণসংখ্যা		empty_region_error;
    পূর্ণসংখ্যা		restart_edit_save;
#ifdef FEAT_LINEBREAK
    পূর্ণসংখ্যা		lbr_saved = curwin->w_p_lbr;
#endif

    // The visual area is remembered for redo
    স্থির redo_VIsual_T   redo_VIsual = {NUL, 0, 0, 0,0};

    পূর্ণসংখ্যা		    include_সারি_break = FALSE;

#যদি defined(FEAT_CLIPBOARD)
    // Yank the visual area into the GUI selection register before we operate
    // on it and lose it forever.
    // Don't do it if a specific register was specified, so that ""x"*P works.
    // This could call do_pending_operator() recursively, but that's OK
    // because gui_yank will be TRUE for the nested call.
    যদি ((clip_star.available || clip_plus.available)
	    && oap->op_type != OP_NOP
	    && !gui_yank
	    && VIsual_active
	    && !redo_VIsual_busy
	    && oap->regname == 0)
	clip_auto_select();
#endif
    old_cursor = curwin->w_cursor;

    // If an operation is pending, handle it...
    যদি ((finish_op || VIsual_active) && oap->op_type != OP_NOP)
    {
	// Yank can be redone when 'y' is in 'cpoptions', but not when yanking
	// for the clipboard.
	পূর্ণসংখ্যা	redo_yank = vim_strchr(p_cpo, CPO_YANK) != ফাঁকা && !gui_yank;

#ifdef FEAT_LINEBREAK
	// Avoid a problem with unwanted linebreaks in block mode.
	(শূন্য)reset_lbr();
#endif
	oap->is_VIsual = VIsual_active;
	যদি (oap->motion_force == 'V')
	    oap->motion_type = MLINE;
	অন্যথা যদি (oap->motion_force == 'v')
	{
	    // If the motion was linewise, "inclusive" will not have been set.
	    // Use "exclusive" to be consistent.  Makes "dvj" work nice.
	    যদি (oap->motion_type == MLINE)
		oap->inclusive = FALSE;
	    // If the motion already was characterwise, toggle "inclusive"
	    অন্যথা যদি (oap->motion_type == MCHAR)
		oap->inclusive = !oap->inclusive;
	    oap->motion_type = MCHAR;
	}
	অন্যথা যদি (oap->motion_force == Ctrl_V)
	{
	    // Change line- or characterwise motion into Visual block mode.
	    যদি (!VIsual_active)
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
	যদি ((redo_yank || oap->op_type != OP_YANK)
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
	    যদি (cap->cmdchar == '/' || cap->cmdchar == '?') // was a search
	    {
		// If 'cpoptions' does not contain 'r', insert the search
		// pattern to really repeat the same command.
		যদি (vim_strchr(p_cpo, CPO_REDO) == ফাঁকা)
		    AppendToRedobuffLit(cap->searchbuf, -1);
		AppendToRedobuff(NL_STR);
	    }
	    অন্যথা যদি (is_ex_cmdchar(cap))
	    {
		// do_cmdline() has stored the first typed line in
		// "repeat_cmdline".  When several lines are typed repeating
		// won't be possible.
		যদি (repeat_cmdline == ফাঁকা)
		    ResetRedobuff();
		অন্যথা
		{
		    যদি (cap->cmdchar == ':')
			AppendToRedobuffLit(repeat_cmdline, -1);
		    অন্যথা
			AppendToRedobuffSpec(repeat_cmdline);
		    AppendToRedobuff(NL_STR);
		    VIM_CLEAR(repeat_cmdline);
		}
	    }
	}

	যদি (redo_VIsual_busy)
	{
	    // Redo of an operation on a Visual area. Use the same size from
	    // redo_VIsual.rv_line_count and redo_VIsual.rv_vcol.
	    oap->start = curwin->w_cursor;
	    curwin->w_cursor.lnum += redo_VIsual.rv_সারি_গুনতি - 1;
	    যদি (curwin->w_cursor.lnum > curbuf->b_ml.ml_সারি_গুনতি)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_সারি_গুনতি;
	    VIsual_mode = redo_VIsual.rv_mode;
	    যদি (redo_VIsual.rv_vcol == MAXCOL || VIsual_mode == 'v')
	    {
		যদি (VIsual_mode == 'v')
		{
		    যদি (redo_VIsual.rv_সারি_গুনতি <= 1)
		    {
			validate_virtcol();
			curwin->w_curswant =
				     curwin->w_virtcol + redo_VIsual.rv_vcol - 1;
		    }
		    অন্যথা
			curwin->w_curswant = redo_VIsual.rv_vcol;
		}
		অন্যথা
		{
		    curwin->w_curswant = MAXCOL;
		}
		coladvance(curwin->w_curswant);
	    }
	    cap->count0 = redo_VIsual.rv_গুনতি;
	    যদি (redo_VIsual.rv_গুনতি != 0)
		cap->count1 = redo_VIsual.rv_গুনতি;
	    অন্যথা
		cap->count1 = 1;
	}
	অন্যথা যদি (VIsual_active)
	{
	    যদি (!gui_yank)
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
	    যদি (VIsual_select && VIsual_mode == 'V'
					    && cap->oap->op_type != OP_DELETE)
	    {
		যদি (LT_POS(VIsual, curwin->w_cursor))
		{
		    VIsual.col = 0;
		    curwin->w_cursor.col = ml_get_len(curwin->w_cursor.lnum);
		}
		অন্যথা
		{
		    curwin->w_cursor.col = 0;
		    VIsual.col = ml_get_len(VIsual.lnum);
		}
		VIsual_mode = 'v';
	    }
	    // If 'selection' is "exclusive", backup one character for
	    // charwise selections.
	    অন্যথা যদি (VIsual_mode == 'v')
		include_সারি_break = unadjust_for_sel();

	    oap->start = VIsual;
	    যদি (VIsual_mode == 'V')
	    {
		oap->start.col = 0;
		oap->start.coladd = 0;
	    }
	}

	// Set oap->start to the first position of the operated text, oap->end
	// to the end of the operated text.  w_cursor is equal to oap->start.
	যদি (LT_POS(oap->start, curwin->w_cursor))
	{
#ifdef FEAT_FOLDING
	    // Include folded lines completely.
	    যদি (!VIsual_active)
	    {
		যদি (hasFolding(oap->start.lnum, &oap->start.lnum, ফাঁকা))
		    oap->start.col = 0;
		যদি ((curwin->w_cursor.col > 0 || oap->inclusive
						  || oap->motion_type == MLINE)
			&& hasFolding(curwin->w_cursor.lnum, ফাঁকা,
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
	অন্যথা
	{
#ifdef FEAT_FOLDING
	    // Include folded lines completely.
	    যদি (!VIsual_active && oap->motion_type == MLINE)
	    {
		যদি (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum,
									ফাঁকা))
		    curwin->w_cursor.col = 0;
		যদি (hasFolding(oap->start.lnum, ফাঁকা, &oap->start.lnum))
		    oap->start.col = ml_get_len(oap->start.lnum);
	    }
#endif
	    oap->end = oap->start;
	    oap->start = curwin->w_cursor;
	}

	// Just in case lines were deleted that make the position invalid.
	check_pos(curwin->w_বাফার, &oap->end);
	oap->সারি_গুনতি = oap->end.lnum - oap->start.lnum + 1;

	// Set "virtual_op" before resetting VIsual_active.
	virtual_op = virtual_active();

	যদি (VIsual_active || redo_VIsual_busy)
	{
	    get_op_vcol(oap, redo_VIsual.rv_vcol, TRUE);

	    যদি (!redo_VIsual_busy && !gui_yank)
	    {
		// Prepare to reselect and redo Visual: this is based on the
		// size of the Visual text
		resel_VIsual_mode = VIsual_mode;
		যদি (curwin->w_curswant == MAXCOL)
		    resel_VIsual_vcol = MAXCOL;
		অন্যথা
		{
		    যদি (VIsual_mode != Ctrl_V)
			getvvcol(curwin, &(oap->end),
						ফাঁকা, ফাঁকা, &oap->end_vcol, 0);
		    যদি (VIsual_mode == Ctrl_V || oap->সারি_গুনতি <= 1)
		    {
			যদি (VIsual_mode != Ctrl_V)
			    getvvcol(curwin, &(oap->start),
					      &oap->start_vcol, ফাঁকা, ফাঁকা, 0);
			resel_VIsual_vcol = oap->end_vcol - oap->start_vcol + 1;
		    }
		    অন্যথা
			resel_VIsual_vcol = oap->end_vcol;
		}
		resel_VIsual_সারি_গুনতি = oap->সারি_গুনতি;
	    }

	    // can't redo yank (unless 'y' is in 'cpoptions') and ":"
	    যদি ((redo_yank || oap->op_type != OP_YANK)
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
		যদি (cap->cmdchar == 'g' && (cap->nchar == 'n'
							|| cap->nchar == 'N'))
		    prep_redo(oap->regname, cap->count0,
			    get_op_char(oap->op_type),
			    get_extra_op_char(oap->op_type),
			    oap->motion_force, cap->cmdchar, cap->nchar);
		অন্যথা যদি (!is_ex_cmdchar(cap))
		{
		    পূর্ণসংখ্যা opchar = get_op_char(oap->op_type);
		    পূর্ণসংখ্যা extra_opchar = get_extra_op_char(oap->op_type);
		    পূর্ণসংখ্যা nchar = oap->op_type == OP_REPLACE ? cap->nchar : NUL;

		    // reverse what nv_replace() did
		    যদি (nchar == REPLACE_CR_NCHAR)
			nchar = CAR;
		    অন্যথা যদি (nchar == REPLACE_NL_NCHAR)
			nchar = NL;

		    যদি (opchar == 'g' && extra_opchar == '@')
			// also repeat the count for 'operatorfunc'
			prep_redo_num2(oap->regname, 0L, NUL, 'v',
				     cap->count0, opchar, extra_opchar, nchar);
		    অন্যথা
			prep_redo(oap->regname, 0L, NUL, 'v',
						  opchar, extra_opchar, nchar);
		}
		যদি (!redo_VIsual_busy)
		{
		    redo_VIsual.rv_mode = resel_VIsual_mode;
		    redo_VIsual.rv_vcol = resel_VIsual_vcol;
		    redo_VIsual.rv_সারি_গুনতি = resel_VIsual_সারি_গুনতি;
		    redo_VIsual.rv_গুনতি = cap->count0;
		    redo_VIsual.rv_arg = cap->arg;
		}
	    }

	    // oap->inclusive defaults to TRUE.
	    // If oap->end is on a NUL (empty line) oap->inclusive becomes
	    // FALSE.  This makes "d}P" and "v}dP" work the same.
	    যদি (oap->motion_force == NUL || oap->motion_type == MLINE)
		oap->inclusive = TRUE;
	    যদি (VIsual_mode == 'V')
		oap->motion_type = MLINE;
	    অন্যথা
	    {
		oap->motion_type = MCHAR;
		যদি (VIsual_mode != Ctrl_V && *ml_get_pos(&(oap->end)) == NUL
			&& (include_সারি_break || !virtual_op))
		{
		    oap->inclusive = FALSE;
		    // Try to include the newline, unless it's an operator
		    // that works on lines only.
		    যদি (*p_sel != 'o'
			    && !op_on_সারিগুলি(oap->op_type)
			    && oap->end.lnum < curbuf->b_ml.ml_সারি_গুনতি)
		    {
			++oap->end.lnum;
			oap->end.col = 0;
			oap->end.coladd = 0;
			++oap->সারি_গুনতি;
		    }
		}
	    }

	    redo_VIsual_busy = FALSE;

	    // Switch Visual off now, so screen updating does
	    // not show inverted text when the screen is redrawn.
	    // With OP_YANK and sometimes with OP_COLON and OP_FILTER there is
	    // no screen redraw, so it is done here to remove the inverted
	    // part.
	    যদি (!gui_yank)
	    {
		VIsual_active = FALSE;
		setmouse();
		mouse_dragging = 0;
		may_clear_cmdline();
		যদি ((oap->op_type == OP_YANK
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
	যদি (has_mbyte && oap->inclusive)
	{
	    পূর্ণসংখ্যা		l;

	    l = (*mb_ptr2len)(ml_get_pos(&oap->end));
	    যদি (l > 1)
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
				&& vim_strchr(p_cpo, CPO_EMPTYREGION) != ফাঁকা);

	// Force a redraw when operating on an empty Visual region, when
	// 'modifiable is off or creating a fold.
	যদি (oap->is_VIsual && (oap->empty || !curbuf->b_p_ma
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
	যদি (	   oap->motion_type == MCHAR
		&& oap->inclusive == FALSE
		&& !(cap->retval & CA_NO_ADJ_OP_END)
		&& oap->end.col == 0
		&& (!oap->is_VIsual || *p_sel == 'o')
		&& !oap->block_mode
		&& oap->সারি_গুনতি > 1)
	{
	    oap->end_adjusted = TRUE;	    // remember that we did this
	    --oap->সারি_গুনতি;
	    --oap->end.lnum;
	    যদি (inindent(0))
		oap->motion_type = MLINE;
	    অন্যথা
	    {
		oap->end.col = ml_get_len(oap->end.lnum);
		যদি (oap->end.col)
		{
		    --oap->end.col;
		    oap->inclusive = TRUE;
		}
	    }
	}
	অন্যথা
	    oap->end_adjusted = FALSE;

	নির্বাচন (oap->op_type)
	{
	ক্ষেত্র OP_LSHIFT:
	ক্ষেত্র OP_RSHIFT:
	    op_shift(oap, TRUE, oap->is_VIsual ? (পূর্ণসংখ্যা)cap->count1 : 1);
	    auto_format(FALSE, TRUE);
	    বিরতি;

	ক্ষেত্র OP_JOIN_NS:
	ক্ষেত্র OP_JOIN:
	    যদি (oap->সারি_গুনতি < 2)
		oap->সারি_গুনতি = 2;
	    যদি (curwin->w_cursor.lnum + oap->সারি_গুনতি - 1 >
						   curbuf->b_ml.ml_সারি_গুনতি)
		beep_flush();
	    অন্যথা
	    {
		(শূন্য)do_join(oap->সারি_গুনতি, oap->op_type == OP_JOIN,
							    TRUE, TRUE, TRUE);
		auto_format(FALSE, TRUE);
	    }
	    বিরতি;

	ক্ষেত্র OP_DELETE:
	    VIsual_reselect = FALSE;	    // don't reselect now
	    যদি (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    অন্যথা
	    {
		(শূন্য)op_delete(oap);
		// save cursor line for undo if it wasn't saved yet
		যদি (oap->motion_type == MLINE && has_format_option(FO_AUTO)
						      && u_save_cursor() == OK)
		    auto_format(FALSE, TRUE);
	    }
	    বিরতি;

	ক্ষেত্র OP_YANK:
	    যদি (empty_region_error)
	    {
		যদি (!gui_yank)
		{
		    vim_beep(BO_OPER);
		    CancelRedo();
		}
	    }
	    অন্যথা
	    {
#ifdef FEAT_LINEBREAK
		restore_lbr(lbr_saved);
#endif
		oap->excl_tr_ws = cap->cmdchar == 'z';
		(শূন্য)op_yank(oap, FALSE, !gui_yank);
	    }
	    check_cursor_col();
	    বিরতি;

	ক্ষেত্র OP_CHANGE:
	    VIsual_reselect = FALSE;	    // don't reselect now
	    যদি (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    অন্যথা
	    {
		// This is a new edit command, not a restart.  Need to
		// remember it to make 'insertmode' work with mappings for
		// Visual mode.  But do this only once and not when typed and
		// 'insertmode' isn't set.
		যদি (p_im || !চাবি_Typed)
		    restart_edit_save = restart_edit;
		অন্যথা
		    restart_edit_save = 0;
		restart_edit = 0;
#ifdef FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#endif
		// trigger TextChangedI
		curbuf->b_last_changedtick_i = CHANGEDTICK(curbuf);

		যদি (op_change(oap))	// will call edit()
		    cap->retval |= CA_COMMAND_BUSY;
		যদি (restart_edit == 0)
		    restart_edit = restart_edit_save;
	    }
	    বিরতি;

	ক্ষেত্র OP_FILTER:
	    যদি (vim_strchr(p_cpo, CPO_FILTER) != ফাঁকা)
		AppendToRedobuff((char_u *)"!\r");  // use any last used !cmd
	    অন্যথা
		bangredo = TRUE;    // do_bang() will put cmd in redo buffer
	    // FALLTHROUGH

	ক্ষেত্র OP_INDENT:
	ক্ষেত্র OP_COLON:

	    // If 'equalprg' is empty, do the indenting internally.
	    যদি (oap->op_type == OP_INDENT && *get_equalprg() == NUL)
	    {
		যদি (curbuf->b_p_lisp)
		{
#ifdef FEAT_EVAL
		    যদি (use_indentexpr_for_lisp())
			op_reindent(oap, get_রাশিমালা_indent);
		    অন্যথা
#endif
			op_reindent(oap, get_lisp_indent);
		    বিরতি;
		}
		op_reindent(oap,
#ifdef FEAT_EVAL
			*curbuf->b_p_inde != NUL ? get_রাশিমালা_indent :
#endif
			    get_c_indent);
		বিরতি;
	    }

	    op_colon(oap);
	    বিরতি;

	ক্ষেত্র OP_TILDE:
	ক্ষেত্র OP_UPPER:
	ক্ষেত্র OP_LOWER:
	ক্ষেত্র OP_ROT13:
	    যদি (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    অন্যথা
		op_tilde(oap);
	    check_cursor_col();
	    বিরতি;

	ক্ষেত্র OP_FORMAT:
#যদি defined(FEAT_EVAL)
	    যদি (*curbuf->b_p_fex != NUL)
		op_formatexpr(oap);	// use expression
	    অন্যথা
#endif
	    {
		যদি (*p_fp != NUL || *curbuf->b_p_fp != NUL)
		    op_colon(oap);		// use external command
		অন্যথা
		    op_format(oap, FALSE);	// use internal function
	    }
	    বিরতি;
	ক্ষেত্র OP_FORMAT2:
	    op_format(oap, TRUE);	// use internal function
	    বিরতি;

	ক্ষেত্র OP_FUNCTION:
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
		বিরতি;
	    }

	ক্ষেত্র OP_ঢোকাও:
	ক্ষেত্র OP_APPEND:
	    VIsual_reselect = FALSE;	// don't reselect now
	    যদি (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    অন্যথা
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

		op_ঢোকাও(oap, cap->count1);
#ifdef FEAT_LINEBREAK
		// Reset linebreak, so that formatting works correctly.
		(শূন্য)reset_lbr();
#endif

		// TODO: when inserting in several lines, should format all
		// the lines.
		auto_format(FALSE, TRUE);

		যদি (restart_edit == 0)
		    restart_edit = restart_edit_save;
		অন্যথা
		    cap->retval |= CA_COMMAND_BUSY;
	    }
	    বিরতি;

	ক্ষেত্র OP_REPLACE:
	    VIsual_reselect = FALSE;	// don't reselect now
	    যদি (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    অন্যথা
	    {
#ifdef FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#endif
		op_replace(oap, cap->nchar);
	    }
	    বিরতি;

#ifdef FEAT_FOLDING
	ক্ষেত্র OP_FOLD:
	    VIsual_reselect = FALSE;	// don't reselect now
	    foldCreate(oap->start.lnum, oap->end.lnum);
	    বিরতি;

	ক্ষেত্র OP_FOLDOPEN:
	ক্ষেত্র OP_FOLDOPENREC:
	ক্ষেত্র OP_FOLDCLOSE:
	ক্ষেত্র OP_FOLDCLOSEREC:
	    VIsual_reselect = FALSE;	// don't reselect now
	    opFoldRange(oap->start.lnum, oap->end.lnum,
		    oap->op_type == OP_FOLDOPEN
					    || oap->op_type == OP_FOLDOPENREC,
		    oap->op_type == OP_FOLDOPENREC
					  || oap->op_type == OP_FOLDCLOSEREC,
					  oap->is_VIsual);
	    বিরতি;

	ক্ষেত্র OP_FOLDDEL:
	ক্ষেত্র OP_FOLDDELREC:
	    VIsual_reselect = FALSE;	// don't reselect now
	    deleteFold(oap->start.lnum, oap->end.lnum,
			       oap->op_type == OP_FOLDDELREC, oap->is_VIsual);
	    বিরতি;
#endif
	ক্ষেত্র OP_NR_ADD:
	ক্ষেত্র OP_NR_SUB:
	    যদি (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    অন্যথা
	    {
		VIsual_active = TRUE;
#ifdef FEAT_LINEBREAK
		restore_lbr(lbr_saved);
#endif
		op_addsub(oap, cap->count1, redo_VIsual.rv_arg);
		VIsual_active = FALSE;
	    }
	    check_cursor_col();
	    বিরতি;
	পূর্বনির্ধারিত:
	    clearopbeep(oap);
	}
	virtual_op = MAYBE;
	যদি (!gui_yank)
	{
	    // if 'sol' not set, go back to old column for some commands
	    যদি (!p_sol && oap->motion_type == MLINE && !oap->end_adjusted
		    && (oap->op_type == OP_LSHIFT || oap->op_type == OP_RSHIFT
						|| oap->op_type == OP_DELETE))
	    {
#ifdef FEAT_LINEBREAK
		(শূন্য)reset_lbr();
#endif
		coladvance(curwin->w_curswant = old_col);
	    }
	}
	অন্যথা
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
    স্থির শূন্য
pbyte(pos_T lp, পূর্ণসংখ্যা c)
{
    char_u *p = ml_get_buf(curbuf, lp.lnum, TRUE);
    পূর্ণসংখ্যা	len = curbuf->b_ml.ml_সারি_len;

    // safety check
    যদি (lp.col >= len)
	lp.col = (len > 1 ? len - 2 : 0);
    *(p + lp.col) = c;
}
