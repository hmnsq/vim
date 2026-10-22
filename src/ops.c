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

ساکن خالی سرکاؤ_بلاک(oparg_T *oap, صحیح amount);
ساکن خالی	mb_adjust_opend(oparg_T *oap);
ساکن صحیح	do_addsub(صحیح op_type, pos_T *pos, صحیح لمبائی, linenr_T Prenum1);
ساکن خالی	pbyte(pos_T lp, صحیح c);
#تعریف_کرو PBYTE(lp, c) pbyte(lp, c)


// Flags for third item in "opchars".
#تعریف_کرو OPF_سطریں  1	// operator always works on lines
#تعریف_کرو OPF_CHANGE 2	// operator changes text

/*
 * The names of operators.
 * IMPORTANT: Index must correspond with defines in vim.h!!!
 * The third field holds OPF_ flags.
 */
ساکن ثابت حرف opchars[][3] =
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
    صحیح
get_op_type(صحیح char1, صحیح char2)
{
    صحیح		i;

    اگر (char1 == 'r')		// ignore second character
	واپس OP_REPLACE;
    اگر (char1 == '~')		// when tilde is an operator
	واپس OP_TILDE;
    اگر (char1 == 'g' && char2 == Ctrl_A)	//جوڑو
	واپس OP_NR_جوڑو;
    اگر (char1 == 'g' && char2 == Ctrl_X)	//تفریق
	واپس OP_NR_SUB;
    اگر (char1 == 'z' && char2 == 'y')	// OP_YANK
	واپس OP_YANK;
    برائے (i = 0; ; ++i)
    {
	اگر (opchars[i][0] == char1 && opchars[i][1] == char2)
	    توڑو;
	اگر (i == (صحیح)ARRAY_لمبائی(opchars) - 1)
	{
	    اندرونی_error("get_op_type()");
	    توڑو;
	}
    }
    واپس i;
}

/*
 * Return TRUE if operator "op" always works on whole lines.
 */
    ساکن صحیح
op_on_سطریں(صحیح op)
{
    واپس opchars[op][2] & OPF_سطریں;
}

#اگر defined(FEAT_کام_CHANNEL)
/*
 * Return TRUE if operator "op" changes text.
 */
    صحیح
op_is_change(صحیح op)
{
    واپس opchars[op][2] & OPF_CHANGE;
}
#اگر_ختم

/*
 * Get first operator command character.
 * Returns 'g' or 'z' if there is another command character.
 */
    صحیح
get_op_char(صحیح optype)
{
    واپس opchars[optype][0];
}

/*
 * Get second operator command character.
 */
    صحیح
get_extra_op_char(صحیح optype)
{
    واپس opchars[optype][1];
}

/*
 * op_shift - handle a shift operation
 */
    خالی
op_سرکاؤ(oparg_T *oap, صحیح curs_چوٹی, صحیح amount)
{
    طویل	    i;
    صحیح		    پہلا_char;
    صحیح		    بلاک_ستون = 0;

    اگر (u_save((linenr_T)(oap->آغاز.lnum - 1),
				       (linenr_T)(oap->انتہا.lnum + 1)) == FAIL)
	واپس;

    اگر (oap->بلاک_منوال)
	بلاک_ستون = curwin->w_cursor.ستون;

    برائے (i = oap->سطر_گنتی; --i >= 0; )
    {
	پہلا_char = *ml_get_curline();
	اگر (پہلا_char == NUL)				// empty line
	    curwin->w_cursor.ستون = 0;
	ورنہ اگر (oap->بلاک_منوال)
	    سرکاؤ_بلاک(oap, amount);
	ورنہ
	    // Move the line right if it doesn't start with '#', 'smartindent'
	    // isn't set or 'cindent' isn't set or '#' isn't in 'cino'.
	    اگر (پہلا_char != '#' || !preprocs_بایاں())
		سرکاؤ_سطر(oap->op_type == OP_LSHIFT, p_sr, amount, FALSE);
	++curwin->w_cursor.lnum;
    }

    بدلا_سطریں(oap->آغاز.lnum, 0, oap->انتہا.lnum + 1, 0L);
    اگر (oap->بلاک_منوال)
    {
	curwin->w_cursor.lnum = oap->آغاز.lnum;
	curwin->w_cursor.ستون = بلاک_ستون;
    }
    ورنہ اگر (curs_چوٹی)	    // put cursor on first line, for ">>"
    {
	curwin->w_cursor.lnum = oap->آغاز.lnum;
	beginline(BL_SOL | BL_FIX);   // shift_line() may have set cursor.col
    }
    ورنہ
	--curwin->w_cursor.lnum;	// put cursor on last line, for ":>"

#اگر_تعریف_شدہ FEAT_FOLDING
    // The cursor line is not in a closed fold
    foldOpenCursor();
#اگر_ختم


    اگر (oap->سطر_گنتی > p_report)
    {
	حرف	    *op;
	حرف	    *پیغام_سطر_single;
	حرف	    *پیغام_سطر_plural;

	اگر (oap->op_type == OP_RSHIFT)
	    op = ">";
	ورنہ
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

    اگر ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_آغاز = oap->آغاز;
	curbuf->b_op_انتہا.lnum = oap->انتہا.lnum;
	curbuf->b_op_انتہا.ستون = ml_get_len(oap->انتہا.lnum);
	اگر (curbuf->b_op_انتہا.ستون > 0)
	    --curbuf->b_op_انتہا.ستون;
    }
}

#اگر_تعریف_شدہ FEAT_VARTABS
/*
 * Return the tabstop width at the index of the variable tabstop array.  If an
 * index greater than the length of the array is given, the last tabstop width
 * in the array is returned.
 */
    ساکن صحیح
get_vts(صحیح *vts_array, صحیح اشاریہ)
{
    صحیح	ts;

    اگر (اشاریہ < 1)
	ts = 0;
    ورنہ اگر (اشاریہ <= vts_array[0])
	ts = vts_array[اشاریہ];
    ورنہ
	ts = vts_array[vts_array[0]];

    واپس ts;
}

/*
 * Return the sum of all the tabstops through the index-th.
 */
    ساکن صحیح
get_vts_مجموع(صحیح *vts_array, صحیح اشاریہ)
{
    صحیح	مجموع = 0;
    صحیح	i;

    // Perform the summation for indices within the actual array.
    برائے (i = 1; i <= اشاریہ && i <= vts_array[0]; i++)
	مجموع += vts_array[i];

    // Add tabstops whose indices exceed the actual array.
    اگر (i <= اشاریہ)
	مجموع += vts_array[vts_array[0]] * (اشاریہ - vts_array[0]);

    واپس مجموع;
}
#اگر_ختم

    ساکن vimlong_T
get_new_sw_indent(
    صحیح		بایاں,		// TRUE if shift is to the left
    صحیح		باری,		// TRUE if new indent is to be to a tabstop
    vimlong_T	amount,		// Number of shifts
    vimlong_T	sw_val)
{
    vimlong_T	گنتی = get_indent();
    vimlong_T	i, j;

    اگر (باری)			// round off indent
    {
	i = trim_مقصد_int(گنتی) / sw_val;	// number of 'shiftwidth' rounded down
	j = trim_مقصد_int(گنتی) % sw_val;	// extra spaces
	اگر (j && بایاں)		// first remove extra spaces
	    --amount;
	اگر (بایاں)
	{
	    i -= amount;
	    اگر (i < 0)
		i = 0;
	}
	ورنہ
	    i += amount;
	گنتی = i * sw_val;
    }
    ورنہ			// original vi indent
    {
	اگر (بایاں)
	{
	    گنتی -= sw_val * amount;
	    اگر (گنتی < 0)
		گنتی = 0;
	}
	ورنہ
	    گنتی += sw_val * amount;
    }

    واپس گنتی;
}

#اگر_تعریف_شدہ FEAT_VARTABS
    ساکن vimlong_T
get_new_vts_indent(
    صحیح	بایاں,			// TRUE if shift is to the left
    صحیح	باری,			// TRUE if new indent is to be to a tabstop
    صحیح	amount,			// Number of shifts
    صحیح	*vts_array)
{
    vimlong_T	indent = get_indent();
    صحیح		vtsi = 0;
    صحیح		vts_indent = 0;
    صحیح		ts = 0;		// Silence uninitialized variable warning.
    صحیح		انحراف;		// Extra indent spaces to the right of the
				// tabstop

    // Find the tabstop at or to the left of the current indent.
    جب (vts_indent <= indent)
    {
	vtsi++;
	ts = get_vts(vts_array, vtsi);
	vts_indent += ts;
    }
    vts_indent -= ts;
    vtsi--;

    انحراف = indent - vts_indent;

    اگر (باری)
    {
	اگر (بایاں)
	{
	    اگر (انحراف == 0)
		indent = get_vts_مجموع(vts_array, vtsi - amount);
	    ورنہ
		indent = get_vts_مجموع(vts_array, vtsi - (amount - 1));
	}
	ورنہ
	    indent = get_vts_مجموع(vts_array, vtsi + amount);
    }
    ورنہ
    {
	اگر (بایاں)
	{
	    اگر (amount > vtsi)
		indent = 0;
	    ورنہ
		indent = get_vts_مجموع(vts_array, vtsi - amount) + انحراف;
	}
	ورنہ
	    indent = get_vts_مجموع(vts_array, vtsi + amount) + انحراف;
    }

    واپس indent;
}
#اگر_ختم

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
    خالی
سرکاؤ_سطر(
    صحیح	بایاں,			// TRUE if shift is to the left
    صحیح	باری,			// TRUE if new indent is to be to a tabstop
    صحیح	amount,			// Number of shifts
    صحیح	call_بدلا_بائٹیں)	// call changed_bytes()
{
    vimlong_T	گنتی;
    طویل	sw_val = curbuf->b_p_sw;
    طویل	ts_val = curbuf->b_p_ts;
#اگر_تعریف_شدہ FEAT_VARTABS
    صحیح		*vts_array = curbuf->b_p_vts_array;
#اگر_ختم

    اگر (sw_val != 0)
	// 'shiftwidth' is not zero; use it as the shift size.
	گنتی = get_new_sw_indent(بایاں, باری, amount, sw_val);
    ورنہ
#اگر_تعریف_شدہ FEAT_VARTABS
	اگر ((vts_array == تہی) || (vts_array[0] == 0))
#اگر_ختم
    {
	// 'shiftwidth is zero and 'vartabstop' is empty; use 'tabstop' as the
	// shift size.
	گنتی = get_new_sw_indent(بایاں, باری, amount, ts_val);
    }
#اگر_تعریف_شدہ FEAT_VARTABS
    ورنہ
    {
	// 'shiftwidth is zero and 'vartabstop' is defined; use 'vartabstop'
	// to determine the new indent.
	گنتی = get_new_vts_indent(بایاں, باری, amount, vts_array);
    }
#اگر_ختم

    // Set new indent
    اگر (State & VREPLACE_FLAG)
	change_indent(INDENT_سیٹ, trim_مقصد_int(گنتی), FALSE, NUL, call_بدلا_بائٹیں);
    ورنہ
	(خالی)سیٹ_indent(trim_مقصد_int(گنتی), call_بدلا_بائٹیں ? SIN_بدلا : 0);
}

/*
 * Shift one line of the current block one shiftwidth right or left.
 * Leaves cursor on first character in block.
 */
    ساکن خالی
سرکاؤ_بلاک(oparg_T *oap, صحیح amount)
{
    صحیح			بایاں = (oap->op_type == OP_LSHIFT);
    صحیح			oldstate = State;
    صحیح			کل;
    char_u		*newp, *oldp;
    size_t		newlen, oldlen;
    صحیح			oldcol = curwin->w_cursor.ستون;
    صحیح			sw_val = (صحیح)get_sw_قدر_indent(curbuf, بایاں);
    صحیح			ts_val = (صحیح)curbuf->b_p_ts;
    ساخت بلاک_def	bd;
    صحیح			incr;
    colnr_T		ws_vcol;
    صحیح			added;
    size_t		new_سطر_len;	// the length of the line after the
					// block shift
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    صحیح			old_p_ri = p_ri;

    p_ri = 0;			// don't want revins in indent
#اگر_ختم

    State = منوال_داخل_کرو;	// don't want MODE_REPLACE for State
    بلاک_prep(oap, &bd, curwin->w_cursor.lnum, TRUE);
    اگر (bd.is_short)
	واپس;

    // total is number of screen columns to be inserted/removed
    کل = (صحیح)((غیرعلامتی)amount * (غیرعلامتی)sw_val);
    اگر ((کل / sw_val) != amount)
	واپس; // multiplication overflow

    oldp = ml_get_curline();
    oldlen = ml_get_curline_len();

    اگر (!بایاں)
    {
	صحیح		tabs = 0, spaces = 0;
	chartabsize_T	cts;

	/*
	 *  1. Get start vcol
	 *  2. Total ws vcols
	 *  3. Divvy into TABs & spp
	 *  4. Construct new string
	 */
	کل += bd.pre_whitesp; // all virtual WS up to & incl a split TAB
	ws_vcol = bd.آغاز_vcol - bd.pre_whitesp;
	اگر (bd.startspaces)
	{
	    اگر (has_mbyte)
	    {
		اگر ((*mb_ptr2len)(bd.textstart) == 1)
		    ++bd.textstart;
		ورنہ
		{
		    ws_vcol = 0;
		    bd.startspaces = 0;
		}
	    }
	    ورنہ
		++bd.textstart;
	}

	// TODO: is passing bd.textstart for start of the line OK?
	init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum,
				   bd.آغاز_vcol, bd.textstart, bd.textstart);
	برائے ( ; VIM_ISWHITE(*cts.cts_ptr); )
	{
	    incr = lbr_chartabsize_adv(&cts);
	    کل += incr;
	    cts.cts_vcol += incr;
	}
	bd.textstart = cts.cts_ptr;
	bd.آغاز_vcol = cts.cts_vcol;
	clear_chartabsize_arg(&cts);

	// OK, now total=all the VWS reqd, and textstart points at the 1st
	// non-ws char in the block.
#اگر_تعریف_شدہ FEAT_VARTABS
	اگر (!curbuf->b_p_et)
	    tabstop_fromto(ws_vcol, ws_vcol + کل,
				ts_val, curbuf->b_p_vts_array, &tabs, &spaces);
	ورنہ
	    spaces = کل;
#ورنہ
	اگر (!curbuf->b_p_et)
	    tabs = ((ws_vcol % ts_val) + کل) / ts_val; // number of tabs
	اگر (tabs > 0)
	    spaces = ((ws_vcol % ts_val) + کل) % ts_val; // number of spp
	ورنہ
	    spaces = کل;
#اگر_ختم
	// if we're splitting a TAB, allow for it
	bd.textcol -= bd.pre_whitesp_c - (bd.startspaces != 0);

	new_سطر_len = bd.textcol + tabs + spaces + (oldlen - (bd.textstart - oldp));
	newp = alloc(new_سطر_len + 1);
	اگر (newp == تہی)
	    واپس;
	mch_memmove(newp, oldp, (size_t)bd.textcol);
	newlen = bd.textcol;
	vim_memset(newp + newlen, TAB, (size_t)tabs);
	newlen += tabs;
	vim_memset(newp + newlen, ' ', (size_t)spaces);
	STRCPY(newp + newlen + spaces, bd.textstart);
    }
    ورنہ //بایاں
    {
	colnr_T	    destination_ستون;	// column to which text in block will
					// be shifted
	char_u	    *verbatim_copy_انتہا;	// end of the part of the line which is
					// copied verbatim
	colnr_T	    verbatim_copy_چوڑائی;// the (displayed) width of this part
					// of line
	size_t	    fill;		// nr of spaces that replace a TAB
	size_t	    بلاک_space_چوڑائی;
	size_t	    سرکاؤ_amount;
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
	اگر (bd.startspaces)
	    MB_PTR_ADV(non_white);

	// The character's column is in "bd.start_vcol".
	non_white_ستون = bd.آغاز_vcol;

	init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum,
				   non_white_ستون, bd.textstart, non_white);
	جب (VIM_ISWHITE(*cts.cts_ptr))
	{
	    incr = lbr_chartabsize_adv(&cts);
	    cts.cts_vcol += incr;
	}
	non_white_ستون = cts.cts_vcol;
	non_white = cts.cts_ptr;
	clear_chartabsize_arg(&cts);

	بلاک_space_چوڑائی = non_white_ستون - oap->آغاز_vcol;
	// We will shift by "total" or "block_space_width", whichever is less.
	سرکاؤ_amount = (بلاک_space_چوڑائی < (size_t)کل
					 ? بلاک_space_چوڑائی : (size_t)کل);

	// The column to which we will shift the text.
	destination_ستون = (colnr_T)(non_white_ستون - سرکاؤ_amount);

	// Now let's find out how much of the beginning of the line we can
	// reuse without modification.
	verbatim_copy_انتہا = bd.textstart;
	verbatim_copy_چوڑائی = bd.آغاز_vcol;

	// If "bd.startspaces" is set, "bd.textstart" points to the character
	// preceding the block. We have to subtract its width to obtain its
	// column number.
	اگر (bd.startspaces)
	    verbatim_copy_چوڑائی -= bd.آغاز_char_vcols;
	init_chartabsize_arg(&cts, curwin, 0, verbatim_copy_چوڑائی,
					     bd.textstart, verbatim_copy_انتہا);
	جب (cts.cts_vcol < destination_ستون)
	{
	    incr = lbr_chartabsize(&cts);
	    اگر (cts.cts_vcol + incr > destination_ستون)
		توڑو;
	    cts.cts_vcol += incr;
	    MB_PTR_ADV(cts.cts_ptr);
	}
	verbatim_copy_چوڑائی = cts.cts_vcol;
	verbatim_copy_انتہا = cts.cts_ptr;
	clear_chartabsize_arg(&cts);

	// If "destination_col" is different from the width of the initial
	// part of the line that will be copied, it means we encountered a tab
	// character, which we will have to partly replace with spaces.
	fill = destination_ستون - verbatim_copy_چوڑائی;

	// The replacement line will consist of:
	// - the beginning of the original line up to "verbatim_copy_end",
	// - "fill" number of spaces,
	// - the rest of the line, pointed to by non_white.
	fixedlen = verbatim_copy_انتہا - oldp;
	new_سطر_len = fixedlen + fill + (oldlen - (non_white - oldp));

	newp = alloc(new_سطر_len + 1);
	اگر (newp == تہی)
	    واپس;
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
    ورنہ
	added = 0 - (صحیح)(oldlen - new_سطر_len);
    inserted_بائٹیں(curwin->w_cursor.lnum, bd.textcol, added);
    State = oldstate;
    curwin->w_cursor.ستون = oldcol;
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    p_ri = old_p_ri;
#اگر_ختم
}

