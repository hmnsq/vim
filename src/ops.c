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

ساكن فراغ کھسکاؤ_بلاک(oparg_T *oap, صحيح amount);
ساكن فراغ	mb_adjust_opend(oparg_T *oap);
ساكن صحيح	do_addsub(صحيح op_type, pos_T *pos, صحيح لمائی, linenr_T Prenum1);
ساكن فراغ	pbyte(pos_T lp, صحيح c);
#تعریف_کرو PBYTE(lp, c) pbyte(lp, c)


// Flags for third item in "opchars".
#تعریف_کرو OPF_سطریں  1	// operator always works on lines
#تعریف_کرو OPF_CHANGE 2	// operator changes text

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
    {'<', NUL, OPF_سطریں | OPF_CHANGE},	// OP_LSHIFT
    {'>', NUL, OPF_سطریں | OPF_CHANGE},	// OP_RSHIFT
    {'!', NUL, OPF_سطریں | OPF_CHANGE},	// OP_FILTER
    {'g', '~', OPF_CHANGE},		// OP_TILDE
    {'=', NUL, OPF_سطریں | OPF_CHANGE},	// OP_INDENT
    {'g', 'q', OPF_سطریں | OPF_CHANGE},	// OP_FORMAT
    {':', NUL, OPF_سطریں},		// OP_COLON
    {'g', 'U', OPF_CHANGE},		// OP_UPPER
    {'g', 'u', OPF_CHANGE},		// OP_LOWER
    {'J', NUL, OPF_سطریں | OPF_CHANGE},	// DO_JOIN
    {'g', 'J', OPF_سطریں | OPF_CHANGE},	// DO_JOIN_NS
    {'g', '?', OPF_CHANGE},		// OP_ROT13
    {'r', NUL, OPF_CHANGE},		// OP_REPLACE
    {'I', NUL, OPF_CHANGE},		// OP_INSERT
    {'A', NUL, OPF_CHANGE},		// OP_APPEND
    {'z', 'f', OPF_سطریں},		// OP_FOLD
    {'z', 'o', OPF_سطریں},		// OP_FOLDOPEN
    {'z', 'O', OPF_سطریں},		// OP_FOLDOPENREC
    {'z', 'c', OPF_سطریں},		// OP_FOLDCLOSE
    {'z', 'C', OPF_سطریں},		// OP_FOLDCLOSEREC
    {'z', 'd', OPF_سطریں},		// OP_FOLDDEL
    {'z', 'D', OPF_سطریں},		// OP_FOLDDELREC
    {'g', 'w', OPF_سطریں | OPF_CHANGE},	// OP_FORMAT2
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
    إذا (char1 == 'g' && char2 == Ctrl_A)	//جوڑو
	أرجع OP_NR_جوڑو;
    إذا (char1 == 'g' && char2 == Ctrl_X)	//گھٹاؤ
	أرجع OP_NR_SUB;
    إذا (char1 == 'z' && char2 == 'y')	// OP_YANK
	أرجع OP_YANK;
    لكل (i = 0; ; ++i)
    {
	إذا (opchars[i][0] == char1 && opchars[i][1] == char2)
	    اكسر;
	إذا (i == (صحيح)ARRAY_لمائی(opchars) - 1)
	{
	    اندرونی_error("get_op_type()");
	    اكسر;
	}
    }
    أرجع i;
}

/*
 * Return TRUE if operator "op" always works on whole lines.
 */
    ساكن صحيح
op_on_سطریں(صحيح op)
{
    أرجع opchars[op][2] & OPF_سطریں;
}

#إذا defined(FEAT_کم_CHANNEL)
/*
 * Return TRUE if operator "op" changes text.
 */
    صحيح
op_is_change(صحيح op)
{
    أرجع opchars[op][2] & OPF_CHANGE;
}
#جے_انت

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
op_کھسکاؤ(oparg_T *oap, صحيح curs_چوٹی, صحيح amount)
{
    لما	    i;
    صحيح		    پہلا_char;
    صحيح		    بلاک_خانہ = 0;

    إذا (u_save((linenr_T)(oap->شروع.lnum - 1),
				       (linenr_T)(oap->سرا.lnum + 1)) == FAIL)
	أرجع;

    إذا (oap->بلاک_منوال)
	بلاک_خانہ = curwin->w_cursor.خانہ;

    لكل (i = oap->سطر_گنتی; --i >= 0; )
    {
	پہلا_char = *ml_get_curline();
	إذا (پہلا_char == NUL)				// empty line
	    curwin->w_cursor.خانہ = 0;
	وإلا إذا (oap->بلاک_منوال)
	    کھسکاؤ_بلاک(oap, amount);
	وإلا
	    // Move the line right if it doesn't start with '#', 'smartindent'
	    // isn't set or 'cindent' isn't set or '#' isn't in 'cino'.
	    إذا (پہلا_char != '#' || !preprocs_بایاں())
		کھسکاؤ_سطر(oap->op_type == OP_LSHIFT, p_sr, amount, FALSE);
	++curwin->w_cursor.lnum;
    }

    بدلیا_سطریں(oap->شروع.lnum, 0, oap->سرا.lnum + 1, 0L);
    إذا (oap->بلاک_منوال)
    {
	curwin->w_cursor.lnum = oap->شروع.lnum;
	curwin->w_cursor.خانہ = بلاک_خانہ;
    }
    وإلا إذا (curs_چوٹی)	    // put cursor on first line, for ">>"
    {
	curwin->w_cursor.lnum = oap->شروع.lnum;
	beginline(BL_SOL | BL_FIX);   // shift_line() may have set cursor.col
    }
    وإلا
	--curwin->w_cursor.lnum;	// put cursor on last line, for ":>"

#جے_تعریف_شدہ FEAT_FOLDING
    // The cursor line is not in a closed fold
    foldOpenCursor();
#جے_انت


    إذا (oap->سطر_گنتی > p_report)
    {
	حرف	    *op;
	حرف	    *پیغام_سطر_single;
	حرف	    *پیغام_سطر_plural;

	إذا (oap->op_type == OP_RSHIFT)
	    op = ">";
	وإلا
	    op = "<";
	پیغام_سطر_single = NGETTEXT("%ld line %sed %d time",
					     "%ld line %sed %d times", amount);
	پیغام_سطر_plural = NGETTEXT("%ld lines %sed %d time",
					    "%ld lines %sed %d times", amount);
	vim_snprintf((حرف *)IObuff, IOSIZE,
		NGETTEXT(پیغام_سطر_single, پیغام_سطر_plural, oap->سطر_گنتی),
		oap->سطر_گنتی, op, amount);
	پیغام_attr_keep((حرف *)IObuff, 0, TRUE);
    }

    إذا ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_شروع = oap->شروع;
	curbuf->b_op_سرا.lnum = oap->سرا.lnum;
	curbuf->b_op_سرا.خانہ = ml_get_len(oap->سرا.lnum);
	إذا (curbuf->b_op_سرا.خانہ > 0)
	    --curbuf->b_op_سرا.خانہ;
    }
}

#جے_تعریف_شدہ FEAT_VARTABS
/*
 * Return the tabstop width at the index of the variable tabstop array.  If an
 * index greater than the length of the array is given, the last tabstop width
 * in the array is returned.
 */
    ساكن صحيح
get_vts(صحيح *vts_array, صحيح اشاریہ)
{
    صحيح	ts;

    إذا (اشاریہ < 1)
	ts = 0;
    وإلا إذا (اشاریہ <= vts_array[0])
	ts = vts_array[اشاریہ];
    وإلا
	ts = vts_array[vts_array[0]];

    أرجع ts;
}

/*
 * Return the sum of all the tabstops through the index-th.
 */
    ساكن صحيح
get_vts_جوڑ(صحيح *vts_array, صحيح اشاریہ)
{
    صحيح	جوڑ = 0;
    صحيح	i;

    // Perform the summation for indices within the actual array.
    لكل (i = 1; i <= اشاریہ && i <= vts_array[0]; i++)
	جوڑ += vts_array[i];

    // Add tabstops whose indices exceed the actual array.
    إذا (i <= اشاریہ)
	جوڑ += vts_array[vts_array[0]] * (اشاریہ - vts_array[0]);

    أرجع جوڑ;
}
#جے_انت

    ساكن vimlong_T
get_new_sw_indent(
    صحيح		بایاں,		// TRUE if shift is to the left
    صحيح		گیڑا,		// TRUE if new indent is to be to a tabstop
    vimlong_T	amount,		// Number of shifts
    vimlong_T	sw_val)
{
    vimlong_T	گنتی = get_indent();
    vimlong_T	i, j;

    إذا (گیڑا)			// round off indent
    {
	i = trim_منزل_int(گنتی) / sw_val;	// number of 'shiftwidth' rounded down
	j = trim_منزل_int(گنتی) % sw_val;	// extra spaces
	إذا (j && بایاں)		// first remove extra spaces
	    --amount;
	إذا (بایاں)
	{
	    i -= amount;
	    إذا (i < 0)
		i = 0;
	}
	وإلا
	    i += amount;
	گنتی = i * sw_val;
    }
    وإلا			// original vi indent
    {
	إذا (بایاں)
	{
	    گنتی -= sw_val * amount;
	    إذا (گنتی < 0)
		گنتی = 0;
	}
	وإلا
	    گنتی += sw_val * amount;
    }

    أرجع گنتی;
}

#جے_تعریف_شدہ FEAT_VARTABS
    ساكن vimlong_T
get_new_vts_indent(
    صحيح	بایاں,			// TRUE if shift is to the left
    صحيح	گیڑا,			// TRUE if new indent is to be to a tabstop
    صحيح	amount,			// Number of shifts
    صحيح	*vts_array)
{
    vimlong_T	indent = get_indent();
    صحيح		vtsi = 0;
    صحيح		vts_indent = 0;
    صحيح		ts = 0;		// Silence uninitialized variable warning.
    صحيح		وتھ;		// Extra indent spaces to the right of the
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

    وتھ = indent - vts_indent;

    إذا (گیڑا)
    {
	إذا (بایاں)
	{
	    إذا (وتھ == 0)
		indent = get_vts_جوڑ(vts_array, vtsi - amount);
	    وإلا
		indent = get_vts_جوڑ(vts_array, vtsi - (amount - 1));
	}
	وإلا
	    indent = get_vts_جوڑ(vts_array, vtsi + amount);
    }
    وإلا
    {
	إذا (بایاں)
	{
	    إذا (amount > vtsi)
		indent = 0;
	    وإلا
		indent = get_vts_جوڑ(vts_array, vtsi - amount) + وتھ;
	}
	وإلا
	    indent = get_vts_جوڑ(vts_array, vtsi + amount) + وتھ;
    }

    أرجع indent;
}
#جے_انت

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
کھسکاؤ_سطر(
    صحيح	بایاں,			// TRUE if shift is to the left
    صحيح	گیڑا,			// TRUE if new indent is to be to a tabstop
    صحيح	amount,			// Number of shifts
    صحيح	call_بدلیا_بائٹاں)	// call changed_bytes()
{
    vimlong_T	گنتی;
    لما	sw_val = curbuf->b_p_sw;
    لما	ts_val = curbuf->b_p_ts;
#جے_تعریف_شدہ FEAT_VARTABS
    صحيح		*vts_array = curbuf->b_p_vts_array;
#جے_انت

    إذا (sw_val != 0)
	// 'shiftwidth' is not zero; use it as the shift size.
	گنتی = get_new_sw_indent(بایاں, گیڑا, amount, sw_val);
    وإلا
#جے_تعریف_شدہ FEAT_VARTABS
	إذا ((vts_array == عدم) || (vts_array[0] == 0))
#جے_انت
    {
	// 'shiftwidth is zero and 'vartabstop' is empty; use 'tabstop' as the
	// shift size.
	گنتی = get_new_sw_indent(بایاں, گیڑا, amount, ts_val);
    }
#جے_تعریف_شدہ FEAT_VARTABS
    وإلا
    {
	// 'shiftwidth is zero and 'vartabstop' is defined; use 'vartabstop'
	// to determine the new indent.
	گنتی = get_new_vts_indent(بایاں, گیڑا, amount, vts_array);
    }
#جے_انت

    // Set new indent
    إذا (State & VREPLACE_FLAG)
	change_indent(INDENT_سیٹ, trim_منزل_int(گنتی), FALSE, NUL, call_بدلیا_بائٹاں);
    وإلا
	(فراغ)سیٹ_indent(trim_منزل_int(گنتی), call_بدلیا_بائٹاں ? SIN_بدلیا : 0);
}

/*
 * Shift one line of the current block one shiftwidth right or left.
 * Leaves cursor on first character in block.
 */
    ساكن فراغ
