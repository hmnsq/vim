/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * tabpanel.c:
 */

#अन्तर्भाव "vim.h"

#जर defined(FEAT_TABPANEL)

स्थिर शून्य do_by_tplmode(पूर्णांक tplmode, पूर्णांक col_start, पूर्णांक col_end,
	पूर्णांक *pcurtab_row, पूर्णांक *ptabpagenr);
स्थिर शून्य tabpanel_free_click_regions(शून्य);
स्थिर शून्य tabpanel_append_click_regions(stl_clickrec_T *clicktab,
	char_u *buf, पूर्णांक row, पूर्णांक col_start, पूर्णांक col_end, पूर्णांक tabnr);
स्थिर शून्य draw_tabpanel_scrollbar(पूर्णांक screen_col);

// set pcurtab_row. don't redraw tabpanel.
#define TPLMODE_GET_CURTAB_ROW	0
// set ptabpagenr. don't redraw tabpanel.
#define TPLMODE_GET_TABPAGENR	1
// redraw tabpanel.
#define TPLMODE_REDRAW		2

#define TPL_FILLCHAR		' '

#define VERT_LEN		1
#define SCROLL_LEN		1

// tpl_align's values
#define ALIGN_डावा		0
#define ALIGN_उजवा		1

स्थिर char_u *opt_name = (char_u *)"tabpanel";
स्थिर पूर्णांक opt_scope = OPT_LOCAL;
स्थिर पूर्णांक tpl_align = ALIGN_डावा;
स्थिर पूर्णांक tpl_columns = 20;
स्थिर bool tpl_is_vert = false;
स्थिर bool tpl_scrollbar = false;
स्थिर पूर्णांक tpl_scroll_offset = 0;
स्थिर पूर्णांक tpl_total_rows = 0;
स्थिर पूर्णांक tpl_scrollbar_col = -1;  // screen column of scrollbar, -1 if none
स्थिर tabpage_T *tpl_last_curtab = रिक्त;   // last curtab seen by draw_tabpanel

प्रकारव्याख्या संरचना {
    win_T   *wp;
    win_T   *cwp;
    char_u  *user_defined;
    पूर्णांक	maxrow;
    पूर्णांक	offsetrow;
    पूर्णांक	*prow;
    पूर्णांक	*pcol;
    पूर्णांक	attr;
    पूर्णांक	col_start;
    पूर्णांक	col_end;
} tabpanel_T;

    पूर्णांक
