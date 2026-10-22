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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_TABPANEL)

நிலையான வெறுமை do_by_tplmode(முழு tplmode, முழு col_start, முழு col_end,
	முழு *pcurtab_row, முழு *ptabpagenr);
நிலையான வெறுமை tabpanel_free_click_regions(வெறுமை);
நிலையான வெறுமை tabpanel_append_click_regions(stl_clickrec_T *clicktab,
	char_u *buf, முழு row, முழு col_start, முழு col_end, முழு tabnr);
நிலையான வெறுமை draw_tabpanel_scrollbar(முழு screen_col);

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
#define ALIGN_இடது		0
#define ALIGN_வலது		1

நிலையான char_u *opt_name = (char_u *)"tabpanel";
நிலையான முழு opt_scope = OPT_LOCAL;
நிலையான முழு tpl_align = ALIGN_இடது;
நிலையான முழு tpl_columns = 20;
நிலையான bool tpl_is_vert = false;
நிலையான bool tpl_scrollbar = false;
நிலையான முழு tpl_scroll_offset = 0;
நிலையான முழு tpl_total_rows = 0;
நிலையான முழு tpl_scrollbar_col = -1;  // screen column of scrollbar, -1 if none
நிலையான tabpage_T *tpl_last_curtab = வெற்று;   // last curtab seen by draw_tabpanel

வகைவரை கட்டமைப்பு {
    win_T   *wp;
    win_T   *cwp;
    char_u  *user_defined;
    முழு	maxrow;
    முழு	offsetrow;
    முழு	*prow;
    முழு	*pcol;
    முழு	attr;
    முழு	col_start;
    முழு	col_end;
} tabpanel_T;

    முழு
tabpanelopt_changed(வெறுமை)
{
    char_u	*p;
    முழு		new_align = ALIGN_இடது;
    long	new_columns = 20;
    bool	new_is_vert = false;
    bool	new_scrollbar = false;

    p = p_tplo;
    போது (*p != NUL)
    {
	எனில் (STRNCMP(p, "align:", 6) == 0)
	{
	    p += 6;
	    எனில் (STRNCMP(p, "left", 4) == 0)
	    {
		p += 4;
		new_align = ALIGN_இடது;
	    }
	    இல்லையெனில் எனில் (STRNCMP(p, "right", 5) == 0)
	    {
		p += 5;
		new_align = ALIGN_வலது;
	    }
	    இல்லையெனில்
		திரும்பு FAIL;
	}
	இல்லையெனில் எனில் (STRNCMP(p, "columns:", 8) == 0 && VIM_ISDIGIT(p[8]))
	{
	    p += 8;
	    new_columns = getdigits(&p);
	    எனில் (new_columns < 0 || new_columns > 1000)
		திரும்பு FAIL;
	}
	இல்லையெனில் எனில் (STRNCMP(p, "vert", 4) == 0)
	{
	    p += 4;
	    new_is_vert = true;
	}
	இல்லையெனில் எனில் (STRNCMP(p, "scrollbar", 9) == 0)
	{
	    p += 9;
	    new_scrollbar = true;
	}

	எனில் (*p != ',' && *p != NUL)
	    திரும்பு FAIL;
	எனில் (*p == ',')
	    ++p;
    }

    tpl_align = new_align;
    tpl_columns = new_columns;
    tpl_is_vert = new_is_vert;
    tpl_scrollbar = new_scrollbar;

    // Re-center the current tab on the next redraw.
    tpl_last_curtab = வெற்று;

    shell_new_columns();
    திரும்பு OK;
}

/*
 * Drop any internal reference to "tp", so draw_tabpanel() never compares
 * against a dangling pointer after the tabpage has been freed.
 */
    வெறுமை
tabpanel_forget_tabpage(மாறிலி tabpage_T *tp)
{
    எனில் (tpl_last_curtab == tp)
	tpl_last_curtab = வெற்று;
}

/*
 * Return the width of tabpanel.
 */
    முழு
tabpanel_width(வெறுமை)
{
    தேர்வு (p_stpl)
    {
	நிலை 0:
	    திரும்பு 0;
	நிலை 1:
	    எனில் (first_tabpage->tp_next == வெற்று)
		திரும்பு 0;
    }
    எனில் (Columns < tpl_columns)
	திரும்பு 0;
    இல்லையெனில்
	திரும்பு tpl_columns;
}

