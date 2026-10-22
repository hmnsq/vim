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

#ಆದರೆ defined(FEAT_VARTABS)

/*
 * Set the integer values corresponding to the string setting of 'vartabstop'.
 * "array" will be set, caller must free it if needed.
 * Return FAIL for an error.
 */
    ಪೂರ್ಣಾಂಕ
tabstop_ಗಣ(char_u *var, ಪೂರ್ಣಾಂಕ **array)
{
    ಪೂರ್ಣಾಂಕ	    valcount = 1;
    ಪೂರ್ಣಾಂಕ	    t;
    char_u  *cp;

    ಆದರೆ (var[0] == NUL || (var[0] == '0' && var[1] == NUL))
    {
	*array = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ OK;
    }

    ಗಾಗಿ (cp = var; *cp != NUL; ++cp)
    {
	ಆದರೆ (cp == var || cp[-1] == ',')
	{
	    char_u *ಕೊನೆ;

	    ಆದರೆ (strtol((ಅಕ್ಷರ *)cp, (ಅಕ್ಷರ **)&ಕೊನೆ, 10) <= 0)
	    {
		ಆದರೆ (cp != ಕೊನೆ)
		    emsg(_(e_ವಾದ_must_be_ಧನಾತ್ಮಕ));
		ಇಲ್ಲದಿದ್ದರೆ
		    semsg(_(e_invalid_ವಾದ_str), cp);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}

	ಆದರೆ (VIM_ISDIGIT(*cp))
	    ಮುಂದುವರಿ;
	ಆದರೆ (cp[0] == ',' && cp > var && cp[-1] != ',' && cp[1] != NUL)
	{
	    ++valcount;
	    ಮುಂದುವರಿ;
	}
	semsg(_(e_invalid_ವಾದ_str), var);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    *array = ALLOC_MULT(ಪೂರ್ಣಾಂಕ, valcount + 1);
    ಆದರೆ (*array == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    (*array)[0] = valcount;

    t = 1;
    ಗಾಗಿ (cp = var; *cp != NUL;)
    {
	ಪೂರ್ಣಾಂಕ n = atoi((ಅಕ್ಷರ *)cp);

	// Catch negative values, overflow and ridiculous big values.
	ಆದರೆ (n <= 0 || n > TABSTOP_MAX)
	{
	    semsg(_(e_invalid_ವಾದ_str), cp);
	    VIM_CLEAR(*array);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	(*array)[t++] = n;
	ಆಗ (*cp != NUL && *cp != ',')
	    ++cp;
	ಆದರೆ (*cp != NUL)
	    ++cp;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Calculate the number of screen spaces a tab will occupy.
 * If "vts" is set then the tab widths are taken from that array,
 * otherwise the value of ts is used.
 */
    ಪೂರ್ಣಾಂಕ
tabstop_padding(colnr_T ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ ts_arg, ಪೂರ್ಣಾಂಕ *vts)
{
    ಪೂರ್ಣಾಂಕ		ts = ts_arg == 0 ? 8 : ts_arg;
    ಪೂರ್ಣಾಂಕ		tabcount;
    colnr_T	tabcol = 0;
    ಪೂರ್ಣಾಂಕ		t;
    ಪೂರ್ಣಾಂಕ		padding = 0;

    ಆದರೆ (vts == ಖಾಲಿ || vts[0] == 0)
	ಹಿಂತಿರುಗಿ ts - (ಲಂಬಸಾಲು % ts);

    tabcount = vts[0];

    ಗಾಗಿ (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	ಆದರೆ (tabcol > ಲಂಬಸಾಲು)
	{
	    padding = (ಪೂರ್ಣಾಂಕ)(tabcol - ಲಂಬಸಾಲು);
	    ನಿಲ್ಲಿಸು;
	}
    }
    ಆದರೆ (t > tabcount)
	padding = vts[tabcount] - (ಪೂರ್ಣಾಂಕ)((ಲಂಬಸಾಲು - tabcol) % vts[tabcount]);

    ಹಿಂತಿರುಗಿ padding;
}

/*
 * Find the size of the tab that covers a particular column.
 *
 * If this is being called as part of a shift operation, col is not the cursor
 * column but is the column number to the left of the first non-whitespace
 * character in the line.  If the shift is to the left (left = TRUE), then
 * return the size of the tab interval to the left of the column.
 */
    ಪೂರ್ಣಾಂಕ
tabstop_at(colnr_T ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ ts, ಪೂರ್ಣಾಂಕ *vts, ಪೂರ್ಣಾಂಕ ಎಡ_2)
{
    ಪೂರ್ಣಾಂಕ		tabcount;	// Number of tab stops in the list of variable
				// tab stops.
    colnr_T	tabcol = 0;	// Column of the tab stop under consideration.
    ಪೂರ್ಣಾಂಕ		t;		// Tabstop index in the list of variable tab
				// stops.
    ಪೂರ್ಣಾಂಕ		tab_ಗಾತ್ರ = 0;	// Size of the tab stop interval to the right
				// or left of the col.

    ಆದರೆ (vts == 0 || vts[0] == 0)
	ಹಿಂತಿರುಗಿ ts;

    tabcount = vts[0];
    ಗಾಗಿ (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	ಆದರೆ (tabcol > ಲಂಬಸಾಲು)
	{
	    // If shifting left (left != 0), and if the column to the left of
	    // the first first non-blank character (col) in the line is
	    // already to the left of the first tabstop, set the shift amount
	    // (tab_size) to just enough to shift the line to the left margin.
	    // The value doesn't seem to matter as long as it is at least that
	    // distance.
	    ಆದರೆ (ಎಡ_2 && (t == 1))
		tab_ಗಾತ್ರ = ಲಂಬಸಾಲು;
	    ಇಲ್ಲದಿದ್ದರೆ
		tab_ಗಾತ್ರ = vts[t - (ಎಡ_2 ? 1 : 0)];
	    ನಿಲ್ಲಿಸು;
	}
    }
    ಆದರೆ (t > tabcount)		// If the value of the index t is beyond the
				// end of the list, use the tab stop value at
				// the end of the list.
	tab_ಗಾತ್ರ = vts[tabcount];

    ಹಿಂತಿರುಗಿ tab_ಗಾತ್ರ;
}

/*
 * Find the column on which a tab starts.
 */
    colnr_T
tabstop_ಆರಂಭ(colnr_T ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ ts, ಪೂರ್ಣಾಂಕ *vts)
{
    ಪೂರ್ಣಾಂಕ		tabcount;
    colnr_T	tabcol = 0;
    ಪೂರ್ಣಾಂಕ		t;
    ಪೂರ್ಣಾಂಕ		excess;

    ಆದರೆ (vts == ಖಾಲಿ || vts[0] == 0)
	ಹಿಂತಿರುಗಿ ಲಂಬಸಾಲು - ಲಂಬಸಾಲು % ts;

    tabcount = vts[0];
    ಗಾಗಿ (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	ಆದರೆ (tabcol > ಲಂಬಸಾಲು)
	    ಹಿಂತಿರುಗಿ tabcol - vts[t];
    }

    excess = tabcol % vts[tabcount];
    ಹಿಂತಿರುಗಿ ಲಂಬಸಾಲು - (ಲಂಬಸಾಲು - excess) % vts[tabcount];
}

/*
 * Find the number of tabs and spaces necessary to get from one column
 * to another.
 */
    ಶೂನ್ಯ
tabstop_fromto(
	colnr_T ಆರಂಭ_ಲಂಬಸಾಲು,
	colnr_T ಕೊನೆ_ಲಂಬಸಾಲು,
	ಪೂರ್ಣಾಂಕ	ts_arg,
	ಪೂರ್ಣಾಂಕ	*vts,
	ಪೂರ್ಣಾಂಕ	*ntabs,
	ಪೂರ್ಣಾಂಕ	*nspcs)
{
    ಪೂರ್ಣಾಂಕ		spaces = ಕೊನೆ_ಲಂಬಸಾಲು - ಆರಂಭ_ಲಂಬಸಾಲು;
    colnr_T	tabcol = 0;
    ಪೂರ್ಣಾಂಕ		padding = 0;
    ಪೂರ್ಣಾಂಕ		tabcount;
    ಪೂರ್ಣಾಂಕ		t;
    ಪೂರ್ಣಾಂಕ		ts = ts_arg == 0 ? curbuf->b_p_ts : ts_arg;

    ಆದರೆ (vts == ಖಾಲಿ || vts[0] == 0)
    {
	ಪೂರ್ಣಾಂಕ tabs = 0;
	ಪೂರ್ಣಾಂಕ initspc = 0;

	initspc = ts - (ಆರಂಭ_ಲಂಬಸಾಲು % ts);
	ಆದರೆ (spaces >= initspc)
	{
	    spaces -= initspc;
	    tabs++;
	}
	tabs += spaces / ts;
	spaces -= (spaces / ts) * ts;

	*ntabs = tabs;
	*nspcs = spaces;
	ಹಿಂತಿರುಗಿ;
    }

    // Find the padding needed to reach the next tabstop.
    tabcount = vts[0];
    ಗಾಗಿ (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	ಆದರೆ (tabcol > ಆರಂಭ_ಲಂಬಸಾಲು)
	{
	    padding = (ಪೂರ್ಣಾಂಕ)(tabcol - ಆರಂಭ_ಲಂಬಸಾಲು);
	    ನಿಲ್ಲಿಸು;
	}
    }
    ಆದರೆ (t > tabcount)
	padding = vts[tabcount] - (ಪೂರ್ಣಾಂಕ)((ಆರಂಭ_ಲಂಬಸಾಲು - tabcol) % vts[tabcount]);

    // If the space needed is less than the padding no tabs can be used.
    ಆದರೆ (spaces < padding)
    {
	*ntabs = 0;
	*nspcs = spaces;
	ಹಿಂತಿರುಗಿ;
    }

    *ntabs = 1;
    spaces -= padding;

    // At least one tab has been used. See if any more will fit.
    ಆಗ (spaces != 0 && ++t <= tabcount)
    {
	padding = vts[t];
	ಆದರೆ (spaces < padding)
	{
	    *nspcs = spaces;
	    ಹಿಂತಿರುಗಿ;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
tabstop_eq(ಪೂರ್ಣಾಂಕ *ts1, ಪೂರ್ಣಾಂಕ *ts2)
{
    ಪೂರ್ಣಾಂಕ		t;

    ಆದರೆ ((ts1 == 0 && ts2) || (ts1 && ts2 == 0))
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ (ts1 == ts2)
	ಹಿಂತಿರುಗಿ TRUE;
    ಆದರೆ (ts1[0] != ts2[0])
	ಹಿಂತಿರುಗಿ FALSE;

    ಗಾಗಿ (t = 1; t <= ts1[0]; ++t)
	ಆದರೆ (ts1[t] != ts2[t])
	    ಹಿಂತಿರುಗಿ FALSE;

    ಹಿಂತಿರುಗಿ TRUE;
}

# ಆದರೆ defined(FEAT_BEVAL)
/*
 * Copy a tabstop array, allocating space for the new array.
 */
    ಪೂರ್ಣಾಂಕ *
tabstop_copy(ಪೂರ್ಣಾಂಕ *oldts)
{
    ಪೂರ್ಣಾಂಕ		*newts;
    ಪೂರ್ಣಾಂಕ		t;

    ಆದರೆ (oldts == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    newts = ALLOC_MULT(ಪೂರ್ಣಾಂಕ, oldts[0] + 1);
    ಆದರೆ (newts != ಖಾಲಿ)
	ಗಾಗಿ (t = 0; t <= oldts[0]; ++t)
	    newts[t] = oldts[t];
    ಹಿಂತಿರುಗಿ newts;
}
# endif

/*
 * Return a count of the number of tabstops.
 */
    ಪೂರ್ಣಾಂಕ
tabstop_ಎಣಿಕೆ(ಪೂರ್ಣಾಂಕ *ts)
{
    ಹಿಂತಿರುಗಿ ts != ಖಾಲಿ ? ts[0] : 0;
}

/*
 * Return the first tabstop, or 8 if there are no tabstops defined.
 */
    ಪೂರ್ಣಾಂಕ
tabstop_ಮೊದಲ(ಪೂರ್ಣಾಂಕ *ts)
{
    ಹಿಂತಿರುಗಿ ts != ಖಾಲಿ ? ts[1] : 8;
}

#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return the effective shiftwidth value for current buffer, using the
 * 'tabstop' value when 'shiftwidth' is zero.
 */
    ದೀರ್ಘ
get_sw_ಮೌಲ್ಯ(buf_T *buf)
{
    ಹಿಂತಿರುಗಿ get_sw_ಮೌಲ್ಯ_ಲಂಬಸಾಲು(buf, 0, FALSE);
}

/*
 * Idem, using "pos".
 */
    ಸ್ಥಿರ ದೀರ್ಘ
get_sw_ಮೌಲ್ಯ_pos(buf_T *buf, pos_T *pos, ಪೂರ್ಣಾಂಕ ಎಡ_2)
{
    pos_T save_cursor = curwin->w_cursor;
    ದೀರ್ಘ sw_ಮೌಲ್ಯ;

    curwin->w_cursor = *pos;
    sw_ಮೌಲ್ಯ = get_sw_ಮೌಲ್ಯ_ಲಂಬಸಾಲು(buf, get_nolist_virtcol(), ಎಡ_2);
    curwin->w_cursor = save_cursor;
    ಹಿಂತಿರುಗಿ sw_ಮೌಲ್ಯ;
}

/*
 * Idem, using the first non-black in the current line.
 */
    ದೀರ್ಘ
get_sw_ಮೌಲ್ಯ_indent(buf_T *buf, ಪೂರ್ಣಾಂಕ ಎಡ_2)
{
    pos_T pos = curwin->w_cursor;

    pos.ಲಂಬಸಾಲು = getwhitecols_curline();
    ಹಿಂತಿರುಗಿ get_sw_ಮೌಲ್ಯ_pos(buf, &pos, ಎಡ_2);
}

/*
 * Idem, using virtual column "col".
 */
    ದೀರ್ಘ
get_sw_ಮೌಲ್ಯ_ಲಂಬಸಾಲು(buf_T *buf, colnr_T ಲಂಬಸಾಲು ಬಳಸದ, ಪೂರ್ಣಾಂಕ ಎಡ_2 ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ buf->b_p_sw ? buf->b_p_sw :
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
	tabstop_at(ಲಂಬಸಾಲು, buf->b_p_ts, buf->b_p_vts_array, ಎಡ_2);
#ಇಲ್ಲದಿದ್ದರೆ
	buf->b_p_ts;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Return the effective softtabstop value for the current buffer, using the
 * 'shiftwidth' value when 'softtabstop' is negative.
 */
    ದೀರ್ಘ
get_sts_ಮೌಲ್ಯ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ curbuf->b_p_sts < 0 ? get_sw_ಮೌಲ್ಯ(curbuf) : curbuf->b_p_sts;
}

/*
 * Count the size (in window cells) of the indent in the current line.
 */
    ಪೂರ್ಣಾಂಕ
get_indent(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
    ಹಿಂತಿರುಗಿ get_indent_str_vtab(ml_get_curline(), (ಪೂರ್ಣಾಂಕ)curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ get_indent_str(ml_get_curline(), (ಪೂರ್ಣಾಂಕ)curbuf->b_p_ts, FALSE);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Count the size (in window cells) of the indent in line "lnum".
 */
    ಪೂರ್ಣಾಂಕ
get_indent_lnum(linenr_T lnum)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
    ಹಿಂತಿರುಗಿ get_indent_str_vtab(ml_get(lnum), (ಪೂರ್ಣಾಂಕ)curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ get_indent_str(ml_get(lnum), (ಪೂರ್ಣಾಂಕ)curbuf->b_p_ts, FALSE);
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(FEAT_FOLDING)
/*
 * Count the size (in window cells) of the indent in line "lnum" of buffer
 * "buf".
 */
    ಪೂರ್ಣಾಂಕ
get_indent_buf(buf_T *buf, linenr_T lnum)
{
# ifdef FEAT_VARTABS
    ಹಿಂತಿರುಗಿ get_indent_str_vtab(ml_get_buf(buf, lnum, FALSE),
			       (ಪೂರ್ಣಾಂಕ)buf->b_p_ts, buf->b_p_vts_array, FALSE);
# ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ get_indent_str(ml_get_buf(buf, lnum, FALSE), (ಪೂರ್ಣಾಂಕ)buf->b_p_ts, FALSE);
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * count the size (in window cells) of the indent in line "ptr", with
 * 'tabstop' at "ts"
 */
    ಪೂರ್ಣಾಂಕ
get_indent_str(
    char_u	*ptr,
    ಪೂರ್ಣಾಂಕ		ts,
    ಪೂರ್ಣಾಂಕ		no_ts) // if TRUE, count a tab as ^I
{
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2 = 0;

    ಗಾಗಿ ( ; *ptr; ++ptr)
    {
	ಆದರೆ (*ptr == TAB)    // count a tab for what it is worth
	{
	    ಆದರೆ (!no_ts)
		ಎಣಿಕೆ_2 += ts - (ಎಣಿಕೆ_2 % ts);
	    ಇಲ್ಲದಿದ್ದರೆ
		ಎಣಿಕೆ_2 += ptr2cells(ptr);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*ptr == ' ')
	    ++ಎಣಿಕೆ_2;		// count a space for one
	ಇಲ್ಲದಿದ್ದರೆ
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_2;
}

#ಆದರೆ defined(FEAT_VARTABS)
/*
 * Count the size (in window cells) of the indent in line "ptr", using
 * variable tabstops.
 * If "no_ts" is TRUE, count a tab as ^I.
 */
    ಪೂರ್ಣಾಂಕ
get_indent_str_vtab(char_u *ptr, ಪೂರ್ಣಾಂಕ ts, ಪೂರ್ಣಾಂಕ *vts, ಪೂರ್ಣಾಂಕ no_ts)
{
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2 = 0;

    ಗಾಗಿ ( ; *ptr; ++ptr)
    {
	ಆದರೆ (*ptr == TAB)    // count a tab for what it is worth
	{
	    ಆದರೆ (!no_ts)
		ಎಣಿಕೆ_2 += tabstop_padding(ಎಣಿಕೆ_2, ts, vts);
	    ಇಲ್ಲದಿದ್ದರೆ
		ಎಣಿಕೆ_2 += ptr2cells(ptr);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*ptr == ' ')
	    ++ಎಣಿಕೆ_2;		// count a space for one
	ಇಲ್ಲದಿದ್ದರೆ
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_2;
}
#ಆದರೆ_ಅಂತ್ಯ

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
    ಪೂರ್ಣಾಂಕ
ಗಣ_indent(
    ಪೂರ್ಣಾಂಕ		ಗಾತ್ರ_2,		    // measured in spaces
    ಪೂರ್ಣಾಂಕ		flags)
{
    char_u	*p;
    char_u	*newline;
    char_u	*oldline;
    char_u	*s;
    ಪೂರ್ಣಾಂಕ		todo;
    ಪೂರ್ಣಾಂಕ		ind_len;	    // measured in characters
    ಪೂರ್ಣಾಂಕ		ಸಾಲು_len;	    // size of the line (including the NUL)
    ಪೂರ್ಣಾಂಕ		doit = FALSE;
    ಪೂರ್ಣಾಂಕ		ind_ಮುಗಿದ = 0;	    // measured in spaces
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
    ಪೂರ್ಣಾಂಕ		ind_ಲಂಬಸಾಲು = 0;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		tab_pad;
    ಪೂರ್ಣಾಂಕ		retval = FALSE;
    ಪೂರ್ಣಾಂಕ		orig_char_len = -1; // number of initial whitespace chars when
				    // 'et' and 'pi' are both set

    // First check if there is anything to do and compute the number of
    // characters needed for the indent.
    todo = ಗಾತ್ರ_2;
    ind_len = 0;
    p = oldline = ml_get_curline();
    ಸಾಲು_len = ml_get_curline_len() + 1;

    // Calculate the buffer size for the new indent, and check to see if it
    // isn't already set

    // if 'expandtab' isn't set: use TABs; if both 'expandtab' and
    // 'preserveindent' are set count the number of characters at the
    // beginning of the line to be copied
    ಆದರೆ (!curbuf->b_p_et || (!(flags & SIN_ಸೇರಿಸು) && curbuf->b_p_pi))
    {
	// If 'preserveindent' is set then reuse as much as possible of
	// the existing indent structure for the new indent
	ಆದರೆ (!(flags & SIN_ಸೇರಿಸು) && curbuf->b_p_pi)
	{
	    ind_ಮುಗಿದ = 0;

	    // count as many characters as we can use
	    ಆಗ (todo > 0 && VIM_ISWHITE(*p))
	    {
		ಆದರೆ (*p == TAB)
		{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
		    tab_pad = tabstop_padding(ind_ಮುಗಿದ, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#ಇಲ್ಲದಿದ್ದರೆ
		    tab_pad = (ಪೂರ್ಣಾಂಕ)curbuf->b_p_ts
					   - (ind_ಮುಗಿದ % (ಪೂರ್ಣಾಂಕ)curbuf->b_p_ts);
#ಆದರೆ_ಅಂತ್ಯ
		    // stop if this tab will overshoot the target
		    ಆದರೆ (todo < tab_pad)
			ನಿಲ್ಲಿಸು;
		    todo -= tab_pad;
		    ++ind_len;
		    ind_ಮುಗಿದ += tab_pad;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    --todo;
		    ++ind_len;
		    ++ind_ಮುಗಿದ;
		}
		++p;
	    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
	    // These diverge from this point.
	    ind_ಲಂಬಸಾಲು = ind_ಮುಗಿದ;
#ಆದರೆ_ಅಂತ್ಯ
	    // Set initial number of whitespace chars to copy if we are
	    // preserving indent but expandtab is set
	    ಆದರೆ (curbuf->b_p_et)
		orig_char_len = ind_len;

	    // Fill to next tabstop with a tab, if possible
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
	    tab_pad = tabstop_padding(ind_ಮುಗಿದ, curbuf->b_p_ts,
						curbuf->b_p_vts_array);
#ಇಲ್ಲದಿದ್ದರೆ
	    tab_pad = (ಪೂರ್ಣಾಂಕ)curbuf->b_p_ts - (ind_ಮುಗಿದ % (ಪೂರ್ಣಾಂಕ)curbuf->b_p_ts);
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (todo >= tab_pad && orig_char_len == -1)
	    {
		doit = TRUE;
		todo -= tab_pad;
		++ind_len;
		// ind_done += tab_pad;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
		ind_ಲಂಬಸಾಲು += tab_pad;
#ಆದರೆ_ಅಂತ್ಯ
	    }
	}

	// count tabs required for indent
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
	ಗಾಗಿ (;;)
	{
	    tab_pad = tabstop_padding(ind_ಲಂಬಸಾಲು, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
	    ಆದರೆ (todo < tab_pad)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (*p != TAB)
		doit = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ
		++p;
	    todo -= tab_pad;
	    ++ind_len;
	    ind_ಲಂಬಸಾಲು += tab_pad;
	}
#ಇಲ್ಲದಿದ್ದರೆ
	ಆಗ (todo >= (ಪೂರ್ಣಾಂಕ)curbuf->b_p_ts)
	{
	    ಆದರೆ (*p != TAB)
		doit = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ
		++p;
	    todo -= (ಪೂರ್ಣಾಂಕ)curbuf->b_p_ts;
	    ++ind_len;
	    // ind_done += (int)curbuf->b_p_ts;
	}
#ಆದರೆ_ಅಂತ್ಯ
    }
    // count spaces required for indent
    ಆಗ (todo > 0)
    {
	ಆದರೆ (*p != ' ')
	    doit = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ
	    ++p;
	--todo;
	++ind_len;
	// ++ind_done;
    }

    // Return if the indent is OK already.
    ಆದರೆ (!doit && !VIM_ISWHITE(*p) && !(flags & SIN_ಸೇರಿಸು))
	ಹಿಂತಿರುಗಿ FALSE;

    // Allocate memory for the new line.
    ಆದರೆ (flags & SIN_ಸೇರಿಸು)
	p = oldline;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	p = skipwhite(p);
	ಸಾಲು_len -= (ಪೂರ್ಣಾಂಕ)(p - oldline);
    }

    // If 'preserveindent' and 'expandtab' are both set keep the original
    // characters and allocate accordingly.  We will fill the rest with spaces
    // after the if (!curbuf->b_p_et) below.
    ಆದರೆ (orig_char_len != -1)
    {
	newline = alloc(orig_char_len + ಗಾತ್ರ_2 - ind_ಮುಗಿದ + ಸಾಲು_len);
	ಆದರೆ (newline == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FALSE;
	todo = ಗಾತ್ರ_2 - ind_ಮುಗಿದ;
	ind_len = orig_char_len + todo;    // Set total length of indent in
					   // characters, which may have been
					   // undercounted until now
	p = oldline;
	s = newline;
	ಆಗ (orig_char_len > 0)
	{
	    *s++ = *p++;
	    orig_char_len--;
	}

	// Skip over any additional white space (useful when newindent is less
	// than old)
	ಆಗ (VIM_ISWHITE(*p))
	    ++p;

    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	todo = ಗಾತ್ರ_2;
	newline = alloc(ind_len + ಸಾಲು_len);
	ಆದರೆ (newline == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FALSE;
	s = newline;
    }

    // Put the characters in the new line.
    // if 'expandtab' isn't set: use TABs
    ಆದರೆ (!curbuf->b_p_et)
    {
	// If 'preserveindent' is set then reuse as much as possible of
	// the existing indent structure for the new indent
	ಆದರೆ (!(flags & SIN_ಸೇರಿಸು) && curbuf->b_p_pi)
	{
	    p = oldline;
	    ind_ಮುಗಿದ = 0;

	    ಆಗ (todo > 0 && VIM_ISWHITE(*p))
	    {
		ಆದರೆ (*p == TAB)
		{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
		    tab_pad = tabstop_padding(ind_ಮುಗಿದ, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#ಇಲ್ಲದಿದ್ದರೆ
		    tab_pad = (ಪೂರ್ಣಾಂಕ)curbuf->b_p_ts
					   - (ind_ಮುಗಿದ % (ಪೂರ್ಣಾಂಕ)curbuf->b_p_ts);
#ಆದರೆ_ಅಂತ್ಯ
		    // stop if this tab will overshoot the target
		    ಆದರೆ (todo < tab_pad)
			ನಿಲ್ಲಿಸು;
		    todo -= tab_pad;
		    ind_ಮುಗಿದ += tab_pad;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    --todo;
		    ++ind_ಮುಗಿದ;
		}
		*s++ = *p++;
	    }

	    // Fill to next tabstop with a tab, if possible
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
	    tab_pad = tabstop_padding(ind_ಮುಗಿದ, curbuf->b_p_ts,
						curbuf->b_p_vts_array);
#ಇಲ್ಲದಿದ್ದರೆ
	    tab_pad = (ಪೂರ್ಣಾಂಕ)curbuf->b_p_ts - (ind_ಮುಗಿದ % (ಪೂರ್ಣಾಂಕ)curbuf->b_p_ts);
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (todo >= tab_pad)
	    {
		*s++ = TAB;
		todo -= tab_pad;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
		ind_ಮುಗಿದ += tab_pad;
#ಆದರೆ_ಅಂತ್ಯ
	    }

	    p = skipwhite(p);
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
	ಗಾಗಿ (;;)
	{
	    tab_pad = tabstop_padding(ind_ಮುಗಿದ, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
	    ಆದರೆ (todo < tab_pad)
		ನಿಲ್ಲಿಸು;
	    *s++ = TAB;
	    todo -= tab_pad;
	    ind_ಮುಗಿದ += tab_pad;
	}
#ಇಲ್ಲದಿದ್ದರೆ
	ಆಗ (todo >= (ಪೂರ್ಣಾಂಕ)curbuf->b_p_ts)
	{
	    *s++ = TAB;
	    todo -= (ಪೂರ್ಣಾಂಕ)curbuf->b_p_ts;
	}
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಆಗ (todo > 0)
    {
	*s++ = ' ';
	--todo;
    }
    mch_memmove(s, p, (size_t)ಸಾಲು_len);

    // Replace the line (unless undo fails).
    ಆದರೆ (!(flags & SIN_UNDO) || u_savesub(curwin->w_cursor.lnum) == OK)
    {
	colnr_T old_ಸ್ಥಾನಾಂತರ = (colnr_T)(p - oldline);
	colnr_T new_ಸ್ಥಾನಾಂತರ = (colnr_T)(s - newline);

	// this may free "newline"
	ml_replace(curwin->w_cursor.lnum, newline, FALSE);
	ಆದರೆ (flags & SIN_ಬದಲಾಯಿತು)
	    ಬದಲಾಯಿತು_ಬೈಟುಗಳು(curwin->w_cursor.lnum, 0);

	// Correct saved cursor position if it is in this line.
	ಆದರೆ (saved_cursor.lnum == curwin->w_cursor.lnum)
	{
	    ಆದರೆ (saved_cursor.ಲಂಬಸಾಲು >= old_ಸ್ಥಾನಾಂತರ)
		// cursor was after the indent, adjust for the number of
		// bytes added/removed
		saved_cursor.ಲಂಬಸಾಲು += ind_len - old_ಸ್ಥಾನಾಂತರ;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (saved_cursor.ಲಂಬಸಾಲು >= new_ಸ್ಥಾನಾಂತರ)
		// cursor was in the indent, and is now after it, put it back
		// at the start of the indent (replacing spaces with TAB)
		saved_cursor.ಲಂಬಸಾಲು = new_ಸ್ಥಾನಾಂತರ;
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	ಆದರೆ (curbuf->b_has_textprop)
	{
	    ಪೂರ್ಣಾಂಕ added = ind_len - old_ಸ್ಥಾನಾಂತರ;

	    // When increasing indent this behaves like spaces were inserted at
	    // the old indent, when decreasing indent it behaves like spaces
	    // were deleted at the new indent.
	    adjust_prop_columns(curwin->w_cursor.lnum,
				  added > 0 ? old_ಸ್ಥಾನಾಂತರ : (colnr_T)ind_len,
				  added, APC_INDENT);
	}
#ಆದರೆ_ಅಂತ್ಯ
	retval = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	vim_free(newline);

    curwin->w_cursor.ಲಂಬಸಾಲು = ind_len;
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Return the indent of the current line after a number.  Return -1 if no
 * number was found.  Used for 'n' in 'formatoptions': numbered list.
 * Since a pattern is used it can actually handle more than numbers.
 */
    ಪೂರ್ಣಾಂಕ
get_ಸಂಖ್ಯೆ_indent(linenr_T lnum)
{
    colnr_T	ಲಂಬಸಾಲು;
    pos_T	pos;

    regmatch_T	regmatch;
    ಪೂರ್ಣಾಂಕ		lead_len = 0;	// length of comment leader

    ಆದರೆ (lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಹಿಂತಿರುಗಿ -1;
    pos.lnum = 0;

    // In format_lines() (i.e. not insert mode), fo+=q is needed too...
    ಆದರೆ ((State & ಬಹುಲಕ_ಸೇರಿಸು) || has_format_option(FO_Q_COMS))
	lead_len = get_leader_len(ml_get(lnum), ಖಾಲಿ, FALSE, TRUE);

    regmatch.regprog = vim_regcomp(curbuf->b_p_flp, RE_MAGIC);
    ಆದರೆ (regmatch.regprog != ಖಾಲಿ)
    {
	regmatch.rm_ic = FALSE;

	// vim_regexec() expects a pointer to a line.  This lets us
	// start matching for the flp beyond any comment leader...
	ಆದರೆ (vim_regexec(&regmatch, ml_get(lnum) + lead_len, (colnr_T)0))
	{
	    pos.lnum = lnum;
	    pos.ಲಂಬಸಾಲು = (colnr_T)(*regmatch.endp - ml_get(lnum));
	    pos.coladd = 0;
	}
	vim_regfree(regmatch.regprog);
    }

    ಆದರೆ (pos.lnum == 0 || *ml_get_pos(&pos) == NUL)
	ಹಿಂತಿರುಗಿ -1;
    getvcol(curwin, &pos, &ಲಂಬಸಾಲು, ಖಾಲಿ, ಖಾಲಿ, 0);
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)ಲಂಬಸಾಲು;
}

#ಆದರೆ defined(FEAT_LINEBREAK)
/*
 * Check "briopt" as 'breakindentopt' and update the members of "wp".
 * This is called when 'breakindentopt' is changed and when a window is
 * initialized.
 * Returns FAIL for failure, OK otherwise.
 */
    ಪೂರ್ಣಾಂಕ
briopt_check(
    char_u	*briopt,	// when NULL: use "wp->w_p_briopt"
    win_T	*wp)		// when NULL: only check "briopt"
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		bri_ಸರಿಸು = 0;
    ದೀರ್ಘ	bri_min = 20;
    ಪೂರ್ಣಾಂಕ		bri_sbr = FALSE;
    ಪೂರ್ಣಾಂಕ		bri_list = 0;
    ಪೂರ್ಣಾಂಕ		bri_vcol = 0;

    ಆದರೆ (briopt != ಖಾಲಿ)
	p = briopt;
    ಇಲ್ಲದಿದ್ದರೆ
	p = wp->w_p_briopt;

    ಆಗ (*p != NUL)
    {
	// Note: Keep this in sync with p_briopt_values
	ಆದರೆ (STRNCMP(p, "shift:", 6) == 0
		 && ((p[6] == '-' && VIM_ISDIGIT(p[7])) || VIM_ISDIGIT(p[6])))
	{
	    p += 6;
	    bri_ಸರಿಸು = getdigits(&p);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "min:", 4) == 0 && VIM_ISDIGIT(p[4]))
	{
	    p += 4;
	    bri_min = getdigits(&p);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "sbr", 3) == 0)
	{
	    p += 3;
	    bri_sbr = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "list:", 5) == 0)
	{
	    p += 5;
	    bri_list = getdigits(&p);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "column:", 7) == 0)
	{
	    p += 7;
	    bri_vcol = getdigits(&p);
	}
	ಆದರೆ (*p != ',' && *p != NUL)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆದರೆ (*p == ',')
	    ++p;
    }

    ಆದರೆ (wp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ OK;

    wp->w_briopt_ಸರಿಸು = bri_ಸರಿಸು;
    wp->w_briopt_min   = bri_min;
    wp->w_briopt_sbr   = bri_sbr;
    wp->w_briopt_list  = bri_list;
    wp->w_briopt_vcol  = bri_vcol;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Return appropriate space number for breakindent, taking influencing
 * parameters into account. Window must be specified, since it is not
 * necessarily always the current one.
 */
    ಪೂರ್ಣಾಂಕ
get_breakindent_win(
    win_T	*wp,
    char_u	*ಸಾಲು) // start of the line
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    prev_indent = 0;	// cached indent value
    ಸ್ಥಿರ ದೀರ್ಘ	    prev_ts     = 0L;	// cached tabstop value
# ifdef FEAT_VARTABS
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ      *prev_vts = ಖಾಲಿ;   // cached vartabs values
# endif
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    prev_fnum   = 0;	// cached buffer number
    ಸ್ಥಿರ char_u   *prev_ಸಾಲು  = ಖಾಲಿ;	// cached copy of "line"
    ಸ್ಥಿರ varnumber_T prev_tick = 0;   // changedtick of cached value
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ      prev_list = 0;	// cached list indent
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ      prev_listopt = 0;	// cached w_p_briopt_list value
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ      prev_no_ts = FALSE;	// cached no_ts value
    ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ prev_dy_uhex = 0;	// cached 'display' "uhex" value
    // cached formatlistpat value
    ಸ್ಥಿರ char_u   *prev_flp = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		    bri = 0;
    // window width minus window margin space, i.e. what rests for text
    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ	    eff_wwidth = wp->w_ಅಗಲ
					  - win_ಲಂಬಸಾಲು_off(wp) + win_ಲಂಬಸಾಲು_off2(wp);

    // In list mode, if 'listchars' "tab" isn't set, a TAB is displayed as ^I.
    ಪೂರ್ಣಾಂಕ no_ts = wp->w_p_list && wp->w_lcs_chars.tab1 == NUL;

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
    ಆದರೆ (prev_fnum != wp->w_ಬಫರ್->b_fnum
	    || prev_ts != wp->w_ಬಫರ್->b_p_ts
# ifdef FEAT_VARTABS
	    || prev_vts != wp->w_ಬಫರ್->b_p_vts_array
# endif
	    || prev_tick != CHANGEDTICK(wp->w_ಬಫರ್)
	    || prev_listopt != wp->w_briopt_list
	    || prev_no_ts != no_ts
	    || prev_dy_uhex != (dy_flags & DY_UHEX)
	    || prev_flp == ಖಾಲಿ
	    || STRCMP(prev_flp, get_flp_ಮೌಲ್ಯ(wp->w_ಬಫರ್)) != 0
	    || prev_ಸಾಲು == ಖಾಲಿ || STRCMP(prev_ಸಾಲು, ಸಾಲು) != 0
	)
    {
	prev_fnum = wp->w_ಬಫರ್->b_fnum;
	vim_free(prev_ಸಾಲು);
	prev_ಸಾಲು = vim_strsave(ಸಾಲು);
	prev_ts = wp->w_ಬಫರ್->b_p_ts;
# ifdef FEAT_VARTABS
	prev_vts = wp->w_ಬಫರ್->b_p_vts_array;
	ಆದರೆ (wp->w_briopt_vcol == 0)
	    prev_indent = get_indent_str_vtab(ಸಾಲು,
				     (ಪೂರ್ಣಾಂಕ)wp->w_ಬಫರ್->b_p_ts,
				      wp->w_ಬಫರ್->b_p_vts_array, no_ts);
# ಇಲ್ಲದಿದ್ದರೆ
	ಆದರೆ (wp->w_briopt_vcol == 0)
	    prev_indent = get_indent_str(ಸಾಲು,
					(ಪೂರ್ಣಾಂಕ)wp->w_ಬಫರ್->b_p_ts, no_ts);
# endif
	prev_tick = CHANGEDTICK(wp->w_ಬಫರ್);
	prev_listopt = wp->w_briopt_list;
	prev_list = 0;
	prev_no_ts = no_ts;
	prev_dy_uhex = (dy_flags & DY_UHEX);
	vim_free(prev_flp);
	prev_flp = vim_strsave(get_flp_ಮೌಲ್ಯ(wp->w_ಬಫರ್));
	// add additional indent for numbered lists
	ಆದರೆ (wp->w_briopt_list != 0 && wp->w_briopt_vcol == 0)
	{
	    regmatch_T	    regmatch;

	    regmatch.regprog = vim_regcomp(prev_flp,
				   RE_MAGIC + RE_STRING + RE_AUTO + RE_STRICT);

	    ಆದರೆ (regmatch.regprog != ಖಾಲಿ)
	    {
		regmatch.rm_ic = FALSE;
		ಆದರೆ (vim_regexec(&regmatch, ಸಾಲು, 0))
		{
		    ಆದರೆ (wp->w_briopt_list > 0)
			prev_list = wp->w_briopt_list;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			char_u	*ptr = *regmatch.startp;
			char_u	*end_ptr = *regmatch.endp;
			ಪೂರ್ಣಾಂಕ	indent = 0;

			// Compute the width of the matched text.
			// Use win_chartabsize() so that TAB size is correct,
			// while wrapping is ignored.
			ಆಗ (ptr < end_ptr)
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
    ಆದರೆ (wp->w_briopt_vcol != 0)
    {
	// column value has priority
	bri = wp->w_briopt_vcol;
	prev_list = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	bri = prev_indent + wp->w_briopt_ಸರಿಸು;

    // Add offset for number column, if 'n' is in 'cpoptions'
    bri += win_ಲಂಬಸಾಲು_off2(wp);

    // add additional indent for numbered lists
    ಆದರೆ (wp->w_briopt_list > 0)
	bri += prev_list;

    // indent minus the length of the showbreak string
    ಆದರೆ (wp->w_briopt_sbr)
	bri -= vim_strsize(get_showbreak_ಮೌಲ್ಯ(wp));


    // never indent past left window margin
    ಆದರೆ (bri < 0)
	bri = 0;

    // always leave at least bri_min characters on the left,
    // if text width is sufficient
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (bri > eff_wwidth - wp->w_briopt_min)
	bri = (eff_wwidth - wp->w_briopt_min < 0)
					   ? 0 : eff_wwidth - wp->w_briopt_min;

    ಹಿಂತಿರುಗಿ bri;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * When extra == 0: Return TRUE if the cursor is before or on the first
 *		    non-blank in the line.
 * When extra == 1: Return TRUE if the cursor is before the first non-blank in
 *		    the line.
 */
    ಪೂರ್ಣಾಂಕ
inindent(ಪೂರ್ಣಾಂಕ extra)
{
    char_u	*ptr;
    colnr_T	ಲಂಬಸಾಲು;

    ಗಾಗಿ (ಲಂಬಸಾಲು = 0, ptr = ml_get_curline(); VIM_ISWHITE(*ptr); ++ಲಂಬಸಾಲು)
	++ptr;
    ಆದರೆ (ಲಂಬಸಾಲು >= curwin->w_cursor.ಲಂಬಸಾಲು + extra)
	ಹಿಂತಿರುಗಿ TRUE;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * op_reindent - handle reindenting a block of lines.
 */
    ಶೂನ್ಯ
op_reindent(oparg_T *oap, ಪೂರ್ಣಾಂಕ (*how)(ಶೂನ್ಯ))
{
    ದೀರ್ಘ	i = 0;
    char_u	*l;
    ಪೂರ್ಣಾಂಕ		amount;
    linenr_T	ಮೊದಲ_ಬದಲಾಯಿತು = 0;
    linenr_T	ಕೊನೆಯ_ಬದಲಾಯಿತು = 0;
    linenr_T	ಆರಂಭ_lnum = curwin->w_cursor.lnum;

    // Don't even try when 'modifiable' is off.
    ಆದರೆ (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	ಹಿಂತಿರುಗಿ;
    }

    // Save for undo.  Do this once for all lines, much faster than doing this
    // for each line separately, especially when undoing.
    ಆದರೆ (u_savecommon(ಆರಂಭ_lnum - 1, ಆರಂಭ_lnum + oap->ಸಾಲು_ಎಣಿಕೆ,
				     ಆರಂಭ_lnum + oap->ಸಾಲು_ಎಣಿಕೆ, FALSE) == OK)
	ಗಾಗಿ (i = oap->ಸಾಲು_ಎಣಿಕೆ; --i >= 0 && !got_int; )
	{
	    // it's a slow thing to do, so give feedback so there's no worry
	    // that the computer's just hung.

	    ಆದರೆ (i > 1
		    && (i % 50 == 0 || i == oap->ಸಾಲು_ಎಣಿಕೆ - 1)
		    && oap->ಸಾಲು_ಎಣಿಕೆ > p_report)
		smsg(_("%ld lines to indent... "), i);

	    // Be vi-compatible: For lisp indenting the first line is not
	    // indented, unless there is only one line.
	    ಆದರೆ (i != oap->ಸಾಲು_ಎಣಿಕೆ - 1 || oap->ಸಾಲು_ಎಣಿಕೆ == 1
						     || how != get_lisp_indent)
	    {
		l = skipwhite(ml_get_curline());
		ಆದರೆ (*l == NUL)		    // empty or blank line
		    amount = 0;
		ಇಲ್ಲದಿದ್ದರೆ
		    amount = how();	    // get the indent for this line

		ಆದರೆ (amount >= 0 && ಗಣ_indent(amount, 0))
		{
		    // did change the indent, call changed_lines() later
		    ಆದರೆ (ಮೊದಲ_ಬದಲಾಯಿತು == 0)
			ಮೊದಲ_ಬದಲಾಯಿತು = curwin->w_cursor.lnum;
		    ಕೊನೆಯ_ಬದಲಾಯಿತು = curwin->w_cursor.lnum;
		}
	    }
	    ++curwin->w_cursor.lnum;
	    curwin->w_cursor.ಲಂಬಸಾಲು = 0;  // make sure it's valid
	}

    // put cursor on first non-blank of indented line
    curwin->w_cursor.lnum = ಆರಂಭ_lnum;
    beginline(BL_SOL | BL_FIX);

    // Mark changed lines so that they will be redrawn.  When Visual
    // highlighting was present, need to continue until the last line.  When
    // there is no change still need to remove the Visual highlighting.
    ಆದರೆ (ಕೊನೆಯ_ಬದಲಾಯಿತು != 0)
	ಬದಲಾಯಿತು_ಸಾಲುಗಳು(ಮೊದಲ_ಬದಲಾಯಿತು, 0,
		oap->is_VIsual ? ಆರಂಭ_lnum + oap->ಸಾಲು_ಎಣಿಕೆ :
		ಕೊನೆಯ_ಬದಲಾಯಿತು + 1, 0L);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (oap->is_VIsual)
	redraw_curbuf_later(UPD_INVERTED);

    ಆದರೆ (oap->ಸಾಲು_ಎಣಿಕೆ > p_report)
    {
	i = oap->ಸಾಲು_ಎಣಿಕೆ - (i + 1);
	smsg(NGETTEXT("%ld line indented ",
						 "%ld lines indented ", i), i);
    }
    ಆದರೆ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// set '[ and '] marks
	curbuf->b_op_ಆರಂಭ = oap->ಆರಂಭ;
	curbuf->b_op_ಕೊನೆ = oap->ಕೊನೆ;
    }
}

/*
 * Return TRUE if lines starting with '#' should be left aligned.
 */
    ಪೂರ್ಣಾಂಕ
preprocs_ಎಡ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ
	(curbuf->b_p_si && !curbuf->b_p_cin) ||
	(curbuf->b_p_cin && ಒಳ_cinkeys('#', ' ', TRUE)
					   && curbuf->b_ind_hash_comment == 0)
	;
}

/*
 * Return TRUE if the conditions are OK for smart indenting.
 */
    ಪೂರ್ಣಾಂಕ
may_do_si(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ curbuf->b_p_si
	&& !curbuf->b_p_cin
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	&& *curbuf->b_p_inde == NUL
#ಆದರೆ_ಅಂತ್ಯ
	&& !p_paste;
}

/*
 * Try to do some very smart auto-indenting.
 * Used when inserting a "normal" character.
 */
    ಶೂನ್ಯ
ins_try_si(ಪೂರ್ಣಾಂಕ c)
{
    pos_T	*pos, old_pos;
    char_u	*ptr;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		ತಾತ್ಕಾಲಿಕ;

    // do some very smart indenting when entering '{' or '}'
    ಆದರೆ (((did_si || can_si_ಹಿಂಭಾಗ) && c == '{')
	    || (can_si && c == '}' && inindent(0)))
    {
	// for '}' set indent equal to indent of line containing matching '{'
	ಆದರೆ (c == '}' && (pos = findmatch(ಖಾಲಿ, '{')) != ಖಾಲಿ)
	{
	    old_pos = curwin->w_cursor;
	    // If the matching '{' has a ')' immediately before it (ignoring
	    // white-space), then line up with the start of the line
	    // containing the matching '(' if there is one.  This handles the
	    // case where an "if (..\n..) {" statement continues over multiple
	    // lines -- webb
	    ptr = ml_get(pos->lnum);
	    i = pos->ಲಂಬಸಾಲು;
	    ಆದರೆ (i > 0)		// skip blanks before '{'
		ಆಗ (--i > 0 && VIM_ISWHITE(ptr[i]))
		    ;
	    curwin->w_cursor.lnum = pos->lnum;
	    curwin->w_cursor.ಲಂಬಸಾಲು = i;
	    ಆದರೆ (ptr[i] == ')' && (pos = findmatch(ಖಾಲಿ, '(')) != ಖಾಲಿ)
		curwin->w_cursor = *pos;
	    i = get_indent();
	    curwin->w_cursor = old_pos;
	    ಆದರೆ (State & VREPLACE_FLAG)
		change_indent(INDENT_ಗಣ, i, FALSE, NUL, TRUE);
	    ಇಲ್ಲದಿದ್ದರೆ
		(ಶೂನ್ಯ)ಗಣ_indent(i, SIN_ಬದಲಾಯಿತು);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು > 0)
	{
	    // when inserting '{' after "O" reduce indent, but not
	    // more than indent of previous line
	    ತಾತ್ಕಾಲಿಕ = TRUE;
	    ಆದರೆ (c == '{' && can_si_ಹಿಂಭಾಗ && curwin->w_cursor.lnum > 1)
	    {
		old_pos = curwin->w_cursor;
		i = get_indent();
		ಆಗ (curwin->w_cursor.lnum > 1)
		{
		    ptr = skipwhite(ml_get(--(curwin->w_cursor.lnum)));

		    // ignore empty lines and lines starting with '#'.
		    ಆದರೆ (*ptr != '#' && *ptr != NUL)
			ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (get_indent() >= i)
		    ತಾತ್ಕಾಲಿಕ = FALSE;
		curwin->w_cursor = old_pos;
	    }
	    ಆದರೆ (ತಾತ್ಕಾಲಿಕ)
		ಸರಿಸು_ಸಾಲು(TRUE, FALSE, 1, TRUE);
	}
    }

    // set indent of '#' always to 0
    ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು > 0 && can_si && c == '#' && inindent(0))
    {
	// remember current indent for next line
	old_indent = get_indent();
	(ಶೂನ್ಯ)ಗಣ_indent(0, SIN_ಬದಲಾಯಿತು);
    }

    // Adjust ai_col, the char at this position can be deleted.
    ಆದರೆ (ai_ಲಂಬಸಾಲು > curwin->w_cursor.ಲಂಬಸಾಲು)
	ai_ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;
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
    ಪೂರ್ಣಾಂಕ		type,
    ಪೂರ್ಣಾಂಕ		amount,
    ಪೂರ್ಣಾಂಕ		ಸುತ್ತು,
    ಪೂರ್ಣಾಂಕ		replaced,	// replaced character, put on replace stack
    ಪೂರ್ಣಾಂಕ		call_ಬದಲಾಯಿತು_ಬೈಟುಗಳು)	// call changed_bytes()
{
    ಪೂರ್ಣಾಂಕ		vcol;
    ಪೂರ್ಣಾಂಕ		ಕೊನೆಯ_vcol;
    ಪೂರ್ಣಾಂಕ		insstart_less;		// reduction for Insstart.col
    ಪೂರ್ಣಾಂಕ		new_cursor_ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*ptr;
    ಪೂರ್ಣಾಂಕ		save_p_list;
    ಪೂರ್ಣಾಂಕ		ಆರಂಭ_ಲಂಬಸಾಲು;
    colnr_T	vc;
    colnr_T	orig_ಲಂಬಸಾಲು = 0;		// init for GCC
    char_u	*new_ಸಾಲು, *orig_ಸಾಲು = ಖಾಲಿ;	// init for GCC

    // MODE_VREPLACE state needs to know what the line was like before changing
    ಆದರೆ (State & VREPLACE_FLAG)
    {
	orig_ಸಾಲು = vim_strnsave(ml_get_curline(), ml_get_curline_len());  // Deal with NULL below
	orig_ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;
    }

    // for the following tricks we don't want list mode
    save_p_list = curwin->w_p_list;
    curwin->w_p_list = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ignore_ಪಠ್ಯ_props = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
    vc = getvcol_nolist(&curwin->w_cursor);
    vcol = vc;

    // For Replace mode we need to fix the replace stack later, which is only
    // possible when the cursor is in the indent.  Remember the number of
    // characters before the cursor if it's possible.
    ಆರಂಭ_ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;

    // determine offset from first non-blank
    new_cursor_ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;
    beginline(BL_WHITE);
    new_cursor_ಲಂಬಸಾಲು -= curwin->w_cursor.ಲಂಬಸಾಲು;

    insstart_less = curwin->w_cursor.ಲಂಬಸಾಲು;

    // If the cursor is in the indent, compute how many screen columns the
    // cursor is to the left of the first non-blank.
    ಆದರೆ (new_cursor_ಲಂಬಸಾಲು < 0)
	vcol = get_indent() - vcol;

    ಆದರೆ (new_cursor_ಲಂಬಸಾಲು > 0)	    // can't fix replace stack
	ಆರಂಭ_ಲಂಬಸಾಲು = -1;

    // Set the new indent.  The cursor will be put on the first non-blank.
    ಆದರೆ (type == INDENT_ಗಣ)
	(ಶೂನ್ಯ)ಗಣ_indent(amount, call_ಬದಲಾಯಿತು_ಬೈಟುಗಳು ? SIN_ಬದಲಾಯಿತು : 0);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ	save_State = State;

	// Avoid being called recursively.
	ಆದರೆ (State & VREPLACE_FLAG)
	    State = ಬಹುಲಕ_ಸೇರಿಸು;
	ಸರಿಸು_ಸಾಲು(type == INDENT_DEC, ಸುತ್ತು, 1, call_ಬದಲಾಯಿತು_ಬೈಟುಗಳು);
	State = save_State;
    }
    insstart_less -= curwin->w_cursor.ಲಂಬಸಾಲು;

    // Try to put cursor on same character.
    // If the cursor is at or after the first non-blank in the line,
    // compute the cursor column relative to the column of the first
    // non-blank character.
    // If we are not in insert mode, leave the cursor on the first non-blank.
    // If the cursor is before the first non-blank, position it relative
    // to the first non-blank, counted in screen columns.
    ಆದರೆ (new_cursor_ಲಂಬಸಾಲು >= 0)
    {
	// When changing the indent while the cursor is touching it, reset
	// Insstart_col to 0.
	ಆದರೆ (new_cursor_ಲಂಬಸಾಲು == 0)
	    insstart_less = MAXCOL;
	new_cursor_ಲಂಬಸಾಲು += curwin->w_cursor.ಲಂಬಸಾಲು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!(State & ಬಹುಲಕ_ಸೇರಿಸು))
	new_cursor_ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	chartabsize_T cts;

	// Compute the screen column where the cursor should be.
	vcol = get_indent() - vcol;
	curwin->w_virtcol = (colnr_T)((vcol < 0) ? 0 : vcol);

	// Advance the cursor until we reach the right screen column.
	ಕೊನೆಯ_vcol = 0;
	ptr = ml_get_curline();
	init_chartabsize_arg(&cts, curwin, 0, 0, ptr, ptr);
	ಆಗ (cts.cts_vcol <= (ಪೂರ್ಣಾಂಕ)curwin->w_virtcol)
	{
	    ಕೊನೆಯ_vcol = cts.cts_vcol;
	    ಆದರೆ (cts.cts_vcol > 0)
		MB_PTR_ADV(cts.cts_ptr);
	    ಆದರೆ (*cts.cts_ptr == NUL)
		ನಿಲ್ಲಿಸು;
	    cts.cts_vcol += lbr_chartabsize(&cts);
	}
	vcol = ಕೊನೆಯ_vcol;
	new_cursor_ಲಂಬಸಾಲು = cts.cts_ptr - cts.cts_ಸಾಲು;
	clear_chartabsize_arg(&cts);

	// May need to insert spaces to be able to position the cursor on
	// the right screen column.
	ಆದರೆ (vcol != (ಪೂರ್ಣಾಂಕ)curwin->w_virtcol)
	{
	    curwin->w_cursor.ಲಂಬಸಾಲು = (colnr_T)new_cursor_ಲಂಬಸಾಲು;
	    i = (ಪೂರ್ಣಾಂಕ)curwin->w_virtcol - vcol;
	    ptr = alloc(i + 1);
	    ಆದರೆ (ptr != ಖಾಲಿ)
	    {
		size_t ptrlen;
		new_cursor_ಲಂಬಸಾಲು += i;
		ptr[i] = NUL;
		ptrlen = i;
		ಆಗ (--i >= 0)
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

    ಆದರೆ (new_cursor_ಲಂಬಸಾಲು <= 0)
	curwin->w_cursor.ಲಂಬಸಾಲು = 0;
    ಇಲ್ಲದಿದ್ದರೆ
	curwin->w_cursor.ಲಂಬಸಾಲು = (colnr_T)new_cursor_ಲಂಬಸಾಲು;
    curwin->w_ಗಣ_curswant = true;
    ಬದಲಾಯಿತು_cline_bef_curs();

    // May have to adjust the start of the insert.
    ಆದರೆ (State & ಬಹುಲಕ_ಸೇರಿಸು)
    {
	ಆದರೆ (curwin->w_cursor.lnum == Insstart.lnum && Insstart.ಲಂಬಸಾಲು != 0)
	{
	    ಆದರೆ ((ಪೂರ್ಣಾಂಕ)Insstart.ಲಂಬಸಾಲು <= insstart_less)
		Insstart.ಲಂಬಸಾಲು = 0;
	    ಇಲ್ಲದಿದ್ದರೆ
		Insstart.ಲಂಬಸಾಲು -= insstart_less;
	}
	ಆದರೆ ((ಪೂರ್ಣಾಂಕ)ai_ಲಂಬಸಾಲು <= insstart_less)
	    ai_ಲಂಬಸಾಲು = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	    ai_ಲಂಬಸಾಲು -= insstart_less;
    }

    // For MODE_REPLACE state, may have to fix the replace stack, if it's
    // possible.  If the number of characters before the cursor decreased, need
    // to pop a few characters from the replace stack.
    // If the number of characters before the cursor increased, need to push a
    // few NULs onto the replace stack.
    ಆದರೆ (REPLACE_ಸಾಮಾನ್ಯ(State) && ಆರಂಭ_ಲಂಬಸಾಲು >= 0)
    {
	ಆಗ (ಆರಂಭ_ಲಂಬಸಾಲು > (ಪೂರ್ಣಾಂಕ)curwin->w_cursor.ಲಂಬಸಾಲು)
	{
	    replace_join(0);	    // remove a NUL from the replace stack
	    --ಆರಂಭ_ಲಂಬಸಾಲು;
	}
	ಆಗ (ಆರಂಭ_ಲಂಬಸಾಲು < (ಪೂರ್ಣಾಂಕ)curwin->w_cursor.ಲಂಬಸಾಲು || replaced)
	{
	    replace_ತಳ್ಳು(NUL);
	    ಆದರೆ (replaced)
	    {
		replace_ತಳ್ಳು(replaced);
		replaced = NUL;
	    }
	    ++ಆರಂಭ_ಲಂಬಸಾಲು;
	}
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ignore_ಪಠ್ಯ_props = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    // For MODE_VREPLACE state, we also have to fix the replace stack.  In this
    // case it is always possible because we backspace over the whole line and
    // then put it back again the way we wanted it.
    ಆದರೆ (State & VREPLACE_FLAG)
    {
	// If orig_line didn't allocate, just return.  At least we did the job,
	// even if you can't backspace.
	ಆದರೆ (orig_ಸಾಲು == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;

	// Save new line
	new_ಸಾಲು = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	ಆದರೆ (new_ಸಾಲು == ಖಾಲಿ)
	{
	    vim_free(orig_ಸಾಲು);
	    ಹಿಂತಿರುಗಿ;
	}

	// We only put back the new line up to the cursor
	new_ಸಾಲು[curwin->w_cursor.ಲಂಬಸಾಲು] = NUL;

	// Put back original line
	ml_replace(curwin->w_cursor.lnum, orig_ಸಾಲು, FALSE);
	curwin->w_cursor.ಲಂಬಸಾಲು = orig_ಲಂಬಸಾಲು;

	// Backspace from cursor to start of line
	backspace_until_ಸ್ತಂಭ(0);

	// Insert new stuff into line again
	ins_ಬೈಟುಗಳು(new_ಸಾಲು);

	vim_free(new_ಸಾಲು);
    }
}

/*
 * Copy the indent from ptr to the current line (and fill to size)
 * Leaves the cursor on the first non-blank in the line.
 * Returns TRUE if the line was changed.
 */
    ಪೂರ್ಣಾಂಕ
copy_indent(ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2, char_u *src)
{
    char_u	*p = ಖಾಲಿ;
    char_u	*ಸಾಲು = ಖಾಲಿ;
    char_u	*s;
    ಪೂರ್ಣಾಂಕ		todo;
    ಪೂರ್ಣಾಂಕ		ind_len;
    ಪೂರ್ಣಾಂಕ		ಸಾಲು_len = 0;
    ಪೂರ್ಣಾಂಕ		tab_pad;
    ಪೂರ್ಣಾಂಕ		ind_ಮುಗಿದ;
    ಪೂರ್ಣಾಂಕ		ಸುತ್ತು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
    ಪೂರ್ಣಾಂಕ		ind_ಲಂಬಸಾಲು;
#ಆದರೆ_ಅಂತ್ಯ

    // Round 1: compute the number of characters needed for the indent
    // Round 2: copy the characters.
    ಗಾಗಿ (ಸುತ್ತು = 1; ಸುತ್ತು <= 2; ++ಸುತ್ತು)
    {
	todo = ಗಾತ್ರ_2;
	ind_len = 0;
	ind_ಮುಗಿದ = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
	ind_ಲಂಬಸಾಲು = 0;
#ಆದರೆ_ಅಂತ್ಯ
	s = src;

	// Count/copy the usable portion of the source line
	ಆಗ (todo > 0 && VIM_ISWHITE(*s))
	{
	    ಆದರೆ (*s == TAB)
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
		tab_pad = tabstop_padding(ind_ಮುಗಿದ, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#ಇಲ್ಲದಿದ್ದರೆ
		tab_pad = (ಪೂರ್ಣಾಂಕ)curbuf->b_p_ts
					   - (ind_ಮುಗಿದ % (ಪೂರ್ಣಾಂಕ)curbuf->b_p_ts);
#ಆದರೆ_ಅಂತ್ಯ
		// Stop if this tab will overshoot the target
		ಆದರೆ (todo < tab_pad)
		    ನಿಲ್ಲಿಸು;
		todo -= tab_pad;
		ind_ಮುಗಿದ += tab_pad;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
		ind_ಲಂಬಸಾಲು += tab_pad;
#ಆದರೆ_ಅಂತ್ಯ
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		--todo;
		++ind_ಮುಗಿದ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
		++ind_ಲಂಬಸಾಲು;
#ಆದರೆ_ಅಂತ್ಯ
	    }
	    ++ind_len;
	    ಆದರೆ (p != ಖಾಲಿ)
		*p++ = *s;
	    ++s;
	}

	// Fill to next tabstop with a tab, if possible
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
	tab_pad = tabstop_padding(ind_ಮುಗಿದ, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#ಇಲ್ಲದಿದ್ದರೆ
	tab_pad = (ಪೂರ್ಣಾಂಕ)curbuf->b_p_ts - (ind_ಮುಗಿದ % (ಪೂರ್ಣಾಂಕ)curbuf->b_p_ts);
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (todo >= tab_pad && !curbuf->b_p_et)
	{
	    todo -= tab_pad;
	    ++ind_len;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
	    ind_ಲಂಬಸಾಲು += tab_pad;
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (p != ಖಾಲಿ)
		*p++ = TAB;
	}

	// Add tabs required for indent
	ಆದರೆ (!curbuf->b_p_et)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
	    ಗಾಗಿ (;;)
	    {
		tab_pad = tabstop_padding(ind_ಲಂಬಸಾಲು, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
		ಆದರೆ (todo < tab_pad)
		    ನಿಲ್ಲಿಸು;
		todo -= tab_pad;
		++ind_len;
		ind_ಲಂಬಸಾಲು += tab_pad;
		ಆದರೆ (p != ಖಾಲಿ)
		    *p++ = TAB;
	    }
#ಇಲ್ಲದಿದ್ದರೆ
	    ಆಗ (todo >= (ಪೂರ್ಣಾಂಕ)curbuf->b_p_ts)
	    {
		todo -= (ಪೂರ್ಣಾಂಕ)curbuf->b_p_ts;
		++ind_len;
		ಆದರೆ (p != ಖಾಲಿ)
		    *p++ = TAB;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	}

	// Count/add spaces required for indent
	ಆಗ (todo > 0)
	{
	    --todo;
	    ++ind_len;
	    ಆದರೆ (p != ಖಾಲಿ)
		*p++ = ' ';
	}

	ಆದರೆ (p == ಖಾಲಿ)
	{
	    // Allocate memory for the result: the copied indent, new indent
	    // and the rest of the line.
	    ಸಾಲು_len = ml_get_curline_len() + 1;
	    ಸಾಲು = alloc(ind_len + ಸಾಲು_len);
	    ಆದರೆ (ಸಾಲು == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ FALSE;
	    p = ಸಾಲು;
	}
    }

    // Append the original line
    mch_memmove(p, ml_get_curline(), (size_t)ಸಾಲು_len);

    // Replace the line
    ml_replace(curwin->w_cursor.lnum, ಸಾಲು, FALSE);

    // Put the cursor after the indent.
    curwin->w_cursor.ಲಂಬಸಾಲು = ind_len;
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Give a "resulting text too long" error and maybe set got_int.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
emsg_ಪಠ್ಯ_too_long(ಶೂನ್ಯ)
{
    emsg(_(e_resulting_ಪಠ್ಯ_too_long));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // when not inside a try/catch set got_int to break out of any loop
    ಆದರೆ (trylevel == 0)
#ಆದರೆ_ಅಂತ್ಯ
	got_int = TRUE;
}

/*
 * ":retab".
 */
    ಶೂನ್ಯ
ex_retab(exarg_T *eap)
{
    linenr_T	lnum;
    ಪೂರ್ಣಾಂಕ		got_tab = FALSE;
    ದೀರ್ಘ	ಅಂಕೆ_spaces = 0;
    ದೀರ್ಘ	ಅಂಕೆ_tabs;
    ದೀರ್ಘ	len;
    ದೀರ್ಘ	ಲಂಬಸಾಲು;
    ದೀರ್ಘ	vcol;
    ದೀರ್ಘ	ಆರಂಭ_ಲಂಬಸಾಲು = 0;		// For start of white-space string
    ದೀರ್ಘ	ಆರಂಭ_vcol = 0;		// For start of white-space string
    ದೀರ್ಘ	old_len;
    ದೀರ್ಘ	new_len;
    char_u	*ptr;
    char_u	*new_ಸಾಲು = (char_u *)1; // init to non-NULL
    ಪೂರ್ಣಾಂಕ		did_undo;		// called u_save for current line
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
    ಪೂರ್ಣಾಂಕ		*new_vts_array = ಖಾಲಿ;
    char_u	*new_ts_str;		// string value of tab argument
#ಇಲ್ಲದಿದ್ದರೆ
    ಪೂರ್ಣಾಂಕ		ತಾತ್ಕಾಲಿಕ;
    ಪೂರ್ಣಾಂಕ		new_ts = 0;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		save_list;
    linenr_T	ಮೊದಲ_ಸಾಲು = 0;		// first changed line
    linenr_T	ಕೊನೆಯ_ಸಾಲು = 0;		// last changed line
    ಪೂರ್ಣಾಂಕ		is_indent_only = 0;	// Only process leading whitespace

    save_list = curwin->w_p_list;
    curwin->w_p_list = 0;	    // don't want list mode here

    ptr = eap->arg;
    ಆದರೆ (STRNCMP(ptr, "-indentonly", 11) == 0 && IS_WHITE_OR_NUL(ptr[11]))
    {
	is_indent_only = 1;
	ptr = skipwhite(ptr + 11);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
    new_ts_str = ptr;
    ಆದರೆ (tabstop_ಗಣ(ptr, &new_vts_array) == FAIL)
	ಹಿಂತಿರುಗಿ;
    ಆಗ (vim_isdigit(*ptr) || *ptr == ',')
	++ptr;

    // This ensures that either new_vts_array and new_ts_str are freshly
    // allocated, or new_vts_array points to an existing array and new_ts_str
    // is null.
    ಆದರೆ (new_vts_array == ಖಾಲಿ)
    {
	new_vts_array = curbuf->b_p_vts_array;
	new_ts_str = ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	new_ts_str = vim_strnsave(new_ts_str, ptr - new_ts_str);
#ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (ptr[0] != NUL && (ptr[0] != '0' || ptr[1] != NUL))
    {
	char_u	*ಕೊನೆ;

	ಆದರೆ (strtol((ಅಕ್ಷರ *)ptr, (ಅಕ್ಷರ **)&ಕೊನೆ, 10) <= 0)
	{
	    ಆದರೆ (ptr != ಕೊನೆ)
		emsg(_(e_ವಾದ_must_be_ಧನಾತ್ಮಕ));
	    ಇಲ್ಲದಿದ್ದರೆ
		semsg(_(e_invalid_ವಾದ_str), ptr);
	    ಹಿಂತಿರುಗಿ;
	}
	new_ts = getdigits(&ptr);
	ಆದರೆ (new_ts < 0 || new_ts > TABSTOP_MAX)
	{
	    semsg(_(e_invalid_ವಾದ_str), eap->arg);
	    ಹಿಂತಿರುಗಿ;
	}
    }
    ಆದರೆ (new_ts == 0)
	new_ts = curbuf->b_p_ts;
#ಆದರೆ_ಅಂತ್ಯ
    ಗಾಗಿ (lnum = eap->line1; !got_int && lnum <= eap->line2; ++lnum)
    {
	ptr = ml_get(lnum);
	old_len = ml_get_len(lnum);
	ಲಂಬಸಾಲು = 0;
	vcol = 0;
	did_undo = FALSE;
	ಗಾಗಿ (;;)
	{
	    ಆದರೆ (VIM_ISWHITE(ptr[ಲಂಬಸಾಲು]))
	    {
		ಆದರೆ (!got_tab && ಅಂಕೆ_spaces == 0)
		{
		    // First consecutive white-space
		    ಆರಂಭ_vcol = vcol;
		    ಆರಂಭ_ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
		}
		ಆದರೆ (ptr[ಲಂಬಸಾಲು] == ' ')
		    ಅಂಕೆ_spaces++;
		ಇಲ್ಲದಿದ್ದರೆ
		    got_tab = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (got_tab || (eap->forceit && ಅಂಕೆ_spaces > 1))
		{
		    // Retabulate this string of white-space

		    // len is virtual length of white string
		    len = ಅಂಕೆ_spaces = vcol - ಆರಂಭ_vcol;
		    ಅಂಕೆ_tabs = 0;
		    ಆದರೆ (!curbuf->b_p_et)
		    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
			ಪೂರ್ಣಾಂಕ t, s;

			tabstop_fromto(ಆರಂಭ_vcol, vcol,
					curbuf->b_p_ts, new_vts_array, &t, &s);
			ಅಂಕೆ_tabs = t;
			ಅಂಕೆ_spaces = s;
#ಇಲ್ಲದಿದ್ದರೆ
			ತಾತ್ಕಾಲಿಕ = new_ts - (ಆರಂಭ_vcol % new_ts);
			ಆದರೆ (ಅಂಕೆ_spaces >= ತಾತ್ಕಾಲಿಕ)
			{
			    ಅಂಕೆ_spaces -= ತಾತ್ಕಾಲಿಕ;
			    ಅಂಕೆ_tabs++;
			}
			ಅಂಕೆ_tabs += ಅಂಕೆ_spaces / new_ts;
			ಅಂಕೆ_spaces -= (ಅಂಕೆ_spaces / new_ts) * new_ts;
#ಆದರೆ_ಅಂತ್ಯ
		    }
		    ಆದರೆ (curbuf->b_p_et || got_tab ||
					(ಅಂಕೆ_spaces + ಅಂಕೆ_tabs < len))
		    {
			ಆದರೆ (did_undo == FALSE)
			{
			    did_undo = TRUE;
			    ಆದರೆ (u_save((linenr_T)(lnum - 1),
						(linenr_T)(lnum + 1)) == FAIL)
			    {
				new_ಸಾಲು = ಖಾಲಿ;	// flag out-of-memory
				ನಿಲ್ಲಿಸು;
			    }
			}

			// len is actual number of white characters used
			len = ಅಂಕೆ_spaces + ಅಂಕೆ_tabs;
			new_len = old_len - ಲಂಬಸಾಲು + ಆರಂಭ_ಲಂಬಸಾಲು + len + 1;
			ಆದರೆ (new_len <= 0 || new_len >= MAXCOL)
			{
			    emsg_ಪಠ್ಯ_too_long();
			    ನಿಲ್ಲಿಸು;
			}
			new_ಸಾಲು = alloc(new_len);
			ಆದರೆ (new_ಸಾಲು == ಖಾಲಿ)
			    ನಿಲ್ಲಿಸು;
			ಆದರೆ (ಆರಂಭ_ಲಂಬಸಾಲು > 0)
			    mch_memmove(new_ಸಾಲು, ptr, (size_t)ಆರಂಭ_ಲಂಬಸಾಲು);
			mch_memmove(new_ಸಾಲು + ಆರಂಭ_ಲಂಬಸಾಲು + len,
				      ptr + ಲಂಬಸಾಲು, (size_t)(old_len - ಲಂಬಸಾಲು + 1));
			ptr = new_ಸಾಲು + ಆರಂಭ_ಲಂಬಸಾಲು;
			ಗಾಗಿ (ಲಂಬಸಾಲು = 0; ಲಂಬಸಾಲು < len; ಲಂಬಸಾಲು++)
			    ptr[ಲಂಬಸಾಲು] = (ಲಂಬಸಾಲು < ಅಂಕೆ_tabs) ? '\t' : ' ';
			ಆದರೆ (ml_replace(lnum, new_ಸಾಲು, FALSE) == OK)
			    // "new_line" may have been copied
			    new_ಸಾಲು = curbuf->b_ml.ml_ಸಾಲು_ptr;
			ಆದರೆ (ಮೊದಲ_ಸಾಲು == 0)
			    ಮೊದಲ_ಸಾಲು = lnum;
			ಕೊನೆಯ_ಸಾಲು = lnum;
			ptr = new_ಸಾಲು;
			old_len = new_len - 1;
			ಲಂಬಸಾಲು = ಆರಂಭ_ಲಂಬಸಾಲು + len;
		    }
		}
		got_tab = FALSE;
		ಅಂಕೆ_spaces = 0;

		ಆದರೆ (is_indent_only)
		    ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (ptr[ಲಂಬಸಾಲು] == NUL)
		ನಿಲ್ಲಿಸು;
	    vcol += chartabsize(ptr + ಲಂಬಸಾಲು, (colnr_T)vcol);
	    ಆದರೆ (vcol >= MAXCOL)
	    {
		emsg_ಪಠ್ಯ_too_long();
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (has_mbyte)
		ಲಂಬಸಾಲು += (*mb_ptr2len)(ptr + ಲಂಬಸಾಲು);
	    ಇಲ್ಲದಿದ್ದರೆ
		++ಲಂಬಸಾಲು;
	}
	ಆದರೆ (new_ಸಾಲು == ಖಾಲಿ)		    // out of memory
	    ನಿಲ್ಲಿಸು;
	ಸಾಲು_breakcheck();
    }
    ಆದರೆ (got_int)
	emsg(_(e_interrupted));

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
    // If a single value was given then it can be considered equal to
    // either the value of 'tabstop' or the value of 'vartabstop'.
    ಆದರೆ (tabstop_ಎಣಿಕೆ(curbuf->b_p_vts_array) == 0
	&& tabstop_ಎಣಿಕೆ(new_vts_array) == 1
	&& curbuf->b_p_ts == tabstop_ಮೊದಲ(new_vts_array))
	; // not changed
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tabstop_ಎಣಿಕೆ(curbuf->b_p_vts_array) > 0
	&& tabstop_eq(curbuf->b_p_vts_array, new_vts_array))
	; // not changed
    ಇಲ್ಲದಿದ್ದರೆ
	redraw_curbuf_later(UPD_NOT_VALID);
#ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (curbuf->b_p_ts != new_ts)
	redraw_curbuf_later(UPD_NOT_VALID);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (ಮೊದಲ_ಸಾಲು != 0)
	ಬದಲಾಯಿತು_ಸಾಲುಗಳು(ಮೊದಲ_ಸಾಲು, 0, ಕೊನೆಯ_ಸಾಲು + 1, 0L);

    curwin->w_p_list = save_list;	// restore 'list'

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
    ಆದರೆ (new_ts_str != ಖಾಲಿ)		// set the new tabstop
    {
	// If 'vartabstop' is in use or if the value given to retab has more
	// than one tabstop then update 'vartabstop'.
	ಪೂರ್ಣಾಂಕ *old_vts_ary = curbuf->b_p_vts_array;

	ಆದರೆ (tabstop_ಎಣಿಕೆ(old_vts_ary) > 0 || tabstop_ಎಣಿಕೆ(new_vts_array) > 1)
	{
	    ಗಣ_string_option_ನೇರ((char_u *)"vts", -1, new_ts_str,
							OPT_FREE|OPT_LOCAL, 0);
	    curbuf->b_p_vts_array = new_vts_array;
	    vim_free(old_vts_ary);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // 'vartabstop' wasn't in use and a single value was given to
	    // retab then update 'tabstop'.
	    curbuf->b_p_ts = tabstop_ಮೊದಲ(new_vts_array);
	    vim_free(new_vts_array);
	}
	vim_free(new_ts_str);
    }
#ಇಲ್ಲದಿದ್ದರೆ
    curbuf->b_p_ts = new_ts;
#ಆದರೆ_ಅಂತ್ಯ
    coladvance(curwin->w_curswant);

    u_clearline();
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Get indent level from 'indentexpr'.
 */
    ಪೂರ್ಣಾಂಕ
get_ಅಭಿವ್ಯಕ್ತಿ_indent(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		indent = -1;
    char_u	*inde_copy;
    pos_T	save_pos;
    colnr_T	save_curswant;
    ಪೂರ್ಣಾಂಕ		save_ಗಣ_curswant;
    ಪೂರ್ಣಾಂಕ		save_State;
    ಪೂರ್ಣಾಂಕ		use_sandbox = was_ಗಣ_insecurely(curwin,
					    (char_u *)"indentexpr", OPT_LOCAL);
    sctx_T	save_sctx = current_sctx;

    // Save and restore cursor position and curswant, in case it was changed
    // via :normal commands
    save_pos = curwin->w_cursor;
    save_curswant = curwin->w_curswant;
    save_ಗಣ_curswant = curwin->w_ಗಣ_curswant;
    ಗಣ_vim_var_nr(VV_LNUM, curwin->w_cursor.lnum);
    ಆದರೆ (use_sandbox)
	++sandbox;
    ++textlock;
    current_sctx = curbuf->b_p_script_ctx[BV_INDE];

    // Need to make a copy, the 'indentexpr' option could be changed while
    // evaluating it.
    inde_copy = vim_strsave(curbuf->b_p_inde);
    ಆದರೆ (inde_copy != ಖಾಲಿ)
    {
	indent = (ಪೂರ್ಣಾಂಕ)eval_ಗಮ್ಯ_ಸಂಖ್ಯೆ(inde_copy, TRUE);
	vim_free(inde_copy);
    }

    ಆದರೆ (use_sandbox)
	--sandbox;
    --textlock;
    current_sctx = save_sctx;

    // Restore the cursor position so that 'indentexpr' doesn't need to.
    // Pretend to be in Insert mode, allow cursor past end of line for "o"
    // command.
    save_State = State;
    State = ಬಹುಲಕ_ಸೇರಿಸು;
    curwin->w_cursor = save_pos;
    curwin->w_curswant = save_curswant;
    curwin->w_ಗಣ_curswant = save_ಗಣ_curswant;
    check_cursor();
    State = save_State;

    // Reset did_throw, unless 'debug' has "throw" and inside a try/catch.
    ಆದರೆ (did_throw && (vim_strchr(p_debug, 't') == ಖಾಲಿ || trylevel == 0))
    {
	handle_did_throw();
	did_throw = FALSE;
    }

    // If there is an error, just keep the current indent.
    ಆದರೆ (indent < 0)
	indent = get_indent();

    ಹಿಂತಿರುಗಿ indent;
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lisp_ಹೊಂದಿಕೆ(char_u *p)
{
    char_u	buf[LSIZE];
    ಪೂರ್ಣಾಂಕ		len;
    char_u	*ಶಬ್ದ = *curbuf->b_p_lw != NUL ? curbuf->b_p_lw : p_lispwords;

    ಆಗ (*ಶಬ್ದ != NUL)
    {
	len = copy_option_part(&ಶಬ್ದ, buf, LSIZE, ",");
	ಆದರೆ (STRNCMP(buf, p, len) == 0 && IS_WHITE_OR_NUL(p[len]))
	    ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
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
    ಪೂರ್ಣಾಂಕ
get_lisp_indent(ಶೂನ್ಯ)
{
    pos_T	*pos, realpos, paren;
    ಪೂರ್ಣಾಂಕ		amount;
    char_u	*that;
    colnr_T	ಲಂಬಸಾಲು;
    colnr_T	firsttry;
    ಪೂರ್ಣಾಂಕ		parencount, quotecount;
    ಪೂರ್ಣಾಂಕ		vi_lisp;

    // Set vi_lisp to use the vi-compatible method
    vi_lisp = (vim_strchr(p_cpo, CPO_LISP) != ಖಾಲಿ);

    realpos = curwin->w_cursor;
    curwin->w_cursor.ಲಂಬಸಾಲು = 0;

    ಆದರೆ ((pos = findmatch(ಖಾಲಿ, '(')) == ಖಾಲಿ)
	pos = findmatch(ಖಾಲಿ, '[');
    ಇಲ್ಲದಿದ್ದರೆ
    {
	paren = *pos;
	pos = findmatch(ಖಾಲಿ, '[');
	ಆದರೆ (pos == ಖಾಲಿ || LT_POSP(pos, &paren))
	    pos = &paren;
    }
    ಆದರೆ (pos != ಖಾಲಿ)
    {
	// Extra trick: Take the indent of the first previous non-white
	// line that is at the same () level.
	amount = -1;
	parencount = 0;

	ಆಗ (--curwin->w_cursor.lnum >= pos->lnum)
	{
	    ಆದರೆ (linewhite(curwin->w_cursor.lnum))
		ಮುಂದುವರಿ;
	    ಗಾಗಿ (that = ml_get_curline(); *that != NUL; ++that)
	    {
		ಆದರೆ (*that == ';')
		{
		    ಆಗ (*(that + 1) != NUL)
			++that;
		    ಮುಂದುವರಿ;
		}
		ಆದರೆ (*that == '\\')
		{
		    ಆದರೆ (*(that + 1) != NUL)
			++that;
		    ಮುಂದುವರಿ;
		}
		ಆದರೆ (*that == '"' && *(that + 1) != NUL)
		{
		    ಆಗ (*++that && *that != '"')
		    {
			// skipping escaped characters in the string
			ಆದರೆ (*that == '\\')
			{
			    ಆದರೆ (*++that == NUL)
				ನಿಲ್ಲಿಸು;
			    ಆದರೆ (that[1] == NUL)
			    {
				++that;
				ನಿಲ್ಲಿಸು;
			    }
			}
		    }
		    ಆದರೆ (*that == NUL)
			ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (*that == '(' || *that == '[')
		    ++parencount;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*that == ')' || *that == ']')
		    --parencount;
	    }
	    ಆದರೆ (parencount == 0)
	    {
		amount = get_indent();
		ನಿಲ್ಲಿಸು;
	    }
	}

	ಆದರೆ (amount == -1)
	{
	    curwin->w_cursor.lnum = pos->lnum;
	    curwin->w_cursor.ಲಂಬಸಾಲು = pos->ಲಂಬಸಾಲು;
	    ಲಂಬಸಾಲು = pos->ಲಂಬಸಾಲು;

	    that = ml_get_curline();

	    ಆದರೆ (vi_lisp && get_indent() == 0)
		amount = 2;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		char_u		*ಸಾಲು = that;
		chartabsize_T	cts;

		init_chartabsize_arg(&cts, curwin, pos->lnum, 0, ಸಾಲು, ಸಾಲು);
		ಆಗ (*cts.cts_ptr != NUL && ಲಂಬಸಾಲು > 0)
		{
		    cts.cts_vcol += lbr_chartabsize_adv(&cts);
		    ಲಂಬಸಾಲು--;
		}
		amount = cts.cts_vcol;
		that = cts.cts_ptr;
		clear_chartabsize_arg(&cts);

		// Some keywords require "body" indenting rules (the
		// non-standard-lisp ones are Scheme special forms):
		//
		// (let ((a 1))    instead    (let ((a 1))
		//   (...))	      of	   (...))

		ಆದರೆ (!vi_lisp && (*that == '(' || *that == '[')
						      && lisp_ಹೊಂದಿಕೆ(that + 1))
		    amount += 2;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (*that != NUL)
		    {
			that++;
			amount++;
		    }
		    firsttry = amount;

		    init_chartabsize_arg(&cts, curwin, (colnr_T)(that - ಸಾಲು),
							   amount, ಸಾಲು, that);
		    ಆಗ (VIM_ISWHITE(*cts.cts_ptr))
		    {
			cts.cts_vcol += lbr_chartabsize(&cts);
			++cts.cts_ptr;
		    }
		    that = cts.cts_ptr;
		    amount = cts.cts_vcol;
		    clear_chartabsize_arg(&cts);

		    ಆದರೆ (*that && *that != ';') // not a comment line
		    {
			// test *that != '(' to accommodate first let/do
			// argument if it is more than one line
			ಆದರೆ (!vi_lisp && *that != '(' && *that != '[')
			    firsttry++;

			parencount = 0;
			quotecount = 0;

			init_chartabsize_arg(&cts, curwin,
				   (colnr_T)(that - ಸಾಲು), amount, ಸಾಲು, that);
			ಆದರೆ (vi_lisp
				|| (*that != '"'
				    && *that != '\''
				    && *that != '#'
				    && (*that < '0' || *that > '9')))
			{
			    ಆಗ (*cts.cts_ptr
				    && (!VIM_ISWHITE(*cts.cts_ptr)
					|| quotecount
					|| parencount)
				    && (!((*cts.cts_ptr == '('
							|| *cts.cts_ptr == '[')
					    && !quotecount
					    && !parencount
					    && vi_lisp)))
			    {
				ಆದರೆ (*cts.cts_ptr == '"')
				    quotecount = !quotecount;
				ಆದರೆ ((*cts.cts_ptr == '(' || *cts.cts_ptr == '[')
							       && !quotecount)
				    ++parencount;
				ಆದರೆ ((*cts.cts_ptr == ')' || *cts.cts_ptr == ']')
							       && !quotecount)
				    --parencount;
				ಆದರೆ (*cts.cts_ptr == '\\'
						    && *(cts.cts_ptr+1) != NUL)
				    cts.cts_vcol += lbr_chartabsize_adv(&cts);
				cts.cts_vcol += lbr_chartabsize_adv(&cts);
			    }
			}
			ಆಗ (VIM_ISWHITE(*cts.cts_ptr))
			{
			    cts.cts_vcol += lbr_chartabsize(&cts);
			    ++cts.cts_ptr;
			}
			that = cts.cts_ptr;
			amount = cts.cts_vcol;
			clear_chartabsize_arg(&cts);

			ಆದರೆ (!*that || *that == ';')
			    amount = firsttry;
		    }
		}
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	amount = 0;	// no matching '(' or '[' found, use zero indent

    curwin->w_cursor = realpos;

    ಹಿಂತಿರುಗಿ amount;
}

/*
 * Re-indent the current line, based on the current contents of it and the
 * surrounding lines. Fixing the cursor position seems really easy -- I'm very
 * confused what all the part that handles Control-T is doing that I'm not.
 * "get_the_indent" should be get_c_indent, get_expr_indent or get_lisp_indent.
 */

    ಶೂನ್ಯ
fixthisline(ಪೂರ್ಣಾಂಕ (*get_the_indent)(ಶೂನ್ಯ))
{
    ಪೂರ್ಣಾಂಕ amount = get_the_indent();

    ಆದರೆ (amount < 0)
	ಹಿಂತಿರುಗಿ;

    change_indent(INDENT_ಗಣ, amount, FALSE, 0, TRUE);
    ಆದರೆ (linewhite(curwin->w_cursor.lnum))
	did_ai = TRUE;	// delete the indent if the line stays empty
}

/*
 * Return TRUE if 'indentexpr' should be used for Lisp indenting.
 * Caller may want to check 'autoindent'.
 */
    ಪೂರ್ಣಾಂಕ
use_indentexpr_for_lisp(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಹಿಂತಿರುಗಿ curbuf->b_p_lisp
		&& *curbuf->b_p_inde != NUL
		&& STRCMP(curbuf->b_p_lop, "expr:1") == 0;
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ FALSE;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Fix indent for 'lisp' and 'cindent'.
 */
    ಶೂನ್ಯ
fix_indent(ಶೂನ್ಯ)
{
    ಆದರೆ (p_paste)
	ಹಿಂತಿರುಗಿ;  // no auto-indenting when 'paste' is set
    ಆದರೆ (curbuf->b_p_lisp && curbuf->b_p_ai)
    {
	ಆದರೆ (use_indentexpr_for_lisp())
	    do_c_ಅಭಿವ್ಯಕ್ತಿ_indent();
	ಇಲ್ಲದಿದ್ದರೆ
	    fixthisline(get_lisp_indent);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cindent_on())
	do_c_ಅಭಿವ್ಯಕ್ತಿ_indent();
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * "indent()" function
 */
    ಶೂನ್ಯ
f_indent(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    ಆದರೆ (ಒಳ_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    lnum = tv_get_lnum(argvars);
    ಆದರೆ (lnum >= 1 && lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	rettv->vval.v_ಸಂಖ್ಯೆ = get_indent_lnum(lnum);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (ಒಳ_vim9script())
	    semsg(_(e_invalid_ಸಾಲು_ಸಂಖ್ಯೆ_nr), lnum);
	rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    }
}

/*
 * "lispindent(lnum)" function
 */
    ಶೂನ್ಯ
f_lispindent(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    pos_T	pos;
    linenr_T	lnum;

    ಆದರೆ (ಒಳ_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    ಆದರೆ (lnum >= 1 && lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_ಸಂಖ್ಯೆ = get_lisp_indent();
	curwin->w_cursor = pos;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಒಳ_vim9script())
	semsg(_(e_invalid_ಸಾಲು_ಸಂಖ್ಯೆ_nr), lnum);
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_ಸಂಖ್ಯೆ = -1;
}
#ಆದರೆ_ಅಂತ್ಯ
