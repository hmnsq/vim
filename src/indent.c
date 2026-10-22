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

#अन्तर्भाव "vim.h"

#यदि defined(FEAT_VARTABS)

/*
 * Set the integer values corresponding to the string setting of 'vartabstop'.
 * "array" will be set, caller must free it if needed.
 * Return FAIL for an error.
 */
    पूर्णाङ्क
tabstop_set(char_u *var, पूर्णाङ्क **array)
{
    पूर्णाङ्क	    valcount = 1;
    पूर्णाङ्क	    t;
    char_u  *cp;

    यदि (var[0] == NUL || (var[0] == '0' && var[1] == NUL))
    {
	*array = रिक्त;
	प्रत्यागम OK;
    }

    प्रति (cp = var; *cp != NUL; ++cp)
    {
	यदि (cp == var || cp[-1] == ',')
	{
	    char_u *end;

	    यदि (म_से_दीर्घ((वर्ण *)cp, (वर्ण **)&end, 10) <= 0)
	    {
		यदि (cp != end)
		    emsg(_(e_argument_must_be_positive));
		अन्यथा
		    semsg(_(e_invalid_argument_str), cp);
		प्रत्यागम FAIL;
	    }
	}

	यदि (VIM_ISDIGIT(*cp))
	    अनुवर्त;
	यदि (cp[0] == ',' && cp > var && cp[-1] != ',' && cp[1] != NUL)
	{
	    ++valcount;
	    अनुवर्त;
	}
	semsg(_(e_invalid_argument_str), var);
	प्रत्यागम FAIL;
    }

    *array = ALLOC_MULT(पूर्णाङ्क, valcount + 1);
    यदि (*array == रिक्त)
	प्रत्यागम FAIL;
    (*array)[0] = valcount;

    t = 1;
    प्रति (cp = var; *cp != NUL;)
    {
	पूर्णाङ्क n = म_से_प((वर्ण *)cp);

	// Catch negative values, overflow and ridiculous big values.
	यदि (n <= 0 || n > TABSTOP_MAX)
	{
	    semsg(_(e_invalid_argument_str), cp);
	    VIM_CLEAR(*array);
	    प्रत्यागम FAIL;
	}
	(*array)[t++] = n;
	यावत् (*cp != NUL && *cp != ',')
	    ++cp;
	यदि (*cp != NUL)
	    ++cp;
    }

    प्रत्यागम OK;
}

/*
 * Calculate the number of screen spaces a tab will occupy.
 * If "vts" is set then the tab widths are taken from that array,
 * otherwise the value of ts is used.
 */
    पूर्णाङ्क
tabstop_padding(colnr_T col, पूर्णाङ्क ts_arg, पूर्णाङ्क *vts)
{
    पूर्णाङ्क		ts = ts_arg == 0 ? 8 : ts_arg;
    पूर्णाङ्क		tabcount;
    colnr_T	tabcol = 0;
    पूर्णाङ्क		t;
    पूर्णाङ्क		padding = 0;

    यदि (vts == रिक्त || vts[0] == 0)
	प्रत्यागम ts - (col % ts);

    tabcount = vts[0];

    प्रति (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	यदि (tabcol > col)
	{
	    padding = (पूर्णाङ्क)(tabcol - col);
	    भङ्ग;
	}
    }
    यदि (t > tabcount)
	padding = vts[tabcount] - (पूर्णाङ्क)((col - tabcol) % vts[tabcount]);

    प्रत्यागम padding;
}

/*
 * Find the size of the tab that covers a particular column.
 *
 * If this is being called as part of a shift operation, col is not the cursor
 * column but is the column number to the left of the first non-whitespace
 * character in the line.  If the shift is to the left (left = TRUE), then
 * return the size of the tab interval to the left of the column.
 */
    पूर्णाङ्क
tabstop_at(colnr_T col, पूर्णाङ्क ts, पूर्णाङ्क *vts, पूर्णाङ्क बायाँ)
{
    पूर्णाङ्क		tabcount;	// Number of tab stops in the list of variable
				// tab stops.
    colnr_T	tabcol = 0;	// Column of the tab stop under consideration.
    पूर्णाङ्क		t;		// Tabstop index in the list of variable tab
				// stops.
    पूर्णाङ्क		tab_size = 0;	// Size of the tab stop interval to the right
				// or left of the col.

    यदि (vts == 0 || vts[0] == 0)
	प्रत्यागम ts;

    tabcount = vts[0];
    प्रति (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	यदि (tabcol > col)
	{
	    // If shifting left (left != 0), and if the column to the left of
	    // the first first non-blank character (col) in the line is
	    // already to the left of the first tabstop, set the shift amount
	    // (tab_size) to just enough to shift the line to the left margin.
	    // The value doesn't seem to matter as long as it is at least that
	    // distance.
	    यदि (बायाँ && (t == 1))
		tab_size = col;
	    अन्यथा
		tab_size = vts[t - (बायाँ ? 1 : 0)];
	    भङ्ग;
	}
    }
    यदि (t > tabcount)		// If the value of the index t is beyond the
				// end of the list, use the tab stop value at
				// the end of the list.
	tab_size = vts[tabcount];

    प्रत्यागम tab_size;
}

/*
 * Find the column on which a tab starts.
 */
    colnr_T
tabstop_start(colnr_T col, पूर्णाङ्क ts, पूर्णाङ्क *vts)
{
    पूर्णाङ्क		tabcount;
    colnr_T	tabcol = 0;
    पूर्णाङ्क		t;
    पूर्णाङ्क		excess;

    यदि (vts == रिक्त || vts[0] == 0)
	प्रत्यागम col - col % ts;

    tabcount = vts[0];
    प्रति (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	यदि (tabcol > col)
	    प्रत्यागम tabcol - vts[t];
    }

    excess = tabcol % vts[tabcount];
    प्रत्यागम col - (col - excess) % vts[tabcount];
}

/*
 * Find the number of tabs and spaces necessary to get from one column
 * to another.
 */
    शून्य
