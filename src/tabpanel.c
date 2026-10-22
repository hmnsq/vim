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

স্থির শূন্য do_by_tplmode(পূর্ণসংখ্যা tplmode, পূর্ণসংখ্যা col_start, পূর্ণসংখ্যা col_end,
	পূর্ণসংখ্যা *pcurtab_row, পূর্ণসংখ্যা *ptabpagenr);
স্থির শূন্য tabpanel_free_click_regions(শূন্য);
স্থির শূন্য tabpanel_append_click_regions(stl_clickrec_T *clicktab,
	char_u *buf, পূর্ণসংখ্যা row, পূর্ণসংখ্যা col_start, পূর্ণসংখ্যা col_end, পূর্ণসংখ্যা tabnr);
স্থির শূন্য draw_tabpanel_scrollbar(পূর্ণসংখ্যা screen_col);

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
স্থির পূর্ণসংখ্যা opt_scope = OPT_LOCAL;
স্থির পূর্ণসংখ্যা tpl_align = ALIGN_বাম;
স্থির পূর্ণসংখ্যা tpl_columns = 20;
স্থির bool tpl_is_vert = false;
স্থির bool tpl_scrollbar = false;
স্থির পূর্ণসংখ্যা tpl_scroll_offset = 0;
স্থির পূর্ণসংখ্যা tpl_যোগ_rows = 0;
স্থির পূর্ণসংখ্যা tpl_scrollbar_col = -1;  // screen column of scrollbar, -1 if none
স্থির tabpage_T *tpl_last_curtab = ফাঁকা;   // last curtab seen by draw_tabpanel

প্রকারলেখ কাঠামো {
    win_T   *wp;
    win_T   *cwp;
    char_u  *user_defined;
    পূর্ণসংখ্যা	maxrow;
    পূর্ণসংখ্যা	offsetrow;
    পূর্ণসংখ্যা	*prow;
    পূর্ণসংখ্যা	*pcol;
    পূর্ণসংখ্যা	attr;
    পূর্ণসংখ্যা	col_start;
    পূর্ণসংখ্যা	col_end;
} tabpanel_T;

    পূর্ণসংখ্যা
tabpanelopt_changed(শূন্য)
{
    char_u	*p;
    পূর্ণসংখ্যা		new_align = ALIGN_বাম;
    long	new_columns = 20;
    bool	new_is_vert = false;
    bool	new_scrollbar = false;

    p = p_tplo;
    যখন (*p != NUL)
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
		ফেরত FAIL;
	}
	অন্যথা যদি (STRNCMP(p, "columns:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    new_columns = getdigits(&p);
	    যদি (new_columns < 0 || new_columns > 1000)
		ফেরত FAIL;
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
	    ফেরত FAIL;
	যদি (*p == ',')
	    ++p;
    }

    tpl_align = new_align;
    tpl_columns = new_columns;
    tpl_is_vert = new_is_vert;
    tpl_scrollbar = new_scrollbar;

    // Re-center the current tab on the next redraw.
    tpl_last_curtab = ফাঁকা;

    shell_new_columns();
    ফেরত OK;
}

/*
 * Drop any internal reference to "tp", so draw_tabpanel() never compares
 * against a dangling pointer after the tabpage has been freed.
 */
    শূন্য
tabpanel_forget_tabpage(ধ্রুবক tabpage_T *tp)
{
    যদি (tpl_last_curtab == tp)
	tpl_last_curtab = ফাঁকা;
}

/*
 * Return the width of tabpanel.
 */
    পূর্ণসংখ্যা
tabpanel_width(শূন্য)
{
    নির্বাচন (p_stpl)
    {
	ক্ষেত্র 0:
	    ফেরত 0;
	ক্ষেত্র 1:
	    যদি (first_tabpage->tp_পরবর্তী == ফাঁকা)
		ফেরত 0;
    }
    যদি (Columns < tpl_columns)
	ফেরত 0;
    অন্যথা
	ফেরত tpl_columns;
}

/*
 * Return the offset of a window considering the width of tabpanel.
 */
    পূর্ণসংখ্যা
tabpanel_leftcol(শূন্য)
{
    ফেরত tpl_align == ALIGN_ডান ? 0 : tabpanel_width();
}