/*
 * Return the offset of a window considering the width of tabpanel.
 */
    முழு
tabpanel_leftcol(வெறுமை)
{
    திரும்பு tpl_align == ALIGN_வலது ? 0 : tabpanel_width();
}

/*
 * Free previously resolved 'tabpanel' click regions.
 */
    நிலையான வெறுமை
tabpanel_free_click_regions(வெறுமை)
{
    முழு n;

    எனில் (tabpanel_stl_click != வெற்று)
    {
	க்கு (n = 0; n < tabpanel_stl_click_மொத்தம்; n++)
	    vim_free(tabpanel_stl_click[n].funcname);
	VIM_CLEAR(tabpanel_stl_click);
    }
    tabpanel_stl_click_மொத்தம் = 0;
}

/*
 * Convert click records produced by build_stl_str_hl() for one line of
 * 'tabpanel' into screen-column based regions and append them to the global
 * tabpanel_stl_click array.  The caller keeps ownership of the funcname
 * strings inside "clicktab" — this function makes its own copies.
 */
    நிலையான வெறுமை
tabpanel_append_click_regions(
	stl_clickrec_T	*clicktab,
	char_u		*buf,
	முழு		row,
	முழு		col_start,
	முழு		col_end,
	முழு		tabnr)
{
    முழு		மொத்தம் = 0;
    முழு		n;
    முழு		base_col;
    முழு		acc_width = 0;
    முழு		max_w = col_end - col_start;
    char_u	*p;
    char_u	*cur_funcname = வெற்று;
    முழு		cur_minwid = 0;
    முழு		region_start_col;
    stl_click_region_T *new_arr;
    முழு		limit;

    எனில் (clicktab == வெற்று)
	திரும்பு;

    க்கு (n = 0; clicktab[n].start != வெற்று; n++)
	மொத்தம்++;
    எனில் (மொத்தம் == 0)
	திரும்பு;

    base_col = (tpl_align == ALIGN_வலது ? topframe->fr_width : 0) + col_start;
    region_start_col = base_col;

    // Grow the global array to make room for up to "count" more regions
    // (one close for each record plus a possible trailing region).
    new_arr = vim_realloc(tabpanel_stl_click,
	    அளவு(stl_click_region_T) * (tabpanel_stl_click_மொத்தம் + மொத்தம் + 1));
    எனில் (new_arr == வெற்று)
	திரும்பு;
    tabpanel_stl_click = new_arr;

    p = buf;
    க்கு (n = 0; clicktab[n].start != வெற்று; n++)
    {
	acc_width += vim_strnsize(p, (முழு)(clicktab[n].start - p));
	p = clicktab[n].start;
	limit = acc_width < max_w ? acc_width : max_w;

	எனில் (cur_funcname != வெற்று)
	{
	    stl_click_region_T *r =
				&tabpanel_stl_click[tabpanel_stl_click_மொத்தம்];
	    r->row = row;
	    r->col_start = region_start_col;
	    r->col_end = base_col + limit;
	    r->funcname = vim_strsave(cur_funcname);
	    r->minwid = cur_minwid;
	    r->tabnr = tabnr;
	    tabpanel_stl_click_மொத்தம்++;
	}

	cur_funcname = clicktab[n].funcname;
	cur_minwid = clicktab[n].minwid;
	region_start_col = base_col + limit;
    }

    // Close the final region if it extends to the end.
    எனில் (cur_funcname != வெற்று)
    {
	stl_click_region_T *r = &tabpanel_stl_click[tabpanel_stl_click_மொத்தம்];
	r->row = row;
	r->col_start = region_start_col;
	r->col_end = base_col + max_w;
	r->funcname = vim_strsave(cur_funcname);
	r->minwid = cur_minwid;
	r->tabnr = tabnr;
	tabpanel_stl_click_மொத்தம்++;
    }
}

/*
 * Ensure the current tab is visible by adjusting tpl_scroll_offset when
 * the selected tab has changed since the previous redraw.  Mouse wheel or
 * scrollbar drag operations leave curtab unchanged, so the user's chosen
 * offset is preserved in those cases.
 */
    நிலையான வெறுமை
