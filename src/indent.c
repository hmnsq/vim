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

#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_VARTABS)

/*
 * Set the integer values corresponding to the string setting of 'vartabstop'.
 * "array" will be set, caller must free it if needed.
 * Return FAIL for an error.
 */
    পূর্ণাঙ্ক
tabstop_set(char_u *var, পূর্ণাঙ্ক **array)
{
    পূর্ণাঙ্ক	    valcount = 1;
    পূর্ণাঙ্ক	    t;
    char_u  *cp;

    যদি (var[0] == NUL || (var[0] == '0' && var[1] == NUL))
    {
	*array = রিক্ত;
	প্রত্যাগম OK;
    }

    প্রতি (cp = var; *cp != NUL; ++cp)
    {
	যদি (cp == var || cp[-1] == ',')
	{
	    char_u *end;

	    যদি (strtol((বর্ণ *)cp, (বর্ণ **)&end, 10) <= 0)
	    {
		যদি (cp != end)
		    emsg(_(e_argument_must_be_positive));
		অন্যথা
		    semsg(_(e_invalid_argument_str), cp);
		প্রত্যাগম FAIL;
	    }
	}

	যদি (VIM_ISDIGIT(*cp))
	    অনুবর্ত;
	যদি (cp[0] == ',' && cp > var && cp[-1] != ',' && cp[1] != NUL)
	{
	    ++valcount;
	    অনুবর্ত;
	}
	semsg(_(e_invalid_argument_str), var);
	প্রত্যাগম FAIL;
    }

    *array = ALLOC_MULT(পূর্ণাঙ্ক, valcount + 1);
    যদি (*array == রিক্ত)
	প্রত্যাগম FAIL;
    (*array)[0] = valcount;

    t = 1;
    প্রতি (cp = var; *cp != NUL;)
    {
	পূর্ণাঙ্ক n = atoi((বর্ণ *)cp);

	// Catch negative values, overflow and ridiculous big values.
	যদি (n <= 0 || n > TABSTOP_MAX)
	{
	    semsg(_(e_invalid_argument_str), cp);
	    VIM_CLEAR(*array);
	    প্রত্যাগম FAIL;
	}
	(*array)[t++] = n;
	যাবত্ (*cp != NUL && *cp != ',')
	    ++cp;
	যদি (*cp != NUL)
	    ++cp;
    }

    প্রত্যাগম OK;
}

/*
 * Calculate the number of screen spaces a tab will occupy.
 * If "vts" is set then the tab widths are taken from that array,
 * otherwise the value of ts is used.
 */
    পূর্ণাঙ্ক
tabstop_padding(colnr_T col, পূর্ণাঙ্ক ts_arg, পূর্ণাঙ্ক *vts)
{
    পূর্ণাঙ্ক		ts = ts_arg == 0 ? 8 : ts_arg;
    পূর্ণাঙ্ক		tabcount;
    colnr_T	tabcol = 0;
    পূর্ণাঙ্ক		t;
    পূর্ণাঙ্ক		padding = 0;

    যদি (vts == রিক্ত || vts[0] == 0)
	প্রত্যাগম ts - (col % ts);

    tabcount = vts[0];

    প্রতি (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	যদি (tabcol > col)
	{
	    padding = (পূর্ণাঙ্ক)(tabcol - col);
	    ভঙ্গ;
	}
    }
    যদি (t > tabcount)
	padding = vts[tabcount] - (পূর্ণাঙ্ক)((col - tabcol) % vts[tabcount]);

    প্রত্যাগম padding;
}

/*
 * Find the size of the tab that covers a particular column.
 *
 * If this is being called as part of a shift operation, col is not the cursor
 * column but is the column number to the left of the first non-whitespace
 * character in the line.  If the shift is to the left (left = TRUE), then
 * return the size of the tab interval to the left of the column.
 */
    পূর্ণাঙ্ক
tabstop_at(colnr_T col, পূর্ণাঙ্ক ts, পূর্ণাঙ্ক *vts, পূর্ণাঙ্ক বাম)
{
    পূর্ণাঙ্ক		tabcount;	// Number of tab stops in the list of variable
				// tab stops.
    colnr_T	tabcol = 0;	// Column of the tab stop under consideration.
    পূর্ণাঙ্ক		t;		// Tabstop index in the list of variable tab
				// stops.
    পূর্ণাঙ্ক		tab_size = 0;	// Size of the tab stop interval to the right
				// or left of the col.

    যদি (vts == 0 || vts[0] == 0)
	প্রত্যাগম ts;

    tabcount = vts[0];
    প্রতি (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	যদি (tabcol > col)
	{
	    // If shifting left (left != 0), and if the column to the left of
	    // the first first non-blank character (col) in the line is
	    // already to the left of the first tabstop, set the shift amount
	    // (tab_size) to just enough to shift the line to the left margin.
	    // The value doesn't seem to matter as long as it is at least that
	    // distance.
	    যদি (বাম && (t == 1))
		tab_size = col;
	    অন্যথা
		tab_size = vts[t - (বাম ? 1 : 0)];
	    ভঙ্গ;
	}
    }
    যদি (t > tabcount)		// If the value of the index t is beyond the
				// end of the list, use the tab stop value at
				// the end of the list.
	tab_size = vts[tabcount];

    প্রত্যাগম tab_size;
}

/*
 * Find the column on which a tab starts.
 */
    colnr_T
tabstop_start(colnr_T col, পূর্ণাঙ্ক ts, পূর্ণাঙ্ক *vts)
{
    পূর্ণাঙ্ক		tabcount;
    colnr_T	tabcol = 0;
    পূর্ণাঙ্ক		t;
    পূর্ণাঙ্ক		excess;

    যদি (vts == রিক্ত || vts[0] == 0)
	প্রত্যাগম col - col % ts;

    tabcount = vts[0];
    প্রতি (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	যদি (tabcol > col)
	    প্রত্যাগম tabcol - vts[t];
    }

    excess = tabcol % vts[tabcount];
    প্রত্যাগম col - (col - excess) % vts[tabcount];
}

/*
 * Find the number of tabs and spaces necessary to get from one column
 * to another.
 */
    শূন্য