/*
 * Insert string "s" (b_insert ? before : after) block :AKelly
 * Caller must prepare for undo.
 */
    ساکن خالی
بلاک_داخل_کرو(
    oparg_T		*oap,
    char_u		*s,
    size_t		slen,
    صحیح			b_داخل_کرو,
    ساخت بلاک_def	*bdp)
{
    صحیح		ts_val;
    صحیح		گنتی = 0;	// extra spaces to replace a cut TAB
    صحیح		spaces = 0;	// non-zero if cutting a TAB
    colnr_T	انحراف;		// pointer along new line
    colnr_T	startcol;	// column where insert starts
    char_u	*newp, *oldp;	// new, old lines
    linenr_T	lnum;		// loop var
    صحیح		oldstate = State;

    State = منوال_داخل_کرو;	// don't want MODE_REPLACE for State

    برائے (lnum = oap->آغاز.lnum + 1; lnum <= oap->انتہا.lnum; lnum++)
    {
	بلاک_prep(oap, bdp, lnum, TRUE);
	اگر (bdp->is_short && b_داخل_کرو)
	    جاری;	// OP_INSERT, line ends before block start

	oldp = ml_get(lnum);

	اگر (b_داخل_کرو)
	{
	    ts_val = bdp->آغاز_char_vcols;
	    spaces = bdp->startspaces;
	    اگر (spaces != 0)
		گنتی = ts_val - 1; // we're cutting a TAB
	    انحراف = bdp->textcol;
	}
	ورنہ // append
	{
	    ts_val = bdp->انتہا_char_vcols;
	    اگر (!bdp->is_short) // spaces = padding after block
	    {
		spaces = (bdp->endspaces ? ts_val - bdp->endspaces : 0);
		اگر (spaces != 0)
		    گنتی = ts_val - 1; // we're cutting a TAB
		انحراف = bdp->textcol + bdp->textlen - (spaces != 0);
	    }
	    ورنہ // spaces = padding to block edge
	    {
		// if $ used, just append to EOL (ie spaces==0)
		اگر (!bdp->is_MAX)
		    spaces = (oap->انتہا_vcol - bdp->انتہا_vcol) + 1;
		گنتی = spaces;
		انحراف = bdp->textcol + bdp->textlen;
	    }
	}

	اگر (has_mbyte && spaces > 0)
	    // avoid copying part of a multi-byte character
	    انحراف -= (*mb_سر_off)(oldp, oldp + انحراف);

	اگر (spaces < 0)  // can happen when the cursor was moved
	    spaces = 0;

	// Make sure the allocated size matches what is actually copied below.
	newp = alloc(ml_get_len(lnum) + spaces + slen
		    + (spaces > 0 && !bdp->is_short ? ts_val - spaces : 0)
								  + گنتی + 1);
	اگر (newp == تہی)
	    جاری;

	// copy up to shifted part
	mch_memmove(newp, oldp, (size_t)انحراف);
	oldp += انحراف;

	// insert pre-padding
	vim_memset(newp + انحراف, ' ', (size_t)spaces);
	startcol = انحراف + spaces;

	// copy the new text
	mch_memmove(newp + startcol, s, slen);
	انحراف += (صحیح)slen;

	اگر (spaces > 0 && !bdp->is_short)
	{
	    اگر (*oldp == TAB)
	    {
		// insert post-padding
		vim_memset(newp + انحراف + spaces, ' ',
						    (size_t)(ts_val - spaces));
		// we're splitting a TAB, don't copy it
		oldp++;
		// We allowed for that TAB, remember this now
		گنتی++;
	    }
	    ورنہ
		// Not a TAB, no extra spaces
		گنتی = spaces;
	}

	اگر (spaces > 0)
	    انحراف += گنتی;
	STRCPY(newp + انحراف, oldp);

	ml_replace(lnum, newp, FALSE);

	اگر (b_داخل_کرو)
	    // correct any text properties
	    inserted_بائٹیں(lnum, startcol, (صحیح)slen);

	اگر (lnum == oap->انتہا.lnum)
	{
	    // Set "']" mark to the end of the block instead of the end of
	    // the insert in the first line.
	    curbuf->b_op_انتہا.lnum = oap->انتہا.lnum;
	    curbuf->b_op_انتہا.ستون = انحراف;
	    اگر (curbuf->b_visual.vi_انتہا.coladd)
	    {
		curbuf->b_visual.vi_انتہا.ستون += curbuf->b_visual.vi_انتہا.coladd;
		curbuf->b_visual.vi_انتہا.coladd = 0;
	    }
	}
    } // for all lnum

    بدلا_سطریں(oap->آغاز.lnum + 1, 0, oap->انتہا.lnum + 1, 0L);

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
    ساخت بلاک_def	bd;
    linenr_T		old_lcount = curbuf->b_ml.ml_سطر_گنتی;
    صحیح			did_yank = FALSE;

    اگر (curbuf->b_ml.ml_flags & ML_خالی_خانے)	    // nothing to do
	واپس OK;

    // Nothing to delete, return here.	Do prepare undo, for op_change().
    اگر (oap->خالی_خانے_2)
	واپس u_save_cursor();

    اگر (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	واپس FAIL;
    }

    اگر (VIsual_select && oap->is_VIsual)
	// use register given with CTRL_R, defaults to zero
	oap->regname = VIsual_select_reg;

#اگر_تعریف_شدہ HAVE_CLIPMETHOD
    adjust_clip_reg(&oap->regname);
#اگر_ختم

    اگر (has_mbyte)
	mb_adjust_opend(oap);

    /*
     * Imitate the strange Vi behaviour: If the delete spans more than one
     * line and motion_type == MCHAR and the result is a blank line, make the
     * delete linewise.  Don't do this for the change command or Visual mode.
     */
    اگر (       oap->motion_type == MCHAR
	    && !oap->is_VIsual
	    && !oap->بلاک_منوال
	    && oap->سطر_گنتی > 1
	    && oap->motion_force == NUL
	    && (vim_strchr(p_cpo, CPO_لفظ) != تہی)
	    && oap->op_type == OP_DELETE)
    {
	ptr = ml_get(oap->انتہا.lnum) + oap->انتہا.ستون;
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
	    && oap->سطر_گنتی == 1
	    && oap->op_type == OP_DELETE
	    && *ml_get(oap->آغاز.lnum) == NUL)
    {
	/*
	 * It's an error to operate on an empty region, when 'E' included in
	 * 'cpoptions' (Vi compatible).
	 */
	اگر (virtual_op)
	    // Virtual editing: Nothing gets deleted, but we set the '[ and ']
	    // marks as if it happened.
	    جاؤ setmarks;
	اگر (vim_strchr(p_cpo, CPO_EMPTYREGION) != تہی)
	    beep_flush();
	واپس OK;
    }

    /*
     * Do a yank of whatever we're about to delete.
     * If a yank register was specified, put the deleted text into that
     * register.  For the black hole register '_' don't yank anything.
     */
    اگر (oap->regname != '_')
    {
#اگر_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
	inc_clip_provider();
#اگر_ختم
	اگر (oap->regname != 0)
	{
	    // check for read-only register
	    اگر (!valid_yank_reg(oap->regname, TRUE))
	    {
		beep_flush();
#اگر_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
		dec_clip_provider();
#اگر_ختم
		واپس OK;
	    }
	    get_yank_رجسٹر(oap->regname, TRUE); // yank into specif'd reg.
	    اگر (op_yank(oap, TRUE, FALSE) == OK)   // yank without message
		did_yank = TRUE;
	}
	ورنہ
	    reset_y_append(); // not appending to unnamed register

	/*
	 * Put deleted text into register 1 and shift number registers if the
	 * delete contains a line break, or when using a specific operator (Vi
	 * compatible)
	 */
	اگر (oap->motion_type == MLINE || oap->سطر_گنتی > 1
							   || oap->use_reg_one)
	{
	    سرکاؤ_delete_registers();
	    اگر (op_yank(oap, TRUE, FALSE) == OK)
		did_yank = TRUE;
	}

	// Yank into small delete register when no named register specified
	// and the delete is within one line.
	اگر ((
#اگر_تعریف_شدہ HAVE_CLIPMETHOD
	    ((clip_unnamed & CLIP_UNNAMED) && oap->regname == '*') ||
	    ((clip_unnamed & CLIP_UNNAMED_PLUS) && oap->regname == '+') ||
#اگر_ختم
	    oap->regname == 0) && oap->motion_type != MLINE
						      && oap->سطر_گنتی == 1)
	{
	    oap->regname = '-';
	    get_yank_رجسٹر(oap->regname, TRUE);
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
	    صحیح پیغام_silent_save = پیغام_silent;

	    پیغام_silent = 0;	// must display the prompt
	    n = ask_yesno((char_u *)_("cannot yank; delete anyway"), TRUE);
	    پیغام_silent = پیغام_silent_save;
	    اگر (n != 'y')
	    {
		emsg(_(e_command_aborted));
#اگر_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
		dec_clip_provider();
#اگر_ختم
		واپس FAIL;
	    }
	}

#اگر defined(FEAT_EVAL)
	اگر (did_yank && has_textyankpost())
	    yank_do_autocmd(oap, get_y_current());
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#اگر_ختم
    }

    /*
     * block mode delete
     */
    اگر (oap->بلاک_منوال)
    {
	اگر (u_save((linenr_T)(oap->آغاز.lnum - 1),
			       (linenr_T)(oap->انتہا.lnum + 1)) == FAIL)
	    واپس FAIL;

	برائے (lnum = curwin->w_cursor.lnum; lnum <= oap->انتہا.lnum; ++lnum)
	{
	    بلاک_prep(oap, &bd, lnum, TRUE);
	    اگر (bd.textlen == 0)	// nothing to delete
		جاری;

	    // Adjust cursor position for tab replaced by spaces and 'lbr'.
	    اگر (lnum == curwin->w_cursor.lnum)
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
	    اگر (newp == تہی)
		جاری;
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

#اگر_تعریف_شدہ FEAT_PROP_POPUP
	    اگر (curbuf->b_has_textprop && n != 0)
		adjust_prop_columns(lnum, bd.textcol, -n, 0);
#اگر_ختم
	}

	check_cursor_ستون();
	بدلا_سطریں(curwin->w_cursor.lnum, curwin->w_cursor.ستون,
						       oap->انتہا.lnum + 1, 0L);
	oap->سطر_گنتی = 0;	    // no lines deleted
    }
    ورنہ اگر (oap->motion_type == MLINE)
    {
	اگر (oap->op_type == OP_CHANGE)
	{
	    // Delete the lines except the first one.  Temporarily move the
	    // cursor to the next line.  Save the current line number, if the
	    // last line is deleted it may be changed.
	    اگر (oap->سطر_گنتی > 1)
	    {
		lnum = curwin->w_cursor.lnum;
		++curwin->w_cursor.lnum;
		del_سطریں((طویل)(oap->سطر_گنتی - 1), TRUE);
		curwin->w_cursor.lnum = lnum;
	    }
	    اگر (u_save_cursor() == FAIL)
		واپس FAIL;
	    اگر (curbuf->b_p_ai)		    // don't delete indent
	    {
		beginline(BL_WHITE);	    // cursor on first non-white
		did_ai = TRUE;		    // delete the indent when ESC hit
		ai_ستون = curwin->w_cursor.ستون;
	    }
	    ورنہ
		beginline(0);		    // cursor in column 0
	    truncate_سطر(FALSE);   // delete the rest of the line,
				    // leaving cursor past last char in line
	    اگر (oap->سطر_گنتی > 1)
		u_clearline();	    // "U" command not possible after "2cc"
	}
	ورنہ
	{
	    del_سطریں(oap->سطر_گنتی, TRUE);
	    beginline(BL_WHITE | BL_FIX);
	    u_clearline();	// "U" command not possible after "dd"
	}
    }
    ورنہ
    {
	اگر (virtual_op)
	{
	    صحیح		endcol = 0;

	    // For virtualedit: break the tabs that are partly included.
	    اگر (gchar_pos(&oap->آغاز) == '\t')
	    {
		اگر (u_save_cursor() == FAIL)	// save first line for undo
		    واپس FAIL;
		اگر (oap->سطر_گنتی == 1)
		    endcol = getviscol2(oap->انتہا.ستون, oap->انتہا.coladd);
		coladvance_force(getviscol2(oap->آغاز.ستون, oap->آغاز.coladd));
		oap->آغاز = curwin->w_cursor;
		اگر (oap->سطر_گنتی == 1)
		{
		    coladvance(endcol);
		    oap->انتہا.ستون = curwin->w_cursor.ستون;
		    oap->انتہا.coladd = curwin->w_cursor.coladd;
		    curwin->w_cursor = oap->آغاز;
		}
	    }

	    // Break a tab only when it's included in the area.
	    اگر (gchar_pos(&oap->انتہا) == '\t'
				     && (صحیح)oap->انتہا.coladd < oap->inclusive)
	    {
		// save last line for undo
		اگر (u_save((linenr_T)(oap->انتہا.lnum - 1),
				       (linenr_T)(oap->انتہا.lnum + 1)) == FAIL)
		    واپس FAIL;
		curwin->w_cursor = oap->انتہا;
		coladvance_force(getviscol2(oap->انتہا.ستون, oap->انتہا.coladd));
		oap->انتہا = curwin->w_cursor;
		curwin->w_cursor = oap->آغاز;
	    }
	    اگر (has_mbyte)
		mb_adjust_opend(oap);
	}

	اگر (oap->سطر_گنتی == 1)	// delete characters within one line
	{
	    اگر (u_save_cursor() == FAIL)	// save line for undo
		واپس FAIL;

	    // if 'cpoptions' contains '$', display '$' at end of change
	    اگر (       vim_strchr(p_cpo, CPO_DOLLAR) != تہی
		    && oap->op_type == OP_CHANGE
		    && oap->انتہا.lnum == curwin->w_cursor.lnum
		    && !oap->is_VIsual)
		display_dollar(oap->انتہا.ستون - !oap->inclusive);

	    n = oap->انتہا.ستون - oap->آغاز.ستون + 1 - !oap->inclusive;

	    اگر (virtual_op)
	    {
		// fix up things for virtualedit-delete:
		// break the tabs which are going to get in our way
		صحیح		len = ml_get_curline_len();

		اگر (oap->انتہا.coladd != 0
			&& (صحیح)oap->انتہا.ستون >= len - 1
			&& !(oap->آغاز.coladd && (صحیح)oap->انتہا.ستون >= len - 1))
		    n++;
		// Delete at least one char (e.g, when on a control char).
		اگر (n == 0 && oap->آغاز.coladd != oap->انتہا.coladd)
		    n = 1;

		// When deleted a char in the line, reset coladd.
		اگر (gchar_cursor() != NUL)
		    curwin->w_cursor.coladd = 0;
	    }
	    (خالی)del_بائٹیں((طویل)n, !virtual_op,
			    oap->op_type == OP_DELETE && !oap->is_VIsual);
	}
	ورنہ				// delete characters between lines
	{
	    pos_T   curpos;

	    // save deleted and changed lines for undo
	    اگر (u_save((linenr_T)(curwin->w_cursor.lnum - 1),
		 (linenr_T)(curwin->w_cursor.lnum + oap->سطر_گنتی)) == FAIL)
		واپس FAIL;

	    truncate_سطر(TRUE);	// delete from cursor to end of line

	    curpos = curwin->w_cursor;	// remember curwin->w_cursor
	    ++curwin->w_cursor.lnum;
	    del_سطریں((طویل)(oap->سطر_گنتی - 2), FALSE);

	    // delete from start of line until op_end
	    n = (oap->انتہا.ستون + 1 - !oap->inclusive);
	    curwin->w_cursor.ستون = 0;
	    (خالی)del_بائٹیں((طویل)n, !virtual_op,
			    oap->op_type == OP_DELETE && !oap->is_VIsual);
	    curwin->w_cursor = curpos;	// restore curwin->w_cursor
	    (خالی)do_join(2, FALSE, FALSE, FALSE, FALSE);
	}
	اگر (oap->op_type == OP_DELETE)
	    auto_format(FALSE, TRUE);
    }

    msgmore(curbuf->b_ml.ml_سطر_گنتی - old_lcount);

setmarks:
    اگر ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	اگر (oap->بلاک_منوال)
	{
	    curbuf->b_op_انتہا.lnum = oap->انتہا.lnum;
	    curbuf->b_op_انتہا.ستون = oap->آغاز.ستون;
	}
	ورنہ
	    curbuf->b_op_انتہا = oap->آغاز;
	curbuf->b_op_آغاز = oap->آغاز;
    }

    واپس OK;
}

/*
 * Adjust end of operating area for ending on a multi-byte character.
 * Used for deletion.
 */
    ساکن خالی
mb_adjust_opend(oparg_T *oap)
{
    char_u	*سطر;
    char_u	*ptr;

    اگر (!oap->inclusive)
	واپس;

    سطر = ml_get(oap->انتہا.lnum);
    ptr = سطر + oap->انتہا.ستون;
    اگر (*ptr != NUL)
    {
	ptr -= (*mb_سر_off)(سطر, ptr);
	ptr += (*mb_ptr2len)(ptr) - 1;
	oap->انتہا.ستون = ptr - سطر;
    }
}

/*
 * Replace the character under the cursor with "c".
 * This takes care of multi-byte characters.
 */
    ساکن خالی
