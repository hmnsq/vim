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

#जर defined(FEAT_VARTABS)

/*
 * Set the integer values corresponding to the string setting of 'vartabstop'.
 * "array" will be set, caller must free it if needed.
 * Return FAIL for an error.
 */
    पूर्णांक
tabstop_set(char_u *var, पूर्णांक **array)
{
    पूर्णांक	    valcount = 1;
    पूर्णांक	    t;
    char_u  *cp;

    जर (var[0] == NUL || (var[0] == '0' && var[1] == NUL))
    {
	*array = रिक्त;
	परत OK;
    }

    साठी (cp = var; *cp != NUL; ++cp)
    {
	जर (cp == var || cp[-1] == ',')
	{
	    char_u *end;

	    जर (म_से_दीर्घ((अक्षर *)cp, (अक्षर **)&end, 10) <= 0)
	    {
		जर (cp != end)
		    emsg(_(e_argument_must_be_positive));
		अन्यथा
		    semsg(_(e_invalid_argument_str), cp);
		परत FAIL;
	    }
	}

	जर (VIM_ISDIGIT(*cp))
	    सुरूठेवा;
	जर (cp[0] == ',' && cp > var && cp[-1] != ',' && cp[1] != NUL)
	{
	    ++valcount;
	    सुरूठेवा;
	}
	semsg(_(e_invalid_argument_str), var);
	परत FAIL;
    }

    *array = ALLOC_MULT(पूर्णांक, valcount + 1);
    जर (*array == रिक्त)
	परत FAIL;
    (*array)[0] = valcount;

    t = 1;
    साठी (cp = var; *cp != NUL;)
    {
	पूर्णांक n = म_से_प((अक्षर *)cp);

	// Catch negative values, overflow and ridiculous big values.
	जर (n <= 0 || n > TABSTOP_MAX)
	{
	    semsg(_(e_invalid_argument_str), cp);
	    VIM_CLEAR(*array);
	    परत FAIL;
	}
	(*array)[t++] = n;
	पर्यंत (*cp != NUL && *cp != ',')
	    ++cp;
	जर (*cp != NUL)
	    ++cp;
    }

    परत OK;
}

/*
 * Calculate the number of screen spaces a tab will occupy.
 * If "vts" is set then the tab widths are taken from that array,
 * otherwise the value of ts is used.
 */
    पूर्णांक
tabstop_padding(colnr_T col, पूर्णांक ts_arg, पूर्णांक *vts)
{
    पूर्णांक		ts = ts_arg == 0 ? 8 : ts_arg;
    पूर्णांक		tabcount;
    colnr_T	tabcol = 0;
    पूर्णांक		t;
    पूर्णांक		padding = 0;

    जर (vts == रिक्त || vts[0] == 0)
	परत ts - (col % ts);

    tabcount = vts[0];

    साठी (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	जर (tabcol > col)
	{
	    padding = (पूर्णांक)(tabcol - col);
	    थांबा;
	}
    }
    जर (t > tabcount)
	padding = vts[tabcount] - (पूर्णांक)((col - tabcol) % vts[tabcount]);

    परत padding;
}

/*
 * Find the size of the tab that covers a particular column.
 *
 * If this is being called as part of a shift operation, col is not the cursor
 * column but is the column number to the left of the first non-whitespace
 * character in the line.  If the shift is to the left (left = TRUE), then
 * return the size of the tab interval to the left of the column.
 */
    पूर्णांक
tabstop_at(colnr_T col, पूर्णांक ts, पूर्णांक *vts, पूर्णांक डावा)
{
    पूर्णांक		tabcount;	// Number of tab stops in the list of variable
				// tab stops.
    colnr_T	tabcol = 0;	// Column of the tab stop under consideration.
    पूर्णांक		t;		// Tabstop index in the list of variable tab
				// stops.
    पूर्णांक		tab_size = 0;	// Size of the tab stop interval to the right
				// or left of the col.

    जर (vts == 0 || vts[0] == 0)
	परत ts;

    tabcount = vts[0];
    साठी (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	जर (tabcol > col)
	{
	    // If shifting left (left != 0), and if the column to the left of
	    // the first first non-blank character (col) in the line is
	    // already to the left of the first tabstop, set the shift amount
	    // (tab_size) to just enough to shift the line to the left margin.
	    // The value doesn't seem to matter as long as it is at least that
	    // distance.
	    जर (डावा && (t == 1))
		tab_size = col;
	    अन्यथा
		tab_size = vts[t - (डावा ? 1 : 0)];
	    थांबा;
	}
    }
    जर (t > tabcount)		// If the value of the index t is beyond the
				// end of the list, use the tab stop value at
				// the end of the list.
	tab_size = vts[tabcount];

    परत tab_size;
}

/*
 * Find the column on which a tab starts.
 */
    colnr_T
tabstop_start(colnr_T col, पूर्णांक ts, पूर्णांक *vts)
{
    पूर्णांक		tabcount;
    colnr_T	tabcol = 0;
    पूर्णांक		t;
    पूर्णांक		excess;

    जर (vts == रिक्त || vts[0] == 0)
	परत col - col % ts;

    tabcount = vts[0];
    साठी (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	जर (tabcol > col)
	    परत tabcol - vts[t];
    }

    excess = tabcol % vts[tabcount];
    परत col - (col - excess) % vts[tabcount];
}

/*
 * Find the number of tabs and spaces necessary to get from one column
 * to another.
 */
    शून्य
