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

#شامل "vim.h"

#اگر defined(FEAT_TABPANEL)

ایستا تهی do_by_tplmode(صحیح tplmode, صحیح col_start, صحیح col_end,
	صحیح *pcurtab_row, صحیح *ptabpagenr);
ایستا تهی tabpanel_free_click_regions(تهی);
ایستا تهی tabpanel_append_click_regions(stl_clickrec_T *clicktab,
	char_u *buf, صحیح row, صحیح col_start, صحیح col_end, صحیح tabnr);
ایستا تهی draw_tabpanel_scrollbar(صحیح screen_col);

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
#define ALIGN_چپ		0
#define ALIGN_راست		1

ایستا char_u *opt_name = (char_u *)"tabpanel";
ایستا صحیح opt_scope = OPT_LOCAL;
ایستا صحیح tpl_align = ALIGN_چپ;
ایستا صحیح tpl_columns = 20;
ایستا bool tpl_is_vert = false;
ایستا bool tpl_scrollbar = false;
ایستا صحیح tpl_scroll_offset = 0;
ایستا صحیح tpl_total_rows = 0;
ایستا صحیح tpl_scrollbar_col = -1;  // screen column of scrollbar, -1 if none
ایستا tabpage_T *tpl_last_curtab = پوچ;   // last curtab seen by draw_tabpanel

تعریفنوع ساختار {
    win_T   *wp;
    win_T   *cwp;
    char_u  *user_defined;
    صحیح	maxrow;
    صحیح	offsetrow;
    صحیح	*prow;
    صحیح	*pcol;
    صحیح	attr;
    صحیح	col_start;
    صحیح	col_end;
} tabpanel_T;

    صحیح
