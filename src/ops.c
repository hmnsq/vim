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

ساكن فراغ سرڪايو_بلاڪ(oparg_T *oap, صحيح amount);
ساكن فراغ	mb_adjust_opend(oparg_T *oap);
ساكن صحيح	do_addsub(صحيح op_type, pos_T *pos, صحيح ڊيگهائي, linenr_T Prenum1);
ساكن فراغ	pbyte(pos_T lp, صحيح c);
#وصف_ڪريو PBYTE(lp, c) pbyte(lp, c)


// Flags for third item in "opchars".
#وصف_ڪريو OPF_سٽون  1	// operator always works on lines
#وصف_ڪريو OPF_CHANGE 2	// operator changes text

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
    {'<', NUL, OPF_سٽون | OPF_CHANGE},	// OP_LSHIFT
    {'>', NUL, OPF_سٽون | OPF_CHANGE},	// OP_RSHIFT
    {'!', NUL, OPF_سٽون | OPF_CHANGE},	// OP_FILTER
    {'g', '~', OPF_CHANGE},		// OP_TILDE
    {'=', NUL, OPF_سٽون | OPF_CHANGE},	// OP_INDENT
    {'g', 'q', OPF_سٽون | OPF_CHANGE},	// OP_FORMAT
    {':', NUL, OPF_سٽون},		// OP_COLON
    {'g', 'U', OPF_CHANGE},		// OP_UPPER
    {'g', 'u', OPF_CHANGE},		// OP_LOWER
    {'J', NUL, OPF_سٽون | OPF_CHANGE},	// DO_JOIN
    {'g', 'J', OPF_سٽون | OPF_CHANGE},	// DO_JOIN_NS
    {'g', '?', OPF_CHANGE},		// OP_ROT13
    {'r', NUL, OPF_CHANGE},		// OP_REPLACE
    {'I', NUL, OPF_CHANGE},		// OP_INSERT
    {'A', NUL, OPF_CHANGE},		// OP_APPEND
    {'z', 'f', OPF_سٽون},		// OP_FOLD
    {'z', 'o', OPF_سٽون},		// OP_FOLDOPEN
    {'z', 'O', OPF_سٽون},		// OP_FOLDOPENREC
    {'z', 'c', OPF_سٽون},		// OP_FOLDCLOSE
    {'z', 'C', OPF_سٽون},		// OP_FOLDCLOSEREC
    {'z', 'd', OPF_سٽون},		// OP_FOLDDEL
    {'z', 'D', OPF_سٽون},		// OP_FOLDDELREC
    {'g', 'w', OPF_سٽون | OPF_CHANGE},	// OP_FORMAT2
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
    إذا (char1 == 'g' && char2 == Ctrl_A)	//جوڙيو
	أرجع OP_NR_جوڙيو;
    إذا (char1 == 'g' && char2 == Ctrl_X)	//ڪٽيو
	أرجع OP_NR_SUB;
    إذا (char1 == 'z' && char2 == 'y')	// OP_YANK
	أرجع OP_YANK;
    لكل (i = 0; ; ++i)
    {
	إذا (opchars[i][0] == char1 && opchars[i][1] == char2)
	    اكسر;
	إذا (i == (صحيح)ARRAY_ڊيگهائي(opchars) - 1)
	{
	    اندروني_error("get_op_type()");
	    اكسر;
	}
    }
    أرجع i;
}

/*
 * Return TRUE if operator "op" always works on whole lines.
 */
    ساكن صحيح
op_on_سٽون(صحيح op)
{
    أرجع opchars[op][2] & OPF_سٽون;
}

#إذا defined(FEAT_ڪم_CHANNEL)
/*
 * Return TRUE if operator "op" changes text.
 */
    صحيح
op_is_change(صحيح op)
{
    أرجع opchars[op][2] & OPF_CHANGE;
}
#جيڪڏهن_پورو

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
op_سرڪايو(oparg_T *oap, صحيح curs_چوٽي, صحيح amount)
{
    ڊگهو	    i;
    صحيح		    پهريون_char;
    صحيح		    بلاڪ_کانو = 0;

    إذا (u_save((linenr_T)(oap->شروعات.lnum - 1),
				       (linenr_T)(oap->پڇاڙي.lnum + 1)) == FAIL)
	أرجع;

    إذا (oap->بلاڪ_گهڻائي)
	بلاڪ_کانو = curwin->w_cursor.کانو;

    لكل (i = oap->سٽ_ڳڻپ; --i >= 0; )
    {
	پهريون_char = *ml_get_curline();
	إذا (پهريون_char == NUL)				// empty line
	    curwin->w_cursor.کانو = 0;
	وإلا إذا (oap->بلاڪ_گهڻائي)
	    سرڪايو_بلاڪ(oap, amount);
	وإلا
	    // Move the line right if it doesn't start with '#', 'smartindent'
	    // isn't set or 'cindent' isn't set or '#' isn't in 'cino'.
	    إذا (پهريون_char != '#' || !preprocs_کاٻو())
		سرڪايو_سٽ(oap->op_type == OP_LSHIFT, p_sr, amount, FALSE);
	++curwin->w_cursor.lnum;
    }

    بدليو_سٽون(oap->شروعات.lnum, 0, oap->پڇاڙي.lnum + 1, 0L);
    إذا (oap->بلاڪ_گهڻائي)
    {
	curwin->w_cursor.lnum = oap->شروعات.lnum;
	curwin->w_cursor.کانو = بلاڪ_کانو;
    }
    وإلا إذا (curs_چوٽي)	    // put cursor on first line, for ">>"
    {
	curwin->w_cursor.lnum = oap->شروعات.lnum;
	beginline(BL_SOL | BL_FIX);   // shift_line() may have set cursor.col
    }
    وإلا
	--curwin->w_cursor.lnum;	// put cursor on last line, for ":>"

#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
    // The cursor line is not in a closed fold
    foldOpenCursor();
#جيڪڏهن_پورو


    إذا (oap->سٽ_ڳڻپ > p_report)
    {
	حرف	    *op;
	حرف	    *نياپو_سٽ_single;
	حرف	    *نياپو_سٽ_plural;

	إذا (oap->op_type == OP_RSHIFT)
	    op = ">";
	وإلا
	    op = "<";
	نياپو_سٽ_single = NGETTEXT("%ld line %sed %d time",
					     "%ld line %sed %d times", amount);
	نياپو_سٽ_plural = NGETTEXT("%ld lines %sed %d time",
					    "%ld lines %sed %d times", amount);
	vim_snprintf((حرف *)IObuff, IOSIZE,
		NGETTEXT(نياپو_سٽ_single, نياپو_سٽ_plural, oap->سٽ_ڳڻپ),
		oap->سٽ_ڳڻپ, op, amount);
	نياپو_attr_keep((حرف *)IObuff, 0, TRUE);
    }

    إذا ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_شروعات = oap->شروعات;
	curbuf->b_op_پڇاڙي.lnum = oap->پڇاڙي.lnum;
	curbuf->b_op_پڇاڙي.کانو = ml_get_len(oap->پڇاڙي.lnum);
	إذا (curbuf->b_op_پڇاڙي.کانو > 0)
	    --curbuf->b_op_پڇاڙي.کانو;
    }
}

#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
/*
 * Return the tabstop width at the index of the variable tabstop array.  If an
 * index greater than the length of the array is given, the last tabstop width
 * in the array is returned.
 */
    ساكن صحيح
get_vts(صحيح *vts_array, صحيح اشاريو)
{
    صحيح	ts;

    إذا (اشاريو < 1)
	ts = 0;
    وإلا إذا (اشاريو <= vts_array[0])
	ts = vts_array[اشاريو];
    وإلا
	ts = vts_array[vts_array[0]];

    أرجع ts;
}

/*
 * Return the sum of all the tabstops through the index-th.
 */
    ساكن صحيح
get_vts_جوڙ(صحيح *vts_array, صحيح اشاريو)
{
    صحيح	جوڙ = 0;
    صحيح	i;

    // Perform the summation for indices within the actual array.
    لكل (i = 1; i <= اشاريو && i <= vts_array[0]; i++)
	جوڙ += vts_array[i];

    // Add tabstops whose indices exceed the actual array.
    إذا (i <= اشاريو)
	جوڙ += vts_array[vts_array[0]] * (اشاريو - vts_array[0]);

    أرجع جوڙ;
}
#جيڪڏهن_پورو

    ساكن vimlong_T
get_new_sw_indent(
    صحيح		کاٻو,		// TRUE if shift is to the left
    صحيح		ڦيرو,		// TRUE if new indent is to be to a tabstop
    vimlong_T	amount,		// Number of shifts
    vimlong_T	sw_val)
{
    vimlong_T	ڳڻپ = get_indent();
    vimlong_T	i, j;

    إذا (ڦيرو)			// round off indent
    {
	i = trim_منزل_int(ڳڻپ) / sw_val;	// number of 'shiftwidth' rounded down
	j = trim_منزل_int(ڳڻپ) % sw_val;	// extra spaces
	إذا (j && کاٻو)		// first remove extra spaces
	    --amount;
	إذا (کاٻو)
	{
	    i -= amount;
	    إذا (i < 0)
		i = 0;
	}
	وإلا
	    i += amount;
	ڳڻپ = i * sw_val;
    }
    وإلا			// original vi indent
    {
	إذا (کاٻو)
	{
	    ڳڻپ -= sw_val * amount;
	    إذا (ڳڻپ < 0)
		ڳڻپ = 0;
	}
	وإلا
	    ڳڻپ += sw_val * amount;
    }

    أرجع ڳڻپ;
}

#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
    ساكن vimlong_T
get_new_vts_indent(
    صحيح	کاٻو,			// TRUE if shift is to the left
    صحيح	ڦيرو,			// TRUE if new indent is to be to a tabstop
    صحيح	amount,			// Number of shifts
    صحيح	*vts_array)
{
    vimlong_T	indent = get_indent();
    صحيح		vtsi = 0;
    صحيح		vts_indent = 0;
    صحيح		ts = 0;		// Silence uninitialized variable warning.
    صحيح		وٿي;		// Extra indent spaces to the right of the
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

    وٿي = indent - vts_indent;

    إذا (ڦيرو)
    {
	إذا (کاٻو)
	{
	    إذا (وٿي == 0)
		indent = get_vts_جوڙ(vts_array, vtsi - amount);
	    وإلا
		indent = get_vts_جوڙ(vts_array, vtsi - (amount - 1));
	}
	وإلا
	    indent = get_vts_جوڙ(vts_array, vtsi + amount);
    }
    وإلا
    {
	إذا (کاٻو)
	{
	    إذا (amount > vtsi)
		indent = 0;
	    وإلا
		indent = get_vts_جوڙ(vts_array, vtsi - amount) + وٿي;
	}
	وإلا
	    indent = get_vts_جوڙ(vts_array, vtsi + amount) + وٿي;
    }

    أرجع indent;
}
#جيڪڏهن_پورو

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
سرڪايو_سٽ(
    صحيح	کاٻو,			// TRUE if shift is to the left
    صحيح	ڦيرو,			// TRUE if new indent is to be to a tabstop
    صحيح	amount,			// Number of shifts
    صحيح	call_بدليو_بائيٽون)	// call changed_bytes()
{
    vimlong_T	ڳڻپ;
    ڊگهو	sw_val = curbuf->b_p_sw;
    ڊگهو	ts_val = curbuf->b_p_ts;
#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
    صحيح		*vts_array = curbuf->b_p_vts_array;
#جيڪڏهن_پورو

    إذا (sw_val != 0)
	// 'shiftwidth' is not zero; use it as the shift size.
	ڳڻپ = get_new_sw_indent(کاٻو, ڦيرو, amount, sw_val);
    وإلا
#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
	إذا ((vts_array == عدم) || (vts_array[0] == 0))
#جيڪڏهن_پورو
    {
	// 'shiftwidth is zero and 'vartabstop' is empty; use 'tabstop' as the
	// shift size.
	ڳڻپ = get_new_sw_indent(کاٻو, ڦيرو, amount, ts_val);
    }
#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
    وإلا
    {
	// 'shiftwidth is zero and 'vartabstop' is defined; use 'vartabstop'
	// to determine the new indent.
	ڳڻپ = get_new_vts_indent(کاٻو, ڦيرو, amount, vts_array);
    }
#جيڪڏهن_پورو

    // Set new indent
    إذا (State & VREPLACE_FLAG)
	change_indent(INDENT_سيٽ, trim_منزل_int(ڳڻپ), FALSE, NUL, call_بدليو_بائيٽون);
    وإلا
	(فراغ)سيٽ_indent(trim_منزل_int(ڳڻپ), call_بدليو_بائيٽون ? SIN_بدليو : 0);
}

/*
 * Shift one line of the current block one shiftwidth right or left.
 * Leaves cursor on first character in block.
 */
    ساكن فراغ
