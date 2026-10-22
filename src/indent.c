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

#ប្រសិនបើ defined(FEAT_VARTABS)

/*
 * Set the integer values corresponding to the string setting of 'vartabstop'.
 * "array" will be set, caller must free it if needed.
 * Return FAIL for an error.
 */
    ចំនួនគត់
tabstop_set(char_u *var, ចំនួនគត់ **array)
{
    ចំនួនគត់	    valcount = 1;
    ចំនួនគត់	    t;
    char_u  *cp;

    ប្រសិនបើ (var[0] == NUL || (var[0] == '0' && var[1] == NUL))
    {
	*array = NULL;
	ត្រឡប់ OK;
    }

    សម្រាប់ (cp = var; *cp != NUL; ++cp)
    {
	ប្រសិនបើ (cp == var || cp[-1] == ',')
	{
	    char_u *end;

	    ប្រសិនបើ (strtol((var_na *)cp, (var_na **)&end, 10) <= 0)
	    {
		ប្រសិនបើ (cp != end)
		    emsg(_(e_argument_must_be_positive));
		បើមិនដូច្នេះ
		    semsg(_(e_invalid_argument_str), cp);
		ត្រឡប់ FAIL;
	    }
	}

	ប្រសិនបើ (VIM_ISDIGIT(*cp))
	    continue;
	ប្រសិនបើ (cp[0] == ',' && cp > var && cp[-1] != ',' && cp[1] != NUL)
	{
	    ++valcount;
	    continue;
	}
	semsg(_(e_invalid_argument_str), var);
	ត្រឡប់ FAIL;
    }

    *array = ALLOC_MULT(ចំនួនគត់, valcount + 1);
    ប្រសិនបើ (*array == NULL)
	ត្រឡប់ FAIL;
    (*array)[0] = valcount;

    t = 1;
    សម្រាប់ (cp = var; *cp != NUL;)
    {
	ចំនួនគត់ n = atoi((var_na *)cp);

	// Catch negative values, overflow and ridiculous big values.
	ប្រសិនបើ (n <= 0 || n > TABSTOP_MAX)
	{
	    semsg(_(e_invalid_argument_str), cp);
	    VIM_CLEAR(*array);
	    ត្រឡប់ FAIL;
	}
	(*array)[t++] = n;
	ខណៈពេល (*cp != NUL && *cp != ',')
	    ++cp;
	ប្រសិនបើ (*cp != NUL)
	    ++cp;
    }

    ត្រឡប់ OK;
}

/*
 * Calculate the number of screen spaces a tab will occupy.
 * If "vts" is set then the tab widths are taken from that array,
 * otherwise the value of ts is used.
 */
    ចំនួនគត់
tabstop_padding(colnr_T col, ចំនួនគត់ ts_arg, ចំនួនគត់ *vts)
{
    ចំនួនគត់		ts = ts_arg == 0 ? 8 : ts_arg;
    ចំនួនគត់		tabcount;
    colnr_T	tabcol = 0;
    ចំនួនគត់		t;
    ចំនួនគត់		padding = 0;

    ប្រសិនបើ (vts == NULL || vts[0] == 0)
	ត្រឡប់ ts - (col % ts);

    tabcount = vts[0];

    សម្រាប់ (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	ប្រសិនបើ (tabcol > col)
	{
	    padding = (ចំនួនគត់)(tabcol - col);
	    bha_nga;
	}
    }
    ប្រសិនបើ (t > tabcount)
	padding = vts[tabcount] - (ចំនួនគត់)((col - tabcol) % vts[tabcount]);

    ត្រឡប់ padding;
}

/*
 * Find the size of the tab that covers a particular column.
 *
 * If this is being called as part of a shift operation, col is not the cursor
 * column but is the column number to the left of the first non-whitespace
 * character in the line.  If the shift is to the left (left = TRUE), then
 * return the size of the tab interval to the left of the column.
 */
    ចំនួនគត់
tabstop_at(colnr_T col, ចំនួនគត់ ts, ចំនួនគត់ *vts, ចំនួនគត់ left)
{
    ចំនួនគត់		tabcount;	// Number of tab stops in the list of variable
				// tab stops.
    colnr_T	tabcol = 0;	// Column of the tab stop under consideration.
    ចំនួនគត់		t;		// Tabstop index in the list of variable tab
				// stops.
    ចំនួនគត់		tab_size = 0;	// Size of the tab stop interval to the right
				// or left of the col.

    ប្រសិនបើ (vts == 0 || vts[0] == 0)
	ត្រឡប់ ts;

    tabcount = vts[0];
    សម្រាប់ (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	ប្រសិនបើ (tabcol > col)
	{
	    // If shifting left (left != 0), and if the column to the left of
	    // the first first non-blank character (col) in the line is
	    // already to the left of the first tabstop, set the shift amount
	    // (tab_size) to just enough to shift the line to the left margin.
	    // The value doesn't seem to matter as long as it is at least that
	    // distance.
	    ប្រសិនបើ (left && (t == 1))
		tab_size = col;
	    បើមិនដូច្នេះ
		tab_size = vts[t - (left ? 1 : 0)];
	    bha_nga;
	}
    }
    ប្រសិនបើ (t > tabcount)		// If the value of the index t is beyond the
				// end of the list, use the tab stop value at
				// the end of the list.
	tab_size = vts[tabcount];

    ត្រឡប់ tab_size;
}

/*
 * Find the column on which a tab starts.
 */
    colnr_T
tabstop_start(colnr_T col, ចំនួនគត់ ts, ចំនួនគត់ *vts)
{
    ចំនួនគត់		tabcount;
    colnr_T	tabcol = 0;
    ចំនួនគត់		t;
    ចំនួនគត់		excess;

    ប្រសិនបើ (vts == NULL || vts[0] == 0)
	ត្រឡប់ col - col % ts;

    tabcount = vts[0];
    សម្រាប់ (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	ប្រសិនបើ (tabcol > col)
	    ត្រឡប់ tabcol - vts[t];
    }

    excess = tabcol % vts[tabcount];
    ត្រឡប់ col - (col - excess) % vts[tabcount];
}

/*
 * Find the number of tabs and spaces necessary to get from one column
 * to another.
 */
    void