کھسکاؤ_بلاک(oparg_T *oap, صحيح amount)
{
    صحيح			بایاں = (oap->op_type == OP_LSHIFT);
    صحيح			oldstate = State;
    صحيح			کل;
    char_u		*newp, *oldp;
    size_t		newlen, oldlen;
    صحيح			oldcol = curwin->w_cursor.خانہ;
    صحيح			sw_val = (صحيح)get_sw_قدر_indent(curbuf, بایاں);
    صحيح			ts_val = (صحيح)curbuf->b_p_ts;
    بنية بلاک_def	bd;
    صحيح			incr;
    colnr_T		ws_vcol;
    صحيح			added;
    size_t		new_سطر_len;	// the length of the line after the
					// block shift
#جے_تعریف_شدہ FEAT_RIGHTLEFT
    صحيح			old_p_ri = p_ri;

    p_ri = 0;			// don't want revins in indent
#جے_انت

    State = منوال_داخل_کرو;	// don't want MODE_REPLACE for State
    بلاک_prep(oap, &bd, curwin->w_cursor.lnum, TRUE);
    إذا (bd.is_short)
	أرجع;

    // total is number of screen columns to be inserted/removed
    کل = (صحيح)((بنانشانی)amount * (بنانشانی)sw_val);
    إذا ((کل / sw_val) != amount)
	أرجع; // multiplication overflow

    oldp = ml_get_curline();
    oldlen = ml_get_curline_len();

    إذا (!بایاں)
    {
	صحيح		tabs = 0, spaces = 0;
	chartabsize_T	cts;

	/*
	 *  1. Get start vcol
	 *  2. Total ws vcols
	 *  3. Divvy into TABs & spp
	 *  4. Construct new string
	 */
	کل += bd.pre_whitesp; // all virtual WS up to & incl a split TAB
	ws_vcol = bd.شروع_vcol - bd.pre_whitesp;
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
				   bd.شروع_vcol, bd.textstart, bd.textstart);
	لكل ( ; VIM_ISWHITE(*cts.cts_ptr); )
	{
	    incr = lbr_chartabsize_adv(&cts);
	    کل += incr;
	    cts.cts_vcol += incr;
	}
	bd.textstart = cts.cts_ptr;
	bd.شروع_vcol = cts.cts_vcol;
	clear_chartabsize_arg(&cts);

	// OK, now total=all the VWS reqd, and textstart points at the 1st
	// non-ws char in the block.
#جے_تعریف_شدہ FEAT_VARTABS
	إذا (!curbuf->b_p_et)
	    tabstop_fromto(ws_vcol, ws_vcol + کل,
				ts_val, curbuf->b_p_vts_array, &tabs, &spaces);
	وإلا
	    spaces = کل;
#وإلا
	إذا (!curbuf->b_p_et)
	    tabs = ((ws_vcol % ts_val) + کل) / ts_val; // number of tabs
	إذا (tabs > 0)
	    spaces = ((ws_vcol % ts_val) + کل) % ts_val; // number of spp
	وإلا
	    spaces = کل;
#جے_انت
	// if we're splitting a TAB, allow for it
	bd.textcol -= bd.pre_whitesp_c - (bd.startspaces != 0);

	new_سطر_len = bd.textcol + tabs + spaces + (oldlen - (bd.textstart - oldp));
	newp = alloc(new_سطر_len + 1);
	إذا (newp == عدم)
	    أرجع;
	mch_memmove(newp, oldp, (size_t)bd.textcol);
	newlen = bd.textcol;
	vim_memset(newp + newlen, TAB, (size_t)tabs);
	newlen += tabs;
	vim_memset(newp + newlen, ' ', (size_t)spaces);
	STRCPY(newp + newlen + spaces, bd.textstart);
    }
    وإلا //بایاں
    {
	colnr_T	    destination_خانہ;	// column to which text in block will
					// be shifted
	char_u	    *verbatim_copy_سرا;	// end of the part of the line which is
					// copied verbatim
	colnr_T	    verbatim_copy_چوڑائی;// the (displayed) width of this part
					// of line
	size_t	    fill;		// nr of spaces that replace a TAB
	size_t	    بلاک_space_چوڑائی;
	size_t	    کھسکاؤ_amount;
	char_u	    *non_white = bd.textstart;
	colnr_T	    non_white_خانہ;
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
	non_white_خانہ = bd.شروع_vcol;

	init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum,
				   non_white_خانہ, bd.textstart, non_white);
	طالما (VIM_ISWHITE(*cts.cts_ptr))
	{
	    incr = lbr_chartabsize_adv(&cts);
	    cts.cts_vcol += incr;
	}
	non_white_خانہ = cts.cts_vcol;
	non_white = cts.cts_ptr;
	clear_chartabsize_arg(&cts);

	بلاک_space_چوڑائی = non_white_خانہ - oap->شروع_vcol;
	// We will shift by "total" or "block_space_width", whichever is less.
	کھسکاؤ_amount = (بلاک_space_چوڑائی < (size_t)کل
					 ? بلاک_space_چوڑائی : (size_t)کل);

	// The column to which we will shift the text.
	destination_خانہ = (colnr_T)(non_white_خانہ - کھسکاؤ_amount);

	// Now let's find out how much of the beginning of the line we can
	// reuse without modification.
	verbatim_copy_سرا = bd.textstart;
	verbatim_copy_چوڑائی = bd.شروع_vcol;

	// If "bd.startspaces" is set, "bd.textstart" points to the character
	// preceding the block. We have to subtract its width to obtain its
	// column number.
	إذا (bd.startspaces)
	    verbatim_copy_چوڑائی -= bd.شروع_char_vcols;
	init_chartabsize_arg(&cts, curwin, 0, verbatim_copy_چوڑائی,
					     bd.textstart, verbatim_copy_سرا);
	طالما (cts.cts_vcol < destination_خانہ)
	{
	    incr = lbr_chartabsize(&cts);
	    إذا (cts.cts_vcol + incr > destination_خانہ)
		اكسر;
	    cts.cts_vcol += incr;
	    MB_PTR_ADV(cts.cts_ptr);
	}
	verbatim_copy_چوڑائی = cts.cts_vcol;
	verbatim_copy_سرا = cts.cts_ptr;
	clear_chartabsize_arg(&cts);

	// If "destination_col" is different from the width of the initial
	// part of the line that will be copied, it means we encountered a tab
	// character, which we will have to partly replace with spaces.
	fill = destination_خانہ - verbatim_copy_چوڑائی;

	// The replacement line will consist of:
	// - the beginning of the original line up to "verbatim_copy_end",
	// - "fill" number of spaces,
	// - the rest of the line, pointed to by non_white.
	fixedlen = verbatim_copy_سرا - oldp;
	new_سطر_len = fixedlen + fill + (oldlen - (non_white - oldp));

	newp = alloc(new_سطر_len + 1);
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
    إذا (new_سطر_len >= oldlen)
	added = (صحيح)(new_سطر_len - oldlen);
    وإلا
	added = 0 - (صحيح)(oldlen - new_سطر_len);
    inserted_بائٹاں(curwin->w_cursor.lnum, bd.textcol, added);
    State = oldstate;
    curwin->w_cursor.خانہ = oldcol;
#جے_تعریف_شدہ FEAT_RIGHTLEFT
    p_ri = old_p_ri;
#جے_انت
}

/*
 * Insert string "s" (b_insert ? before : after) block :AKelly
 * Caller must prepare for undo.
 */
    ساكن فراغ