سرڪايو_بلاڪ(oparg_T *oap, صحيح amount)
{
    صحيح			کاٻو = (oap->op_type == OP_LSHIFT);
    صحيح			oldstate = State;
    صحيح			ڪل;
    char_u		*newp, *oldp;
    size_t		newlen, oldlen;
    صحيح			oldcol = curwin->w_cursor.کانو;
    صحيح			sw_val = (صحيح)get_sw_قدر_indent(curbuf, کاٻو);
    صحيح			ts_val = (صحيح)curbuf->b_p_ts;
    بنية بلاڪ_def	bd;
    صحيح			incr;
    colnr_T		ws_vcol;
    صحيح			added;
    size_t		new_سٽ_len;	// the length of the line after the
					// block shift
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
    صحيح			old_p_ri = p_ri;

    p_ri = 0;			// don't want revins in indent
#جيڪڏهن_پورو

    State = گهڻائي_وجهو;	// don't want MODE_REPLACE for State
    بلاڪ_prep(oap, &bd, curwin->w_cursor.lnum, TRUE);
    إذا (bd.is_short)
	أرجع;

    // total is number of screen columns to be inserted/removed
    ڪل = (صحيح)((بنانشان)amount * (بنانشان)sw_val);
    إذا ((ڪل / sw_val) != amount)
	أرجع; // multiplication overflow

    oldp = ml_get_curline();
    oldlen = ml_get_curline_len();

    إذا (!کاٻو)
    {
	صحيح		tabs = 0, spaces = 0;
	chartabsize_T	cts;

	/*
	 *  1. Get start vcol
	 *  2. Total ws vcols
	 *  3. Divvy into TABs & spp
	 *  4. Construct new string
	 */
	ڪل += bd.pre_whitesp; // all virtual WS up to & incl a split TAB
	ws_vcol = bd.شروعات_vcol - bd.pre_whitesp;
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
				   bd.شروعات_vcol, bd.textstart, bd.textstart);
	لكل ( ; VIM_ISWHITE(*cts.cts_ptr); )
	{
	    incr = lbr_chartabsize_adv(&cts);
	    ڪل += incr;
	    cts.cts_vcol += incr;
	}
	bd.textstart = cts.cts_ptr;
	bd.شروعات_vcol = cts.cts_vcol;
	clear_chartabsize_arg(&cts);

	// OK, now total=all the VWS reqd, and textstart points at the 1st
	// non-ws char in the block.
#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
	إذا (!curbuf->b_p_et)
	    tabstop_fromto(ws_vcol, ws_vcol + ڪل,
				ts_val, curbuf->b_p_vts_array, &tabs, &spaces);
	وإلا
	    spaces = ڪل;
#وإلا
	إذا (!curbuf->b_p_et)
	    tabs = ((ws_vcol % ts_val) + ڪل) / ts_val; // number of tabs
	إذا (tabs > 0)
	    spaces = ((ws_vcol % ts_val) + ڪل) % ts_val; // number of spp
	وإلا
	    spaces = ڪل;
#جيڪڏهن_پورو
	// if we're splitting a TAB, allow for it
	bd.textcol -= bd.pre_whitesp_c - (bd.startspaces != 0);

	new_سٽ_len = bd.textcol + tabs + spaces + (oldlen - (bd.textstart - oldp));
	newp = alloc(new_سٽ_len + 1);
	إذا (newp == عدم)
	    أرجع;
	mch_memmove(newp, oldp, (size_t)bd.textcol);
	newlen = bd.textcol;
	vim_memset(newp + newlen, TAB, (size_t)tabs);
	newlen += tabs;
	vim_memset(newp + newlen, ' ', (size_t)spaces);
	STRCPY(newp + newlen + spaces, bd.textstart);
    }
    وإلا //کاٻو
    {
	colnr_T	    destination_کانو;	// column to which text in block will
					// be shifted
	char_u	    *verbatim_copy_پڇاڙي;	// end of the part of the line which is
					// copied verbatim
	colnr_T	    verbatim_copy_ويڪر;// the (displayed) width of this part
					// of line
	size_t	    fill;		// nr of spaces that replace a TAB
	size_t	    بلاڪ_space_ويڪر;
	size_t	    سرڪايو_amount;
	char_u	    *non_white = bd.textstart;
	colnr_T	    non_white_کانو;
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
	non_white_کانو = bd.شروعات_vcol;

	init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum,
				   non_white_کانو, bd.textstart, non_white);
	طالما (VIM_ISWHITE(*cts.cts_ptr))
	{
	    incr = lbr_chartabsize_adv(&cts);
	    cts.cts_vcol += incr;
	}
	non_white_کانو = cts.cts_vcol;
	non_white = cts.cts_ptr;
	clear_chartabsize_arg(&cts);

	بلاڪ_space_ويڪر = non_white_کانو - oap->شروعات_vcol;
	// We will shift by "total" or "block_space_width", whichever is less.
	سرڪايو_amount = (بلاڪ_space_ويڪر < (size_t)ڪل
					 ? بلاڪ_space_ويڪر : (size_t)ڪل);

	// The column to which we will shift the text.
	destination_کانو = (colnr_T)(non_white_کانو - سرڪايو_amount);

	// Now let's find out how much of the beginning of the line we can
	// reuse without modification.
	verbatim_copy_پڇاڙي = bd.textstart;
	verbatim_copy_ويڪر = bd.شروعات_vcol;

	// If "bd.startspaces" is set, "bd.textstart" points to the character
	// preceding the block. We have to subtract its width to obtain its
	// column number.
	إذا (bd.startspaces)
	    verbatim_copy_ويڪر -= bd.شروعات_char_vcols;
	init_chartabsize_arg(&cts, curwin, 0, verbatim_copy_ويڪر,
					     bd.textstart, verbatim_copy_پڇاڙي);
	طالما (cts.cts_vcol < destination_کانو)
	{
	    incr = lbr_chartabsize(&cts);
	    إذا (cts.cts_vcol + incr > destination_کانو)
		اكسر;
	    cts.cts_vcol += incr;
	    MB_PTR_ADV(cts.cts_ptr);
	}
	verbatim_copy_ويڪر = cts.cts_vcol;
	verbatim_copy_پڇاڙي = cts.cts_ptr;
	clear_chartabsize_arg(&cts);

	// If "destination_col" is different from the width of the initial
	// part of the line that will be copied, it means we encountered a tab
	// character, which we will have to partly replace with spaces.
	fill = destination_کانو - verbatim_copy_ويڪر;

	// The replacement line will consist of:
	// - the beginning of the original line up to "verbatim_copy_end",
	// - "fill" number of spaces,
	// - the rest of the line, pointed to by non_white.
	fixedlen = verbatim_copy_پڇاڙي - oldp;
	new_سٽ_len = fixedlen + fill + (oldlen - (non_white - oldp));

	newp = alloc(new_سٽ_len + 1);
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
    إذا (new_سٽ_len >= oldlen)
	added = (صحيح)(new_سٽ_len - oldlen);
    وإلا
	added = 0 - (صحيح)(oldlen - new_سٽ_len);
    inserted_بائيٽون(curwin->w_cursor.lnum, bd.textcol, added);
    State = oldstate;
    curwin->w_cursor.کانو = oldcol;
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
    p_ri = old_p_ri;
#جيڪڏهن_پورو
}

/*
 * Insert string "s" (b_insert ? before : after) block :AKelly
 * Caller must prepare for undo.
 */
    ساكن فراغ
