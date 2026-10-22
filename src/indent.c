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
tabstop_سیٹ(char_u *var, صحیح **array)
{
    صحیح	    valcount = 1;
    صحیح	    t;
    char_u  *cp;

    اگر (var[0] == NUL || (var[0] == '0' && var[1] == NUL))
    {
	*array = تہی;
	واپس OK;
    }

    برائے (cp = var; *cp != NUL; ++cp)
    {
	اگر (cp == var || cp[-1] == ',')
	{
	    char_u *انتہا;

	    اگر (strtol((حرف *)cp, (حرف **)&انتہا, 10) <= 0)
	    {
		اگر (cp != انتہا)
		    emsg(_(e_دلیل_must_be_مثبت));
		ورنہ
		    semsg(_(e_invalid_دلیل_str), cp);
		واپس FAIL;
	    }
	}

	اگر (VIM_ISDIGIT(*cp))
	    جاری;
	اگر (cp[0] == ',' && cp > var && cp[-1] != ',' && cp[1] != NUL)
	{
	    ++valcount;
	    جاری;
	}
	semsg(_(e_invalid_دلیل_str), var);
	واپس FAIL;
    }

    *array = ALLOC_MULT(صحیح, valcount + 1);
    اگر (*array == تہی)
	واپس FAIL;
    (*array)[0] = valcount;

    t = 1;
    برائے (cp = var; *cp != NUL;)
    {
	صحیح n = atoi((حرف *)cp);

	// Catch negative values, overflow and ridiculous big values.
	اگر (n <= 0 || n > TABSTOP_MAX)
	{
	    semsg(_(e_invalid_دلیل_str), cp);
	    VIM_CLEAR(*array);
	    واپس FAIL;
	}
	(*array)[t++] = n;
	جب (*cp != NUL && *cp != ',')
	    ++cp;
	اگر (*cp != NUL)
	    ++cp;
    }

    واپس OK;
}

/*
 * Calculate the number of screen spaces a tab will occupy.
 * If "vts" is set then the tab widths are taken from that array,
 * otherwise the value of ts is used.
 */
    صحیح