/*
 * Free previously resolved 'tabpanel' click regions.
 */
    স্থির শূন্য
tabpanel_free_click_regions(শূন্য)
{
    পূর্ণসংখ্যা n;

    যদি (tabpanel_stl_click != ফাঁকা)
    {
	জন্য (n = 0; n < tabpanel_stl_click_গুনতি; n++)
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
	পূর্ণসংখ্যা		row,
	পূর্ণসংখ্যা		col_start,
	পূর্ণসংখ্যা		col_end,
	পূর্ণসংখ্যা		tabnr)
{
    পূর্ণসংখ্যা		গুনতি = 0;
    পূর্ণসংখ্যা		n;
    পূর্ণসংখ্যা		base_col;
    পূর্ণসংখ্যা		acc_width = 0;
    পূর্ণসংখ্যা		max_w = col_end - col_start;
    char_u	*p;
    char_u	*cur_funcname = ফাঁকা;
    পূর্ণসংখ্যা		cur_minwid = 0;
    পূর্ণসংখ্যা		region_start_col;
    stl_click_region_T *new_arr;
    পূর্ণসংখ্যা		limit;

    যদি (clicktab == ফাঁকা)
	ফেরত;

    জন্য (n = 0; clicktab[n].start != ফাঁকা; n++)
	গুনতি++;
    যদি (গুনতি == 0)
	ফেরত;

    base_col = (tpl_align == ALIGN_ডান ? topframe->fr_width : 0) + col_start;
    region_start_col = base_col;

    // Grow the global array to make room for up to "count" more regions
    // (one close for each record plus a possible trailing region).
    new_arr = vim_realloc(tabpanel_stl_click,
	    আকার(stl_click_region_T) * (tabpanel_stl_click_গুনতি + গুনতি + 1));
    যদি (new_arr == ফাঁকা)
	ফেরত;
    tabpanel_stl_click = new_arr;

    p = buf;
    জন্য (n = 0; clicktab[n].start != ফাঁকা; n++)
    {
	acc_width += vim_strnsize(p, (পূর্ণসংখ্যা)(clicktab[n].start - p));
	p = clicktab[n].start;
	limit = acc_width < max_w ? acc_width : max_w;

	যদি (cur_funcname != ফাঁকা)
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
    যদি (cur_funcname != ফাঁকা)
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
follow_curtab_if_needed(পূর্ণসংখ্যা curtab_row)
{
    যদি (Rows <= 0 || curtab == tpl_last_curtab)
	ফেরত;

    যদি (curtab_row < tpl_scroll_offset)
	tpl_scroll_offset = curtab_row;
    অন্যথা যদি (curtab_row >= tpl_scroll_offset + Rows)
	tpl_scroll_offset = curtab_row - Rows + 1;

    পূর্ণসংখ্যা max_offset = tpl_যোগ_rows > Rows ? tpl_যোগ_rows - Rows : 0;

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
    পূর্ণসংখ্যা saved_চাবি_Typed = চাবি_Typed;
    পূর্ণসংখ্যা saved_got_int = got_int;
    পূর্ণসংখ্যা maxwidth = tabpanel_width();
    পূর্ণসংখ্যা vs_attr = HL_ATTR(HLF_C);
    পূর্ণসংখ্যা curtab_row = 0;
    পূর্ণসংখ্যা vsrow = 0;
    পূর্ণসংখ্যা is_ডান = tpl_align == ALIGN_ডান;

    যদি (maxwidth == 0)
    {
	tabpanel_free_click_regions();
	ফেরত;
    }

    // Discard old click regions — they'll be rebuilt during redraw below.
    tabpanel_free_click_regions();

    // Reset got_int to avoid build_stl_str_hl() isn't evaluated.
    got_int = FALSE;

    পূর্ণসংখ্যা sb_len = tpl_scrollbar ? SCROLL_LEN : 0;
    পূর্ণসংখ্যা sb_screen_col = -1;

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
		    maxwidth - sb_len, &curtab_row, ফাঁকা);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, VERT_LEN, maxwidth - sb_len,
		    &curtab_row, ফাঁকা);
	    জন্য (vsrow = 0; vsrow < Rows; vsrow++)
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
		    maxwidth - VERT_LEN - sb_len, &curtab_row, ফাঁকা);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - VERT_LEN - sb_len,
		    &curtab_row, ফাঁকা);
	    জন্য (vsrow = 0; vsrow < Rows; vsrow++)
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
		    &curtab_row, ফাঁকা);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_row, ফাঁকা);
	    যদি (tpl_scrollbar)
		sb_screen_col = topframe->fr_width + maxwidth - SCROLL_LEN;
	}
	অন্যথা
	{
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth - sb_len,
		    &curtab_row, ফাঁকা);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_row, ফাঁকা);
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
    পূর্ণসংখ্যা
