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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_VARTABS)

/*
 * Set the integer values corresponding to the string setting of 'vartabstop'.
 * "array" will be set, caller must free it if needed.
 * Return FAIL for an error.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tabstop_set(char_u *var, ପୂର୍ଣ୍ଣସଂଖ୍ୟା **array)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    valcount = 1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    t;
    char_u  *cp;

    ଯଦି (var[0] == NUL || (var[0] == '0' && var[1] == NUL))
    {
	*array = ଖାଲି;
	ଫେରନ୍ତୁ OK;
    }

    ପାଇଁ (cp = var; *cp != NUL; ++cp)
    {
	ଯଦି (cp == var || cp[-1] == ',')
	{
	    char_u *end;

	    ଯଦି (strtol((ଅକ୍ଷର *)cp, (ଅକ୍ଷର **)&end, 10) <= 0)
	    {
		ଯଦି (cp != end)
		    emsg(_(e_argument_must_be_positive));
		ଅନ୍ୟଥା
		    semsg(_(e_invalid_argument_str), cp);
		ଫେରନ୍ତୁ FAIL;
	    }
	}

	ଯଦି (VIM_ISDIGIT(*cp))
	    ଜାରି;
	ଯଦି (cp[0] == ',' && cp > var && cp[-1] != ',' && cp[1] != NUL)
	{
	    ++valcount;
	    ଜାରି;
	}
	semsg(_(e_invalid_argument_str), var);
	ଫେରନ୍ତୁ FAIL;
    }

    *array = ALLOC_MULT(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, valcount + 1);
    ଯଦି (*array == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    (*array)[0] = valcount;

    t = 1;
    ପାଇଁ (cp = var; *cp != NUL;)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = atoi((ଅକ୍ଷର *)cp);

	// Catch negative values, overflow and ridiculous big values.
	ଯଦି (n <= 0 || n > TABSTOP_MAX)
	{
	    semsg(_(e_invalid_argument_str), cp);
	    VIM_CLEAR(*array);
	    ଫେରନ୍ତୁ FAIL;
	}
	(*array)[t++] = n;
	ଯେତେବେଳେ (*cp != NUL && *cp != ',')
	    ++cp;
	ଯଦି (*cp != NUL)
	    ++cp;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Calculate the number of screen spaces a tab will occupy.
 * If "vts" is set then the tab widths are taken from that array,
 * otherwise the value of ts is used.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tabstop_padding(colnr_T col, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ts_arg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *vts)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ts = ts_arg == 0 ? 8 : ts_arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tabcount;
    colnr_T	tabcol = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		t;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		padding = 0;

    ଯଦି (vts == ଖାଲି || vts[0] == 0)
	ଫେରନ୍ତୁ ts - (col % ts);

    tabcount = vts[0];

    ପାଇଁ (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	ଯଦି (tabcol > col)
	{
	    padding = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(tabcol - col);
	    ଭାଙ୍ଗ;
	}
    }
    ଯଦି (t > tabcount)
	padding = vts[tabcount] - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((col - tabcol) % vts[tabcount]);

    ଫେରନ୍ତୁ padding;
}

/*
 * Find the size of the tab that covers a particular column.
 *
 * If this is being called as part of a shift operation, col is not the cursor
 * column but is the column number to the left of the first non-whitespace
 * character in the line.  If the shift is to the left (left = TRUE), then
 * return the size of the tab interval to the left of the column.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tabstop_at(colnr_T col, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ts, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *vts, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ବାମ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tabcount;	// Number of tab stops in the list of variable
				// tab stops.
    colnr_T	tabcol = 0;	// Column of the tab stop under consideration.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		t;		// Tabstop index in the list of variable tab
				// stops.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tab_size = 0;	// Size of the tab stop interval to the right
				// or left of the col.

    ଯଦି (vts == 0 || vts[0] == 0)
	ଫେରନ୍ତୁ ts;

    tabcount = vts[0];
    ପାଇଁ (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	ଯଦି (tabcol > col)
	{
	    // If shifting left (left != 0), and if the column to the left of
	    // the first first non-blank character (col) in the line is
	    // already to the left of the first tabstop, set the shift amount
	    // (tab_size) to just enough to shift the line to the left margin.
	    // The value doesn't seem to matter as long as it is at least that
	    // distance.
	    ଯଦି (ବାମ && (t == 1))
		tab_size = col;
	    ଅନ୍ୟଥା
		tab_size = vts[t - (ବାମ ? 1 : 0)];
	    ଭାଙ୍ଗ;
	}
    }
    ଯଦି (t > tabcount)		// If the value of the index t is beyond the
				// end of the list, use the tab stop value at
				// the end of the list.
	tab_size = vts[tabcount];

    ଫେରନ୍ତୁ tab_size;
}

/*
 * Find the column on which a tab starts.
 */
    colnr_T
tabstop_start(colnr_T col, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ts, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *vts)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tabcount;
    colnr_T	tabcol = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		t;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		excess;

    ଯଦି (vts == ଖାଲି || vts[0] == 0)
	ଫେରନ୍ତୁ col - col % ts;

    tabcount = vts[0];
    ପାଇଁ (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	ଯଦି (tabcol > col)
	    ଫେରନ୍ତୁ tabcol - vts[t];
    }

    excess = tabcol % vts[tabcount];
    ଫେରନ୍ତୁ col - (col - excess) % vts[tabcount];
}

/*
 * Find the number of tabs and spaces necessary to get from one column
 * to another.
 */
    ଶୂନ୍ୟ
