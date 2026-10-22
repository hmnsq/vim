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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_TABPANEL)

સ્થિર શૂન્ય do_by_tplmode(પૂર્ણાંક tplmode, પૂર્ણાંક col_start, પૂર્ણાંક col_end,
	પૂર્ણાંક *pcurtab_row, પૂર્ણાંક *ptabpagenr);
સ્થિર શૂન્ય tabpanel_free_click_regions(શૂન્ય);
સ્થિર શૂન્ય tabpanel_append_click_regions(stl_clickrec_T *clicktab,
	char_u *buf, પૂર્ણાંક row, પૂર્ણાંક col_start, પૂર્ણાંક col_end, પૂર્ણાંક tabnr);
સ્થિર શૂન્ય draw_tabpanel_scrollbar(પૂર્ણાંક screen_col);

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
#define ALIGN_ડાબું		0
#define ALIGN_જમણું		1

સ્થિર char_u *opt_name = (char_u *)"tabpanel";
સ્થિર પૂર્ણાંક opt_scope = OPT_LOCAL;
સ્થિર પૂર્ણાંક tpl_align = ALIGN_ડાબું;
સ્થિર પૂર્ણાંક tpl_columns = 20;
સ્થિર bool tpl_is_vert = false;
સ્થિર bool tpl_scrollbar = false;
સ્થિર પૂર્ણાંક tpl_scroll_offset = 0;
સ્થિર પૂર્ણાંક tpl_total_rows = 0;
સ્થિર પૂર્ણાંક tpl_scrollbar_col = -1;  // screen column of scrollbar, -1 if none
સ્થિર tabpage_T *tpl_last_curtab = ખાલી;   // last curtab seen by draw_tabpanel

પ્રકારવ્યાખ્યા સંરચના {
    win_T   *wp;
    win_T   *cwp;
    char_u  *user_defined;
    પૂર્ણાંક	maxrow;
    પૂર્ણાંક	offsetrow;
    પૂર્ણાંક	*prow;
    પૂર્ણાંક	*pcol;
    પૂર્ણાંક	attr;
    પૂર્ણાંક	col_start;
    પૂર્ણાંક	col_end;
} tabpanel_T;

    પૂર્ણાંક
