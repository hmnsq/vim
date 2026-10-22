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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_TABPANEL)

സ്ഥിതം ശൂന്യം do_by_tplmode(പൂർണ്ണസംഖ്യ tplmode, പൂർണ്ണസംഖ്യ col_start, പൂർണ്ണസംഖ്യ col_end,
	പൂർണ്ണസംഖ്യ *pcurtab_row, പൂർണ്ണസംഖ്യ *ptabpagenr);
സ്ഥിതം ശൂന്യം tabpanel_free_click_regions(ശൂന്യം);
സ്ഥിതം ശൂന്യം tabpanel_append_click_regions(stl_clickrec_T *clicktab,
	char_u *buf, പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col_start, പൂർണ്ണസംഖ്യ col_end, പൂർണ്ണസംഖ്യ tabnr);
സ്ഥിതം ശൂന്യം draw_tabpanel_scrollbar(പൂർണ്ണസംഖ്യ screen_col);

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
#define ALIGN_ഇടത്		0
#define ALIGN_വലത്		1

സ്ഥിതം char_u *opt_name = (char_u *)"tabpanel";
സ്ഥിതം പൂർണ്ണസംഖ്യ opt_scope = OPT_LOCAL;
സ്ഥിതം പൂർണ്ണസംഖ്യ tpl_align = ALIGN_ഇടത്;
സ്ഥിതം പൂർണ്ണസംഖ്യ tpl_columns = 20;
സ്ഥിതം bool tpl_is_vert = false;
സ്ഥിതം bool tpl_scrollbar = false;
സ്ഥിതം പൂർണ്ണസംഖ്യ tpl_scroll_offset = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ tpl_total_rows = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ tpl_scrollbar_col = -1;  // screen column of scrollbar, -1 if none
സ്ഥിതം tabpage_T *tpl_last_curtab = ശൂന്യ;   // last curtab seen by draw_tabpanel

തരനിർവചനം ഘടന {
    win_T   *wp;
    win_T   *cwp;
    char_u  *user_defined;
    പൂർണ്ണസംഖ്യ	maxrow;
    പൂർണ്ണസംഖ്യ	offsetrow;
    പൂർണ്ണസംഖ്യ	*prow;
    പൂർണ്ണസംഖ്യ	*pcol;
    പൂർണ്ണസംഖ്യ	attr;
    പൂർണ്ണസംഖ്യ	col_start;
    പൂർണ്ണസംഖ്യ	col_end;
} tabpanel_T;

    പൂർണ്ണസംഖ്യ
tabpanelopt_changed(ശൂന്യം)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		new_align = ALIGN_ഇടത്;
    long	new_columns = 20;
    bool	new_is_vert = false;
    bool	new_scrollbar = false;

    p = p_tplo;
    ആകുമ്പോൾ (*p != NUL)
    {
	എങ്കിൽ (STRNCMP(p, "align:", 6) == 0)
	{
	    p += 6;
	    എങ്കിൽ (STRNCMP(p, "left", 4) == 0)
	    {
		p += 4;
		new_align = ALIGN_ഇടത്;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "right", 5) == 0)
	    {
		p += 5;
		new_align = ALIGN_വലത്;
	    }
	    അല്ലെങ്കിൽ
		തിരികെ FAIL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "columns:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    new_columns = getdigits(&p);
	    എങ്കിൽ (new_columns < 0 || new_columns > 1000)
		തിരികെ FAIL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "vert", 4) == 0)
	{
	    p += 4;
	    new_is_vert = true;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "scrollbar", 9) == 0)
	{
	    p += 9;
	    new_scrollbar = true;
	}

	എങ്കിൽ (*p != ',' && *p != NUL)
	    തിരികെ FAIL;
	എങ്കിൽ (*p == ',')
	    ++p;
    }

    tpl_align = new_align;
    tpl_columns = new_columns;
    tpl_is_vert = new_is_vert;
    tpl_scrollbar = new_scrollbar;

    // Re-center the current tab on the next redraw.
    tpl_last_curtab = ശൂന്യ;

    shell_new_columns();
    തിരികെ OK;
}

/*
 * Drop any internal reference to "tp", so draw_tabpanel() never compares
 * against a dangling pointer after the tabpage has been freed.
 */
    ശൂന്യം
tabpanel_forget_tabpage(സ്ഥിരം tabpage_T *tp)
{
    എങ്കിൽ (tpl_last_curtab == tp)
	tpl_last_curtab = ശൂന്യ;
}

/*
 * Return the width of tabpanel.
 */
    പൂർണ്ണസംഖ്യ
