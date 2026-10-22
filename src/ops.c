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

#અન્તર્ભાવ "vim.h"

સ્થિર શૂન્ય shift_block(oparg_T *oap, પૂર્ણાંક amount);
સ્થિર શૂન્ય	mb_adjust_opend(oparg_T *oap);
સ્થિર પૂર્ણાંક	do_addsub(પૂર્ણાંક op_type, pos_T *pos, પૂર્ણાંક length, linenr_T Prenum1);
સ્થિર શૂન્ય	pbyte(pos_T lp, પૂર્ણાંક c);
#define PBYTE(lp, c) pbyte(lp, c)


// Flags for third item in "opchars".
#define OPF_લીટીઓ  1	// operator always works on lines
#define OPF_CHANGE 2	// operator changes text

/*
 * The names of operators.
 * IMPORTANT: Index must correspond with defines in vim.h!!!
 * The third field holds OPF_ flags.
 */
સ્થિર અચલ અક્ષર opchars[][3] =
{
    {NUL, NUL, 0},			// OP_NOP
    {'d', NUL, OPF_CHANGE},		// OP_DELETE
    {'y', NUL, 0},			// OP_YANK
    {'c', NUL, OPF_CHANGE},		// OP_CHANGE
    {'<', NUL, OPF_લીટીઓ | OPF_CHANGE},	// OP_LSHIFT
    {'>', NUL, OPF_લીટીઓ | OPF_CHANGE},	// OP_RSHIFT
    {'!', NUL, OPF_લીટીઓ | OPF_CHANGE},	// OP_FILTER
    {'g', '~', OPF_CHANGE},		// OP_TILDE
    {'=', NUL, OPF_લીટીઓ | OPF_CHANGE},	// OP_INDENT
    {'g', 'q', OPF_લીટીઓ | OPF_CHANGE},	// OP_FORMAT
    {':', NUL, OPF_લીટીઓ},		// OP_COLON
    {'g', 'U', OPF_CHANGE},		// OP_UPPER
    {'g', 'u', OPF_CHANGE},		// OP_LOWER
    {'J', NUL, OPF_લીટીઓ | OPF_CHANGE},	// DO_JOIN
    {'g', 'J', OPF_લીટીઓ | OPF_CHANGE},	// DO_JOIN_NS
    {'g', '?', OPF_CHANGE},		// OP_ROT13
    {'r', NUL, OPF_CHANGE},		// OP_REPLACE
    {'I', NUL, OPF_CHANGE},		// OP_INSERT
    {'A', NUL, OPF_CHANGE},		// OP_APPEND
    {'z', 'f', OPF_લીટીઓ},		// OP_FOLD
    {'z', 'o', OPF_લીટીઓ},		// OP_FOLDOPEN
    {'z', 'O', OPF_લીટીઓ},		// OP_FOLDOPENREC
    {'z', 'c', OPF_લીટીઓ},		// OP_FOLDCLOSE
    {'z', 'C', OPF_લીટીઓ},		// OP_FOLDCLOSEREC
    {'z', 'd', OPF_લીટીઓ},		// OP_FOLDDEL
    {'z', 'D', OPF_લીટીઓ},		// OP_FOLDDELREC
    {'g', 'w', OPF_લીટીઓ | OPF_CHANGE},	// OP_FORMAT2
    {'g', '@', OPF_CHANGE},		// OP_FUNCTION
    {Ctrl_A, NUL, OPF_CHANGE},		// OP_NR_ADD
    {Ctrl_X, NUL, OPF_CHANGE},		// OP_NR_SUB
};

/*
 * Translate a command name into an operator type.
 * Must only be called with a valid operator name!
 */
    પૂર્ણાંક
get_op_type(પૂર્ણાંક char1, પૂર્ણાંક char2)
{
    પૂર્ણાંક		i;

    જો (char1 == 'r')		// ignore second character
	પાછા OP_REPLACE;
    જો (char1 == '~')		// when tilde is an operator
	પાછા OP_TILDE;
    જો (char1 == 'g' && char2 == Ctrl_A)	// add
	પાછા OP_NR_ADD;
    જો (char1 == 'g' && char2 == Ctrl_X)	// subtract
	પાછા OP_NR_SUB;
    જો (char1 == 'z' && char2 == 'y')	// OP_YANK
	પાછા OP_YANK;
    માટે (i = 0; ; ++i)
    {
	જો (opchars[i][0] == char1 && opchars[i][1] == char2)
	    તોડો;
	જો (i == (પૂર્ણાંક)ARRAY_LENGTH(opchars) - 1)
	{
	    internal_error("get_op_type()");
	    તોડો;
	}
    }
    પાછા i;
}

/*
 * Return TRUE if operator "op" always works on whole lines.
 */
    સ્થિર પૂર્ણાંક
op_on_લીટીઓ(પૂર્ણાંક op)
{
    પાછા opchars[op][2] & OPF_લીટીઓ;
}

#જો defined(FEAT_JOB_CHANNEL)
/*
 * Return TRUE if operator "op" changes text.
 */
    પૂર્ણાંક
op_is_change(પૂર્ણાંક op)
{
    પાછા opchars[op][2] & OPF_CHANGE;
}
#endif

/*
 * Get first operator command character.
 * Returns 'g' or 'z' if there is another command character.
 */
    પૂર્ણાંક
get_op_char(પૂર્ણાંક optype)
{
    પાછા opchars[optype][0];
}

/*
 * Get second operator command character.
 */
    પૂર્ણાંક
get_extra_op_char(પૂર્ણાંક optype)
{
    પાછા opchars[optype][1];
}

/*
 * op_shift - handle a shift operation
 */
    શૂન્ય
op_shift(oparg_T *oap, પૂર્ણાંક curs_ટોચ, પૂર્ણાંક amount)
{
    long	    i;
    પૂર્ણાંક		    first_char;
    પૂર્ણાંક		    block_col = 0;

    જો (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	પાછા;

    જો (oap->block_mode)
	block_col = curwin->w_cursor.col;

    માટે (i = oap->લીટી_ગણતરી; --i >= 0; )
    {
	first_char = *ml_get_curline();
	જો (first_char == NUL)				// empty line
	    curwin->w_cursor.col = 0;
	નહીંતર જો (oap->block_mode)
	    shift_block(oap, amount);
	નહીંતર
	    // Move the line right if it doesn't start with '#', 'smartindent'
	    // isn't set or 'cindent' isn't set or '#' isn't in 'cino'.
	    જો (first_char != '#' || !preprocs_ડાબું())
		shift_લીટી(oap->op_type == OP_LSHIFT, p_sr, amount, FALSE);
	++curwin->w_cursor.lnum;
    }

    changed_લીટીઓ(oap->start.lnum, 0, oap->end.lnum + 1, 0L);
    જો (oap->block_mode)
    {
	curwin->w_cursor.lnum = oap->start.lnum;
	curwin->w_cursor.col = block_col;
    }
    નહીંતર જો (curs_ટોચ)	    // put cursor on first line, for ">>"
    {
	curwin->w_cursor.lnum = oap->start.lnum;
	beginline(BL_SOL | BL_FIX);   // shift_line() may have set cursor.col
    }
    નહીંતર
	--curwin->w_cursor.lnum;	// put cursor on last line, for ":>"

#ifdef FEAT_FOLDING
    // The cursor line is not in a closed fold
    foldOpenCursor();
#endif


    જો (oap->લીટી_ગણતરી > p_report)
    {
	અક્ષર	    *op;
	અક્ષર	    *સંદેશ_લીટી_single;
	અક્ષર	    *સંદેશ_લીટી_plural;

	જો (oap->op_type == OP_RSHIFT)
	    op = ">";
	નહીંતર
	    op = "<";
	સંદેશ_લીટી_single = NGETTEXT("%ld line %sed %d time",
					     "%ld line %sed %d times", amount);
	સંદેશ_લીટી_plural = NGETTEXT("%ld lines %sed %d time",
					    "%ld lines %sed %d times", amount);
	vim_snprintf((અક્ષર *)IObuff, IOSIZE,
		NGETTEXT(સંદેશ_લીટી_single, સંદેશ_લીટી_plural, oap->લીટી_ગણતરી),
		oap->લીટી_ગણતરી, op, amount);
	સંદેશ_attr_keep((અક્ષર *)IObuff, 0, TRUE);
    }

    જો ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end.lnum = oap->end.lnum;
	curbuf->b_op_end.col = ml_get_len(oap->end.lnum);
	જો (curbuf->b_op_end.col > 0)
	    --curbuf->b_op_end.col;
    }
}

#ifdef FEAT_VARTABS
/*
 * Return the tabstop width at the index of the variable tabstop array.  If an
 * index greater than the length of the array is given, the last tabstop width
 * in the array is returned.
 */
    સ્થિર પૂર્ણાંક
get_vts(પૂર્ણાંક *vts_array, પૂર્ણાંક index)
{
    પૂર્ણાંક	ts;

    જો (index < 1)
	ts = 0;
    નહીંતર જો (index <= vts_array[0])
	ts = vts_array[index];
    નહીંતર
	ts = vts_array[vts_array[0]];

    પાછા ts;
}

/*
 * Return the sum of all the tabstops through the index-th.
 */
    સ્થિર પૂર્ણાંક
get_vts_sum(પૂર્ણાંક *vts_array, પૂર્ણાંક index)
{
    પૂર્ણાંક	sum = 0;
    પૂર્ણાંક	i;

    // Perform the summation for indices within the actual array.
    માટે (i = 1; i <= index && i <= vts_array[0]; i++)
	sum += vts_array[i];

    // Add tabstops whose indices exceed the actual array.
    જો (i <= index)
	sum += vts_array[vts_array[0]] * (index - vts_array[0]);

    પાછા sum;
}
#endif

    સ્થિર vimlong_T
get_new_sw_indent(
    પૂર્ણાંક		ડાબું,		// TRUE if shift is to the left
    પૂર્ણાંક		round,		// TRUE if new indent is to be to a tabstop
    vimlong_T	amount,		// Number of shifts
    vimlong_T	sw_val)
{
    vimlong_T	ગણતરી = get_indent();
    vimlong_T	i, j;

    જો (round)			// round off indent
    {
	i = trim_to_int(ગણતરી) / sw_val;	// number of 'shiftwidth' rounded down
	j = trim_to_int(ગણતરી) % sw_val;	// extra spaces
	જો (j && ડાબું)		// first remove extra spaces
	    --amount;
	જો (ડાબું)
	{
	    i -= amount;
	    જો (i < 0)
		i = 0;
	}
	નહીંતર
	    i += amount;
	ગણતરી = i * sw_val;
    }
    નહીંતર			// original vi indent
    {
	જો (ડાબું)
	{
	    ગણતરી -= sw_val * amount;
	    જો (ગણતરી < 0)
		ગણતરી = 0;
	}
	નહીંતર
	    ગણતરી += sw_val * amount;
    }

    પાછા ગણતરી;
}

#ifdef FEAT_VARTABS
    સ્થિર vimlong_T
