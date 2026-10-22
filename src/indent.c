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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_VARTABS)

/*
 * Set the integer values corresponding to the string setting of 'vartabstop'.
 * "array" will be set, caller must free it if needed.
 * Return FAIL for an error.
 */
    પૂર્ણાંક
tabstop_set(char_u *var, પૂર્ણાંક **array)
{
    પૂર્ણાંક	    valcount = 1;
    પૂર્ણાંક	    t;
    char_u  *cp;

    જો (var[0] == NUL || (var[0] == '0' && var[1] == NUL))
    {
	*array = ખાલી;
	પાછા OK;
    }

    માટે (cp = var; *cp != NUL; ++cp)
    {
	જો (cp == var || cp[-1] == ',')
	{
	    char_u *end;

	    જો (strtol((અક્ષર *)cp, (અક્ષર **)&end, 10) <= 0)
	    {
		જો (cp != end)
		    emsg(_(e_argument_must_be_positive));
		નહીંતર
		    semsg(_(e_invalid_argument_str), cp);
		પાછા FAIL;
	    }
	}

	જો (VIM_ISDIGIT(*cp))
	    ચાલુ;
	જો (cp[0] == ',' && cp > var && cp[-1] != ',' && cp[1] != NUL)
	{
	    ++valcount;
	    ચાલુ;
	}
	semsg(_(e_invalid_argument_str), var);
	પાછા FAIL;
    }

    *array = ALLOC_MULT(પૂર્ણાંક, valcount + 1);
    જો (*array == ખાલી)
	પાછા FAIL;
    (*array)[0] = valcount;

    t = 1;
    માટે (cp = var; *cp != NUL;)
    {
	પૂર્ણાંક n = atoi((અક્ષર *)cp);

	// Catch negative values, overflow and ridiculous big values.
	જો (n <= 0 || n > TABSTOP_MAX)
	{
	    semsg(_(e_invalid_argument_str), cp);
	    VIM_CLEAR(*array);
	    પાછા FAIL;
	}
	(*array)[t++] = n;
	જ્યારે (*cp != NUL && *cp != ',')
	    ++cp;
	જો (*cp != NUL)
	    ++cp;
    }

    પાછા OK;
}

/*
 * Calculate the number of screen spaces a tab will occupy.
 * If "vts" is set then the tab widths are taken from that array,
 * otherwise the value of ts is used.
 */
    પૂર્ણાંક
tabstop_padding(colnr_T col, પૂર્ણાંક ts_arg, પૂર્ણાંક *vts)
{
    પૂર્ણાંક		ts = ts_arg == 0 ? 8 : ts_arg;
    પૂર્ણાંક		tabcount;
    colnr_T	tabcol = 0;
    પૂર્ણાંક		t;
    પૂર્ણાંક		padding = 0;

    જો (vts == ખાલી || vts[0] == 0)
	પાછા ts - (col % ts);

    tabcount = vts[0];

    માટે (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	જો (tabcol > col)
	{
	    padding = (પૂર્ણાંક)(tabcol - col);
	    તોડો;
	}
    }
    જો (t > tabcount)
	padding = vts[tabcount] - (પૂર્ણાંક)((col - tabcol) % vts[tabcount]);

    પાછા padding;
}

/*
 * Find the size of the tab that covers a particular column.
 *
 * If this is being called as part of a shift operation, col is not the cursor
 * column but is the column number to the left of the first non-whitespace
 * character in the line.  If the shift is to the left (left = TRUE), then
 * return the size of the tab interval to the left of the column.
 */
    પૂર્ણાંક
tabstop_at(colnr_T col, પૂર્ણાંક ts, પૂર્ણાંક *vts, પૂર્ણાંક ડાબું)
{
    પૂર્ણાંક		tabcount;	// Number of tab stops in the list of variable
				// tab stops.
    colnr_T	tabcol = 0;	// Column of the tab stop under consideration.
    પૂર્ણાંક		t;		// Tabstop index in the list of variable tab
				// stops.
    પૂર્ણાંક		tab_size = 0;	// Size of the tab stop interval to the right
				// or left of the col.

    જો (vts == 0 || vts[0] == 0)
	પાછા ts;

    tabcount = vts[0];
    માટે (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	જો (tabcol > col)
	{
	    // If shifting left (left != 0), and if the column to the left of
	    // the first first non-blank character (col) in the line is
	    // already to the left of the first tabstop, set the shift amount
	    // (tab_size) to just enough to shift the line to the left margin.
	    // The value doesn't seem to matter as long as it is at least that
	    // distance.
	    જો (ડાબું && (t == 1))
		tab_size = col;
	    નહીંતર
		tab_size = vts[t - (ડાબું ? 1 : 0)];
	    તોડો;
	}
    }
    જો (t > tabcount)		// If the value of the index t is beyond the
				// end of the list, use the tab stop value at
				// the end of the list.
	tab_size = vts[tabcount];

    પાછા tab_size;
}

/*
 * Find the column on which a tab starts.
 */
    colnr_T
tabstop_start(colnr_T col, પૂર્ણાંક ts, પૂર્ણાંક *vts)
{
    પૂર્ણાંક		tabcount;
    colnr_T	tabcol = 0;
    પૂર્ણાંક		t;
    પૂર્ણાંક		excess;

    જો (vts == ખાલી || vts[0] == 0)
	પાછા col - col % ts;

    tabcount = vts[0];
    માટે (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	જો (tabcol > col)
	    પાછા tabcol - vts[t];
    }

    excess = tabcol % vts[tabcount];
    પાછા col - (col - excess) % vts[tabcount];
}

/*
 * Find the number of tabs and spaces necessary to get from one column
 * to another.
 */
    શૂન્ય