tabpanel_width(ശൂന്യം)
{
    തിരഞ്ഞെടുപ്പ് (p_stpl)
    {
	അവസ്ഥ 0:
	    തിരികെ 0;
	അവസ്ഥ 1:
	    എങ്കിൽ (first_tabpage->tp_next == ശൂന്യ)
		തിരികെ 0;
    }
    എങ്കിൽ (Columns < tpl_columns)
	തിരികെ 0;
    അല്ലെങ്കിൽ
	തിരികെ tpl_columns;
}

/*
 * Return the offset of a window considering the width of tabpanel.
 */
    പൂർണ്ണസംഖ്യ
tabpanel_leftcol(ശൂന്യം)
{
    തിരികെ tpl_align == ALIGN_വലത് ? 0 : tabpanel_width();
}

/*
 * Free previously resolved 'tabpanel' click regions.
 */
    സ്ഥിതം ശൂന്യം
tabpanel_free_click_regions(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ n;

    എങ്കിൽ (tabpanel_stl_click != ശൂന്യ)
    {
	വേണ്ടി (n = 0; n < tabpanel_stl_click_എണ്ണം; n++)
	    vim_free(tabpanel_stl_click[n].funcname);
	VIM_CLEAR(tabpanel_stl_click);
    }
    tabpanel_stl_click_എണ്ണം = 0;
}

/*
 * Convert click records produced by build_stl_str_hl() for one line of
 * 'tabpanel' into screen-column based regions and append them to the global
 * tabpanel_stl_click array.  The caller keeps ownership of the funcname
 * strings inside "clicktab" — this function makes its own copies.
 */
    സ്ഥിതം ശൂന്യം
tabpanel_append_click_regions(
	stl_clickrec_T	*clicktab,
	char_u		*buf,
	പൂർണ്ണസംഖ്യ		row,
	പൂർണ്ണസംഖ്യ		col_start,
	പൂർണ്ണസംഖ്യ		col_end,
	പൂർണ്ണസംഖ്യ		tabnr)
{
    പൂർണ്ണസംഖ്യ		എണ്ണം = 0;
    പൂർണ്ണസംഖ്യ		n;
    പൂർണ്ണസംഖ്യ		base_col;
    പൂർണ്ണസംഖ്യ		acc_width = 0;
    പൂർണ്ണസംഖ്യ		max_w = col_end - col_start;
    char_u	*p;
    char_u	*cur_funcname = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		cur_minwid = 0;
    പൂർണ്ണസംഖ്യ		region_start_col;
    stl_click_region_T *new_arr;
    പൂർണ്ണസംഖ്യ		limit;

    എങ്കിൽ (clicktab == ശൂന്യ)
	തിരികെ;

    വേണ്ടി (n = 0; clicktab[n].start != ശൂന്യ; n++)
	എണ്ണം++;
    എങ്കിൽ (എണ്ണം == 0)
	തിരികെ;

    base_col = (tpl_align == ALIGN_വലത് ? topframe->fr_width : 0) + col_start;
    region_start_col = base_col;

    // Grow the global array to make room for up to "count" more regions
    // (one close for each record plus a possible trailing region).
    new_arr = vim_realloc(tabpanel_stl_click,
	    വലുപ്പം(stl_click_region_T) * (tabpanel_stl_click_എണ്ണം + എണ്ണം + 1));
    എങ്കിൽ (new_arr == ശൂന്യ)
	തിരികെ;
    tabpanel_stl_click = new_arr;

    p = buf;
    വേണ്ടി (n = 0; clicktab[n].start != ശൂന്യ; n++)
    {
	acc_width += vim_strnsize(p, (പൂർണ്ണസംഖ്യ)(clicktab[n].start - p));
	p = clicktab[n].start;
	limit = acc_width < max_w ? acc_width : max_w;

	എങ്കിൽ (cur_funcname != ശൂന്യ)
	{
	    stl_click_region_T *r =
				&tabpanel_stl_click[tabpanel_stl_click_എണ്ണം];
	    r->row = row;
	    r->col_start = region_start_col;
	    r->col_end = base_col + limit;
	    r->funcname = vim_strsave(cur_funcname);
	    r->minwid = cur_minwid;
	    r->tabnr = tabnr;
	    tabpanel_stl_click_എണ്ണം++;
	}

	cur_funcname = clicktab[n].funcname;
	cur_minwid = clicktab[n].minwid;
	region_start_col = base_col + limit;
    }

    // Close the final region if it extends to the end.
    എങ്കിൽ (cur_funcname != ശൂന്യ)
    {
	stl_click_region_T *r = &tabpanel_stl_click[tabpanel_stl_click_എണ്ണം];
	r->row = row;
	r->col_start = region_start_col;
	r->col_end = base_col + max_w;
	r->funcname = vim_strsave(cur_funcname);
	r->minwid = cur_minwid;
	r->tabnr = tabnr;
	tabpanel_stl_click_എണ്ണം++;
    }
}