بلاک_داخل_کرو(
    oparg_T		*oap,
    char_u		*s,
    size_t		slen,
    صحيح			b_داخل_کرو,
    بنية بلاک_def	*bdp)
{
    صحيح		ts_val;
    صحيح		گنتی = 0;	// extra spaces to replace a cut TAB
    صحيح		spaces = 0;	// non-zero if cutting a TAB
    colnr_T	وتھ;		// pointer along new line
    colnr_T	startcol;	// column where insert starts
    char_u	*newp, *oldp;	// new, old lines
    linenr_T	lnum;		// loop var
    صحيح		oldstate = State;

    State = منوال_داخل_کرو;	// don't want MODE_REPLACE for State

    لكل (lnum = oap->شروع.lnum + 1; lnum <= oap->سرا.lnum; lnum++)
    {
	بلاک_prep(oap, bdp, lnum, TRUE);
	إذا (bdp->is_short && b_داخل_کرو)
	    استمر;	// OP_INSERT, line ends before block start

	oldp = ml_get(lnum);

	إذا (b_داخل_کرو)
	{
	    ts_val = bdp->شروع_char_vcols;
	    spaces = bdp->startspaces;
	    إذا (spaces != 0)
		گنتی = ts_val - 1; // we're cutting a TAB
	    وتھ = bdp->textcol;
	}
	وإلا // append
	{
	    ts_val = bdp->سرا_char_vcols;
	    إذا (!bdp->is_short) // spaces = padding after block
	    {
		spaces = (bdp->endspaces ? ts_val - bdp->endspaces : 0);
		إذا (spaces != 0)
		    گنتی = ts_val - 1; // we're cutting a TAB
		وتھ = bdp->textcol + bdp->textlen - (spaces != 0);
	    }
	    وإلا // spaces = padding to block edge
	    {
		// if $ used, just append to EOL (ie spaces==0)
		إذا (!bdp->is_MAX)
		    spaces = (oap->سرا_vcol - bdp->سرا_vcol) + 1;
		گنتی = spaces;
		وتھ = bdp->textcol + bdp->textlen;
	    }
	}

	إذا (has_mbyte && spaces > 0)
	    // avoid copying part of a multi-byte character
	    وتھ -= (*mb_سر_off)(oldp, oldp + وتھ);

	إذا (spaces < 0)  // can happen when the cursor was moved
	    spaces = 0;

	// Make sure the allocated size matches what is actually copied below.
	newp = alloc(ml_get_len(lnum) + spaces + slen
		    + (spaces > 0 && !bdp->is_short ? ts_val - spaces : 0)
								  + گنتی + 1);
	إذا (newp == عدم)
	    استمر;

	// copy up to shifted part
	mch_memmove(newp, oldp, (size_t)وتھ);
	oldp += وتھ;

	// insert pre-padding
	vim_memset(newp + وتھ, ' ', (size_t)spaces);
	startcol = وتھ + spaces;

	// copy the new text
	mch_memmove(newp + startcol, s, slen);
	وتھ += (صحيح)slen;

	إذا (spaces > 0 && !bdp->is_short)
	{
	    إذا (*oldp == TAB)
	    {
		// insert post-padding
		vim_memset(newp + وتھ + spaces, ' ',
						    (size_t)(ts_val - spaces));
		// we're splitting a TAB, don't copy it
		oldp++;
		// We allowed for that TAB, remember this now
		گنتی++;
	    }
	    وإلا
		// Not a TAB, no extra spaces
		گنتی = spaces;
	}

	إذا (spaces > 0)
	    وتھ += گنتی;
	STRCPY(newp + وتھ, oldp);

	ml_replace(lnum, newp, FALSE);

	إذا (b_داخل_کرو)
	    // correct any text properties
	    inserted_بائٹاں(lnum, startcol, (صحيح)slen);

	إذا (lnum == oap->سرا.lnum)
	{
	    // Set "']" mark to the end of the block instead of the end of
	    // the insert in the first line.
	    curbuf->b_op_سرا.lnum = oap->سرا.lnum;
	    curbuf->b_op_سرا.خانہ = وتھ;
	    إذا (curbuf->b_visual.vi_سرا.coladd)
	    {
		curbuf->b_visual.vi_سرا.خانہ += curbuf->b_visual.vi_سرا.coladd;
		curbuf->b_visual.vi_سرا.coladd = 0;
	    }
	}
    } // for all lnum

    بدلیا_سطریں(oap->شروع.lnum + 1, 0, oap->سرا.lnum + 1, 0L);

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
    linenr_T		old_lcount = curbuf->b_ml.ml_سطر_گنتی;
    صحيح			did_yank = FALSE;

    إذا (curbuf->b_ml.ml_flags & ML_خالی_تھاواں)	    // nothing to do
	أرجع OK;

    // Nothing to delete, return here.	Do prepare undo, for op_change().
    إذا (oap->خالی_تھاواں_2)
	أرجع u_save_cursor();

    إذا (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	أرجع FAIL;
    }

    إذا (VIsual_select && oap->is_VIsual)
	// use register given with CTRL_R, defaults to zero
	oap->regname = VIsual_select_reg;

#جے_تعریف_شدہ HAVE_CLIPMETHOD
    adjust_clip_reg(&oap->regname);
#جے_انت

    إذا (has_mbyte)
	mb_adjust_opend(oap);

    /*
     * Imitate the strange Vi behaviour: If the delete spans more than one
     * line and motion_type == MCHAR and the result is a blank line, make the
     * delete linewise.  Don't do this for the change command or Visual mode.
     */
    إذا (       oap->motion_type == MCHAR
	    && !oap->is_VIsual
	    && !oap->بلاک_منوال
	    && oap->سطر_گنتی > 1
	    && oap->motion_force == NUL
	    && (vim_strchr(p_cpo, CPO_لفظ) != عدم)
	    && oap->op_type == OP_DELETE)
    {
	ptr = ml_get(oap->سرا.lnum) + oap->سرا.خانہ;
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
	    && oap->سطر_گنتی == 1
	    && oap->op_type == OP_DELETE
	    && *ml_get(oap->شروع.lnum) == NUL)
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
#جے_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
	inc_clip_provider();
#جے_انت
	إذا (oap->regname != 0)
	{
	    // check for read-only register
	    إذا (!valid_yank_reg(oap->regname, TRUE))
	    {
		beep_flush();
#جے_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
		dec_clip_provider();
#جے_انت
		أرجع OK;
	    }
	    get_yank_رجسٹر(oap->regname, TRUE); // yank into specif'd reg.
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
	إذا (oap->motion_type == MLINE || oap->سطر_گنتی > 1
							   || oap->use_reg_one)
	{
	    کھسکاؤ_delete_registers();
	    إذا (op_yank(oap, TRUE, FALSE) == OK)
		did_yank = TRUE;
	}

	// Yank into small delete register when no named register specified
	// and the delete is within one line.
	إذا ((
#جے_تعریف_شدہ HAVE_CLIPMETHOD
	    ((clip_unnamed & CLIP_UNNAMED) && oap->regname == '*') ||
	    ((clip_unnamed & CLIP_UNNAMED_PLUS) && oap->regname == '+') ||
#جے_انت
	    oap->regname == 0) && oap->motion_type != MLINE
						      && oap->سطر_گنتی == 1)
	{
	    oap->regname = '-';
	    get_yank_رجسٹر(oap->regname, TRUE);
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
#جے_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
		dec_clip_provider();
#جے_انت
		أرجع FAIL;
	    }
	}

#إذا defined(FEAT_EVAL)
	إذا (did_yank && has_textyankpost())
	    yank_do_autocmd(oap, get_y_current());
#جے_انت
#جے_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#جے_انت
    }

    /*
     * block mode delete
     */
    إذا (oap->بلاک_منوال)
    {
	إذا (u_save((linenr_T)(oap->شروع.lnum - 1),
			       (linenr_T)(oap->سرا.lnum + 1)) == FAIL)
	    أرجع FAIL;

	لكل (lnum = curwin->w_cursor.lnum; lnum <= oap->سرا.lnum; ++lnum)
	{
	    بلاک_prep(oap, &bd, lnum, TRUE);
	    إذا (bd.textlen == 0)	// nothing to delete
		استمر;

	    // Adjust cursor position for tab replaced by spaces and 'lbr'.
	    إذا (lnum == curwin->w_cursor.lnum)
	    {
		curwin->w_cursor.خانہ = bd.textcol + bd.startspaces;
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

#جے_تعریف_شدہ FEAT_PROP_POPUP
	    إذا (curbuf->b_has_textprop && n != 0)
		adjust_prop_columns(lnum, bd.textcol, -n, 0);
#جے_انت
	}

	check_cursor_خانہ();
	بدلیا_سطریں(curwin->w_cursor.lnum, curwin->w_cursor.خانہ,
						       oap->سرا.lnum + 1, 0L);
	oap->سطر_گنتی = 0;	    // no lines deleted
    }
    وإلا إذا (oap->motion_type == MLINE)
    {
	إذا (oap->op_type == OP_CHANGE)
	{
	    // Delete the lines except the first one.  Temporarily move the
	    // cursor to the next line.  Save the current line number, if the
	    // last line is deleted it may be changed.
	    إذا (oap->سطر_گنتی > 1)
	    {
		lnum = curwin->w_cursor.lnum;
		++curwin->w_cursor.lnum;
		del_سطریں((لما)(oap->سطر_گنتی - 1), TRUE);
		curwin->w_cursor.lnum = lnum;
	    }
	    إذا (u_save_cursor() == FAIL)
		أرجع FAIL;
	    إذا (curbuf->b_p_ai)		    // don't delete indent
	    {
		beginline(BL_WHITE);	    // cursor on first non-white
		did_ai = TRUE;		    // delete the indent when ESC hit
		ai_خانہ = curwin->w_cursor.خانہ;
	    }
	    وإلا
		beginline(0);		    // cursor in column 0
	    truncate_سطر(FALSE);   // delete the rest of the line,
				    // leaving cursor past last char in line
	    إذا (oap->سطر_گنتی > 1)
		u_clearline();	    // "U" command not possible after "2cc"
	}
	وإلا
	{
	    del_سطریں(oap->سطر_گنتی, TRUE);
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
	    إذا (gchar_pos(&oap->شروع) == '\t')
	    {
		إذا (u_save_cursor() == FAIL)	// save first line for undo
		    أرجع FAIL;
		إذا (oap->سطر_گنتی == 1)
		    endcol = getviscol2(oap->سرا.خانہ, oap->سرا.coladd);
		coladvance_force(getviscol2(oap->شروع.خانہ, oap->شروع.coladd));
		oap->شروع = curwin->w_cursor;
		إذا (oap->سطر_گنتی == 1)
		{
		    coladvance(endcol);
		    oap->سرا.خانہ = curwin->w_cursor.خانہ;
		    oap->سرا.coladd = curwin->w_cursor.coladd;
		    curwin->w_cursor = oap->شروع;
		}
	    }

	    // Break a tab only when it's included in the area.
	    إذا (gchar_pos(&oap->سرا) == '\t'
				     && (صحيح)oap->سرا.coladd < oap->inclusive)
	    {
		// save last line for undo
		إذا (u_save((linenr_T)(oap->سرا.lnum - 1),
				       (linenr_T)(oap->سرا.lnum + 1)) == FAIL)
		    أرجع FAIL;
		curwin->w_cursor = oap->سرا;
		coladvance_force(getviscol2(oap->سرا.خانہ, oap->سرا.coladd));
		oap->سرا = curwin->w_cursor;
		curwin->w_cursor = oap->شروع;
	    }
	    إذا (has_mbyte)
		mb_adjust_opend(oap);
	}

	إذا (oap->سطر_گنتی == 1)	// delete characters within one line
	{
	    إذا (u_save_cursor() == FAIL)	// save line for undo
		أرجع FAIL;

	    // if 'cpoptions' contains '$', display '$' at end of change
	    إذا (       vim_strchr(p_cpo, CPO_DOLLAR) != عدم
		    && oap->op_type == OP_CHANGE
		    && oap->سرا.lnum == curwin->w_cursor.lnum
		    && !oap->is_VIsual)
		display_dollar(oap->سرا.خانہ - !oap->inclusive);

	    n = oap->سرا.خانہ - oap->شروع.خانہ + 1 - !oap->inclusive;

	    إذا (virtual_op)
	    {
		// fix up things for virtualedit-delete:
		// break the tabs which are going to get in our way
		صحيح		len = ml_get_curline_len();

		إذا (oap->سرا.coladd != 0
			&& (صحيح)oap->سرا.خانہ >= len - 1
			&& !(oap->شروع.coladd && (صحيح)oap->سرا.خانہ >= len - 1))
		    n++;
		// Delete at least one char (e.g, when on a control char).
		إذا (n == 0 && oap->شروع.coladd != oap->سرا.coladd)
		    n = 1;

		// When deleted a char in the line, reset coladd.
		إذا (gchar_cursor() != NUL)
		    curwin->w_cursor.coladd = 0;
	    }
	    (فراغ)del_بائٹاں((لما)n, !virtual_op,
			    oap->op_type == OP_DELETE && !oap->is_VIsual);
	}
	وإلا				// delete characters between lines
	{
	    pos_T   curpos;

	    // save deleted and changed lines for undo
	    إذا (u_save((linenr_T)(curwin->w_cursor.lnum - 1),
		 (linenr_T)(curwin->w_cursor.lnum + oap->سطر_گنتی)) == FAIL)
		أرجع FAIL;

	    truncate_سطر(TRUE);	// delete from cursor to end of line

	    curpos = curwin->w_cursor;	// remember curwin->w_cursor
	    ++curwin->w_cursor.lnum;
	    del_سطریں((لما)(oap->سطر_گنتی - 2), FALSE);

	    // delete from start of line until op_end
	    n = (oap->سرا.خانہ + 1 - !oap->inclusive);
	    curwin->w_cursor.خانہ = 0;
	    (فراغ)del_بائٹاں((لما)n, !virtual_op,
			    oap->op_type == OP_DELETE && !oap->is_VIsual);
	    curwin->w_cursor = curpos;	// restore curwin->w_cursor
	    (فراغ)do_join(2, FALSE, FALSE, FALSE, FALSE);
	}
	إذا (oap->op_type == OP_DELETE)
	    auto_format(FALSE, TRUE);
    }

    msgmore(curbuf->b_ml.ml_سطر_گنتی - old_lcount);

setmarks:
    إذا ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	إذا (oap->بلاک_منوال)
	{
	    curbuf->b_op_سرا.lnum = oap->سرا.lnum;
	    curbuf->b_op_سرا.خانہ = oap->شروع.خانہ;
	}
	وإلا
	    curbuf->b_op_سرا = oap->شروع;
	curbuf->b_op_شروع = oap->شروع;
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
    char_u	*سطر;
    char_u	*ptr;

    إذا (!oap->inclusive)
	أرجع;

    سطر = ml_get(oap->سرا.lnum);
    ptr = سطر + oap->سرا.خانہ;
    إذا (*ptr != NUL)
    {
	ptr -= (*mb_سر_off)(سطر, ptr);
	ptr += (*mb_ptr2len)(ptr) - 1;
	oap->سرا.خانہ = ptr - سطر;
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

    State = منوال_REPLACE;
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

    إذا ((curbuf->b_ml.ml_flags & ML_خالی_تھاواں ) || oap->خالی_تھاواں_2)
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

    إذا (u_save((linenr_T)(oap->شروع.lnum - 1),
				       (linenr_T)(oap->سرا.lnum + 1)) == FAIL)
	أرجع FAIL;

    /*
     * block mode replace
     */
    إذا (oap->بلاک_منوال)
    {
	bd.is_MAX = (curwin->w_curswant == MAXCOL);
	لكل ( ; curwin->w_cursor.lnum <= oap->سرا.lnum; ++curwin->w_cursor.lnum)
	{
	    curwin->w_cursor.خانہ = 0;  // make sure cursor position is valid
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
		getvpos(&vpos, oap->شروع_vcol);
		bd.startspaces += vpos.coladd;
		n = bd.startspaces;
	    }
	    وإلا
		// allow for pre spaces
		n = (bd.startspaces ? bd.شروع_char_vcols - 1 : 0);

	    // allow for post spp
	    n += (bd.endspaces
		    && !bd.is_oneChar
		    && bd.سرا_char_vcols > 0) ? bd.سرا_char_vcols - 1 : 0;
	    // Figure out how many characters to replace.
	    numc = oap->سرا_vcol - oap->شروع_vcol + 1;
	    إذا (bd.is_short && (!virtual_op || bd.is_MAX))
		numc -= (oap->سرا_vcol - bd.سرا_vcol) + 1;

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
		appended_سطریں_mark(curwin->w_cursor.lnum, 1L);
		oap->سرا.lnum++;
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
	    oap->شروع.خانہ = 0;
	    curwin->w_cursor.خانہ = 0;
	    oap->سرا.خانہ = ml_get_len(oap->سرا.lnum);
	    إذا (oap->سرا.خانہ)
		--oap->سرا.خانہ;
	}
	وإلا إذا (!oap->inclusive)
	    dec(&(oap->سرا));

	طالما (LTOREQ_POS(curwin->w_cursor, oap->سرا))
	{
	    صحيح پورا = FALSE;

	    n = gchar_cursor();
	    إذا (n != NUL)
	    {
		صحيح new_byte_len = (*mb_char2len)(c);
		صحيح old_byte_len = mb_ptr2len(ml_get_cursor());

		إذا (new_byte_len > 1 || old_byte_len > 1)
		{
		    // This is slow, but it handles replacing a single-byte
		    // with a multi-byte and the other way around.
		    إذا (curwin->w_cursor.lnum == oap->سرا.lnum)
			oap->سرا.خانہ += new_byte_len - old_byte_len;
		    replace_character(c);
		    پورا = TRUE;
		}
		وإلا
		{
		    إذا (n == TAB)
		    {
			صحيح سرا_vcol = 0;

			إذا (curwin->w_cursor.lnum == oap->سرا.lnum)
			{
			    // oap->end has to be recalculated when
			    // the tab breaks
			    سرا_vcol = getviscol2(oap->سرا.خانہ,
							     oap->سرا.coladd);
			}
			coladvance_force(getviscol());
			إذا (curwin->w_cursor.lnum == oap->سرا.lnum)
			    getvpos(&oap->سرا, سرا_vcol);
		    }
		    // with "coladd" set may move to just after a TAB
		    إذا (gchar_cursor() != NUL)
		    {
			PBYTE(curwin->w_cursor, c);
			پورا = TRUE;
		    }
		}
	    }
	    إذا (!پورا && virtual_op && curwin->w_cursor.lnum == oap->سرا.lnum)
	    {
		صحيح virtcols = oap->سرا.coladd;

		إذا (curwin->w_cursor.lnum == oap->شروع.lnum
			&& oap->شروع.خانہ == oap->سرا.خانہ && oap->شروع.coladd)
		    virtcols -= oap->شروع.coladd;

		// oap->end has been trimmed so it's effectively inclusive;
		// as a result an extra +1 must be counted so we don't
		// trample the NUL byte.
		coladvance_force(getviscol2(oap->سرا.خانہ, oap->سرا.coladd) + 1);
		curwin->w_cursor.خانہ -= (virtcols + 1);
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

    curwin->w_cursor = oap->شروع;
    check_cursor();
    بدلیا_سطریں(oap->شروع.lnum, oap->شروع.خانہ, oap->سرا.lnum + 1, 0L);

    إذا ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_شروع = oap->شروع;
	curbuf->b_op_سرا = oap->سرا;
    }

    أرجع OK;
}

ساكن صحيح swapchars(صحيح op_type, pos_T *pos, صحيح لمائی);

/*
 * Handle the (non-standard vi) tilde operator.  Also for "gu", "gU" and "g?".
 */
    ساكن فراغ
op_tilde(oparg_T *oap)
{
    pos_T		pos;
    بنية بلاک_def	bd;
    صحيح			did_change = FALSE;

    إذا (u_save((linenr_T)(oap->شروع.lnum - 1),
				       (linenr_T)(oap->سرا.lnum + 1)) == FAIL)
	أرجع;

    pos = oap->شروع;
    إذا (oap->بلاک_منوال)		    // Visual block mode
    {
	لكل (; pos.lnum <= oap->سرا.lnum; ++pos.lnum)
	{
	    صحيح one_change;

	    بلاک_prep(oap, &bd, pos.lnum, FALSE);
	    pos.خانہ = bd.textcol;
	    one_change = swapchars(oap->op_type, &pos, bd.textlen);
	    did_change |= one_change;

#جے_تعریف_شدہ FEAT_NETBEANS_INTG
	    إذا (netbeans_سرگرم() && one_change)
	    {
		char_u *ptr;

		netbeans_removed(curbuf, pos.lnum, bd.textcol,
							    (لما)bd.textlen);
		// get the line now, it may have been flushed
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, bd.textcol,
						&ptr[bd.textcol], bd.textlen);
	    }
#جے_انت
	}
	إذا (did_change)
	    بدلیا_سطریں(oap->شروع.lnum, 0, oap->سرا.lnum + 1, 0L);
    }
    وإلا				    // not block mode
    {
	إذا (oap->motion_type == MLINE)
	{
	    oap->شروع.خانہ = 0;
	    pos.خانہ = 0;
	    oap->سرا.خانہ = ml_get_len(oap->سرا.lnum);
	    إذا (oap->سرا.خانہ)
		--oap->سرا.خانہ;
	}
	وإلا إذا (!oap->inclusive)
	    dec(&(oap->سرا));

	إذا (pos.lnum == oap->سرا.lnum)
	    did_change = swapchars(oap->op_type, &pos,
						  oap->سرا.خانہ - pos.خانہ + 1);
	وإلا
	    لكل (;;)
	    {
		did_change |= swapchars(oap->op_type, &pos,
			pos.lnum == oap->سرا.lnum ? oap->سرا.خانہ + 1
						  : ml_get_pos_len(&pos));
		إذا (LTOREQ_POS(oap->سرا, pos) || inc(&pos) == -1)
		    اكسر;
	    }
	إذا (did_change)
	{
	    بدلیا_سطریں(oap->شروع.lnum, oap->شروع.خانہ, oap->سرا.lnum + 1,
									  0L);
#جے_تعریف_شدہ FEAT_NETBEANS_INTG
	    إذا (netbeans_سرگرم())
	    {
		char_u *ptr;
		صحيح گنتی;

		pos = oap->شروع;
		طالما (pos.lnum < oap->سرا.lnum)
		{
		    ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		    گنتی = ml_get_buf_len(curbuf, pos.lnum) - pos.خانہ;
		    netbeans_removed(curbuf, pos.lnum, pos.خانہ, (لما)گنتی);
		    // get the line again, it may have been flushed
		    ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		    netbeans_inserted(curbuf, pos.lnum, pos.خانہ,
							&ptr[pos.خانہ], گنتی);
		    pos.خانہ = 0;
		    pos.lnum++;
		}
		گنتی = oap->سرا.خانہ - pos.خانہ + 1;
		netbeans_removed(curbuf, pos.lnum, pos.خانہ, (لما)گنتی);
		// get the line again, it may have been flushed
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, pos.خانہ,
							&ptr[pos.خانہ], گنتی);
	    }
#جے_انت
	}
    }

    إذا (!did_change && oap->is_VIsual)
	// No change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    إذا ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set '[ and '] marks.
	curbuf->b_op_شروع = oap->شروع;
	curbuf->b_op_سرا = oap->سرا;
    }

    إذا (oap->سطر_گنتی > p_report)
	smsg(NGETTEXT("%ld line changed", "%ld lines changed",
					    oap->سطر_گنتی), oap->سطر_گنتی);
}

/*
 * Invoke swapchar() on "length" bytes at position "pos".
 * "pos" is advanced to just after the changed characters.
 * "length" is rounded up to include the whole last multi-byte character.
 * Also works correctly when the number of bytes changes.
 * Returns TRUE if some character was changed.
 */
    ساكن صحيح
