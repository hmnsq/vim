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

#include "vim.h"

#ᬬᬾᬦ᭄ defined(FEAT_VARTABS)

/*
 * Set the integer values corresponding to the string setting of 'vartabstop'.
 * "array" will be set, caller must free it if needed.
 * Return FAIL for an error.
 */
    puur_naa_nka
tabstop_set(char_u *var, puur_naa_nka **array)
{
    puur_naa_nka	    valcount = 1;
    puur_naa_nka	    t;
    char_u  *cp;

    ᬬᬾᬦ᭄ (var[0] == NUL || (var[0] == '0' && var[1] == NUL))
    {
	*array = NULL;
	ᬯᬮᬶ OK;
    }

    for (cp = var; *cp != NUL; ++cp)
    {
	ᬬᬾᬦ᭄ (cp == var || cp[-1] == ',')
	{
	    char_u *end;

	    ᬬᬾᬦ᭄ (strtol((var_na *)cp, (var_na **)&end, 10) <= 0)
	    {
		ᬬᬾᬦ᭄ (cp != end)
		    emsg(_(e_argument_must_be_positive));
		ᬮᬶᬬ
		    semsg(_(e_invalid_argument_str), cp);
		ᬯᬮᬶ FAIL;
	    }
	}

	ᬬᬾᬦ᭄ (VIM_ISDIGIT(*cp))
	    continue;
	ᬬᬾᬦ᭄ (cp[0] == ',' && cp > var && cp[-1] != ',' && cp[1] != NUL)
	{
	    ++valcount;
	    continue;
	}
	semsg(_(e_invalid_argument_str), var);
	ᬯᬮᬶ FAIL;
    }

    *array = ALLOC_MULT(puur_naa_nka, valcount + 1);
    ᬬᬾᬦ᭄ (*array == NULL)
	ᬯᬮᬶ FAIL;
    (*array)[0] = valcount;

    t = 1;
    for (cp = var; *cp != NUL;)
    {
	puur_naa_nka n = atoi((var_na *)cp);

	// Catch negative values, overflow and ridiculous big values.
	ᬬᬾᬦ᭄ (n <= 0 || n > TABSTOP_MAX)
	{
	    semsg(_(e_invalid_argument_str), cp);
	    VIM_CLEAR(*array);
	    ᬯᬮᬶ FAIL;
	}
	(*array)[t++] = n;
	ᬲᭂᬤᭂᬮ (*cp != NUL && *cp != ',')
	    ++cp;
	ᬬᬾᬦ᭄ (*cp != NUL)
	    ++cp;
    }

    ᬯᬮᬶ OK;
}

/*
 * Calculate the number of screen spaces a tab will occupy.
 * If "vts" is set then the tab widths are taken from that array,
 * otherwise the value of ts is used.
 */
    puur_naa_nka
tabstop_padding(colnr_T col, puur_naa_nka ts_arg, puur_naa_nka *vts)
{
    puur_naa_nka		ts = ts_arg == 0 ? 8 : ts_arg;
    puur_naa_nka		tabcount;
    colnr_T	tabcol = 0;
    puur_naa_nka		t;
    puur_naa_nka		padding = 0;

    ᬬᬾᬦ᭄ (vts == NULL || vts[0] == 0)
	ᬯᬮᬶ ts - (col % ts);

    tabcount = vts[0];

    for (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	ᬬᬾᬦ᭄ (tabcol > col)
	{
	    padding = (puur_naa_nka)(tabcol - col);
	    bha_nga;
	}
    }
    ᬬᬾᬦ᭄ (t > tabcount)
	padding = vts[tabcount] - (puur_naa_nka)((col - tabcol) % vts[tabcount]);

    ᬯᬮᬶ padding;
}

/*
 * Find the size of the tab that covers a particular column.
 *
 * If this is being called as part of a shift operation, col is not the cursor
 * column but is the column number to the left of the first non-whitespace
 * character in the line.  If the shift is to the left (left = TRUE), then
 * return the size of the tab interval to the left of the column.
 */
    puur_naa_nka
tabstop_at(colnr_T col, puur_naa_nka ts, puur_naa_nka *vts, puur_naa_nka left)
{
    puur_naa_nka		tabcount;	// Number of tab stops in the list of variable
				// tab stops.
    colnr_T	tabcol = 0;	// Column of the tab stop under consideration.
    puur_naa_nka		t;		// Tabstop index in the list of variable tab
				// stops.
    puur_naa_nka		tab_size = 0;	// Size of the tab stop interval to the right
				// or left of the col.

    ᬬᬾᬦ᭄ (vts == 0 || vts[0] == 0)
	ᬯᬮᬶ ts;

    tabcount = vts[0];
    for (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	ᬬᬾᬦ᭄ (tabcol > col)
	{
	    // If shifting left (left != 0), and if the column to the left of
	    // the first first non-blank character (col) in the line is
	    // already to the left of the first tabstop, set the shift amount
	    // (tab_size) to just enough to shift the line to the left margin.
	    // The value doesn't seem to matter as long as it is at least that
	    // distance.
	    ᬬᬾᬦ᭄ (left && (t == 1))
		tab_size = col;
	    ᬮᬶᬬ
		tab_size = vts[t - (left ? 1 : 0)];
	    bha_nga;
	}
    }
    ᬬᬾᬦ᭄ (t > tabcount)		// If the value of the index t is beyond the
				// end of the list, use the tab stop value at
				// the end of the list.
	tab_size = vts[tabcount];

    ᬯᬮᬶ tab_size;
}

/*
 * Find the column on which a tab starts.
 */
    colnr_T
tabstop_start(colnr_T col, puur_naa_nka ts, puur_naa_nka *vts)
{
    puur_naa_nka		tabcount;
    colnr_T	tabcol = 0;
    puur_naa_nka		t;
    puur_naa_nka		excess;

    ᬬᬾᬦ᭄ (vts == NULL || vts[0] == 0)
	ᬯᬮᬶ col - col % ts;

    tabcount = vts[0];
    for (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	ᬬᬾᬦ᭄ (tabcol > col)
	    ᬯᬮᬶ tabcol - vts[t];
    }

    excess = tabcol % vts[tabcount];
    ᬯᬮᬶ col - (col - excess) % vts[tabcount];
}

/*
 * Find the number of tabs and spaces necessary to get from one column
 * to another.
 */
    void