tabstop_fromto(
	colnr_T start_col,
	colnr_T end_col,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ts_arg,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*vts,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*ntabs,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*nspcs)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		spaces = end_col - start_col;
    colnr_T	tabcol = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		padding = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tabcount;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		t;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ts = ts_arg == 0 ? curbuf->b_p_ts : ts_arg;

    ଯଦି (vts == ଖାଲି || vts[0] == 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା tabs = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା initspc = 0;

	initspc = ts - (start_col % ts);
	ଯଦି (spaces >= initspc)
	{
	    spaces -= initspc;
	    tabs++;
	}
	tabs += spaces / ts;
	spaces -= (spaces / ts) * ts;

	*ntabs = tabs;
	*nspcs = spaces;
	ଫେରନ୍ତୁ;
    }

    // Find the padding needed to reach the next tabstop.
    tabcount = vts[0];
    ପାଇଁ (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	ଯଦି (tabcol > start_col)
	{
	    padding = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(tabcol - start_col);
	    ଭାଙ୍ଗ;
	}
    }
    ଯଦି (t > tabcount)
	padding = vts[tabcount] - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((start_col - tabcol) % vts[tabcount]);

    // If the space needed is less than the padding no tabs can be used.
    ଯଦି (spaces < padding)
    {
	*ntabs = 0;
	*nspcs = spaces;
	ଫେରନ୍ତୁ;
    }

    *ntabs = 1;
    spaces -= padding;

    // At least one tab has been used. See if any more will fit.
    ଯେତେବେଳେ (spaces != 0 && ++t <= tabcount)
    {
	padding = vts[t];
	ଯଦି (spaces < padding)
	{
	    *nspcs = spaces;
	    ଫେରନ୍ତୁ;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tabstop_eq(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ts1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ts2)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		t;

    ଯଦି ((ts1 == 0 && ts2) || (ts1 && ts2 == 0))
	ଫେରନ୍ତୁ FALSE;
    ଯଦି (ts1 == ts2)
	ଫେରନ୍ତୁ TRUE;
    ଯଦି (ts1[0] != ts2[0])
	ଫେରନ୍ତୁ FALSE;

    ପାଇଁ (t = 1; t <= ts1[0]; ++t)
	ଯଦି (ts1[t] != ts2[t])
	    ଫେରନ୍ତୁ FALSE;

    ଫେରନ୍ତୁ TRUE;
}

# ଯଦି defined(FEAT_BEVAL)
/*
 * Copy a tabstop array, allocating space for the new array.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *
tabstop_copy(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *oldts)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*newts;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		t;

    ଯଦି (oldts == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    newts = ALLOC_MULT(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, oldts[0] + 1);
    ଯଦି (newts != ଖାଲି)
	ପାଇଁ (t = 0; t <= oldts[0]; ++t)
	    newts[t] = oldts[t];
    ଫେରନ୍ତୁ newts;
}
# endif

/*
 * Return a count of the number of tabstops.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tabstop_ଗଣନା(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ts)
{
    ଫେରନ୍ତୁ ts != ଖାଲି ? ts[0] : 0;
}

/*
 * Return the first tabstop, or 8 if there are no tabstops defined.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tabstop_first(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ts)
{
    ଫେରନ୍ତୁ ts != ଖାଲି ? ts[1] : 8;
}

#endif

/*
 * Return the effective shiftwidth value for current buffer, using the
 * 'tabstop' value when 'shiftwidth' is zero.
 */
    long
get_sw_ମୂଲ୍ୟ(buf_T *buf)
{
    ଫେରନ୍ତୁ get_sw_ମୂଲ୍ୟ_col(buf, 0, FALSE);
}

/*
 * Idem, using "pos".
 */
    ସ୍ଥିର long
get_sw_ମୂଲ୍ୟ_pos(buf_T *buf, pos_T *pos, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ବାମ)
{
    pos_T save_cursor = curwin->w_cursor;
    long sw_ମୂଲ୍ୟ;

    curwin->w_cursor = *pos;
    sw_ମୂଲ୍ୟ = get_sw_ମୂଲ୍ୟ_col(buf, get_nolist_virtcol(), ବାମ);
    curwin->w_cursor = save_cursor;
    ଫେରନ୍ତୁ sw_ମୂଲ୍ୟ;
}

/*
 * Idem, using the first non-black in the current line.
 */
    long
get_sw_ମୂଲ୍ୟ_indent(buf_T *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ବାମ)
{
    pos_T pos = curwin->w_cursor;

    pos.col = getwhitecols_curline();
    ଫେରନ୍ତୁ get_sw_ମୂଲ୍ୟ_pos(buf, &pos, ବାମ);
}

/*
 * Idem, using virtual column "col".
 */
    long
get_sw_ମୂଲ୍ୟ_col(buf_T *buf, colnr_T col UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ବାମ UNUSED)
{
    ଫେରନ୍ତୁ buf->b_p_sw ? buf->b_p_sw :
#ifdef FEAT_VARTABS
	tabstop_at(col, buf->b_p_ts, buf->b_p_vts_array, ବାମ);
#ଅନ୍ୟଥା
	buf->b_p_ts;
#endif
}

/*
 * Return the effective softtabstop value for the current buffer, using the
 * 'shiftwidth' value when 'softtabstop' is negative.
 */
    long
get_sts_ମୂଲ୍ୟ(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ curbuf->b_p_sts < 0 ? get_sw_ମୂଲ୍ୟ(curbuf) : curbuf->b_p_sts;
}

/*
 * Count the size (in window cells) of the indent in the current line.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_indent(ଶୂନ୍ୟ)
{
#ifdef FEAT_VARTABS
    ଫେରନ୍ତୁ get_indent_str_vtab(ml_get_curline(), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ get_indent_str(ml_get_curline(), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curbuf->b_p_ts, FALSE);
#endif
}

/*
 * Count the size (in window cells) of the indent in line "lnum".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_indent_lnum(linenr_T lnum)
{
#ifdef FEAT_VARTABS
    ଫେରନ୍ତୁ get_indent_str_vtab(ml_get(lnum), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ get_indent_str(ml_get(lnum), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curbuf->b_p_ts, FALSE);
#endif
}

#ଯଦି defined(FEAT_FOLDING)
/*
 * Count the size (in window cells) of the indent in line "lnum" of buffer
 * "buf".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_indent_buf(buf_T *buf, linenr_T lnum)
{
# ifdef FEAT_VARTABS
    ଫେରନ୍ତୁ get_indent_str_vtab(ml_get_buf(buf, lnum, FALSE),
			       (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)buf->b_p_ts, buf->b_p_vts_array, FALSE);
# ଅନ୍ୟଥା
    ଫେରନ୍ତୁ get_indent_str(ml_get_buf(buf, lnum, FALSE), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)buf->b_p_ts, FALSE);
# endif
}
#endif

/*
 * count the size (in window cells) of the indent in line "ptr", with
 * 'tabstop' at "ts"
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_indent_str(
    char_u	*ptr,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ts,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		no_ts) // if TRUE, count a tab as ^I
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2 = 0;

    ପାଇଁ ( ; *ptr; ++ptr)
    {
	ଯଦି (*ptr == TAB)    // count a tab for what it is worth
	{
	    ଯଦି (!no_ts)
		ଗଣନା_2 += ts - (ଗଣନା_2 % ts);
	    ଅନ୍ୟଥା
		ଗଣନା_2 += ptr2cells(ptr);
	}
	ଅନ୍ୟଥା ଯଦି (*ptr == ' ')
	    ++ଗଣନା_2;		// count a space for one
	ଅନ୍ୟଥା
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ ଗଣନା_2;
}

#ଯଦି defined(FEAT_VARTABS)
/*
 * Count the size (in window cells) of the indent in line "ptr", using
 * variable tabstops.
 * If "no_ts" is TRUE, count a tab as ^I.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_indent_str_vtab(char_u *ptr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ts, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *vts, ପୂର୍ଣ୍ଣସଂଖ୍ୟା no_ts)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2 = 0;

    ପାଇଁ ( ; *ptr; ++ptr)
    {
	ଯଦି (*ptr == TAB)    // count a tab for what it is worth
	{
	    ଯଦି (!no_ts)
		ଗଣନା_2 += tabstop_padding(ଗଣନା_2, ts, vts);
	    ଅନ୍ୟଥା
		ଗଣନା_2 += ptr2cells(ptr);
	}
	ଅନ୍ୟଥା ଯଦି (*ptr == ' ')
	    ++ଗଣନା_2;		// count a space for one
	ଅନ୍ୟଥା
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ ଗଣନା_2;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_indent(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		size,		    // measured in spaces
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags)
{
    char_u	*p;
    char_u	*newline;
    char_u	*oldline;
    char_u	*s;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ind_len;	    // measured in characters
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଧାଡ଼ି_len;	    // size of the line (including the NUL)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		doit = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ind_done = 0;	    // measured in spaces
#ifdef FEAT_VARTABS
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ind_col = 0;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tab_pad;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		orig_char_len = -1; // number of initial whitespace chars when
				    // 'et' and 'pi' are both set

    // First check if there is anything to do and compute the number of
    // characters needed for the indent.
    todo = size;
    ind_len = 0;
    p = oldline = ml_get_curline();
    ଧାଡ଼ି_len = ml_get_curline_len() + 1;

    // Calculate the buffer size for the new indent, and check to see if it
    // isn't already set

    // if 'expandtab' isn't set: use TABs; if both 'expandtab' and
    // 'preserveindent' are set count the number of characters at the
    // beginning of the line to be copied
    ଯଦି (!curbuf->b_p_et || (!(flags & SIN_ଭର୍ତ୍ତି) && curbuf->b_p_pi))
    {
	// If 'preserveindent' is set then reuse as much as possible of
	// the existing indent structure for the new indent
	ଯଦି (!(flags & SIN_ଭର୍ତ୍ତି) && curbuf->b_p_pi)
	{
	    ind_done = 0;

	    // count as many characters as we can use
	    ଯେତେବେଳେ (todo > 0 && VIM_ISWHITE(*p))
	    {
		ଯଦି (*p == TAB)
		{
#ifdef FEAT_VARTABS
		    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#ଅନ୍ୟଥା
		    tab_pad = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curbuf->b_p_ts
					   - (ind_done % (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curbuf->b_p_ts);
#endif
		    // stop if this tab will overshoot the target
		    ଯଦି (todo < tab_pad)
			ଭାଙ୍ଗ;
		    todo -= tab_pad;
		    ++ind_len;
		    ind_done += tab_pad;
		}
		ଅନ୍ୟଥା
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
	    ଯଦି (curbuf->b_p_et)
		orig_char_len = ind_len;

	    // Fill to next tabstop with a tab, if possible
#ifdef FEAT_VARTABS
	    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
						curbuf->b_p_vts_array);
#ଅନ୍ୟଥା
	    tab_pad = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curbuf->b_p_ts - (ind_done % (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curbuf->b_p_ts);
#endif
	    ଯଦି (todo >= tab_pad && orig_char_len == -1)
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
	ପାଇଁ (;;)
	{
	    tab_pad = tabstop_padding(ind_col, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
	    ଯଦି (todo < tab_pad)
		ଭାଙ୍ଗ;
	    ଯଦି (*p != TAB)
		doit = TRUE;
	    ଅନ୍ୟଥା
		++p;
	    todo -= tab_pad;
	    ++ind_len;
	    ind_col += tab_pad;
	}
#ଅନ୍ୟଥା
	ଯେତେବେଳେ (todo >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curbuf->b_p_ts)
	{
	    ଯଦି (*p != TAB)
		doit = TRUE;
	    ଅନ୍ୟଥା
		++p;
	    todo -= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curbuf->b_p_ts;
	    ++ind_len;
	    // ind_done += (int)curbuf->b_p_ts;
	}
#endif
    }
    // count spaces required for indent
    ଯେତେବେଳେ (todo > 0)
    {
	ଯଦି (*p != ' ')
	    doit = TRUE;
	ଅନ୍ୟଥା
	    ++p;
	--todo;
	++ind_len;
	// ++ind_done;
    }

    // Return if the indent is OK already.
    ଯଦି (!doit && !VIM_ISWHITE(*p) && !(flags & SIN_ଭର୍ତ୍ତି))
	ଫେରନ୍ତୁ FALSE;

    // Allocate memory for the new line.
    ଯଦି (flags & SIN_ଭର୍ତ୍ତି)
	p = oldline;
    ଅନ୍ୟଥା
    {
	p = skipwhite(p);
	ଧାଡ଼ି_len -= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - oldline);
    }

    // If 'preserveindent' and 'expandtab' are both set keep the original
    // characters and allocate accordingly.  We will fill the rest with spaces
    // after the if (!curbuf->b_p_et) below.
    ଯଦି (orig_char_len != -1)
    {
	newline = alloc(orig_char_len + size - ind_done + ଧାଡ଼ି_len);
	ଯଦି (newline == ଖାଲି)
	    ଫେରନ୍ତୁ FALSE;
	todo = size - ind_done;
	ind_len = orig_char_len + todo;    // Set total length of indent in
					   // characters, which may have been
					   // undercounted until now
	p = oldline;
	s = newline;
	ଯେତେବେଳେ (orig_char_len > 0)
	{
	    *s++ = *p++;
	    orig_char_len--;
	}

	// Skip over any additional white space (useful when newindent is less
	// than old)
	ଯେତେବେଳେ (VIM_ISWHITE(*p))
	    ++p;

    }
    ଅନ୍ୟଥା
    {
	todo = size;
	newline = alloc(ind_len + ଧାଡ଼ି_len);
	ଯଦି (newline == ଖାଲି)
	    ଫେରନ୍ତୁ FALSE;
	s = newline;
    }

    // Put the characters in the new line.
    // if 'expandtab' isn't set: use TABs
    ଯଦି (!curbuf->b_p_et)
    {
	// If 'preserveindent' is set then reuse as much as possible of
	// the existing indent structure for the new indent
	ଯଦି (!(flags & SIN_ଭର୍ତ୍ତି) && curbuf->b_p_pi)
	{
	    p = oldline;
	    ind_done = 0;

	    ଯେତେବେଳେ (todo > 0 && VIM_ISWHITE(*p))
	    {
		ଯଦି (*p == TAB)
		{
#ifdef FEAT_VARTABS
		    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#ଅନ୍ୟଥା
		    tab_pad = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curbuf->b_p_ts
					   - (ind_done % (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curbuf->b_p_ts);
#endif
		    // stop if this tab will overshoot the target
		    ଯଦି (todo < tab_pad)
			ଭାଙ୍ଗ;
		    todo -= tab_pad;
		    ind_done += tab_pad;
		}
		ଅନ୍ୟଥା
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
#ଅନ୍ୟଥା
	    tab_pad = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curbuf->b_p_ts - (ind_done % (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curbuf->b_p_ts);
#endif
	    ଯଦି (todo >= tab_pad)
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
	ପାଇଁ (;;)
	{
	    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
	    ଯଦି (todo < tab_pad)
		ଭାଙ୍ଗ;
	    *s++ = TAB;
	    todo -= tab_pad;
	    ind_done += tab_pad;
	}
#ଅନ୍ୟଥା
	ଯେତେବେଳେ (todo >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curbuf->b_p_ts)
	{
	    *s++ = TAB;
	    todo -= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curbuf->b_p_ts;
	}
#endif
    }
    ଯେତେବେଳେ (todo > 0)
    {
	*s++ = ' ';
	--todo;
    }
    mch_memmove(s, p, (size_t)ଧାଡ଼ି_len);

    // Replace the line (unless undo fails).
    ଯଦି (!(flags & SIN_UNDO) || u_savesub(curwin->w_cursor.lnum) == OK)
    {
	colnr_T old_offset = (colnr_T)(p - oldline);
	colnr_T new_offset = (colnr_T)(s - newline);

	// this may free "newline"
	ml_replace(curwin->w_cursor.lnum, newline, FALSE);
	ଯଦି (flags & SIN_CHANGED)
	    changed_bytes(curwin->w_cursor.lnum, 0);

	// Correct saved cursor position if it is in this line.
	ଯଦି (saved_cursor.lnum == curwin->w_cursor.lnum)
	{
	    ଯଦି (saved_cursor.col >= old_offset)
		// cursor was after the indent, adjust for the number of
		// bytes added/removed
		saved_cursor.col += ind_len - old_offset;
	    ଅନ୍ୟଥା ଯଦି (saved_cursor.col >= new_offset)
		// cursor was in the indent, and is now after it, put it back
		// at the start of the indent (replacing spaces with TAB)
		saved_cursor.col = new_offset;
	}
#ifdef FEAT_PROP_POPUP
	ଯଦି (curbuf->b_has_textprop)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା added = ind_len - old_offset;

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
    ଅନ୍ୟଥା
	vim_free(newline);

    curwin->w_cursor.col = ind_len;
    ଫେରନ୍ତୁ retval;
}

/*
 * Return the indent of the current line after a number.  Return -1 if no
 * number was found.  Used for 'n' in 'formatoptions': numbered list.
 * Since a pattern is used it can actually handle more than numbers.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_ସଂଖ୍ୟା_indent(linenr_T lnum)
{
    colnr_T	col;
    pos_T	pos;

    regmatch_T	regmatch;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lead_len = 0;	// length of comment leader

    ଯଦି (lnum > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	ଫେରନ୍ତୁ -1;
    pos.lnum = 0;

    // In format_lines() (i.e. not insert mode), fo+=q is needed too...
    ଯଦି ((State & MODE_ଭର୍ତ୍ତି) || has_format_option(FO_Q_COMS))
	lead_len = get_leader_len(ml_get(lnum), ଖାଲି, FALSE, TRUE);

    regmatch.regprog = vim_regcomp(curbuf->b_p_flp, RE_MAGIC);
    ଯଦି (regmatch.regprog != ଖାଲି)
    {
	regmatch.rm_ic = FALSE;

	// vim_regexec() expects a pointer to a line.  This lets us
	// start matching for the flp beyond any comment leader...
	ଯଦି (vim_regexec(&regmatch, ml_get(lnum) + lead_len, (colnr_T)0))
	{
	    pos.lnum = lnum;
	    pos.col = (colnr_T)(*regmatch.endp - ml_get(lnum));
	    pos.coladd = 0;
	}
	vim_regfree(regmatch.regprog);
    }

    ଯଦି (pos.lnum == 0 || *ml_get_pos(&pos) == NUL)
	ଫେରନ୍ତୁ -1;
    getvcol(curwin, &pos, &col, ଖାଲି, ଖାଲି, 0);
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)col;
}

#ଯଦି defined(FEAT_LINEBREAK)
/*
 * Check "briopt" as 'breakindentopt' and update the members of "wp".
 * This is called when 'breakindentopt' is changed and when a window is
 * initialized.
 * Returns FAIL for failure, OK otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
briopt_check(
    char_u	*briopt,	// when NULL: use "wp->w_p_briopt"
    win_T	*wp)		// when NULL: only check "briopt"
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bri_shift = 0;
    long	bri_min = 20;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bri_sbr = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bri_list = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bri_vcol = 0;

    ଯଦି (briopt != ଖାଲି)
	p = briopt;
    ଅନ୍ୟଥା
	p = wp->w_p_briopt;

    ଯେତେବେଳେ (*p != NUL)
    {
	// Note: Keep this in sync with p_briopt_values
	ଯଦି (STRNCMP(p, "shift:", 6) == 0
		 && ((p[6] == '-' && VIM_ISDIGIT(p[7])) || VIM_ISDIGIT(p[6])))
	{
	    p += 6;
	    bri_shift = getdigits(&p);
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "min:", 4) == 0 && VIM_ISDIGIT(p[4]))
	{
	    p += 4;
	    bri_min = getdigits(&p);
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "sbr", 3) == 0)
	{
	    p += 3;
	    bri_sbr = TRUE;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "list:", 5) == 0)
	{
	    p += 5;
	    bri_list = getdigits(&p);
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "column:", 7) == 0)
	{
	    p += 7;
	    bri_vcol = getdigits(&p);
	}
	ଯଦି (*p != ',' && *p != NUL)
	    ଫେରନ୍ତୁ FAIL;
	ଯଦି (*p == ',')
	    ++p;
    }

    ଯଦି (wp == ଖାଲି)
	ଫେରନ୍ତୁ OK;

    wp->w_briopt_shift = bri_shift;
    wp->w_briopt_min   = bri_min;
    wp->w_briopt_sbr   = bri_sbr;
    wp->w_briopt_list  = bri_list;
    wp->w_briopt_vcol  = bri_vcol;

    ଫେରନ୍ତୁ OK;
}

/*
 * Return appropriate space number for breakindent, taking influencing
 * parameters into account. Window must be specified, since it is not
 * necessarily always the current one.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_breakindent_win(
    win_T	*wp,
    char_u	*ଧାଡ଼ି) // start of the line
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    prev_indent = 0;	// cached indent value
    ସ୍ଥିର long	    prev_ts     = 0L;	// cached tabstop value
# ifdef FEAT_VARTABS
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା      *prev_vts = ଖାଲି;   // cached vartabs values
# endif
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    prev_fnum   = 0;	// cached buffer number
    ସ୍ଥିର char_u   *prev_ଧାଡ଼ି  = ଖାଲି;	// cached copy of "line"
    ସ୍ଥିର varnumber_T prev_tick = 0;   // changedtick of cached value
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା      prev_list = 0;	// cached list indent
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା      prev_listopt = 0;	// cached w_p_briopt_list value
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା      prev_no_ts = FALSE;	// cached no_ts value
    ସ୍ଥିର unsigned prev_dy_uhex = 0;	// cached 'display' "uhex" value
    // cached formatlistpat value
    ସ୍ଥିର char_u   *prev_flp = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    bri = 0;
    // window width minus window margin space, i.e. what rests for text
    ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    eff_wwidth = wp->w_width
					  - win_col_off(wp) + win_col_off2(wp);

    // In list mode, if 'listchars' "tab" isn't set, a TAB is displayed as ^I.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା no_ts = wp->w_p_list && wp->w_lcs_chars.tab1 == NUL;

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
    ଯଦି (prev_fnum != wp->w_buffer->b_fnum
	    || prev_ts != wp->w_buffer->b_p_ts
# ifdef FEAT_VARTABS
	    || prev_vts != wp->w_buffer->b_p_vts_array
# endif
	    || prev_tick != CHANGEDTICK(wp->w_buffer)
	    || prev_listopt != wp->w_briopt_list
	    || prev_no_ts != no_ts
	    || prev_dy_uhex != (dy_flags & DY_UHEX)
	    || prev_flp == ଖାଲି
	    || STRCMP(prev_flp, get_flp_ମୂଲ୍ୟ(wp->w_buffer)) != 0
	    || prev_ଧାଡ଼ି == ଖାଲି || STRCMP(prev_ଧାଡ଼ି, ଧାଡ଼ି) != 0
	)
    {
	prev_fnum = wp->w_buffer->b_fnum;
	vim_free(prev_ଧାଡ଼ି);
	prev_ଧାଡ଼ି = vim_strsave(ଧାଡ଼ି);
	prev_ts = wp->w_buffer->b_p_ts;
# ifdef FEAT_VARTABS
	prev_vts = wp->w_buffer->b_p_vts_array;
	ଯଦି (wp->w_briopt_vcol == 0)
	    prev_indent = get_indent_str_vtab(ଧାଡ଼ି,
				     (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)wp->w_buffer->b_p_ts,
				      wp->w_buffer->b_p_vts_array, no_ts);
# ଅନ୍ୟଥା
	ଯଦି (wp->w_briopt_vcol == 0)
	    prev_indent = get_indent_str(ଧାଡ଼ି,
					(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)wp->w_buffer->b_p_ts, no_ts);
# endif
	prev_tick = CHANGEDTICK(wp->w_buffer);
	prev_listopt = wp->w_briopt_list;
	prev_list = 0;
	prev_no_ts = no_ts;
	prev_dy_uhex = (dy_flags & DY_UHEX);
	vim_free(prev_flp);
	prev_flp = vim_strsave(get_flp_ମୂଲ୍ୟ(wp->w_buffer));
	// add additional indent for numbered lists
	ଯଦି (wp->w_briopt_list != 0 && wp->w_briopt_vcol == 0)
	{
	    regmatch_T	    regmatch;

	    regmatch.regprog = vim_regcomp(prev_flp,
				   RE_MAGIC + RE_STRING + RE_AUTO + RE_STRICT);

	    ଯଦି (regmatch.regprog != ଖାଲି)
	    {
		regmatch.rm_ic = FALSE;
		ଯଦି (vim_regexec(&regmatch, ଧାଡ଼ି, 0))
		{
		    ଯଦି (wp->w_briopt_list > 0)
			prev_list = wp->w_briopt_list;
		    ଅନ୍ୟଥା
		    {
			char_u	*ptr = *regmatch.startp;
			char_u	*end_ptr = *regmatch.endp;
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା	indent = 0;

			// Compute the width of the matched text.
			// Use win_chartabsize() so that TAB size is correct,
			// while wrapping is ignored.
			ଯେତେବେଳେ (ptr < end_ptr)
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
    ଯଦି (wp->w_briopt_vcol != 0)
    {
	// column value has priority
	bri = wp->w_briopt_vcol;
	prev_list = 0;
    }
    ଅନ୍ୟଥା
	bri = prev_indent + wp->w_briopt_shift;

    // Add offset for number column, if 'n' is in 'cpoptions'
    bri += win_col_off2(wp);

    // add additional indent for numbered lists
    ଯଦି (wp->w_briopt_list > 0)
	bri += prev_list;

    // indent minus the length of the showbreak string
    ଯଦି (wp->w_briopt_sbr)
	bri -= vim_strsize(get_showbreak_ମୂଲ୍ୟ(wp));


    // never indent past left window margin
    ଯଦି (bri < 0)
	bri = 0;

    // always leave at least bri_min characters on the left,
    // if text width is sufficient
    ଅନ୍ୟଥା ଯଦି (bri > eff_wwidth - wp->w_briopt_min)
	bri = (eff_wwidth - wp->w_briopt_min < 0)
					   ? 0 : eff_wwidth - wp->w_briopt_min;

    ଫେରନ୍ତୁ bri;
}
#endif

/*
 * When extra == 0: Return TRUE if the cursor is before or on the first
 *		    non-blank in the line.
 * When extra == 1: Return TRUE if the cursor is before the first non-blank in
 *		    the line.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
inindent(ପୂର୍ଣ୍ଣସଂଖ୍ୟା extra)
{
    char_u	*ptr;
    colnr_T	col;

    ପାଇଁ (col = 0, ptr = ml_get_curline(); VIM_ISWHITE(*ptr); ++col)
	++ptr;
    ଯଦି (col >= curwin->w_cursor.col + extra)
	ଫେରନ୍ତୁ TRUE;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ FALSE;
}

/*
 * op_reindent - handle reindenting a block of lines.
 */
    ଶୂନ୍ୟ
op_reindent(oparg_T *oap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*how)(ଶୂନ୍ୟ))
{
    long	i = 0;
    char_u	*l;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		amount;
    linenr_T	first_changed = 0;
    linenr_T	last_changed = 0;
    linenr_T	start_lnum = curwin->w_cursor.lnum;

    // Don't even try when 'modifiable' is off.
    ଯଦି (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	ଫେରନ୍ତୁ;
    }

    // Save for undo.  Do this once for all lines, much faster than doing this
    // for each line separately, especially when undoing.
    ଯଦି (u_savecommon(start_lnum - 1, start_lnum + oap->ଧାଡ଼ି_ଗଣନା,
				     start_lnum + oap->ଧାଡ଼ି_ଗଣନା, FALSE) == OK)
	ପାଇଁ (i = oap->ଧାଡ଼ି_ଗଣନା; --i >= 0 && !got_int; )
	{
	    // it's a slow thing to do, so give feedback so there's no worry
	    // that the computer's just hung.

	    ଯଦି (i > 1
		    && (i % 50 == 0 || i == oap->ଧାଡ଼ି_ଗଣନା - 1)
		    && oap->ଧାଡ଼ି_ଗଣନା > p_report)
		smsg(_("%ld lines to indent... "), i);

	    // Be vi-compatible: For lisp indenting the first line is not
	    // indented, unless there is only one line.
	    ଯଦି (i != oap->ଧାଡ଼ି_ଗଣନା - 1 || oap->ଧାଡ଼ି_ଗଣନା == 1
						     || how != get_lisp_indent)
	    {
		l = skipwhite(ml_get_curline());
		ଯଦି (*l == NUL)		    // empty or blank line
		    amount = 0;
		ଅନ୍ୟଥା
		    amount = how();	    // get the indent for this line

		ଯଦି (amount >= 0 && set_indent(amount, 0))
		{
		    // did change the indent, call changed_lines() later
		    ଯଦି (first_changed == 0)
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
    ଯଦି (last_changed != 0)
	changed_ଧାଡ଼ିଗୁଡ଼ିକ(first_changed, 0,
		oap->is_VIsual ? start_lnum + oap->ଧାଡ଼ି_ଗଣନା :
		last_changed + 1, 0L);
    ଅନ୍ୟଥା ଯଦି (oap->is_VIsual)
	redraw_curbuf_later(UPD_INVERTED);

    ଯଦି (oap->ଧାଡ଼ି_ଗଣନା > p_report)
    {
	i = oap->ଧାଡ଼ି_ଗଣନା - (i + 1);
	smsg(NGETTEXT("%ld line indented ",
						 "%ld lines indented ", i), i);
    }
    ଯଦି ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// set '[ and '] marks
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
    }
}

/*
 * Return TRUE if lines starting with '#' should be left aligned.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
preprocs_ବାମ(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ
	(curbuf->b_p_si && !curbuf->b_p_cin) ||
	(curbuf->b_p_cin && in_cinkeys('#', ' ', TRUE)
					   && curbuf->b_ind_hash_comment == 0)
	;
}

/*
 * Return TRUE if the conditions are OK for smart indenting.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
may_do_si(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ curbuf->b_p_si
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
    ଶୂନ୍ୟ
ins_try_si(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    pos_T	*pos, old_pos;
    char_u	*ptr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		temp;

    // do some very smart indenting when entering '{' or '}'
    ଯଦି (((did_si || can_si_back) && c == '{')
	    || (can_si && c == '}' && inindent(0)))
    {
	// for '}' set indent equal to indent of line containing matching '{'
	ଯଦି (c == '}' && (pos = findmatch(ଖାଲି, '{')) != ଖାଲି)
	{
	    old_pos = curwin->w_cursor;
	    // If the matching '{' has a ')' immediately before it (ignoring
	    // white-space), then line up with the start of the line
	    // containing the matching '(' if there is one.  This handles the
	    // case where an "if (..\n..) {" statement continues over multiple
	    // lines -- webb
	    ptr = ml_get(pos->lnum);
	    i = pos->col;
	    ଯଦି (i > 0)		// skip blanks before '{'
		ଯେତେବେଳେ (--i > 0 && VIM_ISWHITE(ptr[i]))
		    ;
	    curwin->w_cursor.lnum = pos->lnum;
	    curwin->w_cursor.col = i;
	    ଯଦି (ptr[i] == ')' && (pos = findmatch(ଖାଲି, '(')) != ଖାଲି)
		curwin->w_cursor = *pos;
	    i = get_indent();
	    curwin->w_cursor = old_pos;
	    ଯଦି (State & VREPLACE_FLAG)
		change_indent(INDENT_SET, i, FALSE, NUL, TRUE);
	    ଅନ୍ୟଥା
		(ଶୂନ୍ୟ)set_indent(i, SIN_CHANGED);
	}
	ଅନ୍ୟଥା ଯଦି (curwin->w_cursor.col > 0)
	{
	    // when inserting '{' after "O" reduce indent, but not
	    // more than indent of previous line
	    temp = TRUE;
	    ଯଦି (c == '{' && can_si_back && curwin->w_cursor.lnum > 1)
	    {
		old_pos = curwin->w_cursor;
		i = get_indent();
		ଯେତେବେଳେ (curwin->w_cursor.lnum > 1)
		{
		    ptr = skipwhite(ml_get(--(curwin->w_cursor.lnum)));

		    // ignore empty lines and lines starting with '#'.
		    ଯଦି (*ptr != '#' && *ptr != NUL)
			ଭାଙ୍ଗ;
		}
		ଯଦି (get_indent() >= i)
		    temp = FALSE;
		curwin->w_cursor = old_pos;
	    }
	    ଯଦି (temp)
		shift_ଧାଡ଼ି(TRUE, FALSE, 1, TRUE);
	}
    }

    // set indent of '#' always to 0
    ଯଦି (curwin->w_cursor.col > 0 && can_si && c == '#' && inindent(0))
    {
	// remember current indent for next line
	old_indent = get_indent();
	(ଶୂନ୍ୟ)set_indent(0, SIN_CHANGED);
    }

    // Adjust ai_col, the char at this position can be deleted.
    ଯଦି (ai_col > curwin->w_cursor.col)
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
    ଶୂନ୍ୟ
change_indent(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		amount,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		round,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		replaced,	// replaced character, put on replace stack
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		call_changed_bytes)	// call changed_bytes()
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vcol;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		last_vcol;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		insstart_less;		// reduction for Insstart.col
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		new_cursor_col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*ptr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_p_list;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		start_col;
    colnr_T	vc;
    colnr_T	orig_col = 0;		// init for GCC
    char_u	*new_ଧାଡ଼ି, *orig_ଧାଡ଼ି = ଖାଲି;	// init for GCC

    // MODE_VREPLACE state needs to know what the line was like before changing
    ଯଦି (State & VREPLACE_FLAG)
    {
	orig_ଧାଡ଼ି = vim_strnsave(ml_get_curline(), ml_get_curline_len());  // Deal with NULL below
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
    ଯଦି (new_cursor_col < 0)
	vcol = get_indent() - vcol;

    ଯଦି (new_cursor_col > 0)	    // can't fix replace stack
	start_col = -1;

    // Set the new indent.  The cursor will be put on the first non-blank.
    ଯଦି (type == INDENT_SET)
	(ଶୂନ୍ୟ)set_indent(amount, call_changed_bytes ? SIN_CHANGED : 0);
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	save_State = State;

	// Avoid being called recursively.
	ଯଦି (State & VREPLACE_FLAG)
	    State = MODE_ଭର୍ତ୍ତି;
	shift_ଧାଡ଼ି(type == INDENT_DEC, round, 1, call_changed_bytes);
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
    ଯଦି (new_cursor_col >= 0)
    {
	// When changing the indent while the cursor is touching it, reset
	// Insstart_col to 0.
	ଯଦି (new_cursor_col == 0)
	    insstart_less = MAXCOL;
	new_cursor_col += curwin->w_cursor.col;
    }
    ଅନ୍ୟଥା ଯଦି (!(State & MODE_ଭର୍ତ୍ତି))
	new_cursor_col = curwin->w_cursor.col;
    ଅନ୍ୟଥା
    {
	chartabsize_T cts;

	// Compute the screen column where the cursor should be.
	vcol = get_indent() - vcol;
	curwin->w_virtcol = (colnr_T)((vcol < 0) ? 0 : vcol);

	// Advance the cursor until we reach the right screen column.
	last_vcol = 0;
	ptr = ml_get_curline();
	init_chartabsize_arg(&cts, curwin, 0, 0, ptr, ptr);
	ଯେତେବେଳେ (cts.cts_vcol <= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curwin->w_virtcol)
	{
	    last_vcol = cts.cts_vcol;
	    ଯଦି (cts.cts_vcol > 0)
		MB_PTR_ADV(cts.cts_ptr);
	    ଯଦି (*cts.cts_ptr == NUL)
		ଭାଙ୍ଗ;
	    cts.cts_vcol += lbr_chartabsize(&cts);
	}
	vcol = last_vcol;
	new_cursor_col = cts.cts_ptr - cts.cts_ଧାଡ଼ି;
	clear_chartabsize_arg(&cts);

	// May need to insert spaces to be able to position the cursor on
	// the right screen column.
	ଯଦି (vcol != (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curwin->w_virtcol)
	{
	    curwin->w_cursor.col = (colnr_T)new_cursor_col;
	    i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curwin->w_virtcol - vcol;
	    ptr = alloc(i + 1);
	    ଯଦି (ptr != ଖାଲି)
	    {
		size_t ptrlen;
		new_cursor_col += i;
		ptr[i] = NUL;
		ptrlen = i;
		ଯେତେବେଳେ (--i >= 0)
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

    ଯଦି (new_cursor_col <= 0)
	curwin->w_cursor.col = 0;
    ଅନ୍ୟଥା
	curwin->w_cursor.col = (colnr_T)new_cursor_col;
    curwin->w_set_curswant = true;
    changed_cline_bef_curs();

    // May have to adjust the start of the insert.
    ଯଦି (State & MODE_ଭର୍ତ୍ତି)
    {
	ଯଦି (curwin->w_cursor.lnum == Insstart.lnum && Insstart.col != 0)
	{
	    ଯଦି ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Insstart.col <= insstart_less)
		Insstart.col = 0;
	    ଅନ୍ୟଥା
		Insstart.col -= insstart_less;
	}
	ଯଦି ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ai_col <= insstart_less)
	    ai_col = 0;
	ଅନ୍ୟଥା
	    ai_col -= insstart_less;
    }

    // For MODE_REPLACE state, may have to fix the replace stack, if it's
    // possible.  If the number of characters before the cursor decreased, need
    // to pop a few characters from the replace stack.
    // If the number of characters before the cursor increased, need to push a
    // few NULs onto the replace stack.
    ଯଦି (REPLACE_NORMAL(State) && start_col >= 0)
    {
	ଯେତେବେଳେ (start_col > (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curwin->w_cursor.col)
	{
	    replace_join(0);	    // remove a NUL from the replace stack
	    --start_col;
	}
	ଯେତେବେଳେ (start_col < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curwin->w_cursor.col || replaced)
	{
	    replace_ଠେଲ(NUL);
	    ଯଦି (replaced)
	    {
		replace_ଠେଲ(replaced);
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
    ଯଦି (State & VREPLACE_FLAG)
    {
	// If orig_line didn't allocate, just return.  At least we did the job,
	// even if you can't backspace.
	ଯଦି (orig_ଧାଡ଼ି == ଖାଲି)
	    ଫେରନ୍ତୁ;

	// Save new line
	new_ଧାଡ଼ି = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	ଯଦି (new_ଧାଡ଼ି == ଖାଲି)
	{
	    vim_free(orig_ଧାଡ଼ି);
	    ଫେରନ୍ତୁ;
	}

	// We only put back the new line up to the cursor
	new_ଧାଡ଼ି[curwin->w_cursor.col] = NUL;

	// Put back original line
	ml_replace(curwin->w_cursor.lnum, orig_ଧାଡ଼ି, FALSE);
	curwin->w_cursor.col = orig_col;

	// Backspace from cursor to start of line
	backspace_until_column(0);

	// Insert new stuff into line again
	ins_bytes(new_ଧାଡ଼ି);

	vim_free(new_ଧାଡ଼ି);
    }
}

/*
 * Copy the indent from ptr to the current line (and fill to size)
 * Leaves the cursor on the first non-blank in the line.
 * Returns TRUE if the line was changed.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
copy_indent(ପୂର୍ଣ୍ଣସଂଖ୍ୟା size, char_u *src)
{
    char_u	*p = ଖାଲି;
    char_u	*ଧାଡ଼ି = ଖାଲି;
    char_u	*s;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ind_len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଧାଡ଼ି_len = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tab_pad;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ind_done;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		round;
#ifdef FEAT_VARTABS
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ind_col;
#endif

    // Round 1: compute the number of characters needed for the indent
    // Round 2: copy the characters.
    ପାଇଁ (round = 1; round <= 2; ++round)
    {
	todo = size;
	ind_len = 0;
	ind_done = 0;
#ifdef FEAT_VARTABS
	ind_col = 0;
#endif
	s = src;

	// Count/copy the usable portion of the source line
	ଯେତେବେଳେ (todo > 0 && VIM_ISWHITE(*s))
	{
	    ଯଦି (*s == TAB)
	    {
#ifdef FEAT_VARTABS
		tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#ଅନ୍ୟଥା
		tab_pad = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curbuf->b_p_ts
					   - (ind_done % (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curbuf->b_p_ts);
#endif
		// Stop if this tab will overshoot the target
		ଯଦି (todo < tab_pad)
		    ଭାଙ୍ଗ;
		todo -= tab_pad;
		ind_done += tab_pad;
#ifdef FEAT_VARTABS
		ind_col += tab_pad;
#endif
	    }
	    ଅନ୍ୟଥା
	    {
		--todo;
		++ind_done;
#ifdef FEAT_VARTABS
		++ind_col;
#endif
	    }
	    ++ind_len;
	    ଯଦି (p != ଖାଲି)
		*p++ = *s;
	    ++s;
	}

	// Fill to next tabstop with a tab, if possible
#ifdef FEAT_VARTABS
	tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#ଅନ୍ୟଥା
	tab_pad = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curbuf->b_p_ts - (ind_done % (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curbuf->b_p_ts);
#endif
	ଯଦି (todo >= tab_pad && !curbuf->b_p_et)
	{
	    todo -= tab_pad;
	    ++ind_len;
#ifdef FEAT_VARTABS
	    ind_col += tab_pad;
#endif
	    ଯଦି (p != ଖାଲି)
		*p++ = TAB;
	}

	// Add tabs required for indent
	ଯଦି (!curbuf->b_p_et)
	{
#ifdef FEAT_VARTABS
	    ପାଇଁ (;;)
	    {
		tab_pad = tabstop_padding(ind_col, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
		ଯଦି (todo < tab_pad)
		    ଭାଙ୍ଗ;
		todo -= tab_pad;
		++ind_len;
		ind_col += tab_pad;
		ଯଦି (p != ଖାଲି)
		    *p++ = TAB;
	    }
#ଅନ୍ୟଥା
	    ଯେତେବେଳେ (todo >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curbuf->b_p_ts)
	    {
		todo -= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curbuf->b_p_ts;
		++ind_len;
		ଯଦି (p != ଖାଲି)
		    *p++ = TAB;
	    }
#endif
	}

	// Count/add spaces required for indent
	ଯେତେବେଳେ (todo > 0)
	{
	    --todo;
	    ++ind_len;
	    ଯଦି (p != ଖାଲି)
		*p++ = ' ';
	}

	ଯଦି (p == ଖାଲି)
	{
	    // Allocate memory for the result: the copied indent, new indent
	    // and the rest of the line.
	    ଧାଡ଼ି_len = ml_get_curline_len() + 1;
	    ଧାଡ଼ି = alloc(ind_len + ଧାଡ଼ି_len);
	    ଯଦି (ଧାଡ଼ି == ଖାଲି)
		ଫେରନ୍ତୁ FALSE;
	    p = ଧାଡ଼ି;
	}
    }

    // Append the original line
    mch_memmove(p, ml_get_curline(), (size_t)ଧାଡ଼ି_len);

    // Replace the line
    ml_replace(curwin->w_cursor.lnum, ଧାଡ଼ି, FALSE);

    // Put the cursor after the indent.
    curwin->w_cursor.col = ind_len;
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Give a "resulting text too long" error and maybe set got_int.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
emsg_text_too_long(ଶୂନ୍ୟ)
{
    emsg(_(e_resulting_text_too_long));
#ifdef FEAT_EVAL
    // when not inside a try/catch set got_int to break out of any loop
    ଯଦି (trylevel == 0)
#endif
	got_int = TRUE;
}

/*
 * ":retab".
 */
    ଶୂନ୍ୟ
ex_retab(exarg_T *eap)
{
    linenr_T	lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		got_tab = FALSE;
    long	ଅଙ୍କ_spaces = 0;
    long	ଅଙ୍କ_tabs;
    long	len;
    long	col;
    long	vcol;
    long	start_col = 0;		// For start of white-space string
    long	start_vcol = 0;		// For start of white-space string
    long	old_len;
    long	new_len;
    char_u	*ptr;
    char_u	*new_ଧାଡ଼ି = (char_u *)1; // init to non-NULL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_undo;		// called u_save for current line
#ifdef FEAT_VARTABS
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*new_vts_array = ଖାଲି;
    char_u	*new_ts_str;		// string value of tab argument
#ଅନ୍ୟଥା
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		temp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		new_ts = 0;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_list;
    linenr_T	first_ଧାଡ଼ି = 0;		// first changed line
    linenr_T	last_ଧାଡ଼ି = 0;		// last changed line
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_indent_only = 0;	// Only process leading whitespace

    save_list = curwin->w_p_list;
    curwin->w_p_list = 0;	    // don't want list mode here

    ptr = eap->arg;
    ଯଦି (STRNCMP(ptr, "-indentonly", 11) == 0 && IS_WHITE_OR_NUL(ptr[11]))
    {
	is_indent_only = 1;
	ptr = skipwhite(ptr + 11);
    }

#ifdef FEAT_VARTABS
    new_ts_str = ptr;
    ଯଦି (tabstop_set(ptr, &new_vts_array) == FAIL)
	ଫେରନ୍ତୁ;
    ଯେତେବେଳେ (vim_isdigit(*ptr) || *ptr == ',')
	++ptr;

    // This ensures that either new_vts_array and new_ts_str are freshly
    // allocated, or new_vts_array points to an existing array and new_ts_str
    // is null.
    ଯଦି (new_vts_array == ଖାଲି)
    {
	new_vts_array = curbuf->b_p_vts_array;
	new_ts_str = ଖାଲି;
    }
    ଅନ୍ୟଥା
	new_ts_str = vim_strnsave(new_ts_str, ptr - new_ts_str);
#ଅନ୍ୟଥା
    ଯଦି (ptr[0] != NUL && (ptr[0] != '0' || ptr[1] != NUL))
    {
	char_u	*end;

	ଯଦି (strtol((ଅକ୍ଷର *)ptr, (ଅକ୍ଷର **)&end, 10) <= 0)
	{
	    ଯଦି (ptr != end)
		emsg(_(e_argument_must_be_positive));
	    ଅନ୍ୟଥା
		semsg(_(e_invalid_argument_str), ptr);
	    ଫେରନ୍ତୁ;
	}
	new_ts = getdigits(&ptr);
	ଯଦି (new_ts < 0 || new_ts > TABSTOP_MAX)
	{
	    semsg(_(e_invalid_argument_str), eap->arg);
	    ଫେରନ୍ତୁ;
	}
    }
    ଯଦି (new_ts == 0)
	new_ts = curbuf->b_p_ts;
#endif
    ପାଇଁ (lnum = eap->line1; !got_int && lnum <= eap->line2; ++lnum)
    {
	ptr = ml_get(lnum);
	old_len = ml_get_len(lnum);
	col = 0;
	vcol = 0;
	did_undo = FALSE;
	ପାଇଁ (;;)
	{
	    ଯଦି (VIM_ISWHITE(ptr[col]))
	    {
		ଯଦି (!got_tab && ଅଙ୍କ_spaces == 0)
		{
		    // First consecutive white-space
		    start_vcol = vcol;
		    start_col = col;
		}
		ଯଦି (ptr[col] == ' ')
		    ଅଙ୍କ_spaces++;
		ଅନ୍ୟଥା
		    got_tab = TRUE;
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (got_tab || (eap->forceit && ଅଙ୍କ_spaces > 1))
		{
		    // Retabulate this string of white-space

		    // len is virtual length of white string
		    len = ଅଙ୍କ_spaces = vcol - start_vcol;
		    ଅଙ୍କ_tabs = 0;
		    ଯଦି (!curbuf->b_p_et)
		    {
#ifdef FEAT_VARTABS
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା t, s;

			tabstop_fromto(start_vcol, vcol,
					curbuf->b_p_ts, new_vts_array, &t, &s);
			ଅଙ୍କ_tabs = t;
			ଅଙ୍କ_spaces = s;
#ଅନ୍ୟଥା
			temp = new_ts - (start_vcol % new_ts);
			ଯଦି (ଅଙ୍କ_spaces >= temp)
			{
			    ଅଙ୍କ_spaces -= temp;
			    ଅଙ୍କ_tabs++;
			}
			ଅଙ୍କ_tabs += ଅଙ୍କ_spaces / new_ts;
			ଅଙ୍କ_spaces -= (ଅଙ୍କ_spaces / new_ts) * new_ts;
#endif
		    }
		    ଯଦି (curbuf->b_p_et || got_tab ||
					(ଅଙ୍କ_spaces + ଅଙ୍କ_tabs < len))
		    {
			ଯଦି (did_undo == FALSE)
			{
			    did_undo = TRUE;
			    ଯଦି (u_save((linenr_T)(lnum - 1),
						(linenr_T)(lnum + 1)) == FAIL)
			    {
				new_ଧାଡ଼ି = ଖାଲି;	// flag out-of-memory
				ଭାଙ୍ଗ;
			    }
			}

			// len is actual number of white characters used
			len = ଅଙ୍କ_spaces + ଅଙ୍କ_tabs;
			new_len = old_len - col + start_col + len + 1;
			ଯଦି (new_len <= 0 || new_len >= MAXCOL)
			{
			    emsg_text_too_long();
			    ଭାଙ୍ଗ;
			}
			new_ଧାଡ଼ି = alloc(new_len);
			ଯଦି (new_ଧାଡ଼ି == ଖାଲି)
			    ଭାଙ୍ଗ;
			ଯଦି (start_col > 0)
			    mch_memmove(new_ଧାଡ଼ି, ptr, (size_t)start_col);
			mch_memmove(new_ଧାଡ଼ି + start_col + len,
				      ptr + col, (size_t)(old_len - col + 1));
			ptr = new_ଧାଡ଼ି + start_col;
			ପାଇଁ (col = 0; col < len; col++)
			    ptr[col] = (col < ଅଙ୍କ_tabs) ? '\t' : ' ';
			ଯଦି (ml_replace(lnum, new_ଧାଡ଼ି, FALSE) == OK)
			    // "new_line" may have been copied
			    new_ଧାଡ଼ି = curbuf->b_ml.ml_ଧାଡ଼ି_ptr;
			ଯଦି (first_ଧାଡ଼ି == 0)
			    first_ଧାଡ଼ି = lnum;
			last_ଧାଡ଼ି = lnum;
			ptr = new_ଧାଡ଼ି;
			old_len = new_len - 1;
			col = start_col + len;
		    }
		}
		got_tab = FALSE;
		ଅଙ୍କ_spaces = 0;

		ଯଦି (is_indent_only)
		    ଭାଙ୍ଗ;
	    }
	    ଯଦି (ptr[col] == NUL)
		ଭାଙ୍ଗ;
	    vcol += chartabsize(ptr + col, (colnr_T)vcol);
	    ଯଦି (vcol >= MAXCOL)
	    {
		emsg_text_too_long();
		ଭାଙ୍ଗ;
	    }
	    ଯଦି (has_mbyte)
		col += (*mb_ptr2len)(ptr + col);
	    ଅନ୍ୟଥା
		++col;
	}
	ଯଦି (new_ଧାଡ଼ି == ଖାଲି)		    // out of memory
	    ଭାଙ୍ଗ;
	ଧାଡ଼ି_breakcheck();
    }
    ଯଦି (got_int)
	emsg(_(e_interrupted));

#ifdef FEAT_VARTABS
    // If a single value was given then it can be considered equal to
    // either the value of 'tabstop' or the value of 'vartabstop'.
    ଯଦି (tabstop_ଗଣନା(curbuf->b_p_vts_array) == 0
	&& tabstop_ଗଣନା(new_vts_array) == 1
	&& curbuf->b_p_ts == tabstop_first(new_vts_array))
	; // not changed
    ଅନ୍ୟଥା ଯଦି (tabstop_ଗଣନା(curbuf->b_p_vts_array) > 0
	&& tabstop_eq(curbuf->b_p_vts_array, new_vts_array))
	; // not changed
    ଅନ୍ୟଥା
	redraw_curbuf_later(UPD_NOT_VALID);
#ଅନ୍ୟଥା
    ଯଦି (curbuf->b_p_ts != new_ts)
	redraw_curbuf_later(UPD_NOT_VALID);
#endif
    ଯଦି (first_ଧାଡ଼ି != 0)
	changed_ଧାଡ଼ିଗୁଡ଼ିକ(first_ଧାଡ଼ି, 0, last_ଧାଡ଼ି + 1, 0L);

    curwin->w_p_list = save_list;	// restore 'list'

#ifdef FEAT_VARTABS
    ଯଦି (new_ts_str != ଖାଲି)		// set the new tabstop
    {
	// If 'vartabstop' is in use or if the value given to retab has more
	// than one tabstop then update 'vartabstop'.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା *old_vts_ary = curbuf->b_p_vts_array;

	ଯଦି (tabstop_ଗଣନା(old_vts_ary) > 0 || tabstop_ଗଣନା(new_vts_array) > 1)
	{
	    set_string_option_direct((char_u *)"vts", -1, new_ts_str,
							OPT_FREE|OPT_LOCAL, 0);
	    curbuf->b_p_vts_array = new_vts_array;
	    vim_free(old_vts_ary);
	}
	ଅନ୍ୟଥା
	{
	    // 'vartabstop' wasn't in use and a single value was given to
	    // retab then update 'tabstop'.
	    curbuf->b_p_ts = tabstop_first(new_vts_array);
	    vim_free(new_vts_array);
	}
	vim_free(new_ts_str);
    }
#ଅନ୍ୟଥା
    curbuf->b_p_ts = new_ts;
#endif
    coladvance(curwin->w_curswant);

    u_clearline();
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Get indent level from 'indentexpr'.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_ପଦସମଷ୍ଟି_indent(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		indent = -1;
    char_u	*inde_copy;
    pos_T	save_pos;
    colnr_T	save_curswant;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_set_curswant;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_State;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_sandbox = was_set_insecurely(curwin,
					    (char_u *)"indentexpr", OPT_LOCAL);
    sctx_T	save_sctx = current_sctx;

    // Save and restore cursor position and curswant, in case it was changed
    // via :normal commands
    save_pos = curwin->w_cursor;
    save_curswant = curwin->w_curswant;
    save_set_curswant = curwin->w_set_curswant;
    set_vim_var_nr(VV_LNUM, curwin->w_cursor.lnum);
    ଯଦି (use_sandbox)
	++sandbox;
    ++textlock;
    current_sctx = curbuf->b_p_script_ctx[BV_INDE];

    // Need to make a copy, the 'indentexpr' option could be changed while
    // evaluating it.
    inde_copy = vim_strsave(curbuf->b_p_inde);
    ଯଦି (inde_copy != ଖାଲି)
    {
	indent = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)eval_to_ସଂଖ୍ୟା(inde_copy, TRUE);
	vim_free(inde_copy);
    }

    ଯଦି (use_sandbox)
	--sandbox;
    --textlock;
    current_sctx = save_sctx;

    // Restore the cursor position so that 'indentexpr' doesn't need to.
    // Pretend to be in Insert mode, allow cursor past end of line for "o"
    // command.
    save_State = State;
    State = MODE_ଭର୍ତ୍ତି;
    curwin->w_cursor = save_pos;
    curwin->w_curswant = save_curswant;
    curwin->w_set_curswant = save_set_curswant;
    check_cursor();
    State = save_State;

    // Reset did_throw, unless 'debug' has "throw" and inside a try/catch.
    ଯଦି (did_throw && (vim_strchr(p_debug, 't') == ଖାଲି || trylevel == 0))
    {
	handle_did_throw();
	did_throw = FALSE;
    }

    // If there is an error, just keep the current indent.
    ଯଦି (indent < 0)
	indent = get_indent();

    ଫେରନ୍ତୁ indent;
}
#endif

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
lisp_match(char_u *p)
{
    char_u	buf[LSIZE];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    char_u	*word = *curbuf->b_p_lw != NUL ? curbuf->b_p_lw : p_lispwords;

    ଯେତେବେଳେ (*word != NUL)
    {
	len = copy_option_part(&word, buf, LSIZE, ",");
	ଯଦି (STRNCMP(buf, p, len) == 0 && IS_WHITE_OR_NUL(p[len]))
	    ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_lisp_indent(ଶୂନ୍ୟ)
{
    pos_T	*pos, realpos, paren;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		amount;
    char_u	*that;
    colnr_T	col;
    colnr_T	firsttry;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		parencount, quotecount;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vi_lisp;

    // Set vi_lisp to use the vi-compatible method
    vi_lisp = (vim_strchr(p_cpo, CPO_LISP) != ଖାଲି);

    realpos = curwin->w_cursor;
    curwin->w_cursor.col = 0;

    ଯଦି ((pos = findmatch(ଖାଲି, '(')) == ଖାଲି)
	pos = findmatch(ଖାଲି, '[');
    ଅନ୍ୟଥା
    {
	paren = *pos;
	pos = findmatch(ଖାଲି, '[');
	ଯଦି (pos == ଖାଲି || LT_POSP(pos, &paren))
	    pos = &paren;
    }
    ଯଦି (pos != ଖାଲି)
    {
	// Extra trick: Take the indent of the first previous non-white
	// line that is at the same () level.
	amount = -1;
	parencount = 0;

	ଯେତେବେଳେ (--curwin->w_cursor.lnum >= pos->lnum)
	{
	    ଯଦି (linewhite(curwin->w_cursor.lnum))
		ଜାରି;
	    ପାଇଁ (that = ml_get_curline(); *that != NUL; ++that)
	    {
		ଯଦି (*that == ';')
		{
		    ଯେତେବେଳେ (*(that + 1) != NUL)
			++that;
		    ଜାରି;
		}
		ଯଦି (*that == '\\')
		{
		    ଯଦି (*(that + 1) != NUL)
			++that;
		    ଜାରି;
		}
		ଯଦି (*that == '"' && *(that + 1) != NUL)
		{
		    ଯେତେବେଳେ (*++that && *that != '"')
		    {
			// skipping escaped characters in the string
			ଯଦି (*that == '\\')
			{
			    ଯଦି (*++that == NUL)
				ଭାଙ୍ଗ;
			    ଯଦି (that[1] == NUL)
			    {
				++that;
				ଭାଙ୍ଗ;
			    }
			}
		    }
		    ଯଦି (*that == NUL)
			ଭାଙ୍ଗ;
		}
		ଯଦି (*that == '(' || *that == '[')
		    ++parencount;
		ଅନ୍ୟଥା ଯଦି (*that == ')' || *that == ']')
		    --parencount;
	    }
	    ଯଦି (parencount == 0)
	    {
		amount = get_indent();
		ଭାଙ୍ଗ;
	    }
	}

	ଯଦି (amount == -1)
	{
	    curwin->w_cursor.lnum = pos->lnum;
	    curwin->w_cursor.col = pos->col;
	    col = pos->col;

	    that = ml_get_curline();

	    ଯଦି (vi_lisp && get_indent() == 0)
		amount = 2;
	    ଅନ୍ୟଥା
	    {
		char_u		*ଧାଡ଼ି = that;
		chartabsize_T	cts;

		init_chartabsize_arg(&cts, curwin, pos->lnum, 0, ଧାଡ଼ି, ଧାଡ଼ି);
		ଯେତେବେଳେ (*cts.cts_ptr != NUL && col > 0)
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

		ଯଦି (!vi_lisp && (*that == '(' || *that == '[')
						      && lisp_match(that + 1))
		    amount += 2;
		ଅନ୍ୟଥା
		{
		    ଯଦି (*that != NUL)
		    {
			that++;
			amount++;
		    }
		    firsttry = amount;

		    init_chartabsize_arg(&cts, curwin, (colnr_T)(that - ଧାଡ଼ି),
							   amount, ଧାଡ଼ି, that);
		    ଯେତେବେଳେ (VIM_ISWHITE(*cts.cts_ptr))
		    {
			cts.cts_vcol += lbr_chartabsize(&cts);
			++cts.cts_ptr;
		    }
		    that = cts.cts_ptr;
		    amount = cts.cts_vcol;
		    clear_chartabsize_arg(&cts);

		    ଯଦି (*that && *that != ';') // not a comment line
		    {
			// test *that != '(' to accommodate first let/do
			// argument if it is more than one line
			ଯଦି (!vi_lisp && *that != '(' && *that != '[')
			    firsttry++;

			parencount = 0;
			quotecount = 0;

			init_chartabsize_arg(&cts, curwin,
				   (colnr_T)(that - ଧାଡ଼ି), amount, ଧାଡ଼ି, that);
			ଯଦି (vi_lisp
				|| (*that != '"'
				    && *that != '\''
				    && *that != '#'
				    && (*that < '0' || *that > '9')))
			{
			    ଯେତେବେଳେ (*cts.cts_ptr
				    && (!VIM_ISWHITE(*cts.cts_ptr)
					|| quotecount
					|| parencount)
				    && (!((*cts.cts_ptr == '('
							|| *cts.cts_ptr == '[')
					    && !quotecount
					    && !parencount
					    && vi_lisp)))
			    {
				ଯଦି (*cts.cts_ptr == '"')
				    quotecount = !quotecount;
				ଯଦି ((*cts.cts_ptr == '(' || *cts.cts_ptr == '[')
							       && !quotecount)
				    ++parencount;
				ଯଦି ((*cts.cts_ptr == ')' || *cts.cts_ptr == ']')
							       && !quotecount)
				    --parencount;
				ଯଦି (*cts.cts_ptr == '\\'
						    && *(cts.cts_ptr+1) != NUL)
				    cts.cts_vcol += lbr_chartabsize_adv(&cts);
				cts.cts_vcol += lbr_chartabsize_adv(&cts);
			    }
			}
			ଯେତେବେଳେ (VIM_ISWHITE(*cts.cts_ptr))
			{
			    cts.cts_vcol += lbr_chartabsize(&cts);
			    ++cts.cts_ptr;
			}
			that = cts.cts_ptr;
			amount = cts.cts_vcol;
			clear_chartabsize_arg(&cts);

			ଯଦି (!*that || *that == ';')
			    amount = firsttry;
		    }
		}
	    }
	}
    }
    ଅନ୍ୟଥା
	amount = 0;	// no matching '(' or '[' found, use zero indent

    curwin->w_cursor = realpos;

    ଫେରନ୍ତୁ amount;
}

/*
 * Re-indent the current line, based on the current contents of it and the
 * surrounding lines. Fixing the cursor position seems really easy -- I'm very
 * confused what all the part that handles Control-T is doing that I'm not.
 * "get_the_indent" should be get_c_indent, get_expr_indent or get_lisp_indent.
 */

    ଶୂନ୍ୟ
fixthisline(ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*get_the_indent)(ଶୂନ୍ୟ))
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା amount = get_the_indent();

    ଯଦି (amount < 0)
	ଫେରନ୍ତୁ;

    change_indent(INDENT_SET, amount, FALSE, 0, TRUE);
    ଯଦି (linewhite(curwin->w_cursor.lnum))
	did_ai = TRUE;	// delete the indent if the line stays empty
}

/*
 * Return TRUE if 'indentexpr' should be used for Lisp indenting.
 * Caller may want to check 'autoindent'.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
use_indentexpr_for_lisp(ଶୂନ୍ୟ)
{
#ifdef FEAT_EVAL
    ଫେରନ୍ତୁ curbuf->b_p_lisp
		&& *curbuf->b_p_inde != NUL
		&& STRCMP(curbuf->b_p_lop, "expr:1") == 0;
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ FALSE;
#endif
}

/*
 * Fix indent for 'lisp' and 'cindent'.
 */
    ଶୂନ୍ୟ
fix_indent(ଶୂନ୍ୟ)
{
    ଯଦି (p_paste)
	ଫେରନ୍ତୁ;  // no auto-indenting when 'paste' is set
    ଯଦି (curbuf->b_p_lisp && curbuf->b_p_ai)
    {
	ଯଦି (use_indentexpr_for_lisp())
	    do_c_ପଦସମଷ୍ଟି_indent();
	ଅନ୍ୟଥା
	    fixthisline(get_lisp_indent);
    }
    ଅନ୍ୟଥା ଯଦି (cindent_on())
	do_c_ପଦସମଷ୍ଟି_indent();
}

#ଯଦି defined(FEAT_EVAL)
/*
 * "indent()" function
 */
    ଶୂନ୍ୟ
f_indent(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    ଯଦି (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    lnum = tv_get_lnum(argvars);
    ଯଦି (lnum >= 1 && lnum <= curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	rettv->vval.v_ସଂଖ୍ୟା = get_indent_lnum(lnum);
    ଅନ୍ୟଥା
    {
	ଯଦି (in_vim9script())
	    semsg(_(e_invalid_ଧାଡ଼ି_ସଂଖ୍ୟା_nr), lnum);
	rettv->vval.v_ସଂଖ୍ୟା = -1;
    }
}

/*
 * "lispindent(lnum)" function
 */
    ଶୂନ୍ୟ
f_lispindent(typval_T *argvars UNUSED, typval_T *rettv)
{
    pos_T	pos;
    linenr_T	lnum;

    ଯଦି (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    ଯଦି (lnum >= 1 && lnum <= curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_ସଂଖ୍ୟା = get_lisp_indent();
	curwin->w_cursor = pos;
    }
    ଅନ୍ୟଥା ଯଦି (in_vim9script())
	semsg(_(e_invalid_ଧାଡ଼ି_ସଂଖ୍ୟା_nr), lnum);
    ଅନ୍ୟଥା
	rettv->vval.v_ସଂଖ୍ୟା = -1;
}
#endif
