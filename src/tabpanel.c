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

#ضمن "vim.h"

#إذا defined(FEAT_TABPANEL)

ساكن فراغ do_by_tplmode(صحيح tplmode, صحيح col_start, صحيح col_end,
	صحيح *pcurtab_row, صحيح *ptabpagenr);
ساكن فراغ tabpanel_free_click_regions(فراغ);
ساكن فراغ tabpanel_append_click_regions(stl_clickrec_T *clicktab,
	char_u *buf, صحيح row, صحيح col_start, صحيح col_end, صحيح tabnr);
ساكن فراغ draw_tabpanel_scrollbar(صحيح screen_col);

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
#define ALIGN_سول		0
#define ALIGN_ئوڭ		1

ساكن char_u *opt_name = (char_u *)"tabpanel";
ساكن صحيح opt_scope = OPT_LOCAL;
ساكن صحيح tpl_align = ALIGN_سول;
ساكن صحيح tpl_columns = 20;
ساكن bool tpl_is_vert = false;
ساكن bool tpl_scrollbar = false;
ساكن صحيح tpl_scroll_offset = 0;
ساكن صحيح tpl_total_rows = 0;
ساكن صحيح tpl_scrollbar_col = -1;  // screen column of scrollbar, -1 if none
ساكن tabpage_T *tpl_last_curtab = عدم;   // last curtab seen by draw_tabpanel

تعريفنوع بنية {
    win_T   *wp;
    win_T   *cwp;
    char_u  *user_defined;
    صحيح	maxrow;
    صحيح	offsetrow;
    صحيح	*prow;
    صحيح	*pcol;
    صحيح	attr;
    صحيح	col_start;
    صحيح	col_end;
} tabpanel_T;

    صحيح