tabpanelopt_changed(શૂન્ય)
{
    char_u	*p;
    પૂર્ણાંક		new_align = ALIGN_ડાબું;
    long	new_columns = 20;
    bool	new_is_vert = false;
    bool	new_scrollbar = false;

    p = p_tplo;
    જ્યારે (*p != NUL)
    {
	જો (STRNCMP(p, "align:", 6) == 0)
	{
	    p += 6;
	    જો (STRNCMP(p, "left", 4) == 0)
	    {
		p += 4;
		new_align = ALIGN_ડાબું;
	    }
	    નહીંતર જો (STRNCMP(p, "right", 5) == 0)
	    {
		p += 5;
		new_align = ALIGN_જમણું;
	    }
	    નહીંતર
		પાછા FAIL;
	}
	નહીંતર જો (STRNCMP(p, "columns:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    new_columns = getdigits(&p);
	    જો (new_columns < 0 || new_columns > 1000)
		પાછા FAIL;
	}
	નહીંતર જો (STRNCMP(p, "vert", 4) == 0)
	{
	    p += 4;
	    new_is_vert = true;
	}
	નહીંતર જો (STRNCMP(p, "scrollbar", 9) == 0)
	{
	    p += 9;
	    new_scrollbar = true;
	}

	જો (*p != ',' && *p != NUL)
	    પાછા FAIL;
	જો (*p == ',')
	    ++p;
    }

    tpl_align = new_align;
    tpl_columns = new_columns;
    tpl_is_vert = new_is_vert;
    tpl_scrollbar = new_scrollbar;

    // Re-center the current tab on the next redraw.
    tpl_last_curtab = ખાલી;

    shell_new_columns();
    પાછા OK;
}

/*
 * Drop any internal reference to "tp", so draw_tabpanel() never compares
 * against a dangling pointer after the tabpage has been freed.
 */
    શૂન્ય
tabpanel_forget_tabpage(અચલ tabpage_T *tp)
{
    જો (tpl_last_curtab == tp)
	tpl_last_curtab = ખાલી;
}

/*
 * Return the width of tabpanel.
 */
    પૂર્ણાંક
tabpanel_width(શૂન્ય)
{
    પસંદગી (p_stpl)
    {
	સ્થિતિ 0:
	    પાછા 0;
	સ્થિતિ 1:
	    જો (first_tabpage->tp_next == ખાલી)
		પાછા 0;
    }
    જો (Columns < tpl_columns)
	પાછા 0;
    નહીંતર
	પાછા tpl_columns;
}

/*
 * Return the offset of a window considering the width of tabpanel.
 */
    પૂર્ણાંક
tabpanel_leftcol(શૂન્ય)
{
    પાછા tpl_align == ALIGN_જમણું ? 0 : tabpanel_width();
}

/*
 * Free previously resolved 'tabpanel' click regions.
 */
    સ્થિર શૂન્ય
tabpanel_free_click_regions(શૂન્ય)
{
    પૂર્ણાંક n;

    જો (tabpanel_stl_click != ખાલી)
    {
	માટે (n = 0; n < tabpanel_stl_click_ગણતરી; n++)
	    vim_free(tabpanel_stl_click[n].funcname);
	VIM_CLEAR(tabpanel_stl_click);
    }
    tabpanel_stl_click_ગણતરી = 0;
}

/*
 * Convert click records produced by build_stl_str_hl() for one line of
 * 'tabpanel' into screen-column based regions and append them to the global
 * tabpanel_stl_click array.  The caller keeps ownership of the funcname
 * strings inside "clicktab" — this function makes its own copies.
 */
    સ્થિર શૂન્ય
tabpanel_append_click_regions(
	stl_clickrec_T	*clicktab,
	char_u		*buf,
	પૂર્ણાંક		row,
	પૂર્ણાંક		col_start,
	પૂર્ણાંક		col_end,
	પૂર્ણાંક		tabnr)
{
    પૂર્ણાંક		ગણતરી = 0;
    પૂર્ણાંક		n;
    પૂર્ણાંક		base_col;
    પૂર્ણાંક		acc_width = 0;
    પૂર્ણાંક		max_w = col_end - col_start;
    char_u	*p;
    char_u	*cur_funcname = ખાલી;
    પૂર્ણાંક		cur_minwid = 0;
    પૂર્ણાંક		region_start_col;
    stl_click_region_T *new_arr;
    પૂર્ણાંક		limit;

    જો (clicktab == ખાલી)
	પાછા;

    માટે (n = 0; clicktab[n].start != ખાલી; n++)
	ગણતરી++;
    જો (ગણતરી == 0)
	પાછા;

    base_col = (tpl_align == ALIGN_જમણું ? topframe->fr_width : 0) + col_start;
    region_start_col = base_col;

    // Grow the global array to make room for up to "count" more regions
    // (one close for each record plus a possible trailing region).
    new_arr = vim_realloc(tabpanel_stl_click,
	    કદ(stl_click_region_T) * (tabpanel_stl_click_ગણતરી + ગણતરી + 1));
    જો (new_arr == ખાલી)
	પાછા;
    tabpanel_stl_click = new_arr;

    p = buf;
    માટે (n = 0; clicktab[n].start != ખાલી; n++)
    {
	acc_width += vim_strnsize(p, (પૂર્ણાંક)(clicktab[n].start - p));
	p = clicktab[n].start;
	limit = acc_width < max_w ? acc_width : max_w;

	જો (cur_funcname != ખાલી)
	{
	    stl_click_region_T *r =
				&tabpanel_stl_click[tabpanel_stl_click_ગણતરી];
	    r->row = row;
	    r->col_start = region_start_col;
	    r->col_end = base_col + limit;
	    r->funcname = vim_strsave(cur_funcname);
	    r->minwid = cur_minwid;
	    r->tabnr = tabnr;
	    tabpanel_stl_click_ગણતરી++;
	}

	cur_funcname = clicktab[n].funcname;
	cur_minwid = clicktab[n].minwid;
	region_start_col = base_col + limit;
    }

    // Close the final region if it extends to the end.
    જો (cur_funcname != ખાલી)
    {
	stl_click_region_T *r = &tabpanel_stl_click[tabpanel_stl_click_ગણતરી];
	r->row = row;
	r->col_start = region_start_col;
	r->col_end = base_col + max_w;
	r->funcname = vim_strsave(cur_funcname);
	r->minwid = cur_minwid;
	r->tabnr = tabnr;
	tabpanel_stl_click_ગણતરી++;
    }
}

/*
 * Ensure the current tab is visible by adjusting tpl_scroll_offset when
 * the selected tab has changed since the previous redraw.  Mouse wheel or
 * scrollbar drag operations leave curtab unchanged, so the user's chosen
 * offset is preserved in those cases.
 */
    સ્થિર શૂન્ય
follow_curtab_if_needed(પૂર્ણાંક curtab_row)
{
    જો (Rows <= 0 || curtab == tpl_last_curtab)
	પાછા;

    જો (curtab_row < tpl_scroll_offset)
	tpl_scroll_offset = curtab_row;
    નહીંતર જો (curtab_row >= tpl_scroll_offset + Rows)
	tpl_scroll_offset = curtab_row - Rows + 1;

    પૂર્ણાંક max_offset = tpl_total_rows > Rows ? tpl_total_rows - Rows : 0;

    જો (tpl_scroll_offset < 0)
	tpl_scroll_offset = 0;
    નહીંતર જો (tpl_scroll_offset > max_offset)
	tpl_scroll_offset = max_offset;
}

/*
 * draw the tabpanel.
 */
    શૂન્ય
draw_tabpanel(શૂન્ય)
{
    પૂર્ણાંક saved_ચાવી_Typed = ચાવી_Typed;
    પૂર્ણાંક saved_got_int = got_int;
    પૂર્ણાંક maxwidth = tabpanel_width();
    પૂર્ણાંક vs_attr = HL_ATTR(HLF_C);
    પૂર્ણાંક curtab_row = 0;
    પૂર્ણાંક vsrow = 0;
    પૂર્ણાંક is_જમણું = tpl_align == ALIGN_જમણું;

    જો (maxwidth == 0)
    {
	tabpanel_free_click_regions();
	પાછા;
    }

    // Discard old click regions — they'll be rebuilt during redraw below.
    tabpanel_free_click_regions();

    // Reset got_int to avoid build_stl_str_hl() isn't evaluated.
    got_int = FALSE;

    પૂર્ણાંક sb_len = tpl_scrollbar ? SCROLL_LEN : 0;
    પૂર્ણાંક sb_screen_col = -1;

    // The scrollbar is always placed at the right edge of the tabpanel,
    // regardless of 'align'.  The vertical separator sits at the panel's
    // boundary with the buffer area (left edge for align:right, right edge
    // for align:left).
    જો (tpl_is_vert)
    {
	જો (is_જમણું)
	{
	    // Panel on the right: vert at panel's left edge, scrollbar at
	    // panel's right edge (= screen's right edge).
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, VERT_LEN,
		    maxwidth - sb_len, &curtab_row, ખાલી);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, VERT_LEN, maxwidth - sb_len,
		    &curtab_row, ખાલી);
	    માટે (vsrow = 0; vsrow < Rows; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			topframe->fr_width, vs_attr);
	    જો (tpl_scrollbar)
		sb_screen_col = topframe->fr_width + maxwidth - SCROLL_LEN;
	}
	નહીંતર
	{
	    // Panel on the left: scrollbar just left of vert, vert at
	    // panel's right edge (boundary with buffer).
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0,
		    maxwidth - VERT_LEN - sb_len, &curtab_row, ખાલી);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - VERT_LEN - sb_len,
		    &curtab_row, ખાલી);
	    માટે (vsrow = 0; vsrow < Rows; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			maxwidth - VERT_LEN, vs_attr);
	    જો (tpl_scrollbar)
		sb_screen_col = maxwidth - VERT_LEN - SCROLL_LEN;
	}
    }
    નહીંતર
    {
	જો (is_જમણું)
	{
	    // Panel on the right, no vert: scrollbar at screen's right edge.
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth - sb_len,
		    &curtab_row, ખાલી);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_row, ખાલી);
	    જો (tpl_scrollbar)
		sb_screen_col = topframe->fr_width + maxwidth - SCROLL_LEN;
	}
	નહીંતર
	{
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth - sb_len,
		    &curtab_row, ખાલી);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_row, ખાલી);
	    જો (tpl_scrollbar)
		sb_screen_col = maxwidth - SCROLL_LEN;
	}
    }

    tpl_scrollbar_col = sb_screen_col;
    જો (sb_screen_col >= 0)
	draw_tabpanel_scrollbar(sb_screen_col);

    got_int |= saved_got_int;

    // A user function may reset KeyTyped, restore it.
    ચાવી_Typed = saved_ચાવી_Typed;

    tpl_last_curtab = curtab;
    redraw_tabpanel = FALSE;
}