tabstop_fromto(
	colnr_T start_col,
	colnr_T end_col,
	पूर्णांक	ts_arg,
	पूर्णांक	*vts,
	पूर्णांक	*ntabs,
	पूर्णांक	*nspcs)
{
    पूर्णांक		spaces = end_col - start_col;
    colnr_T	tabcol = 0;
    पूर्णांक		padding = 0;
    पूर्णांक		tabcount;
    पूर्णांक		t;
    पूर्णांक		ts = ts_arg == 0 ? curbuf->b_p_ts : ts_arg;

    जर (vts == रिक्त || vts[0] == 0)
    {
	पूर्णांक tabs = 0;
	पूर्णांक initspc = 0;

	initspc = ts - (start_col % ts);
	जर (spaces >= initspc)
	{
	    spaces -= initspc;
	    tabs++;
	}
	tabs += spaces / ts;
	spaces -= (spaces / ts) * ts;

	*ntabs = tabs;
	*nspcs = spaces;
	परत;
    }

    // Find the padding needed to reach the next tabstop.
    tabcount = vts[0];
    साठी (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	जर (tabcol > start_col)
	{
	    padding = (पूर्णांक)(tabcol - start_col);
	    थांबा;
	}
    }
    जर (t > tabcount)
	padding = vts[tabcount] - (पूर्णांक)((start_col - tabcol) % vts[tabcount]);

    // If the space needed is less than the padding no tabs can be used.
    जर (spaces < padding)
    {
	*ntabs = 0;
	*nspcs = spaces;
	परत;
    }

    *ntabs = 1;
    spaces -= padding;

    // At least one tab has been used. See if any more will fit.
    पर्यंत (spaces != 0 && ++t <= tabcount)
    {
	padding = vts[t];
	जर (spaces < padding)
	{
	    *nspcs = spaces;
	    परत;
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
    स्थिर पूर्णांक
tabstop_eq(पूर्णांक *ts1, पूर्णांक *ts2)
{
    पूर्णांक		t;

    जर ((ts1 == 0 && ts2) || (ts1 && ts2 == 0))
	परत FALSE;
    जर (ts1 == ts2)
	परत TRUE;
    जर (ts1[0] != ts2[0])
	परत FALSE;

    साठी (t = 1; t <= ts1[0]; ++t)
	जर (ts1[t] != ts2[t])
	    परत FALSE;

    परत TRUE;
}

# जर defined(FEAT_BEVAL)
/*
 * Copy a tabstop array, allocating space for the new array.
 */
    पूर्णांक *
tabstop_copy(पूर्णांक *oldts)
{
    पूर्णांक		*newts;
    पूर्णांक		t;

    जर (oldts == रिक्त)
	परत रिक्त;
    newts = ALLOC_MULT(पूर्णांक, oldts[0] + 1);
    जर (newts != रिक्त)
	साठी (t = 0; t <= oldts[0]; ++t)
	    newts[t] = oldts[t];
    परत newts;
}
# endif

/*
 * Return a count of the number of tabstops.
 */
    पूर्णांक
tabstop_मोजणी(पूर्णांक *ts)
{
    परत ts != रिक्त ? ts[0] : 0;
}

/*
 * Return the first tabstop, or 8 if there are no tabstops defined.
 */
    पूर्णांक
tabstop_first(पूर्णांक *ts)
{
    परत ts != रिक्त ? ts[1] : 8;
}

#endif

/*
 * Return the effective shiftwidth value for current buffer, using the
 * 'tabstop' value when 'shiftwidth' is zero.
 */
    दीर्घ
get_sw_मूल्य(buf_T *buf)
{
    परत get_sw_मूल्य_col(buf, 0, FALSE);
}

/*
 * Idem, using "pos".
 */
    स्थिर दीर्घ
get_sw_मूल्य_pos(buf_T *buf, pos_T *pos, पूर्णांक डावा)
{
    pos_T save_cursor = curwin->w_cursor;
    दीर्घ sw_मूल्य;

    curwin->w_cursor = *pos;
    sw_मूल्य = get_sw_मूल्य_col(buf, get_nolist_virtcol(), डावा);
    curwin->w_cursor = save_cursor;
    परत sw_मूल्य;
}

/*
 * Idem, using the first non-black in the current line.
 */
    दीर्घ
get_sw_मूल्य_indent(buf_T *buf, पूर्णांक डावा)
{
    pos_T pos = curwin->w_cursor;

    pos.col = getwhitecols_curline();
    परत get_sw_मूल्य_pos(buf, &pos, डावा);
}

/*
 * Idem, using virtual column "col".
 */
    दीर्घ
get_sw_मूल्य_col(buf_T *buf, colnr_T col UNUSED, पूर्णांक डावा UNUSED)
{
    परत buf->b_p_sw ? buf->b_p_sw :
#ifdef FEAT_VARTABS
	tabstop_at(col, buf->b_p_ts, buf->b_p_vts_array, डावा);
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
    परत curbuf->b_p_sts < 0 ? get_sw_मूल्य(curbuf) : curbuf->b_p_sts;
}

/*
 * Count the size (in window cells) of the indent in the current line.
 */
    पूर्णांक
get_indent(शून्य)
{
#ifdef FEAT_VARTABS
    परत get_indent_str_vtab(ml_get_curline(), (पूर्णांक)curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#अन्यथा
    परत get_indent_str(ml_get_curline(), (पूर्णांक)curbuf->b_p_ts, FALSE);
#endif
}

/*
 * Count the size (in window cells) of the indent in line "lnum".
 */
    पूर्णांक
get_indent_lnum(linenr_T lnum)
{
#ifdef FEAT_VARTABS
    परत get_indent_str_vtab(ml_get(lnum), (पूर्णांक)curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#अन्यथा
    परत get_indent_str(ml_get(lnum), (पूर्णांक)curbuf->b_p_ts, FALSE);
#endif
}

#जर defined(FEAT_FOLDING)
/*
 * Count the size (in window cells) of the indent in line "lnum" of buffer
 * "buf".
 */
    पूर्णांक
get_indent_buf(buf_T *buf, linenr_T lnum)
{
# ifdef FEAT_VARTABS
    परत get_indent_str_vtab(ml_get_buf(buf, lnum, FALSE),
			       (पूर्णांक)buf->b_p_ts, buf->b_p_vts_array, FALSE);
# अन्यथा
    परत get_indent_str(ml_get_buf(buf, lnum, FALSE), (पूर्णांक)buf->b_p_ts, FALSE);
# endif
}
#endif

/*
 * count the size (in window cells) of the indent in line "ptr", with
 * 'tabstop' at "ts"
 */
    पूर्णांक
get_indent_str(
    char_u	*ptr,
    पूर्णांक		ts,
    पूर्णांक		no_ts) // if TRUE, count a tab as ^I
{
    पूर्णांक		मोजणी = 0;

    साठी ( ; *ptr; ++ptr)
    {
	जर (*ptr == TAB)    // count a tab for what it is worth
	{
	    जर (!no_ts)
		मोजणी += ts - (मोजणी % ts);
	    अन्यथा
		मोजणी += ptr2cells(ptr);
	}
	अन्यथा जर (*ptr == ' ')
	    ++मोजणी;		// count a space for one
	अन्यथा
	    थांबा;
    }
    परत मोजणी;
}

#जर defined(FEAT_VARTABS)
/*
 * Count the size (in window cells) of the indent in line "ptr", using
 * variable tabstops.
 * If "no_ts" is TRUE, count a tab as ^I.
 */
    पूर्णांक
get_indent_str_vtab(char_u *ptr, पूर्णांक ts, पूर्णांक *vts, पूर्णांक no_ts)
{
    पूर्णांक		मोजणी = 0;

    साठी ( ; *ptr; ++ptr)
    {
	जर (*ptr == TAB)    // count a tab for what it is worth
	{
	    जर (!no_ts)
		मोजणी += tabstop_padding(मोजणी, ts, vts);
	    अन्यथा
		मोजणी += ptr2cells(ptr);
	}
	अन्यथा जर (*ptr == ' ')
	    ++मोजणी;		// count a space for one
	अन्यथा
	    थांबा;
    }
    परत मोजणी;
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
    पूर्णांक
set_indent(
    पूर्णांक		size,		    // measured in spaces
    पूर्णांक		flags)
{
    char_u	*p;
    char_u	*newline;
    char_u	*oldline;
    char_u	*s;
    पूर्णांक		todo;
    पूर्णांक		ind_len;	    // measured in characters
    पूर्णांक		ओळ_len;	    // size of the line (including the NUL)
    पूर्णांक		doit = FALSE;
    पूर्णांक		ind_done = 0;	    // measured in spaces
#ifdef FEAT_VARTABS
    पूर्णांक		ind_col = 0;
#endif
    पूर्णांक		tab_pad;
    पूर्णांक		retval = FALSE;
    पूर्णांक		orig_char_len = -1; // number of initial whitespace chars when
				    // 'et' and 'pi' are both set

    // First check if there is anything to do and compute the number of
    // characters needed for the indent.
    todo = size;
    ind_len = 0;
    p = oldline = ml_get_curline();
    ओळ_len = ml_get_curline_len() + 1;

    // Calculate the buffer size for the new indent, and check to see if it
    // isn't already set

    // if 'expandtab' isn't set: use TABs; if both 'expandtab' and
    // 'preserveindent' are set count the number of characters at the
    // beginning of the line to be copied
    जर (!curbuf->b_p_et || (!(flags & SIN_घाला) && curbuf->b_p_pi))
    {
	// If 'preserveindent' is set then reuse as much as possible of
	// the existing indent structure for the new indent
	जर (!(flags & SIN_घाला) && curbuf->b_p_pi)
	{
	    ind_done = 0;

	    // count as many characters as we can use
	    पर्यंत (todo > 0 && VIM_ISWHITE(*p))
	    {
		जर (*p == TAB)
		{
#ifdef FEAT_VARTABS
		    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#अन्यथा
		    tab_pad = (पूर्णांक)curbuf->b_p_ts
					   - (ind_done % (पूर्णांक)curbuf->b_p_ts);
#endif
		    // stop if this tab will overshoot the target
		    जर (todo < tab_pad)
			थांबा;
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
	    जर (curbuf->b_p_et)
		orig_char_len = ind_len;

	    // Fill to next tabstop with a tab, if possible
#ifdef FEAT_VARTABS
	    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
						curbuf->b_p_vts_array);
#अन्यथा
	    tab_pad = (पूर्णांक)curbuf->b_p_ts - (ind_done % (पूर्णांक)curbuf->b_p_ts);
#endif
	    जर (todo >= tab_pad && orig_char_len == -1)
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
	साठी (;;)
	{
	    tab_pad = tabstop_padding(ind_col, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
	    जर (todo < tab_pad)
		थांबा;
	    जर (*p != TAB)
		doit = TRUE;
	    अन्यथा
		++p;
	    todo -= tab_pad;
	    ++ind_len;
	    ind_col += tab_pad;
	}
#अन्यथा
	पर्यंत (todo >= (पूर्णांक)curbuf->b_p_ts)
	{
	    जर (*p != TAB)
		doit = TRUE;
	    अन्यथा
		++p;
	    todo -= (पूर्णांक)curbuf->b_p_ts;
	    ++ind_len;
	    // ind_done += (int)curbuf->b_p_ts;
	}
#endif
    }
    // count spaces required for indent
    पर्यंत (todo > 0)
    {
	जर (*p != ' ')
	    doit = TRUE;
	अन्यथा
	    ++p;
	--todo;
	++ind_len;
	// ++ind_done;
    }

    // Return if the indent is OK already.
    जर (!doit && !VIM_ISWHITE(*p) && !(flags & SIN_घाला))
	परत FALSE;

    // Allocate memory for the new line.
    जर (flags & SIN_घाला)
	p = oldline;
    अन्यथा
    {
	p = skipwhite(p);
	ओळ_len -= (पूर्णांक)(p - oldline);
    }

    // If 'preserveindent' and 'expandtab' are both set keep the original
    // characters and allocate accordingly.  We will fill the rest with spaces
    // after the if (!curbuf->b_p_et) below.
    जर (orig_char_len != -1)
    {
	newline = alloc(orig_char_len + size - ind_done + ओळ_len);
	जर (newline == रिक्त)
	    परत FALSE;
	todo = size - ind_done;
	ind_len = orig_char_len + todo;    // Set total length of indent in
					   // characters, which may have been
					   // undercounted until now
	p = oldline;
	s = newline;
	पर्यंत (orig_char_len > 0)
	{
	    *s++ = *p++;
	    orig_char_len--;
	}

	// Skip over any additional white space (useful when newindent is less
	// than old)
	पर्यंत (VIM_ISWHITE(*p))
	    ++p;

    }
    अन्यथा
    {
	todo = size;
	newline = alloc(ind_len + ओळ_len);
	जर (newline == रिक्त)
	    परत FALSE;
	s = newline;
    }

    // Put the characters in the new line.
    // if 'expandtab' isn't set: use TABs
    जर (!curbuf->b_p_et)
    {
	// If 'preserveindent' is set then reuse as much as possible of
	// the existing indent structure for the new indent
	जर (!(flags & SIN_घाला) && curbuf->b_p_pi)
	{
	    p = oldline;
	    ind_done = 0;

	    पर्यंत (todo > 0 && VIM_ISWHITE(*p))
	    {
		जर (*p == TAB)
		{
#ifdef FEAT_VARTABS
		    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#अन्यथा
		    tab_pad = (पूर्णांक)curbuf->b_p_ts
					   - (ind_done % (पूर्णांक)curbuf->b_p_ts);
#endif
		    // stop if this tab will overshoot the target
		    जर (todo < tab_pad)
			थांबा;
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
	    tab_pad = (पूर्णांक)curbuf->b_p_ts - (ind_done % (पूर्णांक)curbuf->b_p_ts);
#endif
	    जर (todo >= tab_pad)
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
	साठी (;;)
	{
	    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
	    जर (todo < tab_pad)
		थांबा;
	    *s++ = TAB;
	    todo -= tab_pad;
	    ind_done += tab_pad;
	}
#अन्यथा
	पर्यंत (todo >= (पूर्णांक)curbuf->b_p_ts)
	{
	    *s++ = TAB;
	    todo -= (पूर्णांक)curbuf->b_p_ts;
	}
#endif
    }
    पर्यंत (todo > 0)
    {
	*s++ = ' ';
	--todo;
    }
    mch_memmove(s, p, (माप_प्रकार)ओळ_len);

    // Replace the line (unless undo fails).
    जर (!(flags & SIN_UNDO) || u_savesub(curwin->w_cursor.lnum) == OK)
    {
	colnr_T old_offset = (colnr_T)(p - oldline);
	colnr_T new_offset = (colnr_T)(s - newline);

	// this may free "newline"
	ml_replace(curwin->w_cursor.lnum, newline, FALSE);
	जर (flags & SIN_CHANGED)
	    changed_bytes(curwin->w_cursor.lnum, 0);

	// Correct saved cursor position if it is in this line.
	जर (saved_cursor.lnum == curwin->w_cursor.lnum)
	{
	    जर (saved_cursor.col >= old_offset)
		// cursor was after the indent, adjust for the number of
		// bytes added/removed
		saved_cursor.col += ind_len - old_offset;
	    अन्यथा जर (saved_cursor.col >= new_offset)
		// cursor was in the indent, and is now after it, put it back
		// at the start of the indent (replacing spaces with TAB)
		saved_cursor.col = new_offset;
	}
#ifdef FEAT_PROP_POPUP
	जर (curbuf->b_has_textprop)
	{
	    पूर्णांक added = ind_len - old_offset;

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
    परत retval;
}

/*
 * Return the indent of the current line after a number.  Return -1 if no
 * number was found.  Used for 'n' in 'formatoptions': numbered list.
 * Since a pattern is used it can actually handle more than numbers.
 */
    पूर्णांक
get_संख्या_indent(linenr_T lnum)
{
    colnr_T	col;
    pos_T	pos;

    regmatch_T	regmatch;
    पूर्णांक		lead_len = 0;	// length of comment leader

    जर (lnum > curbuf->b_ml.ml_ओळ_मोजणी)
	परत -1;
    pos.lnum = 0;

    // In format_lines() (i.e. not insert mode), fo+=q is needed too...
    जर ((State & MODE_घाला) || has_format_option(FO_Q_COMS))
	lead_len = get_leader_len(ml_get(lnum), रिक्त, FALSE, TRUE);

    regmatch.regprog = vim_regcomp(curbuf->b_p_flp, RE_MAGIC);
    जर (regmatch.regprog != रिक्त)
    {
	regmatch.rm_ic = FALSE;

	// vim_regexec() expects a pointer to a line.  This lets us
	// start matching for the flp beyond any comment leader...
	जर (vim_regexec(&regmatch, ml_get(lnum) + lead_len, (colnr_T)0))
	{
	    pos.lnum = lnum;
	    pos.col = (colnr_T)(*regmatch.endp - ml_get(lnum));
	    pos.coladd = 0;
	}
	vim_regfree(regmatch.regprog);
    }

    जर (pos.lnum == 0 || *ml_get_pos(&pos) == NUL)
	परत -1;
    getvcol(curwin, &pos, &col, रिक्त, रिक्त, 0);
    परत (पूर्णांक)col;
}

#जर defined(FEAT_LINEBREAK)
/*
 * Check "briopt" as 'breakindentopt' and update the members of "wp".
 * This is called when 'breakindentopt' is changed and when a window is
 * initialized.
 * Returns FAIL for failure, OK otherwise.
 */
    पूर्णांक
briopt_check(
    char_u	*briopt,	// when NULL: use "wp->w_p_briopt"
    win_T	*wp)		// when NULL: only check "briopt"
{
    char_u	*p;
    पूर्णांक		bri_shift = 0;
    दीर्घ	bri_min = 20;
    पूर्णांक		bri_sbr = FALSE;
    पूर्णांक		bri_list = 0;
    पूर्णांक		bri_vcol = 0;

    जर (briopt != रिक्त)
	p = briopt;
    अन्यथा
	p = wp->w_p_briopt;

    पर्यंत (*p != NUL)
    {
	// Note: Keep this in sync with p_briopt_values
	जर (STRNCMP(p, "shift:", 6) == 0
		 && ((p[6] == '-' && VIM_ISDIGIT(p[7])) || VIM_ISDIGIT(p[6])))
	{
	    p += 6;
	    bri_shift = getdigits(&p);
	}
	अन्यथा जर (STRNCMP(p, "min:", 4) == 0 && VIM_ISDIGIT(p[4]))
	{
	    p += 4;
	    bri_min = getdigits(&p);
	}
	अन्यथा जर (STRNCMP(p, "sbr", 3) == 0)
	{
	    p += 3;
	    bri_sbr = TRUE;
	}
	अन्यथा जर (STRNCMP(p, "list:", 5) == 0)
	{
	    p += 5;
	    bri_list = getdigits(&p);
	}
	अन्यथा जर (STRNCMP(p, "column:", 7) == 0)
	{
	    p += 7;
	    bri_vcol = getdigits(&p);
	}
	जर (*p != ',' && *p != NUL)
	    परत FAIL;
	जर (*p == ',')
	    ++p;
    }

    जर (wp == रिक्त)
	परत OK;

    wp->w_briopt_shift = bri_shift;
    wp->w_briopt_min   = bri_min;
    wp->w_briopt_sbr   = bri_sbr;
    wp->w_briopt_list  = bri_list;
    wp->w_briopt_vcol  = bri_vcol;

    परत OK;
}

/*
 * Return appropriate space number for breakindent, taking influencing
 * parameters into account. Window must be specified, since it is not
 * necessarily always the current one.
 */
    पूर्णांक
get_breakindent_win(
    win_T	*wp,
    char_u	*ओळ) // start of the line
{
    स्थिर पूर्णांक	    prev_indent = 0;	// cached indent value
    स्थिर दीर्घ	    prev_ts     = 0L;	// cached tabstop value
# ifdef FEAT_VARTABS
    स्थिर पूर्णांक      *prev_vts = रिक्त;   // cached vartabs values
# endif
    स्थिर पूर्णांक	    prev_fnum   = 0;	// cached buffer number
    स्थिर char_u   *prev_ओळ  = रिक्त;	// cached copy of "line"
    स्थिर varnumber_T prev_tick = 0;   // changedtick of cached value
    स्थिर पूर्णांक      prev_list = 0;	// cached list indent
    स्थिर पूर्णांक      prev_listopt = 0;	// cached w_p_briopt_list value
    स्थिर पूर्णांक      prev_no_ts = FALSE;	// cached no_ts value
    स्थिर अचिन्हित prev_dy_uhex = 0;	// cached 'display' "uhex" value
    // cached formatlistpat value
    स्थिर char_u   *prev_flp = रिक्त;
    पूर्णांक		    bri = 0;
    // window width minus window margin space, i.e. what rests for text
    स्थिरांक पूर्णांक	    eff_wwidth = wp->w_width
					  - win_col_off(wp) + win_col_off2(wp);

    // In list mode, if 'listchars' "tab" isn't set, a TAB is displayed as ^I.
    पूर्णांक no_ts = wp->w_p_list && wp->w_lcs_chars.tab1 == NUL;

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
    जर (prev_fnum != wp->w_buffer->b_fnum
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
	    || prev_ओळ == रिक्त || STRCMP(prev_ओळ, ओळ) != 0
	)
    {
	prev_fnum = wp->w_buffer->b_fnum;
	vim_free(prev_ओळ);
	prev_ओळ = vim_strsave(ओळ);
	prev_ts = wp->w_buffer->b_p_ts;
# ifdef FEAT_VARTABS
	prev_vts = wp->w_buffer->b_p_vts_array;
	जर (wp->w_briopt_vcol == 0)
	    prev_indent = get_indent_str_vtab(ओळ,
				     (पूर्णांक)wp->w_buffer->b_p_ts,
				      wp->w_buffer->b_p_vts_array, no_ts);
# अन्यथा
	जर (wp->w_briopt_vcol == 0)
	    prev_indent = get_indent_str(ओळ,
					(पूर्णांक)wp->w_buffer->b_p_ts, no_ts);
# endif
	prev_tick = CHANGEDTICK(wp->w_buffer);
	prev_listopt = wp->w_briopt_list;
	prev_list = 0;
	prev_no_ts = no_ts;
	prev_dy_uhex = (dy_flags & DY_UHEX);
	vim_free(prev_flp);
	prev_flp = vim_strsave(get_flp_मूल्य(wp->w_buffer));
	// add additional indent for numbered lists
	जर (wp->w_briopt_list != 0 && wp->w_briopt_vcol == 0)
	{
	    regmatch_T	    regmatch;

	    regmatch.regprog = vim_regcomp(prev_flp,
				   RE_MAGIC + RE_STRING + RE_AUTO + RE_STRICT);

	    जर (regmatch.regprog != रिक्त)
	    {
		regmatch.rm_ic = FALSE;
		जर (vim_regexec(&regmatch, ओळ, 0))
		{
		    जर (wp->w_briopt_list > 0)
			prev_list = wp->w_briopt_list;
		    अन्यथा
		    {
			char_u	*ptr = *regmatch.startp;
			char_u	*end_ptr = *regmatch.endp;
			पूर्णांक	indent = 0;

			// Compute the width of the matched text.
			// Use win_chartabsize() so that TAB size is correct,
			// while wrapping is ignored.
			पर्यंत (ptr < end_ptr)
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
    जर (wp->w_briopt_vcol != 0)
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
    जर (wp->w_briopt_list > 0)
	bri += prev_list;

    // indent minus the length of the showbreak string
    जर (wp->w_briopt_sbr)
	bri -= vim_strsize(get_showbreak_मूल्य(wp));


    // never indent past left window margin
    जर (bri < 0)
	bri = 0;

    // always leave at least bri_min characters on the left,
    // if text width is sufficient
    अन्यथा जर (bri > eff_wwidth - wp->w_briopt_min)
	bri = (eff_wwidth - wp->w_briopt_min < 0)
					   ? 0 : eff_wwidth - wp->w_briopt_min;

    परत bri;
}
#endif

/*
 * When extra == 0: Return TRUE if the cursor is before or on the first
 *		    non-blank in the line.
 * When extra == 1: Return TRUE if the cursor is before the first non-blank in
 *		    the line.
 */
    पूर्णांक
inindent(पूर्णांक extra)
{
    char_u	*ptr;
    colnr_T	col;

    साठी (col = 0, ptr = ml_get_curline(); VIM_ISWHITE(*ptr); ++col)
	++ptr;
    जर (col >= curwin->w_cursor.col + extra)
	परत TRUE;
    अन्यथा
	परत FALSE;
}

/*
 * op_reindent - handle reindenting a block of lines.
 */
    शून्य
op_reindent(oparg_T *oap, पूर्णांक (*how)(शून्य))
{
    दीर्घ	i = 0;
    char_u	*l;
    पूर्णांक		amount;
    linenr_T	first_changed = 0;
    linenr_T	last_changed = 0;
    linenr_T	start_lnum = curwin->w_cursor.lnum;

    // Don't even try when 'modifiable' is off.
    जर (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	परत;
    }

    // Save for undo.  Do this once for all lines, much faster than doing this
    // for each line separately, especially when undoing.
    जर (u_savecommon(start_lnum - 1, start_lnum + oap->ओळ_मोजणी,
				     start_lnum + oap->ओळ_मोजणी, FALSE) == OK)
	साठी (i = oap->ओळ_मोजणी; --i >= 0 && !got_int; )
	{
	    // it's a slow thing to do, so give feedback so there's no worry
	    // that the computer's just hung.

	    जर (i > 1
		    && (i % 50 == 0 || i == oap->ओळ_मोजणी - 1)
		    && oap->ओळ_मोजणी > p_report)
		smsg(_("%ld lines to indent... "), i);

	    // Be vi-compatible: For lisp indenting the first line is not
	    // indented, unless there is only one line.
	    जर (i != oap->ओळ_मोजणी - 1 || oap->ओळ_मोजणी == 1
						     || how != get_lisp_indent)
	    {
		l = skipwhite(ml_get_curline());
		जर (*l == NUL)		    // empty or blank line
		    amount = 0;
		अन्यथा
		    amount = how();	    // get the indent for this line

		जर (amount >= 0 && set_indent(amount, 0))
		{
		    // did change the indent, call changed_lines() later
		    जर (first_changed == 0)
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
    जर (last_changed != 0)
	changed_ओळी(first_changed, 0,
		oap->is_VIsual ? start_lnum + oap->ओळ_मोजणी :
		last_changed + 1, 0L);
    अन्यथा जर (oap->is_VIsual)
	redraw_curbuf_later(UPD_INVERTED);

    जर (oap->ओळ_मोजणी > p_report)
    {
	i = oap->ओळ_मोजणी - (i + 1);
	smsg(NGETTEXT("%ld line indented ",
						 "%ld lines indented ", i), i);
    }
    जर ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// set '[ and '] marks
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
    }
}

/*
 * Return TRUE if lines starting with '#' should be left aligned.
 */
    पूर्णांक
preprocs_डावा(शून्य)
{
    परत
	(curbuf->b_p_si && !curbuf->b_p_cin) ||
	(curbuf->b_p_cin && in_cinkeys('#', ' ', TRUE)
					   && curbuf->b_ind_hash_comment == 0)
	;
}

/*
 * Return TRUE if the conditions are OK for smart indenting.
 */
    पूर्णांक
may_do_si(शून्य)
{
    परत curbuf->b_p_si
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
ins_try_si(पूर्णांक c)
{
    pos_T	*pos, old_pos;
    char_u	*ptr;
    पूर्णांक		i;
    पूर्णांक		temp;

    // do some very smart indenting when entering '{' or '}'
    जर (((did_si || can_si_back) && c == '{')
	    || (can_si && c == '}' && inindent(0)))
    {
	// for '}' set indent equal to indent of line containing matching '{'
	जर (c == '}' && (pos = findmatch(रिक्त, '{')) != रिक्त)
	{
	    old_pos = curwin->w_cursor;
	    // If the matching '{' has a ')' immediately before it (ignoring
	    // white-space), then line up with the start of the line
	    // containing the matching '(' if there is one.  This handles the
	    // case where an "if (..\n..) {" statement continues over multiple
	    // lines -- webb
	    ptr = ml_get(pos->lnum);
	    i = pos->col;
	    जर (i > 0)		// skip blanks before '{'
		पर्यंत (--i > 0 && VIM_ISWHITE(ptr[i]))
		    ;
	    curwin->w_cursor.lnum = pos->lnum;
	    curwin->w_cursor.col = i;
	    जर (ptr[i] == ')' && (pos = findmatch(रिक्त, '(')) != रिक्त)
		curwin->w_cursor = *pos;
	    i = get_indent();
	    curwin->w_cursor = old_pos;
	    जर (State & VREPLACE_FLAG)
		change_indent(INDENT_SET, i, FALSE, NUL, TRUE);
	    अन्यथा
		(शून्य)set_indent(i, SIN_CHANGED);
	}
	अन्यथा जर (curwin->w_cursor.col > 0)
	{
	    // when inserting '{' after "O" reduce indent, but not
	    // more than indent of previous line
	    temp = TRUE;
	    जर (c == '{' && can_si_back && curwin->w_cursor.lnum > 1)
	    {
		old_pos = curwin->w_cursor;
		i = get_indent();
		पर्यंत (curwin->w_cursor.lnum > 1)
		{
		    ptr = skipwhite(ml_get(--(curwin->w_cursor.lnum)));

		    // ignore empty lines and lines starting with '#'.
		    जर (*ptr != '#' && *ptr != NUL)
			थांबा;
		}
		जर (get_indent() >= i)
		    temp = FALSE;
		curwin->w_cursor = old_pos;
	    }
	    जर (temp)
		shift_ओळ(TRUE, FALSE, 1, TRUE);
	}
    }

    // set indent of '#' always to 0
    जर (curwin->w_cursor.col > 0 && can_si && c == '#' && inindent(0))
    {
	// remember current indent for next line
	old_indent = get_indent();
	(शून्य)set_indent(0, SIN_CHANGED);
    }

    // Adjust ai_col, the char at this position can be deleted.
    जर (ai_col > curwin->w_cursor.col)
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
    पूर्णांक		type,
    पूर्णांक		amount,
    पूर्णांक		round,
    पूर्णांक		replaced,	// replaced character, put on replace stack
    पूर्णांक		call_changed_bytes)	// call changed_bytes()
{
    पूर्णांक		vcol;
    पूर्णांक		last_vcol;
    पूर्णांक		insstart_less;		// reduction for Insstart.col
    पूर्णांक		new_cursor_col;
    पूर्णांक		i;
    char_u	*ptr;
    पूर्णांक		save_p_list;
    पूर्णांक		start_col;
    colnr_T	vc;
    colnr_T	orig_col = 0;		// init for GCC
    char_u	*new_ओळ, *orig_ओळ = रिक्त;	// init for GCC

    // MODE_VREPLACE state needs to know what the line was like before changing
    जर (State & VREPLACE_FLAG)
    {
	orig_ओळ = vim_strnsave(ml_get_curline(), ml_get_curline_len());  // Deal with NULL below
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
    जर (new_cursor_col < 0)
	vcol = get_indent() - vcol;

    जर (new_cursor_col > 0)	    // can't fix replace stack
	start_col = -1;

    // Set the new indent.  The cursor will be put on the first non-blank.
    जर (type == INDENT_SET)
	(शून्य)set_indent(amount, call_changed_bytes ? SIN_CHANGED : 0);
    अन्यथा
    {
	पूर्णांक	save_State = State;

	// Avoid being called recursively.
	जर (State & VREPLACE_FLAG)
	    State = MODE_घाला;
	shift_ओळ(type == INDENT_DEC, round, 1, call_changed_bytes);
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
    जर (new_cursor_col >= 0)
    {
	// When changing the indent while the cursor is touching it, reset
	// Insstart_col to 0.
	जर (new_cursor_col == 0)
	    insstart_less = MAXCOL;
	new_cursor_col += curwin->w_cursor.col;
    }
    अन्यथा जर (!(State & MODE_घाला))
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
	पर्यंत (cts.cts_vcol <= (पूर्णांक)curwin->w_virtcol)
	{
	    last_vcol = cts.cts_vcol;
	    जर (cts.cts_vcol > 0)
		MB_PTR_ADV(cts.cts_ptr);
	    जर (*cts.cts_ptr == NUL)
		थांबा;
	    cts.cts_vcol += lbr_chartabsize(&cts);
	}
	vcol = last_vcol;
	new_cursor_col = cts.cts_ptr - cts.cts_ओळ;
	clear_chartabsize_arg(&cts);

	// May need to insert spaces to be able to position the cursor on
	// the right screen column.
	जर (vcol != (पूर्णांक)curwin->w_virtcol)
	{
	    curwin->w_cursor.col = (colnr_T)new_cursor_col;
	    i = (पूर्णांक)curwin->w_virtcol - vcol;
	    ptr = alloc(i + 1);
	    जर (ptr != रिक्त)
	    {
		माप_प्रकार ptrlen;
		new_cursor_col += i;
		ptr[i] = NUL;
		ptrlen = i;
		पर्यंत (--i >= 0)
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

    जर (new_cursor_col <= 0)
	curwin->w_cursor.col = 0;
    अन्यथा
	curwin->w_cursor.col = (colnr_T)new_cursor_col;
    curwin->w_set_curswant = true;
    changed_cline_bef_curs();

    // May have to adjust the start of the insert.
    जर (State & MODE_घाला)
    {
	जर (curwin->w_cursor.lnum == Insstart.lnum && Insstart.col != 0)
	{
	    जर ((पूर्णांक)Insstart.col <= insstart_less)
		Insstart.col = 0;
	    अन्यथा
		Insstart.col -= insstart_less;
	}
	जर ((पूर्णांक)ai_col <= insstart_less)
	    ai_col = 0;
	अन्यथा
	    ai_col -= insstart_less;
    }

    // For MODE_REPLACE state, may have to fix the replace stack, if it's
    // possible.  If the number of characters before the cursor decreased, need
    // to pop a few characters from the replace stack.
    // If the number of characters before the cursor increased, need to push a
    // few NULs onto the replace stack.
    जर (REPLACE_NORMAL(State) && start_col >= 0)
    {
	पर्यंत (start_col > (पूर्णांक)curwin->w_cursor.col)
	{
	    replace_join(0);	    // remove a NUL from the replace stack
	    --start_col;
	}
	पर्यंत (start_col < (पूर्णांक)curwin->w_cursor.col || replaced)
	{
	    replace_ढकला(NUL);
	    जर (replaced)
	    {
		replace_ढकला(replaced);
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
    जर (State & VREPLACE_FLAG)
    {
	// If orig_line didn't allocate, just return.  At least we did the job,
	// even if you can't backspace.
	जर (orig_ओळ == रिक्त)
	    परत;

	// Save new line
	new_ओळ = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	जर (new_ओळ == रिक्त)
	{
	    vim_free(orig_ओळ);
	    परत;
	}

	// We only put back the new line up to the cursor
	new_ओळ[curwin->w_cursor.col] = NUL;

	// Put back original line
	ml_replace(curwin->w_cursor.lnum, orig_ओळ, FALSE);
	curwin->w_cursor.col = orig_col;

	// Backspace from cursor to start of line
	backspace_until_column(0);

	// Insert new stuff into line again
	ins_bytes(new_ओळ);

	vim_free(new_ओळ);
    }
}

/*
 * Copy the indent from ptr to the current line (and fill to size)
 * Leaves the cursor on the first non-blank in the line.
 * Returns TRUE if the line was changed.
 */
    पूर्णांक
copy_indent(पूर्णांक size, char_u *src)
{
    char_u	*p = रिक्त;
    char_u	*ओळ = रिक्त;
    char_u	*s;
    पूर्णांक		todo;
    पूर्णांक		ind_len;
    पूर्णांक		ओळ_len = 0;
    पूर्णांक		tab_pad;
    पूर्णांक		ind_done;
    पूर्णांक		round;
#ifdef FEAT_VARTABS
    पूर्णांक		ind_col;
#endif

    // Round 1: compute the number of characters needed for the indent
    // Round 2: copy the characters.
    साठी (round = 1; round <= 2; ++round)
    {
	todo = size;
	ind_len = 0;
	ind_done = 0;
#ifdef FEAT_VARTABS
	ind_col = 0;
#endif
	s = src;

	// Count/copy the usable portion of the source line
	पर्यंत (todo > 0 && VIM_ISWHITE(*s))
	{
	    जर (*s == TAB)
	    {
#ifdef FEAT_VARTABS
		tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#अन्यथा
		tab_pad = (पूर्णांक)curbuf->b_p_ts
					   - (ind_done % (पूर्णांक)curbuf->b_p_ts);
#endif
		// Stop if this tab will overshoot the target
		जर (todo < tab_pad)
		    थांबा;
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
	    जर (p != रिक्त)
		*p++ = *s;
	    ++s;
	}

	// Fill to next tabstop with a tab, if possible
#ifdef FEAT_VARTABS
	tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#अन्यथा
	tab_pad = (पूर्णांक)curbuf->b_p_ts - (ind_done % (पूर्णांक)curbuf->b_p_ts);
#endif
	जर (todo >= tab_pad && !curbuf->b_p_et)
	{
	    todo -= tab_pad;
	    ++ind_len;
#ifdef FEAT_VARTABS
	    ind_col += tab_pad;
#endif
	    जर (p != रिक्त)
		*p++ = TAB;
	}

	// Add tabs required for indent
	जर (!curbuf->b_p_et)
	{
#ifdef FEAT_VARTABS
	    साठी (;;)
	    {
		tab_pad = tabstop_padding(ind_col, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
		जर (todo < tab_pad)
		    थांबा;
		todo -= tab_pad;
		++ind_len;
		ind_col += tab_pad;
		जर (p != रिक्त)
		    *p++ = TAB;
	    }
#अन्यथा
	    पर्यंत (todo >= (पूर्णांक)curbuf->b_p_ts)
	    {
		todo -= (पूर्णांक)curbuf->b_p_ts;
		++ind_len;
		जर (p != रिक्त)
		    *p++ = TAB;
	    }
#endif
	}

	// Count/add spaces required for indent
	पर्यंत (todo > 0)
	{
	    --todo;
	    ++ind_len;
	    जर (p != रिक्त)
		*p++ = ' ';
	}

	जर (p == रिक्त)
	{
	    // Allocate memory for the result: the copied indent, new indent
	    // and the rest of the line.
	    ओळ_len = ml_get_curline_len() + 1;
	    ओळ = alloc(ind_len + ओळ_len);
	    जर (ओळ == रिक्त)
		परत FALSE;
	    p = ओळ;
	}
    }

    // Append the original line
    mch_memmove(p, ml_get_curline(), (माप_प्रकार)ओळ_len);

    // Replace the line
    ml_replace(curwin->w_cursor.lnum, ओळ, FALSE);

    // Put the cursor after the indent.
    curwin->w_cursor.col = ind_len;
    परत TRUE;
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
    जर (trylevel == 0)
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
    पूर्णांक		got_tab = FALSE;
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
    char_u	*new_ओळ = (char_u *)1; // init to non-NULL
    पूर्णांक		did_undo;		// called u_save for current line
#ifdef FEAT_VARTABS
    पूर्णांक		*new_vts_array = रिक्त;
    char_u	*new_ts_str;		// string value of tab argument
#अन्यथा
    पूर्णांक		temp;
    पूर्णांक		new_ts = 0;
#endif
    पूर्णांक		save_list;
    linenr_T	first_ओळ = 0;		// first changed line
    linenr_T	last_ओळ = 0;		// last changed line
    पूर्णांक		is_indent_only = 0;	// Only process leading whitespace

    save_list = curwin->w_p_list;
    curwin->w_p_list = 0;	    // don't want list mode here

    ptr = eap->arg;
    जर (STRNCMP(ptr, "-indentonly", 11) == 0 && IS_WHITE_OR_NUL(ptr[11]))
    {
	is_indent_only = 1;
	ptr = skipwhite(ptr + 11);
    }

#ifdef FEAT_VARTABS
    new_ts_str = ptr;
    जर (tabstop_set(ptr, &new_vts_array) == FAIL)
	परत;
    पर्यंत (vim_isdigit(*ptr) || *ptr == ',')
	++ptr;

    // This ensures that either new_vts_array and new_ts_str are freshly
    // allocated, or new_vts_array points to an existing array and new_ts_str
    // is null.
    जर (new_vts_array == रिक्त)
    {
	new_vts_array = curbuf->b_p_vts_array;
	new_ts_str = रिक्त;
    }
    अन्यथा
	new_ts_str = vim_strnsave(new_ts_str, ptr - new_ts_str);
#अन्यथा
    जर (ptr[0] != NUL && (ptr[0] != '0' || ptr[1] != NUL))
    {
	char_u	*end;

	जर (म_से_दीर्घ((अक्षर *)ptr, (अक्षर **)&end, 10) <= 0)
	{
	    जर (ptr != end)
		emsg(_(e_argument_must_be_positive));
	    अन्यथा
		semsg(_(e_invalid_argument_str), ptr);
	    परत;
	}
	new_ts = getdigits(&ptr);
	जर (new_ts < 0 || new_ts > TABSTOP_MAX)
	{
	    semsg(_(e_invalid_argument_str), eap->arg);
	    परत;
	}
    }
    जर (new_ts == 0)
	new_ts = curbuf->b_p_ts;
#endif
    साठी (lnum = eap->line1; !got_int && lnum <= eap->line2; ++lnum)
    {
	ptr = ml_get(lnum);
	old_len = ml_get_len(lnum);
	col = 0;
	vcol = 0;
	did_undo = FALSE;
	साठी (;;)
	{
	    जर (VIM_ISWHITE(ptr[col]))
	    {
		जर (!got_tab && अंक_spaces == 0)
		{
		    // First consecutive white-space
		    start_vcol = vcol;
		    start_col = col;
		}
		जर (ptr[col] == ' ')
		    अंक_spaces++;
		अन्यथा
		    got_tab = TRUE;
	    }
	    अन्यथा
	    {
		जर (got_tab || (eap->forceit && अंक_spaces > 1))
		{
		    // Retabulate this string of white-space

		    // len is virtual length of white string
		    len = अंक_spaces = vcol - start_vcol;
		    अंक_tabs = 0;
		    जर (!curbuf->b_p_et)
		    {
#ifdef FEAT_VARTABS
			पूर्णांक t, s;

			tabstop_fromto(start_vcol, vcol,
					curbuf->b_p_ts, new_vts_array, &t, &s);
			अंक_tabs = t;
			अंक_spaces = s;
#अन्यथा
			temp = new_ts - (start_vcol % new_ts);
			जर (अंक_spaces >= temp)
			{
			    अंक_spaces -= temp;
			    अंक_tabs++;
			}
			अंक_tabs += अंक_spaces / new_ts;
			अंक_spaces -= (अंक_spaces / new_ts) * new_ts;
#endif
		    }
		    जर (curbuf->b_p_et || got_tab ||
					(अंक_spaces + अंक_tabs < len))
		    {
			जर (did_undo == FALSE)
			{
			    did_undo = TRUE;
			    जर (u_save((linenr_T)(lnum - 1),
						(linenr_T)(lnum + 1)) == FAIL)
			    {
				new_ओळ = रिक्त;	// flag out-of-memory
				थांबा;
			    }
			}

			// len is actual number of white characters used
			len = अंक_spaces + अंक_tabs;
			new_len = old_len - col + start_col + len + 1;
			जर (new_len <= 0 || new_len >= MAXCOL)
			{
			    emsg_text_too_long();
			    थांबा;
			}
			new_ओळ = alloc(new_len);
			जर (new_ओळ == रिक्त)
			    थांबा;
			जर (start_col > 0)
			    mch_memmove(new_ओळ, ptr, (माप_प्रकार)start_col);
			mch_memmove(new_ओळ + start_col + len,
				      ptr + col, (माप_प्रकार)(old_len - col + 1));
			ptr = new_ओळ + start_col;
			साठी (col = 0; col < len; col++)
			    ptr[col] = (col < अंक_tabs) ? '\t' : ' ';
			जर (ml_replace(lnum, new_ओळ, FALSE) == OK)
			    // "new_line" may have been copied
			    new_ओळ = curbuf->b_ml.ml_ओळ_ptr;
			जर (first_ओळ == 0)
			    first_ओळ = lnum;
			last_ओळ = lnum;
			ptr = new_ओळ;
			old_len = new_len - 1;
			col = start_col + len;
		    }
		}
		got_tab = FALSE;
		अंक_spaces = 0;

		जर (is_indent_only)
		    थांबा;
	    }
	    जर (ptr[col] == NUL)
		थांबा;
	    vcol += chartabsize(ptr + col, (colnr_T)vcol);
	    जर (vcol >= MAXCOL)
	    {
		emsg_text_too_long();
		थांबा;
	    }
	    जर (has_mbyte)
		col += (*mb_ptr2len)(ptr + col);
	    अन्यथा
		++col;
	}
	जर (new_ओळ == रिक्त)		    // out of memory
	    थांबा;
	ओळ_breakcheck();
    }
    जर (got_int)
	emsg(_(e_interrupted));

#ifdef FEAT_VARTABS
    // If a single value was given then it can be considered equal to
    // either the value of 'tabstop' or the value of 'vartabstop'.
    जर (tabstop_मोजणी(curbuf->b_p_vts_array) == 0
	&& tabstop_मोजणी(new_vts_array) == 1
	&& curbuf->b_p_ts == tabstop_first(new_vts_array))
	; // not changed
    अन्यथा जर (tabstop_मोजणी(curbuf->b_p_vts_array) > 0
	&& tabstop_eq(curbuf->b_p_vts_array, new_vts_array))
	; // not changed
    अन्यथा
	redraw_curbuf_later(UPD_NOT_VALID);
#अन्यथा
    जर (curbuf->b_p_ts != new_ts)
	redraw_curbuf_later(UPD_NOT_VALID);
#endif
    जर (first_ओळ != 0)
	changed_ओळी(first_ओळ, 0, last_ओळ + 1, 0L);

    curwin->w_p_list = save_list;	// restore 'list'

#ifdef FEAT_VARTABS
    जर (new_ts_str != रिक्त)		// set the new tabstop
    {
	// If 'vartabstop' is in use or if the value given to retab has more
	// than one tabstop then update 'vartabstop'.
	पूर्णांक *old_vts_ary = curbuf->b_p_vts_array;

	जर (tabstop_मोजणी(old_vts_ary) > 0 || tabstop_मोजणी(new_vts_array) > 1)
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

#जर defined(FEAT_EVAL)
/*
 * Get indent level from 'indentexpr'.
 */
    पूर्णांक
get_अभिव्यक्ती_indent(शून्य)
{
    पूर्णांक		indent = -1;
    char_u	*inde_copy;
    pos_T	save_pos;
    colnr_T	save_curswant;
    पूर्णांक		save_set_curswant;
    पूर्णांक		save_State;
    पूर्णांक		use_sandbox = was_set_insecurely(curwin,
					    (char_u *)"indentexpr", OPT_LOCAL);
    sctx_T	save_sctx = current_sctx;

    // Save and restore cursor position and curswant, in case it was changed
    // via :normal commands
    save_pos = curwin->w_cursor;
    save_curswant = curwin->w_curswant;
    save_set_curswant = curwin->w_set_curswant;
    set_vim_var_nr(VV_LNUM, curwin->w_cursor.lnum);
    जर (use_sandbox)
	++sandbox;
    ++textlock;
    current_sctx = curbuf->b_p_script_ctx[BV_INDE];

    // Need to make a copy, the 'indentexpr' option could be changed while
    // evaluating it.
    inde_copy = vim_strsave(curbuf->b_p_inde);
    जर (inde_copy != रिक्त)
    {
	indent = (पूर्णांक)eval_to_संख्या(inde_copy, TRUE);
	vim_free(inde_copy);
    }

    जर (use_sandbox)
	--sandbox;
    --textlock;
    current_sctx = save_sctx;

    // Restore the cursor position so that 'indentexpr' doesn't need to.
    // Pretend to be in Insert mode, allow cursor past end of line for "o"
    // command.
    save_State = State;
    State = MODE_घाला;
    curwin->w_cursor = save_pos;
    curwin->w_curswant = save_curswant;
    curwin->w_set_curswant = save_set_curswant;
    check_cursor();
    State = save_State;

    // Reset did_throw, unless 'debug' has "throw" and inside a try/catch.
    जर (did_throw && (vim_strchr(p_debug, 't') == रिक्त || trylevel == 0))
    {
	handle_did_throw();
	did_throw = FALSE;
    }

    // If there is an error, just keep the current indent.
    जर (indent < 0)
	indent = get_indent();

    परत indent;
}
#endif

    स्थिर पूर्णांक
lisp_match(char_u *p)
{
    char_u	buf[LSIZE];
    पूर्णांक		len;
    char_u	*word = *curbuf->b_p_lw != NUL ? curbuf->b_p_lw : p_lispwords;

    पर्यंत (*word != NUL)
    {
	len = copy_option_part(&word, buf, LSIZE, ",");
	जर (STRNCMP(buf, p, len) == 0 && IS_WHITE_OR_NUL(p[len]))
	    परत TRUE;
    }
    परत FALSE;
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
    पूर्णांक
get_lisp_indent(शून्य)
{
    pos_T	*pos, realpos, paren;
    पूर्णांक		amount;
    char_u	*that;
    colnr_T	col;
    colnr_T	firsttry;
    पूर्णांक		parencount, quotecount;
    पूर्णांक		vi_lisp;

    // Set vi_lisp to use the vi-compatible method
    vi_lisp = (vim_strchr(p_cpo, CPO_LISP) != रिक्त);

    realpos = curwin->w_cursor;
    curwin->w_cursor.col = 0;

    जर ((pos = findmatch(रिक्त, '(')) == रिक्त)
	pos = findmatch(रिक्त, '[');
    अन्यथा
    {
	paren = *pos;
	pos = findmatch(रिक्त, '[');
	जर (pos == रिक्त || LT_POSP(pos, &paren))
	    pos = &paren;
    }
    जर (pos != रिक्त)
    {
	// Extra trick: Take the indent of the first previous non-white
	// line that is at the same () level.
	amount = -1;
	parencount = 0;

	पर्यंत (--curwin->w_cursor.lnum >= pos->lnum)
	{
	    जर (linewhite(curwin->w_cursor.lnum))
		सुरूठेवा;
	    साठी (that = ml_get_curline(); *that != NUL; ++that)
	    {
		जर (*that == ';')
		{
		    पर्यंत (*(that + 1) != NUL)
			++that;
		    सुरूठेवा;
		}
		जर (*that == '\\')
		{
		    जर (*(that + 1) != NUL)
			++that;
		    सुरूठेवा;
		}
		जर (*that == '"' && *(that + 1) != NUL)
		{
		    पर्यंत (*++that && *that != '"')
		    {
			// skipping escaped characters in the string
			जर (*that == '\\')
			{
			    जर (*++that == NUL)
				थांबा;
			    जर (that[1] == NUL)
			    {
				++that;
				थांबा;
			    }
			}
		    }
		    जर (*that == NUL)
			थांबा;
		}
		जर (*that == '(' || *that == '[')
		    ++parencount;
		अन्यथा जर (*that == ')' || *that == ']')
		    --parencount;
	    }
	    जर (parencount == 0)
	    {
		amount = get_indent();
		थांबा;
	    }
	}

	जर (amount == -1)
	{
	    curwin->w_cursor.lnum = pos->lnum;
	    curwin->w_cursor.col = pos->col;
	    col = pos->col;

	    that = ml_get_curline();

	    जर (vi_lisp && get_indent() == 0)
		amount = 2;
	    अन्यथा
	    {
		char_u		*ओळ = that;
		chartabsize_T	cts;

		init_chartabsize_arg(&cts, curwin, pos->lnum, 0, ओळ, ओळ);
		पर्यंत (*cts.cts_ptr != NUL && col > 0)
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

		जर (!vi_lisp && (*that == '(' || *that == '[')
						      && lisp_match(that + 1))
		    amount += 2;
		अन्यथा
		{
		    जर (*that != NUL)
		    {
			that++;
			amount++;
		    }
		    firsttry = amount;

		    init_chartabsize_arg(&cts, curwin, (colnr_T)(that - ओळ),
							   amount, ओळ, that);
		    पर्यंत (VIM_ISWHITE(*cts.cts_ptr))
		    {
			cts.cts_vcol += lbr_chartabsize(&cts);
			++cts.cts_ptr;
		    }
		    that = cts.cts_ptr;
		    amount = cts.cts_vcol;
		    clear_chartabsize_arg(&cts);

		    जर (*that && *that != ';') // not a comment line
		    {
			// test *that != '(' to accommodate first let/do
			// argument if it is more than one line
			जर (!vi_lisp && *that != '(' && *that != '[')
			    firsttry++;

			parencount = 0;
			quotecount = 0;

			init_chartabsize_arg(&cts, curwin,
				   (colnr_T)(that - ओळ), amount, ओळ, that);
			जर (vi_lisp
				|| (*that != '"'
				    && *that != '\''
				    && *that != '#'
				    && (*that < '0' || *that > '9')))
			{
			    पर्यंत (*cts.cts_ptr
				    && (!VIM_ISWHITE(*cts.cts_ptr)
					|| quotecount
					|| parencount)
				    && (!((*cts.cts_ptr == '('
							|| *cts.cts_ptr == '[')
					    && !quotecount
					    && !parencount
					    && vi_lisp)))
			    {
				जर (*cts.cts_ptr == '"')
				    quotecount = !quotecount;
				जर ((*cts.cts_ptr == '(' || *cts.cts_ptr == '[')
							       && !quotecount)
				    ++parencount;
				जर ((*cts.cts_ptr == ')' || *cts.cts_ptr == ']')
							       && !quotecount)
				    --parencount;
				जर (*cts.cts_ptr == '\\'
						    && *(cts.cts_ptr+1) != NUL)
				    cts.cts_vcol += lbr_chartabsize_adv(&cts);
				cts.cts_vcol += lbr_chartabsize_adv(&cts);
			    }
			}
			पर्यंत (VIM_ISWHITE(*cts.cts_ptr))
			{
			    cts.cts_vcol += lbr_chartabsize(&cts);
			    ++cts.cts_ptr;
			}
			that = cts.cts_ptr;
			amount = cts.cts_vcol;
			clear_chartabsize_arg(&cts);

			जर (!*that || *that == ';')
			    amount = firsttry;
		    }
		}
	    }
	}
    }
    अन्यथा
	amount = 0;	// no matching '(' or '[' found, use zero indent

    curwin->w_cursor = realpos;

    परत amount;
}

/*
 * Re-indent the current line, based on the current contents of it and the
 * surrounding lines. Fixing the cursor position seems really easy -- I'm very
 * confused what all the part that handles Control-T is doing that I'm not.
 * "get_the_indent" should be get_c_indent, get_expr_indent or get_lisp_indent.
 */

    शून्य
fixthisline(पूर्णांक (*get_the_indent)(शून्य))
{
    पूर्णांक amount = get_the_indent();

    जर (amount < 0)
	परत;

    change_indent(INDENT_SET, amount, FALSE, 0, TRUE);
    जर (linewhite(curwin->w_cursor.lnum))
	did_ai = TRUE;	// delete the indent if the line stays empty
}

/*
 * Return TRUE if 'indentexpr' should be used for Lisp indenting.
 * Caller may want to check 'autoindent'.
 */
    पूर्णांक
use_indentexpr_for_lisp(शून्य)
{
#ifdef FEAT_EVAL
    परत curbuf->b_p_lisp
		&& *curbuf->b_p_inde != NUL
		&& STRCMP(curbuf->b_p_lop, "expr:1") == 0;
#अन्यथा
    परत FALSE;
#endif
}

/*
 * Fix indent for 'lisp' and 'cindent'.
 */
    शून्य
fix_indent(शून्य)
{
    जर (p_paste)
	परत;  // no auto-indenting when 'paste' is set
    जर (curbuf->b_p_lisp && curbuf->b_p_ai)
    {
	जर (use_indentexpr_for_lisp())
	    do_c_अभिव्यक्ती_indent();
	अन्यथा
	    fixthisline(get_lisp_indent);
    }
    अन्यथा जर (cindent_on())
	do_c_अभिव्यक्ती_indent();
}

#जर defined(FEAT_EVAL)
/*
 * "indent()" function
 */
    शून्य
f_indent(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    जर (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	परत;

    lnum = tv_get_lnum(argvars);
    जर (lnum >= 1 && lnum <= curbuf->b_ml.ml_ओळ_मोजणी)
	rettv->vval.v_संख्या = get_indent_lnum(lnum);
    अन्यथा
    {
	जर (in_vim9script())
	    semsg(_(e_invalid_ओळ_संख्या_nr), lnum);
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

    जर (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	परत;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    जर (lnum >= 1 && lnum <= curbuf->b_ml.ml_ओळ_मोजणी)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_संख्या = get_lisp_indent();
	curwin->w_cursor = pos;
    }
    अन्यथा जर (in_vim9script())
	semsg(_(e_invalid_ओळ_संख्या_nr), lnum);
    अन्यथा
	rettv->vval.v_संख्या = -1;
}
#endif