tabpanelopt_changed(تهی)
{
    char_u	*p;
    صحیح		new_align = ALIGN_چپ;
    long	new_columns = 20;
    bool	new_is_vert = false;
    bool	new_scrollbar = false;

    p = p_tplo;
    وقتی (*p != NUL)
    {
	اگر (STRNCMP(p, "align:", 6) == 0)
	{
	    p += 6;
	    اگر (STRNCMP(p, "left", 4) == 0)
	    {
		p += 4;
		new_align = ALIGN_چپ;
	    }
	    وگرنه اگر (STRNCMP(p, "right", 5) == 0)
	    {
		p += 5;
		new_align = ALIGN_راست;
	    }
	    وگرنه
		بازگشت FAIL;
	}
	وگرنه اگر (STRNCMP(p, "columns:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    new_columns = getdigits(&p);
	    اگر (new_columns < 0 || new_columns > 1000)
		بازگشت FAIL;
	}
	وگرنه اگر (STRNCMP(p, "vert", 4) == 0)
	{
	    p += 4;
	    new_is_vert = true;
	}
	وگرنه اگر (STRNCMP(p, "scrollbar", 9) == 0)
	{
	    p += 9;
	    new_scrollbar = true;
	}

	اگر (*p != ',' && *p != NUL)
	    بازگشت FAIL;
	اگر (*p == ',')
	    ++p;
    }

    tpl_align = new_align;
    tpl_columns = new_columns;
    tpl_is_vert = new_is_vert;
    tpl_scrollbar = new_scrollbar;

    // Re-center the current tab on the next redraw.
    tpl_last_curtab = پوچ;

    shell_new_columns();
    بازگشت OK;
}

/*
 * Drop any internal reference to "tp", so draw_tabpanel() never compares
 * against a dangling pointer after the tabpage has been freed.
 */
    تهی
tabpanel_forget_tabpage(ثابت tabpage_T *tp)
{
    اگر (tpl_last_curtab == tp)
	tpl_last_curtab = پوچ;
}

/*
 * Return the width of tabpanel.
 */
    صحیح
tabpanel_width(تهی)
{
    انتخاب (p_stpl)
    {
	حالت 0:
	    بازگشت 0;
	حالت 1:
	    اگر (first_tabpage->tp_next == پوچ)
		بازگشت 0;
    }
    اگر (Columns < tpl_columns)
	بازگشت 0;
    وگرنه
	بازگشت tpl_columns;
}

/*
 * Return the offset of a window considering the width of tabpanel.
 */
    صحیح
tabpanel_leftcol(تهی)
{
    بازگشت tpl_align == ALIGN_راست ? 0 : tabpanel_width();
}

/*
 * Free previously resolved 'tabpanel' click regions.
 */
    ایستا تهی
tabpanel_free_click_regions(تهی)
{
    صحیح n;

    اگر (tabpanel_stl_click != پوچ)
    {
	برای (n = 0; n < tabpanel_stl_click_شمار; n++)
	    vim_free(tabpanel_stl_click[n].funcname);
	VIM_CLEAR(tabpanel_stl_click);
    }
    tabpanel_stl_click_شمار = 0;
}

/*
 * Convert click records produced by build_stl_str_hl() for one line of
 * 'tabpanel' into screen-column based regions and append them to the global
 * tabpanel_stl_click array.  The caller keeps ownership of the funcname
 * strings inside "clicktab" — this function makes its own copies.
 */
    ایستا تهی
tabpanel_append_click_regions(
	stl_clickrec_T	*clicktab,
	char_u		*buf,
	صحیح		row,
	صحیح		col_start,
	صحیح		col_end,
	صحیح		tabnr)
{
    صحیح		شمار = 0;
    صحیح		n;
    صحیح		base_col;
    صحیح		acc_width = 0;
    صحیح		max_w = col_end - col_start;
    char_u	*p;
    char_u	*cur_funcname = پوچ;
    صحیح		cur_minwid = 0;
    صحیح		region_start_col;
    stl_click_region_T *new_arr;
    صحیح		limit;

    اگر (clicktab == پوچ)
	بازگشت;

    برای (n = 0; clicktab[n].start != پوچ; n++)
	شمار++;
    اگر (شمار == 0)
	بازگشت;

    base_col = (tpl_align == ALIGN_راست ? topframe->fr_width : 0) + col_start;
    region_start_col = base_col;

    // Grow the global array to make room for up to "count" more regions
    // (one close for each record plus a possible trailing region).
    new_arr = vim_realloc(tabpanel_stl_click,
	    اندازه(stl_click_region_T) * (tabpanel_stl_click_شمار + شمار + 1));
    اگر (new_arr == پوچ)
	بازگشت;
    tabpanel_stl_click = new_arr;

    p = buf;
    برای (n = 0; clicktab[n].start != پوچ; n++)
    {
	acc_width += vim_strnsize(p, (صحیح)(clicktab[n].start - p));
	p = clicktab[n].start;
	limit = acc_width < max_w ? acc_width : max_w;

	اگر (cur_funcname != پوچ)
	{
	    stl_click_region_T *r =
				&tabpanel_stl_click[tabpanel_stl_click_شمار];
	    r->row = row;
	    r->col_start = region_start_col;
	    r->col_end = base_col + limit;
	    r->funcname = vim_strsave(cur_funcname);
	    r->minwid = cur_minwid;
	    r->tabnr = tabnr;
	    tabpanel_stl_click_شمار++;
	}

	cur_funcname = clicktab[n].funcname;
	cur_minwid = clicktab[n].minwid;
	region_start_col = base_col + limit;
    }

    // Close the final region if it extends to the end.
    اگر (cur_funcname != پوچ)
    {
	stl_click_region_T *r = &tabpanel_stl_click[tabpanel_stl_click_شمار];
	r->row = row;
	r->col_start = region_start_col;
	r->col_end = base_col + max_w;
	r->funcname = vim_strsave(cur_funcname);
	r->minwid = cur_minwid;
	r->tabnr = tabnr;
	tabpanel_stl_click_شمار++;
    }
}

/*
 * Ensure the current tab is visible by adjusting tpl_scroll_offset when
 * the selected tab has changed since the previous redraw.  Mouse wheel or
 * scrollbar drag operations leave curtab unchanged, so the user's chosen
 * offset is preserved in those cases.
 */
    ایستا تهی
follow_curtab_if_needed(صحیح curtab_row)
{
    اگر (Rows <= 0 || curtab == tpl_last_curtab)
	بازگشت;

    اگر (curtab_row < tpl_scroll_offset)
	tpl_scroll_offset = curtab_row;
    وگرنه اگر (curtab_row >= tpl_scroll_offset + Rows)
	tpl_scroll_offset = curtab_row - Rows + 1;

    صحیح max_offset = tpl_total_rows > Rows ? tpl_total_rows - Rows : 0;

    اگر (tpl_scroll_offset < 0)
	tpl_scroll_offset = 0;
    وگرنه اگر (tpl_scroll_offset > max_offset)
	tpl_scroll_offset = max_offset;
}

/*
 * draw the tabpanel.
 */
    تهی
draw_tabpanel(تهی)
{
    صحیح saved_کلید_Typed = کلید_Typed;
    صحیح saved_got_int = got_int;
    صحیح maxwidth = tabpanel_width();
    صحیح vs_attr = HL_ATTR(HLF_C);
    صحیح curtab_row = 0;
    صحیح vsrow = 0;
    صحیح is_راست = tpl_align == ALIGN_راست;

    اگر (maxwidth == 0)
    {
	tabpanel_free_click_regions();
	بازگشت;
    }

    // Discard old click regions — they'll be rebuilt during redraw below.
    tabpanel_free_click_regions();

    // Reset got_int to avoid build_stl_str_hl() isn't evaluated.
    got_int = FALSE;

    صحیح sb_len = tpl_scrollbar ? SCROLL_LEN : 0;
    صحیح sb_screen_col = -1;

    // The scrollbar is always placed at the right edge of the tabpanel,
    // regardless of 'align'.  The vertical separator sits at the panel's
    // boundary with the buffer area (left edge for align:right, right edge
    // for align:left).
    اگر (tpl_is_vert)
    {
	اگر (is_راست)
	{
	    // Panel on the right: vert at panel's left edge, scrollbar at
	    // panel's right edge (= screen's right edge).
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, VERT_LEN,
		    maxwidth - sb_len, &curtab_row, پوچ);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, VERT_LEN, maxwidth - sb_len,
		    &curtab_row, پوچ);
	    برای (vsrow = 0; vsrow < Rows; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			topframe->fr_width, vs_attr);
	    اگر (tpl_scrollbar)
		sb_screen_col = topframe->fr_width + maxwidth - SCROLL_LEN;
	}
	وگرنه
	{
	    // Panel on the left: scrollbar just left of vert, vert at
	    // panel's right edge (boundary with buffer).
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0,
		    maxwidth - VERT_LEN - sb_len, &curtab_row, پوچ);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - VERT_LEN - sb_len,
		    &curtab_row, پوچ);
	    برای (vsrow = 0; vsrow < Rows; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			maxwidth - VERT_LEN, vs_attr);
	    اگر (tpl_scrollbar)
		sb_screen_col = maxwidth - VERT_LEN - SCROLL_LEN;
	}
    }
    وگرنه
    {
	اگر (is_راست)
	{
	    // Panel on the right, no vert: scrollbar at screen's right edge.
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth - sb_len,
		    &curtab_row, پوچ);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_row, پوچ);
	    اگر (tpl_scrollbar)
		sb_screen_col = topframe->fr_width + maxwidth - SCROLL_LEN;
	}
	وگرنه
	{
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth - sb_len,
		    &curtab_row, پوچ);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_row, پوچ);
	    اگر (tpl_scrollbar)
		sb_screen_col = maxwidth - SCROLL_LEN;
	}
    }

    tpl_scrollbar_col = sb_screen_col;
    اگر (sb_screen_col >= 0)
	draw_tabpanel_scrollbar(sb_screen_col);

    got_int |= saved_got_int;

    // A user function may reset KeyTyped, restore it.
    کلید_Typed = saved_کلید_Typed;

    tpl_last_curtab = curtab;
    redraw_tabpanel = FALSE;
}