tabstop_padding(colnr_T ستون, صحیح ts_arg, صحیح *vts)
{
    صحیح		ts = ts_arg == 0 ? 8 : ts_arg;
    صحیح		tabcount;
    colnr_T	tabcol = 0;
    صحیح		t;
    صحیح		padding = 0;

    اگر (vts == تہی || vts[0] == 0)
	واپس ts - (ستون % ts);

    tabcount = vts[0];

    برائے (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	اگر (tabcol > ستون)
	{
	    padding = (صحیح)(tabcol - ستون);
	    توڑو;
	}
    }
    اگر (t > tabcount)
	padding = vts[tabcount] - (صحیح)((ستون - tabcol) % vts[tabcount]);

    واپس padding;
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
tabstop_at(colnr_T ستون, صحیح ts, صحیح *vts, صحیح بایاں)
{
    صحیح		tabcount;	// Number of tab stops in the list of variable
				// tab stops.
    colnr_T	tabcol = 0;	// Column of the tab stop under consideration.
    صحیح		t;		// Tabstop index in the list of variable tab
				// stops.
    صحیح		tab_سائز = 0;	// Size of the tab stop interval to the right
				// or left of the col.

    اگر (vts == 0 || vts[0] == 0)
	واپس ts;

    tabcount = vts[0];
    برائے (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	اگر (tabcol > ستون)
	{
	    // If shifting left (left != 0), and if the column to the left of
	    // the first first non-blank character (col) in the line is
	    // already to the left of the first tabstop, set the shift amount
	    // (tab_size) to just enough to shift the line to the left margin.
	    // The value doesn't seem to matter as long as it is at least that
	    // distance.
	    اگر (بایاں && (t == 1))
		tab_سائز = ستون;
	    ورنہ
		tab_سائز = vts[t - (بایاں ? 1 : 0)];
	    توڑو;
	}
    }
    اگر (t > tabcount)		// If the value of the index t is beyond the
				// end of the list, use the tab stop value at
				// the end of the list.
	tab_سائز = vts[tabcount];

    واپس tab_سائز;
}

/*
 * Find the column on which a tab starts.
 */
    colnr_T
tabstop_آغاز(colnr_T ستون, صحیح ts, صحیح *vts)
{
    صحیح		tabcount;
    colnr_T	tabcol = 0;
    صحیح		t;
    صحیح		excess;

    اگر (vts == تہی || vts[0] == 0)
	واپس ستون - ستون % ts;

    tabcount = vts[0];
    برائے (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	اگر (tabcol > ستون)
	    واپس tabcol - vts[t];
    }

    excess = tabcol % vts[tabcount];
    واپس ستون - (ستون - excess) % vts[tabcount];
}

/*
 * Find the number of tabs and spaces necessary to get from one column
 * to another.
 */
    خالی
tabstop_fromto(
	colnr_T آغاز_ستون,
	colnr_T انتہا_ستون,
	صحیح	ts_arg,
	صحیح	*vts,
	صحیح	*ntabs,
	صحیح	*nspcs)
{
    صحیح		spaces = انتہا_ستون - آغاز_ستون;
    colnr_T	tabcol = 0;
    صحیح		padding = 0;
    صحیح		tabcount;
    صحیح		t;
    صحیح		ts = ts_arg == 0 ? curbuf->b_p_ts : ts_arg;

    اگر (vts == تہی || vts[0] == 0)
    {
	صحیح tabs = 0;
	صحیح initspc = 0;

	initspc = ts - (آغاز_ستون % ts);
	اگر (spaces >= initspc)
	{
	    spaces -= initspc;
	    tabs++;
	}
	tabs += spaces / ts;
	spaces -= (spaces / ts) * ts;

	*ntabs = tabs;
	*nspcs = spaces;
	واپس;
    }

    // Find the padding needed to reach the next tabstop.
    tabcount = vts[0];
    برائے (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	اگر (tabcol > آغاز_ستون)
	{
	    padding = (صحیح)(tabcol - آغاز_ستون);
	    توڑو;
	}
    }
    اگر (t > tabcount)
	padding = vts[tabcount] - (صحیح)((آغاز_ستون - tabcol) % vts[tabcount]);

    // If the space needed is less than the padding no tabs can be used.
    اگر (spaces < padding)
    {
	*ntabs = 0;
	*nspcs = spaces;
	واپس;
    }

    *ntabs = 1;
    spaces -= padding;

    // At least one tab has been used. See if any more will fit.
    جب (spaces != 0 && ++t <= tabcount)
    {
	padding = vts[t];
	اگر (spaces < padding)
	{
	    *nspcs = spaces;
	    واپس;
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
    ساکن صحیح
tabstop_eq(صحیح *ts1, صحیح *ts2)
{
    صحیح		t;

    اگر ((ts1 == 0 && ts2) || (ts1 && ts2 == 0))
	واپس FALSE;
    اگر (ts1 == ts2)
	واپس TRUE;
    اگر (ts1[0] != ts2[0])
	واپس FALSE;

    برائے (t = 1; t <= ts1[0]; ++t)
	اگر (ts1[t] != ts2[t])
	    واپس FALSE;

    واپس TRUE;
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

    اگر (oldts == تہی)
	واپس تہی;
    newts = ALLOC_MULT(صحیح, oldts[0] + 1);
    اگر (newts != تہی)
	برائے (t = 0; t <= oldts[0]; ++t)
	    newts[t] = oldts[t];
    واپس newts;
}
# endif

/*
 * Return a count of the number of tabstops.
 */
    صحیح
tabstop_گنتی(صحیح *ts)
{
    واپس ts != تہی ? ts[0] : 0;
}

/*
 * Return the first tabstop, or 8 if there are no tabstops defined.
 */
    صحیح
tabstop_پہلا(صحیح *ts)
{
    واپس ts != تہی ? ts[1] : 8;
}

#اگر_ختم

/*
 * Return the effective shiftwidth value for current buffer, using the
 * 'tabstop' value when 'shiftwidth' is zero.
 */
    طویل
get_sw_قدر(buf_T *buf)
{
    واپس get_sw_قدر_ستون(buf, 0, FALSE);
}

/*
 * Idem, using "pos".
 */
    ساکن طویل
get_sw_قدر_pos(buf_T *buf, pos_T *pos, صحیح بایاں)
{
    pos_T save_cursor = curwin->w_cursor;
    طویل sw_قدر;

    curwin->w_cursor = *pos;
    sw_قدر = get_sw_قدر_ستون(buf, get_nolist_virtcol(), بایاں);
    curwin->w_cursor = save_cursor;
    واپس sw_قدر;
}

/*
 * Idem, using the first non-black in the current line.
 */
    طویل
get_sw_قدر_indent(buf_T *buf, صحیح بایاں)
{
    pos_T pos = curwin->w_cursor;

    pos.ستون = getwhitecols_curline();
    واپس get_sw_قدر_pos(buf, &pos, بایاں);
}

/*
 * Idem, using virtual column "col".
 */
    طویل
get_sw_قدر_ستون(buf_T *buf, colnr_T ستون غیر_مستعمل, صحیح بایاں غیر_مستعمل)
{
    واپس buf->b_p_sw ? buf->b_p_sw :
#اگر_تعریف_شدہ FEAT_VARTABS
	tabstop_at(ستون, buf->b_p_ts, buf->b_p_vts_array, بایاں);
#ورنہ
	buf->b_p_ts;
#اگر_ختم
}

/*
 * Return the effective softtabstop value for the current buffer, using the
 * 'shiftwidth' value when 'softtabstop' is negative.
 */
    طویل
get_sts_قدر(خالی)
{
    واپس curbuf->b_p_sts < 0 ? get_sw_قدر(curbuf) : curbuf->b_p_sts;
}

/*
 * Count the size (in window cells) of the indent in the current line.
 */
    صحیح
get_indent(خالی)
{
#اگر_تعریف_شدہ FEAT_VARTABS
    واپس get_indent_str_vtab(ml_get_curline(), (صحیح)curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#ورنہ
    واپس get_indent_str(ml_get_curline(), (صحیح)curbuf->b_p_ts, FALSE);
#اگر_ختم
}

/*
 * Count the size (in window cells) of the indent in line "lnum".
 */
    صحیح
get_indent_lnum(linenr_T lnum)
{
#اگر_تعریف_شدہ FEAT_VARTABS
    واپس get_indent_str_vtab(ml_get(lnum), (صحیح)curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#ورنہ
    واپس get_indent_str(ml_get(lnum), (صحیح)curbuf->b_p_ts, FALSE);
#اگر_ختم
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
    واپس get_indent_str_vtab(ml_get_buf(buf, lnum, FALSE),
			       (صحیح)buf->b_p_ts, buf->b_p_vts_array, FALSE);
# ورنہ
    واپس get_indent_str(ml_get_buf(buf, lnum, FALSE), (صحیح)buf->b_p_ts, FALSE);
# endif
}
#اگر_ختم

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
    صحیح		گنتی = 0;

    برائے ( ; *ptr; ++ptr)
    {
	اگر (*ptr == TAB)    // count a tab for what it is worth
	{
	    اگر (!no_ts)
		گنتی += ts - (گنتی % ts);
	    ورنہ
		گنتی += ptr2cells(ptr);
	}
	ورنہ اگر (*ptr == ' ')
	    ++گنتی;		// count a space for one
	ورنہ
	    توڑو;
    }
    واپس گنتی;
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
    صحیح		گنتی = 0;

    برائے ( ; *ptr; ++ptr)
    {
	اگر (*ptr == TAB)    // count a tab for what it is worth
	{
	    اگر (!no_ts)
		گنتی += tabstop_padding(گنتی, ts, vts);
	    ورنہ
		گنتی += ptr2cells(ptr);
	}
	ورنہ اگر (*ptr == ' ')
	    ++گنتی;		// count a space for one
	ورنہ
	    توڑو;
    }
    واپس گنتی;
}
#اگر_ختم

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
سیٹ_indent(
    صحیح		سائز,		    // measured in spaces
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
    صحیح		ind_مکمل = 0;	    // measured in spaces
#اگر_تعریف_شدہ FEAT_VARTABS
    صحیح		ind_ستون = 0;
#اگر_ختم
    صحیح		tab_pad;
    صحیح		retval = FALSE;
    صحیح		orig_char_len = -1; // number of initial whitespace chars when
				    // 'et' and 'pi' are both set

    // First check if there is anything to do and compute the number of
    // characters needed for the indent.
    todo = سائز;
    ind_len = 0;
    p = oldline = ml_get_curline();
    سطر_len = ml_get_curline_len() + 1;

    // Calculate the buffer size for the new indent, and check to see if it
    // isn't already set

    // if 'expandtab' isn't set: use TABs; if both 'expandtab' and
    // 'preserveindent' are set count the number of characters at the
    // beginning of the line to be copied
    اگر (!curbuf->b_p_et || (!(flags & SIN_داخل_کرو) && curbuf->b_p_pi))
    {
	// If 'preserveindent' is set then reuse as much as possible of
	// the existing indent structure for the new indent
	اگر (!(flags & SIN_داخل_کرو) && curbuf->b_p_pi)
	{
	    ind_مکمل = 0;

	    // count as many characters as we can use
	    جب (todo > 0 && VIM_ISWHITE(*p))
	    {
		اگر (*p == TAB)
		{
#اگر_تعریف_شدہ FEAT_VARTABS
		    tab_pad = tabstop_padding(ind_مکمل, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#ورنہ
		    tab_pad = (صحیح)curbuf->b_p_ts
					   - (ind_مکمل % (صحیح)curbuf->b_p_ts);
#اگر_ختم
		    // stop if this tab will overshoot the target
		    اگر (todo < tab_pad)
			توڑو;
		    todo -= tab_pad;
		    ++ind_len;
		    ind_مکمل += tab_pad;
		}
		ورنہ
		{
		    --todo;
		    ++ind_len;
		    ++ind_مکمل;
		}
		++p;
	    }

#اگر_تعریف_شدہ FEAT_VARTABS
	    // These diverge from this point.
	    ind_ستون = ind_مکمل;
#اگر_ختم
	    // Set initial number of whitespace chars to copy if we are
	    // preserving indent but expandtab is set
	    اگر (curbuf->b_p_et)
		orig_char_len = ind_len;

	    // Fill to next tabstop with a tab, if possible
#اگر_تعریف_شدہ FEAT_VARTABS
	    tab_pad = tabstop_padding(ind_مکمل, curbuf->b_p_ts,
						curbuf->b_p_vts_array);
#ورنہ
	    tab_pad = (صحیح)curbuf->b_p_ts - (ind_مکمل % (صحیح)curbuf->b_p_ts);
#اگر_ختم
	    اگر (todo >= tab_pad && orig_char_len == -1)
	    {
		doit = TRUE;
		todo -= tab_pad;
		++ind_len;
		// ind_done += tab_pad;
#اگر_تعریف_شدہ FEAT_VARTABS
		ind_ستون += tab_pad;
#اگر_ختم
	    }
	}

	// count tabs required for indent
#اگر_تعریف_شدہ FEAT_VARTABS
	برائے (;;)
	{
	    tab_pad = tabstop_padding(ind_ستون, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
	    اگر (todo < tab_pad)
		توڑو;
	    اگر (*p != TAB)
		doit = TRUE;
	    ورنہ
		++p;
	    todo -= tab_pad;
	    ++ind_len;
	    ind_ستون += tab_pad;
	}
#ورنہ
	جب (todo >= (صحیح)curbuf->b_p_ts)
	{
	    اگر (*p != TAB)
		doit = TRUE;
	    ورنہ
		++p;
	    todo -= (صحیح)curbuf->b_p_ts;
	    ++ind_len;
	    // ind_done += (int)curbuf->b_p_ts;
	}
#اگر_ختم
    }
    // count spaces required for indent
    جب (todo > 0)
    {
	اگر (*p != ' ')
	    doit = TRUE;
	ورنہ
	    ++p;
	--todo;
	++ind_len;
	// ++ind_done;
    }

    // Return if the indent is OK already.
    اگر (!doit && !VIM_ISWHITE(*p) && !(flags & SIN_داخل_کرو))
	واپس FALSE;

    // Allocate memory for the new line.
    اگر (flags & SIN_داخل_کرو)
	p = oldline;
    ورنہ
    {
	p = skipwhite(p);
	سطر_len -= (صحیح)(p - oldline);
    }

    // If 'preserveindent' and 'expandtab' are both set keep the original
    // characters and allocate accordingly.  We will fill the rest with spaces
    // after the if (!curbuf->b_p_et) below.
    اگر (orig_char_len != -1)
    {
	newline = alloc(orig_char_len + سائز - ind_مکمل + سطر_len);
	اگر (newline == تہی)
	    واپس FALSE;
	todo = سائز - ind_مکمل;
	ind_len = orig_char_len + todo;    // Set total length of indent in
					   // characters, which may have been
					   // undercounted until now
	p = oldline;
	s = newline;
	جب (orig_char_len > 0)
	{
	    *s++ = *p++;
	    orig_char_len--;
	}

	// Skip over any additional white space (useful when newindent is less
	// than old)
	جب (VIM_ISWHITE(*p))
	    ++p;

    }
    ورنہ
    {
	todo = سائز;
	newline = alloc(ind_len + سطر_len);
	اگر (newline == تہی)
	    واپس FALSE;
	s = newline;
    }

    // Put the characters in the new line.
    // if 'expandtab' isn't set: use TABs
    اگر (!curbuf->b_p_et)
    {
	// If 'preserveindent' is set then reuse as much as possible of
	// the existing indent structure for the new indent
	اگر (!(flags & SIN_داخل_کرو) && curbuf->b_p_pi)
	{
	    p = oldline;
	    ind_مکمل = 0;

	    جب (todo > 0 && VIM_ISWHITE(*p))
	    {
		اگر (*p == TAB)
		{
#اگر_تعریف_شدہ FEAT_VARTABS
		    tab_pad = tabstop_padding(ind_مکمل, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#ورنہ
		    tab_pad = (صحیح)curbuf->b_p_ts
					   - (ind_مکمل % (صحیح)curbuf->b_p_ts);
#اگر_ختم
		    // stop if this tab will overshoot the target
		    اگر (todo < tab_pad)
			توڑو;
		    todo -= tab_pad;
		    ind_مکمل += tab_pad;
		}
		ورنہ
		{
		    --todo;
		    ++ind_مکمل;
		}
		*s++ = *p++;
	    }

	    // Fill to next tabstop with a tab, if possible
#اگر_تعریف_شدہ FEAT_VARTABS
	    tab_pad = tabstop_padding(ind_مکمل, curbuf->b_p_ts,
						curbuf->b_p_vts_array);
#ورنہ
	    tab_pad = (صحیح)curbuf->b_p_ts - (ind_مکمل % (صحیح)curbuf->b_p_ts);
#اگر_ختم
	    اگر (todo >= tab_pad)
	    {
		*s++ = TAB;
		todo -= tab_pad;
#اگر_تعریف_شدہ FEAT_VARTABS
		ind_مکمل += tab_pad;
#اگر_ختم
	    }

	    p = skipwhite(p);
	}

#اگر_تعریف_شدہ FEAT_VARTABS
	برائے (;;)
	{
	    tab_pad = tabstop_padding(ind_مکمل, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
	    اگر (todo < tab_pad)
		توڑو;
	    *s++ = TAB;
	    todo -= tab_pad;
	    ind_مکمل += tab_pad;
	}
#ورنہ
	جب (todo >= (صحیح)curbuf->b_p_ts)
	{
	    *s++ = TAB;
	    todo -= (صحیح)curbuf->b_p_ts;
	}
#اگر_ختم
    }
    جب (todo > 0)
    {
	*s++ = ' ';
	--todo;
    }
    mch_memmove(s, p, (size_t)سطر_len);

    // Replace the line (unless undo fails).
    اگر (!(flags & SIN_UNDO) || u_savesub(curwin->w_cursor.lnum) == OK)
    {
	colnr_T old_انحراف = (colnr_T)(p - oldline);
	colnr_T new_انحراف = (colnr_T)(s - newline);

	// this may free "newline"
	ml_replace(curwin->w_cursor.lnum, newline, FALSE);
	اگر (flags & SIN_بدلا)
	    بدلا_بائٹیں(curwin->w_cursor.lnum, 0);

	// Correct saved cursor position if it is in this line.
	اگر (saved_cursor.lnum == curwin->w_cursor.lnum)
	{
	    اگر (saved_cursor.ستون >= old_انحراف)
		// cursor was after the indent, adjust for the number of
		// bytes added/removed
		saved_cursor.ستون += ind_len - old_انحراف;
	    ورنہ اگر (saved_cursor.ستون >= new_انحراف)
		// cursor was in the indent, and is now after it, put it back
		// at the start of the indent (replacing spaces with TAB)
		saved_cursor.ستون = new_انحراف;
	}
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	اگر (curbuf->b_has_textprop)
	{
	    صحیح added = ind_len - old_انحراف;

	    // When increasing indent this behaves like spaces were inserted at
	    // the old indent, when decreasing indent it behaves like spaces
	    // were deleted at the new indent.
	    adjust_prop_columns(curwin->w_cursor.lnum,
				  added > 0 ? old_انحراف : (colnr_T)ind_len,
				  added, APC_INDENT);
	}
#اگر_ختم
	retval = TRUE;
    }
    ورنہ
	vim_free(newline);

    curwin->w_cursor.ستون = ind_len;
    واپس retval;
}

/*
 * Return the indent of the current line after a number.  Return -1 if no
 * number was found.  Used for 'n' in 'formatoptions': numbered list.
 * Since a pattern is used it can actually handle more than numbers.
 */
    صحیح
get_نمبر_indent(linenr_T lnum)
{
    colnr_T	ستون;
    pos_T	pos;

    regmatch_T	regmatch;
    صحیح		lead_len = 0;	// length of comment leader

    اگر (lnum > curbuf->b_ml.ml_سطر_گنتی)
	واپس -1;
    pos.lnum = 0;

    // In format_lines() (i.e. not insert mode), fo+=q is needed too...
    اگر ((State & منوال_داخل_کرو) || has_format_option(FO_Q_COMS))
	lead_len = get_leader_len(ml_get(lnum), تہی, FALSE, TRUE);

    regmatch.regprog = vim_regcomp(curbuf->b_p_flp, RE_MAGIC);
    اگر (regmatch.regprog != تہی)
    {
	regmatch.rm_ic = FALSE;

	// vim_regexec() expects a pointer to a line.  This lets us
	// start matching for the flp beyond any comment leader...
	اگر (vim_regexec(&regmatch, ml_get(lnum) + lead_len, (colnr_T)0))
	{
	    pos.lnum = lnum;
	    pos.ستون = (colnr_T)(*regmatch.endp - ml_get(lnum));
	    pos.coladd = 0;
	}
	vim_regfree(regmatch.regprog);
    }

    اگر (pos.lnum == 0 || *ml_get_pos(&pos) == NUL)
	واپس -1;
    getvcol(curwin, &pos, &ستون, تہی, تہی, 0);
    واپس (صحیح)ستون;
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
    صحیح		bri_سرکاؤ = 0;
    طویل	bri_min = 20;
    صحیح		bri_sbr = FALSE;
    صحیح		bri_list = 0;
    صحیح		bri_vcol = 0;

    اگر (briopt != تہی)
	p = briopt;
    ورنہ
	p = wp->w_p_briopt;

    جب (*p != NUL)
    {
	// Note: Keep this in sync with p_briopt_values
	اگر (STRNCMP(p, "shift:", 6) == 0
		 && ((p[6] == '-' && VIM_ISDIGIT(p[7])) || VIM_ISDIGIT(p[6])))
	{
	    p += 6;
	    bri_سرکاؤ = getdigits(&p);
	}
	ورنہ اگر (STRNCMP(p, "min:", 4) == 0 && VIM_ISDIGIT(p[4]))
	{
	    p += 4;
	    bri_min = getdigits(&p);
	}
	ورنہ اگر (STRNCMP(p, "sbr", 3) == 0)
	{
	    p += 3;
	    bri_sbr = TRUE;
	}
	ورنہ اگر (STRNCMP(p, "list:", 5) == 0)
	{
	    p += 5;
	    bri_list = getdigits(&p);
	}
	ورنہ اگر (STRNCMP(p, "column:", 7) == 0)
	{
	    p += 7;
	    bri_vcol = getdigits(&p);
	}
	اگر (*p != ',' && *p != NUL)
	    واپس FAIL;
	اگر (*p == ',')
	    ++p;
    }

    اگر (wp == تہی)
	واپس OK;

    wp->w_briopt_سرکاؤ = bri_سرکاؤ;
    wp->w_briopt_min   = bri_min;
    wp->w_briopt_sbr   = bri_sbr;
    wp->w_briopt_list  = bri_list;
    wp->w_briopt_vcol  = bri_vcol;

    واپس OK;
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
    ساکن صحیح	    پچھلا_indent = 0;	// cached indent value
    ساکن طویل	    پچھلا_ts     = 0L;	// cached tabstop value
# ifdef FEAT_VARTABS
    ساکن صحیح      *پچھلا_vts = تہی;   // cached vartabs values
# endif
    ساکن صحیح	    پچھلا_fnum   = 0;	// cached buffer number
    ساکن char_u   *پچھلا_سطر  = تہی;	// cached copy of "line"
    ساکن varnumber_T پچھلا_tick = 0;   // changedtick of cached value
    ساکن صحیح      پچھلا_list = 0;	// cached list indent
    ساکن صحیح      پچھلا_listopt = 0;	// cached w_p_briopt_list value
    ساکن صحیح      پچھلا_no_ts = FALSE;	// cached no_ts value
    ساکن غیرعلامتی پچھلا_dy_uhex = 0;	// cached 'display' "uhex" value
    // cached formatlistpat value
    ساکن char_u   *پچھلا_flp = تہی;
    صحیح		    bri = 0;
    // window width minus window margin space, i.e. what rests for text
    ثابت صحیح	    eff_wwidth = wp->w_چوڑائی
					  - win_ستون_off(wp) + win_ستون_off2(wp);

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
    اگر (پچھلا_fnum != wp->w_بفر->b_fnum
	    || پچھلا_ts != wp->w_بفر->b_p_ts
# ifdef FEAT_VARTABS
	    || پچھلا_vts != wp->w_بفر->b_p_vts_array
# endif
	    || پچھلا_tick != CHANGEDTICK(wp->w_بفر)
	    || پچھلا_listopt != wp->w_briopt_list
	    || پچھلا_no_ts != no_ts
	    || پچھلا_dy_uhex != (dy_flags & DY_UHEX)
	    || پچھلا_flp == تہی
	    || STRCMP(پچھلا_flp, get_flp_قدر(wp->w_بفر)) != 0
	    || پچھلا_سطر == تہی || STRCMP(پچھلا_سطر, سطر) != 0
	)
    {
	پچھلا_fnum = wp->w_بفر->b_fnum;
	vim_free(پچھلا_سطر);
	پچھلا_سطر = vim_strsave(سطر);
	پچھلا_ts = wp->w_بفر->b_p_ts;
# ifdef FEAT_VARTABS
	پچھلا_vts = wp->w_بفر->b_p_vts_array;
	اگر (wp->w_briopt_vcol == 0)
	    پچھلا_indent = get_indent_str_vtab(سطر,
				     (صحیح)wp->w_بفر->b_p_ts,
				      wp->w_بفر->b_p_vts_array, no_ts);
# ورنہ
	اگر (wp->w_briopt_vcol == 0)
	    پچھلا_indent = get_indent_str(سطر,
					(صحیح)wp->w_بفر->b_p_ts, no_ts);
# endif
	پچھلا_tick = CHANGEDTICK(wp->w_بفر);
	پچھلا_listopt = wp->w_briopt_list;
	پچھلا_list = 0;
	پچھلا_no_ts = no_ts;
	پچھلا_dy_uhex = (dy_flags & DY_UHEX);
	vim_free(پچھلا_flp);
	پچھلا_flp = vim_strsave(get_flp_قدر(wp->w_بفر));
	// add additional indent for numbered lists
	اگر (wp->w_briopt_list != 0 && wp->w_briopt_vcol == 0)
	{
	    regmatch_T	    regmatch;

	    regmatch.regprog = vim_regcomp(پچھلا_flp,
				   RE_MAGIC + RE_STRING + RE_AUTO + RE_STRICT);

	    اگر (regmatch.regprog != تہی)
	    {
		regmatch.rm_ic = FALSE;
		اگر (vim_regexec(&regmatch, سطر, 0))
		{
		    اگر (wp->w_briopt_list > 0)
			پچھلا_list = wp->w_briopt_list;
		    ورنہ
		    {
			char_u	*ptr = *regmatch.startp;
			char_u	*end_ptr = *regmatch.endp;
			صحیح	indent = 0;

			// Compute the width of the matched text.
			// Use win_chartabsize() so that TAB size is correct,
			// while wrapping is ignored.
			جب (ptr < end_ptr)
			{
			    indent += win_chartabsize(wp, ptr, indent);
			    MB_PTR_ADV(ptr);
			}
			پچھلا_indent = indent;
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
	پچھلا_list = 0;
    }
    ورنہ
	bri = پچھلا_indent + wp->w_briopt_سرکاؤ;

    // Add offset for number column, if 'n' is in 'cpoptions'
    bri += win_ستون_off2(wp);

    // add additional indent for numbered lists
    اگر (wp->w_briopt_list > 0)
	bri += پچھلا_list;

    // indent minus the length of the showbreak string
    اگر (wp->w_briopt_sbr)
	bri -= vim_strsize(get_showbreak_قدر(wp));


    // never indent past left window margin
    اگر (bri < 0)
	bri = 0;

    // always leave at least bri_min characters on the left,
    // if text width is sufficient
    ورنہ اگر (bri > eff_wwidth - wp->w_briopt_min)
	bri = (eff_wwidth - wp->w_briopt_min < 0)
					   ? 0 : eff_wwidth - wp->w_briopt_min;

    واپس bri;
}
#اگر_ختم

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
    colnr_T	ستون;

    برائے (ستون = 0, ptr = ml_get_curline(); VIM_ISWHITE(*ptr); ++ستون)
	++ptr;
    اگر (ستون >= curwin->w_cursor.ستون + extra)
	واپس TRUE;
    ورنہ
	واپس FALSE;
}

/*
 * op_reindent - handle reindenting a block of lines.
 */
    خالی
op_reindent(oparg_T *oap, صحیح (*how)(خالی))
{
    طویل	i = 0;
    char_u	*l;
    صحیح		amount;
    linenr_T	پہلا_بدلا = 0;
    linenr_T	آخری_بدلا = 0;
    linenr_T	آغاز_lnum = curwin->w_cursor.lnum;

    // Don't even try when 'modifiable' is off.
    اگر (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	واپس;
    }

    // Save for undo.  Do this once for all lines, much faster than doing this
    // for each line separately, especially when undoing.
    اگر (u_savecommon(آغاز_lnum - 1, آغاز_lnum + oap->سطر_گنتی,
				     آغاز_lnum + oap->سطر_گنتی, FALSE) == OK)
	برائے (i = oap->سطر_گنتی; --i >= 0 && !got_int; )
	{
	    // it's a slow thing to do, so give feedback so there's no worry
	    // that the computer's just hung.

	    اگر (i > 1
		    && (i % 50 == 0 || i == oap->سطر_گنتی - 1)
		    && oap->سطر_گنتی > p_report)
		smsg(_("%ld lines to indent... "), i);

	    // Be vi-compatible: For lisp indenting the first line is not
	    // indented, unless there is only one line.
	    اگر (i != oap->سطر_گنتی - 1 || oap->سطر_گنتی == 1
						     || how != get_lisp_indent)
	    {
		l = skipwhite(ml_get_curline());
		اگر (*l == NUL)		    // empty or blank line
		    amount = 0;
		ورنہ
		    amount = how();	    // get the indent for this line

		اگر (amount >= 0 && سیٹ_indent(amount, 0))
		{
		    // did change the indent, call changed_lines() later
		    اگر (پہلا_بدلا == 0)
			پہلا_بدلا = curwin->w_cursor.lnum;
		    آخری_بدلا = curwin->w_cursor.lnum;
		}
	    }
	    ++curwin->w_cursor.lnum;
	    curwin->w_cursor.ستون = 0;  // make sure it's valid
	}

    // put cursor on first non-blank of indented line
    curwin->w_cursor.lnum = آغاز_lnum;
    beginline(BL_SOL | BL_FIX);

    // Mark changed lines so that they will be redrawn.  When Visual
    // highlighting was present, need to continue until the last line.  When
    // there is no change still need to remove the Visual highlighting.
    اگر (آخری_بدلا != 0)
	بدلا_سطریں(پہلا_بدلا, 0,
		oap->is_VIsual ? آغاز_lnum + oap->سطر_گنتی :
		آخری_بدلا + 1, 0L);
    ورنہ اگر (oap->is_VIsual)
	redraw_curbuf_later(UPD_INVERTED);

    اگر (oap->سطر_گنتی > p_report)
    {
	i = oap->سطر_گنتی - (i + 1);
	smsg(NGETTEXT("%ld line indented ",
						 "%ld lines indented ", i), i);
    }
    اگر ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// set '[ and '] marks
	curbuf->b_op_آغاز = oap->آغاز;
	curbuf->b_op_انتہا = oap->انتہا;
    }
}

/*
 * Return TRUE if lines starting with '#' should be left aligned.
 */
    صحیح
preprocs_بایاں(خالی)
{
    واپس
	(curbuf->b_p_si && !curbuf->b_p_cin) ||
	(curbuf->b_p_cin && اندر_cinkeys('#', ' ', TRUE)
					   && curbuf->b_ind_hash_comment == 0)
	;
}

/*
 * Return TRUE if the conditions are OK for smart indenting.
 */
    صحیح
may_do_si(خالی)
{
    واپس curbuf->b_p_si
	&& !curbuf->b_p_cin
#اگر_تعریف_شدہ FEAT_EVAL
	&& *curbuf->b_p_inde == NUL
#اگر_ختم
	&& !p_paste;
}

/*
 * Try to do some very smart auto-indenting.
 * Used when inserting a "normal" character.
 */
    خالی
ins_try_si(صحیح c)
{
    pos_T	*pos, old_pos;
    char_u	*ptr;
    صحیح		i;
    صحیح		عارضی;

    // do some very smart indenting when entering '{' or '}'
    اگر (((did_si || can_si_عقب) && c == '{')
	    || (can_si && c == '}' && inindent(0)))
    {
	// for '}' set indent equal to indent of line containing matching '{'
	اگر (c == '}' && (pos = findmatch(تہی, '{')) != تہی)
	{
	    old_pos = curwin->w_cursor;
	    // If the matching '{' has a ')' immediately before it (ignoring
	    // white-space), then line up with the start of the line
	    // containing the matching '(' if there is one.  This handles the
	    // case where an "if (..\n..) {" statement continues over multiple
	    // lines -- webb
	    ptr = ml_get(pos->lnum);
	    i = pos->ستون;
	    اگر (i > 0)		// skip blanks before '{'
		جب (--i > 0 && VIM_ISWHITE(ptr[i]))
		    ;
	    curwin->w_cursor.lnum = pos->lnum;
	    curwin->w_cursor.ستون = i;
	    اگر (ptr[i] == ')' && (pos = findmatch(تہی, '(')) != تہی)
		curwin->w_cursor = *pos;
	    i = get_indent();
	    curwin->w_cursor = old_pos;
	    اگر (State & VREPLACE_FLAG)
		change_indent(INDENT_سیٹ, i, FALSE, NUL, TRUE);
	    ورنہ
		(خالی)سیٹ_indent(i, SIN_بدلا);
	}
	ورنہ اگر (curwin->w_cursor.ستون > 0)
	{
	    // when inserting '{' after "O" reduce indent, but not
	    // more than indent of previous line
	    عارضی = TRUE;
	    اگر (c == '{' && can_si_عقب && curwin->w_cursor.lnum > 1)
	    {
		old_pos = curwin->w_cursor;
		i = get_indent();
		جب (curwin->w_cursor.lnum > 1)
		{
		    ptr = skipwhite(ml_get(--(curwin->w_cursor.lnum)));

		    // ignore empty lines and lines starting with '#'.
		    اگر (*ptr != '#' && *ptr != NUL)
			توڑو;
		}
		اگر (get_indent() >= i)
		    عارضی = FALSE;
		curwin->w_cursor = old_pos;
	    }
	    اگر (عارضی)
		سرکاؤ_سطر(TRUE, FALSE, 1, TRUE);
	}
    }

    // set indent of '#' always to 0
    اگر (curwin->w_cursor.ستون > 0 && can_si && c == '#' && inindent(0))
    {
	// remember current indent for next line
	old_indent = get_indent();
	(خالی)سیٹ_indent(0, SIN_بدلا);
    }

    // Adjust ai_col, the char at this position can be deleted.
    اگر (ai_ستون > curwin->w_cursor.ستون)
	ai_ستون = curwin->w_cursor.ستون;
}

/*
 * Insert an indent (for <Tab> or CTRL-T) or delete an indent (for CTRL-D).
 * Keep the cursor on the same character.
 * type == INDENT_INC	increase indent (for CTRL-T or <Tab>)
 * type == INDENT_DEC	decrease indent (for CTRL-D)
 * type == INDENT_SET	set indent to "amount"
 * if round is TRUE, round the indent to 'shiftwidth' (only with _INC and _Dec).
 */
    خالی
change_indent(
    صحیح		type,
    صحیح		amount,
    صحیح		باری,
    صحیح		replaced,	// replaced character, put on replace stack
    صحیح		call_بدلا_بائٹیں)	// call changed_bytes()
{
    صحیح		vcol;
    صحیح		آخری_vcol;
    صحیح		insstart_less;		// reduction for Insstart.col
    صحیح		new_cursor_ستون;
    صحیح		i;
    char_u	*ptr;
    صحیح		save_p_list;
    صحیح		آغاز_ستون;
    colnr_T	vc;
    colnr_T	orig_ستون = 0;		// init for GCC
    char_u	*new_سطر, *orig_سطر = تہی;	// init for GCC

    // MODE_VREPLACE state needs to know what the line was like before changing
    اگر (State & VREPLACE_FLAG)
    {
	orig_سطر = vim_strnsave(ml_get_curline(), ml_get_curline_len());  // Deal with NULL below
	orig_ستون = curwin->w_cursor.ستون;
    }

    // for the following tricks we don't want list mode
    save_p_list = curwin->w_p_list;
    curwin->w_p_list = FALSE;
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    ignore_متن_props = TRUE;
#اگر_ختم
    vc = getvcol_nolist(&curwin->w_cursor);
    vcol = vc;

    // For Replace mode we need to fix the replace stack later, which is only
    // possible when the cursor is in the indent.  Remember the number of
    // characters before the cursor if it's possible.
    آغاز_ستون = curwin->w_cursor.ستون;

    // determine offset from first non-blank
    new_cursor_ستون = curwin->w_cursor.ستون;
    beginline(BL_WHITE);
    new_cursor_ستون -= curwin->w_cursor.ستون;

    insstart_less = curwin->w_cursor.ستون;

    // If the cursor is in the indent, compute how many screen columns the
    // cursor is to the left of the first non-blank.
    اگر (new_cursor_ستون < 0)
	vcol = get_indent() - vcol;

    اگر (new_cursor_ستون > 0)	    // can't fix replace stack
	آغاز_ستون = -1;

    // Set the new indent.  The cursor will be put on the first non-blank.
    اگر (type == INDENT_سیٹ)
	(خالی)سیٹ_indent(amount, call_بدلا_بائٹیں ? SIN_بدلا : 0);
    ورنہ
    {
	صحیح	save_State = State;

	// Avoid being called recursively.
	اگر (State & VREPLACE_FLAG)
	    State = منوال_داخل_کرو;
	سرکاؤ_سطر(type == INDENT_DEC, باری, 1, call_بدلا_بائٹیں);
	State = save_State;
    }
    insstart_less -= curwin->w_cursor.ستون;

    // Try to put cursor on same character.
    // If the cursor is at or after the first non-blank in the line,
    // compute the cursor column relative to the column of the first
    // non-blank character.
    // If we are not in insert mode, leave the cursor on the first non-blank.
    // If the cursor is before the first non-blank, position it relative
    // to the first non-blank, counted in screen columns.
    اگر (new_cursor_ستون >= 0)
    {
	// When changing the indent while the cursor is touching it, reset
	// Insstart_col to 0.
	اگر (new_cursor_ستون == 0)
	    insstart_less = MAXCOL;
	new_cursor_ستون += curwin->w_cursor.ستون;
    }
    ورنہ اگر (!(State & منوال_داخل_کرو))
	new_cursor_ستون = curwin->w_cursor.ستون;
    ورنہ
    {
	chartabsize_T cts;

	// Compute the screen column where the cursor should be.
	vcol = get_indent() - vcol;
	curwin->w_virtcol = (colnr_T)((vcol < 0) ? 0 : vcol);

	// Advance the cursor until we reach the right screen column.
	آخری_vcol = 0;
	ptr = ml_get_curline();
	init_chartabsize_arg(&cts, curwin, 0, 0, ptr, ptr);
	جب (cts.cts_vcol <= (صحیح)curwin->w_virtcol)
	{
	    آخری_vcol = cts.cts_vcol;
	    اگر (cts.cts_vcol > 0)
		MB_PTR_ADV(cts.cts_ptr);
	    اگر (*cts.cts_ptr == NUL)
		توڑو;
	    cts.cts_vcol += lbr_chartabsize(&cts);
	}
	vcol = آخری_vcol;
	new_cursor_ستون = cts.cts_ptr - cts.cts_سطر;
	clear_chartabsize_arg(&cts);

	// May need to insert spaces to be able to position the cursor on
	// the right screen column.
	اگر (vcol != (صحیح)curwin->w_virtcol)
	{
	    curwin->w_cursor.ستون = (colnr_T)new_cursor_ستون;
	    i = (صحیح)curwin->w_virtcol - vcol;
	    ptr = alloc(i + 1);
	    اگر (ptr != تہی)
	    {
		size_t ptrlen;
		new_cursor_ستون += i;
		ptr[i] = NUL;
		ptrlen = i;
		جب (--i >= 0)
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

    اگر (new_cursor_ستون <= 0)
	curwin->w_cursor.ستون = 0;
    ورنہ
	curwin->w_cursor.ستون = (colnr_T)new_cursor_ستون;
    curwin->w_سیٹ_curswant = true;
    بدلا_cline_bef_curs();

    // May have to adjust the start of the insert.
    اگر (State & منوال_داخل_کرو)
    {
	اگر (curwin->w_cursor.lnum == Insstart.lnum && Insstart.ستون != 0)
	{
	    اگر ((صحیح)Insstart.ستون <= insstart_less)
		Insstart.ستون = 0;
	    ورنہ
		Insstart.ستون -= insstart_less;
	}
	اگر ((صحیح)ai_ستون <= insstart_less)
	    ai_ستون = 0;
	ورنہ
	    ai_ستون -= insstart_less;
    }

    // For MODE_REPLACE state, may have to fix the replace stack, if it's
    // possible.  If the number of characters before the cursor decreased, need
    // to pop a few characters from the replace stack.
    // If the number of characters before the cursor increased, need to push a
    // few NULs onto the replace stack.
    اگر (REPLACE_معمول(State) && آغاز_ستون >= 0)
    {
	جب (آغاز_ستون > (صحیح)curwin->w_cursor.ستون)
	{
	    replace_join(0);	    // remove a NUL from the replace stack
	    --آغاز_ستون;
	}
	جب (آغاز_ستون < (صحیح)curwin->w_cursor.ستون || replaced)
	{
	    replace_ڈالو(NUL);
	    اگر (replaced)
	    {
		replace_ڈالو(replaced);
		replaced = NUL;
	    }
	    ++آغاز_ستون;
	}
    }
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    ignore_متن_props = FALSE;
#اگر_ختم

    // For MODE_VREPLACE state, we also have to fix the replace stack.  In this
    // case it is always possible because we backspace over the whole line and
    // then put it back again the way we wanted it.
    اگر (State & VREPLACE_FLAG)
    {
	// If orig_line didn't allocate, just return.  At least we did the job,
	// even if you can't backspace.
	اگر (orig_سطر == تہی)
	    واپس;

	// Save new line
	new_سطر = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	اگر (new_سطر == تہی)
	{
	    vim_free(orig_سطر);
	    واپس;
	}

	// We only put back the new line up to the cursor
	new_سطر[curwin->w_cursor.ستون] = NUL;

	// Put back original line
	ml_replace(curwin->w_cursor.lnum, orig_سطر, FALSE);
	curwin->w_cursor.ستون = orig_ستون;

	// Backspace from cursor to start of line
	backspace_until_عمود(0);

	// Insert new stuff into line again
	ins_بائٹیں(new_سطر);

	vim_free(new_سطر);
    }
}

/*
 * Copy the indent from ptr to the current line (and fill to size)
 * Leaves the cursor on the first non-blank in the line.
 * Returns TRUE if the line was changed.
 */
    صحیح
copy_indent(صحیح سائز, char_u *src)
{
    char_u	*p = تہی;
    char_u	*سطر = تہی;
    char_u	*s;
    صحیح		todo;
    صحیح		ind_len;
    صحیح		سطر_len = 0;
    صحیح		tab_pad;
    صحیح		ind_مکمل;
    صحیح		باری;
#اگر_تعریف_شدہ FEAT_VARTABS
    صحیح		ind_ستون;
#اگر_ختم

    // Round 1: compute the number of characters needed for the indent
    // Round 2: copy the characters.
    برائے (باری = 1; باری <= 2; ++باری)
    {
	todo = سائز;
	ind_len = 0;
	ind_مکمل = 0;
#اگر_تعریف_شدہ FEAT_VARTABS
	ind_ستون = 0;
#اگر_ختم
	s = src;

	// Count/copy the usable portion of the source line
	جب (todo > 0 && VIM_ISWHITE(*s))
	{
	    اگر (*s == TAB)
	    {
#اگر_تعریف_شدہ FEAT_VARTABS
		tab_pad = tabstop_padding(ind_مکمل, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#ورنہ
		tab_pad = (صحیح)curbuf->b_p_ts
					   - (ind_مکمل % (صحیح)curbuf->b_p_ts);
#اگر_ختم
		// Stop if this tab will overshoot the target
		اگر (todo < tab_pad)
		    توڑو;
		todo -= tab_pad;
		ind_مکمل += tab_pad;
#اگر_تعریف_شدہ FEAT_VARTABS
		ind_ستون += tab_pad;
#اگر_ختم
	    }
	    ورنہ
	    {
		--todo;
		++ind_مکمل;
#اگر_تعریف_شدہ FEAT_VARTABS
		++ind_ستون;
#اگر_ختم
	    }
	    ++ind_len;
	    اگر (p != تہی)
		*p++ = *s;
	    ++s;
	}

	// Fill to next tabstop with a tab, if possible
#اگر_تعریف_شدہ FEAT_VARTABS
	tab_pad = tabstop_padding(ind_مکمل, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#ورنہ
	tab_pad = (صحیح)curbuf->b_p_ts - (ind_مکمل % (صحیح)curbuf->b_p_ts);
#اگر_ختم
	اگر (todo >= tab_pad && !curbuf->b_p_et)
	{
	    todo -= tab_pad;
	    ++ind_len;
#اگر_تعریف_شدہ FEAT_VARTABS
	    ind_ستون += tab_pad;
#اگر_ختم
	    اگر (p != تہی)
		*p++ = TAB;
	}

	// Add tabs required for indent
	اگر (!curbuf->b_p_et)
	{
#اگر_تعریف_شدہ FEAT_VARTABS
	    برائے (;;)
	    {
		tab_pad = tabstop_padding(ind_ستون, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
		اگر (todo < tab_pad)
		    توڑو;
		todo -= tab_pad;
		++ind_len;
		ind_ستون += tab_pad;
		اگر (p != تہی)
		    *p++ = TAB;
	    }
#ورنہ
	    جب (todo >= (صحیح)curbuf->b_p_ts)
	    {
		todo -= (صحیح)curbuf->b_p_ts;
		++ind_len;
		اگر (p != تہی)
		    *p++ = TAB;
	    }
#اگر_ختم
	}

	// Count/add spaces required for indent
	جب (todo > 0)
	{
	    --todo;
	    ++ind_len;
	    اگر (p != تہی)
		*p++ = ' ';
	}

	اگر (p == تہی)
	{
	    // Allocate memory for the result: the copied indent, new indent
	    // and the rest of the line.
	    سطر_len = ml_get_curline_len() + 1;
	    سطر = alloc(ind_len + سطر_len);
	    اگر (سطر == تہی)
		واپس FALSE;
	    p = سطر;
	}
    }

    // Append the original line
    mch_memmove(p, ml_get_curline(), (size_t)سطر_len);

    // Replace the line
    ml_replace(curwin->w_cursor.lnum, سطر, FALSE);

    // Put the cursor after the indent.
    curwin->w_cursor.ستون = ind_len;
    واپس TRUE;
}

/*
 * Give a "resulting text too long" error and maybe set got_int.
 */
    ساکن خالی
emsg_متن_too_long(خالی)
{
    emsg(_(e_resulting_متن_too_long));
#اگر_تعریف_شدہ FEAT_EVAL
    // when not inside a try/catch set got_int to break out of any loop
    اگر (trylevel == 0)
#اگر_ختم
	got_int = TRUE;
}

/*
 * ":retab".
 */
    خالی
ex_retab(exarg_T *eap)
{
    linenr_T	lnum;
    صحیح		got_tab = FALSE;
    طویل	عدد_spaces = 0;
    طویل	عدد_tabs;
    طویل	len;
    طویل	ستون;
    طویل	vcol;
    طویل	آغاز_ستون = 0;		// For start of white-space string
    طویل	آغاز_vcol = 0;		// For start of white-space string
    طویل	old_len;
    طویل	new_len;
    char_u	*ptr;
    char_u	*new_سطر = (char_u *)1; // init to non-NULL
    صحیح		did_undo;		// called u_save for current line
#اگر_تعریف_شدہ FEAT_VARTABS
    صحیح		*new_vts_array = تہی;
    char_u	*new_ts_str;		// string value of tab argument
#ورنہ
    صحیح		عارضی;
    صحیح		new_ts = 0;
#اگر_ختم
    صحیح		save_list;
    linenr_T	پہلا_سطر = 0;		// first changed line
    linenr_T	آخری_سطر = 0;		// last changed line
    صحیح		is_indent_only = 0;	// Only process leading whitespace

    save_list = curwin->w_p_list;
    curwin->w_p_list = 0;	    // don't want list mode here

    ptr = eap->arg;
    اگر (STRNCMP(ptr, "-indentonly", 11) == 0 && IS_WHITE_OR_NUL(ptr[11]))
    {
	is_indent_only = 1;
	ptr = skipwhite(ptr + 11);
    }

#اگر_تعریف_شدہ FEAT_VARTABS
    new_ts_str = ptr;
    اگر (tabstop_سیٹ(ptr, &new_vts_array) == FAIL)
	واپس;
    جب (vim_isdigit(*ptr) || *ptr == ',')
	++ptr;

    // This ensures that either new_vts_array and new_ts_str are freshly
    // allocated, or new_vts_array points to an existing array and new_ts_str
    // is null.
    اگر (new_vts_array == تہی)
    {
	new_vts_array = curbuf->b_p_vts_array;
	new_ts_str = تہی;
    }
    ورنہ
	new_ts_str = vim_strnsave(new_ts_str, ptr - new_ts_str);
#ورنہ
    اگر (ptr[0] != NUL && (ptr[0] != '0' || ptr[1] != NUL))
    {
	char_u	*انتہا;

	اگر (strtol((حرف *)ptr, (حرف **)&انتہا, 10) <= 0)
	{
	    اگر (ptr != انتہا)
		emsg(_(e_دلیل_must_be_مثبت));
	    ورنہ
		semsg(_(e_invalid_دلیل_str), ptr);
	    واپس;
	}
	new_ts = getdigits(&ptr);
	اگر (new_ts < 0 || new_ts > TABSTOP_MAX)
	{
	    semsg(_(e_invalid_دلیل_str), eap->arg);
	    واپس;
	}
    }
    اگر (new_ts == 0)
	new_ts = curbuf->b_p_ts;
#اگر_ختم
    برائے (lnum = eap->line1; !got_int && lnum <= eap->line2; ++lnum)
    {
	ptr = ml_get(lnum);
	old_len = ml_get_len(lnum);
	ستون = 0;
	vcol = 0;
	did_undo = FALSE;
	برائے (;;)
	{
	    اگر (VIM_ISWHITE(ptr[ستون]))
	    {
		اگر (!got_tab && عدد_spaces == 0)
		{
		    // First consecutive white-space
		    آغاز_vcol = vcol;
		    آغاز_ستون = ستون;
		}
		اگر (ptr[ستون] == ' ')
		    عدد_spaces++;
		ورنہ
		    got_tab = TRUE;
	    }
	    ورنہ
	    {
		اگر (got_tab || (eap->forceit && عدد_spaces > 1))
		{
		    // Retabulate this string of white-space

		    // len is virtual length of white string
		    len = عدد_spaces = vcol - آغاز_vcol;
		    عدد_tabs = 0;
		    اگر (!curbuf->b_p_et)
		    {
#اگر_تعریف_شدہ FEAT_VARTABS
			صحیح t, s;

			tabstop_fromto(آغاز_vcol, vcol,
					curbuf->b_p_ts, new_vts_array, &t, &s);
			عدد_tabs = t;
			عدد_spaces = s;
#ورنہ
			عارضی = new_ts - (آغاز_vcol % new_ts);
			اگر (عدد_spaces >= عارضی)
			{
			    عدد_spaces -= عارضی;
			    عدد_tabs++;
			}
			عدد_tabs += عدد_spaces / new_ts;
			عدد_spaces -= (عدد_spaces / new_ts) * new_ts;
#اگر_ختم
		    }
		    اگر (curbuf->b_p_et || got_tab ||
					(عدد_spaces + عدد_tabs < len))
		    {
			اگر (did_undo == FALSE)
			{
			    did_undo = TRUE;
			    اگر (u_save((linenr_T)(lnum - 1),
						(linenr_T)(lnum + 1)) == FAIL)
			    {
				new_سطر = تہی;	// flag out-of-memory
				توڑو;
			    }
			}

			// len is actual number of white characters used
			len = عدد_spaces + عدد_tabs;
			new_len = old_len - ستون + آغاز_ستون + len + 1;
			اگر (new_len <= 0 || new_len >= MAXCOL)
			{
			    emsg_متن_too_long();
			    توڑو;
			}
			new_سطر = alloc(new_len);
			اگر (new_سطر == تہی)
			    توڑو;
			اگر (آغاز_ستون > 0)
			    mch_memmove(new_سطر, ptr, (size_t)آغاز_ستون);
			mch_memmove(new_سطر + آغاز_ستون + len,
				      ptr + ستون, (size_t)(old_len - ستون + 1));
			ptr = new_سطر + آغاز_ستون;
			برائے (ستون = 0; ستون < len; ستون++)
			    ptr[ستون] = (ستون < عدد_tabs) ? '\t' : ' ';
			اگر (ml_replace(lnum, new_سطر, FALSE) == OK)
			    // "new_line" may have been copied
			    new_سطر = curbuf->b_ml.ml_سطر_ptr;
			اگر (پہلا_سطر == 0)
			    پہلا_سطر = lnum;
			آخری_سطر = lnum;
			ptr = new_سطر;
			old_len = new_len - 1;
			ستون = آغاز_ستون + len;
		    }
		}
		got_tab = FALSE;
		عدد_spaces = 0;

		اگر (is_indent_only)
		    توڑو;
	    }
	    اگر (ptr[ستون] == NUL)
		توڑو;
	    vcol += chartabsize(ptr + ستون, (colnr_T)vcol);
	    اگر (vcol >= MAXCOL)
	    {
		emsg_متن_too_long();
		توڑو;
	    }
	    اگر (has_mbyte)
		ستون += (*mb_ptr2len)(ptr + ستون);
	    ورنہ
		++ستون;
	}
	اگر (new_سطر == تہی)		    // out of memory
	    توڑو;
	سطر_breakcheck();
    }
    اگر (got_int)
	emsg(_(e_interrupted));

#اگر_تعریف_شدہ FEAT_VARTABS
    // If a single value was given then it can be considered equal to
    // either the value of 'tabstop' or the value of 'vartabstop'.
    اگر (tabstop_گنتی(curbuf->b_p_vts_array) == 0
	&& tabstop_گنتی(new_vts_array) == 1
	&& curbuf->b_p_ts == tabstop_پہلا(new_vts_array))
	; // not changed
    ورنہ اگر (tabstop_گنتی(curbuf->b_p_vts_array) > 0
	&& tabstop_eq(curbuf->b_p_vts_array, new_vts_array))
	; // not changed
    ورنہ
	redraw_curbuf_later(UPD_NOT_VALID);
#ورنہ
    اگر (curbuf->b_p_ts != new_ts)
	redraw_curbuf_later(UPD_NOT_VALID);
#اگر_ختم
    اگر (پہلا_سطر != 0)
	بدلا_سطریں(پہلا_سطر, 0, آخری_سطر + 1, 0L);

    curwin->w_p_list = save_list;	// restore 'list'

#اگر_تعریف_شدہ FEAT_VARTABS
    اگر (new_ts_str != تہی)		// set the new tabstop
    {
	// If 'vartabstop' is in use or if the value given to retab has more
	// than one tabstop then update 'vartabstop'.
	صحیح *old_vts_ary = curbuf->b_p_vts_array;

	اگر (tabstop_گنتی(old_vts_ary) > 0 || tabstop_گنتی(new_vts_array) > 1)
	{
	    سیٹ_string_option_براہ_راست((char_u *)"vts", -1, new_ts_str,
							OPT_FREE|OPT_LOCAL, 0);
	    curbuf->b_p_vts_array = new_vts_array;
	    vim_free(old_vts_ary);
	}
	ورنہ
	{
	    // 'vartabstop' wasn't in use and a single value was given to
	    // retab then update 'tabstop'.
	    curbuf->b_p_ts = tabstop_پہلا(new_vts_array);
	    vim_free(new_vts_array);
	}
	vim_free(new_ts_str);
    }
#ورنہ
    curbuf->b_p_ts = new_ts;
#اگر_ختم
    coladvance(curwin->w_curswant);

    u_clearline();
}

#اگر defined(FEAT_EVAL)
/*
 * Get indent level from 'indentexpr'.
 */
    صحیح
get_اظہاریہ_indent(خالی)
{
    صحیح		indent = -1;
    char_u	*inde_copy;
    pos_T	save_pos;
    colnr_T	save_curswant;
    صحیح		save_سیٹ_curswant;
    صحیح		save_State;
    صحیح		use_sandbox = was_سیٹ_insecurely(curwin,
					    (char_u *)"indentexpr", OPT_LOCAL);
    sctx_T	save_sctx = current_sctx;

    // Save and restore cursor position and curswant, in case it was changed
    // via :normal commands
    save_pos = curwin->w_cursor;
    save_curswant = curwin->w_curswant;
    save_سیٹ_curswant = curwin->w_سیٹ_curswant;
    سیٹ_vim_var_nr(VV_LNUM, curwin->w_cursor.lnum);
    اگر (use_sandbox)
	++sandbox;
    ++textlock;
    current_sctx = curbuf->b_p_script_ctx[BV_INDE];

    // Need to make a copy, the 'indentexpr' option could be changed while
    // evaluating it.
    inde_copy = vim_strsave(curbuf->b_p_inde);
    اگر (inde_copy != تہی)
    {
	indent = (صحیح)eval_مقصد_نمبر(inde_copy, TRUE);
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
    State = منوال_داخل_کرو;
    curwin->w_cursor = save_pos;
    curwin->w_curswant = save_curswant;
    curwin->w_سیٹ_curswant = save_سیٹ_curswant;
    check_cursor();
    State = save_State;

    // Reset did_throw, unless 'debug' has "throw" and inside a try/catch.
    اگر (did_throw && (vim_strchr(p_debug, 't') == تہی || trylevel == 0))
    {
	handle_did_throw();
	did_throw = FALSE;
    }

    // If there is an error, just keep the current indent.
    اگر (indent < 0)
	indent = get_indent();

    واپس indent;
}
#اگر_ختم

    ساکن صحیح
lisp_مطابقت(char_u *p)
{
    char_u	buf[LSIZE];
    صحیح		len;
    char_u	*لفظ = *curbuf->b_p_lw != NUL ? curbuf->b_p_lw : p_lispwords;

    جب (*لفظ != NUL)
    {
	len = copy_option_part(&لفظ, buf, LSIZE, ",");
	اگر (STRNCMP(buf, p, len) == 0 && IS_WHITE_OR_NUL(p[len]))
	    واپس TRUE;
    }
    واپس FALSE;
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
get_lisp_indent(خالی)
{
    pos_T	*pos, realpos, paren;
    صحیح		amount;
    char_u	*that;
    colnr_T	ستون;
    colnr_T	firsttry;
    صحیح		parencount, quotecount;
    صحیح		vi_lisp;

    // Set vi_lisp to use the vi-compatible method
    vi_lisp = (vim_strchr(p_cpo, CPO_LISP) != تہی);

    realpos = curwin->w_cursor;
    curwin->w_cursor.ستون = 0;

    اگر ((pos = findmatch(تہی, '(')) == تہی)
	pos = findmatch(تہی, '[');
    ورنہ
    {
	paren = *pos;
	pos = findmatch(تہی, '[');
	اگر (pos == تہی || LT_POSP(pos, &paren))
	    pos = &paren;
    }
    اگر (pos != تہی)
    {
	// Extra trick: Take the indent of the first previous non-white
	// line that is at the same () level.
	amount = -1;
	parencount = 0;

	جب (--curwin->w_cursor.lnum >= pos->lnum)
	{
	    اگر (linewhite(curwin->w_cursor.lnum))
		جاری;
	    برائے (that = ml_get_curline(); *that != NUL; ++that)
	    {
		اگر (*that == ';')
		{
		    جب (*(that + 1) != NUL)
			++that;
		    جاری;
		}
		اگر (*that == '\\')
		{
		    اگر (*(that + 1) != NUL)
			++that;
		    جاری;
		}
		اگر (*that == '"' && *(that + 1) != NUL)
		{
		    جب (*++that && *that != '"')
		    {
			// skipping escaped characters in the string
			اگر (*that == '\\')
			{
			    اگر (*++that == NUL)
				توڑو;
			    اگر (that[1] == NUL)
			    {
				++that;
				توڑو;
			    }
			}
		    }
		    اگر (*that == NUL)
			توڑو;
		}
		اگر (*that == '(' || *that == '[')
		    ++parencount;
		ورنہ اگر (*that == ')' || *that == ']')
		    --parencount;
	    }
	    اگر (parencount == 0)
	    {
		amount = get_indent();
		توڑو;
	    }
	}

	اگر (amount == -1)
	{
	    curwin->w_cursor.lnum = pos->lnum;
	    curwin->w_cursor.ستون = pos->ستون;
	    ستون = pos->ستون;

	    that = ml_get_curline();

	    اگر (vi_lisp && get_indent() == 0)
		amount = 2;
	    ورنہ
	    {
		char_u		*سطر = that;
		chartabsize_T	cts;

		init_chartabsize_arg(&cts, curwin, pos->lnum, 0, سطر, سطر);
		جب (*cts.cts_ptr != NUL && ستون > 0)
		{
		    cts.cts_vcol += lbr_chartabsize_adv(&cts);
		    ستون--;
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
						      && lisp_مطابقت(that + 1))
		    amount += 2;
		ورنہ
		{
		    اگر (*that != NUL)
		    {
			that++;
			amount++;
		    }
		    firsttry = amount;

		    init_chartabsize_arg(&cts, curwin, (colnr_T)(that - سطر),
							   amount, سطر, that);
		    جب (VIM_ISWHITE(*cts.cts_ptr))
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
			    جب (*cts.cts_ptr
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
			جب (VIM_ISWHITE(*cts.cts_ptr))
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
    ورنہ
	amount = 0;	// no matching '(' or '[' found, use zero indent

    curwin->w_cursor = realpos;

    واپس amount;
}

/*
 * Re-indent the current line, based on the current contents of it and the
 * surrounding lines. Fixing the cursor position seems really easy -- I'm very
 * confused what all the part that handles Control-T is doing that I'm not.
 * "get_the_indent" should be get_c_indent, get_expr_indent or get_lisp_indent.
 */

    خالی
fixthisline(صحیح (*get_the_indent)(خالی))
{
    صحیح amount = get_the_indent();

    اگر (amount < 0)
	واپس;

    change_indent(INDENT_سیٹ, amount, FALSE, 0, TRUE);
    اگر (linewhite(curwin->w_cursor.lnum))
	did_ai = TRUE;	// delete the indent if the line stays empty
}

/*
 * Return TRUE if 'indentexpr' should be used for Lisp indenting.
 * Caller may want to check 'autoindent'.
 */
    صحیح
use_indentexpr_for_lisp(خالی)
{
#اگر_تعریف_شدہ FEAT_EVAL
    واپس curbuf->b_p_lisp
		&& *curbuf->b_p_inde != NUL
		&& STRCMP(curbuf->b_p_lop, "expr:1") == 0;
#ورنہ
    واپس FALSE;
#اگر_ختم
}

/*
 * Fix indent for 'lisp' and 'cindent'.
 */
    خالی
fix_indent(خالی)
{
    اگر (p_paste)
	واپس;  // no auto-indenting when 'paste' is set
    اگر (curbuf->b_p_lisp && curbuf->b_p_ai)
    {
	اگر (use_indentexpr_for_lisp())
	    do_c_اظہاریہ_indent();
	ورنہ
	    fixthisline(get_lisp_indent);
    }
    ورنہ اگر (cindent_on())
	do_c_اظہاریہ_indent();
}

#اگر defined(FEAT_EVAL)
/*
 * "indent()" function
 */
    خالی
f_indent(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    اگر (اندر_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	واپس;

    lnum = tv_get_lnum(argvars);
    اگر (lnum >= 1 && lnum <= curbuf->b_ml.ml_سطر_گنتی)
	rettv->vval.v_نمبر = get_indent_lnum(lnum);
    ورنہ
    {
	اگر (اندر_vim9script())
	    semsg(_(e_invalid_سطر_نمبر_nr), lnum);
	rettv->vval.v_نمبر = -1;
    }
}

/*
 * "lispindent(lnum)" function
 */
    خالی
f_lispindent(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    pos_T	pos;
    linenr_T	lnum;

    اگر (اندر_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	واپس;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    اگر (lnum >= 1 && lnum <= curbuf->b_ml.ml_سطر_گنتی)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_نمبر = get_lisp_indent();
	curwin->w_cursor = pos;
    }
    ورنہ اگر (اندر_vim9script())
	semsg(_(e_invalid_سطر_نمبر_nr), lnum);
    ورنہ
	rettv->vval.v_نمبر = -1;
}
#اگر_ختم
