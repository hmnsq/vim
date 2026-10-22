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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_VARTABS)

/*
 * Set the integer values corresponding to the string setting of 'vartabstop'.
 * "array" will be set, caller must free it if needed.
 * Return FAIL for an error.
 */
    முழு
tabstop_set(char_u *var, முழு **array)
{
    முழு	    valcount = 1;
    முழு	    t;
    char_u  *cp;

    எனில் (var[0] == NUL || (var[0] == '0' && var[1] == NUL))
    {
	*array = வெற்று;
	திரும்பு OK;
    }

    க்கு (cp = var; *cp != NUL; ++cp)
    {
	எனில் (cp == var || cp[-1] == ',')
	{
	    char_u *end;

	    எனில் (strtol((எழுத்து *)cp, (எழுத்து **)&end, 10) <= 0)
	    {
		எனில் (cp != end)
		    emsg(_(e_argument_must_be_positive));
		இல்லையெனில்
		    semsg(_(e_invalid_argument_str), cp);
		திரும்பு FAIL;
	    }
	}

	எனில் (VIM_ISDIGIT(*cp))
	    தொடர்;
	எனில் (cp[0] == ',' && cp > var && cp[-1] != ',' && cp[1] != NUL)
	{
	    ++valcount;
	    தொடர்;
	}
	semsg(_(e_invalid_argument_str), var);
	திரும்பு FAIL;
    }

    *array = ALLOC_MULT(முழு, valcount + 1);
    எனில் (*array == வெற்று)
	திரும்பு FAIL;
    (*array)[0] = valcount;

    t = 1;
    க்கு (cp = var; *cp != NUL;)
    {
	முழு n = atoi((எழுத்து *)cp);

	// Catch negative values, overflow and ridiculous big values.
	எனில் (n <= 0 || n > TABSTOP_MAX)
	{
	    semsg(_(e_invalid_argument_str), cp);
	    VIM_CLEAR(*array);
	    திரும்பு FAIL;
	}
	(*array)[t++] = n;
	போது (*cp != NUL && *cp != ',')
	    ++cp;
	எனில் (*cp != NUL)
	    ++cp;
    }

    திரும்பு OK;
}

/*
 * Calculate the number of screen spaces a tab will occupy.
 * If "vts" is set then the tab widths are taken from that array,
 * otherwise the value of ts is used.
 */
    முழு
tabstop_padding(colnr_T col, முழு ts_arg, முழு *vts)
{
    முழு		ts = ts_arg == 0 ? 8 : ts_arg;
    முழு		tabcount;
    colnr_T	tabcol = 0;
    முழு		t;
    முழு		padding = 0;

    எனில் (vts == வெற்று || vts[0] == 0)
	திரும்பு ts - (col % ts);

    tabcount = vts[0];

    க்கு (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	எனில் (tabcol > col)
	{
	    padding = (முழு)(tabcol - col);
	    நிறுத்து;
	}
    }
    எனில் (t > tabcount)
	padding = vts[tabcount] - (முழு)((col - tabcol) % vts[tabcount]);

    திரும்பு padding;
}

/*
 * Find the size of the tab that covers a particular column.
 *
 * If this is being called as part of a shift operation, col is not the cursor
 * column but is the column number to the left of the first non-whitespace
 * character in the line.  If the shift is to the left (left = TRUE), then
 * return the size of the tab interval to the left of the column.
 */
    முழு
tabstop_at(colnr_T col, முழு ts, முழு *vts, முழு இடது)
{
    முழு		tabcount;	// Number of tab stops in the list of variable
				// tab stops.
    colnr_T	tabcol = 0;	// Column of the tab stop under consideration.
    முழு		t;		// Tabstop index in the list of variable tab
				// stops.
    முழு		tab_size = 0;	// Size of the tab stop interval to the right
				// or left of the col.

    எனில் (vts == 0 || vts[0] == 0)
	திரும்பு ts;

    tabcount = vts[0];
    க்கு (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	எனில் (tabcol > col)
	{
	    // If shifting left (left != 0), and if the column to the left of
	    // the first first non-blank character (col) in the line is
	    // already to the left of the first tabstop, set the shift amount
	    // (tab_size) to just enough to shift the line to the left margin.
	    // The value doesn't seem to matter as long as it is at least that
	    // distance.
	    எனில் (இடது && (t == 1))
		tab_size = col;
	    இல்லையெனில்
		tab_size = vts[t - (இடது ? 1 : 0)];
	    நிறுத்து;
	}
    }
    எனில் (t > tabcount)		// If the value of the index t is beyond the
				// end of the list, use the tab stop value at
				// the end of the list.
	tab_size = vts[tabcount];

    திரும்பு tab_size;
}

/*
 * Find the column on which a tab starts.
 */
    colnr_T
tabstop_start(colnr_T col, முழு ts, முழு *vts)
{
    முழு		tabcount;
    colnr_T	tabcol = 0;
    முழு		t;
    முழு		excess;

    எனில் (vts == வெற்று || vts[0] == 0)
	திரும்பு col - col % ts;

    tabcount = vts[0];
    க்கு (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	எனில் (tabcol > col)
	    திரும்பு tabcol - vts[t];
    }

    excess = tabcol % vts[tabcount];
    திரும்பு col - (col - excess) % vts[tabcount];
}

/*
 * Find the number of tabs and spaces necessary to get from one column
 * to another.
 */
    வெறுமை