tabpanelopt_changed(فراغ)
{
    char_u	*p;
    صحيح		new_align = ALIGN_سول;
    long	new_columns = 20;
    bool	new_is_vert = false;
    bool	new_scrollbar = false;

    p = p_tplo;
    طالما (*p != NUL)
    {
	إذا (STRNCMP(p, "align:", 6) == 0)
	{
	    p += 6;
	    إذا (STRNCMP(p, "left", 4) == 0)
	    {
		p += 4;
		new_align = ALIGN_سول;
	    }
	    وإلا إذا (STRNCMP(p, "right", 5) == 0)
	    {
		p += 5;
		new_align = ALIGN_ئوڭ;
	    }
	    وإلا
		أرجع FAIL;
	}
	وإلا إذا (STRNCMP(p, "columns:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    new_columns = getdigits(&p);
	    إذا (new_columns < 0 || new_columns > 1000)
		أرجع FAIL;
	}
	وإلا إذا (STRNCMP(p, "vert", 4) == 0)
	{
	    p += 4;
	    new_is_vert = true;
	}
	وإلا إذا (STRNCMP(p, "scrollbar", 9) == 0)
	{
	    p += 9;
	    new_scrollbar = true;
	}

	إذا (*p != ',' && *p != NUL)
	    أرجع FAIL;
	إذا (*p == ',')
	    ++p;
    }

    tpl_align = new_align;
    tpl_columns = new_columns;
    tpl_is_vert = new_is_vert;
    tpl_scrollbar = new_scrollbar;

    // Re-center the current tab on the next redraw.
    tpl_last_curtab = عدم;

    shell_new_columns();
    أرجع OK;
}

/*
 * Drop any internal reference to "tp", so draw_tabpanel() never compares
 * against a dangling pointer after the tabpage has been freed.
 */
    فراغ
tabpanel_forget_tabpage(ثابت tabpage_T *tp)
{
    إذا (tpl_last_curtab == tp)
	tpl_last_curtab = عدم;
}

/*
 * Return the width of tabpanel.
 */
    صحيح
tabpanel_width(فراغ)
{
    بدل (p_stpl)
    {
	حالة 0:
	    أرجع 0;
	حالة 1:
	    إذا (first_tabpage->tp_next == عدم)
		أرجع 0;
    }
    إذا (Columns < tpl_columns)
	أرجع 0;
    وإلا
	أرجع tpl_columns;
}

/*
 * Return the offset of a window considering the width of tabpanel.
 */
    صحيح
tabpanel_leftcol(فراغ)
{
    أرجع tpl_align == ALIGN_ئوڭ ? 0 : tabpanel_width();
}

/*
 * Free previously resolved 'tabpanel' click regions.
 */
    ساكن فراغ
tabpanel_free_click_regions(فراغ)
{
    صحيح n;

    إذا (tabpanel_stl_click != عدم)
    {
	لكل (n = 0; n < tabpanel_stl_click_سانى; n++)
	    vim_free(tabpanel_stl_click[n].funcname);
	VIM_CLEAR(tabpanel_stl_click);
    }
    tabpanel_stl_click_سانى = 0;
}

/*
 * Convert click records produced by build_stl_str_hl() for one line of
 * 'tabpanel' into screen-column based regions and append them to the global
 * tabpanel_stl_click array.  The caller keeps ownership of the funcname
 * strings inside "clicktab" — this function makes its own copies.
 */
    ساكن فراغ
tabpanel_append_click_regions(
	stl_clickrec_T	*clicktab,
	char_u		*buf,
	صحيح		row,
	صحيح		col_start,
	صحيح		col_end,
	صحيح		tabnr)
{
    صحيح		سانى = 0;
    صحيح		n;
    صحيح		base_col;
    صحيح		acc_width = 0;
    صحيح		max_w = col_end - col_start;
    char_u	*p;
    char_u	*cur_funcname = عدم;
    صحيح		cur_minwid = 0;
    صحيح		region_start_col;
    stl_click_region_T *new_arr;
    صحيح		limit;

    إذا (clicktab == عدم)
	أرجع;

    لكل (n = 0; clicktab[n].start != عدم; n++)
	سانى++;
    إذا (سانى == 0)
	أرجع;

    base_col = (tpl_align == ALIGN_ئوڭ ? topframe->fr_width : 0) + col_start;
    region_start_col = base_col;

    // Grow the global array to make room for up to "count" more regions
    // (one close for each record plus a possible trailing region).
    new_arr = vim_realloc(tabpanel_stl_click,
	    حجم(stl_click_region_T) * (tabpanel_stl_click_سانى + سانى + 1));
    إذا (new_arr == عدم)
	أرجع;
    tabpanel_stl_click = new_arr;

    p = buf;
    لكل (n = 0; clicktab[n].start != عدم; n++)
    {
	acc_width += vim_strnsize(p, (صحيح)(clicktab[n].start - p));
	p = clicktab[n].start;
	limit = acc_width < max_w ? acc_width : max_w;

	إذا (cur_funcname != عدم)
	{
	    stl_click_region_T *r =
				&tabpanel_stl_click[tabpanel_stl_click_سانى];
	    r->row = row;
	    r->col_start = region_start_col;
	    r->col_end = base_col + limit;
	    r->funcname = vim_strsave(cur_funcname);
	    r->minwid = cur_minwid;
	    r->tabnr = tabnr;
	    tabpanel_stl_click_سانى++;
	}

	cur_funcname = clicktab[n].funcname;
	cur_minwid = clicktab[n].minwid;
	region_start_col = base_col + limit;
    }

    // Close the final region if it extends to the end.
    إذا (cur_funcname != عدم)
    {
	stl_click_region_T *r = &tabpanel_stl_click[tabpanel_stl_click_سانى];
	r->row = row;
	r->col_start = region_start_col;
	r->col_end = base_col + max_w;
	r->funcname = vim_strsave(cur_funcname);
	r->minwid = cur_minwid;
	r->tabnr = tabnr;
	tabpanel_stl_click_سانى++;
    }
}

/*
 * Ensure the current tab is visible by adjusting tpl_scroll_offset when
 * the selected tab has changed since the previous redraw.  Mouse wheel or
 * scrollbar drag operations leave curtab unchanged, so the user's chosen
 * offset is preserved in those cases.
 */
    ساكن فراغ
follow_curtab_if_needed(صحيح curtab_row)
{
    إذا (Rows <= 0 || curtab == tpl_last_curtab)
	أرجع;

    إذا (curtab_row < tpl_scroll_offset)
	tpl_scroll_offset = curtab_row;
    وإلا إذا (curtab_row >= tpl_scroll_offset + Rows)
	tpl_scroll_offset = curtab_row - Rows + 1;

    صحيح max_offset = tpl_total_rows > Rows ? tpl_total_rows - Rows : 0;

    إذا (tpl_scroll_offset < 0)
	tpl_scroll_offset = 0;
    وإلا إذا (tpl_scroll_offset > max_offset)
	tpl_scroll_offset = max_offset;
}

/*
 * draw the tabpanel.
 */
    فراغ
draw_tabpanel(فراغ)
{
    صحيح saved_ئاچقۇچ_Typed = ئاچقۇچ_Typed;
    صحيح saved_got_int = got_int;
    صحيح maxwidth = tabpanel_width();
    صحيح vs_attr = HL_ATTR(HLF_C);
    صحيح curtab_row = 0;
    صحيح vsrow = 0;
    صحيح is_ئوڭ = tpl_align == ALIGN_ئوڭ;

    إذا (maxwidth == 0)
    {
	tabpanel_free_click_regions();
	أرجع;
    }

    // Discard old click regions — they'll be rebuilt during redraw below.
    tabpanel_free_click_regions();

    // Reset got_int to avoid build_stl_str_hl() isn't evaluated.
    got_int = FALSE;

    صحيح sb_len = tpl_scrollbar ? SCROLL_LEN : 0;
    صحيح sb_screen_col = -1;

    // The scrollbar is always placed at the right edge of the tabpanel,
    // regardless of 'align'.  The vertical separator sits at the panel's
    // boundary with the buffer area (left edge for align:right, right edge
    // for align:left).
    إذا (tpl_is_vert)
    {
	إذا (is_ئوڭ)
	{
	    // Panel on the right: vert at panel's left edge, scrollbar at
	    // panel's right edge (= screen's right edge).
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, VERT_LEN,
		    maxwidth - sb_len, &curtab_row, عدم);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, VERT_LEN, maxwidth - sb_len,
		    &curtab_row, عدم);
	    لكل (vsrow = 0; vsrow < Rows; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			topframe->fr_width, vs_attr);
	    إذا (tpl_scrollbar)
		sb_screen_col = topframe->fr_width + maxwidth - SCROLL_LEN;
	}
	وإلا
	{
	    // Panel on the left: scrollbar just left of vert, vert at
	    // panel's right edge (boundary with buffer).
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0,
		    maxwidth - VERT_LEN - sb_len, &curtab_row, عدم);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - VERT_LEN - sb_len,
		    &curtab_row, عدم);
	    لكل (vsrow = 0; vsrow < Rows; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			maxwidth - VERT_LEN, vs_attr);
	    إذا (tpl_scrollbar)
		sb_screen_col = maxwidth - VERT_LEN - SCROLL_LEN;
	}
    }
    وإلا
    {
	إذا (is_ئوڭ)
	{
	    // Panel on the right, no vert: scrollbar at screen's right edge.
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth - sb_len,
		    &curtab_row, عدم);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_row, عدم);
	    إذا (tpl_scrollbar)
		sb_screen_col = topframe->fr_width + maxwidth - SCROLL_LEN;
	}
	وإلا
	{
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth - sb_len,
		    &curtab_row, عدم);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_row, عدم);
	    إذا (tpl_scrollbar)
		sb_screen_col = maxwidth - SCROLL_LEN;
	}
    }

    tpl_scrollbar_col = sb_screen_col;
    إذا (sb_screen_col >= 0)
	draw_tabpanel_scrollbar(sb_screen_col);

    got_int |= saved_got_int;

    // A user function may reset KeyTyped, restore it.
    ئاچقۇچ_Typed = saved_ئاچقۇچ_Typed;

    tpl_last_curtab = curtab;
    redraw_tabpanel = FALSE;
}

