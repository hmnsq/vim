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

ساكن فراغ ښويدنه_بلاک(oparg_T *oap, صحيح amount);
ساكن فراغ	mb_adjust_opend(oparg_T *oap);
ساكن صحيح	do_addsub(صحيح op_type, pos_T *pos, صحيح اوږدتیا, linenr_T Prenum1);
ساكن فراغ	pbyte(pos_T lp, صحيح c);
#تعریف_کړه PBYTE(lp, c) pbyte(lp, c)


// Flags for third item in "opchars".
#تعریف_کړه OPF_کرښې  1	// operator always works on lines
#تعریف_کړه OPF_CHANGE 2	// operator changes text

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
    {'<', NUL, OPF_کرښې | OPF_CHANGE},	// OP_LSHIFT
    {'>', NUL, OPF_کرښې | OPF_CHANGE},	// OP_RSHIFT
    {'!', NUL, OPF_کرښې | OPF_CHANGE},	// OP_FILTER
    {'g', '~', OPF_CHANGE},		// OP_TILDE
    {'=', NUL, OPF_کرښې | OPF_CHANGE},	// OP_INDENT
    {'g', 'q', OPF_کرښې | OPF_CHANGE},	// OP_FORMAT
    {':', NUL, OPF_کرښې},		// OP_COLON
    {'g', 'U', OPF_CHANGE},		// OP_UPPER
    {'g', 'u', OPF_CHANGE},		// OP_LOWER
    {'J', NUL, OPF_کرښې | OPF_CHANGE},	// DO_JOIN
    {'g', 'J', OPF_کرښې | OPF_CHANGE},	// DO_JOIN_NS
    {'g', '?', OPF_CHANGE},		// OP_ROT13
    {'r', NUL, OPF_CHANGE},		// OP_REPLACE
    {'I', NUL, OPF_CHANGE},		// OP_INSERT
    {'A', NUL, OPF_CHANGE},		// OP_APPEND
    {'z', 'f', OPF_کرښې},		// OP_FOLD
    {'z', 'o', OPF_کرښې},		// OP_FOLDOPEN
    {'z', 'O', OPF_کرښې},		// OP_FOLDOPENREC
    {'z', 'c', OPF_کرښې},		// OP_FOLDCLOSE
    {'z', 'C', OPF_کرښې},		// OP_FOLDCLOSEREC
    {'z', 'd', OPF_کرښې},		// OP_FOLDDEL
    {'z', 'D', OPF_کرښې},		// OP_FOLDDELREC
    {'g', 'w', OPF_کرښې | OPF_CHANGE},	// OP_FORMAT2
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

    إذا (char1 == 'r')		// ignore second character
	أرجع OP_REPLACE;
    إذا (char1 == '~')		// when tilde is an operator
	أرجع OP_TILDE;
    إذا (char1 == 'g' && char2 == Ctrl_A)	//جمع_کړه
	أرجع OP_NR_جمع_کړه;
    إذا (char1 == 'g' && char2 == Ctrl_X)	//کمول
	أرجع OP_NR_SUB;
    إذا (char1 == 'z' && char2 == 'y')	// OP_YANK
	أرجع OP_YANK;
    لكل (i = 0; ; ++i)
    {
	إذا (opchars[i][0] == char1 && opchars[i][1] == char2)
	    اكسر;
	إذا (i == (صحيح)ARRAY_اوږدتیا(opchars) - 1)
	{
	    دننی_error("get_op_type()");
	    اكسر;
	}
    }
    أرجع i;
}

/*
 * Return TRUE if operator "op" always works on whole lines.
 */
    ساكن صحيح
op_on_کرښې(صحيح op)
{
    أرجع opchars[op][2] & OPF_کرښې;
}

#إذا defined(FEAT_دنده_CHANNEL)
/*
 * Return TRUE if operator "op" changes text.
 */
    صحيح
op_is_change(صحيح op)
{
    أرجع opchars[op][2] & OPF_CHANGE;
}
#د_که_پای

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
op_ښويدنه(oparg_T *oap, صحيح curs_سر, صحيح amount)
{
    اوږد	    i;
    صحيح		    لومړی_char;
    صحيح		    بلاک_ستون = 0;

    إذا (u_save((linenr_T)(oap->پیل.lnum - 1),
				       (linenr_T)(oap->پای_ټکی.lnum + 1)) == FAIL)
	أرجع;

    إذا (oap->بلاک_ډېرپېښ)
	بلاک_ستون = curwin->w_cursor.ستون;

    لكل (i = oap->کرښه_شمېر; --i >= 0; )
    {
	لومړی_char = *ml_get_curline();
	إذا (لومړی_char == NUL)				// empty line
	    curwin->w_cursor.ستون = 0;
	وإلا إذا (oap->بلاک_ډېرپېښ)
	    ښويدنه_بلاک(oap, amount);
	وإلا
	    // Move the line right if it doesn't start with '#', 'smartindent'
	    // isn't set or 'cindent' isn't set or '#' isn't in 'cino'.
	    إذا (لومړی_char != '#' || !preprocs_کیڼ())
		ښويدنه_کرښه(oap->op_type == OP_LSHIFT, p_sr, amount, FALSE);
	++curwin->w_cursor.lnum;
    }

    بدل_شوی_کرښې(oap->پیل.lnum, 0, oap->پای_ټکی.lnum + 1, 0L);
    إذا (oap->بلاک_ډېرپېښ)
    {
	curwin->w_cursor.lnum = oap->پیل.lnum;
	curwin->w_cursor.ستون = بلاک_ستون;
    }
    وإلا إذا (curs_سر)	    // put cursor on first line, for ">>"
    {
	curwin->w_cursor.lnum = oap->پیل.lnum;
	beginline(BL_SOL | BL_FIX);   // shift_line() may have set cursor.col
    }
    وإلا
	--curwin->w_cursor.lnum;	// put cursor on last line, for ":>"

#که_تعریف_شوی FEAT_FOLDING
    // The cursor line is not in a closed fold
    foldOpenCursor();
#د_که_پای


    إذا (oap->کرښه_شمېر > p_report)
    {
	حرف	    *op;
	حرف	    *پیغام_کرښه_single;
	حرف	    *پیغام_کرښه_plural;

	إذا (oap->op_type == OP_RSHIFT)
	    op = ">";
	وإلا
	    op = "<";
	پیغام_کرښه_single = NGETTEXT("%ld line %sed %d time",
					     "%ld line %sed %d times", amount);
	پیغام_کرښه_plural = NGETTEXT("%ld lines %sed %d time",
					    "%ld lines %sed %d times", amount);
	vim_snprintf((حرف *)IObuff, IOSIZE,
		NGETTEXT(پیغام_کرښه_single, پیغام_کرښه_plural, oap->کرښه_شمېر),
		oap->کرښه_شمېر, op, amount);
	پیغام_attr_keep((حرف *)IObuff, 0, TRUE);
    }

    إذا ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_پیل = oap->پیل;
	curbuf->b_op_پای_ټکی.lnum = oap->پای_ټکی.lnum;
	curbuf->b_op_پای_ټکی.ستون = ml_get_len(oap->پای_ټکی.lnum);
	إذا (curbuf->b_op_پای_ټکی.ستون > 0)
	    --curbuf->b_op_پای_ټکی.ستون;
    }
}

#که_تعریف_شوی FEAT_VARTABS
/*
 * Return the tabstop width at the index of the variable tabstop array.  If an
 * index greater than the length of the array is given, the last tabstop width
 * in the array is returned.
 */
    ساكن صحيح
get_vts(صحيح *vts_array, صحيح شاخص)
{
    صحيح	ts;

    إذا (شاخص < 1)
	ts = 0;
    وإلا إذا (شاخص <= vts_array[0])
	ts = vts_array[شاخص];
    وإلا
	ts = vts_array[vts_array[0]];

    أرجع ts;
}

/*
 * Return the sum of all the tabstops through the index-th.
 */
    ساكن صحيح
get_vts_مجموعه(صحيح *vts_array, صحيح شاخص)
{
    صحيح	مجموعه = 0;
    صحيح	i;

    // Perform the summation for indices within the actual array.
    لكل (i = 1; i <= شاخص && i <= vts_array[0]; i++)
	مجموعه += vts_array[i];

    // Add tabstops whose indices exceed the actual array.
    إذا (i <= شاخص)
	مجموعه += vts_array[vts_array[0]] * (شاخص - vts_array[0]);

    أرجع مجموعه;
}
#د_که_پای

    ساكن vimlong_T
get_new_sw_indent(
    صحيح		کیڼ_2,		// TRUE if shift is to the left
    صحيح		پړاو,		// TRUE if new indent is to be to a tabstop
    vimlong_T	amount,		// Number of shifts
    vimlong_T	sw_val)
{
    vimlong_T	شمېر = get_indent();
    vimlong_T	i, j;

    إذا (پړاو)			// round off indent
    {
	i = trim_منزل_int(شمېر) / sw_val;	// number of 'shiftwidth' rounded down
	j = trim_منزل_int(شمېر) % sw_val;	// extra spaces
	إذا (j && کیڼ_2)		// first remove extra spaces
	    --amount;
	إذا (کیڼ_2)
	{
	    i -= amount;
	    إذا (i < 0)
		i = 0;
	}
	وإلا
	    i += amount;
	شمېر = i * sw_val;
    }
    وإلا			// original vi indent
    {
	إذا (کیڼ_2)
	{
	    شمېر -= sw_val * amount;
	    إذا (شمېر < 0)
		شمېر = 0;
	}
	وإلا
	    شمېر += sw_val * amount;
    }

    أرجع شمېر;
}

#که_تعریف_شوی FEAT_VARTABS
    ساكن vimlong_T
get_new_vts_indent(
    صحيح	کیڼ_2,			// TRUE if shift is to the left
    صحيح	پړاو,			// TRUE if new indent is to be to a tabstop
    صحيح	amount,			// Number of shifts
    صحيح	*vts_array)
{
    vimlong_T	indent = get_indent();
    صحيح		vtsi = 0;
    صحيح		vts_indent = 0;
    صحيح		ts = 0;		// Silence uninitialized variable warning.
    صحيح		دننه_واټن;		// Extra indent spaces to the right of the
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

    دننه_واټن = indent - vts_indent;

    إذا (پړاو)
    {
	إذا (کیڼ_2)
	{
	    إذا (دننه_واټن == 0)
		indent = get_vts_مجموعه(vts_array, vtsi - amount);
	    وإلا
		indent = get_vts_مجموعه(vts_array, vtsi - (amount - 1));
	}
	وإلا
	    indent = get_vts_مجموعه(vts_array, vtsi + amount);
    }
    وإلا
    {
	إذا (کیڼ_2)
	{
	    إذا (amount > vtsi)
		indent = 0;
	    وإلا
		indent = get_vts_مجموعه(vts_array, vtsi - amount) + دننه_واټن;
	}
	وإلا
	    indent = get_vts_مجموعه(vts_array, vtsi + amount) + دننه_واټن;
    }

    أرجع indent;
}
#د_که_پای

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
ښويدنه_کرښه(
    صحيح	کیڼ_2,			// TRUE if shift is to the left
    صحيح	پړاو,			// TRUE if new indent is to be to a tabstop
    صحيح	amount,			// Number of shifts
    صحيح	call_بدل_شوی_بایټونه)	// call changed_bytes()
{
    vimlong_T	شمېر;
    اوږد	sw_val = curbuf->b_p_sw;
    اوږد	ts_val = curbuf->b_p_ts;
#که_تعریف_شوی FEAT_VARTABS
    صحيح		*vts_array = curbuf->b_p_vts_array;
#د_که_پای

    إذا (sw_val != 0)
	// 'shiftwidth' is not zero; use it as the shift size.
	شمېر = get_new_sw_indent(کیڼ_2, پړاو, amount, sw_val);
    وإلا
#که_تعریف_شوی FEAT_VARTABS
	إذا ((vts_array == عدم) || (vts_array[0] == 0))
#د_که_پای
    {
	// 'shiftwidth is zero and 'vartabstop' is empty; use 'tabstop' as the
	// shift size.
	شمېر = get_new_sw_indent(کیڼ_2, پړاو, amount, ts_val);
    }
#که_تعریف_شوی FEAT_VARTABS
    وإلا
    {
	// 'shiftwidth is zero and 'vartabstop' is defined; use 'vartabstop'
	// to determine the new indent.
	شمېر = get_new_vts_indent(کیڼ_2, پړاو, amount, vts_array);
    }
#د_که_پای

    // Set new indent
    إذا (State & VREPLACE_FLAG)
	change_indent(INDENT_سېټ, trim_منزل_int(شمېر), FALSE, NUL, call_بدل_شوی_بایټونه);
    وإلا
	(فراغ)سېټ_indent(trim_منزل_int(شمېر), call_بدل_شوی_بایټونه ? SIN_بدل_شوی : 0);
}

/*
 * Shift one line of the current block one shiftwidth right or left.
 * Leaves cursor on first character in block.
 */
    ساكن فراغ