/*
 * Return tabpagenr when clicking and dragging in tabpanel.
 */
    صحیح
get_tabpagenr_on_tabpanel(تهی)
{
    صحیح		maxwidth = tabpanel_width();
    صحیح		curtab_row = 0;
    صحیح		tabpagenr = 0;

    اگر (maxwidth == 0)
	بازگشت -1;

    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth, &curtab_row, پوچ);
    do_by_tplmode(TPLMODE_GET_TABPAGENR, 0, maxwidth, &curtab_row,
	    &tabpagenr);

    بازگشت tabpagenr;
}

/*
 * Fill tailing area between {start_row} and {end_row - 1}.
 */
    ایستا تهی
screen_fill_tailing_area(
	صحیح	tplmode,
	صحیح	row_start,
	صحیح	row_end,
	صحیح	col_start,
	صحیح	col_end,
	صحیح	attr)
{
    صحیح is_راست = tpl_align == ALIGN_راست;
    اگر (tplmode == TPLMODE_REDRAW)
	screen_fill(row_start, row_end,
		(is_راست ? topframe->fr_width : 0) + col_start,
		(is_راست ? topframe->fr_width : 0) + col_end,
		TPL_FILLCHAR, TPL_FILLCHAR, attr);
}

/*
 * screen_puts_len() for tabpanel.
 */
    ایستا تهی
