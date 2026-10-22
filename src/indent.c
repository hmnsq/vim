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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_VARTABS)

/*
 * Set the integer values corresponding to the string setting of 'vartabstop'.
 * "array" will be set, caller must free it if needed.
 * Return FAIL for an error.
 */
    ਪੂਰਨ
tabstop_ਸੈੱਟ(char_u *var, ਪੂਰਨ **array)
{
    ਪੂਰਨ	    valcount = 1;
    ਪੂਰਨ	    t;
    char_u  *cp;

    ਜੇ (var[0] == NUL || (var[0] == '0' && var[1] == NUL))
    {
	*array = ਨਿਰਾ;
	ਵਾਪਸ OK;
    }

    ਲਈ (cp = var; *cp != NUL; ++cp)
    {
	ਜੇ (cp == var || cp[-1] == ',')
	{
	    char_u *ਸਿਰਾ;

	    ਜੇ (strtol((ਅੱਖਰ *)cp, (ਅੱਖਰ **)&ਸਿਰਾ, 10) <= 0)
	    {
		ਜੇ (cp != ਸਿਰਾ)
		    emsg(_(e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ));
		ਨਹੀਂਤਾਂ
		    semsg(_(e_invalid_ਦਲੀਲ_str), cp);
		ਵਾਪਸ FAIL;
	    }
	}

	ਜੇ (VIM_ISDIGIT(*cp))
	    ਜਾਰੀ;
	ਜੇ (cp[0] == ',' && cp > var && cp[-1] != ',' && cp[1] != NUL)
	{
	    ++valcount;
	    ਜਾਰੀ;
	}
	semsg(_(e_invalid_ਦਲੀਲ_str), var);
	ਵਾਪਸ FAIL;
    }

    *array = ALLOC_MULT(ਪੂਰਨ, valcount + 1);
    ਜੇ (*array == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    (*array)[0] = valcount;

    t = 1;
    ਲਈ (cp = var; *cp != NUL;)
    {
	ਪੂਰਨ n = atoi((ਅੱਖਰ *)cp);

	// Catch negative values, overflow and ridiculous big values.
	ਜੇ (n <= 0 || n > TABSTOP_MAX)
	{
	    semsg(_(e_invalid_ਦਲੀਲ_str), cp);
	    VIM_CLEAR(*array);
	    ਵਾਪਸ FAIL;
	}
	(*array)[t++] = n;
	ਜਦੋਂ (*cp != NUL && *cp != ',')
	    ++cp;
	ਜੇ (*cp != NUL)
	    ++cp;
    }

    ਵਾਪਸ OK;
}

/*
 * Calculate the number of screen spaces a tab will occupy.
 * If "vts" is set then the tab widths are taken from that array,
 * otherwise the value of ts is used.
 */
    ਪੂਰਨ
tabstop_padding(colnr_T ਖਾਨਾ, ਪੂਰਨ ts_arg, ਪੂਰਨ *vts)
{
    ਪੂਰਨ		ts = ts_arg == 0 ? 8 : ts_arg;
    ਪੂਰਨ		tabcount;
    colnr_T	tabcol = 0;
    ਪੂਰਨ		t;
    ਪੂਰਨ		padding = 0;

    ਜੇ (vts == ਨਿਰਾ || vts[0] == 0)
	ਵਾਪਸ ts - (ਖਾਨਾ % ts);

    tabcount = vts[0];

    ਲਈ (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	ਜੇ (tabcol > ਖਾਨਾ)
	{
	    padding = (ਪੂਰਨ)(tabcol - ਖਾਨਾ);
	    ਰੋਕੋ;
	}
    }
    ਜੇ (t > tabcount)
	padding = vts[tabcount] - (ਪੂਰਨ)((ਖਾਨਾ - tabcol) % vts[tabcount]);

    ਵਾਪਸ padding;
}

/*
 * Find the size of the tab that covers a particular column.
 *
 * If this is being called as part of a shift operation, col is not the cursor
 * column but is the column number to the left of the first non-whitespace
 * character in the line.  If the shift is to the left (left = TRUE), then
 * return the size of the tab interval to the left of the column.
 */
    ਪੂਰਨ
tabstop_at(colnr_T ਖਾਨਾ, ਪੂਰਨ ts, ਪੂਰਨ *vts, ਪੂਰਨ ਖੱਬਾ)
{
    ਪੂਰਨ		tabcount;	// Number of tab stops in the list of variable
				// tab stops.
    colnr_T	tabcol = 0;	// Column of the tab stop under consideration.
    ਪੂਰਨ		t;		// Tabstop index in the list of variable tab
				// stops.
    ਪੂਰਨ		tab_ਮਾਪ = 0;	// Size of the tab stop interval to the right
				// or left of the col.

    ਜੇ (vts == 0 || vts[0] == 0)
	ਵਾਪਸ ts;

    tabcount = vts[0];
    ਲਈ (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	ਜੇ (tabcol > ਖਾਨਾ)
	{
	    // If shifting left (left != 0), and if the column to the left of
	    // the first first non-blank character (col) in the line is
	    // already to the left of the first tabstop, set the shift amount
	    // (tab_size) to just enough to shift the line to the left margin.
	    // The value doesn't seem to matter as long as it is at least that
	    // distance.
	    ਜੇ (ਖੱਬਾ && (t == 1))
		tab_ਮਾਪ = ਖਾਨਾ;
	    ਨਹੀਂਤਾਂ
		tab_ਮਾਪ = vts[t - (ਖੱਬਾ ? 1 : 0)];
	    ਰੋਕੋ;
	}
    }
    ਜੇ (t > tabcount)		// If the value of the index t is beyond the
				// end of the list, use the tab stop value at
				// the end of the list.
	tab_ਮਾਪ = vts[tabcount];

    ਵਾਪਸ tab_ਮਾਪ;
}

/*
 * Find the column on which a tab starts.
 */
    colnr_T
tabstop_ਸ਼ੁਰੂ(colnr_T ਖਾਨਾ, ਪੂਰਨ ts, ਪੂਰਨ *vts)
{
    ਪੂਰਨ		tabcount;
    colnr_T	tabcol = 0;
    ਪੂਰਨ		t;
    ਪੂਰਨ		excess;

    ਜੇ (vts == ਨਿਰਾ || vts[0] == 0)
	ਵਾਪਸ ਖਾਨਾ - ਖਾਨਾ % ts;

    tabcount = vts[0];
    ਲਈ (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	ਜੇ (tabcol > ਖਾਨਾ)
	    ਵਾਪਸ tabcol - vts[t];
    }

    excess = tabcol % vts[tabcount];
    ਵਾਪਸ ਖਾਨਾ - (ਖਾਨਾ - excess) % vts[tabcount];
}

/*
 * Find the number of tabs and spaces necessary to get from one column
 * to another.
 */
    ਖਾਲੀ
tabstop_fromto(
	colnr_T ਸ਼ੁਰੂ_ਖਾਨਾ,
	colnr_T ਸਿਰਾ_ਖਾਨਾ,
	ਪੂਰਨ	ts_arg,
	ਪੂਰਨ	*vts,
	ਪੂਰਨ	*ntabs,
	ਪੂਰਨ	*nspcs)
{
    ਪੂਰਨ		spaces = ਸਿਰਾ_ਖਾਨਾ - ਸ਼ੁਰੂ_ਖਾਨਾ;
    colnr_T	tabcol = 0;
    ਪੂਰਨ		padding = 0;
    ਪੂਰਨ		tabcount;
    ਪੂਰਨ		t;
    ਪੂਰਨ		ts = ts_arg == 0 ? curbuf->b_p_ts : ts_arg;

    ਜੇ (vts == ਨਿਰਾ || vts[0] == 0)
    {
	ਪੂਰਨ tabs = 0;
	ਪੂਰਨ initspc = 0;

	initspc = ts - (ਸ਼ੁਰੂ_ਖਾਨਾ % ts);
	ਜੇ (spaces >= initspc)
	{
	    spaces -= initspc;
	    tabs++;
	}
	tabs += spaces / ts;
	spaces -= (spaces / ts) * ts;

	*ntabs = tabs;
	*nspcs = spaces;
	ਵਾਪਸ;
    }

    // Find the padding needed to reach the next tabstop.
    tabcount = vts[0];
    ਲਈ (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	ਜੇ (tabcol > ਸ਼ੁਰੂ_ਖਾਨਾ)
	{
	    padding = (ਪੂਰਨ)(tabcol - ਸ਼ੁਰੂ_ਖਾਨਾ);
	    ਰੋਕੋ;
	}
    }
    ਜੇ (t > tabcount)
	padding = vts[tabcount] - (ਪੂਰਨ)((ਸ਼ੁਰੂ_ਖਾਨਾ - tabcol) % vts[tabcount]);

    // If the space needed is less than the padding no tabs can be used.
    ਜੇ (spaces < padding)
    {
	*ntabs = 0;
	*nspcs = spaces;
	ਵਾਪਸ;
    }

    *ntabs = 1;
    spaces -= padding;

    // At least one tab has been used. See if any more will fit.
    ਜਦੋਂ (spaces != 0 && ++t <= tabcount)
    {
	padding = vts[t];
	ਜੇ (spaces < padding)
	{
	    *nspcs = spaces;
	    ਵਾਪਸ;
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
    ਸਥਿਰ ਪੂਰਨ
tabstop_eq(ਪੂਰਨ *ts1, ਪੂਰਨ *ts2)
{
    ਪੂਰਨ		t;

    ਜੇ ((ts1 == 0 && ts2) || (ts1 && ts2 == 0))
	ਵਾਪਸ FALSE;
    ਜੇ (ts1 == ts2)
	ਵਾਪਸ TRUE;
    ਜੇ (ts1[0] != ts2[0])
	ਵਾਪਸ FALSE;

    ਲਈ (t = 1; t <= ts1[0]; ++t)
	ਜੇ (ts1[t] != ts2[t])
	    ਵਾਪਸ FALSE;

    ਵਾਪਸ TRUE;
}

# ਜੇ defined(FEAT_BEVAL)
/*
 * Copy a tabstop array, allocating space for the new array.
 */
    ਪੂਰਨ *
tabstop_copy(ਪੂਰਨ *oldts)
{
    ਪੂਰਨ		*newts;
    ਪੂਰਨ		t;

    ਜੇ (oldts == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    newts = ALLOC_MULT(ਪੂਰਨ, oldts[0] + 1);
    ਜੇ (newts != ਨਿਰਾ)
	ਲਈ (t = 0; t <= oldts[0]; ++t)
	    newts[t] = oldts[t];
    ਵਾਪਸ newts;
}
# endif

/*
 * Return a count of the number of tabstops.
 */
    ਪੂਰਨ
tabstop_ਗਿਣਤੀ(ਪੂਰਨ *ts)
{
    ਵਾਪਸ ts != ਨਿਰਾ ? ts[0] : 0;
}

/*
 * Return the first tabstop, or 8 if there are no tabstops defined.
 */
    ਪੂਰਨ
tabstop_ਪਹਿਲਾ(ਪੂਰਨ *ts)
{
    ਵਾਪਸ ts != ਨਿਰਾ ? ts[1] : 8;
}

#ਜੇ_ਅੰਤ

/*
 * Return the effective shiftwidth value for current buffer, using the
 * 'tabstop' value when 'shiftwidth' is zero.
 */
    ਲੰਮਾ
get_sw_ਮੁੱਲ(buf_T *buf)
{
    ਵਾਪਸ get_sw_ਮੁੱਲ_ਖਾਨਾ(buf, 0, FALSE);
}

/*
 * Idem, using "pos".
 */
    ਸਥਿਰ ਲੰਮਾ
get_sw_ਮੁੱਲ_pos(buf_T *buf, pos_T *pos, ਪੂਰਨ ਖੱਬਾ)
{
    pos_T save_cursor = curwin->w_cursor;
    ਲੰਮਾ sw_ਮੁੱਲ;

    curwin->w_cursor = *pos;
    sw_ਮੁੱਲ = get_sw_ਮੁੱਲ_ਖਾਨਾ(buf, get_nolist_virtcol(), ਖੱਬਾ);
    curwin->w_cursor = save_cursor;
    ਵਾਪਸ sw_ਮੁੱਲ;
}

/*
 * Idem, using the first non-black in the current line.
 */
    ਲੰਮਾ
get_sw_ਮੁੱਲ_indent(buf_T *buf, ਪੂਰਨ ਖੱਬਾ)
{
    pos_T pos = curwin->w_cursor;

    pos.ਖਾਨਾ = getwhitecols_curline();
    ਵਾਪਸ get_sw_ਮੁੱਲ_pos(buf, &pos, ਖੱਬਾ);
}

/*
 * Idem, using virtual column "col".
 */
    ਲੰਮਾ
get_sw_ਮੁੱਲ_ਖਾਨਾ(buf_T *buf, colnr_T ਖਾਨਾ ਅਣਵਰਤਿਆ, ਪੂਰਨ ਖੱਬਾ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ buf->b_p_sw ? buf->b_p_sw :
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
	tabstop_at(ਖਾਨਾ, buf->b_p_ts, buf->b_p_vts_array, ਖੱਬਾ);
#ਨਹੀਂਤਾਂ
	buf->b_p_ts;
#ਜੇ_ਅੰਤ
}

/*
 * Return the effective softtabstop value for the current buffer, using the
 * 'shiftwidth' value when 'softtabstop' is negative.
 */
    ਲੰਮਾ
get_sts_ਮੁੱਲ(ਖਾਲੀ)
{
    ਵਾਪਸ curbuf->b_p_sts < 0 ? get_sw_ਮੁੱਲ(curbuf) : curbuf->b_p_sts;
}

/*
 * Count the size (in window cells) of the indent in the current line.
 */
    ਪੂਰਨ
get_indent(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
    ਵਾਪਸ get_indent_str_vtab(ml_get_curline(), (ਪੂਰਨ)curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ get_indent_str(ml_get_curline(), (ਪੂਰਨ)curbuf->b_p_ts, FALSE);
#ਜੇ_ਅੰਤ
}

/*
 * Count the size (in window cells) of the indent in line "lnum".
 */
    ਪੂਰਨ
get_indent_lnum(linenr_T lnum)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
    ਵਾਪਸ get_indent_str_vtab(ml_get(lnum), (ਪੂਰਨ)curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ get_indent_str(ml_get(lnum), (ਪੂਰਨ)curbuf->b_p_ts, FALSE);
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(FEAT_FOLDING)
/*
 * Count the size (in window cells) of the indent in line "lnum" of buffer
 * "buf".
 */
    ਪੂਰਨ
get_indent_buf(buf_T *buf, linenr_T lnum)
{
# ifdef FEAT_VARTABS
    ਵਾਪਸ get_indent_str_vtab(ml_get_buf(buf, lnum, FALSE),
			       (ਪੂਰਨ)buf->b_p_ts, buf->b_p_vts_array, FALSE);
# ਨਹੀਂਤਾਂ
    ਵਾਪਸ get_indent_str(ml_get_buf(buf, lnum, FALSE), (ਪੂਰਨ)buf->b_p_ts, FALSE);
# endif
}
#ਜੇ_ਅੰਤ

/*
 * count the size (in window cells) of the indent in line "ptr", with
 * 'tabstop' at "ts"
 */
    ਪੂਰਨ
get_indent_str(
    char_u	*ptr,
    ਪੂਰਨ		ts,
    ਪੂਰਨ		no_ts) // if TRUE, count a tab as ^I
{
    ਪੂਰਨ		ਗਿਣਤੀ_2 = 0;

    ਲਈ ( ; *ptr; ++ptr)
    {
	ਜੇ (*ptr == TAB)    // count a tab for what it is worth
	{
	    ਜੇ (!no_ts)
		ਗਿਣਤੀ_2 += ts - (ਗਿਣਤੀ_2 % ts);
	    ਨਹੀਂਤਾਂ
		ਗਿਣਤੀ_2 += ptr2cells(ptr);
	}
	ਨਹੀਂਤਾਂ ਜੇ (*ptr == ' ')
	    ++ਗਿਣਤੀ_2;		// count a space for one
	ਨਹੀਂਤਾਂ
	    ਰੋਕੋ;
    }
    ਵਾਪਸ ਗਿਣਤੀ_2;
}

#ਜੇ defined(FEAT_VARTABS)
/*
 * Count the size (in window cells) of the indent in line "ptr", using
 * variable tabstops.
 * If "no_ts" is TRUE, count a tab as ^I.
 */
    ਪੂਰਨ
get_indent_str_vtab(char_u *ptr, ਪੂਰਨ ts, ਪੂਰਨ *vts, ਪੂਰਨ no_ts)
{
    ਪੂਰਨ		ਗਿਣਤੀ_2 = 0;

    ਲਈ ( ; *ptr; ++ptr)
    {
	ਜੇ (*ptr == TAB)    // count a tab for what it is worth
	{
	    ਜੇ (!no_ts)
		ਗਿਣਤੀ_2 += tabstop_padding(ਗਿਣਤੀ_2, ts, vts);
	    ਨਹੀਂਤਾਂ
		ਗਿਣਤੀ_2 += ptr2cells(ptr);
	}
	ਨਹੀਂਤਾਂ ਜੇ (*ptr == ' ')
	    ++ਗਿਣਤੀ_2;		// count a space for one
	ਨਹੀਂਤਾਂ
	    ਰੋਕੋ;
    }
    ਵਾਪਸ ਗਿਣਤੀ_2;
}
#ਜੇ_ਅੰਤ

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
    ਪੂਰਨ
ਸੈੱਟ_indent(
    ਪੂਰਨ		ਮਾਪ,		    // measured in spaces
    ਪੂਰਨ		flags)
{
    char_u	*p;
    char_u	*newline;
    char_u	*oldline;
    char_u	*s;
    ਪੂਰਨ		todo;
    ਪੂਰਨ		ind_len;	    // measured in characters
    ਪੂਰਨ		ਸਤਰ_len;	    // size of the line (including the NUL)
    ਪੂਰਨ		doit = FALSE;
    ਪੂਰਨ		ind_ਪੂਰਾ = 0;	    // measured in spaces
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
    ਪੂਰਨ		ind_ਖਾਨਾ = 0;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		tab_pad;
    ਪੂਰਨ		retval = FALSE;
    ਪੂਰਨ		orig_char_len = -1; // number of initial whitespace chars when
				    // 'et' and 'pi' are both set

    // First check if there is anything to do and compute the number of
    // characters needed for the indent.
    todo = ਮਾਪ;
    ind_len = 0;
    p = oldline = ml_get_curline();
    ਸਤਰ_len = ml_get_curline_len() + 1;

    // Calculate the buffer size for the new indent, and check to see if it
    // isn't already set

    // if 'expandtab' isn't set: use TABs; if both 'expandtab' and
    // 'preserveindent' are set count the number of characters at the
    // beginning of the line to be copied
    ਜੇ (!curbuf->b_p_et || (!(flags & SIN_ਪਾਓ) && curbuf->b_p_pi))
    {
	// If 'preserveindent' is set then reuse as much as possible of
	// the existing indent structure for the new indent
	ਜੇ (!(flags & SIN_ਪਾਓ) && curbuf->b_p_pi)
	{
	    ind_ਪੂਰਾ = 0;

	    // count as many characters as we can use
	    ਜਦੋਂ (todo > 0 && VIM_ISWHITE(*p))
	    {
		ਜੇ (*p == TAB)
		{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
		    tab_pad = tabstop_padding(ind_ਪੂਰਾ, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#ਨਹੀਂਤਾਂ
		    tab_pad = (ਪੂਰਨ)curbuf->b_p_ts
					   - (ind_ਪੂਰਾ % (ਪੂਰਨ)curbuf->b_p_ts);
#ਜੇ_ਅੰਤ
		    // stop if this tab will overshoot the target
		    ਜੇ (todo < tab_pad)
			ਰੋਕੋ;
		    todo -= tab_pad;
		    ++ind_len;
		    ind_ਪੂਰਾ += tab_pad;
		}
		ਨਹੀਂਤਾਂ
		{
		    --todo;
		    ++ind_len;
		    ++ind_ਪੂਰਾ;
		}
		++p;
	    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
	    // These diverge from this point.
	    ind_ਖਾਨਾ = ind_ਪੂਰਾ;
#ਜੇ_ਅੰਤ
	    // Set initial number of whitespace chars to copy if we are
	    // preserving indent but expandtab is set
	    ਜੇ (curbuf->b_p_et)
		orig_char_len = ind_len;

	    // Fill to next tabstop with a tab, if possible
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
	    tab_pad = tabstop_padding(ind_ਪੂਰਾ, curbuf->b_p_ts,
						curbuf->b_p_vts_array);
#ਨਹੀਂਤਾਂ
	    tab_pad = (ਪੂਰਨ)curbuf->b_p_ts - (ind_ਪੂਰਾ % (ਪੂਰਨ)curbuf->b_p_ts);
#ਜੇ_ਅੰਤ
	    ਜੇ (todo >= tab_pad && orig_char_len == -1)
	    {
		doit = TRUE;
		todo -= tab_pad;
		++ind_len;
		// ind_done += tab_pad;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
		ind_ਖਾਨਾ += tab_pad;
#ਜੇ_ਅੰਤ
	    }
	}

	// count tabs required for indent
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
	ਲਈ (;;)
	{
	    tab_pad = tabstop_padding(ind_ਖਾਨਾ, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
	    ਜੇ (todo < tab_pad)
		ਰੋਕੋ;
	    ਜੇ (*p != TAB)
		doit = TRUE;
	    ਨਹੀਂਤਾਂ
		++p;
	    todo -= tab_pad;
	    ++ind_len;
	    ind_ਖਾਨਾ += tab_pad;
	}
#ਨਹੀਂਤਾਂ
	ਜਦੋਂ (todo >= (ਪੂਰਨ)curbuf->b_p_ts)
	{
	    ਜੇ (*p != TAB)
		doit = TRUE;
	    ਨਹੀਂਤਾਂ
		++p;
	    todo -= (ਪੂਰਨ)curbuf->b_p_ts;
	    ++ind_len;
	    // ind_done += (int)curbuf->b_p_ts;
	}
#ਜੇ_ਅੰਤ
    }
    // count spaces required for indent
    ਜਦੋਂ (todo > 0)
    {
	ਜੇ (*p != ' ')
	    doit = TRUE;
	ਨਹੀਂਤਾਂ
	    ++p;
	--todo;
	++ind_len;
	// ++ind_done;
    }

    // Return if the indent is OK already.
    ਜੇ (!doit && !VIM_ISWHITE(*p) && !(flags & SIN_ਪਾਓ))
	ਵਾਪਸ FALSE;

    // Allocate memory for the new line.
    ਜੇ (flags & SIN_ਪਾਓ)
	p = oldline;
    ਨਹੀਂਤਾਂ
    {
	p = skipwhite(p);
	ਸਤਰ_len -= (ਪੂਰਨ)(p - oldline);
    }

    // If 'preserveindent' and 'expandtab' are both set keep the original
    // characters and allocate accordingly.  We will fill the rest with spaces
    // after the if (!curbuf->b_p_et) below.
    ਜੇ (orig_char_len != -1)
    {
	newline = alloc(orig_char_len + ਮਾਪ - ind_ਪੂਰਾ + ਸਤਰ_len);
	ਜੇ (newline == ਨਿਰਾ)
	    ਵਾਪਸ FALSE;
	todo = ਮਾਪ - ind_ਪੂਰਾ;
	ind_len = orig_char_len + todo;    // Set total length of indent in
					   // characters, which may have been
					   // undercounted until now
	p = oldline;
	s = newline;
	ਜਦੋਂ (orig_char_len > 0)
	{
	    *s++ = *p++;
	    orig_char_len--;
	}

	// Skip over any additional white space (useful when newindent is less
	// than old)
	ਜਦੋਂ (VIM_ISWHITE(*p))
	    ++p;

    }
    ਨਹੀਂਤਾਂ
    {
	todo = ਮਾਪ;
	newline = alloc(ind_len + ਸਤਰ_len);
	ਜੇ (newline == ਨਿਰਾ)
	    ਵਾਪਸ FALSE;
	s = newline;
    }

    // Put the characters in the new line.
    // if 'expandtab' isn't set: use TABs
    ਜੇ (!curbuf->b_p_et)
    {
	// If 'preserveindent' is set then reuse as much as possible of
	// the existing indent structure for the new indent
	ਜੇ (!(flags & SIN_ਪਾਓ) && curbuf->b_p_pi)
	{
	    p = oldline;
	    ind_ਪੂਰਾ = 0;

	    ਜਦੋਂ (todo > 0 && VIM_ISWHITE(*p))
	    {
		ਜੇ (*p == TAB)
		{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
		    tab_pad = tabstop_padding(ind_ਪੂਰਾ, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#ਨਹੀਂਤਾਂ
		    tab_pad = (ਪੂਰਨ)curbuf->b_p_ts
					   - (ind_ਪੂਰਾ % (ਪੂਰਨ)curbuf->b_p_ts);
#ਜੇ_ਅੰਤ
		    // stop if this tab will overshoot the target
		    ਜੇ (todo < tab_pad)
			ਰੋਕੋ;
		    todo -= tab_pad;
		    ind_ਪੂਰਾ += tab_pad;
		}
		ਨਹੀਂਤਾਂ
		{
		    --todo;
		    ++ind_ਪੂਰਾ;
		}
		*s++ = *p++;
	    }

	    // Fill to next tabstop with a tab, if possible
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
	    tab_pad = tabstop_padding(ind_ਪੂਰਾ, curbuf->b_p_ts,
						curbuf->b_p_vts_array);
#ਨਹੀਂਤਾਂ
	    tab_pad = (ਪੂਰਨ)curbuf->b_p_ts - (ind_ਪੂਰਾ % (ਪੂਰਨ)curbuf->b_p_ts);
#ਜੇ_ਅੰਤ
	    ਜੇ (todo >= tab_pad)
	    {
		*s++ = TAB;
		todo -= tab_pad;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
		ind_ਪੂਰਾ += tab_pad;
#ਜੇ_ਅੰਤ
	    }

	    p = skipwhite(p);
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
	ਲਈ (;;)
	{
	    tab_pad = tabstop_padding(ind_ਪੂਰਾ, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
	    ਜੇ (todo < tab_pad)
		ਰੋਕੋ;
	    *s++ = TAB;
	    todo -= tab_pad;
	    ind_ਪੂਰਾ += tab_pad;
	}
#ਨਹੀਂਤਾਂ
	ਜਦੋਂ (todo >= (ਪੂਰਨ)curbuf->b_p_ts)
	{
	    *s++ = TAB;
	    todo -= (ਪੂਰਨ)curbuf->b_p_ts;
	}
#ਜੇ_ਅੰਤ
    }
    ਜਦੋਂ (todo > 0)
    {
	*s++ = ' ';
	--todo;
    }
    mch_memmove(s, p, (size_t)ਸਤਰ_len);

    // Replace the line (unless undo fails).
    ਜੇ (!(flags & SIN_UNDO) || u_savesub(curwin->w_cursor.lnum) == OK)
    {
	colnr_T old_ਵਿੱਥ = (colnr_T)(p - oldline);
	colnr_T new_ਵਿੱਥ = (colnr_T)(s - newline);

	// this may free "newline"
	ml_replace(curwin->w_cursor.lnum, newline, FALSE);
	ਜੇ (flags & SIN_ਬਦਲਿਆ)
	    ਬਦਲਿਆ_ਬਾਈਟਾਂ(curwin->w_cursor.lnum, 0);

	// Correct saved cursor position if it is in this line.
	ਜੇ (saved_cursor.lnum == curwin->w_cursor.lnum)
	{
	    ਜੇ (saved_cursor.ਖਾਨਾ >= old_ਵਿੱਥ)
		// cursor was after the indent, adjust for the number of
		// bytes added/removed
		saved_cursor.ਖਾਨਾ += ind_len - old_ਵਿੱਥ;
	    ਨਹੀਂਤਾਂ ਜੇ (saved_cursor.ਖਾਨਾ >= new_ਵਿੱਥ)
		// cursor was in the indent, and is now after it, put it back
		// at the start of the indent (replacing spaces with TAB)
		saved_cursor.ਖਾਨਾ = new_ਵਿੱਥ;
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	ਜੇ (curbuf->b_has_textprop)
	{
	    ਪੂਰਨ added = ind_len - old_ਵਿੱਥ;

	    // When increasing indent this behaves like spaces were inserted at
	    // the old indent, when decreasing indent it behaves like spaces
	    // were deleted at the new indent.
	    adjust_prop_columns(curwin->w_cursor.lnum,
				  added > 0 ? old_ਵਿੱਥ : (colnr_T)ind_len,
				  added, APC_INDENT);
	}
#ਜੇ_ਅੰਤ
	retval = TRUE;
    }
    ਨਹੀਂਤਾਂ
	vim_free(newline);

    curwin->w_cursor.ਖਾਨਾ = ind_len;
    ਵਾਪਸ retval;
}

/*
 * Return the indent of the current line after a number.  Return -1 if no
 * number was found.  Used for 'n' in 'formatoptions': numbered list.
 * Since a pattern is used it can actually handle more than numbers.
 */
    ਪੂਰਨ
get_ਸੰਖਿਆ_indent(linenr_T lnum)
{
    colnr_T	ਖਾਨਾ;
    pos_T	pos;

    regmatch_T	regmatch;
    ਪੂਰਨ		lead_len = 0;	// length of comment leader

    ਜੇ (lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	ਵਾਪਸ -1;
    pos.lnum = 0;

    // In format_lines() (i.e. not insert mode), fo+=q is needed too...
    ਜੇ ((State & ਬਹੁਲਕ_ਪਾਓ) || has_format_option(FO_Q_COMS))
	lead_len = get_leader_len(ml_get(lnum), ਨਿਰਾ, FALSE, TRUE);

    regmatch.regprog = vim_regcomp(curbuf->b_p_flp, RE_MAGIC);
    ਜੇ (regmatch.regprog != ਨਿਰਾ)
    {
	regmatch.rm_ic = FALSE;

	// vim_regexec() expects a pointer to a line.  This lets us
	// start matching for the flp beyond any comment leader...
	ਜੇ (vim_regexec(&regmatch, ml_get(lnum) + lead_len, (colnr_T)0))
	{
	    pos.lnum = lnum;
	    pos.ਖਾਨਾ = (colnr_T)(*regmatch.endp - ml_get(lnum));
	    pos.coladd = 0;
	}
	vim_regfree(regmatch.regprog);
    }

    ਜੇ (pos.lnum == 0 || *ml_get_pos(&pos) == NUL)
	ਵਾਪਸ -1;
    getvcol(curwin, &pos, &ਖਾਨਾ, ਨਿਰਾ, ਨਿਰਾ, 0);
    ਵਾਪਸ (ਪੂਰਨ)ਖਾਨਾ;
}

#ਜੇ defined(FEAT_LINEBREAK)
/*
 * Check "briopt" as 'breakindentopt' and update the members of "wp".
 * This is called when 'breakindentopt' is changed and when a window is
 * initialized.
 * Returns FAIL for failure, OK otherwise.
 */
    ਪੂਰਨ
briopt_check(
    char_u	*briopt,	// when NULL: use "wp->w_p_briopt"
    win_T	*wp)		// when NULL: only check "briopt"
{
    char_u	*p;
    ਪੂਰਨ		bri_ਖਿਸਕਾਓ = 0;
    ਲੰਮਾ	bri_min = 20;
    ਪੂਰਨ		bri_sbr = FALSE;
    ਪੂਰਨ		bri_list = 0;
    ਪੂਰਨ		bri_vcol = 0;

    ਜੇ (briopt != ਨਿਰਾ)
	p = briopt;
    ਨਹੀਂਤਾਂ
	p = wp->w_p_briopt;

    ਜਦੋਂ (*p != NUL)
    {
	// Note: Keep this in sync with p_briopt_values
	ਜੇ (STRNCMP(p, "shift:", 6) == 0
		 && ((p[6] == '-' && VIM_ISDIGIT(p[7])) || VIM_ISDIGIT(p[6])))
	{
	    p += 6;
	    bri_ਖਿਸਕਾਓ = getdigits(&p);
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "min:", 4) == 0 && VIM_ISDIGIT(p[4]))
	{
	    p += 4;
	    bri_min = getdigits(&p);
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "sbr", 3) == 0)
	{
	    p += 3;
	    bri_sbr = TRUE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "list:", 5) == 0)
	{
	    p += 5;
	    bri_list = getdigits(&p);
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "column:", 7) == 0)
	{
	    p += 7;
	    bri_vcol = getdigits(&p);
	}
	ਜੇ (*p != ',' && *p != NUL)
	    ਵਾਪਸ FAIL;
	ਜੇ (*p == ',')
	    ++p;
    }

    ਜੇ (wp == ਨਿਰਾ)
	ਵਾਪਸ OK;

    wp->w_briopt_ਖਿਸਕਾਓ = bri_ਖਿਸਕਾਓ;
    wp->w_briopt_min   = bri_min;
    wp->w_briopt_sbr   = bri_sbr;
    wp->w_briopt_list  = bri_list;
    wp->w_briopt_vcol  = bri_vcol;

    ਵਾਪਸ OK;
}

/*
 * Return appropriate space number for breakindent, taking influencing
 * parameters into account. Window must be specified, since it is not
 * necessarily always the current one.
 */
    ਪੂਰਨ
get_breakindent_win(
    win_T	*wp,
    char_u	*ਸਤਰ) // start of the line
{
    ਸਥਿਰ ਪੂਰਨ	    prev_indent = 0;	// cached indent value
    ਸਥਿਰ ਲੰਮਾ	    prev_ts     = 0L;	// cached tabstop value
# ifdef FEAT_VARTABS
    ਸਥਿਰ ਪੂਰਨ      *prev_vts = ਨਿਰਾ;   // cached vartabs values
# endif
    ਸਥਿਰ ਪੂਰਨ	    prev_fnum   = 0;	// cached buffer number
    ਸਥਿਰ char_u   *prev_ਸਤਰ  = ਨਿਰਾ;	// cached copy of "line"
    ਸਥਿਰ varnumber_T prev_tick = 0;   // changedtick of cached value
    ਸਥਿਰ ਪੂਰਨ      prev_list = 0;	// cached list indent
    ਸਥਿਰ ਪੂਰਨ      prev_listopt = 0;	// cached w_p_briopt_list value
    ਸਥਿਰ ਪੂਰਨ      prev_no_ts = FALSE;	// cached no_ts value
    ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ prev_dy_uhex = 0;	// cached 'display' "uhex" value
    // cached formatlistpat value
    ਸਥਿਰ char_u   *prev_flp = ਨਿਰਾ;
    ਪੂਰਨ		    bri = 0;
    // window width minus window margin space, i.e. what rests for text
    ਅਚਲ ਪੂਰਨ	    eff_wwidth = wp->w_ਚੌੜਾਈ
					  - win_ਖਾਨਾ_off(wp) + win_ਖਾਨਾ_off2(wp);

    // In list mode, if 'listchars' "tab" isn't set, a TAB is displayed as ^I.
    ਪੂਰਨ no_ts = wp->w_p_list && wp->w_lcs_chars.tab1 == NUL;

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
    ਜੇ (prev_fnum != wp->w_ਬਫਰ->b_fnum
	    || prev_ts != wp->w_ਬਫਰ->b_p_ts
# ifdef FEAT_VARTABS
	    || prev_vts != wp->w_ਬਫਰ->b_p_vts_array
# endif
	    || prev_tick != CHANGEDTICK(wp->w_ਬਫਰ)
	    || prev_listopt != wp->w_briopt_list
	    || prev_no_ts != no_ts
	    || prev_dy_uhex != (dy_flags & DY_UHEX)
	    || prev_flp == ਨਿਰਾ
	    || STRCMP(prev_flp, get_flp_ਮੁੱਲ(wp->w_ਬਫਰ)) != 0
	    || prev_ਸਤਰ == ਨਿਰਾ || STRCMP(prev_ਸਤਰ, ਸਤਰ) != 0
	)
    {
	prev_fnum = wp->w_ਬਫਰ->b_fnum;
	vim_free(prev_ਸਤਰ);
	prev_ਸਤਰ = vim_strsave(ਸਤਰ);
	prev_ts = wp->w_ਬਫਰ->b_p_ts;
# ifdef FEAT_VARTABS
	prev_vts = wp->w_ਬਫਰ->b_p_vts_array;
	ਜੇ (wp->w_briopt_vcol == 0)
	    prev_indent = get_indent_str_vtab(ਸਤਰ,
				     (ਪੂਰਨ)wp->w_ਬਫਰ->b_p_ts,
				      wp->w_ਬਫਰ->b_p_vts_array, no_ts);
# ਨਹੀਂਤਾਂ
	ਜੇ (wp->w_briopt_vcol == 0)
	    prev_indent = get_indent_str(ਸਤਰ,
					(ਪੂਰਨ)wp->w_ਬਫਰ->b_p_ts, no_ts);
# endif
	prev_tick = CHANGEDTICK(wp->w_ਬਫਰ);
	prev_listopt = wp->w_briopt_list;
	prev_list = 0;
	prev_no_ts = no_ts;
	prev_dy_uhex = (dy_flags & DY_UHEX);
	vim_free(prev_flp);
	prev_flp = vim_strsave(get_flp_ਮੁੱਲ(wp->w_ਬਫਰ));
	// add additional indent for numbered lists
	ਜੇ (wp->w_briopt_list != 0 && wp->w_briopt_vcol == 0)
	{
	    regmatch_T	    regmatch;

	    regmatch.regprog = vim_regcomp(prev_flp,
				   RE_MAGIC + RE_STRING + RE_AUTO + RE_STRICT);

	    ਜੇ (regmatch.regprog != ਨਿਰਾ)
	    {
		regmatch.rm_ic = FALSE;
		ਜੇ (vim_regexec(&regmatch, ਸਤਰ, 0))
		{
		    ਜੇ (wp->w_briopt_list > 0)
			prev_list = wp->w_briopt_list;
		    ਨਹੀਂਤਾਂ
		    {
			char_u	*ptr = *regmatch.startp;
			char_u	*end_ptr = *regmatch.endp;
			ਪੂਰਨ	indent = 0;

			// Compute the width of the matched text.
			// Use win_chartabsize() so that TAB size is correct,
			// while wrapping is ignored.
			ਜਦੋਂ (ptr < end_ptr)
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
    ਜੇ (wp->w_briopt_vcol != 0)
    {
	// column value has priority
	bri = wp->w_briopt_vcol;
	prev_list = 0;
    }
    ਨਹੀਂਤਾਂ
	bri = prev_indent + wp->w_briopt_ਖਿਸਕਾਓ;

    // Add offset for number column, if 'n' is in 'cpoptions'
    bri += win_ਖਾਨਾ_off2(wp);

    // add additional indent for numbered lists
    ਜੇ (wp->w_briopt_list > 0)
	bri += prev_list;

    // indent minus the length of the showbreak string
    ਜੇ (wp->w_briopt_sbr)
	bri -= vim_strsize(get_showbreak_ਮੁੱਲ(wp));


    // never indent past left window margin
    ਜੇ (bri < 0)
	bri = 0;

    // always leave at least bri_min characters on the left,
    // if text width is sufficient
    ਨਹੀਂਤਾਂ ਜੇ (bri > eff_wwidth - wp->w_briopt_min)
	bri = (eff_wwidth - wp->w_briopt_min < 0)
					   ? 0 : eff_wwidth - wp->w_briopt_min;

    ਵਾਪਸ bri;
}
#ਜੇ_ਅੰਤ

/*
 * When extra == 0: Return TRUE if the cursor is before or on the first
 *		    non-blank in the line.
 * When extra == 1: Return TRUE if the cursor is before the first non-blank in
 *		    the line.
 */
    ਪੂਰਨ
inindent(ਪੂਰਨ extra)
{
    char_u	*ptr;
    colnr_T	ਖਾਨਾ;

    ਲਈ (ਖਾਨਾ = 0, ptr = ml_get_curline(); VIM_ISWHITE(*ptr); ++ਖਾਨਾ)
	++ptr;
    ਜੇ (ਖਾਨਾ >= curwin->w_cursor.ਖਾਨਾ + extra)
	ਵਾਪਸ TRUE;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ FALSE;
}

/*
 * op_reindent - handle reindenting a block of lines.
 */
    ਖਾਲੀ
op_reindent(oparg_T *oap, ਪੂਰਨ (*how)(ਖਾਲੀ))
{
    ਲੰਮਾ	i = 0;
    char_u	*l;
    ਪੂਰਨ		amount;
    linenr_T	ਪਹਿਲਾ_ਬਦਲਿਆ = 0;
    linenr_T	ਆਖਰੀ_ਬਦਲਿਆ = 0;
    linenr_T	ਸ਼ੁਰੂ_lnum = curwin->w_cursor.lnum;

    // Don't even try when 'modifiable' is off.
    ਜੇ (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	ਵਾਪਸ;
    }

    // Save for undo.  Do this once for all lines, much faster than doing this
    // for each line separately, especially when undoing.
    ਜੇ (u_savecommon(ਸ਼ੁਰੂ_lnum - 1, ਸ਼ੁਰੂ_lnum + oap->ਸਤਰ_ਗਿਣਤੀ,
				     ਸ਼ੁਰੂ_lnum + oap->ਸਤਰ_ਗਿਣਤੀ, FALSE) == OK)
	ਲਈ (i = oap->ਸਤਰ_ਗਿਣਤੀ; --i >= 0 && !got_int; )
	{
	    // it's a slow thing to do, so give feedback so there's no worry
	    // that the computer's just hung.

	    ਜੇ (i > 1
		    && (i % 50 == 0 || i == oap->ਸਤਰ_ਗਿਣਤੀ - 1)
		    && oap->ਸਤਰ_ਗਿਣਤੀ > p_report)
		smsg(_("%ld lines to indent... "), i);

	    // Be vi-compatible: For lisp indenting the first line is not
	    // indented, unless there is only one line.
	    ਜੇ (i != oap->ਸਤਰ_ਗਿਣਤੀ - 1 || oap->ਸਤਰ_ਗਿਣਤੀ == 1
						     || how != get_lisp_indent)
	    {
		l = skipwhite(ml_get_curline());
		ਜੇ (*l == NUL)		    // empty or blank line
		    amount = 0;
		ਨਹੀਂਤਾਂ
		    amount = how();	    // get the indent for this line

		ਜੇ (amount >= 0 && ਸੈੱਟ_indent(amount, 0))
		{
		    // did change the indent, call changed_lines() later
		    ਜੇ (ਪਹਿਲਾ_ਬਦਲਿਆ == 0)
			ਪਹਿਲਾ_ਬਦਲਿਆ = curwin->w_cursor.lnum;
		    ਆਖਰੀ_ਬਦਲਿਆ = curwin->w_cursor.lnum;
		}
	    }
	    ++curwin->w_cursor.lnum;
	    curwin->w_cursor.ਖਾਨਾ = 0;  // make sure it's valid
	}

    // put cursor on first non-blank of indented line
    curwin->w_cursor.lnum = ਸ਼ੁਰੂ_lnum;
    beginline(BL_SOL | BL_FIX);

    // Mark changed lines so that they will be redrawn.  When Visual
    // highlighting was present, need to continue until the last line.  When
    // there is no change still need to remove the Visual highlighting.
    ਜੇ (ਆਖਰੀ_ਬਦਲਿਆ != 0)
	ਬਦਲਿਆ_ਸਤਰਾਂ(ਪਹਿਲਾ_ਬਦਲਿਆ, 0,
		oap->is_VIsual ? ਸ਼ੁਰੂ_lnum + oap->ਸਤਰ_ਗਿਣਤੀ :
		ਆਖਰੀ_ਬਦਲਿਆ + 1, 0L);
    ਨਹੀਂਤਾਂ ਜੇ (oap->is_VIsual)
	redraw_curbuf_later(UPD_INVERTED);

    ਜੇ (oap->ਸਤਰ_ਗਿਣਤੀ > p_report)
    {
	i = oap->ਸਤਰ_ਗਿਣਤੀ - (i + 1);
	smsg(NGETTEXT("%ld line indented ",
						 "%ld lines indented ", i), i);
    }
    ਜੇ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// set '[ and '] marks
	curbuf->b_op_ਸ਼ੁਰੂ = oap->ਸ਼ੁਰੂ;
	curbuf->b_op_ਸਿਰਾ = oap->ਸਿਰਾ;
    }
}

/*
 * Return TRUE if lines starting with '#' should be left aligned.
 */
    ਪੂਰਨ
preprocs_ਖੱਬਾ(ਖਾਲੀ)
{
    ਵਾਪਸ
	(curbuf->b_p_si && !curbuf->b_p_cin) ||
	(curbuf->b_p_cin && ਵਿੱਚ_cinkeys('#', ' ', TRUE)
					   && curbuf->b_ind_hash_comment == 0)
	;
}

/*
 * Return TRUE if the conditions are OK for smart indenting.
 */
    ਪੂਰਨ
may_do_si(ਖਾਲੀ)
{
    ਵਾਪਸ curbuf->b_p_si
	&& !curbuf->b_p_cin
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	&& *curbuf->b_p_inde == NUL
#ਜੇ_ਅੰਤ
	&& !p_paste;
}

/*
 * Try to do some very smart auto-indenting.
 * Used when inserting a "normal" character.
 */
    ਖਾਲੀ
ins_try_si(ਪੂਰਨ c)
{
    pos_T	*pos, old_pos;
    char_u	*ptr;
    ਪੂਰਨ		i;
    ਪੂਰਨ		ਆਰਜ਼ੀ;

    // do some very smart indenting when entering '{' or '}'
    ਜੇ (((did_si || can_si_ਪਿੱਠ) && c == '{')
	    || (can_si && c == '}' && inindent(0)))
    {
	// for '}' set indent equal to indent of line containing matching '{'
	ਜੇ (c == '}' && (pos = findmatch(ਨਿਰਾ, '{')) != ਨਿਰਾ)
	{
	    old_pos = curwin->w_cursor;
	    // If the matching '{' has a ')' immediately before it (ignoring
	    // white-space), then line up with the start of the line
	    // containing the matching '(' if there is one.  This handles the
	    // case where an "if (..\n..) {" statement continues over multiple
	    // lines -- webb
	    ptr = ml_get(pos->lnum);
	    i = pos->ਖਾਨਾ;
	    ਜੇ (i > 0)		// skip blanks before '{'
		ਜਦੋਂ (--i > 0 && VIM_ISWHITE(ptr[i]))
		    ;
	    curwin->w_cursor.lnum = pos->lnum;
	    curwin->w_cursor.ਖਾਨਾ = i;
	    ਜੇ (ptr[i] == ')' && (pos = findmatch(ਨਿਰਾ, '(')) != ਨਿਰਾ)
		curwin->w_cursor = *pos;
	    i = get_indent();
	    curwin->w_cursor = old_pos;
	    ਜੇ (State & VREPLACE_FLAG)
		change_indent(INDENT_ਸੈੱਟ, i, FALSE, NUL, TRUE);
	    ਨਹੀਂਤਾਂ
		(ਖਾਲੀ)ਸੈੱਟ_indent(i, SIN_ਬਦਲਿਆ);
	}
	ਨਹੀਂਤਾਂ ਜੇ (curwin->w_cursor.ਖਾਨਾ > 0)
	{
	    // when inserting '{' after "O" reduce indent, but not
	    // more than indent of previous line
	    ਆਰਜ਼ੀ = TRUE;
	    ਜੇ (c == '{' && can_si_ਪਿੱਠ && curwin->w_cursor.lnum > 1)
	    {
		old_pos = curwin->w_cursor;
		i = get_indent();
		ਜਦੋਂ (curwin->w_cursor.lnum > 1)
		{
		    ptr = skipwhite(ml_get(--(curwin->w_cursor.lnum)));

		    // ignore empty lines and lines starting with '#'.
		    ਜੇ (*ptr != '#' && *ptr != NUL)
			ਰੋਕੋ;
		}
		ਜੇ (get_indent() >= i)
		    ਆਰਜ਼ੀ = FALSE;
		curwin->w_cursor = old_pos;
	    }
	    ਜੇ (ਆਰਜ਼ੀ)
		ਖਿਸਕਾਓ_ਸਤਰ(TRUE, FALSE, 1, TRUE);
	}
    }

    // set indent of '#' always to 0
    ਜੇ (curwin->w_cursor.ਖਾਨਾ > 0 && can_si && c == '#' && inindent(0))
    {
	// remember current indent for next line
	old_indent = get_indent();
	(ਖਾਲੀ)ਸੈੱਟ_indent(0, SIN_ਬਦਲਿਆ);
    }

    // Adjust ai_col, the char at this position can be deleted.
    ਜੇ (ai_ਖਾਨਾ > curwin->w_cursor.ਖਾਨਾ)
	ai_ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;
}

/*
 * Insert an indent (for <Tab> or CTRL-T) or delete an indent (for CTRL-D).
 * Keep the cursor on the same character.
 * type == INDENT_INC	increase indent (for CTRL-T or <Tab>)
 * type == INDENT_DEC	decrease indent (for CTRL-D)
 * type == INDENT_SET	set indent to "amount"
 * if round is TRUE, round the indent to 'shiftwidth' (only with _INC and _Dec).
 */
    ਖਾਲੀ
change_indent(
    ਪੂਰਨ		type,
    ਪੂਰਨ		amount,
    ਪੂਰਨ		ਗੇੜਾ,
    ਪੂਰਨ		replaced,	// replaced character, put on replace stack
    ਪੂਰਨ		call_ਬਦਲਿਆ_ਬਾਈਟਾਂ)	// call changed_bytes()
{
    ਪੂਰਨ		vcol;
    ਪੂਰਨ		ਆਖਰੀ_vcol;
    ਪੂਰਨ		insstart_less;		// reduction for Insstart.col
    ਪੂਰਨ		new_cursor_ਖਾਨਾ;
    ਪੂਰਨ		i;
    char_u	*ptr;
    ਪੂਰਨ		save_p_list;
    ਪੂਰਨ		ਸ਼ੁਰੂ_ਖਾਨਾ;
    colnr_T	vc;
    colnr_T	orig_ਖਾਨਾ = 0;		// init for GCC
    char_u	*new_ਸਤਰ, *orig_ਸਤਰ = ਨਿਰਾ;	// init for GCC

    // MODE_VREPLACE state needs to know what the line was like before changing
    ਜੇ (State & VREPLACE_FLAG)
    {
	orig_ਸਤਰ = vim_strnsave(ml_get_curline(), ml_get_curline_len());  // Deal with NULL below
	orig_ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;
    }

    // for the following tricks we don't want list mode
    save_p_list = curwin->w_p_list;
    curwin->w_p_list = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ignore_ਪਾਠ_props = TRUE;
#ਜੇ_ਅੰਤ
    vc = getvcol_nolist(&curwin->w_cursor);
    vcol = vc;

    // For Replace mode we need to fix the replace stack later, which is only
    // possible when the cursor is in the indent.  Remember the number of
    // characters before the cursor if it's possible.
    ਸ਼ੁਰੂ_ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;

    // determine offset from first non-blank
    new_cursor_ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;
    beginline(BL_WHITE);
    new_cursor_ਖਾਨਾ -= curwin->w_cursor.ਖਾਨਾ;

    insstart_less = curwin->w_cursor.ਖਾਨਾ;

    // If the cursor is in the indent, compute how many screen columns the
    // cursor is to the left of the first non-blank.
    ਜੇ (new_cursor_ਖਾਨਾ < 0)
	vcol = get_indent() - vcol;

    ਜੇ (new_cursor_ਖਾਨਾ > 0)	    // can't fix replace stack
	ਸ਼ੁਰੂ_ਖਾਨਾ = -1;

    // Set the new indent.  The cursor will be put on the first non-blank.
    ਜੇ (type == INDENT_ਸੈੱਟ)
	(ਖਾਲੀ)ਸੈੱਟ_indent(amount, call_ਬਦਲਿਆ_ਬਾਈਟਾਂ ? SIN_ਬਦਲਿਆ : 0);
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ	save_State = State;

	// Avoid being called recursively.
	ਜੇ (State & VREPLACE_FLAG)
	    State = ਬਹੁਲਕ_ਪਾਓ;
	ਖਿਸਕਾਓ_ਸਤਰ(type == INDENT_DEC, ਗੇੜਾ, 1, call_ਬਦਲਿਆ_ਬਾਈਟਾਂ);
	State = save_State;
    }
    insstart_less -= curwin->w_cursor.ਖਾਨਾ;

    // Try to put cursor on same character.
    // If the cursor is at or after the first non-blank in the line,
    // compute the cursor column relative to the column of the first
    // non-blank character.
    // If we are not in insert mode, leave the cursor on the first non-blank.
    // If the cursor is before the first non-blank, position it relative
    // to the first non-blank, counted in screen columns.
    ਜੇ (new_cursor_ਖਾਨਾ >= 0)
    {
	// When changing the indent while the cursor is touching it, reset
	// Insstart_col to 0.
	ਜੇ (new_cursor_ਖਾਨਾ == 0)
	    insstart_less = MAXCOL;
	new_cursor_ਖਾਨਾ += curwin->w_cursor.ਖਾਨਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (!(State & ਬਹੁਲਕ_ਪਾਓ))
	new_cursor_ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;
    ਨਹੀਂਤਾਂ
    {
	chartabsize_T cts;

	// Compute the screen column where the cursor should be.
	vcol = get_indent() - vcol;
	curwin->w_virtcol = (colnr_T)((vcol < 0) ? 0 : vcol);

	// Advance the cursor until we reach the right screen column.
	ਆਖਰੀ_vcol = 0;
	ptr = ml_get_curline();
	init_chartabsize_arg(&cts, curwin, 0, 0, ptr, ptr);
	ਜਦੋਂ (cts.cts_vcol <= (ਪੂਰਨ)curwin->w_virtcol)
	{
	    ਆਖਰੀ_vcol = cts.cts_vcol;
	    ਜੇ (cts.cts_vcol > 0)
		MB_PTR_ADV(cts.cts_ptr);
	    ਜੇ (*cts.cts_ptr == NUL)
		ਰੋਕੋ;
	    cts.cts_vcol += lbr_chartabsize(&cts);
	}
	vcol = ਆਖਰੀ_vcol;
	new_cursor_ਖਾਨਾ = cts.cts_ptr - cts.cts_ਸਤਰ;
	clear_chartabsize_arg(&cts);

	// May need to insert spaces to be able to position the cursor on
	// the right screen column.
	ਜੇ (vcol != (ਪੂਰਨ)curwin->w_virtcol)
	{
	    curwin->w_cursor.ਖਾਨਾ = (colnr_T)new_cursor_ਖਾਨਾ;
	    i = (ਪੂਰਨ)curwin->w_virtcol - vcol;
	    ptr = alloc(i + 1);
	    ਜੇ (ptr != ਨਿਰਾ)
	    {
		size_t ptrlen;
		new_cursor_ਖਾਨਾ += i;
		ptr[i] = NUL;
		ptrlen = i;
		ਜਦੋਂ (--i >= 0)
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

    ਜੇ (new_cursor_ਖਾਨਾ <= 0)
	curwin->w_cursor.ਖਾਨਾ = 0;
    ਨਹੀਂਤਾਂ
	curwin->w_cursor.ਖਾਨਾ = (colnr_T)new_cursor_ਖਾਨਾ;
    curwin->w_ਸੈੱਟ_curswant = true;
    ਬਦਲਿਆ_cline_bef_curs();

    // May have to adjust the start of the insert.
    ਜੇ (State & ਬਹੁਲਕ_ਪਾਓ)
    {
	ਜੇ (curwin->w_cursor.lnum == Insstart.lnum && Insstart.ਖਾਨਾ != 0)
	{
	    ਜੇ ((ਪੂਰਨ)Insstart.ਖਾਨਾ <= insstart_less)
		Insstart.ਖਾਨਾ = 0;
	    ਨਹੀਂਤਾਂ
		Insstart.ਖਾਨਾ -= insstart_less;
	}
	ਜੇ ((ਪੂਰਨ)ai_ਖਾਨਾ <= insstart_less)
	    ai_ਖਾਨਾ = 0;
	ਨਹੀਂਤਾਂ
	    ai_ਖਾਨਾ -= insstart_less;
    }

    // For MODE_REPLACE state, may have to fix the replace stack, if it's
    // possible.  If the number of characters before the cursor decreased, need
    // to pop a few characters from the replace stack.
    // If the number of characters before the cursor increased, need to push a
    // few NULs onto the replace stack.
    ਜੇ (REPLACE_ਸਾਧਾਰਨ(State) && ਸ਼ੁਰੂ_ਖਾਨਾ >= 0)
    {
	ਜਦੋਂ (ਸ਼ੁਰੂ_ਖਾਨਾ > (ਪੂਰਨ)curwin->w_cursor.ਖਾਨਾ)
	{
	    replace_join(0);	    // remove a NUL from the replace stack
	    --ਸ਼ੁਰੂ_ਖਾਨਾ;
	}
	ਜਦੋਂ (ਸ਼ੁਰੂ_ਖਾਨਾ < (ਪੂਰਨ)curwin->w_cursor.ਖਾਨਾ || replaced)
	{
	    replace_ਧੱਕੋ(NUL);
	    ਜੇ (replaced)
	    {
		replace_ਧੱਕੋ(replaced);
		replaced = NUL;
	    }
	    ++ਸ਼ੁਰੂ_ਖਾਨਾ;
	}
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ignore_ਪਾਠ_props = FALSE;
#ਜੇ_ਅੰਤ

    // For MODE_VREPLACE state, we also have to fix the replace stack.  In this
    // case it is always possible because we backspace over the whole line and
    // then put it back again the way we wanted it.
    ਜੇ (State & VREPLACE_FLAG)
    {
	// If orig_line didn't allocate, just return.  At least we did the job,
	// even if you can't backspace.
	ਜੇ (orig_ਸਤਰ == ਨਿਰਾ)
	    ਵਾਪਸ;

	// Save new line
	new_ਸਤਰ = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	ਜੇ (new_ਸਤਰ == ਨਿਰਾ)
	{
	    vim_free(orig_ਸਤਰ);
	    ਵਾਪਸ;
	}

	// We only put back the new line up to the cursor
	new_ਸਤਰ[curwin->w_cursor.ਖਾਨਾ] = NUL;

	// Put back original line
	ml_replace(curwin->w_cursor.lnum, orig_ਸਤਰ, FALSE);
	curwin->w_cursor.ਖਾਨਾ = orig_ਖਾਨਾ;

	// Backspace from cursor to start of line
	backspace_until_ਕਾਲਮ(0);

	// Insert new stuff into line again
	ins_ਬਾਈਟਾਂ(new_ਸਤਰ);

	vim_free(new_ਸਤਰ);
    }
}

/*
 * Copy the indent from ptr to the current line (and fill to size)
 * Leaves the cursor on the first non-blank in the line.
 * Returns TRUE if the line was changed.
 */
    ਪੂਰਨ
copy_indent(ਪੂਰਨ ਮਾਪ, char_u *src)
{
    char_u	*p = ਨਿਰਾ;
    char_u	*ਸਤਰ = ਨਿਰਾ;
    char_u	*s;
    ਪੂਰਨ		todo;
    ਪੂਰਨ		ind_len;
    ਪੂਰਨ		ਸਤਰ_len = 0;
    ਪੂਰਨ		tab_pad;
    ਪੂਰਨ		ind_ਪੂਰਾ;
    ਪੂਰਨ		ਗੇੜਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
    ਪੂਰਨ		ind_ਖਾਨਾ;
#ਜੇ_ਅੰਤ

    // Round 1: compute the number of characters needed for the indent
    // Round 2: copy the characters.
    ਲਈ (ਗੇੜਾ = 1; ਗੇੜਾ <= 2; ++ਗੇੜਾ)
    {
	todo = ਮਾਪ;
	ind_len = 0;
	ind_ਪੂਰਾ = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
	ind_ਖਾਨਾ = 0;
#ਜੇ_ਅੰਤ
	s = src;

	// Count/copy the usable portion of the source line
	ਜਦੋਂ (todo > 0 && VIM_ISWHITE(*s))
	{
	    ਜੇ (*s == TAB)
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
		tab_pad = tabstop_padding(ind_ਪੂਰਾ, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#ਨਹੀਂਤਾਂ
		tab_pad = (ਪੂਰਨ)curbuf->b_p_ts
					   - (ind_ਪੂਰਾ % (ਪੂਰਨ)curbuf->b_p_ts);
#ਜੇ_ਅੰਤ
		// Stop if this tab will overshoot the target
		ਜੇ (todo < tab_pad)
		    ਰੋਕੋ;
		todo -= tab_pad;
		ind_ਪੂਰਾ += tab_pad;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
		ind_ਖਾਨਾ += tab_pad;
#ਜੇ_ਅੰਤ
	    }
	    ਨਹੀਂਤਾਂ
	    {
		--todo;
		++ind_ਪੂਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
		++ind_ਖਾਨਾ;
#ਜੇ_ਅੰਤ
	    }
	    ++ind_len;
	    ਜੇ (p != ਨਿਰਾ)
		*p++ = *s;
	    ++s;
	}

	// Fill to next tabstop with a tab, if possible
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
	tab_pad = tabstop_padding(ind_ਪੂਰਾ, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#ਨਹੀਂਤਾਂ
	tab_pad = (ਪੂਰਨ)curbuf->b_p_ts - (ind_ਪੂਰਾ % (ਪੂਰਨ)curbuf->b_p_ts);
#ਜੇ_ਅੰਤ
	ਜੇ (todo >= tab_pad && !curbuf->b_p_et)
	{
	    todo -= tab_pad;
	    ++ind_len;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
	    ind_ਖਾਨਾ += tab_pad;
#ਜੇ_ਅੰਤ
	    ਜੇ (p != ਨਿਰਾ)
		*p++ = TAB;
	}

	// Add tabs required for indent
	ਜੇ (!curbuf->b_p_et)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
	    ਲਈ (;;)
	    {
		tab_pad = tabstop_padding(ind_ਖਾਨਾ, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
		ਜੇ (todo < tab_pad)
		    ਰੋਕੋ;
		todo -= tab_pad;
		++ind_len;
		ind_ਖਾਨਾ += tab_pad;
		ਜੇ (p != ਨਿਰਾ)
		    *p++ = TAB;
	    }
#ਨਹੀਂਤਾਂ
	    ਜਦੋਂ (todo >= (ਪੂਰਨ)curbuf->b_p_ts)
	    {
		todo -= (ਪੂਰਨ)curbuf->b_p_ts;
		++ind_len;
		ਜੇ (p != ਨਿਰਾ)
		    *p++ = TAB;
	    }
#ਜੇ_ਅੰਤ
	}

	// Count/add spaces required for indent
	ਜਦੋਂ (todo > 0)
	{
	    --todo;
	    ++ind_len;
	    ਜੇ (p != ਨਿਰਾ)
		*p++ = ' ';
	}

	ਜੇ (p == ਨਿਰਾ)
	{
	    // Allocate memory for the result: the copied indent, new indent
	    // and the rest of the line.
	    ਸਤਰ_len = ml_get_curline_len() + 1;
	    ਸਤਰ = alloc(ind_len + ਸਤਰ_len);
	    ਜੇ (ਸਤਰ == ਨਿਰਾ)
		ਵਾਪਸ FALSE;
	    p = ਸਤਰ;
	}
    }

    // Append the original line
    mch_memmove(p, ml_get_curline(), (size_t)ਸਤਰ_len);

    // Replace the line
    ml_replace(curwin->w_cursor.lnum, ਸਤਰ, FALSE);

    // Put the cursor after the indent.
    curwin->w_cursor.ਖਾਨਾ = ind_len;
    ਵਾਪਸ TRUE;
}

/*
 * Give a "resulting text too long" error and maybe set got_int.
 */
    ਸਥਿਰ ਖਾਲੀ
emsg_ਪਾਠ_too_long(ਖਾਲੀ)
{
    emsg(_(e_resulting_ਪਾਠ_too_long));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // when not inside a try/catch set got_int to break out of any loop
    ਜੇ (trylevel == 0)
#ਜੇ_ਅੰਤ
	got_int = TRUE;
}

/*
 * ":retab".
 */
    ਖਾਲੀ
ex_retab(exarg_T *eap)
{
    linenr_T	lnum;
    ਪੂਰਨ		got_tab = FALSE;
    ਲੰਮਾ	ਅੰਕ_spaces = 0;
    ਲੰਮਾ	ਅੰਕ_tabs;
    ਲੰਮਾ	len;
    ਲੰਮਾ	ਖਾਨਾ;
    ਲੰਮਾ	vcol;
    ਲੰਮਾ	ਸ਼ੁਰੂ_ਖਾਨਾ = 0;		// For start of white-space string
    ਲੰਮਾ	ਸ਼ੁਰੂ_vcol = 0;		// For start of white-space string
    ਲੰਮਾ	old_len;
    ਲੰਮਾ	new_len;
    char_u	*ptr;
    char_u	*new_ਸਤਰ = (char_u *)1; // init to non-NULL
    ਪੂਰਨ		did_undo;		// called u_save for current line
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
    ਪੂਰਨ		*new_vts_array = ਨਿਰਾ;
    char_u	*new_ts_str;		// string value of tab argument
#ਨਹੀਂਤਾਂ
    ਪੂਰਨ		ਆਰਜ਼ੀ;
    ਪੂਰਨ		new_ts = 0;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		save_list;
    linenr_T	ਪਹਿਲਾ_ਸਤਰ = 0;		// first changed line
    linenr_T	ਆਖਰੀ_ਸਤਰ = 0;		// last changed line
    ਪੂਰਨ		is_indent_only = 0;	// Only process leading whitespace

    save_list = curwin->w_p_list;
    curwin->w_p_list = 0;	    // don't want list mode here

    ptr = eap->arg;
    ਜੇ (STRNCMP(ptr, "-indentonly", 11) == 0 && IS_WHITE_OR_NUL(ptr[11]))
    {
	is_indent_only = 1;
	ptr = skipwhite(ptr + 11);
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
    new_ts_str = ptr;
    ਜੇ (tabstop_ਸੈੱਟ(ptr, &new_vts_array) == FAIL)
	ਵਾਪਸ;
    ਜਦੋਂ (vim_isdigit(*ptr) || *ptr == ',')
	++ptr;

    // This ensures that either new_vts_array and new_ts_str are freshly
    // allocated, or new_vts_array points to an existing array and new_ts_str
    // is null.
    ਜੇ (new_vts_array == ਨਿਰਾ)
    {
	new_vts_array = curbuf->b_p_vts_array;
	new_ts_str = ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ
	new_ts_str = vim_strnsave(new_ts_str, ptr - new_ts_str);
#ਨਹੀਂਤਾਂ
    ਜੇ (ptr[0] != NUL && (ptr[0] != '0' || ptr[1] != NUL))
    {
	char_u	*ਸਿਰਾ;

	ਜੇ (strtol((ਅੱਖਰ *)ptr, (ਅੱਖਰ **)&ਸਿਰਾ, 10) <= 0)
	{
	    ਜੇ (ptr != ਸਿਰਾ)
		emsg(_(e_ਦਲੀਲ_must_be_ਸਕਾਰਾਤਮਕ));
	    ਨਹੀਂਤਾਂ
		semsg(_(e_invalid_ਦਲੀਲ_str), ptr);
	    ਵਾਪਸ;
	}
	new_ts = getdigits(&ptr);
	ਜੇ (new_ts < 0 || new_ts > TABSTOP_MAX)
	{
	    semsg(_(e_invalid_ਦਲੀਲ_str), eap->arg);
	    ਵਾਪਸ;
	}
    }
    ਜੇ (new_ts == 0)
	new_ts = curbuf->b_p_ts;
#ਜੇ_ਅੰਤ
    ਲਈ (lnum = eap->line1; !got_int && lnum <= eap->line2; ++lnum)
    {
	ptr = ml_get(lnum);
	old_len = ml_get_len(lnum);
	ਖਾਨਾ = 0;
	vcol = 0;
	did_undo = FALSE;
	ਲਈ (;;)
	{
	    ਜੇ (VIM_ISWHITE(ptr[ਖਾਨਾ]))
	    {
		ਜੇ (!got_tab && ਅੰਕ_spaces == 0)
		{
		    // First consecutive white-space
		    ਸ਼ੁਰੂ_vcol = vcol;
		    ਸ਼ੁਰੂ_ਖਾਨਾ = ਖਾਨਾ;
		}
		ਜੇ (ptr[ਖਾਨਾ] == ' ')
		    ਅੰਕ_spaces++;
		ਨਹੀਂਤਾਂ
		    got_tab = TRUE;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (got_tab || (eap->forceit && ਅੰਕ_spaces > 1))
		{
		    // Retabulate this string of white-space

		    // len is virtual length of white string
		    len = ਅੰਕ_spaces = vcol - ਸ਼ੁਰੂ_vcol;
		    ਅੰਕ_tabs = 0;
		    ਜੇ (!curbuf->b_p_et)
		    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
			ਪੂਰਨ t, s;

			tabstop_fromto(ਸ਼ੁਰੂ_vcol, vcol,
					curbuf->b_p_ts, new_vts_array, &t, &s);
			ਅੰਕ_tabs = t;
			ਅੰਕ_spaces = s;
#ਨਹੀਂਤਾਂ
			ਆਰਜ਼ੀ = new_ts - (ਸ਼ੁਰੂ_vcol % new_ts);
			ਜੇ (ਅੰਕ_spaces >= ਆਰਜ਼ੀ)
			{
			    ਅੰਕ_spaces -= ਆਰਜ਼ੀ;
			    ਅੰਕ_tabs++;
			}
			ਅੰਕ_tabs += ਅੰਕ_spaces / new_ts;
			ਅੰਕ_spaces -= (ਅੰਕ_spaces / new_ts) * new_ts;
#ਜੇ_ਅੰਤ
		    }
		    ਜੇ (curbuf->b_p_et || got_tab ||
					(ਅੰਕ_spaces + ਅੰਕ_tabs < len))
		    {
			ਜੇ (did_undo == FALSE)
			{
			    did_undo = TRUE;
			    ਜੇ (u_save((linenr_T)(lnum - 1),
						(linenr_T)(lnum + 1)) == FAIL)
			    {
				new_ਸਤਰ = ਨਿਰਾ;	// flag out-of-memory
				ਰੋਕੋ;
			    }
			}

			// len is actual number of white characters used
			len = ਅੰਕ_spaces + ਅੰਕ_tabs;
			new_len = old_len - ਖਾਨਾ + ਸ਼ੁਰੂ_ਖਾਨਾ + len + 1;
			ਜੇ (new_len <= 0 || new_len >= MAXCOL)
			{
			    emsg_ਪਾਠ_too_long();
			    ਰੋਕੋ;
			}
			new_ਸਤਰ = alloc(new_len);
			ਜੇ (new_ਸਤਰ == ਨਿਰਾ)
			    ਰੋਕੋ;
			ਜੇ (ਸ਼ੁਰੂ_ਖਾਨਾ > 0)
			    mch_memmove(new_ਸਤਰ, ptr, (size_t)ਸ਼ੁਰੂ_ਖਾਨਾ);
			mch_memmove(new_ਸਤਰ + ਸ਼ੁਰੂ_ਖਾਨਾ + len,
				      ptr + ਖਾਨਾ, (size_t)(old_len - ਖਾਨਾ + 1));
			ptr = new_ਸਤਰ + ਸ਼ੁਰੂ_ਖਾਨਾ;
			ਲਈ (ਖਾਨਾ = 0; ਖਾਨਾ < len; ਖਾਨਾ++)
			    ptr[ਖਾਨਾ] = (ਖਾਨਾ < ਅੰਕ_tabs) ? '\t' : ' ';
			ਜੇ (ml_replace(lnum, new_ਸਤਰ, FALSE) == OK)
			    // "new_line" may have been copied
			    new_ਸਤਰ = curbuf->b_ml.ml_ਸਤਰ_ptr;
			ਜੇ (ਪਹਿਲਾ_ਸਤਰ == 0)
			    ਪਹਿਲਾ_ਸਤਰ = lnum;
			ਆਖਰੀ_ਸਤਰ = lnum;
			ptr = new_ਸਤਰ;
			old_len = new_len - 1;
			ਖਾਨਾ = ਸ਼ੁਰੂ_ਖਾਨਾ + len;
		    }
		}
		got_tab = FALSE;
		ਅੰਕ_spaces = 0;

		ਜੇ (is_indent_only)
		    ਰੋਕੋ;
	    }
	    ਜੇ (ptr[ਖਾਨਾ] == NUL)
		ਰੋਕੋ;
	    vcol += chartabsize(ptr + ਖਾਨਾ, (colnr_T)vcol);
	    ਜੇ (vcol >= MAXCOL)
	    {
		emsg_ਪਾਠ_too_long();
		ਰੋਕੋ;
	    }
	    ਜੇ (has_mbyte)
		ਖਾਨਾ += (*mb_ptr2len)(ptr + ਖਾਨਾ);
	    ਨਹੀਂਤਾਂ
		++ਖਾਨਾ;
	}
	ਜੇ (new_ਸਤਰ == ਨਿਰਾ)		    // out of memory
	    ਰੋਕੋ;
	ਸਤਰ_breakcheck();
    }
    ਜੇ (got_int)
	emsg(_(e_interrupted));

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
    // If a single value was given then it can be considered equal to
    // either the value of 'tabstop' or the value of 'vartabstop'.
    ਜੇ (tabstop_ਗਿਣਤੀ(curbuf->b_p_vts_array) == 0
	&& tabstop_ਗਿਣਤੀ(new_vts_array) == 1
	&& curbuf->b_p_ts == tabstop_ਪਹਿਲਾ(new_vts_array))
	; // not changed
    ਨਹੀਂਤਾਂ ਜੇ (tabstop_ਗਿਣਤੀ(curbuf->b_p_vts_array) > 0
	&& tabstop_eq(curbuf->b_p_vts_array, new_vts_array))
	; // not changed
    ਨਹੀਂਤਾਂ
	redraw_curbuf_later(UPD_NOT_VALID);
#ਨਹੀਂਤਾਂ
    ਜੇ (curbuf->b_p_ts != new_ts)
	redraw_curbuf_later(UPD_NOT_VALID);
#ਜੇ_ਅੰਤ
    ਜੇ (ਪਹਿਲਾ_ਸਤਰ != 0)
	ਬਦਲਿਆ_ਸਤਰਾਂ(ਪਹਿਲਾ_ਸਤਰ, 0, ਆਖਰੀ_ਸਤਰ + 1, 0L);

    curwin->w_p_list = save_list;	// restore 'list'

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
    ਜੇ (new_ts_str != ਨਿਰਾ)		// set the new tabstop
    {
	// If 'vartabstop' is in use or if the value given to retab has more
	// than one tabstop then update 'vartabstop'.
	ਪੂਰਨ *old_vts_ary = curbuf->b_p_vts_array;

	ਜੇ (tabstop_ਗਿਣਤੀ(old_vts_ary) > 0 || tabstop_ਗਿਣਤੀ(new_vts_array) > 1)
	{
	    ਸੈੱਟ_string_option_ਸਿੱਧਾ((char_u *)"vts", -1, new_ts_str,
							OPT_FREE|OPT_LOCAL, 0);
	    curbuf->b_p_vts_array = new_vts_array;
	    vim_free(old_vts_ary);
	}
	ਨਹੀਂਤਾਂ
	{
	    // 'vartabstop' wasn't in use and a single value was given to
	    // retab then update 'tabstop'.
	    curbuf->b_p_ts = tabstop_ਪਹਿਲਾ(new_vts_array);
	    vim_free(new_vts_array);
	}
	vim_free(new_ts_str);
    }
#ਨਹੀਂਤਾਂ
    curbuf->b_p_ts = new_ts;
#ਜੇ_ਅੰਤ
    coladvance(curwin->w_curswant);

    u_clearline();
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Get indent level from 'indentexpr'.
 */
    ਪੂਰਨ
get_ਵਿਅੰਜਕ_indent(ਖਾਲੀ)
{
    ਪੂਰਨ		indent = -1;
    char_u	*inde_copy;
    pos_T	save_pos;
    colnr_T	save_curswant;
    ਪੂਰਨ		save_ਸੈੱਟ_curswant;
    ਪੂਰਨ		save_State;
    ਪੂਰਨ		use_sandbox = was_ਸੈੱਟ_insecurely(curwin,
					    (char_u *)"indentexpr", OPT_LOCAL);
    sctx_T	save_sctx = current_sctx;

    // Save and restore cursor position and curswant, in case it was changed
    // via :normal commands
    save_pos = curwin->w_cursor;
    save_curswant = curwin->w_curswant;
    save_ਸੈੱਟ_curswant = curwin->w_ਸੈੱਟ_curswant;
    ਸੈੱਟ_vim_var_nr(VV_LNUM, curwin->w_cursor.lnum);
    ਜੇ (use_sandbox)
	++sandbox;
    ++textlock;
    current_sctx = curbuf->b_p_script_ctx[BV_INDE];

    // Need to make a copy, the 'indentexpr' option could be changed while
    // evaluating it.
    inde_copy = vim_strsave(curbuf->b_p_inde);
    ਜੇ (inde_copy != ਨਿਰਾ)
    {
	indent = (ਪੂਰਨ)eval_ਮੰਜ਼ਿਲ_ਸੰਖਿਆ(inde_copy, TRUE);
	vim_free(inde_copy);
    }

    ਜੇ (use_sandbox)
	--sandbox;
    --textlock;
    current_sctx = save_sctx;

    // Restore the cursor position so that 'indentexpr' doesn't need to.
    // Pretend to be in Insert mode, allow cursor past end of line for "o"
    // command.
    save_State = State;
    State = ਬਹੁਲਕ_ਪਾਓ;
    curwin->w_cursor = save_pos;
    curwin->w_curswant = save_curswant;
    curwin->w_ਸੈੱਟ_curswant = save_ਸੈੱਟ_curswant;
    check_cursor();
    State = save_State;

    // Reset did_throw, unless 'debug' has "throw" and inside a try/catch.
    ਜੇ (did_throw && (vim_strchr(p_debug, 't') == ਨਿਰਾ || trylevel == 0))
    {
	handle_did_throw();
	did_throw = FALSE;
    }

    // If there is an error, just keep the current indent.
    ਜੇ (indent < 0)
	indent = get_indent();

    ਵਾਪਸ indent;
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ ਪੂਰਨ
lisp_ਮੇਲ(char_u *p)
{
    char_u	buf[LSIZE];
    ਪੂਰਨ		len;
    char_u	*ਸ਼ਬਦ = *curbuf->b_p_lw != NUL ? curbuf->b_p_lw : p_lispwords;

    ਜਦੋਂ (*ਸ਼ਬਦ != NUL)
    {
	len = copy_option_part(&ਸ਼ਬਦ, buf, LSIZE, ",");
	ਜੇ (STRNCMP(buf, p, len) == 0 && IS_WHITE_OR_NUL(p[len]))
	    ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
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
    ਪੂਰਨ
get_lisp_indent(ਖਾਲੀ)
{
    pos_T	*pos, realpos, paren;
    ਪੂਰਨ		amount;
    char_u	*that;
    colnr_T	ਖਾਨਾ;
    colnr_T	firsttry;
    ਪੂਰਨ		parencount, quotecount;
    ਪੂਰਨ		vi_lisp;

    // Set vi_lisp to use the vi-compatible method
    vi_lisp = (vim_strchr(p_cpo, CPO_LISP) != ਨਿਰਾ);

    realpos = curwin->w_cursor;
    curwin->w_cursor.ਖਾਨਾ = 0;

    ਜੇ ((pos = findmatch(ਨਿਰਾ, '(')) == ਨਿਰਾ)
	pos = findmatch(ਨਿਰਾ, '[');
    ਨਹੀਂਤਾਂ
    {
	paren = *pos;
	pos = findmatch(ਨਿਰਾ, '[');
	ਜੇ (pos == ਨਿਰਾ || LT_POSP(pos, &paren))
	    pos = &paren;
    }
    ਜੇ (pos != ਨਿਰਾ)
    {
	// Extra trick: Take the indent of the first previous non-white
	// line that is at the same () level.
	amount = -1;
	parencount = 0;

	ਜਦੋਂ (--curwin->w_cursor.lnum >= pos->lnum)
	{
	    ਜੇ (linewhite(curwin->w_cursor.lnum))
		ਜਾਰੀ;
	    ਲਈ (that = ml_get_curline(); *that != NUL; ++that)
	    {
		ਜੇ (*that == ';')
		{
		    ਜਦੋਂ (*(that + 1) != NUL)
			++that;
		    ਜਾਰੀ;
		}
		ਜੇ (*that == '\\')
		{
		    ਜੇ (*(that + 1) != NUL)
			++that;
		    ਜਾਰੀ;
		}
		ਜੇ (*that == '"' && *(that + 1) != NUL)
		{
		    ਜਦੋਂ (*++that && *that != '"')
		    {
			// skipping escaped characters in the string
			ਜੇ (*that == '\\')
			{
			    ਜੇ (*++that == NUL)
				ਰੋਕੋ;
			    ਜੇ (that[1] == NUL)
			    {
				++that;
				ਰੋਕੋ;
			    }
			}
		    }
		    ਜੇ (*that == NUL)
			ਰੋਕੋ;
		}
		ਜੇ (*that == '(' || *that == '[')
		    ++parencount;
		ਨਹੀਂਤਾਂ ਜੇ (*that == ')' || *that == ']')
		    --parencount;
	    }
	    ਜੇ (parencount == 0)
	    {
		amount = get_indent();
		ਰੋਕੋ;
	    }
	}

	ਜੇ (amount == -1)
	{
	    curwin->w_cursor.lnum = pos->lnum;
	    curwin->w_cursor.ਖਾਨਾ = pos->ਖਾਨਾ;
	    ਖਾਨਾ = pos->ਖਾਨਾ;

	    that = ml_get_curline();

	    ਜੇ (vi_lisp && get_indent() == 0)
		amount = 2;
	    ਨਹੀਂਤਾਂ
	    {
		char_u		*ਸਤਰ = that;
		chartabsize_T	cts;

		init_chartabsize_arg(&cts, curwin, pos->lnum, 0, ਸਤਰ, ਸਤਰ);
		ਜਦੋਂ (*cts.cts_ptr != NUL && ਖਾਨਾ > 0)
		{
		    cts.cts_vcol += lbr_chartabsize_adv(&cts);
		    ਖਾਨਾ--;
		}
		amount = cts.cts_vcol;
		that = cts.cts_ptr;
		clear_chartabsize_arg(&cts);

		// Some keywords require "body" indenting rules (the
		// non-standard-lisp ones are Scheme special forms):
		//
		// (let ((a 1))    instead    (let ((a 1))
		//   (...))	      of	   (...))

		ਜੇ (!vi_lisp && (*that == '(' || *that == '[')
						      && lisp_ਮੇਲ(that + 1))
		    amount += 2;
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (*that != NUL)
		    {
			that++;
			amount++;
		    }
		    firsttry = amount;

		    init_chartabsize_arg(&cts, curwin, (colnr_T)(that - ਸਤਰ),
							   amount, ਸਤਰ, that);
		    ਜਦੋਂ (VIM_ISWHITE(*cts.cts_ptr))
		    {
			cts.cts_vcol += lbr_chartabsize(&cts);
			++cts.cts_ptr;
		    }
		    that = cts.cts_ptr;
		    amount = cts.cts_vcol;
		    clear_chartabsize_arg(&cts);

		    ਜੇ (*that && *that != ';') // not a comment line
		    {
			// test *that != '(' to accommodate first let/do
			// argument if it is more than one line
			ਜੇ (!vi_lisp && *that != '(' && *that != '[')
			    firsttry++;

			parencount = 0;
			quotecount = 0;

			init_chartabsize_arg(&cts, curwin,
				   (colnr_T)(that - ਸਤਰ), amount, ਸਤਰ, that);
			ਜੇ (vi_lisp
				|| (*that != '"'
				    && *that != '\''
				    && *that != '#'
				    && (*that < '0' || *that > '9')))
			{
			    ਜਦੋਂ (*cts.cts_ptr
				    && (!VIM_ISWHITE(*cts.cts_ptr)
					|| quotecount
					|| parencount)
				    && (!((*cts.cts_ptr == '('
							|| *cts.cts_ptr == '[')
					    && !quotecount
					    && !parencount
					    && vi_lisp)))
			    {
				ਜੇ (*cts.cts_ptr == '"')
				    quotecount = !quotecount;
				ਜੇ ((*cts.cts_ptr == '(' || *cts.cts_ptr == '[')
							       && !quotecount)
				    ++parencount;
				ਜੇ ((*cts.cts_ptr == ')' || *cts.cts_ptr == ']')
							       && !quotecount)
				    --parencount;
				ਜੇ (*cts.cts_ptr == '\\'
						    && *(cts.cts_ptr+1) != NUL)
				    cts.cts_vcol += lbr_chartabsize_adv(&cts);
				cts.cts_vcol += lbr_chartabsize_adv(&cts);
			    }
			}
			ਜਦੋਂ (VIM_ISWHITE(*cts.cts_ptr))
			{
			    cts.cts_vcol += lbr_chartabsize(&cts);
			    ++cts.cts_ptr;
			}
			that = cts.cts_ptr;
			amount = cts.cts_vcol;
			clear_chartabsize_arg(&cts);

			ਜੇ (!*that || *that == ';')
			    amount = firsttry;
		    }
		}
	    }
	}
    }
    ਨਹੀਂਤਾਂ
	amount = 0;	// no matching '(' or '[' found, use zero indent

    curwin->w_cursor = realpos;

    ਵਾਪਸ amount;
}

/*
 * Re-indent the current line, based on the current contents of it and the
 * surrounding lines. Fixing the cursor position seems really easy -- I'm very
 * confused what all the part that handles Control-T is doing that I'm not.
 * "get_the_indent" should be get_c_indent, get_expr_indent or get_lisp_indent.
 */

    ਖਾਲੀ
fixthisline(ਪੂਰਨ (*get_the_indent)(ਖਾਲੀ))
{
    ਪੂਰਨ amount = get_the_indent();

    ਜੇ (amount < 0)
	ਵਾਪਸ;

    change_indent(INDENT_ਸੈੱਟ, amount, FALSE, 0, TRUE);
    ਜੇ (linewhite(curwin->w_cursor.lnum))
	did_ai = TRUE;	// delete the indent if the line stays empty
}

/*
 * Return TRUE if 'indentexpr' should be used for Lisp indenting.
 * Caller may want to check 'autoindent'.
 */
    ਪੂਰਨ
use_indentexpr_for_lisp(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਵਾਪਸ curbuf->b_p_lisp
		&& *curbuf->b_p_inde != NUL
		&& STRCMP(curbuf->b_p_lop, "expr:1") == 0;
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ FALSE;
#ਜੇ_ਅੰਤ
}

/*
 * Fix indent for 'lisp' and 'cindent'.
 */
    ਖਾਲੀ
fix_indent(ਖਾਲੀ)
{
    ਜੇ (p_paste)
	ਵਾਪਸ;  // no auto-indenting when 'paste' is set
    ਜੇ (curbuf->b_p_lisp && curbuf->b_p_ai)
    {
	ਜੇ (use_indentexpr_for_lisp())
	    do_c_ਵਿਅੰਜਕ_indent();
	ਨਹੀਂਤਾਂ
	    fixthisline(get_lisp_indent);
    }
    ਨਹੀਂਤਾਂ ਜੇ (cindent_on())
	do_c_ਵਿਅੰਜਕ_indent();
}

#ਜੇ defined(FEAT_EVAL)
/*
 * "indent()" function
 */
    ਖਾਲੀ
f_indent(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    lnum = tv_get_lnum(argvars);
    ਜੇ (lnum >= 1 && lnum <= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	rettv->vval.v_ਸੰਖਿਆ = get_indent_lnum(lnum);
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (ਵਿੱਚ_vim9script())
	    semsg(_(e_invalid_ਸਤਰ_ਸੰਖਿਆ_nr), lnum);
	rettv->vval.v_ਸੰਖਿਆ = -1;
    }
}

/*
 * "lispindent(lnum)" function
 */
    ਖਾਲੀ
f_lispindent(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    pos_T	pos;
    linenr_T	lnum;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    ਜੇ (lnum >= 1 && lnum <= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_ਸੰਖਿਆ = get_lisp_indent();
	curwin->w_cursor = pos;
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਚ_vim9script())
	semsg(_(e_invalid_ਸਤਰ_ਸੰਖਿਆ_nr), lnum);
    ਨਹੀਂਤਾਂ
	rettv->vval.v_ਸੰਖਿਆ = -1;
}
#ਜੇ_ਅੰਤ