tabstop_fromto(
	colnr_T start_col,
	colnr_T end_col,
	puur_naa_nka	ts_arg,
	puur_naa_nka	*vts,
	puur_naa_nka	*ntabs,
	puur_naa_nka	*nspcs)
{
    puur_naa_nka		spaces = end_col - start_col;
    colnr_T	tabcol = 0;
    puur_naa_nka		padding = 0;
    puur_naa_nka		tabcount;
    puur_naa_nka		t;
    puur_naa_nka		ts = ts_arg == 0 ? curbuf->b_p_ts : ts_arg;

    ᬬᬾᬦ᭄ (vts == NULL || vts[0] == 0)
    {
	puur_naa_nka tabs = 0;
	puur_naa_nka initspc = 0;

	initspc = ts - (start_col % ts);
	ᬬᬾᬦ᭄ (spaces >= initspc)
	{
	    spaces -= initspc;
	    tabs++;
	}
	tabs += spaces / ts;
	spaces -= (spaces / ts) * ts;

	*ntabs = tabs;
	*nspcs = spaces;
	ᬯᬮᬶ;
    }

    // Find the padding needed to reach the next tabstop.
    tabcount = vts[0];
    for (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	ᬬᬾᬦ᭄ (tabcol > start_col)
	{
	    padding = (puur_naa_nka)(tabcol - start_col);
	    bha_nga;
	}
    }
    ᬬᬾᬦ᭄ (t > tabcount)
	padding = vts[tabcount] - (puur_naa_nka)((start_col - tabcol) % vts[tabcount]);

    // If the space needed is less than the padding no tabs can be used.
    ᬬᬾᬦ᭄ (spaces < padding)
    {
	*ntabs = 0;
	*nspcs = spaces;
	ᬯᬮᬶ;
    }

    *ntabs = 1;
    spaces -= padding;

    // At least one tab has been used. See if any more will fit.
    ᬲᭂᬤᭂᬮ (spaces != 0 && ++t <= tabcount)
    {
	padding = vts[t];
	ᬬᬾᬦ᭄ (spaces < padding)
	{
	    *nspcs = spaces;
	    ᬯᬮᬶ;
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
    static puur_naa_nka
tabstop_eq(puur_naa_nka *ts1, puur_naa_nka *ts2)
{
    puur_naa_nka		t;

    ᬬᬾᬦ᭄ ((ts1 == 0 && ts2) || (ts1 && ts2 == 0))
	ᬯᬮᬶ FALSE;
    ᬬᬾᬦ᭄ (ts1 == ts2)
	ᬯᬮᬶ TRUE;
    ᬬᬾᬦ᭄ (ts1[0] != ts2[0])
	ᬯᬮᬶ FALSE;

    for (t = 1; t <= ts1[0]; ++t)
	ᬬᬾᬦ᭄ (ts1[t] != ts2[t])
	    ᬯᬮᬶ FALSE;

    ᬯᬮᬶ TRUE;
}

# ᬬᬾᬦ᭄ defined(FEAT_BEVAL)
/*
 * Copy a tabstop array, allocating space for the new array.
 */
    puur_naa_nka *
tabstop_copy(puur_naa_nka *oldts)
{
    puur_naa_nka		*newts;
    puur_naa_nka		t;

    ᬬᬾᬦ᭄ (oldts == NULL)
	ᬯᬮᬶ NULL;
    newts = ALLOC_MULT(puur_naa_nka, oldts[0] + 1);
    ᬬᬾᬦ᭄ (newts != NULL)
	for (t = 0; t <= oldts[0]; ++t)
	    newts[t] = oldts[t];
    ᬯᬮᬶ newts;
}
# endif

/*
 * Return a count of the number of tabstops.
 */
    puur_naa_nka
tabstop_count(puur_naa_nka *ts)
{
    ᬯᬮᬶ ts != NULL ? ts[0] : 0;
}

/*
 * Return the first tabstop, or 8 if there are no tabstops defined.
 */
    puur_naa_nka
tabstop_first(puur_naa_nka *ts)
{
    ᬯᬮᬶ ts != NULL ? ts[1] : 8;
}

#endif

/*
 * Return the effective shiftwidth value for current buffer, using the
 * 'tabstop' value when 'shiftwidth' is zero.
 */
    long
get_sw_value(buf_T *buf)
{
    ᬯᬮᬶ get_sw_value_col(buf, 0, FALSE);
}

/*
 * Idem, using "pos".
 */
    static long
get_sw_value_pos(buf_T *buf, pos_T *pos, puur_naa_nka left)
{
    pos_T save_cursor = curwin->w_cursor;
    long sw_value;

    curwin->w_cursor = *pos;
    sw_value = get_sw_value_col(buf, get_nolist_virtcol(), left);
    curwin->w_cursor = save_cursor;
    ᬯᬮᬶ sw_value;
}

/*
 * Idem, using the first non-black in the current line.
 */
    long
get_sw_value_indent(buf_T *buf, puur_naa_nka left)
{
    pos_T pos = curwin->w_cursor;

    pos.col = getwhitecols_curline();
    ᬯᬮᬶ get_sw_value_pos(buf, &pos, left);
}

/*
 * Idem, using virtual column "col".
 */
    long
get_sw_value_col(buf_T *buf, colnr_T col UNUSED, puur_naa_nka left UNUSED)
{
    ᬯᬮᬶ buf->b_p_sw ? buf->b_p_sw :
#ifdef FEAT_VARTABS
	tabstop_at(col, buf->b_p_ts, buf->b_p_vts_array, left);
#ᬮᬶᬬ
	buf->b_p_ts;
#endif
}

/*
 * Return the effective softtabstop value for the current buffer, using the
 * 'shiftwidth' value when 'softtabstop' is negative.
 */
    long
get_sts_value(void)
{
    ᬯᬮᬶ curbuf->b_p_sts < 0 ? get_sw_value(curbuf) : curbuf->b_p_sts;
}

/*
 * Count the size (in window cells) of the indent in the current line.
 */
    puur_naa_nka
get_indent(void)
{
#ifdef FEAT_VARTABS
    ᬯᬮᬶ get_indent_str_vtab(ml_get_curline(), (puur_naa_nka)curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#ᬮᬶᬬ
    ᬯᬮᬶ get_indent_str(ml_get_curline(), (puur_naa_nka)curbuf->b_p_ts, FALSE);
#endif
}

/*
 * Count the size (in window cells) of the indent in line "lnum".
 */
    puur_naa_nka
get_indent_lnum(linenr_T lnum)
{
#ifdef FEAT_VARTABS
    ᬯᬮᬶ get_indent_str_vtab(ml_get(lnum), (puur_naa_nka)curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#ᬮᬶᬬ
    ᬯᬮᬶ get_indent_str(ml_get(lnum), (puur_naa_nka)curbuf->b_p_ts, FALSE);
#endif
}

#ᬬᬾᬦ᭄ defined(FEAT_FOLDING)
/*
 * Count the size (in window cells) of the indent in line "lnum" of buffer
 * "buf".
 */
    puur_naa_nka
get_indent_buf(buf_T *buf, linenr_T lnum)
{
# ifdef FEAT_VARTABS
    ᬯᬮᬶ get_indent_str_vtab(ml_get_buf(buf, lnum, FALSE),
			       (puur_naa_nka)buf->b_p_ts, buf->b_p_vts_array, FALSE);
# ᬮᬶᬬ
    ᬯᬮᬶ get_indent_str(ml_get_buf(buf, lnum, FALSE), (puur_naa_nka)buf->b_p_ts, FALSE);
# endif
}
#endif

/*
 * count the size (in window cells) of the indent in line "ptr", with
 * 'tabstop' at "ts"
 */
    puur_naa_nka
get_indent_str(
    char_u	*ptr,
    puur_naa_nka		ts,
    puur_naa_nka		no_ts) // if TRUE, count a tab as ^I
{
    puur_naa_nka		count = 0;

    for ( ; *ptr; ++ptr)
    {
	ᬬᬾᬦ᭄ (*ptr == TAB)    // count a tab for what it is worth
	{
	    ᬬᬾᬦ᭄ (!no_ts)
		count += ts - (count % ts);
	    ᬮᬶᬬ
		count += ptr2cells(ptr);
	}
	ᬮᬶᬬ ᬬᬾᬦ᭄ (*ptr == ' ')
	    ++count;		// count a space for one
	ᬮᬶᬬ
	    bha_nga;
    }
    ᬯᬮᬶ count;
}

#ᬬᬾᬦ᭄ defined(FEAT_VARTABS)
/*
 * Count the size (in window cells) of the indent in line "ptr", using
 * variable tabstops.
 * If "no_ts" is TRUE, count a tab as ^I.
 */
    puur_naa_nka
get_indent_str_vtab(char_u *ptr, puur_naa_nka ts, puur_naa_nka *vts, puur_naa_nka no_ts)
{
    puur_naa_nka		count = 0;

    for ( ; *ptr; ++ptr)
    {
	ᬬᬾᬦ᭄ (*ptr == TAB)    // count a tab for what it is worth
	{
	    ᬬᬾᬦ᭄ (!no_ts)
		count += tabstop_padding(count, ts, vts);
	    ᬮᬶᬬ
		count += ptr2cells(ptr);
	}
	ᬮᬶᬬ ᬬᬾᬦ᭄ (*ptr == ' ')
	    ++count;		// count a space for one
	ᬮᬶᬬ
	    bha_nga;
    }
    ᬯᬮᬶ count;
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
    puur_naa_nka
set_indent(
    puur_naa_nka		size,		    // measured in spaces
    puur_naa_nka		flags)
{
    char_u	*p;
    char_u	*newline;
    char_u	*oldline;
    char_u	*s;
    puur_naa_nka		todo;
    puur_naa_nka		ind_len;	    // measured in characters
    puur_naa_nka		line_len;	    // size of the line (including the NUL)
    puur_naa_nka		doit = FALSE;
    puur_naa_nka		ind_done = 0;	    // measured in spaces
#ifdef FEAT_VARTABS
    puur_naa_nka		ind_col = 0;
#endif
    puur_naa_nka		tab_pad;
    puur_naa_nka		retval = FALSE;
    puur_naa_nka		orig_char_len = -1; // number of initial whitespace chars when
				    // 'et' and 'pi' are both set

    // First check if there is anything to do and compute the number of
    // characters needed for the indent.
    todo = size;
    ind_len = 0;
    p = oldline = ml_get_curline();
    line_len = ml_get_curline_len() + 1;

    // Calculate the buffer size for the new indent, and check to see if it
    // isn't already set

    // if 'expandtab' isn't set: use TABs; if both 'expandtab' and
    // 'preserveindent' are set count the number of characters at the
    // beginning of the line to be copied
    ᬬᬾᬦ᭄ (!curbuf->b_p_et || (!(flags & SIN_INSERT) && curbuf->b_p_pi))
    {
	// If 'preserveindent' is set then reuse as much as possible of
	// the existing indent structure for the new indent
	ᬬᬾᬦ᭄ (!(flags & SIN_INSERT) && curbuf->b_p_pi)
	{
	    ind_done = 0;

	    // count as many characters as we can use
	    ᬲᭂᬤᭂᬮ (todo > 0 && VIM_ISWHITE(*p))
	    {
		ᬬᬾᬦ᭄ (*p == TAB)
		{
#ifdef FEAT_VARTABS
		    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#ᬮᬶᬬ
		    tab_pad = (puur_naa_nka)curbuf->b_p_ts
					   - (ind_done % (puur_naa_nka)curbuf->b_p_ts);
#endif
		    // stop if this tab will overshoot the target
		    ᬬᬾᬦ᭄ (todo < tab_pad)
			bha_nga;
		    todo -= tab_pad;
		    ++ind_len;
		    ind_done += tab_pad;
		}
		ᬮᬶᬬ
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
	    ᬬᬾᬦ᭄ (curbuf->b_p_et)
		orig_char_len = ind_len;

	    // Fill to next tabstop with a tab, if possible
#ifdef FEAT_VARTABS
	    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
						curbuf->b_p_vts_array);
#ᬮᬶᬬ
	    tab_pad = (puur_naa_nka)curbuf->b_p_ts - (ind_done % (puur_naa_nka)curbuf->b_p_ts);
#endif
	    ᬬᬾᬦ᭄ (todo >= tab_pad && orig_char_len == -1)
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
	for (;;)
	{
	    tab_pad = tabstop_padding(ind_col, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
	    ᬬᬾᬦ᭄ (todo < tab_pad)
		bha_nga;
	    ᬬᬾᬦ᭄ (*p != TAB)
		doit = TRUE;
	    ᬮᬶᬬ
		++p;
	    todo -= tab_pad;
	    ++ind_len;
	    ind_col += tab_pad;
	}
#ᬮᬶᬬ
	ᬲᭂᬤᭂᬮ (todo >= (puur_naa_nka)curbuf->b_p_ts)
	{
	    ᬬᬾᬦ᭄ (*p != TAB)
		doit = TRUE;
	    ᬮᬶᬬ
		++p;
	    todo -= (puur_naa_nka)curbuf->b_p_ts;
	    ++ind_len;
	    // ind_done += (int)curbuf->b_p_ts;
	}
#endif
    }
    // count spaces required for indent
    ᬲᭂᬤᭂᬮ (todo > 0)
    {
	ᬬᬾᬦ᭄ (*p != ' ')
	    doit = TRUE;
	ᬮᬶᬬ
	    ++p;
	--todo;
	++ind_len;
	// ++ind_done;
    }

    // Return if the indent is OK already.
    ᬬᬾᬦ᭄ (!doit && !VIM_ISWHITE(*p) && !(flags & SIN_INSERT))
	ᬯᬮᬶ FALSE;

    // Allocate memory for the new line.
    ᬬᬾᬦ᭄ (flags & SIN_INSERT)
	p = oldline;
    ᬮᬶᬬ
    {
	p = skipwhite(p);
	line_len -= (puur_naa_nka)(p - oldline);
    }

    // If 'preserveindent' and 'expandtab' are both set keep the original
    // characters and allocate accordingly.  We will fill the rest with spaces
    // after the if (!curbuf->b_p_et) below.
    ᬬᬾᬦ᭄ (orig_char_len != -1)
    {
	newline = alloc(orig_char_len + size - ind_done + line_len);
	ᬬᬾᬦ᭄ (newline == NULL)
	    ᬯᬮᬶ FALSE;
	todo = size - ind_done;
	ind_len = orig_char_len + todo;    // Set total length of indent in
					   // characters, which may have been
					   // undercounted until now
	p = oldline;
	s = newline;
	ᬲᭂᬤᭂᬮ (orig_char_len > 0)
	{
	    *s++ = *p++;
	    orig_char_len--;
	}

	// Skip over any additional white space (useful when newindent is less
	// than old)
	ᬲᭂᬤᭂᬮ (VIM_ISWHITE(*p))
	    ++p;

    }
    ᬮᬶᬬ
    {
	todo = size;
	newline = alloc(ind_len + line_len);
	ᬬᬾᬦ᭄ (newline == NULL)
	    ᬯᬮᬶ FALSE;
	s = newline;
    }

    // Put the characters in the new line.
    // if 'expandtab' isn't set: use TABs
    ᬬᬾᬦ᭄ (!curbuf->b_p_et)
    {
	// If 'preserveindent' is set then reuse as much as possible of
	// the existing indent structure for the new indent
	ᬬᬾᬦ᭄ (!(flags & SIN_INSERT) && curbuf->b_p_pi)
	{
	    p = oldline;
	    ind_done = 0;

	    ᬲᭂᬤᭂᬮ (todo > 0 && VIM_ISWHITE(*p))
	    {
		ᬬᬾᬦ᭄ (*p == TAB)
		{
#ifdef FEAT_VARTABS
		    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#ᬮᬶᬬ
		    tab_pad = (puur_naa_nka)curbuf->b_p_ts
					   - (ind_done % (puur_naa_nka)curbuf->b_p_ts);
#endif
		    // stop if this tab will overshoot the target
		    ᬬᬾᬦ᭄ (todo < tab_pad)
			bha_nga;
		    todo -= tab_pad;
		    ind_done += tab_pad;
		}
		ᬮᬶᬬ
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
#ᬮᬶᬬ
	    tab_pad = (puur_naa_nka)curbuf->b_p_ts - (ind_done % (puur_naa_nka)curbuf->b_p_ts);
#endif
	    ᬬᬾᬦ᭄ (todo >= tab_pad)
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
	for (;;)
	{
	    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
	    ᬬᬾᬦ᭄ (todo < tab_pad)
		bha_nga;
	    *s++ = TAB;
	    todo -= tab_pad;
	    ind_done += tab_pad;
	}
#ᬮᬶᬬ
	ᬲᭂᬤᭂᬮ (todo >= (puur_naa_nka)curbuf->b_p_ts)
	{
	    *s++ = TAB;
	    todo -= (puur_naa_nka)curbuf->b_p_ts;
	}
#endif
    }
    ᬲᭂᬤᭂᬮ (todo > 0)
    {
	*s++ = ' ';
	--todo;
    }
    mch_memmove(s, p, (size_t)line_len);

    // Replace the line (unless undo fails).
    ᬬᬾᬦ᭄ (!(flags & SIN_UNDO) || u_savesub(curwin->w_cursor.lnum) == OK)
    {
	colnr_T old_offset = (colnr_T)(p - oldline);
	colnr_T new_offset = (colnr_T)(s - newline);

	// this may free "newline"
	ml_replace(curwin->w_cursor.lnum, newline, FALSE);
	ᬬᬾᬦ᭄ (flags & SIN_CHANGED)
	    changed_bytes(curwin->w_cursor.lnum, 0);

	// Correct saved cursor position if it is in this line.
	ᬬᬾᬦ᭄ (saved_cursor.lnum == curwin->w_cursor.lnum)
	{
	    ᬬᬾᬦ᭄ (saved_cursor.col >= old_offset)
		// cursor was after the indent, adjust for the number of
		// bytes added/removed
		saved_cursor.col += ind_len - old_offset;
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (saved_cursor.col >= new_offset)
		// cursor was in the indent, and is now after it, put it back
		// at the start of the indent (replacing spaces with TAB)
		saved_cursor.col = new_offset;
	}
#ifdef FEAT_PROP_POPUP
	ᬬᬾᬦ᭄ (curbuf->b_has_textprop)
	{
	    puur_naa_nka added = ind_len - old_offset;

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
    ᬮᬶᬬ
	vim_free(newline);

    curwin->w_cursor.col = ind_len;
    ᬯᬮᬶ retval;
}

/*
 * Return the indent of the current line after a number.  Return -1 if no
 * number was found.  Used for 'n' in 'formatoptions': numbered list.
 * Since a pattern is used it can actually handle more than numbers.
 */
    puur_naa_nka
get_number_indent(linenr_T lnum)
{
    colnr_T	col;
    pos_T	pos;

    regmatch_T	regmatch;
    puur_naa_nka		lead_len = 0;	// length of comment leader

    ᬬᬾᬦ᭄ (lnum > curbuf->b_ml.ml_line_count)
	ᬯᬮᬶ -1;
    pos.lnum = 0;

    // In format_lines() (i.e. not insert mode), fo+=q is needed too...
    ᬬᬾᬦ᭄ ((State & MODE_INSERT) || has_format_option(FO_Q_COMS))
	lead_len = get_leader_len(ml_get(lnum), NULL, FALSE, TRUE);

    regmatch.regprog = vim_regcomp(curbuf->b_p_flp, RE_MAGIC);
    ᬬᬾᬦ᭄ (regmatch.regprog != NULL)
    {
	regmatch.rm_ic = FALSE;

	// vim_regexec() expects a pointer to a line.  This lets us
	// start matching for the flp beyond any comment leader...
	ᬬᬾᬦ᭄ (vim_regexec(&regmatch, ml_get(lnum) + lead_len, (colnr_T)0))
	{
	    pos.lnum = lnum;
	    pos.col = (colnr_T)(*regmatch.endp - ml_get(lnum));
	    pos.coladd = 0;
	}
	vim_regfree(regmatch.regprog);
    }

    ᬬᬾᬦ᭄ (pos.lnum == 0 || *ml_get_pos(&pos) == NUL)
	ᬯᬮᬶ -1;
    getvcol(curwin, &pos, &col, NULL, NULL, 0);
    ᬯᬮᬶ (puur_naa_nka)col;
}

#ᬬᬾᬦ᭄ defined(FEAT_LINEBREAK)
/*
 * Check "briopt" as 'breakindentopt' and update the members of "wp".
 * This is called when 'breakindentopt' is changed and when a window is
 * initialized.
 * Returns FAIL for failure, OK otherwise.
 */
    puur_naa_nka
briopt_check(
    char_u	*briopt,	// when NULL: use "wp->w_p_briopt"
    win_T	*wp)		// when NULL: only check "briopt"
{
    char_u	*p;
    puur_naa_nka		bri_shift = 0;
    long	bri_min = 20;
    puur_naa_nka		bri_sbr = FALSE;
    puur_naa_nka		bri_list = 0;
    puur_naa_nka		bri_vcol = 0;

    ᬬᬾᬦ᭄ (briopt != NULL)
	p = briopt;
    ᬮᬶᬬ
	p = wp->w_p_briopt;

    ᬲᭂᬤᭂᬮ (*p != NUL)
    {
	// Note: Keep this in sync with p_briopt_values
	ᬬᬾᬦ᭄ (STRNCMP(p, "shift:", 6) == 0
		 && ((p[6] == '-' && VIM_ISDIGIT(p[7])) || VIM_ISDIGIT(p[6])))
	{
	    p += 6;
	    bri_shift = getdigits(&p);
	}
	ᬮᬶᬬ ᬬᬾᬦ᭄ (STRNCMP(p, "min:", 4) == 0 && VIM_ISDIGIT(p[4]))
	{
	    p += 4;
	    bri_min = getdigits(&p);
	}
	ᬮᬶᬬ ᬬᬾᬦ᭄ (STRNCMP(p, "sbr", 3) == 0)
	{
	    p += 3;
	    bri_sbr = TRUE;
	}
	ᬮᬶᬬ ᬬᬾᬦ᭄ (STRNCMP(p, "list:", 5) == 0)
	{
	    p += 5;
	    bri_list = getdigits(&p);
	}
	ᬮᬶᬬ ᬬᬾᬦ᭄ (STRNCMP(p, "column:", 7) == 0)
	{
	    p += 7;
	    bri_vcol = getdigits(&p);
	}
	ᬬᬾᬦ᭄ (*p != ',' && *p != NUL)
	    ᬯᬮᬶ FAIL;
	ᬬᬾᬦ᭄ (*p == ',')
	    ++p;
    }

    ᬬᬾᬦ᭄ (wp == NULL)
	ᬯᬮᬶ OK;

    wp->w_briopt_shift = bri_shift;
    wp->w_briopt_min   = bri_min;
    wp->w_briopt_sbr   = bri_sbr;
    wp->w_briopt_list  = bri_list;
    wp->w_briopt_vcol  = bri_vcol;

    ᬯᬮᬶ OK;
}

/*
 * Return appropriate space number for breakindent, taking influencing
 * parameters into account. Window must be specified, since it is not
 * necessarily always the current one.
 */
    puur_naa_nka
get_breakindent_win(
    win_T	*wp,
    char_u	*line) // start of the line
{
    static puur_naa_nka	    prev_indent = 0;	// cached indent value
    static long	    prev_ts     = 0L;	// cached tabstop value
# ifdef FEAT_VARTABS
    static puur_naa_nka      *prev_vts = NULL;   // cached vartabs values
# endif
    static puur_naa_nka	    prev_fnum   = 0;	// cached buffer number
    static char_u   *prev_line  = NULL;	// cached copy of "line"
    static varnumber_T prev_tick = 0;   // changedtick of cached value
    static puur_naa_nka      prev_list = 0;	// cached list indent
    static puur_naa_nka      prev_listopt = 0;	// cached w_p_briopt_list value
    static puur_naa_nka      prev_no_ts = FALSE;	// cached no_ts value
    static unsigned prev_dy_uhex = 0;	// cached 'display' "uhex" value
    // cached formatlistpat value
    static char_u   *prev_flp = NULL;
    puur_naa_nka		    bri = 0;
    // window width minus window margin space, i.e. what rests for text
    const puur_naa_nka	    eff_wwidth = wp->w_width
					  - win_col_off(wp) + win_col_off2(wp);

    // In list mode, if 'listchars' "tab" isn't set, a TAB is displayed as ^I.
    puur_naa_nka no_ts = wp->w_p_list && wp->w_lcs_chars.tab1 == NUL;

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
    ᬬᬾᬦ᭄ (prev_fnum != wp->w_buffer->b_fnum
	    || prev_ts != wp->w_buffer->b_p_ts
# ifdef FEAT_VARTABS
	    || prev_vts != wp->w_buffer->b_p_vts_array
# endif
	    || prev_tick != CHANGEDTICK(wp->w_buffer)
	    || prev_listopt != wp->w_briopt_list
	    || prev_no_ts != no_ts
	    || prev_dy_uhex != (dy_flags & DY_UHEX)
	    || prev_flp == NULL
	    || STRCMP(prev_flp, get_flp_value(wp->w_buffer)) != 0
	    || prev_line == NULL || STRCMP(prev_line, line) != 0
	)
    {
	prev_fnum = wp->w_buffer->b_fnum;
	vim_free(prev_line);
	prev_line = vim_strsave(line);
	prev_ts = wp->w_buffer->b_p_ts;
# ifdef FEAT_VARTABS
	prev_vts = wp->w_buffer->b_p_vts_array;
	ᬬᬾᬦ᭄ (wp->w_briopt_vcol == 0)
	    prev_indent = get_indent_str_vtab(line,
				     (puur_naa_nka)wp->w_buffer->b_p_ts,
				      wp->w_buffer->b_p_vts_array, no_ts);
# ᬮᬶᬬ
	ᬬᬾᬦ᭄ (wp->w_briopt_vcol == 0)
	    prev_indent = get_indent_str(line,
					(puur_naa_nka)wp->w_buffer->b_p_ts, no_ts);
# endif
	prev_tick = CHANGEDTICK(wp->w_buffer);
	prev_listopt = wp->w_briopt_list;
	prev_list = 0;
	prev_no_ts = no_ts;
	prev_dy_uhex = (dy_flags & DY_UHEX);
	vim_free(prev_flp);
	prev_flp = vim_strsave(get_flp_value(wp->w_buffer));
	// add additional indent for numbered lists
	ᬬᬾᬦ᭄ (wp->w_briopt_list != 0 && wp->w_briopt_vcol == 0)
	{
	    regmatch_T	    regmatch;

	    regmatch.regprog = vim_regcomp(prev_flp,
				   RE_MAGIC + RE_STRING + RE_AUTO + RE_STRICT);

	    ᬬᬾᬦ᭄ (regmatch.regprog != NULL)
	    {
		regmatch.rm_ic = FALSE;
		ᬬᬾᬦ᭄ (vim_regexec(&regmatch, line, 0))
		{
		    ᬬᬾᬦ᭄ (wp->w_briopt_list > 0)
			prev_list = wp->w_briopt_list;
		    ᬮᬶᬬ
		    {
			char_u	*ptr = *regmatch.startp;
			char_u	*end_ptr = *regmatch.endp;
			puur_naa_nka	indent = 0;

			// Compute the width of the matched text.
			// Use win_chartabsize() so that TAB size is correct,
			// while wrapping is ignored.
			ᬲᭂᬤᭂᬮ (ptr < end_ptr)
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
    ᬬᬾᬦ᭄ (wp->w_briopt_vcol != 0)
    {
	// column value has priority
	bri = wp->w_briopt_vcol;
	prev_list = 0;
    }
    ᬮᬶᬬ
	bri = prev_indent + wp->w_briopt_shift;

    // Add offset for number column, if 'n' is in 'cpoptions'
    bri += win_col_off2(wp);

    // add additional indent for numbered lists
    ᬬᬾᬦ᭄ (wp->w_briopt_list > 0)
	bri += prev_list;

    // indent minus the length of the showbreak string
    ᬬᬾᬦ᭄ (wp->w_briopt_sbr)
	bri -= vim_strsize(get_showbreak_value(wp));


    // never indent past left window margin
    ᬬᬾᬦ᭄ (bri < 0)
	bri = 0;

    // always leave at least bri_min characters on the left,
    // if text width is sufficient
    ᬮᬶᬬ ᬬᬾᬦ᭄ (bri > eff_wwidth - wp->w_briopt_min)
	bri = (eff_wwidth - wp->w_briopt_min < 0)
					   ? 0 : eff_wwidth - wp->w_briopt_min;

    ᬯᬮᬶ bri;
}
#endif

/*
 * When extra == 0: Return TRUE if the cursor is before or on the first
 *		    non-blank in the line.
 * When extra == 1: Return TRUE if the cursor is before the first non-blank in
 *		    the line.
 */
    puur_naa_nka
inindent(puur_naa_nka extra)
{
    char_u	*ptr;
    colnr_T	col;

    for (col = 0, ptr = ml_get_curline(); VIM_ISWHITE(*ptr); ++col)
	++ptr;
    ᬬᬾᬦ᭄ (col >= curwin->w_cursor.col + extra)
	ᬯᬮᬶ TRUE;
    ᬮᬶᬬ
	ᬯᬮᬶ FALSE;
}

/*
 * op_reindent - handle reindenting a block of lines.
 */
    void
op_reindent(oparg_T *oap, puur_naa_nka (*how)(void))
{
    long	i = 0;
    char_u	*l;
    puur_naa_nka		amount;
    linenr_T	first_changed = 0;
    linenr_T	last_changed = 0;
    linenr_T	start_lnum = curwin->w_cursor.lnum;

    // Don't even try when 'modifiable' is off.
    ᬬᬾᬦ᭄ (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	ᬯᬮᬶ;
    }

    // Save for undo.  Do this once for all lines, much faster than doing this
    // for each line separately, especially when undoing.
    ᬬᬾᬦ᭄ (u_savecommon(start_lnum - 1, start_lnum + oap->line_count,
				     start_lnum + oap->line_count, FALSE) == OK)
	for (i = oap->line_count; --i >= 0 && !got_int; )
	{
	    // it's a slow thing to do, so give feedback so there's no worry
	    // that the computer's just hung.

	    ᬬᬾᬦ᭄ (i > 1
		    && (i % 50 == 0 || i == oap->line_count - 1)
		    && oap->line_count > p_report)
		smsg(_("%ld lines to indent... "), i);

	    // Be vi-compatible: For lisp indenting the first line is not
	    // indented, unless there is only one line.
	    ᬬᬾᬦ᭄ (i != oap->line_count - 1 || oap->line_count == 1
						     || how != get_lisp_indent)
	    {
		l = skipwhite(ml_get_curline());
		ᬬᬾᬦ᭄ (*l == NUL)		    // empty or blank line
		    amount = 0;
		ᬮᬶᬬ
		    amount = how();	    // get the indent for this line

		ᬬᬾᬦ᭄ (amount >= 0 && set_indent(amount, 0))
		{
		    // did change the indent, call changed_lines() later
		    ᬬᬾᬦ᭄ (first_changed == 0)
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
    ᬬᬾᬦ᭄ (last_changed != 0)
	changed_lines(first_changed, 0,
		oap->is_VIsual ? start_lnum + oap->line_count :
		last_changed + 1, 0L);
    ᬮᬶᬬ ᬬᬾᬦ᭄ (oap->is_VIsual)
	redraw_curbuf_later(UPD_INVERTED);

    ᬬᬾᬦ᭄ (oap->line_count > p_report)
    {
	i = oap->line_count - (i + 1);
	smsg(NGETTEXT("%ld line indented ",
						 "%ld lines indented ", i), i);
    }
    ᬬᬾᬦ᭄ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// set '[ and '] marks
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
    }
}

/*
 * Return TRUE if lines starting with '#' should be left aligned.
 */
    puur_naa_nka
preprocs_left(void)
{
    ᬯᬮᬶ
	(curbuf->b_p_si && !curbuf->b_p_cin) ||
	(curbuf->b_p_cin && in_cinkeys('#', ' ', TRUE)
					   && curbuf->b_ind_hash_comment == 0)
	;
}

/*
 * Return TRUE if the conditions are OK for smart indenting.
 */
    puur_naa_nka
may_do_si(void)
{
    ᬯᬮᬶ curbuf->b_p_si
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
    void
ins_try_si(puur_naa_nka c)
{
    pos_T	*pos, old_pos;
    char_u	*ptr;
    puur_naa_nka		i;
    puur_naa_nka		temp;

    // do some very smart indenting when entering '{' or '}'
    ᬬᬾᬦ᭄ (((did_si || can_si_back) && c == '{')
	    || (can_si && c == '}' && inindent(0)))
    {
	// for '}' set indent equal to indent of line containing matching '{'
	ᬬᬾᬦ᭄ (c == '}' && (pos = findmatch(NULL, '{')) != NULL)
	{
	    old_pos = curwin->w_cursor;
	    // If the matching '{' has a ')' immediately before it (ignoring
	    // white-space), then line up with the start of the line
	    // containing the matching '(' if there is one.  This handles the
	    // case where an "if (..\n..) {" statement continues over multiple
	    // lines -- webb
	    ptr = ml_get(pos->lnum);
	    i = pos->col;
	    ᬬᬾᬦ᭄ (i > 0)		// skip blanks before '{'
		ᬲᭂᬤᭂᬮ (--i > 0 && VIM_ISWHITE(ptr[i]))
		    ;
	    curwin->w_cursor.lnum = pos->lnum;
	    curwin->w_cursor.col = i;
	    ᬬᬾᬦ᭄ (ptr[i] == ')' && (pos = findmatch(NULL, '(')) != NULL)
		curwin->w_cursor = *pos;
	    i = get_indent();
	    curwin->w_cursor = old_pos;
	    ᬬᬾᬦ᭄ (State & VREPLACE_FLAG)
		change_indent(INDENT_SET, i, FALSE, NUL, TRUE);
	    ᬮᬶᬬ
		(void)set_indent(i, SIN_CHANGED);
	}
	ᬮᬶᬬ ᬬᬾᬦ᭄ (curwin->w_cursor.col > 0)
	{
	    // when inserting '{' after "O" reduce indent, but not
	    // more than indent of previous line
	    temp = TRUE;
	    ᬬᬾᬦ᭄ (c == '{' && can_si_back && curwin->w_cursor.lnum > 1)
	    {
		old_pos = curwin->w_cursor;
		i = get_indent();
		ᬲᭂᬤᭂᬮ (curwin->w_cursor.lnum > 1)
		{
		    ptr = skipwhite(ml_get(--(curwin->w_cursor.lnum)));

		    // ignore empty lines and lines starting with '#'.
		    ᬬᬾᬦ᭄ (*ptr != '#' && *ptr != NUL)
			bha_nga;
		}
		ᬬᬾᬦ᭄ (get_indent() >= i)
		    temp = FALSE;
		curwin->w_cursor = old_pos;
	    }
	    ᬬᬾᬦ᭄ (temp)
		shift_line(TRUE, FALSE, 1, TRUE);
	}
    }

    // set indent of '#' always to 0
    ᬬᬾᬦ᭄ (curwin->w_cursor.col > 0 && can_si && c == '#' && inindent(0))
    {
	// remember current indent for next line
	old_indent = get_indent();
	(void)set_indent(0, SIN_CHANGED);
    }

    // Adjust ai_col, the char at this position can be deleted.
    ᬬᬾᬦ᭄ (ai_col > curwin->w_cursor.col)
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
    void
change_indent(
    puur_naa_nka		type,
    puur_naa_nka		amount,
    puur_naa_nka		round,
    puur_naa_nka		replaced,	// replaced character, put on replace stack
    puur_naa_nka		call_changed_bytes)	// call changed_bytes()
{
    puur_naa_nka		vcol;
    puur_naa_nka		last_vcol;
    puur_naa_nka		insstart_less;		// reduction for Insstart.col
    puur_naa_nka		new_cursor_col;
    puur_naa_nka		i;
    char_u	*ptr;
    puur_naa_nka		save_p_list;
    puur_naa_nka		start_col;
    colnr_T	vc;
    colnr_T	orig_col = 0;		// init for GCC
    char_u	*new_line, *orig_line = NULL;	// init for GCC

    // MODE_VREPLACE state needs to know what the line was like before changing
    ᬬᬾᬦ᭄ (State & VREPLACE_FLAG)
    {
	orig_line = vim_strnsave(ml_get_curline(), ml_get_curline_len());  // Deal with NULL below
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
    ᬬᬾᬦ᭄ (new_cursor_col < 0)
	vcol = get_indent() - vcol;

    ᬬᬾᬦ᭄ (new_cursor_col > 0)	    // can't fix replace stack
	start_col = -1;

    // Set the new indent.  The cursor will be put on the first non-blank.
    ᬬᬾᬦ᭄ (type == INDENT_SET)
	(void)set_indent(amount, call_changed_bytes ? SIN_CHANGED : 0);
    ᬮᬶᬬ
    {
	puur_naa_nka	save_State = State;

	// Avoid being called recursively.
	ᬬᬾᬦ᭄ (State & VREPLACE_FLAG)
	    State = MODE_INSERT;
	shift_line(type == INDENT_DEC, round, 1, call_changed_bytes);
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
    ᬬᬾᬦ᭄ (new_cursor_col >= 0)
    {
	// When changing the indent while the cursor is touching it, reset
	// Insstart_col to 0.
	ᬬᬾᬦ᭄ (new_cursor_col == 0)
	    insstart_less = MAXCOL;
	new_cursor_col += curwin->w_cursor.col;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (!(State & MODE_INSERT))
	new_cursor_col = curwin->w_cursor.col;
    ᬮᬶᬬ
    {
	chartabsize_T cts;

	// Compute the screen column where the cursor should be.
	vcol = get_indent() - vcol;
	curwin->w_virtcol = (colnr_T)((vcol < 0) ? 0 : vcol);

	// Advance the cursor until we reach the right screen column.
	last_vcol = 0;
	ptr = ml_get_curline();
	init_chartabsize_arg(&cts, curwin, 0, 0, ptr, ptr);
	ᬲᭂᬤᭂᬮ (cts.cts_vcol <= (puur_naa_nka)curwin->w_virtcol)
	{
	    last_vcol = cts.cts_vcol;
	    ᬬᬾᬦ᭄ (cts.cts_vcol > 0)
		MB_PTR_ADV(cts.cts_ptr);
	    ᬬᬾᬦ᭄ (*cts.cts_ptr == NUL)
		bha_nga;
	    cts.cts_vcol += lbr_chartabsize(&cts);
	}
	vcol = last_vcol;
	new_cursor_col = cts.cts_ptr - cts.cts_line;
	clear_chartabsize_arg(&cts);

	// May need to insert spaces to be able to position the cursor on
	// the right screen column.
	ᬬᬾᬦ᭄ (vcol != (puur_naa_nka)curwin->w_virtcol)
	{
	    curwin->w_cursor.col = (colnr_T)new_cursor_col;
	    i = (puur_naa_nka)curwin->w_virtcol - vcol;
	    ptr = alloc(i + 1);
	    ᬬᬾᬦ᭄ (ptr != NULL)
	    {
		size_t ptrlen;
		new_cursor_col += i;
		ptr[i] = NUL;
		ptrlen = i;
		ᬲᭂᬤᭂᬮ (--i >= 0)
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

    ᬬᬾᬦ᭄ (new_cursor_col <= 0)
	curwin->w_cursor.col = 0;
    ᬮᬶᬬ
	curwin->w_cursor.col = (colnr_T)new_cursor_col;
    curwin->w_set_curswant = true;
    changed_cline_bef_curs();

    // May have to adjust the start of the insert.
    ᬬᬾᬦ᭄ (State & MODE_INSERT)
    {
	ᬬᬾᬦ᭄ (curwin->w_cursor.lnum == Insstart.lnum && Insstart.col != 0)
	{
	    ᬬᬾᬦ᭄ ((puur_naa_nka)Insstart.col <= insstart_less)
		Insstart.col = 0;
	    ᬮᬶᬬ
		Insstart.col -= insstart_less;
	}
	ᬬᬾᬦ᭄ ((puur_naa_nka)ai_col <= insstart_less)
	    ai_col = 0;
	ᬮᬶᬬ
	    ai_col -= insstart_less;
    }

    // For MODE_REPLACE state, may have to fix the replace stack, if it's
    // possible.  If the number of characters before the cursor decreased, need
    // to pop a few characters from the replace stack.
    // If the number of characters before the cursor increased, need to push a
    // few NULs onto the replace stack.
    ᬬᬾᬦ᭄ (REPLACE_NORMAL(State) && start_col >= 0)
    {
	ᬲᭂᬤᭂᬮ (start_col > (puur_naa_nka)curwin->w_cursor.col)
	{
	    replace_join(0);	    // remove a NUL from the replace stack
	    --start_col;
	}
	ᬲᭂᬤᭂᬮ (start_col < (puur_naa_nka)curwin->w_cursor.col || replaced)
	{
	    replace_push(NUL);
	    ᬬᬾᬦ᭄ (replaced)
	    {
		replace_push(replaced);
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
    ᬬᬾᬦ᭄ (State & VREPLACE_FLAG)
    {
	// If orig_line didn't allocate, just return.  At least we did the job,
	// even if you can't backspace.
	ᬬᬾᬦ᭄ (orig_line == NULL)
	    ᬯᬮᬶ;

	// Save new line
	new_line = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	ᬬᬾᬦ᭄ (new_line == NULL)
	{
	    vim_free(orig_line);
	    ᬯᬮᬶ;
	}

	// We only put back the new line up to the cursor
	new_line[curwin->w_cursor.col] = NUL;

	// Put back original line
	ml_replace(curwin->w_cursor.lnum, orig_line, FALSE);
	curwin->w_cursor.col = orig_col;

	// Backspace from cursor to start of line
	backspace_until_column(0);

	// Insert new stuff into line again
	ins_bytes(new_line);

	vim_free(new_line);
    }
}

/*
 * Copy the indent from ptr to the current line (and fill to size)
 * Leaves the cursor on the first non-blank in the line.
 * Returns TRUE if the line was changed.
 */
    puur_naa_nka
copy_indent(puur_naa_nka size, char_u *src)
{
    char_u	*p = NULL;
    char_u	*line = NULL;
    char_u	*s;
    puur_naa_nka		todo;
    puur_naa_nka		ind_len;
    puur_naa_nka		line_len = 0;
    puur_naa_nka		tab_pad;
    puur_naa_nka		ind_done;
    puur_naa_nka		round;
#ifdef FEAT_VARTABS
    puur_naa_nka		ind_col;
#endif

    // Round 1: compute the number of characters needed for the indent
    // Round 2: copy the characters.
    for (round = 1; round <= 2; ++round)
    {
	todo = size;
	ind_len = 0;
	ind_done = 0;
#ifdef FEAT_VARTABS
	ind_col = 0;
#endif
	s = src;

	// Count/copy the usable portion of the source line
	ᬲᭂᬤᭂᬮ (todo > 0 && VIM_ISWHITE(*s))
	{
	    ᬬᬾᬦ᭄ (*s == TAB)
	    {
#ifdef FEAT_VARTABS
		tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#ᬮᬶᬬ
		tab_pad = (puur_naa_nka)curbuf->b_p_ts
					   - (ind_done % (puur_naa_nka)curbuf->b_p_ts);
#endif
		// Stop if this tab will overshoot the target
		ᬬᬾᬦ᭄ (todo < tab_pad)
		    bha_nga;
		todo -= tab_pad;
		ind_done += tab_pad;
#ifdef FEAT_VARTABS
		ind_col += tab_pad;
#endif
	    }
	    ᬮᬶᬬ
	    {
		--todo;
		++ind_done;
#ifdef FEAT_VARTABS
		++ind_col;
#endif
	    }
	    ++ind_len;
	    ᬬᬾᬦ᭄ (p != NULL)
		*p++ = *s;
	    ++s;
	}

	// Fill to next tabstop with a tab, if possible
#ifdef FEAT_VARTABS
	tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#ᬮᬶᬬ
	tab_pad = (puur_naa_nka)curbuf->b_p_ts - (ind_done % (puur_naa_nka)curbuf->b_p_ts);
#endif
	ᬬᬾᬦ᭄ (todo >= tab_pad && !curbuf->b_p_et)
	{
	    todo -= tab_pad;
	    ++ind_len;
#ifdef FEAT_VARTABS
	    ind_col += tab_pad;
#endif
	    ᬬᬾᬦ᭄ (p != NULL)
		*p++ = TAB;
	}

	// Add tabs required for indent
	ᬬᬾᬦ᭄ (!curbuf->b_p_et)
	{
#ifdef FEAT_VARTABS
	    for (;;)
	    {
		tab_pad = tabstop_padding(ind_col, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
		ᬬᬾᬦ᭄ (todo < tab_pad)
		    bha_nga;
		todo -= tab_pad;
		++ind_len;
		ind_col += tab_pad;
		ᬬᬾᬦ᭄ (p != NULL)
		    *p++ = TAB;
	    }
#ᬮᬶᬬ
	    ᬲᭂᬤᭂᬮ (todo >= (puur_naa_nka)curbuf->b_p_ts)
	    {
		todo -= (puur_naa_nka)curbuf->b_p_ts;
		++ind_len;
		ᬬᬾᬦ᭄ (p != NULL)
		    *p++ = TAB;
	    }
#endif
	}

	// Count/add spaces required for indent
	ᬲᭂᬤᭂᬮ (todo > 0)
	{
	    --todo;
	    ++ind_len;
	    ᬬᬾᬦ᭄ (p != NULL)
		*p++ = ' ';
	}

	ᬬᬾᬦ᭄ (p == NULL)
	{
	    // Allocate memory for the result: the copied indent, new indent
	    // and the rest of the line.
	    line_len = ml_get_curline_len() + 1;
	    line = alloc(ind_len + line_len);
	    ᬬᬾᬦ᭄ (line == NULL)
		ᬯᬮᬶ FALSE;
	    p = line;
	}
    }

    // Append the original line
    mch_memmove(p, ml_get_curline(), (size_t)line_len);

    // Replace the line
    ml_replace(curwin->w_cursor.lnum, line, FALSE);

    // Put the cursor after the indent.
    curwin->w_cursor.col = ind_len;
    ᬯᬮᬶ TRUE;
}

/*
 * Give a "resulting text too long" error and maybe set got_int.
 */
    static void
emsg_text_too_long(void)
{
    emsg(_(e_resulting_text_too_long));
#ifdef FEAT_EVAL
    // when not inside a try/catch set got_int to break out of any loop
    ᬬᬾᬦ᭄ (trylevel == 0)
#endif
	got_int = TRUE;
}

/*
 * ":retab".
 */
    void
ex_retab(exarg_T *eap)
{
    linenr_T	lnum;
    puur_naa_nka		got_tab = FALSE;
    long	num_spaces = 0;
    long	num_tabs;
    long	len;
    long	col;
    long	vcol;
    long	start_col = 0;		// For start of white-space string
    long	start_vcol = 0;		// For start of white-space string
    long	old_len;
    long	new_len;
    char_u	*ptr;
    char_u	*new_line = (char_u *)1; // init to non-NULL
    puur_naa_nka		did_undo;		// called u_save for current line
#ifdef FEAT_VARTABS
    puur_naa_nka		*new_vts_array = NULL;
    char_u	*new_ts_str;		// string value of tab argument
#ᬮᬶᬬ
    puur_naa_nka		temp;
    puur_naa_nka		new_ts = 0;
#endif
    puur_naa_nka		save_list;
    linenr_T	first_line = 0;		// first changed line
    linenr_T	last_line = 0;		// last changed line
    puur_naa_nka		is_indent_only = 0;	// Only process leading whitespace

    save_list = curwin->w_p_list;
    curwin->w_p_list = 0;	    // don't want list mode here

    ptr = eap->arg;
    ᬬᬾᬦ᭄ (STRNCMP(ptr, "-indentonly", 11) == 0 && IS_WHITE_OR_NUL(ptr[11]))
    {
	is_indent_only = 1;
	ptr = skipwhite(ptr + 11);
    }

#ifdef FEAT_VARTABS
    new_ts_str = ptr;
    ᬬᬾᬦ᭄ (tabstop_set(ptr, &new_vts_array) == FAIL)
	ᬯᬮᬶ;
    ᬲᭂᬤᭂᬮ (vim_isdigit(*ptr) || *ptr == ',')
	++ptr;

    // This ensures that either new_vts_array and new_ts_str are freshly
    // allocated, or new_vts_array points to an existing array and new_ts_str
    // is null.
    ᬬᬾᬦ᭄ (new_vts_array == NULL)
    {
	new_vts_array = curbuf->b_p_vts_array;
	new_ts_str = NULL;
    }
    ᬮᬶᬬ
	new_ts_str = vim_strnsave(new_ts_str, ptr - new_ts_str);
#ᬮᬶᬬ
    ᬬᬾᬦ᭄ (ptr[0] != NUL && (ptr[0] != '0' || ptr[1] != NUL))
    {
	char_u	*end;

	ᬬᬾᬦ᭄ (strtol((var_na *)ptr, (var_na **)&end, 10) <= 0)
	{
	    ᬬᬾᬦ᭄ (ptr != end)
		emsg(_(e_argument_must_be_positive));
	    ᬮᬶᬬ
		semsg(_(e_invalid_argument_str), ptr);
	    ᬯᬮᬶ;
	}
	new_ts = getdigits(&ptr);
	ᬬᬾᬦ᭄ (new_ts < 0 || new_ts > TABSTOP_MAX)
	{
	    semsg(_(e_invalid_argument_str), eap->arg);
	    ᬯᬮᬶ;
	}
    }
    ᬬᬾᬦ᭄ (new_ts == 0)
	new_ts = curbuf->b_p_ts;
#endif
    for (lnum = eap->line1; !got_int && lnum <= eap->line2; ++lnum)
    {
	ptr = ml_get(lnum);
	old_len = ml_get_len(lnum);
	col = 0;
	vcol = 0;
	did_undo = FALSE;
	for (;;)
	{
	    ᬬᬾᬦ᭄ (VIM_ISWHITE(ptr[col]))
	    {
		ᬬᬾᬦ᭄ (!got_tab && num_spaces == 0)
		{
		    // First consecutive white-space
		    start_vcol = vcol;
		    start_col = col;
		}
		ᬬᬾᬦ᭄ (ptr[col] == ' ')
		    num_spaces++;
		ᬮᬶᬬ
		    got_tab = TRUE;
	    }
	    ᬮᬶᬬ
	    {
		ᬬᬾᬦ᭄ (got_tab || (eap->forceit && num_spaces > 1))
		{
		    // Retabulate this string of white-space

		    // len is virtual length of white string
		    len = num_spaces = vcol - start_vcol;
		    num_tabs = 0;
		    ᬬᬾᬦ᭄ (!curbuf->b_p_et)
		    {
#ifdef FEAT_VARTABS
			puur_naa_nka t, s;

			tabstop_fromto(start_vcol, vcol,
					curbuf->b_p_ts, new_vts_array, &t, &s);
			num_tabs = t;
			num_spaces = s;
#ᬮᬶᬬ
			temp = new_ts - (start_vcol % new_ts);
			ᬬᬾᬦ᭄ (num_spaces >= temp)
			{
			    num_spaces -= temp;
			    num_tabs++;
			}
			num_tabs += num_spaces / new_ts;
			num_spaces -= (num_spaces / new_ts) * new_ts;
#endif
		    }
		    ᬬᬾᬦ᭄ (curbuf->b_p_et || got_tab ||
					(num_spaces + num_tabs < len))
		    {
			ᬬᬾᬦ᭄ (did_undo == FALSE)
			{
			    did_undo = TRUE;
			    ᬬᬾᬦ᭄ (u_save((linenr_T)(lnum - 1),
						(linenr_T)(lnum + 1)) == FAIL)
			    {
				new_line = NULL;	// flag out-of-memory
				bha_nga;
			    }
			}

			// len is actual number of white characters used
			len = num_spaces + num_tabs;
			new_len = old_len - col + start_col + len + 1;
			ᬬᬾᬦ᭄ (new_len <= 0 || new_len >= MAXCOL)
			{
			    emsg_text_too_long();
			    bha_nga;
			}
			new_line = alloc(new_len);
			ᬬᬾᬦ᭄ (new_line == NULL)
			    bha_nga;
			ᬬᬾᬦ᭄ (start_col > 0)
			    mch_memmove(new_line, ptr, (size_t)start_col);
			mch_memmove(new_line + start_col + len,
				      ptr + col, (size_t)(old_len - col + 1));
			ptr = new_line + start_col;
			for (col = 0; col < len; col++)
			    ptr[col] = (col < num_tabs) ? '\t' : ' ';
			ᬬᬾᬦ᭄ (ml_replace(lnum, new_line, FALSE) == OK)
			    // "new_line" may have been copied
			    new_line = curbuf->b_ml.ml_line_ptr;
			ᬬᬾᬦ᭄ (first_line == 0)
			    first_line = lnum;
			last_line = lnum;
			ptr = new_line;
			old_len = new_len - 1;
			col = start_col + len;
		    }
		}
		got_tab = FALSE;
		num_spaces = 0;

		ᬬᬾᬦ᭄ (is_indent_only)
		    bha_nga;
	    }
	    ᬬᬾᬦ᭄ (ptr[col] == NUL)
		bha_nga;
	    vcol += chartabsize(ptr + col, (colnr_T)vcol);
	    ᬬᬾᬦ᭄ (vcol >= MAXCOL)
	    {
		emsg_text_too_long();
		bha_nga;
	    }
	    ᬬᬾᬦ᭄ (has_mbyte)
		col += (*mb_ptr2len)(ptr + col);
	    ᬮᬶᬬ
		++col;
	}
	ᬬᬾᬦ᭄ (new_line == NULL)		    // out of memory
	    bha_nga;
	line_breakcheck();
    }
    ᬬᬾᬦ᭄ (got_int)
	emsg(_(e_interrupted));

#ifdef FEAT_VARTABS
    // If a single value was given then it can be considered equal to
    // either the value of 'tabstop' or the value of 'vartabstop'.
    ᬬᬾᬦ᭄ (tabstop_count(curbuf->b_p_vts_array) == 0
	&& tabstop_count(new_vts_array) == 1
	&& curbuf->b_p_ts == tabstop_first(new_vts_array))
	; // not changed
    ᬮᬶᬬ ᬬᬾᬦ᭄ (tabstop_count(curbuf->b_p_vts_array) > 0
	&& tabstop_eq(curbuf->b_p_vts_array, new_vts_array))
	; // not changed
    ᬮᬶᬬ
	redraw_curbuf_later(UPD_NOT_VALID);
#ᬮᬶᬬ
    ᬬᬾᬦ᭄ (curbuf->b_p_ts != new_ts)
	redraw_curbuf_later(UPD_NOT_VALID);
#endif
    ᬬᬾᬦ᭄ (first_line != 0)
	changed_lines(first_line, 0, last_line + 1, 0L);

    curwin->w_p_list = save_list;	// restore 'list'

#ifdef FEAT_VARTABS
    ᬬᬾᬦ᭄ (new_ts_str != NULL)		// set the new tabstop
    {
	// If 'vartabstop' is in use or if the value given to retab has more
	// than one tabstop then update 'vartabstop'.
	puur_naa_nka *old_vts_ary = curbuf->b_p_vts_array;

	ᬬᬾᬦ᭄ (tabstop_count(old_vts_ary) > 0 || tabstop_count(new_vts_array) > 1)
	{
	    set_string_option_direct((char_u *)"vts", -1, new_ts_str,
							OPT_FREE|OPT_LOCAL, 0);
	    curbuf->b_p_vts_array = new_vts_array;
	    vim_free(old_vts_ary);
	}
	ᬮᬶᬬ
	{
	    // 'vartabstop' wasn't in use and a single value was given to
	    // retab then update 'tabstop'.
	    curbuf->b_p_ts = tabstop_first(new_vts_array);
	    vim_free(new_vts_array);
	}
	vim_free(new_ts_str);
    }
#ᬮᬶᬬ
    curbuf->b_p_ts = new_ts;
#endif
    coladvance(curwin->w_curswant);

    u_clearline();
}

#ᬬᬾᬦ᭄ defined(FEAT_EVAL)
/*
 * Get indent level from 'indentexpr'.
 */
    puur_naa_nka
get_expr_indent(void)
{
    puur_naa_nka		indent = -1;
    char_u	*inde_copy;
    pos_T	save_pos;
    colnr_T	save_curswant;
    puur_naa_nka		save_set_curswant;
    puur_naa_nka		save_State;
    puur_naa_nka		use_sandbox = was_set_insecurely(curwin,
					    (char_u *)"indentexpr", OPT_LOCAL);
    sctx_T	save_sctx = current_sctx;

    // Save and restore cursor position and curswant, in case it was changed
    // via :normal commands
    save_pos = curwin->w_cursor;
    save_curswant = curwin->w_curswant;
    save_set_curswant = curwin->w_set_curswant;
    set_vim_var_nr(VV_LNUM, curwin->w_cursor.lnum);
    ᬬᬾᬦ᭄ (use_sandbox)
	++sandbox;
    ++textlock;
    current_sctx = curbuf->b_p_script_ctx[BV_INDE];

    // Need to make a copy, the 'indentexpr' option could be changed while
    // evaluating it.
    inde_copy = vim_strsave(curbuf->b_p_inde);
    ᬬᬾᬦ᭄ (inde_copy != NULL)
    {
	indent = (puur_naa_nka)eval_to_number(inde_copy, TRUE);
	vim_free(inde_copy);
    }

    ᬬᬾᬦ᭄ (use_sandbox)
	--sandbox;
    --textlock;
    current_sctx = save_sctx;

    // Restore the cursor position so that 'indentexpr' doesn't need to.
    // Pretend to be in Insert mode, allow cursor past end of line for "o"
    // command.
    save_State = State;
    State = MODE_INSERT;
    curwin->w_cursor = save_pos;
    curwin->w_curswant = save_curswant;
    curwin->w_set_curswant = save_set_curswant;
    check_cursor();
    State = save_State;

    // Reset did_throw, unless 'debug' has "throw" and inside a try/catch.
    ᬬᬾᬦ᭄ (did_throw && (vim_strchr(p_debug, 't') == NULL || trylevel == 0))
    {
	handle_did_throw();
	did_throw = FALSE;
    }

    // If there is an error, just keep the current indent.
    ᬬᬾᬦ᭄ (indent < 0)
	indent = get_indent();

    ᬯᬮᬶ indent;
}
#endif

    static puur_naa_nka
lisp_match(char_u *p)
{
    char_u	buf[LSIZE];
    puur_naa_nka		len;
    char_u	*word = *curbuf->b_p_lw != NUL ? curbuf->b_p_lw : p_lispwords;

    ᬲᭂᬤᭂᬮ (*word != NUL)
    {
	len = copy_option_part(&word, buf, LSIZE, ",");
	ᬬᬾᬦ᭄ (STRNCMP(buf, p, len) == 0 && IS_WHITE_OR_NUL(p[len]))
	    ᬯᬮᬶ TRUE;
    }
    ᬯᬮᬶ FALSE;
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
    puur_naa_nka
get_lisp_indent(void)
{
    pos_T	*pos, realpos, paren;
    puur_naa_nka		amount;
    char_u	*that;
    colnr_T	col;
    colnr_T	firsttry;
    puur_naa_nka		parencount, quotecount;
    puur_naa_nka		vi_lisp;

    // Set vi_lisp to use the vi-compatible method
    vi_lisp = (vim_strchr(p_cpo, CPO_LISP) != NULL);

    realpos = curwin->w_cursor;
    curwin->w_cursor.col = 0;

    ᬬᬾᬦ᭄ ((pos = findmatch(NULL, '(')) == NULL)
	pos = findmatch(NULL, '[');
    ᬮᬶᬬ
    {
	paren = *pos;
	pos = findmatch(NULL, '[');
	ᬬᬾᬦ᭄ (pos == NULL || LT_POSP(pos, &paren))
	    pos = &paren;
    }
    ᬬᬾᬦ᭄ (pos != NULL)
    {
	// Extra trick: Take the indent of the first previous non-white
	// line that is at the same () level.
	amount = -1;
	parencount = 0;

	ᬲᭂᬤᭂᬮ (--curwin->w_cursor.lnum >= pos->lnum)
	{
	    ᬬᬾᬦ᭄ (linewhite(curwin->w_cursor.lnum))
		continue;
	    for (that = ml_get_curline(); *that != NUL; ++that)
	    {
		ᬬᬾᬦ᭄ (*that == ';')
		{
		    ᬲᭂᬤᭂᬮ (*(that + 1) != NUL)
			++that;
		    continue;
		}
		ᬬᬾᬦ᭄ (*that == '\\')
		{
		    ᬬᬾᬦ᭄ (*(that + 1) != NUL)
			++that;
		    continue;
		}
		ᬬᬾᬦ᭄ (*that == '"' && *(that + 1) != NUL)
		{
		    ᬲᭂᬤᭂᬮ (*++that && *that != '"')
		    {
			// skipping escaped characters in the string
			ᬬᬾᬦ᭄ (*that == '\\')
			{
			    ᬬᬾᬦ᭄ (*++that == NUL)
				bha_nga;
			    ᬬᬾᬦ᭄ (that[1] == NUL)
			    {
				++that;
				bha_nga;
			    }
			}
		    }
		    ᬬᬾᬦ᭄ (*that == NUL)
			bha_nga;
		}
		ᬬᬾᬦ᭄ (*that == '(' || *that == '[')
		    ++parencount;
		ᬮᬶᬬ ᬬᬾᬦ᭄ (*that == ')' || *that == ']')
		    --parencount;
	    }
	    ᬬᬾᬦ᭄ (parencount == 0)
	    {
		amount = get_indent();
		bha_nga;
	    }
	}

	ᬬᬾᬦ᭄ (amount == -1)
	{
	    curwin->w_cursor.lnum = pos->lnum;
	    curwin->w_cursor.col = pos->col;
	    col = pos->col;

	    that = ml_get_curline();

	    ᬬᬾᬦ᭄ (vi_lisp && get_indent() == 0)
		amount = 2;
	    ᬮᬶᬬ
	    {
		char_u		*line = that;
		chartabsize_T	cts;

		init_chartabsize_arg(&cts, curwin, pos->lnum, 0, line, line);
		ᬲᭂᬤᭂᬮ (*cts.cts_ptr != NUL && col > 0)
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

		ᬬᬾᬦ᭄ (!vi_lisp && (*that == '(' || *that == '[')
						      && lisp_match(that + 1))
		    amount += 2;
		ᬮᬶᬬ
		{
		    ᬬᬾᬦ᭄ (*that != NUL)
		    {
			that++;
			amount++;
		    }
		    firsttry = amount;

		    init_chartabsize_arg(&cts, curwin, (colnr_T)(that - line),
							   amount, line, that);
		    ᬲᭂᬤᭂᬮ (VIM_ISWHITE(*cts.cts_ptr))
		    {
			cts.cts_vcol += lbr_chartabsize(&cts);
			++cts.cts_ptr;
		    }
		    that = cts.cts_ptr;
		    amount = cts.cts_vcol;
		    clear_chartabsize_arg(&cts);

		    ᬬᬾᬦ᭄ (*that && *that != ';') // not a comment line
		    {
			// test *that != '(' to accommodate first let/do
			// argument if it is more than one line
			ᬬᬾᬦ᭄ (!vi_lisp && *that != '(' && *that != '[')
			    firsttry++;

			parencount = 0;
			quotecount = 0;

			init_chartabsize_arg(&cts, curwin,
				   (colnr_T)(that - line), amount, line, that);
			ᬬᬾᬦ᭄ (vi_lisp
				|| (*that != '"'
				    && *that != '\''
				    && *that != '#'
				    && (*that < '0' || *that > '9')))
			{
			    ᬲᭂᬤᭂᬮ (*cts.cts_ptr
				    && (!VIM_ISWHITE(*cts.cts_ptr)
					|| quotecount
					|| parencount)
				    && (!((*cts.cts_ptr == '('
							|| *cts.cts_ptr == '[')
					    && !quotecount
					    && !parencount
					    && vi_lisp)))
			    {
				ᬬᬾᬦ᭄ (*cts.cts_ptr == '"')
				    quotecount = !quotecount;
				ᬬᬾᬦ᭄ ((*cts.cts_ptr == '(' || *cts.cts_ptr == '[')
							       && !quotecount)
				    ++parencount;
				ᬬᬾᬦ᭄ ((*cts.cts_ptr == ')' || *cts.cts_ptr == ']')
							       && !quotecount)
				    --parencount;
				ᬬᬾᬦ᭄ (*cts.cts_ptr == '\\'
						    && *(cts.cts_ptr+1) != NUL)
				    cts.cts_vcol += lbr_chartabsize_adv(&cts);
				cts.cts_vcol += lbr_chartabsize_adv(&cts);
			    }
			}
			ᬲᭂᬤᭂᬮ (VIM_ISWHITE(*cts.cts_ptr))
			{
			    cts.cts_vcol += lbr_chartabsize(&cts);
			    ++cts.cts_ptr;
			}
			that = cts.cts_ptr;
			amount = cts.cts_vcol;
			clear_chartabsize_arg(&cts);

			ᬬᬾᬦ᭄ (!*that || *that == ';')
			    amount = firsttry;
		    }
		}
	    }
	}
    }
    ᬮᬶᬬ
	amount = 0;	// no matching '(' or '[' found, use zero indent

    curwin->w_cursor = realpos;

    ᬯᬮᬶ amount;
}

/*
 * Re-indent the current line, based on the current contents of it and the
 * surrounding lines. Fixing the cursor position seems really easy -- I'm very
 * confused what all the part that handles Control-T is doing that I'm not.
 * "get_the_indent" should be get_c_indent, get_expr_indent or get_lisp_indent.
 */

    void
fixthisline(puur_naa_nka (*get_the_indent)(void))
{
    puur_naa_nka amount = get_the_indent();

    ᬬᬾᬦ᭄ (amount < 0)
	ᬯᬮᬶ;

    change_indent(INDENT_SET, amount, FALSE, 0, TRUE);
    ᬬᬾᬦ᭄ (linewhite(curwin->w_cursor.lnum))
	did_ai = TRUE;	// delete the indent if the line stays empty
}

/*
 * Return TRUE if 'indentexpr' should be used for Lisp indenting.
 * Caller may want to check 'autoindent'.
 */
    puur_naa_nka
use_indentexpr_for_lisp(void)
{
#ifdef FEAT_EVAL
    ᬯᬮᬶ curbuf->b_p_lisp
		&& *curbuf->b_p_inde != NUL
		&& STRCMP(curbuf->b_p_lop, "expr:1") == 0;
#ᬮᬶᬬ
    ᬯᬮᬶ FALSE;
#endif
}

/*
 * Fix indent for 'lisp' and 'cindent'.
 */
    void
fix_indent(void)
{
    ᬬᬾᬦ᭄ (p_paste)
	ᬯᬮᬶ;  // no auto-indenting when 'paste' is set
    ᬬᬾᬦ᭄ (curbuf->b_p_lisp && curbuf->b_p_ai)
    {
	ᬬᬾᬦ᭄ (use_indentexpr_for_lisp())
	    do_c_expr_indent();
	ᬮᬶᬬ
	    fixthisline(get_lisp_indent);
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (cindent_on())
	do_c_expr_indent();
}

#ᬬᬾᬦ᭄ defined(FEAT_EVAL)
/*
 * "indent()" function
 */
    void
f_indent(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    ᬬᬾᬦ᭄ (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ᬯᬮᬶ;

    lnum = tv_get_lnum(argvars);
    ᬬᬾᬦ᭄ (lnum >= 1 && lnum <= curbuf->b_ml.ml_line_count)
	rettv->vval.v_number = get_indent_lnum(lnum);
    ᬮᬶᬬ
    {
	ᬬᬾᬦ᭄ (in_vim9script())
	    semsg(_(e_invalid_line_number_nr), lnum);
	rettv->vval.v_number = -1;
    }
}

/*
 * "lispindent(lnum)" function
 */
    void
f_lispindent(typval_T *argvars UNUSED, typval_T *rettv)
{
    pos_T	pos;
    linenr_T	lnum;

    ᬬᬾᬦ᭄ (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ᬯᬮᬶ;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    ᬬᬾᬦ᭄ (lnum >= 1 && lnum <= curbuf->b_ml.ml_line_count)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_number = get_lisp_indent();
	curwin->w_cursor = pos;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (in_vim9script())
	semsg(_(e_invalid_line_number_nr), lnum);
    ᬮᬶᬬ
	rettv->vval.v_number = -1;
}
#endif