ښويدنه_بلاک(oparg_T *oap, صحيح amount)
{
    صحيح			کیڼ_2 = (oap->op_type == OP_LSHIFT);
    صحيح			oldstate = State;
    صحيح			ټول;
    char_u		*newp, *oldp;
    size_t		newlen, oldlen;
    صحيح			oldcol = curwin->w_cursor.ستون;
    صحيح			sw_val = (صحيح)get_sw_ارزښت_indent(curbuf, کیڼ_2);
    صحيح			ts_val = (صحيح)curbuf->b_p_ts;
    بنية بلاک_def	bd;
    صحيح			incr;
    colnr_T		ws_vcol;
    صحيح			added;
    size_t		new_کرښه_len;	// the length of the line after the
					// block shift
#که_تعریف_شوی FEAT_RIGHTLEFT
    صحيح			old_p_ri = p_ri;

    p_ri = 0;			// don't want revins in indent
#د_که_پای

    State = ډېرپېښ_ورننباسه;	// don't want MODE_REPLACE for State
    بلاک_prep(oap, &bd, curwin->w_cursor.lnum, TRUE);
    إذا (bd.is_short)
	أرجع;

    // total is number of screen columns to be inserted/removed
    ټول = (صحيح)((بینښه)amount * (بینښه)sw_val);
    إذا ((ټول / sw_val) != amount)
	أرجع; // multiplication overflow

    oldp = ml_get_curline();
    oldlen = ml_get_curline_len();

    إذا (!کیڼ_2)
    {
	صحيح		tabs = 0, spaces = 0;
	chartabsize_T	cts;

	/*
	 *  1. Get start vcol
	 *  2. Total ws vcols
	 *  3. Divvy into TABs & spp
	 *  4. Construct new string
	 */
	ټول += bd.pre_whitesp; // all virtual WS up to & incl a split TAB
	ws_vcol = bd.پیل_vcol - bd.pre_whitesp;
	إذا (bd.startspaces)
	{
	    إذا (has_mbyte)
	    {
		إذا ((*mb_ptr2len)(bd.textstart) == 1)
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
				   bd.پیل_vcol, bd.textstart, bd.textstart);
	لكل ( ; VIM_ISWHITE(*cts.cts_ptr); )
	{
	    incr = lbr_chartabsize_adv(&cts);
	    ټول += incr;
	    cts.cts_vcol += incr;
	}
	bd.textstart = cts.cts_ptr;
	bd.پیل_vcol = cts.cts_vcol;
	clear_chartabsize_arg(&cts);

	// OK, now total=all the VWS reqd, and textstart points at the 1st
	// non-ws char in the block.
#که_تعریف_شوی FEAT_VARTABS
	إذا (!curbuf->b_p_et)
	    tabstop_fromto(ws_vcol, ws_vcol + ټول,
				ts_val, curbuf->b_p_vts_array, &tabs, &spaces);
	وإلا
	    spaces = ټول;
#وإلا
	إذا (!curbuf->b_p_et)
	    tabs = ((ws_vcol % ts_val) + ټول) / ts_val; // number of tabs
	إذا (tabs > 0)
	    spaces = ((ws_vcol % ts_val) + ټول) % ts_val; // number of spp
	وإلا
	    spaces = ټول;
#د_که_پای
	// if we're splitting a TAB, allow for it
	bd.textcol -= bd.pre_whitesp_c - (bd.startspaces != 0);

	new_کرښه_len = bd.textcol + tabs + spaces + (oldlen - (bd.textstart - oldp));
	newp = alloc(new_کرښه_len + 1);
	إذا (newp == عدم)
	    أرجع;
	mch_memmove(newp, oldp, (size_t)bd.textcol);
	newlen = bd.textcol;
	vim_memset(newp + newlen, TAB, (size_t)tabs);
	newlen += tabs;
	vim_memset(newp + newlen, ' ', (size_t)spaces);
	STRCPY(newp + newlen + spaces, bd.textstart);
    }
    وإلا //کیڼ
    {
	colnr_T	    destination_ستون;	// column to which text in block will
					// be shifted
	char_u	    *verbatim_copy_پای_ټکی;	// end of the part of the line which is
					// copied verbatim
	colnr_T	    verbatim_copy_پلنوالی;// the (displayed) width of this part
					// of line
	size_t	    fill;		// nr of spaces that replace a TAB
	size_t	    بلاک_space_پلنوالی;
	size_t	    ښويدنه_amount;
	char_u	    *non_white = bd.textstart;
	colnr_T	    non_white_ستون;
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
	إذا (bd.startspaces)
	    MB_PTR_ADV(non_white);

	// The character's column is in "bd.start_vcol".
	non_white_ستون = bd.پیل_vcol;

	init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum,
				   non_white_ستون, bd.textstart, non_white);
	طالما (VIM_ISWHITE(*cts.cts_ptr))
	{
	    incr = lbr_chartabsize_adv(&cts);
	    cts.cts_vcol += incr;
	}
	non_white_ستون = cts.cts_vcol;
	non_white = cts.cts_ptr;
	clear_chartabsize_arg(&cts);

	بلاک_space_پلنوالی = non_white_ستون - oap->پیل_vcol;
	// We will shift by "total" or "block_space_width", whichever is less.
	ښويدنه_amount = (بلاک_space_پلنوالی < (size_t)ټول
					 ? بلاک_space_پلنوالی : (size_t)ټول);

	// The column to which we will shift the text.
	destination_ستون = (colnr_T)(non_white_ستون - ښويدنه_amount);

	// Now let's find out how much of the beginning of the line we can
	// reuse without modification.
	verbatim_copy_پای_ټکی = bd.textstart;
	verbatim_copy_پلنوالی = bd.پیل_vcol;

	// If "bd.startspaces" is set, "bd.textstart" points to the character
	// preceding the block. We have to subtract its width to obtain its
	// column number.
	إذا (bd.startspaces)
	    verbatim_copy_پلنوالی -= bd.پیل_char_vcols;
	init_chartabsize_arg(&cts, curwin, 0, verbatim_copy_پلنوالی,
					     bd.textstart, verbatim_copy_پای_ټکی);
	طالما (cts.cts_vcol < destination_ستون)
	{
	    incr = lbr_chartabsize(&cts);
	    إذا (cts.cts_vcol + incr > destination_ستون)
		اكسر;
	    cts.cts_vcol += incr;
	    MB_PTR_ADV(cts.cts_ptr);
	}
	verbatim_copy_پلنوالی = cts.cts_vcol;
	verbatim_copy_پای_ټکی = cts.cts_ptr;
	clear_chartabsize_arg(&cts);

	// If "destination_col" is different from the width of the initial
	// part of the line that will be copied, it means we encountered a tab
	// character, which we will have to partly replace with spaces.
	fill = destination_ستون - verbatim_copy_پلنوالی;

	// The replacement line will consist of:
	// - the beginning of the original line up to "verbatim_copy_end",
	// - "fill" number of spaces,
	// - the rest of the line, pointed to by non_white.
	fixedlen = verbatim_copy_پای_ټکی - oldp;
	new_کرښه_len = fixedlen + fill + (oldlen - (non_white - oldp));

	newp = alloc(new_کرښه_len + 1);
	إذا (newp == عدم)
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
    إذا (new_کرښه_len >= oldlen)
	added = (صحيح)(new_کرښه_len - oldlen);
    وإلا
	added = 0 - (صحيح)(oldlen - new_کرښه_len);
    inserted_بایټونه(curwin->w_cursor.lnum, bd.textcol, added);
    State = oldstate;
    curwin->w_cursor.ستون = oldcol;
#که_تعریف_شوی FEAT_RIGHTLEFT
    p_ri = old_p_ri;
#د_که_پای
}

/*
 * Insert string "s" (b_insert ? before : after) block :AKelly
 * Caller must prepare for undo.
 */
    ساكن فراغ
بلاک_ورننباسه(
    oparg_T		*oap,
    char_u		*s,
    size_t		slen,
    صحيح			b_ورننباسه,
    بنية بلاک_def	*bdp)
{
    صحيح		ts_val;
    صحيح		شمېر = 0;	// extra spaces to replace a cut TAB
    صحيح		spaces = 0;	// non-zero if cutting a TAB
    colnr_T	دننه_واټن;		// pointer along new line
    colnr_T	startcol;	// column where insert starts
    char_u	*newp, *oldp;	// new, old lines
    linenr_T	lnum;		// loop var
    صحيح		oldstate = State;

    State = ډېرپېښ_ورننباسه;	// don't want MODE_REPLACE for State

    لكل (lnum = oap->پیل.lnum + 1; lnum <= oap->پای_ټکی.lnum; lnum++)
    {
	بلاک_prep(oap, bdp, lnum, TRUE);
	إذا (bdp->is_short && b_ورننباسه)
	    استمر;	// OP_INSERT, line ends before block start

	oldp = ml_get(lnum);

	إذا (b_ورننباسه)
	{
	    ts_val = bdp->پیل_char_vcols;
	    spaces = bdp->startspaces;
	    إذا (spaces != 0)
		شمېر = ts_val - 1; // we're cutting a TAB
	    دننه_واټن = bdp->textcol;
	}
	وإلا // append
	{
	    ts_val = bdp->پای_ټکی_char_vcols;
	    إذا (!bdp->is_short) // spaces = padding after block
	    {
		spaces = (bdp->endspaces ? ts_val - bdp->endspaces : 0);
		إذا (spaces != 0)
		    شمېر = ts_val - 1; // we're cutting a TAB
		دننه_واټن = bdp->textcol + bdp->textlen - (spaces != 0);
	    }
	    وإلا // spaces = padding to block edge
	    {
		// if $ used, just append to EOL (ie spaces==0)
		إذا (!bdp->is_MAX)
		    spaces = (oap->پای_ټکی_vcol - bdp->پای_ټکی_vcol) + 1;
		شمېر = spaces;
		دننه_واټن = bdp->textcol + bdp->textlen;
	    }
	}

	إذا (has_mbyte && spaces > 0)
	    // avoid copying part of a multi-byte character
	    دننه_واټن -= (*mb_سر_off)(oldp, oldp + دننه_واټن);

	إذا (spaces < 0)  // can happen when the cursor was moved
	    spaces = 0;

	// Make sure the allocated size matches what is actually copied below.
	newp = alloc(ml_get_len(lnum) + spaces + slen
		    + (spaces > 0 && !bdp->is_short ? ts_val - spaces : 0)
								  + شمېر + 1);
	إذا (newp == عدم)
	    استمر;

	// copy up to shifted part
	mch_memmove(newp, oldp, (size_t)دننه_واټن);
	oldp += دننه_واټن;

	// insert pre-padding
	vim_memset(newp + دننه_واټن, ' ', (size_t)spaces);
	startcol = دننه_واټن + spaces;

	// copy the new text
	mch_memmove(newp + startcol, s, slen);
	دننه_واټن += (صحيح)slen;

	إذا (spaces > 0 && !bdp->is_short)
	{
	    إذا (*oldp == TAB)
	    {
		// insert post-padding
		vim_memset(newp + دننه_واټن + spaces, ' ',
						    (size_t)(ts_val - spaces));
		// we're splitting a TAB, don't copy it
		oldp++;
		// We allowed for that TAB, remember this now
		شمېر++;
	    }
	    وإلا
		// Not a TAB, no extra spaces
		شمېر = spaces;
	}

	إذا (spaces > 0)
	    دننه_واټن += شمېر;
	STRCPY(newp + دننه_واټن, oldp);

	ml_replace(lnum, newp, FALSE);

	إذا (b_ورننباسه)
	    // correct any text properties
	    inserted_بایټونه(lnum, startcol, (صحيح)slen);

	إذا (lnum == oap->پای_ټکی.lnum)
	{
	    // Set "']" mark to the end of the block instead of the end of
	    // the insert in the first line.
	    curbuf->b_op_پای_ټکی.lnum = oap->پای_ټکی.lnum;
	    curbuf->b_op_پای_ټکی.ستون = دننه_واټن;
	    إذا (curbuf->b_visual.vi_پای_ټکی.coladd)
	    {
		curbuf->b_visual.vi_پای_ټکی.ستون += curbuf->b_visual.vi_پای_ټکی.coladd;
		curbuf->b_visual.vi_پای_ټکی.coladd = 0;
	    }
	}
    } // for all lnum

    بدل_شوی_کرښې(oap->پیل.lnum + 1, 0, oap->پای_ټکی.lnum + 1, 0L);

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
    بنية بلاک_def	bd;
    linenr_T		old_lcount = curbuf->b_ml.ml_کرښه_شمېر;
    صحيح			did_yank = FALSE;

    إذا (curbuf->b_ml.ml_flags & ML_تشه)	    // nothing to do
	أرجع OK;

    // Nothing to delete, return here.	Do prepare undo, for op_change().
    إذا (oap->تشه_2)
	أرجع u_save_cursor();

    إذا (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	أرجع FAIL;
    }

    إذا (VIsual_select && oap->is_VIsual)
	// use register given with CTRL_R, defaults to zero
	oap->regname = VIsual_select_reg;

#که_تعریف_شوی HAVE_CLIPMETHOD
    adjust_clip_reg(&oap->regname);
#د_که_پای

    إذا (has_mbyte)
	mb_adjust_opend(oap);

    /*
     * Imitate the strange Vi behaviour: If the delete spans more than one
     * line and motion_type == MCHAR and the result is a blank line, make the
     * delete linewise.  Don't do this for the change command or Visual mode.
     */
    إذا (       oap->motion_type == MCHAR
	    && !oap->is_VIsual
	    && !oap->بلاک_ډېرپېښ
	    && oap->کرښه_شمېر > 1
	    && oap->motion_force == NUL
	    && (vim_strchr(p_cpo, CPO_کلمه) != عدم)
	    && oap->op_type == OP_DELETE)
    {
	ptr = ml_get(oap->پای_ټکی.lnum) + oap->پای_ټکی.ستون;
	إذا (*ptr != NUL)
	    ptr += oap->inclusive;
	ptr = skipwhite(ptr);
	إذا (*ptr == NUL && inindent(0))
	    oap->motion_type = MLINE;
    }

    /*
     * Check for trying to delete (e.g. "D") in an empty line.
     * Note: For the change operator it is ok.
     */
    إذا (       oap->motion_type == MCHAR
	    && oap->کرښه_شمېر == 1
	    && oap->op_type == OP_DELETE
	    && *ml_get(oap->پیل.lnum) == NUL)
    {
	/*
	 * It's an error to operate on an empty region, when 'E' included in
	 * 'cpoptions' (Vi compatible).
	 */
	إذا (virtual_op)
	    // Virtual editing: Nothing gets deleted, but we set the '[ and ']
	    // marks as if it happened.
	    اذهب setmarks;
	إذا (vim_strchr(p_cpo, CPO_EMPTYREGION) != عدم)
	    beep_flush();
	أرجع OK;
    }

    /*
     * Do a yank of whatever we're about to delete.
     * If a yank register was specified, put the deleted text into that
     * register.  For the black hole register '_' don't yank anything.
     */
    إذا (oap->regname != '_')
    {
#که_تعریف_شوی FEAT_CLIPBOARD_PROVIDER
	inc_clip_provider();
#د_که_پای
	إذا (oap->regname != 0)
	{
	    // check for read-only register
	    إذا (!valid_yank_reg(oap->regname, TRUE))
	    {
		beep_flush();
#که_تعریف_شوی FEAT_CLIPBOARD_PROVIDER
		dec_clip_provider();
#د_که_پای
		أرجع OK;
	    }
	    get_yank_ثبتوونکی(oap->regname, TRUE); // yank into specif'd reg.
	    إذا (op_yank(oap, TRUE, FALSE) == OK)   // yank without message
		did_yank = TRUE;
	}
	وإلا
	    reset_y_append(); // not appending to unnamed register

	/*
	 * Put deleted text into register 1 and shift number registers if the
	 * delete contains a line break, or when using a specific operator (Vi
	 * compatible)
	 */
	إذا (oap->motion_type == MLINE || oap->کرښه_شمېر > 1
							   || oap->use_reg_one)
	{
	    ښويدنه_delete_registers();
	    إذا (op_yank(oap, TRUE, FALSE) == OK)
		did_yank = TRUE;
	}

	// Yank into small delete register when no named register specified
	// and the delete is within one line.
	إذا ((
#که_تعریف_شوی HAVE_CLIPMETHOD
	    ((clip_unnamed & CLIP_UNNAMED) && oap->regname == '*') ||
	    ((clip_unnamed & CLIP_UNNAMED_PLUS) && oap->regname == '+') ||
#د_که_پای
	    oap->regname == 0) && oap->motion_type != MLINE
						      && oap->کرښه_شمېر == 1)
	{
	    oap->regname = '-';
	    get_yank_ثبتوونکی(oap->regname, TRUE);
	    إذا (op_yank(oap, TRUE, FALSE) == OK)
		did_yank = TRUE;
	    oap->regname = 0;
	}

	/*
	 * If there's too much stuff to fit in the yank register, then get a
	 * confirmation before doing the delete. This is crude, but simple.
	 * And it avoids doing a delete of something we can't put back if we
	 * want.
	 */
	إذا (!did_yank)
	{
	    صحيح پیغام_silent_save = پیغام_silent;

	    پیغام_silent = 0;	// must display the prompt
	    n = ask_yesno((char_u *)_("cannot yank; delete anyway"), TRUE);
	    پیغام_silent = پیغام_silent_save;
	    إذا (n != 'y')
	    {
		emsg(_(e_command_aborted));
#که_تعریف_شوی FEAT_CLIPBOARD_PROVIDER
		dec_clip_provider();
#د_که_پای
		أرجع FAIL;
	    }
	}

#إذا defined(FEAT_EVAL)
	إذا (did_yank && has_textyankpost())
	    yank_do_autocmd(oap, get_y_current());
#د_که_پای
#که_تعریف_شوی FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#د_که_پای
    }

    /*
     * block mode delete
     */
    إذا (oap->بلاک_ډېرپېښ)
    {
	إذا (u_save((linenr_T)(oap->پیل.lnum - 1),
			       (linenr_T)(oap->پای_ټکی.lnum + 1)) == FAIL)
	    أرجع FAIL;

	لكل (lnum = curwin->w_cursor.lnum; lnum <= oap->پای_ټکی.lnum; ++lnum)
	{
	    بلاک_prep(oap, &bd, lnum, TRUE);
	    إذا (bd.textlen == 0)	// nothing to delete
		استمر;

	    // Adjust cursor position for tab replaced by spaces and 'lbr'.
	    إذا (lnum == curwin->w_cursor.lnum)
	    {
		curwin->w_cursor.ستون = bd.textcol + bd.startspaces;
		curwin->w_cursor.coladd = 0;
	    }

	    // "n" == number of chars deleted
	    // If we delete a TAB, it may be replaced by several characters.
	    // Thus the number of characters may increase!
	    n = bd.textlen - bd.startspaces - bd.endspaces;
	    oldp = ml_get(lnum);
	    newp = alloc(ml_get_len(lnum) + 1 - n);
	    إذا (newp == عدم)
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

#که_تعریف_شوی FEAT_PROP_POPUP
	    إذا (curbuf->b_has_textprop && n != 0)
		adjust_prop_columns(lnum, bd.textcol, -n, 0);
#د_که_پای
	}

	check_cursor_ستون();
	بدل_شوی_کرښې(curwin->w_cursor.lnum, curwin->w_cursor.ستون,
						       oap->پای_ټکی.lnum + 1, 0L);
	oap->کرښه_شمېر = 0;	    // no lines deleted
    }
    وإلا إذا (oap->motion_type == MLINE)
    {
	إذا (oap->op_type == OP_CHANGE)
	{
	    // Delete the lines except the first one.  Temporarily move the
	    // cursor to the next line.  Save the current line number, if the
	    // last line is deleted it may be changed.
	    إذا (oap->کرښه_شمېر > 1)
	    {
		lnum = curwin->w_cursor.lnum;
		++curwin->w_cursor.lnum;
		del_کرښې((اوږد)(oap->کرښه_شمېر - 1), TRUE);
		curwin->w_cursor.lnum = lnum;
	    }
	    إذا (u_save_cursor() == FAIL)
		أرجع FAIL;
	    إذا (curbuf->b_p_ai)		    // don't delete indent
	    {
		beginline(BL_WHITE);	    // cursor on first non-white
		did_ai = TRUE;		    // delete the indent when ESC hit
		ai_ستون = curwin->w_cursor.ستون;
	    }
	    وإلا
		beginline(0);		    // cursor in column 0
	    truncate_کرښه(FALSE);   // delete the rest of the line,
				    // leaving cursor past last char in line
	    إذا (oap->کرښه_شمېر > 1)
		u_clearline();	    // "U" command not possible after "2cc"
	}
	وإلا
	{
	    del_کرښې(oap->کرښه_شمېر, TRUE);
	    beginline(BL_WHITE | BL_FIX);
	    u_clearline();	// "U" command not possible after "dd"
	}
    }
    وإلا
    {
	إذا (virtual_op)
	{
	    صحيح		endcol = 0;

	    // For virtualedit: break the tabs that are partly included.
	    إذا (gchar_pos(&oap->پیل) == '\t')
	    {
		إذا (u_save_cursor() == FAIL)	// save first line for undo
		    أرجع FAIL;
		إذا (oap->کرښه_شمېر == 1)
		    endcol = getviscol2(oap->پای_ټکی.ستون, oap->پای_ټکی.coladd);
		coladvance_force(getviscol2(oap->پیل.ستون, oap->پیل.coladd));
		oap->پیل = curwin->w_cursor;
		إذا (oap->کرښه_شمېر == 1)
		{
		    coladvance(endcol);
		    oap->پای_ټکی.ستون = curwin->w_cursor.ستون;
		    oap->پای_ټکی.coladd = curwin->w_cursor.coladd;
		    curwin->w_cursor = oap->پیل;
		}
	    }

	    // Break a tab only when it's included in the area.
	    إذا (gchar_pos(&oap->پای_ټکی) == '\t'
				     && (صحيح)oap->پای_ټکی.coladd < oap->inclusive)
	    {
		// save last line for undo
		إذا (u_save((linenr_T)(oap->پای_ټکی.lnum - 1),
				       (linenr_T)(oap->پای_ټکی.lnum + 1)) == FAIL)
		    أرجع FAIL;
		curwin->w_cursor = oap->پای_ټکی;
		coladvance_force(getviscol2(oap->پای_ټکی.ستون, oap->پای_ټکی.coladd));
		oap->پای_ټکی = curwin->w_cursor;
		curwin->w_cursor = oap->پیل;
	    }
	    إذا (has_mbyte)
		mb_adjust_opend(oap);
	}

	إذا (oap->کرښه_شمېر == 1)	// delete characters within one line
	{
	    إذا (u_save_cursor() == FAIL)	// save line for undo
		أرجع FAIL;

	    // if 'cpoptions' contains '$', display '$' at end of change
	    إذا (       vim_strchr(p_cpo, CPO_DOLLAR) != عدم
		    && oap->op_type == OP_CHANGE
		    && oap->پای_ټکی.lnum == curwin->w_cursor.lnum
		    && !oap->is_VIsual)
		display_dollar(oap->پای_ټکی.ستون - !oap->inclusive);

	    n = oap->پای_ټکی.ستون - oap->پیل.ستون + 1 - !oap->inclusive;

	    إذا (virtual_op)
	    {
		// fix up things for virtualedit-delete:
		// break the tabs which are going to get in our way
		صحيح		len = ml_get_curline_len();

		إذا (oap->پای_ټکی.coladd != 0
			&& (صحيح)oap->پای_ټکی.ستون >= len - 1
			&& !(oap->پیل.coladd && (صحيح)oap->پای_ټکی.ستون >= len - 1))
		    n++;
		// Delete at least one char (e.g, when on a control char).
		إذا (n == 0 && oap->پیل.coladd != oap->پای_ټکی.coladd)
		    n = 1;

		// When deleted a char in the line, reset coladd.
		إذا (gchar_cursor() != NUL)
		    curwin->w_cursor.coladd = 0;
	    }
	    (فراغ)del_بایټونه((اوږد)n, !virtual_op,
			    oap->op_type == OP_DELETE && !oap->is_VIsual);
	}
	وإلا				// delete characters between lines
	{
	    pos_T   curpos;

	    // save deleted and changed lines for undo
	    إذا (u_save((linenr_T)(curwin->w_cursor.lnum - 1),
		 (linenr_T)(curwin->w_cursor.lnum + oap->کرښه_شمېر)) == FAIL)
		أرجع FAIL;

	    truncate_کرښه(TRUE);	// delete from cursor to end of line

	    curpos = curwin->w_cursor;	// remember curwin->w_cursor
	    ++curwin->w_cursor.lnum;
	    del_کرښې((اوږد)(oap->کرښه_شمېر - 2), FALSE);

	    // delete from start of line until op_end
	    n = (oap->پای_ټکی.ستون + 1 - !oap->inclusive);
	    curwin->w_cursor.ستون = 0;
	    (فراغ)del_بایټونه((اوږد)n, !virtual_op,
			    oap->op_type == OP_DELETE && !oap->is_VIsual);
	    curwin->w_cursor = curpos;	// restore curwin->w_cursor
	    (فراغ)do_join(2, FALSE, FALSE, FALSE, FALSE);
	}
	إذا (oap->op_type == OP_DELETE)
	    auto_format(FALSE, TRUE);
    }

    msgmore(curbuf->b_ml.ml_کرښه_شمېر - old_lcount);

setmarks:
    إذا ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	إذا (oap->بلاک_ډېرپېښ)
	{
	    curbuf->b_op_پای_ټکی.lnum = oap->پای_ټکی.lnum;
	    curbuf->b_op_پای_ټکی.ستون = oap->پیل.ستون;
	}
	وإلا
	    curbuf->b_op_پای_ټکی = oap->پیل;
	curbuf->b_op_پیل = oap->پیل;
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
    char_u	*کرښه;
    char_u	*ptr;

    إذا (!oap->inclusive)
	أرجع;

    کرښه = ml_get(oap->پای_ټکی.lnum);
    ptr = کرښه + oap->پای_ټکی.ستون;
    إذا (*ptr != NUL)
    {
	ptr -= (*mb_سر_off)(کرښه, ptr);
	ptr += (*mb_ptr2len)(ptr) - 1;
	oap->پای_ټکی.ستون = ptr - کرښه;
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

    State = ډېرپېښ_REPLACE;
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
    صحيح			عدد_chars;
    char_u		*newp, *oldp;
    size_t		newlen, oldlen;
    بنية بلاک_def	bd;
    char_u		*after_p = عدم;
    صحيح			had_ctrl_v_cr = FALSE;

    إذا ((curbuf->b_ml.ml_flags & ML_تشه ) || oap->تشه_2)
	أرجع OK;	    // nothing to do

    إذا (c == REPLACE_CR_NCHAR)
    {
	had_ctrl_v_cr = TRUE;
	c = CAR;
    }
    وإلا إذا (c == REPLACE_NL_NCHAR)
    {
	had_ctrl_v_cr = TRUE;
	c = NL;
    }

    إذا (has_mbyte)
	mb_adjust_opend(oap);

    إذا (u_save((linenr_T)(oap->پیل.lnum - 1),
				       (linenr_T)(oap->پای_ټکی.lnum + 1)) == FAIL)
	أرجع FAIL;

    /*
     * block mode replace
     */
    إذا (oap->بلاک_ډېرپېښ)
    {
	bd.is_MAX = (curwin->w_curswant == MAXCOL);
	لكل ( ; curwin->w_cursor.lnum <= oap->پای_ټکی.lnum; ++curwin->w_cursor.lnum)
	{
	    curwin->w_cursor.ستون = 0;  // make sure cursor position is valid
	    بلاک_prep(oap, &bd, curwin->w_cursor.lnum, TRUE);
	    إذا (bd.textlen == 0 && (!virtual_op || bd.is_MAX))
		استمر;	    // nothing to replace

	    // n == number of extra chars required
	    // If we split a TAB, it may be replaced by several characters.
	    // Thus the number of characters may increase!
	    // If the range starts in virtual space, count the initial
	    // coladd offset as part of "startspaces"
	    إذا (virtual_op && bd.is_short && *bd.textstart == NUL)
	    {
		pos_T vpos;

		vpos.lnum = curwin->w_cursor.lnum;
		getvpos(&vpos, oap->پیل_vcol);
		bd.startspaces += vpos.coladd;
		n = bd.startspaces;
	    }
	    وإلا
		// allow for pre spaces
		n = (bd.startspaces ? bd.پیل_char_vcols - 1 : 0);

	    // allow for post spp
	    n += (bd.endspaces
		    && !bd.is_oneChar
		    && bd.پای_ټکی_char_vcols > 0) ? bd.پای_ټکی_char_vcols - 1 : 0;
	    // Figure out how many characters to replace.
	    numc = oap->پای_ټکی_vcol - oap->پیل_vcol + 1;
	    إذا (bd.is_short && (!virtual_op || bd.is_MAX))
		numc -= (oap->پای_ټکی_vcol - bd.پای_ټکی_vcol) + 1;

	    // A double-wide character can be replaced only up to half the
	    // times.
	    إذا ((*mb_char2cells)(c) > 1)
	    {
		إذا ((numc & 1) && !bd.is_short)
		{
		    ++bd.endspaces;
		    ++n;
		}
		numc = numc / 2;
	    }

	    // Compute bytes needed, move character count to num_chars.
	    عدد_chars = numc;
	    numc *= (*mb_char2len)(c);
	    // oldlen includes textlen, so don't double count
	    n += numc - bd.textlen;

	    oldp = ml_get_curline();
	    oldlen = ml_get_curline_len();
	    newp = alloc(oldlen + 1 + n);
	    إذا (newp == عدم)
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
	    إذا (had_ctrl_v_cr || (c != '\r' && c != '\n'))
	    {
		إذا (has_mbyte)
		{
		    طالما (--عدد_chars >= 0)
			newlen += (*mb_char2bytes)(c, newp + newlen);
		}
		وإلا
		{
		    vim_memset(newp + newlen, c, (size_t)numc);
		    newlen += numc;
		}
		إذا (!bd.is_short)
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
		إذا (after_p != عدم)
		    STRCPY(after_p, oldp + bd.textcol + bd.textlen);
	    }

	    // replace the line
	    ml_replace(curwin->w_cursor.lnum, newp, FALSE);
	    إذا (after_p != عدم)
	    {
		ml_append(curwin->w_cursor.lnum++, after_p, 0, FALSE);
		appended_کرښې_mark(curwin->w_cursor.lnum, 1L);
		oap->پای_ټکی.lnum++;
		vim_free(after_p);
	    }
	}
    }
    وإلا
    {
	/*
	 * MCHAR and MLINE motion replace.
	 */
	إذا (oap->motion_type == MLINE)
	{
	    oap->پیل.ستون = 0;
	    curwin->w_cursor.ستون = 0;
	    oap->پای_ټکی.ستون = ml_get_len(oap->پای_ټکی.lnum);
	    إذا (oap->پای_ټکی.ستون)
		--oap->پای_ټکی.ستون;
	}
	وإلا إذا (!oap->inclusive)
	    dec(&(oap->پای_ټکی));

	طالما (LTOREQ_POS(curwin->w_cursor, oap->پای_ټکی))
	{
	    صحيح بشپړ = FALSE;

	    n = gchar_cursor();
	    إذا (n != NUL)
	    {
		صحيح new_byte_len = (*mb_char2len)(c);
		صحيح old_byte_len = mb_ptr2len(ml_get_cursor());

		إذا (new_byte_len > 1 || old_byte_len > 1)
		{
		    // This is slow, but it handles replacing a single-byte
		    // with a multi-byte and the other way around.
		    إذا (curwin->w_cursor.lnum == oap->پای_ټکی.lnum)
			oap->پای_ټکی.ستون += new_byte_len - old_byte_len;
		    replace_character(c);
		    بشپړ = TRUE;
		}
		وإلا
		{
		    إذا (n == TAB)
		    {
			صحيح پای_ټکی_vcol = 0;

			إذا (curwin->w_cursor.lnum == oap->پای_ټکی.lnum)
			{
			    // oap->end has to be recalculated when
			    // the tab breaks
			    پای_ټکی_vcol = getviscol2(oap->پای_ټکی.ستون,
							     oap->پای_ټکی.coladd);
			}
			coladvance_force(getviscol());
			إذا (curwin->w_cursor.lnum == oap->پای_ټکی.lnum)
			    getvpos(&oap->پای_ټکی, پای_ټکی_vcol);
		    }
		    // with "coladd" set may move to just after a TAB
		    إذا (gchar_cursor() != NUL)
		    {
			PBYTE(curwin->w_cursor, c);
			بشپړ = TRUE;
		    }
		}
	    }
	    إذا (!بشپړ && virtual_op && curwin->w_cursor.lnum == oap->پای_ټکی.lnum)
	    {
		صحيح virtcols = oap->پای_ټکی.coladd;

		إذا (curwin->w_cursor.lnum == oap->پیل.lnum
			&& oap->پیل.ستون == oap->پای_ټکی.ستون && oap->پیل.coladd)
		    virtcols -= oap->پیل.coladd;

		// oap->end has been trimmed so it's effectively inclusive;
		// as a result an extra +1 must be counted so we don't
		// trample the NUL byte.
		coladvance_force(getviscol2(oap->پای_ټکی.ستون, oap->پای_ټکی.coladd) + 1);
		curwin->w_cursor.ستون -= (virtcols + 1);
		لكل (; virtcols >= 0; virtcols--)
		{
		    إذا ((*mb_char2len)(c) > 1)
		       replace_character(c);
		    وإلا
			PBYTE(curwin->w_cursor, c);
		    إذا (inc(&curwin->w_cursor) == -1)
			اكسر;
		}
	    }

	    // Advance to next character, stop at the end of the file.
	    إذا (inc_cursor() == -1)
		اكسر;
	}
    }

    curwin->w_cursor = oap->پیل;
    check_cursor();
    بدل_شوی_کرښې(oap->پیل.lnum, oap->پیل.ستون, oap->پای_ټکی.lnum + 1, 0L);

    إذا ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_پیل = oap->پیل;
	curbuf->b_op_پای_ټکی = oap->پای_ټکی;
    }

    أرجع OK;
}

ساكن صحيح swapchars(صحيح op_type, pos_T *pos, صحيح اوږدتیا);

/*
 * Handle the (non-standard vi) tilde operator.  Also for "gu", "gU" and "g?".
 */
    ساكن فراغ
op_tilde(oparg_T *oap)
{
    pos_T		pos;
    بنية بلاک_def	bd;
    صحيح			did_change = FALSE;

    إذا (u_save((linenr_T)(oap->پیل.lnum - 1),
				       (linenr_T)(oap->پای_ټکی.lnum + 1)) == FAIL)
	أرجع;

    pos = oap->پیل;
    إذا (oap->بلاک_ډېرپېښ)		    // Visual block mode
    {
	لكل (; pos.lnum <= oap->پای_ټکی.lnum; ++pos.lnum)
	{
	    صحيح one_change;

	    بلاک_prep(oap, &bd, pos.lnum, FALSE);
	    pos.ستون = bd.textcol;
	    one_change = swapchars(oap->op_type, &pos, bd.textlen);
	    did_change |= one_change;

#که_تعریف_شوی FEAT_NETBEANS_INTG
	    إذا (netbeans_فعال() && one_change)
	    {
		char_u *ptr;

		netbeans_removed(curbuf, pos.lnum, bd.textcol,
							    (اوږد)bd.textlen);
		// get the line now, it may have been flushed
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, bd.textcol,
						&ptr[bd.textcol], bd.textlen);
	    }
#د_که_پای
	}
	إذا (did_change)
	    بدل_شوی_کرښې(oap->پیل.lnum, 0, oap->پای_ټکی.lnum + 1, 0L);
    }
    وإلا				    // not block mode
    {
	إذا (oap->motion_type == MLINE)
	{
	    oap->پیل.ستون = 0;
	    pos.ستون = 0;
	    oap->پای_ټکی.ستون = ml_get_len(oap->پای_ټکی.lnum);
	    إذا (oap->پای_ټکی.ستون)
		--oap->پای_ټکی.ستون;
	}
	وإلا إذا (!oap->inclusive)
	    dec(&(oap->پای_ټکی));

	إذا (pos.lnum == oap->پای_ټکی.lnum)
	    did_change = swapchars(oap->op_type, &pos,
						  oap->پای_ټکی.ستون - pos.ستون + 1);
	وإلا
	    لكل (;;)
	    {
		did_change |= swapchars(oap->op_type, &pos,
			pos.lnum == oap->پای_ټکی.lnum ? oap->پای_ټکی.ستون + 1
						  : ml_get_pos_len(&pos));
		إذا (LTOREQ_POS(oap->پای_ټکی, pos) || inc(&pos) == -1)
		    اكسر;
	    }
	إذا (did_change)
	{
	    بدل_شوی_کرښې(oap->پیل.lnum, oap->پیل.ستون, oap->پای_ټکی.lnum + 1,
									  0L);
#که_تعریف_شوی FEAT_NETBEANS_INTG
	    إذا (netbeans_فعال())
	    {
		char_u *ptr;
		صحيح شمېر;

		pos = oap->پیل;
		طالما (pos.lnum < oap->پای_ټکی.lnum)
		{
		    ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		    شمېر = ml_get_buf_len(curbuf, pos.lnum) - pos.ستون;
		    netbeans_removed(curbuf, pos.lnum, pos.ستون, (اوږد)شمېر);
		    // get the line again, it may have been flushed
		    ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		    netbeans_inserted(curbuf, pos.lnum, pos.ستون,
							&ptr[pos.ستون], شمېر);
		    pos.ستون = 0;
		    pos.lnum++;
		}
		شمېر = oap->پای_ټکی.ستون - pos.ستون + 1;
		netbeans_removed(curbuf, pos.lnum, pos.ستون, (اوږد)شمېر);
		// get the line again, it may have been flushed
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, pos.ستون,
							&ptr[pos.ستون], شمېر);
	    }
#د_که_پای
	}
    }

    إذا (!did_change && oap->is_VIsual)
	// No change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    إذا ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set '[ and '] marks.
	curbuf->b_op_پیل = oap->پیل;
	curbuf->b_op_پای_ټکی = oap->پای_ټکی;
    }

    إذا (oap->کرښه_شمېر > p_report)
	smsg(NGETTEXT("%ld line changed", "%ld lines changed",
					    oap->کرښه_شمېر), oap->کرښه_شمېر);
}

