/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * indent.c: Indentation related functions
 */

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_VARTABS)

/*
 * Set the integer values corresponding to the string setting of 'vartabstop'.
 * "array" will be set, caller must free it if needed.
 * Return FAIL for an error.
 */
    ಪೂರ್ಣಾಙ್ಕ
tabstop_set(char_u *var, ಪೂರ್ಣಾಙ್ಕ **array)
{
    ಪೂರ್ಣಾಙ್ಕ	    valcount = 1;
    ಪೂರ್ಣಾಙ್ಕ	    t;
    char_u  *cp;

    ಯದಿ (var[0] == NUL || (var[0] == '0' && var[1] == NUL))
    {
	*array = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    ಪ್ರತಿ (cp = var; *cp != NUL; ++cp)
    {
	ಯದಿ (cp == var || cp[-1] == ',')
	{
	    char_u *end;

	    ಯದಿ (strtol((ವರ್ಣ *)cp, (ವರ್ಣ **)&end, 10) <= 0)
	    {
		ಯದಿ (cp != end)
		    emsg(_(e_argument_must_be_positive));
		ಅನ್ಯಥಾ
		    semsg(_(e_invalid_argument_str), cp);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}

	ಯದಿ (VIM_ISDIGIT(*cp))
	    ಅನುವರ್ತ;
	ಯದಿ (cp[0] == ',' && cp > var && cp[-1] != ',' && cp[1] != NUL)
	{
	    ++valcount;
	    ಅನುವರ್ತ;
	}
	semsg(_(e_invalid_argument_str), var);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    *array = ALLOC_MULT(ಪೂರ್ಣಾಙ್ಕ, valcount + 1);
    ಯದಿ (*array == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    (*array)[0] = valcount;

    t = 1;
    ಪ್ರತಿ (cp = var; *cp != NUL;)
    {
	ಪೂರ್ಣಾಙ್ಕ n = atoi((ವರ್ಣ *)cp);

	// Catch negative values, overflow and ridiculous big values.
	ಯದಿ (n <= 0 || n > TABSTOP_MAX)
	{
	    semsg(_(e_invalid_argument_str), cp);
	    VIM_CLEAR(*array);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	(*array)[t++] = n;
	ಯಾವತ್ (*cp != NUL && *cp != ',')
	    ++cp;
	ಯದಿ (*cp != NUL)
	    ++cp;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Calculate the number of screen spaces a tab will occupy.
 * If "vts" is set then the tab widths are taken from that array,
 * otherwise the value of ts is used.
 */
    ಪೂರ್ಣಾಙ್ಕ
tabstop_padding(colnr_T col, ಪೂರ್ಣಾಙ್ಕ ts_arg, ಪೂರ್ಣಾಙ್ಕ *vts)
{
    ಪೂರ್ಣಾಙ್ಕ		ts = ts_arg == 0 ? 8 : ts_arg;
    ಪೂರ್ಣಾಙ್ಕ		tabcount;
    colnr_T	tabcol = 0;
    ಪೂರ್ಣಾಙ್ಕ		t;
    ಪೂರ್ಣಾಙ್ಕ		padding = 0;

    ಯದಿ (vts == ರಿಕ್ತ || vts[0] == 0)
	ಪ್ರತ್ಯಾಗಮ ts - (col % ts);

    tabcount = vts[0];

    ಪ್ರತಿ (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	ಯದಿ (tabcol > col)
	{
	    padding = (ಪೂರ್ಣಾಙ್ಕ)(tabcol - col);
	    ಭಙ್ಗ;
	}
    }
    ಯದಿ (t > tabcount)
	padding = vts[tabcount] - (ಪೂರ್ಣಾಙ್ಕ)((col - tabcol) % vts[tabcount]);

    ಪ್ರತ್ಯಾಗಮ padding;
}

/*
 * Find the size of the tab that covers a particular column.
 *
 * If this is being called as part of a shift operation, col is not the cursor
 * column but is the column number to the left of the first non-whitespace
 * character in the line.  If the shift is to the left (left = TRUE), then
 * return the size of the tab interval to the left of the column.
 */
    ಪೂರ್ಣಾಙ್ಕ
tabstop_at(colnr_T col, ಪೂರ್ಣಾಙ್ಕ ts, ಪೂರ್ಣಾಙ್ಕ *vts, ಪೂರ್ಣಾಙ್ಕ ಎಡ)
{
    ಪೂರ್ಣಾಙ್ಕ		tabcount;	// Number of tab stops in the list of variable
				// tab stops.
    colnr_T	tabcol = 0;	// Column of the tab stop under consideration.
    ಪೂರ್ಣಾಙ್ಕ		t;		// Tabstop index in the list of variable tab
				// stops.
    ಪೂರ್ಣಾಙ್ಕ		tab_size = 0;	// Size of the tab stop interval to the right
				// or left of the col.

    ಯದಿ (vts == 0 || vts[0] == 0)
	ಪ್ರತ್ಯಾಗಮ ts;

    tabcount = vts[0];
    ಪ್ರತಿ (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	ಯದಿ (tabcol > col)
	{
	    // If shifting left (left != 0), and if the column to the left of
	    // the first first non-blank character (col) in the line is
	    // already to the left of the first tabstop, set the shift amount
	    // (tab_size) to just enough to shift the line to the left margin.
	    // The value doesn't seem to matter as long as it is at least that
	    // distance.
	    ಯದಿ (ಎಡ && (t == 1))
		tab_size = col;
	    ಅನ್ಯಥಾ
		tab_size = vts[t - (ಎಡ ? 1 : 0)];
	    ಭಙ್ಗ;
	}
    }
    ಯದಿ (t > tabcount)		// If the value of the index t is beyond the
				// end of the list, use the tab stop value at
				// the end of the list.
	tab_size = vts[tabcount];

    ಪ್ರತ್ಯಾಗಮ tab_size;
}

/*
 * Find the column on which a tab starts.
 */
    colnr_T
tabstop_start(colnr_T col, ಪೂರ್ಣಾಙ್ಕ ts, ಪೂರ್ಣಾಙ್ಕ *vts)
{
    ಪೂರ್ಣಾಙ್ಕ		tabcount;
    colnr_T	tabcol = 0;
    ಪೂರ್ಣಾಙ್ಕ		t;
    ಪೂರ್ಣಾಙ್ಕ		excess;

    ಯದಿ (vts == ರಿಕ್ತ || vts[0] == 0)
	ಪ್ರತ್ಯಾಗಮ col - col % ts;

    tabcount = vts[0];
    ಪ್ರತಿ (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	ಯದಿ (tabcol > col)
	    ಪ್ರತ್ಯಾಗಮ tabcol - vts[t];
    }

    excess = tabcol % vts[tabcount];
    ಪ್ರತ್ಯಾಗಮ col - (col - excess) % vts[tabcount];
}

/*
 * Find the number of tabs and spaces necessary to get from one column
 * to another.
 */
    ಶೂನ್ಯ
tabstop_fromto(
	colnr_T start_col,
	colnr_T end_col,
	ಪೂರ್ಣಾಙ್ಕ	ts_arg,
	ಪೂರ್ಣಾಙ್ಕ	*vts,
	ಪೂರ್ಣಾಙ್ಕ	*ntabs,
	ಪೂರ್ಣಾಙ್ಕ	*nspcs)
{
    ಪೂರ್ಣಾಙ್ಕ		spaces = end_col - start_col;
    colnr_T	tabcol = 0;
    ಪೂರ್ಣಾಙ್ಕ		padding = 0;
    ಪೂರ್ಣಾಙ್ಕ		tabcount;
    ಪೂರ್ಣಾಙ್ಕ		t;
    ಪೂರ್ಣಾಙ್ಕ		ts = ts_arg == 0 ? curbuf->b_p_ts : ts_arg;

    ಯದಿ (vts == ರಿಕ್ತ || vts[0] == 0)
    {
	ಪೂರ್ಣಾಙ್ಕ tabs = 0;
	ಪೂರ್ಣಾಙ್ಕ initspc = 0;

	initspc = ts - (start_col % ts);
	ಯದಿ (spaces >= initspc)
	{
	    spaces -= initspc;
	    tabs++;
	}
	tabs += spaces / ts;
	spaces -= (spaces / ts) * ts;

	*ntabs = tabs;
	*nspcs = spaces;
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Find the padding needed to reach the next tabstop.
    tabcount = vts[0];
    ಪ್ರತಿ (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	ಯದಿ (tabcol > start_col)
	{
	    padding = (ಪೂರ್ಣಾಙ್ಕ)(tabcol - start_col);
	    ಭಙ್ಗ;
	}
    }
    ಯದಿ (t > tabcount)
	padding = vts[tabcount] - (ಪೂರ್ಣಾಙ್ಕ)((start_col - tabcol) % vts[tabcount]);

    // If the space needed is less than the padding no tabs can be used.
    ಯದಿ (spaces < padding)
    {
	*ntabs = 0;
	*nspcs = spaces;
	ಪ್ರತ್ಯಾಗಮ;
    }

    *ntabs = 1;
    spaces -= padding;

    // At least one tab has been used. See if any more will fit.
    ಯಾವತ್ (spaces != 0 && ++t <= tabcount)
    {
	padding = vts[t];
	ಯದಿ (spaces < padding)
	{
	    *nspcs = spaces;
	    ಪ್ರತ್ಯಾಗಮ;
	}
	++*ntabs;
	spaces -= padding;
    }

    *ntabs += spaces / vts[tabcount];
    *nspcs =  spaces % vts[tabcount];
}

/*
 * See if two tabstop arrays contain the same values.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
tabstop_eq(ಪೂರ್ಣಾಙ್ಕ *ts1, ಪೂರ್ಣಾಙ್ಕ *ts2)
{
    ಪೂರ್ಣಾಙ್ಕ		t;

    ಯದಿ ((ts1 == 0 && ts2) || (ts1 && ts2 == 0))
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಯದಿ (ts1 == ts2)
	ಪ್ರತ್ಯಾಗಮ TRUE;
    ಯದಿ (ts1[0] != ts2[0])
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಪ್ರತಿ (t = 1; t <= ts1[0]; ++t)
	ಯದಿ (ts1[t] != ts2[t])
	    ಪ್ರತ್ಯಾಗಮ FALSE;

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

# ಯದಿ defined(FEAT_BEVAL)
/*
 * Copy a tabstop array, allocating space for the new array.
 */
    ಪೂರ್ಣಾಙ್ಕ *
tabstop_copy(ಪೂರ್ಣಾಙ್ಕ *oldts)
{
    ಪೂರ್ಣಾಙ್ಕ		*newts;
    ಪೂರ್ಣಾಙ್ಕ		t;

    ಯದಿ (oldts == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    newts = ALLOC_MULT(ಪೂರ್ಣಾಙ್ಕ, oldts[0] + 1);
    ಯದಿ (newts != ರಿಕ್ತ)
	ಪ್ರತಿ (t = 0; t <= oldts[0]; ++t)
	    newts[t] = oldts[t];
    ಪ್ರತ್ಯಾಗಮ newts;
}
# endif

/*
 * Return a count of the number of tabstops.
 */
    ಪೂರ್ಣಾಙ್ಕ
tabstop_ಎಣಿಕೆ(ಪೂರ್ಣಾಙ್ಕ *ts)
{
    ಪ್ರತ್ಯಾಗಮ ts != ರಿಕ್ತ ? ts[0] : 0;
}

/*
 * Return the first tabstop, or 8 if there are no tabstops defined.
 */
    ಪೂರ್ಣಾಙ್ಕ
tabstop_first(ಪೂರ್ಣಾಙ್ಕ *ts)
{
    ಪ್ರತ್ಯಾಗಮ ts != ರಿಕ್ತ ? ts[1] : 8;
}

#endif

/*
 * Return the effective shiftwidth value for current buffer, using the
 * 'tabstop' value when 'shiftwidth' is zero.
 */
    long
get_sw_ಮೌಲ್ಯ(buf_T *buf)
{
    ಪ್ರತ್ಯಾಗಮ get_sw_ಮೌಲ್ಯ_col(buf, 0, FALSE);
}

/*
 * Idem, using "pos".
 */
    ಸ್ಥಿರ long
get_sw_ಮೌಲ್ಯ_pos(buf_T *buf, pos_T *pos, ಪೂರ್ಣಾಙ್ಕ ಎಡ)
{
    pos_T save_cursor = curwin->w_cursor;
    long sw_ಮೌಲ್ಯ;

    curwin->w_cursor = *pos;
    sw_ಮೌಲ್ಯ = get_sw_ಮೌಲ್ಯ_col(buf, get_nolist_virtcol(), ಎಡ);
    curwin->w_cursor = save_cursor;
    ಪ್ರತ್ಯಾಗಮ sw_ಮೌಲ್ಯ;
}

/*
 * Idem, using the first non-black in the current line.
 */
    long
get_sw_ಮೌಲ್ಯ_indent(buf_T *buf, ಪೂರ್ಣಾಙ್ಕ ಎಡ)
{
    pos_T pos = curwin->w_cursor;

    pos.col = getwhitecols_curline();
    ಪ್ರತ್ಯಾಗಮ get_sw_ಮೌಲ್ಯ_pos(buf, &pos, ಎಡ);
}

/*
 * Idem, using virtual column "col".
 */
    long
get_sw_ಮೌಲ್ಯ_col(buf_T *buf, colnr_T col UNUSED, ಪೂರ್ಣಾಙ್ಕ ಎಡ UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ buf->b_p_sw ? buf->b_p_sw :
#ifdef FEAT_VARTABS
	tabstop_at(col, buf->b_p_ts, buf->b_p_vts_array, ಎಡ);
#ಅನ್ಯಥಾ
	buf->b_p_ts;
#endif
}

/*
 * Return the effective softtabstop value for the current buffer, using the
 * 'shiftwidth' value when 'softtabstop' is negative.
 */
    long
get_sts_ಮೌಲ್ಯ(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ curbuf->b_p_sts < 0 ? get_sw_ಮೌಲ್ಯ(curbuf) : curbuf->b_p_sts;
}

/*
 * Count the size (in window cells) of the indent in the current line.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_indent(ಶೂನ್ಯ)
{
#ifdef FEAT_VARTABS
    ಪ್ರತ್ಯಾಗಮ get_indent_str_vtab(ml_get_curline(), (ಪೂರ್ಣಾಙ್ಕ)curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ get_indent_str(ml_get_curline(), (ಪೂರ್ಣಾಙ್ಕ)curbuf->b_p_ts, FALSE);
#endif
}

/*
 * Count the size (in window cells) of the indent in line "lnum".
 */
    ಪೂರ್ಣಾಙ್ಕ
get_indent_lnum(linenr_T lnum)
{
#ifdef FEAT_VARTABS
    ಪ್ರತ್ಯಾಗಮ get_indent_str_vtab(ml_get(lnum), (ಪೂರ್ಣಾಙ್ಕ)curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ get_indent_str(ml_get(lnum), (ಪೂರ್ಣಾಙ್ಕ)curbuf->b_p_ts, FALSE);
#endif
}

#ಯದಿ defined(FEAT_FOLDING)
/*
 * Count the size (in window cells) of the indent in line "lnum" of buffer
 * "buf".
 */
    ಪೂರ್ಣಾಙ್ಕ
get_indent_buf(buf_T *buf, linenr_T lnum)
{
# ifdef FEAT_VARTABS
    ಪ್ರತ್ಯಾಗಮ get_indent_str_vtab(ml_get_buf(buf, lnum, FALSE),
			       (ಪೂರ್ಣಾಙ್ಕ)buf->b_p_ts, buf->b_p_vts_array, FALSE);
# ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ get_indent_str(ml_get_buf(buf, lnum, FALSE), (ಪೂರ್ಣಾಙ್ಕ)buf->b_p_ts, FALSE);
# endif
}
#endif

/*
 * count the size (in window cells) of the indent in line "ptr", with
 * 'tabstop' at "ts"
 */
    ಪೂರ್ಣಾಙ್ಕ
get_indent_str(
    char_u	*ptr,
    ಪೂರ್ಣಾಙ್ಕ		ts,
    ಪೂರ್ಣಾಙ್ಕ		no_ts) // if TRUE, count a tab as ^I
{
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ = 0;

    ಪ್ರತಿ ( ; *ptr; ++ptr)
    {
	ಯದಿ (*ptr == TAB)    // count a tab for what it is worth
	{
	    ಯದಿ (!no_ts)
		ಎಣಿಕೆ += ts - (ಎಣಿಕೆ % ts);
	    ಅನ್ಯಥಾ
		ಎಣಿಕೆ += ptr2cells(ptr);
	}
	ಅನ್ಯಥಾ ಯದಿ (*ptr == ' ')
	    ++ಎಣಿಕೆ;		// count a space for one
	ಅನ್ಯಥಾ
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ ಎಣಿಕೆ;
}

#ಯದಿ defined(FEAT_VARTABS)
/*
 * Count the size (in window cells) of the indent in line "ptr", using
 * variable tabstops.
 * If "no_ts" is TRUE, count a tab as ^I.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_indent_str_vtab(char_u *ptr, ಪೂರ್ಣಾಙ್ಕ ts, ಪೂರ್ಣಾಙ್ಕ *vts, ಪೂರ್ಣಾಙ್ಕ no_ts)
{
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ = 0;

    ಪ್ರತಿ ( ; *ptr; ++ptr)
    {
	ಯದಿ (*ptr == TAB)    // count a tab for what it is worth
	{
	    ಯದಿ (!no_ts)
		ಎಣಿಕೆ += tabstop_padding(ಎಣಿಕೆ, ts, vts);
	    ಅನ್ಯಥಾ
		ಎಣಿಕೆ += ptr2cells(ptr);
	}
	ಅನ್ಯಥಾ ಯದಿ (*ptr == ' ')
	    ++ಎಣಿಕೆ;		// count a space for one
	ಅನ್ಯಥಾ
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ ಎಣಿಕೆ;
}
#endif

/*
 * Set the indent of the current line.
 * Leaves the cursor on the first non-blank in the line.
 * Caller must take care of undo.
 * "flags":
 *	SIN_CHANGED:	call changed_bytes() if the line was changed.
 *	SIN_INSERT:	insert the indent in front of the line.
 *	SIN_UNDO:	save line for undo before changing it.
 * Returns TRUE if the line was changed.
 */
    ಪೂರ್ಣಾಙ್ಕ
set_indent(
    ಪೂರ್ಣಾಙ್ಕ		size,		    // measured in spaces
    ಪೂರ್ಣಾಙ್ಕ		flags)
{
    char_u	*p;
    char_u	*newline;
    char_u	*oldline;
    char_u	*s;
    ಪೂರ್ಣಾಙ್ಕ		todo;
    ಪೂರ್ಣಾಙ್ಕ		ind_len;	    // measured in characters
    ಪೂರ್ಣಾಙ್ಕ		ಸಾಲು_len;	    // size of the line (including the NUL)
    ಪೂರ್ಣಾಙ್ಕ		doit = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		ind_done = 0;	    // measured in spaces
#ifdef FEAT_VARTABS
    ಪೂರ್ಣಾಙ್ಕ		ind_col = 0;
#endif
    ಪೂರ್ಣಾಙ್ಕ		tab_pad;
    ಪೂರ್ಣಾಙ್ಕ		retval = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		orig_char_len = -1; // number of initial whitespace chars when
				    // 'et' and 'pi' are both set

    // First check if there is anything to do and compute the number of
    // characters needed for the indent.
    todo = size;
    ind_len = 0;
    p = oldline = ml_get_curline();
    ಸಾಲು_len = ml_get_curline_len() + 1;

    // Calculate the buffer size for the new indent, and check to see if it
    // isn't already set

    // if 'expandtab' isn't set: use TABs; if both 'expandtab' and
    // 'preserveindent' are set count the number of characters at the
    // beginning of the line to be copied
    ಯದಿ (!curbuf->b_p_et || (!(flags & SIN_ಸೇರಿಸು) && curbuf->b_p_pi))
    {
	// If 'preserveindent' is set then reuse as much as possible of
	// the existing indent structure for the new indent
	ಯದಿ (!(flags & SIN_ಸೇರಿಸು) && curbuf->b_p_pi)
	{
	    ind_done = 0;

	    // count as many characters as we can use
	    ಯಾವತ್ (todo > 0 && VIM_ISWHITE(*p))
	    {
		ಯದಿ (*p == TAB)
		{
#ifdef FEAT_VARTABS
		    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#ಅನ್ಯಥಾ
		    tab_pad = (ಪೂರ್ಣಾಙ್ಕ)curbuf->b_p_ts
					   - (ind_done % (ಪೂರ್ಣಾಙ್ಕ)curbuf->b_p_ts);
#endif
		    // stop if this tab will overshoot the target
		    ಯದಿ (todo < tab_pad)
			ಭಙ್ಗ;
		    todo -= tab_pad;
		    ++ind_len;
		    ind_done += tab_pad;
		}
		ಅನ್ಯಥಾ
		{
		    --todo;
		    ++ind_len;
		    ++ind_done;
		}
		++p;
	    }

#ifdef FEAT_VARTABS
	    // These diverge from this point.
	    ind_col = ind_done;
#endif
	    // Set initial number of whitespace chars to copy if we are
	    // preserving indent but expandtab is set
	    ಯದಿ (curbuf->b_p_et)
		orig_char_len = ind_len;

	    // Fill to next tabstop with a tab, if possible
#ifdef FEAT_VARTABS
	    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
						curbuf->b_p_vts_array);
#ಅನ್ಯಥಾ
	    tab_pad = (ಪೂರ್ಣಾಙ್ಕ)curbuf->b_p_ts - (ind_done % (ಪೂರ್ಣಾಙ್ಕ)curbuf->b_p_ts);
#endif
	    ಯದಿ (todo >= tab_pad && orig_char_len == -1)
	    {
		doit = TRUE;
		todo -= tab_pad;
		++ind_len;
		// ind_done += tab_pad;
#ifdef FEAT_VARTABS
		ind_col += tab_pad;
#endif
	    }
	}

	// count tabs required for indent
#ifdef FEAT_VARTABS
	ಪ್ರತಿ (;;)
	{
	    tab_pad = tabstop_padding(ind_col, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
	    ಯದಿ (todo < tab_pad)
		ಭಙ್ಗ;
	    ಯದಿ (*p != TAB)
		doit = TRUE;
	    ಅನ್ಯಥಾ
		++p;
	    todo -= tab_pad;
	    ++ind_len;
	    ind_col += tab_pad;
	}
#ಅನ್ಯಥಾ
	ಯಾವತ್ (todo >= (ಪೂರ್ಣಾಙ್ಕ)curbuf->b_p_ts)
	{
	    ಯದಿ (*p != TAB)
		doit = TRUE;
	    ಅನ್ಯಥಾ
		++p;
	    todo -= (ಪೂರ್ಣಾಙ್ಕ)curbuf->b_p_ts;
	    ++ind_len;
	    // ind_done += (int)curbuf->b_p_ts;
	}
#endif
    }
    // count spaces required for indent
    ಯಾವತ್ (todo > 0)
    {
	ಯದಿ (*p != ' ')
	    doit = TRUE;
	ಅನ್ಯಥಾ
	    ++p;
	--todo;
	++ind_len;
	// ++ind_done;
    }

    // Return if the indent is OK already.
    ಯದಿ (!doit && !VIM_ISWHITE(*p) && !(flags & SIN_ಸೇರಿಸು))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // Allocate memory for the new line.
    ಯದಿ (flags & SIN_ಸೇರಿಸು)
	p = oldline;
    ಅನ್ಯಥಾ
    {
	p = skipwhite(p);
	ಸಾಲು_len -= (ಪೂರ್ಣಾಙ್ಕ)(p - oldline);
    }

    // If 'preserveindent' and 'expandtab' are both set keep the original
    // characters and allocate accordingly.  We will fill the rest with spaces
    // after the if (!curbuf->b_p_et) below.
    ಯದಿ (orig_char_len != -1)
    {
	newline = alloc(orig_char_len + size - ind_done + ಸಾಲು_len);
	ಯದಿ (newline == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	todo = size - ind_done;
	ind_len = orig_char_len + todo;    // Set total length of indent in
					   // characters, which may have been
					   // undercounted until now
	p = oldline;
	s = newline;
	ಯಾವತ್ (orig_char_len > 0)
	{
	    *s++ = *p++;
	    orig_char_len--;
	}

	// Skip over any additional white space (useful when newindent is less
	// than old)
	ಯಾವತ್ (VIM_ISWHITE(*p))
	    ++p;

    }
    ಅನ್ಯಥಾ
    {
	todo = size;
	newline = alloc(ind_len + ಸಾಲು_len);
	ಯದಿ (newline == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	s = newline;
    }

    // Put the characters in the new line.
    // if 'expandtab' isn't set: use TABs
    ಯದಿ (!curbuf->b_p_et)
    {
	// If 'preserveindent' is set then reuse as much as possible of
	// the existing indent structure for the new indent
	ಯದಿ (!(flags & SIN_ಸೇರಿಸು) && curbuf->b_p_pi)
	{
	    p = oldline;
	    ind_done = 0;

	    ಯಾವತ್ (todo > 0 && VIM_ISWHITE(*p))
	    {
		ಯದಿ (*p == TAB)
		{
#ifdef FEAT_VARTABS
		    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#ಅನ್ಯಥಾ
		    tab_pad = (ಪೂರ್ಣಾಙ್ಕ)curbuf->b_p_ts
					   - (ind_done % (ಪೂರ್ಣಾಙ್ಕ)curbuf->b_p_ts);
#endif
		    // stop if this tab will overshoot the target
		    ಯದಿ (todo < tab_pad)
			ಭಙ್ಗ;
		    todo -= tab_pad;
		    ind_done += tab_pad;
		}
		ಅನ್ಯಥಾ
		{
		    --todo;
		    ++ind_done;
		}
		*s++ = *p++;
	    }

	    // Fill to next tabstop with a tab, if possible
#ifdef FEAT_VARTABS
	    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
						curbuf->b_p_vts_array);
#ಅನ್ಯಥಾ
	    tab_pad = (ಪೂರ್ಣಾಙ್ಕ)curbuf->b_p_ts - (ind_done % (ಪೂರ್ಣಾಙ್ಕ)curbuf->b_p_ts);
#endif
	    ಯದಿ (todo >= tab_pad)
	    {
		*s++ = TAB;
		todo -= tab_pad;
#ifdef FEAT_VARTABS
		ind_done += tab_pad;
#endif
	    }

	    p = skipwhite(p);
	}

#ifdef FEAT_VARTABS
	ಪ್ರತಿ (;;)
	{
	    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
	    ಯದಿ (todo < tab_pad)
		ಭಙ್ಗ;
	    *s++ = TAB;
	    todo -= tab_pad;
	    ind_done += tab_pad;
	}
#ಅನ್ಯಥಾ
	ಯಾವತ್ (todo >= (ಪೂರ್ಣಾಙ್ಕ)curbuf->b_p_ts)
	{
	    *s++ = TAB;
	    todo -= (ಪೂರ್ಣಾಙ್ಕ)curbuf->b_p_ts;
	}
#endif
    }
    ಯಾವತ್ (todo > 0)
    {
	*s++ = ' ';
	--todo;
    }
    mch_memmove(s, p, (size_t)ಸಾಲು_len);

    // Replace the line (unless undo fails).
    ಯದಿ (!(flags & SIN_UNDO) || u_savesub(curwin->w_cursor.lnum) == OK)
    {
	colnr_T old_offset = (colnr_T)(p - oldline);
	colnr_T new_offset = (colnr_T)(s - newline);

	// this may free "newline"
	ml_replace(curwin->w_cursor.lnum, newline, FALSE);
	ಯದಿ (flags & SIN_CHANGED)
	    changed_bytes(curwin->w_cursor.lnum, 0);

	// Correct saved cursor position if it is in this line.
	ಯದಿ (saved_cursor.lnum == curwin->w_cursor.lnum)
	{
	    ಯದಿ (saved_cursor.col >= old_offset)
		// cursor was after the indent, adjust for the number of
		// bytes added/removed
		saved_cursor.col += ind_len - old_offset;
	    ಅನ್ಯಥಾ ಯದಿ (saved_cursor.col >= new_offset)
		// cursor was in the indent, and is now after it, put it back
		// at the start of the indent (replacing spaces with TAB)
		saved_cursor.col = new_offset;
	}
#ifdef FEAT_PROP_POPUP
	ಯದಿ (curbuf->b_has_textprop)
	{
	    ಪೂರ್ಣಾಙ್ಕ added = ind_len - old_offset;

	    // When increasing indent this behaves like spaces were inserted at
	    // the old indent, when decreasing indent it behaves like spaces
	    // were deleted at the new indent.
	    adjust_prop_columns(curwin->w_cursor.lnum,
				  added > 0 ? old_offset : (colnr_T)ind_len,
				  added, APC_INDENT);
	}
#endif
	retval = TRUE;
    }
    ಅನ್ಯಥಾ
	vim_free(newline);

    curwin->w_cursor.col = ind_len;
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Return the indent of the current line after a number.  Return -1 if no
 * number was found.  Used for 'n' in 'formatoptions': numbered list.
 * Since a pattern is used it can actually handle more than numbers.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_ಸಂಖ್ಯೆ_indent(linenr_T lnum)
{
    colnr_T	col;
    pos_T	pos;

    regmatch_T	regmatch;
    ಪೂರ್ಣಾಙ್ಕ		lead_len = 0;	// length of comment leader

    ಯದಿ (lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಪ್ರತ್ಯಾಗಮ -1;
    pos.lnum = 0;

    // In format_lines() (i.e. not insert mode), fo+=q is needed too...
    ಯದಿ ((State & MODE_ಸೇರಿಸು) || has_format_option(FO_Q_COMS))
	lead_len = get_leader_len(ml_get(lnum), ರಿಕ್ತ, FALSE, TRUE);

    regmatch.regprog = vim_regcomp(curbuf->b_p_flp, RE_MAGIC);
    ಯದಿ (regmatch.regprog != ರಿಕ್ತ)
    {
	regmatch.rm_ic = FALSE;

	// vim_regexec() expects a pointer to a line.  This lets us
	// start matching for the flp beyond any comment leader...
	ಯದಿ (vim_regexec(&regmatch, ml_get(lnum) + lead_len, (colnr_T)0))
	{
	    pos.lnum = lnum;
	    pos.col = (colnr_T)(*regmatch.endp - ml_get(lnum));
	    pos.coladd = 0;
	}
	vim_regfree(regmatch.regprog);
    }

    ಯದಿ (pos.lnum == 0 || *ml_get_pos(&pos) == NUL)
	ಪ್ರತ್ಯಾಗಮ -1;
    getvcol(curwin, &pos, &col, ರಿಕ್ತ, ರಿಕ್ತ, 0);
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)col;
}

#ಯದಿ defined(FEAT_LINEBREAK)
/*
 * Check "briopt" as 'breakindentopt' and update the members of "wp".
 * This is called when 'breakindentopt' is changed and when a window is
 * initialized.
 * Returns FAIL for failure, OK otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
briopt_check(
    char_u	*briopt,	// when NULL: use "wp->w_p_briopt"
    win_T	*wp)		// when NULL: only check "briopt"
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		bri_shift = 0;
    long	bri_min = 20;
    ಪೂರ್ಣಾಙ್ಕ		bri_sbr = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		bri_list = 0;
    ಪೂರ್ಣಾಙ್ಕ		bri_vcol = 0;

    ಯದಿ (briopt != ರಿಕ್ತ)
	p = briopt;
    ಅನ್ಯಥಾ
	p = wp->w_p_briopt;

    ಯಾವತ್ (*p != NUL)
    {
	// Note: Keep this in sync with p_briopt_values
	ಯದಿ (STRNCMP(p, "shift:", 6) == 0
		 && ((p[6] == '-' && VIM_ISDIGIT(p[7])) || VIM_ISDIGIT(p[6])))
	{
	    p += 6;
	    bri_shift = getdigits(&p);
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "min:", 4) == 0 && VIM_ISDIGIT(p[4]))
	{
	    p += 4;
	    bri_min = getdigits(&p);
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "sbr", 3) == 0)
	{
	    p += 3;
	    bri_sbr = TRUE;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "list:", 5) == 0)
	{
	    p += 5;
	    bri_list = getdigits(&p);
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "column:", 7) == 0)
	{
	    p += 7;
	    bri_vcol = getdigits(&p);
	}
	ಯದಿ (*p != ',' && *p != NUL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಯದಿ (*p == ',')
	    ++p;
    }

    ಯದಿ (wp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ OK;

    wp->w_briopt_shift = bri_shift;
    wp->w_briopt_min   = bri_min;
    wp->w_briopt_sbr   = bri_sbr;
    wp->w_briopt_list  = bri_list;
    wp->w_briopt_vcol  = bri_vcol;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Return appropriate space number for breakindent, taking influencing
 * parameters into account. Window must be specified, since it is not
 * necessarily always the current one.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_breakindent_win(
    win_T	*wp,
    char_u	*ಸಾಲು) // start of the line
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    prev_indent = 0;	// cached indent value
    ಸ್ಥಿರ long	    prev_ts     = 0L;	// cached tabstop value
# ifdef FEAT_VARTABS
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ      *prev_vts = ರಿಕ್ತ;   // cached vartabs values
# endif
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    prev_fnum   = 0;	// cached buffer number
    ಸ್ಥಿರ char_u   *prev_ಸಾಲು  = ರಿಕ್ತ;	// cached copy of "line"
    ಸ್ಥಿರ varnumber_T prev_tick = 0;   // changedtick of cached value
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ      prev_list = 0;	// cached list indent
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ      prev_listopt = 0;	// cached w_p_briopt_list value
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ      prev_no_ts = FALSE;	// cached no_ts value
    ಸ್ಥಿರ unsigned prev_dy_uhex = 0;	// cached 'display' "uhex" value
    // cached formatlistpat value
    ಸ್ಥಿರ char_u   *prev_flp = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		    bri = 0;
    // window width minus window margin space, i.e. what rests for text
    ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ	    eff_wwidth = wp->w_width
					  - win_col_off(wp) + win_col_off2(wp);

    // In list mode, if 'listchars' "tab" isn't set, a TAB is displayed as ^I.
    ಪೂರ್ಣಾಙ್ಕ no_ts = wp->w_p_list && wp->w_lcs_chars.tab1 == NUL;

    // Used cached indent, unless
    // - buffer changed, or
    // - 'tabstop' changed, or
    // - 'vartabstop' changed, or
    // - buffer was changed, or
    // - 'breakindentopt' "list" changed, or
    // - 'list' or 'listchars' "tab" changed, or
    // - 'display' "uhex" flag changed, or
    // - 'formatlistpat' changed, or
    // - line changed.
    ಯದಿ (prev_fnum != wp->w_buffer->b_fnum
	    || prev_ts != wp->w_buffer->b_p_ts
# ifdef FEAT_VARTABS
	    || prev_vts != wp->w_buffer->b_p_vts_array
# endif
	    || prev_tick != CHANGEDTICK(wp->w_buffer)
	    || prev_listopt != wp->w_briopt_list
	    || prev_no_ts != no_ts
	    || prev_dy_uhex != (dy_flags & DY_UHEX)
	    || prev_flp == ರಿಕ್ತ
	    || STRCMP(prev_flp, get_flp_ಮೌಲ್ಯ(wp->w_buffer)) != 0
	    || prev_ಸಾಲು == ರಿಕ್ತ || STRCMP(prev_ಸಾಲು, ಸಾಲು) != 0
	)
    {
	prev_fnum = wp->w_buffer->b_fnum;
	vim_free(prev_ಸಾಲು);
	prev_ಸಾಲು = vim_strsave(ಸಾಲು);
	prev_ts = wp->w_buffer->b_p_ts;
# ifdef FEAT_VARTABS
	prev_vts = wp->w_buffer->b_p_vts_array;
	ಯದಿ (wp->w_briopt_vcol == 0)
	    prev_indent = get_indent_str_vtab(ಸಾಲು,
				     (ಪೂರ್ಣಾಙ್ಕ)wp->w_buffer->b_p_ts,
				      wp->w_buffer->b_p_vts_array, no_ts);
# ಅನ್ಯಥಾ
	ಯದಿ (wp->w_briopt_vcol == 0)
	    prev_indent = get_indent_str(ಸಾಲು,
					(ಪೂರ್ಣಾಙ್ಕ)wp->w_buffer->b_p_ts, no_ts);
# endif
	prev_tick = CHANGEDTICK(wp->w_buffer);
	prev_listopt = wp->w_briopt_list;
	prev_list = 0;
	prev_no_ts = no_ts;
	prev_dy_uhex = (dy_flags & DY_UHEX);
	vim_free(prev_flp);
	prev_flp = vim_strsave(get_flp_ಮೌಲ್ಯ(wp->w_buffer));
	// add additional indent for numbered lists
	ಯದಿ (wp->w_briopt_list != 0 && wp->w_briopt_vcol == 0)
	{
	    regmatch_T	    regmatch;

	    regmatch.regprog = vim_regcomp(prev_flp,
				   RE_MAGIC + RE_STRING + RE_AUTO + RE_STRICT);

	    ಯದಿ (regmatch.regprog != ರಿಕ್ತ)
	    {
		regmatch.rm_ic = FALSE;
		ಯದಿ (vim_regexec(&regmatch, ಸಾಲು, 0))
		{
		    ಯದಿ (wp->w_briopt_list > 0)
			prev_list = wp->w_briopt_list;
		    ಅನ್ಯಥಾ
		    {
			char_u	*ptr = *regmatch.startp;
			char_u	*end_ptr = *regmatch.endp;
			ಪೂರ್ಣಾಙ್ಕ	indent = 0;

			// Compute the width of the matched text.
			// Use win_chartabsize() so that TAB size is correct,
			// while wrapping is ignored.
			ಯಾವತ್ (ptr < end_ptr)
			{
			    indent += win_chartabsize(wp, ptr, indent);
			    MB_PTR_ADV(ptr);
			}
			prev_indent = indent;
		    }
		}
		vim_regfree(regmatch.regprog);
	    }
	}
    }
    ಯದಿ (wp->w_briopt_vcol != 0)
    {
	// column value has priority
	bri = wp->w_briopt_vcol;
	prev_list = 0;
    }
    ಅನ್ಯಥಾ
	bri = prev_indent + wp->w_briopt_shift;

    // Add offset for number column, if 'n' is in 'cpoptions'
    bri += win_col_off2(wp);

    // add additional indent for numbered lists
    ಯದಿ (wp->w_briopt_list > 0)
	bri += prev_list;

    // indent minus the length of the showbreak string
    ಯದಿ (wp->w_briopt_sbr)
	bri -= vim_strsize(get_showbreak_ಮೌಲ್ಯ(wp));


    // never indent past left window margin
    ಯದಿ (bri < 0)
	bri = 0;

    // always leave at least bri_min characters on the left,
    // if text width is sufficient
    ಅನ್ಯಥಾ ಯದಿ (bri > eff_wwidth - wp->w_briopt_min)
	bri = (eff_wwidth - wp->w_briopt_min < 0)
					   ? 0 : eff_wwidth - wp->w_briopt_min;

    ಪ್ರತ್ಯಾಗಮ bri;
}
#endif

/*
 * When extra == 0: Return TRUE if the cursor is before or on the first
 *		    non-blank in the line.
 * When extra == 1: Return TRUE if the cursor is before the first non-blank in
 *		    the line.
 */
    ಪೂರ್ಣಾಙ್ಕ
inindent(ಪೂರ್ಣಾಙ್ಕ extra)
{
    char_u	*ptr;
    colnr_T	col;

    ಪ್ರತಿ (col = 0, ptr = ml_get_curline(); VIM_ISWHITE(*ptr); ++col)
	++ptr;
    ಯದಿ (col >= curwin->w_cursor.col + extra)
	ಪ್ರತ್ಯಾಗಮ TRUE;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * op_reindent - handle reindenting a block of lines.
 */
    ಶೂನ್ಯ
op_reindent(oparg_T *oap, ಪೂರ್ಣಾಙ್ಕ (*how)(ಶೂನ್ಯ))
{
    long	i = 0;
    char_u	*l;
    ಪೂರ್ಣಾಙ್ಕ		amount;
    linenr_T	first_changed = 0;
    linenr_T	last_changed = 0;
    linenr_T	start_lnum = curwin->w_cursor.lnum;

    // Don't even try when 'modifiable' is off.
    ಯದಿ (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Save for undo.  Do this once for all lines, much faster than doing this
    // for each line separately, especially when undoing.
    ಯದಿ (u_savecommon(start_lnum - 1, start_lnum + oap->ಸಾಲು_ಎಣಿಕೆ,
				     start_lnum + oap->ಸಾಲು_ಎಣಿಕೆ, FALSE) == OK)
	ಪ್ರತಿ (i = oap->ಸಾಲು_ಎಣಿಕೆ; --i >= 0 && !got_int; )
	{
	    // it's a slow thing to do, so give feedback so there's no worry
	    // that the computer's just hung.

	    ಯದಿ (i > 1
		    && (i % 50 == 0 || i == oap->ಸಾಲು_ಎಣಿಕೆ - 1)
		    && oap->ಸಾಲು_ಎಣಿಕೆ > p_report)
		smsg(_("%ld lines to indent... "), i);

	    // Be vi-compatible: For lisp indenting the first line is not
	    // indented, unless there is only one line.
	    ಯದಿ (i != oap->ಸಾಲು_ಎಣಿಕೆ - 1 || oap->ಸಾಲು_ಎಣಿಕೆ == 1
						     || how != get_lisp_indent)
	    {
		l = skipwhite(ml_get_curline());
		ಯದಿ (*l == NUL)		    // empty or blank line
		    amount = 0;
		ಅನ್ಯಥಾ
		    amount = how();	    // get the indent for this line

		ಯದಿ (amount >= 0 && set_indent(amount, 0))
		{
		    // did change the indent, call changed_lines() later
		    ಯದಿ (first_changed == 0)
			first_changed = curwin->w_cursor.lnum;
		    last_changed = curwin->w_cursor.lnum;
		}
	    }
	    ++curwin->w_cursor.lnum;
	    curwin->w_cursor.col = 0;  // make sure it's valid
	}

    // put cursor on first non-blank of indented line
    curwin->w_cursor.lnum = start_lnum;
    beginline(BL_SOL | BL_FIX);

    // Mark changed lines so that they will be redrawn.  When Visual
    // highlighting was present, need to continue until the last line.  When
    // there is no change still need to remove the Visual highlighting.
    ಯದಿ (last_changed != 0)
	changed_ಸಾಲುಗಳು(first_changed, 0,
		oap->is_VIsual ? start_lnum + oap->ಸಾಲು_ಎಣಿಕೆ :
		last_changed + 1, 0L);
    ಅನ್ಯಥಾ ಯದಿ (oap->is_VIsual)
	redraw_curbuf_later(UPD_INVERTED);

    ಯದಿ (oap->ಸಾಲು_ಎಣಿಕೆ > p_report)
    {
	i = oap->ಸಾಲು_ಎಣಿಕೆ - (i + 1);
	smsg(NGETTEXT("%ld line indented ",
						 "%ld lines indented ", i), i);
    }
    ಯದಿ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// set '[ and '] marks
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
    }
}

/*
 * Return TRUE if lines starting with '#' should be left aligned.
 */
    ಪೂರ್ಣಾಙ್ಕ
preprocs_ಎಡ(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ
	(curbuf->b_p_si && !curbuf->b_p_cin) ||
	(curbuf->b_p_cin && in_cinkeys('#', ' ', TRUE)
					   && curbuf->b_ind_hash_comment == 0)
	;
}

/*
 * Return TRUE if the conditions are OK for smart indenting.
 */
    ಪೂರ್ಣಾಙ್ಕ
may_do_si(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ curbuf->b_p_si
	&& !curbuf->b_p_cin
#ifdef FEAT_EVAL
	&& *curbuf->b_p_inde == NUL
#endif
	&& !p_paste;
}

/*
 * Try to do some very smart auto-indenting.
 * Used when inserting a "normal" character.
 */
    ಶೂನ್ಯ
ins_try_si(ಪೂರ್ಣಾಙ್ಕ c)
{
    pos_T	*pos, old_pos;
    char_u	*ptr;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		temp;

    // do some very smart indenting when entering '{' or '}'
    ಯದಿ (((did_si || can_si_back) && c == '{')
	    || (can_si && c == '}' && inindent(0)))
    {
	// for '}' set indent equal to indent of line containing matching '{'
	ಯದಿ (c == '}' && (pos = findmatch(ರಿಕ್ತ, '{')) != ರಿಕ್ತ)
	{
	    old_pos = curwin->w_cursor;
	    // If the matching '{' has a ')' immediately before it (ignoring
	    // white-space), then line up with the start of the line
	    // containing the matching '(' if there is one.  This handles the
	    // case where an "if (..\n..) {" statement continues over multiple
	    // lines -- webb
	    ptr = ml_get(pos->lnum);
	    i = pos->col;
	    ಯದಿ (i > 0)		// skip blanks before '{'
		ಯಾವತ್ (--i > 0 && VIM_ISWHITE(ptr[i]))
		    ;
	    curwin->w_cursor.lnum = pos->lnum;
	    curwin->w_cursor.col = i;
	    ಯದಿ (ptr[i] == ')' && (pos = findmatch(ರಿಕ್ತ, '(')) != ರಿಕ್ತ)
		curwin->w_cursor = *pos;
	    i = get_indent();
	    curwin->w_cursor = old_pos;
	    ಯದಿ (State & VREPLACE_FLAG)
		change_indent(INDENT_SET, i, FALSE, NUL, TRUE);
	    ಅನ್ಯಥಾ
		(ಶೂನ್ಯ)set_indent(i, SIN_CHANGED);
	}
	ಅನ್ಯಥಾ ಯದಿ (curwin->w_cursor.col > 0)
	{
	    // when inserting '{' after "O" reduce indent, but not
	    // more than indent of previous line
	    temp = TRUE;
	    ಯದಿ (c == '{' && can_si_back && curwin->w_cursor.lnum > 1)
	    {
		old_pos = curwin->w_cursor;
		i = get_indent();
		ಯಾವತ್ (curwin->w_cursor.lnum > 1)
		{
		    ptr = skipwhite(ml_get(--(curwin->w_cursor.lnum)));

		    // ignore empty lines and lines starting with '#'.
		    ಯದಿ (*ptr != '#' && *ptr != NUL)
			ಭಙ್ಗ;
		}
		ಯದಿ (get_indent() >= i)
		    temp = FALSE;
		curwin->w_cursor = old_pos;
	    }
	    ಯದಿ (temp)
		shift_ಸಾಲು(TRUE, FALSE, 1, TRUE);
	}
    }

    // set indent of '#' always to 0
    ಯದಿ (curwin->w_cursor.col > 0 && can_si && c == '#' && inindent(0))
    {
	// remember current indent for next line
	old_indent = get_indent();
	(ಶೂನ್ಯ)set_indent(0, SIN_CHANGED);
    }

    // Adjust ai_col, the char at this position can be deleted.
    ಯದಿ (ai_col > curwin->w_cursor.col)
	ai_col = curwin->w_cursor.col;
}

/*
 * Insert an indent (for <Tab> or CTRL-T) or delete an indent (for CTRL-D).
 * Keep the cursor on the same character.
 * type == INDENT_INC	increase indent (for CTRL-T or <Tab>)
 * type == INDENT_DEC	decrease indent (for CTRL-D)
 * type == INDENT_SET	set indent to "amount"
 * if round is TRUE, round the indent to 'shiftwidth' (only with _INC and _Dec).
 */
    ಶೂನ್ಯ
change_indent(
    ಪೂರ್ಣಾಙ್ಕ		type,
    ಪೂರ್ಣಾಙ್ಕ		amount,
    ಪೂರ್ಣಾಙ್ಕ		round,
    ಪೂರ್ಣಾಙ್ಕ		replaced,	// replaced character, put on replace stack
    ಪೂರ್ಣಾಙ್ಕ		call_changed_bytes)	// call changed_bytes()
{
    ಪೂರ್ಣಾಙ್ಕ		vcol;
    ಪೂರ್ಣಾಙ್ಕ		last_vcol;
    ಪೂರ್ಣಾಙ್ಕ		insstart_less;		// reduction for Insstart.col
    ಪೂರ್ಣಾಙ್ಕ		new_cursor_col;
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*ptr;
    ಪೂರ್ಣಾಙ್ಕ		save_p_list;
    ಪೂರ್ಣಾಙ್ಕ		start_col;
    colnr_T	vc;
    colnr_T	orig_col = 0;		// init for GCC
    char_u	*new_ಸಾಲು, *orig_ಸಾಲು = ರಿಕ್ತ;	// init for GCC

    // MODE_VREPLACE state needs to know what the line was like before changing
    ಯದಿ (State & VREPLACE_FLAG)
    {
	orig_ಸಾಲು = vim_strnsave(ml_get_curline(), ml_get_curline_len());  // Deal with NULL below
	orig_col = curwin->w_cursor.col;
    }

    // for the following tricks we don't want list mode
    save_p_list = curwin->w_p_list;
    curwin->w_p_list = FALSE;
#ifdef FEAT_PROP_POPUP
    ignore_text_props = TRUE;
#endif
    vc = getvcol_nolist(&curwin->w_cursor);
    vcol = vc;

    // For Replace mode we need to fix the replace stack later, which is only
    // possible when the cursor is in the indent.  Remember the number of
    // characters before the cursor if it's possible.
    start_col = curwin->w_cursor.col;

    // determine offset from first non-blank
    new_cursor_col = curwin->w_cursor.col;
    beginline(BL_WHITE);
    new_cursor_col -= curwin->w_cursor.col;

    insstart_less = curwin->w_cursor.col;

    // If the cursor is in the indent, compute how many screen columns the
    // cursor is to the left of the first non-blank.
    ಯದಿ (new_cursor_col < 0)
	vcol = get_indent() - vcol;

    ಯದಿ (new_cursor_col > 0)	    // can't fix replace stack
	start_col = -1;

    // Set the new indent.  The cursor will be put on the first non-blank.
    ಯದಿ (type == INDENT_SET)
	(ಶೂನ್ಯ)set_indent(amount, call_changed_bytes ? SIN_CHANGED : 0);
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ	save_State = State;

	// Avoid being called recursively.
	ಯದಿ (State & VREPLACE_FLAG)
	    State = MODE_ಸೇರಿಸು;
	shift_ಸಾಲು(type == INDENT_DEC, round, 1, call_changed_bytes);
	State = save_State;
    }
    insstart_less -= curwin->w_cursor.col;

    // Try to put cursor on same character.
    // If the cursor is at or after the first non-blank in the line,
    // compute the cursor column relative to the column of the first
    // non-blank character.
    // If we are not in insert mode, leave the cursor on the first non-blank.
    // If the cursor is before the first non-blank, position it relative
    // to the first non-blank, counted in screen columns.
    ಯದಿ (new_cursor_col >= 0)
    {
	// When changing the indent while the cursor is touching it, reset
	// Insstart_col to 0.
	ಯದಿ (new_cursor_col == 0)
	    insstart_less = MAXCOL;
	new_cursor_col += curwin->w_cursor.col;
    }
    ಅನ್ಯಥಾ ಯದಿ (!(State & MODE_ಸೇರಿಸು))
	new_cursor_col = curwin->w_cursor.col;
    ಅನ್ಯಥಾ
    {
	chartabsize_T cts;

	// Compute the screen column where the cursor should be.
	vcol = get_indent() - vcol;
	curwin->w_virtcol = (colnr_T)((vcol < 0) ? 0 : vcol);

	// Advance the cursor until we reach the right screen column.
	last_vcol = 0;
	ptr = ml_get_curline();
	init_chartabsize_arg(&cts, curwin, 0, 0, ptr, ptr);
	ಯಾವತ್ (cts.cts_vcol <= (ಪೂರ್ಣಾಙ್ಕ)curwin->w_virtcol)
	{
	    last_vcol = cts.cts_vcol;
	    ಯದಿ (cts.cts_vcol > 0)
		MB_PTR_ADV(cts.cts_ptr);
	    ಯದಿ (*cts.cts_ptr == NUL)
		ಭಙ್ಗ;
	    cts.cts_vcol += lbr_chartabsize(&cts);
	}
	vcol = last_vcol;
	new_cursor_col = cts.cts_ptr - cts.cts_ಸಾಲು;
	clear_chartabsize_arg(&cts);

	// May need to insert spaces to be able to position the cursor on
	// the right screen column.
	ಯದಿ (vcol != (ಪೂರ್ಣಾಙ್ಕ)curwin->w_virtcol)
	{
	    curwin->w_cursor.col = (colnr_T)new_cursor_col;
	    i = (ಪೂರ್ಣಾಙ್ಕ)curwin->w_virtcol - vcol;
	    ptr = alloc(i + 1);
	    ಯದಿ (ptr != ರಿಕ್ತ)
	    {
		size_t ptrlen;
		new_cursor_col += i;
		ptr[i] = NUL;
		ptrlen = i;
		ಯಾವತ್ (--i >= 0)
		    ptr[i] = ' ';
		ins_str(ptr, ptrlen);
		vim_free(ptr);
	    }
	}

	// When changing the indent while the cursor is in it, reset
	// Insstart_col to 0.
	insstart_less = MAXCOL;
    }

    curwin->w_p_list = save_p_list;

    ಯದಿ (new_cursor_col <= 0)
	curwin->w_cursor.col = 0;
    ಅನ್ಯಥಾ
	curwin->w_cursor.col = (colnr_T)new_cursor_col;
    curwin->w_set_curswant = true;
    changed_cline_bef_curs();

    // May have to adjust the start of the insert.
    ಯದಿ (State & MODE_ಸೇರಿಸು)
    {
	ಯದಿ (curwin->w_cursor.lnum == Insstart.lnum && Insstart.col != 0)
	{
	    ಯದಿ ((ಪೂರ್ಣಾಙ್ಕ)Insstart.col <= insstart_less)
		Insstart.col = 0;
	    ಅನ್ಯಥಾ
		Insstart.col -= insstart_less;
	}
	ಯದಿ ((ಪೂರ್ಣಾಙ್ಕ)ai_col <= insstart_less)
	    ai_col = 0;
	ಅನ್ಯಥಾ
	    ai_col -= insstart_less;
    }

    // For MODE_REPLACE state, may have to fix the replace stack, if it's
    // possible.  If the number of characters before the cursor decreased, need
    // to pop a few characters from the replace stack.
    // If the number of characters before the cursor increased, need to push a
    // few NULs onto the replace stack.
    ಯದಿ (REPLACE_NORMAL(State) && start_col >= 0)
    {
	ಯಾವತ್ (start_col > (ಪೂರ್ಣಾಙ್ಕ)curwin->w_cursor.col)
	{
	    replace_join(0);	    // remove a NUL from the replace stack
	    --start_col;
	}
	ಯಾವತ್ (start_col < (ಪೂರ್ಣಾಙ್ಕ)curwin->w_cursor.col || replaced)
	{
	    replace_ತಳ್ಳು(NUL);
	    ಯದಿ (replaced)
	    {
		replace_ತಳ್ಳು(replaced);
		replaced = NUL;
	    }
	    ++start_col;
	}
    }
#ifdef FEAT_PROP_POPUP
    ignore_text_props = FALSE;
#endif

    // For MODE_VREPLACE state, we also have to fix the replace stack.  In this
    // case it is always possible because we backspace over the whole line and
    // then put it back again the way we wanted it.
    ಯದಿ (State & VREPLACE_FLAG)
    {
	// If orig_line didn't allocate, just return.  At least we did the job,
	// even if you can't backspace.
	ಯದಿ (orig_ಸಾಲು == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;

	// Save new line
	new_ಸಾಲು = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	ಯದಿ (new_ಸಾಲು == ರಿಕ್ತ)
	{
	    vim_free(orig_ಸಾಲು);
	    ಪ್ರತ್ಯಾಗಮ;
	}

	// We only put back the new line up to the cursor
	new_ಸಾಲು[curwin->w_cursor.col] = NUL;

	// Put back original line
	ml_replace(curwin->w_cursor.lnum, orig_ಸಾಲು, FALSE);
	curwin->w_cursor.col = orig_col;

	// Backspace from cursor to start of line
	backspace_until_column(0);

	// Insert new stuff into line again
	ins_bytes(new_ಸಾಲು);

	vim_free(new_ಸಾಲು);
    }
}

/*
 * Copy the indent from ptr to the current line (and fill to size)
 * Leaves the cursor on the first non-blank in the line.
 * Returns TRUE if the line was changed.
 */
    ಪೂರ್ಣಾಙ್ಕ
copy_indent(ಪೂರ್ಣಾಙ್ಕ size, char_u *src)
{
    char_u	*p = ರಿಕ್ತ;
    char_u	*ಸಾಲು = ರಿಕ್ತ;
    char_u	*s;
    ಪೂರ್ಣಾಙ್ಕ		todo;
    ಪೂರ್ಣಾಙ್ಕ		ind_len;
    ಪೂರ್ಣಾಙ್ಕ		ಸಾಲು_len = 0;
    ಪೂರ್ಣಾಙ್ಕ		tab_pad;
    ಪೂರ್ಣಾಙ್ಕ		ind_done;
    ಪೂರ್ಣಾಙ್ಕ		round;
#ifdef FEAT_VARTABS
    ಪೂರ್ಣಾಙ್ಕ		ind_col;
#endif

    // Round 1: compute the number of characters needed for the indent
    // Round 2: copy the characters.
    ಪ್ರತಿ (round = 1; round <= 2; ++round)
    {
	todo = size;
	ind_len = 0;
	ind_done = 0;
#ifdef FEAT_VARTABS
	ind_col = 0;
#endif
	s = src;

	// Count/copy the usable portion of the source line
	ಯಾವತ್ (todo > 0 && VIM_ISWHITE(*s))
	{
	    ಯದಿ (*s == TAB)
	    {
#ifdef FEAT_VARTABS
		tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#ಅನ್ಯಥಾ
		tab_pad = (ಪೂರ್ಣಾಙ್ಕ)curbuf->b_p_ts
					   - (ind_done % (ಪೂರ್ಣಾಙ್ಕ)curbuf->b_p_ts);
#endif
		// Stop if this tab will overshoot the target
		ಯದಿ (todo < tab_pad)
		    ಭಙ್ಗ;
		todo -= tab_pad;
		ind_done += tab_pad;
#ifdef FEAT_VARTABS
		ind_col += tab_pad;
#endif
	    }
	    ಅನ್ಯಥಾ
	    {
		--todo;
		++ind_done;
#ifdef FEAT_VARTABS
		++ind_col;
#endif
	    }
	    ++ind_len;
	    ಯದಿ (p != ರಿಕ್ತ)
		*p++ = *s;
	    ++s;
	}

	// Fill to next tabstop with a tab, if possible
#ifdef FEAT_VARTABS
	tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#ಅನ್ಯಥಾ
	tab_pad = (ಪೂರ್ಣಾಙ್ಕ)curbuf->b_p_ts - (ind_done % (ಪೂರ್ಣಾಙ್ಕ)curbuf->b_p_ts);
#endif
	ಯದಿ (todo >= tab_pad && !curbuf->b_p_et)
	{
	    todo -= tab_pad;
	    ++ind_len;
#ifdef FEAT_VARTABS
	    ind_col += tab_pad;
#endif
	    ಯದಿ (p != ರಿಕ್ತ)
		*p++ = TAB;
	}

	// Add tabs required for indent
	ಯದಿ (!curbuf->b_p_et)
	{
#ifdef FEAT_VARTABS
	    ಪ್ರತಿ (;;)
	    {
		tab_pad = tabstop_padding(ind_col, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
		ಯದಿ (todo < tab_pad)
		    ಭಙ್ಗ;
		todo -= tab_pad;
		++ind_len;
		ind_col += tab_pad;
		ಯದಿ (p != ರಿಕ್ತ)
		    *p++ = TAB;
	    }
#ಅನ್ಯಥಾ
	    ಯಾವತ್ (todo >= (ಪೂರ್ಣಾಙ್ಕ)curbuf->b_p_ts)
	    {
		todo -= (ಪೂರ್ಣಾಙ್ಕ)curbuf->b_p_ts;
		++ind_len;
		ಯದಿ (p != ರಿಕ್ತ)
		    *p++ = TAB;
	    }
#endif
	}

	// Count/add spaces required for indent
	ಯಾವತ್ (todo > 0)
	{
	    --todo;
	    ++ind_len;
	    ಯದಿ (p != ರಿಕ್ತ)
		*p++ = ' ';
	}

	ಯದಿ (p == ರಿಕ್ತ)
	{
	    // Allocate memory for the result: the copied indent, new indent
	    // and the rest of the line.
	    ಸಾಲು_len = ml_get_curline_len() + 1;
	    ಸಾಲು = alloc(ind_len + ಸಾಲು_len);
	    ಯದಿ (ಸಾಲು == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    p = ಸಾಲು;
	}
    }

    // Append the original line
    mch_memmove(p, ml_get_curline(), (size_t)ಸಾಲು_len);

    // Replace the line
    ml_replace(curwin->w_cursor.lnum, ಸಾಲು, FALSE);

    // Put the cursor after the indent.
    curwin->w_cursor.col = ind_len;
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Give a "resulting text too long" error and maybe set got_int.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
emsg_text_too_long(ಶೂನ್ಯ)
{
    emsg(_(e_resulting_text_too_long));
#ifdef FEAT_EVAL
    // when not inside a try/catch set got_int to break out of any loop
    ಯದಿ (trylevel == 0)
#endif
	got_int = TRUE;
}

/*
 * ":retab".
 */
    ಶೂನ್ಯ
ex_retab(exarg_T *eap)
{
    linenr_T	lnum;
    ಪೂರ್ಣಾಙ್ಕ		got_tab = FALSE;
    long	ಅಂಕೆ_spaces = 0;
    long	ಅಂಕೆ_tabs;
    long	len;
    long	col;
    long	vcol;
    long	start_col = 0;		// For start of white-space string
    long	start_vcol = 0;		// For start of white-space string
    long	old_len;
    long	new_len;
    char_u	*ptr;
    char_u	*new_ಸಾಲು = (char_u *)1; // init to non-NULL
    ಪೂರ್ಣಾಙ್ಕ		did_undo;		// called u_save for current line
#ifdef FEAT_VARTABS
    ಪೂರ್ಣಾಙ್ಕ		*new_vts_array = ರಿಕ್ತ;
    char_u	*new_ts_str;		// string value of tab argument
#ಅನ್ಯಥಾ
    ಪೂರ್ಣಾಙ್ಕ		temp;
    ಪೂರ್ಣಾಙ್ಕ		new_ts = 0;
#endif
    ಪೂರ್ಣಾಙ್ಕ		save_list;
    linenr_T	first_ಸಾಲು = 0;		// first changed line
    linenr_T	last_ಸಾಲು = 0;		// last changed line
    ಪೂರ್ಣಾಙ್ಕ		is_indent_only = 0;	// Only process leading whitespace

    save_list = curwin->w_p_list;
    curwin->w_p_list = 0;	    // don't want list mode here

    ptr = eap->arg;
    ಯದಿ (STRNCMP(ptr, "-indentonly", 11) == 0 && IS_WHITE_OR_NUL(ptr[11]))
    {
	is_indent_only = 1;
	ptr = skipwhite(ptr + 11);
    }

#ifdef FEAT_VARTABS
    new_ts_str = ptr;
    ಯದಿ (tabstop_set(ptr, &new_vts_array) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    ಯಾವತ್ (vim_isdigit(*ptr) || *ptr == ',')
	++ptr;

    // This ensures that either new_vts_array and new_ts_str are freshly
    // allocated, or new_vts_array points to an existing array and new_ts_str
    // is null.
    ಯದಿ (new_vts_array == ರಿಕ್ತ)
    {
	new_vts_array = curbuf->b_p_vts_array;
	new_ts_str = ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ
	new_ts_str = vim_strnsave(new_ts_str, ptr - new_ts_str);
#ಅನ್ಯಥಾ
    ಯದಿ (ptr[0] != NUL && (ptr[0] != '0' || ptr[1] != NUL))
    {
	char_u	*end;

	ಯದಿ (strtol((ವರ್ಣ *)ptr, (ವರ್ಣ **)&end, 10) <= 0)
	{
	    ಯದಿ (ptr != end)
		emsg(_(e_argument_must_be_positive));
	    ಅನ್ಯಥಾ
		semsg(_(e_invalid_argument_str), ptr);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	new_ts = getdigits(&ptr);
	ಯದಿ (new_ts < 0 || new_ts > TABSTOP_MAX)
	{
	    semsg(_(e_invalid_argument_str), eap->arg);
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }
    ಯದಿ (new_ts == 0)
	new_ts = curbuf->b_p_ts;
#endif
    ಪ್ರತಿ (lnum = eap->line1; !got_int && lnum <= eap->line2; ++lnum)
    {
	ptr = ml_get(lnum);
	old_len = ml_get_len(lnum);
	col = 0;
	vcol = 0;
	did_undo = FALSE;
	ಪ್ರತಿ (;;)
	{
	    ಯದಿ (VIM_ISWHITE(ptr[col]))
	    {
		ಯದಿ (!got_tab && ಅಂಕೆ_spaces == 0)
		{
		    // First consecutive white-space
		    start_vcol = vcol;
		    start_col = col;
		}
		ಯದಿ (ptr[col] == ' ')
		    ಅಂಕೆ_spaces++;
		ಅನ್ಯಥಾ
		    got_tab = TRUE;
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (got_tab || (eap->forceit && ಅಂಕೆ_spaces > 1))
		{
		    // Retabulate this string of white-space

		    // len is virtual length of white string
		    len = ಅಂಕೆ_spaces = vcol - start_vcol;
		    ಅಂಕೆ_tabs = 0;
		    ಯದಿ (!curbuf->b_p_et)
		    {
#ifdef FEAT_VARTABS
			ಪೂರ್ಣಾಙ್ಕ t, s;

			tabstop_fromto(start_vcol, vcol,
					curbuf->b_p_ts, new_vts_array, &t, &s);
			ಅಂಕೆ_tabs = t;
			ಅಂಕೆ_spaces = s;
#ಅನ್ಯಥಾ
			temp = new_ts - (start_vcol % new_ts);
			ಯದಿ (ಅಂಕೆ_spaces >= temp)
			{
			    ಅಂಕೆ_spaces -= temp;
			    ಅಂಕೆ_tabs++;
			}
			ಅಂಕೆ_tabs += ಅಂಕೆ_spaces / new_ts;
			ಅಂಕೆ_spaces -= (ಅಂಕೆ_spaces / new_ts) * new_ts;
#endif
		    }
		    ಯದಿ (curbuf->b_p_et || got_tab ||
					(ಅಂಕೆ_spaces + ಅಂಕೆ_tabs < len))
		    {
			ಯದಿ (did_undo == FALSE)
			{
			    did_undo = TRUE;
			    ಯದಿ (u_save((linenr_T)(lnum - 1),
						(linenr_T)(lnum + 1)) == FAIL)
			    {
				new_ಸಾಲು = ರಿಕ್ತ;	// flag out-of-memory
				ಭಙ್ಗ;
			    }
			}

			// len is actual number of white characters used
			len = ಅಂಕೆ_spaces + ಅಂಕೆ_tabs;
			new_len = old_len - col + start_col + len + 1;
			ಯದಿ (new_len <= 0 || new_len >= MAXCOL)
			{
			    emsg_text_too_long();
			    ಭಙ್ಗ;
			}
			new_ಸಾಲು = alloc(new_len);
			ಯದಿ (new_ಸಾಲು == ರಿಕ್ತ)
			    ಭಙ್ಗ;
			ಯದಿ (start_col > 0)
			    mch_memmove(new_ಸಾಲು, ptr, (size_t)start_col);
			mch_memmove(new_ಸಾಲು + start_col + len,
				      ptr + col, (size_t)(old_len - col + 1));
			ptr = new_ಸಾಲು + start_col;
			ಪ್ರತಿ (col = 0; col < len; col++)
			    ptr[col] = (col < ಅಂಕೆ_tabs) ? '\t' : ' ';
			ಯದಿ (ml_replace(lnum, new_ಸಾಲು, FALSE) == OK)
			    // "new_line" may have been copied
			    new_ಸಾಲು = curbuf->b_ml.ml_ಸಾಲು_ptr;
			ಯದಿ (first_ಸಾಲು == 0)
			    first_ಸಾಲು = lnum;
			last_ಸಾಲು = lnum;
			ptr = new_ಸಾಲು;
			old_len = new_len - 1;
			col = start_col + len;
		    }
		}
		got_tab = FALSE;
		ಅಂಕೆ_spaces = 0;

		ಯದಿ (is_indent_only)
		    ಭಙ್ಗ;
	    }
	    ಯದಿ (ptr[col] == NUL)
		ಭಙ್ಗ;
	    vcol += chartabsize(ptr + col, (colnr_T)vcol);
	    ಯದಿ (vcol >= MAXCOL)
	    {
		emsg_text_too_long();
		ಭಙ್ಗ;
	    }
	    ಯದಿ (has_mbyte)
		col += (*mb_ptr2len)(ptr + col);
	    ಅನ್ಯಥಾ
		++col;
	}
	ಯದಿ (new_ಸಾಲು == ರಿಕ್ತ)		    // out of memory
	    ಭಙ್ಗ;
	ಸಾಲು_breakcheck();
    }
    ಯದಿ (got_int)
	emsg(_(e_interrupted));

#ifdef FEAT_VARTABS
    // If a single value was given then it can be considered equal to
    // either the value of 'tabstop' or the value of 'vartabstop'.
    ಯದಿ (tabstop_ಎಣಿಕೆ(curbuf->b_p_vts_array) == 0
	&& tabstop_ಎಣಿಕೆ(new_vts_array) == 1
	&& curbuf->b_p_ts == tabstop_first(new_vts_array))
	; // not changed
    ಅನ್ಯಥಾ ಯದಿ (tabstop_ಎಣಿಕೆ(curbuf->b_p_vts_array) > 0
	&& tabstop_eq(curbuf->b_p_vts_array, new_vts_array))
	; // not changed
    ಅನ್ಯಥಾ
	redraw_curbuf_later(UPD_NOT_VALID);
#ಅನ್ಯಥಾ
    ಯದಿ (curbuf->b_p_ts != new_ts)
	redraw_curbuf_later(UPD_NOT_VALID);
#endif
    ಯದಿ (first_ಸಾಲು != 0)
	changed_ಸಾಲುಗಳು(first_ಸಾಲು, 0, last_ಸಾಲು + 1, 0L);

    curwin->w_p_list = save_list;	// restore 'list'

#ifdef FEAT_VARTABS
    ಯದಿ (new_ts_str != ರಿಕ್ತ)		// set the new tabstop
    {
	// If 'vartabstop' is in use or if the value given to retab has more
	// than one tabstop then update 'vartabstop'.
	ಪೂರ್ಣಾಙ್ಕ *old_vts_ary = curbuf->b_p_vts_array;

	ಯದಿ (tabstop_ಎಣಿಕೆ(old_vts_ary) > 0 || tabstop_ಎಣಿಕೆ(new_vts_array) > 1)
	{
	    set_string_option_direct((char_u *)"vts", -1, new_ts_str,
							OPT_FREE|OPT_LOCAL, 0);
	    curbuf->b_p_vts_array = new_vts_array;
	    vim_free(old_vts_ary);
	}
	ಅನ್ಯಥಾ
	{
	    // 'vartabstop' wasn't in use and a single value was given to
	    // retab then update 'tabstop'.
	    curbuf->b_p_ts = tabstop_first(new_vts_array);
	    vim_free(new_vts_array);
	}
	vim_free(new_ts_str);
    }
#ಅನ್ಯಥಾ
    curbuf->b_p_ts = new_ts;
#endif
    coladvance(curwin->w_curswant);

    u_clearline();
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Get indent level from 'indentexpr'.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_ಅಭಿವ್ಯಕ್ತಿ_indent(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		indent = -1;
    char_u	*inde_copy;
    pos_T	save_pos;
    colnr_T	save_curswant;
    ಪೂರ್ಣಾಙ್ಕ		save_set_curswant;
    ಪೂರ್ಣಾಙ್ಕ		save_State;
    ಪೂರ್ಣಾಙ್ಕ		use_sandbox = was_set_insecurely(curwin,
					    (char_u *)"indentexpr", OPT_LOCAL);
    sctx_T	save_sctx = current_sctx;

    // Save and restore cursor position and curswant, in case it was changed
    // via :normal commands
    save_pos = curwin->w_cursor;
    save_curswant = curwin->w_curswant;
    save_set_curswant = curwin->w_set_curswant;
    set_vim_var_nr(VV_LNUM, curwin->w_cursor.lnum);
    ಯದಿ (use_sandbox)
	++sandbox;
    ++textlock;
    current_sctx = curbuf->b_p_script_ctx[BV_INDE];

    // Need to make a copy, the 'indentexpr' option could be changed while
    // evaluating it.
    inde_copy = vim_strsave(curbuf->b_p_inde);
    ಯದಿ (inde_copy != ರಿಕ್ತ)
    {
	indent = (ಪೂರ್ಣಾಙ್ಕ)eval_to_ಸಂಖ್ಯೆ(inde_copy, TRUE);
	vim_free(inde_copy);
    }

    ಯದಿ (use_sandbox)
	--sandbox;
    --textlock;
    current_sctx = save_sctx;

    // Restore the cursor position so that 'indentexpr' doesn't need to.
    // Pretend to be in Insert mode, allow cursor past end of line for "o"
    // command.
    save_State = State;
    State = MODE_ಸೇರಿಸು;
    curwin->w_cursor = save_pos;
    curwin->w_curswant = save_curswant;
    curwin->w_set_curswant = save_set_curswant;
    check_cursor();
    State = save_State;

    // Reset did_throw, unless 'debug' has "throw" and inside a try/catch.
    ಯದಿ (did_throw && (vim_strchr(p_debug, 't') == ರಿಕ್ತ || trylevel == 0))
    {
	handle_did_throw();
	did_throw = FALSE;
    }

    // If there is an error, just keep the current indent.
    ಯದಿ (indent < 0)
	indent = get_indent();

    ಪ್ರತ್ಯಾಗಮ indent;
}
#endif

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
lisp_match(char_u *p)
{
    char_u	buf[LSIZE];
    ಪೂರ್ಣಾಙ್ಕ		len;
    char_u	*word = *curbuf->b_p_lw != NUL ? curbuf->b_p_lw : p_lispwords;

    ಯಾವತ್ (*word != NUL)
    {
	len = copy_option_part(&word, buf, LSIZE, ",");
	ಯದಿ (STRNCMP(buf, p, len) == 0 && IS_WHITE_OR_NUL(p[len]))
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * When 'p' is present in 'cpoptions, a Vi compatible method is used.
 * The incompatible newer method is quite a bit better at indenting
 * code in lisp-like languages than the traditional one; it's still
 * mostly heuristics however -- Dirk van Deun, dirk@rave.org
 *
 * TODO:
 * Findmatch() should be adapted for lisp, also to make showmatch
 * work correctly: now (v5.3) it seems all C/C++ oriented:
 * - it does not recognize the #\( and #\) notations as character literals
 * - it doesn't know about comments starting with a semicolon
 * - it incorrectly interprets '(' as a character literal
 * All this messes up get_lisp_indent in some rare cases.
 * Update from Sergey Khorev:
 * I tried to fix the first two issues.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_lisp_indent(ಶೂನ್ಯ)
{
    pos_T	*pos, realpos, paren;
    ಪೂರ್ಣಾಙ್ಕ		amount;
    char_u	*that;
    colnr_T	col;
    colnr_T	firsttry;
    ಪೂರ್ಣಾಙ್ಕ		parencount, quotecount;
    ಪೂರ್ಣಾಙ್ಕ		vi_lisp;

    // Set vi_lisp to use the vi-compatible method
    vi_lisp = (vim_strchr(p_cpo, CPO_LISP) != ರಿಕ್ತ);

    realpos = curwin->w_cursor;
    curwin->w_cursor.col = 0;

    ಯದಿ ((pos = findmatch(ರಿಕ್ತ, '(')) == ರಿಕ್ತ)
	pos = findmatch(ರಿಕ್ತ, '[');
    ಅನ್ಯಥಾ
    {
	paren = *pos;
	pos = findmatch(ರಿಕ್ತ, '[');
	ಯದಿ (pos == ರಿಕ್ತ || LT_POSP(pos, &paren))
	    pos = &paren;
    }
    ಯದಿ (pos != ರಿಕ್ತ)
    {
	// Extra trick: Take the indent of the first previous non-white
	// line that is at the same () level.
	amount = -1;
	parencount = 0;

	ಯಾವತ್ (--curwin->w_cursor.lnum >= pos->lnum)
	{
	    ಯದಿ (linewhite(curwin->w_cursor.lnum))
		ಅನುವರ್ತ;
	    ಪ್ರತಿ (that = ml_get_curline(); *that != NUL; ++that)
	    {
		ಯದಿ (*that == ';')
		{
		    ಯಾವತ್ (*(that + 1) != NUL)
			++that;
		    ಅನುವರ್ತ;
		}
		ಯದಿ (*that == '\\')
		{
		    ಯದಿ (*(that + 1) != NUL)
			++that;
		    ಅನುವರ್ತ;
		}
		ಯದಿ (*that == '"' && *(that + 1) != NUL)
		{
		    ಯಾವತ್ (*++that && *that != '"')
		    {
			// skipping escaped characters in the string
			ಯದಿ (*that == '\\')
			{
			    ಯದಿ (*++that == NUL)
				ಭಙ್ಗ;
			    ಯದಿ (that[1] == NUL)
			    {
				++that;
				ಭಙ್ಗ;
			    }
			}
		    }
		    ಯದಿ (*that == NUL)
			ಭಙ್ಗ;
		}
		ಯದಿ (*that == '(' || *that == '[')
		    ++parencount;
		ಅನ್ಯಥಾ ಯದಿ (*that == ')' || *that == ']')
		    --parencount;
	    }
	    ಯದಿ (parencount == 0)
	    {
		amount = get_indent();
		ಭಙ್ಗ;
	    }
	}

	ಯದಿ (amount == -1)
	{
	    curwin->w_cursor.lnum = pos->lnum;
	    curwin->w_cursor.col = pos->col;
	    col = pos->col;

	    that = ml_get_curline();

	    ಯದಿ (vi_lisp && get_indent() == 0)
		amount = 2;
	    ಅನ್ಯಥಾ
	    {
		char_u		*ಸಾಲು = that;
		chartabsize_T	cts;

		init_chartabsize_arg(&cts, curwin, pos->lnum, 0, ಸಾಲು, ಸಾಲು);
		ಯಾವತ್ (*cts.cts_ptr != NUL && col > 0)
		{
		    cts.cts_vcol += lbr_chartabsize_adv(&cts);
		    col--;
		}
		amount = cts.cts_vcol;
		that = cts.cts_ptr;
		clear_chartabsize_arg(&cts);

		// Some keywords require "body" indenting rules (the
		// non-standard-lisp ones are Scheme special forms):
		//
		// (let ((a 1))    instead    (let ((a 1))
		//   (...))	      of	   (...))

		ಯದಿ (!vi_lisp && (*that == '(' || *that == '[')
						      && lisp_match(that + 1))
		    amount += 2;
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (*that != NUL)
		    {
			that++;
			amount++;
		    }
		    firsttry = amount;

		    init_chartabsize_arg(&cts, curwin, (colnr_T)(that - ಸಾಲು),
							   amount, ಸಾಲು, that);
		    ಯಾವತ್ (VIM_ISWHITE(*cts.cts_ptr))
		    {
			cts.cts_vcol += lbr_chartabsize(&cts);
			++cts.cts_ptr;
		    }
		    that = cts.cts_ptr;
		    amount = cts.cts_vcol;
		    clear_chartabsize_arg(&cts);

		    ಯದಿ (*that && *that != ';') // not a comment line
		    {
			// test *that != '(' to accommodate first let/do
			// argument if it is more than one line
			ಯದಿ (!vi_lisp && *that != '(' && *that != '[')
			    firsttry++;

			parencount = 0;
			quotecount = 0;

			init_chartabsize_arg(&cts, curwin,
				   (colnr_T)(that - ಸಾಲು), amount, ಸಾಲು, that);
			ಯದಿ (vi_lisp
				|| (*that != '"'
				    && *that != '\''
				    && *that != '#'
				    && (*that < '0' || *that > '9')))
			{
			    ಯಾವತ್ (*cts.cts_ptr
				    && (!VIM_ISWHITE(*cts.cts_ptr)
					|| quotecount
					|| parencount)
				    && (!((*cts.cts_ptr == '('
							|| *cts.cts_ptr == '[')
					    && !quotecount
					    && !parencount
					    && vi_lisp)))
			    {
				ಯದಿ (*cts.cts_ptr == '"')
				    quotecount = !quotecount;
				ಯದಿ ((*cts.cts_ptr == '(' || *cts.cts_ptr == '[')
							       && !quotecount)
				    ++parencount;
				ಯದಿ ((*cts.cts_ptr == ')' || *cts.cts_ptr == ']')
							       && !quotecount)
				    --parencount;
				ಯದಿ (*cts.cts_ptr == '\\'
						    && *(cts.cts_ptr+1) != NUL)
				    cts.cts_vcol += lbr_chartabsize_adv(&cts);
				cts.cts_vcol += lbr_chartabsize_adv(&cts);
			    }
			}
			ಯಾವತ್ (VIM_ISWHITE(*cts.cts_ptr))
			{
			    cts.cts_vcol += lbr_chartabsize(&cts);
			    ++cts.cts_ptr;
			}
			that = cts.cts_ptr;
			amount = cts.cts_vcol;
			clear_chartabsize_arg(&cts);

			ಯದಿ (!*that || *that == ';')
			    amount = firsttry;
		    }
		}
	    }
	}
    }
    ಅನ್ಯಥಾ
	amount = 0;	// no matching '(' or '[' found, use zero indent

    curwin->w_cursor = realpos;

    ಪ್ರತ್ಯಾಗಮ amount;
}

/*
 * Re-indent the current line, based on the current contents of it and the
 * surrounding lines. Fixing the cursor position seems really easy -- I'm very
 * confused what all the part that handles Control-T is doing that I'm not.
 * "get_the_indent" should be get_c_indent, get_expr_indent or get_lisp_indent.
 */

    ಶೂನ್ಯ
fixthisline(ಪೂರ್ಣಾಙ್ಕ (*get_the_indent)(ಶೂನ್ಯ))
{
    ಪೂರ್ಣಾಙ್ಕ amount = get_the_indent();

    ಯದಿ (amount < 0)
	ಪ್ರತ್ಯಾಗಮ;

    change_indent(INDENT_SET, amount, FALSE, 0, TRUE);
    ಯದಿ (linewhite(curwin->w_cursor.lnum))
	did_ai = TRUE;	// delete the indent if the line stays empty
}

/*
 * Return TRUE if 'indentexpr' should be used for Lisp indenting.
 * Caller may want to check 'autoindent'.
 */
    ಪೂರ್ಣಾಙ್ಕ
use_indentexpr_for_lisp(ಶೂನ್ಯ)
{
#ifdef FEAT_EVAL
    ಪ್ರತ್ಯಾಗಮ curbuf->b_p_lisp
		&& *curbuf->b_p_inde != NUL
		&& STRCMP(curbuf->b_p_lop, "expr:1") == 0;
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ FALSE;
#endif
}

/*
 * Fix indent for 'lisp' and 'cindent'.
 */
    ಶೂನ್ಯ
fix_indent(ಶೂನ್ಯ)
{
    ಯದಿ (p_paste)
	ಪ್ರತ್ಯಾಗಮ;  // no auto-indenting when 'paste' is set
    ಯದಿ (curbuf->b_p_lisp && curbuf->b_p_ai)
    {
	ಯದಿ (use_indentexpr_for_lisp())
	    do_c_ಅಭಿವ್ಯಕ್ತಿ_indent();
	ಅನ್ಯಥಾ
	    fixthisline(get_lisp_indent);
    }
    ಅನ್ಯಥಾ ಯದಿ (cindent_on())
	do_c_ಅಭಿವ್ಯಕ್ತಿ_indent();
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * "indent()" function
 */
    ಶೂನ್ಯ
f_indent(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    ಯದಿ (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    lnum = tv_get_lnum(argvars);
    ಯದಿ (lnum >= 1 && lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	rettv->vval.v_ಸಂಖ್ಯೆ = get_indent_lnum(lnum);
    ಅನ್ಯಥಾ
    {
	ಯದಿ (in_vim9script())
	    semsg(_(e_invalid_ಸಾಲು_ಸಂಖ್ಯೆ_nr), lnum);
	rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    }
}

/*
 * "lispindent(lnum)" function
 */
    ಶೂನ್ಯ
f_lispindent(typval_T *argvars UNUSED, typval_T *rettv)
{
    pos_T	pos;
    linenr_T	lnum;

    ಯದಿ (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    ಯದಿ (lnum >= 1 && lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_ಸಂಖ್ಯೆ = get_lisp_indent();
	curwin->w_cursor = pos;
    }
    ಅನ್ಯಥಾ ಯದಿ (in_vim9script())
	semsg(_(e_invalid_ಸಾಲು_ಸಂಖ್ಯೆ_nr), lnum);
    ಅನ್ಯಥಾ
	rettv->vval.v_ಸಂಖ್ಯೆ = -1;
}
#endif
