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

#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_TABPANEL)

স্থির শূন্য do_by_tplmode(পূর্ণাঙ্ক tplmode, পূর্ণাঙ্ক col_start, পূর্ণাঙ্ক col_end,
	পূর্ণাঙ্ক *pcurtab_row, পূর্ণাঙ্ক *ptabpagenr);
স্থির শূন্য tabpanel_free_click_regions(শূন্য);
স্থির শূন্য tabpanel_append_click_regions(stl_clickrec_T *clicktab,
	char_u *buf, পূর্ণাঙ্ক row, পূর্ণাঙ্ক col_start, পূর্ণাঙ্ক col_end, পূর্ণাঙ্ক tabnr);
স্থির শূন্য draw_tabpanel_scrollbar(পূর্ণাঙ্ক screen_col);

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
#define ALIGN_বাম		0
#define ALIGN_ডান		1

স্থির char_u *opt_name = (char_u *)"tabpanel";
স্থির পূর্ণাঙ্ক opt_scope = OPT_LOCAL;
স্থির পূর্ণাঙ্ক tpl_align = ALIGN_বাম;
স্থির পূর্ণাঙ্ক tpl_columns = 20;
স্থির bool tpl_is_vert = false;
স্থির bool tpl_scrollbar = false;
স্থির পূর্ণাঙ্ক tpl_scroll_offset = 0;
স্থির পূর্ণাঙ্ক tpl_total_rows = 0;
স্থির পূর্ণাঙ্ক tpl_scrollbar_col = -1;  // screen column of scrollbar, -1 if none
স্থির tabpage_T *tpl_last_curtab = রিক্ত;   // last curtab seen by draw_tabpanel

প্রকারনির্দেশ সংরচনা {
    win_T   *wp;
    win_T   *cwp;
    char_u  *user_defined;
    পূর্ণাঙ্ক	maxrow;
    পূর্ণাঙ্ক	offsetrow;
    পূর্ণাঙ্ক	*prow;
    পূর্ণাঙ্ক	*pcol;
    পূর্ণাঙ্ক	attr;
    পূর্ণাঙ্ক	col_start;
    পূর্ণাঙ্ক	col_end;
} tabpanel_T;

    পূর্ণাঙ্ক
