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

#شامل "vim.h"

#اگر defined(FEAT_VARTABS)

/*
 * Set the integer values corresponding to the string setting of 'vartabstop'.
 * "array" will be set, caller must free it if needed.
 * Return FAIL for an error.
 */
    صحیح
tabstop_set(char_u *var, صحیح **array)
{
    صحیح	    valcount = 1;
    صحیح	    t;
    char_u  *cp;

    اگر (var[0] == NUL || (var[0] == '0' && var[1] == NUL))
    {
	*array = پوچ;
	بازگشت OK;
    }

    برای (cp = var; *cp != NUL; ++cp)
    {
	اگر (cp == var || cp[-1] == ',')
	{
	    char_u *end;

	    اگر (strtol((نویسه *)cp, (نویسه **)&end, 10) <= 0)
	    {
		اگر (cp != end)
		    emsg(_(e_argument_must_be_positive));
		وگرنه
		    semsg(_(e_invalid_argument_str), cp);
		بازگشت FAIL;
	    }
	}

	اگر (VIM_ISDIGIT(*cp))
	    ادامه;
	اگر (cp[0] == ',' && cp > var && cp[-1] != ',' && cp[1] != NUL)
	{
	    ++valcount;
	    ادامه;
	}
	semsg(_(e_invalid_argument_str), var);
	بازگشت FAIL;
    }

    *array = ALLOC_MULT(صحیح, valcount + 1);
    اگر (*array == پوچ)
	بازگشت FAIL;
    (*array)[0] = valcount;

    t = 1;
    برای (cp = var; *cp != NUL;)
    {
	صحیح n = atoi((نویسه *)cp);

	// Catch negative values, overflow and ridiculous big values.
	اگر (n <= 0 || n > TABSTOP_MAX)
	{
	    semsg(_(e_invalid_argument_str), cp);
	    VIM_CLEAR(*array);
	    بازگشت FAIL;
	}
	(*array)[t++] = n;
	وقتی (*cp != NUL && *cp != ',')
	    ++cp;
	اگر (*cp != NUL)
	    ++cp;
    }

    بازگشت OK;
}

/*
 * Calculate the number of screen spaces a tab will occupy.
 * If "vts" is set then the tab widths are taken from that array,
 * otherwise the value of ts is used.
 */
    صحیح
tabstop_padding(colnr_T col, صحیح ts_arg, صحیح *vts)
{
    صحیح		ts = ts_arg == 0 ? 8 : ts_arg;
    صحیح		tabcount;
    colnr_T	tabcol = 0;
    صحیح		t;
    صحیح		padding = 0;

    اگر (vts == پوچ || vts[0] == 0)
	بازگشت ts - (col % ts);

    tabcount = vts[0];

    برای (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	اگر (tabcol > col)
	{
	    padding = (صحیح)(tabcol - col);
	    بشکن;
	}
    }
    اگر (t > tabcount)
	padding = vts[tabcount] - (صحیح)((col - tabcol) % vts[tabcount]);

    بازگشت padding;
}

/*
 * Find the size of the tab that covers a particular column.
 *
 * If this is being called as part of a shift operation, col is not the cursor
 * column but is the column number to the left of the first non-whitespace
 * character in the line.  If the shift is to the left (left = TRUE), then
 * return the size of the tab interval to the left of the column.
 */
    صحیح
tabstop_at(colnr_T col, صحیح ts, صحیح *vts, صحیح چپ)
{
    صحیح		tabcount;	// Number of tab stops in the list of variable
				// tab stops.
    colnr_T	tabcol = 0;	// Column of the tab stop under consideration.
    صحیح		t;		// Tabstop index in the list of variable tab
				// stops.
    صحیح		tab_size = 0;	// Size of the tab stop interval to the right
				// or left of the col.

    اگر (vts == 0 || vts[0] == 0)
	بازگشت ts;

    tabcount = vts[0];
    برای (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	اگر (tabcol > col)
	{
	    // If shifting left (left != 0), and if the column to the left of
	    // the first first non-blank character (col) in the line is
	    // already to the left of the first tabstop, set the shift amount
	    // (tab_size) to just enough to shift the line to the left margin.
	    // The value doesn't seem to matter as long as it is at least that
	    // distance.
	    اگر (چپ && (t == 1))
		tab_size = col;
	    وگرنه
		tab_size = vts[t - (چپ ? 1 : 0)];
	    بشکن;
	}
    }
    اگر (t > tabcount)		// If the value of the index t is beyond the
				// end of the list, use the tab stop value at
				// the end of the list.
	tab_size = vts[tabcount];

    بازگشت tab_size;
}

/*
 * Find the column on which a tab starts.
 */
    colnr_T
tabstop_start(colnr_T col, صحیح ts, صحیح *vts)
{
    صحیح		tabcount;
    colnr_T	tabcol = 0;
    صحیح		t;
    صحیح		excess;

    اگر (vts == پوچ || vts[0] == 0)
	بازگشت col - col % ts;

    tabcount = vts[0];
    برای (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	اگر (tabcol > col)
	    بازگشت tabcol - vts[t];
    }

    excess = tabcol % vts[tabcount];
    بازگشت col - (col - excess) % vts[tabcount];
}

/*
 * Find the number of tabs and spaces necessary to get from one column
 * to another.
 */
    تهی