/*
 * Return tabpagenr when clicking and dragging in tabpanel.
 */
    પૂર્ણાંક
get_tabpagenr_on_tabpanel(શૂન્ય)
{
    પૂર્ણાંક		maxwidth = tabpanel_width();
    પૂર્ણાંક		curtab_row = 0;
    પૂર્ણાંક		tabpagenr = 0;

    જો (maxwidth == 0)
	પાછા -1;

    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth, &curtab_row, ખાલી);
    do_by_tplmode(TPLMODE_GET_TABPAGENR, 0, maxwidth, &curtab_row,
	    &tabpagenr);

    પાછા tabpagenr;
}

/*
 * Fill tailing area between {start_row} and {end_row - 1}.
 */
    સ્થિર શૂન્ય
screen_fill_tailing_area(
	પૂર્ણાંક	tplmode,
	પૂર્ણાંક	row_start,
	પૂર્ણાંક	row_end,
	પૂર્ણાંક	col_start,
	પૂર્ણાંક	col_end,
	પૂર્ણાંક	attr)
{
    પૂર્ણાંક is_જમણું = tpl_align == ALIGN_જમણું;
    જો (tplmode == TPLMODE_REDRAW)
	screen_fill(row_start, row_end,
		(is_જમણું ? topframe->fr_width : 0) + col_start,
		(is_જમણું ? topframe->fr_width : 0) + col_end,
		TPL_FILLCHAR, TPL_FILLCHAR, attr);
}