/*
 * Ensure the current tab is visible by adjusting tpl_scroll_offset when
 * the selected tab has changed since the previous redraw.  Mouse wheel or
 * scrollbar drag operations leave curtab unchanged, so the user's chosen
 * offset is preserved in those cases.
 */
    സ്ഥിതം ശൂന്യം
follow_curtab_if_needed(പൂർണ്ണസംഖ്യ curtab_row)
{
    എങ്കിൽ (Rows <= 0 || curtab == tpl_last_curtab)
	തിരികെ;

    എങ്കിൽ (curtab_row < tpl_scroll_offset)
	tpl_scroll_offset = curtab_row;
    അല്ലെങ്കിൽ എങ്കിൽ (curtab_row >= tpl_scroll_offset + Rows)
	tpl_scroll_offset = curtab_row - Rows + 1;

    പൂർണ്ണസംഖ്യ max_offset = tpl_total_rows > Rows ? tpl_total_rows - Rows : 0;

    എങ്കിൽ (tpl_scroll_offset < 0)
	tpl_scroll_offset = 0;
    അല്ലെങ്കിൽ എങ്കിൽ (tpl_scroll_offset > max_offset)
	tpl_scroll_offset = max_offset;
}

/*
 * draw the tabpanel.
 */
    ശൂന്യം
draw_tabpanel(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ saved_താക്കോൽ_Typed = താക്കോൽ_Typed;
    പൂർണ്ണസംഖ്യ saved_got_int = got_int;
    പൂർണ്ണസംഖ്യ maxwidth = tabpanel_width();
    പൂർണ്ണസംഖ്യ vs_attr = HL_ATTR(HLF_C);
    പൂർണ്ണസംഖ്യ curtab_row = 0;
    പൂർണ്ണസംഖ്യ vsrow = 0;
    പൂർണ്ണസംഖ്യ is_വലത് = tpl_align == ALIGN_വലത്;

    എങ്കിൽ (maxwidth == 0)
    {
	tabpanel_free_click_regions();
	തിരികെ;
    }

    // Discard old click regions — they'll be rebuilt during redraw below.
    tabpanel_free_click_regions();

    // Reset got_int to avoid build_stl_str_hl() isn't evaluated.
    got_int = FALSE;

    പൂർണ്ണസംഖ്യ sb_len = tpl_scrollbar ? SCROLL_LEN : 0;
    പൂർണ്ണസംഖ്യ sb_screen_col = -1;

    // The scrollbar is always placed at the right edge of the tabpanel,
    // regardless of 'align'.  The vertical separator sits at the panel's
    // boundary with the buffer area (left edge for align:right, right edge
    // for align:left).
    എങ്കിൽ (tpl_is_vert)
    {
	എങ്കിൽ (is_വലത്)
	{
	    // Panel on the right: vert at panel's left edge, scrollbar at
	    // panel's right edge (= screen's right edge).
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, VERT_LEN,
		    maxwidth - sb_len, &curtab_row, ശൂന്യ);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, VERT_LEN, maxwidth - sb_len,
		    &curtab_row, ശൂന്യ);
	    വേണ്ടി (vsrow = 0; vsrow < Rows; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			topframe->fr_width, vs_attr);
	    എങ്കിൽ (tpl_scrollbar)
		sb_screen_col = topframe->fr_width + maxwidth - SCROLL_LEN;
	}
	അല്ലെങ്കിൽ
	{
	    // Panel on the left: scrollbar just left of vert, vert at
	    // panel's right edge (boundary with buffer).
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0,
		    maxwidth - VERT_LEN - sb_len, &curtab_row, ശൂന്യ);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - VERT_LEN - sb_len,
		    &curtab_row, ശൂന്യ);
	    വേണ്ടി (vsrow = 0; vsrow < Rows; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			maxwidth - VERT_LEN, vs_attr);
	    എങ്കിൽ (tpl_scrollbar)
		sb_screen_col = maxwidth - VERT_LEN - SCROLL_LEN;
	}
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (is_വലത്)
	{
	    // Panel on the right, no vert: scrollbar at screen's right edge.
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth - sb_len,
		    &curtab_row, ശൂന്യ);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_row, ശൂന്യ);
	    എങ്കിൽ (tpl_scrollbar)
		sb_screen_col = topframe->fr_width + maxwidth - SCROLL_LEN;
	}
	അല്ലെങ്കിൽ
	{
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth - sb_len,
		    &curtab_row, ശൂന്യ);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_row, ശൂന്യ);
	    എങ്കിൽ (tpl_scrollbar)
		sb_screen_col = maxwidth - SCROLL_LEN;
	}
    }

    tpl_scrollbar_col = sb_screen_col;
    എങ്കിൽ (sb_screen_col >= 0)
	draw_tabpanel_scrollbar(sb_screen_col);

    got_int |= saved_got_int;

    // A user function may reset KeyTyped, restore it.
    താക്കോൽ_Typed = saved_താക്കോൽ_Typed;

    tpl_last_curtab = curtab;
    redraw_tabpanel = FALSE;
}