/*
 * Invoke swapchar() on "length" bytes at position "pos".
 * "pos" is advanced to just after the changed characters.
 * "length" is rounded up to include the whole last multi-byte character.
 * Also works correctly when the number of bytes changes.
 * Returns TRUE if some character was changed.
 */
    ساكن صحيح
swapchars(صحيح op_type, pos_T *pos, صحيح اوږدتیا)
{
    صحيح todo;
    صحيح	did_change = 0;

    لكل (todo = اوږدتیا; todo > 0; --todo)
    {
	إذا (has_mbyte)
	{
	    صحيح len = (*mb_ptr2len)(ml_get_pos(pos));

	    // we're counting bytes, not characters
	    إذا (len > 0)
		todo -= len - 1;
	}
	did_change |= swapchar(op_type, pos);
	إذا (inc(pos) == -1)    // at end of file
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
    إذا (c >= 0x80 && op_type == OP_ROT13)
	أرجع FALSE;

    // ~ is OP_NOP, g~ is OP_TILDE, gU is OP_UPPER
    إذا ((op_type == OP_پاسنی || op_type == OP_NOP || op_type == OP_TILDE)
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

    إذا (enc_dbcs != 0 && c >= 0x100)	// No lower/uppercase letter
	أرجع FALSE;
    nc = c;
    إذا (MB_ISLOWER(c))
    {
	إذا (op_type == OP_ROT13)
	    nc = ROT13(c, 'a');
	وإلا إذا (op_type != OP_ښکتنی)
	    nc = MB_TOUPPER(c);
    }
    وإلا إذا (MB_ISUPPER(c))
    {
	إذا (op_type == OP_ROT13)
	    nc = ROT13(c, 'A');
	وإلا إذا (op_type != OP_پاسنی)
	    nc = MB_TOLOWER(c);
    }
    إذا (nc != c)
    {
	إذا (enc_utf8 && (c >= 0x80 || nc >= 0x80))
	{
	    pos_T   sp = curwin->w_cursor;

	    curwin->w_cursor = *pos;
	    // don't use del_char(), it also removes composing chars
	    del_بایټونه(utf_ptr2len(ml_get_cursor()), FALSE, FALSE);
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
op_ورننباسه(oparg_T *oap, اوږد count1)
{
    اوږد		pre_textlen = 0;
    colnr_T		ind_pre_ستون = 0, ind_post_ستون;
    صحيح			ind_pre_vcol = 0, ind_post_vcol = 0;
    بنية بلاک_def	bd;
    صحيح			i;
    pos_T		t1;
    pos_T		پیل_ورننباسه;

    // edit() changes this - record it for OP_APPEND
    bd.is_MAX = (curwin->w_curswant == MAXCOL);

    // vis block is still marked. Get rid of it now.
    curwin->w_cursor.lnum = oap->پیل.lnum;
    update_screen(UPD_INVERTED);

    إذا (oap->بلاک_ډېرپېښ)
    {
	// When 'virtualedit' is used, need to insert the extra spaces before
	// doing block_prep().  When only "block" is used, virtual edit is
	// already disabled, but still need it when calling
	// coladvance_force().
	// coladvance_force() uses get_ve_flags() to get the 'virtualedit'
	// state for the current window.  To override that state, we need to
	// set the window-local value of ve_flags rather than the global value.
	إذا (curwin->w_cursor.coladd > 0)
	{
	    صحيح		old_ve_flags = curwin->w_ve_flags;

	    إذا (u_save_cursor() == FAIL)
		أرجع;

	    curwin->w_ve_flags = VE_ALL;
	    coladvance_force(oap->op_type == OP_APPEND
					   ? oap->پای_ټکی_vcol + 1 : getviscol());
	    إذا (oap->op_type == OP_APPEND)
		--curwin->w_cursor.ستون;
	    curwin->w_ve_flags = old_ve_flags;
	}
	// Get the info about the block before entering the text
	بلاک_prep(oap, &bd, oap->پیل.lnum, TRUE);
	// Get indent information
	ind_pre_ستون = (colnr_T)getwhitecols_curline();
	ind_pre_vcol = get_indent();
	pre_textlen = ml_get_len(oap->پیل.lnum) - bd.textcol;
	إذا (oap->op_type == OP_APPEND)
	    pre_textlen -= bd.textlen;
    }

    إذا (oap->op_type == OP_APPEND)
    {
	إذا (oap->بلاک_ډېرپېښ && curwin->w_cursor.coladd == 0)
	{
	    // Move the cursor to the character right of the block.
	    curwin->w_سېټ_curswant = true;
	    طالما (*ml_get_cursor() != NUL
		    && (curwin->w_cursor.ستون < bd.textcol + bd.textlen))
		++curwin->w_cursor.ستون;
	    إذا (bd.is_short && !bd.is_MAX)
	    {
		// First line was too short, make it longer and adjust the
		// values in "bd".
		إذا (u_save_cursor() == FAIL)
		    أرجع;
		لكل (i = 0; i < bd.endspaces; ++i)
		    ins_char(' ');
		bd.textlen += bd.endspaces;
	    }
	}
	وإلا
	{
	    curwin->w_cursor = oap->پای_ټکی;
	    check_cursor_ستون();

	    // Works just like an 'i'nsert on the next character.
	    إذا (!LINEEMPTY(curwin->w_cursor.lnum)
		    && oap->پیل_vcol != oap->پای_ټکی_vcol)
		inc_cursor();
	}
    }

    t1 = oap->پیل;
    پیل_ورننباسه = curwin->w_cursor;
    (فراغ)edit(NUL, FALSE, (linenr_T)count1);

    // When a tab was inserted, and the characters in front of the tab
    // have been converted to a tab as well, the column of the cursor
    // might have actually been reduced, so need to adjust here.
    إذا (t1.lnum == curbuf->b_op_پیل_orig.lnum
	    && LT_POS(curbuf->b_op_پیل_orig, t1))
	oap->پیل = curbuf->b_op_پیل_orig;

    // If user has moved off this line, we don't know what to do, so do
    // nothing.
    // Also don't repeat the insert when Insert mode ended with CTRL-C.
    إذا (curwin->w_cursor.lnum != oap->پیل.lnum || got_int)
	أرجع;

    إذا (oap->بلاک_ډېرپېښ)
    {
	صحيح			ins_len;
	char_u			*firstline, *ins_متن;
	بنية بلاک_def	bd2;
	صحيح			did_indent = FALSE;
	size_t			len;
	size_t			جمع_کړه;
	// offset when cursor was moved in insert mode
	صحيح			دننه_واټن = 0;

	// If indent kicked in, the firstline might have changed
	// but only do that, if the indent actually increased.
	ind_post_ستون = (colnr_T)getwhitecols_curline();
	إذا (curbuf->b_op_پیل.ستون > ind_pre_ستون && ind_post_ستون > ind_pre_ستون)
	{
	    bd.textcol += ind_post_ستون - ind_pre_ستون;
	    ind_post_vcol = get_indent();
	    bd.پیل_vcol += ind_post_vcol - ind_pre_vcol;
	    did_indent = TRUE;
	}

	// The user may have moved the cursor before inserting something, try
	// to adjust the block for that.  But only do it, if the difference
	// does not come from indent kicking in.
	إذا (oap->پیل.lnum == curbuf->b_op_پیل_orig.lnum
						  && !bd.is_MAX && !did_indent)
	{
	    صحيح t = getviscol2(curbuf->b_op_پیل_orig.ستون,
					       curbuf->b_op_پیل_orig.coladd);

	    إذا (oap->op_type == OP_ورننباسه
		    && oap->پیل.ستون + oap->پیل.coladd
			    != curbuf->b_op_پیل_orig.ستون
					      + curbuf->b_op_پیل_orig.coladd)
	    {
		oap->پیل.ستون = curbuf->b_op_پیل_orig.ستون;
		pre_textlen -= t - oap->پیل_vcol;
		oap->پیل_vcol = t;
	    }
	    وإلا إذا (oap->op_type == OP_APPEND
		    && oap->پیل.ستون + oap->پیل.coladd
			    >= curbuf->b_op_پیل_orig.ستون
					      + curbuf->b_op_پیل_orig.coladd)
	    {
		oap->پیل.ستون = curbuf->b_op_پیل_orig.ستون;
		// reset pre_textlen to the value of OP_INSERT
		pre_textlen += bd.textlen;
		pre_textlen -= t - oap->پیل_vcol;
		oap->پیل_vcol = t;
		oap->op_type = OP_ورننباسه;
	    }
	}

	// Spaces and tabs in the indent may have changed to other spaces and
	// tabs.  Get the starting column again and correct the length.
	// Don't do this when "$" used, end-of-line will have changed.
	//
	// if indent was added and the inserted text was after the indent,
	// correct the selection for the new indent.
	إذا (did_indent && bd.textcol - ind_post_ستون > 0)
	{
	    oap->پیل.ستون += ind_post_ستون - ind_pre_ستون;
	    oap->پیل_vcol += ind_post_vcol - ind_pre_vcol;
	    oap->پای_ټکی.ستون += ind_post_ستون - ind_pre_ستون;
	    oap->پای_ټکی_vcol += ind_post_vcol - ind_pre_vcol;
	}
	بلاک_prep(oap, &bd2, oap->پیل.lnum, TRUE);
	إذا (did_indent && bd.textcol - ind_post_ستون > 0)
	{
	    // undo for where "oap" is used below
	    oap->پیل.ستون -= ind_post_ستون - ind_pre_ستون;
	    oap->پیل_vcol -= ind_post_vcol - ind_pre_vcol;
	    oap->پای_ټکی.ستون -= ind_post_ستون - ind_pre_ستون;
	    oap->پای_ټکی_vcol -= ind_post_vcol - ind_pre_vcol;
	}
	إذا (!bd.is_MAX || bd2.textlen < bd.textlen)
	{
	    إذا (oap->op_type == OP_APPEND)
	    {
		pre_textlen += bd2.textlen - bd.textlen;
		إذا (bd2.endspaces)
		    --bd2.textlen;
	    }
	    bd.textcol = bd2.textcol;
	    bd.textlen = bd2.textlen;
	}

	/*
	 * Subsequent calls to ml_get() flush the firstline data - take a
	 * copy of the required string.
	 */
	firstline = ml_get(oap->پیل.lnum);
	len = ml_get_len(oap->پیل.lnum);
	جمع_کړه = bd.textcol;
	إذا (oap->op_type == OP_APPEND)
	{
	    جمع_کړه += bd.textlen;
	    // account for pressing cursor in insert mode when '$' was used
	    إذا (bd.is_MAX
		&& (پیل_ورننباسه.lnum == Insstart.lnum
					   && پیل_ورننباسه.ستون > Insstart.ستون))
	    {
		دننه_واټن = (پیل_ورننباسه.ستون - Insstart.ستون);
		جمع_کړه -= دننه_واټن;
		إذا (oap->پای_ټکی_vcol > دننه_واټن)
		    oap->پای_ټکی_vcol -= (دننه_واټن + 1);
		وإلا
		    // moved outside of the visual block, what to do?
		    أرجع;
	    }
	}
	إذا (جمع_کړه > len)
	    جمع_کړه = len;  // short line, point to the NUL
	firstline += جمع_کړه;
	len -= جمع_کړه;
	إذا (pre_textlen >= 0 && (ins_len = (صحيح)len - pre_textlen - دننه_واټن) > 0)
	{
	    ins_متن = vim_strnsave(firstline, ins_len);
	    إذا (ins_متن != عدم)
	    {
		// block handled here
		إذا (u_save(oap->پیل.lnum,
					 (linenr_T)(oap->پای_ټکی.lnum + 1)) == OK)
		    بلاک_ورننباسه(oap, ins_متن, ins_len, (oap->op_type == OP_ورننباسه),
									 &bd);

		curwin->w_cursor.ستون = oap->پیل.ستون;
		check_cursor();
		vim_free(ins_متن);
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
    اوږد		pre_textlen = 0;
    اوږد		pre_indent = 0;
    char_u		*firstline;
    char_u		*ins_متن, *newp, *oldp;
    بنية بلاک_def	bd;

    l = oap->پیل.ستون;
    إذا (oap->motion_type == MLINE)
    {
	l = 0;
	can_si = may_do_si();	// Like opening a new line, do smart indent
    }

    // First delete the text in the region.  In an empty buffer only need to
    // save for undo
    إذا (curbuf->b_ml.ml_flags & ML_تشه)
    {
	إذا (u_save_cursor() == FAIL)
	    أرجع FALSE;
    }
    وإلا إذا (op_delete(oap) == FAIL)
	أرجع FALSE;

    إذا ((l > curwin->w_cursor.ستون) && !LINEEMPTY(curwin->w_cursor.lnum)
							 && !virtual_op)
	inc_cursor();

    // check for still on same line (<CR> in inserted text meaningless)
    // skip blank lines too
    إذا (oap->بلاک_ډېرپېښ)
    {
	// Add spaces before getting the current line length.
	إذا (virtual_op && (curwin->w_cursor.coladd > 0
						    || gchar_cursor() == NUL))
	    coladvance_force(getviscol());
	firstline = ml_get(oap->پیل.lnum);
	pre_textlen = ml_get_len(oap->پیل.lnum);
	pre_indent = (اوږد)getwhitecols(firstline);
	bd.textcol = curwin->w_cursor.ستون;
    }

    إذا (oap->motion_type == MLINE)
	fix_indent();

    // Reset finish_op now, don't want it set inside edit().
    صحيح save_پای_op = پای_op;
    پای_op = FALSE;

    retval = edit(NUL, FALSE, (linenr_T)1);

    پای_op = save_پای_op;

    /*
     * In Visual block mode, handle copying the new text to all lines of the
     * block.
     * Don't repeat the insert when Insert mode ended with CTRL-C.
     */
    إذا (oap->بلاک_ډېرپېښ && oap->پیل.lnum != oap->پای_ټکی.lnum && !got_int)
    {
	صحيح	ins_len;

	// Auto-indenting may have changed the indent.  If the cursor was past
	// the indent, exclude that indent change from the inserted text.
	firstline = ml_get(oap->پیل.lnum);
	إذا (bd.textcol > (colnr_T)pre_indent)
	{
	    اوږد new_indent = (اوږد)getwhitecols(firstline);

	    pre_textlen += new_indent - pre_indent;
	    bd.textcol += new_indent - pre_indent;
	}

	ins_len = (صحيح)ml_get_len(oap->پیل.lnum) - pre_textlen;
	إذا (ins_len > 0)
	{
	    // Subsequent calls to ml_get() flush the firstline data - take a
	    // copy of the inserted text.
	    إذا ((ins_متن = alloc(ins_len + 1)) != عدم)
	    {
		vim_strncpy(ins_متن, firstline + bd.textcol, ins_len);
		لكل (linenr = oap->پیل.lnum + 1; linenr <= oap->پای_ټکی.lnum;
								     linenr++)
		{
		    بلاک_prep(oap, &bd, linenr, TRUE);
		    إذا (!bd.is_short || virtual_op)
		    {
			pos_T vpos;
			size_t newlen;

			// If the block starts in virtual space, count the
			// initial coladd offset as part of "startspaces"
			إذا (bd.is_short)
			{
			    vpos.lnum = linenr;
			    (فراغ)getvpos(&vpos, oap->پیل_vcol);
			}
			وإلا
			    vpos.coladd = 0;
			oldp = ml_get(linenr);
			newp = alloc(ml_get_len(linenr) + vpos.coladd + ins_len + 1);
			إذا (newp == عدم)
			    استمر;
			// copy up to block start
			mch_memmove(newp, oldp, (size_t)bd.textcol);
			newlen = bd.textcol;
			vim_memset(newp + newlen, ' ', (size_t)vpos.coladd);
			newlen += vpos.coladd;
			mch_memmove(newp + newlen, ins_متن, ins_len);
			STRCPY(newp + newlen + ins_len, oldp + bd.textcol);
			ml_replace(linenr, newp, FALSE);
#که_تعریف_شوی FEAT_PROP_POPUP
			// Shift the properties for linenr as edit() would do.
			إذا (curbuf->b_has_textprop)
			    adjust_prop_columns(linenr, bd.textcol,
						     vpos.coladd + (صحيح)ins_len, 0);
#د_که_پای
		    }
		}
		check_cursor();

		بدل_شوی_کرښې(oap->پیل.lnum + 1, 0, oap->پای_ټکی.lnum + 1, 0L);
	    }
	    vim_free(ins_متن);
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
    بینښه صحيح cur_ve_flags = get_ve_flags();

    صحيح adj_cursor = (curwin->w_cursor.ستون > 0
				&& gchar_cursor() == NUL
				&& (cur_ve_flags & VE_ONEMORE) == 0
				&& (cur_ve_flags & VE_ALL) == 0
				&& !(restart_edit || (State & ډېرپېښ_ورننباسه)));
    إذا (!adj_cursor)
	أرجع;

    // Put the cursor on the last character in the line.
    dec_cursor();

    إذا (cur_ve_flags == VE_ALL)
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
    char_u   *کرښه,
    صحيح      process,
    صحيح	     include_space,
    صحيح      *is_comment)
{
    char_u *comment_flags = عدم;
    صحيح    lead_len;
    صحيح    leader_دننه_واټن = get_وروستی_leader_دننه_واټن(کرښه, &comment_flags);

    *is_comment = FALSE;
    إذا (leader_دننه_واټن != -1)
    {
	// Let's check whether the line ends with an unclosed comment.
	// If the last comment leader has COM_END in flags, there's no comment.
	طالما (*comment_flags)
	{
	    إذا (*comment_flags == COM_پای_ټکی
		    || *comment_flags == ':')
		اكسر;
	    ++comment_flags;
	}
	إذا (*comment_flags != COM_پای_ټکی)
	    *is_comment = TRUE;
    }

    إذا (process == FALSE)
	أرجع کرښه;

    lead_len = get_leader_len(کرښه, &comment_flags, FALSE, include_space);

    إذا (lead_len == 0)
	أرجع کرښه;

    // Find:
    // - COM_END,
    // - colon,
    // whichever comes first.
    طالما (*comment_flags)
    {
	إذا (*comment_flags == COM_پای_ټکی
		|| *comment_flags == ':')
	    اكسر;
	++comment_flags;
    }

    // If we found a colon, it means that we are not processing a line
    // starting with a closing part of a three-part comment. That's good,
    // because we don't want to remove those as this would be annoying.
    إذا (*comment_flags == ':' || *comment_flags == NUL)
	کرښه += lead_len;

    أرجع کرښه;
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
    اوږد    شمېر,
    صحيح	    ورننباسه_space,
    صحيح	    save_undo,
    صحيح	    use_formatoptions ناکارول_شوی,
    صحيح	    setmark)
{
    char_u	*curr = عدم;
    char_u      *curr_پیل = عدم;
    char_u	*cend;
    char_u	*newp;
    size_t	newp_len;
    char_u	*spaces;	// number of spaces inserted before a line
    صحيح		endcurr1 = NUL;
    صحيح		endcurr2 = NUL;
    صحيح		currsize = 0;	// size of the current line
    صحيح		sumsize = 0;	// size of the long new line
    linenr_T	t;
    colnr_T	ستون = 0;
    صحيح		ret = OK;
    صحيح		*comments = عدم;
    صحيح		remove_comments = (use_formatoptions == TRUE)
				  && has_format_option(FO_REMOVE_COMS);
    صحيح		prev_was_comment;

    إذا (save_undo && u_save((linenr_T)(curwin->w_cursor.lnum - 1),
			    (linenr_T)(curwin->w_cursor.lnum + شمېر)) == FAIL)
	أرجع FAIL;

    // Allocate an array to store the number of spaces inserted before each
    // line.  We will use it to pre-compute the length of the new line and the
    // proper placement of each original line in the new one.
    spaces = lalloc_clear(شمېر, TRUE);
    إذا (spaces == عدم)
	أرجع FAIL;
    إذا (remove_comments)
    {
	comments = lalloc_clear(شمېر * حجم(صحيح), TRUE);
	إذا (comments == عدم)
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
    لكل (t = 0; t < شمېر; ++t)
    {
	curr = curr_پیل = ml_get((linenr_T)(curwin->w_cursor.lnum + t));
	إذا (t == 0 && setmark && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set the '[ mark.
	    curwin->w_بفر->b_op_پیل.lnum = curwin->w_cursor.lnum;
	    curwin->w_بفر->b_op_پیل.ستون  = (colnr_T)STRLEN(curr);
	}
	إذا (remove_comments)
	{
	    // We don't want to remove the comment leader if the
	    // previous line is not a comment.
	    إذا (t > 0 && prev_was_comment)
	    {

		char_u *new_curr = skip_comment(curr, TRUE, ورننباسه_space,
							   &prev_was_comment);
		comments[t] = (صحيح)(new_curr - curr);
		curr = new_curr;
	    }
	    وإلا
		curr = skip_comment(curr, FALSE, ورننباسه_space,
							   &prev_was_comment);
	}

	إذا (ورننباسه_space && t > 0)
	{
	    curr = skipwhite(curr);
	    إذا (*curr != NUL && *curr != ')'
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
		إذا (endcurr1 == ' ')
		    endcurr1 = endcurr2;
		وإلا
		    ++spaces[t];
		// extra space when 'joinspaces' set and line ends in '.'
		إذا (       p_js
			&& (endcurr1 == '.'
			    || (vim_strchr(p_cpo, CPO_JOINSP) == عدم
				&& (endcurr1 == '?' || endcurr1 == '!'))))
		    ++spaces[t];
	    }
	}
	currsize = (صحيح)STRLEN(curr);
	sumsize += currsize + spaces[t];
	endcurr1 = endcurr2 = NUL;
	إذا (ورننباسه_space && currsize > 0)
	{
	    إذا (has_mbyte)
	    {
		cend = curr + currsize;
		MB_PTR_شا(curr, cend);
		endcurr1 = (*mb_ptr2char)(cend);
		إذا (cend > curr)
		{
		    MB_PTR_شا(curr, cend);
		    endcurr2 = (*mb_ptr2char)(cend);
		}
	    }
	    وإلا
	    {
		endcurr1 = *(curr + currsize - 1);
		إذا (currsize > 1)
		    endcurr2 = *(curr + currsize - 2);
	    }
	}
	کرښه_breakcheck();
	إذا (got_int)
	{
	    ret = FAIL;
	    اذهب theend;
	}
    }

    // store the column position before last line
    ستون = sumsize - currsize - spaces[شمېر - 1];

    // allocate the space for the new line
    newp_len = sumsize + 1;
    newp = alloc(newp_len);
    إذا (newp == عدم)
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
#که_تعریف_شوی FEAT_PROP_POPUP
    unpacked_memline_T um = um_open_at_no_props(
				curwin->w_بفر, curwin->w_cursor.lnum, 0);
    // um_open_at_no_props may have invalidated "curr".
    صحيح curr_off = (صحيح)(curr - curr_پیل);

    curr = curr_پیل = ml_get((linenr_T)(curwin->w_cursor.lnum + شمېر - 1));
    curr += curr_off;
#د_که_پای

    لكل (t = شمېر - 1; ; --t)
    {
	صحيح spaces_removed;

	cend -= currsize;
	mch_memmove(cend, curr, (size_t)currsize);

	إذا (spaces[t] > 0)
	{
	    cend -= spaces[t];
	    vim_memset(cend, ' ', (size_t)(spaces[t]));
	}

	// If deleting more spaces than adding, the cursor moves no more than
	// what is added if it is inside these spaces.
	spaces_removed = (curr - curr_پیل) - spaces[t];

	mark_ستون_adjust(curwin->w_cursor.lnum + t, (colnr_T)0, -t,
			 (اوږد)(cend - newp - spaces_removed), spaces_removed);
#که_تعریف_شوی FEAT_PROP_POPUP
	prepend_joined_props(&um, curwin->w_cursor.lnum + t,
		t == شمېر - 1, (اوږد)(cend - newp), spaces_removed);
#د_که_پای
	إذا (t == 0)
	    اكسر;
	curr = curr_پیل = ml_get((linenr_T)(curwin->w_cursor.lnum + t - 1));
	إذا (remove_comments)
	    curr += comments[t - 1];
	إذا (ورننباسه_space && t > 1)
	    curr = skipwhite(curr);
	currsize = (صحيح)STRLEN(curr);
    }

#که_تعریف_شوی FEAT_PROP_POPUP
    إذا (um.buf != عدم)
    {
	um_سېټ_متن(&um, newp);
	um_سرچپه_props(&um);
	um_close(&um);
	newp = عدم;  // um_set_text took ownership
    }
    وإلا
#د_که_پای
    ml_replace_len(curwin->w_cursor.lnum, newp, (colnr_T)newp_len, TRUE, FALSE);

    إذا (setmark && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set the '] mark.
	curwin->w_بفر->b_op_پای_ټکی.lnum = curwin->w_cursor.lnum;
	curwin->w_بفر->b_op_پای_ټکی.ستون  = (colnr_T)sumsize;
    }

    // Only report the change in the first line here, del_lines() will report
    // the deleted line.
    بدل_شوی_کرښې(curwin->w_cursor.lnum, currsize,
					       curwin->w_cursor.lnum + 1, 0L);
    /*
     * Delete following lines. To do this we move the cursor there
     * briefly, and then move it back. After del_lines() the cursor may
     * have moved up (last line deleted), so the current lnum is kept in t.
     */
    t = curwin->w_cursor.lnum;
    ++curwin->w_cursor.lnum;
    del_کرښې(شمېر - 1, FALSE);
    curwin->w_cursor.lnum = t;

    /*
     * Set the cursor column:
     * Vi compatible: use the column of the first join
     * vim:	      use the column of the last join
     */
    curwin->w_cursor.ستون =
		    (vim_strchr(p_cpo, CPO_JOINCOL) != عدم ? currsize : ستون);
    check_cursor_ستون();

    curwin->w_cursor.coladd = 0;
    curwin->w_سېټ_curswant = true;

theend:
    vim_free(spaces);
    إذا (remove_comments)
	vim_free(comments);
    أرجع ret;
}

#که_تعریف_شوی FEAT_LINEBREAK
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
    إذا (!curwin->w_p_lbr)
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
    إذا (curwin->w_p_lbr || !lbr_saved)
	أرجع;

    // changing 'linebreak' may require w_virtcol to be updated
    curwin->w_p_lbr = TRUE;
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);
}
#د_که_پای

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
بلاک_prep(
    oparg_T		*oap,
    بنية بلاک_def	*bdp,
    linenr_T		lnum,
    صحيح			is_del)
{
    صحيح		incr = 0;
    char_u	*pend;
    char_u	*pstart;
    char_u	*کرښه;
    char_u	*prev_pstart;
    char_u	*prev_pend;
    chartabsize_T cts;
#که_تعریف_شوی FEAT_LINEBREAK
		// Avoid a problem with unwanted linebreaks in block mode.
    صحيح		lbr_saved = reset_lbr();
#د_که_پای

    bdp->startspaces = 0;
    bdp->endspaces = 0;
    bdp->textlen = 0;
    bdp->پیل_vcol = 0;
    bdp->پای_ټکی_vcol = 0;
    bdp->is_short = FALSE;
    bdp->is_oneChar = FALSE;
    bdp->pre_whitesp = 0;
    bdp->pre_whitesp_c = 0;
    bdp->پای_ټکی_char_vcols = 0;
    bdp->پیل_char_vcols = 0;

    کرښه = ml_get(lnum);
    prev_pstart = کرښه;
    init_chartabsize_arg(&cts, curwin, lnum, bdp->پیل_vcol, کرښه, کرښه);
    طالما (cts.cts_vcol < oap->پیل_vcol && *cts.cts_ptr != NUL)
    {
	// Count a tab for what it's worth (if list mode not on)
	incr = lbr_chartabsize(&cts);
	cts.cts_vcol += incr;
	إذا (VIM_ISWHITE(*cts.cts_ptr))
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
    bdp->پیل_vcol = cts.cts_vcol;
    pstart = cts.cts_ptr;
    clear_chartabsize_arg(&cts);

    bdp->پیل_char_vcols = incr;
    إذا (bdp->پیل_vcol < oap->پیل_vcol)	// line too short
    {
	bdp->پای_ټکی_vcol = bdp->پیل_vcol;
	bdp->is_short = TRUE;
	إذا (!is_del || oap->op_type == OP_APPEND)
	    bdp->endspaces = oap->پای_ټکی_vcol - oap->پیل_vcol + 1;
    }
    وإلا
    {
	// notice: this converts partly selected Multibyte characters to
	// spaces, too.
	bdp->startspaces = bdp->پیل_vcol - oap->پیل_vcol;
	إذا (is_del && bdp->startspaces)
	    bdp->startspaces = bdp->پیل_char_vcols - bdp->startspaces;
	pend = pstart;
	bdp->پای_ټکی_vcol = bdp->پیل_vcol;
	إذا (bdp->پای_ټکی_vcol > oap->پای_ټکی_vcol)	// it's all in one character
	{
	    bdp->is_oneChar = TRUE;
	    إذا (oap->op_type == OP_ورننباسه)
		bdp->endspaces = bdp->پیل_char_vcols - bdp->startspaces;
	    وإلا إذا (oap->op_type == OP_APPEND)
	    {
		bdp->startspaces += oap->پای_ټکی_vcol - oap->پیل_vcol + 1;
		bdp->endspaces = bdp->پیل_char_vcols - bdp->startspaces;
	    }
	    وإلا
	    {
		bdp->startspaces = oap->پای_ټکی_vcol - oap->پیل_vcol + 1;
		إذا (is_del && oap->op_type != OP_LSHIFT)
		{
		    // just putting the sum of those two into
		    // bdp->startspaces doesn't work for Visual replace,
		    // so we have to split the tab in two
		    bdp->startspaces = bdp->پیل_char_vcols
					- (bdp->پیل_vcol - oap->پیل_vcol);
		    bdp->endspaces = bdp->پای_ټکی_vcol - oap->پای_ټکی_vcol - 1;
		}
	    }
	}
	وإلا
	{
	    init_chartabsize_arg(&cts, curwin, lnum, bdp->پای_ټکی_vcol,
								  کرښه, pend);
	    prev_pend = pend;
	    طالما (cts.cts_vcol <= oap->پای_ټکی_vcol && *cts.cts_ptr != NUL)
	    {
		// count a tab for what it's worth (if list mode not on)
		prev_pend = cts.cts_ptr;
		incr = lbr_chartabsize_adv(&cts);
		cts.cts_vcol += incr;
	    }
	    bdp->پای_ټکی_vcol = cts.cts_vcol;
	    pend = cts.cts_ptr;
	    clear_chartabsize_arg(&cts);

	    إذا (bdp->پای_ټکی_vcol <= oap->پای_ټکی_vcol
		    && (!is_del
			|| oap->op_type == OP_APPEND
			|| oap->op_type == OP_REPLACE)) // line too short
	    {
		bdp->is_short = TRUE;
		// Alternative: include spaces to fill up the block.
		// Disadvantage: can lead to trailing spaces when the line is
		// short where the text is put
		// if (!is_del || oap->op_type == OP_APPEND)
		إذا (oap->op_type == OP_APPEND || virtual_op)
		    bdp->endspaces = oap->پای_ټکی_vcol - bdp->پای_ټکی_vcol
							     + oap->inclusive;
		وإلا
		    bdp->endspaces = 0; // replace doesn't add characters
	    }
	    وإلا إذا (bdp->پای_ټکی_vcol > oap->پای_ټکی_vcol)
	    {
		bdp->endspaces = bdp->پای_ټکی_vcol - oap->پای_ټکی_vcol - 1;
		إذا (!is_del && bdp->endspaces)
		{
		    bdp->endspaces = incr - bdp->endspaces;
		    إذا (pend != pstart)
			pend = prev_pend;
		}
	    }
	}
	bdp->پای_ټکی_char_vcols = incr;
	إذا (is_del && bdp->startspaces)
	    pstart = prev_pstart;
	bdp->textlen = (صحيح)(pend - pstart);
    }
    bdp->textcol = (colnr_T) (pstart - کرښه);
    bdp->textstart = pstart;
#که_تعریف_شوی FEAT_LINEBREAK
    restore_lbr(lbr_saved);
#د_که_پای
}

/*
 * Get block text from "start" to "end"
 */
    فراغ
charwise_بلاک_prep(
    pos_T		پیل,
    pos_T		پای_ټکی,
    بنية بلاک_def	*bdp,
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
    bdp->پیل_char_vcols = 0;

    إذا (lnum == پیل.lnum)
    {
	startcol = پیل.ستون;
	إذا (virtual_op)
	{
	    getvcol(curwin, &پیل, &cs, عدم, &ce, 0);
	    إذا (ce != cs && پیل.coladd > 0)
	    {
		// Part of a tab selected -- but don't
		// double-count it.
		bdp->پیل_char_vcols = ce - cs + 1;
		bdp->startspaces = bdp->پیل_char_vcols - پیل.coladd;
		إذا (bdp->startspaces < 0)
		    bdp->startspaces = 0;
		startcol++;
	    }
	}
    }

    إذا (lnum == پای_ټکی.lnum)
    {
	endcol = پای_ټکی.ستون;
	إذا (virtual_op)
	{
	    getvcol(curwin, &پای_ټکی, &cs, عدم, &ce, 0);
	    إذا (p[endcol] == NUL || (cs + پای_ټکی.coladd < ce
			// Don't add space for double-wide
			// char; endcol will be on last byte
			// of multi-byte char.
			&& (*mb_سر_off)(p, p + endcol) == 0))
	    {
		إذا (پیل.lnum == پای_ټکی.lnum && پیل.ستون == پای_ټکی.ستون)
		{
		    // Special case: inside a single char
		    bdp->is_oneChar = TRUE;
		    bdp->startspaces = پای_ټکی.coladd - پیل.coladd + inclusive;
		    endcol = startcol;
		}
		وإلا
		{
		    bdp->endspaces = پای_ټکی.coladd + inclusive;
		    endcol -= inclusive;
		}
	    }
	}
    }
    إذا (endcol == MAXCOL)
	endcol = ml_get_len(lnum);
    إذا (startcol > endcol || bdp->is_oneChar)
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
    بنية بلاک_def	bd;
    صحيح			change_cnt = 0;
    linenr_T		amount = Prenum1;

    // do_addsub() might trigger re-evaluation of 'foldexpr' halfway, when the
    // buffer is not completely updated yet. Postpone updating folds until before
    // the call to changed_lines().
#که_تعریف_شوی FEAT_FOLDING
    disable_fold_update++;
#د_که_پای

    إذا (!VIsual_فعال)
    {
	pos = curwin->w_cursor;
	إذا (u_save_cursor() == FAIL)
	{
#که_تعریف_شوی FEAT_FOLDING
	    disable_fold_update--;
#د_که_پای
	    أرجع;
	}
	change_cnt = do_addsub(oap->op_type, &pos, 0, amount);
#که_تعریف_شوی FEAT_FOLDING
	disable_fold_update--;
#د_که_پای
	إذا (change_cnt)
	    بدل_شوی_کرښې(pos.lnum, 0, pos.lnum + 1, 0L);
    }
    وإلا
    {
	صحيح	one_change;
	صحيح	اوږدتیا;
	pos_T	startpos;

	إذا (u_save((linenr_T)(oap->پیل.lnum - 1),
					(linenr_T)(oap->پای_ټکی.lnum + 1)) == FAIL)
	{
#که_تعریف_شوی FEAT_FOLDING
	    disable_fold_update--;
#د_که_پای
	    أرجع;
	}

	pos = oap->پیل;
	لكل (; pos.lnum <= oap->پای_ټکی.lnum; ++pos.lnum)
	{
	    إذا (oap->بلاک_ډېرپېښ)		    // Visual block mode
	    {
		بلاک_prep(oap, &bd, pos.lnum, FALSE);
		pos.ستون = bd.textcol;
		اوږدتیا = bd.textlen;
	    }
	    وإلا إذا (oap->motion_type == MLINE)
	    {
		curwin->w_cursor.ستون = 0;
		pos.ستون = 0;
		اوږدتیا = ml_get_len(pos.lnum);
	    }
	    وإلا // oap->motion_type == MCHAR
	    {
		إذا (pos.lnum == oap->پیل.lnum && !oap->inclusive)
		    dec(&(oap->پای_ټکی));
		اوږدتیا = ml_get_len(pos.lnum);
		pos.ستون = 0;
		إذا (pos.lnum == oap->پیل.lnum)
		{
		    pos.ستون += oap->پیل.ستون;
		    اوږدتیا -= oap->پیل.ستون;
		}
		إذا (pos.lnum == oap->پای_ټکی.lnum)
		{
		    اوږدتیا = ml_get_len(oap->پای_ټکی.lnum);
		    إذا (oap->پای_ټکی.ستون >= اوږدتیا)
			oap->پای_ټکی.ستون = اوږدتیا - 1;
		    اوږدتیا = oap->پای_ټکی.ستون - pos.ستون + 1;
		}
	    }
	    one_change = do_addsub(oap->op_type, &pos, اوږدتیا, amount);
	    إذا (one_change)
	    {
		// Remember the start position of the first change.
		إذا (change_cnt == 0)
		    startpos = curbuf->b_op_پیل;
		++change_cnt;
	    }

#که_تعریف_شوی FEAT_NETBEANS_INTG
	    إذا (netbeans_فعال() && one_change)
	    {
		char_u *ptr;

		netbeans_removed(curbuf, pos.lnum, pos.ستون, (اوږد)اوږدتیا);
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, pos.ستون,
						&ptr[pos.ستون], اوږدتیا);
	    }
#د_که_پای
	    إذا (g_cmd && one_change)
		amount += Prenum1;
	}

#که_تعریف_شوی FEAT_FOLDING
	disable_fold_update--;
#د_که_پای
	إذا (change_cnt)
	    بدل_شوی_کرښې(oap->پیل.lnum, 0, oap->پای_ټکی.lnum + 1, 0L);

	إذا (!change_cnt && oap->is_VIsual)
	    // No change: need to remove the Visual selection
	    redraw_curbuf_later(UPD_INVERTED);

	// Set '[ mark if something changed. Keep the last end
	// position from do_addsub().
	إذا (change_cnt > 0 && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	    curbuf->b_op_پیل = startpos;

	إذا (change_cnt > p_report)
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
    صحيح		اوږدتیا,
    linenr_T	Prenum1)
{
    صحيح		ستون;
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
    صحيح		کمول;
    صحيح		negative = FALSE;
    صحيح		was_مثبت = TRUE;
    صحيح		visual = VIsual_فعال;
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

    إذا (virtual_فعال())
    {
	save_coladd = pos->coladd;
	pos->coladd = 0;
    }

    curwin->w_cursor = *pos;
    ptr = ml_get(pos->lnum);
    linelen = ml_get_len(pos->lnum);
    ستون = pos->ستون;

    إذا (ستون + !!save_coladd >= linelen)
	اذهب theend;

    /*
     * First check if we are on a hexadecimal number, after the "0x".
     */
    إذا (!VIsual_فعال)
    {
	إذا (do_bin)
	    طالما (ستون > 0 && vim_isbdigit(ptr[ستون]))
	    {
		--ستون;
		إذا (has_mbyte)
		    ستون -= (*mb_سر_off)(ptr, ptr + ستون);
	    }

	إذا (do_hex)
	    طالما (ستون > 0 && vim_isxdigit(ptr[ستون]))
	    {
		--ستون;
		إذا (has_mbyte)
		    ستون -= (*mb_سر_off)(ptr, ptr + ستون);
	    }

	إذا (       do_bin
		&& do_hex
		&& ! ((ستون > 0
		    && (ptr[ستون] == 'X'
			|| ptr[ستون] == 'x')
		    && ptr[ستون - 1] == '0'
		    && (!has_mbyte ||
			!(*mb_سر_off)(ptr, ptr + ستون - 1))
		    && vim_isxdigit(ptr[ستون + 1]))))
	{

	    // In case of binary/hexadecimal pattern overlap match, rescan

	    ستون = pos->ستون;

	    طالما (ستون > 0 && vim_isdigit(ptr[ستون]))
	    {
		ستون--;
		إذا (has_mbyte)
		    ستون -= (*mb_سر_off)(ptr, ptr + ستون);
	    }
	}

	إذا ((       do_hex
		&& ستون > 0
		&& (ptr[ستون] == 'X'
		    || ptr[ستون] == 'x')
		&& ptr[ستون - 1] == '0'
		&& (!has_mbyte ||
		    !(*mb_سر_off)(ptr, ptr + ستون - 1))
		&& vim_isxdigit(ptr[ستون + 1])) ||
	    (       do_bin
		&& ستون > 0
		&& (ptr[ستون] == 'B'
		    || ptr[ستون] == 'b')
		&& ptr[ستون - 1] == '0'
		&& (!has_mbyte ||
		    !(*mb_سر_off)(ptr, ptr + ستون - 1))
		&& vim_isbdigit(ptr[ستون + 1])))
	{
	    // Found hexadecimal or binary number, move to its start.
	    --ستون;
	    إذا (has_mbyte)
		ستون -= (*mb_سر_off)(ptr, ptr + ستون);
	}
	وإلا
	{
	    /*
	     * Search forward and then backward to find the start of number.
	     */
	    ستون = pos->ستون;

	    طالما (ptr[ستون] != NUL
		    && !vim_isdigit(ptr[ستون])
		    && !(do_alpha && ASCII_ISALPHA(ptr[ستون])))
		ستون += mb_ptr2len(ptr + ستون);

	    طالما (ستون > 0
		    && vim_isdigit(ptr[ستون - 1])
		    && !(do_alpha && ASCII_ISALPHA(ptr[ستون])))
	    {
		--ستون;
		إذا (has_mbyte)
		    ستون -= (*mb_سر_off)(ptr, ptr + ستون);
	    }
	}
    }

    إذا (visual)
    {
	طالما (ptr[ستون] != NUL && اوږدتیا > 0
		&& !vim_isdigit(ptr[ستون])
		&& !(do_alpha && ASCII_ISALPHA(ptr[ستون])))
	{
	    صحيح mb_len = mb_ptr2len(ptr + ستون);

	    ستون += mb_len;
	    اوږدتیا -= mb_len;
	}

	إذا (اوږدتیا == 0)
	    اذهب theend;

	إذا (ستون > pos->ستون && ptr[ستون - 1] == '-'
		&& (!has_mbyte || !(*mb_سر_off)(ptr, ptr + ستون - 1))
		&& !do_unsigned)
	{
	    إذا (do_blank && ستون >= 2 && !VIM_ISWHITE(ptr[ستون - 2]))
		blank_unsigned = TRUE;
	    وإلا
	    {
		negative = TRUE;
		was_مثبت = FALSE;
	    }
	}
    }

    /*
     * If a number was found, and saving for undo works, replace the number.
     */
    firstdigit = ptr[ستون];
    إذا (!VIM_ISDIGIT(firstdigit) && !(do_alpha && ASCII_ISALPHA(firstdigit)))
    {
	beep_flush();
	اذهب theend;
    }

    إذا (do_alpha && ASCII_ISALPHA(firstdigit))
    {
	// decrement or increment alphabetic character
	إذا (op_type == OP_NR_SUB)
	{
	    إذا (CharOrd(firstdigit) < Prenum1)
	    {
		إذا (خوندي_isupper(firstdigit))
		    firstdigit = 'A';
		وإلا
		    firstdigit = 'a';
	    }
	    وإلا
		firstdigit -= Prenum1;
	}
	وإلا
	{
	    إذا (26 - CharOrd(firstdigit) - 1 < Prenum1)
	    {
		إذا (خوندي_isupper(firstdigit))
		    firstdigit = 'Z';
		وإلا
		    firstdigit = 'z';
	    }
	    وإلا
		firstdigit += Prenum1;
	}
	curwin->w_cursor.ستون = ستون;
	إذا (!did_change)
	    startpos = curwin->w_cursor;
	did_change = TRUE;
	(فراغ)del_char(FALSE);
	ins_char(firstdigit);
	endpos = curwin->w_cursor;
	curwin->w_cursor.ستون = ستون;
    }
    وإلا
    {
	char_u	*buf1;
	صحيح	buf1len;
	char_u	buf2[NUMBUFLEN];
	صحيح	buf2len;
	pos_T	save_pos;
	صحيح	i;

	إذا (ستون > 0 && ptr[ستون - 1] == '-'
		&& (!has_mbyte ||
		    !(*mb_سر_off)(ptr, ptr + ستون - 1))
		&& !visual
		&& !do_unsigned)
	{
	    إذا (do_blank && ستون >= 2 && !VIM_ISWHITE(ptr[ستون - 2]))
		blank_unsigned = TRUE;
	    وإلا
	    {
		// negative number
		--ستون;
		negative = TRUE;
	    }
	}

	// get the number value (unsigned)
	إذا (visual && VIsual_ډېرپېښ != 'V')
	    maxlen = (curbuf->b_visual.vi_curswant == MAXCOL
		    ? linelen - ستون : اوږدتیا);

	صحيح ډکېدنه = FALSE;
	vim_str2nr(ptr + ستون, &pre, &اوږدتیا,
		0 + (do_bin ? STR2NR_BIN : 0)
		    + (do_oct ? STR2NR_OCT : 0)
		    + (do_hex ? STR2NR_HEX : 0),
		عدم, &n, maxlen, FALSE, &ډکېدنه);

	// ignore leading '-' for hex and octal and bin numbers
	إذا (pre && negative)
	{
	    ++ستون;
	    --اوږدتیا;
	    negative = FALSE;
	}
	// add or subtract
	کمول = FALSE;
	إذا (op_type == OP_NR_SUB)
	    کمول ^= TRUE;
	إذا (negative)
	    کمول ^= TRUE;

	oldn = n;
	إذا (!ډکېدنه)  // if number is too big don't add/subtract
	{
	    إذا (کمول)
		n -= (uvarnumber_T)Prenum1;
	    وإلا
		n += (uvarnumber_T)Prenum1;
	}

	// handle wraparound for decimal numbers
	إذا (!pre)
	{
	    إذا (کمول)
	    {
		إذا (n > oldn)
		{
		    n = 1 + (n ^ (uvarnumber_T)-1);
		    negative ^= TRUE;
		}
	    }
	    وإلا
	    {
		//جمع_کړه
		إذا (n < oldn)
		{
		    n = (n ^ (uvarnumber_T)-1);
		    negative ^= TRUE;
		}
	    }
	    إذا (n == 0)
		negative = FALSE;
	}

	إذا ((do_unsigned || blank_unsigned) && negative)
	{
	    إذا (کمول)
		// sticking at zero.
		n = (uvarnumber_T)0;
	    وإلا
		// sticking at 2^64 - 1.
		n = (uvarnumber_T)(-1);
	    negative = FALSE;
	}

	إذا (visual && !was_مثبت && !negative && ستون > 0)
	{
	    // need to remove the '-'
	    ستون--;
	    اوږدتیا++;
	}

	/*
	 * Delete the old number.
	 */
	curwin->w_cursor.ستون = ستون;
	إذا (!did_change)
	    startpos = curwin->w_cursor;
	did_change = TRUE;
	todel = اوږدتیا;
	c = gchar_cursor();
	/*
	 * Don't include the '-' in the length, only the length of the
	 * part after it is kept the same.
	 */
	إذا (c == '-')
	    --اوږدتیا;

	save_pos = curwin->w_cursor;
	لكل (i = 0; i < todel; ++i)
	{
	    إذا (c < 0x100 && خوندي_isalpha(c))
	    {
		إذا (خوندي_isupper(c))
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
	buf1 = alloc(اوږدتیا + NUMBUFLEN);
	إذا (buf1 == عدم)
	    اذهب theend;
	ptr = buf1;
	إذا (negative && (!visual || was_مثبت))
	    *ptr++ = '-';
	إذا (pre)
	{
	    *ptr++ = '0';
	    --اوږدتیا;
	}
	إذا (pre == 'b' || pre == 'B' ||
	    pre == 'x' || pre == 'X')
	{
	    *ptr++ = pre;
	    --اوږدتیا;
	}

	/*
	 * Put the number characters in buf2[].
	 */
	إذا (pre == 'b' || pre == 'B')
	{
	    صحيح بټ = 0;
	    صحيح بټونه = حجم(uvarnumber_T) * 8;

	    // leading zeros
	    لكل (بټ = بټونه; بټ > 0; بټ--)
		إذا ((n >> (بټ - 1)) & 0x1) اكسر;

	    لكل (buf2len = 0; بټ > 0 && buf2len < (NUMBUFLEN - 1); بټ--)
		buf2[buf2len++] = ((n >> (بټ - 1)) & 0x1) ? '1' : '0';

	    buf2[buf2len] = NUL;
	}
	وإلا إذا (pre == 0)
	    buf2len = vim_snprintf((حرف *)buf2, NUMBUFLEN, "%llu", n);
	وإلا إذا (pre == '0')
	    buf2len = vim_snprintf((حرف *)buf2, NUMBUFLEN, "%llo", n);
	وإلا إذا (pre && hexupper)
	    buf2len = vim_snprintf((حرف *)buf2, NUMBUFLEN, "%llX", n);
	وإلا
	    buf2len = vim_snprintf((حرف *)buf2, NUMBUFLEN, "%llx", n);
	اوږدتیا -= buf2len;

	/*
	 * Adjust number of zeros to the new number of digits, so the
	 * total length of the number remains the same.
	 * Don't do this when
	 * the result may look like an octal number.
	 */
	إذا (firstdigit == '0' && !(do_oct && pre == 0))
	    طالما (اوږدتیا-- > 0)
		*ptr++ = '0';
	*ptr = NUL;
	buf1len = (صحيح)(ptr - buf1);

	STRCPY(buf1 + buf1len, buf2);
	buf1len += buf2len;

	// Insert just after the first character to be removed, so that any
	// text properties will be adjusted.  Then delete the old number
	// afterwards.
	save_pos = curwin->w_cursor;
	إذا (todel > 0)
	    inc_cursor();
	ins_str(buf1, (size_t)buf1len);		// insert the new number
	vim_free(buf1);

	// del_char() will also mark line needing displaying
	إذا (todel > 0)
	{
	    صحيح بایټونه_after = ml_get_curline_len() - curwin->w_cursor.ستون;

	    // Delete the one character before the insert.
	    curwin->w_cursor = save_pos;
	    (فراغ)del_char(FALSE);
	    curwin->w_cursor.ستون = ml_get_curline_len() - بایټونه_after;
	    --todel;
	}
	طالما (todel-- > 0)
	    (فراغ)del_char(FALSE);

	endpos = curwin->w_cursor;
	إذا (did_change && curwin->w_cursor.ستون)
	    --curwin->w_cursor.ستون;
    }

    إذا (did_change && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// set the '[ and '] marks
	curbuf->b_op_پیل = startpos;
	curbuf->b_op_پای_ټکی = endpos;
	إذا (curbuf->b_op_پای_ټکی.ستون > 0)
	    --curbuf->b_op_پای_ټکی.ستون;
    }

theend:
    إذا (visual)
	curwin->w_cursor = save_cursor;
    وإلا إذا (did_change)
	curwin->w_سېټ_curswant = true;
    وإلا إذا (virtual_فعال())
	curwin->w_cursor.coladd = save_coladd;

    أرجع did_change;
}

    فراغ
clear_oparg(oparg_T *oap)
{
    CLEAR_اشاره_ګر(oap);
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
کرښه_شمېر_info(
    char_u	*کرښه,
    varnumber_T	*wc,
    varnumber_T	*cc,
    varnumber_T	حد,
    صحيح		eol_اندازه)
{
    varnumber_T	i;
    varnumber_T	ټکي = 0;
    varnumber_T	chars = 0;
    صحيح		is_کلمه = 0;

    لكل (i = 0; i < حد && کرښه[i] != NUL; )
    {
	إذا (is_کلمه)
	{
	    إذا (vim_isspace(کرښه[i]))
	    {
		ټکي++;
		is_کلمه = 0;
	    }
	}
	وإلا إذا (!vim_isspace(کرښه[i]))
	    is_کلمه = 1;
	++chars;
	i += (*mb_ptr2len)(کرښه + i);
    }

    إذا (is_کلمه)
	ټکي++;
    *wc += ټکي;

    // Add eol_size if the end of line was reached before hitting limit.
    إذا (i < حد && کرښه[i] == NUL)
    {
	i += eol_اندازه;
	chars += eol_اندازه;
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
    varnumber_T	byte_شمېر = 0;
    varnumber_T	bom_شمېر  = 0;
    varnumber_T	byte_شمېر_cursor = 0;
    varnumber_T	char_شمېر = 0;
    varnumber_T	char_شمېر_cursor = 0;
    varnumber_T	کلمه_شمېر = 0;
    varnumber_T	کلمه_شمېر_cursor = 0;
    صحيح		eol_اندازه;
    varnumber_T	وروستی_check = 100000L;
    اوږد	کرښه_شمېر_selected = 0;
    pos_T	min_pos, max_pos;
    oparg_T	oparg;
    بنية بلاک_def	bd;

    /*
     * Compute the length of the file in characters.
     */
    إذا (curbuf->b_ml.ml_flags & ML_تشه)
    {
	إذا (dict == عدم)
	{
	    پیغام(_(no_کرښې_پیغام));
	    أرجع;
	}
    }
    وإلا
    {
	إذا (get_fileformat(curbuf) == EOL_DOS)
	    eol_اندازه = 2;
	وإلا
	    eol_اندازه = 1;

	إذا (VIsual_فعال)
	{
	    إذا (LT_POS(VIsual, curwin->w_cursor))
	    {
		min_pos = VIsual;
		max_pos = curwin->w_cursor;
	    }
	    وإلا
	    {
		min_pos = curwin->w_cursor;
		max_pos = VIsual;
	    }
	    إذا (*p_sel == 'e' && max_pos.ستون > 0)
		--max_pos.ستون;

	    إذا (VIsual_ډېرپېښ == Ctrl_V)
	    {
#که_تعریف_شوی FEAT_LINEBREAK
		char_u * saved_sbr = p_sbr;
		char_u * saved_w_sbr = curwin->w_p_sbr;

		// Make 'sbr' empty for a moment to get the correct size.
		p_sbr = تشه_option;
		curwin->w_p_sbr = تشه_option;
#د_که_پای
		oparg.is_VIsual = 1;
		oparg.بلاک_ډېرپېښ = TRUE;
		oparg.op_type = OP_NOP;
		getvcols(curwin, &min_pos, &max_pos,
					&oparg.پیل_vcol, &oparg.پای_ټکی_vcol, 0);
#که_تعریف_شوی FEAT_LINEBREAK
		p_sbr = saved_sbr;
		curwin->w_p_sbr = saved_w_sbr;
#د_که_پای
		إذا (curwin->w_curswant == MAXCOL)
		    oparg.پای_ټکی_vcol = MAXCOL;
		// Swap the start, end vcol if needed
		إذا (oparg.پای_ټکی_vcol < oparg.پیل_vcol)
		{
		    oparg.پای_ټکی_vcol += oparg.پیل_vcol;
		    oparg.پیل_vcol = oparg.پای_ټکی_vcol - oparg.پیل_vcol;
		    oparg.پای_ټکی_vcol -= oparg.پیل_vcol;
		}
	    }
	    کرښه_شمېر_selected = max_pos.lnum - min_pos.lnum + 1;
	}

	لكل (lnum = 1; lnum <= curbuf->b_ml.ml_کرښه_شمېر; ++lnum)
	{
	    // Check for a CTRL-C every 100000 characters.
	    إذا (byte_شمېر > وروستی_check)
	    {
		ui_breakcheck();
		إذا (got_int)
		    أرجع;
		وروستی_check = byte_شمېر + 100000L;
	    }

	    // Do extra processing for VIsual mode.
	    إذا (VIsual_فعال
		    && lnum >= min_pos.lnum && lnum <= max_pos.lnum)
	    {
		char_u	    *s = عدم;
		اوږد	    len = 0L;

		بدل (VIsual_ډېرپېښ)
		{
		    حالة Ctrl_V:
			virtual_op = virtual_فعال();
			بلاک_prep(&oparg, &bd, lnum, 0);
			virtual_op = MAYBE;
			s = bd.textstart;
			len = (اوږد)bd.textlen;
			اكسر;
		    حالة 'V':
			s = ml_get(lnum);
			len = MAXCOL;
			اكسر;
		    حالة 'v':
			{
			    colnr_T پیل_ستون = (lnum == min_pos.lnum)
							   ? min_pos.ستون : 0;
			    colnr_T پای_ټکی_ستون = (lnum == max_pos.lnum)
				      ? max_pos.ستون - پیل_ستون + 1 : MAXCOL;

			    s = ml_get(lnum) + پیل_ستون;
			    len = پای_ټکی_ستون;
			}
			اكسر;
		}
		إذا (s != عدم)
		{
		    byte_شمېر_cursor += کرښه_شمېر_info(s, &کلمه_شمېر_cursor,
					   &char_شمېر_cursor, len, eol_اندازه);
		    إذا (lnum == curbuf->b_ml.ml_کرښه_شمېر
			    && !curbuf->b_p_eol
			    && (curbuf->b_p_bin || !curbuf->b_p_fixeol)
			    && (اوږد)STRLEN(s) < len)
			byte_شمېر_cursor -= eol_اندازه;
		}
	    }
	    وإلا
	    {
		// In non-visual mode, check for the line the cursor is on
		إذا (lnum == curwin->w_cursor.lnum)
		{
		    کلمه_شمېر_cursor += کلمه_شمېر;
		    char_شمېر_cursor += char_شمېر;
		    byte_شمېر_cursor = byte_شمېر +
			کرښه_شمېر_info(ml_get(lnum),
				&کلمه_شمېر_cursor, &char_شمېر_cursor,
				(varnumber_T)(curwin->w_cursor.ستون + 1),
				eol_اندازه);
		}
	    }
	    // Add to the running totals
	    byte_شمېر += کرښه_شمېر_info(ml_get(lnum), &کلمه_شمېر,
					 &char_شمېر, (varnumber_T)MAXCOL,
					 eol_اندازه);
	}

	// Correction for when last line doesn't have an EOL.
	إذا (!curbuf->b_p_eol && (curbuf->b_p_bin || !curbuf->b_p_fixeol))
	    byte_شمېر -= eol_اندازه;

	إذا (dict == عدم)
	{
	    إذا (VIsual_فعال)
	    {
		إذا (VIsual_ډېرپېښ == Ctrl_V && curwin->w_curswant < MAXCOL)
		{
		    getvcols(curwin, &min_pos, &max_pos,
						&min_pos.ستون, &max_pos.ستون, 0);
		    vim_snprintf((حرف *)buf1, حجم(buf1), _("%ld Cols; "),
			    (اوږد)(oparg.پای_ټکی_vcol - oparg.پیل_vcol + 1));
		}
		وإلا
		    buf1[0] = NUL;

		إذا (char_شمېر_cursor == byte_شمېر_cursor
						    && char_شمېر == byte_شمېر)
		    vim_snprintf((حرف *)IObuff, IOSIZE,
			    _("Selected %s%ld of %ld Lines; %lld of %lld Words; %lld of %lld Bytes"),
			    buf1, کرښه_شمېر_selected,
			    (اوږد)curbuf->b_ml.ml_کرښه_شمېر,
			    کلمه_شمېر_cursor,
			    کلمه_شمېر,
			    byte_شمېر_cursor,
			    byte_شمېر);
		وإلا
		    vim_snprintf((حرف *)IObuff, IOSIZE,
			    _("Selected %s%ld of %ld Lines; %lld of %lld Words; %lld of %lld Chars; %lld of %lld Bytes"),
			    buf1, کرښه_شمېر_selected,
			    (اوږد)curbuf->b_ml.ml_کرښه_شمېر,
			    کلمه_شمېر_cursor,
			    کلمه_شمېر,
			    char_شمېر_cursor,
			    char_شمېر,
			    byte_شمېر_cursor,
			    byte_شمېر);
	    }
	    وإلا
	    {
		p = ml_get_curline();
		validate_virtcol();
		ستون_print(buf1, حجم(buf1), (صحيح)curwin->w_cursor.ستون + 1,
			(صحيح)curwin->w_virtcol + 1);
		ستون_print(buf2, حجم(buf2), ml_get_curline_len(),
							   linetabsize_str(p));

		إذا (char_شمېر_cursor == byte_شمېر_cursor
			&& char_شمېر == byte_شمېر)
		    vim_snprintf((حرف *)IObuff, IOSIZE,
			_("Col %s of %s; Line %ld of %ld; Word %lld of %lld; Byte %lld of %lld"),
			(حرف *)buf1, (حرف *)buf2,
			(اوږد)curwin->w_cursor.lnum,
			(اوږد)curbuf->b_ml.ml_کرښه_شمېر,
			کلمه_شمېر_cursor, کلمه_شمېر,
			byte_شمېر_cursor, byte_شمېر);
		وإلا
		    vim_snprintf((حرف *)IObuff, IOSIZE,
			_("Col %s of %s; Line %ld of %ld; Word %lld of %lld; Char %lld of %lld; Byte %lld of %lld"),
			(حرف *)buf1, (حرف *)buf2,
			(اوږد)curwin->w_cursor.lnum,
			(اوږد)curbuf->b_ml.ml_کرښه_شمېر,
			کلمه_شمېر_cursor, کلمه_شمېر,
			char_شمېر_cursor, char_شمېر,
			byte_شمېر_cursor, byte_شمېر);
	    }
	}

	bom_شمېر = bomb_اندازه();
	إذا (dict == عدم && bom_شمېر > 0)
	{
	    size_t len = STRLEN(IObuff);

	    vim_snprintf((حرف *)IObuff + len, IOSIZE - len,
				 _("(+%lld for BOM)"), bom_شمېر);
	}
	إذا (dict == عدم)
	{
	    // Don't shorten this message, the user asked for it.
	    p = p_shm;
	    p_shm = (char_u *)"";
	    پیغام((حرف *)IObuff);
	    p_shm = p;
	}
    }
#إذا defined(FEAT_EVAL)
    إذا (dict != عدم)
    {
	dict_جمع_کړه_شمېره(dict, "words", کلمه_شمېر);
	dict_جمع_کړه_شمېره(dict, "chars", char_شمېر);
	dict_جمع_کړه_شمېره(dict, "bytes", byte_شمېر + bom_شمېر);
	dict_جمع_کړه_شمېره(dict, VIsual_فعال ? "visual_bytes" : "cursor_bytes",
		byte_شمېر_cursor);
	dict_جمع_کړه_شمېره(dict, VIsual_فعال ? "visual_chars" : "cursor_chars",
		char_شمېر_cursor);
	dict_جمع_کړه_شمېره(dict, VIsual_فعال ? "visual_words" : "cursor_words",
		کلمه_شمېر_cursor);
    }
#د_که_پای
}

/*
 * Handle indent and format operators and visual mode ":".
 */
    ساكن فراغ
op_colon(oparg_T *oap)
{
    stuffcharReadbuff(':');
    إذا (oap->is_VIsual)
	stuffReadbuff((char_u *)"'<,'>");
    وإلا
    {
	// Make the range look nice, so it can be repeated.
	إذا (oap->پیل.lnum == curwin->w_cursor.lnum)
	    stuffcharReadbuff('.');
	وإلا
	    stuffnumReadbuff((اوږد)oap->پیل.lnum);

#که_تعریف_شوی FEAT_FOLDING
	// When using !! on a closed fold the range ".!" works best to operate
	// on, it will be made the whole closed fold later.
	linenr_T پای_ټکی_د_پیل_Fold = oap->پیل.lnum;
	(فراغ)hasFolding(oap->پیل.lnum, عدم, &پای_ټکی_د_پیل_Fold);
#د_که_پای
	إذا (oap->پای_ټکی.lnum != oap->پیل.lnum
#که_تعریف_شوی FEAT_FOLDING
		&& oap->پای_ټکی.lnum != پای_ټکی_د_پیل_Fold
#د_که_پای
	   )
	{
	    // Make it a range with the end line.
	    stuffcharReadbuff(',');
	    إذا (oap->پای_ټکی.lnum == curwin->w_cursor.lnum)
		stuffcharReadbuff('.');
	    وإلا إذا (oap->پای_ټکی.lnum == curbuf->b_ml.ml_کرښه_شمېر)
		stuffcharReadbuff('$');
	    وإلا إذا (oap->پیل.lnum == curwin->w_cursor.lnum
#که_تعریف_شوی FEAT_FOLDING
		    // do not use ".+number" for a closed fold, it would count
		    // folded lines twice
		    && !hasFolding(oap->پای_ټکی.lnum, عدم, عدم)
#د_که_پای
		    )
	    {
		stuffReadbuff((char_u *)".+");
		stuffnumReadbuff((اوږد)oap->کرښه_شمېر - 1);
	    }
	    وإلا
		stuffnumReadbuff((اوږد)oap->پای_ټکی.lnum);
	}
    }
    إذا (oap->op_type != OP_COLON)
	stuffReadbuff((char_u *)"!");
    إذا (oap->op_type == OP_INDENT)
    {
	إذا (*get_equalprg() == NUL)
	    stuffReadbuff((char_u *)"indent");
	وإلا
	    stuffReadbuff(get_equalprg());
	stuffReadbuff((char_u *)"\n");
    }
    وإلا إذا (oap->op_type == OP_FORMAT)
    {
	إذا (*curbuf->b_p_fp != NUL)
	    stuffReadbuff(curbuf->b_p_fp);
	وإلا إذا (*p_fp != NUL)
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
did_سېټ_operatorfunc(optset_T *دلیلونه ناکارول_شوی)
{
    إذا (option_سېټ_callback_func(p_opfunc, &opfunc_cb) == FAIL)
	أرجع e_invalid_دلیل;

    أرجع عدم;
}

#إذا defined(EXITFREE)
    فراغ
free_operatorfunc_option(فراغ)
{
# ifdef FEAT_EVAL
    free_callback(&opfunc_cb);
# endif
}
#د_که_پای

#إذا defined(FEAT_EVAL)
/*
 * Mark the global 'operatorfunc' callback with "copyID" so that it is not
 * garbage collected.
 */
    صحيح
سېټ_ref_دننه_opfunc(صحيح copyID ناکارول_شوی)
{
    صحيح abort = FALSE;

    abort = سېټ_ref_دننه_callback(&opfunc_cb, copyID);

    أرجع abort;
}
#د_که_پای

/*
 * Handle the "g@" operator: call 'operatorfunc'.
 */
    ساكن فراغ
op_function(oparg_T *oap ناکارول_شوی)
{
#که_تعریف_شوی FEAT_EVAL
    typval_T	argv[2];
    pos_T	orig_پیل = curbuf->b_op_پیل;
    pos_T	orig_پای_ټکی = curbuf->b_op_پای_ټکی;
    typval_T	rettv;

    إذا (*p_opfunc == NUL)
	emsg(_(e_operatorfunc_is_تشه));
    وإلا
    {
	// Set '[ and '] marks to text to be operated on.
	curbuf->b_op_پیل = oap->پیل;
	curbuf->b_op_پای_ټکی = oap->پای_ټکی;
	إذا (oap->motion_type != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_پای_ټکی);

	argv[0].v_type = VAR_STRING;
	إذا (oap->بلاک_ډېرپېښ)
	    argv[0].vval.v_string = (char_u *)"block";
	وإلا إذا (oap->motion_type == MLINE)
	    argv[0].vval.v_string = (char_u *)"line";
	وإلا
	    argv[0].vval.v_string = (char_u *)"char";
	argv[1].v_type = VAR_UNKNOWN;

	// Reset virtual_op so that 'virtualedit' can be changed in the
	// function.
	صحيح save_virtual_op = virtual_op;
	virtual_op = MAYBE;

	// Reset finish_op so that mode() returns the right value.
	صحيح save_پای_op = پای_op;
	پای_op = FALSE;

	إذا (call_callback(&opfunc_cb, 0, &rettv, 1, argv) != FAIL)
	    clear_tv(&rettv);

	virtual_op = save_virtual_op;
	پای_op = save_پای_op;
	إذا (cmdmod.cmod_flags & CMOD_LOCKMARKS)
	{
	    curbuf->b_op_پیل = orig_پیل;
	    curbuf->b_op_پای_ټکی = orig_پای_ټکی;
	}
    }
#وإلا
    emsg(_(e_eval_feature_not_شته));
#د_که_پای
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
    colnr_T	    پیل, پای_ټکی;

    إذا (VIsual_ډېرپېښ != Ctrl_V
	    || (!initial && oap->پای_ټکی.ستون < curwin->w_پلنوالی))
	أرجع;

    oap->بلاک_ډېرپېښ = TRUE;

    // prevent from moving onto a trail byte
    إذا (has_mbyte)
	mb_adjustpos(curwin->w_بفر, &oap->پای_ټکی);

    getvvcol(curwin, &(oap->پیل), &oap->پیل_vcol, عدم, &oap->پای_ټکی_vcol, 0);

    إذا (!redo_VIsual_busy)
    {
	getvvcol(curwin, &(oap->پای_ټکی), &پیل, عدم, &پای_ټکی, 0);

	إذا (پیل < oap->پیل_vcol)
	    oap->پیل_vcol = پیل;
	إذا (پای_ټکی > oap->پای_ټکی_vcol)
	{
	    إذا (initial && *p_sel == 'e' && پیل >= 1
				    && پیل - 1 >= oap->پای_ټکی_vcol)
		oap->پای_ټکی_vcol = پیل - 1;
	    وإلا
		oap->پای_ټکی_vcol = پای_ټکی;
	}
    }

    // if '$' was used, get oap->end_vcol from longest line
    إذا (curwin->w_curswant == MAXCOL)
    {
	curwin->w_cursor.ستون = MAXCOL;
	oap->پای_ټکی_vcol = 0;
	لكل (curwin->w_cursor.lnum = oap->پیل.lnum;
		curwin->w_cursor.lnum <= oap->پای_ټکی.lnum;
					++curwin->w_cursor.lnum)
	{
	    getvvcol(curwin, &curwin->w_cursor, عدم, عدم, &پای_ټکی, 0);
	    إذا (پای_ټکی > oap->پای_ټکی_vcol)
		oap->پای_ټکی_vcol = پای_ټکی;
	}
    }
    وإلا إذا (redo_VIsual_busy)
	oap->پای_ټکی_vcol = oap->پیل_vcol + redo_VIsual_vcol - 1;
    // Correct oap->end.col and oap->start.col to be the
    // upper-left and lower-right corner of the block area.
    //
    // (Actually, this does convert column positions into character
    // positions)
    curwin->w_cursor.lnum = oap->پای_ټکی.lnum;
    coladvance(oap->پای_ټکی_vcol);
    oap->پای_ټکی = curwin->w_cursor;

    curwin->w_cursor = oap->پیل;
    coladvance(oap->پیل_vcol);
    oap->پیل = curwin->w_cursor;
}

// Information for redoing the previous Visual selection.
تعريفنوع بنية {
    صحيح		rv_ډېرپېښ;	// 'v', 'V', or Ctrl-V
    linenr_T	rv_کرښه_شمېر;	// number of lines
    colnr_T	rv_vcol;	// number of cols or end column
    اوږد	rv_شمېر;	// count for Visual operator
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
do_pending_operator(cmdarg_T *cap, صحيح old_ستون, صحيح gui_yank)
{
    oparg_T	*oap = cap->oap;
    pos_T	old_cursor;
    صحيح		تشه_region_error;
    صحيح		restart_edit_save;
#که_تعریف_شوی FEAT_LINEBREAK
    صحيح		lbr_saved = curwin->w_p_lbr;
#د_که_پای

    // The visual area is remembered for redo
    ساكن redo_VIsual_T   redo_VIsual = {NUL, 0, 0, 0,0};

    صحيح		    include_کرښه_break = FALSE;

#إذا defined(FEAT_CLIPBOARD)
    // Yank the visual area into the GUI selection register before we operate
    // on it and lose it forever.
    // Don't do it if a specific register was specified, so that ""x"*P works.
    // This could call do_pending_operator() recursively, but that's OK
    // because gui_yank will be TRUE for the nested call.
    إذا ((clip_ستوری.شته || clip_plus.شته)
	    && oap->op_type != OP_NOP
	    && !gui_yank
	    && VIsual_فعال
	    && !redo_VIsual_busy
	    && oap->regname == 0)
	clip_auto_select();
#د_که_پای
    old_cursor = curwin->w_cursor;

    // If an operation is pending, handle it...
    إذا ((پای_op || VIsual_فعال) && oap->op_type != OP_NOP)
    {
	// Yank can be redone when 'y' is in 'cpoptions', but not when yanking
	// for the clipboard.
	صحيح	redo_yank = vim_strchr(p_cpo, CPO_YANK) != عدم && !gui_yank;

#که_تعریف_شوی FEAT_LINEBREAK
	// Avoid a problem with unwanted linebreaks in block mode.
	(فراغ)reset_lbr();
#د_که_پای
	oap->is_VIsual = VIsual_فعال;
	إذا (oap->motion_force == 'V')
	    oap->motion_type = MLINE;
	وإلا إذا (oap->motion_force == 'v')
	{
	    // If the motion was linewise, "inclusive" will not have been set.
	    // Use "exclusive" to be consistent.  Makes "dvj" work nice.
	    إذا (oap->motion_type == MLINE)
		oap->inclusive = FALSE;
	    // If the motion already was characterwise, toggle "inclusive"
	    وإلا إذا (oap->motion_type == MCHAR)
		oap->inclusive = !oap->inclusive;
	    oap->motion_type = MCHAR;
	}
	وإلا إذا (oap->motion_force == Ctrl_V)
	{
	    // Change line- or characterwise motion into Visual block mode.
	    إذا (!VIsual_فعال)
	    {
		VIsual_فعال = TRUE;
		VIsual = oap->پیل;
	    }
	    VIsual_ډېرپېښ = Ctrl_V;
	    VIsual_select = FALSE;
	    VIsual_reselect = FALSE;
	}

	// Only redo yank when 'y' flag is in 'cpoptions'.
	// Never redo "zf" (define fold).
	إذا ((redo_yank || oap->op_type != OP_YANK)
		&& ((!VIsual_فعال || oap->motion_force)
		    // Also redo Operator-pending Visual mode mappings
		    || (VIsual_فعال
			    && is_ex_cmdchar(cap) && oap->op_type != OP_COLON))
		&& cap->cmdchar != 'D'
#که_تعریف_شوی FEAT_FOLDING
		&& oap->op_type != OP_FOLD
		&& oap->op_type != OP_FOLDOPEN
		&& oap->op_type != OP_FOLDOPENREC
		&& oap->op_type != OP_FOLDCLOSE
		&& oap->op_type != OP_FOLDCLOSEREC
		&& oap->op_type != OP_FOLDDEL
		&& oap->op_type != OP_FOLDDELREC
#د_که_پای
		)
	{
	    prep_redo(oap->regname, cap->count0,
		    get_op_char(oap->op_type), get_extra_op_char(oap->op_type),
		    oap->motion_force, cap->cmdchar, cap->nchar);
	    إذا (cap->cmdchar == '/' || cap->cmdchar == '?') // was a search
	    {
		// If 'cpoptions' does not contain 'r', insert the search
		// pattern to really repeat the same command.
		إذا (vim_strchr(p_cpo, CPO_REDO) == عدم)
		    Append_منزل_Redobuff_Lit(cap->searchbuf, -1);
		Append_منزل_Redobuff(NL_STR);
	    }
	    وإلا إذا (is_ex_cmdchar(cap))
	    {
		// do_cmdline() has stored the first typed line in
		// "repeat_cmdline".  When several lines are typed repeating
		// won't be possible.
		إذا (repeat_cmdline == عدم)
		    ResetRedobuff();
		وإلا
		{
		    إذا (cap->cmdchar == ':')
			Append_منزل_Redobuff_Lit(repeat_cmdline, -1);
		    وإلا
			Append_منزل_Redobuff_Spec(repeat_cmdline);
		    Append_منزل_Redobuff(NL_STR);
		    VIM_CLEAR(repeat_cmdline);
		}
	    }
	}

	إذا (redo_VIsual_busy)
	{
	    // Redo of an operation on a Visual area. Use the same size from
	    // redo_VIsual.rv_line_count and redo_VIsual.rv_vcol.
	    oap->پیل = curwin->w_cursor;
	    curwin->w_cursor.lnum += redo_VIsual.rv_کرښه_شمېر - 1;
	    إذا (curwin->w_cursor.lnum > curbuf->b_ml.ml_کرښه_شمېر)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_کرښه_شمېر;
	    VIsual_ډېرپېښ = redo_VIsual.rv_ډېرپېښ;
	    إذا (redo_VIsual.rv_vcol == MAXCOL || VIsual_ډېرپېښ == 'v')
	    {
		إذا (VIsual_ډېرپېښ == 'v')
		{
		    إذا (redo_VIsual.rv_کرښه_شمېر <= 1)
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
	    cap->count0 = redo_VIsual.rv_شمېر;
	    إذا (redo_VIsual.rv_شمېر != 0)
		cap->count1 = redo_VIsual.rv_شمېر;
	    وإلا
		cap->count1 = 1;
	}
	وإلا إذا (VIsual_فعال)
	{
	    إذا (!gui_yank)
	    {
		// Save the current VIsual area for '< and '> marks, and "gv"
		curbuf->b_visual.vi_پیل = VIsual;
		curbuf->b_visual.vi_پای_ټکی = curwin->w_cursor;
		curbuf->b_visual.vi_ډېرپېښ = VIsual_ډېرپېښ;
		restore_visual_ډېرپېښ();
		curbuf->b_visual.vi_curswant = curwin->w_curswant;
#که_تعریف_شوی FEAT_EVAL
		curbuf->b_visual_ډېرپېښ_eval = VIsual_ډېرپېښ;
#د_که_پای
	    }

	    // In Select mode, a linewise selection is operated upon like a
	    // characterwise selection.
	    // Special case: gH<Del> deletes the last line.
	    إذا (VIsual_select && VIsual_ډېرپېښ == 'V'
					    && cap->oap->op_type != OP_DELETE)
	    {
		إذا (LT_POS(VIsual, curwin->w_cursor))
		{
		    VIsual.ستون = 0;
		    curwin->w_cursor.ستون = ml_get_len(curwin->w_cursor.lnum);
		}
		وإلا
		{
		    curwin->w_cursor.ستون = 0;
		    VIsual.ستون = ml_get_len(VIsual.lnum);
		}
		VIsual_ډېرپېښ = 'v';
	    }
	    // If 'selection' is "exclusive", backup one character for
	    // charwise selections.
	    وإلا إذا (VIsual_ډېرپېښ == 'v')
		include_کرښه_break = unadjust_for_sel();

	    oap->پیل = VIsual;
	    إذا (VIsual_ډېرپېښ == 'V')
	    {
		oap->پیل.ستون = 0;
		oap->پیل.coladd = 0;
	    }
	}

	// Set oap->start to the first position of the operated text, oap->end
	// to the end of the operated text.  w_cursor is equal to oap->start.
	إذا (LT_POS(oap->پیل, curwin->w_cursor))
	{
#که_تعریف_شوی FEAT_FOLDING
	    // Include folded lines completely.
	    إذا (!VIsual_فعال)
	    {
		إذا (hasFolding(oap->پیل.lnum, &oap->پیل.lnum, عدم))
		    oap->پیل.ستون = 0;
		إذا ((curwin->w_cursor.ستون > 0 || oap->inclusive
						  || oap->motion_type == MLINE)
			&& hasFolding(curwin->w_cursor.lnum, عدم,
						      &curwin->w_cursor.lnum))
		    curwin->w_cursor.ستون = ml_get_curline_len();
	    }
#د_که_پای
	    oap->پای_ټکی = curwin->w_cursor;
	    curwin->w_cursor = oap->پیل;

	    // w_virtcol may have been updated; if the cursor goes back to its
	    // previous position w_virtcol becomes invalid and isn't updated
	    // automatically.
	    curwin->w_valid &= ~VALID_VIRTCOL;
	}
	وإلا
	{
#که_تعریف_شوی FEAT_FOLDING
	    // Include folded lines completely.
	    إذا (!VIsual_فعال && oap->motion_type == MLINE)
	    {
		إذا (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum,
									عدم))
		    curwin->w_cursor.ستون = 0;
		إذا (hasFolding(oap->پیل.lnum, عدم, &oap->پیل.lnum))
		    oap->پیل.ستون = ml_get_len(oap->پیل.lnum);
	    }
#د_که_پای
	    oap->پای_ټکی = oap->پیل;
	    oap->پیل = curwin->w_cursor;
	}

	// Just in case lines were deleted that make the position invalid.
	check_pos(curwin->w_بفر, &oap->پای_ټکی);
	oap->کرښه_شمېر = oap->پای_ټکی.lnum - oap->پیل.lnum + 1;

	// Set "virtual_op" before resetting VIsual_active.
	virtual_op = virtual_فعال();

	إذا (VIsual_فعال || redo_VIsual_busy)
	{
	    get_op_vcol(oap, redo_VIsual.rv_vcol, TRUE);

	    إذا (!redo_VIsual_busy && !gui_yank)
	    {
		// Prepare to reselect and redo Visual: this is based on the
		// size of the Visual text
		resel_VIsual_ډېرپېښ = VIsual_ډېرپېښ;
		إذا (curwin->w_curswant == MAXCOL)
		    resel_VIsual_vcol = MAXCOL;
		وإلا
		{
		    إذا (VIsual_ډېرپېښ != Ctrl_V)
			getvvcol(curwin, &(oap->پای_ټکی),
						عدم, عدم, &oap->پای_ټکی_vcol, 0);
		    إذا (VIsual_ډېرپېښ == Ctrl_V || oap->کرښه_شمېر <= 1)
		    {
			إذا (VIsual_ډېرپېښ != Ctrl_V)
			    getvvcol(curwin, &(oap->پیل),
					      &oap->پیل_vcol, عدم, عدم, 0);
			resel_VIsual_vcol = oap->پای_ټکی_vcol - oap->پیل_vcol + 1;
		    }
		    وإلا
			resel_VIsual_vcol = oap->پای_ټکی_vcol;
		}
		resel_VIsual_کرښه_شمېر = oap->کرښه_شمېر;
	    }

	    // can't redo yank (unless 'y' is in 'cpoptions') and ":"
	    إذا ((redo_yank || oap->op_type != OP_YANK)
		    && oap->op_type != OP_COLON
#که_تعریف_شوی FEAT_FOLDING
		    && oap->op_type != OP_FOLD
		    && oap->op_type != OP_FOLDOPEN
		    && oap->op_type != OP_FOLDOPENREC
		    && oap->op_type != OP_FOLDCLOSE
		    && oap->op_type != OP_FOLDCLOSEREC
		    && oap->op_type != OP_FOLDDEL
		    && oap->op_type != OP_FOLDDELREC
#د_که_پای
		    && oap->motion_force == NUL
		    )
	    {
		// Prepare for redoing.  Only use the nchar field for "r",
		// otherwise it might be the second char of the operator.
		إذا (cap->cmdchar == 'g' && (cap->nchar == 'n'
							|| cap->nchar == 'N'))
		    prep_redo(oap->regname, cap->count0,
			    get_op_char(oap->op_type),
			    get_extra_op_char(oap->op_type),
			    oap->motion_force, cap->cmdchar, cap->nchar);
		وإلا إذا (!is_ex_cmdchar(cap))
		{
		    صحيح opchar = get_op_char(oap->op_type);
		    صحيح extra_opchar = get_extra_op_char(oap->op_type);
		    صحيح nchar = oap->op_type == OP_REPLACE ? cap->nchar : NUL;

		    // reverse what nv_replace() did
		    إذا (nchar == REPLACE_CR_NCHAR)
			nchar = CAR;
		    وإلا إذا (nchar == REPLACE_NL_NCHAR)
			nchar = NL;

		    إذا (opchar == 'g' && extra_opchar == '@')
			// also repeat the count for 'operatorfunc'
			prep_redo_num2(oap->regname, 0L, NUL, 'v',
				     cap->count0, opchar, extra_opchar, nchar);
		    وإلا
			prep_redo(oap->regname, 0L, NUL, 'v',
						  opchar, extra_opchar, nchar);
		}
		إذا (!redo_VIsual_busy)
		{
		    redo_VIsual.rv_ډېرپېښ = resel_VIsual_ډېرپېښ;
		    redo_VIsual.rv_vcol = resel_VIsual_vcol;
		    redo_VIsual.rv_کرښه_شمېر = resel_VIsual_کرښه_شمېر;
		    redo_VIsual.rv_شمېر = cap->count0;
		    redo_VIsual.rv_arg = cap->arg;
		}
	    }

	    // oap->inclusive defaults to TRUE.
	    // If oap->end is on a NUL (empty line) oap->inclusive becomes
	    // FALSE.  This makes "d}P" and "v}dP" work the same.
	    إذا (oap->motion_force == NUL || oap->motion_type == MLINE)
		oap->inclusive = TRUE;
	    إذا (VIsual_ډېرپېښ == 'V')
		oap->motion_type = MLINE;
	    وإلا
	    {
		oap->motion_type = MCHAR;
		إذا (VIsual_ډېرپېښ != Ctrl_V && *ml_get_pos(&(oap->پای_ټکی)) == NUL
			&& (include_کرښه_break || !virtual_op))
		{
		    oap->inclusive = FALSE;
		    // Try to include the newline, unless it's an operator
		    // that works on lines only.
		    إذا (*p_sel != 'o'
			    && !op_on_کرښې(oap->op_type)
			    && oap->پای_ټکی.lnum < curbuf->b_ml.ml_کرښه_شمېر)
		    {
			++oap->پای_ټکی.lnum;
			oap->پای_ټکی.ستون = 0;
			oap->پای_ټکی.coladd = 0;
			++oap->کرښه_شمېر;
		    }
		}
	    }

	    redo_VIsual_busy = FALSE;

	    // Switch Visual off now, so screen updating does
	    // not show inverted text when the screen is redrawn.
	    // With OP_YANK and sometimes with OP_COLON and OP_FILTER there is
	    // no screen redraw, so it is done here to remove the inverted
	    // part.
	    إذا (!gui_yank)
	    {
		VIsual_فعال = FALSE;
		setmouse();
		mouse_dragging = 0;
		may_clear_cmdline();
		إذا ((oap->op_type == OP_YANK
			    || oap->op_type == OP_COLON
			    || oap->op_type == OP_FUNCTION
			    || oap->op_type == OP_FILTER)
			&& oap->motion_force == NUL)
		{
#که_تعریف_شوی FEAT_LINEBREAK
		    // make sure redrawing is correct
		    restore_lbr(lbr_saved);
#د_که_پای
		    redraw_curbuf_later(UPD_INVERTED);
		}
	    }
	}

	// Include the trailing byte of a multi-byte char.
	إذا (has_mbyte && oap->inclusive)
	{
	    صحيح		l;

	    l = (*mb_ptr2len)(ml_get_pos(&oap->پای_ټکی));
	    إذا (l > 1)
		oap->پای_ټکی.ستون += l - 1;
	}
	curwin->w_سېټ_curswant = true;

	// oap->empty is set when start and end are the same.  The inclusive
	// flag affects this too, unless yanking and the end is on a NUL.
	oap->تشه_2 = (oap->motion_type == MCHAR
		    && (!oap->inclusive
			|| (oap->op_type == OP_YANK
			    && gchar_pos(&oap->پای_ټکی) == NUL))
		    && EQUAL_POS(oap->پیل, oap->پای_ټکی)
		    && !(virtual_op && oap->پیل.coladd != oap->پای_ټکی.coladd));
	// For delete, change and yank, it's an error to operate on an
	// empty region, when 'E' included in 'cpoptions' (Vi compatible).
	تشه_region_error = (oap->تشه_2
				&& vim_strchr(p_cpo, CPO_EMPTYREGION) != عدم);

	// Force a redraw when operating on an empty Visual region, when
	// 'modifiable is off or creating a fold.
	إذا (oap->is_VIsual && (oap->تشه_2 || !curbuf->b_p_ma
#که_تعریف_شوی FEAT_FOLDING
		    || oap->op_type == OP_FOLD
#د_که_پای
		    ))
	{
#که_تعریف_شوی FEAT_LINEBREAK
	    restore_lbr(lbr_saved);
#د_که_پای
	    redraw_curbuf_later(UPD_INVERTED);
	}

	// If the end of an operator is in column one while oap->motion_type
	// is MCHAR and oap->inclusive is FALSE, we put op_end after the last
	// character in the previous line. If op_start is on or before the
	// first non-blank in the line, the operator becomes linewise
	// (strange, but that's the way vi does it).
	إذا (	   oap->motion_type == MCHAR
		&& oap->inclusive == FALSE
		&& !(cap->retval & CA_NO_ADJ_OP_پای_ټکی)
		&& oap->پای_ټکی.ستون == 0
		&& (!oap->is_VIsual || *p_sel == 'o')
		&& !oap->بلاک_ډېرپېښ
		&& oap->کرښه_شمېر > 1)
	{
	    oap->پای_ټکی_adjusted = TRUE;	    // remember that we did this
	    --oap->کرښه_شمېر;
	    --oap->پای_ټکی.lnum;
	    إذا (inindent(0))
		oap->motion_type = MLINE;
	    وإلا
	    {
		oap->پای_ټکی.ستون = ml_get_len(oap->پای_ټکی.lnum);
		إذا (oap->پای_ټکی.ستون)
		{
		    --oap->پای_ټکی.ستون;
		    oap->inclusive = TRUE;
		}
	    }
	}
	وإلا
	    oap->پای_ټکی_adjusted = FALSE;

	بدل (oap->op_type)
	{
	حالة OP_LSHIFT:
	حالة OP_RSHIFT:
	    op_ښويدنه(oap, TRUE, oap->is_VIsual ? (صحيح)cap->count1 : 1);
	    auto_format(FALSE, TRUE);
	    اكسر;

	حالة OP_JOIN_NS:
	حالة OP_JOIN:
	    إذا (oap->کرښه_شمېر < 2)
		oap->کرښه_شمېر = 2;
	    إذا (curwin->w_cursor.lnum + oap->کرښه_شمېر - 1 >
						   curbuf->b_ml.ml_کرښه_شمېر)
		beep_flush();
	    وإلا
	    {
		(فراغ)do_join(oap->کرښه_شمېر, oap->op_type == OP_JOIN,
							    TRUE, TRUE, TRUE);
		auto_format(FALSE, TRUE);
	    }
	    اكسر;

	حالة OP_DELETE:
	    VIsual_reselect = FALSE;	    // don't reselect now
	    إذا (تشه_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    وإلا
	    {
		(فراغ)op_delete(oap);
		// save cursor line for undo if it wasn't saved yet
		إذا (oap->motion_type == MLINE && has_format_option(FO_AUTO)
						      && u_save_cursor() == OK)
		    auto_format(FALSE, TRUE);
	    }
	    اكسر;

	حالة OP_YANK:
	    إذا (تشه_region_error)
	    {
		إذا (!gui_yank)
		{
		    vim_beep(BO_OPER);
		    CancelRedo();
		}
	    }
	    وإلا
	    {
#که_تعریف_شوی FEAT_LINEBREAK
		restore_lbr(lbr_saved);
#د_که_پای
		oap->excl_tr_ws = cap->cmdchar == 'z';
		(فراغ)op_yank(oap, FALSE, !gui_yank);
	    }
	    check_cursor_ستون();
	    اكسر;

	حالة OP_CHANGE:
	    VIsual_reselect = FALSE;	    // don't reselect now
	    إذا (تشه_region_error)
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
		إذا (p_im || !کلی_Typed)
		    restart_edit_save = restart_edit;
		وإلا
		    restart_edit_save = 0;
		restart_edit = 0;
#که_تعریف_شوی FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#د_که_پای
		// trigger TextChangedI
		curbuf->b_وروستی_changedtick_i = CHANGEDTICK(curbuf);

		إذا (op_change(oap))	// will call edit()
		    cap->retval |= CA_COMMAND_BUSY;
		إذا (restart_edit == 0)
		    restart_edit = restart_edit_save;
	    }
	    اكسر;

	حالة OP_FILTER:
	    إذا (vim_strchr(p_cpo, CPO_FILTER) != عدم)
		Append_منزل_Redobuff((char_u *)"!\r");  // use any last used !cmd
	    وإلا
		bangredo = TRUE;    // do_bang() will put cmd in redo buffer
	    // FALLTHROUGH

	حالة OP_INDENT:
	حالة OP_COLON:

	    // If 'equalprg' is empty, do the indenting internally.
	    إذا (oap->op_type == OP_INDENT && *get_equalprg() == NUL)
	    {
		إذا (curbuf->b_p_lisp)
		{
#که_تعریف_شوی FEAT_EVAL
		    إذا (use_indentexpr_for_lisp())
			op_reindent(oap, get_بیان_indent);
		    وإلا
#د_که_پای
			op_reindent(oap, get_lisp_indent);
		    اكسر;
		}
		op_reindent(oap,
#که_تعریف_شوی FEAT_EVAL
			*curbuf->b_p_inde != NUL ? get_بیان_indent :
#د_که_پای
			    get_c_indent);
		اكسر;
	    }

	    op_colon(oap);
	    اكسر;

	حالة OP_TILDE:
	حالة OP_پاسنی:
	حالة OP_ښکتنی:
	حالة OP_ROT13:
	    إذا (تشه_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    وإلا
		op_tilde(oap);
	    check_cursor_ستون();
	    اكسر;

	حالة OP_FORMAT:
#إذا defined(FEAT_EVAL)
	    إذا (*curbuf->b_p_fex != NUL)
		op_formatexpr(oap);	// use expression
	    وإلا
#د_که_پای
	    {
		إذا (*p_fp != NUL || *curbuf->b_p_fp != NUL)
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

#که_تعریف_شوی FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#د_که_پای
		// call 'operatorfunc'
		op_function(oap);

		// Restore the info for redoing Visual mode, the function may
		// invoke another operator and unintentionally change it.
		redo_VIsual = save_redo_VIsual;
		اكسر;
	    }

	حالة OP_ورننباسه:
	حالة OP_APPEND:
	    VIsual_reselect = FALSE;	// don't reselect now
	    إذا (تشه_region_error)
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
#که_تعریف_شوی FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#د_که_پای
		// trigger TextChangedI
		curbuf->b_وروستی_changedtick_i = CHANGEDTICK(curbuf);

		op_ورننباسه(oap, cap->count1);
#که_تعریف_شوی FEAT_LINEBREAK
		// Reset linebreak, so that formatting works correctly.
		(فراغ)reset_lbr();
#د_که_پای

		// TODO: when inserting in several lines, should format all
		// the lines.
		auto_format(FALSE, TRUE);

		إذا (restart_edit == 0)
		    restart_edit = restart_edit_save;
		وإلا
		    cap->retval |= CA_COMMAND_BUSY;
	    }
	    اكسر;

	حالة OP_REPLACE:
	    VIsual_reselect = FALSE;	// don't reselect now
	    إذا (تشه_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    وإلا
	    {
#که_تعریف_شوی FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#د_که_پای
		op_replace(oap, cap->nchar);
	    }
	    اكسر;

#که_تعریف_شوی FEAT_FOLDING
	حالة OP_FOLD:
	    VIsual_reselect = FALSE;	// don't reselect now
	    foldCreate(oap->پیل.lnum, oap->پای_ټکی.lnum);
	    اكسر;

	حالة OP_FOLDOPEN:
	حالة OP_FOLDOPENREC:
	حالة OP_FOLDCLOSE:
	حالة OP_FOLDCLOSEREC:
	    VIsual_reselect = FALSE;	// don't reselect now
	    opFoldRange(oap->پیل.lnum, oap->پای_ټکی.lnum,
		    oap->op_type == OP_FOLDOPEN
					    || oap->op_type == OP_FOLDOPENREC,
		    oap->op_type == OP_FOLDOPENREC
					  || oap->op_type == OP_FOLDCLOSEREC,
					  oap->is_VIsual);
	    اكسر;

	حالة OP_FOLDDEL:
	حالة OP_FOLDDELREC:
	    VIsual_reselect = FALSE;	// don't reselect now
	    deleteFold(oap->پیل.lnum, oap->پای_ټکی.lnum,
			       oap->op_type == OP_FOLDDELREC, oap->is_VIsual);
	    اكسر;
#د_که_پای
	حالة OP_NR_جمع_کړه:
	حالة OP_NR_SUB:
	    إذا (تشه_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    وإلا
	    {
		VIsual_فعال = TRUE;
#که_تعریف_شوی FEAT_LINEBREAK
		restore_lbr(lbr_saved);
#د_که_پای
		op_addsub(oap, cap->count1, redo_VIsual.rv_arg);
		VIsual_فعال = FALSE;
	    }
	    check_cursor_ستون();
	    اكسر;
	افتراضي:
	    clearopbeep(oap);
	}
	virtual_op = MAYBE;
	إذا (!gui_yank)
	{
	    // if 'sol' not set, go back to old column for some commands
	    إذا (!p_sol && oap->motion_type == MLINE && !oap->پای_ټکی_adjusted
		    && (oap->op_type == OP_LSHIFT || oap->op_type == OP_RSHIFT
						|| oap->op_type == OP_DELETE))
	    {
#که_تعریف_شوی FEAT_LINEBREAK
		(فراغ)reset_lbr();
#د_که_پای
		coladvance(curwin->w_curswant = old_ستون);
	    }
	}
	وإلا
	{
	    curwin->w_cursor = old_cursor;
	}
	oap->بلاک_ډېرپېښ = FALSE;
	clearop(oap);
	motion_force = NUL;
    }
#که_تعریف_شوی FEAT_LINEBREAK
    restore_lbr(lbr_saved);
#د_که_پای
}

// put byte 'c' at position 'lp', but
// verify, that the position to place
// is actually safe
    ساكن فراغ
pbyte(pos_T lp, صحيح c)
{
    char_u *p = ml_get_buf(curbuf, lp.lnum, TRUE);
    صحيح	len = curbuf->b_ml.ml_کرښه_len;

    // safety check
    إذا (lp.ستون >= len)
	lp.ستون = (len > 1 ? len - 2 : 0);
    *(p + lp.ستون) = c;
}