tabstop_fromto(
	colnr_T start_col,
	colnr_T end_col,
	પૂર્ણાંક	ts_arg,
	પૂર્ણાંક	*vts,
	પૂર્ણાંક	*ntabs,
	પૂર્ણાંક	*nspcs)
{
    પૂર્ણાંક		spaces = end_col - start_col;
    colnr_T	tabcol = 0;
    પૂર્ણાંક		padding = 0;
    પૂર્ણાંક		tabcount;
    પૂર્ણાંક		t;
    પૂર્ણાંક		ts = ts_arg == 0 ? curbuf->b_p_ts : ts_arg;

    જો (vts == ખાલી || vts[0] == 0)
    {
	પૂર્ણાંક tabs = 0;
	પૂર્ણાંક initspc = 0;

	initspc = ts - (start_col % ts);
	જો (spaces >= initspc)
	{
	    spaces -= initspc;
	    tabs++;
	}
	tabs += spaces / ts;
	spaces -= (spaces / ts) * ts;

	*ntabs = tabs;
	*nspcs = spaces;
	પાછા;
    }

    // Find the padding needed to reach the next tabstop.
    tabcount = vts[0];
    માટે (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	જો (tabcol > start_col)
	{
	    padding = (પૂર્ણાંક)(tabcol - start_col);
	    તોડો;
	}
    }
    જો (t > tabcount)
	padding = vts[tabcount] - (પૂર્ણાંક)((start_col - tabcol) % vts[tabcount]);

    // If the space needed is less than the padding no tabs can be used.
    જો (spaces < padding)
    {
	*ntabs = 0;
	*nspcs = spaces;
	પાછા;
    }

    *ntabs = 1;
    spaces -= padding;

    // At least one tab has been used. See if any more will fit.
    જ્યારે (spaces != 0 && ++t <= tabcount)
    {
	padding = vts[t];
	જો (spaces < padding)
	{
	    *nspcs = spaces;
	    પાછા;
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
    સ્થિર પૂર્ણાંક
tabstop_eq(પૂર્ણાંક *ts1, પૂર્ણાંક *ts2)
{
    પૂર્ણાંક		t;

    જો ((ts1 == 0 && ts2) || (ts1 && ts2 == 0))
	પાછા FALSE;
    જો (ts1 == ts2)
	પાછા TRUE;
    જો (ts1[0] != ts2[0])
	પાછા FALSE;

    માટે (t = 1; t <= ts1[0]; ++t)
	જો (ts1[t] != ts2[t])
	    પાછા FALSE;

    પાછા TRUE;
}

# જો defined(FEAT_BEVAL)
/*
 * Copy a tabstop array, allocating space for the new array.
 */
    પૂર્ણાંક *
tabstop_copy(પૂર્ણાંક *oldts)
{
    પૂર્ણાંક		*newts;
    પૂર્ણાંક		t;

    જો (oldts == ખાલી)
	પાછા ખાલી;
    newts = ALLOC_MULT(પૂર્ણાંક, oldts[0] + 1);
    જો (newts != ખાલી)
	માટે (t = 0; t <= oldts[0]; ++t)
	    newts[t] = oldts[t];
    પાછા newts;
}
# endif

/*
 * Return a count of the number of tabstops.
 */
    પૂર્ણાંક
tabstop_ગણતરી(પૂર્ણાંક *ts)
{
    પાછા ts != ખાલી ? ts[0] : 0;
}

/*
 * Return the first tabstop, or 8 if there are no tabstops defined.
 */
    પૂર્ણાંક
tabstop_first(પૂર્ણાંક *ts)
{
    પાછા ts != ખાલી ? ts[1] : 8;
}

#endif

/*
 * Return the effective shiftwidth value for current buffer, using the
 * 'tabstop' value when 'shiftwidth' is zero.
 */
    long
get_sw_કિંમત(buf_T *buf)
{
    પાછા get_sw_કિંમત_col(buf, 0, FALSE);
}

/*
 * Idem, using "pos".
 */
    સ્થિર long
get_sw_કિંમત_pos(buf_T *buf, pos_T *pos, પૂર્ણાંક ડાબું)
{
    pos_T save_cursor = curwin->w_cursor;
    long sw_કિંમત;

    curwin->w_cursor = *pos;
    sw_કિંમત = get_sw_કિંમત_col(buf, get_nolist_virtcol(), ડાબું);
    curwin->w_cursor = save_cursor;
    પાછા sw_કિંમત;
}

/*
 * Idem, using the first non-black in the current line.
 */
    long
get_sw_કિંમત_indent(buf_T *buf, પૂર્ણાંક ડાબું)
{
    pos_T pos = curwin->w_cursor;

    pos.col = getwhitecols_curline();
    પાછા get_sw_કિંમત_pos(buf, &pos, ડાબું);
}

/*
 * Idem, using virtual column "col".
 */
    long
get_sw_કિંમત_col(buf_T *buf, colnr_T col UNUSED, પૂર્ણાંક ડાબું UNUSED)
{
    પાછા buf->b_p_sw ? buf->b_p_sw :
#ifdef FEAT_VARTABS
	tabstop_at(col, buf->b_p_ts, buf->b_p_vts_array, ડાબું);
#નહીંતર
	buf->b_p_ts;
#endif
}

/*
 * Return the effective softtabstop value for the current buffer, using the
 * 'shiftwidth' value when 'softtabstop' is negative.
 */
    long
get_sts_કિંમત(શૂન્ય)
{
    પાછા curbuf->b_p_sts < 0 ? get_sw_કિંમત(curbuf) : curbuf->b_p_sts;
}

/*
 * Count the size (in window cells) of the indent in the current line.
 */
    પૂર્ણાંક
get_indent(શૂન્ય)
{
#ifdef FEAT_VARTABS
    પાછા get_indent_str_vtab(ml_get_curline(), (પૂર્ણાંક)curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#નહીંતર
    પાછા get_indent_str(ml_get_curline(), (પૂર્ણાંક)curbuf->b_p_ts, FALSE);
#endif
}

/*
 * Count the size (in window cells) of the indent in line "lnum".
 */
    પૂર્ણાંક
get_indent_lnum(linenr_T lnum)
{
#ifdef FEAT_VARTABS
    પાછા get_indent_str_vtab(ml_get(lnum), (પૂર્ણાંક)curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#નહીંતર
    પાછા get_indent_str(ml_get(lnum), (પૂર્ણાંક)curbuf->b_p_ts, FALSE);
#endif
}

#જો defined(FEAT_FOLDING)
/*
 * Count the size (in window cells) of the indent in line "lnum" of buffer
 * "buf".
 */
    પૂર્ણાંક
get_indent_buf(buf_T *buf, linenr_T lnum)
{
# ifdef FEAT_VARTABS
    પાછા get_indent_str_vtab(ml_get_buf(buf, lnum, FALSE),
			       (પૂર્ણાંક)buf->b_p_ts, buf->b_p_vts_array, FALSE);
# નહીંતર
    પાછા get_indent_str(ml_get_buf(buf, lnum, FALSE), (પૂર્ણાંક)buf->b_p_ts, FALSE);
# endif
}
#endif

/*
 * count the size (in window cells) of the indent in line "ptr", with
 * 'tabstop' at "ts"
 */
    પૂર્ણાંક
get_indent_str(
    char_u	*ptr,
    પૂર્ણાંક		ts,
    પૂર્ણાંક		no_ts) // if TRUE, count a tab as ^I
{
    પૂર્ણાંક		ગણતરી = 0;

    માટે ( ; *ptr; ++ptr)
    {
	જો (*ptr == TAB)    // count a tab for what it is worth
	{
	    જો (!no_ts)
		ગણતરી += ts - (ગણતરી % ts);
	    નહીંતર
		ગણતરી += ptr2cells(ptr);
	}
	નહીંતર જો (*ptr == ' ')
	    ++ગણતરી;		// count a space for one
	નહીંતર
	    તોડો;
    }
    પાછા ગણતરી;
}

#જો defined(FEAT_VARTABS)
/*
 * Count the size (in window cells) of the indent in line "ptr", using
 * variable tabstops.
 * If "no_ts" is TRUE, count a tab as ^I.
 */
    પૂર્ણાંક
get_indent_str_vtab(char_u *ptr, પૂર્ણાંક ts, પૂર્ણાંક *vts, પૂર્ણાંક no_ts)
{
    પૂર્ણાંક		ગણતરી = 0;

    માટે ( ; *ptr; ++ptr)
    {
	જો (*ptr == TAB)    // count a tab for what it is worth
	{
	    જો (!no_ts)
		ગણતરી += tabstop_padding(ગણતરી, ts, vts);
	    નહીંતર
		ગણતરી += ptr2cells(ptr);
	}
	નહીંતર જો (*ptr == ' ')
	    ++ગણતરી;		// count a space for one
	નહીંતર
	    તોડો;
    }
    પાછા ગણતરી;
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
    પૂર્ણાંક
set_indent(
    પૂર્ણાંક		size,		    // measured in spaces
    પૂર્ણાંક		flags)
{
    char_u	*p;
    char_u	*newline;
    char_u	*oldline;
    char_u	*s;
    પૂર્ણાંક		todo;
    પૂર્ણાંક		ind_len;	    // measured in characters
    પૂર્ણાંક		લીટી_len;	    // size of the line (including the NUL)
    પૂર્ણાંક		doit = FALSE;
    પૂર્ણાંક		ind_done = 0;	    // measured in spaces
#ifdef FEAT_VARTABS
    પૂર્ણાંક		ind_col = 0;
#endif
    પૂર્ણાંક		tab_pad;
    પૂર્ણાંક		retval = FALSE;
    પૂર્ણાંક		orig_char_len = -1; // number of initial whitespace chars when
				    // 'et' and 'pi' are both set

    // First check if there is anything to do and compute the number of
    // characters needed for the indent.
    todo = size;
    ind_len = 0;
    p = oldline = ml_get_curline();
    લીટી_len = ml_get_curline_len() + 1;

    // Calculate the buffer size for the new indent, and check to see if it
    // isn't already set

    // if 'expandtab' isn't set: use TABs; if both 'expandtab' and
    // 'preserveindent' are set count the number of characters at the
    // beginning of the line to be copied
    જો (!curbuf->b_p_et || (!(flags & SIN_ઉમેરો) && curbuf->b_p_pi))
    {
	// If 'preserveindent' is set then reuse as much as possible of
	// the existing indent structure for the new indent
	જો (!(flags & SIN_ઉમેરો) && curbuf->b_p_pi)
	{
	    ind_done = 0;

	    // count as many characters as we can use
	    જ્યારે (todo > 0 && VIM_ISWHITE(*p))
	    {
		જો (*p == TAB)
		{
#ifdef FEAT_VARTABS
		    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#નહીંતર
		    tab_pad = (પૂર્ણાંક)curbuf->b_p_ts
					   - (ind_done % (પૂર્ણાંક)curbuf->b_p_ts);
#endif
		    // stop if this tab will overshoot the target
		    જો (todo < tab_pad)
			તોડો;
		    todo -= tab_pad;
		    ++ind_len;
		    ind_done += tab_pad;
		}
		નહીંતર
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
	    જો (curbuf->b_p_et)
		orig_char_len = ind_len;

	    // Fill to next tabstop with a tab, if possible
#ifdef FEAT_VARTABS
	    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
						curbuf->b_p_vts_array);
#નહીંતર
	    tab_pad = (પૂર્ણાંક)curbuf->b_p_ts - (ind_done % (પૂર્ણાંક)curbuf->b_p_ts);
#endif
	    જો (todo >= tab_pad && orig_char_len == -1)
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
	માટે (;;)
	{
	    tab_pad = tabstop_padding(ind_col, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
	    જો (todo < tab_pad)
		તોડો;
	    જો (*p != TAB)
		doit = TRUE;
	    નહીંતર
		++p;
	    todo -= tab_pad;
	    ++ind_len;
	    ind_col += tab_pad;
	}
#નહીંતર
	જ્યારે (todo >= (પૂર્ણાંક)curbuf->b_p_ts)
	{
	    જો (*p != TAB)
		doit = TRUE;
	    નહીંતર
		++p;
	    todo -= (પૂર્ણાંક)curbuf->b_p_ts;
	    ++ind_len;
	    // ind_done += (int)curbuf->b_p_ts;
	}
#endif
    }
    // count spaces required for indent
    જ્યારે (todo > 0)
    {
	જો (*p != ' ')
	    doit = TRUE;
	નહીંતર
	    ++p;
	--todo;
	++ind_len;
	// ++ind_done;
    }

    // Return if the indent is OK already.
    જો (!doit && !VIM_ISWHITE(*p) && !(flags & SIN_ઉમેરો))
	પાછા FALSE;

    // Allocate memory for the new line.
    જો (flags & SIN_ઉમેરો)
	p = oldline;
    નહીંતર
    {
	p = skipwhite(p);
	લીટી_len -= (પૂર્ણાંક)(p - oldline);
    }

    // If 'preserveindent' and 'expandtab' are both set keep the original
    // characters and allocate accordingly.  We will fill the rest with spaces
    // after the if (!curbuf->b_p_et) below.
    જો (orig_char_len != -1)
    {
	newline = alloc(orig_char_len + size - ind_done + લીટી_len);
	જો (newline == ખાલી)
	    પાછા FALSE;
	todo = size - ind_done;
	ind_len = orig_char_len + todo;    // Set total length of indent in
					   // characters, which may have been
					   // undercounted until now
	p = oldline;
	s = newline;
	જ્યારે (orig_char_len > 0)
	{
	    *s++ = *p++;
	    orig_char_len--;
	}

	// Skip over any additional white space (useful when newindent is less
	// than old)
	જ્યારે (VIM_ISWHITE(*p))
	    ++p;

    }
    નહીંતર
    {
	todo = size;
	newline = alloc(ind_len + લીટી_len);
	જો (newline == ખાલી)
	    પાછા FALSE;
	s = newline;
    }

    // Put the characters in the new line.
    // if 'expandtab' isn't set: use TABs
    જો (!curbuf->b_p_et)
    {
	// If 'preserveindent' is set then reuse as much as possible of
	// the existing indent structure for the new indent
	જો (!(flags & SIN_ઉમેરો) && curbuf->b_p_pi)
	{
	    p = oldline;
	    ind_done = 0;

	    જ્યારે (todo > 0 && VIM_ISWHITE(*p))
	    {
		જો (*p == TAB)
		{
#ifdef FEAT_VARTABS
		    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#નહીંતર
		    tab_pad = (પૂર્ણાંક)curbuf->b_p_ts
					   - (ind_done % (પૂર્ણાંક)curbuf->b_p_ts);
#endif
		    // stop if this tab will overshoot the target
		    જો (todo < tab_pad)
			તોડો;
		    todo -= tab_pad;
		    ind_done += tab_pad;
		}
		નહીંતર
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
#નહીંતર
	    tab_pad = (પૂર્ણાંક)curbuf->b_p_ts - (ind_done % (પૂર્ણાંક)curbuf->b_p_ts);
#endif
	    જો (todo >= tab_pad)
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
	માટે (;;)
	{
	    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
	    જો (todo < tab_pad)
		તોડો;
	    *s++ = TAB;
	    todo -= tab_pad;
	    ind_done += tab_pad;
	}
#નહીંતર
	જ્યારે (todo >= (પૂર્ણાંક)curbuf->b_p_ts)
	{
	    *s++ = TAB;
	    todo -= (પૂર્ણાંક)curbuf->b_p_ts;
	}
#endif
    }
    જ્યારે (todo > 0)
    {
	*s++ = ' ';
	--todo;
    }
    mch_memmove(s, p, (size_t)લીટી_len);

    // Replace the line (unless undo fails).
    જો (!(flags & SIN_UNDO) || u_savesub(curwin->w_cursor.lnum) == OK)
    {
	colnr_T old_offset = (colnr_T)(p - oldline);
	colnr_T new_offset = (colnr_T)(s - newline);

	// this may free "newline"
	ml_replace(curwin->w_cursor.lnum, newline, FALSE);
	જો (flags & SIN_CHANGED)
	    changed_bytes(curwin->w_cursor.lnum, 0);

	// Correct saved cursor position if it is in this line.
	જો (saved_cursor.lnum == curwin->w_cursor.lnum)
	{
	    જો (saved_cursor.col >= old_offset)
		// cursor was after the indent, adjust for the number of
		// bytes added/removed
		saved_cursor.col += ind_len - old_offset;
	    નહીંતર જો (saved_cursor.col >= new_offset)
		// cursor was in the indent, and is now after it, put it back
		// at the start of the indent (replacing spaces with TAB)
		saved_cursor.col = new_offset;
	}
#ifdef FEAT_PROP_POPUP
	જો (curbuf->b_has_textprop)
	{
	    પૂર્ણાંક added = ind_len - old_offset;

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
    નહીંતર
	vim_free(newline);

    curwin->w_cursor.col = ind_len;
    પાછા retval;
}

/*
 * Return the indent of the current line after a number.  Return -1 if no
 * number was found.  Used for 'n' in 'formatoptions': numbered list.
 * Since a pattern is used it can actually handle more than numbers.
 */
    પૂર્ણાંક
get_સંખ્યા_indent(linenr_T lnum)
{
    colnr_T	col;
    pos_T	pos;

    regmatch_T	regmatch;
    પૂર્ણાંક		lead_len = 0;	// length of comment leader

    જો (lnum > curbuf->b_ml.ml_લીટી_ગણતરી)
	પાછા -1;
    pos.lnum = 0;

    // In format_lines() (i.e. not insert mode), fo+=q is needed too...
    જો ((State & MODE_ઉમેરો) || has_format_option(FO_Q_COMS))
	lead_len = get_leader_len(ml_get(lnum), ખાલી, FALSE, TRUE);

    regmatch.regprog = vim_regcomp(curbuf->b_p_flp, RE_MAGIC);
    જો (regmatch.regprog != ખાલી)
    {
	regmatch.rm_ic = FALSE;

	// vim_regexec() expects a pointer to a line.  This lets us
	// start matching for the flp beyond any comment leader...
	જો (vim_regexec(&regmatch, ml_get(lnum) + lead_len, (colnr_T)0))
	{
	    pos.lnum = lnum;
	    pos.col = (colnr_T)(*regmatch.endp - ml_get(lnum));
	    pos.coladd = 0;
	}
	vim_regfree(regmatch.regprog);
    }

    જો (pos.lnum == 0 || *ml_get_pos(&pos) == NUL)
	પાછા -1;
    getvcol(curwin, &pos, &col, ખાલી, ખાલી, 0);
    પાછા (પૂર્ણાંક)col;
}

#જો defined(FEAT_LINEBREAK)
/*
 * Check "briopt" as 'breakindentopt' and update the members of "wp".
 * This is called when 'breakindentopt' is changed and when a window is
 * initialized.
 * Returns FAIL for failure, OK otherwise.
 */
    પૂર્ણાંક
briopt_check(
    char_u	*briopt,	// when NULL: use "wp->w_p_briopt"
    win_T	*wp)		// when NULL: only check "briopt"
{
    char_u	*p;
    પૂર્ણાંક		bri_shift = 0;
    long	bri_min = 20;
    પૂર્ણાંક		bri_sbr = FALSE;
    પૂર્ણાંક		bri_list = 0;
    પૂર્ણાંક		bri_vcol = 0;

    જો (briopt != ખાલી)
	p = briopt;
    નહીંતર
	p = wp->w_p_briopt;

    જ્યારે (*p != NUL)
    {
	// Note: Keep this in sync with p_briopt_values
	જો (STRNCMP(p, "shift:", 6) == 0
		 && ((p[6] == '-' && VIM_ISDIGIT(p[7])) || VIM_ISDIGIT(p[6])))
	{
	    p += 6;
	    bri_shift = getdigits(&p);
	}
	નહીંતર જો (STRNCMP(p, "min:", 4) == 0 && VIM_ISDIGIT(p[4]))
	{
	    p += 4;
	    bri_min = getdigits(&p);
	}
	નહીંતર જો (STRNCMP(p, "sbr", 3) == 0)
	{
	    p += 3;
	    bri_sbr = TRUE;
	}
	નહીંતર જો (STRNCMP(p, "list:", 5) == 0)
	{
	    p += 5;
	    bri_list = getdigits(&p);
	}
	નહીંતર જો (STRNCMP(p, "column:", 7) == 0)
	{
	    p += 7;
	    bri_vcol = getdigits(&p);
	}
	જો (*p != ',' && *p != NUL)
	    પાછા FAIL;
	જો (*p == ',')
	    ++p;
    }

    જો (wp == ખાલી)
	પાછા OK;

    wp->w_briopt_shift = bri_shift;
    wp->w_briopt_min   = bri_min;
    wp->w_briopt_sbr   = bri_sbr;
    wp->w_briopt_list  = bri_list;
    wp->w_briopt_vcol  = bri_vcol;

    પાછા OK;
}

/*
 * Return appropriate space number for breakindent, taking influencing
 * parameters into account. Window must be specified, since it is not
 * necessarily always the current one.
 */
    પૂર્ણાંક
get_breakindent_win(
    win_T	*wp,
    char_u	*લીટી) // start of the line
{
    સ્થિર પૂર્ણાંક	    prev_indent = 0;	// cached indent value
    સ્થિર long	    prev_ts     = 0L;	// cached tabstop value
# ifdef FEAT_VARTABS
    સ્થિર પૂર્ણાંક      *prev_vts = ખાલી;   // cached vartabs values
# endif
    સ્થિર પૂર્ણાંક	    prev_fnum   = 0;	// cached buffer number
    સ્થિર char_u   *prev_લીટી  = ખાલી;	// cached copy of "line"
    સ્થિર varnumber_T prev_tick = 0;   // changedtick of cached value
    સ્થિર પૂર્ણાંક      prev_list = 0;	// cached list indent
    સ્થિર પૂર્ણાંક      prev_listopt = 0;	// cached w_p_briopt_list value
    સ્થિર પૂર્ણાંક      prev_no_ts = FALSE;	// cached no_ts value
    સ્થિર unsigned prev_dy_uhex = 0;	// cached 'display' "uhex" value
    // cached formatlistpat value
    સ્થિર char_u   *prev_flp = ખાલી;
    પૂર્ણાંક		    bri = 0;
    // window width minus window margin space, i.e. what rests for text
    અચલ પૂર્ણાંક	    eff_wwidth = wp->w_width
					  - win_col_off(wp) + win_col_off2(wp);

    // In list mode, if 'listchars' "tab" isn't set, a TAB is displayed as ^I.
    પૂર્ણાંક no_ts = wp->w_p_list && wp->w_lcs_chars.tab1 == NUL;

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
    જો (prev_fnum != wp->w_buffer->b_fnum
	    || prev_ts != wp->w_buffer->b_p_ts
# ifdef FEAT_VARTABS
	    || prev_vts != wp->w_buffer->b_p_vts_array
# endif
	    || prev_tick != CHANGEDTICK(wp->w_buffer)
	    || prev_listopt != wp->w_briopt_list
	    || prev_no_ts != no_ts
	    || prev_dy_uhex != (dy_flags & DY_UHEX)
	    || prev_flp == ખાલી
	    || STRCMP(prev_flp, get_flp_કિંમત(wp->w_buffer)) != 0
	    || prev_લીટી == ખાલી || STRCMP(prev_લીટી, લીટી) != 0
	)
    {
	prev_fnum = wp->w_buffer->b_fnum;
	vim_free(prev_લીટી);
	prev_લીટી = vim_strsave(લીટી);
	prev_ts = wp->w_buffer->b_p_ts;
# ifdef FEAT_VARTABS
	prev_vts = wp->w_buffer->b_p_vts_array;
	જો (wp->w_briopt_vcol == 0)
	    prev_indent = get_indent_str_vtab(લીટી,
				     (પૂર્ણાંક)wp->w_buffer->b_p_ts,
				      wp->w_buffer->b_p_vts_array, no_ts);
# નહીંતર
	જો (wp->w_briopt_vcol == 0)
	    prev_indent = get_indent_str(લીટી,
					(પૂર્ણાંક)wp->w_buffer->b_p_ts, no_ts);
# endif
	prev_tick = CHANGEDTICK(wp->w_buffer);
	prev_listopt = wp->w_briopt_list;
	prev_list = 0;
	prev_no_ts = no_ts;
	prev_dy_uhex = (dy_flags & DY_UHEX);
	vim_free(prev_flp);
	prev_flp = vim_strsave(get_flp_કિંમત(wp->w_buffer));
	// add additional indent for numbered lists
	જો (wp->w_briopt_list != 0 && wp->w_briopt_vcol == 0)
	{
	    regmatch_T	    regmatch;

	    regmatch.regprog = vim_regcomp(prev_flp,
				   RE_MAGIC + RE_STRING + RE_AUTO + RE_STRICT);

	    જો (regmatch.regprog != ખાલી)
	    {
		regmatch.rm_ic = FALSE;
		જો (vim_regexec(&regmatch, લીટી, 0))
		{
		    જો (wp->w_briopt_list > 0)
			prev_list = wp->w_briopt_list;
		    નહીંતર
		    {
			char_u	*ptr = *regmatch.startp;
			char_u	*end_ptr = *regmatch.endp;
			પૂર્ણાંક	indent = 0;

			// Compute the width of the matched text.
			// Use win_chartabsize() so that TAB size is correct,
			// while wrapping is ignored.
			જ્યારે (ptr < end_ptr)
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
    જો (wp->w_briopt_vcol != 0)
    {
	// column value has priority
	bri = wp->w_briopt_vcol;
	prev_list = 0;
    }
    નહીંતર
	bri = prev_indent + wp->w_briopt_shift;

    // Add offset for number column, if 'n' is in 'cpoptions'
    bri += win_col_off2(wp);

    // add additional indent for numbered lists
    જો (wp->w_briopt_list > 0)
	bri += prev_list;

    // indent minus the length of the showbreak string
    જો (wp->w_briopt_sbr)
	bri -= vim_strsize(get_showbreak_કિંમત(wp));


    // never indent past left window margin
    જો (bri < 0)
	bri = 0;

    // always leave at least bri_min characters on the left,
    // if text width is sufficient
    નહીંતર જો (bri > eff_wwidth - wp->w_briopt_min)
	bri = (eff_wwidth - wp->w_briopt_min < 0)
					   ? 0 : eff_wwidth - wp->w_briopt_min;

    પાછા bri;
}
#endif

/*
 * When extra == 0: Return TRUE if the cursor is before or on the first
 *		    non-blank in the line.
 * When extra == 1: Return TRUE if the cursor is before the first non-blank in
 *		    the line.
 */
    પૂર્ણાંક
inindent(પૂર્ણાંક extra)
{
    char_u	*ptr;
    colnr_T	col;

    માટે (col = 0, ptr = ml_get_curline(); VIM_ISWHITE(*ptr); ++col)
	++ptr;
    જો (col >= curwin->w_cursor.col + extra)
	પાછા TRUE;
    નહીંતર
	પાછા FALSE;
}

/*
 * op_reindent - handle reindenting a block of lines.
 */
    શૂન્ય
op_reindent(oparg_T *oap, પૂર્ણાંક (*how)(શૂન્ય))
{
    long	i = 0;
    char_u	*l;
    પૂર્ણાંક		amount;
    linenr_T	first_changed = 0;
    linenr_T	last_changed = 0;
    linenr_T	start_lnum = curwin->w_cursor.lnum;

    // Don't even try when 'modifiable' is off.
    જો (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	પાછા;
    }

    // Save for undo.  Do this once for all lines, much faster than doing this
    // for each line separately, especially when undoing.
    જો (u_savecommon(start_lnum - 1, start_lnum + oap->લીટી_ગણતરી,
				     start_lnum + oap->લીટી_ગણતરી, FALSE) == OK)
	માટે (i = oap->લીટી_ગણતરી; --i >= 0 && !got_int; )
	{
	    // it's a slow thing to do, so give feedback so there's no worry
	    // that the computer's just hung.

	    જો (i > 1
		    && (i % 50 == 0 || i == oap->લીટી_ગણતરી - 1)
		    && oap->લીટી_ગણતરી > p_report)
		smsg(_("%ld lines to indent... "), i);

	    // Be vi-compatible: For lisp indenting the first line is not
	    // indented, unless there is only one line.
	    જો (i != oap->લીટી_ગણતરી - 1 || oap->લીટી_ગણતરી == 1
						     || how != get_lisp_indent)
	    {
		l = skipwhite(ml_get_curline());
		જો (*l == NUL)		    // empty or blank line
		    amount = 0;
		નહીંતર
		    amount = how();	    // get the indent for this line

		જો (amount >= 0 && set_indent(amount, 0))
		{
		    // did change the indent, call changed_lines() later
		    જો (first_changed == 0)
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
    જો (last_changed != 0)
	changed_લીટીઓ(first_changed, 0,
		oap->is_VIsual ? start_lnum + oap->લીટી_ગણતરી :
		last_changed + 1, 0L);
    નહીંતર જો (oap->is_VIsual)
	redraw_curbuf_later(UPD_INVERTED);

    જો (oap->લીટી_ગણતરી > p_report)
    {
	i = oap->લીટી_ગણતરી - (i + 1);
	smsg(NGETTEXT("%ld line indented ",
						 "%ld lines indented ", i), i);
    }
    જો ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// set '[ and '] marks
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
    }
}

/*
 * Return TRUE if lines starting with '#' should be left aligned.
 */
    પૂર્ણાંક
preprocs_ડાબું(શૂન્ય)
{
    પાછા
	(curbuf->b_p_si && !curbuf->b_p_cin) ||
	(curbuf->b_p_cin && in_cinkeys('#', ' ', TRUE)
					   && curbuf->b_ind_hash_comment == 0)
	;
}

/*
 * Return TRUE if the conditions are OK for smart indenting.
 */
    પૂર્ણાંક
may_do_si(શૂન્ય)
{
    પાછા curbuf->b_p_si
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
    શૂન્ય
ins_try_si(પૂર્ણાંક c)
{
    pos_T	*pos, old_pos;
    char_u	*ptr;
    પૂર્ણાંક		i;
    પૂર્ણાંક		temp;

    // do some very smart indenting when entering '{' or '}'
    જો (((did_si || can_si_back) && c == '{')
	    || (can_si && c == '}' && inindent(0)))
    {
	// for '}' set indent equal to indent of line containing matching '{'
	જો (c == '}' && (pos = findmatch(ખાલી, '{')) != ખાલી)
	{
	    old_pos = curwin->w_cursor;
	    // If the matching '{' has a ')' immediately before it (ignoring
	    // white-space), then line up with the start of the line
	    // containing the matching '(' if there is one.  This handles the
	    // case where an "if (..\n..) {" statement continues over multiple
	    // lines -- webb
	    ptr = ml_get(pos->lnum);
	    i = pos->col;
	    જો (i > 0)		// skip blanks before '{'
		જ્યારે (--i > 0 && VIM_ISWHITE(ptr[i]))
		    ;
	    curwin->w_cursor.lnum = pos->lnum;
	    curwin->w_cursor.col = i;
	    જો (ptr[i] == ')' && (pos = findmatch(ખાલી, '(')) != ખાલી)
		curwin->w_cursor = *pos;
	    i = get_indent();
	    curwin->w_cursor = old_pos;
	    જો (State & VREPLACE_FLAG)
		change_indent(INDENT_SET, i, FALSE, NUL, TRUE);
	    નહીંતર
		(શૂન્ય)set_indent(i, SIN_CHANGED);
	}
	નહીંતર જો (curwin->w_cursor.col > 0)
	{
	    // when inserting '{' after "O" reduce indent, but not
	    // more than indent of previous line
	    temp = TRUE;
	    જો (c == '{' && can_si_back && curwin->w_cursor.lnum > 1)
	    {
		old_pos = curwin->w_cursor;
		i = get_indent();
		જ્યારે (curwin->w_cursor.lnum > 1)
		{
		    ptr = skipwhite(ml_get(--(curwin->w_cursor.lnum)));

		    // ignore empty lines and lines starting with '#'.
		    જો (*ptr != '#' && *ptr != NUL)
			તોડો;
		}
		જો (get_indent() >= i)
		    temp = FALSE;
		curwin->w_cursor = old_pos;
	    }
	    જો (temp)
		shift_લીટી(TRUE, FALSE, 1, TRUE);
	}
    }

    // set indent of '#' always to 0
    જો (curwin->w_cursor.col > 0 && can_si && c == '#' && inindent(0))
    {
	// remember current indent for next line
	old_indent = get_indent();
	(શૂન્ય)set_indent(0, SIN_CHANGED);
    }

    // Adjust ai_col, the char at this position can be deleted.
    જો (ai_col > curwin->w_cursor.col)
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
    શૂન્ય
change_indent(
    પૂર્ણાંક		type,
    પૂર્ણાંક		amount,
    પૂર્ણાંક		round,
    પૂર્ણાંક		replaced,	// replaced character, put on replace stack
    પૂર્ણાંક		call_changed_bytes)	// call changed_bytes()
{
    પૂર્ણાંક		vcol;
    પૂર્ણાંક		last_vcol;
    પૂર્ણાંક		insstart_less;		// reduction for Insstart.col
    પૂર્ણાંક		new_cursor_col;
    પૂર્ણાંક		i;
    char_u	*ptr;
    પૂર્ણાંક		save_p_list;
    પૂર્ણાંક		start_col;
    colnr_T	vc;
    colnr_T	orig_col = 0;		// init for GCC
    char_u	*new_લીટી, *orig_લીટી = ખાલી;	// init for GCC

    // MODE_VREPLACE state needs to know what the line was like before changing
    જો (State & VREPLACE_FLAG)
    {
	orig_લીટી = vim_strnsave(ml_get_curline(), ml_get_curline_len());  // Deal with NULL below
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
    જો (new_cursor_col < 0)
	vcol = get_indent() - vcol;

    જો (new_cursor_col > 0)	    // can't fix replace stack
	start_col = -1;

    // Set the new indent.  The cursor will be put on the first non-blank.
    જો (type == INDENT_SET)
	(શૂન્ય)set_indent(amount, call_changed_bytes ? SIN_CHANGED : 0);
    નહીંતર
    {
	પૂર્ણાંક	save_State = State;

	// Avoid being called recursively.
	જો (State & VREPLACE_FLAG)
	    State = MODE_ઉમેરો;
	shift_લીટી(type == INDENT_DEC, round, 1, call_changed_bytes);
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
    જો (new_cursor_col >= 0)
    {
	// When changing the indent while the cursor is touching it, reset
	// Insstart_col to 0.
	જો (new_cursor_col == 0)
	    insstart_less = MAXCOL;
	new_cursor_col += curwin->w_cursor.col;
    }
    નહીંતર જો (!(State & MODE_ઉમેરો))
	new_cursor_col = curwin->w_cursor.col;
    નહીંતર
    {
	chartabsize_T cts;

	// Compute the screen column where the cursor should be.
	vcol = get_indent() - vcol;
	curwin->w_virtcol = (colnr_T)((vcol < 0) ? 0 : vcol);

	// Advance the cursor until we reach the right screen column.
	last_vcol = 0;
	ptr = ml_get_curline();
	init_chartabsize_arg(&cts, curwin, 0, 0, ptr, ptr);
	જ્યારે (cts.cts_vcol <= (પૂર્ણાંક)curwin->w_virtcol)
	{
	    last_vcol = cts.cts_vcol;
	    જો (cts.cts_vcol > 0)
		MB_PTR_ADV(cts.cts_ptr);
	    જો (*cts.cts_ptr == NUL)
		તોડો;
	    cts.cts_vcol += lbr_chartabsize(&cts);
	}
	vcol = last_vcol;
	new_cursor_col = cts.cts_ptr - cts.cts_લીટી;
	clear_chartabsize_arg(&cts);

	// May need to insert spaces to be able to position the cursor on
	// the right screen column.
	જો (vcol != (પૂર્ણાંક)curwin->w_virtcol)
	{
	    curwin->w_cursor.col = (colnr_T)new_cursor_col;
	    i = (પૂર્ણાંક)curwin->w_virtcol - vcol;
	    ptr = alloc(i + 1);
	    જો (ptr != ખાલી)
	    {
		size_t ptrlen;
		new_cursor_col += i;
		ptr[i] = NUL;
		ptrlen = i;
		જ્યારે (--i >= 0)
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

    જો (new_cursor_col <= 0)
	curwin->w_cursor.col = 0;
    નહીંતર
	curwin->w_cursor.col = (colnr_T)new_cursor_col;
    curwin->w_set_curswant = true;
    changed_cline_bef_curs();

    // May have to adjust the start of the insert.
    જો (State & MODE_ઉમેરો)
    {
	જો (curwin->w_cursor.lnum == Insstart.lnum && Insstart.col != 0)
	{
	    જો ((પૂર્ણાંક)Insstart.col <= insstart_less)
		Insstart.col = 0;
	    નહીંતર
		Insstart.col -= insstart_less;
	}
	જો ((પૂર્ણાંક)ai_col <= insstart_less)
	    ai_col = 0;
	નહીંતર
	    ai_col -= insstart_less;
    }

    // For MODE_REPLACE state, may have to fix the replace stack, if it's
    // possible.  If the number of characters before the cursor decreased, need
    // to pop a few characters from the replace stack.
    // If the number of characters before the cursor increased, need to push a
    // few NULs onto the replace stack.
    જો (REPLACE_NORMAL(State) && start_col >= 0)
    {
	જ્યારે (start_col > (પૂર્ણાંક)curwin->w_cursor.col)
	{
	    replace_join(0);	    // remove a NUL from the replace stack
	    --start_col;
	}
	જ્યારે (start_col < (પૂર્ણાંક)curwin->w_cursor.col || replaced)
	{
	    replace_ધકેલો(NUL);
	    જો (replaced)
	    {
		replace_ધકેલો(replaced);
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
    જો (State & VREPLACE_FLAG)
    {
	// If orig_line didn't allocate, just return.  At least we did the job,
	// even if you can't backspace.
	જો (orig_લીટી == ખાલી)
	    પાછા;

	// Save new line
	new_લીટી = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	જો (new_લીટી == ખાલી)
	{
	    vim_free(orig_લીટી);
	    પાછા;
	}

	// We only put back the new line up to the cursor
	new_લીટી[curwin->w_cursor.col] = NUL;

	// Put back original line
	ml_replace(curwin->w_cursor.lnum, orig_લીટી, FALSE);
	curwin->w_cursor.col = orig_col;

	// Backspace from cursor to start of line
	backspace_until_column(0);

	// Insert new stuff into line again
	ins_bytes(new_લીટી);

	vim_free(new_લીટી);
    }
}

/*
 * Copy the indent from ptr to the current line (and fill to size)
 * Leaves the cursor on the first non-blank in the line.
 * Returns TRUE if the line was changed.
 */
    પૂર્ણાંક
copy_indent(પૂર્ણાંક size, char_u *src)
{
    char_u	*p = ખાલી;
    char_u	*લીટી = ખાલી;
    char_u	*s;
    પૂર્ણાંક		todo;
    પૂર્ણાંક		ind_len;
    પૂર્ણાંક		લીટી_len = 0;
    પૂર્ણાંક		tab_pad;
    પૂર્ણાંક		ind_done;
    પૂર્ણાંક		round;
#ifdef FEAT_VARTABS
    પૂર્ણાંક		ind_col;
#endif

    // Round 1: compute the number of characters needed for the indent
    // Round 2: copy the characters.
    માટે (round = 1; round <= 2; ++round)
    {
	todo = size;
	ind_len = 0;
	ind_done = 0;
#ifdef FEAT_VARTABS
	ind_col = 0;
#endif
	s = src;

	// Count/copy the usable portion of the source line
	જ્યારે (todo > 0 && VIM_ISWHITE(*s))
	{
	    જો (*s == TAB)
	    {
#ifdef FEAT_VARTABS
		tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#નહીંતર
		tab_pad = (પૂર્ણાંક)curbuf->b_p_ts
					   - (ind_done % (પૂર્ણાંક)curbuf->b_p_ts);
#endif
		// Stop if this tab will overshoot the target
		જો (todo < tab_pad)
		    તોડો;
		todo -= tab_pad;
		ind_done += tab_pad;
#ifdef FEAT_VARTABS
		ind_col += tab_pad;
#endif
	    }
	    નહીંતર
	    {
		--todo;
		++ind_done;
#ifdef FEAT_VARTABS
		++ind_col;
#endif
	    }
	    ++ind_len;
	    જો (p != ખાલી)
		*p++ = *s;
	    ++s;
	}

	// Fill to next tabstop with a tab, if possible
#ifdef FEAT_VARTABS
	tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#નહીંતર
	tab_pad = (પૂર્ણાંક)curbuf->b_p_ts - (ind_done % (પૂર્ણાંક)curbuf->b_p_ts);
#endif
	જો (todo >= tab_pad && !curbuf->b_p_et)
	{
	    todo -= tab_pad;
	    ++ind_len;
#ifdef FEAT_VARTABS
	    ind_col += tab_pad;
#endif
	    જો (p != ખાલી)
		*p++ = TAB;
	}

	// Add tabs required for indent
	જો (!curbuf->b_p_et)
	{
#ifdef FEAT_VARTABS
	    માટે (;;)
	    {
		tab_pad = tabstop_padding(ind_col, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
		જો (todo < tab_pad)
		    તોડો;
		todo -= tab_pad;
		++ind_len;
		ind_col += tab_pad;
		જો (p != ખાલી)
		    *p++ = TAB;
	    }
#નહીંતર
	    જ્યારે (todo >= (પૂર્ણાંક)curbuf->b_p_ts)
	    {
		todo -= (પૂર્ણાંક)curbuf->b_p_ts;
		++ind_len;
		જો (p != ખાલી)
		    *p++ = TAB;
	    }
#endif
	}

	// Count/add spaces required for indent
	જ્યારે (todo > 0)
	{
	    --todo;
	    ++ind_len;
	    જો (p != ખાલી)
		*p++ = ' ';
	}

	જો (p == ખાલી)
	{
	    // Allocate memory for the result: the copied indent, new indent
	    // and the rest of the line.
	    લીટી_len = ml_get_curline_len() + 1;
	    લીટી = alloc(ind_len + લીટી_len);
	    જો (લીટી == ખાલી)
		પાછા FALSE;
	    p = લીટી;
	}
    }

    // Append the original line
    mch_memmove(p, ml_get_curline(), (size_t)લીટી_len);

    // Replace the line
    ml_replace(curwin->w_cursor.lnum, લીટી, FALSE);

    // Put the cursor after the indent.
    curwin->w_cursor.col = ind_len;
    પાછા TRUE;
}

/*
 * Give a "resulting text too long" error and maybe set got_int.
 */
    સ્થિર શૂન્ય
emsg_text_too_long(શૂન્ય)
{
    emsg(_(e_resulting_text_too_long));
#ifdef FEAT_EVAL
    // when not inside a try/catch set got_int to break out of any loop
    જો (trylevel == 0)
#endif
	got_int = TRUE;
}

/*
 * ":retab".
 */
    શૂન્ય
ex_retab(exarg_T *eap)
{
    linenr_T	lnum;
    પૂર્ણાંક		got_tab = FALSE;
    long	અંક_spaces = 0;
    long	અંક_tabs;
    long	len;
    long	col;
    long	vcol;
    long	start_col = 0;		// For start of white-space string
    long	start_vcol = 0;		// For start of white-space string
    long	old_len;
    long	new_len;
    char_u	*ptr;
    char_u	*new_લીટી = (char_u *)1; // init to non-NULL
    પૂર્ણાંક		did_undo;		// called u_save for current line
#ifdef FEAT_VARTABS
    પૂર્ણાંક		*new_vts_array = ખાલી;
    char_u	*new_ts_str;		// string value of tab argument
#નહીંતર
    પૂર્ણાંક		temp;
    પૂર્ણાંક		new_ts = 0;
#endif
    પૂર્ણાંક		save_list;
    linenr_T	first_લીટી = 0;		// first changed line
    linenr_T	last_લીટી = 0;		// last changed line
    પૂર્ણાંક		is_indent_only = 0;	// Only process leading whitespace

    save_list = curwin->w_p_list;
    curwin->w_p_list = 0;	    // don't want list mode here

    ptr = eap->arg;
    જો (STRNCMP(ptr, "-indentonly", 11) == 0 && IS_WHITE_OR_NUL(ptr[11]))
    {
	is_indent_only = 1;
	ptr = skipwhite(ptr + 11);
    }

#ifdef FEAT_VARTABS
    new_ts_str = ptr;
    જો (tabstop_set(ptr, &new_vts_array) == FAIL)
	પાછા;
    જ્યારે (vim_isdigit(*ptr) || *ptr == ',')
	++ptr;

    // This ensures that either new_vts_array and new_ts_str are freshly
    // allocated, or new_vts_array points to an existing array and new_ts_str
    // is null.
    જો (new_vts_array == ખાલી)
    {
	new_vts_array = curbuf->b_p_vts_array;
	new_ts_str = ખાલી;
    }
    નહીંતર
	new_ts_str = vim_strnsave(new_ts_str, ptr - new_ts_str);
#નહીંતર
    જો (ptr[0] != NUL && (ptr[0] != '0' || ptr[1] != NUL))
    {
	char_u	*end;

	જો (strtol((અક્ષર *)ptr, (અક્ષર **)&end, 10) <= 0)
	{
	    જો (ptr != end)
		emsg(_(e_argument_must_be_positive));
	    નહીંતર
		semsg(_(e_invalid_argument_str), ptr);
	    પાછા;
	}
	new_ts = getdigits(&ptr);
	જો (new_ts < 0 || new_ts > TABSTOP_MAX)
	{
	    semsg(_(e_invalid_argument_str), eap->arg);
	    પાછા;
	}
    }
    જો (new_ts == 0)
	new_ts = curbuf->b_p_ts;
#endif
    માટે (lnum = eap->line1; !got_int && lnum <= eap->line2; ++lnum)
    {
	ptr = ml_get(lnum);
	old_len = ml_get_len(lnum);
	col = 0;
	vcol = 0;
	did_undo = FALSE;
	માટે (;;)
	{
	    જો (VIM_ISWHITE(ptr[col]))
	    {
		જો (!got_tab && અંક_spaces == 0)
		{
		    // First consecutive white-space
		    start_vcol = vcol;
		    start_col = col;
		}
		જો (ptr[col] == ' ')
		    અંક_spaces++;
		નહીંતર
		    got_tab = TRUE;
	    }
	    નહીંતર
	    {
		જો (got_tab || (eap->forceit && અંક_spaces > 1))
		{
		    // Retabulate this string of white-space

		    // len is virtual length of white string
		    len = અંક_spaces = vcol - start_vcol;
		    અંક_tabs = 0;
		    જો (!curbuf->b_p_et)
		    {
#ifdef FEAT_VARTABS
			પૂર્ણાંક t, s;

			tabstop_fromto(start_vcol, vcol,
					curbuf->b_p_ts, new_vts_array, &t, &s);
			અંક_tabs = t;
			અંક_spaces = s;
#નહીંતર
			temp = new_ts - (start_vcol % new_ts);
			જો (અંક_spaces >= temp)
			{
			    અંક_spaces -= temp;
			    અંક_tabs++;
			}
			અંક_tabs += અંક_spaces / new_ts;
			અંક_spaces -= (અંક_spaces / new_ts) * new_ts;
#endif
		    }
		    જો (curbuf->b_p_et || got_tab ||
					(અંક_spaces + અંક_tabs < len))
		    {
			જો (did_undo == FALSE)
			{
			    did_undo = TRUE;
			    જો (u_save((linenr_T)(lnum - 1),
						(linenr_T)(lnum + 1)) == FAIL)
			    {
				new_લીટી = ખાલી;	// flag out-of-memory
				તોડો;
			    }
			}

			// len is actual number of white characters used
			len = અંક_spaces + અંક_tabs;
			new_len = old_len - col + start_col + len + 1;
			જો (new_len <= 0 || new_len >= MAXCOL)
			{
			    emsg_text_too_long();
			    તોડો;
			}
			new_લીટી = alloc(new_len);
			જો (new_લીટી == ખાલી)
			    તોડો;
			જો (start_col > 0)
			    mch_memmove(new_લીટી, ptr, (size_t)start_col);
			mch_memmove(new_લીટી + start_col + len,
				      ptr + col, (size_t)(old_len - col + 1));
			ptr = new_લીટી + start_col;
			માટે (col = 0; col < len; col++)
			    ptr[col] = (col < અંક_tabs) ? '\t' : ' ';
			જો (ml_replace(lnum, new_લીટી, FALSE) == OK)
			    // "new_line" may have been copied
			    new_લીટી = curbuf->b_ml.ml_લીટી_ptr;
			જો (first_લીટી == 0)
			    first_લીટી = lnum;
			last_લીટી = lnum;
			ptr = new_લીટી;
			old_len = new_len - 1;
			col = start_col + len;
		    }
		}
		got_tab = FALSE;
		અંક_spaces = 0;

		જો (is_indent_only)
		    તોડો;
	    }
	    જો (ptr[col] == NUL)
		તોડો;
	    vcol += chartabsize(ptr + col, (colnr_T)vcol);
	    જો (vcol >= MAXCOL)
	    {
		emsg_text_too_long();
		તોડો;
	    }
	    જો (has_mbyte)
		col += (*mb_ptr2len)(ptr + col);
	    નહીંતર
		++col;
	}
	જો (new_લીટી == ખાલી)		    // out of memory
	    તોડો;
	લીટી_breakcheck();
    }
    જો (got_int)
	emsg(_(e_interrupted));

#ifdef FEAT_VARTABS
    // If a single value was given then it can be considered equal to
    // either the value of 'tabstop' or the value of 'vartabstop'.
    જો (tabstop_ગણતરી(curbuf->b_p_vts_array) == 0
	&& tabstop_ગણતરી(new_vts_array) == 1
	&& curbuf->b_p_ts == tabstop_first(new_vts_array))
	; // not changed
    નહીંતર જો (tabstop_ગણતરી(curbuf->b_p_vts_array) > 0
	&& tabstop_eq(curbuf->b_p_vts_array, new_vts_array))
	; // not changed
    નહીંતર
	redraw_curbuf_later(UPD_NOT_VALID);
#નહીંતર
    જો (curbuf->b_p_ts != new_ts)
	redraw_curbuf_later(UPD_NOT_VALID);
#endif
    જો (first_લીટી != 0)
	changed_લીટીઓ(first_લીટી, 0, last_લીટી + 1, 0L);

    curwin->w_p_list = save_list;	// restore 'list'

#ifdef FEAT_VARTABS
    જો (new_ts_str != ખાલી)		// set the new tabstop
    {
	// If 'vartabstop' is in use or if the value given to retab has more
	// than one tabstop then update 'vartabstop'.
	પૂર્ણાંક *old_vts_ary = curbuf->b_p_vts_array;

	જો (tabstop_ગણતરી(old_vts_ary) > 0 || tabstop_ગણતરી(new_vts_array) > 1)
	{
	    set_string_option_direct((char_u *)"vts", -1, new_ts_str,
							OPT_FREE|OPT_LOCAL, 0);
	    curbuf->b_p_vts_array = new_vts_array;
	    vim_free(old_vts_ary);
	}
	નહીંતર
	{
	    // 'vartabstop' wasn't in use and a single value was given to
	    // retab then update 'tabstop'.
	    curbuf->b_p_ts = tabstop_first(new_vts_array);
	    vim_free(new_vts_array);
	}
	vim_free(new_ts_str);
    }
#નહીંતર
    curbuf->b_p_ts = new_ts;
#endif
    coladvance(curwin->w_curswant);

    u_clearline();
}

#જો defined(FEAT_EVAL)
/*
 * Get indent level from 'indentexpr'.
 */
    પૂર્ણાંક
get_પદાવલિ_indent(શૂન્ય)
{
    પૂર્ણાંક		indent = -1;
    char_u	*inde_copy;
    pos_T	save_pos;
    colnr_T	save_curswant;
    પૂર્ણાંક		save_set_curswant;
    પૂર્ણાંક		save_State;
    પૂર્ણાંક		use_sandbox = was_set_insecurely(curwin,
					    (char_u *)"indentexpr", OPT_LOCAL);
    sctx_T	save_sctx = current_sctx;

    // Save and restore cursor position and curswant, in case it was changed
    // via :normal commands
    save_pos = curwin->w_cursor;
    save_curswant = curwin->w_curswant;
    save_set_curswant = curwin->w_set_curswant;
    set_vim_var_nr(VV_LNUM, curwin->w_cursor.lnum);
    જો (use_sandbox)
	++sandbox;
    ++textlock;
    current_sctx = curbuf->b_p_script_ctx[BV_INDE];

    // Need to make a copy, the 'indentexpr' option could be changed while
    // evaluating it.
    inde_copy = vim_strsave(curbuf->b_p_inde);
    જો (inde_copy != ખાલી)
    {
	indent = (પૂર્ણાંક)eval_to_સંખ્યા(inde_copy, TRUE);
	vim_free(inde_copy);
    }

    જો (use_sandbox)
	--sandbox;
    --textlock;
    current_sctx = save_sctx;

    // Restore the cursor position so that 'indentexpr' doesn't need to.
    // Pretend to be in Insert mode, allow cursor past end of line for "o"
    // command.
    save_State = State;
    State = MODE_ઉમેરો;
    curwin->w_cursor = save_pos;
    curwin->w_curswant = save_curswant;
    curwin->w_set_curswant = save_set_curswant;
    check_cursor();
    State = save_State;

    // Reset did_throw, unless 'debug' has "throw" and inside a try/catch.
    જો (did_throw && (vim_strchr(p_debug, 't') == ખાલી || trylevel == 0))
    {
	handle_did_throw();
	did_throw = FALSE;
    }

    // If there is an error, just keep the current indent.
    જો (indent < 0)
	indent = get_indent();

    પાછા indent;
}
#endif

    સ્થિર પૂર્ણાંક
lisp_match(char_u *p)
{
    char_u	buf[LSIZE];
    પૂર્ણાંક		len;
    char_u	*word = *curbuf->b_p_lw != NUL ? curbuf->b_p_lw : p_lispwords;

    જ્યારે (*word != NUL)
    {
	len = copy_option_part(&word, buf, LSIZE, ",");
	જો (STRNCMP(buf, p, len) == 0 && IS_WHITE_OR_NUL(p[len]))
	    પાછા TRUE;
    }
    પાછા FALSE;
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
    પૂર્ણાંક
get_lisp_indent(શૂન્ય)
{
    pos_T	*pos, realpos, paren;
    પૂર્ણાંક		amount;
    char_u	*that;
    colnr_T	col;
    colnr_T	firsttry;
    પૂર્ણાંક		parencount, quotecount;
    પૂર્ણાંક		vi_lisp;

    // Set vi_lisp to use the vi-compatible method
    vi_lisp = (vim_strchr(p_cpo, CPO_LISP) != ખાલી);

    realpos = curwin->w_cursor;
    curwin->w_cursor.col = 0;

    જો ((pos = findmatch(ખાલી, '(')) == ખાલી)
	pos = findmatch(ખાલી, '[');
    નહીંતર
    {
	paren = *pos;
	pos = findmatch(ખાલી, '[');
	જો (pos == ખાલી || LT_POSP(pos, &paren))
	    pos = &paren;
    }
    જો (pos != ખાલી)
    {
	// Extra trick: Take the indent of the first previous non-white
	// line that is at the same () level.
	amount = -1;
	parencount = 0;

	જ્યારે (--curwin->w_cursor.lnum >= pos->lnum)
	{
	    જો (linewhite(curwin->w_cursor.lnum))
		ચાલુ;
	    માટે (that = ml_get_curline(); *that != NUL; ++that)
	    {
		જો (*that == ';')
		{
		    જ્યારે (*(that + 1) != NUL)
			++that;
		    ચાલુ;
		}
		જો (*that == '\\')
		{
		    જો (*(that + 1) != NUL)
			++that;
		    ચાલુ;
		}
		જો (*that == '"' && *(that + 1) != NUL)
		{
		    જ્યારે (*++that && *that != '"')
		    {
			// skipping escaped characters in the string
			જો (*that == '\\')
			{
			    જો (*++that == NUL)
				તોડો;
			    જો (that[1] == NUL)
			    {
				++that;
				તોડો;
			    }
			}
		    }
		    જો (*that == NUL)
			તોડો;
		}
		જો (*that == '(' || *that == '[')
		    ++parencount;
		નહીંતર જો (*that == ')' || *that == ']')
		    --parencount;
	    }
	    જો (parencount == 0)
	    {
		amount = get_indent();
		તોડો;
	    }
	}

	જો (amount == -1)
	{
	    curwin->w_cursor.lnum = pos->lnum;
	    curwin->w_cursor.col = pos->col;
	    col = pos->col;

	    that = ml_get_curline();

	    જો (vi_lisp && get_indent() == 0)
		amount = 2;
	    નહીંતર
	    {
		char_u		*લીટી = that;
		chartabsize_T	cts;

		init_chartabsize_arg(&cts, curwin, pos->lnum, 0, લીટી, લીટી);
		જ્યારે (*cts.cts_ptr != NUL && col > 0)
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

		જો (!vi_lisp && (*that == '(' || *that == '[')
						      && lisp_match(that + 1))
		    amount += 2;
		નહીંતર
		{
		    જો (*that != NUL)
		    {
			that++;
			amount++;
		    }
		    firsttry = amount;

		    init_chartabsize_arg(&cts, curwin, (colnr_T)(that - લીટી),
							   amount, લીટી, that);
		    જ્યારે (VIM_ISWHITE(*cts.cts_ptr))
		    {
			cts.cts_vcol += lbr_chartabsize(&cts);
			++cts.cts_ptr;
		    }
		    that = cts.cts_ptr;
		    amount = cts.cts_vcol;
		    clear_chartabsize_arg(&cts);

		    જો (*that && *that != ';') // not a comment line
		    {
			// test *that != '(' to accommodate first let/do
			// argument if it is more than one line
			જો (!vi_lisp && *that != '(' && *that != '[')
			    firsttry++;

			parencount = 0;
			quotecount = 0;

			init_chartabsize_arg(&cts, curwin,
				   (colnr_T)(that - લીટી), amount, લીટી, that);
			જો (vi_lisp
				|| (*that != '"'
				    && *that != '\''
				    && *that != '#'
				    && (*that < '0' || *that > '9')))
			{
			    જ્યારે (*cts.cts_ptr
				    && (!VIM_ISWHITE(*cts.cts_ptr)
					|| quotecount
					|| parencount)
				    && (!((*cts.cts_ptr == '('
							|| *cts.cts_ptr == '[')
					    && !quotecount
					    && !parencount
					    && vi_lisp)))
			    {
				જો (*cts.cts_ptr == '"')
				    quotecount = !quotecount;
				જો ((*cts.cts_ptr == '(' || *cts.cts_ptr == '[')
							       && !quotecount)
				    ++parencount;
				જો ((*cts.cts_ptr == ')' || *cts.cts_ptr == ']')
							       && !quotecount)
				    --parencount;
				જો (*cts.cts_ptr == '\\'
						    && *(cts.cts_ptr+1) != NUL)
				    cts.cts_vcol += lbr_chartabsize_adv(&cts);
				cts.cts_vcol += lbr_chartabsize_adv(&cts);
			    }
			}
			જ્યારે (VIM_ISWHITE(*cts.cts_ptr))
			{
			    cts.cts_vcol += lbr_chartabsize(&cts);
			    ++cts.cts_ptr;
			}
			that = cts.cts_ptr;
			amount = cts.cts_vcol;
			clear_chartabsize_arg(&cts);

			જો (!*that || *that == ';')
			    amount = firsttry;
		    }
		}
	    }
	}
    }
    નહીંતર
	amount = 0;	// no matching '(' or '[' found, use zero indent

    curwin->w_cursor = realpos;

    પાછા amount;
}

/*
 * Re-indent the current line, based on the current contents of it and the
 * surrounding lines. Fixing the cursor position seems really easy -- I'm very
 * confused what all the part that handles Control-T is doing that I'm not.
 * "get_the_indent" should be get_c_indent, get_expr_indent or get_lisp_indent.
 */

    શૂન્ય
fixthisline(પૂર્ણાંક (*get_the_indent)(શૂન્ય))
{
    પૂર્ણાંક amount = get_the_indent();

    જો (amount < 0)
	પાછા;

    change_indent(INDENT_SET, amount, FALSE, 0, TRUE);
    જો (linewhite(curwin->w_cursor.lnum))
	did_ai = TRUE;	// delete the indent if the line stays empty
}

/*
 * Return TRUE if 'indentexpr' should be used for Lisp indenting.
 * Caller may want to check 'autoindent'.
 */
    પૂર્ણાંક
use_indentexpr_for_lisp(શૂન્ય)
{
#ifdef FEAT_EVAL
    પાછા curbuf->b_p_lisp
		&& *curbuf->b_p_inde != NUL
		&& STRCMP(curbuf->b_p_lop, "expr:1") == 0;
#નહીંતર
    પાછા FALSE;
#endif
}

/*
 * Fix indent for 'lisp' and 'cindent'.
 */
    શૂન્ય
fix_indent(શૂન્ય)
{
    જો (p_paste)
	પાછા;  // no auto-indenting when 'paste' is set
    જો (curbuf->b_p_lisp && curbuf->b_p_ai)
    {
	જો (use_indentexpr_for_lisp())
	    do_c_પદાવલિ_indent();
	નહીંતર
	    fixthisline(get_lisp_indent);
    }
    નહીંતર જો (cindent_on())
	do_c_પદાવલિ_indent();
}

#જો defined(FEAT_EVAL)
/*
 * "indent()" function
 */
    શૂન્ય
f_indent(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    જો (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	પાછા;

    lnum = tv_get_lnum(argvars);
    જો (lnum >= 1 && lnum <= curbuf->b_ml.ml_લીટી_ગણતરી)
	rettv->vval.v_સંખ્યા = get_indent_lnum(lnum);
    નહીંતર
    {
	જો (in_vim9script())
	    semsg(_(e_invalid_લીટી_સંખ્યા_nr), lnum);
	rettv->vval.v_સંખ્યા = -1;
    }
}

/*
 * "lispindent(lnum)" function
 */
    શૂન્ય
f_lispindent(typval_T *argvars UNUSED, typval_T *rettv)
{
    pos_T	pos;
    linenr_T	lnum;

    જો (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	પાછા;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    જો (lnum >= 1 && lnum <= curbuf->b_ml.ml_લીટી_ગણતરી)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_સંખ્યા = get_lisp_indent();
	curwin->w_cursor = pos;
    }
    નહીંતર જો (in_vim9script())
	semsg(_(e_invalid_લીટી_સંખ્યા_nr), lnum);
    નહીંતર
	rettv->vval.v_સંખ્યા = -1;
}
#endif