tabpanelopt_changed(शून्य)
{
    char_u	*p;
    पूर्णांक		new_align = ALIGN_डावा;
    दीर्घ	new_columns = 20;
    bool	new_is_vert = false;
    bool	new_scrollbar = false;

    p = p_tplo;
    पर्यंत (*p != NUL)
    {
	जर (STRNCMP(p, "align:", 6) == 0)
	{
	    p += 6;
	    जर (STRNCMP(p, "left", 4) == 0)
	    {
		p += 4;
		new_align = ALIGN_डावा;
	    }
	    अन्यथा जर (STRNCMP(p, "right", 5) == 0)
	    {
		p += 5;
		new_align = ALIGN_उजवा;
	    }
	    अन्यथा
		परत FAIL;
	}
	अन्यथा जर (STRNCMP(p, "columns:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    new_columns = getdigits(&p);
	    जर (new_columns < 0 || new_columns > 1000)
		परत FAIL;
	}
	अन्यथा जर (STRNCMP(p, "vert", 4) == 0)
	{
	    p += 4;
	    new_is_vert = true;
	}
	अन्यथा जर (STRNCMP(p, "scrollbar", 9) == 0)
	{
	    p += 9;
	    new_scrollbar = true;
	}

	जर (*p != ',' && *p != NUL)
	    परत FAIL;
	जर (*p == ',')
	    ++p;
    }

    tpl_align = new_align;
    tpl_columns = new_columns;
    tpl_is_vert = new_is_vert;
    tpl_scrollbar = new_scrollbar;

    // Re-center the current tab on the next redraw.
    tpl_last_curtab = रिक्त;

    shell_new_columns();
    परत OK;
}

/*
 * Drop any internal reference to "tp", so draw_tabpanel() never compares
 * against a dangling pointer after the tabpage has been freed.
 */
    शून्य
tabpanel_forget_tabpage(स्थिरांक tabpage_T *tp)
{
    जर (tpl_last_curtab == tp)
	tpl_last_curtab = रिक्त;
}

/*
 * Return the width of tabpanel.
 */
    पूर्णांक
tabpanel_width(शून्य)
{
    निवड (p_stpl)
    {
	स्थिती 0:
	    परत 0;
	स्थिती 1:
	    जर (first_tabpage->tp_next == रिक्त)
		परत 0;
    }
    जर (Columns < tpl_columns)
	परत 0;
    अन्यथा
	परत tpl_columns;
}

/*
 * Return the offset of a window considering the width of tabpanel.
 */
    पूर्णांक
tabpanel_leftcol(शून्य)
{
    परत tpl_align == ALIGN_उजवा ? 0 : tabpanel_width();
}

/*
 * Free previously resolved 'tabpanel' click regions.
 */
    स्थिर शून्य
tabpanel_free_click_regions(शून्य)
{
    पूर्णांक n;

    जर (tabpanel_stl_click != रिक्त)
    {
	साठी (n = 0; n < tabpanel_stl_click_मोजणी; n++)
	    vim_free(tabpanel_stl_click[n].funcname);
	VIM_CLEAR(tabpanel_stl_click);
    }
    tabpanel_stl_click_मोजणी = 0;
}

/*
 * Convert click records produced by build_stl_str_hl() for one line of
 * 'tabpanel' into screen-column based regions and append them to the global
 * tabpanel_stl_click array.  The caller keeps ownership of the funcname
 * strings inside "clicktab" — this function makes its own copies.
 */
    स्थिर शून्य
tabpanel_append_click_regions(
	stl_clickrec_T	*clicktab,
	char_u		*buf,
	पूर्णांक		row,
	पूर्णांक		col_start,
	पूर्णांक		col_end,
	पूर्णांक		tabnr)
{
    पूर्णांक		मोजणी = 0;
    पूर्णांक		n;
    पूर्णांक		base_col;
    पूर्णांक		acc_width = 0;
    पूर्णांक		max_w = col_end - col_start;
    char_u	*p;
    char_u	*cur_funcname = रिक्त;
    पूर्णांक		cur_minwid = 0;
    पूर्णांक		region_start_col;
    stl_click_region_T *new_arr;
    पूर्णांक		limit;

    जर (clicktab == रिक्त)
	परत;

    साठी (n = 0; clicktab[n].start != रिक्त; n++)
	मोजणी++;
    जर (मोजणी == 0)
	परत;

    base_col = (tpl_align == ALIGN_उजवा ? topframe->fr_width : 0) + col_start;
    region_start_col = base_col;

    // Grow the global array to make room for up to "count" more regions
    // (one close for each record plus a possible trailing region).
    new_arr = vim_realloc(tabpanel_stl_click,
	    आकार(stl_click_region_T) * (tabpanel_stl_click_मोजणी + मोजणी + 1));
    जर (new_arr == रिक्त)
	परत;
    tabpanel_stl_click = new_arr;

    p = buf;
    साठी (n = 0; clicktab[n].start != रिक्त; n++)
    {
	acc_width += vim_strnsize(p, (पूर्णांक)(clicktab[n].start - p));
	p = clicktab[n].start;
	limit = acc_width < max_w ? acc_width : max_w;

	जर (cur_funcname != रिक्त)
	{
	    stl_click_region_T *r =
				&tabpanel_stl_click[tabpanel_stl_click_मोजणी];
	    r->row = row;
	    r->col_start = region_start_col;
	    r->col_end = base_col + limit;
	    r->funcname = vim_strsave(cur_funcname);
	    r->minwid = cur_minwid;
	    r->tabnr = tabnr;
	    tabpanel_stl_click_मोजणी++;
	}

	cur_funcname = clicktab[n].funcname;
	cur_minwid = clicktab[n].minwid;
	region_start_col = base_col + limit;
    }

    // Close the final region if it extends to the end.
    जर (cur_funcname != रिक्त)
    {
	stl_click_region_T *r = &tabpanel_stl_click[tabpanel_stl_click_मोजणी];
	r->row = row;
	r->col_start = region_start_col;
	r->col_end = base_col + max_w;
	r->funcname = vim_strsave(cur_funcname);
	r->minwid = cur_minwid;
	r->tabnr = tabnr;
	tabpanel_stl_click_मोजणी++;
    }
}

/*
 * Ensure the current tab is visible by adjusting tpl_scroll_offset when
 * the selected tab has changed since the previous redraw.  Mouse wheel or
 * scrollbar drag operations leave curtab unchanged, so the user's chosen
 * offset is preserved in those cases.
 */
    स्थिर शून्य
follow_curtab_if_needed(पूर्णांक curtab_row)
{
    जर (Rows <= 0 || curtab == tpl_last_curtab)
	परत;

    जर (curtab_row < tpl_scroll_offset)
	tpl_scroll_offset = curtab_row;
    अन्यथा जर (curtab_row >= tpl_scroll_offset + Rows)
	tpl_scroll_offset = curtab_row - Rows + 1;

    पूर्णांक max_offset = tpl_total_rows > Rows ? tpl_total_rows - Rows : 0;

    जर (tpl_scroll_offset < 0)
	tpl_scroll_offset = 0;
    अन्यथा जर (tpl_scroll_offset > max_offset)
	tpl_scroll_offset = max_offset;
}

/*
 * draw the tabpanel.
 */
    शून्य
draw_tabpanel(शून्य)
{
    पूर्णांक saved_किल्ली_Typed = किल्ली_Typed;
    पूर्णांक saved_got_int = got_int;
    पूर्णांक maxwidth = tabpanel_width();
    पूर्णांक vs_attr = HL_ATTR(HLF_C);
    पूर्णांक curtab_row = 0;
    पूर्णांक vsrow = 0;
    पूर्णांक is_उजवा = tpl_align == ALIGN_उजवा;

    जर (maxwidth == 0)
    {
	tabpanel_free_click_regions();
	परत;
    }

    // Discard old click regions — they'll be rebuilt during redraw below.
    tabpanel_free_click_regions();

    // Reset got_int to avoid build_stl_str_hl() isn't evaluated.
    got_int = FALSE;

    पूर्णांक sb_len = tpl_scrollbar ? SCROLL_LEN : 0;
    पूर्णांक sb_screen_col = -1;

    // The scrollbar is always placed at the right edge of the tabpanel,
    // regardless of 'align'.  The vertical separator sits at the panel's
    // boundary with the buffer area (left edge for align:right, right edge
    // for align:left).
    जर (tpl_is_vert)
    {
	जर (is_उजवा)
	{
	    // Panel on the right: vert at panel's left edge, scrollbar at
	    // panel's right edge (= screen's right edge).
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, VERT_LEN,
		    maxwidth - sb_len, &curtab_row, रिक्त);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, VERT_LEN, maxwidth - sb_len,
		    &curtab_row, रिक्त);
	    साठी (vsrow = 0; vsrow < Rows; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			topframe->fr_width, vs_attr);
	    जर (tpl_scrollbar)
		sb_screen_col = topframe->fr_width + maxwidth - SCROLL_LEN;
	}
	अन्यथा
	{
	    // Panel on the left: scrollbar just left of vert, vert at
	    // panel's right edge (boundary with buffer).
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0,
		    maxwidth - VERT_LEN - sb_len, &curtab_row, रिक्त);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - VERT_LEN - sb_len,
		    &curtab_row, रिक्त);
	    साठी (vsrow = 0; vsrow < Rows; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			maxwidth - VERT_LEN, vs_attr);
	    जर (tpl_scrollbar)
		sb_screen_col = maxwidth - VERT_LEN - SCROLL_LEN;
	}
    }
    अन्यथा
    {
	जर (is_उजवा)
	{
	    // Panel on the right, no vert: scrollbar at screen's right edge.
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth - sb_len,
		    &curtab_row, रिक्त);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_row, रिक्त);
	    जर (tpl_scrollbar)
		sb_screen_col = topframe->fr_width + maxwidth - SCROLL_LEN;
	}
	अन्यथा
	{
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth - sb_len,
		    &curtab_row, रिक्त);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_row, रिक्त);
	    जर (tpl_scrollbar)
		sb_screen_col = maxwidth - SCROLL_LEN;
	}
    }

    tpl_scrollbar_col = sb_screen_col;
    जर (sb_screen_col >= 0)
	draw_tabpanel_scrollbar(sb_screen_col);

    got_int |= saved_got_int;

    // A user function may reset KeyTyped, restore it.
    किल्ली_Typed = saved_किल्ली_Typed;

    tpl_last_curtab = curtab;
    redraw_tabpanel = FALSE;
}

