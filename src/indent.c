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

#ضمن "vim.h"

#إذا defined(FEAT_VARTABS)

/*
 * Set the integer values corresponding to the string setting of 'vartabstop'.
 * "array" will be set, caller must free it if needed.
 * Return FAIL for an error.
 */
    صحيح
tabstop_سيٽ(char_u *var, صحيح **array)
{
    صحيح	    valcount = 1;
    صحيح	    t;
    char_u  *cp;

    إذا (var[0] == NUL || (var[0] == '0' && var[1] == NUL))
    {
	*array = عدم;
	أرجع OK;
    }

    لكل (cp = var; *cp != NUL; ++cp)
    {
	إذا (cp == var || cp[-1] == ',')
	{
	    char_u *پڇاڙي;

	    إذا (strtol((حرف *)cp, (حرف **)&پڇاڙي, 10) <= 0)
	    {
		إذا (cp != پڇاڙي)
		    emsg(_(e_دليل_must_be_مثبت));
		وإلا
		    semsg(_(e_invalid_دليل_str), cp);
		أرجع FAIL;
	    }
	}

	إذا (VIM_ISDIGIT(*cp))
	    استمر;
	إذا (cp[0] == ',' && cp > var && cp[-1] != ',' && cp[1] != NUL)
	{
	    ++valcount;
	    استمر;
	}
	semsg(_(e_invalid_دليل_str), var);
	أرجع FAIL;
    }

    *array = ALLOC_MULT(صحيح, valcount + 1);
    إذا (*array == عدم)
	أرجع FAIL;
    (*array)[0] = valcount;

    t = 1;
    لكل (cp = var; *cp != NUL;)
    {
	صحيح n = atoi((حرف *)cp);

	// Catch negative values, overflow and ridiculous big values.
	إذا (n <= 0 || n > TABSTOP_MAX)
	{
	    semsg(_(e_invalid_دليل_str), cp);
	    VIM_CLEAR(*array);
	    أرجع FAIL;
	}
	(*array)[t++] = n;
	طالما (*cp != NUL && *cp != ',')
	    ++cp;
	إذا (*cp != NUL)
	    ++cp;
    }

    أرجع OK;
}

/*
 * Calculate the number of screen spaces a tab will occupy.
 * If "vts" is set then the tab widths are taken from that array,
 * otherwise the value of ts is used.
 */
    صحيح
tabstop_padding(colnr_T کانو, صحيح ts_arg, صحيح *vts)
{
    صحيح		ts = ts_arg == 0 ? 8 : ts_arg;
    صحيح		tabcount;
    colnr_T	tabcol = 0;
    صحيح		t;
    صحيح		padding = 0;

    إذا (vts == عدم || vts[0] == 0)
	أرجع ts - (کانو % ts);

    tabcount = vts[0];

    لكل (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	إذا (tabcol > کانو)
	{
	    padding = (صحيح)(tabcol - کانو);
	    اكسر;
	}
    }
    إذا (t > tabcount)
	padding = vts[tabcount] - (صحيح)((کانو - tabcol) % vts[tabcount]);

    أرجع padding;
}

/*
 * Find the size of the tab that covers a particular column.
 *
 * If this is being called as part of a shift operation, col is not the cursor
 * column but is the column number to the left of the first non-whitespace
 * character in the line.  If the shift is to the left (left = TRUE), then
 * return the size of the tab interval to the left of the column.
 */
    صحيح
tabstop_at(colnr_T کانو, صحيح ts, صحيح *vts, صحيح کاٻو)
{
    صحيح		tabcount;	// Number of tab stops in the list of variable
				// tab stops.
    colnr_T	tabcol = 0;	// Column of the tab stop under consideration.
    صحيح		t;		// Tabstop index in the list of variable tab
				// stops.
    صحيح		tab_سائيز = 0;	// Size of the tab stop interval to the right
				// or left of the col.

    إذا (vts == 0 || vts[0] == 0)
	أرجع ts;

    tabcount = vts[0];
    لكل (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	إذا (tabcol > کانو)
	{
	    // If shifting left (left != 0), and if the column to the left of
	    // the first first non-blank character (col) in the line is
	    // already to the left of the first tabstop, set the shift amount
	    // (tab_size) to just enough to shift the line to the left margin.
	    // The value doesn't seem to matter as long as it is at least that
	    // distance.
	    إذا (کاٻو && (t == 1))
		tab_سائيز = کانو;
	    وإلا
		tab_سائيز = vts[t - (کاٻو ? 1 : 0)];
	    اكسر;
	}
    }
    إذا (t > tabcount)		// If the value of the index t is beyond the
				// end of the list, use the tab stop value at
				// the end of the list.
	tab_سائيز = vts[tabcount];

    أرجع tab_سائيز;
}

/*
 * Find the column on which a tab starts.
 */
    colnr_T
tabstop_شروعات(colnr_T کانو, صحيح ts, صحيح *vts)
{
    صحيح		tabcount;
    colnr_T	tabcol = 0;
    صحيح		t;
    صحيح		excess;

    إذا (vts == عدم || vts[0] == 0)
	أرجع کانو - کانو % ts;

    tabcount = vts[0];
    لكل (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	إذا (tabcol > کانو)
	    أرجع tabcol - vts[t];
    }

    excess = tabcol % vts[tabcount];
    أرجع کانو - (کانو - excess) % vts[tabcount];
}

/*
 * Find the number of tabs and spaces necessary to get from one column
 * to another.
 */
    فراغ