/*
 * screen_puts_len() for tabpanel.
 */
    સ્થિર શૂન્ય
screen_puts_len_for_tabpanel(
	પૂર્ણાંક	    tplmode,
	char_u	    *p,
	પૂર્ણાંક	    len,
	પૂર્ણાંક	    attr,
	tabpanel_T  *pargs)
{
    પૂર્ણાંક		j;
    પૂર્ણાંક		chlen;
    પૂર્ણાંક		chcells;
    char_u	buf[IOSIZE];
    char_u	*temp;

    માટે (j = 0; j < len;)
    {
	જો (tplmode != TPLMODE_GET_CURTAB_ROW
		&& pargs->maxrow <= *pargs->prow - pargs->offsetrow)
	    તોડો;

	જો (has_mbyte)
	    chlen = (*mb_ptr2len)(p + j);
	નહીંતર
	    chlen = 1;

	માટે (પૂર્ણાંક k = 0; k < chlen; k++)
	    buf[k] = p[j + k];
	buf[chlen] = NUL;
	j += chlen;

	// Make all characters printable.
	temp = transstr(buf);
	જો (temp != ખાલી)
	{
	    vim_strncpy(buf, temp, કદ(buf) - 1);
	    vim_free(temp);
	}

	જો (has_mbyte)
	    chcells = (*mb_ptr2cells)(buf);
	નહીંતર
	    chcells = 1;

	જો (pargs->col_end < (*pargs->pcol) + chcells)
	{
	    // fill the tailing area of current row.
	    જો (*pargs->prow - pargs->offsetrow >= 0
		    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
		screen_fill_tailing_area(tplmode,
			*pargs->prow - pargs->offsetrow,
			*pargs->prow - pargs->offsetrow + 1,
			*pargs->pcol, pargs->col_end, attr);
	    *pargs->pcol = pargs->col_end;

	    જો (pargs->col_end < chcells)
		તોડો;
	}

	જો (*pargs->pcol + chcells <= pargs->col_end)
	{
	    પૂર્ણાંક off = (tpl_align == ALIGN_જમણું)
		    ? topframe->fr_width
		    : 0;
	    જો (tplmode == TPLMODE_REDRAW
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
    સ્થિર શૂન્ય
draw_tabpanel_default(પૂર્ણાંક tplmode, tabpanel_T *pargs)
{
    પૂર્ણાંક		modified;
    પૂર્ણાંક		wincount;
    પૂર્ણાંક		len = 0;
    char_u	buf[2] = { NUL, NUL };

    modified = FALSE;
    માટે (wincount = 0; pargs->wp != ખાલી;
	    pargs->wp = pargs->wp->w_next, ++wincount)
	જો (bufIsChanged(pargs->wp->w_buffer))
	    modified = TRUE;

    જો (modified || wincount > 1)
    {
	જો (wincount > 1)
	{
	    vim_snprintf((અક્ષર *)NameBuff, MAXPATHL, "%d", wincount);
	    len = (પૂર્ણાંક)STRLEN(NameBuff);
	    screen_puts_len_for_tabpanel(tplmode, NameBuff, len,
#જો defined(FEAT_SYN_HL)
		    hl_combine_attr(pargs->attr, HL_ATTR(HLF_T)),
#નહીંતર
		    pargs->attr,
#endif
		    pargs);
	}
	જો (modified)
	{
	    buf[0] = '+';
	    screen_puts_len_for_tabpanel(tplmode, buf, 1, pargs->attr, pargs);
	}

	buf[0] = TPL_FILLCHAR;
	screen_puts_len_for_tabpanel(tplmode, buf, 1, pargs->attr, pargs);
    }

    get_trans_bufname(pargs->cwp->w_buffer);
    shorten_dir(NameBuff);
    len = (પૂર્ણાંક)STRLEN(NameBuff);
    screen_puts_len_for_tabpanel(tplmode, NameBuff, len, pargs->attr, pargs);

    // fill the tailing area of current row.
    જો (*pargs->prow - pargs->offsetrow >= 0
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
    સ્થિર શૂન્ય
draw_tabpanel_with_highlight(
	પૂર્ણાંક	    tplmode,
	char_u	    *buf,
	stl_hlrec_T *hltab,
	tabpanel_T  *pargs)
{
    char_u	*p;
    પૂર્ણાંક		curattr;
    પૂર્ણાંક		n;

    curattr = pargs->attr;
    p = buf;
    માટે (n = 0; hltab[n].start != ખાલી; n++)
    {
	screen_puts_len_for_tabpanel(tplmode, p, (પૂર્ણાંક)(hltab[n].start - p),
		curattr, pargs);
	p = hltab[n].start;
	જો (hltab[n].userhl == 0)
	    curattr = pargs->attr;
	નહીંતર જો (hltab[n].userhl < 0)
	    curattr = syn_id2attr(-hltab[n].userhl);
#ifdef FEAT_TERMINAL
	નહીંતર જો (pargs->wp != ખાલી && pargs->wp != curwin
		&& bt_terminal(pargs->wp->w_buffer)
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stltermnc[hltab[n].userhl - 1];
	નહીંતર જો (pargs->wp != ખાલી && bt_terminal(pargs->wp->w_buffer)
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stlterm[hltab[n].userhl - 1];
#endif
	નહીંતર જો (pargs->wp != ખાલી && pargs->wp != curwin
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stlnc[hltab[n].userhl - 1];
	નહીંતર
	    curattr = highlight_user[hltab[n].userhl - 1];
    }
    screen_puts_len_for_tabpanel(tplmode, p, (પૂર્ણાંક)STRLEN(p), curattr, pargs);

    // fill the tailing area of current row.
    જો (*pargs->prow - pargs->offsetrow >= 0
	    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
	screen_fill_tailing_area(tplmode, *pargs->prow - pargs->offsetrow,
		*pargs->prow - pargs->offsetrow + 1, *pargs->pcol,
		pargs->col_end, curattr);
    *pargs->pcol = pargs->col_end;
}

/*
 * do something by tplmode for drawing tabpanel.
 */
    સ્થિર શૂન્ય
do_by_tplmode(
	પૂર્ણાંક	tplmode,
	પૂર્ણાંક	col_start,
	પૂર્ણાંક	col_end,
	પૂર્ણાંક	*pcurtab_row,
	પૂર્ણાંક	*ptabpagenr)
{
    પૂર્ણાંક		attr_tplf = HL_ATTR(HLF_TPLF);
    પૂર્ણાંક		attr_tpls = HL_ATTR(HLF_TPLS);
    પૂર્ણાંક		attr_tpl = HL_ATTR(HLF_TPL);
    પૂર્ણાંક		col = col_start;
    પૂર્ણાંક		row = 0;
    tabpage_T	*tp = ખાલી;
    typval_T	v;
    tabpanel_T	args;

    args.maxrow = Rows;
    args.offsetrow = 0;
    args.col_start = col_start;
    args.col_end = col_end;

    જો (tplmode != TPLMODE_GET_CURTAB_ROW && args.maxrow > 0)
	args.offsetrow = tpl_scroll_offset;

    tp = first_tabpage;

    માટે (row = 0; tp != ખાલી; row++)
    {
	જો (tplmode != TPLMODE_GET_CURTAB_ROW
		&& args.maxrow <= row - args.offsetrow)
	    તોડો;

	col = col_start;

	v.v_type = VAR_સંખ્યા;
	v.vval.v_સંખ્યા = tabpage_index(tp);
	set_var((char_u *)"g:actual_curtabpage", &v, TRUE);

	જો (tp->tp_topframe == topframe)
	{
	    args.attr = attr_tpls;
	    જો (tplmode == TPLMODE_GET_CURTAB_ROW)
		// Capture the row of the current tab and keep iterating so
		// tpl_total_rows receives the true content height below.
		*pcurtab_row = row;
	}
	નહીંતર
	    args.attr = attr_tpl;

	જો (tp == curtab)
	{
	    args.cwp = curwin;
	    args.wp = firstwin;
	}
	નહીંતર
	{
	    args.cwp = tp->tp_curwin;
	    args.wp = tp->tp_firstwin;
	}

	char_u	*usefmt = vim_strsave(p_tpl);

	જો (usefmt != ખાલી && *usefmt != NUL)
	{
	    પૂર્ણાંક	carry_hl = 0;

	    જ્યારે (*usefmt != NUL)
	    {
		char_u	buf[IOSIZE];
		stl_hlrec_T	*hltab;
		stl_hlrec_T	*tabtab;
		stl_clickrec_T	*clicktab = ખાલી;

		જો (tplmode != TPLMODE_GET_CURTAB_ROW
			&& args.maxrow <= row - args.offsetrow)
		    તોડો;

		buf[0] = NUL;
#ifdef ENABLE_STL_MODE_MULTI_NL
		(શૂન્ય)build_stl_str_hl_mline_nl
#નહીંતર
		(શૂન્ય)build_stl_str_hl_mline
#endif
			(args.cwp, buf, કદ(buf),
			&usefmt, opt_name, opt_scope, TPL_FILLCHAR,
			args.col_end - args.col_start, &hltab, &tabtab,
			tplmode == TPLMODE_REDRAW ? &clicktab : ખાલી,
			&carry_hl);

		args.prow = &row;
		args.pcol = &col;

		draw_tabpanel_with_highlight(tplmode, buf, hltab, &args);

		// Record any %[FuncName] click regions for this line once
		// the text has been drawn.  Only visible rows participate.
		જો (tplmode == TPLMODE_REDRAW && clicktab != ખાલી)
		{
		    પૂર્ણાંક screen_row = row - args.offsetrow;
		    પૂર્ણાંક m;

		    જો (screen_row >= 0 && screen_row < args.maxrow)
			tabpanel_append_click_regions(clicktab, buf,
				screen_row, args.col_start, args.col_end,
				(પૂર્ણાંક)v.vval.v_સંખ્યા);
		    // We took ownership of the click records — free the
		    // function names (matches the non-NULL clicktab path in
		    // build_stl_str_hl()).
		    માટે (m = 0; clicktab[m].start != ખાલી; m++)
			vim_free(clicktab[m].funcname);
		}

		// Move to next line for %@
		જો (*usefmt != NUL)
		{
		    row++;
		    col = col_start;
		}
	    }
	}
	નહીંતર
	{
	    args.user_defined = ખાલી;
	    args.prow = &row;
	    args.pcol = &col;
	    draw_tabpanel_default(tplmode, &args);
	}

	vim_free(usefmt);
	do_unlet((char_u *)"g:actual_curtabpage", TRUE);

	tp = tp->tp_next;

	જો ((tplmode == TPLMODE_GET_TABPAGENR)
		&& (mouse_row <= (row - args.offsetrow)))
	{
	    *ptabpagenr = v.vval.v_સંખ્યા;
	    તોડો;
	}
    }

    // fill the area of TabPanelFill.
    screen_fill_tailing_area(tplmode, MAX(row - args.offsetrow, 0), args.maxrow,
	    args.col_start, args.col_end, attr_tplf);

    // Capture the true content height during the GET_CURTAB_ROW pass, which
    // ignores maxrow and therefore walks every tab.  REDRAW stops at the
    // visible edge so its "row" is clamped and unusable here.
    જો (tplmode == TPLMODE_GET_CURTAB_ROW)
	tpl_total_rows = row;
}

/*
 * Draw the tabpanel scrollbar (track + thumb) at screen column 'screen_col'.
 * The scrollbar spans the full screen height.  The thumb position and size
 * are derived from tpl_scroll_offset, tpl_total_rows and Rows.
 */
    સ્થિર શૂન્ય
draw_tabpanel_scrollbar(પૂર્ણાંક screen_col)
{
    પૂર્ણાંક attr_sb = HL_ATTR(HLF_PSB);
    પૂર્ણાંક attr_thumb = HL_ATTR(HLF_PST);
    પૂર્ણાંક thumb_ટોચ = 0;
    પૂર્ણાંક thumb_height = 0;

    જો (tpl_total_rows > Rows && Rows > 0)
    {
	પૂર્ણાંક max_offset = tpl_total_rows - Rows;
	પૂર્ણાંક track_range;

	thumb_height = Rows * Rows / tpl_total_rows;
	જો (thumb_height < 1)
	    thumb_height = 1;

	// Map tpl_scroll_offset onto the track: at offset 0 the thumb's top
	// is at row 0, at the maximum offset its bottom reaches the last
	// row.  This is the exact inverse of tabpanel_drag_scrollbar().
	track_range = Rows - thumb_height;
	જો (track_range > 0 && max_offset > 0)
	    thumb_ટોચ = track_range * tpl_scroll_offset / max_offset;
	નહીંતર
	    thumb_ટોચ = 0;
	જો (thumb_ટોચ + thumb_height > Rows)
	    thumb_ટોચ = Rows - thumb_height;
	જો (thumb_ટોચ < 0)
	    thumb_ટોચ = 0;
    }

    માટે (પૂર્ણાંક r = 0; r < Rows; r++)
    {
	bool on_thumb = thumb_height > 0
	    && r >= thumb_ટોચ && r < thumb_ટોચ + thumb_height;
	screen_putchar(TPL_FILLCHAR, r, screen_col,
		on_thumb ? attr_thumb : attr_sb);
    }
}

/*
 * Return true if the mouse is currently positioned over the tabpanel area.
 */
    bool
mouse_on_tabpanel(શૂન્ય)
{
    જો (tabpanel_width() == 0)
	પાછા false;
    પાછા mouse_col < firstwin->w_wincol
	|| mouse_col >= firstwin->w_wincol + topframe->fr_width;
}

/*
 * Return true if the mouse is currently on the scrollbar column.
 * The scrollbar column is tracked by draw_tabpanel() and is -1 when the
 * scrollbar is not enabled or not yet drawn.
 */
    bool
mouse_on_tabpanel_scrollbar(શૂન્ય)
{
    પાછા tpl_scrollbar && tpl_scrollbar_col >= 0
	&& mouse_col == tpl_scrollbar_col;
}

/*
 * Move the scrollbar thumb so it is vertically centred on screen row
 * 'screen_row', updating tpl_scroll_offset accordingly.  Used for both
 * initial clicks and subsequent drag events.
 * Returns true if the event was consumed (offset changed or not).
 */
    bool
tabpanel_drag_scrollbar(પૂર્ણાંક screen_row)
{
    પૂર્ણાંક thumb_height;
    પૂર્ણાંક max_offset;
    પૂર્ણાંક track_range;
    પૂર્ણાંક thumb_ટોચ;
    પૂર્ણાંક new_offset;

    જો (!tpl_scrollbar || Rows <= 0 || tpl_total_rows <= Rows)
	પાછા false;

    thumb_height = Rows * Rows / tpl_total_rows;
    જો (thumb_height < 1)
	thumb_height = 1;
    track_range = Rows - thumb_height;
    જો (track_range <= 0)
	પાછા true;

    max_offset = tpl_total_rows - Rows;
    thumb_ટોચ = screen_row - thumb_height / 2;
    જો (thumb_ટોચ < 0)
	thumb_ટોચ = 0;
    જો (thumb_ટોચ > track_range)
	thumb_ટોચ = track_range;

    new_offset = thumb_ટોચ * max_offset / track_range;
    જો (new_offset != tpl_scroll_offset)
    {
	tpl_scroll_offset = new_offset;
	redraw_tabpanel = TRUE;
    }
    પાછા true;
}

/*
 * Scroll the tabpanel by 'count' rows in direction 'dir' (1 = down, -1 = up).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_scroll(પૂર્ણાંક dir, પૂર્ણાંક ગણતરી)
{
    પૂર્ણાંક max_offset;
    પૂર્ણાંક new_offset;

    જો (tabpanel_width() == 0)
	પાછા false;

    max_offset = tpl_total_rows - Rows;
    જો (max_offset < 0)
	max_offset = 0;

    new_offset = tpl_scroll_offset + (dir > 0 ? ગણતરી : -ગણતરી);
    જો (new_offset < 0)
	new_offset = 0;
    જો (new_offset > max_offset)
	new_offset = max_offset;
    જો (new_offset == tpl_scroll_offset)
	પાછા false;

    tpl_scroll_offset = new_offset;
    redraw_tabpanel = TRUE;
    પાછા true;
}

/*
 * Set the tabpanel scroll offset to "offset" (clamped to the valid range).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_set_offset(પૂર્ણાંક offset)
{
    પૂર્ણાંક max_offset;

    જો (tabpanel_width() == 0)
	પાછા false;

    max_offset = tpl_total_rows - Rows;
    જો (max_offset < 0)
	max_offset = 0;

    જો (offset < 0)
	offset = 0;
    જો (offset > max_offset)
	offset = max_offset;
    જો (offset == tpl_scroll_offset)
	પાછા false;

    tpl_scroll_offset = offset;
    redraw_tabpanel = TRUE;
    પાછા true;
}

/*
 * "tabpanel_getinfo()" function
 */
    શૂન્ય
f_tabpanel_getinfo(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    પૂર્ણાંક		max_offset;

    જો (rettv_dict_alloc(rettv) == FAIL)
	પાછા;
    d = rettv->vval.v_dict;

    max_offset = tpl_total_rows - Rows;
    જો (max_offset < 0)
	max_offset = 0;

    dict_add_string(d, "align",
	    (char_u *)(tpl_align == ALIGN_જમણું ? "right" : "left"));
    dict_add_સંખ્યા(d, "columns", tabpanel_width());
    dict_add_bool(d, "scrollbar", tpl_scrollbar);
    dict_add_સંખ્યા(d, "offset", tpl_scroll_offset);
    dict_add_સંખ્યા(d, "total", tpl_total_rows);
    dict_add_સંખ્યા(d, "max_offset", max_offset);
}

/*
 * "tabpanel_scroll()" function
 */
    શૂન્ય
f_tabpanel_scroll(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	n;
    પૂર્ણાંક		absolute = 0;
    bool	changed;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_સંખ્યા = VVAL_FALSE;

    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	પાછા;

    n = tv_get_સંખ્યા_chk(&argvars[0], ખાલી);
    જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	જો (argvars[1].v_type != VAR_DICT || argvars[1].vval.v_dict == ખાલી)
	{
	    emsg(_(e_dictionary_required));
	    પાછા;
	}
	absolute = dict_get_bool(argvars[1].vval.v_dict, "absolute", FALSE);
    }

    // Clamp to int range to avoid signed overflow when casting and negating.
    જો (n > INT_MAX)
	n = INT_MAX;
    નહીંતર જો (n < -INT_MAX)
	n = -INT_MAX;

    જો (absolute)
	changed = tabpanel_set_offset((પૂર્ણાંક)n);
    નહીંતર
	changed = tabpanel_scroll(n >= 0 ? 1 : -1,
				  (પૂર્ણાંક)(n >= 0 ? n : -n));

    rettv->vval.v_સંખ્યા = changed ? VVAL_TRUE : VVAL_FALSE;
}

#endif // FEAT_TABPANEL