replace_character(صحیح c)
{
    صحیح n = State;

    State = منوال_REPLACE;
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
    صحیح			عدد_chars;
    char_u		*newp, *oldp;
    size_t		newlen, oldlen;
    ساخت بلاک_def	bd;
    char_u		*after_p = تہی;
    صحیح			had_ctrl_v_cr = FALSE;

    اگر ((curbuf->b_ml.ml_flags & ML_خالی_خانے ) || oap->خالی_خانے_2)
	واپس OK;	    // nothing to do

    اگر (c == REPLACE_CR_NCHAR)
    {
	had_ctrl_v_cr = TRUE;
	c = CAR;
    }
    ورنہ اگر (c == REPLACE_NL_NCHAR)
    {
	had_ctrl_v_cr = TRUE;
	c = NL;
    }

    اگر (has_mbyte)
	mb_adjust_opend(oap);

    اگر (u_save((linenr_T)(oap->آغاز.lnum - 1),
				       (linenr_T)(oap->انتہا.lnum + 1)) == FAIL)
	واپس FAIL;

    /*
     * block mode replace
     */
    اگر (oap->بلاک_منوال)
    {
	bd.is_MAX = (curwin->w_curswant == MAXCOL);
	برائے ( ; curwin->w_cursor.lnum <= oap->انتہا.lnum; ++curwin->w_cursor.lnum)
	{
	    curwin->w_cursor.ستون = 0;  // make sure cursor position is valid
	    بلاک_prep(oap, &bd, curwin->w_cursor.lnum, TRUE);
	    اگر (bd.textlen == 0 && (!virtual_op || bd.is_MAX))
		جاری;	    // nothing to replace

	    // n == number of extra chars required
	    // If we split a TAB, it may be replaced by several characters.
	    // Thus the number of characters may increase!
	    // If the range starts in virtual space, count the initial
	    // coladd offset as part of "startspaces"
	    اگر (virtual_op && bd.is_short && *bd.textstart == NUL)
	    {
		pos_T vpos;

		vpos.lnum = curwin->w_cursor.lnum;
		getvpos(&vpos, oap->آغاز_vcol);
		bd.startspaces += vpos.coladd;
		n = bd.startspaces;
	    }
	    ورنہ
		// allow for pre spaces
		n = (bd.startspaces ? bd.آغاز_char_vcols - 1 : 0);

	    // allow for post spp
	    n += (bd.endspaces
		    && !bd.is_oneChar
		    && bd.انتہا_char_vcols > 0) ? bd.انتہا_char_vcols - 1 : 0;
	    // Figure out how many characters to replace.
	    numc = oap->انتہا_vcol - oap->آغاز_vcol + 1;
	    اگر (bd.is_short && (!virtual_op || bd.is_MAX))
		numc -= (oap->انتہا_vcol - bd.انتہا_vcol) + 1;

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
	    عدد_chars = numc;
	    numc *= (*mb_char2len)(c);
	    // oldlen includes textlen, so don't double count
	    n += numc - bd.textlen;

	    oldp = ml_get_curline();
	    oldlen = ml_get_curline_len();
	    newp = alloc(oldlen + 1 + n);
	    اگر (newp == تہی)
		جاری;
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
		    جب (--عدد_chars >= 0)
			newlen += (*mb_char2bytes)(c, newp + newlen);
		}
		ورنہ
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
	    ورنہ
	    {
		// Replacing with \r or \n means splitting the line.
		after_p = alloc(oldlen + 1 + n - newlen);
		اگر (after_p != تہی)
		    STRCPY(after_p, oldp + bd.textcol + bd.textlen);
	    }

	    // replace the line
	    ml_replace(curwin->w_cursor.lnum, newp, FALSE);
	    اگر (after_p != تہی)
	    {
		ml_append(curwin->w_cursor.lnum++, after_p, 0, FALSE);
		appended_سطریں_mark(curwin->w_cursor.lnum, 1L);
		oap->انتہا.lnum++;
		vim_free(after_p);
	    }
	}
    }
    ورنہ
    {
	/*
	 * MCHAR and MLINE motion replace.
	 */
	اگر (oap->motion_type == MLINE)
	{
	    oap->آغاز.ستون = 0;
	    curwin->w_cursor.ستون = 0;
	    oap->انتہا.ستون = ml_get_len(oap->انتہا.lnum);
	    اگر (oap->انتہا.ستون)
		--oap->انتہا.ستون;
	}
	ورنہ اگر (!oap->inclusive)
	    dec(&(oap->انتہا));

	جب (LTOREQ_POS(curwin->w_cursor, oap->انتہا))
	{
	    صحیح مکمل = FALSE;

	    n = gchar_cursor();
	    اگر (n != NUL)
	    {
		صحیح new_byte_len = (*mb_char2len)(c);
		صحیح old_byte_len = mb_ptr2len(ml_get_cursor());

		اگر (new_byte_len > 1 || old_byte_len > 1)
		{
		    // This is slow, but it handles replacing a single-byte
		    // with a multi-byte and the other way around.
		    اگر (curwin->w_cursor.lnum == oap->انتہا.lnum)
			oap->انتہا.ستون += new_byte_len - old_byte_len;
		    replace_character(c);
		    مکمل = TRUE;
		}
		ورنہ
		{
		    اگر (n == TAB)
		    {
			صحیح انتہا_vcol = 0;

			اگر (curwin->w_cursor.lnum == oap->انتہا.lnum)
			{
			    // oap->end has to be recalculated when
			    // the tab breaks
			    انتہا_vcol = getviscol2(oap->انتہا.ستون,
							     oap->انتہا.coladd);
			}
			coladvance_force(getviscol());
			اگر (curwin->w_cursor.lnum == oap->انتہا.lnum)
			    getvpos(&oap->انتہا, انتہا_vcol);
		    }
		    // with "coladd" set may move to just after a TAB
		    اگر (gchar_cursor() != NUL)
		    {
			PBYTE(curwin->w_cursor, c);
			مکمل = TRUE;
		    }
		}
	    }
	    اگر (!مکمل && virtual_op && curwin->w_cursor.lnum == oap->انتہا.lnum)
	    {
		صحیح virtcols = oap->انتہا.coladd;

		اگر (curwin->w_cursor.lnum == oap->آغاز.lnum
			&& oap->آغاز.ستون == oap->انتہا.ستون && oap->آغاز.coladd)
		    virtcols -= oap->آغاز.coladd;

		// oap->end has been trimmed so it's effectively inclusive;
		// as a result an extra +1 must be counted so we don't
		// trample the NUL byte.
		coladvance_force(getviscol2(oap->انتہا.ستون, oap->انتہا.coladd) + 1);
		curwin->w_cursor.ستون -= (virtcols + 1);
		برائے (; virtcols >= 0; virtcols--)
		{
		    اگر ((*mb_char2len)(c) > 1)
		       replace_character(c);
		    ورنہ
			PBYTE(curwin->w_cursor, c);
		    اگر (inc(&curwin->w_cursor) == -1)
			توڑو;
		}
	    }

	    // Advance to next character, stop at the end of the file.
	    اگر (inc_cursor() == -1)
		توڑو;
	}
    }

    curwin->w_cursor = oap->آغاز;
    check_cursor();
    بدلا_سطریں(oap->آغاز.lnum, oap->آغاز.ستون, oap->انتہا.lnum + 1, 0L);

    اگر ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_آغاز = oap->آغاز;
	curbuf->b_op_انتہا = oap->انتہا;
    }

    واپس OK;
}

ساکن صحیح swapchars(صحیح op_type, pos_T *pos, صحیح لمبائی);

/*
 * Handle the (non-standard vi) tilde operator.  Also for "gu", "gU" and "g?".
 */
    ساکن خالی
op_tilde(oparg_T *oap)
{
    pos_T		pos;
    ساخت بلاک_def	bd;
    صحیح			did_change = FALSE;

    اگر (u_save((linenr_T)(oap->آغاز.lnum - 1),
				       (linenr_T)(oap->انتہا.lnum + 1)) == FAIL)
	واپس;

    pos = oap->آغاز;
    اگر (oap->بلاک_منوال)		    // Visual block mode
    {
	برائے (; pos.lnum <= oap->انتہا.lnum; ++pos.lnum)
	{
	    صحیح one_change;

	    بلاک_prep(oap, &bd, pos.lnum, FALSE);
	    pos.ستون = bd.textcol;
	    one_change = swapchars(oap->op_type, &pos, bd.textlen);
	    did_change |= one_change;

#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
	    اگر (netbeans_فعال() && one_change)
	    {
		char_u *ptr;

		netbeans_removed(curbuf, pos.lnum, bd.textcol,
							    (طویل)bd.textlen);
		// get the line now, it may have been flushed
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, bd.textcol,
						&ptr[bd.textcol], bd.textlen);
	    }
#اگر_ختم
	}
	اگر (did_change)
	    بدلا_سطریں(oap->آغاز.lnum, 0, oap->انتہا.lnum + 1, 0L);
    }
    ورنہ				    // not block mode
    {
	اگر (oap->motion_type == MLINE)
	{
	    oap->آغاز.ستون = 0;
	    pos.ستون = 0;
	    oap->انتہا.ستون = ml_get_len(oap->انتہا.lnum);
	    اگر (oap->انتہا.ستون)
		--oap->انتہا.ستون;
	}
	ورنہ اگر (!oap->inclusive)
	    dec(&(oap->انتہا));

	اگر (pos.lnum == oap->انتہا.lnum)
	    did_change = swapchars(oap->op_type, &pos,
						  oap->انتہا.ستون - pos.ستون + 1);
	ورنہ
	    برائے (;;)
	    {
		did_change |= swapchars(oap->op_type, &pos,
			pos.lnum == oap->انتہا.lnum ? oap->انتہا.ستون + 1
						  : ml_get_pos_len(&pos));
		اگر (LTOREQ_POS(oap->انتہا, pos) || inc(&pos) == -1)
		    توڑو;
	    }
	اگر (did_change)
	{
	    بدلا_سطریں(oap->آغاز.lnum, oap->آغاز.ستون, oap->انتہا.lnum + 1,
									  0L);
#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
	    اگر (netbeans_فعال())
	    {
		char_u *ptr;
		صحیح گنتی;

		pos = oap->آغاز;
		جب (pos.lnum < oap->انتہا.lnum)
		{
		    ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		    گنتی = ml_get_buf_len(curbuf, pos.lnum) - pos.ستون;
		    netbeans_removed(curbuf, pos.lnum, pos.ستون, (طویل)گنتی);
		    // get the line again, it may have been flushed
		    ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		    netbeans_inserted(curbuf, pos.lnum, pos.ستون,
							&ptr[pos.ستون], گنتی);
		    pos.ستون = 0;
		    pos.lnum++;
		}
		گنتی = oap->انتہا.ستون - pos.ستون + 1;
		netbeans_removed(curbuf, pos.lnum, pos.ستون, (طویل)گنتی);
		// get the line again, it may have been flushed
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, pos.ستون,
							&ptr[pos.ستون], گنتی);
	    }
#اگر_ختم
	}
    }

    اگر (!did_change && oap->is_VIsual)
	// No change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    اگر ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set '[ and '] marks.
	curbuf->b_op_آغاز = oap->آغاز;
	curbuf->b_op_انتہا = oap->انتہا;
    }

    اگر (oap->سطر_گنتی > p_report)
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
    ساکن صحیح
swapchars(صحیح op_type, pos_T *pos, صحیح لمبائی)
{
    صحیح todo;
    صحیح	did_change = 0;

    برائے (todo = لمبائی; todo > 0; --todo)
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
	    توڑو;
    }
    واپس did_change;
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
	واپس FALSE;

    // ~ is OP_NOP, g~ is OP_TILDE, gU is OP_UPPER
    اگر ((op_type == OP_بالائی || op_type == OP_NOP || op_type == OP_TILDE)
	    && c == 0xdf
	    && (enc_latin1like || STRCMP(p_enc, "iso-8859-2") == 0))
    {
	pos_T   sp = curwin->w_cursor;

	// Special handling for lowercase German sharp s (ß): convert to uppercase (ẞ).
	curwin->w_cursor = *pos;
	del_char(FALSE);
	ins_char(0x1E9E);
	curwin->w_cursor = sp;
	واپس TRUE;
    }

    اگر (enc_dbcs != 0 && c >= 0x100)	// No lower/uppercase letter
	واپس FALSE;
    nc = c;
    اگر (MB_ISLOWER(c))
    {
	اگر (op_type == OP_ROT13)
	    nc = ROT13(c, 'a');
	ورنہ اگر (op_type != OP_زیریں)
	    nc = MB_TOUPPER(c);
    }
    ورنہ اگر (MB_ISUPPER(c))
    {
	اگر (op_type == OP_ROT13)
	    nc = ROT13(c, 'A');
	ورنہ اگر (op_type != OP_بالائی)
	    nc = MB_TOLOWER(c);
    }
    اگر (nc != c)
    {
	اگر (enc_utf8 && (c >= 0x80 || nc >= 0x80))
	{
	    pos_T   sp = curwin->w_cursor;

	    curwin->w_cursor = *pos;
	    // don't use del_char(), it also removes composing chars
	    del_بائٹیں(utf_ptr2len(ml_get_cursor()), FALSE, FALSE);
	    ins_char(nc);
	    curwin->w_cursor = sp;
	}
	ورنہ
	    PBYTE(*pos, nc);
	واپس TRUE;
    }
    واپس FALSE;
}

/*
 * op_insert - Insert and append operators for Visual mode.
 */
    خالی
