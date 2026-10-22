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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

ਸਥਿਰ ਖਾਲੀ ਖਿਸਕਾਓ_ਖੰਡ(oparg_T *oap, ਪੂਰਨ amount);
ਸਥਿਰ ਖਾਲੀ	mb_adjust_opend(oparg_T *oap);
ਸਥਿਰ ਪੂਰਨ	do_addsub(ਪੂਰਨ op_type, pos_T *pos, ਪੂਰਨ ਲੰਮਾਈ, linenr_T Prenum1);
ਸਥਿਰ ਖਾਲੀ	pbyte(pos_T lp, ਪੂਰਨ c);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PBYTE(lp, c) pbyte(lp, c)


// Flags for third item in "opchars".
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OPF_ਸਤਰਾਂ  1	// operator always works on lines
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OPF_CHANGE 2	// operator changes text

/*
 * The names of operators.
 * IMPORTANT: Index must correspond with defines in vim.h!!!
 * The third field holds OPF_ flags.
 */
ਸਥਿਰ ਅਚਲ ਅੱਖਰ opchars[][3] =
{
    {NUL, NUL, 0},			// OP_NOP
    {'d', NUL, OPF_CHANGE},		// OP_DELETE
    {'y', NUL, 0},			// OP_YANK
    {'c', NUL, OPF_CHANGE},		// OP_CHANGE
    {'<', NUL, OPF_ਸਤਰਾਂ | OPF_CHANGE},	// OP_LSHIFT
    {'>', NUL, OPF_ਸਤਰਾਂ | OPF_CHANGE},	// OP_RSHIFT
    {'!', NUL, OPF_ਸਤਰਾਂ | OPF_CHANGE},	// OP_FILTER
    {'g', '~', OPF_CHANGE},		// OP_TILDE
    {'=', NUL, OPF_ਸਤਰਾਂ | OPF_CHANGE},	// OP_INDENT
    {'g', 'q', OPF_ਸਤਰਾਂ | OPF_CHANGE},	// OP_FORMAT
    {':', NUL, OPF_ਸਤਰਾਂ},		// OP_COLON
    {'g', 'U', OPF_CHANGE},		// OP_UPPER
    {'g', 'u', OPF_CHANGE},		// OP_LOWER
    {'J', NUL, OPF_ਸਤਰਾਂ | OPF_CHANGE},	// DO_JOIN
    {'g', 'J', OPF_ਸਤਰਾਂ | OPF_CHANGE},	// DO_JOIN_NS
    {'g', '?', OPF_CHANGE},		// OP_ROT13
    {'r', NUL, OPF_CHANGE},		// OP_REPLACE
    {'I', NUL, OPF_CHANGE},		// OP_INSERT
    {'A', NUL, OPF_CHANGE},		// OP_APPEND
    {'z', 'f', OPF_ਸਤਰਾਂ},		// OP_FOLD
    {'z', 'o', OPF_ਸਤਰਾਂ},		// OP_FOLDOPEN
    {'z', 'O', OPF_ਸਤਰਾਂ},		// OP_FOLDOPENREC
    {'z', 'c', OPF_ਸਤਰਾਂ},		// OP_FOLDCLOSE
    {'z', 'C', OPF_ਸਤਰਾਂ},		// OP_FOLDCLOSEREC
    {'z', 'd', OPF_ਸਤਰਾਂ},		// OP_FOLDDEL
    {'z', 'D', OPF_ਸਤਰਾਂ},		// OP_FOLDDELREC
    {'g', 'w', OPF_ਸਤਰਾਂ | OPF_CHANGE},	// OP_FORMAT2
    {'g', '@', OPF_CHANGE},		// OP_FUNCTION
    {Ctrl_A, NUL, OPF_CHANGE},		// OP_NR_ADD
    {Ctrl_X, NUL, OPF_CHANGE},		// OP_NR_SUB
};

/*
 * Translate a command name into an operator type.
 * Must only be called with a valid operator name!
 */
    ਪੂਰਨ