/*
 * Return tabpagenr when clicking and dragging in tabpanel.
 */
    പൂർണ്ണസംഖ്യ
get_tabpagenr_on_tabpanel(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		maxwidth = tabpanel_width();
    പൂർണ്ണസംഖ്യ		curtab_row = 0;
    പൂർണ്ണസംഖ്യ		tabpagenr = 0;

    എങ്കിൽ (maxwidth == 0)
	തിരികെ -1;

    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth, &curtab_row, ശൂന്യ);
    do_by_tplmode(TPLMODE_GET_TABPAGENR, 0, maxwidth, &curtab_row,
	    &tabpagenr);

    തിരികെ tabpagenr;
}

/*
 * Fill tailing area between {start_row} and {end_row - 1}.
 */
    സ്ഥിതം ശൂന്യം
screen_fill_tailing_area(
	പൂർണ്ണസംഖ്യ	tplmode,
	പൂർണ്ണസംഖ്യ	row_start,
	പൂർണ്ണസംഖ്യ	row_end,
	പൂർണ്ണസംഖ്യ	col_start,
	പൂർണ്ണസംഖ്യ	col_end,
	പൂർണ്ണസംഖ്യ	attr)
{
    പൂർണ്ണസംഖ്യ is_വലത് = tpl_align == ALIGN_വലത്;
    എങ്കിൽ (tplmode == TPLMODE_REDRAW)
	screen_fill(row_start, row_end,
		(is_വലത് ? topframe->fr_width : 0) + col_start,
		(is_വലത് ? topframe->fr_width : 0) + col_end,
		TPL_FILLCHAR, TPL_FILLCHAR, attr);
}

/*
 * screen_puts_len() for tabpanel.
 */
    സ്ഥിതം ശൂന്യം