tabstop_fromto(
	colnr_T start_col,
	colnr_T end_col,
	صحیح	ts_arg,
	صحیح	*vts,
	صحیح	*ntabs,
	صحیح	*nspcs)
{
    صحیح		spaces = end_col - start_col;
    colnr_T	tabcol = 0;
    صحیح		padding = 0;
    صحیح		tabcount;
    صحیح		t;
    صحیح		ts = ts_arg == 0 ? curbuf->b_p_ts : ts_arg;

    اگر (vts == پوچ || vts[0] == 0)
    {
	صحیح tabs = 0;
	صحیح initspc = 0;

	initspc = ts - (start_col % ts);
	اگر (spaces >= initspc)
	{
	    spaces -= initspc;
	    tabs++;
	}
	tabs += spaces / ts;
	spaces -= (spaces / ts) * ts;

	*ntabs = tabs;
	*nspcs = spaces;
	بازگشت;
    }

    // Find the padding needed to reach the next tabstop.
    tabcount = vts[0];
    برای (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	اگر (tabcol > start_col)
	{
	    padding = (صحیح)(tabcol - start_col);
	    بشکن;
	}
    }
    اگر (t > tabcount)
	padding = vts[tabcount] - (صحیح)((start_col - tabcol) % vts[tabcount]);

    // If the space needed is less than the padding no tabs can be used.
    اگر (spaces < padding)
    {
	*ntabs = 0;
	*nspcs = spaces;
	بازگشت;
    }

    *ntabs = 1;
    spaces -= padding;

    // At least one tab has been used. See if any more will fit.
    وقتی (spaces != 0 && ++t <= tabcount)
    {
	padding = vts[t];
	اگر (spaces < padding)
	{
	    *nspcs = spaces;
	    بازگشت;
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
    ایستا صحیح
tabstop_eq(صحیح *ts1, صحیح *ts2)
{
    صحیح		t;

    اگر ((ts1 == 0 && ts2) || (ts1 && ts2 == 0))
	بازگشت FALSE;
    اگر (ts1 == ts2)
	بازگشت TRUE;
    اگر (ts1[0] != ts2[0])
	بازگشت FALSE;

    برای (t = 1; t <= ts1[0]; ++t)
	اگر (ts1[t] != ts2[t])
	    بازگشت FALSE;

    بازگشت TRUE;
}

# اگر defined(FEAT_BEVAL)
/*
 * Copy a tabstop array, allocating space for the new array.
 */
    صحیح *
tabstop_copy(صحیح *oldts)
{
    صحیح		*newts;
    صحیح		t;

    اگر (oldts == پوچ)
	بازگشت پوچ;
    newts = ALLOC_MULT(صحیح, oldts[0] + 1);
    اگر (newts != پوچ)
	برای (t = 0; t <= oldts[0]; ++t)
	    newts[t] = oldts[t];
    بازگشت newts;
}
# endif

/*
 * Return a count of the number of tabstops.
 */
    صحیح
tabstop_شمار(صحیح *ts)
{
    بازگشت ts != پوچ ? ts[0] : 0;
}

/*
 * Return the first tabstop, or 8 if there are no tabstops defined.
 */
    صحیح
tabstop_first(صحیح *ts)
{
    بازگشت ts != پوچ ? ts[1] : 8;
}

#endif

/*
 * Return the effective shiftwidth value for current buffer, using the
 * 'tabstop' value when 'shiftwidth' is zero.
 */
    long
get_sw_مقدار(buf_T *buf)
{
    بازگشت get_sw_مقدار_col(buf, 0, FALSE);
}

/*
 * Idem, using "pos".
 */
    ایستا long
get_sw_مقدار_pos(buf_T *buf, pos_T *pos, صحیح چپ)
{
    pos_T save_cursor = curwin->w_cursor;
    long sw_مقدار;

    curwin->w_cursor = *pos;
    sw_مقدار = get_sw_مقدار_col(buf, get_nolist_virtcol(), چپ);
    curwin->w_cursor = save_cursor;
    بازگشت sw_مقدار;
}

/*
 * Idem, using the first non-black in the current line.
 */
    long
get_sw_مقدار_indent(buf_T *buf, صحیح چپ)
{
    pos_T pos = curwin->w_cursor;

    pos.col = getwhitecols_curline();
    بازگشت get_sw_مقدار_pos(buf, &pos, چپ);
}

/*
 * Idem, using virtual column "col".
 */
    long
get_sw_مقدار_col(buf_T *buf, colnr_T col UNUSED, صحیح چپ UNUSED)
{
    بازگشت buf->b_p_sw ? buf->b_p_sw :
#ifdef FEAT_VARTABS
	tabstop_at(col, buf->b_p_ts, buf->b_p_vts_array, چپ);
#وگرنه
	buf->b_p_ts;
#endif
}

/*
 * Return the effective softtabstop value for the current buffer, using the
 * 'shiftwidth' value when 'softtabstop' is negative.
 */
    long
get_sts_مقدار(تهی)
{
    بازگشت curbuf->b_p_sts < 0 ? get_sw_مقدار(curbuf) : curbuf->b_p_sts;
}

/*
 * Count the size (in window cells) of the indent in the current line.
 */
    صحیح
get_indent(تهی)
{
#ifdef FEAT_VARTABS
    بازگشت get_indent_str_vtab(ml_get_curline(), (صحیح)curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#وگرنه
    بازگشت get_indent_str(ml_get_curline(), (صحیح)curbuf->b_p_ts, FALSE);
#endif
}

/*
 * Count the size (in window cells) of the indent in line "lnum".
 */
    صحیح
get_indent_lnum(linenr_T lnum)
{
#ifdef FEAT_VARTABS
    بازگشت get_indent_str_vtab(ml_get(lnum), (صحیح)curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#وگرنه
    بازگشت get_indent_str(ml_get(lnum), (صحیح)curbuf->b_p_ts, FALSE);
#endif
}

#اگر defined(FEAT_FOLDING)
/*
 * Count the size (in window cells) of the indent in line "lnum" of buffer
 * "buf".
 */
    صحیح
get_indent_buf(buf_T *buf, linenr_T lnum)
{
# ifdef FEAT_VARTABS
    بازگشت get_indent_str_vtab(ml_get_buf(buf, lnum, FALSE),
			       (صحیح)buf->b_p_ts, buf->b_p_vts_array, FALSE);
# وگرنه
    بازگشت get_indent_str(ml_get_buf(buf, lnum, FALSE), (صحیح)buf->b_p_ts, FALSE);
# endif
}
#endif

/*
 * count the size (in window cells) of the indent in line "ptr", with
 * 'tabstop' at "ts"
 */
    صحیح
get_indent_str(
    char_u	*ptr,
    صحیح		ts,
    صحیح		no_ts) // if TRUE, count a tab as ^I
{
    صحیح		شمار = 0;

    برای ( ; *ptr; ++ptr)
    {
	اگر (*ptr == TAB)    // count a tab for what it is worth
	{
	    اگر (!no_ts)
		شمار += ts - (شمار % ts);
	    وگرنه
		شمار += ptr2cells(ptr);
	}
	وگرنه اگر (*ptr == ' ')
	    ++شمار;		// count a space for one
	وگرنه
	    بشکن;
    }
    بازگشت شمار;
}

#اگر defined(FEAT_VARTABS)
/*
 * Count the size (in window cells) of the indent in line "ptr", using
 * variable tabstops.
 * If "no_ts" is TRUE, count a tab as ^I.
 */
    صحیح
get_indent_str_vtab(char_u *ptr, صحیح ts, صحیح *vts, صحیح no_ts)
{
    صحیح		شمار = 0;

    برای ( ; *ptr; ++ptr)
    {
	اگر (*ptr == TAB)    // count a tab for what it is worth
	{
	    اگر (!no_ts)
		شمار += tabstop_padding(شمار, ts, vts);
	    وگرنه
		شمار += ptr2cells(ptr);
	}
	وگرنه اگر (*ptr == ' ')
	    ++شمار;		// count a space for one
	وگرنه
	    بشکن;
    }
    بازگشت شمار;
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
    صحیح
set_indent(
    صحیح		size,		    // measured in spaces
    صحیح		flags)
{
    char_u	*p;
    char_u	*newline;
    char_u	*oldline;
    char_u	*s;
    صحیح		todo;
    صحیح		ind_len;	    // measured in characters
    صحیح		سطر_len;	    // size of the line (including the NUL)
    صحیح		doit = FALSE;
    صحیح		ind_done = 0;	    // measured in spaces
#ifdef FEAT_VARTABS
    صحیح		ind_col = 0;
#endif
    صحیح		tab_pad;
    صحیح		retval = FALSE;
    صحیح		orig_char_len = -1; // number of initial whitespace chars when
				    // 'et' and 'pi' are both set

    // First check if there is anything to do and compute the number of
    // characters needed for the indent.
    todo = size;
    ind_len = 0;
    p = oldline = ml_get_curline();
    سطر_len = ml_get_curline_len() + 1;

    // Calculate the buffer size for the new indent, and check to see if it
    // isn't already set

    // if 'expandtab' isn't set: use TABs; if both 'expandtab' and
    // 'preserveindent' are set count the number of characters at the
    // beginning of the line to be copied
    اگر (!curbuf->b_p_et || (!(flags & SIN_درج) && curbuf->b_p_pi))
    {
	// If 'preserveindent' is set then reuse as much as possible of
	// the existing indent structure for the new indent
	اگر (!(flags & SIN_درج) && curbuf->b_p_pi)
	{
	    ind_done = 0;

	    // count as many characters as we can use
	    وقتی (todo > 0 && VIM_ISWHITE(*p))
	    {
		اگر (*p == TAB)
		{
#ifdef FEAT_VARTABS
		    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#وگرنه
		    tab_pad = (صحیح)curbuf->b_p_ts
					   - (ind_done % (صحیح)curbuf->b_p_ts);
#endif
		    // stop if this tab will overshoot the target
		    اگر (todo < tab_pad)
			بشکن;
		    todo -= tab_pad;
		    ++ind_len;
		    ind_done += tab_pad;
		}
		وگرنه
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
	    اگر (curbuf->b_p_et)
		orig_char_len = ind_len;

	    // Fill to next tabstop with a tab, if possible
#ifdef FEAT_VARTABS
	    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
						curbuf->b_p_vts_array);
#وگرنه
	    tab_pad = (صحیح)curbuf->b_p_ts - (ind_done % (صحیح)curbuf->b_p_ts);
#endif
	    اگر (todo >= tab_pad && orig_char_len == -1)
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
	برای (;;)
	{
	    tab_pad = tabstop_padding(ind_col, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
	    اگر (todo < tab_pad)
		بشکن;
	    اگر (*p != TAB)
		doit = TRUE;
	    وگرنه
		++p;
	    todo -= tab_pad;
	    ++ind_len;
	    ind_col += tab_pad;
	}
#وگرنه
	وقتی (todo >= (صحیح)curbuf->b_p_ts)
	{
	    اگر (*p != TAB)
		doit = TRUE;
	    وگرنه
		++p;
	    todo -= (صحیح)curbuf->b_p_ts;
	    ++ind_len;
	    // ind_done += (int)curbuf->b_p_ts;
	}
#endif
    }
    // count spaces required for indent
    وقتی (todo > 0)
    {
	اگر (*p != ' ')
	    doit = TRUE;
	وگرنه
	    ++p;
	--todo;
	++ind_len;
	// ++ind_done;
    }

    // Return if the indent is OK already.
    اگر (!doit && !VIM_ISWHITE(*p) && !(flags & SIN_درج))
	بازگشت FALSE;

    // Allocate memory for the new line.
    اگر (flags & SIN_درج)
	p = oldline;
    وگرنه
    {
	p = skipwhite(p);
	سطر_len -= (صحیح)(p - oldline);
    }

    // If 'preserveindent' and 'expandtab' are both set keep the original
    // characters and allocate accordingly.  We will fill the rest with spaces
    // after the if (!curbuf->b_p_et) below.
    اگر (orig_char_len != -1)
    {
	newline = alloc(orig_char_len + size - ind_done + سطر_len);
	اگر (newline == پوچ)
	    بازگشت FALSE;
	todo = size - ind_done;
	ind_len = orig_char_len + todo;    // Set total length of indent in
					   // characters, which may have been
					   // undercounted until now
	p = oldline;
	s = newline;
	وقتی (orig_char_len > 0)
	{
	    *s++ = *p++;
	    orig_char_len--;
	}

	// Skip over any additional white space (useful when newindent is less
	// than old)
	وقتی (VIM_ISWHITE(*p))
	    ++p;

    }
    وگرنه
    {
	todo = size;
	newline = alloc(ind_len + سطر_len);
	اگر (newline == پوچ)
	    بازگشت FALSE;
	s = newline;
    }

    // Put the characters in the new line.
    // if 'expandtab' isn't set: use TABs
    اگر (!curbuf->b_p_et)
    {
	// If 'preserveindent' is set then reuse as much as possible of
	// the existing indent structure for the new indent
	اگر (!(flags & SIN_درج) && curbuf->b_p_pi)
	{
	    p = oldline;
	    ind_done = 0;

	    وقتی (todo > 0 && VIM_ISWHITE(*p))
	    {
		اگر (*p == TAB)
		{
#ifdef FEAT_VARTABS
		    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#وگرنه
		    tab_pad = (صحیح)curbuf->b_p_ts
					   - (ind_done % (صحیح)curbuf->b_p_ts);
#endif
		    // stop if this tab will overshoot the target
		    اگر (todo < tab_pad)
			بشکن;
		    todo -= tab_pad;
		    ind_done += tab_pad;
		}
		وگرنه
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
#وگرنه
	    tab_pad = (صحیح)curbuf->b_p_ts - (ind_done % (صحیح)curbuf->b_p_ts);
#endif
	    اگر (todo >= tab_pad)
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
	برای (;;)
	{
	    tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
	    اگر (todo < tab_pad)
		بشکن;
	    *s++ = TAB;
	    todo -= tab_pad;
	    ind_done += tab_pad;
	}
#وگرنه
	وقتی (todo >= (صحیح)curbuf->b_p_ts)
	{
	    *s++ = TAB;
	    todo -= (صحیح)curbuf->b_p_ts;
	}
#endif
    }
    وقتی (todo > 0)
    {
	*s++ = ' ';
	--todo;
    }
    mch_memmove(s, p, (size_t)سطر_len);

    // Replace the line (unless undo fails).
    اگر (!(flags & SIN_UNDO) || u_savesub(curwin->w_cursor.lnum) == OK)
    {
	colnr_T old_offset = (colnr_T)(p - oldline);
	colnr_T new_offset = (colnr_T)(s - newline);

	// this may free "newline"
	ml_replace(curwin->w_cursor.lnum, newline, FALSE);
	اگر (flags & SIN_CHANGED)
	    changed_bytes(curwin->w_cursor.lnum, 0);

	// Correct saved cursor position if it is in this line.
	اگر (saved_cursor.lnum == curwin->w_cursor.lnum)
	{
	    اگر (saved_cursor.col >= old_offset)
		// cursor was after the indent, adjust for the number of
		// bytes added/removed
		saved_cursor.col += ind_len - old_offset;
	    وگرنه اگر (saved_cursor.col >= new_offset)
		// cursor was in the indent, and is now after it, put it back
		// at the start of the indent (replacing spaces with TAB)
		saved_cursor.col = new_offset;
	}
#ifdef FEAT_PROP_POPUP
	اگر (curbuf->b_has_textprop)
	{
	    صحیح added = ind_len - old_offset;

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
    وگرنه
	vim_free(newline);

    curwin->w_cursor.col = ind_len;
    بازگشت retval;
}

/*
 * Return the indent of the current line after a number.  Return -1 if no
 * number was found.  Used for 'n' in 'formatoptions': numbered list.
 * Since a pattern is used it can actually handle more than numbers.
 */
    صحیح
get_عدد_indent(linenr_T lnum)
{
    colnr_T	col;
    pos_T	pos;

    regmatch_T	regmatch;
    صحیح		lead_len = 0;	// length of comment leader

    اگر (lnum > curbuf->b_ml.ml_سطر_شمار)
	بازگشت -1;
    pos.lnum = 0;

    // In format_lines() (i.e. not insert mode), fo+=q is needed too...
    اگر ((State & MODE_درج) || has_format_option(FO_Q_COMS))
	lead_len = get_leader_len(ml_get(lnum), پوچ, FALSE, TRUE);

    regmatch.regprog = vim_regcomp(curbuf->b_p_flp, RE_MAGIC);
    اگر (regmatch.regprog != پوچ)
    {
	regmatch.rm_ic = FALSE;

	// vim_regexec() expects a pointer to a line.  This lets us
	// start matching for the flp beyond any comment leader...
	اگر (vim_regexec(&regmatch, ml_get(lnum) + lead_len, (colnr_T)0))
	{
	    pos.lnum = lnum;
	    pos.col = (colnr_T)(*regmatch.endp - ml_get(lnum));
	    pos.coladd = 0;
	}
	vim_regfree(regmatch.regprog);
    }

    اگر (pos.lnum == 0 || *ml_get_pos(&pos) == NUL)
	بازگشت -1;
    getvcol(curwin, &pos, &col, پوچ, پوچ, 0);
    بازگشت (صحیح)col;
}

#اگر defined(FEAT_LINEBREAK)
/*
 * Check "briopt" as 'breakindentopt' and update the members of "wp".
 * This is called when 'breakindentopt' is changed and when a window is
 * initialized.
 * Returns FAIL for failure, OK otherwise.
 */
    صحیح
briopt_check(
    char_u	*briopt,	// when NULL: use "wp->w_p_briopt"
    win_T	*wp)		// when NULL: only check "briopt"
{
    char_u	*p;
    صحیح		bri_shift = 0;
    long	bri_min = 20;
    صحیح		bri_sbr = FALSE;
    صحیح		bri_list = 0;
    صحیح		bri_vcol = 0;

    اگر (briopt != پوچ)
	p = briopt;
    وگرنه
	p = wp->w_p_briopt;

    وقتی (*p != NUL)
    {
	// Note: Keep this in sync with p_briopt_values
	اگر (STRNCMP(p, "shift:", 6) == 0
		 && ((p[6] == '-' && VIM_ISDIGIT(p[7])) || VIM_ISDIGIT(p[6])))
	{
	    p += 6;
	    bri_shift = getdigits(&p);
	}
	وگرنه اگر (STRNCMP(p, "min:", 4) == 0 && VIM_ISDIGIT(p[4]))
	{
	    p += 4;
	    bri_min = getdigits(&p);
	}
	وگرنه اگر (STRNCMP(p, "sbr", 3) == 0)
	{
	    p += 3;
	    bri_sbr = TRUE;
	}
	وگرنه اگر (STRNCMP(p, "list:", 5) == 0)
	{
	    p += 5;
	    bri_list = getdigits(&p);
	}
	وگرنه اگر (STRNCMP(p, "column:", 7) == 0)
	{
	    p += 7;
	    bri_vcol = getdigits(&p);
	}
	اگر (*p != ',' && *p != NUL)
	    بازگشت FAIL;
	اگر (*p == ',')
	    ++p;
    }

    اگر (wp == پوچ)
	بازگشت OK;

    wp->w_briopt_shift = bri_shift;
    wp->w_briopt_min   = bri_min;
    wp->w_briopt_sbr   = bri_sbr;
    wp->w_briopt_list  = bri_list;
    wp->w_briopt_vcol  = bri_vcol;

    بازگشت OK;
}

/*
 * Return appropriate space number for breakindent, taking influencing
 * parameters into account. Window must be specified, since it is not
 * necessarily always the current one.
 */
    صحیح
get_breakindent_win(
    win_T	*wp,
    char_u	*سطر) // start of the line
{
    ایستا صحیح	    prev_indent = 0;	// cached indent value
    ایستا long	    prev_ts     = 0L;	// cached tabstop value
# ifdef FEAT_VARTABS
    ایستا صحیح      *prev_vts = پوچ;   // cached vartabs values
# endif
    ایستا صحیح	    prev_fnum   = 0;	// cached buffer number
    ایستا char_u   *prev_سطر  = پوچ;	// cached copy of "line"
    ایستا varnumber_T prev_tick = 0;   // changedtick of cached value
    ایستا صحیح      prev_list = 0;	// cached list indent
    ایستا صحیح      prev_listopt = 0;	// cached w_p_briopt_list value
    ایستا صحیح      prev_no_ts = FALSE;	// cached no_ts value
    ایستا unsigned prev_dy_uhex = 0;	// cached 'display' "uhex" value
    // cached formatlistpat value
    ایستا char_u   *prev_flp = پوچ;
    صحیح		    bri = 0;
    // window width minus window margin space, i.e. what rests for text
    ثابت صحیح	    eff_wwidth = wp->w_width
					  - win_col_off(wp) + win_col_off2(wp);

    // In list mode, if 'listchars' "tab" isn't set, a TAB is displayed as ^I.
    صحیح no_ts = wp->w_p_list && wp->w_lcs_chars.tab1 == NUL;

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
    اگر (prev_fnum != wp->w_buffer->b_fnum
	    || prev_ts != wp->w_buffer->b_p_ts
# ifdef FEAT_VARTABS
	    || prev_vts != wp->w_buffer->b_p_vts_array
# endif
	    || prev_tick != CHANGEDTICK(wp->w_buffer)
	    || prev_listopt != wp->w_briopt_list
	    || prev_no_ts != no_ts
	    || prev_dy_uhex != (dy_flags & DY_UHEX)
	    || prev_flp == پوچ
	    || STRCMP(prev_flp, get_flp_مقدار(wp->w_buffer)) != 0
	    || prev_سطر == پوچ || STRCMP(prev_سطر, سطر) != 0
	)
    {
	prev_fnum = wp->w_buffer->b_fnum;
	vim_free(prev_سطر);
	prev_سطر = vim_strsave(سطر);
	prev_ts = wp->w_buffer->b_p_ts;
# ifdef FEAT_VARTABS
	prev_vts = wp->w_buffer->b_p_vts_array;
	اگر (wp->w_briopt_vcol == 0)
	    prev_indent = get_indent_str_vtab(سطر,
				     (صحیح)wp->w_buffer->b_p_ts,
				      wp->w_buffer->b_p_vts_array, no_ts);
# وگرنه
	اگر (wp->w_briopt_vcol == 0)
	    prev_indent = get_indent_str(سطر,
					(صحیح)wp->w_buffer->b_p_ts, no_ts);
# endif
	prev_tick = CHANGEDTICK(wp->w_buffer);
	prev_listopt = wp->w_briopt_list;
	prev_list = 0;
	prev_no_ts = no_ts;
	prev_dy_uhex = (dy_flags & DY_UHEX);
	vim_free(prev_flp);
	prev_flp = vim_strsave(get_flp_مقدار(wp->w_buffer));
	// add additional indent for numbered lists
	اگر (wp->w_briopt_list != 0 && wp->w_briopt_vcol == 0)
	{
	    regmatch_T	    regmatch;

	    regmatch.regprog = vim_regcomp(prev_flp,
				   RE_MAGIC + RE_STRING + RE_AUTO + RE_STRICT);

	    اگر (regmatch.regprog != پوچ)
	    {
		regmatch.rm_ic = FALSE;
		اگر (vim_regexec(&regmatch, سطر, 0))
		{
		    اگر (wp->w_briopt_list > 0)
			prev_list = wp->w_briopt_list;
		    وگرنه
		    {
			char_u	*ptr = *regmatch.startp;
			char_u	*end_ptr = *regmatch.endp;
			صحیح	indent = 0;

			// Compute the width of the matched text.
			// Use win_chartabsize() so that TAB size is correct,
			// while wrapping is ignored.
			وقتی (ptr < end_ptr)
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
    اگر (wp->w_briopt_vcol != 0)
    {
	// column value has priority
	bri = wp->w_briopt_vcol;
	prev_list = 0;
    }
    وگرنه
	bri = prev_indent + wp->w_briopt_shift;

    // Add offset for number column, if 'n' is in 'cpoptions'
    bri += win_col_off2(wp);

    // add additional indent for numbered lists
    اگر (wp->w_briopt_list > 0)
	bri += prev_list;

    // indent minus the length of the showbreak string
    اگر (wp->w_briopt_sbr)
	bri -= vim_strsize(get_showbreak_مقدار(wp));


    // never indent past left window margin
    اگر (bri < 0)
	bri = 0;

    // always leave at least bri_min characters on the left,
    // if text width is sufficient
    وگرنه اگر (bri > eff_wwidth - wp->w_briopt_min)
	bri = (eff_wwidth - wp->w_briopt_min < 0)
					   ? 0 : eff_wwidth - wp->w_briopt_min;

    بازگشت bri;
}
#endif

/*
 * When extra == 0: Return TRUE if the cursor is before or on the first
 *		    non-blank in the line.
 * When extra == 1: Return TRUE if the cursor is before the first non-blank in
 *		    the line.
 */
    صحیح
inindent(صحیح extra)
{
    char_u	*ptr;
    colnr_T	col;

    برای (col = 0, ptr = ml_get_curline(); VIM_ISWHITE(*ptr); ++col)
	++ptr;
    اگر (col >= curwin->w_cursor.col + extra)
	بازگشت TRUE;
    وگرنه
	بازگشت FALSE;
}

/*
 * op_reindent - handle reindenting a block of lines.
 */
    تهی
op_reindent(oparg_T *oap, صحیح (*how)(تهی))
{
    long	i = 0;
    char_u	*l;
    صحیح		amount;
    linenr_T	first_changed = 0;
    linenr_T	last_changed = 0;
    linenr_T	start_lnum = curwin->w_cursor.lnum;

    // Don't even try when 'modifiable' is off.
    اگر (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	بازگشت;
    }

    // Save for undo.  Do this once for all lines, much faster than doing this
    // for each line separately, especially when undoing.
    اگر (u_savecommon(start_lnum - 1, start_lnum + oap->سطر_شمار,
				     start_lnum + oap->سطر_شمار, FALSE) == OK)
	برای (i = oap->سطر_شمار; --i >= 0 && !got_int; )
	{
	    // it's a slow thing to do, so give feedback so there's no worry
	    // that the computer's just hung.

	    اگر (i > 1
		    && (i % 50 == 0 || i == oap->سطر_شمار - 1)
		    && oap->سطر_شمار > p_report)
		smsg(_("%ld lines to indent... "), i);

	    // Be vi-compatible: For lisp indenting the first line is not
	    // indented, unless there is only one line.
	    اگر (i != oap->سطر_شمار - 1 || oap->سطر_شمار == 1
						     || how != get_lisp_indent)
	    {
		l = skipwhite(ml_get_curline());
		اگر (*l == NUL)		    // empty or blank line
		    amount = 0;
		وگرنه
		    amount = how();	    // get the indent for this line

		اگر (amount >= 0 && set_indent(amount, 0))
		{
		    // did change the indent, call changed_lines() later
		    اگر (first_changed == 0)
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
    اگر (last_changed != 0)
	changed_سطرها(first_changed, 0,
		oap->is_VIsual ? start_lnum + oap->سطر_شمار :
		last_changed + 1, 0L);
    وگرنه اگر (oap->is_VIsual)
	redraw_curbuf_later(UPD_INVERTED);

    اگر (oap->سطر_شمار > p_report)
    {
	i = oap->سطر_شمار - (i + 1);
	smsg(NGETTEXT("%ld line indented ",
						 "%ld lines indented ", i), i);
    }
    اگر ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// set '[ and '] marks
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
    }
}

/*
 * Return TRUE if lines starting with '#' should be left aligned.
 */
    صحیح
preprocs_چپ(تهی)
{
    بازگشت
	(curbuf->b_p_si && !curbuf->b_p_cin) ||
	(curbuf->b_p_cin && in_cinkeys('#', ' ', TRUE)
					   && curbuf->b_ind_hash_comment == 0)
	;
}

/*
 * Return TRUE if the conditions are OK for smart indenting.
 */
    صحیح
may_do_si(تهی)
{
    بازگشت curbuf->b_p_si
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
    تهی
ins_try_si(صحیح c)
{
    pos_T	*pos, old_pos;
    char_u	*ptr;
    صحیح		i;
    صحیح		temp;

    // do some very smart indenting when entering '{' or '}'
    اگر (((did_si || can_si_back) && c == '{')
	    || (can_si && c == '}' && inindent(0)))
    {
	// for '}' set indent equal to indent of line containing matching '{'
	اگر (c == '}' && (pos = findmatch(پوچ, '{')) != پوچ)
	{
	    old_pos = curwin->w_cursor;
	    // If the matching '{' has a ')' immediately before it (ignoring
	    // white-space), then line up with the start of the line
	    // containing the matching '(' if there is one.  This handles the
	    // case where an "if (..\n..) {" statement continues over multiple
	    // lines -- webb
	    ptr = ml_get(pos->lnum);
	    i = pos->col;
	    اگر (i > 0)		// skip blanks before '{'
		وقتی (--i > 0 && VIM_ISWHITE(ptr[i]))
		    ;
	    curwin->w_cursor.lnum = pos->lnum;
	    curwin->w_cursor.col = i;
	    اگر (ptr[i] == ')' && (pos = findmatch(پوچ, '(')) != پوچ)
		curwin->w_cursor = *pos;
	    i = get_indent();
	    curwin->w_cursor = old_pos;
	    اگر (State & VREPLACE_FLAG)
		change_indent(INDENT_SET, i, FALSE, NUL, TRUE);
	    وگرنه
		(تهی)set_indent(i, SIN_CHANGED);
	}
	وگرنه اگر (curwin->w_cursor.col > 0)
	{
	    // when inserting '{' after "O" reduce indent, but not
	    // more than indent of previous line
	    temp = TRUE;
	    اگر (c == '{' && can_si_back && curwin->w_cursor.lnum > 1)
	    {
		old_pos = curwin->w_cursor;
		i = get_indent();
		وقتی (curwin->w_cursor.lnum > 1)
		{
		    ptr = skipwhite(ml_get(--(curwin->w_cursor.lnum)));

		    // ignore empty lines and lines starting with '#'.
		    اگر (*ptr != '#' && *ptr != NUL)
			بشکن;
		}
		اگر (get_indent() >= i)
		    temp = FALSE;
		curwin->w_cursor = old_pos;
	    }
	    اگر (temp)
		shift_سطر(TRUE, FALSE, 1, TRUE);
	}
    }

    // set indent of '#' always to 0
    اگر (curwin->w_cursor.col > 0 && can_si && c == '#' && inindent(0))
    {
	// remember current indent for next line
	old_indent = get_indent();
	(تهی)set_indent(0, SIN_CHANGED);
    }

    // Adjust ai_col, the char at this position can be deleted.
    اگر (ai_col > curwin->w_cursor.col)
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
    تهی
change_indent(
    صحیح		type,
    صحیح		amount,
    صحیح		round,
    صحیح		replaced,	// replaced character, put on replace stack
    صحیح		call_changed_bytes)	// call changed_bytes()
{
    صحیح		vcol;
    صحیح		last_vcol;
    صحیح		insstart_less;		// reduction for Insstart.col
    صحیح		new_cursor_col;
    صحیح		i;
    char_u	*ptr;
    صحیح		save_p_list;
    صحیح		start_col;
    colnr_T	vc;
    colnr_T	orig_col = 0;		// init for GCC
    char_u	*new_سطر, *orig_سطر = پوچ;	// init for GCC

    // MODE_VREPLACE state needs to know what the line was like before changing
    اگر (State & VREPLACE_FLAG)
    {
	orig_سطر = vim_strnsave(ml_get_curline(), ml_get_curline_len());  // Deal with NULL below
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
    اگر (new_cursor_col < 0)
	vcol = get_indent() - vcol;

    اگر (new_cursor_col > 0)	    // can't fix replace stack
	start_col = -1;

    // Set the new indent.  The cursor will be put on the first non-blank.
    اگر (type == INDENT_SET)
	(تهی)set_indent(amount, call_changed_bytes ? SIN_CHANGED : 0);
    وگرنه
    {
	صحیح	save_State = State;

	// Avoid being called recursively.
	اگر (State & VREPLACE_FLAG)
	    State = MODE_درج;
	shift_سطر(type == INDENT_DEC, round, 1, call_changed_bytes);
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
    اگر (new_cursor_col >= 0)
    {
	// When changing the indent while the cursor is touching it, reset
	// Insstart_col to 0.
	اگر (new_cursor_col == 0)
	    insstart_less = MAXCOL;
	new_cursor_col += curwin->w_cursor.col;
    }
    وگرنه اگر (!(State & MODE_درج))
	new_cursor_col = curwin->w_cursor.col;
    وگرنه
    {
	chartabsize_T cts;

	// Compute the screen column where the cursor should be.
	vcol = get_indent() - vcol;
	curwin->w_virtcol = (colnr_T)((vcol < 0) ? 0 : vcol);

	// Advance the cursor until we reach the right screen column.
	last_vcol = 0;
	ptr = ml_get_curline();
	init_chartabsize_arg(&cts, curwin, 0, 0, ptr, ptr);
	وقتی (cts.cts_vcol <= (صحیح)curwin->w_virtcol)
	{
	    last_vcol = cts.cts_vcol;
	    اگر (cts.cts_vcol > 0)
		MB_PTR_ADV(cts.cts_ptr);
	    اگر (*cts.cts_ptr == NUL)
		بشکن;
	    cts.cts_vcol += lbr_chartabsize(&cts);
	}
	vcol = last_vcol;
	new_cursor_col = cts.cts_ptr - cts.cts_سطر;
	clear_chartabsize_arg(&cts);

	// May need to insert spaces to be able to position the cursor on
	// the right screen column.
	اگر (vcol != (صحیح)curwin->w_virtcol)
	{
	    curwin->w_cursor.col = (colnr_T)new_cursor_col;
	    i = (صحیح)curwin->w_virtcol - vcol;
	    ptr = alloc(i + 1);
	    اگر (ptr != پوچ)
	    {
		size_t ptrlen;
		new_cursor_col += i;
		ptr[i] = NUL;
		ptrlen = i;
		وقتی (--i >= 0)
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

    اگر (new_cursor_col <= 0)
	curwin->w_cursor.col = 0;
    وگرنه
	curwin->w_cursor.col = (colnr_T)new_cursor_col;
    curwin->w_set_curswant = true;
    changed_cline_bef_curs();

    // May have to adjust the start of the insert.
    اگر (State & MODE_درج)
    {
	اگر (curwin->w_cursor.lnum == Insstart.lnum && Insstart.col != 0)
	{
	    اگر ((صحیح)Insstart.col <= insstart_less)
		Insstart.col = 0;
	    وگرنه
		Insstart.col -= insstart_less;
	}
	اگر ((صحیح)ai_col <= insstart_less)
	    ai_col = 0;
	وگرنه
	    ai_col -= insstart_less;
    }

    // For MODE_REPLACE state, may have to fix the replace stack, if it's
    // possible.  If the number of characters before the cursor decreased, need
    // to pop a few characters from the replace stack.
    // If the number of characters before the cursor increased, need to push a
    // few NULs onto the replace stack.
    اگر (REPLACE_NORMAL(State) && start_col >= 0)
    {
	وقتی (start_col > (صحیح)curwin->w_cursor.col)
	{
	    replace_join(0);	    // remove a NUL from the replace stack
	    --start_col;
	}
	وقتی (start_col < (صحیح)curwin->w_cursor.col || replaced)
	{
	    replace_افزودن(NUL);
	    اگر (replaced)
	    {
		replace_افزودن(replaced);
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
    اگر (State & VREPLACE_FLAG)
    {
	// If orig_line didn't allocate, just return.  At least we did the job,
	// even if you can't backspace.
	اگر (orig_سطر == پوچ)
	    بازگشت;

	// Save new line
	new_سطر = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	اگر (new_سطر == پوچ)
	{
	    vim_free(orig_سطر);
	    بازگشت;
	}

	// We only put back the new line up to the cursor
	new_سطر[curwin->w_cursor.col] = NUL;

	// Put back original line
	ml_replace(curwin->w_cursor.lnum, orig_سطر, FALSE);
	curwin->w_cursor.col = orig_col;

	// Backspace from cursor to start of line
	backspace_until_column(0);

	// Insert new stuff into line again
	ins_bytes(new_سطر);

	vim_free(new_سطر);
    }
}

/*
 * Copy the indent from ptr to the current line (and fill to size)
 * Leaves the cursor on the first non-blank in the line.
 * Returns TRUE if the line was changed.
 */
    صحیح
copy_indent(صحیح size, char_u *src)
{
    char_u	*p = پوچ;
    char_u	*سطر = پوچ;
    char_u	*s;
    صحیح		todo;
    صحیح		ind_len;
    صحیح		سطر_len = 0;
    صحیح		tab_pad;
    صحیح		ind_done;
    صحیح		round;
#ifdef FEAT_VARTABS
    صحیح		ind_col;
#endif

    // Round 1: compute the number of characters needed for the indent
    // Round 2: copy the characters.
    برای (round = 1; round <= 2; ++round)
    {
	todo = size;
	ind_len = 0;
	ind_done = 0;
#ifdef FEAT_VARTABS
	ind_col = 0;
#endif
	s = src;

	// Count/copy the usable portion of the source line
	وقتی (todo > 0 && VIM_ISWHITE(*s))
	{
	    اگر (*s == TAB)
	    {
#ifdef FEAT_VARTABS
		tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#وگرنه
		tab_pad = (صحیح)curbuf->b_p_ts
					   - (ind_done % (صحیح)curbuf->b_p_ts);
#endif
		// Stop if this tab will overshoot the target
		اگر (todo < tab_pad)
		    بشکن;
		todo -= tab_pad;
		ind_done += tab_pad;
#ifdef FEAT_VARTABS
		ind_col += tab_pad;
#endif
	    }
	    وگرنه
	    {
		--todo;
		++ind_done;
#ifdef FEAT_VARTABS
		++ind_col;
#endif
	    }
	    ++ind_len;
	    اگر (p != پوچ)
		*p++ = *s;
	    ++s;
	}

	// Fill to next tabstop with a tab, if possible
#ifdef FEAT_VARTABS
	tab_pad = tabstop_padding(ind_done, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#وگرنه
	tab_pad = (صحیح)curbuf->b_p_ts - (ind_done % (صحیح)curbuf->b_p_ts);
#endif
	اگر (todo >= tab_pad && !curbuf->b_p_et)
	{
	    todo -= tab_pad;
	    ++ind_len;
#ifdef FEAT_VARTABS
	    ind_col += tab_pad;
#endif
	    اگر (p != پوچ)
		*p++ = TAB;
	}

	// Add tabs required for indent
	اگر (!curbuf->b_p_et)
	{
#ifdef FEAT_VARTABS
	    برای (;;)
	    {
		tab_pad = tabstop_padding(ind_col, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
		اگر (todo < tab_pad)
		    بشکن;
		todo -= tab_pad;
		++ind_len;
		ind_col += tab_pad;
		اگر (p != پوچ)
		    *p++ = TAB;
	    }
#وگرنه
	    وقتی (todo >= (صحیح)curbuf->b_p_ts)
	    {
		todo -= (صحیح)curbuf->b_p_ts;
		++ind_len;
		اگر (p != پوچ)
		    *p++ = TAB;
	    }
#endif
	}

	// Count/add spaces required for indent
	وقتی (todo > 0)
	{
	    --todo;
	    ++ind_len;
	    اگر (p != پوچ)
		*p++ = ' ';
	}

	اگر (p == پوچ)
	{
	    // Allocate memory for the result: the copied indent, new indent
	    // and the rest of the line.
	    سطر_len = ml_get_curline_len() + 1;
	    سطر = alloc(ind_len + سطر_len);
	    اگر (سطر == پوچ)
		بازگشت FALSE;
	    p = سطر;
	}
    }

    // Append the original line
    mch_memmove(p, ml_get_curline(), (size_t)سطر_len);

    // Replace the line
    ml_replace(curwin->w_cursor.lnum, سطر, FALSE);

    // Put the cursor after the indent.
    curwin->w_cursor.col = ind_len;
    بازگشت TRUE;
}

/*
 * Give a "resulting text too long" error and maybe set got_int.
 */
    ایستا تهی
emsg_text_too_long(تهی)
{
    emsg(_(e_resulting_text_too_long));
#ifdef FEAT_EVAL
    // when not inside a try/catch set got_int to break out of any loop
    اگر (trylevel == 0)
#endif
	got_int = TRUE;
}

/*
 * ":retab".
 */
    تهی
ex_retab(exarg_T *eap)
{
    linenr_T	lnum;
    صحیح		got_tab = FALSE;
    long	رقم_spaces = 0;
    long	رقم_tabs;
    long	len;
    long	col;
    long	vcol;
    long	start_col = 0;		// For start of white-space string
    long	start_vcol = 0;		// For start of white-space string
    long	old_len;
    long	new_len;
    char_u	*ptr;
    char_u	*new_سطر = (char_u *)1; // init to non-NULL
    صحیح		did_undo;		// called u_save for current line
#ifdef FEAT_VARTABS
    صحیح		*new_vts_array = پوچ;
    char_u	*new_ts_str;		// string value of tab argument
#وگرنه
    صحیح		temp;
    صحیح		new_ts = 0;
#endif
    صحیح		save_list;
    linenr_T	first_سطر = 0;		// first changed line
    linenr_T	last_سطر = 0;		// last changed line
    صحیح		is_indent_only = 0;	// Only process leading whitespace

    save_list = curwin->w_p_list;
    curwin->w_p_list = 0;	    // don't want list mode here

    ptr = eap->arg;
    اگر (STRNCMP(ptr, "-indentonly", 11) == 0 && IS_WHITE_OR_NUL(ptr[11]))
    {
	is_indent_only = 1;
	ptr = skipwhite(ptr + 11);
    }

#ifdef FEAT_VARTABS
    new_ts_str = ptr;
    اگر (tabstop_set(ptr, &new_vts_array) == FAIL)
	بازگشت;
    وقتی (vim_isdigit(*ptr) || *ptr == ',')
	++ptr;

    // This ensures that either new_vts_array and new_ts_str are freshly
    // allocated, or new_vts_array points to an existing array and new_ts_str
    // is null.
    اگر (new_vts_array == پوچ)
    {
	new_vts_array = curbuf->b_p_vts_array;
	new_ts_str = پوچ;
    }
    وگرنه
	new_ts_str = vim_strnsave(new_ts_str, ptr - new_ts_str);
#وگرنه
    اگر (ptr[0] != NUL && (ptr[0] != '0' || ptr[1] != NUL))
    {
	char_u	*end;

	اگر (strtol((نویسه *)ptr, (نویسه **)&end, 10) <= 0)
	{
	    اگر (ptr != end)
		emsg(_(e_argument_must_be_positive));
	    وگرنه
		semsg(_(e_invalid_argument_str), ptr);
	    بازگشت;
	}
	new_ts = getdigits(&ptr);
	اگر (new_ts < 0 || new_ts > TABSTOP_MAX)
	{
	    semsg(_(e_invalid_argument_str), eap->arg);
	    بازگشت;
	}
    }
    اگر (new_ts == 0)
	new_ts = curbuf->b_p_ts;
#endif
    برای (lnum = eap->line1; !got_int && lnum <= eap->line2; ++lnum)
    {
	ptr = ml_get(lnum);
	old_len = ml_get_len(lnum);
	col = 0;
	vcol = 0;
	did_undo = FALSE;
	برای (;;)
	{
	    اگر (VIM_ISWHITE(ptr[col]))
	    {
		اگر (!got_tab && رقم_spaces == 0)
		{
		    // First consecutive white-space
		    start_vcol = vcol;
		    start_col = col;
		}
		اگر (ptr[col] == ' ')
		    رقم_spaces++;
		وگرنه
		    got_tab = TRUE;
	    }
	    وگرنه
	    {
		اگر (got_tab || (eap->forceit && رقم_spaces > 1))
		{
		    // Retabulate this string of white-space

		    // len is virtual length of white string
		    len = رقم_spaces = vcol - start_vcol;
		    رقم_tabs = 0;
		    اگر (!curbuf->b_p_et)
		    {
#ifdef FEAT_VARTABS
			صحیح t, s;

			tabstop_fromto(start_vcol, vcol,
					curbuf->b_p_ts, new_vts_array, &t, &s);
			رقم_tabs = t;
			رقم_spaces = s;
#وگرنه
			temp = new_ts - (start_vcol % new_ts);
			اگر (رقم_spaces >= temp)
			{
			    رقم_spaces -= temp;
			    رقم_tabs++;
			}
			رقم_tabs += رقم_spaces / new_ts;
			رقم_spaces -= (رقم_spaces / new_ts) * new_ts;
#endif
		    }
		    اگر (curbuf->b_p_et || got_tab ||
					(رقم_spaces + رقم_tabs < len))
		    {
			اگر (did_undo == FALSE)
			{
			    did_undo = TRUE;
			    اگر (u_save((linenr_T)(lnum - 1),
						(linenr_T)(lnum + 1)) == FAIL)
			    {
				new_سطر = پوچ;	// flag out-of-memory
				بشکن;
			    }
			}

			// len is actual number of white characters used
			len = رقم_spaces + رقم_tabs;
			new_len = old_len - col + start_col + len + 1;
			اگر (new_len <= 0 || new_len >= MAXCOL)
			{
			    emsg_text_too_long();
			    بشکن;
			}
			new_سطر = alloc(new_len);
			اگر (new_سطر == پوچ)
			    بشکن;
			اگر (start_col > 0)
			    mch_memmove(new_سطر, ptr, (size_t)start_col);
			mch_memmove(new_سطر + start_col + len,
				      ptr + col, (size_t)(old_len - col + 1));
			ptr = new_سطر + start_col;
			برای (col = 0; col < len; col++)
			    ptr[col] = (col < رقم_tabs) ? '\t' : ' ';
			اگر (ml_replace(lnum, new_سطر, FALSE) == OK)
			    // "new_line" may have been copied
			    new_سطر = curbuf->b_ml.ml_سطر_ptr;
			اگر (first_سطر == 0)
			    first_سطر = lnum;
			last_سطر = lnum;
			ptr = new_سطر;
			old_len = new_len - 1;
			col = start_col + len;
		    }
		}
		got_tab = FALSE;
		رقم_spaces = 0;

		اگر (is_indent_only)
		    بشکن;
	    }
	    اگر (ptr[col] == NUL)
		بشکن;
	    vcol += chartabsize(ptr + col, (colnr_T)vcol);
	    اگر (vcol >= MAXCOL)
	    {
		emsg_text_too_long();
		بشکن;
	    }
	    اگر (has_mbyte)
		col += (*mb_ptr2len)(ptr + col);
	    وگرنه
		++col;
	}
	اگر (new_سطر == پوچ)		    // out of memory
	    بشکن;
	سطر_breakcheck();
    }
    اگر (got_int)
	emsg(_(e_interrupted));

#ifdef FEAT_VARTABS
    // If a single value was given then it can be considered equal to
    // either the value of 'tabstop' or the value of 'vartabstop'.
    اگر (tabstop_شمار(curbuf->b_p_vts_array) == 0
	&& tabstop_شمار(new_vts_array) == 1
	&& curbuf->b_p_ts == tabstop_first(new_vts_array))
	; // not changed
    وگرنه اگر (tabstop_شمار(curbuf->b_p_vts_array) > 0
	&& tabstop_eq(curbuf->b_p_vts_array, new_vts_array))
	; // not changed
    وگرنه
	redraw_curbuf_later(UPD_NOT_VALID);
#وگرنه
    اگر (curbuf->b_p_ts != new_ts)
	redraw_curbuf_later(UPD_NOT_VALID);
#endif
    اگر (first_سطر != 0)
	changed_سطرها(first_سطر, 0, last_سطر + 1, 0L);

    curwin->w_p_list = save_list;	// restore 'list'

#ifdef FEAT_VARTABS
    اگر (new_ts_str != پوچ)		// set the new tabstop
    {
	// If 'vartabstop' is in use or if the value given to retab has more
	// than one tabstop then update 'vartabstop'.
	صحیح *old_vts_ary = curbuf->b_p_vts_array;

	اگر (tabstop_شمار(old_vts_ary) > 0 || tabstop_شمار(new_vts_array) > 1)
	{
	    set_string_option_direct((char_u *)"vts", -1, new_ts_str,
							OPT_FREE|OPT_LOCAL, 0);
	    curbuf->b_p_vts_array = new_vts_array;
	    vim_free(old_vts_ary);
	}
	وگرنه
	{
	    // 'vartabstop' wasn't in use and a single value was given to
	    // retab then update 'tabstop'.
	    curbuf->b_p_ts = tabstop_first(new_vts_array);
	    vim_free(new_vts_array);
	}
	vim_free(new_ts_str);
    }
#وگرنه
    curbuf->b_p_ts = new_ts;
#endif
    coladvance(curwin->w_curswant);

    u_clearline();
}

#اگر defined(FEAT_EVAL)
/*
 * Get indent level from 'indentexpr'.
 */
    صحیح
get_عبارت_indent(تهی)
{
    صحیح		indent = -1;
    char_u	*inde_copy;
    pos_T	save_pos;
    colnr_T	save_curswant;
    صحیح		save_set_curswant;
    صحیح		save_State;
    صحیح		use_sandbox = was_set_insecurely(curwin,
					    (char_u *)"indentexpr", OPT_LOCAL);
    sctx_T	save_sctx = current_sctx;

    // Save and restore cursor position and curswant, in case it was changed
    // via :normal commands
    save_pos = curwin->w_cursor;
    save_curswant = curwin->w_curswant;
    save_set_curswant = curwin->w_set_curswant;
    set_vim_var_nr(VV_LNUM, curwin->w_cursor.lnum);
    اگر (use_sandbox)
	++sandbox;
    ++textlock;
    current_sctx = curbuf->b_p_script_ctx[BV_INDE];

    // Need to make a copy, the 'indentexpr' option could be changed while
    // evaluating it.
    inde_copy = vim_strsave(curbuf->b_p_inde);
    اگر (inde_copy != پوچ)
    {
	indent = (صحیح)eval_to_عدد(inde_copy, TRUE);
	vim_free(inde_copy);
    }

    اگر (use_sandbox)
	--sandbox;
    --textlock;
    current_sctx = save_sctx;

    // Restore the cursor position so that 'indentexpr' doesn't need to.
    // Pretend to be in Insert mode, allow cursor past end of line for "o"
    // command.
    save_State = State;
    State = MODE_درج;
    curwin->w_cursor = save_pos;
    curwin->w_curswant = save_curswant;
    curwin->w_set_curswant = save_set_curswant;
    check_cursor();
    State = save_State;

    // Reset did_throw, unless 'debug' has "throw" and inside a try/catch.
    اگر (did_throw && (vim_strchr(p_debug, 't') == پوچ || trylevel == 0))
    {
	handle_did_throw();
	did_throw = FALSE;
    }

    // If there is an error, just keep the current indent.
    اگر (indent < 0)
	indent = get_indent();

    بازگشت indent;
}
#endif

    ایستا صحیح
lisp_match(char_u *p)
{
    char_u	buf[LSIZE];
    صحیح		len;
    char_u	*word = *curbuf->b_p_lw != NUL ? curbuf->b_p_lw : p_lispwords;

    وقتی (*word != NUL)
    {
	len = copy_option_part(&word, buf, LSIZE, ",");
	اگر (STRNCMP(buf, p, len) == 0 && IS_WHITE_OR_NUL(p[len]))
	    بازگشت TRUE;
    }
    بازگشت FALSE;
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
    صحیح
get_lisp_indent(تهی)
{
    pos_T	*pos, realpos, paren;
    صحیح		amount;
    char_u	*that;
    colnr_T	col;
    colnr_T	firsttry;
    صحیح		parencount, quotecount;
    صحیح		vi_lisp;

    // Set vi_lisp to use the vi-compatible method
    vi_lisp = (vim_strchr(p_cpo, CPO_LISP) != پوچ);

    realpos = curwin->w_cursor;
    curwin->w_cursor.col = 0;

    اگر ((pos = findmatch(پوچ, '(')) == پوچ)
	pos = findmatch(پوچ, '[');
    وگرنه
    {
	paren = *pos;
	pos = findmatch(پوچ, '[');
	اگر (pos == پوچ || LT_POSP(pos, &paren))
	    pos = &paren;
    }
    اگر (pos != پوچ)
    {
	// Extra trick: Take the indent of the first previous non-white
	// line that is at the same () level.
	amount = -1;
	parencount = 0;

	وقتی (--curwin->w_cursor.lnum >= pos->lnum)
	{
	    اگر (linewhite(curwin->w_cursor.lnum))
		ادامه;
	    برای (that = ml_get_curline(); *that != NUL; ++that)
	    {
		اگر (*that == ';')
		{
		    وقتی (*(that + 1) != NUL)
			++that;
		    ادامه;
		}
		اگر (*that == '\\')
		{
		    اگر (*(that + 1) != NUL)
			++that;
		    ادامه;
		}
		اگر (*that == '"' && *(that + 1) != NUL)
		{
		    وقتی (*++that && *that != '"')
		    {
			// skipping escaped characters in the string
			اگر (*that == '\\')
			{
			    اگر (*++that == NUL)
				بشکن;
			    اگر (that[1] == NUL)
			    {
				++that;
				بشکن;
			    }
			}
		    }
		    اگر (*that == NUL)
			بشکن;
		}
		اگر (*that == '(' || *that == '[')
		    ++parencount;
		وگرنه اگر (*that == ')' || *that == ']')
		    --parencount;
	    }
	    اگر (parencount == 0)
	    {
		amount = get_indent();
		بشکن;
	    }
	}

	اگر (amount == -1)
	{
	    curwin->w_cursor.lnum = pos->lnum;
	    curwin->w_cursor.col = pos->col;
	    col = pos->col;

	    that = ml_get_curline();

	    اگر (vi_lisp && get_indent() == 0)
		amount = 2;
	    وگرنه
	    {
		char_u		*سطر = that;
		chartabsize_T	cts;

		init_chartabsize_arg(&cts, curwin, pos->lnum, 0, سطر, سطر);
		وقتی (*cts.cts_ptr != NUL && col > 0)
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

		اگر (!vi_lisp && (*that == '(' || *that == '[')
						      && lisp_match(that + 1))
		    amount += 2;
		وگرنه
		{
		    اگر (*that != NUL)
		    {
			that++;
			amount++;
		    }
		    firsttry = amount;

		    init_chartabsize_arg(&cts, curwin, (colnr_T)(that - سطر),
							   amount, سطر, that);
		    وقتی (VIM_ISWHITE(*cts.cts_ptr))
		    {
			cts.cts_vcol += lbr_chartabsize(&cts);
			++cts.cts_ptr;
		    }
		    that = cts.cts_ptr;
		    amount = cts.cts_vcol;
		    clear_chartabsize_arg(&cts);

		    اگر (*that && *that != ';') // not a comment line
		    {
			// test *that != '(' to accommodate first let/do
			// argument if it is more than one line
			اگر (!vi_lisp && *that != '(' && *that != '[')
			    firsttry++;

			parencount = 0;
			quotecount = 0;

			init_chartabsize_arg(&cts, curwin,
				   (colnr_T)(that - سطر), amount, سطر, that);
			اگر (vi_lisp
				|| (*that != '"'
				    && *that != '\''
				    && *that != '#'
				    && (*that < '0' || *that > '9')))
			{
			    وقتی (*cts.cts_ptr
				    && (!VIM_ISWHITE(*cts.cts_ptr)
					|| quotecount
					|| parencount)
				    && (!((*cts.cts_ptr == '('
							|| *cts.cts_ptr == '[')
					    && !quotecount
					    && !parencount
					    && vi_lisp)))
			    {
				اگر (*cts.cts_ptr == '"')
				    quotecount = !quotecount;
				اگر ((*cts.cts_ptr == '(' || *cts.cts_ptr == '[')
							       && !quotecount)
				    ++parencount;
				اگر ((*cts.cts_ptr == ')' || *cts.cts_ptr == ']')
							       && !quotecount)
				    --parencount;
				اگر (*cts.cts_ptr == '\\'
						    && *(cts.cts_ptr+1) != NUL)
				    cts.cts_vcol += lbr_chartabsize_adv(&cts);
				cts.cts_vcol += lbr_chartabsize_adv(&cts);
			    }
			}
			وقتی (VIM_ISWHITE(*cts.cts_ptr))
			{
			    cts.cts_vcol += lbr_chartabsize(&cts);
			    ++cts.cts_ptr;
			}
			that = cts.cts_ptr;
			amount = cts.cts_vcol;
			clear_chartabsize_arg(&cts);

			اگر (!*that || *that == ';')
			    amount = firsttry;
		    }
		}
	    }
	}
    }
    وگرنه
	amount = 0;	// no matching '(' or '[' found, use zero indent

    curwin->w_cursor = realpos;

    بازگشت amount;
}

/*
 * Re-indent the current line, based on the current contents of it and the
 * surrounding lines. Fixing the cursor position seems really easy -- I'm very
 * confused what all the part that handles Control-T is doing that I'm not.
 * "get_the_indent" should be get_c_indent, get_expr_indent or get_lisp_indent.
 */

    تهی
fixthisline(صحیح (*get_the_indent)(تهی))
{
    صحیح amount = get_the_indent();

    اگر (amount < 0)
	بازگشت;

    change_indent(INDENT_SET, amount, FALSE, 0, TRUE);
    اگر (linewhite(curwin->w_cursor.lnum))
	did_ai = TRUE;	// delete the indent if the line stays empty
}

/*
 * Return TRUE if 'indentexpr' should be used for Lisp indenting.
 * Caller may want to check 'autoindent'.
 */
    صحیح
use_indentexpr_for_lisp(تهی)
{
#ifdef FEAT_EVAL
    بازگشت curbuf->b_p_lisp
		&& *curbuf->b_p_inde != NUL
		&& STRCMP(curbuf->b_p_lop, "expr:1") == 0;
#وگرنه
    بازگشت FALSE;
#endif
}

/*
 * Fix indent for 'lisp' and 'cindent'.
 */
    تهی
fix_indent(تهی)
{
    اگر (p_paste)
	بازگشت;  // no auto-indenting when 'paste' is set
    اگر (curbuf->b_p_lisp && curbuf->b_p_ai)
    {
	اگر (use_indentexpr_for_lisp())
	    do_c_عبارت_indent();
	وگرنه
	    fixthisline(get_lisp_indent);
    }
    وگرنه اگر (cindent_on())
	do_c_عبارت_indent();
}

#اگر defined(FEAT_EVAL)
/*
 * "indent()" function
 */
    تهی
f_indent(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    اگر (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	بازگشت;

    lnum = tv_get_lnum(argvars);
    اگر (lnum >= 1 && lnum <= curbuf->b_ml.ml_سطر_شمار)
	rettv->vval.v_عدد = get_indent_lnum(lnum);
    وگرنه
    {
	اگر (in_vim9script())
	    semsg(_(e_invalid_سطر_عدد_nr), lnum);
	rettv->vval.v_عدد = -1;
    }
}

/*
 * "lispindent(lnum)" function
 */
    تهی
f_lispindent(typval_T *argvars UNUSED, typval_T *rettv)
{
    pos_T	pos;
    linenr_T	lnum;

    اگر (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	بازگشت;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    اگر (lnum >= 1 && lnum <= curbuf->b_ml.ml_سطر_شمار)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_عدد = get_lisp_indent();
	curwin->w_cursor = pos;
    }
    وگرنه اگر (in_vim9script())
	semsg(_(e_invalid_سطر_عدد_nr), lnum);
    وگرنه
	rettv->vval.v_عدد = -1;
}
#endif
