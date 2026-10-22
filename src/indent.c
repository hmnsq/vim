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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_VARTABS)

/*
 * Set the integer values corresponding to the string setting of 'vartabstop'.
 * "array" will be set, caller must free it if needed.
 * Return FAIL for an error.
 */
    పూర్ణాంకం
tabstop_సమితి(char_u *var, పూర్ణాంకం **array)
{
    పూర్ణాంకం	    valcount = 1;
    పూర్ణాంకం	    t;
    char_u  *cp;

    ఒకవేళ (var[0] == NUL || (var[0] == '0' && var[1] == NUL))
    {
	*array = ఖాళీ;
	తిరిగి OK;
    }

    కొరకు (cp = var; *cp != NUL; ++cp)
    {
	ఒకవేళ (cp == var || cp[-1] == ',')
	{
	    char_u *చివర;

	    ఒకవేళ (strtol((అక్షరం *)cp, (అక్షరం **)&చివర, 10) <= 0)
	    {
		ఒకవేళ (cp != చివర)
		    emsg(_(e_వాదం_must_be_ధనాత్మక));
		లేకపోతే
		    semsg(_(e_invalid_వాదం_str), cp);
		తిరిగి FAIL;
	    }
	}

	ఒకవేళ (VIM_ISDIGIT(*cp))
	    కొనసాగు;
	ఒకవేళ (cp[0] == ',' && cp > var && cp[-1] != ',' && cp[1] != NUL)
	{
	    ++valcount;
	    కొనసాగు;
	}
	semsg(_(e_invalid_వాదం_str), var);
	తిరిగి FAIL;
    }

    *array = ALLOC_MULT(పూర్ణాంకం, valcount + 1);
    ఒకవేళ (*array == ఖాళీ)
	తిరిగి FAIL;
    (*array)[0] = valcount;

    t = 1;
    కొరకు (cp = var; *cp != NUL;)
    {
	పూర్ణాంకం n = atoi((అక్షరం *)cp);

	// Catch negative values, overflow and ridiculous big values.
	ఒకవేళ (n <= 0 || n > TABSTOP_MAX)
	{
	    semsg(_(e_invalid_వాదం_str), cp);
	    VIM_CLEAR(*array);
	    తిరిగి FAIL;
	}
	(*array)[t++] = n;
	అయితే (*cp != NUL && *cp != ',')
	    ++cp;
	ఒకవేళ (*cp != NUL)
	    ++cp;
    }

    తిరిగి OK;
}

/*
 * Calculate the number of screen spaces a tab will occupy.
 * If "vts" is set then the tab widths are taken from that array,
 * otherwise the value of ts is used.
 */
    పూర్ణాంకం
tabstop_padding(colnr_T నిలువు, పూర్ణాంకం ts_arg, పూర్ణాంకం *vts)
{
    పూర్ణాంకం		ts = ts_arg == 0 ? 8 : ts_arg;
    పూర్ణాంకం		tabcount;
    colnr_T	tabcol = 0;
    పూర్ణాంకం		t;
    పూర్ణాంకం		padding = 0;

    ఒకవేళ (vts == ఖాళీ || vts[0] == 0)
	తిరిగి ts - (నిలువు % ts);

    tabcount = vts[0];

    కొరకు (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	ఒకవేళ (tabcol > నిలువు)
	{
	    padding = (పూర్ణాంకం)(tabcol - నిలువు);
	    ఆపు;
	}
    }
    ఒకవేళ (t > tabcount)
	padding = vts[tabcount] - (పూర్ణాంకం)((నిలువు - tabcol) % vts[tabcount]);

    తిరిగి padding;
}

/*
 * Find the size of the tab that covers a particular column.
 *
 * If this is being called as part of a shift operation, col is not the cursor
 * column but is the column number to the left of the first non-whitespace
 * character in the line.  If the shift is to the left (left = TRUE), then
 * return the size of the tab interval to the left of the column.
 */
    పూర్ణాంకం
tabstop_at(colnr_T నిలువు, పూర్ణాంకం ts, పూర్ణాంకం *vts, పూర్ణాంకం ఎడమ_2)
{
    పూర్ణాంకం		tabcount;	// Number of tab stops in the list of variable
				// tab stops.
    colnr_T	tabcol = 0;	// Column of the tab stop under consideration.
    పూర్ణాంకం		t;		// Tabstop index in the list of variable tab
				// stops.
    పూర్ణాంకం		tab_కొలత = 0;	// Size of the tab stop interval to the right
				// or left of the col.

    ఒకవేళ (vts == 0 || vts[0] == 0)
	తిరిగి ts;

    tabcount = vts[0];
    కొరకు (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	ఒకవేళ (tabcol > నిలువు)
	{
	    // If shifting left (left != 0), and if the column to the left of
	    // the first first non-blank character (col) in the line is
	    // already to the left of the first tabstop, set the shift amount
	    // (tab_size) to just enough to shift the line to the left margin.
	    // The value doesn't seem to matter as long as it is at least that
	    // distance.
	    ఒకవేళ (ఎడమ_2 && (t == 1))
		tab_కొలత = నిలువు;
	    లేకపోతే
		tab_కొలత = vts[t - (ఎడమ_2 ? 1 : 0)];
	    ఆపు;
	}
    }
    ఒకవేళ (t > tabcount)		// If the value of the index t is beyond the
				// end of the list, use the tab stop value at
				// the end of the list.
	tab_కొలత = vts[tabcount];

    తిరిగి tab_కొలత;
}

/*
 * Find the column on which a tab starts.
 */
    colnr_T
tabstop_ప్రారంభం(colnr_T నిలువు, పూర్ణాంకం ts, పూర్ణాంకం *vts)
{
    పూర్ణాంకం		tabcount;
    colnr_T	tabcol = 0;
    పూర్ణాంకం		t;
    పూర్ణాంకం		excess;

    ఒకవేళ (vts == ఖాళీ || vts[0] == 0)
	తిరిగి నిలువు - నిలువు % ts;

    tabcount = vts[0];
    కొరకు (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	ఒకవేళ (tabcol > నిలువు)
	    తిరిగి tabcol - vts[t];
    }

    excess = tabcol % vts[tabcount];
    తిరిగి నిలువు - (నిలువు - excess) % vts[tabcount];
}

/*
 * Find the number of tabs and spaces necessary to get from one column
 * to another.
 */
    శూన్య