tabstop_fromto(
	colnr_T start_col,
	colnr_T end_col,
	পূর্ণাঙ্ক	ts_arg,
	পূর্ণাঙ্ক	*vts,
	পূর্ণাঙ্ক	*ntabs,
	পূর্ণাঙ্ক	*nspcs)
{
    পূর্ণাঙ্ক		spaces = end_col - start_col;
    colnr_T	tabcol = 0;
    পূর্ণাঙ্ক		padding = 0;
    পূর্ণাঙ্ক		tabcount;
    পূর্ণাঙ্ক		t;
    পূর্ণাঙ্ক		ts = ts_arg == 0 ? curbuf->b_p_ts : ts_arg;

    যদি (vts == রিক্ত || vts[0] == 0)
    {
	পূর্ণাঙ্ক tabs = 0;
	পূর্ণাঙ্ক initspc = 0;

	initspc = ts - (start_col % ts);
	যদি (spaces >= initspc)
	{
	    spaces -= initspc;
	    tabs++;
	}
	tabs += spaces / ts;
	spaces -= (spaces / ts) * ts;

	*ntabs = tabs;
	*nspcs = spaces;
	প্রত্যাগম;
    }

    // Find the padding needed to reach the next tabstop.
    tabcount = vts[0];
    প্রতি (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	যদি (tabcol > start_col)
	{
	    padding = (পূর্ণাঙ্ক)(tabcol - start_col);
	    ভঙ্গ;
	}
    }
    যদি (t > tabcount)
	padding = vts[tabcount] - (পূর্ণাঙ্ক)((start_col - tabcol) % vts[tabcount]);

    // If the space needed is less than the padding no tabs can be used.
    যদি (spaces < padding)
    {
	*ntabs = 0;
	*nspcs = spaces;
	প্রত্যাগম;
    }

    *ntabs = 1;
    spaces -= padding;

    // At least one tab has been used. See if any more will fit.
    যাবত্ (spaces != 0 && ++t <= tabcount)
    {
	padding = vts[t];
	যদি (spaces < padding)
	{
	    *nspcs = spaces;
	    প্রত্যাগম;
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
    স্থির পূর্ণাঙ্ক
tabstop_eq(পূর্ণাঙ্ক *ts1, পূর্ণাঙ্ক *ts2)
{
    পূর্ণাঙ্ক		t;

    যদি ((ts1 == 0 && ts2) || (ts1 && ts2 == 0))
	প্রত্যাগম FALSE;
    যদি (ts1 == ts2)
	প্রত্যাগম TRUE;
    যদি (ts1[0] != ts2[0])
	প্রত্যাগম FALSE;

    প্রতি (t = 1; t <= ts1[0]; ++t)
	যদি (ts1[t] != ts2[t])
	    প্রত্যাগম FALSE;

    প্রত্যাগম TRUE;
}

# যদি defined(FEAT_BEVAL)
/*
 * Copy a tabstop array, allocating space for the new array.
 */
    পূর্ণাঙ্ক *
tabstop_copy(পূর্ণাঙ্ক *oldts)
{
    পূর্ণাঙ্ক		*newts;
    পূর্ণাঙ্ক		t;

    যদি (oldts == রিক্ত)
	প্রত্যাগম রিক্ত;
    newts = ALLOC_MULT(পূর্ণাঙ্ক, oldts[0] + 1);
    যদি (newts != রিক্ত)
	প্রতি (t = 0; t <= oldts[0]; ++t)
	    newts[t] = oldts[t];
    প্রত্যাগম newts;
}
# endif

/*
 * Return a count of the number of tabstops.
 */
    পূর্ণাঙ্ক
tabstop_গুনতি(পূর্ণাঙ্ক *ts)
{
    প্রত্যাগম ts != রিক্ত ? ts[0] : 0;
}

/*
 * Return the first tabstop, or 8 if there are no tabstops defined.
 */
    পূর্ণাঙ্ক
tabstop_first(পূর্ণাঙ্ক *ts)
{
    প্রত্যাগম ts != রিক্ত ? ts[1] : 8;
}

#endif

/*
 * Return the effective shiftwidth value for current buffer, using the
 * 'tabstop' value when 'shiftwidth' is zero.
 */
    long
get_sw_মান(buf_T *buf)
{
    প্রত্যাগম get_sw_মান_col(buf, 0, FALSE);
}

/*
 * Idem, using "pos".
 */
    স্থির long
get_sw_মান_pos(buf_T *buf, pos_T *pos, পূর্ণাঙ্ক বাম)
{
    pos_T save_cursor = curwin->w_cursor;
    long sw_মান;

    curwin->w_cursor = *pos;
    sw_মান = get_sw_মান_col(buf, get_nolist_virtcol(), বাম);
    curwin->w_cursor = save_cursor;
    প্রত্যাগম sw_মান;
}

/*
 * Idem, using the first non-black in the current line.
 */
    long
get_sw_মান_indent(buf_T *buf, পূর্ণাঙ্ক বাম)
{
    pos_T pos = curwin->w_cursor;

    pos.col = getwhitecols_curline();
    প্রত্যাগম get_sw_মান_pos(buf, &pos, বাম);
}

/*
 * Idem, using virtual column "col".
 */
    long
get_sw_মান_col(buf_T *buf, colnr_T col UNUSED, পূর্ণাঙ্ক বাম UNUSED)
{
    প্রত্যাগম buf->b_p_sw ? buf->b_p_sw :
#ifdef FEAT_VARTABS
	tabstop_at(col, buf->b_p_ts, buf->b_p_vts_array, বাম);
#অন্যথা
	buf->b_p_ts;
#endif
}

/*
 * Return the effective softtabstop value for the current buffer, using the
 * 'shiftwidth' value when 'softtabstop' is negative.
 */
    long
get_sts_মান(শূন্য)
{
    প্রত্যাগম curbuf->b_p_sts < 0 ? get_sw_মান(curbuf) : curbuf->b_p_sts;
}

/*
 * Count the size (in window cells) of the indent in the current line.
 */
    পূর্ণাঙ্ক
get_indent(শূন্য)
{
#ifdef FEAT_VARTABS
    প্রত্যাগম get_indent_str_vtab(ml_get_curline(), (পূর্ণাঙ্ক)curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#অন্যথা
    প্রত্যাগম get_indent_str(ml_get_curline(), (পূর্ণাঙ্ক)curbuf->b_p_ts, FALSE);
#endif
}

/*
 * Count the size (in window cells) of the indent in line "lnum".
 */
    পূর্ণাঙ্ক
get_indent_lnum(linenr_T lnum)
{
#ifdef FEAT_VARTABS
    প্রত্যাগম get_indent_str_vtab(ml_get(lnum), (পূর্ণাঙ্ক)curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#অন্যথা
    প্রত্যাগম get_indent_str(ml_get(lnum), (পূর্ণাঙ্ক)curbuf->b_p_ts, FALSE);
#endif
}

#যদি defined(FEAT_FOLDING)
/*
 * Count the size (in window cells) of the indent in line "lnum" of buffer
 * "buf".
 */
    পূর্ণাঙ্ক
get_indent_buf(buf_T *buf, linenr_T lnum)
{
# ifdef FEAT_VARTABS
    প্রত্যাগম get_indent_str_vtab(ml_get_buf(buf, lnum, FALSE),
			       (পূর্ণাঙ্ক)buf->b_p_ts, buf->b_p_vts_array, FALSE);
# অন্যথা
    প্রত্যাগম get_indent_str(ml_get_buf(buf, lnum, FALSE), (পূর্ণাঙ্ক)buf->b_p_ts, FALSE);
# endif
}
#endif

/*
 * count the size (in window cells) of the indent in line "ptr", with
 * 'tabstop' at "ts"
 */
    পূর্ণাঙ্ক
get_indent_str(
    char_u	*ptr,
    পূর্ণাঙ্ক		ts,
    পূর্ণাঙ্ক		no_ts) // if TRUE, count a tab as ^I
{
    পূর্ণাঙ্ক		গুনতি = 0;

    প্রতি ( ; *ptr; ++ptr)
    {
	যদি (*ptr == TAB)    // count a tab for what it is worth
	{
	    যদি (!no_ts)
		গুনতি += ts - (গুনতি % ts);
	    অন্যথা
		গুনতি += ptr2cells(ptr);
	}
	অন্যথা যদি (*ptr == ' ')
	    ++গুনতি;		// count a space for one
	অন্যথা
	    ভঙ্গ;
    }
    প্রত্যাগম গুনতি;
}

#যদি defined(FEAT_VARTABS)
/*
 * Count the size (in window cells) of the indent in line "ptr", using
 * variable tabstops.
 * If "no_ts" is TRUE, count a tab as ^I.
 */
    পূর্ণাঙ্ক
get_indent_str_vtab(char_u *ptr, পূর্ণাঙ্ক ts, পূর্ণাঙ্ক *vts, পূর্ণাঙ্ক no_ts)
{
    পূর্ণাঙ্ক		গুনতি = 0;

    প্রতি ( ; *ptr; ++ptr)
    {
	যদি (*ptr == TAB)    // count a tab for what it is worth
	{
	    যদি (!no_ts)
		গুনতি += tabstop_padding(গুনতি, ts, vts);
	    অন্যথা
		গুনতি += ptr2cells(ptr);
	}
	অন্যথা যদি (*ptr == ' ')
	    ++গুনতি;		// count a space for one
	অন্যথা
	    ভঙ্গ;
    }
    প্রত্যাগম গুনতি;
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
    পূর্ণাঙ্ক
set_indent(
    পূর্ণাঙ্ক		size,		    // measured in spaces
    পূর্ণাঙ্ক		flags)
{
    char_u	*p;
    char_u	*newline;
    char_u	*oldline;
    char_u	*s;
    পূর্ণাঙ্ক		todo;
    পূর্ণাঙ্ক		ind_len;	    // measured in characters
    পূর্ণাঙ্ক		সারি_len;	    // size of the line (including the NUL)
    পূর্ণাঙ্ক		doit = FALSE;
    পূর্ণাঙ্ক		ind_done = 0;	    // measured in spaces
#ifdef FEAT_VARTABS
    পূর্ণাঙ্ক		ind_col = 0;
#endif
    পূর্ণাঙ্ক		tab_pad;
    পূর্ণাঙ্ক		retval = FALSE;
    পূর্ণাঙ্ক		orig_char_len = -1; // number of initial whitespace chars when
				    // 'et' and 'pi' are both set

    // First check if there is anything to do and compute the number of
    // characters needed for the indent.
    todo = size;
    ind_len = 0;
    p = oldline = ml_get_curline();
    সারি_len = ml_get_curline_len() + 1;

    // Calculate the buffer size for the new indent, and check to see if it
    // isn't already set

    // if 'expandtab' isn't set: use TABs; if both 'expandtab' and
    // 'preserveindent' are set count the number of characters at the
    // beginning of the line to be copied
    যদি (!curbuf->b_p_et || (!(flags & SIN_ঢোকাও) && curbuf->b_p_pi))
    {
	// If 'preserveindent' is set then reuse as much as possible of
	// the existing indent structure for the new indent
	যদি (!(flags & SIN_ঢোকাও) && curbuf->b_p_pi)
	{
	    ind_done = 0;

	    // count as many characters as we can use
	    যাবত্ (todo > 0 && VIM_ISWHITE(*p))
	    {
		যদি (*p == TAB)
		{
#ifdef FEAT_VARTABS
		    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#অন্যথা
		    tab_pad = (পূর্ণাঙ্ক)curbuf->b_p_ts
					   - (ind_done % (পূর্ণাঙ্ক)curbuf->b_p_ts);
#endif
		    // stop if this tab will overshoot the target
		    যদি (todo < tab_pad)
			ভঙ্গ;
		    todo -= tab_pad;
		    ++ind_len;
		    ind_done += tab_pad;
		}
		অন্যথা
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
	    যদি (curbuf->b_p_et)
		orig_char_len = ind_len;

	    // Fill to next tabstop with a tab, if possible
#ifdef FEAT_VARTABS
	    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
						curbuf->b_p_vts_array);
#অন্যথা
	    tab_pad = (পূর্ণাঙ্ক)curbuf->b_p_ts - (ind_done % (পূর্ণাঙ্ক)curbuf->b_p_ts);
#endif
	    যদি (todo >= tab_pad && orig_char_len == -1)
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
	প্রতি (;;)
	{
	    tab_pad = tabstop_padding(ind_col, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
	    যদি (todo < tab_pad)
		ভঙ্গ;
	    যদি (*p != TAB)
		doit = TRUE;
	    অন্যথা
		++p;
	    todo -= tab_pad;
	    ++ind_len;
	    ind_col += tab_pad;
	}
#অন্যথা
	যাবত্ (todo >= (পূর্ণাঙ্ক)curbuf->b_p_ts)
	{
	    যদি (*p != TAB)
		doit = TRUE;
	    অন্যথা
		++p;
	    todo -= (পূর্ণাঙ্ক)curbuf->b_p_ts;
	    ++ind_len;
	    // ind_done += (int)curbuf->b_p_ts;
	}
#endif
    }
    // count spaces required for indent
    যাবত্ (todo > 0)
    {
	যদি (*p != ' ')
	    doit = TRUE;
	অন্যথা
	    ++p;
	--todo;
	++ind_len;
	// ++ind_done;
    }

    // Return if the indent is OK already.
    যদি (!doit && !VIM_ISWHITE(*p) && !(flags & SIN_ঢোকাও))
	প্রত্যাগম FALSE;

    // Allocate memory for the new line.
    যদি (flags & SIN_ঢোকাও)
	p = oldline;
    অন্যথা
    {
	p = skipwhite(p);
	সারি_len -= (পূর্ণাঙ্ক)(p - oldline);
    }

    // If 'preserveindent' and 'expandtab' are both set keep the original
    // characters and allocate accordingly.  We will fill the rest with spaces
    // after the if (!curbuf->b_p_et) below.
    যদি (orig_char_len != -1)
    {
	newline = alloc(orig_char_len + size - ind_done + সারি_len);
	যদি (newline == রিক্ত)
	    প্রত্যাগম FALSE;
	todo = size - ind_done;
	ind_len = orig_char_len + todo;    // Set total length of indent in
					   // characters, which may have been
					   // undercounted until now
	p = oldline;
	s = newline;
	যাবত্ (orig_char_len > 0)
	{
	    *s++ = *p++;
	    orig_char_len--;
	}

	// Skip over any additional white space (useful when newindent is less
	// than old)
	যাবত্ (VIM_ISWHITE(*p))
	    ++p;

    }
    অন্যথা
    {
	todo = size;
	newline = alloc(ind_len + সারি_len);
	যদি (newline == রিক্ত)
	    প্রত্যাগম FALSE;
	s = newline;
    }

    // Put the characters in the new line.
    // if 'expandtab' isn't set: use TABs
    যদি (!curbuf->b_p_et)
    {
	// If 'preserveindent' is set then reuse as much as possible of
	// the existing indent structure for the new indent
	যদি (!(flags & SIN_ঢোকাও) && curbuf->b_p_pi)
	{
	    p = oldline;
	    ind_done = 0;

	    যাবত্ (todo > 0 && VIM_ISWHITE(*p))
	    {
		যদি (*p == TAB)
		{
#ifdef FEAT_VARTABS
		    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#অন্যথা
		    tab_pad = (পূর্ণাঙ্ক)curbuf->b_p_ts
					   - (ind_done % (পূর্ণাঙ্ক)curbuf->b_p_ts);
#endif
		    // stop if this tab will overshoot the target
		    যদি (todo < tab_pad)
			ভঙ্গ;
		    todo -= tab_pad;
		    ind_done += tab_pad;
		}
		অন্যথা
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
#অন্যথা
	    tab_pad = (পূর্ণাঙ্ক)curbuf->b_p_ts - (ind_done % (পূর্ণাঙ্ক)curbuf->b_p_ts);
#endif
	    যদি (todo >= tab_pad)
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
	প্রতি (;;)
	{
	    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
	    যদি (todo < tab_pad)
		ভঙ্গ;
	    *s++ = TAB;
	    todo -= tab_pad;
	    ind_done += tab_pad;
	}
#অন্যথা
	যাবত্ (todo >= (পূর্ণাঙ্ক)curbuf->b_p_ts)
	{
	    *s++ = TAB;
	    todo -= (পূর্ণাঙ্ক)curbuf->b_p_ts;
	}
#endif
    }
    যাবত্ (todo > 0)
    {
	*s++ = ' ';
	--todo;
    }
    mch_memmove(s, p, (size_t)সারি_len);

    // Replace the line (unless undo fails).
    যদি (!(flags & SIN_UNDO) || u_savesub(curwin->w_cursor.lnum) == OK)
    {
	colnr_T old_offset = (colnr_T)(p - oldline);
	colnr_T new_offset = (colnr_T)(s - newline);

	// this may free "newline"
	ml_replace(curwin->w_cursor.lnum, newline, FALSE);
	যদি (flags & SIN_CHANGED)
	    changed_bytes(curwin->w_cursor.lnum, 0);

	// Correct saved cursor position if it is in this line.
	যদি (saved_cursor.lnum == curwin->w_cursor.lnum)
	{
	    যদি (saved_cursor.col >= old_offset)
		// cursor was after the indent, adjust for the number of
		// bytes added/removed
		saved_cursor.col += ind_len - old_offset;
	    অন্যথা যদি (saved_cursor.col >= new_offset)
		// cursor was in the indent, and is now after it, put it back
		// at the start of the indent (replacing spaces with TAB)
		saved_cursor.col = new_offset;
	}
#ifdef FEAT_PROP_POPUP
	যদি (curbuf->b_has_textprop)
	{
	    পূর্ণাঙ্ক added = ind_len - old_offset;

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
    অন্যথা
	vim_free(newline);

    curwin->w_cursor.col = ind_len;
    প্রত্যাগম retval;
}

/*
 * Return the indent of the current line after a number.  Return -1 if no
 * number was found.  Used for 'n' in 'formatoptions': numbered list.
 * Since a pattern is used it can actually handle more than numbers.
 */
    পূর্ণাঙ্ক
get_সংখ্যা_indent(linenr_T lnum)
{
    colnr_T	col;
    pos_T	pos;

    regmatch_T	regmatch;
    পূর্ণাঙ্ক		lead_len = 0;	// length of comment leader

    যদি (lnum > curbuf->b_ml.ml_সারি_গুনতি)
	প্রত্যাগম -1;
    pos.lnum = 0;

    // In format_lines() (i.e. not insert mode), fo+=q is needed too...
    যদি ((State & MODE_ঢোকাও) || has_format_option(FO_Q_COMS))
	lead_len = get_leader_len(ml_get(lnum), রিক্ত, FALSE, TRUE);

    regmatch.regprog = vim_regcomp(curbuf->b_p_flp, RE_MAGIC);
    যদি (regmatch.regprog != রিক্ত)
    {
	regmatch.rm_ic = FALSE;

	// vim_regexec() expects a pointer to a line.  This lets us
	// start matching for the flp beyond any comment leader...
	যদি (vim_regexec(&regmatch, ml_get(lnum) + lead_len, (colnr_T)0))
	{
	    pos.lnum = lnum;
	    pos.col = (colnr_T)(*regmatch.endp - ml_get(lnum));
	    pos.coladd = 0;
	}
	vim_regfree(regmatch.regprog);
    }

    যদি (pos.lnum == 0 || *ml_get_pos(&pos) == NUL)
	প্রত্যাগম -1;
    getvcol(curwin, &pos, &col, রিক্ত, রিক্ত, 0);
    প্রত্যাগম (পূর্ণাঙ্ক)col;
}

#যদি defined(FEAT_LINEBREAK)
/*
 * Check "briopt" as 'breakindentopt' and update the members of "wp".
 * This is called when 'breakindentopt' is changed and when a window is
 * initialized.
 * Returns FAIL for failure, OK otherwise.
 */
    পূর্ণাঙ্ক
briopt_check(
    char_u	*briopt,	// when NULL: use "wp->w_p_briopt"
    win_T	*wp)		// when NULL: only check "briopt"
{
    char_u	*p;
    পূর্ণাঙ্ক		bri_shift = 0;
    long	bri_min = 20;
    পূর্ণাঙ্ক		bri_sbr = FALSE;
    পূর্ণাঙ্ক		bri_list = 0;
    পূর্ণাঙ্ক		bri_vcol = 0;

    যদি (briopt != রিক্ত)
	p = briopt;
    অন্যথা
	p = wp->w_p_briopt;

    যাবত্ (*p != NUL)
    {
	// Note: Keep this in sync with p_briopt_values
	যদি (STRNCMP(p, "shift:", 6) == 0
		 && ((p[6] == '-' && VIM_ISDIGIT(p[7])) || VIM_ISDIGIT(p[6])))
	{
	    p += 6;
	    bri_shift = getdigits(&p);
	}
	অন্যথা যদি (STRNCMP(p, "min:", 4) == 0 && VIM_ISDIGIT(p[4]))
	{
	    p += 4;
	    bri_min = getdigits(&p);
	}
	অন্যথা যদি (STRNCMP(p, "sbr", 3) == 0)
	{
	    p += 3;
	    bri_sbr = TRUE;
	}
	অন্যথা যদি (STRNCMP(p, "list:", 5) == 0)
	{
	    p += 5;
	    bri_list = getdigits(&p);
	}
	অন্যথা যদি (STRNCMP(p, "column:", 7) == 0)
	{
	    p += 7;
	    bri_vcol = getdigits(&p);
	}
	যদি (*p != ',' && *p != NUL)
	    প্রত্যাগম FAIL;
	যদি (*p == ',')
	    ++p;
    }

    যদি (wp == রিক্ত)
	প্রত্যাগম OK;

    wp->w_briopt_shift = bri_shift;
    wp->w_briopt_min   = bri_min;
    wp->w_briopt_sbr   = bri_sbr;
    wp->w_briopt_list  = bri_list;
    wp->w_briopt_vcol  = bri_vcol;

    প্রত্যাগম OK;
}

/*
 * Return appropriate space number for breakindent, taking influencing
 * parameters into account. Window must be specified, since it is not
 * necessarily always the current one.
 */
    পূর্ণাঙ্ক
get_breakindent_win(
    win_T	*wp,
    char_u	*সারি) // start of the line
{
    স্থির পূর্ণাঙ্ক	    prev_indent = 0;	// cached indent value
    স্থির long	    prev_ts     = 0L;	// cached tabstop value
# ifdef FEAT_VARTABS
    স্থির পূর্ণাঙ্ক      *prev_vts = রিক্ত;   // cached vartabs values
# endif
    স্থির পূর্ণাঙ্ক	    prev_fnum   = 0;	// cached buffer number
    স্থির char_u   *prev_সারি  = রিক্ত;	// cached copy of "line"
    স্থির varnumber_T prev_tick = 0;   // changedtick of cached value
    স্থির পূর্ণাঙ্ক      prev_list = 0;	// cached list indent
    স্থির পূর্ণাঙ্ক      prev_listopt = 0;	// cached w_p_briopt_list value
    স্থির পূর্ণাঙ্ক      prev_no_ts = FALSE;	// cached no_ts value
    স্থির unsigned prev_dy_uhex = 0;	// cached 'display' "uhex" value
    // cached formatlistpat value
    স্থির char_u   *prev_flp = রিক্ত;
    পূর্ণাঙ্ক		    bri = 0;
    // window width minus window margin space, i.e. what rests for text
    ধ্রুব পূর্ণাঙ্ক	    eff_wwidth = wp->w_width
					  - win_col_off(wp) + win_col_off2(wp);

    // In list mode, if 'listchars' "tab" isn't set, a TAB is displayed as ^I.
    পূর্ণাঙ্ক no_ts = wp->w_p_list && wp->w_lcs_chars.tab1 == NUL;

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
    যদি (prev_fnum != wp->w_buffer->b_fnum
	    || prev_ts != wp->w_buffer->b_p_ts
# ifdef FEAT_VARTABS
	    || prev_vts != wp->w_buffer->b_p_vts_array
# endif
	    || prev_tick != CHANGEDTICK(wp->w_buffer)
	    || prev_listopt != wp->w_briopt_list
	    || prev_no_ts != no_ts
	    || prev_dy_uhex != (dy_flags & DY_UHEX)
	    || prev_flp == রিক্ত
	    || STRCMP(prev_flp, get_flp_মান(wp->w_buffer)) != 0
	    || prev_সারি == রিক্ত || STRCMP(prev_সারি, সারি) != 0
	)
    {
	prev_fnum = wp->w_buffer->b_fnum;
	vim_free(prev_সারি);
	prev_সারি = vim_strsave(সারি);
	prev_ts = wp->w_buffer->b_p_ts;
# ifdef FEAT_VARTABS
	prev_vts = wp->w_buffer->b_p_vts_array;
	যদি (wp->w_briopt_vcol == 0)
	    prev_indent = get_indent_str_vtab(সারি,
				     (পূর্ণাঙ্ক)wp->w_buffer->b_p_ts,
				      wp->w_buffer->b_p_vts_array, no_ts);
# অন্যথা
	যদি (wp->w_briopt_vcol == 0)
	    prev_indent = get_indent_str(সারি,
					(পূর্ণাঙ্ক)wp->w_buffer->b_p_ts, no_ts);
# endif
	prev_tick = CHANGEDTICK(wp->w_buffer);
	prev_listopt = wp->w_briopt_list;
	prev_list = 0;
	prev_no_ts = no_ts;
	prev_dy_uhex = (dy_flags & DY_UHEX);
	vim_free(prev_flp);
	prev_flp = vim_strsave(get_flp_মান(wp->w_buffer));
	// add additional indent for numbered lists
	যদি (wp->w_briopt_list != 0 && wp->w_briopt_vcol == 0)
	{
	    regmatch_T	    regmatch;

	    regmatch.regprog = vim_regcomp(prev_flp,
				   RE_MAGIC + RE_STRING + RE_AUTO + RE_STRICT);

	    যদি (regmatch.regprog != রিক্ত)
	    {
		regmatch.rm_ic = FALSE;
		যদি (vim_regexec(&regmatch, সারি, 0))
		{
		    যদি (wp->w_briopt_list > 0)
			prev_list = wp->w_briopt_list;
		    অন্যথা
		    {
			char_u	*ptr = *regmatch.startp;
			char_u	*end_ptr = *regmatch.endp;
			পূর্ণাঙ্ক	indent = 0;

			// Compute the width of the matched text.
			// Use win_chartabsize() so that TAB size is correct,
			// while wrapping is ignored.
			যাবত্ (ptr < end_ptr)
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
    যদি (wp->w_briopt_vcol != 0)
    {
	// column value has priority
	bri = wp->w_briopt_vcol;
	prev_list = 0;
    }
    অন্যথা
	bri = prev_indent + wp->w_briopt_shift;

    // Add offset for number column, if 'n' is in 'cpoptions'
    bri += win_col_off2(wp);

    // add additional indent for numbered lists
    যদি (wp->w_briopt_list > 0)
	bri += prev_list;

    // indent minus the length of the showbreak string
    যদি (wp->w_briopt_sbr)
	bri -= vim_strsize(get_showbreak_মান(wp));


    // never indent past left window margin
    যদি (bri < 0)
	bri = 0;

    // always leave at least bri_min characters on the left,
    // if text width is sufficient
    অন্যথা যদি (bri > eff_wwidth - wp->w_briopt_min)
	bri = (eff_wwidth - wp->w_briopt_min < 0)
					   ? 0 : eff_wwidth - wp->w_briopt_min;

    প্রত্যাগম bri;
}
#endif

/*
 * When extra == 0: Return TRUE if the cursor is before or on the first
 *		    non-blank in the line.
 * When extra == 1: Return TRUE if the cursor is before the first non-blank in
 *		    the line.
 */
    পূর্ণাঙ্ক
inindent(পূর্ণাঙ্ক extra)
{
    char_u	*ptr;
    colnr_T	col;

    প্রতি (col = 0, ptr = ml_get_curline(); VIM_ISWHITE(*ptr); ++col)
	++ptr;
    যদি (col >= curwin->w_cursor.col + extra)
	প্রত্যাগম TRUE;
    অন্যথা
	প্রত্যাগম FALSE;
}

/*
 * op_reindent - handle reindenting a block of lines.
 */
    শূন্য
op_reindent(oparg_T *oap, পূর্ণাঙ্ক (*how)(শূন্য))
{
    long	i = 0;
    char_u	*l;
    পূর্ণাঙ্ক		amount;
    linenr_T	first_changed = 0;
    linenr_T	last_changed = 0;
    linenr_T	start_lnum = curwin->w_cursor.lnum;

    // Don't even try when 'modifiable' is off.
    যদি (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	প্রত্যাগম;
    }

    // Save for undo.  Do this once for all lines, much faster than doing this
    // for each line separately, especially when undoing.
    যদি (u_savecommon(start_lnum - 1, start_lnum + oap->সারি_গুনতি,
				     start_lnum + oap->সারি_গুনতি, FALSE) == OK)
	প্রতি (i = oap->সারি_গুনতি; --i >= 0 && !got_int; )
	{
	    // it's a slow thing to do, so give feedback so there's no worry
	    // that the computer's just hung.

	    যদি (i > 1
		    && (i % 50 == 0 || i == oap->সারি_গুনতি - 1)
		    && oap->সারি_গুনতি > p_report)
		smsg(_("%ld lines to indent... "), i);

	    // Be vi-compatible: For lisp indenting the first line is not
	    // indented, unless there is only one line.
	    যদি (i != oap->সারি_গুনতি - 1 || oap->সারি_গুনতি == 1
						     || how != get_lisp_indent)
	    {
		l = skipwhite(ml_get_curline());
		যদি (*l == NUL)		    // empty or blank line
		    amount = 0;
		অন্যথা
		    amount = how();	    // get the indent for this line

		যদি (amount >= 0 && set_indent(amount, 0))
		{
		    // did change the indent, call changed_lines() later
		    যদি (first_changed == 0)
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
    যদি (last_changed != 0)
	changed_সারিগুলি(first_changed, 0,
		oap->is_VIsual ? start_lnum + oap->সারি_গুনতি :
		last_changed + 1, 0L);
    অন্যথা যদি (oap->is_VIsual)
	redraw_curbuf_later(UPD_INVERTED);

    যদি (oap->সারি_গুনতি > p_report)
    {
	i = oap->সারি_গুনতি - (i + 1);
	smsg(NGETTEXT("%ld line indented ",
						 "%ld lines indented ", i), i);
    }
    যদি ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// set '[ and '] marks
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
    }
}

/*
 * Return TRUE if lines starting with '#' should be left aligned.
 */
    পূর্ণাঙ্ক
preprocs_বাম(শূন্য)
{
    প্রত্যাগম
	(curbuf->b_p_si && !curbuf->b_p_cin) ||
	(curbuf->b_p_cin && in_cinkeys('#', ' ', TRUE)
					   && curbuf->b_ind_hash_comment == 0)
	;
}

/*
 * Return TRUE if the conditions are OK for smart indenting.
 */
    পূর্ণাঙ্ক
may_do_si(শূন্য)
{
    প্রত্যাগম curbuf->b_p_si
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
    শূন্য
ins_try_si(পূর্ণাঙ্ক c)
{
    pos_T	*pos, old_pos;
    char_u	*ptr;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		temp;

    // do some very smart indenting when entering '{' or '}'
    যদি (((did_si || can_si_back) && c == '{')
	    || (can_si && c == '}' && inindent(0)))
    {
	// for '}' set indent equal to indent of line containing matching '{'
	যদি (c == '}' && (pos = findmatch(রিক্ত, '{')) != রিক্ত)
	{
	    old_pos = curwin->w_cursor;
	    // If the matching '{' has a ')' immediately before it (ignoring
	    // white-space), then line up with the start of the line
	    // containing the matching '(' if there is one.  This handles the
	    // case where an "if (..\n..) {" statement continues over multiple
	    // lines -- webb
	    ptr = ml_get(pos->lnum);
	    i = pos->col;
	    যদি (i > 0)		// skip blanks before '{'
		যাবত্ (--i > 0 && VIM_ISWHITE(ptr[i]))
		    ;
	    curwin->w_cursor.lnum = pos->lnum;
	    curwin->w_cursor.col = i;
	    যদি (ptr[i] == ')' && (pos = findmatch(রিক্ত, '(')) != রিক্ত)
		curwin->w_cursor = *pos;
	    i = get_indent();
	    curwin->w_cursor = old_pos;
	    যদি (State & VREPLACE_FLAG)
		change_indent(INDENT_SET, i, FALSE, NUL, TRUE);
	    অন্যথা
		(শূন্য)set_indent(i, SIN_CHANGED);
	}
	অন্যথা যদি (curwin->w_cursor.col > 0)
	{
	    // when inserting '{' after "O" reduce indent, but not
	    // more than indent of previous line
	    temp = TRUE;
	    যদি (c == '{' && can_si_back && curwin->w_cursor.lnum > 1)
	    {
		old_pos = curwin->w_cursor;
		i = get_indent();
		যাবত্ (curwin->w_cursor.lnum > 1)
		{
		    ptr = skipwhite(ml_get(--(curwin->w_cursor.lnum)));

		    // ignore empty lines and lines starting with '#'.
		    যদি (*ptr != '#' && *ptr != NUL)
			ভঙ্গ;
		}
		যদি (get_indent() >= i)
		    temp = FALSE;
		curwin->w_cursor = old_pos;
	    }
	    যদি (temp)
		shift_সারি(TRUE, FALSE, 1, TRUE);
	}
    }

    // set indent of '#' always to 0
    যদি (curwin->w_cursor.col > 0 && can_si && c == '#' && inindent(0))
    {
	// remember current indent for next line
	old_indent = get_indent();
	(শূন্য)set_indent(0, SIN_CHANGED);
    }

    // Adjust ai_col, the char at this position can be deleted.
    যদি (ai_col > curwin->w_cursor.col)
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
    শূন্য
change_indent(
    পূর্ণাঙ্ক		type,
    পূর্ণাঙ্ক		amount,
    পূর্ণাঙ্ক		round,
    পূর্ণাঙ্ক		replaced,	// replaced character, put on replace stack
    পূর্ণাঙ্ক		call_changed_bytes)	// call changed_bytes()
{
    পূর্ণাঙ্ক		vcol;
    পূর্ণাঙ্ক		last_vcol;
    পূর্ণাঙ্ক		insstart_less;		// reduction for Insstart.col
    পূর্ণাঙ্ক		new_cursor_col;
    পূর্ণাঙ্ক		i;
    char_u	*ptr;
    পূর্ণাঙ্ক		save_p_list;
    পূর্ণাঙ্ক		start_col;
    colnr_T	vc;
    colnr_T	orig_col = 0;		// init for GCC
    char_u	*new_সারি, *orig_সারি = রিক্ত;	// init for GCC

    // MODE_VREPLACE state needs to know what the line was like before changing
    যদি (State & VREPLACE_FLAG)
    {
	orig_সারি = vim_strnsave(ml_get_curline(), ml_get_curline_len());  // Deal with NULL below
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
    যদি (new_cursor_col < 0)
	vcol = get_indent() - vcol;

    যদি (new_cursor_col > 0)	    // can't fix replace stack
	start_col = -1;

    // Set the new indent.  The cursor will be put on the first non-blank.
    যদি (type == INDENT_SET)
	(শূন্য)set_indent(amount, call_changed_bytes ? SIN_CHANGED : 0);
    অন্যথা
    {
	পূর্ণাঙ্ক	save_State = State;

	// Avoid being called recursively.
	যদি (State & VREPLACE_FLAG)
	    State = MODE_ঢোকাও;
	shift_সারি(type == INDENT_DEC, round, 1, call_changed_bytes);
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
    যদি (new_cursor_col >= 0)
    {
	// When changing the indent while the cursor is touching it, reset
	// Insstart_col to 0.
	যদি (new_cursor_col == 0)
	    insstart_less = MAXCOL;
	new_cursor_col += curwin->w_cursor.col;
    }
    অন্যথা যদি (!(State & MODE_ঢোকাও))
	new_cursor_col = curwin->w_cursor.col;
    অন্যথা
    {
	chartabsize_T cts;

	// Compute the screen column where the cursor should be.
	vcol = get_indent() - vcol;
	curwin->w_virtcol = (colnr_T)((vcol < 0) ? 0 : vcol);

	// Advance the cursor until we reach the right screen column.
	last_vcol = 0;
	ptr = ml_get_curline();
	init_chartabsize_arg(&cts, curwin, 0, 0, ptr, ptr);
	যাবত্ (cts.cts_vcol <= (পূর্ণাঙ্ক)curwin->w_virtcol)
	{
	    last_vcol = cts.cts_vcol;
	    যদি (cts.cts_vcol > 0)
		MB_PTR_ADV(cts.cts_ptr);
	    যদি (*cts.cts_ptr == NUL)
		ভঙ্গ;
	    cts.cts_vcol += lbr_chartabsize(&cts);
	}
	vcol = last_vcol;
	new_cursor_col = cts.cts_ptr - cts.cts_সারি;
	clear_chartabsize_arg(&cts);

	// May need to insert spaces to be able to position the cursor on
	// the right screen column.
	যদি (vcol != (পূর্ণাঙ্ক)curwin->w_virtcol)
	{
	    curwin->w_cursor.col = (colnr_T)new_cursor_col;
	    i = (পূর্ণাঙ্ক)curwin->w_virtcol - vcol;
	    ptr = alloc(i + 1);
	    যদি (ptr != রিক্ত)
	    {
		size_t ptrlen;
		new_cursor_col += i;
		ptr[i] = NUL;
		ptrlen = i;
		যাবত্ (--i >= 0)
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

    যদি (new_cursor_col <= 0)
	curwin->w_cursor.col = 0;
    অন্যথা
	curwin->w_cursor.col = (colnr_T)new_cursor_col;
    curwin->w_set_curswant = true;
    changed_cline_bef_curs();

    // May have to adjust the start of the insert.
    যদি (State & MODE_ঢোকাও)
    {
	যদি (curwin->w_cursor.lnum == Insstart.lnum && Insstart.col != 0)
	{
	    যদি ((পূর্ণাঙ্ক)Insstart.col <= insstart_less)
		Insstart.col = 0;
	    অন্যথা
		Insstart.col -= insstart_less;
	}
	যদি ((পূর্ণাঙ্ক)ai_col <= insstart_less)
	    ai_col = 0;
	অন্যথা
	    ai_col -= insstart_less;
    }

    // For MODE_REPLACE state, may have to fix the replace stack, if it's
    // possible.  If the number of characters before the cursor decreased, need
    // to pop a few characters from the replace stack.
    // If the number of characters before the cursor increased, need to push a
    // few NULs onto the replace stack.
    যদি (REPLACE_NORMAL(State) && start_col >= 0)
    {
	যাবত্ (start_col > (পূর্ণাঙ্ক)curwin->w_cursor.col)
	{
	    replace_join(0);	    // remove a NUL from the replace stack
	    --start_col;
	}
	যাবত্ (start_col < (পূর্ণাঙ্ক)curwin->w_cursor.col || replaced)
	{
	    replace_ঠেলো(NUL);
	    যদি (replaced)
	    {
		replace_ঠেলো(replaced);
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
    যদি (State & VREPLACE_FLAG)
    {
	// If orig_line didn't allocate, just return.  At least we did the job,
	// even if you can't backspace.
	যদি (orig_সারি == রিক্ত)
	    প্রত্যাগম;

	// Save new line
	new_সারি = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	যদি (new_সারি == রিক্ত)
	{
	    vim_free(orig_সারি);
	    প্রত্যাগম;
	}

	// We only put back the new line up to the cursor
	new_সারি[curwin->w_cursor.col] = NUL;

	// Put back original line
	ml_replace(curwin->w_cursor.lnum, orig_সারি, FALSE);
	curwin->w_cursor.col = orig_col;

	// Backspace from cursor to start of line
	backspace_until_column(0);

	// Insert new stuff into line again
	ins_bytes(new_সারি);

	vim_free(new_সারি);
    }
}

/*
 * Copy the indent from ptr to the current line (and fill to size)
 * Leaves the cursor on the first non-blank in the line.
 * Returns TRUE if the line was changed.
 */
    পূর্ণাঙ্ক
copy_indent(পূর্ণাঙ্ক size, char_u *src)
{
    char_u	*p = রিক্ত;
    char_u	*সারি = রিক্ত;
    char_u	*s;
    পূর্ণাঙ্ক		todo;
    পূর্ণাঙ্ক		ind_len;
    পূর্ণাঙ্ক		সারি_len = 0;
    পূর্ণাঙ্ক		tab_pad;
    পূর্ণাঙ্ক		ind_done;
    পূর্ণাঙ্ক		round;
#ifdef FEAT_VARTABS
    পূর্ণাঙ্ক		ind_col;
#endif

    // Round 1: compute the number of characters needed for the indent
    // Round 2: copy the characters.
    প্রতি (round = 1; round <= 2; ++round)
    {
	todo = size;
	ind_len = 0;
	ind_done = 0;
#ifdef FEAT_VARTABS
	ind_col = 0;
#endif
	s = src;

	// Count/copy the usable portion of the source line
	যাবত্ (todo > 0 && VIM_ISWHITE(*s))
	{
	    যদি (*s == TAB)
	    {
#ifdef FEAT_VARTABS
		tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#অন্যথা
		tab_pad = (পূর্ণাঙ্ক)curbuf->b_p_ts
					   - (ind_done % (পূর্ণাঙ্ক)curbuf->b_p_ts);
#endif
		// Stop if this tab will overshoot the target
		যদি (todo < tab_pad)
		    ভঙ্গ;
		todo -= tab_pad;
		ind_done += tab_pad;
#ifdef FEAT_VARTABS
		ind_col += tab_pad;
#endif
	    }
	    অন্যথা
	    {
		--todo;
		++ind_done;
#ifdef FEAT_VARTABS
		++ind_col;
#endif
	    }
	    ++ind_len;
	    যদি (p != রিক্ত)
		*p++ = *s;
	    ++s;
	}

	// Fill to next tabstop with a tab, if possible
#ifdef FEAT_VARTABS
	tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#অন্যথা
	tab_pad = (পূর্ণাঙ্ক)curbuf->b_p_ts - (ind_done % (পূর্ণাঙ্ক)curbuf->b_p_ts);
#endif
	যদি (todo >= tab_pad && !curbuf->b_p_et)
	{
	    todo -= tab_pad;
	    ++ind_len;
#ifdef FEAT_VARTABS
	    ind_col += tab_pad;
#endif
	    যদি (p != রিক্ত)
		*p++ = TAB;
	}

	// Add tabs required for indent
	যদি (!curbuf->b_p_et)
	{
#ifdef FEAT_VARTABS
	    প্রতি (;;)
	    {
		tab_pad = tabstop_padding(ind_col, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
		যদি (todo < tab_pad)
		    ভঙ্গ;
		todo -= tab_pad;
		++ind_len;
		ind_col += tab_pad;
		যদি (p != রিক্ত)
		    *p++ = TAB;
	    }
#অন্যথা
	    যাবত্ (todo >= (পূর্ণাঙ্ক)curbuf->b_p_ts)
	    {
		todo -= (পূর্ণাঙ্ক)curbuf->b_p_ts;
		++ind_len;
		যদি (p != রিক্ত)
		    *p++ = TAB;
	    }
#endif
	}

	// Count/add spaces required for indent
	যাবত্ (todo > 0)
	{
	    --todo;
	    ++ind_len;
	    যদি (p != রিক্ত)
		*p++ = ' ';
	}

	যদি (p == রিক্ত)
	{
	    // Allocate memory for the result: the copied indent, new indent
	    // and the rest of the line.
	    সারি_len = ml_get_curline_len() + 1;
	    সারি = alloc(ind_len + সারি_len);
	    যদি (সারি == রিক্ত)
		প্রত্যাগম FALSE;
	    p = সারি;
	}
    }

    // Append the original line
    mch_memmove(p, ml_get_curline(), (size_t)সারি_len);

    // Replace the line
    ml_replace(curwin->w_cursor.lnum, সারি, FALSE);

    // Put the cursor after the indent.
    curwin->w_cursor.col = ind_len;
    প্রত্যাগম TRUE;
}

/*
 * Give a "resulting text too long" error and maybe set got_int.
 */
    স্থির শূন্য
emsg_text_too_long(শূন্য)
{
    emsg(_(e_resulting_text_too_long));
#ifdef FEAT_EVAL
    // when not inside a try/catch set got_int to break out of any loop
    যদি (trylevel == 0)
#endif
	got_int = TRUE;
}

/*
 * ":retab".
 */
    শূন্য
ex_retab(exarg_T *eap)
{
    linenr_T	lnum;
    পূর্ণাঙ্ক		got_tab = FALSE;
    long	অঙ্ক_spaces = 0;
    long	অঙ্ক_tabs;
    long	len;
    long	col;
    long	vcol;
    long	start_col = 0;		// For start of white-space string
    long	start_vcol = 0;		// For start of white-space string
    long	old_len;
    long	new_len;
    char_u	*ptr;
    char_u	*new_সারি = (char_u *)1; // init to non-NULL
    পূর্ণাঙ্ক		did_undo;		// called u_save for current line
#ifdef FEAT_VARTABS
    পূর্ণাঙ্ক		*new_vts_array = রিক্ত;
    char_u	*new_ts_str;		// string value of tab argument
#অন্যথা
    পূর্ণাঙ্ক		temp;
    পূর্ণাঙ্ক		new_ts = 0;
#endif
    পূর্ণাঙ্ক		save_list;
    linenr_T	first_সারি = 0;		// first changed line
    linenr_T	last_সারি = 0;		// last changed line
    পূর্ণাঙ্ক		is_indent_only = 0;	// Only process leading whitespace

    save_list = curwin->w_p_list;
    curwin->w_p_list = 0;	    // don't want list mode here

    ptr = eap->arg;
    যদি (STRNCMP(ptr, "-indentonly", 11) == 0 && IS_WHITE_OR_NUL(ptr[11]))
    {
	is_indent_only = 1;
	ptr = skipwhite(ptr + 11);
    }

#ifdef FEAT_VARTABS
    new_ts_str = ptr;
    যদি (tabstop_set(ptr, &new_vts_array) == FAIL)
	প্রত্যাগম;
    যাবত্ (vim_isdigit(*ptr) || *ptr == ',')
	++ptr;

    // This ensures that either new_vts_array and new_ts_str are freshly
    // allocated, or new_vts_array points to an existing array and new_ts_str
    // is null.
    যদি (new_vts_array == রিক্ত)
    {
	new_vts_array = curbuf->b_p_vts_array;
	new_ts_str = রিক্ত;
    }
    অন্যথা
	new_ts_str = vim_strnsave(new_ts_str, ptr - new_ts_str);
#অন্যথা
    যদি (ptr[0] != NUL && (ptr[0] != '0' || ptr[1] != NUL))
    {
	char_u	*end;

	যদি (strtol((বর্ণ *)ptr, (বর্ণ **)&end, 10) <= 0)
	{
	    যদি (ptr != end)
		emsg(_(e_argument_must_be_positive));
	    অন্যথা
		semsg(_(e_invalid_argument_str), ptr);
	    প্রত্যাগম;
	}
	new_ts = getdigits(&ptr);
	যদি (new_ts < 0 || new_ts > TABSTOP_MAX)
	{
	    semsg(_(e_invalid_argument_str), eap->arg);
	    প্রত্যাগম;
	}
    }
    যদি (new_ts == 0)
	new_ts = curbuf->b_p_ts;
#endif
    প্রতি (lnum = eap->line1; !got_int && lnum <= eap->line2; ++lnum)
    {
	ptr = ml_get(lnum);
	old_len = ml_get_len(lnum);
	col = 0;
	vcol = 0;
	did_undo = FALSE;
	প্রতি (;;)
	{
	    যদি (VIM_ISWHITE(ptr[col]))
	    {
		যদি (!got_tab && অঙ্ক_spaces == 0)
		{
		    // First consecutive white-space
		    start_vcol = vcol;
		    start_col = col;
		}
		যদি (ptr[col] == ' ')
		    অঙ্ক_spaces++;
		অন্যথা
		    got_tab = TRUE;
	    }
	    অন্যথা
	    {
		যদি (got_tab || (eap->forceit && অঙ্ক_spaces > 1))
		{
		    // Retabulate this string of white-space

		    // len is virtual length of white string
		    len = অঙ্ক_spaces = vcol - start_vcol;
		    অঙ্ক_tabs = 0;
		    যদি (!curbuf->b_p_et)
		    {
#ifdef FEAT_VARTABS
			পূর্ণাঙ্ক t, s;

			tabstop_fromto(start_vcol, vcol,
					curbuf->b_p_ts, new_vts_array, &t, &s);
			অঙ্ক_tabs = t;
			অঙ্ক_spaces = s;
#অন্যথা
			temp = new_ts - (start_vcol % new_ts);
			যদি (অঙ্ক_spaces >= temp)
			{
			    অঙ্ক_spaces -= temp;
			    অঙ্ক_tabs++;
			}
			অঙ্ক_tabs += অঙ্ক_spaces / new_ts;
			অঙ্ক_spaces -= (অঙ্ক_spaces / new_ts) * new_ts;
#endif
		    }
		    যদি (curbuf->b_p_et || got_tab ||
					(অঙ্ক_spaces + অঙ্ক_tabs < len))
		    {
			যদি (did_undo == FALSE)
			{
			    did_undo = TRUE;
			    যদি (u_save((linenr_T)(lnum - 1),
						(linenr_T)(lnum + 1)) == FAIL)
			    {
				new_সারি = রিক্ত;	// flag out-of-memory
				ভঙ্গ;
			    }
			}

			// len is actual number of white characters used
			len = অঙ্ক_spaces + অঙ্ক_tabs;
			new_len = old_len - col + start_col + len + 1;
			যদি (new_len <= 0 || new_len >= MAXCOL)
			{
			    emsg_text_too_long();
			    ভঙ্গ;
			}
			new_সারি = alloc(new_len);
			যদি (new_সারি == রিক্ত)
			    ভঙ্গ;
			যদি (start_col > 0)
			    mch_memmove(new_সারি, ptr, (size_t)start_col);
			mch_memmove(new_সারি + start_col + len,
				      ptr + col, (size_t)(old_len - col + 1));
			ptr = new_সারি + start_col;
			প্রতি (col = 0; col < len; col++)
			    ptr[col] = (col < অঙ্ক_tabs) ? '\t' : ' ';
			যদি (ml_replace(lnum, new_সারি, FALSE) == OK)
			    // "new_line" may have been copied
			    new_সারি = curbuf->b_ml.ml_সারি_ptr;
			যদি (first_সারি == 0)
			    first_সারি = lnum;
			last_সারি = lnum;
			ptr = new_সারি;
			old_len = new_len - 1;
			col = start_col + len;
		    }
		}
		got_tab = FALSE;
		অঙ্ক_spaces = 0;

		যদি (is_indent_only)
		    ভঙ্গ;
	    }
	    যদি (ptr[col] == NUL)
		ভঙ্গ;
	    vcol += chartabsize(ptr + col, (colnr_T)vcol);
	    যদি (vcol >= MAXCOL)
	    {
		emsg_text_too_long();
		ভঙ্গ;
	    }
	    যদি (has_mbyte)
		col += (*mb_ptr2len)(ptr + col);
	    অন্যথা
		++col;
	}
	যদি (new_সারি == রিক্ত)		    // out of memory
	    ভঙ্গ;
	সারি_breakcheck();
    }
    যদি (got_int)
	emsg(_(e_interrupted));

#ifdef FEAT_VARTABS
    // If a single value was given then it can be considered equal to
    // either the value of 'tabstop' or the value of 'vartabstop'.
    যদি (tabstop_গুনতি(curbuf->b_p_vts_array) == 0
	&& tabstop_গুনতি(new_vts_array) == 1
	&& curbuf->b_p_ts == tabstop_first(new_vts_array))
	; // not changed
    অন্যথা যদি (tabstop_গুনতি(curbuf->b_p_vts_array) > 0
	&& tabstop_eq(curbuf->b_p_vts_array, new_vts_array))
	; // not changed
    অন্যথা
	redraw_curbuf_later(UPD_NOT_VALID);
#অন্যথা
    যদি (curbuf->b_p_ts != new_ts)
	redraw_curbuf_later(UPD_NOT_VALID);
#endif
    যদি (first_সারি != 0)
	changed_সারিগুলি(first_সারি, 0, last_সারি + 1, 0L);

    curwin->w_p_list = save_list;	// restore 'list'

#ifdef FEAT_VARTABS
    যদি (new_ts_str != রিক্ত)		// set the new tabstop
    {
	// If 'vartabstop' is in use or if the value given to retab has more
	// than one tabstop then update 'vartabstop'.
	পূর্ণাঙ্ক *old_vts_ary = curbuf->b_p_vts_array;

	যদি (tabstop_গুনতি(old_vts_ary) > 0 || tabstop_গুনতি(new_vts_array) > 1)
	{
	    set_string_option_direct((char_u *)"vts", -1, new_ts_str,
							OPT_FREE|OPT_LOCAL, 0);
	    curbuf->b_p_vts_array = new_vts_array;
	    vim_free(old_vts_ary);
	}
	অন্যথা
	{
	    // 'vartabstop' wasn't in use and a single value was given to
	    // retab then update 'tabstop'.
	    curbuf->b_p_ts = tabstop_first(new_vts_array);
	    vim_free(new_vts_array);
	}
	vim_free(new_ts_str);
    }
#অন্যথা
    curbuf->b_p_ts = new_ts;
#endif
    coladvance(curwin->w_curswant);

    u_clearline();
}

#যদি defined(FEAT_EVAL)
/*
 * Get indent level from 'indentexpr'.
 */
    পূর্ণাঙ্ক
get_রাশিমালা_indent(শূন্য)
{
    পূর্ণাঙ্ক		indent = -1;
    char_u	*inde_copy;
    pos_T	save_pos;
    colnr_T	save_curswant;
    পূর্ণাঙ্ক		save_set_curswant;
    পূর্ণাঙ্ক		save_State;
    পূর্ণাঙ্ক		use_sandbox = was_set_insecurely(curwin,
					    (char_u *)"indentexpr", OPT_LOCAL);
    sctx_T	save_sctx = current_sctx;

    // Save and restore cursor position and curswant, in case it was changed
    // via :normal commands
    save_pos = curwin->w_cursor;
    save_curswant = curwin->w_curswant;
    save_set_curswant = curwin->w_set_curswant;
    set_vim_var_nr(VV_LNUM, curwin->w_cursor.lnum);
    যদি (use_sandbox)
	++sandbox;
    ++textlock;
    current_sctx = curbuf->b_p_script_ctx[BV_INDE];

    // Need to make a copy, the 'indentexpr' option could be changed while
    // evaluating it.
    inde_copy = vim_strsave(curbuf->b_p_inde);
    যদি (inde_copy != রিক্ত)
    {
	indent = (পূর্ণাঙ্ক)eval_to_সংখ্যা(inde_copy, TRUE);
	vim_free(inde_copy);
    }

    যদি (use_sandbox)
	--sandbox;
    --textlock;
    current_sctx = save_sctx;

    // Restore the cursor position so that 'indentexpr' doesn't need to.
    // Pretend to be in Insert mode, allow cursor past end of line for "o"
    // command.
    save_State = State;
    State = MODE_ঢোকাও;
    curwin->w_cursor = save_pos;
    curwin->w_curswant = save_curswant;
    curwin->w_set_curswant = save_set_curswant;
    check_cursor();
    State = save_State;

    // Reset did_throw, unless 'debug' has "throw" and inside a try/catch.
    যদি (did_throw && (vim_strchr(p_debug, 't') == রিক্ত || trylevel == 0))
    {
	handle_did_throw();
	did_throw = FALSE;
    }

    // If there is an error, just keep the current indent.
    যদি (indent < 0)
	indent = get_indent();

    প্রত্যাগম indent;
}
#endif

    স্থির পূর্ণাঙ্ক
lisp_match(char_u *p)
{
    char_u	buf[LSIZE];
    পূর্ণাঙ্ক		len;
    char_u	*word = *curbuf->b_p_lw != NUL ? curbuf->b_p_lw : p_lispwords;

    যাবত্ (*word != NUL)
    {
	len = copy_option_part(&word, buf, LSIZE, ",");
	যদি (STRNCMP(buf, p, len) == 0 && IS_WHITE_OR_NUL(p[len]))
	    প্রত্যাগম TRUE;
    }
    প্রত্যাগম FALSE;
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
    পূর্ণাঙ্ক
get_lisp_indent(শূন্য)
{
    pos_T	*pos, realpos, paren;
    পূর্ণাঙ্ক		amount;
    char_u	*that;
    colnr_T	col;
    colnr_T	firsttry;
    পূর্ণাঙ্ক		parencount, quotecount;
    পূর্ণাঙ্ক		vi_lisp;

    // Set vi_lisp to use the vi-compatible method
    vi_lisp = (vim_strchr(p_cpo, CPO_LISP) != রিক্ত);

    realpos = curwin->w_cursor;
    curwin->w_cursor.col = 0;

    যদি ((pos = findmatch(রিক্ত, '(')) == রিক্ত)
	pos = findmatch(রিক্ত, '[');
    অন্যথা
    {
	paren = *pos;
	pos = findmatch(রিক্ত, '[');
	যদি (pos == রিক্ত || LT_POSP(pos, &paren))
	    pos = &paren;
    }
    যদি (pos != রিক্ত)
    {
	// Extra trick: Take the indent of the first previous non-white
	// line that is at the same () level.
	amount = -1;
	parencount = 0;

	যাবত্ (--curwin->w_cursor.lnum >= pos->lnum)
	{
	    যদি (linewhite(curwin->w_cursor.lnum))
		অনুবর্ত;
	    প্রতি (that = ml_get_curline(); *that != NUL; ++that)
	    {
		যদি (*that == ';')
		{
		    যাবত্ (*(that + 1) != NUL)
			++that;
		    অনুবর্ত;
		}
		যদি (*that == '\\')
		{
		    যদি (*(that + 1) != NUL)
			++that;
		    অনুবর্ত;
		}
		যদি (*that == '"' && *(that + 1) != NUL)
		{
		    যাবত্ (*++that && *that != '"')
		    {
			// skipping escaped characters in the string
			যদি (*that == '\\')
			{
			    যদি (*++that == NUL)
				ভঙ্গ;
			    যদি (that[1] == NUL)
			    {
				++that;
				ভঙ্গ;
			    }
			}
		    }
		    যদি (*that == NUL)
			ভঙ্গ;
		}
		যদি (*that == '(' || *that == '[')
		    ++parencount;
		অন্যথা যদি (*that == ')' || *that == ']')
		    --parencount;
	    }
	    যদি (parencount == 0)
	    {
		amount = get_indent();
		ভঙ্গ;
	    }
	}

	যদি (amount == -1)
	{
	    curwin->w_cursor.lnum = pos->lnum;
	    curwin->w_cursor.col = pos->col;
	    col = pos->col;

	    that = ml_get_curline();

	    যদি (vi_lisp && get_indent() == 0)
		amount = 2;
	    অন্যথা
	    {
		char_u		*সারি = that;
		chartabsize_T	cts;

		init_chartabsize_arg(&cts, curwin, pos->lnum, 0, সারি, সারি);
		যাবত্ (*cts.cts_ptr != NUL && col > 0)
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

		যদি (!vi_lisp && (*that == '(' || *that == '[')
						      && lisp_match(that + 1))
		    amount += 2;
		অন্যথা
		{
		    যদি (*that != NUL)
		    {
			that++;
			amount++;
		    }
		    firsttry = amount;

		    init_chartabsize_arg(&cts, curwin, (colnr_T)(that - সারি),
							   amount, সারি, that);
		    যাবত্ (VIM_ISWHITE(*cts.cts_ptr))
		    {
			cts.cts_vcol += lbr_chartabsize(&cts);
			++cts.cts_ptr;
		    }
		    that = cts.cts_ptr;
		    amount = cts.cts_vcol;
		    clear_chartabsize_arg(&cts);

		    যদি (*that && *that != ';') // not a comment line
		    {
			// test *that != '(' to accommodate first let/do
			// argument if it is more than one line
			যদি (!vi_lisp && *that != '(' && *that != '[')
			    firsttry++;

			parencount = 0;
			quotecount = 0;

			init_chartabsize_arg(&cts, curwin,
				   (colnr_T)(that - সারি), amount, সারি, that);
			যদি (vi_lisp
				|| (*that != '"'
				    && *that != '\''
				    && *that != '#'
				    && (*that < '0' || *that > '9')))
			{
			    যাবত্ (*cts.cts_ptr
				    && (!VIM_ISWHITE(*cts.cts_ptr)
					|| quotecount
					|| parencount)
				    && (!((*cts.cts_ptr == '('
							|| *cts.cts_ptr == '[')
					    && !quotecount
					    && !parencount
					    && vi_lisp)))
			    {
				যদি (*cts.cts_ptr == '"')
				    quotecount = !quotecount;
				যদি ((*cts.cts_ptr == '(' || *cts.cts_ptr == '[')
							       && !quotecount)
				    ++parencount;
				যদি ((*cts.cts_ptr == ')' || *cts.cts_ptr == ']')
							       && !quotecount)
				    --parencount;
				যদি (*cts.cts_ptr == '\\'
						    && *(cts.cts_ptr+1) != NUL)
				    cts.cts_vcol += lbr_chartabsize_adv(&cts);
				cts.cts_vcol += lbr_chartabsize_adv(&cts);
			    }
			}
			যাবত্ (VIM_ISWHITE(*cts.cts_ptr))
			{
			    cts.cts_vcol += lbr_chartabsize(&cts);
			    ++cts.cts_ptr;
			}
			that = cts.cts_ptr;
			amount = cts.cts_vcol;
			clear_chartabsize_arg(&cts);

			যদি (!*that || *that == ';')
			    amount = firsttry;
		    }
		}
	    }
	}
    }
    অন্যথা
	amount = 0;	// no matching '(' or '[' found, use zero indent

    curwin->w_cursor = realpos;

    প্রত্যাগম amount;
}

/*
 * Re-indent the current line, based on the current contents of it and the
 * surrounding lines. Fixing the cursor position seems really easy -- I'm very
 * confused what all the part that handles Control-T is doing that I'm not.
 * "get_the_indent" should be get_c_indent, get_expr_indent or get_lisp_indent.
 */

    শূন্য
fixthisline(পূর্ণাঙ্ক (*get_the_indent)(শূন্য))
{
    পূর্ণাঙ্ক amount = get_the_indent();

    যদি (amount < 0)
	প্রত্যাগম;

    change_indent(INDENT_SET, amount, FALSE, 0, TRUE);
    যদি (linewhite(curwin->w_cursor.lnum))
	did_ai = TRUE;	// delete the indent if the line stays empty
}

/*
 * Return TRUE if 'indentexpr' should be used for Lisp indenting.
 * Caller may want to check 'autoindent'.
 */
    পূর্ণাঙ্ক
use_indentexpr_for_lisp(শূন্য)
{
#ifdef FEAT_EVAL
    প্রত্যাগম curbuf->b_p_lisp
		&& *curbuf->b_p_inde != NUL
		&& STRCMP(curbuf->b_p_lop, "expr:1") == 0;
#অন্যথা
    প্রত্যাগম FALSE;
#endif
}

/*
 * Fix indent for 'lisp' and 'cindent'.
 */
    শূন্য
fix_indent(শূন্য)
{
    যদি (p_paste)
	প্রত্যাগম;  // no auto-indenting when 'paste' is set
    যদি (curbuf->b_p_lisp && curbuf->b_p_ai)
    {
	যদি (use_indentexpr_for_lisp())
	    do_c_রাশিমালা_indent();
	অন্যথা
	    fixthisline(get_lisp_indent);
    }
    অন্যথা যদি (cindent_on())
	do_c_রাশিমালা_indent();
}

#যদি defined(FEAT_EVAL)
/*
 * "indent()" function
 */
    শূন্য
f_indent(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    যদি (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	প্রত্যাগম;

    lnum = tv_get_lnum(argvars);
    যদি (lnum >= 1 && lnum <= curbuf->b_ml.ml_সারি_গুনতি)
	rettv->vval.v_সংখ্যা = get_indent_lnum(lnum);
    অন্যথা
    {
	যদি (in_vim9script())
	    semsg(_(e_invalid_সারি_সংখ্যা_nr), lnum);
	rettv->vval.v_সংখ্যা = -1;
    }
}

/*
 * "lispindent(lnum)" function
 */
    শূন্য
f_lispindent(typval_T *argvars UNUSED, typval_T *rettv)
{
    pos_T	pos;
    linenr_T	lnum;

    যদি (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	প্রত্যাগম;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    যদি (lnum >= 1 && lnum <= curbuf->b_ml.ml_সারি_গুনতি)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_সংখ্যা = get_lisp_indent();
	curwin->w_cursor = pos;
    }
    অন্যথা যদি (in_vim9script())
	semsg(_(e_invalid_সারি_সংখ্যা_nr), lnum);
    অন্যথা
	rettv->vval.v_সংখ্যা = -1;
}
#endif