/*
 * Return tabpagenr when clicking and dragging in tabpanel.
 */
    صحيح
get_tabpagenr_on_tabpanel(فراغ)
{
    صحيح		maxwidth = tabpanel_width();
    صحيح		curtab_row = 0;
    صحيح		tabpagenr = 0;

    إذا (maxwidth == 0)
	أرجع -1;

    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth, &curtab_row, عدم);
    do_by_tplmode(TPLMODE_GET_TABPAGENR, 0, maxwidth, &curtab_row,
	    &tabpagenr);

    أرجع tabpagenr;
}

/*
 * Fill tailing area between {start_row} and {end_row - 1}.
 */
    ساكن فراغ
screen_fill_tailing_area(
	صحيح	tplmode,
	صحيح	row_start,
	صحيح	row_end,
	صحيح	col_start,
	صحيح	col_end,
	صحيح	attr)
{
    صحيح is_ئوڭ = tpl_align == ALIGN_ئوڭ;
    إذا (tplmode == TPLMODE_REDRAW)
	screen_fill(row_start, row_end,
		(is_ئوڭ ? topframe->fr_width : 0) + col_start,
		(is_ئوڭ ? topframe->fr_width : 0) + col_end,
		TPL_FILLCHAR, TPL_FILLCHAR, attr);
}