/*
 * Return tabpagenr when clicking and dragging in tabpanel.
 */
    पूर्णांक
get_tabpagenr_on_tabpanel(शून्य)
{
    पूर्णांक		maxwidth = tabpanel_width();
    पूर्णांक		curtab_row = 0;
    पूर्णांक		tabpagenr = 0;

    जर (maxwidth == 0)
	परत -1;

    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth, &curtab_row, रिक्त);
    do_by_tplmode(TPLMODE_GET_TABPAGENR, 0, maxwidth, &curtab_row,
	    &tabpagenr);

    परत tabpagenr;
}

/*
 * Fill tailing area between {start_row} and {end_row - 1}.
 */
    स्थिर शून्य
screen_fill_tailing_area(
	पूर्णांक	tplmode,
	पूर्णांक	row_start,
	पूर्णांक	row_end,
	पूर्णांक	col_start,
	पूर्णांक	col_end,
	पूर्णांक	attr)
{
    पूर्णांक is_उजवा = tpl_align == ALIGN_उजवा;
    जर (tplmode == TPLMODE_REDRAW)
	screen_fill(row_start, row_end,
		(is_उजवा ? topframe->fr_width : 0) + col_start,
		(is_उजवा ? topframe->fr_width : 0) + col_end,
		TPL_FILLCHAR, TPL_FILLCHAR, attr);
}