screen_puts_len_for_tabpanel(
	പൂർണ്ണസംഖ്യ	    tplmode,
	char_u	    *p,
	പൂർണ്ണസംഖ്യ	    len,
	പൂർണ്ണസംഖ്യ	    attr,
	tabpanel_T  *pargs)
{
    പൂർണ്ണസംഖ്യ		j;
    പൂർണ്ണസംഖ്യ		chlen;
    പൂർണ്ണസംഖ്യ		chcells;
    char_u	buf[IOSIZE];
    char_u	*temp;

    വേണ്ടി (j = 0; j < len;)
    {
	എങ്കിൽ (tplmode != TPLMODE_GET_CURTAB_ROW
		&& pargs->maxrow <= *pargs->prow - pargs->offsetrow)
	    നിർത്തുക;

	എങ്കിൽ (has_mbyte)
	    chlen = (*mb_ptr2len)(p + j);
	അല്ലെങ്കിൽ
	    chlen = 1;

	വേണ്ടി (പൂർണ്ണസംഖ്യ k = 0; k < chlen; k++)
	    buf[k] = p[j + k];
	buf[chlen] = NUL;
	j += chlen;

	// Make all characters printable.
	temp = transstr(buf);
	എങ്കിൽ (temp != ശൂന്യ)
	{
	    vim_strncpy(buf, temp, വലുപ്പം(buf) - 1);
	    vim_free(temp);
	}

	എങ്കിൽ (has_mbyte)
	    chcells = (*mb_ptr2cells)(buf);
	അല്ലെങ്കിൽ
	    chcells = 1;

	എങ്കിൽ (pargs->col_end < (*pargs->pcol) + chcells)
	{
	    // fill the tailing area of current row.
	    എങ്കിൽ (*pargs->prow - pargs->offsetrow >= 0
		    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
		screen_fill_tailing_area(tplmode,
			*pargs->prow - pargs->offsetrow,
			*pargs->prow - pargs->offsetrow + 1,
			*pargs->pcol, pargs->col_end, attr);
	    *pargs->pcol = pargs->col_end;

	    എങ്കിൽ (pargs->col_end < chcells)
		നിർത്തുക;
	}

	എങ്കിൽ (*pargs->pcol + chcells <= pargs->col_end)
	{
	    പൂർണ്ണസംഖ്യ off = (tpl_align == ALIGN_വലത്)
		    ? topframe->fr_width
		    : 0;
	    എങ്കിൽ (tplmode == TPLMODE_REDRAW
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
    സ്ഥിതം ശൂന്യം
draw_tabpanel_default(പൂർണ്ണസംഖ്യ tplmode, tabpanel_T *pargs)
{
    പൂർണ്ണസംഖ്യ		modified;
    പൂർണ്ണസംഖ്യ		wincount;
    പൂർണ്ണസംഖ്യ		len = 0;
    char_u	buf[2] = { NUL, NUL };

    modified = FALSE;
    വേണ്ടി (wincount = 0; pargs->wp != ശൂന്യ;
	    pargs->wp = pargs->wp->w_next, ++wincount)
	എങ്കിൽ (bufIsChanged(pargs->wp->w_buffer))
	    modified = TRUE;

    എങ്കിൽ (modified || wincount > 1)
    {
	എങ്കിൽ (wincount > 1)
	{
	    vim_snprintf((അക്ഷരം *)NameBuff, MAXPATHL, "%d", wincount);
	    len = (പൂർണ്ണസംഖ്യ)STRLEN(NameBuff);
	    screen_puts_len_for_tabpanel(tplmode, NameBuff, len,
#എങ്കിൽ defined(FEAT_SYN_HL)
		    hl_combine_attr(pargs->attr, HL_ATTR(HLF_T)),
#അല്ലെങ്കിൽ
		    pargs->attr,
#endif
		    pargs);
	}
	എങ്കിൽ (modified)
	{
	    buf[0] = '+';
	    screen_puts_len_for_tabpanel(tplmode, buf, 1, pargs->attr, pargs);
	}

	buf[0] = TPL_FILLCHAR;
	screen_puts_len_for_tabpanel(tplmode, buf, 1, pargs->attr, pargs);
    }

    get_trans_bufname(pargs->cwp->w_buffer);
    shorten_dir(NameBuff);
    len = (പൂർണ്ണസംഖ്യ)STRLEN(NameBuff);
    screen_puts_len_for_tabpanel(tplmode, NameBuff, len, pargs->attr, pargs);

    // fill the tailing area of current row.
    എങ്കിൽ (*pargs->prow - pargs->offsetrow >= 0
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
    സ്ഥിതം ശൂന്യം
draw_tabpanel_with_highlight(
	പൂർണ്ണസംഖ്യ	    tplmode,
	char_u	    *buf,
	stl_hlrec_T *hltab,
	tabpanel_T  *pargs)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		curattr;
    പൂർണ്ണസംഖ്യ		n;

    curattr = pargs->attr;
    p = buf;
    വേണ്ടി (n = 0; hltab[n].start != ശൂന്യ; n++)
    {
	screen_puts_len_for_tabpanel(tplmode, p, (പൂർണ്ണസംഖ്യ)(hltab[n].start - p),
		curattr, pargs);
	p = hltab[n].start;
	എങ്കിൽ (hltab[n].userhl == 0)
	    curattr = pargs->attr;
	അല്ലെങ്കിൽ എങ്കിൽ (hltab[n].userhl < 0)
	    curattr = syn_id2attr(-hltab[n].userhl);
#ifdef FEAT_TERMINAL
	അല്ലെങ്കിൽ എങ്കിൽ (pargs->wp != ശൂന്യ && pargs->wp != curwin
		&& bt_terminal(pargs->wp->w_buffer)
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stltermnc[hltab[n].userhl - 1];
	അല്ലെങ്കിൽ എങ്കിൽ (pargs->wp != ശൂന്യ && bt_terminal(pargs->wp->w_buffer)
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stlterm[hltab[n].userhl - 1];
#endif
	അല്ലെങ്കിൽ എങ്കിൽ (pargs->wp != ശൂന്യ && pargs->wp != curwin
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stlnc[hltab[n].userhl - 1];
	അല്ലെങ്കിൽ
	    curattr = highlight_user[hltab[n].userhl - 1];
    }
    screen_puts_len_for_tabpanel(tplmode, p, (പൂർണ്ണസംഖ്യ)STRLEN(p), curattr, pargs);

    // fill the tailing area of current row.
    എങ്കിൽ (*pargs->prow - pargs->offsetrow >= 0
	    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
	screen_fill_tailing_area(tplmode, *pargs->prow - pargs->offsetrow,
		*pargs->prow - pargs->offsetrow + 1, *pargs->pcol,
		pargs->col_end, curattr);
    *pargs->pcol = pargs->col_end;
}

/*
 * do something by tplmode for drawing tabpanel.
 */
    സ്ഥിതം ശൂന്യം
do_by_tplmode(
	പൂർണ്ണസംഖ്യ	tplmode,
	പൂർണ്ണസംഖ്യ	col_start,
	പൂർണ്ണസംഖ്യ	col_end,
	പൂർണ്ണസംഖ്യ	*pcurtab_row,
	പൂർണ്ണസംഖ്യ	*ptabpagenr)
{
    പൂർണ്ണസംഖ്യ		attr_tplf = HL_ATTR(HLF_TPLF);
    പൂർണ്ണസംഖ്യ		attr_tpls = HL_ATTR(HLF_TPLS);
    പൂർണ്ണസംഖ്യ		attr_tpl = HL_ATTR(HLF_TPL);
    പൂർണ്ണസംഖ്യ		col = col_start;
    പൂർണ്ണസംഖ്യ		row = 0;
    tabpage_T	*tp = ശൂന്യ;
    typval_T	v;
    tabpanel_T	args;

    args.maxrow = Rows;
    args.offsetrow = 0;
    args.col_start = col_start;
    args.col_end = col_end;

    എങ്കിൽ (tplmode != TPLMODE_GET_CURTAB_ROW && args.maxrow > 0)
	args.offsetrow = tpl_scroll_offset;

    tp = first_tabpage;

    വേണ്ടി (row = 0; tp != ശൂന്യ; row++)
    {
	എങ്കിൽ (tplmode != TPLMODE_GET_CURTAB_ROW
		&& args.maxrow <= row - args.offsetrow)
	    നിർത്തുക;

	col = col_start;

	v.v_type = VAR_സംഖ്യ;
	v.vval.v_സംഖ്യ = tabpage_index(tp);
	set_var((char_u *)"g:actual_curtabpage", &v, TRUE);

	എങ്കിൽ (tp->tp_topframe == topframe)
	{
	    args.attr = attr_tpls;
	    എങ്കിൽ (tplmode == TPLMODE_GET_CURTAB_ROW)
		// Capture the row of the current tab and keep iterating so
		// tpl_total_rows receives the true content height below.
		*pcurtab_row = row;
	}
	അല്ലെങ്കിൽ
	    args.attr = attr_tpl;

	എങ്കിൽ (tp == curtab)
	{
	    args.cwp = curwin;
	    args.wp = firstwin;
	}
	അല്ലെങ്കിൽ
	{
	    args.cwp = tp->tp_curwin;
	    args.wp = tp->tp_firstwin;
	}

	char_u	*usefmt = vim_strsave(p_tpl);

	എങ്കിൽ (usefmt != ശൂന്യ && *usefmt != NUL)
	{
	    പൂർണ്ണസംഖ്യ	carry_hl = 0;

	    ആകുമ്പോൾ (*usefmt != NUL)
	    {
		char_u	buf[IOSIZE];
		stl_hlrec_T	*hltab;
		stl_hlrec_T	*tabtab;
		stl_clickrec_T	*clicktab = ശൂന്യ;

		എങ്കിൽ (tplmode != TPLMODE_GET_CURTAB_ROW
			&& args.maxrow <= row - args.offsetrow)
		    നിർത്തുക;

		buf[0] = NUL;
#ifdef ENABLE_STL_MODE_MULTI_NL
		(ശൂന്യം)build_stl_str_hl_mline_nl
#അല്ലെങ്കിൽ
		(ശൂന്യം)build_stl_str_hl_mline
#endif
			(args.cwp, buf, വലുപ്പം(buf),
			&usefmt, opt_name, opt_scope, TPL_FILLCHAR,
			args.col_end - args.col_start, &hltab, &tabtab,
			tplmode == TPLMODE_REDRAW ? &clicktab : ശൂന്യ,
			&carry_hl);

		args.prow = &row;
		args.pcol = &col;

		draw_tabpanel_with_highlight(tplmode, buf, hltab, &args);

		// Record any %[FuncName] click regions for this line once
		// the text has been drawn.  Only visible rows participate.
		എങ്കിൽ (tplmode == TPLMODE_REDRAW && clicktab != ശൂന്യ)
		{
		    പൂർണ്ണസംഖ്യ screen_row = row - args.offsetrow;
		    പൂർണ്ണസംഖ്യ m;

		    എങ്കിൽ (screen_row >= 0 && screen_row < args.maxrow)
			tabpanel_append_click_regions(clicktab, buf,
				screen_row, args.col_start, args.col_end,
				(പൂർണ്ണസംഖ്യ)v.vval.v_സംഖ്യ);
		    // We took ownership of the click records — free the
		    // function names (matches the non-NULL clicktab path in
		    // build_stl_str_hl()).
		    വേണ്ടി (m = 0; clicktab[m].start != ശൂന്യ; m++)
			vim_free(clicktab[m].funcname);
		}

		// Move to next line for %@
		എങ്കിൽ (*usefmt != NUL)
		{
		    row++;
		    col = col_start;
		}
	    }
	}
	അല്ലെങ്കിൽ
	{
	    args.user_defined = ശൂന്യ;
	    args.prow = &row;
	    args.pcol = &col;
	    draw_tabpanel_default(tplmode, &args);
	}

	vim_free(usefmt);
	do_unlet((char_u *)"g:actual_curtabpage", TRUE);

	tp = tp->tp_next;

	എങ്കിൽ ((tplmode == TPLMODE_GET_TABPAGENR)
		&& (mouse_row <= (row - args.offsetrow)))
	{
	    *ptabpagenr = v.vval.v_സംഖ്യ;
	    നിർത്തുക;
	}
    }

    // fill the area of TabPanelFill.
    screen_fill_tailing_area(tplmode, MAX(row - args.offsetrow, 0), args.maxrow,
	    args.col_start, args.col_end, attr_tplf);

    // Capture the true content height during the GET_CURTAB_ROW pass, which
    // ignores maxrow and therefore walks every tab.  REDRAW stops at the
    // visible edge so its "row" is clamped and unusable here.
    എങ്കിൽ (tplmode == TPLMODE_GET_CURTAB_ROW)
	tpl_total_rows = row;
}

/*
 * Draw the tabpanel scrollbar (track + thumb) at screen column 'screen_col'.
 * The scrollbar spans the full screen height.  The thumb position and size
 * are derived from tpl_scroll_offset, tpl_total_rows and Rows.
 */
    സ്ഥിതം ശൂന്യം
draw_tabpanel_scrollbar(പൂർണ്ണസംഖ്യ screen_col)
{
    പൂർണ്ണസംഖ്യ attr_sb = HL_ATTR(HLF_PSB);
    പൂർണ്ണസംഖ്യ attr_thumb = HL_ATTR(HLF_PST);
    പൂർണ്ണസംഖ്യ thumb_മുകൾ = 0;
    പൂർണ്ണസംഖ്യ thumb_height = 0;

    എങ്കിൽ (tpl_total_rows > Rows && Rows > 0)
    {
	പൂർണ്ണസംഖ്യ max_offset = tpl_total_rows - Rows;
	പൂർണ്ണസംഖ്യ track_range;

	thumb_height = Rows * Rows / tpl_total_rows;
	എങ്കിൽ (thumb_height < 1)
	    thumb_height = 1;

	// Map tpl_scroll_offset onto the track: at offset 0 the thumb's top
	// is at row 0, at the maximum offset its bottom reaches the last
	// row.  This is the exact inverse of tabpanel_drag_scrollbar().
	track_range = Rows - thumb_height;
	എങ്കിൽ (track_range > 0 && max_offset > 0)
	    thumb_മുകൾ = track_range * tpl_scroll_offset / max_offset;
	അല്ലെങ്കിൽ
	    thumb_മുകൾ = 0;
	എങ്കിൽ (thumb_മുകൾ + thumb_height > Rows)
	    thumb_മുകൾ = Rows - thumb_height;
	എങ്കിൽ (thumb_മുകൾ < 0)
	    thumb_മുകൾ = 0;
    }

    വേണ്ടി (പൂർണ്ണസംഖ്യ r = 0; r < Rows; r++)
    {
	bool on_thumb = thumb_height > 0
	    && r >= thumb_മുകൾ && r < thumb_മുകൾ + thumb_height;
	screen_putchar(TPL_FILLCHAR, r, screen_col,
		on_thumb ? attr_thumb : attr_sb);
    }
}

/*
 * Return true if the mouse is currently positioned over the tabpanel area.
 */
    bool
mouse_on_tabpanel(ശൂന്യം)
{
    എങ്കിൽ (tabpanel_width() == 0)
	തിരികെ false;
    തിരികെ mouse_col < firstwin->w_wincol
	|| mouse_col >= firstwin->w_wincol + topframe->fr_width;
}

/*
 * Return true if the mouse is currently on the scrollbar column.
 * The scrollbar column is tracked by draw_tabpanel() and is -1 when the
 * scrollbar is not enabled or not yet drawn.
 */
    bool
mouse_on_tabpanel_scrollbar(ശൂന്യം)
{
    തിരികെ tpl_scrollbar && tpl_scrollbar_col >= 0
	&& mouse_col == tpl_scrollbar_col;
}

/*
 * Move the scrollbar thumb so it is vertically centred on screen row
 * 'screen_row', updating tpl_scroll_offset accordingly.  Used for both
 * initial clicks and subsequent drag events.
 * Returns true if the event was consumed (offset changed or not).
 */
    bool
tabpanel_drag_scrollbar(പൂർണ്ണസംഖ്യ screen_row)
{
    പൂർണ്ണസംഖ്യ thumb_height;
    പൂർണ്ണസംഖ്യ max_offset;
    പൂർണ്ണസംഖ്യ track_range;
    പൂർണ്ണസംഖ്യ thumb_മുകൾ;
    പൂർണ്ണസംഖ്യ new_offset;

    എങ്കിൽ (!tpl_scrollbar || Rows <= 0 || tpl_total_rows <= Rows)
	തിരികെ false;

    thumb_height = Rows * Rows / tpl_total_rows;
    എങ്കിൽ (thumb_height < 1)
	thumb_height = 1;
    track_range = Rows - thumb_height;
    എങ്കിൽ (track_range <= 0)
	തിരികെ true;

    max_offset = tpl_total_rows - Rows;
    thumb_മുകൾ = screen_row - thumb_height / 2;
    എങ്കിൽ (thumb_മുകൾ < 0)
	thumb_മുകൾ = 0;
    എങ്കിൽ (thumb_മുകൾ > track_range)
	thumb_മുകൾ = track_range;

    new_offset = thumb_മുകൾ * max_offset / track_range;
    എങ്കിൽ (new_offset != tpl_scroll_offset)
    {
	tpl_scroll_offset = new_offset;
	redraw_tabpanel = TRUE;
    }
    തിരികെ true;
}

/*
 * Scroll the tabpanel by 'count' rows in direction 'dir' (1 = down, -1 = up).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_scroll(പൂർണ്ണസംഖ്യ dir, പൂർണ്ണസംഖ്യ എണ്ണം)
{
    പൂർണ്ണസംഖ്യ max_offset;
    പൂർണ്ണസംഖ്യ new_offset;

    എങ്കിൽ (tabpanel_width() == 0)
	തിരികെ false;

    max_offset = tpl_total_rows - Rows;
    എങ്കിൽ (max_offset < 0)
	max_offset = 0;

    new_offset = tpl_scroll_offset + (dir > 0 ? എണ്ണം : -എണ്ണം);
    എങ്കിൽ (new_offset < 0)
	new_offset = 0;
    എങ്കിൽ (new_offset > max_offset)
	new_offset = max_offset;
    എങ്കിൽ (new_offset == tpl_scroll_offset)
	തിരികെ false;

    tpl_scroll_offset = new_offset;
    redraw_tabpanel = TRUE;
    തിരികെ true;
}

/*
 * Set the tabpanel scroll offset to "offset" (clamped to the valid range).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_set_offset(പൂർണ്ണസംഖ്യ offset)
{
    പൂർണ്ണസംഖ്യ max_offset;

    എങ്കിൽ (tabpanel_width() == 0)
	തിരികെ false;

    max_offset = tpl_total_rows - Rows;
    എങ്കിൽ (max_offset < 0)
	max_offset = 0;

    എങ്കിൽ (offset < 0)
	offset = 0;
    എങ്കിൽ (offset > max_offset)
	offset = max_offset;
    എങ്കിൽ (offset == tpl_scroll_offset)
	തിരികെ false;

    tpl_scroll_offset = offset;
    redraw_tabpanel = TRUE;
    തിരികെ true;
}

/*
 * "tabpanel_getinfo()" function
 */
    ശൂന്യം
f_tabpanel_getinfo(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    പൂർണ്ണസംഖ്യ		max_offset;

    എങ്കിൽ (rettv_dict_alloc(rettv) == FAIL)
	തിരികെ;
    d = rettv->vval.v_dict;

    max_offset = tpl_total_rows - Rows;
    എങ്കിൽ (max_offset < 0)
	max_offset = 0;

    dict_add_string(d, "align",
	    (char_u *)(tpl_align == ALIGN_വലത് ? "right" : "left"));
    dict_add_സംഖ്യ(d, "columns", tabpanel_width());
    dict_add_bool(d, "scrollbar", tpl_scrollbar);
    dict_add_സംഖ്യ(d, "offset", tpl_scroll_offset);
    dict_add_സംഖ്യ(d, "total", tpl_total_rows);
    dict_add_സംഖ്യ(d, "max_offset", max_offset);
}

/*
 * "tabpanel_scroll()" function
 */
    ശൂന്യം
f_tabpanel_scroll(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	n;
    പൂർണ്ണസംഖ്യ		absolute = 0;
    bool	changed;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_സംഖ്യ = VVAL_FALSE;

    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	തിരികെ;

    n = tv_get_സംഖ്യ_chk(&argvars[0], ശൂന്യ);
    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
    {
	എങ്കിൽ (argvars[1].v_type != VAR_DICT || argvars[1].vval.v_dict == ശൂന്യ)
	{
	    emsg(_(e_dictionary_required));
	    തിരികെ;
	}
	absolute = dict_get_bool(argvars[1].vval.v_dict, "absolute", FALSE);
    }

    // Clamp to int range to avoid signed overflow when casting and negating.
    എങ്കിൽ (n > INT_MAX)
	n = INT_MAX;
    അല്ലെങ്കിൽ എങ്കിൽ (n < -INT_MAX)
	n = -INT_MAX;

    എങ്കിൽ (absolute)
	changed = tabpanel_set_offset((പൂർണ്ണസംഖ്യ)n);
    അല്ലെങ്കിൽ
	changed = tabpanel_scroll(n >= 0 ? 1 : -1,
				  (പൂർണ്ണസംഖ്യ)(n >= 0 ? n : -n));

    rettv->vval.v_സംഖ്യ = changed ? VVAL_TRUE : VVAL_FALSE;
}

#endif // FEAT_TABPANEL