/*
 * screen_puts_len() for tabpanel.
 */
    ساكن فراغ
screen_puts_len_for_tabpanel(
	صحيح	    tplmode,
	char_u	    *p,
	صحيح	    len,
	صحيح	    attr,
	tabpanel_T  *pargs)
{
    صحيح		j;
    صحيح		chlen;
    صحيح		chcells;
    char_u	buf[IOSIZE];
    char_u	*temp;

    لكل (j = 0; j < len;)
    {
	إذا (tplmode != TPLMODE_GET_CURTAB_ROW
		&& pargs->maxrow <= *pargs->prow - pargs->offsetrow)
	    اكسر;

	إذا (has_mbyte)
	    chlen = (*mb_ptr2len)(p + j);
	وإلا
	    chlen = 1;

	لكل (صحيح k = 0; k < chlen; k++)
	    buf[k] = p[j + k];
	buf[chlen] = NUL;
	j += chlen;

	// Make all characters printable.
	temp = transstr(buf);
	إذا (temp != عدم)
	{
	    vim_strncpy(buf, temp, حجم(buf) - 1);
	    vim_free(temp);
	}

	إذا (has_mbyte)
	    chcells = (*mb_ptr2cells)(buf);
	وإلا
	    chcells = 1;

	إذا (pargs->col_end < (*pargs->pcol) + chcells)
	{
	    // fill the tailing area of current row.
	    إذا (*pargs->prow - pargs->offsetrow >= 0
		    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
		screen_fill_tailing_area(tplmode,
			*pargs->prow - pargs->offsetrow,
			*pargs->prow - pargs->offsetrow + 1,
			*pargs->pcol, pargs->col_end, attr);
	    *pargs->pcol = pargs->col_end;

	    إذا (pargs->col_end < chcells)
		اكسر;
	}

	إذا (*pargs->pcol + chcells <= pargs->col_end)
	{
	    صحيح off = (tpl_align == ALIGN_ئوڭ)
		    ? topframe->fr_width
		    : 0;
	    إذا (tplmode == TPLMODE_REDRAW
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
    ساكن فراغ
draw_tabpanel_default(صحيح tplmode, tabpanel_T *pargs)
{
    صحيح		modified;
    صحيح		wincount;
    صحيح		len = 0;
    char_u	buf[2] = { NUL, NUL };

    modified = FALSE;
    لكل (wincount = 0; pargs->wp != عدم;
	    pargs->wp = pargs->wp->w_next, ++wincount)
	إذا (bufIsChanged(pargs->wp->w_buffer))
	    modified = TRUE;

    إذا (modified || wincount > 1)
    {
	إذا (wincount > 1)
	{
	    vim_snprintf((حرف *)NameBuff, MAXPATHL, "%d", wincount);
	    len = (صحيح)STRLEN(NameBuff);
	    screen_puts_len_for_tabpanel(tplmode, NameBuff, len,
#إذا defined(FEAT_SYN_HL)
		    hl_combine_attr(pargs->attr, HL_ATTR(HLF_T)),
#وإلا
		    pargs->attr,
#endif
		    pargs);
	}
	إذا (modified)
	{
	    buf[0] = '+';
	    screen_puts_len_for_tabpanel(tplmode, buf, 1, pargs->attr, pargs);
	}

	buf[0] = TPL_FILLCHAR;
	screen_puts_len_for_tabpanel(tplmode, buf, 1, pargs->attr, pargs);
    }

    get_trans_bufname(pargs->cwp->w_buffer);
    shorten_dir(NameBuff);
    len = (صحيح)STRLEN(NameBuff);
    screen_puts_len_for_tabpanel(tplmode, NameBuff, len, pargs->attr, pargs);

    // fill the tailing area of current row.
    إذا (*pargs->prow - pargs->offsetrow >= 0
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
    ساكن فراغ
draw_tabpanel_with_highlight(
	صحيح	    tplmode,
	char_u	    *buf,
	stl_hlrec_T *hltab,
	tabpanel_T  *pargs)
{
    char_u	*p;
    صحيح		curattr;
    صحيح		n;

    curattr = pargs->attr;
    p = buf;
    لكل (n = 0; hltab[n].start != عدم; n++)
    {
	screen_puts_len_for_tabpanel(tplmode, p, (صحيح)(hltab[n].start - p),
		curattr, pargs);
	p = hltab[n].start;
	إذا (hltab[n].userhl == 0)
	    curattr = pargs->attr;
	وإلا إذا (hltab[n].userhl < 0)
	    curattr = syn_id2attr(-hltab[n].userhl);
#ifdef FEAT_TERMINAL
	وإلا إذا (pargs->wp != عدم && pargs->wp != curwin
		&& bt_terminal(pargs->wp->w_buffer)
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stltermnc[hltab[n].userhl - 1];
	وإلا إذا (pargs->wp != عدم && bt_terminal(pargs->wp->w_buffer)
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stlterm[hltab[n].userhl - 1];
#endif
	وإلا إذا (pargs->wp != عدم && pargs->wp != curwin
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stlnc[hltab[n].userhl - 1];
	وإلا
	    curattr = highlight_user[hltab[n].userhl - 1];
    }
    screen_puts_len_for_tabpanel(tplmode, p, (صحيح)STRLEN(p), curattr, pargs);

    // fill the tailing area of current row.
    إذا (*pargs->prow - pargs->offsetrow >= 0
	    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
	screen_fill_tailing_area(tplmode, *pargs->prow - pargs->offsetrow,
		*pargs->prow - pargs->offsetrow + 1, *pargs->pcol,
		pargs->col_end, curattr);
    *pargs->pcol = pargs->col_end;
}

/*
 * do something by tplmode for drawing tabpanel.
 */
    ساكن فراغ
do_by_tplmode(
	صحيح	tplmode,
	صحيح	col_start,
	صحيح	col_end,
	صحيح	*pcurtab_row,
	صحيح	*ptabpagenr)
{
    صحيح		attr_tplf = HL_ATTR(HLF_TPLF);
    صحيح		attr_tpls = HL_ATTR(HLF_TPLS);
    صحيح		attr_tpl = HL_ATTR(HLF_TPL);
    صحيح		col = col_start;
    صحيح		row = 0;
    tabpage_T	*tp = عدم;
    typval_T	v;
    tabpanel_T	args;

    args.maxrow = Rows;
    args.offsetrow = 0;
    args.col_start = col_start;
    args.col_end = col_end;

    إذا (tplmode != TPLMODE_GET_CURTAB_ROW && args.maxrow > 0)
	args.offsetrow = tpl_scroll_offset;

    tp = first_tabpage;

    لكل (row = 0; tp != عدم; row++)
    {
	إذا (tplmode != TPLMODE_GET_CURTAB_ROW
		&& args.maxrow <= row - args.offsetrow)
	    اكسر;

	col = col_start;

	v.v_type = VAR_سان;
	v.vval.v_سان = tabpage_index(tp);
	set_var((char_u *)"g:actual_curtabpage", &v, TRUE);

	إذا (tp->tp_topframe == topframe)
	{
	    args.attr = attr_tpls;
	    إذا (tplmode == TPLMODE_GET_CURTAB_ROW)
		// Capture the row of the current tab and keep iterating so
		// tpl_total_rows receives the true content height below.
		*pcurtab_row = row;
	}
	وإلا
	    args.attr = attr_tpl;

	إذا (tp == curtab)
	{
	    args.cwp = curwin;
	    args.wp = firstwin;
	}
	وإلا
	{
	    args.cwp = tp->tp_curwin;
	    args.wp = tp->tp_firstwin;
	}

	char_u	*usefmt = vim_strsave(p_tpl);

	إذا (usefmt != عدم && *usefmt != NUL)
	{
	    صحيح	carry_hl = 0;

	    طالما (*usefmt != NUL)
	    {
		char_u	buf[IOSIZE];
		stl_hlrec_T	*hltab;
		stl_hlrec_T	*tabtab;
		stl_clickrec_T	*clicktab = عدم;

		إذا (tplmode != TPLMODE_GET_CURTAB_ROW
			&& args.maxrow <= row - args.offsetrow)
		    اكسر;

		buf[0] = NUL;
#ifdef ENABLE_STL_MODE_MULTI_NL
		(فراغ)build_stl_str_hl_mline_nl
#وإلا
		(فراغ)build_stl_str_hl_mline
#endif
			(args.cwp, buf, حجم(buf),
			&usefmt, opt_name, opt_scope, TPL_FILLCHAR,
			args.col_end - args.col_start, &hltab, &tabtab,
			tplmode == TPLMODE_REDRAW ? &clicktab : عدم,
			&carry_hl);

		args.prow = &row;
		args.pcol = &col;

		draw_tabpanel_with_highlight(tplmode, buf, hltab, &args);

		// Record any %[FuncName] click regions for this line once
		// the text has been drawn.  Only visible rows participate.
		إذا (tplmode == TPLMODE_REDRAW && clicktab != عدم)
		{
		    صحيح screen_row = row - args.offsetrow;
		    صحيح m;

		    إذا (screen_row >= 0 && screen_row < args.maxrow)
			tabpanel_append_click_regions(clicktab, buf,
				screen_row, args.col_start, args.col_end,
				(صحيح)v.vval.v_سان);
		    // We took ownership of the click records — free the
		    // function names (matches the non-NULL clicktab path in
		    // build_stl_str_hl()).
		    لكل (m = 0; clicktab[m].start != عدم; m++)
			vim_free(clicktab[m].funcname);
		}

		// Move to next line for %@
		إذا (*usefmt != NUL)
		{
		    row++;
		    col = col_start;
		}
	    }
	}
	وإلا
	{
	    args.user_defined = عدم;
	    args.prow = &row;
	    args.pcol = &col;
	    draw_tabpanel_default(tplmode, &args);
	}

	vim_free(usefmt);
	do_unlet((char_u *)"g:actual_curtabpage", TRUE);

	tp = tp->tp_next;

	إذا ((tplmode == TPLMODE_GET_TABPAGENR)
		&& (mouse_row <= (row - args.offsetrow)))
	{
	    *ptabpagenr = v.vval.v_سان;
	    اكسر;
	}
    }

    // fill the area of TabPanelFill.
    screen_fill_tailing_area(tplmode, MAX(row - args.offsetrow, 0), args.maxrow,
	    args.col_start, args.col_end, attr_tplf);

    // Capture the true content height during the GET_CURTAB_ROW pass, which
    // ignores maxrow and therefore walks every tab.  REDRAW stops at the
    // visible edge so its "row" is clamped and unusable here.
    إذا (tplmode == TPLMODE_GET_CURTAB_ROW)
	tpl_total_rows = row;
}

/*
 * Draw the tabpanel scrollbar (track + thumb) at screen column 'screen_col'.
 * The scrollbar spans the full screen height.  The thumb position and size
 * are derived from tpl_scroll_offset, tpl_total_rows and Rows.
 */
    ساكن فراغ
draw_tabpanel_scrollbar(صحيح screen_col)
{
    صحيح attr_sb = HL_ATTR(HLF_PSB);
    صحيح attr_thumb = HL_ATTR(HLF_PST);
    صحيح thumb_چوققا = 0;
    صحيح thumb_height = 0;

    إذا (tpl_total_rows > Rows && Rows > 0)
    {
	صحيح max_offset = tpl_total_rows - Rows;
	صحيح track_range;

	thumb_height = Rows * Rows / tpl_total_rows;
	إذا (thumb_height < 1)
	    thumb_height = 1;

	// Map tpl_scroll_offset onto the track: at offset 0 the thumb's top
	// is at row 0, at the maximum offset its bottom reaches the last
	// row.  This is the exact inverse of tabpanel_drag_scrollbar().
	track_range = Rows - thumb_height;
	إذا (track_range > 0 && max_offset > 0)
	    thumb_چوققا = track_range * tpl_scroll_offset / max_offset;
	وإلا
	    thumb_چوققا = 0;
	إذا (thumb_چوققا + thumb_height > Rows)
	    thumb_چوققا = Rows - thumb_height;
	إذا (thumb_چوققا < 0)
	    thumb_چوققا = 0;
    }

    لكل (صحيح r = 0; r < Rows; r++)
    {
	bool on_thumb = thumb_height > 0
	    && r >= thumb_چوققا && r < thumb_چوققا + thumb_height;
	screen_putchar(TPL_FILLCHAR, r, screen_col,
		on_thumb ? attr_thumb : attr_sb);
    }
}

/*
 * Return true if the mouse is currently positioned over the tabpanel area.
 */
    bool
mouse_on_tabpanel(فراغ)
{
    إذا (tabpanel_width() == 0)
	أرجع false;
    أرجع mouse_col < firstwin->w_wincol
	|| mouse_col >= firstwin->w_wincol + topframe->fr_width;
}

/*
 * Return true if the mouse is currently on the scrollbar column.
 * The scrollbar column is tracked by draw_tabpanel() and is -1 when the
 * scrollbar is not enabled or not yet drawn.
 */
    bool
mouse_on_tabpanel_scrollbar(فراغ)
{
    أرجع tpl_scrollbar && tpl_scrollbar_col >= 0
	&& mouse_col == tpl_scrollbar_col;
}

/*
 * Move the scrollbar thumb so it is vertically centred on screen row
 * 'screen_row', updating tpl_scroll_offset accordingly.  Used for both
 * initial clicks and subsequent drag events.
 * Returns true if the event was consumed (offset changed or not).
 */
    bool
tabpanel_drag_scrollbar(صحيح screen_row)
{
    صحيح thumb_height;
    صحيح max_offset;
    صحيح track_range;
    صحيح thumb_چوققا;
    صحيح new_offset;

    إذا (!tpl_scrollbar || Rows <= 0 || tpl_total_rows <= Rows)
	أرجع false;

    thumb_height = Rows * Rows / tpl_total_rows;
    إذا (thumb_height < 1)
	thumb_height = 1;
    track_range = Rows - thumb_height;
    إذا (track_range <= 0)
	أرجع true;

    max_offset = tpl_total_rows - Rows;
    thumb_چوققا = screen_row - thumb_height / 2;
    إذا (thumb_چوققا < 0)
	thumb_چوققا = 0;
    إذا (thumb_چوققا > track_range)
	thumb_چوققا = track_range;

    new_offset = thumb_چوققا * max_offset / track_range;
    إذا (new_offset != tpl_scroll_offset)
    {
	tpl_scroll_offset = new_offset;
	redraw_tabpanel = TRUE;
    }
    أرجع true;
}

/*
 * Scroll the tabpanel by 'count' rows in direction 'dir' (1 = down, -1 = up).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_scroll(صحيح dir, صحيح سانى)
{
    صحيح max_offset;
    صحيح new_offset;

    إذا (tabpanel_width() == 0)
	أرجع false;

    max_offset = tpl_total_rows - Rows;
    إذا (max_offset < 0)
	max_offset = 0;

    new_offset = tpl_scroll_offset + (dir > 0 ? سانى : -سانى);
    إذا (new_offset < 0)
	new_offset = 0;
    إذا (new_offset > max_offset)
	new_offset = max_offset;
    إذا (new_offset == tpl_scroll_offset)
	أرجع false;

    tpl_scroll_offset = new_offset;
    redraw_tabpanel = TRUE;
    أرجع true;
}

/*
 * Set the tabpanel scroll offset to "offset" (clamped to the valid range).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_set_offset(صحيح offset)
{
    صحيح max_offset;

    إذا (tabpanel_width() == 0)
	أرجع false;

    max_offset = tpl_total_rows - Rows;
    إذا (max_offset < 0)
	max_offset = 0;

    إذا (offset < 0)
	offset = 0;
    إذا (offset > max_offset)
	offset = max_offset;
    إذا (offset == tpl_scroll_offset)
	أرجع false;

    tpl_scroll_offset = offset;
    redraw_tabpanel = TRUE;
    أرجع true;
}

/*
 * "tabpanel_getinfo()" function
 */
    فراغ
f_tabpanel_getinfo(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    صحيح		max_offset;

    إذا (rettv_dict_alloc(rettv) == FAIL)
	أرجع;
    d = rettv->vval.v_dict;

    max_offset = tpl_total_rows - Rows;
    إذا (max_offset < 0)
	max_offset = 0;

    dict_add_string(d, "align",
	    (char_u *)(tpl_align == ALIGN_ئوڭ ? "right" : "left"));
    dict_add_سان(d, "columns", tabpanel_width());
    dict_add_bool(d, "scrollbar", tpl_scrollbar);
    dict_add_سان(d, "offset", tpl_scroll_offset);
    dict_add_سان(d, "total", tpl_total_rows);
    dict_add_سان(d, "max_offset", max_offset);
}

/*
 * "tabpanel_scroll()" function
 */
    فراغ
f_tabpanel_scroll(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	n;
    صحيح		absolute = 0;
    bool	changed;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_سان = VVAL_FALSE;

    إذا (in_vim9script()
	    && (check_for_سان_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	أرجع;

    n = tv_get_سان_chk(&argvars[0], عدم);
    إذا (argvars[1].v_type != VAR_UNKNOWN)
    {
	إذا (argvars[1].v_type != VAR_DICT || argvars[1].vval.v_dict == عدم)
	{
	    emsg(_(e_dictionary_required));
	    أرجع;
	}
	absolute = dict_get_bool(argvars[1].vval.v_dict, "absolute", FALSE);
    }

    // Clamp to int range to avoid signed overflow when casting and negating.
    إذا (n > INT_MAX)
	n = INT_MAX;
    وإلا إذا (n < -INT_MAX)
	n = -INT_MAX;

    إذا (absolute)
	changed = tabpanel_set_offset((صحيح)n);
    وإلا
	changed = tabpanel_scroll(n >= 0 ? 1 : -1,
				  (صحيح)(n >= 0 ? n : -n));

    rettv->vval.v_سان = changed ? VVAL_TRUE : VVAL_FALSE;
}

#endif // FEAT_TABPANEL