op_داخل_کرو(oparg_T *oap, طویل count1)
{
    طویل		pre_textlen = 0;
    colnr_T		ind_pre_ستون = 0, ind_post_ستون;
    صحیح			ind_pre_vcol = 0, ind_post_vcol = 0;
    ساخت بلاک_def	bd;
    صحیح			i;
    pos_T		t1;
    pos_T		آغاز_داخل_کرو;

    // edit() changes this - record it for OP_APPEND
    bd.is_MAX = (curwin->w_curswant == MAXCOL);

    // vis block is still marked. Get rid of it now.
    curwin->w_cursor.lnum = oap->آغاز.lnum;
    update_screen(UPD_INVERTED);

    اگر (oap->بلاک_منوال)
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
		واپس;

	    curwin->w_ve_flags = VE_ALL;
	    coladvance_force(oap->op_type == OP_APPEND
					   ? oap->انتہا_vcol + 1 : getviscol());
	    اگر (oap->op_type == OP_APPEND)
		--curwin->w_cursor.ستون;
	    curwin->w_ve_flags = old_ve_flags;
	}
	// Get the info about the block before entering the text
	بلاک_prep(oap, &bd, oap->آغاز.lnum, TRUE);
	// Get indent information
	ind_pre_ستون = (colnr_T)getwhitecols_curline();
	ind_pre_vcol = get_indent();
	pre_textlen = ml_get_len(oap->آغاز.lnum) - bd.textcol;
	اگر (oap->op_type == OP_APPEND)
	    pre_textlen -= bd.textlen;
    }

    اگر (oap->op_type == OP_APPEND)
    {
	اگر (oap->بلاک_منوال && curwin->w_cursor.coladd == 0)
	{
	    // Move the cursor to the character right of the block.
	    curwin->w_سیٹ_curswant = true;
	    جب (*ml_get_cursor() != NUL
		    && (curwin->w_cursor.ستون < bd.textcol + bd.textlen))
		++curwin->w_cursor.ستون;
	    اگر (bd.is_short && !bd.is_MAX)
	    {
		// First line was too short, make it longer and adjust the
		// values in "bd".
		اگر (u_save_cursor() == FAIL)
		    واپس;
		برائے (i = 0; i < bd.endspaces; ++i)
		    ins_char(' ');
		bd.textlen += bd.endspaces;
	    }
	}
	ورنہ
	{
	    curwin->w_cursor = oap->انتہا;
	    check_cursor_ستون();

	    // Works just like an 'i'nsert on the next character.
	    اگر (!LINEEMPTY(curwin->w_cursor.lnum)
		    && oap->آغاز_vcol != oap->انتہا_vcol)
		inc_cursor();
	}
    }

    t1 = oap->آغاز;
    آغاز_داخل_کرو = curwin->w_cursor;
    (خالی)edit(NUL, FALSE, (linenr_T)count1);

    // When a tab was inserted, and the characters in front of the tab
    // have been converted to a tab as well, the column of the cursor
    // might have actually been reduced, so need to adjust here.
    اگر (t1.lnum == curbuf->b_op_آغاز_orig.lnum
	    && LT_POS(curbuf->b_op_آغاز_orig, t1))
	oap->آغاز = curbuf->b_op_آغاز_orig;

    // If user has moved off this line, we don't know what to do, so do
    // nothing.
    // Also don't repeat the insert when Insert mode ended with CTRL-C.
    اگر (curwin->w_cursor.lnum != oap->آغاز.lnum || got_int)
	واپس;

    اگر (oap->بلاک_منوال)
    {
	صحیح			ins_len;
	char_u			*firstline, *ins_متن;
	ساخت بلاک_def	bd2;
	صحیح			did_indent = FALSE;
	size_t			len;
	size_t			جوڑو;
	// offset when cursor was moved in insert mode
	صحیح			انحراف = 0;

	// If indent kicked in, the firstline might have changed
	// but only do that, if the indent actually increased.
	ind_post_ستون = (colnr_T)getwhitecols_curline();
	اگر (curbuf->b_op_آغاز.ستون > ind_pre_ستون && ind_post_ستون > ind_pre_ستون)
	{
	    bd.textcol += ind_post_ستون - ind_pre_ستون;
	    ind_post_vcol = get_indent();
	    bd.آغاز_vcol += ind_post_vcol - ind_pre_vcol;
	    did_indent = TRUE;
	}

	// The user may have moved the cursor before inserting something, try
	// to adjust the block for that.  But only do it, if the difference
	// does not come from indent kicking in.
	اگر (oap->آغاز.lnum == curbuf->b_op_آغاز_orig.lnum
						  && !bd.is_MAX && !did_indent)
	{
	    صحیح t = getviscol2(curbuf->b_op_آغاز_orig.ستون,
					       curbuf->b_op_آغاز_orig.coladd);

	    اگر (oap->op_type == OP_داخل_کرو
		    && oap->آغاز.ستون + oap->آغاز.coladd
			    != curbuf->b_op_آغاز_orig.ستون
					      + curbuf->b_op_آغاز_orig.coladd)
	    {
		oap->آغاز.ستون = curbuf->b_op_آغاز_orig.ستون;
		pre_textlen -= t - oap->آغاز_vcol;
		oap->آغاز_vcol = t;
	    }
	    ورنہ اگر (oap->op_type == OP_APPEND
		    && oap->آغاز.ستون + oap->آغاز.coladd
			    >= curbuf->b_op_آغاز_orig.ستون
					      + curbuf->b_op_آغاز_orig.coladd)
	    {
		oap->آغاز.ستون = curbuf->b_op_آغاز_orig.ستون;
		// reset pre_textlen to the value of OP_INSERT
		pre_textlen += bd.textlen;
		pre_textlen -= t - oap->آغاز_vcol;
		oap->آغاز_vcol = t;
		oap->op_type = OP_داخل_کرو;
	    }
	}

	// Spaces and tabs in the indent may have changed to other spaces and
	// tabs.  Get the starting column again and correct the length.
	// Don't do this when "$" used, end-of-line will have changed.
	//
	// if indent was added and the inserted text was after the indent,
	// correct the selection for the new indent.
	اگر (did_indent && bd.textcol - ind_post_ستون > 0)
	{
	    oap->آغاز.ستون += ind_post_ستون - ind_pre_ستون;
	    oap->آغاز_vcol += ind_post_vcol - ind_pre_vcol;
	    oap->انتہا.ستون += ind_post_ستون - ind_pre_ستون;
	    oap->انتہا_vcol += ind_post_vcol - ind_pre_vcol;
	}
	بلاک_prep(oap, &bd2, oap->آغاز.lnum, TRUE);
	اگر (did_indent && bd.textcol - ind_post_ستون > 0)
	{
	    // undo for where "oap" is used below
	    oap->آغاز.ستون -= ind_post_ستون - ind_pre_ستون;
	    oap->آغاز_vcol -= ind_post_vcol - ind_pre_vcol;
	    oap->انتہا.ستون -= ind_post_ستون - ind_pre_ستون;
	    oap->انتہا_vcol -= ind_post_vcol - ind_pre_vcol;
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
	firstline = ml_get(oap->آغاز.lnum);
	len = ml_get_len(oap->آغاز.lnum);
	جوڑو = bd.textcol;
	اگر (oap->op_type == OP_APPEND)
	{
	    جوڑو += bd.textlen;
	    // account for pressing cursor in insert mode when '$' was used
	    اگر (bd.is_MAX
		&& (آغاز_داخل_کرو.lnum == Insstart.lnum
					   && آغاز_داخل_کرو.ستون > Insstart.ستون))
	    {
		انحراف = (آغاز_داخل_کرو.ستون - Insstart.ستون);
		جوڑو -= انحراف;
		اگر (oap->انتہا_vcol > انحراف)
		    oap->انتہا_vcol -= (انحراف + 1);
		ورنہ
		    // moved outside of the visual block, what to do?
		    واپس;
	    }
	}
	اگر (جوڑو > len)
	    جوڑو = len;  // short line, point to the NUL
	firstline += جوڑو;
	len -= جوڑو;
	اگر (pre_textlen >= 0 && (ins_len = (صحیح)len - pre_textlen - انحراف) > 0)
	{
	    ins_متن = vim_strnsave(firstline, ins_len);
	    اگر (ins_متن != تہی)
	    {
		// block handled here
		اگر (u_save(oap->آغاز.lnum,
					 (linenr_T)(oap->انتہا.lnum + 1)) == OK)
		    بلاک_داخل_کرو(oap, ins_متن, ins_len, (oap->op_type == OP_داخل_کرو),
									 &bd);

		curwin->w_cursor.ستون = oap->آغاز.ستون;
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
    صحیح
op_change(oparg_T *oap)
{
    colnr_T		l;
    صحیح			retval;
    linenr_T		linenr;
    طویل		pre_textlen = 0;
    طویل		pre_indent = 0;
    char_u		*firstline;
    char_u		*ins_متن, *newp, *oldp;
    ساخت بلاک_def	bd;

    l = oap->آغاز.ستون;
    اگر (oap->motion_type == MLINE)
    {
	l = 0;
	can_si = may_do_si();	// Like opening a new line, do smart indent
    }

    // First delete the text in the region.  In an empty buffer only need to
    // save for undo
    اگر (curbuf->b_ml.ml_flags & ML_خالی_خانے)
    {
	اگر (u_save_cursor() == FAIL)
	    واپس FALSE;
    }
    ورنہ اگر (op_delete(oap) == FAIL)
	واپس FALSE;

    اگر ((l > curwin->w_cursor.ستون) && !LINEEMPTY(curwin->w_cursor.lnum)
							 && !virtual_op)
	inc_cursor();

    // check for still on same line (<CR> in inserted text meaningless)
    // skip blank lines too
    اگر (oap->بلاک_منوال)
    {
	// Add spaces before getting the current line length.
	اگر (virtual_op && (curwin->w_cursor.coladd > 0
						    || gchar_cursor() == NUL))
	    coladvance_force(getviscol());
	firstline = ml_get(oap->آغاز.lnum);
	pre_textlen = ml_get_len(oap->آغاز.lnum);
	pre_indent = (طویل)getwhitecols(firstline);
	bd.textcol = curwin->w_cursor.ستون;
    }

    اگر (oap->motion_type == MLINE)
	fix_indent();

    // Reset finish_op now, don't want it set inside edit().
    صحیح save_اختتام_op = اختتام_op;
    اختتام_op = FALSE;

    retval = edit(NUL, FALSE, (linenr_T)1);

    اختتام_op = save_اختتام_op;

    /*
     * In Visual block mode, handle copying the new text to all lines of the
     * block.
     * Don't repeat the insert when Insert mode ended with CTRL-C.
     */
    اگر (oap->بلاک_منوال && oap->آغاز.lnum != oap->انتہا.lnum && !got_int)
    {
	صحیح	ins_len;

	// Auto-indenting may have changed the indent.  If the cursor was past
	// the indent, exclude that indent change from the inserted text.
	firstline = ml_get(oap->آغاز.lnum);
	اگر (bd.textcol > (colnr_T)pre_indent)
	{
	    طویل new_indent = (طویل)getwhitecols(firstline);

	    pre_textlen += new_indent - pre_indent;
	    bd.textcol += new_indent - pre_indent;
	}

	ins_len = (صحیح)ml_get_len(oap->آغاز.lnum) - pre_textlen;
	اگر (ins_len > 0)
	{
	    // Subsequent calls to ml_get() flush the firstline data - take a
	    // copy of the inserted text.
	    اگر ((ins_متن = alloc(ins_len + 1)) != تہی)
	    {
		vim_strncpy(ins_متن, firstline + bd.textcol, ins_len);
		برائے (linenr = oap->آغاز.lnum + 1; linenr <= oap->انتہا.lnum;
								     linenr++)
		{
		    بلاک_prep(oap, &bd, linenr, TRUE);
		    اگر (!bd.is_short || virtual_op)
		    {
			pos_T vpos;
			size_t newlen;

			// If the block starts in virtual space, count the
			// initial coladd offset as part of "startspaces"
			اگر (bd.is_short)
			{
			    vpos.lnum = linenr;
			    (خالی)getvpos(&vpos, oap->آغاز_vcol);
			}
			ورنہ
			    vpos.coladd = 0;
			oldp = ml_get(linenr);
			newp = alloc(ml_get_len(linenr) + vpos.coladd + ins_len + 1);
			اگر (newp == تہی)
			    جاری;
			// copy up to block start
			mch_memmove(newp, oldp, (size_t)bd.textcol);
			newlen = bd.textcol;
			vim_memset(newp + newlen, ' ', (size_t)vpos.coladd);
			newlen += vpos.coladd;
			mch_memmove(newp + newlen, ins_متن, ins_len);
			STRCPY(newp + newlen + ins_len, oldp + bd.textcol);
			ml_replace(linenr, newp, FALSE);
#اگر_تعریف_شدہ FEAT_PROP_POPUP
			// Shift the properties for linenr as edit() would do.
			اگر (curbuf->b_has_textprop)
			    adjust_prop_columns(linenr, bd.textcol,
						     vpos.coladd + (صحیح)ins_len, 0);
#اگر_ختم
		    }
		}
		check_cursor();

		بدلا_سطریں(oap->آغاز.lnum + 1, 0, oap->انتہا.lnum + 1, 0L);
	    }
	    vim_free(ins_متن);
	}
    }
    auto_format(FALSE, TRUE);

    واپس retval;
}

/*
 * When the cursor is on the NUL past the end of the line and it should not be
 * there move it left.
 */
    خالی
adjust_cursor_eol(خالی)
{
    غیرعلامتی صحیح cur_ve_flags = get_ve_flags();

    صحیح adj_cursor = (curwin->w_cursor.ستون > 0
				&& gchar_cursor() == NUL
				&& (cur_ve_flags & VE_ONEMORE) == 0
				&& (cur_ve_flags & VE_ALL) == 0
				&& !(restart_edit || (State & منوال_داخل_کرو)));
    اگر (!adj_cursor)
	واپس;

    // Put the cursor on the last character in the line.
    dec_cursor();

    اگر (cur_ve_flags == VE_ALL)
    {
	colnr_T	    scol, ecol;

	// Coladd is set to the width of the last character.
	getvcol(curwin, &curwin->w_cursor, &scol, تہی, &ecol, 0);
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
    char_u *comment_flags = تہی;
    صحیح    lead_len;
    صحیح    leader_انحراف = get_آخری_leader_انحراف(سطر, &comment_flags);

    *is_comment = FALSE;
    اگر (leader_انحراف != -1)
    {
	// Let's check whether the line ends with an unclosed comment.
	// If the last comment leader has COM_END in flags, there's no comment.
	جب (*comment_flags)
	{
	    اگر (*comment_flags == COM_انتہا
		    || *comment_flags == ':')
		توڑو;
	    ++comment_flags;
	}
	اگر (*comment_flags != COM_انتہا)
	    *is_comment = TRUE;
    }

    اگر (process == FALSE)
	واپس سطر;

    lead_len = get_leader_len(سطر, &comment_flags, FALSE, include_space);

    اگر (lead_len == 0)
	واپس سطر;

    // Find:
    // - COM_END,
    // - colon,
    // whichever comes first.
    جب (*comment_flags)
    {
	اگر (*comment_flags == COM_انتہا
		|| *comment_flags == ':')
	    توڑو;
	++comment_flags;
    }

    // If we found a colon, it means that we are not processing a line
    // starting with a closing part of a three-part comment. That's good,
    // because we don't want to remove those as this would be annoying.
    اگر (*comment_flags == ':' || *comment_flags == NUL)
	سطر += lead_len;

    واپس سطر;
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
    طویل    گنتی,
    صحیح	    داخل_کرو_space,
    صحیح	    save_undo,
    صحیح	    use_formatoptions غیر_مستعمل,
    صحیح	    setmark)
{
    char_u	*curr = تہی;
    char_u      *curr_آغاز = تہی;
    char_u	*cend;
    char_u	*newp;
    size_t	newp_len;
    char_u	*spaces;	// number of spaces inserted before a line
    صحیح		endcurr1 = NUL;
    صحیح		endcurr2 = NUL;
    صحیح		currsize = 0;	// size of the current line
    صحیح		sumsize = 0;	// size of the long new line
    linenr_T	t;
    colnr_T	ستون = 0;
    صحیح		ret = OK;
    صحیح		*comments = تہی;
    صحیح		remove_comments = (use_formatoptions == TRUE)
				  && has_format_option(FO_REMOVE_COMS);
    صحیح		پچھلا_was_comment;

    اگر (save_undo && u_save((linenr_T)(curwin->w_cursor.lnum - 1),
			    (linenr_T)(curwin->w_cursor.lnum + گنتی)) == FAIL)
	واپس FAIL;

    // Allocate an array to store the number of spaces inserted before each
    // line.  We will use it to pre-compute the length of the new line and the
    // proper placement of each original line in the new one.
    spaces = lalloc_clear(گنتی, TRUE);
    اگر (spaces == تہی)
	واپس FAIL;
    اگر (remove_comments)
    {
	comments = lalloc_clear(گنتی * حجم(صحیح), TRUE);
	اگر (comments == تہی)
	{
	    vim_free(spaces);
	    واپس FAIL;
	}
    }

    /*
     * Don't move anything yet, just compute the final line length
     * and setup the array of space strings lengths
     * This loops forward over the joined lines.
     */
    برائے (t = 0; t < گنتی; ++t)
    {
	curr = curr_آغاز = ml_get((linenr_T)(curwin->w_cursor.lnum + t));
	اگر (t == 0 && setmark && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set the '[ mark.
	    curwin->w_بفر->b_op_آغاز.lnum = curwin->w_cursor.lnum;
	    curwin->w_بفر->b_op_آغاز.ستون  = (colnr_T)STRLEN(curr);
	}
	اگر (remove_comments)
	{
	    // We don't want to remove the comment leader if the
	    // previous line is not a comment.
	    اگر (t > 0 && پچھلا_was_comment)
	    {

		char_u *new_curr = skip_comment(curr, TRUE, داخل_کرو_space,
							   &پچھلا_was_comment);
		comments[t] = (صحیح)(new_curr - curr);
		curr = new_curr;
	    }
	    ورنہ
		curr = skip_comment(curr, FALSE, داخل_کرو_space,
							   &پچھلا_was_comment);
	}

	اگر (داخل_کرو_space && t > 0)
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
		ورنہ
		    ++spaces[t];
		// extra space when 'joinspaces' set and line ends in '.'
		اگر (       p_js
			&& (endcurr1 == '.'
			    || (vim_strchr(p_cpo, CPO_JOINSP) == تہی
				&& (endcurr1 == '?' || endcurr1 == '!'))))
		    ++spaces[t];
	    }
	}
	currsize = (صحیح)STRLEN(curr);
	sumsize += currsize + spaces[t];
	endcurr1 = endcurr2 = NUL;
	اگر (داخل_کرو_space && currsize > 0)
	{
	    اگر (has_mbyte)
	    {
		cend = curr + currsize;
		MB_PTR_عقب(curr, cend);
		endcurr1 = (*mb_ptr2char)(cend);
		اگر (cend > curr)
		{
		    MB_PTR_عقب(curr, cend);
		    endcurr2 = (*mb_ptr2char)(cend);
		}
	    }
	    ورنہ
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
	    جاؤ theend;
	}
    }

    // store the column position before last line
    ستون = sumsize - currsize - spaces[گنتی - 1];

    // allocate the space for the new line
    newp_len = sumsize + 1;
    newp = alloc(newp_len);
    اگر (newp == تہی)
    {
	ret = FAIL;
	جاؤ theend;
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
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    unpacked_memline_T um = um_open_at_no_props(
				curwin->w_بفر, curwin->w_cursor.lnum, 0);
    // um_open_at_no_props may have invalidated "curr".
    صحیح curr_off = (صحیح)(curr - curr_آغاز);

    curr = curr_آغاز = ml_get((linenr_T)(curwin->w_cursor.lnum + گنتی - 1));
    curr += curr_off;
#اگر_ختم

    برائے (t = گنتی - 1; ; --t)
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
	spaces_removed = (curr - curr_آغاز) - spaces[t];

	mark_ستون_adjust(curwin->w_cursor.lnum + t, (colnr_T)0, -t,
			 (طویل)(cend - newp - spaces_removed), spaces_removed);
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	prepend_joined_props(&um, curwin->w_cursor.lnum + t,
		t == گنتی - 1, (طویل)(cend - newp), spaces_removed);
#اگر_ختم
	اگر (t == 0)
	    توڑو;
	curr = curr_آغاز = ml_get((linenr_T)(curwin->w_cursor.lnum + t - 1));
	اگر (remove_comments)
	    curr += comments[t - 1];
	اگر (داخل_کرو_space && t > 1)
	    curr = skipwhite(curr);
	currsize = (صحیح)STRLEN(curr);
    }

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    اگر (um.buf != تہی)
    {
	um_سیٹ_متن(&um, newp);
	um_الٹ_props(&um);
	um_close(&um);
	newp = تہی;  // um_set_text took ownership
    }
    ورنہ
#اگر_ختم
    ml_replace_len(curwin->w_cursor.lnum, newp, (colnr_T)newp_len, TRUE, FALSE);

    اگر (setmark && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set the '] mark.
	curwin->w_بفر->b_op_انتہا.lnum = curwin->w_cursor.lnum;
	curwin->w_بفر->b_op_انتہا.ستون  = (colnr_T)sumsize;
    }

    // Only report the change in the first line here, del_lines() will report
    // the deleted line.
    بدلا_سطریں(curwin->w_cursor.lnum, currsize,
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
    curwin->w_cursor.ستون =
		    (vim_strchr(p_cpo, CPO_JOINCOL) != تہی ? currsize : ستون);
    check_cursor_ستون();

    curwin->w_cursor.coladd = 0;
    curwin->w_سیٹ_curswant = true;

theend:
    vim_free(spaces);
    اگر (remove_comments)
	vim_free(comments);
    واپس ret;
}

#اگر_تعریف_شدہ FEAT_LINEBREAK
/*
 * TODO: consider using "tailp" of win_lbr_chartabsize() instead of temporarily
 * resetting 'linebreak'.
 *
 * Reset 'linebreak' and take care of side effects.
 * Returns the previous value, to be passed to restore_lbr().
 */
    صحیح
reset_lbr(خالی)
{
    اگر (!curwin->w_p_lbr)
	واپس FALSE;
    // changing 'linebreak' may require w_virtcol to be updated
    curwin->w_p_lbr = FALSE;
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);
    واپس TRUE;
}

/*
 * Restore 'linebreak' and take care of side effects.
 */
    خالی
restore_lbr(صحیح lbr_saved)
{
    اگر (curwin->w_p_lbr || !lbr_saved)
	واپس;

    // changing 'linebreak' may require w_virtcol to be updated
    curwin->w_p_lbr = TRUE;
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);
}
#اگر_ختم

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
    خالی
بلاک_prep(
    oparg_T		*oap,
    ساخت بلاک_def	*bdp,
    linenr_T		lnum,
    صحیح			is_del)
{
    صحیح		incr = 0;
    char_u	*pend;
    char_u	*pstart;
    char_u	*سطر;
    char_u	*پچھلا_pstart;
    char_u	*پچھلا_pend;
    chartabsize_T cts;
#اگر_تعریف_شدہ FEAT_LINEBREAK
		// Avoid a problem with unwanted linebreaks in block mode.
    صحیح		lbr_saved = reset_lbr();
#اگر_ختم

    bdp->startspaces = 0;
    bdp->endspaces = 0;
    bdp->textlen = 0;
    bdp->آغاز_vcol = 0;
    bdp->انتہا_vcol = 0;
    bdp->is_short = FALSE;
    bdp->is_oneChar = FALSE;
    bdp->pre_whitesp = 0;
    bdp->pre_whitesp_c = 0;
    bdp->انتہا_char_vcols = 0;
    bdp->آغاز_char_vcols = 0;

    سطر = ml_get(lnum);
    پچھلا_pstart = سطر;
    init_chartabsize_arg(&cts, curwin, lnum, bdp->آغاز_vcol, سطر, سطر);
    جب (cts.cts_vcol < oap->آغاز_vcol && *cts.cts_ptr != NUL)
    {
	// Count a tab for what it's worth (if list mode not on)
	incr = lbr_chartabsize(&cts);
	cts.cts_vcol += incr;
	اگر (VIM_ISWHITE(*cts.cts_ptr))
	{
	    bdp->pre_whitesp += incr;
	    bdp->pre_whitesp_c++;
	}
	ورنہ
	{
	    bdp->pre_whitesp = 0;
	    bdp->pre_whitesp_c = 0;
	}
	پچھلا_pstart = cts.cts_ptr;
	MB_PTR_ADV(cts.cts_ptr);
    }
    bdp->آغاز_vcol = cts.cts_vcol;
    pstart = cts.cts_ptr;
    clear_chartabsize_arg(&cts);

    bdp->آغاز_char_vcols = incr;
    اگر (bdp->آغاز_vcol < oap->آغاز_vcol)	// line too short
    {
	bdp->انتہا_vcol = bdp->آغاز_vcol;
	bdp->is_short = TRUE;
	اگر (!is_del || oap->op_type == OP_APPEND)
	    bdp->endspaces = oap->انتہا_vcol - oap->آغاز_vcol + 1;
    }
    ورنہ
    {
	// notice: this converts partly selected Multibyte characters to
	// spaces, too.
	bdp->startspaces = bdp->آغاز_vcol - oap->آغاز_vcol;
	اگر (is_del && bdp->startspaces)
	    bdp->startspaces = bdp->آغاز_char_vcols - bdp->startspaces;
	pend = pstart;
	bdp->انتہا_vcol = bdp->آغاز_vcol;
	اگر (bdp->انتہا_vcol > oap->انتہا_vcol)	// it's all in one character
	{
	    bdp->is_oneChar = TRUE;
	    اگر (oap->op_type == OP_داخل_کرو)
		bdp->endspaces = bdp->آغاز_char_vcols - bdp->startspaces;
	    ورنہ اگر (oap->op_type == OP_APPEND)
	    {
		bdp->startspaces += oap->انتہا_vcol - oap->آغاز_vcol + 1;
		bdp->endspaces = bdp->آغاز_char_vcols - bdp->startspaces;
	    }
	    ورنہ
	    {
		bdp->startspaces = oap->انتہا_vcol - oap->آغاز_vcol + 1;
		اگر (is_del && oap->op_type != OP_LSHIFT)
		{
		    // just putting the sum of those two into
		    // bdp->startspaces doesn't work for Visual replace,
		    // so we have to split the tab in two
		    bdp->startspaces = bdp->آغاز_char_vcols
					- (bdp->آغاز_vcol - oap->آغاز_vcol);
		    bdp->endspaces = bdp->انتہا_vcol - oap->انتہا_vcol - 1;
		}
	    }
	}
	ورنہ
	{
	    init_chartabsize_arg(&cts, curwin, lnum, bdp->انتہا_vcol,
								  سطر, pend);
	    پچھلا_pend = pend;
	    جب (cts.cts_vcol <= oap->انتہا_vcol && *cts.cts_ptr != NUL)
	    {
		// count a tab for what it's worth (if list mode not on)
		پچھلا_pend = cts.cts_ptr;
		incr = lbr_chartabsize_adv(&cts);
		cts.cts_vcol += incr;
	    }
	    bdp->انتہا_vcol = cts.cts_vcol;
	    pend = cts.cts_ptr;
	    clear_chartabsize_arg(&cts);

	    اگر (bdp->انتہا_vcol <= oap->انتہا_vcol
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
		    bdp->endspaces = oap->انتہا_vcol - bdp->انتہا_vcol
							     + oap->inclusive;
		ورنہ
		    bdp->endspaces = 0; // replace doesn't add characters
	    }
	    ورنہ اگر (bdp->انتہا_vcol > oap->انتہا_vcol)
	    {
		bdp->endspaces = bdp->انتہا_vcol - oap->انتہا_vcol - 1;
		اگر (!is_del && bdp->endspaces)
		{
		    bdp->endspaces = incr - bdp->endspaces;
		    اگر (pend != pstart)
			pend = پچھلا_pend;
		}
	    }
	}
	bdp->انتہا_char_vcols = incr;
	اگر (is_del && bdp->startspaces)
	    pstart = پچھلا_pstart;
	bdp->textlen = (صحیح)(pend - pstart);
    }
    bdp->textcol = (colnr_T) (pstart - سطر);
    bdp->textstart = pstart;
#اگر_تعریف_شدہ FEAT_LINEBREAK
    restore_lbr(lbr_saved);
#اگر_ختم
}

/*
 * Get block text from "start" to "end"
 */
    خالی
charwise_بلاک_prep(
    pos_T		آغاز,
    pos_T		انتہا,
    ساخت بلاک_def	*bdp,
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
    bdp->آغاز_char_vcols = 0;

    اگر (lnum == آغاز.lnum)
    {
	startcol = آغاز.ستون;
	اگر (virtual_op)
	{
	    getvcol(curwin, &آغاز, &cs, تہی, &ce, 0);
	    اگر (ce != cs && آغاز.coladd > 0)
	    {
		// Part of a tab selected -- but don't
		// double-count it.
		bdp->آغاز_char_vcols = ce - cs + 1;
		bdp->startspaces = bdp->آغاز_char_vcols - آغاز.coladd;
		اگر (bdp->startspaces < 0)
		    bdp->startspaces = 0;
		startcol++;
	    }
	}
    }

    اگر (lnum == انتہا.lnum)
    {
	endcol = انتہا.ستون;
	اگر (virtual_op)
	{
	    getvcol(curwin, &انتہا, &cs, تہی, &ce, 0);
	    اگر (p[endcol] == NUL || (cs + انتہا.coladd < ce
			// Don't add space for double-wide
			// char; endcol will be on last byte
			// of multi-byte char.
			&& (*mb_سر_off)(p, p + endcol) == 0))
	    {
		اگر (آغاز.lnum == انتہا.lnum && آغاز.ستون == انتہا.ستون)
		{
		    // Special case: inside a single char
		    bdp->is_oneChar = TRUE;
		    bdp->startspaces = انتہا.coladd - آغاز.coladd + inclusive;
		    endcol = startcol;
		}
		ورنہ
		{
		    bdp->endspaces = انتہا.coladd + inclusive;
		    endcol -= inclusive;
		}
	    }
	}
    }
    اگر (endcol == MAXCOL)
	endcol = ml_get_len(lnum);
    اگر (startcol > endcol || bdp->is_oneChar)
	bdp->textlen = 0;
    ورنہ
	bdp->textlen = endcol - startcol + inclusive;
    bdp->textcol = startcol;
    bdp->textstart = startcol <= plen ? p + startcol : p;
}

/*
 * Handle the add/subtract operator.
 */
    خالی
op_addsub(
    oparg_T	*oap,
    linenr_T	Prenum1,	    // Amount of add/subtract
    صحیح		g_cmd)		    // was g<c-a>/g<c-x>
{
    pos_T		pos;
    ساخت بلاک_def	bd;
    صحیح			change_cnt = 0;
    linenr_T		amount = Prenum1;

    // do_addsub() might trigger re-evaluation of 'foldexpr' halfway, when the
    // buffer is not completely updated yet. Postpone updating folds until before
    // the call to changed_lines().
#اگر_تعریف_شدہ FEAT_FOLDING
    disable_fold_update++;
#اگر_ختم

    اگر (!VIsual_فعال)
    {
	pos = curwin->w_cursor;
	اگر (u_save_cursor() == FAIL)
	{
#اگر_تعریف_شدہ FEAT_FOLDING
	    disable_fold_update--;
#اگر_ختم
	    واپس;
	}
	change_cnt = do_addsub(oap->op_type, &pos, 0, amount);
#اگر_تعریف_شدہ FEAT_FOLDING
	disable_fold_update--;
#اگر_ختم
	اگر (change_cnt)
	    بدلا_سطریں(pos.lnum, 0, pos.lnum + 1, 0L);
    }
    ورنہ
    {
	صحیح	one_change;
	صحیح	لمبائی;
	pos_T	startpos;

	اگر (u_save((linenr_T)(oap->آغاز.lnum - 1),
					(linenr_T)(oap->انتہا.lnum + 1)) == FAIL)
	{
#اگر_تعریف_شدہ FEAT_FOLDING
	    disable_fold_update--;
#اگر_ختم
	    واپس;
	}

	pos = oap->آغاز;
	برائے (; pos.lnum <= oap->انتہا.lnum; ++pos.lnum)
	{
	    اگر (oap->بلاک_منوال)		    // Visual block mode
	    {
		بلاک_prep(oap, &bd, pos.lnum, FALSE);
		pos.ستون = bd.textcol;
		لمبائی = bd.textlen;
	    }
	    ورنہ اگر (oap->motion_type == MLINE)
	    {
		curwin->w_cursor.ستون = 0;
		pos.ستون = 0;
		لمبائی = ml_get_len(pos.lnum);
	    }
	    ورنہ // oap->motion_type == MCHAR
	    {
		اگر (pos.lnum == oap->آغاز.lnum && !oap->inclusive)
		    dec(&(oap->انتہا));
		لمبائی = ml_get_len(pos.lnum);
		pos.ستون = 0;
		اگر (pos.lnum == oap->آغاز.lnum)
		{
		    pos.ستون += oap->آغاز.ستون;
		    لمبائی -= oap->آغاز.ستون;
		}
		اگر (pos.lnum == oap->انتہا.lnum)
		{
		    لمبائی = ml_get_len(oap->انتہا.lnum);
		    اگر (oap->انتہا.ستون >= لمبائی)
			oap->انتہا.ستون = لمبائی - 1;
		    لمبائی = oap->انتہا.ستون - pos.ستون + 1;
		}
	    }
	    one_change = do_addsub(oap->op_type, &pos, لمبائی, amount);
	    اگر (one_change)
	    {
		// Remember the start position of the first change.
		اگر (change_cnt == 0)
		    startpos = curbuf->b_op_آغاز;
		++change_cnt;
	    }

#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
	    اگر (netbeans_فعال() && one_change)
	    {
		char_u *ptr;

		netbeans_removed(curbuf, pos.lnum, pos.ستون, (طویل)لمبائی);
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, pos.ستون,
						&ptr[pos.ستون], لمبائی);
	    }
#اگر_ختم
	    اگر (g_cmd && one_change)
		amount += Prenum1;
	}

#اگر_تعریف_شدہ FEAT_FOLDING
	disable_fold_update--;
#اگر_ختم
	اگر (change_cnt)
	    بدلا_سطریں(oap->آغاز.lnum, 0, oap->انتہا.lnum + 1, 0L);

	اگر (!change_cnt && oap->is_VIsual)
	    // No change: need to remove the Visual selection
	    redraw_curbuf_later(UPD_INVERTED);

	// Set '[ mark if something changed. Keep the last end
	// position from do_addsub().
	اگر (change_cnt > 0 && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	    curbuf->b_op_آغاز = startpos;

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
    ساکن صحیح
do_addsub(
    صحیح		op_type,
    pos_T	*pos,
    صحیح		لمبائی,
    linenr_T	Prenum1)
{
    صحیح		ستون;
    صحیح		pre;		// 'X'/'x': hex; '0': octal; 'B'/'b': bin
    ساکن صحیح	hexupper = FALSE;	// 0xABC
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
    صحیح		تفریق;
    صحیح		negative = FALSE;
    صحیح		was_مثبت = TRUE;
    صحیح		visual = VIsual_فعال;
    صحیح		did_change = FALSE;
    pos_T	save_cursor = curwin->w_cursor;
    صحیح		maxlen = 0;
    pos_T	startpos;
    pos_T	endpos;
    colnr_T	save_coladd = 0;

    do_hex = (vim_strchr(curbuf->b_p_nf, 'x') != تہی);	// "heX"
    do_oct = (vim_strchr(curbuf->b_p_nf, 'o') != تہی);	// "Octal"
    do_bin = (vim_strchr(curbuf->b_p_nf, 'b') != تہی);	// "Bin"
    do_alpha = (vim_strchr(curbuf->b_p_nf, 'p') != تہی);	// "alPha"
    do_unsigned = (vim_strchr(curbuf->b_p_nf, 'u') != تہی);	// "Unsigned"
    do_blank = (vim_strchr(curbuf->b_p_nf, 'k') != تہی);	// "blanK"

    اگر (virtual_فعال())
    {
	save_coladd = pos->coladd;
	pos->coladd = 0;
    }

    curwin->w_cursor = *pos;
    ptr = ml_get(pos->lnum);
    linelen = ml_get_len(pos->lnum);
    ستون = pos->ستون;

    اگر (ستون + !!save_coladd >= linelen)
	جاؤ theend;

    /*
     * First check if we are on a hexadecimal number, after the "0x".
     */
    اگر (!VIsual_فعال)
    {
	اگر (do_bin)
	    جب (ستون > 0 && vim_isbdigit(ptr[ستون]))
	    {
		--ستون;
		اگر (has_mbyte)
		    ستون -= (*mb_سر_off)(ptr, ptr + ستون);
	    }

	اگر (do_hex)
	    جب (ستون > 0 && vim_isxdigit(ptr[ستون]))
	    {
		--ستون;
		اگر (has_mbyte)
		    ستون -= (*mb_سر_off)(ptr, ptr + ستون);
	    }

	اگر (       do_bin
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

	    جب (ستون > 0 && vim_isdigit(ptr[ستون]))
	    {
		ستون--;
		اگر (has_mbyte)
		    ستون -= (*mb_سر_off)(ptr, ptr + ستون);
	    }
	}

	اگر ((       do_hex
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
	    اگر (has_mbyte)
		ستون -= (*mb_سر_off)(ptr, ptr + ستون);
	}
	ورنہ
	{
	    /*
	     * Search forward and then backward to find the start of number.
	     */
	    ستون = pos->ستون;

	    جب (ptr[ستون] != NUL
		    && !vim_isdigit(ptr[ستون])
		    && !(do_alpha && ASCII_ISALPHA(ptr[ستون])))
		ستون += mb_ptr2len(ptr + ستون);

	    جب (ستون > 0
		    && vim_isdigit(ptr[ستون - 1])
		    && !(do_alpha && ASCII_ISALPHA(ptr[ستون])))
	    {
		--ستون;
		اگر (has_mbyte)
		    ستون -= (*mb_سر_off)(ptr, ptr + ستون);
	    }
	}
    }

    اگر (visual)
    {
	جب (ptr[ستون] != NUL && لمبائی > 0
		&& !vim_isdigit(ptr[ستون])
		&& !(do_alpha && ASCII_ISALPHA(ptr[ستون])))
	{
	    صحیح mb_len = mb_ptr2len(ptr + ستون);

	    ستون += mb_len;
	    لمبائی -= mb_len;
	}

	اگر (لمبائی == 0)
	    جاؤ theend;

	اگر (ستون > pos->ستون && ptr[ستون - 1] == '-'
		&& (!has_mbyte || !(*mb_سر_off)(ptr, ptr + ستون - 1))
		&& !do_unsigned)
	{
	    اگر (do_blank && ستون >= 2 && !VIM_ISWHITE(ptr[ستون - 2]))
		blank_unsigned = TRUE;
	    ورنہ
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
    اگر (!VIM_ISDIGIT(firstdigit) && !(do_alpha && ASCII_ISALPHA(firstdigit)))
    {
	beep_flush();
	جاؤ theend;
    }

    اگر (do_alpha && ASCII_ISALPHA(firstdigit))
    {
	// decrement or increment alphabetic character
	اگر (op_type == OP_NR_SUB)
	{
	    اگر (CharOrd(firstdigit) < Prenum1)
	    {
		اگر (محفوظ_isupper(firstdigit))
		    firstdigit = 'A';
		ورنہ
		    firstdigit = 'a';
	    }
	    ورنہ
		firstdigit -= Prenum1;
	}
	ورنہ
	{
	    اگر (26 - CharOrd(firstdigit) - 1 < Prenum1)
	    {
		اگر (محفوظ_isupper(firstdigit))
		    firstdigit = 'Z';
		ورنہ
		    firstdigit = 'z';
	    }
	    ورنہ
		firstdigit += Prenum1;
	}
	curwin->w_cursor.ستون = ستون;
	اگر (!did_change)
	    startpos = curwin->w_cursor;
	did_change = TRUE;
	(خالی)del_char(FALSE);
	ins_char(firstdigit);
	endpos = curwin->w_cursor;
	curwin->w_cursor.ستون = ستون;
    }
    ورنہ
    {
	char_u	*buf1;
	صحیح	buf1len;
	char_u	buf2[NUMBUFLEN];
	صحیح	buf2len;
	pos_T	save_pos;
	صحیح	i;

	اگر (ستون > 0 && ptr[ستون - 1] == '-'
		&& (!has_mbyte ||
		    !(*mb_سر_off)(ptr, ptr + ستون - 1))
		&& !visual
		&& !do_unsigned)
	{
	    اگر (do_blank && ستون >= 2 && !VIM_ISWHITE(ptr[ستون - 2]))
		blank_unsigned = TRUE;
	    ورنہ
	    {
		// negative number
		--ستون;
		negative = TRUE;
	    }
	}

	// get the number value (unsigned)
	اگر (visual && VIsual_منوال != 'V')
	    maxlen = (curbuf->b_visual.vi_curswant == MAXCOL
		    ? linelen - ستون : لمبائی);

	صحیح حد_سے_تجاوز = FALSE;
	vim_str2nr(ptr + ستون, &pre, &لمبائی,
		0 + (do_bin ? STR2NR_BIN : 0)
		    + (do_oct ? STR2NR_OCT : 0)
		    + (do_hex ? STR2NR_HEX : 0),
		تہی, &n, maxlen, FALSE, &حد_سے_تجاوز);

	// ignore leading '-' for hex and octal and bin numbers
	اگر (pre && negative)
	{
	    ++ستون;
	    --لمبائی;
	    negative = FALSE;
	}
	// add or subtract
	تفریق = FALSE;
	اگر (op_type == OP_NR_SUB)
	    تفریق ^= TRUE;
	اگر (negative)
	    تفریق ^= TRUE;

	oldn = n;
	اگر (!حد_سے_تجاوز)  // if number is too big don't add/subtract
	{
	    اگر (تفریق)
		n -= (uvarnumber_T)Prenum1;
	    ورنہ
		n += (uvarnumber_T)Prenum1;
	}

	// handle wraparound for decimal numbers
	اگر (!pre)
	{
	    اگر (تفریق)
	    {
		اگر (n > oldn)
		{
		    n = 1 + (n ^ (uvarnumber_T)-1);
		    negative ^= TRUE;
		}
	    }
	    ورنہ
	    {
		//جوڑو
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
	    اگر (تفریق)
		// sticking at zero.
		n = (uvarnumber_T)0;
	    ورنہ
		// sticking at 2^64 - 1.
		n = (uvarnumber_T)(-1);
	    negative = FALSE;
	}

	اگر (visual && !was_مثبت && !negative && ستون > 0)
	{
	    // need to remove the '-'
	    ستون--;
	    لمبائی++;
	}

	/*
	 * Delete the old number.
	 */
	curwin->w_cursor.ستون = ستون;
	اگر (!did_change)
	    startpos = curwin->w_cursor;
	did_change = TRUE;
	todel = لمبائی;
	c = gchar_cursor();
	/*
	 * Don't include the '-' in the length, only the length of the
	 * part after it is kept the same.
	 */
	اگر (c == '-')
	    --لمبائی;

	save_pos = curwin->w_cursor;
	برائے (i = 0; i < todel; ++i)
	{
	    اگر (c < 0x100 && محفوظ_isalpha(c))
	    {
		اگر (محفوظ_isupper(c))
		    hexupper = TRUE;
		ورنہ
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
	buf1 = alloc(لمبائی + NUMBUFLEN);
	اگر (buf1 == تہی)
	    جاؤ theend;
	ptr = buf1;
	اگر (negative && (!visual || was_مثبت))
	    *ptr++ = '-';
	اگر (pre)
	{
	    *ptr++ = '0';
	    --لمبائی;
	}
	اگر (pre == 'b' || pre == 'B' ||
	    pre == 'x' || pre == 'X')
	{
	    *ptr++ = pre;
	    --لمبائی;
	}

	/*
	 * Put the number characters in buf2[].
	 */
	اگر (pre == 'b' || pre == 'B')
	{
	    صحیح ثنائی_ہندسہ = 0;
	    صحیح بٹ = حجم(uvarnumber_T) * 8;

	    // leading zeros
	    برائے (ثنائی_ہندسہ = بٹ; ثنائی_ہندسہ > 0; ثنائی_ہندسہ--)
		اگر ((n >> (ثنائی_ہندسہ - 1)) & 0x1) توڑو;

	    برائے (buf2len = 0; ثنائی_ہندسہ > 0 && buf2len < (NUMBUFLEN - 1); ثنائی_ہندسہ--)
		buf2[buf2len++] = ((n >> (ثنائی_ہندسہ - 1)) & 0x1) ? '1' : '0';

	    buf2[buf2len] = NUL;
	}
	ورنہ اگر (pre == 0)
	    buf2len = vim_snprintf((حرف *)buf2, NUMBUFLEN, "%llu", n);
	ورنہ اگر (pre == '0')
	    buf2len = vim_snprintf((حرف *)buf2, NUMBUFLEN, "%llo", n);
	ورنہ اگر (pre && hexupper)
	    buf2len = vim_snprintf((حرف *)buf2, NUMBUFLEN, "%llX", n);
	ورنہ
	    buf2len = vim_snprintf((حرف *)buf2, NUMBUFLEN, "%llx", n);
	لمبائی -= buf2len;

	/*
	 * Adjust number of zeros to the new number of digits, so the
	 * total length of the number remains the same.
	 * Don't do this when
	 * the result may look like an octal number.
	 */
	اگر (firstdigit == '0' && !(do_oct && pre == 0))
	    جب (لمبائی-- > 0)
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
	    صحیح بائٹیں_after = ml_get_curline_len() - curwin->w_cursor.ستون;

	    // Delete the one character before the insert.
	    curwin->w_cursor = save_pos;
	    (خالی)del_char(FALSE);
	    curwin->w_cursor.ستون = ml_get_curline_len() - بائٹیں_after;
	    --todel;
	}
	جب (todel-- > 0)
	    (خالی)del_char(FALSE);

	endpos = curwin->w_cursor;
	اگر (did_change && curwin->w_cursor.ستون)
	    --curwin->w_cursor.ستون;
    }

    اگر (did_change && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// set the '[ and '] marks
	curbuf->b_op_آغاز = startpos;
	curbuf->b_op_انتہا = endpos;
	اگر (curbuf->b_op_انتہا.ستون > 0)
	    --curbuf->b_op_انتہا.ستون;
    }

theend:
    اگر (visual)
	curwin->w_cursor = save_cursor;
    ورنہ اگر (did_change)
	curwin->w_سیٹ_curswant = true;
    ورنہ اگر (virtual_فعال())
	curwin->w_cursor.coladd = save_coladd;

    واپس did_change;
}

    خالی
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
    ساکن varnumber_T
سطر_گنتی_info(
    char_u	*سطر,
    varnumber_T	*wc,
    varnumber_T	*cc,
    varnumber_T	حد_بندی,
    صحیح		eol_سائز)
{
    varnumber_T	i;
    varnumber_T	الفاظ = 0;
    varnumber_T	chars = 0;
    صحیح		is_لفظ = 0;

    برائے (i = 0; i < حد_بندی && سطر[i] != NUL; )
    {
	اگر (is_لفظ)
	{
	    اگر (vim_isspace(سطر[i]))
	    {
		الفاظ++;
		is_لفظ = 0;
	    }
	}
	ورنہ اگر (!vim_isspace(سطر[i]))
	    is_لفظ = 1;
	++chars;
	i += (*mb_ptr2len)(سطر + i);
    }

    اگر (is_لفظ)
	الفاظ++;
    *wc += الفاظ;

    // Add eol_size if the end of line was reached before hitting limit.
    اگر (i < حد_بندی && سطر[i] == NUL)
    {
	i += eol_سائز;
	chars += eol_سائز;
    }
    *cc += chars;
    واپس i;
}

/*
 * Give some info about the position of the cursor (for "g CTRL-G").
 * In Visual mode, give some info about the selected region.  (In this case,
 * the *_count_cursor variables store running totals for the selection.)
 * When "dict" is not NULL store the info there instead of showing it.
 */
    خالی
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
    صحیح		eol_سائز;
    varnumber_T	آخری_check = 100000L;
    طویل	سطر_گنتی_selected = 0;
    pos_T	min_pos, max_pos;
    oparg_T	oparg;
    ساخت بلاک_def	bd;

    /*
     * Compute the length of the file in characters.
     */
    اگر (curbuf->b_ml.ml_flags & ML_خالی_خانے)
    {
	اگر (dict == تہی)
	{
	    پیغام(_(no_سطریں_پیغام));
	    واپس;
	}
    }
    ورنہ
    {
	اگر (get_fileformat(curbuf) == EOL_DOS)
	    eol_سائز = 2;
	ورنہ
	    eol_سائز = 1;

	اگر (VIsual_فعال)
	{
	    اگر (LT_POS(VIsual, curwin->w_cursor))
	    {
		min_pos = VIsual;
		max_pos = curwin->w_cursor;
	    }
	    ورنہ
	    {
		min_pos = curwin->w_cursor;
		max_pos = VIsual;
	    }
	    اگر (*p_sel == 'e' && max_pos.ستون > 0)
		--max_pos.ستون;

	    اگر (VIsual_منوال == Ctrl_V)
	    {
#اگر_تعریف_شدہ FEAT_LINEBREAK
		char_u * saved_sbr = p_sbr;
		char_u * saved_w_sbr = curwin->w_p_sbr;

		// Make 'sbr' empty for a moment to get the correct size.
		p_sbr = خالی_خانے_option;
		curwin->w_p_sbr = خالی_خانے_option;
#اگر_ختم
		oparg.is_VIsual = 1;
		oparg.بلاک_منوال = TRUE;
		oparg.op_type = OP_NOP;
		getvcols(curwin, &min_pos, &max_pos,
					&oparg.آغاز_vcol, &oparg.انتہا_vcol, 0);
#اگر_تعریف_شدہ FEAT_LINEBREAK
		p_sbr = saved_sbr;
		curwin->w_p_sbr = saved_w_sbr;
#اگر_ختم
		اگر (curwin->w_curswant == MAXCOL)
		    oparg.انتہا_vcol = MAXCOL;
		// Swap the start, end vcol if needed
		اگر (oparg.انتہا_vcol < oparg.آغاز_vcol)
		{
		    oparg.انتہا_vcol += oparg.آغاز_vcol;
		    oparg.آغاز_vcol = oparg.انتہا_vcol - oparg.آغاز_vcol;
		    oparg.انتہا_vcol -= oparg.آغاز_vcol;
		}
	    }
	    سطر_گنتی_selected = max_pos.lnum - min_pos.lnum + 1;
	}

	برائے (lnum = 1; lnum <= curbuf->b_ml.ml_سطر_گنتی; ++lnum)
	{
	    // Check for a CTRL-C every 100000 characters.
	    اگر (byte_گنتی > آخری_check)
	    {
		ui_breakcheck();
		اگر (got_int)
		    واپس;
		آخری_check = byte_گنتی + 100000L;
	    }

	    // Do extra processing for VIsual mode.
	    اگر (VIsual_فعال
		    && lnum >= min_pos.lnum && lnum <= max_pos.lnum)
	    {
		char_u	    *s = تہی;
		طویل	    len = 0L;

		انتخاب (VIsual_منوال)
		{
		    حالت Ctrl_V:
			virtual_op = virtual_فعال();
			بلاک_prep(&oparg, &bd, lnum, 0);
			virtual_op = MAYBE;
			s = bd.textstart;
			len = (طویل)bd.textlen;
			توڑو;
		    حالت 'V':
			s = ml_get(lnum);
			len = MAXCOL;
			توڑو;
		    حالت 'v':
			{
			    colnr_T آغاز_ستون = (lnum == min_pos.lnum)
							   ? min_pos.ستون : 0;
			    colnr_T انتہا_ستون = (lnum == max_pos.lnum)
				      ? max_pos.ستون - آغاز_ستون + 1 : MAXCOL;

			    s = ml_get(lnum) + آغاز_ستون;
			    len = انتہا_ستون;
			}
			توڑو;
		}
		اگر (s != تہی)
		{
		    byte_گنتی_cursor += سطر_گنتی_info(s, &لفظ_گنتی_cursor,
					   &char_گنتی_cursor, len, eol_سائز);
		    اگر (lnum == curbuf->b_ml.ml_سطر_گنتی
			    && !curbuf->b_p_eol
			    && (curbuf->b_p_bin || !curbuf->b_p_fixeol)
			    && (طویل)STRLEN(s) < len)
			byte_گنتی_cursor -= eol_سائز;
		}
	    }
	    ورنہ
	    {
		// In non-visual mode, check for the line the cursor is on
		اگر (lnum == curwin->w_cursor.lnum)
		{
		    لفظ_گنتی_cursor += لفظ_گنتی;
		    char_گنتی_cursor += char_گنتی;
		    byte_گنتی_cursor = byte_گنتی +
			سطر_گنتی_info(ml_get(lnum),
				&لفظ_گنتی_cursor, &char_گنتی_cursor,
				(varnumber_T)(curwin->w_cursor.ستون + 1),
				eol_سائز);
		}
	    }
	    // Add to the running totals
	    byte_گنتی += سطر_گنتی_info(ml_get(lnum), &لفظ_گنتی,
					 &char_گنتی, (varnumber_T)MAXCOL,
					 eol_سائز);
	}

	// Correction for when last line doesn't have an EOL.
	اگر (!curbuf->b_p_eol && (curbuf->b_p_bin || !curbuf->b_p_fixeol))
	    byte_گنتی -= eol_سائز;

	اگر (dict == تہی)
	{
	    اگر (VIsual_فعال)
	    {
		اگر (VIsual_منوال == Ctrl_V && curwin->w_curswant < MAXCOL)
		{
		    getvcols(curwin, &min_pos, &max_pos,
						&min_pos.ستون, &max_pos.ستون, 0);
		    vim_snprintf((حرف *)buf1, حجم(buf1), _("%ld Cols; "),
			    (طویل)(oparg.انتہا_vcol - oparg.آغاز_vcol + 1));
		}
		ورنہ
		    buf1[0] = NUL;

		اگر (char_گنتی_cursor == byte_گنتی_cursor
						    && char_گنتی == byte_گنتی)
		    vim_snprintf((حرف *)IObuff, IOSIZE,
			    _("Selected %s%ld of %ld Lines; %lld of %lld Words; %lld of %lld Bytes"),
			    buf1, سطر_گنتی_selected,
			    (طویل)curbuf->b_ml.ml_سطر_گنتی,
			    لفظ_گنتی_cursor,
			    لفظ_گنتی,
			    byte_گنتی_cursor,
			    byte_گنتی);
		ورنہ
		    vim_snprintf((حرف *)IObuff, IOSIZE,
			    _("Selected %s%ld of %ld Lines; %lld of %lld Words; %lld of %lld Chars; %lld of %lld Bytes"),
			    buf1, سطر_گنتی_selected,
			    (طویل)curbuf->b_ml.ml_سطر_گنتی,
			    لفظ_گنتی_cursor,
			    لفظ_گنتی,
			    char_گنتی_cursor,
			    char_گنتی,
			    byte_گنتی_cursor,
			    byte_گنتی);
	    }
	    ورنہ
	    {
		p = ml_get_curline();
		validate_virtcol();
		ستون_print(buf1, حجم(buf1), (صحیح)curwin->w_cursor.ستون + 1,
			(صحیح)curwin->w_virtcol + 1);
		ستون_print(buf2, حجم(buf2), ml_get_curline_len(),
							   linetabsize_str(p));

		اگر (char_گنتی_cursor == byte_گنتی_cursor
			&& char_گنتی == byte_گنتی)
		    vim_snprintf((حرف *)IObuff, IOSIZE,
			_("Col %s of %s; Line %ld of %ld; Word %lld of %lld; Byte %lld of %lld"),
			(حرف *)buf1, (حرف *)buf2,
			(طویل)curwin->w_cursor.lnum,
			(طویل)curbuf->b_ml.ml_سطر_گنتی,
			لفظ_گنتی_cursor, لفظ_گنتی,
			byte_گنتی_cursor, byte_گنتی);
		ورنہ
		    vim_snprintf((حرف *)IObuff, IOSIZE,
			_("Col %s of %s; Line %ld of %ld; Word %lld of %lld; Char %lld of %lld; Byte %lld of %lld"),
			(حرف *)buf1, (حرف *)buf2,
			(طویل)curwin->w_cursor.lnum,
			(طویل)curbuf->b_ml.ml_سطر_گنتی,
			لفظ_گنتی_cursor, لفظ_گنتی,
			char_گنتی_cursor, char_گنتی,
			byte_گنتی_cursor, byte_گنتی);
	    }
	}

	bom_گنتی = bomb_سائز();
	اگر (dict == تہی && bom_گنتی > 0)
	{
	    size_t len = STRLEN(IObuff);

	    vim_snprintf((حرف *)IObuff + len, IOSIZE - len,
				 _("(+%lld for BOM)"), bom_گنتی);
	}
	اگر (dict == تہی)
	{
	    // Don't shorten this message, the user asked for it.
	    p = p_shm;
	    p_shm = (char_u *)"";
	    پیغام((حرف *)IObuff);
	    p_shm = p;
	}
    }
#اگر defined(FEAT_EVAL)
    اگر (dict != تہی)
    {
	dict_جوڑو_نمبر(dict, "words", لفظ_گنتی);
	dict_جوڑو_نمبر(dict, "chars", char_گنتی);
	dict_جوڑو_نمبر(dict, "bytes", byte_گنتی + bom_گنتی);
	dict_جوڑو_نمبر(dict, VIsual_فعال ? "visual_bytes" : "cursor_bytes",
		byte_گنتی_cursor);
	dict_جوڑو_نمبر(dict, VIsual_فعال ? "visual_chars" : "cursor_chars",
		char_گنتی_cursor);
	dict_جوڑو_نمبر(dict, VIsual_فعال ? "visual_words" : "cursor_words",
		لفظ_گنتی_cursor);
    }
#اگر_ختم
}

/*
 * Handle indent and format operators and visual mode ":".
 */
    ساکن خالی
op_colon(oparg_T *oap)
{
    stuffcharReadbuff(':');
    اگر (oap->is_VIsual)
	stuffReadbuff((char_u *)"'<,'>");
    ورنہ
    {
	// Make the range look nice, so it can be repeated.
	اگر (oap->آغاز.lnum == curwin->w_cursor.lnum)
	    stuffcharReadbuff('.');
	ورنہ
	    stuffnumReadbuff((طویل)oap->آغاز.lnum);

#اگر_تعریف_شدہ FEAT_FOLDING
	// When using !! on a closed fold the range ".!" works best to operate
	// on, it will be made the whole closed fold later.
	linenr_T انتہا_کا_آغاز_Fold = oap->آغاز.lnum;
	(خالی)hasFolding(oap->آغاز.lnum, تہی, &انتہا_کا_آغاز_Fold);
#اگر_ختم
	اگر (oap->انتہا.lnum != oap->آغاز.lnum
#اگر_تعریف_شدہ FEAT_FOLDING
		&& oap->انتہا.lnum != انتہا_کا_آغاز_Fold
#اگر_ختم
	   )
	{
	    // Make it a range with the end line.
	    stuffcharReadbuff(',');
	    اگر (oap->انتہا.lnum == curwin->w_cursor.lnum)
		stuffcharReadbuff('.');
	    ورنہ اگر (oap->انتہا.lnum == curbuf->b_ml.ml_سطر_گنتی)
		stuffcharReadbuff('$');
	    ورنہ اگر (oap->آغاز.lnum == curwin->w_cursor.lnum
#اگر_تعریف_شدہ FEAT_FOLDING
		    // do not use ".+number" for a closed fold, it would count
		    // folded lines twice
		    && !hasFolding(oap->انتہا.lnum, تہی, تہی)
#اگر_ختم
		    )
	    {
		stuffReadbuff((char_u *)".+");
		stuffnumReadbuff((طویل)oap->سطر_گنتی - 1);
	    }
	    ورنہ
		stuffnumReadbuff((طویل)oap->انتہا.lnum);
	}
    }
    اگر (oap->op_type != OP_COLON)
	stuffReadbuff((char_u *)"!");
    اگر (oap->op_type == OP_INDENT)
    {
	اگر (*get_equalprg() == NUL)
	    stuffReadbuff((char_u *)"indent");
	ورنہ
	    stuffReadbuff(get_equalprg());
	stuffReadbuff((char_u *)"\n");
    }
    ورنہ اگر (oap->op_type == OP_FORMAT)
    {
	اگر (*curbuf->b_p_fp != NUL)
	    stuffReadbuff(curbuf->b_p_fp);
	ورنہ اگر (*p_fp != NUL)
	    stuffReadbuff(p_fp);
	ورنہ
	    stuffReadbuff((char_u *)"fmt");
	stuffReadbuff((char_u *)"\n']");
    }

    // do_cmdline() does the rest
}

// callback function for 'operatorfunc'
ساکن callback_T opfunc_cb;

/*
 * Process the 'operatorfunc' option value.
 * Returns OK or FAIL.
 */
    حرف *
did_سیٹ_operatorfunc(optset_T *دلائل غیر_مستعمل)
{
    اگر (option_سیٹ_callback_func(p_opfunc, &opfunc_cb) == FAIL)
	واپس e_invalid_دلیل;

    واپس تہی;
}

#اگر defined(EXITFREE)
    خالی
free_operatorfunc_option(خالی)
{
# ifdef FEAT_EVAL
    free_callback(&opfunc_cb);
# endif
}
#اگر_ختم

#اگر defined(FEAT_EVAL)
/*
 * Mark the global 'operatorfunc' callback with "copyID" so that it is not
 * garbage collected.
 */
    صحیح
سیٹ_ref_اندر_opfunc(صحیح copyID غیر_مستعمل)
{
    صحیح abort = FALSE;

    abort = سیٹ_ref_اندر_callback(&opfunc_cb, copyID);

    واپس abort;
}
#اگر_ختم

/*
 * Handle the "g@" operator: call 'operatorfunc'.
 */
    ساکن خالی
op_function(oparg_T *oap غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_EVAL
    typval_T	argv[2];
    pos_T	orig_آغاز = curbuf->b_op_آغاز;
    pos_T	orig_انتہا = curbuf->b_op_انتہا;
    typval_T	rettv;

    اگر (*p_opfunc == NUL)
	emsg(_(e_operatorfunc_is_خالی_خانے));
    ورنہ
    {
	// Set '[ and '] marks to text to be operated on.
	curbuf->b_op_آغاز = oap->آغاز;
	curbuf->b_op_انتہا = oap->انتہا;
	اگر (oap->motion_type != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_انتہا);

	argv[0].v_type = VAR_STRING;
	اگر (oap->بلاک_منوال)
	    argv[0].vval.v_string = (char_u *)"block";
	ورنہ اگر (oap->motion_type == MLINE)
	    argv[0].vval.v_string = (char_u *)"line";
	ورنہ
	    argv[0].vval.v_string = (char_u *)"char";
	argv[1].v_type = VAR_UNKNOWN;

	// Reset virtual_op so that 'virtualedit' can be changed in the
	// function.
	صحیح save_virtual_op = virtual_op;
	virtual_op = MAYBE;

	// Reset finish_op so that mode() returns the right value.
	صحیح save_اختتام_op = اختتام_op;
	اختتام_op = FALSE;

	اگر (call_callback(&opfunc_cb, 0, &rettv, 1, argv) != FAIL)
	    clear_tv(&rettv);

	virtual_op = save_virtual_op;
	اختتام_op = save_اختتام_op;
	اگر (cmdmod.cmod_flags & CMOD_LOCKMARKS)
	{
	    curbuf->b_op_آغاز = orig_آغاز;
	    curbuf->b_op_انتہا = orig_انتہا;
	}
    }
#ورنہ
    emsg(_(e_eval_feature_not_دستیاب));
#اگر_ختم
}

/*
 * Calculate start/end virtual columns for operating in block mode.
 */
    ساکن خالی
get_op_vcol(
    oparg_T	*oap,
    colnr_T	redo_VIsual_vcol,
    صحیح		initial)    // when TRUE adjust position for 'selectmode'
{
    colnr_T	    آغاز, انتہا;

    اگر (VIsual_منوال != Ctrl_V
	    || (!initial && oap->انتہا.ستون < curwin->w_چوڑائی))
	واپس;

    oap->بلاک_منوال = TRUE;

    // prevent from moving onto a trail byte
    اگر (has_mbyte)
	mb_adjustpos(curwin->w_بفر, &oap->انتہا);

    getvvcol(curwin, &(oap->آغاز), &oap->آغاز_vcol, تہی, &oap->انتہا_vcol, 0);

    اگر (!redo_VIsual_busy)
    {
	getvvcol(curwin, &(oap->انتہا), &آغاز, تہی, &انتہا, 0);

	اگر (آغاز < oap->آغاز_vcol)
	    oap->آغاز_vcol = آغاز;
	اگر (انتہا > oap->انتہا_vcol)
	{
	    اگر (initial && *p_sel == 'e' && آغاز >= 1
				    && آغاز - 1 >= oap->انتہا_vcol)
		oap->انتہا_vcol = آغاز - 1;
	    ورنہ
		oap->انتہا_vcol = انتہا;
	}
    }

    // if '$' was used, get oap->end_vcol from longest line
    اگر (curwin->w_curswant == MAXCOL)
    {
	curwin->w_cursor.ستون = MAXCOL;
	oap->انتہا_vcol = 0;
	برائے (curwin->w_cursor.lnum = oap->آغاز.lnum;
		curwin->w_cursor.lnum <= oap->انتہا.lnum;
					++curwin->w_cursor.lnum)
	{
	    getvvcol(curwin, &curwin->w_cursor, تہی, تہی, &انتہا, 0);
	    اگر (انتہا > oap->انتہا_vcol)
		oap->انتہا_vcol = انتہا;
	}
    }
    ورنہ اگر (redo_VIsual_busy)
	oap->انتہا_vcol = oap->آغاز_vcol + redo_VIsual_vcol - 1;
    // Correct oap->end.col and oap->start.col to be the
    // upper-left and lower-right corner of the block area.
    //
    // (Actually, this does convert column positions into character
    // positions)
    curwin->w_cursor.lnum = oap->انتہا.lnum;
    coladvance(oap->انتہا_vcol);
    oap->انتہا = curwin->w_cursor;

    curwin->w_cursor = oap->آغاز;
    coladvance(oap->آغاز_vcol);
    oap->آغاز = curwin->w_cursor;
}

// Information for redoing the previous Visual selection.
قسمتعریف ساخت {
    صحیح		rv_منوال;	// 'v', 'V', or Ctrl-V
    linenr_T	rv_سطر_گنتی;	// number of lines
    colnr_T	rv_vcol;	// number of cols or end column
    طویل	rv_گنتی;	// count for Visual operator
    صحیح		rv_arg;		// extra argument
} redo_VIsual_T;

    ساکن صحیح
is_ex_cmdchar(cmdarg_T *cap)
{
    واپس cap->cmdchar == ':'
	|| cap->cmdchar == K_COMMAND
	|| cap->cmdchar == K_SCRIPT_COMMAND;
}

/*
 * Handle an operator after Visual mode or when the movement is finished.
 * "gui_yank" is true when yanking text for the clipboard.
 */
    خالی
do_pending_operator(cmdarg_T *cap, صحیح old_ستون, صحیح gui_yank)
{
    oparg_T	*oap = cap->oap;
    pos_T	old_cursor;
    صحیح		خالی_خانے_region_error;
    صحیح		restart_edit_save;
#اگر_تعریف_شدہ FEAT_LINEBREAK
    صحیح		lbr_saved = curwin->w_p_lbr;
#اگر_ختم

    // The visual area is remembered for redo
    ساکن redo_VIsual_T   redo_VIsual = {NUL, 0, 0, 0,0};

    صحیح		    include_سطر_break = FALSE;

#اگر defined(FEAT_CLIPBOARD)
    // Yank the visual area into the GUI selection register before we operate
    // on it and lose it forever.
    // Don't do it if a specific register was specified, so that ""x"*P works.
    // This could call do_pending_operator() recursively, but that's OK
    // because gui_yank will be TRUE for the nested call.
    اگر ((clip_ستارہ.دستیاب || clip_plus.دستیاب)
	    && oap->op_type != OP_NOP
	    && !gui_yank
	    && VIsual_فعال
	    && !redo_VIsual_busy
	    && oap->regname == 0)
	clip_auto_select();
#اگر_ختم
    old_cursor = curwin->w_cursor;

    // If an operation is pending, handle it...
    اگر ((اختتام_op || VIsual_فعال) && oap->op_type != OP_NOP)
    {
	// Yank can be redone when 'y' is in 'cpoptions', but not when yanking
	// for the clipboard.
	صحیح	redo_yank = vim_strchr(p_cpo, CPO_YANK) != تہی && !gui_yank;

#اگر_تعریف_شدہ FEAT_LINEBREAK
	// Avoid a problem with unwanted linebreaks in block mode.
	(خالی)reset_lbr();
#اگر_ختم
	oap->is_VIsual = VIsual_فعال;
	اگر (oap->motion_force == 'V')
	    oap->motion_type = MLINE;
	ورنہ اگر (oap->motion_force == 'v')
	{
	    // If the motion was linewise, "inclusive" will not have been set.
	    // Use "exclusive" to be consistent.  Makes "dvj" work nice.
	    اگر (oap->motion_type == MLINE)
		oap->inclusive = FALSE;
	    // If the motion already was characterwise, toggle "inclusive"
	    ورنہ اگر (oap->motion_type == MCHAR)
		oap->inclusive = !oap->inclusive;
	    oap->motion_type = MCHAR;
	}
	ورنہ اگر (oap->motion_force == Ctrl_V)
	{
	    // Change line- or characterwise motion into Visual block mode.
	    اگر (!VIsual_فعال)
	    {
		VIsual_فعال = TRUE;
		VIsual = oap->آغاز;
	    }
	    VIsual_منوال = Ctrl_V;
	    VIsual_select = FALSE;
	    VIsual_reselect = FALSE;
	}

	// Only redo yank when 'y' flag is in 'cpoptions'.
	// Never redo "zf" (define fold).
	اگر ((redo_yank || oap->op_type != OP_YANK)
		&& ((!VIsual_فعال || oap->motion_force)
		    // Also redo Operator-pending Visual mode mappings
		    || (VIsual_فعال
			    && is_ex_cmdchar(cap) && oap->op_type != OP_COLON))
		&& cap->cmdchar != 'D'
#اگر_تعریف_شدہ FEAT_FOLDING
		&& oap->op_type != OP_FOLD
		&& oap->op_type != OP_FOLDOPEN
		&& oap->op_type != OP_FOLDOPENREC
		&& oap->op_type != OP_FOLDCLOSE
		&& oap->op_type != OP_FOLDCLOSEREC
		&& oap->op_type != OP_FOLDDEL
		&& oap->op_type != OP_FOLDDELREC
#اگر_ختم
		)
	{
	    prep_redo(oap->regname, cap->count0,
		    get_op_char(oap->op_type), get_extra_op_char(oap->op_type),
		    oap->motion_force, cap->cmdchar, cap->nchar);
	    اگر (cap->cmdchar == '/' || cap->cmdchar == '?') // was a search
	    {
		// If 'cpoptions' does not contain 'r', insert the search
		// pattern to really repeat the same command.
		اگر (vim_strchr(p_cpo, CPO_REDO) == تہی)
		    Append_مقصد_Redobuff_Lit(cap->searchbuf, -1);
		Append_مقصد_Redobuff(NL_STR);
	    }
	    ورنہ اگر (is_ex_cmdchar(cap))
	    {
		// do_cmdline() has stored the first typed line in
		// "repeat_cmdline".  When several lines are typed repeating
		// won't be possible.
		اگر (repeat_cmdline == تہی)
		    ResetRedobuff();
		ورنہ
		{
		    اگر (cap->cmdchar == ':')
			Append_مقصد_Redobuff_Lit(repeat_cmdline, -1);
		    ورنہ
			Append_مقصد_Redobuff_Spec(repeat_cmdline);
		    Append_مقصد_Redobuff(NL_STR);
		    VIM_CLEAR(repeat_cmdline);
		}
	    }
	}

	اگر (redo_VIsual_busy)
	{
	    // Redo of an operation on a Visual area. Use the same size from
	    // redo_VIsual.rv_line_count and redo_VIsual.rv_vcol.
	    oap->آغاز = curwin->w_cursor;
	    curwin->w_cursor.lnum += redo_VIsual.rv_سطر_گنتی - 1;
	    اگر (curwin->w_cursor.lnum > curbuf->b_ml.ml_سطر_گنتی)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_سطر_گنتی;
	    VIsual_منوال = redo_VIsual.rv_منوال;
	    اگر (redo_VIsual.rv_vcol == MAXCOL || VIsual_منوال == 'v')
	    {
		اگر (VIsual_منوال == 'v')
		{
		    اگر (redo_VIsual.rv_سطر_گنتی <= 1)
		    {
			validate_virtcol();
			curwin->w_curswant =
				     curwin->w_virtcol + redo_VIsual.rv_vcol - 1;
		    }
		    ورنہ
			curwin->w_curswant = redo_VIsual.rv_vcol;
		}
		ورنہ
		{
		    curwin->w_curswant = MAXCOL;
		}
		coladvance(curwin->w_curswant);
	    }
	    cap->count0 = redo_VIsual.rv_گنتی;
	    اگر (redo_VIsual.rv_گنتی != 0)
		cap->count1 = redo_VIsual.rv_گنتی;
	    ورنہ
		cap->count1 = 1;
	}
	ورنہ اگر (VIsual_فعال)
	{
	    اگر (!gui_yank)
	    {
		// Save the current VIsual area for '< and '> marks, and "gv"
		curbuf->b_visual.vi_آغاز = VIsual;
		curbuf->b_visual.vi_انتہا = curwin->w_cursor;
		curbuf->b_visual.vi_منوال = VIsual_منوال;
		restore_visual_منوال();
		curbuf->b_visual.vi_curswant = curwin->w_curswant;
#اگر_تعریف_شدہ FEAT_EVAL
		curbuf->b_visual_منوال_eval = VIsual_منوال;
#اگر_ختم
	    }

	    // In Select mode, a linewise selection is operated upon like a
	    // characterwise selection.
	    // Special case: gH<Del> deletes the last line.
	    اگر (VIsual_select && VIsual_منوال == 'V'
					    && cap->oap->op_type != OP_DELETE)
	    {
		اگر (LT_POS(VIsual, curwin->w_cursor))
		{
		    VIsual.ستون = 0;
		    curwin->w_cursor.ستون = ml_get_len(curwin->w_cursor.lnum);
		}
		ورنہ
		{
		    curwin->w_cursor.ستون = 0;
		    VIsual.ستون = ml_get_len(VIsual.lnum);
		}
		VIsual_منوال = 'v';
	    }
	    // If 'selection' is "exclusive", backup one character for
	    // charwise selections.
	    ورنہ اگر (VIsual_منوال == 'v')
		include_سطر_break = unadjust_for_sel();

	    oap->آغاز = VIsual;
	    اگر (VIsual_منوال == 'V')
	    {
		oap->آغاز.ستون = 0;
		oap->آغاز.coladd = 0;
	    }
	}

	// Set oap->start to the first position of the operated text, oap->end
	// to the end of the operated text.  w_cursor is equal to oap->start.
	اگر (LT_POS(oap->آغاز, curwin->w_cursor))
	{
#اگر_تعریف_شدہ FEAT_FOLDING
	    // Include folded lines completely.
	    اگر (!VIsual_فعال)
	    {
		اگر (hasFolding(oap->آغاز.lnum, &oap->آغاز.lnum, تہی))
		    oap->آغاز.ستون = 0;
		اگر ((curwin->w_cursor.ستون > 0 || oap->inclusive
						  || oap->motion_type == MLINE)
			&& hasFolding(curwin->w_cursor.lnum, تہی,
						      &curwin->w_cursor.lnum))
		    curwin->w_cursor.ستون = ml_get_curline_len();
	    }
#اگر_ختم
	    oap->انتہا = curwin->w_cursor;
	    curwin->w_cursor = oap->آغاز;

	    // w_virtcol may have been updated; if the cursor goes back to its
	    // previous position w_virtcol becomes invalid and isn't updated
	    // automatically.
	    curwin->w_valid &= ~VALID_VIRTCOL;
	}
	ورنہ
	{
#اگر_تعریف_شدہ FEAT_FOLDING
	    // Include folded lines completely.
	    اگر (!VIsual_فعال && oap->motion_type == MLINE)
	    {
		اگر (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum,
									تہی))
		    curwin->w_cursor.ستون = 0;
		اگر (hasFolding(oap->آغاز.lnum, تہی, &oap->آغاز.lnum))
		    oap->آغاز.ستون = ml_get_len(oap->آغاز.lnum);
	    }
#اگر_ختم
	    oap->انتہا = oap->آغاز;
	    oap->آغاز = curwin->w_cursor;
	}

	// Just in case lines were deleted that make the position invalid.
	check_pos(curwin->w_بفر, &oap->انتہا);
	oap->سطر_گنتی = oap->انتہا.lnum - oap->آغاز.lnum + 1;

	// Set "virtual_op" before resetting VIsual_active.
	virtual_op = virtual_فعال();

	اگر (VIsual_فعال || redo_VIsual_busy)
	{
	    get_op_vcol(oap, redo_VIsual.rv_vcol, TRUE);

	    اگر (!redo_VIsual_busy && !gui_yank)
	    {
		// Prepare to reselect and redo Visual: this is based on the
		// size of the Visual text
		resel_VIsual_منوال = VIsual_منوال;
		اگر (curwin->w_curswant == MAXCOL)
		    resel_VIsual_vcol = MAXCOL;
		ورنہ
		{
		    اگر (VIsual_منوال != Ctrl_V)
			getvvcol(curwin, &(oap->انتہا),
						تہی, تہی, &oap->انتہا_vcol, 0);
		    اگر (VIsual_منوال == Ctrl_V || oap->سطر_گنتی <= 1)
		    {
			اگر (VIsual_منوال != Ctrl_V)
			    getvvcol(curwin, &(oap->آغاز),
					      &oap->آغاز_vcol, تہی, تہی, 0);
			resel_VIsual_vcol = oap->انتہا_vcol - oap->آغاز_vcol + 1;
		    }
		    ورنہ
			resel_VIsual_vcol = oap->انتہا_vcol;
		}
		resel_VIsual_سطر_گنتی = oap->سطر_گنتی;
	    }

	    // can't redo yank (unless 'y' is in 'cpoptions') and ":"
	    اگر ((redo_yank || oap->op_type != OP_YANK)
		    && oap->op_type != OP_COLON
#اگر_تعریف_شدہ FEAT_FOLDING
		    && oap->op_type != OP_FOLD
		    && oap->op_type != OP_FOLDOPEN
		    && oap->op_type != OP_FOLDOPENREC
		    && oap->op_type != OP_FOLDCLOSE
		    && oap->op_type != OP_FOLDCLOSEREC
		    && oap->op_type != OP_FOLDDEL
		    && oap->op_type != OP_FOLDDELREC
#اگر_ختم
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
		ورنہ اگر (!is_ex_cmdchar(cap))
		{
		    صحیح opchar = get_op_char(oap->op_type);
		    صحیح extra_opchar = get_extra_op_char(oap->op_type);
		    صحیح nchar = oap->op_type == OP_REPLACE ? cap->nchar : NUL;

		    // reverse what nv_replace() did
		    اگر (nchar == REPLACE_CR_NCHAR)
			nchar = CAR;
		    ورنہ اگر (nchar == REPLACE_NL_NCHAR)
			nchar = NL;

		    اگر (opchar == 'g' && extra_opchar == '@')
			// also repeat the count for 'operatorfunc'
			prep_redo_num2(oap->regname, 0L, NUL, 'v',
				     cap->count0, opchar, extra_opchar, nchar);
		    ورنہ
			prep_redo(oap->regname, 0L, NUL, 'v',
						  opchar, extra_opchar, nchar);
		}
		اگر (!redo_VIsual_busy)
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
	    اگر (oap->motion_force == NUL || oap->motion_type == MLINE)
		oap->inclusive = TRUE;
	    اگر (VIsual_منوال == 'V')
		oap->motion_type = MLINE;
	    ورنہ
	    {
		oap->motion_type = MCHAR;
		اگر (VIsual_منوال != Ctrl_V && *ml_get_pos(&(oap->انتہا)) == NUL
			&& (include_سطر_break || !virtual_op))
		{
		    oap->inclusive = FALSE;
		    // Try to include the newline, unless it's an operator
		    // that works on lines only.
		    اگر (*p_sel != 'o'
			    && !op_on_سطریں(oap->op_type)
			    && oap->انتہا.lnum < curbuf->b_ml.ml_سطر_گنتی)
		    {
			++oap->انتہا.lnum;
			oap->انتہا.ستون = 0;
			oap->انتہا.coladd = 0;
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
	    اگر (!gui_yank)
	    {
		VIsual_فعال = FALSE;
		setmouse();
		mouse_dragging = 0;
		may_clear_cmdline();
		اگر ((oap->op_type == OP_YANK
			    || oap->op_type == OP_COLON
			    || oap->op_type == OP_FUNCTION
			    || oap->op_type == OP_FILTER)
			&& oap->motion_force == NUL)
		{
#اگر_تعریف_شدہ FEAT_LINEBREAK
		    // make sure redrawing is correct
		    restore_lbr(lbr_saved);
#اگر_ختم
		    redraw_curbuf_later(UPD_INVERTED);
		}
	    }
	}

	// Include the trailing byte of a multi-byte char.
	اگر (has_mbyte && oap->inclusive)
	{
	    صحیح		l;

	    l = (*mb_ptr2len)(ml_get_pos(&oap->انتہا));
	    اگر (l > 1)
		oap->انتہا.ستون += l - 1;
	}
	curwin->w_سیٹ_curswant = true;

	// oap->empty is set when start and end are the same.  The inclusive
	// flag affects this too, unless yanking and the end is on a NUL.
	oap->خالی_خانے_2 = (oap->motion_type == MCHAR
		    && (!oap->inclusive
			|| (oap->op_type == OP_YANK
			    && gchar_pos(&oap->انتہا) == NUL))
		    && EQUAL_POS(oap->آغاز, oap->انتہا)
		    && !(virtual_op && oap->آغاز.coladd != oap->انتہا.coladd));
	// For delete, change and yank, it's an error to operate on an
	// empty region, when 'E' included in 'cpoptions' (Vi compatible).
	خالی_خانے_region_error = (oap->خالی_خانے_2
				&& vim_strchr(p_cpo, CPO_EMPTYREGION) != تہی);

	// Force a redraw when operating on an empty Visual region, when
	// 'modifiable is off or creating a fold.
	اگر (oap->is_VIsual && (oap->خالی_خانے_2 || !curbuf->b_p_ma
#اگر_تعریف_شدہ FEAT_FOLDING
		    || oap->op_type == OP_FOLD
#اگر_ختم
		    ))
	{
#اگر_تعریف_شدہ FEAT_LINEBREAK
	    restore_lbr(lbr_saved);
#اگر_ختم
	    redraw_curbuf_later(UPD_INVERTED);
	}

	// If the end of an operator is in column one while oap->motion_type
	// is MCHAR and oap->inclusive is FALSE, we put op_end after the last
	// character in the previous line. If op_start is on or before the
	// first non-blank in the line, the operator becomes linewise
	// (strange, but that's the way vi does it).
	اگر (	   oap->motion_type == MCHAR
		&& oap->inclusive == FALSE
		&& !(cap->retval & CA_NO_ADJ_OP_انتہا)
		&& oap->انتہا.ستون == 0
		&& (!oap->is_VIsual || *p_sel == 'o')
		&& !oap->بلاک_منوال
		&& oap->سطر_گنتی > 1)
	{
	    oap->انتہا_adjusted = TRUE;	    // remember that we did this
	    --oap->سطر_گنتی;
	    --oap->انتہا.lnum;
	    اگر (inindent(0))
		oap->motion_type = MLINE;
	    ورنہ
	    {
		oap->انتہا.ستون = ml_get_len(oap->انتہا.lnum);
		اگر (oap->انتہا.ستون)
		{
		    --oap->انتہا.ستون;
		    oap->inclusive = TRUE;
		}
	    }
	}
	ورنہ
	    oap->انتہا_adjusted = FALSE;

	انتخاب (oap->op_type)
	{
	حالت OP_LSHIFT:
	حالت OP_RSHIFT:
	    op_سرکاؤ(oap, TRUE, oap->is_VIsual ? (صحیح)cap->count1 : 1);
	    auto_format(FALSE, TRUE);
	    توڑو;

	حالت OP_JOIN_NS:
	حالت OP_JOIN:
	    اگر (oap->سطر_گنتی < 2)
		oap->سطر_گنتی = 2;
	    اگر (curwin->w_cursor.lnum + oap->سطر_گنتی - 1 >
						   curbuf->b_ml.ml_سطر_گنتی)
		beep_flush();
	    ورنہ
	    {
		(خالی)do_join(oap->سطر_گنتی, oap->op_type == OP_JOIN,
							    TRUE, TRUE, TRUE);
		auto_format(FALSE, TRUE);
	    }
	    توڑو;

	حالت OP_DELETE:
	    VIsual_reselect = FALSE;	    // don't reselect now
	    اگر (خالی_خانے_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    ورنہ
	    {
		(خالی)op_delete(oap);
		// save cursor line for undo if it wasn't saved yet
		اگر (oap->motion_type == MLINE && has_format_option(FO_AUTO)
						      && u_save_cursor() == OK)
		    auto_format(FALSE, TRUE);
	    }
	    توڑو;

	حالت OP_YANK:
	    اگر (خالی_خانے_region_error)
	    {
		اگر (!gui_yank)
		{
		    vim_beep(BO_OPER);
		    CancelRedo();
		}
	    }
	    ورنہ
	    {
#اگر_تعریف_شدہ FEAT_LINEBREAK
		restore_lbr(lbr_saved);
#اگر_ختم
		oap->excl_tr_ws = cap->cmdchar == 'z';
		(خالی)op_yank(oap, FALSE, !gui_yank);
	    }
	    check_cursor_ستون();
	    توڑو;

	حالت OP_CHANGE:
	    VIsual_reselect = FALSE;	    // don't reselect now
	    اگر (خالی_خانے_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    ورنہ
	    {
		// This is a new edit command, not a restart.  Need to
		// remember it to make 'insertmode' work with mappings for
		// Visual mode.  But do this only once and not when typed and
		// 'insertmode' isn't set.
		اگر (p_im || !کلید_Typed)
		    restart_edit_save = restart_edit;
		ورنہ
		    restart_edit_save = 0;
		restart_edit = 0;
#اگر_تعریف_شدہ FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#اگر_ختم
		// trigger TextChangedI
		curbuf->b_آخری_changedtick_i = CHANGEDTICK(curbuf);

		اگر (op_change(oap))	// will call edit()
		    cap->retval |= CA_COMMAND_BUSY;
		اگر (restart_edit == 0)
		    restart_edit = restart_edit_save;
	    }
	    توڑو;

	حالت OP_FILTER:
	    اگر (vim_strchr(p_cpo, CPO_FILTER) != تہی)
		Append_مقصد_Redobuff((char_u *)"!\r");  // use any last used !cmd
	    ورنہ
		bangredo = TRUE;    // do_bang() will put cmd in redo buffer
	    // FALLTHROUGH

	حالت OP_INDENT:
	حالت OP_COLON:

	    // If 'equalprg' is empty, do the indenting internally.
	    اگر (oap->op_type == OP_INDENT && *get_equalprg() == NUL)
	    {
		اگر (curbuf->b_p_lisp)
		{
#اگر_تعریف_شدہ FEAT_EVAL
		    اگر (use_indentexpr_for_lisp())
			op_reindent(oap, get_اظہاریہ_indent);
		    ورنہ
#اگر_ختم
			op_reindent(oap, get_lisp_indent);
		    توڑو;
		}
		op_reindent(oap,
#اگر_تعریف_شدہ FEAT_EVAL
			*curbuf->b_p_inde != NUL ? get_اظہاریہ_indent :
#اگر_ختم
			    get_c_indent);
		توڑو;
	    }

	    op_colon(oap);
	    توڑو;

	حالت OP_TILDE:
	حالت OP_بالائی:
	حالت OP_زیریں:
	حالت OP_ROT13:
	    اگر (خالی_خانے_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    ورنہ
		op_tilde(oap);
	    check_cursor_ستون();
	    توڑو;

	حالت OP_FORMAT:
#اگر defined(FEAT_EVAL)
	    اگر (*curbuf->b_p_fex != NUL)
		op_formatexpr(oap);	// use expression
	    ورنہ
#اگر_ختم
	    {
		اگر (*p_fp != NUL || *curbuf->b_p_fp != NUL)
		    op_colon(oap);		// use external command
		ورنہ
		    op_format(oap, FALSE);	// use internal function
	    }
	    توڑو;
	حالت OP_FORMAT2:
	    op_format(oap, TRUE);	// use internal function
	    توڑو;

	حالت OP_FUNCTION:
	    {
		redo_VIsual_T   save_redo_VIsual = redo_VIsual;

#اگر_تعریف_شدہ FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#اگر_ختم
		// call 'operatorfunc'
		op_function(oap);

		// Restore the info for redoing Visual mode, the function may
		// invoke another operator and unintentionally change it.
		redo_VIsual = save_redo_VIsual;
		توڑو;
	    }

	حالت OP_داخل_کرو:
	حالت OP_APPEND:
	    VIsual_reselect = FALSE;	// don't reselect now
	    اگر (خالی_خانے_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    ورنہ
	    {
		// This is a new edit command, not a restart.  Need to
		// remember it to make 'insertmode' work with mappings for
		// Visual mode.  But do this only once.
		restart_edit_save = restart_edit;
		restart_edit = 0;
#اگر_تعریف_شدہ FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#اگر_ختم
		// trigger TextChangedI
		curbuf->b_آخری_changedtick_i = CHANGEDTICK(curbuf);

		op_داخل_کرو(oap, cap->count1);
#اگر_تعریف_شدہ FEAT_LINEBREAK
		// Reset linebreak, so that formatting works correctly.
		(خالی)reset_lbr();
#اگر_ختم

		// TODO: when inserting in several lines, should format all
		// the lines.
		auto_format(FALSE, TRUE);

		اگر (restart_edit == 0)
		    restart_edit = restart_edit_save;
		ورنہ
		    cap->retval |= CA_COMMAND_BUSY;
	    }
	    توڑو;

	حالت OP_REPLACE:
	    VIsual_reselect = FALSE;	// don't reselect now
	    اگر (خالی_خانے_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    ورنہ
	    {
#اگر_تعریف_شدہ FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#اگر_ختم
		op_replace(oap, cap->nchar);
	    }
	    توڑو;

#اگر_تعریف_شدہ FEAT_FOLDING
	حالت OP_FOLD:
	    VIsual_reselect = FALSE;	// don't reselect now
	    foldCreate(oap->آغاز.lnum, oap->انتہا.lnum);
	    توڑو;

	حالت OP_FOLDOPEN:
	حالت OP_FOLDOPENREC:
	حالت OP_FOLDCLOSE:
	حالت OP_FOLDCLOSEREC:
	    VIsual_reselect = FALSE;	// don't reselect now
	    opFoldRange(oap->آغاز.lnum, oap->انتہا.lnum,
		    oap->op_type == OP_FOLDOPEN
					    || oap->op_type == OP_FOLDOPENREC,
		    oap->op_type == OP_FOLDOPENREC
					  || oap->op_type == OP_FOLDCLOSEREC,
					  oap->is_VIsual);
	    توڑو;

	حالت OP_FOLDDEL:
	حالت OP_FOLDDELREC:
	    VIsual_reselect = FALSE;	// don't reselect now
	    deleteFold(oap->آغاز.lnum, oap->انتہا.lnum,
			       oap->op_type == OP_FOLDDELREC, oap->is_VIsual);
	    توڑو;
#اگر_ختم
	حالت OP_NR_جوڑو:
	حالت OP_NR_SUB:
	    اگر (خالی_خانے_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    ورنہ
	    {
		VIsual_فعال = TRUE;
#اگر_تعریف_شدہ FEAT_LINEBREAK
		restore_lbr(lbr_saved);
#اگر_ختم
		op_addsub(oap, cap->count1, redo_VIsual.rv_arg);
		VIsual_فعال = FALSE;
	    }
	    check_cursor_ستون();
	    توڑو;
	طےشدہ:
	    clearopbeep(oap);
	}
	virtual_op = MAYBE;
	اگر (!gui_yank)
	{
	    // if 'sol' not set, go back to old column for some commands
	    اگر (!p_sol && oap->motion_type == MLINE && !oap->انتہا_adjusted
		    && (oap->op_type == OP_LSHIFT || oap->op_type == OP_RSHIFT
						|| oap->op_type == OP_DELETE))
	    {
#اگر_تعریف_شدہ FEAT_LINEBREAK
		(خالی)reset_lbr();
#اگر_ختم
		coladvance(curwin->w_curswant = old_ستون);
	    }
	}
	ورنہ
	{
	    curwin->w_cursor = old_cursor;
	}
	oap->بلاک_منوال = FALSE;
	clearop(oap);
	motion_force = NUL;
    }
#اگر_تعریف_شدہ FEAT_LINEBREAK
    restore_lbr(lbr_saved);
#اگر_ختم
}

// put byte 'c' at position 'lp', but
// verify, that the position to place
// is actually safe
    ساکن خالی
pbyte(pos_T lp, صحیح c)
{
    char_u *p = ml_get_buf(curbuf, lp.lnum, TRUE);
    صحیح	len = curbuf->b_ml.ml_سطر_len;

    // safety check
    اگر (lp.ستون >= len)
	lp.ستون = (len > 1 ? len - 2 : 0);
    *(p + lp.ستون) = c;
}