follow_curtab_if_needed(முழு curtab_row)
{
    எனில் (Rows <= 0 || curtab == tpl_last_curtab)
	திரும்பு;

    எனில் (curtab_row < tpl_scroll_offset)
	tpl_scroll_offset = curtab_row;
    இல்லையெனில் எனில் (curtab_row >= tpl_scroll_offset + Rows)
	tpl_scroll_offset = curtab_row - Rows + 1;

    முழு max_offset = tpl_total_rows > Rows ? tpl_total_rows - Rows : 0;

    எனில் (tpl_scroll_offset < 0)
	tpl_scroll_offset = 0;
    இல்லையெனில் எனில் (tpl_scroll_offset > max_offset)
	tpl_scroll_offset = max_offset;
}

/*
 * draw the tabpanel.
 */
    வெறுமை
draw_tabpanel(வெறுமை)
{
    முழு saved_சாவி_Typed = சாவி_Typed;
    முழு saved_got_int = got_int;
    முழு maxwidth = tabpanel_width();
    முழு vs_attr = HL_ATTR(HLF_C);
    முழு curtab_row = 0;
    முழு vsrow = 0;
    முழு is_வலது = tpl_align == ALIGN_வலது;

    எனில் (maxwidth == 0)
    {
	tabpanel_free_click_regions();
	திரும்பு;
    }

    // Discard old click regions — they'll be rebuilt during redraw below.
    tabpanel_free_click_regions();

    // Reset got_int to avoid build_stl_str_hl() isn't evaluated.
    got_int = FALSE;

    முழு sb_len = tpl_scrollbar ? SCROLL_LEN : 0;
    முழு sb_screen_col = -1;

    // The scrollbar is always placed at the right edge of the tabpanel,
    // regardless of 'align'.  The vertical separator sits at the panel's
    // boundary with the buffer area (left edge for align:right, right edge
    // for align:left).
    எனில் (tpl_is_vert)
    {
	எனில் (is_வலது)
	{
	    // Panel on the right: vert at panel's left edge, scrollbar at
	    // panel's right edge (= screen's right edge).
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, VERT_LEN,
		    maxwidth - sb_len, &curtab_row, வெற்று);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, VERT_LEN, maxwidth - sb_len,
		    &curtab_row, வெற்று);
	    க்கு (vsrow = 0; vsrow < Rows; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			topframe->fr_width, vs_attr);
	    எனில் (tpl_scrollbar)
		sb_screen_col = topframe->fr_width + maxwidth - SCROLL_LEN;
	}
	இல்லையெனில்
	{
	    // Panel on the left: scrollbar just left of vert, vert at
	    // panel's right edge (boundary with buffer).
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0,
		    maxwidth - VERT_LEN - sb_len, &curtab_row, வெற்று);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - VERT_LEN - sb_len,
		    &curtab_row, வெற்று);
	    க்கு (vsrow = 0; vsrow < Rows; vsrow++)
		screen_putchar(curwin->w_fill_chars.tpl_vert, vsrow,
			maxwidth - VERT_LEN, vs_attr);
	    எனில் (tpl_scrollbar)
		sb_screen_col = maxwidth - VERT_LEN - SCROLL_LEN;
	}
    }
    இல்லையெனில்
    {
	எனில் (is_வலது)
	{
	    // Panel on the right, no vert: scrollbar at screen's right edge.
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth - sb_len,
		    &curtab_row, வெற்று);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_row, வெற்று);
	    எனில் (tpl_scrollbar)
		sb_screen_col = topframe->fr_width + maxwidth - SCROLL_LEN;
	}
	இல்லையெனில்
	{
	    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth - sb_len,
		    &curtab_row, வெற்று);
	    follow_curtab_if_needed(curtab_row);
	    do_by_tplmode(TPLMODE_REDRAW, 0, maxwidth - sb_len,
		    &curtab_row, வெற்று);
	    எனில் (tpl_scrollbar)
		sb_screen_col = maxwidth - SCROLL_LEN;
	}
    }

    tpl_scrollbar_col = sb_screen_col;
    எனில் (sb_screen_col >= 0)
	draw_tabpanel_scrollbar(sb_screen_col);

    got_int |= saved_got_int;

    // A user function may reset KeyTyped, restore it.
    சாவி_Typed = saved_சாவி_Typed;

    tpl_last_curtab = curtab;
    redraw_tabpanel = FALSE;
}