tabpanelopt_changed(শূন্য)
{
    char_u	*p;
    পূর্ণাঙ্ক		new_align = ALIGN_বাম;
    long	new_columns = 20;
    bool	new_is_vert = false;
    bool	new_scrollbar = false;

    p = p_tplo;
    যাবত্ (*p != NUL)
    {
	যদি (STRNCMP(p, "align:", 6) == 0)
	{
	    p += 6;
	    যদি (STRNCMP(p, "left", 4) == 0)
	    {
		p += 4;
		new_align = ALIGN_বাম;
	    }
	    অন্যথা যদি (STRNCMP(p, "right", 5) == 0)
	    {
		p += 5;
		new_align = ALIGN_ডান;
	    }
	    অন্যথা
		প্রত্যাগম FAIL;
	}
	অন্যথা যদি (STRNCMP(p, "columns:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    new_columns = getdigits(&p);
	    যদি (new_columns < 0 || new_columns > 1000)
		প্রত্যাগম FAIL;
	}
	অন্যথা যদি (STRNCMP(p, "vert", 4) == 0)
	{
	    p += 4;
	    new_is_vert = true;
	}
	অন্যথা যদি (STRNCMP(p, "scrollbar", 9) == 0)
	{
	    p += 9;
	    new_scrollbar = true;
	}

	যদি (*p != ',' && *p != NUL)
	    প্রত্যাগম FAIL;
	যদি (*p == ',')
	    ++p;
    }

    tpl_align = new_align;
    tpl_columns = new_columns;
    tpl_is_vert = new_is_vert;
    tpl_scrollbar = new_scrollbar;

    // Re-center the current tab on the next redraw.
    tpl_last_curtab = রিক্ত;

    shell_new_columns();
    প্রত্যাগম OK;
}

/*
 * Drop any internal reference to "tp", so draw_tabpanel() never compares
 * against a dangling pointer after the tabpage has been freed.
 */
    শূন্য
tabpanel_forget_tabpage(ধ্রুব tabpage_T *tp)
{
    যদি (tpl_last_curtab == tp)
	tpl_last_curtab = রিক্ত;
}

/*
 * Return the width of tabpanel.
 */
    পূর্ণাঙ্ক
tabpanel_width(শূন্য)
{
    বিকল্প (p_stpl)
    {
	পক্ষ 0:
	    প্রত্যাগম 0;
	পক্ষ 1:
	    যদি (first_tabpage->tp_next == রিক্ত)
		প্রত্যাগম 0;
    }
    যদি (Columns < tpl_columns)
	প্রত্যাগম 0;
    অন্যথা
	প্রত্যাগম tpl_columns;
}

/*
 * Return the offset of a window considering the width of tabpanel.
 */
    পূর্ণাঙ্ক
tabpanel_leftcol(শূন্য)
{
    প্রত্যাগম tpl_align == ALIGN_ডান ? 0 : tabpanel_width();
}

/*
 * Free previously resolved 'tabpanel' click regions.
 */
    স্থির শূন্য
tabpanel_free_click_regions(শূন্য)
{
    পূর্ণাঙ্ক n;

    যদি (tabpanel_stl_click != রিক্ত)
    {
	প্রতি (n = 0; n < tabpanel_stl_click_গুনতি; n++)
	    vim_free(tabpanel_stl_click[n].funcname);
	VIM_CLEAR(tabpanel_stl_click);
    }
    tabpanel_stl_click_গুনতি = 0;
}

/*
 * Convert click records produced by build_stl_str_hl() for one line of
 * 'tabpanel' into screen-column based regions and append them to the global
 * tabpanel_stl_click array.  The caller keeps ownership of the funcname
 * strings inside "clicktab" — this function makes its own copies.
 */
    স্থির শূন্য
tabpanel_append_click_regions(
	stl_clickrec_T	*clicktab,
	char_u		*buf,
	পূর্ণাঙ্ক		row,
	পূর্ণাঙ্ক		col_start,
	পূর্ণাঙ্ক		col_end,
	পূর্ণাঙ্ক		tabnr)
{
    পূর্ণাঙ্ক		গুনতি = 0;
    পূর্ণাঙ্ক		n;
    পূর্ণাঙ্ক		base_col;
    পূর্ণাঙ্ক		acc_width = 0;
    পূর্ণাঙ্ক		max_w = col_end - col_start;
    char_u	*p;
    char_u	*cur_funcname = রিক্ত;
    পূর্ণাঙ্ক		cur_minwid = 0;
    পূর্ণাঙ্ক		region_start_col;
    stl_click_region_T *new_arr;
    পূর্ণাঙ্ক		limit;

    যদি (clicktab == রিক্ত)
	প্রত্যাগম;

    প্রতি (n = 0; clicktab[n].start != রিক্ত; n++)
	গুনতি++;
    যদি (গুনতি == 0)
	প্রত্যাগম;

    base_col = (tpl_align == ALIGN_ডান ? topframe->fr_width : 0) + col_start;
    region_start_col = base_col;

    // Grow the global array to make room for up to "count" more regions
    // (one close for each record plus a possible trailing region).
    new_arr = vim_realloc(tabpanel_stl_click,
	    পরিমাণ(stl_click_region_T) * (tabpanel_stl_click_গুনতি + গুনতি + 1));
    যদি (new_arr == রিক্ত)
	প্রত্যাগম;
    tabpanel_stl_click = new_arr;

    p = buf;
    প্রতি (n = 0; clicktab[n].start != রিক্ত; n++)
    {
	acc_width += vim_strnsize(p, (পূর্ণাঙ্ক)(clicktab[n].start - p));
	p = clicktab[n].start;
	limit = acc_width < max_w ? acc_width : max_w;

	যদি (cur_funcname != রিক্ত)
	{
	    stl_click_region_T *r =
				&tabpanel_stl_click[tabpanel_stl_click_গুনতি];
	    r->row = row;
	    r->col_start = region_start_col;
	    r->col_end = base_col + limit;
	    r->funcname = vim_strsave(cur_funcname);
	    r->minwid = cur_minwid;
	    r->tabnr = tabnr;
	    tabpanel_stl_click_গুনতি++;
	}

	cur_funcname = clicktab[n].funcname;
	cur_minwid = clicktab[n].minwid;
	region_start_col = base_col + limit;
    }

    // Close the final region if it extends to the end.
    যদি (cur_funcname != রিক্ত)
    {
	stl_click_region_T *r = &tabpanel_stl_click[tabpanel_stl_click_গুনতি];
	r->row = row;
	r->col_start = region_start_col;
	r->col_end = base_col + max_w;
	r->funcname = vim_strsave(cur_funcname);
	r->minwid = cur_minwid;
	r->tabnr = tabnr;
	tabpanel_stl_click_গুনতি++;
    }
}

/*
 * Ensure the current tab is visible by adjusting tpl_scroll_offset when
 * the selected tab has changed since the previous redraw.  Mouse wheel or
 * scrollbar drag operations leave curtab unchanged, so the user's chosen
 * offset is preserved in those cases.
 */
    স্থির শূন্য
follow_curtab_if_needed(পূর্ণাঙ্ক curtab_row)
{
    যদি (Rows <= 0 || curtab == tpl_last_curtab)
	প্রত্যাগম;

    যদি (curtab_row < tpl_scroll_offset)
	tpl_scroll_offset = curtab_row;
    অন্যথা যদি (curtab_row >= tpl_scroll_offset + Rows)
	tpl_scroll_offset = curtab_row - Rows + 1;

    পূর্ণাঙ্ক max_offset = tpl_total_rows > Rows ? tpl_total_rows - Rows : 0;

    যদি (tpl_scroll_offset < 0)
	tpl_scroll_offset = 0;
    অন্যথা যদি (tpl_scroll_offset > max_offset)
	tpl_scroll_offset = max_offset;
}

/*
 * draw the tabpanel.
 */
    শূন্য
draw_tabpanel(শূন্য)
{
    পূর্ণাঙ্ক saved_চাবি_Typed = চাবি_Typed;
    পূর্ণাঙ্ক saved_got_int = got_int;
    পূর্ণাঙ্ক maxwidth = tabpanel_width();
    পূর্ণাঙ্ক vs_attr = HL_ATTR(HLF_C);
    পূর্ণাঙ্ক curtab_row = 0;
    পূর্ণাঙ্ক vsrow = 0;
    পূর্ণাঙ্ক is_ডান = tpl_align == ALIGN_ডান;

    যদি (maxwidth == 0)
    {
	tabpanel_free_click_regions();
	প্রত্যাগম;
    }

    // Discard old click regions — they'll be rebuilt during redraw below.
    tabpanel_free_click_regions();

    // Reset got_int to avoid build_stl_str_hl() isn't evaluated.
    got_int = FALSE;

    পূর্ণাঙ্ক sb_len = tpl_scrollbar ? SCROLL_LEN : 0;
    পূর্ণাঙ্ক sb_screen_col = -1;

    // The scrollbar is always placed at the right edge of the tabpanel,
    // regardless of 'align'.  The vertical separator sits at the panel's
    // boundary with the buffer area (left edge for align:right, right edge
    // for align:left).
    যদি (tpl_is_vert)
    {
	যদি (is_ডান)
	{
	    // Panel on the right: vert at panel's left edge, scrollbar at
	    // panel's right edge (= screen's right edge).
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, VERT_LEN,
		    maxwidth - sb_len, &curtab_row, রিক্ত);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, VERT_LEN, maxwidth - sb_len,
		    &curtab_row, রিক্ত);
	    প্রতি (vsrow = 0; vsrow < Rows; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			topframe->fr_width, vs_attr);
	    যদি (tpl_scrollbar)
		sb_screen_col = topframe->fr_width + maxwidth - SCROLL_LEN;
	}
	অন্যথা
	{
	    // Panel on the left: scrollbar just left of vert, vert at
	    // panel's right edge (boundary with buffer).
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0,
		    maxwidth - VERT_LEN - sb_len, &curtab_row, রিক্ত);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - VERT_LEN - sb_len,
		    &curtab_row, রিক্ত);
	    প্রতি (vsrow = 0; vsrow < Rows; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			maxwidth - VERT_LEN, vs_attr);
	    যদি (tpl_scrollbar)
		sb_screen_col = maxwidth - VERT_LEN - SCROLL_LEN;
	}
    }
    অন্যথা
    {
	যদি (is_ডান)
	{
	    // Panel on the right, no vert: scrollbar at screen's right edge.
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth - sb_len,
		    &curtab_row, রিক্ত);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_row, রিক্ত);
	    যদি (tpl_scrollbar)
		sb_screen_col = topframe->fr_width + maxwidth - SCROLL_LEN;
	}
	অন্যথা
	{
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth - sb_len,
		    &curtab_row, রিক্ত);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_row, রিক্ত);
	    যদি (tpl_scrollbar)
		sb_screen_col = maxwidth - SCROLL_LEN;
	}
    }

    tpl_scrollbar_col = sb_screen_col;
    যদি (sb_screen_col >= 0)
	draw_tabpanel_scrollbar(sb_screen_col);

    got_int |= saved_got_int;

    // A user function may reset KeyTyped, restore it.
    চাবি_Typed = saved_চাবি_Typed;

    tpl_last_curtab = curtab;
    redraw_tabpanel = FALSE;
}