بلاڪ_وجهو(
    oparg_T		*oap,
    char_u		*s,
    size_t		slen,
    صحيح			b_وجهو,
    بنية بلاڪ_def	*bdp)
{
    صحيح		ts_val;
    صحيح		ڳڻپ = 0;	// extra spaces to replace a cut TAB
    صحيح		spaces = 0;	// non-zero if cutting a TAB
    colnr_T	وٿي;		// pointer along new line
    colnr_T	startcol;	// column where insert starts
    char_u	*newp, *oldp;	// new, old lines
    linenr_T	lnum;		// loop var
    صحيح		oldstate = State;

    State = گهڻائي_وجهو;	// don't want MODE_REPLACE for State

    لكل (lnum = oap->شروعات.lnum + 1; lnum <= oap->پڇاڙي.lnum; lnum++)
    {
	بلاڪ_prep(oap, bdp, lnum, TRUE);
	إذا (bdp->is_short && b_وجهو)
	    استمر;	// OP_INSERT, line ends before block start

	oldp = ml_get(lnum);

	إذا (b_وجهو)
	{
	    ts_val = bdp->شروعات_char_vcols;
	    spaces = bdp->startspaces;
	    إذا (spaces != 0)
		ڳڻپ = ts_val - 1; // we're cutting a TAB
	    وٿي = bdp->textcol;
	}
	وإلا // append
	{
	    ts_val = bdp->پڇاڙي_char_vcols;
	    إذا (!bdp->is_short) // spaces = padding after block
	    {
		spaces = (bdp->endspaces ? ts_val - bdp->endspaces : 0);
		إذا (spaces != 0)
		    ڳڻپ = ts_val - 1; // we're cutting a TAB
		وٿي = bdp->textcol + bdp->textlen - (spaces != 0);
	    }
	    وإلا // spaces = padding to block edge
	    {
		// if $ used, just append to EOL (ie spaces==0)
		إذا (!bdp->is_MAX)
		    spaces = (oap->پڇاڙي_vcol - bdp->پڇاڙي_vcol) + 1;
		ڳڻپ = spaces;
		وٿي = bdp->textcol + bdp->textlen;
	    }
	}

	إذا (has_mbyte && spaces > 0)
	    // avoid copying part of a multi-byte character
	    وٿي -= (*mb_سر_off)(oldp, oldp + وٿي);

	إذا (spaces < 0)  // can happen when the cursor was moved
	    spaces = 0;

	// Make sure the allocated size matches what is actually copied below.
	newp = alloc(ml_get_len(lnum) + spaces + slen
		    + (spaces > 0 && !bdp->is_short ? ts_val - spaces : 0)
								  + ڳڻپ + 1);
	إذا (newp == عدم)
	    استمر;

	// copy up to shifted part
	mch_memmove(newp, oldp, (size_t)وٿي);
	oldp += وٿي;

	// insert pre-padding
	vim_memset(newp + وٿي, ' ', (size_t)spaces);
	startcol = وٿي + spaces;

	// copy the new text
	mch_memmove(newp + startcol, s, slen);
	وٿي += (صحيح)slen;

	إذا (spaces > 0 && !bdp->is_short)
	{
	    إذا (*oldp == TAB)
	    {
		// insert post-padding
		vim_memset(newp + وٿي + spaces, ' ',
						    (size_t)(ts_val - spaces));
		// we're splitting a TAB, don't copy it
		oldp++;
		// We allowed for that TAB, remember this now
		ڳڻپ++;
	    }
	    وإلا
		// Not a TAB, no extra spaces
		ڳڻپ = spaces;
	}

	إذا (spaces > 0)
	    وٿي += ڳڻپ;
	STRCPY(newp + وٿي, oldp);

	ml_replace(lnum, newp, FALSE);

	إذا (b_وجهو)
	    // correct any text properties
	    inserted_بائيٽون(lnum, startcol, (صحيح)slen);

	إذا (lnum == oap->پڇاڙي.lnum)
	{
	    // Set "']" mark to the end of the block instead of the end of
	    // the insert in the first line.
	    curbuf->b_op_پڇاڙي.lnum = oap->پڇاڙي.lnum;
	    curbuf->b_op_پڇاڙي.کانو = وٿي;
	    إذا (curbuf->b_visual.vi_پڇاڙي.coladd)
	    {
		curbuf->b_visual.vi_پڇاڙي.کانو += curbuf->b_visual.vi_پڇاڙي.coladd;
		curbuf->b_visual.vi_پڇاڙي.coladd = 0;
	    }
	}
    } // for all lnum

    بدليو_سٽون(oap->شروعات.lnum + 1, 0, oap->پڇاڙي.lnum + 1, 0L);

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
    بنية بلاڪ_def	bd;
    linenr_T		old_lcount = curbuf->b_ml.ml_سٽ_ڳڻپ;
    صحيح			did_yank = FALSE;

    إذا (curbuf->b_ml.ml_flags & ML_خالي_جايون)	    // nothing to do
	أرجع OK;

    // Nothing to delete, return here.	Do prepare undo, for op_change().
    إذا (oap->خالي_جايون_2)
	أرجع u_save_cursor();

    إذا (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	أرجع FAIL;
    }

    إذا (VIsual_select && oap->is_VIsual)
	// use register given with CTRL_R, defaults to zero
	oap->regname = VIsual_select_reg;

#جيڪڏهن_وصف_ٿيل HAVE_CLIPMETHOD
    adjust_clip_reg(&oap->regname);
#جيڪڏهن_پورو

    إذا (has_mbyte)
	mb_adjust_opend(oap);

    /*
     * Imitate the strange Vi behaviour: If the delete spans more than one
     * line and motion_type == MCHAR and the result is a blank line, make the
     * delete linewise.  Don't do this for the change command or Visual mode.
     */
    إذا (       oap->motion_type == MCHAR
	    && !oap->is_VIsual
	    && !oap->بلاڪ_گهڻائي
	    && oap->سٽ_ڳڻپ > 1
	    && oap->motion_force == NUL
	    && (vim_strchr(p_cpo, CPO_لفظ) != عدم)
	    && oap->op_type == OP_DELETE)
    {
	ptr = ml_get(oap->پڇاڙي.lnum) + oap->پڇاڙي.کانو;
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
	    && oap->سٽ_ڳڻپ == 1
	    && oap->op_type == OP_DELETE
	    && *ml_get(oap->شروعات.lnum) == NUL)
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
#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD_PROVIDER
	inc_clip_provider();
#جيڪڏهن_پورو
	إذا (oap->regname != 0)
	{
	    // check for read-only register
	    إذا (!valid_yank_reg(oap->regname, TRUE))
	    {
		beep_flush();
#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD_PROVIDER
		dec_clip_provider();
#جيڪڏهن_پورو
		أرجع OK;
	    }
	    get_yank_رجسٽر(oap->regname, TRUE); // yank into specif'd reg.
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
	إذا (oap->motion_type == MLINE || oap->سٽ_ڳڻپ > 1
							   || oap->use_reg_one)
	{
	    سرڪايو_delete_registers();
	    إذا (op_yank(oap, TRUE, FALSE) == OK)
		did_yank = TRUE;
	}

	// Yank into small delete register when no named register specified
	// and the delete is within one line.
	إذا ((
#جيڪڏهن_وصف_ٿيل HAVE_CLIPMETHOD
	    ((clip_unnamed & CLIP_UNNAMED) && oap->regname == '*') ||
	    ((clip_unnamed & CLIP_UNNAMED_PLUS) && oap->regname == '+') ||
#جيڪڏهن_پورو
	    oap->regname == 0) && oap->motion_type != MLINE
						      && oap->سٽ_ڳڻپ == 1)
	{
	    oap->regname = '-';
	    get_yank_رجسٽر(oap->regname, TRUE);
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
	    صحيح نياپو_silent_save = نياپو_silent;

	    نياپو_silent = 0;	// must display the prompt
	    n = ask_yesno((char_u *)_("cannot yank; delete anyway"), TRUE);
	    نياپو_silent = نياپو_silent_save;
	    إذا (n != 'y')
	    {
		emsg(_(e_command_aborted));
#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD_PROVIDER
		dec_clip_provider();
#جيڪڏهن_پورو
		أرجع FAIL;
	    }
	}

#إذا defined(FEAT_EVAL)
	إذا (did_yank && has_textyankpost())
	    yank_do_autocmd(oap, get_y_current());
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#جيڪڏهن_پورو
    }

    /*
     * block mode delete
     */
    إذا (oap->بلاڪ_گهڻائي)
    {
	إذا (u_save((linenr_T)(oap->شروعات.lnum - 1),
			       (linenr_T)(oap->پڇاڙي.lnum + 1)) == FAIL)
	    أرجع FAIL;

	لكل (lnum = curwin->w_cursor.lnum; lnum <= oap->پڇاڙي.lnum; ++lnum)
	{
	    بلاڪ_prep(oap, &bd, lnum, TRUE);
	    إذا (bd.textlen == 0)	// nothing to delete
		استمر;

	    // Adjust cursor position for tab replaced by spaces and 'lbr'.
	    إذا (lnum == curwin->w_cursor.lnum)
	    {
		curwin->w_cursor.کانو = bd.textcol + bd.startspaces;
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

#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
	    إذا (curbuf->b_has_textprop && n != 0)
		adjust_prop_columns(lnum, bd.textcol, -n, 0);
#جيڪڏهن_پورو
	}

	check_cursor_کانو();
	بدليو_سٽون(curwin->w_cursor.lnum, curwin->w_cursor.کانو,
						       oap->پڇاڙي.lnum + 1, 0L);
	oap->سٽ_ڳڻپ = 0;	    // no lines deleted
    }
    وإلا إذا (oap->motion_type == MLINE)
    {
	إذا (oap->op_type == OP_CHANGE)
	{
	    // Delete the lines except the first one.  Temporarily move the
	    // cursor to the next line.  Save the current line number, if the
	    // last line is deleted it may be changed.
	    إذا (oap->سٽ_ڳڻپ > 1)
	    {
		lnum = curwin->w_cursor.lnum;
		++curwin->w_cursor.lnum;
		del_سٽون((ڊگهو)(oap->سٽ_ڳڻپ - 1), TRUE);
		curwin->w_cursor.lnum = lnum;
	    }
	    إذا (u_save_cursor() == FAIL)
		أرجع FAIL;
	    إذا (curbuf->b_p_ai)		    // don't delete indent
	    {
		beginline(BL_WHITE);	    // cursor on first non-white
		did_ai = TRUE;		    // delete the indent when ESC hit
		ai_کانو = curwin->w_cursor.کانو;
	    }
	    وإلا
		beginline(0);		    // cursor in column 0
	    truncate_سٽ(FALSE);   // delete the rest of the line,
				    // leaving cursor past last char in line
	    إذا (oap->سٽ_ڳڻپ > 1)
		u_clearline();	    // "U" command not possible after "2cc"
	}
	وإلا
	{
	    del_سٽون(oap->سٽ_ڳڻپ, TRUE);
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
	    إذا (gchar_pos(&oap->شروعات) == '\t')
	    {
		إذا (u_save_cursor() == FAIL)	// save first line for undo
		    أرجع FAIL;
		إذا (oap->سٽ_ڳڻپ == 1)
		    endcol = getviscol2(oap->پڇاڙي.کانو, oap->پڇاڙي.coladd);
		coladvance_force(getviscol2(oap->شروعات.کانو, oap->شروعات.coladd));
		oap->شروعات = curwin->w_cursor;
		إذا (oap->سٽ_ڳڻپ == 1)
		{
		    coladvance(endcol);
		    oap->پڇاڙي.کانو = curwin->w_cursor.کانو;
		    oap->پڇاڙي.coladd = curwin->w_cursor.coladd;
		    curwin->w_cursor = oap->شروعات;
		}
	    }

	    // Break a tab only when it's included in the area.
	    إذا (gchar_pos(&oap->پڇاڙي) == '\t'
				     && (صحيح)oap->پڇاڙي.coladd < oap->inclusive)
	    {
		// save last line for undo
		إذا (u_save((linenr_T)(oap->پڇاڙي.lnum - 1),
				       (linenr_T)(oap->پڇاڙي.lnum + 1)) == FAIL)
		    أرجع FAIL;
		curwin->w_cursor = oap->پڇاڙي;
		coladvance_force(getviscol2(oap->پڇاڙي.کانو, oap->پڇاڙي.coladd));
		oap->پڇاڙي = curwin->w_cursor;
		curwin->w_cursor = oap->شروعات;
	    }
	    إذا (has_mbyte)
		mb_adjust_opend(oap);
	}

	إذا (oap->سٽ_ڳڻپ == 1)	// delete characters within one line
	{
	    إذا (u_save_cursor() == FAIL)	// save line for undo
		أرجع FAIL;

	    // if 'cpoptions' contains '$', display '$' at end of change
	    إذا (       vim_strchr(p_cpo, CPO_DOLLAR) != عدم
		    && oap->op_type == OP_CHANGE
		    && oap->پڇاڙي.lnum == curwin->w_cursor.lnum
		    && !oap->is_VIsual)
		display_dollar(oap->پڇاڙي.کانو - !oap->inclusive);

	    n = oap->پڇاڙي.کانو - oap->شروعات.کانو + 1 - !oap->inclusive;

	    إذا (virtual_op)
	    {
		// fix up things for virtualedit-delete:
		// break the tabs which are going to get in our way
		صحيح		len = ml_get_curline_len();

		إذا (oap->پڇاڙي.coladd != 0
			&& (صحيح)oap->پڇاڙي.کانو >= len - 1
			&& !(oap->شروعات.coladd && (صحيح)oap->پڇاڙي.کانو >= len - 1))
		    n++;
		// Delete at least one char (e.g, when on a control char).
		إذا (n == 0 && oap->شروعات.coladd != oap->پڇاڙي.coladd)
		    n = 1;

		// When deleted a char in the line, reset coladd.
		إذا (gchar_cursor() != NUL)
		    curwin->w_cursor.coladd = 0;
	    }
	    (فراغ)del_بائيٽون((ڊگهو)n, !virtual_op,
			    oap->op_type == OP_DELETE && !oap->is_VIsual);
	}
	وإلا				// delete characters between lines
	{
	    pos_T   curpos;

	    // save deleted and changed lines for undo
	    إذا (u_save((linenr_T)(curwin->w_cursor.lnum - 1),
		 (linenr_T)(curwin->w_cursor.lnum + oap->سٽ_ڳڻپ)) == FAIL)
		أرجع FAIL;

	    truncate_سٽ(TRUE);	// delete from cursor to end of line

	    curpos = curwin->w_cursor;	// remember curwin->w_cursor
	    ++curwin->w_cursor.lnum;
	    del_سٽون((ڊگهو)(oap->سٽ_ڳڻپ - 2), FALSE);

	    // delete from start of line until op_end
	    n = (oap->پڇاڙي.کانو + 1 - !oap->inclusive);
	    curwin->w_cursor.کانو = 0;
	    (فراغ)del_بائيٽون((ڊگهو)n, !virtual_op,
			    oap->op_type == OP_DELETE && !oap->is_VIsual);
	    curwin->w_cursor = curpos;	// restore curwin->w_cursor
	    (فراغ)do_join(2, FALSE, FALSE, FALSE, FALSE);
	}
	إذا (oap->op_type == OP_DELETE)
	    auto_format(FALSE, TRUE);
    }

    msgmore(curbuf->b_ml.ml_سٽ_ڳڻپ - old_lcount);

setmarks:
    إذا ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	إذا (oap->بلاڪ_گهڻائي)
	{
	    curbuf->b_op_پڇاڙي.lnum = oap->پڇاڙي.lnum;
	    curbuf->b_op_پڇاڙي.کانو = oap->شروعات.کانو;
	}
	وإلا
	    curbuf->b_op_پڇاڙي = oap->شروعات;
	curbuf->b_op_شروعات = oap->شروعات;
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
    char_u	*سٽ;
    char_u	*ptr;

    إذا (!oap->inclusive)
	أرجع;

    سٽ = ml_get(oap->پڇاڙي.lnum);
    ptr = سٽ + oap->پڇاڙي.کانو;
    إذا (*ptr != NUL)
    {
	ptr -= (*mb_سر_off)(سٽ, ptr);
	ptr += (*mb_ptr2len)(ptr) - 1;
	oap->پڇاڙي.کانو = ptr - سٽ;
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

    State = گهڻائي_REPLACE;
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
    صحيح			انگ_chars;
    char_u		*newp, *oldp;
    size_t		newlen, oldlen;
    بنية بلاڪ_def	bd;
    char_u		*after_p = عدم;
    صحيح			had_ctrl_v_cr = FALSE;

    إذا ((curbuf->b_ml.ml_flags & ML_خالي_جايون ) || oap->خالي_جايون_2)
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

    إذا (u_save((linenr_T)(oap->شروعات.lnum - 1),
				       (linenr_T)(oap->پڇاڙي.lnum + 1)) == FAIL)
	أرجع FAIL;

    /*
     * block mode replace
     */
    إذا (oap->بلاڪ_گهڻائي)
    {
	bd.is_MAX = (curwin->w_curswant == MAXCOL);
	لكل ( ; curwin->w_cursor.lnum <= oap->پڇاڙي.lnum; ++curwin->w_cursor.lnum)
	{
	    curwin->w_cursor.کانو = 0;  // make sure cursor position is valid
	    بلاڪ_prep(oap, &bd, curwin->w_cursor.lnum, TRUE);
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
		getvpos(&vpos, oap->شروعات_vcol);
		bd.startspaces += vpos.coladd;
		n = bd.startspaces;
	    }
	    وإلا
		// allow for pre spaces
		n = (bd.startspaces ? bd.شروعات_char_vcols - 1 : 0);

	    // allow for post spp
	    n += (bd.endspaces
		    && !bd.is_oneChar
		    && bd.پڇاڙي_char_vcols > 0) ? bd.پڇاڙي_char_vcols - 1 : 0;
	    // Figure out how many characters to replace.
	    numc = oap->پڇاڙي_vcol - oap->شروعات_vcol + 1;
	    إذا (bd.is_short && (!virtual_op || bd.is_MAX))
		numc -= (oap->پڇاڙي_vcol - bd.پڇاڙي_vcol) + 1;

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
	    انگ_chars = numc;
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
		    طالما (--انگ_chars >= 0)
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
		appended_سٽون_mark(curwin->w_cursor.lnum, 1L);
		oap->پڇاڙي.lnum++;
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
	    oap->شروعات.کانو = 0;
	    curwin->w_cursor.کانو = 0;
	    oap->پڇاڙي.کانو = ml_get_len(oap->پڇاڙي.lnum);
	    إذا (oap->پڇاڙي.کانو)
		--oap->پڇاڙي.کانو;
	}
	وإلا إذا (!oap->inclusive)
	    dec(&(oap->پڇاڙي));

	طالما (LTOREQ_POS(curwin->w_cursor, oap->پڇاڙي))
	{
	    صحيح پورو = FALSE;

	    n = gchar_cursor();
	    إذا (n != NUL)
	    {
		صحيح new_byte_len = (*mb_char2len)(c);
		صحيح old_byte_len = mb_ptr2len(ml_get_cursor());

		إذا (new_byte_len > 1 || old_byte_len > 1)
		{
		    // This is slow, but it handles replacing a single-byte
		    // with a multi-byte and the other way around.
		    إذا (curwin->w_cursor.lnum == oap->پڇاڙي.lnum)
			oap->پڇاڙي.کانو += new_byte_len - old_byte_len;
		    replace_character(c);
		    پورو = TRUE;
		}
		وإلا
		{
		    إذا (n == TAB)
		    {
			صحيح پڇاڙي_vcol = 0;

			إذا (curwin->w_cursor.lnum == oap->پڇاڙي.lnum)
			{
			    // oap->end has to be recalculated when
			    // the tab breaks
			    پڇاڙي_vcol = getviscol2(oap->پڇاڙي.کانو,
							     oap->پڇاڙي.coladd);
			}
			coladvance_force(getviscol());
			إذا (curwin->w_cursor.lnum == oap->پڇاڙي.lnum)
			    getvpos(&oap->پڇاڙي, پڇاڙي_vcol);
		    }
		    // with "coladd" set may move to just after a TAB
		    إذا (gchar_cursor() != NUL)
		    {
			PBYTE(curwin->w_cursor, c);
			پورو = TRUE;
		    }
		}
	    }
	    إذا (!پورو && virtual_op && curwin->w_cursor.lnum == oap->پڇاڙي.lnum)
	    {
		صحيح virtcols = oap->پڇاڙي.coladd;

		إذا (curwin->w_cursor.lnum == oap->شروعات.lnum
			&& oap->شروعات.کانو == oap->پڇاڙي.کانو && oap->شروعات.coladd)
		    virtcols -= oap->شروعات.coladd;

		// oap->end has been trimmed so it's effectively inclusive;
		// as a result an extra +1 must be counted so we don't
		// trample the NUL byte.
		coladvance_force(getviscol2(oap->پڇاڙي.کانو, oap->پڇاڙي.coladd) + 1);
		curwin->w_cursor.کانو -= (virtcols + 1);
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

    curwin->w_cursor = oap->شروعات;
    check_cursor();
    بدليو_سٽون(oap->شروعات.lnum, oap->شروعات.کانو, oap->پڇاڙي.lnum + 1, 0L);

    إذا ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_شروعات = oap->شروعات;
	curbuf->b_op_پڇاڙي = oap->پڇاڙي;
    }

    أرجع OK;
}

ساكن صحيح swapchars(صحيح op_type, pos_T *pos, صحيح ڊيگهائي);

/*
 * Handle the (non-standard vi) tilde operator.  Also for "gu", "gU" and "g?".
 */
    ساكن فراغ
op_tilde(oparg_T *oap)
{
    pos_T		pos;
    بنية بلاڪ_def	bd;
    صحيح			did_change = FALSE;

    إذا (u_save((linenr_T)(oap->شروعات.lnum - 1),
				       (linenr_T)(oap->پڇاڙي.lnum + 1)) == FAIL)
	أرجع;

    pos = oap->شروعات;
    إذا (oap->بلاڪ_گهڻائي)		    // Visual block mode
    {
	لكل (; pos.lnum <= oap->پڇاڙي.lnum; ++pos.lnum)
	{
	    صحيح one_change;

	    بلاڪ_prep(oap, &bd, pos.lnum, FALSE);
	    pos.کانو = bd.textcol;
	    one_change = swapchars(oap->op_type, &pos, bd.textlen);
	    did_change |= one_change;

#جيڪڏهن_وصف_ٿيل FEAT_NETBEANS_INTG
	    إذا (netbeans_سرگرم() && one_change)
	    {
		char_u *ptr;

		netbeans_removed(curbuf, pos.lnum, bd.textcol,
							    (ڊگهو)bd.textlen);
		// get the line now, it may have been flushed
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, bd.textcol,
						&ptr[bd.textcol], bd.textlen);
	    }
#جيڪڏهن_پورو
	}
	إذا (did_change)
	    بدليو_سٽون(oap->شروعات.lnum, 0, oap->پڇاڙي.lnum + 1, 0L);
    }
    وإلا				    // not block mode
    {
	إذا (oap->motion_type == MLINE)
	{
	    oap->شروعات.کانو = 0;
	    pos.کانو = 0;
	    oap->پڇاڙي.کانو = ml_get_len(oap->پڇاڙي.lnum);
	    إذا (oap->پڇاڙي.کانو)
		--oap->پڇاڙي.کانو;
	}
	وإلا إذا (!oap->inclusive)
	    dec(&(oap->پڇاڙي));

	إذا (pos.lnum == oap->پڇاڙي.lnum)
	    did_change = swapchars(oap->op_type, &pos,
						  oap->پڇاڙي.کانو - pos.کانو + 1);
	وإلا
	    لكل (;;)
	    {
		did_change |= swapchars(oap->op_type, &pos,
			pos.lnum == oap->پڇاڙي.lnum ? oap->پڇاڙي.کانو + 1
						  : ml_get_pos_len(&pos));
		إذا (LTOREQ_POS(oap->پڇاڙي, pos) || inc(&pos) == -1)
		    اكسر;
	    }
	إذا (did_change)
	{
	    بدليو_سٽون(oap->شروعات.lnum, oap->شروعات.کانو, oap->پڇاڙي.lnum + 1,
									  0L);
#جيڪڏهن_وصف_ٿيل FEAT_NETBEANS_INTG
	    إذا (netbeans_سرگرم())
	    {
		char_u *ptr;
		صحيح ڳڻپ;

		pos = oap->شروعات;
		طالما (pos.lnum < oap->پڇاڙي.lnum)
		{
		    ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		    ڳڻپ = ml_get_buf_len(curbuf, pos.lnum) - pos.کانو;
		    netbeans_removed(curbuf, pos.lnum, pos.کانو, (ڊگهو)ڳڻپ);
		    // get the line again, it may have been flushed
		    ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		    netbeans_inserted(curbuf, pos.lnum, pos.کانو,
							&ptr[pos.کانو], ڳڻپ);
		    pos.کانو = 0;
		    pos.lnum++;
		}
		ڳڻپ = oap->پڇاڙي.کانو - pos.کانو + 1;
		netbeans_removed(curbuf, pos.lnum, pos.کانو, (ڊگهو)ڳڻپ);
		// get the line again, it may have been flushed
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, pos.کانو,
							&ptr[pos.کانو], ڳڻپ);
	    }
#جيڪڏهن_پورو
	}
    }

    إذا (!did_change && oap->is_VIsual)
	// No change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    إذا ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set '[ and '] marks.
	curbuf->b_op_شروعات = oap->شروعات;
	curbuf->b_op_پڇاڙي = oap->پڇاڙي;
    }

    إذا (oap->سٽ_ڳڻپ > p_report)
	smsg(NGETTEXT("%ld line changed", "%ld lines changed",
					    oap->سٽ_ڳڻپ), oap->سٽ_ڳڻپ);
}

/*
 * Invoke swapchar() on "length" bytes at position "pos".
 * "pos" is advanced to just after the changed characters.
 * "length" is rounded up to include the whole last multi-byte character.
 * Also works correctly when the number of bytes changes.
 * Returns TRUE if some character was changed.
 */
    ساكن صحيح
