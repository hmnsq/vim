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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_VARTABS)

/*
 * Set the integer values corresponding to the string setting of 'vartabstop'.
 * "array" will be set, caller must free it if needed.
 * Return FAIL for an error.
 */
    പൂർണ്ണസംഖ്യ
tabstop_set(char_u *var, പൂർണ്ണസംഖ്യ **array)
{
    പൂർണ്ണസംഖ്യ	    valcount = 1;
    പൂർണ്ണസംഖ്യ	    t;
    char_u  *cp;

    എങ്കിൽ (var[0] == NUL || (var[0] == '0' && var[1] == NUL))
    {
	*array = ശൂന്യ;
	തിരികെ OK;
    }

    വേണ്ടി (cp = var; *cp != NUL; ++cp)
    {
	എങ്കിൽ (cp == var || cp[-1] == ',')
	{
	    char_u *end;

	    എങ്കിൽ (strtol((അക്ഷരം *)cp, (അക്ഷരം **)&end, 10) <= 0)
	    {
		എങ്കിൽ (cp != end)
		    emsg(_(e_argument_must_be_positive));
		അല്ലെങ്കിൽ
		    semsg(_(e_invalid_argument_str), cp);
		തിരികെ FAIL;
	    }
	}

	എങ്കിൽ (VIM_ISDIGIT(*cp))
	    തുടരുക;
	എങ്കിൽ (cp[0] == ',' && cp > var && cp[-1] != ',' && cp[1] != NUL)
	{
	    ++valcount;
	    തുടരുക;
	}
	semsg(_(e_invalid_argument_str), var);
	തിരികെ FAIL;
    }

    *array = ALLOC_MULT(പൂർണ്ണസംഖ്യ, valcount + 1);
    എങ്കിൽ (*array == ശൂന്യ)
	തിരികെ FAIL;
    (*array)[0] = valcount;

    t = 1;
    വേണ്ടി (cp = var; *cp != NUL;)
    {
	പൂർണ്ണസംഖ്യ n = atoi((അക്ഷരം *)cp);

	// Catch negative values, overflow and ridiculous big values.
	എങ്കിൽ (n <= 0 || n > TABSTOP_MAX)
	{
	    semsg(_(e_invalid_argument_str), cp);
	    VIM_CLEAR(*array);
	    തിരികെ FAIL;
	}
	(*array)[t++] = n;
	ആകുമ്പോൾ (*cp != NUL && *cp != ',')
	    ++cp;
	എങ്കിൽ (*cp != NUL)
	    ++cp;
    }

    തിരികെ OK;
}

/*
 * Calculate the number of screen spaces a tab will occupy.
 * If "vts" is set then the tab widths are taken from that array,
 * otherwise the value of ts is used.
 */
    പൂർണ്ണസംഖ്യ
tabstop_padding(colnr_T col, പൂർണ്ണസംഖ്യ ts_arg, പൂർണ്ണസംഖ്യ *vts)
{
    പൂർണ്ണസംഖ്യ		ts = ts_arg == 0 ? 8 : ts_arg;
    പൂർണ്ണസംഖ്യ		tabcount;
    colnr_T	tabcol = 0;
    പൂർണ്ണസംഖ്യ		t;
    പൂർണ്ണസംഖ്യ		padding = 0;

    എങ്കിൽ (vts == ശൂന്യ || vts[0] == 0)
	തിരികെ ts - (col % ts);

    tabcount = vts[0];

    വേണ്ടി (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	എങ്കിൽ (tabcol > col)
	{
	    padding = (പൂർണ്ണസംഖ്യ)(tabcol - col);
	    നിർത്തുക;
	}
    }
    എങ്കിൽ (t > tabcount)
	padding = vts[tabcount] - (പൂർണ്ണസംഖ്യ)((col - tabcol) % vts[tabcount]);

    തിരികെ padding;
}

/*
 * Find the size of the tab that covers a particular column.
 *
 * If this is being called as part of a shift operation, col is not the cursor
 * column but is the column number to the left of the first non-whitespace
 * character in the line.  If the shift is to the left (left = TRUE), then
 * return the size of the tab interval to the left of the column.
 */
    പൂർണ്ണസംഖ്യ
tabstop_at(colnr_T col, പൂർണ്ണസംഖ്യ ts, പൂർണ്ണസംഖ്യ *vts, പൂർണ്ണസംഖ്യ ഇടത്)
{
    പൂർണ്ണസംഖ്യ		tabcount;	// Number of tab stops in the list of variable
				// tab stops.
    colnr_T	tabcol = 0;	// Column of the tab stop under consideration.
    പൂർണ്ണസംഖ്യ		t;		// Tabstop index in the list of variable tab
				// stops.
    പൂർണ്ണസംഖ്യ		tab_size = 0;	// Size of the tab stop interval to the right
				// or left of the col.

    എങ്കിൽ (vts == 0 || vts[0] == 0)
	തിരികെ ts;

    tabcount = vts[0];
    വേണ്ടി (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	എങ്കിൽ (tabcol > col)
	{
	    // If shifting left (left != 0), and if the column to the left of
	    // the first first non-blank character (col) in the line is
	    // already to the left of the first tabstop, set the shift amount
	    // (tab_size) to just enough to shift the line to the left margin.
	    // The value doesn't seem to matter as long as it is at least that
	    // distance.
	    എങ്കിൽ (ഇടത് && (t == 1))
		tab_size = col;
	    അല്ലെങ്കിൽ
		tab_size = vts[t - (ഇടത് ? 1 : 0)];
	    നിർത്തുക;
	}
    }
    എങ്കിൽ (t > tabcount)		// If the value of the index t is beyond the
				// end of the list, use the tab stop value at
				// the end of the list.
	tab_size = vts[tabcount];

    തിരികെ tab_size;
}

/*
 * Find the column on which a tab starts.
 */
    colnr_T
tabstop_start(colnr_T col, പൂർണ്ണസംഖ്യ ts, പൂർണ്ണസംഖ്യ *vts)
{
    പൂർണ്ണസംഖ്യ		tabcount;
    colnr_T	tabcol = 0;
    പൂർണ്ണസംഖ്യ		t;
    പൂർണ്ണസംഖ്യ		excess;

    എങ്കിൽ (vts == ശൂന്യ || vts[0] == 0)
	തിരികെ col - col % ts;

    tabcount = vts[0];
    വേണ്ടി (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	എങ്കിൽ (tabcol > col)
	    തിരികെ tabcol - vts[t];
    }

    excess = tabcol % vts[tabcount];
    തിരികെ col - (col - excess) % vts[tabcount];
}

/*
 * Find the number of tabs and spaces necessary to get from one column
 * to another.
 */
    ശൂന്യം