/*
 * Return tabpagenr when clicking and dragging in tabpanel.
 */
    முழு
get_tabpagenr_on_tabpanel(வெறுமை)
{
    முழு		maxwidth = tabpanel_width();
    முழு		curtab_row = 0;
    முழு		tabpagenr = 0;

    எனில் (maxwidth == 0)
	திரும்பு -1;

    do_by_tplmode(TPLMODE_GET_CURTAB_ROW, 0, maxwidth, &curtab_row, வெற்று);
    do_by_tplmode(TPLMODE_GET_TABPAGENR, 0, maxwidth, &curtab_row,
	    &tabpagenr);

    திரும்பு tabpagenr;
}

/*
 * Fill tailing area between {start_row} and {end_row - 1}.
 */
    நிலையான வெறுமை
screen_fill_tailing_area(
	முழு	tplmode,
	முழு	row_start,
	முழு	row_end,
	முழு	col_start,
	முழு	col_end,
	முழு	attr)
{
    முழு is_வலது = tpl_align == ALIGN_வலது;
    எனில் (tplmode == TPLMODE_REDRAW)
	screen_fill(row_start, row_end,
		(is_வலது ? topframe->fr_width : 0) + col_start,
		(is_வலது ? topframe->fr_width : 0) + col_end,
		TPL_FILLCHAR, TPL_FILLCHAR, attr);
}

/*
 * screen_puts_len() for tabpanel.
 */
    நிலையான வெறுமை