tabstop_fromto(
	colnr_T start_col,
	colnr_T end_col,
	முழு	ts_arg,
	முழு	*vts,
	முழு	*ntabs,
	முழு	*nspcs)
{
    முழு		spaces = end_col - start_col;
    colnr_T	tabcol = 0;
    முழு		padding = 0;
    முழு		tabcount;
    முழு		t;
    முழு		ts = ts_arg == 0 ? curbuf->b_p_ts : ts_arg;

    எனில் (vts == வெற்று || vts[0] == 0)
    {
	முழு tabs = 0;
	முழு initspc = 0;

	initspc = ts - (start_col % ts);
	எனில் (spaces >= initspc)
	{
	    spaces -= initspc;
	    tabs++;
	}
	tabs += spaces / ts;
	spaces -= (spaces / ts) * ts;

	*ntabs = tabs;
	*nspcs = spaces;
	திரும்பு;
    }

    // Find the padding needed to reach the next tabstop.
    tabcount = vts[0];
    க்கு (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	எனில் (tabcol > start_col)
	{
	    padding = (முழு)(tabcol - start_col);
	    நிறுத்து;
	}
    }
    எனில் (t > tabcount)
	padding = vts[tabcount] - (முழு)((start_col - tabcol) % vts[tabcount]);

    // If the space needed is less than the padding no tabs can be used.
    எனில் (spaces < padding)
    {
	*ntabs = 0;
	*nspcs = spaces;
	திரும்பு;
    }

    *ntabs = 1;
    spaces -= padding;

    // At least one tab has been used. See if any more will fit.
    போது (spaces != 0 && ++t <= tabcount)
    {
	padding = vts[t];
	எனில் (spaces < padding)
	{
	    *nspcs = spaces;
	    திரும்பு;
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
    நிலையான முழு
tabstop_eq(முழு *ts1, முழு *ts2)
{
    முழு		t;

    எனில் ((ts1 == 0 && ts2) || (ts1 && ts2 == 0))
	திரும்பு FALSE;
    எனில் (ts1 == ts2)
	திரும்பு TRUE;
    எனில் (ts1[0] != ts2[0])
	திரும்பு FALSE;

    க்கு (t = 1; t <= ts1[0]; ++t)
	எனில் (ts1[t] != ts2[t])
	    திரும்பு FALSE;

    திரும்பு TRUE;
}

# எனில் defined(FEAT_BEVAL)
/*
 * Copy a tabstop array, allocating space for the new array.
 */
    முழு *
tabstop_copy(முழு *oldts)
{
    முழு		*newts;
    முழு		t;

    எனில் (oldts == வெற்று)
	திரும்பு வெற்று;
    newts = ALLOC_MULT(முழு, oldts[0] + 1);
    எனில் (newts != வெற்று)
	க்கு (t = 0; t <= oldts[0]; ++t)
	    newts[t] = oldts[t];
    திரும்பு newts;
}
# endif

/*
 * Return a count of the number of tabstops.
 */
    முழு
tabstop_மொத்தம்(முழு *ts)
{
    திரும்பு ts != வெற்று ? ts[0] : 0;
}

/*
 * Return the first tabstop, or 8 if there are no tabstops defined.
 */
    முழு
tabstop_first(முழு *ts)
{
    திரும்பு ts != வெற்று ? ts[1] : 8;
}

#endif

/*
 * Return the effective shiftwidth value for current buffer, using the
 * 'tabstop' value when 'shiftwidth' is zero.
 */
    long
get_sw_மதிப்பு(buf_T *buf)
{
    திரும்பு get_sw_மதிப்பு_col(buf, 0, FALSE);
}

/*
 * Idem, using "pos".
 */
    நிலையான long
get_sw_மதிப்பு_pos(buf_T *buf, pos_T *pos, முழு இடது)
{
    pos_T save_cursor = curwin->w_cursor;
    long sw_மதிப்பு;

    curwin->w_cursor = *pos;
    sw_மதிப்பு = get_sw_மதிப்பு_col(buf, get_nolist_virtcol(), இடது);
    curwin->w_cursor = save_cursor;
    திரும்பு sw_மதிப்பு;
}

/*
 * Idem, using the first non-black in the current line.
 */
    long
get_sw_மதிப்பு_indent(buf_T *buf, முழு இடது)
{
    pos_T pos = curwin->w_cursor;

    pos.col = getwhitecols_curline();
    திரும்பு get_sw_மதிப்பு_pos(buf, &pos, இடது);
}

/*
 * Idem, using virtual column "col".
 */
    long
get_sw_மதிப்பு_col(buf_T *buf, colnr_T col UNUSED, முழு இடது UNUSED)
{
    திரும்பு buf->b_p_sw ? buf->b_p_sw :
#ifdef FEAT_VARTABS
	tabstop_at(col, buf->b_p_ts, buf->b_p_vts_array, இடது);
#இல்லையெனில்
	buf->b_p_ts;
#endif
}

/*
 * Return the effective softtabstop value for the current buffer, using the
 * 'shiftwidth' value when 'softtabstop' is negative.
 */
    long
get_sts_மதிப்பு(வெறுமை)
{
    திரும்பு curbuf->b_p_sts < 0 ? get_sw_மதிப்பு(curbuf) : curbuf->b_p_sts;
}

/*
 * Count the size (in window cells) of the indent in the current line.
 */
    முழு
get_indent(வெறுமை)
{
#ifdef FEAT_VARTABS
    திரும்பு get_indent_str_vtab(ml_get_curline(), (முழு)curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#இல்லையெனில்
    திரும்பு get_indent_str(ml_get_curline(), (முழு)curbuf->b_p_ts, FALSE);
#endif
}

/*
 * Count the size (in window cells) of the indent in line "lnum".
 */
    முழு
get_indent_lnum(linenr_T lnum)
{
#ifdef FEAT_VARTABS
    திரும்பு get_indent_str_vtab(ml_get(lnum), (முழு)curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#இல்லையெனில்
    திரும்பு get_indent_str(ml_get(lnum), (முழு)curbuf->b_p_ts, FALSE);
#endif
}

#எனில் defined(FEAT_FOLDING)
/*
 * Count the size (in window cells) of the indent in line "lnum" of buffer
 * "buf".
 */
    முழு
get_indent_buf(buf_T *buf, linenr_T lnum)
{
# ifdef FEAT_VARTABS
    திரும்பு get_indent_str_vtab(ml_get_buf(buf, lnum, FALSE),
			       (முழு)buf->b_p_ts, buf->b_p_vts_array, FALSE);
# இல்லையெனில்
    திரும்பு get_indent_str(ml_get_buf(buf, lnum, FALSE), (முழு)buf->b_p_ts, FALSE);
# endif
}
#endif

/*
 * count the size (in window cells) of the indent in line "ptr", with
 * 'tabstop' at "ts"
 */
    முழு
get_indent_str(
    char_u	*ptr,
    முழு		ts,
    முழு		no_ts) // if TRUE, count a tab as ^I
{
    முழு		மொத்தம் = 0;

    க்கு ( ; *ptr; ++ptr)
    {
	எனில் (*ptr == TAB)    // count a tab for what it is worth
	{
	    எனில் (!no_ts)
		மொத்தம் += ts - (மொத்தம் % ts);
	    இல்லையெனில்
		மொத்தம் += ptr2cells(ptr);
	}
	இல்லையெனில் எனில் (*ptr == ' ')
	    ++மொத்தம்;		// count a space for one
	இல்லையெனில்
	    நிறுத்து;
    }
    திரும்பு மொத்தம்;
}

#எனில் defined(FEAT_VARTABS)
/*
 * Count the size (in window cells) of the indent in line "ptr", using
 * variable tabstops.
 * If "no_ts" is TRUE, count a tab as ^I.
 */
    முழு
get_indent_str_vtab(char_u *ptr, முழு ts, முழு *vts, முழு no_ts)
{
    முழு		மொத்தம் = 0;

    க்கு ( ; *ptr; ++ptr)
    {
	எனில் (*ptr == TAB)    // count a tab for what it is worth
	{
	    எனில் (!no_ts)
		மொத்தம் += tabstop_padding(மொத்தம், ts, vts);
	    இல்லையெனில்
		மொத்தம் += ptr2cells(ptr);
	}
	இல்லையெனில் எனில் (*ptr == ' ')
	    ++மொத்தம்;		// count a space for one
	இல்லையெனில்
	    நிறுத்து;
    }
    திரும்பு மொத்தம்;
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
    முழு
set_indent(
    முழு		size,		    // measured in spaces
    முழு		flags)
{
    char_u	*p;
    char_u	*newline;
    char_u	*oldline;
    char_u	*s;
    முழு		todo;
    முழு		ind_len;	    // measured in characters
    முழு		வரி_len;	    // size of the line (including the NUL)
    முழு		doit = FALSE;
    முழு		ind_done = 0;	    // measured in spaces
#ifdef FEAT_VARTABS
    முழு		ind_col = 0;
#endif
    முழு		tab_pad;
    முழு		retval = FALSE;
    முழு		orig_char_len = -1; // number of initial whitespace chars when
				    // 'et' and 'pi' are both set

    // First check if there is anything to do and compute the number of
    // characters needed for the indent.
    todo = size;
    ind_len = 0;
    p = oldline = ml_get_curline();
    வரி_len = ml_get_curline_len() + 1;

    // Calculate the buffer size for the new indent, and check to see if it
    // isn't already set

    // if 'expandtab' isn't set: use TABs; if both 'expandtab' and
    // 'preserveindent' are set count the number of characters at the
    // beginning of the line to be copied
    எனில் (!curbuf->b_p_et || (!(flags & SIN_செருகு) && curbuf->b_p_pi))
    {
	// If 'preserveindent' is set then reuse as much as possible of
	// the existing indent structure for the new indent
	எனில் (!(flags & SIN_செருகு) && curbuf->b_p_pi)
	{
	    ind_done = 0;

	    // count as many characters as we can use
	    போது (todo > 0 && VIM_ISWHITE(*p))
	    {
		எனில் (*p == TAB)
		{
#ifdef FEAT_VARTABS
		    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#இல்லையெனில்
		    tab_pad = (முழு)curbuf->b_p_ts
					   - (ind_done % (முழு)curbuf->b_p_ts);
#endif
		    // stop if this tab will overshoot the target
		    எனில் (todo < tab_pad)
			நிறுத்து;
		    todo -= tab_pad;
		    ++ind_len;
		    ind_done += tab_pad;
		}
		இல்லையெனில்
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
	    எனில் (curbuf->b_p_et)
		orig_char_len = ind_len;

	    // Fill to next tabstop with a tab, if possible
#ifdef FEAT_VARTABS
	    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
						curbuf->b_p_vts_array);
#இல்லையெனில்
	    tab_pad = (முழு)curbuf->b_p_ts - (ind_done % (முழு)curbuf->b_p_ts);
#endif
	    எனில் (todo >= tab_pad && orig_char_len == -1)
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
	க்கு (;;)
	{
	    tab_pad = tabstop_padding(ind_col, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
	    எனில் (todo < tab_pad)
		நிறுத்து;
	    எனில் (*p != TAB)
		doit = TRUE;
	    இல்லையெனில்
		++p;
	    todo -= tab_pad;
	    ++ind_len;
	    ind_col += tab_pad;
	}
#இல்லையெனில்
	போது (todo >= (முழு)curbuf->b_p_ts)
	{
	    எனில் (*p != TAB)
		doit = TRUE;
	    இல்லையெனில்
		++p;
	    todo -= (முழு)curbuf->b_p_ts;
	    ++ind_len;
	    // ind_done += (int)curbuf->b_p_ts;
	}
#endif
    }
    // count spaces required for indent
    போது (todo > 0)
    {
	எனில் (*p != ' ')
	    doit = TRUE;
	இல்லையெனில்
	    ++p;
	--todo;
	++ind_len;
	// ++ind_done;
    }

    // Return if the indent is OK already.
    எனில் (!doit && !VIM_ISWHITE(*p) && !(flags & SIN_செருகு))
	திரும்பு FALSE;

    // Allocate memory for the new line.
    எனில் (flags & SIN_செருகு)
	p = oldline;
    இல்லையெனில்
    {
	p = skipwhite(p);
	வரி_len -= (முழு)(p - oldline);
    }

    // If 'preserveindent' and 'expandtab' are both set keep the original
    // characters and allocate accordingly.  We will fill the rest with spaces
    // after the if (!curbuf->b_p_et) below.
    எனில் (orig_char_len != -1)
    {
	newline = alloc(orig_char_len + size - ind_done + வரி_len);
	எனில் (newline == வெற்று)
	    திரும்பு FALSE;
	todo = size - ind_done;
	ind_len = orig_char_len + todo;    // Set total length of indent in
					   // characters, which may have been
					   // undercounted until now
	p = oldline;
	s = newline;
	போது (orig_char_len > 0)
	{
	    *s++ = *p++;
	    orig_char_len--;
	}

	// Skip over any additional white space (useful when newindent is less
	// than old)
	போது (VIM_ISWHITE(*p))
	    ++p;

    }
    இல்லையெனில்
    {
	todo = size;
	newline = alloc(ind_len + வரி_len);
	எனில் (newline == வெற்று)
	    திரும்பு FALSE;
	s = newline;
    }

    // Put the characters in the new line.
    // if 'expandtab' isn't set: use TABs
    எனில் (!curbuf->b_p_et)
    {
	// If 'preserveindent' is set then reuse as much as possible of
	// the existing indent structure for the new indent
	எனில் (!(flags & SIN_செருகு) && curbuf->b_p_pi)
	{
	    p = oldline;
	    ind_done = 0;

	    போது (todo > 0 && VIM_ISWHITE(*p))
	    {
		எனில் (*p == TAB)
		{
#ifdef FEAT_VARTABS
		    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#இல்லையெனில்
		    tab_pad = (முழு)curbuf->b_p_ts
					   - (ind_done % (முழு)curbuf->b_p_ts);
#endif
		    // stop if this tab will overshoot the target
		    எனில் (todo < tab_pad)
			நிறுத்து;
		    todo -= tab_pad;
		    ind_done += tab_pad;
		}
		இல்லையெனில்
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
#இல்லையெனில்
	    tab_pad = (முழு)curbuf->b_p_ts - (ind_done % (முழு)curbuf->b_p_ts);
#endif
	    எனில் (todo >= tab_pad)
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
	க்கு (;;)
	{
	    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
	    எனில் (todo < tab_pad)
		நிறுத்து;
	    *s++ = TAB;
	    todo -= tab_pad;
	    ind_done += tab_pad;
	}
#இல்லையெனில்
	போது (todo >= (முழு)curbuf->b_p_ts)
	{
	    *s++ = TAB;
	    todo -= (முழு)curbuf->b_p_ts;
	}
#endif
    }
    போது (todo > 0)
    {
	*s++ = ' ';
	--todo;
    }
    mch_memmove(s, p, (size_t)வரி_len);

    // Replace the line (unless undo fails).
    எனில் (!(flags & SIN_UNDO) || u_savesub(curwin->w_cursor.lnum) == OK)
    {
	colnr_T old_offset = (colnr_T)(p - oldline);
	colnr_T new_offset = (colnr_T)(s - newline);

	// this may free "newline"
	ml_replace(curwin->w_cursor.lnum, newline, FALSE);
	எனில் (flags & SIN_CHANGED)
	    changed_bytes(curwin->w_cursor.lnum, 0);

	// Correct saved cursor position if it is in this line.
	எனில் (saved_cursor.lnum == curwin->w_cursor.lnum)
	{
	    எனில் (saved_cursor.col >= old_offset)
		// cursor was after the indent, adjust for the number of
		// bytes added/removed
		saved_cursor.col += ind_len - old_offset;
	    இல்லையெனில் எனில் (saved_cursor.col >= new_offset)
		// cursor was in the indent, and is now after it, put it back
		// at the start of the indent (replacing spaces with TAB)
		saved_cursor.col = new_offset;
	}
#ifdef FEAT_PROP_POPUP
	எனில் (curbuf->b_has_textprop)
	{
	    முழு added = ind_len - old_offset;

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
    இல்லையெனில்
	vim_free(newline);

    curwin->w_cursor.col = ind_len;
    திரும்பு retval;
}

/*
 * Return the indent of the current line after a number.  Return -1 if no
 * number was found.  Used for 'n' in 'formatoptions': numbered list.
 * Since a pattern is used it can actually handle more than numbers.
 */
    முழு
get_எண்_indent(linenr_T lnum)
{
    colnr_T	col;
    pos_T	pos;

    regmatch_T	regmatch;
    முழு		lead_len = 0;	// length of comment leader

    எனில் (lnum > curbuf->b_ml.ml_வரி_மொத்தம்)
	திரும்பு -1;
    pos.lnum = 0;

    // In format_lines() (i.e. not insert mode), fo+=q is needed too...
    எனில் ((State & MODE_செருகு) || has_format_option(FO_Q_COMS))
	lead_len = get_leader_len(ml_get(lnum), வெற்று, FALSE, TRUE);

    regmatch.regprog = vim_regcomp(curbuf->b_p_flp, RE_MAGIC);
    எனில் (regmatch.regprog != வெற்று)
    {
	regmatch.rm_ic = FALSE;

	// vim_regexec() expects a pointer to a line.  This lets us
	// start matching for the flp beyond any comment leader...
	எனில் (vim_regexec(&regmatch, ml_get(lnum) + lead_len, (colnr_T)0))
	{
	    pos.lnum = lnum;
	    pos.col = (colnr_T)(*regmatch.endp - ml_get(lnum));
	    pos.coladd = 0;
	}
	vim_regfree(regmatch.regprog);
    }

    எனில் (pos.lnum == 0 || *ml_get_pos(&pos) == NUL)
	திரும்பு -1;
    getvcol(curwin, &pos, &col, வெற்று, வெற்று, 0);
    திரும்பு (முழு)col;
}

#எனில் defined(FEAT_LINEBREAK)
/*
 * Check "briopt" as 'breakindentopt' and update the members of "wp".
 * This is called when 'breakindentopt' is changed and when a window is
 * initialized.
 * Returns FAIL for failure, OK otherwise.
 */
    முழு
briopt_check(
    char_u	*briopt,	// when NULL: use "wp->w_p_briopt"
    win_T	*wp)		// when NULL: only check "briopt"
{
    char_u	*p;
    முழு		bri_shift = 0;
    long	bri_min = 20;
    முழு		bri_sbr = FALSE;
    முழு		bri_list = 0;
    முழு		bri_vcol = 0;

    எனில் (briopt != வெற்று)
	p = briopt;
    இல்லையெனில்
	p = wp->w_p_briopt;

    போது (*p != NUL)
    {
	// Note: Keep this in sync with p_briopt_values
	எனில் (STRNCMP(p, "shift:", 6) == 0
		 && ((p[6] == '-' && VIM_ISDIGIT(p[7])) || VIM_ISDIGIT(p[6])))
	{
	    p += 6;
	    bri_shift = getdigits(&p);
	}
	இல்லையெனில் எனில் (STRNCMP(p, "min:", 4) == 0 && VIM_ISDIGIT(p[4]))
	{
	    p += 4;
	    bri_min = getdigits(&p);
	}
	இல்லையெனில் எனில் (STRNCMP(p, "sbr", 3) == 0)
	{
	    p += 3;
	    bri_sbr = TRUE;
	}
	இல்லையெனில் எனில் (STRNCMP(p, "list:", 5) == 0)
	{
	    p += 5;
	    bri_list = getdigits(&p);
	}
	இல்லையெனில் எனில் (STRNCMP(p, "column:", 7) == 0)
	{
	    p += 7;
	    bri_vcol = getdigits(&p);
	}
	எனில் (*p != ',' && *p != NUL)
	    திரும்பு FAIL;
	எனில் (*p == ',')
	    ++p;
    }

    எனில் (wp == வெற்று)
	திரும்பு OK;

    wp->w_briopt_shift = bri_shift;
    wp->w_briopt_min   = bri_min;
    wp->w_briopt_sbr   = bri_sbr;
    wp->w_briopt_list  = bri_list;
    wp->w_briopt_vcol  = bri_vcol;

    திரும்பு OK;
}

/*
 * Return appropriate space number for breakindent, taking influencing
 * parameters into account. Window must be specified, since it is not
 * necessarily always the current one.
 */
    முழு
get_breakindent_win(
    win_T	*wp,
    char_u	*வரி) // start of the line
{
    நிலையான முழு	    prev_indent = 0;	// cached indent value
    நிலையான long	    prev_ts     = 0L;	// cached tabstop value
# ifdef FEAT_VARTABS
    நிலையான முழு      *prev_vts = வெற்று;   // cached vartabs values
# endif
    நிலையான முழு	    prev_fnum   = 0;	// cached buffer number
    நிலையான char_u   *prev_வரி  = வெற்று;	// cached copy of "line"
    நிலையான varnumber_T prev_tick = 0;   // changedtick of cached value
    நிலையான முழு      prev_list = 0;	// cached list indent
    நிலையான முழு      prev_listopt = 0;	// cached w_p_briopt_list value
    நிலையான முழு      prev_no_ts = FALSE;	// cached no_ts value
    நிலையான unsigned prev_dy_uhex = 0;	// cached 'display' "uhex" value
    // cached formatlistpat value
    நிலையான char_u   *prev_flp = வெற்று;
    முழு		    bri = 0;
    // window width minus window margin space, i.e. what rests for text
    மாறிலி முழு	    eff_wwidth = wp->w_width
					  - win_col_off(wp) + win_col_off2(wp);

    // In list mode, if 'listchars' "tab" isn't set, a TAB is displayed as ^I.
    முழு no_ts = wp->w_p_list && wp->w_lcs_chars.tab1 == NUL;

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
    எனில் (prev_fnum != wp->w_buffer->b_fnum
	    || prev_ts != wp->w_buffer->b_p_ts
# ifdef FEAT_VARTABS
	    || prev_vts != wp->w_buffer->b_p_vts_array
# endif
	    || prev_tick != CHANGEDTICK(wp->w_buffer)
	    || prev_listopt != wp->w_briopt_list
	    || prev_no_ts != no_ts
	    || prev_dy_uhex != (dy_flags & DY_UHEX)
	    || prev_flp == வெற்று
	    || STRCMP(prev_flp, get_flp_மதிப்பு(wp->w_buffer)) != 0
	    || prev_வரி == வெற்று || STRCMP(prev_வரி, வரி) != 0
	)
    {
	prev_fnum = wp->w_buffer->b_fnum;
	vim_free(prev_வரி);
	prev_வரி = vim_strsave(வரி);
	prev_ts = wp->w_buffer->b_p_ts;
# ifdef FEAT_VARTABS
	prev_vts = wp->w_buffer->b_p_vts_array;
	எனில் (wp->w_briopt_vcol == 0)
	    prev_indent = get_indent_str_vtab(வரி,
				     (முழு)wp->w_buffer->b_p_ts,
				      wp->w_buffer->b_p_vts_array, no_ts);
# இல்லையெனில்
	எனில் (wp->w_briopt_vcol == 0)
	    prev_indent = get_indent_str(வரி,
					(முழு)wp->w_buffer->b_p_ts, no_ts);
# endif
	prev_tick = CHANGEDTICK(wp->w_buffer);
	prev_listopt = wp->w_briopt_list;
	prev_list = 0;
	prev_no_ts = no_ts;
	prev_dy_uhex = (dy_flags & DY_UHEX);
	vim_free(prev_flp);
	prev_flp = vim_strsave(get_flp_மதிப்பு(wp->w_buffer));
	// add additional indent for numbered lists
	எனில் (wp->w_briopt_list != 0 && wp->w_briopt_vcol == 0)
	{
	    regmatch_T	    regmatch;

	    regmatch.regprog = vim_regcomp(prev_flp,
				   RE_MAGIC + RE_STRING + RE_AUTO + RE_STRICT);

	    எனில் (regmatch.regprog != வெற்று)
	    {
		regmatch.rm_ic = FALSE;
		எனில் (vim_regexec(&regmatch, வரி, 0))
		{
		    எனில் (wp->w_briopt_list > 0)
			prev_list = wp->w_briopt_list;
		    இல்லையெனில்
		    {
			char_u	*ptr = *regmatch.startp;
			char_u	*end_ptr = *regmatch.endp;
			முழு	indent = 0;

			// Compute the width of the matched text.
			// Use win_chartabsize() so that TAB size is correct,
			// while wrapping is ignored.
			போது (ptr < end_ptr)
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
    எனில் (wp->w_briopt_vcol != 0)
    {
	// column value has priority
	bri = wp->w_briopt_vcol;
	prev_list = 0;
    }
    இல்லையெனில்
	bri = prev_indent + wp->w_briopt_shift;

    // Add offset for number column, if 'n' is in 'cpoptions'
    bri += win_col_off2(wp);

    // add additional indent for numbered lists
    எனில் (wp->w_briopt_list > 0)
	bri += prev_list;

    // indent minus the length of the showbreak string
    எனில் (wp->w_briopt_sbr)
	bri -= vim_strsize(get_showbreak_மதிப்பு(wp));


    // never indent past left window margin
    எனில் (bri < 0)
	bri = 0;

    // always leave at least bri_min characters on the left,
    // if text width is sufficient
    இல்லையெனில் எனில் (bri > eff_wwidth - wp->w_briopt_min)
	bri = (eff_wwidth - wp->w_briopt_min < 0)
					   ? 0 : eff_wwidth - wp->w_briopt_min;

    திரும்பு bri;
}
#endif

/*
 * When extra == 0: Return TRUE if the cursor is before or on the first
 *		    non-blank in the line.
 * When extra == 1: Return TRUE if the cursor is before the first non-blank in
 *		    the line.
 */
    முழு
inindent(முழு extra)
{
    char_u	*ptr;
    colnr_T	col;

    க்கு (col = 0, ptr = ml_get_curline(); VIM_ISWHITE(*ptr); ++col)
	++ptr;
    எனில் (col >= curwin->w_cursor.col + extra)
	திரும்பு TRUE;
    இல்லையெனில்
	திரும்பு FALSE;
}

/*
 * op_reindent - handle reindenting a block of lines.
 */
    வெறுமை
op_reindent(oparg_T *oap, முழு (*how)(வெறுமை))
{
    long	i = 0;
    char_u	*l;
    முழு		amount;
    linenr_T	first_changed = 0;
    linenr_T	last_changed = 0;
    linenr_T	start_lnum = curwin->w_cursor.lnum;

    // Don't even try when 'modifiable' is off.
    எனில் (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	திரும்பு;
    }

    // Save for undo.  Do this once for all lines, much faster than doing this
    // for each line separately, especially when undoing.
    எனில் (u_savecommon(start_lnum - 1, start_lnum + oap->வரி_மொத்தம்,
				     start_lnum + oap->வரி_மொத்தம், FALSE) == OK)
	க்கு (i = oap->வரி_மொத்தம்; --i >= 0 && !got_int; )
	{
	    // it's a slow thing to do, so give feedback so there's no worry
	    // that the computer's just hung.

	    எனில் (i > 1
		    && (i % 50 == 0 || i == oap->வரி_மொத்தம் - 1)
		    && oap->வரி_மொத்தம் > p_report)
		smsg(_("%ld lines to indent... "), i);

	    // Be vi-compatible: For lisp indenting the first line is not
	    // indented, unless there is only one line.
	    எனில் (i != oap->வரி_மொத்தம் - 1 || oap->வரி_மொத்தம் == 1
						     || how != get_lisp_indent)
	    {
		l = skipwhite(ml_get_curline());
		எனில் (*l == NUL)		    // empty or blank line
		    amount = 0;
		இல்லையெனில்
		    amount = how();	    // get the indent for this line

		எனில் (amount >= 0 && set_indent(amount, 0))
		{
		    // did change the indent, call changed_lines() later
		    எனில் (first_changed == 0)
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
    எனில் (last_changed != 0)
	changed_வரிகள்(first_changed, 0,
		oap->is_VIsual ? start_lnum + oap->வரி_மொத்தம் :
		last_changed + 1, 0L);
    இல்லையெனில் எனில் (oap->is_VIsual)
	redraw_curbuf_later(UPD_INVERTED);

    எனில் (oap->வரி_மொத்தம் > p_report)
    {
	i = oap->வரி_மொத்தம் - (i + 1);
	smsg(NGETTEXT("%ld line indented ",
						 "%ld lines indented ", i), i);
    }
    எனில் ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// set '[ and '] marks
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
    }
}

/*
 * Return TRUE if lines starting with '#' should be left aligned.
 */
    முழு
preprocs_இடது(வெறுமை)
{
    திரும்பு
	(curbuf->b_p_si && !curbuf->b_p_cin) ||
	(curbuf->b_p_cin && in_cinkeys('#', ' ', TRUE)
					   && curbuf->b_ind_hash_comment == 0)
	;
}

/*
 * Return TRUE if the conditions are OK for smart indenting.
 */
    முழு
may_do_si(வெறுமை)
{
    திரும்பு curbuf->b_p_si
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
    வெறுமை
ins_try_si(முழு c)
{
    pos_T	*pos, old_pos;
    char_u	*ptr;
    முழு		i;
    முழு		temp;

    // do some very smart indenting when entering '{' or '}'
    எனில் (((did_si || can_si_back) && c == '{')
	    || (can_si && c == '}' && inindent(0)))
    {
	// for '}' set indent equal to indent of line containing matching '{'
	எனில் (c == '}' && (pos = findmatch(வெற்று, '{')) != வெற்று)
	{
	    old_pos = curwin->w_cursor;
	    // If the matching '{' has a ')' immediately before it (ignoring
	    // white-space), then line up with the start of the line
	    // containing the matching '(' if there is one.  This handles the
	    // case where an "if (..\n..) {" statement continues over multiple
	    // lines -- webb
	    ptr = ml_get(pos->lnum);
	    i = pos->col;
	    எனில் (i > 0)		// skip blanks before '{'
		போது (--i > 0 && VIM_ISWHITE(ptr[i]))
		    ;
	    curwin->w_cursor.lnum = pos->lnum;
	    curwin->w_cursor.col = i;
	    எனில் (ptr[i] == ')' && (pos = findmatch(வெற்று, '(')) != வெற்று)
		curwin->w_cursor = *pos;
	    i = get_indent();
	    curwin->w_cursor = old_pos;
	    எனில் (State & VREPLACE_FLAG)
		change_indent(INDENT_SET, i, FALSE, NUL, TRUE);
	    இல்லையெனில்
		(வெறுமை)set_indent(i, SIN_CHANGED);
	}
	இல்லையெனில் எனில் (curwin->w_cursor.col > 0)
	{
	    // when inserting '{' after "O" reduce indent, but not
	    // more than indent of previous line
	    temp = TRUE;
	    எனில் (c == '{' && can_si_back && curwin->w_cursor.lnum > 1)
	    {
		old_pos = curwin->w_cursor;
		i = get_indent();
		போது (curwin->w_cursor.lnum > 1)
		{
		    ptr = skipwhite(ml_get(--(curwin->w_cursor.lnum)));

		    // ignore empty lines and lines starting with '#'.
		    எனில் (*ptr != '#' && *ptr != NUL)
			நிறுத்து;
		}
		எனில் (get_indent() >= i)
		    temp = FALSE;
		curwin->w_cursor = old_pos;
	    }
	    எனில் (temp)
		shift_வரி(TRUE, FALSE, 1, TRUE);
	}
    }

    // set indent of '#' always to 0
    எனில் (curwin->w_cursor.col > 0 && can_si && c == '#' && inindent(0))
    {
	// remember current indent for next line
	old_indent = get_indent();
	(வெறுமை)set_indent(0, SIN_CHANGED);
    }

    // Adjust ai_col, the char at this position can be deleted.
    எனில் (ai_col > curwin->w_cursor.col)
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
    வெறுமை
change_indent(
    முழு		type,
    முழு		amount,
    முழு		round,
    முழு		replaced,	// replaced character, put on replace stack
    முழு		call_changed_bytes)	// call changed_bytes()
{
    முழு		vcol;
    முழு		last_vcol;
    முழு		insstart_less;		// reduction for Insstart.col
    முழு		new_cursor_col;
    முழு		i;
    char_u	*ptr;
    முழு		save_p_list;
    முழு		start_col;
    colnr_T	vc;
    colnr_T	orig_col = 0;		// init for GCC
    char_u	*new_வரி, *orig_வரி = வெற்று;	// init for GCC

    // MODE_VREPLACE state needs to know what the line was like before changing
    எனில் (State & VREPLACE_FLAG)
    {
	orig_வரி = vim_strnsave(ml_get_curline(), ml_get_curline_len());  // Deal with NULL below
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
    எனில் (new_cursor_col < 0)
	vcol = get_indent() - vcol;

    எனில் (new_cursor_col > 0)	    // can't fix replace stack
	start_col = -1;

    // Set the new indent.  The cursor will be put on the first non-blank.
    எனில் (type == INDENT_SET)
	(வெறுமை)set_indent(amount, call_changed_bytes ? SIN_CHANGED : 0);
    இல்லையெனில்
    {
	முழு	save_State = State;

	// Avoid being called recursively.
	எனில் (State & VREPLACE_FLAG)
	    State = MODE_செருகு;
	shift_வரி(type == INDENT_DEC, round, 1, call_changed_bytes);
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
    எனில் (new_cursor_col >= 0)
    {
	// When changing the indent while the cursor is touching it, reset
	// Insstart_col to 0.
	எனில் (new_cursor_col == 0)
	    insstart_less = MAXCOL;
	new_cursor_col += curwin->w_cursor.col;
    }
    இல்லையெனில் எனில் (!(State & MODE_செருகு))
	new_cursor_col = curwin->w_cursor.col;
    இல்லையெனில்
    {
	chartabsize_T cts;

	// Compute the screen column where the cursor should be.
	vcol = get_indent() - vcol;
	curwin->w_virtcol = (colnr_T)((vcol < 0) ? 0 : vcol);

	// Advance the cursor until we reach the right screen column.
	last_vcol = 0;
	ptr = ml_get_curline();
	init_chartabsize_arg(&cts, curwin, 0, 0, ptr, ptr);
	போது (cts.cts_vcol <= (முழு)curwin->w_virtcol)
	{
	    last_vcol = cts.cts_vcol;
	    எனில் (cts.cts_vcol > 0)
		MB_PTR_ADV(cts.cts_ptr);
	    எனில் (*cts.cts_ptr == NUL)
		நிறுத்து;
	    cts.cts_vcol += lbr_chartabsize(&cts);
	}
	vcol = last_vcol;
	new_cursor_col = cts.cts_ptr - cts.cts_வரி;
	clear_chartabsize_arg(&cts);

	// May need to insert spaces to be able to position the cursor on
	// the right screen column.
	எனில் (vcol != (முழு)curwin->w_virtcol)
	{
	    curwin->w_cursor.col = (colnr_T)new_cursor_col;
	    i = (முழு)curwin->w_virtcol - vcol;
	    ptr = alloc(i + 1);
	    எனில் (ptr != வெற்று)
	    {
		size_t ptrlen;
		new_cursor_col += i;
		ptr[i] = NUL;
		ptrlen = i;
		போது (--i >= 0)
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

    எனில் (new_cursor_col <= 0)
	curwin->w_cursor.col = 0;
    இல்லையெனில்
	curwin->w_cursor.col = (colnr_T)new_cursor_col;
    curwin->w_set_curswant = true;
    changed_cline_bef_curs();

    // May have to adjust the start of the insert.
    எனில் (State & MODE_செருகு)
    {
	எனில் (curwin->w_cursor.lnum == Insstart.lnum && Insstart.col != 0)
	{
	    எனில் ((முழு)Insstart.col <= insstart_less)
		Insstart.col = 0;
	    இல்லையெனில்
		Insstart.col -= insstart_less;
	}
	எனில் ((முழு)ai_col <= insstart_less)
	    ai_col = 0;
	இல்லையெனில்
	    ai_col -= insstart_less;
    }

    // For MODE_REPLACE state, may have to fix the replace stack, if it's
    // possible.  If the number of characters before the cursor decreased, need
    // to pop a few characters from the replace stack.
    // If the number of characters before the cursor increased, need to push a
    // few NULs onto the replace stack.
    எனில் (REPLACE_NORMAL(State) && start_col >= 0)
    {
	போது (start_col > (முழு)curwin->w_cursor.col)
	{
	    replace_join(0);	    // remove a NUL from the replace stack
	    --start_col;
	}
	போது (start_col < (முழு)curwin->w_cursor.col || replaced)
	{
	    replace_தள்ளு(NUL);
	    எனில் (replaced)
	    {
		replace_தள்ளு(replaced);
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
    எனில் (State & VREPLACE_FLAG)
    {
	// If orig_line didn't allocate, just return.  At least we did the job,
	// even if you can't backspace.
	எனில் (orig_வரி == வெற்று)
	    திரும்பு;

	// Save new line
	new_வரி = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	எனில் (new_வரி == வெற்று)
	{
	    vim_free(orig_வரி);
	    திரும்பு;
	}

	// We only put back the new line up to the cursor
	new_வரி[curwin->w_cursor.col] = NUL;

	// Put back original line
	ml_replace(curwin->w_cursor.lnum, orig_வரி, FALSE);
	curwin->w_cursor.col = orig_col;

	// Backspace from cursor to start of line
	backspace_until_column(0);

	// Insert new stuff into line again
	ins_bytes(new_வரி);

	vim_free(new_வரி);
    }
}

/*
 * Copy the indent from ptr to the current line (and fill to size)
 * Leaves the cursor on the first non-blank in the line.
 * Returns TRUE if the line was changed.
 */
    முழு
copy_indent(முழு size, char_u *src)
{
    char_u	*p = வெற்று;
    char_u	*வரி = வெற்று;
    char_u	*s;
    முழு		todo;
    முழு		ind_len;
    முழு		வரி_len = 0;
    முழு		tab_pad;
    முழு		ind_done;
    முழு		round;
#ifdef FEAT_VARTABS
    முழு		ind_col;
#endif

    // Round 1: compute the number of characters needed for the indent
    // Round 2: copy the characters.
    க்கு (round = 1; round <= 2; ++round)
    {
	todo = size;
	ind_len = 0;
	ind_done = 0;
#ifdef FEAT_VARTABS
	ind_col = 0;
#endif
	s = src;

	// Count/copy the usable portion of the source line
	போது (todo > 0 && VIM_ISWHITE(*s))
	{
	    எனில் (*s == TAB)
	    {
#ifdef FEAT_VARTABS
		tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#இல்லையெனில்
		tab_pad = (முழு)curbuf->b_p_ts
					   - (ind_done % (முழு)curbuf->b_p_ts);
#endif
		// Stop if this tab will overshoot the target
		எனில் (todo < tab_pad)
		    நிறுத்து;
		todo -= tab_pad;
		ind_done += tab_pad;
#ifdef FEAT_VARTABS
		ind_col += tab_pad;
#endif
	    }
	    இல்லையெனில்
	    {
		--todo;
		++ind_done;
#ifdef FEAT_VARTABS
		++ind_col;
#endif
	    }
	    ++ind_len;
	    எனில் (p != வெற்று)
		*p++ = *s;
	    ++s;
	}

	// Fill to next tabstop with a tab, if possible
#ifdef FEAT_VARTABS
	tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#இல்லையெனில்
	tab_pad = (முழு)curbuf->b_p_ts - (ind_done % (முழு)curbuf->b_p_ts);
#endif
	எனில் (todo >= tab_pad && !curbuf->b_p_et)
	{
	    todo -= tab_pad;
	    ++ind_len;
#ifdef FEAT_VARTABS
	    ind_col += tab_pad;
#endif
	    எனில் (p != வெற்று)
		*p++ = TAB;
	}

	// Add tabs required for indent
	எனில் (!curbuf->b_p_et)
	{
#ifdef FEAT_VARTABS
	    க்கு (;;)
	    {
		tab_pad = tabstop_padding(ind_col, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
		எனில் (todo < tab_pad)
		    நிறுத்து;
		todo -= tab_pad;
		++ind_len;
		ind_col += tab_pad;
		எனில் (p != வெற்று)
		    *p++ = TAB;
	    }
#இல்லையெனில்
	    போது (todo >= (முழு)curbuf->b_p_ts)
	    {
		todo -= (முழு)curbuf->b_p_ts;
		++ind_len;
		எனில் (p != வெற்று)
		    *p++ = TAB;
	    }
#endif
	}

	// Count/add spaces required for indent
	போது (todo > 0)
	{
	    --todo;
	    ++ind_len;
	    எனில் (p != வெற்று)
		*p++ = ' ';
	}

	எனில் (p == வெற்று)
	{
	    // Allocate memory for the result: the copied indent, new indent
	    // and the rest of the line.
	    வரி_len = ml_get_curline_len() + 1;
	    வரி = alloc(ind_len + வரி_len);
	    எனில் (வரி == வெற்று)
		திரும்பு FALSE;
	    p = வரி;
	}
    }

    // Append the original line
    mch_memmove(p, ml_get_curline(), (size_t)வரி_len);

    // Replace the line
    ml_replace(curwin->w_cursor.lnum, வரி, FALSE);

    // Put the cursor after the indent.
    curwin->w_cursor.col = ind_len;
    திரும்பு TRUE;
}

/*
 * Give a "resulting text too long" error and maybe set got_int.
 */
    நிலையான வெறுமை
emsg_text_too_long(வெறுமை)
{
    emsg(_(e_resulting_text_too_long));
#ifdef FEAT_EVAL
    // when not inside a try/catch set got_int to break out of any loop
    எனில் (trylevel == 0)
#endif
	got_int = TRUE;
}

/*
 * ":retab".
 */
    வெறுமை
ex_retab(exarg_T *eap)
{
    linenr_T	lnum;
    முழு		got_tab = FALSE;
    long	இலக்கம்_spaces = 0;
    long	இலக்கம்_tabs;
    long	len;
    long	col;
    long	vcol;
    long	start_col = 0;		// For start of white-space string
    long	start_vcol = 0;		// For start of white-space string
    long	old_len;
    long	new_len;
    char_u	*ptr;
    char_u	*new_வரி = (char_u *)1; // init to non-NULL
    முழு		did_undo;		// called u_save for current line
#ifdef FEAT_VARTABS
    முழு		*new_vts_array = வெற்று;
    char_u	*new_ts_str;		// string value of tab argument
#இல்லையெனில்
    முழு		temp;
    முழு		new_ts = 0;
#endif
    முழு		save_list;
    linenr_T	first_வரி = 0;		// first changed line
    linenr_T	last_வரி = 0;		// last changed line
    முழு		is_indent_only = 0;	// Only process leading whitespace

    save_list = curwin->w_p_list;
    curwin->w_p_list = 0;	    // don't want list mode here

    ptr = eap->arg;
    எனில் (STRNCMP(ptr, "-indentonly", 11) == 0 && IS_WHITE_OR_NUL(ptr[11]))
    {
	is_indent_only = 1;
	ptr = skipwhite(ptr + 11);
    }

#ifdef FEAT_VARTABS
    new_ts_str = ptr;
    எனில் (tabstop_set(ptr, &new_vts_array) == FAIL)
	திரும்பு;
    போது (vim_isdigit(*ptr) || *ptr == ',')
	++ptr;

    // This ensures that either new_vts_array and new_ts_str are freshly
    // allocated, or new_vts_array points to an existing array and new_ts_str
    // is null.
    எனில் (new_vts_array == வெற்று)
    {
	new_vts_array = curbuf->b_p_vts_array;
	new_ts_str = வெற்று;
    }
    இல்லையெனில்
	new_ts_str = vim_strnsave(new_ts_str, ptr - new_ts_str);
#இல்லையெனில்
    எனில் (ptr[0] != NUL && (ptr[0] != '0' || ptr[1] != NUL))
    {
	char_u	*end;

	எனில் (strtol((எழுத்து *)ptr, (எழுத்து **)&end, 10) <= 0)
	{
	    எனில் (ptr != end)
		emsg(_(e_argument_must_be_positive));
	    இல்லையெனில்
		semsg(_(e_invalid_argument_str), ptr);
	    திரும்பு;
	}
	new_ts = getdigits(&ptr);
	எனில் (new_ts < 0 || new_ts > TABSTOP_MAX)
	{
	    semsg(_(e_invalid_argument_str), eap->arg);
	    திரும்பு;
	}
    }
    எனில் (new_ts == 0)
	new_ts = curbuf->b_p_ts;
#endif
    க்கு (lnum = eap->line1; !got_int && lnum <= eap->line2; ++lnum)
    {
	ptr = ml_get(lnum);
	old_len = ml_get_len(lnum);
	col = 0;
	vcol = 0;
	did_undo = FALSE;
	க்கு (;;)
	{
	    எனில் (VIM_ISWHITE(ptr[col]))
	    {
		எனில் (!got_tab && இலக்கம்_spaces == 0)
		{
		    // First consecutive white-space
		    start_vcol = vcol;
		    start_col = col;
		}
		எனில் (ptr[col] == ' ')
		    இலக்கம்_spaces++;
		இல்லையெனில்
		    got_tab = TRUE;
	    }
	    இல்லையெனில்
	    {
		எனில் (got_tab || (eap->forceit && இலக்கம்_spaces > 1))
		{
		    // Retabulate this string of white-space

		    // len is virtual length of white string
		    len = இலக்கம்_spaces = vcol - start_vcol;
		    இலக்கம்_tabs = 0;
		    எனில் (!curbuf->b_p_et)
		    {
#ifdef FEAT_VARTABS
			முழு t, s;

			tabstop_fromto(start_vcol, vcol,
					curbuf->b_p_ts, new_vts_array, &t, &s);
			இலக்கம்_tabs = t;
			இலக்கம்_spaces = s;
#இல்லையெனில்
			temp = new_ts - (start_vcol % new_ts);
			எனில் (இலக்கம்_spaces >= temp)
			{
			    இலக்கம்_spaces -= temp;
			    இலக்கம்_tabs++;
			}
			இலக்கம்_tabs += இலக்கம்_spaces / new_ts;
			இலக்கம்_spaces -= (இலக்கம்_spaces / new_ts) * new_ts;
#endif
		    }
		    எனில் (curbuf->b_p_et || got_tab ||
					(இலக்கம்_spaces + இலக்கம்_tabs < len))
		    {
			எனில் (did_undo == FALSE)
			{
			    did_undo = TRUE;
			    எனில் (u_save((linenr_T)(lnum - 1),
						(linenr_T)(lnum + 1)) == FAIL)
			    {
				new_வரி = வெற்று;	// flag out-of-memory
				நிறுத்து;
			    }
			}

			// len is actual number of white characters used
			len = இலக்கம்_spaces + இலக்கம்_tabs;
			new_len = old_len - col + start_col + len + 1;
			எனில் (new_len <= 0 || new_len >= MAXCOL)
			{
			    emsg_text_too_long();
			    நிறுத்து;
			}
			new_வரி = alloc(new_len);
			எனில் (new_வரி == வெற்று)
			    நிறுத்து;
			எனில் (start_col > 0)
			    mch_memmove(new_வரி, ptr, (size_t)start_col);
			mch_memmove(new_வரி + start_col + len,
				      ptr + col, (size_t)(old_len - col + 1));
			ptr = new_வரி + start_col;
			க்கு (col = 0; col < len; col++)
			    ptr[col] = (col < இலக்கம்_tabs) ? '\t' : ' ';
			எனில் (ml_replace(lnum, new_வரி, FALSE) == OK)
			    // "new_line" may have been copied
			    new_வரி = curbuf->b_ml.ml_வரி_ptr;
			எனில் (first_வரி == 0)
			    first_வரி = lnum;
			last_வரி = lnum;
			ptr = new_வரி;
			old_len = new_len - 1;
			col = start_col + len;
		    }
		}
		got_tab = FALSE;
		இலக்கம்_spaces = 0;

		எனில் (is_indent_only)
		    நிறுத்து;
	    }
	    எனில் (ptr[col] == NUL)
		நிறுத்து;
	    vcol += chartabsize(ptr + col, (colnr_T)vcol);
	    எனில் (vcol >= MAXCOL)
	    {
		emsg_text_too_long();
		நிறுத்து;
	    }
	    எனில் (has_mbyte)
		col += (*mb_ptr2len)(ptr + col);
	    இல்லையெனில்
		++col;
	}
	எனில் (new_வரி == வெற்று)		    // out of memory
	    நிறுத்து;
	வரி_breakcheck();
    }
    எனில் (got_int)
	emsg(_(e_interrupted));

#ifdef FEAT_VARTABS
    // If a single value was given then it can be considered equal to
    // either the value of 'tabstop' or the value of 'vartabstop'.
    எனில் (tabstop_மொத்தம்(curbuf->b_p_vts_array) == 0
	&& tabstop_மொத்தம்(new_vts_array) == 1
	&& curbuf->b_p_ts == tabstop_first(new_vts_array))
	; // not changed
    இல்லையெனில் எனில் (tabstop_மொத்தம்(curbuf->b_p_vts_array) > 0
	&& tabstop_eq(curbuf->b_p_vts_array, new_vts_array))
	; // not changed
    இல்லையெனில்
	redraw_curbuf_later(UPD_NOT_VALID);
#இல்லையெனில்
    எனில் (curbuf->b_p_ts != new_ts)
	redraw_curbuf_later(UPD_NOT_VALID);
#endif
    எனில் (first_வரி != 0)
	changed_வரிகள்(first_வரி, 0, last_வரி + 1, 0L);

    curwin->w_p_list = save_list;	// restore 'list'

#ifdef FEAT_VARTABS
    எனில் (new_ts_str != வெற்று)		// set the new tabstop
    {
	// If 'vartabstop' is in use or if the value given to retab has more
	// than one tabstop then update 'vartabstop'.
	முழு *old_vts_ary = curbuf->b_p_vts_array;

	எனில் (tabstop_மொத்தம்(old_vts_ary) > 0 || tabstop_மொத்தம்(new_vts_array) > 1)
	{
	    set_string_option_direct((char_u *)"vts", -1, new_ts_str,
							OPT_FREE|OPT_LOCAL, 0);
	    curbuf->b_p_vts_array = new_vts_array;
	    vim_free(old_vts_ary);
	}
	இல்லையெனில்
	{
	    // 'vartabstop' wasn't in use and a single value was given to
	    // retab then update 'tabstop'.
	    curbuf->b_p_ts = tabstop_first(new_vts_array);
	    vim_free(new_vts_array);
	}
	vim_free(new_ts_str);
    }
#இல்லையெனில்
    curbuf->b_p_ts = new_ts;
#endif
    coladvance(curwin->w_curswant);

    u_clearline();
}

#எனில் defined(FEAT_EVAL)
/*
 * Get indent level from 'indentexpr'.
 */
    முழு
get_கோவை_indent(வெறுமை)
{
    முழு		indent = -1;
    char_u	*inde_copy;
    pos_T	save_pos;
    colnr_T	save_curswant;
    முழு		save_set_curswant;
    முழு		save_State;
    முழு		use_sandbox = was_set_insecurely(curwin,
					    (char_u *)"indentexpr", OPT_LOCAL);
    sctx_T	save_sctx = current_sctx;

    // Save and restore cursor position and curswant, in case it was changed
    // via :normal commands
    save_pos = curwin->w_cursor;
    save_curswant = curwin->w_curswant;
    save_set_curswant = curwin->w_set_curswant;
    set_vim_var_nr(VV_LNUM, curwin->w_cursor.lnum);
    எனில் (use_sandbox)
	++sandbox;
    ++textlock;
    current_sctx = curbuf->b_p_script_ctx[BV_INDE];

    // Need to make a copy, the 'indentexpr' option could be changed while
    // evaluating it.
    inde_copy = vim_strsave(curbuf->b_p_inde);
    எனில் (inde_copy != வெற்று)
    {
	indent = (முழு)eval_to_எண்(inde_copy, TRUE);
	vim_free(inde_copy);
    }

    எனில் (use_sandbox)
	--sandbox;
    --textlock;
    current_sctx = save_sctx;

    // Restore the cursor position so that 'indentexpr' doesn't need to.
    // Pretend to be in Insert mode, allow cursor past end of line for "o"
    // command.
    save_State = State;
    State = MODE_செருகு;
    curwin->w_cursor = save_pos;
    curwin->w_curswant = save_curswant;
    curwin->w_set_curswant = save_set_curswant;
    check_cursor();
    State = save_State;

    // Reset did_throw, unless 'debug' has "throw" and inside a try/catch.
    எனில் (did_throw && (vim_strchr(p_debug, 't') == வெற்று || trylevel == 0))
    {
	handle_did_throw();
	did_throw = FALSE;
    }

    // If there is an error, just keep the current indent.
    எனில் (indent < 0)
	indent = get_indent();

    திரும்பு indent;
}
#endif

    நிலையான முழு
lisp_match(char_u *p)
{
    char_u	buf[LSIZE];
    முழு		len;
    char_u	*word = *curbuf->b_p_lw != NUL ? curbuf->b_p_lw : p_lispwords;

    போது (*word != NUL)
    {
	len = copy_option_part(&word, buf, LSIZE, ",");
	எனில் (STRNCMP(buf, p, len) == 0 && IS_WHITE_OR_NUL(p[len]))
	    திரும்பு TRUE;
    }
    திரும்பு FALSE;
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
    முழு
get_lisp_indent(வெறுமை)
{
    pos_T	*pos, realpos, paren;
    முழு		amount;
    char_u	*that;
    colnr_T	col;
    colnr_T	firsttry;
    முழு		parencount, quotecount;
    முழு		vi_lisp;

    // Set vi_lisp to use the vi-compatible method
    vi_lisp = (vim_strchr(p_cpo, CPO_LISP) != வெற்று);

    realpos = curwin->w_cursor;
    curwin->w_cursor.col = 0;

    எனில் ((pos = findmatch(வெற்று, '(')) == வெற்று)
	pos = findmatch(வெற்று, '[');
    இல்லையெனில்
    {
	paren = *pos;
	pos = findmatch(வெற்று, '[');
	எனில் (pos == வெற்று || LT_POSP(pos, &paren))
	    pos = &paren;
    }
    எனில் (pos != வெற்று)
    {
	// Extra trick: Take the indent of the first previous non-white
	// line that is at the same () level.
	amount = -1;
	parencount = 0;

	போது (--curwin->w_cursor.lnum >= pos->lnum)
	{
	    எனில் (linewhite(curwin->w_cursor.lnum))
		தொடர்;
	    க்கு (that = ml_get_curline(); *that != NUL; ++that)
	    {
		எனில் (*that == ';')
		{
		    போது (*(that + 1) != NUL)
			++that;
		    தொடர்;
		}
		எனில் (*that == '\\')
		{
		    எனில் (*(that + 1) != NUL)
			++that;
		    தொடர்;
		}
		எனில் (*that == '"' && *(that + 1) != NUL)
		{
		    போது (*++that && *that != '"')
		    {
			// skipping escaped characters in the string
			எனில் (*that == '\\')
			{
			    எனில் (*++that == NUL)
				நிறுத்து;
			    எனில் (that[1] == NUL)
			    {
				++that;
				நிறுத்து;
			    }
			}
		    }
		    எனில் (*that == NUL)
			நிறுத்து;
		}
		எனில் (*that == '(' || *that == '[')
		    ++parencount;
		இல்லையெனில் எனில் (*that == ')' || *that == ']')
		    --parencount;
	    }
	    எனில் (parencount == 0)
	    {
		amount = get_indent();
		நிறுத்து;
	    }
	}

	எனில் (amount == -1)
	{
	    curwin->w_cursor.lnum = pos->lnum;
	    curwin->w_cursor.col = pos->col;
	    col = pos->col;

	    that = ml_get_curline();

	    எனில் (vi_lisp && get_indent() == 0)
		amount = 2;
	    இல்லையெனில்
	    {
		char_u		*வரி = that;
		chartabsize_T	cts;

		init_chartabsize_arg(&cts, curwin, pos->lnum, 0, வரி, வரி);
		போது (*cts.cts_ptr != NUL && col > 0)
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

		எனில் (!vi_lisp && (*that == '(' || *that == '[')
						      && lisp_match(that + 1))
		    amount += 2;
		இல்லையெனில்
		{
		    எனில் (*that != NUL)
		    {
			that++;
			amount++;
		    }
		    firsttry = amount;

		    init_chartabsize_arg(&cts, curwin, (colnr_T)(that - வரி),
							   amount, வரி, that);
		    போது (VIM_ISWHITE(*cts.cts_ptr))
		    {
			cts.cts_vcol += lbr_chartabsize(&cts);
			++cts.cts_ptr;
		    }
		    that = cts.cts_ptr;
		    amount = cts.cts_vcol;
		    clear_chartabsize_arg(&cts);

		    எனில் (*that && *that != ';') // not a comment line
		    {
			// test *that != '(' to accommodate first let/do
			// argument if it is more than one line
			எனில் (!vi_lisp && *that != '(' && *that != '[')
			    firsttry++;

			parencount = 0;
			quotecount = 0;

			init_chartabsize_arg(&cts, curwin,
				   (colnr_T)(that - வரி), amount, வரி, that);
			எனில் (vi_lisp
				|| (*that != '"'
				    && *that != '\''
				    && *that != '#'
				    && (*that < '0' || *that > '9')))
			{
			    போது (*cts.cts_ptr
				    && (!VIM_ISWHITE(*cts.cts_ptr)
					|| quotecount
					|| parencount)
				    && (!((*cts.cts_ptr == '('
							|| *cts.cts_ptr == '[')
					    && !quotecount
					    && !parencount
					    && vi_lisp)))
			    {
				எனில் (*cts.cts_ptr == '"')
				    quotecount = !quotecount;
				எனில் ((*cts.cts_ptr == '(' || *cts.cts_ptr == '[')
							       && !quotecount)
				    ++parencount;
				எனில் ((*cts.cts_ptr == ')' || *cts.cts_ptr == ']')
							       && !quotecount)
				    --parencount;
				எனில் (*cts.cts_ptr == '\\'
						    && *(cts.cts_ptr+1) != NUL)
				    cts.cts_vcol += lbr_chartabsize_adv(&cts);
				cts.cts_vcol += lbr_chartabsize_adv(&cts);
			    }
			}
			போது (VIM_ISWHITE(*cts.cts_ptr))
			{
			    cts.cts_vcol += lbr_chartabsize(&cts);
			    ++cts.cts_ptr;
			}
			that = cts.cts_ptr;
			amount = cts.cts_vcol;
			clear_chartabsize_arg(&cts);

			எனில் (!*that || *that == ';')
			    amount = firsttry;
		    }
		}
	    }
	}
    }
    இல்லையெனில்
	amount = 0;	// no matching '(' or '[' found, use zero indent

    curwin->w_cursor = realpos;

    திரும்பு amount;
}

/*
 * Re-indent the current line, based on the current contents of it and the
 * surrounding lines. Fixing the cursor position seems really easy -- I'm very
 * confused what all the part that handles Control-T is doing that I'm not.
 * "get_the_indent" should be get_c_indent, get_expr_indent or get_lisp_indent.
 */

    வெறுமை
fixthisline(முழு (*get_the_indent)(வெறுமை))
{
    முழு amount = get_the_indent();

    எனில் (amount < 0)
	திரும்பு;

    change_indent(INDENT_SET, amount, FALSE, 0, TRUE);
    எனில் (linewhite(curwin->w_cursor.lnum))
	did_ai = TRUE;	// delete the indent if the line stays empty
}

/*
 * Return TRUE if 'indentexpr' should be used for Lisp indenting.
 * Caller may want to check 'autoindent'.
 */
    முழு
use_indentexpr_for_lisp(வெறுமை)
{
#ifdef FEAT_EVAL
    திரும்பு curbuf->b_p_lisp
		&& *curbuf->b_p_inde != NUL
		&& STRCMP(curbuf->b_p_lop, "expr:1") == 0;
#இல்லையெனில்
    திரும்பு FALSE;
#endif
}

/*
 * Fix indent for 'lisp' and 'cindent'.
 */
    வெறுமை
fix_indent(வெறுமை)
{
    எனில் (p_paste)
	திரும்பு;  // no auto-indenting when 'paste' is set
    எனில் (curbuf->b_p_lisp && curbuf->b_p_ai)
    {
	எனில் (use_indentexpr_for_lisp())
	    do_c_கோவை_indent();
	இல்லையெனில்
	    fixthisline(get_lisp_indent);
    }
    இல்லையெனில் எனில் (cindent_on())
	do_c_கோவை_indent();
}

#எனில் defined(FEAT_EVAL)
/*
 * "indent()" function
 */
    வெறுமை
f_indent(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    எனில் (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	திரும்பு;

    lnum = tv_get_lnum(argvars);
    எனில் (lnum >= 1 && lnum <= curbuf->b_ml.ml_வரி_மொத்தம்)
	rettv->vval.v_எண் = get_indent_lnum(lnum);
    இல்லையெனில்
    {
	எனில் (in_vim9script())
	    semsg(_(e_invalid_வரி_எண்_nr), lnum);
	rettv->vval.v_எண் = -1;
    }
}

/*
 * "lispindent(lnum)" function
 */
    வெறுமை
f_lispindent(typval_T *argvars UNUSED, typval_T *rettv)
{
    pos_T	pos;
    linenr_T	lnum;

    எனில் (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	திரும்பு;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    எனில் (lnum >= 1 && lnum <= curbuf->b_ml.ml_வரி_மொத்தம்)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_எண் = get_lisp_indent();
	curwin->w_cursor = pos;
    }
    இல்லையெனில் எனில் (in_vim9script())
	semsg(_(e_invalid_வரி_எண்_nr), lnum);
    இல்லையெனில்
	rettv->vval.v_எண் = -1;
}
#endif