tabstop_fromto(
	colnr_T شروعات_کانو,
	colnr_T پڇاڙي_کانو,
	صحيح	ts_arg,
	صحيح	*vts,
	صحيح	*ntabs,
	صحيح	*nspcs)
{
    صحيح		spaces = پڇاڙي_کانو - شروعات_کانو;
    colnr_T	tabcol = 0;
    صحيح		padding = 0;
    صحيح		tabcount;
    صحيح		t;
    صحيح		ts = ts_arg == 0 ? curbuf->b_p_ts : ts_arg;

    إذا (vts == عدم || vts[0] == 0)
    {
	صحيح tabs = 0;
	صحيح initspc = 0;

	initspc = ts - (شروعات_کانو % ts);
	إذا (spaces >= initspc)
	{
	    spaces -= initspc;
	    tabs++;
	}
	tabs += spaces / ts;
	spaces -= (spaces / ts) * ts;

	*ntabs = tabs;
	*nspcs = spaces;
	أرجع;
    }

    // Find the padding needed to reach the next tabstop.
    tabcount = vts[0];
    لكل (t = 1; t <= tabcount; ++t)
    {
	tabcol += vts[t];
	إذا (tabcol > شروعات_کانو)
	{
	    padding = (صحيح)(tabcol - شروعات_کانو);
	    اكسر;
	}
    }
    إذا (t > tabcount)
	padding = vts[tabcount] - (صحيح)((شروعات_کانو - tabcol) % vts[tabcount]);

    // If the space needed is less than the padding no tabs can be used.
    إذا (spaces < padding)
    {
	*ntabs = 0;
	*nspcs = spaces;
	أرجع;
    }

    *ntabs = 1;
    spaces -= padding;

    // At least one tab has been used. See if any more will fit.
    طالما (spaces != 0 && ++t <= tabcount)
    {
	padding = vts[t];
	إذا (spaces < padding)
	{
	    *nspcs = spaces;
	    أرجع;
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
    ساكن صحيح
tabstop_eq(صحيح *ts1, صحيح *ts2)
{
    صحيح		t;

    إذا ((ts1 == 0 && ts2) || (ts1 && ts2 == 0))
	أرجع FALSE;
    إذا (ts1 == ts2)
	أرجع TRUE;
    إذا (ts1[0] != ts2[0])
	أرجع FALSE;

    لكل (t = 1; t <= ts1[0]; ++t)
	إذا (ts1[t] != ts2[t])
	    أرجع FALSE;

    أرجع TRUE;
}

# إذا defined(FEAT_BEVAL)
/*
 * Copy a tabstop array, allocating space for the new array.
 */
    صحيح *
tabstop_copy(صحيح *oldts)
{
    صحيح		*newts;
    صحيح		t;

    إذا (oldts == عدم)
	أرجع عدم;
    newts = ALLOC_MULT(صحيح, oldts[0] + 1);
    إذا (newts != عدم)
	لكل (t = 0; t <= oldts[0]; ++t)
	    newts[t] = oldts[t];
    أرجع newts;
}
# endif

/*
 * Return a count of the number of tabstops.
 */
    صحيح
tabstop_ڳڻپ(صحيح *ts)
{
    أرجع ts != عدم ? ts[0] : 0;
}

/*
 * Return the first tabstop, or 8 if there are no tabstops defined.
 */
    صحيح
tabstop_پهريون(صحيح *ts)
{
    أرجع ts != عدم ? ts[1] : 8;
}

#جيڪڏهن_پورو

/*
 * Return the effective shiftwidth value for current buffer, using the
 * 'tabstop' value when 'shiftwidth' is zero.
 */
    ڊگهو
get_sw_قدر(buf_T *buf)
{
    أرجع get_sw_قدر_کانو(buf, 0, FALSE);
}

/*
 * Idem, using "pos".
 */
    ساكن ڊگهو
get_sw_قدر_pos(buf_T *buf, pos_T *pos, صحيح کاٻو)
{
    pos_T save_cursor = curwin->w_cursor;
    ڊگهو sw_قدر;

    curwin->w_cursor = *pos;
    sw_قدر = get_sw_قدر_کانو(buf, get_nolist_virtcol(), کاٻو);
    curwin->w_cursor = save_cursor;
    أرجع sw_قدر;
}

/*
 * Idem, using the first non-black in the current line.
 */
    ڊگهو
get_sw_قدر_indent(buf_T *buf, صحيح کاٻو)
{
    pos_T pos = curwin->w_cursor;

    pos.کانو = getwhitecols_curline();
    أرجع get_sw_قدر_pos(buf, &pos, کاٻو);
}

/*
 * Idem, using virtual column "col".
 */
    ڊگهو
get_sw_قدر_کانو(buf_T *buf, colnr_T کانو اڻ_استعمال_ٿيل, صحيح کاٻو اڻ_استعمال_ٿيل)
{
    أرجع buf->b_p_sw ? buf->b_p_sw :
#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
	tabstop_at(کانو, buf->b_p_ts, buf->b_p_vts_array, کاٻو);
#وإلا
	buf->b_p_ts;
#جيڪڏهن_پورو
}

/*
 * Return the effective softtabstop value for the current buffer, using the
 * 'shiftwidth' value when 'softtabstop' is negative.
 */
    ڊگهو
get_sts_قدر(فراغ)
{
    أرجع curbuf->b_p_sts < 0 ? get_sw_قدر(curbuf) : curbuf->b_p_sts;
}

/*
 * Count the size (in window cells) of the indent in the current line.
 */
    صحيح
get_indent(فراغ)
{
#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
    أرجع get_indent_str_vtab(ml_get_curline(), (صحيح)curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#وإلا
    أرجع get_indent_str(ml_get_curline(), (صحيح)curbuf->b_p_ts, FALSE);
#جيڪڏهن_پورو
}

/*
 * Count the size (in window cells) of the indent in line "lnum".
 */
    صحيح
get_indent_lnum(linenr_T lnum)
{
#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
    أرجع get_indent_str_vtab(ml_get(lnum), (صحيح)curbuf->b_p_ts,
						 curbuf->b_p_vts_array, FALSE);
#وإلا
    أرجع get_indent_str(ml_get(lnum), (صحيح)curbuf->b_p_ts, FALSE);
#جيڪڏهن_پورو
}

#إذا defined(FEAT_FOLDING)
/*
 * Count the size (in window cells) of the indent in line "lnum" of buffer
 * "buf".
 */
    صحيح
get_indent_buf(buf_T *buf, linenr_T lnum)
{
# ifdef FEAT_VARTABS
    أرجع get_indent_str_vtab(ml_get_buf(buf, lnum, FALSE),
			       (صحيح)buf->b_p_ts, buf->b_p_vts_array, FALSE);
# وإلا
    أرجع get_indent_str(ml_get_buf(buf, lnum, FALSE), (صحيح)buf->b_p_ts, FALSE);
# endif
}
#جيڪڏهن_پورو

/*
 * count the size (in window cells) of the indent in line "ptr", with
 * 'tabstop' at "ts"
 */
    صحيح
get_indent_str(
    char_u	*ptr,
    صحيح		ts,
    صحيح		no_ts) // if TRUE, count a tab as ^I
{
    صحيح		ڳڻپ = 0;

    لكل ( ; *ptr; ++ptr)
    {
	إذا (*ptr == TAB)    // count a tab for what it is worth
	{
	    إذا (!no_ts)
		ڳڻپ += ts - (ڳڻپ % ts);
	    وإلا
		ڳڻپ += ptr2cells(ptr);
	}
	وإلا إذا (*ptr == ' ')
	    ++ڳڻپ;		// count a space for one
	وإلا
	    اكسر;
    }
    أرجع ڳڻپ;
}

#إذا defined(FEAT_VARTABS)
/*
 * Count the size (in window cells) of the indent in line "ptr", using
 * variable tabstops.
 * If "no_ts" is TRUE, count a tab as ^I.
 */
    صحيح
get_indent_str_vtab(char_u *ptr, صحيح ts, صحيح *vts, صحيح no_ts)
{
    صحيح		ڳڻپ = 0;

    لكل ( ; *ptr; ++ptr)
    {
	إذا (*ptr == TAB)    // count a tab for what it is worth
	{
	    إذا (!no_ts)
		ڳڻپ += tabstop_padding(ڳڻپ, ts, vts);
	    وإلا
		ڳڻپ += ptr2cells(ptr);
	}
	وإلا إذا (*ptr == ' ')
	    ++ڳڻپ;		// count a space for one
	وإلا
	    اكسر;
    }
    أرجع ڳڻپ;
}
#جيڪڏهن_پورو

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
    صحيح
سيٽ_indent(
    صحيح		سائيز,		    // measured in spaces
    صحيح		flags)
{
    char_u	*p;
    char_u	*newline;
    char_u	*oldline;
    char_u	*s;
    صحيح		todo;
    صحيح		ind_len;	    // measured in characters
    صحيح		سٽ_len;	    // size of the line (including the NUL)
    صحيح		doit = FALSE;
    صحيح		ind_پورو = 0;	    // measured in spaces
#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
    صحيح		ind_کانو = 0;
#جيڪڏهن_پورو
    صحيح		tab_pad;
    صحيح		retval = FALSE;
    صحيح		orig_char_len = -1; // number of initial whitespace chars when
				    // 'et' and 'pi' are both set

    // First check if there is anything to do and compute the number of
    // characters needed for the indent.
    todo = سائيز;
    ind_len = 0;
    p = oldline = ml_get_curline();
    سٽ_len = ml_get_curline_len() + 1;

    // Calculate the buffer size for the new indent, and check to see if it
    // isn't already set

    // if 'expandtab' isn't set: use TABs; if both 'expandtab' and
    // 'preserveindent' are set count the number of characters at the
    // beginning of the line to be copied
    إذا (!curbuf->b_p_et || (!(flags & SIN_وجهو) && curbuf->b_p_pi))
    {
	// If 'preserveindent' is set then reuse as much as possible of
	// the existing indent structure for the new indent
	إذا (!(flags & SIN_وجهو) && curbuf->b_p_pi)
	{
	    ind_پورو = 0;

	    // count as many characters as we can use
	    طالما (todo > 0 && VIM_ISWHITE(*p))
	    {
		إذا (*p == TAB)
		{
#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
		    tab_pad = tabstop_padding(ind_پورو, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#وإلا
		    tab_pad = (صحيح)curbuf->b_p_ts
					   - (ind_پورو % (صحيح)curbuf->b_p_ts);
#جيڪڏهن_پورو
		    // stop if this tab will overshoot the target
		    إذا (todo < tab_pad)
			اكسر;
		    todo -= tab_pad;
		    ++ind_len;
		    ind_پورو += tab_pad;
		}
		وإلا
		{
		    --todo;
		    ++ind_len;
		    ++ind_پورو;
		}
		++p;
	    }

#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
	    // These diverge from this point.
	    ind_کانو = ind_پورو;
#جيڪڏهن_پورو
	    // Set initial number of whitespace chars to copy if we are
	    // preserving indent but expandtab is set
	    إذا (curbuf->b_p_et)
		orig_char_len = ind_len;

	    // Fill to next tabstop with a tab, if possible
#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
	    tab_pad = tabstop_padding(ind_پورو, curbuf->b_p_ts,
						curbuf->b_p_vts_array);
#وإلا
	    tab_pad = (صحيح)curbuf->b_p_ts - (ind_پورو % (صحيح)curbuf->b_p_ts);
#جيڪڏهن_پورو
	    إذا (todo >= tab_pad && orig_char_len == -1)
	    {
		doit = TRUE;
		todo -= tab_pad;
		++ind_len;
		// ind_done += tab_pad;
#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
		ind_کانو += tab_pad;
#جيڪڏهن_پورو
	    }
	}

	// count tabs required for indent
#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
	لكل (;;)
	{
	    tab_pad = tabstop_padding(ind_کانو, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
	    إذا (todo < tab_pad)
		اكسر;
	    إذا (*p != TAB)
		doit = TRUE;
	    وإلا
		++p;
	    todo -= tab_pad;
	    ++ind_len;
	    ind_کانو += tab_pad;
	}
#وإلا
	طالما (todo >= (صحيح)curbuf->b_p_ts)
	{
	    إذا (*p != TAB)
		doit = TRUE;
	    وإلا
		++p;
	    todo -= (صحيح)curbuf->b_p_ts;
	    ++ind_len;
	    // ind_done += (int)curbuf->b_p_ts;
	}
#جيڪڏهن_پورو
    }
    // count spaces required for indent
    طالما (todo > 0)
    {
	إذا (*p != ' ')
	    doit = TRUE;
	وإلا
	    ++p;
	--todo;
	++ind_len;
	// ++ind_done;
    }

    // Return if the indent is OK already.
    إذا (!doit && !VIM_ISWHITE(*p) && !(flags & SIN_وجهو))
	أرجع FALSE;

    // Allocate memory for the new line.
    إذا (flags & SIN_وجهو)
	p = oldline;
    وإلا
    {
	p = skipwhite(p);
	سٽ_len -= (صحيح)(p - oldline);
    }

    // If 'preserveindent' and 'expandtab' are both set keep the original
    // characters and allocate accordingly.  We will fill the rest with spaces
    // after the if (!curbuf->b_p_et) below.
    إذا (orig_char_len != -1)
    {
	newline = alloc(orig_char_len + سائيز - ind_پورو + سٽ_len);
	إذا (newline == عدم)
	    أرجع FALSE;
	todo = سائيز - ind_پورو;
	ind_len = orig_char_len + todo;    // Set total length of indent in
					   // characters, which may have been
					   // undercounted until now
	p = oldline;
	s = newline;
	طالما (orig_char_len > 0)
	{
	    *s++ = *p++;
	    orig_char_len--;
	}

	// Skip over any additional white space (useful when newindent is less
	// than old)
	طالما (VIM_ISWHITE(*p))
	    ++p;

    }
    وإلا
    {
	todo = سائيز;
	newline = alloc(ind_len + سٽ_len);
	إذا (newline == عدم)
	    أرجع FALSE;
	s = newline;
    }

    // Put the characters in the new line.
    // if 'expandtab' isn't set: use TABs
    إذا (!curbuf->b_p_et)
    {
	// If 'preserveindent' is set then reuse as much as possible of
	// the existing indent structure for the new indent
	إذا (!(flags & SIN_وجهو) && curbuf->b_p_pi)
	{
	    p = oldline;
	    ind_پورو = 0;

	    طالما (todo > 0 && VIM_ISWHITE(*p))
	    {
		إذا (*p == TAB)
		{
#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
		    tab_pad = tabstop_padding(ind_پورو, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#وإلا
		    tab_pad = (صحيح)curbuf->b_p_ts
					   - (ind_پورو % (صحيح)curbuf->b_p_ts);
#جيڪڏهن_پورو
		    // stop if this tab will overshoot the target
		    إذا (todo < tab_pad)
			اكسر;
		    todo -= tab_pad;
		    ind_پورو += tab_pad;
		}
		وإلا
		{
		    --todo;
		    ++ind_پورو;
		}
		*s++ = *p++;
	    }

	    // Fill to next tabstop with a tab, if possible
#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
	    tab_pad = tabstop_padding(ind_پورو, curbuf->b_p_ts,
						curbuf->b_p_vts_array);
#وإلا
	    tab_pad = (صحيح)curbuf->b_p_ts - (ind_پورو % (صحيح)curbuf->b_p_ts);
#جيڪڏهن_پورو
	    إذا (todo >= tab_pad)
	    {
		*s++ = TAB;
		todo -= tab_pad;
#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
		ind_پورو += tab_pad;
#جيڪڏهن_پورو
	    }

	    p = skipwhite(p);
	}

#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
	لكل (;;)
	{
	    tab_pad = tabstop_padding(ind_پورو, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
	    إذا (todo < tab_pad)
		اكسر;
	    *s++ = TAB;
	    todo -= tab_pad;
	    ind_پورو += tab_pad;
	}
#وإلا
	طالما (todo >= (صحيح)curbuf->b_p_ts)
	{
	    *s++ = TAB;
	    todo -= (صحيح)curbuf->b_p_ts;
	}
#جيڪڏهن_پورو
    }
    طالما (todo > 0)
    {
	*s++ = ' ';
	--todo;
    }
    mch_memmove(s, p, (size_t)سٽ_len);

    // Replace the line (unless undo fails).
    إذا (!(flags & SIN_UNDO) || u_savesub(curwin->w_cursor.lnum) == OK)
    {
	colnr_T old_وٿي = (colnr_T)(p - oldline);
	colnr_T new_وٿي = (colnr_T)(s - newline);

	// this may free "newline"
	ml_replace(curwin->w_cursor.lnum, newline, FALSE);
	إذا (flags & SIN_بدليو)
	    بدليو_بائيٽون(curwin->w_cursor.lnum, 0);

	// Correct saved cursor position if it is in this line.
	إذا (saved_cursor.lnum == curwin->w_cursor.lnum)
	{
	    إذا (saved_cursor.کانو >= old_وٿي)
		// cursor was after the indent, adjust for the number of
		// bytes added/removed
		saved_cursor.کانو += ind_len - old_وٿي;
	    وإلا إذا (saved_cursor.کانو >= new_وٿي)
		// cursor was in the indent, and is now after it, put it back
		// at the start of the indent (replacing spaces with TAB)
		saved_cursor.کانو = new_وٿي;
	}
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
	إذا (curbuf->b_has_textprop)
	{
	    صحيح added = ind_len - old_وٿي;

	    // When increasing indent this behaves like spaces were inserted at
	    // the old indent, when decreasing indent it behaves like spaces
	    // were deleted at the new indent.
	    adjust_prop_columns(curwin->w_cursor.lnum,
				  added > 0 ? old_وٿي : (colnr_T)ind_len,
				  added, APC_INDENT);
	}
#جيڪڏهن_پورو
	retval = TRUE;
    }
    وإلا
	vim_free(newline);

    curwin->w_cursor.کانو = ind_len;
    أرجع retval;
}

/*
 * Return the indent of the current line after a number.  Return -1 if no
 * number was found.  Used for 'n' in 'formatoptions': numbered list.
 * Since a pattern is used it can actually handle more than numbers.
 */
    صحيح
get_عدد_indent(linenr_T lnum)
{
    colnr_T	کانو;
    pos_T	pos;

    regmatch_T	regmatch;
    صحيح		lead_len = 0;	// length of comment leader

    إذا (lnum > curbuf->b_ml.ml_سٽ_ڳڻپ)
	أرجع -1;
    pos.lnum = 0;

    // In format_lines() (i.e. not insert mode), fo+=q is needed too...
    إذا ((State & گهڻائي_وجهو) || has_format_option(FO_Q_COMS))
	lead_len = get_leader_len(ml_get(lnum), عدم, FALSE, TRUE);

    regmatch.regprog = vim_regcomp(curbuf->b_p_flp, RE_MAGIC);
    إذا (regmatch.regprog != عدم)
    {
	regmatch.rm_ic = FALSE;

	// vim_regexec() expects a pointer to a line.  This lets us
	// start matching for the flp beyond any comment leader...
	إذا (vim_regexec(&regmatch, ml_get(lnum) + lead_len, (colnr_T)0))
	{
	    pos.lnum = lnum;
	    pos.کانو = (colnr_T)(*regmatch.endp - ml_get(lnum));
	    pos.coladd = 0;
	}
	vim_regfree(regmatch.regprog);
    }

    إذا (pos.lnum == 0 || *ml_get_pos(&pos) == NUL)
	أرجع -1;
    getvcol(curwin, &pos, &کانو, عدم, عدم, 0);
    أرجع (صحيح)کانو;
}

#إذا defined(FEAT_LINEBREAK)
/*
 * Check "briopt" as 'breakindentopt' and update the members of "wp".
 * This is called when 'breakindentopt' is changed and when a window is
 * initialized.
 * Returns FAIL for failure, OK otherwise.
 */
    صحيح
briopt_check(
    char_u	*briopt,	// when NULL: use "wp->w_p_briopt"
    win_T	*wp)		// when NULL: only check "briopt"
{
    char_u	*p;
    صحيح		bri_سرڪايو = 0;
    ڊگهو	bri_min = 20;
    صحيح		bri_sbr = FALSE;
    صحيح		bri_list = 0;
    صحيح		bri_vcol = 0;

    إذا (briopt != عدم)
	p = briopt;
    وإلا
	p = wp->w_p_briopt;

    طالما (*p != NUL)
    {
	// Note: Keep this in sync with p_briopt_values
	إذا (STRNCMP(p, "shift:", 6) == 0
		 && ((p[6] == '-' && VIM_ISDIGIT(p[7])) || VIM_ISDIGIT(p[6])))
	{
	    p += 6;
	    bri_سرڪايو = getdigits(&p);
	}
	وإلا إذا (STRNCMP(p, "min:", 4) == 0 && VIM_ISDIGIT(p[4]))
	{
	    p += 4;
	    bri_min = getdigits(&p);
	}
	وإلا إذا (STRNCMP(p, "sbr", 3) == 0)
	{
	    p += 3;
	    bri_sbr = TRUE;
	}
	وإلا إذا (STRNCMP(p, "list:", 5) == 0)
	{
	    p += 5;
	    bri_list = getdigits(&p);
	}
	وإلا إذا (STRNCMP(p, "column:", 7) == 0)
	{
	    p += 7;
	    bri_vcol = getdigits(&p);
	}
	إذا (*p != ',' && *p != NUL)
	    أرجع FAIL;
	إذا (*p == ',')
	    ++p;
    }

    إذا (wp == عدم)
	أرجع OK;

    wp->w_briopt_سرڪايو = bri_سرڪايو;
    wp->w_briopt_min   = bri_min;
    wp->w_briopt_sbr   = bri_sbr;
    wp->w_briopt_list  = bri_list;
    wp->w_briopt_vcol  = bri_vcol;

    أرجع OK;
}

/*
 * Return appropriate space number for breakindent, taking influencing
 * parameters into account. Window must be specified, since it is not
 * necessarily always the current one.
 */
    صحيح
get_breakindent_win(
    win_T	*wp,
    char_u	*سٽ) // start of the line
{
    ساكن صحيح	    prev_indent = 0;	// cached indent value
    ساكن ڊگهو	    prev_ts     = 0L;	// cached tabstop value
# ifdef FEAT_VARTABS
    ساكن صحيح      *prev_vts = عدم;   // cached vartabs values
# endif
    ساكن صحيح	    prev_fnum   = 0;	// cached buffer number
    ساكن char_u   *prev_سٽ  = عدم;	// cached copy of "line"
    ساكن varnumber_T prev_tick = 0;   // changedtick of cached value
    ساكن صحيح      prev_list = 0;	// cached list indent
    ساكن صحيح      prev_listopt = 0;	// cached w_p_briopt_list value
    ساكن صحيح      prev_no_ts = FALSE;	// cached no_ts value
    ساكن بنانشان prev_dy_uhex = 0;	// cached 'display' "uhex" value
    // cached formatlistpat value
    ساكن char_u   *prev_flp = عدم;
    صحيح		    bri = 0;
    // window width minus window margin space, i.e. what rests for text
    ثابت صحيح	    eff_wwidth = wp->w_ويڪر
					  - win_کانو_off(wp) + win_کانو_off2(wp);

    // In list mode, if 'listchars' "tab" isn't set, a TAB is displayed as ^I.
    صحيح no_ts = wp->w_p_list && wp->w_lcs_chars.tab1 == NUL;

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
    إذا (prev_fnum != wp->w_بفر->b_fnum
	    || prev_ts != wp->w_بفر->b_p_ts
# ifdef FEAT_VARTABS
	    || prev_vts != wp->w_بفر->b_p_vts_array
# endif
	    || prev_tick != CHANGEDTICK(wp->w_بفر)
	    || prev_listopt != wp->w_briopt_list
	    || prev_no_ts != no_ts
	    || prev_dy_uhex != (dy_flags & DY_UHEX)
	    || prev_flp == عدم
	    || STRCMP(prev_flp, get_flp_قدر(wp->w_بفر)) != 0
	    || prev_سٽ == عدم || STRCMP(prev_سٽ, سٽ) != 0
	)
    {
	prev_fnum = wp->w_بفر->b_fnum;
	vim_free(prev_سٽ);
	prev_سٽ = vim_strsave(سٽ);
	prev_ts = wp->w_بفر->b_p_ts;
# ifdef FEAT_VARTABS
	prev_vts = wp->w_بفر->b_p_vts_array;
	إذا (wp->w_briopt_vcol == 0)
	    prev_indent = get_indent_str_vtab(سٽ,
				     (صحيح)wp->w_بفر->b_p_ts,
				      wp->w_بفر->b_p_vts_array, no_ts);
# وإلا
	إذا (wp->w_briopt_vcol == 0)
	    prev_indent = get_indent_str(سٽ,
					(صحيح)wp->w_بفر->b_p_ts, no_ts);
# endif
	prev_tick = CHANGEDTICK(wp->w_بفر);
	prev_listopt = wp->w_briopt_list;
	prev_list = 0;
	prev_no_ts = no_ts;
	prev_dy_uhex = (dy_flags & DY_UHEX);
	vim_free(prev_flp);
	prev_flp = vim_strsave(get_flp_قدر(wp->w_بفر));
	// add additional indent for numbered lists
	إذا (wp->w_briopt_list != 0 && wp->w_briopt_vcol == 0)
	{
	    regmatch_T	    regmatch;

	    regmatch.regprog = vim_regcomp(prev_flp,
				   RE_MAGIC + RE_STRING + RE_AUTO + RE_STRICT);

	    إذا (regmatch.regprog != عدم)
	    {
		regmatch.rm_ic = FALSE;
		إذا (vim_regexec(&regmatch, سٽ, 0))
		{
		    إذا (wp->w_briopt_list > 0)
			prev_list = wp->w_briopt_list;
		    وإلا
		    {
			char_u	*ptr = *regmatch.startp;
			char_u	*end_ptr = *regmatch.endp;
			صحيح	indent = 0;

			// Compute the width of the matched text.
			// Use win_chartabsize() so that TAB size is correct,
			// while wrapping is ignored.
			طالما (ptr < end_ptr)
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
    إذا (wp->w_briopt_vcol != 0)
    {
	// column value has priority
	bri = wp->w_briopt_vcol;
	prev_list = 0;
    }
    وإلا
	bri = prev_indent + wp->w_briopt_سرڪايو;

    // Add offset for number column, if 'n' is in 'cpoptions'
    bri += win_کانو_off2(wp);

    // add additional indent for numbered lists
    إذا (wp->w_briopt_list > 0)
	bri += prev_list;

    // indent minus the length of the showbreak string
    إذا (wp->w_briopt_sbr)
	bri -= vim_strsize(get_showbreak_قدر(wp));


    // never indent past left window margin
    إذا (bri < 0)
	bri = 0;

    // always leave at least bri_min characters on the left,
    // if text width is sufficient
    وإلا إذا (bri > eff_wwidth - wp->w_briopt_min)
	bri = (eff_wwidth - wp->w_briopt_min < 0)
					   ? 0 : eff_wwidth - wp->w_briopt_min;

    أرجع bri;
}
#جيڪڏهن_پورو

/*
 * When extra == 0: Return TRUE if the cursor is before or on the first
 *		    non-blank in the line.
 * When extra == 1: Return TRUE if the cursor is before the first non-blank in
 *		    the line.
 */
    صحيح
inindent(صحيح extra)
{
    char_u	*ptr;
    colnr_T	کانو;

    لكل (کانو = 0, ptr = ml_get_curline(); VIM_ISWHITE(*ptr); ++کانو)
	++ptr;
    إذا (کانو >= curwin->w_cursor.کانو + extra)
	أرجع TRUE;
    وإلا
	أرجع FALSE;
}

/*
 * op_reindent - handle reindenting a block of lines.
 */
    فراغ
op_reindent(oparg_T *oap, صحيح (*how)(فراغ))
{
    ڊگهو	i = 0;
    char_u	*l;
    صحيح		amount;
    linenr_T	پهريون_بدليو = 0;
    linenr_T	آخري_بدليو = 0;
    linenr_T	شروعات_lnum = curwin->w_cursor.lnum;

    // Don't even try when 'modifiable' is off.
    إذا (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	أرجع;
    }

    // Save for undo.  Do this once for all lines, much faster than doing this
    // for each line separately, especially when undoing.
    إذا (u_savecommon(شروعات_lnum - 1, شروعات_lnum + oap->سٽ_ڳڻپ,
				     شروعات_lnum + oap->سٽ_ڳڻپ, FALSE) == OK)
	لكل (i = oap->سٽ_ڳڻپ; --i >= 0 && !got_int; )
	{
	    // it's a slow thing to do, so give feedback so there's no worry
	    // that the computer's just hung.

	    إذا (i > 1
		    && (i % 50 == 0 || i == oap->سٽ_ڳڻپ - 1)
		    && oap->سٽ_ڳڻپ > p_report)
		smsg(_("%ld lines to indent... "), i);

	    // Be vi-compatible: For lisp indenting the first line is not
	    // indented, unless there is only one line.
	    إذا (i != oap->سٽ_ڳڻپ - 1 || oap->سٽ_ڳڻپ == 1
						     || how != get_lisp_indent)
	    {
		l = skipwhite(ml_get_curline());
		إذا (*l == NUL)		    // empty or blank line
		    amount = 0;
		وإلا
		    amount = how();	    // get the indent for this line

		إذا (amount >= 0 && سيٽ_indent(amount, 0))
		{
		    // did change the indent, call changed_lines() later
		    إذا (پهريون_بدليو == 0)
			پهريون_بدليو = curwin->w_cursor.lnum;
		    آخري_بدليو = curwin->w_cursor.lnum;
		}
	    }
	    ++curwin->w_cursor.lnum;
	    curwin->w_cursor.کانو = 0;  // make sure it's valid
	}

    // put cursor on first non-blank of indented line
    curwin->w_cursor.lnum = شروعات_lnum;
    beginline(BL_SOL | BL_FIX);

    // Mark changed lines so that they will be redrawn.  When Visual
    // highlighting was present, need to continue until the last line.  When
    // there is no change still need to remove the Visual highlighting.
    إذا (آخري_بدليو != 0)
	بدليو_سٽون(پهريون_بدليو, 0,
		oap->is_VIsual ? شروعات_lnum + oap->سٽ_ڳڻپ :
		آخري_بدليو + 1, 0L);
    وإلا إذا (oap->is_VIsual)
	redraw_curbuf_later(UPD_INVERTED);

    إذا (oap->سٽ_ڳڻپ > p_report)
    {
	i = oap->سٽ_ڳڻپ - (i + 1);
	smsg(NGETTEXT("%ld line indented ",
						 "%ld lines indented ", i), i);
    }
    إذا ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// set '[ and '] marks
	curbuf->b_op_شروعات = oap->شروعات;
	curbuf->b_op_پڇاڙي = oap->پڇاڙي;
    }
}

/*
 * Return TRUE if lines starting with '#' should be left aligned.
 */
    صحيح
preprocs_کاٻو(فراغ)
{
    أرجع
	(curbuf->b_p_si && !curbuf->b_p_cin) ||
	(curbuf->b_p_cin && اندريون_cinkeys('#', ' ', TRUE)
					   && curbuf->b_ind_hash_comment == 0)
	;
}

/*
 * Return TRUE if the conditions are OK for smart indenting.
 */
    صحيح
may_do_si(فراغ)
{
    أرجع curbuf->b_p_si
	&& !curbuf->b_p_cin
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	&& *curbuf->b_p_inde == NUL
#جيڪڏهن_پورو
	&& !p_paste;
}

/*
 * Try to do some very smart auto-indenting.
 * Used when inserting a "normal" character.
 */
    فراغ
ins_try_si(صحيح c)
{
    pos_T	*pos, old_pos;
    char_u	*ptr;
    صحيح		i;
    صحيح		عارضي;

    // do some very smart indenting when entering '{' or '}'
    إذا (((did_si || can_si_پٺ) && c == '{')
	    || (can_si && c == '}' && inindent(0)))
    {
	// for '}' set indent equal to indent of line containing matching '{'
	إذا (c == '}' && (pos = findmatch(عدم, '{')) != عدم)
	{
	    old_pos = curwin->w_cursor;
	    // If the matching '{' has a ')' immediately before it (ignoring
	    // white-space), then line up with the start of the line
	    // containing the matching '(' if there is one.  This handles the
	    // case where an "if (..\n..) {" statement continues over multiple
	    // lines -- webb
	    ptr = ml_get(pos->lnum);
	    i = pos->کانو;
	    إذا (i > 0)		// skip blanks before '{'
		طالما (--i > 0 && VIM_ISWHITE(ptr[i]))
		    ;
	    curwin->w_cursor.lnum = pos->lnum;
	    curwin->w_cursor.کانو = i;
	    إذا (ptr[i] == ')' && (pos = findmatch(عدم, '(')) != عدم)
		curwin->w_cursor = *pos;
	    i = get_indent();
	    curwin->w_cursor = old_pos;
	    إذا (State & VREPLACE_FLAG)
		change_indent(INDENT_سيٽ, i, FALSE, NUL, TRUE);
	    وإلا
		(فراغ)سيٽ_indent(i, SIN_بدليو);
	}
	وإلا إذا (curwin->w_cursor.کانو > 0)
	{
	    // when inserting '{' after "O" reduce indent, but not
	    // more than indent of previous line
	    عارضي = TRUE;
	    إذا (c == '{' && can_si_پٺ && curwin->w_cursor.lnum > 1)
	    {
		old_pos = curwin->w_cursor;
		i = get_indent();
		طالما (curwin->w_cursor.lnum > 1)
		{
		    ptr = skipwhite(ml_get(--(curwin->w_cursor.lnum)));

		    // ignore empty lines and lines starting with '#'.
		    إذا (*ptr != '#' && *ptr != NUL)
			اكسر;
		}
		إذا (get_indent() >= i)
		    عارضي = FALSE;
		curwin->w_cursor = old_pos;
	    }
	    إذا (عارضي)
		سرڪايو_سٽ(TRUE, FALSE, 1, TRUE);
	}
    }

    // set indent of '#' always to 0
    إذا (curwin->w_cursor.کانو > 0 && can_si && c == '#' && inindent(0))
    {
	// remember current indent for next line
	old_indent = get_indent();
	(فراغ)سيٽ_indent(0, SIN_بدليو);
    }

    // Adjust ai_col, the char at this position can be deleted.
    إذا (ai_کانو > curwin->w_cursor.کانو)
	ai_کانو = curwin->w_cursor.کانو;
}

/*
 * Insert an indent (for <Tab> or CTRL-T) or delete an indent (for CTRL-D).
 * Keep the cursor on the same character.
 * type == INDENT_INC	increase indent (for CTRL-T or <Tab>)
 * type == INDENT_DEC	decrease indent (for CTRL-D)
 * type == INDENT_SET	set indent to "amount"
 * if round is TRUE, round the indent to 'shiftwidth' (only with _INC and _Dec).
 */
    فراغ
change_indent(
    صحيح		type,
    صحيح		amount,
    صحيح		ڦيرو,
    صحيح		replaced,	// replaced character, put on replace stack
    صحيح		call_بدليو_بائيٽون)	// call changed_bytes()
{
    صحيح		vcol;
    صحيح		آخري_vcol;
    صحيح		insstart_less;		// reduction for Insstart.col
    صحيح		new_cursor_کانو;
    صحيح		i;
    char_u	*ptr;
    صحيح		save_p_list;
    صحيح		شروعات_کانو;
    colnr_T	vc;
    colnr_T	orig_کانو = 0;		// init for GCC
    char_u	*new_سٽ, *orig_سٽ = عدم;	// init for GCC

    // MODE_VREPLACE state needs to know what the line was like before changing
    إذا (State & VREPLACE_FLAG)
    {
	orig_سٽ = vim_strnsave(ml_get_curline(), ml_get_curline_len());  // Deal with NULL below
	orig_کانو = curwin->w_cursor.کانو;
    }

    // for the following tricks we don't want list mode
    save_p_list = curwin->w_p_list;
    curwin->w_p_list = FALSE;
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    ignore_متن_props = TRUE;
#جيڪڏهن_پورو
    vc = getvcol_nolist(&curwin->w_cursor);
    vcol = vc;

    // For Replace mode we need to fix the replace stack later, which is only
    // possible when the cursor is in the indent.  Remember the number of
    // characters before the cursor if it's possible.
    شروعات_کانو = curwin->w_cursor.کانو;

    // determine offset from first non-blank
    new_cursor_کانو = curwin->w_cursor.کانو;
    beginline(BL_WHITE);
    new_cursor_کانو -= curwin->w_cursor.کانو;

    insstart_less = curwin->w_cursor.کانو;

    // If the cursor is in the indent, compute how many screen columns the
    // cursor is to the left of the first non-blank.
    إذا (new_cursor_کانو < 0)
	vcol = get_indent() - vcol;

    إذا (new_cursor_کانو > 0)	    // can't fix replace stack
	شروعات_کانو = -1;

    // Set the new indent.  The cursor will be put on the first non-blank.
    إذا (type == INDENT_سيٽ)
	(فراغ)سيٽ_indent(amount, call_بدليو_بائيٽون ? SIN_بدليو : 0);
    وإلا
    {
	صحيح	save_State = State;

	// Avoid being called recursively.
	إذا (State & VREPLACE_FLAG)
	    State = گهڻائي_وجهو;
	سرڪايو_سٽ(type == INDENT_DEC, ڦيرو, 1, call_بدليو_بائيٽون);
	State = save_State;
    }
    insstart_less -= curwin->w_cursor.کانو;

    // Try to put cursor on same character.
    // If the cursor is at or after the first non-blank in the line,
    // compute the cursor column relative to the column of the first
    // non-blank character.
    // If we are not in insert mode, leave the cursor on the first non-blank.
    // If the cursor is before the first non-blank, position it relative
    // to the first non-blank, counted in screen columns.
    إذا (new_cursor_کانو >= 0)
    {
	// When changing the indent while the cursor is touching it, reset
	// Insstart_col to 0.
	إذا (new_cursor_کانو == 0)
	    insstart_less = MAXCOL;
	new_cursor_کانو += curwin->w_cursor.کانو;
    }
    وإلا إذا (!(State & گهڻائي_وجهو))
	new_cursor_کانو = curwin->w_cursor.کانو;
    وإلا
    {
	chartabsize_T cts;

	// Compute the screen column where the cursor should be.
	vcol = get_indent() - vcol;
	curwin->w_virtcol = (colnr_T)((vcol < 0) ? 0 : vcol);

	// Advance the cursor until we reach the right screen column.
	آخري_vcol = 0;
	ptr = ml_get_curline();
	init_chartabsize_arg(&cts, curwin, 0, 0, ptr, ptr);
	طالما (cts.cts_vcol <= (صحيح)curwin->w_virtcol)
	{
	    آخري_vcol = cts.cts_vcol;
	    إذا (cts.cts_vcol > 0)
		MB_PTR_ADV(cts.cts_ptr);
	    إذا (*cts.cts_ptr == NUL)
		اكسر;
	    cts.cts_vcol += lbr_chartabsize(&cts);
	}
	vcol = آخري_vcol;
	new_cursor_کانو = cts.cts_ptr - cts.cts_سٽ;
	clear_chartabsize_arg(&cts);

	// May need to insert spaces to be able to position the cursor on
	// the right screen column.
	إذا (vcol != (صحيح)curwin->w_virtcol)
	{
	    curwin->w_cursor.کانو = (colnr_T)new_cursor_کانو;
	    i = (صحيح)curwin->w_virtcol - vcol;
	    ptr = alloc(i + 1);
	    إذا (ptr != عدم)
	    {
		size_t ptrlen;
		new_cursor_کانو += i;
		ptr[i] = NUL;
		ptrlen = i;
		طالما (--i >= 0)
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

    إذا (new_cursor_کانو <= 0)
	curwin->w_cursor.کانو = 0;
    وإلا
	curwin->w_cursor.کانو = (colnr_T)new_cursor_کانو;
    curwin->w_سيٽ_curswant = true;
    بدليو_cline_bef_curs();

    // May have to adjust the start of the insert.
    إذا (State & گهڻائي_وجهو)
    {
	إذا (curwin->w_cursor.lnum == Insstart.lnum && Insstart.کانو != 0)
	{
	    إذا ((صحيح)Insstart.کانو <= insstart_less)
		Insstart.کانو = 0;
	    وإلا
		Insstart.کانو -= insstart_less;
	}
	إذا ((صحيح)ai_کانو <= insstart_less)
	    ai_کانو = 0;
	وإلا
	    ai_کانو -= insstart_less;
    }

    // For MODE_REPLACE state, may have to fix the replace stack, if it's
    // possible.  If the number of characters before the cursor decreased, need
    // to pop a few characters from the replace stack.
    // If the number of characters before the cursor increased, need to push a
    // few NULs onto the replace stack.
    إذا (REPLACE_عام(State) && شروعات_کانو >= 0)
    {
	طالما (شروعات_کانو > (صحيح)curwin->w_cursor.کانو)
	{
	    replace_join(0);	    // remove a NUL from the replace stack
	    --شروعات_کانو;
	}
	طالما (شروعات_کانو < (صحيح)curwin->w_cursor.کانو || replaced)
	{
	    replace_ڌڪو(NUL);
	    إذا (replaced)
	    {
		replace_ڌڪو(replaced);
		replaced = NUL;
	    }
	    ++شروعات_کانو;
	}
    }
#جيڪڏهن_وصف_ٿيل FEAT_PROP_POPUP
    ignore_متن_props = FALSE;
#جيڪڏهن_پورو

    // For MODE_VREPLACE state, we also have to fix the replace stack.  In this
    // case it is always possible because we backspace over the whole line and
    // then put it back again the way we wanted it.
    إذا (State & VREPLACE_FLAG)
    {
	// If orig_line didn't allocate, just return.  At least we did the job,
	// even if you can't backspace.
	إذا (orig_سٽ == عدم)
	    أرجع;

	// Save new line
	new_سٽ = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	إذا (new_سٽ == عدم)
	{
	    vim_free(orig_سٽ);
	    أرجع;
	}

	// We only put back the new line up to the cursor
	new_سٽ[curwin->w_cursor.کانو] = NUL;

	// Put back original line
	ml_replace(curwin->w_cursor.lnum, orig_سٽ, FALSE);
	curwin->w_cursor.کانو = orig_کانو;

	// Backspace from cursor to start of line
	backspace_until_ڪالم(0);

	// Insert new stuff into line again
	ins_بائيٽون(new_سٽ);

	vim_free(new_سٽ);
    }
}

/*
 * Copy the indent from ptr to the current line (and fill to size)
 * Leaves the cursor on the first non-blank in the line.
 * Returns TRUE if the line was changed.
 */
    صحيح
copy_indent(صحيح سائيز, char_u *src)
{
    char_u	*p = عدم;
    char_u	*سٽ = عدم;
    char_u	*s;
    صحيح		todo;
    صحيح		ind_len;
    صحيح		سٽ_len = 0;
    صحيح		tab_pad;
    صحيح		ind_پورو;
    صحيح		ڦيرو;
#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
    صحيح		ind_کانو;
#جيڪڏهن_پورو

    // Round 1: compute the number of characters needed for the indent
    // Round 2: copy the characters.
    لكل (ڦيرو = 1; ڦيرو <= 2; ++ڦيرو)
    {
	todo = سائيز;
	ind_len = 0;
	ind_پورو = 0;
#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
	ind_کانو = 0;
#جيڪڏهن_پورو
	s = src;

	// Count/copy the usable portion of the source line
	طالما (todo > 0 && VIM_ISWHITE(*s))
	{
	    إذا (*s == TAB)
	    {
#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
		tab_pad = tabstop_padding(ind_پورو, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#وإلا
		tab_pad = (صحيح)curbuf->b_p_ts
					   - (ind_پورو % (صحيح)curbuf->b_p_ts);
#جيڪڏهن_پورو
		// Stop if this tab will overshoot the target
		إذا (todo < tab_pad)
		    اكسر;
		todo -= tab_pad;
		ind_پورو += tab_pad;
#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
		ind_کانو += tab_pad;
#جيڪڏهن_پورو
	    }
	    وإلا
	    {
		--todo;
		++ind_پورو;
#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
		++ind_کانو;
#جيڪڏهن_پورو
	    }
	    ++ind_len;
	    إذا (p != عدم)
		*p++ = *s;
	    ++s;
	}

	// Fill to next tabstop with a tab, if possible
#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
	tab_pad = tabstop_padding(ind_پورو, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
#وإلا
	tab_pad = (صحيح)curbuf->b_p_ts - (ind_پورو % (صحيح)curbuf->b_p_ts);
#جيڪڏهن_پورو
	إذا (todo >= tab_pad && !curbuf->b_p_et)
	{
	    todo -= tab_pad;
	    ++ind_len;
#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
	    ind_کانو += tab_pad;
#جيڪڏهن_پورو
	    إذا (p != عدم)
		*p++ = TAB;
	}

	// Add tabs required for indent
	إذا (!curbuf->b_p_et)
	{
#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
	    لكل (;;)
	    {
		tab_pad = tabstop_padding(ind_کانو, curbuf->b_p_ts,
							curbuf->b_p_vts_array);
		إذا (todo < tab_pad)
		    اكسر;
		todo -= tab_pad;
		++ind_len;
		ind_کانو += tab_pad;
		إذا (p != عدم)
		    *p++ = TAB;
	    }
#وإلا
	    طالما (todo >= (صحيح)curbuf->b_p_ts)
	    {
		todo -= (صحيح)curbuf->b_p_ts;
		++ind_len;
		إذا (p != عدم)
		    *p++ = TAB;
	    }
#جيڪڏهن_پورو
	}

	// Count/add spaces required for indent
	طالما (todo > 0)
	{
	    --todo;
	    ++ind_len;
	    إذا (p != عدم)
		*p++ = ' ';
	}

	إذا (p == عدم)
	{
	    // Allocate memory for the result: the copied indent, new indent
	    // and the rest of the line.
	    سٽ_len = ml_get_curline_len() + 1;
	    سٽ = alloc(ind_len + سٽ_len);
	    إذا (سٽ == عدم)
		أرجع FALSE;
	    p = سٽ;
	}
    }

    // Append the original line
    mch_memmove(p, ml_get_curline(), (size_t)سٽ_len);

    // Replace the line
    ml_replace(curwin->w_cursor.lnum, سٽ, FALSE);

    // Put the cursor after the indent.
    curwin->w_cursor.کانو = ind_len;
    أرجع TRUE;
}

/*
 * Give a "resulting text too long" error and maybe set got_int.
 */
    ساكن فراغ
emsg_متن_too_long(فراغ)
{
    emsg(_(e_resulting_متن_too_long));
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    // when not inside a try/catch set got_int to break out of any loop
    إذا (trylevel == 0)
#جيڪڏهن_پورو
	got_int = TRUE;
}

/*
 * ":retab".
 */
    فراغ
ex_retab(exarg_T *eap)
{
    linenr_T	lnum;
    صحيح		got_tab = FALSE;
    ڊگهو	انگ_spaces = 0;
    ڊگهو	انگ_tabs;
    ڊگهو	len;
    ڊگهو	کانو;
    ڊگهو	vcol;
    ڊگهو	شروعات_کانو = 0;		// For start of white-space string
    ڊگهو	شروعات_vcol = 0;		// For start of white-space string
    ڊگهو	old_len;
    ڊگهو	new_len;
    char_u	*ptr;
    char_u	*new_سٽ = (char_u *)1; // init to non-NULL
    صحيح		did_undo;		// called u_save for current line
#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
    صحيح		*new_vts_array = عدم;
    char_u	*new_ts_str;		// string value of tab argument
#وإلا
    صحيح		عارضي;
    صحيح		new_ts = 0;
#جيڪڏهن_پورو
    صحيح		save_list;
    linenr_T	پهريون_سٽ = 0;		// first changed line
    linenr_T	آخري_سٽ = 0;		// last changed line
    صحيح		is_indent_only = 0;	// Only process leading whitespace

    save_list = curwin->w_p_list;
    curwin->w_p_list = 0;	    // don't want list mode here

    ptr = eap->arg;
    إذا (STRNCMP(ptr, "-indentonly", 11) == 0 && IS_WHITE_OR_NUL(ptr[11]))
    {
	is_indent_only = 1;
	ptr = skipwhite(ptr + 11);
    }

#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
    new_ts_str = ptr;
    إذا (tabstop_سيٽ(ptr, &new_vts_array) == FAIL)
	أرجع;
    طالما (vim_isdigit(*ptr) || *ptr == ',')
	++ptr;

    // This ensures that either new_vts_array and new_ts_str are freshly
    // allocated, or new_vts_array points to an existing array and new_ts_str
    // is null.
    إذا (new_vts_array == عدم)
    {
	new_vts_array = curbuf->b_p_vts_array;
	new_ts_str = عدم;
    }
    وإلا
	new_ts_str = vim_strnsave(new_ts_str, ptr - new_ts_str);
#وإلا
    إذا (ptr[0] != NUL && (ptr[0] != '0' || ptr[1] != NUL))
    {
	char_u	*پڇاڙي;

	إذا (strtol((حرف *)ptr, (حرف **)&پڇاڙي, 10) <= 0)
	{
	    إذا (ptr != پڇاڙي)
		emsg(_(e_دليل_must_be_مثبت));
	    وإلا
		semsg(_(e_invalid_دليل_str), ptr);
	    أرجع;
	}
	new_ts = getdigits(&ptr);
	إذا (new_ts < 0 || new_ts > TABSTOP_MAX)
	{
	    semsg(_(e_invalid_دليل_str), eap->arg);
	    أرجع;
	}
    }
    إذا (new_ts == 0)
	new_ts = curbuf->b_p_ts;
#جيڪڏهن_پورو
    لكل (lnum = eap->line1; !got_int && lnum <= eap->line2; ++lnum)
    {
	ptr = ml_get(lnum);
	old_len = ml_get_len(lnum);
	کانو = 0;
	vcol = 0;
	did_undo = FALSE;
	لكل (;;)
	{
	    إذا (VIM_ISWHITE(ptr[کانو]))
	    {
		إذا (!got_tab && انگ_spaces == 0)
		{
		    // First consecutive white-space
		    شروعات_vcol = vcol;
		    شروعات_کانو = کانو;
		}
		إذا (ptr[کانو] == ' ')
		    انگ_spaces++;
		وإلا
		    got_tab = TRUE;
	    }
	    وإلا
	    {
		إذا (got_tab || (eap->forceit && انگ_spaces > 1))
		{
		    // Retabulate this string of white-space

		    // len is virtual length of white string
		    len = انگ_spaces = vcol - شروعات_vcol;
		    انگ_tabs = 0;
		    إذا (!curbuf->b_p_et)
		    {
#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
			صحيح t, s;

			tabstop_fromto(شروعات_vcol, vcol,
					curbuf->b_p_ts, new_vts_array, &t, &s);
			انگ_tabs = t;
			انگ_spaces = s;
#وإلا
			عارضي = new_ts - (شروعات_vcol % new_ts);
			إذا (انگ_spaces >= عارضي)
			{
			    انگ_spaces -= عارضي;
			    انگ_tabs++;
			}
			انگ_tabs += انگ_spaces / new_ts;
			انگ_spaces -= (انگ_spaces / new_ts) * new_ts;
#جيڪڏهن_پورو
		    }
		    إذا (curbuf->b_p_et || got_tab ||
					(انگ_spaces + انگ_tabs < len))
		    {
			إذا (did_undo == FALSE)
			{
			    did_undo = TRUE;
			    إذا (u_save((linenr_T)(lnum - 1),
						(linenr_T)(lnum + 1)) == FAIL)
			    {
				new_سٽ = عدم;	// flag out-of-memory
				اكسر;
			    }
			}

			// len is actual number of white characters used
			len = انگ_spaces + انگ_tabs;
			new_len = old_len - کانو + شروعات_کانو + len + 1;
			إذا (new_len <= 0 || new_len >= MAXCOL)
			{
			    emsg_متن_too_long();
			    اكسر;
			}
			new_سٽ = alloc(new_len);
			إذا (new_سٽ == عدم)
			    اكسر;
			إذا (شروعات_کانو > 0)
			    mch_memmove(new_سٽ, ptr, (size_t)شروعات_کانو);
			mch_memmove(new_سٽ + شروعات_کانو + len,
				      ptr + کانو, (size_t)(old_len - کانو + 1));
			ptr = new_سٽ + شروعات_کانو;
			لكل (کانو = 0; کانو < len; کانو++)
			    ptr[کانو] = (کانو < انگ_tabs) ? '\t' : ' ';
			إذا (ml_replace(lnum, new_سٽ, FALSE) == OK)
			    // "new_line" may have been copied
			    new_سٽ = curbuf->b_ml.ml_سٽ_ptr;
			إذا (پهريون_سٽ == 0)
			    پهريون_سٽ = lnum;
			آخري_سٽ = lnum;
			ptr = new_سٽ;
			old_len = new_len - 1;
			کانو = شروعات_کانو + len;
		    }
		}
		got_tab = FALSE;
		انگ_spaces = 0;

		إذا (is_indent_only)
		    اكسر;
	    }
	    إذا (ptr[کانو] == NUL)
		اكسر;
	    vcol += chartabsize(ptr + کانو, (colnr_T)vcol);
	    إذا (vcol >= MAXCOL)
	    {
		emsg_متن_too_long();
		اكسر;
	    }
	    إذا (has_mbyte)
		کانو += (*mb_ptr2len)(ptr + کانو);
	    وإلا
		++کانو;
	}
	إذا (new_سٽ == عدم)		    // out of memory
	    اكسر;
	سٽ_breakcheck();
    }
    إذا (got_int)
	emsg(_(e_interrupted));

#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
    // If a single value was given then it can be considered equal to
    // either the value of 'tabstop' or the value of 'vartabstop'.
    إذا (tabstop_ڳڻپ(curbuf->b_p_vts_array) == 0
	&& tabstop_ڳڻپ(new_vts_array) == 1
	&& curbuf->b_p_ts == tabstop_پهريون(new_vts_array))
	; // not changed
    وإلا إذا (tabstop_ڳڻپ(curbuf->b_p_vts_array) > 0
	&& tabstop_eq(curbuf->b_p_vts_array, new_vts_array))
	; // not changed
    وإلا
	redraw_curbuf_later(UPD_NOT_VALID);
#وإلا
    إذا (curbuf->b_p_ts != new_ts)
	redraw_curbuf_later(UPD_NOT_VALID);
#جيڪڏهن_پورو
    إذا (پهريون_سٽ != 0)
	بدليو_سٽون(پهريون_سٽ, 0, آخري_سٽ + 1, 0L);

    curwin->w_p_list = save_list;	// restore 'list'

#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
    إذا (new_ts_str != عدم)		// set the new tabstop
    {
	// If 'vartabstop' is in use or if the value given to retab has more
	// than one tabstop then update 'vartabstop'.
	صحيح *old_vts_ary = curbuf->b_p_vts_array;

	إذا (tabstop_ڳڻپ(old_vts_ary) > 0 || tabstop_ڳڻپ(new_vts_array) > 1)
	{
	    سيٽ_string_option_سڌو((char_u *)"vts", -1, new_ts_str,
							OPT_FREE|OPT_LOCAL, 0);
	    curbuf->b_p_vts_array = new_vts_array;
	    vim_free(old_vts_ary);
	}
	وإلا
	{
	    // 'vartabstop' wasn't in use and a single value was given to
	    // retab then update 'tabstop'.
	    curbuf->b_p_ts = tabstop_پهريون(new_vts_array);
	    vim_free(new_vts_array);
	}
	vim_free(new_ts_str);
    }
#وإلا
    curbuf->b_p_ts = new_ts;
#جيڪڏهن_پورو
    coladvance(curwin->w_curswant);

    u_clearline();
}

#إذا defined(FEAT_EVAL)
/*
 * Get indent level from 'indentexpr'.
 */
    صحيح
get_اظهار_indent(فراغ)
{
    صحيح		indent = -1;
    char_u	*inde_copy;
    pos_T	save_pos;
    colnr_T	save_curswant;
    صحيح		save_سيٽ_curswant;
    صحيح		save_State;
    صحيح		use_sandbox = was_سيٽ_insecurely(curwin,
					    (char_u *)"indentexpr", OPT_LOCAL);
    sctx_T	save_sctx = current_sctx;

    // Save and restore cursor position and curswant, in case it was changed
    // via :normal commands
    save_pos = curwin->w_cursor;
    save_curswant = curwin->w_curswant;
    save_سيٽ_curswant = curwin->w_سيٽ_curswant;
    سيٽ_vim_var_nr(VV_LNUM, curwin->w_cursor.lnum);
    إذا (use_sandbox)
	++sandbox;
    ++textlock;
    current_sctx = curbuf->b_p_script_ctx[BV_INDE];

    // Need to make a copy, the 'indentexpr' option could be changed while
    // evaluating it.
    inde_copy = vim_strsave(curbuf->b_p_inde);
    إذا (inde_copy != عدم)
    {
	indent = (صحيح)eval_منزل_عدد(inde_copy, TRUE);
	vim_free(inde_copy);
    }

    إذا (use_sandbox)
	--sandbox;
    --textlock;
    current_sctx = save_sctx;

    // Restore the cursor position so that 'indentexpr' doesn't need to.
    // Pretend to be in Insert mode, allow cursor past end of line for "o"
    // command.
    save_State = State;
    State = گهڻائي_وجهو;
    curwin->w_cursor = save_pos;
    curwin->w_curswant = save_curswant;
    curwin->w_سيٽ_curswant = save_سيٽ_curswant;
    check_cursor();
    State = save_State;

    // Reset did_throw, unless 'debug' has "throw" and inside a try/catch.
    إذا (did_throw && (vim_strchr(p_debug, 't') == عدم || trylevel == 0))
    {
	handle_did_throw();
	did_throw = FALSE;
    }

    // If there is an error, just keep the current indent.
    إذا (indent < 0)
	indent = get_indent();

    أرجع indent;
}
#جيڪڏهن_پورو

    ساكن صحيح
lisp_ميلاپ(char_u *p)
{
    char_u	buf[LSIZE];
    صحيح		len;
    char_u	*لفظ = *curbuf->b_p_lw != NUL ? curbuf->b_p_lw : p_lispwords;

    طالما (*لفظ != NUL)
    {
	len = copy_option_part(&لفظ, buf, LSIZE, ",");
	إذا (STRNCMP(buf, p, len) == 0 && IS_WHITE_OR_NUL(p[len]))
	    أرجع TRUE;
    }
    أرجع FALSE;
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
    صحيح
get_lisp_indent(فراغ)
{
    pos_T	*pos, realpos, paren;
    صحيح		amount;
    char_u	*that;
    colnr_T	کانو;
    colnr_T	firsttry;
    صحيح		parencount, quotecount;
    صحيح		vi_lisp;

    // Set vi_lisp to use the vi-compatible method
    vi_lisp = (vim_strchr(p_cpo, CPO_LISP) != عدم);

    realpos = curwin->w_cursor;
    curwin->w_cursor.کانو = 0;

    إذا ((pos = findmatch(عدم, '(')) == عدم)
	pos = findmatch(عدم, '[');
    وإلا
    {
	paren = *pos;
	pos = findmatch(عدم, '[');
	إذا (pos == عدم || LT_POSP(pos, &paren))
	    pos = &paren;
    }
    إذا (pos != عدم)
    {
	// Extra trick: Take the indent of the first previous non-white
	// line that is at the same () level.
	amount = -1;
	parencount = 0;

	طالما (--curwin->w_cursor.lnum >= pos->lnum)
	{
	    إذا (linewhite(curwin->w_cursor.lnum))
		استمر;
	    لكل (that = ml_get_curline(); *that != NUL; ++that)
	    {
		إذا (*that == ';')
		{
		    طالما (*(that + 1) != NUL)
			++that;
		    استمر;
		}
		إذا (*that == '\\')
		{
		    إذا (*(that + 1) != NUL)
			++that;
		    استمر;
		}
		إذا (*that == '"' && *(that + 1) != NUL)
		{
		    طالما (*++that && *that != '"')
		    {
			// skipping escaped characters in the string
			إذا (*that == '\\')
			{
			    إذا (*++that == NUL)
				اكسر;
			    إذا (that[1] == NUL)
			    {
				++that;
				اكسر;
			    }
			}
		    }
		    إذا (*that == NUL)
			اكسر;
		}
		إذا (*that == '(' || *that == '[')
		    ++parencount;
		وإلا إذا (*that == ')' || *that == ']')
		    --parencount;
	    }
	    إذا (parencount == 0)
	    {
		amount = get_indent();
		اكسر;
	    }
	}

	إذا (amount == -1)
	{
	    curwin->w_cursor.lnum = pos->lnum;
	    curwin->w_cursor.کانو = pos->کانو;
	    کانو = pos->کانو;

	    that = ml_get_curline();

	    إذا (vi_lisp && get_indent() == 0)
		amount = 2;
	    وإلا
	    {
		char_u		*سٽ = that;
		chartabsize_T	cts;

		init_chartabsize_arg(&cts, curwin, pos->lnum, 0, سٽ, سٽ);
		طالما (*cts.cts_ptr != NUL && کانو > 0)
		{
		    cts.cts_vcol += lbr_chartabsize_adv(&cts);
		    کانو--;
		}
		amount = cts.cts_vcol;
		that = cts.cts_ptr;
		clear_chartabsize_arg(&cts);

		// Some keywords require "body" indenting rules (the
		// non-standard-lisp ones are Scheme special forms):
		//
		// (let ((a 1))    instead    (let ((a 1))
		//   (...))	      of	   (...))

		إذا (!vi_lisp && (*that == '(' || *that == '[')
						      && lisp_ميلاپ(that + 1))
		    amount += 2;
		وإلا
		{
		    إذا (*that != NUL)
		    {
			that++;
			amount++;
		    }
		    firsttry = amount;

		    init_chartabsize_arg(&cts, curwin, (colnr_T)(that - سٽ),
							   amount, سٽ, that);
		    طالما (VIM_ISWHITE(*cts.cts_ptr))
		    {
			cts.cts_vcol += lbr_chartabsize(&cts);
			++cts.cts_ptr;
		    }
		    that = cts.cts_ptr;
		    amount = cts.cts_vcol;
		    clear_chartabsize_arg(&cts);

		    إذا (*that && *that != ';') // not a comment line
		    {
			// test *that != '(' to accommodate first let/do
			// argument if it is more than one line
			إذا (!vi_lisp && *that != '(' && *that != '[')
			    firsttry++;

			parencount = 0;
			quotecount = 0;

			init_chartabsize_arg(&cts, curwin,
				   (colnr_T)(that - سٽ), amount, سٽ, that);
			إذا (vi_lisp
				|| (*that != '"'
				    && *that != '\''
				    && *that != '#'
				    && (*that < '0' || *that > '9')))
			{
			    طالما (*cts.cts_ptr
				    && (!VIM_ISWHITE(*cts.cts_ptr)
					|| quotecount
					|| parencount)
				    && (!((*cts.cts_ptr == '('
							|| *cts.cts_ptr == '[')
					    && !quotecount
					    && !parencount
					    && vi_lisp)))
			    {
				إذا (*cts.cts_ptr == '"')
				    quotecount = !quotecount;
				إذا ((*cts.cts_ptr == '(' || *cts.cts_ptr == '[')
							       && !quotecount)
				    ++parencount;
				إذا ((*cts.cts_ptr == ')' || *cts.cts_ptr == ']')
							       && !quotecount)
				    --parencount;
				إذا (*cts.cts_ptr == '\\'
						    && *(cts.cts_ptr+1) != NUL)
				    cts.cts_vcol += lbr_chartabsize_adv(&cts);
				cts.cts_vcol += lbr_chartabsize_adv(&cts);
			    }
			}
			طالما (VIM_ISWHITE(*cts.cts_ptr))
			{
			    cts.cts_vcol += lbr_chartabsize(&cts);
			    ++cts.cts_ptr;
			}
			that = cts.cts_ptr;
			amount = cts.cts_vcol;
			clear_chartabsize_arg(&cts);

			إذا (!*that || *that == ';')
			    amount = firsttry;
		    }
		}
	    }
	}
    }
    وإلا
	amount = 0;	// no matching '(' or '[' found, use zero indent

    curwin->w_cursor = realpos;

    أرجع amount;
}

/*
 * Re-indent the current line, based on the current contents of it and the
 * surrounding lines. Fixing the cursor position seems really easy -- I'm very
 * confused what all the part that handles Control-T is doing that I'm not.
 * "get_the_indent" should be get_c_indent, get_expr_indent or get_lisp_indent.
 */

    فراغ
fixthisline(صحيح (*get_the_indent)(فراغ))
{
    صحيح amount = get_the_indent();

    إذا (amount < 0)
	أرجع;

    change_indent(INDENT_سيٽ, amount, FALSE, 0, TRUE);
    إذا (linewhite(curwin->w_cursor.lnum))
	did_ai = TRUE;	// delete the indent if the line stays empty
}

/*
 * Return TRUE if 'indentexpr' should be used for Lisp indenting.
 * Caller may want to check 'autoindent'.
 */
    صحيح
use_indentexpr_for_lisp(فراغ)
{
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    أرجع curbuf->b_p_lisp
		&& *curbuf->b_p_inde != NUL
		&& STRCMP(curbuf->b_p_lop, "expr:1") == 0;
#وإلا
    أرجع FALSE;
#جيڪڏهن_پورو
}

/*
 * Fix indent for 'lisp' and 'cindent'.
 */
    فراغ
fix_indent(فراغ)
{
    إذا (p_paste)
	أرجع;  // no auto-indenting when 'paste' is set
    إذا (curbuf->b_p_lisp && curbuf->b_p_ai)
    {
	إذا (use_indentexpr_for_lisp())
	    do_c_اظهار_indent();
	وإلا
	    fixthisline(get_lisp_indent);
    }
    وإلا إذا (cindent_on())
	do_c_اظهار_indent();
}

#إذا defined(FEAT_EVAL)
/*
 * "indent()" function
 */
    فراغ
f_indent(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;

    إذا (اندريون_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	أرجع;

    lnum = tv_get_lnum(argvars);
    إذا (lnum >= 1 && lnum <= curbuf->b_ml.ml_سٽ_ڳڻپ)
	rettv->vval.v_عدد = get_indent_lnum(lnum);
    وإلا
    {
	إذا (اندريون_vim9script())
	    semsg(_(e_invalid_سٽ_عدد_nr), lnum);
	rettv->vval.v_عدد = -1;
    }
}

/*
 * "lispindent(lnum)" function
 */
    فراغ
f_lispindent(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    pos_T	pos;
    linenr_T	lnum;

    إذا (اندريون_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	أرجع;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    إذا (lnum >= 1 && lnum <= curbuf->b_ml.ml_سٽ_ڳڻپ)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_عدد = get_lisp_indent();
	curwin->w_cursor = pos;
    }
    وإلا إذا (اندريون_vim9script())
	semsg(_(e_invalid_سٽ_عدد_nr), lnum);
    وإلا
	rettv->vval.v_عدد = -1;
}
#جيڪڏهن_پورو