swapchars(صحيح op_type, pos_T *pos, صحيح لمائی)
{
    صحيح todo;
    صحيح	did_change = 0;

    لكل (todo = لمائی; todo > 0; --todo)
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
    إذا ((op_type == OP_اتلا || op_type == OP_NOP || op_type == OP_TILDE)
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
	وإلا إذا (op_type != OP_ہیٹھلا)
	    nc = MB_TOUPPER(c);
    }
    وإلا إذا (MB_ISUPPER(c))
    {
	إذا (op_type == OP_ROT13)
	    nc = ROT13(c, 'A');
	وإلا إذا (op_type != OP_اتلا)
	    nc = MB_TOLOWER(c);
    }
    إذا (nc != c)
    {
	إذا (enc_utf8 && (c >= 0x80 || nc >= 0x80))
	{
	    pos_T   sp = curwin->w_cursor;

	    curwin->w_cursor = *pos;
	    // don't use del_char(), it also removes composing chars
	    del_بائٹاں(utf_ptr2len(ml_get_cursor()), FALSE, FALSE);
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
op_داخل_کرو(oparg_T *oap, لما count1)
{
    لما		pre_textlen = 0;
    colnr_T		ind_pre_خانہ = 0, ind_post_خانہ;
    صحيح			ind_pre_vcol = 0, ind_post_vcol = 0;
    بنية بلاک_def	bd;
    صحيح			i;
    pos_T		t1;
    pos_T		شروع_داخل_کرو;

    // edit() changes this - record it for OP_APPEND
    bd.is_MAX = (curwin->w_curswant == MAXCOL);

    // vis block is still marked. Get rid of it now.
    curwin->w_cursor.lnum = oap->شروع.lnum;
    update_screen(UPD_INVERTED);

    إذا (oap->بلاک_منوال)
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
					   ? oap->سرا_vcol + 1 : getviscol());
	    إذا (oap->op_type == OP_APPEND)
		--curwin->w_cursor.خانہ;
	    curwin->w_ve_flags = old_ve_flags;
	}
	// Get the info about the block before entering the text
	بلاک_prep(oap, &bd, oap->شروع.lnum, TRUE);
	// Get indent information
	ind_pre_خانہ = (colnr_T)getwhitecols_curline();
	ind_pre_vcol = get_indent();
	pre_textlen = ml_get_len(oap->شروع.lnum) - bd.textcol;
	إذا (oap->op_type == OP_APPEND)
	    pre_textlen -= bd.textlen;
    }

    إذا (oap->op_type == OP_APPEND)
    {
	إذا (oap->بلاک_منوال && curwin->w_cursor.coladd == 0)
	{
	    // Move the cursor to the character right of the block.
	    curwin->w_سیٹ_curswant = true;
	    طالما (*ml_get_cursor() != NUL
		    && (curwin->w_cursor.خانہ < bd.textcol + bd.textlen))
		++curwin->w_cursor.خانہ;
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
	    curwin->w_cursor = oap->سرا;
	    check_cursor_خانہ();

	    // Works just like an 'i'nsert on the next character.
	    إذا (!LINEEMPTY(curwin->w_cursor.lnum)
		    && oap->شروع_vcol != oap->سرا_vcol)
		inc_cursor();
	}
    }

    t1 = oap->شروع;
    شروع_داخل_کرو = curwin->w_cursor;
    (فراغ)edit(NUL, FALSE, (linenr_T)count1);

    // When a tab was inserted, and the characters in front of the tab
    // have been converted to a tab as well, the column of the cursor
    // might have actually been reduced, so need to adjust here.
    إذا (t1.lnum == curbuf->b_op_شروع_orig.lnum
	    && LT_POS(curbuf->b_op_شروع_orig, t1))
	oap->شروع = curbuf->b_op_شروع_orig;

    // If user has moved off this line, we don't know what to do, so do
    // nothing.
    // Also don't repeat the insert when Insert mode ended with CTRL-C.
    إذا (curwin->w_cursor.lnum != oap->شروع.lnum || got_int)
	أرجع;

    إذا (oap->بلاک_منوال)
    {
	صحيح			ins_len;
	char_u			*firstline, *ins_متن;
	بنية بلاک_def	bd2;
	صحيح			did_indent = FALSE;
	size_t			len;
	size_t			جوڑو;
	// offset when cursor was moved in insert mode
	صحيح			وتھ = 0;

	// If indent kicked in, the firstline might have changed
	// but only do that, if the indent actually increased.
	ind_post_خانہ = (colnr_T)getwhitecols_curline();
	إذا (curbuf->b_op_شروع.خانہ > ind_pre_خانہ && ind_post_خانہ > ind_pre_خانہ)
	{
	    bd.textcol += ind_post_خانہ - ind_pre_خانہ;
	    ind_post_vcol = get_indent();
	    bd.شروع_vcol += ind_post_vcol - ind_pre_vcol;
	    did_indent = TRUE;
	}

	// The user may have moved the cursor before inserting something, try
	// to adjust the block for that.  But only do it, if the difference
	// does not come from indent kicking in.
	إذا (oap->شروع.lnum == curbuf->b_op_شروع_orig.lnum
						  && !bd.is_MAX && !did_indent)
	{
	    صحيح t = getviscol2(curbuf->b_op_شروع_orig.خانہ,
					       curbuf->b_op_شروع_orig.coladd);

	    إذا (oap->op_type == OP_داخل_کرو
		    && oap->شروع.خانہ + oap->شروع.coladd
			    != curbuf->b_op_شروع_orig.خانہ
					      + curbuf->b_op_شروع_orig.coladd)
	    {
		oap->شروع.خانہ = curbuf->b_op_شروع_orig.خانہ;
		pre_textlen -= t - oap->شروع_vcol;
		oap->شروع_vcol = t;
	    }
	    وإلا إذا (oap->op_type == OP_APPEND
		    && oap->شروع.خانہ + oap->شروع.coladd
			    >= curbuf->b_op_شروع_orig.خانہ
					      + curbuf->b_op_شروع_orig.coladd)
	    {
		oap->شروع.خانہ = curbuf->b_op_شروع_orig.خانہ;
		// reset pre_textlen to the value of OP_INSERT
		pre_textlen += bd.textlen;
		pre_textlen -= t - oap->شروع_vcol;
		oap->شروع_vcol = t;
		oap->op_type = OP_داخل_کرو;
	    }
	}

	// Spaces and tabs in the indent may have changed to other spaces and
	// tabs.  Get the starting column again and correct the length.
	// Don't do this when "$" used, end-of-line will have changed.
	//
	// if indent was added and the inserted text was after the indent,
	// correct the selection for the new indent.
	إذا (did_indent && bd.textcol - ind_post_خانہ > 0)
	{
	    oap->شروع.خانہ += ind_post_خانہ - ind_pre_خانہ;
	    oap->شروع_vcol += ind_post_vcol - ind_pre_vcol;
	    oap->سرا.خانہ += ind_post_خانہ - ind_pre_خانہ;
	    oap->سرا_vcol += ind_post_vcol - ind_pre_vcol;
	}
	بلاک_prep(oap, &bd2, oap->شروع.lnum, TRUE);
	إذا (did_indent && bd.textcol - ind_post_خانہ > 0)
	{
	    // undo for where "oap" is used below
	    oap->شروع.خانہ -= ind_post_خانہ - ind_pre_خانہ;
	    oap->شروع_vcol -= ind_post_vcol - ind_pre_vcol;
	    oap->سرا.خانہ -= ind_post_خانہ - ind_pre_خانہ;
	    oap->سرا_vcol -= ind_post_vcol - ind_pre_vcol;
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
	firstline = ml_get(oap->شروع.lnum);
	len = ml_get_len(oap->شروع.lnum);
	جوڑو = bd.textcol;
	إذا (oap->op_type == OP_APPEND)
	{
	    جوڑو += bd.textlen;
	    // account for pressing cursor in insert mode when '$' was used
	    إذا (bd.is_MAX
		&& (شروع_داخل_کرو.lnum == Insstart.lnum
					   && شروع_داخل_کرو.خانہ > Insstart.خانہ))
	    {
		وتھ = (شروع_داخل_کرو.خانہ - Insstart.خانہ);
		جوڑو -= وتھ;
		إذا (oap->سرا_vcol > وتھ)
		    oap->سرا_vcol -= (وتھ + 1);
		وإلا
		    // moved outside of the visual block, what to do?
		    أرجع;
	    }
	}
	إذا (جوڑو > len)
	    جوڑو = len;  // short line, point to the NUL
	firstline += جوڑو;
	len -= جوڑو;
	إذا (pre_textlen >= 0 && (ins_len = (صحيح)len - pre_textlen - وتھ) > 0)
	{
	    ins_متن = vim_strnsave(firstline, ins_len);
	    إذا (ins_متن != عدم)
	    {
		// block handled here
		إذا (u_save(oap->شروع.lnum,
					 (linenr_T)(oap->سرا.lnum + 1)) == OK)
		    بلاک_داخل_کرو(oap, ins_متن, ins_len, (oap->op_type == OP_داخل_کرو),
									 &bd);

		curwin->w_cursor.خانہ = oap->شروع.خانہ;
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
    لما		pre_textlen = 0;
    لما		pre_indent = 0;
    char_u		*firstline;
    char_u		*ins_متن, *newp, *oldp;
    بنية بلاک_def	bd;

    l = oap->شروع.خانہ;
    إذا (oap->motion_type == MLINE)
    {
	l = 0;
	can_si = may_do_si();	// Like opening a new line, do smart indent
    }

    // First delete the text in the region.  In an empty buffer only need to
    // save for undo
    إذا (curbuf->b_ml.ml_flags & ML_خالی_تھاواں)
    {
	إذا (u_save_cursor() == FAIL)
	    أرجع FALSE;
    }
    وإلا إذا (op_delete(oap) == FAIL)
	أرجع FALSE;

    إذا ((l > curwin->w_cursor.خانہ) && !LINEEMPTY(curwin->w_cursor.lnum)
							 && !virtual_op)
	inc_cursor();

    // check for still on same line (<CR> in inserted text meaningless)
    // skip blank lines too
    إذا (oap->بلاک_منوال)
    {
	// Add spaces before getting the current line length.
	إذا (virtual_op && (curwin->w_cursor.coladd > 0
						    || gchar_cursor() == NUL))
	    coladvance_force(getviscol());
	firstline = ml_get(oap->شروع.lnum);
	pre_textlen = ml_get_len(oap->شروع.lnum);
	pre_indent = (لما)getwhitecols(firstline);
	bd.textcol = curwin->w_cursor.خانہ;
    }

    إذا (oap->motion_type == MLINE)
	fix_indent();

    // Reset finish_op now, don't want it set inside edit().
    صحيح save_انت_op = انت_op;
    انت_op = FALSE;

    retval = edit(NUL, FALSE, (linenr_T)1);

    انت_op = save_انت_op;

    /*
     * In Visual block mode, handle copying the new text to all lines of the
     * block.
     * Don't repeat the insert when Insert mode ended with CTRL-C.
     */
    إذا (oap->بلاک_منوال && oap->شروع.lnum != oap->سرا.lnum && !got_int)
    {
	صحيح	ins_len;

	// Auto-indenting may have changed the indent.  If the cursor was past
	// the indent, exclude that indent change from the inserted text.
	firstline = ml_get(oap->شروع.lnum);
	إذا (bd.textcol > (colnr_T)pre_indent)
	{
	    لما new_indent = (لما)getwhitecols(firstline);

	    pre_textlen += new_indent - pre_indent;
	    bd.textcol += new_indent - pre_indent;
	}

	ins_len = (صحيح)ml_get_len(oap->شروع.lnum) - pre_textlen;
	إذا (ins_len > 0)
	{
	    // Subsequent calls to ml_get() flush the firstline data - take a
	    // copy of the inserted text.
	    إذا ((ins_متن = alloc(ins_len + 1)) != عدم)
	    {
		vim_strncpy(ins_متن, firstline + bd.textcol, ins_len);
		لكل (linenr = oap->شروع.lnum + 1; linenr <= oap->سرا.lnum;
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
			    (فراغ)getvpos(&vpos, oap->شروع_vcol);
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
#جے_تعریف_شدہ FEAT_PROP_POPUP
			// Shift the properties for linenr as edit() would do.
			إذا (curbuf->b_has_textprop)
			    adjust_prop_columns(linenr, bd.textcol,
						     vpos.coladd + (صحيح)ins_len, 0);
#جے_انت
		    }
		}
		check_cursor();

		بدلیا_سطریں(oap->شروع.lnum + 1, 0, oap->سرا.lnum + 1, 0L);
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
    بنانشانی صحيح cur_ve_flags = get_ve_flags();

    صحيح adj_cursor = (curwin->w_cursor.خانہ > 0
				&& gchar_cursor() == NUL
				&& (cur_ve_flags & VE_ONEMORE) == 0
				&& (cur_ve_flags & VE_ALL) == 0
				&& !(restart_edit || (State & منوال_داخل_کرو)));
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
    char_u   *سطر,
    صحيح      process,
    صحيح	     include_space,
    صحيح      *is_comment)
{
    char_u *comment_flags = عدم;
    صحيح    lead_len;
    صحيح    leader_وتھ = get_آخری_leader_وتھ(سطر, &comment_flags);

    *is_comment = FALSE;
    إذا (leader_وتھ != -1)
    {
	// Let's check whether the line ends with an unclosed comment.
	// If the last comment leader has COM_END in flags, there's no comment.
	طالما (*comment_flags)
	{
	    إذا (*comment_flags == COM_سرا
		    || *comment_flags == ':')
		اكسر;
	    ++comment_flags;
	}
	إذا (*comment_flags != COM_سرا)
	    *is_comment = TRUE;
    }

    إذا (process == FALSE)
	أرجع سطر;

    lead_len = get_leader_len(سطر, &comment_flags, FALSE, include_space);

    إذا (lead_len == 0)
	أرجع سطر;

    // Find:
    // - COM_END,
    // - colon,
    // whichever comes first.
    طالما (*comment_flags)
    {
	إذا (*comment_flags == COM_سرا
		|| *comment_flags == ':')
	    اكسر;
	++comment_flags;
    }

    // If we found a colon, it means that we are not processing a line
    // starting with a closing part of a three-part comment. That's good,
    // because we don't want to remove those as this would be annoying.
    إذا (*comment_flags == ':' || *comment_flags == NUL)
	سطر += lead_len;

    أرجع سطر;
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
    لما    گنتی,
    صحيح	    داخل_کرو_space,
    صحيح	    save_undo,
    صحيح	    use_formatoptions بن_ورتیا,
    صحيح	    setmark)
{
    char_u	*curr = عدم;
    char_u      *curr_شروع = عدم;
    char_u	*cend;
    char_u	*newp;
    size_t	newp_len;
    char_u	*spaces;	// number of spaces inserted before a line
    صحيح		endcurr1 = NUL;
    صحيح		endcurr2 = NUL;
    صحيح		currsize = 0;	// size of the current line
    صحيح		sumsize = 0;	// size of the long new line
    linenr_T	t;
    colnr_T	خانہ = 0;
    صحيح		ret = OK;
    صحيح		*comments = عدم;
    صحيح		remove_comments = (use_formatoptions == TRUE)
				  && has_format_option(FO_REMOVE_COMS);
    صحيح		prev_was_comment;

    إذا (save_undo && u_save((linenr_T)(curwin->w_cursor.lnum - 1),
			    (linenr_T)(curwin->w_cursor.lnum + گنتی)) == FAIL)
	أرجع FAIL;

    // Allocate an array to store the number of spaces inserted before each
    // line.  We will use it to pre-compute the length of the new line and the
    // proper placement of each original line in the new one.
    spaces = lalloc_clear(گنتی, TRUE);
    إذا (spaces == عدم)
	أرجع FAIL;
    إذا (remove_comments)
    {
	comments = lalloc_clear(گنتی * حجم(صحيح), TRUE);
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
    لكل (t = 0; t < گنتی; ++t)
    {
	curr = curr_شروع = ml_get((linenr_T)(curwin->w_cursor.lnum + t));
	إذا (t == 0 && setmark && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set the '[ mark.
	    curwin->w_بفر->b_op_شروع.lnum = curwin->w_cursor.lnum;
	    curwin->w_بفر->b_op_شروع.خانہ  = (colnr_T)STRLEN(curr);
	}
	إذا (remove_comments)
	{
	    // We don't want to remove the comment leader if the
	    // previous line is not a comment.
	    إذا (t > 0 && prev_was_comment)
	    {

		char_u *new_curr = skip_comment(curr, TRUE, داخل_کرو_space,
							   &prev_was_comment);
		comments[t] = (صحيح)(new_curr - curr);
		curr = new_curr;
	    }
	    وإلا
		curr = skip_comment(curr, FALSE, داخل_کرو_space,
							   &prev_was_comment);
	}

	إذا (داخل_کرو_space && t > 0)
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
	إذا (داخل_کرو_space && currsize > 0)
	{
	    إذا (has_mbyte)
	    {
		cend = curr + currsize;
		MB_PTR_پٹھ(curr, cend);
		endcurr1 = (*mb_ptr2char)(cend);
		إذا (cend > curr)
		{
		    MB_PTR_پٹھ(curr, cend);
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
	سطر_breakcheck();
	إذا (got_int)
	{
	    ret = FAIL;
	    اذهب theend;
	}
    }

    // store the column position before last line
    خانہ = sumsize - currsize - spaces[گنتی - 1];

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
#جے_تعریف_شدہ FEAT_PROP_POPUP
    unpacked_memline_T um = um_open_at_no_props(
				curwin->w_بفر, curwin->w_cursor.lnum, 0);
    // um_open_at_no_props may have invalidated "curr".
    صحيح curr_off = (صحيح)(curr - curr_شروع);

    curr = curr_شروع = ml_get((linenr_T)(curwin->w_cursor.lnum + گنتی - 1));
    curr += curr_off;
#جے_انت

    لكل (t = گنتی - 1; ; --t)
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
	spaces_removed = (curr - curr_شروع) - spaces[t];

	mark_خانہ_adjust(curwin->w_cursor.lnum + t, (colnr_T)0, -t,
			 (لما)(cend - newp - spaces_removed), spaces_removed);
#جے_تعریف_شدہ FEAT_PROP_POPUP
	prepend_joined_props(&um, curwin->w_cursor.lnum + t,
		t == گنتی - 1, (لما)(cend - newp), spaces_removed);
#جے_انت
	إذا (t == 0)
	    اكسر;
	curr = curr_شروع = ml_get((linenr_T)(curwin->w_cursor.lnum + t - 1));
	إذا (remove_comments)
	    curr += comments[t - 1];
	إذا (داخل_کرو_space && t > 1)
	    curr = skipwhite(curr);
	currsize = (صحيح)STRLEN(curr);
    }

#جے_تعریف_شدہ FEAT_PROP_POPUP
    إذا (um.buf != عدم)
    {
	um_سیٹ_متن(&um, newp);
	um_الٹ_props(&um);
	um_close(&um);
	newp = عدم;  // um_set_text took ownership
    }
    وإلا
#جے_انت
    ml_replace_len(curwin->w_cursor.lnum, newp, (colnr_T)newp_len, TRUE, FALSE);

    إذا (setmark && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set the '] mark.
	curwin->w_بفر->b_op_سرا.lnum = curwin->w_cursor.lnum;
	curwin->w_بفر->b_op_سرا.خانہ  = (colnr_T)sumsize;
    }

    // Only report the change in the first line here, del_lines() will report
    // the deleted line.
    بدلیا_سطریں(curwin->w_cursor.lnum, currsize,
					       curwin->w_cursor.lnum + 1, 0L);
    /*
     * Delete following lines. To do this we move the cursor there
     * briefly, and then move it back. After del_lines() the cursor may
     * have moved up (last line deleted), so the current lnum is kept in t.
     */
    t = curwin->w_cursor.lnum;
    ++curwin->w_cursor.lnum;
    del_سطریں(گنتی - 1, FALSE);
    curwin->w_cursor.lnum = t;

    /*
     * Set the cursor column:
     * Vi compatible: use the column of the first join
     * vim:	      use the column of the last join
     */
    curwin->w_cursor.خانہ =
		    (vim_strchr(p_cpo, CPO_JOINCOL) != عدم ? currsize : خانہ);
    check_cursor_خانہ();

    curwin->w_cursor.coladd = 0;
    curwin->w_سیٹ_curswant = true;

theend:
    vim_free(spaces);
    إذا (remove_comments)
	vim_free(comments);
    أرجع ret;
}

#جے_تعریف_شدہ FEAT_LINEBREAK
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
#جے_انت

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
    char_u	*سطر;
    char_u	*prev_pstart;
    char_u	*prev_pend;
    chartabsize_T cts;
#جے_تعریف_شدہ FEAT_LINEBREAK
		// Avoid a problem with unwanted linebreaks in block mode.
    صحيح		lbr_saved = reset_lbr();
#جے_انت

    bdp->startspaces = 0;
    bdp->endspaces = 0;
    bdp->textlen = 0;
    bdp->شروع_vcol = 0;
    bdp->سرا_vcol = 0;
    bdp->is_short = FALSE;
    bdp->is_oneChar = FALSE;
    bdp->pre_whitesp = 0;
    bdp->pre_whitesp_c = 0;
    bdp->سرا_char_vcols = 0;
    bdp->شروع_char_vcols = 0;

    سطر = ml_get(lnum);
    prev_pstart = سطر;
    init_chartabsize_arg(&cts, curwin, lnum, bdp->شروع_vcol, سطر, سطر);
    طالما (cts.cts_vcol < oap->شروع_vcol && *cts.cts_ptr != NUL)
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
    bdp->شروع_vcol = cts.cts_vcol;
    pstart = cts.cts_ptr;
    clear_chartabsize_arg(&cts);

    bdp->شروع_char_vcols = incr;
    إذا (bdp->شروع_vcol < oap->شروع_vcol)	// line too short
    {
	bdp->سرا_vcol = bdp->شروع_vcol;
	bdp->is_short = TRUE;
	إذا (!is_del || oap->op_type == OP_APPEND)
	    bdp->endspaces = oap->سرا_vcol - oap->شروع_vcol + 1;
    }
    وإلا
    {
	// notice: this converts partly selected Multibyte characters to
	// spaces, too.
	bdp->startspaces = bdp->شروع_vcol - oap->شروع_vcol;
	إذا (is_del && bdp->startspaces)
	    bdp->startspaces = bdp->شروع_char_vcols - bdp->startspaces;
	pend = pstart;
	bdp->سرا_vcol = bdp->شروع_vcol;
	إذا (bdp->سرا_vcol > oap->سرا_vcol)	// it's all in one character
	{
	    bdp->is_oneChar = TRUE;
	    إذا (oap->op_type == OP_داخل_کرو)
		bdp->endspaces = bdp->شروع_char_vcols - bdp->startspaces;
	    وإلا إذا (oap->op_type == OP_APPEND)
	    {
		bdp->startspaces += oap->سرا_vcol - oap->شروع_vcol + 1;
		bdp->endspaces = bdp->شروع_char_vcols - bdp->startspaces;
	    }
	    وإلا
	    {
		bdp->startspaces = oap->سرا_vcol - oap->شروع_vcol + 1;
		إذا (is_del && oap->op_type != OP_LSHIFT)
		{
		    // just putting the sum of those two into
		    // bdp->startspaces doesn't work for Visual replace,
		    // so we have to split the tab in two
		    bdp->startspaces = bdp->شروع_char_vcols
					- (bdp->شروع_vcol - oap->شروع_vcol);
		    bdp->endspaces = bdp->سرا_vcol - oap->سرا_vcol - 1;
		}
	    }
	}
	وإلا
	{
	    init_chartabsize_arg(&cts, curwin, lnum, bdp->سرا_vcol,
								  سطر, pend);
	    prev_pend = pend;
	    طالما (cts.cts_vcol <= oap->سرا_vcol && *cts.cts_ptr != NUL)
	    {
		// count a tab for what it's worth (if list mode not on)
		prev_pend = cts.cts_ptr;
		incr = lbr_chartabsize_adv(&cts);
		cts.cts_vcol += incr;
	    }
	    bdp->سرا_vcol = cts.cts_vcol;
	    pend = cts.cts_ptr;
	    clear_chartabsize_arg(&cts);

	    إذا (bdp->سرا_vcol <= oap->سرا_vcol
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
		    bdp->endspaces = oap->سرا_vcol - bdp->سرا_vcol
							     + oap->inclusive;
		وإلا
		    bdp->endspaces = 0; // replace doesn't add characters
	    }
	    وإلا إذا (bdp->سرا_vcol > oap->سرا_vcol)
	    {
		bdp->endspaces = bdp->سرا_vcol - oap->سرا_vcol - 1;
		إذا (!is_del && bdp->endspaces)
		{
		    bdp->endspaces = incr - bdp->endspaces;
		    إذا (pend != pstart)
			pend = prev_pend;
		}
	    }
	}
	bdp->سرا_char_vcols = incr;
	إذا (is_del && bdp->startspaces)
	    pstart = prev_pstart;
	bdp->textlen = (صحيح)(pend - pstart);
    }
    bdp->textcol = (colnr_T) (pstart - سطر);
    bdp->textstart = pstart;
#جے_تعریف_شدہ FEAT_LINEBREAK
    restore_lbr(lbr_saved);
#جے_انت
}

/*
 * Get block text from "start" to "end"
 */
    فراغ
charwise_بلاک_prep(
    pos_T		شروع,
    pos_T		سرا,
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
    bdp->شروع_char_vcols = 0;

    إذا (lnum == شروع.lnum)
    {
	startcol = شروع.خانہ;
	إذا (virtual_op)
	{
	    getvcol(curwin, &شروع, &cs, عدم, &ce, 0);
	    إذا (ce != cs && شروع.coladd > 0)
	    {
		// Part of a tab selected -- but don't
		// double-count it.
		bdp->شروع_char_vcols = ce - cs + 1;
		bdp->startspaces = bdp->شروع_char_vcols - شروع.coladd;
		إذا (bdp->startspaces < 0)
		    bdp->startspaces = 0;
		startcol++;
	    }
	}
    }

    إذا (lnum == سرا.lnum)
    {
	endcol = سرا.خانہ;
	إذا (virtual_op)
	{
	    getvcol(curwin, &سرا, &cs, عدم, &ce, 0);
	    إذا (p[endcol] == NUL || (cs + سرا.coladd < ce
			// Don't add space for double-wide
			// char; endcol will be on last byte
			// of multi-byte char.
			&& (*mb_سر_off)(p, p + endcol) == 0))
	    {
		إذا (شروع.lnum == سرا.lnum && شروع.خانہ == سرا.خانہ)
		{
		    // Special case: inside a single char
		    bdp->is_oneChar = TRUE;
		    bdp->startspaces = سرا.coladd - شروع.coladd + inclusive;
		    endcol = startcol;
		}
		وإلا
		{
		    bdp->endspaces = سرا.coladd + inclusive;
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
#جے_تعریف_شدہ FEAT_FOLDING
    disable_fold_update++;
#جے_انت

    إذا (!VIsual_سرگرم)
    {
	pos = curwin->w_cursor;
	إذا (u_save_cursor() == FAIL)
	{
#جے_تعریف_شدہ FEAT_FOLDING
	    disable_fold_update--;
#جے_انت
	    أرجع;
	}
	change_cnt = do_addsub(oap->op_type, &pos, 0, amount);
#جے_تعریف_شدہ FEAT_FOLDING
	disable_fold_update--;
#جے_انت
	إذا (change_cnt)
	    بدلیا_سطریں(pos.lnum, 0, pos.lnum + 1, 0L);
    }
    وإلا
    {
	صحيح	one_change;
	صحيح	لمائی;
	pos_T	startpos;

	إذا (u_save((linenr_T)(oap->شروع.lnum - 1),
					(linenr_T)(oap->سرا.lnum + 1)) == FAIL)
	{
#جے_تعریف_شدہ FEAT_FOLDING
	    disable_fold_update--;
#جے_انت
	    أرجع;
	}

	pos = oap->شروع;
	لكل (; pos.lnum <= oap->سرا.lnum; ++pos.lnum)
	{
	    إذا (oap->بلاک_منوال)		    // Visual block mode
	    {
		بلاک_prep(oap, &bd, pos.lnum, FALSE);
		pos.خانہ = bd.textcol;
		لمائی = bd.textlen;
	    }
	    وإلا إذا (oap->motion_type == MLINE)
	    {
		curwin->w_cursor.خانہ = 0;
		pos.خانہ = 0;
		لمائی = ml_get_len(pos.lnum);
	    }
	    وإلا // oap->motion_type == MCHAR
	    {
		إذا (pos.lnum == oap->شروع.lnum && !oap->inclusive)
		    dec(&(oap->سرا));
		لمائی = ml_get_len(pos.lnum);
		pos.خانہ = 0;
		إذا (pos.lnum == oap->شروع.lnum)
		{
		    pos.خانہ += oap->شروع.خانہ;
		    لمائی -= oap->شروع.خانہ;
		}
		إذا (pos.lnum == oap->سرا.lnum)
		{
		    لمائی = ml_get_len(oap->سرا.lnum);
		    إذا (oap->سرا.خانہ >= لمائی)
			oap->سرا.خانہ = لمائی - 1;
		    لمائی = oap->سرا.خانہ - pos.خانہ + 1;
		}
	    }
	    one_change = do_addsub(oap->op_type, &pos, لمائی, amount);
	    إذا (one_change)
	    {
		// Remember the start position of the first change.
		إذا (change_cnt == 0)
		    startpos = curbuf->b_op_شروع;
		++change_cnt;
	    }

#جے_تعریف_شدہ FEAT_NETBEANS_INTG
	    إذا (netbeans_سرگرم() && one_change)
	    {
		char_u *ptr;

		netbeans_removed(curbuf, pos.lnum, pos.خانہ, (لما)لمائی);
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, pos.خانہ,
						&ptr[pos.خانہ], لمائی);
	    }
#جے_انت
	    إذا (g_cmd && one_change)
		amount += Prenum1;
	}

#جے_تعریف_شدہ FEAT_FOLDING
	disable_fold_update--;
#جے_انت
	إذا (change_cnt)
	    بدلیا_سطریں(oap->شروع.lnum, 0, oap->سرا.lnum + 1, 0L);

	إذا (!change_cnt && oap->is_VIsual)
	    // No change: need to remove the Visual selection
	    redraw_curbuf_later(UPD_INVERTED);

	// Set '[ mark if something changed. Keep the last end
	// position from do_addsub().
	إذا (change_cnt > 0 && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	    curbuf->b_op_شروع = startpos;

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
    صحيح		لمائی,
    linenr_T	Prenum1)
{
    صحيح		خانہ;
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
    صحيح		گھٹاؤ;
    صحيح		negative = FALSE;
    صحيح		was_مثبت = TRUE;
    صحيح		visual = VIsual_سرگرم;
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

    إذا (virtual_سرگرم())
    {
	save_coladd = pos->coladd;
	pos->coladd = 0;
    }

    curwin->w_cursor = *pos;
    ptr = ml_get(pos->lnum);
    linelen = ml_get_len(pos->lnum);
    خانہ = pos->خانہ;

    إذا (خانہ + !!save_coladd >= linelen)
	اذهب theend;

    /*
     * First check if we are on a hexadecimal number, after the "0x".
     */
    إذا (!VIsual_سرگرم)
    {
	إذا (do_bin)
	    طالما (خانہ > 0 && vim_isbdigit(ptr[خانہ]))
	    {
		--خانہ;
		إذا (has_mbyte)
		    خانہ -= (*mb_سر_off)(ptr, ptr + خانہ);
	    }

	إذا (do_hex)
	    طالما (خانہ > 0 && vim_isxdigit(ptr[خانہ]))
	    {
		--خانہ;
		إذا (has_mbyte)
		    خانہ -= (*mb_سر_off)(ptr, ptr + خانہ);
	    }

	إذا (       do_bin
		&& do_hex
		&& ! ((خانہ > 0
		    && (ptr[خانہ] == 'X'
			|| ptr[خانہ] == 'x')
		    && ptr[خانہ - 1] == '0'
		    && (!has_mbyte ||
			!(*mb_سر_off)(ptr, ptr + خانہ - 1))
		    && vim_isxdigit(ptr[خانہ + 1]))))
	{

	    // In case of binary/hexadecimal pattern overlap match, rescan

	    خانہ = pos->خانہ;

	    طالما (خانہ > 0 && vim_isdigit(ptr[خانہ]))
	    {
		خانہ--;
		إذا (has_mbyte)
		    خانہ -= (*mb_سر_off)(ptr, ptr + خانہ);
	    }
	}

	إذا ((       do_hex
		&& خانہ > 0
		&& (ptr[خانہ] == 'X'
		    || ptr[خانہ] == 'x')
		&& ptr[خانہ - 1] == '0'
		&& (!has_mbyte ||
		    !(*mb_سر_off)(ptr, ptr + خانہ - 1))
		&& vim_isxdigit(ptr[خانہ + 1])) ||
	    (       do_bin
		&& خانہ > 0
		&& (ptr[خانہ] == 'B'
		    || ptr[خانہ] == 'b')
		&& ptr[خانہ - 1] == '0'
		&& (!has_mbyte ||
		    !(*mb_سر_off)(ptr, ptr + خانہ - 1))
		&& vim_isbdigit(ptr[خانہ + 1])))
	{
	    // Found hexadecimal or binary number, move to its start.
	    --خانہ;
	    إذا (has_mbyte)
		خانہ -= (*mb_سر_off)(ptr, ptr + خانہ);
	}
	وإلا
	{
	    /*
	     * Search forward and then backward to find the start of number.
	     */
	    خانہ = pos->خانہ;

	    طالما (ptr[خانہ] != NUL
		    && !vim_isdigit(ptr[خانہ])
		    && !(do_alpha && ASCII_ISALPHA(ptr[خانہ])))
		خانہ += mb_ptr2len(ptr + خانہ);

	    طالما (خانہ > 0
		    && vim_isdigit(ptr[خانہ - 1])
		    && !(do_alpha && ASCII_ISALPHA(ptr[خانہ])))
	    {
		--خانہ;
		إذا (has_mbyte)
		    خانہ -= (*mb_سر_off)(ptr, ptr + خانہ);
	    }
	}
    }

    إذا (visual)
    {
	طالما (ptr[خانہ] != NUL && لمائی > 0
		&& !vim_isdigit(ptr[خانہ])
		&& !(do_alpha && ASCII_ISALPHA(ptr[خانہ])))
	{
	    صحيح mb_len = mb_ptr2len(ptr + خانہ);

	    خانہ += mb_len;
	    لمائی -= mb_len;
	}

	إذا (لمائی == 0)
	    اذهب theend;

	إذا (خانہ > pos->خانہ && ptr[خانہ - 1] == '-'
		&& (!has_mbyte || !(*mb_سر_off)(ptr, ptr + خانہ - 1))
		&& !do_unsigned)
	{
	    إذا (do_blank && خانہ >= 2 && !VIM_ISWHITE(ptr[خانہ - 2]))
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
    firstdigit = ptr[خانہ];
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
		إذا (محفوظ_isupper(firstdigit))
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
		إذا (محفوظ_isupper(firstdigit))
		    firstdigit = 'Z';
		وإلا
		    firstdigit = 'z';
	    }
	    وإلا
		firstdigit += Prenum1;
	}
	curwin->w_cursor.خانہ = خانہ;
	إذا (!did_change)
	    startpos = curwin->w_cursor;
	did_change = TRUE;
	(فراغ)del_char(FALSE);
	ins_char(firstdigit);
	endpos = curwin->w_cursor;
	curwin->w_cursor.خانہ = خانہ;
    }
    وإلا
    {
	char_u	*buf1;
	صحيح	buf1len;
	char_u	buf2[NUMBUFLEN];
	صحيح	buf2len;
	pos_T	save_pos;
	صحيح	i;

	إذا (خانہ > 0 && ptr[خانہ - 1] == '-'
		&& (!has_mbyte ||
		    !(*mb_سر_off)(ptr, ptr + خانہ - 1))
		&& !visual
		&& !do_unsigned)
	{
	    إذا (do_blank && خانہ >= 2 && !VIM_ISWHITE(ptr[خانہ - 2]))
		blank_unsigned = TRUE;
	    وإلا
	    {
		// negative number
		--خانہ;
		negative = TRUE;
	    }
	}

	// get the number value (unsigned)
	إذا (visual && VIsual_منوال != 'V')
	    maxlen = (curbuf->b_visual.vi_curswant == MAXCOL
		    ? linelen - خانہ : لمائی);

	صحيح حد_توں_ودھ = FALSE;
	vim_str2nr(ptr + خانہ, &pre, &لمائی,
		0 + (do_bin ? STR2NR_BIN : 0)
		    + (do_oct ? STR2NR_OCT : 0)
		    + (do_hex ? STR2NR_HEX : 0),
		عدم, &n, maxlen, FALSE, &حد_توں_ودھ);

	// ignore leading '-' for hex and octal and bin numbers
	إذا (pre && negative)
	{
	    ++خانہ;
	    --لمائی;
	    negative = FALSE;
	}
	// add or subtract
	گھٹاؤ = FALSE;
	إذا (op_type == OP_NR_SUB)
	    گھٹاؤ ^= TRUE;
	إذا (negative)
	    گھٹاؤ ^= TRUE;

	oldn = n;
	إذا (!حد_توں_ودھ)  // if number is too big don't add/subtract
	{
	    إذا (گھٹاؤ)
		n -= (uvarnumber_T)Prenum1;
	    وإلا
		n += (uvarnumber_T)Prenum1;
	}

	// handle wraparound for decimal numbers
	إذا (!pre)
	{
	    إذا (گھٹاؤ)
	    {
		إذا (n > oldn)
		{
		    n = 1 + (n ^ (uvarnumber_T)-1);
		    negative ^= TRUE;
		}
	    }
	    وإلا
	    {
		//جوڑو
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
	    إذا (گھٹاؤ)
		// sticking at zero.
		n = (uvarnumber_T)0;
	    وإلا
		// sticking at 2^64 - 1.
		n = (uvarnumber_T)(-1);
	    negative = FALSE;
	}

	إذا (visual && !was_مثبت && !negative && خانہ > 0)
	{
	    // need to remove the '-'
	    خانہ--;
	    لمائی++;
	}

	/*
	 * Delete the old number.
	 */
	curwin->w_cursor.خانہ = خانہ;
	إذا (!did_change)
	    startpos = curwin->w_cursor;
	did_change = TRUE;
	todel = لمائی;
	c = gchar_cursor();
	/*
	 * Don't include the '-' in the length, only the length of the
	 * part after it is kept the same.
	 */
	إذا (c == '-')
	    --لمائی;

	save_pos = curwin->w_cursor;
	لكل (i = 0; i < todel; ++i)
	{
	    إذا (c < 0x100 && محفوظ_isalpha(c))
	    {
		إذا (محفوظ_isupper(c))
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
	buf1 = alloc(لمائی + NUMBUFLEN);
	إذا (buf1 == عدم)
	    اذهب theend;
	ptr = buf1;
	إذا (negative && (!visual || was_مثبت))
	    *ptr++ = '-';
	إذا (pre)
	{
	    *ptr++ = '0';
	    --لمائی;
	}
	إذا (pre == 'b' || pre == 'B' ||
	    pre == 'x' || pre == 'X')
	{
	    *ptr++ = pre;
	    --لمائی;
	}

	/*
	 * Put the number characters in buf2[].
	 */
	إذا (pre == 'b' || pre == 'B')
	{
	    صحيح ثنائی_ہندسہ = 0;
	    صحيح بٹ = حجم(uvarnumber_T) * 8;

	    // leading zeros
	    لكل (ثنائی_ہندسہ = بٹ; ثنائی_ہندسہ > 0; ثنائی_ہندسہ--)
		إذا ((n >> (ثنائی_ہندسہ - 1)) & 0x1) اكسر;

	    لكل (buf2len = 0; ثنائی_ہندسہ > 0 && buf2len < (NUMBUFLEN - 1); ثنائی_ہندسہ--)
		buf2[buf2len++] = ((n >> (ثنائی_ہندسہ - 1)) & 0x1) ? '1' : '0';

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
	لمائی -= buf2len;

	/*
	 * Adjust number of zeros to the new number of digits, so the
	 * total length of the number remains the same.
	 * Don't do this when
	 * the result may look like an octal number.
	 */
	إذا (firstdigit == '0' && !(do_oct && pre == 0))
	    طالما (لمائی-- > 0)
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
	    صحيح بائٹاں_after = ml_get_curline_len() - curwin->w_cursor.خانہ;

	    // Delete the one character before the insert.
	    curwin->w_cursor = save_pos;
	    (فراغ)del_char(FALSE);
	    curwin->w_cursor.خانہ = ml_get_curline_len() - بائٹاں_after;
	    --todel;
	}
	طالما (todel-- > 0)
	    (فراغ)del_char(FALSE);

	endpos = curwin->w_cursor;
	إذا (did_change && curwin->w_cursor.خانہ)
	    --curwin->w_cursor.خانہ;
    }

    إذا (did_change && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// set the '[ and '] marks
	curbuf->b_op_شروع = startpos;
	curbuf->b_op_سرا = endpos;
	إذا (curbuf->b_op_سرا.خانہ > 0)
	    --curbuf->b_op_سرا.خانہ;
    }

theend:
    إذا (visual)
	curwin->w_cursor = save_cursor;
    وإلا إذا (did_change)
	curwin->w_سیٹ_curswant = true;
    وإلا إذا (virtual_سرگرم())
	curwin->w_cursor.coladd = save_coladd;

    أرجع did_change;
}

    فراغ
clear_oparg(oparg_T *oap)
{
    CLEAR_اشارہ_گر(oap);
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
سطر_گنتی_info(
    char_u	*سطر,
    varnumber_T	*wc,
    varnumber_T	*cc,
    varnumber_T	حد_بندی,
    صحيح		eol_ناپ)
{
    varnumber_T	i;
    varnumber_T	الفاظ = 0;
    varnumber_T	chars = 0;
    صحيح		is_لفظ = 0;

    لكل (i = 0; i < حد_بندی && سطر[i] != NUL; )
    {
	إذا (is_لفظ)
	{
	    إذا (vim_isspace(سطر[i]))
	    {
		الفاظ++;
		is_لفظ = 0;
	    }
	}
	وإلا إذا (!vim_isspace(سطر[i]))
	    is_لفظ = 1;
	++chars;
	i += (*mb_ptr2len)(سطر + i);
    }

    إذا (is_لفظ)
	الفاظ++;
    *wc += الفاظ;

    // Add eol_size if the end of line was reached before hitting limit.
    إذا (i < حد_بندی && سطر[i] == NUL)
    {
	i += eol_ناپ;
	chars += eol_ناپ;
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
    varnumber_T	byte_گنتی = 0;
    varnumber_T	bom_گنتی  = 0;
    varnumber_T	byte_گنتی_cursor = 0;
    varnumber_T	char_گنتی = 0;
    varnumber_T	char_گنتی_cursor = 0;
    varnumber_T	لفظ_گنتی = 0;
    varnumber_T	لفظ_گنتی_cursor = 0;
    صحيح		eol_ناپ;
    varnumber_T	آخری_check = 100000L;
    لما	سطر_گنتی_selected = 0;
    pos_T	min_pos, max_pos;
    oparg_T	oparg;
    بنية بلاک_def	bd;

    /*
     * Compute the length of the file in characters.
     */
    إذا (curbuf->b_ml.ml_flags & ML_خالی_تھاواں)
    {
	إذا (dict == عدم)
	{
	    پیغام(_(no_سطریں_پیغام));
	    أرجع;
	}
    }
    وإلا
    {
	إذا (get_fileformat(curbuf) == EOL_DOS)
	    eol_ناپ = 2;
	وإلا
	    eol_ناپ = 1;

	إذا (VIsual_سرگرم)
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
	    إذا (*p_sel == 'e' && max_pos.خانہ > 0)
		--max_pos.خانہ;

	    إذا (VIsual_منوال == Ctrl_V)
	    {
#جے_تعریف_شدہ FEAT_LINEBREAK
		char_u * saved_sbr = p_sbr;
		char_u * saved_w_sbr = curwin->w_p_sbr;

		// Make 'sbr' empty for a moment to get the correct size.
		p_sbr = خالی_تھاواں_option;
		curwin->w_p_sbr = خالی_تھاواں_option;
#جے_انت
		oparg.is_VIsual = 1;
		oparg.بلاک_منوال = TRUE;
		oparg.op_type = OP_NOP;
		getvcols(curwin, &min_pos, &max_pos,
					&oparg.شروع_vcol, &oparg.سرا_vcol, 0);
#جے_تعریف_شدہ FEAT_LINEBREAK
		p_sbr = saved_sbr;
		curwin->w_p_sbr = saved_w_sbr;
#جے_انت
		إذا (curwin->w_curswant == MAXCOL)
		    oparg.سرا_vcol = MAXCOL;
		// Swap the start, end vcol if needed
		إذا (oparg.سرا_vcol < oparg.شروع_vcol)
		{
		    oparg.سرا_vcol += oparg.شروع_vcol;
		    oparg.شروع_vcol = oparg.سرا_vcol - oparg.شروع_vcol;
		    oparg.سرا_vcol -= oparg.شروع_vcol;
		}
	    }
	    سطر_گنتی_selected = max_pos.lnum - min_pos.lnum + 1;
	}

	لكل (lnum = 1; lnum <= curbuf->b_ml.ml_سطر_گنتی; ++lnum)
	{
	    // Check for a CTRL-C every 100000 characters.
	    إذا (byte_گنتی > آخری_check)
	    {
		ui_breakcheck();
		إذا (got_int)
		    أرجع;
		آخری_check = byte_گنتی + 100000L;
	    }

	    // Do extra processing for VIsual mode.
	    إذا (VIsual_سرگرم
		    && lnum >= min_pos.lnum && lnum <= max_pos.lnum)
	    {
		char_u	    *s = عدم;
		لما	    len = 0L;

		بدل (VIsual_منوال)
		{
		    حالة Ctrl_V:
			virtual_op = virtual_سرگرم();
			بلاک_prep(&oparg, &bd, lnum, 0);
			virtual_op = MAYBE;
			s = bd.textstart;
			len = (لما)bd.textlen;
			اكسر;
		    حالة 'V':
			s = ml_get(lnum);
			len = MAXCOL;
			اكسر;
		    حالة 'v':
			{
			    colnr_T شروع_خانہ = (lnum == min_pos.lnum)
							   ? min_pos.خانہ : 0;
			    colnr_T سرا_خانہ = (lnum == max_pos.lnum)
				      ? max_pos.خانہ - شروع_خانہ + 1 : MAXCOL;

			    s = ml_get(lnum) + شروع_خانہ;
			    len = سرا_خانہ;
			}
			اكسر;
		}
		إذا (s != عدم)
		{
		    byte_گنتی_cursor += سطر_گنتی_info(s, &لفظ_گنتی_cursor,
					   &char_گنتی_cursor, len, eol_ناپ);
		    إذا (lnum == curbuf->b_ml.ml_سطر_گنتی
			    && !curbuf->b_p_eol
			    && (curbuf->b_p_bin || !curbuf->b_p_fixeol)
			    && (لما)STRLEN(s) < len)
			byte_گنتی_cursor -= eol_ناپ;
		}
	    }
	    وإلا
	    {
		// In non-visual mode, check for the line the cursor is on
		إذا (lnum == curwin->w_cursor.lnum)
		{
		    لفظ_گنتی_cursor += لفظ_گنتی;
		    char_گنتی_cursor += char_گنتی;
		    byte_گنتی_cursor = byte_گنتی +
			سطر_گنتی_info(ml_get(lnum),
				&لفظ_گنتی_cursor, &char_گنتی_cursor,
				(varnumber_T)(curwin->w_cursor.خانہ + 1),
				eol_ناپ);
		}
	    }
	    // Add to the running totals
	    byte_گنتی += سطر_گنتی_info(ml_get(lnum), &لفظ_گنتی,
					 &char_گنتی, (varnumber_T)MAXCOL,
					 eol_ناپ);
	}

	// Correction for when last line doesn't have an EOL.
	إذا (!curbuf->b_p_eol && (curbuf->b_p_bin || !curbuf->b_p_fixeol))
	    byte_گنتی -= eol_ناپ;

	إذا (dict == عدم)
	{
	    إذا (VIsual_سرگرم)
	    {
		إذا (VIsual_منوال == Ctrl_V && curwin->w_curswant < MAXCOL)
		{
		    getvcols(curwin, &min_pos, &max_pos,
						&min_pos.خانہ, &max_pos.خانہ, 0);
		    vim_snprintf((حرف *)buf1, حجم(buf1), _("%ld Cols; "),
			    (لما)(oparg.سرا_vcol - oparg.شروع_vcol + 1));
		}
		وإلا
		    buf1[0] = NUL;

		إذا (char_گنتی_cursor == byte_گنتی_cursor
						    && char_گنتی == byte_گنتی)
		    vim_snprintf((حرف *)IObuff, IOSIZE,
			    _("Selected %s%ld of %ld Lines; %lld of %lld Words; %lld of %lld Bytes"),
			    buf1, سطر_گنتی_selected,
			    (لما)curbuf->b_ml.ml_سطر_گنتی,
			    لفظ_گنتی_cursor,
			    لفظ_گنتی,
			    byte_گنتی_cursor,
			    byte_گنتی);
		وإلا
		    vim_snprintf((حرف *)IObuff, IOSIZE,
			    _("Selected %s%ld of %ld Lines; %lld of %lld Words; %lld of %lld Chars; %lld of %lld Bytes"),
			    buf1, سطر_گنتی_selected,
			    (لما)curbuf->b_ml.ml_سطر_گنتی,
			    لفظ_گنتی_cursor,
			    لفظ_گنتی,
			    char_گنتی_cursor,
			    char_گنتی,
			    byte_گنتی_cursor,
			    byte_گنتی);
	    }
	    وإلا
	    {
		p = ml_get_curline();
		validate_virtcol();
		خانہ_print(buf1, حجم(buf1), (صحيح)curwin->w_cursor.خانہ + 1,
			(صحيح)curwin->w_virtcol + 1);
		خانہ_print(buf2, حجم(buf2), ml_get_curline_len(),
							   linetabsize_str(p));

		إذا (char_گنتی_cursor == byte_گنتی_cursor
			&& char_گنتی == byte_گنتی)
		    vim_snprintf((حرف *)IObuff, IOSIZE,
			_("Col %s of %s; Line %ld of %ld; Word %lld of %lld; Byte %lld of %lld"),
			(حرف *)buf1, (حرف *)buf2,
			(لما)curwin->w_cursor.lnum,
			(لما)curbuf->b_ml.ml_سطر_گنتی,
			لفظ_گنتی_cursor, لفظ_گنتی,
			byte_گنتی_cursor, byte_گنتی);
		وإلا
		    vim_snprintf((حرف *)IObuff, IOSIZE,
			_("Col %s of %s; Line %ld of %ld; Word %lld of %lld; Char %lld of %lld; Byte %lld of %lld"),
			(حرف *)buf1, (حرف *)buf2,
			(لما)curwin->w_cursor.lnum,
			(لما)curbuf->b_ml.ml_سطر_گنتی,
			لفظ_گنتی_cursor, لفظ_گنتی,
			char_گنتی_cursor, char_گنتی,
			byte_گنتی_cursor, byte_گنتی);
	    }
	}

	bom_گنتی = bomb_ناپ();
	إذا (dict == عدم && bom_گنتی > 0)
	{
	    size_t len = STRLEN(IObuff);

	    vim_snprintf((حرف *)IObuff + len, IOSIZE - len,
				 _("(+%lld for BOM)"), bom_گنتی);
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
	dict_جوڑو_نمبر(dict, "words", لفظ_گنتی);
	dict_جوڑو_نمبر(dict, "chars", char_گنتی);
	dict_جوڑو_نمبر(dict, "bytes", byte_گنتی + bom_گنتی);
	dict_جوڑو_نمبر(dict, VIsual_سرگرم ? "visual_bytes" : "cursor_bytes",
		byte_گنتی_cursor);
	dict_جوڑو_نمبر(dict, VIsual_سرگرم ? "visual_chars" : "cursor_chars",
		char_گنتی_cursor);
	dict_جوڑو_نمبر(dict, VIsual_سرگرم ? "visual_words" : "cursor_words",
		لفظ_گنتی_cursor);
    }
#جے_انت
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
	إذا (oap->شروع.lnum == curwin->w_cursor.lnum)
	    stuffcharReadbuff('.');
	وإلا
	    stuffnumReadbuff((لما)oap->شروع.lnum);

#جے_تعریف_شدہ FEAT_FOLDING
	// When using !! on a closed fold the range ".!" works best to operate
	// on, it will be made the whole closed fold later.
	linenr_T سرا_دا_شروع_Fold = oap->شروع.lnum;
	(فراغ)hasFolding(oap->شروع.lnum, عدم, &سرا_دا_شروع_Fold);
#جے_انت
	إذا (oap->سرا.lnum != oap->شروع.lnum
#جے_تعریف_شدہ FEAT_FOLDING
		&& oap->سرا.lnum != سرا_دا_شروع_Fold
#جے_انت
	   )
	{
	    // Make it a range with the end line.
	    stuffcharReadbuff(',');
	    إذا (oap->سرا.lnum == curwin->w_cursor.lnum)
		stuffcharReadbuff('.');
	    وإلا إذا (oap->سرا.lnum == curbuf->b_ml.ml_سطر_گنتی)
		stuffcharReadbuff('$');
	    وإلا إذا (oap->شروع.lnum == curwin->w_cursor.lnum
#جے_تعریف_شدہ FEAT_FOLDING
		    // do not use ".+number" for a closed fold, it would count
		    // folded lines twice
		    && !hasFolding(oap->سرا.lnum, عدم, عدم)
#جے_انت
		    )
	    {
		stuffReadbuff((char_u *)".+");
		stuffnumReadbuff((لما)oap->سطر_گنتی - 1);
	    }
	    وإلا
		stuffnumReadbuff((لما)oap->سرا.lnum);
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
did_سیٹ_operatorfunc(optset_T *دلیلاں بن_ورتیا)
{
    إذا (option_سیٹ_callback_func(p_opfunc, &opfunc_cb) == FAIL)
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
#جے_انت

#إذا defined(FEAT_EVAL)
/*
 * Mark the global 'operatorfunc' callback with "copyID" so that it is not
 * garbage collected.
 */
    صحيح
سیٹ_ref_وچ_opfunc(صحيح copyID بن_ورتیا)
{
    صحيح abort = FALSE;

    abort = سیٹ_ref_وچ_callback(&opfunc_cb, copyID);

    أرجع abort;
}
#جے_انت

/*
 * Handle the "g@" operator: call 'operatorfunc'.
 */
    ساكن فراغ
op_function(oparg_T *oap بن_ورتیا)
{
#جے_تعریف_شدہ FEAT_EVAL
    typval_T	argv[2];
    pos_T	orig_شروع = curbuf->b_op_شروع;
    pos_T	orig_سرا = curbuf->b_op_سرا;
    typval_T	rettv;

    إذا (*p_opfunc == NUL)
	emsg(_(e_operatorfunc_is_خالی_تھاواں));
    وإلا
    {
	// Set '[ and '] marks to text to be operated on.
	curbuf->b_op_شروع = oap->شروع;
	curbuf->b_op_سرا = oap->سرا;
	إذا (oap->motion_type != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_سرا);

	argv[0].v_type = VAR_STRING;
	إذا (oap->بلاک_منوال)
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
	صحيح save_انت_op = انت_op;
	انت_op = FALSE;

	إذا (call_callback(&opfunc_cb, 0, &rettv, 1, argv) != FAIL)
	    clear_tv(&rettv);

	virtual_op = save_virtual_op;
	انت_op = save_انت_op;
	إذا (cmdmod.cmod_flags & CMOD_LOCKMARKS)
	{
	    curbuf->b_op_شروع = orig_شروع;
	    curbuf->b_op_سرا = orig_سرا;
	}
    }
#وإلا
    emsg(_(e_eval_feature_not_دستیاب));
#جے_انت
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
    colnr_T	    شروع, سرا;

    إذا (VIsual_منوال != Ctrl_V
	    || (!initial && oap->سرا.خانہ < curwin->w_چوڑائی))
	أرجع;

    oap->بلاک_منوال = TRUE;

    // prevent from moving onto a trail byte
    إذا (has_mbyte)
	mb_adjustpos(curwin->w_بفر, &oap->سرا);

    getvvcol(curwin, &(oap->شروع), &oap->شروع_vcol, عدم, &oap->سرا_vcol, 0);

    إذا (!redo_VIsual_busy)
    {
	getvvcol(curwin, &(oap->سرا), &شروع, عدم, &سرا, 0);

	إذا (شروع < oap->شروع_vcol)
	    oap->شروع_vcol = شروع;
	إذا (سرا > oap->سرا_vcol)
	{
	    إذا (initial && *p_sel == 'e' && شروع >= 1
				    && شروع - 1 >= oap->سرا_vcol)
		oap->سرا_vcol = شروع - 1;
	    وإلا
		oap->سرا_vcol = سرا;
	}
    }

    // if '$' was used, get oap->end_vcol from longest line
    إذا (curwin->w_curswant == MAXCOL)
    {
	curwin->w_cursor.خانہ = MAXCOL;
	oap->سرا_vcol = 0;
	لكل (curwin->w_cursor.lnum = oap->شروع.lnum;
		curwin->w_cursor.lnum <= oap->سرا.lnum;
					++curwin->w_cursor.lnum)
	{
	    getvvcol(curwin, &curwin->w_cursor, عدم, عدم, &سرا, 0);
	    إذا (سرا > oap->سرا_vcol)
		oap->سرا_vcol = سرا;
	}
    }
    وإلا إذا (redo_VIsual_busy)
	oap->سرا_vcol = oap->شروع_vcol + redo_VIsual_vcol - 1;
    // Correct oap->end.col and oap->start.col to be the
    // upper-left and lower-right corner of the block area.
    //
    // (Actually, this does convert column positions into character
    // positions)
    curwin->w_cursor.lnum = oap->سرا.lnum;
    coladvance(oap->سرا_vcol);
    oap->سرا = curwin->w_cursor;

    curwin->w_cursor = oap->شروع;
    coladvance(oap->شروع_vcol);
    oap->شروع = curwin->w_cursor;
}

// Information for redoing the previous Visual selection.
تعريفنوع بنية {
    صحيح		rv_منوال;	// 'v', 'V', or Ctrl-V
    linenr_T	rv_سطر_گنتی;	// number of lines
    colnr_T	rv_vcol;	// number of cols or end column
    لما	rv_گنتی;	// count for Visual operator
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
do_pending_operator(cmdarg_T *cap, صحيح old_خانہ, صحيح gui_yank)
{
    oparg_T	*oap = cap->oap;
    pos_T	old_cursor;
    صحيح		خالی_تھاواں_region_error;
    صحيح		restart_edit_save;
#جے_تعریف_شدہ FEAT_LINEBREAK
    صحيح		lbr_saved = curwin->w_p_lbr;
#جے_انت

    // The visual area is remembered for redo
    ساكن redo_VIsual_T   redo_VIsual = {NUL, 0, 0, 0,0};

    صحيح		    include_سطر_break = FALSE;

#إذا defined(FEAT_CLIPBOARD)
    // Yank the visual area into the GUI selection register before we operate
    // on it and lose it forever.
    // Don't do it if a specific register was specified, so that ""x"*P works.
    // This could call do_pending_operator() recursively, but that's OK
    // because gui_yank will be TRUE for the nested call.
    إذا ((clip_تارا.دستیاب || clip_plus.دستیاب)
	    && oap->op_type != OP_NOP
	    && !gui_yank
	    && VIsual_سرگرم
	    && !redo_VIsual_busy
	    && oap->regname == 0)
	clip_auto_select();
#جے_انت
    old_cursor = curwin->w_cursor;

    // If an operation is pending, handle it...
    إذا ((انت_op || VIsual_سرگرم) && oap->op_type != OP_NOP)
    {
	// Yank can be redone when 'y' is in 'cpoptions', but not when yanking
	// for the clipboard.
	صحيح	redo_yank = vim_strchr(p_cpo, CPO_YANK) != عدم && !gui_yank;

#جے_تعریف_شدہ FEAT_LINEBREAK
	// Avoid a problem with unwanted linebreaks in block mode.
	(فراغ)reset_lbr();
#جے_انت
	oap->is_VIsual = VIsual_سرگرم;
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
	    إذا (!VIsual_سرگرم)
	    {
		VIsual_سرگرم = TRUE;
		VIsual = oap->شروع;
	    }
	    VIsual_منوال = Ctrl_V;
	    VIsual_select = FALSE;
	    VIsual_reselect = FALSE;
	}

	// Only redo yank when 'y' flag is in 'cpoptions'.
	// Never redo "zf" (define fold).
	إذا ((redo_yank || oap->op_type != OP_YANK)
		&& ((!VIsual_سرگرم || oap->motion_force)
		    // Also redo Operator-pending Visual mode mappings
		    || (VIsual_سرگرم
			    && is_ex_cmdchar(cap) && oap->op_type != OP_COLON))
		&& cap->cmdchar != 'D'
#جے_تعریف_شدہ FEAT_FOLDING
		&& oap->op_type != OP_FOLD
		&& oap->op_type != OP_FOLDOPEN
		&& oap->op_type != OP_FOLDOPENREC
		&& oap->op_type != OP_FOLDCLOSE
		&& oap->op_type != OP_FOLDCLOSEREC
		&& oap->op_type != OP_FOLDDEL
		&& oap->op_type != OP_FOLDDELREC
#جے_انت
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
	    oap->شروع = curwin->w_cursor;
	    curwin->w_cursor.lnum += redo_VIsual.rv_سطر_گنتی - 1;
	    إذا (curwin->w_cursor.lnum > curbuf->b_ml.ml_سطر_گنتی)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_سطر_گنتی;
	    VIsual_منوال = redo_VIsual.rv_منوال;
	    إذا (redo_VIsual.rv_vcol == MAXCOL || VIsual_منوال == 'v')
	    {
		إذا (VIsual_منوال == 'v')
		{
		    إذا (redo_VIsual.rv_سطر_گنتی <= 1)
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
	    cap->count0 = redo_VIsual.rv_گنتی;
	    إذا (redo_VIsual.rv_گنتی != 0)
		cap->count1 = redo_VIsual.rv_گنتی;
	    وإلا
		cap->count1 = 1;
	}
	وإلا إذا (VIsual_سرگرم)
	{
	    إذا (!gui_yank)
	    {
		// Save the current VIsual area for '< and '> marks, and "gv"
		curbuf->b_visual.vi_شروع = VIsual;
		curbuf->b_visual.vi_سرا = curwin->w_cursor;
		curbuf->b_visual.vi_منوال = VIsual_منوال;
		restore_visual_منوال();
		curbuf->b_visual.vi_curswant = curwin->w_curswant;
#جے_تعریف_شدہ FEAT_EVAL
		curbuf->b_visual_منوال_eval = VIsual_منوال;
#جے_انت
	    }

	    // In Select mode, a linewise selection is operated upon like a
	    // characterwise selection.
	    // Special case: gH<Del> deletes the last line.
	    إذا (VIsual_select && VIsual_منوال == 'V'
					    && cap->oap->op_type != OP_DELETE)
	    {
		إذا (LT_POS(VIsual, curwin->w_cursor))
		{
		    VIsual.خانہ = 0;
		    curwin->w_cursor.خانہ = ml_get_len(curwin->w_cursor.lnum);
		}
		وإلا
		{
		    curwin->w_cursor.خانہ = 0;
		    VIsual.خانہ = ml_get_len(VIsual.lnum);
		}
		VIsual_منوال = 'v';
	    }
	    // If 'selection' is "exclusive", backup one character for
	    // charwise selections.
	    وإلا إذا (VIsual_منوال == 'v')
		include_سطر_break = unadjust_for_sel();

	    oap->شروع = VIsual;
	    إذا (VIsual_منوال == 'V')
	    {
		oap->شروع.خانہ = 0;
		oap->شروع.coladd = 0;
	    }
	}

	// Set oap->start to the first position of the operated text, oap->end
	// to the end of the operated text.  w_cursor is equal to oap->start.
	إذا (LT_POS(oap->شروع, curwin->w_cursor))
	{
#جے_تعریف_شدہ FEAT_FOLDING
	    // Include folded lines completely.
	    إذا (!VIsual_سرگرم)
	    {
		إذا (hasFolding(oap->شروع.lnum, &oap->شروع.lnum, عدم))
		    oap->شروع.خانہ = 0;
		إذا ((curwin->w_cursor.خانہ > 0 || oap->inclusive
						  || oap->motion_type == MLINE)
			&& hasFolding(curwin->w_cursor.lnum, عدم,
						      &curwin->w_cursor.lnum))
		    curwin->w_cursor.خانہ = ml_get_curline_len();
	    }
#جے_انت
	    oap->سرا = curwin->w_cursor;
	    curwin->w_cursor = oap->شروع;

	    // w_virtcol may have been updated; if the cursor goes back to its
	    // previous position w_virtcol becomes invalid and isn't updated
	    // automatically.
	    curwin->w_valid &= ~VALID_VIRTCOL;
	}
	وإلا
	{
#جے_تعریف_شدہ FEAT_FOLDING
	    // Include folded lines completely.
	    إذا (!VIsual_سرگرم && oap->motion_type == MLINE)
	    {
		إذا (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum,
									عدم))
		    curwin->w_cursor.خانہ = 0;
		إذا (hasFolding(oap->شروع.lnum, عدم, &oap->شروع.lnum))
		    oap->شروع.خانہ = ml_get_len(oap->شروع.lnum);
	    }
#جے_انت
	    oap->سرا = oap->شروع;
	    oap->شروع = curwin->w_cursor;
	}

	// Just in case lines were deleted that make the position invalid.
	check_pos(curwin->w_بفر, &oap->سرا);
	oap->سطر_گنتی = oap->سرا.lnum - oap->شروع.lnum + 1;

	// Set "virtual_op" before resetting VIsual_active.
	virtual_op = virtual_سرگرم();

	إذا (VIsual_سرگرم || redo_VIsual_busy)
	{
	    get_op_vcol(oap, redo_VIsual.rv_vcol, TRUE);

	    إذا (!redo_VIsual_busy && !gui_yank)
	    {
		// Prepare to reselect and redo Visual: this is based on the
		// size of the Visual text
		resel_VIsual_منوال = VIsual_منوال;
		إذا (curwin->w_curswant == MAXCOL)
		    resel_VIsual_vcol = MAXCOL;
		وإلا
		{
		    إذا (VIsual_منوال != Ctrl_V)
			getvvcol(curwin, &(oap->سرا),
						عدم, عدم, &oap->سرا_vcol, 0);
		    إذا (VIsual_منوال == Ctrl_V || oap->سطر_گنتی <= 1)
		    {
			إذا (VIsual_منوال != Ctrl_V)
			    getvvcol(curwin, &(oap->شروع),
					      &oap->شروع_vcol, عدم, عدم, 0);
			resel_VIsual_vcol = oap->سرا_vcol - oap->شروع_vcol + 1;
		    }
		    وإلا
			resel_VIsual_vcol = oap->سرا_vcol;
		}
		resel_VIsual_سطر_گنتی = oap->سطر_گنتی;
	    }

	    // can't redo yank (unless 'y' is in 'cpoptions') and ":"
	    إذا ((redo_yank || oap->op_type != OP_YANK)
		    && oap->op_type != OP_COLON
#جے_تعریف_شدہ FEAT_FOLDING
		    && oap->op_type != OP_FOLD
		    && oap->op_type != OP_FOLDOPEN
		    && oap->op_type != OP_FOLDOPENREC
		    && oap->op_type != OP_FOLDCLOSE
		    && oap->op_type != OP_FOLDCLOSEREC
		    && oap->op_type != OP_FOLDDEL
		    && oap->op_type != OP_FOLDDELREC
#جے_انت
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
		    redo_VIsual.rv_منوال = resel_VIsual_منوال;
		    redo_VIsual.rv_vcol = resel_VIsual_vcol;
		    redo_VIsual.rv_سطر_گنتی = resel_VIsual_سطر_گنتی;
		    redo_VIsual.rv_گنتی = cap->count0;
		    redo_VIsual.rv_arg = cap->arg;
		}
	    }

	    // oap->inclusive defaults to TRUE.
	    // If oap->end is on a NUL (empty line) oap->inclusive becomes
	    // FALSE.  This makes "d}P" and "v}dP" work the same.
	    إذا (oap->motion_force == NUL || oap->motion_type == MLINE)
		oap->inclusive = TRUE;
	    إذا (VIsual_منوال == 'V')
		oap->motion_type = MLINE;
	    وإلا
	    {
		oap->motion_type = MCHAR;
		إذا (VIsual_منوال != Ctrl_V && *ml_get_pos(&(oap->سرا)) == NUL
			&& (include_سطر_break || !virtual_op))
		{
		    oap->inclusive = FALSE;
		    // Try to include the newline, unless it's an operator
		    // that works on lines only.
		    إذا (*p_sel != 'o'
			    && !op_on_سطریں(oap->op_type)
			    && oap->سرا.lnum < curbuf->b_ml.ml_سطر_گنتی)
		    {
			++oap->سرا.lnum;
			oap->سرا.خانہ = 0;
			oap->سرا.coladd = 0;
			++oap->سطر_گنتی;
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
		VIsual_سرگرم = FALSE;
		setmouse();
		mouse_dragging = 0;
		may_clear_cmdline();
		إذا ((oap->op_type == OP_YANK
			    || oap->op_type == OP_COLON
			    || oap->op_type == OP_FUNCTION
			    || oap->op_type == OP_FILTER)
			&& oap->motion_force == NUL)
		{
#جے_تعریف_شدہ FEAT_LINEBREAK
		    // make sure redrawing is correct
		    restore_lbr(lbr_saved);
#جے_انت
		    redraw_curbuf_later(UPD_INVERTED);
		}
	    }
	}

	// Include the trailing byte of a multi-byte char.
	إذا (has_mbyte && oap->inclusive)
	{
	    صحيح		l;

	    l = (*mb_ptr2len)(ml_get_pos(&oap->سرا));
	    إذا (l > 1)
		oap->سرا.خانہ += l - 1;
	}
	curwin->w_سیٹ_curswant = true;

	// oap->empty is set when start and end are the same.  The inclusive
	// flag affects this too, unless yanking and the end is on a NUL.
	oap->خالی_تھاواں_2 = (oap->motion_type == MCHAR
		    && (!oap->inclusive
			|| (oap->op_type == OP_YANK
			    && gchar_pos(&oap->سرا) == NUL))
		    && EQUAL_POS(oap->شروع, oap->سرا)
		    && !(virtual_op && oap->شروع.coladd != oap->سرا.coladd));
	// For delete, change and yank, it's an error to operate on an
	// empty region, when 'E' included in 'cpoptions' (Vi compatible).
	خالی_تھاواں_region_error = (oap->خالی_تھاواں_2
				&& vim_strchr(p_cpo, CPO_EMPTYREGION) != عدم);

	// Force a redraw when operating on an empty Visual region, when
	// 'modifiable is off or creating a fold.
	إذا (oap->is_VIsual && (oap->خالی_تھاواں_2 || !curbuf->b_p_ma
#جے_تعریف_شدہ FEAT_FOLDING
		    || oap->op_type == OP_FOLD
#جے_انت
		    ))
	{
#جے_تعریف_شدہ FEAT_LINEBREAK
	    restore_lbr(lbr_saved);
#جے_انت
	    redraw_curbuf_later(UPD_INVERTED);
	}

	// If the end of an operator is in column one while oap->motion_type
	// is MCHAR and oap->inclusive is FALSE, we put op_end after the last
	// character in the previous line. If op_start is on or before the
	// first non-blank in the line, the operator becomes linewise
	// (strange, but that's the way vi does it).
	إذا (	   oap->motion_type == MCHAR
		&& oap->inclusive == FALSE
		&& !(cap->retval & CA_NO_ADJ_OP_سرا)
		&& oap->سرا.خانہ == 0
		&& (!oap->is_VIsual || *p_sel == 'o')
		&& !oap->بلاک_منوال
		&& oap->سطر_گنتی > 1)
	{
	    oap->سرا_adjusted = TRUE;	    // remember that we did this
	    --oap->سطر_گنتی;
	    --oap->سرا.lnum;
	    إذا (inindent(0))
		oap->motion_type = MLINE;
	    وإلا
	    {
		oap->سرا.خانہ = ml_get_len(oap->سرا.lnum);
		إذا (oap->سرا.خانہ)
		{
		    --oap->سرا.خانہ;
		    oap->inclusive = TRUE;
		}
	    }
	}
	وإلا
	    oap->سرا_adjusted = FALSE;

	بدل (oap->op_type)
	{
	حالة OP_LSHIFT:
	حالة OP_RSHIFT:
	    op_کھسکاؤ(oap, TRUE, oap->is_VIsual ? (صحيح)cap->count1 : 1);
	    auto_format(FALSE, TRUE);
	    اكسر;

	حالة OP_JOIN_NS:
	حالة OP_JOIN:
	    إذا (oap->سطر_گنتی < 2)
		oap->سطر_گنتی = 2;
	    إذا (curwin->w_cursor.lnum + oap->سطر_گنتی - 1 >
						   curbuf->b_ml.ml_سطر_گنتی)
		beep_flush();
	    وإلا
	    {
		(فراغ)do_join(oap->سطر_گنتی, oap->op_type == OP_JOIN,
							    TRUE, TRUE, TRUE);
		auto_format(FALSE, TRUE);
	    }
	    اكسر;

	حالة OP_DELETE:
	    VIsual_reselect = FALSE;	    // don't reselect now
	    إذا (خالی_تھاواں_region_error)
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
	    إذا (خالی_تھاواں_region_error)
	    {
		إذا (!gui_yank)
		{
		    vim_beep(BO_OPER);
		    CancelRedo();
		}
	    }
	    وإلا
	    {
#جے_تعریف_شدہ FEAT_LINEBREAK
		restore_lbr(lbr_saved);
#جے_انت
		oap->excl_tr_ws = cap->cmdchar == 'z';
		(فراغ)op_yank(oap, FALSE, !gui_yank);
	    }
	    check_cursor_خانہ();
	    اكسر;

	حالة OP_CHANGE:
	    VIsual_reselect = FALSE;	    // don't reselect now
	    إذا (خالی_تھاواں_region_error)
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
		إذا (p_im || !کلید_Typed)
		    restart_edit_save = restart_edit;
		وإلا
		    restart_edit_save = 0;
		restart_edit = 0;
#جے_تعریف_شدہ FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#جے_انت
		// trigger TextChangedI
		curbuf->b_آخری_changedtick_i = CHANGEDTICK(curbuf);

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
#جے_تعریف_شدہ FEAT_EVAL
		    إذا (use_indentexpr_for_lisp())
			op_reindent(oap, get_اظہاریہ_indent);
		    وإلا
#جے_انت
			op_reindent(oap, get_lisp_indent);
		    اكسر;
		}
		op_reindent(oap,
#جے_تعریف_شدہ FEAT_EVAL
			*curbuf->b_p_inde != NUL ? get_اظہاریہ_indent :
#جے_انت
			    get_c_indent);
		اكسر;
	    }

	    op_colon(oap);
	    اكسر;

	حالة OP_TILDE:
	حالة OP_اتلا:
	حالة OP_ہیٹھلا:
	حالة OP_ROT13:
	    إذا (خالی_تھاواں_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    وإلا
		op_tilde(oap);
	    check_cursor_خانہ();
	    اكسر;

	حالة OP_FORMAT:
#إذا defined(FEAT_EVAL)
	    إذا (*curbuf->b_p_fex != NUL)
		op_formatexpr(oap);	// use expression
	    وإلا
#جے_انت
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

#جے_تعریف_شدہ FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#جے_انت
		// call 'operatorfunc'
		op_function(oap);

		// Restore the info for redoing Visual mode, the function may
		// invoke another operator and unintentionally change it.
		redo_VIsual = save_redo_VIsual;
		اكسر;
	    }

	حالة OP_داخل_کرو:
	حالة OP_APPEND:
	    VIsual_reselect = FALSE;	// don't reselect now
	    إذا (خالی_تھاواں_region_error)
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
#جے_تعریف_شدہ FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#جے_انت
		// trigger TextChangedI
		curbuf->b_آخری_changedtick_i = CHANGEDTICK(curbuf);

		op_داخل_کرو(oap, cap->count1);
#جے_تعریف_شدہ FEAT_LINEBREAK
		// Reset linebreak, so that formatting works correctly.
		(فراغ)reset_lbr();
#جے_انت

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
	    إذا (خالی_تھاواں_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    وإلا
	    {
#جے_تعریف_شدہ FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#جے_انت
		op_replace(oap, cap->nchar);
	    }
	    اكسر;

#جے_تعریف_شدہ FEAT_FOLDING
	حالة OP_FOLD:
	    VIsual_reselect = FALSE;	// don't reselect now
	    foldCreate(oap->شروع.lnum, oap->سرا.lnum);
	    اكسر;

	حالة OP_FOLDOPEN:
	حالة OP_FOLDOPENREC:
	حالة OP_FOLDCLOSE:
	حالة OP_FOLDCLOSEREC:
	    VIsual_reselect = FALSE;	// don't reselect now
	    opFoldRange(oap->شروع.lnum, oap->سرا.lnum,
		    oap->op_type == OP_FOLDOPEN
					    || oap->op_type == OP_FOLDOPENREC,
		    oap->op_type == OP_FOLDOPENREC
					  || oap->op_type == OP_FOLDCLOSEREC,
					  oap->is_VIsual);
	    اكسر;

	حالة OP_FOLDDEL:
	حالة OP_FOLDDELREC:
	    VIsual_reselect = FALSE;	// don't reselect now
	    deleteFold(oap->شروع.lnum, oap->سرا.lnum,
			       oap->op_type == OP_FOLDDELREC, oap->is_VIsual);
	    اكسر;
#جے_انت
	حالة OP_NR_جوڑو:
	حالة OP_NR_SUB:
	    إذا (خالی_تھاواں_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    وإلا
	    {
		VIsual_سرگرم = TRUE;
#جے_تعریف_شدہ FEAT_LINEBREAK
		restore_lbr(lbr_saved);
#جے_انت
		op_addsub(oap, cap->count1, redo_VIsual.rv_arg);
		VIsual_سرگرم = FALSE;
	    }
	    check_cursor_خانہ();
	    اكسر;
	افتراضي:
	    clearopbeep(oap);
	}
	virtual_op = MAYBE;
	إذا (!gui_yank)
	{
	    // if 'sol' not set, go back to old column for some commands
	    إذا (!p_sol && oap->motion_type == MLINE && !oap->سرا_adjusted
		    && (oap->op_type == OP_LSHIFT || oap->op_type == OP_RSHIFT
						|| oap->op_type == OP_DELETE))
	    {
#جے_تعریف_شدہ FEAT_LINEBREAK
		(فراغ)reset_lbr();
#جے_انت
		coladvance(curwin->w_curswant = old_خانہ);
	    }
	}
	وإلا
	{
	    curwin->w_cursor = old_cursor;
	}
	oap->بلاک_منوال = FALSE;
	clearop(oap);
	motion_force = NUL;
    }
#جے_تعریف_شدہ FEAT_LINEBREAK
    restore_lbr(lbr_saved);
#جے_انت
}

// put byte 'c' at position 'lp', but
// verify, that the position to place
// is actually safe
    ساكن فراغ
pbyte(pos_T lp, صحيح c)
{
    char_u *p = ml_get_buf(curbuf, lp.lnum, TRUE);
    صحيح	len = curbuf->b_ml.ml_سطر_len;

    // safety check
    إذا (lp.خانہ >= len)
	lp.خانہ = (len > 1 ? len - 2 : 0);
    *(p + lp.خانہ) = c;
}