tabstop_fromto(
	colnr_T start_col,
	colnr_T end_col,
	पूर्णाङ्क	ts_arg,
	पूर्णाङ्क	*vts,
	पूर्णाङ्क	*ntabs,
	पूर्णाङ्क	*nspcs)
{
    पूर्णाङ्क		spaces = end_col - start_col;
    colnr_T	tabcol = 0;
    पूर्णाङ्क		padding = 0;
    पूर्णाङ्क		tabcount;
    पूर्णाङ्क		t;
    पूर्णाङ्क		ts = ts_arg == 0 ? curbuf->b_p_ts : ts_arg;

    यदि (vts == रिक्त || vts[0] == 0)
    {
	पूर्णाङ्क tabs = 0;
	पूर्णाङ्क initspc = 0;

	initspc = ts - (start_col % ts);
	यदि (spaces >= initspc)
	{
	    spaces -= initspc;
	    tabs++;
	}
	tabs += spaces / ts;
	spaces -= (spaces / ts) * ts;

	*ntabs = tabs;
	*nspcs = spaces;
	प्रत्यागम;
    }

    // Find the padding needed to reach the next tabstop.
    tabcount = vts[0];
    प्रति (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	यदि (tabcol > start_col)
	{
	    padding = (पूर्णाङ्क)(tabcol - start_col);
	    भङ्ग;
	}
    }
    यदि (t > tabcount)
	padding = vts[tabcount] - (पूर्णाङ्क)((start_col - tabcol) % vts[tabcount]);

    // If the space needed is less than the padding no tabs can be used.
    यदि (spaces < padding)
    {
	*ntabs = 0;
	*nspcs = spaces;
	प्रत्यागम;
    }

    *ntabs = 1;
    spaces -= padding;

    // At least one tab has been used. See if any more will fit.
    यावत् (spaces != 0 && ++t <= tabcount)
    {
	padding = vts[t];
	यदि (spaces < padding)
	{
	    *nspcs = spaces;
	    प्रत्यागम;
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
    स्थिर पूर्णाङ्क
tabstop_eq(पूर्णाङ्क *ts1, पूर्णाङ्क *ts2)
{
    पूर्णाङ्क		t;

    यदि ((ts1 == 0 && ts2) || (ts1 && ts2 == 0))
	प्रत्यागम FALSE;
    यदि (ts1 == ts2)
	प्रत्यागम TRUE;
    यदि (ts1[0] != ts2[0])
	प्रत्यागम FALSE;

    प्रति (t = 1; t <= ts1[0]; ++t)
	यदि (ts1[t] != ts2[t])
	    प्रत्यागम FALSE;

    प्रत्यागम TRUE;
}

# यदि defined(FEAT_BEVAL)
/*
 * Copy a tabstop array, allocating space for the new array.
 */
    पूर्णाङ्क *
tabstop_copy(पूर्णाङ्क *oldts)
{
    पूर्णाङ्क		*newts;
    पूर्णाङ्क		t;

    यदि (oldts == रिक्त)
	प्रत्यागम रिक्त;
    newts = ALLOC_MULT(पूर्णाङ्क, oldts[0] + 1);
    यदि (newts != रिक्त)
	प्रति (t = 0; t <= oldts[0]; ++t)
	    newts[t] = oldts[t];
    प्रत्यागम newts;
}
# endif

/*
 * Return a count of the number of tabstops.
 */
    पूर्णाङ्क
tabstop_गिनती(पूर्णाङ्क *ts)
{
    प्रत्यागम ts != रिक्त ? ts[0] : 0;
}

/*
 * Return the first tabstop, or 8 if there are no tabstops defined.
 */
    पूर्णाङ्क
tabstop_first(पूर्णाङ्क *ts)
{
    प्रत्यागम ts != रिक्त ? ts[1] : 8;
}

#endif

/*
 * Return the effective shiftwidth value for current buffer, using the
 * 'tabstop' value when 'shiftwidth' is zero.
 */
    दीर्घ
get_sw_मूल्य(buf_T *buf)
{
    प्रत्यागम get_sw_मूल्य_col(buf, 0, FALSE);
}

/*
 * Idem, using "pos".
 */
    स्थिर दीर्घ
get_sw_मूल्य_pos(buf_T *buf, pos_T *pos, पूर्णाङ्क बायाँ)
{
    pos_T save_cursor = curwin->w_cursor;
    दीर्घ sw_मूल्य;

    curwin->w_cursor = *pos;
    sw_मूल्य = get_sw_मूल्य_col(buf, get_nolist_virtcol(), बायाँ);
    curwin->w_cursor = save_cursor;
    प्रत्यागम sw_मूल्य;
}

/*
 * Idem, using the first non-black in the current line.
 */
    दीर्घ
get_sw_मूल्य_indent(buf_T *buf, पूर्णाङ्क बायाँ)
{
    pos_T pos = curwin->w_cursor;

    pos.col = getwhitecols_curline();
    प्रत्यागम get_sw_मूल्य_pos(buf, &pos, बायाँ);
}

/*
 * Idem, using virtual column "col".
 */
    दीर्घ
get_sw_मूल्य_col(buf_T *buf, colnr_T col UNUSED, पूर्णाङ्क बायाँ UNUSED)
{
    प्रत्यागम buf->b_p_sw ? buf->b_p_sw :
#ifdef FEAT_VARTABS
	tabstop_at(col, buf->b_p_ts, buf->b_p_vts_array, बायाँ);
#अन्यथा
	buf->b_p_ts;
#endif
}

/*
 * Return the effective softtabstop value for the current buffer, using the
 * 'shiftwidth' value when 'softtabstop' is negative.
 */
    दीर्घ
get_sts_मूल्य(शून्य)
{
    प्रत्यागम curbuf->b_p_sts < 0 ? get_sw_मूल्य(curbuf) : curbuf->b_p_sts;
}

/*
 * Count the size (in window cells) of the indent in the current line.
 */
    पूर्णाङ्क
get_indent(शून्य)
{
#ifdef FEAT_VARTABS
    प्रत्यागम get_indent_str_vtab(ml_get_curline(), (पूर्णाङ्क)curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#अन्यथा
    प्रत्यागम get_indent_str(ml_get_curline(), (पूर्णाङ्क)curbuf->b_p_ts, FALSE);
#endif
}

/*
 * Count the size (in window cells) of the indent in line "lnum".
 */
    पूर्णाङ्क
get_indent_lnum(linenr_T lnum)
{
#ifdef FEAT_VARTABS
    प्रत्यागम get_indent_str_vtab(ml_get(lnum), (पूर्णाङ्क)curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#अन्यथा
    प्रत्यागम get_indent_str(ml_get(lnum), (पूर्णाङ्क)curbuf->b_p_ts, FALSE);
#endif
}

#यदि defined(FEAT_FOLDING)
/*
 * Count the size (in window cells) of the indent in line "lnum" of buffer
 * "buf".
 */
    पूर्णाङ्क
get_indent_buf(buf_T *buf, linenr_T lnum)
{
# ifdef FEAT_VARTABS
    प्रत्यागम get_indent_str_vtab(ml_get_buf(buf, lnum, FALSE),
			       (पूर्णाङ्क)buf->b_p_ts, buf->b_p_vts_array, FALSE);
# अन्यथा
    प्रत्यागम get_indent_str(ml_get_buf(buf, lnum, FALSE), (पूर्णाङ्क)buf->b_p_ts, FALSE);
# endif
}
#endif

/*
 * count the size (in window cells) of the indent in line "ptr", with
 * 'tabstop' at "ts"
 */
    पूर्णाङ्क
get_indent_str(
    char_u	*ptr,
    पूर्णाङ्क		ts,
    पूर्णाङ्क		no_ts) // if TRUE, count a tab as ^I
{
    पूर्णाङ्क		गिनती = 0;

    प्रति ( ; *ptr; ++ptr)
    {
	यदि (*ptr == TAB)    // count a tab for what it is worth
	{
	    यदि (!no_ts)
		गिनती += ts - (गिनती % ts);
	    अन्यथा
		गिनती += ptr2cells(ptr);
	}
	अन्यथा यदि (*ptr == ' ')
	    ++गिनती;		// count a space for one
	अन्यथा
	    भङ्ग;
    }
    प्रत्यागम गिनती;
}

#यदि defined(FEAT_VARTABS)
/*
 * Count the size (in window cells) of the indent in line "ptr", using
 * variable tabstops.
 * If "no_ts" is TRUE, count a tab as ^I.
 */
    पूर्णाङ्क
get_indent_str_vtab(char_u *ptr, पूर्णाङ्क ts, पूर्णाङ्क *vts, पूर्णाङ्क no_ts)
{
    पूर्णाङ्क		गिनती = 0;

    प्रति ( ; *ptr; ++ptr)
    {
	यदि (*ptr == TAB)    // count a tab for what it is worth
	{
	    यदि (!no_ts)
		गिनती += tabstop_padding(गिनती, ts, vts);
	    अन्यथा
		गिनती += ptr2cells(ptr);
	}
	अन्यथा यदि (*ptr == ' ')
	    ++गिनती;		// count a space for one
	अन्यथा
	    भङ्ग;
    }
    प्रत्यागम गिनती;
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
    पूर्णाङ्क
set_indent(
    पूर्णाङ्क		size,		    // measured in spaces
    पूर्णाङ्क		flags)
{
    char_u	*p;
    char_u	*newline;
    char_u	*oldline;
    char_u	*s;
    पूर्णाङ्क		todo;
    पूर्णाङ्क		ind_len;	    // measured in characters
    पूर्णाङ्क		पंक्ति_len;	    // size of the line (including the NUL)
    पूर्णाङ्क		doit = FALSE;
    पूर्णाङ्क		ind_done = 0;	    // measured in spaces
#ifdef FEAT_VARTABS
    पूर्णाङ्क		ind_col = 0;
#endif
    पूर्णाङ्क		tab_pad;
    पूर्णाङ्क		retval = FALSE;
    पूर्णाङ्क		orig_char_len = -1; // number of initial whitespace chars when
				    // 'et' and 'pi' are both set

    // First check if there is anything to do and compute the number of
    // characters needed for the indent.
    todo = size;
    ind_len = 0;
    p = oldline = ml_get_curline();
    पंक्ति_len = ml_get_curline_len() + 1;

    // Calculate the buffer size for the new indent, and check to see if it
    // isn't already set

    // if 'expandtab' isn't set: use TABs; if both 'expandtab' and
    // 'preserveindent' are set count the number of characters at the
    // beginning of the line to be copied
    यदि (!curbuf->b_p_et || (!(flags & SIN_जोड़ो) && curbuf->b_p_pi))
    {
	// If 'preserveindent' is set then reuse as much as possible of
	// the existing indent structure for the new indent
	यदि (!(flags & SIN_जोड़ो) && curbuf->b_p_pi)
	{
	    ind_done = 0;

	    // count as many characters as we can use
	    यावत् (todo > 0 && VIM_ISWHITE(*p))
	    {
		यदि (*p == TAB)
		{
#ifdef FEAT_VARTABS
		    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#अन्यथा
		    tab_pad = (पूर्णाङ्क)curbuf->b_p_ts
					   - (ind_done % (पूर्णाङ्क)curbuf->b_p_ts);
#endif
		    // stop if this tab will overshoot the target
		    यदि (todo < tab_pad)
			भङ्ग;
		    todo -= tab_pad;
		    ++ind_len;
		    ind_done += tab_pad;
		}
		अन्यथा
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
	    यदि (curbuf->b_p_et)
		orig_char_len = ind_len;

	    // Fill to next tabstop with a tab, if possible
#ifdef FEAT_VARTABS
	    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
						curbuf->b_p_vts_array);
#अन्यथा
	    tab_pad = (पूर्णाङ्क)curbuf->b_p_ts - (ind_done % (पूर्णाङ्क)curbuf->b_p_ts);
#endif
	    यदि (todo >= tab_pad && orig_char_len == -1)
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
	प्रति (;;)
	{
	    tab_pad = tabstop_padding(ind_col, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
	    यदि (todo < tab_pad)
		भङ्ग;
	    यदि (*p != TAB)
		doit = TRUE;
	    अन्यथा
		++p;
	    todo -= tab_pad;
	    ++ind_len;
	    ind_col += tab_pad;
	}
#अन्यथा
	यावत् (todo >= (पूर्णाङ्क)curbuf->b_p_ts)
	{
	    यदि (*p != TAB)
		doit = TRUE;
	    अन्यथा
		++p;
	    todo -= (पूर्णाङ्क)curbuf->b_p_ts;
	    ++ind_len;
	    // ind_done += (int)curbuf->b_p_ts;
	}
#endif
    }
    // count spaces required for indent
    यावत् (todo > 0)
    {
	यदि (*p != ' ')
	    doit = TRUE;
	अन्यथा
	    ++p;
	--todo;
	++ind_len;
	// ++ind_done;
    }

    // Return if the indent is OK already.
    यदि (!doit && !VIM_ISWHITE(*p) && !(flags & SIN_जोड़ो))
	प्रत्यागम FALSE;

    // Allocate memory for the new line.
    यदि (flags & SIN_जोड़ो)
	p = oldline;
    अन्यथा
    {
	p = skipwhite(p);
	पंक्ति_len -= (पूर्णाङ्क)(p - oldline);
    }

    // If 'preserveindent' and 'expandtab' are both set keep the original
    // characters and allocate accordingly.  We will fill the rest with spaces
    // after the if (!curbuf->b_p_et) below.
    यदि (orig_char_len != -1)
    {
	newline = alloc(orig_char_len + size - ind_done + पंक्ति_len);
	यदि (newline == रिक्त)
	    प्रत्यागम FALSE;
	todo = size - ind_done;
	ind_len = orig_char_len + todo;    // Set total length of indent in
					   // characters, which may have been
					   // undercounted until now
	p = oldline;
	s = newline;
	यावत् (orig_char_len > 0)
	{
	    *s++ = *p++;
	    orig_char_len--;
	}

	// Skip over any additional white space (useful when newindent is less
	// than old)
	यावत् (VIM_ISWHITE(*p))
	    ++p;

    }
    अन्यथा
    {
	todo = size;
	newline = alloc(ind_len + पंक्ति_len);
	यदि (newline == रिक्त)
	    प्रत्यागम FALSE;
	s = newline;
    }

    // Put the characters in the new line.
    // if 'expandtab' isn't set: use TABs
    यदि (!curbuf->b_p_et)
    {
	// If 'preserveindent' is set then reuse as much as possible of
	// the existing indent structure for the new indent
	यदि (!(flags & SIN_जोड़ो) && curbuf->b_p_pi)
	{
	    p = oldline;
	    ind_done = 0;

	    यावत् (todo > 0 && VIM_ISWHITE(*p))
	    {
		यदि (*p == TAB)
		{
#ifdef FEAT_VARTABS
		    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#अन्यथा
		    tab_pad = (पूर्णाङ्क)curbuf->b_p_ts
					   - (ind_done % (पूर्णाङ्क)curbuf->b_p_ts);
#endif
		    // stop if this tab will overshoot the target
		    यदि (todo < tab_pad)
			भङ्ग;
		    todo -= tab_pad;
		    ind_done += tab_pad;
		}
		अन्यथा
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
#अन्यथा
	    tab_pad = (पूर्णाङ्क)curbuf->b_p_ts - (ind_done % (पूर्णाङ्क)curbuf->b_p_ts);
#endif
	    यदि (todo >= tab_pad)
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
	प्रति (;;)
	{
	    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
	    यदि (todo < tab_pad)
		भङ्ग;
	    *s++ = TAB;
	    todo -= tab_pad;
	    ind_done += tab_pad;
	}
#अन्यथा
	यावत् (todo >= (पूर्णाङ्क)curbuf->b_p_ts)
	{
	    *s++ = TAB;
	    todo -= (पूर्णाङ्क)curbuf->b_p_ts;
	}
#endif
    }
    यावत् (todo > 0)
    {
	*s++ = ' ';
	--todo;
    }
    mch_memmove(s, p, (माप_प्रकार)पंक्ति_len);

    // Replace the line (unless undo fails).
    यदि (!(flags & SIN_UNDO) || u_savesub(curwin->w_cursor.lnum) == OK)
    {
	colnr_T old_offset = (colnr_T)(p - oldline);
	colnr_T new_offset = (colnr_T)(s - newline);

	// this may free "newline"
	ml_replace(curwin->w_cursor.lnum, newline, FALSE);
	यदि (flags & SIN_CHANGED)
	    changed_bytes(curwin->w_cursor.lnum, 0);

	// Correct saved cursor position if it is in this line.
	यदि (saved_cursor.lnum == curwin->w_cursor.lnum)
	{
	    यदि (saved_cursor.col >= old_offset)
		// cursor was after the indent, adjust for the number of
		// bytes added/removed
		saved_cursor.col += ind_len - old_offset;
	    अन्यथा यदि (saved_cursor.col >= new_offset)
		// cursor was in the indent, and is now after it, put it back
		// at the start of the indent (replacing spaces with TAB)
		saved_cursor.col = new_offset;
	}
#ifdef FEAT_PROP_POPUP
	यदि (curbuf->b_has_textprop)
	{
	    पूर्णाङ्क added = ind_len - old_offset;

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
    अन्यथा
	vim_free(newline);

    curwin->w_cursor.col = ind_len;
    प्रत्यागम retval;
}

/*
 * Return the indent of the current line after a number.  Return -1 if no
 * number was found.  Used for 'n' in 'formatoptions': numbered list.
 * Since a pattern is used it can actually handle more than numbers.
 */
    पूर्णाङ्क
get_संख्या_indent(linenr_T lnum)
{
    colnr_T	col;
    pos_T	pos;

    regmatch_T	regmatch;
    पूर्णाङ्क		lead_len = 0;	// length of comment leader

    यदि (lnum > curbuf->b_ml.ml_पंक्ति_गिनती)
	प्रत्यागम -1;
    pos.lnum = 0;

    // In format_lines() (i.e. not insert mode), fo+=q is needed too...
    यदि ((State & MODE_जोड़ो) || has_format_option(FO_Q_COMS))
	lead_len = get_leader_len(ml_get(lnum), रिक्त, FALSE, TRUE);

    regmatch.regprog = vim_regcomp(curbuf->b_p_flp, RE_MAGIC);
    यदि (regmatch.regprog != रिक्त)
    {
	regmatch.rm_ic = FALSE;

	// vim_regexec() expects a pointer to a line.  This lets us
	// start matching for the flp beyond any comment leader...
	यदि (vim_regexec(&regmatch, ml_get(lnum) + lead_len, (colnr_T)0))
	{
	    pos.lnum = lnum;
	    pos.col = (colnr_T)(*regmatch.endp - ml_get(lnum));
	    pos.coladd = 0;
	}
	vim_regfree(regmatch.regprog);
    }

    यदि (pos.lnum == 0 || *ml_get_pos(&pos) == NUL)
	प्रत्यागम -1;
    getvcol(curwin, &pos, &col, रिक्त, रिक्त, 0);
    प्रत्यागम (पूर्णाङ्क)col;
}

#यदि defined(FEAT_LINEBREAK)
/*
 * Check "briopt" as 'breakindentopt' and update the members of "wp".
 * This is called when 'breakindentopt' is changed and when a window is
 * initialized.
 * Returns FAIL for failure, OK otherwise.
 */
    पूर्णाङ्क
briopt_check(
    char_u	*briopt,	// when NULL: use "wp->w_p_briopt"
    win_T	*wp)		// when NULL: only check "briopt"
{
    char_u	*p;
    पूर्णाङ्क		bri_shift = 0;
    दीर्घ	bri_min = 20;
    पूर्णाङ्क		bri_sbr = FALSE;
    पूर्णाङ्क		bri_list = 0;
    पूर्णाङ्क		bri_vcol = 0;

    यदि (briopt != रिक्त)
	p = briopt;
    अन्यथा
	p = wp->w_p_briopt;

    यावत् (*p != NUL)
    {
	// Note: Keep this in sync with p_briopt_values
	यदि (STRNCMP(p, "shift:", 6) == 0
		 && ((p[6] == '-' && VIM_ISDIGIT(p[7])) || VIM_ISDIGIT(p[6])))
	{
	    p += 6;
	    bri_shift = getdigits(&p);
	}
	अन्यथा यदि (STRNCMP(p, "min:", 4) == 0 && VIM_ISDIGIT(p[4]))
	{
	    p += 4;
	    bri_min = getdigits(&p);
	}
	अन्यथा यदि (STRNCMP(p, "sbr", 3) == 0)
	{
	    p += 3;
	    bri_sbr = TRUE;
	}
	अन्यथा यदि (STRNCMP(p, "list:", 5) == 0)
	{
	    p += 5;
	    bri_list = getdigits(&p);
	}
	अन्यथा यदि (STRNCMP(p, "column:", 7) == 0)
	{
	    p += 7;
	    bri_vcol = getdigits(&p);
	}
	यदि (*p != ',' && *p != NUL)
	    प्रत्यागम FAIL;
	यदि (*p == ',')
	    ++p;
    }

    यदि (wp == रिक्त)
	प्रत्यागम OK;

    wp->w_briopt_shift = bri_shift;
    wp->w_briopt_min   = bri_min;
    wp->w_briopt_sbr   = bri_sbr;
    wp->w_briopt_list  = bri_list;
    wp->w_briopt_vcol  = bri_vcol;

    प्रत्यागम OK;
}

/*
 * Return appropriate space number for breakindent, taking influencing
 * parameters into account. Window must be specified, since it is not
 * necessarily always the current one.
 */
    पूर्णाङ्क
get_breakindent_win(
    win_T	*wp,
    char_u	*पंक्ति) // start of the line
{
    स्थिर पूर्णाङ्क	    prev_indent = 0;	// cached indent value
    स्थिर दीर्घ	    prev_ts     = 0L;	// cached tabstop value
# ifdef FEAT_VARTABS
    स्थिर पूर्णाङ्क      *prev_vts = रिक्त;   // cached vartabs values
# endif
    स्थिर पूर्णाङ्क	    prev_fnum   = 0;	// cached buffer number
    स्थिर char_u   *prev_पंक्ति  = रिक्त;	// cached copy of "line"
    स्थिर varnumber_T prev_tick = 0;   // changedtick of cached value
    स्थिर पूर्णाङ्क      prev_list = 0;	// cached list indent
    स्थिर पूर्णाङ्क      prev_listopt = 0;	// cached w_p_briopt_list value
    स्थिर पूर्णाङ्क      prev_no_ts = FALSE;	// cached no_ts value
    स्थिर अचिन्हित prev_dy_uhex = 0;	// cached 'display' "uhex" value
    // cached formatlistpat value
    स्थिर char_u   *prev_flp = रिक्त;
    पूर्णाङ्क		    bri = 0;
    // window width minus window margin space, i.e. what rests for text
    ध्रुव पूर्णाङ्क	    eff_wwidth = wp->w_width
					  - win_col_off(wp) + win_col_off2(wp);

    // In list mode, if 'listchars' "tab" isn't set, a TAB is displayed as ^I.
    पूर्णाङ्क no_ts = wp->w_p_list && wp->w_lcs_chars.tab1 == NUL;

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
    यदि (prev_fnum != wp->w_buffer->b_fnum
	    || prev_ts != wp->w_buffer->b_p_ts
# ifdef FEAT_VARTABS
	    || prev_vts != wp->w_buffer->b_p_vts_array
# endif
	    || prev_tick != CHANGEDTICK(wp->w_buffer)
	    || prev_listopt != wp->w_briopt_list
	    || prev_no_ts != no_ts
	    || prev_dy_uhex != (dy_flags & DY_UHEX)
	    || prev_flp == रिक्त
	    || STRCMP(prev_flp, get_flp_मूल्य(wp->w_buffer)) != 0
	    || prev_पंक्ति == रिक्त || STRCMP(prev_पंक्ति, पंक्ति) != 0
	)
    {
	prev_fnum = wp->w_buffer->b_fnum;
	vim_free(prev_पंक्ति);
	prev_पंक्ति = vim_strsave(पंक्ति);
	prev_ts = wp->w_buffer->b_p_ts;
# ifdef FEAT_VARTABS
	prev_vts = wp->w_buffer->b_p_vts_array;
	यदि (wp->w_briopt_vcol == 0)
	    prev_indent = get_indent_str_vtab(पंक्ति,
				     (पूर्णाङ्क)wp->w_buffer->b_p_ts,
				      wp->w_buffer->b_p_vts_array, no_ts);
# अन्यथा
	यदि (wp->w_briopt_vcol == 0)
	    prev_indent = get_indent_str(पंक्ति,
					(पूर्णाङ्क)wp->w_buffer->b_p_ts, no_ts);
# endif
	prev_tick = CHANGEDTICK(wp->w_buffer);
	prev_listopt = wp->w_briopt_list;
	prev_list = 0;
	prev_no_ts = no_ts;
	prev_dy_uhex = (dy_flags & DY_UHEX);
	vim_free(prev_flp);
	prev_flp = vim_strsave(get_flp_मूल्य(wp->w_buffer));
	// add additional indent for numbered lists
	यदि (wp->w_briopt_list != 0 && wp->w_briopt_vcol == 0)
	{
	    regmatch_T	    regmatch;

	    regmatch.regprog = vim_regcomp(prev_flp,
				   RE_MAGIC + RE_STRING + RE_AUTO + RE_STRICT);

	    यदि (regmatch.regprog != रिक्त)
	    {
		regmatch.rm_ic = FALSE;
		यदि (vim_regexec(&regmatch, पंक्ति, 0))
		{
		    यदि (wp->w_briopt_list > 0)
			prev_list = wp->w_briopt_list;
		    अन्यथा
		    {
			char_u	*ptr = *regmatch.startp;
			char_u	*end_ptr = *regmatch.endp;
			पूर्णाङ्क	indent = 0;

			// Compute the width of the matched text.
			// Use win_chartabsize() so that TAB size is correct,
			// while wrapping is ignored.
			यावत् (ptr < end_ptr)
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
    यदि (wp->w_briopt_vcol != 0)
    {
	// column value has priority
	bri = wp->w_briopt_vcol;
	prev_list = 0;
    }
    अन्यथा
	bri = prev_indent + wp->w_briopt_shift;

    // Add offset for number column, if 'n' is in 'cpoptions'
    bri += win_col_off2(wp);

    // add additional indent for numbered lists
    यदि (wp->w_briopt_list > 0)
	bri += prev_list;

    // indent minus the length of the showbreak string
    यदि (wp->w_briopt_sbr)
	bri -= vim_strsize(get_showbreak_मूल्य(wp));


    // never indent past left window margin
    यदि (bri < 0)
	bri = 0;

    // always leave at least bri_min characters on the left,
    // if text width is sufficient
    अन्यथा यदि (bri > eff_wwidth - wp->w_briopt_min)
	bri = (eff_wwidth - wp->w_briopt_min < 0)
					   ? 0 : eff_wwidth - wp->w_briopt_min;

    प्रत्यागम bri;
}
#endif

/*
 * When extra == 0: Return TRUE if the cursor is before or on the first
 *		    non-blank in the line.
 * When extra == 1: Return TRUE if the cursor is before the first non-blank in
 *		    the line.
 */
    पूर्णाङ्क
inindent(पूर्णाङ्क extra)
{
    char_u	*ptr;
    colnr_T	col;

    प्रति (col = 0, ptr = ml_get_curline(); VIM_ISWHITE(*ptr); ++col)
	++ptr;
    यदि (col >= curwin->w_cursor.col + extra)
	प्रत्यागम TRUE;
    अन्यथा
	प्रत्यागम FALSE;
}

/*
 * op_reindent - handle reindenting a block of lines.
 */
    शून्य
op_reindent(oparg_T *oap, पूर्णाङ्क (*how)(शून्य))
{
    दीर्घ	i = 0;
    char_u	*l;
    पूर्णाङ्क		amount;
    linenr_T	first_changed = 0;
    linenr_T	last_changed = 0;
    linenr_T	start_lnum = curwin->w_cursor.lnum;

    // Don't even try when 'modifiable' is off.
    यदि (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	प्रत्यागम;
    }

    // Save for undo.  Do this once for all lines, much faster than doing this
    // for each line separately, especially when undoing.
    यदि (u_savecommon(start_lnum - 1, start_lnum + oap->पंक्ति_गिनती,
				     start_lnum + oap->पंक्ति_गिनती, FALSE) == OK)
	प्रति (i = oap->पंक्ति_गिनती; --i >= 0 && !got_int; )
	{
	    // it's a slow thing to do, so give feedback so there's no worry
	    // that the computer's just hung.

	    यदि (i > 1
		    && (i % 50 == 0 || i == oap->पंक्ति_गिनती - 1)
		    && oap->पंक्ति_गिनती > p_report)
		smsg(_("%ld lines to indent... "), i);

	    // Be vi-compatible: For lisp indenting the first line is not
	    // indented, unless there is only one line.
	    यदि (i != oap->पंक्ति_गिनती - 1 || oap->पंक्ति_गिनती == 1
						     || how != get_lisp_indent)
	    {
		l = skipwhite(ml_get_curline());
		यदि (*l == NUL)		    // empty or blank line
		    amount = 0;
		अन्यथा
		    amount = how();	    // get the indent for this line

		यदि (amount >= 0 && set_indent(amount, 0))
		{
		    // did change the indent, call changed_lines() later
		    यदि (first_changed == 0)
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
    यदि (last_changed != 0)
	changed_पंक्तियाँ(first_changed, 0,
		oap->is_VIsual ? start_lnum + oap->पंक्ति_गिनती :
		last_changed + 1, 0L);
    अन्यथा यदि (oap->is_VIsual)
	redraw_curbuf_later(UPD_INVERTED);

    यदि (oap->पंक्ति_गिनती > p_report)
    {
	i = oap->पंक्ति_गिनती - (i + 1);
	smsg(NGETTEXT("%ld line indented ",
						 "%ld lines indented ", i), i);
    }
    यदि ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// set '[ and '] marks
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
    }
}

/*
 * Return TRUE if lines starting with '#' should be left aligned.
 */
    पूर्णाङ्क
preprocs_बायाँ(शून्य)
{
    प्रत्यागम
	(curbuf->b_p_si && !curbuf->b_p_cin) ||
	(curbuf->b_p_cin && in_cinkeys('#', ' ', TRUE)
					   && curbuf->b_ind_hash_comment == 0)
	;
}

/*
 * Return TRUE if the conditions are OK for smart indenting.
 */
    पूर्णाङ्क
may_do_si(शून्य)
{
    प्रत्यागम curbuf->b_p_si
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
    शून्य
ins_try_si(पूर्णाङ्क c)
{
    pos_T	*pos, old_pos;
    char_u	*ptr;
    पूर्णाङ्क		i;
    पूर्णाङ्क		temp;

    // do some very smart indenting when entering '{' or '}'
    यदि (((did_si || can_si_back) && c == '{')
	    || (can_si && c == '}' && inindent(0)))
    {
	// for '}' set indent equal to indent of line containing matching '{'
	यदि (c == '}' && (pos = findmatch(रिक्त, '{')) != रिक्त)
	{
	    old_pos = curwin->w_cursor;
	    // If the matching '{' has a ')' immediately before it (ignoring
	    // white-space), then line up with the start of the line
	    // containing the matching '(' if there is one.  This handles the
	    // case where an "if (..\n..) {" statement continues over multiple
	    // lines -- webb
	    ptr = ml_get(pos->lnum);
	    i = pos->col;
	    यदि (i > 0)		// skip blanks before '{'
		यावत् (--i > 0 && VIM_ISWHITE(ptr[i]))
		    ;
	    curwin->w_cursor.lnum = pos->lnum;
	    curwin->w_cursor.col = i;
	    यदि (ptr[i] == ')' && (pos = findmatch(रिक्त, '(')) != रिक्त)
		curwin->w_cursor = *pos;
	    i = get_indent();
	    curwin->w_cursor = old_pos;
	    यदि (State & VREPLACE_FLAG)
		change_indent(INDENT_SET, i, FALSE, NUL, TRUE);
	    अन्यथा
		(शून्य)set_indent(i, SIN_CHANGED);
	}
	अन्यथा यदि (curwin->w_cursor.col > 0)
	{
	    // when inserting '{' after "O" reduce indent, but not
	    // more than indent of previous line
	    temp = TRUE;
	    यदि (c == '{' && can_si_back && curwin->w_cursor.lnum > 1)
	    {
		old_pos = curwin->w_cursor;
		i = get_indent();
		यावत् (curwin->w_cursor.lnum > 1)
		{
		    ptr = skipwhite(ml_get(--(curwin->w_cursor.lnum)));

		    // ignore empty lines and lines starting with '#'.
		    यदि (*ptr != '#' && *ptr != NUL)
			भङ्ग;
		}
		यदि (get_indent() >= i)
		    temp = FALSE;
		curwin->w_cursor = old_pos;
	    }
	    यदि (temp)
		shift_पंक्ति(TRUE, FALSE, 1, TRUE);
	}
    }

    // set indent of '#' always to 0
    यदि (curwin->w_cursor.col > 0 && can_si && c == '#' && inindent(0))
    {
	// remember current indent for next line
	old_indent = get_indent();
	(शून्य)set_indent(0, SIN_CHANGED);
    }

    // Adjust ai_col, the char at this position can be deleted.
    यदि (ai_col > curwin->w_cursor.col)
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
    शून्य
change_indent(
    पूर्णाङ्क		type,
    पूर्णाङ्क		amount,
    पूर्णाङ्क		round,
    पूर्णाङ्क		replaced,	// replaced character, put on replace stack
    पूर्णाङ्क		call_changed_bytes)	// call changed_bytes()
{
    पूर्णाङ्क		vcol;
    पूर्णाङ्क		last_vcol;
    पूर्णाङ्क		insstart_less;		// reduction for Insstart.col
    पूर्णाङ्क		new_cursor_col;
    पूर्णाङ्क		i;
    char_u	*ptr;
    पूर्णाङ्क		save_p_list;
    पूर्णाङ्क		start_col;
    colnr_T	vc;
    colnr_T	orig_col = 0;		// init for GCC
    char_u	*new_पंक्ति, *orig_पंक्ति = रिक्त;	// init for GCC

    // MODE_VREPLACE state needs to know what the line was like before changing
    यदि (State & VREPLACE_FLAG)
    {
	orig_पंक्ति = vim_strnsave(ml_get_curline(), ml_get_curline_len());  // Deal with NULL below
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
    यदि (new_cursor_col < 0)
	vcol = get_indent() - vcol;

    यदि (new_cursor_col > 0)	    // can't fix replace stack
	start_col = -1;

    // Set the new indent.  The cursor will be put on the first non-blank.
    यदि (type == INDENT_SET)
	(शून्य)set_indent(amount, call_changed_bytes ? SIN_CHANGED : 0);
    अन्यथा
    {
	पूर्णाङ्क	save_State = State;

	// Avoid being called recursively.
	यदि (State & VREPLACE_FLAG)
	    State = MODE_जोड़ो;
	shift_पंक्ति(type == INDENT_DEC, round, 1, call_changed_bytes);
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
    यदि (new_cursor_col >= 0)
    {
	// When changing the indent while the cursor is touching it, reset
	// Insstart_col to 0.
	यदि (new_cursor_col == 0)
	    insstart_less = MAXCOL;
	new_cursor_col += curwin->w_cursor.col;
    }
    अन्यथा यदि (!(State & MODE_जोड़ो))
	new_cursor_col = curwin->w_cursor.col;
    अन्यथा
    {
	chartabsize_T cts;

	// Compute the screen column where the cursor should be.
	vcol = get_indent() - vcol;
	curwin->w_virtcol = (colnr_T)((vcol < 0) ? 0 : vcol);

	// Advance the cursor until we reach the right screen column.
	last_vcol = 0;
	ptr = ml_get_curline();
	init_chartabsize_arg(&cts, curwin, 0, 0, ptr, ptr);
	यावत् (cts.cts_vcol <= (पूर्णाङ्क)curwin->w_virtcol)
	{
	    last_vcol = cts.cts_vcol;
	    यदि (cts.cts_vcol > 0)
		MB_PTR_ADV(cts.cts_ptr);
	    यदि (*cts.cts_ptr == NUL)
		भङ्ग;
	    cts.cts_vcol += lbr_chartabsize(&cts);
	}
	vcol = last_vcol;
	new_cursor_col = cts.cts_ptr - cts.cts_पंक्ति;
	clear_chartabsize_arg(&cts);

	// May need to insert spaces to be able to position the cursor on
	// the right screen column.
	यदि (vcol != (पूर्णाङ्क)curwin->w_virtcol)
	{
	    curwin->w_cursor.col = (colnr_T)new_cursor_col;
	    i = (पूर्णाङ्क)curwin->w_virtcol - vcol;
	    ptr = alloc(i + 1);
	    यदि (ptr != रिक्त)
	    {
		माप_प्रकार ptrlen;
		new_cursor_col += i;
		ptr[i] = NUL;
		ptrlen = i;
		यावत् (--i >= 0)
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

    यदि (new_cursor_col <= 0)
	curwin->w_cursor.col = 0;
    अन्यथा
	curwin->w_cursor.col = (colnr_T)new_cursor_col;
    curwin->w_set_curswant = true;
    changed_cline_bef_curs();

    // May have to adjust the start of the insert.
    यदि (State & MODE_जोड़ो)
    {
	यदि (curwin->w_cursor.lnum == Insstart.lnum && Insstart.col != 0)
	{
	    यदि ((पूर्णाङ्क)Insstart.col <= insstart_less)
		Insstart.col = 0;
	    अन्यथा
		Insstart.col -= insstart_less;
	}
	यदि ((पूर्णाङ्क)ai_col <= insstart_less)
	    ai_col = 0;
	अन्यथा
	    ai_col -= insstart_less;
    }

    // For MODE_REPLACE state, may have to fix the replace stack, if it's
    // possible.  If the number of characters before the cursor decreased, need
    // to pop a few characters from the replace stack.
    // If the number of characters before the cursor increased, need to push a
    // few NULs onto the replace stack.
    यदि (REPLACE_NORMAL(State) && start_col >= 0)
    {
	यावत् (start_col > (पूर्णाङ्क)curwin->w_cursor.col)
	{
	    replace_join(0);	    // remove a NUL from the replace stack
	    --start_col;
	}
	यावत् (start_col < (पूर्णाङ्क)curwin->w_cursor.col || replaced)
	{
	    replace_धकेलो(NUL);
	    यदि (replaced)
	    {
		replace_धकेलो(replaced);
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
    यदि (State & VREPLACE_FLAG)
    {
	// If orig_line didn't allocate, just return.  At least we did the job,
	// even if you can't backspace.
	यदि (orig_पंक्ति == रिक्त)
	    प्रत्यागम;

	// Save new line
	new_पंक्ति = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	यदि (new_पंक्ति == रिक्त)
	{
	    vim_free(orig_पंक्ति);
	    प्रत्यागम;
	}

	// We only put back the new line up to the cursor
	new_पंक्ति[curwin->w_cursor.col] = NUL;

	// Put back original line
	ml_replace(curwin->w_cursor.lnum, orig_पंक्ति, FALSE);
	curwin->w_cursor.col = orig_col;

	// Backspace from cursor to start of line
	backspace_until_column(0);

	// Insert new stuff into line again
	ins_bytes(new_पंक्ति);

	vim_free(new_पंक्ति);
    }
}

/*
 * Copy the indent from ptr to the current line (and fill to size)
 * Leaves the cursor on the first non-blank in the line.
 * Returns TRUE if the line was changed.
 */
    पूर्णाङ्क
copy_indent(पूर्णाङ्क size, char_u *src)
{
    char_u	*p = रिक्त;
    char_u	*पंक्ति = रिक्त;
    char_u	*s;
    पूर्णाङ्क		todo;
    पूर्णाङ्क		ind_len;
    पूर्णाङ्क		पंक्ति_len = 0;
    पूर्णाङ्क		tab_pad;
    पूर्णाङ्क		ind_done;
    पूर्णाङ्क		round;
#ifdef FEAT_VARTABS
    पूर्णाङ्क		ind_col;
#endif

    // Round 1: compute the number of characters needed for the indent
    // Round 2: copy the characters.
    प्रति (round = 1; round <= 2; ++round)
    {
	todo = size;
	ind_len = 0;
	ind_done = 0;
#ifdef FEAT_VARTABS
	ind_col = 0;
#endif
	s = src;

	// Count/copy the usable portion of the source line
	यावत् (todo > 0 && VIM_ISWHITE(*s))
	{
	    यदि (*s == TAB)
	    {
#ifdef FEAT_VARTABS
		tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#अन्यथा
		tab_pad = (पूर्णाङ्क)curbuf->b_p_ts
					   - (ind_done % (पूर्णाङ्क)curbuf->b_p_ts);
#endif
		// Stop if this tab will overshoot the target
		यदि (todo < tab_pad)
		    भङ्ग;
		todo -= tab_pad;
		ind_done += tab_pad;
#ifdef FEAT_VARTABS
		ind_col += tab_pad;
#endif
	    }
	    अन्यथा
	    {
		--todo;
		++ind_done;
#ifdef FEAT_VARTABS
		++ind_col;
#endif
	    }
	    ++ind_len;
	    यदि (p != रिक्त)
		*p++ = *s;
	    ++s;
	}

	// Fill to next tabstop with a tab, if possible
#ifdef FEAT_VARTABS
	tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#अन्यथा
	tab_pad = (पूर्णाङ्क)curbuf->b_p_ts - (ind_done % (पूर्णाङ्क)curbuf->b_p_ts);
#endif
	यदि (todo >= tab_pad && !curbuf->b_p_et)
	{
	    todo -= tab_pad;
	    ++ind_len;
#ifdef FEAT_VARTABS
	    ind_col += tab_pad;
#endif
	    यदि (p != रिक्त)
		*p++ = TAB;
	}

	// Add tabs required for indent
	यदि (!curbuf->b_p_et)
	{
#ifdef FEAT_VARTABS
	    प्रति (;;)
	    {
		tab_pad = tabstop_padding(ind_col, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
		यदि (todo < tab_pad)
		    भङ्ग;
		todo -= tab_pad;
		++ind_len;
		ind_col += tab_pad;
		यदि (p != रिक्त)
		    *p++ = TAB;
	    }
#अन्यथा
	    यावत् (todo >= (पूर्णाङ्क)curbuf->b_p_ts)
	    {
		todo -= (पूर्णाङ्क)curbuf->b_p_ts;
		++ind_len;
		यदि (p != रिक्त)
		    *p++ = TAB;
	    }
#endif
	}

	// Count/add spaces required for indent
	यावत् (todo > 0)
	{
	    --todo;
	    ++ind_len;
	    यदि (p != रिक्त)
		*p++ = ' ';
	}

	यदि (p == रिक्त)
	{
	    // Allocate memory for the result: the copied indent, new indent
	    // and the rest of the line.
	    पंक्ति_len = ml_get_curline_len() + 1;
	    पंक्ति = alloc(ind_len + पंक्ति_len);
	    यदि (पंक्ति == रिक्त)
		प्रत्यागम FALSE;
	    p = पंक्ति;
	}
    }

    // Append the original line
    mch_memmove(p, ml_get_curline(), (माप_प्रकार)पंक्ति_len);

    // Replace the line
    ml_replace(curwin->w_cursor.lnum, पंक्ति, FALSE);

    // Put the cursor after the indent.
    curwin->w_cursor.col = ind_len;
    प्रत्यागम TRUE;
}

/*
 * Give a "resulting text too long" error and maybe set got_int.
 */
    स्थिर शून्य
emsg_text_too_long(शून्य)
{
    emsg(_(e_resulting_text_too_long));
#ifdef FEAT_EVAL
    // when not inside a try/catch set got_int to break out of any loop
    यदि (trylevel == 0)
#endif
	got_int = TRUE;
}

/*
 * ":retab".
 */
    शून्य
ex_retab(exarg_T *eap)
{
    linenr_T	lnum;
    पूर्णाङ्क		got_tab = FALSE;
    दीर्घ	अंक_spaces = 0;
    दीर्घ	अंक_tabs;
    दीर्घ	len;
    दीर्घ	col;
    दीर्घ	vcol;
    दीर्घ	start_col = 0;		// For start of white-space string
    दीर्घ	start_vcol = 0;		// For start of white-space string
    दीर्घ	old_len;
    दीर्घ	new_len;
    char_u	*ptr;
    char_u	*new_पंक्ति = (char_u *)1; // init to non-NULL
    पूर्णाङ्क		did_undo;		// called u_save for current line
#ifdef FEAT_VARTABS
    पूर्णाङ्क		*new_vts_array = रिक्त;
    char_u	*new_ts_str;		// string value of tab argument
#अन्यथा
    पूर्णाङ्क		temp;
    पूर्णाङ्क		new_ts = 0;
#endif
    पूर्णाङ्क		save_list;
    linenr_T	first_पंक्ति = 0;		// first changed line
    linenr_T	last_पंक्ति = 0;		// last changed line
    पूर्णाङ्क		is_indent_only = 0;	// Only process leading whitespace

    save_list = curwin->w_p_list;
    curwin->w_p_list = 0;	    // don't want list mode here

    ptr = eap->arg;
    यदि (STRNCMP(ptr, "-indentonly", 11) == 0 && IS_WHITE_OR_NUL(ptr[11]))
    {
	is_indent_only = 1;
	ptr = skipwhite(ptr + 11);
    }

#ifdef FEAT_VARTABS
    new_ts_str = ptr;
    यदि (tabstop_set(ptr, &new_vts_array) == FAIL)
	प्रत्यागम;
    यावत् (vim_isdigit(*ptr) || *ptr == ',')
	++ptr;

    // This ensures that either new_vts_array and new_ts_str are freshly
    // allocated, or new_vts_array points to an existing array and new_ts_str
    // is null.
    यदि (new_vts_array == रिक्त)
    {
	new_vts_array = curbuf->b_p_vts_array;
	new_ts_str = रिक्त;
    }
    अन्यथा
	new_ts_str = vim_strnsave(new_ts_str, ptr - new_ts_str);
#अन्यथा
    यदि (ptr[0] != NUL && (ptr[0] != '0' || ptr[1] != NUL))
    {
	char_u	*end;

	यदि (म_से_दीर्घ((वर्ण *)ptr, (वर्ण **)&end, 10) <= 0)
	{
	    यदि (ptr != end)
		emsg(_(e_argument_must_be_positive));
	    अन्यथा
		semsg(_(e_invalid_argument_str), ptr);
	    प्रत्यागम;
	}
	new_ts = getdigits(&ptr);
	यदि (new_ts < 0 || new_ts > TABSTOP_MAX)
	{
	    semsg(_(e_invalid_argument_str), eap->arg);
	    प्रत्यागम;
	}
    }
    यदि (new_ts == 0)
	new_ts = curbuf->b_p_ts;
#endif
    प्रति (lnum = eap->line1; !got_int && lnum <= eap->line2; ++lnum)
    {
	ptr = ml_get(lnum);
	old_len = ml_get_len(lnum);
	col = 0;
	vcol = 0;
	did_undo = FALSE;
	प्रति (;;)
	{
	    यदि (VIM_ISWHITE(ptr[col]))
	    {
		यदि (!got_tab && अंक_spaces == 0)
		{
		    // First consecutive white-space
		    start_vcol = vcol;
		    start_col = col;
		}
		यदि (ptr[col] == ' ')
		    अंक_spaces++;
		अन्यथा
		    got_tab = TRUE;
	    }
	    अन्यथा
	    {
		यदि (got_tab || (eap->forceit && अंक_spaces > 1))
		{
		    // Retabulate this string of white-space

		    // len is virtual length of white string
		    len = अंक_spaces = vcol - start_vcol;
		    अंक_tabs = 0;
		    यदि (!curbuf->b_p_et)
		    {
#ifdef FEAT_VARTABS
			पूर्णाङ्क t, s;

			tabstop_fromto(start_vcol, vcol,
					curbuf->b_p_ts, new_vts_array, &t, &s);
			अंक_tabs = t;
			अंक_spaces = s;
#अन्यथा
			temp = new_ts - (start_vcol % new_ts);
			यदि (अंक_spaces >= temp)
			{
			    अंक_spaces -= temp;
			    अंक_tabs++;
			}
			अंक_tabs += अंक_spaces / new_ts;
			अंक_spaces -= (अंक_spaces / new_ts) * new_ts;
#endif
		    }
		    यदि (curbuf->b_p_et || got_tab ||
					(अंक_spaces + अंक_tabs < len))
		    {
			यदि (did_undo == FALSE)
			{
			    did_undo = TRUE;
			    यदि (u_save((linenr_T)(lnum - 1),
						(linenr_T)(lnum + 1)) == FAIL)
			    {
				new_पंक्ति = रिक्त;	// flag out-of-memory
				भङ्ग;
			    }
			}

			// len is actual number of white characters used
			len = अंक_spaces + अंक_tabs;
			new_len = old_len - col + start_col + len + 1;
			यदि (new_len <= 0 || new_len >= MAXCOL)
			{
			    emsg_text_too_long();
			    भङ्ग;
			}
			new_पंक्ति = alloc(new_len);
			यदि (new_पंक्ति == रिक्त)
			    भङ्ग;
			यदि (start_col > 0)
			    mch_memmove(new_पंक्ति, ptr, (माप_प्रकार)start_col);
			mch_memmove(new_पंक्ति + start_col + len,
				      ptr + col, (माप_प्रकार)(old_len - col + 1));
			ptr = new_पंक्ति + start_col;
			प्रति (col = 0; col < len; col++)
			    ptr[col] = (col < अंक_tabs) ? '\t' : ' ';
			यदि (ml_replace(lnum, new_पंक्ति, FALSE) == OK)
			    // "new_line" may have been copied
			    new_पंक्ति = curbuf->b_ml.ml_पंक्ति_ptr;
			यदि (first_पंक्ति == 0)
			    first_पंक्ति = lnum;
			last_पंक्ति = lnum;
			ptr = new_पंक्ति;
			old_len = new_len - 1;
			col = start_col + len;
		    }
		}
		got_tab = FALSE;
		अंक_spaces = 0;

		यदि (is_indent_only)
		    भङ्ग;
	    }
	    यदि (ptr[col] == NUL)
		भङ्ग;
	    vcol += chartabsize(ptr + col, (colnr_T)vcol);
	    यदि (vcol >= MAXCOL)
	    {
		emsg_text_too_long();
		भङ्ग;
	    }
	    यदि (has_mbyte)
		col += (*mb_ptr2len)(ptr + col);
	    अन्यथा
		++col;
	}
	यदि (new_पंक्ति == रिक्त)		    // out of memory
	    भङ्ग;
	पंक्ति_breakcheck();
    }
    यदि (got_int)
	emsg(_(e_interrupted));

#ifdef FEAT_VARTABS
    // If a single value was given then it can be considered equal to
    // either the value of 'tabstop' or the value of 'vartabstop'.
    यदि (tabstop_गिनती(curbuf->b_p_vts_array) == 0
	&& tabstop_गिनती(new_vts_array) == 1
	&& curbuf->b_p_ts == tabstop_first(new_vts_array))
	; // not changed
    अन्यथा यदि (tabstop_गिनती(curbuf->b_p_vts_array) > 0
	&& tabstop_eq(curbuf->b_p_vts_array, new_vts_array))
	; // not changed
    अन्यथा
	redraw_curbuf_later(UPD_NOT_VALID);
#अन्यथा
    यदि (curbuf->b_p_ts != new_ts)
	redraw_curbuf_later(UPD_NOT_VALID);
#endif
    यदि (first_पंक्ति != 0)
	changed_पंक्तियाँ(first_पंक्ति, 0, last_पंक्ति + 1, 0L);

    curwin->w_p_list = save_list;	// restore 'list'

#ifdef FEAT_VARTABS
    यदि (new_ts_str != रिक्त)		// set the new tabstop
    {
	// If 'vartabstop' is in use or if the value given to retab has more
	// than one tabstop then update 'vartabstop'.
	पूर्णाङ्क *old_vts_ary = curbuf->b_p_vts_array;

	यदि (tabstop_गिनती(old_vts_ary) > 0 || tabstop_गिनती(new_vts_array) > 1)
	{
	    set_string_option_direct((char_u *)"vts", -1, new_ts_str,
							OPT_FREE|OPT_LOCAL, 0);
	    curbuf->b_p_vts_array = new_vts_array;
	    vim_free(old_vts_ary);
	}
	अन्यथा
	{
	    // 'vartabstop' wasn't in use and a single value was given to
	    // retab then update 'tabstop'.
	    curbuf->b_p_ts = tabstop_first(new_vts_array);
	    vim_free(new_vts_array);
	}
	vim_free(new_ts_str);
    }
#अन्यथा
    curbuf->b_p_ts = new_ts;
#endif
    coladvance(curwin->w_curswant);

    u_clearline();
}

#यदि defined(FEAT_EVAL)
/*
 * Get indent level from 'indentexpr'.
 */
    पूर्णाङ्क
get_व्यंजक_indent(शून्य)
{
    पूर्णाङ्क		indent = -1;
    char_u	*inde_copy;
    pos_T	save_pos;
    colnr_T	save_curswant;
    पूर्णाङ्क		save_set_curswant;
    पूर्णाङ्क		save_State;
    पूर्णाङ्क		use_sandbox = was_set_insecurely(curwin,
					    (char_u *)"indentexpr", OPT_LOCAL);
    sctx_T	save_sctx = current_sctx;

    // Save and restore cursor position and curswant, in case it was changed
    // via :normal commands
    save_pos = curwin->w_cursor;
    save_curswant = curwin->w_curswant;
    save_set_curswant = curwin->w_set_curswant;
    set_vim_var_nr(VV_LNUM, curwin->w_cursor.lnum);
    यदि (use_sandbox)
	++sandbox;
    ++textlock;
    current_sctx = curbuf->b_p_script_ctx[BV_INDE];

    // Need to make a copy, the 'indentexpr' option could be changed while
    // evaluating it.
    inde_copy = vim_strsave(curbuf->b_p_inde);
    यदि (inde_copy != रिक्त)
    {
	indent = (पूर्णाङ्क)eval_to_संख्या(inde_copy, TRUE);
	vim_free(inde_copy);
    }

    यदि (use_sandbox)
	--sandbox;
    --textlock;
    current_sctx = save_sctx;

    // Restore the cursor position so that 'indentexpr' doesn't need to.
    // Pretend to be in Insert mode, allow cursor past end of line for "o"
    // command.
    save_State = State;
    State = MODE_जोड़ो;
    curwin->w_cursor = save_pos;
    curwin->w_curswant = save_curswant;
    curwin->w_set_curswant = save_set_curswant;
    check_cursor();
    State = save_State;

    // Reset did_throw, unless 'debug' has "throw" and inside a try/catch.
    यदि (did_throw && (vim_strchr(p_debug, 't') == रिक्त || trylevel == 0))
    {
	handle_did_throw();
	did_throw = FALSE;
    }

    // If there is an error, just keep the current indent.
    यदि (indent < 0)
	indent = get_indent();

    प्रत्यागम indent;
}
#endif

    स्थिर पूर्णाङ्क
lisp_match(char_u *p)
{
    char_u	buf[LSIZE];
    पूर्णाङ्क		len;
    char_u	*word = *curbuf->b_p_lw != NUL ? curbuf->b_p_lw : p_lispwords;

    यावत् (*word != NUL)
    {
	len = copy_option_part(&word, buf, LSIZE, ",");
	यदि (STRNCMP(buf, p, len) == 0 && IS_WHITE_OR_NUL(p[len]))
	    प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
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
    पूर्णाङ्क
get_lisp_indent(शून्य)
{
    pos_T	*pos, realpos, paren;
    पूर्णाङ्क		amount;
    char_u	*that;
    colnr_T	col;
    colnr_T	firsttry;
    पूर्णाङ्क		parencount, quotecount;
    पूर्णाङ्क		vi_lisp;

    // Set vi_lisp to use the vi-compatible method
    vi_lisp = (vim_strchr(p_cpo, CPO_LISP) != रिक्त);

    realpos = curwin->w_cursor;
    curwin->w_cursor.col = 0;

    यदि ((pos = findmatch(रिक्त, '(')) == रिक्त)
	pos = findmatch(रिक्त, '[');
    अन्यथा
    {
	paren = *pos;
	pos = findmatch(रिक्त, '[');
	यदि (pos == रिक्त || LT_POSP(pos, &paren))
	    pos = &paren;
    }
    यदि (pos != रिक्त)
    {
	// Extra trick: Take the indent of the first previous non-white
	// line that is at the same () level.
	amount = -1;
	parencount = 0;

	यावत् (--curwin->w_cursor.lnum >= pos->lnum)
	{
	    यदि (linewhite(curwin->w_cursor.lnum))
		अनुवर्त;
	    प्रति (that = ml_get_curline(); *that != NUL; ++that)
	    {
		यदि (*that == ';')
		{
		    यावत् (*(that + 1) != NUL)
			++that;
		    अनुवर्त;
		}
		यदि (*that == '\\')
		{
		    यदि (*(that + 1) != NUL)
			++that;
		    अनुवर्त;
		}
		यदि (*that == '"' && *(that + 1) != NUL)
		{
		    यावत् (*++that && *that != '"')
		    {
			// skipping escaped characters in the string
			यदि (*that == '\\')
			{
			    यदि (*++that == NUL)
				भङ्ग;
			    यदि (that[1] == NUL)
			    {
				++that;
				भङ्ग;
			    }
			}
		    }
		    यदि (*that == NUL)
			भङ्ग;
		}
		यदि (*that == '(' || *that == '[')
		    ++parencount;
		अन्यथा यदि (*that == ')' || *that == ']')
		    --parencount;
	    }
	    यदि (parencount == 0)
	    {
		amount = get_indent();
		भङ्ग;
	    }
	}

	यदि (amount == -1)
	{
	    curwin->w_cursor.lnum = pos->lnum;
	    curwin->w_cursor.col = pos->col;
	    col = pos->col;

	    that = ml_get_curline();

	    यदि (vi_lisp && get_indent() == 0)
		amount = 2;
	    अन्यथा
	    {
		char_u		*पंक्ति = that;
		chartabsize_T	cts;

		init_chartabsize_arg(&cts, curwin, pos->lnum, 0, पंक्ति, पंक्ति);
		यावत् (*cts.cts_ptr != NUL && col > 0)
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

		यदि (!vi_lisp && (*that == '(' || *that == '[')
						      && lisp_match(that + 1))
		    amount += 2;
		अन्यथा
		{
		    यदि (*that != NUL)
		    {
			that++;
			amount++;
		    }
		    firsttry = amount;

		    init_chartabsize_arg(&cts, curwin, (colnr_T)(that - पंक्ति),
							   amount, पंक्ति, that);
		    यावत् (VIM_ISWHITE(*cts.cts_ptr))
		    {
			cts.cts_vcol += lbr_chartabsize(&cts);
			++cts.cts_ptr;
		    }
		    that = cts.cts_ptr;
		    amount = cts.cts_vcol;
		    clear_chartabsize_arg(&cts);

		    यदि (*that && *that != ';') // not a comment line
		    {
			// test *that != '(' to accommodate first let/do
			// argument if it is more than one line
			यदि (!vi_lisp && *that != '(' && *that != '[')
			    firsttry++;

			parencount = 0;
			quotecount = 0;

			init_chartabsize_arg(&cts, curwin,
				   (colnr_T)(that - पंक्ति), amount, पंक्ति, that);
			यदि (vi_lisp
				|| (*that != '"'
				    && *that != '\''
				    && *that != '#'
				    && (*that < '0' || *that > '9')))
			{
			    यावत् (*cts.cts_ptr
				    && (!VIM_ISWHITE(*cts.cts_ptr)
					|| quotecount
					|| parencount)
				    && (!((*cts.cts_ptr == '('
							|| *cts.cts_ptr == '[')
					    && !quotecount
					    && !parencount
					    && vi_lisp)))
			    {
				यदि (*cts.cts_ptr == '"')
				    quotecount = !quotecount;
				यदि ((*cts.cts_ptr == '(' || *cts.cts_ptr == '[')
							       && !quotecount)
				    ++parencount;
				यदि ((*cts.cts_ptr == ')' || *cts.cts_ptr == ']')
							       && !quotecount)
				    --parencount;
				यदि (*cts.cts_ptr == '\\'
						    && *(cts.cts_ptr+1) != NUL)
				    cts.cts_vcol += lbr_chartabsize_adv(&cts);
				cts.cts_vcol += lbr_chartabsize_adv(&cts);
			    }
			}
			यावत् (VIM_ISWHITE(*cts.cts_ptr))
			{
			    cts.cts_vcol += lbr_chartabsize(&cts);
			    ++cts.cts_ptr;
			}
			that = cts.cts_ptr;
			amount = cts.cts_vcol;
			clear_chartabsize_arg(&cts);

			यदि (!*that || *that == ';')
			    amount = firsttry;
		    }
		}
	    }
	}
    }
    अन्यथा
	amount = 0;	// no matching '(' or '[' found, use zero indent

    curwin->w_cursor = realpos;

    प्रत्यागम amount;
}

/*
 * Re-indent the current line, based on the current contents of it and the
 * surrounding lines. Fixing the cursor position seems really easy -- I'm very
 * confused what all the part that handles Control-T is doing that I'm not.
 * "get_the_indent" should be get_c_indent, get_expr_indent or get_lisp_indent.
 */

    शून्य
fixthisline(पूर्णाङ्क (*get_the_indent)(शून्य))
{
    पूर्णाङ्क amount = get_the_indent();

    यदि (amount < 0)
	प्रत्यागम;

    change_indent(INDENT_SET, amount, FALSE, 0, TRUE);
    यदि (linewhite(curwin->w_cursor.lnum))
	did_ai = TRUE;	// delete the indent if the line stays empty
}

/*
 * Return TRUE if 'indentexpr' should be used for Lisp indenting.
 * Caller may want to check 'autoindent'.
 */
    पूर्णाङ्क
use_indentexpr_for_lisp(शून्य)
{
#ifdef FEAT_EVAL
    प्रत्यागम curbuf->b_p_lisp
		&& *curbuf->b_p_inde != NUL
		&& STRCMP(curbuf->b_p_lop, "expr:1") == 0;
#अन्यथा
    प्रत्यागम FALSE;
#endif
}

/*
 * Fix indent for 'lisp' and 'cindent'.
 */
    शून्य
fix_indent(शून्य)
{
    यदि (p_paste)
	प्रत्यागम;  // no auto-indenting when 'paste' is set
    यदि (curbuf->b_p_lisp && curbuf->b_p_ai)
    {
	यदि (use_indentexpr_for_lisp())
	    do_c_व्यंजक_indent();
	अन्यथा
	    fixthisline(get_lisp_indent);
    }
    अन्यथा यदि (cindent_on())
	do_c_व्यंजक_indent();
}

#यदि defined(FEAT_EVAL)
/*
 * "indent()" function
 */
    शून्य
f_indent(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    यदि (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    lnum = tv_get_lnum(argvars);
    यदि (lnum >= 1 && lnum <= curbuf->b_ml.ml_पंक्ति_गिनती)
	rettv->vval.v_संख्या = get_indent_lnum(lnum);
    अन्यथा
    {
	यदि (in_vim9script())
	    semsg(_(e_invalid_पंक्ति_संख्या_nr), lnum);
	rettv->vval.v_संख्या = -1;
    }
}

/*
 * "lispindent(lnum)" function
 */
    शून्य
f_lispindent(typval_T *argvars UNUSED, typval_T *rettv)
{
    pos_T	pos;
    linenr_T	lnum;

    यदि (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    यदि (lnum >= 1 && lnum <= curbuf->b_ml.ml_पंक्ति_गिनती)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_संख्या = get_lisp_indent();
	curwin->w_cursor = pos;
    }
    अन्यथा यदि (in_vim9script())
	semsg(_(e_invalid_पंक्ति_संख्या_nr), lnum);
    अन्यथा
	rettv->vval.v_संख्या = -1;
}
#endif