get_op_type(ਪੂਰਨ char1, ਪੂਰਨ char2)
{
    ਪੂਰਨ		i;

    ਜੇ (char1 == 'r')		// ignore second character
	ਵਾਪਸ OP_REPLACE;
    ਜੇ (char1 == '~')		// when tilde is an operator
	ਵਾਪਸ OP_TILDE;
    ਜੇ (char1 == 'g' && char2 == Ctrl_A)	//ਜੋੜੋ
	ਵਾਪਸ OP_NR_ਜੋੜੋ;
    ਜੇ (char1 == 'g' && char2 == Ctrl_X)	//ਘਟਾਓ
	ਵਾਪਸ OP_NR_SUB;
    ਜੇ (char1 == 'z' && char2 == 'y')	// OP_YANK
	ਵਾਪਸ OP_YANK;
    ਲਈ (i = 0; ; ++i)
    {
	ਜੇ (opchars[i][0] == char1 && opchars[i][1] == char2)
	    ਰੋਕੋ;
	ਜੇ (i == (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(opchars) - 1)
	{
	    ਅੰਦਰੂਨੀ_error("get_op_type()");
	    ਰੋਕੋ;
	}
    }
    ਵਾਪਸ i;
}

/*
 * Return TRUE if operator "op" always works on whole lines.
 */
    ਸਥਿਰ ਪੂਰਨ
op_on_ਸਤਰਾਂ(ਪੂਰਨ op)
{
    ਵਾਪਸ opchars[op][2] & OPF_ਸਤਰਾਂ;
}

#ਜੇ defined(FEAT_ਕੰਮ_CHANNEL)
/*
 * Return TRUE if operator "op" changes text.
 */
    ਪੂਰਨ
op_is_change(ਪੂਰਨ op)
{
    ਵਾਪਸ opchars[op][2] & OPF_CHANGE;
}
#ਜੇ_ਅੰਤ

/*
 * Get first operator command character.
 * Returns 'g' or 'z' if there is another command character.
 */
    ਪੂਰਨ
get_op_char(ਪੂਰਨ optype)
{
    ਵਾਪਸ opchars[optype][0];
}

/*
 * Get second operator command character.
 */
    ਪੂਰਨ
get_extra_op_char(ਪੂਰਨ optype)
{
    ਵਾਪਸ opchars[optype][1];
}

/*
 * op_shift - handle a shift operation
 */
    ਖਾਲੀ
op_ਖਿਸਕਾਓ(oparg_T *oap, ਪੂਰਨ curs_ਸਿਖਰ, ਪੂਰਨ amount)
{
    ਲੰਮਾ	    i;
    ਪੂਰਨ		    ਪਹਿਲਾ_char;
    ਪੂਰਨ		    ਖੰਡ_ਖਾਨਾ = 0;

    ਜੇ (u_save((linenr_T)(oap->ਸ਼ੁਰੂ.lnum - 1),
				       (linenr_T)(oap->ਸਿਰਾ.lnum + 1)) == FAIL)
	ਵਾਪਸ;

    ਜੇ (oap->ਖੰਡ_ਬਹੁਲਕ)
	ਖੰਡ_ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;

    ਲਈ (i = oap->ਸਤਰ_ਗਿਣਤੀ; --i >= 0; )
    {
	ਪਹਿਲਾ_char = *ml_get_curline();
	ਜੇ (ਪਹਿਲਾ_char == NUL)				// empty line
	    curwin->w_cursor.ਖਾਨਾ = 0;
	ਨਹੀਂਤਾਂ ਜੇ (oap->ਖੰਡ_ਬਹੁਲਕ)
	    ਖਿਸਕਾਓ_ਖੰਡ(oap, amount);
	ਨਹੀਂਤਾਂ
	    // Move the line right if it doesn't start with '#', 'smartindent'
	    // isn't set or 'cindent' isn't set or '#' isn't in 'cino'.
	    ਜੇ (ਪਹਿਲਾ_char != '#' || !preprocs_ਖੱਬਾ())
		ਖਿਸਕਾਓ_ਸਤਰ(oap->op_type == OP_LSHIFT, p_sr, amount, FALSE);
	++curwin->w_cursor.lnum;
    }

    ਬਦਲਿਆ_ਸਤਰਾਂ(oap->ਸ਼ੁਰੂ.lnum, 0, oap->ਸਿਰਾ.lnum + 1, 0L);
    ਜੇ (oap->ਖੰਡ_ਬਹੁਲਕ)
    {
	curwin->w_cursor.lnum = oap->ਸ਼ੁਰੂ.lnum;
	curwin->w_cursor.ਖਾਨਾ = ਖੰਡ_ਖਾਨਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (curs_ਸਿਖਰ)	    // put cursor on first line, for ">>"
    {
	curwin->w_cursor.lnum = oap->ਸ਼ੁਰੂ.lnum;
	beginline(BL_SOL | BL_FIX);   // shift_line() may have set cursor.col
    }
    ਨਹੀਂਤਾਂ
	--curwin->w_cursor.lnum;	// put cursor on last line, for ":>"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    // The cursor line is not in a closed fold
    foldOpenCursor();
#ਜੇ_ਅੰਤ


    ਜੇ (oap->ਸਤਰ_ਗਿਣਤੀ > p_report)
    {
	ਅੱਖਰ	    *op;
	ਅੱਖਰ	    *ਸੁਨੇਹਾ_ਸਤਰ_single;
	ਅੱਖਰ	    *ਸੁਨੇਹਾ_ਸਤਰ_plural;

	ਜੇ (oap->op_type == OP_RSHIFT)
	    op = ">";
	ਨਹੀਂਤਾਂ
	    op = "<";
	ਸੁਨੇਹਾ_ਸਤਰ_single = NGETTEXT("%ld line %sed %d time",
					     "%ld line %sed %d times", amount);
	ਸੁਨੇਹਾ_ਸਤਰ_plural = NGETTEXT("%ld lines %sed %d time",
					    "%ld lines %sed %d times", amount);
	vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE,
		NGETTEXT(ਸੁਨੇਹਾ_ਸਤਰ_single, ਸੁਨੇਹਾ_ਸਤਰ_plural, oap->ਸਤਰ_ਗਿਣਤੀ),
		oap->ਸਤਰ_ਗਿਣਤੀ, op, amount);
	ਸੁਨੇਹਾ_attr_keep((ਅੱਖਰ *)IObuff, 0, TRUE);
    }

    ਜੇ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_ਸ਼ੁਰੂ = oap->ਸ਼ੁਰੂ;
	curbuf->b_op_ਸਿਰਾ.lnum = oap->ਸਿਰਾ.lnum;
	curbuf->b_op_ਸਿਰਾ.ਖਾਨਾ = ml_get_len(oap->ਸਿਰਾ.lnum);
	ਜੇ (curbuf->b_op_ਸਿਰਾ.ਖਾਨਾ > 0)
	    --curbuf->b_op_ਸਿਰਾ.ਖਾਨਾ;
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
/*
 * Return the tabstop width at the index of the variable tabstop array.  If an
 * index greater than the length of the array is given, the last tabstop width
 * in the array is returned.
 */
    ਸਥਿਰ ਪੂਰਨ
get_vts(ਪੂਰਨ *vts_array, ਪੂਰਨ ਸੂਚਕਾਂਕ)
{
    ਪੂਰਨ	ts;

    ਜੇ (ਸੂਚਕਾਂਕ < 1)
	ts = 0;
    ਨਹੀਂਤਾਂ ਜੇ (ਸੂਚਕਾਂਕ <= vts_array[0])
	ts = vts_array[ਸੂਚਕਾਂਕ];
    ਨਹੀਂਤਾਂ
	ts = vts_array[vts_array[0]];

    ਵਾਪਸ ts;
}

/*
 * Return the sum of all the tabstops through the index-th.
 */
    ਸਥਿਰ ਪੂਰਨ
get_vts_ਜੋੜ(ਪੂਰਨ *vts_array, ਪੂਰਨ ਸੂਚਕਾਂਕ)
{
    ਪੂਰਨ	ਜੋੜ = 0;
    ਪੂਰਨ	i;

    // Perform the summation for indices within the actual array.
    ਲਈ (i = 1; i <= ਸੂਚਕਾਂਕ && i <= vts_array[0]; i++)
	ਜੋੜ += vts_array[i];

    // Add tabstops whose indices exceed the actual array.
    ਜੇ (i <= ਸੂਚਕਾਂਕ)
	ਜੋੜ += vts_array[vts_array[0]] * (ਸੂਚਕਾਂਕ - vts_array[0]);

    ਵਾਪਸ ਜੋੜ;
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ vimlong_T
get_new_sw_indent(
    ਪੂਰਨ		ਖੱਬਾ,		// TRUE if shift is to the left
    ਪੂਰਨ		ਗੇੜਾ,		// TRUE if new indent is to be to a tabstop
    vimlong_T	amount,		// Number of shifts
    vimlong_T	sw_val)
{
    vimlong_T	ਗਿਣਤੀ_2 = get_indent();
    vimlong_T	i, j;

    ਜੇ (ਗੇੜਾ)			// round off indent
    {
	i = trim_ਮੰਜ਼ਿਲ_int(ਗਿਣਤੀ_2) / sw_val;	// number of 'shiftwidth' rounded down
	j = trim_ਮੰਜ਼ਿਲ_int(ਗਿਣਤੀ_2) % sw_val;	// extra spaces
	ਜੇ (j && ਖੱਬਾ)		// first remove extra spaces
	    --amount;
	ਜੇ (ਖੱਬਾ)
	{
	    i -= amount;
	    ਜੇ (i < 0)
		i = 0;
	}
	ਨਹੀਂਤਾਂ
	    i += amount;
	ਗਿਣਤੀ_2 = i * sw_val;
    }
    ਨਹੀਂਤਾਂ			// original vi indent
    {
	ਜੇ (ਖੱਬਾ)
	{
	    ਗਿਣਤੀ_2 -= sw_val * amount;
	    ਜੇ (ਗਿਣਤੀ_2 < 0)
		ਗਿਣਤੀ_2 = 0;
	}
	ਨਹੀਂਤਾਂ
	    ਗਿਣਤੀ_2 += sw_val * amount;
    }

    ਵਾਪਸ ਗਿਣਤੀ_2;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
    ਸਥਿਰ vimlong_T
get_new_vts_indent(
    ਪੂਰਨ	ਖੱਬਾ,			// TRUE if shift is to the left
    ਪੂਰਨ	ਗੇੜਾ,			// TRUE if new indent is to be to a tabstop
    ਪੂਰਨ	amount,			// Number of shifts
    ਪੂਰਨ	*vts_array)
{
    vimlong_T	indent = get_indent();
    ਪੂਰਨ		vtsi = 0;
    ਪੂਰਨ		vts_indent = 0;
    ਪੂਰਨ		ts = 0;		// Silence uninitialized variable warning.
    ਪੂਰਨ		ਵਿੱਥ;		// Extra indent spaces to the right of the
				// tabstop

    // Find the tabstop at or to the left of the current indent.
    ਜਦੋਂ (vts_indent <= indent)
    {
	vtsi++;
	ts = get_vts(vts_array, vtsi);
	vts_indent += ts;
    }
    vts_indent -= ts;
    vtsi--;

    ਵਿੱਥ = indent - vts_indent;

    ਜੇ (ਗੇੜਾ)
    {
	ਜੇ (ਖੱਬਾ)
	{
	    ਜੇ (ਵਿੱਥ == 0)
		indent = get_vts_ਜੋੜ(vts_array, vtsi - amount);
	    ਨਹੀਂਤਾਂ
		indent = get_vts_ਜੋੜ(vts_array, vtsi - (amount - 1));
	}
	ਨਹੀਂਤਾਂ
	    indent = get_vts_ਜੋੜ(vts_array, vtsi + amount);
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (ਖੱਬਾ)
	{
	    ਜੇ (amount > vtsi)
		indent = 0;
	    ਨਹੀਂਤਾਂ
		indent = get_vts_ਜੋੜ(vts_array, vtsi - amount) + ਵਿੱਥ;
	}
	ਨਹੀਂਤਾਂ
	    indent = get_vts_ਜੋੜ(vts_array, vtsi + amount) + ਵਿੱਥ;
    }

    ਵਾਪਸ indent;
}
#ਜੇ_ਅੰਤ

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
    ਖਾਲੀ
ਖਿਸਕਾਓ_ਸਤਰ(
    ਪੂਰਨ	ਖੱਬਾ,			// TRUE if shift is to the left
    ਪੂਰਨ	ਗੇੜਾ,			// TRUE if new indent is to be to a tabstop
    ਪੂਰਨ	amount,			// Number of shifts
    ਪੂਰਨ	call_ਬਦਲਿਆ_ਬਾਈਟਾਂ)	// call changed_bytes()
{
    vimlong_T	ਗਿਣਤੀ_2;
    ਲੰਮਾ	sw_val = curbuf->b_p_sw;
    ਲੰਮਾ	ts_val = curbuf->b_p_ts;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
    ਪੂਰਨ		*vts_array = curbuf->b_p_vts_array;
#ਜੇ_ਅੰਤ

    ਜੇ (sw_val != 0)
	// 'shiftwidth' is not zero; use it as the shift size.
	ਗਿਣਤੀ_2 = get_new_sw_indent(ਖੱਬਾ, ਗੇੜਾ, amount, sw_val);
    ਨਹੀਂਤਾਂ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
	ਜੇ ((vts_array == ਨਿਰਾ) || (vts_array[0] == 0))
#ਜੇ_ਅੰਤ
    {
	// 'shiftwidth is zero and 'vartabstop' is empty; use 'tabstop' as the
	// shift size.
	ਗਿਣਤੀ_2 = get_new_sw_indent(ਖੱਬਾ, ਗੇੜਾ, amount, ts_val);
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
    ਨਹੀਂਤਾਂ
    {
	// 'shiftwidth is zero and 'vartabstop' is defined; use 'vartabstop'
	// to determine the new indent.
	ਗਿਣਤੀ_2 = get_new_vts_indent(ਖੱਬਾ, ਗੇੜਾ, amount, vts_array);
    }
#ਜੇ_ਅੰਤ

    // Set new indent
    ਜੇ (State & VREPLACE_FLAG)
	change_indent(INDENT_ਸੈੱਟ, trim_ਮੰਜ਼ਿਲ_int(ਗਿਣਤੀ_2), FALSE, NUL, call_ਬਦਲਿਆ_ਬਾਈਟਾਂ);
    ਨਹੀਂਤਾਂ
	(ਖਾਲੀ)ਸੈੱਟ_indent(trim_ਮੰਜ਼ਿਲ_int(ਗਿਣਤੀ_2), call_ਬਦਲਿਆ_ਬਾਈਟਾਂ ? SIN_ਬਦਲਿਆ : 0);
}

/*
 * Shift one line of the current block one shiftwidth right or left.
 * Leaves cursor on first character in block.
 */
    ਸਥਿਰ ਖਾਲੀ
ਖਿਸਕਾਓ_ਖੰਡ(oparg_T *oap, ਪੂਰਨ amount)
{
    ਪੂਰਨ			ਖੱਬਾ = (oap->op_type == OP_LSHIFT);
    ਪੂਰਨ			oldstate = State;
    ਪੂਰਨ			ਕੁੱਲ;
    char_u		*newp, *oldp;
    size_t		newlen, oldlen;
    ਪੂਰਨ			oldcol = curwin->w_cursor.ਖਾਨਾ;
    ਪੂਰਨ			sw_val = (ਪੂਰਨ)get_sw_ਮੁੱਲ_indent(curbuf, ਖੱਬਾ);
    ਪੂਰਨ			ts_val = (ਪੂਰਨ)curbuf->b_p_ts;
    ਬਣਤਰ ਖੰਡ_def	bd;
    ਪੂਰਨ			incr;
    colnr_T		ws_vcol;
    ਪੂਰਨ			added;
    size_t		new_ਸਤਰ_len;	// the length of the line after the
					// block shift
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    ਪੂਰਨ			old_p_ri = p_ri;

    p_ri = 0;			// don't want revins in indent
#ਜੇ_ਅੰਤ

    State = ਬਹੁਲਕ_ਪਾਓ;	// don't want MODE_REPLACE for State
    ਖੰਡ_prep(oap, &bd, curwin->w_cursor.lnum, TRUE);
    ਜੇ (bd.is_short)
	ਵਾਪਸ;

    // total is number of screen columns to be inserted/removed
    ਕੁੱਲ = (ਪੂਰਨ)((ਬਿਨਾਨਿਸ਼ਾਨੀ)amount * (ਬਿਨਾਨਿਸ਼ਾਨੀ)sw_val);
    ਜੇ ((ਕੁੱਲ / sw_val) != amount)
	ਵਾਪਸ; // multiplication overflow

    oldp = ml_get_curline();
    oldlen = ml_get_curline_len();

    ਜੇ (!ਖੱਬਾ)
    {
	ਪੂਰਨ		tabs = 0, spaces = 0;
	chartabsize_T	cts;

	/*
	 *  1. Get start vcol
	 *  2. Total ws vcols
	 *  3. Divvy into TABs & spp
	 *  4. Construct new string
	 */
	ਕੁੱਲ += bd.pre_whitesp; // all virtual WS up to & incl a split TAB
	ws_vcol = bd.ਸ਼ੁਰੂ_vcol - bd.pre_whitesp;
	ਜੇ (bd.startspaces)
	{
	    ਜੇ (has_mbyte)
	    {
		ਜੇ ((*mb_ptr2len)(bd.textstart) == 1)
		    ++bd.textstart;
		ਨਹੀਂਤਾਂ
		{
		    ws_vcol = 0;
		    bd.startspaces = 0;
		}
	    }
	    ਨਹੀਂਤਾਂ
		++bd.textstart;
	}

	// TODO: is passing bd.textstart for start of the line OK?
	init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum,
				   bd.ਸ਼ੁਰੂ_vcol, bd.textstart, bd.textstart);
	ਲਈ ( ; VIM_ISWHITE(*cts.cts_ptr); )
	{
	    incr = lbr_chartabsize_adv(&cts);
	    ਕੁੱਲ += incr;
	    cts.cts_vcol += incr;
	}
	bd.textstart = cts.cts_ptr;
	bd.ਸ਼ੁਰੂ_vcol = cts.cts_vcol;
	clear_chartabsize_arg(&cts);

	// OK, now total=all the VWS reqd, and textstart points at the 1st
	// non-ws char in the block.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
	ਜੇ (!curbuf->b_p_et)
	    tabstop_fromto(ws_vcol, ws_vcol + ਕੁੱਲ,
				ts_val, curbuf->b_p_vts_array, &tabs, &spaces);
	ਨਹੀਂਤਾਂ
	    spaces = ਕੁੱਲ;
#ਨਹੀਂਤਾਂ
	ਜੇ (!curbuf->b_p_et)
	    tabs = ((ws_vcol % ts_val) + ਕੁੱਲ) / ts_val; // number of tabs
	ਜੇ (tabs > 0)
	    spaces = ((ws_vcol % ts_val) + ਕੁੱਲ) % ts_val; // number of spp
	ਨਹੀਂਤਾਂ
	    spaces = ਕੁੱਲ;
#ਜੇ_ਅੰਤ
	// if we're splitting a TAB, allow for it
	bd.textcol -= bd.pre_whitesp_c - (bd.startspaces != 0);

	new_ਸਤਰ_len = bd.textcol + tabs + spaces + (oldlen - (bd.textstart - oldp));
	newp = alloc(new_ਸਤਰ_len + 1);
	ਜੇ (newp == ਨਿਰਾ)
	    ਵਾਪਸ;
	mch_memmove(newp, oldp, (size_t)bd.textcol);
	newlen = bd.textcol;
	vim_memset(newp + newlen, TAB, (size_t)tabs);
	newlen += tabs;
	vim_memset(newp + newlen, ' ', (size_t)spaces);
	STRCPY(newp + newlen + spaces, bd.textstart);
    }
    ਨਹੀਂਤਾਂ //ਖੱਬਾ
    {
	colnr_T	    destination_ਖਾਨਾ;	// column to which text in block will
					// be shifted
	char_u	    *verbatim_copy_ਸਿਰਾ;	// end of the part of the line which is
					// copied verbatim
	colnr_T	    verbatim_copy_ਚੌੜਾਈ;// the (displayed) width of this part
					// of line
	size_t	    fill;		// nr of spaces that replace a TAB
	size_t	    ਖੰਡ_space_ਚੌੜਾਈ;
	size_t	    ਖਿਸਕਾਓ_amount;
	char_u	    *non_white = bd.textstart;
	colnr_T	    non_white_ਖਾਨਾ;
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
	ਜੇ (bd.startspaces)
	    MB_PTR_ADV(non_white);

	// The character's column is in "bd.start_vcol".
	non_white_ਖਾਨਾ = bd.ਸ਼ੁਰੂ_vcol;

	init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum,
				   non_white_ਖਾਨਾ, bd.textstart, non_white);
	ਜਦੋਂ (VIM_ISWHITE(*cts.cts_ptr))
	{
	    incr = lbr_chartabsize_adv(&cts);
	    cts.cts_vcol += incr;
	}
	non_white_ਖਾਨਾ = cts.cts_vcol;
	non_white = cts.cts_ptr;
	clear_chartabsize_arg(&cts);

	ਖੰਡ_space_ਚੌੜਾਈ = non_white_ਖਾਨਾ - oap->ਸ਼ੁਰੂ_vcol;
	// We will shift by "total" or "block_space_width", whichever is less.
	ਖਿਸਕਾਓ_amount = (ਖੰਡ_space_ਚੌੜਾਈ < (size_t)ਕੁੱਲ
					 ? ਖੰਡ_space_ਚੌੜਾਈ : (size_t)ਕੁੱਲ);

	// The column to which we will shift the text.
	destination_ਖਾਨਾ = (colnr_T)(non_white_ਖਾਨਾ - ਖਿਸਕਾਓ_amount);

	// Now let's find out how much of the beginning of the line we can
	// reuse without modification.
	verbatim_copy_ਸਿਰਾ = bd.textstart;
	verbatim_copy_ਚੌੜਾਈ = bd.ਸ਼ੁਰੂ_vcol;

	// If "bd.startspaces" is set, "bd.textstart" points to the character
	// preceding the block. We have to subtract its width to obtain its
	// column number.
	ਜੇ (bd.startspaces)
	    verbatim_copy_ਚੌੜਾਈ -= bd.ਸ਼ੁਰੂ_char_vcols;
	init_chartabsize_arg(&cts, curwin, 0, verbatim_copy_ਚੌੜਾਈ,
					     bd.textstart, verbatim_copy_ਸਿਰਾ);
	ਜਦੋਂ (cts.cts_vcol < destination_ਖਾਨਾ)
	{
	    incr = lbr_chartabsize(&cts);
	    ਜੇ (cts.cts_vcol + incr > destination_ਖਾਨਾ)
		ਰੋਕੋ;
	    cts.cts_vcol += incr;
	    MB_PTR_ADV(cts.cts_ptr);
	}
	verbatim_copy_ਚੌੜਾਈ = cts.cts_vcol;
	verbatim_copy_ਸਿਰਾ = cts.cts_ptr;
	clear_chartabsize_arg(&cts);

	// If "destination_col" is different from the width of the initial
	// part of the line that will be copied, it means we encountered a tab
	// character, which we will have to partly replace with spaces.
	fill = destination_ਖਾਨਾ - verbatim_copy_ਚੌੜਾਈ;

	// The replacement line will consist of:
	// - the beginning of the original line up to "verbatim_copy_end",
	// - "fill" number of spaces,
	// - the rest of the line, pointed to by non_white.
	fixedlen = verbatim_copy_ਸਿਰਾ - oldp;
	new_ਸਤਰ_len = fixedlen + fill + (oldlen - (non_white - oldp));

	newp = alloc(new_ਸਤਰ_len + 1);
	ਜੇ (newp == ਨਿਰਾ)
	    ਵਾਪਸ;
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
    ਜੇ (new_ਸਤਰ_len >= oldlen)
	added = (ਪੂਰਨ)(new_ਸਤਰ_len - oldlen);
    ਨਹੀਂਤਾਂ
	added = 0 - (ਪੂਰਨ)(oldlen - new_ਸਤਰ_len);
    inserted_ਬਾਈਟਾਂ(curwin->w_cursor.lnum, bd.textcol, added);
    State = oldstate;
    curwin->w_cursor.ਖਾਨਾ = oldcol;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    p_ri = old_p_ri;
#ਜੇ_ਅੰਤ
}

/*
 * Insert string "s" (b_insert ? before : after) block :AKelly
 * Caller must prepare for undo.
 */
    ਸਥਿਰ ਖਾਲੀ
ਖੰਡ_ਪਾਓ(
    oparg_T		*oap,
    char_u		*s,
    size_t		slen,
    ਪੂਰਨ			b_ਪਾਓ,
    ਬਣਤਰ ਖੰਡ_def	*bdp)
{
    ਪੂਰਨ		ts_val;
    ਪੂਰਨ		ਗਿਣਤੀ_2 = 0;	// extra spaces to replace a cut TAB
    ਪੂਰਨ		spaces = 0;	// non-zero if cutting a TAB
    colnr_T	ਵਿੱਥ;		// pointer along new line
    colnr_T	startcol;	// column where insert starts
    char_u	*newp, *oldp;	// new, old lines
    linenr_T	lnum;		// loop var
    ਪੂਰਨ		oldstate = State;

    State = ਬਹੁਲਕ_ਪਾਓ;	// don't want MODE_REPLACE for State

    ਲਈ (lnum = oap->ਸ਼ੁਰੂ.lnum + 1; lnum <= oap->ਸਿਰਾ.lnum; lnum++)
    {
	ਖੰਡ_prep(oap, bdp, lnum, TRUE);
	ਜੇ (bdp->is_short && b_ਪਾਓ)
	    ਜਾਰੀ;	// OP_INSERT, line ends before block start

	oldp = ml_get(lnum);

	ਜੇ (b_ਪਾਓ)
	{
	    ts_val = bdp->ਸ਼ੁਰੂ_char_vcols;
	    spaces = bdp->startspaces;
	    ਜੇ (spaces != 0)
		ਗਿਣਤੀ_2 = ts_val - 1; // we're cutting a TAB
	    ਵਿੱਥ = bdp->textcol;
	}
	ਨਹੀਂਤਾਂ // append
	{
	    ts_val = bdp->ਸਿਰਾ_char_vcols;
	    ਜੇ (!bdp->is_short) // spaces = padding after block
	    {
		spaces = (bdp->endspaces ? ts_val - bdp->endspaces : 0);
		ਜੇ (spaces != 0)
		    ਗਿਣਤੀ_2 = ts_val - 1; // we're cutting a TAB
		ਵਿੱਥ = bdp->textcol + bdp->textlen - (spaces != 0);
	    }
	    ਨਹੀਂਤਾਂ // spaces = padding to block edge
	    {
		// if $ used, just append to EOL (ie spaces==0)
		ਜੇ (!bdp->is_MAX)
		    spaces = (oap->ਸਿਰਾ_vcol - bdp->ਸਿਰਾ_vcol) + 1;
		ਗਿਣਤੀ_2 = spaces;
		ਵਿੱਥ = bdp->textcol + bdp->textlen;
	    }
	}

	ਜੇ (has_mbyte && spaces > 0)
	    // avoid copying part of a multi-byte character
	    ਵਿੱਥ -= (*mb_ਸਿਰ_off)(oldp, oldp + ਵਿੱਥ);

	ਜੇ (spaces < 0)  // can happen when the cursor was moved
	    spaces = 0;

	// Make sure the allocated size matches what is actually copied below.
	newp = alloc(ml_get_len(lnum) + spaces + slen
		    + (spaces > 0 && !bdp->is_short ? ts_val - spaces : 0)
								  + ਗਿਣਤੀ_2 + 1);
	ਜੇ (newp == ਨਿਰਾ)
	    ਜਾਰੀ;

	// copy up to shifted part
	mch_memmove(newp, oldp, (size_t)ਵਿੱਥ);
	oldp += ਵਿੱਥ;

	// insert pre-padding
	vim_memset(newp + ਵਿੱਥ, ' ', (size_t)spaces);
	startcol = ਵਿੱਥ + spaces;

	// copy the new text
	mch_memmove(newp + startcol, s, slen);
	ਵਿੱਥ += (ਪੂਰਨ)slen;

	ਜੇ (spaces > 0 && !bdp->is_short)
	{
	    ਜੇ (*oldp == TAB)
	    {
		// insert post-padding
		vim_memset(newp + ਵਿੱਥ + spaces, ' ',
						    (size_t)(ts_val - spaces));
		// we're splitting a TAB, don't copy it
		oldp++;
		// We allowed for that TAB, remember this now
		ਗਿਣਤੀ_2++;
	    }
	    ਨਹੀਂਤਾਂ
		// Not a TAB, no extra spaces
		ਗਿਣਤੀ_2 = spaces;
	}

	ਜੇ (spaces > 0)
	    ਵਿੱਥ += ਗਿਣਤੀ_2;
	STRCPY(newp + ਵਿੱਥ, oldp);

	ml_replace(lnum, newp, FALSE);

	ਜੇ (b_ਪਾਓ)
	    // correct any text properties
	    inserted_ਬਾਈਟਾਂ(lnum, startcol, (ਪੂਰਨ)slen);

	ਜੇ (lnum == oap->ਸਿਰਾ.lnum)
	{
	    // Set "']" mark to the end of the block instead of the end of
	    // the insert in the first line.
	    curbuf->b_op_ਸਿਰਾ.lnum = oap->ਸਿਰਾ.lnum;
	    curbuf->b_op_ਸਿਰਾ.ਖਾਨਾ = ਵਿੱਥ;
	    ਜੇ (curbuf->b_visual.vi_ਸਿਰਾ.coladd)
	    {
		curbuf->b_visual.vi_ਸਿਰਾ.ਖਾਨਾ += curbuf->b_visual.vi_ਸਿਰਾ.coladd;
		curbuf->b_visual.vi_ਸਿਰਾ.coladd = 0;
	    }
	}
    } // for all lnum

    ਬਦਲਿਆ_ਸਤਰਾਂ(oap->ਸ਼ੁਰੂ.lnum + 1, 0, oap->ਸਿਰਾ.lnum + 1, 0L);

    State = oldstate;
}

/*
 * Handle a delete operation.
 *
 * Return FAIL if undo failed, OK otherwise.
 */
    ਪੂਰਨ
op_delete(oparg_T *oap)
{
    ਪੂਰਨ			n;
    linenr_T		lnum;
    char_u		*ptr;
    char_u		*newp, *oldp;
    ਬਣਤਰ ਖੰਡ_def	bd;
    linenr_T		old_lcount = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    ਪੂਰਨ			did_yank = FALSE;

    ਜੇ (curbuf->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ)	    // nothing to do
	ਵਾਪਸ OK;

    // Nothing to delete, return here.	Do prepare undo, for op_change().
    ਜੇ (oap->ਖਾਲੀ_ਥਾਵਾਂ_2)
	ਵਾਪਸ u_save_cursor();

    ਜੇ (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	ਵਾਪਸ FAIL;
    }

    ਜੇ (VIsual_select && oap->is_VIsual)
	// use register given with CTRL_R, defaults to zero
	oap->regname = VIsual_select_reg;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_CLIPMETHOD
    adjust_clip_reg(&oap->regname);
#ਜੇ_ਅੰਤ

    ਜੇ (has_mbyte)
	mb_adjust_opend(oap);

    /*
     * Imitate the strange Vi behaviour: If the delete spans more than one
     * line and motion_type == MCHAR and the result is a blank line, make the
     * delete linewise.  Don't do this for the change command or Visual mode.
     */
    ਜੇ (       oap->motion_type == MCHAR
	    && !oap->is_VIsual
	    && !oap->ਖੰਡ_ਬਹੁਲਕ
	    && oap->ਸਤਰ_ਗਿਣਤੀ > 1
	    && oap->motion_force == NUL
	    && (vim_strchr(p_cpo, CPO_ਸ਼ਬਦ) != ਨਿਰਾ)
	    && oap->op_type == OP_DELETE)
    {
	ptr = ml_get(oap->ਸਿਰਾ.lnum) + oap->ਸਿਰਾ.ਖਾਨਾ;
	ਜੇ (*ptr != NUL)
	    ptr += oap->inclusive;
	ptr = skipwhite(ptr);
	ਜੇ (*ptr == NUL && inindent(0))
	    oap->motion_type = MLINE;
    }

    /*
     * Check for trying to delete (e.g. "D") in an empty line.
     * Note: For the change operator it is ok.
     */
    ਜੇ (       oap->motion_type == MCHAR
	    && oap->ਸਤਰ_ਗਿਣਤੀ == 1
	    && oap->op_type == OP_DELETE
	    && *ml_get(oap->ਸ਼ੁਰੂ.lnum) == NUL)
    {
	/*
	 * It's an error to operate on an empty region, when 'E' included in
	 * 'cpoptions' (Vi compatible).
	 */
	ਜੇ (virtual_op)
	    // Virtual editing: Nothing gets deleted, but we set the '[ and ']
	    // marks as if it happened.
	    ਜਾਓ setmarks;
	ਜੇ (vim_strchr(p_cpo, CPO_EMPTYREGION) != ਨਿਰਾ)
	    beep_flush();
	ਵਾਪਸ OK;
    }

    /*
     * Do a yank of whatever we're about to delete.
     * If a yank register was specified, put the deleted text into that
     * register.  For the black hole register '_' don't yank anything.
     */
    ਜੇ (oap->regname != '_')
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD_PROVIDER
	inc_clip_provider();
#ਜੇ_ਅੰਤ
	ਜੇ (oap->regname != 0)
	{
	    // check for read-only register
	    ਜੇ (!valid_yank_reg(oap->regname, TRUE))
	    {
		beep_flush();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD_PROVIDER
		dec_clip_provider();
#ਜੇ_ਅੰਤ
		ਵਾਪਸ OK;
	    }
	    get_yank_ਰਜਿਸਟਰ(oap->regname, TRUE); // yank into specif'd reg.
	    ਜੇ (op_yank(oap, TRUE, FALSE) == OK)   // yank without message
		did_yank = TRUE;
	}
	ਨਹੀਂਤਾਂ
	    reset_y_append(); // not appending to unnamed register

	/*
	 * Put deleted text into register 1 and shift number registers if the
	 * delete contains a line break, or when using a specific operator (Vi
	 * compatible)
	 */
	ਜੇ (oap->motion_type == MLINE || oap->ਸਤਰ_ਗਿਣਤੀ > 1
							   || oap->use_reg_one)
	{
	    ਖਿਸਕਾਓ_delete_registers();
	    ਜੇ (op_yank(oap, TRUE, FALSE) == OK)
		did_yank = TRUE;
	}

	// Yank into small delete register when no named register specified
	// and the delete is within one line.
	ਜੇ ((
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_CLIPMETHOD
	    ((clip_unnamed & CLIP_UNNAMED) && oap->regname == '*') ||
	    ((clip_unnamed & CLIP_UNNAMED_PLUS) && oap->regname == '+') ||
#ਜੇ_ਅੰਤ
	    oap->regname == 0) && oap->motion_type != MLINE
						      && oap->ਸਤਰ_ਗਿਣਤੀ == 1)
	{
	    oap->regname = '-';
	    get_yank_ਰਜਿਸਟਰ(oap->regname, TRUE);
	    ਜੇ (op_yank(oap, TRUE, FALSE) == OK)
		did_yank = TRUE;
	    oap->regname = 0;
	}

	/*
	 * If there's too much stuff to fit in the yank register, then get a
	 * confirmation before doing the delete. This is crude, but simple.
	 * And it avoids doing a delete of something we can't put back if we
	 * want.
	 */
	ਜੇ (!did_yank)
	{
	    ਪੂਰਨ ਸੁਨੇਹਾ_silent_save = ਸੁਨੇਹਾ_silent;

	    ਸੁਨੇਹਾ_silent = 0;	// must display the prompt
	    n = ask_yesno((char_u *)_("cannot yank; delete anyway"), TRUE);
	    ਸੁਨੇਹਾ_silent = ਸੁਨੇਹਾ_silent_save;
	    ਜੇ (n != 'y')
	    {
		emsg(_(e_command_aborted));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD_PROVIDER
		dec_clip_provider();
#ਜੇ_ਅੰਤ
		ਵਾਪਸ FAIL;
	    }
	}

#ਜੇ defined(FEAT_EVAL)
	ਜੇ (did_yank && has_textyankpost())
	    yank_do_autocmd(oap, get_y_current());
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#ਜੇ_ਅੰਤ
    }

    /*
     * block mode delete
     */
    ਜੇ (oap->ਖੰਡ_ਬਹੁਲਕ)
    {
	ਜੇ (u_save((linenr_T)(oap->ਸ਼ੁਰੂ.lnum - 1),
			       (linenr_T)(oap->ਸਿਰਾ.lnum + 1)) == FAIL)
	    ਵਾਪਸ FAIL;

	ਲਈ (lnum = curwin->w_cursor.lnum; lnum <= oap->ਸਿਰਾ.lnum; ++lnum)
	{
	    ਖੰਡ_prep(oap, &bd, lnum, TRUE);
	    ਜੇ (bd.textlen == 0)	// nothing to delete
		ਜਾਰੀ;

	    // Adjust cursor position for tab replaced by spaces and 'lbr'.
	    ਜੇ (lnum == curwin->w_cursor.lnum)
	    {
		curwin->w_cursor.ਖਾਨਾ = bd.textcol + bd.startspaces;
		curwin->w_cursor.coladd = 0;
	    }

	    // "n" == number of chars deleted
	    // If we delete a TAB, it may be replaced by several characters.
	    // Thus the number of characters may increase!
	    n = bd.textlen - bd.startspaces - bd.endspaces;
	    oldp = ml_get(lnum);
	    newp = alloc(ml_get_len(lnum) + 1 - n);
	    ਜੇ (newp == ਨਿਰਾ)
		ਜਾਰੀ;
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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	    ਜੇ (curbuf->b_has_textprop && n != 0)
		adjust_prop_columns(lnum, bd.textcol, -n, 0);
#ਜੇ_ਅੰਤ
	}

	check_cursor_ਖਾਨਾ();
	ਬਦਲਿਆ_ਸਤਰਾਂ(curwin->w_cursor.lnum, curwin->w_cursor.ਖਾਨਾ,
						       oap->ਸਿਰਾ.lnum + 1, 0L);
	oap->ਸਤਰ_ਗਿਣਤੀ = 0;	    // no lines deleted
    }
    ਨਹੀਂਤਾਂ ਜੇ (oap->motion_type == MLINE)
    {
	ਜੇ (oap->op_type == OP_CHANGE)
	{
	    // Delete the lines except the first one.  Temporarily move the
	    // cursor to the next line.  Save the current line number, if the
	    // last line is deleted it may be changed.
	    ਜੇ (oap->ਸਤਰ_ਗਿਣਤੀ > 1)
	    {
		lnum = curwin->w_cursor.lnum;
		++curwin->w_cursor.lnum;
		del_ਸਤਰਾਂ((ਲੰਮਾ)(oap->ਸਤਰ_ਗਿਣਤੀ - 1), TRUE);
		curwin->w_cursor.lnum = lnum;
	    }
	    ਜੇ (u_save_cursor() == FAIL)
		ਵਾਪਸ FAIL;
	    ਜੇ (curbuf->b_p_ai)		    // don't delete indent
	    {
		beginline(BL_WHITE);	    // cursor on first non-white
		did_ai = TRUE;		    // delete the indent when ESC hit
		ai_ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;
	    }
	    ਨਹੀਂਤਾਂ
		beginline(0);		    // cursor in column 0
	    truncate_ਸਤਰ(FALSE);   // delete the rest of the line,
				    // leaving cursor past last char in line
	    ਜੇ (oap->ਸਤਰ_ਗਿਣਤੀ > 1)
		u_clearline();	    // "U" command not possible after "2cc"
	}
	ਨਹੀਂਤਾਂ
	{
	    del_ਸਤਰਾਂ(oap->ਸਤਰ_ਗਿਣਤੀ, TRUE);
	    beginline(BL_WHITE | BL_FIX);
	    u_clearline();	// "U" command not possible after "dd"
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (virtual_op)
	{
	    ਪੂਰਨ		endcol = 0;

	    // For virtualedit: break the tabs that are partly included.
	    ਜੇ (gchar_pos(&oap->ਸ਼ੁਰੂ) == '\t')
	    {
		ਜੇ (u_save_cursor() == FAIL)	// save first line for undo
		    ਵਾਪਸ FAIL;
		ਜੇ (oap->ਸਤਰ_ਗਿਣਤੀ == 1)
		    endcol = getviscol2(oap->ਸਿਰਾ.ਖਾਨਾ, oap->ਸਿਰਾ.coladd);
		coladvance_force(getviscol2(oap->ਸ਼ੁਰੂ.ਖਾਨਾ, oap->ਸ਼ੁਰੂ.coladd));
		oap->ਸ਼ੁਰੂ = curwin->w_cursor;
		ਜੇ (oap->ਸਤਰ_ਗਿਣਤੀ == 1)
		{
		    coladvance(endcol);
		    oap->ਸਿਰਾ.ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;
		    oap->ਸਿਰਾ.coladd = curwin->w_cursor.coladd;
		    curwin->w_cursor = oap->ਸ਼ੁਰੂ;
		}
	    }

	    // Break a tab only when it's included in the area.
	    ਜੇ (gchar_pos(&oap->ਸਿਰਾ) == '\t'
				     && (ਪੂਰਨ)oap->ਸਿਰਾ.coladd < oap->inclusive)
	    {
		// save last line for undo
		ਜੇ (u_save((linenr_T)(oap->ਸਿਰਾ.lnum - 1),
				       (linenr_T)(oap->ਸਿਰਾ.lnum + 1)) == FAIL)
		    ਵਾਪਸ FAIL;
		curwin->w_cursor = oap->ਸਿਰਾ;
		coladvance_force(getviscol2(oap->ਸਿਰਾ.ਖਾਨਾ, oap->ਸਿਰਾ.coladd));
		oap->ਸਿਰਾ = curwin->w_cursor;
		curwin->w_cursor = oap->ਸ਼ੁਰੂ;
	    }
	    ਜੇ (has_mbyte)
		mb_adjust_opend(oap);
	}

	ਜੇ (oap->ਸਤਰ_ਗਿਣਤੀ == 1)	// delete characters within one line
	{
	    ਜੇ (u_save_cursor() == FAIL)	// save line for undo
		ਵਾਪਸ FAIL;

	    // if 'cpoptions' contains '$', display '$' at end of change
	    ਜੇ (       vim_strchr(p_cpo, CPO_DOLLAR) != ਨਿਰਾ
		    && oap->op_type == OP_CHANGE
		    && oap->ਸਿਰਾ.lnum == curwin->w_cursor.lnum
		    && !oap->is_VIsual)
		display_dollar(oap->ਸਿਰਾ.ਖਾਨਾ - !oap->inclusive);

	    n = oap->ਸਿਰਾ.ਖਾਨਾ - oap->ਸ਼ੁਰੂ.ਖਾਨਾ + 1 - !oap->inclusive;

	    ਜੇ (virtual_op)
	    {
		// fix up things for virtualedit-delete:
		// break the tabs which are going to get in our way
		ਪੂਰਨ		len = ml_get_curline_len();

		ਜੇ (oap->ਸਿਰਾ.coladd != 0
			&& (ਪੂਰਨ)oap->ਸਿਰਾ.ਖਾਨਾ >= len - 1
			&& !(oap->ਸ਼ੁਰੂ.coladd && (ਪੂਰਨ)oap->ਸਿਰਾ.ਖਾਨਾ >= len - 1))
		    n++;
		// Delete at least one char (e.g, when on a control char).
		ਜੇ (n == 0 && oap->ਸ਼ੁਰੂ.coladd != oap->ਸਿਰਾ.coladd)
		    n = 1;

		// When deleted a char in the line, reset coladd.
		ਜੇ (gchar_cursor() != NUL)
		    curwin->w_cursor.coladd = 0;
	    }
	    (ਖਾਲੀ)del_ਬਾਈਟਾਂ((ਲੰਮਾ)n, !virtual_op,
			    oap->op_type == OP_DELETE && !oap->is_VIsual);
	}
	ਨਹੀਂਤਾਂ				// delete characters between lines
	{
	    pos_T   curpos;

	    // save deleted and changed lines for undo
	    ਜੇ (u_save((linenr_T)(curwin->w_cursor.lnum - 1),
		 (linenr_T)(curwin->w_cursor.lnum + oap->ਸਤਰ_ਗਿਣਤੀ)) == FAIL)
		ਵਾਪਸ FAIL;

	    truncate_ਸਤਰ(TRUE);	// delete from cursor to end of line

	    curpos = curwin->w_cursor;	// remember curwin->w_cursor
	    ++curwin->w_cursor.lnum;
	    del_ਸਤਰਾਂ((ਲੰਮਾ)(oap->ਸਤਰ_ਗਿਣਤੀ - 2), FALSE);

	    // delete from start of line until op_end
	    n = (oap->ਸਿਰਾ.ਖਾਨਾ + 1 - !oap->inclusive);
	    curwin->w_cursor.ਖਾਨਾ = 0;
	    (ਖਾਲੀ)del_ਬਾਈਟਾਂ((ਲੰਮਾ)n, !virtual_op,
			    oap->op_type == OP_DELETE && !oap->is_VIsual);
	    curwin->w_cursor = curpos;	// restore curwin->w_cursor
	    (ਖਾਲੀ)do_join(2, FALSE, FALSE, FALSE, FALSE);
	}
	ਜੇ (oap->op_type == OP_DELETE)
	    auto_format(FALSE, TRUE);
    }

    msgmore(curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ - old_lcount);

setmarks:
    ਜੇ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	ਜੇ (oap->ਖੰਡ_ਬਹੁਲਕ)
	{
	    curbuf->b_op_ਸਿਰਾ.lnum = oap->ਸਿਰਾ.lnum;
	    curbuf->b_op_ਸਿਰਾ.ਖਾਨਾ = oap->ਸ਼ੁਰੂ.ਖਾਨਾ;
	}
	ਨਹੀਂਤਾਂ
	    curbuf->b_op_ਸਿਰਾ = oap->ਸ਼ੁਰੂ;
	curbuf->b_op_ਸ਼ੁਰੂ = oap->ਸ਼ੁਰੂ;
    }

    ਵਾਪਸ OK;
}

/*
 * Adjust end of operating area for ending on a multi-byte character.
 * Used for deletion.
 */
    ਸਥਿਰ ਖਾਲੀ
mb_adjust_opend(oparg_T *oap)
{
    char_u	*ਸਤਰ;
    char_u	*ptr;

    ਜੇ (!oap->inclusive)
	ਵਾਪਸ;

    ਸਤਰ = ml_get(oap->ਸਿਰਾ.lnum);
    ptr = ਸਤਰ + oap->ਸਿਰਾ.ਖਾਨਾ;
    ਜੇ (*ptr != NUL)
    {
	ptr -= (*mb_ਸਿਰ_off)(ਸਤਰ, ptr);
	ptr += (*mb_ptr2len)(ptr) - 1;
	oap->ਸਿਰਾ.ਖਾਨਾ = ptr - ਸਤਰ;
    }
}

/*
 * Replace the character under the cursor with "c".
 * This takes care of multi-byte characters.
 */
    ਸਥਿਰ ਖਾਲੀ
replace_character(ਪੂਰਨ c)
{
    ਪੂਰਨ n = State;

    State = ਬਹੁਲਕ_REPLACE;
    ins_char(c);
    State = n;
    // Backup to the replaced character.
    dec_cursor();
}

/*
 * Replace a whole area with one character.
 */
    ਪੂਰਨ
op_replace(oparg_T *oap, ਪੂਰਨ c)
{
    ਪੂਰਨ			n, numc;
    ਪੂਰਨ			ਅੰਕ_chars;
    char_u		*newp, *oldp;
    size_t		newlen, oldlen;
    ਬਣਤਰ ਖੰਡ_def	bd;
    char_u		*after_p = ਨਿਰਾ;
    ਪੂਰਨ			had_ctrl_v_cr = FALSE;

    ਜੇ ((curbuf->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ ) || oap->ਖਾਲੀ_ਥਾਵਾਂ_2)
	ਵਾਪਸ OK;	    // nothing to do

    ਜੇ (c == REPLACE_CR_NCHAR)
    {
	had_ctrl_v_cr = TRUE;
	c = CAR;
    }
    ਨਹੀਂਤਾਂ ਜੇ (c == REPLACE_NL_NCHAR)
    {
	had_ctrl_v_cr = TRUE;
	c = NL;
    }

    ਜੇ (has_mbyte)
	mb_adjust_opend(oap);

    ਜੇ (u_save((linenr_T)(oap->ਸ਼ੁਰੂ.lnum - 1),
				       (linenr_T)(oap->ਸਿਰਾ.lnum + 1)) == FAIL)
	ਵਾਪਸ FAIL;

    /*
     * block mode replace
     */
    ਜੇ (oap->ਖੰਡ_ਬਹੁਲਕ)
    {
	bd.is_MAX = (curwin->w_curswant == MAXCOL);
	ਲਈ ( ; curwin->w_cursor.lnum <= oap->ਸਿਰਾ.lnum; ++curwin->w_cursor.lnum)
	{
	    curwin->w_cursor.ਖਾਨਾ = 0;  // make sure cursor position is valid
	    ਖੰਡ_prep(oap, &bd, curwin->w_cursor.lnum, TRUE);
	    ਜੇ (bd.textlen == 0 && (!virtual_op || bd.is_MAX))
		ਜਾਰੀ;	    // nothing to replace

	    // n == number of extra chars required
	    // If we split a TAB, it may be replaced by several characters.
	    // Thus the number of characters may increase!
	    // If the range starts in virtual space, count the initial
	    // coladd offset as part of "startspaces"
	    ਜੇ (virtual_op && bd.is_short && *bd.textstart == NUL)
	    {
		pos_T vpos;

		vpos.lnum = curwin->w_cursor.lnum;
		getvpos(&vpos, oap->ਸ਼ੁਰੂ_vcol);
		bd.startspaces += vpos.coladd;
		n = bd.startspaces;
	    }
	    ਨਹੀਂਤਾਂ
		// allow for pre spaces
		n = (bd.startspaces ? bd.ਸ਼ੁਰੂ_char_vcols - 1 : 0);

	    // allow for post spp
	    n += (bd.endspaces
		    && !bd.is_oneChar
		    && bd.ਸਿਰਾ_char_vcols > 0) ? bd.ਸਿਰਾ_char_vcols - 1 : 0;
	    // Figure out how many characters to replace.
	    numc = oap->ਸਿਰਾ_vcol - oap->ਸ਼ੁਰੂ_vcol + 1;
	    ਜੇ (bd.is_short && (!virtual_op || bd.is_MAX))
		numc -= (oap->ਸਿਰਾ_vcol - bd.ਸਿਰਾ_vcol) + 1;

	    // A double-wide character can be replaced only up to half the
	    // times.
	    ਜੇ ((*mb_char2cells)(c) > 1)
	    {
		ਜੇ ((numc & 1) && !bd.is_short)
		{
		    ++bd.endspaces;
		    ++n;
		}
		numc = numc / 2;
	    }

	    // Compute bytes needed, move character count to num_chars.
	    ਅੰਕ_chars = numc;
	    numc *= (*mb_char2len)(c);
	    // oldlen includes textlen, so don't double count
	    n += numc - bd.textlen;

	    oldp = ml_get_curline();
	    oldlen = ml_get_curline_len();
	    newp = alloc(oldlen + 1 + n);
	    ਜੇ (newp == ਨਿਰਾ)
		ਜਾਰੀ;
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
	    ਜੇ (had_ctrl_v_cr || (c != '\r' && c != '\n'))
	    {
		ਜੇ (has_mbyte)
		{
		    ਜਦੋਂ (--ਅੰਕ_chars >= 0)
			newlen += (*mb_char2bytes)(c, newp + newlen);
		}
		ਨਹੀਂਤਾਂ
		{
		    vim_memset(newp + newlen, c, (size_t)numc);
		    newlen += numc;
		}
		ਜੇ (!bd.is_short)
		{
		    // insert post-spaces
		    vim_memset(newp + newlen, ' ', (size_t)bd.endspaces);
		    // copy the part after the changed part
		    STRCPY(newp + newlen + bd.endspaces,
				oldp + bd.textcol + bd.textlen);
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// Replacing with \r or \n means splitting the line.
		after_p = alloc(oldlen + 1 + n - newlen);
		ਜੇ (after_p != ਨਿਰਾ)
		    STRCPY(after_p, oldp + bd.textcol + bd.textlen);
	    }

	    // replace the line
	    ml_replace(curwin->w_cursor.lnum, newp, FALSE);
	    ਜੇ (after_p != ਨਿਰਾ)
	    {
		ml_append(curwin->w_cursor.lnum++, after_p, 0, FALSE);
		appended_ਸਤਰਾਂ_mark(curwin->w_cursor.lnum, 1L);
		oap->ਸਿਰਾ.lnum++;
		vim_free(after_p);
	    }
	}
    }
    ਨਹੀਂਤਾਂ
    {
	/*
	 * MCHAR and MLINE motion replace.
	 */
	ਜੇ (oap->motion_type == MLINE)
	{
	    oap->ਸ਼ੁਰੂ.ਖਾਨਾ = 0;
	    curwin->w_cursor.ਖਾਨਾ = 0;
	    oap->ਸਿਰਾ.ਖਾਨਾ = ml_get_len(oap->ਸਿਰਾ.lnum);
	    ਜੇ (oap->ਸਿਰਾ.ਖਾਨਾ)
		--oap->ਸਿਰਾ.ਖਾਨਾ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (!oap->inclusive)
	    dec(&(oap->ਸਿਰਾ));

	ਜਦੋਂ (LTOREQ_POS(curwin->w_cursor, oap->ਸਿਰਾ))
	{
	    ਪੂਰਨ ਪੂਰਾ = FALSE;

	    n = gchar_cursor();
	    ਜੇ (n != NUL)
	    {
		ਪੂਰਨ new_byte_len = (*mb_char2len)(c);
		ਪੂਰਨ old_byte_len = mb_ptr2len(ml_get_cursor());

		ਜੇ (new_byte_len > 1 || old_byte_len > 1)
		{
		    // This is slow, but it handles replacing a single-byte
		    // with a multi-byte and the other way around.
		    ਜੇ (curwin->w_cursor.lnum == oap->ਸਿਰਾ.lnum)
			oap->ਸਿਰਾ.ਖਾਨਾ += new_byte_len - old_byte_len;
		    replace_character(c);
		    ਪੂਰਾ = TRUE;
		}
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (n == TAB)
		    {
			ਪੂਰਨ ਸਿਰਾ_vcol = 0;

			ਜੇ (curwin->w_cursor.lnum == oap->ਸਿਰਾ.lnum)
			{
			    // oap->end has to be recalculated when
			    // the tab breaks
			    ਸਿਰਾ_vcol = getviscol2(oap->ਸਿਰਾ.ਖਾਨਾ,
							     oap->ਸਿਰਾ.coladd);
			}
			coladvance_force(getviscol());
			ਜੇ (curwin->w_cursor.lnum == oap->ਸਿਰਾ.lnum)
			    getvpos(&oap->ਸਿਰਾ, ਸਿਰਾ_vcol);
		    }
		    // with "coladd" set may move to just after a TAB
		    ਜੇ (gchar_cursor() != NUL)
		    {
			PBYTE(curwin->w_cursor, c);
			ਪੂਰਾ = TRUE;
		    }
		}
	    }
	    ਜੇ (!ਪੂਰਾ && virtual_op && curwin->w_cursor.lnum == oap->ਸਿਰਾ.lnum)
	    {
		ਪੂਰਨ virtcols = oap->ਸਿਰਾ.coladd;

		ਜੇ (curwin->w_cursor.lnum == oap->ਸ਼ੁਰੂ.lnum
			&& oap->ਸ਼ੁਰੂ.ਖਾਨਾ == oap->ਸਿਰਾ.ਖਾਨਾ && oap->ਸ਼ੁਰੂ.coladd)
		    virtcols -= oap->ਸ਼ੁਰੂ.coladd;

		// oap->end has been trimmed so it's effectively inclusive;
		// as a result an extra +1 must be counted so we don't
		// trample the NUL byte.
		coladvance_force(getviscol2(oap->ਸਿਰਾ.ਖਾਨਾ, oap->ਸਿਰਾ.coladd) + 1);
		curwin->w_cursor.ਖਾਨਾ -= (virtcols + 1);
		ਲਈ (; virtcols >= 0; virtcols--)
		{
		    ਜੇ ((*mb_char2len)(c) > 1)
		       replace_character(c);
		    ਨਹੀਂਤਾਂ
			PBYTE(curwin->w_cursor, c);
		    ਜੇ (inc(&curwin->w_cursor) == -1)
			ਰੋਕੋ;
		}
	    }

	    // Advance to next character, stop at the end of the file.
	    ਜੇ (inc_cursor() == -1)
		ਰੋਕੋ;
	}
    }

    curwin->w_cursor = oap->ਸ਼ੁਰੂ;
    check_cursor();
    ਬਦਲਿਆ_ਸਤਰਾਂ(oap->ਸ਼ੁਰੂ.lnum, oap->ਸ਼ੁਰੂ.ਖਾਨਾ, oap->ਸਿਰਾ.lnum + 1, 0L);

    ਜੇ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_ਸ਼ੁਰੂ = oap->ਸ਼ੁਰੂ;
	curbuf->b_op_ਸਿਰਾ = oap->ਸਿਰਾ;
    }

    ਵਾਪਸ OK;
}

ਸਥਿਰ ਪੂਰਨ swapchars(ਪੂਰਨ op_type, pos_T *pos, ਪੂਰਨ ਲੰਮਾਈ);

/*
 * Handle the (non-standard vi) tilde operator.  Also for "gu", "gU" and "g?".
 */
    ਸਥਿਰ ਖਾਲੀ
op_tilde(oparg_T *oap)
{
    pos_T		pos;
    ਬਣਤਰ ਖੰਡ_def	bd;
    ਪੂਰਨ			did_change = FALSE;

    ਜੇ (u_save((linenr_T)(oap->ਸ਼ੁਰੂ.lnum - 1),
				       (linenr_T)(oap->ਸਿਰਾ.lnum + 1)) == FAIL)
	ਵਾਪਸ;

    pos = oap->ਸ਼ੁਰੂ;
    ਜੇ (oap->ਖੰਡ_ਬਹੁਲਕ)		    // Visual block mode
    {
	ਲਈ (; pos.lnum <= oap->ਸਿਰਾ.lnum; ++pos.lnum)
	{
	    ਪੂਰਨ one_change;

	    ਖੰਡ_prep(oap, &bd, pos.lnum, FALSE);
	    pos.ਖਾਨਾ = bd.textcol;
	    one_change = swapchars(oap->op_type, &pos, bd.textlen);
	    did_change |= one_change;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
	    ਜੇ (netbeans_ਸਰਗਰਮ() && one_change)
	    {
		char_u *ptr;

		netbeans_removed(curbuf, pos.lnum, bd.textcol,
							    (ਲੰਮਾ)bd.textlen);
		// get the line now, it may have been flushed
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, bd.textcol,
						&ptr[bd.textcol], bd.textlen);
	    }
#ਜੇ_ਅੰਤ
	}
	ਜੇ (did_change)
	    ਬਦਲਿਆ_ਸਤਰਾਂ(oap->ਸ਼ੁਰੂ.lnum, 0, oap->ਸਿਰਾ.lnum + 1, 0L);
    }
    ਨਹੀਂਤਾਂ				    // not block mode
    {
	ਜੇ (oap->motion_type == MLINE)
	{
	    oap->ਸ਼ੁਰੂ.ਖਾਨਾ = 0;
	    pos.ਖਾਨਾ = 0;
	    oap->ਸਿਰਾ.ਖਾਨਾ = ml_get_len(oap->ਸਿਰਾ.lnum);
	    ਜੇ (oap->ਸਿਰਾ.ਖਾਨਾ)
		--oap->ਸਿਰਾ.ਖਾਨਾ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (!oap->inclusive)
	    dec(&(oap->ਸਿਰਾ));

	ਜੇ (pos.lnum == oap->ਸਿਰਾ.lnum)
	    did_change = swapchars(oap->op_type, &pos,
						  oap->ਸਿਰਾ.ਖਾਨਾ - pos.ਖਾਨਾ + 1);
	ਨਹੀਂਤਾਂ
	    ਲਈ (;;)
	    {
		did_change |= swapchars(oap->op_type, &pos,
			pos.lnum == oap->ਸਿਰਾ.lnum ? oap->ਸਿਰਾ.ਖਾਨਾ + 1
						  : ml_get_pos_len(&pos));
		ਜੇ (LTOREQ_POS(oap->ਸਿਰਾ, pos) || inc(&pos) == -1)
		    ਰੋਕੋ;
	    }
	ਜੇ (did_change)
	{
	    ਬਦਲਿਆ_ਸਤਰਾਂ(oap->ਸ਼ੁਰੂ.lnum, oap->ਸ਼ੁਰੂ.ਖਾਨਾ, oap->ਸਿਰਾ.lnum + 1,
									  0L);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
	    ਜੇ (netbeans_ਸਰਗਰਮ())
	    {
		char_u *ptr;
		ਪੂਰਨ ਗਿਣਤੀ_2;

		pos = oap->ਸ਼ੁਰੂ;
		ਜਦੋਂ (pos.lnum < oap->ਸਿਰਾ.lnum)
		{
		    ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		    ਗਿਣਤੀ_2 = ml_get_buf_len(curbuf, pos.lnum) - pos.ਖਾਨਾ;
		    netbeans_removed(curbuf, pos.lnum, pos.ਖਾਨਾ, (ਲੰਮਾ)ਗਿਣਤੀ_2);
		    // get the line again, it may have been flushed
		    ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		    netbeans_inserted(curbuf, pos.lnum, pos.ਖਾਨਾ,
							&ptr[pos.ਖਾਨਾ], ਗਿਣਤੀ_2);
		    pos.ਖਾਨਾ = 0;
		    pos.lnum++;
		}
		ਗਿਣਤੀ_2 = oap->ਸਿਰਾ.ਖਾਨਾ - pos.ਖਾਨਾ + 1;
		netbeans_removed(curbuf, pos.lnum, pos.ਖਾਨਾ, (ਲੰਮਾ)ਗਿਣਤੀ_2);
		// get the line again, it may have been flushed
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, pos.ਖਾਨਾ,
							&ptr[pos.ਖਾਨਾ], ਗਿਣਤੀ_2);
	    }
#ਜੇ_ਅੰਤ
	}
    }

    ਜੇ (!did_change && oap->is_VIsual)
	// No change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    ਜੇ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set '[ and '] marks.
	curbuf->b_op_ਸ਼ੁਰੂ = oap->ਸ਼ੁਰੂ;
	curbuf->b_op_ਸਿਰਾ = oap->ਸਿਰਾ;
    }

    ਜੇ (oap->ਸਤਰ_ਗਿਣਤੀ > p_report)
	smsg(NGETTEXT("%ld line changed", "%ld lines changed",
					    oap->ਸਤਰ_ਗਿਣਤੀ), oap->ਸਤਰ_ਗਿਣਤੀ);
}

/*
 * Invoke swapchar() on "length" bytes at position "pos".
 * "pos" is advanced to just after the changed characters.
 * "length" is rounded up to include the whole last multi-byte character.
 * Also works correctly when the number of bytes changes.
 * Returns TRUE if some character was changed.
 */
    ਸਥਿਰ ਪੂਰਨ
swapchars(ਪੂਰਨ op_type, pos_T *pos, ਪੂਰਨ ਲੰਮਾਈ)
{
    ਪੂਰਨ todo;
    ਪੂਰਨ	did_change = 0;

    ਲਈ (todo = ਲੰਮਾਈ; todo > 0; --todo)
    {
	ਜੇ (has_mbyte)
	{
	    ਪੂਰਨ len = (*mb_ptr2len)(ml_get_pos(pos));

	    // we're counting bytes, not characters
	    ਜੇ (len > 0)
		todo -= len - 1;
	}
	did_change |= swapchar(op_type, pos);
	ਜੇ (inc(pos) == -1)    // at end of file
	    ਰੋਕੋ;
    }
    ਵਾਪਸ did_change;
}

/*
 * If op_type == OP_UPPER: make uppercase,
 * if op_type == OP_LOWER: make lowercase,
 * if op_type == OP_ROT13: do rot13 encoding,
 * else swap case of character at 'pos'
 * returns TRUE when something actually changed.
 */
    ਪੂਰਨ
swapchar(ਪੂਰਨ op_type, pos_T *pos)
{
    ਪੂਰਨ	    c;
    ਪੂਰਨ	    nc;

    c = gchar_pos(pos);

    // Only do rot13 encoding for ASCII characters.
    ਜੇ (c >= 0x80 && op_type == OP_ROT13)
	ਵਾਪਸ FALSE;

    // ~ is OP_NOP, g~ is OP_TILDE, gU is OP_UPPER
    ਜੇ ((op_type == OP_ਉੱਤਲਾ || op_type == OP_NOP || op_type == OP_TILDE)
	    && c == 0xdf
	    && (enc_latin1like || STRCMP(p_enc, "iso-8859-2") == 0))
    {
	pos_T   sp = curwin->w_cursor;

	// Special handling for lowercase German sharp s (ß): convert to uppercase (ẞ).
	curwin->w_cursor = *pos;
	del_char(FALSE);
	ins_char(0x1E9E);
	curwin->w_cursor = sp;
	ਵਾਪਸ TRUE;
    }

    ਜੇ (enc_dbcs != 0 && c >= 0x100)	// No lower/uppercase letter
	ਵਾਪਸ FALSE;
    nc = c;
    ਜੇ (MB_ISLOWER(c))
    {
	ਜੇ (op_type == OP_ROT13)
	    nc = ROT13(c, 'a');
	ਨਹੀਂਤਾਂ ਜੇ (op_type != OP_ਹੇਠਲਾ)
	    nc = MB_TOUPPER(c);
    }
    ਨਹੀਂਤਾਂ ਜੇ (MB_ISUPPER(c))
    {
	ਜੇ (op_type == OP_ROT13)
	    nc = ROT13(c, 'A');
	ਨਹੀਂਤਾਂ ਜੇ (op_type != OP_ਉੱਤਲਾ)
	    nc = MB_TOLOWER(c);
    }
    ਜੇ (nc != c)
    {
	ਜੇ (enc_utf8 && (c >= 0x80 || nc >= 0x80))
	{
	    pos_T   sp = curwin->w_cursor;

	    curwin->w_cursor = *pos;
	    // don't use del_char(), it also removes composing chars
	    del_ਬਾਈਟਾਂ(utf_ptr2len(ml_get_cursor()), FALSE, FALSE);
	    ins_char(nc);
	    curwin->w_cursor = sp;
	}
	ਨਹੀਂਤਾਂ
	    PBYTE(*pos, nc);
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * op_insert - Insert and append operators for Visual mode.
 */
    ਖਾਲੀ
op_ਪਾਓ(oparg_T *oap, ਲੰਮਾ count1)
{
    ਲੰਮਾ		pre_textlen = 0;
    colnr_T		ind_pre_ਖਾਨਾ = 0, ind_post_ਖਾਨਾ;
    ਪੂਰਨ			ind_pre_vcol = 0, ind_post_vcol = 0;
    ਬਣਤਰ ਖੰਡ_def	bd;
    ਪੂਰਨ			i;
    pos_T		t1;
    pos_T		ਸ਼ੁਰੂ_ਪਾਓ;

    // edit() changes this - record it for OP_APPEND
    bd.is_MAX = (curwin->w_curswant == MAXCOL);

    // vis block is still marked. Get rid of it now.
    curwin->w_cursor.lnum = oap->ਸ਼ੁਰੂ.lnum;
    update_screen(UPD_INVERTED);

    ਜੇ (oap->ਖੰਡ_ਬਹੁਲਕ)
    {
	// When 'virtualedit' is used, need to insert the extra spaces before
	// doing block_prep().  When only "block" is used, virtual edit is
	// already disabled, but still need it when calling
	// coladvance_force().
	// coladvance_force() uses get_ve_flags() to get the 'virtualedit'
	// state for the current window.  To override that state, we need to
	// set the window-local value of ve_flags rather than the global value.
	ਜੇ (curwin->w_cursor.coladd > 0)
	{
	    ਪੂਰਨ		old_ve_flags = curwin->w_ve_flags;

	    ਜੇ (u_save_cursor() == FAIL)
		ਵਾਪਸ;

	    curwin->w_ve_flags = VE_ALL;
	    coladvance_force(oap->op_type == OP_APPEND
					   ? oap->ਸਿਰਾ_vcol + 1 : getviscol());
	    ਜੇ (oap->op_type == OP_APPEND)
		--curwin->w_cursor.ਖਾਨਾ;
	    curwin->w_ve_flags = old_ve_flags;
	}
	// Get the info about the block before entering the text
	ਖੰਡ_prep(oap, &bd, oap->ਸ਼ੁਰੂ.lnum, TRUE);
	// Get indent information
	ind_pre_ਖਾਨਾ = (colnr_T)getwhitecols_curline();
	ind_pre_vcol = get_indent();
	pre_textlen = ml_get_len(oap->ਸ਼ੁਰੂ.lnum) - bd.textcol;
	ਜੇ (oap->op_type == OP_APPEND)
	    pre_textlen -= bd.textlen;
    }

    ਜੇ (oap->op_type == OP_APPEND)
    {
	ਜੇ (oap->ਖੰਡ_ਬਹੁਲਕ && curwin->w_cursor.coladd == 0)
	{
	    // Move the cursor to the character right of the block.
	    curwin->w_ਸੈੱਟ_curswant = true;
	    ਜਦੋਂ (*ml_get_cursor() != NUL
		    && (curwin->w_cursor.ਖਾਨਾ < bd.textcol + bd.textlen))
		++curwin->w_cursor.ਖਾਨਾ;
	    ਜੇ (bd.is_short && !bd.is_MAX)
	    {
		// First line was too short, make it longer and adjust the
		// values in "bd".
		ਜੇ (u_save_cursor() == FAIL)
		    ਵਾਪਸ;
		ਲਈ (i = 0; i < bd.endspaces; ++i)
		    ins_char(' ');
		bd.textlen += bd.endspaces;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    curwin->w_cursor = oap->ਸਿਰਾ;
	    check_cursor_ਖਾਨਾ();

	    // Works just like an 'i'nsert on the next character.
	    ਜੇ (!LINEEMPTY(curwin->w_cursor.lnum)
		    && oap->ਸ਼ੁਰੂ_vcol != oap->ਸਿਰਾ_vcol)
		inc_cursor();
	}
    }

    t1 = oap->ਸ਼ੁਰੂ;
    ਸ਼ੁਰੂ_ਪਾਓ = curwin->w_cursor;
    (ਖਾਲੀ)edit(NUL, FALSE, (linenr_T)count1);

    // When a tab was inserted, and the characters in front of the tab
    // have been converted to a tab as well, the column of the cursor
    // might have actually been reduced, so need to adjust here.
    ਜੇ (t1.lnum == curbuf->b_op_ਸ਼ੁਰੂ_orig.lnum
	    && LT_POS(curbuf->b_op_ਸ਼ੁਰੂ_orig, t1))
	oap->ਸ਼ੁਰੂ = curbuf->b_op_ਸ਼ੁਰੂ_orig;

    // If user has moved off this line, we don't know what to do, so do
    // nothing.
    // Also don't repeat the insert when Insert mode ended with CTRL-C.
    ਜੇ (curwin->w_cursor.lnum != oap->ਸ਼ੁਰੂ.lnum || got_int)
	ਵਾਪਸ;

    ਜੇ (oap->ਖੰਡ_ਬਹੁਲਕ)
    {
	ਪੂਰਨ			ins_len;
	char_u			*firstline, *ins_ਪਾਠ;
	ਬਣਤਰ ਖੰਡ_def	bd2;
	ਪੂਰਨ			did_indent = FALSE;
	size_t			len;
	size_t			ਜੋੜੋ;
	// offset when cursor was moved in insert mode
	ਪੂਰਨ			ਵਿੱਥ = 0;

	// If indent kicked in, the firstline might have changed
	// but only do that, if the indent actually increased.
	ind_post_ਖਾਨਾ = (colnr_T)getwhitecols_curline();
	ਜੇ (curbuf->b_op_ਸ਼ੁਰੂ.ਖਾਨਾ > ind_pre_ਖਾਨਾ && ind_post_ਖਾਨਾ > ind_pre_ਖਾਨਾ)
	{
	    bd.textcol += ind_post_ਖਾਨਾ - ind_pre_ਖਾਨਾ;
	    ind_post_vcol = get_indent();
	    bd.ਸ਼ੁਰੂ_vcol += ind_post_vcol - ind_pre_vcol;
	    did_indent = TRUE;
	}

	// The user may have moved the cursor before inserting something, try
	// to adjust the block for that.  But only do it, if the difference
	// does not come from indent kicking in.
	ਜੇ (oap->ਸ਼ੁਰੂ.lnum == curbuf->b_op_ਸ਼ੁਰੂ_orig.lnum
						  && !bd.is_MAX && !did_indent)
	{
	    ਪੂਰਨ t = getviscol2(curbuf->b_op_ਸ਼ੁਰੂ_orig.ਖਾਨਾ,
					       curbuf->b_op_ਸ਼ੁਰੂ_orig.coladd);

	    ਜੇ (oap->op_type == OP_ਪਾਓ
		    && oap->ਸ਼ੁਰੂ.ਖਾਨਾ + oap->ਸ਼ੁਰੂ.coladd
			    != curbuf->b_op_ਸ਼ੁਰੂ_orig.ਖਾਨਾ
					      + curbuf->b_op_ਸ਼ੁਰੂ_orig.coladd)
	    {
		oap->ਸ਼ੁਰੂ.ਖਾਨਾ = curbuf->b_op_ਸ਼ੁਰੂ_orig.ਖਾਨਾ;
		pre_textlen -= t - oap->ਸ਼ੁਰੂ_vcol;
		oap->ਸ਼ੁਰੂ_vcol = t;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (oap->op_type == OP_APPEND
		    && oap->ਸ਼ੁਰੂ.ਖਾਨਾ + oap->ਸ਼ੁਰੂ.coladd
			    >= curbuf->b_op_ਸ਼ੁਰੂ_orig.ਖਾਨਾ
					      + curbuf->b_op_ਸ਼ੁਰੂ_orig.coladd)
	    {
		oap->ਸ਼ੁਰੂ.ਖਾਨਾ = curbuf->b_op_ਸ਼ੁਰੂ_orig.ਖਾਨਾ;
		// reset pre_textlen to the value of OP_INSERT
		pre_textlen += bd.textlen;
		pre_textlen -= t - oap->ਸ਼ੁਰੂ_vcol;
		oap->ਸ਼ੁਰੂ_vcol = t;
		oap->op_type = OP_ਪਾਓ;
	    }
	}

	// Spaces and tabs in the indent may have changed to other spaces and
	// tabs.  Get the starting column again and correct the length.
	// Don't do this when "$" used, end-of-line will have changed.
	//
	// if indent was added and the inserted text was after the indent,
	// correct the selection for the new indent.
	ਜੇ (did_indent && bd.textcol - ind_post_ਖਾਨਾ > 0)
	{
	    oap->ਸ਼ੁਰੂ.ਖਾਨਾ += ind_post_ਖਾਨਾ - ind_pre_ਖਾਨਾ;
	    oap->ਸ਼ੁਰੂ_vcol += ind_post_vcol - ind_pre_vcol;
	    oap->ਸਿਰਾ.ਖਾਨਾ += ind_post_ਖਾਨਾ - ind_pre_ਖਾਨਾ;
	    oap->ਸਿਰਾ_vcol += ind_post_vcol - ind_pre_vcol;
	}
	ਖੰਡ_prep(oap, &bd2, oap->ਸ਼ੁਰੂ.lnum, TRUE);
	ਜੇ (did_indent && bd.textcol - ind_post_ਖਾਨਾ > 0)
	{
	    // undo for where "oap" is used below
	    oap->ਸ਼ੁਰੂ.ਖਾਨਾ -= ind_post_ਖਾਨਾ - ind_pre_ਖਾਨਾ;
	    oap->ਸ਼ੁਰੂ_vcol -= ind_post_vcol - ind_pre_vcol;
	    oap->ਸਿਰਾ.ਖਾਨਾ -= ind_post_ਖਾਨਾ - ind_pre_ਖਾਨਾ;
	    oap->ਸਿਰਾ_vcol -= ind_post_vcol - ind_pre_vcol;
	}
	ਜੇ (!bd.is_MAX || bd2.textlen < bd.textlen)
	{
	    ਜੇ (oap->op_type == OP_APPEND)
	    {
		pre_textlen += bd2.textlen - bd.textlen;
		ਜੇ (bd2.endspaces)
		    --bd2.textlen;
	    }
	    bd.textcol = bd2.textcol;
	    bd.textlen = bd2.textlen;
	}

	/*
	 * Subsequent calls to ml_get() flush the firstline data - take a
	 * copy of the required string.
	 */
	firstline = ml_get(oap->ਸ਼ੁਰੂ.lnum);
	len = ml_get_len(oap->ਸ਼ੁਰੂ.lnum);
	ਜੋੜੋ = bd.textcol;
	ਜੇ (oap->op_type == OP_APPEND)
	{
	    ਜੋੜੋ += bd.textlen;
	    // account for pressing cursor in insert mode when '$' was used
	    ਜੇ (bd.is_MAX
		&& (ਸ਼ੁਰੂ_ਪਾਓ.lnum == Insstart.lnum
					   && ਸ਼ੁਰੂ_ਪਾਓ.ਖਾਨਾ > Insstart.ਖਾਨਾ))
	    {
		ਵਿੱਥ = (ਸ਼ੁਰੂ_ਪਾਓ.ਖਾਨਾ - Insstart.ਖਾਨਾ);
		ਜੋੜੋ -= ਵਿੱਥ;
		ਜੇ (oap->ਸਿਰਾ_vcol > ਵਿੱਥ)
		    oap->ਸਿਰਾ_vcol -= (ਵਿੱਥ + 1);
		ਨਹੀਂਤਾਂ
		    // moved outside of the visual block, what to do?
		    ਵਾਪਸ;
	    }
	}
	ਜੇ (ਜੋੜੋ > len)
	    ਜੋੜੋ = len;  // short line, point to the NUL
	firstline += ਜੋੜੋ;
	len -= ਜੋੜੋ;
	ਜੇ (pre_textlen >= 0 && (ins_len = (ਪੂਰਨ)len - pre_textlen - ਵਿੱਥ) > 0)
	{
	    ins_ਪਾਠ = vim_strnsave(firstline, ins_len);
	    ਜੇ (ins_ਪਾਠ != ਨਿਰਾ)
	    {
		// block handled here
		ਜੇ (u_save(oap->ਸ਼ੁਰੂ.lnum,
					 (linenr_T)(oap->ਸਿਰਾ.lnum + 1)) == OK)
		    ਖੰਡ_ਪਾਓ(oap, ins_ਪਾਠ, ins_len, (oap->op_type == OP_ਪਾਓ),
									 &bd);

		curwin->w_cursor.ਖਾਨਾ = oap->ਸ਼ੁਰੂ.ਖਾਨਾ;
		check_cursor();
		vim_free(ins_ਪਾਠ);
	    }
	}
    }
}

/*
 * op_change - handle a change operation
 *
 * return TRUE if edit() returns because of a CTRL-O command
 */
    ਪੂਰਨ
op_change(oparg_T *oap)
{
    colnr_T		l;
    ਪੂਰਨ			retval;
    linenr_T		linenr;
    ਲੰਮਾ		pre_textlen = 0;
    ਲੰਮਾ		pre_indent = 0;
    char_u		*firstline;
    char_u		*ins_ਪਾਠ, *newp, *oldp;
    ਬਣਤਰ ਖੰਡ_def	bd;

    l = oap->ਸ਼ੁਰੂ.ਖਾਨਾ;
    ਜੇ (oap->motion_type == MLINE)
    {
	l = 0;
	can_si = may_do_si();	// Like opening a new line, do smart indent
    }

    // First delete the text in the region.  In an empty buffer only need to
    // save for undo
    ਜੇ (curbuf->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ)
    {
	ਜੇ (u_save_cursor() == FAIL)
	    ਵਾਪਸ FALSE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (op_delete(oap) == FAIL)
	ਵਾਪਸ FALSE;

    ਜੇ ((l > curwin->w_cursor.ਖਾਨਾ) && !LINEEMPTY(curwin->w_cursor.lnum)
							 && !virtual_op)
	inc_cursor();

    // check for still on same line (<CR> in inserted text meaningless)
    // skip blank lines too
    ਜੇ (oap->ਖੰਡ_ਬਹੁਲਕ)
    {
	// Add spaces before getting the current line length.
	ਜੇ (virtual_op && (curwin->w_cursor.coladd > 0
						    || gchar_cursor() == NUL))
	    coladvance_force(getviscol());
	firstline = ml_get(oap->ਸ਼ੁਰੂ.lnum);
	pre_textlen = ml_get_len(oap->ਸ਼ੁਰੂ.lnum);
	pre_indent = (ਲੰਮਾ)getwhitecols(firstline);
	bd.textcol = curwin->w_cursor.ਖਾਨਾ;
    }

    ਜੇ (oap->motion_type == MLINE)
	fix_indent();

    // Reset finish_op now, don't want it set inside edit().
    ਪੂਰਨ save_ਅੰਤ_op = ਅੰਤ_op;
    ਅੰਤ_op = FALSE;

    retval = edit(NUL, FALSE, (linenr_T)1);

    ਅੰਤ_op = save_ਅੰਤ_op;

    /*
     * In Visual block mode, handle copying the new text to all lines of the
     * block.
     * Don't repeat the insert when Insert mode ended with CTRL-C.
     */
    ਜੇ (oap->ਖੰਡ_ਬਹੁਲਕ && oap->ਸ਼ੁਰੂ.lnum != oap->ਸਿਰਾ.lnum && !got_int)
    {
	ਪੂਰਨ	ins_len;

	// Auto-indenting may have changed the indent.  If the cursor was past
	// the indent, exclude that indent change from the inserted text.
	firstline = ml_get(oap->ਸ਼ੁਰੂ.lnum);
	ਜੇ (bd.textcol > (colnr_T)pre_indent)
	{
	    ਲੰਮਾ new_indent = (ਲੰਮਾ)getwhitecols(firstline);

	    pre_textlen += new_indent - pre_indent;
	    bd.textcol += new_indent - pre_indent;
	}

	ins_len = (ਪੂਰਨ)ml_get_len(oap->ਸ਼ੁਰੂ.lnum) - pre_textlen;
	ਜੇ (ins_len > 0)
	{
	    // Subsequent calls to ml_get() flush the firstline data - take a
	    // copy of the inserted text.
	    ਜੇ ((ins_ਪਾਠ = alloc(ins_len + 1)) != ਨਿਰਾ)
	    {
		vim_strncpy(ins_ਪਾਠ, firstline + bd.textcol, ins_len);
		ਲਈ (linenr = oap->ਸ਼ੁਰੂ.lnum + 1; linenr <= oap->ਸਿਰਾ.lnum;
								     linenr++)
		{
		    ਖੰਡ_prep(oap, &bd, linenr, TRUE);
		    ਜੇ (!bd.is_short || virtual_op)
		    {
			pos_T vpos;
			size_t newlen;

			// If the block starts in virtual space, count the
			// initial coladd offset as part of "startspaces"
			ਜੇ (bd.is_short)
			{
			    vpos.lnum = linenr;
			    (ਖਾਲੀ)getvpos(&vpos, oap->ਸ਼ੁਰੂ_vcol);
			}
			ਨਹੀਂਤਾਂ
			    vpos.coladd = 0;
			oldp = ml_get(linenr);
			newp = alloc(ml_get_len(linenr) + vpos.coladd + ins_len + 1);
			ਜੇ (newp == ਨਿਰਾ)
			    ਜਾਰੀ;
			// copy up to block start
			mch_memmove(newp, oldp, (size_t)bd.textcol);
			newlen = bd.textcol;
			vim_memset(newp + newlen, ' ', (size_t)vpos.coladd);
			newlen += vpos.coladd;
			mch_memmove(newp + newlen, ins_ਪਾਠ, ins_len);
			STRCPY(newp + newlen + ins_len, oldp + bd.textcol);
			ml_replace(linenr, newp, FALSE);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
			// Shift the properties for linenr as edit() would do.
			ਜੇ (curbuf->b_has_textprop)
			    adjust_prop_columns(linenr, bd.textcol,
						     vpos.coladd + (ਪੂਰਨ)ins_len, 0);
#ਜੇ_ਅੰਤ
		    }
		}
		check_cursor();

		ਬਦਲਿਆ_ਸਤਰਾਂ(oap->ਸ਼ੁਰੂ.lnum + 1, 0, oap->ਸਿਰਾ.lnum + 1, 0L);
	    }
	    vim_free(ins_ਪਾਠ);
	}
    }
    auto_format(FALSE, TRUE);

    ਵਾਪਸ retval;
}

/*
 * When the cursor is on the NUL past the end of the line and it should not be
 * there move it left.
 */
    ਖਾਲੀ
adjust_cursor_eol(ਖਾਲੀ)
{
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ cur_ve_flags = get_ve_flags();

    ਪੂਰਨ adj_cursor = (curwin->w_cursor.ਖਾਨਾ > 0
				&& gchar_cursor() == NUL
				&& (cur_ve_flags & VE_ONEMORE) == 0
				&& (cur_ve_flags & VE_ALL) == 0
				&& !(restart_edit || (State & ਬਹੁਲਕ_ਪਾਓ)));
    ਜੇ (!adj_cursor)
	ਵਾਪਸ;

    // Put the cursor on the last character in the line.
    dec_cursor();

    ਜੇ (cur_ve_flags == VE_ALL)
    {
	colnr_T	    scol, ecol;

	// Coladd is set to the width of the last character.
	getvcol(curwin, &curwin->w_cursor, &scol, ਨਿਰਾ, &ecol, 0);
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
    char_u   *ਸਤਰ,
    ਪੂਰਨ      process,
    ਪੂਰਨ	     include_space,
    ਪੂਰਨ      *is_comment)
{
    char_u *comment_flags = ਨਿਰਾ;
    ਪੂਰਨ    lead_len;
    ਪੂਰਨ    leader_ਵਿੱਥ = get_ਆਖਰੀ_leader_ਵਿੱਥ(ਸਤਰ, &comment_flags);

    *is_comment = FALSE;
    ਜੇ (leader_ਵਿੱਥ != -1)
    {
	// Let's check whether the line ends with an unclosed comment.
	// If the last comment leader has COM_END in flags, there's no comment.
	ਜਦੋਂ (*comment_flags)
	{
	    ਜੇ (*comment_flags == COM_ਸਿਰਾ
		    || *comment_flags == ':')
		ਰੋਕੋ;
	    ++comment_flags;
	}
	ਜੇ (*comment_flags != COM_ਸਿਰਾ)
	    *is_comment = TRUE;
    }

    ਜੇ (process == FALSE)
	ਵਾਪਸ ਸਤਰ;

    lead_len = get_leader_len(ਸਤਰ, &comment_flags, FALSE, include_space);

    ਜੇ (lead_len == 0)
	ਵਾਪਸ ਸਤਰ;

    // Find:
    // - COM_END,
    // - colon,
    // whichever comes first.
    ਜਦੋਂ (*comment_flags)
    {
	ਜੇ (*comment_flags == COM_ਸਿਰਾ
		|| *comment_flags == ':')
	    ਰੋਕੋ;
	++comment_flags;
    }

    // If we found a colon, it means that we are not processing a line
    // starting with a closing part of a three-part comment. That's good,
    // because we don't want to remove those as this would be annoying.
    ਜੇ (*comment_flags == ':' || *comment_flags == NUL)
	ਸਤਰ += lead_len;

    ਵਾਪਸ ਸਤਰ;
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
    ਪੂਰਨ
do_join(
    ਲੰਮਾ    ਗਿਣਤੀ_2,
    ਪੂਰਨ	    ਪਾਓ_space,
    ਪੂਰਨ	    save_undo,
    ਪੂਰਨ	    use_formatoptions ਅਣਵਰਤਿਆ,
    ਪੂਰਨ	    setmark)
{
    char_u	*curr = ਨਿਰਾ;
    char_u      *curr_ਸ਼ੁਰੂ = ਨਿਰਾ;
    char_u	*cend;
    char_u	*newp;
    size_t	newp_len;
    char_u	*spaces;	// number of spaces inserted before a line
    ਪੂਰਨ		endcurr1 = NUL;
    ਪੂਰਨ		endcurr2 = NUL;
    ਪੂਰਨ		currsize = 0;	// size of the current line
    ਪੂਰਨ		sumsize = 0;	// size of the long new line
    linenr_T	t;
    colnr_T	ਖਾਨਾ = 0;
    ਪੂਰਨ		ret = OK;
    ਪੂਰਨ		*comments = ਨਿਰਾ;
    ਪੂਰਨ		remove_comments = (use_formatoptions == TRUE)
				  && has_format_option(FO_REMOVE_COMS);
    ਪੂਰਨ		prev_was_comment;

    ਜੇ (save_undo && u_save((linenr_T)(curwin->w_cursor.lnum - 1),
			    (linenr_T)(curwin->w_cursor.lnum + ਗਿਣਤੀ_2)) == FAIL)
	ਵਾਪਸ FAIL;

    // Allocate an array to store the number of spaces inserted before each
    // line.  We will use it to pre-compute the length of the new line and the
    // proper placement of each original line in the new one.
    spaces = lalloc_clear(ਗਿਣਤੀ_2, TRUE);
    ਜੇ (spaces == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਜੇ (remove_comments)
    {
	comments = lalloc_clear(ਗਿਣਤੀ_2 * ਆਕਾਰ(ਪੂਰਨ), TRUE);
	ਜੇ (comments == ਨਿਰਾ)
	{
	    vim_free(spaces);
	    ਵਾਪਸ FAIL;
	}
    }

    /*
     * Don't move anything yet, just compute the final line length
     * and setup the array of space strings lengths
     * This loops forward over the joined lines.
     */
    ਲਈ (t = 0; t < ਗਿਣਤੀ_2; ++t)
    {
	curr = curr_ਸ਼ੁਰੂ = ml_get((linenr_T)(curwin->w_cursor.lnum + t));
	ਜੇ (t == 0 && setmark && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set the '[ mark.
	    curwin->w_ਬਫਰ->b_op_ਸ਼ੁਰੂ.lnum = curwin->w_cursor.lnum;
	    curwin->w_ਬਫਰ->b_op_ਸ਼ੁਰੂ.ਖਾਨਾ  = (colnr_T)STRLEN(curr);
	}
	ਜੇ (remove_comments)
	{
	    // We don't want to remove the comment leader if the
	    // previous line is not a comment.
	    ਜੇ (t > 0 && prev_was_comment)
	    {

		char_u *new_curr = skip_comment(curr, TRUE, ਪਾਓ_space,
							   &prev_was_comment);
		comments[t] = (ਪੂਰਨ)(new_curr - curr);
		curr = new_curr;
	    }
	    ਨਹੀਂਤਾਂ
		curr = skip_comment(curr, FALSE, ਪਾਓ_space,
							   &prev_was_comment);
	}

	ਜੇ (ਪਾਓ_space && t > 0)
	{
	    curr = skipwhite(curr);
	    ਜੇ (*curr != NUL && *curr != ')'
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
		ਜੇ (endcurr1 == ' ')
		    endcurr1 = endcurr2;
		ਨਹੀਂਤਾਂ
		    ++spaces[t];
		// extra space when 'joinspaces' set and line ends in '.'
		ਜੇ (       p_js
			&& (endcurr1 == '.'
			    || (vim_strchr(p_cpo, CPO_JOINSP) == ਨਿਰਾ
				&& (endcurr1 == '?' || endcurr1 == '!'))))
		    ++spaces[t];
	    }
	}
	currsize = (ਪੂਰਨ)STRLEN(curr);
	sumsize += currsize + spaces[t];
	endcurr1 = endcurr2 = NUL;
	ਜੇ (ਪਾਓ_space && currsize > 0)
	{
	    ਜੇ (has_mbyte)
	    {
		cend = curr + currsize;
		MB_PTR_ਪਿੱਠ(curr, cend);
		endcurr1 = (*mb_ptr2char)(cend);
		ਜੇ (cend > curr)
		{
		    MB_PTR_ਪਿੱਠ(curr, cend);
		    endcurr2 = (*mb_ptr2char)(cend);
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		endcurr1 = *(curr + currsize - 1);
		ਜੇ (currsize > 1)
		    endcurr2 = *(curr + currsize - 2);
	    }
	}
	ਸਤਰ_breakcheck();
	ਜੇ (got_int)
	{
	    ret = FAIL;
	    ਜਾਓ theend;
	}
    }

    // store the column position before last line
    ਖਾਨਾ = sumsize - currsize - spaces[ਗਿਣਤੀ_2 - 1];

    // allocate the space for the new line
    newp_len = sumsize + 1;
    newp = alloc(newp_len);
    ਜੇ (newp == ਨਿਰਾ)
    {
	ret = FAIL;
	ਜਾਓ theend;
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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    unpacked_memline_T um = um_open_at_no_props(
				curwin->w_ਬਫਰ, curwin->w_cursor.lnum, 0);
    // um_open_at_no_props may have invalidated "curr".
    ਪੂਰਨ curr_off = (ਪੂਰਨ)(curr - curr_ਸ਼ੁਰੂ);

    curr = curr_ਸ਼ੁਰੂ = ml_get((linenr_T)(curwin->w_cursor.lnum + ਗਿਣਤੀ_2 - 1));
    curr += curr_off;
#ਜੇ_ਅੰਤ

    ਲਈ (t = ਗਿਣਤੀ_2 - 1; ; --t)
    {
	ਪੂਰਨ spaces_removed;

	cend -= currsize;
	mch_memmove(cend, curr, (size_t)currsize);

	ਜੇ (spaces[t] > 0)
	{
	    cend -= spaces[t];
	    vim_memset(cend, ' ', (size_t)(spaces[t]));
	}

	// If deleting more spaces than adding, the cursor moves no more than
	// what is added if it is inside these spaces.
	spaces_removed = (curr - curr_ਸ਼ੁਰੂ) - spaces[t];

	mark_ਖਾਨਾ_adjust(curwin->w_cursor.lnum + t, (colnr_T)0, -t,
			 (ਲੰਮਾ)(cend - newp - spaces_removed), spaces_removed);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	prepend_joined_props(&um, curwin->w_cursor.lnum + t,
		t == ਗਿਣਤੀ_2 - 1, (ਲੰਮਾ)(cend - newp), spaces_removed);
#ਜੇ_ਅੰਤ
	ਜੇ (t == 0)
	    ਰੋਕੋ;
	curr = curr_ਸ਼ੁਰੂ = ml_get((linenr_T)(curwin->w_cursor.lnum + t - 1));
	ਜੇ (remove_comments)
	    curr += comments[t - 1];
	ਜੇ (ਪਾਓ_space && t > 1)
	    curr = skipwhite(curr);
	currsize = (ਪੂਰਨ)STRLEN(curr);
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਜੇ (um.buf != ਨਿਰਾ)
    {
	um_ਸੈੱਟ_ਪਾਠ(&um, newp);
	um_ਉਲਟ_props(&um);
	um_close(&um);
	newp = ਨਿਰਾ;  // um_set_text took ownership
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    ml_replace_len(curwin->w_cursor.lnum, newp, (colnr_T)newp_len, TRUE, FALSE);

    ਜੇ (setmark && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set the '] mark.
	curwin->w_ਬਫਰ->b_op_ਸਿਰਾ.lnum = curwin->w_cursor.lnum;
	curwin->w_ਬਫਰ->b_op_ਸਿਰਾ.ਖਾਨਾ  = (colnr_T)sumsize;
    }

    // Only report the change in the first line here, del_lines() will report
    // the deleted line.
    ਬਦਲਿਆ_ਸਤਰਾਂ(curwin->w_cursor.lnum, currsize,
					       curwin->w_cursor.lnum + 1, 0L);
    /*
     * Delete following lines. To do this we move the cursor there
     * briefly, and then move it back. After del_lines() the cursor may
     * have moved up (last line deleted), so the current lnum is kept in t.
     */
    t = curwin->w_cursor.lnum;
    ++curwin->w_cursor.lnum;
    del_ਸਤਰਾਂ(ਗਿਣਤੀ_2 - 1, FALSE);
    curwin->w_cursor.lnum = t;

    /*
     * Set the cursor column:
     * Vi compatible: use the column of the first join
     * vim:	      use the column of the last join
     */
    curwin->w_cursor.ਖਾਨਾ =
		    (vim_strchr(p_cpo, CPO_JOINCOL) != ਨਿਰਾ ? currsize : ਖਾਨਾ);
    check_cursor_ਖਾਨਾ();

    curwin->w_cursor.coladd = 0;
    curwin->w_ਸੈੱਟ_curswant = true;

theend:
    vim_free(spaces);
    ਜੇ (remove_comments)
	vim_free(comments);
    ਵਾਪਸ ret;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
/*
 * TODO: consider using "tailp" of win_lbr_chartabsize() instead of temporarily
 * resetting 'linebreak'.
 *
 * Reset 'linebreak' and take care of side effects.
 * Returns the previous value, to be passed to restore_lbr().
 */
    ਪੂਰਨ
reset_lbr(ਖਾਲੀ)
{
    ਜੇ (!curwin->w_p_lbr)
	ਵਾਪਸ FALSE;
    // changing 'linebreak' may require w_virtcol to be updated
    curwin->w_p_lbr = FALSE;
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);
    ਵਾਪਸ TRUE;
}

/*
 * Restore 'linebreak' and take care of side effects.
 */
    ਖਾਲੀ
restore_lbr(ਪੂਰਨ lbr_saved)
{
    ਜੇ (curwin->w_p_lbr || !lbr_saved)
	ਵਾਪਸ;

    // changing 'linebreak' may require w_virtcol to be updated
    curwin->w_p_lbr = TRUE;
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);
}
#ਜੇ_ਅੰਤ

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
    ਖਾਲੀ
ਖੰਡ_prep(
    oparg_T		*oap,
    ਬਣਤਰ ਖੰਡ_def	*bdp,
    linenr_T		lnum,
    ਪੂਰਨ			is_del)
{
    ਪੂਰਨ		incr = 0;
    char_u	*pend;
    char_u	*pstart;
    char_u	*ਸਤਰ;
    char_u	*prev_pstart;
    char_u	*prev_pend;
    chartabsize_T cts;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
		// Avoid a problem with unwanted linebreaks in block mode.
    ਪੂਰਨ		lbr_saved = reset_lbr();
#ਜੇ_ਅੰਤ

    bdp->startspaces = 0;
    bdp->endspaces = 0;
    bdp->textlen = 0;
    bdp->ਸ਼ੁਰੂ_vcol = 0;
    bdp->ਸਿਰਾ_vcol = 0;
    bdp->is_short = FALSE;
    bdp->is_oneChar = FALSE;
    bdp->pre_whitesp = 0;
    bdp->pre_whitesp_c = 0;
    bdp->ਸਿਰਾ_char_vcols = 0;
    bdp->ਸ਼ੁਰੂ_char_vcols = 0;

    ਸਤਰ = ml_get(lnum);
    prev_pstart = ਸਤਰ;
    init_chartabsize_arg(&cts, curwin, lnum, bdp->ਸ਼ੁਰੂ_vcol, ਸਤਰ, ਸਤਰ);
    ਜਦੋਂ (cts.cts_vcol < oap->ਸ਼ੁਰੂ_vcol && *cts.cts_ptr != NUL)
    {
	// Count a tab for what it's worth (if list mode not on)
	incr = lbr_chartabsize(&cts);
	cts.cts_vcol += incr;
	ਜੇ (VIM_ISWHITE(*cts.cts_ptr))
	{
	    bdp->pre_whitesp += incr;
	    bdp->pre_whitesp_c++;
	}
	ਨਹੀਂਤਾਂ
	{
	    bdp->pre_whitesp = 0;
	    bdp->pre_whitesp_c = 0;
	}
	prev_pstart = cts.cts_ptr;
	MB_PTR_ADV(cts.cts_ptr);
    }
    bdp->ਸ਼ੁਰੂ_vcol = cts.cts_vcol;
    pstart = cts.cts_ptr;
    clear_chartabsize_arg(&cts);

    bdp->ਸ਼ੁਰੂ_char_vcols = incr;
    ਜੇ (bdp->ਸ਼ੁਰੂ_vcol < oap->ਸ਼ੁਰੂ_vcol)	// line too short
    {
	bdp->ਸਿਰਾ_vcol = bdp->ਸ਼ੁਰੂ_vcol;
	bdp->is_short = TRUE;
	ਜੇ (!is_del || oap->op_type == OP_APPEND)
	    bdp->endspaces = oap->ਸਿਰਾ_vcol - oap->ਸ਼ੁਰੂ_vcol + 1;
    }
    ਨਹੀਂਤਾਂ
    {
	// notice: this converts partly selected Multibyte characters to
	// spaces, too.
	bdp->startspaces = bdp->ਸ਼ੁਰੂ_vcol - oap->ਸ਼ੁਰੂ_vcol;
	ਜੇ (is_del && bdp->startspaces)
	    bdp->startspaces = bdp->ਸ਼ੁਰੂ_char_vcols - bdp->startspaces;
	pend = pstart;
	bdp->ਸਿਰਾ_vcol = bdp->ਸ਼ੁਰੂ_vcol;
	ਜੇ (bdp->ਸਿਰਾ_vcol > oap->ਸਿਰਾ_vcol)	// it's all in one character
	{
	    bdp->is_oneChar = TRUE;
	    ਜੇ (oap->op_type == OP_ਪਾਓ)
		bdp->endspaces = bdp->ਸ਼ੁਰੂ_char_vcols - bdp->startspaces;
	    ਨਹੀਂਤਾਂ ਜੇ (oap->op_type == OP_APPEND)
	    {
		bdp->startspaces += oap->ਸਿਰਾ_vcol - oap->ਸ਼ੁਰੂ_vcol + 1;
		bdp->endspaces = bdp->ਸ਼ੁਰੂ_char_vcols - bdp->startspaces;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		bdp->startspaces = oap->ਸਿਰਾ_vcol - oap->ਸ਼ੁਰੂ_vcol + 1;
		ਜੇ (is_del && oap->op_type != OP_LSHIFT)
		{
		    // just putting the sum of those two into
		    // bdp->startspaces doesn't work for Visual replace,
		    // so we have to split the tab in two
		    bdp->startspaces = bdp->ਸ਼ੁਰੂ_char_vcols
					- (bdp->ਸ਼ੁਰੂ_vcol - oap->ਸ਼ੁਰੂ_vcol);
		    bdp->endspaces = bdp->ਸਿਰਾ_vcol - oap->ਸਿਰਾ_vcol - 1;
		}
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    init_chartabsize_arg(&cts, curwin, lnum, bdp->ਸਿਰਾ_vcol,
								  ਸਤਰ, pend);
	    prev_pend = pend;
	    ਜਦੋਂ (cts.cts_vcol <= oap->ਸਿਰਾ_vcol && *cts.cts_ptr != NUL)
	    {
		// count a tab for what it's worth (if list mode not on)
		prev_pend = cts.cts_ptr;
		incr = lbr_chartabsize_adv(&cts);
		cts.cts_vcol += incr;
	    }
	    bdp->ਸਿਰਾ_vcol = cts.cts_vcol;
	    pend = cts.cts_ptr;
	    clear_chartabsize_arg(&cts);

	    ਜੇ (bdp->ਸਿਰਾ_vcol <= oap->ਸਿਰਾ_vcol
		    && (!is_del
			|| oap->op_type == OP_APPEND
			|| oap->op_type == OP_REPLACE)) // line too short
	    {
		bdp->is_short = TRUE;
		// Alternative: include spaces to fill up the block.
		// Disadvantage: can lead to trailing spaces when the line is
		// short where the text is put
		// if (!is_del || oap->op_type == OP_APPEND)
		ਜੇ (oap->op_type == OP_APPEND || virtual_op)
		    bdp->endspaces = oap->ਸਿਰਾ_vcol - bdp->ਸਿਰਾ_vcol
							     + oap->inclusive;
		ਨਹੀਂਤਾਂ
		    bdp->endspaces = 0; // replace doesn't add characters
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (bdp->ਸਿਰਾ_vcol > oap->ਸਿਰਾ_vcol)
	    {
		bdp->endspaces = bdp->ਸਿਰਾ_vcol - oap->ਸਿਰਾ_vcol - 1;
		ਜੇ (!is_del && bdp->endspaces)
		{
		    bdp->endspaces = incr - bdp->endspaces;
		    ਜੇ (pend != pstart)
			pend = prev_pend;
		}
	    }
	}
	bdp->ਸਿਰਾ_char_vcols = incr;
	ਜੇ (is_del && bdp->startspaces)
	    pstart = prev_pstart;
	bdp->textlen = (ਪੂਰਨ)(pend - pstart);
    }
    bdp->textcol = (colnr_T) (pstart - ਸਤਰ);
    bdp->textstart = pstart;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    restore_lbr(lbr_saved);
#ਜੇ_ਅੰਤ
}

/*
 * Get block text from "start" to "end"
 */
    ਖਾਲੀ
charwise_ਖੰਡ_prep(
    pos_T		ਸ਼ੁਰੂ,
    pos_T		ਸਿਰਾ,
    ਬਣਤਰ ਖੰਡ_def	*bdp,
    linenr_T		lnum,
    ਪੂਰਨ			inclusive)
{
    colnr_T startcol = 0, endcol = MAXCOL;
    colnr_T cs, ce;
    char_u *p;
    ਪੂਰਨ	plen = ml_get_len(lnum);

    p = ml_get(lnum);
    bdp->startspaces = 0;
    bdp->endspaces = 0;
    bdp->is_oneChar = FALSE;
    bdp->ਸ਼ੁਰੂ_char_vcols = 0;

    ਜੇ (lnum == ਸ਼ੁਰੂ.lnum)
    {
	startcol = ਸ਼ੁਰੂ.ਖਾਨਾ;
	ਜੇ (virtual_op)
	{
	    getvcol(curwin, &ਸ਼ੁਰੂ, &cs, ਨਿਰਾ, &ce, 0);
	    ਜੇ (ce != cs && ਸ਼ੁਰੂ.coladd > 0)
	    {
		// Part of a tab selected -- but don't
		// double-count it.
		bdp->ਸ਼ੁਰੂ_char_vcols = ce - cs + 1;
		bdp->startspaces = bdp->ਸ਼ੁਰੂ_char_vcols - ਸ਼ੁਰੂ.coladd;
		ਜੇ (bdp->startspaces < 0)
		    bdp->startspaces = 0;
		startcol++;
	    }
	}
    }

    ਜੇ (lnum == ਸਿਰਾ.lnum)
    {
	endcol = ਸਿਰਾ.ਖਾਨਾ;
	ਜੇ (virtual_op)
	{
	    getvcol(curwin, &ਸਿਰਾ, &cs, ਨਿਰਾ, &ce, 0);
	    ਜੇ (p[endcol] == NUL || (cs + ਸਿਰਾ.coladd < ce
			// Don't add space for double-wide
			// char; endcol will be on last byte
			// of multi-byte char.
			&& (*mb_ਸਿਰ_off)(p, p + endcol) == 0))
	    {
		ਜੇ (ਸ਼ੁਰੂ.lnum == ਸਿਰਾ.lnum && ਸ਼ੁਰੂ.ਖਾਨਾ == ਸਿਰਾ.ਖਾਨਾ)
		{
		    // Special case: inside a single char
		    bdp->is_oneChar = TRUE;
		    bdp->startspaces = ਸਿਰਾ.coladd - ਸ਼ੁਰੂ.coladd + inclusive;
		    endcol = startcol;
		}
		ਨਹੀਂਤਾਂ
		{
		    bdp->endspaces = ਸਿਰਾ.coladd + inclusive;
		    endcol -= inclusive;
		}
	    }
	}
    }
    ਜੇ (endcol == MAXCOL)
	endcol = ml_get_len(lnum);
    ਜੇ (startcol > endcol || bdp->is_oneChar)
	bdp->textlen = 0;
    ਨਹੀਂਤਾਂ
	bdp->textlen = endcol - startcol + inclusive;
    bdp->textcol = startcol;
    bdp->textstart = startcol <= plen ? p + startcol : p;
}

/*
 * Handle the add/subtract operator.
 */
    ਖਾਲੀ
op_addsub(
    oparg_T	*oap,
    linenr_T	Prenum1,	    // Amount of add/subtract
    ਪੂਰਨ		g_cmd)		    // was g<c-a>/g<c-x>
{
    pos_T		pos;
    ਬਣਤਰ ਖੰਡ_def	bd;
    ਪੂਰਨ			change_cnt = 0;
    linenr_T		amount = Prenum1;

    // do_addsub() might trigger re-evaluation of 'foldexpr' halfway, when the
    // buffer is not completely updated yet. Postpone updating folds until before
    // the call to changed_lines().
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    disable_fold_update++;
#ਜੇ_ਅੰਤ

    ਜੇ (!VIsual_ਸਰਗਰਮ)
    {
	pos = curwin->w_cursor;
	ਜੇ (u_save_cursor() == FAIL)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    disable_fold_update--;
#ਜੇ_ਅੰਤ
	    ਵਾਪਸ;
	}
	change_cnt = do_addsub(oap->op_type, &pos, 0, amount);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	disable_fold_update--;
#ਜੇ_ਅੰਤ
	ਜੇ (change_cnt)
	    ਬਦਲਿਆ_ਸਤਰਾਂ(pos.lnum, 0, pos.lnum + 1, 0L);
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ	one_change;
	ਪੂਰਨ	ਲੰਮਾਈ;
	pos_T	startpos;

	ਜੇ (u_save((linenr_T)(oap->ਸ਼ੁਰੂ.lnum - 1),
					(linenr_T)(oap->ਸਿਰਾ.lnum + 1)) == FAIL)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    disable_fold_update--;
#ਜੇ_ਅੰਤ
	    ਵਾਪਸ;
	}

	pos = oap->ਸ਼ੁਰੂ;
	ਲਈ (; pos.lnum <= oap->ਸਿਰਾ.lnum; ++pos.lnum)
	{
	    ਜੇ (oap->ਖੰਡ_ਬਹੁਲਕ)		    // Visual block mode
	    {
		ਖੰਡ_prep(oap, &bd, pos.lnum, FALSE);
		pos.ਖਾਨਾ = bd.textcol;
		ਲੰਮਾਈ = bd.textlen;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (oap->motion_type == MLINE)
	    {
		curwin->w_cursor.ਖਾਨਾ = 0;
		pos.ਖਾਨਾ = 0;
		ਲੰਮਾਈ = ml_get_len(pos.lnum);
	    }
	    ਨਹੀਂਤਾਂ // oap->motion_type == MCHAR
	    {
		ਜੇ (pos.lnum == oap->ਸ਼ੁਰੂ.lnum && !oap->inclusive)
		    dec(&(oap->ਸਿਰਾ));
		ਲੰਮਾਈ = ml_get_len(pos.lnum);
		pos.ਖਾਨਾ = 0;
		ਜੇ (pos.lnum == oap->ਸ਼ੁਰੂ.lnum)
		{
		    pos.ਖਾਨਾ += oap->ਸ਼ੁਰੂ.ਖਾਨਾ;
		    ਲੰਮਾਈ -= oap->ਸ਼ੁਰੂ.ਖਾਨਾ;
		}
		ਜੇ (pos.lnum == oap->ਸਿਰਾ.lnum)
		{
		    ਲੰਮਾਈ = ml_get_len(oap->ਸਿਰਾ.lnum);
		    ਜੇ (oap->ਸਿਰਾ.ਖਾਨਾ >= ਲੰਮਾਈ)
			oap->ਸਿਰਾ.ਖਾਨਾ = ਲੰਮਾਈ - 1;
		    ਲੰਮਾਈ = oap->ਸਿਰਾ.ਖਾਨਾ - pos.ਖਾਨਾ + 1;
		}
	    }
	    one_change = do_addsub(oap->op_type, &pos, ਲੰਮਾਈ, amount);
	    ਜੇ (one_change)
	    {
		// Remember the start position of the first change.
		ਜੇ (change_cnt == 0)
		    startpos = curbuf->b_op_ਸ਼ੁਰੂ;
		++change_cnt;
	    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
	    ਜੇ (netbeans_ਸਰਗਰਮ() && one_change)
	    {
		char_u *ptr;

		netbeans_removed(curbuf, pos.lnum, pos.ਖਾਨਾ, (ਲੰਮਾ)ਲੰਮਾਈ);
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, pos.ਖਾਨਾ,
						&ptr[pos.ਖਾਨਾ], ਲੰਮਾਈ);
	    }
#ਜੇ_ਅੰਤ
	    ਜੇ (g_cmd && one_change)
		amount += Prenum1;
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	disable_fold_update--;
#ਜੇ_ਅੰਤ
	ਜੇ (change_cnt)
	    ਬਦਲਿਆ_ਸਤਰਾਂ(oap->ਸ਼ੁਰੂ.lnum, 0, oap->ਸਿਰਾ.lnum + 1, 0L);

	ਜੇ (!change_cnt && oap->is_VIsual)
	    // No change: need to remove the Visual selection
	    redraw_curbuf_later(UPD_INVERTED);

	// Set '[ mark if something changed. Keep the last end
	// position from do_addsub().
	ਜੇ (change_cnt > 0 && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	    curbuf->b_op_ਸ਼ੁਰੂ = startpos;

	ਜੇ (change_cnt > p_report)
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
    ਸਥਿਰ ਪੂਰਨ
do_addsub(
    ਪੂਰਨ		op_type,
    pos_T	*pos,
    ਪੂਰਨ		ਲੰਮਾਈ,
    linenr_T	Prenum1)
{
    ਪੂਰਨ		ਖਾਨਾ;
    ਪੂਰਨ		pre;		// 'X'/'x': hex; '0': octal; 'B'/'b': bin
    ਸਥਿਰ ਪੂਰਨ	hexupper = FALSE;	// 0xABC
    uvarnumber_T	n;
    uvarnumber_T	oldn;
    char_u	*ptr;
    ਪੂਰਨ		linelen;
    ਪੂਰਨ		c;
    ਪੂਰਨ		todel;
    ਪੂਰਨ		do_hex;
    ਪੂਰਨ		do_oct;
    ਪੂਰਨ		do_bin;
    ਪੂਰਨ		do_alpha;
    ਪੂਰਨ		do_unsigned;
    ਪੂਰਨ		do_blank;
    ਪੂਰਨ		blank_unsigned = FALSE;	// blank: treat as unsigned?
    ਪੂਰਨ		firstdigit;
    ਪੂਰਨ		ਘਟਾਓ;
    ਪੂਰਨ		negative = FALSE;
    ਪੂਰਨ		was_ਸਕਾਰਾਤਮਕ = TRUE;
    ਪੂਰਨ		visual = VIsual_ਸਰਗਰਮ;
    ਪੂਰਨ		did_change = FALSE;
    pos_T	save_cursor = curwin->w_cursor;
    ਪੂਰਨ		maxlen = 0;
    pos_T	startpos;
    pos_T	endpos;
    colnr_T	save_coladd = 0;

    do_hex = (vim_strchr(curbuf->b_p_nf, 'x') != ਨਿਰਾ);	// "heX"
    do_oct = (vim_strchr(curbuf->b_p_nf, 'o') != ਨਿਰਾ);	// "Octal"
    do_bin = (vim_strchr(curbuf->b_p_nf, 'b') != ਨਿਰਾ);	// "Bin"
    do_alpha = (vim_strchr(curbuf->b_p_nf, 'p') != ਨਿਰਾ);	// "alPha"
    do_unsigned = (vim_strchr(curbuf->b_p_nf, 'u') != ਨਿਰਾ);	// "Unsigned"
    do_blank = (vim_strchr(curbuf->b_p_nf, 'k') != ਨਿਰਾ);	// "blanK"

    ਜੇ (virtual_ਸਰਗਰਮ())
    {
	save_coladd = pos->coladd;
	pos->coladd = 0;
    }

    curwin->w_cursor = *pos;
    ptr = ml_get(pos->lnum);
    linelen = ml_get_len(pos->lnum);
    ਖਾਨਾ = pos->ਖਾਨਾ;

    ਜੇ (ਖਾਨਾ + !!save_coladd >= linelen)
	ਜਾਓ theend;

    /*
     * First check if we are on a hexadecimal number, after the "0x".
     */
    ਜੇ (!VIsual_ਸਰਗਰਮ)
    {
	ਜੇ (do_bin)
	    ਜਦੋਂ (ਖਾਨਾ > 0 && vim_isbdigit(ptr[ਖਾਨਾ]))
	    {
		--ਖਾਨਾ;
		ਜੇ (has_mbyte)
		    ਖਾਨਾ -= (*mb_ਸਿਰ_off)(ptr, ptr + ਖਾਨਾ);
	    }

	ਜੇ (do_hex)
	    ਜਦੋਂ (ਖਾਨਾ > 0 && vim_isxdigit(ptr[ਖਾਨਾ]))
	    {
		--ਖਾਨਾ;
		ਜੇ (has_mbyte)
		    ਖਾਨਾ -= (*mb_ਸਿਰ_off)(ptr, ptr + ਖਾਨਾ);
	    }

	ਜੇ (       do_bin
		&& do_hex
		&& ! ((ਖਾਨਾ > 0
		    && (ptr[ਖਾਨਾ] == 'X'
			|| ptr[ਖਾਨਾ] == 'x')
		    && ptr[ਖਾਨਾ - 1] == '0'
		    && (!has_mbyte ||
			!(*mb_ਸਿਰ_off)(ptr, ptr + ਖਾਨਾ - 1))
		    && vim_isxdigit(ptr[ਖਾਨਾ + 1]))))
	{

	    // In case of binary/hexadecimal pattern overlap match, rescan

	    ਖਾਨਾ = pos->ਖਾਨਾ;

	    ਜਦੋਂ (ਖਾਨਾ > 0 && vim_isdigit(ptr[ਖਾਨਾ]))
	    {
		ਖਾਨਾ--;
		ਜੇ (has_mbyte)
		    ਖਾਨਾ -= (*mb_ਸਿਰ_off)(ptr, ptr + ਖਾਨਾ);
	    }
	}

	ਜੇ ((       do_hex
		&& ਖਾਨਾ > 0
		&& (ptr[ਖਾਨਾ] == 'X'
		    || ptr[ਖਾਨਾ] == 'x')
		&& ptr[ਖਾਨਾ - 1] == '0'
		&& (!has_mbyte ||
		    !(*mb_ਸਿਰ_off)(ptr, ptr + ਖਾਨਾ - 1))
		&& vim_isxdigit(ptr[ਖਾਨਾ + 1])) ||
	    (       do_bin
		&& ਖਾਨਾ > 0
		&& (ptr[ਖਾਨਾ] == 'B'
		    || ptr[ਖਾਨਾ] == 'b')
		&& ptr[ਖਾਨਾ - 1] == '0'
		&& (!has_mbyte ||
		    !(*mb_ਸਿਰ_off)(ptr, ptr + ਖਾਨਾ - 1))
		&& vim_isbdigit(ptr[ਖਾਨਾ + 1])))
	{
	    // Found hexadecimal or binary number, move to its start.
	    --ਖਾਨਾ;
	    ਜੇ (has_mbyte)
		ਖਾਨਾ -= (*mb_ਸਿਰ_off)(ptr, ptr + ਖਾਨਾ);
	}
	ਨਹੀਂਤਾਂ
	{
	    /*
	     * Search forward and then backward to find the start of number.
	     */
	    ਖਾਨਾ = pos->ਖਾਨਾ;

	    ਜਦੋਂ (ptr[ਖਾਨਾ] != NUL
		    && !vim_isdigit(ptr[ਖਾਨਾ])
		    && !(do_alpha && ASCII_ISALPHA(ptr[ਖਾਨਾ])))
		ਖਾਨਾ += mb_ptr2len(ptr + ਖਾਨਾ);

	    ਜਦੋਂ (ਖਾਨਾ > 0
		    && vim_isdigit(ptr[ਖਾਨਾ - 1])
		    && !(do_alpha && ASCII_ISALPHA(ptr[ਖਾਨਾ])))
	    {
		--ਖਾਨਾ;
		ਜੇ (has_mbyte)
		    ਖਾਨਾ -= (*mb_ਸਿਰ_off)(ptr, ptr + ਖਾਨਾ);
	    }
	}
    }

    ਜੇ (visual)
    {
	ਜਦੋਂ (ptr[ਖਾਨਾ] != NUL && ਲੰਮਾਈ > 0
		&& !vim_isdigit(ptr[ਖਾਨਾ])
		&& !(do_alpha && ASCII_ISALPHA(ptr[ਖਾਨਾ])))
	{
	    ਪੂਰਨ mb_len = mb_ptr2len(ptr + ਖਾਨਾ);

	    ਖਾਨਾ += mb_len;
	    ਲੰਮਾਈ -= mb_len;
	}

	ਜੇ (ਲੰਮਾਈ == 0)
	    ਜਾਓ theend;

	ਜੇ (ਖਾਨਾ > pos->ਖਾਨਾ && ptr[ਖਾਨਾ - 1] == '-'
		&& (!has_mbyte || !(*mb_ਸਿਰ_off)(ptr, ptr + ਖਾਨਾ - 1))
		&& !do_unsigned)
	{
	    ਜੇ (do_blank && ਖਾਨਾ >= 2 && !VIM_ISWHITE(ptr[ਖਾਨਾ - 2]))
		blank_unsigned = TRUE;
	    ਨਹੀਂਤਾਂ
	    {
		negative = TRUE;
		was_ਸਕਾਰਾਤਮਕ = FALSE;
	    }
	}
    }

    /*
     * If a number was found, and saving for undo works, replace the number.
     */
    firstdigit = ptr[ਖਾਨਾ];
    ਜੇ (!VIM_ISDIGIT(firstdigit) && !(do_alpha && ASCII_ISALPHA(firstdigit)))
    {
	beep_flush();
	ਜਾਓ theend;
    }

    ਜੇ (do_alpha && ASCII_ISALPHA(firstdigit))
    {
	// decrement or increment alphabetic character
	ਜੇ (op_type == OP_NR_SUB)
	{
	    ਜੇ (CharOrd(firstdigit) < Prenum1)
	    {
		ਜੇ (ਸੁਰੱਖਿਅਤ_isupper(firstdigit))
		    firstdigit = 'A';
		ਨਹੀਂਤਾਂ
		    firstdigit = 'a';
	    }
	    ਨਹੀਂਤਾਂ
		firstdigit -= Prenum1;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (26 - CharOrd(firstdigit) - 1 < Prenum1)
	    {
		ਜੇ (ਸੁਰੱਖਿਅਤ_isupper(firstdigit))
		    firstdigit = 'Z';
		ਨਹੀਂਤਾਂ
		    firstdigit = 'z';
	    }
	    ਨਹੀਂਤਾਂ
		firstdigit += Prenum1;
	}
	curwin->w_cursor.ਖਾਨਾ = ਖਾਨਾ;
	ਜੇ (!did_change)
	    startpos = curwin->w_cursor;
	did_change = TRUE;
	(ਖਾਲੀ)del_char(FALSE);
	ins_char(firstdigit);
	endpos = curwin->w_cursor;
	curwin->w_cursor.ਖਾਨਾ = ਖਾਨਾ;
    }
    ਨਹੀਂਤਾਂ
    {
	char_u	*buf1;
	ਪੂਰਨ	buf1len;
	char_u	buf2[NUMBUFLEN];
	ਪੂਰਨ	buf2len;
	pos_T	save_pos;
	ਪੂਰਨ	i;

	ਜੇ (ਖਾਨਾ > 0 && ptr[ਖਾਨਾ - 1] == '-'
		&& (!has_mbyte ||
		    !(*mb_ਸਿਰ_off)(ptr, ptr + ਖਾਨਾ - 1))
		&& !visual
		&& !do_unsigned)
	{
	    ਜੇ (do_blank && ਖਾਨਾ >= 2 && !VIM_ISWHITE(ptr[ਖਾਨਾ - 2]))
		blank_unsigned = TRUE;
	    ਨਹੀਂਤਾਂ
	    {
		// negative number
		--ਖਾਨਾ;
		negative = TRUE;
	    }
	}

	// get the number value (unsigned)
	ਜੇ (visual && VIsual_ਬਹੁਲਕ != 'V')
	    maxlen = (curbuf->b_visual.vi_curswant == MAXCOL
		    ? linelen - ਖਾਨਾ : ਲੰਮਾਈ);

	ਪੂਰਨ ਉਛਾਲ = FALSE;
	vim_str2nr(ptr + ਖਾਨਾ, &pre, &ਲੰਮਾਈ,
		0 + (do_bin ? STR2NR_BIN : 0)
		    + (do_oct ? STR2NR_OCT : 0)
		    + (do_hex ? STR2NR_HEX : 0),
		ਨਿਰਾ, &n, maxlen, FALSE, &ਉਛਾਲ);

	// ignore leading '-' for hex and octal and bin numbers
	ਜੇ (pre && negative)
	{
	    ++ਖਾਨਾ;
	    --ਲੰਮਾਈ;
	    negative = FALSE;
	}
	// add or subtract
	ਘਟਾਓ = FALSE;
	ਜੇ (op_type == OP_NR_SUB)
	    ਘਟਾਓ ^= TRUE;
	ਜੇ (negative)
	    ਘਟਾਓ ^= TRUE;

	oldn = n;
	ਜੇ (!ਉਛਾਲ)  // if number is too big don't add/subtract
	{
	    ਜੇ (ਘਟਾਓ)
		n -= (uvarnumber_T)Prenum1;
	    ਨਹੀਂਤਾਂ
		n += (uvarnumber_T)Prenum1;
	}

	// handle wraparound for decimal numbers
	ਜੇ (!pre)
	{
	    ਜੇ (ਘਟਾਓ)
	    {
		ਜੇ (n > oldn)
		{
		    n = 1 + (n ^ (uvarnumber_T)-1);
		    negative ^= TRUE;
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		//ਜੋੜੋ
		ਜੇ (n < oldn)
		{
		    n = (n ^ (uvarnumber_T)-1);
		    negative ^= TRUE;
		}
	    }
	    ਜੇ (n == 0)
		negative = FALSE;
	}

	ਜੇ ((do_unsigned || blank_unsigned) && negative)
	{
	    ਜੇ (ਘਟਾਓ)
		// sticking at zero.
		n = (uvarnumber_T)0;
	    ਨਹੀਂਤਾਂ
		// sticking at 2^64 - 1.
		n = (uvarnumber_T)(-1);
	    negative = FALSE;
	}

	ਜੇ (visual && !was_ਸਕਾਰਾਤਮਕ && !negative && ਖਾਨਾ > 0)
	{
	    // need to remove the '-'
	    ਖਾਨਾ--;
	    ਲੰਮਾਈ++;
	}

	/*
	 * Delete the old number.
	 */
	curwin->w_cursor.ਖਾਨਾ = ਖਾਨਾ;
	ਜੇ (!did_change)
	    startpos = curwin->w_cursor;
	did_change = TRUE;
	todel = ਲੰਮਾਈ;
	c = gchar_cursor();
	/*
	 * Don't include the '-' in the length, only the length of the
	 * part after it is kept the same.
	 */
	ਜੇ (c == '-')
	    --ਲੰਮਾਈ;

	save_pos = curwin->w_cursor;
	ਲਈ (i = 0; i < todel; ++i)
	{
	    ਜੇ (c < 0x100 && ਸੁਰੱਖਿਅਤ_isalpha(c))
	    {
		ਜੇ (ਸੁਰੱਖਿਅਤ_isupper(c))
		    hexupper = TRUE;
		ਨਹੀਂਤਾਂ
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
	buf1 = alloc(ਲੰਮਾਈ + NUMBUFLEN);
	ਜੇ (buf1 == ਨਿਰਾ)
	    ਜਾਓ theend;
	ptr = buf1;
	ਜੇ (negative && (!visual || was_ਸਕਾਰਾਤਮਕ))
	    *ptr++ = '-';
	ਜੇ (pre)
	{
	    *ptr++ = '0';
	    --ਲੰਮਾਈ;
	}
	ਜੇ (pre == 'b' || pre == 'B' ||
	    pre == 'x' || pre == 'X')
	{
	    *ptr++ = pre;
	    --ਲੰਮਾਈ;
	}

	/*
	 * Put the number characters in buf2[].
	 */
	ਜੇ (pre == 'b' || pre == 'B')
	{
	    ਪੂਰਨ ਦੋਆਧਾਰੀ_ਅੰਕ = 0;
	    ਪੂਰਨ ਬਿੱਟ = ਆਕਾਰ(uvarnumber_T) * 8;

	    // leading zeros
	    ਲਈ (ਦੋਆਧਾਰੀ_ਅੰਕ = ਬਿੱਟ; ਦੋਆਧਾਰੀ_ਅੰਕ > 0; ਦੋਆਧਾਰੀ_ਅੰਕ--)
		ਜੇ ((n >> (ਦੋਆਧਾਰੀ_ਅੰਕ - 1)) & 0x1) ਰੋਕੋ;

	    ਲਈ (buf2len = 0; ਦੋਆਧਾਰੀ_ਅੰਕ > 0 && buf2len < (NUMBUFLEN - 1); ਦੋਆਧਾਰੀ_ਅੰਕ--)
		buf2[buf2len++] = ((n >> (ਦੋਆਧਾਰੀ_ਅੰਕ - 1)) & 0x1) ? '1' : '0';

	    buf2[buf2len] = NUL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (pre == 0)
	    buf2len = vim_snprintf((ਅੱਖਰ *)buf2, NUMBUFLEN, "%llu", n);
	ਨਹੀਂਤਾਂ ਜੇ (pre == '0')
	    buf2len = vim_snprintf((ਅੱਖਰ *)buf2, NUMBUFLEN, "%llo", n);
	ਨਹੀਂਤਾਂ ਜੇ (pre && hexupper)
	    buf2len = vim_snprintf((ਅੱਖਰ *)buf2, NUMBUFLEN, "%llX", n);
	ਨਹੀਂਤਾਂ
	    buf2len = vim_snprintf((ਅੱਖਰ *)buf2, NUMBUFLEN, "%llx", n);
	ਲੰਮਾਈ -= buf2len;

	/*
	 * Adjust number of zeros to the new number of digits, so the
	 * total length of the number remains the same.
	 * Don't do this when
	 * the result may look like an octal number.
	 */
	ਜੇ (firstdigit == '0' && !(do_oct && pre == 0))
	    ਜਦੋਂ (ਲੰਮਾਈ-- > 0)
		*ptr++ = '0';
	*ptr = NUL;
	buf1len = (ਪੂਰਨ)(ptr - buf1);

	STRCPY(buf1 + buf1len, buf2);
	buf1len += buf2len;

	// Insert just after the first character to be removed, so that any
	// text properties will be adjusted.  Then delete the old number
	// afterwards.
	save_pos = curwin->w_cursor;
	ਜੇ (todel > 0)
	    inc_cursor();
	ins_str(buf1, (size_t)buf1len);		// insert the new number
	vim_free(buf1);

	// del_char() will also mark line needing displaying
	ਜੇ (todel > 0)
	{
	    ਪੂਰਨ ਬਾਈਟਾਂ_after = ml_get_curline_len() - curwin->w_cursor.ਖਾਨਾ;

	    // Delete the one character before the insert.
	    curwin->w_cursor = save_pos;
	    (ਖਾਲੀ)del_char(FALSE);
	    curwin->w_cursor.ਖਾਨਾ = ml_get_curline_len() - ਬਾਈਟਾਂ_after;
	    --todel;
	}
	ਜਦੋਂ (todel-- > 0)
	    (ਖਾਲੀ)del_char(FALSE);

	endpos = curwin->w_cursor;
	ਜੇ (did_change && curwin->w_cursor.ਖਾਨਾ)
	    --curwin->w_cursor.ਖਾਨਾ;
    }

    ਜੇ (did_change && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// set the '[ and '] marks
	curbuf->b_op_ਸ਼ੁਰੂ = startpos;
	curbuf->b_op_ਸਿਰਾ = endpos;
	ਜੇ (curbuf->b_op_ਸਿਰਾ.ਖਾਨਾ > 0)
	    --curbuf->b_op_ਸਿਰਾ.ਖਾਨਾ;
    }

theend:
    ਜੇ (visual)
	curwin->w_cursor = save_cursor;
    ਨਹੀਂਤਾਂ ਜੇ (did_change)
	curwin->w_ਸੈੱਟ_curswant = true;
    ਨਹੀਂਤਾਂ ਜੇ (virtual_ਸਰਗਰਮ())
	curwin->w_cursor.coladd = save_coladd;

    ਵਾਪਸ did_change;
}

    ਖਾਲੀ
clear_oparg(oparg_T *oap)
{
    CLEAR_ਸੂਚਕ(oap);
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
    ਸਥਿਰ varnumber_T
ਸਤਰ_ਗਿਣਤੀ_info(
    char_u	*ਸਤਰ,
    varnumber_T	*wc,
    varnumber_T	*cc,
    varnumber_T	ਹੱਦਬੰਦੀ,
    ਪੂਰਨ		eol_ਮਾਪ)
{
    varnumber_T	i;
    varnumber_T	ਸ਼ਬਦ_3 = 0;
    varnumber_T	chars = 0;
    ਪੂਰਨ		is_ਸ਼ਬਦ = 0;

    ਲਈ (i = 0; i < ਹੱਦਬੰਦੀ && ਸਤਰ[i] != NUL; )
    {
	ਜੇ (is_ਸ਼ਬਦ)
	{
	    ਜੇ (vim_isspace(ਸਤਰ[i]))
	    {
		ਸ਼ਬਦ_3++;
		is_ਸ਼ਬਦ = 0;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (!vim_isspace(ਸਤਰ[i]))
	    is_ਸ਼ਬਦ = 1;
	++chars;
	i += (*mb_ptr2len)(ਸਤਰ + i);
    }

    ਜੇ (is_ਸ਼ਬਦ)
	ਸ਼ਬਦ_3++;
    *wc += ਸ਼ਬਦ_3;

    // Add eol_size if the end of line was reached before hitting limit.
    ਜੇ (i < ਹੱਦਬੰਦੀ && ਸਤਰ[i] == NUL)
    {
	i += eol_ਮਾਪ;
	chars += eol_ਮਾਪ;
    }
    *cc += chars;
    ਵਾਪਸ i;
}

/*
 * Give some info about the position of the cursor (for "g CTRL-G").
 * In Visual mode, give some info about the selected region.  (In this case,
 * the *_count_cursor variables store running totals for the selection.)
 * When "dict" is not NULL store the info there instead of showing it.
 */
    ਖਾਲੀ
cursor_pos_info(dict_T *dict)
{
    char_u	*p;
    char_u	buf1[50];
    char_u	buf2[40];
    linenr_T	lnum;
    varnumber_T	byte_ਗਿਣਤੀ = 0;
    varnumber_T	bom_ਗਿਣਤੀ  = 0;
    varnumber_T	byte_ਗਿਣਤੀ_cursor = 0;
    varnumber_T	char_ਗਿਣਤੀ = 0;
    varnumber_T	char_ਗਿਣਤੀ_cursor = 0;
    varnumber_T	ਸ਼ਬਦ_ਗਿਣਤੀ = 0;
    varnumber_T	ਸ਼ਬਦ_ਗਿਣਤੀ_cursor = 0;
    ਪੂਰਨ		eol_ਮਾਪ;
    varnumber_T	ਆਖਰੀ_check = 100000L;
    ਲੰਮਾ	ਸਤਰ_ਗਿਣਤੀ_selected = 0;
    pos_T	min_pos, max_pos;
    oparg_T	oparg;
    ਬਣਤਰ ਖੰਡ_def	bd;

    /*
     * Compute the length of the file in characters.
     */
    ਜੇ (curbuf->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ)
    {
	ਜੇ (dict == ਨਿਰਾ)
	{
	    ਸੁਨੇਹਾ(_(no_ਸਤਰਾਂ_ਸੁਨੇਹਾ));
	    ਵਾਪਸ;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (get_fileformat(curbuf) == EOL_DOS)
	    eol_ਮਾਪ = 2;
	ਨਹੀਂਤਾਂ
	    eol_ਮਾਪ = 1;

	ਜੇ (VIsual_ਸਰਗਰਮ)
	{
	    ਜੇ (LT_POS(VIsual, curwin->w_cursor))
	    {
		min_pos = VIsual;
		max_pos = curwin->w_cursor;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		min_pos = curwin->w_cursor;
		max_pos = VIsual;
	    }
	    ਜੇ (*p_sel == 'e' && max_pos.ਖਾਨਾ > 0)
		--max_pos.ਖਾਨਾ;

	    ਜੇ (VIsual_ਬਹੁਲਕ == Ctrl_V)
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
		char_u * saved_sbr = p_sbr;
		char_u * saved_w_sbr = curwin->w_p_sbr;

		// Make 'sbr' empty for a moment to get the correct size.
		p_sbr = ਖਾਲੀ_ਥਾਵਾਂ_option;
		curwin->w_p_sbr = ਖਾਲੀ_ਥਾਵਾਂ_option;
#ਜੇ_ਅੰਤ
		oparg.is_VIsual = 1;
		oparg.ਖੰਡ_ਬਹੁਲਕ = TRUE;
		oparg.op_type = OP_NOP;
		getvcols(curwin, &min_pos, &max_pos,
					&oparg.ਸ਼ੁਰੂ_vcol, &oparg.ਸਿਰਾ_vcol, 0);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
		p_sbr = saved_sbr;
		curwin->w_p_sbr = saved_w_sbr;
#ਜੇ_ਅੰਤ
		ਜੇ (curwin->w_curswant == MAXCOL)
		    oparg.ਸਿਰਾ_vcol = MAXCOL;
		// Swap the start, end vcol if needed
		ਜੇ (oparg.ਸਿਰਾ_vcol < oparg.ਸ਼ੁਰੂ_vcol)
		{
		    oparg.ਸਿਰਾ_vcol += oparg.ਸ਼ੁਰੂ_vcol;
		    oparg.ਸ਼ੁਰੂ_vcol = oparg.ਸਿਰਾ_vcol - oparg.ਸ਼ੁਰੂ_vcol;
		    oparg.ਸਿਰਾ_vcol -= oparg.ਸ਼ੁਰੂ_vcol;
		}
	    }
	    ਸਤਰ_ਗਿਣਤੀ_selected = max_pos.lnum - min_pos.lnum + 1;
	}

	ਲਈ (lnum = 1; lnum <= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ; ++lnum)
	{
	    // Check for a CTRL-C every 100000 characters.
	    ਜੇ (byte_ਗਿਣਤੀ > ਆਖਰੀ_check)
	    {
		ui_breakcheck();
		ਜੇ (got_int)
		    ਵਾਪਸ;
		ਆਖਰੀ_check = byte_ਗਿਣਤੀ + 100000L;
	    }

	    // Do extra processing for VIsual mode.
	    ਜੇ (VIsual_ਸਰਗਰਮ
		    && lnum >= min_pos.lnum && lnum <= max_pos.lnum)
	    {
		char_u	    *s = ਨਿਰਾ;
		ਲੰਮਾ	    len = 0L;

		ਚੋਣ (VIsual_ਬਹੁਲਕ)
		{
		    ਹਾਲਤ Ctrl_V:
			virtual_op = virtual_ਸਰਗਰਮ();
			ਖੰਡ_prep(&oparg, &bd, lnum, 0);
			virtual_op = MAYBE;
			s = bd.textstart;
			len = (ਲੰਮਾ)bd.textlen;
			ਰੋਕੋ;
		    ਹਾਲਤ 'V':
			s = ml_get(lnum);
			len = MAXCOL;
			ਰੋਕੋ;
		    ਹਾਲਤ 'v':
			{
			    colnr_T ਸ਼ੁਰੂ_ਖਾਨਾ = (lnum == min_pos.lnum)
							   ? min_pos.ਖਾਨਾ : 0;
			    colnr_T ਸਿਰਾ_ਖਾਨਾ = (lnum == max_pos.lnum)
				      ? max_pos.ਖਾਨਾ - ਸ਼ੁਰੂ_ਖਾਨਾ + 1 : MAXCOL;

			    s = ml_get(lnum) + ਸ਼ੁਰੂ_ਖਾਨਾ;
			    len = ਸਿਰਾ_ਖਾਨਾ;
			}
			ਰੋਕੋ;
		}
		ਜੇ (s != ਨਿਰਾ)
		{
		    byte_ਗਿਣਤੀ_cursor += ਸਤਰ_ਗਿਣਤੀ_info(s, &ਸ਼ਬਦ_ਗਿਣਤੀ_cursor,
					   &char_ਗਿਣਤੀ_cursor, len, eol_ਮਾਪ);
		    ਜੇ (lnum == curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
			    && !curbuf->b_p_eol
			    && (curbuf->b_p_bin || !curbuf->b_p_fixeol)
			    && (ਲੰਮਾ)STRLEN(s) < len)
			byte_ਗਿਣਤੀ_cursor -= eol_ਮਾਪ;
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// In non-visual mode, check for the line the cursor is on
		ਜੇ (lnum == curwin->w_cursor.lnum)
		{
		    ਸ਼ਬਦ_ਗਿਣਤੀ_cursor += ਸ਼ਬਦ_ਗਿਣਤੀ;
		    char_ਗਿਣਤੀ_cursor += char_ਗਿਣਤੀ;
		    byte_ਗਿਣਤੀ_cursor = byte_ਗਿਣਤੀ +
			ਸਤਰ_ਗਿਣਤੀ_info(ml_get(lnum),
				&ਸ਼ਬਦ_ਗਿਣਤੀ_cursor, &char_ਗਿਣਤੀ_cursor,
				(varnumber_T)(curwin->w_cursor.ਖਾਨਾ + 1),
				eol_ਮਾਪ);
		}
	    }
	    // Add to the running totals
	    byte_ਗਿਣਤੀ += ਸਤਰ_ਗਿਣਤੀ_info(ml_get(lnum), &ਸ਼ਬਦ_ਗਿਣਤੀ,
					 &char_ਗਿਣਤੀ, (varnumber_T)MAXCOL,
					 eol_ਮਾਪ);
	}

	// Correction for when last line doesn't have an EOL.
	ਜੇ (!curbuf->b_p_eol && (curbuf->b_p_bin || !curbuf->b_p_fixeol))
	    byte_ਗਿਣਤੀ -= eol_ਮਾਪ;

	ਜੇ (dict == ਨਿਰਾ)
	{
	    ਜੇ (VIsual_ਸਰਗਰਮ)
	    {
		ਜੇ (VIsual_ਬਹੁਲਕ == Ctrl_V && curwin->w_curswant < MAXCOL)
		{
		    getvcols(curwin, &min_pos, &max_pos,
						&min_pos.ਖਾਨਾ, &max_pos.ਖਾਨਾ, 0);
		    vim_snprintf((ਅੱਖਰ *)buf1, ਆਕਾਰ(buf1), _("%ld Cols; "),
			    (ਲੰਮਾ)(oparg.ਸਿਰਾ_vcol - oparg.ਸ਼ੁਰੂ_vcol + 1));
		}
		ਨਹੀਂਤਾਂ
		    buf1[0] = NUL;

		ਜੇ (char_ਗਿਣਤੀ_cursor == byte_ਗਿਣਤੀ_cursor
						    && char_ਗਿਣਤੀ == byte_ਗਿਣਤੀ)
		    vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE,
			    _("Selected %s%ld of %ld Lines; %lld of %lld Words; %lld of %lld Bytes"),
			    buf1, ਸਤਰ_ਗਿਣਤੀ_selected,
			    (ਲੰਮਾ)curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ,
			    ਸ਼ਬਦ_ਗਿਣਤੀ_cursor,
			    ਸ਼ਬਦ_ਗਿਣਤੀ,
			    byte_ਗਿਣਤੀ_cursor,
			    byte_ਗਿਣਤੀ);
		ਨਹੀਂਤਾਂ
		    vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE,
			    _("Selected %s%ld of %ld Lines; %lld of %lld Words; %lld of %lld Chars; %lld of %lld Bytes"),
			    buf1, ਸਤਰ_ਗਿਣਤੀ_selected,
			    (ਲੰਮਾ)curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ,
			    ਸ਼ਬਦ_ਗਿਣਤੀ_cursor,
			    ਸ਼ਬਦ_ਗਿਣਤੀ,
			    char_ਗਿਣਤੀ_cursor,
			    char_ਗਿਣਤੀ,
			    byte_ਗਿਣਤੀ_cursor,
			    byte_ਗਿਣਤੀ);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		p = ml_get_curline();
		validate_virtcol();
		ਖਾਨਾ_print(buf1, ਆਕਾਰ(buf1), (ਪੂਰਨ)curwin->w_cursor.ਖਾਨਾ + 1,
			(ਪੂਰਨ)curwin->w_virtcol + 1);
		ਖਾਨਾ_print(buf2, ਆਕਾਰ(buf2), ml_get_curline_len(),
							   linetabsize_str(p));

		ਜੇ (char_ਗਿਣਤੀ_cursor == byte_ਗਿਣਤੀ_cursor
			&& char_ਗਿਣਤੀ == byte_ਗਿਣਤੀ)
		    vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE,
			_("Col %s of %s; Line %ld of %ld; Word %lld of %lld; Byte %lld of %lld"),
			(ਅੱਖਰ *)buf1, (ਅੱਖਰ *)buf2,
			(ਲੰਮਾ)curwin->w_cursor.lnum,
			(ਲੰਮਾ)curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ,
			ਸ਼ਬਦ_ਗਿਣਤੀ_cursor, ਸ਼ਬਦ_ਗਿਣਤੀ,
			byte_ਗਿਣਤੀ_cursor, byte_ਗਿਣਤੀ);
		ਨਹੀਂਤਾਂ
		    vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE,
			_("Col %s of %s; Line %ld of %ld; Word %lld of %lld; Char %lld of %lld; Byte %lld of %lld"),
			(ਅੱਖਰ *)buf1, (ਅੱਖਰ *)buf2,
			(ਲੰਮਾ)curwin->w_cursor.lnum,
			(ਲੰਮਾ)curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ,
			ਸ਼ਬਦ_ਗਿਣਤੀ_cursor, ਸ਼ਬਦ_ਗਿਣਤੀ,
			char_ਗਿਣਤੀ_cursor, char_ਗਿਣਤੀ,
			byte_ਗਿਣਤੀ_cursor, byte_ਗਿਣਤੀ);
	    }
	}

	bom_ਗਿਣਤੀ = bomb_ਮਾਪ();
	ਜੇ (dict == ਨਿਰਾ && bom_ਗਿਣਤੀ > 0)
	{
	    size_t len = STRLEN(IObuff);

	    vim_snprintf((ਅੱਖਰ *)IObuff + len, IOSIZE - len,
				 _("(+%lld for BOM)"), bom_ਗਿਣਤੀ);
	}
	ਜੇ (dict == ਨਿਰਾ)
	{
	    // Don't shorten this message, the user asked for it.
	    p = p_shm;
	    p_shm = (char_u *)"";
	    ਸੁਨੇਹਾ((ਅੱਖਰ *)IObuff);
	    p_shm = p;
	}
    }
#ਜੇ defined(FEAT_EVAL)
    ਜੇ (dict != ਨਿਰਾ)
    {
	dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "words", ਸ਼ਬਦ_ਗਿਣਤੀ);
	dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "chars", char_ਗਿਣਤੀ);
	dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "bytes", byte_ਗਿਣਤੀ + bom_ਗਿਣਤੀ);
	dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, VIsual_ਸਰਗਰਮ ? "visual_bytes" : "cursor_bytes",
		byte_ਗਿਣਤੀ_cursor);
	dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, VIsual_ਸਰਗਰਮ ? "visual_chars" : "cursor_chars",
		char_ਗਿਣਤੀ_cursor);
	dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, VIsual_ਸਰਗਰਮ ? "visual_words" : "cursor_words",
		ਸ਼ਬਦ_ਗਿਣਤੀ_cursor);
    }
#ਜੇ_ਅੰਤ
}

/*
 * Handle indent and format operators and visual mode ":".
 */
    ਸਥਿਰ ਖਾਲੀ
op_colon(oparg_T *oap)
{
    stuffcharReadbuff(':');
    ਜੇ (oap->is_VIsual)
	stuffReadbuff((char_u *)"'<,'>");
    ਨਹੀਂਤਾਂ
    {
	// Make the range look nice, so it can be repeated.
	ਜੇ (oap->ਸ਼ੁਰੂ.lnum == curwin->w_cursor.lnum)
	    stuffcharReadbuff('.');
	ਨਹੀਂਤਾਂ
	    stuffnumReadbuff((ਲੰਮਾ)oap->ਸ਼ੁਰੂ.lnum);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	// When using !! on a closed fold the range ".!" works best to operate
	// on, it will be made the whole closed fold later.
	linenr_T ਸਿਰਾ_ਦਾ_ਸ਼ੁਰੂ_Fold = oap->ਸ਼ੁਰੂ.lnum;
	(ਖਾਲੀ)hasFolding(oap->ਸ਼ੁਰੂ.lnum, ਨਿਰਾ, &ਸਿਰਾ_ਦਾ_ਸ਼ੁਰੂ_Fold);
#ਜੇ_ਅੰਤ
	ਜੇ (oap->ਸਿਰਾ.lnum != oap->ਸ਼ੁਰੂ.lnum
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
		&& oap->ਸਿਰਾ.lnum != ਸਿਰਾ_ਦਾ_ਸ਼ੁਰੂ_Fold
#ਜੇ_ਅੰਤ
	   )
	{
	    // Make it a range with the end line.
	    stuffcharReadbuff(',');
	    ਜੇ (oap->ਸਿਰਾ.lnum == curwin->w_cursor.lnum)
		stuffcharReadbuff('.');
	    ਨਹੀਂਤਾਂ ਜੇ (oap->ਸਿਰਾ.lnum == curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		stuffcharReadbuff('$');
	    ਨਹੀਂਤਾਂ ਜੇ (oap->ਸ਼ੁਰੂ.lnum == curwin->w_cursor.lnum
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
		    // do not use ".+number" for a closed fold, it would count
		    // folded lines twice
		    && !hasFolding(oap->ਸਿਰਾ.lnum, ਨਿਰਾ, ਨਿਰਾ)
#ਜੇ_ਅੰਤ
		    )
	    {
		stuffReadbuff((char_u *)".+");
		stuffnumReadbuff((ਲੰਮਾ)oap->ਸਤਰ_ਗਿਣਤੀ - 1);
	    }
	    ਨਹੀਂਤਾਂ
		stuffnumReadbuff((ਲੰਮਾ)oap->ਸਿਰਾ.lnum);
	}
    }
    ਜੇ (oap->op_type != OP_COLON)
	stuffReadbuff((char_u *)"!");
    ਜੇ (oap->op_type == OP_INDENT)
    {
	ਜੇ (*get_equalprg() == NUL)
	    stuffReadbuff((char_u *)"indent");
	ਨਹੀਂਤਾਂ
	    stuffReadbuff(get_equalprg());
	stuffReadbuff((char_u *)"\n");
    }
    ਨਹੀਂਤਾਂ ਜੇ (oap->op_type == OP_FORMAT)
    {
	ਜੇ (*curbuf->b_p_fp != NUL)
	    stuffReadbuff(curbuf->b_p_fp);
	ਨਹੀਂਤਾਂ ਜੇ (*p_fp != NUL)
	    stuffReadbuff(p_fp);
	ਨਹੀਂਤਾਂ
	    stuffReadbuff((char_u *)"fmt");
	stuffReadbuff((char_u *)"\n']");
    }

    // do_cmdline() does the rest
}

// callback function for 'operatorfunc'
ਸਥਿਰ callback_T opfunc_cb;

/*
 * Process the 'operatorfunc' option value.
 * Returns OK or FAIL.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_operatorfunc(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਜੇ (option_ਸੈੱਟ_callback_func(p_opfunc, &opfunc_cb) == FAIL)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

    ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ defined(EXITFREE)
    ਖਾਲੀ
free_operatorfunc_option(ਖਾਲੀ)
{
# ifdef FEAT_EVAL
    free_callback(&opfunc_cb);
# endif
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_EVAL)
/*
 * Mark the global 'operatorfunc' callback with "copyID" so that it is not
 * garbage collected.
 */
    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_opfunc(ਪੂਰਨ copyID ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ abort = FALSE;

    abort = ਸੈੱਟ_ref_ਵਿੱਚ_callback(&opfunc_cb, copyID);

    ਵਾਪਸ abort;
}
#ਜੇ_ਅੰਤ

/*
 * Handle the "g@" operator: call 'operatorfunc'.
 */
    ਸਥਿਰ ਖਾਲੀ
op_function(oparg_T *oap ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    typval_T	argv[2];
    pos_T	orig_ਸ਼ੁਰੂ = curbuf->b_op_ਸ਼ੁਰੂ;
    pos_T	orig_ਸਿਰਾ = curbuf->b_op_ਸਿਰਾ;
    typval_T	rettv;

    ਜੇ (*p_opfunc == NUL)
	emsg(_(e_operatorfunc_is_ਖਾਲੀ_ਥਾਵਾਂ));
    ਨਹੀਂਤਾਂ
    {
	// Set '[ and '] marks to text to be operated on.
	curbuf->b_op_ਸ਼ੁਰੂ = oap->ਸ਼ੁਰੂ;
	curbuf->b_op_ਸਿਰਾ = oap->ਸਿਰਾ;
	ਜੇ (oap->motion_type != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_ਸਿਰਾ);

	argv[0].v_type = VAR_STRING;
	ਜੇ (oap->ਖੰਡ_ਬਹੁਲਕ)
	    argv[0].vval.v_string = (char_u *)"block";
	ਨਹੀਂਤਾਂ ਜੇ (oap->motion_type == MLINE)
	    argv[0].vval.v_string = (char_u *)"line";
	ਨਹੀਂਤਾਂ
	    argv[0].vval.v_string = (char_u *)"char";
	argv[1].v_type = VAR_UNKNOWN;

	// Reset virtual_op so that 'virtualedit' can be changed in the
	// function.
	ਪੂਰਨ save_virtual_op = virtual_op;
	virtual_op = MAYBE;

	// Reset finish_op so that mode() returns the right value.
	ਪੂਰਨ save_ਅੰਤ_op = ਅੰਤ_op;
	ਅੰਤ_op = FALSE;

	ਜੇ (call_callback(&opfunc_cb, 0, &rettv, 1, argv) != FAIL)
	    clear_tv(&rettv);

	virtual_op = save_virtual_op;
	ਅੰਤ_op = save_ਅੰਤ_op;
	ਜੇ (cmdmod.cmod_flags & CMOD_LOCKMARKS)
	{
	    curbuf->b_op_ਸ਼ੁਰੂ = orig_ਸ਼ੁਰੂ;
	    curbuf->b_op_ਸਿਰਾ = orig_ਸਿਰਾ;
	}
    }
#ਨਹੀਂਤਾਂ
    emsg(_(e_eval_feature_not_ਉਪਲਬਧ));
#ਜੇ_ਅੰਤ
}

/*
 * Calculate start/end virtual columns for operating in block mode.
 */
    ਸਥਿਰ ਖਾਲੀ
get_op_vcol(
    oparg_T	*oap,
    colnr_T	redo_VIsual_vcol,
    ਪੂਰਨ		initial)    // when TRUE adjust position for 'selectmode'
{
    colnr_T	    ਸ਼ੁਰੂ, ਸਿਰਾ;

    ਜੇ (VIsual_ਬਹੁਲਕ != Ctrl_V
	    || (!initial && oap->ਸਿਰਾ.ਖਾਨਾ < curwin->w_ਚੌੜਾਈ))
	ਵਾਪਸ;

    oap->ਖੰਡ_ਬਹੁਲਕ = TRUE;

    // prevent from moving onto a trail byte
    ਜੇ (has_mbyte)
	mb_adjustpos(curwin->w_ਬਫਰ, &oap->ਸਿਰਾ);

    getvvcol(curwin, &(oap->ਸ਼ੁਰੂ), &oap->ਸ਼ੁਰੂ_vcol, ਨਿਰਾ, &oap->ਸਿਰਾ_vcol, 0);

    ਜੇ (!redo_VIsual_busy)
    {
	getvvcol(curwin, &(oap->ਸਿਰਾ), &ਸ਼ੁਰੂ, ਨਿਰਾ, &ਸਿਰਾ, 0);

	ਜੇ (ਸ਼ੁਰੂ < oap->ਸ਼ੁਰੂ_vcol)
	    oap->ਸ਼ੁਰੂ_vcol = ਸ਼ੁਰੂ;
	ਜੇ (ਸਿਰਾ > oap->ਸਿਰਾ_vcol)
	{
	    ਜੇ (initial && *p_sel == 'e' && ਸ਼ੁਰੂ >= 1
				    && ਸ਼ੁਰੂ - 1 >= oap->ਸਿਰਾ_vcol)
		oap->ਸਿਰਾ_vcol = ਸ਼ੁਰੂ - 1;
	    ਨਹੀਂਤਾਂ
		oap->ਸਿਰਾ_vcol = ਸਿਰਾ;
	}
    }

    // if '$' was used, get oap->end_vcol from longest line
    ਜੇ (curwin->w_curswant == MAXCOL)
    {
	curwin->w_cursor.ਖਾਨਾ = MAXCOL;
	oap->ਸਿਰਾ_vcol = 0;
	ਲਈ (curwin->w_cursor.lnum = oap->ਸ਼ੁਰੂ.lnum;
		curwin->w_cursor.lnum <= oap->ਸਿਰਾ.lnum;
					++curwin->w_cursor.lnum)
	{
	    getvvcol(curwin, &curwin->w_cursor, ਨਿਰਾ, ਨਿਰਾ, &ਸਿਰਾ, 0);
	    ਜੇ (ਸਿਰਾ > oap->ਸਿਰਾ_vcol)
		oap->ਸਿਰਾ_vcol = ਸਿਰਾ;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (redo_VIsual_busy)
	oap->ਸਿਰਾ_vcol = oap->ਸ਼ੁਰੂ_vcol + redo_VIsual_vcol - 1;
    // Correct oap->end.col and oap->start.col to be the
    // upper-left and lower-right corner of the block area.
    //
    // (Actually, this does convert column positions into character
    // positions)
    curwin->w_cursor.lnum = oap->ਸਿਰਾ.lnum;
    coladvance(oap->ਸਿਰਾ_vcol);
    oap->ਸਿਰਾ = curwin->w_cursor;

    curwin->w_cursor = oap->ਸ਼ੁਰੂ;
    coladvance(oap->ਸ਼ੁਰੂ_vcol);
    oap->ਸ਼ੁਰੂ = curwin->w_cursor;
}

// Information for redoing the previous Visual selection.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਪੂਰਨ		rv_ਬਹੁਲਕ;	// 'v', 'V', or Ctrl-V
    linenr_T	rv_ਸਤਰ_ਗਿਣਤੀ;	// number of lines
    colnr_T	rv_vcol;	// number of cols or end column
    ਲੰਮਾ	rv_ਗਿਣਤੀ;	// count for Visual operator
    ਪੂਰਨ		rv_arg;		// extra argument
} redo_VIsual_T;

    ਸਥਿਰ ਪੂਰਨ
is_ex_cmdchar(cmdarg_T *cap)
{
    ਵਾਪਸ cap->cmdchar == ':'
	|| cap->cmdchar == K_COMMAND
	|| cap->cmdchar == K_SCRIPT_COMMAND;
}

/*
 * Handle an operator after Visual mode or when the movement is finished.
 * "gui_yank" is true when yanking text for the clipboard.
 */
    ਖਾਲੀ
do_pending_operator(cmdarg_T *cap, ਪੂਰਨ old_ਖਾਨਾ, ਪੂਰਨ gui_yank)
{
    oparg_T	*oap = cap->oap;
    pos_T	old_cursor;
    ਪੂਰਨ		ਖਾਲੀ_ਥਾਵਾਂ_region_error;
    ਪੂਰਨ		restart_edit_save;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    ਪੂਰਨ		lbr_saved = curwin->w_p_lbr;
#ਜੇ_ਅੰਤ

    // The visual area is remembered for redo
    ਸਥਿਰ redo_VIsual_T   redo_VIsual = {NUL, 0, 0, 0,0};

    ਪੂਰਨ		    include_ਸਤਰ_break = FALSE;

#ਜੇ defined(FEAT_CLIPBOARD)
    // Yank the visual area into the GUI selection register before we operate
    // on it and lose it forever.
    // Don't do it if a specific register was specified, so that ""x"*P works.
    // This could call do_pending_operator() recursively, but that's OK
    // because gui_yank will be TRUE for the nested call.
    ਜੇ ((clip_ਤਾਰਾ.ਉਪਲਬਧ || clip_plus.ਉਪਲਬਧ)
	    && oap->op_type != OP_NOP
	    && !gui_yank
	    && VIsual_ਸਰਗਰਮ
	    && !redo_VIsual_busy
	    && oap->regname == 0)
	clip_auto_select();
#ਜੇ_ਅੰਤ
    old_cursor = curwin->w_cursor;

    // If an operation is pending, handle it...
    ਜੇ ((ਅੰਤ_op || VIsual_ਸਰਗਰਮ) && oap->op_type != OP_NOP)
    {
	// Yank can be redone when 'y' is in 'cpoptions', but not when yanking
	// for the clipboard.
	ਪੂਰਨ	redo_yank = vim_strchr(p_cpo, CPO_YANK) != ਨਿਰਾ && !gui_yank;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
	// Avoid a problem with unwanted linebreaks in block mode.
	(ਖਾਲੀ)reset_lbr();
#ਜੇ_ਅੰਤ
	oap->is_VIsual = VIsual_ਸਰਗਰਮ;
	ਜੇ (oap->motion_force == 'V')
	    oap->motion_type = MLINE;
	ਨਹੀਂਤਾਂ ਜੇ (oap->motion_force == 'v')
	{
	    // If the motion was linewise, "inclusive" will not have been set.
	    // Use "exclusive" to be consistent.  Makes "dvj" work nice.
	    ਜੇ (oap->motion_type == MLINE)
		oap->inclusive = FALSE;
	    // If the motion already was characterwise, toggle "inclusive"
	    ਨਹੀਂਤਾਂ ਜੇ (oap->motion_type == MCHAR)
		oap->inclusive = !oap->inclusive;
	    oap->motion_type = MCHAR;
	}
	ਨਹੀਂਤਾਂ ਜੇ (oap->motion_force == Ctrl_V)
	{
	    // Change line- or characterwise motion into Visual block mode.
	    ਜੇ (!VIsual_ਸਰਗਰਮ)
	    {
		VIsual_ਸਰਗਰਮ = TRUE;
		VIsual = oap->ਸ਼ੁਰੂ;
	    }
	    VIsual_ਬਹੁਲਕ = Ctrl_V;
	    VIsual_select = FALSE;
	    VIsual_reselect = FALSE;
	}

	// Only redo yank when 'y' flag is in 'cpoptions'.
	// Never redo "zf" (define fold).
	ਜੇ ((redo_yank || oap->op_type != OP_YANK)
		&& ((!VIsual_ਸਰਗਰਮ || oap->motion_force)
		    // Also redo Operator-pending Visual mode mappings
		    || (VIsual_ਸਰਗਰਮ
			    && is_ex_cmdchar(cap) && oap->op_type != OP_COLON))
		&& cap->cmdchar != 'D'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
		&& oap->op_type != OP_FOLD
		&& oap->op_type != OP_FOLDOPEN
		&& oap->op_type != OP_FOLDOPENREC
		&& oap->op_type != OP_FOLDCLOSE
		&& oap->op_type != OP_FOLDCLOSEREC
		&& oap->op_type != OP_FOLDDEL
		&& oap->op_type != OP_FOLDDELREC
#ਜੇ_ਅੰਤ
		)
	{
	    prep_redo(oap->regname, cap->count0,
		    get_op_char(oap->op_type), get_extra_op_char(oap->op_type),
		    oap->motion_force, cap->cmdchar, cap->nchar);
	    ਜੇ (cap->cmdchar == '/' || cap->cmdchar == '?') // was a search
	    {
		// If 'cpoptions' does not contain 'r', insert the search
		// pattern to really repeat the same command.
		ਜੇ (vim_strchr(p_cpo, CPO_REDO) == ਨਿਰਾ)
		    Append_ਮੰਜ਼ਿਲ_Redobuff_Lit(cap->searchbuf, -1);
		Append_ਮੰਜ਼ਿਲ_Redobuff(NL_STR);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (is_ex_cmdchar(cap))
	    {
		// do_cmdline() has stored the first typed line in
		// "repeat_cmdline".  When several lines are typed repeating
		// won't be possible.
		ਜੇ (repeat_cmdline == ਨਿਰਾ)
		    ResetRedobuff();
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (cap->cmdchar == ':')
			Append_ਮੰਜ਼ਿਲ_Redobuff_Lit(repeat_cmdline, -1);
		    ਨਹੀਂਤਾਂ
			Append_ਮੰਜ਼ਿਲ_Redobuff_Spec(repeat_cmdline);
		    Append_ਮੰਜ਼ਿਲ_Redobuff(NL_STR);
		    VIM_CLEAR(repeat_cmdline);
		}
	    }
	}

	ਜੇ (redo_VIsual_busy)
	{
	    // Redo of an operation on a Visual area. Use the same size from
	    // redo_VIsual.rv_line_count and redo_VIsual.rv_vcol.
	    oap->ਸ਼ੁਰੂ = curwin->w_cursor;
	    curwin->w_cursor.lnum += redo_VIsual.rv_ਸਤਰ_ਗਿਣਤੀ - 1;
	    ਜੇ (curwin->w_cursor.lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	    VIsual_ਬਹੁਲਕ = redo_VIsual.rv_ਬਹੁਲਕ;
	    ਜੇ (redo_VIsual.rv_vcol == MAXCOL || VIsual_ਬਹੁਲਕ == 'v')
	    {
		ਜੇ (VIsual_ਬਹੁਲਕ == 'v')
		{
		    ਜੇ (redo_VIsual.rv_ਸਤਰ_ਗਿਣਤੀ <= 1)
		    {
			validate_virtcol();
			curwin->w_curswant =
				     curwin->w_virtcol + redo_VIsual.rv_vcol - 1;
		    }
		    ਨਹੀਂਤਾਂ
			curwin->w_curswant = redo_VIsual.rv_vcol;
		}
		ਨਹੀਂਤਾਂ
		{
		    curwin->w_curswant = MAXCOL;
		}
		coladvance(curwin->w_curswant);
	    }
	    cap->count0 = redo_VIsual.rv_ਗਿਣਤੀ;
	    ਜੇ (redo_VIsual.rv_ਗਿਣਤੀ != 0)
		cap->count1 = redo_VIsual.rv_ਗਿਣਤੀ;
	    ਨਹੀਂਤਾਂ
		cap->count1 = 1;
	}
	ਨਹੀਂਤਾਂ ਜੇ (VIsual_ਸਰਗਰਮ)
	{
	    ਜੇ (!gui_yank)
	    {
		// Save the current VIsual area for '< and '> marks, and "gv"
		curbuf->b_visual.vi_ਸ਼ੁਰੂ = VIsual;
		curbuf->b_visual.vi_ਸਿਰਾ = curwin->w_cursor;
		curbuf->b_visual.vi_ਬਹੁਲਕ = VIsual_ਬਹੁਲਕ;
		restore_visual_ਬਹੁਲਕ();
		curbuf->b_visual.vi_curswant = curwin->w_curswant;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		curbuf->b_visual_ਬਹੁਲਕ_eval = VIsual_ਬਹੁਲਕ;
#ਜੇ_ਅੰਤ
	    }

	    // In Select mode, a linewise selection is operated upon like a
	    // characterwise selection.
	    // Special case: gH<Del> deletes the last line.
	    ਜੇ (VIsual_select && VIsual_ਬਹੁਲਕ == 'V'
					    && cap->oap->op_type != OP_DELETE)
	    {
		ਜੇ (LT_POS(VIsual, curwin->w_cursor))
		{
		    VIsual.ਖਾਨਾ = 0;
		    curwin->w_cursor.ਖਾਨਾ = ml_get_len(curwin->w_cursor.lnum);
		}
		ਨਹੀਂਤਾਂ
		{
		    curwin->w_cursor.ਖਾਨਾ = 0;
		    VIsual.ਖਾਨਾ = ml_get_len(VIsual.lnum);
		}
		VIsual_ਬਹੁਲਕ = 'v';
	    }
	    // If 'selection' is "exclusive", backup one character for
	    // charwise selections.
	    ਨਹੀਂਤਾਂ ਜੇ (VIsual_ਬਹੁਲਕ == 'v')
		include_ਸਤਰ_break = unadjust_for_sel();

	    oap->ਸ਼ੁਰੂ = VIsual;
	    ਜੇ (VIsual_ਬਹੁਲਕ == 'V')
	    {
		oap->ਸ਼ੁਰੂ.ਖਾਨਾ = 0;
		oap->ਸ਼ੁਰੂ.coladd = 0;
	    }
	}

	// Set oap->start to the first position of the operated text, oap->end
	// to the end of the operated text.  w_cursor is equal to oap->start.
	ਜੇ (LT_POS(oap->ਸ਼ੁਰੂ, curwin->w_cursor))
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    // Include folded lines completely.
	    ਜੇ (!VIsual_ਸਰਗਰਮ)
	    {
		ਜੇ (hasFolding(oap->ਸ਼ੁਰੂ.lnum, &oap->ਸ਼ੁਰੂ.lnum, ਨਿਰਾ))
		    oap->ਸ਼ੁਰੂ.ਖਾਨਾ = 0;
		ਜੇ ((curwin->w_cursor.ਖਾਨਾ > 0 || oap->inclusive
						  || oap->motion_type == MLINE)
			&& hasFolding(curwin->w_cursor.lnum, ਨਿਰਾ,
						      &curwin->w_cursor.lnum))
		    curwin->w_cursor.ਖਾਨਾ = ml_get_curline_len();
	    }
#ਜੇ_ਅੰਤ
	    oap->ਸਿਰਾ = curwin->w_cursor;
	    curwin->w_cursor = oap->ਸ਼ੁਰੂ;

	    // w_virtcol may have been updated; if the cursor goes back to its
	    // previous position w_virtcol becomes invalid and isn't updated
	    // automatically.
	    curwin->w_valid &= ~VALID_VIRTCOL;
	}
	ਨਹੀਂਤਾਂ
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    // Include folded lines completely.
	    ਜੇ (!VIsual_ਸਰਗਰਮ && oap->motion_type == MLINE)
	    {
		ਜੇ (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum,
									ਨਿਰਾ))
		    curwin->w_cursor.ਖਾਨਾ = 0;
		ਜੇ (hasFolding(oap->ਸ਼ੁਰੂ.lnum, ਨਿਰਾ, &oap->ਸ਼ੁਰੂ.lnum))
		    oap->ਸ਼ੁਰੂ.ਖਾਨਾ = ml_get_len(oap->ਸ਼ੁਰੂ.lnum);
	    }
#ਜੇ_ਅੰਤ
	    oap->ਸਿਰਾ = oap->ਸ਼ੁਰੂ;
	    oap->ਸ਼ੁਰੂ = curwin->w_cursor;
	}

	// Just in case lines were deleted that make the position invalid.
	check_pos(curwin->w_ਬਫਰ, &oap->ਸਿਰਾ);
	oap->ਸਤਰ_ਗਿਣਤੀ = oap->ਸਿਰਾ.lnum - oap->ਸ਼ੁਰੂ.lnum + 1;

	// Set "virtual_op" before resetting VIsual_active.
	virtual_op = virtual_ਸਰਗਰਮ();

	ਜੇ (VIsual_ਸਰਗਰਮ || redo_VIsual_busy)
	{
	    get_op_vcol(oap, redo_VIsual.rv_vcol, TRUE);

	    ਜੇ (!redo_VIsual_busy && !gui_yank)
	    {
		// Prepare to reselect and redo Visual: this is based on the
		// size of the Visual text
		resel_VIsual_ਬਹੁਲਕ = VIsual_ਬਹੁਲਕ;
		ਜੇ (curwin->w_curswant == MAXCOL)
		    resel_VIsual_vcol = MAXCOL;
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (VIsual_ਬਹੁਲਕ != Ctrl_V)
			getvvcol(curwin, &(oap->ਸਿਰਾ),
						ਨਿਰਾ, ਨਿਰਾ, &oap->ਸਿਰਾ_vcol, 0);
		    ਜੇ (VIsual_ਬਹੁਲਕ == Ctrl_V || oap->ਸਤਰ_ਗਿਣਤੀ <= 1)
		    {
			ਜੇ (VIsual_ਬਹੁਲਕ != Ctrl_V)
			    getvvcol(curwin, &(oap->ਸ਼ੁਰੂ),
					      &oap->ਸ਼ੁਰੂ_vcol, ਨਿਰਾ, ਨਿਰਾ, 0);
			resel_VIsual_vcol = oap->ਸਿਰਾ_vcol - oap->ਸ਼ੁਰੂ_vcol + 1;
		    }
		    ਨਹੀਂਤਾਂ
			resel_VIsual_vcol = oap->ਸਿਰਾ_vcol;
		}
		resel_VIsual_ਸਤਰ_ਗਿਣਤੀ = oap->ਸਤਰ_ਗਿਣਤੀ;
	    }

	    // can't redo yank (unless 'y' is in 'cpoptions') and ":"
	    ਜੇ ((redo_yank || oap->op_type != OP_YANK)
		    && oap->op_type != OP_COLON
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
		    && oap->op_type != OP_FOLD
		    && oap->op_type != OP_FOLDOPEN
		    && oap->op_type != OP_FOLDOPENREC
		    && oap->op_type != OP_FOLDCLOSE
		    && oap->op_type != OP_FOLDCLOSEREC
		    && oap->op_type != OP_FOLDDEL
		    && oap->op_type != OP_FOLDDELREC
#ਜੇ_ਅੰਤ
		    && oap->motion_force == NUL
		    )
	    {
		// Prepare for redoing.  Only use the nchar field for "r",
		// otherwise it might be the second char of the operator.
		ਜੇ (cap->cmdchar == 'g' && (cap->nchar == 'n'
							|| cap->nchar == 'N'))
		    prep_redo(oap->regname, cap->count0,
			    get_op_char(oap->op_type),
			    get_extra_op_char(oap->op_type),
			    oap->motion_force, cap->cmdchar, cap->nchar);
		ਨਹੀਂਤਾਂ ਜੇ (!is_ex_cmdchar(cap))
		{
		    ਪੂਰਨ opchar = get_op_char(oap->op_type);
		    ਪੂਰਨ extra_opchar = get_extra_op_char(oap->op_type);
		    ਪੂਰਨ nchar = oap->op_type == OP_REPLACE ? cap->nchar : NUL;

		    // reverse what nv_replace() did
		    ਜੇ (nchar == REPLACE_CR_NCHAR)
			nchar = CAR;
		    ਨਹੀਂਤਾਂ ਜੇ (nchar == REPLACE_NL_NCHAR)
			nchar = NL;

		    ਜੇ (opchar == 'g' && extra_opchar == '@')
			// also repeat the count for 'operatorfunc'
			prep_redo_num2(oap->regname, 0L, NUL, 'v',
				     cap->count0, opchar, extra_opchar, nchar);
		    ਨਹੀਂਤਾਂ
			prep_redo(oap->regname, 0L, NUL, 'v',
						  opchar, extra_opchar, nchar);
		}
		ਜੇ (!redo_VIsual_busy)
		{
		    redo_VIsual.rv_ਬਹੁਲਕ = resel_VIsual_ਬਹੁਲਕ;
		    redo_VIsual.rv_vcol = resel_VIsual_vcol;
		    redo_VIsual.rv_ਸਤਰ_ਗਿਣਤੀ = resel_VIsual_ਸਤਰ_ਗਿਣਤੀ;
		    redo_VIsual.rv_ਗਿਣਤੀ = cap->count0;
		    redo_VIsual.rv_arg = cap->arg;
		}
	    }

	    // oap->inclusive defaults to TRUE.
	    // If oap->end is on a NUL (empty line) oap->inclusive becomes
	    // FALSE.  This makes "d}P" and "v}dP" work the same.
	    ਜੇ (oap->motion_force == NUL || oap->motion_type == MLINE)
		oap->inclusive = TRUE;
	    ਜੇ (VIsual_ਬਹੁਲਕ == 'V')
		oap->motion_type = MLINE;
	    ਨਹੀਂਤਾਂ
	    {
		oap->motion_type = MCHAR;
		ਜੇ (VIsual_ਬਹੁਲਕ != Ctrl_V && *ml_get_pos(&(oap->ਸਿਰਾ)) == NUL
			&& (include_ਸਤਰ_break || !virtual_op))
		{
		    oap->inclusive = FALSE;
		    // Try to include the newline, unless it's an operator
		    // that works on lines only.
		    ਜੇ (*p_sel != 'o'
			    && !op_on_ਸਤਰਾਂ(oap->op_type)
			    && oap->ਸਿਰਾ.lnum < curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		    {
			++oap->ਸਿਰਾ.lnum;
			oap->ਸਿਰਾ.ਖਾਨਾ = 0;
			oap->ਸਿਰਾ.coladd = 0;
			++oap->ਸਤਰ_ਗਿਣਤੀ;
		    }
		}
	    }

	    redo_VIsual_busy = FALSE;

	    // Switch Visual off now, so screen updating does
	    // not show inverted text when the screen is redrawn.
	    // With OP_YANK and sometimes with OP_COLON and OP_FILTER there is
	    // no screen redraw, so it is done here to remove the inverted
	    // part.
	    ਜੇ (!gui_yank)
	    {
		VIsual_ਸਰਗਰਮ = FALSE;
		setmouse();
		mouse_dragging = 0;
		may_clear_cmdline();
		ਜੇ ((oap->op_type == OP_YANK
			    || oap->op_type == OP_COLON
			    || oap->op_type == OP_FUNCTION
			    || oap->op_type == OP_FILTER)
			&& oap->motion_force == NUL)
		{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
		    // make sure redrawing is correct
		    restore_lbr(lbr_saved);
#ਜੇ_ਅੰਤ
		    redraw_curbuf_later(UPD_INVERTED);
		}
	    }
	}

	// Include the trailing byte of a multi-byte char.
	ਜੇ (has_mbyte && oap->inclusive)
	{
	    ਪੂਰਨ		l;

	    l = (*mb_ptr2len)(ml_get_pos(&oap->ਸਿਰਾ));
	    ਜੇ (l > 1)
		oap->ਸਿਰਾ.ਖਾਨਾ += l - 1;
	}
	curwin->w_ਸੈੱਟ_curswant = true;

	// oap->empty is set when start and end are the same.  The inclusive
	// flag affects this too, unless yanking and the end is on a NUL.
	oap->ਖਾਲੀ_ਥਾਵਾਂ_2 = (oap->motion_type == MCHAR
		    && (!oap->inclusive
			|| (oap->op_type == OP_YANK
			    && gchar_pos(&oap->ਸਿਰਾ) == NUL))
		    && EQUAL_POS(oap->ਸ਼ੁਰੂ, oap->ਸਿਰਾ)
		    && !(virtual_op && oap->ਸ਼ੁਰੂ.coladd != oap->ਸਿਰਾ.coladd));
	// For delete, change and yank, it's an error to operate on an
	// empty region, when 'E' included in 'cpoptions' (Vi compatible).
	ਖਾਲੀ_ਥਾਵਾਂ_region_error = (oap->ਖਾਲੀ_ਥਾਵਾਂ_2
				&& vim_strchr(p_cpo, CPO_EMPTYREGION) != ਨਿਰਾ);

	// Force a redraw when operating on an empty Visual region, when
	// 'modifiable is off or creating a fold.
	ਜੇ (oap->is_VIsual && (oap->ਖਾਲੀ_ਥਾਵਾਂ_2 || !curbuf->b_p_ma
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
		    || oap->op_type == OP_FOLD
#ਜੇ_ਅੰਤ
		    ))
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
	    restore_lbr(lbr_saved);
#ਜੇ_ਅੰਤ
	    redraw_curbuf_later(UPD_INVERTED);
	}

	// If the end of an operator is in column one while oap->motion_type
	// is MCHAR and oap->inclusive is FALSE, we put op_end after the last
	// character in the previous line. If op_start is on or before the
	// first non-blank in the line, the operator becomes linewise
	// (strange, but that's the way vi does it).
	ਜੇ (	   oap->motion_type == MCHAR
		&& oap->inclusive == FALSE
		&& !(cap->retval & CA_NO_ADJ_OP_ਸਿਰਾ)
		&& oap->ਸਿਰਾ.ਖਾਨਾ == 0
		&& (!oap->is_VIsual || *p_sel == 'o')
		&& !oap->ਖੰਡ_ਬਹੁਲਕ
		&& oap->ਸਤਰ_ਗਿਣਤੀ > 1)
	{
	    oap->ਸਿਰਾ_adjusted = TRUE;	    // remember that we did this
	    --oap->ਸਤਰ_ਗਿਣਤੀ;
	    --oap->ਸਿਰਾ.lnum;
	    ਜੇ (inindent(0))
		oap->motion_type = MLINE;
	    ਨਹੀਂਤਾਂ
	    {
		oap->ਸਿਰਾ.ਖਾਨਾ = ml_get_len(oap->ਸਿਰਾ.lnum);
		ਜੇ (oap->ਸਿਰਾ.ਖਾਨਾ)
		{
		    --oap->ਸਿਰਾ.ਖਾਨਾ;
		    oap->inclusive = TRUE;
		}
	    }
	}
	ਨਹੀਂਤਾਂ
	    oap->ਸਿਰਾ_adjusted = FALSE;

	ਚੋਣ (oap->op_type)
	{
	ਹਾਲਤ OP_LSHIFT:
	ਹਾਲਤ OP_RSHIFT:
	    op_ਖਿਸਕਾਓ(oap, TRUE, oap->is_VIsual ? (ਪੂਰਨ)cap->count1 : 1);
	    auto_format(FALSE, TRUE);
	    ਰੋਕੋ;

	ਹਾਲਤ OP_JOIN_NS:
	ਹਾਲਤ OP_JOIN:
	    ਜੇ (oap->ਸਤਰ_ਗਿਣਤੀ < 2)
		oap->ਸਤਰ_ਗਿਣਤੀ = 2;
	    ਜੇ (curwin->w_cursor.lnum + oap->ਸਤਰ_ਗਿਣਤੀ - 1 >
						   curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		beep_flush();
	    ਨਹੀਂਤਾਂ
	    {
		(ਖਾਲੀ)do_join(oap->ਸਤਰ_ਗਿਣਤੀ, oap->op_type == OP_JOIN,
							    TRUE, TRUE, TRUE);
		auto_format(FALSE, TRUE);
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ OP_DELETE:
	    VIsual_reselect = FALSE;	    // don't reselect now
	    ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    ਨਹੀਂਤਾਂ
	    {
		(ਖਾਲੀ)op_delete(oap);
		// save cursor line for undo if it wasn't saved yet
		ਜੇ (oap->motion_type == MLINE && has_format_option(FO_AUTO)
						      && u_save_cursor() == OK)
		    auto_format(FALSE, TRUE);
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ OP_YANK:
	    ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_region_error)
	    {
		ਜੇ (!gui_yank)
		{
		    vim_beep(BO_OPER);
		    CancelRedo();
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
		restore_lbr(lbr_saved);
#ਜੇ_ਅੰਤ
		oap->excl_tr_ws = cap->cmdchar == 'z';
		(ਖਾਲੀ)op_yank(oap, FALSE, !gui_yank);
	    }
	    check_cursor_ਖਾਨਾ();
	    ਰੋਕੋ;

	ਹਾਲਤ OP_CHANGE:
	    VIsual_reselect = FALSE;	    // don't reselect now
	    ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// This is a new edit command, not a restart.  Need to
		// remember it to make 'insertmode' work with mappings for
		// Visual mode.  But do this only once and not when typed and
		// 'insertmode' isn't set.
		ਜੇ (p_im || !ਕੁੰਜੀ_Typed)
		    restart_edit_save = restart_edit;
		ਨਹੀਂਤਾਂ
		    restart_edit_save = 0;
		restart_edit = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#ਜੇ_ਅੰਤ
		// trigger TextChangedI
		curbuf->b_ਆਖਰੀ_changedtick_i = CHANGEDTICK(curbuf);

		ਜੇ (op_change(oap))	// will call edit()
		    cap->retval |= CA_COMMAND_BUSY;
		ਜੇ (restart_edit == 0)
		    restart_edit = restart_edit_save;
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ OP_FILTER:
	    ਜੇ (vim_strchr(p_cpo, CPO_FILTER) != ਨਿਰਾ)
		Append_ਮੰਜ਼ਿਲ_Redobuff((char_u *)"!\r");  // use any last used !cmd
	    ਨਹੀਂਤਾਂ
		bangredo = TRUE;    // do_bang() will put cmd in redo buffer
	    // FALLTHROUGH

	ਹਾਲਤ OP_INDENT:
	ਹਾਲਤ OP_COLON:

	    // If 'equalprg' is empty, do the indenting internally.
	    ਜੇ (oap->op_type == OP_INDENT && *get_equalprg() == NUL)
	    {
		ਜੇ (curbuf->b_p_lisp)
		{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		    ਜੇ (use_indentexpr_for_lisp())
			op_reindent(oap, get_ਵਿਅੰਜਕ_indent);
		    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
			op_reindent(oap, get_lisp_indent);
		    ਰੋਕੋ;
		}
		op_reindent(oap,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
			*curbuf->b_p_inde != NUL ? get_ਵਿਅੰਜਕ_indent :
#ਜੇ_ਅੰਤ
			    get_c_indent);
		ਰੋਕੋ;
	    }

	    op_colon(oap);
	    ਰੋਕੋ;

	ਹਾਲਤ OP_TILDE:
	ਹਾਲਤ OP_ਉੱਤਲਾ:
	ਹਾਲਤ OP_ਹੇਠਲਾ:
	ਹਾਲਤ OP_ROT13:
	    ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    ਨਹੀਂਤਾਂ
		op_tilde(oap);
	    check_cursor_ਖਾਨਾ();
	    ਰੋਕੋ;

	ਹਾਲਤ OP_FORMAT:
#ਜੇ defined(FEAT_EVAL)
	    ਜੇ (*curbuf->b_p_fex != NUL)
		op_formatexpr(oap);	// use expression
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    {
		ਜੇ (*p_fp != NUL || *curbuf->b_p_fp != NUL)
		    op_colon(oap);		// use external command
		ਨਹੀਂਤਾਂ
		    op_format(oap, FALSE);	// use internal function
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ OP_FORMAT2:
	    op_format(oap, TRUE);	// use internal function
	    ਰੋਕੋ;

	ਹਾਲਤ OP_FUNCTION:
	    {
		redo_VIsual_T   save_redo_VIsual = redo_VIsual;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#ਜੇ_ਅੰਤ
		// call 'operatorfunc'
		op_function(oap);

		// Restore the info for redoing Visual mode, the function may
		// invoke another operator and unintentionally change it.
		redo_VIsual = save_redo_VIsual;
		ਰੋਕੋ;
	    }

	ਹਾਲਤ OP_ਪਾਓ:
	ਹਾਲਤ OP_APPEND:
	    VIsual_reselect = FALSE;	// don't reselect now
	    ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// This is a new edit command, not a restart.  Need to
		// remember it to make 'insertmode' work with mappings for
		// Visual mode.  But do this only once.
		restart_edit_save = restart_edit;
		restart_edit = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#ਜੇ_ਅੰਤ
		// trigger TextChangedI
		curbuf->b_ਆਖਰੀ_changedtick_i = CHANGEDTICK(curbuf);

		op_ਪਾਓ(oap, cap->count1);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
		// Reset linebreak, so that formatting works correctly.
		(ਖਾਲੀ)reset_lbr();
#ਜੇ_ਅੰਤ

		// TODO: when inserting in several lines, should format all
		// the lines.
		auto_format(FALSE, TRUE);

		ਜੇ (restart_edit == 0)
		    restart_edit = restart_edit_save;
		ਨਹੀਂਤਾਂ
		    cap->retval |= CA_COMMAND_BUSY;
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ OP_REPLACE:
	    VIsual_reselect = FALSE;	// don't reselect now
	    ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    ਨਹੀਂਤਾਂ
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#ਜੇ_ਅੰਤ
		op_replace(oap, cap->nchar);
	    }
	    ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	ਹਾਲਤ OP_FOLD:
	    VIsual_reselect = FALSE;	// don't reselect now
	    foldCreate(oap->ਸ਼ੁਰੂ.lnum, oap->ਸਿਰਾ.lnum);
	    ਰੋਕੋ;

	ਹਾਲਤ OP_FOLDOPEN:
	ਹਾਲਤ OP_FOLDOPENREC:
	ਹਾਲਤ OP_FOLDCLOSE:
	ਹਾਲਤ OP_FOLDCLOSEREC:
	    VIsual_reselect = FALSE;	// don't reselect now
	    opFoldRange(oap->ਸ਼ੁਰੂ.lnum, oap->ਸਿਰਾ.lnum,
		    oap->op_type == OP_FOLDOPEN
					    || oap->op_type == OP_FOLDOPENREC,
		    oap->op_type == OP_FOLDOPENREC
					  || oap->op_type == OP_FOLDCLOSEREC,
					  oap->is_VIsual);
	    ਰੋਕੋ;

	ਹਾਲਤ OP_FOLDDEL:
	ਹਾਲਤ OP_FOLDDELREC:
	    VIsual_reselect = FALSE;	// don't reselect now
	    deleteFold(oap->ਸ਼ੁਰੂ.lnum, oap->ਸਿਰਾ.lnum,
			       oap->op_type == OP_FOLDDELREC, oap->is_VIsual);
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	ਹਾਲਤ OP_NR_ਜੋੜੋ:
	ਹਾਲਤ OP_NR_SUB:
	    ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    ਨਹੀਂਤਾਂ
	    {
		VIsual_ਸਰਗਰਮ = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
		restore_lbr(lbr_saved);
#ਜੇ_ਅੰਤ
		op_addsub(oap, cap->count1, redo_VIsual.rv_arg);
		VIsual_ਸਰਗਰਮ = FALSE;
	    }
	    check_cursor_ਖਾਨਾ();
	    ਰੋਕੋ;
	ਮੂਲ:
	    clearopbeep(oap);
	}
	virtual_op = MAYBE;
	ਜੇ (!gui_yank)
	{
	    // if 'sol' not set, go back to old column for some commands
	    ਜੇ (!p_sol && oap->motion_type == MLINE && !oap->ਸਿਰਾ_adjusted
		    && (oap->op_type == OP_LSHIFT || oap->op_type == OP_RSHIFT
						|| oap->op_type == OP_DELETE))
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
		(ਖਾਲੀ)reset_lbr();
#ਜੇ_ਅੰਤ
		coladvance(curwin->w_curswant = old_ਖਾਨਾ);
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    curwin->w_cursor = old_cursor;
	}
	oap->ਖੰਡ_ਬਹੁਲਕ = FALSE;
	clearop(oap);
	motion_force = NUL;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    restore_lbr(lbr_saved);
#ਜੇ_ਅੰਤ
}

// put byte 'c' at position 'lp', but
// verify, that the position to place
// is actually safe
    ਸਥਿਰ ਖਾਲੀ
pbyte(pos_T lp, ਪੂਰਨ c)
{
    char_u *p = ml_get_buf(curbuf, lp.lnum, TRUE);
    ਪੂਰਨ	len = curbuf->b_ml.ml_ਸਤਰ_len;

    // safety check
    ਜੇ (lp.ਖਾਨਾ >= len)
	lp.ਖਾਨਾ = (len > 1 ? len - 2 : 0);
    *(p + lp.ਖਾਨਾ) = c;
}