get_tabpagenr_on_tabpanel(শূন্য)
{
    পূর্ণসংখ্যা		maxwidth = tabpanel_width();
    পূর্ণসংখ্যা		curtab_row = 0;
    পূর্ণসংখ্যা		tabpagenr = 0;

    যদি (maxwidth == 0)
	ফেরত -1;

    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth, &curtab_row, ফাঁকা);
    do_by_tplmode(TPLMODE_GET_TABPAGENR, 0, maxwidth, &curtab_row,
	    &tabpagenr);

    ফেরত tabpagenr;
}

/*
 * Fill tailing area between {start_row} and {end_row - 1}.
 */
    স্থির শূন্য
screen_fill_tailing_area(
	পূর্ণসংখ্যা	tplmode,
	পূর্ণসংখ্যা	row_start,
	পূর্ণসংখ্যা	row_end,
	পূর্ণসংখ্যা	col_start,
	পূর্ণসংখ্যা	col_end,
	পূর্ণসংখ্যা	attr)
{
    পূর্ণসংখ্যা is_ডান = tpl_align == ALIGN_ডান;
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
	পূর্ণসংখ্যা	    tplmode,
	char_u	    *p,
	পূর্ণসংখ্যা	    len,
	পূর্ণসংখ্যা	    attr,
	tabpanel_T  *pargs)
{
    পূর্ণসংখ্যা		j;
    পূর্ণসংখ্যা		chlen;
    পূর্ণসংখ্যা		chcells;
    char_u	buf[IOSIZE];
    char_u	*অস্থায়ী;

    জন্য (j = 0; j < len;)
    {
	যদি (tplmode != TPLMODE_GET_CURTAB_ROW
		&& pargs->maxrow <= *pargs->prow - pargs->offsetrow)
	    বিরতি;

	যদি (has_mbyte)
	    chlen = (*mb_ptr2len)(p + j);
	অন্যথা
	    chlen = 1;

	জন্য (পূর্ণসংখ্যা k = 0; k < chlen; k++)
	    buf[k] = p[j + k];
	buf[chlen] = NUL;
	j += chlen;

	// Make all characters printable.
	অস্থায়ী = transstr(buf);
	যদি (অস্থায়ী != ফাঁকা)
	{
	    vim_strncpy(buf, অস্থায়ী, আকার(buf) - 1);
	    vim_free(অস্থায়ী);
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
		বিরতি;
	}

	যদি (*pargs->pcol + chcells <= pargs->col_end)
	{
	    পূর্ণসংখ্যা off = (tpl_align == ALIGN_ডান)
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
draw_tabpanel_default(পূর্ণসংখ্যা tplmode, tabpanel_T *pargs)
{
    পূর্ণসংখ্যা		modified;
    পূর্ণসংখ্যা		wincount;
    পূর্ণসংখ্যা		len = 0;
    char_u	buf[2] = { NUL, NUL };

    modified = FALSE;
    জন্য (wincount = 0; pargs->wp != ফাঁকা;
	    pargs->wp = pargs->wp->w_পরবর্তী, ++wincount)
	যদি (bufIsChanged(pargs->wp->w_বাফার))
	    modified = TRUE;

    যদি (modified || wincount > 1)
    {
	যদি (wincount > 1)
	{
	    vim_snprintf((অক্ষর *)NameBuff, MAXPATHL, "%d", wincount);
	    len = (পূর্ণসংখ্যা)STRLEN(NameBuff);
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

    get_trans_bufname(pargs->cwp->w_বাফার);
    shorten_dir(NameBuff);
    len = (পূর্ণসংখ্যা)STRLEN(NameBuff);
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
	পূর্ণসংখ্যা	    tplmode,
	char_u	    *buf,
	stl_hlrec_T *hltab,
	tabpanel_T  *pargs)
{
    char_u	*p;
    পূর্ণসংখ্যা		curattr;
    পূর্ণসংখ্যা		n;

    curattr = pargs->attr;
    p = buf;
    জন্য (n = 0; hltab[n].start != ফাঁকা; n++)
    {
	screen_puts_len_for_tabpanel(tplmode, p, (পূর্ণসংখ্যা)(hltab[n].start - p),
		curattr, pargs);
	p = hltab[n].start;
	যদি (hltab[n].userhl == 0)
	    curattr = pargs->attr;
	অন্যথা যদি (hltab[n].userhl < 0)
	    curattr = syn_id2attr(-hltab[n].userhl);
#ifdef FEAT_TERMINAL
	অন্যথা যদি (pargs->wp != ফাঁকা && pargs->wp != curwin
		&& bt_terminal(pargs->wp->w_বাফার)
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stltermnc[hltab[n].userhl - 1];
	অন্যথা যদি (pargs->wp != ফাঁকা && bt_terminal(pargs->wp->w_বাফার)
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stlterm[hltab[n].userhl - 1];
#endif
	অন্যথা যদি (pargs->wp != ফাঁকা && pargs->wp != curwin
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stlnc[hltab[n].userhl - 1];
	অন্যথা
	    curattr = highlight_user[hltab[n].userhl - 1];
    }
    screen_puts_len_for_tabpanel(tplmode, p, (পূর্ণসংখ্যা)STRLEN(p), curattr, pargs);

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
	পূর্ণসংখ্যা	tplmode,
	পূর্ণসংখ্যা	col_start,
	পূর্ণসংখ্যা	col_end,
	পূর্ণসংখ্যা	*pcurtab_row,
	পূর্ণসংখ্যা	*ptabpagenr)
{
    পূর্ণসংখ্যা		attr_tplf = HL_ATTR(HLF_TPLF);
    পূর্ণসংখ্যা		attr_tpls = HL_ATTR(HLF_TPLS);
    পূর্ণসংখ্যা		attr_tpl = HL_ATTR(HLF_TPL);
    পূর্ণসংখ্যা		col = col_start;
    পূর্ণসংখ্যা		row = 0;
    tabpage_T	*tp = ফাঁকা;
    typval_T	v;
    tabpanel_T	args;

    args.maxrow = Rows;
    args.offsetrow = 0;
    args.col_start = col_start;
    args.col_end = col_end;

    যদি (tplmode != TPLMODE_GET_CURTAB_ROW && args.maxrow > 0)
	args.offsetrow = tpl_scroll_offset;

    tp = first_tabpage;

    জন্য (row = 0; tp != ফাঁকা; row++)
    {
	যদি (tplmode != TPLMODE_GET_CURTAB_ROW
		&& args.maxrow <= row - args.offsetrow)
	    বিরতি;

	col = col_start;

	v.v_type = VAR_সংখ্যা;
	v.vval.v_সংখ্যা = tabpage_সূচক(tp);
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

	যদি (usefmt != ফাঁকা && *usefmt != NUL)
	{
	    পূর্ণসংখ্যা	carry_hl = 0;

	    যখন (*usefmt != NUL)
	    {
		char_u	buf[IOSIZE];
		stl_hlrec_T	*hltab;
		stl_hlrec_T	*tabtab;
		stl_clickrec_T	*clicktab = ফাঁকা;

		যদি (tplmode != TPLMODE_GET_CURTAB_ROW
			&& args.maxrow <= row - args.offsetrow)
		    বিরতি;

		buf[0] = NUL;
#ifdef ENABLE_STL_MODE_MULTI_NL
		(শূন্য)build_stl_str_hl_mline_nl
#অন্যথা
		(শূন্য)build_stl_str_hl_mline
#endif
			(args.cwp, buf, আকার(buf),
			&usefmt, opt_name, opt_scope, TPL_FILLCHAR,
			args.col_end - args.col_start, &hltab, &tabtab,
			tplmode == TPLMODE_REDRAW ? &clicktab : ফাঁকা,
			&carry_hl);

		args.prow = &row;
		args.pcol = &col;

		draw_tabpanel_with_highlight(tplmode, buf, hltab, &args);

		// Record any %[FuncName] click regions for this line once
		// the text has been drawn.  Only visible rows participate.
		যদি (tplmode == TPLMODE_REDRAW && clicktab != ফাঁকা)
		{
		    পূর্ণসংখ্যা screen_row = row - args.offsetrow;
		    পূর্ণসংখ্যা m;

		    যদি (screen_row >= 0 && screen_row < args.maxrow)
			tabpanel_append_click_regions(clicktab, buf,
				screen_row, args.col_start, args.col_end,
				(পূর্ণসংখ্যা)v.vval.v_সংখ্যা);
		    // We took ownership of the click records — free the
		    // function names (matches the non-NULL clicktab path in
		    // build_stl_str_hl()).
		    জন্য (m = 0; clicktab[m].start != ফাঁকা; m++)
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
	    args.user_defined = ফাঁকা;
	    args.prow = &row;
	    args.pcol = &col;
	    draw_tabpanel_default(tplmode, &args);
	}

	vim_free(usefmt);
	do_unlet((char_u *)"g:actual_curtabpage", TRUE);

	tp = tp->tp_পরবর্তী;

	যদি ((tplmode == TPLMODE_GET_TABPAGENR)
		&& (mouse_row <= (row - args.offsetrow)))
	{
	    *ptabpagenr = v.vval.v_সংখ্যা;
	    বিরতি;
	}
    }

    // fill the area of TabPanelFill.
    screen_fill_tailing_area(tplmode, MAX(row - args.offsetrow, 0), args.maxrow,
	    args.col_start, args.col_end, attr_tplf);

    // Capture the true content height during the GET_CURTAB_ROW pass, which
    // ignores maxrow and therefore walks every tab.  REDRAW stops at the
    // visible edge so its "row" is clamped and unusable here.
    যদি (tplmode == TPLMODE_GET_CURTAB_ROW)
	tpl_যোগ_rows = row;
}

/*
 * Draw the tabpanel scrollbar (track + thumb) at screen column 'screen_col'.
 * The scrollbar spans the full screen height.  The thumb position and size
 * are derived from tpl_scroll_offset, tpl_total_rows and Rows.
 */
    স্থির শূন্য
draw_tabpanel_scrollbar(পূর্ণসংখ্যা screen_col)
{
    পূর্ণসংখ্যা attr_sb = HL_ATTR(HLF_PSB);
    পূর্ণসংখ্যা attr_thumb = HL_ATTR(HLF_PST);
    পূর্ণসংখ্যা thumb_শীর্ষ = 0;
    পূর্ণসংখ্যা thumb_height = 0;

    যদি (tpl_যোগ_rows > Rows && Rows > 0)
    {
	পূর্ণসংখ্যা max_offset = tpl_যোগ_rows - Rows;
	পূর্ণসংখ্যা track_range;

	thumb_height = Rows * Rows / tpl_যোগ_rows;
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

    জন্য (পূর্ণসংখ্যা r = 0; r < Rows; r++)
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
	ফেরত false;
    ফেরত mouse_col < firstwin->w_wincol
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
    ফেরত tpl_scrollbar && tpl_scrollbar_col >= 0
	&& mouse_col == tpl_scrollbar_col;
}

/*
 * Move the scrollbar thumb so it is vertically centred on screen row
 * 'screen_row', updating tpl_scroll_offset accordingly.  Used for both
 * initial clicks and subsequent drag events.
 * Returns true if the event was consumed (offset changed or not).
 */
    bool
tabpanel_drag_scrollbar(পূর্ণসংখ্যা screen_row)
{
    পূর্ণসংখ্যা thumb_height;
    পূর্ণসংখ্যা max_offset;
    পূর্ণসংখ্যা track_range;
    পূর্ণসংখ্যা thumb_শীর্ষ;
    পূর্ণসংখ্যা new_offset;

    যদি (!tpl_scrollbar || Rows <= 0 || tpl_যোগ_rows <= Rows)
	ফেরত false;

    thumb_height = Rows * Rows / tpl_যোগ_rows;
    যদি (thumb_height < 1)
	thumb_height = 1;
    track_range = Rows - thumb_height;
    যদি (track_range <= 0)
	ফেরত true;

    max_offset = tpl_যোগ_rows - Rows;
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
    ফেরত true;
}

/*
 * Scroll the tabpanel by 'count' rows in direction 'dir' (1 = down, -1 = up).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_scroll(পূর্ণসংখ্যা dir, পূর্ণসংখ্যা গুনতি)
{
    পূর্ণসংখ্যা max_offset;
    পূর্ণসংখ্যা new_offset;

    যদি (tabpanel_width() == 0)
	ফেরত false;

    max_offset = tpl_যোগ_rows - Rows;
    যদি (max_offset < 0)
	max_offset = 0;

    new_offset = tpl_scroll_offset + (dir > 0 ? গুনতি : -গুনতি);
    যদি (new_offset < 0)
	new_offset = 0;
    যদি (new_offset > max_offset)
	new_offset = max_offset;
    যদি (new_offset == tpl_scroll_offset)
	ফেরত false;

    tpl_scroll_offset = new_offset;
    redraw_tabpanel = TRUE;
    ফেরত true;
}

/*
 * Set the tabpanel scroll offset to "offset" (clamped to the valid range).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_set_offset(পূর্ণসংখ্যা offset)
{
    পূর্ণসংখ্যা max_offset;

    যদি (tabpanel_width() == 0)
	ফেরত false;

    max_offset = tpl_যোগ_rows - Rows;
    যদি (max_offset < 0)
	max_offset = 0;

    যদি (offset < 0)
	offset = 0;
    যদি (offset > max_offset)
	offset = max_offset;
    যদি (offset == tpl_scroll_offset)
	ফেরত false;

    tpl_scroll_offset = offset;
    redraw_tabpanel = TRUE;
    ফেরত true;
}

/*
 * "tabpanel_getinfo()" function
 */
    শূন্য
f_tabpanel_getinfo(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    পূর্ণসংখ্যা		max_offset;

    যদি (rettv_dict_alloc(rettv) == FAIL)
	ফেরত;
    d = rettv->vval.v_dict;

    max_offset = tpl_যোগ_rows - Rows;
    যদি (max_offset < 0)
	max_offset = 0;

    dict_add_string(d, "align",
	    (char_u *)(tpl_align == ALIGN_ডান ? "right" : "left"));
    dict_add_সংখ্যা(d, "columns", tabpanel_width());
    dict_add_bool(d, "scrollbar", tpl_scrollbar);
    dict_add_সংখ্যা(d, "offset", tpl_scroll_offset);
    dict_add_সংখ্যা(d, "total", tpl_যোগ_rows);
    dict_add_সংখ্যা(d, "max_offset", max_offset);
}

/*
 * "tabpanel_scroll()" function
 */
    শূন্য
f_tabpanel_scroll(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	n;
    পূর্ণসংখ্যা		absolute = 0;
    bool	changed;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_সংখ্যা = VVAL_FALSE;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ফেরত;

    n = tv_get_সংখ্যা_chk(&argvars[0], ফাঁকা);
    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	যদি (argvars[1].v_type != VAR_DICT || argvars[1].vval.v_dict == ফাঁকা)
	{
	    emsg(_(e_dictionary_required));
	    ফেরত;
	}
	absolute = dict_get_bool(argvars[1].vval.v_dict, "absolute", FALSE);
    }

    // Clamp to int range to avoid signed overflow when casting and negating.
    যদি (n > INT_MAX)
	n = INT_MAX;
    অন্যথা যদি (n < -INT_MAX)
	n = -INT_MAX;

    যদি (absolute)
	changed = tabpanel_set_offset((পূর্ণসংখ্যা)n);
    অন্যথা
	changed = tabpanel_scroll(n >= 0 ? 1 : -1,
				  (পূর্ণসংখ্যা)(n >= 0 ? n : -n));

    rettv->vval.v_সংখ্যা = changed ? VVAL_TRUE : VVAL_FALSE;
}

#endif // FEAT_TABPANEL