screen_puts_len_for_tabpanel(
	صحیح	    tplmode,
	char_u	    *p,
	صحیح	    len,
	صحیح	    attr,
	tabpanel_T  *pargs)
{
    صحیح		j;
    صحیح		chlen;
    صحیح		chcells;
    char_u	buf[IOSIZE];
    char_u	*temp;

    برای (j = 0; j < len;)
    {
	اگر (tplmode != TPLMODE_GET_CURTAB_ROW
		&& pargs->maxrow <= *pargs->prow - pargs->offsetrow)
	    بشکن;

	اگر (has_mbyte)
	    chlen = (*mb_ptr2len)(p + j);
	وگرنه
	    chlen = 1;

	برای (صحیح k = 0; k < chlen; k++)
	    buf[k] = p[j + k];
	buf[chlen] = NUL;
	j += chlen;

	// Make all characters printable.
	temp = transstr(buf);
	اگر (temp != پوچ)
	{
	    vim_strncpy(buf, temp, اندازه(buf) - 1);
	    vim_free(temp);
	}

	اگر (has_mbyte)
	    chcells = (*mb_ptr2cells)(buf);
	وگرنه
	    chcells = 1;

	اگر (pargs->col_end < (*pargs->pcol) + chcells)
	{
	    // fill the tailing area of current row.
	    اگر (*pargs->prow - pargs->offsetrow >= 0
		    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
		screen_fill_tailing_area(tplmode,
			*pargs->prow - pargs->offsetrow,
			*pargs->prow - pargs->offsetrow + 1,
			*pargs->pcol, pargs->col_end, attr);
	    *pargs->pcol = pargs->col_end;

	    اگر (pargs->col_end < chcells)
		بشکن;
	}

	اگر (*pargs->pcol + chcells <= pargs->col_end)
	{
	    صحیح off = (tpl_align == ALIGN_راست)
		    ? topframe->fr_width
		    : 0;
	    اگر (tplmode == TPLMODE_REDRAW
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
    ایستا تهی
draw_tabpanel_default(صحیح tplmode, tabpanel_T *pargs)
{
    صحیح		modified;
    صحیح		wincount;
    صحیح		len = 0;
    char_u	buf[2] = { NUL, NUL };

    modified = FALSE;
    برای (wincount = 0; pargs->wp != پوچ;
	    pargs->wp = pargs->wp->w_next, ++wincount)
	اگر (bufIsChanged(pargs->wp->w_buffer))
	    modified = TRUE;

    اگر (modified || wincount > 1)
    {
	اگر (wincount > 1)
	{
	    vim_snprintf((نویسه *)NameBuff, MAXPATHL, "%d", wincount);
	    len = (صحیح)STRLEN(NameBuff);
	    screen_puts_len_for_tabpanel(tplmode, NameBuff, len,
#اگر defined(FEAT_SYN_HL)
		    hl_combine_attr(pargs->attr, HL_ATTR(HLF_T)),
#وگرنه
		    pargs->attr,
#endif
		    pargs);
	}
	اگر (modified)
	{
	    buf[0] = '+';
	    screen_puts_len_for_tabpanel(tplmode, buf, 1, pargs->attr, pargs);
	}

	buf[0] = TPL_FILLCHAR;
	screen_puts_len_for_tabpanel(tplmode, buf, 1, pargs->attr, pargs);
    }

    get_trans_bufname(pargs->cwp->w_buffer);
    shorten_dir(NameBuff);
    len = (صحیح)STRLEN(NameBuff);
    screen_puts_len_for_tabpanel(tplmode, NameBuff, len, pargs->attr, pargs);

    // fill the tailing area of current row.
    اگر (*pargs->prow - pargs->offsetrow >= 0
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
    ایستا تهی
draw_tabpanel_with_highlight(
	صحیح	    tplmode,
	char_u	    *buf,
	stl_hlrec_T *hltab,
	tabpanel_T  *pargs)
{
    char_u	*p;
    صحیح		curattr;
    صحیح		n;

    curattr = pargs->attr;
    p = buf;
    برای (n = 0; hltab[n].start != پوچ; n++)
    {
	screen_puts_len_for_tabpanel(tplmode, p, (صحیح)(hltab[n].start - p),
		curattr, pargs);
	p = hltab[n].start;
	اگر (hltab[n].userhl == 0)
	    curattr = pargs->attr;
	وگرنه اگر (hltab[n].userhl < 0)
	    curattr = syn_id2attr(-hltab[n].userhl);
#ifdef FEAT_TERMINAL
	وگرنه اگر (pargs->wp != پوچ && pargs->wp != curwin
		&& bt_terminal(pargs->wp->w_buffer)
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stltermnc[hltab[n].userhl - 1];
	وگرنه اگر (pargs->wp != پوچ && bt_terminal(pargs->wp->w_buffer)
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stlterm[hltab[n].userhl - 1];
#endif
	وگرنه اگر (pargs->wp != پوچ && pargs->wp != curwin
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stlnc[hltab[n].userhl - 1];
	وگرنه
	    curattr = highlight_user[hltab[n].userhl - 1];
    }
    screen_puts_len_for_tabpanel(tplmode, p, (صحیح)STRLEN(p), curattr, pargs);

    // fill the tailing area of current row.
    اگر (*pargs->prow - pargs->offsetrow >= 0
	    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
	screen_fill_tailing_area(tplmode, *pargs->prow - pargs->offsetrow,
		*pargs->prow - pargs->offsetrow + 1, *pargs->pcol,
		pargs->col_end, curattr);
    *pargs->pcol = pargs->col_end;
}

/*
 * do something by tplmode for drawing tabpanel.
 */
    ایستا تهی
do_by_tplmode(
	صحیح	tplmode,
	صحیح	col_start,
	صحیح	col_end,
	صحیح	*pcurtab_row,
	صحیح	*ptabpagenr)
{
    صحیح		attr_tplf = HL_ATTR(HLF_TPLF);
    صحیح		attr_tpls = HL_ATTR(HLF_TPLS);
    صحیح		attr_tpl = HL_ATTR(HLF_TPL);
    صحیح		col = col_start;
    صحیح		row = 0;
    tabpage_T	*tp = پوچ;
    typval_T	v;
    tabpanel_T	args;

    args.maxrow = Rows;
    args.offsetrow = 0;
    args.col_start = col_start;
    args.col_end = col_end;

    اگر (tplmode != TPLMODE_GET_CURTAB_ROW && args.maxrow > 0)
	args.offsetrow = tpl_scroll_offset;

    tp = first_tabpage;

    برای (row = 0; tp != پوچ; row++)
    {
	اگر (tplmode != TPLMODE_GET_CURTAB_ROW
		&& args.maxrow <= row - args.offsetrow)
	    بشکن;

	col = col_start;

	v.v_type = VAR_عدد;
	v.vval.v_عدد = tabpage_index(tp);
	set_var((char_u *)"g:actual_curtabpage", &v, TRUE);

	اگر (tp->tp_topframe == topframe)
	{
	    args.attr = attr_tpls;
	    اگر (tplmode == TPLMODE_GET_CURTAB_ROW)
		// Capture the row of the current tab and keep iterating so
		// tpl_total_rows receives the true content height below.
		*pcurtab_row = row;
	}
	وگرنه
	    args.attr = attr_tpl;

	اگر (tp == curtab)
	{
	    args.cwp = curwin;
	    args.wp = firstwin;
	}
	وگرنه
	{
	    args.cwp = tp->tp_curwin;
	    args.wp = tp->tp_firstwin;
	}

	char_u	*usefmt = vim_strsave(p_tpl);

	اگر (usefmt != پوچ && *usefmt != NUL)
	{
	    صحیح	carry_hl = 0;

	    وقتی (*usefmt != NUL)
	    {
		char_u	buf[IOSIZE];
		stl_hlrec_T	*hltab;
		stl_hlrec_T	*tabtab;
		stl_clickrec_T	*clicktab = پوچ;

		اگر (tplmode != TPLMODE_GET_CURTAB_ROW
			&& args.maxrow <= row - args.offsetrow)
		    بشکن;

		buf[0] = NUL;
#ifdef ENABLE_STL_MODE_MULTI_NL
		(تهی)build_stl_str_hl_mline_nl
#وگرنه
		(تهی)build_stl_str_hl_mline
#endif
			(args.cwp, buf, اندازه(buf),
			&usefmt, opt_name, opt_scope, TPL_FILLCHAR,
			args.col_end - args.col_start, &hltab, &tabtab,
			tplmode == TPLMODE_REDRAW ? &clicktab : پوچ,
			&carry_hl);

		args.prow = &row;
		args.pcol = &col;

		draw_tabpanel_with_highlight(tplmode, buf, hltab, &args);

		// Record any %[FuncName] click regions for this line once
		// the text has been drawn.  Only visible rows participate.
		اگر (tplmode == TPLMODE_REDRAW && clicktab != پوچ)
		{
		    صحیح screen_row = row - args.offsetrow;
		    صحیح m;

		    اگر (screen_row >= 0 && screen_row < args.maxrow)
			tabpanel_append_click_regions(clicktab, buf,
				screen_row, args.col_start, args.col_end,
				(صحیح)v.vval.v_عدد);
		    // We took ownership of the click records — free the
		    // function names (matches the non-NULL clicktab path in
		    // build_stl_str_hl()).
		    برای (m = 0; clicktab[m].start != پوچ; m++)
			vim_free(clicktab[m].funcname);
		}

		// Move to next line for %@
		اگر (*usefmt != NUL)
		{
		    row++;
		    col = col_start;
		}
	    }
	}
	وگرنه
	{
	    args.user_defined = پوچ;
	    args.prow = &row;
	    args.pcol = &col;
	    draw_tabpanel_default(tplmode, &args);
	}

	vim_free(usefmt);
	do_unlet((char_u *)"g:actual_curtabpage", TRUE);

	tp = tp->tp_next;

	اگر ((tplmode == TPLMODE_GET_TABPAGENR)
		&& (mouse_row <= (row - args.offsetrow)))
	{
	    *ptabpagenr = v.vval.v_عدد;
	    بشکن;
	}
    }

    // fill the area of TabPanelFill.
    screen_fill_tailing_area(tplmode, MAX(row - args.offsetrow, 0), args.maxrow,
	    args.col_start, args.col_end, attr_tplf);

    // Capture the true content height during the GET_CURTAB_ROW pass, which
    // ignores maxrow and therefore walks every tab.  REDRAW stops at the
    // visible edge so its "row" is clamped and unusable here.
    اگر (tplmode == TPLMODE_GET_CURTAB_ROW)
	tpl_total_rows = row;
}

/*
 * Draw the tabpanel scrollbar (track + thumb) at screen column 'screen_col'.
 * The scrollbar spans the full screen height.  The thumb position and size
 * are derived from tpl_scroll_offset, tpl_total_rows and Rows.
 */
    ایستا تهی
draw_tabpanel_scrollbar(صحیح screen_col)
{
    صحیح attr_sb = HL_ATTR(HLF_PSB);
    صحیح attr_thumb = HL_ATTR(HLF_PST);
    صحیح thumb_بالاترین = 0;
    صحیح thumb_height = 0;

    اگر (tpl_total_rows > Rows && Rows > 0)
    {
	صحیح max_offset = tpl_total_rows - Rows;
	صحیح track_range;

	thumb_height = Rows * Rows / tpl_total_rows;
	اگر (thumb_height < 1)
	    thumb_height = 1;

	// Map tpl_scroll_offset onto the track: at offset 0 the thumb's top
	// is at row 0, at the maximum offset its bottom reaches the last
	// row.  This is the exact inverse of tabpanel_drag_scrollbar().
	track_range = Rows - thumb_height;
	اگر (track_range > 0 && max_offset > 0)
	    thumb_بالاترین = track_range * tpl_scroll_offset / max_offset;
	وگرنه
	    thumb_بالاترین = 0;
	اگر (thumb_بالاترین + thumb_height > Rows)
	    thumb_بالاترین = Rows - thumb_height;
	اگر (thumb_بالاترین < 0)
	    thumb_بالاترین = 0;
    }

    برای (صحیح r = 0; r < Rows; r++)
    {
	bool on_thumb = thumb_height > 0
	    && r >= thumb_بالاترین && r < thumb_بالاترین + thumb_height;
	screen_putchar(TPL_FILLCHAR, r, screen_col,
		on_thumb ? attr_thumb : attr_sb);
    }
}

/*
 * Return true if the mouse is currently positioned over the tabpanel area.
 */
    bool
mouse_on_tabpanel(تهی)
{
    اگر (tabpanel_width() == 0)
	بازگشت false;
    بازگشت mouse_col < firstwin->w_wincol
	|| mouse_col >= firstwin->w_wincol + topframe->fr_width;
}

/*
 * Return true if the mouse is currently on the scrollbar column.
 * The scrollbar column is tracked by draw_tabpanel() and is -1 when the
 * scrollbar is not enabled or not yet drawn.
 */
    bool
mouse_on_tabpanel_scrollbar(تهی)
{
    بازگشت tpl_scrollbar && tpl_scrollbar_col >= 0
	&& mouse_col == tpl_scrollbar_col;
}

/*
 * Move the scrollbar thumb so it is vertically centred on screen row
 * 'screen_row', updating tpl_scroll_offset accordingly.  Used for both
 * initial clicks and subsequent drag events.
 * Returns true if the event was consumed (offset changed or not).
 */
    bool
tabpanel_drag_scrollbar(صحیح screen_row)
{
    صحیح thumb_height;
    صحیح max_offset;
    صحیح track_range;
    صحیح thumb_بالاترین;
    صحیح new_offset;

    اگر (!tpl_scrollbar || Rows <= 0 || tpl_total_rows <= Rows)
	بازگشت false;

    thumb_height = Rows * Rows / tpl_total_rows;
    اگر (thumb_height < 1)
	thumb_height = 1;
    track_range = Rows - thumb_height;
    اگر (track_range <= 0)
	بازگشت true;

    max_offset = tpl_total_rows - Rows;
    thumb_بالاترین = screen_row - thumb_height / 2;
    اگر (thumb_بالاترین < 0)
	thumb_بالاترین = 0;
    اگر (thumb_بالاترین > track_range)
	thumb_بالاترین = track_range;

    new_offset = thumb_بالاترین * max_offset / track_range;
    اگر (new_offset != tpl_scroll_offset)
    {
	tpl_scroll_offset = new_offset;
	redraw_tabpanel = TRUE;
    }
    بازگشت true;
}

/*
 * Scroll the tabpanel by 'count' rows in direction 'dir' (1 = down, -1 = up).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_scroll(صحیح dir, صحیح شمار)
{
    صحیح max_offset;
    صحیح new_offset;

    اگر (tabpanel_width() == 0)
	بازگشت false;

    max_offset = tpl_total_rows - Rows;
    اگر (max_offset < 0)
	max_offset = 0;

    new_offset = tpl_scroll_offset + (dir > 0 ? شمار : -شمار);
    اگر (new_offset < 0)
	new_offset = 0;
    اگر (new_offset > max_offset)
	new_offset = max_offset;
    اگر (new_offset == tpl_scroll_offset)
	بازگشت false;

    tpl_scroll_offset = new_offset;
    redraw_tabpanel = TRUE;
    بازگشت true;
}

/*
 * Set the tabpanel scroll offset to "offset" (clamped to the valid range).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_set_offset(صحیح offset)
{
    صحیح max_offset;

    اگر (tabpanel_width() == 0)
	بازگشت false;

    max_offset = tpl_total_rows - Rows;
    اگر (max_offset < 0)
	max_offset = 0;

    اگر (offset < 0)
	offset = 0;
    اگر (offset > max_offset)
	offset = max_offset;
    اگر (offset == tpl_scroll_offset)
	بازگشت false;

    tpl_scroll_offset = offset;
    redraw_tabpanel = TRUE;
    بازگشت true;
}

/*
 * "tabpanel_getinfo()" function
 */
    تهی
f_tabpanel_getinfo(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    صحیح		max_offset;

    اگر (rettv_dict_alloc(rettv) == FAIL)
	بازگشت;
    d = rettv->vval.v_dict;

    max_offset = tpl_total_rows - Rows;
    اگر (max_offset < 0)
	max_offset = 0;

    dict_add_string(d, "align",
	    (char_u *)(tpl_align == ALIGN_راست ? "right" : "left"));
    dict_add_عدد(d, "columns", tabpanel_width());
    dict_add_bool(d, "scrollbar", tpl_scrollbar);
    dict_add_عدد(d, "offset", tpl_scroll_offset);
    dict_add_عدد(d, "total", tpl_total_rows);
    dict_add_عدد(d, "max_offset", max_offset);
}

/*
 * "tabpanel_scroll()" function
 */
    تهی
f_tabpanel_scroll(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	n;
    صحیح		absolute = 0;
    bool	changed;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_عدد = VVAL_FALSE;

    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	بازگشت;

    n = tv_get_عدد_chk(&argvars[0], پوچ);
    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	اگر (argvars[1].v_type != VAR_DICT || argvars[1].vval.v_dict == پوچ)
	{
	    emsg(_(e_dictionary_required));
	    بازگشت;
	}
	absolute = dict_get_bool(argvars[1].vval.v_dict, "absolute", FALSE);
    }

    // Clamp to int range to avoid signed overflow when casting and negating.
    اگر (n > INT_MAX)
	n = INT_MAX;
    وگرنه اگر (n < -INT_MAX)
	n = -INT_MAX;

    اگر (absolute)
	changed = tabpanel_set_offset((صحیح)n);
    وگرنه
	changed = tabpanel_scroll(n >= 0 ? 1 : -1,
				  (صحیح)(n >= 0 ? n : -n));

    rettv->vval.v_عدد = changed ? VVAL_TRUE : VVAL_FALSE;
}

#endif // FEAT_TABPANEL