/*
 * screen_puts_len() for tabpanel.
 */
    स्थिर शून्य
screen_puts_len_for_tabpanel(
	पूर्णांक	    tplmode,
	char_u	    *p,
	पूर्णांक	    len,
	पूर्णांक	    attr,
	tabpanel_T  *pargs)
{
    पूर्णांक		j;
    पूर्णांक		chlen;
    पूर्णांक		chcells;
    char_u	buf[IOSIZE];
    char_u	*temp;

    साठी (j = 0; j < len;)
    {
	जर (tplmode != TPLMODE_GET_CURTAB_ROW
		&& pargs->maxrow <= *pargs->prow - pargs->offsetrow)
	    थांबा;

	जर (has_mbyte)
	    chlen = (*mb_ptr2len)(p + j);
	अन्यथा
	    chlen = 1;

	साठी (पूर्णांक k = 0; k < chlen; k++)
	    buf[k] = p[j + k];
	buf[chlen] = NUL;
	j += chlen;

	// Make all characters printable.
	temp = transstr(buf);
	जर (temp != रिक्त)
	{
	    vim_strncpy(buf, temp, आकार(buf) - 1);
	    vim_free(temp);
	}

	जर (has_mbyte)
	    chcells = (*mb_ptr2cells)(buf);
	अन्यथा
	    chcells = 1;

	जर (pargs->col_end < (*pargs->pcol) + chcells)
	{
	    // fill the tailing area of current row.
	    जर (*pargs->prow - pargs->offsetrow >= 0
		    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
		screen_fill_tailing_area(tplmode,
			*pargs->prow - pargs->offsetrow,
			*pargs->prow - pargs->offsetrow + 1,
			*pargs->pcol, pargs->col_end, attr);
	    *pargs->pcol = pargs->col_end;

	    जर (pargs->col_end < chcells)
		थांबा;
	}

	जर (*pargs->pcol + chcells <= pargs->col_end)
	{
	    पूर्णांक off = (tpl_align == ALIGN_उजवा)
		    ? topframe->fr_width
		    : 0;
	    जर (tplmode == TPLMODE_REDRAW
		    && (*pargs->prow - pargs->offsetrow >= 0
		    && *pargs->prow - pargs->offsetrow < pargs->maxrow))
		screen_puts(buf, *pargs->prow - pargs->offsetrow,
			*pargs->pcol + off, attr);
	    *pargs->pcol += chcells;
	}
    }
}

/*
 * default tabpanel drawing behavior if 'tabpanel' option is empty.
 */
    स्थिर शून्य
draw_tabpanel_default(पूर्णांक tplmode, tabpanel_T *pargs)
{
    पूर्णांक		modified;
    पूर्णांक		wincount;
    पूर्णांक		len = 0;
    char_u	buf[2] = { NUL, NUL };

    modified = FALSE;
    साठी (wincount = 0; pargs->wp != रिक्त;
	    pargs->wp = pargs->wp->w_next, ++wincount)
	जर (bufIsChanged(pargs->wp->w_buffer))
	    modified = TRUE;

    जर (modified || wincount > 1)
    {
	जर (wincount > 1)
	{
	    vim_snprintf((अक्षर *)NameBuff, MAXPATHL, "%d", wincount);
	    len = (पूर्णांक)STRLEN(NameBuff);
	    screen_puts_len_for_tabpanel(tplmode, NameBuff, len,
#जर defined(FEAT_SYN_HL)
		    hl_combine_attr(pargs->attr, HL_ATTR(HLF_T)),
#अन्यथा
		    pargs->attr,
#endif
		    pargs);
	}
	जर (modified)
	{
	    buf[0] = '+';
	    screen_puts_len_for_tabpanel(tplmode, buf, 1, pargs->attr, pargs);
	}

	buf[0] = TPL_FILLCHAR;
	screen_puts_len_for_tabpanel(tplmode, buf, 1, pargs->attr, pargs);
    }

    get_trans_bufname(pargs->cwp->w_buffer);
    shorten_dir(NameBuff);
    len = (पूर्णांक)STRLEN(NameBuff);
    screen_puts_len_for_tabpanel(tplmode, NameBuff, len, pargs->attr, pargs);

    // fill the tailing area of current row.
    जर (*pargs->prow - pargs->offsetrow >= 0
	    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
	screen_fill_tailing_area(tplmode, *pargs->prow - pargs->offsetrow,
		*pargs->prow - pargs->offsetrow + 1,
		*pargs->pcol, pargs->col_end, pargs->attr);
    *pargs->pcol = pargs->col_end;
}

/*
 * Draw tabpanel content with highlight handling.
 * Processes hltab entries and fills tailing area.
 */
    स्थिर शून्य
draw_tabpanel_with_highlight(
	पूर्णांक	    tplmode,
	char_u	    *buf,
	stl_hlrec_T *hltab,
	tabpanel_T  *pargs)
{
    char_u	*p;
    पूर्णांक		curattr;
    पूर्णांक		n;

    curattr = pargs->attr;
    p = buf;
    साठी (n = 0; hltab[n].start != रिक्त; n++)
    {
	screen_puts_len_for_tabpanel(tplmode, p, (पूर्णांक)(hltab[n].start - p),
		curattr, pargs);
	p = hltab[n].start;
	जर (hltab[n].userhl == 0)
	    curattr = pargs->attr;
	अन्यथा जर (hltab[n].userhl < 0)
	    curattr = syn_id2attr(-hltab[n].userhl);
#ifdef FEAT_TERMINAL
	अन्यथा जर (pargs->wp != रिक्त && pargs->wp != curwin
		&& bt_terminal(pargs->wp->w_buffer)
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stltermnc[hltab[n].userhl - 1];
	अन्यथा जर (pargs->wp != रिक्त && bt_terminal(pargs->wp->w_buffer)
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stlterm[hltab[n].userhl - 1];
#endif
	अन्यथा जर (pargs->wp != रिक्त && pargs->wp != curwin
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stlnc[hltab[n].userhl - 1];
	अन्यथा
	    curattr = highlight_user[hltab[n].userhl - 1];
    }
    screen_puts_len_for_tabpanel(tplmode, p, (पूर्णांक)STRLEN(p), curattr, pargs);

    // fill the tailing area of current row.
    जर (*pargs->prow - pargs->offsetrow >= 0
	    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
	screen_fill_tailing_area(tplmode, *pargs->prow - pargs->offsetrow,
		*pargs->prow - pargs->offsetrow + 1, *pargs->pcol,
		pargs->col_end, curattr);
    *pargs->pcol = pargs->col_end;
}

/*
 * do something by tplmode for drawing tabpanel.
 */
    स्थिर शून्य
do_by_tplmode(
	पूर्णांक	tplmode,
	पूर्णांक	col_start,
	पूर्णांक	col_end,
	पूर्णांक	*pcurtab_row,
	पूर्णांक	*ptabpagenr)
{
    पूर्णांक		attr_tplf = HL_ATTR(HLF_TPLF);
    पूर्णांक		attr_tpls = HL_ATTR(HLF_TPLS);
    पूर्णांक		attr_tpl = HL_ATTR(HLF_TPL);
    पूर्णांक		col = col_start;
    पूर्णांक		row = 0;
    tabpage_T	*tp = रिक्त;
    typval_T	v;
    tabpanel_T	args;

    args.maxrow = Rows;
    args.offsetrow = 0;
    args.col_start = col_start;
    args.col_end = col_end;

    जर (tplmode != TPLMODE_GET_CURTAB_ROW && args.maxrow > 0)
	args.offsetrow = tpl_scroll_offset;

    tp = first_tabpage;

    साठी (row = 0; tp != रिक्त; row++)
    {
	जर (tplmode != TPLMODE_GET_CURTAB_ROW
		&& args.maxrow <= row - args.offsetrow)
	    थांबा;

	col = col_start;

	v.v_type = VAR_संख्या;
	v.vval.v_संख्या = tabpage_index(tp);
	set_var((char_u *)"g:actual_curtabpage", &v, TRUE);

	जर (tp->tp_topframe == topframe)
	{
	    args.attr = attr_tpls;
	    जर (tplmode == TPLMODE_GET_CURTAB_ROW)
		// Capture the row of the current tab and keep iterating so
		// tpl_total_rows receives the true content height below.
		*pcurtab_row = row;
	}
	अन्यथा
	    args.attr = attr_tpl;

	जर (tp == curtab)
	{
	    args.cwp = curwin;
	    args.wp = firstwin;
	}
	अन्यथा
	{
	    args.cwp = tp->tp_curwin;
	    args.wp = tp->tp_firstwin;
	}

	char_u	*usefmt = vim_strsave(p_tpl);

	जर (usefmt != रिक्त && *usefmt != NUL)
	{
	    पूर्णांक	carry_hl = 0;

	    पर्यंत (*usefmt != NUL)
	    {
		char_u	buf[IOSIZE];
		stl_hlrec_T	*hltab;
		stl_hlrec_T	*tabtab;
		stl_clickrec_T	*clicktab = रिक्त;

		जर (tplmode != TPLMODE_GET_CURTAB_ROW
			&& args.maxrow <= row - args.offsetrow)
		    थांबा;

		buf[0] = NUL;
#ifdef ENABLE_STL_MODE_MULTI_NL
		(शून्य)build_stl_str_hl_mline_nl
#अन्यथा
		(शून्य)build_stl_str_hl_mline
#endif
			(args.cwp, buf, आकार(buf),
			&usefmt, opt_name, opt_scope, TPL_FILLCHAR,
			args.col_end - args.col_start, &hltab, &tabtab,
			tplmode == TPLMODE_REDRAW ? &clicktab : रिक्त,
			&carry_hl);

		args.prow = &row;
		args.pcol = &col;

		draw_tabpanel_with_highlight(tplmode, buf, hltab, &args);

		// Record any %[FuncName] click regions for this line once
		// the text has been drawn.  Only visible rows participate.
		जर (tplmode == TPLMODE_REDRAW && clicktab != रिक्त)
		{
		    पूर्णांक screen_row = row - args.offsetrow;
		    पूर्णांक m;

		    जर (screen_row >= 0 && screen_row < args.maxrow)
			tabpanel_append_click_regions(clicktab, buf,
				screen_row, args.col_start, args.col_end,
				(पूर्णांक)v.vval.v_संख्या);
		    // We took ownership of the click records — free the
		    // function names (matches the non-NULL clicktab path in
		    // build_stl_str_hl()).
		    साठी (m = 0; clicktab[m].start != रिक्त; m++)
			vim_free(clicktab[m].funcname);
		}

		// Move to next line for %@
		जर (*usefmt != NUL)
		{
		    row++;
		    col = col_start;
		}
	    }
	}
	अन्यथा
	{
	    args.user_defined = रिक्त;
	    args.prow = &row;
	    args.pcol = &col;
	    draw_tabpanel_default(tplmode, &args);
	}

	vim_free(usefmt);
	do_unlet((char_u *)"g:actual_curtabpage", TRUE);

	tp = tp->tp_next;

	जर ((tplmode == TPLMODE_GET_TABPAGENR)
		&& (mouse_row <= (row - args.offsetrow)))
	{
	    *ptabpagenr = v.vval.v_संख्या;
	    थांबा;
	}
    }

    // fill the area of TabPanelFill.
    screen_fill_tailing_area(tplmode, MAX(row - args.offsetrow, 0), args.maxrow,
	    args.col_start, args.col_end, attr_tplf);

    // Capture the true content height during the GET_CURTAB_ROW pass, which
    // ignores maxrow and therefore walks every tab.  REDRAW stops at the
    // visible edge so its "row" is clamped and unusable here.
    जर (tplmode == TPLMODE_GET_CURTAB_ROW)
	tpl_total_rows = row;
}

/*
 * Draw the tabpanel scrollbar (track + thumb) at screen column 'screen_col'.
 * The scrollbar spans the full screen height.  The thumb position and size
 * are derived from tpl_scroll_offset, tpl_total_rows and Rows.
 */
    स्थिर शून्य
draw_tabpanel_scrollbar(पूर्णांक screen_col)
{
    पूर्णांक attr_sb = HL_ATTR(HLF_PSB);
    पूर्णांक attr_thumb = HL_ATTR(HLF_PST);
    पूर्णांक thumb_शीर्ष = 0;
    पूर्णांक thumb_height = 0;

    जर (tpl_total_rows > Rows && Rows > 0)
    {
	पूर्णांक max_offset = tpl_total_rows - Rows;
	पूर्णांक track_range;

	thumb_height = Rows * Rows / tpl_total_rows;
	जर (thumb_height < 1)
	    thumb_height = 1;

	// Map tpl_scroll_offset onto the track: at offset 0 the thumb's top
	// is at row 0, at the maximum offset its bottom reaches the last
	// row.  This is the exact inverse of tabpanel_drag_scrollbar().
	track_range = Rows - thumb_height;
	जर (track_range > 0 && max_offset > 0)
	    thumb_शीर्ष = track_range * tpl_scroll_offset / max_offset;
	अन्यथा
	    thumb_शीर्ष = 0;
	जर (thumb_शीर्ष + thumb_height > Rows)
	    thumb_शीर्ष = Rows - thumb_height;
	जर (thumb_शीर्ष < 0)
	    thumb_शीर्ष = 0;
    }

    साठी (पूर्णांक r = 0; r < Rows; r++)
    {
	bool on_thumb = thumb_height > 0
	    && r >= thumb_शीर्ष && r < thumb_शीर्ष + thumb_height;
	screen_putchar(TPL_FILLCHAR, r, screen_col,
		on_thumb ? attr_thumb : attr_sb);
    }
}

/*
 * Return true if the mouse is currently positioned over the tabpanel area.
 */
    bool
mouse_on_tabpanel(शून्य)
{
    जर (tabpanel_width() == 0)
	परत false;
    परत mouse_col < firstwin->w_wincol
	|| mouse_col >= firstwin->w_wincol + topframe->fr_width;
}

/*
 * Return true if the mouse is currently on the scrollbar column.
 * The scrollbar column is tracked by draw_tabpanel() and is -1 when the
 * scrollbar is not enabled or not yet drawn.
 */
    bool
mouse_on_tabpanel_scrollbar(शून्य)
{
    परत tpl_scrollbar && tpl_scrollbar_col >= 0
	&& mouse_col == tpl_scrollbar_col;
}

/*
 * Move the scrollbar thumb so it is vertically centred on screen row
 * 'screen_row', updating tpl_scroll_offset accordingly.  Used for both
 * initial clicks and subsequent drag events.
 * Returns true if the event was consumed (offset changed or not).
 */
    bool
tabpanel_drag_scrollbar(पूर्णांक screen_row)
{
    पूर्णांक thumb_height;
    पूर्णांक max_offset;
    पूर्णांक track_range;
    पूर्णांक thumb_शीर्ष;
    पूर्णांक new_offset;

    जर (!tpl_scrollbar || Rows <= 0 || tpl_total_rows <= Rows)
	परत false;

    thumb_height = Rows * Rows / tpl_total_rows;
    जर (thumb_height < 1)
	thumb_height = 1;
    track_range = Rows - thumb_height;
    जर (track_range <= 0)
	परत true;

    max_offset = tpl_total_rows - Rows;
    thumb_शीर्ष = screen_row - thumb_height / 2;
    जर (thumb_शीर्ष < 0)
	thumb_शीर्ष = 0;
    जर (thumb_शीर्ष > track_range)
	thumb_शीर्ष = track_range;

    new_offset = thumb_शीर्ष * max_offset / track_range;
    जर (new_offset != tpl_scroll_offset)
    {
	tpl_scroll_offset = new_offset;
	redraw_tabpanel = TRUE;
    }
    परत true;
}

/*
 * Scroll the tabpanel by 'count' rows in direction 'dir' (1 = down, -1 = up).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_scroll(पूर्णांक dir, पूर्णांक मोजणी)
{
    पूर्णांक max_offset;
    पूर्णांक new_offset;

    जर (tabpanel_width() == 0)
	परत false;

    max_offset = tpl_total_rows - Rows;
    जर (max_offset < 0)
	max_offset = 0;

    new_offset = tpl_scroll_offset + (dir > 0 ? मोजणी : -मोजणी);
    जर (new_offset < 0)
	new_offset = 0;
    जर (new_offset > max_offset)
	new_offset = max_offset;
    जर (new_offset == tpl_scroll_offset)
	परत false;

    tpl_scroll_offset = new_offset;
    redraw_tabpanel = TRUE;
    परत true;
}

/*
 * Set the tabpanel scroll offset to "offset" (clamped to the valid range).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_set_offset(पूर्णांक offset)
{
    पूर्णांक max_offset;

    जर (tabpanel_width() == 0)
	परत false;

    max_offset = tpl_total_rows - Rows;
    जर (max_offset < 0)
	max_offset = 0;

    जर (offset < 0)
	offset = 0;
    जर (offset > max_offset)
	offset = max_offset;
    जर (offset == tpl_scroll_offset)
	परत false;

    tpl_scroll_offset = offset;
    redraw_tabpanel = TRUE;
    परत true;
}

/*
 * "tabpanel_getinfo()" function
 */
    शून्य
f_tabpanel_getinfo(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    पूर्णांक		max_offset;

    जर (rettv_dict_alloc(rettv) == FAIL)
	परत;
    d = rettv->vval.v_dict;

    max_offset = tpl_total_rows - Rows;
    जर (max_offset < 0)
	max_offset = 0;

    dict_add_string(d, "align",
	    (char_u *)(tpl_align == ALIGN_उजवा ? "right" : "left"));
    dict_add_संख्या(d, "columns", tabpanel_width());
    dict_add_bool(d, "scrollbar", tpl_scrollbar);
    dict_add_संख्या(d, "offset", tpl_scroll_offset);
    dict_add_संख्या(d, "total", tpl_total_rows);
    dict_add_संख्या(d, "max_offset", max_offset);
}

/*
 * "tabpanel_scroll()" function
 */
    शून्य
f_tabpanel_scroll(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	n;
    पूर्णांक		absolute = 0;
    bool	changed;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_संख्या = VVAL_FALSE;

    जर (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	परत;

    n = tv_get_संख्या_chk(&argvars[0], रिक्त);
    जर (argvars[1].v_type != VAR_UNKNOWN)
    {
	जर (argvars[1].v_type != VAR_DICT || argvars[1].vval.v_dict == रिक्त)
	{
	    emsg(_(e_dictionary_required));
	    परत;
	}
	absolute = dict_get_bool(argvars[1].vval.v_dict, "absolute", FALSE);
    }

    // Clamp to int range to avoid signed overflow when casting and negating.
    जर (n > पूर्णांक_उच्च)
	n = पूर्णांक_उच्च;
    अन्यथा जर (n < -पूर्णांक_उच्च)
	n = -पूर्णांक_उच्च;

    जर (absolute)
	changed = tabpanel_set_offset((पूर्णांक)n);
    अन्यथा
	changed = tabpanel_scroll(n >= 0 ? 1 : -1,
				  (पूर्णांक)(n >= 0 ? n : -n));

    rettv->vval.v_संख्या = changed ? VVAL_TRUE : VVAL_FALSE;
}

#endif // FEAT_TABPANEL