swapchars(صحيح op_type, pos_T *pos, صحيح ڊيگهائي)
{
    صحيح todo;
    صحيح	did_change = 0;

    لكل (todo = ڊيگهائي; todo > 0; --todo)
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
    إذا ((op_type == OP_مٿانهون || op_type == OP_NOP || op_type == OP_TILDE)
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
	وإلا إذا (op_type != OP_هيٺاهون)
	    nc = MB_TOUPPER(c);
    }
    وإلا إذا (MB_ISUPPER(c))
    {
	إذا (op_type == OP_ROT13)
	    nc = ROT13(c, 'A');
	وإلا إذا (op_type != OP_مٿانهون)
	    nc = MB_TOLOWER(c);
    }
    إذا (nc != c)
    {
	إذا (enc_utf8 && (c >= 0x80 || nc >= 0x80))
	{
	    pos_T   sp = curwin->w_cursor;

	    curwin->w_cursor = *pos;
	    // don't use del_char(), it also removes composing chars
	    del_بائيٽون(utf_ptr2len(ml_get_cursor()), FALSE, FALSE);
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
op_وجهو(oparg_T *oap, ڊگهو count1)
{
    ڊگهو		pre_textlen = 0;
    colnr_T		ind_pre_کانو = 0, ind_post_کانو;
    صحيح			ind_pre_vcol = 0, ind_post_vcol = 0;
    بنية بلاڪ_def	bd;
    صحيح			i;
    pos_T		t1;
    pos_T		شروعات_وجهو;

    // edit() changes this - record it for OP_APPEND
    bd.is_MAX = (curwin->w_curswant == MAXCOL);

    // vis block is still marked. Get rid of it now.
    curwin->w_cursor.lnum = oap->شروعات.lnum;
    update_screen(UPD_INVERTED);

    إذا (oap->بلاڪ_گهڻائي)
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
					   ? oap->پڇاڙي_vcol + 1 : getviscol());
	    إذا (oap->op_type == OP_APPEND)
		--curwin->w_cursor.کانو;
	    curwin->w_ve_flags = old_ve_flags;
	}
	// Get the info about the block before entering the text
	بلاڪ_prep(oap, &bd, oap->شروعات.lnum, TRUE);
	// Get indent information
	ind_pre_کانو = (colnr_T)getwhitecols_curline();
	ind_pre_vcol = get_indent();
	pre_textlen = ml_get_len(oap->شروعات.lnum) - bd.textcol;
	إذا (oap->op_type == OP_APPEND)
	    pre_textlen -= bd.textlen;
    }

    إذا (oap->op_type == OP_APPEND)
    {
	إذا (oap->بلاڪ_گهڻائي && curwin->w_cursor.coladd == 0)
	{
	    // Move the cursor to the character right of the block.
	    curwin->w_سيٽ_curswant = true;
	    طالما (*ml_get_cursor() != NUL
		    && (curwin->w_cursor.کانو < bd.textcol + bd.textlen))
		++curwin->w_cursor.کانو;
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
	    curwin->w_cursor = oap->پڇاڙي;
	    check_cursor_کانو();

	    // Works just like an 'i'nsert on the next character.
	    إذا (!LINEEMPTY(curwin->w_cursor.lnum)
		    && oap->شروعات_vcol != oap->پڇاڙي_vcol)
		inc_cursor();
	}
    }

    t1 = oap->شروعات;
    شروعات_وجهو = curwin->w_cursor;
    (فراغ)edit(NUL, FALSE, (linenr_T)count1);

    // When a tab was inserted, and the characters in front of the tab
    // have been converted to a tab as well, the column of the cursor
    // might have actually been reduced, so need to adjust here.
    إذا (t1.lnum == curbuf->b_op_شروعات_orig.lnum
	    && LT_POS(curbuf->b_op_شروعات_orig, t1))
	oap->شروعات = curbuf->b_op_شروعات_orig;

    // If user has moved off this line, we don't know what to do, so do
    // nothing.
    // Also don't repeat the insert when Insert mode ended with CTRL-C.
    إذا (curwin->w_cursor.lnum != oap->شروعات.lnum || got_int)
	أرجع;

    إذا (oap->بلاڪ_گهڻائي)
    {
	صحيح			ins_len;
	char_u			*firstline, *ins_متن;
	بنية بلاڪ_def	bd2;
	صحيح			did_indent = FALSE;
	size_t			len;
	size_t			جوڙيو;
	// offset when cursor was moved in insert mode
	صحيح			وٿي = 0;

	// If indent kicked in, the firstline might have changed
	// but only do that, if the indent actually increased.
	ind_post_کانو = (colnr_T)getwhitecols_curline();
	إذا (curbuf->b_op_شروعات.کانو > ind_pre_کانو && ind_post_کانو > ind_pre_کانو)
	{
	    bd.textcol += ind_post_کانو - ind_pre_کانو;
	    ind_post_vcol = get_indent();
	    bd.شروعات_vcol += ind_post_vcol - ind_pre_vcol;
	    did_indent = TRUE;
	}

	// The user may have moved the cursor before inserting something, try
	// to adjust the block for that.  But only do it, if the difference
	// does not come from indent kicking in.
	إذا (oap->شروعات.lnum == curbuf->b_op_شروعات_orig.lnum
						  && !bd.is_MAX && !did_indent)
	{
	    صحيح t = getviscol2(curbuf->b_op_شروعات_orig.کانو,
					       curbuf->b_op_شروعات_orig.coladd);

	    إذا (oap->op_type == OP_وجهو
		    && oap->شروعات.کانو + oap->شروعات.coladd
			    != curbuf->b_op_شروعات_orig.کانو
					      + curbuf->b_op_شروعات_orig.coladd)
	    {
		oap->شروعات.کانو = curbuf->b_op_شروعات_orig.کانو;
		pre_textlen -= t - oap->شروعات_vcol;
		oap->شروعات_vcol = t;
	    }
	    وإلا إذا (oap->op_type == OP_APPEND
		    && oap->شروعات.کانو + oap->شروعات.coladd
			    >= curbuf->b_op_شروعات_orig.کانو
					      + curbuf->b_op_شروعات_orig.coladd)
	    {
		oap->شروعات.کانو = curbuf->b_op_شروعات_orig.کانو;
		// reset pre_textlen to the value of OP_INSERT
		pre_textlen += bd.textlen;
		pre_textlen -= t - oap->شروعات_vcol;
		oap->شروعات_vcol = t;
		oap->op_type = OP_وجهو;
	    }
	}

	// Spaces and tabs in the indent may have changed to other spaces and
	// tabs.  Get the starting column again and correct the length.
	// Don't do this when "$" used, end-of-line will have changed.
	//
	// if indent was added and the inserted text was after the indent,
	// correct the selection for the new indent.
	إذا (did_indent && bd.textcol - ind_post_کانو > 0)
	{
	    oap->شروعات.کانو += ind_post_کانو - ind_pre_کانو;
	    oap->شروعات_vcol += ind_post_vcol - ind_pre_vcol;
	    oap->پڇاڙي.کانو += ind_post_کانو - ind_pre_کانو;
	    oap->پڇاڙي_vcol += ind_post_vcol - ind_pre_vcol;
	}
	بلاڪ_prep(oap, &bd2, oap->شروعات.lnum, TRUE);
	إذا (did_indent && bd.textcol - ind_post_کانو > 0)
	{
	    // undo for where "oap" is used below
	    oap->شروعات.کانو -= ind_post_کانو - ind_pre_کانو;
	    oap->شروعات_vcol -= ind_post_vcol - ind_pre_vcol;
	    oap->پڇاڙي.کانو -= ind_post_کانو - ind_pre_کانو;
	    oap->پڇاڙي_vcol -= ind_post_vcol - ind_pre_vcol;
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
	firstline = ml_get(oap->شروعات.lnum);
	len = ml_get_len(oap->شروعات.lnum);
	جوڙيو = bd.textcol;
	إذا (oap->op_type == OP_APPEND)
	{
	    جوڙيو += bd.textlen;
	    // account for pressing cursor in insert mode when '$' was used
	    إذا (bd.is_MAX
		&& (شروعات_وجهو.lnum == Insstart.lnum
					   && شروعات_وجهو.کانو > Insstart.کانو))
	    {
		وٿي = (شروعات_وجهو.کانو - Insstart.کانو);
		جوڙيو -= وٿي;
		إذا (oap->پڇاڙي_vcol > وٿي)
		    oap->پڇاڙي_vcol -= (وٿي + 1);
		وإلا
		    // moved outside of the visual block, what to do?
		    أرجع;
	    }
	}
	إذا (جوڙيو > len)
	    جوڙيو = len;  // short line, point to the NUL
	firstline += جوڙيو;
	len -= جوڙيو;
	إذا (pre_textlen >= 0 && (ins_len = (صحيح)len - pre_textlen - وٿي) > 0)
	{
	    ins_متن = vim_strnsave(firstline, ins_len);
	    إذا (ins_متن != عدم)
	    {
		// block handled here
		إذا (u_save(oap->شروعات.lnum,
					 (linenr_T)(oap->پڇاڙي.lnum + 1)) == OK)
		    بلاڪ_وجهو(oap, ins_متن, ins_len, (oap->op_type == OP_وجهو),
									 &bd);

		curwin->w_cursor.کانو = oap->شروعات.کانو;
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
    ڊگهو		pre_textlen = 0;
    ڊگهو		pre_indent = 0;
    char_u		*firstline;
    char_u		*ins_متن, *newp, *oldp;
    بنية بلاڪ_def	bd;

    l = oap->شروعات.کانو;
    إذا (oap->motion_type == MLINE)
    {
	l = 0;
	can_si = may_do_si();	// Like opening a new line, do smart indent
    }

    // First delete the text in the region.  In an empty buffer only need to
    // save for undo
    إذا (curbuf->b_ml.ml_flags & ML_خالي_جايون)
    {
	إذا (u_save_cursor() == FAIL)
	    أرجع FALSE;
    }
    وإلا إذا (op_delete(oap) == FAIL)
	أرجع FALSE;

    إذا ((l > curwin->w_cursor.کانو) && !LINEEMPTY(curwin->w_cursor.lnum)
							 && !virtual_op)
	inc_cursor();

    // check for still on same line (<CR> in inserted text meaningless)
    // skip blank lines too
    إذا (oap->بلاڪ_گهڻائي)
    {
	// Add spaces before getting the current line length.
	إذا (virtual_op && (curwin->w_cursor.coladd > 0
						    || gchar_cursor() == NUL))
	    coladvance_force(getviscol());
	firstline = ml_get(oap->شروعات.lnum);
	pre_textlen = ml_get_len(oap->شروعات.lnum);
	pre_indent = (ڊگهو)getwhitecols(firstline);
	bd.textcol = curwin->w_cursor.کانو;
    }

    إذا (oap->motion_type == MLINE)
	fix_indent();

    // Reset finish_op now, don't want it set inside edit().
    صحيح save_پڄاڻي_op = پڄاڻي_op;
    پڄاڻي_op = FALSE;

    retval = edit(NUL, FALSE, (linenr_T)1);

    پڄاڻي_op = save_پڄاڻي_op;

    /*
     * In Visual block mode, handle copying the new text to all lines of the
     * block.
     * Don't repeat the insert when Insert mode ended with CTRL-C.
     */
    إذا (oap->بلاڪ_گهڻائي && oap->شروعات.lnum != oap->پڇاڙي.lnum && !got_int)
    {
	صحيح	ins_len;

	// Auto-indenting may have changed the indent.  If the cursor was past
	// the indent, exclude that indent change from the inserted text.
	firstline = ml_get(oap->شروعات.lnum);
	إذا (bd.textcol > (colnr_T)pre_indent)
	{
	    ڊگهو new_indent = (ڊگهو)getwhitecols(firstline);

	    pre_textlen += new_indent - pre_indent;
	    bd.textcol += new_indent - pre_indent;
	}

	ins_len = (صحيح)ml_get_len(oap->شروعات.lnum) - pre_textlen;
	إذا (ins_len > 0)
	{
	    // Subsequent calls to ml_get() flush the firstline data - take a
	    // copy of the inserted text.
	    إذا ((ins_متن = alloc(ins_len + 1)) != عدم)
	    {
		vim_strncpy(ins_متن, firstline + bd.textcol, ins_len);
		لكل (linenr = oap->شروعات.lnum + 1; linenr <= oap->پڇاڙي.lnum;
								     linenr++)
		{
		    بلاڪ_prep(oap, &bd, linenr, TRUE);
		    إذا (!bd.is_short || virtual_op)
		    {
			pos_T vpos;
			size_t newlen;

			// If the block starts in virtual space, count the
			// initial coladd offset as part of "startspaces"
			إذا (bd.is_short)
			{
			    vpos.lnum = linenr;
			    (فراغ)getvpos(&vpos, oap->شروعات_vcol);
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
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
			// Shift the properties for linenr as edit() would do.
			إذا (curbuf->b_has_textprop)
			    adjust_prop_columns(linenr, bd.textcol,
						     vpos.coladd + (صحيح)ins_len, 0);
#جيڪڏهن_پورو
		    }
		}
		check_cursor();

		بدليو_سٽون(oap->شروعات.lnum + 1, 0, oap->پڇاڙي.lnum + 1, 0L);
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
    بنانشان صحيح cur_ve_flags = get_ve_flags();

    صحيح adj_cursor = (curwin->w_cursor.کانو > 0
				&& gchar_cursor() == NUL
				&& (cur_ve_flags & VE_ONEMORE) == 0
				&& (cur_ve_flags & VE_ALL) == 0
				&& !(restart_edit || (State & گهڻائي_وجهو)));
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
    char_u   *سٽ,
    صحيح      process,
    صحيح	     include_space,
    صحيح      *is_comment)
{
    char_u *comment_flags = عدم;
    صحيح    lead_len;
    صحيح    leader_وٿي = get_آخري_leader_وٿي(سٽ, &comment_flags);

    *is_comment = FALSE;
    إذا (leader_وٿي != -1)
    {
	// Let's check whether the line ends with an unclosed comment.
	// If the last comment leader has COM_END in flags, there's no comment.
	طالما (*comment_flags)
	{
	    إذا (*comment_flags == COM_پڇاڙي
		    || *comment_flags == ':')
		اكسر;
	    ++comment_flags;
	}
	إذا (*comment_flags != COM_پڇاڙي)
	    *is_comment = TRUE;
    }

    إذا (process == FALSE)
	أرجع سٽ;

    lead_len = get_leader_len(سٽ, &comment_flags, FALSE, include_space);

    إذا (lead_len == 0)
	أرجع سٽ;

    // Find:
    // - COM_END,
    // - colon,
    // whichever comes first.
    طالما (*comment_flags)
    {
	إذا (*comment_flags == COM_پڇاڙي
		|| *comment_flags == ':')
	    اكسر;
	++comment_flags;
    }

    // If we found a colon, it means that we are not processing a line
    // starting with a closing part of a three-part comment. That's good,
    // because we don't want to remove those as this would be annoying.
    إذا (*comment_flags == ':' || *comment_flags == NUL)
	سٽ += lead_len;

    أرجع سٽ;
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
    ڊگهو    ڳڻپ,
    صحيح	    وجهو_space,
    صحيح	    save_undo,
    صحيح	    use_formatoptions اڻ_استعمال_ٿيل,
    صحيح	    setmark)
{
    char_u	*curr = عدم;
    char_u      *curr_شروعات = عدم;
    char_u	*cend;
    char_u	*newp;
    size_t	newp_len;
    char_u	*spaces;	// number of spaces inserted before a line
    صحيح		endcurr1 = NUL;
    صحيح		endcurr2 = NUL;
    صحيح		currsize = 0;	// size of the current line
    صحيح		sumsize = 0;	// size of the long new line
    linenr_T	t;
    colnr_T	کانو = 0;
    صحيح		ret = OK;
    صحيح		*comments = عدم;
    صحيح		remove_comments = (use_formatoptions == TRUE)
				  && has_format_option(FO_REMOVE_COMS);
    صحيح		prev_was_comment;

    إذا (save_undo && u_save((linenr_T)(curwin->w_cursor.lnum - 1),
			    (linenr_T)(curwin->w_cursor.lnum + ڳڻپ)) == FAIL)
	أرجع FAIL;

    // Allocate an array to store the number of spaces inserted before each
    // line.  We will use it to pre-compute the length of the new line and the
    // proper placement of each original line in the new one.
    spaces = lalloc_clear(ڳڻپ, TRUE);
    إذا (spaces == عدم)
	أرجع FAIL;
    إذا (remove_comments)
    {
	comments = lalloc_clear(ڳڻپ * حجم(صحيح), TRUE);
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
    لكل (t = 0; t < ڳڻپ; ++t)
    {
	curr = curr_شروعات = ml_get((linenr_T)(curwin->w_cursor.lnum + t));
	إذا (t == 0 && setmark && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set the '[ mark.
	    curwin->w_بفر->b_op_شروعات.lnum = curwin->w_cursor.lnum;
	    curwin->w_بفر->b_op_شروعات.کانو  = (colnr_T)STRLEN(curr);
	}
	إذا (remove_comments)
	{
	    // We don't want to remove the comment leader if the
	    // previous line is not a comment.
	    إذا (t > 0 && prev_was_comment)
	    {

		char_u *new_curr = skip_comment(curr, TRUE, وجهو_space,
							   &prev_was_comment);
		comments[t] = (صحيح)(new_curr - curr);
		curr = new_curr;
	    }
	    وإلا
		curr = skip_comment(curr, FALSE, وجهو_space,
							   &prev_was_comment);
	}

	إذا (وجهو_space && t > 0)
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
	إذا (وجهو_space && currsize > 0)
	{
	    إذا (has_mbyte)
	    {
		cend = curr + currsize;
		MB_PTR_پٺ(curr, cend);
		endcurr1 = (*mb_ptr2char)(cend);
		إذا (cend > curr)
		{
		    MB_PTR_پٺ(curr, cend);
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
	سٽ_breakcheck();
	إذا (got_int)
	{
	    ret = FAIL;
	    اذهب theend;
	}
    }

    // store the column position before last line
    کانو = sumsize - currsize - spaces[ڳڻپ - 1];

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
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    unpacked_memline_T um = um_open_at_no_props(
				curwin->w_بفر, curwin->w_cursor.lnum, 0);
    // um_open_at_no_props may have invalidated "curr".
    صحيح curr_off = (صحيح)(curr - curr_شروعات);

    curr = curr_شروعات = ml_get((linenr_T)(curwin->w_cursor.lnum + ڳڻپ - 1));
    curr += curr_off;
#جيڪڏهن_پورو

    لكل (t = ڳڻپ - 1; ; --t)
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
	spaces_removed = (curr - curr_شروعات) - spaces[t];

	mark_کانو_adjust(curwin->w_cursor.lnum + t, (colnr_T)0, -t,
			 (ڊگهو)(cend - newp - spaces_removed), spaces_removed);
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
	prepend_joined_props(&um, curwin->w_cursor.lnum + t,
		t == ڳڻپ - 1, (ڊگهو)(cend - newp), spaces_removed);
#جيڪڏهن_پورو
	إذا (t == 0)
	    اكسر;
	curr = curr_شروعات = ml_get((linenr_T)(curwin->w_cursor.lnum + t - 1));
	إذا (remove_comments)
	    curr += comments[t - 1];
	إذا (وجهو_space && t > 1)
	    curr = skipwhite(curr);
	currsize = (صحيح)STRLEN(curr);
    }

#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    إذا (um.buf != عدم)
    {
	um_سيٽ_متن(&um, newp);
	um_ابتو_props(&um);
	um_close(&um);
	newp = عدم;  // um_set_text took ownership
    }
    وإلا
#جيڪڏهن_پورو
    ml_replace_len(curwin->w_cursor.lnum, newp, (colnr_T)newp_len, TRUE, FALSE);

    إذا (setmark && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set the '] mark.
	curwin->w_بفر->b_op_پڇاڙي.lnum = curwin->w_cursor.lnum;
	curwin->w_بفر->b_op_پڇاڙي.کانو  = (colnr_T)sumsize;
    }

    // Only report the change in the first line here, del_lines() will report
    // the deleted line.
    بدليو_سٽون(curwin->w_cursor.lnum, currsize,
					       curwin->w_cursor.lnum + 1, 0L);
    /*
     * Delete following lines. To do this we move the cursor there
     * briefly, and then move it back. After del_lines() the cursor may
     * have moved up (last line deleted), so the current lnum is kept in t.
     */
    t = curwin->w_cursor.lnum;
    ++curwin->w_cursor.lnum;
    del_سٽون(ڳڻپ - 1, FALSE);
    curwin->w_cursor.lnum = t;

    /*
     * Set the cursor column:
     * Vi compatible: use the column of the first join
     * vim:	      use the column of the last join
     */
    curwin->w_cursor.کانو =
		    (vim_strchr(p_cpo, CPO_JOINCOL) != عدم ? currsize : کانو);
    check_cursor_کانو();

    curwin->w_cursor.coladd = 0;
    curwin->w_سيٽ_curswant = true;

theend:
    vim_free(spaces);
    إذا (remove_comments)
	vim_free(comments);
    أرجع ret;
}

#جيڪڏهن_وصف_ٿيل FEAT_LINEBREAK
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
#جيڪڏهن_پورو

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
بلاڪ_prep(
    oparg_T		*oap,
    بنية بلاڪ_def	*bdp,
    linenr_T		lnum,
    صحيح			is_del)
{
    صحيح		incr = 0;
    char_u	*pend;
    char_u	*pstart;
    char_u	*سٽ;
    char_u	*prev_pstart;
    char_u	*prev_pend;
    chartabsize_T cts;
#جيڪڏهن_وصف_ٿيل FEAT_LINEBREAK
		// Avoid a problem with unwanted linebreaks in block mode.
    صحيح		lbr_saved = reset_lbr();
#جيڪڏهن_پورو

    bdp->startspaces = 0;
    bdp->endspaces = 0;
    bdp->textlen = 0;
    bdp->شروعات_vcol = 0;
    bdp->پڇاڙي_vcol = 0;
    bdp->is_short = FALSE;
    bdp->is_oneChar = FALSE;
    bdp->pre_whitesp = 0;
    bdp->pre_whitesp_c = 0;
    bdp->پڇاڙي_char_vcols = 0;
    bdp->شروعات_char_vcols = 0;

    سٽ = ml_get(lnum);
    prev_pstart = سٽ;
    init_chartabsize_arg(&cts, curwin, lnum, bdp->شروعات_vcol, سٽ, سٽ);
    طالما (cts.cts_vcol < oap->شروعات_vcol && *cts.cts_ptr != NUL)
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
    bdp->شروعات_vcol = cts.cts_vcol;
    pstart = cts.cts_ptr;
    clear_chartabsize_arg(&cts);

    bdp->شروعات_char_vcols = incr;
    إذا (bdp->شروعات_vcol < oap->شروعات_vcol)	// line too short
    {
	bdp->پڇاڙي_vcol = bdp->شروعات_vcol;
	bdp->is_short = TRUE;
	إذا (!is_del || oap->op_type == OP_APPEND)
	    bdp->endspaces = oap->پڇاڙي_vcol - oap->شروعات_vcol + 1;
    }
    وإلا
    {
	// notice: this converts partly selected Multibyte characters to
	// spaces, too.
	bdp->startspaces = bdp->شروعات_vcol - oap->شروعات_vcol;
	إذا (is_del && bdp->startspaces)
	    bdp->startspaces = bdp->شروعات_char_vcols - bdp->startspaces;
	pend = pstart;
	bdp->پڇاڙي_vcol = bdp->شروعات_vcol;
	إذا (bdp->پڇاڙي_vcol > oap->پڇاڙي_vcol)	// it's all in one character
	{
	    bdp->is_oneChar = TRUE;
	    إذا (oap->op_type == OP_وجهو)
		bdp->endspaces = bdp->شروعات_char_vcols - bdp->startspaces;
	    وإلا إذا (oap->op_type == OP_APPEND)
	    {
		bdp->startspaces += oap->پڇاڙي_vcol - oap->شروعات_vcol + 1;
		bdp->endspaces = bdp->شروعات_char_vcols - bdp->startspaces;
	    }
	    وإلا
	    {
		bdp->startspaces = oap->پڇاڙي_vcol - oap->شروعات_vcol + 1;
		إذا (is_del && oap->op_type != OP_LSHIFT)
		{
		    // just putting the sum of those two into
		    // bdp->startspaces doesn't work for Visual replace,
		    // so we have to split the tab in two
		    bdp->startspaces = bdp->شروعات_char_vcols
					- (bdp->شروعات_vcol - oap->شروعات_vcol);
		    bdp->endspaces = bdp->پڇاڙي_vcol - oap->پڇاڙي_vcol - 1;
		}
	    }
	}
	وإلا
	{
	    init_chartabsize_arg(&cts, curwin, lnum, bdp->پڇاڙي_vcol,
								  سٽ, pend);
	    prev_pend = pend;
	    طالما (cts.cts_vcol <= oap->پڇاڙي_vcol && *cts.cts_ptr != NUL)
	    {
		// count a tab for what it's worth (if list mode not on)
		prev_pend = cts.cts_ptr;
		incr = lbr_chartabsize_adv(&cts);
		cts.cts_vcol += incr;
	    }
	    bdp->پڇاڙي_vcol = cts.cts_vcol;
	    pend = cts.cts_ptr;
	    clear_chartabsize_arg(&cts);

	    إذا (bdp->پڇاڙي_vcol <= oap->پڇاڙي_vcol
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
		    bdp->endspaces = oap->پڇاڙي_vcol - bdp->پڇاڙي_vcol
							     + oap->inclusive;
		وإلا
		    bdp->endspaces = 0; // replace doesn't add characters
	    }
	    وإلا إذا (bdp->پڇاڙي_vcol > oap->پڇاڙي_vcol)
	    {
		bdp->endspaces = bdp->پڇاڙي_vcol - oap->پڇاڙي_vcol - 1;
		إذا (!is_del && bdp->endspaces)
		{
		    bdp->endspaces = incr - bdp->endspaces;
		    إذا (pend != pstart)
			pend = prev_pend;
		}
	    }
	}
	bdp->پڇاڙي_char_vcols = incr;
	إذا (is_del && bdp->startspaces)
	    pstart = prev_pstart;
	bdp->textlen = (صحيح)(pend - pstart);
    }
    bdp->textcol = (colnr_T) (pstart - سٽ);
    bdp->textstart = pstart;
#جيڪڏهن_وصف_ٿيل FEAT_LINEBREAK
    restore_lbr(lbr_saved);
#جيڪڏهن_پورو
}

/*
 * Get block text from "start" to "end"
 */
    فراغ
charwise_بلاڪ_prep(
    pos_T		شروعات,
    pos_T		پڇاڙي,
    بنية بلاڪ_def	*bdp,
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
    bdp->شروعات_char_vcols = 0;

    إذا (lnum == شروعات.lnum)
    {
	startcol = شروعات.کانو;
	إذا (virtual_op)
	{
	    getvcol(curwin, &شروعات, &cs, عدم, &ce, 0);
	    إذا (ce != cs && شروعات.coladd > 0)
	    {
		// Part of a tab selected -- but don't
		// double-count it.
		bdp->شروعات_char_vcols = ce - cs + 1;
		bdp->startspaces = bdp->شروعات_char_vcols - شروعات.coladd;
		إذا (bdp->startspaces < 0)
		    bdp->startspaces = 0;
		startcol++;
	    }
	}
    }

    إذا (lnum == پڇاڙي.lnum)
    {
	endcol = پڇاڙي.کانو;
	إذا (virtual_op)
	{
	    getvcol(curwin, &پڇاڙي, &cs, عدم, &ce, 0);
	    إذا (p[endcol] == NUL || (cs + پڇاڙي.coladd < ce
			// Don't add space for double-wide
			// char; endcol will be on last byte
			// of multi-byte char.
			&& (*mb_سر_off)(p, p + endcol) == 0))
	    {
		إذا (شروعات.lnum == پڇاڙي.lnum && شروعات.کانو == پڇاڙي.کانو)
		{
		    // Special case: inside a single char
		    bdp->is_oneChar = TRUE;
		    bdp->startspaces = پڇاڙي.coladd - شروعات.coladd + inclusive;
		    endcol = startcol;
		}
		وإلا
		{
		    bdp->endspaces = پڇاڙي.coladd + inclusive;
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
    بنية بلاڪ_def	bd;
    صحيح			change_cnt = 0;
    linenr_T		amount = Prenum1;

    // do_addsub() might trigger re-evaluation of 'foldexpr' halfway, when the
    // buffer is not completely updated yet. Postpone updating folds until before
    // the call to changed_lines().
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
    disable_fold_update++;
#جيڪڏهن_پورو

    إذا (!VIsual_سرگرم)
    {
	pos = curwin->w_cursor;
	إذا (u_save_cursor() == FAIL)
	{
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	    disable_fold_update--;
#جيڪڏهن_پورو
	    أرجع;
	}
	change_cnt = do_addsub(oap->op_type, &pos, 0, amount);
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	disable_fold_update--;
#جيڪڏهن_پورو
	إذا (change_cnt)
	    بدليو_سٽون(pos.lnum, 0, pos.lnum + 1, 0L);
    }
    وإلا
    {
	صحيح	one_change;
	صحيح	ڊيگهائي;
	pos_T	startpos;

	إذا (u_save((linenr_T)(oap->شروعات.lnum - 1),
					(linenr_T)(oap->پڇاڙي.lnum + 1)) == FAIL)
	{
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	    disable_fold_update--;
#جيڪڏهن_پورو
	    أرجع;
	}

	pos = oap->شروعات;
	لكل (; pos.lnum <= oap->پڇاڙي.lnum; ++pos.lnum)
	{
	    إذا (oap->بلاڪ_گهڻائي)		    // Visual block mode
	    {
		بلاڪ_prep(oap, &bd, pos.lnum, FALSE);
		pos.کانو = bd.textcol;
		ڊيگهائي = bd.textlen;
	    }
	    وإلا إذا (oap->motion_type == MLINE)
	    {
		curwin->w_cursor.کانو = 0;
		pos.کانو = 0;
		ڊيگهائي = ml_get_len(pos.lnum);
	    }
	    وإلا // oap->motion_type == MCHAR
	    {
		إذا (pos.lnum == oap->شروعات.lnum && !oap->inclusive)
		    dec(&(oap->پڇاڙي));
		ڊيگهائي = ml_get_len(pos.lnum);
		pos.کانو = 0;
		إذا (pos.lnum == oap->شروعات.lnum)
		{
		    pos.کانو += oap->شروعات.کانو;
		    ڊيگهائي -= oap->شروعات.کانو;
		}
		إذا (pos.lnum == oap->پڇاڙي.lnum)
		{
		    ڊيگهائي = ml_get_len(oap->پڇاڙي.lnum);
		    إذا (oap->پڇاڙي.کانو >= ڊيگهائي)
			oap->پڇاڙي.کانو = ڊيگهائي - 1;
		    ڊيگهائي = oap->پڇاڙي.کانو - pos.کانو + 1;
		}
	    }
	    one_change = do_addsub(oap->op_type, &pos, ڊيگهائي, amount);
	    إذا (one_change)
	    {
		// Remember the start position of the first change.
		إذا (change_cnt == 0)
		    startpos = curbuf->b_op_شروعات;
		++change_cnt;
	    }

#جيڪڏهن_وصف_ٿيل FEAT_NETBEANS_INTG
	    إذا (netbeans_سرگرم() && one_change)
	    {
		char_u *ptr;

		netbeans_removed(curbuf, pos.lnum, pos.کانو, (ڊگهو)ڊيگهائي);
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, pos.کانو,
						&ptr[pos.کانو], ڊيگهائي);
	    }
#جيڪڏهن_پورو
	    إذا (g_cmd && one_change)
		amount += Prenum1;
	}

#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	disable_fold_update--;
#جيڪڏهن_پورو
	إذا (change_cnt)
	    بدليو_سٽون(oap->شروعات.lnum, 0, oap->پڇاڙي.lnum + 1, 0L);

	إذا (!change_cnt && oap->is_VIsual)
	    // No change: need to remove the Visual selection
	    redraw_curbuf_later(UPD_INVERTED);

	// Set '[ mark if something changed. Keep the last end
	// position from do_addsub().
	إذا (change_cnt > 0 && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	    curbuf->b_op_شروعات = startpos;

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
    صحيح		ڊيگهائي,
    linenr_T	Prenum1)
{
    صحيح		کانو;
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
    صحيح		ڪٽيو;
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
    کانو = pos->کانو;

    إذا (کانو + !!save_coladd >= linelen)
	اذهب theend;

    /*
     * First check if we are on a hexadecimal number, after the "0x".
     */
    إذا (!VIsual_سرگرم)
    {
	إذا (do_bin)
	    طالما (کانو > 0 && vim_isbdigit(ptr[کانو]))
	    {
		--کانو;
		إذا (has_mbyte)
		    کانو -= (*mb_سر_off)(ptr, ptr + کانو);
	    }

	إذا (do_hex)
	    طالما (کانو > 0 && vim_isxdigit(ptr[کانو]))
	    {
		--کانو;
		إذا (has_mbyte)
		    کانو -= (*mb_سر_off)(ptr, ptr + کانو);
	    }

	إذا (       do_bin
		&& do_hex
		&& ! ((کانو > 0
		    && (ptr[کانو] == 'X'
			|| ptr[کانو] == 'x')
		    && ptr[کانو - 1] == '0'
		    && (!has_mbyte ||
			!(*mb_سر_off)(ptr, ptr + کانو - 1))
		    && vim_isxdigit(ptr[کانو + 1]))))
	{

	    // In case of binary/hexadecimal pattern overlap match, rescan

	    کانو = pos->کانو;

	    طالما (کانو > 0 && vim_isdigit(ptr[کانو]))
	    {
		کانو--;
		إذا (has_mbyte)
		    کانو -= (*mb_سر_off)(ptr, ptr + کانو);
	    }
	}

	إذا ((       do_hex
		&& کانو > 0
		&& (ptr[کانو] == 'X'
		    || ptr[کانو] == 'x')
		&& ptr[کانو - 1] == '0'
		&& (!has_mbyte ||
		    !(*mb_سر_off)(ptr, ptr + کانو - 1))
		&& vim_isxdigit(ptr[کانو + 1])) ||
	    (       do_bin
		&& کانو > 0
		&& (ptr[کانو] == 'B'
		    || ptr[کانو] == 'b')
		&& ptr[کانو - 1] == '0'
		&& (!has_mbyte ||
		    !(*mb_سر_off)(ptr, ptr + کانو - 1))
		&& vim_isbdigit(ptr[کانو + 1])))
	{
	    // Found hexadecimal or binary number, move to its start.
	    --کانو;
	    إذا (has_mbyte)
		کانو -= (*mb_سر_off)(ptr, ptr + کانو);
	}
	وإلا
	{
	    /*
	     * Search forward and then backward to find the start of number.
	     */
	    کانو = pos->کانو;

	    طالما (ptr[کانو] != NUL
		    && !vim_isdigit(ptr[کانو])
		    && !(do_alpha && ASCII_ISALPHA(ptr[کانو])))
		کانو += mb_ptr2len(ptr + کانو);

	    طالما (کانو > 0
		    && vim_isdigit(ptr[کانو - 1])
		    && !(do_alpha && ASCII_ISALPHA(ptr[کانو])))
	    {
		--کانو;
		إذا (has_mbyte)
		    کانو -= (*mb_سر_off)(ptr, ptr + کانو);
	    }
	}
    }

    إذا (visual)
    {
	طالما (ptr[کانو] != NUL && ڊيگهائي > 0
		&& !vim_isdigit(ptr[کانو])
		&& !(do_alpha && ASCII_ISALPHA(ptr[کانو])))
	{
	    صحيح mb_len = mb_ptr2len(ptr + کانو);

	    کانو += mb_len;
	    ڊيگهائي -= mb_len;
	}

	إذا (ڊيگهائي == 0)
	    اذهب theend;

	إذا (کانو > pos->کانو && ptr[کانو - 1] == '-'
		&& (!has_mbyte || !(*mb_سر_off)(ptr, ptr + کانو - 1))
		&& !do_unsigned)
	{
	    إذا (do_blank && کانو >= 2 && !VIM_ISWHITE(ptr[کانو - 2]))
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
    firstdigit = ptr[کانو];
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
	curwin->w_cursor.کانو = کانو;
	إذا (!did_change)
	    startpos = curwin->w_cursor;
	did_change = TRUE;
	(فراغ)del_char(FALSE);
	ins_char(firstdigit);
	endpos = curwin->w_cursor;
	curwin->w_cursor.کانو = کانو;
    }
    وإلا
    {
	char_u	*buf1;
	صحيح	buf1len;
	char_u	buf2[NUMBUFLEN];
	صحيح	buf2len;
	pos_T	save_pos;
	صحيح	i;

	إذا (کانو > 0 && ptr[کانو - 1] == '-'
		&& (!has_mbyte ||
		    !(*mb_سر_off)(ptr, ptr + کانو - 1))
		&& !visual
		&& !do_unsigned)
	{
	    إذا (do_blank && کانو >= 2 && !VIM_ISWHITE(ptr[کانو - 2]))
		blank_unsigned = TRUE;
	    وإلا
	    {
		// negative number
		--کانو;
		negative = TRUE;
	    }
	}

	// get the number value (unsigned)
	إذا (visual && VIsual_گهڻائي != 'V')
	    maxlen = (curbuf->b_visual.vi_curswant == MAXCOL
		    ? linelen - کانو : ڊيگهائي);

	صحيح حد_کان_مٿي = FALSE;
	vim_str2nr(ptr + کانو, &pre, &ڊيگهائي,
		0 + (do_bin ? STR2NR_BIN : 0)
		    + (do_oct ? STR2NR_OCT : 0)
		    + (do_hex ? STR2NR_HEX : 0),
		عدم, &n, maxlen, FALSE, &حد_کان_مٿي);

	// ignore leading '-' for hex and octal and bin numbers
	إذا (pre && negative)
	{
	    ++کانو;
	    --ڊيگهائي;
	    negative = FALSE;
	}
	// add or subtract
	ڪٽيو = FALSE;
	إذا (op_type == OP_NR_SUB)
	    ڪٽيو ^= TRUE;
	إذا (negative)
	    ڪٽيو ^= TRUE;

	oldn = n;
	إذا (!حد_کان_مٿي)  // if number is too big don't add/subtract
	{
	    إذا (ڪٽيو)
		n -= (uvarnumber_T)Prenum1;
	    وإلا
		n += (uvarnumber_T)Prenum1;
	}

	// handle wraparound for decimal numbers
	إذا (!pre)
	{
	    إذا (ڪٽيو)
	    {
		إذا (n > oldn)
		{
		    n = 1 + (n ^ (uvarnumber_T)-1);
		    negative ^= TRUE;
		}
	    }
	    وإلا
	    {
		//جوڙيو
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
	    إذا (ڪٽيو)
		// sticking at zero.
		n = (uvarnumber_T)0;
	    وإلا
		// sticking at 2^64 - 1.
		n = (uvarnumber_T)(-1);
	    negative = FALSE;
	}

	إذا (visual && !was_مثبت && !negative && کانو > 0)
	{
	    // need to remove the '-'
	    کانو--;
	    ڊيگهائي++;
	}

	/*
	 * Delete the old number.
	 */
	curwin->w_cursor.کانو = کانو;
	إذا (!did_change)
	    startpos = curwin->w_cursor;
	did_change = TRUE;
	todel = ڊيگهائي;
	c = gchar_cursor();
	/*
	 * Don't include the '-' in the length, only the length of the
	 * part after it is kept the same.
	 */
	إذا (c == '-')
	    --ڊيگهائي;

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
	buf1 = alloc(ڊيگهائي + NUMBUFLEN);
	إذا (buf1 == عدم)
	    اذهب theend;
	ptr = buf1;
	إذا (negative && (!visual || was_مثبت))
	    *ptr++ = '-';
	إذا (pre)
	{
	    *ptr++ = '0';
	    --ڊيگهائي;
	}
	إذا (pre == 'b' || pre == 'B' ||
	    pre == 'x' || pre == 'X')
	{
	    *ptr++ = pre;
	    --ڊيگهائي;
	}

	/*
	 * Put the number characters in buf2[].
	 */
	إذا (pre == 'b' || pre == 'B')
	{
	    صحيح ٻٽو_انگ = 0;
	    صحيح بٽ = حجم(uvarnumber_T) * 8;

	    // leading zeros
	    لكل (ٻٽو_انگ = بٽ; ٻٽو_انگ > 0; ٻٽو_انگ--)
		إذا ((n >> (ٻٽو_انگ - 1)) & 0x1) اكسر;

	    لكل (buf2len = 0; ٻٽو_انگ > 0 && buf2len < (NUMBUFLEN - 1); ٻٽو_انگ--)
		buf2[buf2len++] = ((n >> (ٻٽو_انگ - 1)) & 0x1) ? '1' : '0';

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
	ڊيگهائي -= buf2len;

	/*
	 * Adjust number of zeros to the new number of digits, so the
	 * total length of the number remains the same.
	 * Don't do this when
	 * the result may look like an octal number.
	 */
	إذا (firstdigit == '0' && !(do_oct && pre == 0))
	    طالما (ڊيگهائي-- > 0)
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
	    صحيح بائيٽون_after = ml_get_curline_len() - curwin->w_cursor.کانو;

	    // Delete the one character before the insert.
	    curwin->w_cursor = save_pos;
	    (فراغ)del_char(FALSE);
	    curwin->w_cursor.کانو = ml_get_curline_len() - بائيٽون_after;
	    --todel;
	}
	طالما (todel-- > 0)
	    (فراغ)del_char(FALSE);

	endpos = curwin->w_cursor;
	إذا (did_change && curwin->w_cursor.کانو)
	    --curwin->w_cursor.کانو;
    }

    إذا (did_change && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// set the '[ and '] marks
	curbuf->b_op_شروعات = startpos;
	curbuf->b_op_پڇاڙي = endpos;
	إذا (curbuf->b_op_پڇاڙي.کانو > 0)
	    --curbuf->b_op_پڇاڙي.کانو;
    }

theend:
    إذا (visual)
	curwin->w_cursor = save_cursor;
    وإلا إذا (did_change)
	curwin->w_سيٽ_curswant = true;
    وإلا إذا (virtual_سرگرم())
	curwin->w_cursor.coladd = save_coladd;

    أرجع did_change;
}

    فراغ
clear_oparg(oparg_T *oap)
{
    CLEAR_اشارو(oap);
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
سٽ_ڳڻپ_info(
    char_u	*سٽ,
    varnumber_T	*wc,
    varnumber_T	*cc,
    varnumber_T	حد_بندي,
    صحيح		eol_سائيز)
{
    varnumber_T	i;
    varnumber_T	لفظ_3 = 0;
    varnumber_T	chars = 0;
    صحيح		is_لفظ = 0;

    لكل (i = 0; i < حد_بندي && سٽ[i] != NUL; )
    {
	إذا (is_لفظ)
	{
	    إذا (vim_isspace(سٽ[i]))
	    {
		لفظ_3++;
		is_لفظ = 0;
	    }
	}
	وإلا إذا (!vim_isspace(سٽ[i]))
	    is_لفظ = 1;
	++chars;
	i += (*mb_ptr2len)(سٽ + i);
    }

    إذا (is_لفظ)
	لفظ_3++;
    *wc += لفظ_3;

    // Add eol_size if the end of line was reached before hitting limit.
    إذا (i < حد_بندي && سٽ[i] == NUL)
    {
	i += eol_سائيز;
	chars += eol_سائيز;
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
    varnumber_T	byte_ڳڻپ = 0;
    varnumber_T	bom_ڳڻپ  = 0;
    varnumber_T	byte_ڳڻپ_cursor = 0;
    varnumber_T	char_ڳڻپ = 0;
    varnumber_T	char_ڳڻپ_cursor = 0;
    varnumber_T	لفظ_ڳڻپ = 0;
    varnumber_T	لفظ_ڳڻپ_cursor = 0;
    صحيح		eol_سائيز;
    varnumber_T	آخري_check = 100000L;
    ڊگهو	سٽ_ڳڻپ_selected = 0;
    pos_T	min_pos, max_pos;
    oparg_T	oparg;
    بنية بلاڪ_def	bd;

    /*
     * Compute the length of the file in characters.
     */
    إذا (curbuf->b_ml.ml_flags & ML_خالي_جايون)
    {
	إذا (dict == عدم)
	{
	    نياپو(_(no_سٽون_نياپو));
	    أرجع;
	}
    }
    وإلا
    {
	إذا (get_fileformat(curbuf) == EOL_DOS)
	    eol_سائيز = 2;
	وإلا
	    eol_سائيز = 1;

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
	    إذا (*p_sel == 'e' && max_pos.کانو > 0)
		--max_pos.کانو;

	    إذا (VIsual_گهڻائي == Ctrl_V)
	    {
#جيڪڏهن_وصف_ٿيل FEAT_LINEBREAK
		char_u * saved_sbr = p_sbr;
		char_u * saved_w_sbr = curwin->w_p_sbr;

		// Make 'sbr' empty for a moment to get the correct size.
		p_sbr = خالي_جايون_option;
		curwin->w_p_sbr = خالي_جايون_option;
#جيڪڏهن_پورو
		oparg.is_VIsual = 1;
		oparg.بلاڪ_گهڻائي = TRUE;
		oparg.op_type = OP_NOP;
		getvcols(curwin, &min_pos, &max_pos,
					&oparg.شروعات_vcol, &oparg.پڇاڙي_vcol, 0);
#جيڪڏهن_وصف_ٿيل FEAT_LINEBREAK
		p_sbr = saved_sbr;
		curwin->w_p_sbr = saved_w_sbr;
#جيڪڏهن_پورو
		إذا (curwin->w_curswant == MAXCOL)
		    oparg.پڇاڙي_vcol = MAXCOL;
		// Swap the start, end vcol if needed
		إذا (oparg.پڇاڙي_vcol < oparg.شروعات_vcol)
		{
		    oparg.پڇاڙي_vcol += oparg.شروعات_vcol;
		    oparg.شروعات_vcol = oparg.پڇاڙي_vcol - oparg.شروعات_vcol;
		    oparg.پڇاڙي_vcol -= oparg.شروعات_vcol;
		}
	    }
	    سٽ_ڳڻپ_selected = max_pos.lnum - min_pos.lnum + 1;
	}

	لكل (lnum = 1; lnum <= curbuf->b_ml.ml_سٽ_ڳڻپ; ++lnum)
	{
	    // Check for a CTRL-C every 100000 characters.
	    إذا (byte_ڳڻپ > آخري_check)
	    {
		ui_breakcheck();
		إذا (got_int)
		    أرجع;
		آخري_check = byte_ڳڻپ + 100000L;
	    }

	    // Do extra processing for VIsual mode.
	    إذا (VIsual_سرگرم
		    && lnum >= min_pos.lnum && lnum <= max_pos.lnum)
	    {
		char_u	    *s = عدم;
		ڊگهو	    len = 0L;

		بدل (VIsual_گهڻائي)
		{
		    حالة Ctrl_V:
			virtual_op = virtual_سرگرم();
			بلاڪ_prep(&oparg, &bd, lnum, 0);
			virtual_op = MAYBE;
			s = bd.textstart;
			len = (ڊگهو)bd.textlen;
			اكسر;
		    حالة 'V':
			s = ml_get(lnum);
			len = MAXCOL;
			اكسر;
		    حالة 'v':
			{
			    colnr_T شروعات_کانو = (lnum == min_pos.lnum)
							   ? min_pos.کانو : 0;
			    colnr_T پڇاڙي_کانو = (lnum == max_pos.lnum)
				      ? max_pos.کانو - شروعات_کانو + 1 : MAXCOL;

			    s = ml_get(lnum) + شروعات_کانو;
			    len = پڇاڙي_کانو;
			}
			اكسر;
		}
		إذا (s != عدم)
		{
		    byte_ڳڻپ_cursor += سٽ_ڳڻپ_info(s, &لفظ_ڳڻپ_cursor,
					   &char_ڳڻپ_cursor, len, eol_سائيز);
		    إذا (lnum == curbuf->b_ml.ml_سٽ_ڳڻپ
			    && !curbuf->b_p_eol
			    && (curbuf->b_p_bin || !curbuf->b_p_fixeol)
			    && (ڊگهو)STRLEN(s) < len)
			byte_ڳڻپ_cursor -= eol_سائيز;
		}
	    }
	    وإلا
	    {
		// In non-visual mode, check for the line the cursor is on
		إذا (lnum == curwin->w_cursor.lnum)
		{
		    لفظ_ڳڻپ_cursor += لفظ_ڳڻپ;
		    char_ڳڻپ_cursor += char_ڳڻپ;
		    byte_ڳڻپ_cursor = byte_ڳڻپ +
			سٽ_ڳڻپ_info(ml_get(lnum),
				&لفظ_ڳڻپ_cursor, &char_ڳڻپ_cursor,
				(varnumber_T)(curwin->w_cursor.کانو + 1),
				eol_سائيز);
		}
	    }
	    // Add to the running totals
	    byte_ڳڻپ += سٽ_ڳڻپ_info(ml_get(lnum), &لفظ_ڳڻپ,
					 &char_ڳڻپ, (varnumber_T)MAXCOL,
					 eol_سائيز);
	}

	// Correction for when last line doesn't have an EOL.
	إذا (!curbuf->b_p_eol && (curbuf->b_p_bin || !curbuf->b_p_fixeol))
	    byte_ڳڻپ -= eol_سائيز;

	إذا (dict == عدم)
	{
	    إذا (VIsual_سرگرم)
	    {
		إذا (VIsual_گهڻائي == Ctrl_V && curwin->w_curswant < MAXCOL)
		{
		    getvcols(curwin, &min_pos, &max_pos,
						&min_pos.کانو, &max_pos.کانو, 0);
		    vim_snprintf((حرف *)buf1, حجم(buf1), _("%ld Cols; "),
			    (ڊگهو)(oparg.پڇاڙي_vcol - oparg.شروعات_vcol + 1));
		}
		وإلا
		    buf1[0] = NUL;

		إذا (char_ڳڻپ_cursor == byte_ڳڻپ_cursor
						    && char_ڳڻپ == byte_ڳڻپ)
		    vim_snprintf((حرف *)IObuff, IOSIZE,
			    _("Selected %s%ld of %ld Lines; %lld of %lld Words; %lld of %lld Bytes"),
			    buf1, سٽ_ڳڻپ_selected,
			    (ڊگهو)curbuf->b_ml.ml_سٽ_ڳڻپ,
			    لفظ_ڳڻپ_cursor,
			    لفظ_ڳڻپ,
			    byte_ڳڻپ_cursor,
			    byte_ڳڻپ);
		وإلا
		    vim_snprintf((حرف *)IObuff, IOSIZE,
			    _("Selected %s%ld of %ld Lines; %lld of %lld Words; %lld of %lld Chars; %lld of %lld Bytes"),
			    buf1, سٽ_ڳڻپ_selected,
			    (ڊگهو)curbuf->b_ml.ml_سٽ_ڳڻپ,
			    لفظ_ڳڻپ_cursor,
			    لفظ_ڳڻپ,
			    char_ڳڻپ_cursor,
			    char_ڳڻپ,
			    byte_ڳڻپ_cursor,
			    byte_ڳڻپ);
	    }
	    وإلا
	    {
		p = ml_get_curline();
		validate_virtcol();
		کانو_print(buf1, حجم(buf1), (صحيح)curwin->w_cursor.کانو + 1,
			(صحيح)curwin->w_virtcol + 1);
		کانو_print(buf2, حجم(buf2), ml_get_curline_len(),
							   linetabsize_str(p));

		إذا (char_ڳڻپ_cursor == byte_ڳڻپ_cursor
			&& char_ڳڻپ == byte_ڳڻپ)
		    vim_snprintf((حرف *)IObuff, IOSIZE,
			_("Col %s of %s; Line %ld of %ld; Word %lld of %lld; Byte %lld of %lld"),
			(حرف *)buf1, (حرف *)buf2,
			(ڊگهو)curwin->w_cursor.lnum,
			(ڊگهو)curbuf->b_ml.ml_سٽ_ڳڻپ,
			لفظ_ڳڻپ_cursor, لفظ_ڳڻپ,
			byte_ڳڻپ_cursor, byte_ڳڻپ);
		وإلا
		    vim_snprintf((حرف *)IObuff, IOSIZE,
			_("Col %s of %s; Line %ld of %ld; Word %lld of %lld; Char %lld of %lld; Byte %lld of %lld"),
			(حرف *)buf1, (حرف *)buf2,
			(ڊگهو)curwin->w_cursor.lnum,
			(ڊگهو)curbuf->b_ml.ml_سٽ_ڳڻپ,
			لفظ_ڳڻپ_cursor, لفظ_ڳڻپ,
			char_ڳڻپ_cursor, char_ڳڻپ,
			byte_ڳڻپ_cursor, byte_ڳڻپ);
	    }
	}

	bom_ڳڻپ = bomb_سائيز();
	إذا (dict == عدم && bom_ڳڻپ > 0)
	{
	    size_t len = STRLEN(IObuff);

	    vim_snprintf((حرف *)IObuff + len, IOSIZE - len,
				 _("(+%lld for BOM)"), bom_ڳڻپ);
	}
	إذا (dict == عدم)
	{
	    // Don't shorten this message, the user asked for it.
	    p = p_shm;
	    p_shm = (char_u *)"";
	    نياپو((حرف *)IObuff);
	    p_shm = p;
	}
    }
#إذا defined(FEAT_EVAL)
    إذا (dict != عدم)
    {
	dict_جوڙيو_عدد(dict, "words", لفظ_ڳڻپ);
	dict_جوڙيو_عدد(dict, "chars", char_ڳڻپ);
	dict_جوڙيو_عدد(dict, "bytes", byte_ڳڻپ + bom_ڳڻپ);
	dict_جوڙيو_عدد(dict, VIsual_سرگرم ? "visual_bytes" : "cursor_bytes",
		byte_ڳڻپ_cursor);
	dict_جوڙيو_عدد(dict, VIsual_سرگرم ? "visual_chars" : "cursor_chars",
		char_ڳڻپ_cursor);
	dict_جوڙيو_عدد(dict, VIsual_سرگرم ? "visual_words" : "cursor_words",
		لفظ_ڳڻپ_cursor);
    }
#جيڪڏهن_پورو
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
	إذا (oap->شروعات.lnum == curwin->w_cursor.lnum)
	    stuffcharReadbuff('.');
	وإلا
	    stuffnumReadbuff((ڊگهو)oap->شروعات.lnum);

#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	// When using !! on a closed fold the range ".!" works best to operate
	// on, it will be made the whole closed fold later.
	linenr_T پڇاڙي_جو_شروعات_Fold = oap->شروعات.lnum;
	(فراغ)hasFolding(oap->شروعات.lnum, عدم, &پڇاڙي_جو_شروعات_Fold);
#جيڪڏهن_پورو
	إذا (oap->پڇاڙي.lnum != oap->شروعات.lnum
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
		&& oap->پڇاڙي.lnum != پڇاڙي_جو_شروعات_Fold
#جيڪڏهن_پورو
	   )
	{
	    // Make it a range with the end line.
	    stuffcharReadbuff(',');
	    إذا (oap->پڇاڙي.lnum == curwin->w_cursor.lnum)
		stuffcharReadbuff('.');
	    وإلا إذا (oap->پڇاڙي.lnum == curbuf->b_ml.ml_سٽ_ڳڻپ)
		stuffcharReadbuff('$');
	    وإلا إذا (oap->شروعات.lnum == curwin->w_cursor.lnum
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
		    // do not use ".+number" for a closed fold, it would count
		    // folded lines twice
		    && !hasFolding(oap->پڇاڙي.lnum, عدم, عدم)
#جيڪڏهن_پورو
		    )
	    {
		stuffReadbuff((char_u *)".+");
		stuffnumReadbuff((ڊگهو)oap->سٽ_ڳڻپ - 1);
	    }
	    وإلا
		stuffnumReadbuff((ڊگهو)oap->پڇاڙي.lnum);
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
did_سيٽ_operatorfunc(optset_T *دليلون اڻ_استعمال_ٿيل)
{
    إذا (option_سيٽ_callback_func(p_opfunc, &opfunc_cb) == FAIL)
	أرجع e_invalid_دليل;

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
#جيڪڏهن_پورو

#إذا defined(FEAT_EVAL)
/*
 * Mark the global 'operatorfunc' callback with "copyID" so that it is not
 * garbage collected.
 */
    صحيح
سيٽ_ref_اندريون_opfunc(صحيح copyID اڻ_استعمال_ٿيل)
{
    صحيح abort = FALSE;

    abort = سيٽ_ref_اندريون_callback(&opfunc_cb, copyID);

    أرجع abort;
}
#جيڪڏهن_پورو

/*
 * Handle the "g@" operator: call 'operatorfunc'.
 */
    ساكن فراغ
op_function(oparg_T *oap اڻ_استعمال_ٿيل)
{
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    typval_T	argv[2];
    pos_T	orig_شروعات = curbuf->b_op_شروعات;
    pos_T	orig_پڇاڙي = curbuf->b_op_پڇاڙي;
    typval_T	rettv;

    إذا (*p_opfunc == NUL)
	emsg(_(e_operatorfunc_is_خالي_جايون));
    وإلا
    {
	// Set '[ and '] marks to text to be operated on.
	curbuf->b_op_شروعات = oap->شروعات;
	curbuf->b_op_پڇاڙي = oap->پڇاڙي;
	إذا (oap->motion_type != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_پڇاڙي);

	argv[0].v_type = VAR_STRING;
	إذا (oap->بلاڪ_گهڻائي)
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
	صحيح save_پڄاڻي_op = پڄاڻي_op;
	پڄاڻي_op = FALSE;

	إذا (call_callback(&opfunc_cb, 0, &rettv, 1, argv) != FAIL)
	    clear_tv(&rettv);

	virtual_op = save_virtual_op;
	پڄاڻي_op = save_پڄاڻي_op;
	إذا (cmdmod.cmod_flags & CMOD_LOCKMARKS)
	{
	    curbuf->b_op_شروعات = orig_شروعات;
	    curbuf->b_op_پڇاڙي = orig_پڇاڙي;
	}
    }
#وإلا
    emsg(_(e_eval_feature_not_موجود));
#جيڪڏهن_پورو
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
    colnr_T	    شروعات, پڇاڙي;

    إذا (VIsual_گهڻائي != Ctrl_V
	    || (!initial && oap->پڇاڙي.کانو < curwin->w_ويڪر))
	أرجع;

    oap->بلاڪ_گهڻائي = TRUE;

    // prevent from moving onto a trail byte
    إذا (has_mbyte)
	mb_adjustpos(curwin->w_بفر, &oap->پڇاڙي);

    getvvcol(curwin, &(oap->شروعات), &oap->شروعات_vcol, عدم, &oap->پڇاڙي_vcol, 0);

    إذا (!redo_VIsual_busy)
    {
	getvvcol(curwin, &(oap->پڇاڙي), &شروعات, عدم, &پڇاڙي, 0);

	إذا (شروعات < oap->شروعات_vcol)
	    oap->شروعات_vcol = شروعات;
	إذا (پڇاڙي > oap->پڇاڙي_vcol)
	{
	    إذا (initial && *p_sel == 'e' && شروعات >= 1
				    && شروعات - 1 >= oap->پڇاڙي_vcol)
		oap->پڇاڙي_vcol = شروعات - 1;
	    وإلا
		oap->پڇاڙي_vcol = پڇاڙي;
	}
    }

    // if '$' was used, get oap->end_vcol from longest line
    إذا (curwin->w_curswant == MAXCOL)
    {
	curwin->w_cursor.کانو = MAXCOL;
	oap->پڇاڙي_vcol = 0;
	لكل (curwin->w_cursor.lnum = oap->شروعات.lnum;
		curwin->w_cursor.lnum <= oap->پڇاڙي.lnum;
					++curwin->w_cursor.lnum)
	{
	    getvvcol(curwin, &curwin->w_cursor, عدم, عدم, &پڇاڙي, 0);
	    إذا (پڇاڙي > oap->پڇاڙي_vcol)
		oap->پڇاڙي_vcol = پڇاڙي;
	}
    }
    وإلا إذا (redo_VIsual_busy)
	oap->پڇاڙي_vcol = oap->شروعات_vcol + redo_VIsual_vcol - 1;
    // Correct oap->end.col and oap->start.col to be the
    // upper-left and lower-right corner of the block area.
    //
    // (Actually, this does convert column positions into character
    // positions)
    curwin->w_cursor.lnum = oap->پڇاڙي.lnum;
    coladvance(oap->پڇاڙي_vcol);
    oap->پڇاڙي = curwin->w_cursor;

    curwin->w_cursor = oap->شروعات;
    coladvance(oap->شروعات_vcol);
    oap->شروعات = curwin->w_cursor;
}

// Information for redoing the previous Visual selection.
تعريفنوع بنية {
    صحيح		rv_گهڻائي;	// 'v', 'V', or Ctrl-V
    linenr_T	rv_سٽ_ڳڻپ;	// number of lines
    colnr_T	rv_vcol;	// number of cols or end column
    ڊگهو	rv_ڳڻپ;	// count for Visual operator
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
do_pending_operator(cmdarg_T *cap, صحيح old_کانو, صحيح gui_yank)
{
    oparg_T	*oap = cap->oap;
    pos_T	old_cursor;
    صحيح		خالي_جايون_region_error;
    صحيح		restart_edit_save;
#جيڪڏهن_وصف_ٿيل FEAT_LINEBREAK
    صحيح		lbr_saved = curwin->w_p_lbr;
#جيڪڏهن_پورو

    // The visual area is remembered for redo
    ساكن redo_VIsual_T   redo_VIsual = {NUL, 0, 0, 0,0};

    صحيح		    include_سٽ_break = FALSE;

#إذا defined(FEAT_CLIPBOARD)
    // Yank the visual area into the GUI selection register before we operate
    // on it and lose it forever.
    // Don't do it if a specific register was specified, so that ""x"*P works.
    // This could call do_pending_operator() recursively, but that's OK
    // because gui_yank will be TRUE for the nested call.
    إذا ((clip_تارو.موجود || clip_plus.موجود)
	    && oap->op_type != OP_NOP
	    && !gui_yank
	    && VIsual_سرگرم
	    && !redo_VIsual_busy
	    && oap->regname == 0)
	clip_auto_select();
#جيڪڏهن_پورو
    old_cursor = curwin->w_cursor;

    // If an operation is pending, handle it...
    إذا ((پڄاڻي_op || VIsual_سرگرم) && oap->op_type != OP_NOP)
    {
	// Yank can be redone when 'y' is in 'cpoptions', but not when yanking
	// for the clipboard.
	صحيح	redo_yank = vim_strchr(p_cpo, CPO_YANK) != عدم && !gui_yank;

#جيڪڏهن_وصف_ٿيل FEAT_LINEBREAK
	// Avoid a problem with unwanted linebreaks in block mode.
	(فراغ)reset_lbr();
#جيڪڏهن_پورو
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
		VIsual = oap->شروعات;
	    }
	    VIsual_گهڻائي = Ctrl_V;
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
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
		&& oap->op_type != OP_FOLD
		&& oap->op_type != OP_FOLDOPEN
		&& oap->op_type != OP_FOLDOPENREC
		&& oap->op_type != OP_FOLDCLOSE
		&& oap->op_type != OP_FOLDCLOSEREC
		&& oap->op_type != OP_FOLDDEL
		&& oap->op_type != OP_FOLDDELREC
#جيڪڏهن_پورو
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
	    oap->شروعات = curwin->w_cursor;
	    curwin->w_cursor.lnum += redo_VIsual.rv_سٽ_ڳڻپ - 1;
	    إذا (curwin->w_cursor.lnum > curbuf->b_ml.ml_سٽ_ڳڻپ)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_سٽ_ڳڻپ;
	    VIsual_گهڻائي = redo_VIsual.rv_گهڻائي;
	    إذا (redo_VIsual.rv_vcol == MAXCOL || VIsual_گهڻائي == 'v')
	    {
		إذا (VIsual_گهڻائي == 'v')
		{
		    إذا (redo_VIsual.rv_سٽ_ڳڻپ <= 1)
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
	    cap->count0 = redo_VIsual.rv_ڳڻپ;
	    إذا (redo_VIsual.rv_ڳڻپ != 0)
		cap->count1 = redo_VIsual.rv_ڳڻپ;
	    وإلا
		cap->count1 = 1;
	}
	وإلا إذا (VIsual_سرگرم)
	{
	    إذا (!gui_yank)
	    {
		// Save the current VIsual area for '< and '> marks, and "gv"
		curbuf->b_visual.vi_شروعات = VIsual;
		curbuf->b_visual.vi_پڇاڙي = curwin->w_cursor;
		curbuf->b_visual.vi_گهڻائي = VIsual_گهڻائي;
		restore_visual_گهڻائي();
		curbuf->b_visual.vi_curswant = curwin->w_curswant;
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
		curbuf->b_visual_گهڻائي_eval = VIsual_گهڻائي;
#جيڪڏهن_پورو
	    }

	    // In Select mode, a linewise selection is operated upon like a
	    // characterwise selection.
	    // Special case: gH<Del> deletes the last line.
	    إذا (VIsual_select && VIsual_گهڻائي == 'V'
					    && cap->oap->op_type != OP_DELETE)
	    {
		إذا (LT_POS(VIsual, curwin->w_cursor))
		{
		    VIsual.کانو = 0;
		    curwin->w_cursor.کانو = ml_get_len(curwin->w_cursor.lnum);
		}
		وإلا
		{
		    curwin->w_cursor.کانو = 0;
		    VIsual.کانو = ml_get_len(VIsual.lnum);
		}
		VIsual_گهڻائي = 'v';
	    }
	    // If 'selection' is "exclusive", backup one character for
	    // charwise selections.
	    وإلا إذا (VIsual_گهڻائي == 'v')
		include_سٽ_break = unadjust_for_sel();

	    oap->شروعات = VIsual;
	    إذا (VIsual_گهڻائي == 'V')
	    {
		oap->شروعات.کانو = 0;
		oap->شروعات.coladd = 0;
	    }
	}

	// Set oap->start to the first position of the operated text, oap->end
	// to the end of the operated text.  w_cursor is equal to oap->start.
	إذا (LT_POS(oap->شروعات, curwin->w_cursor))
	{
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	    // Include folded lines completely.
	    إذا (!VIsual_سرگرم)
	    {
		إذا (hasFolding(oap->شروعات.lnum, &oap->شروعات.lnum, عدم))
		    oap->شروعات.کانو = 0;
		إذا ((curwin->w_cursor.کانو > 0 || oap->inclusive
						  || oap->motion_type == MLINE)
			&& hasFolding(curwin->w_cursor.lnum, عدم,
						      &curwin->w_cursor.lnum))
		    curwin->w_cursor.کانو = ml_get_curline_len();
	    }
#جيڪڏهن_پورو
	    oap->پڇاڙي = curwin->w_cursor;
	    curwin->w_cursor = oap->شروعات;

	    // w_virtcol may have been updated; if the cursor goes back to its
	    // previous position w_virtcol becomes invalid and isn't updated
	    // automatically.
	    curwin->w_valid &= ~VALID_VIRTCOL;
	}
	وإلا
	{
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	    // Include folded lines completely.
	    إذا (!VIsual_سرگرم && oap->motion_type == MLINE)
	    {
		إذا (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum,
									عدم))
		    curwin->w_cursor.کانو = 0;
		إذا (hasFolding(oap->شروعات.lnum, عدم, &oap->شروعات.lnum))
		    oap->شروعات.کانو = ml_get_len(oap->شروعات.lnum);
	    }
#جيڪڏهن_پورو
	    oap->پڇاڙي = oap->شروعات;
	    oap->شروعات = curwin->w_cursor;
	}

	// Just in case lines were deleted that make the position invalid.
	check_pos(curwin->w_بفر, &oap->پڇاڙي);
	oap->سٽ_ڳڻپ = oap->پڇاڙي.lnum - oap->شروعات.lnum + 1;

	// Set "virtual_op" before resetting VIsual_active.
	virtual_op = virtual_سرگرم();

	إذا (VIsual_سرگرم || redo_VIsual_busy)
	{
	    get_op_vcol(oap, redo_VIsual.rv_vcol, TRUE);

	    إذا (!redo_VIsual_busy && !gui_yank)
	    {
		// Prepare to reselect and redo Visual: this is based on the
		// size of the Visual text
		resel_VIsual_گهڻائي = VIsual_گهڻائي;
		إذا (curwin->w_curswant == MAXCOL)
		    resel_VIsual_vcol = MAXCOL;
		وإلا
		{
		    إذا (VIsual_گهڻائي != Ctrl_V)
			getvvcol(curwin, &(oap->پڇاڙي),
						عدم, عدم, &oap->پڇاڙي_vcol, 0);
		    إذا (VIsual_گهڻائي == Ctrl_V || oap->سٽ_ڳڻپ <= 1)
		    {
			إذا (VIsual_گهڻائي != Ctrl_V)
			    getvvcol(curwin, &(oap->شروعات),
					      &oap->شروعات_vcol, عدم, عدم, 0);
			resel_VIsual_vcol = oap->پڇاڙي_vcol - oap->شروعات_vcol + 1;
		    }
		    وإلا
			resel_VIsual_vcol = oap->پڇاڙي_vcol;
		}
		resel_VIsual_سٽ_ڳڻپ = oap->سٽ_ڳڻپ;
	    }

	    // can't redo yank (unless 'y' is in 'cpoptions') and ":"
	    إذا ((redo_yank || oap->op_type != OP_YANK)
		    && oap->op_type != OP_COLON
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
		    && oap->op_type != OP_FOLD
		    && oap->op_type != OP_FOLDOPEN
		    && oap->op_type != OP_FOLDOPENREC
		    && oap->op_type != OP_FOLDCLOSE
		    && oap->op_type != OP_FOLDCLOSEREC
		    && oap->op_type != OP_FOLDDEL
		    && oap->op_type != OP_FOLDDELREC
#جيڪڏهن_پورو
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
		    redo_VIsual.rv_گهڻائي = resel_VIsual_گهڻائي;
		    redo_VIsual.rv_vcol = resel_VIsual_vcol;
		    redo_VIsual.rv_سٽ_ڳڻپ = resel_VIsual_سٽ_ڳڻپ;
		    redo_VIsual.rv_ڳڻپ = cap->count0;
		    redo_VIsual.rv_arg = cap->arg;
		}
	    }

	    // oap->inclusive defaults to TRUE.
	    // If oap->end is on a NUL (empty line) oap->inclusive becomes
	    // FALSE.  This makes "d}P" and "v}dP" work the same.
	    إذا (oap->motion_force == NUL || oap->motion_type == MLINE)
		oap->inclusive = TRUE;
	    إذا (VIsual_گهڻائي == 'V')
		oap->motion_type = MLINE;
	    وإلا
	    {
		oap->motion_type = MCHAR;
		إذا (VIsual_گهڻائي != Ctrl_V && *ml_get_pos(&(oap->پڇاڙي)) == NUL
			&& (include_سٽ_break || !virtual_op))
		{
		    oap->inclusive = FALSE;
		    // Try to include the newline, unless it's an operator
		    // that works on lines only.
		    إذا (*p_sel != 'o'
			    && !op_on_سٽون(oap->op_type)
			    && oap->پڇاڙي.lnum < curbuf->b_ml.ml_سٽ_ڳڻپ)
		    {
			++oap->پڇاڙي.lnum;
			oap->پڇاڙي.کانو = 0;
			oap->پڇاڙي.coladd = 0;
			++oap->سٽ_ڳڻپ;
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
#جيڪڏهن_وصف_ٿيل FEAT_LINEBREAK
		    // make sure redrawing is correct
		    restore_lbr(lbr_saved);
#جيڪڏهن_پورو
		    redraw_curbuf_later(UPD_INVERTED);
		}
	    }
	}

	// Include the trailing byte of a multi-byte char.
	إذا (has_mbyte && oap->inclusive)
	{
	    صحيح		l;

	    l = (*mb_ptr2len)(ml_get_pos(&oap->پڇاڙي));
	    إذا (l > 1)
		oap->پڇاڙي.کانو += l - 1;
	}
	curwin->w_سيٽ_curswant = true;

	// oap->empty is set when start and end are the same.  The inclusive
	// flag affects this too, unless yanking and the end is on a NUL.
	oap->خالي_جايون_2 = (oap->motion_type == MCHAR
		    && (!oap->inclusive
			|| (oap->op_type == OP_YANK
			    && gchar_pos(&oap->پڇاڙي) == NUL))
		    && EQUAL_POS(oap->شروعات, oap->پڇاڙي)
		    && !(virtual_op && oap->شروعات.coladd != oap->پڇاڙي.coladd));
	// For delete, change and yank, it's an error to operate on an
	// empty region, when 'E' included in 'cpoptions' (Vi compatible).
	خالي_جايون_region_error = (oap->خالي_جايون_2
				&& vim_strchr(p_cpo, CPO_EMPTYREGION) != عدم);

	// Force a redraw when operating on an empty Visual region, when
	// 'modifiable is off or creating a fold.
	إذا (oap->is_VIsual && (oap->خالي_جايون_2 || !curbuf->b_p_ma
#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
		    || oap->op_type == OP_FOLD
#جيڪڏهن_پورو
		    ))
	{
#جيڪڏهن_وصف_ٿيل FEAT_LINEBREAK
	    restore_lbr(lbr_saved);
#جيڪڏهن_پورو
	    redraw_curbuf_later(UPD_INVERTED);
	}

	// If the end of an operator is in column one while oap->motion_type
	// is MCHAR and oap->inclusive is FALSE, we put op_end after the last
	// character in the previous line. If op_start is on or before the
	// first non-blank in the line, the operator becomes linewise
	// (strange, but that's the way vi does it).
	إذا (	   oap->motion_type == MCHAR
		&& oap->inclusive == FALSE
		&& !(cap->retval & CA_NO_ADJ_OP_پڇاڙي)
		&& oap->پڇاڙي.کانو == 0
		&& (!oap->is_VIsual || *p_sel == 'o')
		&& !oap->بلاڪ_گهڻائي
		&& oap->سٽ_ڳڻپ > 1)
	{
	    oap->پڇاڙي_adjusted = TRUE;	    // remember that we did this
	    --oap->سٽ_ڳڻپ;
	    --oap->پڇاڙي.lnum;
	    إذا (inindent(0))
		oap->motion_type = MLINE;
	    وإلا
	    {
		oap->پڇاڙي.کانو = ml_get_len(oap->پڇاڙي.lnum);
		إذا (oap->پڇاڙي.کانو)
		{
		    --oap->پڇاڙي.کانو;
		    oap->inclusive = TRUE;
		}
	    }
	}
	وإلا
	    oap->پڇاڙي_adjusted = FALSE;

	بدل (oap->op_type)
	{
	حالة OP_LSHIFT:
	حالة OP_RSHIFT:
	    op_سرڪايو(oap, TRUE, oap->is_VIsual ? (صحيح)cap->count1 : 1);
	    auto_format(FALSE, TRUE);
	    اكسر;

	حالة OP_JOIN_NS:
	حالة OP_JOIN:
	    إذا (oap->سٽ_ڳڻپ < 2)
		oap->سٽ_ڳڻپ = 2;
	    إذا (curwin->w_cursor.lnum + oap->سٽ_ڳڻپ - 1 >
						   curbuf->b_ml.ml_سٽ_ڳڻپ)
		beep_flush();
	    وإلا
	    {
		(فراغ)do_join(oap->سٽ_ڳڻپ, oap->op_type == OP_JOIN,
							    TRUE, TRUE, TRUE);
		auto_format(FALSE, TRUE);
	    }
	    اكسر;

	حالة OP_DELETE:
	    VIsual_reselect = FALSE;	    // don't reselect now
	    إذا (خالي_جايون_region_error)
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
	    إذا (خالي_جايون_region_error)
	    {
		إذا (!gui_yank)
		{
		    vim_beep(BO_OPER);
		    CancelRedo();
		}
	    }
	    وإلا
	    {
#جيڪڏهن_وصف_ٿيل FEAT_LINEBREAK
		restore_lbr(lbr_saved);
#جيڪڏهن_پورو
		oap->excl_tr_ws = cap->cmdchar == 'z';
		(فراغ)op_yank(oap, FALSE, !gui_yank);
	    }
	    check_cursor_کانو();
	    اكسر;

	حالة OP_CHANGE:
	    VIsual_reselect = FALSE;	    // don't reselect now
	    إذا (خالي_جايون_region_error)
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
		إذا (p_im || !ڪنجي_Typed)
		    restart_edit_save = restart_edit;
		وإلا
		    restart_edit_save = 0;
		restart_edit = 0;
#جيڪڏهن_وصف_ٿيل FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#جيڪڏهن_پورو
		// trigger TextChangedI
		curbuf->b_آخري_changedtick_i = CHANGEDTICK(curbuf);

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
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
		    إذا (use_indentexpr_for_lisp())
			op_reindent(oap, get_اظهار_indent);
		    وإلا
#جيڪڏهن_پورو
			op_reindent(oap, get_lisp_indent);
		    اكسر;
		}
		op_reindent(oap,
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
			*curbuf->b_p_inde != NUL ? get_اظهار_indent :
#جيڪڏهن_پورو
			    get_c_indent);
		اكسر;
	    }

	    op_colon(oap);
	    اكسر;

	حالة OP_TILDE:
	حالة OP_مٿانهون:
	حالة OP_هيٺاهون:
	حالة OP_ROT13:
	    إذا (خالي_جايون_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    وإلا
		op_tilde(oap);
	    check_cursor_کانو();
	    اكسر;

	حالة OP_FORMAT:
#إذا defined(FEAT_EVAL)
	    إذا (*curbuf->b_p_fex != NUL)
		op_formatexpr(oap);	// use expression
	    وإلا
#جيڪڏهن_پورو
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

#جيڪڏهن_وصف_ٿيل FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#جيڪڏهن_پورو
		// call 'operatorfunc'
		op_function(oap);

		// Restore the info for redoing Visual mode, the function may
		// invoke another operator and unintentionally change it.
		redo_VIsual = save_redo_VIsual;
		اكسر;
	    }

	حالة OP_وجهو:
	حالة OP_APPEND:
	    VIsual_reselect = FALSE;	// don't reselect now
	    إذا (خالي_جايون_region_error)
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
#جيڪڏهن_وصف_ٿيل FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#جيڪڏهن_پورو
		// trigger TextChangedI
		curbuf->b_آخري_changedtick_i = CHANGEDTICK(curbuf);

		op_وجهو(oap, cap->count1);
#جيڪڏهن_وصف_ٿيل FEAT_LINEBREAK
		// Reset linebreak, so that formatting works correctly.
		(فراغ)reset_lbr();
#جيڪڏهن_پورو

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
	    إذا (خالي_جايون_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    وإلا
	    {
#جيڪڏهن_وصف_ٿيل FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#جيڪڏهن_پورو
		op_replace(oap, cap->nchar);
	    }
	    اكسر;

#جيڪڏهن_وصف_ٿيل FEAT_FOLDING
	حالة OP_FOLD:
	    VIsual_reselect = FALSE;	// don't reselect now
	    foldCreate(oap->شروعات.lnum, oap->پڇاڙي.lnum);
	    اكسر;

	حالة OP_FOLDOPEN:
	حالة OP_FOLDOPENREC:
	حالة OP_FOLDCLOSE:
	حالة OP_FOLDCLOSEREC:
	    VIsual_reselect = FALSE;	// don't reselect now
	    opFoldRange(oap->شروعات.lnum, oap->پڇاڙي.lnum,
		    oap->op_type == OP_FOLDOPEN
					    || oap->op_type == OP_FOLDOPENREC,
		    oap->op_type == OP_FOLDOPENREC
					  || oap->op_type == OP_FOLDCLOSEREC,
					  oap->is_VIsual);
	    اكسر;

	حالة OP_FOLDDEL:
	حالة OP_FOLDDELREC:
	    VIsual_reselect = FALSE;	// don't reselect now
	    deleteFold(oap->شروعات.lnum, oap->پڇاڙي.lnum,
			       oap->op_type == OP_FOLDDELREC, oap->is_VIsual);
	    اكسر;
#جيڪڏهن_پورو
	حالة OP_NR_جوڙيو:
	حالة OP_NR_SUB:
	    إذا (خالي_جايون_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    وإلا
	    {
		VIsual_سرگرم = TRUE;
#جيڪڏهن_وصف_ٿيل FEAT_LINEBREAK
		restore_lbr(lbr_saved);
#جيڪڏهن_پورو
		op_addsub(oap, cap->count1, redo_VIsual.rv_arg);
		VIsual_سرگرم = FALSE;
	    }
	    check_cursor_کانو();
	    اكسر;
	افتراضي:
	    clearopbeep(oap);
	}
	virtual_op = MAYBE;
	إذا (!gui_yank)
	{
	    // if 'sol' not set, go back to old column for some commands
	    إذا (!p_sol && oap->motion_type == MLINE && !oap->پڇاڙي_adjusted
		    && (oap->op_type == OP_LSHIFT || oap->op_type == OP_RSHIFT
						|| oap->op_type == OP_DELETE))
	    {
#جيڪڏهن_وصف_ٿيل FEAT_LINEBREAK
		(فراغ)reset_lbr();
#جيڪڏهن_پورو
		coladvance(curwin->w_curswant = old_کانو);
	    }
	}
	وإلا
	{
	    curwin->w_cursor = old_cursor;
	}
	oap->بلاڪ_گهڻائي = FALSE;
	clearop(oap);
	motion_force = NUL;
    }
#جيڪڏهن_وصف_ٿيل FEAT_LINEBREAK
    restore_lbr(lbr_saved);
#جيڪڏهن_پورو
}

// put byte 'c' at position 'lp', but
// verify, that the position to place
// is actually safe
    ساكن فراغ
pbyte(pos_T lp, صحيح c)
{
    char_u *p = ml_get_buf(curbuf, lp.lnum, TRUE);
    صحيح	len = curbuf->b_ml.ml_سٽ_len;

    // safety check
    إذا (lp.کانو >= len)
	lp.کانو = (len > 1 ? len - 2 : 0);
    *(p + lp.کانو) = c;
}
