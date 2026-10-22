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

#ಅನ್ತರ್ಭಾವ "vim.h"

ಸ್ಥಿರ ಶೂನ್ಯ ಸರಿಸು_ಖಂಡ(oparg_T *oap, ಪೂರ್ಣಾಂಕ amount);
ಸ್ಥಿರ ಶೂನ್ಯ	mb_adjust_opend(oparg_T *oap);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	do_addsub(ಪೂರ್ಣಾಂಕ op_type, pos_T *pos, ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ, linenr_T Prenum1);
ಸ್ಥಿರ ಶೂನ್ಯ	pbyte(pos_T lp, ಪೂರ್ಣಾಂಕ c);
#ವ್ಯಾಖ್ಯಾನಿಸು PBYTE(lp, c) pbyte(lp, c)


// Flags for third item in "opchars".
#ವ್ಯಾಖ್ಯಾನಿಸು OPF_ಸಾಲುಗಳು  1	// operator always works on lines
#ವ್ಯಾಖ್ಯಾನಿಸು OPF_CHANGE 2	// operator changes text

/*
 * The names of operators.
 * IMPORTANT: Index must correspond with defines in vim.h!!!
 * The third field holds OPF_ flags.
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ opchars[][3] =
{
    {NUL, NUL, 0},			// OP_NOP
    {'d', NUL, OPF_CHANGE},		// OP_DELETE
    {'y', NUL, 0},			// OP_YANK
    {'c', NUL, OPF_CHANGE},		// OP_CHANGE
    {'<', NUL, OPF_ಸಾಲುಗಳು | OPF_CHANGE},	// OP_LSHIFT
    {'>', NUL, OPF_ಸಾಲುಗಳು | OPF_CHANGE},	// OP_RSHIFT
    {'!', NUL, OPF_ಸಾಲುಗಳು | OPF_CHANGE},	// OP_FILTER
    {'g', '~', OPF_CHANGE},		// OP_TILDE
    {'=', NUL, OPF_ಸಾಲುಗಳು | OPF_CHANGE},	// OP_INDENT
    {'g', 'q', OPF_ಸಾಲುಗಳು | OPF_CHANGE},	// OP_FORMAT
    {':', NUL, OPF_ಸಾಲುಗಳು},		// OP_COLON
    {'g', 'U', OPF_CHANGE},		// OP_UPPER
    {'g', 'u', OPF_CHANGE},		// OP_LOWER
    {'J', NUL, OPF_ಸಾಲುಗಳು | OPF_CHANGE},	// DO_JOIN
    {'g', 'J', OPF_ಸಾಲುಗಳು | OPF_CHANGE},	// DO_JOIN_NS
    {'g', '?', OPF_CHANGE},		// OP_ROT13
    {'r', NUL, OPF_CHANGE},		// OP_REPLACE
    {'I', NUL, OPF_CHANGE},		// OP_INSERT
    {'A', NUL, OPF_CHANGE},		// OP_APPEND
    {'z', 'f', OPF_ಸಾಲುಗಳು},		// OP_FOLD
    {'z', 'o', OPF_ಸಾಲುಗಳು},		// OP_FOLDOPEN
    {'z', 'O', OPF_ಸಾಲುಗಳು},		// OP_FOLDOPENREC
    {'z', 'c', OPF_ಸಾಲುಗಳು},		// OP_FOLDCLOSE
    {'z', 'C', OPF_ಸಾಲುಗಳು},		// OP_FOLDCLOSEREC
    {'z', 'd', OPF_ಸಾಲುಗಳು},		// OP_FOLDDEL
    {'z', 'D', OPF_ಸಾಲುಗಳು},		// OP_FOLDDELREC
    {'g', 'w', OPF_ಸಾಲುಗಳು | OPF_CHANGE},	// OP_FORMAT2
    {'g', '@', OPF_CHANGE},		// OP_FUNCTION
    {Ctrl_A, NUL, OPF_CHANGE},		// OP_NR_ADD
    {Ctrl_X, NUL, OPF_CHANGE},		// OP_NR_SUB
};

/*
 * Translate a command name into an operator type.
 * Must only be called with a valid operator name!
 */
    ಪೂರ್ಣಾಂಕ