screen_puts_len_for_tabpanel(
	முழு	    tplmode,
	char_u	    *p,
	முழு	    len,
	முழு	    attr,
	tabpanel_T  *pargs)
{
    முழு		j;
    முழு		chlen;
    முழு		chcells;
    char_u	buf[IOSIZE];
    char_u	*temp;

    க்கு (j = 0; j < len;)
    {
	எனில் (tplmode != TPLMODE_GET_CURTAB_ROW
		&& pargs->maxrow <= *pargs->prow - pargs->offsetrow)
	    நிறுத்து;

	எனில் (has_mbyte)
	    chlen = (*mb_ptr2len)(p + j);
	இல்லையெனில்
	    chlen = 1;

	க்கு (முழு k = 0; k < chlen; k++)
	    buf[k] = p[j + k];
	buf[chlen] = NUL;
	j += chlen;

	// Make all characters printable.
	temp = transstr(buf);
	எனில் (temp != வெற்று)
	{
	    vim_strncpy(buf, temp, அளவு(buf) - 1);
	    vim_free(temp);
	}

	எனில் (has_mbyte)
	    chcells = (*mb_ptr2cells)(buf);
	இல்லையெனில்
	    chcells = 1;

	எனில் (pargs->col_end < (*pargs->pcol) + chcells)
	{
	    // fill the tailing area of current row.
	    எனில் (*pargs->prow - pargs->offsetrow >= 0
		    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
		screen_fill_tailing_area(tplmode,
			*pargs->prow - pargs->offsetrow,
			*pargs->prow - pargs->offsetrow + 1,
			*pargs->pcol, pargs->col_end, attr);
	    *pargs->pcol = pargs->col_end;

	    எனில் (pargs->col_end < chcells)
		நிறுத்து;
	}

	எனில் (*pargs->pcol + chcells <= pargs->col_end)
	{
	    முழு off = (tpl_align == ALIGN_வலது)
		    ? topframe->fr_width
		    : 0;
	    எனில் (tplmode == TPLMODE_REDRAW
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
    நிலையான வெறுமை
draw_tabpanel_default(முழு tplmode, tabpanel_T *pargs)
{
    முழு		modified;
    முழு		wincount;
    முழு		len = 0;
    char_u	buf[2] = { NUL, NUL };

    modified = FALSE;
    க்கு (wincount = 0; pargs->wp != வெற்று;
	    pargs->wp = pargs->wp->w_next, ++wincount)
	எனில் (bufIsChanged(pargs->wp->w_buffer))
	    modified = TRUE;

    எனில் (modified || wincount > 1)
    {
	எனில் (wincount > 1)
	{
	    vim_snprintf((எழுத்து *)NameBuff, MAXPATHL, "%d", wincount);
	    len = (முழு)STRLEN(NameBuff);
	    screen_puts_len_for_tabpanel(tplmode, NameBuff, len,
#எனில் defined(FEAT_SYN_HL)
		    hl_combine_attr(pargs->attr, HL_ATTR(HLF_T)),
#இல்லையெனில்
		    pargs->attr,
#endif
		    pargs);
	}
	எனில் (modified)
	{
	    buf[0] = '+';
	    screen_puts_len_for_tabpanel(tplmode, buf, 1, pargs->attr, pargs);
	}

	buf[0] = TPL_FILLCHAR;
	screen_puts_len_for_tabpanel(tplmode, buf, 1, pargs->attr, pargs);
    }

    get_trans_bufname(pargs->cwp->w_buffer);
    shorten_dir(NameBuff);
    len = (முழு)STRLEN(NameBuff);
    screen_puts_len_for_tabpanel(tplmode, NameBuff, len, pargs->attr, pargs);

    // fill the tailing area of current row.
    எனில் (*pargs->prow - pargs->offsetrow >= 0
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
    நிலையான வெறுமை
draw_tabpanel_with_highlight(
	முழு	    tplmode,
	char_u	    *buf,
	stl_hlrec_T *hltab,
	tabpanel_T  *pargs)
{
    char_u	*p;
    முழு		curattr;
    முழு		n;

    curattr = pargs->attr;
    p = buf;
    க்கு (n = 0; hltab[n].start != வெற்று; n++)
    {
	screen_puts_len_for_tabpanel(tplmode, p, (முழு)(hltab[n].start - p),
		curattr, pargs);
	p = hltab[n].start;
	எனில் (hltab[n].userhl == 0)
	    curattr = pargs->attr;
	இல்லையெனில் எனில் (hltab[n].userhl < 0)
	    curattr = syn_id2attr(-hltab[n].userhl);
#ifdef FEAT_TERMINAL
	இல்லையெனில் எனில் (pargs->wp != வெற்று && pargs->wp != curwin
		&& bt_terminal(pargs->wp->w_buffer)
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stltermnc[hltab[n].userhl - 1];
	இல்லையெனில் எனில் (pargs->wp != வெற்று && bt_terminal(pargs->wp->w_buffer)
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stlterm[hltab[n].userhl - 1];
#endif
	இல்லையெனில் எனில் (pargs->wp != வெற்று && pargs->wp != curwin
		&& pargs->wp->w_status_height != 0)
	    curattr = highlight_stlnc[hltab[n].userhl - 1];
	இல்லையெனில்
	    curattr = highlight_user[hltab[n].userhl - 1];
    }
    screen_puts_len_for_tabpanel(tplmode, p, (முழு)STRLEN(p), curattr, pargs);

    // fill the tailing area of current row.
    எனில் (*pargs->prow - pargs->offsetrow >= 0
	    && *pargs->prow - pargs->offsetrow < pargs->maxrow)
	screen_fill_tailing_area(tplmode, *pargs->prow - pargs->offsetrow,
		*pargs->prow - pargs->offsetrow + 1, *pargs->pcol,
		pargs->col_end, curattr);
    *pargs->pcol = pargs->col_end;
}

/*
 * do something by tplmode for drawing tabpanel.
 */
    நிலையான வெறுமை
do_by_tplmode(
	முழு	tplmode,
	முழு	col_start,
	முழு	col_end,
	முழு	*pcurtab_row,
	முழு	*ptabpagenr)
{
    முழு		attr_tplf = HL_ATTR(HLF_TPLF);
    முழு		attr_tpls = HL_ATTR(HLF_TPLS);
    முழு		attr_tpl = HL_ATTR(HLF_TPL);
    முழு		col = col_start;
    முழு		row = 0;
    tabpage_T	*tp = வெற்று;
    typval_T	v;
    tabpanel_T	args;

    args.maxrow = Rows;
    args.offsetrow = 0;
    args.col_start = col_start;
    args.col_end = col_end;

    எனில் (tplmode != TPLMODE_GET_CURTAB_ROW && args.maxrow > 0)
	args.offsetrow = tpl_scroll_offset;

    tp = first_tabpage;

    க்கு (row = 0; tp != வெற்று; row++)
    {
	எனில் (tplmode != TPLMODE_GET_CURTAB_ROW
		&& args.maxrow <= row - args.offsetrow)
	    நிறுத்து;

	col = col_start;

	v.v_type = VAR_எண்;
	v.vval.v_எண் = tabpage_index(tp);
	set_var((char_u *)"g:actual_curtabpage", &v, TRUE);

	எனில் (tp->tp_topframe == topframe)
	{
	    args.attr = attr_tpls;
	    எனில் (tplmode == TPLMODE_GET_CURTAB_ROW)
		// Capture the row of the current tab and keep iterating so
		// tpl_total_rows receives the true content height below.
		*pcurtab_row = row;
	}
	இல்லையெனில்
	    args.attr = attr_tpl;

	எனில் (tp == curtab)
	{
	    args.cwp = curwin;
	    args.wp = firstwin;
	}
	இல்லையெனில்
	{
	    args.cwp = tp->tp_curwin;
	    args.wp = tp->tp_firstwin;
	}

	char_u	*usefmt = vim_strsave(p_tpl);

	எனில் (usefmt != வெற்று && *usefmt != NUL)
	{
	    முழு	carry_hl = 0;

	    போது (*usefmt != NUL)
	    {
		char_u	buf[IOSIZE];
		stl_hlrec_T	*hltab;
		stl_hlrec_T	*tabtab;
		stl_clickrec_T	*clicktab = வெற்று;

		எனில் (tplmode != TPLMODE_GET_CURTAB_ROW
			&& args.maxrow <= row - args.offsetrow)
		    நிறுத்து;

		buf[0] = NUL;
#ifdef ENABLE_STL_MODE_MULTI_NL
		(வெறுமை)build_stl_str_hl_mline_nl
#இல்லையெனில்
		(வெறுமை)build_stl_str_hl_mline
#endif
			(args.cwp, buf, அளவு(buf),
			&usefmt, opt_name, opt_scope, TPL_FILLCHAR,
			args.col_end - args.col_start, &hltab, &tabtab,
			tplmode == TPLMODE_REDRAW ? &clicktab : வெற்று,
			&carry_hl);

		args.prow = &row;
		args.pcol = &col;

		draw_tabpanel_with_highlight(tplmode, buf, hltab, &args);

		// Record any %[FuncName] click regions for this line once
		// the text has been drawn.  Only visible rows participate.
		எனில் (tplmode == TPLMODE_REDRAW && clicktab != வெற்று)
		{
		    முழு screen_row = row - args.offsetrow;
		    முழு m;

		    எனில் (screen_row >= 0 && screen_row < args.maxrow)
			tabpanel_append_click_regions(clicktab, buf,
				screen_row, args.col_start, args.col_end,
				(முழு)v.vval.v_எண்);
		    // We took ownership of the click records — free the
		    // function names (matches the non-NULL clicktab path in
		    // build_stl_str_hl()).
		    க்கு (m = 0; clicktab[m].start != வெற்று; m++)
			vim_free(clicktab[m].funcname);
		}

		// Move to next line for %@
		எனில் (*usefmt != NUL)
		{
		    row++;
		    col = col_start;
		}
	    }
	}
	இல்லையெனில்
	{
	    args.user_defined = வெற்று;
	    args.prow = &row;
	    args.pcol = &col;
	    draw_tabpanel_default(tplmode, &args);
	}

	vim_free(usefmt);
	do_unlet((char_u *)"g:actual_curtabpage", TRUE);

	tp = tp->tp_next;

	எனில் ((tplmode == TPLMODE_GET_TABPAGENR)
		&& (mouse_row <= (row - args.offsetrow)))
	{
	    *ptabpagenr = v.vval.v_எண்;
	    நிறுத்து;
	}
    }

    // fill the area of TabPanelFill.
    screen_fill_tailing_area(tplmode, MAX(row - args.offsetrow, 0), args.maxrow,
	    args.col_start, args.col_end, attr_tplf);

    // Capture the true content height during the GET_CURTAB_ROW pass, which
    // ignores maxrow and therefore walks every tab.  REDRAW stops at the
    // visible edge so its "row" is clamped and unusable here.
    எனில் (tplmode == TPLMODE_GET_CURTAB_ROW)
	tpl_total_rows = row;
}

/*
 * Draw the tabpanel scrollbar (track + thumb) at screen column 'screen_col'.
 * The scrollbar spans the full screen height.  The thumb position and size
 * are derived from tpl_scroll_offset, tpl_total_rows and Rows.
 */
    நிலையான வெறுமை
draw_tabpanel_scrollbar(முழு screen_col)
{
    முழு attr_sb = HL_ATTR(HLF_PSB);
    முழு attr_thumb = HL_ATTR(HLF_PST);
    முழு thumb_உச்சி = 0;
    முழு thumb_height = 0;

    எனில் (tpl_total_rows > Rows && Rows > 0)
    {
	முழு max_offset = tpl_total_rows - Rows;
	முழு track_range;

	thumb_height = Rows * Rows / tpl_total_rows;
	எனில் (thumb_height < 1)
	    thumb_height = 1;

	// Map tpl_scroll_offset onto the track: at offset 0 the thumb's top
	// is at row 0, at the maximum offset its bottom reaches the last
	// row.  This is the exact inverse of tabpanel_drag_scrollbar().
	track_range = Rows - thumb_height;
	எனில் (track_range > 0 && max_offset > 0)
	    thumb_உச்சி = track_range * tpl_scroll_offset / max_offset;
	இல்லையெனில்
	    thumb_உச்சி = 0;
	எனில் (thumb_உச்சி + thumb_height > Rows)
	    thumb_உச்சி = Rows - thumb_height;
	எனில் (thumb_உச்சி < 0)
	    thumb_உச்சி = 0;
    }

    க்கு (முழு r = 0; r < Rows; r++)
    {
	bool on_thumb = thumb_height > 0
	    && r >= thumb_உச்சி && r < thumb_உச்சி + thumb_height;
	screen_putchar(TPL_FILLCHAR, r, screen_col,
		on_thumb ? attr_thumb : attr_sb);
    }
}

/*
 * Return true if the mouse is currently positioned over the tabpanel area.
 */
    bool
mouse_on_tabpanel(வெறுமை)
{
    எனில் (tabpanel_width() == 0)
	திரும்பு false;
    திரும்பு mouse_col < firstwin->w_wincol
	|| mouse_col >= firstwin->w_wincol + topframe->fr_width;
}

/*
 * Return true if the mouse is currently on the scrollbar column.
 * The scrollbar column is tracked by draw_tabpanel() and is -1 when the
 * scrollbar is not enabled or not yet drawn.
 */
    bool
mouse_on_tabpanel_scrollbar(வெறுமை)
{
    திரும்பு tpl_scrollbar && tpl_scrollbar_col >= 0
	&& mouse_col == tpl_scrollbar_col;
}

/*
 * Move the scrollbar thumb so it is vertically centred on screen row
 * 'screen_row', updating tpl_scroll_offset accordingly.  Used for both
 * initial clicks and subsequent drag events.
 * Returns true if the event was consumed (offset changed or not).
 */
    bool
tabpanel_drag_scrollbar(முழு screen_row)
{
    முழு thumb_height;
    முழு max_offset;
    முழு track_range;
    முழு thumb_உச்சி;
    முழு new_offset;

    எனில் (!tpl_scrollbar || Rows <= 0 || tpl_total_rows <= Rows)
	திரும்பு false;

    thumb_height = Rows * Rows / tpl_total_rows;
    எனில் (thumb_height < 1)
	thumb_height = 1;
    track_range = Rows - thumb_height;
    எனில் (track_range <= 0)
	திரும்பு true;

    max_offset = tpl_total_rows - Rows;
    thumb_உச்சி = screen_row - thumb_height / 2;
    எனில் (thumb_உச்சி < 0)
	thumb_உச்சி = 0;
    எனில் (thumb_உச்சி > track_range)
	thumb_உச்சி = track_range;

    new_offset = thumb_உச்சி * max_offset / track_range;
    எனில் (new_offset != tpl_scroll_offset)
    {
	tpl_scroll_offset = new_offset;
	redraw_tabpanel = TRUE;
    }
    திரும்பு true;
}

/*
 * Scroll the tabpanel by 'count' rows in direction 'dir' (1 = down, -1 = up).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_scroll(முழு dir, முழு மொத்தம்)
{
    முழு max_offset;
    முழு new_offset;

    எனில் (tabpanel_width() == 0)
	திரும்பு false;

    max_offset = tpl_total_rows - Rows;
    எனில் (max_offset < 0)
	max_offset = 0;

    new_offset = tpl_scroll_offset + (dir > 0 ? மொத்தம் : -மொத்தம்);
    எனில் (new_offset < 0)
	new_offset = 0;
    எனில் (new_offset > max_offset)
	new_offset = max_offset;
    எனில் (new_offset == tpl_scroll_offset)
	திரும்பு false;

    tpl_scroll_offset = new_offset;
    redraw_tabpanel = TRUE;
    திரும்பு true;
}

/*
 * Set the tabpanel scroll offset to "offset" (clamped to the valid range).
 * Returns true if the offset changed and a redraw was scheduled.
 */
    bool
tabpanel_set_offset(முழு offset)
{
    முழு max_offset;

    எனில் (tabpanel_width() == 0)
	திரும்பு false;

    max_offset = tpl_total_rows - Rows;
    எனில் (max_offset < 0)
	max_offset = 0;

    எனில் (offset < 0)
	offset = 0;
    எனில் (offset > max_offset)
	offset = max_offset;
    எனில் (offset == tpl_scroll_offset)
	திரும்பு false;

    tpl_scroll_offset = offset;
    redraw_tabpanel = TRUE;
    திரும்பு true;
}

/*
 * "tabpanel_getinfo()" function
 */
    வெறுமை
f_tabpanel_getinfo(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    முழு		max_offset;

    எனில் (rettv_dict_alloc(rettv) == FAIL)
	திரும்பு;
    d = rettv->vval.v_dict;

    max_offset = tpl_total_rows - Rows;
    எனில் (max_offset < 0)
	max_offset = 0;

    dict_add_string(d, "align",
	    (char_u *)(tpl_align == ALIGN_வலது ? "right" : "left"));
    dict_add_எண்(d, "columns", tabpanel_width());
    dict_add_bool(d, "scrollbar", tpl_scrollbar);
    dict_add_எண்(d, "offset", tpl_scroll_offset);
    dict_add_எண்(d, "total", tpl_total_rows);
    dict_add_எண்(d, "max_offset", max_offset);
}

/*
 * "tabpanel_scroll()" function
 */
    வெறுமை
f_tabpanel_scroll(typval_T *argvars, typval_T *rettv)
{
    varnumber_T	n;
    முழு		absolute = 0;
    bool	changed;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_எண் = VVAL_FALSE;

    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	திரும்பு;

    n = tv_get_எண்_chk(&argvars[0], வெற்று);
    எனில் (argvars[1].v_type != VAR_UNKNOWN)
    {
	எனில் (argvars[1].v_type != VAR_DICT || argvars[1].vval.v_dict == வெற்று)
	{
	    emsg(_(e_dictionary_required));
	    திரும்பு;
	}
	absolute = dict_get_bool(argvars[1].vval.v_dict, "absolute", FALSE);
    }

    // Clamp to int range to avoid signed overflow when casting and negating.
    எனில் (n > INT_MAX)
	n = INT_MAX;
    இல்லையெனில் எனில் (n < -INT_MAX)
	n = -INT_MAX;

    எனில் (absolute)
	changed = tabpanel_set_offset((முழு)n);
    இல்லையெனில்
	changed = tabpanel_scroll(n >= 0 ? 1 : -1,
				  (முழு)(n >= 0 ? n : -n));

    rettv->vval.v_எண் = changed ? VVAL_TRUE : VVAL_FALSE;
}

#endif // FEAT_TABPANEL