tabstop_fromto(
	colnr_T start_col,
	colnr_T end_col,
	ចំនួនគត់	ts_arg,
	ចំនួនគត់	*vts,
	ចំនួនគត់	*ntabs,
	ចំនួនគត់	*nspcs)
{
    ចំនួនគត់		spaces = end_col - start_col;
    colnr_T	tabcol = 0;
    ចំនួនគត់		padding = 0;
    ចំនួនគត់		tabcount;
    ចំនួនគត់		t;
    ចំនួនគត់		ts = ts_arg == 0 ? curbuf->b_p_ts : ts_arg;

    ប្រសិនបើ (vts == NULL || vts[0] == 0)
    {
	ចំនួនគត់ tabs = 0;
	ចំនួនគត់ initspc = 0;

	initspc = ts - (start_col % ts);
	ប្រសិនបើ (spaces >= initspc)
	{
	    spaces -= initspc;
	    tabs++;
	}
	tabs += spaces / ts;
	spaces -= (spaces / ts) * ts;

	*ntabs = tabs;
	*nspcs = spaces;
	ត្រឡប់;
    }

    // Find the padding needed to reach the next tabstop.
    tabcount = vts[0];
    សម្រាប់ (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	ប្រសិនបើ (tabcol > start_col)
	{
	    padding = (ចំនួនគត់)(tabcol - start_col);
	    bha_nga;
	}
    }
    ប្រសិនបើ (t > tabcount)
	padding = vts[tabcount] - (ចំនួនគត់)((start_col - tabcol) % vts[tabcount]);

    // If the space needed is less than the padding no tabs can be used.
    ប្រសិនបើ (spaces < padding)
    {
	*ntabs = 0;
	*nspcs = spaces;
	ត្រឡប់;
    }

    *ntabs = 1;
    spaces -= padding;

    // At least one tab has been used. See if any more will fit.
    ខណៈពេល (spaces != 0 && ++t <= tabcount)
    {
	padding = vts[t];
	ប្រសិនបើ (spaces < padding)
	{
	    *nspcs = spaces;
	    ត្រឡប់;
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
    static ចំនួនគត់
tabstop_eq(ចំនួនគត់ *ts1, ចំនួនគត់ *ts2)
{
    ចំនួនគត់		t;

    ប្រសិនបើ ((ts1 == 0 && ts2) || (ts1 && ts2 == 0))
	ត្រឡប់ FALSE;
    ប្រសិនបើ (ts1 == ts2)
	ត្រឡប់ TRUE;
    ប្រសិនបើ (ts1[0] != ts2[0])
	ត្រឡប់ FALSE;

    សម្រាប់ (t = 1; t <= ts1[0]; ++t)
	ប្រសិនបើ (ts1[t] != ts2[t])
	    ត្រឡប់ FALSE;

    ត្រឡប់ TRUE;
}

# ប្រសិនបើ defined(FEAT_BEVAL)
/*
 * Copy a tabstop array, allocating space for the new array.
 */
    ចំនួនគត់ *
tabstop_copy(ចំនួនគត់ *oldts)
{
    ចំនួនគត់		*newts;
    ចំនួនគត់		t;

    ប្រសិនបើ (oldts == NULL)
	ត្រឡប់ NULL;
    newts = ALLOC_MULT(ចំនួនគត់, oldts[0] + 1);
    ប្រសិនបើ (newts != NULL)
	សម្រាប់ (t = 0; t <= oldts[0]; ++t)
	    newts[t] = oldts[t];
    ត្រឡប់ newts;
}
# endif

/*
 * Return a count of the number of tabstops.
 */
    ចំនួនគត់
tabstop_count(ចំនួនគត់ *ts)
{
    ត្រឡប់ ts != NULL ? ts[0] : 0;
}

/*
 * Return the first tabstop, or 8 if there are no tabstops defined.
 */
    ចំនួនគត់
tabstop_first(ចំនួនគត់ *ts)
{
    ត្រឡប់ ts != NULL ? ts[1] : 8;
}

#endif

/*
 * Return the effective shiftwidth value for current buffer, using the
 * 'tabstop' value when 'shiftwidth' is zero.
 */
    long
get_sw_value(buf_T *buf)
{
    ត្រឡប់ get_sw_value_col(buf, 0, FALSE);
}

/*
 * Idem, using "pos".
 */
    static long
get_sw_value_pos(buf_T *buf, pos_T *pos, ចំនួនគត់ left)
{
    pos_T save_cursor = curwin->w_cursor;
    long sw_value;

    curwin->w_cursor = *pos;
    sw_value = get_sw_value_col(buf, get_nolist_virtcol(), left);
    curwin->w_cursor = save_cursor;
    ត្រឡប់ sw_value;
}

/*
 * Idem, using the first non-black in the current line.
 */
    long
get_sw_value_indent(buf_T *buf, ចំនួនគត់ left)
{
    pos_T pos = curwin->w_cursor;

    pos.col = getwhitecols_curline();
    ត្រឡប់ get_sw_value_pos(buf, &pos, left);
}

/*
 * Idem, using virtual column "col".
 */
    long
get_sw_value_col(buf_T *buf, colnr_T col UNUSED, ចំនួនគត់ left UNUSED)
{
    ត្រឡប់ buf->b_p_sw ? buf->b_p_sw :
#ifdef FEAT_VARTABS
	tabstop_at(col, buf->b_p_ts, buf->b_p_vts_array, left);
#បើមិនដូច្នេះ
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
    ត្រឡប់ curbuf->b_p_sts < 0 ? get_sw_value(curbuf) : curbuf->b_p_sts;
}

/*
 * Count the size (in window cells) of the indent in the current line.
 */
    ចំនួនគត់
get_indent(void)
{
#ifdef FEAT_VARTABS
    ត្រឡប់ get_indent_str_vtab(ml_get_curline(), (ចំនួនគត់)curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#បើមិនដូច្នេះ
    ត្រឡប់ get_indent_str(ml_get_curline(), (ចំនួនគត់)curbuf->b_p_ts, FALSE);
#endif
}

/*
 * Count the size (in window cells) of the indent in line "lnum".
 */
    ចំនួនគត់
get_indent_lnum(linenr_T lnum)
{
#ifdef FEAT_VARTABS
    ត្រឡប់ get_indent_str_vtab(ml_get(lnum), (ចំនួនគត់)curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#បើមិនដូច្នេះ
    ត្រឡប់ get_indent_str(ml_get(lnum), (ចំនួនគត់)curbuf->b_p_ts, FALSE);
#endif
}

#ប្រសិនបើ defined(FEAT_FOLDING)
/*
 * Count the size (in window cells) of the indent in line "lnum" of buffer
 * "buf".
 */
    ចំនួនគត់
get_indent_buf(buf_T *buf, linenr_T lnum)
{
# ifdef FEAT_VARTABS
    ត្រឡប់ get_indent_str_vtab(ml_get_buf(buf, lnum, FALSE),
			       (ចំនួនគត់)buf->b_p_ts, buf->b_p_vts_array, FALSE);
# បើមិនដូច្នេះ
    ត្រឡប់ get_indent_str(ml_get_buf(buf, lnum, FALSE), (ចំនួនគត់)buf->b_p_ts, FALSE);
# endif
}
#endif

/*
 * count the size (in window cells) of the indent in line "ptr", with
 * 'tabstop' at "ts"
 */
    ចំនួនគត់
get_indent_str(
    char_u	*ptr,
    ចំនួនគត់		ts,
    ចំនួនគត់		no_ts) // if TRUE, count a tab as ^I
{
    ចំនួនគត់		count = 0;

    សម្រាប់ ( ; *ptr; ++ptr)
    {
	ប្រសិនបើ (*ptr == TAB)    // count a tab for what it is worth
	{
	    ប្រសិនបើ (!no_ts)
		count += ts - (count % ts);
	    បើមិនដូច្នេះ
		count += ptr2cells(ptr);
	}
	បើមិនដូច្នេះ ប្រសិនបើ (*ptr == ' ')
	    ++count;		// count a space for one
	បើមិនដូច្នេះ
	    bha_nga;
    }
    ត្រឡប់ count;
}

#ប្រសិនបើ defined(FEAT_VARTABS)
/*
 * Count the size (in window cells) of the indent in line "ptr", using
 * variable tabstops.
 * If "no_ts" is TRUE, count a tab as ^I.
 */
    ចំនួនគត់
get_indent_str_vtab(char_u *ptr, ចំនួនគត់ ts, ចំនួនគត់ *vts, ចំនួនគត់ no_ts)
{
    ចំនួនគត់		count = 0;

    សម្រាប់ ( ; *ptr; ++ptr)
    {
	ប្រសិនបើ (*ptr == TAB)    // count a tab for what it is worth
	{
	    ប្រសិនបើ (!no_ts)
		count += tabstop_padding(count, ts, vts);
	    បើមិនដូច្នេះ
		count += ptr2cells(ptr);
	}
	បើមិនដូច្នេះ ប្រសិនបើ (*ptr == ' ')
	    ++count;		// count a space for one
	បើមិនដូច្នេះ
	    bha_nga;
    }
    ត្រឡប់ count;
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
    ចំនួនគត់
set_indent(
    ចំនួនគត់		size,		    // measured in spaces
    ចំនួនគត់		flags)
{
    char_u	*p;
    char_u	*newline;
    char_u	*oldline;
    char_u	*s;
    ចំនួនគត់		todo;
    ចំនួនគត់		ind_len;	    // measured in characters
    ចំនួនគត់		line_len;	    // size of the line (including the NUL)
    ចំនួនគត់		doit = FALSE;
    ចំនួនគត់		ind_done = 0;	    // measured in spaces
#ifdef FEAT_VARTABS
    ចំនួនគត់		ind_col = 0;
#endif
    ចំនួនគត់		tab_pad;
    ចំនួនគត់		retval = FALSE;
    ចំនួនគត់		orig_char_len = -1; // number of initial whitespace chars when
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
    ប្រសិនបើ (!curbuf->b_p_et || (!(flags & SIN_INSERT) && curbuf->b_p_pi))
    {
	// If 'preserveindent' is set then reuse as much as possible of
	// the existing indent structure for the new indent
	ប្រសិនបើ (!(flags & SIN_INSERT) && curbuf->b_p_pi)
	{
	    ind_done = 0;

	    // count as many characters as we can use
	    ខណៈពេល (todo > 0 && VIM_ISWHITE(*p))
	    {
		ប្រសិនបើ (*p == TAB)
		{
#ifdef FEAT_VARTABS
		    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#បើមិនដូច្នេះ
		    tab_pad = (ចំនួនគត់)curbuf->b_p_ts
					   - (ind_done % (ចំនួនគត់)curbuf->b_p_ts);
#endif
		    // stop if this tab will overshoot the target
		    ប្រសិនបើ (todo < tab_pad)
			bha_nga;
		    todo -= tab_pad;
		    ++ind_len;
		    ind_done += tab_pad;
		}
		បើមិនដូច្នេះ
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
	    ប្រសិនបើ (curbuf->b_p_et)
		orig_char_len = ind_len;

	    // Fill to next tabstop with a tab, if possible
#ifdef FEAT_VARTABS
	    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
						curbuf->b_p_vts_array);
#បើមិនដូច្នេះ
	    tab_pad = (ចំនួនគត់)curbuf->b_p_ts - (ind_done % (ចំនួនគត់)curbuf->b_p_ts);
#endif
	    ប្រសិនបើ (todo >= tab_pad && orig_char_len == -1)
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
	សម្រាប់ (;;)
	{
	    tab_pad = tabstop_padding(ind_col, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
	    ប្រសិនបើ (todo < tab_pad)
		bha_nga;
	    ប្រសិនបើ (*p != TAB)
		doit = TRUE;
	    បើមិនដូច្នេះ
		++p;
	    todo -= tab_pad;
	    ++ind_len;
	    ind_col += tab_pad;
	}
#បើមិនដូច្នេះ
	ខណៈពេល (todo >= (ចំនួនគត់)curbuf->b_p_ts)
	{
	    ប្រសិនបើ (*p != TAB)
		doit = TRUE;
	    បើមិនដូច្នេះ
		++p;
	    todo -= (ចំនួនគត់)curbuf->b_p_ts;
	    ++ind_len;
	    // ind_done += (int)curbuf->b_p_ts;
	}
#endif
    }
    // count spaces required for indent
    ខណៈពេល (todo > 0)
    {
	ប្រសិនបើ (*p != ' ')
	    doit = TRUE;
	បើមិនដូច្នេះ
	    ++p;
	--todo;
	++ind_len;
	// ++ind_done;
    }

    // Return if the indent is OK already.
    ប្រសិនបើ (!doit && !VIM_ISWHITE(*p) && !(flags & SIN_INSERT))
	ត្រឡប់ FALSE;

    // Allocate memory for the new line.
    ប្រសិនបើ (flags & SIN_INSERT)
	p = oldline;
    បើមិនដូច្នេះ
    {
	p = skipwhite(p);
	line_len -= (ចំនួនគត់)(p - oldline);
    }

    // If 'preserveindent' and 'expandtab' are both set keep the original
    // characters and allocate accordingly.  We will fill the rest with spaces
    // after the if (!curbuf->b_p_et) below.
    ប្រសិនបើ (orig_char_len != -1)
    {
	newline = alloc(orig_char_len + size - ind_done + line_len);
	ប្រសិនបើ (newline == NULL)
	    ត្រឡប់ FALSE;
	todo = size - ind_done;
	ind_len = orig_char_len + todo;    // Set total length of indent in
					   // characters, which may have been
					   // undercounted until now
	p = oldline;
	s = newline;
	ខណៈពេល (orig_char_len > 0)
	{
	    *s++ = *p++;
	    orig_char_len--;
	}

	// Skip over any additional white space (useful when newindent is less
	// than old)
	ខណៈពេល (VIM_ISWHITE(*p))
	    ++p;

    }
    បើមិនដូច្នេះ
    {
	todo = size;
	newline = alloc(ind_len + line_len);
	ប្រសិនបើ (newline == NULL)
	    ត្រឡប់ FALSE;
	s = newline;
    }

    // Put the characters in the new line.
    // if 'expandtab' isn't set: use TABs
    ប្រសិនបើ (!curbuf->b_p_et)
    {
	// If 'preserveindent' is set then reuse as much as possible of
	// the existing indent structure for the new indent
	ប្រសិនបើ (!(flags & SIN_INSERT) && curbuf->b_p_pi)
	{
	    p = oldline;
	    ind_done = 0;

	    ខណៈពេល (todo > 0 && VIM_ISWHITE(*p))
	    {
		ប្រសិនបើ (*p == TAB)
		{
#ifdef FEAT_VARTABS
		    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#បើមិនដូច្នេះ
		    tab_pad = (ចំនួនគត់)curbuf->b_p_ts
					   - (ind_done % (ចំនួនគត់)curbuf->b_p_ts);
#endif
		    // stop if this tab will overshoot the target
		    ប្រសិនបើ (todo < tab_pad)
			bha_nga;
		    todo -= tab_pad;
		    ind_done += tab_pad;
		}
		បើមិនដូច្នេះ
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
#បើមិនដូច្នេះ
	    tab_pad = (ចំនួនគត់)curbuf->b_p_ts - (ind_done % (ចំនួនគត់)curbuf->b_p_ts);
#endif
	    ប្រសិនបើ (todo >= tab_pad)
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
	សម្រាប់ (;;)
	{
	    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
	    ប្រសិនបើ (todo < tab_pad)
		bha_nga;
	    *s++ = TAB;
	    todo -= tab_pad;
	    ind_done += tab_pad;
	}
#បើមិនដូច្នេះ
	ខណៈពេល (todo >= (ចំនួនគត់)curbuf->b_p_ts)
	{
	    *s++ = TAB;
	    todo -= (ចំនួនគត់)curbuf->b_p_ts;
	}
#endif
    }
    ខណៈពេល (todo > 0)
    {
	*s++ = ' ';
	--todo;
    }
    mch_memmove(s, p, (size_t)line_len);

    // Replace the line (unless undo fails).
    ប្រសិនបើ (!(flags & SIN_UNDO) || u_savesub(curwin->w_cursor.lnum) == OK)
    {
	colnr_T old_offset = (colnr_T)(p - oldline);
	colnr_T new_offset = (colnr_T)(s - newline);

	// this may free "newline"
	ml_replace(curwin->w_cursor.lnum, newline, FALSE);
	ប្រសិនបើ (flags & SIN_CHANGED)
	    changed_bytes(curwin->w_cursor.lnum, 0);

	// Correct saved cursor position if it is in this line.
	ប្រសិនបើ (saved_cursor.lnum == curwin->w_cursor.lnum)
	{
	    ប្រសិនបើ (saved_cursor.col >= old_offset)
		// cursor was after the indent, adjust for the number of
		// bytes added/removed
		saved_cursor.col += ind_len - old_offset;
	    បើមិនដូច្នេះ ប្រសិនបើ (saved_cursor.col >= new_offset)
		// cursor was in the indent, and is now after it, put it back
		// at the start of the indent (replacing spaces with TAB)
		saved_cursor.col = new_offset;
	}
#ifdef FEAT_PROP_POPUP
	ប្រសិនបើ (curbuf->b_has_textprop)
	{
	    ចំនួនគត់ added = ind_len - old_offset;

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
    បើមិនដូច្នេះ
	vim_free(newline);

    curwin->w_cursor.col = ind_len;
    ត្រឡប់ retval;
}

/*
 * Return the indent of the current line after a number.  Return -1 if no
 * number was found.  Used for 'n' in 'formatoptions': numbered list.
 * Since a pattern is used it can actually handle more than numbers.
 */
    ចំនួនគត់
get_number_indent(linenr_T lnum)
{
    colnr_T	col;
    pos_T	pos;

    regmatch_T	regmatch;
    ចំនួនគត់		lead_len = 0;	// length of comment leader

    ប្រសិនបើ (lnum > curbuf->b_ml.ml_line_count)
	ត្រឡប់ -1;
    pos.lnum = 0;

    // In format_lines() (i.e. not insert mode), fo+=q is needed too...
    ប្រសិនបើ ((State & MODE_INSERT) || has_format_option(FO_Q_COMS))
	lead_len = get_leader_len(ml_get(lnum), NULL, FALSE, TRUE);

    regmatch.regprog = vim_regcomp(curbuf->b_p_flp, RE_MAGIC);
    ប្រសិនបើ (regmatch.regprog != NULL)
    {
	regmatch.rm_ic = FALSE;

	// vim_regexec() expects a pointer to a line.  This lets us
	// start matching for the flp beyond any comment leader...
	ប្រសិនបើ (vim_regexec(&regmatch, ml_get(lnum) + lead_len, (colnr_T)0))
	{
	    pos.lnum = lnum;
	    pos.col = (colnr_T)(*regmatch.endp - ml_get(lnum));
	    pos.coladd = 0;
	}
	vim_regfree(regmatch.regprog);
    }

    ប្រសិនបើ (pos.lnum == 0 || *ml_get_pos(&pos) == NUL)
	ត្រឡប់ -1;
    getvcol(curwin, &pos, &col, NULL, NULL, 0);
    ត្រឡប់ (ចំនួនគត់)col;
}

#ប្រសិនបើ defined(FEAT_LINEBREAK)
/*
 * Check "briopt" as 'breakindentopt' and update the members of "wp".
 * This is called when 'breakindentopt' is changed and when a window is
 * initialized.
 * Returns FAIL for failure, OK otherwise.
 */
    ចំនួនគត់
briopt_check(
    char_u	*briopt,	// when NULL: use "wp->w_p_briopt"
    win_T	*wp)		// when NULL: only check "briopt"
{
    char_u	*p;
    ចំនួនគត់		bri_shift = 0;
    long	bri_min = 20;
    ចំនួនគត់		bri_sbr = FALSE;
    ចំនួនគត់		bri_list = 0;
    ចំនួនគត់		bri_vcol = 0;

    ប្រសិនបើ (briopt != NULL)
	p = briopt;
    បើមិនដូច្នេះ
	p = wp->w_p_briopt;

    ខណៈពេល (*p != NUL)
    {
	// Note: Keep this in sync with p_briopt_values
	ប្រសិនបើ (STRNCMP(p, "shift:", 6) == 0
		 && ((p[6] == '-' && VIM_ISDIGIT(p[7])) || VIM_ISDIGIT(p[6])))
	{
	    p += 6;
	    bri_shift = getdigits(&p);
	}
	បើមិនដូច្នេះ ប្រសិនបើ (STRNCMP(p, "min:", 4) == 0 && VIM_ISDIGIT(p[4]))
	{
	    p += 4;
	    bri_min = getdigits(&p);
	}
	បើមិនដូច្នេះ ប្រសិនបើ (STRNCMP(p, "sbr", 3) == 0)
	{
	    p += 3;
	    bri_sbr = TRUE;
	}
	បើមិនដូច្នេះ ប្រសិនបើ (STRNCMP(p, "list:", 5) == 0)
	{
	    p += 5;
	    bri_list = getdigits(&p);
	}
	បើមិនដូច្នេះ ប្រសិនបើ (STRNCMP(p, "column:", 7) == 0)
	{
	    p += 7;
	    bri_vcol = getdigits(&p);
	}
	ប្រសិនបើ (*p != ',' && *p != NUL)
	    ត្រឡប់ FAIL;
	ប្រសិនបើ (*p == ',')
	    ++p;
    }

    ប្រសិនបើ (wp == NULL)
	ត្រឡប់ OK;

    wp->w_briopt_shift = bri_shift;
    wp->w_briopt_min   = bri_min;
    wp->w_briopt_sbr   = bri_sbr;
    wp->w_briopt_list  = bri_list;
    wp->w_briopt_vcol  = bri_vcol;

    ត្រឡប់ OK;
}

/*
 * Return appropriate space number for breakindent, taking influencing
 * parameters into account. Window must be specified, since it is not
 * necessarily always the current one.
 */
    ចំនួនគត់
get_breakindent_win(
    win_T	*wp,
    char_u	*line) // start of the line
{
    static ចំនួនគត់	    prev_indent = 0;	// cached indent value
    static long	    prev_ts     = 0L;	// cached tabstop value
# ifdef FEAT_VARTABS
    static ចំនួនគត់      *prev_vts = NULL;   // cached vartabs values
# endif
    static ចំនួនគត់	    prev_fnum   = 0;	// cached buffer number
    static char_u   *prev_line  = NULL;	// cached copy of "line"
    static varnumber_T prev_tick = 0;   // changedtick of cached value
    static ចំនួនគត់      prev_list = 0;	// cached list indent
    static ចំនួនគត់      prev_listopt = 0;	// cached w_p_briopt_list value
    static ចំនួនគត់      prev_no_ts = FALSE;	// cached no_ts value
    static unsigned prev_dy_uhex = 0;	// cached 'display' "uhex" value
    // cached formatlistpat value
    static char_u   *prev_flp = NULL;
    ចំនួនគត់		    bri = 0;
    // window width minus window margin space, i.e. what rests for text
    const ចំនួនគត់	    eff_wwidth = wp->w_width
					  - win_col_off(wp) + win_col_off2(wp);

    // In list mode, if 'listchars' "tab" isn't set, a TAB is displayed as ^I.
    ចំនួនគត់ no_ts = wp->w_p_list && wp->w_lcs_chars.tab1 == NUL;

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
    ប្រសិនបើ (prev_fnum != wp->w_buffer->b_fnum
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
	ប្រសិនបើ (wp->w_briopt_vcol == 0)
	    prev_indent = get_indent_str_vtab(line,
				     (ចំនួនគត់)wp->w_buffer->b_p_ts,
				      wp->w_buffer->b_p_vts_array, no_ts);
# បើមិនដូច្នេះ
	ប្រសិនបើ (wp->w_briopt_vcol == 0)
	    prev_indent = get_indent_str(line,
					(ចំនួនគត់)wp->w_buffer->b_p_ts, no_ts);
# endif
	prev_tick = CHANGEDTICK(wp->w_buffer);
	prev_listopt = wp->w_briopt_list;
	prev_list = 0;
	prev_no_ts = no_ts;
	prev_dy_uhex = (dy_flags & DY_UHEX);
	vim_free(prev_flp);
	prev_flp = vim_strsave(get_flp_value(wp->w_buffer));
	// add additional indent for numbered lists
	ប្រសិនបើ (wp->w_briopt_list != 0 && wp->w_briopt_vcol == 0)
	{
	    regmatch_T	    regmatch;

	    regmatch.regprog = vim_regcomp(prev_flp,
				   RE_MAGIC + RE_STRING + RE_AUTO + RE_STRICT);

	    ប្រសិនបើ (regmatch.regprog != NULL)
	    {
		regmatch.rm_ic = FALSE;
		ប្រសិនបើ (vim_regexec(&regmatch, line, 0))
		{
		    ប្រសិនបើ (wp->w_briopt_list > 0)
			prev_list = wp->w_briopt_list;
		    បើមិនដូច្នេះ
		    {
			char_u	*ptr = *regmatch.startp;
			char_u	*end_ptr = *regmatch.endp;
			ចំនួនគត់	indent = 0;

			// Compute the width of the matched text.
			// Use win_chartabsize() so that TAB size is correct,
			// while wrapping is ignored.
			ខណៈពេល (ptr < end_ptr)
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
    ប្រសិនបើ (wp->w_briopt_vcol != 0)
    {
	// column value has priority
	bri = wp->w_briopt_vcol;
	prev_list = 0;
    }
    បើមិនដូច្នេះ
	bri = prev_indent + wp->w_briopt_shift;

    // Add offset for number column, if 'n' is in 'cpoptions'
    bri += win_col_off2(wp);

    // add additional indent for numbered lists
    ប្រសិនបើ (wp->w_briopt_list > 0)
	bri += prev_list;

    // indent minus the length of the showbreak string
    ប្រសិនបើ (wp->w_briopt_sbr)
	bri -= vim_strsize(get_showbreak_value(wp));


    // never indent past left window margin
    ប្រសិនបើ (bri < 0)
	bri = 0;

    // always leave at least bri_min characters on the left,
    // if text width is sufficient
    បើមិនដូច្នេះ ប្រសិនបើ (bri > eff_wwidth - wp->w_briopt_min)
	bri = (eff_wwidth - wp->w_briopt_min < 0)
					   ? 0 : eff_wwidth - wp->w_briopt_min;

    ត្រឡប់ bri;
}
#endif

/*
 * When extra == 0: Return TRUE if the cursor is before or on the first
 *		    non-blank in the line.
 * When extra == 1: Return TRUE if the cursor is before the first non-blank in
 *		    the line.
 */
    ចំនួនគត់
inindent(ចំនួនគត់ extra)
{
    char_u	*ptr;
    colnr_T	col;

    សម្រាប់ (col = 0, ptr = ml_get_curline(); VIM_ISWHITE(*ptr); ++col)
	++ptr;
    ប្រសិនបើ (col >= curwin->w_cursor.col + extra)
	ត្រឡប់ TRUE;
    បើមិនដូច្នេះ
	ត្រឡប់ FALSE;
}

/*
 * op_reindent - handle reindenting a block of lines.
 */
    void
op_reindent(oparg_T *oap, ចំនួនគត់ (*how)(void))
{
    long	i = 0;
    char_u	*l;
    ចំនួនគត់		amount;
    linenr_T	first_changed = 0;
    linenr_T	last_changed = 0;
    linenr_T	start_lnum = curwin->w_cursor.lnum;

    // Don't even try when 'modifiable' is off.
    ប្រសិនបើ (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	ត្រឡប់;
    }

    // Save for undo.  Do this once for all lines, much faster than doing this
    // for each line separately, especially when undoing.
    ប្រសិនបើ (u_savecommon(start_lnum - 1, start_lnum + oap->line_count,
				     start_lnum + oap->line_count, FALSE) == OK)
	សម្រាប់ (i = oap->line_count; --i >= 0 && !got_int; )
	{
	    // it's a slow thing to do, so give feedback so there's no worry
	    // that the computer's just hung.

	    ប្រសិនបើ (i > 1
		    && (i % 50 == 0 || i == oap->line_count - 1)
		    && oap->line_count > p_report)
		smsg(_("%ld lines to indent... "), i);

	    // Be vi-compatible: For lisp indenting the first line is not
	    // indented, unless there is only one line.
	    ប្រសិនបើ (i != oap->line_count - 1 || oap->line_count == 1
						     || how != get_lisp_indent)
	    {
		l = skipwhite(ml_get_curline());
		ប្រសិនបើ (*l == NUL)		    // empty or blank line
		    amount = 0;
		បើមិនដូច្នេះ
		    amount = how();	    // get the indent for this line

		ប្រសិនបើ (amount >= 0 && set_indent(amount, 0))
		{
		    // did change the indent, call changed_lines() later
		    ប្រសិនបើ (first_changed == 0)
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
    ប្រសិនបើ (last_changed != 0)
	changed_lines(first_changed, 0,
		oap->is_VIsual ? start_lnum + oap->line_count :
		last_changed + 1, 0L);
    បើមិនដូច្នេះ ប្រសិនបើ (oap->is_VIsual)
	redraw_curbuf_later(UPD_INVERTED);

    ប្រសិនបើ (oap->line_count > p_report)
    {
	i = oap->line_count - (i + 1);
	smsg(NGETTEXT("%ld line indented ",
						 "%ld lines indented ", i), i);
    }
    ប្រសិនបើ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// set '[ and '] marks
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
    }
}

/*
 * Return TRUE if lines starting with '#' should be left aligned.
 */
    ចំនួនគត់
preprocs_left(void)
{
    ត្រឡប់
	(curbuf->b_p_si && !curbuf->b_p_cin) ||
	(curbuf->b_p_cin && in_cinkeys('#', ' ', TRUE)
					   && curbuf->b_ind_hash_comment == 0)
	;
}

/*
 * Return TRUE if the conditions are OK for smart indenting.
 */
    ចំនួនគត់
may_do_si(void)
{
    ត្រឡប់ curbuf->b_p_si
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
ins_try_si(ចំនួនគត់ c)
{
    pos_T	*pos, old_pos;
    char_u	*ptr;
    ចំនួនគត់		i;
    ចំនួនគត់		temp;

    // do some very smart indenting when entering '{' or '}'
    ប្រសិនបើ (((did_si || can_si_back) && c == '{')
	    || (can_si && c == '}' && inindent(0)))
    {
	// for '}' set indent equal to indent of line containing matching '{'
	ប្រសិនបើ (c == '}' && (pos = findmatch(NULL, '{')) != NULL)
	{
	    old_pos = curwin->w_cursor;
	    // If the matching '{' has a ')' immediately before it (ignoring
	    // white-space), then line up with the start of the line
	    // containing the matching '(' if there is one.  This handles the
	    // case where an "if (..\n..) {" statement continues over multiple
	    // lines -- webb
	    ptr = ml_get(pos->lnum);
	    i = pos->col;
	    ប្រសិនបើ (i > 0)		// skip blanks before '{'
		ខណៈពេល (--i > 0 && VIM_ISWHITE(ptr[i]))
		    ;
	    curwin->w_cursor.lnum = pos->lnum;
	    curwin->w_cursor.col = i;
	    ប្រសិនបើ (ptr[i] == ')' && (pos = findmatch(NULL, '(')) != NULL)
		curwin->w_cursor = *pos;
	    i = get_indent();
	    curwin->w_cursor = old_pos;
	    ប្រសិនបើ (State & VREPLACE_FLAG)
		change_indent(INDENT_SET, i, FALSE, NUL, TRUE);
	    បើមិនដូច្នេះ
		(void)set_indent(i, SIN_CHANGED);
	}
	បើមិនដូច្នេះ ប្រសិនបើ (curwin->w_cursor.col > 0)
	{
	    // when inserting '{' after "O" reduce indent, but not
	    // more than indent of previous line
	    temp = TRUE;
	    ប្រសិនបើ (c == '{' && can_si_back && curwin->w_cursor.lnum > 1)
	    {
		old_pos = curwin->w_cursor;
		i = get_indent();
		ខណៈពេល (curwin->w_cursor.lnum > 1)
		{
		    ptr = skipwhite(ml_get(--(curwin->w_cursor.lnum)));

		    // ignore empty lines and lines starting with '#'.
		    ប្រសិនបើ (*ptr != '#' && *ptr != NUL)
			bha_nga;
		}
		ប្រសិនបើ (get_indent() >= i)
		    temp = FALSE;
		curwin->w_cursor = old_pos;
	    }
	    ប្រសិនបើ (temp)
		shift_line(TRUE, FALSE, 1, TRUE);
	}
    }

    // set indent of '#' always to 0
    ប្រសិនបើ (curwin->w_cursor.col > 0 && can_si && c == '#' && inindent(0))
    {
	// remember current indent for next line
	old_indent = get_indent();
	(void)set_indent(0, SIN_CHANGED);
    }

    // Adjust ai_col, the char at this position can be deleted.
    ប្រសិនបើ (ai_col > curwin->w_cursor.col)
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
    ចំនួនគត់		type,
    ចំនួនគត់		amount,
    ចំនួនគត់		round,
    ចំនួនគត់		replaced,	// replaced character, put on replace stack
    ចំនួនគត់		call_changed_bytes)	// call changed_bytes()
{
    ចំនួនគត់		vcol;
    ចំនួនគត់		last_vcol;
    ចំនួនគត់		insstart_less;		// reduction for Insstart.col
    ចំនួនគត់		new_cursor_col;
    ចំនួនគត់		i;
    char_u	*ptr;
    ចំនួនគត់		save_p_list;
    ចំនួនគត់		start_col;
    colnr_T	vc;
    colnr_T	orig_col = 0;		// init for GCC
    char_u	*new_line, *orig_line = NULL;	// init for GCC

    // MODE_VREPLACE state needs to know what the line was like before changing
    ប្រសិនបើ (State & VREPLACE_FLAG)
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
    ប្រសិនបើ (new_cursor_col < 0)
	vcol = get_indent() - vcol;

    ប្រសិនបើ (new_cursor_col > 0)	    // can't fix replace stack
	start_col = -1;

    // Set the new indent.  The cursor will be put on the first non-blank.
    ប្រសិនបើ (type == INDENT_SET)
	(void)set_indent(amount, call_changed_bytes ? SIN_CHANGED : 0);
    បើមិនដូច្នេះ
    {
	ចំនួនគត់	save_State = State;

	// Avoid being called recursively.
	ប្រសិនបើ (State & VREPLACE_FLAG)
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
    ប្រសិនបើ (new_cursor_col >= 0)
    {
	// When changing the indent while the cursor is touching it, reset
	// Insstart_col to 0.
	ប្រសិនបើ (new_cursor_col == 0)
	    insstart_less = MAXCOL;
	new_cursor_col += curwin->w_cursor.col;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (!(State & MODE_INSERT))
	new_cursor_col = curwin->w_cursor.col;
    បើមិនដូច្នេះ
    {
	chartabsize_T cts;

	// Compute the screen column where the cursor should be.
	vcol = get_indent() - vcol;
	curwin->w_virtcol = (colnr_T)((vcol < 0) ? 0 : vcol);

	// Advance the cursor until we reach the right screen column.
	last_vcol = 0;
	ptr = ml_get_curline();
	init_chartabsize_arg(&cts, curwin, 0, 0, ptr, ptr);
	ខណៈពេល (cts.cts_vcol <= (ចំនួនគត់)curwin->w_virtcol)
	{
	    last_vcol = cts.cts_vcol;
	    ប្រសិនបើ (cts.cts_vcol > 0)
		MB_PTR_ADV(cts.cts_ptr);
	    ប្រសិនបើ (*cts.cts_ptr == NUL)
		bha_nga;
	    cts.cts_vcol += lbr_chartabsize(&cts);
	}
	vcol = last_vcol;
	new_cursor_col = cts.cts_ptr - cts.cts_line;
	clear_chartabsize_arg(&cts);

	// May need to insert spaces to be able to position the cursor on
	// the right screen column.
	ប្រសិនបើ (vcol != (ចំនួនគត់)curwin->w_virtcol)
	{
	    curwin->w_cursor.col = (colnr_T)new_cursor_col;
	    i = (ចំនួនគត់)curwin->w_virtcol - vcol;
	    ptr = alloc(i + 1);
	    ប្រសិនបើ (ptr != NULL)
	    {
		size_t ptrlen;
		new_cursor_col += i;
		ptr[i] = NUL;
		ptrlen = i;
		ខណៈពេល (--i >= 0)
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

    ប្រសិនបើ (new_cursor_col <= 0)
	curwin->w_cursor.col = 0;
    បើមិនដូច្នេះ
	curwin->w_cursor.col = (colnr_T)new_cursor_col;
    curwin->w_set_curswant = true;
    changed_cline_bef_curs();

    // May have to adjust the start of the insert.
    ប្រសិនបើ (State & MODE_INSERT)
    {
	ប្រសិនបើ (curwin->w_cursor.lnum == Insstart.lnum && Insstart.col != 0)
	{
	    ប្រសិនបើ ((ចំនួនគត់)Insstart.col <= insstart_less)
		Insstart.col = 0;
	    បើមិនដូច្នេះ
		Insstart.col -= insstart_less;
	}
	ប្រសិនបើ ((ចំនួនគត់)ai_col <= insstart_less)
	    ai_col = 0;
	បើមិនដូច្នេះ
	    ai_col -= insstart_less;
    }

    // For MODE_REPLACE state, may have to fix the replace stack, if it's
    // possible.  If the number of characters before the cursor decreased, need
    // to pop a few characters from the replace stack.
    // If the number of characters before the cursor increased, need to push a
    // few NULs onto the replace stack.
    ប្រសិនបើ (REPLACE_NORMAL(State) && start_col >= 0)
    {
	ខណៈពេល (start_col > (ចំនួនគត់)curwin->w_cursor.col)
	{
	    replace_join(0);	    // remove a NUL from the replace stack
	    --start_col;
	}
	ខណៈពេល (start_col < (ចំនួនគត់)curwin->w_cursor.col || replaced)
	{
	    replace_push(NUL);
	    ប្រសិនបើ (replaced)
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
    ប្រសិនបើ (State & VREPLACE_FLAG)
    {
	// If orig_line didn't allocate, just return.  At least we did the job,
	// even if you can't backspace.
	ប្រសិនបើ (orig_line == NULL)
	    ត្រឡប់;

	// Save new line
	new_line = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	ប្រសិនបើ (new_line == NULL)
	{
	    vim_free(orig_line);
	    ត្រឡប់;
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
    ចំនួនគត់
copy_indent(ចំនួនគត់ size, char_u *src)
{
    char_u	*p = NULL;
    char_u	*line = NULL;
    char_u	*s;
    ចំនួនគត់		todo;
    ចំនួនគត់		ind_len;
    ចំនួនគត់		line_len = 0;
    ចំនួនគត់		tab_pad;
    ចំនួនគត់		ind_done;
    ចំនួនគត់		round;
#ifdef FEAT_VARTABS
    ចំនួនគត់		ind_col;
#endif

    // Round 1: compute the number of characters needed for the indent
    // Round 2: copy the characters.
    សម្រាប់ (round = 1; round <= 2; ++round)
    {
	todo = size;
	ind_len = 0;
	ind_done = 0;
#ifdef FEAT_VARTABS
	ind_col = 0;
#endif
	s = src;

	// Count/copy the usable portion of the source line
	ខណៈពេល (todo > 0 && VIM_ISWHITE(*s))
	{
	    ប្រសិនបើ (*s == TAB)
	    {
#ifdef FEAT_VARTABS
		tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#បើមិនដូច្នេះ
		tab_pad = (ចំនួនគត់)curbuf->b_p_ts
					   - (ind_done % (ចំនួនគត់)curbuf->b_p_ts);
#endif
		// Stop if this tab will overshoot the target
		ប្រសិនបើ (todo < tab_pad)
		    bha_nga;
		todo -= tab_pad;
		ind_done += tab_pad;
#ifdef FEAT_VARTABS
		ind_col += tab_pad;
#endif
	    }
	    បើមិនដូច្នេះ
	    {
		--todo;
		++ind_done;
#ifdef FEAT_VARTABS
		++ind_col;
#endif
	    }
	    ++ind_len;
	    ប្រសិនបើ (p != NULL)
		*p++ = *s;
	    ++s;
	}

	// Fill to next tabstop with a tab, if possible
#ifdef FEAT_VARTABS
	tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#បើមិនដូច្នេះ
	tab_pad = (ចំនួនគត់)curbuf->b_p_ts - (ind_done % (ចំនួនគត់)curbuf->b_p_ts);
#endif
	ប្រសិនបើ (todo >= tab_pad && !curbuf->b_p_et)
	{
	    todo -= tab_pad;
	    ++ind_len;
#ifdef FEAT_VARTABS
	    ind_col += tab_pad;
#endif
	    ប្រសិនបើ (p != NULL)
		*p++ = TAB;
	}

	// Add tabs required for indent
	ប្រសិនបើ (!curbuf->b_p_et)
	{
#ifdef FEAT_VARTABS
	    សម្រាប់ (;;)
	    {
		tab_pad = tabstop_padding(ind_col, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
		ប្រសិនបើ (todo < tab_pad)
		    bha_nga;
		todo -= tab_pad;
		++ind_len;
		ind_col += tab_pad;
		ប្រសិនបើ (p != NULL)
		    *p++ = TAB;
	    }
#បើមិនដូច្នេះ
	    ខណៈពេល (todo >= (ចំនួនគត់)curbuf->b_p_ts)
	    {
		todo -= (ចំនួនគត់)curbuf->b_p_ts;
		++ind_len;
		ប្រសិនបើ (p != NULL)
		    *p++ = TAB;
	    }
#endif
	}

	// Count/add spaces required for indent
	ខណៈពេល (todo > 0)
	{
	    --todo;
	    ++ind_len;
	    ប្រសិនបើ (p != NULL)
		*p++ = ' ';
	}

	ប្រសិនបើ (p == NULL)
	{
	    // Allocate memory for the result: the copied indent, new indent
	    // and the rest of the line.
	    line_len = ml_get_curline_len() + 1;
	    line = alloc(ind_len + line_len);
	    ប្រសិនបើ (line == NULL)
		ត្រឡប់ FALSE;
	    p = line;
	}
    }

    // Append the original line
    mch_memmove(p, ml_get_curline(), (size_t)line_len);

    // Replace the line
    ml_replace(curwin->w_cursor.lnum, line, FALSE);

    // Put the cursor after the indent.
    curwin->w_cursor.col = ind_len;
    ត្រឡប់ TRUE;
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
    ប្រសិនបើ (trylevel == 0)
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
    ចំនួនគត់		got_tab = FALSE;
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
    ចំនួនគត់		did_undo;		// called u_save for current line
#ifdef FEAT_VARTABS
    ចំនួនគត់		*new_vts_array = NULL;
    char_u	*new_ts_str;		// string value of tab argument
#បើមិនដូច្នេះ
    ចំនួនគត់		temp;
    ចំនួនគត់		new_ts = 0;
#endif
    ចំនួនគត់		save_list;
    linenr_T	first_line = 0;		// first changed line
    linenr_T	last_line = 0;		// last changed line
    ចំនួនគត់		is_indent_only = 0;	// Only process leading whitespace

    save_list = curwin->w_p_list;
    curwin->w_p_list = 0;	    // don't want list mode here

    ptr = eap->arg;
    ប្រសិនបើ (STRNCMP(ptr, "-indentonly", 11) == 0 && IS_WHITE_OR_NUL(ptr[11]))
    {
	is_indent_only = 1;
	ptr = skipwhite(ptr + 11);
    }

#ifdef FEAT_VARTABS
    new_ts_str = ptr;
    ប្រសិនបើ (tabstop_set(ptr, &new_vts_array) == FAIL)
	ត្រឡប់;
    ខណៈពេល (vim_isdigit(*ptr) || *ptr == ',')
	++ptr;

    // This ensures that either new_vts_array and new_ts_str are freshly
    // allocated, or new_vts_array points to an existing array and new_ts_str
    // is null.
    ប្រសិនបើ (new_vts_array == NULL)
    {
	new_vts_array = curbuf->b_p_vts_array;
	new_ts_str = NULL;
    }
    បើមិនដូច្នេះ
	new_ts_str = vim_strnsave(new_ts_str, ptr - new_ts_str);
#បើមិនដូច្នេះ
    ប្រសិនបើ (ptr[0] != NUL && (ptr[0] != '0' || ptr[1] != NUL))
    {
	char_u	*end;

	ប្រសិនបើ (strtol((var_na *)ptr, (var_na **)&end, 10) <= 0)
	{
	    ប្រសិនបើ (ptr != end)
		emsg(_(e_argument_must_be_positive));
	    បើមិនដូច្នេះ
		semsg(_(e_invalid_argument_str), ptr);
	    ត្រឡប់;
	}
	new_ts = getdigits(&ptr);
	ប្រសិនបើ (new_ts < 0 || new_ts > TABSTOP_MAX)
	{
	    semsg(_(e_invalid_argument_str), eap->arg);
	    ត្រឡប់;
	}
    }
    ប្រសិនបើ (new_ts == 0)
	new_ts = curbuf->b_p_ts;
#endif
    សម្រាប់ (lnum = eap->line1; !got_int && lnum <= eap->line2; ++lnum)
    {
	ptr = ml_get(lnum);
	old_len = ml_get_len(lnum);
	col = 0;
	vcol = 0;
	did_undo = FALSE;
	សម្រាប់ (;;)
	{
	    ប្រសិនបើ (VIM_ISWHITE(ptr[col]))
	    {
		ប្រសិនបើ (!got_tab && num_spaces == 0)
		{
		    // First consecutive white-space
		    start_vcol = vcol;
		    start_col = col;
		}
		ប្រសិនបើ (ptr[col] == ' ')
		    num_spaces++;
		បើមិនដូច្នេះ
		    got_tab = TRUE;
	    }
	    បើមិនដូច្នេះ
	    {
		ប្រសិនបើ (got_tab || (eap->forceit && num_spaces > 1))
		{
		    // Retabulate this string of white-space

		    // len is virtual length of white string
		    len = num_spaces = vcol - start_vcol;
		    num_tabs = 0;
		    ប្រសិនបើ (!curbuf->b_p_et)
		    {
#ifdef FEAT_VARTABS
			ចំនួនគត់ t, s;

			tabstop_fromto(start_vcol, vcol,
					curbuf->b_p_ts, new_vts_array, &t, &s);
			num_tabs = t;
			num_spaces = s;
#បើមិនដូច្នេះ
			temp = new_ts - (start_vcol % new_ts);
			ប្រសិនបើ (num_spaces >= temp)
			{
			    num_spaces -= temp;
			    num_tabs++;
			}
			num_tabs += num_spaces / new_ts;
			num_spaces -= (num_spaces / new_ts) * new_ts;
#endif
		    }
		    ប្រសិនបើ (curbuf->b_p_et || got_tab ||
					(num_spaces + num_tabs < len))
		    {
			ប្រសិនបើ (did_undo == FALSE)
			{
			    did_undo = TRUE;
			    ប្រសិនបើ (u_save((linenr_T)(lnum - 1),
						(linenr_T)(lnum + 1)) == FAIL)
			    {
				new_line = NULL;	// flag out-of-memory
				bha_nga;
			    }
			}

			// len is actual number of white characters used
			len = num_spaces + num_tabs;
			new_len = old_len - col + start_col + len + 1;
			ប្រសិនបើ (new_len <= 0 || new_len >= MAXCOL)
			{
			    emsg_text_too_long();
			    bha_nga;
			}
			new_line = alloc(new_len);
			ប្រសិនបើ (new_line == NULL)
			    bha_nga;
			ប្រសិនបើ (start_col > 0)
			    mch_memmove(new_line, ptr, (size_t)start_col);
			mch_memmove(new_line + start_col + len,
				      ptr + col, (size_t)(old_len - col + 1));
			ptr = new_line + start_col;
			សម្រាប់ (col = 0; col < len; col++)
			    ptr[col] = (col < num_tabs) ? '\t' : ' ';
			ប្រសិនបើ (ml_replace(lnum, new_line, FALSE) == OK)
			    // "new_line" may have been copied
			    new_line = curbuf->b_ml.ml_line_ptr;
			ប្រសិនបើ (first_line == 0)
			    first_line = lnum;
			last_line = lnum;
			ptr = new_line;
			old_len = new_len - 1;
			col = start_col + len;
		    }
		}
		got_tab = FALSE;
		num_spaces = 0;

		ប្រសិនបើ (is_indent_only)
		    bha_nga;
	    }
	    ប្រសិនបើ (ptr[col] == NUL)
		bha_nga;
	    vcol += chartabsize(ptr + col, (colnr_T)vcol);
	    ប្រសិនបើ (vcol >= MAXCOL)
	    {
		emsg_text_too_long();
		bha_nga;
	    }
	    ប្រសិនបើ (has_mbyte)
		col += (*mb_ptr2len)(ptr + col);
	    បើមិនដូច្នេះ
		++col;
	}
	ប្រសិនបើ (new_line == NULL)		    // out of memory
	    bha_nga;
	line_breakcheck();
    }
    ប្រសិនបើ (got_int)
	emsg(_(e_interrupted));

#ifdef FEAT_VARTABS
    // If a single value was given then it can be considered equal to
    // either the value of 'tabstop' or the value of 'vartabstop'.
    ប្រសិនបើ (tabstop_count(curbuf->b_p_vts_array) == 0
	&& tabstop_count(new_vts_array) == 1
	&& curbuf->b_p_ts == tabstop_first(new_vts_array))
	; // not changed
    បើមិនដូច្នេះ ប្រសិនបើ (tabstop_count(curbuf->b_p_vts_array) > 0
	&& tabstop_eq(curbuf->b_p_vts_array, new_vts_array))
	; // not changed
    បើមិនដូច្នេះ
	redraw_curbuf_later(UPD_NOT_VALID);
#បើមិនដូច្នេះ
    ប្រសិនបើ (curbuf->b_p_ts != new_ts)
	redraw_curbuf_later(UPD_NOT_VALID);
#endif
    ប្រសិនបើ (first_line != 0)
	changed_lines(first_line, 0, last_line + 1, 0L);

    curwin->w_p_list = save_list;	// restore 'list'

#ifdef FEAT_VARTABS
    ប្រសិនបើ (new_ts_str != NULL)		// set the new tabstop
    {
	// If 'vartabstop' is in use or if the value given to retab has more
	// than one tabstop then update 'vartabstop'.
	ចំនួនគត់ *old_vts_ary = curbuf->b_p_vts_array;

	ប្រសិនបើ (tabstop_count(old_vts_ary) > 0 || tabstop_count(new_vts_array) > 1)
	{
	    set_string_option_direct((char_u *)"vts", -1, new_ts_str,
							OPT_FREE|OPT_LOCAL, 0);
	    curbuf->b_p_vts_array = new_vts_array;
	    vim_free(old_vts_ary);
	}
	បើមិនដូច្នេះ
	{
	    // 'vartabstop' wasn't in use and a single value was given to
	    // retab then update 'tabstop'.
	    curbuf->b_p_ts = tabstop_first(new_vts_array);
	    vim_free(new_vts_array);
	}
	vim_free(new_ts_str);
    }
#បើមិនដូច្នេះ
    curbuf->b_p_ts = new_ts;
#endif
    coladvance(curwin->w_curswant);

    u_clearline();
}

#ប្រសិនបើ defined(FEAT_EVAL)
/*
 * Get indent level from 'indentexpr'.
 */
    ចំនួនគត់
get_expr_indent(void)
{
    ចំនួនគត់		indent = -1;
    char_u	*inde_copy;
    pos_T	save_pos;
    colnr_T	save_curswant;
    ចំនួនគត់		save_set_curswant;
    ចំនួនគត់		save_State;
    ចំនួនគត់		use_sandbox = was_set_insecurely(curwin,
					    (char_u *)"indentexpr", OPT_LOCAL);
    sctx_T	save_sctx = current_sctx;

    // Save and restore cursor position and curswant, in case it was changed
    // via :normal commands
    save_pos = curwin->w_cursor;
    save_curswant = curwin->w_curswant;
    save_set_curswant = curwin->w_set_curswant;
    set_vim_var_nr(VV_LNUM, curwin->w_cursor.lnum);
    ប្រសិនបើ (use_sandbox)
	++sandbox;
    ++textlock;
    current_sctx = curbuf->b_p_script_ctx[BV_INDE];

    // Need to make a copy, the 'indentexpr' option could be changed while
    // evaluating it.
    inde_copy = vim_strsave(curbuf->b_p_inde);
    ប្រសិនបើ (inde_copy != NULL)
    {
	indent = (ចំនួនគត់)eval_to_number(inde_copy, TRUE);
	vim_free(inde_copy);
    }

    ប្រសិនបើ (use_sandbox)
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
    ប្រសិនបើ (did_throw && (vim_strchr(p_debug, 't') == NULL || trylevel == 0))
    {
	handle_did_throw();
	did_throw = FALSE;
    }

    // If there is an error, just keep the current indent.
    ប្រសិនបើ (indent < 0)
	indent = get_indent();

    ត្រឡប់ indent;
}
#endif

    static ចំនួនគត់
lisp_match(char_u *p)
{
    char_u	buf[LSIZE];
    ចំនួនគត់		len;
    char_u	*word = *curbuf->b_p_lw != NUL ? curbuf->b_p_lw : p_lispwords;

    ខណៈពេល (*word != NUL)
    {
	len = copy_option_part(&word, buf, LSIZE, ",");
	ប្រសិនបើ (STRNCMP(buf, p, len) == 0 && IS_WHITE_OR_NUL(p[len]))
	    ត្រឡប់ TRUE;
    }
    ត្រឡប់ FALSE;
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
    ចំនួនគត់
get_lisp_indent(void)
{
    pos_T	*pos, realpos, paren;
    ចំនួនគត់		amount;
    char_u	*that;
    colnr_T	col;
    colnr_T	firsttry;
    ចំនួនគត់		parencount, quotecount;
    ចំនួនគត់		vi_lisp;

    // Set vi_lisp to use the vi-compatible method
    vi_lisp = (vim_strchr(p_cpo, CPO_LISP) != NULL);

    realpos = curwin->w_cursor;
    curwin->w_cursor.col = 0;

    ប្រសិនបើ ((pos = findmatch(NULL, '(')) == NULL)
	pos = findmatch(NULL, '[');
    បើមិនដូច្នេះ
    {
	paren = *pos;
	pos = findmatch(NULL, '[');
	ប្រសិនបើ (pos == NULL || LT_POSP(pos, &paren))
	    pos = &paren;
    }
    ប្រសិនបើ (pos != NULL)
    {
	// Extra trick: Take the indent of the first previous non-white
	// line that is at the same () level.
	amount = -1;
	parencount = 0;

	ខណៈពេល (--curwin->w_cursor.lnum >= pos->lnum)
	{
	    ប្រសិនបើ (linewhite(curwin->w_cursor.lnum))
		continue;
	    សម្រាប់ (that = ml_get_curline(); *that != NUL; ++that)
	    {
		ប្រសិនបើ (*that == ';')
		{
		    ខណៈពេល (*(that + 1) != NUL)
			++that;
		    continue;
		}
		ប្រសិនបើ (*that == '\\')
		{
		    ប្រសិនបើ (*(that + 1) != NUL)
			++that;
		    continue;
		}
		ប្រសិនបើ (*that == '"' && *(that + 1) != NUL)
		{
		    ខណៈពេល (*++that && *that != '"')
		    {
			// skipping escaped characters in the string
			ប្រសិនបើ (*that == '\\')
			{
			    ប្រសិនបើ (*++that == NUL)
				bha_nga;
			    ប្រសិនបើ (that[1] == NUL)
			    {
				++that;
				bha_nga;
			    }
			}
		    }
		    ប្រសិនបើ (*that == NUL)
			bha_nga;
		}
		ប្រសិនបើ (*that == '(' || *that == '[')
		    ++parencount;
		បើមិនដូច្នេះ ប្រសិនបើ (*that == ')' || *that == ']')
		    --parencount;
	    }
	    ប្រសិនបើ (parencount == 0)
	    {
		amount = get_indent();
		bha_nga;
	    }
	}

	ប្រសិនបើ (amount == -1)
	{
	    curwin->w_cursor.lnum = pos->lnum;
	    curwin->w_cursor.col = pos->col;
	    col = pos->col;

	    that = ml_get_curline();

	    ប្រសិនបើ (vi_lisp && get_indent() == 0)
		amount = 2;
	    បើមិនដូច្នេះ
	    {
		char_u		*line = that;
		chartabsize_T	cts;

		init_chartabsize_arg(&cts, curwin, pos->lnum, 0, line, line);
		ខណៈពេល (*cts.cts_ptr != NUL && col > 0)
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

		ប្រសិនបើ (!vi_lisp && (*that == '(' || *that == '[')
						      && lisp_match(that + 1))
		    amount += 2;
		បើមិនដូច្នេះ
		{
		    ប្រសិនបើ (*that != NUL)
		    {
			that++;
			amount++;
		    }
		    firsttry = amount;

		    init_chartabsize_arg(&cts, curwin, (colnr_T)(that - line),
							   amount, line, that);
		    ខណៈពេល (VIM_ISWHITE(*cts.cts_ptr))
		    {
			cts.cts_vcol += lbr_chartabsize(&cts);
			++cts.cts_ptr;
		    }
		    that = cts.cts_ptr;
		    amount = cts.cts_vcol;
		    clear_chartabsize_arg(&cts);

		    ប្រសិនបើ (*that && *that != ';') // not a comment line
		    {
			// test *that != '(' to accommodate first let/do
			// argument if it is more than one line
			ប្រសិនបើ (!vi_lisp && *that != '(' && *that != '[')
			    firsttry++;

			parencount = 0;
			quotecount = 0;

			init_chartabsize_arg(&cts, curwin,
				   (colnr_T)(that - line), amount, line, that);
			ប្រសិនបើ (vi_lisp
				|| (*that != '"'
				    && *that != '\''
				    && *that != '#'
				    && (*that < '0' || *that > '9')))
			{
			    ខណៈពេល (*cts.cts_ptr
				    && (!VIM_ISWHITE(*cts.cts_ptr)
					|| quotecount
					|| parencount)
				    && (!((*cts.cts_ptr == '('
							|| *cts.cts_ptr == '[')
					    && !quotecount
					    && !parencount
					    && vi_lisp)))
			    {
				ប្រសិនបើ (*cts.cts_ptr == '"')
				    quotecount = !quotecount;
				ប្រសិនបើ ((*cts.cts_ptr == '(' || *cts.cts_ptr == '[')
							       && !quotecount)
				    ++parencount;
				ប្រសិនបើ ((*cts.cts_ptr == ')' || *cts.cts_ptr == ']')
							       && !quotecount)
				    --parencount;
				ប្រសិនបើ (*cts.cts_ptr == '\\'
						    && *(cts.cts_ptr+1) != NUL)
				    cts.cts_vcol += lbr_chartabsize_adv(&cts);
				cts.cts_vcol += lbr_chartabsize_adv(&cts);
			    }
			}
			ខណៈពេល (VIM_ISWHITE(*cts.cts_ptr))
			{
			    cts.cts_vcol += lbr_chartabsize(&cts);
			    ++cts.cts_ptr;
			}
			that = cts.cts_ptr;
			amount = cts.cts_vcol;
			clear_chartabsize_arg(&cts);

			ប្រសិនបើ (!*that || *that == ';')
			    amount = firsttry;
		    }
		}
	    }
	}
    }
    បើមិនដូច្នេះ
	amount = 0;	// no matching '(' or '[' found, use zero indent

    curwin->w_cursor = realpos;

    ត្រឡប់ amount;
}

/*
 * Re-indent the current line, based on the current contents of it and the
 * surrounding lines. Fixing the cursor position seems really easy -- I'm very
 * confused what all the part that handles Control-T is doing that I'm not.
 * "get_the_indent" should be get_c_indent, get_expr_indent or get_lisp_indent.
 */

    void
fixthisline(ចំនួនគត់ (*get_the_indent)(void))
{
    ចំនួនគត់ amount = get_the_indent();

    ប្រសិនបើ (amount < 0)
	ត្រឡប់;

    change_indent(INDENT_SET, amount, FALSE, 0, TRUE);
    ប្រសិនបើ (linewhite(curwin->w_cursor.lnum))
	did_ai = TRUE;	// delete the indent if the line stays empty
}

/*
 * Return TRUE if 'indentexpr' should be used for Lisp indenting.
 * Caller may want to check 'autoindent'.
 */
    ចំនួនគត់
use_indentexpr_for_lisp(void)
{
#ifdef FEAT_EVAL
    ត្រឡប់ curbuf->b_p_lisp
		&& *curbuf->b_p_inde != NUL
		&& STRCMP(curbuf->b_p_lop, "expr:1") == 0;
#បើមិនដូច្នេះ
    ត្រឡប់ FALSE;
#endif
}

/*
 * Fix indent for 'lisp' and 'cindent'.
 */
    void
fix_indent(void)
{
    ប្រសិនបើ (p_paste)
	ត្រឡប់;  // no auto-indenting when 'paste' is set
    ប្រសិនបើ (curbuf->b_p_lisp && curbuf->b_p_ai)
    {
	ប្រសិនបើ (use_indentexpr_for_lisp())
	    do_c_expr_indent();
	បើមិនដូច្នេះ
	    fixthisline(get_lisp_indent);
    }
    បើមិនដូច្នេះ ប្រសិនបើ (cindent_on())
	do_c_expr_indent();
}

#ប្រសិនបើ defined(FEAT_EVAL)
/*
 * "indent()" function
 */
    void
f_indent(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    ប្រសិនបើ (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ត្រឡប់;

    lnum = tv_get_lnum(argvars);
    ប្រសិនបើ (lnum >= 1 && lnum <= curbuf->b_ml.ml_line_count)
	rettv->vval.v_number = get_indent_lnum(lnum);
    បើមិនដូច្នេះ
    {
	ប្រសិនបើ (in_vim9script())
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

    ប្រសិនបើ (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ត្រឡប់;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    ប្រសិនបើ (lnum >= 1 && lnum <= curbuf->b_ml.ml_line_count)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_number = get_lisp_indent();
	curwin->w_cursor = pos;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (in_vim9script())
	semsg(_(e_invalid_line_number_nr), lnum);
    បើមិនដូច្នេះ
	rettv->vval.v_number = -1;
}
#endif