get_new_vts_indent(
    પૂર્ણાંક	ડાબું,			// TRUE if shift is to the left
    પૂર્ણાંક	round,			// TRUE if new indent is to be to a tabstop
    પૂર્ણાંક	amount,			// Number of shifts
    પૂર્ણાંક	*vts_array)
{
    vimlong_T	indent = get_indent();
    પૂર્ણાંક		vtsi = 0;
    પૂર્ણાંક		vts_indent = 0;
    પૂર્ણાંક		ts = 0;		// Silence uninitialized variable warning.
    પૂર્ણાંક		offset;		// Extra indent spaces to the right of the
				// tabstop

    // Find the tabstop at or to the left of the current indent.
    જ્યારે (vts_indent <= indent)
    {
	vtsi++;
	ts = get_vts(vts_array, vtsi);
	vts_indent += ts;
    }
    vts_indent -= ts;
    vtsi--;

    offset = indent - vts_indent;

    જો (round)
    {
	જો (ડાબું)
	{
	    જો (offset == 0)
		indent = get_vts_sum(vts_array, vtsi - amount);
	    નહીંતર
		indent = get_vts_sum(vts_array, vtsi - (amount - 1));
	}
	નહીંતર
	    indent = get_vts_sum(vts_array, vtsi + amount);
    }
    નહીંતર
    {
	જો (ડાબું)
	{
	    જો (amount > vtsi)
		indent = 0;
	    નહીંતર
		indent = get_vts_sum(vts_array, vtsi - amount) + offset;
	}
	નહીંતર
	    indent = get_vts_sum(vts_array, vtsi + amount) + offset;
    }

    પાછા indent;
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
    શૂન્ય
shift_લીટી(
    પૂર્ણાંક	ડાબું,			// TRUE if shift is to the left
    પૂર્ણાંક	round,			// TRUE if new indent is to be to a tabstop
    પૂર્ણાંક	amount,			// Number of shifts
    પૂર્ણાંક	call_changed_bytes)	// call changed_bytes()
{
    vimlong_T	ગણતરી;
    long	sw_val = curbuf->b_p_sw;
    long	ts_val = curbuf->b_p_ts;
#ifdef FEAT_VARTABS
    પૂર્ણાંક		*vts_array = curbuf->b_p_vts_array;
#endif

    જો (sw_val != 0)
	// 'shiftwidth' is not zero; use it as the shift size.
	ગણતરી = get_new_sw_indent(ડાબું, round, amount, sw_val);
    નહીંતર
#ifdef FEAT_VARTABS
	જો ((vts_array == ખાલી) || (vts_array[0] == 0))
#endif
    {
	// 'shiftwidth is zero and 'vartabstop' is empty; use 'tabstop' as the
	// shift size.
	ગણતરી = get_new_sw_indent(ડાબું, round, amount, ts_val);
    }
#ifdef FEAT_VARTABS
    નહીંતર
    {
	// 'shiftwidth is zero and 'vartabstop' is defined; use 'vartabstop'
	// to determine the new indent.
	ગણતરી = get_new_vts_indent(ડાબું, round, amount, vts_array);
    }
#endif

    // Set new indent
    જો (State & VREPLACE_FLAG)
	change_indent(INDENT_SET, trim_to_int(ગણતરી), FALSE, NUL, call_changed_bytes);
    નહીંતર
	(શૂન્ય)set_indent(trim_to_int(ગણતરી), call_changed_bytes ? SIN_CHANGED : 0);
}

/*
 * Shift one line of the current block one shiftwidth right or left.
 * Leaves cursor on first character in block.
 */
    સ્થિર શૂન્ય
shift_block(oparg_T *oap, પૂર્ણાંક amount)
{
    પૂર્ણાંક			ડાબું = (oap->op_type == OP_LSHIFT);
    પૂર્ણાંક			oldstate = State;
    પૂર્ણાંક			total;
    char_u		*newp, *oldp;
    size_t		newlen, oldlen;
    પૂર્ણાંક			oldcol = curwin->w_cursor.col;
    પૂર્ણાંક			sw_val = (પૂર્ણાંક)get_sw_કિંમત_indent(curbuf, ડાબું);
    પૂર્ણાંક			ts_val = (પૂર્ણાંક)curbuf->b_p_ts;
    સંરચના block_def	bd;
    પૂર્ણાંક			incr;
    colnr_T		ws_vcol;
    પૂર્ણાંક			added;
    size_t		new_લીટી_len;	// the length of the line after the
					// block shift
#ifdef FEAT_RIGHTLEFT
    પૂર્ણાંક			old_p_ri = p_ri;

    p_ri = 0;			// don't want revins in indent
#endif

    State = MODE_ઉમેરો;	// don't want MODE_REPLACE for State
    block_prep(oap, &bd, curwin->w_cursor.lnum, TRUE);
    જો (bd.is_short)
	પાછા;

    // total is number of screen columns to be inserted/removed
    total = (પૂર્ણાંક)((unsigned)amount * (unsigned)sw_val);
    જો ((total / sw_val) != amount)
	પાછા; // multiplication overflow

    oldp = ml_get_curline();
    oldlen = ml_get_curline_len();

    જો (!ડાબું)
    {
	પૂર્ણાંક		tabs = 0, spaces = 0;
	chartabsize_T	cts;

	/*
	 *  1. Get start vcol
	 *  2. Total ws vcols
	 *  3. Divvy into TABs & spp
	 *  4. Construct new string
	 */
	total += bd.pre_whitesp; // all virtual WS up to & incl a split TAB
	ws_vcol = bd.start_vcol - bd.pre_whitesp;
	જો (bd.startspaces)
	{
	    જો (has_mbyte)
	    {
		જો ((*mb_ptr2len)(bd.textstart) == 1)
		    ++bd.textstart;
		નહીંતર
		{
		    ws_vcol = 0;
		    bd.startspaces = 0;
		}
	    }
	    નહીંતર
		++bd.textstart;
	}

	// TODO: is passing bd.textstart for start of the line OK?
	init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum,
				   bd.start_vcol, bd.textstart, bd.textstart);
	માટે ( ; VIM_ISWHITE(*cts.cts_ptr); )
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
	જો (!curbuf->b_p_et)
	    tabstop_fromto(ws_vcol, ws_vcol + total,
				ts_val, curbuf->b_p_vts_array, &tabs, &spaces);
	નહીંતર
	    spaces = total;
#નહીંતર
	જો (!curbuf->b_p_et)
	    tabs = ((ws_vcol % ts_val) + total) / ts_val; // number of tabs
	જો (tabs > 0)
	    spaces = ((ws_vcol % ts_val) + total) % ts_val; // number of spp
	નહીંતર
	    spaces = total;
#endif
	// if we're splitting a TAB, allow for it
	bd.textcol -= bd.pre_whitesp_c - (bd.startspaces != 0);

	new_લીટી_len = bd.textcol + tabs + spaces + (oldlen - (bd.textstart - oldp));
	newp = alloc(new_લીટી_len + 1);
	જો (newp == ખાલી)
	    પાછા;
	mch_memmove(newp, oldp, (size_t)bd.textcol);
	newlen = bd.textcol;
	vim_memset(newp + newlen, TAB, (size_t)tabs);
	newlen += tabs;
	vim_memset(newp + newlen, ' ', (size_t)spaces);
	STRCPY(newp + newlen + spaces, bd.textstart);
    }
    નહીંતર //ડાબું
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
	જો (bd.startspaces)
	    MB_PTR_ADV(non_white);

	// The character's column is in "bd.start_vcol".
	non_white_col = bd.start_vcol;

	init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum,
				   non_white_col, bd.textstart, non_white);
	જ્યારે (VIM_ISWHITE(*cts.cts_ptr))
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
	જો (bd.startspaces)
	    verbatim_copy_width -= bd.start_char_vcols;
	init_chartabsize_arg(&cts, curwin, 0, verbatim_copy_width,
					     bd.textstart, verbatim_copy_end);
	જ્યારે (cts.cts_vcol < destination_col)
	{
	    incr = lbr_chartabsize(&cts);
	    જો (cts.cts_vcol + incr > destination_col)
		તોડો;
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
	new_લીટી_len = fixedlen + fill + (oldlen - (non_white - oldp));

	newp = alloc(new_લીટી_len + 1);
	જો (newp == ખાલી)
	    પાછા;
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
    જો (new_લીટી_len >= oldlen)
	added = (પૂર્ણાંક)(new_લીટી_len - oldlen);
    નહીંતર
	added = 0 - (પૂર્ણાંક)(oldlen - new_લીટી_len);
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
    સ્થિર શૂન્ય
block_ઉમેરો(
    oparg_T		*oap,
    char_u		*s,
    size_t		slen,
    પૂર્ણાંક			b_ઉમેરો,
    સંરચના block_def	*bdp)
{
    પૂર્ણાંક		ts_val;
    પૂર્ણાંક		ગણતરી = 0;	// extra spaces to replace a cut TAB
    પૂર્ણાંક		spaces = 0;	// non-zero if cutting a TAB
    colnr_T	offset;		// pointer along new line
    colnr_T	startcol;	// column where insert starts
    char_u	*newp, *oldp;	// new, old lines
    linenr_T	lnum;		// loop var
    પૂર્ણાંક		oldstate = State;

    State = MODE_ઉમેરો;	// don't want MODE_REPLACE for State

    માટે (lnum = oap->start.lnum + 1; lnum <= oap->end.lnum; lnum++)
    {
	block_prep(oap, bdp, lnum, TRUE);
	જો (bdp->is_short && b_ઉમેરો)
	    ચાલુ;	// OP_INSERT, line ends before block start

	oldp = ml_get(lnum);

	જો (b_ઉમેરો)
	{
	    ts_val = bdp->start_char_vcols;
	    spaces = bdp->startspaces;
	    જો (spaces != 0)
		ગણતરી = ts_val - 1; // we're cutting a TAB
	    offset = bdp->textcol;
	}
	નહીંતર // append
	{
	    ts_val = bdp->end_char_vcols;
	    જો (!bdp->is_short) // spaces = padding after block
	    {
		spaces = (bdp->endspaces ? ts_val - bdp->endspaces : 0);
		જો (spaces != 0)
		    ગણતરી = ts_val - 1; // we're cutting a TAB
		offset = bdp->textcol + bdp->textlen - (spaces != 0);
	    }
	    નહીંતર // spaces = padding to block edge
	    {
		// if $ used, just append to EOL (ie spaces==0)
		જો (!bdp->is_MAX)
		    spaces = (oap->end_vcol - bdp->end_vcol) + 1;
		ગણતરી = spaces;
		offset = bdp->textcol + bdp->textlen;
	    }
	}

	જો (has_mbyte && spaces > 0)
	    // avoid copying part of a multi-byte character
	    offset -= (*mb_head_off)(oldp, oldp + offset);

	જો (spaces < 0)  // can happen when the cursor was moved
	    spaces = 0;

	// Make sure the allocated size matches what is actually copied below.
	newp = alloc(ml_get_len(lnum) + spaces + slen
		    + (spaces > 0 && !bdp->is_short ? ts_val - spaces : 0)
								  + ગણતરી + 1);
	જો (newp == ખાલી)
	    ચાલુ;

	// copy up to shifted part
	mch_memmove(newp, oldp, (size_t)offset);
	oldp += offset;

	// insert pre-padding
	vim_memset(newp + offset, ' ', (size_t)spaces);
	startcol = offset + spaces;

	// copy the new text
	mch_memmove(newp + startcol, s, slen);
	offset += (પૂર્ણાંક)slen;

	જો (spaces > 0 && !bdp->is_short)
	{
	    જો (*oldp == TAB)
	    {
		// insert post-padding
		vim_memset(newp + offset + spaces, ' ',
						    (size_t)(ts_val - spaces));
		// we're splitting a TAB, don't copy it
		oldp++;
		// We allowed for that TAB, remember this now
		ગણતરી++;
	    }
	    નહીંતર
		// Not a TAB, no extra spaces
		ગણતરી = spaces;
	}

	જો (spaces > 0)
	    offset += ગણતરી;
	STRCPY(newp + offset, oldp);

	ml_replace(lnum, newp, FALSE);

	જો (b_ઉમેરો)
	    // correct any text properties
	    inserted_bytes(lnum, startcol, (પૂર્ણાંક)slen);

	જો (lnum == oap->end.lnum)
	{
	    // Set "']" mark to the end of the block instead of the end of
	    // the insert in the first line.
	    curbuf->b_op_end.lnum = oap->end.lnum;
	    curbuf->b_op_end.col = offset;
	    જો (curbuf->b_visual.vi_end.coladd)
	    {
		curbuf->b_visual.vi_end.col += curbuf->b_visual.vi_end.coladd;
		curbuf->b_visual.vi_end.coladd = 0;
	    }
	}
    } // for all lnum

    changed_લીટીઓ(oap->start.lnum + 1, 0, oap->end.lnum + 1, 0L);

    State = oldstate;
}

/*
 * Handle a delete operation.
 *
 * Return FAIL if undo failed, OK otherwise.
 */
    પૂર્ણાંક
op_delete(oparg_T *oap)
{
    પૂર્ણાંક			n;
    linenr_T		lnum;
    char_u		*ptr;
    char_u		*newp, *oldp;
    સંરચના block_def	bd;
    linenr_T		old_lcount = curbuf->b_ml.ml_લીટી_ગણતરી;
    પૂર્ણાંક			did_yank = FALSE;

    જો (curbuf->b_ml.ml_flags & ML_EMPTY)	    // nothing to do
	પાછા OK;

    // Nothing to delete, return here.	Do prepare undo, for op_change().
    જો (oap->empty)
	પાછા u_save_cursor();

    જો (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	પાછા FAIL;
    }

    જો (VIsual_select && oap->is_VIsual)
	// use register given with CTRL_R, defaults to zero
	oap->regname = VIsual_select_reg;

#ifdef HAVE_CLIPMETHOD
    adjust_clip_reg(&oap->regname);
#endif

    જો (has_mbyte)
	mb_adjust_opend(oap);

    /*
     * Imitate the strange Vi behaviour: If the delete spans more than one
     * line and motion_type == MCHAR and the result is a blank line, make the
     * delete linewise.  Don't do this for the change command or Visual mode.
     */
    જો (       oap->motion_type == MCHAR
	    && !oap->is_VIsual
	    && !oap->block_mode
	    && oap->લીટી_ગણતરી > 1
	    && oap->motion_force == NUL
	    && (vim_strchr(p_cpo, CPO_WORD) != ખાલી)
	    && oap->op_type == OP_DELETE)
    {
	ptr = ml_get(oap->end.lnum) + oap->end.col;
	જો (*ptr != NUL)
	    ptr += oap->inclusive;
	ptr = skipwhite(ptr);
	જો (*ptr == NUL && inindent(0))
	    oap->motion_type = MLINE;
    }

    /*
     * Check for trying to delete (e.g. "D") in an empty line.
     * Note: For the change operator it is ok.
     */
    જો (       oap->motion_type == MCHAR
	    && oap->લીટી_ગણતરી == 1
	    && oap->op_type == OP_DELETE
	    && *ml_get(oap->start.lnum) == NUL)
    {
	/*
	 * It's an error to operate on an empty region, when 'E' included in
	 * 'cpoptions' (Vi compatible).
	 */
	જો (virtual_op)
	    // Virtual editing: Nothing gets deleted, but we set the '[ and ']
	    // marks as if it happened.
	    જાઓ setmarks;
	જો (vim_strchr(p_cpo, CPO_EMPTYREGION) != ખાલી)
	    beep_flush();
	પાછા OK;
    }

    /*
     * Do a yank of whatever we're about to delete.
     * If a yank register was specified, put the deleted text into that
     * register.  For the black hole register '_' don't yank anything.
     */
    જો (oap->regname != '_')
    {
#ifdef FEAT_CLIPBOARD_PROVIDER
	inc_clip_provider();
#endif
	જો (oap->regname != 0)
	{
	    // check for read-only register
	    જો (!valid_yank_reg(oap->regname, TRUE))
	    {
		beep_flush();
#ifdef FEAT_CLIPBOARD_PROVIDER
		dec_clip_provider();
#endif
		પાછા OK;
	    }
	    get_yank_register(oap->regname, TRUE); // yank into specif'd reg.
	    જો (op_yank(oap, TRUE, FALSE) == OK)   // yank without message
		did_yank = TRUE;
	}
	નહીંતર
	    reset_y_append(); // not appending to unnamed register

	/*
	 * Put deleted text into register 1 and shift number registers if the
	 * delete contains a line break, or when using a specific operator (Vi
	 * compatible)
	 */
	જો (oap->motion_type == MLINE || oap->લીટી_ગણતરી > 1
							   || oap->use_reg_one)
	{
	    shift_delete_registers();
	    જો (op_yank(oap, TRUE, FALSE) == OK)
		did_yank = TRUE;
	}

	// Yank into small delete register when no named register specified
	// and the delete is within one line.
	જો ((
#ifdef HAVE_CLIPMETHOD
	    ((clip_unnamed & CLIP_UNNAMED) && oap->regname == '*') ||
	    ((clip_unnamed & CLIP_UNNAMED_PLUS) && oap->regname == '+') ||
#endif
	    oap->regname == 0) && oap->motion_type != MLINE
						      && oap->લીટી_ગણતરી == 1)
	{
	    oap->regname = '-';
	    get_yank_register(oap->regname, TRUE);
	    જો (op_yank(oap, TRUE, FALSE) == OK)
		did_yank = TRUE;
	    oap->regname = 0;
	}

	/*
	 * If there's too much stuff to fit in the yank register, then get a
	 * confirmation before doing the delete. This is crude, but simple.
	 * And it avoids doing a delete of something we can't put back if we
	 * want.
	 */
	જો (!did_yank)
	{
	    પૂર્ણાંક સંદેશ_silent_save = સંદેશ_silent;

	    સંદેશ_silent = 0;	// must display the prompt
	    n = ask_yesno((char_u *)_("cannot yank; delete anyway"), TRUE);
	    સંદેશ_silent = સંદેશ_silent_save;
	    જો (n != 'y')
	    {
		emsg(_(e_command_aborted));
#ifdef FEAT_CLIPBOARD_PROVIDER
		dec_clip_provider();
#endif
		પાછા FAIL;
	    }
	}

#જો defined(FEAT_EVAL)
	જો (did_yank && has_textyankpost())
	    yank_do_autocmd(oap, get_y_current());
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#endif
    }

    /*
     * block mode delete
     */
    જો (oap->block_mode)
    {
	જો (u_save((linenr_T)(oap->start.lnum - 1),
			       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	    પાછા FAIL;

	માટે (lnum = curwin->w_cursor.lnum; lnum <= oap->end.lnum; ++lnum)
	{
	    block_prep(oap, &bd, lnum, TRUE);
	    જો (bd.textlen == 0)	// nothing to delete
		ચાલુ;

	    // Adjust cursor position for tab replaced by spaces and 'lbr'.
	    જો (lnum == curwin->w_cursor.lnum)
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
	    જો (newp == ખાલી)
		ચાલુ;
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
	    જો (curbuf->b_has_textprop && n != 0)
		adjust_prop_columns(lnum, bd.textcol, -n, 0);
#endif
	}

	check_cursor_col();
	changed_લીટીઓ(curwin->w_cursor.lnum, curwin->w_cursor.col,
						       oap->end.lnum + 1, 0L);
	oap->લીટી_ગણતરી = 0;	    // no lines deleted
    }
    નહીંતર જો (oap->motion_type == MLINE)
    {
	જો (oap->op_type == OP_CHANGE)
	{
	    // Delete the lines except the first one.  Temporarily move the
	    // cursor to the next line.  Save the current line number, if the
	    // last line is deleted it may be changed.
	    જો (oap->લીટી_ગણતરી > 1)
	    {
		lnum = curwin->w_cursor.lnum;
		++curwin->w_cursor.lnum;
		del_લીટીઓ((long)(oap->લીટી_ગણતરી - 1), TRUE);
		curwin->w_cursor.lnum = lnum;
	    }
	    જો (u_save_cursor() == FAIL)
		પાછા FAIL;
	    જો (curbuf->b_p_ai)		    // don't delete indent
	    {
		beginline(BL_WHITE);	    // cursor on first non-white
		did_ai = TRUE;		    // delete the indent when ESC hit
		ai_col = curwin->w_cursor.col;
	    }
	    નહીંતર
		beginline(0);		    // cursor in column 0
	    truncate_લીટી(FALSE);   // delete the rest of the line,
				    // leaving cursor past last char in line
	    જો (oap->લીટી_ગણતરી > 1)
		u_clearline();	    // "U" command not possible after "2cc"
	}
	નહીંતર
	{
	    del_લીટીઓ(oap->લીટી_ગણતરી, TRUE);
	    beginline(BL_WHITE | BL_FIX);
	    u_clearline();	// "U" command not possible after "dd"
	}
    }
    નહીંતર
    {
	જો (virtual_op)
	{
	    પૂર્ણાંક		endcol = 0;

	    // For virtualedit: break the tabs that are partly included.
	    જો (gchar_pos(&oap->start) == '\t')
	    {
		જો (u_save_cursor() == FAIL)	// save first line for undo
		    પાછા FAIL;
		જો (oap->લીટી_ગણતરી == 1)
		    endcol = getviscol2(oap->end.col, oap->end.coladd);
		coladvance_force(getviscol2(oap->start.col, oap->start.coladd));
		oap->start = curwin->w_cursor;
		જો (oap->લીટી_ગણતરી == 1)
		{
		    coladvance(endcol);
		    oap->end.col = curwin->w_cursor.col;
		    oap->end.coladd = curwin->w_cursor.coladd;
		    curwin->w_cursor = oap->start;
		}
	    }

	    // Break a tab only when it's included in the area.
	    જો (gchar_pos(&oap->end) == '\t'
				     && (પૂર્ણાંક)oap->end.coladd < oap->inclusive)
	    {
		// save last line for undo
		જો (u_save((linenr_T)(oap->end.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
		    પાછા FAIL;
		curwin->w_cursor = oap->end;
		coladvance_force(getviscol2(oap->end.col, oap->end.coladd));
		oap->end = curwin->w_cursor;
		curwin->w_cursor = oap->start;
	    }
	    જો (has_mbyte)
		mb_adjust_opend(oap);
	}

	જો (oap->લીટી_ગણતરી == 1)	// delete characters within one line
	{
	    જો (u_save_cursor() == FAIL)	// save line for undo
		પાછા FAIL;

	    // if 'cpoptions' contains '$', display '$' at end of change
	    જો (       vim_strchr(p_cpo, CPO_DOLLAR) != ખાલી
		    && oap->op_type == OP_CHANGE
		    && oap->end.lnum == curwin->w_cursor.lnum
		    && !oap->is_VIsual)
		display_dollar(oap->end.col - !oap->inclusive);

	    n = oap->end.col - oap->start.col + 1 - !oap->inclusive;

	    જો (virtual_op)
	    {
		// fix up things for virtualedit-delete:
		// break the tabs which are going to get in our way
		પૂર્ણાંક		len = ml_get_curline_len();

		જો (oap->end.coladd != 0
			&& (પૂર્ણાંક)oap->end.col >= len - 1
			&& !(oap->start.coladd && (પૂર્ણાંક)oap->end.col >= len - 1))
		    n++;
		// Delete at least one char (e.g, when on a control char).
		જો (n == 0 && oap->start.coladd != oap->end.coladd)
		    n = 1;

		// When deleted a char in the line, reset coladd.
		જો (gchar_cursor() != NUL)
		    curwin->w_cursor.coladd = 0;
	    }
	    (શૂન્ય)del_bytes((long)n, !virtual_op,
			    oap->op_type == OP_DELETE && !oap->is_VIsual);
	}
	નહીંતર				// delete characters between lines
	{
	    pos_T   curpos;

	    // save deleted and changed lines for undo
	    જો (u_save((linenr_T)(curwin->w_cursor.lnum - 1),
		 (linenr_T)(curwin->w_cursor.lnum + oap->લીટી_ગણતરી)) == FAIL)
		પાછા FAIL;

	    truncate_લીટી(TRUE);	// delete from cursor to end of line

	    curpos = curwin->w_cursor;	// remember curwin->w_cursor
	    ++curwin->w_cursor.lnum;
	    del_લીટીઓ((long)(oap->લીટી_ગણતરી - 2), FALSE);

	    // delete from start of line until op_end
	    n = (oap->end.col + 1 - !oap->inclusive);
	    curwin->w_cursor.col = 0;
	    (શૂન્ય)del_bytes((long)n, !virtual_op,
			    oap->op_type == OP_DELETE && !oap->is_VIsual);
	    curwin->w_cursor = curpos;	// restore curwin->w_cursor
	    (શૂન્ય)do_join(2, FALSE, FALSE, FALSE, FALSE);
	}
	જો (oap->op_type == OP_DELETE)
	    auto_format(FALSE, TRUE);
    }

    msgmore(curbuf->b_ml.ml_લીટી_ગણતરી - old_lcount);

setmarks:
    જો ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	જો (oap->block_mode)
	{
	    curbuf->b_op_end.lnum = oap->end.lnum;
	    curbuf->b_op_end.col = oap->start.col;
	}
	નહીંતર
	    curbuf->b_op_end = oap->start;
	curbuf->b_op_start = oap->start;
    }

    પાછા OK;
}

/*
 * Adjust end of operating area for ending on a multi-byte character.
 * Used for deletion.
 */
    સ્થિર શૂન્ય
mb_adjust_opend(oparg_T *oap)
{
    char_u	*લીટી;
    char_u	*ptr;

    જો (!oap->inclusive)
	પાછા;

    લીટી = ml_get(oap->end.lnum);
    ptr = લીટી + oap->end.col;
    જો (*ptr != NUL)
    {
	ptr -= (*mb_head_off)(લીટી, ptr);
	ptr += (*mb_ptr2len)(ptr) - 1;
	oap->end.col = ptr - લીટી;
    }
}

/*
 * Replace the character under the cursor with "c".
 * This takes care of multi-byte characters.
 */
    સ્થિર શૂન્ય
replace_character(પૂર્ણાંક c)
{
    પૂર્ણાંક n = State;

    State = MODE_REPLACE;
    ins_char(c);
    State = n;
    // Backup to the replaced character.
    dec_cursor();
}

/*
 * Replace a whole area with one character.
 */
    પૂર્ણાંક
op_replace(oparg_T *oap, પૂર્ણાંક c)
{
    પૂર્ણાંક			n, numc;
    પૂર્ણાંક			અંક_chars;
    char_u		*newp, *oldp;
    size_t		newlen, oldlen;
    સંરચના block_def	bd;
    char_u		*after_p = ખાલી;
    પૂર્ણાંક			had_ctrl_v_cr = FALSE;

    જો ((curbuf->b_ml.ml_flags & ML_EMPTY ) || oap->empty)
	પાછા OK;	    // nothing to do

    જો (c == REPLACE_CR_NCHAR)
    {
	had_ctrl_v_cr = TRUE;
	c = CAR;
    }
    નહીંતર જો (c == REPLACE_NL_NCHAR)
    {
	had_ctrl_v_cr = TRUE;
	c = NL;
    }

    જો (has_mbyte)
	mb_adjust_opend(oap);

    જો (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	પાછા FAIL;

    /*
     * block mode replace
     */
    જો (oap->block_mode)
    {
	bd.is_MAX = (curwin->w_curswant == MAXCOL);
	માટે ( ; curwin->w_cursor.lnum <= oap->end.lnum; ++curwin->w_cursor.lnum)
	{
	    curwin->w_cursor.col = 0;  // make sure cursor position is valid
	    block_prep(oap, &bd, curwin->w_cursor.lnum, TRUE);
	    જો (bd.textlen == 0 && (!virtual_op || bd.is_MAX))
		ચાલુ;	    // nothing to replace

	    // n == number of extra chars required
	    // If we split a TAB, it may be replaced by several characters.
	    // Thus the number of characters may increase!
	    // If the range starts in virtual space, count the initial
	    // coladd offset as part of "startspaces"
	    જો (virtual_op && bd.is_short && *bd.textstart == NUL)
	    {
		pos_T vpos;

		vpos.lnum = curwin->w_cursor.lnum;
		getvpos(&vpos, oap->start_vcol);
		bd.startspaces += vpos.coladd;
		n = bd.startspaces;
	    }
	    નહીંતર
		// allow for pre spaces
		n = (bd.startspaces ? bd.start_char_vcols - 1 : 0);

	    // allow for post spp
	    n += (bd.endspaces
		    && !bd.is_oneChar
		    && bd.end_char_vcols > 0) ? bd.end_char_vcols - 1 : 0;
	    // Figure out how many characters to replace.
	    numc = oap->end_vcol - oap->start_vcol + 1;
	    જો (bd.is_short && (!virtual_op || bd.is_MAX))
		numc -= (oap->end_vcol - bd.end_vcol) + 1;

	    // A double-wide character can be replaced only up to half the
	    // times.
	    જો ((*mb_char2cells)(c) > 1)
	    {
		જો ((numc & 1) && !bd.is_short)
		{
		    ++bd.endspaces;
		    ++n;
		}
		numc = numc / 2;
	    }

	    // Compute bytes needed, move character count to num_chars.
	    અંક_chars = numc;
	    numc *= (*mb_char2len)(c);
	    // oldlen includes textlen, so don't double count
	    n += numc - bd.textlen;

	    oldp = ml_get_curline();
	    oldlen = ml_get_curline_len();
	    newp = alloc(oldlen + 1 + n);
	    જો (newp == ખાલી)
		ચાલુ;
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
	    જો (had_ctrl_v_cr || (c != '\r' && c != '\n'))
	    {
		જો (has_mbyte)
		{
		    જ્યારે (--અંક_chars >= 0)
			newlen += (*mb_char2bytes)(c, newp + newlen);
		}
		નહીંતર
		{
		    vim_memset(newp + newlen, c, (size_t)numc);
		    newlen += numc;
		}
		જો (!bd.is_short)
		{
		    // insert post-spaces
		    vim_memset(newp + newlen, ' ', (size_t)bd.endspaces);
		    // copy the part after the changed part
		    STRCPY(newp + newlen + bd.endspaces,
				oldp + bd.textcol + bd.textlen);
		}
	    }
	    નહીંતર
	    {
		// Replacing with \r or \n means splitting the line.
		after_p = alloc(oldlen + 1 + n - newlen);
		જો (after_p != ખાલી)
		    STRCPY(after_p, oldp + bd.textcol + bd.textlen);
	    }

	    // replace the line
	    ml_replace(curwin->w_cursor.lnum, newp, FALSE);
	    જો (after_p != ખાલી)
	    {
		ml_append(curwin->w_cursor.lnum++, after_p, 0, FALSE);
		appended_લીટીઓ_mark(curwin->w_cursor.lnum, 1L);
		oap->end.lnum++;
		vim_free(after_p);
	    }
	}
    }
    નહીંતર
    {
	/*
	 * MCHAR and MLINE motion replace.
	 */
	જો (oap->motion_type == MLINE)
	{
	    oap->start.col = 0;
	    curwin->w_cursor.col = 0;
	    oap->end.col = ml_get_len(oap->end.lnum);
	    જો (oap->end.col)
		--oap->end.col;
	}
	નહીંતર જો (!oap->inclusive)
	    dec(&(oap->end));

	જ્યારે (LTOREQ_POS(curwin->w_cursor, oap->end))
	{
	    પૂર્ણાંક done = FALSE;

	    n = gchar_cursor();
	    જો (n != NUL)
	    {
		પૂર્ણાંક new_byte_len = (*mb_char2len)(c);
		પૂર્ણાંક old_byte_len = mb_ptr2len(ml_get_cursor());

		જો (new_byte_len > 1 || old_byte_len > 1)
		{
		    // This is slow, but it handles replacing a single-byte
		    // with a multi-byte and the other way around.
		    જો (curwin->w_cursor.lnum == oap->end.lnum)
			oap->end.col += new_byte_len - old_byte_len;
		    replace_character(c);
		    done = TRUE;
		}
		નહીંતર
		{
		    જો (n == TAB)
		    {
			પૂર્ણાંક end_vcol = 0;

			જો (curwin->w_cursor.lnum == oap->end.lnum)
			{
			    // oap->end has to be recalculated when
			    // the tab breaks
			    end_vcol = getviscol2(oap->end.col,
							     oap->end.coladd);
			}
			coladvance_force(getviscol());
			જો (curwin->w_cursor.lnum == oap->end.lnum)
			    getvpos(&oap->end, end_vcol);
		    }
		    // with "coladd" set may move to just after a TAB
		    જો (gchar_cursor() != NUL)
		    {
			PBYTE(curwin->w_cursor, c);
			done = TRUE;
		    }
		}
	    }
	    જો (!done && virtual_op && curwin->w_cursor.lnum == oap->end.lnum)
	    {
		પૂર્ણાંક virtcols = oap->end.coladd;

		જો (curwin->w_cursor.lnum == oap->start.lnum
			&& oap->start.col == oap->end.col && oap->start.coladd)
		    virtcols -= oap->start.coladd;

		// oap->end has been trimmed so it's effectively inclusive;
		// as a result an extra +1 must be counted so we don't
		// trample the NUL byte.
		coladvance_force(getviscol2(oap->end.col, oap->end.coladd) + 1);
		curwin->w_cursor.col -= (virtcols + 1);
		માટે (; virtcols >= 0; virtcols--)
		{
		    જો ((*mb_char2len)(c) > 1)
		       replace_character(c);
		    નહીંતર
			PBYTE(curwin->w_cursor, c);
		    જો (inc(&curwin->w_cursor) == -1)
			તોડો;
		}
	    }

	    // Advance to next character, stop at the end of the file.
	    જો (inc_cursor() == -1)
		તોડો;
	}
    }

    curwin->w_cursor = oap->start;
    check_cursor();
    changed_લીટીઓ(oap->start.lnum, oap->start.col, oap->end.lnum + 1, 0L);

    જો ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
    }

    પાછા OK;
}

સ્થિર પૂર્ણાંક swapchars(પૂર્ણાંક op_type, pos_T *pos, પૂર્ણાંક length);

/*
 * Handle the (non-standard vi) tilde operator.  Also for "gu", "gU" and "g?".
 */
    સ્થિર શૂન્ય
op_tilde(oparg_T *oap)
{
    pos_T		pos;
    સંરચના block_def	bd;
    પૂર્ણાંક			did_change = FALSE;

    જો (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	પાછા;

    pos = oap->start;
    જો (oap->block_mode)		    // Visual block mode
    {
	માટે (; pos.lnum <= oap->end.lnum; ++pos.lnum)
	{
	    પૂર્ણાંક one_change;

	    block_prep(oap, &bd, pos.lnum, FALSE);
	    pos.col = bd.textcol;
	    one_change = swapchars(oap->op_type, &pos, bd.textlen);
	    did_change |= one_change;

#ifdef FEAT_NETBEANS_INTG
	    જો (netbeans_active() && one_change)
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
	જો (did_change)
	    changed_લીટીઓ(oap->start.lnum, 0, oap->end.lnum + 1, 0L);
    }
    નહીંતર				    // not block mode
    {
	જો (oap->motion_type == MLINE)
	{
	    oap->start.col = 0;
	    pos.col = 0;
	    oap->end.col = ml_get_len(oap->end.lnum);
	    જો (oap->end.col)
		--oap->end.col;
	}
	નહીંતર જો (!oap->inclusive)
	    dec(&(oap->end));

	જો (pos.lnum == oap->end.lnum)
	    did_change = swapchars(oap->op_type, &pos,
						  oap->end.col - pos.col + 1);
	નહીંતર
	    માટે (;;)
	    {
		did_change |= swapchars(oap->op_type, &pos,
			pos.lnum == oap->end.lnum ? oap->end.col + 1
						  : ml_get_pos_len(&pos));
		જો (LTOREQ_POS(oap->end, pos) || inc(&pos) == -1)
		    તોડો;
	    }
	જો (did_change)
	{
	    changed_લીટીઓ(oap->start.lnum, oap->start.col, oap->end.lnum + 1,
									  0L);
#ifdef FEAT_NETBEANS_INTG
	    જો (netbeans_active())
	    {
		char_u *ptr;
		પૂર્ણાંક ગણતરી;

		pos = oap->start;
		જ્યારે (pos.lnum < oap->end.lnum)
		{
		    ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		    ગણતરી = ml_get_buf_len(curbuf, pos.lnum) - pos.col;
		    netbeans_removed(curbuf, pos.lnum, pos.col, (long)ગણતરી);
		    // get the line again, it may have been flushed
		    ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		    netbeans_inserted(curbuf, pos.lnum, pos.col,
							&ptr[pos.col], ગણતરી);
		    pos.col = 0;
		    pos.lnum++;
		}
		ગણતરી = oap->end.col - pos.col + 1;
		netbeans_removed(curbuf, pos.lnum, pos.col, (long)ગણતરી);
		// get the line again, it may have been flushed
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, pos.col,
							&ptr[pos.col], ગણતરી);
	    }
#endif
	}
    }

    જો (!did_change && oap->is_VIsual)
	// No change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    જો ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set '[ and '] marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
    }

    જો (oap->લીટી_ગણતરી > p_report)
	smsg(NGETTEXT("%ld line changed", "%ld lines changed",
					    oap->લીટી_ગણતરી), oap->લીટી_ગણતરી);
}

/*
 * Invoke swapchar() on "length" bytes at position "pos".
 * "pos" is advanced to just after the changed characters.
 * "length" is rounded up to include the whole last multi-byte character.
 * Also works correctly when the number of bytes changes.
 * Returns TRUE if some character was changed.
 */
    સ્થિર પૂર્ણાંક
swapchars(પૂર્ણાંક op_type, pos_T *pos, પૂર્ણાંક length)
{
    પૂર્ણાંક todo;
    પૂર્ણાંક	did_change = 0;

    માટે (todo = length; todo > 0; --todo)
    {
	જો (has_mbyte)
	{
	    પૂર્ણાંક len = (*mb_ptr2len)(ml_get_pos(pos));

	    // we're counting bytes, not characters
	    જો (len > 0)
		todo -= len - 1;
	}
	did_change |= swapchar(op_type, pos);
	જો (inc(pos) == -1)    // at end of file
	    તોડો;
    }
    પાછા did_change;
}

/*
 * If op_type == OP_UPPER: make uppercase,
 * if op_type == OP_LOWER: make lowercase,
 * if op_type == OP_ROT13: do rot13 encoding,
 * else swap case of character at 'pos'
 * returns TRUE when something actually changed.
 */
    પૂર્ણાંક
swapchar(પૂર્ણાંક op_type, pos_T *pos)
{
    પૂર્ણાંક	    c;
    પૂર્ણાંક	    nc;

    c = gchar_pos(pos);

    // Only do rot13 encoding for ASCII characters.
    જો (c >= 0x80 && op_type == OP_ROT13)
	પાછા FALSE;

    // ~ is OP_NOP, g~ is OP_TILDE, gU is OP_UPPER
    જો ((op_type == OP_UPPER || op_type == OP_NOP || op_type == OP_TILDE)
	    && c == 0xdf
	    && (enc_latin1like || STRCMP(p_enc, "iso-8859-2") == 0))
    {
	pos_T   sp = curwin->w_cursor;

	// Special handling for lowercase German sharp s (ß): convert to uppercase (ẞ).
	curwin->w_cursor = *pos;
	del_char(FALSE);
	ins_char(0x1E9E);
	curwin->w_cursor = sp;
	પાછા TRUE;
    }

    જો (enc_dbcs != 0 && c >= 0x100)	// No lower/uppercase letter
	પાછા FALSE;
    nc = c;
    જો (MB_ISLOWER(c))
    {
	જો (op_type == OP_ROT13)
	    nc = ROT13(c, 'a');
	નહીંતર જો (op_type != OP_LOWER)
	    nc = MB_TOUPPER(c);
    }
    નહીંતર જો (MB_ISUPPER(c))
    {
	જો (op_type == OP_ROT13)
	    nc = ROT13(c, 'A');
	નહીંતર જો (op_type != OP_UPPER)
	    nc = MB_TOLOWER(c);
    }
    જો (nc != c)
    {
	જો (enc_utf8 && (c >= 0x80 || nc >= 0x80))
	{
	    pos_T   sp = curwin->w_cursor;

	    curwin->w_cursor = *pos;
	    // don't use del_char(), it also removes composing chars
	    del_bytes(utf_ptr2len(ml_get_cursor()), FALSE, FALSE);
	    ins_char(nc);
	    curwin->w_cursor = sp;
	}
	નહીંતર
	    PBYTE(*pos, nc);
	પાછા TRUE;
    }
    પાછા FALSE;
}

/*
 * op_insert - Insert and append operators for Visual mode.
 */
    શૂન્ય
op_ઉમેરો(oparg_T *oap, long count1)
{
    long		pre_textlen = 0;
    colnr_T		ind_pre_col = 0, ind_post_col;
    પૂર્ણાંક			ind_pre_vcol = 0, ind_post_vcol = 0;
    સંરચના block_def	bd;
    પૂર્ણાંક			i;
    pos_T		t1;
    pos_T		start_ઉમેરો;

    // edit() changes this - record it for OP_APPEND
    bd.is_MAX = (curwin->w_curswant == MAXCOL);

    // vis block is still marked. Get rid of it now.
    curwin->w_cursor.lnum = oap->start.lnum;
    update_screen(UPD_INVERTED);

    જો (oap->block_mode)
    {
	// When 'virtualedit' is used, need to insert the extra spaces before
	// doing block_prep().  When only "block" is used, virtual edit is
	// already disabled, but still need it when calling
	// coladvance_force().
	// coladvance_force() uses get_ve_flags() to get the 'virtualedit'
	// state for the current window.  To override that state, we need to
	// set the window-local value of ve_flags rather than the global value.
	જો (curwin->w_cursor.coladd > 0)
	{
	    પૂર્ણાંક		old_ve_flags = curwin->w_ve_flags;

	    જો (u_save_cursor() == FAIL)
		પાછા;

	    curwin->w_ve_flags = VE_ALL;
	    coladvance_force(oap->op_type == OP_APPEND
					   ? oap->end_vcol + 1 : getviscol());
	    જો (oap->op_type == OP_APPEND)
		--curwin->w_cursor.col;
	    curwin->w_ve_flags = old_ve_flags;
	}
	// Get the info about the block before entering the text
	block_prep(oap, &bd, oap->start.lnum, TRUE);
	// Get indent information
	ind_pre_col = (colnr_T)getwhitecols_curline();
	ind_pre_vcol = get_indent();
	pre_textlen = ml_get_len(oap->start.lnum) - bd.textcol;
	જો (oap->op_type == OP_APPEND)
	    pre_textlen -= bd.textlen;
    }

    જો (oap->op_type == OP_APPEND)
    {
	જો (oap->block_mode && curwin->w_cursor.coladd == 0)
	{
	    // Move the cursor to the character right of the block.
	    curwin->w_set_curswant = true;
	    જ્યારે (*ml_get_cursor() != NUL
		    && (curwin->w_cursor.col < bd.textcol + bd.textlen))
		++curwin->w_cursor.col;
	    જો (bd.is_short && !bd.is_MAX)
	    {
		// First line was too short, make it longer and adjust the
		// values in "bd".
		જો (u_save_cursor() == FAIL)
		    પાછા;
		માટે (i = 0; i < bd.endspaces; ++i)
		    ins_char(' ');
		bd.textlen += bd.endspaces;
	    }
	}
	નહીંતર
	{
	    curwin->w_cursor = oap->end;
	    check_cursor_col();

	    // Works just like an 'i'nsert on the next character.
	    જો (!LINEEMPTY(curwin->w_cursor.lnum)
		    && oap->start_vcol != oap->end_vcol)
		inc_cursor();
	}
    }

    t1 = oap->start;
    start_ઉમેરો = curwin->w_cursor;
    (શૂન્ય)edit(NUL, FALSE, (linenr_T)count1);

    // When a tab was inserted, and the characters in front of the tab
    // have been converted to a tab as well, the column of the cursor
    // might have actually been reduced, so need to adjust here.
    જો (t1.lnum == curbuf->b_op_start_orig.lnum
	    && LT_POS(curbuf->b_op_start_orig, t1))
	oap->start = curbuf->b_op_start_orig;

    // If user has moved off this line, we don't know what to do, so do
    // nothing.
    // Also don't repeat the insert when Insert mode ended with CTRL-C.
    જો (curwin->w_cursor.lnum != oap->start.lnum || got_int)
	પાછા;

    જો (oap->block_mode)
    {
	પૂર્ણાંક			ins_len;
	char_u			*firstline, *ins_text;
	સંરચના block_def	bd2;
	પૂર્ણાંક			did_indent = FALSE;
	size_t			len;
	size_t			add;
	// offset when cursor was moved in insert mode
	પૂર્ણાંક			offset = 0;

	// If indent kicked in, the firstline might have changed
	// but only do that, if the indent actually increased.
	ind_post_col = (colnr_T)getwhitecols_curline();
	જો (curbuf->b_op_start.col > ind_pre_col && ind_post_col > ind_pre_col)
	{
	    bd.textcol += ind_post_col - ind_pre_col;
	    ind_post_vcol = get_indent();
	    bd.start_vcol += ind_post_vcol - ind_pre_vcol;
	    did_indent = TRUE;
	}

	// The user may have moved the cursor before inserting something, try
	// to adjust the block for that.  But only do it, if the difference
	// does not come from indent kicking in.
	જો (oap->start.lnum == curbuf->b_op_start_orig.lnum
						  && !bd.is_MAX && !did_indent)
	{
	    પૂર્ણાંક t = getviscol2(curbuf->b_op_start_orig.col,
					       curbuf->b_op_start_orig.coladd);

	    જો (oap->op_type == OP_ઉમેરો
		    && oap->start.col + oap->start.coladd
			    != curbuf->b_op_start_orig.col
					      + curbuf->b_op_start_orig.coladd)
	    {
		oap->start.col = curbuf->b_op_start_orig.col;
		pre_textlen -= t - oap->start_vcol;
		oap->start_vcol = t;
	    }
	    નહીંતર જો (oap->op_type == OP_APPEND
		    && oap->start.col + oap->start.coladd
			    >= curbuf->b_op_start_orig.col
					      + curbuf->b_op_start_orig.coladd)
	    {
		oap->start.col = curbuf->b_op_start_orig.col;
		// reset pre_textlen to the value of OP_INSERT
		pre_textlen += bd.textlen;
		pre_textlen -= t - oap->start_vcol;
		oap->start_vcol = t;
		oap->op_type = OP_ઉમેરો;
	    }
	}

	// Spaces and tabs in the indent may have changed to other spaces and
	// tabs.  Get the starting column again and correct the length.
	// Don't do this when "$" used, end-of-line will have changed.
	//
	// if indent was added and the inserted text was after the indent,
	// correct the selection for the new indent.
	જો (did_indent && bd.textcol - ind_post_col > 0)
	{
	    oap->start.col += ind_post_col - ind_pre_col;
	    oap->start_vcol += ind_post_vcol - ind_pre_vcol;
	    oap->end.col += ind_post_col - ind_pre_col;
	    oap->end_vcol += ind_post_vcol - ind_pre_vcol;
	}
	block_prep(oap, &bd2, oap->start.lnum, TRUE);
	જો (did_indent && bd.textcol - ind_post_col > 0)
	{
	    // undo for where "oap" is used below
	    oap->start.col -= ind_post_col - ind_pre_col;
	    oap->start_vcol -= ind_post_vcol - ind_pre_vcol;
	    oap->end.col -= ind_post_col - ind_pre_col;
	    oap->end_vcol -= ind_post_vcol - ind_pre_vcol;
	}
	જો (!bd.is_MAX || bd2.textlen < bd.textlen)
	{
	    જો (oap->op_type == OP_APPEND)
	    {
		pre_textlen += bd2.textlen - bd.textlen;
		જો (bd2.endspaces)
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
	જો (oap->op_type == OP_APPEND)
	{
	    add += bd.textlen;
	    // account for pressing cursor in insert mode when '$' was used
	    જો (bd.is_MAX
		&& (start_ઉમેરો.lnum == Insstart.lnum
					   && start_ઉમેરો.col > Insstart.col))
	    {
		offset = (start_ઉમેરો.col - Insstart.col);
		add -= offset;
		જો (oap->end_vcol > offset)
		    oap->end_vcol -= (offset + 1);
		નહીંતર
		    // moved outside of the visual block, what to do?
		    પાછા;
	    }
	}
	જો (add > len)
	    add = len;  // short line, point to the NUL
	firstline += add;
	len -= add;
	જો (pre_textlen >= 0 && (ins_len = (પૂર્ણાંક)len - pre_textlen - offset) > 0)
	{
	    ins_text = vim_strnsave(firstline, ins_len);
	    જો (ins_text != ખાલી)
	    {
		// block handled here
		જો (u_save(oap->start.lnum,
					 (linenr_T)(oap->end.lnum + 1)) == OK)
		    block_ઉમેરો(oap, ins_text, ins_len, (oap->op_type == OP_ઉમેરો),
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
    પૂર્ણાંક
op_change(oparg_T *oap)
{
    colnr_T		l;
    પૂર્ણાંક			retval;
    linenr_T		linenr;
    long		pre_textlen = 0;
    long		pre_indent = 0;
    char_u		*firstline;
    char_u		*ins_text, *newp, *oldp;
    સંરચના block_def	bd;

    l = oap->start.col;
    જો (oap->motion_type == MLINE)
    {
	l = 0;
	can_si = may_do_si();	// Like opening a new line, do smart indent
    }

    // First delete the text in the region.  In an empty buffer only need to
    // save for undo
    જો (curbuf->b_ml.ml_flags & ML_EMPTY)
    {
	જો (u_save_cursor() == FAIL)
	    પાછા FALSE;
    }
    નહીંતર જો (op_delete(oap) == FAIL)
	પાછા FALSE;

    જો ((l > curwin->w_cursor.col) && !LINEEMPTY(curwin->w_cursor.lnum)
							 && !virtual_op)
	inc_cursor();

    // check for still on same line (<CR> in inserted text meaningless)
    // skip blank lines too
    જો (oap->block_mode)
    {
	// Add spaces before getting the current line length.
	જો (virtual_op && (curwin->w_cursor.coladd > 0
						    || gchar_cursor() == NUL))
	    coladvance_force(getviscol());
	firstline = ml_get(oap->start.lnum);
	pre_textlen = ml_get_len(oap->start.lnum);
	pre_indent = (long)getwhitecols(firstline);
	bd.textcol = curwin->w_cursor.col;
    }

    જો (oap->motion_type == MLINE)
	fix_indent();

    // Reset finish_op now, don't want it set inside edit().
    પૂર્ણાંક save_finish_op = finish_op;
    finish_op = FALSE;

    retval = edit(NUL, FALSE, (linenr_T)1);

    finish_op = save_finish_op;

    /*
     * In Visual block mode, handle copying the new text to all lines of the
     * block.
     * Don't repeat the insert when Insert mode ended with CTRL-C.
     */
    જો (oap->block_mode && oap->start.lnum != oap->end.lnum && !got_int)
    {
	પૂર્ણાંક	ins_len;

	// Auto-indenting may have changed the indent.  If the cursor was past
	// the indent, exclude that indent change from the inserted text.
	firstline = ml_get(oap->start.lnum);
	જો (bd.textcol > (colnr_T)pre_indent)
	{
	    long new_indent = (long)getwhitecols(firstline);

	    pre_textlen += new_indent - pre_indent;
	    bd.textcol += new_indent - pre_indent;
	}

	ins_len = (પૂર્ણાંક)ml_get_len(oap->start.lnum) - pre_textlen;
	જો (ins_len > 0)
	{
	    // Subsequent calls to ml_get() flush the firstline data - take a
	    // copy of the inserted text.
	    જો ((ins_text = alloc(ins_len + 1)) != ખાલી)
	    {
		vim_strncpy(ins_text, firstline + bd.textcol, ins_len);
		માટે (linenr = oap->start.lnum + 1; linenr <= oap->end.lnum;
								     linenr++)
		{
		    block_prep(oap, &bd, linenr, TRUE);
		    જો (!bd.is_short || virtual_op)
		    {
			pos_T vpos;
			size_t newlen;

			// If the block starts in virtual space, count the
			// initial coladd offset as part of "startspaces"
			જો (bd.is_short)
			{
			    vpos.lnum = linenr;
			    (શૂન્ય)getvpos(&vpos, oap->start_vcol);
			}
			નહીંતર
			    vpos.coladd = 0;
			oldp = ml_get(linenr);
			newp = alloc(ml_get_len(linenr) + vpos.coladd + ins_len + 1);
			જો (newp == ખાલી)
			    ચાલુ;
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
			જો (curbuf->b_has_textprop)
			    adjust_prop_columns(linenr, bd.textcol,
						     vpos.coladd + (પૂર્ણાંક)ins_len, 0);
#endif
		    }
		}
		check_cursor();

		changed_લીટીઓ(oap->start.lnum + 1, 0, oap->end.lnum + 1, 0L);
	    }
	    vim_free(ins_text);
	}
    }
    auto_format(FALSE, TRUE);

    પાછા retval;
}

/*
 * When the cursor is on the NUL past the end of the line and it should not be
 * there move it left.
 */
    શૂન્ય
adjust_cursor_eol(શૂન્ય)
{
    unsigned પૂર્ણાંક cur_ve_flags = get_ve_flags();

    પૂર્ણાંક adj_cursor = (curwin->w_cursor.col > 0
				&& gchar_cursor() == NUL
				&& (cur_ve_flags & VE_ONEMORE) == 0
				&& (cur_ve_flags & VE_ALL) == 0
				&& !(restart_edit || (State & MODE_ઉમેરો)));
    જો (!adj_cursor)
	પાછા;

    // Put the cursor on the last character in the line.
    dec_cursor();

    જો (cur_ve_flags == VE_ALL)
    {
	colnr_T	    scol, ecol;

	// Coladd is set to the width of the last character.
	getvcol(curwin, &curwin->w_cursor, &scol, ખાલી, &ecol, 0);
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
    char_u   *લીટી,
    પૂર્ણાંક      process,
    પૂર્ણાંક	     include_space,
    પૂર્ણાંક      *is_comment)
{
    char_u *comment_flags = ખાલી;
    પૂર્ણાંક    lead_len;
    પૂર્ણાંક    leader_offset = get_last_leader_offset(લીટી, &comment_flags);

    *is_comment = FALSE;
    જો (leader_offset != -1)
    {
	// Let's check whether the line ends with an unclosed comment.
	// If the last comment leader has COM_END in flags, there's no comment.
	જ્યારે (*comment_flags)
	{
	    જો (*comment_flags == COM_END
		    || *comment_flags == ':')
		તોડો;
	    ++comment_flags;
	}
	જો (*comment_flags != COM_END)
	    *is_comment = TRUE;
    }

    જો (process == FALSE)
	પાછા લીટી;

    lead_len = get_leader_len(લીટી, &comment_flags, FALSE, include_space);

    જો (lead_len == 0)
	પાછા લીટી;

    // Find:
    // - COM_END,
    // - colon,
    // whichever comes first.
    જ્યારે (*comment_flags)
    {
	જો (*comment_flags == COM_END
		|| *comment_flags == ':')
	    તોડો;
	++comment_flags;
    }

    // If we found a colon, it means that we are not processing a line
    // starting with a closing part of a three-part comment. That's good,
    // because we don't want to remove those as this would be annoying.
    જો (*comment_flags == ':' || *comment_flags == NUL)
	લીટી += lead_len;

    પાછા લીટી;
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
    પૂર્ણાંક
do_join(
    long    ગણતરી,
    પૂર્ણાંક	    ઉમેરો_space,
    પૂર્ણાંક	    save_undo,
    પૂર્ણાંક	    use_formatoptions UNUSED,
    પૂર્ણાંક	    setmark)
{
    char_u	*curr = ખાલી;
    char_u      *curr_start = ખાલી;
    char_u	*cend;
    char_u	*newp;
    size_t	newp_len;
    char_u	*spaces;	// number of spaces inserted before a line
    પૂર્ણાંક		endcurr1 = NUL;
    પૂર્ણાંક		endcurr2 = NUL;
    પૂર્ણાંક		currsize = 0;	// size of the current line
    પૂર્ણાંક		sumsize = 0;	// size of the long new line
    linenr_T	t;
    colnr_T	col = 0;
    પૂર્ણાંક		ret = OK;
    પૂર્ણાંક		*comments = ખાલી;
    પૂર્ણાંક		remove_comments = (use_formatoptions == TRUE)
				  && has_format_option(FO_REMOVE_COMS);
    પૂર્ણાંક		prev_was_comment;

    જો (save_undo && u_save((linenr_T)(curwin->w_cursor.lnum - 1),
			    (linenr_T)(curwin->w_cursor.lnum + ગણતરી)) == FAIL)
	પાછા FAIL;

    // Allocate an array to store the number of spaces inserted before each
    // line.  We will use it to pre-compute the length of the new line and the
    // proper placement of each original line in the new one.
    spaces = lalloc_clear(ગણતરી, TRUE);
    જો (spaces == ખાલી)
	પાછા FAIL;
    જો (remove_comments)
    {
	comments = lalloc_clear(ગણતરી * કદ(પૂર્ણાંક), TRUE);
	જો (comments == ખાલી)
	{
	    vim_free(spaces);
	    પાછા FAIL;
	}
    }

    /*
     * Don't move anything yet, just compute the final line length
     * and setup the array of space strings lengths
     * This loops forward over the joined lines.
     */
    માટે (t = 0; t < ગણતરી; ++t)
    {
	curr = curr_start = ml_get((linenr_T)(curwin->w_cursor.lnum + t));
	જો (t == 0 && setmark && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set the '[ mark.
	    curwin->w_buffer->b_op_start.lnum = curwin->w_cursor.lnum;
	    curwin->w_buffer->b_op_start.col  = (colnr_T)STRLEN(curr);
	}
	જો (remove_comments)
	{
	    // We don't want to remove the comment leader if the
	    // previous line is not a comment.
	    જો (t > 0 && prev_was_comment)
	    {

		char_u *new_curr = skip_comment(curr, TRUE, ઉમેરો_space,
							   &prev_was_comment);
		comments[t] = (પૂર્ણાંક)(new_curr - curr);
		curr = new_curr;
	    }
	    નહીંતર
		curr = skip_comment(curr, FALSE, ઉમેરો_space,
							   &prev_was_comment);
	}

	જો (ઉમેરો_space && t > 0)
	{
	    curr = skipwhite(curr);
	    જો (*curr != NUL && *curr != ')'
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
		જો (endcurr1 == ' ')
		    endcurr1 = endcurr2;
		નહીંતર
		    ++spaces[t];
		// extra space when 'joinspaces' set and line ends in '.'
		જો (       p_js
			&& (endcurr1 == '.'
			    || (vim_strchr(p_cpo, CPO_JOINSP) == ખાલી
				&& (endcurr1 == '?' || endcurr1 == '!'))))
		    ++spaces[t];
	    }
	}
	currsize = (પૂર્ણાંક)STRLEN(curr);
	sumsize += currsize + spaces[t];
	endcurr1 = endcurr2 = NUL;
	જો (ઉમેરો_space && currsize > 0)
	{
	    જો (has_mbyte)
	    {
		cend = curr + currsize;
		MB_PTR_BACK(curr, cend);
		endcurr1 = (*mb_ptr2char)(cend);
		જો (cend > curr)
		{
		    MB_PTR_BACK(curr, cend);
		    endcurr2 = (*mb_ptr2char)(cend);
		}
	    }
	    નહીંતર
	    {
		endcurr1 = *(curr + currsize - 1);
		જો (currsize > 1)
		    endcurr2 = *(curr + currsize - 2);
	    }
	}
	લીટી_breakcheck();
	જો (got_int)
	{
	    ret = FAIL;
	    જાઓ theend;
	}
    }

    // store the column position before last line
    col = sumsize - currsize - spaces[ગણતરી - 1];

    // allocate the space for the new line
    newp_len = sumsize + 1;
    newp = alloc(newp_len);
    જો (newp == ખાલી)
    {
	ret = FAIL;
	જાઓ theend;
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
    પૂર્ણાંક curr_off = (પૂર્ણાંક)(curr - curr_start);

    curr = curr_start = ml_get((linenr_T)(curwin->w_cursor.lnum + ગણતરી - 1));
    curr += curr_off;
#endif

    માટે (t = ગણતરી - 1; ; --t)
    {
	પૂર્ણાંક spaces_removed;

	cend -= currsize;
	mch_memmove(cend, curr, (size_t)currsize);

	જો (spaces[t] > 0)
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
		t == ગણતરી - 1, (long)(cend - newp), spaces_removed);
#endif
	જો (t == 0)
	    તોડો;
	curr = curr_start = ml_get((linenr_T)(curwin->w_cursor.lnum + t - 1));
	જો (remove_comments)
	    curr += comments[t - 1];
	જો (ઉમેરો_space && t > 1)
	    curr = skipwhite(curr);
	currsize = (પૂર્ણાંક)STRLEN(curr);
    }

#ifdef FEAT_PROP_POPUP
    જો (um.buf != ખાલી)
    {
	um_set_text(&um, newp);
	um_reverse_props(&um);
	um_close(&um);
	newp = ખાલી;  // um_set_text took ownership
    }
    નહીંતર
#endif
    ml_replace_len(curwin->w_cursor.lnum, newp, (colnr_T)newp_len, TRUE, FALSE);

    જો (setmark && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set the '] mark.
	curwin->w_buffer->b_op_end.lnum = curwin->w_cursor.lnum;
	curwin->w_buffer->b_op_end.col  = (colnr_T)sumsize;
    }

    // Only report the change in the first line here, del_lines() will report
    // the deleted line.
    changed_લીટીઓ(curwin->w_cursor.lnum, currsize,
					       curwin->w_cursor.lnum + 1, 0L);
    /*
     * Delete following lines. To do this we move the cursor there
     * briefly, and then move it back. After del_lines() the cursor may
     * have moved up (last line deleted), so the current lnum is kept in t.
     */
    t = curwin->w_cursor.lnum;
    ++curwin->w_cursor.lnum;
    del_લીટીઓ(ગણતરી - 1, FALSE);
    curwin->w_cursor.lnum = t;

    /*
     * Set the cursor column:
     * Vi compatible: use the column of the first join
     * vim:	      use the column of the last join
     */
    curwin->w_cursor.col =
		    (vim_strchr(p_cpo, CPO_JOINCOL) != ખાલી ? currsize : col);
    check_cursor_col();

    curwin->w_cursor.coladd = 0;
    curwin->w_set_curswant = true;

theend:
    vim_free(spaces);
    જો (remove_comments)
	vim_free(comments);
    પાછા ret;
}

#ifdef FEAT_LINEBREAK
/*
 * TODO: consider using "tailp" of win_lbr_chartabsize() instead of temporarily
 * resetting 'linebreak'.
 *
 * Reset 'linebreak' and take care of side effects.
 * Returns the previous value, to be passed to restore_lbr().
 */
    પૂર્ણાંક
reset_lbr(શૂન્ય)
{
    જો (!curwin->w_p_lbr)
	પાછા FALSE;
    // changing 'linebreak' may require w_virtcol to be updated
    curwin->w_p_lbr = FALSE;
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);
    પાછા TRUE;
}

/*
 * Restore 'linebreak' and take care of side effects.
 */
    શૂન્ય
restore_lbr(પૂર્ણાંક lbr_saved)
{
    જો (curwin->w_p_lbr || !lbr_saved)
	પાછા;

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
    શૂન્ય
block_prep(
    oparg_T		*oap,
    સંરચના block_def	*bdp,
    linenr_T		lnum,
    પૂર્ણાંક			is_del)
{
    પૂર્ણાંક		incr = 0;
    char_u	*pend;
    char_u	*pstart;
    char_u	*લીટી;
    char_u	*prev_pstart;
    char_u	*prev_pend;
    chartabsize_T cts;
#ifdef FEAT_LINEBREAK
		// Avoid a problem with unwanted linebreaks in block mode.
    પૂર્ણાંક		lbr_saved = reset_lbr();
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

    લીટી = ml_get(lnum);
    prev_pstart = લીટી;
    init_chartabsize_arg(&cts, curwin, lnum, bdp->start_vcol, લીટી, લીટી);
    જ્યારે (cts.cts_vcol < oap->start_vcol && *cts.cts_ptr != NUL)
    {
	// Count a tab for what it's worth (if list mode not on)
	incr = lbr_chartabsize(&cts);
	cts.cts_vcol += incr;
	જો (VIM_ISWHITE(*cts.cts_ptr))
	{
	    bdp->pre_whitesp += incr;
	    bdp->pre_whitesp_c++;
	}
	નહીંતર
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
    જો (bdp->start_vcol < oap->start_vcol)	// line too short
    {
	bdp->end_vcol = bdp->start_vcol;
	bdp->is_short = TRUE;
	જો (!is_del || oap->op_type == OP_APPEND)
	    bdp->endspaces = oap->end_vcol - oap->start_vcol + 1;
    }
    નહીંતર
    {
	// notice: this converts partly selected Multibyte characters to
	// spaces, too.
	bdp->startspaces = bdp->start_vcol - oap->start_vcol;
	જો (is_del && bdp->startspaces)
	    bdp->startspaces = bdp->start_char_vcols - bdp->startspaces;
	pend = pstart;
	bdp->end_vcol = bdp->start_vcol;
	જો (bdp->end_vcol > oap->end_vcol)	// it's all in one character
	{
	    bdp->is_oneChar = TRUE;
	    જો (oap->op_type == OP_ઉમેરો)
		bdp->endspaces = bdp->start_char_vcols - bdp->startspaces;
	    નહીંતર જો (oap->op_type == OP_APPEND)
	    {
		bdp->startspaces += oap->end_vcol - oap->start_vcol + 1;
		bdp->endspaces = bdp->start_char_vcols - bdp->startspaces;
	    }
	    નહીંતર
	    {
		bdp->startspaces = oap->end_vcol - oap->start_vcol + 1;
		જો (is_del && oap->op_type != OP_LSHIFT)
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
	નહીંતર
	{
	    init_chartabsize_arg(&cts, curwin, lnum, bdp->end_vcol,
								  લીટી, pend);
	    prev_pend = pend;
	    જ્યારે (cts.cts_vcol <= oap->end_vcol && *cts.cts_ptr != NUL)
	    {
		// count a tab for what it's worth (if list mode not on)
		prev_pend = cts.cts_ptr;
		incr = lbr_chartabsize_adv(&cts);
		cts.cts_vcol += incr;
	    }
	    bdp->end_vcol = cts.cts_vcol;
	    pend = cts.cts_ptr;
	    clear_chartabsize_arg(&cts);

	    જો (bdp->end_vcol <= oap->end_vcol
		    && (!is_del
			|| oap->op_type == OP_APPEND
			|| oap->op_type == OP_REPLACE)) // line too short
	    {
		bdp->is_short = TRUE;
		// Alternative: include spaces to fill up the block.
		// Disadvantage: can lead to trailing spaces when the line is
		// short where the text is put
		// if (!is_del || oap->op_type == OP_APPEND)
		જો (oap->op_type == OP_APPEND || virtual_op)
		    bdp->endspaces = oap->end_vcol - bdp->end_vcol
							     + oap->inclusive;
		નહીંતર
		    bdp->endspaces = 0; // replace doesn't add characters
	    }
	    નહીંતર જો (bdp->end_vcol > oap->end_vcol)
	    {
		bdp->endspaces = bdp->end_vcol - oap->end_vcol - 1;
		જો (!is_del && bdp->endspaces)
		{
		    bdp->endspaces = incr - bdp->endspaces;
		    જો (pend != pstart)
			pend = prev_pend;
		}
	    }
	}
	bdp->end_char_vcols = incr;
	જો (is_del && bdp->startspaces)
	    pstart = prev_pstart;
	bdp->textlen = (પૂર્ણાંક)(pend - pstart);
    }
    bdp->textcol = (colnr_T) (pstart - લીટી);
    bdp->textstart = pstart;
#ifdef FEAT_LINEBREAK
    restore_lbr(lbr_saved);
#endif
}

/*
 * Get block text from "start" to "end"
 */
    શૂન્ય
charwise_block_prep(
    pos_T		start,
    pos_T		end,
    સંરચના block_def	*bdp,
    linenr_T		lnum,
    પૂર્ણાંક			inclusive)
{
    colnr_T startcol = 0, endcol = MAXCOL;
    colnr_T cs, ce;
    char_u *p;
    પૂર્ણાંક	plen = ml_get_len(lnum);

    p = ml_get(lnum);
    bdp->startspaces = 0;
    bdp->endspaces = 0;
    bdp->is_oneChar = FALSE;
    bdp->start_char_vcols = 0;

    જો (lnum == start.lnum)
    {
	startcol = start.col;
	જો (virtual_op)
	{
	    getvcol(curwin, &start, &cs, ખાલી, &ce, 0);
	    જો (ce != cs && start.coladd > 0)
	    {
		// Part of a tab selected -- but don't
		// double-count it.
		bdp->start_char_vcols = ce - cs + 1;
		bdp->startspaces = bdp->start_char_vcols - start.coladd;
		જો (bdp->startspaces < 0)
		    bdp->startspaces = 0;
		startcol++;
	    }
	}
    }

    જો (lnum == end.lnum)
    {
	endcol = end.col;
	જો (virtual_op)
	{
	    getvcol(curwin, &end, &cs, ખાલી, &ce, 0);
	    જો (p[endcol] == NUL || (cs + end.coladd < ce
			// Don't add space for double-wide
			// char; endcol will be on last byte
			// of multi-byte char.
			&& (*mb_head_off)(p, p + endcol) == 0))
	    {
		જો (start.lnum == end.lnum && start.col == end.col)
		{
		    // Special case: inside a single char
		    bdp->is_oneChar = TRUE;
		    bdp->startspaces = end.coladd - start.coladd + inclusive;
		    endcol = startcol;
		}
		નહીંતર
		{
		    bdp->endspaces = end.coladd + inclusive;
		    endcol -= inclusive;
		}
	    }
	}
    }
    જો (endcol == MAXCOL)
	endcol = ml_get_len(lnum);
    જો (startcol > endcol || bdp->is_oneChar)
	bdp->textlen = 0;
    નહીંતર
	bdp->textlen = endcol - startcol + inclusive;
    bdp->textcol = startcol;
    bdp->textstart = startcol <= plen ? p + startcol : p;
}

/*
 * Handle the add/subtract operator.
 */
    શૂન્ય
op_addsub(
    oparg_T	*oap,
    linenr_T	Prenum1,	    // Amount of add/subtract
    પૂર્ણાંક		g_cmd)		    // was g<c-a>/g<c-x>
{
    pos_T		pos;
    સંરચના block_def	bd;
    પૂર્ણાંક			change_cnt = 0;
    linenr_T		amount = Prenum1;

    // do_addsub() might trigger re-evaluation of 'foldexpr' halfway, when the
    // buffer is not completely updated yet. Postpone updating folds until before
    // the call to changed_lines().
#ifdef FEAT_FOLDING
    disable_fold_update++;
#endif

    જો (!VIsual_active)
    {
	pos = curwin->w_cursor;
	જો (u_save_cursor() == FAIL)
	{
#ifdef FEAT_FOLDING
	    disable_fold_update--;
#endif
	    પાછા;
	}
	change_cnt = do_addsub(oap->op_type, &pos, 0, amount);
#ifdef FEAT_FOLDING
	disable_fold_update--;
#endif
	જો (change_cnt)
	    changed_લીટીઓ(pos.lnum, 0, pos.lnum + 1, 0L);
    }
    નહીંતર
    {
	પૂર્ણાંક	one_change;
	પૂર્ણાંક	length;
	pos_T	startpos;

	જો (u_save((linenr_T)(oap->start.lnum - 1),
					(linenr_T)(oap->end.lnum + 1)) == FAIL)
	{
#ifdef FEAT_FOLDING
	    disable_fold_update--;
#endif
	    પાછા;
	}

	pos = oap->start;
	માટે (; pos.lnum <= oap->end.lnum; ++pos.lnum)
	{
	    જો (oap->block_mode)		    // Visual block mode
	    {
		block_prep(oap, &bd, pos.lnum, FALSE);
		pos.col = bd.textcol;
		length = bd.textlen;
	    }
	    નહીંતર જો (oap->motion_type == MLINE)
	    {
		curwin->w_cursor.col = 0;
		pos.col = 0;
		length = ml_get_len(pos.lnum);
	    }
	    નહીંતર // oap->motion_type == MCHAR
	    {
		જો (pos.lnum == oap->start.lnum && !oap->inclusive)
		    dec(&(oap->end));
		length = ml_get_len(pos.lnum);
		pos.col = 0;
		જો (pos.lnum == oap->start.lnum)
		{
		    pos.col += oap->start.col;
		    length -= oap->start.col;
		}
		જો (pos.lnum == oap->end.lnum)
		{
		    length = ml_get_len(oap->end.lnum);
		    જો (oap->end.col >= length)
			oap->end.col = length - 1;
		    length = oap->end.col - pos.col + 1;
		}
	    }
	    one_change = do_addsub(oap->op_type, &pos, length, amount);
	    જો (one_change)
	    {
		// Remember the start position of the first change.
		જો (change_cnt == 0)
		    startpos = curbuf->b_op_start;
		++change_cnt;
	    }

#ifdef FEAT_NETBEANS_INTG
	    જો (netbeans_active() && one_change)
	    {
		char_u *ptr;

		netbeans_removed(curbuf, pos.lnum, pos.col, (long)length);
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, pos.col,
						&ptr[pos.col], length);
	    }
#endif
	    જો (g_cmd && one_change)
		amount += Prenum1;
	}

#ifdef FEAT_FOLDING
	disable_fold_update--;
#endif
	જો (change_cnt)
	    changed_લીટીઓ(oap->start.lnum, 0, oap->end.lnum + 1, 0L);

	જો (!change_cnt && oap->is_VIsual)
	    // No change: need to remove the Visual selection
	    redraw_curbuf_later(UPD_INVERTED);

	// Set '[ mark if something changed. Keep the last end
	// position from do_addsub().
	જો (change_cnt > 0 && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	    curbuf->b_op_start = startpos;

	જો (change_cnt > p_report)
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
    સ્થિર પૂર્ણાંક
do_addsub(
    પૂર્ણાંક		op_type,
    pos_T	*pos,
    પૂર્ણાંક		length,
    linenr_T	Prenum1)
{
    પૂર્ણાંક		col;
    પૂર્ણાંક		pre;		// 'X'/'x': hex; '0': octal; 'B'/'b': bin
    સ્થિર પૂર્ણાંક	hexupper = FALSE;	// 0xABC
    uvarnumber_T	n;
    uvarnumber_T	oldn;
    char_u	*ptr;
    પૂર્ણાંક		linelen;
    પૂર્ણાંક		c;
    પૂર્ણાંક		todel;
    પૂર્ણાંક		do_hex;
    પૂર્ણાંક		do_oct;
    પૂર્ણાંક		do_bin;
    પૂર્ણાંક		do_alpha;
    પૂર્ણાંક		do_unsigned;
    પૂર્ણાંક		do_blank;
    પૂર્ણાંક		blank_unsigned = FALSE;	// blank: treat as unsigned?
    પૂર્ણાંક		firstdigit;
    પૂર્ણાંક		subtract;
    પૂર્ણાંક		negative = FALSE;
    પૂર્ણાંક		was_positive = TRUE;
    પૂર્ણાંક		visual = VIsual_active;
    પૂર્ણાંક		did_change = FALSE;
    pos_T	save_cursor = curwin->w_cursor;
    પૂર્ણાંક		maxlen = 0;
    pos_T	startpos;
    pos_T	endpos;
    colnr_T	save_coladd = 0;

    do_hex = (vim_strchr(curbuf->b_p_nf, 'x') != ખાલી);	// "heX"
    do_oct = (vim_strchr(curbuf->b_p_nf, 'o') != ખાલી);	// "Octal"
    do_bin = (vim_strchr(curbuf->b_p_nf, 'b') != ખાલી);	// "Bin"
    do_alpha = (vim_strchr(curbuf->b_p_nf, 'p') != ખાલી);	// "alPha"
    do_unsigned = (vim_strchr(curbuf->b_p_nf, 'u') != ખાલી);	// "Unsigned"
    do_blank = (vim_strchr(curbuf->b_p_nf, 'k') != ખાલી);	// "blanK"

    જો (virtual_active())
    {
	save_coladd = pos->coladd;
	pos->coladd = 0;
    }

    curwin->w_cursor = *pos;
    ptr = ml_get(pos->lnum);
    linelen = ml_get_len(pos->lnum);
    col = pos->col;

    જો (col + !!save_coladd >= linelen)
	જાઓ theend;

    /*
     * First check if we are on a hexadecimal number, after the "0x".
     */
    જો (!VIsual_active)
    {
	જો (do_bin)
	    જ્યારે (col > 0 && vim_isbdigit(ptr[col]))
	    {
		--col;
		જો (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }

	જો (do_hex)
	    જ્યારે (col > 0 && vim_isxdigit(ptr[col]))
	    {
		--col;
		જો (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }

	જો (       do_bin
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

	    જ્યારે (col > 0 && vim_isdigit(ptr[col]))
	    {
		col--;
		જો (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }
	}

	જો ((       do_hex
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
	    જો (has_mbyte)
		col -= (*mb_head_off)(ptr, ptr + col);
	}
	નહીંતર
	{
	    /*
	     * Search forward and then backward to find the start of number.
	     */
	    col = pos->col;

	    જ્યારે (ptr[col] != NUL
		    && !vim_isdigit(ptr[col])
		    && !(do_alpha && ASCII_ISALPHA(ptr[col])))
		col += mb_ptr2len(ptr + col);

	    જ્યારે (col > 0
		    && vim_isdigit(ptr[col - 1])
		    && !(do_alpha && ASCII_ISALPHA(ptr[col])))
	    {
		--col;
		જો (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }
	}
    }

    જો (visual)
    {
	જ્યારે (ptr[col] != NUL && length > 0
		&& !vim_isdigit(ptr[col])
		&& !(do_alpha && ASCII_ISALPHA(ptr[col])))
	{
	    પૂર્ણાંક mb_len = mb_ptr2len(ptr + col);

	    col += mb_len;
	    length -= mb_len;
	}

	જો (length == 0)
	    જાઓ theend;

	જો (col > pos->col && ptr[col - 1] == '-'
		&& (!has_mbyte || !(*mb_head_off)(ptr, ptr + col - 1))
		&& !do_unsigned)
	{
	    જો (do_blank && col >= 2 && !VIM_ISWHITE(ptr[col - 2]))
		blank_unsigned = TRUE;
	    નહીંતર
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
    જો (!VIM_ISDIGIT(firstdigit) && !(do_alpha && ASCII_ISALPHA(firstdigit)))
    {
	beep_flush();
	જાઓ theend;
    }

    જો (do_alpha && ASCII_ISALPHA(firstdigit))
    {
	// decrement or increment alphabetic character
	જો (op_type == OP_NR_SUB)
	{
	    જો (CharOrd(firstdigit) < Prenum1)
	    {
		જો (SAFE_isupper(firstdigit))
		    firstdigit = 'A';
		નહીંતર
		    firstdigit = 'a';
	    }
	    નહીંતર
		firstdigit -= Prenum1;
	}
	નહીંતર
	{
	    જો (26 - CharOrd(firstdigit) - 1 < Prenum1)
	    {
		જો (SAFE_isupper(firstdigit))
		    firstdigit = 'Z';
		નહીંતર
		    firstdigit = 'z';
	    }
	    નહીંતર
		firstdigit += Prenum1;
	}
	curwin->w_cursor.col = col;
	જો (!did_change)
	    startpos = curwin->w_cursor;
	did_change = TRUE;
	(શૂન્ય)del_char(FALSE);
	ins_char(firstdigit);
	endpos = curwin->w_cursor;
	curwin->w_cursor.col = col;
    }
    નહીંતર
    {
	char_u	*buf1;
	પૂર્ણાંક	buf1len;
	char_u	buf2[NUMBUFLEN];
	પૂર્ણાંક	buf2len;
	pos_T	save_pos;
	પૂર્ણાંક	i;

	જો (col > 0 && ptr[col - 1] == '-'
		&& (!has_mbyte ||
		    !(*mb_head_off)(ptr, ptr + col - 1))
		&& !visual
		&& !do_unsigned)
	{
	    જો (do_blank && col >= 2 && !VIM_ISWHITE(ptr[col - 2]))
		blank_unsigned = TRUE;
	    નહીંતર
	    {
		// negative number
		--col;
		negative = TRUE;
	    }
	}

	// get the number value (unsigned)
	જો (visual && VIsual_mode != 'V')
	    maxlen = (curbuf->b_visual.vi_curswant == MAXCOL
		    ? linelen - col : length);

	પૂર્ણાંક overflow = FALSE;
	vim_str2nr(ptr + col, &pre, &length,
		0 + (do_bin ? STR2NR_BIN : 0)
		    + (do_oct ? STR2NR_OCT : 0)
		    + (do_hex ? STR2NR_HEX : 0),
		ખાલી, &n, maxlen, FALSE, &overflow);

	// ignore leading '-' for hex and octal and bin numbers
	જો (pre && negative)
	{
	    ++col;
	    --length;
	    negative = FALSE;
	}
	// add or subtract
	subtract = FALSE;
	જો (op_type == OP_NR_SUB)
	    subtract ^= TRUE;
	જો (negative)
	    subtract ^= TRUE;

	oldn = n;
	જો (!overflow)  // if number is too big don't add/subtract
	{
	    જો (subtract)
		n -= (uvarnumber_T)Prenum1;
	    નહીંતર
		n += (uvarnumber_T)Prenum1;
	}

	// handle wraparound for decimal numbers
	જો (!pre)
	{
	    જો (subtract)
	    {
		જો (n > oldn)
		{
		    n = 1 + (n ^ (uvarnumber_T)-1);
		    negative ^= TRUE;
		}
	    }
	    નહીંતર
	    {
		// add
		જો (n < oldn)
		{
		    n = (n ^ (uvarnumber_T)-1);
		    negative ^= TRUE;
		}
	    }
	    જો (n == 0)
		negative = FALSE;
	}

	જો ((do_unsigned || blank_unsigned) && negative)
	{
	    જો (subtract)
		// sticking at zero.
		n = (uvarnumber_T)0;
	    નહીંતર
		// sticking at 2^64 - 1.
		n = (uvarnumber_T)(-1);
	    negative = FALSE;
	}

	જો (visual && !was_positive && !negative && col > 0)
	{
	    // need to remove the '-'
	    col--;
	    length++;
	}

	/*
	 * Delete the old number.
	 */
	curwin->w_cursor.col = col;
	જો (!did_change)
	    startpos = curwin->w_cursor;
	did_change = TRUE;
	todel = length;
	c = gchar_cursor();
	/*
	 * Don't include the '-' in the length, only the length of the
	 * part after it is kept the same.
	 */
	જો (c == '-')
	    --length;

	save_pos = curwin->w_cursor;
	માટે (i = 0; i < todel; ++i)
	{
	    જો (c < 0x100 && SAFE_isalpha(c))
	    {
		જો (SAFE_isupper(c))
		    hexupper = TRUE;
		નહીંતર
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
	જો (buf1 == ખાલી)
	    જાઓ theend;
	ptr = buf1;
	જો (negative && (!visual || was_positive))
	    *ptr++ = '-';
	જો (pre)
	{
	    *ptr++ = '0';
	    --length;
	}
	જો (pre == 'b' || pre == 'B' ||
	    pre == 'x' || pre == 'X')
	{
	    *ptr++ = pre;
	    --length;
	}

	/*
	 * Put the number characters in buf2[].
	 */
	જો (pre == 'b' || pre == 'B')
	{
	    પૂર્ણાંક bit = 0;
	    પૂર્ણાંક bits = કદ(uvarnumber_T) * 8;

	    // leading zeros
	    માટે (bit = bits; bit > 0; bit--)
		જો ((n >> (bit - 1)) & 0x1) તોડો;

	    માટે (buf2len = 0; bit > 0 && buf2len < (NUMBUFLEN - 1); bit--)
		buf2[buf2len++] = ((n >> (bit - 1)) & 0x1) ? '1' : '0';

	    buf2[buf2len] = NUL;
	}
	નહીંતર જો (pre == 0)
	    buf2len = vim_snprintf((અક્ષર *)buf2, NUMBUFLEN, "%llu", n);
	નહીંતર જો (pre == '0')
	    buf2len = vim_snprintf((અક્ષર *)buf2, NUMBUFLEN, "%llo", n);
	નહીંતર જો (pre && hexupper)
	    buf2len = vim_snprintf((અક્ષર *)buf2, NUMBUFLEN, "%llX", n);
	નહીંતર
	    buf2len = vim_snprintf((અક્ષર *)buf2, NUMBUFLEN, "%llx", n);
	length -= buf2len;

	/*
	 * Adjust number of zeros to the new number of digits, so the
	 * total length of the number remains the same.
	 * Don't do this when
	 * the result may look like an octal number.
	 */
	જો (firstdigit == '0' && !(do_oct && pre == 0))
	    જ્યારે (length-- > 0)
		*ptr++ = '0';
	*ptr = NUL;
	buf1len = (પૂર્ણાંક)(ptr - buf1);

	STRCPY(buf1 + buf1len, buf2);
	buf1len += buf2len;

	// Insert just after the first character to be removed, so that any
	// text properties will be adjusted.  Then delete the old number
	// afterwards.
	save_pos = curwin->w_cursor;
	જો (todel > 0)
	    inc_cursor();
	ins_str(buf1, (size_t)buf1len);		// insert the new number
	vim_free(buf1);

	// del_char() will also mark line needing displaying
	જો (todel > 0)
	{
	    પૂર્ણાંક bytes_after = ml_get_curline_len() - curwin->w_cursor.col;

	    // Delete the one character before the insert.
	    curwin->w_cursor = save_pos;
	    (શૂન્ય)del_char(FALSE);
	    curwin->w_cursor.col = ml_get_curline_len() - bytes_after;
	    --todel;
	}
	જ્યારે (todel-- > 0)
	    (શૂન્ય)del_char(FALSE);

	endpos = curwin->w_cursor;
	જો (did_change && curwin->w_cursor.col)
	    --curwin->w_cursor.col;
    }

    જો (did_change && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// set the '[ and '] marks
	curbuf->b_op_start = startpos;
	curbuf->b_op_end = endpos;
	જો (curbuf->b_op_end.col > 0)
	    --curbuf->b_op_end.col;
    }

theend:
    જો (visual)
	curwin->w_cursor = save_cursor;
    નહીંતર જો (did_change)
	curwin->w_set_curswant = true;
    નહીંતર જો (virtual_active())
	curwin->w_cursor.coladd = save_coladd;

    પાછા did_change;
}

    શૂન્ય
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
    સ્થિર varnumber_T
લીટી_ગણતરી_info(
    char_u	*લીટી,
    varnumber_T	*wc,
    varnumber_T	*cc,
    varnumber_T	limit,
    પૂર્ણાંક		eol_size)
{
    varnumber_T	i;
    varnumber_T	શબ્દો = 0;
    varnumber_T	chars = 0;
    પૂર્ણાંક		is_word = 0;

    માટે (i = 0; i < limit && લીટી[i] != NUL; )
    {
	જો (is_word)
	{
	    જો (vim_isspace(લીટી[i]))
	    {
		શબ્દો++;
		is_word = 0;
	    }
	}
	નહીંતર જો (!vim_isspace(લીટી[i]))
	    is_word = 1;
	++chars;
	i += (*mb_ptr2len)(લીટી + i);
    }

    જો (is_word)
	શબ્દો++;
    *wc += શબ્દો;

    // Add eol_size if the end of line was reached before hitting limit.
    જો (i < limit && લીટી[i] == NUL)
    {
	i += eol_size;
	chars += eol_size;
    }
    *cc += chars;
    પાછા i;
}

/*
 * Give some info about the position of the cursor (for "g CTRL-G").
 * In Visual mode, give some info about the selected region.  (In this case,
 * the *_count_cursor variables store running totals for the selection.)
 * When "dict" is not NULL store the info there instead of showing it.
 */
    શૂન્ય
cursor_pos_info(dict_T *dict)
{
    char_u	*p;
    char_u	buf1[50];
    char_u	buf2[40];
    linenr_T	lnum;
    varnumber_T	byte_ગણતરી = 0;
    varnumber_T	bom_ગણતરી  = 0;
    varnumber_T	byte_ગણતરી_cursor = 0;
    varnumber_T	char_ગણતરી = 0;
    varnumber_T	char_ગણતરી_cursor = 0;
    varnumber_T	word_ગણતરી = 0;
    varnumber_T	word_ગણતરી_cursor = 0;
    પૂર્ણાંક		eol_size;
    varnumber_T	last_check = 100000L;
    long	લીટી_ગણતરી_selected = 0;
    pos_T	min_pos, max_pos;
    oparg_T	oparg;
    સંરચના block_def	bd;

    /*
     * Compute the length of the file in characters.
     */
    જો (curbuf->b_ml.ml_flags & ML_EMPTY)
    {
	જો (dict == ખાલી)
	{
	    સંદેશ(_(no_લીટીઓ_સંદેશ));
	    પાછા;
	}
    }
    નહીંતર
    {
	જો (get_fileformat(curbuf) == EOL_DOS)
	    eol_size = 2;
	નહીંતર
	    eol_size = 1;

	જો (VIsual_active)
	{
	    જો (LT_POS(VIsual, curwin->w_cursor))
	    {
		min_pos = VIsual;
		max_pos = curwin->w_cursor;
	    }
	    નહીંતર
	    {
		min_pos = curwin->w_cursor;
		max_pos = VIsual;
	    }
	    જો (*p_sel == 'e' && max_pos.col > 0)
		--max_pos.col;

	    જો (VIsual_mode == Ctrl_V)
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
		જો (curwin->w_curswant == MAXCOL)
		    oparg.end_vcol = MAXCOL;
		// Swap the start, end vcol if needed
		જો (oparg.end_vcol < oparg.start_vcol)
		{
		    oparg.end_vcol += oparg.start_vcol;
		    oparg.start_vcol = oparg.end_vcol - oparg.start_vcol;
		    oparg.end_vcol -= oparg.start_vcol;
		}
	    }
	    લીટી_ગણતરી_selected = max_pos.lnum - min_pos.lnum + 1;
	}

	માટે (lnum = 1; lnum <= curbuf->b_ml.ml_લીટી_ગણતરી; ++lnum)
	{
	    // Check for a CTRL-C every 100000 characters.
	    જો (byte_ગણતરી > last_check)
	    {
		ui_breakcheck();
		જો (got_int)
		    પાછા;
		last_check = byte_ગણતરી + 100000L;
	    }

	    // Do extra processing for VIsual mode.
	    જો (VIsual_active
		    && lnum >= min_pos.lnum && lnum <= max_pos.lnum)
	    {
		char_u	    *s = ખાલી;
		long	    len = 0L;

		પસંદગી (VIsual_mode)
		{
		    સ્થિતિ Ctrl_V:
			virtual_op = virtual_active();
			block_prep(&oparg, &bd, lnum, 0);
			virtual_op = MAYBE;
			s = bd.textstart;
			len = (long)bd.textlen;
			તોડો;
		    સ્થિતિ 'V':
			s = ml_get(lnum);
			len = MAXCOL;
			તોડો;
		    સ્થિતિ 'v':
			{
			    colnr_T start_col = (lnum == min_pos.lnum)
							   ? min_pos.col : 0;
			    colnr_T end_col = (lnum == max_pos.lnum)
				      ? max_pos.col - start_col + 1 : MAXCOL;

			    s = ml_get(lnum) + start_col;
			    len = end_col;
			}
			તોડો;
		}
		જો (s != ખાલી)
		{
		    byte_ગણતરી_cursor += લીટી_ગણતરી_info(s, &word_ગણતરી_cursor,
					   &char_ગણતરી_cursor, len, eol_size);
		    જો (lnum == curbuf->b_ml.ml_લીટી_ગણતરી
			    && !curbuf->b_p_eol
			    && (curbuf->b_p_bin || !curbuf->b_p_fixeol)
			    && (long)STRLEN(s) < len)
			byte_ગણતરી_cursor -= eol_size;
		}
	    }
	    નહીંતર
	    {
		// In non-visual mode, check for the line the cursor is on
		જો (lnum == curwin->w_cursor.lnum)
		{
		    word_ગણતરી_cursor += word_ગણતરી;
		    char_ગણતરી_cursor += char_ગણતરી;
		    byte_ગણતરી_cursor = byte_ગણતરી +
			લીટી_ગણતરી_info(ml_get(lnum),
				&word_ગણતરી_cursor, &char_ગણતરી_cursor,
				(varnumber_T)(curwin->w_cursor.col + 1),
				eol_size);
		}
	    }
	    // Add to the running totals
	    byte_ગણતરી += લીટી_ગણતરી_info(ml_get(lnum), &word_ગણતરી,
					 &char_ગણતરી, (varnumber_T)MAXCOL,
					 eol_size);
	}

	// Correction for when last line doesn't have an EOL.
	જો (!curbuf->b_p_eol && (curbuf->b_p_bin || !curbuf->b_p_fixeol))
	    byte_ગણતરી -= eol_size;

	જો (dict == ખાલી)
	{
	    જો (VIsual_active)
	    {
		જો (VIsual_mode == Ctrl_V && curwin->w_curswant < MAXCOL)
		{
		    getvcols(curwin, &min_pos, &max_pos,
						&min_pos.col, &max_pos.col, 0);
		    vim_snprintf((અક્ષર *)buf1, કદ(buf1), _("%ld Cols; "),
			    (long)(oparg.end_vcol - oparg.start_vcol + 1));
		}
		નહીંતર
		    buf1[0] = NUL;

		જો (char_ગણતરી_cursor == byte_ગણતરી_cursor
						    && char_ગણતરી == byte_ગણતરી)
		    vim_snprintf((અક્ષર *)IObuff, IOSIZE,
			    _("Selected %s%ld of %ld Lines; %lld of %lld Words; %lld of %lld Bytes"),
			    buf1, લીટી_ગણતરી_selected,
			    (long)curbuf->b_ml.ml_લીટી_ગણતરી,
			    word_ગણતરી_cursor,
			    word_ગણતરી,
			    byte_ગણતરી_cursor,
			    byte_ગણતરી);
		નહીંતર
		    vim_snprintf((અક્ષર *)IObuff, IOSIZE,
			    _("Selected %s%ld of %ld Lines; %lld of %lld Words; %lld of %lld Chars; %lld of %lld Bytes"),
			    buf1, લીટી_ગણતરી_selected,
			    (long)curbuf->b_ml.ml_લીટી_ગણતરી,
			    word_ગણતરી_cursor,
			    word_ગણતરી,
			    char_ગણતરી_cursor,
			    char_ગણતરી,
			    byte_ગણતરી_cursor,
			    byte_ગણતરી);
	    }
	    નહીંતર
	    {
		p = ml_get_curline();
		validate_virtcol();
		col_print(buf1, કદ(buf1), (પૂર્ણાંક)curwin->w_cursor.col + 1,
			(પૂર્ણાંક)curwin->w_virtcol + 1);
		col_print(buf2, કદ(buf2), ml_get_curline_len(),
							   linetabsize_str(p));

		જો (char_ગણતરી_cursor == byte_ગણતરી_cursor
			&& char_ગણતરી == byte_ગણતરી)
		    vim_snprintf((અક્ષર *)IObuff, IOSIZE,
			_("Col %s of %s; Line %ld of %ld; Word %lld of %lld; Byte %lld of %lld"),
			(અક્ષર *)buf1, (અક્ષર *)buf2,
			(long)curwin->w_cursor.lnum,
			(long)curbuf->b_ml.ml_લીટી_ગણતરી,
			word_ગણતરી_cursor, word_ગણતરી,
			byte_ગણતરી_cursor, byte_ગણતરી);
		નહીંતર
		    vim_snprintf((અક્ષર *)IObuff, IOSIZE,
			_("Col %s of %s; Line %ld of %ld; Word %lld of %lld; Char %lld of %lld; Byte %lld of %lld"),
			(અક્ષર *)buf1, (અક્ષર *)buf2,
			(long)curwin->w_cursor.lnum,
			(long)curbuf->b_ml.ml_લીટી_ગણતરી,
			word_ગણતરી_cursor, word_ગણતરી,
			char_ગણતરી_cursor, char_ગણતરી,
			byte_ગણતરી_cursor, byte_ગણતરી);
	    }
	}

	bom_ગણતરી = bomb_size();
	જો (dict == ખાલી && bom_ગણતરી > 0)
	{
	    size_t len = STRLEN(IObuff);

	    vim_snprintf((અક્ષર *)IObuff + len, IOSIZE - len,
				 _("(+%lld for BOM)"), bom_ગણતરી);
	}
	જો (dict == ખાલી)
	{
	    // Don't shorten this message, the user asked for it.
	    p = p_shm;
	    p_shm = (char_u *)"";
	    સંદેશ((અક્ષર *)IObuff);
	    p_shm = p;
	}
    }
#જો defined(FEAT_EVAL)
    જો (dict != ખાલી)
    {
	dict_add_સંખ્યા(dict, "words", word_ગણતરી);
	dict_add_સંખ્યા(dict, "chars", char_ગણતરી);
	dict_add_સંખ્યા(dict, "bytes", byte_ગણતરી + bom_ગણતરી);
	dict_add_સંખ્યા(dict, VIsual_active ? "visual_bytes" : "cursor_bytes",
		byte_ગણતરી_cursor);
	dict_add_સંખ્યા(dict, VIsual_active ? "visual_chars" : "cursor_chars",
		char_ગણતરી_cursor);
	dict_add_સંખ્યા(dict, VIsual_active ? "visual_words" : "cursor_words",
		word_ગણતરી_cursor);
    }
#endif
}

/*
 * Handle indent and format operators and visual mode ":".
 */
    સ્થિર શૂન્ય
op_colon(oparg_T *oap)
{
    stuffcharReadbuff(':');
    જો (oap->is_VIsual)
	stuffReadbuff((char_u *)"'<,'>");
    નહીંતર
    {
	// Make the range look nice, so it can be repeated.
	જો (oap->start.lnum == curwin->w_cursor.lnum)
	    stuffcharReadbuff('.');
	નહીંતર
	    stuffnumReadbuff((long)oap->start.lnum);

#ifdef FEAT_FOLDING
	// When using !! on a closed fold the range ".!" works best to operate
	// on, it will be made the whole closed fold later.
	linenr_T endOfStartFold = oap->start.lnum;
	(શૂન્ય)hasFolding(oap->start.lnum, ખાલી, &endOfStartFold);
#endif
	જો (oap->end.lnum != oap->start.lnum
#ifdef FEAT_FOLDING
		&& oap->end.lnum != endOfStartFold
#endif
	   )
	{
	    // Make it a range with the end line.
	    stuffcharReadbuff(',');
	    જો (oap->end.lnum == curwin->w_cursor.lnum)
		stuffcharReadbuff('.');
	    નહીંતર જો (oap->end.lnum == curbuf->b_ml.ml_લીટી_ગણતરી)
		stuffcharReadbuff('$');
	    નહીંતર જો (oap->start.lnum == curwin->w_cursor.lnum
#ifdef FEAT_FOLDING
		    // do not use ".+number" for a closed fold, it would count
		    // folded lines twice
		    && !hasFolding(oap->end.lnum, ખાલી, ખાલી)
#endif
		    )
	    {
		stuffReadbuff((char_u *)".+");
		stuffnumReadbuff((long)oap->લીટી_ગણતરી - 1);
	    }
	    નહીંતર
		stuffnumReadbuff((long)oap->end.lnum);
	}
    }
    જો (oap->op_type != OP_COLON)
	stuffReadbuff((char_u *)"!");
    જો (oap->op_type == OP_INDENT)
    {
	જો (*get_equalprg() == NUL)
	    stuffReadbuff((char_u *)"indent");
	નહીંતર
	    stuffReadbuff(get_equalprg());
	stuffReadbuff((char_u *)"\n");
    }
    નહીંતર જો (oap->op_type == OP_FORMAT)
    {
	જો (*curbuf->b_p_fp != NUL)
	    stuffReadbuff(curbuf->b_p_fp);
	નહીંતર જો (*p_fp != NUL)
	    stuffReadbuff(p_fp);
	નહીંતર
	    stuffReadbuff((char_u *)"fmt");
	stuffReadbuff((char_u *)"\n']");
    }

    // do_cmdline() does the rest
}

// callback function for 'operatorfunc'
સ્થિર callback_T opfunc_cb;

/*
 * Process the 'operatorfunc' option value.
 * Returns OK or FAIL.
 */
    અક્ષર *
did_set_operatorfunc(optset_T *args UNUSED)
{
    જો (option_set_callback_func(p_opfunc, &opfunc_cb) == FAIL)
	પાછા e_invalid_argument;

    પાછા ખાલી;
}

#જો defined(EXITFREE)
    શૂન્ય
free_operatorfunc_option(શૂન્ય)
{
# ifdef FEAT_EVAL
    free_callback(&opfunc_cb);
# endif
}
#endif

#જો defined(FEAT_EVAL)
/*
 * Mark the global 'operatorfunc' callback with "copyID" so that it is not
 * garbage collected.
 */
    પૂર્ણાંક
set_ref_in_opfunc(પૂર્ણાંક copyID UNUSED)
{
    પૂર્ણાંક abort = FALSE;

    abort = set_ref_in_callback(&opfunc_cb, copyID);

    પાછા abort;
}
#endif

/*
 * Handle the "g@" operator: call 'operatorfunc'.
 */
    સ્થિર શૂન્ય
op_function(oparg_T *oap UNUSED)
{
#ifdef FEAT_EVAL
    typval_T	argv[2];
    pos_T	orig_start = curbuf->b_op_start;
    pos_T	orig_end = curbuf->b_op_end;
    typval_T	rettv;

    જો (*p_opfunc == NUL)
	emsg(_(e_operatorfunc_is_empty));
    નહીંતર
    {
	// Set '[ and '] marks to text to be operated on.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
	જો (oap->motion_type != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_end);

	argv[0].v_type = VAR_STRING;
	જો (oap->block_mode)
	    argv[0].vval.v_string = (char_u *)"block";
	નહીંતર જો (oap->motion_type == MLINE)
	    argv[0].vval.v_string = (char_u *)"line";
	નહીંતર
	    argv[0].vval.v_string = (char_u *)"char";
	argv[1].v_type = VAR_UNKNOWN;

	// Reset virtual_op so that 'virtualedit' can be changed in the
	// function.
	પૂર્ણાંક save_virtual_op = virtual_op;
	virtual_op = MAYBE;

	// Reset finish_op so that mode() returns the right value.
	પૂર્ણાંક save_finish_op = finish_op;
	finish_op = FALSE;

	જો (call_callback(&opfunc_cb, 0, &rettv, 1, argv) != FAIL)
	    clear_tv(&rettv);

	virtual_op = save_virtual_op;
	finish_op = save_finish_op;
	જો (cmdmod.cmod_flags & CMOD_LOCKMARKS)
	{
	    curbuf->b_op_start = orig_start;
	    curbuf->b_op_end = orig_end;
	}
    }
#નહીંતર
    emsg(_(e_eval_feature_not_available));
#endif
}

/*
 * Calculate start/end virtual columns for operating in block mode.
 */
    સ્થિર શૂન્ય
get_op_vcol(
    oparg_T	*oap,
    colnr_T	redo_VIsual_vcol,
    પૂર્ણાંક		initial)    // when TRUE adjust position for 'selectmode'
{
    colnr_T	    start, end;

    જો (VIsual_mode != Ctrl_V
	    || (!initial && oap->end.col < curwin->w_width))
	પાછા;

    oap->block_mode = TRUE;

    // prevent from moving onto a trail byte
    જો (has_mbyte)
	mb_adjustpos(curwin->w_buffer, &oap->end);

    getvvcol(curwin, &(oap->start), &oap->start_vcol, ખાલી, &oap->end_vcol, 0);

    જો (!redo_VIsual_busy)
    {
	getvvcol(curwin, &(oap->end), &start, ખાલી, &end, 0);

	જો (start < oap->start_vcol)
	    oap->start_vcol = start;
	જો (end > oap->end_vcol)
	{
	    જો (initial && *p_sel == 'e' && start >= 1
				    && start - 1 >= oap->end_vcol)
		oap->end_vcol = start - 1;
	    નહીંતર
		oap->end_vcol = end;
	}
    }

    // if '$' was used, get oap->end_vcol from longest line
    જો (curwin->w_curswant == MAXCOL)
    {
	curwin->w_cursor.col = MAXCOL;
	oap->end_vcol = 0;
	માટે (curwin->w_cursor.lnum = oap->start.lnum;
		curwin->w_cursor.lnum <= oap->end.lnum;
					++curwin->w_cursor.lnum)
	{
	    getvvcol(curwin, &curwin->w_cursor, ખાલી, ખાલી, &end, 0);
	    જો (end > oap->end_vcol)
		oap->end_vcol = end;
	}
    }
    નહીંતર જો (redo_VIsual_busy)
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
પ્રકારવ્યાખ્યા સંરચના {
    પૂર્ણાંક		rv_mode;	// 'v', 'V', or Ctrl-V
    linenr_T	rv_લીટી_ગણતરી;	// number of lines
    colnr_T	rv_vcol;	// number of cols or end column
    long	rv_ગણતરી;	// count for Visual operator
    પૂર્ણાંક		rv_arg;		// extra argument
} redo_VIsual_T;

    સ્થિર પૂર્ણાંક
is_ex_cmdchar(cmdarg_T *cap)
{
    પાછા cap->cmdchar == ':'
	|| cap->cmdchar == K_COMMAND
	|| cap->cmdchar == K_SCRIPT_COMMAND;
}

/*
 * Handle an operator after Visual mode or when the movement is finished.
 * "gui_yank" is true when yanking text for the clipboard.
 */
    શૂન્ય
do_pending_operator(cmdarg_T *cap, પૂર્ણાંક old_col, પૂર્ણાંક gui_yank)
{
    oparg_T	*oap = cap->oap;
    pos_T	old_cursor;
    પૂર્ણાંક		empty_region_error;
    પૂર્ણાંક		restart_edit_save;
#ifdef FEAT_LINEBREAK
    પૂર્ણાંક		lbr_saved = curwin->w_p_lbr;
#endif

    // The visual area is remembered for redo
    સ્થિર redo_VIsual_T   redo_VIsual = {NUL, 0, 0, 0,0};

    પૂર્ણાંક		    include_લીટી_break = FALSE;

#જો defined(FEAT_CLIPBOARD)
    // Yank the visual area into the GUI selection register before we operate
    // on it and lose it forever.
    // Don't do it if a specific register was specified, so that ""x"*P works.
    // This could call do_pending_operator() recursively, but that's OK
    // because gui_yank will be TRUE for the nested call.
    જો ((clip_star.available || clip_plus.available)
	    && oap->op_type != OP_NOP
	    && !gui_yank
	    && VIsual_active
	    && !redo_VIsual_busy
	    && oap->regname == 0)
	clip_auto_select();
#endif
    old_cursor = curwin->w_cursor;

    // If an operation is pending, handle it...
    જો ((finish_op || VIsual_active) && oap->op_type != OP_NOP)
    {
	// Yank can be redone when 'y' is in 'cpoptions', but not when yanking
	// for the clipboard.
	પૂર્ણાંક	redo_yank = vim_strchr(p_cpo, CPO_YANK) != ખાલી && !gui_yank;

#ifdef FEAT_LINEBREAK
	// Avoid a problem with unwanted linebreaks in block mode.
	(શૂન્ય)reset_lbr();
#endif
	oap->is_VIsual = VIsual_active;
	જો (oap->motion_force == 'V')
	    oap->motion_type = MLINE;
	નહીંતર જો (oap->motion_force == 'v')
	{
	    // If the motion was linewise, "inclusive" will not have been set.
	    // Use "exclusive" to be consistent.  Makes "dvj" work nice.
	    જો (oap->motion_type == MLINE)
		oap->inclusive = FALSE;
	    // If the motion already was characterwise, toggle "inclusive"
	    નહીંતર જો (oap->motion_type == MCHAR)
		oap->inclusive = !oap->inclusive;
	    oap->motion_type = MCHAR;
	}
	નહીંતર જો (oap->motion_force == Ctrl_V)
	{
	    // Change line- or characterwise motion into Visual block mode.
	    જો (!VIsual_active)
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
	જો ((redo_yank || oap->op_type != OP_YANK)
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
	    જો (cap->cmdchar == '/' || cap->cmdchar == '?') // was a search
	    {
		// If 'cpoptions' does not contain 'r', insert the search
		// pattern to really repeat the same command.
		જો (vim_strchr(p_cpo, CPO_REDO) == ખાલી)
		    AppendToRedobuffLit(cap->searchbuf, -1);
		AppendToRedobuff(NL_STR);
	    }
	    નહીંતર જો (is_ex_cmdchar(cap))
	    {
		// do_cmdline() has stored the first typed line in
		// "repeat_cmdline".  When several lines are typed repeating
		// won't be possible.
		જો (repeat_cmdline == ખાલી)
		    ResetRedobuff();
		નહીંતર
		{
		    જો (cap->cmdchar == ':')
			AppendToRedobuffLit(repeat_cmdline, -1);
		    નહીંતર
			AppendToRedobuffSpec(repeat_cmdline);
		    AppendToRedobuff(NL_STR);
		    VIM_CLEAR(repeat_cmdline);
		}
	    }
	}

	જો (redo_VIsual_busy)
	{
	    // Redo of an operation on a Visual area. Use the same size from
	    // redo_VIsual.rv_line_count and redo_VIsual.rv_vcol.
	    oap->start = curwin->w_cursor;
	    curwin->w_cursor.lnum += redo_VIsual.rv_લીટી_ગણતરી - 1;
	    જો (curwin->w_cursor.lnum > curbuf->b_ml.ml_લીટી_ગણતરી)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_લીટી_ગણતરી;
	    VIsual_mode = redo_VIsual.rv_mode;
	    જો (redo_VIsual.rv_vcol == MAXCOL || VIsual_mode == 'v')
	    {
		જો (VIsual_mode == 'v')
		{
		    જો (redo_VIsual.rv_લીટી_ગણતરી <= 1)
		    {
			validate_virtcol();
			curwin->w_curswant =
				     curwin->w_virtcol + redo_VIsual.rv_vcol - 1;
		    }
		    નહીંતર
			curwin->w_curswant = redo_VIsual.rv_vcol;
		}
		નહીંતર
		{
		    curwin->w_curswant = MAXCOL;
		}
		coladvance(curwin->w_curswant);
	    }
	    cap->count0 = redo_VIsual.rv_ગણતરી;
	    જો (redo_VIsual.rv_ગણતરી != 0)
		cap->count1 = redo_VIsual.rv_ગણતરી;
	    નહીંતર
		cap->count1 = 1;
	}
	નહીંતર જો (VIsual_active)
	{
	    જો (!gui_yank)
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
	    જો (VIsual_select && VIsual_mode == 'V'
					    && cap->oap->op_type != OP_DELETE)
	    {
		જો (LT_POS(VIsual, curwin->w_cursor))
		{
		    VIsual.col = 0;
		    curwin->w_cursor.col = ml_get_len(curwin->w_cursor.lnum);
		}
		નહીંતર
		{
		    curwin->w_cursor.col = 0;
		    VIsual.col = ml_get_len(VIsual.lnum);
		}
		VIsual_mode = 'v';
	    }
	    // If 'selection' is "exclusive", backup one character for
	    // charwise selections.
	    નહીંતર જો (VIsual_mode == 'v')
		include_લીટી_break = unadjust_for_sel();

	    oap->start = VIsual;
	    જો (VIsual_mode == 'V')
	    {
		oap->start.col = 0;
		oap->start.coladd = 0;
	    }
	}

	// Set oap->start to the first position of the operated text, oap->end
	// to the end of the operated text.  w_cursor is equal to oap->start.
	જો (LT_POS(oap->start, curwin->w_cursor))
	{
#ifdef FEAT_FOLDING
	    // Include folded lines completely.
	    જો (!VIsual_active)
	    {
		જો (hasFolding(oap->start.lnum, &oap->start.lnum, ખાલી))
		    oap->start.col = 0;
		જો ((curwin->w_cursor.col > 0 || oap->inclusive
						  || oap->motion_type == MLINE)
			&& hasFolding(curwin->w_cursor.lnum, ખાલી,
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
	નહીંતર
	{
#ifdef FEAT_FOLDING
	    // Include folded lines completely.
	    જો (!VIsual_active && oap->motion_type == MLINE)
	    {
		જો (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum,
									ખાલી))
		    curwin->w_cursor.col = 0;
		જો (hasFolding(oap->start.lnum, ખાલી, &oap->start.lnum))
		    oap->start.col = ml_get_len(oap->start.lnum);
	    }
#endif
	    oap->end = oap->start;
	    oap->start = curwin->w_cursor;
	}

	// Just in case lines were deleted that make the position invalid.
	check_pos(curwin->w_buffer, &oap->end);
	oap->લીટી_ગણતરી = oap->end.lnum - oap->start.lnum + 1;

	// Set "virtual_op" before resetting VIsual_active.
	virtual_op = virtual_active();

	જો (VIsual_active || redo_VIsual_busy)
	{
	    get_op_vcol(oap, redo_VIsual.rv_vcol, TRUE);

	    જો (!redo_VIsual_busy && !gui_yank)
	    {
		// Prepare to reselect and redo Visual: this is based on the
		// size of the Visual text
		resel_VIsual_mode = VIsual_mode;
		જો (curwin->w_curswant == MAXCOL)
		    resel_VIsual_vcol = MAXCOL;
		નહીંતર
		{
		    જો (VIsual_mode != Ctrl_V)
			getvvcol(curwin, &(oap->end),
						ખાલી, ખાલી, &oap->end_vcol, 0);
		    જો (VIsual_mode == Ctrl_V || oap->લીટી_ગણતરી <= 1)
		    {
			જો (VIsual_mode != Ctrl_V)
			    getvvcol(curwin, &(oap->start),
					      &oap->start_vcol, ખાલી, ખાલી, 0);
			resel_VIsual_vcol = oap->end_vcol - oap->start_vcol + 1;
		    }
		    નહીંતર
			resel_VIsual_vcol = oap->end_vcol;
		}
		resel_VIsual_લીટી_ગણતરી = oap->લીટી_ગણતરી;
	    }

	    // can't redo yank (unless 'y' is in 'cpoptions') and ":"
	    જો ((redo_yank || oap->op_type != OP_YANK)
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
		જો (cap->cmdchar == 'g' && (cap->nchar == 'n'
							|| cap->nchar == 'N'))
		    prep_redo(oap->regname, cap->count0,
			    get_op_char(oap->op_type),
			    get_extra_op_char(oap->op_type),
			    oap->motion_force, cap->cmdchar, cap->nchar);
		નહીંતર જો (!is_ex_cmdchar(cap))
		{
		    પૂર્ણાંક opchar = get_op_char(oap->op_type);
		    પૂર્ણાંક extra_opchar = get_extra_op_char(oap->op_type);
		    પૂર્ણાંક nchar = oap->op_type == OP_REPLACE ? cap->nchar : NUL;

		    // reverse what nv_replace() did
		    જો (nchar == REPLACE_CR_NCHAR)
			nchar = CAR;
		    નહીંતર જો (nchar == REPLACE_NL_NCHAR)
			nchar = NL;

		    જો (opchar == 'g' && extra_opchar == '@')
			// also repeat the count for 'operatorfunc'
			prep_redo_num2(oap->regname, 0L, NUL, 'v',
				     cap->count0, opchar, extra_opchar, nchar);
		    નહીંતર
			prep_redo(oap->regname, 0L, NUL, 'v',
						  opchar, extra_opchar, nchar);
		}
		જો (!redo_VIsual_busy)
		{
		    redo_VIsual.rv_mode = resel_VIsual_mode;
		    redo_VIsual.rv_vcol = resel_VIsual_vcol;
		    redo_VIsual.rv_લીટી_ગણતરી = resel_VIsual_લીટી_ગણતરી;
		    redo_VIsual.rv_ગણતરી = cap->count0;
		    redo_VIsual.rv_arg = cap->arg;
		}
	    }

	    // oap->inclusive defaults to TRUE.
	    // If oap->end is on a NUL (empty line) oap->inclusive becomes
	    // FALSE.  This makes "d}P" and "v}dP" work the same.
	    જો (oap->motion_force == NUL || oap->motion_type == MLINE)
		oap->inclusive = TRUE;
	    જો (VIsual_mode == 'V')
		oap->motion_type = MLINE;
	    નહીંતર
	    {
		oap->motion_type = MCHAR;
		જો (VIsual_mode != Ctrl_V && *ml_get_pos(&(oap->end)) == NUL
			&& (include_લીટી_break || !virtual_op))
		{
		    oap->inclusive = FALSE;
		    // Try to include the newline, unless it's an operator
		    // that works on lines only.
		    જો (*p_sel != 'o'
			    && !op_on_લીટીઓ(oap->op_type)
			    && oap->end.lnum < curbuf->b_ml.ml_લીટી_ગણતરી)
		    {
			++oap->end.lnum;
			oap->end.col = 0;
			oap->end.coladd = 0;
			++oap->લીટી_ગણતરી;
		    }
		}
	    }

	    redo_VIsual_busy = FALSE;

	    // Switch Visual off now, so screen updating does
	    // not show inverted text when the screen is redrawn.
	    // With OP_YANK and sometimes with OP_COLON and OP_FILTER there is
	    // no screen redraw, so it is done here to remove the inverted
	    // part.
	    જો (!gui_yank)
	    {
		VIsual_active = FALSE;
		setmouse();
		mouse_dragging = 0;
		may_clear_cmdline();
		જો ((oap->op_type == OP_YANK
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
	જો (has_mbyte && oap->inclusive)
	{
	    પૂર્ણાંક		l;

	    l = (*mb_ptr2len)(ml_get_pos(&oap->end));
	    જો (l > 1)
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
				&& vim_strchr(p_cpo, CPO_EMPTYREGION) != ખાલી);

	// Force a redraw when operating on an empty Visual region, when
	// 'modifiable is off or creating a fold.
	જો (oap->is_VIsual && (oap->empty || !curbuf->b_p_ma
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
	જો (	   oap->motion_type == MCHAR
		&& oap->inclusive == FALSE
		&& !(cap->retval & CA_NO_ADJ_OP_END)
		&& oap->end.col == 0
		&& (!oap->is_VIsual || *p_sel == 'o')
		&& !oap->block_mode
		&& oap->લીટી_ગણતરી > 1)
	{
	    oap->end_adjusted = TRUE;	    // remember that we did this
	    --oap->લીટી_ગણતરી;
	    --oap->end.lnum;
	    જો (inindent(0))
		oap->motion_type = MLINE;
	    નહીંતર
	    {
		oap->end.col = ml_get_len(oap->end.lnum);
		જો (oap->end.col)
		{
		    --oap->end.col;
		    oap->inclusive = TRUE;
		}
	    }
	}
	નહીંતર
	    oap->end_adjusted = FALSE;

	પસંદગી (oap->op_type)
	{
	સ્થિતિ OP_LSHIFT:
	સ્થિતિ OP_RSHIFT:
	    op_shift(oap, TRUE, oap->is_VIsual ? (પૂર્ણાંક)cap->count1 : 1);
	    auto_format(FALSE, TRUE);
	    તોડો;

	સ્થિતિ OP_JOIN_NS:
	સ્થિતિ OP_JOIN:
	    જો (oap->લીટી_ગણતરી < 2)
		oap->લીટી_ગણતરી = 2;
	    જો (curwin->w_cursor.lnum + oap->લીટી_ગણતરી - 1 >
						   curbuf->b_ml.ml_લીટી_ગણતરી)
		beep_flush();
	    નહીંતર
	    {
		(શૂન્ય)do_join(oap->લીટી_ગણતરી, oap->op_type == OP_JOIN,
							    TRUE, TRUE, TRUE);
		auto_format(FALSE, TRUE);
	    }
	    તોડો;

	સ્થિતિ OP_DELETE:
	    VIsual_reselect = FALSE;	    // don't reselect now
	    જો (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    નહીંતર
	    {
		(શૂન્ય)op_delete(oap);
		// save cursor line for undo if it wasn't saved yet
		જો (oap->motion_type == MLINE && has_format_option(FO_AUTO)
						      && u_save_cursor() == OK)
		    auto_format(FALSE, TRUE);
	    }
	    તોડો;

	સ્થિતિ OP_YANK:
	    જો (empty_region_error)
	    {
		જો (!gui_yank)
		{
		    vim_beep(BO_OPER);
		    CancelRedo();
		}
	    }
	    નહીંતર
	    {
#ifdef FEAT_LINEBREAK
		restore_lbr(lbr_saved);
#endif
		oap->excl_tr_ws = cap->cmdchar == 'z';
		(શૂન્ય)op_yank(oap, FALSE, !gui_yank);
	    }
	    check_cursor_col();
	    તોડો;

	સ્થિતિ OP_CHANGE:
	    VIsual_reselect = FALSE;	    // don't reselect now
	    જો (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    નહીંતર
	    {
		// This is a new edit command, not a restart.  Need to
		// remember it to make 'insertmode' work with mappings for
		// Visual mode.  But do this only once and not when typed and
		// 'insertmode' isn't set.
		જો (p_im || !ચાવી_Typed)
		    restart_edit_save = restart_edit;
		નહીંતર
		    restart_edit_save = 0;
		restart_edit = 0;
#ifdef FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#endif
		// trigger TextChangedI
		curbuf->b_last_changedtick_i = CHANGEDTICK(curbuf);

		જો (op_change(oap))	// will call edit()
		    cap->retval |= CA_COMMAND_BUSY;
		જો (restart_edit == 0)
		    restart_edit = restart_edit_save;
	    }
	    તોડો;

	સ્થિતિ OP_FILTER:
	    જો (vim_strchr(p_cpo, CPO_FILTER) != ખાલી)
		AppendToRedobuff((char_u *)"!\r");  // use any last used !cmd
	    નહીંતર
		bangredo = TRUE;    // do_bang() will put cmd in redo buffer
	    // FALLTHROUGH

	સ્થિતિ OP_INDENT:
	સ્થિતિ OP_COLON:

	    // If 'equalprg' is empty, do the indenting internally.
	    જો (oap->op_type == OP_INDENT && *get_equalprg() == NUL)
	    {
		જો (curbuf->b_p_lisp)
		{
#ifdef FEAT_EVAL
		    જો (use_indentexpr_for_lisp())
			op_reindent(oap, get_પદાવલિ_indent);
		    નહીંતર
#endif
			op_reindent(oap, get_lisp_indent);
		    તોડો;
		}
		op_reindent(oap,
#ifdef FEAT_EVAL
			*curbuf->b_p_inde != NUL ? get_પદાવલિ_indent :
#endif
			    get_c_indent);
		તોડો;
	    }

	    op_colon(oap);
	    તોડો;

	સ્થિતિ OP_TILDE:
	સ્થિતિ OP_UPPER:
	સ્થિતિ OP_LOWER:
	સ્થિતિ OP_ROT13:
	    જો (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    નહીંતર
		op_tilde(oap);
	    check_cursor_col();
	    તોડો;

	સ્થિતિ OP_FORMAT:
#જો defined(FEAT_EVAL)
	    જો (*curbuf->b_p_fex != NUL)
		op_formatexpr(oap);	// use expression
	    નહીંતર
#endif
	    {
		જો (*p_fp != NUL || *curbuf->b_p_fp != NUL)
		    op_colon(oap);		// use external command
		નહીંતર
		    op_format(oap, FALSE);	// use internal function
	    }
	    તોડો;
	સ્થિતિ OP_FORMAT2:
	    op_format(oap, TRUE);	// use internal function
	    તોડો;

	સ્થિતિ OP_FUNCTION:
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
		તોડો;
	    }

	સ્થિતિ OP_ઉમેરો:
	સ્થિતિ OP_APPEND:
	    VIsual_reselect = FALSE;	// don't reselect now
	    જો (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    નહીંતર
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

		op_ઉમેરો(oap, cap->count1);
#ifdef FEAT_LINEBREAK
		// Reset linebreak, so that formatting works correctly.
		(શૂન્ય)reset_lbr();
#endif

		// TODO: when inserting in several lines, should format all
		// the lines.
		auto_format(FALSE, TRUE);

		જો (restart_edit == 0)
		    restart_edit = restart_edit_save;
		નહીંતર
		    cap->retval |= CA_COMMAND_BUSY;
	    }
	    તોડો;

	સ્થિતિ OP_REPLACE:
	    VIsual_reselect = FALSE;	// don't reselect now
	    જો (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    નહીંતર
	    {
#ifdef FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#endif
		op_replace(oap, cap->nchar);
	    }
	    તોડો;

#ifdef FEAT_FOLDING
	સ્થિતિ OP_FOLD:
	    VIsual_reselect = FALSE;	// don't reselect now
	    foldCreate(oap->start.lnum, oap->end.lnum);
	    તોડો;

	સ્થિતિ OP_FOLDOPEN:
	સ્થિતિ OP_FOLDOPENREC:
	સ્થિતિ OP_FOLDCLOSE:
	સ્થિતિ OP_FOLDCLOSEREC:
	    VIsual_reselect = FALSE;	// don't reselect now
	    opFoldRange(oap->start.lnum, oap->end.lnum,
		    oap->op_type == OP_FOLDOPEN
					    || oap->op_type == OP_FOLDOPENREC,
		    oap->op_type == OP_FOLDOPENREC
					  || oap->op_type == OP_FOLDCLOSEREC,
					  oap->is_VIsual);
	    તોડો;

	સ્થિતિ OP_FOLDDEL:
	સ્થિતિ OP_FOLDDELREC:
	    VIsual_reselect = FALSE;	// don't reselect now
	    deleteFold(oap->start.lnum, oap->end.lnum,
			       oap->op_type == OP_FOLDDELREC, oap->is_VIsual);
	    તોડો;
#endif
	સ્થિતિ OP_NR_ADD:
	સ્થિતિ OP_NR_SUB:
	    જો (empty_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    નહીંતર
	    {
		VIsual_active = TRUE;
#ifdef FEAT_LINEBREAK
		restore_lbr(lbr_saved);
#endif
		op_addsub(oap, cap->count1, redo_VIsual.rv_arg);
		VIsual_active = FALSE;
	    }
	    check_cursor_col();
	    તોડો;
	મૂળભૂત:
	    clearopbeep(oap);
	}
	virtual_op = MAYBE;
	જો (!gui_yank)
	{
	    // if 'sol' not set, go back to old column for some commands
	    જો (!p_sol && oap->motion_type == MLINE && !oap->end_adjusted
		    && (oap->op_type == OP_LSHIFT || oap->op_type == OP_RSHIFT
						|| oap->op_type == OP_DELETE))
	    {
#ifdef FEAT_LINEBREAK
		(શૂન્ય)reset_lbr();
#endif
		coladvance(curwin->w_curswant = old_col);
	    }
	}
	નહીંતર
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
    સ્થિર શૂન્ય
pbyte(pos_T lp, પૂર્ણાંક c)
{
    char_u *p = ml_get_buf(curbuf, lp.lnum, TRUE);
    પૂર્ણાંક	len = curbuf->b_ml.ml_લીટી_len;

    // safety check
    જો (lp.col >= len)
	lp.col = (len > 1 ? len - 2 : 0);
    *(p + lp.col) = c;
}