get_op_type(ಪೂರ್ಣಾಂಕ char1, ಪೂರ್ಣಾಂಕ char2)
{
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (char1 == 'r')		// ignore second character
	ಹಿಂತಿರುಗಿ OP_REPLACE;
    ಆದರೆ (char1 == '~')		// when tilde is an operator
	ಹಿಂತಿರುಗಿ OP_TILDE;
    ಆದರೆ (char1 == 'g' && char2 == Ctrl_A)	//ಕೂಡಿಸು
	ಹಿಂತಿರುಗಿ OP_NR_ಕೂಡಿಸು;
    ಆದರೆ (char1 == 'g' && char2 == Ctrl_X)	//ಕಳೆ
	ಹಿಂತಿರುಗಿ OP_NR_SUB;
    ಆದರೆ (char1 == 'z' && char2 == 'y')	// OP_YANK
	ಹಿಂತಿರುಗಿ OP_YANK;
    ಗಾಗಿ (i = 0; ; ++i)
    {
	ಆದರೆ (opchars[i][0] == char1 && opchars[i][1] == char2)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (i == (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(opchars) - 1)
	{
	    ಆಂತರಿಕ_error("get_op_type()");
	    ನಿಲ್ಲಿಸು;
	}
    }
    ಹಿಂತಿರುಗಿ i;
}

/*
 * Return TRUE if operator "op" always works on whole lines.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
op_on_ಸಾಲುಗಳು(ಪೂರ್ಣಾಂಕ op)
{
    ಹಿಂತಿರುಗಿ opchars[op][2] & OPF_ಸಾಲುಗಳು;
}

#ಆದರೆ defined(FEAT_ಕೆಲಸ_CHANNEL)
/*
 * Return TRUE if operator "op" changes text.
 */
    ಪೂರ್ಣಾಂಕ
op_is_change(ಪೂರ್ಣಾಂಕ op)
{
    ಹಿಂತಿರುಗಿ opchars[op][2] & OPF_CHANGE;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Get first operator command character.
 * Returns 'g' or 'z' if there is another command character.
 */
    ಪೂರ್ಣಾಂಕ
get_op_char(ಪೂರ್ಣಾಂಕ optype)
{
    ಹಿಂತಿರುಗಿ opchars[optype][0];
}

/*
 * Get second operator command character.
 */
    ಪೂರ್ಣಾಂಕ
get_extra_op_char(ಪೂರ್ಣಾಂಕ optype)
{
    ಹಿಂತಿರುಗಿ opchars[optype][1];
}

/*
 * op_shift - handle a shift operation
 */
    ಶೂನ್ಯ
op_ಸರಿಸು(oparg_T *oap, ಪೂರ್ಣಾಂಕ curs_ತುದಿ, ಪೂರ್ಣಾಂಕ amount)
{
    ದೀರ್ಘ	    i;
    ಪೂರ್ಣಾಂಕ		    ಮೊದಲ_char;
    ಪೂರ್ಣಾಂಕ		    ಖಂಡ_ಲಂಬಸಾಲು = 0;

    ಆದರೆ (u_save((linenr_T)(oap->ಆರಂಭ.lnum - 1),
				       (linenr_T)(oap->ಕೊನೆ.lnum + 1)) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (oap->ಖಂಡ_ಬಹುಲಕ)
	ಖಂಡ_ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;

    ಗಾಗಿ (i = oap->ಸಾಲು_ಎಣಿಕೆ; --i >= 0; )
    {
	ಮೊದಲ_char = *ml_get_curline();
	ಆದರೆ (ಮೊದಲ_char == NUL)				// empty line
	    curwin->w_cursor.ಲಂಬಸಾಲು = 0;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (oap->ಖಂಡ_ಬಹುಲಕ)
	    ಸರಿಸು_ಖಂಡ(oap, amount);
	ಇಲ್ಲದಿದ್ದರೆ
	    // Move the line right if it doesn't start with '#', 'smartindent'
	    // isn't set or 'cindent' isn't set or '#' isn't in 'cino'.
	    ಆದರೆ (ಮೊದಲ_char != '#' || !preprocs_ಎಡ())
		ಸರಿಸು_ಸಾಲು(oap->op_type == OP_LSHIFT, p_sr, amount, FALSE);
	++curwin->w_cursor.lnum;
    }

    ಬದಲಾಯಿತು_ಸಾಲುಗಳು(oap->ಆರಂಭ.lnum, 0, oap->ಕೊನೆ.lnum + 1, 0L);
    ಆದರೆ (oap->ಖಂಡ_ಬಹುಲಕ)
    {
	curwin->w_cursor.lnum = oap->ಆರಂಭ.lnum;
	curwin->w_cursor.ಲಂಬಸಾಲು = ಖಂಡ_ಲಂಬಸಾಲು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curs_ತುದಿ)	    // put cursor on first line, for ">>"
    {
	curwin->w_cursor.lnum = oap->ಆರಂಭ.lnum;
	beginline(BL_SOL | BL_FIX);   // shift_line() may have set cursor.col
    }
    ಇಲ್ಲದಿದ್ದರೆ
	--curwin->w_cursor.lnum;	// put cursor on last line, for ":>"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    // The cursor line is not in a closed fold
    foldOpenCursor();
#ಆದರೆ_ಅಂತ್ಯ


    ಆದರೆ (oap->ಸಾಲು_ಎಣಿಕೆ > p_report)
    {
	ಅಕ್ಷರ	    *op;
	ಅಕ್ಷರ	    *ಸಂದೇಶ_ಸಾಲು_single;
	ಅಕ್ಷರ	    *ಸಂದೇಶ_ಸಾಲು_plural;

	ಆದರೆ (oap->op_type == OP_RSHIFT)
	    op = ">";
	ಇಲ್ಲದಿದ್ದರೆ
	    op = "<";
	ಸಂದೇಶ_ಸಾಲು_single = NGETTEXT("%ld line %sed %d time",
					     "%ld line %sed %d times", amount);
	ಸಂದೇಶ_ಸಾಲು_plural = NGETTEXT("%ld lines %sed %d time",
					    "%ld lines %sed %d times", amount);
	vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE,
		NGETTEXT(ಸಂದೇಶ_ಸಾಲು_single, ಸಂದೇಶ_ಸಾಲು_plural, oap->ಸಾಲು_ಎಣಿಕೆ),
		oap->ಸಾಲು_ಎಣಿಕೆ, op, amount);
	ಸಂದೇಶ_attr_keep((ಅಕ್ಷರ *)IObuff, 0, TRUE);
    }

    ಆದರೆ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_ಆರಂಭ = oap->ಆರಂಭ;
	curbuf->b_op_ಕೊನೆ.lnum = oap->ಕೊನೆ.lnum;
	curbuf->b_op_ಕೊನೆ.ಲಂಬಸಾಲು = ml_get_len(oap->ಕೊನೆ.lnum);
	ಆದರೆ (curbuf->b_op_ಕೊನೆ.ಲಂಬಸಾಲು > 0)
	    --curbuf->b_op_ಕೊನೆ.ಲಂಬಸಾಲು;
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
/*
 * Return the tabstop width at the index of the variable tabstop array.  If an
 * index greater than the length of the array is given, the last tabstop width
 * in the array is returned.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_vts(ಪೂರ್ಣಾಂಕ *vts_array, ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ)
{
    ಪೂರ್ಣಾಂಕ	ts;

    ಆದರೆ (ಸೂಚ್ಯಂಕ < 1)
	ts = 0;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸೂಚ್ಯಂಕ <= vts_array[0])
	ts = vts_array[ಸೂಚ್ಯಂಕ];
    ಇಲ್ಲದಿದ್ದರೆ
	ts = vts_array[vts_array[0]];

    ಹಿಂತಿರುಗಿ ts;
}

/*
 * Return the sum of all the tabstops through the index-th.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_vts_ಮೊತ್ತ(ಪೂರ್ಣಾಂಕ *vts_array, ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ)
{
    ಪೂರ್ಣಾಂಕ	ಮೊತ್ತ = 0;
    ಪೂರ್ಣಾಂಕ	i;

    // Perform the summation for indices within the actual array.
    ಗಾಗಿ (i = 1; i <= ಸೂಚ್ಯಂಕ && i <= vts_array[0]; i++)
	ಮೊತ್ತ += vts_array[i];

    // Add tabstops whose indices exceed the actual array.
    ಆದರೆ (i <= ಸೂಚ್ಯಂಕ)
	ಮೊತ್ತ += vts_array[vts_array[0]] * (ಸೂಚ್ಯಂಕ - vts_array[0]);

    ಹಿಂತಿರುಗಿ ಮೊತ್ತ;
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ vimlong_T
get_new_sw_indent(
    ಪೂರ್ಣಾಂಕ		ಎಡ_2,		// TRUE if shift is to the left
    ಪೂರ್ಣಾಂಕ		ಸುತ್ತು,		// TRUE if new indent is to be to a tabstop
    vimlong_T	amount,		// Number of shifts
    vimlong_T	sw_val)
{
    vimlong_T	ಎಣಿಕೆ_2 = get_indent();
    vimlong_T	i, j;

    ಆದರೆ (ಸುತ್ತು)			// round off indent
    {
	i = trim_ಗಮ್ಯ_int(ಎಣಿಕೆ_2) / sw_val;	// number of 'shiftwidth' rounded down
	j = trim_ಗಮ್ಯ_int(ಎಣಿಕೆ_2) % sw_val;	// extra spaces
	ಆದರೆ (j && ಎಡ_2)		// first remove extra spaces
	    --amount;
	ಆದರೆ (ಎಡ_2)
	{
	    i -= amount;
	    ಆದರೆ (i < 0)
		i = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    i += amount;
	ಎಣಿಕೆ_2 = i * sw_val;
    }
    ಇಲ್ಲದಿದ್ದರೆ			// original vi indent
    {
	ಆದರೆ (ಎಡ_2)
	{
	    ಎಣಿಕೆ_2 -= sw_val * amount;
	    ಆದರೆ (ಎಣಿಕೆ_2 < 0)
		ಎಣಿಕೆ_2 = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಎಣಿಕೆ_2 += sw_val * amount;
    }

    ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_2;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
    ಸ್ಥಿರ vimlong_T
get_new_vts_indent(
    ಪೂರ್ಣಾಂಕ	ಎಡ_2,			// TRUE if shift is to the left
    ಪೂರ್ಣಾಂಕ	ಸುತ್ತು,			// TRUE if new indent is to be to a tabstop
    ಪೂರ್ಣಾಂಕ	amount,			// Number of shifts
    ಪೂರ್ಣಾಂಕ	*vts_array)
{
    vimlong_T	indent = get_indent();
    ಪೂರ್ಣಾಂಕ		vtsi = 0;
    ಪೂರ್ಣಾಂಕ		vts_indent = 0;
    ಪೂರ್ಣಾಂಕ		ts = 0;		// Silence uninitialized variable warning.
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಾನಾಂತರ;		// Extra indent spaces to the right of the
				// tabstop

    // Find the tabstop at or to the left of the current indent.
    ಆಗ (vts_indent <= indent)
    {
	vtsi++;
	ts = get_vts(vts_array, vtsi);
	vts_indent += ts;
    }
    vts_indent -= ts;
    vtsi--;

    ಸ್ಥಾನಾಂತರ = indent - vts_indent;

    ಆದರೆ (ಸುತ್ತು)
    {
	ಆದರೆ (ಎಡ_2)
	{
	    ಆದರೆ (ಸ್ಥಾನಾಂತರ == 0)
		indent = get_vts_ಮೊತ್ತ(vts_array, vtsi - amount);
	    ಇಲ್ಲದಿದ್ದರೆ
		indent = get_vts_ಮೊತ್ತ(vts_array, vtsi - (amount - 1));
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    indent = get_vts_ಮೊತ್ತ(vts_array, vtsi + amount);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (ಎಡ_2)
	{
	    ಆದರೆ (amount > vtsi)
		indent = 0;
	    ಇಲ್ಲದಿದ್ದರೆ
		indent = get_vts_ಮೊತ್ತ(vts_array, vtsi - amount) + ಸ್ಥಾನಾಂತರ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    indent = get_vts_ಮೊತ್ತ(vts_array, vtsi + amount) + ಸ್ಥಾನಾಂತರ;
    }

    ಹಿಂತಿರುಗಿ indent;
}
#ಆದರೆ_ಅಂತ್ಯ

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
    ಶೂನ್ಯ
ಸರಿಸು_ಸಾಲು(
    ಪೂರ್ಣಾಂಕ	ಎಡ_2,			// TRUE if shift is to the left
    ಪೂರ್ಣಾಂಕ	ಸುತ್ತು,			// TRUE if new indent is to be to a tabstop
    ಪೂರ್ಣಾಂಕ	amount,			// Number of shifts
    ಪೂರ್ಣಾಂಕ	call_ಬದಲಾಯಿತು_ಬೈಟುಗಳು)	// call changed_bytes()
{
    vimlong_T	ಎಣಿಕೆ_2;
    ದೀರ್ಘ	sw_val = curbuf->b_p_sw;
    ದೀರ್ಘ	ts_val = curbuf->b_p_ts;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
    ಪೂರ್ಣಾಂಕ		*vts_array = curbuf->b_p_vts_array;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (sw_val != 0)
	// 'shiftwidth' is not zero; use it as the shift size.
	ಎಣಿಕೆ_2 = get_new_sw_indent(ಎಡ_2, ಸುತ್ತು, amount, sw_val);
    ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
	ಆದರೆ ((vts_array == ಖಾಲಿ) || (vts_array[0] == 0))
#ಆದರೆ_ಅಂತ್ಯ
    {
	// 'shiftwidth is zero and 'vartabstop' is empty; use 'tabstop' as the
	// shift size.
	ಎಣಿಕೆ_2 = get_new_sw_indent(ಎಡ_2, ಸುತ್ತು, amount, ts_val);
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// 'shiftwidth is zero and 'vartabstop' is defined; use 'vartabstop'
	// to determine the new indent.
	ಎಣಿಕೆ_2 = get_new_vts_indent(ಎಡ_2, ಸುತ್ತು, amount, vts_array);
    }
#ಆದರೆ_ಅಂತ್ಯ

    // Set new indent
    ಆದರೆ (State & VREPLACE_FLAG)
	change_indent(INDENT_ಗಣ, trim_ಗಮ್ಯ_int(ಎಣಿಕೆ_2), FALSE, NUL, call_ಬದಲಾಯಿತು_ಬೈಟುಗಳು);
    ಇಲ್ಲದಿದ್ದರೆ
	(ಶೂನ್ಯ)ಗಣ_indent(trim_ಗಮ್ಯ_int(ಎಣಿಕೆ_2), call_ಬದಲಾಯಿತು_ಬೈಟುಗಳು ? SIN_ಬದಲಾಯಿತು : 0);
}

/*
 * Shift one line of the current block one shiftwidth right or left.
 * Leaves cursor on first character in block.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸರಿಸು_ಖಂಡ(oparg_T *oap, ಪೂರ್ಣಾಂಕ amount)
{
    ಪೂರ್ಣಾಂಕ			ಎಡ_2 = (oap->op_type == OP_LSHIFT);
    ಪೂರ್ಣಾಂಕ			oldstate = State;
    ಪೂರ್ಣಾಂಕ			ಒಟ್ಟು;
    char_u		*newp, *oldp;
    size_t		newlen, oldlen;
    ಪೂರ್ಣಾಂಕ			oldcol = curwin->w_cursor.ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ			sw_val = (ಪೂರ್ಣಾಂಕ)get_sw_ಮೌಲ್ಯ_indent(curbuf, ಎಡ_2);
    ಪೂರ್ಣಾಂಕ			ts_val = (ಪೂರ್ಣಾಂಕ)curbuf->b_p_ts;
    ರಚನೆ ಖಂಡ_def	bd;
    ಪೂರ್ಣಾಂಕ			incr;
    colnr_T		ws_vcol;
    ಪೂರ್ಣಾಂಕ			added;
    size_t		new_ಸಾಲು_len;	// the length of the line after the
					// block shift
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    ಪೂರ್ಣಾಂಕ			old_p_ri = p_ri;

    p_ri = 0;			// don't want revins in indent
#ಆದರೆ_ಅಂತ್ಯ

    State = ಬಹುಲಕ_ಸೇರಿಸು;	// don't want MODE_REPLACE for State
    ಖಂಡ_prep(oap, &bd, curwin->w_cursor.lnum, TRUE);
    ಆದರೆ (bd.is_short)
	ಹಿಂತಿರುಗಿ;

    // total is number of screen columns to be inserted/removed
    ಒಟ್ಟು = (ಪೂರ್ಣಾಂಕ)((ಚಿಹ್ನೆರಹಿತ)amount * (ಚಿಹ್ನೆರಹಿತ)sw_val);
    ಆದರೆ ((ಒಟ್ಟು / sw_val) != amount)
	ಹಿಂತಿರುಗಿ; // multiplication overflow

    oldp = ml_get_curline();
    oldlen = ml_get_curline_len();

    ಆದರೆ (!ಎಡ_2)
    {
	ಪೂರ್ಣಾಂಕ		tabs = 0, spaces = 0;
	chartabsize_T	cts;

	/*
	 *  1. Get start vcol
	 *  2. Total ws vcols
	 *  3. Divvy into TABs & spp
	 *  4. Construct new string
	 */
	ಒಟ್ಟು += bd.pre_whitesp; // all virtual WS up to & incl a split TAB
	ws_vcol = bd.ಆರಂಭ_vcol - bd.pre_whitesp;
	ಆದರೆ (bd.startspaces)
	{
	    ಆದರೆ (has_mbyte)
	    {
		ಆದರೆ ((*mb_ptr2len)(bd.textstart) == 1)
		    ++bd.textstart;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ws_vcol = 0;
		    bd.startspaces = 0;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		++bd.textstart;
	}

	// TODO: is passing bd.textstart for start of the line OK?
	init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum,
				   bd.ಆರಂಭ_vcol, bd.textstart, bd.textstart);
	ಗಾಗಿ ( ; VIM_ISWHITE(*cts.cts_ptr); )
	{
	    incr = lbr_chartabsize_adv(&cts);
	    ಒಟ್ಟು += incr;
	    cts.cts_vcol += incr;
	}
	bd.textstart = cts.cts_ptr;
	bd.ಆರಂಭ_vcol = cts.cts_vcol;
	clear_chartabsize_arg(&cts);

	// OK, now total=all the VWS reqd, and textstart points at the 1st
	// non-ws char in the block.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
	ಆದರೆ (!curbuf->b_p_et)
	    tabstop_fromto(ws_vcol, ws_vcol + ಒಟ್ಟು,
				ts_val, curbuf->b_p_vts_array, &tabs, &spaces);
	ಇಲ್ಲದಿದ್ದರೆ
	    spaces = ಒಟ್ಟು;
#ಇಲ್ಲದಿದ್ದರೆ
	ಆದರೆ (!curbuf->b_p_et)
	    tabs = ((ws_vcol % ts_val) + ಒಟ್ಟು) / ts_val; // number of tabs
	ಆದರೆ (tabs > 0)
	    spaces = ((ws_vcol % ts_val) + ಒಟ್ಟು) % ts_val; // number of spp
	ಇಲ್ಲದಿದ್ದರೆ
	    spaces = ಒಟ್ಟು;
#ಆದರೆ_ಅಂತ್ಯ
	// if we're splitting a TAB, allow for it
	bd.textcol -= bd.pre_whitesp_c - (bd.startspaces != 0);

	new_ಸಾಲು_len = bd.textcol + tabs + spaces + (oldlen - (bd.textstart - oldp));
	newp = alloc(new_ಸಾಲು_len + 1);
	ಆದರೆ (newp == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	mch_memmove(newp, oldp, (size_t)bd.textcol);
	newlen = bd.textcol;
	vim_memset(newp + newlen, TAB, (size_t)tabs);
	newlen += tabs;
	vim_memset(newp + newlen, ' ', (size_t)spaces);
	STRCPY(newp + newlen + spaces, bd.textstart);
    }
    ಇಲ್ಲದಿದ್ದರೆ //ಎಡ
    {
	colnr_T	    destination_ಲಂಬಸಾಲು;	// column to which text in block will
					// be shifted
	char_u	    *verbatim_copy_ಕೊನೆ;	// end of the part of the line which is
					// copied verbatim
	colnr_T	    verbatim_copy_ಅಗಲ;// the (displayed) width of this part
					// of line
	size_t	    fill;		// nr of spaces that replace a TAB
	size_t	    ಖಂಡ_space_ಅಗಲ;
	size_t	    ಸರಿಸು_amount;
	char_u	    *non_white = bd.textstart;
	colnr_T	    non_white_ಲಂಬಸಾಲು;
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
	ಆದರೆ (bd.startspaces)
	    MB_PTR_ADV(non_white);

	// The character's column is in "bd.start_vcol".
	non_white_ಲಂಬಸಾಲು = bd.ಆರಂಭ_vcol;

	init_chartabsize_arg(&cts, curwin, curwin->w_cursor.lnum,
				   non_white_ಲಂಬಸಾಲು, bd.textstart, non_white);
	ಆಗ (VIM_ISWHITE(*cts.cts_ptr))
	{
	    incr = lbr_chartabsize_adv(&cts);
	    cts.cts_vcol += incr;
	}
	non_white_ಲಂಬಸಾಲು = cts.cts_vcol;
	non_white = cts.cts_ptr;
	clear_chartabsize_arg(&cts);

	ಖಂಡ_space_ಅಗಲ = non_white_ಲಂಬಸಾಲು - oap->ಆರಂಭ_vcol;
	// We will shift by "total" or "block_space_width", whichever is less.
	ಸರಿಸು_amount = (ಖಂಡ_space_ಅಗಲ < (size_t)ಒಟ್ಟು
					 ? ಖಂಡ_space_ಅಗಲ : (size_t)ಒಟ್ಟು);

	// The column to which we will shift the text.
	destination_ಲಂಬಸಾಲು = (colnr_T)(non_white_ಲಂಬಸಾಲು - ಸರಿಸು_amount);

	// Now let's find out how much of the beginning of the line we can
	// reuse without modification.
	verbatim_copy_ಕೊನೆ = bd.textstart;
	verbatim_copy_ಅಗಲ = bd.ಆರಂಭ_vcol;

	// If "bd.startspaces" is set, "bd.textstart" points to the character
	// preceding the block. We have to subtract its width to obtain its
	// column number.
	ಆದರೆ (bd.startspaces)
	    verbatim_copy_ಅಗಲ -= bd.ಆರಂಭ_char_vcols;
	init_chartabsize_arg(&cts, curwin, 0, verbatim_copy_ಅಗಲ,
					     bd.textstart, verbatim_copy_ಕೊನೆ);
	ಆಗ (cts.cts_vcol < destination_ಲಂಬಸಾಲು)
	{
	    incr = lbr_chartabsize(&cts);
	    ಆದರೆ (cts.cts_vcol + incr > destination_ಲಂಬಸಾಲು)
		ನಿಲ್ಲಿಸು;
	    cts.cts_vcol += incr;
	    MB_PTR_ADV(cts.cts_ptr);
	}
	verbatim_copy_ಅಗಲ = cts.cts_vcol;
	verbatim_copy_ಕೊನೆ = cts.cts_ptr;
	clear_chartabsize_arg(&cts);

	// If "destination_col" is different from the width of the initial
	// part of the line that will be copied, it means we encountered a tab
	// character, which we will have to partly replace with spaces.
	fill = destination_ಲಂಬಸಾಲು - verbatim_copy_ಅಗಲ;

	// The replacement line will consist of:
	// - the beginning of the original line up to "verbatim_copy_end",
	// - "fill" number of spaces,
	// - the rest of the line, pointed to by non_white.
	fixedlen = verbatim_copy_ಕೊನೆ - oldp;
	new_ಸಾಲು_len = fixedlen + fill + (oldlen - (non_white - oldp));

	newp = alloc(new_ಸಾಲು_len + 1);
	ಆದರೆ (newp == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
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
    ಆದರೆ (new_ಸಾಲು_len >= oldlen)
	added = (ಪೂರ್ಣಾಂಕ)(new_ಸಾಲು_len - oldlen);
    ಇಲ್ಲದಿದ್ದರೆ
	added = 0 - (ಪೂರ್ಣಾಂಕ)(oldlen - new_ಸಾಲು_len);
    inserted_ಬೈಟುಗಳು(curwin->w_cursor.lnum, bd.textcol, added);
    State = oldstate;
    curwin->w_cursor.ಲಂಬಸಾಲು = oldcol;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    p_ri = old_p_ri;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Insert string "s" (b_insert ? before : after) block :AKelly
 * Caller must prepare for undo.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಖಂಡ_ಸೇರಿಸು(
    oparg_T		*oap,
    char_u		*s,
    size_t		slen,
    ಪೂರ್ಣಾಂಕ			b_ಸೇರಿಸು,
    ರಚನೆ ಖಂಡ_def	*bdp)
{
    ಪೂರ್ಣಾಂಕ		ts_val;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2 = 0;	// extra spaces to replace a cut TAB
    ಪೂರ್ಣಾಂಕ		spaces = 0;	// non-zero if cutting a TAB
    colnr_T	ಸ್ಥಾನಾಂತರ;		// pointer along new line
    colnr_T	startcol;	// column where insert starts
    char_u	*newp, *oldp;	// new, old lines
    linenr_T	lnum;		// loop var
    ಪೂರ್ಣಾಂಕ		oldstate = State;

    State = ಬಹುಲಕ_ಸೇರಿಸು;	// don't want MODE_REPLACE for State

    ಗಾಗಿ (lnum = oap->ಆರಂಭ.lnum + 1; lnum <= oap->ಕೊನೆ.lnum; lnum++)
    {
	ಖಂಡ_prep(oap, bdp, lnum, TRUE);
	ಆದರೆ (bdp->is_short && b_ಸೇರಿಸು)
	    ಮುಂದುವರಿ;	// OP_INSERT, line ends before block start

	oldp = ml_get(lnum);

	ಆದರೆ (b_ಸೇರಿಸು)
	{
	    ts_val = bdp->ಆರಂಭ_char_vcols;
	    spaces = bdp->startspaces;
	    ಆದರೆ (spaces != 0)
		ಎಣಿಕೆ_2 = ts_val - 1; // we're cutting a TAB
	    ಸ್ಥಾನಾಂತರ = bdp->textcol;
	}
	ಇಲ್ಲದಿದ್ದರೆ // append
	{
	    ts_val = bdp->ಕೊನೆ_char_vcols;
	    ಆದರೆ (!bdp->is_short) // spaces = padding after block
	    {
		spaces = (bdp->endspaces ? ts_val - bdp->endspaces : 0);
		ಆದರೆ (spaces != 0)
		    ಎಣಿಕೆ_2 = ts_val - 1; // we're cutting a TAB
		ಸ್ಥಾನಾಂತರ = bdp->textcol + bdp->textlen - (spaces != 0);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ // spaces = padding to block edge
	    {
		// if $ used, just append to EOL (ie spaces==0)
		ಆದರೆ (!bdp->is_MAX)
		    spaces = (oap->ಕೊನೆ_vcol - bdp->ಕೊನೆ_vcol) + 1;
		ಎಣಿಕೆ_2 = spaces;
		ಸ್ಥಾನಾಂತರ = bdp->textcol + bdp->textlen;
	    }
	}

	ಆದರೆ (has_mbyte && spaces > 0)
	    // avoid copying part of a multi-byte character
	    ಸ್ಥಾನಾಂತರ -= (*mb_ತಲೆ_off)(oldp, oldp + ಸ್ಥಾನಾಂತರ);

	ಆದರೆ (spaces < 0)  // can happen when the cursor was moved
	    spaces = 0;

	// Make sure the allocated size matches what is actually copied below.
	newp = alloc(ml_get_len(lnum) + spaces + slen
		    + (spaces > 0 && !bdp->is_short ? ts_val - spaces : 0)
								  + ಎಣಿಕೆ_2 + 1);
	ಆದರೆ (newp == ಖಾಲಿ)
	    ಮುಂದುವರಿ;

	// copy up to shifted part
	mch_memmove(newp, oldp, (size_t)ಸ್ಥಾನಾಂತರ);
	oldp += ಸ್ಥಾನಾಂತರ;

	// insert pre-padding
	vim_memset(newp + ಸ್ಥಾನಾಂತರ, ' ', (size_t)spaces);
	startcol = ಸ್ಥಾನಾಂತರ + spaces;

	// copy the new text
	mch_memmove(newp + startcol, s, slen);
	ಸ್ಥಾನಾಂತರ += (ಪೂರ್ಣಾಂಕ)slen;

	ಆದರೆ (spaces > 0 && !bdp->is_short)
	{
	    ಆದರೆ (*oldp == TAB)
	    {
		// insert post-padding
		vim_memset(newp + ಸ್ಥಾನಾಂತರ + spaces, ' ',
						    (size_t)(ts_val - spaces));
		// we're splitting a TAB, don't copy it
		oldp++;
		// We allowed for that TAB, remember this now
		ಎಣಿಕೆ_2++;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		// Not a TAB, no extra spaces
		ಎಣಿಕೆ_2 = spaces;
	}

	ಆದರೆ (spaces > 0)
	    ಸ್ಥಾನಾಂತರ += ಎಣಿಕೆ_2;
	STRCPY(newp + ಸ್ಥಾನಾಂತರ, oldp);

	ml_replace(lnum, newp, FALSE);

	ಆದರೆ (b_ಸೇರಿಸು)
	    // correct any text properties
	    inserted_ಬೈಟುಗಳು(lnum, startcol, (ಪೂರ್ಣಾಂಕ)slen);

	ಆದರೆ (lnum == oap->ಕೊನೆ.lnum)
	{
	    // Set "']" mark to the end of the block instead of the end of
	    // the insert in the first line.
	    curbuf->b_op_ಕೊನೆ.lnum = oap->ಕೊನೆ.lnum;
	    curbuf->b_op_ಕೊನೆ.ಲಂಬಸಾಲು = ಸ್ಥಾನಾಂತರ;
	    ಆದರೆ (curbuf->b_visual.vi_ಕೊನೆ.coladd)
	    {
		curbuf->b_visual.vi_ಕೊನೆ.ಲಂಬಸಾಲು += curbuf->b_visual.vi_ಕೊನೆ.coladd;
		curbuf->b_visual.vi_ಕೊನೆ.coladd = 0;
	    }
	}
    } // for all lnum

    ಬದಲಾಯಿತು_ಸಾಲುಗಳು(oap->ಆರಂಭ.lnum + 1, 0, oap->ಕೊನೆ.lnum + 1, 0L);

    State = oldstate;
}

/*
 * Handle a delete operation.
 *
 * Return FAIL if undo failed, OK otherwise.
 */
    ಪೂರ್ಣಾಂಕ
op_delete(oparg_T *oap)
{
    ಪೂರ್ಣಾಂಕ			n;
    linenr_T		lnum;
    char_u		*ptr;
    char_u		*newp, *oldp;
    ರಚನೆ ಖಂಡ_def	bd;
    linenr_T		old_lcount = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಂಕ			did_yank = FALSE;

    ಆದರೆ (curbuf->b_ml.ml_flags & ML_ಖಾಲಿಗಳು)	    // nothing to do
	ಹಿಂತಿರುಗಿ OK;

    // Nothing to delete, return here.	Do prepare undo, for op_change().
    ಆದರೆ (oap->ಖಾಲಿಗಳು_2)
	ಹಿಂತಿರುಗಿ u_save_cursor();

    ಆದರೆ (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (VIsual_select && oap->is_VIsual)
	// use register given with CTRL_R, defaults to zero
	oap->regname = VIsual_select_reg;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_CLIPMETHOD
    adjust_clip_reg(&oap->regname);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (has_mbyte)
	mb_adjust_opend(oap);

    /*
     * Imitate the strange Vi behaviour: If the delete spans more than one
     * line and motion_type == MCHAR and the result is a blank line, make the
     * delete linewise.  Don't do this for the change command or Visual mode.
     */
    ಆದರೆ (       oap->motion_type == MCHAR
	    && !oap->is_VIsual
	    && !oap->ಖಂಡ_ಬಹುಲಕ
	    && oap->ಸಾಲು_ಎಣಿಕೆ > 1
	    && oap->motion_force == NUL
	    && (vim_strchr(p_cpo, CPO_ಶಬ್ದ) != ಖಾಲಿ)
	    && oap->op_type == OP_DELETE)
    {
	ptr = ml_get(oap->ಕೊನೆ.lnum) + oap->ಕೊನೆ.ಲಂಬಸಾಲು;
	ಆದರೆ (*ptr != NUL)
	    ptr += oap->inclusive;
	ptr = skipwhite(ptr);
	ಆದರೆ (*ptr == NUL && inindent(0))
	    oap->motion_type = MLINE;
    }

    /*
     * Check for trying to delete (e.g. "D") in an empty line.
     * Note: For the change operator it is ok.
     */
    ಆದರೆ (       oap->motion_type == MCHAR
	    && oap->ಸಾಲು_ಎಣಿಕೆ == 1
	    && oap->op_type == OP_DELETE
	    && *ml_get(oap->ಆರಂಭ.lnum) == NUL)
    {
	/*
	 * It's an error to operate on an empty region, when 'E' included in
	 * 'cpoptions' (Vi compatible).
	 */
	ಆದರೆ (virtual_op)
	    // Virtual editing: Nothing gets deleted, but we set the '[ and ']
	    // marks as if it happened.
	    ಹೋಗು setmarks;
	ಆದರೆ (vim_strchr(p_cpo, CPO_EMPTYREGION) != ಖಾಲಿ)
	    beep_flush();
	ಹಿಂತಿರುಗಿ OK;
    }

    /*
     * Do a yank of whatever we're about to delete.
     * If a yank register was specified, put the deleted text into that
     * register.  For the black hole register '_' don't yank anything.
     */
    ಆದರೆ (oap->regname != '_')
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD_PROVIDER
	inc_clip_provider();
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (oap->regname != 0)
	{
	    // check for read-only register
	    ಆದರೆ (!valid_yank_reg(oap->regname, TRUE))
	    {
		beep_flush();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD_PROVIDER
		dec_clip_provider();
#ಆದರೆ_ಅಂತ್ಯ
		ಹಿಂತಿರುಗಿ OK;
	    }
	    get_yank_ರಿಜಿಸ್ಟರ್(oap->regname, TRUE); // yank into specif'd reg.
	    ಆದರೆ (op_yank(oap, TRUE, FALSE) == OK)   // yank without message
		did_yank = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    reset_y_append(); // not appending to unnamed register

	/*
	 * Put deleted text into register 1 and shift number registers if the
	 * delete contains a line break, or when using a specific operator (Vi
	 * compatible)
	 */
	ಆದರೆ (oap->motion_type == MLINE || oap->ಸಾಲು_ಎಣಿಕೆ > 1
							   || oap->use_reg_one)
	{
	    ಸರಿಸು_delete_registers();
	    ಆದರೆ (op_yank(oap, TRUE, FALSE) == OK)
		did_yank = TRUE;
	}

	// Yank into small delete register when no named register specified
	// and the delete is within one line.
	ಆದರೆ ((
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_CLIPMETHOD
	    ((clip_unnamed & CLIP_UNNAMED) && oap->regname == '*') ||
	    ((clip_unnamed & CLIP_UNNAMED_PLUS) && oap->regname == '+') ||
#ಆದರೆ_ಅಂತ್ಯ
	    oap->regname == 0) && oap->motion_type != MLINE
						      && oap->ಸಾಲು_ಎಣಿಕೆ == 1)
	{
	    oap->regname = '-';
	    get_yank_ರಿಜಿಸ್ಟರ್(oap->regname, TRUE);
	    ಆದರೆ (op_yank(oap, TRUE, FALSE) == OK)
		did_yank = TRUE;
	    oap->regname = 0;
	}

	/*
	 * If there's too much stuff to fit in the yank register, then get a
	 * confirmation before doing the delete. This is crude, but simple.
	 * And it avoids doing a delete of something we can't put back if we
	 * want.
	 */
	ಆದರೆ (!did_yank)
	{
	    ಪೂರ್ಣಾಂಕ ಸಂದೇಶ_silent_save = ಸಂದೇಶ_silent;

	    ಸಂದೇಶ_silent = 0;	// must display the prompt
	    n = ask_yesno((char_u *)_("cannot yank; delete anyway"), TRUE);
	    ಸಂದೇಶ_silent = ಸಂದೇಶ_silent_save;
	    ಆದರೆ (n != 'y')
	    {
		emsg(_(e_command_aborted));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD_PROVIDER
		dec_clip_provider();
#ಆದರೆ_ಅಂತ್ಯ
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}

#ಆದರೆ defined(FEAT_EVAL)
	ಆದರೆ (did_yank && has_textyankpost())
	    yank_do_autocmd(oap, get_y_current());
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD_PROVIDER
	dec_clip_provider();
#ಆದರೆ_ಅಂತ್ಯ
    }

    /*
     * block mode delete
     */
    ಆದರೆ (oap->ಖಂಡ_ಬಹುಲಕ)
    {
	ಆದರೆ (u_save((linenr_T)(oap->ಆರಂಭ.lnum - 1),
			       (linenr_T)(oap->ಕೊನೆ.lnum + 1)) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಗಾಗಿ (lnum = curwin->w_cursor.lnum; lnum <= oap->ಕೊನೆ.lnum; ++lnum)
	{
	    ಖಂಡ_prep(oap, &bd, lnum, TRUE);
	    ಆದರೆ (bd.textlen == 0)	// nothing to delete
		ಮುಂದುವರಿ;

	    // Adjust cursor position for tab replaced by spaces and 'lbr'.
	    ಆದರೆ (lnum == curwin->w_cursor.lnum)
	    {
		curwin->w_cursor.ಲಂಬಸಾಲು = bd.textcol + bd.startspaces;
		curwin->w_cursor.coladd = 0;
	    }

	    // "n" == number of chars deleted
	    // If we delete a TAB, it may be replaced by several characters.
	    // Thus the number of characters may increase!
	    n = bd.textlen - bd.startspaces - bd.endspaces;
	    oldp = ml_get(lnum);
	    newp = alloc(ml_get_len(lnum) + 1 - n);
	    ಆದರೆ (newp == ಖಾಲಿ)
		ಮುಂದುವರಿ;
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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	    ಆದರೆ (curbuf->b_has_textprop && n != 0)
		adjust_prop_columns(lnum, bd.textcol, -n, 0);
#ಆದರೆ_ಅಂತ್ಯ
	}

	check_cursor_ಲಂಬಸಾಲು();
	ಬದಲಾಯಿತು_ಸಾಲುಗಳು(curwin->w_cursor.lnum, curwin->w_cursor.ಲಂಬಸಾಲು,
						       oap->ಕೊನೆ.lnum + 1, 0L);
	oap->ಸಾಲು_ಎಣಿಕೆ = 0;	    // no lines deleted
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (oap->motion_type == MLINE)
    {
	ಆದರೆ (oap->op_type == OP_CHANGE)
	{
	    // Delete the lines except the first one.  Temporarily move the
	    // cursor to the next line.  Save the current line number, if the
	    // last line is deleted it may be changed.
	    ಆದರೆ (oap->ಸಾಲು_ಎಣಿಕೆ > 1)
	    {
		lnum = curwin->w_cursor.lnum;
		++curwin->w_cursor.lnum;
		del_ಸಾಲುಗಳು((ದೀರ್ಘ)(oap->ಸಾಲು_ಎಣಿಕೆ - 1), TRUE);
		curwin->w_cursor.lnum = lnum;
	    }
	    ಆದರೆ (u_save_cursor() == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	    ಆದರೆ (curbuf->b_p_ai)		    // don't delete indent
	    {
		beginline(BL_WHITE);	    // cursor on first non-white
		did_ai = TRUE;		    // delete the indent when ESC hit
		ai_ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		beginline(0);		    // cursor in column 0
	    truncate_ಸಾಲು(FALSE);   // delete the rest of the line,
				    // leaving cursor past last char in line
	    ಆದರೆ (oap->ಸಾಲು_ಎಣಿಕೆ > 1)
		u_clearline();	    // "U" command not possible after "2cc"
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    del_ಸಾಲುಗಳು(oap->ಸಾಲು_ಎಣಿಕೆ, TRUE);
	    beginline(BL_WHITE | BL_FIX);
	    u_clearline();	// "U" command not possible after "dd"
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (virtual_op)
	{
	    ಪೂರ್ಣಾಂಕ		endcol = 0;

	    // For virtualedit: break the tabs that are partly included.
	    ಆದರೆ (gchar_pos(&oap->ಆರಂಭ) == '\t')
	    {
		ಆದರೆ (u_save_cursor() == FAIL)	// save first line for undo
		    ಹಿಂತಿರುಗಿ FAIL;
		ಆದರೆ (oap->ಸಾಲು_ಎಣಿಕೆ == 1)
		    endcol = getviscol2(oap->ಕೊನೆ.ಲಂಬಸಾಲು, oap->ಕೊನೆ.coladd);
		coladvance_force(getviscol2(oap->ಆರಂಭ.ಲಂಬಸಾಲು, oap->ಆರಂಭ.coladd));
		oap->ಆರಂಭ = curwin->w_cursor;
		ಆದರೆ (oap->ಸಾಲು_ಎಣಿಕೆ == 1)
		{
		    coladvance(endcol);
		    oap->ಕೊನೆ.ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;
		    oap->ಕೊನೆ.coladd = curwin->w_cursor.coladd;
		    curwin->w_cursor = oap->ಆರಂಭ;
		}
	    }

	    // Break a tab only when it's included in the area.
	    ಆದರೆ (gchar_pos(&oap->ಕೊನೆ) == '\t'
				     && (ಪೂರ್ಣಾಂಕ)oap->ಕೊನೆ.coladd < oap->inclusive)
	    {
		// save last line for undo
		ಆದರೆ (u_save((linenr_T)(oap->ಕೊನೆ.lnum - 1),
				       (linenr_T)(oap->ಕೊನೆ.lnum + 1)) == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
		curwin->w_cursor = oap->ಕೊನೆ;
		coladvance_force(getviscol2(oap->ಕೊನೆ.ಲಂಬಸಾಲು, oap->ಕೊನೆ.coladd));
		oap->ಕೊನೆ = curwin->w_cursor;
		curwin->w_cursor = oap->ಆರಂಭ;
	    }
	    ಆದರೆ (has_mbyte)
		mb_adjust_opend(oap);
	}

	ಆದರೆ (oap->ಸಾಲು_ಎಣಿಕೆ == 1)	// delete characters within one line
	{
	    ಆದರೆ (u_save_cursor() == FAIL)	// save line for undo
		ಹಿಂತಿರುಗಿ FAIL;

	    // if 'cpoptions' contains '$', display '$' at end of change
	    ಆದರೆ (       vim_strchr(p_cpo, CPO_DOLLAR) != ಖಾಲಿ
		    && oap->op_type == OP_CHANGE
		    && oap->ಕೊನೆ.lnum == curwin->w_cursor.lnum
		    && !oap->is_VIsual)
		display_dollar(oap->ಕೊನೆ.ಲಂಬಸಾಲು - !oap->inclusive);

	    n = oap->ಕೊನೆ.ಲಂಬಸಾಲು - oap->ಆರಂಭ.ಲಂಬಸಾಲು + 1 - !oap->inclusive;

	    ಆದರೆ (virtual_op)
	    {
		// fix up things for virtualedit-delete:
		// break the tabs which are going to get in our way
		ಪೂರ್ಣಾಂಕ		len = ml_get_curline_len();

		ಆದರೆ (oap->ಕೊನೆ.coladd != 0
			&& (ಪೂರ್ಣಾಂಕ)oap->ಕೊನೆ.ಲಂಬಸಾಲು >= len - 1
			&& !(oap->ಆರಂಭ.coladd && (ಪೂರ್ಣಾಂಕ)oap->ಕೊನೆ.ಲಂಬಸಾಲು >= len - 1))
		    n++;
		// Delete at least one char (e.g, when on a control char).
		ಆದರೆ (n == 0 && oap->ಆರಂಭ.coladd != oap->ಕೊನೆ.coladd)
		    n = 1;

		// When deleted a char in the line, reset coladd.
		ಆದರೆ (gchar_cursor() != NUL)
		    curwin->w_cursor.coladd = 0;
	    }
	    (ಶೂನ್ಯ)del_ಬೈಟುಗಳು((ದೀರ್ಘ)n, !virtual_op,
			    oap->op_type == OP_DELETE && !oap->is_VIsual);
	}
	ಇಲ್ಲದಿದ್ದರೆ				// delete characters between lines
	{
	    pos_T   curpos;

	    // save deleted and changed lines for undo
	    ಆದರೆ (u_save((linenr_T)(curwin->w_cursor.lnum - 1),
		 (linenr_T)(curwin->w_cursor.lnum + oap->ಸಾಲು_ಎಣಿಕೆ)) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;

	    truncate_ಸಾಲು(TRUE);	// delete from cursor to end of line

	    curpos = curwin->w_cursor;	// remember curwin->w_cursor
	    ++curwin->w_cursor.lnum;
	    del_ಸಾಲುಗಳು((ದೀರ್ಘ)(oap->ಸಾಲು_ಎಣಿಕೆ - 2), FALSE);

	    // delete from start of line until op_end
	    n = (oap->ಕೊನೆ.ಲಂಬಸಾಲು + 1 - !oap->inclusive);
	    curwin->w_cursor.ಲಂಬಸಾಲು = 0;
	    (ಶೂನ್ಯ)del_ಬೈಟುಗಳು((ದೀರ್ಘ)n, !virtual_op,
			    oap->op_type == OP_DELETE && !oap->is_VIsual);
	    curwin->w_cursor = curpos;	// restore curwin->w_cursor
	    (ಶೂನ್ಯ)do_join(2, FALSE, FALSE, FALSE, FALSE);
	}
	ಆದರೆ (oap->op_type == OP_DELETE)
	    auto_format(FALSE, TRUE);
    }

    msgmore(curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - old_lcount);

setmarks:
    ಆದರೆ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	ಆದರೆ (oap->ಖಂಡ_ಬಹುಲಕ)
	{
	    curbuf->b_op_ಕೊನೆ.lnum = oap->ಕೊನೆ.lnum;
	    curbuf->b_op_ಕೊನೆ.ಲಂಬಸಾಲು = oap->ಆರಂಭ.ಲಂಬಸಾಲು;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    curbuf->b_op_ಕೊನೆ = oap->ಆರಂಭ;
	curbuf->b_op_ಆರಂಭ = oap->ಆರಂಭ;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Adjust end of operating area for ending on a multi-byte character.
 * Used for deletion.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
mb_adjust_opend(oparg_T *oap)
{
    char_u	*ಸಾಲು;
    char_u	*ptr;

    ಆದರೆ (!oap->inclusive)
	ಹಿಂತಿರುಗಿ;

    ಸಾಲು = ml_get(oap->ಕೊನೆ.lnum);
    ptr = ಸಾಲು + oap->ಕೊನೆ.ಲಂಬಸಾಲು;
    ಆದರೆ (*ptr != NUL)
    {
	ptr -= (*mb_ತಲೆ_off)(ಸಾಲು, ptr);
	ptr += (*mb_ptr2len)(ptr) - 1;
	oap->ಕೊನೆ.ಲಂಬಸಾಲು = ptr - ಸಾಲು;
    }
}

/*
 * Replace the character under the cursor with "c".
 * This takes care of multi-byte characters.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
replace_character(ಪೂರ್ಣಾಂಕ c)
{
    ಪೂರ್ಣಾಂಕ n = State;

    State = ಬಹುಲಕ_REPLACE;
    ins_char(c);
    State = n;
    // Backup to the replaced character.
    dec_cursor();
}

/*
 * Replace a whole area with one character.
 */
    ಪೂರ್ಣಾಂಕ
op_replace(oparg_T *oap, ಪೂರ್ಣಾಂಕ c)
{
    ಪೂರ್ಣಾಂಕ			n, numc;
    ಪೂರ್ಣಾಂಕ			ಅಂಕೆ_chars;
    char_u		*newp, *oldp;
    size_t		newlen, oldlen;
    ರಚನೆ ಖಂಡ_def	bd;
    char_u		*after_p = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ			had_ctrl_v_cr = FALSE;

    ಆದರೆ ((curbuf->b_ml.ml_flags & ML_ಖಾಲಿಗಳು ) || oap->ಖಾಲಿಗಳು_2)
	ಹಿಂತಿರುಗಿ OK;	    // nothing to do

    ಆದರೆ (c == REPLACE_CR_NCHAR)
    {
	had_ctrl_v_cr = TRUE;
	c = CAR;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == REPLACE_NL_NCHAR)
    {
	had_ctrl_v_cr = TRUE;
	c = NL;
    }

    ಆದರೆ (has_mbyte)
	mb_adjust_opend(oap);

    ಆದರೆ (u_save((linenr_T)(oap->ಆರಂಭ.lnum - 1),
				       (linenr_T)(oap->ಕೊನೆ.lnum + 1)) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    /*
     * block mode replace
     */
    ಆದರೆ (oap->ಖಂಡ_ಬಹುಲಕ)
    {
	bd.is_MAX = (curwin->w_curswant == MAXCOL);
	ಗಾಗಿ ( ; curwin->w_cursor.lnum <= oap->ಕೊನೆ.lnum; ++curwin->w_cursor.lnum)
	{
	    curwin->w_cursor.ಲಂಬಸಾಲು = 0;  // make sure cursor position is valid
	    ಖಂಡ_prep(oap, &bd, curwin->w_cursor.lnum, TRUE);
	    ಆದರೆ (bd.textlen == 0 && (!virtual_op || bd.is_MAX))
		ಮುಂದುವರಿ;	    // nothing to replace

	    // n == number of extra chars required
	    // If we split a TAB, it may be replaced by several characters.
	    // Thus the number of characters may increase!
	    // If the range starts in virtual space, count the initial
	    // coladd offset as part of "startspaces"
	    ಆದರೆ (virtual_op && bd.is_short && *bd.textstart == NUL)
	    {
		pos_T vpos;

		vpos.lnum = curwin->w_cursor.lnum;
		getvpos(&vpos, oap->ಆರಂಭ_vcol);
		bd.startspaces += vpos.coladd;
		n = bd.startspaces;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		// allow for pre spaces
		n = (bd.startspaces ? bd.ಆರಂಭ_char_vcols - 1 : 0);

	    // allow for post spp
	    n += (bd.endspaces
		    && !bd.is_oneChar
		    && bd.ಕೊನೆ_char_vcols > 0) ? bd.ಕೊನೆ_char_vcols - 1 : 0;
	    // Figure out how many characters to replace.
	    numc = oap->ಕೊನೆ_vcol - oap->ಆರಂಭ_vcol + 1;
	    ಆದರೆ (bd.is_short && (!virtual_op || bd.is_MAX))
		numc -= (oap->ಕೊನೆ_vcol - bd.ಕೊನೆ_vcol) + 1;

	    // A double-wide character can be replaced only up to half the
	    // times.
	    ಆದರೆ ((*mb_char2cells)(c) > 1)
	    {
		ಆದರೆ ((numc & 1) && !bd.is_short)
		{
		    ++bd.endspaces;
		    ++n;
		}
		numc = numc / 2;
	    }

	    // Compute bytes needed, move character count to num_chars.
	    ಅಂಕೆ_chars = numc;
	    numc *= (*mb_char2len)(c);
	    // oldlen includes textlen, so don't double count
	    n += numc - bd.textlen;

	    oldp = ml_get_curline();
	    oldlen = ml_get_curline_len();
	    newp = alloc(oldlen + 1 + n);
	    ಆದರೆ (newp == ಖಾಲಿ)
		ಮುಂದುವರಿ;
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
	    ಆದರೆ (had_ctrl_v_cr || (c != '\r' && c != '\n'))
	    {
		ಆದರೆ (has_mbyte)
		{
		    ಆಗ (--ಅಂಕೆ_chars >= 0)
			newlen += (*mb_char2bytes)(c, newp + newlen);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    vim_memset(newp + newlen, c, (size_t)numc);
		    newlen += numc;
		}
		ಆದರೆ (!bd.is_short)
		{
		    // insert post-spaces
		    vim_memset(newp + newlen, ' ', (size_t)bd.endspaces);
		    // copy the part after the changed part
		    STRCPY(newp + newlen + bd.endspaces,
				oldp + bd.textcol + bd.textlen);
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Replacing with \r or \n means splitting the line.
		after_p = alloc(oldlen + 1 + n - newlen);
		ಆದರೆ (after_p != ಖಾಲಿ)
		    STRCPY(after_p, oldp + bd.textcol + bd.textlen);
	    }

	    // replace the line
	    ml_replace(curwin->w_cursor.lnum, newp, FALSE);
	    ಆದರೆ (after_p != ಖಾಲಿ)
	    {
		ml_append(curwin->w_cursor.lnum++, after_p, 0, FALSE);
		appended_ಸಾಲುಗಳು_mark(curwin->w_cursor.lnum, 1L);
		oap->ಕೊನೆ.lnum++;
		vim_free(after_p);
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	/*
	 * MCHAR and MLINE motion replace.
	 */
	ಆದರೆ (oap->motion_type == MLINE)
	{
	    oap->ಆರಂಭ.ಲಂಬಸಾಲು = 0;
	    curwin->w_cursor.ಲಂಬಸಾಲು = 0;
	    oap->ಕೊನೆ.ಲಂಬಸಾಲು = ml_get_len(oap->ಕೊನೆ.lnum);
	    ಆದರೆ (oap->ಕೊನೆ.ಲಂಬಸಾಲು)
		--oap->ಕೊನೆ.ಲಂಬಸಾಲು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!oap->inclusive)
	    dec(&(oap->ಕೊನೆ));

	ಆಗ (LTOREQ_POS(curwin->w_cursor, oap->ಕೊನೆ))
	{
	    ಪೂರ್ಣಾಂಕ ಮುಗಿದ = FALSE;

	    n = gchar_cursor();
	    ಆದರೆ (n != NUL)
	    {
		ಪೂರ್ಣಾಂಕ new_byte_len = (*mb_char2len)(c);
		ಪೂರ್ಣಾಂಕ old_byte_len = mb_ptr2len(ml_get_cursor());

		ಆದರೆ (new_byte_len > 1 || old_byte_len > 1)
		{
		    // This is slow, but it handles replacing a single-byte
		    // with a multi-byte and the other way around.
		    ಆದರೆ (curwin->w_cursor.lnum == oap->ಕೊನೆ.lnum)
			oap->ಕೊನೆ.ಲಂಬಸಾಲು += new_byte_len - old_byte_len;
		    replace_character(c);
		    ಮುಗಿದ = TRUE;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (n == TAB)
		    {
			ಪೂರ್ಣಾಂಕ ಕೊನೆ_vcol = 0;

			ಆದರೆ (curwin->w_cursor.lnum == oap->ಕೊನೆ.lnum)
			{
			    // oap->end has to be recalculated when
			    // the tab breaks
			    ಕೊನೆ_vcol = getviscol2(oap->ಕೊನೆ.ಲಂಬಸಾಲು,
							     oap->ಕೊನೆ.coladd);
			}
			coladvance_force(getviscol());
			ಆದರೆ (curwin->w_cursor.lnum == oap->ಕೊನೆ.lnum)
			    getvpos(&oap->ಕೊನೆ, ಕೊನೆ_vcol);
		    }
		    // with "coladd" set may move to just after a TAB
		    ಆದರೆ (gchar_cursor() != NUL)
		    {
			PBYTE(curwin->w_cursor, c);
			ಮುಗಿದ = TRUE;
		    }
		}
	    }
	    ಆದರೆ (!ಮುಗಿದ && virtual_op && curwin->w_cursor.lnum == oap->ಕೊನೆ.lnum)
	    {
		ಪೂರ್ಣಾಂಕ virtcols = oap->ಕೊನೆ.coladd;

		ಆದರೆ (curwin->w_cursor.lnum == oap->ಆರಂಭ.lnum
			&& oap->ಆರಂಭ.ಲಂಬಸಾಲು == oap->ಕೊನೆ.ಲಂಬಸಾಲು && oap->ಆರಂಭ.coladd)
		    virtcols -= oap->ಆರಂಭ.coladd;

		// oap->end has been trimmed so it's effectively inclusive;
		// as a result an extra +1 must be counted so we don't
		// trample the NUL byte.
		coladvance_force(getviscol2(oap->ಕೊನೆ.ಲಂಬಸಾಲು, oap->ಕೊನೆ.coladd) + 1);
		curwin->w_cursor.ಲಂಬಸಾಲು -= (virtcols + 1);
		ಗಾಗಿ (; virtcols >= 0; virtcols--)
		{
		    ಆದರೆ ((*mb_char2len)(c) > 1)
		       replace_character(c);
		    ಇಲ್ಲದಿದ್ದರೆ
			PBYTE(curwin->w_cursor, c);
		    ಆದರೆ (inc(&curwin->w_cursor) == -1)
			ನಿಲ್ಲಿಸು;
		}
	    }

	    // Advance to next character, stop at the end of the file.
	    ಆದರೆ (inc_cursor() == -1)
		ನಿಲ್ಲಿಸು;
	}
    }

    curwin->w_cursor = oap->ಆರಂಭ;
    check_cursor();
    ಬದಲಾಯಿತು_ಸಾಲುಗಳು(oap->ಆರಂಭ.lnum, oap->ಆರಂಭ.ಲಂಬಸಾಲು, oap->ಕೊನೆ.lnum + 1, 0L);

    ಆದರೆ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set "'[" and "']" marks.
	curbuf->b_op_ಆರಂಭ = oap->ಆರಂಭ;
	curbuf->b_op_ಕೊನೆ = oap->ಕೊನೆ;
    }

    ಹಿಂತಿರುಗಿ OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ swapchars(ಪೂರ್ಣಾಂಕ op_type, pos_T *pos, ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ);

/*
 * Handle the (non-standard vi) tilde operator.  Also for "gu", "gU" and "g?".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
op_tilde(oparg_T *oap)
{
    pos_T		pos;
    ರಚನೆ ಖಂಡ_def	bd;
    ಪೂರ್ಣಾಂಕ			did_change = FALSE;

    ಆದರೆ (u_save((linenr_T)(oap->ಆರಂಭ.lnum - 1),
				       (linenr_T)(oap->ಕೊನೆ.lnum + 1)) == FAIL)
	ಹಿಂತಿರುಗಿ;

    pos = oap->ಆರಂಭ;
    ಆದರೆ (oap->ಖಂಡ_ಬಹುಲಕ)		    // Visual block mode
    {
	ಗಾಗಿ (; pos.lnum <= oap->ಕೊನೆ.lnum; ++pos.lnum)
	{
	    ಪೂರ್ಣಾಂಕ one_change;

	    ಖಂಡ_prep(oap, &bd, pos.lnum, FALSE);
	    pos.ಲಂಬಸಾಲು = bd.textcol;
	    one_change = swapchars(oap->op_type, &pos, bd.textlen);
	    did_change |= one_change;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
	    ಆದರೆ (netbeans_ಸಕ್ರಿಯ() && one_change)
	    {
		char_u *ptr;

		netbeans_removed(curbuf, pos.lnum, bd.textcol,
							    (ದೀರ್ಘ)bd.textlen);
		// get the line now, it may have been flushed
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, bd.textcol,
						&ptr[bd.textcol], bd.textlen);
	    }
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಆದರೆ (did_change)
	    ಬದಲಾಯಿತು_ಸಾಲುಗಳು(oap->ಆರಂಭ.lnum, 0, oap->ಕೊನೆ.lnum + 1, 0L);
    }
    ಇಲ್ಲದಿದ್ದರೆ				    // not block mode
    {
	ಆದರೆ (oap->motion_type == MLINE)
	{
	    oap->ಆರಂಭ.ಲಂಬಸಾಲು = 0;
	    pos.ಲಂಬಸಾಲು = 0;
	    oap->ಕೊನೆ.ಲಂಬಸಾಲು = ml_get_len(oap->ಕೊನೆ.lnum);
	    ಆದರೆ (oap->ಕೊನೆ.ಲಂಬಸಾಲು)
		--oap->ಕೊನೆ.ಲಂಬಸಾಲು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!oap->inclusive)
	    dec(&(oap->ಕೊನೆ));

	ಆದರೆ (pos.lnum == oap->ಕೊನೆ.lnum)
	    did_change = swapchars(oap->op_type, &pos,
						  oap->ಕೊನೆ.ಲಂಬಸಾಲು - pos.ಲಂಬಸಾಲು + 1);
	ಇಲ್ಲದಿದ್ದರೆ
	    ಗಾಗಿ (;;)
	    {
		did_change |= swapchars(oap->op_type, &pos,
			pos.lnum == oap->ಕೊನೆ.lnum ? oap->ಕೊನೆ.ಲಂಬಸಾಲು + 1
						  : ml_get_pos_len(&pos));
		ಆದರೆ (LTOREQ_POS(oap->ಕೊನೆ, pos) || inc(&pos) == -1)
		    ನಿಲ್ಲಿಸು;
	    }
	ಆದರೆ (did_change)
	{
	    ಬದಲಾಯಿತು_ಸಾಲುಗಳು(oap->ಆರಂಭ.lnum, oap->ಆರಂಭ.ಲಂಬಸಾಲು, oap->ಕೊನೆ.lnum + 1,
									  0L);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
	    ಆದರೆ (netbeans_ಸಕ್ರಿಯ())
	    {
		char_u *ptr;
		ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2;

		pos = oap->ಆರಂಭ;
		ಆಗ (pos.lnum < oap->ಕೊನೆ.lnum)
		{
		    ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		    ಎಣಿಕೆ_2 = ml_get_buf_len(curbuf, pos.lnum) - pos.ಲಂಬಸಾಲು;
		    netbeans_removed(curbuf, pos.lnum, pos.ಲಂಬಸಾಲು, (ದೀರ್ಘ)ಎಣಿಕೆ_2);
		    // get the line again, it may have been flushed
		    ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		    netbeans_inserted(curbuf, pos.lnum, pos.ಲಂಬಸಾಲು,
							&ptr[pos.ಲಂಬಸಾಲು], ಎಣಿಕೆ_2);
		    pos.ಲಂಬಸಾಲು = 0;
		    pos.lnum++;
		}
		ಎಣಿಕೆ_2 = oap->ಕೊನೆ.ಲಂಬಸಾಲು - pos.ಲಂಬಸಾಲು + 1;
		netbeans_removed(curbuf, pos.lnum, pos.ಲಂಬಸಾಲು, (ದೀರ್ಘ)ಎಣಿಕೆ_2);
		// get the line again, it may have been flushed
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, pos.ಲಂಬಸಾಲು,
							&ptr[pos.ಲಂಬಸಾಲು], ಎಣಿಕೆ_2);
	    }
#ಆದರೆ_ಅಂತ್ಯ
	}
    }

    ಆದರೆ (!did_change && oap->is_VIsual)
	// No change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    ಆದರೆ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set '[ and '] marks.
	curbuf->b_op_ಆರಂಭ = oap->ಆರಂಭ;
	curbuf->b_op_ಕೊನೆ = oap->ಕೊನೆ;
    }

    ಆದರೆ (oap->ಸಾಲು_ಎಣಿಕೆ > p_report)
	smsg(NGETTEXT("%ld line changed", "%ld lines changed",
					    oap->ಸಾಲು_ಎಣಿಕೆ), oap->ಸಾಲು_ಎಣಿಕೆ);
}

/*
 * Invoke swapchar() on "length" bytes at position "pos".
 * "pos" is advanced to just after the changed characters.
 * "length" is rounded up to include the whole last multi-byte character.
 * Also works correctly when the number of bytes changes.
 * Returns TRUE if some character was changed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
swapchars(ಪೂರ್ಣಾಂಕ op_type, pos_T *pos, ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ)
{
    ಪೂರ್ಣಾಂಕ todo;
    ಪೂರ್ಣಾಂಕ	did_change = 0;

    ಗಾಗಿ (todo = ಉದ್ದಳತೆ; todo > 0; --todo)
    {
	ಆದರೆ (has_mbyte)
	{
	    ಪೂರ್ಣಾಂಕ len = (*mb_ptr2len)(ml_get_pos(pos));

	    // we're counting bytes, not characters
	    ಆದರೆ (len > 0)
		todo -= len - 1;
	}
	did_change |= swapchar(op_type, pos);
	ಆದರೆ (inc(pos) == -1)    // at end of file
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ did_change;
}

/*
 * If op_type == OP_UPPER: make uppercase,
 * if op_type == OP_LOWER: make lowercase,
 * if op_type == OP_ROT13: do rot13 encoding,
 * else swap case of character at 'pos'
 * returns TRUE when something actually changed.
 */
    ಪೂರ್ಣಾಂಕ
swapchar(ಪೂರ್ಣಾಂಕ op_type, pos_T *pos)
{
    ಪೂರ್ಣಾಂಕ	    c;
    ಪೂರ್ಣಾಂಕ	    nc;

    c = gchar_pos(pos);

    // Only do rot13 encoding for ASCII characters.
    ಆದರೆ (c >= 0x80 && op_type == OP_ROT13)
	ಹಿಂತಿರುಗಿ FALSE;

    // ~ is OP_NOP, g~ is OP_TILDE, gU is OP_UPPER
    ಆದರೆ ((op_type == OP_ಊರ್ಧ್ವ || op_type == OP_NOP || op_type == OP_TILDE)
	    && c == 0xdf
	    && (enc_latin1like || STRCMP(p_enc, "iso-8859-2") == 0))
    {
	pos_T   sp = curwin->w_cursor;

	// Special handling for lowercase German sharp s (ß): convert to uppercase (ẞ).
	curwin->w_cursor = *pos;
	del_char(FALSE);
	ins_char(0x1E9E);
	curwin->w_cursor = sp;
	ಹಿಂತಿರುಗಿ TRUE;
    }

    ಆದರೆ (enc_dbcs != 0 && c >= 0x100)	// No lower/uppercase letter
	ಹಿಂತಿರುಗಿ FALSE;
    nc = c;
    ಆದರೆ (MB_ISLOWER(c))
    {
	ಆದರೆ (op_type == OP_ROT13)
	    nc = ROT13(c, 'a');
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (op_type != OP_ಅಧೋ)
	    nc = MB_TOUPPER(c);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (MB_ISUPPER(c))
    {
	ಆದರೆ (op_type == OP_ROT13)
	    nc = ROT13(c, 'A');
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (op_type != OP_ಊರ್ಧ್ವ)
	    nc = MB_TOLOWER(c);
    }
    ಆದರೆ (nc != c)
    {
	ಆದರೆ (enc_utf8 && (c >= 0x80 || nc >= 0x80))
	{
	    pos_T   sp = curwin->w_cursor;

	    curwin->w_cursor = *pos;
	    // don't use del_char(), it also removes composing chars
	    del_ಬೈಟುಗಳು(utf_ptr2len(ml_get_cursor()), FALSE, FALSE);
	    ins_char(nc);
	    curwin->w_cursor = sp;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    PBYTE(*pos, nc);
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * op_insert - Insert and append operators for Visual mode.
 */
    ಶೂನ್ಯ
op_ಸೇರಿಸು(oparg_T *oap, ದೀರ್ಘ count1)
{
    ದೀರ್ಘ		pre_textlen = 0;
    colnr_T		ind_pre_ಲಂಬಸಾಲು = 0, ind_post_ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ			ind_pre_vcol = 0, ind_post_vcol = 0;
    ರಚನೆ ಖಂಡ_def	bd;
    ಪೂರ್ಣಾಂಕ			i;
    pos_T		t1;
    pos_T		ಆರಂಭ_ಸೇರಿಸು;

    // edit() changes this - record it for OP_APPEND
    bd.is_MAX = (curwin->w_curswant == MAXCOL);

    // vis block is still marked. Get rid of it now.
    curwin->w_cursor.lnum = oap->ಆರಂಭ.lnum;
    update_screen(UPD_INVERTED);

    ಆದರೆ (oap->ಖಂಡ_ಬಹುಲಕ)
    {
	// When 'virtualedit' is used, need to insert the extra spaces before
	// doing block_prep().  When only "block" is used, virtual edit is
	// already disabled, but still need it when calling
	// coladvance_force().
	// coladvance_force() uses get_ve_flags() to get the 'virtualedit'
	// state for the current window.  To override that state, we need to
	// set the window-local value of ve_flags rather than the global value.
	ಆದರೆ (curwin->w_cursor.coladd > 0)
	{
	    ಪೂರ್ಣಾಂಕ		old_ve_flags = curwin->w_ve_flags;

	    ಆದರೆ (u_save_cursor() == FAIL)
		ಹಿಂತಿರುಗಿ;

	    curwin->w_ve_flags = VE_ALL;
	    coladvance_force(oap->op_type == OP_APPEND
					   ? oap->ಕೊನೆ_vcol + 1 : getviscol());
	    ಆದರೆ (oap->op_type == OP_APPEND)
		--curwin->w_cursor.ಲಂಬಸಾಲು;
	    curwin->w_ve_flags = old_ve_flags;
	}
	// Get the info about the block before entering the text
	ಖಂಡ_prep(oap, &bd, oap->ಆರಂಭ.lnum, TRUE);
	// Get indent information
	ind_pre_ಲಂಬಸಾಲು = (colnr_T)getwhitecols_curline();
	ind_pre_vcol = get_indent();
	pre_textlen = ml_get_len(oap->ಆರಂಭ.lnum) - bd.textcol;
	ಆದರೆ (oap->op_type == OP_APPEND)
	    pre_textlen -= bd.textlen;
    }

    ಆದರೆ (oap->op_type == OP_APPEND)
    {
	ಆದರೆ (oap->ಖಂಡ_ಬಹುಲಕ && curwin->w_cursor.coladd == 0)
	{
	    // Move the cursor to the character right of the block.
	    curwin->w_ಗಣ_curswant = true;
	    ಆಗ (*ml_get_cursor() != NUL
		    && (curwin->w_cursor.ಲಂಬಸಾಲು < bd.textcol + bd.textlen))
		++curwin->w_cursor.ಲಂಬಸಾಲು;
	    ಆದರೆ (bd.is_short && !bd.is_MAX)
	    {
		// First line was too short, make it longer and adjust the
		// values in "bd".
		ಆದರೆ (u_save_cursor() == FAIL)
		    ಹಿಂತಿರುಗಿ;
		ಗಾಗಿ (i = 0; i < bd.endspaces; ++i)
		    ins_char(' ');
		bd.textlen += bd.endspaces;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    curwin->w_cursor = oap->ಕೊನೆ;
	    check_cursor_ಲಂಬಸಾಲು();

	    // Works just like an 'i'nsert on the next character.
	    ಆದರೆ (!LINEEMPTY(curwin->w_cursor.lnum)
		    && oap->ಆರಂಭ_vcol != oap->ಕೊನೆ_vcol)
		inc_cursor();
	}
    }

    t1 = oap->ಆರಂಭ;
    ಆರಂಭ_ಸೇರಿಸು = curwin->w_cursor;
    (ಶೂನ್ಯ)edit(NUL, FALSE, (linenr_T)count1);

    // When a tab was inserted, and the characters in front of the tab
    // have been converted to a tab as well, the column of the cursor
    // might have actually been reduced, so need to adjust here.
    ಆದರೆ (t1.lnum == curbuf->b_op_ಆರಂಭ_orig.lnum
	    && LT_POS(curbuf->b_op_ಆರಂಭ_orig, t1))
	oap->ಆರಂಭ = curbuf->b_op_ಆರಂಭ_orig;

    // If user has moved off this line, we don't know what to do, so do
    // nothing.
    // Also don't repeat the insert when Insert mode ended with CTRL-C.
    ಆದರೆ (curwin->w_cursor.lnum != oap->ಆರಂಭ.lnum || got_int)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (oap->ಖಂಡ_ಬಹುಲಕ)
    {
	ಪೂರ್ಣಾಂಕ			ins_len;
	char_u			*firstline, *ins_ಪಠ್ಯ;
	ರಚನೆ ಖಂಡ_def	bd2;
	ಪೂರ್ಣಾಂಕ			did_indent = FALSE;
	size_t			len;
	size_t			ಕೂಡಿಸು;
	// offset when cursor was moved in insert mode
	ಪೂರ್ಣಾಂಕ			ಸ್ಥಾನಾಂತರ = 0;

	// If indent kicked in, the firstline might have changed
	// but only do that, if the indent actually increased.
	ind_post_ಲಂಬಸಾಲು = (colnr_T)getwhitecols_curline();
	ಆದರೆ (curbuf->b_op_ಆರಂಭ.ಲಂಬಸಾಲು > ind_pre_ಲಂಬಸಾಲು && ind_post_ಲಂಬಸಾಲು > ind_pre_ಲಂಬಸಾಲು)
	{
	    bd.textcol += ind_post_ಲಂಬಸಾಲು - ind_pre_ಲಂಬಸಾಲು;
	    ind_post_vcol = get_indent();
	    bd.ಆರಂಭ_vcol += ind_post_vcol - ind_pre_vcol;
	    did_indent = TRUE;
	}

	// The user may have moved the cursor before inserting something, try
	// to adjust the block for that.  But only do it, if the difference
	// does not come from indent kicking in.
	ಆದರೆ (oap->ಆರಂಭ.lnum == curbuf->b_op_ಆರಂಭ_orig.lnum
						  && !bd.is_MAX && !did_indent)
	{
	    ಪೂರ್ಣಾಂಕ t = getviscol2(curbuf->b_op_ಆರಂಭ_orig.ಲಂಬಸಾಲು,
					       curbuf->b_op_ಆರಂಭ_orig.coladd);

	    ಆದರೆ (oap->op_type == OP_ಸೇರಿಸು
		    && oap->ಆರಂಭ.ಲಂಬಸಾಲು + oap->ಆರಂಭ.coladd
			    != curbuf->b_op_ಆರಂಭ_orig.ಲಂಬಸಾಲು
					      + curbuf->b_op_ಆರಂಭ_orig.coladd)
	    {
		oap->ಆರಂಭ.ಲಂಬಸಾಲು = curbuf->b_op_ಆರಂಭ_orig.ಲಂಬಸಾಲು;
		pre_textlen -= t - oap->ಆರಂಭ_vcol;
		oap->ಆರಂಭ_vcol = t;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (oap->op_type == OP_APPEND
		    && oap->ಆರಂಭ.ಲಂಬಸಾಲು + oap->ಆರಂಭ.coladd
			    >= curbuf->b_op_ಆರಂಭ_orig.ಲಂಬಸಾಲು
					      + curbuf->b_op_ಆರಂಭ_orig.coladd)
	    {
		oap->ಆರಂಭ.ಲಂಬಸಾಲು = curbuf->b_op_ಆರಂಭ_orig.ಲಂಬಸಾಲು;
		// reset pre_textlen to the value of OP_INSERT
		pre_textlen += bd.textlen;
		pre_textlen -= t - oap->ಆರಂಭ_vcol;
		oap->ಆರಂಭ_vcol = t;
		oap->op_type = OP_ಸೇರಿಸು;
	    }
	}

	// Spaces and tabs in the indent may have changed to other spaces and
	// tabs.  Get the starting column again and correct the length.
	// Don't do this when "$" used, end-of-line will have changed.
	//
	// if indent was added and the inserted text was after the indent,
	// correct the selection for the new indent.
	ಆದರೆ (did_indent && bd.textcol - ind_post_ಲಂಬಸಾಲು > 0)
	{
	    oap->ಆರಂಭ.ಲಂಬಸಾಲು += ind_post_ಲಂಬಸಾಲು - ind_pre_ಲಂಬಸಾಲು;
	    oap->ಆರಂಭ_vcol += ind_post_vcol - ind_pre_vcol;
	    oap->ಕೊನೆ.ಲಂಬಸಾಲು += ind_post_ಲಂಬಸಾಲು - ind_pre_ಲಂಬಸಾಲು;
	    oap->ಕೊನೆ_vcol += ind_post_vcol - ind_pre_vcol;
	}
	ಖಂಡ_prep(oap, &bd2, oap->ಆರಂಭ.lnum, TRUE);
	ಆದರೆ (did_indent && bd.textcol - ind_post_ಲಂಬಸಾಲು > 0)
	{
	    // undo for where "oap" is used below
	    oap->ಆರಂಭ.ಲಂಬಸಾಲು -= ind_post_ಲಂಬಸಾಲು - ind_pre_ಲಂಬಸಾಲು;
	    oap->ಆರಂಭ_vcol -= ind_post_vcol - ind_pre_vcol;
	    oap->ಕೊನೆ.ಲಂಬಸಾಲು -= ind_post_ಲಂಬಸಾಲು - ind_pre_ಲಂಬಸಾಲು;
	    oap->ಕೊನೆ_vcol -= ind_post_vcol - ind_pre_vcol;
	}
	ಆದರೆ (!bd.is_MAX || bd2.textlen < bd.textlen)
	{
	    ಆದರೆ (oap->op_type == OP_APPEND)
	    {
		pre_textlen += bd2.textlen - bd.textlen;
		ಆದರೆ (bd2.endspaces)
		    --bd2.textlen;
	    }
	    bd.textcol = bd2.textcol;
	    bd.textlen = bd2.textlen;
	}

	/*
	 * Subsequent calls to ml_get() flush the firstline data - take a
	 * copy of the required string.
	 */
	firstline = ml_get(oap->ಆರಂಭ.lnum);
	len = ml_get_len(oap->ಆರಂಭ.lnum);
	ಕೂಡಿಸು = bd.textcol;
	ಆದರೆ (oap->op_type == OP_APPEND)
	{
	    ಕೂಡಿಸು += bd.textlen;
	    // account for pressing cursor in insert mode when '$' was used
	    ಆದರೆ (bd.is_MAX
		&& (ಆರಂಭ_ಸೇರಿಸು.lnum == Insstart.lnum
					   && ಆರಂಭ_ಸೇರಿಸು.ಲಂಬಸಾಲು > Insstart.ಲಂಬಸಾಲು))
	    {
		ಸ್ಥಾನಾಂತರ = (ಆರಂಭ_ಸೇರಿಸು.ಲಂಬಸಾಲು - Insstart.ಲಂಬಸಾಲು);
		ಕೂಡಿಸು -= ಸ್ಥಾನಾಂತರ;
		ಆದರೆ (oap->ಕೊನೆ_vcol > ಸ್ಥಾನಾಂತರ)
		    oap->ಕೊನೆ_vcol -= (ಸ್ಥಾನಾಂತರ + 1);
		ಇಲ್ಲದಿದ್ದರೆ
		    // moved outside of the visual block, what to do?
		    ಹಿಂತಿರುಗಿ;
	    }
	}
	ಆದರೆ (ಕೂಡಿಸು > len)
	    ಕೂಡಿಸು = len;  // short line, point to the NUL
	firstline += ಕೂಡಿಸು;
	len -= ಕೂಡಿಸು;
	ಆದರೆ (pre_textlen >= 0 && (ins_len = (ಪೂರ್ಣಾಂಕ)len - pre_textlen - ಸ್ಥಾನಾಂತರ) > 0)
	{
	    ins_ಪಠ್ಯ = vim_strnsave(firstline, ins_len);
	    ಆದರೆ (ins_ಪಠ್ಯ != ಖಾಲಿ)
	    {
		// block handled here
		ಆದರೆ (u_save(oap->ಆರಂಭ.lnum,
					 (linenr_T)(oap->ಕೊನೆ.lnum + 1)) == OK)
		    ಖಂಡ_ಸೇರಿಸು(oap, ins_ಪಠ್ಯ, ins_len, (oap->op_type == OP_ಸೇರಿಸು),
									 &bd);

		curwin->w_cursor.ಲಂಬಸಾಲು = oap->ಆರಂಭ.ಲಂಬಸಾಲು;
		check_cursor();
		vim_free(ins_ಪಠ್ಯ);
	    }
	}
    }
}

/*
 * op_change - handle a change operation
 *
 * return TRUE if edit() returns because of a CTRL-O command
 */
    ಪೂರ್ಣಾಂಕ
op_change(oparg_T *oap)
{
    colnr_T		l;
    ಪೂರ್ಣಾಂಕ			retval;
    linenr_T		linenr;
    ದೀರ್ಘ		pre_textlen = 0;
    ದೀರ್ಘ		pre_indent = 0;
    char_u		*firstline;
    char_u		*ins_ಪಠ್ಯ, *newp, *oldp;
    ರಚನೆ ಖಂಡ_def	bd;

    l = oap->ಆರಂಭ.ಲಂಬಸಾಲು;
    ಆದರೆ (oap->motion_type == MLINE)
    {
	l = 0;
	can_si = may_do_si();	// Like opening a new line, do smart indent
    }

    // First delete the text in the region.  In an empty buffer only need to
    // save for undo
    ಆದರೆ (curbuf->b_ml.ml_flags & ML_ಖಾಲಿಗಳು)
    {
	ಆದರೆ (u_save_cursor() == FAIL)
	    ಹಿಂತಿರುಗಿ FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (op_delete(oap) == FAIL)
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ ((l > curwin->w_cursor.ಲಂಬಸಾಲು) && !LINEEMPTY(curwin->w_cursor.lnum)
							 && !virtual_op)
	inc_cursor();

    // check for still on same line (<CR> in inserted text meaningless)
    // skip blank lines too
    ಆದರೆ (oap->ಖಂಡ_ಬಹುಲಕ)
    {
	// Add spaces before getting the current line length.
	ಆದರೆ (virtual_op && (curwin->w_cursor.coladd > 0
						    || gchar_cursor() == NUL))
	    coladvance_force(getviscol());
	firstline = ml_get(oap->ಆರಂಭ.lnum);
	pre_textlen = ml_get_len(oap->ಆರಂಭ.lnum);
	pre_indent = (ದೀರ್ಘ)getwhitecols(firstline);
	bd.textcol = curwin->w_cursor.ಲಂಬಸಾಲು;
    }

    ಆದರೆ (oap->motion_type == MLINE)
	fix_indent();

    // Reset finish_op now, don't want it set inside edit().
    ಪೂರ್ಣಾಂಕ save_ಮುಕ್ತಾಯ_op = ಮುಕ್ತಾಯ_op;
    ಮುಕ್ತಾಯ_op = FALSE;

    retval = edit(NUL, FALSE, (linenr_T)1);

    ಮುಕ್ತಾಯ_op = save_ಮುಕ್ತಾಯ_op;

    /*
     * In Visual block mode, handle copying the new text to all lines of the
     * block.
     * Don't repeat the insert when Insert mode ended with CTRL-C.
     */
    ಆದರೆ (oap->ಖಂಡ_ಬಹುಲಕ && oap->ಆರಂಭ.lnum != oap->ಕೊನೆ.lnum && !got_int)
    {
	ಪೂರ್ಣಾಂಕ	ins_len;

	// Auto-indenting may have changed the indent.  If the cursor was past
	// the indent, exclude that indent change from the inserted text.
	firstline = ml_get(oap->ಆರಂಭ.lnum);
	ಆದರೆ (bd.textcol > (colnr_T)pre_indent)
	{
	    ದೀರ್ಘ new_indent = (ದೀರ್ಘ)getwhitecols(firstline);

	    pre_textlen += new_indent - pre_indent;
	    bd.textcol += new_indent - pre_indent;
	}

	ins_len = (ಪೂರ್ಣಾಂಕ)ml_get_len(oap->ಆರಂಭ.lnum) - pre_textlen;
	ಆದರೆ (ins_len > 0)
	{
	    // Subsequent calls to ml_get() flush the firstline data - take a
	    // copy of the inserted text.
	    ಆದರೆ ((ins_ಪಠ್ಯ = alloc(ins_len + 1)) != ಖಾಲಿ)
	    {
		vim_strncpy(ins_ಪಠ್ಯ, firstline + bd.textcol, ins_len);
		ಗಾಗಿ (linenr = oap->ಆರಂಭ.lnum + 1; linenr <= oap->ಕೊನೆ.lnum;
								     linenr++)
		{
		    ಖಂಡ_prep(oap, &bd, linenr, TRUE);
		    ಆದರೆ (!bd.is_short || virtual_op)
		    {
			pos_T vpos;
			size_t newlen;

			// If the block starts in virtual space, count the
			// initial coladd offset as part of "startspaces"
			ಆದರೆ (bd.is_short)
			{
			    vpos.lnum = linenr;
			    (ಶೂನ್ಯ)getvpos(&vpos, oap->ಆರಂಭ_vcol);
			}
			ಇಲ್ಲದಿದ್ದರೆ
			    vpos.coladd = 0;
			oldp = ml_get(linenr);
			newp = alloc(ml_get_len(linenr) + vpos.coladd + ins_len + 1);
			ಆದರೆ (newp == ಖಾಲಿ)
			    ಮುಂದುವರಿ;
			// copy up to block start
			mch_memmove(newp, oldp, (size_t)bd.textcol);
			newlen = bd.textcol;
			vim_memset(newp + newlen, ' ', (size_t)vpos.coladd);
			newlen += vpos.coladd;
			mch_memmove(newp + newlen, ins_ಪಠ್ಯ, ins_len);
			STRCPY(newp + newlen + ins_len, oldp + bd.textcol);
			ml_replace(linenr, newp, FALSE);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
			// Shift the properties for linenr as edit() would do.
			ಆದರೆ (curbuf->b_has_textprop)
			    adjust_prop_columns(linenr, bd.textcol,
						     vpos.coladd + (ಪೂರ್ಣಾಂಕ)ins_len, 0);
#ಆದರೆ_ಅಂತ್ಯ
		    }
		}
		check_cursor();

		ಬದಲಾಯಿತು_ಸಾಲುಗಳು(oap->ಆರಂಭ.lnum + 1, 0, oap->ಕೊನೆ.lnum + 1, 0L);
	    }
	    vim_free(ins_ಪಠ್ಯ);
	}
    }
    auto_format(FALSE, TRUE);

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * When the cursor is on the NUL past the end of the line and it should not be
 * there move it left.
 */
    ಶೂನ್ಯ
adjust_cursor_eol(ಶೂನ್ಯ)
{
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ cur_ve_flags = get_ve_flags();

    ಪೂರ್ಣಾಂಕ adj_cursor = (curwin->w_cursor.ಲಂಬಸಾಲು > 0
				&& gchar_cursor() == NUL
				&& (cur_ve_flags & VE_ONEMORE) == 0
				&& (cur_ve_flags & VE_ALL) == 0
				&& !(restart_edit || (State & ಬಹುಲಕ_ಸೇರಿಸು)));
    ಆದರೆ (!adj_cursor)
	ಹಿಂತಿರುಗಿ;

    // Put the cursor on the last character in the line.
    dec_cursor();

    ಆದರೆ (cur_ve_flags == VE_ALL)
    {
	colnr_T	    scol, ecol;

	// Coladd is set to the width of the last character.
	getvcol(curwin, &curwin->w_cursor, &scol, ಖಾಲಿ, &ecol, 0);
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
    char_u   *ಸಾಲು,
    ಪೂರ್ಣಾಂಕ      process,
    ಪೂರ್ಣಾಂಕ	     include_space,
    ಪೂರ್ಣಾಂಕ      *is_comment)
{
    char_u *comment_flags = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ    lead_len;
    ಪೂರ್ಣಾಂಕ    leader_ಸ್ಥಾನಾಂತರ = get_ಕೊನೆಯ_leader_ಸ್ಥಾನಾಂತರ(ಸಾಲು, &comment_flags);

    *is_comment = FALSE;
    ಆದರೆ (leader_ಸ್ಥಾನಾಂತರ != -1)
    {
	// Let's check whether the line ends with an unclosed comment.
	// If the last comment leader has COM_END in flags, there's no comment.
	ಆಗ (*comment_flags)
	{
	    ಆದರೆ (*comment_flags == COM_ಕೊನೆ
		    || *comment_flags == ':')
		ನಿಲ್ಲಿಸು;
	    ++comment_flags;
	}
	ಆದರೆ (*comment_flags != COM_ಕೊನೆ)
	    *is_comment = TRUE;
    }

    ಆದರೆ (process == FALSE)
	ಹಿಂತಿರುಗಿ ಸಾಲು;

    lead_len = get_leader_len(ಸಾಲು, &comment_flags, FALSE, include_space);

    ಆದರೆ (lead_len == 0)
	ಹಿಂತಿರುಗಿ ಸಾಲು;

    // Find:
    // - COM_END,
    // - colon,
    // whichever comes first.
    ಆಗ (*comment_flags)
    {
	ಆದರೆ (*comment_flags == COM_ಕೊನೆ
		|| *comment_flags == ':')
	    ನಿಲ್ಲಿಸು;
	++comment_flags;
    }

    // If we found a colon, it means that we are not processing a line
    // starting with a closing part of a three-part comment. That's good,
    // because we don't want to remove those as this would be annoying.
    ಆದರೆ (*comment_flags == ':' || *comment_flags == NUL)
	ಸಾಲು += lead_len;

    ಹಿಂತಿರುಗಿ ಸಾಲು;
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
    ಪೂರ್ಣಾಂಕ
do_join(
    ದೀರ್ಘ    ಎಣಿಕೆ_2,
    ಪೂರ್ಣಾಂಕ	    ಸೇರಿಸು_space,
    ಪೂರ್ಣಾಂಕ	    save_undo,
    ಪೂರ್ಣಾಂಕ	    use_formatoptions ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ	    setmark)
{
    char_u	*curr = ಖಾಲಿ;
    char_u      *curr_ಆರಂಭ = ಖಾಲಿ;
    char_u	*cend;
    char_u	*newp;
    size_t	newp_len;
    char_u	*spaces;	// number of spaces inserted before a line
    ಪೂರ್ಣಾಂಕ		endcurr1 = NUL;
    ಪೂರ್ಣಾಂಕ		endcurr2 = NUL;
    ಪೂರ್ಣಾಂಕ		currsize = 0;	// size of the current line
    ಪೂರ್ಣಾಂಕ		sumsize = 0;	// size of the long new line
    linenr_T	t;
    colnr_T	ಲಂಬಸಾಲು = 0;
    ಪೂರ್ಣಾಂಕ		ret = OK;
    ಪೂರ್ಣಾಂಕ		*comments = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		remove_comments = (use_formatoptions == TRUE)
				  && has_format_option(FO_REMOVE_COMS);
    ಪೂರ್ಣಾಂಕ		prev_was_comment;

    ಆದರೆ (save_undo && u_save((linenr_T)(curwin->w_cursor.lnum - 1),
			    (linenr_T)(curwin->w_cursor.lnum + ಎಣಿಕೆ_2)) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    // Allocate an array to store the number of spaces inserted before each
    // line.  We will use it to pre-compute the length of the new line and the
    // proper placement of each original line in the new one.
    spaces = lalloc_clear(ಎಣಿಕೆ_2, TRUE);
    ಆದರೆ (spaces == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (remove_comments)
    {
	comments = lalloc_clear(ಎಣಿಕೆ_2 * ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ), TRUE);
	ಆದರೆ (comments == ಖಾಲಿ)
	{
	    vim_free(spaces);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }

    /*
     * Don't move anything yet, just compute the final line length
     * and setup the array of space strings lengths
     * This loops forward over the joined lines.
     */
    ಗಾಗಿ (t = 0; t < ಎಣಿಕೆ_2; ++t)
    {
	curr = curr_ಆರಂಭ = ml_get((linenr_T)(curwin->w_cursor.lnum + t));
	ಆದರೆ (t == 0 && setmark && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	{
	    // Set the '[ mark.
	    curwin->w_ಬಫರ್->b_op_ಆರಂಭ.lnum = curwin->w_cursor.lnum;
	    curwin->w_ಬಫರ್->b_op_ಆರಂಭ.ಲಂಬಸಾಲು  = (colnr_T)STRLEN(curr);
	}
	ಆದರೆ (remove_comments)
	{
	    // We don't want to remove the comment leader if the
	    // previous line is not a comment.
	    ಆದರೆ (t > 0 && prev_was_comment)
	    {

		char_u *new_curr = skip_comment(curr, TRUE, ಸೇರಿಸು_space,
							   &prev_was_comment);
		comments[t] = (ಪೂರ್ಣಾಂಕ)(new_curr - curr);
		curr = new_curr;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		curr = skip_comment(curr, FALSE, ಸೇರಿಸು_space,
							   &prev_was_comment);
	}

	ಆದರೆ (ಸೇರಿಸು_space && t > 0)
	{
	    curr = skipwhite(curr);
	    ಆದರೆ (*curr != NUL && *curr != ')'
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
		ಆದರೆ (endcurr1 == ' ')
		    endcurr1 = endcurr2;
		ಇಲ್ಲದಿದ್ದರೆ
		    ++spaces[t];
		// extra space when 'joinspaces' set and line ends in '.'
		ಆದರೆ (       p_js
			&& (endcurr1 == '.'
			    || (vim_strchr(p_cpo, CPO_JOINSP) == ಖಾಲಿ
				&& (endcurr1 == '?' || endcurr1 == '!'))))
		    ++spaces[t];
	    }
	}
	currsize = (ಪೂರ್ಣಾಂಕ)STRLEN(curr);
	sumsize += currsize + spaces[t];
	endcurr1 = endcurr2 = NUL;
	ಆದರೆ (ಸೇರಿಸು_space && currsize > 0)
	{
	    ಆದರೆ (has_mbyte)
	    {
		cend = curr + currsize;
		MB_PTR_ಹಿಂಭಾಗ(curr, cend);
		endcurr1 = (*mb_ptr2char)(cend);
		ಆದರೆ (cend > curr)
		{
		    MB_PTR_ಹಿಂಭಾಗ(curr, cend);
		    endcurr2 = (*mb_ptr2char)(cend);
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		endcurr1 = *(curr + currsize - 1);
		ಆದರೆ (currsize > 1)
		    endcurr2 = *(curr + currsize - 2);
	    }
	}
	ಸಾಲು_breakcheck();
	ಆದರೆ (got_int)
	{
	    ret = FAIL;
	    ಹೋಗು theend;
	}
    }

    // store the column position before last line
    ಲಂಬಸಾಲು = sumsize - currsize - spaces[ಎಣಿಕೆ_2 - 1];

    // allocate the space for the new line
    newp_len = sumsize + 1;
    newp = alloc(newp_len);
    ಆದರೆ (newp == ಖಾಲಿ)
    {
	ret = FAIL;
	ಹೋಗು theend;
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    unpacked_memline_T um = um_open_at_no_props(
				curwin->w_ಬಫರ್, curwin->w_cursor.lnum, 0);
    // um_open_at_no_props may have invalidated "curr".
    ಪೂರ್ಣಾಂಕ curr_off = (ಪೂರ್ಣಾಂಕ)(curr - curr_ಆರಂಭ);

    curr = curr_ಆರಂಭ = ml_get((linenr_T)(curwin->w_cursor.lnum + ಎಣಿಕೆ_2 - 1));
    curr += curr_off;
#ಆದರೆ_ಅಂತ್ಯ

    ಗಾಗಿ (t = ಎಣಿಕೆ_2 - 1; ; --t)
    {
	ಪೂರ್ಣಾಂಕ spaces_removed;

	cend -= currsize;
	mch_memmove(cend, curr, (size_t)currsize);

	ಆದರೆ (spaces[t] > 0)
	{
	    cend -= spaces[t];
	    vim_memset(cend, ' ', (size_t)(spaces[t]));
	}

	// If deleting more spaces than adding, the cursor moves no more than
	// what is added if it is inside these spaces.
	spaces_removed = (curr - curr_ಆರಂಭ) - spaces[t];

	mark_ಲಂಬಸಾಲು_adjust(curwin->w_cursor.lnum + t, (colnr_T)0, -t,
			 (ದೀರ್ಘ)(cend - newp - spaces_removed), spaces_removed);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	prepend_joined_props(&um, curwin->w_cursor.lnum + t,
		t == ಎಣಿಕೆ_2 - 1, (ದೀರ್ಘ)(cend - newp), spaces_removed);
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (t == 0)
	    ನಿಲ್ಲಿಸು;
	curr = curr_ಆರಂಭ = ml_get((linenr_T)(curwin->w_cursor.lnum + t - 1));
	ಆದರೆ (remove_comments)
	    curr += comments[t - 1];
	ಆದರೆ (ಸೇರಿಸು_space && t > 1)
	    curr = skipwhite(curr);
	currsize = (ಪೂರ್ಣಾಂಕ)STRLEN(curr);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಆದರೆ (um.buf != ಖಾಲಿ)
    {
	um_ಗಣ_ಪಠ್ಯ(&um, newp);
	um_ಹಿಮ್ಮುಖ_props(&um);
	um_close(&um);
	newp = ಖಾಲಿ;  // um_set_text took ownership
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    ml_replace_len(curwin->w_cursor.lnum, newp, (colnr_T)newp_len, TRUE, FALSE);

    ಆದರೆ (setmark && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set the '] mark.
	curwin->w_ಬಫರ್->b_op_ಕೊನೆ.lnum = curwin->w_cursor.lnum;
	curwin->w_ಬಫರ್->b_op_ಕೊನೆ.ಲಂಬಸಾಲು  = (colnr_T)sumsize;
    }

    // Only report the change in the first line here, del_lines() will report
    // the deleted line.
    ಬದಲಾಯಿತು_ಸಾಲುಗಳು(curwin->w_cursor.lnum, currsize,
					       curwin->w_cursor.lnum + 1, 0L);
    /*
     * Delete following lines. To do this we move the cursor there
     * briefly, and then move it back. After del_lines() the cursor may
     * have moved up (last line deleted), so the current lnum is kept in t.
     */
    t = curwin->w_cursor.lnum;
    ++curwin->w_cursor.lnum;
    del_ಸಾಲುಗಳು(ಎಣಿಕೆ_2 - 1, FALSE);
    curwin->w_cursor.lnum = t;

    /*
     * Set the cursor column:
     * Vi compatible: use the column of the first join
     * vim:	      use the column of the last join
     */
    curwin->w_cursor.ಲಂಬಸಾಲು =
		    (vim_strchr(p_cpo, CPO_JOINCOL) != ಖಾಲಿ ? currsize : ಲಂಬಸಾಲು);
    check_cursor_ಲಂಬಸಾಲು();

    curwin->w_cursor.coladd = 0;
    curwin->w_ಗಣ_curswant = true;

theend:
    vim_free(spaces);
    ಆದರೆ (remove_comments)
	vim_free(comments);
    ಹಿಂತಿರುಗಿ ret;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
/*
 * TODO: consider using "tailp" of win_lbr_chartabsize() instead of temporarily
 * resetting 'linebreak'.
 *
 * Reset 'linebreak' and take care of side effects.
 * Returns the previous value, to be passed to restore_lbr().
 */
    ಪೂರ್ಣಾಂಕ
reset_lbr(ಶೂನ್ಯ)
{
    ಆದರೆ (!curwin->w_p_lbr)
	ಹಿಂತಿರುಗಿ FALSE;
    // changing 'linebreak' may require w_virtcol to be updated
    curwin->w_p_lbr = FALSE;
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Restore 'linebreak' and take care of side effects.
 */
    ಶೂನ್ಯ
restore_lbr(ಪೂರ್ಣಾಂಕ lbr_saved)
{
    ಆದರೆ (curwin->w_p_lbr || !lbr_saved)
	ಹಿಂತಿರುಗಿ;

    // changing 'linebreak' may require w_virtcol to be updated
    curwin->w_p_lbr = TRUE;
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);
}
#ಆದರೆ_ಅಂತ್ಯ

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
    ಶೂನ್ಯ
ಖಂಡ_prep(
    oparg_T		*oap,
    ರಚನೆ ಖಂಡ_def	*bdp,
    linenr_T		lnum,
    ಪೂರ್ಣಾಂಕ			is_del)
{
    ಪೂರ್ಣಾಂಕ		incr = 0;
    char_u	*pend;
    char_u	*pstart;
    char_u	*ಸಾಲು;
    char_u	*prev_pstart;
    char_u	*prev_pend;
    chartabsize_T cts;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
		// Avoid a problem with unwanted linebreaks in block mode.
    ಪೂರ್ಣಾಂಕ		lbr_saved = reset_lbr();
#ಆದರೆ_ಅಂತ್ಯ

    bdp->startspaces = 0;
    bdp->endspaces = 0;
    bdp->textlen = 0;
    bdp->ಆರಂಭ_vcol = 0;
    bdp->ಕೊನೆ_vcol = 0;
    bdp->is_short = FALSE;
    bdp->is_oneChar = FALSE;
    bdp->pre_whitesp = 0;
    bdp->pre_whitesp_c = 0;
    bdp->ಕೊನೆ_char_vcols = 0;
    bdp->ಆರಂಭ_char_vcols = 0;

    ಸಾಲು = ml_get(lnum);
    prev_pstart = ಸಾಲು;
    init_chartabsize_arg(&cts, curwin, lnum, bdp->ಆರಂಭ_vcol, ಸಾಲು, ಸಾಲು);
    ಆಗ (cts.cts_vcol < oap->ಆರಂಭ_vcol && *cts.cts_ptr != NUL)
    {
	// Count a tab for what it's worth (if list mode not on)
	incr = lbr_chartabsize(&cts);
	cts.cts_vcol += incr;
	ಆದರೆ (VIM_ISWHITE(*cts.cts_ptr))
	{
	    bdp->pre_whitesp += incr;
	    bdp->pre_whitesp_c++;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    bdp->pre_whitesp = 0;
	    bdp->pre_whitesp_c = 0;
	}
	prev_pstart = cts.cts_ptr;
	MB_PTR_ADV(cts.cts_ptr);
    }
    bdp->ಆರಂಭ_vcol = cts.cts_vcol;
    pstart = cts.cts_ptr;
    clear_chartabsize_arg(&cts);

    bdp->ಆರಂಭ_char_vcols = incr;
    ಆದರೆ (bdp->ಆರಂಭ_vcol < oap->ಆರಂಭ_vcol)	// line too short
    {
	bdp->ಕೊನೆ_vcol = bdp->ಆರಂಭ_vcol;
	bdp->is_short = TRUE;
	ಆದರೆ (!is_del || oap->op_type == OP_APPEND)
	    bdp->endspaces = oap->ಕೊನೆ_vcol - oap->ಆರಂಭ_vcol + 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// notice: this converts partly selected Multibyte characters to
	// spaces, too.
	bdp->startspaces = bdp->ಆರಂಭ_vcol - oap->ಆರಂಭ_vcol;
	ಆದರೆ (is_del && bdp->startspaces)
	    bdp->startspaces = bdp->ಆರಂಭ_char_vcols - bdp->startspaces;
	pend = pstart;
	bdp->ಕೊನೆ_vcol = bdp->ಆರಂಭ_vcol;
	ಆದರೆ (bdp->ಕೊನೆ_vcol > oap->ಕೊನೆ_vcol)	// it's all in one character
	{
	    bdp->is_oneChar = TRUE;
	    ಆದರೆ (oap->op_type == OP_ಸೇರಿಸು)
		bdp->endspaces = bdp->ಆರಂಭ_char_vcols - bdp->startspaces;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (oap->op_type == OP_APPEND)
	    {
		bdp->startspaces += oap->ಕೊನೆ_vcol - oap->ಆರಂಭ_vcol + 1;
		bdp->endspaces = bdp->ಆರಂಭ_char_vcols - bdp->startspaces;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		bdp->startspaces = oap->ಕೊನೆ_vcol - oap->ಆರಂಭ_vcol + 1;
		ಆದರೆ (is_del && oap->op_type != OP_LSHIFT)
		{
		    // just putting the sum of those two into
		    // bdp->startspaces doesn't work for Visual replace,
		    // so we have to split the tab in two
		    bdp->startspaces = bdp->ಆರಂಭ_char_vcols
					- (bdp->ಆರಂಭ_vcol - oap->ಆರಂಭ_vcol);
		    bdp->endspaces = bdp->ಕೊನೆ_vcol - oap->ಕೊನೆ_vcol - 1;
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    init_chartabsize_arg(&cts, curwin, lnum, bdp->ಕೊನೆ_vcol,
								  ಸಾಲು, pend);
	    prev_pend = pend;
	    ಆಗ (cts.cts_vcol <= oap->ಕೊನೆ_vcol && *cts.cts_ptr != NUL)
	    {
		// count a tab for what it's worth (if list mode not on)
		prev_pend = cts.cts_ptr;
		incr = lbr_chartabsize_adv(&cts);
		cts.cts_vcol += incr;
	    }
	    bdp->ಕೊನೆ_vcol = cts.cts_vcol;
	    pend = cts.cts_ptr;
	    clear_chartabsize_arg(&cts);

	    ಆದರೆ (bdp->ಕೊನೆ_vcol <= oap->ಕೊನೆ_vcol
		    && (!is_del
			|| oap->op_type == OP_APPEND
			|| oap->op_type == OP_REPLACE)) // line too short
	    {
		bdp->is_short = TRUE;
		// Alternative: include spaces to fill up the block.
		// Disadvantage: can lead to trailing spaces when the line is
		// short where the text is put
		// if (!is_del || oap->op_type == OP_APPEND)
		ಆದರೆ (oap->op_type == OP_APPEND || virtual_op)
		    bdp->endspaces = oap->ಕೊನೆ_vcol - bdp->ಕೊನೆ_vcol
							     + oap->inclusive;
		ಇಲ್ಲದಿದ್ದರೆ
		    bdp->endspaces = 0; // replace doesn't add characters
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (bdp->ಕೊನೆ_vcol > oap->ಕೊನೆ_vcol)
	    {
		bdp->endspaces = bdp->ಕೊನೆ_vcol - oap->ಕೊನೆ_vcol - 1;
		ಆದರೆ (!is_del && bdp->endspaces)
		{
		    bdp->endspaces = incr - bdp->endspaces;
		    ಆದರೆ (pend != pstart)
			pend = prev_pend;
		}
	    }
	}
	bdp->ಕೊನೆ_char_vcols = incr;
	ಆದರೆ (is_del && bdp->startspaces)
	    pstart = prev_pstart;
	bdp->textlen = (ಪೂರ್ಣಾಂಕ)(pend - pstart);
    }
    bdp->textcol = (colnr_T) (pstart - ಸಾಲು);
    bdp->textstart = pstart;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    restore_lbr(lbr_saved);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Get block text from "start" to "end"
 */
    ಶೂನ್ಯ
charwise_ಖಂಡ_prep(
    pos_T		ಆರಂಭ,
    pos_T		ಕೊನೆ,
    ರಚನೆ ಖಂಡ_def	*bdp,
    linenr_T		lnum,
    ಪೂರ್ಣಾಂಕ			inclusive)
{
    colnr_T startcol = 0, endcol = MAXCOL;
    colnr_T cs, ce;
    char_u *p;
    ಪೂರ್ಣಾಂಕ	plen = ml_get_len(lnum);

    p = ml_get(lnum);
    bdp->startspaces = 0;
    bdp->endspaces = 0;
    bdp->is_oneChar = FALSE;
    bdp->ಆರಂಭ_char_vcols = 0;

    ಆದರೆ (lnum == ಆರಂಭ.lnum)
    {
	startcol = ಆರಂಭ.ಲಂಬಸಾಲು;
	ಆದರೆ (virtual_op)
	{
	    getvcol(curwin, &ಆರಂಭ, &cs, ಖಾಲಿ, &ce, 0);
	    ಆದರೆ (ce != cs && ಆರಂಭ.coladd > 0)
	    {
		// Part of a tab selected -- but don't
		// double-count it.
		bdp->ಆರಂಭ_char_vcols = ce - cs + 1;
		bdp->startspaces = bdp->ಆರಂಭ_char_vcols - ಆರಂಭ.coladd;
		ಆದರೆ (bdp->startspaces < 0)
		    bdp->startspaces = 0;
		startcol++;
	    }
	}
    }

    ಆದರೆ (lnum == ಕೊನೆ.lnum)
    {
	endcol = ಕೊನೆ.ಲಂಬಸಾಲು;
	ಆದರೆ (virtual_op)
	{
	    getvcol(curwin, &ಕೊನೆ, &cs, ಖಾಲಿ, &ce, 0);
	    ಆದರೆ (p[endcol] == NUL || (cs + ಕೊನೆ.coladd < ce
			// Don't add space for double-wide
			// char; endcol will be on last byte
			// of multi-byte char.
			&& (*mb_ತಲೆ_off)(p, p + endcol) == 0))
	    {
		ಆದರೆ (ಆರಂಭ.lnum == ಕೊನೆ.lnum && ಆರಂಭ.ಲಂಬಸಾಲು == ಕೊನೆ.ಲಂಬಸಾಲು)
		{
		    // Special case: inside a single char
		    bdp->is_oneChar = TRUE;
		    bdp->startspaces = ಕೊನೆ.coladd - ಆರಂಭ.coladd + inclusive;
		    endcol = startcol;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    bdp->endspaces = ಕೊನೆ.coladd + inclusive;
		    endcol -= inclusive;
		}
	    }
	}
    }
    ಆದರೆ (endcol == MAXCOL)
	endcol = ml_get_len(lnum);
    ಆದರೆ (startcol > endcol || bdp->is_oneChar)
	bdp->textlen = 0;
    ಇಲ್ಲದಿದ್ದರೆ
	bdp->textlen = endcol - startcol + inclusive;
    bdp->textcol = startcol;
    bdp->textstart = startcol <= plen ? p + startcol : p;
}

/*
 * Handle the add/subtract operator.
 */
    ಶೂನ್ಯ
op_addsub(
    oparg_T	*oap,
    linenr_T	Prenum1,	    // Amount of add/subtract
    ಪೂರ್ಣಾಂಕ		g_cmd)		    // was g<c-a>/g<c-x>
{
    pos_T		pos;
    ರಚನೆ ಖಂಡ_def	bd;
    ಪೂರ್ಣಾಂಕ			change_cnt = 0;
    linenr_T		amount = Prenum1;

    // do_addsub() might trigger re-evaluation of 'foldexpr' halfway, when the
    // buffer is not completely updated yet. Postpone updating folds until before
    // the call to changed_lines().
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    disable_fold_update++;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (!VIsual_ಸಕ್ರಿಯ)
    {
	pos = curwin->w_cursor;
	ಆದರೆ (u_save_cursor() == FAIL)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    disable_fold_update--;
#ಆದರೆ_ಅಂತ್ಯ
	    ಹಿಂತಿರುಗಿ;
	}
	change_cnt = do_addsub(oap->op_type, &pos, 0, amount);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	disable_fold_update--;
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (change_cnt)
	    ಬದಲಾಯಿತು_ಸಾಲುಗಳು(pos.lnum, 0, pos.lnum + 1, 0L);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ	one_change;
	ಪೂರ್ಣಾಂಕ	ಉದ್ದಳತೆ;
	pos_T	startpos;

	ಆದರೆ (u_save((linenr_T)(oap->ಆರಂಭ.lnum - 1),
					(linenr_T)(oap->ಕೊನೆ.lnum + 1)) == FAIL)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    disable_fold_update--;
#ಆದರೆ_ಅಂತ್ಯ
	    ಹಿಂತಿರುಗಿ;
	}

	pos = oap->ಆರಂಭ;
	ಗಾಗಿ (; pos.lnum <= oap->ಕೊನೆ.lnum; ++pos.lnum)
	{
	    ಆದರೆ (oap->ಖಂಡ_ಬಹುಲಕ)		    // Visual block mode
	    {
		ಖಂಡ_prep(oap, &bd, pos.lnum, FALSE);
		pos.ಲಂಬಸಾಲು = bd.textcol;
		ಉದ್ದಳತೆ = bd.textlen;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (oap->motion_type == MLINE)
	    {
		curwin->w_cursor.ಲಂಬಸಾಲು = 0;
		pos.ಲಂಬಸಾಲು = 0;
		ಉದ್ದಳತೆ = ml_get_len(pos.lnum);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ // oap->motion_type == MCHAR
	    {
		ಆದರೆ (pos.lnum == oap->ಆರಂಭ.lnum && !oap->inclusive)
		    dec(&(oap->ಕೊನೆ));
		ಉದ್ದಳತೆ = ml_get_len(pos.lnum);
		pos.ಲಂಬಸಾಲು = 0;
		ಆದರೆ (pos.lnum == oap->ಆರಂಭ.lnum)
		{
		    pos.ಲಂಬಸಾಲು += oap->ಆರಂಭ.ಲಂಬಸಾಲು;
		    ಉದ್ದಳತೆ -= oap->ಆರಂಭ.ಲಂಬಸಾಲು;
		}
		ಆದರೆ (pos.lnum == oap->ಕೊನೆ.lnum)
		{
		    ಉದ್ದಳತೆ = ml_get_len(oap->ಕೊನೆ.lnum);
		    ಆದರೆ (oap->ಕೊನೆ.ಲಂಬಸಾಲು >= ಉದ್ದಳತೆ)
			oap->ಕೊನೆ.ಲಂಬಸಾಲು = ಉದ್ದಳತೆ - 1;
		    ಉದ್ದಳತೆ = oap->ಕೊನೆ.ಲಂಬಸಾಲು - pos.ಲಂಬಸಾಲು + 1;
		}
	    }
	    one_change = do_addsub(oap->op_type, &pos, ಉದ್ದಳತೆ, amount);
	    ಆದರೆ (one_change)
	    {
		// Remember the start position of the first change.
		ಆದರೆ (change_cnt == 0)
		    startpos = curbuf->b_op_ಆರಂಭ;
		++change_cnt;
	    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
	    ಆದರೆ (netbeans_ಸಕ್ರಿಯ() && one_change)
	    {
		char_u *ptr;

		netbeans_removed(curbuf, pos.lnum, pos.ಲಂಬಸಾಲು, (ದೀರ್ಘ)ಉದ್ದಳತೆ);
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, pos.ಲಂಬಸಾಲು,
						&ptr[pos.ಲಂಬಸಾಲು], ಉದ್ದಳತೆ);
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (g_cmd && one_change)
		amount += Prenum1;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	disable_fold_update--;
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (change_cnt)
	    ಬದಲಾಯಿತು_ಸಾಲುಗಳು(oap->ಆರಂಭ.lnum, 0, oap->ಕೊನೆ.lnum + 1, 0L);

	ಆದರೆ (!change_cnt && oap->is_VIsual)
	    // No change: need to remove the Visual selection
	    redraw_curbuf_later(UPD_INVERTED);

	// Set '[ mark if something changed. Keep the last end
	// position from do_addsub().
	ಆದರೆ (change_cnt > 0 && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	    curbuf->b_op_ಆರಂಭ = startpos;

	ಆದರೆ (change_cnt > p_report)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
do_addsub(
    ಪೂರ್ಣಾಂಕ		op_type,
    pos_T	*pos,
    ಪೂರ್ಣಾಂಕ		ಉದ್ದಳತೆ,
    linenr_T	Prenum1)
{
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		pre;		// 'X'/'x': hex; '0': octal; 'B'/'b': bin
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	hexupper = FALSE;	// 0xABC
    uvarnumber_T	n;
    uvarnumber_T	oldn;
    char_u	*ptr;
    ಪೂರ್ಣಾಂಕ		linelen;
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		todel;
    ಪೂರ್ಣಾಂಕ		do_hex;
    ಪೂರ್ಣಾಂಕ		do_oct;
    ಪೂರ್ಣಾಂಕ		do_bin;
    ಪೂರ್ಣಾಂಕ		do_alpha;
    ಪೂರ್ಣಾಂಕ		do_unsigned;
    ಪೂರ್ಣಾಂಕ		do_blank;
    ಪೂರ್ಣಾಂಕ		blank_unsigned = FALSE;	// blank: treat as unsigned?
    ಪೂರ್ಣಾಂಕ		firstdigit;
    ಪೂರ್ಣಾಂಕ		ಕಳೆ;
    ಪೂರ್ಣಾಂಕ		negative = FALSE;
    ಪೂರ್ಣಾಂಕ		was_ಧನಾತ್ಮಕ = TRUE;
    ಪೂರ್ಣಾಂಕ		visual = VIsual_ಸಕ್ರಿಯ;
    ಪೂರ್ಣಾಂಕ		did_change = FALSE;
    pos_T	save_cursor = curwin->w_cursor;
    ಪೂರ್ಣಾಂಕ		maxlen = 0;
    pos_T	startpos;
    pos_T	endpos;
    colnr_T	save_coladd = 0;

    do_hex = (vim_strchr(curbuf->b_p_nf, 'x') != ಖಾಲಿ);	// "heX"
    do_oct = (vim_strchr(curbuf->b_p_nf, 'o') != ಖಾಲಿ);	// "Octal"
    do_bin = (vim_strchr(curbuf->b_p_nf, 'b') != ಖಾಲಿ);	// "Bin"
    do_alpha = (vim_strchr(curbuf->b_p_nf, 'p') != ಖಾಲಿ);	// "alPha"
    do_unsigned = (vim_strchr(curbuf->b_p_nf, 'u') != ಖಾಲಿ);	// "Unsigned"
    do_blank = (vim_strchr(curbuf->b_p_nf, 'k') != ಖಾಲಿ);	// "blanK"

    ಆದರೆ (virtual_ಸಕ್ರಿಯ())
    {
	save_coladd = pos->coladd;
	pos->coladd = 0;
    }

    curwin->w_cursor = *pos;
    ptr = ml_get(pos->lnum);
    linelen = ml_get_len(pos->lnum);
    ಲಂಬಸಾಲು = pos->ಲಂಬಸಾಲು;

    ಆದರೆ (ಲಂಬಸಾಲು + !!save_coladd >= linelen)
	ಹೋಗು theend;

    /*
     * First check if we are on a hexadecimal number, after the "0x".
     */
    ಆದರೆ (!VIsual_ಸಕ್ರಿಯ)
    {
	ಆದರೆ (do_bin)
	    ಆಗ (ಲಂಬಸಾಲು > 0 && vim_isbdigit(ptr[ಲಂಬಸಾಲು]))
	    {
		--ಲಂಬಸಾಲು;
		ಆದರೆ (has_mbyte)
		    ಲಂಬಸಾಲು -= (*mb_ತಲೆ_off)(ptr, ptr + ಲಂಬಸಾಲು);
	    }

	ಆದರೆ (do_hex)
	    ಆಗ (ಲಂಬಸಾಲು > 0 && vim_isxdigit(ptr[ಲಂಬಸಾಲು]))
	    {
		--ಲಂಬಸಾಲು;
		ಆದರೆ (has_mbyte)
		    ಲಂಬಸಾಲು -= (*mb_ತಲೆ_off)(ptr, ptr + ಲಂಬಸಾಲು);
	    }

	ಆದರೆ (       do_bin
		&& do_hex
		&& ! ((ಲಂಬಸಾಲು > 0
		    && (ptr[ಲಂಬಸಾಲು] == 'X'
			|| ptr[ಲಂಬಸಾಲು] == 'x')
		    && ptr[ಲಂಬಸಾಲು - 1] == '0'
		    && (!has_mbyte ||
			!(*mb_ತಲೆ_off)(ptr, ptr + ಲಂಬಸಾಲು - 1))
		    && vim_isxdigit(ptr[ಲಂಬಸಾಲು + 1]))))
	{

	    // In case of binary/hexadecimal pattern overlap match, rescan

	    ಲಂಬಸಾಲು = pos->ಲಂಬಸಾಲು;

	    ಆಗ (ಲಂಬಸಾಲು > 0 && vim_isdigit(ptr[ಲಂಬಸಾಲು]))
	    {
		ಲಂಬಸಾಲು--;
		ಆದರೆ (has_mbyte)
		    ಲಂಬಸಾಲು -= (*mb_ತಲೆ_off)(ptr, ptr + ಲಂಬಸಾಲು);
	    }
	}

	ಆದರೆ ((       do_hex
		&& ಲಂಬಸಾಲು > 0
		&& (ptr[ಲಂಬಸಾಲು] == 'X'
		    || ptr[ಲಂಬಸಾಲು] == 'x')
		&& ptr[ಲಂಬಸಾಲು - 1] == '0'
		&& (!has_mbyte ||
		    !(*mb_ತಲೆ_off)(ptr, ptr + ಲಂಬಸಾಲು - 1))
		&& vim_isxdigit(ptr[ಲಂಬಸಾಲು + 1])) ||
	    (       do_bin
		&& ಲಂಬಸಾಲು > 0
		&& (ptr[ಲಂಬಸಾಲು] == 'B'
		    || ptr[ಲಂಬಸಾಲು] == 'b')
		&& ptr[ಲಂಬಸಾಲು - 1] == '0'
		&& (!has_mbyte ||
		    !(*mb_ತಲೆ_off)(ptr, ptr + ಲಂಬಸಾಲು - 1))
		&& vim_isbdigit(ptr[ಲಂಬಸಾಲು + 1])))
	{
	    // Found hexadecimal or binary number, move to its start.
	    --ಲಂಬಸಾಲು;
	    ಆದರೆ (has_mbyte)
		ಲಂಬಸಾಲು -= (*mb_ತಲೆ_off)(ptr, ptr + ಲಂಬಸಾಲು);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    /*
	     * Search forward and then backward to find the start of number.
	     */
	    ಲಂಬಸಾಲು = pos->ಲಂಬಸಾಲು;

	    ಆಗ (ptr[ಲಂಬಸಾಲು] != NUL
		    && !vim_isdigit(ptr[ಲಂಬಸಾಲು])
		    && !(do_alpha && ASCII_ISALPHA(ptr[ಲಂಬಸಾಲು])))
		ಲಂಬಸಾಲು += mb_ptr2len(ptr + ಲಂಬಸಾಲು);

	    ಆಗ (ಲಂಬಸಾಲು > 0
		    && vim_isdigit(ptr[ಲಂಬಸಾಲು - 1])
		    && !(do_alpha && ASCII_ISALPHA(ptr[ಲಂಬಸಾಲು])))
	    {
		--ಲಂಬಸಾಲು;
		ಆದರೆ (has_mbyte)
		    ಲಂಬಸಾಲು -= (*mb_ತಲೆ_off)(ptr, ptr + ಲಂಬಸಾಲು);
	    }
	}
    }

    ಆದರೆ (visual)
    {
	ಆಗ (ptr[ಲಂಬಸಾಲು] != NUL && ಉದ್ದಳತೆ > 0
		&& !vim_isdigit(ptr[ಲಂಬಸಾಲು])
		&& !(do_alpha && ASCII_ISALPHA(ptr[ಲಂಬಸಾಲು])))
	{
	    ಪೂರ್ಣಾಂಕ mb_len = mb_ptr2len(ptr + ಲಂಬಸಾಲು);

	    ಲಂಬಸಾಲು += mb_len;
	    ಉದ್ದಳತೆ -= mb_len;
	}

	ಆದರೆ (ಉದ್ದಳತೆ == 0)
	    ಹೋಗು theend;

	ಆದರೆ (ಲಂಬಸಾಲು > pos->ಲಂಬಸಾಲು && ptr[ಲಂಬಸಾಲು - 1] == '-'
		&& (!has_mbyte || !(*mb_ತಲೆ_off)(ptr, ptr + ಲಂಬಸಾಲು - 1))
		&& !do_unsigned)
	{
	    ಆದರೆ (do_blank && ಲಂಬಸಾಲು >= 2 && !VIM_ISWHITE(ptr[ಲಂಬಸಾಲು - 2]))
		blank_unsigned = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		negative = TRUE;
		was_ಧನಾತ್ಮಕ = FALSE;
	    }
	}
    }

    /*
     * If a number was found, and saving for undo works, replace the number.
     */
    firstdigit = ptr[ಲಂಬಸಾಲು];
    ಆದರೆ (!VIM_ISDIGIT(firstdigit) && !(do_alpha && ASCII_ISALPHA(firstdigit)))
    {
	beep_flush();
	ಹೋಗು theend;
    }

    ಆದರೆ (do_alpha && ASCII_ISALPHA(firstdigit))
    {
	// decrement or increment alphabetic character
	ಆದರೆ (op_type == OP_NR_SUB)
	{
	    ಆದರೆ (CharOrd(firstdigit) < Prenum1)
	    {
		ಆದರೆ (ಸುರಕ್ಷಿತ_isupper(firstdigit))
		    firstdigit = 'A';
		ಇಲ್ಲದಿದ್ದರೆ
		    firstdigit = 'a';
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		firstdigit -= Prenum1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (26 - CharOrd(firstdigit) - 1 < Prenum1)
	    {
		ಆದರೆ (ಸುರಕ್ಷಿತ_isupper(firstdigit))
		    firstdigit = 'Z';
		ಇಲ್ಲದಿದ್ದರೆ
		    firstdigit = 'z';
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		firstdigit += Prenum1;
	}
	curwin->w_cursor.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
	ಆದರೆ (!did_change)
	    startpos = curwin->w_cursor;
	did_change = TRUE;
	(ಶೂನ್ಯ)del_char(FALSE);
	ins_char(firstdigit);
	endpos = curwin->w_cursor;
	curwin->w_cursor.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	char_u	*buf1;
	ಪೂರ್ಣಾಂಕ	buf1len;
	char_u	buf2[NUMBUFLEN];
	ಪೂರ್ಣಾಂಕ	buf2len;
	pos_T	save_pos;
	ಪೂರ್ಣಾಂಕ	i;

	ಆದರೆ (ಲಂಬಸಾಲು > 0 && ptr[ಲಂಬಸಾಲು - 1] == '-'
		&& (!has_mbyte ||
		    !(*mb_ತಲೆ_off)(ptr, ptr + ಲಂಬಸಾಲು - 1))
		&& !visual
		&& !do_unsigned)
	{
	    ಆದರೆ (do_blank && ಲಂಬಸಾಲು >= 2 && !VIM_ISWHITE(ptr[ಲಂಬಸಾಲು - 2]))
		blank_unsigned = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// negative number
		--ಲಂಬಸಾಲು;
		negative = TRUE;
	    }
	}

	// get the number value (unsigned)
	ಆದರೆ (visual && VIsual_ಬಹುಲಕ != 'V')
	    maxlen = (curbuf->b_visual.vi_curswant == MAXCOL
		    ? linelen - ಲಂಬಸಾಲು : ಉದ್ದಳತೆ);

	ಪೂರ್ಣಾಂಕ ಅತಿಪ್ರವಾಹ = FALSE;
	vim_str2nr(ptr + ಲಂಬಸಾಲು, &pre, &ಉದ್ದಳತೆ,
		0 + (do_bin ? STR2NR_BIN : 0)
		    + (do_oct ? STR2NR_OCT : 0)
		    + (do_hex ? STR2NR_HEX : 0),
		ಖಾಲಿ, &n, maxlen, FALSE, &ಅತಿಪ್ರವಾಹ);

	// ignore leading '-' for hex and octal and bin numbers
	ಆದರೆ (pre && negative)
	{
	    ++ಲಂಬಸಾಲು;
	    --ಉದ್ದಳತೆ;
	    negative = FALSE;
	}
	// add or subtract
	ಕಳೆ = FALSE;
	ಆದರೆ (op_type == OP_NR_SUB)
	    ಕಳೆ ^= TRUE;
	ಆದರೆ (negative)
	    ಕಳೆ ^= TRUE;

	oldn = n;
	ಆದರೆ (!ಅತಿಪ್ರವಾಹ)  // if number is too big don't add/subtract
	{
	    ಆದರೆ (ಕಳೆ)
		n -= (uvarnumber_T)Prenum1;
	    ಇಲ್ಲದಿದ್ದರೆ
		n += (uvarnumber_T)Prenum1;
	}

	// handle wraparound for decimal numbers
	ಆದರೆ (!pre)
	{
	    ಆದರೆ (ಕಳೆ)
	    {
		ಆದರೆ (n > oldn)
		{
		    n = 1 + (n ^ (uvarnumber_T)-1);
		    negative ^= TRUE;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		//ಕೂಡಿಸು
		ಆದರೆ (n < oldn)
		{
		    n = (n ^ (uvarnumber_T)-1);
		    negative ^= TRUE;
		}
	    }
	    ಆದರೆ (n == 0)
		negative = FALSE;
	}

	ಆದರೆ ((do_unsigned || blank_unsigned) && negative)
	{
	    ಆದರೆ (ಕಳೆ)
		// sticking at zero.
		n = (uvarnumber_T)0;
	    ಇಲ್ಲದಿದ್ದರೆ
		// sticking at 2^64 - 1.
		n = (uvarnumber_T)(-1);
	    negative = FALSE;
	}

	ಆದರೆ (visual && !was_ಧನಾತ್ಮಕ && !negative && ಲಂಬಸಾಲು > 0)
	{
	    // need to remove the '-'
	    ಲಂಬಸಾಲು--;
	    ಉದ್ದಳತೆ++;
	}

	/*
	 * Delete the old number.
	 */
	curwin->w_cursor.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
	ಆದರೆ (!did_change)
	    startpos = curwin->w_cursor;
	did_change = TRUE;
	todel = ಉದ್ದಳತೆ;
	c = gchar_cursor();
	/*
	 * Don't include the '-' in the length, only the length of the
	 * part after it is kept the same.
	 */
	ಆದರೆ (c == '-')
	    --ಉದ್ದಳತೆ;

	save_pos = curwin->w_cursor;
	ಗಾಗಿ (i = 0; i < todel; ++i)
	{
	    ಆದರೆ (c < 0x100 && ಸುರಕ್ಷಿತ_isalpha(c))
	    {
		ಆದರೆ (ಸುರಕ್ಷಿತ_isupper(c))
		    hexupper = TRUE;
		ಇಲ್ಲದಿದ್ದರೆ
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
	buf1 = alloc(ಉದ್ದಳತೆ + NUMBUFLEN);
	ಆದರೆ (buf1 == ಖಾಲಿ)
	    ಹೋಗು theend;
	ptr = buf1;
	ಆದರೆ (negative && (!visual || was_ಧನಾತ್ಮಕ))
	    *ptr++ = '-';
	ಆದರೆ (pre)
	{
	    *ptr++ = '0';
	    --ಉದ್ದಳತೆ;
	}
	ಆದರೆ (pre == 'b' || pre == 'B' ||
	    pre == 'x' || pre == 'X')
	{
	    *ptr++ = pre;
	    --ಉದ್ದಳತೆ;
	}

	/*
	 * Put the number characters in buf2[].
	 */
	ಆದರೆ (pre == 'b' || pre == 'B')
	{
	    ಪೂರ್ಣಾಂಕ ಬಿಟ್ = 0;
	    ಪೂರ್ಣಾಂಕ ಬಿಟ್ಟುಗಳು = ಗಾತ್ರ(uvarnumber_T) * 8;

	    // leading zeros
	    ಗಾಗಿ (ಬಿಟ್ = ಬಿಟ್ಟುಗಳು; ಬಿಟ್ > 0; ಬಿಟ್--)
		ಆದರೆ ((n >> (ಬಿಟ್ - 1)) & 0x1) ನಿಲ್ಲಿಸು;

	    ಗಾಗಿ (buf2len = 0; ಬಿಟ್ > 0 && buf2len < (NUMBUFLEN - 1); ಬಿಟ್--)
		buf2[buf2len++] = ((n >> (ಬಿಟ್ - 1)) & 0x1) ? '1' : '0';

	    buf2[buf2len] = NUL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pre == 0)
	    buf2len = vim_snprintf((ಅಕ್ಷರ *)buf2, NUMBUFLEN, "%llu", n);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pre == '0')
	    buf2len = vim_snprintf((ಅಕ್ಷರ *)buf2, NUMBUFLEN, "%llo", n);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pre && hexupper)
	    buf2len = vim_snprintf((ಅಕ್ಷರ *)buf2, NUMBUFLEN, "%llX", n);
	ಇಲ್ಲದಿದ್ದರೆ
	    buf2len = vim_snprintf((ಅಕ್ಷರ *)buf2, NUMBUFLEN, "%llx", n);
	ಉದ್ದಳತೆ -= buf2len;

	/*
	 * Adjust number of zeros to the new number of digits, so the
	 * total length of the number remains the same.
	 * Don't do this when
	 * the result may look like an octal number.
	 */
	ಆದರೆ (firstdigit == '0' && !(do_oct && pre == 0))
	    ಆಗ (ಉದ್ದಳತೆ-- > 0)
		*ptr++ = '0';
	*ptr = NUL;
	buf1len = (ಪೂರ್ಣಾಂಕ)(ptr - buf1);

	STRCPY(buf1 + buf1len, buf2);
	buf1len += buf2len;

	// Insert just after the first character to be removed, so that any
	// text properties will be adjusted.  Then delete the old number
	// afterwards.
	save_pos = curwin->w_cursor;
	ಆದರೆ (todel > 0)
	    inc_cursor();
	ins_str(buf1, (size_t)buf1len);		// insert the new number
	vim_free(buf1);

	// del_char() will also mark line needing displaying
	ಆದರೆ (todel > 0)
	{
	    ಪೂರ್ಣಾಂಕ ಬೈಟುಗಳು_after = ml_get_curline_len() - curwin->w_cursor.ಲಂಬಸಾಲು;

	    // Delete the one character before the insert.
	    curwin->w_cursor = save_pos;
	    (ಶೂನ್ಯ)del_char(FALSE);
	    curwin->w_cursor.ಲಂಬಸಾಲು = ml_get_curline_len() - ಬೈಟುಗಳು_after;
	    --todel;
	}
	ಆಗ (todel-- > 0)
	    (ಶೂನ್ಯ)del_char(FALSE);

	endpos = curwin->w_cursor;
	ಆದರೆ (did_change && curwin->w_cursor.ಲಂಬಸಾಲು)
	    --curwin->w_cursor.ಲಂಬಸಾಲು;
    }

    ಆದರೆ (did_change && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// set the '[ and '] marks
	curbuf->b_op_ಆರಂಭ = startpos;
	curbuf->b_op_ಕೊನೆ = endpos;
	ಆದರೆ (curbuf->b_op_ಕೊನೆ.ಲಂಬಸಾಲು > 0)
	    --curbuf->b_op_ಕೊನೆ.ಲಂಬಸಾಲು;
    }

theend:
    ಆದರೆ (visual)
	curwin->w_cursor = save_cursor;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (did_change)
	curwin->w_ಗಣ_curswant = true;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (virtual_ಸಕ್ರಿಯ())
	curwin->w_cursor.coladd = save_coladd;

    ಹಿಂತಿರುಗಿ did_change;
}

    ಶೂನ್ಯ
clear_oparg(oparg_T *oap)
{
    CLEAR_ಸೂಚಕ(oap);
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
    ಸ್ಥಿರ varnumber_T
ಸಾಲು_ಎಣಿಕೆ_info(
    char_u	*ಸಾಲು,
    varnumber_T	*wc,
    varnumber_T	*cc,
    varnumber_T	ಎಲ್ಲೆ,
    ಪೂರ್ಣಾಂಕ		eol_ಗಾತ್ರ)
{
    varnumber_T	i;
    varnumber_T	ಪದಗಳು = 0;
    varnumber_T	chars = 0;
    ಪೂರ್ಣಾಂಕ		is_ಶಬ್ದ = 0;

    ಗಾಗಿ (i = 0; i < ಎಲ್ಲೆ && ಸಾಲು[i] != NUL; )
    {
	ಆದರೆ (is_ಶಬ್ದ)
	{
	    ಆದರೆ (vim_isspace(ಸಾಲು[i]))
	    {
		ಪದಗಳು++;
		is_ಶಬ್ದ = 0;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!vim_isspace(ಸಾಲು[i]))
	    is_ಶಬ್ದ = 1;
	++chars;
	i += (*mb_ptr2len)(ಸಾಲು + i);
    }

    ಆದರೆ (is_ಶಬ್ದ)
	ಪದಗಳು++;
    *wc += ಪದಗಳು;

    // Add eol_size if the end of line was reached before hitting limit.
    ಆದರೆ (i < ಎಲ್ಲೆ && ಸಾಲು[i] == NUL)
    {
	i += eol_ಗಾತ್ರ;
	chars += eol_ಗಾತ್ರ;
    }
    *cc += chars;
    ಹಿಂತಿರುಗಿ i;
}

/*
 * Give some info about the position of the cursor (for "g CTRL-G").
 * In Visual mode, give some info about the selected region.  (In this case,
 * the *_count_cursor variables store running totals for the selection.)
 * When "dict" is not NULL store the info there instead of showing it.
 */
    ಶೂನ್ಯ
cursor_pos_info(dict_T *dict)
{
    char_u	*p;
    char_u	buf1[50];
    char_u	buf2[40];
    linenr_T	lnum;
    varnumber_T	byte_ಎಣಿಕೆ = 0;
    varnumber_T	bom_ಎಣಿಕೆ  = 0;
    varnumber_T	byte_ಎಣಿಕೆ_cursor = 0;
    varnumber_T	char_ಎಣಿಕೆ = 0;
    varnumber_T	char_ಎಣಿಕೆ_cursor = 0;
    varnumber_T	ಶಬ್ದ_ಎಣಿಕೆ = 0;
    varnumber_T	ಶಬ್ದ_ಎಣಿಕೆ_cursor = 0;
    ಪೂರ್ಣಾಂಕ		eol_ಗಾತ್ರ;
    varnumber_T	ಕೊನೆಯ_check = 100000L;
    ದೀರ್ಘ	ಸಾಲು_ಎಣಿಕೆ_selected = 0;
    pos_T	min_pos, max_pos;
    oparg_T	oparg;
    ರಚನೆ ಖಂಡ_def	bd;

    /*
     * Compute the length of the file in characters.
     */
    ಆದರೆ (curbuf->b_ml.ml_flags & ML_ಖಾಲಿಗಳು)
    {
	ಆದರೆ (dict == ಖಾಲಿ)
	{
	    ಸಂದೇಶ(_(no_ಸಾಲುಗಳು_ಸಂದೇಶ));
	    ಹಿಂತಿರುಗಿ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (get_fileformat(curbuf) == EOL_DOS)
	    eol_ಗಾತ್ರ = 2;
	ಇಲ್ಲದಿದ್ದರೆ
	    eol_ಗಾತ್ರ = 1;

	ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
	{
	    ಆದರೆ (LT_POS(VIsual, curwin->w_cursor))
	    {
		min_pos = VIsual;
		max_pos = curwin->w_cursor;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		min_pos = curwin->w_cursor;
		max_pos = VIsual;
	    }
	    ಆದರೆ (*p_sel == 'e' && max_pos.ಲಂಬಸಾಲು > 0)
		--max_pos.ಲಂಬಸಾಲು;

	    ಆದರೆ (VIsual_ಬಹುಲಕ == Ctrl_V)
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
		char_u * saved_sbr = p_sbr;
		char_u * saved_w_sbr = curwin->w_p_sbr;

		// Make 'sbr' empty for a moment to get the correct size.
		p_sbr = ಖಾಲಿಗಳು_option;
		curwin->w_p_sbr = ಖಾಲಿಗಳು_option;
#ಆದರೆ_ಅಂತ್ಯ
		oparg.is_VIsual = 1;
		oparg.ಖಂಡ_ಬಹುಲಕ = TRUE;
		oparg.op_type = OP_NOP;
		getvcols(curwin, &min_pos, &max_pos,
					&oparg.ಆರಂಭ_vcol, &oparg.ಕೊನೆ_vcol, 0);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
		p_sbr = saved_sbr;
		curwin->w_p_sbr = saved_w_sbr;
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ (curwin->w_curswant == MAXCOL)
		    oparg.ಕೊನೆ_vcol = MAXCOL;
		// Swap the start, end vcol if needed
		ಆದರೆ (oparg.ಕೊನೆ_vcol < oparg.ಆರಂಭ_vcol)
		{
		    oparg.ಕೊನೆ_vcol += oparg.ಆರಂಭ_vcol;
		    oparg.ಆರಂಭ_vcol = oparg.ಕೊನೆ_vcol - oparg.ಆರಂಭ_vcol;
		    oparg.ಕೊನೆ_vcol -= oparg.ಆರಂಭ_vcol;
		}
	    }
	    ಸಾಲು_ಎಣಿಕೆ_selected = max_pos.lnum - min_pos.lnum + 1;
	}

	ಗಾಗಿ (lnum = 1; lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ; ++lnum)
	{
	    // Check for a CTRL-C every 100000 characters.
	    ಆದರೆ (byte_ಎಣಿಕೆ > ಕೊನೆಯ_check)
	    {
		ui_breakcheck();
		ಆದರೆ (got_int)
		    ಹಿಂತಿರುಗಿ;
		ಕೊನೆಯ_check = byte_ಎಣಿಕೆ + 100000L;
	    }

	    // Do extra processing for VIsual mode.
	    ಆದರೆ (VIsual_ಸಕ್ರಿಯ
		    && lnum >= min_pos.lnum && lnum <= max_pos.lnum)
	    {
		char_u	    *s = ಖಾಲಿ;
		ದೀರ್ಘ	    len = 0L;

		ಆಯ್ಕೆ (VIsual_ಬಹುಲಕ)
		{
		    ಸ್ಥಿತಿ Ctrl_V:
			virtual_op = virtual_ಸಕ್ರಿಯ();
			ಖಂಡ_prep(&oparg, &bd, lnum, 0);
			virtual_op = MAYBE;
			s = bd.textstart;
			len = (ದೀರ್ಘ)bd.textlen;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'V':
			s = ml_get(lnum);
			len = MAXCOL;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 'v':
			{
			    colnr_T ಆರಂಭ_ಲಂಬಸಾಲು = (lnum == min_pos.lnum)
							   ? min_pos.ಲಂಬಸಾಲು : 0;
			    colnr_T ಕೊನೆ_ಲಂಬಸಾಲು = (lnum == max_pos.lnum)
				      ? max_pos.ಲಂಬಸಾಲು - ಆರಂಭ_ಲಂಬಸಾಲು + 1 : MAXCOL;

			    s = ml_get(lnum) + ಆರಂಭ_ಲಂಬಸಾಲು;
			    len = ಕೊನೆ_ಲಂಬಸಾಲು;
			}
			ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (s != ಖಾಲಿ)
		{
		    byte_ಎಣಿಕೆ_cursor += ಸಾಲು_ಎಣಿಕೆ_info(s, &ಶಬ್ದ_ಎಣಿಕೆ_cursor,
					   &char_ಎಣಿಕೆ_cursor, len, eol_ಗಾತ್ರ);
		    ಆದರೆ (lnum == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
			    && !curbuf->b_p_eol
			    && (curbuf->b_p_bin || !curbuf->b_p_fixeol)
			    && (ದೀರ್ಘ)STRLEN(s) < len)
			byte_ಎಣಿಕೆ_cursor -= eol_ಗಾತ್ರ;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// In non-visual mode, check for the line the cursor is on
		ಆದರೆ (lnum == curwin->w_cursor.lnum)
		{
		    ಶಬ್ದ_ಎಣಿಕೆ_cursor += ಶಬ್ದ_ಎಣಿಕೆ;
		    char_ಎಣಿಕೆ_cursor += char_ಎಣಿಕೆ;
		    byte_ಎಣಿಕೆ_cursor = byte_ಎಣಿಕೆ +
			ಸಾಲು_ಎಣಿಕೆ_info(ml_get(lnum),
				&ಶಬ್ದ_ಎಣಿಕೆ_cursor, &char_ಎಣಿಕೆ_cursor,
				(varnumber_T)(curwin->w_cursor.ಲಂಬಸಾಲು + 1),
				eol_ಗಾತ್ರ);
		}
	    }
	    // Add to the running totals
	    byte_ಎಣಿಕೆ += ಸಾಲು_ಎಣಿಕೆ_info(ml_get(lnum), &ಶಬ್ದ_ಎಣಿಕೆ,
					 &char_ಎಣಿಕೆ, (varnumber_T)MAXCOL,
					 eol_ಗಾತ್ರ);
	}

	// Correction for when last line doesn't have an EOL.
	ಆದರೆ (!curbuf->b_p_eol && (curbuf->b_p_bin || !curbuf->b_p_fixeol))
	    byte_ಎಣಿಕೆ -= eol_ಗಾತ್ರ;

	ಆದರೆ (dict == ಖಾಲಿ)
	{
	    ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
	    {
		ಆದರೆ (VIsual_ಬಹುಲಕ == Ctrl_V && curwin->w_curswant < MAXCOL)
		{
		    getvcols(curwin, &min_pos, &max_pos,
						&min_pos.ಲಂಬಸಾಲು, &max_pos.ಲಂಬಸಾಲು, 0);
		    vim_snprintf((ಅಕ್ಷರ *)buf1, ಗಾತ್ರ(buf1), _("%ld Cols; "),
			    (ದೀರ್ಘ)(oparg.ಕೊನೆ_vcol - oparg.ಆರಂಭ_vcol + 1));
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    buf1[0] = NUL;

		ಆದರೆ (char_ಎಣಿಕೆ_cursor == byte_ಎಣಿಕೆ_cursor
						    && char_ಎಣಿಕೆ == byte_ಎಣಿಕೆ)
		    vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE,
			    _("Selected %s%ld of %ld Lines; %lld of %lld Words; %lld of %lld Bytes"),
			    buf1, ಸಾಲು_ಎಣಿಕೆ_selected,
			    (ದೀರ್ಘ)curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ,
			    ಶಬ್ದ_ಎಣಿಕೆ_cursor,
			    ಶಬ್ದ_ಎಣಿಕೆ,
			    byte_ಎಣಿಕೆ_cursor,
			    byte_ಎಣಿಕೆ);
		ಇಲ್ಲದಿದ್ದರೆ
		    vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE,
			    _("Selected %s%ld of %ld Lines; %lld of %lld Words; %lld of %lld Chars; %lld of %lld Bytes"),
			    buf1, ಸಾಲು_ಎಣಿಕೆ_selected,
			    (ದೀರ್ಘ)curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ,
			    ಶಬ್ದ_ಎಣಿಕೆ_cursor,
			    ಶಬ್ದ_ಎಣಿಕೆ,
			    char_ಎಣಿಕೆ_cursor,
			    char_ಎಣಿಕೆ,
			    byte_ಎಣಿಕೆ_cursor,
			    byte_ಎಣಿಕೆ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		p = ml_get_curline();
		validate_virtcol();
		ಲಂಬಸಾಲು_print(buf1, ಗಾತ್ರ(buf1), (ಪೂರ್ಣಾಂಕ)curwin->w_cursor.ಲಂಬಸಾಲು + 1,
			(ಪೂರ್ಣಾಂಕ)curwin->w_virtcol + 1);
		ಲಂಬಸಾಲು_print(buf2, ಗಾತ್ರ(buf2), ml_get_curline_len(),
							   linetabsize_str(p));

		ಆದರೆ (char_ಎಣಿಕೆ_cursor == byte_ಎಣಿಕೆ_cursor
			&& char_ಎಣಿಕೆ == byte_ಎಣಿಕೆ)
		    vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE,
			_("Col %s of %s; Line %ld of %ld; Word %lld of %lld; Byte %lld of %lld"),
			(ಅಕ್ಷರ *)buf1, (ಅಕ್ಷರ *)buf2,
			(ದೀರ್ಘ)curwin->w_cursor.lnum,
			(ದೀರ್ಘ)curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ,
			ಶಬ್ದ_ಎಣಿಕೆ_cursor, ಶಬ್ದ_ಎಣಿಕೆ,
			byte_ಎಣಿಕೆ_cursor, byte_ಎಣಿಕೆ);
		ಇಲ್ಲದಿದ್ದರೆ
		    vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE,
			_("Col %s of %s; Line %ld of %ld; Word %lld of %lld; Char %lld of %lld; Byte %lld of %lld"),
			(ಅಕ್ಷರ *)buf1, (ಅಕ್ಷರ *)buf2,
			(ದೀರ್ಘ)curwin->w_cursor.lnum,
			(ದೀರ್ಘ)curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ,
			ಶಬ್ದ_ಎಣಿಕೆ_cursor, ಶಬ್ದ_ಎಣಿಕೆ,
			char_ಎಣಿಕೆ_cursor, char_ಎಣಿಕೆ,
			byte_ಎಣಿಕೆ_cursor, byte_ಎಣಿಕೆ);
	    }
	}

	bom_ಎಣಿಕೆ = bomb_ಗಾತ್ರ();
	ಆದರೆ (dict == ಖಾಲಿ && bom_ಎಣಿಕೆ > 0)
	{
	    size_t len = STRLEN(IObuff);

	    vim_snprintf((ಅಕ್ಷರ *)IObuff + len, IOSIZE - len,
				 _("(+%lld for BOM)"), bom_ಎಣಿಕೆ);
	}
	ಆದರೆ (dict == ಖಾಲಿ)
	{
	    // Don't shorten this message, the user asked for it.
	    p = p_shm;
	    p_shm = (char_u *)"";
	    ಸಂದೇಶ((ಅಕ್ಷರ *)IObuff);
	    p_shm = p;
	}
    }
#ಆದರೆ defined(FEAT_EVAL)
    ಆದರೆ (dict != ಖಾಲಿ)
    {
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "words", ಶಬ್ದ_ಎಣಿಕೆ);
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "chars", char_ಎಣಿಕೆ);
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "bytes", byte_ಎಣಿಕೆ + bom_ಎಣಿಕೆ);
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, VIsual_ಸಕ್ರಿಯ ? "visual_bytes" : "cursor_bytes",
		byte_ಎಣಿಕೆ_cursor);
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, VIsual_ಸಕ್ರಿಯ ? "visual_chars" : "cursor_chars",
		char_ಎಣಿಕೆ_cursor);
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, VIsual_ಸಕ್ರಿಯ ? "visual_words" : "cursor_words",
		ಶಬ್ದ_ಎಣಿಕೆ_cursor);
    }
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Handle indent and format operators and visual mode ":".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
op_colon(oparg_T *oap)
{
    stuffcharReadbuff(':');
    ಆದರೆ (oap->is_VIsual)
	stuffReadbuff((char_u *)"'<,'>");
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Make the range look nice, so it can be repeated.
	ಆದರೆ (oap->ಆರಂಭ.lnum == curwin->w_cursor.lnum)
	    stuffcharReadbuff('.');
	ಇಲ್ಲದಿದ್ದರೆ
	    stuffnumReadbuff((ದೀರ್ಘ)oap->ಆರಂಭ.lnum);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	// When using !! on a closed fold the range ".!" works best to operate
	// on, it will be made the whole closed fold later.
	linenr_T ಕೊನೆ_ದ_ಆರಂಭ_Fold = oap->ಆರಂಭ.lnum;
	(ಶೂನ್ಯ)hasFolding(oap->ಆರಂಭ.lnum, ಖಾಲಿ, &ಕೊನೆ_ದ_ಆರಂಭ_Fold);
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (oap->ಕೊನೆ.lnum != oap->ಆರಂಭ.lnum
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
		&& oap->ಕೊನೆ.lnum != ಕೊನೆ_ದ_ಆರಂಭ_Fold
#ಆದರೆ_ಅಂತ್ಯ
	   )
	{
	    // Make it a range with the end line.
	    stuffcharReadbuff(',');
	    ಆದರೆ (oap->ಕೊನೆ.lnum == curwin->w_cursor.lnum)
		stuffcharReadbuff('.');
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (oap->ಕೊನೆ.lnum == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		stuffcharReadbuff('$');
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (oap->ಆರಂಭ.lnum == curwin->w_cursor.lnum
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
		    // do not use ".+number" for a closed fold, it would count
		    // folded lines twice
		    && !hasFolding(oap->ಕೊನೆ.lnum, ಖಾಲಿ, ಖಾಲಿ)
#ಆದರೆ_ಅಂತ್ಯ
		    )
	    {
		stuffReadbuff((char_u *)".+");
		stuffnumReadbuff((ದೀರ್ಘ)oap->ಸಾಲು_ಎಣಿಕೆ - 1);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		stuffnumReadbuff((ದೀರ್ಘ)oap->ಕೊನೆ.lnum);
	}
    }
    ಆದರೆ (oap->op_type != OP_COLON)
	stuffReadbuff((char_u *)"!");
    ಆದರೆ (oap->op_type == OP_INDENT)
    {
	ಆದರೆ (*get_equalprg() == NUL)
	    stuffReadbuff((char_u *)"indent");
	ಇಲ್ಲದಿದ್ದರೆ
	    stuffReadbuff(get_equalprg());
	stuffReadbuff((char_u *)"\n");
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (oap->op_type == OP_FORMAT)
    {
	ಆದರೆ (*curbuf->b_p_fp != NUL)
	    stuffReadbuff(curbuf->b_p_fp);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p_fp != NUL)
	    stuffReadbuff(p_fp);
	ಇಲ್ಲದಿದ್ದರೆ
	    stuffReadbuff((char_u *)"fmt");
	stuffReadbuff((char_u *)"\n']");
    }

    // do_cmdline() does the rest
}

// callback function for 'operatorfunc'
ಸ್ಥಿರ callback_T opfunc_cb;

/*
 * Process the 'operatorfunc' option value.
 * Returns OK or FAIL.
 */
    ಅಕ್ಷರ *
did_ಗಣ_operatorfunc(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಆದರೆ (option_ಗಣ_callback_func(p_opfunc, &opfunc_cb) == FAIL)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
free_operatorfunc_option(ಶೂನ್ಯ)
{
# ifdef FEAT_EVAL
    free_callback(&opfunc_cb);
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Mark the global 'operatorfunc' callback with "copyID" so that it is not
 * garbage collected.
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_opfunc(ಪೂರ್ಣಾಂಕ copyID ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ abort = FALSE;

    abort = ಗಣ_ref_ಒಳ_callback(&opfunc_cb, copyID);

    ಹಿಂತಿರುಗಿ abort;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Handle the "g@" operator: call 'operatorfunc'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
op_function(oparg_T *oap ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    typval_T	argv[2];
    pos_T	orig_ಆರಂಭ = curbuf->b_op_ಆರಂಭ;
    pos_T	orig_ಕೊನೆ = curbuf->b_op_ಕೊನೆ;
    typval_T	rettv;

    ಆದರೆ (*p_opfunc == NUL)
	emsg(_(e_operatorfunc_is_ಖಾಲಿಗಳು));
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Set '[ and '] marks to text to be operated on.
	curbuf->b_op_ಆರಂಭ = oap->ಆರಂಭ;
	curbuf->b_op_ಕೊನೆ = oap->ಕೊನೆ;
	ಆದರೆ (oap->motion_type != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_ಕೊನೆ);

	argv[0].v_type = VAR_STRING;
	ಆದರೆ (oap->ಖಂಡ_ಬಹುಲಕ)
	    argv[0].vval.v_string = (char_u *)"block";
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (oap->motion_type == MLINE)
	    argv[0].vval.v_string = (char_u *)"line";
	ಇಲ್ಲದಿದ್ದರೆ
	    argv[0].vval.v_string = (char_u *)"char";
	argv[1].v_type = VAR_UNKNOWN;

	// Reset virtual_op so that 'virtualedit' can be changed in the
	// function.
	ಪೂರ್ಣಾಂಕ save_virtual_op = virtual_op;
	virtual_op = MAYBE;

	// Reset finish_op so that mode() returns the right value.
	ಪೂರ್ಣಾಂಕ save_ಮುಕ್ತಾಯ_op = ಮುಕ್ತಾಯ_op;
	ಮುಕ್ತಾಯ_op = FALSE;

	ಆದರೆ (call_callback(&opfunc_cb, 0, &rettv, 1, argv) != FAIL)
	    clear_tv(&rettv);

	virtual_op = save_virtual_op;
	ಮುಕ್ತಾಯ_op = save_ಮುಕ್ತಾಯ_op;
	ಆದರೆ (cmdmod.cmod_flags & CMOD_LOCKMARKS)
	{
	    curbuf->b_op_ಆರಂಭ = orig_ಆರಂಭ;
	    curbuf->b_op_ಕೊನೆ = orig_ಕೊನೆ;
	}
    }
#ಇಲ್ಲದಿದ್ದರೆ
    emsg(_(e_eval_feature_not_ಲಭ್ಯ));
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Calculate start/end virtual columns for operating in block mode.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_op_vcol(
    oparg_T	*oap,
    colnr_T	redo_VIsual_vcol,
    ಪೂರ್ಣಾಂಕ		initial)    // when TRUE adjust position for 'selectmode'
{
    colnr_T	    ಆರಂಭ, ಕೊನೆ;

    ಆದರೆ (VIsual_ಬಹುಲಕ != Ctrl_V
	    || (!initial && oap->ಕೊನೆ.ಲಂಬಸಾಲು < curwin->w_ಅಗಲ))
	ಹಿಂತಿರುಗಿ;

    oap->ಖಂಡ_ಬಹುಲಕ = TRUE;

    // prevent from moving onto a trail byte
    ಆದರೆ (has_mbyte)
	mb_adjustpos(curwin->w_ಬಫರ್, &oap->ಕೊನೆ);

    getvvcol(curwin, &(oap->ಆರಂಭ), &oap->ಆರಂಭ_vcol, ಖಾಲಿ, &oap->ಕೊನೆ_vcol, 0);

    ಆದರೆ (!redo_VIsual_busy)
    {
	getvvcol(curwin, &(oap->ಕೊನೆ), &ಆರಂಭ, ಖಾಲಿ, &ಕೊನೆ, 0);

	ಆದರೆ (ಆರಂಭ < oap->ಆರಂಭ_vcol)
	    oap->ಆರಂಭ_vcol = ಆರಂಭ;
	ಆದರೆ (ಕೊನೆ > oap->ಕೊನೆ_vcol)
	{
	    ಆದರೆ (initial && *p_sel == 'e' && ಆರಂಭ >= 1
				    && ಆರಂಭ - 1 >= oap->ಕೊನೆ_vcol)
		oap->ಕೊನೆ_vcol = ಆರಂಭ - 1;
	    ಇಲ್ಲದಿದ್ದರೆ
		oap->ಕೊನೆ_vcol = ಕೊನೆ;
	}
    }

    // if '$' was used, get oap->end_vcol from longest line
    ಆದರೆ (curwin->w_curswant == MAXCOL)
    {
	curwin->w_cursor.ಲಂಬಸಾಲು = MAXCOL;
	oap->ಕೊನೆ_vcol = 0;
	ಗಾಗಿ (curwin->w_cursor.lnum = oap->ಆರಂಭ.lnum;
		curwin->w_cursor.lnum <= oap->ಕೊನೆ.lnum;
					++curwin->w_cursor.lnum)
	{
	    getvvcol(curwin, &curwin->w_cursor, ಖಾಲಿ, ಖಾಲಿ, &ಕೊನೆ, 0);
	    ಆದರೆ (ಕೊನೆ > oap->ಕೊನೆ_vcol)
		oap->ಕೊನೆ_vcol = ಕೊನೆ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (redo_VIsual_busy)
	oap->ಕೊನೆ_vcol = oap->ಆರಂಭ_vcol + redo_VIsual_vcol - 1;
    // Correct oap->end.col and oap->start.col to be the
    // upper-left and lower-right corner of the block area.
    //
    // (Actually, this does convert column positions into character
    // positions)
    curwin->w_cursor.lnum = oap->ಕೊನೆ.lnum;
    coladvance(oap->ಕೊನೆ_vcol);
    oap->ಕೊನೆ = curwin->w_cursor;

    curwin->w_cursor = oap->ಆರಂಭ;
    coladvance(oap->ಆರಂಭ_vcol);
    oap->ಆರಂಭ = curwin->w_cursor;
}

// Information for redoing the previous Visual selection.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		rv_ಬಹುಲಕ;	// 'v', 'V', or Ctrl-V
    linenr_T	rv_ಸಾಲು_ಎಣಿಕೆ;	// number of lines
    colnr_T	rv_vcol;	// number of cols or end column
    ದೀರ್ಘ	rv_ಎಣಿಕೆ;	// count for Visual operator
    ಪೂರ್ಣಾಂಕ		rv_arg;		// extra argument
} redo_VIsual_T;

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
is_ex_cmdchar(cmdarg_T *cap)
{
    ಹಿಂತಿರುಗಿ cap->cmdchar == ':'
	|| cap->cmdchar == K_COMMAND
	|| cap->cmdchar == K_SCRIPT_COMMAND;
}

/*
 * Handle an operator after Visual mode or when the movement is finished.
 * "gui_yank" is true when yanking text for the clipboard.
 */
    ಶೂನ್ಯ
do_pending_operator(cmdarg_T *cap, ಪೂರ್ಣಾಂಕ old_ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ gui_yank)
{
    oparg_T	*oap = cap->oap;
    pos_T	old_cursor;
    ಪೂರ್ಣಾಂಕ		ಖಾಲಿಗಳು_region_error;
    ಪೂರ್ಣಾಂಕ		restart_edit_save;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    ಪೂರ್ಣಾಂಕ		lbr_saved = curwin->w_p_lbr;
#ಆದರೆ_ಅಂತ್ಯ

    // The visual area is remembered for redo
    ಸ್ಥಿರ redo_VIsual_T   redo_VIsual = {NUL, 0, 0, 0,0};

    ಪೂರ್ಣಾಂಕ		    include_ಸಾಲು_break = FALSE;

#ಆದರೆ defined(FEAT_CLIPBOARD)
    // Yank the visual area into the GUI selection register before we operate
    // on it and lose it forever.
    // Don't do it if a specific register was specified, so that ""x"*P works.
    // This could call do_pending_operator() recursively, but that's OK
    // because gui_yank will be TRUE for the nested call.
    ಆದರೆ ((clip_ನಕ್ಷತ್ರ.ಲಭ್ಯ || clip_plus.ಲಭ್ಯ)
	    && oap->op_type != OP_NOP
	    && !gui_yank
	    && VIsual_ಸಕ್ರಿಯ
	    && !redo_VIsual_busy
	    && oap->regname == 0)
	clip_auto_select();
#ಆದರೆ_ಅಂತ್ಯ
    old_cursor = curwin->w_cursor;

    // If an operation is pending, handle it...
    ಆದರೆ ((ಮುಕ್ತಾಯ_op || VIsual_ಸಕ್ರಿಯ) && oap->op_type != OP_NOP)
    {
	// Yank can be redone when 'y' is in 'cpoptions', but not when yanking
	// for the clipboard.
	ಪೂರ್ಣಾಂಕ	redo_yank = vim_strchr(p_cpo, CPO_YANK) != ಖಾಲಿ && !gui_yank;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
	// Avoid a problem with unwanted linebreaks in block mode.
	(ಶೂನ್ಯ)reset_lbr();
#ಆದರೆ_ಅಂತ್ಯ
	oap->is_VIsual = VIsual_ಸಕ್ರಿಯ;
	ಆದರೆ (oap->motion_force == 'V')
	    oap->motion_type = MLINE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (oap->motion_force == 'v')
	{
	    // If the motion was linewise, "inclusive" will not have been set.
	    // Use "exclusive" to be consistent.  Makes "dvj" work nice.
	    ಆದರೆ (oap->motion_type == MLINE)
		oap->inclusive = FALSE;
	    // If the motion already was characterwise, toggle "inclusive"
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (oap->motion_type == MCHAR)
		oap->inclusive = !oap->inclusive;
	    oap->motion_type = MCHAR;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (oap->motion_force == Ctrl_V)
	{
	    // Change line- or characterwise motion into Visual block mode.
	    ಆದರೆ (!VIsual_ಸಕ್ರಿಯ)
	    {
		VIsual_ಸಕ್ರಿಯ = TRUE;
		VIsual = oap->ಆರಂಭ;
	    }
	    VIsual_ಬಹುಲಕ = Ctrl_V;
	    VIsual_select = FALSE;
	    VIsual_reselect = FALSE;
	}

	// Only redo yank when 'y' flag is in 'cpoptions'.
	// Never redo "zf" (define fold).
	ಆದರೆ ((redo_yank || oap->op_type != OP_YANK)
		&& ((!VIsual_ಸಕ್ರಿಯ || oap->motion_force)
		    // Also redo Operator-pending Visual mode mappings
		    || (VIsual_ಸಕ್ರಿಯ
			    && is_ex_cmdchar(cap) && oap->op_type != OP_COLON))
		&& cap->cmdchar != 'D'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
		&& oap->op_type != OP_FOLD
		&& oap->op_type != OP_FOLDOPEN
		&& oap->op_type != OP_FOLDOPENREC
		&& oap->op_type != OP_FOLDCLOSE
		&& oap->op_type != OP_FOLDCLOSEREC
		&& oap->op_type != OP_FOLDDEL
		&& oap->op_type != OP_FOLDDELREC
#ಆದರೆ_ಅಂತ್ಯ
		)
	{
	    prep_redo(oap->regname, cap->count0,
		    get_op_char(oap->op_type), get_extra_op_char(oap->op_type),
		    oap->motion_force, cap->cmdchar, cap->nchar);
	    ಆದರೆ (cap->cmdchar == '/' || cap->cmdchar == '?') // was a search
	    {
		// If 'cpoptions' does not contain 'r', insert the search
		// pattern to really repeat the same command.
		ಆದರೆ (vim_strchr(p_cpo, CPO_REDO) == ಖಾಲಿ)
		    Append_ಗಮ್ಯ_Redobuff_Lit(cap->searchbuf, -1);
		Append_ಗಮ್ಯ_Redobuff(NL_STR);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_ex_cmdchar(cap))
	    {
		// do_cmdline() has stored the first typed line in
		// "repeat_cmdline".  When several lines are typed repeating
		// won't be possible.
		ಆದರೆ (repeat_cmdline == ಖಾಲಿ)
		    ResetRedobuff();
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (cap->cmdchar == ':')
			Append_ಗಮ್ಯ_Redobuff_Lit(repeat_cmdline, -1);
		    ಇಲ್ಲದಿದ್ದರೆ
			Append_ಗಮ್ಯ_Redobuff_Spec(repeat_cmdline);
		    Append_ಗಮ್ಯ_Redobuff(NL_STR);
		    VIM_CLEAR(repeat_cmdline);
		}
	    }
	}

	ಆದರೆ (redo_VIsual_busy)
	{
	    // Redo of an operation on a Visual area. Use the same size from
	    // redo_VIsual.rv_line_count and redo_VIsual.rv_vcol.
	    oap->ಆರಂಭ = curwin->w_cursor;
	    curwin->w_cursor.lnum += redo_VIsual.rv_ಸಾಲು_ಎಣಿಕೆ - 1;
	    ಆದರೆ (curwin->w_cursor.lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    VIsual_ಬಹುಲಕ = redo_VIsual.rv_ಬಹುಲಕ;
	    ಆದರೆ (redo_VIsual.rv_vcol == MAXCOL || VIsual_ಬಹುಲಕ == 'v')
	    {
		ಆದರೆ (VIsual_ಬಹುಲಕ == 'v')
		{
		    ಆದರೆ (redo_VIsual.rv_ಸಾಲು_ಎಣಿಕೆ <= 1)
		    {
			validate_virtcol();
			curwin->w_curswant =
				     curwin->w_virtcol + redo_VIsual.rv_vcol - 1;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			curwin->w_curswant = redo_VIsual.rv_vcol;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    curwin->w_curswant = MAXCOL;
		}
		coladvance(curwin->w_curswant);
	    }
	    cap->count0 = redo_VIsual.rv_ಎಣಿಕೆ;
	    ಆದರೆ (redo_VIsual.rv_ಎಣಿಕೆ != 0)
		cap->count1 = redo_VIsual.rv_ಎಣಿಕೆ;
	    ಇಲ್ಲದಿದ್ದರೆ
		cap->count1 = 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
	{
	    ಆದರೆ (!gui_yank)
	    {
		// Save the current VIsual area for '< and '> marks, and "gv"
		curbuf->b_visual.vi_ಆರಂಭ = VIsual;
		curbuf->b_visual.vi_ಕೊನೆ = curwin->w_cursor;
		curbuf->b_visual.vi_ಬಹುಲಕ = VIsual_ಬಹುಲಕ;
		restore_visual_ಬಹುಲಕ();
		curbuf->b_visual.vi_curswant = curwin->w_curswant;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		curbuf->b_visual_ಬಹುಲಕ_eval = VIsual_ಬಹುಲಕ;
#ಆದರೆ_ಅಂತ್ಯ
	    }

	    // In Select mode, a linewise selection is operated upon like a
	    // characterwise selection.
	    // Special case: gH<Del> deletes the last line.
	    ಆದರೆ (VIsual_select && VIsual_ಬಹುಲಕ == 'V'
					    && cap->oap->op_type != OP_DELETE)
	    {
		ಆದರೆ (LT_POS(VIsual, curwin->w_cursor))
		{
		    VIsual.ಲಂಬಸಾಲು = 0;
		    curwin->w_cursor.ಲಂಬಸಾಲು = ml_get_len(curwin->w_cursor.lnum);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    curwin->w_cursor.ಲಂಬಸಾಲು = 0;
		    VIsual.ಲಂಬಸಾಲು = ml_get_len(VIsual.lnum);
		}
		VIsual_ಬಹುಲಕ = 'v';
	    }
	    // If 'selection' is "exclusive", backup one character for
	    // charwise selections.
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (VIsual_ಬಹುಲಕ == 'v')
		include_ಸಾಲು_break = unadjust_for_sel();

	    oap->ಆರಂಭ = VIsual;
	    ಆದರೆ (VIsual_ಬಹುಲಕ == 'V')
	    {
		oap->ಆರಂಭ.ಲಂಬಸಾಲು = 0;
		oap->ಆರಂಭ.coladd = 0;
	    }
	}

	// Set oap->start to the first position of the operated text, oap->end
	// to the end of the operated text.  w_cursor is equal to oap->start.
	ಆದರೆ (LT_POS(oap->ಆರಂಭ, curwin->w_cursor))
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    // Include folded lines completely.
	    ಆದರೆ (!VIsual_ಸಕ್ರಿಯ)
	    {
		ಆದರೆ (hasFolding(oap->ಆರಂಭ.lnum, &oap->ಆರಂಭ.lnum, ಖಾಲಿ))
		    oap->ಆರಂಭ.ಲಂಬಸಾಲು = 0;
		ಆದರೆ ((curwin->w_cursor.ಲಂಬಸಾಲು > 0 || oap->inclusive
						  || oap->motion_type == MLINE)
			&& hasFolding(curwin->w_cursor.lnum, ಖಾಲಿ,
						      &curwin->w_cursor.lnum))
		    curwin->w_cursor.ಲಂಬಸಾಲು = ml_get_curline_len();
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    oap->ಕೊನೆ = curwin->w_cursor;
	    curwin->w_cursor = oap->ಆರಂಭ;

	    // w_virtcol may have been updated; if the cursor goes back to its
	    // previous position w_virtcol becomes invalid and isn't updated
	    // automatically.
	    curwin->w_valid &= ~VALID_VIRTCOL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    // Include folded lines completely.
	    ಆದರೆ (!VIsual_ಸಕ್ರಿಯ && oap->motion_type == MLINE)
	    {
		ಆದರೆ (hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum,
									ಖಾಲಿ))
		    curwin->w_cursor.ಲಂಬಸಾಲು = 0;
		ಆದರೆ (hasFolding(oap->ಆರಂಭ.lnum, ಖಾಲಿ, &oap->ಆರಂಭ.lnum))
		    oap->ಆರಂಭ.ಲಂಬಸಾಲು = ml_get_len(oap->ಆರಂಭ.lnum);
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    oap->ಕೊನೆ = oap->ಆರಂಭ;
	    oap->ಆರಂಭ = curwin->w_cursor;
	}

	// Just in case lines were deleted that make the position invalid.
	check_pos(curwin->w_ಬಫರ್, &oap->ಕೊನೆ);
	oap->ಸಾಲು_ಎಣಿಕೆ = oap->ಕೊನೆ.lnum - oap->ಆರಂಭ.lnum + 1;

	// Set "virtual_op" before resetting VIsual_active.
	virtual_op = virtual_ಸಕ್ರಿಯ();

	ಆದರೆ (VIsual_ಸಕ್ರಿಯ || redo_VIsual_busy)
	{
	    get_op_vcol(oap, redo_VIsual.rv_vcol, TRUE);

	    ಆದರೆ (!redo_VIsual_busy && !gui_yank)
	    {
		// Prepare to reselect and redo Visual: this is based on the
		// size of the Visual text
		resel_VIsual_ಬಹುಲಕ = VIsual_ಬಹುಲಕ;
		ಆದರೆ (curwin->w_curswant == MAXCOL)
		    resel_VIsual_vcol = MAXCOL;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (VIsual_ಬಹುಲಕ != Ctrl_V)
			getvvcol(curwin, &(oap->ಕೊನೆ),
						ಖಾಲಿ, ಖಾಲಿ, &oap->ಕೊನೆ_vcol, 0);
		    ಆದರೆ (VIsual_ಬಹುಲಕ == Ctrl_V || oap->ಸಾಲು_ಎಣಿಕೆ <= 1)
		    {
			ಆದರೆ (VIsual_ಬಹುಲಕ != Ctrl_V)
			    getvvcol(curwin, &(oap->ಆರಂಭ),
					      &oap->ಆರಂಭ_vcol, ಖಾಲಿ, ಖಾಲಿ, 0);
			resel_VIsual_vcol = oap->ಕೊನೆ_vcol - oap->ಆರಂಭ_vcol + 1;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			resel_VIsual_vcol = oap->ಕೊನೆ_vcol;
		}
		resel_VIsual_ಸಾಲು_ಎಣಿಕೆ = oap->ಸಾಲು_ಎಣಿಕೆ;
	    }

	    // can't redo yank (unless 'y' is in 'cpoptions') and ":"
	    ಆದರೆ ((redo_yank || oap->op_type != OP_YANK)
		    && oap->op_type != OP_COLON
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
		    && oap->op_type != OP_FOLD
		    && oap->op_type != OP_FOLDOPEN
		    && oap->op_type != OP_FOLDOPENREC
		    && oap->op_type != OP_FOLDCLOSE
		    && oap->op_type != OP_FOLDCLOSEREC
		    && oap->op_type != OP_FOLDDEL
		    && oap->op_type != OP_FOLDDELREC
#ಆದರೆ_ಅಂತ್ಯ
		    && oap->motion_force == NUL
		    )
	    {
		// Prepare for redoing.  Only use the nchar field for "r",
		// otherwise it might be the second char of the operator.
		ಆದರೆ (cap->cmdchar == 'g' && (cap->nchar == 'n'
							|| cap->nchar == 'N'))
		    prep_redo(oap->regname, cap->count0,
			    get_op_char(oap->op_type),
			    get_extra_op_char(oap->op_type),
			    oap->motion_force, cap->cmdchar, cap->nchar);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!is_ex_cmdchar(cap))
		{
		    ಪೂರ್ಣಾಂಕ opchar = get_op_char(oap->op_type);
		    ಪೂರ್ಣಾಂಕ extra_opchar = get_extra_op_char(oap->op_type);
		    ಪೂರ್ಣಾಂಕ nchar = oap->op_type == OP_REPLACE ? cap->nchar : NUL;

		    // reverse what nv_replace() did
		    ಆದರೆ (nchar == REPLACE_CR_NCHAR)
			nchar = CAR;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (nchar == REPLACE_NL_NCHAR)
			nchar = NL;

		    ಆದರೆ (opchar == 'g' && extra_opchar == '@')
			// also repeat the count for 'operatorfunc'
			prep_redo_num2(oap->regname, 0L, NUL, 'v',
				     cap->count0, opchar, extra_opchar, nchar);
		    ಇಲ್ಲದಿದ್ದರೆ
			prep_redo(oap->regname, 0L, NUL, 'v',
						  opchar, extra_opchar, nchar);
		}
		ಆದರೆ (!redo_VIsual_busy)
		{
		    redo_VIsual.rv_ಬಹುಲಕ = resel_VIsual_ಬಹುಲಕ;
		    redo_VIsual.rv_vcol = resel_VIsual_vcol;
		    redo_VIsual.rv_ಸಾಲು_ಎಣಿಕೆ = resel_VIsual_ಸಾಲು_ಎಣಿಕೆ;
		    redo_VIsual.rv_ಎಣಿಕೆ = cap->count0;
		    redo_VIsual.rv_arg = cap->arg;
		}
	    }

	    // oap->inclusive defaults to TRUE.
	    // If oap->end is on a NUL (empty line) oap->inclusive becomes
	    // FALSE.  This makes "d}P" and "v}dP" work the same.
	    ಆದರೆ (oap->motion_force == NUL || oap->motion_type == MLINE)
		oap->inclusive = TRUE;
	    ಆದರೆ (VIsual_ಬಹುಲಕ == 'V')
		oap->motion_type = MLINE;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		oap->motion_type = MCHAR;
		ಆದರೆ (VIsual_ಬಹುಲಕ != Ctrl_V && *ml_get_pos(&(oap->ಕೊನೆ)) == NUL
			&& (include_ಸಾಲು_break || !virtual_op))
		{
		    oap->inclusive = FALSE;
		    // Try to include the newline, unless it's an operator
		    // that works on lines only.
		    ಆದರೆ (*p_sel != 'o'
			    && !op_on_ಸಾಲುಗಳು(oap->op_type)
			    && oap->ಕೊನೆ.lnum < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		    {
			++oap->ಕೊನೆ.lnum;
			oap->ಕೊನೆ.ಲಂಬಸಾಲು = 0;
			oap->ಕೊನೆ.coladd = 0;
			++oap->ಸಾಲು_ಎಣಿಕೆ;
		    }
		}
	    }

	    redo_VIsual_busy = FALSE;

	    // Switch Visual off now, so screen updating does
	    // not show inverted text when the screen is redrawn.
	    // With OP_YANK and sometimes with OP_COLON and OP_FILTER there is
	    // no screen redraw, so it is done here to remove the inverted
	    // part.
	    ಆದರೆ (!gui_yank)
	    {
		VIsual_ಸಕ್ರಿಯ = FALSE;
		setmouse();
		mouse_dragging = 0;
		may_clear_cmdline();
		ಆದರೆ ((oap->op_type == OP_YANK
			    || oap->op_type == OP_COLON
			    || oap->op_type == OP_FUNCTION
			    || oap->op_type == OP_FILTER)
			&& oap->motion_force == NUL)
		{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
		    // make sure redrawing is correct
		    restore_lbr(lbr_saved);
#ಆದರೆ_ಅಂತ್ಯ
		    redraw_curbuf_later(UPD_INVERTED);
		}
	    }
	}

	// Include the trailing byte of a multi-byte char.
	ಆದರೆ (has_mbyte && oap->inclusive)
	{
	    ಪೂರ್ಣಾಂಕ		l;

	    l = (*mb_ptr2len)(ml_get_pos(&oap->ಕೊನೆ));
	    ಆದರೆ (l > 1)
		oap->ಕೊನೆ.ಲಂಬಸಾಲು += l - 1;
	}
	curwin->w_ಗಣ_curswant = true;

	// oap->empty is set when start and end are the same.  The inclusive
	// flag affects this too, unless yanking and the end is on a NUL.
	oap->ಖಾಲಿಗಳು_2 = (oap->motion_type == MCHAR
		    && (!oap->inclusive
			|| (oap->op_type == OP_YANK
			    && gchar_pos(&oap->ಕೊನೆ) == NUL))
		    && EQUAL_POS(oap->ಆರಂಭ, oap->ಕೊನೆ)
		    && !(virtual_op && oap->ಆರಂಭ.coladd != oap->ಕೊನೆ.coladd));
	// For delete, change and yank, it's an error to operate on an
	// empty region, when 'E' included in 'cpoptions' (Vi compatible).
	ಖಾಲಿಗಳು_region_error = (oap->ಖಾಲಿಗಳು_2
				&& vim_strchr(p_cpo, CPO_EMPTYREGION) != ಖಾಲಿ);

	// Force a redraw when operating on an empty Visual region, when
	// 'modifiable is off or creating a fold.
	ಆದರೆ (oap->is_VIsual && (oap->ಖಾಲಿಗಳು_2 || !curbuf->b_p_ma
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
		    || oap->op_type == OP_FOLD
#ಆದರೆ_ಅಂತ್ಯ
		    ))
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
	    restore_lbr(lbr_saved);
#ಆದರೆ_ಅಂತ್ಯ
	    redraw_curbuf_later(UPD_INVERTED);
	}

	// If the end of an operator is in column one while oap->motion_type
	// is MCHAR and oap->inclusive is FALSE, we put op_end after the last
	// character in the previous line. If op_start is on or before the
	// first non-blank in the line, the operator becomes linewise
	// (strange, but that's the way vi does it).
	ಆದರೆ (	   oap->motion_type == MCHAR
		&& oap->inclusive == FALSE
		&& !(cap->retval & CA_NO_ADJ_OP_ಕೊನೆ)
		&& oap->ಕೊನೆ.ಲಂಬಸಾಲು == 0
		&& (!oap->is_VIsual || *p_sel == 'o')
		&& !oap->ಖಂಡ_ಬಹುಲಕ
		&& oap->ಸಾಲು_ಎಣಿಕೆ > 1)
	{
	    oap->ಕೊನೆ_adjusted = TRUE;	    // remember that we did this
	    --oap->ಸಾಲು_ಎಣಿಕೆ;
	    --oap->ಕೊನೆ.lnum;
	    ಆದರೆ (inindent(0))
		oap->motion_type = MLINE;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		oap->ಕೊನೆ.ಲಂಬಸಾಲು = ml_get_len(oap->ಕೊನೆ.lnum);
		ಆದರೆ (oap->ಕೊನೆ.ಲಂಬಸಾಲು)
		{
		    --oap->ಕೊನೆ.ಲಂಬಸಾಲು;
		    oap->inclusive = TRUE;
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    oap->ಕೊನೆ_adjusted = FALSE;

	ಆಯ್ಕೆ (oap->op_type)
	{
	ಸ್ಥಿತಿ OP_LSHIFT:
	ಸ್ಥಿತಿ OP_RSHIFT:
	    op_ಸರಿಸು(oap, TRUE, oap->is_VIsual ? (ಪೂರ್ಣಾಂಕ)cap->count1 : 1);
	    auto_format(FALSE, TRUE);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ OP_JOIN_NS:
	ಸ್ಥಿತಿ OP_JOIN:
	    ಆದರೆ (oap->ಸಾಲು_ಎಣಿಕೆ < 2)
		oap->ಸಾಲು_ಎಣಿಕೆ = 2;
	    ಆದರೆ (curwin->w_cursor.lnum + oap->ಸಾಲು_ಎಣಿಕೆ - 1 >
						   curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		beep_flush();
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		(ಶೂನ್ಯ)do_join(oap->ಸಾಲು_ಎಣಿಕೆ, oap->op_type == OP_JOIN,
							    TRUE, TRUE, TRUE);
		auto_format(FALSE, TRUE);
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ OP_DELETE:
	    VIsual_reselect = FALSE;	    // don't reselect now
	    ಆದರೆ (ಖಾಲಿಗಳು_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		(ಶೂನ್ಯ)op_delete(oap);
		// save cursor line for undo if it wasn't saved yet
		ಆದರೆ (oap->motion_type == MLINE && has_format_option(FO_AUTO)
						      && u_save_cursor() == OK)
		    auto_format(FALSE, TRUE);
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ OP_YANK:
	    ಆದರೆ (ಖಾಲಿಗಳು_region_error)
	    {
		ಆದರೆ (!gui_yank)
		{
		    vim_beep(BO_OPER);
		    CancelRedo();
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
		restore_lbr(lbr_saved);
#ಆದರೆ_ಅಂತ್ಯ
		oap->excl_tr_ws = cap->cmdchar == 'z';
		(ಶೂನ್ಯ)op_yank(oap, FALSE, !gui_yank);
	    }
	    check_cursor_ಲಂಬಸಾಲು();
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ OP_CHANGE:
	    VIsual_reselect = FALSE;	    // don't reselect now
	    ಆದರೆ (ಖಾಲಿಗಳು_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// This is a new edit command, not a restart.  Need to
		// remember it to make 'insertmode' work with mappings for
		// Visual mode.  But do this only once and not when typed and
		// 'insertmode' isn't set.
		ಆದರೆ (p_im || !ಕೀಲಿ_Typed)
		    restart_edit_save = restart_edit;
		ಇಲ್ಲದಿದ್ದರೆ
		    restart_edit_save = 0;
		restart_edit = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#ಆದರೆ_ಅಂತ್ಯ
		// trigger TextChangedI
		curbuf->b_ಕೊನೆಯ_changedtick_i = CHANGEDTICK(curbuf);

		ಆದರೆ (op_change(oap))	// will call edit()
		    cap->retval |= CA_COMMAND_BUSY;
		ಆದರೆ (restart_edit == 0)
		    restart_edit = restart_edit_save;
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ OP_FILTER:
	    ಆದರೆ (vim_strchr(p_cpo, CPO_FILTER) != ಖಾಲಿ)
		Append_ಗಮ್ಯ_Redobuff((char_u *)"!\r");  // use any last used !cmd
	    ಇಲ್ಲದಿದ್ದರೆ
		bangredo = TRUE;    // do_bang() will put cmd in redo buffer
	    // FALLTHROUGH

	ಸ್ಥಿತಿ OP_INDENT:
	ಸ್ಥಿತಿ OP_COLON:

	    // If 'equalprg' is empty, do the indenting internally.
	    ಆದರೆ (oap->op_type == OP_INDENT && *get_equalprg() == NUL)
	    {
		ಆದರೆ (curbuf->b_p_lisp)
		{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		    ಆದರೆ (use_indentexpr_for_lisp())
			op_reindent(oap, get_ಅಭಿವ್ಯಕ್ತಿ_indent);
		    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
			op_reindent(oap, get_lisp_indent);
		    ನಿಲ್ಲಿಸು;
		}
		op_reindent(oap,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
			*curbuf->b_p_inde != NUL ? get_ಅಭಿವ್ಯಕ್ತಿ_indent :
#ಆದರೆ_ಅಂತ್ಯ
			    get_c_indent);
		ನಿಲ್ಲಿಸು;
	    }

	    op_colon(oap);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ OP_TILDE:
	ಸ್ಥಿತಿ OP_ಊರ್ಧ್ವ:
	ಸ್ಥಿತಿ OP_ಅಧೋ:
	ಸ್ಥಿತಿ OP_ROT13:
	    ಆದರೆ (ಖಾಲಿಗಳು_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		op_tilde(oap);
	    check_cursor_ಲಂಬಸಾಲು();
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ OP_FORMAT:
#ಆದರೆ defined(FEAT_EVAL)
	    ಆದರೆ (*curbuf->b_p_fex != NUL)
		op_formatexpr(oap);	// use expression
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    {
		ಆದರೆ (*p_fp != NUL || *curbuf->b_p_fp != NUL)
		    op_colon(oap);		// use external command
		ಇಲ್ಲದಿದ್ದರೆ
		    op_format(oap, FALSE);	// use internal function
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ OP_FORMAT2:
	    op_format(oap, TRUE);	// use internal function
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ OP_FUNCTION:
	    {
		redo_VIsual_T   save_redo_VIsual = redo_VIsual;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#ಆದರೆ_ಅಂತ್ಯ
		// call 'operatorfunc'
		op_function(oap);

		// Restore the info for redoing Visual mode, the function may
		// invoke another operator and unintentionally change it.
		redo_VIsual = save_redo_VIsual;
		ನಿಲ್ಲಿಸು;
	    }

	ಸ್ಥಿತಿ OP_ಸೇರಿಸು:
	ಸ್ಥಿತಿ OP_APPEND:
	    VIsual_reselect = FALSE;	// don't reselect now
	    ಆದರೆ (ಖಾಲಿಗಳು_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// This is a new edit command, not a restart.  Need to
		// remember it to make 'insertmode' work with mappings for
		// Visual mode.  But do this only once.
		restart_edit_save = restart_edit;
		restart_edit = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#ಆದರೆ_ಅಂತ್ಯ
		// trigger TextChangedI
		curbuf->b_ಕೊನೆಯ_changedtick_i = CHANGEDTICK(curbuf);

		op_ಸೇರಿಸು(oap, cap->count1);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
		// Reset linebreak, so that formatting works correctly.
		(ಶೂನ್ಯ)reset_lbr();
#ಆದರೆ_ಅಂತ್ಯ

		// TODO: when inserting in several lines, should format all
		// the lines.
		auto_format(FALSE, TRUE);

		ಆದರೆ (restart_edit == 0)
		    restart_edit = restart_edit_save;
		ಇಲ್ಲದಿದ್ದರೆ
		    cap->retval |= CA_COMMAND_BUSY;
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ OP_REPLACE:
	    VIsual_reselect = FALSE;	// don't reselect now
	    ಆದರೆ (ಖಾಲಿಗಳು_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
		// Restore linebreak, so that when the user edits it looks as
		// before.
		restore_lbr(lbr_saved);
#ಆದರೆ_ಅಂತ್ಯ
		op_replace(oap, cap->nchar);
	    }
	    ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	ಸ್ಥಿತಿ OP_FOLD:
	    VIsual_reselect = FALSE;	// don't reselect now
	    foldCreate(oap->ಆರಂಭ.lnum, oap->ಕೊನೆ.lnum);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ OP_FOLDOPEN:
	ಸ್ಥಿತಿ OP_FOLDOPENREC:
	ಸ್ಥಿತಿ OP_FOLDCLOSE:
	ಸ್ಥಿತಿ OP_FOLDCLOSEREC:
	    VIsual_reselect = FALSE;	// don't reselect now
	    opFoldRange(oap->ಆರಂಭ.lnum, oap->ಕೊನೆ.lnum,
		    oap->op_type == OP_FOLDOPEN
					    || oap->op_type == OP_FOLDOPENREC,
		    oap->op_type == OP_FOLDOPENREC
					  || oap->op_type == OP_FOLDCLOSEREC,
					  oap->is_VIsual);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ OP_FOLDDEL:
	ಸ್ಥಿತಿ OP_FOLDDELREC:
	    VIsual_reselect = FALSE;	// don't reselect now
	    deleteFold(oap->ಆರಂಭ.lnum, oap->ಕೊನೆ.lnum,
			       oap->op_type == OP_FOLDDELREC, oap->is_VIsual);
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ OP_NR_ಕೂಡಿಸು:
	ಸ್ಥಿತಿ OP_NR_SUB:
	    ಆದರೆ (ಖಾಲಿಗಳು_region_error)
	    {
		vim_beep(BO_OPER);
		CancelRedo();
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		VIsual_ಸಕ್ರಿಯ = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
		restore_lbr(lbr_saved);
#ಆದರೆ_ಅಂತ್ಯ
		op_addsub(oap, cap->count1, redo_VIsual.rv_arg);
		VIsual_ಸಕ್ರಿಯ = FALSE;
	    }
	    check_cursor_ಲಂಬಸಾಲು();
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    clearopbeep(oap);
	}
	virtual_op = MAYBE;
	ಆದರೆ (!gui_yank)
	{
	    // if 'sol' not set, go back to old column for some commands
	    ಆದರೆ (!p_sol && oap->motion_type == MLINE && !oap->ಕೊನೆ_adjusted
		    && (oap->op_type == OP_LSHIFT || oap->op_type == OP_RSHIFT
						|| oap->op_type == OP_DELETE))
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
		(ಶೂನ್ಯ)reset_lbr();
#ಆದರೆ_ಅಂತ್ಯ
		coladvance(curwin->w_curswant = old_ಲಂಬಸಾಲು);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    curwin->w_cursor = old_cursor;
	}
	oap->ಖಂಡ_ಬಹುಲಕ = FALSE;
	clearop(oap);
	motion_force = NUL;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    restore_lbr(lbr_saved);
#ಆದರೆ_ಅಂತ್ಯ
}

// put byte 'c' at position 'lp', but
// verify, that the position to place
// is actually safe
    ಸ್ಥಿರ ಶೂನ್ಯ
pbyte(pos_T lp, ಪೂರ್ಣಾಂಕ c)
{
    char_u *p = ml_get_buf(curbuf, lp.lnum, TRUE);
    ಪೂರ್ಣಾಂಕ	len = curbuf->b_ml.ml_ಸಾಲು_len;

    // safety check
    ಆದರೆ (lp.ಲಂಬಸಾಲು >= len)
	lp.ಲಂಬಸಾಲು = (len > 1 ? len - 2 : 0);
    *(p + lp.ಲಂಬಸಾಲು) = c;
}