/*
 * Return tabpagenr when clicking and dragging in tabpanel.
 */
    পূর্ণাঙ্ক
get_tabpagenr_on_tabpanel(শূন্য)
{
    পূর্ণাঙ্ক		maxwidth = tabpanel_width();
    পূর্ণাঙ্ক		curtab_row = 0;
    পূর্ণাঙ্ক		tabpagenr = 0;

    যদি (maxwidth == 0)
	প্রত্যাগম -1;

    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth, &curtab_row, রিক্ত);
    do_by_tplmode(TPLMODE_GET_TABPAGENR, 0, maxwidth, &curtab_row,
	    &tabpagenr);

    প্রত্যাগম tabpagenr;
}

/*
 * Fill tailing area between {start_row} and {end_row - 1}.
 */
    স্থির শূন্য
screen_fill_tailing_area(
	পূর্ণাঙ্ক	tplmode,
	পূর্ণাঙ্ক	row_start,
	পূর্ণাঙ্ক	row_end,
	পূর্ণাঙ্ক	col_start,
	পূর্ণাঙ্ক	col_end,
	পূর্ণাঙ্ক	attr)
{
    পূর্ণাঙ্ক is_ডান = tpl_align == ALIGN_ডান;
    যদি (tplmode == TPLMODE_REDRAW)
	screen_fill(row_start, row_end,
		(is_ডান ? topframe->fr_width : 0) + col_start,
		(is_ডান ? topframe->fr_width : 0) + col_end,
		TPL_FILLCHAR, TPL_FILLCHAR, attr);
}