tabstop_fromto(
	colnr_T ప్రారంభం_నిలువు,
	colnr_T చివర_నిలువు,
	పూర్ణాంకం	ts_arg,
	పూర్ణాంకం	*vts,
	పూర్ణాంకం	*ntabs,
	పూర్ణాంకం	*nspcs)
{
    పూర్ణాంకం		spaces = చివర_నిలువు - ప్రారంభం_నిలువు;
    colnr_T	tabcol = 0;
    పూర్ణాంకం		padding = 0;
    పూర్ణాంకం		tabcount;
    పూర్ణాంకం		t;
    పూర్ణాంకం		ts = ts_arg == 0 ? curbuf->b_p_ts : ts_arg;

    ఒకవేళ (vts == ఖాళీ || vts[0] == 0)
    {
	పూర్ణాంకం tabs = 0;
	పూర్ణాంకం initspc = 0;

	initspc = ts - (ప్రారంభం_నిలువు % ts);
	ఒకవేళ (spaces >= initspc)
	{
	    spaces -= initspc;
	    tabs++;
	}
	tabs += spaces / ts;
	spaces -= (spaces / ts) * ts;

	*ntabs = tabs;
	*nspcs = spaces;
	తిరిగి;
    }

    // Find the padding needed to reach the next tabstop.
    tabcount = vts[0];
    కొరకు (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	ఒకవేళ (tabcol > ప్రారంభం_నిలువు)
	{
	    padding = (పూర్ణాంకం)(tabcol - ప్రారంభం_నిలువు);
	    ఆపు;
	}
    }
    ఒకవేళ (t > tabcount)
	padding = vts[tabcount] - (పూర్ణాంకం)((ప్రారంభం_నిలువు - tabcol) % vts[tabcount]);

    // If the space needed is less than the padding no tabs can be used.
    ఒకవేళ (spaces < padding)
    {
	*ntabs = 0;
	*nspcs = spaces;
	తిరిగి;
    }

    *ntabs = 1;
    spaces -= padding;

    // At least one tab has been used. See if any more will fit.
    అయితే (spaces != 0 && ++t <= tabcount)
    {
	padding = vts[t];
	ఒకవేళ (spaces < padding)
	{
	    *nspcs = spaces;
	    తిరిగి;
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
    స్థిర పూర్ణాంకం
tabstop_eq(పూర్ణాంకం *ts1, పూర్ణాంకం *ts2)
{
    పూర్ణాంకం		t;

    ఒకవేళ ((ts1 == 0 && ts2) || (ts1 && ts2 == 0))
	తిరిగి FALSE;
    ఒకవేళ (ts1 == ts2)
	తిరిగి TRUE;
    ఒకవేళ (ts1[0] != ts2[0])
	తిరిగి FALSE;

    కొరకు (t = 1; t <= ts1[0]; ++t)
	ఒకవేళ (ts1[t] != ts2[t])
	    తిరిగి FALSE;

    తిరిగి TRUE;
}

# ఒకవేళ defined(FEAT_BEVAL)
/*
 * Copy a tabstop array, allocating space for the new array.
 */
    పూర్ణాంకం *
tabstop_copy(పూర్ణాంకం *oldts)
{
    పూర్ణాంకం		*newts;
    పూర్ణాంకం		t;

    ఒకవేళ (oldts == ఖాళీ)
	తిరిగి ఖాళీ;
    newts = ALLOC_MULT(పూర్ణాంకం, oldts[0] + 1);
    ఒకవేళ (newts != ఖాళీ)
	కొరకు (t = 0; t <= oldts[0]; ++t)
	    newts[t] = oldts[t];
    తిరిగి newts;
}
# endif

/*
 * Return a count of the number of tabstops.
 */
    పూర్ణాంకం
tabstop_లెక్క(పూర్ణాంకం *ts)
{
    తిరిగి ts != ఖాళీ ? ts[0] : 0;
}

/*
 * Return the first tabstop, or 8 if there are no tabstops defined.
 */
    పూర్ణాంకం
tabstop_మొదటి(పూర్ణాంకం *ts)
{
    తిరిగి ts != ఖాళీ ? ts[1] : 8;
}

#అయితే_ముగింపు

/*
 * Return the effective shiftwidth value for current buffer, using the
 * 'tabstop' value when 'shiftwidth' is zero.
 */
    దీర్ఘ
get_sw_విలువ(buf_T *buf)
{
    తిరిగి get_sw_విలువ_నిలువు(buf, 0, FALSE);
}

/*
 * Idem, using "pos".
 */
    స్థిర దీర్ఘ
get_sw_విలువ_pos(buf_T *buf, pos_T *pos, పూర్ణాంకం ఎడమ_2)
{
    pos_T save_cursor = curwin->w_cursor;
    దీర్ఘ sw_విలువ;

    curwin->w_cursor = *pos;
    sw_విలువ = get_sw_విలువ_నిలువు(buf, get_nolist_virtcol(), ఎడమ_2);
    curwin->w_cursor = save_cursor;
    తిరిగి sw_విలువ;
}

/*
 * Idem, using the first non-black in the current line.
 */
    దీర్ఘ
get_sw_విలువ_indent(buf_T *buf, పూర్ణాంకం ఎడమ_2)
{
    pos_T pos = curwin->w_cursor;

    pos.నిలువు = getwhitecols_curline();
    తిరిగి get_sw_విలువ_pos(buf, &pos, ఎడమ_2);
}

/*
 * Idem, using virtual column "col".
 */
    దీర్ఘ
get_sw_విలువ_నిలువు(buf_T *buf, colnr_T నిలువు వాడని, పూర్ణాంకం ఎడమ_2 వాడని)
{
    తిరిగి buf->b_p_sw ? buf->b_p_sw :
#నిర్వచితమైతే FEAT_VARTABS
	tabstop_at(నిలువు, buf->b_p_ts, buf->b_p_vts_array, ఎడమ_2);
#లేకపోతే
	buf->b_p_ts;
#అయితే_ముగింపు
}

/*
 * Return the effective softtabstop value for the current buffer, using the
 * 'shiftwidth' value when 'softtabstop' is negative.
 */
    దీర్ఘ
get_sts_విలువ(శూన్య)
{
    తిరిగి curbuf->b_p_sts < 0 ? get_sw_విలువ(curbuf) : curbuf->b_p_sts;
}

/*
 * Count the size (in window cells) of the indent in the current line.
 */
    పూర్ణాంకం
get_indent(శూన్య)
{
#నిర్వచితమైతే FEAT_VARTABS
    తిరిగి get_indent_str_vtab(ml_get_curline(), (పూర్ణాంకం)curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#లేకపోతే
    తిరిగి get_indent_str(ml_get_curline(), (పూర్ణాంకం)curbuf->b_p_ts, FALSE);
#అయితే_ముగింపు
}

/*
 * Count the size (in window cells) of the indent in line "lnum".
 */
    పూర్ణాంకం
get_indent_lnum(linenr_T lnum)
{
#నిర్వచితమైతే FEAT_VARTABS
    తిరిగి get_indent_str_vtab(ml_get(lnum), (పూర్ణాంకం)curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#లేకపోతే
    తిరిగి get_indent_str(ml_get(lnum), (పూర్ణాంకం)curbuf->b_p_ts, FALSE);
#అయితే_ముగింపు
}

#ఒకవేళ defined(FEAT_FOLDING)
/*
 * Count the size (in window cells) of the indent in line "lnum" of buffer
 * "buf".
 */
    పూర్ణాంకం
get_indent_buf(buf_T *buf, linenr_T lnum)
{
# ifdef FEAT_VARTABS
    తిరిగి get_indent_str_vtab(ml_get_buf(buf, lnum, FALSE),
			       (పూర్ణాంకం)buf->b_p_ts, buf->b_p_vts_array, FALSE);
# లేకపోతే
    తిరిగి get_indent_str(ml_get_buf(buf, lnum, FALSE), (పూర్ణాంకం)buf->b_p_ts, FALSE);
# endif
}
#అయితే_ముగింపు

/*
 * count the size (in window cells) of the indent in line "ptr", with
 * 'tabstop' at "ts"
 */
    పూర్ణాంకం
get_indent_str(
    char_u	*ptr,
    పూర్ణాంకం		ts,
    పూర్ణాంకం		no_ts) // if TRUE, count a tab as ^I
{
    పూర్ణాంకం		లెక్క = 0;

    కొరకు ( ; *ptr; ++ptr)
    {
	ఒకవేళ (*ptr == TAB)    // count a tab for what it is worth
	{
	    ఒకవేళ (!no_ts)
		లెక్క += ts - (లెక్క % ts);
	    లేకపోతే
		లెక్క += ptr2cells(ptr);
	}
	లేకపోతే ఒకవేళ (*ptr == ' ')
	    ++లెక్క;		// count a space for one
	లేకపోతే
	    ఆపు;
    }
    తిరిగి లెక్క;
}

#ఒకవేళ defined(FEAT_VARTABS)
/*
 * Count the size (in window cells) of the indent in line "ptr", using
 * variable tabstops.
 * If "no_ts" is TRUE, count a tab as ^I.
 */
    పూర్ణాంకం
get_indent_str_vtab(char_u *ptr, పూర్ణాంకం ts, పూర్ణాంకం *vts, పూర్ణాంకం no_ts)
{
    పూర్ణాంకం		లెక్క = 0;

    కొరకు ( ; *ptr; ++ptr)
    {
	ఒకవేళ (*ptr == TAB)    // count a tab for what it is worth
	{
	    ఒకవేళ (!no_ts)
		లెక్క += tabstop_padding(లెక్క, ts, vts);
	    లేకపోతే
		లెక్క += ptr2cells(ptr);
	}
	లేకపోతే ఒకవేళ (*ptr == ' ')
	    ++లెక్క;		// count a space for one
	లేకపోతే
	    ఆపు;
    }
    తిరిగి లెక్క;
}
#అయితే_ముగింపు

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
    పూర్ణాంకం
సమితి_indent(
    పూర్ణాంకం		కొలత,		    // measured in spaces
    పూర్ణాంకం		flags)
{
    char_u	*p;
    char_u	*newline;
    char_u	*oldline;
    char_u	*s;
    పూర్ణాంకం		todo;
    పూర్ణాంకం		ind_len;	    // measured in characters
    పూర్ణాంకం		పంక్తి_len;	    // size of the line (including the NUL)
    పూర్ణాంకం		doit = FALSE;
    పూర్ణాంకం		ind_పూర్తి = 0;	    // measured in spaces
#నిర్వచితమైతే FEAT_VARTABS
    పూర్ణాంకం		ind_నిలువు = 0;
#అయితే_ముగింపు
    పూర్ణాంకం		tab_pad;
    పూర్ణాంకం		retval = FALSE;
    పూర్ణాంకం		orig_char_len = -1; // number of initial whitespace chars when
				    // 'et' and 'pi' are both set

    // First check if there is anything to do and compute the number of
    // characters needed for the indent.
    todo = కొలత;
    ind_len = 0;
    p = oldline = ml_get_curline();
    పంక్తి_len = ml_get_curline_len() + 1;

    // Calculate the buffer size for the new indent, and check to see if it
    // isn't already set

    // if 'expandtab' isn't set: use TABs; if both 'expandtab' and
    // 'preserveindent' are set count the number of characters at the
    // beginning of the line to be copied
    ఒకవేళ (!curbuf->b_p_et || (!(flags & SIN_చొప్పించు) && curbuf->b_p_pi))
    {
	// If 'preserveindent' is set then reuse as much as possible of
	// the existing indent structure for the new indent
	ఒకవేళ (!(flags & SIN_చొప్పించు) && curbuf->b_p_pi)
	{
	    ind_పూర్తి = 0;

	    // count as many characters as we can use
	    అయితే (todo > 0 && VIM_ISWHITE(*p))
	    {
		ఒకవేళ (*p == TAB)
		{
#నిర్వచితమైతే FEAT_VARTABS
		    tab_pad = tabstop_padding(ind_పూర్తి, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#లేకపోతే
		    tab_pad = (పూర్ణాంకం)curbuf->b_p_ts
					   - (ind_పూర్తి % (పూర్ణాంకం)curbuf->b_p_ts);
#అయితే_ముగింపు
		    // stop if this tab will overshoot the target
		    ఒకవేళ (todo < tab_pad)
			ఆపు;
		    todo -= tab_pad;
		    ++ind_len;
		    ind_పూర్తి += tab_pad;
		}
		లేకపోతే
		{
		    --todo;
		    ++ind_len;
		    ++ind_పూర్తి;
		}
		++p;
	    }

#నిర్వచితమైతే FEAT_VARTABS
	    // These diverge from this point.
	    ind_నిలువు = ind_పూర్తి;
#అయితే_ముగింపు
	    // Set initial number of whitespace chars to copy if we are
	    // preserving indent but expandtab is set
	    ఒకవేళ (curbuf->b_p_et)
		orig_char_len = ind_len;

	    // Fill to next tabstop with a tab, if possible
#నిర్వచితమైతే FEAT_VARTABS
	    tab_pad = tabstop_padding(ind_పూర్తి, curbuf->b_p_ts,
						curbuf->b_p_vts_array);
#లేకపోతే
	    tab_pad = (పూర్ణాంకం)curbuf->b_p_ts - (ind_పూర్తి % (పూర్ణాంకం)curbuf->b_p_ts);
#అయితే_ముగింపు
	    ఒకవేళ (todo >= tab_pad && orig_char_len == -1)
	    {
		doit = TRUE;
		todo -= tab_pad;
		++ind_len;
		// ind_done += tab_pad;
#నిర్వచితమైతే FEAT_VARTABS
		ind_నిలువు += tab_pad;
#అయితే_ముగింపు
	    }
	}

	// count tabs required for indent
#నిర్వచితమైతే FEAT_VARTABS
	కొరకు (;;)
	{
	    tab_pad = tabstop_padding(ind_నిలువు, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
	    ఒకవేళ (todo < tab_pad)
		ఆపు;
	    ఒకవేళ (*p != TAB)
		doit = TRUE;
	    లేకపోతే
		++p;
	    todo -= tab_pad;
	    ++ind_len;
	    ind_నిలువు += tab_pad;
	}
#లేకపోతే
	అయితే (todo >= (పూర్ణాంకం)curbuf->b_p_ts)
	{
	    ఒకవేళ (*p != TAB)
		doit = TRUE;
	    లేకపోతే
		++p;
	    todo -= (పూర్ణాంకం)curbuf->b_p_ts;
	    ++ind_len;
	    // ind_done += (int)curbuf->b_p_ts;
	}
#అయితే_ముగింపు
    }
    // count spaces required for indent
    అయితే (todo > 0)
    {
	ఒకవేళ (*p != ' ')
	    doit = TRUE;
	లేకపోతే
	    ++p;
	--todo;
	++ind_len;
	// ++ind_done;
    }

    // Return if the indent is OK already.
    ఒకవేళ (!doit && !VIM_ISWHITE(*p) && !(flags & SIN_చొప్పించు))
	తిరిగి FALSE;

    // Allocate memory for the new line.
    ఒకవేళ (flags & SIN_చొప్పించు)
	p = oldline;
    లేకపోతే
    {
	p = skipwhite(p);
	పంక్తి_len -= (పూర్ణాంకం)(p - oldline);
    }

    // If 'preserveindent' and 'expandtab' are both set keep the original
    // characters and allocate accordingly.  We will fill the rest with spaces
    // after the if (!curbuf->b_p_et) below.
    ఒకవేళ (orig_char_len != -1)
    {
	newline = alloc(orig_char_len + కొలత - ind_పూర్తి + పంక్తి_len);
	ఒకవేళ (newline == ఖాళీ)
	    తిరిగి FALSE;
	todo = కొలత - ind_పూర్తి;
	ind_len = orig_char_len + todo;    // Set total length of indent in
					   // characters, which may have been
					   // undercounted until now
	p = oldline;
	s = newline;
	అయితే (orig_char_len > 0)
	{
	    *s++ = *p++;
	    orig_char_len--;
	}

	// Skip over any additional white space (useful when newindent is less
	// than old)
	అయితే (VIM_ISWHITE(*p))
	    ++p;

    }
    లేకపోతే
    {
	todo = కొలత;
	newline = alloc(ind_len + పంక్తి_len);
	ఒకవేళ (newline == ఖాళీ)
	    తిరిగి FALSE;
	s = newline;
    }

    // Put the characters in the new line.
    // if 'expandtab' isn't set: use TABs
    ఒకవేళ (!curbuf->b_p_et)
    {
	// If 'preserveindent' is set then reuse as much as possible of
	// the existing indent structure for the new indent
	ఒకవేళ (!(flags & SIN_చొప్పించు) && curbuf->b_p_pi)
	{
	    p = oldline;
	    ind_పూర్తి = 0;

	    అయితే (todo > 0 && VIM_ISWHITE(*p))
	    {
		ఒకవేళ (*p == TAB)
		{
#నిర్వచితమైతే FEAT_VARTABS
		    tab_pad = tabstop_padding(ind_పూర్తి, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#లేకపోతే
		    tab_pad = (పూర్ణాంకం)curbuf->b_p_ts
					   - (ind_పూర్తి % (పూర్ణాంకం)curbuf->b_p_ts);
#అయితే_ముగింపు
		    // stop if this tab will overshoot the target
		    ఒకవేళ (todo < tab_pad)
			ఆపు;
		    todo -= tab_pad;
		    ind_పూర్తి += tab_pad;
		}
		లేకపోతే
		{
		    --todo;
		    ++ind_పూర్తి;
		}
		*s++ = *p++;
	    }

	    // Fill to next tabstop with a tab, if possible
#నిర్వచితమైతే FEAT_VARTABS
	    tab_pad = tabstop_padding(ind_పూర్తి, curbuf->b_p_ts,
						curbuf->b_p_vts_array);
#లేకపోతే
	    tab_pad = (పూర్ణాంకం)curbuf->b_p_ts - (ind_పూర్తి % (పూర్ణాంకం)curbuf->b_p_ts);
#అయితే_ముగింపు
	    ఒకవేళ (todo >= tab_pad)
	    {
		*s++ = TAB;
		todo -= tab_pad;
#నిర్వచితమైతే FEAT_VARTABS
		ind_పూర్తి += tab_pad;
#అయితే_ముగింపు
	    }

	    p = skipwhite(p);
	}

#నిర్వచితమైతే FEAT_VARTABS
	కొరకు (;;)
	{
	    tab_pad = tabstop_padding(ind_పూర్తి, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
	    ఒకవేళ (todo < tab_pad)
		ఆపు;
	    *s++ = TAB;
	    todo -= tab_pad;
	    ind_పూర్తి += tab_pad;
	}
#లేకపోతే
	అయితే (todo >= (పూర్ణాంకం)curbuf->b_p_ts)
	{
	    *s++ = TAB;
	    todo -= (పూర్ణాంకం)curbuf->b_p_ts;
	}
#అయితే_ముగింపు
    }
    అయితే (todo > 0)
    {
	*s++ = ' ';
	--todo;
    }
    mch_memmove(s, p, (size_t)పంక్తి_len);

    // Replace the line (unless undo fails).
    ఒకవేళ (!(flags & SIN_UNDO) || u_savesub(curwin->w_cursor.lnum) == OK)
    {
	colnr_T old_స్థానభ్రంశం = (colnr_T)(p - oldline);
	colnr_T new_స్థానభ్రంశం = (colnr_T)(s - newline);

	// this may free "newline"
	ml_replace(curwin->w_cursor.lnum, newline, FALSE);
	ఒకవేళ (flags & SIN_మారింది)
	    మారింది_బైట్లు(curwin->w_cursor.lnum, 0);

	// Correct saved cursor position if it is in this line.
	ఒకవేళ (saved_cursor.lnum == curwin->w_cursor.lnum)
	{
	    ఒకవేళ (saved_cursor.నిలువు >= old_స్థానభ్రంశం)
		// cursor was after the indent, adjust for the number of
		// bytes added/removed
		saved_cursor.నిలువు += ind_len - old_స్థానభ్రంశం;
	    లేకపోతే ఒకవేళ (saved_cursor.నిలువు >= new_స్థానభ్రంశం)
		// cursor was in the indent, and is now after it, put it back
		// at the start of the indent (replacing spaces with TAB)
		saved_cursor.నిలువు = new_స్థానభ్రంశం;
	}
#నిర్వచితమైతే FEAT_PROP_POPUP
	ఒకవేళ (curbuf->b_has_textprop)
	{
	    పూర్ణాంకం added = ind_len - old_స్థానభ్రంశం;

	    // When increasing indent this behaves like spaces were inserted at
	    // the old indent, when decreasing indent it behaves like spaces
	    // were deleted at the new indent.
	    adjust_prop_columns(curwin->w_cursor.lnum,
				  added > 0 ? old_స్థానభ్రంశం : (colnr_T)ind_len,
				  added, APC_INDENT);
	}
#అయితే_ముగింపు
	retval = TRUE;
    }
    లేకపోతే
	vim_free(newline);

    curwin->w_cursor.నిలువు = ind_len;
    తిరిగి retval;
}

/*
 * Return the indent of the current line after a number.  Return -1 if no
 * number was found.  Used for 'n' in 'formatoptions': numbered list.
 * Since a pattern is used it can actually handle more than numbers.
 */
    పూర్ణాంకం
get_సంఖ్య_indent(linenr_T lnum)
{
    colnr_T	నిలువు;
    pos_T	pos;

    regmatch_T	regmatch;
    పూర్ణాంకం		lead_len = 0;	// length of comment leader

    ఒకవేళ (lnum > curbuf->b_ml.ml_పంక్తి_లెక్క)
	తిరిగి -1;
    pos.lnum = 0;

    // In format_lines() (i.e. not insert mode), fo+=q is needed too...
    ఒకవేళ ((State & బహుళకం_చొప్పించు) || has_format_option(FO_Q_COMS))
	lead_len = get_leader_len(ml_get(lnum), ఖాళీ, FALSE, TRUE);

    regmatch.regprog = vim_regcomp(curbuf->b_p_flp, RE_MAGIC);
    ఒకవేళ (regmatch.regprog != ఖాళీ)
    {
	regmatch.rm_ic = FALSE;

	// vim_regexec() expects a pointer to a line.  This lets us
	// start matching for the flp beyond any comment leader...
	ఒకవేళ (vim_regexec(&regmatch, ml_get(lnum) + lead_len, (colnr_T)0))
	{
	    pos.lnum = lnum;
	    pos.నిలువు = (colnr_T)(*regmatch.endp - ml_get(lnum));
	    pos.coladd = 0;
	}
	vim_regfree(regmatch.regprog);
    }

    ఒకవేళ (pos.lnum == 0 || *ml_get_pos(&pos) == NUL)
	తిరిగి -1;
    getvcol(curwin, &pos, &నిలువు, ఖాళీ, ఖాళీ, 0);
    తిరిగి (పూర్ణాంకం)నిలువు;
}

#ఒకవేళ defined(FEAT_LINEBREAK)
/*
 * Check "briopt" as 'breakindentopt' and update the members of "wp".
 * This is called when 'breakindentopt' is changed and when a window is
 * initialized.
 * Returns FAIL for failure, OK otherwise.
 */
    పూర్ణాంకం
briopt_check(
    char_u	*briopt,	// when NULL: use "wp->w_p_briopt"
    win_T	*wp)		// when NULL: only check "briopt"
{
    char_u	*p;
    పూర్ణాంకం		bri_జరుపు = 0;
    దీర్ఘ	bri_min = 20;
    పూర్ణాంకం		bri_sbr = FALSE;
    పూర్ణాంకం		bri_list = 0;
    పూర్ణాంకం		bri_vcol = 0;

    ఒకవేళ (briopt != ఖాళీ)
	p = briopt;
    లేకపోతే
	p = wp->w_p_briopt;

    అయితే (*p != NUL)
    {
	// Note: Keep this in sync with p_briopt_values
	ఒకవేళ (STRNCMP(p, "shift:", 6) == 0
		 && ((p[6] == '-' && VIM_ISDIGIT(p[7])) || VIM_ISDIGIT(p[6])))
	{
	    p += 6;
	    bri_జరుపు = getdigits(&p);
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "min:", 4) == 0 && VIM_ISDIGIT(p[4]))
	{
	    p += 4;
	    bri_min = getdigits(&p);
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "sbr", 3) == 0)
	{
	    p += 3;
	    bri_sbr = TRUE;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "list:", 5) == 0)
	{
	    p += 5;
	    bri_list = getdigits(&p);
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "column:", 7) == 0)
	{
	    p += 7;
	    bri_vcol = getdigits(&p);
	}
	ఒకవేళ (*p != ',' && *p != NUL)
	    తిరిగి FAIL;
	ఒకవేళ (*p == ',')
	    ++p;
    }

    ఒకవేళ (wp == ఖాళీ)
	తిరిగి OK;

    wp->w_briopt_జరుపు = bri_జరుపు;
    wp->w_briopt_min   = bri_min;
    wp->w_briopt_sbr   = bri_sbr;
    wp->w_briopt_list  = bri_list;
    wp->w_briopt_vcol  = bri_vcol;

    తిరిగి OK;
}

/*
 * Return appropriate space number for breakindent, taking influencing
 * parameters into account. Window must be specified, since it is not
 * necessarily always the current one.
 */
    పూర్ణాంకం
get_breakindent_win(
    win_T	*wp,
    char_u	*పంక్తి) // start of the line
{
    స్థిర పూర్ణాంకం	    prev_indent = 0;	// cached indent value
    స్థిర దీర్ఘ	    prev_ts     = 0L;	// cached tabstop value
# ifdef FEAT_VARTABS
    స్థిర పూర్ణాంకం      *prev_vts = ఖాళీ;   // cached vartabs values
# endif
    స్థిర పూర్ణాంకం	    prev_fnum   = 0;	// cached buffer number
    స్థిర char_u   *prev_పంక్తి  = ఖాళీ;	// cached copy of "line"
    స్థిర varnumber_T prev_tick = 0;   // changedtick of cached value
    స్థిర పూర్ణాంకం      prev_list = 0;	// cached list indent
    స్థిర పూర్ణాంకం      prev_listopt = 0;	// cached w_p_briopt_list value
    స్థిర పూర్ణాంకం      prev_no_ts = FALSE;	// cached no_ts value
    స్థిర గుర్తులేని prev_dy_uhex = 0;	// cached 'display' "uhex" value
    // cached formatlistpat value
    స్థిర char_u   *prev_flp = ఖాళీ;
    పూర్ణాంకం		    bri = 0;
    // window width minus window margin space, i.e. what rests for text
    స్థిరరాశి పూర్ణాంకం	    eff_wwidth = wp->w_వెడల్పు
					  - win_నిలువు_off(wp) + win_నిలువు_off2(wp);

    // In list mode, if 'listchars' "tab" isn't set, a TAB is displayed as ^I.
    పూర్ణాంకం no_ts = wp->w_p_list && wp->w_lcs_chars.tab1 == NUL;

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
    ఒకవేళ (prev_fnum != wp->w_బఫరు->b_fnum
	    || prev_ts != wp->w_బఫరు->b_p_ts
# ifdef FEAT_VARTABS
	    || prev_vts != wp->w_బఫరు->b_p_vts_array
# endif
	    || prev_tick != CHANGEDTICK(wp->w_బఫరు)
	    || prev_listopt != wp->w_briopt_list
	    || prev_no_ts != no_ts
	    || prev_dy_uhex != (dy_flags & DY_UHEX)
	    || prev_flp == ఖాళీ
	    || STRCMP(prev_flp, get_flp_విలువ(wp->w_బఫరు)) != 0
	    || prev_పంక్తి == ఖాళీ || STRCMP(prev_పంక్తి, పంక్తి) != 0
	)
    {
	prev_fnum = wp->w_బఫరు->b_fnum;
	vim_free(prev_పంక్తి);
	prev_పంక్తి = vim_strsave(పంక్తి);
	prev_ts = wp->w_బఫరు->b_p_ts;
# ifdef FEAT_VARTABS
	prev_vts = wp->w_బఫరు->b_p_vts_array;
	ఒకవేళ (wp->w_briopt_vcol == 0)
	    prev_indent = get_indent_str_vtab(పంక్తి,
				     (పూర్ణాంకం)wp->w_బఫరు->b_p_ts,
				      wp->w_బఫరు->b_p_vts_array, no_ts);
# లేకపోతే
	ఒకవేళ (wp->w_briopt_vcol == 0)
	    prev_indent = get_indent_str(పంక్తి,
					(పూర్ణాంకం)wp->w_బఫరు->b_p_ts, no_ts);
# endif
	prev_tick = CHANGEDTICK(wp->w_బఫరు);
	prev_listopt = wp->w_briopt_list;
	prev_list = 0;
	prev_no_ts = no_ts;
	prev_dy_uhex = (dy_flags & DY_UHEX);
	vim_free(prev_flp);
	prev_flp = vim_strsave(get_flp_విలువ(wp->w_బఫరు));
	// add additional indent for numbered lists
	ఒకవేళ (wp->w_briopt_list != 0 && wp->w_briopt_vcol == 0)
	{
	    regmatch_T	    regmatch;

	    regmatch.regprog = vim_regcomp(prev_flp,
				   RE_MAGIC + RE_STRING + RE_AUTO + RE_STRICT);

	    ఒకవేళ (regmatch.regprog != ఖాళీ)
	    {
		regmatch.rm_ic = FALSE;
		ఒకవేళ (vim_regexec(&regmatch, పంక్తి, 0))
		{
		    ఒకవేళ (wp->w_briopt_list > 0)
			prev_list = wp->w_briopt_list;
		    లేకపోతే
		    {
			char_u	*ptr = *regmatch.startp;
			char_u	*end_ptr = *regmatch.endp;
			పూర్ణాంకం	indent = 0;

			// Compute the width of the matched text.
			// Use win_chartabsize() so that TAB size is correct,
			// while wrapping is ignored.
			అయితే (ptr < end_ptr)
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
    ఒకవేళ (wp->w_briopt_vcol != 0)
    {
	// column value has priority
	bri = wp->w_briopt_vcol;
	prev_list = 0;
    }
    లేకపోతే
	bri = prev_indent + wp->w_briopt_జరుపు;

    // Add offset for number column, if 'n' is in 'cpoptions'
    bri += win_నిలువు_off2(wp);

    // add additional indent for numbered lists
    ఒకవేళ (wp->w_briopt_list > 0)
	bri += prev_list;

    // indent minus the length of the showbreak string
    ఒకవేళ (wp->w_briopt_sbr)
	bri -= vim_strsize(get_showbreak_విలువ(wp));


    // never indent past left window margin
    ఒకవేళ (bri < 0)
	bri = 0;

    // always leave at least bri_min characters on the left,
    // if text width is sufficient
    లేకపోతే ఒకవేళ (bri > eff_wwidth - wp->w_briopt_min)
	bri = (eff_wwidth - wp->w_briopt_min < 0)
					   ? 0 : eff_wwidth - wp->w_briopt_min;

    తిరిగి bri;
}
#అయితే_ముగింపు

/*
 * When extra == 0: Return TRUE if the cursor is before or on the first
 *		    non-blank in the line.
 * When extra == 1: Return TRUE if the cursor is before the first non-blank in
 *		    the line.
 */
    పూర్ణాంకం
inindent(పూర్ణాంకం extra)
{
    char_u	*ptr;
    colnr_T	నిలువు;

    కొరకు (నిలువు = 0, ptr = ml_get_curline(); VIM_ISWHITE(*ptr); ++నిలువు)
	++ptr;
    ఒకవేళ (నిలువు >= curwin->w_cursor.నిలువు + extra)
	తిరిగి TRUE;
    లేకపోతే
	తిరిగి FALSE;
}

/*
 * op_reindent - handle reindenting a block of lines.
 */
    శూన్య
op_reindent(oparg_T *oap, పూర్ణాంకం (*how)(శూన్య))
{
    దీర్ఘ	i = 0;
    char_u	*l;
    పూర్ణాంకం		amount;
    linenr_T	మొదటి_మారింది = 0;
    linenr_T	చివరి_మారింది = 0;
    linenr_T	ప్రారంభం_lnum = curwin->w_cursor.lnum;

    // Don't even try when 'modifiable' is off.
    ఒకవేళ (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	తిరిగి;
    }

    // Save for undo.  Do this once for all lines, much faster than doing this
    // for each line separately, especially when undoing.
    ఒకవేళ (u_savecommon(ప్రారంభం_lnum - 1, ప్రారంభం_lnum + oap->పంక్తి_లెక్క,
				     ప్రారంభం_lnum + oap->పంక్తి_లెక్క, FALSE) == OK)
	కొరకు (i = oap->పంక్తి_లెక్క; --i >= 0 && !got_int; )
	{
	    // it's a slow thing to do, so give feedback so there's no worry
	    // that the computer's just hung.

	    ఒకవేళ (i > 1
		    && (i % 50 == 0 || i == oap->పంక్తి_లెక్క - 1)
		    && oap->పంక్తి_లెక్క > p_report)
		smsg(_("%ld lines to indent... "), i);

	    // Be vi-compatible: For lisp indenting the first line is not
	    // indented, unless there is only one line.
	    ఒకవేళ (i != oap->పంక్తి_లెక్క - 1 || oap->పంక్తి_లెక్క == 1
						     || how != get_lisp_indent)
	    {
		l = skipwhite(ml_get_curline());
		ఒకవేళ (*l == NUL)		    // empty or blank line
		    amount = 0;
		లేకపోతే
		    amount = how();	    // get the indent for this line

		ఒకవేళ (amount >= 0 && సమితి_indent(amount, 0))
		{
		    // did change the indent, call changed_lines() later
		    ఒకవేళ (మొదటి_మారింది == 0)
			మొదటి_మారింది = curwin->w_cursor.lnum;
		    చివరి_మారింది = curwin->w_cursor.lnum;
		}
	    }
	    ++curwin->w_cursor.lnum;
	    curwin->w_cursor.నిలువు = 0;  // make sure it's valid
	}

    // put cursor on first non-blank of indented line
    curwin->w_cursor.lnum = ప్రారంభం_lnum;
    beginline(BL_SOL | BL_FIX);

    // Mark changed lines so that they will be redrawn.  When Visual
    // highlighting was present, need to continue until the last line.  When
    // there is no change still need to remove the Visual highlighting.
    ఒకవేళ (చివరి_మారింది != 0)
	మారింది_పంక్తులు(మొదటి_మారింది, 0,
		oap->is_VIsual ? ప్రారంభం_lnum + oap->పంక్తి_లెక్క :
		చివరి_మారింది + 1, 0L);
    లేకపోతే ఒకవేళ (oap->is_VIsual)
	redraw_curbuf_later(UPD_INVERTED);

    ఒకవేళ (oap->పంక్తి_లెక్క > p_report)
    {
	i = oap->పంక్తి_లెక్క - (i + 1);
	smsg(NGETTEXT("%ld line indented ",
						 "%ld lines indented ", i), i);
    }
    ఒకవేళ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// set '[ and '] marks
	curbuf->b_op_ప్రారంభం = oap->ప్రారంభం;
	curbuf->b_op_చివర = oap->చివర;
    }
}

/*
 * Return TRUE if lines starting with '#' should be left aligned.
 */
    పూర్ణాంకం
preprocs_ఎడమ(శూన్య)
{
    తిరిగి
	(curbuf->b_p_si && !curbuf->b_p_cin) ||
	(curbuf->b_p_cin && లోపల_cinkeys('#', ' ', TRUE)
					   && curbuf->b_ind_hash_comment == 0)
	;
}

/*
 * Return TRUE if the conditions are OK for smart indenting.
 */
    పూర్ణాంకం
may_do_si(శూన్య)
{
    తిరిగి curbuf->b_p_si
	&& !curbuf->b_p_cin
#నిర్వచితమైతే FEAT_EVAL
	&& *curbuf->b_p_inde == NUL
#అయితే_ముగింపు
	&& !p_paste;
}

/*
 * Try to do some very smart auto-indenting.
 * Used when inserting a "normal" character.
 */
    శూన్య
ins_try_si(పూర్ణాంకం c)
{
    pos_T	*pos, old_pos;
    char_u	*ptr;
    పూర్ణాంకం		i;
    పూర్ణాంకం		తాత్కాలిక;

    // do some very smart indenting when entering '{' or '}'
    ఒకవేళ (((did_si || can_si_వెనుక) && c == '{')
	    || (can_si && c == '}' && inindent(0)))
    {
	// for '}' set indent equal to indent of line containing matching '{'
	ఒకవేళ (c == '}' && (pos = findmatch(ఖాళీ, '{')) != ఖాళీ)
	{
	    old_pos = curwin->w_cursor;
	    // If the matching '{' has a ')' immediately before it (ignoring
	    // white-space), then line up with the start of the line
	    // containing the matching '(' if there is one.  This handles the
	    // case where an "if (..\n..) {" statement continues over multiple
	    // lines -- webb
	    ptr = ml_get(pos->lnum);
	    i = pos->నిలువు;
	    ఒకవేళ (i > 0)		// skip blanks before '{'
		అయితే (--i > 0 && VIM_ISWHITE(ptr[i]))
		    ;
	    curwin->w_cursor.lnum = pos->lnum;
	    curwin->w_cursor.నిలువు = i;
	    ఒకవేళ (ptr[i] == ')' && (pos = findmatch(ఖాళీ, '(')) != ఖాళీ)
		curwin->w_cursor = *pos;
	    i = get_indent();
	    curwin->w_cursor = old_pos;
	    ఒకవేళ (State & VREPLACE_FLAG)
		change_indent(INDENT_సమితి, i, FALSE, NUL, TRUE);
	    లేకపోతే
		(శూన్య)సమితి_indent(i, SIN_మారింది);
	}
	లేకపోతే ఒకవేళ (curwin->w_cursor.నిలువు > 0)
	{
	    // when inserting '{' after "O" reduce indent, but not
	    // more than indent of previous line
	    తాత్కాలిక = TRUE;
	    ఒకవేళ (c == '{' && can_si_వెనుక && curwin->w_cursor.lnum > 1)
	    {
		old_pos = curwin->w_cursor;
		i = get_indent();
		అయితే (curwin->w_cursor.lnum > 1)
		{
		    ptr = skipwhite(ml_get(--(curwin->w_cursor.lnum)));

		    // ignore empty lines and lines starting with '#'.
		    ఒకవేళ (*ptr != '#' && *ptr != NUL)
			ఆపు;
		}
		ఒకవేళ (get_indent() >= i)
		    తాత్కాలిక = FALSE;
		curwin->w_cursor = old_pos;
	    }
	    ఒకవేళ (తాత్కాలిక)
		జరుపు_పంక్తి(TRUE, FALSE, 1, TRUE);
	}
    }

    // set indent of '#' always to 0
    ఒకవేళ (curwin->w_cursor.నిలువు > 0 && can_si && c == '#' && inindent(0))
    {
	// remember current indent for next line
	old_indent = get_indent();
	(శూన్య)సమితి_indent(0, SIN_మారింది);
    }

    // Adjust ai_col, the char at this position can be deleted.
    ఒకవేళ (ai_నిలువు > curwin->w_cursor.నిలువు)
	ai_నిలువు = curwin->w_cursor.నిలువు;
}

/*
 * Insert an indent (for <Tab> or CTRL-T) or delete an indent (for CTRL-D).
 * Keep the cursor on the same character.
 * type == INDENT_INC	increase indent (for CTRL-T or <Tab>)
 * type == INDENT_DEC	decrease indent (for CTRL-D)
 * type == INDENT_SET	set indent to "amount"
 * if round is TRUE, round the indent to 'shiftwidth' (only with _INC and _Dec).
 */
    శూన్య
change_indent(
    పూర్ణాంకం		type,
    పూర్ణాంకం		amount,
    పూర్ణాంకం		విడత,
    పూర్ణాంకం		replaced,	// replaced character, put on replace stack
    పూర్ణాంకం		call_మారింది_బైట్లు)	// call changed_bytes()
{
    పూర్ణాంకం		vcol;
    పూర్ణాంకం		చివరి_vcol;
    పూర్ణాంకం		insstart_less;		// reduction for Insstart.col
    పూర్ణాంకం		new_cursor_నిలువు;
    పూర్ణాంకం		i;
    char_u	*ptr;
    పూర్ణాంకం		save_p_list;
    పూర్ణాంకం		ప్రారంభం_నిలువు;
    colnr_T	vc;
    colnr_T	orig_నిలువు = 0;		// init for GCC
    char_u	*new_పంక్తి, *orig_పంక్తి = ఖాళీ;	// init for GCC

    // MODE_VREPLACE state needs to know what the line was like before changing
    ఒకవేళ (State & VREPLACE_FLAG)
    {
	orig_పంక్తి = vim_strnsave(ml_get_curline(), ml_get_curline_len());  // Deal with NULL below
	orig_నిలువు = curwin->w_cursor.నిలువు;
    }

    // for the following tricks we don't want list mode
    save_p_list = curwin->w_p_list;
    curwin->w_p_list = FALSE;
#నిర్వచితమైతే FEAT_PROP_POPUP
    ignore_పాఠ్యం_props = TRUE;
#అయితే_ముగింపు
    vc = getvcol_nolist(&curwin->w_cursor);
    vcol = vc;

    // For Replace mode we need to fix the replace stack later, which is only
    // possible when the cursor is in the indent.  Remember the number of
    // characters before the cursor if it's possible.
    ప్రారంభం_నిలువు = curwin->w_cursor.నిలువు;

    // determine offset from first non-blank
    new_cursor_నిలువు = curwin->w_cursor.నిలువు;
    beginline(BL_WHITE);
    new_cursor_నిలువు -= curwin->w_cursor.నిలువు;

    insstart_less = curwin->w_cursor.నిలువు;

    // If the cursor is in the indent, compute how many screen columns the
    // cursor is to the left of the first non-blank.
    ఒకవేళ (new_cursor_నిలువు < 0)
	vcol = get_indent() - vcol;

    ఒకవేళ (new_cursor_నిలువు > 0)	    // can't fix replace stack
	ప్రారంభం_నిలువు = -1;

    // Set the new indent.  The cursor will be put on the first non-blank.
    ఒకవేళ (type == INDENT_సమితి)
	(శూన్య)సమితి_indent(amount, call_మారింది_బైట్లు ? SIN_మారింది : 0);
    లేకపోతే
    {
	పూర్ణాంకం	save_State = State;

	// Avoid being called recursively.
	ఒకవేళ (State & VREPLACE_FLAG)
	    State = బహుళకం_చొప్పించు;
	జరుపు_పంక్తి(type == INDENT_DEC, విడత, 1, call_మారింది_బైట్లు);
	State = save_State;
    }
    insstart_less -= curwin->w_cursor.నిలువు;

    // Try to put cursor on same character.
    // If the cursor is at or after the first non-blank in the line,
    // compute the cursor column relative to the column of the first
    // non-blank character.
    // If we are not in insert mode, leave the cursor on the first non-blank.
    // If the cursor is before the first non-blank, position it relative
    // to the first non-blank, counted in screen columns.
    ఒకవేళ (new_cursor_నిలువు >= 0)
    {
	// When changing the indent while the cursor is touching it, reset
	// Insstart_col to 0.
	ఒకవేళ (new_cursor_నిలువు == 0)
	    insstart_less = MAXCOL;
	new_cursor_నిలువు += curwin->w_cursor.నిలువు;
    }
    లేకపోతే ఒకవేళ (!(State & బహుళకం_చొప్పించు))
	new_cursor_నిలువు = curwin->w_cursor.నిలువు;
    లేకపోతే
    {
	chartabsize_T cts;

	// Compute the screen column where the cursor should be.
	vcol = get_indent() - vcol;
	curwin->w_virtcol = (colnr_T)((vcol < 0) ? 0 : vcol);

	// Advance the cursor until we reach the right screen column.
	చివరి_vcol = 0;
	ptr = ml_get_curline();
	init_chartabsize_arg(&cts, curwin, 0, 0, ptr, ptr);
	అయితే (cts.cts_vcol <= (పూర్ణాంకం)curwin->w_virtcol)
	{
	    చివరి_vcol = cts.cts_vcol;
	    ఒకవేళ (cts.cts_vcol > 0)
		MB_PTR_ADV(cts.cts_ptr);
	    ఒకవేళ (*cts.cts_ptr == NUL)
		ఆపు;
	    cts.cts_vcol += lbr_chartabsize(&cts);
	}
	vcol = చివరి_vcol;
	new_cursor_నిలువు = cts.cts_ptr - cts.cts_పంక్తి;
	clear_chartabsize_arg(&cts);

	// May need to insert spaces to be able to position the cursor on
	// the right screen column.
	ఒకవేళ (vcol != (పూర్ణాంకం)curwin->w_virtcol)
	{
	    curwin->w_cursor.నిలువు = (colnr_T)new_cursor_నిలువు;
	    i = (పూర్ణాంకం)curwin->w_virtcol - vcol;
	    ptr = alloc(i + 1);
	    ఒకవేళ (ptr != ఖాళీ)
	    {
		size_t ptrlen;
		new_cursor_నిలువు += i;
		ptr[i] = NUL;
		ptrlen = i;
		అయితే (--i >= 0)
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

    ఒకవేళ (new_cursor_నిలువు <= 0)
	curwin->w_cursor.నిలువు = 0;
    లేకపోతే
	curwin->w_cursor.నిలువు = (colnr_T)new_cursor_నిలువు;
    curwin->w_సమితి_curswant = true;
    మారింది_cline_bef_curs();

    // May have to adjust the start of the insert.
    ఒకవేళ (State & బహుళకం_చొప్పించు)
    {
	ఒకవేళ (curwin->w_cursor.lnum == Insstart.lnum && Insstart.నిలువు != 0)
	{
	    ఒకవేళ ((పూర్ణాంకం)Insstart.నిలువు <= insstart_less)
		Insstart.నిలువు = 0;
	    లేకపోతే
		Insstart.నిలువు -= insstart_less;
	}
	ఒకవేళ ((పూర్ణాంకం)ai_నిలువు <= insstart_less)
	    ai_నిలువు = 0;
	లేకపోతే
	    ai_నిలువు -= insstart_less;
    }

    // For MODE_REPLACE state, may have to fix the replace stack, if it's
    // possible.  If the number of characters before the cursor decreased, need
    // to pop a few characters from the replace stack.
    // If the number of characters before the cursor increased, need to push a
    // few NULs onto the replace stack.
    ఒకవేళ (REPLACE_సాధారణ(State) && ప్రారంభం_నిలువు >= 0)
    {
	అయితే (ప్రారంభం_నిలువు > (పూర్ణాంకం)curwin->w_cursor.నిలువు)
	{
	    replace_join(0);	    // remove a NUL from the replace stack
	    --ప్రారంభం_నిలువు;
	}
	అయితే (ప్రారంభం_నిలువు < (పూర్ణాంకం)curwin->w_cursor.నిలువు || replaced)
	{
	    replace_నెట్టు(NUL);
	    ఒకవేళ (replaced)
	    {
		replace_నెట్టు(replaced);
		replaced = NUL;
	    }
	    ++ప్రారంభం_నిలువు;
	}
    }
#నిర్వచితమైతే FEAT_PROP_POPUP
    ignore_పాఠ్యం_props = FALSE;
#అయితే_ముగింపు

    // For MODE_VREPLACE state, we also have to fix the replace stack.  In this
    // case it is always possible because we backspace over the whole line and
    // then put it back again the way we wanted it.
    ఒకవేళ (State & VREPLACE_FLAG)
    {
	// If orig_line didn't allocate, just return.  At least we did the job,
	// even if you can't backspace.
	ఒకవేళ (orig_పంక్తి == ఖాళీ)
	    తిరిగి;

	// Save new line
	new_పంక్తి = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	ఒకవేళ (new_పంక్తి == ఖాళీ)
	{
	    vim_free(orig_పంక్తి);
	    తిరిగి;
	}

	// We only put back the new line up to the cursor
	new_పంక్తి[curwin->w_cursor.నిలువు] = NUL;

	// Put back original line
	ml_replace(curwin->w_cursor.lnum, orig_పంక్తి, FALSE);
	curwin->w_cursor.నిలువు = orig_నిలువు;

	// Backspace from cursor to start of line
	backspace_until_స్తంభం(0);

	// Insert new stuff into line again
	ins_బైట్లు(new_పంక్తి);

	vim_free(new_పంక్తి);
    }
}

/*
 * Copy the indent from ptr to the current line (and fill to size)
 * Leaves the cursor on the first non-blank in the line.
 * Returns TRUE if the line was changed.
 */
    పూర్ణాంకం
copy_indent(పూర్ణాంకం కొలత, char_u *src)
{
    char_u	*p = ఖాళీ;
    char_u	*పంక్తి = ఖాళీ;
    char_u	*s;
    పూర్ణాంకం		todo;
    పూర్ణాంకం		ind_len;
    పూర్ణాంకం		పంక్తి_len = 0;
    పూర్ణాంకం		tab_pad;
    పూర్ణాంకం		ind_పూర్తి;
    పూర్ణాంకం		విడత;
#నిర్వచితమైతే FEAT_VARTABS
    పూర్ణాంకం		ind_నిలువు;
#అయితే_ముగింపు

    // Round 1: compute the number of characters needed for the indent
    // Round 2: copy the characters.
    కొరకు (విడత = 1; విడత <= 2; ++విడత)
    {
	todo = కొలత;
	ind_len = 0;
	ind_పూర్తి = 0;
#నిర్వచితమైతే FEAT_VARTABS
	ind_నిలువు = 0;
#అయితే_ముగింపు
	s = src;

	// Count/copy the usable portion of the source line
	అయితే (todo > 0 && VIM_ISWHITE(*s))
	{
	    ఒకవేళ (*s == TAB)
	    {
#నిర్వచితమైతే FEAT_VARTABS
		tab_pad = tabstop_padding(ind_పూర్తి, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#లేకపోతే
		tab_pad = (పూర్ణాంకం)curbuf->b_p_ts
					   - (ind_పూర్తి % (పూర్ణాంకం)curbuf->b_p_ts);
#అయితే_ముగింపు
		// Stop if this tab will overshoot the target
		ఒకవేళ (todo < tab_pad)
		    ఆపు;
		todo -= tab_pad;
		ind_పూర్తి += tab_pad;
#నిర్వచితమైతే FEAT_VARTABS
		ind_నిలువు += tab_pad;
#అయితే_ముగింపు
	    }
	    లేకపోతే
	    {
		--todo;
		++ind_పూర్తి;
#నిర్వచితమైతే FEAT_VARTABS
		++ind_నిలువు;
#అయితే_ముగింపు
	    }
	    ++ind_len;
	    ఒకవేళ (p != ఖాళీ)
		*p++ = *s;
	    ++s;
	}

	// Fill to next tabstop with a tab, if possible
#నిర్వచితమైతే FEAT_VARTABS
	tab_pad = tabstop_padding(ind_పూర్తి, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#లేకపోతే
	tab_pad = (పూర్ణాంకం)curbuf->b_p_ts - (ind_పూర్తి % (పూర్ణాంకం)curbuf->b_p_ts);
#అయితే_ముగింపు
	ఒకవేళ (todo >= tab_pad && !curbuf->b_p_et)
	{
	    todo -= tab_pad;
	    ++ind_len;
#నిర్వచితమైతే FEAT_VARTABS
	    ind_నిలువు += tab_pad;
#అయితే_ముగింపు
	    ఒకవేళ (p != ఖాళీ)
		*p++ = TAB;
	}

	// Add tabs required for indent
	ఒకవేళ (!curbuf->b_p_et)
	{
#నిర్వచితమైతే FEAT_VARTABS
	    కొరకు (;;)
	    {
		tab_pad = tabstop_padding(ind_నిలువు, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
		ఒకవేళ (todo < tab_pad)
		    ఆపు;
		todo -= tab_pad;
		++ind_len;
		ind_నిలువు += tab_pad;
		ఒకవేళ (p != ఖాళీ)
		    *p++ = TAB;
	    }
#లేకపోతే
	    అయితే (todo >= (పూర్ణాంకం)curbuf->b_p_ts)
	    {
		todo -= (పూర్ణాంకం)curbuf->b_p_ts;
		++ind_len;
		ఒకవేళ (p != ఖాళీ)
		    *p++ = TAB;
	    }
#అయితే_ముగింపు
	}

	// Count/add spaces required for indent
	అయితే (todo > 0)
	{
	    --todo;
	    ++ind_len;
	    ఒకవేళ (p != ఖాళీ)
		*p++ = ' ';
	}

	ఒకవేళ (p == ఖాళీ)
	{
	    // Allocate memory for the result: the copied indent, new indent
	    // and the rest of the line.
	    పంక్తి_len = ml_get_curline_len() + 1;
	    పంక్తి = alloc(ind_len + పంక్తి_len);
	    ఒకవేళ (పంక్తి == ఖాళీ)
		తిరిగి FALSE;
	    p = పంక్తి;
	}
    }

    // Append the original line
    mch_memmove(p, ml_get_curline(), (size_t)పంక్తి_len);

    // Replace the line
    ml_replace(curwin->w_cursor.lnum, పంక్తి, FALSE);

    // Put the cursor after the indent.
    curwin->w_cursor.నిలువు = ind_len;
    తిరిగి TRUE;
}

/*
 * Give a "resulting text too long" error and maybe set got_int.
 */
    స్థిర శూన్య
emsg_పాఠ్యం_too_long(శూన్య)
{
    emsg(_(e_resulting_పాఠ్యం_too_long));
#నిర్వచితమైతే FEAT_EVAL
    // when not inside a try/catch set got_int to break out of any loop
    ఒకవేళ (trylevel == 0)
#అయితే_ముగింపు
	got_int = TRUE;
}

/*
 * ":retab".
 */
    శూన్య
ex_retab(exarg_T *eap)
{
    linenr_T	lnum;
    పూర్ణాంకం		got_tab = FALSE;
    దీర్ఘ	అంకె_spaces = 0;
    దీర్ఘ	అంకె_tabs;
    దీర్ఘ	len;
    దీర్ఘ	నిలువు;
    దీర్ఘ	vcol;
    దీర్ఘ	ప్రారంభం_నిలువు = 0;		// For start of white-space string
    దీర్ఘ	ప్రారంభం_vcol = 0;		// For start of white-space string
    దీర్ఘ	old_len;
    దీర్ఘ	new_len;
    char_u	*ptr;
    char_u	*new_పంక్తి = (char_u *)1; // init to non-NULL
    పూర్ణాంకం		did_undo;		// called u_save for current line
#నిర్వచితమైతే FEAT_VARTABS
    పూర్ణాంకం		*new_vts_array = ఖాళీ;
    char_u	*new_ts_str;		// string value of tab argument
#లేకపోతే
    పూర్ణాంకం		తాత్కాలిక;
    పూర్ణాంకం		new_ts = 0;
#అయితే_ముగింపు
    పూర్ణాంకం		save_list;
    linenr_T	మొదటి_పంక్తి = 0;		// first changed line
    linenr_T	చివరి_పంక్తి = 0;		// last changed line
    పూర్ణాంకం		is_indent_only = 0;	// Only process leading whitespace

    save_list = curwin->w_p_list;
    curwin->w_p_list = 0;	    // don't want list mode here

    ptr = eap->arg;
    ఒకవేళ (STRNCMP(ptr, "-indentonly", 11) == 0 && IS_WHITE_OR_NUL(ptr[11]))
    {
	is_indent_only = 1;
	ptr = skipwhite(ptr + 11);
    }

#నిర్వచితమైతే FEAT_VARTABS
    new_ts_str = ptr;
    ఒకవేళ (tabstop_సమితి(ptr, &new_vts_array) == FAIL)
	తిరిగి;
    అయితే (vim_isdigit(*ptr) || *ptr == ',')
	++ptr;

    // This ensures that either new_vts_array and new_ts_str are freshly
    // allocated, or new_vts_array points to an existing array and new_ts_str
    // is null.
    ఒకవేళ (new_vts_array == ఖాళీ)
    {
	new_vts_array = curbuf->b_p_vts_array;
	new_ts_str = ఖాళీ;
    }
    లేకపోతే
	new_ts_str = vim_strnsave(new_ts_str, ptr - new_ts_str);
#లేకపోతే
    ఒకవేళ (ptr[0] != NUL && (ptr[0] != '0' || ptr[1] != NUL))
    {
	char_u	*చివర;

	ఒకవేళ (strtol((అక్షరం *)ptr, (అక్షరం **)&చివర, 10) <= 0)
	{
	    ఒకవేళ (ptr != చివర)
		emsg(_(e_వాదం_must_be_ధనాత్మక));
	    లేకపోతే
		semsg(_(e_invalid_వాదం_str), ptr);
	    తిరిగి;
	}
	new_ts = getdigits(&ptr);
	ఒకవేళ (new_ts < 0 || new_ts > TABSTOP_MAX)
	{
	    semsg(_(e_invalid_వాదం_str), eap->arg);
	    తిరిగి;
	}
    }
    ఒకవేళ (new_ts == 0)
	new_ts = curbuf->b_p_ts;
#అయితే_ముగింపు
    కొరకు (lnum = eap->line1; !got_int && lnum <= eap->line2; ++lnum)
    {
	ptr = ml_get(lnum);
	old_len = ml_get_len(lnum);
	నిలువు = 0;
	vcol = 0;
	did_undo = FALSE;
	కొరకు (;;)
	{
	    ఒకవేళ (VIM_ISWHITE(ptr[నిలువు]))
	    {
		ఒకవేళ (!got_tab && అంకె_spaces == 0)
		{
		    // First consecutive white-space
		    ప్రారంభం_vcol = vcol;
		    ప్రారంభం_నిలువు = నిలువు;
		}
		ఒకవేళ (ptr[నిలువు] == ' ')
		    అంకె_spaces++;
		లేకపోతే
		    got_tab = TRUE;
	    }
	    లేకపోతే
	    {
		ఒకవేళ (got_tab || (eap->forceit && అంకె_spaces > 1))
		{
		    // Retabulate this string of white-space

		    // len is virtual length of white string
		    len = అంకె_spaces = vcol - ప్రారంభం_vcol;
		    అంకె_tabs = 0;
		    ఒకవేళ (!curbuf->b_p_et)
		    {
#నిర్వచితమైతే FEAT_VARTABS
			పూర్ణాంకం t, s;

			tabstop_fromto(ప్రారంభం_vcol, vcol,
					curbuf->b_p_ts, new_vts_array, &t, &s);
			అంకె_tabs = t;
			అంకె_spaces = s;
#లేకపోతే
			తాత్కాలిక = new_ts - (ప్రారంభం_vcol % new_ts);
			ఒకవేళ (అంకె_spaces >= తాత్కాలిక)
			{
			    అంకె_spaces -= తాత్కాలిక;
			    అంకె_tabs++;
			}
			అంకె_tabs += అంకె_spaces / new_ts;
			అంకె_spaces -= (అంకె_spaces / new_ts) * new_ts;
#అయితే_ముగింపు
		    }
		    ఒకవేళ (curbuf->b_p_et || got_tab ||
					(అంకె_spaces + అంకె_tabs < len))
		    {
			ఒకవేళ (did_undo == FALSE)
			{
			    did_undo = TRUE;
			    ఒకవేళ (u_save((linenr_T)(lnum - 1),
						(linenr_T)(lnum + 1)) == FAIL)
			    {
				new_పంక్తి = ఖాళీ;	// flag out-of-memory
				ఆపు;
			    }
			}

			// len is actual number of white characters used
			len = అంకె_spaces + అంకె_tabs;
			new_len = old_len - నిలువు + ప్రారంభం_నిలువు + len + 1;
			ఒకవేళ (new_len <= 0 || new_len >= MAXCOL)
			{
			    emsg_పాఠ్యం_too_long();
			    ఆపు;
			}
			new_పంక్తి = alloc(new_len);
			ఒకవేళ (new_పంక్తి == ఖాళీ)
			    ఆపు;
			ఒకవేళ (ప్రారంభం_నిలువు > 0)
			    mch_memmove(new_పంక్తి, ptr, (size_t)ప్రారంభం_నిలువు);
			mch_memmove(new_పంక్తి + ప్రారంభం_నిలువు + len,
				      ptr + నిలువు, (size_t)(old_len - నిలువు + 1));
			ptr = new_పంక్తి + ప్రారంభం_నిలువు;
			కొరకు (నిలువు = 0; నిలువు < len; నిలువు++)
			    ptr[నిలువు] = (నిలువు < అంకె_tabs) ? '\t' : ' ';
			ఒకవేళ (ml_replace(lnum, new_పంక్తి, FALSE) == OK)
			    // "new_line" may have been copied
			    new_పంక్తి = curbuf->b_ml.ml_పంక్తి_ptr;
			ఒకవేళ (మొదటి_పంక్తి == 0)
			    మొదటి_పంక్తి = lnum;
			చివరి_పంక్తి = lnum;
			ptr = new_పంక్తి;
			old_len = new_len - 1;
			నిలువు = ప్రారంభం_నిలువు + len;
		    }
		}
		got_tab = FALSE;
		అంకె_spaces = 0;

		ఒకవేళ (is_indent_only)
		    ఆపు;
	    }
	    ఒకవేళ (ptr[నిలువు] == NUL)
		ఆపు;
	    vcol += chartabsize(ptr + నిలువు, (colnr_T)vcol);
	    ఒకవేళ (vcol >= MAXCOL)
	    {
		emsg_పాఠ్యం_too_long();
		ఆపు;
	    }
	    ఒకవేళ (has_mbyte)
		నిలువు += (*mb_ptr2len)(ptr + నిలువు);
	    లేకపోతే
		++నిలువు;
	}
	ఒకవేళ (new_పంక్తి == ఖాళీ)		    // out of memory
	    ఆపు;
	పంక్తి_breakcheck();
    }
    ఒకవేళ (got_int)
	emsg(_(e_interrupted));

#నిర్వచితమైతే FEAT_VARTABS
    // If a single value was given then it can be considered equal to
    // either the value of 'tabstop' or the value of 'vartabstop'.
    ఒకవేళ (tabstop_లెక్క(curbuf->b_p_vts_array) == 0
	&& tabstop_లెక్క(new_vts_array) == 1
	&& curbuf->b_p_ts == tabstop_మొదటి(new_vts_array))
	; // not changed
    లేకపోతే ఒకవేళ (tabstop_లెక్క(curbuf->b_p_vts_array) > 0
	&& tabstop_eq(curbuf->b_p_vts_array, new_vts_array))
	; // not changed
    లేకపోతే
	redraw_curbuf_later(UPD_NOT_VALID);
#లేకపోతే
    ఒకవేళ (curbuf->b_p_ts != new_ts)
	redraw_curbuf_later(UPD_NOT_VALID);
#అయితే_ముగింపు
    ఒకవేళ (మొదటి_పంక్తి != 0)
	మారింది_పంక్తులు(మొదటి_పంక్తి, 0, చివరి_పంక్తి + 1, 0L);

    curwin->w_p_list = save_list;	// restore 'list'

#నిర్వచితమైతే FEAT_VARTABS
    ఒకవేళ (new_ts_str != ఖాళీ)		// set the new tabstop
    {
	// If 'vartabstop' is in use or if the value given to retab has more
	// than one tabstop then update 'vartabstop'.
	పూర్ణాంకం *old_vts_ary = curbuf->b_p_vts_array;

	ఒకవేళ (tabstop_లెక్క(old_vts_ary) > 0 || tabstop_లెక్క(new_vts_array) > 1)
	{
	    సమితి_string_option_ప్రత్యక్ష((char_u *)"vts", -1, new_ts_str,
							OPT_FREE|OPT_LOCAL, 0);
	    curbuf->b_p_vts_array = new_vts_array;
	    vim_free(old_vts_ary);
	}
	లేకపోతే
	{
	    // 'vartabstop' wasn't in use and a single value was given to
	    // retab then update 'tabstop'.
	    curbuf->b_p_ts = tabstop_మొదటి(new_vts_array);
	    vim_free(new_vts_array);
	}
	vim_free(new_ts_str);
    }
#లేకపోతే
    curbuf->b_p_ts = new_ts;
#అయితే_ముగింపు
    coladvance(curwin->w_curswant);

    u_clearline();
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Get indent level from 'indentexpr'.
 */
    పూర్ణాంకం
get_సమాసం_indent(శూన్య)
{
    పూర్ణాంకం		indent = -1;
    char_u	*inde_copy;
    pos_T	save_pos;
    colnr_T	save_curswant;
    పూర్ణాంకం		save_సమితి_curswant;
    పూర్ణాంకం		save_State;
    పూర్ణాంకం		use_sandbox = was_సమితి_insecurely(curwin,
					    (char_u *)"indentexpr", OPT_LOCAL);
    sctx_T	save_sctx = current_sctx;

    // Save and restore cursor position and curswant, in case it was changed
    // via :normal commands
    save_pos = curwin->w_cursor;
    save_curswant = curwin->w_curswant;
    save_సమితి_curswant = curwin->w_సమితి_curswant;
    సమితి_vim_var_nr(VV_LNUM, curwin->w_cursor.lnum);
    ఒకవేళ (use_sandbox)
	++sandbox;
    ++textlock;
    current_sctx = curbuf->b_p_script_ctx[BV_INDE];

    // Need to make a copy, the 'indentexpr' option could be changed while
    // evaluating it.
    inde_copy = vim_strsave(curbuf->b_p_inde);
    ఒకవేళ (inde_copy != ఖాళీ)
    {
	indent = (పూర్ణాంకం)eval_గమ్యం_సంఖ్య(inde_copy, TRUE);
	vim_free(inde_copy);
    }

    ఒకవేళ (use_sandbox)
	--sandbox;
    --textlock;
    current_sctx = save_sctx;

    // Restore the cursor position so that 'indentexpr' doesn't need to.
    // Pretend to be in Insert mode, allow cursor past end of line for "o"
    // command.
    save_State = State;
    State = బహుళకం_చొప్పించు;
    curwin->w_cursor = save_pos;
    curwin->w_curswant = save_curswant;
    curwin->w_సమితి_curswant = save_సమితి_curswant;
    check_cursor();
    State = save_State;

    // Reset did_throw, unless 'debug' has "throw" and inside a try/catch.
    ఒకవేళ (did_throw && (vim_strchr(p_debug, 't') == ఖాళీ || trylevel == 0))
    {
	handle_did_throw();
	did_throw = FALSE;
    }

    // If there is an error, just keep the current indent.
    ఒకవేళ (indent < 0)
	indent = get_indent();

    తిరిగి indent;
}
#అయితే_ముగింపు

    స్థిర పూర్ణాంకం
lisp_సరిపోలిక(char_u *p)
{
    char_u	buf[LSIZE];
    పూర్ణాంకం		len;
    char_u	*మాట = *curbuf->b_p_lw != NUL ? curbuf->b_p_lw : p_lispwords;

    అయితే (*మాట != NUL)
    {
	len = copy_option_part(&మాట, buf, LSIZE, ",");
	ఒకవేళ (STRNCMP(buf, p, len) == 0 && IS_WHITE_OR_NUL(p[len]))
	    తిరిగి TRUE;
    }
    తిరిగి FALSE;
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
    పూర్ణాంకం
get_lisp_indent(శూన్య)
{
    pos_T	*pos, realpos, paren;
    పూర్ణాంకం		amount;
    char_u	*that;
    colnr_T	నిలువు;
    colnr_T	firsttry;
    పూర్ణాంకం		parencount, quotecount;
    పూర్ణాంకం		vi_lisp;

    // Set vi_lisp to use the vi-compatible method
    vi_lisp = (vim_strchr(p_cpo, CPO_LISP) != ఖాళీ);

    realpos = curwin->w_cursor;
    curwin->w_cursor.నిలువు = 0;

    ఒకవేళ ((pos = findmatch(ఖాళీ, '(')) == ఖాళీ)
	pos = findmatch(ఖాళీ, '[');
    లేకపోతే
    {
	paren = *pos;
	pos = findmatch(ఖాళీ, '[');
	ఒకవేళ (pos == ఖాళీ || LT_POSP(pos, &paren))
	    pos = &paren;
    }
    ఒకవేళ (pos != ఖాళీ)
    {
	// Extra trick: Take the indent of the first previous non-white
	// line that is at the same () level.
	amount = -1;
	parencount = 0;

	అయితే (--curwin->w_cursor.lnum >= pos->lnum)
	{
	    ఒకవేళ (linewhite(curwin->w_cursor.lnum))
		కొనసాగు;
	    కొరకు (that = ml_get_curline(); *that != NUL; ++that)
	    {
		ఒకవేళ (*that == ';')
		{
		    అయితే (*(that + 1) != NUL)
			++that;
		    కొనసాగు;
		}
		ఒకవేళ (*that == '\\')
		{
		    ఒకవేళ (*(that + 1) != NUL)
			++that;
		    కొనసాగు;
		}
		ఒకవేళ (*that == '"' && *(that + 1) != NUL)
		{
		    అయితే (*++that && *that != '"')
		    {
			// skipping escaped characters in the string
			ఒకవేళ (*that == '\\')
			{
			    ఒకవేళ (*++that == NUL)
				ఆపు;
			    ఒకవేళ (that[1] == NUL)
			    {
				++that;
				ఆపు;
			    }
			}
		    }
		    ఒకవేళ (*that == NUL)
			ఆపు;
		}
		ఒకవేళ (*that == '(' || *that == '[')
		    ++parencount;
		లేకపోతే ఒకవేళ (*that == ')' || *that == ']')
		    --parencount;
	    }
	    ఒకవేళ (parencount == 0)
	    {
		amount = get_indent();
		ఆపు;
	    }
	}

	ఒకవేళ (amount == -1)
	{
	    curwin->w_cursor.lnum = pos->lnum;
	    curwin->w_cursor.నిలువు = pos->నిలువు;
	    నిలువు = pos->నిలువు;

	    that = ml_get_curline();

	    ఒకవేళ (vi_lisp && get_indent() == 0)
		amount = 2;
	    లేకపోతే
	    {
		char_u		*పంక్తి = that;
		chartabsize_T	cts;

		init_chartabsize_arg(&cts, curwin, pos->lnum, 0, పంక్తి, పంక్తి);
		అయితే (*cts.cts_ptr != NUL && నిలువు > 0)
		{
		    cts.cts_vcol += lbr_chartabsize_adv(&cts);
		    నిలువు--;
		}
		amount = cts.cts_vcol;
		that = cts.cts_ptr;
		clear_chartabsize_arg(&cts);

		// Some keywords require "body" indenting rules (the
		// non-standard-lisp ones are Scheme special forms):
		//
		// (let ((a 1))    instead    (let ((a 1))
		//   (...))	      of	   (...))

		ఒకవేళ (!vi_lisp && (*that == '(' || *that == '[')
						      && lisp_సరిపోలిక(that + 1))
		    amount += 2;
		లేకపోతే
		{
		    ఒకవేళ (*that != NUL)
		    {
			that++;
			amount++;
		    }
		    firsttry = amount;

		    init_chartabsize_arg(&cts, curwin, (colnr_T)(that - పంక్తి),
							   amount, పంక్తి, that);
		    అయితే (VIM_ISWHITE(*cts.cts_ptr))
		    {
			cts.cts_vcol += lbr_chartabsize(&cts);
			++cts.cts_ptr;
		    }
		    that = cts.cts_ptr;
		    amount = cts.cts_vcol;
		    clear_chartabsize_arg(&cts);

		    ఒకవేళ (*that && *that != ';') // not a comment line
		    {
			// test *that != '(' to accommodate first let/do
			// argument if it is more than one line
			ఒకవేళ (!vi_lisp && *that != '(' && *that != '[')
			    firsttry++;

			parencount = 0;
			quotecount = 0;

			init_chartabsize_arg(&cts, curwin,
				   (colnr_T)(that - పంక్తి), amount, పంక్తి, that);
			ఒకవేళ (vi_lisp
				|| (*that != '"'
				    && *that != '\''
				    && *that != '#'
				    && (*that < '0' || *that > '9')))
			{
			    అయితే (*cts.cts_ptr
				    && (!VIM_ISWHITE(*cts.cts_ptr)
					|| quotecount
					|| parencount)
				    && (!((*cts.cts_ptr == '('
							|| *cts.cts_ptr == '[')
					    && !quotecount
					    && !parencount
					    && vi_lisp)))
			    {
				ఒకవేళ (*cts.cts_ptr == '"')
				    quotecount = !quotecount;
				ఒకవేళ ((*cts.cts_ptr == '(' || *cts.cts_ptr == '[')
							       && !quotecount)
				    ++parencount;
				ఒకవేళ ((*cts.cts_ptr == ')' || *cts.cts_ptr == ']')
							       && !quotecount)
				    --parencount;
				ఒకవేళ (*cts.cts_ptr == '\\'
						    && *(cts.cts_ptr+1) != NUL)
				    cts.cts_vcol += lbr_chartabsize_adv(&cts);
				cts.cts_vcol += lbr_chartabsize_adv(&cts);
			    }
			}
			అయితే (VIM_ISWHITE(*cts.cts_ptr))
			{
			    cts.cts_vcol += lbr_chartabsize(&cts);
			    ++cts.cts_ptr;
			}
			that = cts.cts_ptr;
			amount = cts.cts_vcol;
			clear_chartabsize_arg(&cts);

			ఒకవేళ (!*that || *that == ';')
			    amount = firsttry;
		    }
		}
	    }
	}
    }
    లేకపోతే
	amount = 0;	// no matching '(' or '[' found, use zero indent

    curwin->w_cursor = realpos;

    తిరిగి amount;
}

/*
 * Re-indent the current line, based on the current contents of it and the
 * surrounding lines. Fixing the cursor position seems really easy -- I'm very
 * confused what all the part that handles Control-T is doing that I'm not.
 * "get_the_indent" should be get_c_indent, get_expr_indent or get_lisp_indent.
 */

    శూన్య
fixthisline(పూర్ణాంకం (*get_the_indent)(శూన్య))
{
    పూర్ణాంకం amount = get_the_indent();

    ఒకవేళ (amount < 0)
	తిరిగి;

    change_indent(INDENT_సమితి, amount, FALSE, 0, TRUE);
    ఒకవేళ (linewhite(curwin->w_cursor.lnum))
	did_ai = TRUE;	// delete the indent if the line stays empty
}

/*
 * Return TRUE if 'indentexpr' should be used for Lisp indenting.
 * Caller may want to check 'autoindent'.
 */
    పూర్ణాంకం
use_indentexpr_for_lisp(శూన్య)
{
#నిర్వచితమైతే FEAT_EVAL
    తిరిగి curbuf->b_p_lisp
		&& *curbuf->b_p_inde != NUL
		&& STRCMP(curbuf->b_p_lop, "expr:1") == 0;
#లేకపోతే
    తిరిగి FALSE;
#అయితే_ముగింపు
}

/*
 * Fix indent for 'lisp' and 'cindent'.
 */
    శూన్య
fix_indent(శూన్య)
{
    ఒకవేళ (p_paste)
	తిరిగి;  // no auto-indenting when 'paste' is set
    ఒకవేళ (curbuf->b_p_lisp && curbuf->b_p_ai)
    {
	ఒకవేళ (use_indentexpr_for_lisp())
	    do_c_సమాసం_indent();
	లేకపోతే
	    fixthisline(get_lisp_indent);
    }
    లేకపోతే ఒకవేళ (cindent_on())
	do_c_సమాసం_indent();
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * "indent()" function
 */
    శూన్య
f_indent(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    ఒకవేళ (లోపల_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	తిరిగి;

    lnum = tv_get_lnum(argvars);
    ఒకవేళ (lnum >= 1 && lnum <= curbuf->b_ml.ml_పంక్తి_లెక్క)
	rettv->vval.v_సంఖ్య = get_indent_lnum(lnum);
    లేకపోతే
    {
	ఒకవేళ (లోపల_vim9script())
	    semsg(_(e_invalid_పంక్తి_సంఖ్య_nr), lnum);
	rettv->vval.v_సంఖ్య = -1;
    }
}

/*
 * "lispindent(lnum)" function
 */
    శూన్య
f_lispindent(typval_T *argvars వాడని, typval_T *rettv)
{
    pos_T	pos;
    linenr_T	lnum;

    ఒకవేళ (లోపల_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	తిరిగి;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    ఒకవేళ (lnum >= 1 && lnum <= curbuf->b_ml.ml_పంక్తి_లెక్క)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_సంఖ్య = get_lisp_indent();
	curwin->w_cursor = pos;
    }
    లేకపోతే ఒకవేళ (లోపల_vim9script())
	semsg(_(e_invalid_పంక్తి_సంఖ్య_nr), lnum);
    లేకపోతే
	rettv->vval.v_సంఖ్య = -1;
}
#అయితే_ముగింపు