tabstop_fromto(
	colnr_T start_col,
	colnr_T end_col,
	പൂർണ്ണസംഖ്യ	ts_arg,
	പൂർണ്ണസംഖ്യ	*vts,
	പൂർണ്ണസംഖ്യ	*ntabs,
	പൂർണ്ണസംഖ്യ	*nspcs)
{
    പൂർണ്ണസംഖ്യ		spaces = end_col - start_col;
    colnr_T	tabcol = 0;
    പൂർണ്ണസംഖ്യ		padding = 0;
    പൂർണ്ണസംഖ്യ		tabcount;
    പൂർണ്ണസംഖ്യ		t;
    പൂർണ്ണസംഖ്യ		ts = ts_arg == 0 ? curbuf->b_p_ts : ts_arg;

    എങ്കിൽ (vts == ശൂന്യ || vts[0] == 0)
    {
	പൂർണ്ണസംഖ്യ tabs = 0;
	പൂർണ്ണസംഖ്യ initspc = 0;

	initspc = ts - (start_col % ts);
	എങ്കിൽ (spaces >= initspc)
	{
	    spaces -= initspc;
	    tabs++;
	}
	tabs += spaces / ts;
	spaces -= (spaces / ts) * ts;

	*ntabs = tabs;
	*nspcs = spaces;
	തിരികെ;
    }

    // Find the padding needed to reach the next tabstop.
    tabcount = vts[0];
    വേണ്ടി (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	എങ്കിൽ (tabcol > start_col)
	{
	    padding = (പൂർണ്ണസംഖ്യ)(tabcol - start_col);
	    നിർത്തുക;
	}
    }
    എങ്കിൽ (t > tabcount)
	padding = vts[tabcount] - (പൂർണ്ണസംഖ്യ)((start_col - tabcol) % vts[tabcount]);

    // If the space needed is less than the padding no tabs can be used.
    എങ്കിൽ (spaces < padding)
    {
	*ntabs = 0;
	*nspcs = spaces;
	തിരികെ;
    }

    *ntabs = 1;
    spaces -= padding;

    // At least one tab has been used. See if any more will fit.
    ആകുമ്പോൾ (spaces != 0 && ++t <= tabcount)
    {
	padding = vts[t];
	എങ്കിൽ (spaces < padding)
	{
	    *nspcs = spaces;
	    തിരികെ;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
tabstop_eq(പൂർണ്ണസംഖ്യ *ts1, പൂർണ്ണസംഖ്യ *ts2)
{
    പൂർണ്ണസംഖ്യ		t;

    എങ്കിൽ ((ts1 == 0 && ts2) || (ts1 && ts2 == 0))
	തിരികെ FALSE;
    എങ്കിൽ (ts1 == ts2)
	തിരികെ TRUE;
    എങ്കിൽ (ts1[0] != ts2[0])
	തിരികെ FALSE;

    വേണ്ടി (t = 1; t <= ts1[0]; ++t)
	എങ്കിൽ (ts1[t] != ts2[t])
	    തിരികെ FALSE;

    തിരികെ TRUE;
}

# എങ്കിൽ defined(FEAT_BEVAL)
/*
 * Copy a tabstop array, allocating space for the new array.
 */
    പൂർണ്ണസംഖ്യ *
tabstop_copy(പൂർണ്ണസംഖ്യ *oldts)
{
    പൂർണ്ണസംഖ്യ		*newts;
    പൂർണ്ണസംഖ്യ		t;

    എങ്കിൽ (oldts == ശൂന്യ)
	തിരികെ ശൂന്യ;
    newts = ALLOC_MULT(പൂർണ്ണസംഖ്യ, oldts[0] + 1);
    എങ്കിൽ (newts != ശൂന്യ)
	വേണ്ടി (t = 0; t <= oldts[0]; ++t)
	    newts[t] = oldts[t];
    തിരികെ newts;
}
# endif

/*
 * Return a count of the number of tabstops.
 */
    പൂർണ്ണസംഖ്യ
tabstop_എണ്ണം(പൂർണ്ണസംഖ്യ *ts)
{
    തിരികെ ts != ശൂന്യ ? ts[0] : 0;
}

/*
 * Return the first tabstop, or 8 if there are no tabstops defined.
 */
    പൂർണ്ണസംഖ്യ
tabstop_first(പൂർണ്ണസംഖ്യ *ts)
{
    തിരികെ ts != ശൂന്യ ? ts[1] : 8;
}

#endif

/*
 * Return the effective shiftwidth value for current buffer, using the
 * 'tabstop' value when 'shiftwidth' is zero.
 */
    long
get_sw_മൂല്യം(buf_T *buf)
{
    തിരികെ get_sw_മൂല്യം_col(buf, 0, FALSE);
}

/*
 * Idem, using "pos".
 */
    സ്ഥിതം long
get_sw_മൂല്യം_pos(buf_T *buf, pos_T *pos, പൂർണ്ണസംഖ്യ ഇടത്)
{
    pos_T save_cursor = curwin->w_cursor;
    long sw_മൂല്യം;

    curwin->w_cursor = *pos;
    sw_മൂല്യം = get_sw_മൂല്യം_col(buf, get_nolist_virtcol(), ഇടത്);
    curwin->w_cursor = save_cursor;
    തിരികെ sw_മൂല്യം;
}

/*
 * Idem, using the first non-black in the current line.
 */
    long
get_sw_മൂല്യം_indent(buf_T *buf, പൂർണ്ണസംഖ്യ ഇടത്)
{
    pos_T pos = curwin->w_cursor;

    pos.col = getwhitecols_curline();
    തിരികെ get_sw_മൂല്യം_pos(buf, &pos, ഇടത്);
}

/*
 * Idem, using virtual column "col".
 */
    long
get_sw_മൂല്യം_col(buf_T *buf, colnr_T col UNUSED, പൂർണ്ണസംഖ്യ ഇടത് UNUSED)
{
    തിരികെ buf->b_p_sw ? buf->b_p_sw :
#ifdef FEAT_VARTABS
	tabstop_at(col, buf->b_p_ts, buf->b_p_vts_array, ഇടത്);
#അല്ലെങ്കിൽ
	buf->b_p_ts;
#endif
}

/*
 * Return the effective softtabstop value for the current buffer, using the
 * 'shiftwidth' value when 'softtabstop' is negative.
 */
    long
get_sts_മൂല്യം(ശൂന്യം)
{
    തിരികെ curbuf->b_p_sts < 0 ? get_sw_മൂല്യം(curbuf) : curbuf->b_p_sts;
}

/*
 * Count the size (in window cells) of the indent in the current line.
 */
    പൂർണ്ണസംഖ്യ
get_indent(ശൂന്യം)
{
#ifdef FEAT_VARTABS
    തിരികെ get_indent_str_vtab(ml_get_curline(), (പൂർണ്ണസംഖ്യ)curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#അല്ലെങ്കിൽ
    തിരികെ get_indent_str(ml_get_curline(), (പൂർണ്ണസംഖ്യ)curbuf->b_p_ts, FALSE);
#endif
}

/*
 * Count the size (in window cells) of the indent in line "lnum".
 */
    പൂർണ്ണസംഖ്യ
get_indent_lnum(linenr_T lnum)
{
#ifdef FEAT_VARTABS
    തിരികെ get_indent_str_vtab(ml_get(lnum), (പൂർണ്ണസംഖ്യ)curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#അല്ലെങ്കിൽ
    തിരികെ get_indent_str(ml_get(lnum), (പൂർണ്ണസംഖ്യ)curbuf->b_p_ts, FALSE);
#endif
}

#എങ്കിൽ defined(FEAT_FOLDING)
/*
 * Count the size (in window cells) of the indent in line "lnum" of buffer
 * "buf".
 */
    പൂർണ്ണസംഖ്യ
get_indent_buf(buf_T *buf, linenr_T lnum)
{
# ifdef FEAT_VARTABS
    തിരികെ get_indent_str_vtab(ml_get_buf(buf, lnum, FALSE),
			       (പൂർണ്ണസംഖ്യ)buf->b_p_ts, buf->b_p_vts_array, FALSE);
# അല്ലെങ്കിൽ
    തിരികെ get_indent_str(ml_get_buf(buf, lnum, FALSE), (പൂർണ്ണസംഖ്യ)buf->b_p_ts, FALSE);
# endif
}
#endif

/*
 * count the size (in window cells) of the indent in line "ptr", with
 * 'tabstop' at "ts"
 */
    പൂർണ്ണസംഖ്യ
get_indent_str(
    char_u	*ptr,
    പൂർണ്ണസംഖ്യ		ts,
    പൂർണ്ണസംഖ്യ		no_ts) // if TRUE, count a tab as ^I
{
    പൂർണ്ണസംഖ്യ		എണ്ണം = 0;

    വേണ്ടി ( ; *ptr; ++ptr)
    {
	എങ്കിൽ (*ptr == TAB)    // count a tab for what it is worth
	{
	    എങ്കിൽ (!no_ts)
		എണ്ണം += ts - (എണ്ണം % ts);
	    അല്ലെങ്കിൽ
		എണ്ണം += ptr2cells(ptr);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*ptr == ' ')
	    ++എണ്ണം;		// count a space for one
	അല്ലെങ്കിൽ
	    നിർത്തുക;
    }
    തിരികെ എണ്ണം;
}

#എങ്കിൽ defined(FEAT_VARTABS)
/*
 * Count the size (in window cells) of the indent in line "ptr", using
 * variable tabstops.
 * If "no_ts" is TRUE, count a tab as ^I.
 */
    പൂർണ്ണസംഖ്യ
get_indent_str_vtab(char_u *ptr, പൂർണ്ണസംഖ്യ ts, പൂർണ്ണസംഖ്യ *vts, പൂർണ്ണസംഖ്യ no_ts)
{
    പൂർണ്ണസംഖ്യ		എണ്ണം = 0;

    വേണ്ടി ( ; *ptr; ++ptr)
    {
	എങ്കിൽ (*ptr == TAB)    // count a tab for what it is worth
	{
	    എങ്കിൽ (!no_ts)
		എണ്ണം += tabstop_padding(എണ്ണം, ts, vts);
	    അല്ലെങ്കിൽ
		എണ്ണം += ptr2cells(ptr);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*ptr == ' ')
	    ++എണ്ണം;		// count a space for one
	അല്ലെങ്കിൽ
	    നിർത്തുക;
    }
    തിരികെ എണ്ണം;
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
    പൂർണ്ണസംഖ്യ
set_indent(
    പൂർണ്ണസംഖ്യ		size,		    // measured in spaces
    പൂർണ്ണസംഖ്യ		flags)
{
    char_u	*p;
    char_u	*newline;
    char_u	*oldline;
    char_u	*s;
    പൂർണ്ണസംഖ്യ		todo;
    പൂർണ്ണസംഖ്യ		ind_len;	    // measured in characters
    പൂർണ്ണസംഖ്യ		വരി_len;	    // size of the line (including the NUL)
    പൂർണ്ണസംഖ്യ		doit = FALSE;
    പൂർണ്ണസംഖ്യ		ind_done = 0;	    // measured in spaces
#ifdef FEAT_VARTABS
    പൂർണ്ണസംഖ്യ		ind_col = 0;
#endif
    പൂർണ്ണസംഖ്യ		tab_pad;
    പൂർണ്ണസംഖ്യ		retval = FALSE;
    പൂർണ്ണസംഖ്യ		orig_char_len = -1; // number of initial whitespace chars when
				    // 'et' and 'pi' are both set

    // First check if there is anything to do and compute the number of
    // characters needed for the indent.
    todo = size;
    ind_len = 0;
    p = oldline = ml_get_curline();
    വരി_len = ml_get_curline_len() + 1;

    // Calculate the buffer size for the new indent, and check to see if it
    // isn't already set

    // if 'expandtab' isn't set: use TABs; if both 'expandtab' and
    // 'preserveindent' are set count the number of characters at the
    // beginning of the line to be copied
    എങ്കിൽ (!curbuf->b_p_et || (!(flags & SIN_തിരുകുക) && curbuf->b_p_pi))
    {
	// If 'preserveindent' is set then reuse as much as possible of
	// the existing indent structure for the new indent
	എങ്കിൽ (!(flags & SIN_തിരുകുക) && curbuf->b_p_pi)
	{
	    ind_done = 0;

	    // count as many characters as we can use
	    ആകുമ്പോൾ (todo > 0 && VIM_ISWHITE(*p))
	    {
		എങ്കിൽ (*p == TAB)
		{
#ifdef FEAT_VARTABS
		    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#അല്ലെങ്കിൽ
		    tab_pad = (പൂർണ്ണസംഖ്യ)curbuf->b_p_ts
					   - (ind_done % (പൂർണ്ണസംഖ്യ)curbuf->b_p_ts);
#endif
		    // stop if this tab will overshoot the target
		    എങ്കിൽ (todo < tab_pad)
			നിർത്തുക;
		    todo -= tab_pad;
		    ++ind_len;
		    ind_done += tab_pad;
		}
		അല്ലെങ്കിൽ
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
	    എങ്കിൽ (curbuf->b_p_et)
		orig_char_len = ind_len;

	    // Fill to next tabstop with a tab, if possible
#ifdef FEAT_VARTABS
	    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
						curbuf->b_p_vts_array);
#അല്ലെങ്കിൽ
	    tab_pad = (പൂർണ്ണസംഖ്യ)curbuf->b_p_ts - (ind_done % (പൂർണ്ണസംഖ്യ)curbuf->b_p_ts);
#endif
	    എങ്കിൽ (todo >= tab_pad && orig_char_len == -1)
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
	വേണ്ടി (;;)
	{
	    tab_pad = tabstop_padding(ind_col, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
	    എങ്കിൽ (todo < tab_pad)
		നിർത്തുക;
	    എങ്കിൽ (*p != TAB)
		doit = TRUE;
	    അല്ലെങ്കിൽ
		++p;
	    todo -= tab_pad;
	    ++ind_len;
	    ind_col += tab_pad;
	}
#അല്ലെങ്കിൽ
	ആകുമ്പോൾ (todo >= (പൂർണ്ണസംഖ്യ)curbuf->b_p_ts)
	{
	    എങ്കിൽ (*p != TAB)
		doit = TRUE;
	    അല്ലെങ്കിൽ
		++p;
	    todo -= (പൂർണ്ണസംഖ്യ)curbuf->b_p_ts;
	    ++ind_len;
	    // ind_done += (int)curbuf->b_p_ts;
	}
#endif
    }
    // count spaces required for indent
    ആകുമ്പോൾ (todo > 0)
    {
	എങ്കിൽ (*p != ' ')
	    doit = TRUE;
	അല്ലെങ്കിൽ
	    ++p;
	--todo;
	++ind_len;
	// ++ind_done;
    }

    // Return if the indent is OK already.
    എങ്കിൽ (!doit && !VIM_ISWHITE(*p) && !(flags & SIN_തിരുകുക))
	തിരികെ FALSE;

    // Allocate memory for the new line.
    എങ്കിൽ (flags & SIN_തിരുകുക)
	p = oldline;
    അല്ലെങ്കിൽ
    {
	p = skipwhite(p);
	വരി_len -= (പൂർണ്ണസംഖ്യ)(p - oldline);
    }

    // If 'preserveindent' and 'expandtab' are both set keep the original
    // characters and allocate accordingly.  We will fill the rest with spaces
    // after the if (!curbuf->b_p_et) below.
    എങ്കിൽ (orig_char_len != -1)
    {
	newline = alloc(orig_char_len + size - ind_done + വരി_len);
	എങ്കിൽ (newline == ശൂന്യ)
	    തിരികെ FALSE;
	todo = size - ind_done;
	ind_len = orig_char_len + todo;    // Set total length of indent in
					   // characters, which may have been
					   // undercounted until now
	p = oldline;
	s = newline;
	ആകുമ്പോൾ (orig_char_len > 0)
	{
	    *s++ = *p++;
	    orig_char_len--;
	}

	// Skip over any additional white space (useful when newindent is less
	// than old)
	ആകുമ്പോൾ (VIM_ISWHITE(*p))
	    ++p;

    }
    അല്ലെങ്കിൽ
    {
	todo = size;
	newline = alloc(ind_len + വരി_len);
	എങ്കിൽ (newline == ശൂന്യ)
	    തിരികെ FALSE;
	s = newline;
    }

    // Put the characters in the new line.
    // if 'expandtab' isn't set: use TABs
    എങ്കിൽ (!curbuf->b_p_et)
    {
	// If 'preserveindent' is set then reuse as much as possible of
	// the existing indent structure for the new indent
	എങ്കിൽ (!(flags & SIN_തിരുകുക) && curbuf->b_p_pi)
	{
	    p = oldline;
	    ind_done = 0;

	    ആകുമ്പോൾ (todo > 0 && VIM_ISWHITE(*p))
	    {
		എങ്കിൽ (*p == TAB)
		{
#ifdef FEAT_VARTABS
		    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#അല്ലെങ്കിൽ
		    tab_pad = (പൂർണ്ണസംഖ്യ)curbuf->b_p_ts
					   - (ind_done % (പൂർണ്ണസംഖ്യ)curbuf->b_p_ts);
#endif
		    // stop if this tab will overshoot the target
		    എങ്കിൽ (todo < tab_pad)
			നിർത്തുക;
		    todo -= tab_pad;
		    ind_done += tab_pad;
		}
		അല്ലെങ്കിൽ
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
#അല്ലെങ്കിൽ
	    tab_pad = (പൂർണ്ണസംഖ്യ)curbuf->b_p_ts - (ind_done % (പൂർണ്ണസംഖ്യ)curbuf->b_p_ts);
#endif
	    എങ്കിൽ (todo >= tab_pad)
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
	വേണ്ടി (;;)
	{
	    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
	    എങ്കിൽ (todo < tab_pad)
		നിർത്തുക;
	    *s++ = TAB;
	    todo -= tab_pad;
	    ind_done += tab_pad;
	}
#അല്ലെങ്കിൽ
	ആകുമ്പോൾ (todo >= (പൂർണ്ണസംഖ്യ)curbuf->b_p_ts)
	{
	    *s++ = TAB;
	    todo -= (പൂർണ്ണസംഖ്യ)curbuf->b_p_ts;
	}
#endif
    }
    ആകുമ്പോൾ (todo > 0)
    {
	*s++ = ' ';
	--todo;
    }
    mch_memmove(s, p, (size_t)വരി_len);

    // Replace the line (unless undo fails).
    എങ്കിൽ (!(flags & SIN_UNDO) || u_savesub(curwin->w_cursor.lnum) == OK)
    {
	colnr_T old_offset = (colnr_T)(p - oldline);
	colnr_T new_offset = (colnr_T)(s - newline);

	// this may free "newline"
	ml_replace(curwin->w_cursor.lnum, newline, FALSE);
	എങ്കിൽ (flags & SIN_CHANGED)
	    changed_bytes(curwin->w_cursor.lnum, 0);

	// Correct saved cursor position if it is in this line.
	എങ്കിൽ (saved_cursor.lnum == curwin->w_cursor.lnum)
	{
	    എങ്കിൽ (saved_cursor.col >= old_offset)
		// cursor was after the indent, adjust for the number of
		// bytes added/removed
		saved_cursor.col += ind_len - old_offset;
	    അല്ലെങ്കിൽ എങ്കിൽ (saved_cursor.col >= new_offset)
		// cursor was in the indent, and is now after it, put it back
		// at the start of the indent (replacing spaces with TAB)
		saved_cursor.col = new_offset;
	}
#ifdef FEAT_PROP_POPUP
	എങ്കിൽ (curbuf->b_has_textprop)
	{
	    പൂർണ്ണസംഖ്യ added = ind_len - old_offset;

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
    അല്ലെങ്കിൽ
	vim_free(newline);

    curwin->w_cursor.col = ind_len;
    തിരികെ retval;
}

/*
 * Return the indent of the current line after a number.  Return -1 if no
 * number was found.  Used for 'n' in 'formatoptions': numbered list.
 * Since a pattern is used it can actually handle more than numbers.
 */
    പൂർണ്ണസംഖ്യ
get_സംഖ്യ_indent(linenr_T lnum)
{
    colnr_T	col;
    pos_T	pos;

    regmatch_T	regmatch;
    പൂർണ്ണസംഖ്യ		lead_len = 0;	// length of comment leader

    എങ്കിൽ (lnum > curbuf->b_ml.ml_വരി_എണ്ണം)
	തിരികെ -1;
    pos.lnum = 0;

    // In format_lines() (i.e. not insert mode), fo+=q is needed too...
    എങ്കിൽ ((State & MODE_തിരുകുക) || has_format_option(FO_Q_COMS))
	lead_len = get_leader_len(ml_get(lnum), ശൂന്യ, FALSE, TRUE);

    regmatch.regprog = vim_regcomp(curbuf->b_p_flp, RE_MAGIC);
    എങ്കിൽ (regmatch.regprog != ശൂന്യ)
    {
	regmatch.rm_ic = FALSE;

	// vim_regexec() expects a pointer to a line.  This lets us
	// start matching for the flp beyond any comment leader...
	എങ്കിൽ (vim_regexec(&regmatch, ml_get(lnum) + lead_len, (colnr_T)0))
	{
	    pos.lnum = lnum;
	    pos.col = (colnr_T)(*regmatch.endp - ml_get(lnum));
	    pos.coladd = 0;
	}
	vim_regfree(regmatch.regprog);
    }

    എങ്കിൽ (pos.lnum == 0 || *ml_get_pos(&pos) == NUL)
	തിരികെ -1;
    getvcol(curwin, &pos, &col, ശൂന്യ, ശൂന്യ, 0);
    തിരികെ (പൂർണ്ണസംഖ്യ)col;
}

#എങ്കിൽ defined(FEAT_LINEBREAK)
/*
 * Check "briopt" as 'breakindentopt' and update the members of "wp".
 * This is called when 'breakindentopt' is changed and when a window is
 * initialized.
 * Returns FAIL for failure, OK otherwise.
 */
    പൂർണ്ണസംഖ്യ
briopt_check(
    char_u	*briopt,	// when NULL: use "wp->w_p_briopt"
    win_T	*wp)		// when NULL: only check "briopt"
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		bri_shift = 0;
    long	bri_min = 20;
    പൂർണ്ണസംഖ്യ		bri_sbr = FALSE;
    പൂർണ്ണസംഖ്യ		bri_list = 0;
    പൂർണ്ണസംഖ്യ		bri_vcol = 0;

    എങ്കിൽ (briopt != ശൂന്യ)
	p = briopt;
    അല്ലെങ്കിൽ
	p = wp->w_p_briopt;

    ആകുമ്പോൾ (*p != NUL)
    {
	// Note: Keep this in sync with p_briopt_values
	എങ്കിൽ (STRNCMP(p, "shift:", 6) == 0
		 && ((p[6] == '-' && VIM_ISDIGIT(p[7])) || VIM_ISDIGIT(p[6])))
	{
	    p += 6;
	    bri_shift = getdigits(&p);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "min:", 4) == 0 && VIM_ISDIGIT(p[4]))
	{
	    p += 4;
	    bri_min = getdigits(&p);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "sbr", 3) == 0)
	{
	    p += 3;
	    bri_sbr = TRUE;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "list:", 5) == 0)
	{
	    p += 5;
	    bri_list = getdigits(&p);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "column:", 7) == 0)
	{
	    p += 7;
	    bri_vcol = getdigits(&p);
	}
	എങ്കിൽ (*p != ',' && *p != NUL)
	    തിരികെ FAIL;
	എങ്കിൽ (*p == ',')
	    ++p;
    }

    എങ്കിൽ (wp == ശൂന്യ)
	തിരികെ OK;

    wp->w_briopt_shift = bri_shift;
    wp->w_briopt_min   = bri_min;
    wp->w_briopt_sbr   = bri_sbr;
    wp->w_briopt_list  = bri_list;
    wp->w_briopt_vcol  = bri_vcol;

    തിരികെ OK;
}

/*
 * Return appropriate space number for breakindent, taking influencing
 * parameters into account. Window must be specified, since it is not
 * necessarily always the current one.
 */
    പൂർണ്ണസംഖ്യ
get_breakindent_win(
    win_T	*wp,
    char_u	*വരി) // start of the line
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ	    prev_indent = 0;	// cached indent value
    സ്ഥിതം long	    prev_ts     = 0L;	// cached tabstop value
# ifdef FEAT_VARTABS
    സ്ഥിതം പൂർണ്ണസംഖ്യ      *prev_vts = ശൂന്യ;   // cached vartabs values
# endif
    സ്ഥിതം പൂർണ്ണസംഖ്യ	    prev_fnum   = 0;	// cached buffer number
    സ്ഥിതം char_u   *prev_വരി  = ശൂന്യ;	// cached copy of "line"
    സ്ഥിതം varnumber_T prev_tick = 0;   // changedtick of cached value
    സ്ഥിതം പൂർണ്ണസംഖ്യ      prev_list = 0;	// cached list indent
    സ്ഥിതം പൂർണ്ണസംഖ്യ      prev_listopt = 0;	// cached w_p_briopt_list value
    സ്ഥിതം പൂർണ്ണസംഖ്യ      prev_no_ts = FALSE;	// cached no_ts value
    സ്ഥിതം unsigned prev_dy_uhex = 0;	// cached 'display' "uhex" value
    // cached formatlistpat value
    സ്ഥിതം char_u   *prev_flp = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		    bri = 0;
    // window width minus window margin space, i.e. what rests for text
    സ്ഥിരം പൂർണ്ണസംഖ്യ	    eff_wwidth = wp->w_width
					  - win_col_off(wp) + win_col_off2(wp);

    // In list mode, if 'listchars' "tab" isn't set, a TAB is displayed as ^I.
    പൂർണ്ണസംഖ്യ no_ts = wp->w_p_list && wp->w_lcs_chars.tab1 == NUL;

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
    എങ്കിൽ (prev_fnum != wp->w_buffer->b_fnum
	    || prev_ts != wp->w_buffer->b_p_ts
# ifdef FEAT_VARTABS
	    || prev_vts != wp->w_buffer->b_p_vts_array
# endif
	    || prev_tick != CHANGEDTICK(wp->w_buffer)
	    || prev_listopt != wp->w_briopt_list
	    || prev_no_ts != no_ts
	    || prev_dy_uhex != (dy_flags & DY_UHEX)
	    || prev_flp == ശൂന്യ
	    || STRCMP(prev_flp, get_flp_മൂല്യം(wp->w_buffer)) != 0
	    || prev_വരി == ശൂന്യ || STRCMP(prev_വരി, വരി) != 0
	)
    {
	prev_fnum = wp->w_buffer->b_fnum;
	vim_free(prev_വരി);
	prev_വരി = vim_strsave(വരി);
	prev_ts = wp->w_buffer->b_p_ts;
# ifdef FEAT_VARTABS
	prev_vts = wp->w_buffer->b_p_vts_array;
	എങ്കിൽ (wp->w_briopt_vcol == 0)
	    prev_indent = get_indent_str_vtab(വരി,
				     (പൂർണ്ണസംഖ്യ)wp->w_buffer->b_p_ts,
				      wp->w_buffer->b_p_vts_array, no_ts);
# അല്ലെങ്കിൽ
	എങ്കിൽ (wp->w_briopt_vcol == 0)
	    prev_indent = get_indent_str(വരി,
					(പൂർണ്ണസംഖ്യ)wp->w_buffer->b_p_ts, no_ts);
# endif
	prev_tick = CHANGEDTICK(wp->w_buffer);
	prev_listopt = wp->w_briopt_list;
	prev_list = 0;
	prev_no_ts = no_ts;
	prev_dy_uhex = (dy_flags & DY_UHEX);
	vim_free(prev_flp);
	prev_flp = vim_strsave(get_flp_മൂല്യം(wp->w_buffer));
	// add additional indent for numbered lists
	എങ്കിൽ (wp->w_briopt_list != 0 && wp->w_briopt_vcol == 0)
	{
	    regmatch_T	    regmatch;

	    regmatch.regprog = vim_regcomp(prev_flp,
				   RE_MAGIC + RE_STRING + RE_AUTO + RE_STRICT);

	    എങ്കിൽ (regmatch.regprog != ശൂന്യ)
	    {
		regmatch.rm_ic = FALSE;
		എങ്കിൽ (vim_regexec(&regmatch, വരി, 0))
		{
		    എങ്കിൽ (wp->w_briopt_list > 0)
			prev_list = wp->w_briopt_list;
		    അല്ലെങ്കിൽ
		    {
			char_u	*ptr = *regmatch.startp;
			char_u	*end_ptr = *regmatch.endp;
			പൂർണ്ണസംഖ്യ	indent = 0;

			// Compute the width of the matched text.
			// Use win_chartabsize() so that TAB size is correct,
			// while wrapping is ignored.
			ആകുമ്പോൾ (ptr < end_ptr)
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
    എങ്കിൽ (wp->w_briopt_vcol != 0)
    {
	// column value has priority
	bri = wp->w_briopt_vcol;
	prev_list = 0;
    }
    അല്ലെങ്കിൽ
	bri = prev_indent + wp->w_briopt_shift;

    // Add offset for number column, if 'n' is in 'cpoptions'
    bri += win_col_off2(wp);

    // add additional indent for numbered lists
    എങ്കിൽ (wp->w_briopt_list > 0)
	bri += prev_list;

    // indent minus the length of the showbreak string
    എങ്കിൽ (wp->w_briopt_sbr)
	bri -= vim_strsize(get_showbreak_മൂല്യം(wp));


    // never indent past left window margin
    എങ്കിൽ (bri < 0)
	bri = 0;

    // always leave at least bri_min characters on the left,
    // if text width is sufficient
    അല്ലെങ്കിൽ എങ്കിൽ (bri > eff_wwidth - wp->w_briopt_min)
	bri = (eff_wwidth - wp->w_briopt_min < 0)
					   ? 0 : eff_wwidth - wp->w_briopt_min;

    തിരികെ bri;
}
#endif

/*
 * When extra == 0: Return TRUE if the cursor is before or on the first
 *		    non-blank in the line.
 * When extra == 1: Return TRUE if the cursor is before the first non-blank in
 *		    the line.
 */
    പൂർണ്ണസംഖ്യ
inindent(പൂർണ്ണസംഖ്യ extra)
{
    char_u	*ptr;
    colnr_T	col;

    വേണ്ടി (col = 0, ptr = ml_get_curline(); VIM_ISWHITE(*ptr); ++col)
	++ptr;
    എങ്കിൽ (col >= curwin->w_cursor.col + extra)
	തിരികെ TRUE;
    അല്ലെങ്കിൽ
	തിരികെ FALSE;
}

/*
 * op_reindent - handle reindenting a block of lines.
 */
    ശൂന്യം
op_reindent(oparg_T *oap, പൂർണ്ണസംഖ്യ (*how)(ശൂന്യം))
{
    long	i = 0;
    char_u	*l;
    പൂർണ്ണസംഖ്യ		amount;
    linenr_T	first_changed = 0;
    linenr_T	last_changed = 0;
    linenr_T	start_lnum = curwin->w_cursor.lnum;

    // Don't even try when 'modifiable' is off.
    എങ്കിൽ (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	തിരികെ;
    }

    // Save for undo.  Do this once for all lines, much faster than doing this
    // for each line separately, especially when undoing.
    എങ്കിൽ (u_savecommon(start_lnum - 1, start_lnum + oap->വരി_എണ്ണം,
				     start_lnum + oap->വരി_എണ്ണം, FALSE) == OK)
	വേണ്ടി (i = oap->വരി_എണ്ണം; --i >= 0 && !got_int; )
	{
	    // it's a slow thing to do, so give feedback so there's no worry
	    // that the computer's just hung.

	    എങ്കിൽ (i > 1
		    && (i % 50 == 0 || i == oap->വരി_എണ്ണം - 1)
		    && oap->വരി_എണ്ണം > p_report)
		smsg(_("%ld lines to indent... "), i);

	    // Be vi-compatible: For lisp indenting the first line is not
	    // indented, unless there is only one line.
	    എങ്കിൽ (i != oap->വരി_എണ്ണം - 1 || oap->വരി_എണ്ണം == 1
						     || how != get_lisp_indent)
	    {
		l = skipwhite(ml_get_curline());
		എങ്കിൽ (*l == NUL)		    // empty or blank line
		    amount = 0;
		അല്ലെങ്കിൽ
		    amount = how();	    // get the indent for this line

		എങ്കിൽ (amount >= 0 && set_indent(amount, 0))
		{
		    // did change the indent, call changed_lines() later
		    എങ്കിൽ (first_changed == 0)
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
    എങ്കിൽ (last_changed != 0)
	changed_വരികൾ(first_changed, 0,
		oap->is_VIsual ? start_lnum + oap->വരി_എണ്ണം :
		last_changed + 1, 0L);
    അല്ലെങ്കിൽ എങ്കിൽ (oap->is_VIsual)
	redraw_curbuf_later(UPD_INVERTED);

    എങ്കിൽ (oap->വരി_എണ്ണം > p_report)
    {
	i = oap->വരി_എണ്ണം - (i + 1);
	smsg(NGETTEXT("%ld line indented ",
						 "%ld lines indented ", i), i);
    }
    എങ്കിൽ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// set '[ and '] marks
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
    }
}

/*
 * Return TRUE if lines starting with '#' should be left aligned.
 */
    പൂർണ്ണസംഖ്യ
preprocs_ഇടത്(ശൂന്യം)
{
    തിരികെ
	(curbuf->b_p_si && !curbuf->b_p_cin) ||
	(curbuf->b_p_cin && in_cinkeys('#', ' ', TRUE)
					   && curbuf->b_ind_hash_comment == 0)
	;
}

/*
 * Return TRUE if the conditions are OK for smart indenting.
 */
    പൂർണ്ണസംഖ്യ
may_do_si(ശൂന്യം)
{
    തിരികെ curbuf->b_p_si
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
    ശൂന്യം
ins_try_si(പൂർണ്ണസംഖ്യ c)
{
    pos_T	*pos, old_pos;
    char_u	*ptr;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		temp;

    // do some very smart indenting when entering '{' or '}'
    എങ്കിൽ (((did_si || can_si_back) && c == '{')
	    || (can_si && c == '}' && inindent(0)))
    {
	// for '}' set indent equal to indent of line containing matching '{'
	എങ്കിൽ (c == '}' && (pos = findmatch(ശൂന്യ, '{')) != ശൂന്യ)
	{
	    old_pos = curwin->w_cursor;
	    // If the matching '{' has a ')' immediately before it (ignoring
	    // white-space), then line up with the start of the line
	    // containing the matching '(' if there is one.  This handles the
	    // case where an "if (..\n..) {" statement continues over multiple
	    // lines -- webb
	    ptr = ml_get(pos->lnum);
	    i = pos->col;
	    എങ്കിൽ (i > 0)		// skip blanks before '{'
		ആകുമ്പോൾ (--i > 0 && VIM_ISWHITE(ptr[i]))
		    ;
	    curwin->w_cursor.lnum = pos->lnum;
	    curwin->w_cursor.col = i;
	    എങ്കിൽ (ptr[i] == ')' && (pos = findmatch(ശൂന്യ, '(')) != ശൂന്യ)
		curwin->w_cursor = *pos;
	    i = get_indent();
	    curwin->w_cursor = old_pos;
	    എങ്കിൽ (State & VREPLACE_FLAG)
		change_indent(INDENT_SET, i, FALSE, NUL, TRUE);
	    അല്ലെങ്കിൽ
		(ശൂന്യം)set_indent(i, SIN_CHANGED);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (curwin->w_cursor.col > 0)
	{
	    // when inserting '{' after "O" reduce indent, but not
	    // more than indent of previous line
	    temp = TRUE;
	    എങ്കിൽ (c == '{' && can_si_back && curwin->w_cursor.lnum > 1)
	    {
		old_pos = curwin->w_cursor;
		i = get_indent();
		ആകുമ്പോൾ (curwin->w_cursor.lnum > 1)
		{
		    ptr = skipwhite(ml_get(--(curwin->w_cursor.lnum)));

		    // ignore empty lines and lines starting with '#'.
		    എങ്കിൽ (*ptr != '#' && *ptr != NUL)
			നിർത്തുക;
		}
		എങ്കിൽ (get_indent() >= i)
		    temp = FALSE;
		curwin->w_cursor = old_pos;
	    }
	    എങ്കിൽ (temp)
		shift_വരി(TRUE, FALSE, 1, TRUE);
	}
    }

    // set indent of '#' always to 0
    എങ്കിൽ (curwin->w_cursor.col > 0 && can_si && c == '#' && inindent(0))
    {
	// remember current indent for next line
	old_indent = get_indent();
	(ശൂന്യം)set_indent(0, SIN_CHANGED);
    }

    // Adjust ai_col, the char at this position can be deleted.
    എങ്കിൽ (ai_col > curwin->w_cursor.col)
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
    ശൂന്യം
change_indent(
    പൂർണ്ണസംഖ്യ		type,
    പൂർണ്ണസംഖ്യ		amount,
    പൂർണ്ണസംഖ്യ		round,
    പൂർണ്ണസംഖ്യ		replaced,	// replaced character, put on replace stack
    പൂർണ്ണസംഖ്യ		call_changed_bytes)	// call changed_bytes()
{
    പൂർണ്ണസംഖ്യ		vcol;
    പൂർണ്ണസംഖ്യ		last_vcol;
    പൂർണ്ണസംഖ്യ		insstart_less;		// reduction for Insstart.col
    പൂർണ്ണസംഖ്യ		new_cursor_col;
    പൂർണ്ണസംഖ്യ		i;
    char_u	*ptr;
    പൂർണ്ണസംഖ്യ		save_p_list;
    പൂർണ്ണസംഖ്യ		start_col;
    colnr_T	vc;
    colnr_T	orig_col = 0;		// init for GCC
    char_u	*new_വരി, *orig_വരി = ശൂന്യ;	// init for GCC

    // MODE_VREPLACE state needs to know what the line was like before changing
    എങ്കിൽ (State & VREPLACE_FLAG)
    {
	orig_വരി = vim_strnsave(ml_get_curline(), ml_get_curline_len());  // Deal with NULL below
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
    എങ്കിൽ (new_cursor_col < 0)
	vcol = get_indent() - vcol;

    എങ്കിൽ (new_cursor_col > 0)	    // can't fix replace stack
	start_col = -1;

    // Set the new indent.  The cursor will be put on the first non-blank.
    എങ്കിൽ (type == INDENT_SET)
	(ശൂന്യം)set_indent(amount, call_changed_bytes ? SIN_CHANGED : 0);
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ	save_State = State;

	// Avoid being called recursively.
	എങ്കിൽ (State & VREPLACE_FLAG)
	    State = MODE_തിരുകുക;
	shift_വരി(type == INDENT_DEC, round, 1, call_changed_bytes);
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
    എങ്കിൽ (new_cursor_col >= 0)
    {
	// When changing the indent while the cursor is touching it, reset
	// Insstart_col to 0.
	എങ്കിൽ (new_cursor_col == 0)
	    insstart_less = MAXCOL;
	new_cursor_col += curwin->w_cursor.col;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (!(State & MODE_തിരുകുക))
	new_cursor_col = curwin->w_cursor.col;
    അല്ലെങ്കിൽ
    {
	chartabsize_T cts;

	// Compute the screen column where the cursor should be.
	vcol = get_indent() - vcol;
	curwin->w_virtcol = (colnr_T)((vcol < 0) ? 0 : vcol);

	// Advance the cursor until we reach the right screen column.
	last_vcol = 0;
	ptr = ml_get_curline();
	init_chartabsize_arg(&cts, curwin, 0, 0, ptr, ptr);
	ആകുമ്പോൾ (cts.cts_vcol <= (പൂർണ്ണസംഖ്യ)curwin->w_virtcol)
	{
	    last_vcol = cts.cts_vcol;
	    എങ്കിൽ (cts.cts_vcol > 0)
		MB_PTR_ADV(cts.cts_ptr);
	    എങ്കിൽ (*cts.cts_ptr == NUL)
		നിർത്തുക;
	    cts.cts_vcol += lbr_chartabsize(&cts);
	}
	vcol = last_vcol;
	new_cursor_col = cts.cts_ptr - cts.cts_വരി;
	clear_chartabsize_arg(&cts);

	// May need to insert spaces to be able to position the cursor on
	// the right screen column.
	എങ്കിൽ (vcol != (പൂർണ്ണസംഖ്യ)curwin->w_virtcol)
	{
	    curwin->w_cursor.col = (colnr_T)new_cursor_col;
	    i = (പൂർണ്ണസംഖ്യ)curwin->w_virtcol - vcol;
	    ptr = alloc(i + 1);
	    എങ്കിൽ (ptr != ശൂന്യ)
	    {
		size_t ptrlen;
		new_cursor_col += i;
		ptr[i] = NUL;
		ptrlen = i;
		ആകുമ്പോൾ (--i >= 0)
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

    എങ്കിൽ (new_cursor_col <= 0)
	curwin->w_cursor.col = 0;
    അല്ലെങ്കിൽ
	curwin->w_cursor.col = (colnr_T)new_cursor_col;
    curwin->w_set_curswant = true;
    changed_cline_bef_curs();

    // May have to adjust the start of the insert.
    എങ്കിൽ (State & MODE_തിരുകുക)
    {
	എങ്കിൽ (curwin->w_cursor.lnum == Insstart.lnum && Insstart.col != 0)
	{
	    എങ്കിൽ ((പൂർണ്ണസംഖ്യ)Insstart.col <= insstart_less)
		Insstart.col = 0;
	    അല്ലെങ്കിൽ
		Insstart.col -= insstart_less;
	}
	എങ്കിൽ ((പൂർണ്ണസംഖ്യ)ai_col <= insstart_less)
	    ai_col = 0;
	അല്ലെങ്കിൽ
	    ai_col -= insstart_less;
    }

    // For MODE_REPLACE state, may have to fix the replace stack, if it's
    // possible.  If the number of characters before the cursor decreased, need
    // to pop a few characters from the replace stack.
    // If the number of characters before the cursor increased, need to push a
    // few NULs onto the replace stack.
    എങ്കിൽ (REPLACE_NORMAL(State) && start_col >= 0)
    {
	ആകുമ്പോൾ (start_col > (പൂർണ്ണസംഖ്യ)curwin->w_cursor.col)
	{
	    replace_join(0);	    // remove a NUL from the replace stack
	    --start_col;
	}
	ആകുമ്പോൾ (start_col < (പൂർണ്ണസംഖ്യ)curwin->w_cursor.col || replaced)
	{
	    replace_തള്ളുക(NUL);
	    എങ്കിൽ (replaced)
	    {
		replace_തള്ളുക(replaced);
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
    എങ്കിൽ (State & VREPLACE_FLAG)
    {
	// If orig_line didn't allocate, just return.  At least we did the job,
	// even if you can't backspace.
	എങ്കിൽ (orig_വരി == ശൂന്യ)
	    തിരികെ;

	// Save new line
	new_വരി = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	എങ്കിൽ (new_വരി == ശൂന്യ)
	{
	    vim_free(orig_വരി);
	    തിരികെ;
	}

	// We only put back the new line up to the cursor
	new_വരി[curwin->w_cursor.col] = NUL;

	// Put back original line
	ml_replace(curwin->w_cursor.lnum, orig_വരി, FALSE);
	curwin->w_cursor.col = orig_col;

	// Backspace from cursor to start of line
	backspace_until_column(0);

	// Insert new stuff into line again
	ins_bytes(new_വരി);

	vim_free(new_വരി);
    }
}

/*
 * Copy the indent from ptr to the current line (and fill to size)
 * Leaves the cursor on the first non-blank in the line.
 * Returns TRUE if the line was changed.
 */
    പൂർണ്ണസംഖ്യ
copy_indent(പൂർണ്ണസംഖ്യ size, char_u *src)
{
    char_u	*p = ശൂന്യ;
    char_u	*വരി = ശൂന്യ;
    char_u	*s;
    പൂർണ്ണസംഖ്യ		todo;
    പൂർണ്ണസംഖ്യ		ind_len;
    പൂർണ്ണസംഖ്യ		വരി_len = 0;
    പൂർണ്ണസംഖ്യ		tab_pad;
    പൂർണ്ണസംഖ്യ		ind_done;
    പൂർണ്ണസംഖ്യ		round;
#ifdef FEAT_VARTABS
    പൂർണ്ണസംഖ്യ		ind_col;
#endif

    // Round 1: compute the number of characters needed for the indent
    // Round 2: copy the characters.
    വേണ്ടി (round = 1; round <= 2; ++round)
    {
	todo = size;
	ind_len = 0;
	ind_done = 0;
#ifdef FEAT_VARTABS
	ind_col = 0;
#endif
	s = src;

	// Count/copy the usable portion of the source line
	ആകുമ്പോൾ (todo > 0 && VIM_ISWHITE(*s))
	{
	    എങ്കിൽ (*s == TAB)
	    {
#ifdef FEAT_VARTABS
		tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#അല്ലെങ്കിൽ
		tab_pad = (പൂർണ്ണസംഖ്യ)curbuf->b_p_ts
					   - (ind_done % (പൂർണ്ണസംഖ്യ)curbuf->b_p_ts);
#endif
		// Stop if this tab will overshoot the target
		എങ്കിൽ (todo < tab_pad)
		    നിർത്തുക;
		todo -= tab_pad;
		ind_done += tab_pad;
#ifdef FEAT_VARTABS
		ind_col += tab_pad;
#endif
	    }
	    അല്ലെങ്കിൽ
	    {
		--todo;
		++ind_done;
#ifdef FEAT_VARTABS
		++ind_col;
#endif
	    }
	    ++ind_len;
	    എങ്കിൽ (p != ശൂന്യ)
		*p++ = *s;
	    ++s;
	}

	// Fill to next tabstop with a tab, if possible
#ifdef FEAT_VARTABS
	tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#അല്ലെങ്കിൽ
	tab_pad = (പൂർണ്ണസംഖ്യ)curbuf->b_p_ts - (ind_done % (പൂർണ്ണസംഖ്യ)curbuf->b_p_ts);
#endif
	എങ്കിൽ (todo >= tab_pad && !curbuf->b_p_et)
	{
	    todo -= tab_pad;
	    ++ind_len;
#ifdef FEAT_VARTABS
	    ind_col += tab_pad;
#endif
	    എങ്കിൽ (p != ശൂന്യ)
		*p++ = TAB;
	}

	// Add tabs required for indent
	എങ്കിൽ (!curbuf->b_p_et)
	{
#ifdef FEAT_VARTABS
	    വേണ്ടി (;;)
	    {
		tab_pad = tabstop_padding(ind_col, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
		എങ്കിൽ (todo < tab_pad)
		    നിർത്തുക;
		todo -= tab_pad;
		++ind_len;
		ind_col += tab_pad;
		എങ്കിൽ (p != ശൂന്യ)
		    *p++ = TAB;
	    }
#അല്ലെങ്കിൽ
	    ആകുമ്പോൾ (todo >= (പൂർണ്ണസംഖ്യ)curbuf->b_p_ts)
	    {
		todo -= (പൂർണ്ണസംഖ്യ)curbuf->b_p_ts;
		++ind_len;
		എങ്കിൽ (p != ശൂന്യ)
		    *p++ = TAB;
	    }
#endif
	}

	// Count/add spaces required for indent
	ആകുമ്പോൾ (todo > 0)
	{
	    --todo;
	    ++ind_len;
	    എങ്കിൽ (p != ശൂന്യ)
		*p++ = ' ';
	}

	എങ്കിൽ (p == ശൂന്യ)
	{
	    // Allocate memory for the result: the copied indent, new indent
	    // and the rest of the line.
	    വരി_len = ml_get_curline_len() + 1;
	    വരി = alloc(ind_len + വരി_len);
	    എങ്കിൽ (വരി == ശൂന്യ)
		തിരികെ FALSE;
	    p = വരി;
	}
    }

    // Append the original line
    mch_memmove(p, ml_get_curline(), (size_t)വരി_len);

    // Replace the line
    ml_replace(curwin->w_cursor.lnum, വരി, FALSE);

    // Put the cursor after the indent.
    curwin->w_cursor.col = ind_len;
    തിരികെ TRUE;
}

/*
 * Give a "resulting text too long" error and maybe set got_int.
 */
    സ്ഥിതം ശൂന്യം
emsg_text_too_long(ശൂന്യം)
{
    emsg(_(e_resulting_text_too_long));
#ifdef FEAT_EVAL
    // when not inside a try/catch set got_int to break out of any loop
    എങ്കിൽ (trylevel == 0)
#endif
	got_int = TRUE;
}

/*
 * ":retab".
 */
    ശൂന്യം
ex_retab(exarg_T *eap)
{
    linenr_T	lnum;
    പൂർണ്ണസംഖ്യ		got_tab = FALSE;
    long	അക്കം_spaces = 0;
    long	അക്കം_tabs;
    long	len;
    long	col;
    long	vcol;
    long	start_col = 0;		// For start of white-space string
    long	start_vcol = 0;		// For start of white-space string
    long	old_len;
    long	new_len;
    char_u	*ptr;
    char_u	*new_വരി = (char_u *)1; // init to non-NULL
    പൂർണ്ണസംഖ്യ		did_undo;		// called u_save for current line
#ifdef FEAT_VARTABS
    പൂർണ്ണസംഖ്യ		*new_vts_array = ശൂന്യ;
    char_u	*new_ts_str;		// string value of tab argument
#അല്ലെങ്കിൽ
    പൂർണ്ണസംഖ്യ		temp;
    പൂർണ്ണസംഖ്യ		new_ts = 0;
#endif
    പൂർണ്ണസംഖ്യ		save_list;
    linenr_T	first_വരി = 0;		// first changed line
    linenr_T	last_വരി = 0;		// last changed line
    പൂർണ്ണസംഖ്യ		is_indent_only = 0;	// Only process leading whitespace

    save_list = curwin->w_p_list;
    curwin->w_p_list = 0;	    // don't want list mode here

    ptr = eap->arg;
    എങ്കിൽ (STRNCMP(ptr, "-indentonly", 11) == 0 && IS_WHITE_OR_NUL(ptr[11]))
    {
	is_indent_only = 1;
	ptr = skipwhite(ptr + 11);
    }

#ifdef FEAT_VARTABS
    new_ts_str = ptr;
    എങ്കിൽ (tabstop_set(ptr, &new_vts_array) == FAIL)
	തിരികെ;
    ആകുമ്പോൾ (vim_isdigit(*ptr) || *ptr == ',')
	++ptr;

    // This ensures that either new_vts_array and new_ts_str are freshly
    // allocated, or new_vts_array points to an existing array and new_ts_str
    // is null.
    എങ്കിൽ (new_vts_array == ശൂന്യ)
    {
	new_vts_array = curbuf->b_p_vts_array;
	new_ts_str = ശൂന്യ;
    }
    അല്ലെങ്കിൽ
	new_ts_str = vim_strnsave(new_ts_str, ptr - new_ts_str);
#അല്ലെങ്കിൽ
    എങ്കിൽ (ptr[0] != NUL && (ptr[0] != '0' || ptr[1] != NUL))
    {
	char_u	*end;

	എങ്കിൽ (strtol((അക്ഷരം *)ptr, (അക്ഷരം **)&end, 10) <= 0)
	{
	    എങ്കിൽ (ptr != end)
		emsg(_(e_argument_must_be_positive));
	    അല്ലെങ്കിൽ
		semsg(_(e_invalid_argument_str), ptr);
	    തിരികെ;
	}
	new_ts = getdigits(&ptr);
	എങ്കിൽ (new_ts < 0 || new_ts > TABSTOP_MAX)
	{
	    semsg(_(e_invalid_argument_str), eap->arg);
	    തിരികെ;
	}
    }
    എങ്കിൽ (new_ts == 0)
	new_ts = curbuf->b_p_ts;
#endif
    വേണ്ടി (lnum = eap->line1; !got_int && lnum <= eap->line2; ++lnum)
    {
	ptr = ml_get(lnum);
	old_len = ml_get_len(lnum);
	col = 0;
	vcol = 0;
	did_undo = FALSE;
	വേണ്ടി (;;)
	{
	    എങ്കിൽ (VIM_ISWHITE(ptr[col]))
	    {
		എങ്കിൽ (!got_tab && അക്കം_spaces == 0)
		{
		    // First consecutive white-space
		    start_vcol = vcol;
		    start_col = col;
		}
		എങ്കിൽ (ptr[col] == ' ')
		    അക്കം_spaces++;
		അല്ലെങ്കിൽ
		    got_tab = TRUE;
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (got_tab || (eap->forceit && അക്കം_spaces > 1))
		{
		    // Retabulate this string of white-space

		    // len is virtual length of white string
		    len = അക്കം_spaces = vcol - start_vcol;
		    അക്കം_tabs = 0;
		    എങ്കിൽ (!curbuf->b_p_et)
		    {
#ifdef FEAT_VARTABS
			പൂർണ്ണസംഖ്യ t, s;

			tabstop_fromto(start_vcol, vcol,
					curbuf->b_p_ts, new_vts_array, &t, &s);
			അക്കം_tabs = t;
			അക്കം_spaces = s;
#അല്ലെങ്കിൽ
			temp = new_ts - (start_vcol % new_ts);
			എങ്കിൽ (അക്കം_spaces >= temp)
			{
			    അക്കം_spaces -= temp;
			    അക്കം_tabs++;
			}
			അക്കം_tabs += അക്കം_spaces / new_ts;
			അക്കം_spaces -= (അക്കം_spaces / new_ts) * new_ts;
#endif
		    }
		    എങ്കിൽ (curbuf->b_p_et || got_tab ||
					(അക്കം_spaces + അക്കം_tabs < len))
		    {
			എങ്കിൽ (did_undo == FALSE)
			{
			    did_undo = TRUE;
			    എങ്കിൽ (u_save((linenr_T)(lnum - 1),
						(linenr_T)(lnum + 1)) == FAIL)
			    {
				new_വരി = ശൂന്യ;	// flag out-of-memory
				നിർത്തുക;
			    }
			}

			// len is actual number of white characters used
			len = അക്കം_spaces + അക്കം_tabs;
			new_len = old_len - col + start_col + len + 1;
			എങ്കിൽ (new_len <= 0 || new_len >= MAXCOL)
			{
			    emsg_text_too_long();
			    നിർത്തുക;
			}
			new_വരി = alloc(new_len);
			എങ്കിൽ (new_വരി == ശൂന്യ)
			    നിർത്തുക;
			എങ്കിൽ (start_col > 0)
			    mch_memmove(new_വരി, ptr, (size_t)start_col);
			mch_memmove(new_വരി + start_col + len,
				      ptr + col, (size_t)(old_len - col + 1));
			ptr = new_വരി + start_col;
			വേണ്ടി (col = 0; col < len; col++)
			    ptr[col] = (col < അക്കം_tabs) ? '\t' : ' ';
			എങ്കിൽ (ml_replace(lnum, new_വരി, FALSE) == OK)
			    // "new_line" may have been copied
			    new_വരി = curbuf->b_ml.ml_വരി_ptr;
			എങ്കിൽ (first_വരി == 0)
			    first_വരി = lnum;
			last_വരി = lnum;
			ptr = new_വരി;
			old_len = new_len - 1;
			col = start_col + len;
		    }
		}
		got_tab = FALSE;
		അക്കം_spaces = 0;

		എങ്കിൽ (is_indent_only)
		    നിർത്തുക;
	    }
	    എങ്കിൽ (ptr[col] == NUL)
		നിർത്തുക;
	    vcol += chartabsize(ptr + col, (colnr_T)vcol);
	    എങ്കിൽ (vcol >= MAXCOL)
	    {
		emsg_text_too_long();
		നിർത്തുക;
	    }
	    എങ്കിൽ (has_mbyte)
		col += (*mb_ptr2len)(ptr + col);
	    അല്ലെങ്കിൽ
		++col;
	}
	എങ്കിൽ (new_വരി == ശൂന്യ)		    // out of memory
	    നിർത്തുക;
	വരി_breakcheck();
    }
    എങ്കിൽ (got_int)
	emsg(_(e_interrupted));

#ifdef FEAT_VARTABS
    // If a single value was given then it can be considered equal to
    // either the value of 'tabstop' or the value of 'vartabstop'.
    എങ്കിൽ (tabstop_എണ്ണം(curbuf->b_p_vts_array) == 0
	&& tabstop_എണ്ണം(new_vts_array) == 1
	&& curbuf->b_p_ts == tabstop_first(new_vts_array))
	; // not changed
    അല്ലെങ്കിൽ എങ്കിൽ (tabstop_എണ്ണം(curbuf->b_p_vts_array) > 0
	&& tabstop_eq(curbuf->b_p_vts_array, new_vts_array))
	; // not changed
    അല്ലെങ്കിൽ
	redraw_curbuf_later(UPD_NOT_VALID);
#അല്ലെങ്കിൽ
    എങ്കിൽ (curbuf->b_p_ts != new_ts)
	redraw_curbuf_later(UPD_NOT_VALID);
#endif
    എങ്കിൽ (first_വരി != 0)
	changed_വരികൾ(first_വരി, 0, last_വരി + 1, 0L);

    curwin->w_p_list = save_list;	// restore 'list'

#ifdef FEAT_VARTABS
    എങ്കിൽ (new_ts_str != ശൂന്യ)		// set the new tabstop
    {
	// If 'vartabstop' is in use or if the value given to retab has more
	// than one tabstop then update 'vartabstop'.
	പൂർണ്ണസംഖ്യ *old_vts_ary = curbuf->b_p_vts_array;

	എങ്കിൽ (tabstop_എണ്ണം(old_vts_ary) > 0 || tabstop_എണ്ണം(new_vts_array) > 1)
	{
	    set_string_option_direct((char_u *)"vts", -1, new_ts_str,
							OPT_FREE|OPT_LOCAL, 0);
	    curbuf->b_p_vts_array = new_vts_array;
	    vim_free(old_vts_ary);
	}
	അല്ലെങ്കിൽ
	{
	    // 'vartabstop' wasn't in use and a single value was given to
	    // retab then update 'tabstop'.
	    curbuf->b_p_ts = tabstop_first(new_vts_array);
	    vim_free(new_vts_array);
	}
	vim_free(new_ts_str);
    }
#അല്ലെങ്കിൽ
    curbuf->b_p_ts = new_ts;
#endif
    coladvance(curwin->w_curswant);

    u_clearline();
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Get indent level from 'indentexpr'.
 */
    പൂർണ്ണസംഖ്യ
get_വ്യഞ്ജകം_indent(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		indent = -1;
    char_u	*inde_copy;
    pos_T	save_pos;
    colnr_T	save_curswant;
    പൂർണ്ണസംഖ്യ		save_set_curswant;
    പൂർണ്ണസംഖ്യ		save_State;
    പൂർണ്ണസംഖ്യ		use_sandbox = was_set_insecurely(curwin,
					    (char_u *)"indentexpr", OPT_LOCAL);
    sctx_T	save_sctx = current_sctx;

    // Save and restore cursor position and curswant, in case it was changed
    // via :normal commands
    save_pos = curwin->w_cursor;
    save_curswant = curwin->w_curswant;
    save_set_curswant = curwin->w_set_curswant;
    set_vim_var_nr(VV_LNUM, curwin->w_cursor.lnum);
    എങ്കിൽ (use_sandbox)
	++sandbox;
    ++textlock;
    current_sctx = curbuf->b_p_script_ctx[BV_INDE];

    // Need to make a copy, the 'indentexpr' option could be changed while
    // evaluating it.
    inde_copy = vim_strsave(curbuf->b_p_inde);
    എങ്കിൽ (inde_copy != ശൂന്യ)
    {
	indent = (പൂർണ്ണസംഖ്യ)eval_to_സംഖ്യ(inde_copy, TRUE);
	vim_free(inde_copy);
    }

    എങ്കിൽ (use_sandbox)
	--sandbox;
    --textlock;
    current_sctx = save_sctx;

    // Restore the cursor position so that 'indentexpr' doesn't need to.
    // Pretend to be in Insert mode, allow cursor past end of line for "o"
    // command.
    save_State = State;
    State = MODE_തിരുകുക;
    curwin->w_cursor = save_pos;
    curwin->w_curswant = save_curswant;
    curwin->w_set_curswant = save_set_curswant;
    check_cursor();
    State = save_State;

    // Reset did_throw, unless 'debug' has "throw" and inside a try/catch.
    എങ്കിൽ (did_throw && (vim_strchr(p_debug, 't') == ശൂന്യ || trylevel == 0))
    {
	handle_did_throw();
	did_throw = FALSE;
    }

    // If there is an error, just keep the current indent.
    എങ്കിൽ (indent < 0)
	indent = get_indent();

    തിരികെ indent;
}
#endif

    സ്ഥിതം പൂർണ്ണസംഖ്യ
lisp_match(char_u *p)
{
    char_u	buf[LSIZE];
    പൂർണ്ണസംഖ്യ		len;
    char_u	*word = *curbuf->b_p_lw != NUL ? curbuf->b_p_lw : p_lispwords;

    ആകുമ്പോൾ (*word != NUL)
    {
	len = copy_option_part(&word, buf, LSIZE, ",");
	എങ്കിൽ (STRNCMP(buf, p, len) == 0 && IS_WHITE_OR_NUL(p[len]))
	    തിരികെ TRUE;
    }
    തിരികെ FALSE;
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
    പൂർണ്ണസംഖ്യ
get_lisp_indent(ശൂന്യം)
{
    pos_T	*pos, realpos, paren;
    പൂർണ്ണസംഖ്യ		amount;
    char_u	*that;
    colnr_T	col;
    colnr_T	firsttry;
    പൂർണ്ണസംഖ്യ		parencount, quotecount;
    പൂർണ്ണസംഖ്യ		vi_lisp;

    // Set vi_lisp to use the vi-compatible method
    vi_lisp = (vim_strchr(p_cpo, CPO_LISP) != ശൂന്യ);

    realpos = curwin->w_cursor;
    curwin->w_cursor.col = 0;

    എങ്കിൽ ((pos = findmatch(ശൂന്യ, '(')) == ശൂന്യ)
	pos = findmatch(ശൂന്യ, '[');
    അല്ലെങ്കിൽ
    {
	paren = *pos;
	pos = findmatch(ശൂന്യ, '[');
	എങ്കിൽ (pos == ശൂന്യ || LT_POSP(pos, &paren))
	    pos = &paren;
    }
    എങ്കിൽ (pos != ശൂന്യ)
    {
	// Extra trick: Take the indent of the first previous non-white
	// line that is at the same () level.
	amount = -1;
	parencount = 0;

	ആകുമ്പോൾ (--curwin->w_cursor.lnum >= pos->lnum)
	{
	    എങ്കിൽ (linewhite(curwin->w_cursor.lnum))
		തുടരുക;
	    വേണ്ടി (that = ml_get_curline(); *that != NUL; ++that)
	    {
		എങ്കിൽ (*that == ';')
		{
		    ആകുമ്പോൾ (*(that + 1) != NUL)
			++that;
		    തുടരുക;
		}
		എങ്കിൽ (*that == '\\')
		{
		    എങ്കിൽ (*(that + 1) != NUL)
			++that;
		    തുടരുക;
		}
		എങ്കിൽ (*that == '"' && *(that + 1) != NUL)
		{
		    ആകുമ്പോൾ (*++that && *that != '"')
		    {
			// skipping escaped characters in the string
			എങ്കിൽ (*that == '\\')
			{
			    എങ്കിൽ (*++that == NUL)
				നിർത്തുക;
			    എങ്കിൽ (that[1] == NUL)
			    {
				++that;
				നിർത്തുക;
			    }
			}
		    }
		    എങ്കിൽ (*that == NUL)
			നിർത്തുക;
		}
		എങ്കിൽ (*that == '(' || *that == '[')
		    ++parencount;
		അല്ലെങ്കിൽ എങ്കിൽ (*that == ')' || *that == ']')
		    --parencount;
	    }
	    എങ്കിൽ (parencount == 0)
	    {
		amount = get_indent();
		നിർത്തുക;
	    }
	}

	എങ്കിൽ (amount == -1)
	{
	    curwin->w_cursor.lnum = pos->lnum;
	    curwin->w_cursor.col = pos->col;
	    col = pos->col;

	    that = ml_get_curline();

	    എങ്കിൽ (vi_lisp && get_indent() == 0)
		amount = 2;
	    അല്ലെങ്കിൽ
	    {
		char_u		*വരി = that;
		chartabsize_T	cts;

		init_chartabsize_arg(&cts, curwin, pos->lnum, 0, വരി, വരി);
		ആകുമ്പോൾ (*cts.cts_ptr != NUL && col > 0)
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

		എങ്കിൽ (!vi_lisp && (*that == '(' || *that == '[')
						      && lisp_match(that + 1))
		    amount += 2;
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (*that != NUL)
		    {
			that++;
			amount++;
		    }
		    firsttry = amount;

		    init_chartabsize_arg(&cts, curwin, (colnr_T)(that - വരി),
							   amount, വരി, that);
		    ആകുമ്പോൾ (VIM_ISWHITE(*cts.cts_ptr))
		    {
			cts.cts_vcol += lbr_chartabsize(&cts);
			++cts.cts_ptr;
		    }
		    that = cts.cts_ptr;
		    amount = cts.cts_vcol;
		    clear_chartabsize_arg(&cts);

		    എങ്കിൽ (*that && *that != ';') // not a comment line
		    {
			// test *that != '(' to accommodate first let/do
			// argument if it is more than one line
			എങ്കിൽ (!vi_lisp && *that != '(' && *that != '[')
			    firsttry++;

			parencount = 0;
			quotecount = 0;

			init_chartabsize_arg(&cts, curwin,
				   (colnr_T)(that - വരി), amount, വരി, that);
			എങ്കിൽ (vi_lisp
				|| (*that != '"'
				    && *that != '\''
				    && *that != '#'
				    && (*that < '0' || *that > '9')))
			{
			    ആകുമ്പോൾ (*cts.cts_ptr
				    && (!VIM_ISWHITE(*cts.cts_ptr)
					|| quotecount
					|| parencount)
				    && (!((*cts.cts_ptr == '('
							|| *cts.cts_ptr == '[')
					    && !quotecount
					    && !parencount
					    && vi_lisp)))
			    {
				എങ്കിൽ (*cts.cts_ptr == '"')
				    quotecount = !quotecount;
				എങ്കിൽ ((*cts.cts_ptr == '(' || *cts.cts_ptr == '[')
							       && !quotecount)
				    ++parencount;
				എങ്കിൽ ((*cts.cts_ptr == ')' || *cts.cts_ptr == ']')
							       && !quotecount)
				    --parencount;
				എങ്കിൽ (*cts.cts_ptr == '\\'
						    && *(cts.cts_ptr+1) != NUL)
				    cts.cts_vcol += lbr_chartabsize_adv(&cts);
				cts.cts_vcol += lbr_chartabsize_adv(&cts);
			    }
			}
			ആകുമ്പോൾ (VIM_ISWHITE(*cts.cts_ptr))
			{
			    cts.cts_vcol += lbr_chartabsize(&cts);
			    ++cts.cts_ptr;
			}
			that = cts.cts_ptr;
			amount = cts.cts_vcol;
			clear_chartabsize_arg(&cts);

			എങ്കിൽ (!*that || *that == ';')
			    amount = firsttry;
		    }
		}
	    }
	}
    }
    അല്ലെങ്കിൽ
	amount = 0;	// no matching '(' or '[' found, use zero indent

    curwin->w_cursor = realpos;

    തിരികെ amount;
}

/*
 * Re-indent the current line, based on the current contents of it and the
 * surrounding lines. Fixing the cursor position seems really easy -- I'm very
 * confused what all the part that handles Control-T is doing that I'm not.
 * "get_the_indent" should be get_c_indent, get_expr_indent or get_lisp_indent.
 */

    ശൂന്യം
fixthisline(പൂർണ്ണസംഖ്യ (*get_the_indent)(ശൂന്യം))
{
    പൂർണ്ണസംഖ്യ amount = get_the_indent();

    എങ്കിൽ (amount < 0)
	തിരികെ;

    change_indent(INDENT_SET, amount, FALSE, 0, TRUE);
    എങ്കിൽ (linewhite(curwin->w_cursor.lnum))
	did_ai = TRUE;	// delete the indent if the line stays empty
}

/*
 * Return TRUE if 'indentexpr' should be used for Lisp indenting.
 * Caller may want to check 'autoindent'.
 */
    പൂർണ്ണസംഖ്യ
use_indentexpr_for_lisp(ശൂന്യം)
{
#ifdef FEAT_EVAL
    തിരികെ curbuf->b_p_lisp
		&& *curbuf->b_p_inde != NUL
		&& STRCMP(curbuf->b_p_lop, "expr:1") == 0;
#അല്ലെങ്കിൽ
    തിരികെ FALSE;
#endif
}

/*
 * Fix indent for 'lisp' and 'cindent'.
 */
    ശൂന്യം
fix_indent(ശൂന്യം)
{
    എങ്കിൽ (p_paste)
	തിരികെ;  // no auto-indenting when 'paste' is set
    എങ്കിൽ (curbuf->b_p_lisp && curbuf->b_p_ai)
    {
	എങ്കിൽ (use_indentexpr_for_lisp())
	    do_c_വ്യഞ്ജകം_indent();
	അല്ലെങ്കിൽ
	    fixthisline(get_lisp_indent);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (cindent_on())
	do_c_വ്യഞ്ജകം_indent();
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * "indent()" function
 */
    ശൂന്യം
f_indent(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    എങ്കിൽ (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	തിരികെ;

    lnum = tv_get_lnum(argvars);
    എങ്കിൽ (lnum >= 1 && lnum <= curbuf->b_ml.ml_വരി_എണ്ണം)
	rettv->vval.v_സംഖ്യ = get_indent_lnum(lnum);
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (in_vim9script())
	    semsg(_(e_invalid_വരി_സംഖ്യ_nr), lnum);
	rettv->vval.v_സംഖ്യ = -1;
    }
}

/*
 * "lispindent(lnum)" function
 */
    ശൂന്യം
f_lispindent(typval_T *argvars UNUSED, typval_T *rettv)
{
    pos_T	pos;
    linenr_T	lnum;

    എങ്കിൽ (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	തിരികെ;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    എങ്കിൽ (lnum >= 1 && lnum <= curbuf->b_ml.ml_വരി_എണ്ണം)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_സംഖ്യ = get_lisp_indent();
	curwin->w_cursor = pos;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (in_vim9script())
	semsg(_(e_invalid_വരി_സംഖ്യ_nr), lnum);
    അല്ലെങ്കിൽ
	rettv->vval.v_സംഖ്യ = -1;
}
#endif