/*
 * screen_puts_len() for tabpanel.
 */
    স্থির শূন্য
screen_puts_len_for_tabpanel(
	পূর্ণাঙ্ক	    tplmode,
	char_u	    *p,
	পূর্ণাঙ্ক	    len,
	পূর্ণাঙ্ক	    attr,
	tabpanel_T  *pargs)
{
    পূর্ণাঙ্ক		j;
    পূর্ণাঙ্ক		chlen;
    পূর্ণাঙ্ক		chcells;
    char_u	buf[IOSIZE];
    char_u	*temp;

    প্রতি (j = 0; j < len;)
    {
	যদি (tplmode != TPLMODE_GET_CURTAB_ROW
		&& pargs->maxrow <= *pargs->prow - pargs->offsetrow)
	    ভঙ্গ;

	যদি (has_mbyte)
	    chlen = (*mb_ptr2len)(p + j);
	অন্যথা
	    chlen = 1;

	প্রতি (পূর্ণাঙ্ক k = 0; k < chlen; k++)
	    buf[k] = p[j + k];
	buf[chlen] = NUL;
	j += chlen;

	// Make all characters printable.
	temp = transstr(buf);
	যদি (temp != রিক্ত)
	{
	    vim_strncpy(buf, temp, পরিমাণ(buf) - 1);
	    vim_free(temp);
	}

	যদি (has_mbyte)
	    chcells = (*mb_ptr2cells)(buf);
	অন্যথা
	    chcells = 1;

	যদি (pargs->col_end < (*pargs->pcol) + chcells)
	{
	    // fill the tailing area of current row.
	    যদি (*pargs->prow - pargs->offsetrow >= 0
		    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
		screen_fill_tailing_area(tplmode,
			*pargs->prow - pargs->offsetrow,
			*pargs->prow - pargs->offsetrow + 1,
			*pargs->pcol, pargs->col_end, attr);
	    *pargs->pcol = pargs->col_end;

	    যদি (pargs->col_end < chcells)
		ভঙ্গ;
	}

	যদি (*pargs->pcol + chcells <= pargs->col_end)
	{
	    পূর্ণাঙ্ক off = (tpl_align == ALIGN_ডান)
		    ? topframe->fr_width
		    : 0;
	    যদি (tplmode == TPLMODE_REDRAW
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
    স্থির শূন্য
draw_tabpanel_default(পূর্ণাঙ্ক tplmode, tabpanel_T *pargs)
{
    পূর্ণাঙ্ক		modified;
    পূর্ণাঙ্ক		wincount;
    পূর্ণাঙ্ক		len = 0;
    char_u	buf[2] = { NUL, NUL };

    modified = FALSE;
    প্রতি (wincount = 0; pargs->wp != রিক্ত;
	    pargs->wp = pargs->wp->w_next, ++wincount)
	যদি (bufIsChanged(pargs->wp->w_buffer))
	    modified = TRUE;

    যদি (modified || wincount > 1)
    {
	যদি (wincount > 1)
	{
	    vim_snprintf((বর্ণ *)NameBuff, MAXPATHL, "%d", wincount);
	    len = (পূর্ণাঙ্ক)STRLEN(NameBuff);
	    screen_puts_len_for_tabpanel(tplmode, NameBuff, len,
#যদি defined(FEAT_SYN_HL)
		    hl_combine_attr(pargs->attr, HL_ATTR(HLF_T)),
#অন্যথা
		    pargs->attr,
#endif
		    pargs);
	}
	যদি (modified)
	{
	    buf[0] = '+';
	    screen_puts_len_for_tabpanel(tplmode, buf, 1, pargs->attr, pargs);
	}

	buf[0] = TPL_FILLCHAR;
	screen_puts_len_for_tabpanel(tplmode, buf, 1, pargs->attr, pargs);
    }

    get_trans_bufname(pargs->cwp->w_buffer);
    shorten_dir(NameBuff);
    len = (পূর্ণাঙ্ক)STRLEN(NameBuff);
    screen_puts_len_for_tabpanel(tplmode, NameBuff, len, pargs->attr, pargs);

    // fill the tailing area of current row.
    যদি (*pargs->prow - pargs->offsetrow >= 0
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
    স্থির শূন্য
draw_tabpanel_with_highlight(
	পূর্ণাঙ্ক	    tplmode,
	char_u	    *buf,
	stl_hlrec_T *hltab,
	tabpanel_T  *pargs)
{
    char_u	*p;
    পূর্ণাঙ্ক		curattr;
    পূর্ণাঙ্ক		n;

    curattr = pargs->attr;
    p = buf;
    প্রতি (n = 0; hltab[n].start != রিক্ত; n++)
    {
	screen_puts_len_for_tabpanel(tplmode, p, (পূর্ণাঙ্ক)(hltab[n].start - p),
		curattr, pargs);
	p = hltab[n].start;
	যদি (hltab[n].userhl == 0)
	    curattr = pargs->attr;
	অন্যথা যদি (hltab[n].userhl < 0)
	    curattr = syn_id2attr(-hltab[n].userhl);
#ifdef FEAT_TERMINAL
	অন্যথা যদি (pargs->wp != রিক্ত && pargs->wp != curwin
		&& bt_terminal(pargs->wp->w_buffer)
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stltermnc[hltab[n].userhl - 1];
	অন্যথা যদি (pargs->wp != রিক্ত && bt_terminal(pargs->wp->w_buffer)
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stlterm[hltab[n].userhl - 1];
#endif
	অন্যথা যদি (pargs->wp != রিক্ত && pargs->wp != curwin
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stlnc[hltab[n].userhl - 1];
	অন্যথা
	    curattr = highlight_user[hltab[n].userhl - 1];
    }
    screen_puts_len_for_tabpanel(tplmode, p, (পূর্ণাঙ্ক)STRLEN(p), curattr, pargs);

    // fill the tailing area of current row.
    যদি (*pargs->prow - pargs->offsetrow >= 0
	    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
	screen_fill_tailing_area(tplmode, *pargs->prow - pargs->offsetrow,
		*pargs->prow - pargs->offsetrow + 1, *pargs->pcol,
		pargs->col_end, curattr);
    *pargs->pcol = pargs->col_end;
}

/*
 * do something by tplmode for drawing tabpanel.
 */
    স্থির শূন্য
do_by_tplmode(
	পূর্ণাঙ্ক	tplmode,
	পূর্ণাঙ্ক	col_start,
	পূর্ণাঙ্ক	col_end,
	পূর্ণাঙ্ক	*pcurtab_row,
	পূর্ণাঙ্ক	*ptabpagenr)
{
    পূর্ণাঙ্ক		attr_tplf = HL_ATTR(HLF_TPLF);
    পূর্ণাঙ্ক		attr_tpls = HL_ATTR(HLF_TPLS);
    পূর্ণাঙ্ক		attr_tpl = HL_ATTR(HLF_TPL);
    পূর্ণাঙ্ক		col = col_start;
    পূর্ণাঙ্ক		row = 0;
    tabpage_T	*tp = রিক্ত;
    typval_T	v;
    tabpanel_T	args;

    args.maxrow = Rows;
    args.offsetrow = 0;
    args.col_start = col_start;
    args.col_end = col_end;

    যদি (tplmode != TPLMODE_GET_CURTAB_ROW && args.maxrow > 0)
	args.offsetrow = tpl_scroll_offset;

    tp = first_tabpage;

    প্রতি (row = 0; tp != রিক্ত; row++)
    {
	যদি (tplmode != TPLMODE_GET_CURTAB_ROW
		&& args.maxrow <= row - args.offsetrow)
	    ভঙ্গ;

	col = col_start;

	v.v_type = VAR_সংখ্যা;
	v.vval.v_সংখ্যা = tabpage_index(tp);
	set_var((char_u *)"g:actual_curtabpage", &v, TRUE);

	যদি (tp->tp_topframe == topframe)
	{
	    args.attr = attr_tpls;
	    যদি (tplmode == TPLMODE_GET_CURTAB_ROW)
		// Capture the row of the current tab and keep iterating so
		// tpl_total_rows receives the true content height below.
		*pcurtab_row = row;
	}
	অন্যথা
	    args.attr = attr_tpl;

	যদি (tp == curtab)
	{
	    args.cwp = curwin;
	    args.wp = firstwin;
	}
	অন্যথা
	{
	    args.cwp = tp->tp_curwin;
	    args.wp = tp->tp_firstwin;
	}

	char_u	*usefmt = vim_strsave(p_tpl);

	যদি (usefmt != রিক্ত && *usefmt != NUL)
	{
	    পূর্ণাঙ্ক	carry_hl = 0;

	    যাবত্ (*usefmt != NUL)
	    {
		char_u	buf[IOSIZE];
		stl_hlrec_T	*hltab;
		stl_hlrec_T	*tabtab;
		stl_clickrec_T	*clicktab = রিক্ত;

		যদি (tplmode != TPLMODE_GET_CURTAB_ROW
			&& args.maxrow <= row - args.offsetrow)
		    ভঙ্গ;

		buf[0] = NUL;
#ifdef ENABLE_STL_MODE_MULTI_NL
		(শূন্য)build_stl_str_hl_mline_nl
#অন্যথা
		(শূন্য)build_stl_str_hl_mline
#endif
			(args.cwp, buf, পরিমাণ(buf),
			&usefmt, opt_name, opt_scope, TPL_FILLCHAR,
			args.col_end - args.col_start, &hltab, &tabtab,
			tplmode == TPLMODE_REDRAW ? &clicktab : রিক্ত,
			&carry_hl);

		args.prow = &row;
		args.pcol = &col;

		draw_tabpanel_with_highlight(tplmode, buf, hltab, &args);

		// Record any %[FuncName] click regions for this line once
		// the text has been drawn.  Only visible rows participate.
		যদি (tplmode == TPLMODE_REDRAW && clicktab != রিক্ত)
		{
		    পূর্ণাঙ্ক screen_row = row - args.offsetrow;
		    পূর্ণাঙ্ক m;

		    যদি (screen_row >= 0 && screen_row < args.maxrow)
			tabpanel_append_click_regions(clicktab, buf,
				screen_row, args.col_start, args.col_end,
				(পূর্ণাঙ্ক)v.vval.v_সংখ্যা);
		    // We took ownership of the click records — free the
		    // function names (matches the non-NULL clicktab path in
		    // build_stl_str_hl()).
		    প্রতি (m = 0; clicktab[m].start != রিক্ত; m++)
			vim_free(clicktab[m].funcname);
		}

		// Move to next line for %@
		যদি (*usefmt != NUL)
		{
		    row++;
		    col = col_start;
		}
	    }
	}
	অন্যথা
	{
	    args.user_defined = রিক্ত;
	    args.prow = &row;
	    args.pcol = &col;
	    draw_tabpanel_default(tplmode, &args);
	}

	vim_free(usefmt);
	do_unlet((char_u *)"g:actual_curtabpage", TRUE);

	tp = tp->tp_next;

	যদি ((tplmode == TPLMODE_GET_TABPAGENR)
		&& (mouse_row <= (row - args.offsetrow)))
	{
	    *ptabpagenr = v.vval.v_সংখ্যা;
	    ভঙ্গ;
	}
    }

    // fill the area of TabPanelFill.
    screen_fill_tailing_area(tplmode, MAX(row - args.offsetrow, 0), args.maxrow,
	    args.col_start, args.col_end, attr_tplf);

    // Capture the true content height during the GET_CURTAB_ROW pass, which
    // ignores maxrow and therefore walks every tab.  REDRAW stops at the
    // visible edge so its "row" is clamped and unusable here.
    যদি (tplmode == TPLMODE_GET_CURTAB_ROW)
	tpl_total_rows = row;
}

/*
 * Draw the tabpanel scrollbar (track + thumb) at screen column 'screen_col'.
 * The scrollbar spans the full screen height.  The thumb position and size
 * are derived from tpl_scroll_offset, tpl_total_rows and Rows.
 */
    স্থির শূন্য
draw_tabpanel_scrollbar(পূর্ণাঙ্ক screen_col)
{
    পূর্ণাঙ্ক attr_sb = HL_ATTR(HLF_PSB);
    পূর্ণাঙ্ক attr_thumb = HL_ATTR(HLF_PST);
    পূর্ণাঙ্ক thumb_শীর্ষ = 0;
    পূর্ণাঙ্ক thumb_height = 0;

    যদি (tpl_total_rows > Rows && Rows > 0)
    {
	পূর্ণাঙ্ক max_offset = tpl_total_rows - Rows;
	পূর্ণাঙ্ক track_range;

	thumb_height = Rows * Rows / tpl_total_rows;
	যদি (thumb_height < 1)
	    thumb_height = 1;

	// Map tpl_scroll_offset onto the track: at offset 0 the thumb's top
	// is at row 0, at the maximum offset its bottom reaches the last
	// row.  This is the exact inverse of tabpanel_drag_scrollbar().
	track_range = Rows - thumb_height;
	যদি (track_range > 0 && max_offset > 0)
	    thumb_শীর্ষ = track_range * tpl_scroll_offset / max_offset;
	অন্যথা
	    thumb_শীর্ষ = 0;
	যদি (thumb_শীর্ষ + thumb_height > Rows)
	    thumb_শীর্ষ = Rows - thumb_height;
	যদি (thumb_শীর্ষ < 0)
	    thumb_শীর্ষ = 0;
    }

    প্রতি (পূর্ণাঙ্ক r = 0; r < Rows; r++)
    {
	bool on_thumb = thumb_height > 0
	    && r >= thumb_শীর্ষ && r < thumb_শীর্ষ + thumb_height;
	screen_putchar(TPL_FILLCHAR, r, screen_col,
		on_thumb ? attr_thumb : attr_sb);
    }
}

/*
 * Return true if the mouse is currently positioned over the tabpanel area.
 */
    bool
mouse_on_tabpanel(শূন্য)
{
    যদি (tabpanel_width() == 0)
	প্রত্যাগম false;
    প্রত্যাগম mouse_col < firstwin->w_wincol
	|| mouse_col >= firstwin->w_wincol + topframe->fr_width;
}

/*
 * Return true if the mouse is currently on the scrollbar column.
 * The scrollbar column is tracked by draw_tabpanel() and is -1 when the
 * scrollbar is not enabled or not yet drawn.
 */
    bool
mouse_on_tabpanel_scrollbar(শূন্য)
{
    প্রত্যাগম tpl_scrollbar && tpl_scrollbar_col >= 0
	&& mouse_col == tpl_scrollbar_col;
}

/*
 * Move the scrollbar thumb so it is vertically centred on screen row
 * 'screen_row', updating tpl_scroll_offset accordingly.  Used for both
 * initial clicks and subsequent drag events.
 * Returns true if the event was consumed (offset changed or not).
 */
    bool
tabpanel_drag_scrollbar(পূর্ণাঙ্ক screen_row)
{
    পূর্ণাঙ্ক thumb_height;
    পূর্ণাঙ্ক max_offset;
    পূর্ণাঙ্ক track_range;
    পূর্ণাঙ্ক thumb_শীর্ষ;
    পূর্ণাঙ্ক new_offset;

    যদি (!tpl_scrollbar || Rows <= 0 || tpl_total_rows <= Rows)
	প্রত্যাগম false;

    thumb_height = Rows * Rows / tpl_total_rows;
    যদি (thumb_height < 1)
	thumb_height = 1;
    track_range = Rows - thumb_height;
    যদি (track_range <= 0)
	প্রত্যাগম true;

    max_offset = tpl_total_rows - Rows;
    thumb_শীর্ষ = screen_row - thumb_height / 2;
    যদি (thumb_শীর্ষ < 0)
	thumb_শীর্ষ = 0;
    যদি (thumb_শীর্ষ > track_range)
	thumb_শীর্ষ = track_range;

    new_offset = thumb_শীর্ষ * max_offset / track_range;
    যদি (new_offset != tpl_scroll_offset)
    {
	tpl_scroll_offset = new_offset;
	redraw_tabpanel = TRUE;
    }
    প্রত্যাগম true;
}

/*
 * Scroll the tabpanel by 'count' rows in direction 'dir' (1 = down, -1 = up).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_scroll(পূর্ণাঙ্ক dir, পূর্ণাঙ্ক গুনতি)
{
    পূর্ণাঙ্ক max_offset;
    পূর্ণাঙ্ক new_offset;

    যদি (tabpanel_width() == 0)
	প্রত্যাগম false;

    max_offset = tpl_total_rows - Rows;
    যদি (max_offset < 0)
	max_offset = 0;

    new_offset = tpl_scroll_offset + (dir > 0 ? গুনতি : -গুনতি);
    যদি (new_offset < 0)
	new_offset = 0;
    যদি (new_offset > max_offset)
	new_offset = max_offset;
    যদি (new_offset == tpl_scroll_offset)
	প্রত্যাগম false;

    tpl_scroll_offset = new_offset;
    redraw_tabpanel = TRUE;
    প্রত্যাগম true;
}

/*
 * Set the tabpanel scroll offset to "offset" (clamped to the valid range).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_set_offset(পূর্ণাঙ্ক offset)
{
    পূর্ণাঙ্ক max_offset;

    যদি (tabpanel_width() == 0)
	প্রত্যাগম false;

    max_offset = tpl_total_rows - Rows;
    যদি (max_offset < 0)
	max_offset = 0;

    যদি (offset < 0)
	offset = 0;
    যদি (offset > max_offset)
	offset = max_offset;
    যদি (offset == tpl_scroll_offset)
	প্রত্যাগম false;

    tpl_scroll_offset = offset;
    redraw_tabpanel = TRUE;
    প্রত্যাগম true;
}

/*
 * "tabpanel_getinfo()" function
 */
    শূন্য
f_tabpanel_getinfo(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    পূর্ণাঙ্ক		max_offset;

    যদি (rettv_dict_alloc(rettv) == FAIL)
	প্রত্যাগম;
    d = rettv->vval.v_dict;

    max_offset = tpl_total_rows - Rows;
    যদি (max_offset < 0)
	max_offset = 0;

    dict_add_string(d, "align",
	    (char_u *)(tpl_align == ALIGN_ডান ? "right" : "left"));
    dict_add_সংখ্যা(d, "columns", tabpanel_width());
    dict_add_bool(d, "scrollbar", tpl_scrollbar);
    dict_add_সংখ্যা(d, "offset", tpl_scroll_offset);
    dict_add_সংখ্যা(d, "total", tpl_total_rows);
    dict_add_সংখ্যা(d, "max_offset", max_offset);
}

/*
 * "tabpanel_scroll()" function
 */
    শূন্য
f_tabpanel_scroll(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	n;
    পূর্ণাঙ্ক		absolute = 0;
    bool	changed;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_সংখ্যা = VVAL_FALSE;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	প্রত্যাগম;

    n = tv_get_সংখ্যা_chk(&argvars[0], রিক্ত);
    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	যদি (argvars[1].v_type != VAR_DICT || argvars[1].vval.v_dict == রিক্ত)
	{
	    emsg(_(e_dictionary_required));
	    প্রত্যাগম;
	}
	absolute = dict_get_bool(argvars[1].vval.v_dict, "absolute", FALSE);
    }

    // Clamp to int range to avoid signed overflow when casting and negating.
    যদি (n > INT_MAX)
	n = INT_MAX;
    অন্যথা যদি (n < -INT_MAX)
	n = -INT_MAX;

    যদি (absolute)
	changed = tabpanel_set_offset((পূর্ণাঙ্ক)n);
    অন্যথা
	changed = tabpanel_scroll(n >= 0 ? 1 : -1,
				  (পূর্ণাঙ্ক)(n >= 0 ? n : -n));

    rettv->vval.v_সংখ্যা = changed ? VVAL_TRUE : VVAL_FALSE;
}

#endif // FEAT_TABPANEL
